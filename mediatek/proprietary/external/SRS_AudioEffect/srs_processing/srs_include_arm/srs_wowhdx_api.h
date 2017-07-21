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
 *  Exposes all srs_wowhdplus APIs
 *
 *  RCS keywords:
 *	$Id$
 *  $Author$
 *  $Date$
 *	
********************************************************************************/

#ifndef __SRS_WOWHDPLUS_API_H__
#define __SRS_WOWHDPLUS_API_H__

#include "srs_wowhdx_def.h"


#ifdef __cplusplus
extern "C"{
#endif /*__cplusplus*/

//API declaration here:
int					SRS_Wowhdx_GetObjSize(const SRSWowhdxCreateData *pAttr);
SRSResult			SRS_Wowhdx_CreateObj(SRSWowhdxObj *pObj, void* pBuf, SRS_Allocator alloc, void *pContext, const SRSWowhdxCreateData *whdxAttr);

SRSResult			SRS_Wowhdx_InitObj32k(SRSWowhdxObj wpObj, const SRSWowhdxFilterConfig *pFilterConfig);
SRSResult			SRS_Wowhdx_InitObj44k(SRSWowhdxObj wpObj, const SRSWowhdxFilterConfig *pFilterConfig); //Using default filters if pFilterConfig is NULL
SRSResult			SRS_Wowhdx_InitObj48k(SRSWowhdxObj wpObj, const SRSWowhdxFilterConfig *pFilterConfig);

SRSResult			SRS_Wowhdx_Process(SRSWowhdxObj wpObj, SRSStereoCh *audioIO, void *ws);

void				SRS_Wowhdx_SetControlDefaults(SRSWowhdxObj wpObj);

//Control 'set' APIs:
SRSResult			SRS_Wowhdx_SetControls(SRSWowhdxObj wpObj, const SRSWowhdxControls *pCtrls);	//Set all controls of WOW HDX

void				SRS_Wowhdx_SetEnable(SRSWowhdxObj wpObj, SRSBool enable);
SRSResult			SRS_Wowhdx_SetInputGain(SRSWowhdxObj wpObj, SRSInt16 gain);
SRSResult			SRS_Wowhdx_SetOutputGain(SRSWowhdxObj wpObj, SRSInt16 gain);
SRSResult			SRS_Wowhdx_SetBypassGain(SRSWowhdxObj wpObj, SRSInt16 gain);
SRSResult			SRS_Wowhdx_SetEnhanceMode(SRSWowhdxObj wpObj, SRSWowhdxStereoEnhanceMode mode);
void				SRS_Wowhdx_SetCrossoverEnable(SRSWowhdxObj wpObj, SRSBool enable); //enables/disables the crossover when the enhance mode is SRS3D


//TruBass APIs:
SRSResult			SRS_Wowhdx_SetTBHDControls(SRSWowhdxObj wpObj, const SRSWowhdxTruBassControls *pCtrls); //Set all TBHD controls

SRSResult			SRS_Wowhdx_SetTBHDCustomSpeakerCoefs(SRSWowhdxObj wpObj, const SRSInt32 *coefs); //Set custom speaker filter coefficients

void				SRS_Wowhdx_SetTBHDEnable(SRSWowhdxObj wpObj, SRSBool enable);
void				SRS_Wowhdx_SetTBHDSplitAnalysisEnable(SRSWowhdxObj wpObj, SRSBool enable);
void				SRS_Wowhdx_SetTBHDLevelIndependentEnable(SRSWowhdxObj wpObj, SRSBool enable);
SRSResult			SRS_Wowhdx_SetTBHDLevel(SRSWowhdxObj wpObj, SRSInt16 level);		//TruBass control
SRSResult			SRS_Wowhdx_SetTBHDMode(SRSWowhdxObj wpObj, SRSTBHDMode mode); //mono mode or stereo mode
SRSResult			SRS_Wowhdx_SetTBHDSpeakerSize(SRSWowhdxObj wpObj, SRSTBHDSpeakerSize size); 
SRSResult			SRS_Wowhdx_SetTBHDCompressorLevel(SRSWowhdxObj wpObj, SRSInt16 level); //Compressor control: 0.0~1.0


//Post TruBass HPF:
void				SRS_Wowhdx_SetHpfEnable(SRSWowhdxObj wpObj, SRSBool enable); //Turn on/off the post TruBass HPF

//SRS3D APIs:
SRSResult			SRS_Wowhdx_SetSRS3DControls(SRSWowhdxObj wpObj, const SRSWowhdxSRS3DControls *pCtrls); //Set all SRS3D controls

SRSResult			SRS_Wowhdx_SetSRS3DMode(SRSWowhdxObj wpObj, SRSSrs3DMode mode);
SRSResult			SRS_Wowhdx_SetSRS3DSpaceLevel(SRSWowhdxObj wpObj, SRSInt16 level);
SRSResult			SRS_Wowhdx_SetSRS3DCenterLevel(SRSWowhdxObj wpObj, SRSInt16 level);
void				SRS_Wowhdx_SetSRS3DHeadphoneEnable(SRSWowhdxObj wpObj, SRSBool enable);
void				SRS_Wowhdx_SetSRS3DHighBitRateEnable(SRSWowhdxObj wpObj, SRSBool enable);

//Focus APIs:
void				SRS_Wowhdx_SetFocusEnable(SRSWowhdxObj wpObj, SRSBool enable);
SRSResult			SRS_Wowhdx_SetFocusLevel(SRSWowhdxObj wpObj, SRSInt16 level);

//Definition APIs:
void				SRS_Wowhdx_SetDefEnable(SRSWowhdxObj wpObj, SRSBool enable);
SRSResult			SRS_Wowhdx_SetDefLevel(SRSWowhdxObj wpObj, SRSInt16 level);

//WideSurround APIs:
SRSResult			SRS_Wowhdx_SetWdSrdControls(SRSWowhdxObj wpObj, const SRSWowhdxWideSrdControls *pCtrls); //Set all WideSurround controls

SRSResult			SRS_Wowhdx_SetWdSrdCenterBoostGain(SRSWowhdxObj wpObj, SRSInt16 gain);
SRSResult			SRS_Wowhdx_SetWdSrdSpeakerSeparationLevel(SRSWowhdxObj wpObj, SRSInt16 separation);
void				SRS_Wowhdx_SetWdSrdHandsetHPFEnable(SRSWowhdxObj wpObj, SRSBool enable);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Control Get functions:
//Overall APIs:
void						SRS_Wowhdx_GetControls(SRSWowhdxObj wpObj, SRSWowhdxControls *pCtrls);	//Get all controls of WOW HDX

SRSBool						SRS_Wowhdx_GetEnable(SRSWowhdxObj wpObj);
SRSInt16					SRS_Wowhdx_GetInputGain(SRSWowhdxObj wpObj);
SRSInt16					SRS_Wowhdx_GetOutputGain(SRSWowhdxObj wpObj);
SRSInt16					SRS_Wowhdx_GetBypassGain(SRSWowhdxObj wpObj);
SRSWowhdxStereoEnhanceMode	SRS_Wowhdx_GetEnhanceMode(SRSWowhdxObj wpObj);
SRSBool						SRS_Wowhdx_GetCrossoverEnable(SRSWowhdxObj wpObj);


//TruBass APIs:
void				SRS_Wowhdx_GetTBHDControls(SRSWowhdxObj wpObj, SRSWowhdxTruBassControls *pCtrls); //Get all TBHD controls

SRSTBHDCustomSpeakerCoefs	SRS_Wowhdx_GetTBHDCustomSpeakerCoefs(SRSWowhdxObj wpObj);

SRSBool				SRS_Wowhdx_GetTBHDEnable(SRSWowhdxObj wpObj);
SRSBool				SRS_Wowhdx_GetTBHDSplitAnalysisEnable(SRSWowhdxObj wpObj);
SRSBool				SRS_Wowhdx_GetTBHDLevelIndependentEnable(SRSWowhdxObj wpObj);	
SRSInt16			SRS_Wowhdx_GetTBHDLevel(SRSWowhdxObj wpObj);		
SRSTBHDMode			SRS_Wowhdx_GetTBHDMode(SRSWowhdxObj wpObj); 
SRSTBHDSpeakerSize	SRS_Wowhdx_GetTBHDSpeakerSize(SRSWowhdxObj wpObj); 
SRSInt16			SRS_Wowhdx_GetTBHDCompressorLevel(SRSWowhdxObj wpObj);

//Post TruBass HPF:
SRSBool				SRS_Wowhdx_GetHpfEnable(SRSWowhdxObj wpObj);

//SRS3D APIs:
void					SRS_Wowhdx_GetSRS3DControls(SRSWowhdxObj wpObj, SRSWowhdxSRS3DControls *pCtrls); //Get all SRS3D controls

SRSSrs3DMode			SRS_Wowhdx_GetSRS3DMode(SRSWowhdxObj wpObj);
SRSInt16				SRS_Wowhdx_GetSRS3DSpaceLevel(SRSWowhdxObj wpObj);
SRSInt16				SRS_Wowhdx_GetSRS3DCenterLevel(SRSWowhdxObj wpObj);
SRSBool					SRS_Wowhdx_GetSRS3DHeadphoneEnable(SRSWowhdxObj wpObj);
SRSBool					SRS_Wowhdx_GetSRS3DHighBitRateEnable(SRSWowhdxObj wpObj);

//Focus APIs:
SRSBool					SRS_Wowhdx_GetFocusEnable(SRSWowhdxObj wpObj);
SRSInt16				SRS_Wowhdx_GetFocusLevel(SRSWowhdxObj wpObj);

//Definition APIs:
SRSBool					SRS_Wowhdx_GetDefEnable(SRSWowhdxObj wpObj);
SRSInt16				SRS_Wowhdx_GetDefLevel(SRSWowhdxObj wpObj);

//WideSurround APIs:
void					SRS_Wowhdx_GetWdSrdControls(SRSWowhdxObj wpObj, SRSWowhdxWideSrdControls *pCtrls); //Get all WideSurround controls

SRSInt16				SRS_Wowhdx_GetWdSrdCenterBoostGain(SRSWowhdxObj wpObj);
SRSInt16				SRS_Wowhdx_GetWdSrdSpeakerSeparation(SRSWowhdxObj wpObj);
SRSBool					SRS_Wowhdx_GetWdSrdHandsetHPFEnable(SRSWowhdxObj wpObj);

//version info query functions
unsigned char			SRS_Wowhdx_GetTechVersion(SRSVersion which);
unsigned char			SRS_Wowhdx_GetLibVersion(SRSVersion which);


const SRSLibInfo*		SRS_Wowhdx_GetLibsInfo();

#ifdef __cplusplus
}
#endif /*__cplusplus*/

#endif /*__SRS_WOWHDPLUS_API_H__*/
