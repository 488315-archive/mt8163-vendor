/* Copyright Statement:
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein is
 * confidential and proprietary to MediaTek Inc. and/or its licensors. Without
 * the prior written permission of MediaTek inc. and/or its licensors, any
 * reproduction, modification, use or disclosure of MediaTek Software, and
 * information contained herein, in whole or in part, shall be strictly
 * prohibited.
 *
 * MediaTek Inc. (C) 2010. All rights reserved.
 *
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE")
 * RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO RECEIVER
 * ON AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL
 * WARRANTIES, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR
 * NONINFRINGEMENT. NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH
 * RESPECT TO THE SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY,
 * INCORPORATED IN, OR SUPPLIED WITH THE MEDIATEK SOFTWARE, AND RECEIVER AGREES
 * TO LOOK ONLY TO SUCH THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO.
 * RECEIVER EXPRESSLY ACKNOWLEDGES THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO
 * OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES CONTAINED IN MEDIATEK
 * SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK SOFTWARE
 * RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S
 * ENTIRE AND CUMULATIVE LIABILITY WITH RESPECT TO THE MEDIATEK SOFTWARE
 * RELEASED HEREUNDER WILL BE, AT MEDIATEK'S OPTION, TO REVISE OR REPLACE THE
 * MEDIATEK SOFTWARE AT ISSUE, OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE
 * CHARGE PAID BY RECEIVER TO MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 *
 * The following software/firmware and/or related documentation ("MediaTek
 * Software") have been modified by MediaTek Inc. All revisions are subject to
 * any receiver's applicable license agreements with MediaTek Inc.
 */


#include <imageio/ispio_pipe_ports.h>
#include <featurePipe/vsdof/IDepthMapPipe.h>


#include "DepthMapPipeUtils.h"

namespace NSCam {
namespace NSCamFeature {
namespace NSFeaturePipe {

static bool do_mkdir(char const*const path, uint_t const mode)
{
    struct stat st;
    //
    if  ( 0 != ::stat(path, &st) )
    {
        //  Directory does not exist.
        if  ( 0 != ::mkdir(path, mode) && EEXIST != errno )
        {
            MY_LOGE("fail to mkdir [%s]: %d[%s]", path, errno, ::strerror(errno));
            return  false;
        }
    }
    else if ( ! S_ISDIR(st.st_mode) )
    {
        MY_LOGE("!S_ISDIR");
        return  false;
    }
    //
    return  true;
}



MVOID
TemplateStatistics::
inherit(TemplateStatistics& stat)
{
    mFrameIDToLocMap_mvIn = stat.mFrameIDToLocMap_mvIn;
    mFrameIDToLocMap_mvOut = stat.mFrameIDToLocMap_mvOut;
    mFrameIDToLocMap_mvCrop = stat.mFrameIDToLocMap_mvCrop;

    mvInTotalSize = stat.mvInTotalSize;
    mvOutTotalSize = stat.mvOutTotalSize;
    mvCropTotalSize = stat.mvCropTotalSize;
}

MVOID
TemplateStatistics::
setActiveFrame(MUINT32 frameID)
{
    //MY_LOGD("setActiveFrame frameID=%d", frameID);
    if(mFrameIDToLocMap_mvIn.indexOfKey(frameID) < 0)
        mFrameIDToLocMap_mvIn.add(frameID, TemplateLocStat());
    if(mFrameIDToLocMap_mvOut.indexOfKey(frameID) < 0)
        mFrameIDToLocMap_mvOut.add(frameID, TemplateLocStat());
    if(mFrameIDToLocMap_mvCrop.indexOfKey(frameID) < 0)
        mFrameIDToLocMap_mvCrop.add(frameID, TemplateLocStat());

    miActiveFrameID = frameID;
}

MVOID
TemplateStatistics::
addCrop(eCropGroup groupID)
{
    if(miActiveFrameID<0)
    {
        MY_LOGE("Not valid miActiveFrameID=%d", miActiveFrameID);
        return;
    }
    // store loc information to stats
    TemplateLocStat& statItem = mFrameIDToLocMap_mvCrop.editValueFor(miActiveFrameID);
    statItem.mKeyToLocMap.add((MUINT32)groupID, mvCropTotalSize++);

    //MY_LOGD("TemplateStatistics miFrameID=%d Crop Loc: groupID=%d  Loc=%d mvCropTotalSize=%d", miActiveFrameID, groupID, mFrameIDToLocMap_mvCrop.valueFor(miActiveFrameID).mKeyToLocMap.valueFor((MUINT32)groupID), mvCropTotalSize);
}

MVOID
TemplateStatistics::
addMvIn(MUINT32 portIdx)
{
    if(miActiveFrameID<0)
    {
        MY_LOGE("Not valid miActiveFrameID=%d", miActiveFrameID);
        return;
    }
    // store loc information to stats
    TemplateLocStat& statItem = mFrameIDToLocMap_mvIn.editValueFor(miActiveFrameID);
    statItem.mKeyToLocMap.add(portIdx, mvInTotalSize++);
    //MY_LOGD("TemplateStatistics miFrameID=%d, add mvIn, portID.index=%d  Loc=%d mvInTotalSize=%d ", miActiveFrameID, portIdx, mFrameIDToLocMap_mvIn.valueFor(miActiveFrameID).mKeyToLocMap.valueFor(portIdx), mvInTotalSize);

}

MVOID
TemplateStatistics::addMvOut(MUINT32 portIdx)
{
    if(miActiveFrameID<0)
    {
        MY_LOGE("Not valid miActiveFrameID=%d", miActiveFrameID);
        return;
    }
    // store loc information to stats
    TemplateLocStat& statItem = mFrameIDToLocMap_mvOut.editValueFor(miActiveFrameID);
    statItem.mKeyToLocMap.add(portIdx, mvOutTotalSize++);
    //MY_LOGD("TemplateStatistics miFrameID=%d, add mvOut, portID.index=%d  Loc=%d mvOutTotalSize=%d", miActiveFrameID, portIdx, mFrameIDToLocMap_mvOut.valueFor(miActiveFrameID).mKeyToLocMap.valueFor(portIdx), mvOutTotalSize);

}


QParamTemplateGenerator::
QParamTemplateGenerator()
{
}

QParamTemplateGenerator::
QParamTemplateGenerator(MUINT32 frameID, ENormalStreamTag streamTag, TemplateStatistics* pLearningStat)
: miFrameID(frameID), mStreamTag(streamTag)
{
    if(pLearningStat != NULL)
    {
        mStats.inherit(*pLearningStat);
    }

    mStats.setActiveFrame(frameID);

}

QParamTemplateGenerator&
QParamTemplateGenerator::
addCrop(eCropGroup groupID, MPoint startLoc, MSize cropSize, MSize resizeDst)
{
    MCrpRsInfo cropInfo;
    cropInfo.mGroupID = (MUINT32) groupID;
    cropInfo.mCropRect.p_fractional.x=0;
    cropInfo.mCropRect.p_fractional.y=0;
    cropInfo.mCropRect.p_integral.x=startLoc.x;
    cropInfo.mCropRect.p_integral.y=startLoc.y;
    cropInfo.mCropRect.s=cropSize;
    cropInfo.mResizeDst=resizeDst;
    cropInfo.mFrameGroup = miFrameID;
    mvCropRsInfo.push_back(cropInfo);
    //MY_LOGD("addCrop miFrameID=%d, groupID=%d : mvCropRsInfo size=%d", miFrameID, groupID, mvCropRsInfo.size());
    // store loc information to stats
    mStats.addCrop(groupID);


    return *this;
}

QParamTemplateGenerator&
QParamTemplateGenerator::addInput(NSCam::NSIoPipe::PortID portID)
{
    Input src;
    src.mPortID = portID;
    src.mPortID.group = miFrameID;
    mvIn.push_back(src);
    // store loc information to stats
    mStats.addMvIn(portID.index);
    //MY_LOGD("addInput miFrameID=%d portID=%d : mvIn size=%d", miFrameID, portID.index, mvIn.size());

    return *this;
}

QParamTemplateGenerator &
QParamTemplateGenerator::addOutput(NSCam::NSIoPipe::PortID portID, MINT32 transform)
{
    Output out;
    out.mPortID = portID;
    out.mPortID.group = miFrameID;
    out.mTransform = transform;
    mvOut.push_back(out);

    // store loc information to stats
    mStats.addMvOut(portID.index);
    //MY_LOGD("addOutput miFrameID=%d  portID=%d : mvOut size=%d", miFrameID, portID.index, mvOut.size());

    return *this;
}

QParamTemplateGenerator&
QParamTemplateGenerator::addModuleInfo(MUINT32 moduleTag, MVOID* moduleStruct)
{
    ModuleInfo moduleInfo;
    moduleInfo.moduleTag = moduleTag;
    moduleInfo.frameGroup = miFrameID;
    moduleInfo.moduleStruct = moduleStruct;
    mvModuleData.push_back(moduleInfo);
    return *this;
}

MBOOL
QParamTemplateGenerator::generate(QParams& rOutputParams, TemplateStatistics& rStats)
{
    rOutputParams.mvStreamTag.push_back(mStreamTag);
    rOutputParams.mvIn.appendVector(mvIn);
    rOutputParams.mvOut.appendVector(mvOut);
    rOutputParams.mvCropRsInfo.appendVector(mvCropRsInfo);
    rOutputParams.mvModuleData.appendVector(mvModuleData);

    if(checkValid())
    {
        rStats = mStats;
        return MTRUE;
    }
    else
        return MFALSE;
}

MBOOL
QParamTemplateGenerator::checkValid()
{
    bool bAllSuccess = MTRUE;
    // for each mvIn find its mvOut
    for(Input input: mvIn)
    {
        bool bSuccess = MFALSE;
        for(Output output : mvOut)
        {
            if(input.mPortID.group == output.mPortID.group)
            {
                bSuccess = MTRUE;
                break;
            }
        }
        if(!bSuccess)
        {
            MY_LOGE("FrameID:%d has input buffer, but has no output buffer!!", input.mPortID.group);
            bAllSuccess = MFALSE;
        }
    }

    // for each mvOut find its mvIn
    for(Output output: mvOut)
    {
        bool bSuccess = MFALSE;
        for(Input input : mvIn)
        {
            if(input.mPortID.group == output.mPortID.group)
            {
                bSuccess = MTRUE;
                break;
            }
        }
        if(!bSuccess)
        {
            MY_LOGE("FrameID:%d has output buffer, but has no input buffer!!", output.mPortID.group);
            bAllSuccess = MFALSE;
        }
    }

    // for each mvOut, check the corresponding crop setting is ready
    for(Output output: mvOut)
    {
        MINT32 findTarget;
        if(output.mPortID.index == NSImageio::NSIspio::EPortIndex_WROTO)
            findTarget = (MINT32)eCROP_WROT;
        else if(output.mPortID.index == NSImageio::NSIspio::EPortIndex_WDMAO)
            findTarget = (MINT32)eCROP_WDMA;
        else if(output.mPortID.index == NSImageio::NSIspio::EPortIndex_IMG2O)
            findTarget = (MINT32)eCROP_CRZ;
        else
            continue;

        bool bIsFind = MFALSE;
        for(MCrpRsInfo rsInfo : mvCropRsInfo)
        {
            if(rsInfo.mFrameGroup == output.mPortID.group && rsInfo.mGroupID == findTarget)
            {
                bIsFind = MTRUE;
                break;
            }
        }
        if(!bIsFind)
        {
            MY_LOGE("FrameID:%d has output buffer with portID=%d, but has no the needed crop:%d",
                    output.mPortID.group, output.mPortID.index, findTarget);
            bAllSuccess = MFALSE;
        }
    }

    return bAllSuccess;
}

QParamTemplateFiller::
QParamTemplateFiller(QParams& target, const TemplateStatistics& tempStats)
: mTargetQParam(target), mbSuccess(MTRUE), mTemplStats(tempStats)
{}

QParamTemplateFiller&
QParamTemplateFiller::insertTuningBuf(MUINT frameID, MVOID* pTuningBuf)
{
    ssize_t res = mTargetQParam.mvTuningData.insertAt(pTuningBuf, frameID);
    if(res<0)
    {
        MY_LOGE("QParamTemplateFiller: insertTuningBuf error, BAD_INDEX:%d  NO_MEMORY:%d",
                    res == BAD_INDEX, res==NO_MEMORY);

        mbSuccess = MFALSE;
    }
    return *this;
}

QParamTemplateFiller&
QParamTemplateFiller::delOutputPort(MUINT frameID, NSCam::NSIoPipe::PortID portID, eCropGroup cropGID)
{
    // output port
    if(mTemplStats.mFrameIDToLocMap_mvOut.indexOfKey(frameID) < 0)
    {
        return *this;;
    }


    TemplateLocStat mvOutStats = mTemplStats.mFrameIDToLocMap_mvOut.valueFor(frameID);
    if(mvOutStats.mKeyToLocMap.indexOfKey(portID.index) < 0)
    {
        return *this;;
    }


    // store the location of delete out port
    mDelOutPortLocVec.add(mvOutStats.mKeyToLocMap.valueFor(portID.index));

    // crop
    if(mTemplStats.mFrameIDToLocMap_mvCrop.indexOfKey(frameID) < 0)
    {
        return *this;;
    }


    TemplateLocStat mvCropStats = mTemplStats.mFrameIDToLocMap_mvCrop.valueFor(frameID);
    if(mvCropStats.mKeyToLocMap.indexOfKey(cropGID) < 0)
    {
        return *this;;
    }

    // store the location of delete crop GID
    mDelCropLocVec.add(mvCropStats.mKeyToLocMap.valueFor(cropGID));

    return *this;
}

QParamTemplateFiller&
QParamTemplateFiller::insertInputBuf(MUINT frameID, NSCam::NSIoPipe::PortID portID, sp<EffectFrameInfo> pFrameInfo)
{
    sp<IImageBuffer> pImgBuf;
    pFrameInfo->getFrameBuffer(pImgBuf);
    this->insertInputBuf(frameID, portID, pImgBuf.get());
    return *this;
}

QParamTemplateFiller&
QParamTemplateFiller::insertInputBuf(MUINT frameID, NSCam::NSIoPipe::PortID portID, IImageBuffer* pImgBuf)
{
    //MY_LOGD("insertInputBuf===> frameID=%d, portID=%d", frameID, portID.index);
    int size = mTargetQParam.mvIn.size();
    if(size == 0)
    {
        MY_LOGE("QParamTemplateFiller: insertInputBuf error, not exist any mvIn");
        mbSuccess = MFALSE;
        return *this;
    }

    if(mTemplStats.mFrameIDToLocMap_mvIn.indexOfKey(frameID) < 0 ||
        mTemplStats.mFrameIDToLocMap_mvIn.valueFor(frameID).mKeyToLocMap.indexOfKey(portID.index) < 0)
    {
        MY_LOGE("QParamTemplateFiller: insertInputBuf error, cannot find the correct position, frameID=%d portID=%d ",
                    frameID, portID.index);
        mbSuccess = MFALSE;
        return *this;
    }

    int index = mTemplStats.mFrameIDToLocMap_mvIn.valueFor(frameID).mKeyToLocMap.valueFor(portID.index);

    //MY_LOGD("insertInputBuf: frameID=%d  portID.index=%d  index=%d pImgBuf=%x　　size=%d", frameID, portID.index, index, pImgBuf, mTargetQParam.mvIn.size());

    mTargetQParam.mvIn.editItemAt(index).mBuffer = pImgBuf;

    //MY_LOGD("insertOutputBuf: result check =%x", mTargetQParam.mvIn[index].mBuffer);

    return *this;


}

QParamTemplateFiller&
QParamTemplateFiller::insertOutputBuf(MUINT frameID, NSCam::NSIoPipe::PortID portID, sp<EffectFrameInfo> pFrameInfo)
{
    sp<IImageBuffer> pImgBuf;
    pFrameInfo->getFrameBuffer(pImgBuf);
    this->insertOutputBuf(frameID, portID, pImgBuf.get());
    return *this;
}


QParamTemplateFiller&
QParamTemplateFiller::insertOutputBuf(MUINT frameID, NSCam::NSIoPipe::PortID portID, IImageBuffer* pImgBuf)
{
    //MY_LOGD("insertOutputBuf===> frameID=%d, portID=%d", frameID, portID.index);
    int size = mTargetQParam.mvOut.size();
    if(size == 0)
    {
        MY_LOGE("QParamTemplateFiller: insertOutputBuf error, not exist any mvOut");
        mbSuccess = MFALSE;
        return *this;
    }

    if(mTemplStats.mFrameIDToLocMap_mvOut.indexOfKey(frameID) < 0 ||
        mTemplStats.mFrameIDToLocMap_mvOut.valueFor(frameID).mKeyToLocMap.indexOfKey(portID.index) < 0)
    {
        MY_LOGE("QParamTemplateFiller: insertOutputBuf error, cannot find the correct position, frameID=%d portID=%d ",
                    frameID, portID.index);
        mbSuccess = MFALSE;
        return *this;
    }

    int index = mTemplStats.mFrameIDToLocMap_mvOut.valueFor(frameID).mKeyToLocMap.valueFor(portID.index);

    //MY_LOGD("insertOutputBuf: frameID=%d  portID.index=%d  index=%d pImgBuf=%x", frameID, portID.index, index, pImgBuf);

    mTargetQParam.mvOut.editItemAt(index).mBuffer = pImgBuf;

    //MY_LOGD("insertOutputBuf: result check =%x", mTargetQParam.mvOut[index].mBuffer);


    return *this;
}

QParamTemplateFiller&
QParamTemplateFiller::setCrop(MUINT frameID, eCropGroup groupID, MPoint startLoc, MSize cropSize, MSize resizeDst)
{

    int size = mTargetQParam.mvCropRsInfo.size();
    if(size == 0)
    {
        MY_LOGE("QParamTemplateFiller: setCrop error, not exist any mvCropRsInfo");
        mbSuccess = MFALSE;
        return *this;
    }

    if(mTemplStats.mFrameIDToLocMap_mvCrop.indexOfKey(frameID) < 0 ||
        mTemplStats.mFrameIDToLocMap_mvCrop.valueFor(frameID).mKeyToLocMap.indexOfKey((MUINT32)groupID) < 0)
    {
        MY_LOGE("QParamTemplateFiller: setCrop error, cannot find the correct position, frameID=%d groupID=%d ",
                    frameID, groupID);
        mbSuccess = MFALSE;
        return *this;
    }
    int index = mTemplStats.mFrameIDToLocMap_mvCrop.valueFor(frameID).mKeyToLocMap.valueFor((MUINT32)groupID);

    MCrpRsInfo& cropInfo = mTargetQParam.mvCropRsInfo.editItemAt(index);
    cropInfo.mGroupID = (MUINT32) groupID;
    cropInfo.mCropRect.p_fractional.x=0;
    cropInfo.mCropRect.p_fractional.y=0;
    cropInfo.mCropRect.p_integral.x=startLoc.x;
    cropInfo.mCropRect.p_integral.y=startLoc.y;
    cropInfo.mCropRect.s=cropSize;
    cropInfo.mResizeDst=resizeDst;
    cropInfo.mFrameGroup = frameID;

    return *this;
}

static int comparator(const MUINT* lhs, const MUINT* rhs)
{
    return *lhs < *rhs;
}

MBOOL
QParamTemplateFiller::validate()
{
    if(!mbSuccess)
        return MFALSE;

    mDelOutPortLocVec.sort(comparator);
    mDelCropLocVec.sort(comparator);

    for(size_t index=0;index<mDelOutPortLocVec.size();++index)
    {
        //MY_LOGD("sort~~~~index=%d ~~~~~~~~~out port loc~~~~~~~~~~~%d", index, mDelOutPortLocVec[index]);
        mTargetQParam.mvOut.removeAt(mDelOutPortLocVec[index]);
    }

    for(size_t index=0;index<mDelCropLocVec.size();++index)
    {
        //MY_LOGD("sort~~~~index=%d ~~~~~~~~crop loc~~~~~~~~~~~~%d", index, mDelCropLocVec[index]);
        mTargetQParam.mvCropRsInfo.removeAt(mDelCropLocVec[index]);
    }

    return mbSuccess;
}

void SetDefaultTuning(dip_x_reg_t* pIspReg, MUINT32* tuningBuf, tuning_tag tag, int enFgMode)
{
    printf("SetDefaultTuning (%d), enFgMode(%d)n", tag, enFgMode);
    MUINT32 fgModeRegBit = (enFgMode&0x01)<<10;
    switch(tag)
    {
        case tuning_tag_G2G:
            pIspReg->DIP_X_G2G_CNV_1.Raw = 0x00000200;
            pIspReg->DIP_X_G2G_CNV_2.Raw = 0x00000000;
            pIspReg->DIP_X_G2G_CNV_3.Raw = 0x02000000;
            pIspReg->DIP_X_G2G_CNV_4.Raw = 0x00000000;
            pIspReg->DIP_X_G2G_CNV_5.Raw = 0x00000000;
            pIspReg->DIP_X_G2G_CNV_6.Raw = 0x00000200;
            pIspReg->DIP_X_G2G_CTRL.Raw = 0x00000009;
            pIspReg->DIP_X_CTL_RGB_EN.Bits.G2G_EN = 1;
            break;
        case tuning_tag_G2C:
            pIspReg->DIP_X_G2C_CONV_0A.Raw = 0x012D0099;
            pIspReg->DIP_X_G2C_CONV_0B.Raw = 0x0000003A;
            pIspReg->DIP_X_G2C_CONV_1A.Raw = 0x075607AA;
            pIspReg->DIP_X_G2C_CONV_1B.Raw = 0x00000100;
            pIspReg->DIP_X_G2C_CONV_2A.Raw = 0x072A0100;
            pIspReg->DIP_X_G2C_CONV_2B.Raw = 0x000007D6;
            pIspReg->DIP_X_G2C_SHADE_CON_1.Raw = 0x0118000E;
            pIspReg->DIP_X_G2C_SHADE_CON_2.Raw = 0x0074B740;
            pIspReg->DIP_X_G2C_SHADE_CON_3.Raw = 0x00000133;
            pIspReg->DIP_X_G2C_SHADE_TAR.Raw = 0x079F0A5A;
            pIspReg->DIP_X_G2C_SHADE_SP.Raw = 0x00000000;
            pIspReg->DIP_X_G2C_CFC_CON_1.Raw = 0x03f70080;
            pIspReg->DIP_X_G2C_CFC_CON_2.Raw = 0x29485294;
            pIspReg->DIP_X_CTL_YUV_EN.Bits.G2C_EN = 1;
            break;
        case tuning_tag_GGM:
            //tuningBuf[0x1000>>2] = 0x08000800;
            tuningBuf[0x00001000>>2] = 0x08000800; /* 0x15023000: DIP_A_GGM_LUT_RB[0] */
            tuningBuf[0x00001004>>2] = 0x08020802; /* 0x15023004: DIP_A_GGM_LUT_RB[1] */
            tuningBuf[0x00001008>>2] = 0x08040804; /* 0x15023008: DIP_A_GGM_LUT_RB[2] */
            tuningBuf[0x0000100C>>2] = 0x08060806; /* 0x1502300C: DIP_A_GGM_LUT_RB[3] */
            tuningBuf[0x00001010>>2] = 0x08080808; /* 0x15023010: DIP_A_GGM_LUT_RB[4] */
            tuningBuf[0x00001014>>2] = 0x080A080A; /* 0x15023014: DIP_A_GGM_LUT_RB[5] */
            tuningBuf[0x00001018>>2] = 0x080C080C; /* 0x15023018: DIP_A_GGM_LUT_RB[6] */
            tuningBuf[0x0000101C>>2] = 0x080E080E; /* 0x1502301C: DIP_A_GGM_LUT_RB[7] */
            tuningBuf[0x00001020>>2] = 0x08100810; /* 0x15023020: DIP_A_GGM_LUT_RB[8] */
            tuningBuf[0x00001024>>2] = 0x08120812; /* 0x15023024: DIP_A_GGM_LUT_RB[9] */
            tuningBuf[0x00001028>>2] = 0x08140814; /* 0x15023028: DIP_A_GGM_LUT_RB[10] */
            tuningBuf[0x0000102C>>2] = 0x08160816; /* 0x1502302C: DIP_A_GGM_LUT_RB[11] */
            tuningBuf[0x00001030>>2] = 0x08180818; /* 0x15023030: DIP_A_GGM_LUT_RB[12] */
            tuningBuf[0x00001034>>2] = 0x081A081A; /* 0x15023034: DIP_A_GGM_LUT_RB[13] */
            tuningBuf[0x00001038>>2] = 0x081C081C; /* 0x15023038: DIP_A_GGM_LUT_RB[14] */
            tuningBuf[0x0000103C>>2] = 0x081E081E; /* 0x1502303C: DIP_A_GGM_LUT_RB[15] */
            tuningBuf[0x00001040>>2] = 0x08200820; /* 0x15023040: DIP_A_GGM_LUT_RB[16] */
            tuningBuf[0x00001044>>2] = 0x08220822; /* 0x15023044: DIP_A_GGM_LUT_RB[17] */
            tuningBuf[0x00001048>>2] = 0x08240824; /* 0x15023048: DIP_A_GGM_LUT_RB[18] */
            tuningBuf[0x0000104C>>2] = 0x08260826; /* 0x1502304C: DIP_A_GGM_LUT_RB[19] */
            tuningBuf[0x00001050>>2] = 0x08280828; /* 0x15023050: DIP_A_GGM_LUT_RB[20] */
            tuningBuf[0x00001054>>2] = 0x082A082A; /* 0x15023054: DIP_A_GGM_LUT_RB[21] */
            tuningBuf[0x00001058>>2] = 0x082C082C; /* 0x15023058: DIP_A_GGM_LUT_RB[22] */
            tuningBuf[0x0000105C>>2] = 0x082E082E; /* 0x1502305C: DIP_A_GGM_LUT_RB[23] */
            tuningBuf[0x00001060>>2] = 0x08300830; /* 0x15023060: DIP_A_GGM_LUT_RB[24] */
            tuningBuf[0x00001064>>2] = 0x08320832; /* 0x15023064: DIP_A_GGM_LUT_RB[25] */
            tuningBuf[0x00001068>>2] = 0x08340834; /* 0x15023068: DIP_A_GGM_LUT_RB[26] */
            tuningBuf[0x0000106C>>2] = 0x08360836; /* 0x1502306C: DIP_A_GGM_LUT_RB[27] */
            tuningBuf[0x00001070>>2] = 0x08380838; /* 0x15023070: DIP_A_GGM_LUT_RB[28] */
            tuningBuf[0x00001074>>2] = 0x083A083A; /* 0x15023074: DIP_A_GGM_LUT_RB[29] */
            tuningBuf[0x00001078>>2] = 0x083C083C; /* 0x15023078: DIP_A_GGM_LUT_RB[30] */
            tuningBuf[0x0000107C>>2] = 0x083E083E; /* 0x1502307C: DIP_A_GGM_LUT_RB[31] */
            tuningBuf[0x00001080>>2] = 0x08400840; /* 0x15023080: DIP_A_GGM_LUT_RB[32] */
            tuningBuf[0x00001084>>2] = 0x08420842; /* 0x15023084: DIP_A_GGM_LUT_RB[33] */
            tuningBuf[0x00001088>>2] = 0x08440844; /* 0x15023088: DIP_A_GGM_LUT_RB[34] */
            tuningBuf[0x0000108C>>2] = 0x08460846; /* 0x1502308C: DIP_A_GGM_LUT_RB[35] */
            tuningBuf[0x00001090>>2] = 0x08480848; /* 0x15023090: DIP_A_GGM_LUT_RB[36] */
            tuningBuf[0x00001094>>2] = 0x084A084A; /* 0x15023094: DIP_A_GGM_LUT_RB[37] */
            tuningBuf[0x00001098>>2] = 0x084C084C; /* 0x15023098: DIP_A_GGM_LUT_RB[38] */
            tuningBuf[0x0000109C>>2] = 0x084E084E; /* 0x1502309C: DIP_A_GGM_LUT_RB[39] */
            tuningBuf[0x000010A0>>2] = 0x08500850; /* 0x150230A0: DIP_A_GGM_LUT_RB[40] */
            tuningBuf[0x000010A4>>2] = 0x08520852; /* 0x150230A4: DIP_A_GGM_LUT_RB[41] */
            tuningBuf[0x000010A8>>2] = 0x08540854; /* 0x150230A8: DIP_A_GGM_LUT_RB[42] */
            tuningBuf[0x000010AC>>2] = 0x08560856; /* 0x150230AC: DIP_A_GGM_LUT_RB[43] */
            tuningBuf[0x000010B0>>2] = 0x08580858; /* 0x150230B0: DIP_A_GGM_LUT_RB[44] */
            tuningBuf[0x000010B4>>2] = 0x085A085A; /* 0x150230B4: DIP_A_GGM_LUT_RB[45] */
            tuningBuf[0x000010B8>>2] = 0x085C085C; /* 0x150230B8: DIP_A_GGM_LUT_RB[46] */
            tuningBuf[0x000010BC>>2] = 0x085E085E; /* 0x150230BC: DIP_A_GGM_LUT_RB[47] */
            tuningBuf[0x000010C0>>2] = 0x08600860; /* 0x150230C0: DIP_A_GGM_LUT_RB[48] */
            tuningBuf[0x000010C4>>2] = 0x08620862; /* 0x150230C4: DIP_A_GGM_LUT_RB[49] */
            tuningBuf[0x000010C8>>2] = 0x08640864; /* 0x150230C8: DIP_A_GGM_LUT_RB[50] */
            tuningBuf[0x000010CC>>2] = 0x08660866; /* 0x150230CC: DIP_A_GGM_LUT_RB[51] */
            tuningBuf[0x000010D0>>2] = 0x08680868; /* 0x150230D0: DIP_A_GGM_LUT_RB[52] */
            tuningBuf[0x000010D4>>2] = 0x086A086A; /* 0x150230D4: DIP_A_GGM_LUT_RB[53] */
            tuningBuf[0x000010D8>>2] = 0x086C086C; /* 0x150230D8: DIP_A_GGM_LUT_RB[54] */
            tuningBuf[0x000010DC>>2] = 0x086E086E; /* 0x150230DC: DIP_A_GGM_LUT_RB[55] */
            tuningBuf[0x000010E0>>2] = 0x08700870; /* 0x150230E0: DIP_A_GGM_LUT_RB[56] */
            tuningBuf[0x000010E4>>2] = 0x08720872; /* 0x150230E4: DIP_A_GGM_LUT_RB[57] */
            tuningBuf[0x000010E8>>2] = 0x08740874; /* 0x150230E8: DIP_A_GGM_LUT_RB[58] */
            tuningBuf[0x000010EC>>2] = 0x08760876; /* 0x150230EC: DIP_A_GGM_LUT_RB[59] */
            tuningBuf[0x000010F0>>2] = 0x08780878; /* 0x150230F0: DIP_A_GGM_LUT_RB[60] */
            tuningBuf[0x000010F4>>2] = 0x087A087A; /* 0x150230F4: DIP_A_GGM_LUT_RB[61] */
            tuningBuf[0x000010F8>>2] = 0x087C087C; /* 0x150230F8: DIP_A_GGM_LUT_RB[62] */
            tuningBuf[0x000010FC>>2] = 0x087E087E; /* 0x150230FC: DIP_A_GGM_LUT_RB[63] */
            tuningBuf[0x00001100>>2] = 0x10801080; /* 0x15023100: DIP_A_GGM_LUT_RB[64] */
            tuningBuf[0x00001104>>2] = 0x10841084; /* 0x15023104: DIP_A_GGM_LUT_RB[65] */
            tuningBuf[0x00001108>>2] = 0x10881088; /* 0x15023108: DIP_A_GGM_LUT_RB[66] */
            tuningBuf[0x0000110C>>2] = 0x108C108C; /* 0x1502310C: DIP_A_GGM_LUT_RB[67] */
            tuningBuf[0x00001110>>2] = 0x10901090; /* 0x15023110: DIP_A_GGM_LUT_RB[68] */
            tuningBuf[0x00001114>>2] = 0x10941094; /* 0x15023114: DIP_A_GGM_LUT_RB[69] */
            tuningBuf[0x00001118>>2] = 0x10981098; /* 0x15023118: DIP_A_GGM_LUT_RB[70] */
            tuningBuf[0x0000111C>>2] = 0x109C109C; /* 0x1502311C: DIP_A_GGM_LUT_RB[71] */
            tuningBuf[0x00001120>>2] = 0x10A010A0; /* 0x15023120: DIP_A_GGM_LUT_RB[72] */
            tuningBuf[0x00001124>>2] = 0x10A410A4; /* 0x15023124: DIP_A_GGM_LUT_RB[73] */
            tuningBuf[0x00001128>>2] = 0x10A810A8; /* 0x15023128: DIP_A_GGM_LUT_RB[74] */
            tuningBuf[0x0000112C>>2] = 0x10AC10AC; /* 0x1502312C: DIP_A_GGM_LUT_RB[75] */
            tuningBuf[0x00001130>>2] = 0x10B010B0; /* 0x15023130: DIP_A_GGM_LUT_RB[76] */
            tuningBuf[0x00001134>>2] = 0x10B410B4; /* 0x15023134: DIP_A_GGM_LUT_RB[77] */
            tuningBuf[0x00001138>>2] = 0x10B810B8; /* 0x15023138: DIP_A_GGM_LUT_RB[78] */
            tuningBuf[0x0000113C>>2] = 0x10BC10BC; /* 0x1502313C: DIP_A_GGM_LUT_RB[79] */
            tuningBuf[0x00001140>>2] = 0x10C010C0; /* 0x15023140: DIP_A_GGM_LUT_RB[80] */
            tuningBuf[0x00001144>>2] = 0x10C410C4; /* 0x15023144: DIP_A_GGM_LUT_RB[81] */
            tuningBuf[0x00001148>>2] = 0x10C810C8; /* 0x15023148: DIP_A_GGM_LUT_RB[82] */
            tuningBuf[0x0000114C>>2] = 0x10CC10CC; /* 0x1502314C: DIP_A_GGM_LUT_RB[83] */
            tuningBuf[0x00001150>>2] = 0x10D010D0; /* 0x15023150: DIP_A_GGM_LUT_RB[84] */
            tuningBuf[0x00001154>>2] = 0x10D410D4; /* 0x15023154: DIP_A_GGM_LUT_RB[85] */
            tuningBuf[0x00001158>>2] = 0x10D810D8; /* 0x15023158: DIP_A_GGM_LUT_RB[86] */
            tuningBuf[0x0000115C>>2] = 0x10DC10DC; /* 0x1502315C: DIP_A_GGM_LUT_RB[87] */
            tuningBuf[0x00001160>>2] = 0x10E010E0; /* 0x15023160: DIP_A_GGM_LUT_RB[88] */
            tuningBuf[0x00001164>>2] = 0x10E410E4; /* 0x15023164: DIP_A_GGM_LUT_RB[89] */
            tuningBuf[0x00001168>>2] = 0x10E810E8; /* 0x15023168: DIP_A_GGM_LUT_RB[90] */
            tuningBuf[0x0000116C>>2] = 0x10EC10EC; /* 0x1502316C: DIP_A_GGM_LUT_RB[91] */
            tuningBuf[0x00001170>>2] = 0x10F010F0; /* 0x15023170: DIP_A_GGM_LUT_RB[92] */
            tuningBuf[0x00001174>>2] = 0x10F410F4; /* 0x15023174: DIP_A_GGM_LUT_RB[93] */
            tuningBuf[0x00001178>>2] = 0x10F810F8; /* 0x15023178: DIP_A_GGM_LUT_RB[94] */
            tuningBuf[0x0000117C>>2] = 0x10FC10FC; /* 0x1502317C: DIP_A_GGM_LUT_RB[95] */
            tuningBuf[0x00001180>>2] = 0x21002100; /* 0x15023180: DIP_A_GGM_LUT_RB[96] */
            tuningBuf[0x00001184>>2] = 0x21082108; /* 0x15023184: DIP_A_GGM_LUT_RB[97] */
            tuningBuf[0x00001188>>2] = 0x21102110; /* 0x15023188: DIP_A_GGM_LUT_RB[98] */
            tuningBuf[0x0000118C>>2] = 0x21182118; /* 0x1502318C: DIP_A_GGM_LUT_RB[99] */
            tuningBuf[0x00001190>>2] = 0x21202120; /* 0x15023190: DIP_A_GGM_LUT_RB[100] */
            tuningBuf[0x00001194>>2] = 0x21282128; /* 0x15023194: DIP_A_GGM_LUT_RB[101] */
            tuningBuf[0x00001198>>2] = 0x21302130; /* 0x15023198: DIP_A_GGM_LUT_RB[102] */
            tuningBuf[0x0000119C>>2] = 0x21382138; /* 0x1502319C: DIP_A_GGM_LUT_RB[103] */
            tuningBuf[0x000011A0>>2] = 0x21402140; /* 0x150231A0: DIP_A_GGM_LUT_RB[104] */
            tuningBuf[0x000011A4>>2] = 0x21482148; /* 0x150231A4: DIP_A_GGM_LUT_RB[105] */
            tuningBuf[0x000011A8>>2] = 0x21502150; /* 0x150231A8: DIP_A_GGM_LUT_RB[106] */
            tuningBuf[0x000011AC>>2] = 0x21582158; /* 0x150231AC: DIP_A_GGM_LUT_RB[107] */
            tuningBuf[0x000011B0>>2] = 0x21602160; /* 0x150231B0: DIP_A_GGM_LUT_RB[108] */
            tuningBuf[0x000011B4>>2] = 0x21682168; /* 0x150231B4: DIP_A_GGM_LUT_RB[109] */
            tuningBuf[0x000011B8>>2] = 0x21702170; /* 0x150231B8: DIP_A_GGM_LUT_RB[110] */
            tuningBuf[0x000011BC>>2] = 0x21782178; /* 0x150231BC: DIP_A_GGM_LUT_RB[111] */
            tuningBuf[0x000011C0>>2] = 0x21802180; /* 0x150231C0: DIP_A_GGM_LUT_RB[112] */
            tuningBuf[0x000011C4>>2] = 0x21882188; /* 0x150231C4: DIP_A_GGM_LUT_RB[113] */
            tuningBuf[0x000011C8>>2] = 0x21902190; /* 0x150231C8: DIP_A_GGM_LUT_RB[114] */
            tuningBuf[0x000011CC>>2] = 0x21982198; /* 0x150231CC: DIP_A_GGM_LUT_RB[115] */
            tuningBuf[0x000011D0>>2] = 0x21A021A0; /* 0x150231D0: DIP_A_GGM_LUT_RB[116] */
            tuningBuf[0x000011D4>>2] = 0x21A821A8; /* 0x150231D4: DIP_A_GGM_LUT_RB[117] */
            tuningBuf[0x000011D8>>2] = 0x21B021B0; /* 0x150231D8: DIP_A_GGM_LUT_RB[118] */
            tuningBuf[0x000011DC>>2] = 0x21B821B8; /* 0x150231DC: DIP_A_GGM_LUT_RB[119] */
            tuningBuf[0x000011E0>>2] = 0x21C021C0; /* 0x150231E0: DIP_A_GGM_LUT_RB[120] */
            tuningBuf[0x000011E4>>2] = 0x21C821C8; /* 0x150231E4: DIP_A_GGM_LUT_RB[121] */
            tuningBuf[0x000011E8>>2] = 0x21D021D0; /* 0x150231E8: DIP_A_GGM_LUT_RB[122] */
            tuningBuf[0x000011EC>>2] = 0x21D821D8; /* 0x150231EC: DIP_A_GGM_LUT_RB[123] */
            tuningBuf[0x000011F0>>2] = 0x21E021E0; /* 0x150231F0: DIP_A_GGM_LUT_RB[124] */
            tuningBuf[0x000011F4>>2] = 0x21E821E8; /* 0x150231F4: DIP_A_GGM_LUT_RB[125] */
            tuningBuf[0x000011F8>>2] = 0x21F021F0; /* 0x150231F8: DIP_A_GGM_LUT_RB[126] */
            tuningBuf[0x000011FC>>2] = 0x21F821F8; /* 0x150231FC: DIP_A_GGM_LUT_RB[127] */
            tuningBuf[0x00001200>>2] = 0x82008200; /* 0x15023200: DIP_A_GGM_LUT_RB[128] */
            tuningBuf[0x00001204>>2] = 0x82208220; /* 0x15023204: DIP_A_GGM_LUT_RB[129] */
            tuningBuf[0x00001208>>2] = 0x82408240; /* 0x15023208: DIP_A_GGM_LUT_RB[130] */
            tuningBuf[0x0000120C>>2] = 0x82608260; /* 0x1502320C: DIP_A_GGM_LUT_RB[131] */
            tuningBuf[0x00001210>>2] = 0x82808280; /* 0x15023210: DIP_A_GGM_LUT_RB[132] */
            tuningBuf[0x00001214>>2] = 0x82A082A0; /* 0x15023214: DIP_A_GGM_LUT_RB[133] */
            tuningBuf[0x00001218>>2] = 0x82C082C0; /* 0x15023218: DIP_A_GGM_LUT_RB[134] */
            tuningBuf[0x0000121C>>2] = 0x82E082E0; /* 0x1502321C: DIP_A_GGM_LUT_RB[135] */
            tuningBuf[0x00001220>>2] = 0x83008300; /* 0x15023220: DIP_A_GGM_LUT_RB[136] */
            tuningBuf[0x00001224>>2] = 0x83208320; /* 0x15023224: DIP_A_GGM_LUT_RB[137] */
            tuningBuf[0x00001228>>2] = 0x83408340; /* 0x15023228: DIP_A_GGM_LUT_RB[138] */
            tuningBuf[0x0000122C>>2] = 0x83608360; /* 0x1502322C: DIP_A_GGM_LUT_RB[139] */
            tuningBuf[0x00001230>>2] = 0x83808380; /* 0x15023230: DIP_A_GGM_LUT_RB[140] */
            tuningBuf[0x00001234>>2] = 0x83A083A0; /* 0x15023234: DIP_A_GGM_LUT_RB[141] */
            tuningBuf[0x00001238>>2] = 0x83C083C0; /* 0x15023238: DIP_A_GGM_LUT_RB[142] */
            tuningBuf[0x0000123C>>2] = 0x7FE07FE0; /* 0x1502323C: DIP_A_GGM_LUT_RB[143] */
            tuningBuf[0x00001240>>2] = 0x00000800; /* 0x15023240: DIP_A_GGM_LUT_G[0] */
            tuningBuf[0x00001244>>2] = 0x00000802; /* 0x15023244: DIP_A_GGM_LUT_G[1] */
            tuningBuf[0x00001248>>2] = 0x00000804; /* 0x15023248: DIP_A_GGM_LUT_G[2] */
            tuningBuf[0x0000124C>>2] = 0x00000806; /* 0x1502324C: DIP_A_GGM_LUT_G[3] */
            tuningBuf[0x00001250>>2] = 0x00000808; /* 0x15023250: DIP_A_GGM_LUT_G[4] */
            tuningBuf[0x00001254>>2] = 0x0000080A; /* 0x15023254: DIP_A_GGM_LUT_G[5] */
            tuningBuf[0x00001258>>2] = 0x0000080C; /* 0x15023258: DIP_A_GGM_LUT_G[6] */
            tuningBuf[0x0000125C>>2] = 0x0000080E; /* 0x1502325C: DIP_A_GGM_LUT_G[7] */
            tuningBuf[0x00001260>>2] = 0x00000810; /* 0x15023260: DIP_A_GGM_LUT_G[8] */
            tuningBuf[0x00001264>>2] = 0x00000812; /* 0x15023264: DIP_A_GGM_LUT_G[9] */
            tuningBuf[0x00001268>>2] = 0x00000814; /* 0x15023268: DIP_A_GGM_LUT_G[10] */
            tuningBuf[0x0000126C>>2] = 0x00000816; /* 0x1502326C: DIP_A_GGM_LUT_G[11] */
            tuningBuf[0x00001270>>2] = 0x00000818; /* 0x15023270: DIP_A_GGM_LUT_G[12] */
            tuningBuf[0x00001274>>2] = 0x0000081A; /* 0x15023274: DIP_A_GGM_LUT_G[13] */
            tuningBuf[0x00001278>>2] = 0x0000081C; /* 0x15023278: DIP_A_GGM_LUT_G[14] */
            tuningBuf[0x0000127C>>2] = 0x0000081E; /* 0x1502327C: DIP_A_GGM_LUT_G[15] */
            tuningBuf[0x00001280>>2] = 0x00000820; /* 0x15023280: DIP_A_GGM_LUT_G[16] */
            tuningBuf[0x00001284>>2] = 0x00000822; /* 0x15023284: DIP_A_GGM_LUT_G[17] */
            tuningBuf[0x00001288>>2] = 0x00000824; /* 0x15023288: DIP_A_GGM_LUT_G[18] */
            tuningBuf[0x0000128C>>2] = 0x00000826; /* 0x1502328C: DIP_A_GGM_LUT_G[19] */
            tuningBuf[0x00001290>>2] = 0x00000828; /* 0x15023290: DIP_A_GGM_LUT_G[20] */
            tuningBuf[0x00001294>>2] = 0x0000082A; /* 0x15023294: DIP_A_GGM_LUT_G[21] */
            tuningBuf[0x00001298>>2] = 0x0000082C; /* 0x15023298: DIP_A_GGM_LUT_G[22] */
            tuningBuf[0x0000129C>>2] = 0x0000082E; /* 0x1502329C: DIP_A_GGM_LUT_G[23] */
            tuningBuf[0x000012A0>>2] = 0x00000830; /* 0x150232A0: DIP_A_GGM_LUT_G[24] */
            tuningBuf[0x000012A4>>2] = 0x00000832; /* 0x150232A4: DIP_A_GGM_LUT_G[25] */
            tuningBuf[0x000012A8>>2] = 0x00000834; /* 0x150232A8: DIP_A_GGM_LUT_G[26] */
            tuningBuf[0x000012AC>>2] = 0x00000836; /* 0x150232AC: DIP_A_GGM_LUT_G[27] */
            tuningBuf[0x000012B0>>2] = 0x00000838; /* 0x150232B0: DIP_A_GGM_LUT_G[28] */
            tuningBuf[0x000012B4>>2] = 0x0000083A; /* 0x150232B4: DIP_A_GGM_LUT_G[29] */
            tuningBuf[0x000012B8>>2] = 0x0000083C; /* 0x150232B8: DIP_A_GGM_LUT_G[30] */
            tuningBuf[0x000012BC>>2] = 0x0000083E; /* 0x150232BC: DIP_A_GGM_LUT_G[31] */
            tuningBuf[0x000012C0>>2] = 0x00000840; /* 0x150232C0: DIP_A_GGM_LUT_G[32] */
            tuningBuf[0x000012C4>>2] = 0x00000842; /* 0x150232C4: DIP_A_GGM_LUT_G[33] */
            tuningBuf[0x000012C8>>2] = 0x00000844; /* 0x150232C8: DIP_A_GGM_LUT_G[34] */
            tuningBuf[0x000012CC>>2] = 0x00000846; /* 0x150232CC: DIP_A_GGM_LUT_G[35] */
            tuningBuf[0x000012D0>>2] = 0x00000848; /* 0x150232D0: DIP_A_GGM_LUT_G[36] */
            tuningBuf[0x000012D4>>2] = 0x0000084A; /* 0x150232D4: DIP_A_GGM_LUT_G[37] */
            tuningBuf[0x000012D8>>2] = 0x0000084C; /* 0x150232D8: DIP_A_GGM_LUT_G[38] */
            tuningBuf[0x000012DC>>2] = 0x0000084E; /* 0x150232DC: DIP_A_GGM_LUT_G[39] */
            tuningBuf[0x000012E0>>2] = 0x00000850; /* 0x150232E0: DIP_A_GGM_LUT_G[40] */
            tuningBuf[0x000012E4>>2] = 0x00000852; /* 0x150232E4: DIP_A_GGM_LUT_G[41] */
            tuningBuf[0x000012E8>>2] = 0x00000854; /* 0x150232E8: DIP_A_GGM_LUT_G[42] */
            tuningBuf[0x000012EC>>2] = 0x00000856; /* 0x150232EC: DIP_A_GGM_LUT_G[43] */
            tuningBuf[0x000012F0>>2] = 0x00000858; /* 0x150232F0: DIP_A_GGM_LUT_G[44] */
            tuningBuf[0x000012F4>>2] = 0x0000085A; /* 0x150232F4: DIP_A_GGM_LUT_G[45] */
            tuningBuf[0x000012F8>>2] = 0x0000085C; /* 0x150232F8: DIP_A_GGM_LUT_G[46] */
            tuningBuf[0x000012FC>>2] = 0x0000085E; /* 0x150232FC: DIP_A_GGM_LUT_G[47] */
            tuningBuf[0x00001300>>2] = 0x00000860; /* 0x15023300: DIP_A_GGM_LUT_G[48] */
            tuningBuf[0x00001304>>2] = 0x00000862; /* 0x15023304: DIP_A_GGM_LUT_G[49] */
            tuningBuf[0x00001308>>2] = 0x00000864; /* 0x15023308: DIP_A_GGM_LUT_G[50] */
            tuningBuf[0x0000130C>>2] = 0x00000866; /* 0x1502330C: DIP_A_GGM_LUT_G[51] */
            tuningBuf[0x00001310>>2] = 0x00000868; /* 0x15023310: DIP_A_GGM_LUT_G[52] */
            tuningBuf[0x00001314>>2] = 0x0000086A; /* 0x15023314: DIP_A_GGM_LUT_G[53] */
            tuningBuf[0x00001318>>2] = 0x0000086C; /* 0x15023318: DIP_A_GGM_LUT_G[54] */
            tuningBuf[0x0000131C>>2] = 0x0000086E; /* 0x1502331C: DIP_A_GGM_LUT_G[55] */
            tuningBuf[0x00001320>>2] = 0x00000870; /* 0x15023320: DIP_A_GGM_LUT_G[56] */
            tuningBuf[0x00001324>>2] = 0x00000872; /* 0x15023324: DIP_A_GGM_LUT_G[57] */
            tuningBuf[0x00001328>>2] = 0x00000874; /* 0x15023328: DIP_A_GGM_LUT_G[58] */
            tuningBuf[0x0000132C>>2] = 0x00000876; /* 0x1502332C: DIP_A_GGM_LUT_G[59] */
            tuningBuf[0x00001330>>2] = 0x00000878; /* 0x15023330: DIP_A_GGM_LUT_G[60] */
            tuningBuf[0x00001334>>2] = 0x0000087A; /* 0x15023334: DIP_A_GGM_LUT_G[61] */
            tuningBuf[0x00001338>>2] = 0x0000087C; /* 0x15023338: DIP_A_GGM_LUT_G[62] */
            tuningBuf[0x0000133C>>2] = 0x0000087E; /* 0x1502333C: DIP_A_GGM_LUT_G[63] */
            tuningBuf[0x00001340>>2] = 0x00001080; /* 0x15023340: DIP_A_GGM_LUT_G[64] */
            tuningBuf[0x00001344>>2] = 0x00001084; /* 0x15023344: DIP_A_GGM_LUT_G[65] */
            tuningBuf[0x00001348>>2] = 0x00001088; /* 0x15023348: DIP_A_GGM_LUT_G[66] */
            tuningBuf[0x0000134C>>2] = 0x0000108C; /* 0x1502334C: DIP_A_GGM_LUT_G[67] */
            tuningBuf[0x00001350>>2] = 0x00001090; /* 0x15023350: DIP_A_GGM_LUT_G[68] */
            tuningBuf[0x00001354>>2] = 0x00001094; /* 0x15023354: DIP_A_GGM_LUT_G[69] */
            tuningBuf[0x00001358>>2] = 0x00001098; /* 0x15023358: DIP_A_GGM_LUT_G[70] */
            tuningBuf[0x0000135C>>2] = 0x0000109C; /* 0x1502335C: DIP_A_GGM_LUT_G[71] */
            tuningBuf[0x00001360>>2] = 0x000010A0; /* 0x15023360: DIP_A_GGM_LUT_G[72] */
            tuningBuf[0x00001364>>2] = 0x000010A4; /* 0x15023364: DIP_A_GGM_LUT_G[73] */
            tuningBuf[0x00001368>>2] = 0x000010A8; /* 0x15023368: DIP_A_GGM_LUT_G[74] */
            tuningBuf[0x0000136C>>2] = 0x000010AC; /* 0x1502336C: DIP_A_GGM_LUT_G[75] */
            tuningBuf[0x00001370>>2] = 0x000010B0; /* 0x15023370: DIP_A_GGM_LUT_G[76] */
            tuningBuf[0x00001374>>2] = 0x000010B4; /* 0x15023374: DIP_A_GGM_LUT_G[77] */
            tuningBuf[0x00001378>>2] = 0x000010B8; /* 0x15023378: DIP_A_GGM_LUT_G[78] */
            tuningBuf[0x0000137C>>2] = 0x000010BC; /* 0x1502337C: DIP_A_GGM_LUT_G[79] */
            tuningBuf[0x00001380>>2] = 0x000010C0; /* 0x15023380: DIP_A_GGM_LUT_G[80] */
            tuningBuf[0x00001384>>2] = 0x000010C4; /* 0x15023384: DIP_A_GGM_LUT_G[81] */
            tuningBuf[0x00001388>>2] = 0x000010C8; /* 0x15023388: DIP_A_GGM_LUT_G[82] */
            tuningBuf[0x0000138C>>2] = 0x000010CC; /* 0x1502338C: DIP_A_GGM_LUT_G[83] */
            tuningBuf[0x00001390>>2] = 0x000010D0; /* 0x15023390: DIP_A_GGM_LUT_G[84] */
            tuningBuf[0x00001394>>2] = 0x000010D4; /* 0x15023394: DIP_A_GGM_LUT_G[85] */
            tuningBuf[0x00001398>>2] = 0x000010D8; /* 0x15023398: DIP_A_GGM_LUT_G[86] */
            tuningBuf[0x0000139C>>2] = 0x000010DC; /* 0x1502339C: DIP_A_GGM_LUT_G[87] */
            tuningBuf[0x000013A0>>2] = 0x000010E0; /* 0x150233A0: DIP_A_GGM_LUT_G[88] */
            tuningBuf[0x000013A4>>2] = 0x000010E4; /* 0x150233A4: DIP_A_GGM_LUT_G[89] */
            tuningBuf[0x000013A8>>2] = 0x000010E8; /* 0x150233A8: DIP_A_GGM_LUT_G[90] */
            tuningBuf[0x000013AC>>2] = 0x000010EC; /* 0x150233AC: DIP_A_GGM_LUT_G[91] */
            tuningBuf[0x000013B0>>2] = 0x000010F0; /* 0x150233B0: DIP_A_GGM_LUT_G[92] */
            tuningBuf[0x000013B4>>2] = 0x000010F4; /* 0x150233B4: DIP_A_GGM_LUT_G[93] */
            tuningBuf[0x000013B8>>2] = 0x000010F8; /* 0x150233B8: DIP_A_GGM_LUT_G[94] */
            tuningBuf[0x000013BC>>2] = 0x000010FC; /* 0x150233BC: DIP_A_GGM_LUT_G[95] */
            tuningBuf[0x000013C0>>2] = 0x00002100; /* 0x150233C0: DIP_A_GGM_LUT_G[96] */
            tuningBuf[0x000013C4>>2] = 0x00002108; /* 0x150233C4: DIP_A_GGM_LUT_G[97] */
            tuningBuf[0x000013C8>>2] = 0x00002110; /* 0x150233C8: DIP_A_GGM_LUT_G[98] */
            tuningBuf[0x000013CC>>2] = 0x00002118; /* 0x150233CC: DIP_A_GGM_LUT_G[99] */
            tuningBuf[0x000013D0>>2] = 0x00002120; /* 0x150233D0: DIP_A_GGM_LUT_G[100] */
            tuningBuf[0x000013D4>>2] = 0x00002128; /* 0x150233D4: DIP_A_GGM_LUT_G[101] */
            tuningBuf[0x000013D8>>2] = 0x00002130; /* 0x150233D8: DIP_A_GGM_LUT_G[102] */
            tuningBuf[0x000013DC>>2] = 0x00002138; /* 0x150233DC: DIP_A_GGM_LUT_G[103] */
            tuningBuf[0x000013E0>>2] = 0x00002140; /* 0x150233E0: DIP_A_GGM_LUT_G[104] */
            tuningBuf[0x000013E4>>2] = 0x00002148; /* 0x150233E4: DIP_A_GGM_LUT_G[105] */
            tuningBuf[0x000013E8>>2] = 0x00002150; /* 0x150233E8: DIP_A_GGM_LUT_G[106] */
            tuningBuf[0x000013EC>>2] = 0x00002158; /* 0x150233EC: DIP_A_GGM_LUT_G[107] */
            tuningBuf[0x000013F0>>2] = 0x00002160; /* 0x150233F0: DIP_A_GGM_LUT_G[108] */
            tuningBuf[0x000013F4>>2] = 0x00002168; /* 0x150233F4: DIP_A_GGM_LUT_G[109] */
            tuningBuf[0x000013F8>>2] = 0x00002170; /* 0x150233F8: DIP_A_GGM_LUT_G[110] */
            tuningBuf[0x000013FC>>2] = 0x00002178; /* 0x150233FC: DIP_A_GGM_LUT_G[111] */
            tuningBuf[0x00001400>>2] = 0x00002180; /* 0x15023400: DIP_A_GGM_LUT_G[112] */
            tuningBuf[0x00001404>>2] = 0x00002188; /* 0x15023404: DIP_A_GGM_LUT_G[113] */
            tuningBuf[0x00001408>>2] = 0x00002190; /* 0x15023408: DIP_A_GGM_LUT_G[114] */
            tuningBuf[0x0000140C>>2] = 0x00002198; /* 0x1502340C: DIP_A_GGM_LUT_G[115] */
            tuningBuf[0x00001410>>2] = 0x000021A0; /* 0x15023410: DIP_A_GGM_LUT_G[116] */
            tuningBuf[0x00001414>>2] = 0x000021A8; /* 0x15023414: DIP_A_GGM_LUT_G[117] */
            tuningBuf[0x00001418>>2] = 0x000021B0; /* 0x15023418: DIP_A_GGM_LUT_G[118] */
            tuningBuf[0x0000141C>>2] = 0x000021B8; /* 0x1502341C: DIP_A_GGM_LUT_G[119] */
            tuningBuf[0x00001420>>2] = 0x000021C0; /* 0x15023420: DIP_A_GGM_LUT_G[120] */
            tuningBuf[0x00001424>>2] = 0x000021C8; /* 0x15023424: DIP_A_GGM_LUT_G[121] */
            tuningBuf[0x00001428>>2] = 0x000021D0; /* 0x15023428: DIP_A_GGM_LUT_G[122] */
            tuningBuf[0x0000142C>>2] = 0x000021D8; /* 0x1502342C: DIP_A_GGM_LUT_G[123] */
            tuningBuf[0x00001430>>2] = 0x000021E0; /* 0x15023430: DIP_A_GGM_LUT_G[124] */
            tuningBuf[0x00001434>>2] = 0x000021E8; /* 0x15023434: DIP_A_GGM_LUT_G[125] */
            tuningBuf[0x00001438>>2] = 0x000021F0; /* 0x15023438: DIP_A_GGM_LUT_G[126] */
            tuningBuf[0x0000143C>>2] = 0x000021F8; /* 0x1502343C: DIP_A_GGM_LUT_G[127] */
            tuningBuf[0x00001440>>2] = 0x00008200; /* 0x15023440: DIP_A_GGM_LUT_G[128] */
            tuningBuf[0x00001444>>2] = 0x00008220; /* 0x15023444: DIP_A_GGM_LUT_G[129] */
            tuningBuf[0x00001448>>2] = 0x00008240; /* 0x15023448: DIP_A_GGM_LUT_G[130] */
            tuningBuf[0x0000144C>>2] = 0x00008260; /* 0x1502344C: DIP_A_GGM_LUT_G[131] */
            tuningBuf[0x00001450>>2] = 0x00008280; /* 0x15023450: DIP_A_GGM_LUT_G[132] */
            tuningBuf[0x00001454>>2] = 0x000082A0; /* 0x15023454: DIP_A_GGM_LUT_G[133] */
            tuningBuf[0x00001458>>2] = 0x000082C0; /* 0x15023458: DIP_A_GGM_LUT_G[134] */
            tuningBuf[0x0000145C>>2] = 0x000082E0; /* 0x1502345C: DIP_A_GGM_LUT_G[135] */
            tuningBuf[0x00001460>>2] = 0x00008300; /* 0x15023460: DIP_A_GGM_LUT_G[136] */
            tuningBuf[0x00001464>>2] = 0x00008320; /* 0x15023464: DIP_A_GGM_LUT_G[137] */
            tuningBuf[0x00001468>>2] = 0x00008340; /* 0x15023468: DIP_A_GGM_LUT_G[138] */
            tuningBuf[0x0000146C>>2] = 0x00008360; /* 0x1502346C: DIP_A_GGM_LUT_G[139] */
            tuningBuf[0x00001470>>2] = 0x00008380; /* 0x15023470: DIP_A_GGM_LUT_G[140] */
            tuningBuf[0x00001474>>2] = 0x000083A0; /* 0x15023474: DIP_A_GGM_LUT_G[141] */
            tuningBuf[0x00001478>>2] = 0x000083C0; /* 0x15023478: DIP_A_GGM_LUT_G[142] */
            tuningBuf[0x0000147C>>2] = 0x00007FE0; /* 0x1502347C: DIP_A_GGM_LUT_G[143] */
            tuningBuf[0x00001480>>2] = 0x00000000; /* 0x15023480: DIP_A_GGM_CTRL */
            tuningBuf[0x00003000>>2] = 0x51FC32A2; /* 0x15025000: DIP_B_GGM_LUT_RB[0] */
            tuningBuf[0x00003004>>2] = 0x7CD35AAB; /* 0x15025004: DIP_B_GGM_LUT_RB[1] */
            tuningBuf[0x00003008>>2] = 0x832D3C37; /* 0x15025008: DIP_B_GGM_LUT_RB[2] */
            tuningBuf[0x0000300C>>2] = 0x19837F05; /* 0x1502500C: DIP_B_GGM_LUT_RB[3] */
            tuningBuf[0x00003010>>2] = 0xC5849CE7; /* 0x15025010: DIP_B_GGM_LUT_RB[4] */
            tuningBuf[0x00003014>>2] = 0xF302BF18; /* 0x15025014: DIP_B_GGM_LUT_RB[5] */
            tuningBuf[0x00003018>>2] = 0xAFB97024; /* 0x15025018: DIP_B_GGM_LUT_RB[6] */
            tuningBuf[0x0000301C>>2] = 0x4D0CC69E; /* 0x1502501C: DIP_B_GGM_LUT_RB[7] */
            tuningBuf[0x00003020>>2] = 0xC4F0D74B; /* 0x15025020: DIP_B_GGM_LUT_RB[8] */
            tuningBuf[0x00003024>>2] = 0x66A581F1; /* 0x15025024: DIP_B_GGM_LUT_RB[9] */
            tuningBuf[0x00003028>>2] = 0x4FF5E73F; /* 0x15025028: DIP_B_GGM_LUT_RB[10] */
            tuningBuf[0x0000302C>>2] = 0xC4718E8E; /* 0x1502502C: DIP_B_GGM_LUT_RB[11] */
            tuningBuf[0x00003030>>2] = 0x9C86DCB9; /* 0x15025030: DIP_B_GGM_LUT_RB[12] */
            tuningBuf[0x00003034>>2] = 0xBC3F0C2E; /* 0x15025034: DIP_B_GGM_LUT_RB[13] */
            tuningBuf[0x00003038>>2] = 0xA0204380; /* 0x15025038: DIP_B_GGM_LUT_RB[14] */
            tuningBuf[0x0000303C>>2] = 0x70314E74; /* 0x1502503C: DIP_B_GGM_LUT_RB[15] */
            tuningBuf[0x00003040>>2] = 0x5C52EDA8; /* 0x15025040: DIP_B_GGM_LUT_RB[16] */
            tuningBuf[0x00003044>>2] = 0x0C77F482; /* 0x15025044: DIP_B_GGM_LUT_RB[17] */
            tuningBuf[0x00003048>>2] = 0x2CE55E8C; /* 0x15025048: DIP_B_GGM_LUT_RB[18] */
            tuningBuf[0x0000304C>>2] = 0xC2B86EF5; /* 0x1502504C: DIP_B_GGM_LUT_RB[19] */
            tuningBuf[0x00003050>>2] = 0x118F80F7; /* 0x15025050: DIP_B_GGM_LUT_RB[20] */
            tuningBuf[0x00003054>>2] = 0xF430D1D9; /* 0x15025054: DIP_B_GGM_LUT_RB[21] */
            tuningBuf[0x00003058>>2] = 0x543E5522; /* 0x15025058: DIP_B_GGM_LUT_RB[22] */
            tuningBuf[0x0000305C>>2] = 0xF0F5A76C; /* 0x1502505C: DIP_B_GGM_LUT_RB[23] */
            tuningBuf[0x00003060>>2] = 0x446E7727; /* 0x15025060: DIP_B_GGM_LUT_RB[24] */
            tuningBuf[0x00003064>>2] = 0xE1A63E64; /* 0x15025064: DIP_B_GGM_LUT_RB[25] */
            tuningBuf[0x00003068>>2] = 0xEC939B44; /* 0x15025068: DIP_B_GGM_LUT_RB[26] */
            tuningBuf[0x0000306C>>2] = 0xC8ABC865; /* 0x1502506C: DIP_B_GGM_LUT_RB[27] */
            tuningBuf[0x00003070>>2] = 0x8BE4B9AD; /* 0x15025070: DIP_B_GGM_LUT_RB[28] */
            tuningBuf[0x00003074>>2] = 0x0CE97EDB; /* 0x15025074: DIP_B_GGM_LUT_RB[29] */
            tuningBuf[0x00003078>>2] = 0x38FD6F8D; /* 0x15025078: DIP_B_GGM_LUT_RB[30] */
            tuningBuf[0x0000307C>>2] = 0xA34FC705; /* 0x1502507C: DIP_B_GGM_LUT_RB[31] */
            tuningBuf[0x00003080>>2] = 0x875961C7; /* 0x15025080: DIP_B_GGM_LUT_RB[32] */
            tuningBuf[0x00003084>>2] = 0x1C4F550D; /* 0x15025084: DIP_B_GGM_LUT_RB[33] */
            tuningBuf[0x00003088>>2] = 0x7B40D1D2; /* 0x15025088: DIP_B_GGM_LUT_RB[34] */
            tuningBuf[0x0000308C>>2] = 0xB069AF63; /* 0x1502508C: DIP_B_GGM_LUT_RB[35] */
            tuningBuf[0x00003090>>2] = 0x8AD7D124; /* 0x15025090: DIP_B_GGM_LUT_RB[36] */
            tuningBuf[0x00003094>>2] = 0xF9EDA6D1; /* 0x15025094: DIP_B_GGM_LUT_RB[37] */
            tuningBuf[0x00003098>>2] = 0x5D907A88; /* 0x15025098: DIP_B_GGM_LUT_RB[38] */
            tuningBuf[0x0000309C>>2] = 0x2E233543; /* 0x1502509C: DIP_B_GGM_LUT_RB[39] */
            tuningBuf[0x000030A0>>2] = 0x80AD5315; /* 0x150250A0: DIP_B_GGM_LUT_RB[40] */
            tuningBuf[0x000030A4>>2] = 0xC3E272CB; /* 0x150250A4: DIP_B_GGM_LUT_RB[41] */
            tuningBuf[0x000030A8>>2] = 0x846AD653; /* 0x150250A8: DIP_B_GGM_LUT_RB[42] */
            tuningBuf[0x000030AC>>2] = 0x9A89C881; /* 0x150250AC: DIP_B_GGM_LUT_RB[43] */
            tuningBuf[0x000030B0>>2] = 0x5A90B9ED; /* 0x150250B0: DIP_B_GGM_LUT_RB[44] */
            tuningBuf[0x000030B4>>2] = 0x0BD2F910; /* 0x150250B4: DIP_B_GGM_LUT_RB[45] */
            tuningBuf[0x000030B8>>2] = 0xEFE451E9; /* 0x150250B8: DIP_B_GGM_LUT_RB[46] */
            tuningBuf[0x000030BC>>2] = 0x0E57DB60; /* 0x150250BC: DIP_B_GGM_LUT_RB[47] */
            tuningBuf[0x000030C0>>2] = 0x1444C642; /* 0x150250C0: DIP_B_GGM_LUT_RB[48] */
            tuningBuf[0x000030C4>>2] = 0x0E04AFD6; /* 0x150250C4: DIP_B_GGM_LUT_RB[49] */
            tuningBuf[0x000030C8>>2] = 0x2BD9F148; /* 0x150250C8: DIP_B_GGM_LUT_RB[50] */
            tuningBuf[0x000030CC>>2] = 0x9D1D1E6E; /* 0x150250CC: DIP_B_GGM_LUT_RB[51] */
            tuningBuf[0x000030D0>>2] = 0xEBEE3E03; /* 0x150250D0: DIP_B_GGM_LUT_RB[52] */
            tuningBuf[0x000030D4>>2] = 0x1EBB3E11; /* 0x150250D4: DIP_B_GGM_LUT_RB[53] */
            tuningBuf[0x000030D8>>2] = 0x2CC18D75; /* 0x150250D8: DIP_B_GGM_LUT_RB[54] */
            tuningBuf[0x000030DC>>2] = 0xEA162348; /* 0x150250DC: DIP_B_GGM_LUT_RB[55] */
            tuningBuf[0x000030E0>>2] = 0xE3E7EB69; /* 0x150250E0: DIP_B_GGM_LUT_RB[56] */
            tuningBuf[0x000030E4>>2] = 0x7ACFE8FD; /* 0x150250E4: DIP_B_GGM_LUT_RB[57] */
            tuningBuf[0x000030E8>>2] = 0xA4253C0A; /* 0x150250E8: DIP_B_GGM_LUT_RB[58] */
            tuningBuf[0x000030EC>>2] = 0x8B03FFA2; /* 0x150250EC: DIP_B_GGM_LUT_RB[59] */
            tuningBuf[0x000030F0>>2] = 0xE8994F52; /* 0x150250F0: DIP_B_GGM_LUT_RB[60] */
            tuningBuf[0x000030F4>>2] = 0xCF5DDB50; /* 0x150250F4: DIP_B_GGM_LUT_RB[61] */
            tuningBuf[0x000030F8>>2] = 0xA6BF21A2; /* 0x150250F8: DIP_B_GGM_LUT_RB[62] */
            tuningBuf[0x000030FC>>2] = 0xB98A101C; /* 0x150250FC: DIP_B_GGM_LUT_RB[63] */
            tuningBuf[0x00003100>>2] = 0x9BDA2515; /* 0x15025100: DIP_B_GGM_LUT_RB[64] */
            tuningBuf[0x00003104>>2] = 0xF9A256DF; /* 0x15025104: DIP_B_GGM_LUT_RB[65] */
            tuningBuf[0x00003108>>2] = 0xD84834D4; /* 0x15025108: DIP_B_GGM_LUT_RB[66] */
            tuningBuf[0x0000310C>>2] = 0x9FD42127; /* 0x1502510C: DIP_B_GGM_LUT_RB[67] */
            tuningBuf[0x00003110>>2] = 0x357C27D1; /* 0x15025110: DIP_B_GGM_LUT_RB[68] */
            tuningBuf[0x00003114>>2] = 0x053D1CD3; /* 0x15025114: DIP_B_GGM_LUT_RB[69] */
            tuningBuf[0x00003118>>2] = 0x758FA6EB; /* 0x15025118: DIP_B_GGM_LUT_RB[70] */
            tuningBuf[0x0000311C>>2] = 0xB4D85B4C; /* 0x1502511C: DIP_B_GGM_LUT_RB[71] */
            tuningBuf[0x00003120>>2] = 0x85F839D1; /* 0x15025120: DIP_B_GGM_LUT_RB[72] */
            tuningBuf[0x00003124>>2] = 0xFFE9F8ED; /* 0x15025124: DIP_B_GGM_LUT_RB[73] */
            tuningBuf[0x00003128>>2] = 0x45EF466C; /* 0x15025128: DIP_B_GGM_LUT_RB[74] */
            tuningBuf[0x0000312C>>2] = 0xF2875267; /* 0x1502512C: DIP_B_GGM_LUT_RB[75] */
            tuningBuf[0x00003130>>2] = 0x396A9866; /* 0x15025130: DIP_B_GGM_LUT_RB[76] */
            tuningBuf[0x00003134>>2] = 0xB3DE96C4; /* 0x15025134: DIP_B_GGM_LUT_RB[77] */
            tuningBuf[0x00003138>>2] = 0x1085FA77; /* 0x15025138: DIP_B_GGM_LUT_RB[78] */
            tuningBuf[0x0000313C>>2] = 0x97BBB97B; /* 0x1502513C: DIP_B_GGM_LUT_RB[79] */
            tuningBuf[0x00003140>>2] = 0xD9B2A1B9; /* 0x15025140: DIP_B_GGM_LUT_RB[80] */
            tuningBuf[0x00003144>>2] = 0x5FCFCE67; /* 0x15025144: DIP_B_GGM_LUT_RB[81] */
            tuningBuf[0x00003148>>2] = 0x969A3174; /* 0x15025148: DIP_B_GGM_LUT_RB[82] */
            tuningBuf[0x0000314C>>2] = 0xF94C1AF7; /* 0x1502514C: DIP_B_GGM_LUT_RB[83] */
            tuningBuf[0x00003150>>2] = 0x0A95B6C1; /* 0x15025150: DIP_B_GGM_LUT_RB[84] */
            tuningBuf[0x00003154>>2] = 0x0D245257; /* 0x15025154: DIP_B_GGM_LUT_RB[85] */
            tuningBuf[0x00003158>>2] = 0xF042416F; /* 0x15025158: DIP_B_GGM_LUT_RB[86] */
            tuningBuf[0x0000315C>>2] = 0x270D5666; /* 0x1502515C: DIP_B_GGM_LUT_RB[87] */
            tuningBuf[0x00003160>>2] = 0xD1DF52FC; /* 0x15025160: DIP_B_GGM_LUT_RB[88] */
            tuningBuf[0x00003164>>2] = 0xA93F054E; /* 0x15025164: DIP_B_GGM_LUT_RB[89] */
            tuningBuf[0x00003168>>2] = 0xDA58E517; /* 0x15025168: DIP_B_GGM_LUT_RB[90] */
            tuningBuf[0x0000316C>>2] = 0x827E0CCA; /* 0x1502516C: DIP_B_GGM_LUT_RB[91] */
            tuningBuf[0x00003170>>2] = 0x4E469D0B; /* 0x15025170: DIP_B_GGM_LUT_RB[92] */
            tuningBuf[0x00003174>>2] = 0xD9D8B48C; /* 0x15025174: DIP_B_GGM_LUT_RB[93] */
            tuningBuf[0x00003178>>2] = 0xFC7ACAF0; /* 0x15025178: DIP_B_GGM_LUT_RB[94] */
            tuningBuf[0x0000317C>>2] = 0x85EA04C7; /* 0x1502517C: DIP_B_GGM_LUT_RB[95] */
            tuningBuf[0x00003180>>2] = 0x8292AA9D; /* 0x15025180: DIP_B_GGM_LUT_RB[96] */
            tuningBuf[0x00003184>>2] = 0xA75FFC20; /* 0x15025184: DIP_B_GGM_LUT_RB[97] */
            tuningBuf[0x00003188>>2] = 0xA4CDA426; /* 0x15025188: DIP_B_GGM_LUT_RB[98] */
            tuningBuf[0x0000318C>>2] = 0xF4CF7855; /* 0x1502518C: DIP_B_GGM_LUT_RB[99] */
            tuningBuf[0x00003190>>2] = 0x1ABC3489; /* 0x15025190: DIP_B_GGM_LUT_RB[100] */
            tuningBuf[0x00003194>>2] = 0x1DD15E31; /* 0x15025194: DIP_B_GGM_LUT_RB[101] */
            tuningBuf[0x00003198>>2] = 0x52944250; /* 0x15025198: DIP_B_GGM_LUT_RB[102] */
            tuningBuf[0x0000319C>>2] = 0xBC53E69F; /* 0x1502519C: DIP_B_GGM_LUT_RB[103] */
            tuningBuf[0x000031A0>>2] = 0x40B1AF29; /* 0x150251A0: DIP_B_GGM_LUT_RB[104] */
            tuningBuf[0x000031A4>>2] = 0x91AEEBAE; /* 0x150251A4: DIP_B_GGM_LUT_RB[105] */
            tuningBuf[0x000031A8>>2] = 0xDFCC883F; /* 0x150251A8: DIP_B_GGM_LUT_RB[106] */
            tuningBuf[0x000031AC>>2] = 0xFE93F521; /* 0x150251AC: DIP_B_GGM_LUT_RB[107] */
            tuningBuf[0x000031B0>>2] = 0xC3FE399C; /* 0x150251B0: DIP_B_GGM_LUT_RB[108] */
            tuningBuf[0x000031B4>>2] = 0xB06CCBF5; /* 0x150251B4: DIP_B_GGM_LUT_RB[109] */
            tuningBuf[0x000031B8>>2] = 0xA1941704; /* 0x150251B8: DIP_B_GGM_LUT_RB[110] */
            tuningBuf[0x000031BC>>2] = 0xC8A9CE6A; /* 0x150251BC: DIP_B_GGM_LUT_RB[111] */
            tuningBuf[0x000031C0>>2] = 0xB466055C; /* 0x150251C0: DIP_B_GGM_LUT_RB[112] */
            tuningBuf[0x000031C4>>2] = 0xD2CF41C1; /* 0x150251C4: DIP_B_GGM_LUT_RB[113] */
            tuningBuf[0x000031C8>>2] = 0x2F900ED7; /* 0x150251C8: DIP_B_GGM_LUT_RB[114] */
            tuningBuf[0x000031CC>>2] = 0xD0A13E6D; /* 0x150251CC: DIP_B_GGM_LUT_RB[115] */
            tuningBuf[0x000031D0>>2] = 0x23EB776C; /* 0x150251D0: DIP_B_GGM_LUT_RB[116] */
            tuningBuf[0x000031D4>>2] = 0xF8329688; /* 0x150251D4: DIP_B_GGM_LUT_RB[117] */
            tuningBuf[0x000031D8>>2] = 0x04BF03BA; /* 0x150251D8: DIP_B_GGM_LUT_RB[118] */
            tuningBuf[0x000031DC>>2] = 0xFE0383A3; /* 0x150251DC: DIP_B_GGM_LUT_RB[119] */
            tuningBuf[0x000031E0>>2] = 0xB9F354D1; /* 0x150251E0: DIP_B_GGM_LUT_RB[120] */
            tuningBuf[0x000031E4>>2] = 0x1FC774F3; /* 0x150251E4: DIP_B_GGM_LUT_RB[121] */
            tuningBuf[0x000031E8>>2] = 0x8950DC74; /* 0x150251E8: DIP_B_GGM_LUT_RB[122] */
            tuningBuf[0x000031EC>>2] = 0x58C6BA69; /* 0x150251EC: DIP_B_GGM_LUT_RB[123] */
            tuningBuf[0x000031F0>>2] = 0x807E3B2D; /* 0x150251F0: DIP_B_GGM_LUT_RB[124] */
            tuningBuf[0x000031F4>>2] = 0xF2342D86; /* 0x150251F4: DIP_B_GGM_LUT_RB[125] */
            tuningBuf[0x000031F8>>2] = 0x809CBF51; /* 0x150251F8: DIP_B_GGM_LUT_RB[126] */
            tuningBuf[0x000031FC>>2] = 0x2FFC258F; /* 0x150251FC: DIP_B_GGM_LUT_RB[127] */
            tuningBuf[0x00003200>>2] = 0x9FE0EF0E; /* 0x15025200: DIP_B_GGM_LUT_RB[128] */
            tuningBuf[0x00003204>>2] = 0x05BE409B; /* 0x15025204: DIP_B_GGM_LUT_RB[129] */
            tuningBuf[0x00003208>>2] = 0x3FEDF830; /* 0x15025208: DIP_B_GGM_LUT_RB[130] */
            tuningBuf[0x0000320C>>2] = 0xD991AED8; /* 0x1502520C: DIP_B_GGM_LUT_RB[131] */
            tuningBuf[0x00003210>>2] = 0x95B77374; /* 0x15025210: DIP_B_GGM_LUT_RB[132] */
            tuningBuf[0x00003214>>2] = 0x92B3573D; /* 0x15025214: DIP_B_GGM_LUT_RB[133] */
            tuningBuf[0x00003218>>2] = 0x267E8F95; /* 0x15025218: DIP_B_GGM_LUT_RB[134] */
            tuningBuf[0x0000321C>>2] = 0x9F030BEC; /* 0x1502521C: DIP_B_GGM_LUT_RB[135] */
            tuningBuf[0x00003220>>2] = 0x1C0B9A54; /* 0x15025220: DIP_B_GGM_LUT_RB[136] */
            tuningBuf[0x00003224>>2] = 0x53454A3A; /* 0x15025224: DIP_B_GGM_LUT_RB[137] */
            tuningBuf[0x00003228>>2] = 0xC44FF7FB; /* 0x15025228: DIP_B_GGM_LUT_RB[138] */
            tuningBuf[0x0000322C>>2] = 0x8B920BAF; /* 0x1502522C: DIP_B_GGM_LUT_RB[139] */
            tuningBuf[0x00003230>>2] = 0xBB36387E; /* 0x15025230: DIP_B_GGM_LUT_RB[140] */
            tuningBuf[0x00003234>>2] = 0x19D78E97; /* 0x15025234: DIP_B_GGM_LUT_RB[141] */
            tuningBuf[0x00003238>>2] = 0x1B3BBF42; /* 0x15025238: DIP_B_GGM_LUT_RB[142] */
            tuningBuf[0x0000323C>>2] = 0x1086B7BD; /* 0x1502523C: DIP_B_GGM_LUT_RB[143] */
            tuningBuf[0x00003240>>2] = 0x0000AF3A; /* 0x15025240: DIP_B_GGM_LUT_G[0] */
            tuningBuf[0x00003244>>2] = 0x00004C1D; /* 0x15025244: DIP_B_GGM_LUT_G[1] */
            tuningBuf[0x00003248>>2] = 0x000061FD; /* 0x15025248: DIP_B_GGM_LUT_G[2] */
            tuningBuf[0x0000324C>>2] = 0x000088A7; /* 0x1502524C: DIP_B_GGM_LUT_G[3] */
            tuningBuf[0x00003250>>2] = 0x00002412; /* 0x15025250: DIP_B_GGM_LUT_G[4] */
            tuningBuf[0x00003254>>2] = 0x0000332D; /* 0x15025254: DIP_B_GGM_LUT_G[5] */
            tuningBuf[0x00003258>>2] = 0x0000F137; /* 0x15025258: DIP_B_GGM_LUT_G[6] */
            tuningBuf[0x0000325C>>2] = 0x00007A64; /* 0x1502525C: DIP_B_GGM_LUT_G[7] */
            tuningBuf[0x00003260>>2] = 0x000064ED; /* 0x15025260: DIP_B_GGM_LUT_G[8] */
            tuningBuf[0x00003264>>2] = 0x000083FA; /* 0x15025264: DIP_B_GGM_LUT_G[9] */
            tuningBuf[0x00003268>>2] = 0x0000C410; /* 0x15025268: DIP_B_GGM_LUT_G[10] */
            tuningBuf[0x0000326C>>2] = 0x000076FD; /* 0x1502526C: DIP_B_GGM_LUT_G[11] */
            tuningBuf[0x00003270>>2] = 0x0000310F; /* 0x15025270: DIP_B_GGM_LUT_G[12] */
            tuningBuf[0x00003274>>2] = 0x000076DC; /* 0x15025274: DIP_B_GGM_LUT_G[13] */
            tuningBuf[0x00003278>>2] = 0x0000B6B4; /* 0x15025278: DIP_B_GGM_LUT_G[14] */
            tuningBuf[0x0000327C>>2] = 0x00003CEF; /* 0x1502527C: DIP_B_GGM_LUT_G[15] */
            tuningBuf[0x00003280>>2] = 0x000077F2; /* 0x15025280: DIP_B_GGM_LUT_G[16] */
            tuningBuf[0x00003284>>2] = 0x0000902D; /* 0x15025284: DIP_B_GGM_LUT_G[17] */
            tuningBuf[0x00003288>>2] = 0x00009E30; /* 0x15025288: DIP_B_GGM_LUT_G[18] */
            tuningBuf[0x0000328C>>2] = 0x0000FA5C; /* 0x1502528C: DIP_B_GGM_LUT_G[19] */
            tuningBuf[0x00003290>>2] = 0x0000A3AB; /* 0x15025290: DIP_B_GGM_LUT_G[20] */
            tuningBuf[0x00003294>>2] = 0x0000F9E5; /* 0x15025294: DIP_B_GGM_LUT_G[21] */
            tuningBuf[0x00003298>>2] = 0x0000F6DA; /* 0x15025298: DIP_B_GGM_LUT_G[22] */
            tuningBuf[0x0000329C>>2] = 0x00002058; /* 0x1502529C: DIP_B_GGM_LUT_G[23] */
            tuningBuf[0x000032A0>>2] = 0x00001DB8; /* 0x150252A0: DIP_B_GGM_LUT_G[24] */
            tuningBuf[0x000032A4>>2] = 0x00003EE7; /* 0x150252A4: DIP_B_GGM_LUT_G[25] */
            tuningBuf[0x000032A8>>2] = 0x0000B8B1; /* 0x150252A8: DIP_B_GGM_LUT_G[26] */
            tuningBuf[0x000032AC>>2] = 0x000090F3; /* 0x150252AC: DIP_B_GGM_LUT_G[27] */
            tuningBuf[0x000032B0>>2] = 0x00001637; /* 0x150252B0: DIP_B_GGM_LUT_G[28] */
            tuningBuf[0x000032B4>>2] = 0x00007895; /* 0x150252B4: DIP_B_GGM_LUT_G[29] */
            tuningBuf[0x000032B8>>2] = 0x00003BF4; /* 0x150252B8: DIP_B_GGM_LUT_G[30] */
            tuningBuf[0x000032BC>>2] = 0x0000AF04; /* 0x150252BC: DIP_B_GGM_LUT_G[31] */
            tuningBuf[0x000032C0>>2] = 0x0000119A; /* 0x150252C0: DIP_B_GGM_LUT_G[32] */
            tuningBuf[0x000032C4>>2] = 0x0000A4C0; /* 0x150252C4: DIP_B_GGM_LUT_G[33] */
            tuningBuf[0x000032C8>>2] = 0x000000B9; /* 0x150252C8: DIP_B_GGM_LUT_G[34] */
            tuningBuf[0x000032CC>>2] = 0x0000531E; /* 0x150252CC: DIP_B_GGM_LUT_G[35] */
            tuningBuf[0x000032D0>>2] = 0x0000BEAC; /* 0x150252D0: DIP_B_GGM_LUT_G[36] */
            tuningBuf[0x000032D4>>2] = 0x00004E6B; /* 0x150252D4: DIP_B_GGM_LUT_G[37] */
            tuningBuf[0x000032D8>>2] = 0x00005BE5; /* 0x150252D8: DIP_B_GGM_LUT_G[38] */
            tuningBuf[0x000032DC>>2] = 0x000008F7; /* 0x150252DC: DIP_B_GGM_LUT_G[39] */
            tuningBuf[0x000032E0>>2] = 0x0000D97D; /* 0x150252E0: DIP_B_GGM_LUT_G[40] */
            tuningBuf[0x000032E4>>2] = 0x000069F5; /* 0x150252E4: DIP_B_GGM_LUT_G[41] */
            tuningBuf[0x000032E8>>2] = 0x00002048; /* 0x150252E8: DIP_B_GGM_LUT_G[42] */
            tuningBuf[0x000032EC>>2] = 0x000088A8; /* 0x150252EC: DIP_B_GGM_LUT_G[43] */
            tuningBuf[0x000032F0>>2] = 0x0000C246; /* 0x150252F0: DIP_B_GGM_LUT_G[44] */
            tuningBuf[0x000032F4>>2] = 0x0000EFE7; /* 0x150252F4: DIP_B_GGM_LUT_G[45] */
            tuningBuf[0x000032F8>>2] = 0x00002821; /* 0x150252F8: DIP_B_GGM_LUT_G[46] */
            tuningBuf[0x000032FC>>2] = 0x00005662; /* 0x150252FC: DIP_B_GGM_LUT_G[47] */
            tuningBuf[0x00003300>>2] = 0x0000EF56; /* 0x15025300: DIP_B_GGM_LUT_G[48] */
            tuningBuf[0x00003304>>2] = 0x00000C9A; /* 0x15025304: DIP_B_GGM_LUT_G[49] */
            tuningBuf[0x00003308>>2] = 0x0000C677; /* 0x15025308: DIP_B_GGM_LUT_G[50] */
            tuningBuf[0x0000330C>>2] = 0x0000D528; /* 0x1502530C: DIP_B_GGM_LUT_G[51] */
            tuningBuf[0x00003310>>2] = 0x0000A1AA; /* 0x15025310: DIP_B_GGM_LUT_G[52] */
            tuningBuf[0x00003314>>2] = 0x0000C734; /* 0x15025314: DIP_B_GGM_LUT_G[53] */
            tuningBuf[0x00003318>>2] = 0x0000E931; /* 0x15025318: DIP_B_GGM_LUT_G[54] */
            tuningBuf[0x0000331C>>2] = 0x00001E41; /* 0x1502531C: DIP_B_GGM_LUT_G[55] */
            tuningBuf[0x00003320>>2] = 0x0000FF7E; /* 0x15025320: DIP_B_GGM_LUT_G[56] */
            tuningBuf[0x00003324>>2] = 0x0000827C; /* 0x15025324: DIP_B_GGM_LUT_G[57] */
            tuningBuf[0x00003328>>2] = 0x0000AC78; /* 0x15025328: DIP_B_GGM_LUT_G[58] */
            tuningBuf[0x0000332C>>2] = 0x000070F7; /* 0x1502532C: DIP_B_GGM_LUT_G[59] */
            tuningBuf[0x00003330>>2] = 0x000054D4; /* 0x15025330: DIP_B_GGM_LUT_G[60] */
            tuningBuf[0x00003334>>2] = 0x0000950D; /* 0x15025334: DIP_B_GGM_LUT_G[61] */
            tuningBuf[0x00003338>>2] = 0x0000D624; /* 0x15025338: DIP_B_GGM_LUT_G[62] */
            tuningBuf[0x0000333C>>2] = 0x00002151; /* 0x1502533C: DIP_B_GGM_LUT_G[63] */
            tuningBuf[0x00003340>>2] = 0x00004241; /* 0x15025340: DIP_B_GGM_LUT_G[64] */
            tuningBuf[0x00003344>>2] = 0x00001A91; /* 0x15025344: DIP_B_GGM_LUT_G[65] */
            tuningBuf[0x00003348>>2] = 0x0000C2E7; /* 0x15025348: DIP_B_GGM_LUT_G[66] */
            tuningBuf[0x0000334C>>2] = 0x0000FBF6; /* 0x1502534C: DIP_B_GGM_LUT_G[67] */
            tuningBuf[0x00003350>>2] = 0x0000CDD3; /* 0x15025350: DIP_B_GGM_LUT_G[68] */
            tuningBuf[0x00003354>>2] = 0x00005C1F; /* 0x15025354: DIP_B_GGM_LUT_G[69] */
            tuningBuf[0x00003358>>2] = 0x00002A50; /* 0x15025358: DIP_B_GGM_LUT_G[70] */
            tuningBuf[0x0000335C>>2] = 0x0000ED09; /* 0x1502535C: DIP_B_GGM_LUT_G[71] */
            tuningBuf[0x00003360>>2] = 0x00006FA8; /* 0x15025360: DIP_B_GGM_LUT_G[72] */
            tuningBuf[0x00003364>>2] = 0x0000BBC4; /* 0x15025364: DIP_B_GGM_LUT_G[73] */
            tuningBuf[0x00003368>>2] = 0x00003E82; /* 0x15025368: DIP_B_GGM_LUT_G[74] */
            tuningBuf[0x0000336C>>2] = 0x0000BE3C; /* 0x1502536C: DIP_B_GGM_LUT_G[75] */
            tuningBuf[0x00003370>>2] = 0x0000756E; /* 0x15025370: DIP_B_GGM_LUT_G[76] */
            tuningBuf[0x00003374>>2] = 0x00009E14; /* 0x15025374: DIP_B_GGM_LUT_G[77] */
            tuningBuf[0x00003378>>2] = 0x0000EF5C; /* 0x15025378: DIP_B_GGM_LUT_G[78] */
            tuningBuf[0x0000337C>>2] = 0x0000B770; /* 0x1502537C: DIP_B_GGM_LUT_G[79] */
            tuningBuf[0x00003380>>2] = 0x000057C0; /* 0x15025380: DIP_B_GGM_LUT_G[80] */
            tuningBuf[0x00003384>>2] = 0x0000A47B; /* 0x15025384: DIP_B_GGM_LUT_G[81] */
            tuningBuf[0x00003388>>2] = 0x000041B0; /* 0x15025388: DIP_B_GGM_LUT_G[82] */
            tuningBuf[0x0000338C>>2] = 0x0000E787; /* 0x1502538C: DIP_B_GGM_LUT_G[83] */
            tuningBuf[0x00003390>>2] = 0x000067E3; /* 0x15025390: DIP_B_GGM_LUT_G[84] */
            tuningBuf[0x00003394>>2] = 0x00002BC6; /* 0x15025394: DIP_B_GGM_LUT_G[85] */
            tuningBuf[0x00003398>>2] = 0x0000BBD8; /* 0x15025398: DIP_B_GGM_LUT_G[86] */
            tuningBuf[0x0000339C>>2] = 0x0000057A; /* 0x1502539C: DIP_B_GGM_LUT_G[87] */
            tuningBuf[0x000033A0>>2] = 0x00003BFF; /* 0x150253A0: DIP_B_GGM_LUT_G[88] */
            tuningBuf[0x000033A4>>2] = 0x00000122; /* 0x150253A4: DIP_B_GGM_LUT_G[89] */
            tuningBuf[0x000033A8>>2] = 0x0000D958; /* 0x150253A8: DIP_B_GGM_LUT_G[90] */
            tuningBuf[0x000033AC>>2] = 0x000035A9; /* 0x150253AC: DIP_B_GGM_LUT_G[91] */
            tuningBuf[0x000033B0>>2] = 0x0000A94E; /* 0x150253B0: DIP_B_GGM_LUT_G[92] */
            tuningBuf[0x000033B4>>2] = 0x0000D3F6; /* 0x150253B4: DIP_B_GGM_LUT_G[93] */
            tuningBuf[0x000033B8>>2] = 0x00000D3F; /* 0x150253B8: DIP_B_GGM_LUT_G[94] */
            tuningBuf[0x000033BC>>2] = 0x00009276; /* 0x150253BC: DIP_B_GGM_LUT_G[95] */
            tuningBuf[0x000033C0>>2] = 0x0000E1DC; /* 0x150253C0: DIP_B_GGM_LUT_G[96] */
            tuningBuf[0x000033C4>>2] = 0x0000AFB4; /* 0x150253C4: DIP_B_GGM_LUT_G[97] */
            tuningBuf[0x000033C8>>2] = 0x0000F79F; /* 0x150253C8: DIP_B_GGM_LUT_G[98] */
            tuningBuf[0x000033CC>>2] = 0x00001FF1; /* 0x150253CC: DIP_B_GGM_LUT_G[99] */
            tuningBuf[0x000033D0>>2] = 0x00000A7B; /* 0x150253D0: DIP_B_GGM_LUT_G[100] */
            tuningBuf[0x000033D4>>2] = 0x0000BC4D; /* 0x150253D4: DIP_B_GGM_LUT_G[101] */
            tuningBuf[0x000033D8>>2] = 0x0000F204; /* 0x150253D8: DIP_B_GGM_LUT_G[102] */
            tuningBuf[0x000033DC>>2] = 0x00008334; /* 0x150253DC: DIP_B_GGM_LUT_G[103] */
            tuningBuf[0x000033E0>>2] = 0x0000A4A4; /* 0x150253E0: DIP_B_GGM_LUT_G[104] */
            tuningBuf[0x000033E4>>2] = 0x000001E0; /* 0x150253E4: DIP_B_GGM_LUT_G[105] */
            tuningBuf[0x000033E8>>2] = 0x00009C5D; /* 0x150253E8: DIP_B_GGM_LUT_G[106] */
            tuningBuf[0x000033EC>>2] = 0x00008D42; /* 0x150253EC: DIP_B_GGM_LUT_G[107] */
            tuningBuf[0x000033F0>>2] = 0x00006921; /* 0x150253F0: DIP_B_GGM_LUT_G[108] */
            tuningBuf[0x000033F4>>2] = 0x0000AD72; /* 0x150253F4: DIP_B_GGM_LUT_G[109] */
            tuningBuf[0x000033F8>>2] = 0x00006E43; /* 0x150253F8: DIP_B_GGM_LUT_G[110] */
            tuningBuf[0x000033FC>>2] = 0x0000D9C8; /* 0x150253FC: DIP_B_GGM_LUT_G[111] */
            tuningBuf[0x00003400>>2] = 0x00008FBE; /* 0x15025400: DIP_B_GGM_LUT_G[112] */
            tuningBuf[0x00003404>>2] = 0x00005E0B; /* 0x15025404: DIP_B_GGM_LUT_G[113] */
            tuningBuf[0x00003408>>2] = 0x0000CBB1; /* 0x15025408: DIP_B_GGM_LUT_G[114] */
            tuningBuf[0x0000340C>>2] = 0x0000C41C; /* 0x1502540C: DIP_B_GGM_LUT_G[115] */
            tuningBuf[0x00003410>>2] = 0x000080D3; /* 0x15025410: DIP_B_GGM_LUT_G[116] */
            tuningBuf[0x00003414>>2] = 0x0000F698; /* 0x15025414: DIP_B_GGM_LUT_G[117] */
            tuningBuf[0x00003418>>2] = 0x0000F16F; /* 0x15025418: DIP_B_GGM_LUT_G[118] */
            tuningBuf[0x0000341C>>2] = 0x00009D18; /* 0x1502541C: DIP_B_GGM_LUT_G[119] */
            tuningBuf[0x00003420>>2] = 0x00006923; /* 0x15025420: DIP_B_GGM_LUT_G[120] */
            tuningBuf[0x00003424>>2] = 0x000009FA; /* 0x15025424: DIP_B_GGM_LUT_G[121] */
            tuningBuf[0x00003428>>2] = 0x0000CBF8; /* 0x15025428: DIP_B_GGM_LUT_G[122] */
            tuningBuf[0x0000342C>>2] = 0x0000E856; /* 0x1502542C: DIP_B_GGM_LUT_G[123] */
            tuningBuf[0x00003430>>2] = 0x00005476; /* 0x15025430: DIP_B_GGM_LUT_G[124] */
            tuningBuf[0x00003434>>2] = 0x00002008; /* 0x15025434: DIP_B_GGM_LUT_G[125] */
            tuningBuf[0x00003438>>2] = 0x0000E70F; /* 0x15025438: DIP_B_GGM_LUT_G[126] */
            tuningBuf[0x0000343C>>2] = 0x0000DAFB; /* 0x1502543C: DIP_B_GGM_LUT_G[127] */
            tuningBuf[0x00003440>>2] = 0x00001F75; /* 0x15025440: DIP_B_GGM_LUT_G[128] */
            tuningBuf[0x00003444>>2] = 0x0000D91F; /* 0x15025444: DIP_B_GGM_LUT_G[129] */
            tuningBuf[0x00003448>>2] = 0x00004430; /* 0x15025448: DIP_B_GGM_LUT_G[130] */
            tuningBuf[0x0000344C>>2] = 0x0000375E; /* 0x1502544C: DIP_B_GGM_LUT_G[131] */
            tuningBuf[0x00003450>>2] = 0x000027CB; /* 0x15025450: DIP_B_GGM_LUT_G[132] */
            tuningBuf[0x00003454>>2] = 0x0000D6E6; /* 0x15025454: DIP_B_GGM_LUT_G[133] */
            tuningBuf[0x00003458>>2] = 0x0000BD2D; /* 0x15025458: DIP_B_GGM_LUT_G[134] */
            tuningBuf[0x0000345C>>2] = 0x00004148; /* 0x1502545C: DIP_B_GGM_LUT_G[135] */
            tuningBuf[0x00003460>>2] = 0x0000E03E; /* 0x15025460: DIP_B_GGM_LUT_G[136] */
            tuningBuf[0x00003464>>2] = 0x0000B386; /* 0x15025464: DIP_B_GGM_LUT_G[137] */
            tuningBuf[0x00003468>>2] = 0x00005405; /* 0x15025468: DIP_B_GGM_LUT_G[138] */
            tuningBuf[0x0000346C>>2] = 0x00007CF4; /* 0x1502546C: DIP_B_GGM_LUT_G[139] */
            tuningBuf[0x00003470>>2] = 0x0000810A; /* 0x15025470: DIP_B_GGM_LUT_G[140] */
            tuningBuf[0x00003474>>2] = 0x0000AF77; /* 0x15025474: DIP_B_GGM_LUT_G[141] */
            tuningBuf[0x00003478>>2] = 0x000032BE; /* 0x15025478: DIP_B_GGM_LUT_G[142] */
            tuningBuf[0x0000347C>>2] = 0x000019F6; /* 0x1502547C: DIP_B_GGM_LUT_G[143] */
            tuningBuf[0x00003480>>2] = 0x00000001; /* 0x15025480: DIP_B_GGM_CTRL */
            pIspReg->DIP_X_CTL_RGB_EN.Bits.GGM_EN = 1;
            break;
        case tuning_tag_UDM:
            pIspReg->DIP_X_UDM_INTP_CRS.Raw = 0x38303060;
            pIspReg->DIP_X_UDM_INTP_NAT.Raw = 0x1430063F;
            pIspReg->DIP_X_UDM_INTP_AUG.Raw = 0x00600600;
            pIspReg->DIP_X_UDM_LUMA_LUT1.Raw = 0x07FF0100;
            pIspReg->DIP_X_UDM_LUMA_LUT2.Raw = 0x02008020;
            pIspReg->DIP_X_UDM_SL_CTL.Raw = 0x003FFFE0;
            pIspReg->DIP_X_UDM_HFTD_CTL.Raw = 0x08421000;
            pIspReg->DIP_X_UDM_NR_STR.Raw = 0x81028000;
            pIspReg->DIP_X_UDM_NR_ACT.Raw = 0x00000050;
            pIspReg->DIP_X_UDM_HF_STR.Raw = 0x00000000;
            pIspReg->DIP_X_UDM_HF_ACT1.Raw = 0x145034DC;
            pIspReg->DIP_X_UDM_HF_ACT2.Raw = 0x0034FF55;
            pIspReg->DIP_X_UDM_CLIP.Raw = 0x00DF2064;
            pIspReg->DIP_X_UDM_DSB.Raw = 0x007FA800|fgModeRegBit;
            pIspReg->DIP_X_UDM_TILE_EDGE.Raw = 0x0000000F;
            pIspReg->DIP_X_UDM_DSL.Raw = 0x00000000;
            pIspReg->DIP_X_UDM_SPARE_1.Raw = 0x00000000;
            pIspReg->DIP_X_UDM_SPARE_2.Raw = 0x00000000;
            pIspReg->DIP_X_UDM_SPARE_3.Raw = 0x00000000;
            pIspReg->DIP_X_CTL_RGB_EN.Bits.UDM_EN = 1;
            break;
        default:
            break;
    }
}

}
}
}

