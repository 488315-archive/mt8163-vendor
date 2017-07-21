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
#define LOG_TAG "StereoTuningProvider"

#include <vsdof/hal/stereo_tuning_provider.h>
#include <vsdof/hal/stereo_common.h>
#include <vsdof/hal/stereo_setting_provider.h>

#include <vsdof/hal/rapidjson/writer.h>
#include <vsdof/hal/rapidjson/stringbuffer.h>
#include <vsdof/hal/rapidjson/document.h>     // rapidjson's DOM-style API
#include <vsdof/hal/rapidjson/prettywriter.h> // for stringify JSON
#include <vsdof/hal/rapidjson/filewritestream.h>
using namespace rapidjson;

using namespace StereoHAL;

#define PROPERTY_TUNING_PREFIX  STEREO_PROPERTY_PREFIX"tuning."
#define PROPERTY_DPE_TUNING     PROPERTY_TUNING_PREFIX"dpe"     //debug.STEREO.tuning.dpe, 1 to read/write tuning file
#define PROPERTY_WMF_TUNING     PROPERTY_TUNING_PREFIX"wmf"     //debug.STEREO.tuning.wmf,  1 to read/write tuning file
#define PROPERTY_GF_TUNING      PROPERTY_TUNING_PREFIX"gf"      //debug.STEREO.tuning.gf,   value of 0, 1, 2
#define PROPERTY_BOKEH_TUNING   PROPERTY_TUNING_PREFIX"bokeh"   //debug.STEREO.tuning.bokeh, 1 to read/write tuning file

#define DPE_TUNING_FILE_PATH    "/sdcard/tuning/dpe_params"    //0: Not to read tuning file
#define WMF_TUNING_FILE_PATH    "/sdcard/tuning/wmf_params"
#define BOKEH_TUNING_FILE_PATH  "/sdcard/tuning/bokeh_params"

const bool LOG_ENABLED = StereoSettingProvider::isLogEnabled(PERPERTY_DEPTHMAP_NODE_LOG);
const bool LOG_ENABLED_BOKEH = StereoSettingProvider::isLogEnabled(PERPERTY_BOKEH_NODE_LOG);

#define STEREO_TUNING_PROVIDER_DEBUG

#ifdef STEREO_TUNING_PROVIDER_DEBUG    // Enable debug log.

#undef __func__
#define __func__ __FUNCTION__

#ifndef GTEST
#define MY_LOGD(fmt, arg...)    CAM_LOGD("[%s]" fmt, __func__, ##arg)
#define MY_LOGI(fmt, arg...)    CAM_LOGI("[%s]" fmt, __func__, ##arg)
#define MY_LOGW(fmt, arg...)    CAM_LOGW("[%s] WRN(%5d):" fmt, __func__, __LINE__, ##arg)
#define MY_LOGE(fmt, arg...)    CAM_LOGE("[%s] %s ERROR(%5d):" fmt, __func__,__FILE__, __LINE__, ##arg)
#else
#define MY_LOGD(fmt, arg...)    printf("[D][%s]" fmt"\n", __func__, ##arg)
#define MY_LOGI(fmt, arg...)    printf("[I][%s]" fmt"\n", __func__, ##arg)
#define MY_LOGW(fmt, arg...)    printf("[W][%s] WRN(%5d):" fmt"\n", __func__, __LINE__, ##arg)
#define MY_LOGE(fmt, arg...)    printf("[E][%s] %s ERROR(%5d):" fmt"\n", __func__,__FILE__, __LINE__, ##arg)
#endif

#else   // Disable debug log.
#define MY_LOGD(a,...)
#define MY_LOGI(a,...)
#define MY_LOGW(a,...)
#define MY_LOGE(a,...)
#endif  // STEREO_TUNING_PROVIDER_DEBUG

#define MY_LOGV_IF(cond, arg...)    if (cond) { MY_LOGV(arg); }
#define MY_LOGD_IF(cond, arg...)    if (cond) { MY_LOGD(arg); }
#define MY_LOGI_IF(cond, arg...)    if (cond) { MY_LOGI(arg); }
#define MY_LOGW_IF(cond, arg...)    if (cond) { MY_LOGW(arg); }
#define MY_LOGE_IF(cond, arg...)    if (cond) { MY_LOGE(arg); }

void
StereoTuningProvider::initDPETuningInfo(DVEConfig *tuningBuffer)
{
    if(NULL == tuningBuffer) {
        return;
    }

    DVEConfig config;
    ::memcpy(tuningBuffer, &config, sizeof(config));    //recover defaul value

    tuningBuffer->Dve_l_Bbox_En                         = 1;
    tuningBuffer->Dve_r_Bbox_En                         = 1;
    tuningBuffer->Dve_Mask_En                           = 1;

    tuningBuffer->Dve_Org_Horz_Sr_0                     = 255;
    tuningBuffer->Dve_Org_Horz_Sr_1                     = 128;

    tuningBuffer->Dve_Org_l_Bbox.DVE_ORG_BBOX_LEFT      = 0;
    tuningBuffer->Dve_Org_l_Bbox.DVE_ORG_BBOX_BOTTOM    = 1152;
    tuningBuffer->Dve_Org_l_Bbox.DVE_ORG_BBOX_TOP       = 0;
    tuningBuffer->Dve_Org_l_Bbox.DVE_ORG_BBOX_RIGHT     = 2176;
    tuningBuffer->Dve_Org_r_Bbox.DVE_ORG_BBOX_LEFT      = 128;
    tuningBuffer->Dve_Org_r_Bbox.DVE_ORG_BBOX_BOTTOM    = 1120;
    tuningBuffer->Dve_Org_r_Bbox.DVE_ORG_BBOX_TOP       = 32;
    tuningBuffer->Dve_Org_r_Bbox.DVE_ORG_BBOX_RIGHT     = 2048;

    tuningBuffer->Dve_Cand_Num              = 7;
    tuningBuffer->Dve_Cand_0.DVE_CAND_SEL   = 0xB;
    tuningBuffer->Dve_Cand_0.DVE_CAND_TYPE  = 0x1;
    tuningBuffer->Dve_Cand_1.DVE_CAND_SEL   = 0x12;
    tuningBuffer->Dve_Cand_1.DVE_CAND_TYPE  = 0x2;
    tuningBuffer->Dve_Cand_2.DVE_CAND_SEL   = 0x7;
    tuningBuffer->Dve_Cand_2.DVE_CAND_TYPE  = 0x1;
    tuningBuffer->Dve_Cand_3.DVE_CAND_SEL   = 0x1F;
    tuningBuffer->Dve_Cand_3.DVE_CAND_TYPE  = 0x6;
    tuningBuffer->Dve_Cand_4.DVE_CAND_SEL   = 0x19;
    tuningBuffer->Dve_Cand_4.DVE_CAND_TYPE  = 0x3;
    tuningBuffer->Dve_Cand_5.DVE_CAND_SEL   = 0x1A;
    tuningBuffer->Dve_Cand_5.DVE_CAND_TYPE  = 0x3;
    tuningBuffer->Dve_Cand_6.DVE_CAND_SEL   = 0x1C;
    tuningBuffer->Dve_Cand_6.DVE_CAND_TYPE  = 0x4;
    tuningBuffer->Dve_Cand_7.DVE_CAND_SEL   = 0x1C;
    tuningBuffer->Dve_Cand_7.DVE_CAND_TYPE  = 0x4;
    tuningBuffer->DVE_VERT_GMV  = 0;
    tuningBuffer->DVE_HORZ_GMV  = 0;

    tuningBuffer->Dve_Type_Penality_Ctrl.DVE_RAND_COST      = 24;
    tuningBuffer->Dve_Type_Penality_Ctrl.DVE_GMV_COST       = 3;
    tuningBuffer->Dve_Type_Penality_Ctrl.DVE_PREV_COST      = 3;
    tuningBuffer->Dve_Type_Penality_Ctrl.DVE_NBR_COST       = 5;
    tuningBuffer->Dve_Type_Penality_Ctrl.DVE_REFINE_COST    = 3;
    tuningBuffer->Dve_Type_Penality_Ctrl.DVE_TMPR_COST      = 5;
    tuningBuffer->Dve_Type_Penality_Ctrl.DVE_SPTL_COST      = 1;
}

bool
StereoTuningProvider::getDPETuningInfo(DVEConfig *tuningBuffer)
{
    if(NULL == tuningBuffer) {
        return false;
    }

    static bool useDefault = true;
    bool result = true;

    int propVal = checkStereoProperty(PROPERTY_DPE_TUNING);
    MY_LOGD_IF(LOG_ENABLED, "%s: %d", PROPERTY_DPE_TUNING, propVal);
    if(propVal > 0) {
        Document document;
        document.SetObject();

        //if DPE_TUNING_FILE_PATH exists
        struct stat st;
        if(0 == stat(DPE_TUNING_FILE_PATH, &st)) {
            //Only read if updated
            static time_t sLastModifyTime = 0;
            if(st.st_mtime > sLastModifyTime) {
                //  read the file
                MY_LOGD_IF(LOG_ENABLED, "Read DPE parameters...");
                DVEConfig config;
                FILE *fp = fopen(DPE_TUNING_FILE_PATH, "r");
                if(NULL == fp) {
                    MY_LOGW("Cannot read DPE tuning file(err: %s)", ::strerror(errno));
                    result = false;
                } else {
                    //Get file size
                    fseek(fp, 0, SEEK_END);
                    long fsize = ftell(fp);
                    fseek(fp, 0, SEEK_SET);

                    size_t read_size = 0;
                    char *tuningContent = new(std::nothrow) char[fsize+1];
                    if(NULL == tuningContent) {
                        result = false;
                        MY_LOGE("Cannot allocate tuning buffer");
                    } else {
                        read_size = fread(tuningContent, fsize, 1, fp);
                    }
                    fclose(fp);

                    if(read_size <= 0) {
                        result = false;
                        MY_LOGE("No tuning info");
                    } else {
                        document.Parse(tuningContent);
                        if(!document.IsObject()) {
                            result = false;
                            MY_LOGW("Parse DPE tuning info failed");
                        } else {
                            useDefault = false;
                            sLastModifyTime = st.st_mtime;

                            #ifdef GTEST
                            StringBuffer sb;
                            PrettyWriter<StringBuffer> swriter(sb);
                            swriter.SetIndent(' ', 4);
                            document.Accept(swriter);    // Accept() traverses the DOM and generates Handler events.
                            printf("Read DPE params:\n%s\n", sb.GetString());
                            #endif

                            tuningBuffer->Dve_l_Bbox_En = document["Dve_l_Bbox_En"].GetInt();
                            tuningBuffer->Dve_r_Bbox_En = document["Dve_r_Bbox_En"].GetInt();
                            tuningBuffer->Dve_Mask_En   = document["Dve_Mask_En"].GetInt();

                            tuningBuffer->Dve_Org_Horz_Sr_0 = document["Dve_Org_Horz_Sr_0"].GetInt();
                            tuningBuffer->Dve_Org_Horz_Sr_1 = document["Dve_Org_Horz_Sr_1"].GetInt();

                            Value& val = document["Dve_Org_l_Bbox"];
                            tuningBuffer->Dve_Org_l_Bbox.DVE_ORG_BBOX_LEFT   = val["DVE_ORG_BBOX_LEFT"].GetInt();
                            tuningBuffer->Dve_Org_l_Bbox.DVE_ORG_BBOX_BOTTOM = val["DVE_ORG_BBOX_BOTTOM"].GetInt();
                            tuningBuffer->Dve_Org_l_Bbox.DVE_ORG_BBOX_TOP    = val["DVE_ORG_BBOX_TOP"].GetInt();
                            tuningBuffer->Dve_Org_l_Bbox.DVE_ORG_BBOX_RIGHT  = val["DVE_ORG_BBOX_RIGHT"].GetInt();

                            val = document["Dve_Org_r_Bbox"];
                            tuningBuffer->Dve_Org_r_Bbox.DVE_ORG_BBOX_LEFT   = val["DVE_ORG_BBOX_LEFT"].GetInt();
                            tuningBuffer->Dve_Org_r_Bbox.DVE_ORG_BBOX_BOTTOM = val["DVE_ORG_BBOX_BOTTOM"].GetInt();
                            tuningBuffer->Dve_Org_r_Bbox.DVE_ORG_BBOX_TOP    = val["DVE_ORG_BBOX_TOP"].GetInt();
                            tuningBuffer->Dve_Org_r_Bbox.DVE_ORG_BBOX_RIGHT  = val["DVE_ORG_BBOX_RIGHT"].GetInt();

                            tuningBuffer->Dve_Cand_Num = document["Dve_Cand_Num"].GetInt();

                            val = document["Dve_Cand_0"];
                            tuningBuffer->Dve_Cand_0.DVE_CAND_SEL  = val["DVE_CAND_SEL"].GetInt();
                            tuningBuffer->Dve_Cand_0.DVE_CAND_TYPE = val["DVE_CAND_TYPE"].GetInt();
                            val = document["Dve_Cand_1"];
                            tuningBuffer->Dve_Cand_1.DVE_CAND_SEL  = val["DVE_CAND_SEL"].GetInt();
                            tuningBuffer->Dve_Cand_1.DVE_CAND_TYPE = val["DVE_CAND_TYPE"].GetInt();
                            val = document["Dve_Cand_2"];
                            tuningBuffer->Dve_Cand_2.DVE_CAND_SEL  = val["DVE_CAND_SEL"].GetInt();
                            tuningBuffer->Dve_Cand_2.DVE_CAND_TYPE = val["DVE_CAND_TYPE"].GetInt();
                            val = document["Dve_Cand_3"];
                            tuningBuffer->Dve_Cand_3.DVE_CAND_SEL  = val["DVE_CAND_SEL"].GetInt();
                            tuningBuffer->Dve_Cand_3.DVE_CAND_TYPE = val["DVE_CAND_TYPE"].GetInt();
                            val = document["Dve_Cand_4"];
                            tuningBuffer->Dve_Cand_4.DVE_CAND_SEL  = val["DVE_CAND_SEL"].GetInt();
                            tuningBuffer->Dve_Cand_4.DVE_CAND_TYPE = val["DVE_CAND_TYPE"].GetInt();
                            val = document["Dve_Cand_5"];
                            tuningBuffer->Dve_Cand_5.DVE_CAND_SEL  = val["DVE_CAND_SEL"].GetInt();
                            tuningBuffer->Dve_Cand_5.DVE_CAND_TYPE = val["DVE_CAND_TYPE"].GetInt();
                            val = document["Dve_Cand_6"];
                            tuningBuffer->Dve_Cand_6.DVE_CAND_SEL  = val["DVE_CAND_SEL"].GetInt();
                            tuningBuffer->Dve_Cand_6.DVE_CAND_TYPE = val["DVE_CAND_TYPE"].GetInt();
                            val = document["Dve_Cand_7"];
                            tuningBuffer->Dve_Cand_7.DVE_CAND_SEL  = val["DVE_CAND_SEL"].GetInt();
                            tuningBuffer->Dve_Cand_7.DVE_CAND_TYPE = val["DVE_CAND_TYPE"].GetInt();

                            tuningBuffer->DVE_VERT_GMV = document["DVE_VERT_GMV"].GetInt();
                            tuningBuffer->DVE_HORZ_GMV = document["DVE_HORZ_GMV"].GetInt();

                            val = document["Dve_Type_Penality_Ctrl"];
                            tuningBuffer->Dve_Type_Penality_Ctrl.DVE_RAND_COST      = val["DVE_RAND_COST"].GetInt();
                            tuningBuffer->Dve_Type_Penality_Ctrl.DVE_GMV_COST       = val["DVE_GMV_COST"].GetInt();
                            tuningBuffer->Dve_Type_Penality_Ctrl.DVE_PREV_COST      = val["DVE_PREV_COST"].GetInt();
                            tuningBuffer->Dve_Type_Penality_Ctrl.DVE_NBR_COST       = val["DVE_NBR_COST"].GetInt();
                            tuningBuffer->Dve_Type_Penality_Ctrl.DVE_REFINE_COST    = val["DVE_REFINE_COST"].GetInt();
                            tuningBuffer->Dve_Type_Penality_Ctrl.DVE_TMPR_COST      = val["DVE_TMPR_COST"].GetInt();
                            tuningBuffer->Dve_Type_Penality_Ctrl.DVE_SPTL_COST      = val["DVE_SPTL_COST"].GetInt();
                            MY_LOGD_IF(LOG_ENABLED, "Read DPE parameters...done");
                        }
                    }

                    if(tuningContent) {
                        delete [] tuningContent;
                    }
                }
            }
        } else {
            //  dump config to file
            FILE* fp = fopen(DPE_TUNING_FILE_PATH, "w+"); // non-Windows use "w"
            if(NULL == fp)
            {
                result = false;
                MY_LOGW("Cannot open DPE tuning file(err: %s)", ::strerror(errno));
            }
            else
            {
                MY_LOGD_IF(LOG_ENABLED, "Dump DPE parameters...");
                if(useDefault) {
                    initDPETuningInfo(tuningBuffer);
                }

                Document::AllocatorType& allocator = document.GetAllocator();

                document.AddMember("Dve_l_Bbox_En", (int)tuningBuffer->Dve_l_Bbox_En, allocator);
                document.AddMember("Dve_r_Bbox_En", (int)tuningBuffer->Dve_r_Bbox_En, allocator);
                document.AddMember("Dve_Mask_En", (int)tuningBuffer->Dve_Mask_En, allocator);

                document.AddMember("Dve_Org_Horz_Sr_0", (int)tuningBuffer->Dve_Org_Horz_Sr_0, allocator);
                document.AddMember("Dve_Org_Horz_Sr_1", (int)tuningBuffer->Dve_Org_Horz_Sr_1, allocator);

                Value l_Bbox(kObjectType);
                l_Bbox.AddMember("DVE_ORG_BBOX_LEFT", tuningBuffer->Dve_Org_l_Bbox.DVE_ORG_BBOX_LEFT, allocator);
                l_Bbox.AddMember("DVE_ORG_BBOX_BOTTOM", tuningBuffer->Dve_Org_l_Bbox.DVE_ORG_BBOX_BOTTOM, allocator);
                l_Bbox.AddMember("DVE_ORG_BBOX_TOP", tuningBuffer->Dve_Org_l_Bbox.DVE_ORG_BBOX_TOP, allocator);
                l_Bbox.AddMember("DVE_ORG_BBOX_RIGHT", tuningBuffer->Dve_Org_l_Bbox.DVE_ORG_BBOX_RIGHT, allocator);
                document.AddMember("Dve_Org_l_Bbox", l_Bbox, allocator);

                Value r_Bbox(kObjectType);
                r_Bbox.AddMember("DVE_ORG_BBOX_LEFT", tuningBuffer->Dve_Org_r_Bbox.DVE_ORG_BBOX_LEFT, allocator);
                r_Bbox.AddMember("DVE_ORG_BBOX_BOTTOM", tuningBuffer->Dve_Org_r_Bbox.DVE_ORG_BBOX_BOTTOM, allocator);
                r_Bbox.AddMember("DVE_ORG_BBOX_TOP", tuningBuffer->Dve_Org_r_Bbox.DVE_ORG_BBOX_TOP, allocator);
                r_Bbox.AddMember("DVE_ORG_BBOX_RIGHT", tuningBuffer->Dve_Org_r_Bbox.DVE_ORG_BBOX_RIGHT, allocator);
                document.AddMember("Dve_Org_r_Bbox", r_Bbox, allocator);

                document.AddMember("Dve_Cand_Num", tuningBuffer->Dve_Cand_Num, allocator);

                Value cand_0(kObjectType);
                cand_0.AddMember("DVE_CAND_SEL", tuningBuffer->Dve_Cand_0.DVE_CAND_SEL, allocator);
                cand_0.AddMember("DVE_CAND_TYPE", tuningBuffer->Dve_Cand_0.DVE_CAND_TYPE, allocator);
                document.AddMember("Dve_Cand_0", cand_0, allocator);

                Value cand_1(kObjectType);
                cand_1.AddMember("DVE_CAND_SEL", tuningBuffer->Dve_Cand_1.DVE_CAND_SEL, allocator);
                cand_1.AddMember("DVE_CAND_TYPE", tuningBuffer->Dve_Cand_1.DVE_CAND_TYPE, allocator);
                document.AddMember("Dve_Cand_1", cand_1, allocator);

                Value cand_2(kObjectType);
                cand_2.AddMember("DVE_CAND_SEL", tuningBuffer->Dve_Cand_2.DVE_CAND_SEL, allocator);
                cand_2.AddMember("DVE_CAND_TYPE", tuningBuffer->Dve_Cand_2.DVE_CAND_TYPE, allocator);
                document.AddMember("Dve_Cand_2", cand_2, allocator);

                Value cand_3(kObjectType);
                cand_3.AddMember("DVE_CAND_SEL", tuningBuffer->Dve_Cand_3.DVE_CAND_SEL, allocator);
                cand_3.AddMember("DVE_CAND_TYPE", tuningBuffer->Dve_Cand_3.DVE_CAND_TYPE, allocator);
                document.AddMember("Dve_Cand_3", cand_3, allocator);

                Value cand_4(kObjectType);
                cand_4.AddMember("DVE_CAND_SEL", tuningBuffer->Dve_Cand_4.DVE_CAND_SEL, allocator);
                cand_4.AddMember("DVE_CAND_TYPE", tuningBuffer->Dve_Cand_4.DVE_CAND_TYPE, allocator);
                document.AddMember("Dve_Cand_4", cand_4, allocator);

                Value cand_5(kObjectType);
                cand_5.AddMember("DVE_CAND_SEL", tuningBuffer->Dve_Cand_5.DVE_CAND_SEL, allocator);
                cand_5.AddMember("DVE_CAND_TYPE", tuningBuffer->Dve_Cand_5.DVE_CAND_TYPE, allocator);
                document.AddMember("Dve_Cand_5", cand_5, allocator);

                Value cand_6(kObjectType);
                cand_6.AddMember("DVE_CAND_SEL", tuningBuffer->Dve_Cand_6.DVE_CAND_SEL, allocator);
                cand_6.AddMember("DVE_CAND_TYPE", tuningBuffer->Dve_Cand_6.DVE_CAND_TYPE, allocator);
                document.AddMember("Dve_Cand_6", cand_6, allocator);

                Value cand_7(kObjectType);
                cand_7.AddMember("DVE_CAND_SEL", tuningBuffer->Dve_Cand_7.DVE_CAND_SEL, allocator);
                cand_7.AddMember("DVE_CAND_TYPE", tuningBuffer->Dve_Cand_7.DVE_CAND_TYPE, allocator);
                document.AddMember("Dve_Cand_7", cand_7, allocator);

                document.AddMember("DVE_VERT_GMV", tuningBuffer->DVE_VERT_GMV, allocator);
                document.AddMember("DVE_HORZ_GMV", tuningBuffer->DVE_HORZ_GMV, allocator);

                Value dve_Type_Penality_Ctrl(kObjectType);
                dve_Type_Penality_Ctrl.AddMember("DVE_RAND_COST", tuningBuffer->Dve_Type_Penality_Ctrl.DVE_RAND_COST, allocator);
                dve_Type_Penality_Ctrl.AddMember("DVE_GMV_COST", tuningBuffer->Dve_Type_Penality_Ctrl.DVE_GMV_COST, allocator);
                dve_Type_Penality_Ctrl.AddMember("DVE_PREV_COST", tuningBuffer->Dve_Type_Penality_Ctrl.DVE_PREV_COST, allocator);
                dve_Type_Penality_Ctrl.AddMember("DVE_NBR_COST", tuningBuffer->Dve_Type_Penality_Ctrl.DVE_NBR_COST, allocator);
                dve_Type_Penality_Ctrl.AddMember("DVE_REFINE_COST", tuningBuffer->Dve_Type_Penality_Ctrl.DVE_REFINE_COST, allocator);
                dve_Type_Penality_Ctrl.AddMember("DVE_TMPR_COST", tuningBuffer->Dve_Type_Penality_Ctrl.DVE_TMPR_COST, allocator);
                dve_Type_Penality_Ctrl.AddMember("DVE_SPTL_COST", tuningBuffer->Dve_Type_Penality_Ctrl.DVE_SPTL_COST, allocator);
                document.AddMember("Dve_Type_Penality_Ctrl", dve_Type_Penality_Ctrl, allocator);

                char writeBuffer[1024];
                FileWriteStream os(fp, writeBuffer, sizeof(writeBuffer));
                PrettyWriter<FileWriteStream> writer(os);
                writer.SetIndent(' ', 4);
                document.Accept(writer);

                fflush(fp);
                fclose(fp);

                MY_LOGD_IF(LOG_ENABLED, "Dump DPE parameters...done");

                #ifdef GTEST
                StringBuffer sb;
                PrettyWriter<StringBuffer> swriter(sb);
                swriter.SetIndent(' ', 4);
                document.Accept(swriter);    // Accept() traverses the DOM and generates Handler events.
                printf("Write DPE params:\n%s\n", sb.GetString());
                #endif
            }
        }
    }

    if(useDefault) {
        initDPETuningInfo(tuningBuffer);
    }

    MY_LOGD_IF(LOG_ENABLED, "===== DPE Parameters =====");
    MY_LOGD_IF(LOG_ENABLED, "Dve_l_Bbox_En:                         %d", tuningBuffer->Dve_l_Bbox_En);
    MY_LOGD_IF(LOG_ENABLED, "Dve_r_Bbox_En:                         %d", tuningBuffer->Dve_r_Bbox_En);

    MY_LOGD_IF(LOG_ENABLED, "Dve_Org_l_Bbox.DVE_ORG_BBOX_LEFT:      %d", tuningBuffer->Dve_Org_l_Bbox.DVE_ORG_BBOX_LEFT);
    MY_LOGD_IF(LOG_ENABLED, "Dve_Org_l_Bbox.DVE_ORG_BBOX_BOTTOM:    %d", tuningBuffer->Dve_Org_l_Bbox.DVE_ORG_BBOX_BOTTOM);
    MY_LOGD_IF(LOG_ENABLED, "Dve_Org_l_Bbox.DVE_ORG_BBOX_TOP:       %d", tuningBuffer->Dve_Org_l_Bbox.DVE_ORG_BBOX_TOP);
    MY_LOGD_IF(LOG_ENABLED, "Dve_Org_l_Bbox.DVE_ORG_BBOX_RIGHT:     %d", tuningBuffer->Dve_Org_l_Bbox.DVE_ORG_BBOX_RIGHT);
    MY_LOGD_IF(LOG_ENABLED, "Dve_Org_r_Bbox.DVE_ORG_BBOX_LEFT:      %d", tuningBuffer->Dve_Org_r_Bbox.DVE_ORG_BBOX_LEFT);
    MY_LOGD_IF(LOG_ENABLED, "Dve_Org_r_Bbox.DVE_ORG_BBOX_BOTTOM:    %d", tuningBuffer->Dve_Org_r_Bbox.DVE_ORG_BBOX_BOTTOM);
    MY_LOGD_IF(LOG_ENABLED, "Dve_Org_r_Bbox.DVE_ORG_BBOX_TOP:       %d", tuningBuffer->Dve_Org_r_Bbox.DVE_ORG_BBOX_TOP);
    MY_LOGD_IF(LOG_ENABLED, "Dve_Org_r_Bbox.DVE_ORG_BBOX_RIGHT:     %d", tuningBuffer->Dve_Org_r_Bbox.DVE_ORG_BBOX_RIGHT);

    MY_LOGD_IF(LOG_ENABLED, "Dve_Cand_Num:                          %d", tuningBuffer->Dve_Cand_Num);
    MY_LOGD_IF(LOG_ENABLED, "Dve_Cand_0.DVE_CAND_SEL:               0x%X", tuningBuffer->Dve_Cand_0.DVE_CAND_SEL);
    MY_LOGD_IF(LOG_ENABLED, "Dve_Cand_0.DVE_CAND_TYPE:              0x%X", tuningBuffer->Dve_Cand_0.DVE_CAND_TYPE);
    MY_LOGD_IF(LOG_ENABLED, "Dve_Cand_1.DVE_CAND_SEL:               0x%X", tuningBuffer->Dve_Cand_1.DVE_CAND_SEL);
    MY_LOGD_IF(LOG_ENABLED, "Dve_Cand_1.DVE_CAND_TYPE:              0x%X", tuningBuffer->Dve_Cand_1.DVE_CAND_TYPE);
    MY_LOGD_IF(LOG_ENABLED, "Dve_Cand_2.DVE_CAND_SEL:               0x%X", tuningBuffer->Dve_Cand_2.DVE_CAND_SEL);
    MY_LOGD_IF(LOG_ENABLED, "Dve_Cand_2.DVE_CAND_TYPE:              0x%X", tuningBuffer->Dve_Cand_2.DVE_CAND_TYPE);
    MY_LOGD_IF(LOG_ENABLED, "Dve_Cand_3.DVE_CAND_SEL:               0x%X", tuningBuffer->Dve_Cand_3.DVE_CAND_SEL);
    MY_LOGD_IF(LOG_ENABLED, "Dve_Cand_3.DVE_CAND_TYPE:              0x%X", tuningBuffer->Dve_Cand_3.DVE_CAND_TYPE);
    MY_LOGD_IF(LOG_ENABLED, "Dve_Cand_4.DVE_CAND_SEL:               0x%X", tuningBuffer->Dve_Cand_4.DVE_CAND_SEL);
    MY_LOGD_IF(LOG_ENABLED, "Dve_Cand_4.DVE_CAND_TYPE:              0x%X", tuningBuffer->Dve_Cand_4.DVE_CAND_TYPE);
    MY_LOGD_IF(LOG_ENABLED, "Dve_Cand_5.DVE_CAND_SEL:               0x%X", tuningBuffer->Dve_Cand_5.DVE_CAND_SEL);
    MY_LOGD_IF(LOG_ENABLED, "Dve_Cand_5.DVE_CAND_TYPE:              0x%X", tuningBuffer->Dve_Cand_5.DVE_CAND_TYPE);
    MY_LOGD_IF(LOG_ENABLED, "Dve_Cand_6.DVE_CAND_SEL:               0x%X", tuningBuffer->Dve_Cand_6.DVE_CAND_SEL);
    MY_LOGD_IF(LOG_ENABLED, "Dve_Cand_6.DVE_CAND_TYPE:              0x%X", tuningBuffer->Dve_Cand_6.DVE_CAND_TYPE);
    MY_LOGD_IF(LOG_ENABLED, "Dve_Cand_7.DVE_CAND_SEL:               0x%X", tuningBuffer->Dve_Cand_7.DVE_CAND_SEL);
    MY_LOGD_IF(LOG_ENABLED, "Dve_Cand_7.DVE_CAND_TYPE:              0x%X", tuningBuffer->Dve_Cand_7.DVE_CAND_TYPE);

    MY_LOGD_IF(LOG_ENABLED, "DVE_VERT_GMV:                          %d", tuningBuffer->DVE_VERT_GMV);
    MY_LOGD_IF(LOG_ENABLED, "DVE_HORZ_GMV:                          %d", tuningBuffer->DVE_HORZ_GMV);

    MY_LOGD_IF(LOG_ENABLED, "Dve_Type_Penality_Ctrl.DVE_RAND_COST   %d", tuningBuffer->Dve_Type_Penality_Ctrl.DVE_RAND_COST);
    MY_LOGD_IF(LOG_ENABLED, "Dve_Type_Penality_Ctrl.DVE_GMV_COST    %d", tuningBuffer->Dve_Type_Penality_Ctrl.DVE_GMV_COST);
    MY_LOGD_IF(LOG_ENABLED, "Dve_Type_Penality_Ctrl.DVE_PREV_COST   %d", tuningBuffer->Dve_Type_Penality_Ctrl.DVE_PREV_COST);
    MY_LOGD_IF(LOG_ENABLED, "Dve_Type_Penality_Ctrl.DVE_NBR_COST    %d", tuningBuffer->Dve_Type_Penality_Ctrl.DVE_NBR_COST);
    MY_LOGD_IF(LOG_ENABLED, "Dve_Type_Penality_Ctrl.DVE_REFINE_COST %d", tuningBuffer->Dve_Type_Penality_Ctrl.DVE_REFINE_COST);
    MY_LOGD_IF(LOG_ENABLED, "Dve_Type_Penality_Ctrl.DVE_TMPR_COST   %d", tuningBuffer->Dve_Type_Penality_Ctrl.DVE_TMPR_COST);
    MY_LOGD_IF(LOG_ENABLED, "Dve_Type_Penality_Ctrl.DVE_SPTL_COST   %d", tuningBuffer->Dve_Type_Penality_Ctrl.DVE_SPTL_COST);

    return result;
}

bool
StereoTuningProvider::getWMFTuningInfo(WMFEFILTERSIZE &size, void *tbliBuffer)
{
    size = WMFE_FILTER_SIZE_5x5;

    if(NULL == tbliBuffer) {
        return false;
    }

    bool result = true;

    const int TBLI_SIZE = 128;
    const short DEFAULT_TBLI[] = {
        1000,   920,    846,    778,    716,    659,    606,    558,    //  8
        513,    472,    434,    399,    367,    338,    311,    286,    // 16
        263,    242,    223,    205,    188,    173,    159,    147,    // 24
        135,    124,    114,    105,    96,     89,     82,     75,     // 32
        69,     63,     58,     54,     49,     45,     42,     38,     // 40
        35,     32,     30,     27,     25,     23,     21,     19,     // 48
        18,     16,     15,     14,     13,     12,     11,     10,     // 56
        9,      8,      7,      7,      6,      6,      5,      5,      // 64
        4,      4,      4,      3,      3,      3,      2,      2,      // 72
        2,      2,      2,      1,      1,      1,      1,      1,      // 80
        1,      1,      1,      1,      1,      1,      1,      1,      // 88
        1,      1,      1,      1,      1,      1,      1,      1,      // 96
        1,      1,      1,      1,      1,      1,      1,      1,      //104
        1,      1,      1,      1,      1,      1,      1,      1,      //112
        1,      1,      1,      1,      1,      1,      1,      1,      //120
        1,      1,      1,      1,      1,      1,      1,      1       //128
    };
    short *tbli = const_cast<short *>(DEFAULT_TBLI);

    static bool useDefault = true;

    int propVal = checkStereoProperty(PROPERTY_WMF_TUNING);
    MY_LOGD_IF(LOG_ENABLED, "%s: %d", PROPERTY_WMF_TUNING, propVal);
    if(propVal > 0) {
        Document document;
        document.SetObject();

        //if WMF_TUNING_FILE_PATH exists
        struct stat st;
        if(0 == stat(WMF_TUNING_FILE_PATH, &st)) {
            static time_t sLastModifyTime = 0;
            if(st.st_mtime > sLastModifyTime) {
                useDefault = false;
                sLastModifyTime = st.st_mtime;

                //  read the file
                MY_LOGD_IF("Read WMF parameters...");

                FILE *fp = fopen(WMF_TUNING_FILE_PATH, "r");
                if(NULL == fp) {
                    result = false;
                    MY_LOGW("Cannot read WMF tuning file(err: %s)", ::strerror(errno));
                } else {
                    //Get file size
                    fseek(fp, 0, SEEK_END);
                    long fsize = ftell(fp);
                    fseek(fp, 0, SEEK_SET);

                    size_t read_size = 0;
                    char *tuningContent = new(std::nothrow) char[fsize+1];
                    if(NULL == tuningContent) {
                        MY_LOGE("Cannot allocate tuning buffer");
                    } else {
                        read_size = fread(tuningContent, fsize, 1, fp);
                    }
                    fclose(fp);

                    if(read_size <= 0) {
                        result = false;
                        MY_LOGE("No tuning info");
                    } else {
                        document.Parse(tuningContent);
                        if(!document.IsObject()) {
                            result = false;
                            MY_LOGW("Parse WMF tuning info failed");
                        } else {
                            #ifdef GTEST
                            StringBuffer sb;
                            PrettyWriter<StringBuffer> swriter(sb);
                            swriter.SetIndent(' ', 4);
                            document.Accept(swriter);    // Accept() traverses the DOM and generates Handler events.
                            printf("Read WMF params:\n%s\n", sb.GetString());
                            #endif

                            const char *strSize = document["WMFEFILTERSIZE"].GetString();
                            if(NULL != strSize) {
                                if(!strcmp(strSize, "WMFE_FILTER_SIZE_1x1")) {
                                    size = WMFE_FILTER_SIZE_1x1;
                                } else if(!strcmp(strSize, "WMFE_FILTER_SIZE_3x3")) {
                                    size = WMFE_FILTER_SIZE_3x3;
                                } else if(!strcmp(strSize, "WMFE_FILTER_SIZE_7x7")) {
                                    size = WMFE_FILTER_SIZE_7x7;
                                } else {
                                    size = WMFE_FILTER_SIZE_5x5;
                                }
                            }

                            Value& val = document["tbli"];
                            if(TBLI_SIZE != val.Size()) {
                                MY_LOGE("Read %d elements, should be %d", val.Size(), TBLI_SIZE);
                            } else {
                                tbli = (short *)tbliBuffer;
                                for(int i = 0; i < TBLI_SIZE; i++) {
                                    tbli[i] = val[i].GetInt();
                                }
                            }
                            MY_LOGD_IF(LOG_ENABLED, "Read WMF parameters...done");
                        }
                    }

                    if(tuningContent) {
                        delete [] tuningContent;
                    }
                }
            }
        } else {
            //  dump config to file
            MY_LOGD_IF(LOG_ENABLED, "Dump WMF parameters...");
            FILE *fp = fopen(WMF_TUNING_FILE_PATH, "w+");
            if(NULL == fp)
            {
                result = false;
                MY_LOGW("Cannot open WMF tuning file(err: %s)", ::strerror(errno));
            }
            else
            {
                Document::AllocatorType& allocator = document.GetAllocator();

                switch(size) {
                case WMFE_FILTER_SIZE_1x1:
                    document.AddMember("WMFEFILTERSIZE", "WMFE_FILTER_SIZE_1x1", allocator);
                    break;
                case WMFE_FILTER_SIZE_3x3:
                    document.AddMember("WMFEFILTERSIZE", "WMFE_FILTER_SIZE_3x3", allocator);
                    break;
                case WMFE_FILTER_SIZE_7x7:
                    document.AddMember("WMFEFILTERSIZE", "WMFE_FILTER_SIZE_7x7", allocator);
                    break;
                case WMFE_FILTER_SIZE_5x5:
                default:
                    document.AddMember("WMFEFILTERSIZE", "WMFE_FILTER_SIZE_5x5", allocator);
                    break;
                }

                Value val(kArrayType);
                const short *TBLI = (useDefault) ? DEFAULT_TBLI : (short *)tbliBuffer;
                for(int i = 0; i < TBLI_SIZE; i++) {
                    val.PushBack(Value(TBLI[i]).Move(), allocator);
                }
                document.AddMember("tbli", val, allocator);

                char writeBuffer[1024];
                FileWriteStream os(fp, writeBuffer, sizeof(writeBuffer));
                PrettyWriter<FileWriteStream> writer(os);
                writer.SetIndent(' ', 4);
                document.Accept(writer);

                fflush(fp);
                fclose(fp);

                #ifdef GTEST
                StringBuffer sb;
                PrettyWriter<StringBuffer> swriter(sb);
                swriter.SetIndent(' ', 4);
                document.Accept(swriter);    // Accept() traverses the DOM and generates Handler events.
                printf("Write WMF params:\n%s\n", sb.GetString());
                #endif
            }
            MY_LOGD_IF(LOG_ENABLED, "Dump WMF parameters...done");
        }
    }

    if(useDefault) {
        ::memcpy(tbliBuffer, DEFAULT_TBLI, sizeof(DEFAULT_TBLI));
    }

    MY_LOGD_IF(LOG_ENABLED, "===== WMF TBLI =====");
    for(int i = 0; i < TBLI_SIZE; i++) {
        if((i+1) % 8 == 0) {
            MY_LOGD_IF(LOG_ENABLED, "%4d, %4d, %4d, %4d, %4d, %4d, %4d, %4d,",
                       tbli[i-7], tbli[i-6], tbli[i-5], tbli[i-4],
                       tbli[i-3], tbli[i-2], tbli[i-1], tbli[i]);
        }
    }
    return result;
}

ENUM_CLEAR_REGION
StereoTuningProvider::getGFTuningInfo()
{
    int propVal = checkStereoProperty(PROPERTY_GF_TUNING);
    MY_LOGD_IF(LOG_ENABLED_BOKEH, "%s: %d", PROPERTY_GF_TUNING, propVal);
    if(propVal >= E_CLEAR_REGION_SMALL && propVal <= E_CLEAR_REGION_LARGE) {
        return (ENUM_CLEAR_REGION)propVal;
    }

    return E_CLEAR_REGION_MEDIUM;
}

void
StereoTuningProvider::initBokehTuningInfo(dip_x_reg_t *tuning, ENUM_BOKEH_STRENGTH eBokehStrength)
{
    if(NULL == tuning) {
        return;
    }

    tuning->DIP_X_CTL_YUV_EN.Bits.NBC2_EN       = 1;  //Enable NBC2
    tuning->DIP_X_ANR2_CON1.Bits.ANR2_MODE      = 1;
    tuning->DIP_X_ANR2_CON1.Bits.ANR2_ENC       = 1;
    tuning->DIP_X_ANR2_CON1.Bits.ANR2_LCE_LINK  = 0; //Must be 0

    switch(eBokehStrength) {
//        case E_BOKEH_STRENGTH_WEAK:
//            tuning->DIP_X_BOK_CON.Bits.BOK_MODE;
//            tuning->DIP_X_BOK_CON.Bits.BOK_AP_MODE;
//            tuning->DIP_X_BOK_CON.Bits.BOK_FGBG_MODE;
//            tuning->DIP_X_BOK_CON.Bits.BOK_FGBG_WT;
//
//            tuning->DIP_X_BOK_TUN.Bits.BOK_STR_WT;
//            tuning->DIP_X_BOK_TUN.Bits.BOK_WT_GAIN;
//            tuning->DIP_X_BOK_TUN.Bits.BOK_INTENSITY;
//            tuning->DIP_X_BOK_TUN.Bits.BOK_DOF_M;
//
//            tuning->DIP_X_BOK_OFF.Bits.BOK_XOFF;
//            tuning->DIP_X_BOK_OFF.Bits.BOK_YOFF;
//
//            tuning->DIP_X_BOK_RSV1.Bits.BOK_RSV1;
//            break;
//        case E_BOKEH_STRENGTH_STRONG:
//            tuning->DIP_X_BOK_CON.Bits.BOK_MODE;
//            tuning->DIP_X_BOK_CON.Bits.BOK_AP_MODE;
//            tuning->DIP_X_BOK_CON.Bits.BOK_FGBG_MODE;
//            tuning->DIP_X_BOK_CON.Bits.BOK_FGBG_WT;
//
//            tuning->DIP_X_BOK_TUN.Bits.BOK_STR_WT;
//            tuning->DIP_X_BOK_TUN.Bits.BOK_WT_GAIN;
//            tuning->DIP_X_BOK_TUN.Bits.BOK_INTENSITY;
//            tuning->DIP_X_BOK_TUN.Bits.BOK_DOF_M;
//
//            tuning->DIP_X_BOK_OFF.Bits.BOK_XOFF;
//            tuning->DIP_X_BOK_OFF.Bits.BOK_YOFF;
//
//            tuning->DIP_X_BOK_RSV1.Bits.BOK_RSV1;
//            break;
        case E_BOKEH_STRENGTH_NORMAL:
        default:
            tuning->DIP_X_BOK_CON.Bits.BOK_MODE         = 0;    //Should be 0
            tuning->DIP_X_BOK_CON.Bits.BOK_AP_MODE      = 0;
            tuning->DIP_X_BOK_CON.Bits.BOK_FGBG_MODE    = 0;
            tuning->DIP_X_BOK_CON.Bits.BOK_FGBG_WT      = 4;

            tuning->DIP_X_BOK_TUN.Bits.BOK_STR_WT       = 63;
            tuning->DIP_X_BOK_TUN.Bits.BOK_WT_GAIN      = 63;
            tuning->DIP_X_BOK_TUN.Bits.BOK_INTENSITY    = 255;
            tuning->DIP_X_BOK_TUN.Bits.BOK_DOF_M        = 24;

            tuning->DIP_X_BOK_OFF.Bits.BOK_XOFF         = 0;
            tuning->DIP_X_BOK_OFF.Bits.BOK_YOFF         = 0;

            tuning->DIP_X_BOK_RSV1.Bits.BOK_RSV1        = 0;
            break;
    }
}

bool
StereoTuningProvider::getBokehTuningInfo(void *tuningBuffer, ENUM_BOKEH_STRENGTH eBokehStrength)
{
    dip_x_reg_t *tuning = (dip_x_reg_t *)tuningBuffer;

    static bool useDefault = true;
    bool result = true;

    int propVal = checkStereoProperty(PROPERTY_BOKEH_TUNING);
    MY_LOGD_IF(LOG_ENABLED_BOKEH, "%s: %d", PROPERTY_BOKEH_TUNING, propVal);
    if(propVal > 0) {
        Document document;
        document.SetObject();

        //if BOKEH_TUNING_FILE_PATH exists
        struct stat st;
        if(0 == stat(BOKEH_TUNING_FILE_PATH, &st)) {
            static time_t sLastModifyTime = 0;
            if(st.st_mtime > sLastModifyTime) {
                useDefault = false;
                sLastModifyTime = st.st_mtime;

                //  read the file
                MY_LOGD_IF(LOG_ENABLED_BOKEH, "Read Bokeh parameters...");

                FILE *fp = fopen(BOKEH_TUNING_FILE_PATH, "r");
                if(NULL == fp) {
                    result = false;
                    MY_LOGW("Cannot read Bokeh tuning file(err: %s)", ::strerror(errno));
                } else {
                    //Get file size
                    fseek(fp, 0, SEEK_END);
                    long fsize = ftell(fp);
                    fseek(fp, 0, SEEK_SET);

                    size_t read_size = 0;
                    char *tuningContent = new(std::nothrow) char[fsize+1];
                    if(NULL == tuningContent) {
                        result = false;
                        MY_LOGE("Cannot allocate tuning buffer");
                    } else {
                        read_size = fread(tuningContent, fsize, 1, fp);
                    }
                    fclose(fp);

                    if(read_size <= 0) {
                        result = false;
                        MY_LOGE("No tuning info");
                    } else {
                        document.Parse(tuningContent);
                        if(!document.IsObject()) {
                            result = false;
                            MY_LOGW("Parse Bokeh tuning info failed");
                        } else {
                            #ifdef GTEST
                            StringBuffer sb;
                            PrettyWriter<StringBuffer> swriter(sb);
                            swriter.SetIndent(' ', 4);
                            document.Accept(swriter);    // Accept() traverses the DOM and generates Handler events.
                            printf("Read Bokeh params:\n%s\n", sb.GetString());
                            #endif

                            Value& val = document["DIP_X_BOK_CON"];
                            tuning->DIP_X_BOK_CON.Bits.BOK_MODE = val["BOK_MODE"].GetInt();
                            tuning->DIP_X_BOK_CON.Bits.BOK_AP_MODE = val["BOK_AP_MODE"].GetInt();
                            tuning->DIP_X_BOK_CON.Bits.BOK_FGBG_MODE = val["BOK_FGBG_MODE"].GetInt();
                            tuning->DIP_X_BOK_CON.Bits.BOK_FGBG_WT = val["BOK_FGBG_WT"].GetInt();

                            val = document["DIP_X_BOK_TUN"];
                            tuning->DIP_X_BOK_TUN.Bits.BOK_STR_WT = val["BOK_STR_WT"].GetInt();
                            tuning->DIP_X_BOK_TUN.Bits.BOK_WT_GAIN = val["BOK_WT_GAIN"].GetInt();
                            tuning->DIP_X_BOK_TUN.Bits.BOK_INTENSITY = val["BOK_INTENSITY"].GetInt();
                            tuning->DIP_X_BOK_TUN.Bits.BOK_DOF_M = val["BOK_DOF_M"].GetInt();

                            val = document["DIP_X_BOK_OFF"];
                            tuning->DIP_X_BOK_OFF.Bits.BOK_XOFF = val["BOK_XOFF"].GetInt();
                            tuning->DIP_X_BOK_OFF.Bits.BOK_YOFF = val["BOK_YOFF"].GetInt();;

                            tuning->DIP_X_BOK_RSV1.Bits.BOK_RSV1 = document["DIP_X_BOK_RSV1"].GetInt();

                            MY_LOGD_IF(LOG_ENABLED_BOKEH, "Read Bokeh parameters...done");
                        }
                    }

                    if(tuningContent) {
                        delete [] tuningContent;
                    }
                }
            }
        } else {
            //  dump config to file
            MY_LOGD_IF(LOG_ENABLED_BOKEH, "Dump Bokeh parameters...");
            FILE *fp = fopen(BOKEH_TUNING_FILE_PATH, "w+");
            if(NULL == fp)
            {
                result = false;
                MY_LOGW("Cannot open Bokeh tuning file(err: %s)", ::strerror(errno));
            }
            else
            {
                if(useDefault) {
                    initBokehTuningInfo(tuning, eBokehStrength);
                }

                Document::AllocatorType& allocator = document.GetAllocator();

                Value DIP_X_BOK_CON(kObjectType);
                DIP_X_BOK_CON.AddMember("BOK_MODE", tuning->DIP_X_BOK_CON.Bits.BOK_MODE, allocator);
                DIP_X_BOK_CON.AddMember("BOK_AP_MODE", tuning->DIP_X_BOK_CON.Bits.BOK_AP_MODE, allocator);
                DIP_X_BOK_CON.AddMember("BOK_FGBG_MODE", tuning->DIP_X_BOK_CON.Bits.BOK_FGBG_MODE, allocator);
                DIP_X_BOK_CON.AddMember("BOK_FGBG_WT", tuning->DIP_X_BOK_CON.Bits.BOK_FGBG_WT, allocator);
                document.AddMember("DIP_X_BOK_CON", DIP_X_BOK_CON, allocator);

                Value DIP_X_BOK_TUN(kObjectType);
                DIP_X_BOK_TUN.AddMember("BOK_STR_WT", tuning->DIP_X_BOK_TUN.Bits.BOK_STR_WT, allocator);
                DIP_X_BOK_TUN.AddMember("BOK_WT_GAIN", tuning->DIP_X_BOK_TUN.Bits.BOK_WT_GAIN, allocator);
                DIP_X_BOK_TUN.AddMember("BOK_INTENSITY", tuning->DIP_X_BOK_TUN.Bits.BOK_INTENSITY, allocator);
                DIP_X_BOK_TUN.AddMember("BOK_DOF_M", tuning->DIP_X_BOK_TUN.Bits.BOK_DOF_M, allocator);
                document.AddMember("DIP_X_BOK_TUN", DIP_X_BOK_TUN, allocator);

                Value DIP_X_BOK_OFF(kObjectType);
                DIP_X_BOK_OFF.AddMember("BOK_XOFF", tuning->DIP_X_BOK_OFF.Bits.BOK_XOFF, allocator);
                DIP_X_BOK_OFF.AddMember("BOK_YOFF", tuning->DIP_X_BOK_OFF.Bits.BOK_YOFF, allocator);
                document.AddMember("DIP_X_BOK_OFF", DIP_X_BOK_OFF, allocator);

                document.AddMember("DIP_X_BOK_RSV1", tuning->DIP_X_BOK_RSV1.Bits.BOK_RSV1, allocator);

                char writeBuffer[1024];
                FileWriteStream os(fp, writeBuffer, sizeof(writeBuffer));
                PrettyWriter<FileWriteStream> writer(os);
                writer.SetIndent(' ', 4);
                document.Accept(writer);

                fflush(fp);
                fclose(fp);

                #ifdef GTEST
                StringBuffer sb;
                PrettyWriter<StringBuffer> swriter(sb);
                swriter.SetIndent(' ', 4);
                document.Accept(swriter);    // Accept() traverses the DOM and generates Handler events.
                printf("Write Bokeh params:\n%s\n", sb.GetString());
                #endif
            }
            MY_LOGD_IF(LOG_ENABLED_BOKEH, "Dump Bokeh parameters...done");
        }
    }

    if(useDefault) {
        initBokehTuningInfo(tuning, eBokehStrength);
    }

    //Dump log
    MY_LOGD_IF(LOG_ENABLED_BOKEH, "===== Bokeh Parameters =====");
    MY_LOGD_IF(LOG_ENABLED_BOKEH, "DIP_X_CTL_YUV_EN.Bits.NBC2_EN:       %d", tuning->DIP_X_CTL_YUV_EN.Bits.NBC2_EN);
    MY_LOGD_IF(LOG_ENABLED_BOKEH, "DIP_X_ANR2_CON1.Bits.ANR2_MODE:      %d", tuning->DIP_X_ANR2_CON1.Bits.ANR2_MODE);
    MY_LOGD_IF(LOG_ENABLED_BOKEH, "DIP_X_ANR2_CON1.Bits.ANR2_ENC:       %d", tuning->DIP_X_ANR2_CON1.Bits.ANR2_ENC);
    MY_LOGD_IF(LOG_ENABLED_BOKEH, "DIP_X_ANR2_CON1.Bits.ANR2_LCE_LINK:  %d", tuning->DIP_X_ANR2_CON1.Bits.ANR2_LCE_LINK);

    MY_LOGD_IF(LOG_ENABLED_BOKEH, "DIP_X_BOK_CON.Bits.BOK_MODE:         %d", tuning->DIP_X_BOK_CON.Bits.BOK_MODE);
    MY_LOGD_IF(LOG_ENABLED_BOKEH, "DIP_X_BOK_CON.Bits.BOK_AP_MODE:      %d", tuning->DIP_X_BOK_CON.Bits.BOK_AP_MODE);
    MY_LOGD_IF(LOG_ENABLED_BOKEH, "DIP_X_BOK_CON.Bits.BOK_FGBG_MODE:    %d", tuning->DIP_X_BOK_CON.Bits.BOK_FGBG_MODE);
    MY_LOGD_IF(LOG_ENABLED_BOKEH, "DIP_X_BOK_CON.Bits.BOK_FGBG_WT:      %d", tuning->DIP_X_BOK_CON.Bits.BOK_FGBG_WT);

    MY_LOGD_IF(LOG_ENABLED_BOKEH, "DIP_X_BOK_TUN.Bits.BOK_STR_WT:       %d", tuning->DIP_X_BOK_TUN.Bits.BOK_STR_WT);
    MY_LOGD_IF(LOG_ENABLED_BOKEH, "DIP_X_BOK_TUN.Bits.BOK_WT_GAIN:      %d", tuning->DIP_X_BOK_TUN.Bits.BOK_WT_GAIN);
    MY_LOGD_IF(LOG_ENABLED_BOKEH, "DIP_X_BOK_TUN.Bits.BOK_INTENSITY:    %d", tuning->DIP_X_BOK_TUN.Bits.BOK_INTENSITY);
    MY_LOGD_IF(LOG_ENABLED_BOKEH, "DIP_X_BOK_TUN.Bits.BOK_DOF_M:        %d", tuning->DIP_X_BOK_TUN.Bits.BOK_DOF_M);

    MY_LOGD_IF(LOG_ENABLED_BOKEH, "DIP_X_BOK_OFF.Bits.BOK_XOFF:         %d", tuning->DIP_X_BOK_OFF.Bits.BOK_XOFF);
    MY_LOGD_IF(LOG_ENABLED_BOKEH, "DIP_X_BOK_OFF.Bits.BOK_YOFF:         %d", tuning->DIP_X_BOK_OFF.Bits.BOK_YOFF);

    MY_LOGD_IF(LOG_ENABLED_BOKEH, "DIP_X_BOK_RSV1.Bits.BOK_RSV1:        %d", tuning->DIP_X_BOK_RSV1.Bits.BOK_RSV1);

    return result;
}

bool
StereoTuningProvider::getFMTuningInfo(ENUM_FM_DIRECTION direction, DIP_X_REG_FM_SIZE &fmSizeInfo, DIP_X_REG_FM_TH_CON0 &fmThInfo)
{
    if(E_FM_R_TO_L == direction) {
        fmSizeInfo.Bits.FM_OFFSET_X  = 20;
        fmSizeInfo.Bits.FM_OFFSET_Y  = 16;
        fmSizeInfo.Bits.FM_SR_TYPE   = 0;

        fmThInfo.Bits.FM_MIN_RATIO   = 90;
        fmThInfo.Bits.FM_SAD_TH      = 1023;
        fmThInfo.Bits.FM_RES_TH      = 0;
    } else {
        fmSizeInfo.Bits.FM_OFFSET_X  = 12;
        fmSizeInfo.Bits.FM_OFFSET_Y  = 16;
        fmSizeInfo.Bits.FM_SR_TYPE   = 0;

        fmThInfo.Bits.FM_MIN_RATIO   = 90;
        fmThInfo.Bits.FM_SAD_TH      = 1023;
        fmThInfo.Bits.FM_RES_TH      = 0;
    }

    return true;
}
