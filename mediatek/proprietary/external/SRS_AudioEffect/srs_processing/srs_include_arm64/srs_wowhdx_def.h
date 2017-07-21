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
 *  srs_wowhdplus typedefs, constants
 *
 *  RCS keywords:
 *	$Id$
 *  $Author$
 *  $Date$
 *	
********************************************************************************/

#ifndef __SRS_WOWHDPLUS_DEF_H__
#define __SRS_WOWHDPLUS_DEF_H__

#include "srs_typedefs.h"
#include "srs_iir_def.h"
#include "srs_focus_def.h"
#include "srs_def_def.h"
#include "srs_tbhd_def.h"
#include "srs_srs3d_def.h"
#include "srs_widesrd_def.h"



#define	 SRS_WOWHDX_GAIN_IWL				1

typedef  struct _SRSWowhdxObj{int _;} *SRSWowhdxObj;

#define SRS_WOWHDX_WORKSPACE_SIZE		(_SRS_WHDP_MAX(\
												_SRS_WHDP_MAX(sizeof(SRSInt32)*SRS_WOWHDX_BLK_SZ*2+8, SRS_TBHD_WORKSPACE_SIZE(SRS_WOWHDX_BLK_SZ)), \
												_SRS_WHDP_MAX(SRS_SRS3D_WORKSPACE_SIZE(SRS_WOWHDX_BLK_SZ), SRS_WIDESRD_WORKSPACE_SIZE) \
											 ) + sizeof(SRSInt32)*SRS_WOWHDX_BLK_SZ*2)

#define	SRS_WOWHDX_BLK_SZ				SRS_WIDESRD_BLK_SZ

#define SRS_WOWHDX_XOVER_ORDER	4		//The order of the cross over filter


typedef enum
{
	SRS_WOWHDX_STEREO_ENHANCE_NONE,		//No stereo enhancement
	SRS_WOWHDX_STEREO_ENHANCE_SRS3D,	//SRS3D mode
	SRS_WOWHDX_STEREO_ENHANCE_WDSRD,	//WideSurround mode
	SRS_WOWHDX_STEREO_ENHANCE_UNUSED = 0x7FFFFFFF  //force to generate 32-bit enum
} SRSWowhdxStereoEnhanceMode;

typedef struct
{
	//The order of the crossover is fixed to 4
	const SRSInt16		*XoverLpfCoefs;	//The coefficients of xover LPF
	const SRSInt16		*XoverHpfCoefs;	//The coefficients of xover HPF
	
	int					HpfOrder;		//The HPF order
	const SRSInt16		*HpfCoefs;		//The HPF coefficients

} SRSWowhdxFilterConfig;

typedef struct
{
	int						MaxHpfOrder;		//The maximum HPF order that the WOW HDX object should support
	
} SRSWowhdxCreateData;

typedef struct
{
	SRSBool				Enable;
	SRSBool				SplitAnalysisEnable;  //Enable/disable the split analysis feature
	SRSBool				LevelIndependentEnable; //Enable/disalbe the level independent feature
	SRSInt16			Level;	//TruBass Level, 0.0~1.0
	SRSInt16			CompressorLevel;	//0.0~1.0
	SRSTBHDSpeakerSize	SpkSize; //{AudioFilter, Analysis Filter}
	SRSTBHDMode			Mode;		//Mono mode or stereo mode TB
} SRSWowhdxTruBassControls;

typedef struct
{
	SRSInt16			SpaceLevel;
	SRSInt16			CenterLevel;
	SRSBool				HeadphoneEnable;
	SRSBool				HighBitRateEnable;
	SRSSrs3DMode		SRS3DMode;
} SRSWowhdxSRS3DControls;

typedef struct
{
	SRSInt16			SpeakerSeparationLevel;	//Speaker separation, 0~1, default 0.272
	SRSInt16			CenterBoostGain;	//Center boost gain, 0.25~1, default 0.625
	SRSBool				HandsetHPFEnable;	//Enable/disable the handset device HPF
} SRSWowhdxWideSrdControls;

typedef struct
{
	SRSBool						Enable;
	SRSInt16					InputGain;
	SRSInt16					OutputGain;
	SRSInt16					BypassGain;
	SRSWowhdxStereoEnhanceMode	EnhanceMode;
	SRSBool						XoverEnable;
	SRSBool						FocusEnable;
	SRSBool						DefEnable;
	SRSInt16					FocusLevel;
	SRSInt16					DefLevel;
	SRSBool						HpfEnable;

	SRSWowhdxTruBassControls	TBHDControls;
	SRSWowhdxSRS3DControls		SRS3DControls;
	SRSWowhdxWideSrdControls	WdSrdControls;
} SRSWowhdxControls;

//////////////////////////////////////////////////////////////////////////
#define _SRS_WHDP_MAX(a, b)	((a)>=(b)? (a):(b))

#endif /*__SRS_WOWHDPLUS_DEF_H__*/
