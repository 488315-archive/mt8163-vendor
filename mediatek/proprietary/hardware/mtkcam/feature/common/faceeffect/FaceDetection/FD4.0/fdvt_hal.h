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
TO REVISE OR REPLACE THE MEDIATEK SOFTWARE AT ISSUE, OR REFUND ANY SOFTWARE LICENSE
 *     FEES OR SERVICE CHARGE PAID BY BUYER TO MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 *
 *     THE TRANSACTION CONTEMPLATED HEREUNDER SHALL BE CONSTRUED IN ACCORDANCE WITH THE LAWS
 *     OF THE STATE OF CALIFORNIA, USA, EXCLUDING ITS CONFLICT OF LAWS PRINCIPLES.
 ************************************************************************************************/
/*
** $Log: fd_hal_base.h $
 *
*/

#ifndef _FDVT_HAL_H_
#define _FDVT_HAL_H_

#include <fd_hal_base.h>
#include <pthread.h>
#include <semaphore.h>
#include <utils/Mutex.h>

class MTKDetection;
/*******************************************************************************
*
********************************************************************************/
class halFDVT: public halFDBase
{
public:
    //
    static halFDBase* getInstance();
    virtual void destroyInstance();
    //
    /////////////////////////////////////////////////////////////////////////
    //
    // halFDBase () -
    //! \brief FD Hal constructor
    //
    /////////////////////////////////////////////////////////////////////////
    halFDVT();

    /////////////////////////////////////////////////////////////////////////
    //
    // ~mhalCamBase () -
    //! \brief mhal cam base descontrustor
    //
    /////////////////////////////////////////////////////////////////////////
    virtual ~halFDVT();

    /////////////////////////////////////////////////////////////////////////
    //
    // mHalFDInit () -
    //! \brief init face detection
    //
    /////////////////////////////////////////////////////////////////////////
    //virtual MINT32 halFDInit(MUINT32 fdW, MUINT32 fdH, MUINT32 WorkingBuffer, MUINT32 WorkingBufferSize, MBOOL   SWResizerEnable);
    virtual MINT32 halFDInit(MUINT32 fdW, MUINT32 fdH, MUINT8 *WorkingBuffer, MUINT32 WorkingBufferSize, MBOOL SWResizerEnable, MUINT8 Current_mode);

    /////////////////////////////////////////////////////////////////////////
    //
    // halFDGetVersion () -
    //! \brief get FD version
    //
    /////////////////////////////////////////////////////////////////////////
    virtual MINT32 halFDGetVersion();

    /////////////////////////////////////////////////////////////////////////
    //
    // mHalFDVTDo () -
    //! \brief process face detection
    //
    /////////////////////////////////////////////////////////////////////////
    virtual MINT32 halFDDo( MUINT8 *ScaleImages,
                            MUINT8 *ImageBuffer1, //RGB565 QVGA
                            MUINT8 *ImageBuffer2, //Source Buffer (Y)
                            MBOOL   SDEnable,
                            MINT32 rRotation_Info,
                            MUINT8 *ImageBufferPhy,    //Source Buffer Physical
                            MUINT8 *ImageBufferPhy2 = NULL,
                            MUINT8 *ImageBufferPhy3 = NULL,
                            MBOOL   AE_Stable = 1
                            );

    /////////////////////////////////////////////////////////////////////////
    //
    // mHalFDVTUninit () -
    //! \brief FDVT uninit
    //
    /////////////////////////////////////////////////////////////////////////
    virtual MINT32 halFDUninit();

    /////////////////////////////////////////////////////////////////////////
    //
    // mHalFDVTDrawFaceRect () -
    //! \brief draw FDVT face detection result rectangle
    //
    /////////////////////////////////////////////////////////////////////////
    virtual MINT32 halFDDrawFaceRect(MUINT8 *pbuf);

    /////////////////////////////////////////////////////////////////////////
    //
    // mHalSDVTDrawFaceRect () -
    //! \brief draw smile detection result rectangle
    //
    /////////////////////////////////////////////////////////////////////////
    virtual MINT32 halSDDrawFaceRect(MUINT8 *pbuf);

    /////////////////////////////////////////////////////////////////////////
    //
    // mHalFDVTGetFaceInfo () -
    //! \brief get face detection result
    //
    /////////////////////////////////////////////////////////////////////////
    virtual MINT32 halFDGetFaceInfo(MtkCameraFaceMetadata *fd_info_result);

    /////////////////////////////////////////////////////////////////////////
    //
    // mHalFDVTGetFaceResult () -
    //! \brief get face detection result
    //
    /////////////////////////////////////////////////////////////////////////
    virtual MINT32 halFDGetFaceResult(MtkCameraFaceMetadata * fd_result, MINT32 ResultMode = 1);

    /////////////////////////////////////////////////////////////////////////
    //
    // mHalFDVTSetDispInfo () -
    //! \brief set display info
    //
    /////////////////////////////////////////////////////////////////////////
    virtual MINT32 mHalFDSetDispInfo(MUINT32 x, MUINT32 y, MUINT32 w, MUINT32 h, MUINT32 rotate, MUINT32 sensor_rotate, MINT32 CameraId) ;

    /////////////////////////////////////////////////////////////////////////
    //
    // mHalSetDetectPara () -
    //! \brief set detection parameter
    //
    /////////////////////////////////////////////////////////////////////////
    virtual MINT32 halSetDetectPara(MUINT8 Para) ;

    /////////////////////////////////////////////////////////////////////////
    //
    // mHalSDGetSmileResult () -
    //! \brief get smile detection result
    //
    /////////////////////////////////////////////////////////////////////////
    virtual MINT32 halSDGetSmileResult( ) ;


    /////////////////////////////////////////////////////////////////////////
    //
    // halFDBufferCreate () -
    //! \brief create face buffer
    //
    /////////////////////////////////////////////////////////////////////////
    virtual MINT32 halFDBufferCreate(MUINT8 *dstAddr, MUINT8 *srcAddr, MUINT8  ucBufferGroup);

    /////////////////////////////////////////////////////////////////////////
    //
    // halFTBufferCreate () -
    //! \brief create face tracking buffer
    //
    /////////////////////////////////////////////////////////////////////////
    virtual MINT32 halFTBufferCreate(MUINT8 *dstAddr, MUINT8 *srcAddr, MUINT8  ucPlane, MUINT32 src_width, MUINT32 src_height);

    /////////////////////////////////////////////////////////////////////////
    //
    // halFDYUYV2ExtractY () -
    //! \brief create Y Channel
    //
    /////////////////////////////////////////////////////////////////////////
    virtual MINT32 halFDYUYV2ExtractY(MUINT8 *dstAddr, MUINT8 *srcAddr, MUINT32 src_width, MUINT32 src_height);

    /////////////////////////////////////////////////////////////////////////
    //
    // halFDASDBufferCreate () -
    //! \brief create face buffer
    //
    /////////////////////////////////////////////////////////////////////////
    virtual MINT32 halFDASDBufferCreate(MUINT8 *dstAddr, MUINT8 *srcAddr, MUINT8  ucBufferGroup);

    virtual MINT32 halFTBufferCreateAsync(MUINT8 *dstAddr, MUINT8 *srcAddr, MUINT8  ucPlane, MUINT32 src_width, MUINT32 src_height);

    static void AcquireFTBuffer(void* arg);

    static void LockFTBuffer(void* arg);

    static void UnlockFTBuffer(void* arg);

private:
    static void* onFTThreadLoop(void*);

protected:

    MTKDetection* m_pMTKFDVTObj;

    MUINT32 m_FDW;
    MUINT32 m_FDH;
    MUINT32 m_DispW;
    MUINT32 m_DispH;
    MUINT32 m_DispX;
    MUINT32 m_DispY;
    MUINT32 m_DispRoate;
    MUINT32 m_RegisterBuff;
    MUINT32 m_BuffCount;
    MUINT8 m_DetectPara;

    MBOOL   m_Inited;

    pthread_t   mFTThread;
    sem_t       mSemFTExec;
    sem_t       mSemFTLock;
    MINT32      mFTStop;
    MINT32      mFTBufReady;
    struct FTParam {
        MUINT8 *dstAddr;
        MUINT8 *srcAddr;
        MUINT8 ucPlane;
        MUINT32 src_width;
        MUINT32 src_height;
    };
    struct FTParam mFTParameter;
};

#endif

