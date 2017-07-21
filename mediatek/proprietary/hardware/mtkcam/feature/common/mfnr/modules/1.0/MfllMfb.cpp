#include "IMfllCore.h"
#include "MfllMfb.h"
#include "MfllLog.h"

/* mtkcam related headers */
#include <hwutils/HwMisc.h>
#include <metadata/mtk_platform_metadata_tag.h>
#include <drv/isp_reg.h>

#include <iopipe/SImager/IImageTransform.h> // IImageTransform
#include <utils/Mutex.h> // android::Mutex

#include <memory>

#define LOG_TAG "MtkCam/MfllCore/Mfb"

using namespace NSCam::NSIoPipe::NSPostProc;
using namespace NSCam;
using namespace NSCam::Utils::Format;
using namespace NS3Av3;
using namespace mfll;

using android::sp;
using android::Mutex;
using NSCam::IImageBuffer;
using NSCam::NSIoPipe::EPortType_Memory;
using NSCam::NSIoPipe::NSSImager::IImageTransform;
using NSCam::NSIoPipe::NSPostProc::Input;
using NSCam::NSIoPipe::NSPostProc::Output;
using NSCam::NSIoPipe::NSPostProc::QParams;
using NSCam::NSIoPipe::PortID;

// defined in ispio_pipe_ports.h
// type(port type) index(port index) inout(0:in/1:out)
#include <imageio/ispio_pipe_ports.h>
static const PortID PORT_IMGI   ( NSIoPipe::EPortType_Memory, NSImageio::NSIspio::EPortIndex_IMGI,  0);
static const PortID PORT_IMGBI  ( NSIoPipe::EPortType_Memory, NSImageio::NSIspio::EPortIndex_IMGBI, 0);
static const PortID PORT_IMGCI  ( NSIoPipe::EPortType_Memory, NSImageio::NSIspio::EPortIndex_IMGCI, 0);
static const PortID PORT_VIPI   ( NSIoPipe::EPortType_Memory, NSImageio::NSIspio::EPortIndex_VIPI,  0);
static const PortID PORT_VIP3I  ( NSIoPipe::EPortType_Memory, NSImageio::NSIspio::EPortIndex_VIP3I, 0);
static const PortID PORT_DEPI   ( NSIoPipe::EPortType_Memory, NSImageio::NSIspio::EPortIndex_DEPI,  0);
static const PortID PORT_WDMAO  ( NSIoPipe::EPortType_Memory, NSImageio::NSIspio::EPortIndex_WDMAO, 1);
static const PortID PORT_WROTO  ( NSIoPipe::EPortType_Memory, NSImageio::NSIspio::EPortIndex_WROTO, 1);
static const PortID PORT_MFBO   ( NSIoPipe::EPortType_Memory, NSImageio::NSIspio::EPortIndex_MFBO,  1);
static const PortID PORT_IMG2O  ( NSIoPipe::EPortType_Memory, NSImageio::NSIspio::EPortIndex_IMG2O, 1);
static const PortID PORT_IMG3O  ( NSIoPipe::EPortType_Memory, NSImageio::NSIspio::EPortIndex_IMG3O, 1);

/**
 *  Describe the pass 2 port ID
 *
 *  Notice that, MCrpRsInfo::mGroupID is only works if only if input port is IMGI which describe:
 *  mGroupID = 1: Cropping of the source image
 *  mGroupID = 2: Cropping/Reszing of the first output image
 *  mGroupID = 3: Cropping/Reszing of the second output image
 *
 */
/******************************************************************************
 *  RAW2YUV
 *****************************************************************************/
#define RAW2YUV_PORT_IN             PORT_IMGI
#define RAW2YUV_PORT_OUT            PORT_WDMAO
#define RAW2YUV_PORT_OUT2           PORT_WROTO

/******************************************************************************
 *  MFB
 *****************************************************************************/
/* port */
#define MFB_PORT_IN_BASE_FRAME      PORT_IMGI
#define MFB_PORT_IN_REF_FRAME       PORT_IMGBI
#define MFB_PORT_IN_WEIGHTING       PORT_IMGCI
#define MFB_PORT_OUT_FRAME          PORT_IMG2O
#define MFB_PORT_OUT_WEIGHTING      PORT_MFBO

/******************************************************************************
 *  MIX
 *****************************************************************************/
/* port */
#define MIX_PORT_IN_BASE_FRAME          PORT_IMGI
#define MIX_PORT_IN_GOLDEN_FRAME        PORT_VIPI
#define MIX_PORT_IN_WEIGHTING           PORT_IMGBI
#define MIX_PORT_OUT_FRAME              PORT_IMG2O

/*
 * Mutex for pass 2 operation
 *
 * make sure pass 2 is thread-safe, basically it's not ...
 */
static Mutex gMutexPass2Lock;

/* For P2 driver callback usage */
#define P2CBPARAM_MAGICNUM 0xABC
typedef struct _p2cbParam {
    MfllMfb         *self;
    Mutex           *mutex;
    unsigned int    magicNum;
    _p2cbParam ()
    {
        self = NULL;
        mutex = NULL;
        magicNum = P2CBPARAM_MAGICNUM;
    };
    _p2cbParam(MfllMfb *s, Mutex *m)
    {
        self = s;
        mutex = m;
        magicNum = P2CBPARAM_MAGICNUM;
    }
} p2cbParam_t;

static MVOID pass2CbFunc(QParams& rParams)
{
    mfllAutoLogFunc();
    p2cbParam_t *p = reinterpret_cast<p2cbParam_t*>(rParams.mpCookie);
    /* check Magic NUM */
    if (p->magicNum != P2CBPARAM_MAGICNUM) {
        mfllLogE("%s: pass2 param is weird, magic num is different", __FUNCTION__);
    }
    p->mutex->unlock();
}

template <typename T>
inline MVOID updateEntry(IMetadata* pMetadata, MUINT32 const tag, T const& val)
{
    if (pMetadata == NULL)
    {
        mfllLogE("pMetadata is NULL");
        return;
    }

    IMetadata::IEntry entry(tag);
    entry.push_back(val, Type2Type<T>());
    pMetadata->update(tag, entry);
}

static MRect calCrop(MRect const &rSrc, MRect const &rDst, uint32_t ratio)
{
    #define ROUND_TO_2X(x) ((x) & (~0x1))

    MRect rCrop;

    // srcW/srcH < dstW/dstH
    if (rSrc.s.w * rDst.s.h < rDst.s.w * rSrc.s.h)
    {
        rCrop.s.w = rSrc.s.w;
        rCrop.s.h = rSrc.s.w * rDst.s.h / rDst.s.w;
    }
    // srcW/srcH > dstW/dstH
    else if (rSrc.s.w * rDst.s.h > rDst.s.w * rSrc.s.h)
    {
        rCrop.s.w = rSrc.s.h * rDst.s.w / rDst.s.h;
        rCrop.s.h = rSrc.s.h;
    }
    // srcW/srcH == dstW/dstH
    else
    {
        rCrop.s.w = rSrc.s.w;
        rCrop.s.h = rSrc.s.h;
    }

    rCrop.s.w =  ROUND_TO_2X(rCrop.s.w * 100 / ratio);
    rCrop.s.h =  ROUND_TO_2X(rCrop.s.h * 100 / ratio);

    rCrop.p.x = (rSrc.s.w - rCrop.s.w) / 2;
    rCrop.p.y = (rSrc.s.h - rCrop.s.h) / 2;

    #undef ROUND_TO_2X
    return rCrop;
}

/**
 *  M F L L    M F B
 */
IMfllMfb* IMfllMfb::createInstance(void)
{
    return (IMfllMfb*)new MfllMfb();
}

void IMfllMfb::destroyInstance(void)
{
    decStrong((void*)this);
}

MfllMfb::MfllMfb(void)
{
    m_pNormalStream = NULL;
    m_p3A = NULL;
    m_shotMode = (enum MfllMode)0;
    m_syncPrivateData = NULL;
    m_syncPrivateDataSize = 0;
    m_encodeYuvCount = 0;
    m_blendCount = 0;
}

MfllMfb::~MfllMfb(void)
{
    mfllAutoLogFunc();

    if (m_pNormalStream) {
        m_pNormalStream->uninit(LOG_TAG);
        m_pNormalStream->destroyInstance();
        m_pNormalStream = NULL;
    }

    if (m_p3A) {
        m_p3A->destroyInstance(LOG_TAG);
    }
}

enum MfllErr MfllMfb::init(int sensorId)
{
    mfllAutoLogFunc();
    mfllLogD("Init MfllMfb with sensor id --> %d", sensorId);
    m_sensorId = sensorId;

    m_pNormalStream = INormalStream::createInstance(sensorId);
    if (m_pNormalStream == NULL) {
        mfllLogE("create INormalStream fail");
    }
    else {
        m_pNormalStream->init(LOG_TAG);
    }

    m_p3A = IHal3A::createInstance(IHal3A::E_Camera_3, sensorId, LOG_TAG);
    if (m_p3A == NULL) {
        mfllLogE("create IHal3A fail");
    }

    return MfllErr_Ok;
}

enum MfllErr MfllMfb::blend(IMfllImageBuffer *base, IMfllImageBuffer *ref, IMfllImageBuffer *out, IMfllImageBuffer *wt_in, IMfllImageBuffer *wt_out)
{
    mfllAutoLogFunc();

    /* It MUST NOT be NULL, so don't do error handling */
    if (base == NULL)
        mfllLogE("%s: base is NULL", __FUNCTION__);
    if (ref == NULL)
        mfllLogE("%s: ref is NULL", __FUNCTION__);
    if (out == NULL)
        mfllLogE("%s: out is NULL", __FUNCTION__);
    if (wt_out == NULL)
        mfllLogE("%s: wt_out is NULL", __FUNCTION__);

    return blend(
            static_cast<MfllImageBuffer*>(base)->getImageBuffer(),
            static_cast<MfllImageBuffer*>(ref)->getImageBuffer(),
            static_cast<MfllImageBuffer*>(out)->getImageBuffer(),
            wt_in ? static_cast<MfllImageBuffer*>(wt_in)->getImageBuffer() : NULL,
            static_cast<MfllImageBuffer*>(wt_out)->getImageBuffer()
            );
}

enum MfllErr MfllMfb::blend(IImageBuffer *base, IImageBuffer *ref, IImageBuffer *out, IImageBuffer *wt_in, IImageBuffer *wt_out)
{
    enum MfllErr err = MfllErr_Ok;
    MBOOL bRet = MTRUE;

    mfllAutoLogFunc();
    mfllAutoTraceFunc();

    /* get member resource here */
    m_mutex.lock();
    enum NoiseReductionType nrType = m_nrType;
    EIspProfile_T profile = (EIspProfile_T)0; // which will be set later.
    int sensorId = m_sensorId;
    void *privateData = m_syncPrivateData;
    size_t privateDataSize = m_syncPrivateDataSize;
    int index = m_blendCount++;
    m_mutex.unlock();

    /**
     *  P A S S 2
     *
     *  Configure input parameters
     */
    QParams params;
    params.mvStreamTag.push_back(NSCam::NSIoPipe::NSPostProc::ENormalStreamTag_MFB_Bld);

    /* input: source frame, base frame [IMGI port] */
    {
        Input p;
        p.mBuffer       = base;
        p.mPortID       = MFB_PORT_IN_BASE_FRAME;
        p.mPortID.group = 0;
        params.mvIn.push_back(p);

        /* crop info */
        MCrpRsInfo crop;
        crop.mGroupID                   = 1; // source cropping info, set 1
        crop.mCropRect.p_integral.x     = 0; // position of cropping window, in pixel, integer
        crop.mCropRect.p_integral.y     = 0;
        crop.mCropRect.p_fractional.x   = 0; // always be 0
        crop.mCropRect.p_fractional.y   = 0;
        crop.mCropRect.s.w              = base->getImgSize().w;
        crop.mCropRect.s.h              = base->getImgSize().h;
        crop.mResizeDst.w               = base->getImgSize().w;
        crop.mResizeDst.h               = base->getImgSize().h;
        crop.mFrameGroup                = 0;
        params.mvCropRsInfo.push_back(crop);
        mfllLogD("%s: input base addr = %p", __FUNCTION__, p.mBuffer);
        mfllLogD("%s: input base size = %dx%d", __FUNCTION__, crop.mCropRect.s.w, crop.mCropRect.s.h);
        mfllLogD("%s: input base dst size = %dx%d", __FUNCTION__, crop.mResizeDst.w, crop.mResizeDst.h);
    }

    /* input: reference frame */
    {
        Input p;
        p.mBuffer       = ref;
        p.mPortID       = MFB_PORT_IN_REF_FRAME;
        p.mPortID.group = 0;
        params.mvIn.push_back(p);
        /* This input port doesn't support crop */
    }

    /* input: weighting table, for not the first blending, we need to give weighting map */
    if(wt_in) {
        Input p;
        p.mBuffer       = wt_in;
        p.mPortID       = MFB_PORT_IN_WEIGHTING;
        p.mPortID.group = 0;
        params.mvIn.push_back(p);
        /* This intput port doesn't support crop */
    }

    /* output: blended frame */
    {
        Output p;
        p.mBuffer       = out;
        p.mPortID       = MFB_PORT_OUT_FRAME;
        p.mPortID.group = 0;
        params.mvOut.push_back(p);

        /* crop info */
        MCrpRsInfo crop;
        crop.mGroupID                   = 2; // the first output frame, set to 2
        crop.mCropRect.p_integral.x     = 0; // position of cropping window, in pixel, integer
        crop.mCropRect.p_integral.y     = 0;
        crop.mCropRect.p_fractional.x   = 0;
        crop.mCropRect.p_fractional.y   = 0;
        crop.mCropRect.s.w              = base->getImgSize().w;
        crop.mCropRect.s.h              = base->getImgSize().h;
        crop.mResizeDst.w               = out->getImgSize().w;
        crop.mResizeDst.h               = out->getImgSize().h;
        crop.mFrameGroup                = 0;
        params.mvCropRsInfo.push_back(crop);
    }

    /* output: new weighting table */
    {
        Output p;
        p.mBuffer       = wt_out;
        p.mPortID       = MFB_PORT_OUT_WEIGHTING;
        p.mPortID.group = 0;
        params.mvOut.push_back(p);

        /* crop info */
        MCrpRsInfo crop;
        crop.mGroupID                   = 3; // the second output frame, set to 3
        crop.mCropRect.p_integral.x     = 0; // position of cropping window, in pixel, integer
        crop.mCropRect.p_integral.y     = 0;
        crop.mCropRect.p_fractional.x   = 0;
        crop.mCropRect.p_fractional.y   = 0;
        crop.mCropRect.s.w              = base->getImgSize().w;
        crop.mCropRect.s.h              = base->getImgSize().h;
        crop.mResizeDst.w               = wt_out->getImgSize().w;
        crop.mResizeDst.h               = wt_out->getImgSize().h;
        crop.mFrameGroup                = 0;
        params.mvCropRsInfo.push_back(crop);
    }

    /* determine ISP profile */
    if (IMfllCore::isZsdMode(m_shotMode)) {
        profile = (nrType == NoiseReductionType_SWNR ? EIspProfile_VSS_MFB_Blending_All_Off_SWNR :  EIspProfile_VSS_MFB_Blending_All_Off);
    }
    else {
        profile = (nrType == NoiseReductionType_SWNR ? EIspProfile_MFB_Blending_All_Off_SWNR :  EIspProfile_MFB_Blending_All_Off);
    }

    /* execute pass 2 operation */
    {
        /* pass 2 critical section */
        Mutex::Autolock _l(gMutexPass2Lock);

        /* add tuning data is necessary */
        std::unique_ptr<char> tuning_reg(new char[sizeof(dip_x_reg_t)]());
        void *tuning_lsc;

        mfllLogD("%s: create tuning register data chunk with size %d",
                __FUNCTION__, sizeof(dip_x_reg_t));

        TuningParam rTuningParam = { NULL, NULL};
        rTuningParam.pRegBuf = tuning_reg.get();

        updateEntry<MUINT8>(&(m_vMetaSet[index].halMeta), MTK_3A_PGN_ENABLE, 0);
        updateEntry<MUINT8>(&(m_vMetaSet[index].halMeta), MTK_3A_ISP_PROFILE, profile);

        if (m_p3A->setIsp(0, m_vMetaSet[index], &rTuningParam, NULL) == 0) {
            mfllLogD("%s: get tuning data, reg=%p, lsc=%p",
                    __FUNCTION__, rTuningParam.pRegBuf, rTuningParam.pLsc2Buf);

            // FIXME: cannot set isp profile to trigger MFB, force registers value here.
            {
                int bilinearFilter = (base->getImgSize() == ref->getImgSize() ? 0 : 1);
                dip_x_reg_t *tuningDat = (dip_x_reg_t*)tuning_reg.get();
                memset((void*)tuningDat, 0, sizeof(dip_x_reg_t)); // TODO: why

                /* MFB CON */
                tuningDat->DIP_X_MFB_CON.Raw = 0;
                tuningDat->DIP_X_MFB_CON.Bits.BLD_LL_TH_E               = 15;
                tuningDat->DIP_X_MFB_CON.Bits.BLD_LL_DB_EN              = 1;
                tuningDat->DIP_X_MFB_CON.Bits.BLD_LL_BRZ_EN             = bilinearFilter;
                tuningDat->DIP_X_MFB_CON.Bits.BLD_MODE = (index == 0 ? 0 : 1); // 0: first time, 1: blend with input weighting

                /* MFB_LL_CON1 */
                tuningDat->DIP_X_MFB_LL_CON1.Raw = 0;
                tuningDat->DIP_X_MFB_LL_CON1.Bits.BLD_LL_CLIP_TH2       = 16;
                tuningDat->DIP_X_MFB_LL_CON1.Bits.BLD_LL_CLIP_TH1       = 16;
                tuningDat->DIP_X_MFB_LL_CON1.Bits.BLD_LL_FLT_WT_MODE2   = 1;
                tuningDat->DIP_X_MFB_LL_CON1.Bits.BLD_LL_FLT_WT_MODE1   = 2;
                tuningDat->DIP_X_MFB_LL_CON1.Bits.BLD_LL_FLT_MODE       = 2;

                /* MFB_LL_CON2 */
                tuningDat->DIP_X_MFB_LL_CON2.Raw = 0;
                tuningDat->DIP_X_MFB_LL_CON2.Bits.BLD_LL_TH2            = 16;
                tuningDat->DIP_X_MFB_LL_CON2.Bits.BLD_LL_TH1            = 0;
                tuningDat->DIP_X_MFB_LL_CON2.Bits.BLD_LL_DT1            = 4;
                tuningDat->DIP_X_MFB_LL_CON2.Bits.BLD_LL_MAX_WT         = 6;

                /* MFB_LL_CON3 */
                /* output resolution */
                tuningDat->DIP_X_MFB_LL_CON3.Raw =
                    (wt_out->getImgSize().h << 16) | wt_out->getImgSize().w;

                /* MFB_LL_CON4 */
                tuningDat->DIP_X_MFB_LL_CON4.Raw = 0;
                tuningDat->DIP_X_MFB_LL_CON4.Bits.BLD_LL_DB_YDIST       = 0;
                tuningDat->DIP_X_MFB_LL_CON4.Bits.BLD_LL_DB_XDIST       = 4;

                /* MFB_EDGE */
                tuningDat->DIP_X_MFB_EDGE.Raw = 0xf; //

                /* copy from iopie test code */
                tuningDat->DIP_X_CTL_YUV_EN.Bits.G2C_EN = 1;//enable bit
                tuningDat->DIP_X_G2C_CONV_0A.Raw = 0x0200;
                tuningDat->DIP_X_G2C_CONV_0B.Raw = 0x0;
                tuningDat->DIP_X_G2C_CONV_1A.Raw = 0x02000000;
                tuningDat->DIP_X_G2C_CONV_1B.Raw = 0x0;
                tuningDat->DIP_X_G2C_CONV_2A.Raw = 0x0;
                tuningDat->DIP_X_G2C_CONV_2B.Raw = 0x0200;

                mfllLogD("MFB_CON    =0x%x", tuningDat->DIP_X_MFB_CON.Raw);
                mfllLogD("MFB_LL_CON1=0x%x", tuningDat->DIP_X_MFB_LL_CON1.Raw);
                mfllLogD("MFB_LL_CON2=0x%x", tuningDat->DIP_X_MFB_LL_CON2.Raw);
                mfllLogD("MFB_LL_CON3=0x%x", tuningDat->DIP_X_MFB_LL_CON3.Raw);
                mfllLogD("MFB_LL_CON4=0x%x", tuningDat->DIP_X_MFB_LL_CON4.Raw);
                mfllLogD("MFB_EDGE   =0x%x", tuningDat->DIP_X_MFB_EDGE.Raw);

            }
            params.mvTuningData.push_back((void*)tuning_reg.get()); // adding tuning data

        }
        else {
            mfllLogE("%s: set ISP profile failed...");
            err = MfllErr_UnexpectedError;
        }

        /**
         *  Finally, we're going to invoke P2 driver to encode Raw. Notice that,
         *  we must use async function call to trigger P2 driver, which means
         *  that we have to give a callback function to P2 driver.
         *
         *  E.g.:
         *      QParams::mpfnCallback = CALLBACK_FUNCTION
         *      QParams::mpCookie --> argument that CALLBACK_FUNCTION will get
         *
         *  Due to we wanna make the P2 driver as a synchronized flow, here we
         *  simply using Mutex to sync async call of P2 driver.
         */
        Mutex mutex;
        mutex.lock();

        p2cbParam_t p2cbPack(this, &mutex);

        params.mpfnCallback = pass2CbFunc;
        params.mpCookie = (void*)&p2cbPack;

        mfllLogD("%s: enque pass 2", __FUNCTION__);
        m_pNormalStream->enque(params);
        mfllLogD("%s: deque pass 2", __FUNCTION__);

        /* lock again, and wait */
        mutex.lock(); // locked, and wait unlock from pass2CbFunc.
        mutex.unlock(); // unlock.

        mfllLogD("mfb dequed");
    }
lbExit:
    return err;
}

enum MfllErr MfllMfb::mix(IMfllImageBuffer *base, IMfllImageBuffer *ref, IMfllImageBuffer *out, IMfllImageBuffer *wt)
{
    mfllAutoLogFunc();
    mfllAutoTraceFunc();

    enum MfllErr err = MfllErr_Ok;

    IImageBuffer *img_src = 0;
    IImageBuffer *img_ref = 0;
    IImageBuffer *img_dst = 0;
    IImageBuffer *img_wt = 0;

    /* check buffers */
    if (base == 0 || ref == 0 || out == 0 || wt == 0) {
        mfllLogE("%s: any argument cannot be NULL", __FUNCTION__);
        return MfllErr_BadArgument;
    }

    img_src = static_cast<MfllImageBuffer*>(base)->getImageBuffer();
    img_ref = static_cast<MfllImageBuffer*>(ref)->getImageBuffer();
    img_dst = static_cast<MfllImageBuffer*>(out)->getImageBuffer();
    img_wt =  static_cast<MfllImageBuffer*>(wt)->getImageBuffer();

    /* check resolution */
    {
        MSize size_src = img_src->getImgSize();
        MSize size_dst = img_dst->getImgSize();
        MSize size_wt  = img_wt->getImgSize();

        if (size_src != size_dst || size_src != size_wt) {
            mfllLogE("%s: Resolution of images are not the same", __FUNCTION__);
            mfllLogE("%s: src=%dx%d, dst=%dx%d, wt=%dx%d",
                    size_src.w, size_src.h,
                    size_dst.w, size_dst.h,
                    size_wt.w, size_wt.h);
            return MfllErr_BadArgument;
        }
    }

    m_mutex.lock();
    int sensorId = m_sensorId;
    enum NoiseReductionType nrType = m_nrType;
    enum MfllMode shotMode = m_shotMode;
    EIspProfile_T profile = (EIspProfile_T)0; // which will be set later.
    void *privateData = m_syncPrivateData;
    size_t privateDataSize = m_syncPrivateDataSize;
    m_mutex.unlock();

	MBOOL 	ret = MTRUE;

    /**
     * P A S S 2
     */
    QParams params;

    /* Mixing */
    params.mvStreamTag.push_back(NSCam::NSIoPipe::NSPostProc::ENormalStreamTag_MFB_Mix);

    // input: blended frame [IMGI port]
    {
        Input p;
        p.mPortID       = MIX_PORT_IN_BASE_FRAME; // should be IMGI port
        p.mPortID.group = 0;
        p.mBuffer       = img_src;
        params.mvIn.push_back(p);
        /* crop info */
        MCrpRsInfo crop;
        crop.mGroupID                   = 1; // source cropping info should be 1
        crop.mCropRect.p_integral.x     = 0; // cropping window offset, in pixel, integer
        crop.mCropRect.p_integral.y     = 0;
        crop.mCropRect.p_fractional.x   = 0; // always be 0
        crop.mCropRect.p_fractional.y   = 0;
        crop.mCropRect.s.w              = img_src->getImgSize().w; // cropping window size
        crop.mCropRect.s.h              = img_src->getImgSize().h;
        crop.mResizeDst.w               = img_src->getImgSize().w; // resize size
        crop.mResizeDst.h               = img_src->getImgSize().h;
        crop.mFrameGroup                = 0; // always be 0
        params.mvCropRsInfo.push_back(crop); // should be the first element in vector
    }

    // input: golden frame
    {
        Input p;
        p.mPortID       = MIX_PORT_IN_GOLDEN_FRAME;
        p.mPortID.group = 0;
        p.mBuffer       = img_ref;
        params.mvIn.push_back(p);
        /* this port is not support crop info */
    }

    // input: weighting
    {
        Input p;
        p.mPortID       = MIX_PORT_IN_WEIGHTING;
        p.mPortID.group = 0;
        p.mBuffer       = img_wt;
        params.mvIn.push_back(p);
        /* this port is not support crop info */
    }

    // output: a frame
    {
        Output p;
        p.mPortID       = MIX_PORT_OUT_FRAME;
        p.mPortID.group = 0;
        p.mBuffer       = img_dst;
        params.mvOut.push_back(p);
        /* crop info */
        MCrpRsInfo crop;
        crop.mGroupID                   = 2; // Mixing stage, it's only a output frame, hence the index of output cropping is 2
        crop.mCropRect.p_integral.x     = 0; // cropping window position, in pixel, integer
        crop.mCropRect.p_integral.y     = 0;
        crop.mCropRect.p_fractional.x   = 0; // always be 0
        crop.mCropRect.p_fractional.y   = 0;
        crop.mCropRect.s.w              = img_src->getImgSize().w;
        crop.mCropRect.s.h              = img_src->getImgSize().h;
        crop.mResizeDst.w               = img_dst->getImgSize().w;
        crop.mResizeDst.h               = img_dst->getImgSize().h;
        crop.mFrameGroup                = 0;
        params.mvCropRsInfo.push_back(crop);
    }

    /* determine ISP profile */
    if (IMfllCore::isZsdMode(shotMode)) {
        profile = (nrType == NoiseReductionType_SWNR ? EIspProfile_VSS_MFB_PostProc_Mixing_SWNR : EIspProfile_VSS_MFB_PostProc_Mixing);
    }
    else {
        profile = (nrType == NoiseReductionType_SWNR ? EIspProfile_MFB_PostProc_Mixing_SWNR : EIspProfile_MFB_PostProc_Mixing);
    }

    /* execute pass 2 operation */
    {
        /* pass 2 critical section */
        Mutex::Autolock _l(gMutexPass2Lock);

        /* add tuning data is necessary */
        std::unique_ptr<char> tuning_reg(new char[sizeof(dip_x_reg_t)]());
        void *tuning_lsc;

        mfllLogD("%s: create tuning register data chunk with size %d",
                __FUNCTION__, sizeof(dip_x_reg_t));

        TuningParam rTuningParam = { NULL, NULL};
        rTuningParam.pRegBuf = tuning_reg.get();

        updateEntry<MUINT8>(&(m_vMetaSet[0].halMeta), MTK_3A_PGN_ENABLE, 0);
        updateEntry<MUINT8>(&(m_vMetaSet[0].halMeta), MTK_3A_ISP_PROFILE, profile);

        if (m_p3A->setIsp(0, m_vMetaSet[0], &rTuningParam, NULL) == 0) {

            mfllLogD("%s: get tuning data, reg=%p, lsc=%p",
                    __FUNCTION__, rTuningParam.pRegBuf, rTuningParam.pLsc2Buf);

            params.mvTuningData.push_back(tuning_reg.get()); // adding tuning data
        }
        else {
            mfllLogE("%s: set ISP profile failed...");
            err = MfllErr_UnexpectedError;
        }

        /**
         *  Finally, we're going to invoke P2 driver to encode Raw. Notice that,
         *  we must use async function call to trigger P2 driver, which means
         *  that we have to give a callback function to P2 driver.
         *
         *  E.g.:
         *      QParams::mpfnCallback = CALLBACK_FUNCTION
         *      QParams::mpCookie --> argument that CALLBACK_FUNCTION will get
         *
         *  Due to we wanna make the P2 driver as a synchronized flow, here we
         *  simply using Mutex to sync async call of P2 driver.
         */
        Mutex mutex;
        mutex.lock();

        p2cbParam_t p2cbPack(this, &mutex);

        params.mpfnCallback = pass2CbFunc;
        params.mpCookie = (void*)&p2cbPack;

        m_pNormalStream->enque(params);

        /* lock again, and wait */
        mutex.lock(); // locked, and wait unlock from pass2CbFunc.
        mutex.unlock(); // unlock.

        mfllLogD("mix dequed");
    }

    return err;
}

enum MfllErr MfllMfb::setSyncPrivateData(void *data, size_t size)
{
    mfllLogD("data=%p, size=%d", data, size);
    if (data == NULL || size <= 0) {
        mfllLogW("set sync private data receieve NULL, ignored");
        return MfllErr_Ok;
    }
    Mutex::Autolock _l(m_mutex);
    MetaSet_T *m = (MetaSet_T*)data;
    m_metaSet = *m;
    m_vMetaSet.push_back(*m);
    mfllLogD("saves MetaSet_T, size = %d", m_vMetaSet.size());
    return MfllErr_Ok;
}

enum MfllErr MfllMfb::setNoiseReductionType(const enum NoiseReductionType &t)
{
    Mutex::Autolock _l(m_mutex);
    m_nrType = t;
    return MfllErr_Ok;
}

/******************************************************************************
 * encodeRawToYuv
 *
 * Interface for encoding a RAW buffer to an YUV buffer
 *****************************************************************************/
enum MfllErr MfllMfb::encodeRawToYuv(IMfllImageBuffer *input, IMfllImageBuffer *output, const enum YuvStage &s)
{
    return encodeRawToYuv(input, output, NULL, s);
}

/******************************************************************************
 * encodeRawToYuv
 *
 * Interface for encoding a RAW buffer to two YUV buffers
 *****************************************************************************/
enum MfllErr MfllMfb::encodeRawToYuv(
            IMfllImageBuffer *input,
            IMfllImageBuffer *output,
            IMfllImageBuffer *output_q,
            enum YuvStage s /* = YuvStage_RawToYuy2 */)
{
    enum MfllErr err = MfllErr_Ok;
    IImageBuffer *iimgOut2 = NULL;

    mfllAutoLogFunc();
    mfllAutoTraceFunc();

    if (input == NULL) {
        mfllLogE("%s: input buffer is NULL", __FUNCTION__);
        err = MfllErr_BadArgument;
    }
    if (output == NULL) {
        mfllLogE("%s: output buffer is NULL", __FUNCTION__);
        err = MfllErr_BadArgument;
    }
    if (err != MfllErr_Ok)
        goto lbExit;

    if (output_q) {
        iimgOut2 = reinterpret_cast<MfllImageBuffer*>(output_q)->getImageBuffer();
    }

    err = encodeRawToYuv(
            reinterpret_cast<MfllImageBuffer*>(input)->getImageBuffer(),
            reinterpret_cast<MfllImageBuffer*>(output)->getImageBuffer(),
            iimgOut2,
            s);

lbExit:
    return err;

}

/******************************************************************************
 * encodeRawToYuv
 *
 * Real implmentation that to control PASS 2 drvier for encoding RAW to YUV
 *****************************************************************************/
enum MfllErr MfllMfb::encodeRawToYuv(
        IImageBuffer *src,
        IImageBuffer *dst,
        IImageBuffer *dst2,
        const enum YuvStage &s)
{
    enum MfllErr err = MfllErr_Ok;
    MBOOL bRet = MTRUE;
    EIspProfile_T profile = EIspProfile_MFB_PostProc_EE_Off;

    mfllAutoLogFunc();

    /* If it's encoding base RAW ... */
    bool bBaseYuvStage = (s == YuvStage_BaseYuy2 || s == YuvStage_GoldenYuy2) ? true : false;

    m_mutex.lock();
    int index = m_encodeYuvCount++;
    int sensorId = m_sensorId;
    IImageBuffer *pInput = src;
    IImageBuffer *pOutput = dst;
    IImageBuffer *pOutputQ = dst2;
    int gid = 1;
    m_mutex.unlock();

    /* Do not increase YUV stage if it's encoding base YUV or golden YUV */
    if (bBaseYuvStage) {
        m_encodeYuvCount--;
        index = 0; // Use the first metadata
    }

    /**
     *  Select profile based on Stage:
     *  1) Raw to Yv16
     *  2) Encoding base YUV
     *  3) Encoding golden YUV
     *
     */
    switch(s) {
    case YuvStage_RawToYuy2:
    case YuvStage_RawToYv16:
        profile = EIspProfile_MFB_PostProc_EE_Off;
        break;

    /* Don't do EE */
    case YuvStage_BaseYuy2:
        if (IMfllCore::isZsdMode(m_shotMode)) {
            profile = EIspProfile_VSS_MFB_PostProc_EE_Off; // Not related with MNR
        }
        else {
            profile = EIspProfile_MFB_PostProc_EE_Off;
        }
        break;
    case YuvStage_GoldenYuy2:
        if (IMfllCore::isZsdMode(m_shotMode)) {
            profile = (m_nrType == NoiseReductionType_SWNR ? EIspProfile_VSS_MFB_PostProc_ANR_EE_SWNR : EIspProfile_VSS_MFB_PostProc_ANR_EE);
        }
        else {
            profile = (m_nrType == NoiseReductionType_SWNR ? EIspProfile_MFB_PostProc_ANR_EE_SWNR : EIspProfile_MFB_PostProc_ANR_EE);
        }
        break;
    } // switch

    /**
     *  Ok, we're going to configure P2 driver
     */
    QParams    params;

    /* Raw to yuv always using ENormalStreamTag_Normal */
    params.mvStreamTag.push_back(NSCam::NSIoPipe::NSPostProc::ENormalStreamTag_Normal);

    /* input: source frame [IMGI port] */
    {
        Input p;
        p.mBuffer          = pInput;
        p.mPortID          = RAW2YUV_PORT_IN;
        p.mPortID.group    = 0; // always be 0
        params.mvIn.push_back(p);

        /* cropping info */
        MCrpRsInfo crop;
        crop.mGroupID       = 1; // source cropping should be 1, only works with IMGI port
        crop.mCropRect.p_integral.x = 0; // position pixel, in integer
        crop.mCropRect.p_integral.y = 0;
        crop.mCropRect.p_fractional.x = 0; // always be 0
        crop.mCropRect.p_fractional.y = 0;
        crop.mCropRect.s.w  = pInput->getImgSize().w;
        crop.mCropRect.s.h  = pInput->getImgSize().h;
        crop.mResizeDst.w   = pInput->getImgSize().w;
        crop.mResizeDst.h   = pInput->getImgSize().h;
        crop.mFrameGroup    = 0;
        params.mvCropRsInfo.push_back(crop);
    }

    /* output: destination frame [WDMAO port] */
    {
        Output p;
        p.mBuffer          = pOutput;
        p.mPortID          = RAW2YUV_PORT_OUT;
        p.mPortID.group    = 0; // always be 0
        params.mvOut.push_back(p);

        /**
         *  Check resolution between input and output, if the ratio is different,
         *  a cropping window should be applied.
         */
        MRect srcCrop = calCrop(
                MRect(MPoint(0,0), pInput->getImgSize()),
                MRect(MPoint(0,0), pOutput->getImgSize()),
                100);

        /**
         *  Cropping info, only works with input port is IMGI port.
         *  mGroupID should be the index of the MCrpRsInfo.
         */
        MCrpRsInfo crop;
        crop.mGroupID       = 2; // the first output frame, it should be 2
        crop.mCropRect.p_integral.x = srcCrop.p.x; // position pixel, in integer
        crop.mCropRect.p_integral.y = srcCrop.p.y;
        crop.mCropRect.p_fractional.x = 0; // always be 0
        crop.mCropRect.p_fractional.y = 0;
        crop.mCropRect.s.w  = srcCrop.s.w;
        crop.mCropRect.s.h  = srcCrop.s.h;
        crop.mResizeDst.w   = pOutput->getImgSize().w;
        crop.mResizeDst.h   = pOutput->getImgSize().h;
        crop.mFrameGroup    = 0;
        params.mvCropRsInfo.push_back(crop);
    }

    /* output2: destination frame [WROTO prot] */
    if (pOutputQ) {
        Output p;
        p.mBuffer          = pOutputQ;
        p.mPortID          = RAW2YUV_PORT_OUT2;
        p.mPortID.group    = 0; // always be 0
        params.mvOut.push_back(p);

        /**
         *  Check resolution between input and output, if the ratio is different,
         *  a cropping window should be applied.
         */
        MRect srcCrop = calCrop(
                MRect(MPoint(0,0), pInput->getImgSize()),
                MRect(MPoint(0,0), pOutputQ->getImgSize()),
                100);

        /**
         *  Cropping info, only works with input port is IMGI port.
         *  mGroupID should be the index of the MCrpRsInfo.
         */
        MCrpRsInfo crop;
        crop.mGroupID       = 3; // the second output frame, it should be 3
        crop.mCropRect.p_integral.x = srcCrop.p.x; // position pixel, in integer
        crop.mCropRect.p_integral.y = srcCrop.p.y;
        crop.mCropRect.p_fractional.x = 0; // always be 0
        crop.mCropRect.p_fractional.y = 0;
        crop.mCropRect.s.w  = srcCrop.s.w;
        crop.mCropRect.s.h  = srcCrop.s.h;
        crop.mResizeDst.w   = pOutputQ->getImgSize().w;
        crop.mResizeDst.h   = pOutputQ->getImgSize().h;
        crop.mFrameGroup    = 0;
        params.mvCropRsInfo.push_back(crop);

        mfllLogD("output2: srcCrop (x,y,w,h)=(%d,%d,%d,%d)",
                crop.mCropRect.p_integral.x, crop.mCropRect.p_integral.y,
                crop.mCropRect.s.w, crop.mCropRect.s.h);
        mfllLogD("output2: dstSize (w,h)=(%d,%d",
                crop.mResizeDst.w, crop.mResizeDst.h);
    }

    /* get ready to operate P2 driver, it's a long stroy ... */
    {
        /* pass 2 critical section */
        mfllLogD("%s: wait pass2 critical section", __FUNCTION__);
        Mutex::Autolock _l(gMutexPass2Lock);
        mfllLogD("%s: enter pass2 critical section", __FUNCTION__);

        /* add tuning data is necessary */
        std::unique_ptr<char> tuning_reg(new char[sizeof(dip_x_reg_t)]());
        void *tuning_lsc;

        mfllLogD("%s: create tuning register data chunk with size %d",
                __FUNCTION__, sizeof(dip_x_reg_t));

        {
            TuningParam rTuningParam = { NULL, NULL};
            rTuningParam.pRegBuf = tuning_reg.get();
            updateEntry<MUINT8>(&(m_vMetaSet[index].halMeta), MTK_3A_PGN_ENABLE, 1);
            updateEntry<MUINT8>(&(m_vMetaSet[index].halMeta), MTK_3A_ISP_PROFILE, profile);

            if (m_p3A->setIsp(0, m_vMetaSet[index], &rTuningParam, NULL) == 0) {

                mfllLogD("%s: get tuning data, reg=%p, lsc=%p",
                        __FUNCTION__, rTuningParam.pRegBuf, rTuningParam.pLsc2Buf);

                params.mvTuningData.push_back(tuning_reg.get()); // adding tuning data

                /* LSC tuning data is constructed as an IImageBuffer, and we don't need to release */
                IImageBuffer* pSrc = static_cast<IImageBuffer*>(rTuningParam.pLsc2Buf);
                if (pSrc != NULL) {
                    Input src;
                    src.mPortID         = PORT_DEPI;
                    src.mPortID.group   = 0;
                    src.mBuffer         = pSrc;
                    params.mvIn.push_back(src);
                }
                else {
                    mfllLogE("create LSC image buffer fail");
                    err = MfllErr_UnexpectedError;
                }
            }
            else {
                mfllLogE("%s: set ISP profile failed...");
                err = MfllErr_UnexpectedError;
            }
        }

        /**
         *  Finally, we're going to invoke P2 driver to encode Raw. Notice that,
         *  we must use async function call to trigger P2 driver, which means
         *  that we have to give a callback function to P2 driver.
         *
         *  E.g.:
         *      QParams::mpfnCallback = CALLBACK_FUNCTION
         *      QParams::mpCookie --> argument that CALLBACK_FUNCTION will get
         *
         *  Due to we wanna make the P2 driver as a synchronized flow, here we
         *  simply using Mutex to sync async call of P2 driver.
         */
        Mutex mutex;
        mutex.lock();

        p2cbParam_t p2cbPack(this, &mutex);

        params.mpfnCallback = pass2CbFunc;
        params.mpCookie = (void*)&p2cbPack;

        mfllLogD("%s: enque", __FUNCTION__);
        m_pNormalStream->enque(params);
        mfllLogD("%s: dequed", __FUNCTION__);

        /* lock again, and wait */
        mutex.lock(); // locked, and wait unlock from pass2CbFunc.
        mutex.unlock(); // unlock.

        mfllLogD("p2 dequed");
    }

lbExit:
    return err;
}

/******************************************************************************
 * convertImage
 *
 * An interface for converting image follows transform and crop rectangle
 *****************************************************************************/
enum MfllErr MfllMfb::convertImage(
        sp<IMfllImageBuffer> src,
        sp<IMfllImageBuffer> dst,
        sp<IMfllImageBuffer> dst2 /* = NULL */,
        unsigned int transform /* = 0 */,
        unsigned int transform2 /* = 0 */,
        const MfllRect_t cropRect /* = MfllRect_t() */,
        const MfllRect_t cropRect2 /* = MfllRect_t() */)
{
    mfllAutoLogFunc();
    enum MfllErr err = MfllErr_Ok;
    if (src.get() == NULL) {
        mfllLogE("%s: source image is NULL", __FUNCTION__);
        err = MfllErr_BadArgument;
        return err;
    }

    if (dst.get() == NULL) {
        mfllLogE("%s: destination image is NULL", __FUNCTION__);
        err = MfllErr_BadArgument;
        return err;
    }

    int usingCrop = 0;
    int usingCrop2 = 0;
    MRect r, r2; // default construct size as 0

#define IS_ZERO_CROP(c) c.w == 0 && c.h == 0
    if (IS_ZERO_CROP(cropRect))
        usingCrop = 0;
    else
        usingCrop = 1;

    if (usingCrop) {
        r.p.x = cropRect.x;
        r.p.y = cropRect.y;
        r.s.w = cropRect.w;
        r.s.h = cropRect.h;
    }

    /* check if dst2 exists */
    if (dst2.get()) {
        mfllLogD("%s: the second image exists", __FUNCTION__);
        usingCrop2 =  (IS_ZERO_CROP(cropRect2)) ? false : true;
        if (usingCrop2) {
            r2.p.x = cropRect.x;
            r2.p.y = cropRect.y;
            r2.s.w = cropRect.w;
            r2.s.h = cropRect.h;
        }
    }
#undef IS_ZERO_CROP

    return convertImage(
            reinterpret_cast<MfllImageBuffer*>(src.get())->getImageBuffer(),
            reinterpret_cast<MfllImageBuffer*>(dst.get())->getImageBuffer(),
            dst2.get() ? reinterpret_cast<MfllImageBuffer*>(dst2.get())->getImageBuffer() : NULL,
            transform,
            transform2,
            usingCrop,
            usingCrop2,
            r,
            r2);
}

/******************************************************************************
 * convertImage
 *
 * Real implmentation that to convert image
 *****************************************************************************/
enum MfllErr MfllMfb::convertImage(
        IImageBuffer *img_src,
        IImageBuffer *img_dst,
        IImageBuffer *img_dst2,
        unsigned int transform,
        unsigned int transform2,
        int usingCrop /* = 0 */,
        int usingCrop2 /* = 0 */,
        MRect cropRect /* = {MPoint(0, 0), MSize(0,0)} */,
        MRect cropRect2 /*= {MPoint(0, 0), MSize(0,0)} */
)
{
    MBOOL ret = MTRUE;
    enum MfllErr err = MfllErr_Ok;
    IImageTransform *pTransformer = NULL;

    mfllAutoLogFunc();

    if (img_src == 0 || img_dst == 0) {
        mfllLogE("%s: source or destination is NULL", __FUNCTION__);
        err =  MfllErr_BadArgument;
        goto lbExit;
    }

    // Create Instance
    pTransformer = IImageTransform::createInstance();
    if (pTransformer == NULL) {
        mfllLogE("%s: create IImageTransform failed", __FUNCTION__);
        err = MfllErr_UnexpectedError;
        goto lbExit;
    }

    // mapping transform from degree to enum
    switch (transform) {
    case 90:    transform = eTransform_ROT_90;  break;
    case 180:   transform = eTransform_ROT_180; break;
    case 270:   transform = eTransform_ROT_270; break;
    default:    mfllLogW("%s: orientation (transform) degree is not 0,90,180, or 270. Set it to 0", __FUNCTION__);
    case 0:     transform = 0;   break;
    }
    switch (transform2) {
    case 90:    transform2 = eTransform_ROT_90;  break;
    case 180:   transform2 = eTransform_ROT_180; break;
    case 270:   transform2 = eTransform_ROT_270; break;
    default:    mfllLogW("%s: orientation (transform2) degree is not 0,90,180, or 270. Set it to 0", __FUNCTION__);
    case 0:     transform2 = 0;   break;
    }

    // calculate crop1
    {
        MRect srcRect(MPoint(0,0), img_src->getImgSize());
        MRect dstRect(MPoint(0,0), img_dst->getImgSize());
        // update cropping window if transform is 90 or 270 degree
        if (transform == eTransform_ROT_90 || transform == eTransform_ROT_270) {
            dstRect = MRect(MPoint(0,0), MSize(img_dst->getImgSize().h, img_dst->getImgSize().w));
        }
        cropRect = calCrop(srcRect, dstRect, 100);
    }

    // calculate crop2 (if necessary)
    if (img_dst2) {
        MRect srcRect(MPoint(0,0), img_src->getImgSize());
        MRect dstRect2(MPoint(0,0), img_dst2->getImgSize());

        // update cropping window if transform is 90 or 270 degree
        if (transform2 == eTransform_ROT_90 || transform2 == eTransform_ROT_270) {
            dstRect2 = MRect(MPoint(0,0), MSize(img_dst2->getImgSize().h, img_dst2->getImgSize().w));
        }

        cropRect2 = calCrop(srcRect, dstRect2, 100);
    }

    if (img_dst2 == NULL) {
        ret = pTransformer->execute(img_src, img_dst, 0, cropRect, transform, 3000);
    }
    else {
        ret = pTransformer->execute(
                img_src,
                img_dst,
                img_dst2,
                cropRect,
                cropRect2,
                transform,
                transform2,
                3000);
    }

    if (ret != MTRUE) {
        mfllLogE("%s: IImageTransform::execute failed", __FUNCTION__);
        err = MfllErr_UnexpectedError;
        goto lbExit;
    }

lbExit:

    if (pTransformer)
        pTransformer->destroyInstance();

    return err;
}

