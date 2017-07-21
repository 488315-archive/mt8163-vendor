/*****************************************************************************
*  Copyright Statement:
*  --------------------
*  This software is protected by Copyright and the information contained
*  herein is confidential. The software may not be copied and the information
*  contained herein may not be used or disclosed except with the written
*  permission of MediaTek Inc. (C) 2008
*
*  BY OPENING THIS FILE, BUYER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
*  THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE")
*  RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO BUYER ON
*  AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL WARRANTIES,
*  EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
*  MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
*  NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
*  SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
*  SUPPLIED WITH THE MEDIATEK SOFTWARE, AND BUYER AGREES TO LOOK ONLY TO SUCH
*  THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. MEDIATEK SHALL ALSO
*  NOT BE RESPONSIBLE FOR ANY MEDIATEK SOFTWARE RELEASES MADE TO BUYER'S
*  SPECIFICATION OR TO CONFORM TO A PARTICULAR STANDARD OR OPEN FORUM.
*
*  BUYER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND CUMULATIVE
*  LIABILITY WITH RESPECT TO THE MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
*  AT MEDIATEK'S OPTION, TO REVISE OR REPLACE THE MEDIATEK SOFTWARE AT ISSUE,
*  OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY BUYER TO
*  MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
*
*  THE TRANSACTION CONTEMPLATED HEREUNDER SHALL BE CONSTRUED IN ACCORDANCE
*  WITH THE LAWS OF THE STATE OF CALIFORNIA, USA, EXCLUDING ITS CONFLICT OF
*  LAWS PRINCIPLES.  ANY DISPUTES, CONTROVERSIES OR CLAIMS ARISING THEREOF AND
*  RELATED THERETO SHALL BE SETTLED BY ARBITRATION IN SAN FRANCISCO, CA, UNDER
*  THE RULES OF THE INTERNATIONAL CHAMBER OF COMMERCE (ICC).
*
***************************************************************/
#ifndef MPE_API_C
#define MPE_API_C

#ifdef __cplusplus
  extern "C" {
#endif

/***************************************************************************** 
 * Include
 *****************************************************************************/ 
#include <stdlib.h> 
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdarg.h>
#include <utils/Log.h> // For Debug
#include "mpe_API.h"
#include "mpe_sys_common.h"
/***************************************************************************** 
 * Define
 *****************************************************************************/
#ifdef DEBUG_LOG

#ifdef LOG_TAG
#undef LOG_TAG
#endif

#define LOG_TAG "MPE_FA"
#define LOG_EN

#ifdef LOG_EN     
#define FLPE(...)		MPE_sys_dbg(MPE_LOG_ERROR, LOG_TAG, __VA_ARGS__)
#define FLPD(...)		MPE_sys_dbg(MPE_LOG_DEBUG, LOG_TAG, __VA_ARGS__)
#define FLPW(...)		MPE_sys_dbg(MPE_LOG_WARN, LOG_TAG, __VA_ARGS__)
#else
#define FLPD(...)
#define FLPW(...)
#define FLPE(...)
#endif

#endif

/***************************************************************************** 
 * Functions
 *****************************************************************************/
extern void MPE_mnl_callback_reg_name (MPEMNLCallBack *cb_name);
extern void MPE_flp_callback_reg_name (MPEFLPCallBack *cb_name);

/*****************************************************************************
 * FUNCTION
 *  MPE_API_init
 * DESCRIPTION
 *  API Initialize MPE
 * PARAMETERS
 * RETURNS
 *  TRUE/FALSE (initialize MPE)
 *****************************************************************************/
BL MPE_API_init (void) {
  BL ret = FALSE;
  MPE_task_init();
  ret = MPE_threads_create();
  usleep(1000);
  FLPD("MPE thread create=%d\n",ret);
  return ret;
}
/*****************************************************************************
 * FUNCTION
 *  MPE_API_deinit
 * DESCRIPTION
 *  Terminate MPE 
 * PARAMETERS
 * RETURNS
 *  TRUE/FALSE (terminate MPE)
 *****************************************************************************/
 BL MPE_API_deinit (void) {
  MPE_MSG *new_msg;
  mtk_uint16 payload_len;
  BL ret = FALSE;
  
  payload_len = sizeof(MPE_MSG);
  new_msg = MPE_Mem_Alloc(payload_len);

  if(new_msg == NULL) {
  	return ret;
  }
  new_msg->length = payload_len;
  new_msg->type = MPE_MODE_DEINIT;

  if(MPE_sys_msg_send(MPE_TASKID_MAIN, new_msg) != MPE_SUCCESS ) {
  	MPE_Msg_Ring_Buf_free(new_msg);
  }
  MPE_task_uninit();
  
  //Release main thread
  if(MPE_threads_release() != TRUE ) {
     FLPD("MPE thread release failed \n");
  }
  ret = TRUE;
  return ret;
}
/*****************************************************************************
 * FUNCTION
 *  MPE_API_run
 * DESCRIPTION
 *  Configure and run MPE
 * PARAMETERS
 *  1). init source: FLP, MNL
 *  1). int sampling_rate: desired sensor sampling rate (in ms)
 * RETURNS
 *  TRUE: Run message successfully sent
 *  FALSE: Run message failed to send
 *****************************************************************************/
BL MPE_API_run (MPE_LOC_SOURCE source, int sampling_rate) {
  MPE_MSG *new_msg;
  mtk_uint16 payload_len;
  BL ret = FALSE;
  
  payload_len = sizeof(int)+ sizeof(int);
  new_msg = MPE_Mem_Alloc( (mtk_uint16)(sizeof(MPE_MSG) + payload_len));

  if(new_msg == NULL) {
    return ret;
  }
  new_msg->length = payload_len;
  new_msg->type = MPE_MODE_RUN;

  if(payload_len >0) {
     memcpy( ((mtk_int8*)new_msg)+sizeof(MPE_MSG), &source, sizeof(int));
     memcpy( ((mtk_int8*)new_msg)+sizeof(MPE_MSG)+sizeof(int), &sampling_rate, sizeof(int));
  }

  if(MPE_sys_msg_send(MPE_TASKID_MAIN,new_msg) != MPE_SUCCESS ) {
     MPE_Msg_Ring_Buf_free(new_msg);
     return FALSE;
  }
  return TRUE;
}

/*****************************************************************************
 * FUNCTION
 *  MPE_API_pause
 * DESCRIPTION
 *  Pause the MPE
 * PARAMETERS
 *  1). init source: FLP, MNL
 *  2). int flag: pause mode
 *                  0: partial pause: stop the data reading but keep the timer
 *                  1: full stop : stop both the data reading and timer
 * RETURNS
 *  TRUE: Run message successfully sent
 *  FALSE: Run message failed to send
 *****************************************************************************/
BL MPE_API_pause (MPE_LOC_SOURCE source,int flag) {
  MPE_MSG *new_msg;
  mtk_uint16 payload_len;
  BL ret = FALSE;
  
  payload_len = sizeof(int)+ sizeof(int);
  new_msg = MPE_Mem_Alloc( (mtk_uint16)(sizeof(MPE_MSG) + payload_len));

  if(new_msg == NULL) {
    return ret;
  }

  new_msg->length = payload_len;
  new_msg->type = MPE_MODE_PAUSE;

  if(payload_len >0) {
     memcpy( ((mtk_int8*)new_msg)+sizeof(MPE_MSG), &source, sizeof(int));
     memcpy( ((mtk_int8*)new_msg)+sizeof(MPE_MSG)+sizeof(int), &flag, sizeof(int));      
  }

  if( MPE_sys_msg_send(MPE_TASKID_MAIN,new_msg) != MPE_SUCCESS ) {
     MPE_Msg_Ring_Buf_free(new_msg);
     return ret;
  }
  ret = TRUE;
  return ret;
}
/*****************************************************************************
 * FUNCTION
 *  MPE_API_continue
 * DESCRIPTION
 *  Resume MPE
 * PARAMETERS
 * RETURNS
 *  TRUE: Run message successfully sent
 *  FALSE: Run message failed to send
 *****************************************************************************/
BL MPE_API_continue (void) {
  MPE_MSG *new_msg;
  mtk_uint16 payload_len;
  BL ret = FALSE;
  
  payload_len = 0;
  new_msg = MPE_Mem_Alloc( (mtk_uint16)(sizeof(MPE_MSG) + payload_len));

  if(new_msg == NULL) {
     return ret;
  }
  
  new_msg->length = payload_len;
  new_msg->type = MPE_MODE_CONTINUE;

  if( MPE_sys_msg_send(MPE_TASKID_MAIN,new_msg) != MPE_SUCCESS ) {
     MPE_Msg_Ring_Buf_free(new_msg);
     return ret;
  }
  ret = TRUE;
  return ret;
}
/*****************************************************************************
 * FUNCTION
 *  MPE_API_MNL_loc_input_data
 * DESCRIPTION
 *  Get location data from MNL
 * PARAMETERS
 *  1). location info
 * RETURNS
 *  data: successed or failed
 *****************************************************************************/
BL MPE_API_MNL_loc_input_data (MNL_location_input_t data ) {
  MPE_MSG *new_msg;
  mtk_uint16 payload_len;
  BL ret = FALSE;

  payload_len = sizeof(MNL_location_input_t);
  new_msg = MPE_Mem_Alloc( (mtk_uint16)(sizeof(MPE_MSG) + payload_len));

  if(new_msg == NULL) {
     return ret;
  }
  
  new_msg->length = payload_len;
  new_msg->type = MPE_MODE_MNL_INPUT_LOC;
  memcpy( ((mtk_int8*)new_msg)+sizeof(MPE_MSG), &data, sizeof(MNL_location_input_t));

  if( MPE_sys_msg_send(MPE_TASKID_MAIN,new_msg) != MPE_SUCCESS ) {
     MPE_Msg_Ring_Buf_free(new_msg);
     return ret;
  }
  ret = TRUE;
  return ret;
}
/*****************************************************************************
 * FUNCTION
 *  MPE_API_FLP_loc_input_data
 * DESCRIPTION
 *  Get location data from FLP
 * PARAMETERS
 *  1). location info
 * RETURNS
 *  data: successed or failed
 *****************************************************************************/
BL MPE_API_FLP_loc_input_data (FLP_location_input_t data ) {
  MPE_MSG *new_msg;
  mtk_uint16 payload_len;
  BL ret = FALSE;

  payload_len = sizeof(FLP_location_input_t);
  new_msg = MPE_Mem_Alloc( (mtk_uint16)(sizeof(MPE_MSG) + payload_len));

  if(new_msg == NULL) {
     return ret;
  }
  
  new_msg->length = payload_len;
  new_msg->type = MPE_MODE_FLP_INPUT_LOC;
  memcpy( ((mtk_int8*)new_msg)+sizeof(MPE_MSG), &data, sizeof(FLP_location_input_t));

  if( MPE_sys_msg_send(MPE_TASKID_MAIN,new_msg) != MPE_SUCCESS ) {
     MPE_Msg_Ring_Buf_free(new_msg);
     return ret;
  }
  ret = TRUE;
  return ret;
}
/*****************************************************************************
 * FUNCTION
 *  MPE_MNL_callback_reg
 * DESCRIPTION
 * Registration of MPE callback functions in order to output DR(ADR/PDR) info to MNL
 * PARAMETERS
 * RETURNS
 *  MPE_status: setting success/failed
 *****************************************************************************/
BL MPE_MNL_callback_reg (MPEMNLCallBack *cb_name) {
   MPE_mnl_callback_reg_name(cb_name);
   return TRUE;
}
/*****************************************************************************
 * FUNCTION
 *  MPE_FLP_callback_reg
 * DESCRIPTION
 * Registration of MPE callback functions in order to output DR(ADR/PDR) info to FLP
 * PARAMETERS
 * RETURNS
 *  MPE_status: setting success/failed
 *****************************************************************************/
BL MPE_FLP_callback_reg (MPEFLPCallBack *cb_name) {
   MPE_flp_callback_reg_name(cb_name);
   return TRUE;
}

#ifdef __cplusplus
  extern "C" }
#endif

#endif //#ifndef MPE_FUSION_API_C
