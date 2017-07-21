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
 *  Exposes all srs_cshp APIs
 *
 *  RCS keywords:
 *	$Id: //srstech/srs_cshp/std_fxp/include/srs_cshp_api.h#7 $
 *  $Author: oscarh $
 *  $Date: 2010/11/16 $
 *	
********************************************************************************/

#ifndef __SRS_CSHP_API_H__
#define __SRS_CSHP_API_H__

#include "srs_cshp_def.h"


#ifdef __cplusplus
extern "C"{
#endif /*__cplusplus*/

int			SRS_CSHP_GetObjSize();
SRSResult	SRS_CSHP_CreateObj(SRSCshpObj *pObj, void *pBuf, SRS_Allocator alloc, void *pContext);

void		SRS_CSHP_SetControlDefaults(SRSCshpObj cshpObj);

SRSResult	SRS_CSHP_InitObj16k(SRSCshpObj cshpObj);
SRSResult	SRS_CSHP_InitObj22k(SRSCshpObj cshpObj);
SRSResult	SRS_CSHP_InitObj24k(SRSCshpObj cshpObj);
SRSResult	SRS_CSHP_InitObj32k(SRSCshpObj cshpObj);
SRSResult	SRS_CSHP_InitObj44k(SRSCshpObj cshpObj);
SRSResult	SRS_CSHP_InitObj48k(SRSCshpObj cshpObj);



SRSResult	SRS_CSHP_Process(SRSCshpObj cshpObj, SRSStereoCh *audioIO, int blockFrames, void *ws); //stereo audio processing
SRSResult	SRS_CSHP_Process_5P1(SRSCshpObj cshpObj, SRS5Point1Ch *audioIn, SRSStereoCh *audioOut, int blockFrames, void *ws); //5.1 input, stereo output processing

//Set/Get functions with structure parameters:
SRSResult	SRS_CSHP_SetControls(SRSCshpObj cshpObj, const SRSCshpControls *pCtrls); //Set all CSHP controls
SRSResult	SRS_CSHP_SetTruBassControls(SRSCshpObj cshpObj, const SRSCshpTruBassControls *pCtrls); //Set TBHD controls

void		SRS_CSHP_GetControls(SRSCshpObj cshpObj, SRSCshpControls *pCtrls);
void		SRS_CSHP_GetTruBassControls(SRSCshpObj cshpObj, SRSCshpTruBassControls *pCtrls);

SRSResult	SRS_CSHP_SetTruBassCustomSpeakerFilterCoefs(SRSCshpObj cshpObj, const SRSInt32 *coefs); //Set custom speaker coefficients

//"Set" functions:
void				SRS_CSHP_SetEnable(SRSCshpObj cshpObj, SRSBool enable);
SRSResult			SRS_CSHP_SetInputGain(SRSCshpObj cshpObj, SRSInt16 gain);	//I16.1
SRSResult			SRS_CSHP_SetOutputGain(SRSCshpObj cshpObj, SRSInt16 gain);	//I16.1
SRSResult			SRS_CSHP_SetBypassGain(SRSCshpObj cshpObj, SRSInt16 gain);	//I16.1
SRSResult			SRS_CSHP_SetRenderingMode(SRSCshpObj cshpObj, SRSCshpRenderingMode mode);

//CSDecoder:
SRSResult			SRS_CSHP_SetCSDecoderProcMode(SRSCshpObj cshpObj, SRSCSDProcMode mode);

//Focus (DialogClarity):
void				SRS_CSHP_SetDialogClarityEnable(SRSCshpObj cshpObj, SRSBool enable);
SRSResult			SRS_CSHP_SetDialogClarityLevel(SRSCshpObj cshpObj, SRSInt16 level); //I16.SRS_FOCUS_IWL (I16.1)

//Definition:
void				SRS_CSHP_SetDefinitionEnable(SRSCshpObj cshpObj, SRSBool enable);
SRSResult			SRS_CSHP_SetDefinitionLevel(SRSCshpObj cshpObj, SRSInt16 level); //I16.SRS_DEFINITION_IWL (I16.1)

//TruBass:
void				SRS_CSHP_SetTruBassEnable(SRSCshpObj cshpObj, SRSBool enable);
SRSResult			SRS_CSHP_SetTruBassLevel(SRSCshpObj cshpObj, SRSInt16 level);	//I16.SRS_TB_CTRL_IWL (I16.1)
SRSResult			SRS_CSHP_SetTruBassCompressorLevel(SRSCshpObj cshpObj, SRSInt16 level); //compressor control, 0~1.0, default 0.25, I16.SRS_SATB_CTRL_IWL
SRSResult			SRS_CSHP_SetTruBassSpeakerSize(SRSCshpObj cshpObj, SRSTBHDSpeakerSize sz);
SRSResult			SRS_CSHP_SetTruBassMode(SRSCshpObj cshpObj, SRSTBHDMode mode);
void				SRS_CSHP_SetTruBassSplitAnalysisEnable(SRSCshpObj cshpObj, SRSBool enable);				//disabled by default

//Limiter:
void				SRS_CSHP_SetLimiterEnable(SRSCshpObj cshpObj, SRSBool enable);
SRSResult			SRS_CSHP_SetLimiterMinimalGain(SRSCshpObj cshpObj, SRSInt32 value); //I32.2

//"Get" functions:
SRSBool				SRS_CSHP_GetEnable(SRSCshpObj cshpObj);
SRSInt16			SRS_CSHP_GetInputGain(SRSCshpObj cshpObj);	//max gain: <1
SRSInt16			SRS_CSHP_GetOutputGain(SRSCshpObj cshpObj);	//max gain: <1
SRSInt16			SRS_CSHP_GetBypassGain(SRSCshpObj cshpObj);	//max gain: <1
SRSCshpRenderingMode SRS_CSHP_GetRenderingMode(SRSCshpObj cshpObj);

//CSDecoder:
SRSCSDProcMode		SRS_CSHP_GetCSDecoderProcMode(SRSCshpObj cshpObj);

//Focus (DialogClarity):
SRSBool				SRS_CSHP_GetDialogClarityEnable(SRSCshpObj cshpObj);
SRSInt16			SRS_CSHP_GetDialogClarityLevel(SRSCshpObj cshpObj);

//Definition:
SRSBool				SRS_CSHP_GetDefinitionEnable(SRSCshpObj cshpObj);
SRSInt16			SRS_CSHP_GetDefinitionLevel(SRSCshpObj cshpObj);

//TruBass:
SRSBool				SRS_CSHP_GetTruBassEnable(SRSCshpObj cshpObj);
SRSInt16			SRS_CSHP_GetTruBassLevel(SRSCshpObj cshpObj);
SRSInt16			SRS_CSHP_GetTruBassCompressorLevel(SRSCshpObj cshpObj);
SRSTBHDSpeakerSize	SRS_CSHP_GetTruBassSpeakerSize(SRSCshpObj cshpObj);
SRSTBHDMode			SRS_CSHP_GetTruBassMode(SRSCshpObj cshpObj);
SRSTBHDCustomSpeakerCoefs	SRS_CSHP_GetTruBassCustomSpeakerFilterCoefs(SRSCshpObj cshpObj);
int					SRS_CSHP_GetTruBassSplitAnalysisEnable(SRSCshpObj cshpObj);

//Limiter:
SRSBool				SRS_CSHP_GetLimiterEnable(SRSCshpObj cshpObj);
SRSInt32			SRS_CSHP_GetLimiterMinimalGain(SRSCshpObj cshpObj);

//Version info get functions:
unsigned char SRS_CSHP_GetTechVersion(SRSVersion which);
unsigned char SRS_CSHP_GetLibVersion(SRSVersion which);

const SRSLibInfo*	SRS_CSHP_GetLibsInfo();

#ifdef __cplusplus
}
#endif /*__cplusplus*/


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


#endif /*__SRS_CSHP_API_H__*/
