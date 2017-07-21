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
#include <LegacyPipeline/stereo/ContextBuilder/ImageStreamManager.h>
#include <LegacyPipeline/stereo/ContextBuilder/MetaStreamManager.h>
//
#define ZSD_MAIN2_P1_IMGO_ENABLE    0
//
#define VDO_MAIN1_P1_IMGO_ENABLE    0
#define VDO_MAIN2_P1_IMGO_ENABLE    0
//
/*******************************************************************************
*
* Preview Scenario
*
*******************************************************************************/
namespace StereoPipelineMgrData
{
namespace p1
{
namespace prv
{
static const struct MetaStreamManager::metadata_info_setting gStereoMetaTbl_PrvP1[] =
{
    //
    {"App:Meta:Control", eSTREAMID_META_APP_CONTROL,    eSTREAMTYPE_META_IN,    10, 1, eStreamType_META_APP},
    {"Hal:Meta:Control", eSTREAMID_META_HAL_CONTROL,    eSTREAMTYPE_META_IN,    10, 1, eStreamType_META_HAL},
    //
    {"App:Meta:ResultP1", eSTREAMID_META_APP_DYNAMIC_P1, eSTREAMTYPE_META_OUT,   10, 1, eStreamType_META_APP},
    {"Hal:Meta:ResultP1", eSTREAMID_META_HAL_DYNAMIC_P1, eSTREAMTYPE_META_INOUT, 10, 1, eStreamType_META_HAL},
    //
    {"", 0, 0, 0, 0},
};
//
static struct ImageStreamManager::image_stream_info_pool_setting gStereoImgStreamTbl_PrvP1[] =
{
    { //RRZO. P1 output sizes will be updated later when constructing P1 pipelines
        "Hal:Image:ResizeRaw", eSTREAMID_IMAGE_PIPE_RAW_RESIZER, eSTREAMTYPE_IMAGE_INOUT, 6, 4,
        eBUFFER_USAGE_SW_READ_OFTEN |eBUFFER_USAGE_HW_CAMERA_READWRITE,
        eImgFmt_FG_BAYER10, MSize(0, 0), 0, 0, eStreamType_IMG_HAL_PROVIDER, NULL
    },
    //End
    {"", 0, 0, 0, 0, 0, 0, MSize(), 0, 0, eStreamType(), 0},
};
//
static const MINT32 gStereoConnectSetting_PrvP1[] =
{
    eNODEID_P1Node,
    -1,
    -1,
};
//
static const struct MetaStreamManager::metadata_info_setting gStereoMetaTbl_PrvP1Main2[] =
{
    //
    {"App:Meta:Control_Main2",  eSTREAMID_META_APP_CONTROL_MAIN2,   eSTREAMTYPE_META_IN,    10, 1, eStreamType_META_APP},
    {"Hal:Meta:Control_Main2",  eSTREAMID_META_HAL_CONTROL_MAIN2,   eSTREAMTYPE_META_IN,    10, 1, eStreamType_META_HAL},
    //
    {"App:Meta:ResultP1_Main2", eSTREAMID_META_APP_DYNAMIC_P1_MAIN2, eSTREAMTYPE_META_OUT,   10, 1, eStreamType_META_APP},
    {"Hal:Meta:ResultP1_Main2", eSTREAMID_META_HAL_DYNAMIC_P1_MAIN2, eSTREAMTYPE_META_INOUT, 10, 1, eStreamType_META_HAL},
    //
    {"", 0, 0, 0, 0},
};
//
static struct ImageStreamManager::image_stream_info_pool_setting gStereoImgStreamTbl_PrvP1Main2[] =
{
    { //RRZO. P1 output sizes will be updated later when constructing P1 pipelines
        "Hal:Image:ResizeRaw_Main2", eSTREAMID_IMAGE_PIPE_RAW_RESIZER_01, eSTREAMTYPE_IMAGE_INOUT, 6, 4,
        eBUFFER_USAGE_SW_READ_OFTEN |eBUFFER_USAGE_HW_CAMERA_READWRITE,
        eImgFmt_FG_BAYER10, MSize(0, 0), 0, 0, eStreamType_IMG_HAL_PROVIDER, NULL
    },
    //End
    {"", 0, 0, 0, 0, 0, 0, MSize(), 0, 0, eStreamType(), 0},
};
//
static const MINT32 gStereoConnectSetting_PrvP1Main2[] =
{
    eNODEID_P1Node_main2,
    -1,
    -1,
};

}
}
}
//
namespace StereoPipelineMgrData
{
namespace p2
{
namespace prv
{
//
// Stereo P2 preview: MetaStreamInfo table
//
static const struct MetaStreamManager::metadata_info_setting gStereoMetaTbl_P2Prv[] =
{
    // Hal
    {"App:Meta:Control",            eSTREAMID_META_APP_CONTROL,          eSTREAMTYPE_META_IN,    0, 0, eStreamType_META_APP},
    {"Hal:Meta:Control",            eSTREAMID_META_HAL_CONTROL,          eSTREAMTYPE_META_IN,    0, 0, eStreamType_META_HAL},
    // DepthMapNode - input
    {"Hal:Meta:P1:Dynamic",         eSTREAMID_META_HAL_DYNAMIC_P1,       eSTREAMTYPE_META_INOUT, 10, 1, eStreamType_META_HAL},
    {"Hal:Meta:P1_main2:Dynamic",   eSTREAMID_META_HAL_DYNAMIC_P1_MAIN2, eSTREAMTYPE_META_INOUT, 10, 1, eStreamType_META_HAL},
    // DepthMapNode - ouput
    {"Hal:Meta:DynamicDepth",       eSTREAMID_META_HAL_DYNAMIC_DEPTH,    eSTREAMTYPE_META_OUT,   10, 1, eStreamType_META_HAL},
    {"APP:Meta:DynamicDepth",       eSTREAMID_META_APP_DYNAMIC_DEPTH,    eSTREAMTYPE_META_OUT,   10, 1, eStreamType_META_APP},
    // BokehNode
    {"Hal:Meta:DynamicBokeh",       eSTREAMID_META_HAL_DYNAMIC_BOKEH,    eSTREAMTYPE_META_INOUT, 10, 1, eStreamType_META_HAL},
    // App
    // BokehNode - Ouput
    {"App:Meta:Bokeh",              eSTREAMID_META_APP_DYNAMIC_BOKEH,    eSTREAMTYPE_META_OUT,   10, 1, eStreamType_META_APP},
    // FD
    {"App:Meta:Depth:FD",           eSTREAMID_META_APP_DYNAMIC_FDYUV,    eSTREAMTYPE_META_OUT,   10, 1, eStreamType_META_APP},
    // End
    {"", 0, 0, 0, 0},
};
//
static struct ImageStreamManager::image_stream_info_pool_setting gStereoImgStreamTbl_P2Prv[] =
{
    // Hal
    // DepthMapNode - input
    {"Hal:Image:Pass1_FullRaw",     eSTREAMID_IMAGE_PIPE_RAW_OPAQUE,               eSTREAMTYPE_IMAGE_INOUT, 0, 0, eBUFFER_USAGE_SW_READ_OFTEN |eBUFFER_USAGE_HW_CAMERA_READWRITE,                              eImgFmt_YV12, MSize(4208, 3120), 0, 0, eStreamType_IMG_HAL_PROVIDER, NULL}, // temp run
    {"Hal:Image:Pass1_ResizeRaw",   eSTREAMID_IMAGE_PIPE_RAW_RESIZER,              eSTREAMTYPE_IMAGE_INOUT, 0, 0, eBUFFER_USAGE_SW_READ_OFTEN |eBUFFER_USAGE_HW_CAMERA_READWRITE,                              eImgFmt_YV12, MSize(2104, 1183), 0, 0, eStreamType_IMG_HAL_PROVIDER, NULL}, // temp run
    {"Hal:Image:Pass2_ResizeRaw",   eSTREAMID_IMAGE_PIPE_RAW_RESIZER_01,           eSTREAMTYPE_IMAGE_INOUT, 0, 0, eBUFFER_USAGE_SW_READ_OFTEN |eBUFFER_USAGE_HW_CAMERA_READWRITE,                              eImgFmt_YV12, MSize(1280, 720), 0, 0, eStreamType_IMG_HAL_PROVIDER, NULL}, // temp run
    // DepthMapNode - output
    // Hal
    {"Hal:Image:MY_S",              eSTREAMID_IMAGE_PIPE_DEPTHMAPNODE_MY_SYUV,         eSTREAMTYPE_IMAGE_INOUT, 5, 5, eBUFFER_USAGE_SW_READ_OFTEN |eBUFFER_USAGE_HW_CAMERA_READWRITE| eBUFFER_USAGE_SW_WRITE_OFTEN, eImgFmt_YV12, MSize(240, 136), 0, 0, eStreamType_IMG_HAL_POOL, NULL},
    {"Hal:Image:DMW",               eSTREAMID_IMAGE_PIPE_DEPTHMAPNODE_DMWYUV,          eSTREAMTYPE_IMAGE_INOUT, 5, 5, eBUFFER_USAGE_SW_READ_OFTEN |eBUFFER_USAGE_HW_CAMERA_READWRITE| eBUFFER_USAGE_SW_WRITE_OFTEN, eImgFmt_YV12, MSize(240, 136), 0, 0, eStreamType_IMG_HAL_POOL, NULL},
    {"Hal:Image:MainImage",         eSTREAMID_IMAGE_PIPE_DEPTHMAPNODE_MAINIMAGEYUV,    eSTREAMTYPE_IMAGE_INOUT, 5, 5, eBUFFER_USAGE_SW_READ_OFTEN |eBUFFER_USAGE_HW_CAMERA_READWRITE,                               eImgFmt_YUY2, MSize(1920, 1080), 0, 0, eStreamType_IMG_HAL_POOL, NULL},
    {"Hal:Image:FD",                eSTREAMID_IMAGE_PIPE_DEPTHMAPNODE_FDYUV,           eSTREAMTYPE_IMAGE_OUT,   5, 5, eBUFFER_USAGE_SW_READ_OFTEN |eBUFFER_USAGE_HW_CAMERA_READWRITE,                               eImgFmt_YUY2, MSize(640, 360), 0, 0, eStreamType_IMG_HAL_POOL, NULL},
    // Bokeh - output
    {"Hal:Image:Preview",           eSTREAMID_IMAGE_PIPE_BOKEHNODE_PREVIEWYUV,         eSTREAMTYPE_IMAGE_INOUT, 5, 5, eBUFFER_USAGE_SW_READ_OFTEN |eBUFFER_USAGE_HW_CAMERA_READWRITE,                               eImgFmt_YV12, MSize(1920, 1080), 0, 0, eStreamType_IMG_HAL_PROVIDER, NULL},
    //End
    {"", 0, 0, 0, 0, 0, 0, MSize(), 0, 0, eStreamType(), 0},
};
//
static const MINT32 gStereoP2ConnectSetting_P2Prv[] =
{
    eNODEID_DepthMapNode,
    eNODEID_BokehNode,
    -1,
    -1,
};
};
};
};

/*******************************************************************************
*
* Capture Scenario
*
*******************************************************************************/
namespace StereoPipelineMgrData
{
namespace p2
{
namespace cap
{
static const struct MetaStreamManager::metadata_info_setting gStereoMetaTbl_P2Cap[] =
{
    // Hal
    {"App:Meta:Control",            eSTREAMID_META_APP_CONTROL,         eSTREAMTYPE_META_IN,    0, 0, eStreamType_META_APP},
    {"Hal:Meta:Control",            eSTREAMID_META_HAL_CONTROL,         eSTREAMTYPE_META_IN,    0, 0, eStreamType_META_HAL},
    // DepthMapNode - input
    {"Hal:Meta:P1:Dynamic",         eSTREAMID_META_HAL_DYNAMIC_P1,       eSTREAMTYPE_META_INOUT, 5, 1, eStreamType_META_HAL},
    {"Hal:Meta:P1_main2:Dynamic",   eSTREAMID_META_HAL_CONTROL_MAIN2,   eSTREAMTYPE_META_INOUT, 5, 1, eStreamType_META_HAL},
    // DepthMapNode - ouput
    {"Hal:Meta:DynamicDepth",       eSTREAMID_META_HAL_DYNAMIC_DEPTH,    eSTREAMTYPE_META_OUT,   1, 1, eStreamType_META_HAL},
    {"APP:Meta:DynamicDepth",       eSTREAMID_META_APP_DYNAMIC_DEPTH,    eSTREAMTYPE_META_OUT,   1, 1, eStreamType_META_APP},
    // BokehNode - Ouput
    {"Hal:Meta:DynamicBokeh",       eSTREAMID_META_HAL_DYNAMIC_BOKEH,    eSTREAMTYPE_META_INOUT, 1, 1, eStreamType_META_HAL},
    {"App:Meta:Bokeh",              eSTREAMID_META_APP_DYNAMIC_BOKEH,    eSTREAMTYPE_META_OUT,   1, 1, eStreamType_META_APP},
    // JpefEnc_BokehNode - output
    {"App:Meta:JpgEnc_Bokeh",       eSTREAMID_META_APP_DYNAMIC_BOKEH_JPG, eSTREAMTYPE_META_OUT,   1, 1, eStreamType_META_APP},
    // JpefEnc_CleanImageNode - output
    {"App:Meta:JpgEnc_Clean",       eSTREAMID_META_APP_DYNAMIC_JPEG,      eSTREAMTYPE_META_OUT,   1, 1, eStreamType_META_APP},
    // JpefEnc_JpsImageNode - output
    {"App:Meta:JpgEnc_JPS",         eSTREAMID_META_APP_DYNAMIC_JPS,  eSTREAMTYPE_META_OUT,   1, 1, eStreamType_META_APP},
    // End
    {"", 0, 0, 0, 0},
};
//
static struct ImageStreamManager::image_stream_info_pool_setting gStereoImgStreamTbl_P2Cap[] =
{
    // Hal
    // DepthMapNode - input
    {"Hal:Image:Pass1_FullRaw",     eSTREAMID_IMAGE_PIPE_RAW_OPAQUE,               eSTREAMTYPE_IMAGE_INOUT, 5, 1, eBUFFER_USAGE_SW_READ_OFTEN |eBUFFER_USAGE_HW_CAMERA_READWRITE,                              eImgFmt_YV12, MSize(4176, 3088), 0, 0, eStreamType_IMG_HAL_PROVIDER, NULL}, // temp run
    {"Hal:Image:Pass1_ResizeRaw",   eSTREAMID_IMAGE_PIPE_RAW_RESIZER,                 eSTREAMTYPE_IMAGE_INOUT, 5, 1, eBUFFER_USAGE_SW_READ_OFTEN |eBUFFER_USAGE_HW_CAMERA_READWRITE,                              eImgFmt_YV12, MSize(2304, 1296), 0, 0, eStreamType_IMG_HAL_PROVIDER, NULL}, // temp run
    {"Hal:Image:Pass2_ResizeRaw",   eSTREAMID_IMAGE_PIPE_RAW_RESIZER_01,           eSTREAMTYPE_IMAGE_INOUT, 5, 1, eBUFFER_USAGE_SW_READ_OFTEN |eBUFFER_USAGE_HW_CAMERA_READWRITE,                              eImgFmt_YV12, MSize(1600, 1200), 0, 0, eStreamType_IMG_HAL_PROVIDER, NULL}, // temp run
    // DepthMapNode - output
    // Hal
    {"Hal:Image:MY_S",              eSTREAMID_IMAGE_PIPE_DEPTHMAPNODE_MY_SYUV,         eSTREAMTYPE_IMAGE_INOUT, 5, 1, eBUFFER_USAGE_SW_READ_OFTEN |eBUFFER_USAGE_HW_CAMERA_READWRITE| eBUFFER_USAGE_SW_WRITE_OFTEN, eImgFmt_YV12, MSize(240, 136), 0, 0, eStreamType_IMG_HAL_POOL, NULL},
    {"Hal:Image:DMW",               eSTREAMID_IMAGE_PIPE_DEPTHMAPNODE_DMWYUV,          eSTREAMTYPE_IMAGE_INOUT, 5, 1, eBUFFER_USAGE_SW_READ_OFTEN |eBUFFER_USAGE_HW_CAMERA_READWRITE| eBUFFER_USAGE_SW_WRITE_OFTEN, eImgFmt_YV12, MSize(240, 136), 0, 0, eStreamType_IMG_HAL_POOL, NULL},
    {"Hal:Image:MainImage_Cap",     eSTREAMID_IMAGE_PIPE_DEPTHMAPNODE_MAINIMAGE_CAPYUV,eSTREAMTYPE_IMAGE_INOUT, 5, 1, eBUFFER_USAGE_SW_READ_OFTEN |eBUFFER_USAGE_HW_CAMERA_READWRITE,                              eImgFmt_YV12, MSize(1920, 1080), 0, 0, eStreamType_IMG_HAL_POOL, NULL},
    {"Hal:Image:JPSMain1",          eSTREAMID_IMAGE_PIPE_DEPTHMAPNODE_JPSMAIN1YUV,     eSTREAMTYPE_IMAGE_INOUT, 5, 1, eBUFFER_USAGE_SW_READ_OFTEN |eBUFFER_USAGE_HW_CAMERA_READWRITE,                              eImgFmt_YV12, MSize(1920, 1080), 0, 0, eStreamType_IMG_HAL_POOL, NULL},
    {"Hal:Image:JPSMain2",          eSTREAMID_IMAGE_PIPE_DEPTHMAPNODE_JPSMAIN2YUV,     eSTREAMTYPE_IMAGE_INOUT, 5, 1, eBUFFER_USAGE_SW_READ_OFTEN |eBUFFER_USAGE_HW_CAMERA_READWRITE,                              eImgFmt_YV12, MSize(1920, 1080), 0, 0, eStreamType_IMG_HAL_POOL, NULL},
    // APP
    {"Hal:Image:DepthMap",          eSTREAMID_IMAGE_PIPE_DEPTHMAPNODE_DEPTHMAPYUV,     eSTREAMTYPE_IMAGE_INOUT, 5, 1, eBUFFER_USAGE_SW_READ_OFTEN |eBUFFER_USAGE_HW_CAMERA_READWRITE,                              eImgFmt_YV12, MSize(480, 270), 0, 0, eStreamType_IMG_HAL_PROVIDER, NULL},
    // Bokeh - output
    {"Hal:Image:CleanImage",        eSTREAMID_IMAGE_PIPE_BOKEHNODE_CLEANIMAGEYUV,      eSTREAMTYPE_IMAGE_INOUT, 1, 1, eBUFFER_USAGE_SW_READ_OFTEN |eBUFFER_USAGE_HW_CAMERA_READWRITE,                              eImgFmt_YV12, MSize(1920, 1080), 0, 0, eStreamType_IMG_HAL_POOL, NULL},
    {"Hal:Image:BokehResult",       eSTREAMID_IMAGE_PIPE_BOKEHNODE_RESULTYUV,          eSTREAMTYPE_IMAGE_INOUT, 1, 1, eBUFFER_USAGE_SW_READ_OFTEN |eBUFFER_USAGE_HW_CAMERA_READWRITE,                              eImgFmt_YV12, MSize(1920, 1080), 0, 0, eStreamType_IMG_HAL_POOL, NULL},
    // JpegEnc_BokehNode
    {"Hal:Image:JpegEnc_Bokeh",     eSTREAMID_IMAGE_PIPE_JPG_Bokeh,                    eSTREAMTYPE_IMAGE_INOUT, 1, 1, eBUFFER_USAGE_SW_READ_OFTEN |eBUFFER_USAGE_HW_CAMERA_READWRITE,                              eImgFmt_YV12, MSize(1920, 1080), 0, 0, eStreamType_IMG_HAL_PROVIDER, NULL},
    // JpegEnc_BokehNode
    {"Hal:Image:JpegEnc_Clean",     eSTREAMID_IMAGE_JPEG,                              eSTREAMTYPE_IMAGE_INOUT, 1, 1, eBUFFER_USAGE_SW_READ_OFTEN |eBUFFER_USAGE_HW_CAMERA_READWRITE,                              eImgFmt_YV12, MSize(1920, 1080), 0, 0, eStreamType_IMG_HAL_PROVIDER, NULL},
    // JpegEnc_BokehNode
    {"Hal:Image:JpegEnc_JPS",       eSTREAMID_IMAGE_PIPE_JPG_JPS,                      eSTREAMTYPE_IMAGE_INOUT, 1, 1, eBUFFER_USAGE_SW_READ_OFTEN |eBUFFER_USAGE_HW_CAMERA_READWRITE,                              eImgFmt_YV12, MSize(1920, 1080), 0, 0, eStreamType_IMG_HAL_PROVIDER, NULL},
    //End
    {"", 0, 0, 0, 0, 0, 0, MSize(), 0, 0, eStreamType(), 0},
};
//
static const MINT32 gStereoP2ConnectSetting_P2Cap[] =
{
    eNODEID_DepthMapNode,    eNODEID_BokehNode,
    eNODEID_DepthMapNode,    eNODEID_DualITNode,
    eNODEID_BokehNode,       eNODEID_JpegNode,
    eNODEID_BokehNode,       eNODEID_JpegNode_Bokeh,
    eNODEID_DualITNode,      eNODEID_JpegNode_JPS,
    -1,
    -1,
};
};
};
};

/*******************************************************************************
*
* Video-Record Scenario
*
*******************************************************************************/
namespace StereoPipelineMgrData
{
namespace p1
{
namespace VdoRec
{
//
static const struct MetaStreamManager::metadata_info_setting gStereoMetaTbl_VdoP1[] =
{
    //
    {"App:Meta:Control", eSTREAMID_META_APP_CONTROL,    eSTREAMTYPE_META_IN,    10, 1, eStreamType_META_APP},
    {"Hal:Meta:Control", eSTREAMID_META_HAL_CONTROL,    eSTREAMTYPE_META_IN,    10, 1, eStreamType_META_HAL},
    //
    {"App:Meta:ResultP1", eSTREAMID_META_APP_DYNAMIC_P1, eSTREAMTYPE_META_OUT,   10, 1, eStreamType_META_APP},
    {"Hal:Meta:ResultP1", eSTREAMID_META_HAL_DYNAMIC_P1, eSTREAMTYPE_META_INOUT, 10, 1, eStreamType_META_HAL},
    //
    {"", 0, 0, 0, 0},
};
//
static struct ImageStreamManager::image_stream_info_pool_setting gStereoImgStreamTbl_VdoP1[] =
{
    { //RRZO. P1 output sizes will be updated later when constructing P1 pipelines
        "Hal:Image:ResizeRaw", eSTREAMID_IMAGE_PIPE_RAW_RESIZER, eSTREAMTYPE_IMAGE_INOUT, 6, 4,
        eBUFFER_USAGE_SW_READ_OFTEN |eBUFFER_USAGE_HW_CAMERA_READWRITE,
        eImgFmt_FG_BAYER10, MSize(0, 0), 0, 0, eStreamType_IMG_HAL_PROVIDER, NULL
    },
#if VDO_MAIN1_P1_IMGO_ENABLE
    { //IMGO. P1 output sizes will be updated later when constructing P1 pipelines
        "Hal:Image:FullRaw", eSTREAMID_IMAGE_PIPE_RAW_OPAQUE, eSTREAMTYPE_IMAGE_INOUT, 6, 4,
        eBUFFER_USAGE_SW_READ_OFTEN |eBUFFER_USAGE_HW_CAMERA_READWRITE,
        eImgFmt_BAYER10, MSize(0, 0), 0, 0, eStreamType_IMG_HAL_PROVIDER, NULL
    },
#endif
    //End
    {"", 0, 0, 0, 0, 0, 0, MSize(), 0, 0, eStreamType(), 0},
};
//
static const MINT32 gStereoConnectSetting_VdoP1[] =
{
    eNODEID_P1Node,
    -1,
    -1,
};
//
static const struct MetaStreamManager::metadata_info_setting gStereoMetaTbl_VdoP1Main2[] =
{
    //
    {"App:Meta:Control_Main2",  eSTREAMID_META_APP_CONTROL_MAIN2,   eSTREAMTYPE_META_IN,    10, 1, eStreamType_META_APP},
    {"Hal:Meta:Control_Main2",  eSTREAMID_META_HAL_CONTROL_MAIN2,   eSTREAMTYPE_META_IN,    10, 1, eStreamType_META_HAL},
    //
    {"App:Meta:ResultP1_Main2", eSTREAMID_META_APP_DYNAMIC_P1_MAIN2, eSTREAMTYPE_META_OUT,   10, 1, eStreamType_META_APP},
    {"Hal:Meta:ResultP1_Main2", eSTREAMID_META_HAL_DYNAMIC_P1_MAIN2, eSTREAMTYPE_META_INOUT, 10, 1, eStreamType_META_HAL},
    //
    {"", 0, 0, 0, 0},
};
//
static struct ImageStreamManager::image_stream_info_pool_setting gStereoImgStreamTbl_VdoP1Main2[] =
{
    { //RRZO. P1 output sizes will be updated later when constructing P1 pipelines
        "Hal:Image:ResizeRaw_Main2", eSTREAMID_IMAGE_PIPE_RAW_RESIZER_01, eSTREAMTYPE_IMAGE_INOUT, 6, 4,
        eBUFFER_USAGE_SW_READ_OFTEN |eBUFFER_USAGE_HW_CAMERA_READWRITE,
        eImgFmt_FG_BAYER10, MSize(0, 0), 0, 0, eStreamType_IMG_HAL_PROVIDER, NULL
    },
    //End
    {"", 0, 0, 0, 0, 0, 0, MSize(), 0, 0, eStreamType(), 0},
};
//
static const MINT32 gStereoConnectSetting_VdoP1Main2[] =
{
    eNODEID_P1Node_main2,
    -1,
    -1,
};

}
}
}
//
namespace StereoPipelineMgrData
{
namespace p2
{
namespace VdoRec
{
//
// Stereo P2 VideoRecord: MetaStreamInfo table
//
static const struct MetaStreamManager::metadata_info_setting gStereoMetaTbl_P2VdoRec[] =
{
    // Hal
    {"App:Meta:Control",            eSTREAMID_META_APP_CONTROL,          eSTREAMTYPE_META_IN,    0, 0, eStreamType_META_APP},
    {"Hal:Meta:Control",            eSTREAMID_META_HAL_CONTROL,          eSTREAMTYPE_META_IN,    0, 0, eStreamType_META_HAL},
    // DepthMapNode - input
    {"Hal:Meta:P1:Dynamic",         eSTREAMID_META_HAL_DYNAMIC_P1,       eSTREAMTYPE_META_INOUT, 10, 1, eStreamType_META_HAL},
    {"Hal:Meta:P1_main2:Dynamic",   eSTREAMID_META_HAL_DYNAMIC_P1_MAIN2, eSTREAMTYPE_META_INOUT, 10, 1, eStreamType_META_HAL},
    // DepthMapNode - ouput
    {"Hal:Meta:DynamicDepth",       eSTREAMID_META_HAL_DYNAMIC_DEPTH,    eSTREAMTYPE_META_OUT,   10, 1, eStreamType_META_HAL},
    {"APP:Meta:DynamicDepth",       eSTREAMID_META_APP_DYNAMIC_DEPTH,    eSTREAMTYPE_META_OUT,   10, 1, eStreamType_META_APP},
    // BokehNode
    {"Hal:Meta:DynamicBokeh",       eSTREAMID_META_HAL_DYNAMIC_BOKEH,    eSTREAMTYPE_META_INOUT, 10, 1, eStreamType_META_HAL},
    // App
    // BokehNode - Ouput
    {"App:Meta:Bokeh",              eSTREAMID_META_APP_DYNAMIC_BOKEH,    eSTREAMTYPE_META_OUT,   10, 1, eStreamType_META_APP},
    // FD
    {"App:Meta:Depth:FD",           eSTREAMID_META_APP_DYNAMIC_FDYUV,    eSTREAMTYPE_META_OUT,   10, 1, eStreamType_META_APP},
    // End
    {"", 0, 0, 0, 0},
};
//
static struct ImageStreamManager::image_stream_info_pool_setting gStereoImgStreamTbl_P2VdoRec[] =
{
    // Hal
    // DepthMapNode - input
    //{"Hal:Image:Pass1_FullRaw",     eSTREAMID_IMAGE_PIPE_RAW_OPAQUE,               eSTREAMTYPE_IMAGE_INOUT, 0, 0, eBUFFER_USAGE_SW_READ_OFTEN |eBUFFER_USAGE_HW_CAMERA_READWRITE,                              eImgFmt_YV12, MSize(4208, 3120), 0, 0, eStreamType_IMG_HAL_PROVIDER, NULL}, // temp run
    {"Hal:Image:Pass1_ResizeRaw",   eSTREAMID_IMAGE_PIPE_RAW_RESIZER,              eSTREAMTYPE_IMAGE_INOUT, 0, 0, eBUFFER_USAGE_SW_READ_OFTEN |eBUFFER_USAGE_HW_CAMERA_READWRITE,                              eImgFmt_YV12, MSize(2104, 1183), 0, 0, eStreamType_IMG_HAL_PROVIDER, NULL}, // temp run
    {"Hal:Image:Pass2_ResizeRaw",   eSTREAMID_IMAGE_PIPE_RAW_RESIZER_01,           eSTREAMTYPE_IMAGE_INOUT, 0, 0, eBUFFER_USAGE_SW_READ_OFTEN |eBUFFER_USAGE_HW_CAMERA_READWRITE,                              eImgFmt_YV12, MSize(1280, 720), 0, 0, eStreamType_IMG_HAL_PROVIDER, NULL}, // temp run
    // DepthMapNode - output
    // Hal
    {"Hal:Image:MY_S",              eSTREAMID_IMAGE_PIPE_DEPTHMAPNODE_MY_SYUV,         eSTREAMTYPE_IMAGE_INOUT, 5, 5, eBUFFER_USAGE_SW_READ_OFTEN |eBUFFER_USAGE_HW_CAMERA_READWRITE| eBUFFER_USAGE_SW_WRITE_OFTEN, eImgFmt_YV12, MSize(240, 136), 0, 0, eStreamType_IMG_HAL_POOL, NULL},
    {"Hal:Image:DMW",               eSTREAMID_IMAGE_PIPE_DEPTHMAPNODE_DMWYUV,          eSTREAMTYPE_IMAGE_INOUT, 5, 5, eBUFFER_USAGE_SW_READ_OFTEN |eBUFFER_USAGE_HW_CAMERA_READWRITE| eBUFFER_USAGE_SW_WRITE_OFTEN, eImgFmt_YV12, MSize(240, 136), 0, 0, eStreamType_IMG_HAL_POOL, NULL},
    {"Hal:Image:MainImage",         eSTREAMID_IMAGE_PIPE_DEPTHMAPNODE_MAINIMAGEYUV,    eSTREAMTYPE_IMAGE_INOUT, 5, 5, eBUFFER_USAGE_SW_READ_OFTEN |eBUFFER_USAGE_HW_CAMERA_READWRITE,                               eImgFmt_YUY2, MSize(1920, 1080), 0, 0, eStreamType_IMG_HAL_POOL, NULL},
    {"Hal:Image:FD",                eSTREAMID_IMAGE_PIPE_DEPTHMAPNODE_FDYUV,           eSTREAMTYPE_IMAGE_OUT,   5, 5, eBUFFER_USAGE_SW_READ_OFTEN |eBUFFER_USAGE_HW_CAMERA_READWRITE,                               eImgFmt_YUY2, MSize(640, 360), 0, 0, eStreamType_IMG_HAL_POOL, NULL},
    // Bokeh - output
    {"Hal:Image:Preview",           eSTREAMID_IMAGE_PIPE_BOKEHNODE_PREVIEWYUV,         eSTREAMTYPE_IMAGE_INOUT, 5, 5, eBUFFER_USAGE_SW_READ_OFTEN |eBUFFER_USAGE_HW_CAMERA_READWRITE,                               eImgFmt_YV12, MSize(1920, 1080), 0, 0, eStreamType_IMG_HAL_PROVIDER, NULL},
    {"Hal:Image:Record",            eSTREAMID_IMAGE_PIPE_BOKEHNODE_RECORDYUV,          eSTREAMTYPE_IMAGE_INOUT, 5, 5, eBUFFER_USAGE_SW_READ_OFTEN |eBUFFER_USAGE_HW_CAMERA_READWRITE,                               eImgFmt_YV12, MSize(1920, 1080), 0, 0, eStreamType_IMG_HAL_PROVIDER, NULL},
    //End
    {"", 0, 0, 0, 0, 0, 0, MSize(), 0, 0, eStreamType(), 0},
};
//
static const MINT32 gStereoP2ConnectSetting_P2VdoRec[] =
{
    eNODEID_DepthMapNode,
    eNODEID_BokehNode,
    -1,
    -1,
};
};
};
};

/*******************************************************************************
*
* ZSD Scenario
*
*******************************************************************************/
namespace StereoPipelineMgrData
{
namespace p1
{
namespace zsd
{
static const struct MetaStreamManager::metadata_info_setting gStereoMetaTbl_ZsdP1[] =
{
    //
    {"App:Meta:Control", eSTREAMID_META_APP_CONTROL,    eSTREAMTYPE_META_IN,    10, 1, eStreamType_META_APP},
    {"Hal:Meta:Control", eSTREAMID_META_HAL_CONTROL,    eSTREAMTYPE_META_IN,    10, 1, eStreamType_META_HAL},
    //
    {"App:Meta:ResultP1", eSTREAMID_META_APP_DYNAMIC_P1, eSTREAMTYPE_META_OUT,   10, 1, eStreamType_META_APP},
    {"Hal:Meta:ResultP1", eSTREAMID_META_HAL_DYNAMIC_P1, eSTREAMTYPE_META_INOUT, 10, 1, eStreamType_META_HAL},
    //
    {"", 0, 0, 0, 0},
};
//
static struct ImageStreamManager::image_stream_info_pool_setting gStereoImgStreamTbl_ZsdP1[] =
{
    { //RRZO. P1 output sizes will be updated later when constructing P1 pipelines
        "Hal:Image:ResizeRaw", eSTREAMID_IMAGE_PIPE_RAW_RESIZER, eSTREAMTYPE_IMAGE_INOUT, 10, 10,
        eBUFFER_USAGE_SW_READ_OFTEN |eBUFFER_USAGE_HW_CAMERA_READWRITE,
        eImgFmt_FG_BAYER10, MSize(0, 0), 0, 0, eStreamType_IMG_HAL_PROVIDER, NULL
    },
    { //IMGO. P1 output sizes will be updated later when constructing P1 pipelines
        "Hal:Image:FullRaw", eSTREAMID_IMAGE_PIPE_RAW_OPAQUE, eSTREAMTYPE_IMAGE_INOUT, 10, 10,
        eBUFFER_USAGE_SW_READ_OFTEN |eBUFFER_USAGE_HW_CAMERA_READWRITE,
        eImgFmt_BAYER10, MSize(0, 0), 0, 0, eStreamType_IMG_HAL_PROVIDER, NULL
    },
    //End
    {"", 0, 0, 0, 0, 0, 0, MSize(), 0, 0, eStreamType(), 0},
};
//
static const MINT32 gStereoConnectSetting_ZsdP1[] =
{
    eNODEID_P1Node,
    -1,
    -1,
};
//
static const struct MetaStreamManager::metadata_info_setting gStereoMetaTbl_ZsdP1Main2[] =
{
    //
    {"App:Meta:Control_Main2",  eSTREAMID_META_APP_CONTROL_MAIN2,   eSTREAMTYPE_META_IN,    10, 1, eStreamType_META_APP},
    {"Hal:Meta:Control_Main2",  eSTREAMID_META_HAL_CONTROL_MAIN2,   eSTREAMTYPE_META_IN,    10, 1, eStreamType_META_HAL},
    //
    {"App:Meta:ResultP1_Main2", eSTREAMID_META_APP_DYNAMIC_P1_MAIN2, eSTREAMTYPE_META_OUT,   10, 1, eStreamType_META_APP},
    {"Hal:Meta:ResultP1_Main2", eSTREAMID_META_HAL_DYNAMIC_P1_MAIN2, eSTREAMTYPE_META_INOUT, 10, 1, eStreamType_META_HAL},
    //
    {"", 0, 0, 0, 0},
};
//
static struct ImageStreamManager::image_stream_info_pool_setting gStereoImgStreamTbl_ZsdP1Main2[] =
{
    { //RRZO. P1 output sizes will be updated later when constructing P1 pipelines
        "Hal:Image:ResizeRaw_Main2", eSTREAMID_IMAGE_PIPE_RAW_RESIZER_01, eSTREAMTYPE_IMAGE_INOUT, 10, 10,
        eBUFFER_USAGE_SW_READ_OFTEN |eBUFFER_USAGE_HW_CAMERA_READWRITE,
        eImgFmt_FG_BAYER10, MSize(0, 0), 0, 0, eStreamType_IMG_HAL_PROVIDER, NULL
    },
#if ZSD_MAIN2_P1_IMGO_ENABLE
    { //IMGO. P1 output sizes will be updated later when constructing P1 pipelines
        "Hal:Image:FullRaw_Main2", eSTREAMID_IMAGE_PIPE_RAW_OPAQUE_MAIN2, eSTREAMTYPE_IMAGE_INOUT, 10, 10,
        eBUFFER_USAGE_SW_READ_OFTEN |eBUFFER_USAGE_HW_CAMERA_READWRITE,
        eImgFmt_BAYER10, MSize(0, 0), 0, 0, eStreamType_IMG_HAL_PROVIDER, NULL
    },
#endif
    //End
    {"", 0, 0, 0, 0, 0, 0, MSize(), 0, 0, eStreamType(), 0},
};
//
static const MINT32 gStereoConnectSetting_ZsdP1Main2[] =
{
    eNODEID_P1Node_main2,
    -1,
    -1,
};

}
}
}

