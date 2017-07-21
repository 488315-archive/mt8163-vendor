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
 *  The header file exposes all filter implementation APIs
 *
 *	Author: Oscar Huang
 *
 *$Id: //srstech/srs_common/std_fxp/include/srs_filters_api.h#6 $
 *$Author: oscarh $
 *$Date: 2010/08/12 $
********************************************************************************/

#ifndef __SRS_FILTERS_H__
#define __SRS_FILTERS_H__

#include "srs_typedefs.h"

//Data type definition here:


#ifdef __cplusplus
extern "C"{
#endif /*__cplusplus*/

//API declaration here:

void	SRS_Iir2ndTdf2_c32s64(SRSInt32 *inout, int nsize, const SRSInt32 *coef, int coefIwl, SRSInt64 *state);
void	SRS_Iir2ndTdf2_c32s32(SRSInt32 *inout, int nsize, const SRSInt32 *coef, int coefIwl, SRSInt32 *state);
void	SRS_Iir2ndTdf2_c16(SRSInt32 *inout, int nsize, const SRSInt16 *coef, int coefIwl, SRSInt32 *state);
void	SRS_Iir1stTdf2_c32s64(SRSInt32 *inout, int nsize, const SRSInt32 *coef, int coefIwl, SRSInt64 *state);
void	SRS_Iir1stTdf2_c32s32(SRSInt32 *inout, int nsize, const SRSInt32 *coef, int coefIwl, SRSInt32 *state);
void	SRS_Iir1stTdf2_c16(SRSInt32 *inout, int nsize, const SRSInt16 *coef, int coefIwl, SRSInt32 *state);
void	SRS_Iir2ndDf1_c32(SRSInt32 *inout, int nsize, const SRSInt32 *coef, int coefIwl, SRSInt32 *state);
void	SRS_Iir2ndDf1_c16(SRSInt32 *inout, int nsize, const SRSInt16 *coef, int coefIwl, SRSInt32 *state);
void	SRS_Iir1stDf1_c32(SRSInt32 *inout, int nsize, const SRSInt32 *coef, int coefIwl, SRSInt32 *state);
void	SRS_Iir1stDf1_c16(SRSInt32 *inout, int nsize, const SRSInt16 *coef, int coefIwl, SRSInt32 *state);
void	SRS_PhaseShift90(SRSStereoCh *input, SRSStereoCh *output, SRSInt32 *state, int blockSamples);

void	SRS_Iir2ndTdf2Cascaded_c32s64(SRSInt32 *inout, int blockSamples, const SRSInt32 *coef, SRSInt64 *state, int nSos);
void	SRS_Iir2ndTdf2Cascaded_c32s32(SRSInt32 *inout, int blockSamples, const SRSInt32 *coef, SRSInt32 *state, int nSos);
void	SRS_Iir2ndTdf2Cascaded_c16(SRSInt32 *inout, int blockSamples, const SRSInt16 *coef, SRSInt32 *state, int nSos);

void	SRS_FirFiltering(SRSInt32 *xIn, int blockSamples, const SRSInt32 *coefs, int taps, SRSInt32 *yOut, SRSInt32 *state);

#ifdef __cplusplus
}
#endif /*__cplusplus*/

#endif /*__SRS_FILTERS_H__*/


