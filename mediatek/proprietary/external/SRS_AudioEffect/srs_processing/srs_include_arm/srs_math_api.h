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
 *  Basic arithmetic functions header file
 *
 *	Author: Oscar Huang
 *	
 *	(RCS keywords below, do not edit)
 *  $Id: //srstech/srs_common/std_fxp/include/srs_math_api.h#9 $
 *  $Author: zesenz $
 *  $Date: 2011/02/16 $
********************************************************************************/

#ifndef __SRS_MATH_API_H__
#define __SRS_MATH_API_H__

#include "srs_typedefs.h"

//32x16=high 32-bit MSB:
//Note: acc and in32 cannot be the same variable
#define SRS_FXP32X16(acc, in32, in16) do{															\
									acc = (SRSInt32)(SRSUint16)(in32) * (SRSInt32)(in16);		\
									acc += 0x8000;													\
									acc >>= 16;														\
									acc += ((in32)>>16) * (SRSInt32)(in16);						\
								  }while(0)


#ifdef __cplusplus
extern "C"{
#endif /*__cplusplus*/

SRSInt32  SRS_CountLeadingZeroes(SRSUint32 xIn);
SRSUint32 SRS_FxpSqrt(SRSUint32 xIn);
SRSUint32 SRS_FxpPow2_32(SRSUint32 x);
SRSUint32 SRS_FxpPow2_16(SRSUint16 x);
SRSInt32  SRS_FxpLog2(SRSUint32 x);
SRSInt32 SRS_CalRecipro(SRSInt32 d, int *iwl);
SRSInt32 SRS_Sin(SRSUint32 x);
SRSInt32 SRS_SinCore(SRSInt32 sign,SRSInt32 x);
SRSInt32 SRS_CosCore(SRSInt32 sign,SRSInt32 x);
SRSInt32 SRS_Cos(SRSUint32 x);
SRSInt32 SRS_Tan(SRSInt32 x);

SRSInt32 SRS_Modulus(SRSInt32 x, SRSInt32 y);

void	SRS_ApplyGain(SRSInt32 *audioIO, int blockSamples, SRSInt16 gain, int gainIwl);	//performs X*gain
void	SRS_ApplyGainWithAnticlip16(SRSInt32 *audioIO, int blockSamples, SRSInt16 gain);	//performs X*gain, then applies anticlip, using 16-bit anticlip table
void	SRS_ApplyGainWithAnticlip32(SRSInt32 *audioIO, int blockSamples, SRSInt16 gain);	//performs X*gain, then applies anticlip, using 32-bit anticlip table

void	SRS_MixAndScale(SRSInt32 *audioIO, SRSInt32 *y, int blockSamples, SRSInt16 gain, int gainIwl); //performs (AudioIO+Y)*gain

void	SRS_LeftShift8Bits(SRSInt32 *audio, int blockSamples);
void	SRS_RightShift8Bits(SRSInt32 *audio, int blockSamples);

void	SRS_PowXY(SRSInt32 *out, int *oiwl, SRSUint32 x, int xiwl, SRSInt32 y, int yiwl);

SRSInt32	SRS_Div(int oiwl, SRSInt32 x, int xiwl, SRSInt32 y, int yiwl);


#ifdef __cplusplus
}
#endif /*__cplusplus*/


#endif /*__SRS_MATH_API_H__*/

