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
 *  SRS CSHP types, contants
 *
 *  RCS keywords:
 *	$Id: //srstech/srs_cshp/std_fxp/include/srs_cshp_def.h#2 $
 *  $Author: oscarh $
 *  $Date: 2011/02/15 $
 *	
********************************************************************************/

#ifndef __SRS_CSHP_DEF_H__
#define __SRS_CSHP_DEF_H__

#include "srs_typedefs.h"
#include "srs_csdecoder_def.h"
#include "srs_def_def.h"
#include "srs_focus_def.h"
#include "srs_limiter_def.h"
#include "srs_hp360_def.h"
#include "srs_tbhd_def.h"


typedef struct _SRSCshpObj *SRSCshpObj;



#define SRS_CSHP_WORKSPACE_SIZE(blockSize)		(SRS_CSHP_MAX(\
													SRS_CSHP_MAX(SRS_CSD_WORKSPACE_SIZE(blockSize), SRS_TBHD_WORKSPACE_SIZE(blockSize)), \
													SRS_HP360_WORKSPACE_SIZE(blockSize) \
												 ) + 6*sizeof(SRSInt32)*(blockSize) + 8)

#define SRS_CSHP_GAIN_IWL	1		//iwl of InputGain, OutputGain and BypassGain

typedef enum
{
	SRS_CSHP_PASSIVE_DECODER,
	SRS_CSHP_CS_DECODER, 
	SRS_CSHP_RENDER_UNUSED = 0x7FFFFFFF //force to generate 32-bit enum
} SRSCshpRenderingMode;

#define SRS_CSHP_MAX(a, b)	((a)>=(b)? (a):(b))

typedef struct
{
	SRSBool				Enable;
	SRSBool				SplitAnalysisEnable;  //Enable/disable the split analysis feature
	SRSInt16			CompressorLevel;	//0.0~1.0
	SRSInt16			Level;	//TruBass Level, 0.0~1.0
	SRSTBHDSpeakerSize	SpkSize; //{AudioFilter, Analysis Filter}
	SRSTBHDMode			Mode;		//Mono mode or stereo mode TB
} SRSCshpTruBassControls;

typedef struct
{
	SRSBool					Enable;
	SRSInt16				InputGain;
	SRSInt16				OutputGain;
	SRSInt16				BypassGain;
	SRSCshpRenderingMode	Render;
	SRSCSDProcMode			CSDProcMode; //CSDecoder processing mode

	SRSBool					DCEnable;    //DialogClariy Enable
	SRSBool					DefEnable;	 //Definition Enable
	SRSInt16				DCLevel;	 //DialogClarity Level
	SRSInt16				DefLevel;	 //Definition Level

	SRSCshpTruBassControls	TBHDControls;	//TruBass HD controls

	SRSBool					LmtEnable;		//Limiter Enable
	SRSInt32				LmtMinimalGain; //Limiter minimal gain (headroom)
} SRSCshpControls;

#endif //__SRS_CSHP_DEF_H__
