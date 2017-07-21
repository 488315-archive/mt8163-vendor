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
 *  Exposes all SRS ActiveEQ APIs
 *
 *  RCS keywords:
 *	$Id$
 *  $Author$
 *  $Date$
 *	
********************************************************************************/

#ifndef __SRS_AEQ_API_H__
#define __SRS_AEQ_API_H__

#include "srs_aeq_def.h"


//SRS ActiveEQ processing flow:
//
//  Input(mono) -->  IIR Filter  -->  FIR Filter --> Output
//
//The coefficients of the IIR filter must be generated using SRS' ActiveEQ design tool.
//


#ifdef __cplusplus
extern "C"{
#endif /*__cplusplus*/

//Return the AEQ object buffer size according to the maximum filter orders (specified by pAttr) 
//that the object needs to support:
int			SRS_AEQ_GetObjSize(SRSAeqCreateData *pAttr);	

//Create an AEQ object
//Must specify the maximum FIR order and IIR order the object should support in pAttr
SRSResult	SRS_AEQ_CreateObj(SRSAeqObj *pObj, void *pBuf, SRS_Allocator alloc, void *pContext, SRSAeqCreateData *pAttr);

//Restore all controls to their default values:
void		SRS_AEQ_SetControlDefaults(SRSAeqObj aeqObj);

//Init AEQ object
//Must specify filter orders and coeffcient at initialization time.
//Set the FIR order to 0 if there is no FIR portion in the AEQ.
//AEQ supports reinitialization with a different filter configurations, but the orders must not greater than those specified
//when the AEQ object was created.
SRSResult	SRS_AEQ_InitObj(SRSAeqObj aeqObj, const SRSAeqFiltersConfig *filterCfg); 

//The AEQ processing function. NOT support in place operation, i.e, inChan and outChan cannot be the same buffer or overlap.
SRSResult	SRS_AEQ_Process(SRSAeqObj aeqObj, SRSInt32 *inChan, SRSInt32 *outChan, int blockSamples, void *workSpace);

//"Set" functions:
void		SRS_AEQ_SetEnable(SRSAeqObj aeqObj, SRSBool enable);
SRSResult	SRS_AEQ_SetInputGain(SRSAeqObj aeqObj, SRSInt16 gain); //0.0~1.0, I16.SRS_AEQ_GAIN_IWL (I16.1)
SRSResult	SRS_AEQ_SetOutputGain(SRSAeqObj aeqObj, SRSInt16 gain);//0.0~1.0, I16.SRS_AEQ_GAIN_IWL (I16.1)
SRSResult	SRS_AEQ_SetBypassGain(SRSAeqObj aeqObj, SRSInt16 gain);//0.0~1.0,I16.SRS_AEQ_GAIN_IWL (I16.1)
void        SRS_AEQ_SetIirEnable(SRSAeqObj aeqObj, SRSBool enable);
void        SRS_AEQ_SetFirEnable(SRSAeqObj aeqObj, SRSBool enable);
SRSResult   SRS_AEQ_SetControls(SRSAeqObj aeqObj, const SRSAeqControls *pCtrls);

//"Get" functions:
SRSBool		SRS_AEQ_GetEnable(SRSAeqObj aeqObj);
SRSInt16	SRS_AEQ_GetInputGain(SRSAeqObj aeqObj);
SRSInt16	SRS_AEQ_GetOutputGain(SRSAeqObj aeqObj);
SRSInt16	SRS_AEQ_GetBypassGain(SRSAeqObj aeqObj);
SRSBool     SRS_AEQ_GetIirEnable(SRSAeqObj aeqObj);
SRSBool     SRS_AEQ_GetFirEnable(SRSAeqObj aeqObj);
void        SRS_AEQ_GetControls(SRSAeqObj aeqObj, SRSAeqControls *pCtrls);

SRSAeqFilterOrders SRS_AEQ_GetMaxFilterOrders(SRSAeqObj aeqObj);

// version info query function
unsigned char	SRS_AEQ_GetTechVersion(SRSVersion which);
unsigned char	SRS_AEQ_GetLibVersion(SRSVersion which);

#ifdef __cplusplus
}
#endif /*__cplusplus*/

#endif /*__SRS_AEQ_API_H__*/
