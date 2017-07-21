/********************************************************************************************
 *     LEGAL DISCLAIMER
 *
 *     (Header of MediaTek Software/Firmware Release or Documentation)
 *
 *     BY OPENING OR USING THIS FILE, BUYER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 *     THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE") RECEIVED
 *     FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO BUYER ON AN "AS-IS" BASIS
 *     ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL WARRANTIES, EXPRESS OR IMPLIED,
 *     INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR
 *     A PARTICULAR PURPOSE OR NONINFRINGEMENT. NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY
 *     WHATSOEVER WITH RESPECT TO THE SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY,
 *     INCORPORATED IN, OR SUPPLIED WITH THE MEDIATEK SOFTWARE, AND BUYER AGREES TO LOOK
 *     ONLY TO SUCH THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. MEDIATEK SHALL ALSO
 *     NOT BE RESPONSIBLE FOR ANY MEDIATEK SOFTWARE RELEASES MADE TO BUYER'S SPECIFICATION
 *     OR TO CONFORM TO A PARTICULAR STANDARD OR OPEN FORUM.
 *
 *     BUYER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND CUMULATIVE LIABILITY WITH
 *     RESPECT TO THE MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE, AT MEDIATEK'S OPTION,
 *     TO REVISE OR REPLACE THE MEDIATEK SOFTWARE AT ISSUE, OR REFUND ANY SOFTWARE LICENSE
 *     FEES OR SERVICE CHARGE PAID BY BUYER TO MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 *
 *     THE TRANSACTION CONTEMPLATED HEREUNDER SHALL BE CONSTRUED IN ACCORDANCE WITH THE LAWS
 *     OF THE STATE OF CALIFORNIA, USA, EXCLUDING ITS CONFLICT OF LAWS PRINCIPLES.
 ************************************************************************************************/
#ifndef _STEREO_DP_UTIL_H_
#define _STEREO_DP_UTIL_H_

#include <utils/include/Format.h>
#include <DpBlitStream.h>

using namespace NSCam;
#define IS_ALLOC_GB    true

class StereoDpUtil {
public:
    static bool transformImage(
                        IImageBuffer *imgSrc,
                        IImageBuffer *imgDst,
                        ENUM_ROTATION eRotateDegree = eRotate_0,
                        DpRect *dpROI = NULL
                       )
    {
        if(NULL == imgSrc ||
           NULL == imgDst)
        {
            return false;
        }

        DpBlitStream   *pStream = new DpBlitStream();
        const MUINT32 PLANE_COUNT = 4;
        void           *pBuffer[PLANE_COUNT];
        uint32_t       size[PLANE_COUNT];

        //Set src
        pBuffer[0] = (imgSrc->getPlaneCount() > 0) ? (void *)imgSrc->getBufVA(0) : NULL;
        pBuffer[1] = (imgSrc->getPlaneCount() > 1) ? (void *)imgSrc->getBufVA(1) : NULL;
        pBuffer[2] = (imgSrc->getPlaneCount() > 2) ? (void *)imgSrc->getBufVA(2) : NULL;
        pBuffer[3] = (imgSrc->getPlaneCount() > 3) ? (void *)imgSrc->getBufVA(3) : NULL;

        size[0] = (imgSrc->getPlaneCount() > 0) ? imgSrc->getBufSizeInBytes(0) : 0;
        size[1] = (imgSrc->getPlaneCount() > 1) ? imgSrc->getBufSizeInBytes(1) : 0;
        size[2] = (imgSrc->getPlaneCount() > 2) ? imgSrc->getBufSizeInBytes(2) : 0;
        size[3] = (imgSrc->getPlaneCount() > 3) ? imgSrc->getBufSizeInBytes(3) : 0;
        pStream->setSrcBuffer(pBuffer, size, imgSrc->getPlaneCount());

        //Only support YV12 and RGBA now
        DpColorFormat imgFormat = DP_COLOR_UNKNOWN;
        if(eImgFmt_YV12 == imgSrc->getImgFormat()) {
            imgFormat = DP_COLOR_YV12;
        } else if(eImgFmt_RGBA8888 == imgSrc->getImgFormat()) {
            imgFormat = DP_COLOR_RGBA8888;
        }

        if(DP_COLOR_UNKNOWN == imgFormat) {
            return false;
        }
        pStream->setSrcConfig(imgSrc->getImgSize().w, imgSrc->getImgSize().h,
                              imgFormat, eInterlace_None);

        // Set dst
        pBuffer[0] = (imgDst->getPlaneCount() > 0) ? (void *)imgDst->getBufVA(0) : NULL;
        pBuffer[1] = (imgDst->getPlaneCount() > 1) ? (void *)imgDst->getBufVA(1) : NULL;
        pBuffer[2] = (imgDst->getPlaneCount() > 2) ? (void *)imgDst->getBufVA(2) : NULL;
        pBuffer[3] = (imgDst->getPlaneCount() > 3) ? (void *)imgDst->getBufVA(3) : NULL;

        size[0] = (imgDst->getPlaneCount() > 0) ? imgDst->getBufSizeInBytes(0) : 0;
        size[1] = (imgDst->getPlaneCount() > 1) ? imgDst->getBufSizeInBytes(1) : 0;
        size[2] = (imgDst->getPlaneCount() > 2) ? imgDst->getBufSizeInBytes(2) : 0;
        size[3] = (imgDst->getPlaneCount() > 3) ? imgDst->getBufSizeInBytes(3) : 0;

        //For centerize ROI, we need to clean padding area by ourself
        for(MUINT32 n = 0; n < PLANE_COUNT; n++) {
            if(size[n] > 0) {
                ::memset(pBuffer[n], 0, size[n]);
            }
        }
        pStream->setDstBuffer(pBuffer, size, imgDst->getPlaneCount());

        //Only support YV12 and RGBA now
        imgFormat = DP_COLOR_UNKNOWN;
        if(eImgFmt_YV12 == imgDst->getImgFormat()) {
            imgFormat = DP_COLOR_YV12;
        } else if(eImgFmt_RGBA8888 == imgDst->getImgFormat()) {
            imgFormat = DP_COLOR_RGBA8888;
        }

        if(DP_COLOR_UNKNOWN == imgFormat) {
            return false;
        }

        if(eRotate_0   == eRotateDegree ||
           eRotate_180 == eRotateDegree)
        {
            pStream->setDstConfig(imgDst->getImgSize().w, imgDst->getImgSize().h, imgFormat, eInterlace_None, dpROI);
        } else {
            pStream->setDstConfig(imgDst->getImgSize().h, imgDst->getImgSize().w, imgFormat, eInterlace_None, dpROI);
        }

        pStream->setRotate(eRotateDegree);
        pStream->invalidate();
        delete pStream;

        return true;
    }

    static bool rotateBuffer(
                        MUINT8 *bufSrc,
                        MUINT8 *bufDst,
                        MSize szBuf,
                        ENUM_ROTATION eRotateDegree = eRotate_0
                       )
    {
        if(NULL == bufSrc
           || NULL == bufDst
           || 0 == szBuf.w
           || 0 == szBuf.h)
        {
            return false;
        }

        DpBlitStream   *pStream = new DpBlitStream();
        void           *pBuffer[1];
        uint32_t       size[1];

        //Set src
        pBuffer[0] = bufSrc;
        size[0] = szBuf.w * szBuf.h;
        pStream->setSrcBuffer(pBuffer, size, 1);
        pStream->setSrcConfig(szBuf.w, szBuf.h, DP_COLOR_GREY);

        // Set dst
        pBuffer[0] = bufDst;
        size[0] = szBuf.w * szBuf.h;
        pStream->setDstBuffer(pBuffer, size, 1);

        if(eRotate_0   == eRotateDegree ||
           eRotate_180 == eRotateDegree)
        {
            pStream->setDstConfig(szBuf.w, szBuf.h, DP_COLOR_GREY);
        } else {
            pStream->setDstConfig(szBuf.h, szBuf.w, DP_COLOR_GREY);
        }

        pStream->setRotate(eRotateDegree);
        pStream->invalidate();
        delete pStream;

        return true;
    }

    //Create IImgeBuffer from buffer, only support YV12 and mask
    static bool allocImageBuffer(const char *TAG, EImageFormat fmt, MSize size, bool isGB, sp<IImageBuffer>& retImage)
    {
        if( NULL == TAG
            || 0 == strlen(TAG)
            || ( eImgFmt_YV12 != fmt &&
                 eImgFmt_Y8   != fmt ) )
        {
            return false;
        }

        IImageBufferAllocator* allocator = IImageBufferAllocator::getInstance();
        if( NULL == allocator ) {
            ALOGE("cannot get allocator");
            return false;
        }

        MUINT const PLANE_COUNT = NSCam::Utils::Format::queryPlaneCount(fmt);
        MINT32 bufBoundaryInBytes[3] = {0, 0, 0};
        MUINT32 bufStridesInBytes[3];

        if( fmt == eImgFmt_BAYER8  ||
            fmt == eImgFmt_BAYER10 ||
            fmt == eImgFmt_BAYER12 ||
            fmt == eImgFmt_BAYER14 )
        {
//            for (MUINT i = 0; i < PLANE_COUNT; i++)
//            {
//                bufStridesInBytes[i] = srcbuf->getBufStridesInBytes(i);
//            }
        }
        else
        {
            for (MUINT i = 0; i < PLANE_COUNT; i++)
            {
                bufStridesInBytes[i] =
                    (NSCam::Utils::Format::queryPlaneWidthInPixels(fmt, i, size.w) * NSCam::Utils::Format::queryPlaneBitsPerPixel(fmt, i))>>3;
            }
        }

        ALOGD("alloc %d x %d, fmt 0x%X", size.w, size.h, fmt);
        IImageBufferAllocator::ImgParam imgParam(
                fmt,
                size,
                bufStridesInBytes,
                bufBoundaryInBytes,
                PLANE_COUNT
                );

        //
        if(isGB)
        {
            IImageBufferAllocator::ExtraParam extParam(eBUFFER_USAGE_HW_TEXTURE|eBUFFER_USAGE_SW_WRITE_RARELY);
            retImage = allocator->alloc_gb(TAG, imgParam, extParam);
        }
        else
        {
            retImage = allocator->alloc_ion(TAG, imgParam);
        }

        if( NULL == retImage.get() ) {
            return false;
        }

        if ( !retImage->lockBuf( TAG, eBUFFER_USAGE_SW_MASK | eBUFFER_USAGE_HW_MASK ) )
        {
            ALOGE("lock Buffer failed");
            return false;
        }

        if ( !retImage->syncCache( eCACHECTRL_INVALID ) )
        {
            ALOGE("syncCache failed");
            return false;
        }

        return true;
    }

    static bool freeImageBuffer(const char *TAG, sp<IImageBuffer>& imgBuf)
    {
        if( NULL == TAG
            || 0 == strlen(TAG)
            || NULL == imgBuf.get() )
        {
            return false;
        }

        IImageBufferAllocator* allocator = IImageBufferAllocator::getInstance();
        if( !imgBuf->unlockBuf( TAG ) )
        {
            ALOGE("unlock Buffer failed");
            return false;
        }

        allocator->free(imgBuf.get());
        imgBuf = NULL;

        return true;
    }

};
#endif