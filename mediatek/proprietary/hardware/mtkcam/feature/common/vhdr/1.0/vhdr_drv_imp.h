/* Copyright Statement:
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein
 * is confidential and proprietary to MediaTek Inc. and/or its licensors.
 * Without the prior written permission of MediaTek inc. and/or its licensors,
 * any reproduction, modification, use or disclosure of MediaTek Software,
 * and information contained herein, in whole or in part, shall be strictly prohibited.
 */
/* MediaTek Inc. (C) 2010. All rights reserved.
 *
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE")
 * RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO RECEIVER ON
 * AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL WARRANTIES,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
 * NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
 * SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
 * SUPPLIED WITH THE MEDIATEK SOFTWARE, AND RECEIVER AGREES TO LOOK ONLY TO SUCH
 * THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES
 * THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES
 * CONTAINED IN MEDIATEK SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK
 * SOFTWARE RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND
 * CUMULATIVE LIABILITY WITH RESPECT TO THE MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
 * AT MEDIATEK'S OPTION, TO REVISE OR REPLACE THE MEDIATEK SOFTWARE AT ISSUE,
 * OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY RECEIVER TO
 * MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 *
 * The following software/firmware and/or related documentation ("MediaTek Software")
 * have been modified by MediaTek Inc. All revisions are subject to any receiver's
 * applicable license agreements with MediaTek Inc.
 */

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

/**
* @file vhdr_drv_imp.h
*
* VHDR Driver Implementation Header File
*
*/


#ifndef _VHDR_DRV_IMP_H_
#define _VHDR_DRV_IMP_H_

#include "vhdr_drv.h"
#include <iopipe/CamIO/Cam_Notify.h>

/**
  * @brief LCSO data structure
  *
*/
typedef struct _LCSO_DATA_
{
    MUINT32 va;
    MUINT32 pa;
    MINT64  timeStamp;
}LCSO_DATA;



/**
  *@brief VHDR Pass1 HW Setting Callback
*/
class VHdrP1Cb : public P1_TUNING_NOTIFY
{
 	public:
 	   VHdrP1Cb(MVOID *arg);
       virtual ~VHdrP1Cb();

 	virtual void p1TuningNotify(MVOID *pInput,MVOID *pOutput);
    virtual const char* TuningName() { return "Update VHDR"; }
 };


/**
  *@brief Implementation of VHdrDrv class
*/
class VHdrDrvImp : public VHdrDrv
{
public:

    /**
         *@brief Create VHdrDrv object
         *@param[in] sensorIdx : sensor index
         *@return
         *-VHdrDrv object
       */
    static VHdrDrv *GetInstance(const MUINT32 &aSensorIdx);

    /**
         *@brief Destroy VHdrDrv object
         *@param[in] userName : user name,i.e. who destroy VHdrDrv object
       */
    virtual MVOID DestroyInstance();

    /**
         *@brief Initialization function
       */
    virtual MINT32 Init(MUINT32 lcsoW, MUINT32 lcsoH);

    /**
         *@brief Unitialization function
       */
    virtual MINT32 Uninit();

    /**
         *@brief Set flare info
         *@param[in] aFlareGain : flare gain
         *@param[in] aFlareOffset : flare offset
       */
    virtual MVOID SetFlareInfo(const MINT16 &aFlareGain,const MINT16 &aFlareOffset);

    /**
         *@brief Set sensor info
         *@param[in] aSensorDev : sensor device
         *@param[in] aSensorTg : flare sensor TG
       */
    virtual MVOID SetSensorInfo(const MUINT32 &aSensorDev,const MUINT32 &aSensorTg);

    /**
         *@brief Set configure done
         *@param[in] aState : state enum
       */
    virtual MVOID SetLcsoThreadState(VHDR_STATE_ENUM aState);

    /**
         *@brief Set configure done
         *@param[in] aCmd : VHDR_CMD_ENUM
         *@param[in] arg1
         *@param[in] arg2
         *@param[in] arg3
       */
    virtual MVOID SetVideoSizeInfo(const VHDR_CMD_ENUM &aCmd,MINT32 arg1,MINT32 arg2 = -1,MINT32 arg3 = -1);

    /**
         *@brief Config LCS
         *@return
         *-VHDR_RETURN_NO_ERROR indicates success, otherwise indicates fail
       */
    virtual MINT32 ConfigLcs();

    /**
         *@brief Config LCSO
         *@return
         *-VHDR_RETURN_NO_ERROR indicates success, otherwise indicates fail
       */
    virtual MINT32 ConfigLcso();

    /**
         *@brief Config RMG for ivhdr
         *@param[in] aLeFirst : 0 : first two rows are SE,  1 : first two rows are LE
       */
    virtual MVOID ConfigRmg_ivHDR(const MUINT32 &aLeFirst);

    /**
         *@brief Config RMG for zvhdr
         *@param[in] zPattern : 0: zhdr no support , 1~ 8 suppport pattern
       */
    virtual MVOID ConfigRmg_zvHDR(const MUINT32 &zPattern);

    /**
         *@brief Update LCS
         *@return
         *-VHDR_RETURN_NO_ERROR indicates success, otherwise indicates fail
       */
    virtual MINT32 UpdateLcs();

    /**
         *@brief Update LCSO time stamp according to pass1 image
         *@param[in] aTimeStamp : time stamp of  pass1 image
       */
    virtual MVOID UpdateLcsoTimeStamp(const MINT64 &aTimeStamp);

    /**
         *@brief Get LCEI DMA setting
         *@param[in,out] apLceiDmaInfo : LCEI_DMA_INFO
         *@param[in] aTimeStamp : time stamp of pass1 image which is going to run pass2
       */
    virtual MVOID GetLceiDmaInfo(LCEI_DMA_INFO *apLceiDmaInfo,const MINT64 &aTimeStamp);

    /**
         *@brief Enable LCS and LCSO
         *@param[in] aEn : MFALSE - disable, MTRUE - enable
         *@return
         *-VHDR_RETURN_NO_ERROR indicates success, otherwise indicates fail
       */
    virtual MINT32 EnableLcs(const MBOOL &aEn);

    /**
         *@brief Enable LCE
         *@param[in] aEn : MFALSE - disable, MTRUE - enable
       */
    virtual MVOID EnableLce(const MBOOL &aEn);

    /**
         *@brief Enable RMG
         *@param[in] aEn : MFALSE - disable, MTRUE - enable
       */
    virtual MVOID EnableRmg(const MBOOL &aEn);

    /**
         *@brief Enable RMM
         *@param[in] aEn : MFALSE - disable, MTRUE - enable
       */
    virtual MVOID EnableRmm(const MBOOL &aEn);

   /*
    * @brief: Get Buffer for LCS writing LCSO data.
    * @param[out] spBuf : next writtable buffer for LCS.
    */
   // virtual MINT32 GetBufLCSO(android::sp<IImageBuffer>& spBuf);
#if 0
     /**
         *@brief do vhdr and Get LCEI DMA result for P2 Use.
         *@param[in,out] apLceiDmaInfo : output LCEI_DMA_INFO
         *@param[in] lcsoBuf : lcso output from HW. We just record its mem addr into DMA Info now.
       */
    virtual MVOID DoVHdr(LCEI_DMA_INFO *apLceiDmaInfo,const android::sp<IImageBuffer>& lcsoBuf);
#endif


     //---------------------------------------------------------------------------------------------

    MBOOL mVrWithEis;
    MINT32 mVideoPrvW;
    MINT32 mVideoPrvH;
    MINT32 mPass1OutW;
    MINT32 mPass1OutH;
    MUINT32 mZoomRatio;

    MBOOL mLcsEn;
    MBOOL mLcsD_En;
    MUINT32 mLcsAws;
    MUINT32 mLcsD_Aws;
    MUINT32 mLcs_outW;
    MUINT32 mLcs_outH;
    MUINT32 mLcsD_outW;
    MUINT32 mLcsD_outH;
    MUINT32 mFlareGain;
    MUINT32 mFlareOffset;
    MUINT32 mIsVhdrConfig;

    //sensor
    MUINT32 mSensorTg;
private:

    /**
         *@brief LCSO thread loop
       */
    static MVOID *LcsoThreadLoop(MVOID *arg);

    /**
         *@brief VHdrDrvImp constructor
         *@param[in] aSensorIdx : sensor index
       */
    VHdrDrvImp(const MUINT32 &aSensorIdx);

    /**
         *@brief VHdrDrvImp destructor
       */
    ~VHdrDrvImp() {}

    /**
         *@brief Update LCSO base address
         *@return
         *-VHDR_RETURN_NO_ERROR indicates success, otherwise indicates fail
       */
    MINT32 UpdateLcso();

    /**
         *@brief Update LCSO base address index
         *@return
         *-VHDR_RETURN_NO_ERROR indicates success, otherwise indicates fail
       */
    MINT32 UpdateLcsoIdx();

    /**
         *@brief Create memory by using IMem (Deprecated)
         *@param[in,out] memSize : input already calculated size and will set to alingSize
         *@param[in] bufCnt : how many memory need to be created
         *@param[in,out] bufInfo : pointer to IMEM_BUF_INFO
         *@return
         *-EIS_RETURN_NO_ERROR indicates success, otherwise indicates fail
       */
    MINT32 CreateMemBuf(MUINT32 &memSize,const MUINT32 &bufCnt, IMEM_BUF_INFO *bufInfo);

    /**
         *@brief Destroy memory by using IMem (Deprecated)
         *@param[in] bufCnt : how many memory need to be destroyed
         *@param[in,out] bufInfo : pointer to IMEM_BUF_INFO
         *@return
         *-EIS_RETURN_NO_ERROR indicates success, otherwise indicates fail
       */
    MINT32 DestroyMemBuf(const MUINT32 &bufCnt, IMEM_BUF_INFO *bufInfo);
#if 0
     /**
         *@brief Create continous IImageBuffer and slice it into many small buffers.
         *@param[in] memSize : each slice memory size, will align to L1 cache
         *@param[in] num : how many buffer slice
         *@param[out] spMainImageBuf : continous blob heap for buffer slice allocate.
         *@param[out] spVecSliceBufs : store all slice of sp<IImageBuffer>
         *@return
         *VHDR_RETURN_NO_ERROR indicates success, otherwise indicates fail
       */
    MINT32 CreateMultiMemBuf(MUINT32 memSize, MUINT32 num, android::sp<IImageBuffer>& spMainImageBuf, VecSpImgBuf& spVecSliceBufs);

     /**
         *@brief Destroy continous IImageBuffer its slice buffers.
         *@param[out] spMainImageBuf : continous blob heap for buffer slice allocate. It will be destroy.
         *@param[out] spVecSliceBufs : store all slice of sp<IImageBuffer>. It will be clear.
         *@return
         *VHDR_RETURN_NO_ERROR indicates success, otherwise indicates fail
       */
    MINT32 DestroMultiMemBuf(android::sp<IImageBuffer>& spMainImageBuf, VecSpImgBuf& spVecSliceBufs);
#endif


    /**
         *@brief Convert sensor device  enum to ESensorDev_T type
         *@param[in] aSensorDev : sensor device  enum
         *@return
         *-ESensorDev_T type
       */
    ESensorDev_T ConvertSensorDevType(const MUINT32 &aSensorDev);

    /**
         *@brief Convert sensor TG  enum to ESensorTG_T type
         *@param[in] aSensorTg : sensor Tg  enum
         *@return
         *-ESensorTG_T type
       */
    ESensorTG_T ConvertSensorTgType(const MUINT32 &aSensorTg);

    /**
         *@brief Dump LCSO data
         *@param[in] usr : user name
         *@param[in] addr : LCEO virtual address
       */
    MVOID DumpLcso(const char *usr,const MUINT32 &addr);

    /**
         *@brief Save data to file
         *@param[in] filepath : file path
         *@param[in] addr : virtual address
       */
    MBOOL SaveToFile(char const *filepath,const MUINTPTR &addr);

    /**
         *@brief Set state
         *@param[in] aState : state
       */
    MVOID SetVHdrState(const VHDR_STATE_ENUM &aState);

    /**
         *@brief Get state
         *@return
         *-state
       */
    VHDR_STATE_ENUM GetVHdrState();

    /**
         *@brief Change thread setting
         *@param[in] userName : thread name
       */
    MVOID ChangeThreadSetting(char const *userName);

    /**
         *@brief Get LCS input width and height
         *@param[in/out] w : lcs width
         *@param[in/out] h : lcs height
       */
    MVOID GetLCSInputDimen(MUINT32& w, MUINT32& h);

    /***************************************************************************************/

    volatile MINT32 mUsers;
    mutable Mutex mLock;
    mutable Mutex mIdxLock;
    mutable Mutex mQueLock;

    // INormalPipe
    INormalPipe *m_pNormalPipe;

    // ISP driver object
#if VHDR_RUN_LEGACY
    IspDrv *m_pISPDrvObj;
    IspDrv *m_pISPVirtDrv;  // for command queue

    //IMem object
    IMemDrv *m_pIMemDrv;
#endif
    IMEM_BUF_INFO mLcso_memInfo;
    IMEM_BUF_INFO mLcsoD_memInfo;
    MUINT32 mLcso_addrIdx;
    MUINT32 mLcso_virAddr;
    MUINT32 mLcso_phyAddr;
    MUINT32 mLcsoD_addrIdx;
    MUINT32 mLcsoD_virAddr;
    MUINT32 mLcsoD_phyAddr;

    //member variable
    MUINT32 mSensorIdx;

    // sensor
    MUINT32 mSensorDev;

    // state
    VHDR_STATE_ENUM mState;
#if VHDR_RUN_LEGACY
    // thread
    pthread_t mLcsoThread;
    sem_t mLcsoSem;
    sem_t mLcsoSemEnd;
    sem_t mTsSem;
    sem_t mUninitSem;
#endif
    // data queue
    queue<LCSO_DATA> mLcsoData;
    queue<LCSO_DATA> mLcsoD_Data;
    queue<LCSO_DATA> mLcsoData4P2;
    queue<LCSO_DATA> mLcsoD_Data4P2;




    // Pass1 HW setting callback
    VHdrP1Cb *mpVHdrP1Cb;
};

#endif

