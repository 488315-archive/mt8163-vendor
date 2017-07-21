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
 *  Exposes all srs_trubass APIs
 *
 *  RCS keywords:
 *	$Id: //srstech/srs_sa_trubass/std_fxp/include/srs_tbhd_api.h#3 $
 *  $Author: oscarh $
 *  $Date: 2010/11/16 $
 *	
********************************************************************************/

#ifndef __SRS_SA_TRUBASS_API_H__
#define __SRS_SA_TRUBASS_API_H__

#include "srs_tbhd_def.h"


#ifdef __cplusplus
extern "C"{
#endif /*__cplusplus*/

int					SRS_TBHD_GetObjSize();
SRSResult			SRS_TBHD_CreateObj(SRSTBHDObj *pObj, void* pBuf, SRS_Allocator alloc,void* pContext);

SRSResult			SRS_TBHD_InitObj8k(SRSTBHDObj tbObj);
SRSResult			SRS_TBHD_InitObj11k(SRSTBHDObj tbObj);
SRSResult			SRS_TBHD_InitObj16k(SRSTBHDObj tbObj);
SRSResult			SRS_TBHD_InitObj22k(SRSTBHDObj tbObj);
SRSResult			SRS_TBHD_InitObj24k(SRSTBHDObj tbObj);
SRSResult			SRS_TBHD_InitObj32k(SRSTBHDObj tbObj);
SRSResult			SRS_TBHD_InitObj44k(SRSTBHDObj tbObj);
SRSResult			SRS_TBHD_InitObj48k(SRSTBHDObj tbObj);
SRSResult			SRS_TBHD_InitObj88k(SRSTBHDObj tbObj);
SRSResult			SRS_TBHD_InitObj96k(SRSTBHDObj tbObj);

void				SRS_TBHD_Process(SRSTBHDObj tbObj, SRSStereoCh *audioIO, int blockSize, void* ws);

SRSResult			SRS_TBHD_SetControls(SRSTBHDObj tbObj, const SRSTBHDControls *pCtrls);	//Set all controls
void				SRS_TBHD_GetControls(SRSTBHDObj tbObj, SRSTBHDControls *pCtrls);		//Get all controls

///////////////////////////////Set Functions///////////////////////////////////////////////////////////
void				SRS_TBHD_SetControlDefaults(SRSTBHDObj tbObj);

void				SRS_TBHD_SetEnable(SRSTBHDObj tbObj, SRSBool enable);
void				SRS_TBHD_SetLevelIndependentEnable(SRSTBHDObj tbObj, SRSBool enable);
void				SRS_TBHD_SetSplitAnalysisEnable(SRSTBHDObj tbObj, SRSBool enable);//If False, AudioFilter is used for analysis, and the AnalysisFilter is not referenced.
void				SRS_TBHD_SetOutputOption(SRSTBHDObj tbObj, SRSTBHDOutputOption opt);

SRSResult			SRS_TBHD_SetInputGain(SRSTBHDObj tbObj, SRSInt16 gain);	//I16.SRS_TBHD_GAIN_IWL
SRSResult			SRS_TBHD_SetOutputGain(SRSTBHDObj tbObj, SRSInt16 gain);	//I16.SRS_TBHD_GAIN_IWL
SRSResult			SRS_TBHD_SetBypassGain(SRSTBHDObj tbObj, SRSInt16 gain);	//I16.SRS_TBHD_GAIN_IWL
SRSResult			SRS_TBHD_SetLevel(SRSTBHDObj tbObj, SRSInt16 level);		//I16.SRS_TBHD_CTRL_IWL
SRSResult			SRS_TBHD_SetCompressorLevel(SRSTBHDObj tbObj, SRSInt16 level);	//I16.SRS_TBHD_CTRL_IWL
SRSResult			SRS_TBHD_SetReferenceLevel(SRSTBHDObj tbObj, SRSInt16 level);	//I16.SRS_TBHD_REFLEVEL_IWL

SRSResult			SRS_TBHD_SetSpeakerSize(SRSTBHDObj tbObj, SRSTBHDSpeakerSize sz);
SRSResult			SRS_TBHD_SetMode(SRSTBHDObj tbObj, SRSTBHDMode mode);

/* Set the custom speaker filter coefficients */
/* "coefs" must point to an array of SRSInt32 with length 16. Coeffcients must be arranged in the pattern as type SRSTBHDCustomSpeakerCoefs */
/* The memory pointed to by "custom" is copied by the Set function, then no longer used */
void				SRS_TBHD_SetCustomSpeakerFilterCoefs(SRSTBHDObj tbObj, const SRSInt32 *coefs);

///////////////////////////////Get Function///////////////////////////////////////////////////////////

SRSBool				SRS_TBHD_GetEnable(SRSTBHDObj tbObj);
SRSBool				SRS_TBHD_GetLevelIndependentEnable(SRSTBHDObj tbObj);
SRSBool				SRS_TBHD_GetSplitAnalysisEnable(SRSTBHDObj tbObj);
SRSInt16			SRS_TBHD_GetInputGain(SRSTBHDObj tbObj);
SRSInt16			SRS_TBHD_GetOutputGain(SRSTBHDObj tbObj);
SRSInt16			SRS_TBHD_GetBypassGain(SRSTBHDObj tbObj);
SRSInt16			SRS_TBHD_GetLevel(SRSTBHDObj tbObj);
SRSInt16			SRS_TBHD_GetCompressorLevel(SRSTBHDObj tbObj);
SRSInt16			SRS_TBHD_GetReferenceLevel(SRSTBHDObj tbObj);


SRSTBHDMode					SRS_TBHD_GetMode(SRSTBHDObj tbObj);
SRSTBHDSpeakerSize			SRS_TBHD_GetSpeakerSize(SRSTBHDObj tbObj);
SRSTBHDOutputOption			SRS_TBHD_GetOutputOption(SRSTBHDObj tbObj);
SRSTBHDCustomSpeakerCoefs	SRS_TBHD_GetCustomSpeakerFilteCoefs(SRSTBHDObj tbObj);

unsigned char	SRS_TBHD_GetTechVersion(SRSVersion which);
unsigned char	SRS_TBHD_GetLibVersion(SRSVersion which);

const SRSLibInfo*  SRS_TBHD_GetLibsInfo();

#ifdef __cplusplus
}
#endif /*__cplusplus*/


#endif /*__SRS_SA_TRUBASS_API_H__*/
