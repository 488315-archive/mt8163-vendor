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
 *  SRS audio crossover filters (Linkwitz¨CRiley crossover) filter design APIs
 *
 *  Authour: Oscar Huang
 *
 *  RCS keywords:
 *	$Id$
 *  $Author$
 *  $Date$
 *	
********************************************************************************/
#ifndef __SRS_XOVER_DESIGN_API_H__
#define __SRS_XOVER_DESIGN_API_H__

#include "srs_typedefs.h"
#include "srs_xover_design_def.h"

#ifdef __cplusplus
extern "C"{
#endif /*__cplusplus*/

//SRS' specific crossover filter design:

/*************************************************************************************************
 * Design floating point xover filters
 * Parameters:
 *		cutoff: [IN] The cutoff frequency
 *		order:	[IN] The order of the L-R filter
 *		sampleRate: [IN] The sampling rate
 *		lpfCoefs, hpfCoefs: [OUT] The designed LPF and HPF xover filter coefficients. The filter coefficients are
 *                    returned in the array in the following pattern respectively:

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
SRSResult SRS_XoverFilterDesignFloat(float cutoff, int order, int sampleRate, float *lpfCoefs, float *hpfCoefs);

//SRS' specific crossover filter design:
/*************************************************************************************************
 * Design 32-bit/16-bit fixed point xover filters
 * Parameters:
 *		cutoff: [IN] The cutoff frequency
 *		order:	[IN] The order of the L-R filter
 *		sampleRate: [IN] The sampling rate
 *		lpfCoefs, hpfCoefs: [OUT] The designed LPF and HPF xover filter coefficients. The filter coefficients are
 *                    returned in the array in the following pattern respectively:


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
		ws:	[IN] workspace scratch memory, whose size must be at least SRS_XOVER_DESIGN_WORKSPACE_SIZE(order)
 
 * Return: SRS_STAT_NO_ERROR if design succeeds.Otherwise, an error code.

**************************************************************************************************/
SRSResult SRS_XoverFilterDesignFxp16(float cutoff, int order, int sampleRate, SRSInt16 *lpfCoefs, SRSInt16 *hpfCoefs, void *ws);
SRSResult SRS_XoverFilterDesignFxp24(float cutoff, int order, int sampleRate, SRSInt24 *lpfCoefs, SRSInt24 *hpfCoefs, void *ws);
SRSResult SRS_XoverFilterDesignFxp32(float cutoff, int order, int sampleRate, SRSInt32 *lpfCoefs, SRSInt32 *hpfCoefs, void *ws);


//Linkwitz-Riley crossover filter design below:
/*************************************************************************************************
 * Design floating point xover filters
 * Parameters:
 *		cutoff: [IN] The cutoff frequency
 *		order:	[IN] The order of the L-R filter
 *		sampleRate: [IN] The sampling rate
 *		lpfCoefs, hpfCoefs: [OUT] The designed LPF and HPF xover filter coefficients. The filter coefficients are
 *                    returned in the array in the following pattern respectively:

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
SRSResult SRS_LinkwitzRileyDesignFloat(float cutoff, int order, int sampleRate, float *lpfCoefs, float *hpfCoefs);


/*************************************************************************************************
 * Design 32-bit/16-bit fixed point xover filters
 * Parameters:
 *		cutoff: [IN] The cutoff frequency
 *		order:	[IN] The order of the L-R filter
 *		sampleRate: [IN] The sampling rate
 *		lpfCoefs, hpfCoefs: [OUT] The designed LPF and HPF xover filter coefficients. The filter coefficients are
 *                    returned in the array in the following pattern respectively:


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
		ws:	[IN] workspace scratch memory, whose size must be at least SRS_XOVER_DESIGN_WORKSPACE_SIZE(order)
 
 * Return: SRS_STAT_NO_ERROR if design succeeds.Otherwise, an error code.

**************************************************************************************************/
SRSResult SRS_LinkwitzRileyDesignFxp16(float cutoff, int order, int sampleRate, SRSInt16 *lpfCoefs, SRSInt16 *hpfCoefs, void *ws);
SRSResult SRS_LinkwitzRileyDesignFxp32(float cutoff, int order, int sampleRate, SRSInt32 *lpfCoefs, SRSInt32 *hpfCoefs, void *ws);

#ifdef __cplusplus
}
#endif /*__cplusplus*/

#endif //__SRS_XOVER_DESIGN_API_H__
