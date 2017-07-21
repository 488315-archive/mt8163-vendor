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
 *  SRS TruBass filter design APIs
 *
 *  Authour: Oscar Huang
 *
 *  RCS keywords:
 *	$Id: //srstech/srs_designer/std_fxp/include/srs_trubass_design_api.h#2 $
 *  $Author: oscarh $
 *  $Date: 2011/01/12 $
 *	
********************************************************************************/
#ifndef __SRS_TRUBASS_DESIGN_API_H__
#define __SRS_TRUBASS_DESIGN_API_H__

#include "srs_trubass_design_def.h"
#include "srs_typedefs.h"

#ifdef __cplusplus
extern "C"{
#endif /*__cplusplus*/

/******************************************************************************************
 * Design floating point speaker filters for Trubass
 * Parameters:
 *	lowFreq: [IN] low pass filter cut off of the speaker filter
 *  samplingRate: [IN] the sampling rate
 *  coefs: [OUT] the designed filter coefficients. Filter coefficients are arranged in
 *				  the following pattern:
	struct
	{
		float	LowPassAudioFilterCoef[2]; //b0, a1
		float	LowBandAudioFilterCoef[3]; //b0, a1, a2
		float	MidBandAudioFilterCoef[3]; //b0, a1, a2
	}
 *
*******************************************************************************************/
//void SRS_TruBassFilterDesignFloat(float lowFreq, float samplingRate, float *coefs);

/******************************************************************************************
 * Design floating point speaker filters for Split-Analysis Trubass
 * Parameters:
 *	lowFreqAudio: [IN] low pass filter cut off of the audio filter
 *  lowFreqAnalysis: [IN] low pass filter cut off of the analysis filter
 *  samplingRate: [IN] the sampling rate
 *  speakerCoefs: [OUT] the designed filter coefficients. Filter coefficients are arranged in
 *				  the following pattern:
	union
	{
		struct
		{
			float	LowPassAudioFilterCoef[2]; //b0, a1
			float	LowBandAudioFilterCoef[3]; //b0, a1, a2
			float	MidBandAudioFilterCoef[3]; //b0, a1, a2
			float	LowPassAnalysisFilterCoef[2];
			float	LowBandAnalysisFilterCoef[3];
			float	MidBandAnalysisFilterCoef[3];
		} Struct;
		float	Array[16];
	} 
*******************************************************************************************/
void SRS_TruBassHDFilterDesignFloat(float lowFreqAudio, float lowFreqAnalysis, int sampleRate, float *speakerCoefs);


/******************************************************************************************
 * Design fixed-point speaker filters for TruBass. The returned coefficient structure can
 * be used with the SRS_SetTruBassCustomSpeakerFilterCoefs API of srs_trubass
 * Parameters:
  *  lowFreq: [IN]low pass filter cut off of the speaker filter
  *  samplingRate: [IN] the sampling rate
  *  ws:  [IN] workspace, whose size must be at least SRS_TRUBASS_DESIGN_WORKSPACE_SIZE bytes
* Return:
  * The designed fixed-point coefficient structure
*******************************************************************************************/
//SRSTruBassCustomSpeakerCoefs SRS_TruBassFilterDesignFxp(float lowFreq, float samplingRate, void *ws);


/******************************************************************************************
 * Design 32-bit fixed-point speaker filters for Split-Analysis TruBass
 * Parameters:
 *	lowFreqAudio: [IN] low pass filter cut off of the audio filter
 *  lowFreqAnalysis: [IN] low pass filter cut off of the analysis filter
 *  samplingRate: [IN] the sampling rate
 *  speakerCoefs: [OUT] the designed filter coefficients. Filter coefficients are arranged in
 *				  the following pattern:
	union
	{
		struct
		{
			SRSInt32	LowPassAudioFilterCoef[2];
			SRSInt32	LowBandAudioFilterCoef[3];
			SRSInt32	MidBandAudioFilterCoef[3];
			SRSInt32	LowPassAnalysisFilterCoef[2];
			SRSInt32	LowBandAnalysisFilterCoef[3];
			SRSInt32	MidBandAnalysisFilterCoef[3];
		} Struct;
		SRSInt32	Array[16];
	} 
 *  ws:  [IN] workspace, whose size must be at least SRS_TBHD_DESIGN_WORKSPACE_SIZE bytes
*******************************************************************************************/
void SRS_TruBassHDFilterDesignFxp32(float lowFreqAudio, float lowFreqAnalysis, int sampleRate, SRSInt32 *speakerCoefs, void *ws);
void SRS_TruBassHDFilterDesignFxp24(float lowFreqAudio, float lowFreqAnalysis, int sampleRate, SRSInt24 *speakerCoefs, void *ws);

#ifdef __cplusplus
}
#endif /*__cplusplus*/

#endif //__SRS_TRUBASS_DESIGN_API_H__
