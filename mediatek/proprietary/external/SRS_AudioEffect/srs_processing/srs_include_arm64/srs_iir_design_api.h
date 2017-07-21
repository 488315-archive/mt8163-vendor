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
 *  SRS IIR filter design APIs
 *
 *  Authour: Zesen Zhuang
 *
 *  RCS keywords:
 *	$Id$
 *  $Author$
 *  $Date$
 *	
********************************************************************************/
#ifndef __SRS_IIR_DESIGN_API_H__
#define __SRS_IIR_DESIGN_API_H__

#include "srs_typedefs.h"
#include "srs_iir_design_def.h"

#ifdef __cplusplus
extern "C"{
#endif /*__cplusplus*/

/*************************************************************************************************
 * Design floating point IIR filters
 * Parameters:
 *		cutoff_freq:  [IN] The cutoff frequency
 *		order:		  [IN] Filter order
 *		sampleRate:	  [IN] The sample rate in Hz
 *		type:		  [IN] The type of IIR to design: LPF or HPF
 *		coefs:	[OUT] The designed filter coefficients. The filter coefficients are
 *                    returned in the array in the following pattern:

					//{
					//Band 0:
					//Coefficient B0
					//Coefficient B1
					//Coefficient B2
					//Coefficient A1
					//Coefficient A2
					               
					//Band 1:
					//Coefficient B0
					//Coefficient B1
					//Coefficient B2
					//Coefficient A1
					//Coefficient A2

					//...(more bands)
					      
					//Gain
					}
 
 * Return: SRS_STAT_NO_ERROR if design succeeds.Otherwise, an error code.

**************************************************************************************************/
SRSResult SRS_IirFilterDesignFloat(float cutoff_freq, int order, int sampleRate, SRSIirDesignType type, float *coefs);


/*************************************************************************************************
 * Design 32-bit/16-bit fixed point IIR filters
 * Parameters:
 *		cutoff_freq:  [IN] The cutoff frequency
 *		order:		  [IN] Filter order
 *		sampleRate:	  [IN] The sample rate in Hz
 *		type:		  [IN] The type of IIR to design: LPF or HPF
 *		coefs:	[OUT] The designed filter coefficients.The filter coefficients are
 *                    returned in the array in the following pattern:

					//{
					//Band 0:
					//iwl
					//Coefficient B0
					//Coefficient B1
					//Coefficient B2
					//Coefficient A1
					//Coefficient A2
					               
					//Band 1:
					//iwl
					//Coefficient B0
					//Coefficient B1
					//Coefficient B2
					//Coefficient A1
					//Coefficient A2

					//...(more bands)
					    
					//Gain iwl    
					//Gain
					}
		ws:	[IN] workspace scratch memory, whose size must be at least SRS_IIR_DESIGN_WORKSPACE_SIZE(order)
 
 * Return: SRS_STAT_NO_ERROR if design succeeds.Otherwise, an error code.

**************************************************************************************************/
SRSResult SRS_IirFilterDesignFxp16(float cutoff_freq, int order, int sampleRate, SRSIirDesignType type, SRSInt16 *coefs, void *ws);
SRSResult SRS_IirFilterDesignFxp24(float cutoff_freq, int order, int sampleRate, SRSIirDesignType type, SRSInt24 *coefs, void *ws);
SRSResult SRS_IirFilterDesignFxp32(float cutoff_freq, int order, int sampleRate, SRSIirDesignType type, SRSInt32 *coefs, void *ws);


/*************************************************************************************************
 * Scale floating point IIR filters using time-domain 1-norm. Scaling an IIR filter minimizing the
 * oveflow chances in the fixed-point IIR filtering code.This step should be done prio to quantizing
 * the IIR into fixed-point.
 *
 * Parameters:
 *		coefs:	[IN/OUT] The designed filter coefficients. The filter coefficients are
 *                    returned in the array in the following pattern:

					//{
					//Band 0:
					//Coefficient B0
					//Coefficient B1
					//Coefficient B2
					//Coefficient A1
					//Coefficient A2
					               
					//Band 1:
					//Coefficient B0
					//Coefficient B1
					//Coefficient B2
					//Coefficient A1
					//Coefficient A2

					//...(more bands)
					      
					//Gain
					}
 *		order:	[IN] Filter order
 *		ws:  [IN] workspace scratch memory, whose size must be at least SRS_IIR_DESIGN_WORKSPACE_SIZE(order)
**************************************************************************************************/
void SRS_ScaleCascadedTransposedDirectForm2IIR(float *coefs, int order, void *ws);

/*************************************************************************************************
 * Concatenates two floating point cascaded SOS IIRs.
 *
 *
 * Parameters:
 *		iir1, iir2:	[IN] The two filters to be concatenated. The filter coefficients in the following pattern:

					//{
					//Band 0:
					//Coefficient B0
					//Coefficient B1
					//Coefficient B2
					//Coefficient A1
					//Coefficient A2
					               
					//Band 1:
					//Coefficient B0
					//Coefficient B1
					//Coefficient B2
					//Coefficient A1
					//Coefficient A2

					//...(more bands)
					      
					//Gain
					}
 *		order1, order2:	[IN] Filter orders of iir1 and iir2 respectively
 *		concatenatedIIR: [OUT] The concatenated IIR, which has the same array pattern as iir1 and iir2. 
 *		order:	[OUT] The order of the concatenated IIR, which is (order1+1)/2*2+(order2+1)/2*2.
 *
 * Remarks:
 *		You must allocate sufficent memory pointed to by concatenatedIIR for saving the concatenated filters prior 
 *      to calling the function. The length of the concatenatedIIR array is:
 *                               (order1+1)/2*5 + (order2+1)/2*5 +1, or equivalently:
 *			SRS_IIR_FLOAT_COEFFICIENT_ARRAY_LEN(order1) + SRS_IIR_FLOAT_COEFFICIENT_ARRAY_LEN(order2) - 1
 *
**************************************************************************************************/
void  SRS_ConcatenateIIRs(const float *iir1, int order1, const float *iir2,  int order2,  float *concatenatedIIR, int *order);

#ifdef _DEFINED_FIXED_
/*************************************************************************************************
 *  Quantize a floating point cascaded SOS IIR to fxp32, fxp24 or fxp16.
 *
 *
 * Parameters:
 *		iir:	[IN] The floating point IIR coefficients array
 *		order:	[IN] The order of the IIR
 *		fxpIIR:	[OUT] The quantized fixed-point coefficients array, whose length is SRS_IIR_FXP_COEFFICIENT_ARRAY_LEN(order).
					  Allocate sufficient memory for it prior to calling the functions.
 *
**************************************************************************************************/
void	SRS_QuantizeIIR_Fxp32(const float *iir, int order,  SRSInt32  *fxpIIR);
void	SRS_QuantizeIIR_Fxp24(const float *iir, int order,  SRSInt24  *fxpIIR);
void	SRS_QuantizeIIR_Fxp16(const float *iir, int order,  SRSInt16  *fxpIIR);
#endif

/*************************************************************************************************
 *  Converts fixed-point cascaded SOS IIR to floating point version
 *
 *
 * Parameters:
 *		fxpIIR:	[IN] The fixed-point IIR coefficients array
 *		order:	[IN] The order of the IIR
 *		iir:	[OUT] The converted floating point coefficients array, whose length is SRS_IIR_FLOAT_COEFFICIENT_ARRAY_LEN(order).
					  Allocate sufficient memory for it prior to calling the functions.
 *
**************************************************************************************************/
void	SRS_Fxp32IIRToFloat(const SRSInt32 *fxpIIR, int order, float *iir);	//32-bit fxp to floating point
void	SRS_Fxp24IIRToFloat(const SRSInt24 *fxpIIR, int order, float *iir);	//24-bit fxp to floating point
void	SRS_Fxp16IIRToFloat(const SRSInt16 *fxpIIR, int order, float *iir);	//16-bit fxp to floating point

#ifdef __cplusplus
}
#endif /*__cplusplus*/

#endif //__SRS_IIR_DESIGN_API_H__
