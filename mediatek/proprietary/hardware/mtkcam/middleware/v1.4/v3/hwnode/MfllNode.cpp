#include <Log.h>
#include "MFCNodeImp.h"
#include "hwnode_utilities.h"

#include <v3/hwnode/MfllNode.h>
#include <metadata/mtk_platform_metadata_tag.h>


#include <Hal3/aaa_hal_common.h>
#include <Hal3/IHal3A.h>

#include <iopipe/PostProc/INormalStream.h>
#include <iopipe/PostProc/IPortEnum.h>

#include <metadata/IMetadataProvider.h>

#include <IHalSensor.h>

/* Mfll related */
#include <common/mfnr/MfllLog.h>
#include <common/mfnr/IMfllCore.h>
#include <common/mfnr/IMfllImageBuffer.h>
#include <common/mfnr/IMfllCapturer.h>
#include <common/mfnr/IMfllMfb.h>

#include <sys/prctl.h>
#include <sys/resource.h>
#include <utils/RWLock.h>
#include <utils/Thread.h>
#include <cutils/properties.h>

#include <pthread.h>
#include <vector>
#include <queue>

#define LOG_TAG "MtkCam/MfllNode"

#define MY_LOGV(fmt, arg...)        CAM_LOGV("[%s] " fmt, __FUNCTION__, ##arg)
#define MY_LOGD(fmt, arg...)        CAM_LOGD("[%s] " fmt, __FUNCTION__, ##arg)
#define MY_LOGI(fmt, arg...)        CAM_LOGI("[%s] " fmt, __FUNCTION__, ##arg)
#define MY_LOGW(fmt, arg...)        CAM_LOGW("[%s] " fmt, __FUNCTION__, ##arg)
#define MY_LOGE(fmt, arg...)        CAM_LOGE("[%s] " fmt, __FUNCTION__, ##arg)
#define MY_LOGA(fmt, arg...)        CAM_LOGA("[%s] " fmt, __FUNCTION__, ##arg)
#define MY_LOGF(fmt, arg...)        CAM_LOGF("[%s] " fmt, __FUNCTION__, ##arg)

#define MY_LOGV_IF(cond, ...)       do { if ( (cond) ) { MY_LOGV(__VA_ARGS__); } }while(0)
#define MY_LOGD_IF(cond, ...)       do { if ( (cond) ) { MY_LOGD(__VA_ARGS__); } }while(0)
#define MY_LOGI_IF(cond, ...)       do { if ( (cond) ) { MY_LOGI(__VA_ARGS__); } }while(0)
#define MY_LOGW_IF(cond, ...)       do { if ( (cond) ) { MY_LOGW(__VA_ARGS__); } }while(0)
#define MY_LOGE_IF(cond, ...)       do { if ( (cond) ) { MY_LOGE(__VA_ARGS__); } }while(0)
#define MY_LOGA_IF(cond, ...)       do { if ( (cond) ) { MY_LOGA(__VA_ARGS__); } }while(0)
#define MY_LOGF_IF(cond, ...)       do { if ( (cond) ) { MY_LOGF(__VA_ARGS__); } }while(0)

using namespace android;
using namespace NSCam;
using namespace NSCam::v3;
using namespace NSCam::Utils::Sync;
using namespace NS3Av3;
using namespace std;
using namespace NSIoPipe;
using namespace NSIoPipe::NSPostProc;

using mfll::IMfllCore;
using mfll::IMfllImageBuffer;
using mfll::IMfllCapturer;
using mfll::IMfllMfb;
using mfll::MfllErr;

// FIXME: do not force capture num
#define CAPTURE_NUM 6

typedef sp<IImageBuffer> IImageBufferSP;

/**
 *  Trick: to use std::vector rather than android::Vector to avoid compile error
 *         due to that IImageBuffer is not totally implemented by android::sp.
 */
typedef std::vector<IImageBufferSP> IImageBuffer_Vector;


/**
 *  Since lots work has been done by MFCNodeImp, MfllNode only takes care about
 *  the necessary implementation that MFNR needs to do and interfaces from MFCNodeImp
 */
class MfllNodeImp : public MFCNodeImp /* Using MFCNodeImp as common parts */
{
public:
    MfllNodeImp();

public:
    /* for event queue usage */
    typedef enum tagEventType {
        EventType_InitMfllCore = 0,
        EventType_WaitGetCaptureParam,
        EventType_DoSingleFrame,
        EventType_DoMfll,
        EventType_DoMoveOwnerShip,
        EventType_Exit,
        /* size */
        EventType_Size
    } EventType;

    typedef struct _EventPack {
        EventType type;
        void* arg;
        _EventPack(const EventType &t, void *p)
            : type(t)
            , arg(p)
        {}
        _EventPack(const EventType& t)
            : _EventPack(t, NULL)
        {}
        _EventPack()
            : _EventPack(EventType_Size)
        {}
    } EventPack;

/**
 *  Implementations of MFCNode
 */
protected:
    virtual void onInit(InitParams &rParams);
    virtual void onUninit(void);
    virtual void onConfig(ConfigParams &rParams);
    virtual void onQueue(sp<IPipelineFrame> &f);
    virtual void onFlush(sp<IPipelineFrame> &f);
    virtual void onDequeue(sp<IPipelineFrame> &f);
    virtual void onImageBuffer(const eIMGBUFTP &t, sp < IImageBuffer > &img);
    virtual void onMetadata(const eMDATATP &t, IMetadata *m);

/**
 *  General methods
 */
public:
    inline int getCapturedFrameNum(void) { return mCaptureFrameNum; }

/* event loop implementation */
public:
    void enqueEvent(const EventType &t);
    void enqueEvent(const EventPack &e);
    EventPack dequeEvent();
private:
    pthread_t mThreadEventLoop;
    std::queue<EventPack> mEventQueue;
    pthread_mutex_t mMutexEventQueue;
    pthread_cond_t mCondEventQueue;

/* do perations */
public:
    void doInitMfllCore();
    void doWaitGetParamCapture();
    void doMfll();
    void doMoveOwnerShip();


/* Attributes */
private:
    int m_widthCapture;
    int m_heightCapture;

    /* represent capture frame number */
    int mCaptureFrameNum;
    /* counting full size IImageBuffer MfllNode received. */
    int mNowCaptureFrameNum;

    Vector<FrameParams>     mvFrameParams;

    /* Saves full size RAW */
    IImageBuffer_Vector     mvInputRaws;
    /* Saves output YUV buffer for JPEG node, it should be only a buffer or something wrong. */
    IImageBuffer_Vector     mvOutputYuvJepg;

    sp<IMfllImageBuffer> mOutputMfllYuv;
    sp<IMfllImageBuffer> mOutputMfllThumbnail;
    std::vector< sp<IMfllImageBuffer> > mvInputMfllRaws;
    std::vector<mfll::MfllMotionVector_t> mvInputGmv;
    std::vector< MetaSet_T > mvMetaSet;
    std::vector< IMetadata > mvMetaSetApp;
    std::vector< IMetadata > mvMetaSetHal;

    /* MFNR related objects */
    sp<IMfllCore>       mpMfllCore;
    sp<IMfllCapturer>   mpMfllCapturer;
    sp<IMfllMfb>        mpMfb;

    mutable RWLock  mRWLockConfig;

    /* mutex/cond */
    pthread_mutex_t mMutexSizeInfo;
    pthread_cond_t mCondSizeInfo;

    /* JPEG orientation */
    MINT32 mOrientation;
};

/**
 *
 */

typedef MfllNodeImp::EventType EventType;
typedef MfllNodeImp::EventPack EventPack;


void* mfll_thread_loop(void *arg)
{
    MfllNodeImp *pNode = (MfllNodeImp*)arg;
    EventPack e;

    pNode->incStrong((void*)pNode);

    while (true) {
        MY_LOGD("deque event...");
        e = pNode->dequeEvent();
        MY_LOGD("dequed event --> id:%d, argument:%p", (unsigned int)e.type, e.arg);

        switch (e.type) {
        case MfllNodeImp::EventType_InitMfllCore:
            pNode->doInitMfllCore();
            break;
        case MfllNodeImp::EventType_WaitGetCaptureParam:
            pNode->doWaitGetParamCapture();
            break;
        case MfllNodeImp::EventType_DoMfll:
            pNode->doMfll();
            break;
        case MfllNodeImp::EventType_DoMoveOwnerShip:
            pNode->doMoveOwnerShip();
            break;
        }

        if (e.type == MfllNodeImp::EventType_Exit) {
            MY_LOGD("exit eventloop");
            break;
        }
    }

    pNode->decStrong((void*)pNode);

    return 0;
}

/**
 *  F U N C T I O N    I M P L M E N T A T I O N
 */
android::sp<MfllNode>
MfllNode::createInstance(void)
{
    return (MfllNode*)new MfllNodeImp;
}

MERROR
MfllNode::getCaptureParamSet(MINT32 openId, Vector<CaptureParam_T> &vCaptureParams)
{
    mfllAutoLogFunc();

    IHal3A *pHal3A = IHal3A::createInstance(IHal3A::E_Camera_3, openId, LOG_TAG);

    if (pHal3A == NULL) {
        mfllLogE("get 3A information failed");
        return BAD_VALUE;
    }

    /* assume AE is fixed now ... */
    CaptureParam_T tmpCap3AParam;
    pHal3A->send3ACtrl(E3ACtrl_GetExposureParam , (MINTPTR)&tmpCap3AParam, 0);

    if (pHal3A) pHal3A->destroyInstance(LOG_TAG);

    int exp = tmpCap3AParam.u4Eposuretime;
    int iso = tmpCap3AParam.u4RealISO;

    mfll::MfllConfig_t mfllCfg;
    mfllCfg.exposure = exp;
    mfllCfg.iso = iso;
    mfll::IMfllCore::getCaptureInfo(&mfllCfg, &mfllCfg);
    /* TODO: decide capture parameters based on AIS/MFNR */

    for (int i = 0; i < mfllCfg.capture_num ; i ++)
        vCaptureParams.push_back(tmpCap3AParam);

    return OK;
}

/**
 *  M F L L N O D E I M P
 */
MfllNodeImp::MfllNodeImp()
    : mNowCaptureFrameNum(0)
    , mCaptureFrameNum(0)
    , mMutexSizeInfo(PTHREAD_MUTEX_INITIALIZER)
    , mCondSizeInfo(PTHREAD_COND_INITIALIZER)
    , mMutexEventQueue(PTHREAD_MUTEX_INITIALIZER)
    , mCondEventQueue(PTHREAD_COND_INITIALIZER)
    , m_widthCapture(0)
    , m_heightCapture(0)
    , mOrientation(-1)
{
    mfllAutoLogFunc();
}

void MfllNodeImp::onInit(InitParams &rParams)
{
    /* create event loop */
    pthread_attr_t attr = {0, NULL, 1024 * 1024, 4096, SCHED_OTHER, ANDROID_PRIORITY_FOREGROUND};
    pthread_create(&mThreadEventLoop, &attr, mfll_thread_loop, (void*)this);
    // detach??
    // pthread_detach(xxx);
}

void MfllNodeImp::onUninit(void)
{
    MY_LOGD("%s +", __FUNCTION__);

    // release output frames
    for (MINT32 i = 0; i < mvFrameParams.size(); i ++) {

        releaseFrameParams(&mvFrameParams.editItemAt(i), false); // Do not delete FrameParams.
    }
    mvFrameParams.clear();
    mvInputRaws.clear();
    mvOutputYuvJepg.clear();

    {
        /* Exit event queue */
        void *_ret;
        enqueEvent(EventType_Exit);
        pthread_join(mThreadEventLoop, &_ret);
    }
    MY_LOGD("%s -", __FUNCTION__);
}

void MfllNodeImp::onConfig(ConfigParams &rParams)
{
    MY_LOGD("%s +", __FUNCTION__);

    if (rParams.vInFullRaw.empty()) {
        MY_LOGE("vInFullRaw is empty");
        // TODO: error handling
    }

    else {
        /* get ouput yuv size and orientation */
        MSize yuvSize = rParams.vOutYuvJpeg->getImgSize();
        MSize rawSize = rParams.vInFullRaw[0]->getImgSize();
        int transform = rParams.vOutYuvJpeg->getTransform();
        if (transform == eTransform_ROT_90 || transform == eTransform_ROT_270) {
            MSize t = yuvSize;
            yuvSize.w = t.h;
            yuvSize.h = t.w;
        }
        switch (transform) {
        case eTransform_ROT_90:     mOrientation = 90;  break;
        case eTransform_ROT_180:    mOrientation = 180; break;
        case eTransform_ROT_270:    mOrientation = 270; break;
        default:                    mOrientation = 0;   break;
        }

        mCaptureFrameNum = CAPTURE_NUM; // TODO: dynamic query capture frame rawCount;
        // TODO: if possible, using yuvSize but now MDP cannot support resizing of this resolution to YUV thumbnail size
        m_widthCapture = rawSize.w;
        m_heightCapture = rawSize.h;

        /* Ok, we got infromation we need, we can start MFNR now */
        enqueEvent(EventType_InitMfllCore);
        enqueEvent(EventType_WaitGetCaptureParam);
        enqueEvent(EventType_DoMfll);
        enqueEvent(EventType_DoMoveOwnerShip);
        enqueEvent(EventType_Exit);
    }
    MY_LOGD("%s -", __FUNCTION__);
}

void MfllNodeImp::onQueue(sp<IPipelineFrame> &f)
{
    mfllAutoLogFunc();

    MY_LOGD("onQueue %d", mNowCaptureFrameNum);

    FrameParams __frameParam;
    processFrame(f, &__frameParam);
    mvFrameParams.push_back(__frameParam);

    /* push metadata */
    MetaSet_T m = {
        .appMeta = mvMetaSetApp[mNowCaptureFrameNum],
        .halMeta = mvMetaSetHal[mNowCaptureFrameNum]
    };
    mpMfb->setSyncPrivateData((void*)&m, 1);

    /* push image */
    mpMfllCapturer->queueFullSizeRaw(mvInputMfllRaws.back());
    mpMfllCapturer->queueGmv(mvInputGmv.back());

    mNowCaptureFrameNum++;
}

void MfllNodeImp::onFlush(sp<IPipelineFrame> &f)
{
}

void MfllNodeImp::onDequeue(sp<IPipelineFrame> &f)
{
}

void MfllNodeImp::onImageBuffer(const eIMGBUFTP &t, sp<IImageBuffer> &img)
{
    mfllAutoLogFunc();

    switch (t) {
    case eIMGBUFTP_IN_RAW_FULLSIZE:
        MY_LOGD("On full size raw (%dx%d)", img->getImgSize().w, img->getImgSize().h);
        mvInputMfllRaws.push_back(IMfllImageBuffer::createInstance());
        mvInputMfllRaws.back()->setImageBuffer(img.get());
        break;
    case eIMGBUFTP_OUT_YUV_JPEG:
        MY_LOGD("On output YUV JPEG (%dx%d)", img->getImgSize().w, img->getImgSize().h);
        mOutputMfllYuv = IMfllImageBuffer::createInstance();
        mOutputMfllYuv->setImageBuffer(img.get());
        break;
    case eIMGBUFTP_OUT_YUV_THUMBNAIL:
        MY_LOGD("On output YUV JPEG thumbnail (%dx%d)", img->getImgSize().w, img->getImgSize().h);
        mOutputMfllThumbnail = IMfllImageBuffer::createInstance();
        mOutputMfllThumbnail->setImageBuffer(img.get());
        break;
    default: /* we don't care others */ break;
    }
}

void MfllNodeImp::onMetadata(const eMDATATP &t, IMetadata *m)
{
    mfllAutoLogFunc();

    switch (t) {
    case eMDATATP_IN_APP:
        mvMetaSetApp.push_back(*m);
        break;
    case eMDATATP_IN_HAL:
        {
            mvMetaSetHal.push_back(*m);

            mfll::MfllMotionVector  mv;
            MSize                   rzoSize;
            MBOOL                   ret = MTRUE;

            /* to get GMV info and the working resolution */
            ret = retrieveGmvInfo(m, mv.x, mv.y, rzoSize);

            /* if works, mapping it from rzoDomain to MfllCore domain */
            if (ret == MTRUE) {
                MY_LOGD("%s: GMV(x,y)=(%d,%d), unit based on resized RAW",
                        __FUNCTION__,
                        mv.x, mv.y);
                MY_LOGD("%s: mfll domain (width): %d, gmv domain(width): %d",
                        __FUNCTION__, m_widthCapture, rzoSize.w);
                /**
                 *  the cropping crops height only, not for width. Hence, just
                 *  simply uses width to calculate the ratio.
                 */
                float ratio = (float)(m_widthCapture/2) / (float)rzoSize.w;
                mv.x = (int)((float)mv.x * ratio + (mv.x < 0 ? -0.5f : 0.5f));
                mv.y = (int)((float)mv.y * ratio + (mv.y < 0 ? -0.5f : 0.5f));

                MY_LOGD("%s: GMV'(x,y)=(%d,%d), unit: Mfll domain",
                        __FUNCTION__,
                        mv.x, mv.y);
            }
            /* no matter ok or not, add GMV to MfllCapturer to avoid deadlock */
            mvInputGmv.push_back(mv);
        }
        break;
    case eMDATATP_OUT_APP:
    case eMDATATP_OUT_HAL:
    default: /* we don't care others */ break;
    }
}


void MfllNodeImp::doInitMfllCore()
{
    mfll::MfllBypassOption_t bypassOption;
    for (int i = 0; i < MFLL_MAX_FRAMES; i++)
        bypassOption.bypassAllocRawBuffer[i] = 1;

    /* create instances */
    mpMfllCore = IMfllCore::createInstance();
    mpMfllCapturer = IMfllCapturer::createInstance();
    mpMfb = IMfllMfb::createInstance();
    mpMfb->init(getOpenId());

    mpMfllCore->setBypassOption(bypassOption);
    mpMfllCore->setCapturer(mpMfllCapturer);
    mpMfllCore->setMfb(mpMfb);

    if (m_widthCapture <=0 || m_heightCapture <=0) {
        MY_LOGE("Capture resolution hasn't been set correctly");
    }
    else {
        MY_LOGD("Init mfll with capture size %d x %d", m_widthCapture, m_heightCapture);
        MY_LOGD("Init mfll with capture QSize %d x %d", m_widthCapture, m_heightCapture);
        mpMfllCore->setCaptureResolution(m_widthCapture, m_heightCapture);
        /* using full size MC */
        mpMfllCore->setCaptureQResolution(m_widthCapture, m_heightCapture);
    }
}

void MfllNodeImp::doWaitGetParamCapture()
{
}

void MfllNodeImp::doMfll()
{
    mfllAutoLogFunc();

    mfll::MfllConfig_t cfg;

    cfg.sensor_id = mOpenId;
    cfg.capture_num = getCapturedFrameNum();
    cfg.blend_num = getCapturedFrameNum();

    // TODO: specify feature options here for mfll
    CAM_TRACE_BEGIN("doMfll");
    mpMfllCore->doMfll(cfg); // synchronized call
    CAM_TRACE_END();

    /* ok. convert to output yuv for jpeg node */
    sp<IMfllImageBuffer> img = mpMfllCore->retrieveBuffer(mfll::MfllBuffer_MixedYuv);
    if (img.get() == NULL) {
        mfllLogE("%s: mixed output buffer is NULL", __FUNCTION__);
        return;
    }

    CAM_TRACE_BEGIN("doThumbnail");
    mpMfb->convertImage(
            img,
            mOutputMfllYuv,
            mOutputMfllThumbnail,
            (mOrientation < 0 ? 0 : mOrientation),
            0 // never rotate thumbnail
            );
    CAM_TRACE_END();
}

void MfllNodeImp::doMoveOwnerShip()
{
     // unlock and image/meta buffer, mark status and then apply buffer to be released
    for (MINT32 i = 0; i < mvFrameParams.size(); i++) {
        releaseFrame(&mvFrameParams.editItemAt(i));
    }
    // dispatch the main frame (i.e. index is 0) to the next node
    onDispatchFrame(mvFrameParams[0].pFrame);
}

/**
 *  Event Queue
 */
void MfllNodeImp::enqueEvent(const EventType &t)
{
    enqueEvent(EventPack(t));
}

void MfllNodeImp::enqueEvent(const EventPack &e)
{
    MY_LOGD("enque event --> %d", e);
    pthread_mutex_lock(&mMutexEventQueue);
    mEventQueue.push(e);
    pthread_cond_broadcast(&mCondEventQueue);
    pthread_mutex_unlock(&mMutexEventQueue);
}

EventPack MfllNodeImp::dequeEvent()
{
    EventPack e;
    pthread_mutex_lock(&mMutexEventQueue);
    if (mEventQueue.empty()) {
        pthread_cond_wait(&mCondEventQueue, &mMutexEventQueue);
    }
    e = mEventQueue.front();
    mEventQueue.pop();
    pthread_mutex_unlock(&mMutexEventQueue);

    return e;
}

