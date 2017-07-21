/*======================================================================*
 DTS, Inc.
 5220 Las Virgenes Road
 Calabasas, CA 91302  USA

 CONFIDENTIAL: CONTAINS CONFIDENTIAL PROPRIETARY INFORMATION OWNED BY
 DTS, INC. AND/OR ITS AFFILIATES ("DTS"), INCLUDING BUT NOT LIMITED TO
 TRADE SECRETS, KNOW-HOW, TECHNICAL AND BUSINESS INFORMATION. USE,
 DISCLOSURE OR DISTRIBUTION OF THE SOFTWARE IN ANY FORM IS LIMITED TO
 SPECIFICALLY AUTHORIZED LICENSEES OF DTS.  ANY UNAUTHORIZED
 DISCLOSURE IS A VIOLATION OF STATE, FEDERAL, AND INTERNATIONAL LAWS.
 BOTH CIVIL AND CRIMINAL PENALTIES APPLY.

 DO NOT DUPLICATE. COPYRIGHT 2014, DTS, INC. ALL RIGHTS RESERVED.
 UNAUTHORIZED DUPLICATION IS A VIOLATION OF STATE, FEDERAL AND
 INTERNATIONAL LAWS.

 ALGORITHMS, DATA STRUCTURES AND METHODS CONTAINED IN THIS SOFTWARE
 MAY BE PROTECTED BY ONE OR MORE PATENTS OR PATENT APPLICATIONS.
 UNLESS OTHERWISE PROVIDED UNDER THE TERMS OF A FULLY-EXECUTED WRITTEN
 AGREEMENT BY AND BETWEEN THE RECIPIENT HEREOF AND DTS, THE FOLLOWING
 TERMS SHALL APPLY TO ANY USE OF THE SOFTWARE (THE "PRODUCT") AND, AS
 APPLICABLE, ANY RELATED DOCUMENTATION:  (i) ANY USE OF THE PRODUCT
 AND ANY RELATED DOCUMENTATION IS AT THE RECIPIENT'S SOLE RISK:
 (ii) THE PRODUCT AND ANY RELATED DOCUMENTATION ARE PROVIDED "AS IS"
 AND WITHOUT WARRANTY OF ANY KIND AND DTS EXPRESSLY DISCLAIMS ALL
 WARRANTIES, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO ANY
 IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 PURPOSE, REGARDLESS OF WHETHER DTS KNOWS OR HAS REASON TO KNOW OF THE
 USER'S PARTICULAR NEEDS; (iii) DTS DOES NOT WARRANT THAT THE PRODUCT
 OR ANY RELATED DOCUMENTATION WILL MEET USER'S REQUIREMENTS, OR THAT
 DEFECTS IN THE PRODUCT OR ANY RELATED DOCUMENTATION WILL BE
 CORRECTED; (iv) DTS DOES NOT WARRANT THAT THE OPERATION OF ANY
 HARDWARE OR SOFTWARE ASSOCIATED WITH THIS DOCUMENT WILL BE
 UNINTERRUPTED OR ERROR-FREE; AND (v) UNDER NO CIRCUMSTANCES,
 INCLUDING NEGLIGENCE, SHALL DTS OR THE DIRECTORS, OFFICERS, EMPLOYEES,
 OR AGENTS OF DTS, BE LIABLE TO USER FOR ANY INCIDENTAL, INDIRECT,
 SPECIAL, OR CONSEQUENTIAL DAMAGES (INCLUDING BUT NOT LIMITED TO
 DAMAGES FOR LOSS OF BUSINESS PROFITS, BUSINESS INTERRUPTION, AND LOSS
 OF BUSINESS INFORMATION) ARISING OUT OF THE USE, MISUSE, OR INABILITY
 TO USE THE PRODUCT OR ANY RELATED DOCUMENTATION.
*======================================================================*/

/********************************************************************************
 *	SRS Labs CONFIDENTIAL
 *	@Copyright 2010 by SRS Labs.
 *	All rights reserved.
 *
 *  Description:
 *  Exposes all srs_limiter APIs
 *
 *  RCS keywords:
 *	$Id: //srstech/srs_common/std_fxp/include/srs_limiter_api.h#4 $
 *  $Author: oscarh $
 *  $Date: 2010/09/21 $
 *	
********************************************************************************/

#ifndef __SRS_LIMITER_API_H__
#define __SRS_LIMITER_API_H__

#include "srs_limiter_def.h"


#ifdef __cplusplus
extern "C"{
#endif /*__cplusplus*/

int			SRS_Limiter_GetObjSize();

SRSResult	SRS_Limiter_CreateObj(SRSLimiterObj *pObj, void *pBuf, SRS_Allocator alloc,void* pContext);

SRSResult	SRS_Limiter_InitObj8k(SRSLimiterObj lmtObj);
SRSResult	SRS_Limiter_InitObj11k(SRSLimiterObj lmtObj);
SRSResult	SRS_Limiter_InitObj16k(SRSLimiterObj lmtObj);
SRSResult	SRS_Limiter_InitObj22k(SRSLimiterObj lmtObj);
SRSResult	SRS_Limiter_InitObj24k(SRSLimiterObj lmtObj);
SRSResult	SRS_Limiter_InitObj32k(SRSLimiterObj lmtObj);
SRSResult	SRS_Limiter_InitObj44k(SRSLimiterObj lmtObj);
SRSResult	SRS_Limiter_InitObj48k(SRSLimiterObj lmtObj);
SRSResult	SRS_Limiter_InitObj88k(SRSLimiterObj lmtObj);
SRSResult	SRS_Limiter_InitObj96k(SRSLimiterObj lmtObj);

SRSResult	SRS_Limiter_ProcessMono(SRSLimiterObj lmtObj, SRSInt32 *audioChan, int blockSamples);
SRSResult	SRS_Limiter_ProcessStereo(SRSLimiterObj lmtObj, SRSStereoCh *audioIO, int blockSamples);

SRSResult	SRS_Limiter_SetMinimalGain(SRSLimiterObj lmtObj, SRSInt32 minGain); //limit: I32.2
void		SRS_Limiter_SetEnable(SRSLimiterObj lmtObj, SRSBool enable);

SRSInt32	SRS_Limiter_GetMinimalGain(SRSLimiterObj lmtObj);
SRSBool		SRS_Limiter_GetEnable(SRSLimiterObj lmtObj);

void		SRS_Limiter_SetControlDefaults(SRSLimiterObj lmtObj);


#ifdef __cplusplus
}
#endif /*__cplusplus*/


//////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif /*__SRS_LIMITER_API_H__*/
