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
 *  SRS Definition public APIs header file
 *
 *	Author: Oscar Huang
 *	
 *	(RCS keywords below, do not edit)
 *  $Id: //srstech/srs_common/std_fxp/include/srs_definition_api.h#4 $
 *  $Author: oscarh $
 *  $Date: 2010/09/21 $
********************************************************************************/

#ifndef __SRS_DEFINITION_API_H__
#define __SRS_DEFINITION_API_H__

#include "srs_typedefs.h"
#include "srs_fxp.h"
#include "srs_def_def.h"


#ifdef __cplusplus
extern "C"{
#endif /*__cplusplus*/

int				SRS_Def_GetObjSize();
SRSResult		SRS_Def_CreateObj(SRSDefObj *pObj, void *pBuf, SRS_Allocator alloc,void* pContext);

SRSResult		SRS_Def_InitObj8k(SRSDefObj defObj);	
SRSResult		SRS_Def_InitObj11k(SRSDefObj defObj);
SRSResult		SRS_Def_InitObj16k(SRSDefObj defObj);
SRSResult		SRS_Def_InitObj22k(SRSDefObj defObj);
SRSResult		SRS_Def_InitObj24k(SRSDefObj defObj);
SRSResult		SRS_Def_InitObj32k(SRSDefObj defObj);
SRSResult		SRS_Def_InitObj44k(SRSDefObj defObj);
SRSResult		SRS_Def_InitObj48k(SRSDefObj defObj);
SRSResult		SRS_Def_InitObj88k(SRSDefObj defObj);
SRSResult		SRS_Def_InitObj96k(SRSDefObj defObj);

void			SRS_Def_SetControlDefaults(SRSDefObj defObj);

void		SRS_Def_FilterProcess(SRSDefObj defObj, SRSInt32 *audioIO, int blockSamples);
void		SRS_Def_Process(SRSDefObj defObj, SRSInt32 *audioIO, int blockSamples, void *ws);

//"Set" functions:
void		SRS_Def_SetEnable(SRSDefObj defObj, SRSBool enable);
SRSResult	SRS_Def_SetInputGain(SRSDefObj defObj, SRSInt16 gain);
SRSResult	SRS_Def_SetOutputGain(SRSDefObj defObj, SRSInt16 gain);
SRSResult	SRS_Def_SetBypassGain(SRSDefObj defObj, SRSInt16 gain);
SRSResult	SRS_Def_SetLevel(SRSDefObj defObj, SRSInt16 level);

//"Get" functions:
SRSBool		SRS_Def_GetEnable(SRSDefObj defObj);
SRSInt16	SRS_Def_GetInputGain(SRSDefObj defObj);
SRSInt16	SRS_Def_GetOutputGain(SRSDefObj defObj);
SRSInt16	SRS_Def_GetBypassGain(SRSDefObj defObj);
SRSInt16	SRS_Def_GetLevel(SRSDefObj defObj);

#ifdef __cplusplus
}
#endif /*__cplusplus*/


////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////



#endif //__SRS_DEFINITION_API_H__
