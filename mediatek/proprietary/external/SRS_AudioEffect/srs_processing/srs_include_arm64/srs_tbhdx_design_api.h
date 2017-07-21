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
 *  SRS TBHDX filter design APIs
 *
 *  Authour: Oscar Huang
 *
 *  RCS keywords:
 *	$Id$
 *  $Author: oscarh $
 *  $Date: 2012/05/10$
 *	
********************************************************************************/
#ifndef __SRS_TBHDX_DESIGN_API_H__
#define __SRS_TBHDX_DESIGN_API_H__

#include "srs_tbhdx_design_def.h"
#include "srs_typedefs.h"

#ifdef __cplusplus
extern "C"{
#endif /*__cplusplus*/


/******************************************************************************************
 * Design floating point filters for TBHDX
 * Parameters:
 *	spkSize: [IN] The speaker size in Hz, must be positive
 *  tGain: [IN] Temporal gain, a.k.a Bass Control, valid range [0.0, 1.0]
 *  hpRatio: [IN] High pass ratio, valid range [0, 1.0]
 *  extBass: [IN] Extended bass control, valid range [0, 1.0]
 *  samplingRate: [IN] The sampling rate in Hz
 *  coefs: [OUT] The buffer for the designed filter coefficients. The size of the buffer must be
 *               at least SRS_TBHDX_FLOAT_COEFFICIENT_ARRAY_LEN elements. Filter coefficients are 
 *               arranged in the following pattern:
	union
	{
		struct
		{
			float					LowPassFilterCoef[8];			//{b0,b1,a1;  b0,b1,b2,a1,a2}, the first 3 elements are coefficients for 1st order LPF, the rest are for 2nd order LPF
			float					HighPassFilterCoef[8];			//{b0,b1,a1;  b0,b1,b2,a1,a2}, the first 3 elements are coefficients for 1st order HPF, the rest are for 2nd order HPF
			float					ShelvingFilterCoef[5];			//{b0,b1,b2,a1,a2}
			float					GainFilterCoef[5];				//{b0,b1,b2,a1,a2}
			float					HPShelvingFilterCoef[3];		//{b0,b1,a1}

			float					SpkSize;						//safe to cast it to int
			float					tGain;
			float					hpRatio;
			float					extBass;
		} Struct;
		float	Array[SRS_TBHDX_FLOAT_COEFFICIENT_ARRAY_LEN];
	} SRSTBHDXCustomFilters;
*******************************************************************************************/
SRSResult  SRS_TBHDX_FilterDesignFloat(int spkSize, float tGain, float hpRatio, float extBass, int sampleRate, float *coefs);




/******************************************************************************************
 * Design fixed-point filters for TBHDX
 * Parameters:
 *	spkSize: [IN] The speaker size in Hz, must be positive
 *  tGain: [IN] Temporal gain, a.k.a Bass Control, valid range [0.0, 1.0]
 *  hpRatio: [IN] High pass ratio, valid range [0, 1.0]
 *  extBass: [IN] Extended bass control, valid range [0, 1.0]
 *  samplingRate: [IN] The sampling rate in Hz
 *  coefs: [OUT] The buffer for the designed filter coefficients. The size of the buffer must be
 *               at least SRS_TBHDX_FXP_COEFFICIENT_ARRAY_LEN elements. Filter coefficients are 
 *               arranged in the following pattern:
	union
	{
		struct
		{
			fxpType					LowPassFilterCoef[10];		//{iwl, b0,b1,a1;  iwl, b0,b1,b2,a1,a2}the first 4 elements are coefficients for 1st order LPF, the rest are for 2nd order LPF
			fxpType					HighPassFilterCoef[10];		//{iwl, b0,b1,a1;  iwl, b0,b1,b2,a1,a2}the first 4 elements are coefficients for 1st order HPF, the rest are for 2nd order HPF
			fxpType					ShelvingFilterCoef[6];		//{iwl, b0,b1,b2,a1,a2}	
			fxpType					GainFilterCoef[6];			//{iwl, b0,b1,b2,a1,a2}  
			fxpType					HPShelvingFilterCoef[4];	//{iwl, b0,b1,a1}  

			fxpType					SpkSize;					//safe to cast it to int
			fxpType					tGain;						//iwl: 2
			fxpType					hpRatio;					//iwl: 1
			fxpType					extBass;					//iwl: 1
		} Struct;
		fxpType	Array[SRS_TBHDX_FXP_COEFFICIENT_ARRAY_LEN];
	} SRSTBHDXCustomFilters;
 *
 *  ws:  [IN] workspace, whose size must be at least SRS_TBHDX_DESIGN_WORKSPACE_SIZE bytes
*******************************************************************************************/
SRSResult  SRS_TBHDX_FilterDesignFxp32(int spkSize, float tGain, float hpRatio, float extBass, int sampleRate, SRSInt32 *coefs, void *ws);
SRSResult  SRS_TBHDX_FilterDesignFxp24(int spkSize, float tGain, float hpRatio, float extBass, int sampleRate, SRSInt24 *coefs, void *ws);
SRSResult  SRS_TBHDX_FilterDesignFxp16(int spkSize, float tGain, float hpRatio, float extBass, int sampleRate, SRSInt16 *coefs, void *ws);


#ifdef __cplusplus
}
#endif /*__cplusplus*/

#endif //__SRS_TBHDX_DESIGN_API_H__
