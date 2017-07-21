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
 *	@Copyright 2011 by SRS Labs.
 *	All rights reserved.
 *
 *  Description:
 *  Functions for scrambling/unscrambling IIR and FIR coefficients
 *
 *  Authour: Oscar Huang
 *
 *  RCS keywords:
 *	$Id$
 *  $Author$
 *  $Date$
 *	
********************************************************************************/
#ifndef __SRS_FILTER_SCRAMBLE_API_H__
#define __SRS_FILTER_SCRAMBLE_API_H__

#include "srs_typedefs.h"
#include "srs_designer_def.h"


#ifdef __cplusplus
extern "C"{
#endif /*__cplusplus*/

/*************************************************************************************************
 *  Scrambles/unscrambles SRSFoat32 type SOS cascaded IIR coefficients.
 *
 * Parameters:
 *		orgCoef:[IN] The bit-pattern of the floating point IIR coefficients.
 *		order:	[IN] The order of the IIR
 *		scrambled:	[OUT] The bit-pattern of the scrambled/unscambled filter coefficients.The length
 *                        is SRS_IIR_FLOAT_COEFFICIENT_ARRAY_LEN(order)
 *
 * Remarks:
 *		The function supports in place operation, that is, scrambled can be set to orgCoef.The format
 *		of the filter coefficients is assumed identical to the one in function SRS_IirFilterDesignFloat.
 *
**************************************************************************************************/
void	SRS_Float32IIRScramble(SRSInt32 *orgCoef, int order, SRSInt32 *scrambled);



/*************************************************************************************************
 *  Scrambles/unscrambles SRSFoat32 type FIR coefficients.
 *
 * Parameters:
 *		orgCoef:[IN] The bit-pattern of the floating point FIR coefficients.
 *		order:	[IN] The order of the FIR. order=taps-1
 *		scrambled:	[OUT] The bit-pattern of the scrambled/unscambled filter coefficients.The length
 *                        is order+2
 *
 * Remarks:
 *		The function supports in place operation, that is, scrambled can be set to orgCoef.
 *
**************************************************************************************************/
void	SRS_Float32FIRScramble(SRSInt32 *orgCoef, int order, SRSInt32 *scrambled);


/*************************************************************************************************
 *  Scrambles/unscrambles std_fxp type SOS cascaded IIR coefficients.
 *
 * Parameters:
 *		orgCoef:[IN] The fixed-point IIR coefficients.
 *		order:	[IN] The order of the IIR
 *		scrambled:	[OUT] The scrambled/unscambled filter coefficients.The length
 *                        is SRS_IIR_FXP_COEFFICIENT_ARRAY_LEN(order)
 *
 * Remarks:
 *		The function supports in place operation, that is, scrambled can be set to orgCoef.The format
 *		of the filter coefficients is assumed identical to the one in function SRS_IirFilterDesignFxp32.
 *
**************************************************************************************************/
void	SRS_StdFxpIIRScramble(SRSInt32 *orgCoef, int order, SRSInt32 *scrambled);


/*************************************************************************************************
 *  Scrambles/unscrambles std_fxp type FIR coefficients.
 *
 * Parameters:
 *		orgCoef:[IN] The fixed-point FIR coefficients.
 *		order:	[IN] The order of the FIR. order=taps-1
 *		scrambled:	[OUT] The bit-pattern of the scrambled/unscambled filter coefficients. The length
 *                        is order+4
 *
 * Remarks:
 *		The function supports in place operation, that is, scrambled can be set to orgCoef.
 *
**************************************************************************************************/
void	SRS_StdFxpFIRScramble(SRSInt32 *orgCoef, int order, SRSInt32 *scrambled);


/*************************************************************************************************
 *  Scrambles/unscrambles 24-bit fxp type SOS cascaded IIR coefficients.
 *
 * Parameters:
 *		orgCoef:[IN] The fixed-point IIR coefficients.
 *		order:	[IN] The order of the IIR
 *		scrambled:	[OUT] The scrambled/unscambled filter coefficients. The length
 *                        is SRS_IIR_FXP_COEFFICIENT_ARRAY_LEN(order)
 *
 * Remarks:
 *		The function supports in place operation, that is, scrambled can be set to orgCoef.The format
 *		of the filter coefficients is assumed identical to the one in function SRS_IirFilterDesignFxp24.
 *
**************************************************************************************************/
void	SRS_Fxp24IIRScramble(SRSInt24 *orgCoef, int order, SRSInt24 *scrambled);


/*************************************************************************************************
 *  Scrambles/unscrambles std_fxp type FIR coefficients.
 *
 * Parameters:
 *		orgCoef:[IN] The fixed-point FIR coefficients.
 *		order:	[IN] The order of the FIR. order=taps-1
 *		scrambled:	[OUT] The bit-pattern of the scrambled/unscambled filter coefficients. The length
 *                        is order+4
 *
 * Remarks:
 *		The function supports in place operation, that is, scrambled can be set to orgCoef.
 *
**************************************************************************************************/
void	SRS_Fxp24FIRScramble(SRSInt24 *orgCoef, int order, SRSInt24 *scrambled);


/*************************************************************************************************
 *  Scrambles/unscrambles 16-bit fxp type SOS cascaded IIR coefficients.
 *
 * Parameters:
 *		orgCoef:[IN] The fixed-point IIR coefficients.
 *		order:	[IN] The order of the IIR
 *		scrambled:	[OUT] The scrambled/unscambled filter coefficients. The length
 *                        is SRS_IIR_FXP_COEFFICIENT_ARRAY_LEN(order)
 *
 * Remarks:
 *		The function supports in place operation, that is, scrambled can be set to orgCoef.The format
 *		of the filter coefficients is assumed identical to the one in function SRS_IirFilterDesignFxp16.
 *
**************************************************************************************************/
void	SRS_Fxp16IIRScramble(SRSInt16 *orgCoef, int order, SRSInt16 *scrambled);


/*************************************************************************************************
 *  Scrambles/unscrambles std_fxp type FIR coefficients.
 *
 * Parameters:
 *		orgCoef:[IN] The fixed-point FIR coefficients.
 *		order:	[IN] The order of the FIR. order=taps-1
 *		scrambled:	[OUT] The bit-pattern of the scrambled/unscambled filter coefficients. The length
 *                        is order+4
 *
 * Remarks:
 *		The function supports in place operation, that is, scrambled can be set to orgCoef.
 *
**************************************************************************************************/
void	SRS_Fxp16FIRScramble(SRSInt16 *orgCoef, int order, SRSInt16 *scrambled);


#ifdef __cplusplus
}
#endif /*__cplusplus*/

#endif //__SRS_FILTER_SCRAMBLE_API_H__
