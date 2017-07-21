/* Copyright Statement:
*
* This software/firmware and related documentation ("MediaTek Software") are
* protected under relevant copyright laws. The information contained herein
* is confidential and proprietary to MediaTek Inc. and/or its licensors.
* Without the prior written permission of MediaTek inc. and/or its licensors,
* any reproduction, modification, use or disclosure of MediaTek Software,
* and information contained herein, in whole or in part, shall be strictly prohibited.
*/
/* MediaTek Inc. (C) 2010. All rights reserved.
*
* BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
* THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE")
* RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO RECEIVER ON
* AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL WARRANTIES,
* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
* NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
* SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
* SUPPLIED WITH THE MEDIATEK SOFTWARE, AND RECEIVER AGREES TO LOOK ONLY TO SUCH
* THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES
* THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES
* CONTAINED IN MEDIATEK SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK
* SOFTWARE RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
* STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND
* CUMULATIVE LIABILITY WITH RESPECT TO THE MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
* AT MEDIATEK'S OPTION, TO REVISE OR REPLACE THE MEDIATEK SOFTWARE AT ISSUE,
* OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY RECEIVER TO
* MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
*
* The following software/firmware and/or related documentation ("MediaTek Software")
* have been modified by MediaTek Inc. All revisions are subject to any receiver's
 * applicable license agreements with MediaTek Inc.
*/

#ifndef _CAMERA_AE_PLINETABLE_SP2509MIPIRAW_H
#define _CAMERA_AE_PLINETABLE_SP2509MIPIRAW_H

#include "camera_custom_AEPlinetable.h"
static strEvPline sPreviewPLineTable_60Hz =
{
{
    {198,1140,1024, 0, 0, 0},  //TV = 12.30(5 lines)  AV=2.97  SV=4.44  BV=10.83
    {198,1140,1040, 0, 0, 0},  //TV = 12.30(5 lines)  AV=2.97  SV=4.46  BV=10.81
    {198,1216,1048, 0, 0, 0},  //TV = 12.30(5 lines)  AV=2.97  SV=4.57  BV=10.70
    {198,1328,1024, 0, 0, 0},  //TV = 12.30(5 lines)  AV=2.97  SV=4.66  BV=10.61
    {237,1140,1064, 0, 0, 0},  //TV = 12.04(6 lines)  AV=2.97  SV=4.50  BV=10.52
    {237,1216,1072, 0, 0, 0},  //TV = 12.04(6 lines)  AV=2.97  SV=4.60  BV=10.41
    {277,1140,1056, 0, 0, 0},  //TV = 11.82(7 lines)  AV=2.97  SV=4.49  BV=10.30
    {277,1216,1056, 0, 0, 0},  //TV = 11.82(7 lines)  AV=2.97  SV=4.58  BV=10.21
    {316,1140,1056, 0, 0, 0},  //TV = 11.63(8 lines)  AV=2.97  SV=4.49  BV=10.11
    {316,1216,1064, 0, 0, 0},  //TV = 11.63(8 lines)  AV=2.97  SV=4.59  BV=10.01
    {356,1140,1080, 0, 0, 0},  //TV = 11.46(9 lines)  AV=2.97  SV=4.52  BV=9.91
    {395,1140,1040, 0, 0, 0},  //TV = 11.31(10 lines)  AV=2.97  SV=4.46  BV=9.81
    {435,1140,1024, 0, 0, 0},  //TV = 11.17(11 lines)  AV=2.97  SV=4.44  BV=9.70
    {435,1140,1088, 0, 0, 0},  //TV = 11.17(11 lines)  AV=2.97  SV=4.53  BV=9.61
    {474,1140,1064, 0, 0, 0},  //TV = 11.04(12 lines)  AV=2.97  SV=4.50  BV=9.52
    {513,1140,1056, 0, 0, 0},  //TV = 10.93(13 lines)  AV=2.97  SV=4.49  BV=9.41
    {553,1140,1056, 0, 0, 0},  //TV = 10.82(14 lines)  AV=2.97  SV=4.49  BV=9.31
    {592,1140,1056, 0, 0, 0},  //TV = 10.72(15 lines)  AV=2.97  SV=4.49  BV=9.21
    {632,1140,1056, 0, 0, 0},  //TV = 10.63(16 lines)  AV=2.97  SV=4.49  BV=9.11
    {671,1140,1064, 0, 0, 0},  //TV = 10.54(17 lines)  AV=2.97  SV=4.50  BV=9.02
    {750,1140,1024, 0, 0, 0},  //TV = 10.38(19 lines)  AV=2.97  SV=4.44  BV=8.91
    {790,1140,1040, 0, 0, 0},  //TV = 10.31(20 lines)  AV=2.97  SV=4.46  BV=8.81
    {869,1140,1024, 0, 0, 0},  //TV = 10.17(22 lines)  AV=2.97  SV=4.44  BV=8.70
    {908,1140,1040, 0, 0, 0},  //TV = 10.11(23 lines)  AV=2.97  SV=4.46  BV=8.61
    {987,1140,1024, 0, 0, 0},  //TV = 9.98(25 lines)  AV=2.97  SV=4.44  BV=8.51
    {1066,1140,1024, 0, 0, 0},  //TV = 9.87(27 lines)  AV=2.97  SV=4.44  BV=8.40
    {1105,1140,1056, 0, 0, 0},  //TV = 9.82(28 lines)  AV=2.97  SV=4.49  BV=8.31
    {1224,1140,1024, 0, 0, 0},  //TV = 9.67(31 lines)  AV=2.97  SV=4.44  BV=8.20
    {1303,1140,1024, 0, 0, 0},  //TV = 9.58(33 lines)  AV=2.97  SV=4.44  BV=8.11
    {1382,1140,1040, 0, 0, 0},  //TV = 9.50(35 lines)  AV=2.97  SV=4.46  BV=8.01
    {1500,1140,1024, 0, 0, 0},  //TV = 9.38(38 lines)  AV=2.97  SV=4.44  BV=7.91
    {1618,1140,1024, 0, 0, 0},  //TV = 9.27(41 lines)  AV=2.97  SV=4.44  BV=7.80
    {1697,1140,1040, 0, 0, 0},  //TV = 9.20(43 lines)  AV=2.97  SV=4.46  BV=7.71
    {1855,1140,1024, 0, 0, 0},  //TV = 9.07(47 lines)  AV=2.97  SV=4.44  BV=7.60
    {1973,1140,1024, 0, 0, 0},  //TV = 8.99(50 lines)  AV=2.97  SV=4.44  BV=7.51
    {2131,1140,1024, 0, 0, 0},  //TV = 8.87(54 lines)  AV=2.97  SV=4.44  BV=7.40
    {2250,1140,1032, 0, 0, 0},  //TV = 8.80(57 lines)  AV=2.97  SV=4.45  BV=7.31
    {2447,1140,1024, 0, 0, 0},  //TV = 8.67(62 lines)  AV=2.97  SV=4.44  BV=7.20
    {2605,1140,1024, 0, 0, 0},  //TV = 8.58(66 lines)  AV=2.97  SV=4.44  BV=7.11
    {2802,1140,1024, 0, 0, 0},  //TV = 8.48(71 lines)  AV=2.97  SV=4.44  BV=7.01
    {2999,1140,1024, 0, 0, 0},  //TV = 8.38(76 lines)  AV=2.97  SV=4.44  BV=6.91
    {3236,1140,1024, 0, 0, 0},  //TV = 8.27(82 lines)  AV=2.97  SV=4.44  BV=6.80
    {3473,1140,1024, 0, 0, 0},  //TV = 8.17(88 lines)  AV=2.97  SV=4.44  BV=6.70
    {3710,1140,1024, 0, 0, 0},  //TV = 8.07(94 lines)  AV=2.97  SV=4.44  BV=6.60
    {3986,1140,1024, 0, 0, 0},  //TV = 7.97(101 lines)  AV=2.97  SV=4.44  BV=6.50
    {4262,1140,1024, 0, 0, 0},  //TV = 7.87(108 lines)  AV=2.97  SV=4.44  BV=6.40
    {4538,1140,1024, 0, 0, 0},  //TV = 7.78(115 lines)  AV=2.97  SV=4.44  BV=6.31
    {4893,1140,1024, 0, 0, 0},  //TV = 7.68(124 lines)  AV=2.97  SV=4.44  BV=6.20
    {5209,1140,1024, 0, 0, 0},  //TV = 7.58(132 lines)  AV=2.97  SV=4.44  BV=6.11
    {5643,1140,1024, 0, 0, 0},  //TV = 7.47(143 lines)  AV=2.97  SV=4.44  BV=6.00
    {6038,1140,1024, 0, 0, 0},  //TV = 7.37(153 lines)  AV=2.97  SV=4.44  BV=5.90
    {6472,1140,1024, 0, 0, 0},  //TV = 7.27(164 lines)  AV=2.97  SV=4.44  BV=5.80
    {6906,1140,1024, 0, 0, 0},  //TV = 7.18(175 lines)  AV=2.97  SV=4.44  BV=5.71
    {7419,1140,1024, 0, 0, 0},  //TV = 7.07(188 lines)  AV=2.97  SV=4.44  BV=5.60
    {7971,1140,1024, 0, 0, 0},  //TV = 6.97(202 lines)  AV=2.97  SV=4.44  BV=5.50
    {8326,1140,1048, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=4.48  BV=5.40
    {8326,1216,1048, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=4.57  BV=5.31
    {8326,1328,1024, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=4.66  BV=5.22
    {8326,1424,1024, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=4.76  BV=5.12
    {8326,1536,1024, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=4.87  BV=5.01
    {8326,1632,1032, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=4.97  BV=4.91
    {8326,1728,1040, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=5.06  BV=4.81
    {8326,1840,1048, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=5.17  BV=4.71
    {8326,1936,1072, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=5.27  BV=4.61
    {8326,2144,1032, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=5.36  BV=4.51
    {16652,1140,1048, 0, 0, 0},  //TV = 5.91(422 lines)  AV=2.97  SV=4.48  BV=4.40
    {16652,1216,1048, 0, 0, 0},  //TV = 5.91(422 lines)  AV=2.97  SV=4.57  BV=4.31
    {16652,1328,1024, 0, 0, 0},  //TV = 5.91(422 lines)  AV=2.97  SV=4.66  BV=4.22
    {16652,1424,1024, 0, 0, 0},  //TV = 5.91(422 lines)  AV=2.97  SV=4.76  BV=4.12
    {16652,1424,1104, 0, 0, 0},  //TV = 5.91(422 lines)  AV=2.97  SV=4.87  BV=4.01
    {16652,1632,1032, 0, 0, 0},  //TV = 5.91(422 lines)  AV=2.97  SV=4.97  BV=3.91
    {25017,1140,1056, 0, 0, 0},  //TV = 5.32(634 lines)  AV=2.97  SV=4.49  BV=3.81
    {25017,1216,1064, 0, 0, 0},  //TV = 5.32(634 lines)  AV=2.97  SV=4.59  BV=3.70
    {25017,1328,1048, 0, 0, 0},  //TV = 5.32(634 lines)  AV=2.97  SV=4.70  BV=3.60
    {25017,1424,1040, 0, 0, 0},  //TV = 5.32(634 lines)  AV=2.97  SV=4.78  BV=3.51
    {33343,1140,1048, 0, 0, 0},  //TV = 4.91(845 lines)  AV=2.97  SV=4.48  BV=3.40
    {33343,1216,1048, 0, 0, 0},  //TV = 4.91(845 lines)  AV=2.97  SV=4.57  BV=3.31
    {33343,1328,1024, 0, 0, 0},  //TV = 4.91(845 lines)  AV=2.97  SV=4.66  BV=3.22
    {41669,1140,1024, 0, 0, 0},  //TV = 4.58(1056 lines)  AV=2.97  SV=4.44  BV=3.11
    {41669,1216,1032, 0, 0, 0},  //TV = 4.58(1056 lines)  AV=2.97  SV=4.55  BV=3.01
    {41669,1216,1104, 0, 0, 0},  //TV = 4.58(1056 lines)  AV=2.97  SV=4.64  BV=2.91
    {49994,1140,1056, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.49  BV=2.81
    {49994,1216,1064, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.59  BV=2.70
    {49994,1328,1040, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.68  BV=2.61
    {49994,1424,1040, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.78  BV=2.51
    {49994,1536,1032, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.88  BV=2.41
    {49994,1632,1040, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.98  BV=2.31
    {49994,1728,1056, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.09  BV=2.21
    {49994,1840,1064, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.19  BV=2.11
    {49994,2048,1024, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.29  BV=2.01
    {49994,2144,1048, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.39  BV=1.91
    {49994,2240,1072, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.48  BV=1.81
    {49994,2448,1048, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.58  BV=1.72
    {49994,2656,1040, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.68  BV=1.61
    {49994,2864,1032, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.78  BV=1.51
    {49994,3072,1032, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.88  BV=1.41
    {49994,3328,1024, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.99  BV=1.31
    {49994,3456,1056, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=6.09  BV=1.21
    {58319,3200,1048, 0, 0, 0},  //TV = 4.10(1478 lines)  AV=2.97  SV=5.96  BV=1.11
    {66645,3072,1024, 0, 0, 0},  //TV = 3.91(1689 lines)  AV=2.97  SV=5.87  BV=1.01
    {66645,3200,1056, 0, 0, 0},  //TV = 3.91(1689 lines)  AV=2.97  SV=5.98  BV=0.90
    {75011,3072,1048, 0, 0, 0},  //TV = 3.74(1901 lines)  AV=2.97  SV=5.91  BV=0.80
    {75011,3328,1032, 0, 0, 0},  //TV = 3.74(1901 lines)  AV=2.97  SV=6.00  BV=0.71
    {83336,3200,1032, 0, 0, 0},  //TV = 3.58(2112 lines)  AV=2.97  SV=5.94  BV=0.61
    {83336,3456,1024, 0, 0, 0},  //TV = 3.58(2112 lines)  AV=2.97  SV=6.04  BV=0.51
    {83336,3712,1024, 0, 0, 0},  //TV = 3.58(2112 lines)  AV=2.97  SV=6.14  BV=0.41
    {83336,3968,1032, 0, 0, 0},  //TV = 3.58(2112 lines)  AV=2.97  SV=6.25  BV=0.30
    {83336,4224,1032, 0, 0, 0},  //TV = 3.58(2112 lines)  AV=2.97  SV=6.34  BV=0.21
    {91661,4096,1040, 0, 0, 0},  //TV = 3.45(2323 lines)  AV=2.97  SV=6.31  BV=0.11
    {99987,4096,1032, 0, 0, 0},  //TV = 3.32(2534 lines)  AV=2.97  SV=6.30  BV=-0.01
    {108313,4096,1024, 0, 0, 0},  //TV = 3.21(2745 lines)  AV=2.97  SV=6.29  BV=-0.11
    {108313,4352,1032, 0, 0, 0},  //TV = 3.21(2745 lines)  AV=2.97  SV=6.39  BV=-0.21
    {116678,4224,1048, 0, 0, 0},  //TV = 3.10(2957 lines)  AV=2.97  SV=6.36  BV=-0.29
    {125004,4224,1048, 0, 0, 0},  //TV = 3.00(3168 lines)  AV=2.97  SV=6.36  BV=-0.39
    {125004,4608,1032, 0, 0, 0},  //TV = 3.00(3168 lines)  AV=2.97  SV=6.47  BV=-0.50
    {125004,4864,1048, 0, 0, 0},  //TV = 3.00(3168 lines)  AV=2.97  SV=6.57  BV=-0.60
    {125004,5376,1024, 0, 0, 0},  //TV = 3.00(3168 lines)  AV=2.97  SV=6.68  BV=-0.71
    {125004,5632,1040, 0, 0, 0},  //TV = 3.00(3168 lines)  AV=2.97  SV=6.77  BV=-0.80
    {125004,6016,1040, 0, 0, 0},  //TV = 3.00(3168 lines)  AV=2.97  SV=6.86  BV=-0.89
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
},
};

static strEvPline sPreviewPLineTable_50Hz =
{
{
    {198,1140,1024, 0, 0, 0},  //TV = 12.30(5 lines)  AV=2.97  SV=4.44  BV=10.83
    {198,1140,1040, 0, 0, 0},  //TV = 12.30(5 lines)  AV=2.97  SV=4.46  BV=10.81
    {198,1216,1048, 0, 0, 0},  //TV = 12.30(5 lines)  AV=2.97  SV=4.57  BV=10.70
    {198,1328,1024, 0, 0, 0},  //TV = 12.30(5 lines)  AV=2.97  SV=4.66  BV=10.61
    {237,1140,1064, 0, 0, 0},  //TV = 12.04(6 lines)  AV=2.97  SV=4.50  BV=10.52
    {237,1216,1072, 0, 0, 0},  //TV = 12.04(6 lines)  AV=2.97  SV=4.60  BV=10.41
    {277,1140,1056, 0, 0, 0},  //TV = 11.82(7 lines)  AV=2.97  SV=4.49  BV=10.30
    {277,1216,1056, 0, 0, 0},  //TV = 11.82(7 lines)  AV=2.97  SV=4.58  BV=10.21
    {316,1140,1056, 0, 0, 0},  //TV = 11.63(8 lines)  AV=2.97  SV=4.49  BV=10.11
    {316,1216,1064, 0, 0, 0},  //TV = 11.63(8 lines)  AV=2.97  SV=4.59  BV=10.01
    {356,1140,1080, 0, 0, 0},  //TV = 11.46(9 lines)  AV=2.97  SV=4.52  BV=9.91
    {395,1140,1040, 0, 0, 0},  //TV = 11.31(10 lines)  AV=2.97  SV=4.46  BV=9.81
    {435,1140,1024, 0, 0, 0},  //TV = 11.17(11 lines)  AV=2.97  SV=4.44  BV=9.70
    {435,1140,1088, 0, 0, 0},  //TV = 11.17(11 lines)  AV=2.97  SV=4.53  BV=9.61
    {474,1140,1064, 0, 0, 0},  //TV = 11.04(12 lines)  AV=2.97  SV=4.50  BV=9.52
    {513,1140,1056, 0, 0, 0},  //TV = 10.93(13 lines)  AV=2.97  SV=4.49  BV=9.41
    {553,1140,1056, 0, 0, 0},  //TV = 10.82(14 lines)  AV=2.97  SV=4.49  BV=9.31
    {592,1140,1056, 0, 0, 0},  //TV = 10.72(15 lines)  AV=2.97  SV=4.49  BV=9.21
    {632,1140,1056, 0, 0, 0},  //TV = 10.63(16 lines)  AV=2.97  SV=4.49  BV=9.11
    {671,1140,1064, 0, 0, 0},  //TV = 10.54(17 lines)  AV=2.97  SV=4.50  BV=9.02
    {750,1140,1024, 0, 0, 0},  //TV = 10.38(19 lines)  AV=2.97  SV=4.44  BV=8.91
    {790,1140,1040, 0, 0, 0},  //TV = 10.31(20 lines)  AV=2.97  SV=4.46  BV=8.81
    {869,1140,1024, 0, 0, 0},  //TV = 10.17(22 lines)  AV=2.97  SV=4.44  BV=8.70
    {908,1140,1040, 0, 0, 0},  //TV = 10.11(23 lines)  AV=2.97  SV=4.46  BV=8.61
    {987,1140,1024, 0, 0, 0},  //TV = 9.98(25 lines)  AV=2.97  SV=4.44  BV=8.51
    {1066,1140,1024, 0, 0, 0},  //TV = 9.87(27 lines)  AV=2.97  SV=4.44  BV=8.40
    {1105,1140,1056, 0, 0, 0},  //TV = 9.82(28 lines)  AV=2.97  SV=4.49  BV=8.31
    {1224,1140,1024, 0, 0, 0},  //TV = 9.67(31 lines)  AV=2.97  SV=4.44  BV=8.20
    {1303,1140,1024, 0, 0, 0},  //TV = 9.58(33 lines)  AV=2.97  SV=4.44  BV=8.11
    {1382,1140,1040, 0, 0, 0},  //TV = 9.50(35 lines)  AV=2.97  SV=4.46  BV=8.01
    {1500,1140,1024, 0, 0, 0},  //TV = 9.38(38 lines)  AV=2.97  SV=4.44  BV=7.91
    {1618,1140,1024, 0, 0, 0},  //TV = 9.27(41 lines)  AV=2.97  SV=4.44  BV=7.80
    {1697,1140,1040, 0, 0, 0},  //TV = 9.20(43 lines)  AV=2.97  SV=4.46  BV=7.71
    {1855,1140,1024, 0, 0, 0},  //TV = 9.07(47 lines)  AV=2.97  SV=4.44  BV=7.60
    {1973,1140,1024, 0, 0, 0},  //TV = 8.99(50 lines)  AV=2.97  SV=4.44  BV=7.51
    {2131,1140,1024, 0, 0, 0},  //TV = 8.87(54 lines)  AV=2.97  SV=4.44  BV=7.40
    {2250,1140,1032, 0, 0, 0},  //TV = 8.80(57 lines)  AV=2.97  SV=4.45  BV=7.31
    {2447,1140,1024, 0, 0, 0},  //TV = 8.67(62 lines)  AV=2.97  SV=4.44  BV=7.20
    {2605,1140,1024, 0, 0, 0},  //TV = 8.58(66 lines)  AV=2.97  SV=4.44  BV=7.11
    {2802,1140,1024, 0, 0, 0},  //TV = 8.48(71 lines)  AV=2.97  SV=4.44  BV=7.01
    {2999,1140,1024, 0, 0, 0},  //TV = 8.38(76 lines)  AV=2.97  SV=4.44  BV=6.91
    {3236,1140,1024, 0, 0, 0},  //TV = 8.27(82 lines)  AV=2.97  SV=4.44  BV=6.80
    {3473,1140,1024, 0, 0, 0},  //TV = 8.17(88 lines)  AV=2.97  SV=4.44  BV=6.70
    {3710,1140,1024, 0, 0, 0},  //TV = 8.07(94 lines)  AV=2.97  SV=4.44  BV=6.60
    {3986,1140,1024, 0, 0, 0},  //TV = 7.97(101 lines)  AV=2.97  SV=4.44  BV=6.50
    {4262,1140,1024, 0, 0, 0},  //TV = 7.87(108 lines)  AV=2.97  SV=4.44  BV=6.40
    {4538,1140,1024, 0, 0, 0},  //TV = 7.78(115 lines)  AV=2.97  SV=4.44  BV=6.31
    {4893,1140,1024, 0, 0, 0},  //TV = 7.68(124 lines)  AV=2.97  SV=4.44  BV=6.20
    {5209,1140,1024, 0, 0, 0},  //TV = 7.58(132 lines)  AV=2.97  SV=4.44  BV=6.11
    {5643,1140,1024, 0, 0, 0},  //TV = 7.47(143 lines)  AV=2.97  SV=4.44  BV=6.00
    {6038,1140,1024, 0, 0, 0},  //TV = 7.37(153 lines)  AV=2.97  SV=4.44  BV=5.90
    {6472,1140,1024, 0, 0, 0},  //TV = 7.27(164 lines)  AV=2.97  SV=4.44  BV=5.80
    {6906,1140,1024, 0, 0, 0},  //TV = 7.18(175 lines)  AV=2.97  SV=4.44  BV=5.71
    {7419,1140,1024, 0, 0, 0},  //TV = 7.07(188 lines)  AV=2.97  SV=4.44  BV=5.60
    {7971,1140,1024, 0, 0, 0},  //TV = 6.97(202 lines)  AV=2.97  SV=4.44  BV=5.50
    {8484,1140,1024, 0, 0, 0},  //TV = 6.88(215 lines)  AV=2.97  SV=4.44  BV=5.41
    {9155,1140,1024, 0, 0, 0},  //TV = 6.77(232 lines)  AV=2.97  SV=4.44  BV=5.30
    {9826,1140,1024, 0, 0, 0},  //TV = 6.67(249 lines)  AV=2.97  SV=4.44  BV=5.20
    {9983,1140,1072, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=4.51  BV=5.11
    {9983,1216,1072, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=4.60  BV=5.02
    {9983,1328,1056, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=4.71  BV=4.91
    {9983,1424,1056, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=4.81  BV=4.81
    {9983,1536,1048, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=4.91  BV=4.71
    {9983,1632,1064, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=5.01  BV=4.60
    {9983,1728,1072, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=5.11  BV=4.51
    {9983,1936,1024, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=5.21  BV=4.41
    {9983,2048,1040, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=5.31  BV=4.31
    {9983,2144,1064, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=5.41  BV=4.21
    {20006,1140,1064, 0, 0, 0},  //TV = 5.64(507 lines)  AV=2.97  SV=4.50  BV=4.12
    {20006,1216,1072, 0, 0, 0},  //TV = 5.64(507 lines)  AV=2.97  SV=4.60  BV=4.01
    {20006,1328,1048, 0, 0, 0},  //TV = 5.64(507 lines)  AV=2.97  SV=4.70  BV=3.92
    {20006,1424,1064, 0, 0, 0},  //TV = 5.64(507 lines)  AV=2.97  SV=4.82  BV=3.80
    {20006,1536,1048, 0, 0, 0},  //TV = 5.64(507 lines)  AV=2.97  SV=4.91  BV=3.71
    {20006,1632,1056, 0, 0, 0},  //TV = 5.64(507 lines)  AV=2.97  SV=5.00  BV=3.61
    {29989,1140,1080, 0, 0, 0},  //TV = 5.06(760 lines)  AV=2.97  SV=4.52  BV=3.51
    {29989,1216,1096, 0, 0, 0},  //TV = 5.06(760 lines)  AV=2.97  SV=4.63  BV=3.40
    {29989,1328,1064, 0, 0, 0},  //TV = 5.06(760 lines)  AV=2.97  SV=4.72  BV=3.31
    {29989,1424,1072, 0, 0, 0},  //TV = 5.06(760 lines)  AV=2.97  SV=4.83  BV=3.20
    {40011,1140,1064, 0, 0, 0},  //TV = 4.64(1014 lines)  AV=2.97  SV=4.50  BV=3.12
    {40011,1216,1072, 0, 0, 0},  //TV = 4.64(1014 lines)  AV=2.97  SV=4.60  BV=3.01
    {40011,1328,1048, 0, 0, 0},  //TV = 4.64(1014 lines)  AV=2.97  SV=4.70  BV=2.92
    {49994,1140,1056, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.49  BV=2.81
    {49994,1216,1064, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.59  BV=2.70
    {49994,1328,1040, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.68  BV=2.61
    {49994,1424,1040, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.78  BV=2.51
    {49994,1536,1032, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.88  BV=2.41
    {49994,1632,1040, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.98  BV=2.31
    {49994,1728,1056, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.09  BV=2.21
    {49994,1840,1064, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.19  BV=2.11
    {49994,2048,1024, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.29  BV=2.01
    {49994,2144,1048, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.39  BV=1.91
    {49994,2240,1072, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.48  BV=1.81
    {49994,2448,1048, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.58  BV=1.72
    {49994,2656,1040, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.68  BV=1.61
    {49994,2864,1032, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.78  BV=1.51
    {49994,3072,1032, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.88  BV=1.41
    {49994,3328,1024, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.99  BV=1.31
    {49994,3456,1056, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=6.09  BV=1.21
    {60017,3072,1064, 0, 0, 0},  //TV = 4.06(1521 lines)  AV=2.97  SV=5.93  BV=1.10
    {60017,3328,1048, 0, 0, 0},  //TV = 4.06(1521 lines)  AV=2.97  SV=6.02  BV=1.01
    {69999,3072,1048, 0, 0, 0},  //TV = 3.84(1774 lines)  AV=2.97  SV=5.91  BV=0.90
    {69999,3328,1032, 0, 0, 0},  //TV = 3.84(1774 lines)  AV=2.97  SV=6.00  BV=0.81
    {79982,3072,1048, 0, 0, 0},  //TV = 3.64(2027 lines)  AV=2.97  SV=5.91  BV=0.71
    {79982,3328,1032, 0, 0, 0},  //TV = 3.64(2027 lines)  AV=2.97  SV=6.00  BV=0.62
    {79982,3584,1032, 0, 0, 0},  //TV = 3.64(2027 lines)  AV=2.97  SV=6.11  BV=0.51
    {79982,3840,1032, 0, 0, 0},  //TV = 3.64(2027 lines)  AV=2.97  SV=6.20  BV=0.41
    {79982,4096,1040, 0, 0, 0},  //TV = 3.64(2027 lines)  AV=2.97  SV=6.31  BV=0.31
    {79982,4352,1040, 0, 0, 0},  //TV = 3.64(2027 lines)  AV=2.97  SV=6.40  BV=0.22
    {90005,4224,1024, 0, 0, 0},  //TV = 3.47(2281 lines)  AV=2.97  SV=6.33  BV=0.11
    {99987,4096,1032, 0, 0, 0},  //TV = 3.32(2534 lines)  AV=2.97  SV=6.30  BV=-0.01
    {99987,4352,1040, 0, 0, 0},  //TV = 3.32(2534 lines)  AV=2.97  SV=6.40  BV=-0.10
    {110010,4224,1040, 0, 0, 0},  //TV = 3.18(2788 lines)  AV=2.97  SV=6.35  BV=-0.20
    {119992,4096,1056, 0, 0, 0},  //TV = 3.06(3041 lines)  AV=2.97  SV=6.33  BV=-0.30
    {119992,4480,1032, 0, 0, 0},  //TV = 3.06(3041 lines)  AV=2.97  SV=6.43  BV=-0.40
    {119992,4736,1048, 0, 0, 0},  //TV = 3.06(3041 lines)  AV=2.97  SV=6.53  BV=-0.50
    {119992,5120,1040, 0, 0, 0},  //TV = 3.06(3041 lines)  AV=2.97  SV=6.63  BV=-0.60
    {119992,5504,1040, 0, 0, 0},  //TV = 3.06(3041 lines)  AV=2.97  SV=6.74  BV=-0.71
    {119992,5888,1040, 0, 0, 0},  //TV = 3.06(3041 lines)  AV=2.97  SV=6.83  BV=-0.80
    {119992,6144,1064, 0, 0, 0},  //TV = 3.06(3041 lines)  AV=2.97  SV=6.93  BV=-0.90
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
},
};

static strAETable g_AE_PreviewAutoTable =
{
    AETABLE_RPEVIEW_AUTO,    //eAETableID
    119,    //u4TotalIndex
    20,    //u4StrobeTrigerBV
    109,    //i4MaxBV
    -9,    //i4MinBV
    90,    //i4EffectiveMaxBV
    -10,      //i4EffectiveMinBV
    LIB3A_AE_ISO_SPEED_AUTO, //ISO SPEED
    sPreviewPLineTable_60Hz,
    sPreviewPLineTable_50Hz,
    NULL,
};

static strEvPline sCapturePLineTable_60Hz =
{
{
    {198,1140,1024, 0, 0, 0},  //TV = 12.30(5 lines)  AV=2.97  SV=4.44  BV=10.83
    {198,1140,1040, 0, 0, 0},  //TV = 12.30(5 lines)  AV=2.97  SV=4.46  BV=10.81
    {198,1216,1048, 0, 0, 0},  //TV = 12.30(5 lines)  AV=2.97  SV=4.57  BV=10.70
    {198,1328,1024, 0, 0, 0},  //TV = 12.30(5 lines)  AV=2.97  SV=4.66  BV=10.61
    {237,1140,1064, 0, 0, 0},  //TV = 12.04(6 lines)  AV=2.97  SV=4.50  BV=10.52
    {237,1216,1072, 0, 0, 0},  //TV = 12.04(6 lines)  AV=2.97  SV=4.60  BV=10.41
    {277,1140,1056, 0, 0, 0},  //TV = 11.82(7 lines)  AV=2.97  SV=4.49  BV=10.30
    {277,1216,1056, 0, 0, 0},  //TV = 11.82(7 lines)  AV=2.97  SV=4.58  BV=10.21
    {316,1140,1056, 0, 0, 0},  //TV = 11.63(8 lines)  AV=2.97  SV=4.49  BV=10.11
    {316,1216,1064, 0, 0, 0},  //TV = 11.63(8 lines)  AV=2.97  SV=4.59  BV=10.01
    {356,1140,1080, 0, 0, 0},  //TV = 11.46(9 lines)  AV=2.97  SV=4.52  BV=9.91
    {395,1140,1040, 0, 0, 0},  //TV = 11.31(10 lines)  AV=2.97  SV=4.46  BV=9.81
    {435,1140,1024, 0, 0, 0},  //TV = 11.17(11 lines)  AV=2.97  SV=4.44  BV=9.70
    {435,1140,1088, 0, 0, 0},  //TV = 11.17(11 lines)  AV=2.97  SV=4.53  BV=9.61
    {474,1140,1064, 0, 0, 0},  //TV = 11.04(12 lines)  AV=2.97  SV=4.50  BV=9.52
    {513,1140,1056, 0, 0, 0},  //TV = 10.93(13 lines)  AV=2.97  SV=4.49  BV=9.41
    {553,1140,1056, 0, 0, 0},  //TV = 10.82(14 lines)  AV=2.97  SV=4.49  BV=9.31
    {592,1140,1056, 0, 0, 0},  //TV = 10.72(15 lines)  AV=2.97  SV=4.49  BV=9.21
    {632,1140,1056, 0, 0, 0},  //TV = 10.63(16 lines)  AV=2.97  SV=4.49  BV=9.11
    {671,1140,1064, 0, 0, 0},  //TV = 10.54(17 lines)  AV=2.97  SV=4.50  BV=9.02
    {750,1140,1024, 0, 0, 0},  //TV = 10.38(19 lines)  AV=2.97  SV=4.44  BV=8.91
    {790,1140,1040, 0, 0, 0},  //TV = 10.31(20 lines)  AV=2.97  SV=4.46  BV=8.81
    {869,1140,1024, 0, 0, 0},  //TV = 10.17(22 lines)  AV=2.97  SV=4.44  BV=8.70
    {908,1140,1040, 0, 0, 0},  //TV = 10.11(23 lines)  AV=2.97  SV=4.46  BV=8.61
    {987,1140,1024, 0, 0, 0},  //TV = 9.98(25 lines)  AV=2.97  SV=4.44  BV=8.51
    {1066,1140,1024, 0, 0, 0},  //TV = 9.87(27 lines)  AV=2.97  SV=4.44  BV=8.40
    {1105,1140,1056, 0, 0, 0},  //TV = 9.82(28 lines)  AV=2.97  SV=4.49  BV=8.31
    {1224,1140,1024, 0, 0, 0},  //TV = 9.67(31 lines)  AV=2.97  SV=4.44  BV=8.20
    {1303,1140,1024, 0, 0, 0},  //TV = 9.58(33 lines)  AV=2.97  SV=4.44  BV=8.11
    {1382,1140,1040, 0, 0, 0},  //TV = 9.50(35 lines)  AV=2.97  SV=4.46  BV=8.01
    {1500,1140,1024, 0, 0, 0},  //TV = 9.38(38 lines)  AV=2.97  SV=4.44  BV=7.91
    {1618,1140,1024, 0, 0, 0},  //TV = 9.27(41 lines)  AV=2.97  SV=4.44  BV=7.80
    {1697,1140,1040, 0, 0, 0},  //TV = 9.20(43 lines)  AV=2.97  SV=4.46  BV=7.71
    {1855,1140,1024, 0, 0, 0},  //TV = 9.07(47 lines)  AV=2.97  SV=4.44  BV=7.60
    {1973,1140,1024, 0, 0, 0},  //TV = 8.99(50 lines)  AV=2.97  SV=4.44  BV=7.51
    {2131,1140,1024, 0, 0, 0},  //TV = 8.87(54 lines)  AV=2.97  SV=4.44  BV=7.40
    {2250,1140,1032, 0, 0, 0},  //TV = 8.80(57 lines)  AV=2.97  SV=4.45  BV=7.31
    {2447,1140,1024, 0, 0, 0},  //TV = 8.67(62 lines)  AV=2.97  SV=4.44  BV=7.20
    {2605,1140,1024, 0, 0, 0},  //TV = 8.58(66 lines)  AV=2.97  SV=4.44  BV=7.11
    {2802,1140,1024, 0, 0, 0},  //TV = 8.48(71 lines)  AV=2.97  SV=4.44  BV=7.01
    {2999,1140,1024, 0, 0, 0},  //TV = 8.38(76 lines)  AV=2.97  SV=4.44  BV=6.91
    {3236,1140,1024, 0, 0, 0},  //TV = 8.27(82 lines)  AV=2.97  SV=4.44  BV=6.80
    {3473,1140,1024, 0, 0, 0},  //TV = 8.17(88 lines)  AV=2.97  SV=4.44  BV=6.70
    {3710,1140,1024, 0, 0, 0},  //TV = 8.07(94 lines)  AV=2.97  SV=4.44  BV=6.60
    {3986,1140,1024, 0, 0, 0},  //TV = 7.97(101 lines)  AV=2.97  SV=4.44  BV=6.50
    {4262,1140,1024, 0, 0, 0},  //TV = 7.87(108 lines)  AV=2.97  SV=4.44  BV=6.40
    {4538,1140,1024, 0, 0, 0},  //TV = 7.78(115 lines)  AV=2.97  SV=4.44  BV=6.31
    {4893,1140,1024, 0, 0, 0},  //TV = 7.68(124 lines)  AV=2.97  SV=4.44  BV=6.20
    {5209,1140,1024, 0, 0, 0},  //TV = 7.58(132 lines)  AV=2.97  SV=4.44  BV=6.11
    {5643,1140,1024, 0, 0, 0},  //TV = 7.47(143 lines)  AV=2.97  SV=4.44  BV=6.00
    {6038,1140,1024, 0, 0, 0},  //TV = 7.37(153 lines)  AV=2.97  SV=4.44  BV=5.90
    {6472,1140,1024, 0, 0, 0},  //TV = 7.27(164 lines)  AV=2.97  SV=4.44  BV=5.80
    {6906,1140,1024, 0, 0, 0},  //TV = 7.18(175 lines)  AV=2.97  SV=4.44  BV=5.71
    {7419,1140,1024, 0, 0, 0},  //TV = 7.07(188 lines)  AV=2.97  SV=4.44  BV=5.60
    {7971,1140,1024, 0, 0, 0},  //TV = 6.97(202 lines)  AV=2.97  SV=4.44  BV=5.50
    {8326,1140,1048, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=4.48  BV=5.40
    {8326,1216,1048, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=4.57  BV=5.31
    {8326,1328,1024, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=4.66  BV=5.22
    {8326,1424,1024, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=4.76  BV=5.12
    {8326,1536,1024, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=4.87  BV=5.01
    {8326,1632,1032, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=4.97  BV=4.91
    {8326,1728,1040, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=5.06  BV=4.81
    {8326,1840,1048, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=5.17  BV=4.71
    {8326,1936,1072, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=5.27  BV=4.61
    {8326,2144,1032, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=5.36  BV=4.51
    {16652,1140,1048, 0, 0, 0},  //TV = 5.91(422 lines)  AV=2.97  SV=4.48  BV=4.40
    {16652,1216,1048, 0, 0, 0},  //TV = 5.91(422 lines)  AV=2.97  SV=4.57  BV=4.31
    {16652,1328,1024, 0, 0, 0},  //TV = 5.91(422 lines)  AV=2.97  SV=4.66  BV=4.22
    {16652,1424,1024, 0, 0, 0},  //TV = 5.91(422 lines)  AV=2.97  SV=4.76  BV=4.12
    {16652,1424,1104, 0, 0, 0},  //TV = 5.91(422 lines)  AV=2.97  SV=4.87  BV=4.01
    {16652,1632,1032, 0, 0, 0},  //TV = 5.91(422 lines)  AV=2.97  SV=4.97  BV=3.91
    {25017,1140,1056, 0, 0, 0},  //TV = 5.32(634 lines)  AV=2.97  SV=4.49  BV=3.81
    {25017,1216,1064, 0, 0, 0},  //TV = 5.32(634 lines)  AV=2.97  SV=4.59  BV=3.70
    {25017,1328,1048, 0, 0, 0},  //TV = 5.32(634 lines)  AV=2.97  SV=4.70  BV=3.60
    {25017,1424,1040, 0, 0, 0},  //TV = 5.32(634 lines)  AV=2.97  SV=4.78  BV=3.51
    {33343,1140,1048, 0, 0, 0},  //TV = 4.91(845 lines)  AV=2.97  SV=4.48  BV=3.40
    {33343,1216,1048, 0, 0, 0},  //TV = 4.91(845 lines)  AV=2.97  SV=4.57  BV=3.31
    {33343,1328,1024, 0, 0, 0},  //TV = 4.91(845 lines)  AV=2.97  SV=4.66  BV=3.22
    {41669,1140,1024, 0, 0, 0},  //TV = 4.58(1056 lines)  AV=2.97  SV=4.44  BV=3.11
    {41669,1216,1032, 0, 0, 0},  //TV = 4.58(1056 lines)  AV=2.97  SV=4.55  BV=3.01
    {41669,1216,1104, 0, 0, 0},  //TV = 4.58(1056 lines)  AV=2.97  SV=4.64  BV=2.91
    {49994,1140,1056, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.49  BV=2.81
    {49994,1216,1064, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.59  BV=2.70
    {49994,1328,1040, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.68  BV=2.61
    {49994,1424,1040, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.78  BV=2.51
    {49994,1536,1032, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.88  BV=2.41
    {49994,1632,1040, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.98  BV=2.31
    {49994,1728,1056, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.09  BV=2.21
    {49994,1840,1064, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.19  BV=2.11
    {49994,2048,1024, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.29  BV=2.01
    {49994,2144,1048, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.39  BV=1.91
    {49994,2240,1072, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.48  BV=1.81
    {49994,2448,1048, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.58  BV=1.72
    {49994,2656,1040, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.68  BV=1.61
    {49994,2864,1032, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.78  BV=1.51
    {49994,3072,1032, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.88  BV=1.41
    {49994,3328,1024, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.99  BV=1.31
    {49994,3456,1056, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=6.09  BV=1.21
    {58319,3200,1048, 0, 0, 0},  //TV = 4.10(1478 lines)  AV=2.97  SV=5.96  BV=1.11
    {66645,3072,1024, 0, 0, 0},  //TV = 3.91(1689 lines)  AV=2.97  SV=5.87  BV=1.01
    {66645,3200,1056, 0, 0, 0},  //TV = 3.91(1689 lines)  AV=2.97  SV=5.98  BV=0.90
    {75011,3072,1048, 0, 0, 0},  //TV = 3.74(1901 lines)  AV=2.97  SV=5.91  BV=0.80
    {75011,3328,1032, 0, 0, 0},  //TV = 3.74(1901 lines)  AV=2.97  SV=6.00  BV=0.71
    {83336,3200,1032, 0, 0, 0},  //TV = 3.58(2112 lines)  AV=2.97  SV=5.94  BV=0.61
    {83336,3456,1024, 0, 0, 0},  //TV = 3.58(2112 lines)  AV=2.97  SV=6.04  BV=0.51
    {83336,3712,1024, 0, 0, 0},  //TV = 3.58(2112 lines)  AV=2.97  SV=6.14  BV=0.41
    {83336,3968,1032, 0, 0, 0},  //TV = 3.58(2112 lines)  AV=2.97  SV=6.25  BV=0.30
    {83336,4224,1032, 0, 0, 0},  //TV = 3.58(2112 lines)  AV=2.97  SV=6.34  BV=0.21
    {91661,4096,1040, 0, 0, 0},  //TV = 3.45(2323 lines)  AV=2.97  SV=6.31  BV=0.11
    {99987,4096,1032, 0, 0, 0},  //TV = 3.32(2534 lines)  AV=2.97  SV=6.30  BV=-0.01
    {108313,4096,1024, 0, 0, 0},  //TV = 3.21(2745 lines)  AV=2.97  SV=6.29  BV=-0.11
    {108313,4352,1032, 0, 0, 0},  //TV = 3.21(2745 lines)  AV=2.97  SV=6.39  BV=-0.21
    {116678,4224,1048, 0, 0, 0},  //TV = 3.10(2957 lines)  AV=2.97  SV=6.36  BV=-0.29
    {125004,4224,1048, 0, 0, 0},  //TV = 3.00(3168 lines)  AV=2.97  SV=6.36  BV=-0.39
    {125004,4608,1032, 0, 0, 0},  //TV = 3.00(3168 lines)  AV=2.97  SV=6.47  BV=-0.50
    {125004,4864,1048, 0, 0, 0},  //TV = 3.00(3168 lines)  AV=2.97  SV=6.57  BV=-0.60
    {125004,5376,1024, 0, 0, 0},  //TV = 3.00(3168 lines)  AV=2.97  SV=6.68  BV=-0.71
    {125004,5632,1040, 0, 0, 0},  //TV = 3.00(3168 lines)  AV=2.97  SV=6.77  BV=-0.80
    {125004,6016,1040, 0, 0, 0},  //TV = 3.00(3168 lines)  AV=2.97  SV=6.86  BV=-0.89
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
},
};

static strEvPline sCapturePLineTable_50Hz =
{
{
    {198,1140,1024, 0, 0, 0},  //TV = 12.30(5 lines)  AV=2.97  SV=4.44  BV=10.83
    {198,1140,1040, 0, 0, 0},  //TV = 12.30(5 lines)  AV=2.97  SV=4.46  BV=10.81
    {198,1216,1048, 0, 0, 0},  //TV = 12.30(5 lines)  AV=2.97  SV=4.57  BV=10.70
    {198,1328,1024, 0, 0, 0},  //TV = 12.30(5 lines)  AV=2.97  SV=4.66  BV=10.61
    {237,1140,1064, 0, 0, 0},  //TV = 12.04(6 lines)  AV=2.97  SV=4.50  BV=10.52
    {237,1216,1072, 0, 0, 0},  //TV = 12.04(6 lines)  AV=2.97  SV=4.60  BV=10.41
    {277,1140,1056, 0, 0, 0},  //TV = 11.82(7 lines)  AV=2.97  SV=4.49  BV=10.30
    {277,1216,1056, 0, 0, 0},  //TV = 11.82(7 lines)  AV=2.97  SV=4.58  BV=10.21
    {316,1140,1056, 0, 0, 0},  //TV = 11.63(8 lines)  AV=2.97  SV=4.49  BV=10.11
    {316,1216,1064, 0, 0, 0},  //TV = 11.63(8 lines)  AV=2.97  SV=4.59  BV=10.01
    {356,1140,1080, 0, 0, 0},  //TV = 11.46(9 lines)  AV=2.97  SV=4.52  BV=9.91
    {395,1140,1040, 0, 0, 0},  //TV = 11.31(10 lines)  AV=2.97  SV=4.46  BV=9.81
    {435,1140,1024, 0, 0, 0},  //TV = 11.17(11 lines)  AV=2.97  SV=4.44  BV=9.70
    {435,1140,1088, 0, 0, 0},  //TV = 11.17(11 lines)  AV=2.97  SV=4.53  BV=9.61
    {474,1140,1064, 0, 0, 0},  //TV = 11.04(12 lines)  AV=2.97  SV=4.50  BV=9.52
    {513,1140,1056, 0, 0, 0},  //TV = 10.93(13 lines)  AV=2.97  SV=4.49  BV=9.41
    {553,1140,1056, 0, 0, 0},  //TV = 10.82(14 lines)  AV=2.97  SV=4.49  BV=9.31
    {592,1140,1056, 0, 0, 0},  //TV = 10.72(15 lines)  AV=2.97  SV=4.49  BV=9.21
    {632,1140,1056, 0, 0, 0},  //TV = 10.63(16 lines)  AV=2.97  SV=4.49  BV=9.11
    {671,1140,1064, 0, 0, 0},  //TV = 10.54(17 lines)  AV=2.97  SV=4.50  BV=9.02
    {750,1140,1024, 0, 0, 0},  //TV = 10.38(19 lines)  AV=2.97  SV=4.44  BV=8.91
    {790,1140,1040, 0, 0, 0},  //TV = 10.31(20 lines)  AV=2.97  SV=4.46  BV=8.81
    {869,1140,1024, 0, 0, 0},  //TV = 10.17(22 lines)  AV=2.97  SV=4.44  BV=8.70
    {908,1140,1040, 0, 0, 0},  //TV = 10.11(23 lines)  AV=2.97  SV=4.46  BV=8.61
    {987,1140,1024, 0, 0, 0},  //TV = 9.98(25 lines)  AV=2.97  SV=4.44  BV=8.51
    {1066,1140,1024, 0, 0, 0},  //TV = 9.87(27 lines)  AV=2.97  SV=4.44  BV=8.40
    {1105,1140,1056, 0, 0, 0},  //TV = 9.82(28 lines)  AV=2.97  SV=4.49  BV=8.31
    {1224,1140,1024, 0, 0, 0},  //TV = 9.67(31 lines)  AV=2.97  SV=4.44  BV=8.20
    {1303,1140,1024, 0, 0, 0},  //TV = 9.58(33 lines)  AV=2.97  SV=4.44  BV=8.11
    {1382,1140,1040, 0, 0, 0},  //TV = 9.50(35 lines)  AV=2.97  SV=4.46  BV=8.01
    {1500,1140,1024, 0, 0, 0},  //TV = 9.38(38 lines)  AV=2.97  SV=4.44  BV=7.91
    {1618,1140,1024, 0, 0, 0},  //TV = 9.27(41 lines)  AV=2.97  SV=4.44  BV=7.80
    {1697,1140,1040, 0, 0, 0},  //TV = 9.20(43 lines)  AV=2.97  SV=4.46  BV=7.71
    {1855,1140,1024, 0, 0, 0},  //TV = 9.07(47 lines)  AV=2.97  SV=4.44  BV=7.60
    {1973,1140,1024, 0, 0, 0},  //TV = 8.99(50 lines)  AV=2.97  SV=4.44  BV=7.51
    {2131,1140,1024, 0, 0, 0},  //TV = 8.87(54 lines)  AV=2.97  SV=4.44  BV=7.40
    {2250,1140,1032, 0, 0, 0},  //TV = 8.80(57 lines)  AV=2.97  SV=4.45  BV=7.31
    {2447,1140,1024, 0, 0, 0},  //TV = 8.67(62 lines)  AV=2.97  SV=4.44  BV=7.20
    {2605,1140,1024, 0, 0, 0},  //TV = 8.58(66 lines)  AV=2.97  SV=4.44  BV=7.11
    {2802,1140,1024, 0, 0, 0},  //TV = 8.48(71 lines)  AV=2.97  SV=4.44  BV=7.01
    {2999,1140,1024, 0, 0, 0},  //TV = 8.38(76 lines)  AV=2.97  SV=4.44  BV=6.91
    {3236,1140,1024, 0, 0, 0},  //TV = 8.27(82 lines)  AV=2.97  SV=4.44  BV=6.80
    {3473,1140,1024, 0, 0, 0},  //TV = 8.17(88 lines)  AV=2.97  SV=4.44  BV=6.70
    {3710,1140,1024, 0, 0, 0},  //TV = 8.07(94 lines)  AV=2.97  SV=4.44  BV=6.60
    {3986,1140,1024, 0, 0, 0},  //TV = 7.97(101 lines)  AV=2.97  SV=4.44  BV=6.50
    {4262,1140,1024, 0, 0, 0},  //TV = 7.87(108 lines)  AV=2.97  SV=4.44  BV=6.40
    {4538,1140,1024, 0, 0, 0},  //TV = 7.78(115 lines)  AV=2.97  SV=4.44  BV=6.31
    {4893,1140,1024, 0, 0, 0},  //TV = 7.68(124 lines)  AV=2.97  SV=4.44  BV=6.20
    {5209,1140,1024, 0, 0, 0},  //TV = 7.58(132 lines)  AV=2.97  SV=4.44  BV=6.11
    {5643,1140,1024, 0, 0, 0},  //TV = 7.47(143 lines)  AV=2.97  SV=4.44  BV=6.00
    {6038,1140,1024, 0, 0, 0},  //TV = 7.37(153 lines)  AV=2.97  SV=4.44  BV=5.90
    {6472,1140,1024, 0, 0, 0},  //TV = 7.27(164 lines)  AV=2.97  SV=4.44  BV=5.80
    {6906,1140,1024, 0, 0, 0},  //TV = 7.18(175 lines)  AV=2.97  SV=4.44  BV=5.71
    {7419,1140,1024, 0, 0, 0},  //TV = 7.07(188 lines)  AV=2.97  SV=4.44  BV=5.60
    {7971,1140,1024, 0, 0, 0},  //TV = 6.97(202 lines)  AV=2.97  SV=4.44  BV=5.50
    {8484,1140,1024, 0, 0, 0},  //TV = 6.88(215 lines)  AV=2.97  SV=4.44  BV=5.41
    {9155,1140,1024, 0, 0, 0},  //TV = 6.77(232 lines)  AV=2.97  SV=4.44  BV=5.30
    {9826,1140,1024, 0, 0, 0},  //TV = 6.67(249 lines)  AV=2.97  SV=4.44  BV=5.20
    {9983,1140,1072, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=4.51  BV=5.11
    {9983,1216,1072, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=4.60  BV=5.02
    {9983,1328,1056, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=4.71  BV=4.91
    {9983,1424,1056, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=4.81  BV=4.81
    {9983,1536,1048, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=4.91  BV=4.71
    {9983,1632,1064, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=5.01  BV=4.60
    {9983,1728,1072, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=5.11  BV=4.51
    {9983,1936,1024, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=5.21  BV=4.41
    {9983,2048,1040, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=5.31  BV=4.31
    {9983,2144,1064, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=5.41  BV=4.21
    {20006,1140,1064, 0, 0, 0},  //TV = 5.64(507 lines)  AV=2.97  SV=4.50  BV=4.12
    {20006,1216,1072, 0, 0, 0},  //TV = 5.64(507 lines)  AV=2.97  SV=4.60  BV=4.01
    {20006,1328,1048, 0, 0, 0},  //TV = 5.64(507 lines)  AV=2.97  SV=4.70  BV=3.92
    {20006,1424,1064, 0, 0, 0},  //TV = 5.64(507 lines)  AV=2.97  SV=4.82  BV=3.80
    {20006,1536,1048, 0, 0, 0},  //TV = 5.64(507 lines)  AV=2.97  SV=4.91  BV=3.71
    {20006,1632,1056, 0, 0, 0},  //TV = 5.64(507 lines)  AV=2.97  SV=5.00  BV=3.61
    {29989,1140,1080, 0, 0, 0},  //TV = 5.06(760 lines)  AV=2.97  SV=4.52  BV=3.51
    {29989,1216,1096, 0, 0, 0},  //TV = 5.06(760 lines)  AV=2.97  SV=4.63  BV=3.40
    {29989,1328,1064, 0, 0, 0},  //TV = 5.06(760 lines)  AV=2.97  SV=4.72  BV=3.31
    {29989,1424,1072, 0, 0, 0},  //TV = 5.06(760 lines)  AV=2.97  SV=4.83  BV=3.20
    {40011,1140,1064, 0, 0, 0},  //TV = 4.64(1014 lines)  AV=2.97  SV=4.50  BV=3.12
    {40011,1216,1072, 0, 0, 0},  //TV = 4.64(1014 lines)  AV=2.97  SV=4.60  BV=3.01
    {40011,1328,1048, 0, 0, 0},  //TV = 4.64(1014 lines)  AV=2.97  SV=4.70  BV=2.92
    {49994,1140,1056, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.49  BV=2.81
    {49994,1216,1064, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.59  BV=2.70
    {49994,1328,1040, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.68  BV=2.61
    {49994,1424,1040, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.78  BV=2.51
    {49994,1536,1032, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.88  BV=2.41
    {49994,1632,1040, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.98  BV=2.31
    {49994,1728,1056, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.09  BV=2.21
    {49994,1840,1064, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.19  BV=2.11
    {49994,2048,1024, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.29  BV=2.01
    {49994,2144,1048, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.39  BV=1.91
    {49994,2240,1072, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.48  BV=1.81
    {49994,2448,1048, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.58  BV=1.72
    {49994,2656,1040, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.68  BV=1.61
    {49994,2864,1032, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.78  BV=1.51
    {49994,3072,1032, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.88  BV=1.41
    {49994,3328,1024, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.99  BV=1.31
    {49994,3456,1056, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=6.09  BV=1.21
    {60017,3072,1064, 0, 0, 0},  //TV = 4.06(1521 lines)  AV=2.97  SV=5.93  BV=1.10
    {60017,3328,1048, 0, 0, 0},  //TV = 4.06(1521 lines)  AV=2.97  SV=6.02  BV=1.01
    {69999,3072,1048, 0, 0, 0},  //TV = 3.84(1774 lines)  AV=2.97  SV=5.91  BV=0.90
    {69999,3328,1032, 0, 0, 0},  //TV = 3.84(1774 lines)  AV=2.97  SV=6.00  BV=0.81
    {79982,3072,1048, 0, 0, 0},  //TV = 3.64(2027 lines)  AV=2.97  SV=5.91  BV=0.71
    {79982,3328,1032, 0, 0, 0},  //TV = 3.64(2027 lines)  AV=2.97  SV=6.00  BV=0.62
    {79982,3584,1032, 0, 0, 0},  //TV = 3.64(2027 lines)  AV=2.97  SV=6.11  BV=0.51
    {79982,3840,1032, 0, 0, 0},  //TV = 3.64(2027 lines)  AV=2.97  SV=6.20  BV=0.41
    {79982,4096,1040, 0, 0, 0},  //TV = 3.64(2027 lines)  AV=2.97  SV=6.31  BV=0.31
    {79982,4352,1040, 0, 0, 0},  //TV = 3.64(2027 lines)  AV=2.97  SV=6.40  BV=0.22
    {90005,4224,1024, 0, 0, 0},  //TV = 3.47(2281 lines)  AV=2.97  SV=6.33  BV=0.11
    {99987,4096,1032, 0, 0, 0},  //TV = 3.32(2534 lines)  AV=2.97  SV=6.30  BV=-0.01
    {99987,4352,1040, 0, 0, 0},  //TV = 3.32(2534 lines)  AV=2.97  SV=6.40  BV=-0.10
    {110010,4224,1040, 0, 0, 0},  //TV = 3.18(2788 lines)  AV=2.97  SV=6.35  BV=-0.20
    {119992,4096,1056, 0, 0, 0},  //TV = 3.06(3041 lines)  AV=2.97  SV=6.33  BV=-0.30
    {119992,4480,1032, 0, 0, 0},  //TV = 3.06(3041 lines)  AV=2.97  SV=6.43  BV=-0.40
    {119992,4736,1048, 0, 0, 0},  //TV = 3.06(3041 lines)  AV=2.97  SV=6.53  BV=-0.50
    {119992,5120,1040, 0, 0, 0},  //TV = 3.06(3041 lines)  AV=2.97  SV=6.63  BV=-0.60
    {119992,5504,1040, 0, 0, 0},  //TV = 3.06(3041 lines)  AV=2.97  SV=6.74  BV=-0.71
    {119992,5888,1040, 0, 0, 0},  //TV = 3.06(3041 lines)  AV=2.97  SV=6.83  BV=-0.80
    {119992,6144,1064, 0, 0, 0},  //TV = 3.06(3041 lines)  AV=2.97  SV=6.93  BV=-0.90
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
},
};

static strAETable g_AE_CaptureAutoTable =
{
    AETABLE_CAPTURE_AUTO,    //eAETableID
    119,    //u4TotalIndex
    20,    //u4StrobeTrigerBV
    109,    //i4MaxBV
    -9,    //i4MinBV
    90,    //i4EffectiveMaxBV
    0,      //i4EffectiveMinBV
    LIB3A_AE_ISO_SPEED_AUTO, //ISO SPEED
    sCapturePLineTable_60Hz,
    sCapturePLineTable_50Hz,
    NULL,
};

static strEvPline sVideoPLineTable_60Hz =
{
{
    {198,1140,1024, 0, 0, 0},  //TV = 12.30(5 lines)  AV=2.97  SV=4.44  BV=10.83
    {198,1140,1040, 0, 0, 0},  //TV = 12.30(5 lines)  AV=2.97  SV=4.46  BV=10.81
    {198,1216,1048, 0, 0, 0},  //TV = 12.30(5 lines)  AV=2.97  SV=4.57  BV=10.70
    {198,1328,1024, 0, 0, 0},  //TV = 12.30(5 lines)  AV=2.97  SV=4.66  BV=10.61
    {237,1140,1064, 0, 0, 0},  //TV = 12.04(6 lines)  AV=2.97  SV=4.50  BV=10.52
    {237,1216,1072, 0, 0, 0},  //TV = 12.04(6 lines)  AV=2.97  SV=4.60  BV=10.41
    {277,1140,1056, 0, 0, 0},  //TV = 11.82(7 lines)  AV=2.97  SV=4.49  BV=10.30
    {277,1216,1056, 0, 0, 0},  //TV = 11.82(7 lines)  AV=2.97  SV=4.58  BV=10.21
    {316,1140,1056, 0, 0, 0},  //TV = 11.63(8 lines)  AV=2.97  SV=4.49  BV=10.11
    {316,1216,1064, 0, 0, 0},  //TV = 11.63(8 lines)  AV=2.97  SV=4.59  BV=10.01
    {356,1140,1080, 0, 0, 0},  //TV = 11.46(9 lines)  AV=2.97  SV=4.52  BV=9.91
    {395,1140,1040, 0, 0, 0},  //TV = 11.31(10 lines)  AV=2.97  SV=4.46  BV=9.81
    {435,1140,1024, 0, 0, 0},  //TV = 11.17(11 lines)  AV=2.97  SV=4.44  BV=9.70
    {435,1140,1088, 0, 0, 0},  //TV = 11.17(11 lines)  AV=2.97  SV=4.53  BV=9.61
    {474,1140,1064, 0, 0, 0},  //TV = 11.04(12 lines)  AV=2.97  SV=4.50  BV=9.52
    {513,1140,1056, 0, 0, 0},  //TV = 10.93(13 lines)  AV=2.97  SV=4.49  BV=9.41
    {553,1140,1056, 0, 0, 0},  //TV = 10.82(14 lines)  AV=2.97  SV=4.49  BV=9.31
    {592,1140,1056, 0, 0, 0},  //TV = 10.72(15 lines)  AV=2.97  SV=4.49  BV=9.21
    {632,1140,1056, 0, 0, 0},  //TV = 10.63(16 lines)  AV=2.97  SV=4.49  BV=9.11
    {671,1140,1064, 0, 0, 0},  //TV = 10.54(17 lines)  AV=2.97  SV=4.50  BV=9.02
    {750,1140,1024, 0, 0, 0},  //TV = 10.38(19 lines)  AV=2.97  SV=4.44  BV=8.91
    {790,1140,1040, 0, 0, 0},  //TV = 10.31(20 lines)  AV=2.97  SV=4.46  BV=8.81
    {869,1140,1024, 0, 0, 0},  //TV = 10.17(22 lines)  AV=2.97  SV=4.44  BV=8.70
    {908,1140,1040, 0, 0, 0},  //TV = 10.11(23 lines)  AV=2.97  SV=4.46  BV=8.61
    {987,1140,1024, 0, 0, 0},  //TV = 9.98(25 lines)  AV=2.97  SV=4.44  BV=8.51
    {1066,1140,1024, 0, 0, 0},  //TV = 9.87(27 lines)  AV=2.97  SV=4.44  BV=8.40
    {1105,1140,1056, 0, 0, 0},  //TV = 9.82(28 lines)  AV=2.97  SV=4.49  BV=8.31
    {1224,1140,1024, 0, 0, 0},  //TV = 9.67(31 lines)  AV=2.97  SV=4.44  BV=8.20
    {1303,1140,1024, 0, 0, 0},  //TV = 9.58(33 lines)  AV=2.97  SV=4.44  BV=8.11
    {1382,1140,1040, 0, 0, 0},  //TV = 9.50(35 lines)  AV=2.97  SV=4.46  BV=8.01
    {1500,1140,1024, 0, 0, 0},  //TV = 9.38(38 lines)  AV=2.97  SV=4.44  BV=7.91
    {1618,1140,1024, 0, 0, 0},  //TV = 9.27(41 lines)  AV=2.97  SV=4.44  BV=7.80
    {1697,1140,1040, 0, 0, 0},  //TV = 9.20(43 lines)  AV=2.97  SV=4.46  BV=7.71
    {1855,1140,1024, 0, 0, 0},  //TV = 9.07(47 lines)  AV=2.97  SV=4.44  BV=7.60
    {1973,1140,1024, 0, 0, 0},  //TV = 8.99(50 lines)  AV=2.97  SV=4.44  BV=7.51
    {2131,1140,1024, 0, 0, 0},  //TV = 8.87(54 lines)  AV=2.97  SV=4.44  BV=7.40
    {2250,1140,1032, 0, 0, 0},  //TV = 8.80(57 lines)  AV=2.97  SV=4.45  BV=7.31
    {2447,1140,1024, 0, 0, 0},  //TV = 8.67(62 lines)  AV=2.97  SV=4.44  BV=7.20
    {2605,1140,1024, 0, 0, 0},  //TV = 8.58(66 lines)  AV=2.97  SV=4.44  BV=7.11
    {2802,1140,1024, 0, 0, 0},  //TV = 8.48(71 lines)  AV=2.97  SV=4.44  BV=7.01
    {2999,1140,1024, 0, 0, 0},  //TV = 8.38(76 lines)  AV=2.97  SV=4.44  BV=6.91
    {3236,1140,1024, 0, 0, 0},  //TV = 8.27(82 lines)  AV=2.97  SV=4.44  BV=6.80
    {3473,1140,1024, 0, 0, 0},  //TV = 8.17(88 lines)  AV=2.97  SV=4.44  BV=6.70
    {3710,1140,1024, 0, 0, 0},  //TV = 8.07(94 lines)  AV=2.97  SV=4.44  BV=6.60
    {3986,1140,1024, 0, 0, 0},  //TV = 7.97(101 lines)  AV=2.97  SV=4.44  BV=6.50
    {4262,1140,1024, 0, 0, 0},  //TV = 7.87(108 lines)  AV=2.97  SV=4.44  BV=6.40
    {4538,1140,1024, 0, 0, 0},  //TV = 7.78(115 lines)  AV=2.97  SV=4.44  BV=6.31
    {4893,1140,1024, 0, 0, 0},  //TV = 7.68(124 lines)  AV=2.97  SV=4.44  BV=6.20
    {5209,1140,1024, 0, 0, 0},  //TV = 7.58(132 lines)  AV=2.97  SV=4.44  BV=6.11
    {5643,1140,1024, 0, 0, 0},  //TV = 7.47(143 lines)  AV=2.97  SV=4.44  BV=6.00
    {6038,1140,1024, 0, 0, 0},  //TV = 7.37(153 lines)  AV=2.97  SV=4.44  BV=5.90
    {6472,1140,1024, 0, 0, 0},  //TV = 7.27(164 lines)  AV=2.97  SV=4.44  BV=5.80
    {6906,1140,1024, 0, 0, 0},  //TV = 7.18(175 lines)  AV=2.97  SV=4.44  BV=5.71
    {7419,1140,1024, 0, 0, 0},  //TV = 7.07(188 lines)  AV=2.97  SV=4.44  BV=5.60
    {7971,1140,1024, 0, 0, 0},  //TV = 6.97(202 lines)  AV=2.97  SV=4.44  BV=5.50
    {8326,1140,1048, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=4.48  BV=5.40
    {8326,1216,1048, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=4.57  BV=5.31
    {8326,1328,1024, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=4.66  BV=5.22
    {8326,1424,1024, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=4.76  BV=5.12
    {8326,1536,1024, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=4.87  BV=5.01
    {8326,1632,1032, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=4.97  BV=4.91
    {8326,1728,1040, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=5.06  BV=4.81
    {8326,1840,1048, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=5.17  BV=4.71
    {8326,1936,1072, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=5.27  BV=4.61
    {8326,2144,1032, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=5.36  BV=4.51
    {16652,1140,1048, 0, 0, 0},  //TV = 5.91(422 lines)  AV=2.97  SV=4.48  BV=4.40
    {16652,1216,1048, 0, 0, 0},  //TV = 5.91(422 lines)  AV=2.97  SV=4.57  BV=4.31
    {16652,1328,1024, 0, 0, 0},  //TV = 5.91(422 lines)  AV=2.97  SV=4.66  BV=4.22
    {16652,1424,1024, 0, 0, 0},  //TV = 5.91(422 lines)  AV=2.97  SV=4.76  BV=4.12
    {16652,1424,1104, 0, 0, 0},  //TV = 5.91(422 lines)  AV=2.97  SV=4.87  BV=4.01
    {16652,1632,1032, 0, 0, 0},  //TV = 5.91(422 lines)  AV=2.97  SV=4.97  BV=3.91
    {25017,1140,1056, 0, 0, 0},  //TV = 5.32(634 lines)  AV=2.97  SV=4.49  BV=3.81
    {25017,1216,1064, 0, 0, 0},  //TV = 5.32(634 lines)  AV=2.97  SV=4.59  BV=3.70
    {25017,1328,1048, 0, 0, 0},  //TV = 5.32(634 lines)  AV=2.97  SV=4.70  BV=3.60
    {25017,1424,1040, 0, 0, 0},  //TV = 5.32(634 lines)  AV=2.97  SV=4.78  BV=3.51
    {33185,1140,1048, 0, 0, 0},  //TV = 4.91(841 lines)  AV=2.97  SV=4.48  BV=3.41
    {33185,1216,1056, 0, 0, 0},  //TV = 4.91(841 lines)  AV=2.97  SV=4.58  BV=3.30
    {33185,1328,1032, 0, 0, 0},  //TV = 4.91(841 lines)  AV=2.97  SV=4.67  BV=3.21
    {41669,1140,1024, 0, 0, 0},  //TV = 4.58(1056 lines)  AV=2.97  SV=4.44  BV=3.11
    {41669,1216,1032, 0, 0, 0},  //TV = 4.58(1056 lines)  AV=2.97  SV=4.55  BV=3.01
    {41669,1216,1104, 0, 0, 0},  //TV = 4.58(1056 lines)  AV=2.97  SV=4.64  BV=2.91
    {49994,1140,1056, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.49  BV=2.81
    {49994,1216,1064, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.59  BV=2.70
    {49994,1328,1040, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.68  BV=2.61
    {49994,1424,1040, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.78  BV=2.51
    {49994,1536,1032, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.88  BV=2.41
    {49994,1632,1040, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.98  BV=2.31
    {49994,1728,1056, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.09  BV=2.21
    {49994,1840,1064, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.19  BV=2.11
    {49994,2048,1024, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.29  BV=2.01
    {49994,2144,1048, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.39  BV=1.91
    {49994,2240,1072, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.48  BV=1.81
    {49994,2448,1048, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.58  BV=1.72
    {49994,2656,1040, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.68  BV=1.61
    {49994,2864,1032, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.78  BV=1.51
    {49994,3072,1032, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.88  BV=1.41
    {49994,3328,1024, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.99  BV=1.31
    {49994,3456,1056, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=6.09  BV=1.21
    {58319,3200,1048, 0, 0, 0},  //TV = 4.10(1478 lines)  AV=2.97  SV=5.96  BV=1.11
    {66527,3072,1024, 0, 0, 0},  //TV = 3.91(1686 lines)  AV=2.97  SV=5.87  BV=1.01
    {66527,3200,1056, 0, 0, 0},  //TV = 3.91(1686 lines)  AV=2.97  SV=5.98  BV=0.91
    {75011,3072,1048, 0, 0, 0},  //TV = 3.74(1901 lines)  AV=2.97  SV=5.91  BV=0.80
    {75011,3328,1032, 0, 0, 0},  //TV = 3.74(1901 lines)  AV=2.97  SV=6.00  BV=0.71
    {83336,3200,1032, 0, 0, 0},  //TV = 3.58(2112 lines)  AV=2.97  SV=5.94  BV=0.61
    {83336,3456,1024, 0, 0, 0},  //TV = 3.58(2112 lines)  AV=2.97  SV=6.04  BV=0.51
    {83336,3712,1024, 0, 0, 0},  //TV = 3.58(2112 lines)  AV=2.97  SV=6.14  BV=0.41
    {83336,3968,1032, 0, 0, 0},  //TV = 3.58(2112 lines)  AV=2.97  SV=6.25  BV=0.30
    {83336,4224,1032, 0, 0, 0},  //TV = 3.58(2112 lines)  AV=2.97  SV=6.34  BV=0.21
    {91661,4096,1040, 0, 0, 0},  //TV = 3.45(2323 lines)  AV=2.97  SV=6.31  BV=0.11
    {99987,4096,1032, 0, 0, 0},  //TV = 3.32(2534 lines)  AV=2.97  SV=6.30  BV=-0.01
    {108313,4096,1024, 0, 0, 0},  //TV = 3.21(2745 lines)  AV=2.97  SV=6.29  BV=-0.11
    {108313,4352,1032, 0, 0, 0},  //TV = 3.21(2745 lines)  AV=2.97  SV=6.39  BV=-0.21
    {116678,4224,1048, 0, 0, 0},  //TV = 3.10(2957 lines)  AV=2.97  SV=6.36  BV=-0.29
    {125004,4224,1048, 0, 0, 0},  //TV = 3.00(3168 lines)  AV=2.97  SV=6.36  BV=-0.39
    {125004,4608,1032, 0, 0, 0},  //TV = 3.00(3168 lines)  AV=2.97  SV=6.47  BV=-0.50
    {125004,4864,1048, 0, 0, 0},  //TV = 3.00(3168 lines)  AV=2.97  SV=6.57  BV=-0.60
    {125004,5376,1024, 0, 0, 0},  //TV = 3.00(3168 lines)  AV=2.97  SV=6.68  BV=-0.71
    {125004,5632,1040, 0, 0, 0},  //TV = 3.00(3168 lines)  AV=2.97  SV=6.77  BV=-0.80
    {125004,6016,1040, 0, 0, 0},  //TV = 3.00(3168 lines)  AV=2.97  SV=6.86  BV=-0.89
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
},
};

static strEvPline sVideoPLineTable_50Hz =
{
{
    {198,1140,1024, 0, 0, 0},  //TV = 12.30(5 lines)  AV=2.97  SV=4.44  BV=10.83
    {198,1140,1040, 0, 0, 0},  //TV = 12.30(5 lines)  AV=2.97  SV=4.46  BV=10.81
    {198,1216,1048, 0, 0, 0},  //TV = 12.30(5 lines)  AV=2.97  SV=4.57  BV=10.70
    {198,1328,1024, 0, 0, 0},  //TV = 12.30(5 lines)  AV=2.97  SV=4.66  BV=10.61
    {237,1140,1064, 0, 0, 0},  //TV = 12.04(6 lines)  AV=2.97  SV=4.50  BV=10.52
    {237,1216,1072, 0, 0, 0},  //TV = 12.04(6 lines)  AV=2.97  SV=4.60  BV=10.41
    {277,1140,1056, 0, 0, 0},  //TV = 11.82(7 lines)  AV=2.97  SV=4.49  BV=10.30
    {277,1216,1056, 0, 0, 0},  //TV = 11.82(7 lines)  AV=2.97  SV=4.58  BV=10.21
    {316,1140,1056, 0, 0, 0},  //TV = 11.63(8 lines)  AV=2.97  SV=4.49  BV=10.11
    {316,1216,1064, 0, 0, 0},  //TV = 11.63(8 lines)  AV=2.97  SV=4.59  BV=10.01
    {356,1140,1080, 0, 0, 0},  //TV = 11.46(9 lines)  AV=2.97  SV=4.52  BV=9.91
    {395,1140,1040, 0, 0, 0},  //TV = 11.31(10 lines)  AV=2.97  SV=4.46  BV=9.81
    {435,1140,1024, 0, 0, 0},  //TV = 11.17(11 lines)  AV=2.97  SV=4.44  BV=9.70
    {435,1140,1088, 0, 0, 0},  //TV = 11.17(11 lines)  AV=2.97  SV=4.53  BV=9.61
    {474,1140,1064, 0, 0, 0},  //TV = 11.04(12 lines)  AV=2.97  SV=4.50  BV=9.52
    {513,1140,1056, 0, 0, 0},  //TV = 10.93(13 lines)  AV=2.97  SV=4.49  BV=9.41
    {553,1140,1056, 0, 0, 0},  //TV = 10.82(14 lines)  AV=2.97  SV=4.49  BV=9.31
    {592,1140,1056, 0, 0, 0},  //TV = 10.72(15 lines)  AV=2.97  SV=4.49  BV=9.21
    {632,1140,1056, 0, 0, 0},  //TV = 10.63(16 lines)  AV=2.97  SV=4.49  BV=9.11
    {671,1140,1064, 0, 0, 0},  //TV = 10.54(17 lines)  AV=2.97  SV=4.50  BV=9.02
    {750,1140,1024, 0, 0, 0},  //TV = 10.38(19 lines)  AV=2.97  SV=4.44  BV=8.91
    {790,1140,1040, 0, 0, 0},  //TV = 10.31(20 lines)  AV=2.97  SV=4.46  BV=8.81
    {869,1140,1024, 0, 0, 0},  //TV = 10.17(22 lines)  AV=2.97  SV=4.44  BV=8.70
    {908,1140,1040, 0, 0, 0},  //TV = 10.11(23 lines)  AV=2.97  SV=4.46  BV=8.61
    {987,1140,1024, 0, 0, 0},  //TV = 9.98(25 lines)  AV=2.97  SV=4.44  BV=8.51
    {1066,1140,1024, 0, 0, 0},  //TV = 9.87(27 lines)  AV=2.97  SV=4.44  BV=8.40
    {1105,1140,1056, 0, 0, 0},  //TV = 9.82(28 lines)  AV=2.97  SV=4.49  BV=8.31
    {1224,1140,1024, 0, 0, 0},  //TV = 9.67(31 lines)  AV=2.97  SV=4.44  BV=8.20
    {1303,1140,1024, 0, 0, 0},  //TV = 9.58(33 lines)  AV=2.97  SV=4.44  BV=8.11
    {1382,1140,1040, 0, 0, 0},  //TV = 9.50(35 lines)  AV=2.97  SV=4.46  BV=8.01
    {1500,1140,1024, 0, 0, 0},  //TV = 9.38(38 lines)  AV=2.97  SV=4.44  BV=7.91
    {1618,1140,1024, 0, 0, 0},  //TV = 9.27(41 lines)  AV=2.97  SV=4.44  BV=7.80
    {1697,1140,1040, 0, 0, 0},  //TV = 9.20(43 lines)  AV=2.97  SV=4.46  BV=7.71
    {1855,1140,1024, 0, 0, 0},  //TV = 9.07(47 lines)  AV=2.97  SV=4.44  BV=7.60
    {1973,1140,1024, 0, 0, 0},  //TV = 8.99(50 lines)  AV=2.97  SV=4.44  BV=7.51
    {2131,1140,1024, 0, 0, 0},  //TV = 8.87(54 lines)  AV=2.97  SV=4.44  BV=7.40
    {2250,1140,1032, 0, 0, 0},  //TV = 8.80(57 lines)  AV=2.97  SV=4.45  BV=7.31
    {2447,1140,1024, 0, 0, 0},  //TV = 8.67(62 lines)  AV=2.97  SV=4.44  BV=7.20
    {2605,1140,1024, 0, 0, 0},  //TV = 8.58(66 lines)  AV=2.97  SV=4.44  BV=7.11
    {2802,1140,1024, 0, 0, 0},  //TV = 8.48(71 lines)  AV=2.97  SV=4.44  BV=7.01
    {2999,1140,1024, 0, 0, 0},  //TV = 8.38(76 lines)  AV=2.97  SV=4.44  BV=6.91
    {3236,1140,1024, 0, 0, 0},  //TV = 8.27(82 lines)  AV=2.97  SV=4.44  BV=6.80
    {3473,1140,1024, 0, 0, 0},  //TV = 8.17(88 lines)  AV=2.97  SV=4.44  BV=6.70
    {3710,1140,1024, 0, 0, 0},  //TV = 8.07(94 lines)  AV=2.97  SV=4.44  BV=6.60
    {3986,1140,1024, 0, 0, 0},  //TV = 7.97(101 lines)  AV=2.97  SV=4.44  BV=6.50
    {4262,1140,1024, 0, 0, 0},  //TV = 7.87(108 lines)  AV=2.97  SV=4.44  BV=6.40
    {4538,1140,1024, 0, 0, 0},  //TV = 7.78(115 lines)  AV=2.97  SV=4.44  BV=6.31
    {4893,1140,1024, 0, 0, 0},  //TV = 7.68(124 lines)  AV=2.97  SV=4.44  BV=6.20
    {5209,1140,1024, 0, 0, 0},  //TV = 7.58(132 lines)  AV=2.97  SV=4.44  BV=6.11
    {5643,1140,1024, 0, 0, 0},  //TV = 7.47(143 lines)  AV=2.97  SV=4.44  BV=6.00
    {6038,1140,1024, 0, 0, 0},  //TV = 7.37(153 lines)  AV=2.97  SV=4.44  BV=5.90
    {6472,1140,1024, 0, 0, 0},  //TV = 7.27(164 lines)  AV=2.97  SV=4.44  BV=5.80
    {6906,1140,1024, 0, 0, 0},  //TV = 7.18(175 lines)  AV=2.97  SV=4.44  BV=5.71
    {7419,1140,1024, 0, 0, 0},  //TV = 7.07(188 lines)  AV=2.97  SV=4.44  BV=5.60
    {7971,1140,1024, 0, 0, 0},  //TV = 6.97(202 lines)  AV=2.97  SV=4.44  BV=5.50
    {8484,1140,1024, 0, 0, 0},  //TV = 6.88(215 lines)  AV=2.97  SV=4.44  BV=5.41
    {9155,1140,1024, 0, 0, 0},  //TV = 6.77(232 lines)  AV=2.97  SV=4.44  BV=5.30
    {9826,1140,1024, 0, 0, 0},  //TV = 6.67(249 lines)  AV=2.97  SV=4.44  BV=5.20
    {9983,1140,1072, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=4.51  BV=5.11
    {9983,1216,1072, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=4.60  BV=5.02
    {9983,1328,1056, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=4.71  BV=4.91
    {9983,1424,1056, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=4.81  BV=4.81
    {9983,1536,1048, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=4.91  BV=4.71
    {9983,1632,1064, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=5.01  BV=4.60
    {9983,1728,1072, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=5.11  BV=4.51
    {9983,1936,1024, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=5.21  BV=4.41
    {9983,2048,1040, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=5.31  BV=4.31
    {9983,2144,1064, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=5.41  BV=4.21
    {20006,1140,1064, 0, 0, 0},  //TV = 5.64(507 lines)  AV=2.97  SV=4.50  BV=4.12
    {20006,1216,1072, 0, 0, 0},  //TV = 5.64(507 lines)  AV=2.97  SV=4.60  BV=4.01
    {20006,1328,1048, 0, 0, 0},  //TV = 5.64(507 lines)  AV=2.97  SV=4.70  BV=3.92
    {20006,1424,1064, 0, 0, 0},  //TV = 5.64(507 lines)  AV=2.97  SV=4.82  BV=3.80
    {20006,1536,1048, 0, 0, 0},  //TV = 5.64(507 lines)  AV=2.97  SV=4.91  BV=3.71
    {20006,1632,1056, 0, 0, 0},  //TV = 5.64(507 lines)  AV=2.97  SV=5.00  BV=3.61
    {29989,1140,1080, 0, 0, 0},  //TV = 5.06(760 lines)  AV=2.97  SV=4.52  BV=3.51
    {29989,1216,1096, 0, 0, 0},  //TV = 5.06(760 lines)  AV=2.97  SV=4.63  BV=3.40
    {29989,1328,1064, 0, 0, 0},  //TV = 5.06(760 lines)  AV=2.97  SV=4.72  BV=3.31
    {29989,1424,1072, 0, 0, 0},  //TV = 5.06(760 lines)  AV=2.97  SV=4.83  BV=3.20
    {40011,1140,1064, 0, 0, 0},  //TV = 4.64(1014 lines)  AV=2.97  SV=4.50  BV=3.12
    {40011,1216,1072, 0, 0, 0},  //TV = 4.64(1014 lines)  AV=2.97  SV=4.60  BV=3.01
    {40011,1328,1048, 0, 0, 0},  //TV = 4.64(1014 lines)  AV=2.97  SV=4.70  BV=2.92
    {49994,1140,1056, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.49  BV=2.81
    {49994,1216,1064, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.59  BV=2.70
    {49994,1328,1040, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.68  BV=2.61
    {49994,1424,1040, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.78  BV=2.51
    {49994,1536,1032, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.88  BV=2.41
    {49994,1632,1040, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.98  BV=2.31
    {49994,1728,1056, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.09  BV=2.21
    {49994,1840,1064, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.19  BV=2.11
    {49994,2048,1024, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.29  BV=2.01
    {49994,2144,1048, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.39  BV=1.91
    {49994,2240,1072, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.48  BV=1.81
    {49994,2448,1048, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.58  BV=1.72
    {49994,2656,1040, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.68  BV=1.61
    {49994,2864,1032, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.78  BV=1.51
    {49994,3072,1032, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.88  BV=1.41
    {49994,3328,1024, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.99  BV=1.31
    {49994,3456,1056, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=6.09  BV=1.21
    {60017,3072,1064, 0, 0, 0},  //TV = 4.06(1521 lines)  AV=2.97  SV=5.93  BV=1.10
    {60017,3328,1048, 0, 0, 0},  //TV = 4.06(1521 lines)  AV=2.97  SV=6.02  BV=1.01
    {69999,3072,1048, 0, 0, 0},  //TV = 3.84(1774 lines)  AV=2.97  SV=5.91  BV=0.90
    {69999,3328,1032, 0, 0, 0},  //TV = 3.84(1774 lines)  AV=2.97  SV=6.00  BV=0.81
    {79982,3072,1048, 0, 0, 0},  //TV = 3.64(2027 lines)  AV=2.97  SV=5.91  BV=0.71
    {79982,3328,1032, 0, 0, 0},  //TV = 3.64(2027 lines)  AV=2.97  SV=6.00  BV=0.62
    {79982,3584,1032, 0, 0, 0},  //TV = 3.64(2027 lines)  AV=2.97  SV=6.11  BV=0.51
    {79982,3840,1032, 0, 0, 0},  //TV = 3.64(2027 lines)  AV=2.97  SV=6.20  BV=0.41
    {79982,4096,1040, 0, 0, 0},  //TV = 3.64(2027 lines)  AV=2.97  SV=6.31  BV=0.31
    {79982,4352,1040, 0, 0, 0},  //TV = 3.64(2027 lines)  AV=2.97  SV=6.40  BV=0.22
    {90005,4224,1024, 0, 0, 0},  //TV = 3.47(2281 lines)  AV=2.97  SV=6.33  BV=0.11
    {99987,4096,1032, 0, 0, 0},  //TV = 3.32(2534 lines)  AV=2.97  SV=6.30  BV=-0.01
    {99987,4352,1040, 0, 0, 0},  //TV = 3.32(2534 lines)  AV=2.97  SV=6.40  BV=-0.10
    {110010,4224,1040, 0, 0, 0},  //TV = 3.18(2788 lines)  AV=2.97  SV=6.35  BV=-0.20
    {119992,4096,1056, 0, 0, 0},  //TV = 3.06(3041 lines)  AV=2.97  SV=6.33  BV=-0.30
    {119992,4480,1032, 0, 0, 0},  //TV = 3.06(3041 lines)  AV=2.97  SV=6.43  BV=-0.40
    {119992,4736,1048, 0, 0, 0},  //TV = 3.06(3041 lines)  AV=2.97  SV=6.53  BV=-0.50
    {119992,5120,1040, 0, 0, 0},  //TV = 3.06(3041 lines)  AV=2.97  SV=6.63  BV=-0.60
    {119992,5504,1040, 0, 0, 0},  //TV = 3.06(3041 lines)  AV=2.97  SV=6.74  BV=-0.71
    {119992,5888,1040, 0, 0, 0},  //TV = 3.06(3041 lines)  AV=2.97  SV=6.83  BV=-0.80
    {119992,6144,1064, 0, 0, 0},  //TV = 3.06(3041 lines)  AV=2.97  SV=6.93  BV=-0.90
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
},
};

static strAETable g_AE_VideoAutoTable =
{
    AETABLE_VIDEO_AUTO,    //eAETableID
    119,    //u4TotalIndex
    20,    //u4StrobeTrigerBV
    109,    //i4MaxBV
    -9,    //i4MinBV
    90,    //i4EffectiveMaxBV
    0,      //i4EffectiveMinBV
    LIB3A_AE_ISO_SPEED_AUTO, //ISO SPEED
    sVideoPLineTable_60Hz,
    sVideoPLineTable_50Hz,
    NULL,
};

static strEvPline sVideo1PLineTable_60Hz =
{
{
    {198,1140,1024, 0, 0, 0},  //TV = 12.30(5 lines)  AV=2.97  SV=4.44  BV=10.83
    {198,1140,1040, 0, 0, 0},  //TV = 12.30(5 lines)  AV=2.97  SV=4.46  BV=10.81
    {198,1216,1048, 0, 0, 0},  //TV = 12.30(5 lines)  AV=2.97  SV=4.57  BV=10.70
    {198,1328,1024, 0, 0, 0},  //TV = 12.30(5 lines)  AV=2.97  SV=4.66  BV=10.61
    {237,1140,1064, 0, 0, 0},  //TV = 12.04(6 lines)  AV=2.97  SV=4.50  BV=10.52
    {237,1216,1072, 0, 0, 0},  //TV = 12.04(6 lines)  AV=2.97  SV=4.60  BV=10.41
    {277,1140,1056, 0, 0, 0},  //TV = 11.82(7 lines)  AV=2.97  SV=4.49  BV=10.30
    {277,1216,1056, 0, 0, 0},  //TV = 11.82(7 lines)  AV=2.97  SV=4.58  BV=10.21
    {316,1140,1056, 0, 0, 0},  //TV = 11.63(8 lines)  AV=2.97  SV=4.49  BV=10.11
    {316,1216,1064, 0, 0, 0},  //TV = 11.63(8 lines)  AV=2.97  SV=4.59  BV=10.01
    {356,1140,1080, 0, 0, 0},  //TV = 11.46(9 lines)  AV=2.97  SV=4.52  BV=9.91
    {395,1140,1040, 0, 0, 0},  //TV = 11.31(10 lines)  AV=2.97  SV=4.46  BV=9.81
    {435,1140,1024, 0, 0, 0},  //TV = 11.17(11 lines)  AV=2.97  SV=4.44  BV=9.70
    {435,1140,1088, 0, 0, 0},  //TV = 11.17(11 lines)  AV=2.97  SV=4.53  BV=9.61
    {474,1140,1064, 0, 0, 0},  //TV = 11.04(12 lines)  AV=2.97  SV=4.50  BV=9.52
    {513,1140,1056, 0, 0, 0},  //TV = 10.93(13 lines)  AV=2.97  SV=4.49  BV=9.41
    {553,1140,1056, 0, 0, 0},  //TV = 10.82(14 lines)  AV=2.97  SV=4.49  BV=9.31
    {592,1140,1056, 0, 0, 0},  //TV = 10.72(15 lines)  AV=2.97  SV=4.49  BV=9.21
    {632,1140,1056, 0, 0, 0},  //TV = 10.63(16 lines)  AV=2.97  SV=4.49  BV=9.11
    {671,1140,1064, 0, 0, 0},  //TV = 10.54(17 lines)  AV=2.97  SV=4.50  BV=9.02
    {750,1140,1024, 0, 0, 0},  //TV = 10.38(19 lines)  AV=2.97  SV=4.44  BV=8.91
    {790,1140,1040, 0, 0, 0},  //TV = 10.31(20 lines)  AV=2.97  SV=4.46  BV=8.81
    {869,1140,1024, 0, 0, 0},  //TV = 10.17(22 lines)  AV=2.97  SV=4.44  BV=8.70
    {908,1140,1040, 0, 0, 0},  //TV = 10.11(23 lines)  AV=2.97  SV=4.46  BV=8.61
    {987,1140,1024, 0, 0, 0},  //TV = 9.98(25 lines)  AV=2.97  SV=4.44  BV=8.51
    {1066,1140,1024, 0, 0, 0},  //TV = 9.87(27 lines)  AV=2.97  SV=4.44  BV=8.40
    {1105,1140,1056, 0, 0, 0},  //TV = 9.82(28 lines)  AV=2.97  SV=4.49  BV=8.31
    {1224,1140,1024, 0, 0, 0},  //TV = 9.67(31 lines)  AV=2.97  SV=4.44  BV=8.20
    {1303,1140,1024, 0, 0, 0},  //TV = 9.58(33 lines)  AV=2.97  SV=4.44  BV=8.11
    {1382,1140,1040, 0, 0, 0},  //TV = 9.50(35 lines)  AV=2.97  SV=4.46  BV=8.01
    {1500,1140,1024, 0, 0, 0},  //TV = 9.38(38 lines)  AV=2.97  SV=4.44  BV=7.91
    {1618,1140,1024, 0, 0, 0},  //TV = 9.27(41 lines)  AV=2.97  SV=4.44  BV=7.80
    {1697,1140,1040, 0, 0, 0},  //TV = 9.20(43 lines)  AV=2.97  SV=4.46  BV=7.71
    {1855,1140,1024, 0, 0, 0},  //TV = 9.07(47 lines)  AV=2.97  SV=4.44  BV=7.60
    {1973,1140,1024, 0, 0, 0},  //TV = 8.99(50 lines)  AV=2.97  SV=4.44  BV=7.51
    {2131,1140,1024, 0, 0, 0},  //TV = 8.87(54 lines)  AV=2.97  SV=4.44  BV=7.40
    {2250,1140,1032, 0, 0, 0},  //TV = 8.80(57 lines)  AV=2.97  SV=4.45  BV=7.31
    {2447,1140,1024, 0, 0, 0},  //TV = 8.67(62 lines)  AV=2.97  SV=4.44  BV=7.20
    {2605,1140,1024, 0, 0, 0},  //TV = 8.58(66 lines)  AV=2.97  SV=4.44  BV=7.11
    {2802,1140,1024, 0, 0, 0},  //TV = 8.48(71 lines)  AV=2.97  SV=4.44  BV=7.01
    {2999,1140,1024, 0, 0, 0},  //TV = 8.38(76 lines)  AV=2.97  SV=4.44  BV=6.91
    {3236,1140,1024, 0, 0, 0},  //TV = 8.27(82 lines)  AV=2.97  SV=4.44  BV=6.80
    {3473,1140,1024, 0, 0, 0},  //TV = 8.17(88 lines)  AV=2.97  SV=4.44  BV=6.70
    {3710,1140,1024, 0, 0, 0},  //TV = 8.07(94 lines)  AV=2.97  SV=4.44  BV=6.60
    {3986,1140,1024, 0, 0, 0},  //TV = 7.97(101 lines)  AV=2.97  SV=4.44  BV=6.50
    {4262,1140,1024, 0, 0, 0},  //TV = 7.87(108 lines)  AV=2.97  SV=4.44  BV=6.40
    {4538,1140,1024, 0, 0, 0},  //TV = 7.78(115 lines)  AV=2.97  SV=4.44  BV=6.31
    {4893,1140,1024, 0, 0, 0},  //TV = 7.68(124 lines)  AV=2.97  SV=4.44  BV=6.20
    {5209,1140,1024, 0, 0, 0},  //TV = 7.58(132 lines)  AV=2.97  SV=4.44  BV=6.11
    {5643,1140,1024, 0, 0, 0},  //TV = 7.47(143 lines)  AV=2.97  SV=4.44  BV=6.00
    {6038,1140,1024, 0, 0, 0},  //TV = 7.37(153 lines)  AV=2.97  SV=4.44  BV=5.90
    {6472,1140,1024, 0, 0, 0},  //TV = 7.27(164 lines)  AV=2.97  SV=4.44  BV=5.80
    {6906,1140,1024, 0, 0, 0},  //TV = 7.18(175 lines)  AV=2.97  SV=4.44  BV=5.71
    {7419,1140,1024, 0, 0, 0},  //TV = 7.07(188 lines)  AV=2.97  SV=4.44  BV=5.60
    {7971,1140,1024, 0, 0, 0},  //TV = 6.97(202 lines)  AV=2.97  SV=4.44  BV=5.50
    {8326,1140,1048, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=4.48  BV=5.40
    {8326,1216,1048, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=4.57  BV=5.31
    {8326,1328,1024, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=4.66  BV=5.22
    {8326,1424,1024, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=4.76  BV=5.12
    {8326,1536,1024, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=4.87  BV=5.01
    {8326,1632,1032, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=4.97  BV=4.91
    {8326,1728,1040, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=5.06  BV=4.81
    {8326,1840,1048, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=5.17  BV=4.71
    {8326,1936,1072, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=5.27  BV=4.61
    {8326,2144,1032, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=5.36  BV=4.51
    {16652,1140,1048, 0, 0, 0},  //TV = 5.91(422 lines)  AV=2.97  SV=4.48  BV=4.40
    {16652,1216,1048, 0, 0, 0},  //TV = 5.91(422 lines)  AV=2.97  SV=4.57  BV=4.31
    {16652,1328,1024, 0, 0, 0},  //TV = 5.91(422 lines)  AV=2.97  SV=4.66  BV=4.22
    {16652,1424,1024, 0, 0, 0},  //TV = 5.91(422 lines)  AV=2.97  SV=4.76  BV=4.12
    {16652,1424,1104, 0, 0, 0},  //TV = 5.91(422 lines)  AV=2.97  SV=4.87  BV=4.01
    {16652,1632,1032, 0, 0, 0},  //TV = 5.91(422 lines)  AV=2.97  SV=4.97  BV=3.91
    {25017,1140,1056, 0, 0, 0},  //TV = 5.32(634 lines)  AV=2.97  SV=4.49  BV=3.81
    {25017,1216,1064, 0, 0, 0},  //TV = 5.32(634 lines)  AV=2.97  SV=4.59  BV=3.70
    {25017,1328,1048, 0, 0, 0},  //TV = 5.32(634 lines)  AV=2.97  SV=4.70  BV=3.60
    {25017,1424,1040, 0, 0, 0},  //TV = 5.32(634 lines)  AV=2.97  SV=4.78  BV=3.51
    {33185,1140,1048, 0, 0, 0},  //TV = 4.91(841 lines)  AV=2.97  SV=4.48  BV=3.41
    {33185,1216,1056, 0, 0, 0},  //TV = 4.91(841 lines)  AV=2.97  SV=4.58  BV=3.30
    {33185,1328,1032, 0, 0, 0},  //TV = 4.91(841 lines)  AV=2.97  SV=4.67  BV=3.21
    {41669,1140,1024, 0, 0, 0},  //TV = 4.58(1056 lines)  AV=2.97  SV=4.44  BV=3.11
    {41669,1216,1032, 0, 0, 0},  //TV = 4.58(1056 lines)  AV=2.97  SV=4.55  BV=3.01
    {41669,1216,1104, 0, 0, 0},  //TV = 4.58(1056 lines)  AV=2.97  SV=4.64  BV=2.91
    {49994,1140,1056, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.49  BV=2.81
    {49994,1216,1064, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.59  BV=2.70
    {49994,1328,1040, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.68  BV=2.61
    {49994,1424,1040, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.78  BV=2.51
    {49994,1536,1032, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.88  BV=2.41
    {49994,1632,1040, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.98  BV=2.31
    {49994,1728,1056, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.09  BV=2.21
    {49994,1840,1064, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.19  BV=2.11
    {49994,2048,1024, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.29  BV=2.01
    {49994,2144,1048, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.39  BV=1.91
    {49994,2240,1072, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.48  BV=1.81
    {49994,2448,1048, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.58  BV=1.72
    {49994,2656,1040, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.68  BV=1.61
    {49994,2864,1032, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.78  BV=1.51
    {49994,3072,1032, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.88  BV=1.41
    {49994,3328,1024, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.99  BV=1.31
    {49994,3456,1056, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=6.09  BV=1.21
    {58319,3200,1048, 0, 0, 0},  //TV = 4.10(1478 lines)  AV=2.97  SV=5.96  BV=1.11
    {66527,3072,1024, 0, 0, 0},  //TV = 3.91(1686 lines)  AV=2.97  SV=5.87  BV=1.01
    {66527,3200,1056, 0, 0, 0},  //TV = 3.91(1686 lines)  AV=2.97  SV=5.98  BV=0.91
    {75011,3072,1048, 0, 0, 0},  //TV = 3.74(1901 lines)  AV=2.97  SV=5.91  BV=0.80
    {75011,3328,1032, 0, 0, 0},  //TV = 3.74(1901 lines)  AV=2.97  SV=6.00  BV=0.71
    {83336,3200,1032, 0, 0, 0},  //TV = 3.58(2112 lines)  AV=2.97  SV=5.94  BV=0.61
    {83336,3456,1024, 0, 0, 0},  //TV = 3.58(2112 lines)  AV=2.97  SV=6.04  BV=0.51
    {83336,3712,1024, 0, 0, 0},  //TV = 3.58(2112 lines)  AV=2.97  SV=6.14  BV=0.41
    {83336,3968,1032, 0, 0, 0},  //TV = 3.58(2112 lines)  AV=2.97  SV=6.25  BV=0.30
    {83336,4224,1032, 0, 0, 0},  //TV = 3.58(2112 lines)  AV=2.97  SV=6.34  BV=0.21
    {91661,4096,1040, 0, 0, 0},  //TV = 3.45(2323 lines)  AV=2.97  SV=6.31  BV=0.11
    {99987,4096,1032, 0, 0, 0},  //TV = 3.32(2534 lines)  AV=2.97  SV=6.30  BV=-0.01
    {108313,4096,1024, 0, 0, 0},  //TV = 3.21(2745 lines)  AV=2.97  SV=6.29  BV=-0.11
    {108313,4352,1032, 0, 0, 0},  //TV = 3.21(2745 lines)  AV=2.97  SV=6.39  BV=-0.21
    {116678,4224,1048, 0, 0, 0},  //TV = 3.10(2957 lines)  AV=2.97  SV=6.36  BV=-0.29
    {125004,4224,1048, 0, 0, 0},  //TV = 3.00(3168 lines)  AV=2.97  SV=6.36  BV=-0.39
    {125004,4608,1032, 0, 0, 0},  //TV = 3.00(3168 lines)  AV=2.97  SV=6.47  BV=-0.50
    {125004,4864,1048, 0, 0, 0},  //TV = 3.00(3168 lines)  AV=2.97  SV=6.57  BV=-0.60
    {125004,5376,1024, 0, 0, 0},  //TV = 3.00(3168 lines)  AV=2.97  SV=6.68  BV=-0.71
    {125004,5632,1040, 0, 0, 0},  //TV = 3.00(3168 lines)  AV=2.97  SV=6.77  BV=-0.80
    {125004,6016,1040, 0, 0, 0},  //TV = 3.00(3168 lines)  AV=2.97  SV=6.86  BV=-0.89
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
},
};

static strEvPline sVideo1PLineTable_50Hz =
{
{
    {198,1140,1024, 0, 0, 0},  //TV = 12.30(5 lines)  AV=2.97  SV=4.44  BV=10.83
    {198,1140,1040, 0, 0, 0},  //TV = 12.30(5 lines)  AV=2.97  SV=4.46  BV=10.81
    {198,1216,1048, 0, 0, 0},  //TV = 12.30(5 lines)  AV=2.97  SV=4.57  BV=10.70
    {198,1328,1024, 0, 0, 0},  //TV = 12.30(5 lines)  AV=2.97  SV=4.66  BV=10.61
    {237,1140,1064, 0, 0, 0},  //TV = 12.04(6 lines)  AV=2.97  SV=4.50  BV=10.52
    {237,1216,1072, 0, 0, 0},  //TV = 12.04(6 lines)  AV=2.97  SV=4.60  BV=10.41
    {277,1140,1056, 0, 0, 0},  //TV = 11.82(7 lines)  AV=2.97  SV=4.49  BV=10.30
    {277,1216,1056, 0, 0, 0},  //TV = 11.82(7 lines)  AV=2.97  SV=4.58  BV=10.21
    {316,1140,1056, 0, 0, 0},  //TV = 11.63(8 lines)  AV=2.97  SV=4.49  BV=10.11
    {316,1216,1064, 0, 0, 0},  //TV = 11.63(8 lines)  AV=2.97  SV=4.59  BV=10.01
    {356,1140,1080, 0, 0, 0},  //TV = 11.46(9 lines)  AV=2.97  SV=4.52  BV=9.91
    {395,1140,1040, 0, 0, 0},  //TV = 11.31(10 lines)  AV=2.97  SV=4.46  BV=9.81
    {435,1140,1024, 0, 0, 0},  //TV = 11.17(11 lines)  AV=2.97  SV=4.44  BV=9.70
    {435,1140,1088, 0, 0, 0},  //TV = 11.17(11 lines)  AV=2.97  SV=4.53  BV=9.61
    {474,1140,1064, 0, 0, 0},  //TV = 11.04(12 lines)  AV=2.97  SV=4.50  BV=9.52
    {513,1140,1056, 0, 0, 0},  //TV = 10.93(13 lines)  AV=2.97  SV=4.49  BV=9.41
    {553,1140,1056, 0, 0, 0},  //TV = 10.82(14 lines)  AV=2.97  SV=4.49  BV=9.31
    {592,1140,1056, 0, 0, 0},  //TV = 10.72(15 lines)  AV=2.97  SV=4.49  BV=9.21
    {632,1140,1056, 0, 0, 0},  //TV = 10.63(16 lines)  AV=2.97  SV=4.49  BV=9.11
    {671,1140,1064, 0, 0, 0},  //TV = 10.54(17 lines)  AV=2.97  SV=4.50  BV=9.02
    {750,1140,1024, 0, 0, 0},  //TV = 10.38(19 lines)  AV=2.97  SV=4.44  BV=8.91
    {790,1140,1040, 0, 0, 0},  //TV = 10.31(20 lines)  AV=2.97  SV=4.46  BV=8.81
    {869,1140,1024, 0, 0, 0},  //TV = 10.17(22 lines)  AV=2.97  SV=4.44  BV=8.70
    {908,1140,1040, 0, 0, 0},  //TV = 10.11(23 lines)  AV=2.97  SV=4.46  BV=8.61
    {987,1140,1024, 0, 0, 0},  //TV = 9.98(25 lines)  AV=2.97  SV=4.44  BV=8.51
    {1066,1140,1024, 0, 0, 0},  //TV = 9.87(27 lines)  AV=2.97  SV=4.44  BV=8.40
    {1105,1140,1056, 0, 0, 0},  //TV = 9.82(28 lines)  AV=2.97  SV=4.49  BV=8.31
    {1224,1140,1024, 0, 0, 0},  //TV = 9.67(31 lines)  AV=2.97  SV=4.44  BV=8.20
    {1303,1140,1024, 0, 0, 0},  //TV = 9.58(33 lines)  AV=2.97  SV=4.44  BV=8.11
    {1382,1140,1040, 0, 0, 0},  //TV = 9.50(35 lines)  AV=2.97  SV=4.46  BV=8.01
    {1500,1140,1024, 0, 0, 0},  //TV = 9.38(38 lines)  AV=2.97  SV=4.44  BV=7.91
    {1618,1140,1024, 0, 0, 0},  //TV = 9.27(41 lines)  AV=2.97  SV=4.44  BV=7.80
    {1697,1140,1040, 0, 0, 0},  //TV = 9.20(43 lines)  AV=2.97  SV=4.46  BV=7.71
    {1855,1140,1024, 0, 0, 0},  //TV = 9.07(47 lines)  AV=2.97  SV=4.44  BV=7.60
    {1973,1140,1024, 0, 0, 0},  //TV = 8.99(50 lines)  AV=2.97  SV=4.44  BV=7.51
    {2131,1140,1024, 0, 0, 0},  //TV = 8.87(54 lines)  AV=2.97  SV=4.44  BV=7.40
    {2250,1140,1032, 0, 0, 0},  //TV = 8.80(57 lines)  AV=2.97  SV=4.45  BV=7.31
    {2447,1140,1024, 0, 0, 0},  //TV = 8.67(62 lines)  AV=2.97  SV=4.44  BV=7.20
    {2605,1140,1024, 0, 0, 0},  //TV = 8.58(66 lines)  AV=2.97  SV=4.44  BV=7.11
    {2802,1140,1024, 0, 0, 0},  //TV = 8.48(71 lines)  AV=2.97  SV=4.44  BV=7.01
    {2999,1140,1024, 0, 0, 0},  //TV = 8.38(76 lines)  AV=2.97  SV=4.44  BV=6.91
    {3236,1140,1024, 0, 0, 0},  //TV = 8.27(82 lines)  AV=2.97  SV=4.44  BV=6.80
    {3473,1140,1024, 0, 0, 0},  //TV = 8.17(88 lines)  AV=2.97  SV=4.44  BV=6.70
    {3710,1140,1024, 0, 0, 0},  //TV = 8.07(94 lines)  AV=2.97  SV=4.44  BV=6.60
    {3986,1140,1024, 0, 0, 0},  //TV = 7.97(101 lines)  AV=2.97  SV=4.44  BV=6.50
    {4262,1140,1024, 0, 0, 0},  //TV = 7.87(108 lines)  AV=2.97  SV=4.44  BV=6.40
    {4538,1140,1024, 0, 0, 0},  //TV = 7.78(115 lines)  AV=2.97  SV=4.44  BV=6.31
    {4893,1140,1024, 0, 0, 0},  //TV = 7.68(124 lines)  AV=2.97  SV=4.44  BV=6.20
    {5209,1140,1024, 0, 0, 0},  //TV = 7.58(132 lines)  AV=2.97  SV=4.44  BV=6.11
    {5643,1140,1024, 0, 0, 0},  //TV = 7.47(143 lines)  AV=2.97  SV=4.44  BV=6.00
    {6038,1140,1024, 0, 0, 0},  //TV = 7.37(153 lines)  AV=2.97  SV=4.44  BV=5.90
    {6472,1140,1024, 0, 0, 0},  //TV = 7.27(164 lines)  AV=2.97  SV=4.44  BV=5.80
    {6906,1140,1024, 0, 0, 0},  //TV = 7.18(175 lines)  AV=2.97  SV=4.44  BV=5.71
    {7419,1140,1024, 0, 0, 0},  //TV = 7.07(188 lines)  AV=2.97  SV=4.44  BV=5.60
    {7971,1140,1024, 0, 0, 0},  //TV = 6.97(202 lines)  AV=2.97  SV=4.44  BV=5.50
    {8484,1140,1024, 0, 0, 0},  //TV = 6.88(215 lines)  AV=2.97  SV=4.44  BV=5.41
    {9155,1140,1024, 0, 0, 0},  //TV = 6.77(232 lines)  AV=2.97  SV=4.44  BV=5.30
    {9826,1140,1024, 0, 0, 0},  //TV = 6.67(249 lines)  AV=2.97  SV=4.44  BV=5.20
    {9983,1140,1072, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=4.51  BV=5.11
    {9983,1216,1072, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=4.60  BV=5.02
    {9983,1328,1056, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=4.71  BV=4.91
    {9983,1424,1056, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=4.81  BV=4.81
    {9983,1536,1048, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=4.91  BV=4.71
    {9983,1632,1064, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=5.01  BV=4.60
    {9983,1728,1072, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=5.11  BV=4.51
    {9983,1936,1024, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=5.21  BV=4.41
    {9983,2048,1040, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=5.31  BV=4.31
    {9983,2144,1064, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=5.41  BV=4.21
    {20006,1140,1064, 0, 0, 0},  //TV = 5.64(507 lines)  AV=2.97  SV=4.50  BV=4.12
    {20006,1216,1072, 0, 0, 0},  //TV = 5.64(507 lines)  AV=2.97  SV=4.60  BV=4.01
    {20006,1328,1048, 0, 0, 0},  //TV = 5.64(507 lines)  AV=2.97  SV=4.70  BV=3.92
    {20006,1424,1064, 0, 0, 0},  //TV = 5.64(507 lines)  AV=2.97  SV=4.82  BV=3.80
    {20006,1536,1048, 0, 0, 0},  //TV = 5.64(507 lines)  AV=2.97  SV=4.91  BV=3.71
    {20006,1632,1056, 0, 0, 0},  //TV = 5.64(507 lines)  AV=2.97  SV=5.00  BV=3.61
    {29989,1140,1080, 0, 0, 0},  //TV = 5.06(760 lines)  AV=2.97  SV=4.52  BV=3.51
    {29989,1216,1096, 0, 0, 0},  //TV = 5.06(760 lines)  AV=2.97  SV=4.63  BV=3.40
    {29989,1328,1064, 0, 0, 0},  //TV = 5.06(760 lines)  AV=2.97  SV=4.72  BV=3.31
    {29989,1424,1072, 0, 0, 0},  //TV = 5.06(760 lines)  AV=2.97  SV=4.83  BV=3.20
    {40011,1140,1064, 0, 0, 0},  //TV = 4.64(1014 lines)  AV=2.97  SV=4.50  BV=3.12
    {40011,1216,1072, 0, 0, 0},  //TV = 4.64(1014 lines)  AV=2.97  SV=4.60  BV=3.01
    {40011,1328,1048, 0, 0, 0},  //TV = 4.64(1014 lines)  AV=2.97  SV=4.70  BV=2.92
    {49994,1140,1056, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.49  BV=2.81
    {49994,1216,1064, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.59  BV=2.70
    {49994,1328,1040, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.68  BV=2.61
    {49994,1424,1040, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.78  BV=2.51
    {49994,1536,1032, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.88  BV=2.41
    {49994,1632,1040, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.98  BV=2.31
    {49994,1728,1056, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.09  BV=2.21
    {49994,1840,1064, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.19  BV=2.11
    {49994,2048,1024, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.29  BV=2.01
    {49994,2144,1048, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.39  BV=1.91
    {49994,2240,1072, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.48  BV=1.81
    {49994,2448,1048, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.58  BV=1.72
    {49994,2656,1040, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.68  BV=1.61
    {49994,2864,1032, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.78  BV=1.51
    {49994,3072,1032, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.88  BV=1.41
    {49994,3328,1024, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.99  BV=1.31
    {49994,3456,1056, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=6.09  BV=1.21
    {60017,3072,1064, 0, 0, 0},  //TV = 4.06(1521 lines)  AV=2.97  SV=5.93  BV=1.10
    {60017,3328,1048, 0, 0, 0},  //TV = 4.06(1521 lines)  AV=2.97  SV=6.02  BV=1.01
    {69999,3072,1048, 0, 0, 0},  //TV = 3.84(1774 lines)  AV=2.97  SV=5.91  BV=0.90
    {69999,3328,1032, 0, 0, 0},  //TV = 3.84(1774 lines)  AV=2.97  SV=6.00  BV=0.81
    {79982,3072,1048, 0, 0, 0},  //TV = 3.64(2027 lines)  AV=2.97  SV=5.91  BV=0.71
    {79982,3328,1032, 0, 0, 0},  //TV = 3.64(2027 lines)  AV=2.97  SV=6.00  BV=0.62
    {79982,3584,1032, 0, 0, 0},  //TV = 3.64(2027 lines)  AV=2.97  SV=6.11  BV=0.51
    {79982,3840,1032, 0, 0, 0},  //TV = 3.64(2027 lines)  AV=2.97  SV=6.20  BV=0.41
    {79982,4096,1040, 0, 0, 0},  //TV = 3.64(2027 lines)  AV=2.97  SV=6.31  BV=0.31
    {79982,4352,1040, 0, 0, 0},  //TV = 3.64(2027 lines)  AV=2.97  SV=6.40  BV=0.22
    {90005,4224,1024, 0, 0, 0},  //TV = 3.47(2281 lines)  AV=2.97  SV=6.33  BV=0.11
    {99987,4096,1032, 0, 0, 0},  //TV = 3.32(2534 lines)  AV=2.97  SV=6.30  BV=-0.01
    {99987,4352,1040, 0, 0, 0},  //TV = 3.32(2534 lines)  AV=2.97  SV=6.40  BV=-0.10
    {110010,4224,1040, 0, 0, 0},  //TV = 3.18(2788 lines)  AV=2.97  SV=6.35  BV=-0.20
    {119992,4096,1056, 0, 0, 0},  //TV = 3.06(3041 lines)  AV=2.97  SV=6.33  BV=-0.30
    {119992,4480,1032, 0, 0, 0},  //TV = 3.06(3041 lines)  AV=2.97  SV=6.43  BV=-0.40
    {119992,4736,1048, 0, 0, 0},  //TV = 3.06(3041 lines)  AV=2.97  SV=6.53  BV=-0.50
    {119992,5120,1040, 0, 0, 0},  //TV = 3.06(3041 lines)  AV=2.97  SV=6.63  BV=-0.60
    {119992,5504,1040, 0, 0, 0},  //TV = 3.06(3041 lines)  AV=2.97  SV=6.74  BV=-0.71
    {119992,5888,1040, 0, 0, 0},  //TV = 3.06(3041 lines)  AV=2.97  SV=6.83  BV=-0.80
    {119992,6144,1064, 0, 0, 0},  //TV = 3.06(3041 lines)  AV=2.97  SV=6.93  BV=-0.90
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
},
};

static strAETable g_AE_Video1AutoTable =
{
    AETABLE_VIDEO1_AUTO,    //eAETableID
    119,    //u4TotalIndex
    20,    //u4StrobeTrigerBV
    109,    //i4MaxBV
    -9,    //i4MinBV
    90,    //i4EffectiveMaxBV
    0,      //i4EffectiveMinBV
    LIB3A_AE_ISO_SPEED_AUTO, //ISO SPEED
    sVideo1PLineTable_60Hz,
    sVideo1PLineTable_50Hz,
    NULL,
};

static strEvPline sVideo2PLineTable_60Hz =
{
{
    {198,1140,1024, 0, 0, 0},  //TV = 12.30(5 lines)  AV=2.97  SV=4.44  BV=10.83
    {198,1140,1040, 0, 0, 0},  //TV = 12.30(5 lines)  AV=2.97  SV=4.46  BV=10.81
    {198,1216,1048, 0, 0, 0},  //TV = 12.30(5 lines)  AV=2.97  SV=4.57  BV=10.70
    {198,1328,1024, 0, 0, 0},  //TV = 12.30(5 lines)  AV=2.97  SV=4.66  BV=10.61
    {237,1140,1064, 0, 0, 0},  //TV = 12.04(6 lines)  AV=2.97  SV=4.50  BV=10.52
    {237,1216,1072, 0, 0, 0},  //TV = 12.04(6 lines)  AV=2.97  SV=4.60  BV=10.41
    {277,1140,1056, 0, 0, 0},  //TV = 11.82(7 lines)  AV=2.97  SV=4.49  BV=10.30
    {277,1216,1056, 0, 0, 0},  //TV = 11.82(7 lines)  AV=2.97  SV=4.58  BV=10.21
    {316,1140,1056, 0, 0, 0},  //TV = 11.63(8 lines)  AV=2.97  SV=4.49  BV=10.11
    {316,1216,1064, 0, 0, 0},  //TV = 11.63(8 lines)  AV=2.97  SV=4.59  BV=10.01
    {356,1140,1080, 0, 0, 0},  //TV = 11.46(9 lines)  AV=2.97  SV=4.52  BV=9.91
    {395,1140,1040, 0, 0, 0},  //TV = 11.31(10 lines)  AV=2.97  SV=4.46  BV=9.81
    {435,1140,1024, 0, 0, 0},  //TV = 11.17(11 lines)  AV=2.97  SV=4.44  BV=9.70
    {435,1140,1088, 0, 0, 0},  //TV = 11.17(11 lines)  AV=2.97  SV=4.53  BV=9.61
    {474,1140,1064, 0, 0, 0},  //TV = 11.04(12 lines)  AV=2.97  SV=4.50  BV=9.52
    {513,1140,1056, 0, 0, 0},  //TV = 10.93(13 lines)  AV=2.97  SV=4.49  BV=9.41
    {553,1140,1056, 0, 0, 0},  //TV = 10.82(14 lines)  AV=2.97  SV=4.49  BV=9.31
    {592,1140,1056, 0, 0, 0},  //TV = 10.72(15 lines)  AV=2.97  SV=4.49  BV=9.21
    {632,1140,1056, 0, 0, 0},  //TV = 10.63(16 lines)  AV=2.97  SV=4.49  BV=9.11
    {671,1140,1064, 0, 0, 0},  //TV = 10.54(17 lines)  AV=2.97  SV=4.50  BV=9.02
    {750,1140,1024, 0, 0, 0},  //TV = 10.38(19 lines)  AV=2.97  SV=4.44  BV=8.91
    {790,1140,1040, 0, 0, 0},  //TV = 10.31(20 lines)  AV=2.97  SV=4.46  BV=8.81
    {869,1140,1024, 0, 0, 0},  //TV = 10.17(22 lines)  AV=2.97  SV=4.44  BV=8.70
    {908,1140,1040, 0, 0, 0},  //TV = 10.11(23 lines)  AV=2.97  SV=4.46  BV=8.61
    {987,1140,1024, 0, 0, 0},  //TV = 9.98(25 lines)  AV=2.97  SV=4.44  BV=8.51
    {1066,1140,1024, 0, 0, 0},  //TV = 9.87(27 lines)  AV=2.97  SV=4.44  BV=8.40
    {1105,1140,1056, 0, 0, 0},  //TV = 9.82(28 lines)  AV=2.97  SV=4.49  BV=8.31
    {1224,1140,1024, 0, 0, 0},  //TV = 9.67(31 lines)  AV=2.97  SV=4.44  BV=8.20
    {1303,1140,1024, 0, 0, 0},  //TV = 9.58(33 lines)  AV=2.97  SV=4.44  BV=8.11
    {1382,1140,1040, 0, 0, 0},  //TV = 9.50(35 lines)  AV=2.97  SV=4.46  BV=8.01
    {1500,1140,1024, 0, 0, 0},  //TV = 9.38(38 lines)  AV=2.97  SV=4.44  BV=7.91
    {1618,1140,1024, 0, 0, 0},  //TV = 9.27(41 lines)  AV=2.97  SV=4.44  BV=7.80
    {1697,1140,1040, 0, 0, 0},  //TV = 9.20(43 lines)  AV=2.97  SV=4.46  BV=7.71
    {1855,1140,1024, 0, 0, 0},  //TV = 9.07(47 lines)  AV=2.97  SV=4.44  BV=7.60
    {1973,1140,1024, 0, 0, 0},  //TV = 8.99(50 lines)  AV=2.97  SV=4.44  BV=7.51
    {2131,1140,1024, 0, 0, 0},  //TV = 8.87(54 lines)  AV=2.97  SV=4.44  BV=7.40
    {2250,1140,1032, 0, 0, 0},  //TV = 8.80(57 lines)  AV=2.97  SV=4.45  BV=7.31
    {2447,1140,1024, 0, 0, 0},  //TV = 8.67(62 lines)  AV=2.97  SV=4.44  BV=7.20
    {2605,1140,1024, 0, 0, 0},  //TV = 8.58(66 lines)  AV=2.97  SV=4.44  BV=7.11
    {2802,1140,1024, 0, 0, 0},  //TV = 8.48(71 lines)  AV=2.97  SV=4.44  BV=7.01
    {2999,1140,1024, 0, 0, 0},  //TV = 8.38(76 lines)  AV=2.97  SV=4.44  BV=6.91
    {3236,1140,1024, 0, 0, 0},  //TV = 8.27(82 lines)  AV=2.97  SV=4.44  BV=6.80
    {3473,1140,1024, 0, 0, 0},  //TV = 8.17(88 lines)  AV=2.97  SV=4.44  BV=6.70
    {3710,1140,1024, 0, 0, 0},  //TV = 8.07(94 lines)  AV=2.97  SV=4.44  BV=6.60
    {3986,1140,1024, 0, 0, 0},  //TV = 7.97(101 lines)  AV=2.97  SV=4.44  BV=6.50
    {4262,1140,1024, 0, 0, 0},  //TV = 7.87(108 lines)  AV=2.97  SV=4.44  BV=6.40
    {4538,1140,1024, 0, 0, 0},  //TV = 7.78(115 lines)  AV=2.97  SV=4.44  BV=6.31
    {4893,1140,1024, 0, 0, 0},  //TV = 7.68(124 lines)  AV=2.97  SV=4.44  BV=6.20
    {5209,1140,1024, 0, 0, 0},  //TV = 7.58(132 lines)  AV=2.97  SV=4.44  BV=6.11
    {5643,1140,1024, 0, 0, 0},  //TV = 7.47(143 lines)  AV=2.97  SV=4.44  BV=6.00
    {6038,1140,1024, 0, 0, 0},  //TV = 7.37(153 lines)  AV=2.97  SV=4.44  BV=5.90
    {6472,1140,1024, 0, 0, 0},  //TV = 7.27(164 lines)  AV=2.97  SV=4.44  BV=5.80
    {6906,1140,1024, 0, 0, 0},  //TV = 7.18(175 lines)  AV=2.97  SV=4.44  BV=5.71
    {7419,1140,1024, 0, 0, 0},  //TV = 7.07(188 lines)  AV=2.97  SV=4.44  BV=5.60
    {7971,1140,1024, 0, 0, 0},  //TV = 6.97(202 lines)  AV=2.97  SV=4.44  BV=5.50
    {8326,1140,1048, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=4.48  BV=5.40
    {8326,1216,1048, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=4.57  BV=5.31
    {8326,1328,1024, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=4.66  BV=5.22
    {8326,1424,1024, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=4.76  BV=5.12
    {8326,1536,1024, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=4.87  BV=5.01
    {8326,1632,1032, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=4.97  BV=4.91
    {8326,1728,1040, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=5.06  BV=4.81
    {8326,1840,1048, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=5.17  BV=4.71
    {8326,1936,1072, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=5.27  BV=4.61
    {8326,2144,1032, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=5.36  BV=4.51
    {16652,1140,1048, 0, 0, 0},  //TV = 5.91(422 lines)  AV=2.97  SV=4.48  BV=4.40
    {16652,1216,1048, 0, 0, 0},  //TV = 5.91(422 lines)  AV=2.97  SV=4.57  BV=4.31
    {16652,1328,1024, 0, 0, 0},  //TV = 5.91(422 lines)  AV=2.97  SV=4.66  BV=4.22
    {16652,1424,1024, 0, 0, 0},  //TV = 5.91(422 lines)  AV=2.97  SV=4.76  BV=4.12
    {16652,1424,1104, 0, 0, 0},  //TV = 5.91(422 lines)  AV=2.97  SV=4.87  BV=4.01
    {16652,1632,1032, 0, 0, 0},  //TV = 5.91(422 lines)  AV=2.97  SV=4.97  BV=3.91
    {25017,1140,1056, 0, 0, 0},  //TV = 5.32(634 lines)  AV=2.97  SV=4.49  BV=3.81
    {25017,1216,1064, 0, 0, 0},  //TV = 5.32(634 lines)  AV=2.97  SV=4.59  BV=3.70
    {25017,1328,1048, 0, 0, 0},  //TV = 5.32(634 lines)  AV=2.97  SV=4.70  BV=3.60
    {25017,1424,1040, 0, 0, 0},  //TV = 5.32(634 lines)  AV=2.97  SV=4.78  BV=3.51
    {33185,1140,1048, 0, 0, 0},  //TV = 4.91(841 lines)  AV=2.97  SV=4.48  BV=3.41
    {33185,1216,1056, 0, 0, 0},  //TV = 4.91(841 lines)  AV=2.97  SV=4.58  BV=3.30
    {33185,1328,1032, 0, 0, 0},  //TV = 4.91(841 lines)  AV=2.97  SV=4.67  BV=3.21
    {41669,1140,1024, 0, 0, 0},  //TV = 4.58(1056 lines)  AV=2.97  SV=4.44  BV=3.11
    {41669,1216,1032, 0, 0, 0},  //TV = 4.58(1056 lines)  AV=2.97  SV=4.55  BV=3.01
    {41669,1216,1104, 0, 0, 0},  //TV = 4.58(1056 lines)  AV=2.97  SV=4.64  BV=2.91
    {49994,1140,1056, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.49  BV=2.81
    {49994,1216,1064, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.59  BV=2.70
    {49994,1328,1040, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.68  BV=2.61
    {49994,1424,1040, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.78  BV=2.51
    {49994,1536,1032, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.88  BV=2.41
    {49994,1632,1040, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.98  BV=2.31
    {49994,1728,1056, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.09  BV=2.21
    {49994,1840,1064, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.19  BV=2.11
    {49994,2048,1024, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.29  BV=2.01
    {49994,2144,1048, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.39  BV=1.91
    {49994,2240,1072, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.48  BV=1.81
    {49994,2448,1048, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.58  BV=1.72
    {49994,2656,1040, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.68  BV=1.61
    {49994,2864,1032, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.78  BV=1.51
    {49994,3072,1032, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.88  BV=1.41
    {49994,3328,1024, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.99  BV=1.31
    {49994,3456,1056, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=6.09  BV=1.21
    {58319,3200,1048, 0, 0, 0},  //TV = 4.10(1478 lines)  AV=2.97  SV=5.96  BV=1.11
    {66527,3072,1024, 0, 0, 0},  //TV = 3.91(1686 lines)  AV=2.97  SV=5.87  BV=1.01
    {66527,3200,1056, 0, 0, 0},  //TV = 3.91(1686 lines)  AV=2.97  SV=5.98  BV=0.91
    {75011,3072,1048, 0, 0, 0},  //TV = 3.74(1901 lines)  AV=2.97  SV=5.91  BV=0.80
    {75011,3328,1032, 0, 0, 0},  //TV = 3.74(1901 lines)  AV=2.97  SV=6.00  BV=0.71
    {83336,3200,1032, 0, 0, 0},  //TV = 3.58(2112 lines)  AV=2.97  SV=5.94  BV=0.61
    {83336,3456,1024, 0, 0, 0},  //TV = 3.58(2112 lines)  AV=2.97  SV=6.04  BV=0.51
    {83336,3712,1024, 0, 0, 0},  //TV = 3.58(2112 lines)  AV=2.97  SV=6.14  BV=0.41
    {83336,3968,1032, 0, 0, 0},  //TV = 3.58(2112 lines)  AV=2.97  SV=6.25  BV=0.30
    {83336,4224,1032, 0, 0, 0},  //TV = 3.58(2112 lines)  AV=2.97  SV=6.34  BV=0.21
    {91661,4096,1040, 0, 0, 0},  //TV = 3.45(2323 lines)  AV=2.97  SV=6.31  BV=0.11
    {99987,4096,1032, 0, 0, 0},  //TV = 3.32(2534 lines)  AV=2.97  SV=6.30  BV=-0.01
    {108313,4096,1024, 0, 0, 0},  //TV = 3.21(2745 lines)  AV=2.97  SV=6.29  BV=-0.11
    {108313,4352,1032, 0, 0, 0},  //TV = 3.21(2745 lines)  AV=2.97  SV=6.39  BV=-0.21
    {116678,4224,1048, 0, 0, 0},  //TV = 3.10(2957 lines)  AV=2.97  SV=6.36  BV=-0.29
    {125004,4224,1048, 0, 0, 0},  //TV = 3.00(3168 lines)  AV=2.97  SV=6.36  BV=-0.39
    {125004,4608,1032, 0, 0, 0},  //TV = 3.00(3168 lines)  AV=2.97  SV=6.47  BV=-0.50
    {125004,4864,1048, 0, 0, 0},  //TV = 3.00(3168 lines)  AV=2.97  SV=6.57  BV=-0.60
    {125004,5376,1024, 0, 0, 0},  //TV = 3.00(3168 lines)  AV=2.97  SV=6.68  BV=-0.71
    {125004,5632,1040, 0, 0, 0},  //TV = 3.00(3168 lines)  AV=2.97  SV=6.77  BV=-0.80
    {125004,6016,1040, 0, 0, 0},  //TV = 3.00(3168 lines)  AV=2.97  SV=6.86  BV=-0.89
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
},
};

static strEvPline sVideo2PLineTable_50Hz =
{
{
    {198,1140,1024, 0, 0, 0},  //TV = 12.30(5 lines)  AV=2.97  SV=4.44  BV=10.83
    {198,1140,1040, 0, 0, 0},  //TV = 12.30(5 lines)  AV=2.97  SV=4.46  BV=10.81
    {198,1216,1048, 0, 0, 0},  //TV = 12.30(5 lines)  AV=2.97  SV=4.57  BV=10.70
    {198,1328,1024, 0, 0, 0},  //TV = 12.30(5 lines)  AV=2.97  SV=4.66  BV=10.61
    {237,1140,1064, 0, 0, 0},  //TV = 12.04(6 lines)  AV=2.97  SV=4.50  BV=10.52
    {237,1216,1072, 0, 0, 0},  //TV = 12.04(6 lines)  AV=2.97  SV=4.60  BV=10.41
    {277,1140,1056, 0, 0, 0},  //TV = 11.82(7 lines)  AV=2.97  SV=4.49  BV=10.30
    {277,1216,1056, 0, 0, 0},  //TV = 11.82(7 lines)  AV=2.97  SV=4.58  BV=10.21
    {316,1140,1056, 0, 0, 0},  //TV = 11.63(8 lines)  AV=2.97  SV=4.49  BV=10.11
    {316,1216,1064, 0, 0, 0},  //TV = 11.63(8 lines)  AV=2.97  SV=4.59  BV=10.01
    {356,1140,1080, 0, 0, 0},  //TV = 11.46(9 lines)  AV=2.97  SV=4.52  BV=9.91
    {395,1140,1040, 0, 0, 0},  //TV = 11.31(10 lines)  AV=2.97  SV=4.46  BV=9.81
    {435,1140,1024, 0, 0, 0},  //TV = 11.17(11 lines)  AV=2.97  SV=4.44  BV=9.70
    {435,1140,1088, 0, 0, 0},  //TV = 11.17(11 lines)  AV=2.97  SV=4.53  BV=9.61
    {474,1140,1064, 0, 0, 0},  //TV = 11.04(12 lines)  AV=2.97  SV=4.50  BV=9.52
    {513,1140,1056, 0, 0, 0},  //TV = 10.93(13 lines)  AV=2.97  SV=4.49  BV=9.41
    {553,1140,1056, 0, 0, 0},  //TV = 10.82(14 lines)  AV=2.97  SV=4.49  BV=9.31
    {592,1140,1056, 0, 0, 0},  //TV = 10.72(15 lines)  AV=2.97  SV=4.49  BV=9.21
    {632,1140,1056, 0, 0, 0},  //TV = 10.63(16 lines)  AV=2.97  SV=4.49  BV=9.11
    {671,1140,1064, 0, 0, 0},  //TV = 10.54(17 lines)  AV=2.97  SV=4.50  BV=9.02
    {750,1140,1024, 0, 0, 0},  //TV = 10.38(19 lines)  AV=2.97  SV=4.44  BV=8.91
    {790,1140,1040, 0, 0, 0},  //TV = 10.31(20 lines)  AV=2.97  SV=4.46  BV=8.81
    {869,1140,1024, 0, 0, 0},  //TV = 10.17(22 lines)  AV=2.97  SV=4.44  BV=8.70
    {908,1140,1040, 0, 0, 0},  //TV = 10.11(23 lines)  AV=2.97  SV=4.46  BV=8.61
    {987,1140,1024, 0, 0, 0},  //TV = 9.98(25 lines)  AV=2.97  SV=4.44  BV=8.51
    {1066,1140,1024, 0, 0, 0},  //TV = 9.87(27 lines)  AV=2.97  SV=4.44  BV=8.40
    {1105,1140,1056, 0, 0, 0},  //TV = 9.82(28 lines)  AV=2.97  SV=4.49  BV=8.31
    {1224,1140,1024, 0, 0, 0},  //TV = 9.67(31 lines)  AV=2.97  SV=4.44  BV=8.20
    {1303,1140,1024, 0, 0, 0},  //TV = 9.58(33 lines)  AV=2.97  SV=4.44  BV=8.11
    {1382,1140,1040, 0, 0, 0},  //TV = 9.50(35 lines)  AV=2.97  SV=4.46  BV=8.01
    {1500,1140,1024, 0, 0, 0},  //TV = 9.38(38 lines)  AV=2.97  SV=4.44  BV=7.91
    {1618,1140,1024, 0, 0, 0},  //TV = 9.27(41 lines)  AV=2.97  SV=4.44  BV=7.80
    {1697,1140,1040, 0, 0, 0},  //TV = 9.20(43 lines)  AV=2.97  SV=4.46  BV=7.71
    {1855,1140,1024, 0, 0, 0},  //TV = 9.07(47 lines)  AV=2.97  SV=4.44  BV=7.60
    {1973,1140,1024, 0, 0, 0},  //TV = 8.99(50 lines)  AV=2.97  SV=4.44  BV=7.51
    {2131,1140,1024, 0, 0, 0},  //TV = 8.87(54 lines)  AV=2.97  SV=4.44  BV=7.40
    {2250,1140,1032, 0, 0, 0},  //TV = 8.80(57 lines)  AV=2.97  SV=4.45  BV=7.31
    {2447,1140,1024, 0, 0, 0},  //TV = 8.67(62 lines)  AV=2.97  SV=4.44  BV=7.20
    {2605,1140,1024, 0, 0, 0},  //TV = 8.58(66 lines)  AV=2.97  SV=4.44  BV=7.11
    {2802,1140,1024, 0, 0, 0},  //TV = 8.48(71 lines)  AV=2.97  SV=4.44  BV=7.01
    {2999,1140,1024, 0, 0, 0},  //TV = 8.38(76 lines)  AV=2.97  SV=4.44  BV=6.91
    {3236,1140,1024, 0, 0, 0},  //TV = 8.27(82 lines)  AV=2.97  SV=4.44  BV=6.80
    {3473,1140,1024, 0, 0, 0},  //TV = 8.17(88 lines)  AV=2.97  SV=4.44  BV=6.70
    {3710,1140,1024, 0, 0, 0},  //TV = 8.07(94 lines)  AV=2.97  SV=4.44  BV=6.60
    {3986,1140,1024, 0, 0, 0},  //TV = 7.97(101 lines)  AV=2.97  SV=4.44  BV=6.50
    {4262,1140,1024, 0, 0, 0},  //TV = 7.87(108 lines)  AV=2.97  SV=4.44  BV=6.40
    {4538,1140,1024, 0, 0, 0},  //TV = 7.78(115 lines)  AV=2.97  SV=4.44  BV=6.31
    {4893,1140,1024, 0, 0, 0},  //TV = 7.68(124 lines)  AV=2.97  SV=4.44  BV=6.20
    {5209,1140,1024, 0, 0, 0},  //TV = 7.58(132 lines)  AV=2.97  SV=4.44  BV=6.11
    {5643,1140,1024, 0, 0, 0},  //TV = 7.47(143 lines)  AV=2.97  SV=4.44  BV=6.00
    {6038,1140,1024, 0, 0, 0},  //TV = 7.37(153 lines)  AV=2.97  SV=4.44  BV=5.90
    {6472,1140,1024, 0, 0, 0},  //TV = 7.27(164 lines)  AV=2.97  SV=4.44  BV=5.80
    {6906,1140,1024, 0, 0, 0},  //TV = 7.18(175 lines)  AV=2.97  SV=4.44  BV=5.71
    {7419,1140,1024, 0, 0, 0},  //TV = 7.07(188 lines)  AV=2.97  SV=4.44  BV=5.60
    {7971,1140,1024, 0, 0, 0},  //TV = 6.97(202 lines)  AV=2.97  SV=4.44  BV=5.50
    {8484,1140,1024, 0, 0, 0},  //TV = 6.88(215 lines)  AV=2.97  SV=4.44  BV=5.41
    {9155,1140,1024, 0, 0, 0},  //TV = 6.77(232 lines)  AV=2.97  SV=4.44  BV=5.30
    {9826,1140,1024, 0, 0, 0},  //TV = 6.67(249 lines)  AV=2.97  SV=4.44  BV=5.20
    {9983,1140,1072, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=4.51  BV=5.11
    {9983,1216,1072, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=4.60  BV=5.02
    {9983,1328,1056, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=4.71  BV=4.91
    {9983,1424,1056, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=4.81  BV=4.81
    {9983,1536,1048, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=4.91  BV=4.71
    {9983,1632,1064, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=5.01  BV=4.60
    {9983,1728,1072, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=5.11  BV=4.51
    {9983,1936,1024, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=5.21  BV=4.41
    {9983,2048,1040, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=5.31  BV=4.31
    {9983,2144,1064, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=5.41  BV=4.21
    {20006,1140,1064, 0, 0, 0},  //TV = 5.64(507 lines)  AV=2.97  SV=4.50  BV=4.12
    {20006,1216,1072, 0, 0, 0},  //TV = 5.64(507 lines)  AV=2.97  SV=4.60  BV=4.01
    {20006,1328,1048, 0, 0, 0},  //TV = 5.64(507 lines)  AV=2.97  SV=4.70  BV=3.92
    {20006,1424,1064, 0, 0, 0},  //TV = 5.64(507 lines)  AV=2.97  SV=4.82  BV=3.80
    {20006,1536,1048, 0, 0, 0},  //TV = 5.64(507 lines)  AV=2.97  SV=4.91  BV=3.71
    {20006,1632,1056, 0, 0, 0},  //TV = 5.64(507 lines)  AV=2.97  SV=5.00  BV=3.61
    {29989,1140,1080, 0, 0, 0},  //TV = 5.06(760 lines)  AV=2.97  SV=4.52  BV=3.51
    {29989,1216,1096, 0, 0, 0},  //TV = 5.06(760 lines)  AV=2.97  SV=4.63  BV=3.40
    {29989,1328,1064, 0, 0, 0},  //TV = 5.06(760 lines)  AV=2.97  SV=4.72  BV=3.31
    {29989,1424,1072, 0, 0, 0},  //TV = 5.06(760 lines)  AV=2.97  SV=4.83  BV=3.20
    {40011,1140,1064, 0, 0, 0},  //TV = 4.64(1014 lines)  AV=2.97  SV=4.50  BV=3.12
    {40011,1216,1072, 0, 0, 0},  //TV = 4.64(1014 lines)  AV=2.97  SV=4.60  BV=3.01
    {40011,1328,1048, 0, 0, 0},  //TV = 4.64(1014 lines)  AV=2.97  SV=4.70  BV=2.92
    {49994,1140,1056, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.49  BV=2.81
    {49994,1216,1064, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.59  BV=2.70
    {49994,1328,1040, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.68  BV=2.61
    {49994,1424,1040, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.78  BV=2.51
    {49994,1536,1032, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.88  BV=2.41
    {49994,1632,1040, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.98  BV=2.31
    {49994,1728,1056, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.09  BV=2.21
    {49994,1840,1064, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.19  BV=2.11
    {49994,2048,1024, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.29  BV=2.01
    {49994,2144,1048, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.39  BV=1.91
    {49994,2240,1072, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.48  BV=1.81
    {49994,2448,1048, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.58  BV=1.72
    {49994,2656,1040, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.68  BV=1.61
    {49994,2864,1032, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.78  BV=1.51
    {49994,3072,1032, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.88  BV=1.41
    {49994,3328,1024, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.99  BV=1.31
    {49994,3456,1056, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=6.09  BV=1.21
    {60017,3072,1064, 0, 0, 0},  //TV = 4.06(1521 lines)  AV=2.97  SV=5.93  BV=1.10
    {60017,3328,1048, 0, 0, 0},  //TV = 4.06(1521 lines)  AV=2.97  SV=6.02  BV=1.01
    {69999,3072,1048, 0, 0, 0},  //TV = 3.84(1774 lines)  AV=2.97  SV=5.91  BV=0.90
    {69999,3328,1032, 0, 0, 0},  //TV = 3.84(1774 lines)  AV=2.97  SV=6.00  BV=0.81
    {79982,3072,1048, 0, 0, 0},  //TV = 3.64(2027 lines)  AV=2.97  SV=5.91  BV=0.71
    {79982,3328,1032, 0, 0, 0},  //TV = 3.64(2027 lines)  AV=2.97  SV=6.00  BV=0.62
    {79982,3584,1032, 0, 0, 0},  //TV = 3.64(2027 lines)  AV=2.97  SV=6.11  BV=0.51
    {79982,3840,1032, 0, 0, 0},  //TV = 3.64(2027 lines)  AV=2.97  SV=6.20  BV=0.41
    {79982,4096,1040, 0, 0, 0},  //TV = 3.64(2027 lines)  AV=2.97  SV=6.31  BV=0.31
    {79982,4352,1040, 0, 0, 0},  //TV = 3.64(2027 lines)  AV=2.97  SV=6.40  BV=0.22
    {90005,4224,1024, 0, 0, 0},  //TV = 3.47(2281 lines)  AV=2.97  SV=6.33  BV=0.11
    {99987,4096,1032, 0, 0, 0},  //TV = 3.32(2534 lines)  AV=2.97  SV=6.30  BV=-0.01
    {99987,4352,1040, 0, 0, 0},  //TV = 3.32(2534 lines)  AV=2.97  SV=6.40  BV=-0.10
    {110010,4224,1040, 0, 0, 0},  //TV = 3.18(2788 lines)  AV=2.97  SV=6.35  BV=-0.20
    {119992,4096,1056, 0, 0, 0},  //TV = 3.06(3041 lines)  AV=2.97  SV=6.33  BV=-0.30
    {119992,4480,1032, 0, 0, 0},  //TV = 3.06(3041 lines)  AV=2.97  SV=6.43  BV=-0.40
    {119992,4736,1048, 0, 0, 0},  //TV = 3.06(3041 lines)  AV=2.97  SV=6.53  BV=-0.50
    {119992,5120,1040, 0, 0, 0},  //TV = 3.06(3041 lines)  AV=2.97  SV=6.63  BV=-0.60
    {119992,5504,1040, 0, 0, 0},  //TV = 3.06(3041 lines)  AV=2.97  SV=6.74  BV=-0.71
    {119992,5888,1040, 0, 0, 0},  //TV = 3.06(3041 lines)  AV=2.97  SV=6.83  BV=-0.80
    {119992,6144,1064, 0, 0, 0},  //TV = 3.06(3041 lines)  AV=2.97  SV=6.93  BV=-0.90
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
},
};

static strAETable g_AE_Video2AutoTable =
{
    AETABLE_VIDEO2_AUTO,    //eAETableID
    119,    //u4TotalIndex
    20,    //u4StrobeTrigerBV
    109,    //i4MaxBV
    -9,    //i4MinBV
    90,    //i4EffectiveMaxBV
    0,      //i4EffectiveMinBV
    LIB3A_AE_ISO_SPEED_AUTO, //ISO SPEED
    sVideo2PLineTable_60Hz,
    sVideo2PLineTable_50Hz,
    NULL,
};

static strEvPline sCustom1PLineTable_60Hz =
{
{
    {198,1140,1024, 0, 0, 0},  //TV = 12.30(5 lines)  AV=2.97  SV=4.44  BV=10.83
    {198,1140,1040, 0, 0, 0},  //TV = 12.30(5 lines)  AV=2.97  SV=4.46  BV=10.81
    {198,1216,1048, 0, 0, 0},  //TV = 12.30(5 lines)  AV=2.97  SV=4.57  BV=10.70
    {198,1328,1024, 0, 0, 0},  //TV = 12.30(5 lines)  AV=2.97  SV=4.66  BV=10.61
    {237,1140,1064, 0, 0, 0},  //TV = 12.04(6 lines)  AV=2.97  SV=4.50  BV=10.52
    {237,1216,1072, 0, 0, 0},  //TV = 12.04(6 lines)  AV=2.97  SV=4.60  BV=10.41
    {277,1140,1056, 0, 0, 0},  //TV = 11.82(7 lines)  AV=2.97  SV=4.49  BV=10.30
    {277,1216,1056, 0, 0, 0},  //TV = 11.82(7 lines)  AV=2.97  SV=4.58  BV=10.21
    {316,1140,1056, 0, 0, 0},  //TV = 11.63(8 lines)  AV=2.97  SV=4.49  BV=10.11
    {316,1216,1064, 0, 0, 0},  //TV = 11.63(8 lines)  AV=2.97  SV=4.59  BV=10.01
    {356,1140,1080, 0, 0, 0},  //TV = 11.46(9 lines)  AV=2.97  SV=4.52  BV=9.91
    {395,1140,1040, 0, 0, 0},  //TV = 11.31(10 lines)  AV=2.97  SV=4.46  BV=9.81
    {435,1140,1024, 0, 0, 0},  //TV = 11.17(11 lines)  AV=2.97  SV=4.44  BV=9.70
    {435,1140,1088, 0, 0, 0},  //TV = 11.17(11 lines)  AV=2.97  SV=4.53  BV=9.61
    {474,1140,1064, 0, 0, 0},  //TV = 11.04(12 lines)  AV=2.97  SV=4.50  BV=9.52
    {513,1140,1056, 0, 0, 0},  //TV = 10.93(13 lines)  AV=2.97  SV=4.49  BV=9.41
    {553,1140,1056, 0, 0, 0},  //TV = 10.82(14 lines)  AV=2.97  SV=4.49  BV=9.31
    {592,1140,1056, 0, 0, 0},  //TV = 10.72(15 lines)  AV=2.97  SV=4.49  BV=9.21
    {632,1140,1056, 0, 0, 0},  //TV = 10.63(16 lines)  AV=2.97  SV=4.49  BV=9.11
    {671,1140,1064, 0, 0, 0},  //TV = 10.54(17 lines)  AV=2.97  SV=4.50  BV=9.02
    {750,1140,1024, 0, 0, 0},  //TV = 10.38(19 lines)  AV=2.97  SV=4.44  BV=8.91
    {790,1140,1040, 0, 0, 0},  //TV = 10.31(20 lines)  AV=2.97  SV=4.46  BV=8.81
    {869,1140,1024, 0, 0, 0},  //TV = 10.17(22 lines)  AV=2.97  SV=4.44  BV=8.70
    {908,1140,1040, 0, 0, 0},  //TV = 10.11(23 lines)  AV=2.97  SV=4.46  BV=8.61
    {987,1140,1024, 0, 0, 0},  //TV = 9.98(25 lines)  AV=2.97  SV=4.44  BV=8.51
    {1066,1140,1024, 0, 0, 0},  //TV = 9.87(27 lines)  AV=2.97  SV=4.44  BV=8.40
    {1105,1140,1056, 0, 0, 0},  //TV = 9.82(28 lines)  AV=2.97  SV=4.49  BV=8.31
    {1224,1140,1024, 0, 0, 0},  //TV = 9.67(31 lines)  AV=2.97  SV=4.44  BV=8.20
    {1303,1140,1024, 0, 0, 0},  //TV = 9.58(33 lines)  AV=2.97  SV=4.44  BV=8.11
    {1382,1140,1040, 0, 0, 0},  //TV = 9.50(35 lines)  AV=2.97  SV=4.46  BV=8.01
    {1500,1140,1024, 0, 0, 0},  //TV = 9.38(38 lines)  AV=2.97  SV=4.44  BV=7.91
    {1618,1140,1024, 0, 0, 0},  //TV = 9.27(41 lines)  AV=2.97  SV=4.44  BV=7.80
    {1697,1140,1040, 0, 0, 0},  //TV = 9.20(43 lines)  AV=2.97  SV=4.46  BV=7.71
    {1855,1140,1024, 0, 0, 0},  //TV = 9.07(47 lines)  AV=2.97  SV=4.44  BV=7.60
    {1973,1140,1024, 0, 0, 0},  //TV = 8.99(50 lines)  AV=2.97  SV=4.44  BV=7.51
    {2131,1140,1024, 0, 0, 0},  //TV = 8.87(54 lines)  AV=2.97  SV=4.44  BV=7.40
    {2250,1140,1032, 0, 0, 0},  //TV = 8.80(57 lines)  AV=2.97  SV=4.45  BV=7.31
    {2447,1140,1024, 0, 0, 0},  //TV = 8.67(62 lines)  AV=2.97  SV=4.44  BV=7.20
    {2605,1140,1024, 0, 0, 0},  //TV = 8.58(66 lines)  AV=2.97  SV=4.44  BV=7.11
    {2802,1140,1024, 0, 0, 0},  //TV = 8.48(71 lines)  AV=2.97  SV=4.44  BV=7.01
    {2999,1140,1024, 0, 0, 0},  //TV = 8.38(76 lines)  AV=2.97  SV=4.44  BV=6.91
    {3236,1140,1024, 0, 0, 0},  //TV = 8.27(82 lines)  AV=2.97  SV=4.44  BV=6.80
    {3473,1140,1024, 0, 0, 0},  //TV = 8.17(88 lines)  AV=2.97  SV=4.44  BV=6.70
    {3710,1140,1024, 0, 0, 0},  //TV = 8.07(94 lines)  AV=2.97  SV=4.44  BV=6.60
    {3986,1140,1024, 0, 0, 0},  //TV = 7.97(101 lines)  AV=2.97  SV=4.44  BV=6.50
    {4262,1140,1024, 0, 0, 0},  //TV = 7.87(108 lines)  AV=2.97  SV=4.44  BV=6.40
    {4538,1140,1024, 0, 0, 0},  //TV = 7.78(115 lines)  AV=2.97  SV=4.44  BV=6.31
    {4893,1140,1024, 0, 0, 0},  //TV = 7.68(124 lines)  AV=2.97  SV=4.44  BV=6.20
    {5209,1140,1024, 0, 0, 0},  //TV = 7.58(132 lines)  AV=2.97  SV=4.44  BV=6.11
    {5643,1140,1024, 0, 0, 0},  //TV = 7.47(143 lines)  AV=2.97  SV=4.44  BV=6.00
    {6038,1140,1024, 0, 0, 0},  //TV = 7.37(153 lines)  AV=2.97  SV=4.44  BV=5.90
    {6472,1140,1024, 0, 0, 0},  //TV = 7.27(164 lines)  AV=2.97  SV=4.44  BV=5.80
    {6906,1140,1024, 0, 0, 0},  //TV = 7.18(175 lines)  AV=2.97  SV=4.44  BV=5.71
    {7419,1140,1024, 0, 0, 0},  //TV = 7.07(188 lines)  AV=2.97  SV=4.44  BV=5.60
    {7971,1140,1024, 0, 0, 0},  //TV = 6.97(202 lines)  AV=2.97  SV=4.44  BV=5.50
    {8326,1140,1048, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=4.48  BV=5.40
    {8326,1216,1048, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=4.57  BV=5.31
    {8326,1328,1024, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=4.66  BV=5.22
    {8326,1424,1024, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=4.76  BV=5.12
    {8326,1536,1024, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=4.87  BV=5.01
    {8326,1632,1032, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=4.97  BV=4.91
    {8326,1728,1040, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=5.06  BV=4.81
    {8326,1840,1048, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=5.17  BV=4.71
    {8326,1936,1072, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=5.27  BV=4.61
    {8326,2144,1032, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=5.36  BV=4.51
    {16652,1140,1048, 0, 0, 0},  //TV = 5.91(422 lines)  AV=2.97  SV=4.48  BV=4.40
    {16652,1216,1048, 0, 0, 0},  //TV = 5.91(422 lines)  AV=2.97  SV=4.57  BV=4.31
    {16652,1328,1024, 0, 0, 0},  //TV = 5.91(422 lines)  AV=2.97  SV=4.66  BV=4.22
    {16652,1424,1024, 0, 0, 0},  //TV = 5.91(422 lines)  AV=2.97  SV=4.76  BV=4.12
    {16652,1424,1104, 0, 0, 0},  //TV = 5.91(422 lines)  AV=2.97  SV=4.87  BV=4.01
    {16652,1632,1032, 0, 0, 0},  //TV = 5.91(422 lines)  AV=2.97  SV=4.97  BV=3.91
    {25017,1140,1056, 0, 0, 0},  //TV = 5.32(634 lines)  AV=2.97  SV=4.49  BV=3.81
    {25017,1216,1064, 0, 0, 0},  //TV = 5.32(634 lines)  AV=2.97  SV=4.59  BV=3.70
    {25017,1328,1048, 0, 0, 0},  //TV = 5.32(634 lines)  AV=2.97  SV=4.70  BV=3.60
    {25017,1424,1040, 0, 0, 0},  //TV = 5.32(634 lines)  AV=2.97  SV=4.78  BV=3.51
    {33343,1140,1048, 0, 0, 0},  //TV = 4.91(845 lines)  AV=2.97  SV=4.48  BV=3.40
    {33343,1216,1048, 0, 0, 0},  //TV = 4.91(845 lines)  AV=2.97  SV=4.57  BV=3.31
    {33343,1328,1024, 0, 0, 0},  //TV = 4.91(845 lines)  AV=2.97  SV=4.66  BV=3.22
    {41669,1140,1024, 0, 0, 0},  //TV = 4.58(1056 lines)  AV=2.97  SV=4.44  BV=3.11
    {41669,1216,1032, 0, 0, 0},  //TV = 4.58(1056 lines)  AV=2.97  SV=4.55  BV=3.01
    {41669,1216,1104, 0, 0, 0},  //TV = 4.58(1056 lines)  AV=2.97  SV=4.64  BV=2.91
    {49994,1140,1056, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.49  BV=2.81
    {49994,1216,1064, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.59  BV=2.70
    {49994,1328,1040, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.68  BV=2.61
    {49994,1424,1040, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.78  BV=2.51
    {49994,1536,1032, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.88  BV=2.41
    {49994,1632,1040, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.98  BV=2.31
    {49994,1728,1056, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.09  BV=2.21
    {49994,1840,1064, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.19  BV=2.11
    {49994,2048,1024, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.29  BV=2.01
    {49994,2144,1048, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.39  BV=1.91
    {49994,2240,1072, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.48  BV=1.81
    {49994,2448,1048, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.58  BV=1.72
    {49994,2656,1040, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.68  BV=1.61
    {49994,2864,1032, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.78  BV=1.51
    {49994,3072,1032, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.88  BV=1.41
    {49994,3328,1024, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.99  BV=1.31
    {49994,3456,1056, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=6.09  BV=1.21
    {58319,3200,1048, 0, 0, 0},  //TV = 4.10(1478 lines)  AV=2.97  SV=5.96  BV=1.11
    {66645,3072,1024, 0, 0, 0},  //TV = 3.91(1689 lines)  AV=2.97  SV=5.87  BV=1.01
    {66645,3200,1056, 0, 0, 0},  //TV = 3.91(1689 lines)  AV=2.97  SV=5.98  BV=0.90
    {75011,3072,1048, 0, 0, 0},  //TV = 3.74(1901 lines)  AV=2.97  SV=5.91  BV=0.80
    {75011,3328,1032, 0, 0, 0},  //TV = 3.74(1901 lines)  AV=2.97  SV=6.00  BV=0.71
    {83336,3200,1032, 0, 0, 0},  //TV = 3.58(2112 lines)  AV=2.97  SV=5.94  BV=0.61
    {83336,3456,1024, 0, 0, 0},  //TV = 3.58(2112 lines)  AV=2.97  SV=6.04  BV=0.51
    {83336,3712,1024, 0, 0, 0},  //TV = 3.58(2112 lines)  AV=2.97  SV=6.14  BV=0.41
    {83336,3968,1032, 0, 0, 0},  //TV = 3.58(2112 lines)  AV=2.97  SV=6.25  BV=0.30
    {83336,4224,1032, 0, 0, 0},  //TV = 3.58(2112 lines)  AV=2.97  SV=6.34  BV=0.21
    {91661,4096,1040, 0, 0, 0},  //TV = 3.45(2323 lines)  AV=2.97  SV=6.31  BV=0.11
    {99987,4096,1032, 0, 0, 0},  //TV = 3.32(2534 lines)  AV=2.97  SV=6.30  BV=-0.01
    {108313,4096,1024, 0, 0, 0},  //TV = 3.21(2745 lines)  AV=2.97  SV=6.29  BV=-0.11
    {108313,4352,1032, 0, 0, 0},  //TV = 3.21(2745 lines)  AV=2.97  SV=6.39  BV=-0.21
    {116678,4224,1048, 0, 0, 0},  //TV = 3.10(2957 lines)  AV=2.97  SV=6.36  BV=-0.29
    {125004,4224,1048, 0, 0, 0},  //TV = 3.00(3168 lines)  AV=2.97  SV=6.36  BV=-0.39
    {125004,4608,1032, 0, 0, 0},  //TV = 3.00(3168 lines)  AV=2.97  SV=6.47  BV=-0.50
    {125004,4864,1048, 0, 0, 0},  //TV = 3.00(3168 lines)  AV=2.97  SV=6.57  BV=-0.60
    {125004,5376,1024, 0, 0, 0},  //TV = 3.00(3168 lines)  AV=2.97  SV=6.68  BV=-0.71
    {125004,5632,1040, 0, 0, 0},  //TV = 3.00(3168 lines)  AV=2.97  SV=6.77  BV=-0.80
    {125004,6016,1040, 0, 0, 0},  //TV = 3.00(3168 lines)  AV=2.97  SV=6.86  BV=-0.89
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
},
};

static strEvPline sCustom1PLineTable_50Hz =
{
{
    {198,1140,1024, 0, 0, 0},  //TV = 12.30(5 lines)  AV=2.97  SV=4.44  BV=10.83
    {198,1140,1040, 0, 0, 0},  //TV = 12.30(5 lines)  AV=2.97  SV=4.46  BV=10.81
    {198,1216,1048, 0, 0, 0},  //TV = 12.30(5 lines)  AV=2.97  SV=4.57  BV=10.70
    {198,1328,1024, 0, 0, 0},  //TV = 12.30(5 lines)  AV=2.97  SV=4.66  BV=10.61
    {237,1140,1064, 0, 0, 0},  //TV = 12.04(6 lines)  AV=2.97  SV=4.50  BV=10.52
    {237,1216,1072, 0, 0, 0},  //TV = 12.04(6 lines)  AV=2.97  SV=4.60  BV=10.41
    {277,1140,1056, 0, 0, 0},  //TV = 11.82(7 lines)  AV=2.97  SV=4.49  BV=10.30
    {277,1216,1056, 0, 0, 0},  //TV = 11.82(7 lines)  AV=2.97  SV=4.58  BV=10.21
    {316,1140,1056, 0, 0, 0},  //TV = 11.63(8 lines)  AV=2.97  SV=4.49  BV=10.11
    {316,1216,1064, 0, 0, 0},  //TV = 11.63(8 lines)  AV=2.97  SV=4.59  BV=10.01
    {356,1140,1080, 0, 0, 0},  //TV = 11.46(9 lines)  AV=2.97  SV=4.52  BV=9.91
    {395,1140,1040, 0, 0, 0},  //TV = 11.31(10 lines)  AV=2.97  SV=4.46  BV=9.81
    {435,1140,1024, 0, 0, 0},  //TV = 11.17(11 lines)  AV=2.97  SV=4.44  BV=9.70
    {435,1140,1088, 0, 0, 0},  //TV = 11.17(11 lines)  AV=2.97  SV=4.53  BV=9.61
    {474,1140,1064, 0, 0, 0},  //TV = 11.04(12 lines)  AV=2.97  SV=4.50  BV=9.52
    {513,1140,1056, 0, 0, 0},  //TV = 10.93(13 lines)  AV=2.97  SV=4.49  BV=9.41
    {553,1140,1056, 0, 0, 0},  //TV = 10.82(14 lines)  AV=2.97  SV=4.49  BV=9.31
    {592,1140,1056, 0, 0, 0},  //TV = 10.72(15 lines)  AV=2.97  SV=4.49  BV=9.21
    {632,1140,1056, 0, 0, 0},  //TV = 10.63(16 lines)  AV=2.97  SV=4.49  BV=9.11
    {671,1140,1064, 0, 0, 0},  //TV = 10.54(17 lines)  AV=2.97  SV=4.50  BV=9.02
    {750,1140,1024, 0, 0, 0},  //TV = 10.38(19 lines)  AV=2.97  SV=4.44  BV=8.91
    {790,1140,1040, 0, 0, 0},  //TV = 10.31(20 lines)  AV=2.97  SV=4.46  BV=8.81
    {869,1140,1024, 0, 0, 0},  //TV = 10.17(22 lines)  AV=2.97  SV=4.44  BV=8.70
    {908,1140,1040, 0, 0, 0},  //TV = 10.11(23 lines)  AV=2.97  SV=4.46  BV=8.61
    {987,1140,1024, 0, 0, 0},  //TV = 9.98(25 lines)  AV=2.97  SV=4.44  BV=8.51
    {1066,1140,1024, 0, 0, 0},  //TV = 9.87(27 lines)  AV=2.97  SV=4.44  BV=8.40
    {1105,1140,1056, 0, 0, 0},  //TV = 9.82(28 lines)  AV=2.97  SV=4.49  BV=8.31
    {1224,1140,1024, 0, 0, 0},  //TV = 9.67(31 lines)  AV=2.97  SV=4.44  BV=8.20
    {1303,1140,1024, 0, 0, 0},  //TV = 9.58(33 lines)  AV=2.97  SV=4.44  BV=8.11
    {1382,1140,1040, 0, 0, 0},  //TV = 9.50(35 lines)  AV=2.97  SV=4.46  BV=8.01
    {1500,1140,1024, 0, 0, 0},  //TV = 9.38(38 lines)  AV=2.97  SV=4.44  BV=7.91
    {1618,1140,1024, 0, 0, 0},  //TV = 9.27(41 lines)  AV=2.97  SV=4.44  BV=7.80
    {1697,1140,1040, 0, 0, 0},  //TV = 9.20(43 lines)  AV=2.97  SV=4.46  BV=7.71
    {1855,1140,1024, 0, 0, 0},  //TV = 9.07(47 lines)  AV=2.97  SV=4.44  BV=7.60
    {1973,1140,1024, 0, 0, 0},  //TV = 8.99(50 lines)  AV=2.97  SV=4.44  BV=7.51
    {2131,1140,1024, 0, 0, 0},  //TV = 8.87(54 lines)  AV=2.97  SV=4.44  BV=7.40
    {2250,1140,1032, 0, 0, 0},  //TV = 8.80(57 lines)  AV=2.97  SV=4.45  BV=7.31
    {2447,1140,1024, 0, 0, 0},  //TV = 8.67(62 lines)  AV=2.97  SV=4.44  BV=7.20
    {2605,1140,1024, 0, 0, 0},  //TV = 8.58(66 lines)  AV=2.97  SV=4.44  BV=7.11
    {2802,1140,1024, 0, 0, 0},  //TV = 8.48(71 lines)  AV=2.97  SV=4.44  BV=7.01
    {2999,1140,1024, 0, 0, 0},  //TV = 8.38(76 lines)  AV=2.97  SV=4.44  BV=6.91
    {3236,1140,1024, 0, 0, 0},  //TV = 8.27(82 lines)  AV=2.97  SV=4.44  BV=6.80
    {3473,1140,1024, 0, 0, 0},  //TV = 8.17(88 lines)  AV=2.97  SV=4.44  BV=6.70
    {3710,1140,1024, 0, 0, 0},  //TV = 8.07(94 lines)  AV=2.97  SV=4.44  BV=6.60
    {3986,1140,1024, 0, 0, 0},  //TV = 7.97(101 lines)  AV=2.97  SV=4.44  BV=6.50
    {4262,1140,1024, 0, 0, 0},  //TV = 7.87(108 lines)  AV=2.97  SV=4.44  BV=6.40
    {4538,1140,1024, 0, 0, 0},  //TV = 7.78(115 lines)  AV=2.97  SV=4.44  BV=6.31
    {4893,1140,1024, 0, 0, 0},  //TV = 7.68(124 lines)  AV=2.97  SV=4.44  BV=6.20
    {5209,1140,1024, 0, 0, 0},  //TV = 7.58(132 lines)  AV=2.97  SV=4.44  BV=6.11
    {5643,1140,1024, 0, 0, 0},  //TV = 7.47(143 lines)  AV=2.97  SV=4.44  BV=6.00
    {6038,1140,1024, 0, 0, 0},  //TV = 7.37(153 lines)  AV=2.97  SV=4.44  BV=5.90
    {6472,1140,1024, 0, 0, 0},  //TV = 7.27(164 lines)  AV=2.97  SV=4.44  BV=5.80
    {6906,1140,1024, 0, 0, 0},  //TV = 7.18(175 lines)  AV=2.97  SV=4.44  BV=5.71
    {7419,1140,1024, 0, 0, 0},  //TV = 7.07(188 lines)  AV=2.97  SV=4.44  BV=5.60
    {7971,1140,1024, 0, 0, 0},  //TV = 6.97(202 lines)  AV=2.97  SV=4.44  BV=5.50
    {8484,1140,1024, 0, 0, 0},  //TV = 6.88(215 lines)  AV=2.97  SV=4.44  BV=5.41
    {9155,1140,1024, 0, 0, 0},  //TV = 6.77(232 lines)  AV=2.97  SV=4.44  BV=5.30
    {9826,1140,1024, 0, 0, 0},  //TV = 6.67(249 lines)  AV=2.97  SV=4.44  BV=5.20
    {9983,1140,1072, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=4.51  BV=5.11
    {9983,1216,1072, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=4.60  BV=5.02
    {9983,1328,1056, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=4.71  BV=4.91
    {9983,1424,1056, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=4.81  BV=4.81
    {9983,1536,1048, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=4.91  BV=4.71
    {9983,1632,1064, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=5.01  BV=4.60
    {9983,1728,1072, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=5.11  BV=4.51
    {9983,1936,1024, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=5.21  BV=4.41
    {9983,2048,1040, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=5.31  BV=4.31
    {9983,2144,1064, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=5.41  BV=4.21
    {20006,1140,1064, 0, 0, 0},  //TV = 5.64(507 lines)  AV=2.97  SV=4.50  BV=4.12
    {20006,1216,1072, 0, 0, 0},  //TV = 5.64(507 lines)  AV=2.97  SV=4.60  BV=4.01
    {20006,1328,1048, 0, 0, 0},  //TV = 5.64(507 lines)  AV=2.97  SV=4.70  BV=3.92
    {20006,1424,1064, 0, 0, 0},  //TV = 5.64(507 lines)  AV=2.97  SV=4.82  BV=3.80
    {20006,1536,1048, 0, 0, 0},  //TV = 5.64(507 lines)  AV=2.97  SV=4.91  BV=3.71
    {20006,1632,1056, 0, 0, 0},  //TV = 5.64(507 lines)  AV=2.97  SV=5.00  BV=3.61
    {29989,1140,1080, 0, 0, 0},  //TV = 5.06(760 lines)  AV=2.97  SV=4.52  BV=3.51
    {29989,1216,1096, 0, 0, 0},  //TV = 5.06(760 lines)  AV=2.97  SV=4.63  BV=3.40
    {29989,1328,1064, 0, 0, 0},  //TV = 5.06(760 lines)  AV=2.97  SV=4.72  BV=3.31
    {29989,1424,1072, 0, 0, 0},  //TV = 5.06(760 lines)  AV=2.97  SV=4.83  BV=3.20
    {40011,1140,1064, 0, 0, 0},  //TV = 4.64(1014 lines)  AV=2.97  SV=4.50  BV=3.12
    {40011,1216,1072, 0, 0, 0},  //TV = 4.64(1014 lines)  AV=2.97  SV=4.60  BV=3.01
    {40011,1328,1048, 0, 0, 0},  //TV = 4.64(1014 lines)  AV=2.97  SV=4.70  BV=2.92
    {49994,1140,1056, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.49  BV=2.81
    {49994,1216,1064, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.59  BV=2.70
    {49994,1328,1040, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.68  BV=2.61
    {49994,1424,1040, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.78  BV=2.51
    {49994,1536,1032, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.88  BV=2.41
    {49994,1632,1040, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.98  BV=2.31
    {49994,1728,1056, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.09  BV=2.21
    {49994,1840,1064, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.19  BV=2.11
    {49994,2048,1024, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.29  BV=2.01
    {49994,2144,1048, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.39  BV=1.91
    {49994,2240,1072, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.48  BV=1.81
    {49994,2448,1048, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.58  BV=1.72
    {49994,2656,1040, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.68  BV=1.61
    {49994,2864,1032, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.78  BV=1.51
    {49994,3072,1032, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.88  BV=1.41
    {49994,3328,1024, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.99  BV=1.31
    {49994,3456,1056, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=6.09  BV=1.21
    {60017,3072,1064, 0, 0, 0},  //TV = 4.06(1521 lines)  AV=2.97  SV=5.93  BV=1.10
    {60017,3328,1048, 0, 0, 0},  //TV = 4.06(1521 lines)  AV=2.97  SV=6.02  BV=1.01
    {69999,3072,1048, 0, 0, 0},  //TV = 3.84(1774 lines)  AV=2.97  SV=5.91  BV=0.90
    {69999,3328,1032, 0, 0, 0},  //TV = 3.84(1774 lines)  AV=2.97  SV=6.00  BV=0.81
    {79982,3072,1048, 0, 0, 0},  //TV = 3.64(2027 lines)  AV=2.97  SV=5.91  BV=0.71
    {79982,3328,1032, 0, 0, 0},  //TV = 3.64(2027 lines)  AV=2.97  SV=6.00  BV=0.62
    {79982,3584,1032, 0, 0, 0},  //TV = 3.64(2027 lines)  AV=2.97  SV=6.11  BV=0.51
    {79982,3840,1032, 0, 0, 0},  //TV = 3.64(2027 lines)  AV=2.97  SV=6.20  BV=0.41
    {79982,4096,1040, 0, 0, 0},  //TV = 3.64(2027 lines)  AV=2.97  SV=6.31  BV=0.31
    {79982,4352,1040, 0, 0, 0},  //TV = 3.64(2027 lines)  AV=2.97  SV=6.40  BV=0.22
    {90005,4224,1024, 0, 0, 0},  //TV = 3.47(2281 lines)  AV=2.97  SV=6.33  BV=0.11
    {99987,4096,1032, 0, 0, 0},  //TV = 3.32(2534 lines)  AV=2.97  SV=6.30  BV=-0.01
    {99987,4352,1040, 0, 0, 0},  //TV = 3.32(2534 lines)  AV=2.97  SV=6.40  BV=-0.10
    {110010,4224,1040, 0, 0, 0},  //TV = 3.18(2788 lines)  AV=2.97  SV=6.35  BV=-0.20
    {119992,4096,1056, 0, 0, 0},  //TV = 3.06(3041 lines)  AV=2.97  SV=6.33  BV=-0.30
    {119992,4480,1032, 0, 0, 0},  //TV = 3.06(3041 lines)  AV=2.97  SV=6.43  BV=-0.40
    {119992,4736,1048, 0, 0, 0},  //TV = 3.06(3041 lines)  AV=2.97  SV=6.53  BV=-0.50
    {119992,5120,1040, 0, 0, 0},  //TV = 3.06(3041 lines)  AV=2.97  SV=6.63  BV=-0.60
    {119992,5504,1040, 0, 0, 0},  //TV = 3.06(3041 lines)  AV=2.97  SV=6.74  BV=-0.71
    {119992,5888,1040, 0, 0, 0},  //TV = 3.06(3041 lines)  AV=2.97  SV=6.83  BV=-0.80
    {119992,6144,1064, 0, 0, 0},  //TV = 3.06(3041 lines)  AV=2.97  SV=6.93  BV=-0.90
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
},
};

static strAETable g_AE_Custom1AutoTable =
{
    AETABLE_CUSTOM1_AUTO,    //eAETableID
    119,    //u4TotalIndex
    20,    //u4StrobeTrigerBV
    109,    //i4MaxBV
    -9,    //i4MinBV
    90,    //i4EffectiveMaxBV
    0,      //i4EffectiveMinBV
    LIB3A_AE_ISO_SPEED_AUTO, //ISO SPEED
    sCustom1PLineTable_60Hz,
    sCustom1PLineTable_50Hz,
    NULL,
};

static strEvPline sCustom2PLineTable_60Hz =
{
{
    {198,1140,1024, 0, 0, 0},  //TV = 12.30(5 lines)  AV=2.97  SV=4.44  BV=10.83
    {198,1140,1040, 0, 0, 0},  //TV = 12.30(5 lines)  AV=2.97  SV=4.46  BV=10.81
    {198,1216,1048, 0, 0, 0},  //TV = 12.30(5 lines)  AV=2.97  SV=4.57  BV=10.70
    {198,1328,1024, 0, 0, 0},  //TV = 12.30(5 lines)  AV=2.97  SV=4.66  BV=10.61
    {237,1140,1064, 0, 0, 0},  //TV = 12.04(6 lines)  AV=2.97  SV=4.50  BV=10.52
    {237,1216,1072, 0, 0, 0},  //TV = 12.04(6 lines)  AV=2.97  SV=4.60  BV=10.41
    {277,1140,1056, 0, 0, 0},  //TV = 11.82(7 lines)  AV=2.97  SV=4.49  BV=10.30
    {277,1216,1056, 0, 0, 0},  //TV = 11.82(7 lines)  AV=2.97  SV=4.58  BV=10.21
    {316,1140,1056, 0, 0, 0},  //TV = 11.63(8 lines)  AV=2.97  SV=4.49  BV=10.11
    {316,1216,1064, 0, 0, 0},  //TV = 11.63(8 lines)  AV=2.97  SV=4.59  BV=10.01
    {356,1140,1080, 0, 0, 0},  //TV = 11.46(9 lines)  AV=2.97  SV=4.52  BV=9.91
    {395,1140,1040, 0, 0, 0},  //TV = 11.31(10 lines)  AV=2.97  SV=4.46  BV=9.81
    {435,1140,1024, 0, 0, 0},  //TV = 11.17(11 lines)  AV=2.97  SV=4.44  BV=9.70
    {435,1140,1088, 0, 0, 0},  //TV = 11.17(11 lines)  AV=2.97  SV=4.53  BV=9.61
    {474,1140,1064, 0, 0, 0},  //TV = 11.04(12 lines)  AV=2.97  SV=4.50  BV=9.52
    {513,1140,1056, 0, 0, 0},  //TV = 10.93(13 lines)  AV=2.97  SV=4.49  BV=9.41
    {553,1140,1056, 0, 0, 0},  //TV = 10.82(14 lines)  AV=2.97  SV=4.49  BV=9.31
    {592,1140,1056, 0, 0, 0},  //TV = 10.72(15 lines)  AV=2.97  SV=4.49  BV=9.21
    {632,1140,1056, 0, 0, 0},  //TV = 10.63(16 lines)  AV=2.97  SV=4.49  BV=9.11
    {671,1140,1064, 0, 0, 0},  //TV = 10.54(17 lines)  AV=2.97  SV=4.50  BV=9.02
    {750,1140,1024, 0, 0, 0},  //TV = 10.38(19 lines)  AV=2.97  SV=4.44  BV=8.91
    {790,1140,1040, 0, 0, 0},  //TV = 10.31(20 lines)  AV=2.97  SV=4.46  BV=8.81
    {869,1140,1024, 0, 0, 0},  //TV = 10.17(22 lines)  AV=2.97  SV=4.44  BV=8.70
    {908,1140,1040, 0, 0, 0},  //TV = 10.11(23 lines)  AV=2.97  SV=4.46  BV=8.61
    {987,1140,1024, 0, 0, 0},  //TV = 9.98(25 lines)  AV=2.97  SV=4.44  BV=8.51
    {1066,1140,1024, 0, 0, 0},  //TV = 9.87(27 lines)  AV=2.97  SV=4.44  BV=8.40
    {1105,1140,1056, 0, 0, 0},  //TV = 9.82(28 lines)  AV=2.97  SV=4.49  BV=8.31
    {1224,1140,1024, 0, 0, 0},  //TV = 9.67(31 lines)  AV=2.97  SV=4.44  BV=8.20
    {1303,1140,1024, 0, 0, 0},  //TV = 9.58(33 lines)  AV=2.97  SV=4.44  BV=8.11
    {1382,1140,1040, 0, 0, 0},  //TV = 9.50(35 lines)  AV=2.97  SV=4.46  BV=8.01
    {1500,1140,1024, 0, 0, 0},  //TV = 9.38(38 lines)  AV=2.97  SV=4.44  BV=7.91
    {1618,1140,1024, 0, 0, 0},  //TV = 9.27(41 lines)  AV=2.97  SV=4.44  BV=7.80
    {1697,1140,1040, 0, 0, 0},  //TV = 9.20(43 lines)  AV=2.97  SV=4.46  BV=7.71
    {1855,1140,1024, 0, 0, 0},  //TV = 9.07(47 lines)  AV=2.97  SV=4.44  BV=7.60
    {1973,1140,1024, 0, 0, 0},  //TV = 8.99(50 lines)  AV=2.97  SV=4.44  BV=7.51
    {2131,1140,1024, 0, 0, 0},  //TV = 8.87(54 lines)  AV=2.97  SV=4.44  BV=7.40
    {2250,1140,1032, 0, 0, 0},  //TV = 8.80(57 lines)  AV=2.97  SV=4.45  BV=7.31
    {2447,1140,1024, 0, 0, 0},  //TV = 8.67(62 lines)  AV=2.97  SV=4.44  BV=7.20
    {2605,1140,1024, 0, 0, 0},  //TV = 8.58(66 lines)  AV=2.97  SV=4.44  BV=7.11
    {2802,1140,1024, 0, 0, 0},  //TV = 8.48(71 lines)  AV=2.97  SV=4.44  BV=7.01
    {2999,1140,1024, 0, 0, 0},  //TV = 8.38(76 lines)  AV=2.97  SV=4.44  BV=6.91
    {3236,1140,1024, 0, 0, 0},  //TV = 8.27(82 lines)  AV=2.97  SV=4.44  BV=6.80
    {3473,1140,1024, 0, 0, 0},  //TV = 8.17(88 lines)  AV=2.97  SV=4.44  BV=6.70
    {3710,1140,1024, 0, 0, 0},  //TV = 8.07(94 lines)  AV=2.97  SV=4.44  BV=6.60
    {3986,1140,1024, 0, 0, 0},  //TV = 7.97(101 lines)  AV=2.97  SV=4.44  BV=6.50
    {4262,1140,1024, 0, 0, 0},  //TV = 7.87(108 lines)  AV=2.97  SV=4.44  BV=6.40
    {4538,1140,1024, 0, 0, 0},  //TV = 7.78(115 lines)  AV=2.97  SV=4.44  BV=6.31
    {4893,1140,1024, 0, 0, 0},  //TV = 7.68(124 lines)  AV=2.97  SV=4.44  BV=6.20
    {5209,1140,1024, 0, 0, 0},  //TV = 7.58(132 lines)  AV=2.97  SV=4.44  BV=6.11
    {5643,1140,1024, 0, 0, 0},  //TV = 7.47(143 lines)  AV=2.97  SV=4.44  BV=6.00
    {6038,1140,1024, 0, 0, 0},  //TV = 7.37(153 lines)  AV=2.97  SV=4.44  BV=5.90
    {6472,1140,1024, 0, 0, 0},  //TV = 7.27(164 lines)  AV=2.97  SV=4.44  BV=5.80
    {6906,1140,1024, 0, 0, 0},  //TV = 7.18(175 lines)  AV=2.97  SV=4.44  BV=5.71
    {7419,1140,1024, 0, 0, 0},  //TV = 7.07(188 lines)  AV=2.97  SV=4.44  BV=5.60
    {7971,1140,1024, 0, 0, 0},  //TV = 6.97(202 lines)  AV=2.97  SV=4.44  BV=5.50
    {8326,1140,1048, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=4.48  BV=5.40
    {8326,1216,1048, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=4.57  BV=5.31
    {8326,1328,1024, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=4.66  BV=5.22
    {8326,1424,1024, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=4.76  BV=5.12
    {8326,1536,1024, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=4.87  BV=5.01
    {8326,1632,1032, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=4.97  BV=4.91
    {8326,1728,1040, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=5.06  BV=4.81
    {8326,1840,1048, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=5.17  BV=4.71
    {8326,1936,1072, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=5.27  BV=4.61
    {8326,2144,1032, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=5.36  BV=4.51
    {16652,1140,1048, 0, 0, 0},  //TV = 5.91(422 lines)  AV=2.97  SV=4.48  BV=4.40
    {16652,1216,1048, 0, 0, 0},  //TV = 5.91(422 lines)  AV=2.97  SV=4.57  BV=4.31
    {16652,1328,1024, 0, 0, 0},  //TV = 5.91(422 lines)  AV=2.97  SV=4.66  BV=4.22
    {16652,1424,1024, 0, 0, 0},  //TV = 5.91(422 lines)  AV=2.97  SV=4.76  BV=4.12
    {16652,1424,1104, 0, 0, 0},  //TV = 5.91(422 lines)  AV=2.97  SV=4.87  BV=4.01
    {16652,1632,1032, 0, 0, 0},  //TV = 5.91(422 lines)  AV=2.97  SV=4.97  BV=3.91
    {25017,1140,1056, 0, 0, 0},  //TV = 5.32(634 lines)  AV=2.97  SV=4.49  BV=3.81
    {25017,1216,1064, 0, 0, 0},  //TV = 5.32(634 lines)  AV=2.97  SV=4.59  BV=3.70
    {25017,1328,1048, 0, 0, 0},  //TV = 5.32(634 lines)  AV=2.97  SV=4.70  BV=3.60
    {25017,1424,1040, 0, 0, 0},  //TV = 5.32(634 lines)  AV=2.97  SV=4.78  BV=3.51
    {33343,1140,1048, 0, 0, 0},  //TV = 4.91(845 lines)  AV=2.97  SV=4.48  BV=3.40
    {33343,1216,1048, 0, 0, 0},  //TV = 4.91(845 lines)  AV=2.97  SV=4.57  BV=3.31
    {33343,1328,1024, 0, 0, 0},  //TV = 4.91(845 lines)  AV=2.97  SV=4.66  BV=3.22
    {41669,1140,1024, 0, 0, 0},  //TV = 4.58(1056 lines)  AV=2.97  SV=4.44  BV=3.11
    {41669,1216,1032, 0, 0, 0},  //TV = 4.58(1056 lines)  AV=2.97  SV=4.55  BV=3.01
    {41669,1216,1104, 0, 0, 0},  //TV = 4.58(1056 lines)  AV=2.97  SV=4.64  BV=2.91
    {49994,1140,1056, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.49  BV=2.81
    {49994,1216,1064, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.59  BV=2.70
    {49994,1328,1040, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.68  BV=2.61
    {49994,1424,1040, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.78  BV=2.51
    {49994,1536,1032, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.88  BV=2.41
    {49994,1632,1040, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.98  BV=2.31
    {49994,1728,1056, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.09  BV=2.21
    {49994,1840,1064, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.19  BV=2.11
    {49994,2048,1024, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.29  BV=2.01
    {49994,2144,1048, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.39  BV=1.91
    {49994,2240,1072, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.48  BV=1.81
    {49994,2448,1048, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.58  BV=1.72
    {49994,2656,1040, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.68  BV=1.61
    {49994,2864,1032, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.78  BV=1.51
    {49994,3072,1032, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.88  BV=1.41
    {49994,3328,1024, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.99  BV=1.31
    {49994,3456,1056, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=6.09  BV=1.21
    {58319,3200,1048, 0, 0, 0},  //TV = 4.10(1478 lines)  AV=2.97  SV=5.96  BV=1.11
    {66645,3072,1024, 0, 0, 0},  //TV = 3.91(1689 lines)  AV=2.97  SV=5.87  BV=1.01
    {66645,3200,1056, 0, 0, 0},  //TV = 3.91(1689 lines)  AV=2.97  SV=5.98  BV=0.90
    {75011,3072,1048, 0, 0, 0},  //TV = 3.74(1901 lines)  AV=2.97  SV=5.91  BV=0.80
    {75011,3328,1032, 0, 0, 0},  //TV = 3.74(1901 lines)  AV=2.97  SV=6.00  BV=0.71
    {83336,3200,1032, 0, 0, 0},  //TV = 3.58(2112 lines)  AV=2.97  SV=5.94  BV=0.61
    {83336,3456,1024, 0, 0, 0},  //TV = 3.58(2112 lines)  AV=2.97  SV=6.04  BV=0.51
    {83336,3712,1024, 0, 0, 0},  //TV = 3.58(2112 lines)  AV=2.97  SV=6.14  BV=0.41
    {83336,3968,1032, 0, 0, 0},  //TV = 3.58(2112 lines)  AV=2.97  SV=6.25  BV=0.30
    {83336,4224,1032, 0, 0, 0},  //TV = 3.58(2112 lines)  AV=2.97  SV=6.34  BV=0.21
    {91661,4096,1040, 0, 0, 0},  //TV = 3.45(2323 lines)  AV=2.97  SV=6.31  BV=0.11
    {99987,4096,1032, 0, 0, 0},  //TV = 3.32(2534 lines)  AV=2.97  SV=6.30  BV=-0.01
    {108313,4096,1024, 0, 0, 0},  //TV = 3.21(2745 lines)  AV=2.97  SV=6.29  BV=-0.11
    {108313,4352,1032, 0, 0, 0},  //TV = 3.21(2745 lines)  AV=2.97  SV=6.39  BV=-0.21
    {116678,4224,1048, 0, 0, 0},  //TV = 3.10(2957 lines)  AV=2.97  SV=6.36  BV=-0.29
    {125004,4224,1048, 0, 0, 0},  //TV = 3.00(3168 lines)  AV=2.97  SV=6.36  BV=-0.39
    {125004,4608,1032, 0, 0, 0},  //TV = 3.00(3168 lines)  AV=2.97  SV=6.47  BV=-0.50
    {125004,4864,1048, 0, 0, 0},  //TV = 3.00(3168 lines)  AV=2.97  SV=6.57  BV=-0.60
    {125004,5376,1024, 0, 0, 0},  //TV = 3.00(3168 lines)  AV=2.97  SV=6.68  BV=-0.71
    {125004,5632,1040, 0, 0, 0},  //TV = 3.00(3168 lines)  AV=2.97  SV=6.77  BV=-0.80
    {125004,6016,1040, 0, 0, 0},  //TV = 3.00(3168 lines)  AV=2.97  SV=6.86  BV=-0.89
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
},
};

static strEvPline sCustom2PLineTable_50Hz =
{
{
    {198,1140,1024, 0, 0, 0},  //TV = 12.30(5 lines)  AV=2.97  SV=4.44  BV=10.83
    {198,1140,1040, 0, 0, 0},  //TV = 12.30(5 lines)  AV=2.97  SV=4.46  BV=10.81
    {198,1216,1048, 0, 0, 0},  //TV = 12.30(5 lines)  AV=2.97  SV=4.57  BV=10.70
    {198,1328,1024, 0, 0, 0},  //TV = 12.30(5 lines)  AV=2.97  SV=4.66  BV=10.61
    {237,1140,1064, 0, 0, 0},  //TV = 12.04(6 lines)  AV=2.97  SV=4.50  BV=10.52
    {237,1216,1072, 0, 0, 0},  //TV = 12.04(6 lines)  AV=2.97  SV=4.60  BV=10.41
    {277,1140,1056, 0, 0, 0},  //TV = 11.82(7 lines)  AV=2.97  SV=4.49  BV=10.30
    {277,1216,1056, 0, 0, 0},  //TV = 11.82(7 lines)  AV=2.97  SV=4.58  BV=10.21
    {316,1140,1056, 0, 0, 0},  //TV = 11.63(8 lines)  AV=2.97  SV=4.49  BV=10.11
    {316,1216,1064, 0, 0, 0},  //TV = 11.63(8 lines)  AV=2.97  SV=4.59  BV=10.01
    {356,1140,1080, 0, 0, 0},  //TV = 11.46(9 lines)  AV=2.97  SV=4.52  BV=9.91
    {395,1140,1040, 0, 0, 0},  //TV = 11.31(10 lines)  AV=2.97  SV=4.46  BV=9.81
    {435,1140,1024, 0, 0, 0},  //TV = 11.17(11 lines)  AV=2.97  SV=4.44  BV=9.70
    {435,1140,1088, 0, 0, 0},  //TV = 11.17(11 lines)  AV=2.97  SV=4.53  BV=9.61
    {474,1140,1064, 0, 0, 0},  //TV = 11.04(12 lines)  AV=2.97  SV=4.50  BV=9.52
    {513,1140,1056, 0, 0, 0},  //TV = 10.93(13 lines)  AV=2.97  SV=4.49  BV=9.41
    {553,1140,1056, 0, 0, 0},  //TV = 10.82(14 lines)  AV=2.97  SV=4.49  BV=9.31
    {592,1140,1056, 0, 0, 0},  //TV = 10.72(15 lines)  AV=2.97  SV=4.49  BV=9.21
    {632,1140,1056, 0, 0, 0},  //TV = 10.63(16 lines)  AV=2.97  SV=4.49  BV=9.11
    {671,1140,1064, 0, 0, 0},  //TV = 10.54(17 lines)  AV=2.97  SV=4.50  BV=9.02
    {750,1140,1024, 0, 0, 0},  //TV = 10.38(19 lines)  AV=2.97  SV=4.44  BV=8.91
    {790,1140,1040, 0, 0, 0},  //TV = 10.31(20 lines)  AV=2.97  SV=4.46  BV=8.81
    {869,1140,1024, 0, 0, 0},  //TV = 10.17(22 lines)  AV=2.97  SV=4.44  BV=8.70
    {908,1140,1040, 0, 0, 0},  //TV = 10.11(23 lines)  AV=2.97  SV=4.46  BV=8.61
    {987,1140,1024, 0, 0, 0},  //TV = 9.98(25 lines)  AV=2.97  SV=4.44  BV=8.51
    {1066,1140,1024, 0, 0, 0},  //TV = 9.87(27 lines)  AV=2.97  SV=4.44  BV=8.40
    {1105,1140,1056, 0, 0, 0},  //TV = 9.82(28 lines)  AV=2.97  SV=4.49  BV=8.31
    {1224,1140,1024, 0, 0, 0},  //TV = 9.67(31 lines)  AV=2.97  SV=4.44  BV=8.20
    {1303,1140,1024, 0, 0, 0},  //TV = 9.58(33 lines)  AV=2.97  SV=4.44  BV=8.11
    {1382,1140,1040, 0, 0, 0},  //TV = 9.50(35 lines)  AV=2.97  SV=4.46  BV=8.01
    {1500,1140,1024, 0, 0, 0},  //TV = 9.38(38 lines)  AV=2.97  SV=4.44  BV=7.91
    {1618,1140,1024, 0, 0, 0},  //TV = 9.27(41 lines)  AV=2.97  SV=4.44  BV=7.80
    {1697,1140,1040, 0, 0, 0},  //TV = 9.20(43 lines)  AV=2.97  SV=4.46  BV=7.71
    {1855,1140,1024, 0, 0, 0},  //TV = 9.07(47 lines)  AV=2.97  SV=4.44  BV=7.60
    {1973,1140,1024, 0, 0, 0},  //TV = 8.99(50 lines)  AV=2.97  SV=4.44  BV=7.51
    {2131,1140,1024, 0, 0, 0},  //TV = 8.87(54 lines)  AV=2.97  SV=4.44  BV=7.40
    {2250,1140,1032, 0, 0, 0},  //TV = 8.80(57 lines)  AV=2.97  SV=4.45  BV=7.31
    {2447,1140,1024, 0, 0, 0},  //TV = 8.67(62 lines)  AV=2.97  SV=4.44  BV=7.20
    {2605,1140,1024, 0, 0, 0},  //TV = 8.58(66 lines)  AV=2.97  SV=4.44  BV=7.11
    {2802,1140,1024, 0, 0, 0},  //TV = 8.48(71 lines)  AV=2.97  SV=4.44  BV=7.01
    {2999,1140,1024, 0, 0, 0},  //TV = 8.38(76 lines)  AV=2.97  SV=4.44  BV=6.91
    {3236,1140,1024, 0, 0, 0},  //TV = 8.27(82 lines)  AV=2.97  SV=4.44  BV=6.80
    {3473,1140,1024, 0, 0, 0},  //TV = 8.17(88 lines)  AV=2.97  SV=4.44  BV=6.70
    {3710,1140,1024, 0, 0, 0},  //TV = 8.07(94 lines)  AV=2.97  SV=4.44  BV=6.60
    {3986,1140,1024, 0, 0, 0},  //TV = 7.97(101 lines)  AV=2.97  SV=4.44  BV=6.50
    {4262,1140,1024, 0, 0, 0},  //TV = 7.87(108 lines)  AV=2.97  SV=4.44  BV=6.40
    {4538,1140,1024, 0, 0, 0},  //TV = 7.78(115 lines)  AV=2.97  SV=4.44  BV=6.31
    {4893,1140,1024, 0, 0, 0},  //TV = 7.68(124 lines)  AV=2.97  SV=4.44  BV=6.20
    {5209,1140,1024, 0, 0, 0},  //TV = 7.58(132 lines)  AV=2.97  SV=4.44  BV=6.11
    {5643,1140,1024, 0, 0, 0},  //TV = 7.47(143 lines)  AV=2.97  SV=4.44  BV=6.00
    {6038,1140,1024, 0, 0, 0},  //TV = 7.37(153 lines)  AV=2.97  SV=4.44  BV=5.90
    {6472,1140,1024, 0, 0, 0},  //TV = 7.27(164 lines)  AV=2.97  SV=4.44  BV=5.80
    {6906,1140,1024, 0, 0, 0},  //TV = 7.18(175 lines)  AV=2.97  SV=4.44  BV=5.71
    {7419,1140,1024, 0, 0, 0},  //TV = 7.07(188 lines)  AV=2.97  SV=4.44  BV=5.60
    {7971,1140,1024, 0, 0, 0},  //TV = 6.97(202 lines)  AV=2.97  SV=4.44  BV=5.50
    {8484,1140,1024, 0, 0, 0},  //TV = 6.88(215 lines)  AV=2.97  SV=4.44  BV=5.41
    {9155,1140,1024, 0, 0, 0},  //TV = 6.77(232 lines)  AV=2.97  SV=4.44  BV=5.30
    {9826,1140,1024, 0, 0, 0},  //TV = 6.67(249 lines)  AV=2.97  SV=4.44  BV=5.20
    {9983,1140,1072, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=4.51  BV=5.11
    {9983,1216,1072, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=4.60  BV=5.02
    {9983,1328,1056, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=4.71  BV=4.91
    {9983,1424,1056, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=4.81  BV=4.81
    {9983,1536,1048, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=4.91  BV=4.71
    {9983,1632,1064, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=5.01  BV=4.60
    {9983,1728,1072, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=5.11  BV=4.51
    {9983,1936,1024, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=5.21  BV=4.41
    {9983,2048,1040, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=5.31  BV=4.31
    {9983,2144,1064, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=5.41  BV=4.21
    {20006,1140,1064, 0, 0, 0},  //TV = 5.64(507 lines)  AV=2.97  SV=4.50  BV=4.12
    {20006,1216,1072, 0, 0, 0},  //TV = 5.64(507 lines)  AV=2.97  SV=4.60  BV=4.01
    {20006,1328,1048, 0, 0, 0},  //TV = 5.64(507 lines)  AV=2.97  SV=4.70  BV=3.92
    {20006,1424,1064, 0, 0, 0},  //TV = 5.64(507 lines)  AV=2.97  SV=4.82  BV=3.80
    {20006,1536,1048, 0, 0, 0},  //TV = 5.64(507 lines)  AV=2.97  SV=4.91  BV=3.71
    {20006,1632,1056, 0, 0, 0},  //TV = 5.64(507 lines)  AV=2.97  SV=5.00  BV=3.61
    {29989,1140,1080, 0, 0, 0},  //TV = 5.06(760 lines)  AV=2.97  SV=4.52  BV=3.51
    {29989,1216,1096, 0, 0, 0},  //TV = 5.06(760 lines)  AV=2.97  SV=4.63  BV=3.40
    {29989,1328,1064, 0, 0, 0},  //TV = 5.06(760 lines)  AV=2.97  SV=4.72  BV=3.31
    {29989,1424,1072, 0, 0, 0},  //TV = 5.06(760 lines)  AV=2.97  SV=4.83  BV=3.20
    {40011,1140,1064, 0, 0, 0},  //TV = 4.64(1014 lines)  AV=2.97  SV=4.50  BV=3.12
    {40011,1216,1072, 0, 0, 0},  //TV = 4.64(1014 lines)  AV=2.97  SV=4.60  BV=3.01
    {40011,1328,1048, 0, 0, 0},  //TV = 4.64(1014 lines)  AV=2.97  SV=4.70  BV=2.92
    {49994,1140,1056, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.49  BV=2.81
    {49994,1216,1064, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.59  BV=2.70
    {49994,1328,1040, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.68  BV=2.61
    {49994,1424,1040, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.78  BV=2.51
    {49994,1536,1032, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.88  BV=2.41
    {49994,1632,1040, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.98  BV=2.31
    {49994,1728,1056, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.09  BV=2.21
    {49994,1840,1064, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.19  BV=2.11
    {49994,2048,1024, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.29  BV=2.01
    {49994,2144,1048, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.39  BV=1.91
    {49994,2240,1072, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.48  BV=1.81
    {49994,2448,1048, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.58  BV=1.72
    {49994,2656,1040, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.68  BV=1.61
    {49994,2864,1032, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.78  BV=1.51
    {49994,3072,1032, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.88  BV=1.41
    {49994,3328,1024, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.99  BV=1.31
    {49994,3456,1056, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=6.09  BV=1.21
    {60017,3072,1064, 0, 0, 0},  //TV = 4.06(1521 lines)  AV=2.97  SV=5.93  BV=1.10
    {60017,3328,1048, 0, 0, 0},  //TV = 4.06(1521 lines)  AV=2.97  SV=6.02  BV=1.01
    {69999,3072,1048, 0, 0, 0},  //TV = 3.84(1774 lines)  AV=2.97  SV=5.91  BV=0.90
    {69999,3328,1032, 0, 0, 0},  //TV = 3.84(1774 lines)  AV=2.97  SV=6.00  BV=0.81
    {79982,3072,1048, 0, 0, 0},  //TV = 3.64(2027 lines)  AV=2.97  SV=5.91  BV=0.71
    {79982,3328,1032, 0, 0, 0},  //TV = 3.64(2027 lines)  AV=2.97  SV=6.00  BV=0.62
    {79982,3584,1032, 0, 0, 0},  //TV = 3.64(2027 lines)  AV=2.97  SV=6.11  BV=0.51
    {79982,3840,1032, 0, 0, 0},  //TV = 3.64(2027 lines)  AV=2.97  SV=6.20  BV=0.41
    {79982,4096,1040, 0, 0, 0},  //TV = 3.64(2027 lines)  AV=2.97  SV=6.31  BV=0.31
    {79982,4352,1040, 0, 0, 0},  //TV = 3.64(2027 lines)  AV=2.97  SV=6.40  BV=0.22
    {90005,4224,1024, 0, 0, 0},  //TV = 3.47(2281 lines)  AV=2.97  SV=6.33  BV=0.11
    {99987,4096,1032, 0, 0, 0},  //TV = 3.32(2534 lines)  AV=2.97  SV=6.30  BV=-0.01
    {99987,4352,1040, 0, 0, 0},  //TV = 3.32(2534 lines)  AV=2.97  SV=6.40  BV=-0.10
    {110010,4224,1040, 0, 0, 0},  //TV = 3.18(2788 lines)  AV=2.97  SV=6.35  BV=-0.20
    {119992,4096,1056, 0, 0, 0},  //TV = 3.06(3041 lines)  AV=2.97  SV=6.33  BV=-0.30
    {119992,4480,1032, 0, 0, 0},  //TV = 3.06(3041 lines)  AV=2.97  SV=6.43  BV=-0.40
    {119992,4736,1048, 0, 0, 0},  //TV = 3.06(3041 lines)  AV=2.97  SV=6.53  BV=-0.50
    {119992,5120,1040, 0, 0, 0},  //TV = 3.06(3041 lines)  AV=2.97  SV=6.63  BV=-0.60
    {119992,5504,1040, 0, 0, 0},  //TV = 3.06(3041 lines)  AV=2.97  SV=6.74  BV=-0.71
    {119992,5888,1040, 0, 0, 0},  //TV = 3.06(3041 lines)  AV=2.97  SV=6.83  BV=-0.80
    {119992,6144,1064, 0, 0, 0},  //TV = 3.06(3041 lines)  AV=2.97  SV=6.93  BV=-0.90
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
},
};

static strAETable g_AE_Custom2AutoTable =
{
    AETABLE_CUSTOM2_AUTO,    //eAETableID
    119,    //u4TotalIndex
    20,    //u4StrobeTrigerBV
    109,    //i4MaxBV
    -9,    //i4MinBV
    90,    //i4EffectiveMaxBV
    0,      //i4EffectiveMinBV
    LIB3A_AE_ISO_SPEED_AUTO, //ISO SPEED
    sCustom2PLineTable_60Hz,
    sCustom2PLineTable_50Hz,
    NULL,
};

static strEvPline sCustom3PLineTable_60Hz =
{
{
    {198,1140,1024, 0, 0, 0},  //TV = 12.30(5 lines)  AV=2.97  SV=4.44  BV=10.83
    {198,1140,1040, 0, 0, 0},  //TV = 12.30(5 lines)  AV=2.97  SV=4.46  BV=10.81
    {198,1216,1048, 0, 0, 0},  //TV = 12.30(5 lines)  AV=2.97  SV=4.57  BV=10.70
    {198,1328,1024, 0, 0, 0},  //TV = 12.30(5 lines)  AV=2.97  SV=4.66  BV=10.61
    {237,1140,1064, 0, 0, 0},  //TV = 12.04(6 lines)  AV=2.97  SV=4.50  BV=10.52
    {237,1216,1072, 0, 0, 0},  //TV = 12.04(6 lines)  AV=2.97  SV=4.60  BV=10.41
    {277,1140,1056, 0, 0, 0},  //TV = 11.82(7 lines)  AV=2.97  SV=4.49  BV=10.30
    {277,1216,1056, 0, 0, 0},  //TV = 11.82(7 lines)  AV=2.97  SV=4.58  BV=10.21
    {316,1140,1056, 0, 0, 0},  //TV = 11.63(8 lines)  AV=2.97  SV=4.49  BV=10.11
    {316,1216,1064, 0, 0, 0},  //TV = 11.63(8 lines)  AV=2.97  SV=4.59  BV=10.01
    {356,1140,1080, 0, 0, 0},  //TV = 11.46(9 lines)  AV=2.97  SV=4.52  BV=9.91
    {395,1140,1040, 0, 0, 0},  //TV = 11.31(10 lines)  AV=2.97  SV=4.46  BV=9.81
    {435,1140,1024, 0, 0, 0},  //TV = 11.17(11 lines)  AV=2.97  SV=4.44  BV=9.70
    {435,1140,1088, 0, 0, 0},  //TV = 11.17(11 lines)  AV=2.97  SV=4.53  BV=9.61
    {474,1140,1064, 0, 0, 0},  //TV = 11.04(12 lines)  AV=2.97  SV=4.50  BV=9.52
    {513,1140,1056, 0, 0, 0},  //TV = 10.93(13 lines)  AV=2.97  SV=4.49  BV=9.41
    {553,1140,1056, 0, 0, 0},  //TV = 10.82(14 lines)  AV=2.97  SV=4.49  BV=9.31
    {592,1140,1056, 0, 0, 0},  //TV = 10.72(15 lines)  AV=2.97  SV=4.49  BV=9.21
    {632,1140,1056, 0, 0, 0},  //TV = 10.63(16 lines)  AV=2.97  SV=4.49  BV=9.11
    {671,1140,1064, 0, 0, 0},  //TV = 10.54(17 lines)  AV=2.97  SV=4.50  BV=9.02
    {750,1140,1024, 0, 0, 0},  //TV = 10.38(19 lines)  AV=2.97  SV=4.44  BV=8.91
    {790,1140,1040, 0, 0, 0},  //TV = 10.31(20 lines)  AV=2.97  SV=4.46  BV=8.81
    {869,1140,1024, 0, 0, 0},  //TV = 10.17(22 lines)  AV=2.97  SV=4.44  BV=8.70
    {908,1140,1040, 0, 0, 0},  //TV = 10.11(23 lines)  AV=2.97  SV=4.46  BV=8.61
    {987,1140,1024, 0, 0, 0},  //TV = 9.98(25 lines)  AV=2.97  SV=4.44  BV=8.51
    {1066,1140,1024, 0, 0, 0},  //TV = 9.87(27 lines)  AV=2.97  SV=4.44  BV=8.40
    {1105,1140,1056, 0, 0, 0},  //TV = 9.82(28 lines)  AV=2.97  SV=4.49  BV=8.31
    {1224,1140,1024, 0, 0, 0},  //TV = 9.67(31 lines)  AV=2.97  SV=4.44  BV=8.20
    {1303,1140,1024, 0, 0, 0},  //TV = 9.58(33 lines)  AV=2.97  SV=4.44  BV=8.11
    {1382,1140,1040, 0, 0, 0},  //TV = 9.50(35 lines)  AV=2.97  SV=4.46  BV=8.01
    {1500,1140,1024, 0, 0, 0},  //TV = 9.38(38 lines)  AV=2.97  SV=4.44  BV=7.91
    {1618,1140,1024, 0, 0, 0},  //TV = 9.27(41 lines)  AV=2.97  SV=4.44  BV=7.80
    {1697,1140,1040, 0, 0, 0},  //TV = 9.20(43 lines)  AV=2.97  SV=4.46  BV=7.71
    {1855,1140,1024, 0, 0, 0},  //TV = 9.07(47 lines)  AV=2.97  SV=4.44  BV=7.60
    {1973,1140,1024, 0, 0, 0},  //TV = 8.99(50 lines)  AV=2.97  SV=4.44  BV=7.51
    {2131,1140,1024, 0, 0, 0},  //TV = 8.87(54 lines)  AV=2.97  SV=4.44  BV=7.40
    {2250,1140,1032, 0, 0, 0},  //TV = 8.80(57 lines)  AV=2.97  SV=4.45  BV=7.31
    {2447,1140,1024, 0, 0, 0},  //TV = 8.67(62 lines)  AV=2.97  SV=4.44  BV=7.20
    {2605,1140,1024, 0, 0, 0},  //TV = 8.58(66 lines)  AV=2.97  SV=4.44  BV=7.11
    {2802,1140,1024, 0, 0, 0},  //TV = 8.48(71 lines)  AV=2.97  SV=4.44  BV=7.01
    {2999,1140,1024, 0, 0, 0},  //TV = 8.38(76 lines)  AV=2.97  SV=4.44  BV=6.91
    {3236,1140,1024, 0, 0, 0},  //TV = 8.27(82 lines)  AV=2.97  SV=4.44  BV=6.80
    {3473,1140,1024, 0, 0, 0},  //TV = 8.17(88 lines)  AV=2.97  SV=4.44  BV=6.70
    {3710,1140,1024, 0, 0, 0},  //TV = 8.07(94 lines)  AV=2.97  SV=4.44  BV=6.60
    {3986,1140,1024, 0, 0, 0},  //TV = 7.97(101 lines)  AV=2.97  SV=4.44  BV=6.50
    {4262,1140,1024, 0, 0, 0},  //TV = 7.87(108 lines)  AV=2.97  SV=4.44  BV=6.40
    {4538,1140,1024, 0, 0, 0},  //TV = 7.78(115 lines)  AV=2.97  SV=4.44  BV=6.31
    {4893,1140,1024, 0, 0, 0},  //TV = 7.68(124 lines)  AV=2.97  SV=4.44  BV=6.20
    {5209,1140,1024, 0, 0, 0},  //TV = 7.58(132 lines)  AV=2.97  SV=4.44  BV=6.11
    {5643,1140,1024, 0, 0, 0},  //TV = 7.47(143 lines)  AV=2.97  SV=4.44  BV=6.00
    {6038,1140,1024, 0, 0, 0},  //TV = 7.37(153 lines)  AV=2.97  SV=4.44  BV=5.90
    {6472,1140,1024, 0, 0, 0},  //TV = 7.27(164 lines)  AV=2.97  SV=4.44  BV=5.80
    {6906,1140,1024, 0, 0, 0},  //TV = 7.18(175 lines)  AV=2.97  SV=4.44  BV=5.71
    {7419,1140,1024, 0, 0, 0},  //TV = 7.07(188 lines)  AV=2.97  SV=4.44  BV=5.60
    {7971,1140,1024, 0, 0, 0},  //TV = 6.97(202 lines)  AV=2.97  SV=4.44  BV=5.50
    {8326,1140,1048, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=4.48  BV=5.40
    {8326,1216,1048, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=4.57  BV=5.31
    {8326,1328,1024, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=4.66  BV=5.22
    {8326,1424,1024, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=4.76  BV=5.12
    {8326,1536,1024, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=4.87  BV=5.01
    {8326,1632,1032, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=4.97  BV=4.91
    {8326,1728,1040, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=5.06  BV=4.81
    {8326,1840,1048, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=5.17  BV=4.71
    {8326,1936,1072, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=5.27  BV=4.61
    {8326,2144,1032, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=5.36  BV=4.51
    {16652,1140,1048, 0, 0, 0},  //TV = 5.91(422 lines)  AV=2.97  SV=4.48  BV=4.40
    {16652,1216,1048, 0, 0, 0},  //TV = 5.91(422 lines)  AV=2.97  SV=4.57  BV=4.31
    {16652,1328,1024, 0, 0, 0},  //TV = 5.91(422 lines)  AV=2.97  SV=4.66  BV=4.22
    {16652,1424,1024, 0, 0, 0},  //TV = 5.91(422 lines)  AV=2.97  SV=4.76  BV=4.12
    {16652,1424,1104, 0, 0, 0},  //TV = 5.91(422 lines)  AV=2.97  SV=4.87  BV=4.01
    {16652,1632,1032, 0, 0, 0},  //TV = 5.91(422 lines)  AV=2.97  SV=4.97  BV=3.91
    {25017,1140,1056, 0, 0, 0},  //TV = 5.32(634 lines)  AV=2.97  SV=4.49  BV=3.81
    {25017,1216,1064, 0, 0, 0},  //TV = 5.32(634 lines)  AV=2.97  SV=4.59  BV=3.70
    {25017,1328,1048, 0, 0, 0},  //TV = 5.32(634 lines)  AV=2.97  SV=4.70  BV=3.60
    {25017,1424,1040, 0, 0, 0},  //TV = 5.32(634 lines)  AV=2.97  SV=4.78  BV=3.51
    {33343,1140,1048, 0, 0, 0},  //TV = 4.91(845 lines)  AV=2.97  SV=4.48  BV=3.40
    {33343,1216,1048, 0, 0, 0},  //TV = 4.91(845 lines)  AV=2.97  SV=4.57  BV=3.31
    {33343,1328,1024, 0, 0, 0},  //TV = 4.91(845 lines)  AV=2.97  SV=4.66  BV=3.22
    {41669,1140,1024, 0, 0, 0},  //TV = 4.58(1056 lines)  AV=2.97  SV=4.44  BV=3.11
    {41669,1216,1032, 0, 0, 0},  //TV = 4.58(1056 lines)  AV=2.97  SV=4.55  BV=3.01
    {41669,1216,1104, 0, 0, 0},  //TV = 4.58(1056 lines)  AV=2.97  SV=4.64  BV=2.91
    {49994,1140,1056, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.49  BV=2.81
    {49994,1216,1064, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.59  BV=2.70
    {49994,1328,1040, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.68  BV=2.61
    {49994,1424,1040, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.78  BV=2.51
    {49994,1536,1032, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.88  BV=2.41
    {49994,1632,1040, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.98  BV=2.31
    {49994,1728,1056, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.09  BV=2.21
    {49994,1840,1064, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.19  BV=2.11
    {49994,2048,1024, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.29  BV=2.01
    {49994,2144,1048, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.39  BV=1.91
    {49994,2240,1072, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.48  BV=1.81
    {49994,2448,1048, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.58  BV=1.72
    {49994,2656,1040, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.68  BV=1.61
    {49994,2864,1032, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.78  BV=1.51
    {49994,3072,1032, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.88  BV=1.41
    {49994,3328,1024, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.99  BV=1.31
    {49994,3456,1056, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=6.09  BV=1.21
    {58319,3200,1048, 0, 0, 0},  //TV = 4.10(1478 lines)  AV=2.97  SV=5.96  BV=1.11
    {66645,3072,1024, 0, 0, 0},  //TV = 3.91(1689 lines)  AV=2.97  SV=5.87  BV=1.01
    {66645,3200,1056, 0, 0, 0},  //TV = 3.91(1689 lines)  AV=2.97  SV=5.98  BV=0.90
    {75011,3072,1048, 0, 0, 0},  //TV = 3.74(1901 lines)  AV=2.97  SV=5.91  BV=0.80
    {75011,3328,1032, 0, 0, 0},  //TV = 3.74(1901 lines)  AV=2.97  SV=6.00  BV=0.71
    {83336,3200,1032, 0, 0, 0},  //TV = 3.58(2112 lines)  AV=2.97  SV=5.94  BV=0.61
    {83336,3456,1024, 0, 0, 0},  //TV = 3.58(2112 lines)  AV=2.97  SV=6.04  BV=0.51
    {83336,3712,1024, 0, 0, 0},  //TV = 3.58(2112 lines)  AV=2.97  SV=6.14  BV=0.41
    {83336,3968,1032, 0, 0, 0},  //TV = 3.58(2112 lines)  AV=2.97  SV=6.25  BV=0.30
    {83336,4224,1032, 0, 0, 0},  //TV = 3.58(2112 lines)  AV=2.97  SV=6.34  BV=0.21
    {91661,4096,1040, 0, 0, 0},  //TV = 3.45(2323 lines)  AV=2.97  SV=6.31  BV=0.11
    {99987,4096,1032, 0, 0, 0},  //TV = 3.32(2534 lines)  AV=2.97  SV=6.30  BV=-0.01
    {108313,4096,1024, 0, 0, 0},  //TV = 3.21(2745 lines)  AV=2.97  SV=6.29  BV=-0.11
    {108313,4352,1032, 0, 0, 0},  //TV = 3.21(2745 lines)  AV=2.97  SV=6.39  BV=-0.21
    {116678,4224,1048, 0, 0, 0},  //TV = 3.10(2957 lines)  AV=2.97  SV=6.36  BV=-0.29
    {125004,4224,1048, 0, 0, 0},  //TV = 3.00(3168 lines)  AV=2.97  SV=6.36  BV=-0.39
    {125004,4608,1032, 0, 0, 0},  //TV = 3.00(3168 lines)  AV=2.97  SV=6.47  BV=-0.50
    {125004,4864,1048, 0, 0, 0},  //TV = 3.00(3168 lines)  AV=2.97  SV=6.57  BV=-0.60
    {125004,5376,1024, 0, 0, 0},  //TV = 3.00(3168 lines)  AV=2.97  SV=6.68  BV=-0.71
    {125004,5632,1040, 0, 0, 0},  //TV = 3.00(3168 lines)  AV=2.97  SV=6.77  BV=-0.80
    {125004,6016,1040, 0, 0, 0},  //TV = 3.00(3168 lines)  AV=2.97  SV=6.86  BV=-0.89
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
},
};

static strEvPline sCustom3PLineTable_50Hz =
{
{
    {198,1140,1024, 0, 0, 0},  //TV = 12.30(5 lines)  AV=2.97  SV=4.44  BV=10.83
    {198,1140,1040, 0, 0, 0},  //TV = 12.30(5 lines)  AV=2.97  SV=4.46  BV=10.81
    {198,1216,1048, 0, 0, 0},  //TV = 12.30(5 lines)  AV=2.97  SV=4.57  BV=10.70
    {198,1328,1024, 0, 0, 0},  //TV = 12.30(5 lines)  AV=2.97  SV=4.66  BV=10.61
    {237,1140,1064, 0, 0, 0},  //TV = 12.04(6 lines)  AV=2.97  SV=4.50  BV=10.52
    {237,1216,1072, 0, 0, 0},  //TV = 12.04(6 lines)  AV=2.97  SV=4.60  BV=10.41
    {277,1140,1056, 0, 0, 0},  //TV = 11.82(7 lines)  AV=2.97  SV=4.49  BV=10.30
    {277,1216,1056, 0, 0, 0},  //TV = 11.82(7 lines)  AV=2.97  SV=4.58  BV=10.21
    {316,1140,1056, 0, 0, 0},  //TV = 11.63(8 lines)  AV=2.97  SV=4.49  BV=10.11
    {316,1216,1064, 0, 0, 0},  //TV = 11.63(8 lines)  AV=2.97  SV=4.59  BV=10.01
    {356,1140,1080, 0, 0, 0},  //TV = 11.46(9 lines)  AV=2.97  SV=4.52  BV=9.91
    {395,1140,1040, 0, 0, 0},  //TV = 11.31(10 lines)  AV=2.97  SV=4.46  BV=9.81
    {435,1140,1024, 0, 0, 0},  //TV = 11.17(11 lines)  AV=2.97  SV=4.44  BV=9.70
    {435,1140,1088, 0, 0, 0},  //TV = 11.17(11 lines)  AV=2.97  SV=4.53  BV=9.61
    {474,1140,1064, 0, 0, 0},  //TV = 11.04(12 lines)  AV=2.97  SV=4.50  BV=9.52
    {513,1140,1056, 0, 0, 0},  //TV = 10.93(13 lines)  AV=2.97  SV=4.49  BV=9.41
    {553,1140,1056, 0, 0, 0},  //TV = 10.82(14 lines)  AV=2.97  SV=4.49  BV=9.31
    {592,1140,1056, 0, 0, 0},  //TV = 10.72(15 lines)  AV=2.97  SV=4.49  BV=9.21
    {632,1140,1056, 0, 0, 0},  //TV = 10.63(16 lines)  AV=2.97  SV=4.49  BV=9.11
    {671,1140,1064, 0, 0, 0},  //TV = 10.54(17 lines)  AV=2.97  SV=4.50  BV=9.02
    {750,1140,1024, 0, 0, 0},  //TV = 10.38(19 lines)  AV=2.97  SV=4.44  BV=8.91
    {790,1140,1040, 0, 0, 0},  //TV = 10.31(20 lines)  AV=2.97  SV=4.46  BV=8.81
    {869,1140,1024, 0, 0, 0},  //TV = 10.17(22 lines)  AV=2.97  SV=4.44  BV=8.70
    {908,1140,1040, 0, 0, 0},  //TV = 10.11(23 lines)  AV=2.97  SV=4.46  BV=8.61
    {987,1140,1024, 0, 0, 0},  //TV = 9.98(25 lines)  AV=2.97  SV=4.44  BV=8.51
    {1066,1140,1024, 0, 0, 0},  //TV = 9.87(27 lines)  AV=2.97  SV=4.44  BV=8.40
    {1105,1140,1056, 0, 0, 0},  //TV = 9.82(28 lines)  AV=2.97  SV=4.49  BV=8.31
    {1224,1140,1024, 0, 0, 0},  //TV = 9.67(31 lines)  AV=2.97  SV=4.44  BV=8.20
    {1303,1140,1024, 0, 0, 0},  //TV = 9.58(33 lines)  AV=2.97  SV=4.44  BV=8.11
    {1382,1140,1040, 0, 0, 0},  //TV = 9.50(35 lines)  AV=2.97  SV=4.46  BV=8.01
    {1500,1140,1024, 0, 0, 0},  //TV = 9.38(38 lines)  AV=2.97  SV=4.44  BV=7.91
    {1618,1140,1024, 0, 0, 0},  //TV = 9.27(41 lines)  AV=2.97  SV=4.44  BV=7.80
    {1697,1140,1040, 0, 0, 0},  //TV = 9.20(43 lines)  AV=2.97  SV=4.46  BV=7.71
    {1855,1140,1024, 0, 0, 0},  //TV = 9.07(47 lines)  AV=2.97  SV=4.44  BV=7.60
    {1973,1140,1024, 0, 0, 0},  //TV = 8.99(50 lines)  AV=2.97  SV=4.44  BV=7.51
    {2131,1140,1024, 0, 0, 0},  //TV = 8.87(54 lines)  AV=2.97  SV=4.44  BV=7.40
    {2250,1140,1032, 0, 0, 0},  //TV = 8.80(57 lines)  AV=2.97  SV=4.45  BV=7.31
    {2447,1140,1024, 0, 0, 0},  //TV = 8.67(62 lines)  AV=2.97  SV=4.44  BV=7.20
    {2605,1140,1024, 0, 0, 0},  //TV = 8.58(66 lines)  AV=2.97  SV=4.44  BV=7.11
    {2802,1140,1024, 0, 0, 0},  //TV = 8.48(71 lines)  AV=2.97  SV=4.44  BV=7.01
    {2999,1140,1024, 0, 0, 0},  //TV = 8.38(76 lines)  AV=2.97  SV=4.44  BV=6.91
    {3236,1140,1024, 0, 0, 0},  //TV = 8.27(82 lines)  AV=2.97  SV=4.44  BV=6.80
    {3473,1140,1024, 0, 0, 0},  //TV = 8.17(88 lines)  AV=2.97  SV=4.44  BV=6.70
    {3710,1140,1024, 0, 0, 0},  //TV = 8.07(94 lines)  AV=2.97  SV=4.44  BV=6.60
    {3986,1140,1024, 0, 0, 0},  //TV = 7.97(101 lines)  AV=2.97  SV=4.44  BV=6.50
    {4262,1140,1024, 0, 0, 0},  //TV = 7.87(108 lines)  AV=2.97  SV=4.44  BV=6.40
    {4538,1140,1024, 0, 0, 0},  //TV = 7.78(115 lines)  AV=2.97  SV=4.44  BV=6.31
    {4893,1140,1024, 0, 0, 0},  //TV = 7.68(124 lines)  AV=2.97  SV=4.44  BV=6.20
    {5209,1140,1024, 0, 0, 0},  //TV = 7.58(132 lines)  AV=2.97  SV=4.44  BV=6.11
    {5643,1140,1024, 0, 0, 0},  //TV = 7.47(143 lines)  AV=2.97  SV=4.44  BV=6.00
    {6038,1140,1024, 0, 0, 0},  //TV = 7.37(153 lines)  AV=2.97  SV=4.44  BV=5.90
    {6472,1140,1024, 0, 0, 0},  //TV = 7.27(164 lines)  AV=2.97  SV=4.44  BV=5.80
    {6906,1140,1024, 0, 0, 0},  //TV = 7.18(175 lines)  AV=2.97  SV=4.44  BV=5.71
    {7419,1140,1024, 0, 0, 0},  //TV = 7.07(188 lines)  AV=2.97  SV=4.44  BV=5.60
    {7971,1140,1024, 0, 0, 0},  //TV = 6.97(202 lines)  AV=2.97  SV=4.44  BV=5.50
    {8484,1140,1024, 0, 0, 0},  //TV = 6.88(215 lines)  AV=2.97  SV=4.44  BV=5.41
    {9155,1140,1024, 0, 0, 0},  //TV = 6.77(232 lines)  AV=2.97  SV=4.44  BV=5.30
    {9826,1140,1024, 0, 0, 0},  //TV = 6.67(249 lines)  AV=2.97  SV=4.44  BV=5.20
    {9983,1140,1072, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=4.51  BV=5.11
    {9983,1216,1072, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=4.60  BV=5.02
    {9983,1328,1056, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=4.71  BV=4.91
    {9983,1424,1056, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=4.81  BV=4.81
    {9983,1536,1048, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=4.91  BV=4.71
    {9983,1632,1064, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=5.01  BV=4.60
    {9983,1728,1072, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=5.11  BV=4.51
    {9983,1936,1024, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=5.21  BV=4.41
    {9983,2048,1040, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=5.31  BV=4.31
    {9983,2144,1064, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=5.41  BV=4.21
    {20006,1140,1064, 0, 0, 0},  //TV = 5.64(507 lines)  AV=2.97  SV=4.50  BV=4.12
    {20006,1216,1072, 0, 0, 0},  //TV = 5.64(507 lines)  AV=2.97  SV=4.60  BV=4.01
    {20006,1328,1048, 0, 0, 0},  //TV = 5.64(507 lines)  AV=2.97  SV=4.70  BV=3.92
    {20006,1424,1064, 0, 0, 0},  //TV = 5.64(507 lines)  AV=2.97  SV=4.82  BV=3.80
    {20006,1536,1048, 0, 0, 0},  //TV = 5.64(507 lines)  AV=2.97  SV=4.91  BV=3.71
    {20006,1632,1056, 0, 0, 0},  //TV = 5.64(507 lines)  AV=2.97  SV=5.00  BV=3.61
    {29989,1140,1080, 0, 0, 0},  //TV = 5.06(760 lines)  AV=2.97  SV=4.52  BV=3.51
    {29989,1216,1096, 0, 0, 0},  //TV = 5.06(760 lines)  AV=2.97  SV=4.63  BV=3.40
    {29989,1328,1064, 0, 0, 0},  //TV = 5.06(760 lines)  AV=2.97  SV=4.72  BV=3.31
    {29989,1424,1072, 0, 0, 0},  //TV = 5.06(760 lines)  AV=2.97  SV=4.83  BV=3.20
    {40011,1140,1064, 0, 0, 0},  //TV = 4.64(1014 lines)  AV=2.97  SV=4.50  BV=3.12
    {40011,1216,1072, 0, 0, 0},  //TV = 4.64(1014 lines)  AV=2.97  SV=4.60  BV=3.01
    {40011,1328,1048, 0, 0, 0},  //TV = 4.64(1014 lines)  AV=2.97  SV=4.70  BV=2.92
    {49994,1140,1056, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.49  BV=2.81
    {49994,1216,1064, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.59  BV=2.70
    {49994,1328,1040, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.68  BV=2.61
    {49994,1424,1040, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.78  BV=2.51
    {49994,1536,1032, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.88  BV=2.41
    {49994,1632,1040, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.98  BV=2.31
    {49994,1728,1056, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.09  BV=2.21
    {49994,1840,1064, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.19  BV=2.11
    {49994,2048,1024, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.29  BV=2.01
    {49994,2144,1048, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.39  BV=1.91
    {49994,2240,1072, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.48  BV=1.81
    {49994,2448,1048, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.58  BV=1.72
    {49994,2656,1040, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.68  BV=1.61
    {49994,2864,1032, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.78  BV=1.51
    {49994,3072,1032, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.88  BV=1.41
    {49994,3328,1024, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.99  BV=1.31
    {49994,3456,1056, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=6.09  BV=1.21
    {60017,3072,1064, 0, 0, 0},  //TV = 4.06(1521 lines)  AV=2.97  SV=5.93  BV=1.10
    {60017,3328,1048, 0, 0, 0},  //TV = 4.06(1521 lines)  AV=2.97  SV=6.02  BV=1.01
    {69999,3072,1048, 0, 0, 0},  //TV = 3.84(1774 lines)  AV=2.97  SV=5.91  BV=0.90
    {69999,3328,1032, 0, 0, 0},  //TV = 3.84(1774 lines)  AV=2.97  SV=6.00  BV=0.81
    {79982,3072,1048, 0, 0, 0},  //TV = 3.64(2027 lines)  AV=2.97  SV=5.91  BV=0.71
    {79982,3328,1032, 0, 0, 0},  //TV = 3.64(2027 lines)  AV=2.97  SV=6.00  BV=0.62
    {79982,3584,1032, 0, 0, 0},  //TV = 3.64(2027 lines)  AV=2.97  SV=6.11  BV=0.51
    {79982,3840,1032, 0, 0, 0},  //TV = 3.64(2027 lines)  AV=2.97  SV=6.20  BV=0.41
    {79982,4096,1040, 0, 0, 0},  //TV = 3.64(2027 lines)  AV=2.97  SV=6.31  BV=0.31
    {79982,4352,1040, 0, 0, 0},  //TV = 3.64(2027 lines)  AV=2.97  SV=6.40  BV=0.22
    {90005,4224,1024, 0, 0, 0},  //TV = 3.47(2281 lines)  AV=2.97  SV=6.33  BV=0.11
    {99987,4096,1032, 0, 0, 0},  //TV = 3.32(2534 lines)  AV=2.97  SV=6.30  BV=-0.01
    {99987,4352,1040, 0, 0, 0},  //TV = 3.32(2534 lines)  AV=2.97  SV=6.40  BV=-0.10
    {110010,4224,1040, 0, 0, 0},  //TV = 3.18(2788 lines)  AV=2.97  SV=6.35  BV=-0.20
    {119992,4096,1056, 0, 0, 0},  //TV = 3.06(3041 lines)  AV=2.97  SV=6.33  BV=-0.30
    {119992,4480,1032, 0, 0, 0},  //TV = 3.06(3041 lines)  AV=2.97  SV=6.43  BV=-0.40
    {119992,4736,1048, 0, 0, 0},  //TV = 3.06(3041 lines)  AV=2.97  SV=6.53  BV=-0.50
    {119992,5120,1040, 0, 0, 0},  //TV = 3.06(3041 lines)  AV=2.97  SV=6.63  BV=-0.60
    {119992,5504,1040, 0, 0, 0},  //TV = 3.06(3041 lines)  AV=2.97  SV=6.74  BV=-0.71
    {119992,5888,1040, 0, 0, 0},  //TV = 3.06(3041 lines)  AV=2.97  SV=6.83  BV=-0.80
    {119992,6144,1064, 0, 0, 0},  //TV = 3.06(3041 lines)  AV=2.97  SV=6.93  BV=-0.90
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
},
};

static strAETable g_AE_Custom3AutoTable =
{
    AETABLE_CUSTOM3_AUTO,    //eAETableID
    119,    //u4TotalIndex
    20,    //u4StrobeTrigerBV
    109,    //i4MaxBV
    -9,    //i4MinBV
    90,    //i4EffectiveMaxBV
    0,      //i4EffectiveMinBV
    LIB3A_AE_ISO_SPEED_AUTO, //ISO SPEED
    sCustom3PLineTable_60Hz,
    sCustom3PLineTable_50Hz,
    NULL,
};

static strEvPline sCustom4PLineTable_60Hz =
{
{
    {198,1140,1024, 0, 0, 0},  //TV = 12.30(5 lines)  AV=2.97  SV=4.44  BV=10.83
    {198,1140,1040, 0, 0, 0},  //TV = 12.30(5 lines)  AV=2.97  SV=4.46  BV=10.81
    {198,1216,1048, 0, 0, 0},  //TV = 12.30(5 lines)  AV=2.97  SV=4.57  BV=10.70
    {198,1328,1024, 0, 0, 0},  //TV = 12.30(5 lines)  AV=2.97  SV=4.66  BV=10.61
    {237,1140,1064, 0, 0, 0},  //TV = 12.04(6 lines)  AV=2.97  SV=4.50  BV=10.52
    {237,1216,1072, 0, 0, 0},  //TV = 12.04(6 lines)  AV=2.97  SV=4.60  BV=10.41
    {277,1140,1056, 0, 0, 0},  //TV = 11.82(7 lines)  AV=2.97  SV=4.49  BV=10.30
    {277,1216,1056, 0, 0, 0},  //TV = 11.82(7 lines)  AV=2.97  SV=4.58  BV=10.21
    {316,1140,1056, 0, 0, 0},  //TV = 11.63(8 lines)  AV=2.97  SV=4.49  BV=10.11
    {316,1216,1064, 0, 0, 0},  //TV = 11.63(8 lines)  AV=2.97  SV=4.59  BV=10.01
    {356,1140,1080, 0, 0, 0},  //TV = 11.46(9 lines)  AV=2.97  SV=4.52  BV=9.91
    {395,1140,1040, 0, 0, 0},  //TV = 11.31(10 lines)  AV=2.97  SV=4.46  BV=9.81
    {435,1140,1024, 0, 0, 0},  //TV = 11.17(11 lines)  AV=2.97  SV=4.44  BV=9.70
    {435,1140,1088, 0, 0, 0},  //TV = 11.17(11 lines)  AV=2.97  SV=4.53  BV=9.61
    {474,1140,1064, 0, 0, 0},  //TV = 11.04(12 lines)  AV=2.97  SV=4.50  BV=9.52
    {513,1140,1056, 0, 0, 0},  //TV = 10.93(13 lines)  AV=2.97  SV=4.49  BV=9.41
    {553,1140,1056, 0, 0, 0},  //TV = 10.82(14 lines)  AV=2.97  SV=4.49  BV=9.31
    {592,1140,1056, 0, 0, 0},  //TV = 10.72(15 lines)  AV=2.97  SV=4.49  BV=9.21
    {632,1140,1056, 0, 0, 0},  //TV = 10.63(16 lines)  AV=2.97  SV=4.49  BV=9.11
    {671,1140,1064, 0, 0, 0},  //TV = 10.54(17 lines)  AV=2.97  SV=4.50  BV=9.02
    {750,1140,1024, 0, 0, 0},  //TV = 10.38(19 lines)  AV=2.97  SV=4.44  BV=8.91
    {790,1140,1040, 0, 0, 0},  //TV = 10.31(20 lines)  AV=2.97  SV=4.46  BV=8.81
    {869,1140,1024, 0, 0, 0},  //TV = 10.17(22 lines)  AV=2.97  SV=4.44  BV=8.70
    {908,1140,1040, 0, 0, 0},  //TV = 10.11(23 lines)  AV=2.97  SV=4.46  BV=8.61
    {987,1140,1024, 0, 0, 0},  //TV = 9.98(25 lines)  AV=2.97  SV=4.44  BV=8.51
    {1066,1140,1024, 0, 0, 0},  //TV = 9.87(27 lines)  AV=2.97  SV=4.44  BV=8.40
    {1105,1140,1056, 0, 0, 0},  //TV = 9.82(28 lines)  AV=2.97  SV=4.49  BV=8.31
    {1224,1140,1024, 0, 0, 0},  //TV = 9.67(31 lines)  AV=2.97  SV=4.44  BV=8.20
    {1303,1140,1024, 0, 0, 0},  //TV = 9.58(33 lines)  AV=2.97  SV=4.44  BV=8.11
    {1382,1140,1040, 0, 0, 0},  //TV = 9.50(35 lines)  AV=2.97  SV=4.46  BV=8.01
    {1500,1140,1024, 0, 0, 0},  //TV = 9.38(38 lines)  AV=2.97  SV=4.44  BV=7.91
    {1618,1140,1024, 0, 0, 0},  //TV = 9.27(41 lines)  AV=2.97  SV=4.44  BV=7.80
    {1697,1140,1040, 0, 0, 0},  //TV = 9.20(43 lines)  AV=2.97  SV=4.46  BV=7.71
    {1855,1140,1024, 0, 0, 0},  //TV = 9.07(47 lines)  AV=2.97  SV=4.44  BV=7.60
    {1973,1140,1024, 0, 0, 0},  //TV = 8.99(50 lines)  AV=2.97  SV=4.44  BV=7.51
    {2131,1140,1024, 0, 0, 0},  //TV = 8.87(54 lines)  AV=2.97  SV=4.44  BV=7.40
    {2250,1140,1032, 0, 0, 0},  //TV = 8.80(57 lines)  AV=2.97  SV=4.45  BV=7.31
    {2447,1140,1024, 0, 0, 0},  //TV = 8.67(62 lines)  AV=2.97  SV=4.44  BV=7.20
    {2605,1140,1024, 0, 0, 0},  //TV = 8.58(66 lines)  AV=2.97  SV=4.44  BV=7.11
    {2802,1140,1024, 0, 0, 0},  //TV = 8.48(71 lines)  AV=2.97  SV=4.44  BV=7.01
    {2999,1140,1024, 0, 0, 0},  //TV = 8.38(76 lines)  AV=2.97  SV=4.44  BV=6.91
    {3236,1140,1024, 0, 0, 0},  //TV = 8.27(82 lines)  AV=2.97  SV=4.44  BV=6.80
    {3473,1140,1024, 0, 0, 0},  //TV = 8.17(88 lines)  AV=2.97  SV=4.44  BV=6.70
    {3710,1140,1024, 0, 0, 0},  //TV = 8.07(94 lines)  AV=2.97  SV=4.44  BV=6.60
    {3986,1140,1024, 0, 0, 0},  //TV = 7.97(101 lines)  AV=2.97  SV=4.44  BV=6.50
    {4262,1140,1024, 0, 0, 0},  //TV = 7.87(108 lines)  AV=2.97  SV=4.44  BV=6.40
    {4538,1140,1024, 0, 0, 0},  //TV = 7.78(115 lines)  AV=2.97  SV=4.44  BV=6.31
    {4893,1140,1024, 0, 0, 0},  //TV = 7.68(124 lines)  AV=2.97  SV=4.44  BV=6.20
    {5209,1140,1024, 0, 0, 0},  //TV = 7.58(132 lines)  AV=2.97  SV=4.44  BV=6.11
    {5643,1140,1024, 0, 0, 0},  //TV = 7.47(143 lines)  AV=2.97  SV=4.44  BV=6.00
    {6038,1140,1024, 0, 0, 0},  //TV = 7.37(153 lines)  AV=2.97  SV=4.44  BV=5.90
    {6472,1140,1024, 0, 0, 0},  //TV = 7.27(164 lines)  AV=2.97  SV=4.44  BV=5.80
    {6906,1140,1024, 0, 0, 0},  //TV = 7.18(175 lines)  AV=2.97  SV=4.44  BV=5.71
    {7419,1140,1024, 0, 0, 0},  //TV = 7.07(188 lines)  AV=2.97  SV=4.44  BV=5.60
    {7971,1140,1024, 0, 0, 0},  //TV = 6.97(202 lines)  AV=2.97  SV=4.44  BV=5.50
    {8326,1140,1048, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=4.48  BV=5.40
    {8326,1216,1048, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=4.57  BV=5.31
    {8326,1328,1024, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=4.66  BV=5.22
    {8326,1424,1024, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=4.76  BV=5.12
    {8326,1536,1024, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=4.87  BV=5.01
    {8326,1632,1032, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=4.97  BV=4.91
    {8326,1728,1040, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=5.06  BV=4.81
    {8326,1840,1048, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=5.17  BV=4.71
    {8326,1936,1072, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=5.27  BV=4.61
    {8326,2144,1032, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=5.36  BV=4.51
    {16652,1140,1048, 0, 0, 0},  //TV = 5.91(422 lines)  AV=2.97  SV=4.48  BV=4.40
    {16652,1216,1048, 0, 0, 0},  //TV = 5.91(422 lines)  AV=2.97  SV=4.57  BV=4.31
    {16652,1328,1024, 0, 0, 0},  //TV = 5.91(422 lines)  AV=2.97  SV=4.66  BV=4.22
    {16652,1424,1024, 0, 0, 0},  //TV = 5.91(422 lines)  AV=2.97  SV=4.76  BV=4.12
    {16652,1424,1104, 0, 0, 0},  //TV = 5.91(422 lines)  AV=2.97  SV=4.87  BV=4.01
    {16652,1632,1032, 0, 0, 0},  //TV = 5.91(422 lines)  AV=2.97  SV=4.97  BV=3.91
    {25017,1140,1056, 0, 0, 0},  //TV = 5.32(634 lines)  AV=2.97  SV=4.49  BV=3.81
    {25017,1216,1064, 0, 0, 0},  //TV = 5.32(634 lines)  AV=2.97  SV=4.59  BV=3.70
    {25017,1328,1048, 0, 0, 0},  //TV = 5.32(634 lines)  AV=2.97  SV=4.70  BV=3.60
    {25017,1424,1040, 0, 0, 0},  //TV = 5.32(634 lines)  AV=2.97  SV=4.78  BV=3.51
    {33343,1140,1048, 0, 0, 0},  //TV = 4.91(845 lines)  AV=2.97  SV=4.48  BV=3.40
    {33343,1216,1048, 0, 0, 0},  //TV = 4.91(845 lines)  AV=2.97  SV=4.57  BV=3.31
    {33343,1328,1024, 0, 0, 0},  //TV = 4.91(845 lines)  AV=2.97  SV=4.66  BV=3.22
    {41669,1140,1024, 0, 0, 0},  //TV = 4.58(1056 lines)  AV=2.97  SV=4.44  BV=3.11
    {41669,1216,1032, 0, 0, 0},  //TV = 4.58(1056 lines)  AV=2.97  SV=4.55  BV=3.01
    {41669,1216,1104, 0, 0, 0},  //TV = 4.58(1056 lines)  AV=2.97  SV=4.64  BV=2.91
    {49994,1140,1056, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.49  BV=2.81
    {49994,1216,1064, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.59  BV=2.70
    {49994,1328,1040, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.68  BV=2.61
    {49994,1424,1040, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.78  BV=2.51
    {49994,1536,1032, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.88  BV=2.41
    {49994,1632,1040, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.98  BV=2.31
    {49994,1728,1056, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.09  BV=2.21
    {49994,1840,1064, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.19  BV=2.11
    {49994,2048,1024, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.29  BV=2.01
    {49994,2144,1048, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.39  BV=1.91
    {49994,2240,1072, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.48  BV=1.81
    {49994,2448,1048, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.58  BV=1.72
    {49994,2656,1040, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.68  BV=1.61
    {49994,2864,1032, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.78  BV=1.51
    {49994,3072,1032, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.88  BV=1.41
    {49994,3328,1024, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.99  BV=1.31
    {49994,3456,1056, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=6.09  BV=1.21
    {58319,3200,1048, 0, 0, 0},  //TV = 4.10(1478 lines)  AV=2.97  SV=5.96  BV=1.11
    {66645,3072,1024, 0, 0, 0},  //TV = 3.91(1689 lines)  AV=2.97  SV=5.87  BV=1.01
    {66645,3200,1056, 0, 0, 0},  //TV = 3.91(1689 lines)  AV=2.97  SV=5.98  BV=0.90
    {75011,3072,1048, 0, 0, 0},  //TV = 3.74(1901 lines)  AV=2.97  SV=5.91  BV=0.80
    {75011,3328,1032, 0, 0, 0},  //TV = 3.74(1901 lines)  AV=2.97  SV=6.00  BV=0.71
    {83336,3200,1032, 0, 0, 0},  //TV = 3.58(2112 lines)  AV=2.97  SV=5.94  BV=0.61
    {83336,3456,1024, 0, 0, 0},  //TV = 3.58(2112 lines)  AV=2.97  SV=6.04  BV=0.51
    {83336,3712,1024, 0, 0, 0},  //TV = 3.58(2112 lines)  AV=2.97  SV=6.14  BV=0.41
    {83336,3968,1032, 0, 0, 0},  //TV = 3.58(2112 lines)  AV=2.97  SV=6.25  BV=0.30
    {83336,4224,1032, 0, 0, 0},  //TV = 3.58(2112 lines)  AV=2.97  SV=6.34  BV=0.21
    {91661,4096,1040, 0, 0, 0},  //TV = 3.45(2323 lines)  AV=2.97  SV=6.31  BV=0.11
    {99987,4096,1032, 0, 0, 0},  //TV = 3.32(2534 lines)  AV=2.97  SV=6.30  BV=-0.01
    {108313,4096,1024, 0, 0, 0},  //TV = 3.21(2745 lines)  AV=2.97  SV=6.29  BV=-0.11
    {108313,4352,1032, 0, 0, 0},  //TV = 3.21(2745 lines)  AV=2.97  SV=6.39  BV=-0.21
    {116678,4224,1048, 0, 0, 0},  //TV = 3.10(2957 lines)  AV=2.97  SV=6.36  BV=-0.29
    {125004,4224,1048, 0, 0, 0},  //TV = 3.00(3168 lines)  AV=2.97  SV=6.36  BV=-0.39
    {125004,4608,1032, 0, 0, 0},  //TV = 3.00(3168 lines)  AV=2.97  SV=6.47  BV=-0.50
    {125004,4864,1048, 0, 0, 0},  //TV = 3.00(3168 lines)  AV=2.97  SV=6.57  BV=-0.60
    {125004,5376,1024, 0, 0, 0},  //TV = 3.00(3168 lines)  AV=2.97  SV=6.68  BV=-0.71
    {125004,5632,1040, 0, 0, 0},  //TV = 3.00(3168 lines)  AV=2.97  SV=6.77  BV=-0.80
    {125004,6016,1040, 0, 0, 0},  //TV = 3.00(3168 lines)  AV=2.97  SV=6.86  BV=-0.89
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
},
};

static strEvPline sCustom4PLineTable_50Hz =
{
{
    {198,1140,1024, 0, 0, 0},  //TV = 12.30(5 lines)  AV=2.97  SV=4.44  BV=10.83
    {198,1140,1040, 0, 0, 0},  //TV = 12.30(5 lines)  AV=2.97  SV=4.46  BV=10.81
    {198,1216,1048, 0, 0, 0},  //TV = 12.30(5 lines)  AV=2.97  SV=4.57  BV=10.70
    {198,1328,1024, 0, 0, 0},  //TV = 12.30(5 lines)  AV=2.97  SV=4.66  BV=10.61
    {237,1140,1064, 0, 0, 0},  //TV = 12.04(6 lines)  AV=2.97  SV=4.50  BV=10.52
    {237,1216,1072, 0, 0, 0},  //TV = 12.04(6 lines)  AV=2.97  SV=4.60  BV=10.41
    {277,1140,1056, 0, 0, 0},  //TV = 11.82(7 lines)  AV=2.97  SV=4.49  BV=10.30
    {277,1216,1056, 0, 0, 0},  //TV = 11.82(7 lines)  AV=2.97  SV=4.58  BV=10.21
    {316,1140,1056, 0, 0, 0},  //TV = 11.63(8 lines)  AV=2.97  SV=4.49  BV=10.11
    {316,1216,1064, 0, 0, 0},  //TV = 11.63(8 lines)  AV=2.97  SV=4.59  BV=10.01
    {356,1140,1080, 0, 0, 0},  //TV = 11.46(9 lines)  AV=2.97  SV=4.52  BV=9.91
    {395,1140,1040, 0, 0, 0},  //TV = 11.31(10 lines)  AV=2.97  SV=4.46  BV=9.81
    {435,1140,1024, 0, 0, 0},  //TV = 11.17(11 lines)  AV=2.97  SV=4.44  BV=9.70
    {435,1140,1088, 0, 0, 0},  //TV = 11.17(11 lines)  AV=2.97  SV=4.53  BV=9.61
    {474,1140,1064, 0, 0, 0},  //TV = 11.04(12 lines)  AV=2.97  SV=4.50  BV=9.52
    {513,1140,1056, 0, 0, 0},  //TV = 10.93(13 lines)  AV=2.97  SV=4.49  BV=9.41
    {553,1140,1056, 0, 0, 0},  //TV = 10.82(14 lines)  AV=2.97  SV=4.49  BV=9.31
    {592,1140,1056, 0, 0, 0},  //TV = 10.72(15 lines)  AV=2.97  SV=4.49  BV=9.21
    {632,1140,1056, 0, 0, 0},  //TV = 10.63(16 lines)  AV=2.97  SV=4.49  BV=9.11
    {671,1140,1064, 0, 0, 0},  //TV = 10.54(17 lines)  AV=2.97  SV=4.50  BV=9.02
    {750,1140,1024, 0, 0, 0},  //TV = 10.38(19 lines)  AV=2.97  SV=4.44  BV=8.91
    {790,1140,1040, 0, 0, 0},  //TV = 10.31(20 lines)  AV=2.97  SV=4.46  BV=8.81
    {869,1140,1024, 0, 0, 0},  //TV = 10.17(22 lines)  AV=2.97  SV=4.44  BV=8.70
    {908,1140,1040, 0, 0, 0},  //TV = 10.11(23 lines)  AV=2.97  SV=4.46  BV=8.61
    {987,1140,1024, 0, 0, 0},  //TV = 9.98(25 lines)  AV=2.97  SV=4.44  BV=8.51
    {1066,1140,1024, 0, 0, 0},  //TV = 9.87(27 lines)  AV=2.97  SV=4.44  BV=8.40
    {1105,1140,1056, 0, 0, 0},  //TV = 9.82(28 lines)  AV=2.97  SV=4.49  BV=8.31
    {1224,1140,1024, 0, 0, 0},  //TV = 9.67(31 lines)  AV=2.97  SV=4.44  BV=8.20
    {1303,1140,1024, 0, 0, 0},  //TV = 9.58(33 lines)  AV=2.97  SV=4.44  BV=8.11
    {1382,1140,1040, 0, 0, 0},  //TV = 9.50(35 lines)  AV=2.97  SV=4.46  BV=8.01
    {1500,1140,1024, 0, 0, 0},  //TV = 9.38(38 lines)  AV=2.97  SV=4.44  BV=7.91
    {1618,1140,1024, 0, 0, 0},  //TV = 9.27(41 lines)  AV=2.97  SV=4.44  BV=7.80
    {1697,1140,1040, 0, 0, 0},  //TV = 9.20(43 lines)  AV=2.97  SV=4.46  BV=7.71
    {1855,1140,1024, 0, 0, 0},  //TV = 9.07(47 lines)  AV=2.97  SV=4.44  BV=7.60
    {1973,1140,1024, 0, 0, 0},  //TV = 8.99(50 lines)  AV=2.97  SV=4.44  BV=7.51
    {2131,1140,1024, 0, 0, 0},  //TV = 8.87(54 lines)  AV=2.97  SV=4.44  BV=7.40
    {2250,1140,1032, 0, 0, 0},  //TV = 8.80(57 lines)  AV=2.97  SV=4.45  BV=7.31
    {2447,1140,1024, 0, 0, 0},  //TV = 8.67(62 lines)  AV=2.97  SV=4.44  BV=7.20
    {2605,1140,1024, 0, 0, 0},  //TV = 8.58(66 lines)  AV=2.97  SV=4.44  BV=7.11
    {2802,1140,1024, 0, 0, 0},  //TV = 8.48(71 lines)  AV=2.97  SV=4.44  BV=7.01
    {2999,1140,1024, 0, 0, 0},  //TV = 8.38(76 lines)  AV=2.97  SV=4.44  BV=6.91
    {3236,1140,1024, 0, 0, 0},  //TV = 8.27(82 lines)  AV=2.97  SV=4.44  BV=6.80
    {3473,1140,1024, 0, 0, 0},  //TV = 8.17(88 lines)  AV=2.97  SV=4.44  BV=6.70
    {3710,1140,1024, 0, 0, 0},  //TV = 8.07(94 lines)  AV=2.97  SV=4.44  BV=6.60
    {3986,1140,1024, 0, 0, 0},  //TV = 7.97(101 lines)  AV=2.97  SV=4.44  BV=6.50
    {4262,1140,1024, 0, 0, 0},  //TV = 7.87(108 lines)  AV=2.97  SV=4.44  BV=6.40
    {4538,1140,1024, 0, 0, 0},  //TV = 7.78(115 lines)  AV=2.97  SV=4.44  BV=6.31
    {4893,1140,1024, 0, 0, 0},  //TV = 7.68(124 lines)  AV=2.97  SV=4.44  BV=6.20
    {5209,1140,1024, 0, 0, 0},  //TV = 7.58(132 lines)  AV=2.97  SV=4.44  BV=6.11
    {5643,1140,1024, 0, 0, 0},  //TV = 7.47(143 lines)  AV=2.97  SV=4.44  BV=6.00
    {6038,1140,1024, 0, 0, 0},  //TV = 7.37(153 lines)  AV=2.97  SV=4.44  BV=5.90
    {6472,1140,1024, 0, 0, 0},  //TV = 7.27(164 lines)  AV=2.97  SV=4.44  BV=5.80
    {6906,1140,1024, 0, 0, 0},  //TV = 7.18(175 lines)  AV=2.97  SV=4.44  BV=5.71
    {7419,1140,1024, 0, 0, 0},  //TV = 7.07(188 lines)  AV=2.97  SV=4.44  BV=5.60
    {7971,1140,1024, 0, 0, 0},  //TV = 6.97(202 lines)  AV=2.97  SV=4.44  BV=5.50
    {8484,1140,1024, 0, 0, 0},  //TV = 6.88(215 lines)  AV=2.97  SV=4.44  BV=5.41
    {9155,1140,1024, 0, 0, 0},  //TV = 6.77(232 lines)  AV=2.97  SV=4.44  BV=5.30
    {9826,1140,1024, 0, 0, 0},  //TV = 6.67(249 lines)  AV=2.97  SV=4.44  BV=5.20
    {9983,1140,1072, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=4.51  BV=5.11
    {9983,1216,1072, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=4.60  BV=5.02
    {9983,1328,1056, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=4.71  BV=4.91
    {9983,1424,1056, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=4.81  BV=4.81
    {9983,1536,1048, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=4.91  BV=4.71
    {9983,1632,1064, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=5.01  BV=4.60
    {9983,1728,1072, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=5.11  BV=4.51
    {9983,1936,1024, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=5.21  BV=4.41
    {9983,2048,1040, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=5.31  BV=4.31
    {9983,2144,1064, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=5.41  BV=4.21
    {20006,1140,1064, 0, 0, 0},  //TV = 5.64(507 lines)  AV=2.97  SV=4.50  BV=4.12
    {20006,1216,1072, 0, 0, 0},  //TV = 5.64(507 lines)  AV=2.97  SV=4.60  BV=4.01
    {20006,1328,1048, 0, 0, 0},  //TV = 5.64(507 lines)  AV=2.97  SV=4.70  BV=3.92
    {20006,1424,1064, 0, 0, 0},  //TV = 5.64(507 lines)  AV=2.97  SV=4.82  BV=3.80
    {20006,1536,1048, 0, 0, 0},  //TV = 5.64(507 lines)  AV=2.97  SV=4.91  BV=3.71
    {20006,1632,1056, 0, 0, 0},  //TV = 5.64(507 lines)  AV=2.97  SV=5.00  BV=3.61
    {29989,1140,1080, 0, 0, 0},  //TV = 5.06(760 lines)  AV=2.97  SV=4.52  BV=3.51
    {29989,1216,1096, 0, 0, 0},  //TV = 5.06(760 lines)  AV=2.97  SV=4.63  BV=3.40
    {29989,1328,1064, 0, 0, 0},  //TV = 5.06(760 lines)  AV=2.97  SV=4.72  BV=3.31
    {29989,1424,1072, 0, 0, 0},  //TV = 5.06(760 lines)  AV=2.97  SV=4.83  BV=3.20
    {40011,1140,1064, 0, 0, 0},  //TV = 4.64(1014 lines)  AV=2.97  SV=4.50  BV=3.12
    {40011,1216,1072, 0, 0, 0},  //TV = 4.64(1014 lines)  AV=2.97  SV=4.60  BV=3.01
    {40011,1328,1048, 0, 0, 0},  //TV = 4.64(1014 lines)  AV=2.97  SV=4.70  BV=2.92
    {49994,1140,1056, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.49  BV=2.81
    {49994,1216,1064, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.59  BV=2.70
    {49994,1328,1040, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.68  BV=2.61
    {49994,1424,1040, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.78  BV=2.51
    {49994,1536,1032, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.88  BV=2.41
    {49994,1632,1040, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.98  BV=2.31
    {49994,1728,1056, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.09  BV=2.21
    {49994,1840,1064, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.19  BV=2.11
    {49994,2048,1024, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.29  BV=2.01
    {49994,2144,1048, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.39  BV=1.91
    {49994,2240,1072, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.48  BV=1.81
    {49994,2448,1048, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.58  BV=1.72
    {49994,2656,1040, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.68  BV=1.61
    {49994,2864,1032, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.78  BV=1.51
    {49994,3072,1032, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.88  BV=1.41
    {49994,3328,1024, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.99  BV=1.31
    {49994,3456,1056, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=6.09  BV=1.21
    {60017,3072,1064, 0, 0, 0},  //TV = 4.06(1521 lines)  AV=2.97  SV=5.93  BV=1.10
    {60017,3328,1048, 0, 0, 0},  //TV = 4.06(1521 lines)  AV=2.97  SV=6.02  BV=1.01
    {69999,3072,1048, 0, 0, 0},  //TV = 3.84(1774 lines)  AV=2.97  SV=5.91  BV=0.90
    {69999,3328,1032, 0, 0, 0},  //TV = 3.84(1774 lines)  AV=2.97  SV=6.00  BV=0.81
    {79982,3072,1048, 0, 0, 0},  //TV = 3.64(2027 lines)  AV=2.97  SV=5.91  BV=0.71
    {79982,3328,1032, 0, 0, 0},  //TV = 3.64(2027 lines)  AV=2.97  SV=6.00  BV=0.62
    {79982,3584,1032, 0, 0, 0},  //TV = 3.64(2027 lines)  AV=2.97  SV=6.11  BV=0.51
    {79982,3840,1032, 0, 0, 0},  //TV = 3.64(2027 lines)  AV=2.97  SV=6.20  BV=0.41
    {79982,4096,1040, 0, 0, 0},  //TV = 3.64(2027 lines)  AV=2.97  SV=6.31  BV=0.31
    {79982,4352,1040, 0, 0, 0},  //TV = 3.64(2027 lines)  AV=2.97  SV=6.40  BV=0.22
    {90005,4224,1024, 0, 0, 0},  //TV = 3.47(2281 lines)  AV=2.97  SV=6.33  BV=0.11
    {99987,4096,1032, 0, 0, 0},  //TV = 3.32(2534 lines)  AV=2.97  SV=6.30  BV=-0.01
    {99987,4352,1040, 0, 0, 0},  //TV = 3.32(2534 lines)  AV=2.97  SV=6.40  BV=-0.10
    {110010,4224,1040, 0, 0, 0},  //TV = 3.18(2788 lines)  AV=2.97  SV=6.35  BV=-0.20
    {119992,4096,1056, 0, 0, 0},  //TV = 3.06(3041 lines)  AV=2.97  SV=6.33  BV=-0.30
    {119992,4480,1032, 0, 0, 0},  //TV = 3.06(3041 lines)  AV=2.97  SV=6.43  BV=-0.40
    {119992,4736,1048, 0, 0, 0},  //TV = 3.06(3041 lines)  AV=2.97  SV=6.53  BV=-0.50
    {119992,5120,1040, 0, 0, 0},  //TV = 3.06(3041 lines)  AV=2.97  SV=6.63  BV=-0.60
    {119992,5504,1040, 0, 0, 0},  //TV = 3.06(3041 lines)  AV=2.97  SV=6.74  BV=-0.71
    {119992,5888,1040, 0, 0, 0},  //TV = 3.06(3041 lines)  AV=2.97  SV=6.83  BV=-0.80
    {119992,6144,1064, 0, 0, 0},  //TV = 3.06(3041 lines)  AV=2.97  SV=6.93  BV=-0.90
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
},
};

static strAETable g_AE_Custom4AutoTable =
{
    AETABLE_CUSTOM4_AUTO,    //eAETableID
    119,    //u4TotalIndex
    20,    //u4StrobeTrigerBV
    109,    //i4MaxBV
    -9,    //i4MinBV
    90,    //i4EffectiveMaxBV
    0,      //i4EffectiveMinBV
    LIB3A_AE_ISO_SPEED_AUTO, //ISO SPEED
    sCustom4PLineTable_60Hz,
    sCustom4PLineTable_50Hz,
    NULL,
};

static strEvPline sCustom5PLineTable_60Hz =
{
{
    {198,1140,1024, 0, 0, 0},  //TV = 12.30(5 lines)  AV=2.97  SV=4.44  BV=10.83
    {198,1140,1040, 0, 0, 0},  //TV = 12.30(5 lines)  AV=2.97  SV=4.46  BV=10.81
    {198,1216,1048, 0, 0, 0},  //TV = 12.30(5 lines)  AV=2.97  SV=4.57  BV=10.70
    {198,1328,1024, 0, 0, 0},  //TV = 12.30(5 lines)  AV=2.97  SV=4.66  BV=10.61
    {237,1140,1064, 0, 0, 0},  //TV = 12.04(6 lines)  AV=2.97  SV=4.50  BV=10.52
    {237,1216,1072, 0, 0, 0},  //TV = 12.04(6 lines)  AV=2.97  SV=4.60  BV=10.41
    {277,1140,1056, 0, 0, 0},  //TV = 11.82(7 lines)  AV=2.97  SV=4.49  BV=10.30
    {277,1216,1056, 0, 0, 0},  //TV = 11.82(7 lines)  AV=2.97  SV=4.58  BV=10.21
    {316,1140,1056, 0, 0, 0},  //TV = 11.63(8 lines)  AV=2.97  SV=4.49  BV=10.11
    {316,1216,1064, 0, 0, 0},  //TV = 11.63(8 lines)  AV=2.97  SV=4.59  BV=10.01
    {356,1140,1080, 0, 0, 0},  //TV = 11.46(9 lines)  AV=2.97  SV=4.52  BV=9.91
    {395,1140,1040, 0, 0, 0},  //TV = 11.31(10 lines)  AV=2.97  SV=4.46  BV=9.81
    {435,1140,1024, 0, 0, 0},  //TV = 11.17(11 lines)  AV=2.97  SV=4.44  BV=9.70
    {435,1140,1088, 0, 0, 0},  //TV = 11.17(11 lines)  AV=2.97  SV=4.53  BV=9.61
    {474,1140,1064, 0, 0, 0},  //TV = 11.04(12 lines)  AV=2.97  SV=4.50  BV=9.52
    {513,1140,1056, 0, 0, 0},  //TV = 10.93(13 lines)  AV=2.97  SV=4.49  BV=9.41
    {553,1140,1056, 0, 0, 0},  //TV = 10.82(14 lines)  AV=2.97  SV=4.49  BV=9.31
    {592,1140,1056, 0, 0, 0},  //TV = 10.72(15 lines)  AV=2.97  SV=4.49  BV=9.21
    {632,1140,1056, 0, 0, 0},  //TV = 10.63(16 lines)  AV=2.97  SV=4.49  BV=9.11
    {671,1140,1064, 0, 0, 0},  //TV = 10.54(17 lines)  AV=2.97  SV=4.50  BV=9.02
    {750,1140,1024, 0, 0, 0},  //TV = 10.38(19 lines)  AV=2.97  SV=4.44  BV=8.91
    {790,1140,1040, 0, 0, 0},  //TV = 10.31(20 lines)  AV=2.97  SV=4.46  BV=8.81
    {869,1140,1024, 0, 0, 0},  //TV = 10.17(22 lines)  AV=2.97  SV=4.44  BV=8.70
    {908,1140,1040, 0, 0, 0},  //TV = 10.11(23 lines)  AV=2.97  SV=4.46  BV=8.61
    {987,1140,1024, 0, 0, 0},  //TV = 9.98(25 lines)  AV=2.97  SV=4.44  BV=8.51
    {1066,1140,1024, 0, 0, 0},  //TV = 9.87(27 lines)  AV=2.97  SV=4.44  BV=8.40
    {1105,1140,1056, 0, 0, 0},  //TV = 9.82(28 lines)  AV=2.97  SV=4.49  BV=8.31
    {1224,1140,1024, 0, 0, 0},  //TV = 9.67(31 lines)  AV=2.97  SV=4.44  BV=8.20
    {1303,1140,1024, 0, 0, 0},  //TV = 9.58(33 lines)  AV=2.97  SV=4.44  BV=8.11
    {1382,1140,1040, 0, 0, 0},  //TV = 9.50(35 lines)  AV=2.97  SV=4.46  BV=8.01
    {1500,1140,1024, 0, 0, 0},  //TV = 9.38(38 lines)  AV=2.97  SV=4.44  BV=7.91
    {1618,1140,1024, 0, 0, 0},  //TV = 9.27(41 lines)  AV=2.97  SV=4.44  BV=7.80
    {1697,1140,1040, 0, 0, 0},  //TV = 9.20(43 lines)  AV=2.97  SV=4.46  BV=7.71
    {1855,1140,1024, 0, 0, 0},  //TV = 9.07(47 lines)  AV=2.97  SV=4.44  BV=7.60
    {1973,1140,1024, 0, 0, 0},  //TV = 8.99(50 lines)  AV=2.97  SV=4.44  BV=7.51
    {2131,1140,1024, 0, 0, 0},  //TV = 8.87(54 lines)  AV=2.97  SV=4.44  BV=7.40
    {2250,1140,1032, 0, 0, 0},  //TV = 8.80(57 lines)  AV=2.97  SV=4.45  BV=7.31
    {2447,1140,1024, 0, 0, 0},  //TV = 8.67(62 lines)  AV=2.97  SV=4.44  BV=7.20
    {2605,1140,1024, 0, 0, 0},  //TV = 8.58(66 lines)  AV=2.97  SV=4.44  BV=7.11
    {2802,1140,1024, 0, 0, 0},  //TV = 8.48(71 lines)  AV=2.97  SV=4.44  BV=7.01
    {2999,1140,1024, 0, 0, 0},  //TV = 8.38(76 lines)  AV=2.97  SV=4.44  BV=6.91
    {3236,1140,1024, 0, 0, 0},  //TV = 8.27(82 lines)  AV=2.97  SV=4.44  BV=6.80
    {3473,1140,1024, 0, 0, 0},  //TV = 8.17(88 lines)  AV=2.97  SV=4.44  BV=6.70
    {3710,1140,1024, 0, 0, 0},  //TV = 8.07(94 lines)  AV=2.97  SV=4.44  BV=6.60
    {3986,1140,1024, 0, 0, 0},  //TV = 7.97(101 lines)  AV=2.97  SV=4.44  BV=6.50
    {4262,1140,1024, 0, 0, 0},  //TV = 7.87(108 lines)  AV=2.97  SV=4.44  BV=6.40
    {4538,1140,1024, 0, 0, 0},  //TV = 7.78(115 lines)  AV=2.97  SV=4.44  BV=6.31
    {4893,1140,1024, 0, 0, 0},  //TV = 7.68(124 lines)  AV=2.97  SV=4.44  BV=6.20
    {5209,1140,1024, 0, 0, 0},  //TV = 7.58(132 lines)  AV=2.97  SV=4.44  BV=6.11
    {5643,1140,1024, 0, 0, 0},  //TV = 7.47(143 lines)  AV=2.97  SV=4.44  BV=6.00
    {6038,1140,1024, 0, 0, 0},  //TV = 7.37(153 lines)  AV=2.97  SV=4.44  BV=5.90
    {6472,1140,1024, 0, 0, 0},  //TV = 7.27(164 lines)  AV=2.97  SV=4.44  BV=5.80
    {6906,1140,1024, 0, 0, 0},  //TV = 7.18(175 lines)  AV=2.97  SV=4.44  BV=5.71
    {7419,1140,1024, 0, 0, 0},  //TV = 7.07(188 lines)  AV=2.97  SV=4.44  BV=5.60
    {7971,1140,1024, 0, 0, 0},  //TV = 6.97(202 lines)  AV=2.97  SV=4.44  BV=5.50
    {8326,1140,1048, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=4.48  BV=5.40
    {8326,1216,1048, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=4.57  BV=5.31
    {8326,1328,1024, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=4.66  BV=5.22
    {8326,1424,1024, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=4.76  BV=5.12
    {8326,1536,1024, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=4.87  BV=5.01
    {8326,1632,1032, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=4.97  BV=4.91
    {8326,1728,1040, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=5.06  BV=4.81
    {8326,1840,1048, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=5.17  BV=4.71
    {8326,1936,1072, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=5.27  BV=4.61
    {8326,2144,1032, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=5.36  BV=4.51
    {16652,1140,1048, 0, 0, 0},  //TV = 5.91(422 lines)  AV=2.97  SV=4.48  BV=4.40
    {16652,1216,1048, 0, 0, 0},  //TV = 5.91(422 lines)  AV=2.97  SV=4.57  BV=4.31
    {16652,1328,1024, 0, 0, 0},  //TV = 5.91(422 lines)  AV=2.97  SV=4.66  BV=4.22
    {16652,1424,1024, 0, 0, 0},  //TV = 5.91(422 lines)  AV=2.97  SV=4.76  BV=4.12
    {16652,1424,1104, 0, 0, 0},  //TV = 5.91(422 lines)  AV=2.97  SV=4.87  BV=4.01
    {16652,1632,1032, 0, 0, 0},  //TV = 5.91(422 lines)  AV=2.97  SV=4.97  BV=3.91
    {25017,1140,1056, 0, 0, 0},  //TV = 5.32(634 lines)  AV=2.97  SV=4.49  BV=3.81
    {25017,1216,1064, 0, 0, 0},  //TV = 5.32(634 lines)  AV=2.97  SV=4.59  BV=3.70
    {25017,1328,1048, 0, 0, 0},  //TV = 5.32(634 lines)  AV=2.97  SV=4.70  BV=3.60
    {25017,1424,1040, 0, 0, 0},  //TV = 5.32(634 lines)  AV=2.97  SV=4.78  BV=3.51
    {33343,1140,1048, 0, 0, 0},  //TV = 4.91(845 lines)  AV=2.97  SV=4.48  BV=3.40
    {33343,1216,1048, 0, 0, 0},  //TV = 4.91(845 lines)  AV=2.97  SV=4.57  BV=3.31
    {33343,1328,1024, 0, 0, 0},  //TV = 4.91(845 lines)  AV=2.97  SV=4.66  BV=3.22
    {41669,1140,1024, 0, 0, 0},  //TV = 4.58(1056 lines)  AV=2.97  SV=4.44  BV=3.11
    {41669,1216,1032, 0, 0, 0},  //TV = 4.58(1056 lines)  AV=2.97  SV=4.55  BV=3.01
    {41669,1216,1104, 0, 0, 0},  //TV = 4.58(1056 lines)  AV=2.97  SV=4.64  BV=2.91
    {49994,1140,1056, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.49  BV=2.81
    {49994,1216,1064, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.59  BV=2.70
    {49994,1328,1040, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.68  BV=2.61
    {49994,1424,1040, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.78  BV=2.51
    {49994,1536,1032, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.88  BV=2.41
    {49994,1632,1040, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.98  BV=2.31
    {49994,1728,1056, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.09  BV=2.21
    {49994,1840,1064, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.19  BV=2.11
    {49994,2048,1024, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.29  BV=2.01
    {49994,2144,1048, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.39  BV=1.91
    {49994,2240,1072, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.48  BV=1.81
    {49994,2448,1048, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.58  BV=1.72
    {49994,2656,1040, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.68  BV=1.61
    {49994,2864,1032, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.78  BV=1.51
    {49994,3072,1032, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.88  BV=1.41
    {49994,3328,1024, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.99  BV=1.31
    {49994,3456,1056, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=6.09  BV=1.21
    {58319,3200,1048, 0, 0, 0},  //TV = 4.10(1478 lines)  AV=2.97  SV=5.96  BV=1.11
    {66645,3072,1024, 0, 0, 0},  //TV = 3.91(1689 lines)  AV=2.97  SV=5.87  BV=1.01
    {66645,3200,1056, 0, 0, 0},  //TV = 3.91(1689 lines)  AV=2.97  SV=5.98  BV=0.90
    {75011,3072,1048, 0, 0, 0},  //TV = 3.74(1901 lines)  AV=2.97  SV=5.91  BV=0.80
    {75011,3328,1032, 0, 0, 0},  //TV = 3.74(1901 lines)  AV=2.97  SV=6.00  BV=0.71
    {83336,3200,1032, 0, 0, 0},  //TV = 3.58(2112 lines)  AV=2.97  SV=5.94  BV=0.61
    {83336,3456,1024, 0, 0, 0},  //TV = 3.58(2112 lines)  AV=2.97  SV=6.04  BV=0.51
    {83336,3712,1024, 0, 0, 0},  //TV = 3.58(2112 lines)  AV=2.97  SV=6.14  BV=0.41
    {83336,3968,1032, 0, 0, 0},  //TV = 3.58(2112 lines)  AV=2.97  SV=6.25  BV=0.30
    {83336,4224,1032, 0, 0, 0},  //TV = 3.58(2112 lines)  AV=2.97  SV=6.34  BV=0.21
    {91661,4096,1040, 0, 0, 0},  //TV = 3.45(2323 lines)  AV=2.97  SV=6.31  BV=0.11
    {99987,4096,1032, 0, 0, 0},  //TV = 3.32(2534 lines)  AV=2.97  SV=6.30  BV=-0.01
    {108313,4096,1024, 0, 0, 0},  //TV = 3.21(2745 lines)  AV=2.97  SV=6.29  BV=-0.11
    {108313,4352,1032, 0, 0, 0},  //TV = 3.21(2745 lines)  AV=2.97  SV=6.39  BV=-0.21
    {116678,4224,1048, 0, 0, 0},  //TV = 3.10(2957 lines)  AV=2.97  SV=6.36  BV=-0.29
    {125004,4224,1048, 0, 0, 0},  //TV = 3.00(3168 lines)  AV=2.97  SV=6.36  BV=-0.39
    {125004,4608,1032, 0, 0, 0},  //TV = 3.00(3168 lines)  AV=2.97  SV=6.47  BV=-0.50
    {125004,4864,1048, 0, 0, 0},  //TV = 3.00(3168 lines)  AV=2.97  SV=6.57  BV=-0.60
    {125004,5376,1024, 0, 0, 0},  //TV = 3.00(3168 lines)  AV=2.97  SV=6.68  BV=-0.71
    {125004,5632,1040, 0, 0, 0},  //TV = 3.00(3168 lines)  AV=2.97  SV=6.77  BV=-0.80
    {125004,6016,1040, 0, 0, 0},  //TV = 3.00(3168 lines)  AV=2.97  SV=6.86  BV=-0.89
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
},
};

static strEvPline sCustom5PLineTable_50Hz =
{
{
    {198,1140,1024, 0, 0, 0},  //TV = 12.30(5 lines)  AV=2.97  SV=4.44  BV=10.83
    {198,1140,1040, 0, 0, 0},  //TV = 12.30(5 lines)  AV=2.97  SV=4.46  BV=10.81
    {198,1216,1048, 0, 0, 0},  //TV = 12.30(5 lines)  AV=2.97  SV=4.57  BV=10.70
    {198,1328,1024, 0, 0, 0},  //TV = 12.30(5 lines)  AV=2.97  SV=4.66  BV=10.61
    {237,1140,1064, 0, 0, 0},  //TV = 12.04(6 lines)  AV=2.97  SV=4.50  BV=10.52
    {237,1216,1072, 0, 0, 0},  //TV = 12.04(6 lines)  AV=2.97  SV=4.60  BV=10.41
    {277,1140,1056, 0, 0, 0},  //TV = 11.82(7 lines)  AV=2.97  SV=4.49  BV=10.30
    {277,1216,1056, 0, 0, 0},  //TV = 11.82(7 lines)  AV=2.97  SV=4.58  BV=10.21
    {316,1140,1056, 0, 0, 0},  //TV = 11.63(8 lines)  AV=2.97  SV=4.49  BV=10.11
    {316,1216,1064, 0, 0, 0},  //TV = 11.63(8 lines)  AV=2.97  SV=4.59  BV=10.01
    {356,1140,1080, 0, 0, 0},  //TV = 11.46(9 lines)  AV=2.97  SV=4.52  BV=9.91
    {395,1140,1040, 0, 0, 0},  //TV = 11.31(10 lines)  AV=2.97  SV=4.46  BV=9.81
    {435,1140,1024, 0, 0, 0},  //TV = 11.17(11 lines)  AV=2.97  SV=4.44  BV=9.70
    {435,1140,1088, 0, 0, 0},  //TV = 11.17(11 lines)  AV=2.97  SV=4.53  BV=9.61
    {474,1140,1064, 0, 0, 0},  //TV = 11.04(12 lines)  AV=2.97  SV=4.50  BV=9.52
    {513,1140,1056, 0, 0, 0},  //TV = 10.93(13 lines)  AV=2.97  SV=4.49  BV=9.41
    {553,1140,1056, 0, 0, 0},  //TV = 10.82(14 lines)  AV=2.97  SV=4.49  BV=9.31
    {592,1140,1056, 0, 0, 0},  //TV = 10.72(15 lines)  AV=2.97  SV=4.49  BV=9.21
    {632,1140,1056, 0, 0, 0},  //TV = 10.63(16 lines)  AV=2.97  SV=4.49  BV=9.11
    {671,1140,1064, 0, 0, 0},  //TV = 10.54(17 lines)  AV=2.97  SV=4.50  BV=9.02
    {750,1140,1024, 0, 0, 0},  //TV = 10.38(19 lines)  AV=2.97  SV=4.44  BV=8.91
    {790,1140,1040, 0, 0, 0},  //TV = 10.31(20 lines)  AV=2.97  SV=4.46  BV=8.81
    {869,1140,1024, 0, 0, 0},  //TV = 10.17(22 lines)  AV=2.97  SV=4.44  BV=8.70
    {908,1140,1040, 0, 0, 0},  //TV = 10.11(23 lines)  AV=2.97  SV=4.46  BV=8.61
    {987,1140,1024, 0, 0, 0},  //TV = 9.98(25 lines)  AV=2.97  SV=4.44  BV=8.51
    {1066,1140,1024, 0, 0, 0},  //TV = 9.87(27 lines)  AV=2.97  SV=4.44  BV=8.40
    {1105,1140,1056, 0, 0, 0},  //TV = 9.82(28 lines)  AV=2.97  SV=4.49  BV=8.31
    {1224,1140,1024, 0, 0, 0},  //TV = 9.67(31 lines)  AV=2.97  SV=4.44  BV=8.20
    {1303,1140,1024, 0, 0, 0},  //TV = 9.58(33 lines)  AV=2.97  SV=4.44  BV=8.11
    {1382,1140,1040, 0, 0, 0},  //TV = 9.50(35 lines)  AV=2.97  SV=4.46  BV=8.01
    {1500,1140,1024, 0, 0, 0},  //TV = 9.38(38 lines)  AV=2.97  SV=4.44  BV=7.91
    {1618,1140,1024, 0, 0, 0},  //TV = 9.27(41 lines)  AV=2.97  SV=4.44  BV=7.80
    {1697,1140,1040, 0, 0, 0},  //TV = 9.20(43 lines)  AV=2.97  SV=4.46  BV=7.71
    {1855,1140,1024, 0, 0, 0},  //TV = 9.07(47 lines)  AV=2.97  SV=4.44  BV=7.60
    {1973,1140,1024, 0, 0, 0},  //TV = 8.99(50 lines)  AV=2.97  SV=4.44  BV=7.51
    {2131,1140,1024, 0, 0, 0},  //TV = 8.87(54 lines)  AV=2.97  SV=4.44  BV=7.40
    {2250,1140,1032, 0, 0, 0},  //TV = 8.80(57 lines)  AV=2.97  SV=4.45  BV=7.31
    {2447,1140,1024, 0, 0, 0},  //TV = 8.67(62 lines)  AV=2.97  SV=4.44  BV=7.20
    {2605,1140,1024, 0, 0, 0},  //TV = 8.58(66 lines)  AV=2.97  SV=4.44  BV=7.11
    {2802,1140,1024, 0, 0, 0},  //TV = 8.48(71 lines)  AV=2.97  SV=4.44  BV=7.01
    {2999,1140,1024, 0, 0, 0},  //TV = 8.38(76 lines)  AV=2.97  SV=4.44  BV=6.91
    {3236,1140,1024, 0, 0, 0},  //TV = 8.27(82 lines)  AV=2.97  SV=4.44  BV=6.80
    {3473,1140,1024, 0, 0, 0},  //TV = 8.17(88 lines)  AV=2.97  SV=4.44  BV=6.70
    {3710,1140,1024, 0, 0, 0},  //TV = 8.07(94 lines)  AV=2.97  SV=4.44  BV=6.60
    {3986,1140,1024, 0, 0, 0},  //TV = 7.97(101 lines)  AV=2.97  SV=4.44  BV=6.50
    {4262,1140,1024, 0, 0, 0},  //TV = 7.87(108 lines)  AV=2.97  SV=4.44  BV=6.40
    {4538,1140,1024, 0, 0, 0},  //TV = 7.78(115 lines)  AV=2.97  SV=4.44  BV=6.31
    {4893,1140,1024, 0, 0, 0},  //TV = 7.68(124 lines)  AV=2.97  SV=4.44  BV=6.20
    {5209,1140,1024, 0, 0, 0},  //TV = 7.58(132 lines)  AV=2.97  SV=4.44  BV=6.11
    {5643,1140,1024, 0, 0, 0},  //TV = 7.47(143 lines)  AV=2.97  SV=4.44  BV=6.00
    {6038,1140,1024, 0, 0, 0},  //TV = 7.37(153 lines)  AV=2.97  SV=4.44  BV=5.90
    {6472,1140,1024, 0, 0, 0},  //TV = 7.27(164 lines)  AV=2.97  SV=4.44  BV=5.80
    {6906,1140,1024, 0, 0, 0},  //TV = 7.18(175 lines)  AV=2.97  SV=4.44  BV=5.71
    {7419,1140,1024, 0, 0, 0},  //TV = 7.07(188 lines)  AV=2.97  SV=4.44  BV=5.60
    {7971,1140,1024, 0, 0, 0},  //TV = 6.97(202 lines)  AV=2.97  SV=4.44  BV=5.50
    {8484,1140,1024, 0, 0, 0},  //TV = 6.88(215 lines)  AV=2.97  SV=4.44  BV=5.41
    {9155,1140,1024, 0, 0, 0},  //TV = 6.77(232 lines)  AV=2.97  SV=4.44  BV=5.30
    {9826,1140,1024, 0, 0, 0},  //TV = 6.67(249 lines)  AV=2.97  SV=4.44  BV=5.20
    {9983,1140,1072, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=4.51  BV=5.11
    {9983,1216,1072, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=4.60  BV=5.02
    {9983,1328,1056, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=4.71  BV=4.91
    {9983,1424,1056, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=4.81  BV=4.81
    {9983,1536,1048, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=4.91  BV=4.71
    {9983,1632,1064, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=5.01  BV=4.60
    {9983,1728,1072, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=5.11  BV=4.51
    {9983,1936,1024, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=5.21  BV=4.41
    {9983,2048,1040, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=5.31  BV=4.31
    {9983,2144,1064, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=5.41  BV=4.21
    {20006,1140,1064, 0, 0, 0},  //TV = 5.64(507 lines)  AV=2.97  SV=4.50  BV=4.12
    {20006,1216,1072, 0, 0, 0},  //TV = 5.64(507 lines)  AV=2.97  SV=4.60  BV=4.01
    {20006,1328,1048, 0, 0, 0},  //TV = 5.64(507 lines)  AV=2.97  SV=4.70  BV=3.92
    {20006,1424,1064, 0, 0, 0},  //TV = 5.64(507 lines)  AV=2.97  SV=4.82  BV=3.80
    {20006,1536,1048, 0, 0, 0},  //TV = 5.64(507 lines)  AV=2.97  SV=4.91  BV=3.71
    {20006,1632,1056, 0, 0, 0},  //TV = 5.64(507 lines)  AV=2.97  SV=5.00  BV=3.61
    {29989,1140,1080, 0, 0, 0},  //TV = 5.06(760 lines)  AV=2.97  SV=4.52  BV=3.51
    {29989,1216,1096, 0, 0, 0},  //TV = 5.06(760 lines)  AV=2.97  SV=4.63  BV=3.40
    {29989,1328,1064, 0, 0, 0},  //TV = 5.06(760 lines)  AV=2.97  SV=4.72  BV=3.31
    {29989,1424,1072, 0, 0, 0},  //TV = 5.06(760 lines)  AV=2.97  SV=4.83  BV=3.20
    {40011,1140,1064, 0, 0, 0},  //TV = 4.64(1014 lines)  AV=2.97  SV=4.50  BV=3.12
    {40011,1216,1072, 0, 0, 0},  //TV = 4.64(1014 lines)  AV=2.97  SV=4.60  BV=3.01
    {40011,1328,1048, 0, 0, 0},  //TV = 4.64(1014 lines)  AV=2.97  SV=4.70  BV=2.92
    {49994,1140,1056, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.49  BV=2.81
    {49994,1216,1064, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.59  BV=2.70
    {49994,1328,1040, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.68  BV=2.61
    {49994,1424,1040, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.78  BV=2.51
    {49994,1536,1032, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.88  BV=2.41
    {49994,1632,1040, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.98  BV=2.31
    {49994,1728,1056, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.09  BV=2.21
    {49994,1840,1064, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.19  BV=2.11
    {49994,2048,1024, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.29  BV=2.01
    {49994,2144,1048, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.39  BV=1.91
    {49994,2240,1072, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.48  BV=1.81
    {49994,2448,1048, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.58  BV=1.72
    {49994,2656,1040, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.68  BV=1.61
    {49994,2864,1032, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.78  BV=1.51
    {49994,3072,1032, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.88  BV=1.41
    {49994,3328,1024, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.99  BV=1.31
    {49994,3456,1056, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=6.09  BV=1.21
    {60017,3072,1064, 0, 0, 0},  //TV = 4.06(1521 lines)  AV=2.97  SV=5.93  BV=1.10
    {60017,3328,1048, 0, 0, 0},  //TV = 4.06(1521 lines)  AV=2.97  SV=6.02  BV=1.01
    {69999,3072,1048, 0, 0, 0},  //TV = 3.84(1774 lines)  AV=2.97  SV=5.91  BV=0.90
    {69999,3328,1032, 0, 0, 0},  //TV = 3.84(1774 lines)  AV=2.97  SV=6.00  BV=0.81
    {79982,3072,1048, 0, 0, 0},  //TV = 3.64(2027 lines)  AV=2.97  SV=5.91  BV=0.71
    {79982,3328,1032, 0, 0, 0},  //TV = 3.64(2027 lines)  AV=2.97  SV=6.00  BV=0.62
    {79982,3584,1032, 0, 0, 0},  //TV = 3.64(2027 lines)  AV=2.97  SV=6.11  BV=0.51
    {79982,3840,1032, 0, 0, 0},  //TV = 3.64(2027 lines)  AV=2.97  SV=6.20  BV=0.41
    {79982,4096,1040, 0, 0, 0},  //TV = 3.64(2027 lines)  AV=2.97  SV=6.31  BV=0.31
    {79982,4352,1040, 0, 0, 0},  //TV = 3.64(2027 lines)  AV=2.97  SV=6.40  BV=0.22
    {90005,4224,1024, 0, 0, 0},  //TV = 3.47(2281 lines)  AV=2.97  SV=6.33  BV=0.11
    {99987,4096,1032, 0, 0, 0},  //TV = 3.32(2534 lines)  AV=2.97  SV=6.30  BV=-0.01
    {99987,4352,1040, 0, 0, 0},  //TV = 3.32(2534 lines)  AV=2.97  SV=6.40  BV=-0.10
    {110010,4224,1040, 0, 0, 0},  //TV = 3.18(2788 lines)  AV=2.97  SV=6.35  BV=-0.20
    {119992,4096,1056, 0, 0, 0},  //TV = 3.06(3041 lines)  AV=2.97  SV=6.33  BV=-0.30
    {119992,4480,1032, 0, 0, 0},  //TV = 3.06(3041 lines)  AV=2.97  SV=6.43  BV=-0.40
    {119992,4736,1048, 0, 0, 0},  //TV = 3.06(3041 lines)  AV=2.97  SV=6.53  BV=-0.50
    {119992,5120,1040, 0, 0, 0},  //TV = 3.06(3041 lines)  AV=2.97  SV=6.63  BV=-0.60
    {119992,5504,1040, 0, 0, 0},  //TV = 3.06(3041 lines)  AV=2.97  SV=6.74  BV=-0.71
    {119992,5888,1040, 0, 0, 0},  //TV = 3.06(3041 lines)  AV=2.97  SV=6.83  BV=-0.80
    {119992,6144,1064, 0, 0, 0},  //TV = 3.06(3041 lines)  AV=2.97  SV=6.93  BV=-0.90
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
},
};

static strAETable g_AE_Custom5AutoTable =
{
    AETABLE_CUSTOM5_AUTO,    //eAETableID
    119,    //u4TotalIndex
    20,    //u4StrobeTrigerBV
    109,    //i4MaxBV
    -9,    //i4MinBV
    90,    //i4EffectiveMaxBV
    0,      //i4EffectiveMinBV
    LIB3A_AE_ISO_SPEED_AUTO, //ISO SPEED
    sCustom5PLineTable_60Hz,
    sCustom5PLineTable_50Hz,
    NULL,
};

static strEvPline sVideoNightPLineTable_60Hz =
{
{
    {198,1140,1024, 0, 0, 0},  //TV = 12.30(5 lines)  AV=2.97  SV=4.44  BV=10.83
    {198,1140,1040, 0, 0, 0},  //TV = 12.30(5 lines)  AV=2.97  SV=4.46  BV=10.81
    {198,1216,1048, 0, 0, 0},  //TV = 12.30(5 lines)  AV=2.97  SV=4.57  BV=10.70
    {198,1328,1024, 0, 0, 0},  //TV = 12.30(5 lines)  AV=2.97  SV=4.66  BV=10.61
    {237,1140,1064, 0, 0, 0},  //TV = 12.04(6 lines)  AV=2.97  SV=4.50  BV=10.52
    {237,1216,1072, 0, 0, 0},  //TV = 12.04(6 lines)  AV=2.97  SV=4.60  BV=10.41
    {277,1140,1056, 0, 0, 0},  //TV = 11.82(7 lines)  AV=2.97  SV=4.49  BV=10.30
    {277,1216,1056, 0, 0, 0},  //TV = 11.82(7 lines)  AV=2.97  SV=4.58  BV=10.21
    {316,1140,1056, 0, 0, 0},  //TV = 11.63(8 lines)  AV=2.97  SV=4.49  BV=10.11
    {316,1216,1064, 0, 0, 0},  //TV = 11.63(8 lines)  AV=2.97  SV=4.59  BV=10.01
    {356,1140,1080, 0, 0, 0},  //TV = 11.46(9 lines)  AV=2.97  SV=4.52  BV=9.91
    {395,1140,1040, 0, 0, 0},  //TV = 11.31(10 lines)  AV=2.97  SV=4.46  BV=9.81
    {435,1140,1024, 0, 0, 0},  //TV = 11.17(11 lines)  AV=2.97  SV=4.44  BV=9.70
    {435,1140,1088, 0, 0, 0},  //TV = 11.17(11 lines)  AV=2.97  SV=4.53  BV=9.61
    {474,1140,1064, 0, 0, 0},  //TV = 11.04(12 lines)  AV=2.97  SV=4.50  BV=9.52
    {513,1140,1056, 0, 0, 0},  //TV = 10.93(13 lines)  AV=2.97  SV=4.49  BV=9.41
    {553,1140,1056, 0, 0, 0},  //TV = 10.82(14 lines)  AV=2.97  SV=4.49  BV=9.31
    {592,1140,1056, 0, 0, 0},  //TV = 10.72(15 lines)  AV=2.97  SV=4.49  BV=9.21
    {632,1140,1056, 0, 0, 0},  //TV = 10.63(16 lines)  AV=2.97  SV=4.49  BV=9.11
    {671,1140,1064, 0, 0, 0},  //TV = 10.54(17 lines)  AV=2.97  SV=4.50  BV=9.02
    {750,1140,1024, 0, 0, 0},  //TV = 10.38(19 lines)  AV=2.97  SV=4.44  BV=8.91
    {790,1140,1040, 0, 0, 0},  //TV = 10.31(20 lines)  AV=2.97  SV=4.46  BV=8.81
    {869,1140,1024, 0, 0, 0},  //TV = 10.17(22 lines)  AV=2.97  SV=4.44  BV=8.70
    {908,1140,1040, 0, 0, 0},  //TV = 10.11(23 lines)  AV=2.97  SV=4.46  BV=8.61
    {987,1140,1024, 0, 0, 0},  //TV = 9.98(25 lines)  AV=2.97  SV=4.44  BV=8.51
    {1066,1140,1024, 0, 0, 0},  //TV = 9.87(27 lines)  AV=2.97  SV=4.44  BV=8.40
    {1105,1140,1056, 0, 0, 0},  //TV = 9.82(28 lines)  AV=2.97  SV=4.49  BV=8.31
    {1224,1140,1024, 0, 0, 0},  //TV = 9.67(31 lines)  AV=2.97  SV=4.44  BV=8.20
    {1303,1140,1024, 0, 0, 0},  //TV = 9.58(33 lines)  AV=2.97  SV=4.44  BV=8.11
    {1382,1140,1040, 0, 0, 0},  //TV = 9.50(35 lines)  AV=2.97  SV=4.46  BV=8.01
    {1500,1140,1024, 0, 0, 0},  //TV = 9.38(38 lines)  AV=2.97  SV=4.44  BV=7.91
    {1618,1140,1024, 0, 0, 0},  //TV = 9.27(41 lines)  AV=2.97  SV=4.44  BV=7.80
    {1697,1140,1040, 0, 0, 0},  //TV = 9.20(43 lines)  AV=2.97  SV=4.46  BV=7.71
    {1855,1140,1024, 0, 0, 0},  //TV = 9.07(47 lines)  AV=2.97  SV=4.44  BV=7.60
    {1973,1140,1024, 0, 0, 0},  //TV = 8.99(50 lines)  AV=2.97  SV=4.44  BV=7.51
    {2131,1140,1024, 0, 0, 0},  //TV = 8.87(54 lines)  AV=2.97  SV=4.44  BV=7.40
    {2250,1140,1032, 0, 0, 0},  //TV = 8.80(57 lines)  AV=2.97  SV=4.45  BV=7.31
    {2447,1140,1024, 0, 0, 0},  //TV = 8.67(62 lines)  AV=2.97  SV=4.44  BV=7.20
    {2605,1140,1024, 0, 0, 0},  //TV = 8.58(66 lines)  AV=2.97  SV=4.44  BV=7.11
    {2802,1140,1024, 0, 0, 0},  //TV = 8.48(71 lines)  AV=2.97  SV=4.44  BV=7.01
    {2999,1140,1024, 0, 0, 0},  //TV = 8.38(76 lines)  AV=2.97  SV=4.44  BV=6.91
    {3236,1140,1024, 0, 0, 0},  //TV = 8.27(82 lines)  AV=2.97  SV=4.44  BV=6.80
    {3473,1140,1024, 0, 0, 0},  //TV = 8.17(88 lines)  AV=2.97  SV=4.44  BV=6.70
    {3710,1140,1024, 0, 0, 0},  //TV = 8.07(94 lines)  AV=2.97  SV=4.44  BV=6.60
    {3986,1140,1024, 0, 0, 0},  //TV = 7.97(101 lines)  AV=2.97  SV=4.44  BV=6.50
    {4262,1140,1024, 0, 0, 0},  //TV = 7.87(108 lines)  AV=2.97  SV=4.44  BV=6.40
    {4538,1140,1024, 0, 0, 0},  //TV = 7.78(115 lines)  AV=2.97  SV=4.44  BV=6.31
    {4893,1140,1024, 0, 0, 0},  //TV = 7.68(124 lines)  AV=2.97  SV=4.44  BV=6.20
    {5209,1140,1024, 0, 0, 0},  //TV = 7.58(132 lines)  AV=2.97  SV=4.44  BV=6.11
    {5643,1140,1024, 0, 0, 0},  //TV = 7.47(143 lines)  AV=2.97  SV=4.44  BV=6.00
    {6038,1140,1024, 0, 0, 0},  //TV = 7.37(153 lines)  AV=2.97  SV=4.44  BV=5.90
    {6472,1140,1024, 0, 0, 0},  //TV = 7.27(164 lines)  AV=2.97  SV=4.44  BV=5.80
    {6906,1140,1024, 0, 0, 0},  //TV = 7.18(175 lines)  AV=2.97  SV=4.44  BV=5.71
    {7419,1140,1024, 0, 0, 0},  //TV = 7.07(188 lines)  AV=2.97  SV=4.44  BV=5.60
    {7971,1140,1024, 0, 0, 0},  //TV = 6.97(202 lines)  AV=2.97  SV=4.44  BV=5.50
    {8326,1140,1048, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=4.48  BV=5.40
    {8326,1216,1048, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=4.57  BV=5.31
    {8326,1328,1024, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=4.66  BV=5.22
    {8326,1424,1024, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=4.76  BV=5.12
    {8326,1536,1024, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=4.87  BV=5.01
    {8326,1632,1032, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=4.97  BV=4.91
    {8326,1728,1040, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=5.06  BV=4.81
    {8326,1840,1048, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=5.17  BV=4.71
    {8326,1936,1072, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=5.27  BV=4.61
    {8326,2144,1032, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=5.36  BV=4.51
    {16652,1140,1048, 0, 0, 0},  //TV = 5.91(422 lines)  AV=2.97  SV=4.48  BV=4.40
    {16652,1216,1048, 0, 0, 0},  //TV = 5.91(422 lines)  AV=2.97  SV=4.57  BV=4.31
    {16652,1328,1024, 0, 0, 0},  //TV = 5.91(422 lines)  AV=2.97  SV=4.66  BV=4.22
    {16652,1424,1024, 0, 0, 0},  //TV = 5.91(422 lines)  AV=2.97  SV=4.76  BV=4.12
    {16652,1424,1104, 0, 0, 0},  //TV = 5.91(422 lines)  AV=2.97  SV=4.87  BV=4.01
    {16652,1632,1032, 0, 0, 0},  //TV = 5.91(422 lines)  AV=2.97  SV=4.97  BV=3.91
    {25017,1140,1056, 0, 0, 0},  //TV = 5.32(634 lines)  AV=2.97  SV=4.49  BV=3.81
    {25017,1216,1064, 0, 0, 0},  //TV = 5.32(634 lines)  AV=2.97  SV=4.59  BV=3.70
    {25017,1328,1048, 0, 0, 0},  //TV = 5.32(634 lines)  AV=2.97  SV=4.70  BV=3.60
    {25017,1424,1040, 0, 0, 0},  //TV = 5.32(634 lines)  AV=2.97  SV=4.78  BV=3.51
    {33185,1140,1048, 0, 0, 0},  //TV = 4.91(841 lines)  AV=2.97  SV=4.48  BV=3.41
    {33185,1216,1056, 0, 0, 0},  //TV = 4.91(841 lines)  AV=2.97  SV=4.58  BV=3.30
    {33185,1328,1032, 0, 0, 0},  //TV = 4.91(841 lines)  AV=2.97  SV=4.67  BV=3.21
    {41669,1140,1024, 0, 0, 0},  //TV = 4.58(1056 lines)  AV=2.97  SV=4.44  BV=3.11
    {41669,1216,1032, 0, 0, 0},  //TV = 4.58(1056 lines)  AV=2.97  SV=4.55  BV=3.01
    {41669,1216,1104, 0, 0, 0},  //TV = 4.58(1056 lines)  AV=2.97  SV=4.64  BV=2.91
    {49994,1140,1056, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.49  BV=2.81
    {49994,1216,1064, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.59  BV=2.70
    {49994,1328,1040, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.68  BV=2.61
    {49994,1424,1040, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.78  BV=2.51
    {49994,1536,1032, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.88  BV=2.41
    {49994,1632,1040, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.98  BV=2.31
    {49994,1728,1056, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.09  BV=2.21
    {49994,1840,1064, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.19  BV=2.11
    {49994,2048,1024, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.29  BV=2.01
    {49994,2144,1048, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.39  BV=1.91
    {49994,2240,1072, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.48  BV=1.81
    {49994,2448,1048, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.58  BV=1.72
    {49994,2656,1040, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.68  BV=1.61
    {49994,2864,1032, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.78  BV=1.51
    {49994,3072,1032, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.88  BV=1.41
    {49994,3328,1024, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.99  BV=1.31
    {49994,3456,1056, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=6.09  BV=1.21
    {58319,3200,1048, 0, 0, 0},  //TV = 4.10(1478 lines)  AV=2.97  SV=5.96  BV=1.11
    {66527,3072,1024, 0, 0, 0},  //TV = 3.91(1686 lines)  AV=2.97  SV=5.87  BV=1.01
    {66527,3200,1056, 0, 0, 0},  //TV = 3.91(1686 lines)  AV=2.97  SV=5.98  BV=0.91
    {75011,3072,1048, 0, 0, 0},  //TV = 3.74(1901 lines)  AV=2.97  SV=5.91  BV=0.80
    {75011,3328,1032, 0, 0, 0},  //TV = 3.74(1901 lines)  AV=2.97  SV=6.00  BV=0.71
    {83336,3200,1032, 0, 0, 0},  //TV = 3.58(2112 lines)  AV=2.97  SV=5.94  BV=0.61
    {83336,3456,1024, 0, 0, 0},  //TV = 3.58(2112 lines)  AV=2.97  SV=6.04  BV=0.51
    {83336,3712,1024, 0, 0, 0},  //TV = 3.58(2112 lines)  AV=2.97  SV=6.14  BV=0.41
    {83336,3968,1032, 0, 0, 0},  //TV = 3.58(2112 lines)  AV=2.97  SV=6.25  BV=0.30
    {83336,4224,1032, 0, 0, 0},  //TV = 3.58(2112 lines)  AV=2.97  SV=6.34  BV=0.21
    {91661,4096,1040, 0, 0, 0},  //TV = 3.45(2323 lines)  AV=2.97  SV=6.31  BV=0.11
    {99987,4096,1032, 0, 0, 0},  //TV = 3.32(2534 lines)  AV=2.97  SV=6.30  BV=-0.01
    {108313,4096,1024, 0, 0, 0},  //TV = 3.21(2745 lines)  AV=2.97  SV=6.29  BV=-0.11
    {108313,4352,1032, 0, 0, 0},  //TV = 3.21(2745 lines)  AV=2.97  SV=6.39  BV=-0.21
    {116678,4224,1048, 0, 0, 0},  //TV = 3.10(2957 lines)  AV=2.97  SV=6.36  BV=-0.29
    {125004,4224,1048, 0, 0, 0},  //TV = 3.00(3168 lines)  AV=2.97  SV=6.36  BV=-0.39
    {141655,4096,1032, 0, 0, 0},  //TV = 2.82(3590 lines)  AV=2.97  SV=6.30  BV=-0.51
    {149980,4096,1040, 0, 0, 0},  //TV = 2.74(3801 lines)  AV=2.97  SV=6.31  BV=-0.60
    {158346,4096,1056, 0, 0, 0},  //TV = 2.66(4013 lines)  AV=2.97  SV=6.33  BV=-0.70
    {166672,4224,1048, 0, 0, 0},  //TV = 2.58(4224 lines)  AV=2.97  SV=6.36  BV=-0.81
    {166672,4608,1024, 0, 0, 0},  //TV = 2.58(4224 lines)  AV=2.97  SV=6.46  BV=-0.90
    {166672,4864,1040, 0, 0, 0},  //TV = 2.58(4224 lines)  AV=2.97  SV=6.56  BV=-1.00
    {166672,5248,1032, 0, 0, 0},  //TV = 2.58(4224 lines)  AV=2.97  SV=6.66  BV=-1.10
    {166672,5632,1032, 0, 0, 0},  //TV = 2.58(4224 lines)  AV=2.97  SV=6.76  BV=-1.20
    {166672,6016,1032, 0, 0, 0},  //TV = 2.58(4224 lines)  AV=2.97  SV=6.85  BV=-1.30
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
},
};

static strEvPline sVideoNightPLineTable_50Hz =
{
{
    {198,1140,1024, 0, 0, 0},  //TV = 12.30(5 lines)  AV=2.97  SV=4.44  BV=10.83
    {198,1140,1040, 0, 0, 0},  //TV = 12.30(5 lines)  AV=2.97  SV=4.46  BV=10.81
    {198,1216,1048, 0, 0, 0},  //TV = 12.30(5 lines)  AV=2.97  SV=4.57  BV=10.70
    {198,1328,1024, 0, 0, 0},  //TV = 12.30(5 lines)  AV=2.97  SV=4.66  BV=10.61
    {237,1140,1064, 0, 0, 0},  //TV = 12.04(6 lines)  AV=2.97  SV=4.50  BV=10.52
    {237,1216,1072, 0, 0, 0},  //TV = 12.04(6 lines)  AV=2.97  SV=4.60  BV=10.41
    {277,1140,1056, 0, 0, 0},  //TV = 11.82(7 lines)  AV=2.97  SV=4.49  BV=10.30
    {277,1216,1056, 0, 0, 0},  //TV = 11.82(7 lines)  AV=2.97  SV=4.58  BV=10.21
    {316,1140,1056, 0, 0, 0},  //TV = 11.63(8 lines)  AV=2.97  SV=4.49  BV=10.11
    {316,1216,1064, 0, 0, 0},  //TV = 11.63(8 lines)  AV=2.97  SV=4.59  BV=10.01
    {356,1140,1080, 0, 0, 0},  //TV = 11.46(9 lines)  AV=2.97  SV=4.52  BV=9.91
    {395,1140,1040, 0, 0, 0},  //TV = 11.31(10 lines)  AV=2.97  SV=4.46  BV=9.81
    {435,1140,1024, 0, 0, 0},  //TV = 11.17(11 lines)  AV=2.97  SV=4.44  BV=9.70
    {435,1140,1088, 0, 0, 0},  //TV = 11.17(11 lines)  AV=2.97  SV=4.53  BV=9.61
    {474,1140,1064, 0, 0, 0},  //TV = 11.04(12 lines)  AV=2.97  SV=4.50  BV=9.52
    {513,1140,1056, 0, 0, 0},  //TV = 10.93(13 lines)  AV=2.97  SV=4.49  BV=9.41
    {553,1140,1056, 0, 0, 0},  //TV = 10.82(14 lines)  AV=2.97  SV=4.49  BV=9.31
    {592,1140,1056, 0, 0, 0},  //TV = 10.72(15 lines)  AV=2.97  SV=4.49  BV=9.21
    {632,1140,1056, 0, 0, 0},  //TV = 10.63(16 lines)  AV=2.97  SV=4.49  BV=9.11
    {671,1140,1064, 0, 0, 0},  //TV = 10.54(17 lines)  AV=2.97  SV=4.50  BV=9.02
    {750,1140,1024, 0, 0, 0},  //TV = 10.38(19 lines)  AV=2.97  SV=4.44  BV=8.91
    {790,1140,1040, 0, 0, 0},  //TV = 10.31(20 lines)  AV=2.97  SV=4.46  BV=8.81
    {869,1140,1024, 0, 0, 0},  //TV = 10.17(22 lines)  AV=2.97  SV=4.44  BV=8.70
    {908,1140,1040, 0, 0, 0},  //TV = 10.11(23 lines)  AV=2.97  SV=4.46  BV=8.61
    {987,1140,1024, 0, 0, 0},  //TV = 9.98(25 lines)  AV=2.97  SV=4.44  BV=8.51
    {1066,1140,1024, 0, 0, 0},  //TV = 9.87(27 lines)  AV=2.97  SV=4.44  BV=8.40
    {1105,1140,1056, 0, 0, 0},  //TV = 9.82(28 lines)  AV=2.97  SV=4.49  BV=8.31
    {1224,1140,1024, 0, 0, 0},  //TV = 9.67(31 lines)  AV=2.97  SV=4.44  BV=8.20
    {1303,1140,1024, 0, 0, 0},  //TV = 9.58(33 lines)  AV=2.97  SV=4.44  BV=8.11
    {1382,1140,1040, 0, 0, 0},  //TV = 9.50(35 lines)  AV=2.97  SV=4.46  BV=8.01
    {1500,1140,1024, 0, 0, 0},  //TV = 9.38(38 lines)  AV=2.97  SV=4.44  BV=7.91
    {1618,1140,1024, 0, 0, 0},  //TV = 9.27(41 lines)  AV=2.97  SV=4.44  BV=7.80
    {1697,1140,1040, 0, 0, 0},  //TV = 9.20(43 lines)  AV=2.97  SV=4.46  BV=7.71
    {1855,1140,1024, 0, 0, 0},  //TV = 9.07(47 lines)  AV=2.97  SV=4.44  BV=7.60
    {1973,1140,1024, 0, 0, 0},  //TV = 8.99(50 lines)  AV=2.97  SV=4.44  BV=7.51
    {2131,1140,1024, 0, 0, 0},  //TV = 8.87(54 lines)  AV=2.97  SV=4.44  BV=7.40
    {2250,1140,1032, 0, 0, 0},  //TV = 8.80(57 lines)  AV=2.97  SV=4.45  BV=7.31
    {2447,1140,1024, 0, 0, 0},  //TV = 8.67(62 lines)  AV=2.97  SV=4.44  BV=7.20
    {2605,1140,1024, 0, 0, 0},  //TV = 8.58(66 lines)  AV=2.97  SV=4.44  BV=7.11
    {2802,1140,1024, 0, 0, 0},  //TV = 8.48(71 lines)  AV=2.97  SV=4.44  BV=7.01
    {2999,1140,1024, 0, 0, 0},  //TV = 8.38(76 lines)  AV=2.97  SV=4.44  BV=6.91
    {3236,1140,1024, 0, 0, 0},  //TV = 8.27(82 lines)  AV=2.97  SV=4.44  BV=6.80
    {3473,1140,1024, 0, 0, 0},  //TV = 8.17(88 lines)  AV=2.97  SV=4.44  BV=6.70
    {3710,1140,1024, 0, 0, 0},  //TV = 8.07(94 lines)  AV=2.97  SV=4.44  BV=6.60
    {3986,1140,1024, 0, 0, 0},  //TV = 7.97(101 lines)  AV=2.97  SV=4.44  BV=6.50
    {4262,1140,1024, 0, 0, 0},  //TV = 7.87(108 lines)  AV=2.97  SV=4.44  BV=6.40
    {4538,1140,1024, 0, 0, 0},  //TV = 7.78(115 lines)  AV=2.97  SV=4.44  BV=6.31
    {4893,1140,1024, 0, 0, 0},  //TV = 7.68(124 lines)  AV=2.97  SV=4.44  BV=6.20
    {5209,1140,1024, 0, 0, 0},  //TV = 7.58(132 lines)  AV=2.97  SV=4.44  BV=6.11
    {5643,1140,1024, 0, 0, 0},  //TV = 7.47(143 lines)  AV=2.97  SV=4.44  BV=6.00
    {6038,1140,1024, 0, 0, 0},  //TV = 7.37(153 lines)  AV=2.97  SV=4.44  BV=5.90
    {6472,1140,1024, 0, 0, 0},  //TV = 7.27(164 lines)  AV=2.97  SV=4.44  BV=5.80
    {6906,1140,1024, 0, 0, 0},  //TV = 7.18(175 lines)  AV=2.97  SV=4.44  BV=5.71
    {7419,1140,1024, 0, 0, 0},  //TV = 7.07(188 lines)  AV=2.97  SV=4.44  BV=5.60
    {7971,1140,1024, 0, 0, 0},  //TV = 6.97(202 lines)  AV=2.97  SV=4.44  BV=5.50
    {8484,1140,1024, 0, 0, 0},  //TV = 6.88(215 lines)  AV=2.97  SV=4.44  BV=5.41
    {9155,1140,1024, 0, 0, 0},  //TV = 6.77(232 lines)  AV=2.97  SV=4.44  BV=5.30
    {9826,1140,1024, 0, 0, 0},  //TV = 6.67(249 lines)  AV=2.97  SV=4.44  BV=5.20
    {9983,1140,1072, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=4.51  BV=5.11
    {9983,1216,1072, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=4.60  BV=5.02
    {9983,1328,1056, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=4.71  BV=4.91
    {9983,1424,1056, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=4.81  BV=4.81
    {9983,1536,1048, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=4.91  BV=4.71
    {9983,1632,1064, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=5.01  BV=4.60
    {9983,1728,1072, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=5.11  BV=4.51
    {9983,1936,1024, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=5.21  BV=4.41
    {9983,2048,1040, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=5.31  BV=4.31
    {9983,2144,1064, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=5.41  BV=4.21
    {20006,1140,1064, 0, 0, 0},  //TV = 5.64(507 lines)  AV=2.97  SV=4.50  BV=4.12
    {20006,1216,1072, 0, 0, 0},  //TV = 5.64(507 lines)  AV=2.97  SV=4.60  BV=4.01
    {20006,1328,1048, 0, 0, 0},  //TV = 5.64(507 lines)  AV=2.97  SV=4.70  BV=3.92
    {20006,1424,1064, 0, 0, 0},  //TV = 5.64(507 lines)  AV=2.97  SV=4.82  BV=3.80
    {20006,1536,1048, 0, 0, 0},  //TV = 5.64(507 lines)  AV=2.97  SV=4.91  BV=3.71
    {20006,1632,1056, 0, 0, 0},  //TV = 5.64(507 lines)  AV=2.97  SV=5.00  BV=3.61
    {29989,1140,1080, 0, 0, 0},  //TV = 5.06(760 lines)  AV=2.97  SV=4.52  BV=3.51
    {29989,1216,1096, 0, 0, 0},  //TV = 5.06(760 lines)  AV=2.97  SV=4.63  BV=3.40
    {29989,1328,1064, 0, 0, 0},  //TV = 5.06(760 lines)  AV=2.97  SV=4.72  BV=3.31
    {29989,1424,1072, 0, 0, 0},  //TV = 5.06(760 lines)  AV=2.97  SV=4.83  BV=3.20
    {40011,1140,1064, 0, 0, 0},  //TV = 4.64(1014 lines)  AV=2.97  SV=4.50  BV=3.12
    {40011,1216,1072, 0, 0, 0},  //TV = 4.64(1014 lines)  AV=2.97  SV=4.60  BV=3.01
    {40011,1328,1048, 0, 0, 0},  //TV = 4.64(1014 lines)  AV=2.97  SV=4.70  BV=2.92
    {49994,1140,1056, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.49  BV=2.81
    {49994,1216,1064, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.59  BV=2.70
    {49994,1328,1040, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.68  BV=2.61
    {49994,1424,1040, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.78  BV=2.51
    {49994,1536,1032, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.88  BV=2.41
    {49994,1632,1040, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.98  BV=2.31
    {49994,1728,1056, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.09  BV=2.21
    {49994,1840,1064, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.19  BV=2.11
    {49994,2048,1024, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.29  BV=2.01
    {49994,2144,1048, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.39  BV=1.91
    {49994,2240,1072, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.48  BV=1.81
    {49994,2448,1048, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.58  BV=1.72
    {49994,2656,1040, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.68  BV=1.61
    {49994,2864,1032, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.78  BV=1.51
    {49994,3072,1032, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.88  BV=1.41
    {49994,3328,1024, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.99  BV=1.31
    {49994,3456,1056, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=6.09  BV=1.21
    {60017,3072,1064, 0, 0, 0},  //TV = 4.06(1521 lines)  AV=2.97  SV=5.93  BV=1.10
    {60017,3328,1048, 0, 0, 0},  //TV = 4.06(1521 lines)  AV=2.97  SV=6.02  BV=1.01
    {69999,3072,1048, 0, 0, 0},  //TV = 3.84(1774 lines)  AV=2.97  SV=5.91  BV=0.90
    {69999,3328,1032, 0, 0, 0},  //TV = 3.84(1774 lines)  AV=2.97  SV=6.00  BV=0.81
    {79982,3072,1048, 0, 0, 0},  //TV = 3.64(2027 lines)  AV=2.97  SV=5.91  BV=0.71
    {79982,3328,1032, 0, 0, 0},  //TV = 3.64(2027 lines)  AV=2.97  SV=6.00  BV=0.62
    {79982,3584,1032, 0, 0, 0},  //TV = 3.64(2027 lines)  AV=2.97  SV=6.11  BV=0.51
    {79982,3840,1032, 0, 0, 0},  //TV = 3.64(2027 lines)  AV=2.97  SV=6.20  BV=0.41
    {79982,4096,1040, 0, 0, 0},  //TV = 3.64(2027 lines)  AV=2.97  SV=6.31  BV=0.31
    {79982,4352,1040, 0, 0, 0},  //TV = 3.64(2027 lines)  AV=2.97  SV=6.40  BV=0.22
    {90005,4224,1024, 0, 0, 0},  //TV = 3.47(2281 lines)  AV=2.97  SV=6.33  BV=0.11
    {99987,4096,1032, 0, 0, 0},  //TV = 3.32(2534 lines)  AV=2.97  SV=6.30  BV=-0.01
    {99987,4352,1040, 0, 0, 0},  //TV = 3.32(2534 lines)  AV=2.97  SV=6.40  BV=-0.10
    {110010,4224,1040, 0, 0, 0},  //TV = 3.18(2788 lines)  AV=2.97  SV=6.35  BV=-0.20
    {119992,4096,1056, 0, 0, 0},  //TV = 3.06(3041 lines)  AV=2.97  SV=6.33  BV=-0.30
    {130015,4096,1040, 0, 0, 0},  //TV = 2.94(3295 lines)  AV=2.97  SV=6.31  BV=-0.40
    {139997,4096,1040, 0, 0, 0},  //TV = 2.84(3548 lines)  AV=2.97  SV=6.31  BV=-0.50
    {149980,4096,1040, 0, 0, 0},  //TV = 2.74(3801 lines)  AV=2.97  SV=6.31  BV=-0.60
    {160003,4096,1040, 0, 0, 0},  //TV = 2.64(4055 lines)  AV=2.97  SV=6.31  BV=-0.69
    {160003,4480,1024, 0, 0, 0},  //TV = 2.64(4055 lines)  AV=2.97  SV=6.42  BV=-0.80
    {160003,4736,1040, 0, 0, 0},  //TV = 2.64(4055 lines)  AV=2.97  SV=6.52  BV=-0.90
    {160003,5120,1032, 0, 0, 0},  //TV = 2.64(4055 lines)  AV=2.97  SV=6.62  BV=-1.01
    {160003,5504,1024, 0, 0, 0},  //TV = 2.64(4055 lines)  AV=2.97  SV=6.71  BV=-1.10
    {160003,5888,1024, 0, 0, 0},  //TV = 2.64(4055 lines)  AV=2.97  SV=6.81  BV=-1.20
    {160003,6144,1056, 0, 0, 0},  //TV = 2.64(4055 lines)  AV=2.97  SV=6.92  BV=-1.30
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
},
};

static strAETable g_AE_VideoNightTable =
{
    AETABLE_VIDEO_NIGHT,    //eAETableID
    123,    //u4TotalIndex
    20,    //u4StrobeTrigerBV
    109,    //i4MaxBV
    -13,    //i4MinBV
    90,    //i4EffectiveMaxBV
    -10,      //i4EffectiveMinBV
    LIB3A_AE_ISO_SPEED_AUTO, //ISO SPEED
    sVideoNightPLineTable_60Hz,
    sVideoNightPLineTable_50Hz,
    NULL,
};

static strEvPline sCaptureISO100PLineTable_60Hz =
{
{
    {198,1140,1024, 0, 0, 0},  //TV = 12.30(5 lines)  AV=2.97  SV=4.44  BV=10.83
    {198,1140,1040, 0, 0, 0},  //TV = 12.30(5 lines)  AV=2.97  SV=4.46  BV=10.81
    {198,1216,1048, 0, 0, 0},  //TV = 12.30(5 lines)  AV=2.97  SV=4.57  BV=10.70
    {198,1328,1024, 0, 0, 0},  //TV = 12.30(5 lines)  AV=2.97  SV=4.66  BV=10.61
    {237,1140,1064, 0, 0, 0},  //TV = 12.04(6 lines)  AV=2.97  SV=4.50  BV=10.52
    {237,1216,1072, 0, 0, 0},  //TV = 12.04(6 lines)  AV=2.97  SV=4.60  BV=10.41
    {277,1140,1056, 0, 0, 0},  //TV = 11.82(7 lines)  AV=2.97  SV=4.49  BV=10.30
    {277,1216,1056, 0, 0, 0},  //TV = 11.82(7 lines)  AV=2.97  SV=4.58  BV=10.21
    {316,1140,1056, 0, 0, 0},  //TV = 11.63(8 lines)  AV=2.97  SV=4.49  BV=10.11
    {316,1216,1064, 0, 0, 0},  //TV = 11.63(8 lines)  AV=2.97  SV=4.59  BV=10.01
    {356,1140,1080, 0, 0, 0},  //TV = 11.46(9 lines)  AV=2.97  SV=4.52  BV=9.91
    {395,1140,1040, 0, 0, 0},  //TV = 11.31(10 lines)  AV=2.97  SV=4.46  BV=9.81
    {435,1140,1024, 0, 0, 0},  //TV = 11.17(11 lines)  AV=2.97  SV=4.44  BV=9.70
    {435,1140,1088, 0, 0, 0},  //TV = 11.17(11 lines)  AV=2.97  SV=4.53  BV=9.61
    {474,1140,1064, 0, 0, 0},  //TV = 11.04(12 lines)  AV=2.97  SV=4.50  BV=9.52
    {513,1140,1056, 0, 0, 0},  //TV = 10.93(13 lines)  AV=2.97  SV=4.49  BV=9.41
    {553,1140,1056, 0, 0, 0},  //TV = 10.82(14 lines)  AV=2.97  SV=4.49  BV=9.31
    {592,1140,1056, 0, 0, 0},  //TV = 10.72(15 lines)  AV=2.97  SV=4.49  BV=9.21
    {632,1140,1056, 0, 0, 0},  //TV = 10.63(16 lines)  AV=2.97  SV=4.49  BV=9.11
    {671,1140,1064, 0, 0, 0},  //TV = 10.54(17 lines)  AV=2.97  SV=4.50  BV=9.02
    {750,1140,1024, 0, 0, 0},  //TV = 10.38(19 lines)  AV=2.97  SV=4.44  BV=8.91
    {790,1140,1040, 0, 0, 0},  //TV = 10.31(20 lines)  AV=2.97  SV=4.46  BV=8.81
    {869,1140,1024, 0, 0, 0},  //TV = 10.17(22 lines)  AV=2.97  SV=4.44  BV=8.70
    {908,1140,1040, 0, 0, 0},  //TV = 10.11(23 lines)  AV=2.97  SV=4.46  BV=8.61
    {987,1140,1024, 0, 0, 0},  //TV = 9.98(25 lines)  AV=2.97  SV=4.44  BV=8.51
    {1066,1140,1024, 0, 0, 0},  //TV = 9.87(27 lines)  AV=2.97  SV=4.44  BV=8.40
    {1105,1140,1056, 0, 0, 0},  //TV = 9.82(28 lines)  AV=2.97  SV=4.49  BV=8.31
    {1224,1140,1024, 0, 0, 0},  //TV = 9.67(31 lines)  AV=2.97  SV=4.44  BV=8.20
    {1303,1140,1024, 0, 0, 0},  //TV = 9.58(33 lines)  AV=2.97  SV=4.44  BV=8.11
    {1382,1140,1040, 0, 0, 0},  //TV = 9.50(35 lines)  AV=2.97  SV=4.46  BV=8.01
    {1500,1140,1024, 0, 0, 0},  //TV = 9.38(38 lines)  AV=2.97  SV=4.44  BV=7.91
    {1618,1140,1024, 0, 0, 0},  //TV = 9.27(41 lines)  AV=2.97  SV=4.44  BV=7.80
    {1697,1140,1040, 0, 0, 0},  //TV = 9.20(43 lines)  AV=2.97  SV=4.46  BV=7.71
    {1855,1140,1024, 0, 0, 0},  //TV = 9.07(47 lines)  AV=2.97  SV=4.44  BV=7.60
    {1973,1140,1024, 0, 0, 0},  //TV = 8.99(50 lines)  AV=2.97  SV=4.44  BV=7.51
    {2131,1140,1024, 0, 0, 0},  //TV = 8.87(54 lines)  AV=2.97  SV=4.44  BV=7.40
    {2250,1140,1032, 0, 0, 0},  //TV = 8.80(57 lines)  AV=2.97  SV=4.45  BV=7.31
    {2447,1140,1024, 0, 0, 0},  //TV = 8.67(62 lines)  AV=2.97  SV=4.44  BV=7.20
    {2605,1140,1024, 0, 0, 0},  //TV = 8.58(66 lines)  AV=2.97  SV=4.44  BV=7.11
    {2802,1140,1024, 0, 0, 0},  //TV = 8.48(71 lines)  AV=2.97  SV=4.44  BV=7.01
    {2999,1140,1024, 0, 0, 0},  //TV = 8.38(76 lines)  AV=2.97  SV=4.44  BV=6.91
    {3236,1140,1024, 0, 0, 0},  //TV = 8.27(82 lines)  AV=2.97  SV=4.44  BV=6.80
    {3473,1140,1024, 0, 0, 0},  //TV = 8.17(88 lines)  AV=2.97  SV=4.44  BV=6.70
    {3710,1140,1024, 0, 0, 0},  //TV = 8.07(94 lines)  AV=2.97  SV=4.44  BV=6.60
    {3986,1140,1024, 0, 0, 0},  //TV = 7.97(101 lines)  AV=2.97  SV=4.44  BV=6.50
    {4262,1140,1024, 0, 0, 0},  //TV = 7.87(108 lines)  AV=2.97  SV=4.44  BV=6.40
    {4538,1140,1024, 0, 0, 0},  //TV = 7.78(115 lines)  AV=2.97  SV=4.44  BV=6.31
    {4893,1140,1024, 0, 0, 0},  //TV = 7.68(124 lines)  AV=2.97  SV=4.44  BV=6.20
    {5209,1140,1024, 0, 0, 0},  //TV = 7.58(132 lines)  AV=2.97  SV=4.44  BV=6.11
    {5643,1140,1024, 0, 0, 0},  //TV = 7.47(143 lines)  AV=2.97  SV=4.44  BV=6.00
    {6038,1140,1024, 0, 0, 0},  //TV = 7.37(153 lines)  AV=2.97  SV=4.44  BV=5.90
    {6472,1140,1024, 0, 0, 0},  //TV = 7.27(164 lines)  AV=2.97  SV=4.44  BV=5.80
    {6906,1140,1024, 0, 0, 0},  //TV = 7.18(175 lines)  AV=2.97  SV=4.44  BV=5.71
    {7419,1140,1024, 0, 0, 0},  //TV = 7.07(188 lines)  AV=2.97  SV=4.44  BV=5.60
    {7971,1140,1024, 0, 0, 0},  //TV = 6.97(202 lines)  AV=2.97  SV=4.44  BV=5.50
    {8326,1140,1048, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=4.48  BV=5.40
    {8326,1216,1048, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=4.57  BV=5.31
    {8326,1328,1024, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=4.66  BV=5.22
    {8326,1424,1024, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=4.76  BV=5.12
    {8326,1536,1024, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=4.87  BV=5.01
    {8326,1632,1032, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=4.97  BV=4.91
    {8326,1728,1040, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=5.06  BV=4.81
    {8326,1840,1048, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=5.17  BV=4.71
    {8326,1936,1072, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=5.27  BV=4.61
    {8326,2144,1032, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=5.36  BV=4.51
    {16652,1140,1048, 0, 0, 0},  //TV = 5.91(422 lines)  AV=2.97  SV=4.48  BV=4.40
    {16652,1216,1048, 0, 0, 0},  //TV = 5.91(422 lines)  AV=2.97  SV=4.57  BV=4.31
    {16652,1328,1024, 0, 0, 0},  //TV = 5.91(422 lines)  AV=2.97  SV=4.66  BV=4.22
    {16652,1424,1024, 0, 0, 0},  //TV = 5.91(422 lines)  AV=2.97  SV=4.76  BV=4.12
    {16652,1424,1104, 0, 0, 0},  //TV = 5.91(422 lines)  AV=2.97  SV=4.87  BV=4.01
    {16652,1632,1032, 0, 0, 0},  //TV = 5.91(422 lines)  AV=2.97  SV=4.97  BV=3.91
    {25017,1140,1056, 0, 0, 0},  //TV = 5.32(634 lines)  AV=2.97  SV=4.49  BV=3.81
    {25017,1216,1064, 0, 0, 0},  //TV = 5.32(634 lines)  AV=2.97  SV=4.59  BV=3.70
    {25017,1328,1048, 0, 0, 0},  //TV = 5.32(634 lines)  AV=2.97  SV=4.70  BV=3.60
    {25017,1424,1040, 0, 0, 0},  //TV = 5.32(634 lines)  AV=2.97  SV=4.78  BV=3.51
    {33343,1140,1048, 0, 0, 0},  //TV = 4.91(845 lines)  AV=2.97  SV=4.48  BV=3.40
    {33343,1216,1048, 0, 0, 0},  //TV = 4.91(845 lines)  AV=2.97  SV=4.57  BV=3.31
    {33343,1328,1024, 0, 0, 0},  //TV = 4.91(845 lines)  AV=2.97  SV=4.66  BV=3.22
    {41669,1140,1024, 0, 0, 0},  //TV = 4.58(1056 lines)  AV=2.97  SV=4.44  BV=3.11
    {41669,1216,1032, 0, 0, 0},  //TV = 4.58(1056 lines)  AV=2.97  SV=4.55  BV=3.01
    {41669,1216,1104, 0, 0, 0},  //TV = 4.58(1056 lines)  AV=2.97  SV=4.64  BV=2.91
    {49994,1140,1056, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.49  BV=2.81
    {49994,1216,1064, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.59  BV=2.70
    {49994,1328,1040, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.68  BV=2.61
    {49994,1424,1040, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.78  BV=2.51
    {49994,1536,1032, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.88  BV=2.41
    {49994,1632,1040, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.98  BV=2.31
    {49994,1728,1056, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.09  BV=2.21
    {49994,1840,1064, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.19  BV=2.11
    {49994,2048,1024, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.29  BV=2.01
    {49994,2144,1048, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.39  BV=1.91
    {49994,2240,1072, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.48  BV=1.81
    {49994,2448,1048, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.58  BV=1.72
    {49994,2656,1040, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.68  BV=1.61
    {49994,2864,1032, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.78  BV=1.51
    {49994,3072,1032, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.88  BV=1.41
    {49994,3328,1024, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.99  BV=1.31
    {49994,3456,1056, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=6.09  BV=1.21
    {58319,3200,1048, 0, 0, 0},  //TV = 4.10(1478 lines)  AV=2.97  SV=5.96  BV=1.11
    {66645,3072,1024, 0, 0, 0},  //TV = 3.91(1689 lines)  AV=2.97  SV=5.87  BV=1.01
    {66645,3200,1056, 0, 0, 0},  //TV = 3.91(1689 lines)  AV=2.97  SV=5.98  BV=0.90
    {75011,3072,1048, 0, 0, 0},  //TV = 3.74(1901 lines)  AV=2.97  SV=5.91  BV=0.80
    {75011,3328,1032, 0, 0, 0},  //TV = 3.74(1901 lines)  AV=2.97  SV=6.00  BV=0.71
    {83336,3200,1032, 0, 0, 0},  //TV = 3.58(2112 lines)  AV=2.97  SV=5.94  BV=0.61
    {83336,3456,1024, 0, 0, 0},  //TV = 3.58(2112 lines)  AV=2.97  SV=6.04  BV=0.51
    {83336,3712,1024, 0, 0, 0},  //TV = 3.58(2112 lines)  AV=2.97  SV=6.14  BV=0.41
    {83336,3968,1032, 0, 0, 0},  //TV = 3.58(2112 lines)  AV=2.97  SV=6.25  BV=0.30
    {83336,4224,1032, 0, 0, 0},  //TV = 3.58(2112 lines)  AV=2.97  SV=6.34  BV=0.21
    {91661,4096,1040, 0, 0, 0},  //TV = 3.45(2323 lines)  AV=2.97  SV=6.31  BV=0.11
    {99987,4096,1032, 0, 0, 0},  //TV = 3.32(2534 lines)  AV=2.97  SV=6.30  BV=-0.01
    {108313,4096,1024, 0, 0, 0},  //TV = 3.21(2745 lines)  AV=2.97  SV=6.29  BV=-0.11
    {108313,4352,1032, 0, 0, 0},  //TV = 3.21(2745 lines)  AV=2.97  SV=6.39  BV=-0.21
    {116678,4224,1048, 0, 0, 0},  //TV = 3.10(2957 lines)  AV=2.97  SV=6.36  BV=-0.29
    {125004,4224,1048, 0, 0, 0},  //TV = 3.00(3168 lines)  AV=2.97  SV=6.36  BV=-0.39
    {125004,4608,1032, 0, 0, 0},  //TV = 3.00(3168 lines)  AV=2.97  SV=6.47  BV=-0.50
    {125004,4864,1048, 0, 0, 0},  //TV = 3.00(3168 lines)  AV=2.97  SV=6.57  BV=-0.60
    {125004,5376,1024, 0, 0, 0},  //TV = 3.00(3168 lines)  AV=2.97  SV=6.68  BV=-0.71
    {125004,5632,1040, 0, 0, 0},  //TV = 3.00(3168 lines)  AV=2.97  SV=6.77  BV=-0.80
    {125004,6016,1040, 0, 0, 0},  //TV = 3.00(3168 lines)  AV=2.97  SV=6.86  BV=-0.89
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
},
};

static strEvPline sCaptureISO100PLineTable_50Hz =
{
{
    {198,1140,1024, 0, 0, 0},  //TV = 12.30(5 lines)  AV=2.97  SV=4.44  BV=10.83
    {198,1140,1040, 0, 0, 0},  //TV = 12.30(5 lines)  AV=2.97  SV=4.46  BV=10.81
    {198,1216,1048, 0, 0, 0},  //TV = 12.30(5 lines)  AV=2.97  SV=4.57  BV=10.70
    {198,1328,1024, 0, 0, 0},  //TV = 12.30(5 lines)  AV=2.97  SV=4.66  BV=10.61
    {237,1140,1064, 0, 0, 0},  //TV = 12.04(6 lines)  AV=2.97  SV=4.50  BV=10.52
    {237,1216,1072, 0, 0, 0},  //TV = 12.04(6 lines)  AV=2.97  SV=4.60  BV=10.41
    {277,1140,1056, 0, 0, 0},  //TV = 11.82(7 lines)  AV=2.97  SV=4.49  BV=10.30
    {277,1216,1056, 0, 0, 0},  //TV = 11.82(7 lines)  AV=2.97  SV=4.58  BV=10.21
    {316,1140,1056, 0, 0, 0},  //TV = 11.63(8 lines)  AV=2.97  SV=4.49  BV=10.11
    {316,1216,1064, 0, 0, 0},  //TV = 11.63(8 lines)  AV=2.97  SV=4.59  BV=10.01
    {356,1140,1080, 0, 0, 0},  //TV = 11.46(9 lines)  AV=2.97  SV=4.52  BV=9.91
    {395,1140,1040, 0, 0, 0},  //TV = 11.31(10 lines)  AV=2.97  SV=4.46  BV=9.81
    {435,1140,1024, 0, 0, 0},  //TV = 11.17(11 lines)  AV=2.97  SV=4.44  BV=9.70
    {435,1140,1088, 0, 0, 0},  //TV = 11.17(11 lines)  AV=2.97  SV=4.53  BV=9.61
    {474,1140,1064, 0, 0, 0},  //TV = 11.04(12 lines)  AV=2.97  SV=4.50  BV=9.52
    {513,1140,1056, 0, 0, 0},  //TV = 10.93(13 lines)  AV=2.97  SV=4.49  BV=9.41
    {553,1140,1056, 0, 0, 0},  //TV = 10.82(14 lines)  AV=2.97  SV=4.49  BV=9.31
    {592,1140,1056, 0, 0, 0},  //TV = 10.72(15 lines)  AV=2.97  SV=4.49  BV=9.21
    {632,1140,1056, 0, 0, 0},  //TV = 10.63(16 lines)  AV=2.97  SV=4.49  BV=9.11
    {671,1140,1064, 0, 0, 0},  //TV = 10.54(17 lines)  AV=2.97  SV=4.50  BV=9.02
    {750,1140,1024, 0, 0, 0},  //TV = 10.38(19 lines)  AV=2.97  SV=4.44  BV=8.91
    {790,1140,1040, 0, 0, 0},  //TV = 10.31(20 lines)  AV=2.97  SV=4.46  BV=8.81
    {869,1140,1024, 0, 0, 0},  //TV = 10.17(22 lines)  AV=2.97  SV=4.44  BV=8.70
    {908,1140,1040, 0, 0, 0},  //TV = 10.11(23 lines)  AV=2.97  SV=4.46  BV=8.61
    {987,1140,1024, 0, 0, 0},  //TV = 9.98(25 lines)  AV=2.97  SV=4.44  BV=8.51
    {1066,1140,1024, 0, 0, 0},  //TV = 9.87(27 lines)  AV=2.97  SV=4.44  BV=8.40
    {1105,1140,1056, 0, 0, 0},  //TV = 9.82(28 lines)  AV=2.97  SV=4.49  BV=8.31
    {1224,1140,1024, 0, 0, 0},  //TV = 9.67(31 lines)  AV=2.97  SV=4.44  BV=8.20
    {1303,1140,1024, 0, 0, 0},  //TV = 9.58(33 lines)  AV=2.97  SV=4.44  BV=8.11
    {1382,1140,1040, 0, 0, 0},  //TV = 9.50(35 lines)  AV=2.97  SV=4.46  BV=8.01
    {1500,1140,1024, 0, 0, 0},  //TV = 9.38(38 lines)  AV=2.97  SV=4.44  BV=7.91
    {1618,1140,1024, 0, 0, 0},  //TV = 9.27(41 lines)  AV=2.97  SV=4.44  BV=7.80
    {1697,1140,1040, 0, 0, 0},  //TV = 9.20(43 lines)  AV=2.97  SV=4.46  BV=7.71
    {1855,1140,1024, 0, 0, 0},  //TV = 9.07(47 lines)  AV=2.97  SV=4.44  BV=7.60
    {1973,1140,1024, 0, 0, 0},  //TV = 8.99(50 lines)  AV=2.97  SV=4.44  BV=7.51
    {2131,1140,1024, 0, 0, 0},  //TV = 8.87(54 lines)  AV=2.97  SV=4.44  BV=7.40
    {2250,1140,1032, 0, 0, 0},  //TV = 8.80(57 lines)  AV=2.97  SV=4.45  BV=7.31
    {2447,1140,1024, 0, 0, 0},  //TV = 8.67(62 lines)  AV=2.97  SV=4.44  BV=7.20
    {2605,1140,1024, 0, 0, 0},  //TV = 8.58(66 lines)  AV=2.97  SV=4.44  BV=7.11
    {2802,1140,1024, 0, 0, 0},  //TV = 8.48(71 lines)  AV=2.97  SV=4.44  BV=7.01
    {2999,1140,1024, 0, 0, 0},  //TV = 8.38(76 lines)  AV=2.97  SV=4.44  BV=6.91
    {3236,1140,1024, 0, 0, 0},  //TV = 8.27(82 lines)  AV=2.97  SV=4.44  BV=6.80
    {3473,1140,1024, 0, 0, 0},  //TV = 8.17(88 lines)  AV=2.97  SV=4.44  BV=6.70
    {3710,1140,1024, 0, 0, 0},  //TV = 8.07(94 lines)  AV=2.97  SV=4.44  BV=6.60
    {3986,1140,1024, 0, 0, 0},  //TV = 7.97(101 lines)  AV=2.97  SV=4.44  BV=6.50
    {4262,1140,1024, 0, 0, 0},  //TV = 7.87(108 lines)  AV=2.97  SV=4.44  BV=6.40
    {4538,1140,1024, 0, 0, 0},  //TV = 7.78(115 lines)  AV=2.97  SV=4.44  BV=6.31
    {4893,1140,1024, 0, 0, 0},  //TV = 7.68(124 lines)  AV=2.97  SV=4.44  BV=6.20
    {5209,1140,1024, 0, 0, 0},  //TV = 7.58(132 lines)  AV=2.97  SV=4.44  BV=6.11
    {5643,1140,1024, 0, 0, 0},  //TV = 7.47(143 lines)  AV=2.97  SV=4.44  BV=6.00
    {6038,1140,1024, 0, 0, 0},  //TV = 7.37(153 lines)  AV=2.97  SV=4.44  BV=5.90
    {6472,1140,1024, 0, 0, 0},  //TV = 7.27(164 lines)  AV=2.97  SV=4.44  BV=5.80
    {6906,1140,1024, 0, 0, 0},  //TV = 7.18(175 lines)  AV=2.97  SV=4.44  BV=5.71
    {7419,1140,1024, 0, 0, 0},  //TV = 7.07(188 lines)  AV=2.97  SV=4.44  BV=5.60
    {7971,1140,1024, 0, 0, 0},  //TV = 6.97(202 lines)  AV=2.97  SV=4.44  BV=5.50
    {8484,1140,1024, 0, 0, 0},  //TV = 6.88(215 lines)  AV=2.97  SV=4.44  BV=5.41
    {9155,1140,1024, 0, 0, 0},  //TV = 6.77(232 lines)  AV=2.97  SV=4.44  BV=5.30
    {9826,1140,1024, 0, 0, 0},  //TV = 6.67(249 lines)  AV=2.97  SV=4.44  BV=5.20
    {9983,1140,1072, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=4.51  BV=5.11
    {9983,1216,1072, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=4.60  BV=5.02
    {9983,1328,1056, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=4.71  BV=4.91
    {9983,1424,1056, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=4.81  BV=4.81
    {9983,1536,1048, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=4.91  BV=4.71
    {9983,1632,1064, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=5.01  BV=4.60
    {9983,1728,1072, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=5.11  BV=4.51
    {9983,1936,1024, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=5.21  BV=4.41
    {9983,2048,1040, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=5.31  BV=4.31
    {9983,2144,1064, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=5.41  BV=4.21
    {20006,1140,1064, 0, 0, 0},  //TV = 5.64(507 lines)  AV=2.97  SV=4.50  BV=4.12
    {20006,1216,1072, 0, 0, 0},  //TV = 5.64(507 lines)  AV=2.97  SV=4.60  BV=4.01
    {20006,1328,1048, 0, 0, 0},  //TV = 5.64(507 lines)  AV=2.97  SV=4.70  BV=3.92
    {20006,1424,1064, 0, 0, 0},  //TV = 5.64(507 lines)  AV=2.97  SV=4.82  BV=3.80
    {20006,1536,1048, 0, 0, 0},  //TV = 5.64(507 lines)  AV=2.97  SV=4.91  BV=3.71
    {20006,1632,1056, 0, 0, 0},  //TV = 5.64(507 lines)  AV=2.97  SV=5.00  BV=3.61
    {29989,1140,1080, 0, 0, 0},  //TV = 5.06(760 lines)  AV=2.97  SV=4.52  BV=3.51
    {29989,1216,1096, 0, 0, 0},  //TV = 5.06(760 lines)  AV=2.97  SV=4.63  BV=3.40
    {29989,1328,1064, 0, 0, 0},  //TV = 5.06(760 lines)  AV=2.97  SV=4.72  BV=3.31
    {29989,1424,1072, 0, 0, 0},  //TV = 5.06(760 lines)  AV=2.97  SV=4.83  BV=3.20
    {40011,1140,1064, 0, 0, 0},  //TV = 4.64(1014 lines)  AV=2.97  SV=4.50  BV=3.12
    {40011,1216,1072, 0, 0, 0},  //TV = 4.64(1014 lines)  AV=2.97  SV=4.60  BV=3.01
    {40011,1328,1048, 0, 0, 0},  //TV = 4.64(1014 lines)  AV=2.97  SV=4.70  BV=2.92
    {49994,1140,1056, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.49  BV=2.81
    {49994,1216,1064, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.59  BV=2.70
    {49994,1328,1040, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.68  BV=2.61
    {49994,1424,1040, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.78  BV=2.51
    {49994,1536,1032, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.88  BV=2.41
    {49994,1632,1040, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.98  BV=2.31
    {49994,1728,1056, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.09  BV=2.21
    {49994,1840,1064, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.19  BV=2.11
    {49994,2048,1024, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.29  BV=2.01
    {49994,2144,1048, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.39  BV=1.91
    {49994,2240,1072, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.48  BV=1.81
    {49994,2448,1048, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.58  BV=1.72
    {49994,2656,1040, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.68  BV=1.61
    {49994,2864,1032, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.78  BV=1.51
    {49994,3072,1032, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.88  BV=1.41
    {49994,3328,1024, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.99  BV=1.31
    {49994,3456,1056, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=6.09  BV=1.21
    {60017,3072,1064, 0, 0, 0},  //TV = 4.06(1521 lines)  AV=2.97  SV=5.93  BV=1.10
    {60017,3328,1048, 0, 0, 0},  //TV = 4.06(1521 lines)  AV=2.97  SV=6.02  BV=1.01
    {69999,3072,1048, 0, 0, 0},  //TV = 3.84(1774 lines)  AV=2.97  SV=5.91  BV=0.90
    {69999,3328,1032, 0, 0, 0},  //TV = 3.84(1774 lines)  AV=2.97  SV=6.00  BV=0.81
    {79982,3072,1048, 0, 0, 0},  //TV = 3.64(2027 lines)  AV=2.97  SV=5.91  BV=0.71
    {79982,3328,1032, 0, 0, 0},  //TV = 3.64(2027 lines)  AV=2.97  SV=6.00  BV=0.62
    {79982,3584,1032, 0, 0, 0},  //TV = 3.64(2027 lines)  AV=2.97  SV=6.11  BV=0.51
    {79982,3840,1032, 0, 0, 0},  //TV = 3.64(2027 lines)  AV=2.97  SV=6.20  BV=0.41
    {79982,4096,1040, 0, 0, 0},  //TV = 3.64(2027 lines)  AV=2.97  SV=6.31  BV=0.31
    {79982,4352,1040, 0, 0, 0},  //TV = 3.64(2027 lines)  AV=2.97  SV=6.40  BV=0.22
    {90005,4224,1024, 0, 0, 0},  //TV = 3.47(2281 lines)  AV=2.97  SV=6.33  BV=0.11
    {99987,4096,1032, 0, 0, 0},  //TV = 3.32(2534 lines)  AV=2.97  SV=6.30  BV=-0.01
    {99987,4352,1040, 0, 0, 0},  //TV = 3.32(2534 lines)  AV=2.97  SV=6.40  BV=-0.10
    {110010,4224,1040, 0, 0, 0},  //TV = 3.18(2788 lines)  AV=2.97  SV=6.35  BV=-0.20
    {119992,4096,1056, 0, 0, 0},  //TV = 3.06(3041 lines)  AV=2.97  SV=6.33  BV=-0.30
    {119992,4480,1032, 0, 0, 0},  //TV = 3.06(3041 lines)  AV=2.97  SV=6.43  BV=-0.40
    {119992,4736,1048, 0, 0, 0},  //TV = 3.06(3041 lines)  AV=2.97  SV=6.53  BV=-0.50
    {119992,5120,1040, 0, 0, 0},  //TV = 3.06(3041 lines)  AV=2.97  SV=6.63  BV=-0.60
    {119992,5504,1040, 0, 0, 0},  //TV = 3.06(3041 lines)  AV=2.97  SV=6.74  BV=-0.71
    {119992,5888,1040, 0, 0, 0},  //TV = 3.06(3041 lines)  AV=2.97  SV=6.83  BV=-0.80
    {119992,6144,1064, 0, 0, 0},  //TV = 3.06(3041 lines)  AV=2.97  SV=6.93  BV=-0.90
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
},
};

static strAETable g_AE_CaptureISO100Table =
{
    AETABLE_CAPTURE_ISO100,    //eAETableID
    119,    //u4TotalIndex
    20,    //u4StrobeTrigerBV
    109,    //i4MaxBV
    -9,    //i4MinBV
    90,    //i4EffectiveMaxBV
    0,      //i4EffectiveMinBV
    LIB3A_AE_ISO_SPEED_100, //ISO SPEED
    sCaptureISO100PLineTable_60Hz,
    sCaptureISO100PLineTable_50Hz,
    NULL,
};

static strEvPline sCaptureISO200PLineTable_60Hz =
{
{
    {198,1140,1024, 0, 0, 0},  //TV = 12.30(5 lines)  AV=2.97  SV=4.44  BV=10.83
    {198,1140,1040, 0, 0, 0},  //TV = 12.30(5 lines)  AV=2.97  SV=4.46  BV=10.81
    {198,1216,1048, 0, 0, 0},  //TV = 12.30(5 lines)  AV=2.97  SV=4.57  BV=10.70
    {198,1328,1024, 0, 0, 0},  //TV = 12.30(5 lines)  AV=2.97  SV=4.66  BV=10.61
    {237,1140,1064, 0, 0, 0},  //TV = 12.04(6 lines)  AV=2.97  SV=4.50  BV=10.52
    {237,1216,1072, 0, 0, 0},  //TV = 12.04(6 lines)  AV=2.97  SV=4.60  BV=10.41
    {277,1140,1056, 0, 0, 0},  //TV = 11.82(7 lines)  AV=2.97  SV=4.49  BV=10.30
    {277,1216,1056, 0, 0, 0},  //TV = 11.82(7 lines)  AV=2.97  SV=4.58  BV=10.21
    {316,1140,1056, 0, 0, 0},  //TV = 11.63(8 lines)  AV=2.97  SV=4.49  BV=10.11
    {316,1216,1064, 0, 0, 0},  //TV = 11.63(8 lines)  AV=2.97  SV=4.59  BV=10.01
    {356,1140,1080, 0, 0, 0},  //TV = 11.46(9 lines)  AV=2.97  SV=4.52  BV=9.91
    {395,1140,1040, 0, 0, 0},  //TV = 11.31(10 lines)  AV=2.97  SV=4.46  BV=9.81
    {435,1140,1024, 0, 0, 0},  //TV = 11.17(11 lines)  AV=2.97  SV=4.44  BV=9.70
    {435,1140,1088, 0, 0, 0},  //TV = 11.17(11 lines)  AV=2.97  SV=4.53  BV=9.61
    {474,1140,1064, 0, 0, 0},  //TV = 11.04(12 lines)  AV=2.97  SV=4.50  BV=9.52
    {513,1140,1056, 0, 0, 0},  //TV = 10.93(13 lines)  AV=2.97  SV=4.49  BV=9.41
    {553,1140,1056, 0, 0, 0},  //TV = 10.82(14 lines)  AV=2.97  SV=4.49  BV=9.31
    {592,1140,1056, 0, 0, 0},  //TV = 10.72(15 lines)  AV=2.97  SV=4.49  BV=9.21
    {632,1140,1056, 0, 0, 0},  //TV = 10.63(16 lines)  AV=2.97  SV=4.49  BV=9.11
    {671,1140,1064, 0, 0, 0},  //TV = 10.54(17 lines)  AV=2.97  SV=4.50  BV=9.02
    {750,1140,1024, 0, 0, 0},  //TV = 10.38(19 lines)  AV=2.97  SV=4.44  BV=8.91
    {790,1140,1040, 0, 0, 0},  //TV = 10.31(20 lines)  AV=2.97  SV=4.46  BV=8.81
    {869,1140,1024, 0, 0, 0},  //TV = 10.17(22 lines)  AV=2.97  SV=4.44  BV=8.70
    {908,1140,1040, 0, 0, 0},  //TV = 10.11(23 lines)  AV=2.97  SV=4.46  BV=8.61
    {987,1140,1024, 0, 0, 0},  //TV = 9.98(25 lines)  AV=2.97  SV=4.44  BV=8.51
    {1066,1140,1024, 0, 0, 0},  //TV = 9.87(27 lines)  AV=2.97  SV=4.44  BV=8.40
    {1105,1140,1056, 0, 0, 0},  //TV = 9.82(28 lines)  AV=2.97  SV=4.49  BV=8.31
    {1224,1140,1024, 0, 0, 0},  //TV = 9.67(31 lines)  AV=2.97  SV=4.44  BV=8.20
    {1303,1140,1024, 0, 0, 0},  //TV = 9.58(33 lines)  AV=2.97  SV=4.44  BV=8.11
    {1382,1140,1040, 0, 0, 0},  //TV = 9.50(35 lines)  AV=2.97  SV=4.46  BV=8.01
    {1500,1140,1024, 0, 0, 0},  //TV = 9.38(38 lines)  AV=2.97  SV=4.44  BV=7.91
    {1618,1140,1024, 0, 0, 0},  //TV = 9.27(41 lines)  AV=2.97  SV=4.44  BV=7.80
    {1697,1140,1040, 0, 0, 0},  //TV = 9.20(43 lines)  AV=2.97  SV=4.46  BV=7.71
    {1855,1140,1024, 0, 0, 0},  //TV = 9.07(47 lines)  AV=2.97  SV=4.44  BV=7.60
    {1973,1140,1024, 0, 0, 0},  //TV = 8.99(50 lines)  AV=2.97  SV=4.44  BV=7.51
    {2131,1140,1024, 0, 0, 0},  //TV = 8.87(54 lines)  AV=2.97  SV=4.44  BV=7.40
    {2250,1140,1032, 0, 0, 0},  //TV = 8.80(57 lines)  AV=2.97  SV=4.45  BV=7.31
    {2447,1140,1024, 0, 0, 0},  //TV = 8.67(62 lines)  AV=2.97  SV=4.44  BV=7.20
    {2605,1140,1024, 0, 0, 0},  //TV = 8.58(66 lines)  AV=2.97  SV=4.44  BV=7.11
    {2802,1140,1024, 0, 0, 0},  //TV = 8.48(71 lines)  AV=2.97  SV=4.44  BV=7.01
    {2999,1140,1024, 0, 0, 0},  //TV = 8.38(76 lines)  AV=2.97  SV=4.44  BV=6.91
    {3236,1140,1024, 0, 0, 0},  //TV = 8.27(82 lines)  AV=2.97  SV=4.44  BV=6.80
    {3473,1140,1024, 0, 0, 0},  //TV = 8.17(88 lines)  AV=2.97  SV=4.44  BV=6.70
    {3710,1140,1024, 0, 0, 0},  //TV = 8.07(94 lines)  AV=2.97  SV=4.44  BV=6.60
    {3986,1140,1024, 0, 0, 0},  //TV = 7.97(101 lines)  AV=2.97  SV=4.44  BV=6.50
    {4262,1140,1024, 0, 0, 0},  //TV = 7.87(108 lines)  AV=2.97  SV=4.44  BV=6.40
    {4538,1140,1024, 0, 0, 0},  //TV = 7.78(115 lines)  AV=2.97  SV=4.44  BV=6.31
    {4893,1140,1024, 0, 0, 0},  //TV = 7.68(124 lines)  AV=2.97  SV=4.44  BV=6.20
    {5209,1140,1024, 0, 0, 0},  //TV = 7.58(132 lines)  AV=2.97  SV=4.44  BV=6.11
    {5643,1140,1024, 0, 0, 0},  //TV = 7.47(143 lines)  AV=2.97  SV=4.44  BV=6.00
    {6038,1140,1024, 0, 0, 0},  //TV = 7.37(153 lines)  AV=2.97  SV=4.44  BV=5.90
    {6472,1140,1024, 0, 0, 0},  //TV = 7.27(164 lines)  AV=2.97  SV=4.44  BV=5.80
    {6906,1140,1024, 0, 0, 0},  //TV = 7.18(175 lines)  AV=2.97  SV=4.44  BV=5.71
    {7419,1140,1024, 0, 0, 0},  //TV = 7.07(188 lines)  AV=2.97  SV=4.44  BV=5.60
    {7971,1140,1024, 0, 0, 0},  //TV = 6.97(202 lines)  AV=2.97  SV=4.44  BV=5.50
    {8326,1140,1048, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=4.48  BV=5.40
    {8326,1216,1048, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=4.57  BV=5.31
    {8326,1328,1024, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=4.66  BV=5.22
    {8326,1424,1024, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=4.76  BV=5.12
    {8326,1536,1024, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=4.87  BV=5.01
    {8326,1632,1032, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=4.97  BV=4.91
    {8326,1728,1040, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=5.06  BV=4.81
    {8326,1840,1048, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=5.17  BV=4.71
    {8326,1936,1072, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=5.27  BV=4.61
    {8326,2144,1032, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=5.36  BV=4.51
    {16652,1140,1048, 0, 0, 0},  //TV = 5.91(422 lines)  AV=2.97  SV=4.48  BV=4.40
    {16652,1216,1048, 0, 0, 0},  //TV = 5.91(422 lines)  AV=2.97  SV=4.57  BV=4.31
    {16652,1328,1024, 0, 0, 0},  //TV = 5.91(422 lines)  AV=2.97  SV=4.66  BV=4.22
    {16652,1424,1024, 0, 0, 0},  //TV = 5.91(422 lines)  AV=2.97  SV=4.76  BV=4.12
    {16652,1424,1104, 0, 0, 0},  //TV = 5.91(422 lines)  AV=2.97  SV=4.87  BV=4.01
    {16652,1632,1032, 0, 0, 0},  //TV = 5.91(422 lines)  AV=2.97  SV=4.97  BV=3.91
    {25017,1140,1056, 0, 0, 0},  //TV = 5.32(634 lines)  AV=2.97  SV=4.49  BV=3.81
    {25017,1216,1064, 0, 0, 0},  //TV = 5.32(634 lines)  AV=2.97  SV=4.59  BV=3.70
    {25017,1328,1048, 0, 0, 0},  //TV = 5.32(634 lines)  AV=2.97  SV=4.70  BV=3.60
    {25017,1424,1040, 0, 0, 0},  //TV = 5.32(634 lines)  AV=2.97  SV=4.78  BV=3.51
    {33343,1140,1048, 0, 0, 0},  //TV = 4.91(845 lines)  AV=2.97  SV=4.48  BV=3.40
    {33343,1216,1048, 0, 0, 0},  //TV = 4.91(845 lines)  AV=2.97  SV=4.57  BV=3.31
    {33343,1328,1024, 0, 0, 0},  //TV = 4.91(845 lines)  AV=2.97  SV=4.66  BV=3.22
    {41669,1140,1024, 0, 0, 0},  //TV = 4.58(1056 lines)  AV=2.97  SV=4.44  BV=3.11
    {41669,1216,1032, 0, 0, 0},  //TV = 4.58(1056 lines)  AV=2.97  SV=4.55  BV=3.01
    {41669,1216,1104, 0, 0, 0},  //TV = 4.58(1056 lines)  AV=2.97  SV=4.64  BV=2.91
    {49994,1140,1056, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.49  BV=2.81
    {49994,1216,1064, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.59  BV=2.70
    {49994,1328,1040, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.68  BV=2.61
    {49994,1424,1040, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.78  BV=2.51
    {49994,1536,1032, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.88  BV=2.41
    {49994,1632,1040, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.98  BV=2.31
    {49994,1728,1056, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.09  BV=2.21
    {49994,1840,1064, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.19  BV=2.11
    {49994,2048,1024, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.29  BV=2.01
    {49994,2144,1048, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.39  BV=1.91
    {49994,2240,1072, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.48  BV=1.81
    {49994,2448,1048, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.58  BV=1.72
    {49994,2656,1040, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.68  BV=1.61
    {49994,2864,1032, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.78  BV=1.51
    {49994,3072,1032, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.88  BV=1.41
    {49994,3328,1024, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.99  BV=1.31
    {49994,3456,1056, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=6.09  BV=1.21
    {58319,3200,1048, 0, 0, 0},  //TV = 4.10(1478 lines)  AV=2.97  SV=5.96  BV=1.11
    {66645,3072,1024, 0, 0, 0},  //TV = 3.91(1689 lines)  AV=2.97  SV=5.87  BV=1.01
    {66645,3200,1056, 0, 0, 0},  //TV = 3.91(1689 lines)  AV=2.97  SV=5.98  BV=0.90
    {75011,3072,1048, 0, 0, 0},  //TV = 3.74(1901 lines)  AV=2.97  SV=5.91  BV=0.80
    {75011,3328,1032, 0, 0, 0},  //TV = 3.74(1901 lines)  AV=2.97  SV=6.00  BV=0.71
    {83336,3200,1032, 0, 0, 0},  //TV = 3.58(2112 lines)  AV=2.97  SV=5.94  BV=0.61
    {83336,3456,1024, 0, 0, 0},  //TV = 3.58(2112 lines)  AV=2.97  SV=6.04  BV=0.51
    {83336,3712,1024, 0, 0, 0},  //TV = 3.58(2112 lines)  AV=2.97  SV=6.14  BV=0.41
    {83336,3968,1032, 0, 0, 0},  //TV = 3.58(2112 lines)  AV=2.97  SV=6.25  BV=0.30
    {83336,4224,1032, 0, 0, 0},  //TV = 3.58(2112 lines)  AV=2.97  SV=6.34  BV=0.21
    {91661,4096,1040, 0, 0, 0},  //TV = 3.45(2323 lines)  AV=2.97  SV=6.31  BV=0.11
    {99987,4096,1032, 0, 0, 0},  //TV = 3.32(2534 lines)  AV=2.97  SV=6.30  BV=-0.01
    {108313,4096,1024, 0, 0, 0},  //TV = 3.21(2745 lines)  AV=2.97  SV=6.29  BV=-0.11
    {108313,4352,1032, 0, 0, 0},  //TV = 3.21(2745 lines)  AV=2.97  SV=6.39  BV=-0.21
    {116678,4224,1048, 0, 0, 0},  //TV = 3.10(2957 lines)  AV=2.97  SV=6.36  BV=-0.29
    {125004,4224,1048, 0, 0, 0},  //TV = 3.00(3168 lines)  AV=2.97  SV=6.36  BV=-0.39
    {125004,4608,1032, 0, 0, 0},  //TV = 3.00(3168 lines)  AV=2.97  SV=6.47  BV=-0.50
    {125004,4864,1048, 0, 0, 0},  //TV = 3.00(3168 lines)  AV=2.97  SV=6.57  BV=-0.60
    {125004,5376,1024, 0, 0, 0},  //TV = 3.00(3168 lines)  AV=2.97  SV=6.68  BV=-0.71
    {125004,5632,1040, 0, 0, 0},  //TV = 3.00(3168 lines)  AV=2.97  SV=6.77  BV=-0.80
    {125004,6016,1040, 0, 0, 0},  //TV = 3.00(3168 lines)  AV=2.97  SV=6.86  BV=-0.89
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
},
};

static strEvPline sCaptureISO200PLineTable_50Hz =
{
{
    {198,1140,1024, 0, 0, 0},  //TV = 12.30(5 lines)  AV=2.97  SV=4.44  BV=10.83
    {198,1140,1040, 0, 0, 0},  //TV = 12.30(5 lines)  AV=2.97  SV=4.46  BV=10.81
    {198,1216,1048, 0, 0, 0},  //TV = 12.30(5 lines)  AV=2.97  SV=4.57  BV=10.70
    {198,1328,1024, 0, 0, 0},  //TV = 12.30(5 lines)  AV=2.97  SV=4.66  BV=10.61
    {237,1140,1064, 0, 0, 0},  //TV = 12.04(6 lines)  AV=2.97  SV=4.50  BV=10.52
    {237,1216,1072, 0, 0, 0},  //TV = 12.04(6 lines)  AV=2.97  SV=4.60  BV=10.41
    {277,1140,1056, 0, 0, 0},  //TV = 11.82(7 lines)  AV=2.97  SV=4.49  BV=10.30
    {277,1216,1056, 0, 0, 0},  //TV = 11.82(7 lines)  AV=2.97  SV=4.58  BV=10.21
    {316,1140,1056, 0, 0, 0},  //TV = 11.63(8 lines)  AV=2.97  SV=4.49  BV=10.11
    {316,1216,1064, 0, 0, 0},  //TV = 11.63(8 lines)  AV=2.97  SV=4.59  BV=10.01
    {356,1140,1080, 0, 0, 0},  //TV = 11.46(9 lines)  AV=2.97  SV=4.52  BV=9.91
    {395,1140,1040, 0, 0, 0},  //TV = 11.31(10 lines)  AV=2.97  SV=4.46  BV=9.81
    {435,1140,1024, 0, 0, 0},  //TV = 11.17(11 lines)  AV=2.97  SV=4.44  BV=9.70
    {435,1140,1088, 0, 0, 0},  //TV = 11.17(11 lines)  AV=2.97  SV=4.53  BV=9.61
    {474,1140,1064, 0, 0, 0},  //TV = 11.04(12 lines)  AV=2.97  SV=4.50  BV=9.52
    {513,1140,1056, 0, 0, 0},  //TV = 10.93(13 lines)  AV=2.97  SV=4.49  BV=9.41
    {553,1140,1056, 0, 0, 0},  //TV = 10.82(14 lines)  AV=2.97  SV=4.49  BV=9.31
    {592,1140,1056, 0, 0, 0},  //TV = 10.72(15 lines)  AV=2.97  SV=4.49  BV=9.21
    {632,1140,1056, 0, 0, 0},  //TV = 10.63(16 lines)  AV=2.97  SV=4.49  BV=9.11
    {671,1140,1064, 0, 0, 0},  //TV = 10.54(17 lines)  AV=2.97  SV=4.50  BV=9.02
    {750,1140,1024, 0, 0, 0},  //TV = 10.38(19 lines)  AV=2.97  SV=4.44  BV=8.91
    {790,1140,1040, 0, 0, 0},  //TV = 10.31(20 lines)  AV=2.97  SV=4.46  BV=8.81
    {869,1140,1024, 0, 0, 0},  //TV = 10.17(22 lines)  AV=2.97  SV=4.44  BV=8.70
    {908,1140,1040, 0, 0, 0},  //TV = 10.11(23 lines)  AV=2.97  SV=4.46  BV=8.61
    {987,1140,1024, 0, 0, 0},  //TV = 9.98(25 lines)  AV=2.97  SV=4.44  BV=8.51
    {1066,1140,1024, 0, 0, 0},  //TV = 9.87(27 lines)  AV=2.97  SV=4.44  BV=8.40
    {1105,1140,1056, 0, 0, 0},  //TV = 9.82(28 lines)  AV=2.97  SV=4.49  BV=8.31
    {1224,1140,1024, 0, 0, 0},  //TV = 9.67(31 lines)  AV=2.97  SV=4.44  BV=8.20
    {1303,1140,1024, 0, 0, 0},  //TV = 9.58(33 lines)  AV=2.97  SV=4.44  BV=8.11
    {1382,1140,1040, 0, 0, 0},  //TV = 9.50(35 lines)  AV=2.97  SV=4.46  BV=8.01
    {1500,1140,1024, 0, 0, 0},  //TV = 9.38(38 lines)  AV=2.97  SV=4.44  BV=7.91
    {1618,1140,1024, 0, 0, 0},  //TV = 9.27(41 lines)  AV=2.97  SV=4.44  BV=7.80
    {1697,1140,1040, 0, 0, 0},  //TV = 9.20(43 lines)  AV=2.97  SV=4.46  BV=7.71
    {1855,1140,1024, 0, 0, 0},  //TV = 9.07(47 lines)  AV=2.97  SV=4.44  BV=7.60
    {1973,1140,1024, 0, 0, 0},  //TV = 8.99(50 lines)  AV=2.97  SV=4.44  BV=7.51
    {2131,1140,1024, 0, 0, 0},  //TV = 8.87(54 lines)  AV=2.97  SV=4.44  BV=7.40
    {2250,1140,1032, 0, 0, 0},  //TV = 8.80(57 lines)  AV=2.97  SV=4.45  BV=7.31
    {2447,1140,1024, 0, 0, 0},  //TV = 8.67(62 lines)  AV=2.97  SV=4.44  BV=7.20
    {2605,1140,1024, 0, 0, 0},  //TV = 8.58(66 lines)  AV=2.97  SV=4.44  BV=7.11
    {2802,1140,1024, 0, 0, 0},  //TV = 8.48(71 lines)  AV=2.97  SV=4.44  BV=7.01
    {2999,1140,1024, 0, 0, 0},  //TV = 8.38(76 lines)  AV=2.97  SV=4.44  BV=6.91
    {3236,1140,1024, 0, 0, 0},  //TV = 8.27(82 lines)  AV=2.97  SV=4.44  BV=6.80
    {3473,1140,1024, 0, 0, 0},  //TV = 8.17(88 lines)  AV=2.97  SV=4.44  BV=6.70
    {3710,1140,1024, 0, 0, 0},  //TV = 8.07(94 lines)  AV=2.97  SV=4.44  BV=6.60
    {3986,1140,1024, 0, 0, 0},  //TV = 7.97(101 lines)  AV=2.97  SV=4.44  BV=6.50
    {4262,1140,1024, 0, 0, 0},  //TV = 7.87(108 lines)  AV=2.97  SV=4.44  BV=6.40
    {4538,1140,1024, 0, 0, 0},  //TV = 7.78(115 lines)  AV=2.97  SV=4.44  BV=6.31
    {4893,1140,1024, 0, 0, 0},  //TV = 7.68(124 lines)  AV=2.97  SV=4.44  BV=6.20
    {5209,1140,1024, 0, 0, 0},  //TV = 7.58(132 lines)  AV=2.97  SV=4.44  BV=6.11
    {5643,1140,1024, 0, 0, 0},  //TV = 7.47(143 lines)  AV=2.97  SV=4.44  BV=6.00
    {6038,1140,1024, 0, 0, 0},  //TV = 7.37(153 lines)  AV=2.97  SV=4.44  BV=5.90
    {6472,1140,1024, 0, 0, 0},  //TV = 7.27(164 lines)  AV=2.97  SV=4.44  BV=5.80
    {6906,1140,1024, 0, 0, 0},  //TV = 7.18(175 lines)  AV=2.97  SV=4.44  BV=5.71
    {7419,1140,1024, 0, 0, 0},  //TV = 7.07(188 lines)  AV=2.97  SV=4.44  BV=5.60
    {7971,1140,1024, 0, 0, 0},  //TV = 6.97(202 lines)  AV=2.97  SV=4.44  BV=5.50
    {8484,1140,1024, 0, 0, 0},  //TV = 6.88(215 lines)  AV=2.97  SV=4.44  BV=5.41
    {9155,1140,1024, 0, 0, 0},  //TV = 6.77(232 lines)  AV=2.97  SV=4.44  BV=5.30
    {9826,1140,1024, 0, 0, 0},  //TV = 6.67(249 lines)  AV=2.97  SV=4.44  BV=5.20
    {9983,1140,1072, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=4.51  BV=5.11
    {9983,1216,1072, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=4.60  BV=5.02
    {9983,1328,1056, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=4.71  BV=4.91
    {9983,1424,1056, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=4.81  BV=4.81
    {9983,1536,1048, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=4.91  BV=4.71
    {9983,1632,1064, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=5.01  BV=4.60
    {9983,1728,1072, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=5.11  BV=4.51
    {9983,1936,1024, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=5.21  BV=4.41
    {9983,2048,1040, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=5.31  BV=4.31
    {9983,2144,1064, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=5.41  BV=4.21
    {20006,1140,1064, 0, 0, 0},  //TV = 5.64(507 lines)  AV=2.97  SV=4.50  BV=4.12
    {20006,1216,1072, 0, 0, 0},  //TV = 5.64(507 lines)  AV=2.97  SV=4.60  BV=4.01
    {20006,1328,1048, 0, 0, 0},  //TV = 5.64(507 lines)  AV=2.97  SV=4.70  BV=3.92
    {20006,1424,1064, 0, 0, 0},  //TV = 5.64(507 lines)  AV=2.97  SV=4.82  BV=3.80
    {20006,1536,1048, 0, 0, 0},  //TV = 5.64(507 lines)  AV=2.97  SV=4.91  BV=3.71
    {20006,1632,1056, 0, 0, 0},  //TV = 5.64(507 lines)  AV=2.97  SV=5.00  BV=3.61
    {29989,1140,1080, 0, 0, 0},  //TV = 5.06(760 lines)  AV=2.97  SV=4.52  BV=3.51
    {29989,1216,1096, 0, 0, 0},  //TV = 5.06(760 lines)  AV=2.97  SV=4.63  BV=3.40
    {29989,1328,1064, 0, 0, 0},  //TV = 5.06(760 lines)  AV=2.97  SV=4.72  BV=3.31
    {29989,1424,1072, 0, 0, 0},  //TV = 5.06(760 lines)  AV=2.97  SV=4.83  BV=3.20
    {40011,1140,1064, 0, 0, 0},  //TV = 4.64(1014 lines)  AV=2.97  SV=4.50  BV=3.12
    {40011,1216,1072, 0, 0, 0},  //TV = 4.64(1014 lines)  AV=2.97  SV=4.60  BV=3.01
    {40011,1328,1048, 0, 0, 0},  //TV = 4.64(1014 lines)  AV=2.97  SV=4.70  BV=2.92
    {49994,1140,1056, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.49  BV=2.81
    {49994,1216,1064, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.59  BV=2.70
    {49994,1328,1040, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.68  BV=2.61
    {49994,1424,1040, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.78  BV=2.51
    {49994,1536,1032, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.88  BV=2.41
    {49994,1632,1040, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.98  BV=2.31
    {49994,1728,1056, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.09  BV=2.21
    {49994,1840,1064, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.19  BV=2.11
    {49994,2048,1024, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.29  BV=2.01
    {49994,2144,1048, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.39  BV=1.91
    {49994,2240,1072, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.48  BV=1.81
    {49994,2448,1048, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.58  BV=1.72
    {49994,2656,1040, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.68  BV=1.61
    {49994,2864,1032, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.78  BV=1.51
    {49994,3072,1032, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.88  BV=1.41
    {49994,3328,1024, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.99  BV=1.31
    {49994,3456,1056, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=6.09  BV=1.21
    {60017,3072,1064, 0, 0, 0},  //TV = 4.06(1521 lines)  AV=2.97  SV=5.93  BV=1.10
    {60017,3328,1048, 0, 0, 0},  //TV = 4.06(1521 lines)  AV=2.97  SV=6.02  BV=1.01
    {69999,3072,1048, 0, 0, 0},  //TV = 3.84(1774 lines)  AV=2.97  SV=5.91  BV=0.90
    {69999,3328,1032, 0, 0, 0},  //TV = 3.84(1774 lines)  AV=2.97  SV=6.00  BV=0.81
    {79982,3072,1048, 0, 0, 0},  //TV = 3.64(2027 lines)  AV=2.97  SV=5.91  BV=0.71
    {79982,3328,1032, 0, 0, 0},  //TV = 3.64(2027 lines)  AV=2.97  SV=6.00  BV=0.62
    {79982,3584,1032, 0, 0, 0},  //TV = 3.64(2027 lines)  AV=2.97  SV=6.11  BV=0.51
    {79982,3840,1032, 0, 0, 0},  //TV = 3.64(2027 lines)  AV=2.97  SV=6.20  BV=0.41
    {79982,4096,1040, 0, 0, 0},  //TV = 3.64(2027 lines)  AV=2.97  SV=6.31  BV=0.31
    {79982,4352,1040, 0, 0, 0},  //TV = 3.64(2027 lines)  AV=2.97  SV=6.40  BV=0.22
    {90005,4224,1024, 0, 0, 0},  //TV = 3.47(2281 lines)  AV=2.97  SV=6.33  BV=0.11
    {99987,4096,1032, 0, 0, 0},  //TV = 3.32(2534 lines)  AV=2.97  SV=6.30  BV=-0.01
    {99987,4352,1040, 0, 0, 0},  //TV = 3.32(2534 lines)  AV=2.97  SV=6.40  BV=-0.10
    {110010,4224,1040, 0, 0, 0},  //TV = 3.18(2788 lines)  AV=2.97  SV=6.35  BV=-0.20
    {119992,4096,1056, 0, 0, 0},  //TV = 3.06(3041 lines)  AV=2.97  SV=6.33  BV=-0.30
    {119992,4480,1032, 0, 0, 0},  //TV = 3.06(3041 lines)  AV=2.97  SV=6.43  BV=-0.40
    {119992,4736,1048, 0, 0, 0},  //TV = 3.06(3041 lines)  AV=2.97  SV=6.53  BV=-0.50
    {119992,5120,1040, 0, 0, 0},  //TV = 3.06(3041 lines)  AV=2.97  SV=6.63  BV=-0.60
    {119992,5504,1040, 0, 0, 0},  //TV = 3.06(3041 lines)  AV=2.97  SV=6.74  BV=-0.71
    {119992,5888,1040, 0, 0, 0},  //TV = 3.06(3041 lines)  AV=2.97  SV=6.83  BV=-0.80
    {119992,6144,1064, 0, 0, 0},  //TV = 3.06(3041 lines)  AV=2.97  SV=6.93  BV=-0.90
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
},
};

static strAETable g_AE_CaptureISO200Table =
{
    AETABLE_CAPTURE_ISO200,    //eAETableID
    119,    //u4TotalIndex
    20,    //u4StrobeTrigerBV
    109,    //i4MaxBV
    -9,    //i4MinBV
    90,    //i4EffectiveMaxBV
    0,      //i4EffectiveMinBV
    LIB3A_AE_ISO_SPEED_200, //ISO SPEED
    sCaptureISO200PLineTable_60Hz,
    sCaptureISO200PLineTable_50Hz,
    NULL,
};

static strEvPline sCaptureISO400PLineTable_60Hz =
{
{
    {198,1140,1024, 0, 0, 0},  //TV = 12.30(5 lines)  AV=2.97  SV=4.44  BV=10.83
    {198,1140,1040, 0, 0, 0},  //TV = 12.30(5 lines)  AV=2.97  SV=4.46  BV=10.81
    {198,1216,1048, 0, 0, 0},  //TV = 12.30(5 lines)  AV=2.97  SV=4.57  BV=10.70
    {198,1328,1024, 0, 0, 0},  //TV = 12.30(5 lines)  AV=2.97  SV=4.66  BV=10.61
    {237,1140,1064, 0, 0, 0},  //TV = 12.04(6 lines)  AV=2.97  SV=4.50  BV=10.52
    {237,1216,1072, 0, 0, 0},  //TV = 12.04(6 lines)  AV=2.97  SV=4.60  BV=10.41
    {277,1140,1056, 0, 0, 0},  //TV = 11.82(7 lines)  AV=2.97  SV=4.49  BV=10.30
    {277,1216,1056, 0, 0, 0},  //TV = 11.82(7 lines)  AV=2.97  SV=4.58  BV=10.21
    {316,1140,1056, 0, 0, 0},  //TV = 11.63(8 lines)  AV=2.97  SV=4.49  BV=10.11
    {316,1216,1064, 0, 0, 0},  //TV = 11.63(8 lines)  AV=2.97  SV=4.59  BV=10.01
    {356,1140,1080, 0, 0, 0},  //TV = 11.46(9 lines)  AV=2.97  SV=4.52  BV=9.91
    {395,1140,1040, 0, 0, 0},  //TV = 11.31(10 lines)  AV=2.97  SV=4.46  BV=9.81
    {435,1140,1024, 0, 0, 0},  //TV = 11.17(11 lines)  AV=2.97  SV=4.44  BV=9.70
    {435,1140,1088, 0, 0, 0},  //TV = 11.17(11 lines)  AV=2.97  SV=4.53  BV=9.61
    {474,1140,1064, 0, 0, 0},  //TV = 11.04(12 lines)  AV=2.97  SV=4.50  BV=9.52
    {513,1140,1056, 0, 0, 0},  //TV = 10.93(13 lines)  AV=2.97  SV=4.49  BV=9.41
    {553,1140,1056, 0, 0, 0},  //TV = 10.82(14 lines)  AV=2.97  SV=4.49  BV=9.31
    {592,1140,1056, 0, 0, 0},  //TV = 10.72(15 lines)  AV=2.97  SV=4.49  BV=9.21
    {632,1140,1056, 0, 0, 0},  //TV = 10.63(16 lines)  AV=2.97  SV=4.49  BV=9.11
    {671,1140,1064, 0, 0, 0},  //TV = 10.54(17 lines)  AV=2.97  SV=4.50  BV=9.02
    {750,1140,1024, 0, 0, 0},  //TV = 10.38(19 lines)  AV=2.97  SV=4.44  BV=8.91
    {790,1140,1040, 0, 0, 0},  //TV = 10.31(20 lines)  AV=2.97  SV=4.46  BV=8.81
    {869,1140,1024, 0, 0, 0},  //TV = 10.17(22 lines)  AV=2.97  SV=4.44  BV=8.70
    {908,1140,1040, 0, 0, 0},  //TV = 10.11(23 lines)  AV=2.97  SV=4.46  BV=8.61
    {987,1140,1024, 0, 0, 0},  //TV = 9.98(25 lines)  AV=2.97  SV=4.44  BV=8.51
    {1066,1140,1024, 0, 0, 0},  //TV = 9.87(27 lines)  AV=2.97  SV=4.44  BV=8.40
    {1105,1140,1056, 0, 0, 0},  //TV = 9.82(28 lines)  AV=2.97  SV=4.49  BV=8.31
    {1224,1140,1024, 0, 0, 0},  //TV = 9.67(31 lines)  AV=2.97  SV=4.44  BV=8.20
    {1303,1140,1024, 0, 0, 0},  //TV = 9.58(33 lines)  AV=2.97  SV=4.44  BV=8.11
    {1382,1140,1040, 0, 0, 0},  //TV = 9.50(35 lines)  AV=2.97  SV=4.46  BV=8.01
    {1500,1140,1024, 0, 0, 0},  //TV = 9.38(38 lines)  AV=2.97  SV=4.44  BV=7.91
    {1618,1140,1024, 0, 0, 0},  //TV = 9.27(41 lines)  AV=2.97  SV=4.44  BV=7.80
    {1697,1140,1040, 0, 0, 0},  //TV = 9.20(43 lines)  AV=2.97  SV=4.46  BV=7.71
    {1855,1140,1024, 0, 0, 0},  //TV = 9.07(47 lines)  AV=2.97  SV=4.44  BV=7.60
    {1973,1140,1024, 0, 0, 0},  //TV = 8.99(50 lines)  AV=2.97  SV=4.44  BV=7.51
    {2131,1140,1024, 0, 0, 0},  //TV = 8.87(54 lines)  AV=2.97  SV=4.44  BV=7.40
    {2250,1140,1032, 0, 0, 0},  //TV = 8.80(57 lines)  AV=2.97  SV=4.45  BV=7.31
    {2447,1140,1024, 0, 0, 0},  //TV = 8.67(62 lines)  AV=2.97  SV=4.44  BV=7.20
    {2605,1140,1024, 0, 0, 0},  //TV = 8.58(66 lines)  AV=2.97  SV=4.44  BV=7.11
    {2802,1140,1024, 0, 0, 0},  //TV = 8.48(71 lines)  AV=2.97  SV=4.44  BV=7.01
    {2999,1140,1024, 0, 0, 0},  //TV = 8.38(76 lines)  AV=2.97  SV=4.44  BV=6.91
    {3236,1140,1024, 0, 0, 0},  //TV = 8.27(82 lines)  AV=2.97  SV=4.44  BV=6.80
    {3473,1140,1024, 0, 0, 0},  //TV = 8.17(88 lines)  AV=2.97  SV=4.44  BV=6.70
    {3710,1140,1024, 0, 0, 0},  //TV = 8.07(94 lines)  AV=2.97  SV=4.44  BV=6.60
    {3986,1140,1024, 0, 0, 0},  //TV = 7.97(101 lines)  AV=2.97  SV=4.44  BV=6.50
    {4262,1140,1024, 0, 0, 0},  //TV = 7.87(108 lines)  AV=2.97  SV=4.44  BV=6.40
    {4538,1140,1024, 0, 0, 0},  //TV = 7.78(115 lines)  AV=2.97  SV=4.44  BV=6.31
    {4893,1140,1024, 0, 0, 0},  //TV = 7.68(124 lines)  AV=2.97  SV=4.44  BV=6.20
    {5209,1140,1024, 0, 0, 0},  //TV = 7.58(132 lines)  AV=2.97  SV=4.44  BV=6.11
    {5643,1140,1024, 0, 0, 0},  //TV = 7.47(143 lines)  AV=2.97  SV=4.44  BV=6.00
    {6038,1140,1024, 0, 0, 0},  //TV = 7.37(153 lines)  AV=2.97  SV=4.44  BV=5.90
    {6472,1140,1024, 0, 0, 0},  //TV = 7.27(164 lines)  AV=2.97  SV=4.44  BV=5.80
    {6906,1140,1024, 0, 0, 0},  //TV = 7.18(175 lines)  AV=2.97  SV=4.44  BV=5.71
    {7419,1140,1024, 0, 0, 0},  //TV = 7.07(188 lines)  AV=2.97  SV=4.44  BV=5.60
    {7971,1140,1024, 0, 0, 0},  //TV = 6.97(202 lines)  AV=2.97  SV=4.44  BV=5.50
    {8326,1140,1048, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=4.48  BV=5.40
    {8326,1216,1048, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=4.57  BV=5.31
    {8326,1328,1024, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=4.66  BV=5.22
    {8326,1424,1024, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=4.76  BV=5.12
    {8326,1536,1024, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=4.87  BV=5.01
    {8326,1632,1032, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=4.97  BV=4.91
    {8326,1728,1040, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=5.06  BV=4.81
    {8326,1840,1048, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=5.17  BV=4.71
    {8326,1936,1072, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=5.27  BV=4.61
    {8326,2144,1032, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=5.36  BV=4.51
    {16652,1140,1048, 0, 0, 0},  //TV = 5.91(422 lines)  AV=2.97  SV=4.48  BV=4.40
    {16652,1216,1048, 0, 0, 0},  //TV = 5.91(422 lines)  AV=2.97  SV=4.57  BV=4.31
    {16652,1328,1024, 0, 0, 0},  //TV = 5.91(422 lines)  AV=2.97  SV=4.66  BV=4.22
    {16652,1424,1024, 0, 0, 0},  //TV = 5.91(422 lines)  AV=2.97  SV=4.76  BV=4.12
    {16652,1424,1104, 0, 0, 0},  //TV = 5.91(422 lines)  AV=2.97  SV=4.87  BV=4.01
    {16652,1632,1032, 0, 0, 0},  //TV = 5.91(422 lines)  AV=2.97  SV=4.97  BV=3.91
    {25017,1140,1056, 0, 0, 0},  //TV = 5.32(634 lines)  AV=2.97  SV=4.49  BV=3.81
    {25017,1216,1064, 0, 0, 0},  //TV = 5.32(634 lines)  AV=2.97  SV=4.59  BV=3.70
    {25017,1328,1048, 0, 0, 0},  //TV = 5.32(634 lines)  AV=2.97  SV=4.70  BV=3.60
    {25017,1424,1040, 0, 0, 0},  //TV = 5.32(634 lines)  AV=2.97  SV=4.78  BV=3.51
    {33343,1140,1048, 0, 0, 0},  //TV = 4.91(845 lines)  AV=2.97  SV=4.48  BV=3.40
    {33343,1216,1048, 0, 0, 0},  //TV = 4.91(845 lines)  AV=2.97  SV=4.57  BV=3.31
    {33343,1328,1024, 0, 0, 0},  //TV = 4.91(845 lines)  AV=2.97  SV=4.66  BV=3.22
    {41669,1140,1024, 0, 0, 0},  //TV = 4.58(1056 lines)  AV=2.97  SV=4.44  BV=3.11
    {41669,1216,1032, 0, 0, 0},  //TV = 4.58(1056 lines)  AV=2.97  SV=4.55  BV=3.01
    {41669,1216,1104, 0, 0, 0},  //TV = 4.58(1056 lines)  AV=2.97  SV=4.64  BV=2.91
    {49994,1140,1056, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.49  BV=2.81
    {49994,1216,1064, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.59  BV=2.70
    {49994,1328,1040, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.68  BV=2.61
    {49994,1424,1040, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.78  BV=2.51
    {49994,1536,1032, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.88  BV=2.41
    {49994,1632,1040, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.98  BV=2.31
    {49994,1728,1056, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.09  BV=2.21
    {49994,1840,1064, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.19  BV=2.11
    {49994,2048,1024, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.29  BV=2.01
    {49994,2144,1048, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.39  BV=1.91
    {49994,2240,1072, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.48  BV=1.81
    {49994,2448,1048, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.58  BV=1.72
    {49994,2656,1040, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.68  BV=1.61
    {49994,2864,1032, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.78  BV=1.51
    {49994,3072,1032, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.88  BV=1.41
    {49994,3328,1024, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.99  BV=1.31
    {49994,3456,1056, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=6.09  BV=1.21
    {58319,3200,1048, 0, 0, 0},  //TV = 4.10(1478 lines)  AV=2.97  SV=5.96  BV=1.11
    {66645,3072,1024, 0, 0, 0},  //TV = 3.91(1689 lines)  AV=2.97  SV=5.87  BV=1.01
    {66645,3200,1056, 0, 0, 0},  //TV = 3.91(1689 lines)  AV=2.97  SV=5.98  BV=0.90
    {75011,3072,1048, 0, 0, 0},  //TV = 3.74(1901 lines)  AV=2.97  SV=5.91  BV=0.80
    {75011,3328,1032, 0, 0, 0},  //TV = 3.74(1901 lines)  AV=2.97  SV=6.00  BV=0.71
    {83336,3200,1032, 0, 0, 0},  //TV = 3.58(2112 lines)  AV=2.97  SV=5.94  BV=0.61
    {83336,3456,1024, 0, 0, 0},  //TV = 3.58(2112 lines)  AV=2.97  SV=6.04  BV=0.51
    {83336,3712,1024, 0, 0, 0},  //TV = 3.58(2112 lines)  AV=2.97  SV=6.14  BV=0.41
    {83336,3968,1032, 0, 0, 0},  //TV = 3.58(2112 lines)  AV=2.97  SV=6.25  BV=0.30
    {83336,4224,1032, 0, 0, 0},  //TV = 3.58(2112 lines)  AV=2.97  SV=6.34  BV=0.21
    {91661,4096,1040, 0, 0, 0},  //TV = 3.45(2323 lines)  AV=2.97  SV=6.31  BV=0.11
    {99987,4096,1032, 0, 0, 0},  //TV = 3.32(2534 lines)  AV=2.97  SV=6.30  BV=-0.01
    {108313,4096,1024, 0, 0, 0},  //TV = 3.21(2745 lines)  AV=2.97  SV=6.29  BV=-0.11
    {108313,4352,1032, 0, 0, 0},  //TV = 3.21(2745 lines)  AV=2.97  SV=6.39  BV=-0.21
    {116678,4224,1048, 0, 0, 0},  //TV = 3.10(2957 lines)  AV=2.97  SV=6.36  BV=-0.29
    {125004,4224,1048, 0, 0, 0},  //TV = 3.00(3168 lines)  AV=2.97  SV=6.36  BV=-0.39
    {125004,4608,1032, 0, 0, 0},  //TV = 3.00(3168 lines)  AV=2.97  SV=6.47  BV=-0.50
    {125004,4864,1048, 0, 0, 0},  //TV = 3.00(3168 lines)  AV=2.97  SV=6.57  BV=-0.60
    {125004,5376,1024, 0, 0, 0},  //TV = 3.00(3168 lines)  AV=2.97  SV=6.68  BV=-0.71
    {125004,5632,1040, 0, 0, 0},  //TV = 3.00(3168 lines)  AV=2.97  SV=6.77  BV=-0.80
    {125004,6016,1040, 0, 0, 0},  //TV = 3.00(3168 lines)  AV=2.97  SV=6.86  BV=-0.89
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
},
};

static strEvPline sCaptureISO400PLineTable_50Hz =
{
{
    {198,1140,1024, 0, 0, 0},  //TV = 12.30(5 lines)  AV=2.97  SV=4.44  BV=10.83
    {198,1140,1040, 0, 0, 0},  //TV = 12.30(5 lines)  AV=2.97  SV=4.46  BV=10.81
    {198,1216,1048, 0, 0, 0},  //TV = 12.30(5 lines)  AV=2.97  SV=4.57  BV=10.70
    {198,1328,1024, 0, 0, 0},  //TV = 12.30(5 lines)  AV=2.97  SV=4.66  BV=10.61
    {237,1140,1064, 0, 0, 0},  //TV = 12.04(6 lines)  AV=2.97  SV=4.50  BV=10.52
    {237,1216,1072, 0, 0, 0},  //TV = 12.04(6 lines)  AV=2.97  SV=4.60  BV=10.41
    {277,1140,1056, 0, 0, 0},  //TV = 11.82(7 lines)  AV=2.97  SV=4.49  BV=10.30
    {277,1216,1056, 0, 0, 0},  //TV = 11.82(7 lines)  AV=2.97  SV=4.58  BV=10.21
    {316,1140,1056, 0, 0, 0},  //TV = 11.63(8 lines)  AV=2.97  SV=4.49  BV=10.11
    {316,1216,1064, 0, 0, 0},  //TV = 11.63(8 lines)  AV=2.97  SV=4.59  BV=10.01
    {356,1140,1080, 0, 0, 0},  //TV = 11.46(9 lines)  AV=2.97  SV=4.52  BV=9.91
    {395,1140,1040, 0, 0, 0},  //TV = 11.31(10 lines)  AV=2.97  SV=4.46  BV=9.81
    {435,1140,1024, 0, 0, 0},  //TV = 11.17(11 lines)  AV=2.97  SV=4.44  BV=9.70
    {435,1140,1088, 0, 0, 0},  //TV = 11.17(11 lines)  AV=2.97  SV=4.53  BV=9.61
    {474,1140,1064, 0, 0, 0},  //TV = 11.04(12 lines)  AV=2.97  SV=4.50  BV=9.52
    {513,1140,1056, 0, 0, 0},  //TV = 10.93(13 lines)  AV=2.97  SV=4.49  BV=9.41
    {553,1140,1056, 0, 0, 0},  //TV = 10.82(14 lines)  AV=2.97  SV=4.49  BV=9.31
    {592,1140,1056, 0, 0, 0},  //TV = 10.72(15 lines)  AV=2.97  SV=4.49  BV=9.21
    {632,1140,1056, 0, 0, 0},  //TV = 10.63(16 lines)  AV=2.97  SV=4.49  BV=9.11
    {671,1140,1064, 0, 0, 0},  //TV = 10.54(17 lines)  AV=2.97  SV=4.50  BV=9.02
    {750,1140,1024, 0, 0, 0},  //TV = 10.38(19 lines)  AV=2.97  SV=4.44  BV=8.91
    {790,1140,1040, 0, 0, 0},  //TV = 10.31(20 lines)  AV=2.97  SV=4.46  BV=8.81
    {869,1140,1024, 0, 0, 0},  //TV = 10.17(22 lines)  AV=2.97  SV=4.44  BV=8.70
    {908,1140,1040, 0, 0, 0},  //TV = 10.11(23 lines)  AV=2.97  SV=4.46  BV=8.61
    {987,1140,1024, 0, 0, 0},  //TV = 9.98(25 lines)  AV=2.97  SV=4.44  BV=8.51
    {1066,1140,1024, 0, 0, 0},  //TV = 9.87(27 lines)  AV=2.97  SV=4.44  BV=8.40
    {1105,1140,1056, 0, 0, 0},  //TV = 9.82(28 lines)  AV=2.97  SV=4.49  BV=8.31
    {1224,1140,1024, 0, 0, 0},  //TV = 9.67(31 lines)  AV=2.97  SV=4.44  BV=8.20
    {1303,1140,1024, 0, 0, 0},  //TV = 9.58(33 lines)  AV=2.97  SV=4.44  BV=8.11
    {1382,1140,1040, 0, 0, 0},  //TV = 9.50(35 lines)  AV=2.97  SV=4.46  BV=8.01
    {1500,1140,1024, 0, 0, 0},  //TV = 9.38(38 lines)  AV=2.97  SV=4.44  BV=7.91
    {1618,1140,1024, 0, 0, 0},  //TV = 9.27(41 lines)  AV=2.97  SV=4.44  BV=7.80
    {1697,1140,1040, 0, 0, 0},  //TV = 9.20(43 lines)  AV=2.97  SV=4.46  BV=7.71
    {1855,1140,1024, 0, 0, 0},  //TV = 9.07(47 lines)  AV=2.97  SV=4.44  BV=7.60
    {1973,1140,1024, 0, 0, 0},  //TV = 8.99(50 lines)  AV=2.97  SV=4.44  BV=7.51
    {2131,1140,1024, 0, 0, 0},  //TV = 8.87(54 lines)  AV=2.97  SV=4.44  BV=7.40
    {2250,1140,1032, 0, 0, 0},  //TV = 8.80(57 lines)  AV=2.97  SV=4.45  BV=7.31
    {2447,1140,1024, 0, 0, 0},  //TV = 8.67(62 lines)  AV=2.97  SV=4.44  BV=7.20
    {2605,1140,1024, 0, 0, 0},  //TV = 8.58(66 lines)  AV=2.97  SV=4.44  BV=7.11
    {2802,1140,1024, 0, 0, 0},  //TV = 8.48(71 lines)  AV=2.97  SV=4.44  BV=7.01
    {2999,1140,1024, 0, 0, 0},  //TV = 8.38(76 lines)  AV=2.97  SV=4.44  BV=6.91
    {3236,1140,1024, 0, 0, 0},  //TV = 8.27(82 lines)  AV=2.97  SV=4.44  BV=6.80
    {3473,1140,1024, 0, 0, 0},  //TV = 8.17(88 lines)  AV=2.97  SV=4.44  BV=6.70
    {3710,1140,1024, 0, 0, 0},  //TV = 8.07(94 lines)  AV=2.97  SV=4.44  BV=6.60
    {3986,1140,1024, 0, 0, 0},  //TV = 7.97(101 lines)  AV=2.97  SV=4.44  BV=6.50
    {4262,1140,1024, 0, 0, 0},  //TV = 7.87(108 lines)  AV=2.97  SV=4.44  BV=6.40
    {4538,1140,1024, 0, 0, 0},  //TV = 7.78(115 lines)  AV=2.97  SV=4.44  BV=6.31
    {4893,1140,1024, 0, 0, 0},  //TV = 7.68(124 lines)  AV=2.97  SV=4.44  BV=6.20
    {5209,1140,1024, 0, 0, 0},  //TV = 7.58(132 lines)  AV=2.97  SV=4.44  BV=6.11
    {5643,1140,1024, 0, 0, 0},  //TV = 7.47(143 lines)  AV=2.97  SV=4.44  BV=6.00
    {6038,1140,1024, 0, 0, 0},  //TV = 7.37(153 lines)  AV=2.97  SV=4.44  BV=5.90
    {6472,1140,1024, 0, 0, 0},  //TV = 7.27(164 lines)  AV=2.97  SV=4.44  BV=5.80
    {6906,1140,1024, 0, 0, 0},  //TV = 7.18(175 lines)  AV=2.97  SV=4.44  BV=5.71
    {7419,1140,1024, 0, 0, 0},  //TV = 7.07(188 lines)  AV=2.97  SV=4.44  BV=5.60
    {7971,1140,1024, 0, 0, 0},  //TV = 6.97(202 lines)  AV=2.97  SV=4.44  BV=5.50
    {8484,1140,1024, 0, 0, 0},  //TV = 6.88(215 lines)  AV=2.97  SV=4.44  BV=5.41
    {9155,1140,1024, 0, 0, 0},  //TV = 6.77(232 lines)  AV=2.97  SV=4.44  BV=5.30
    {9826,1140,1024, 0, 0, 0},  //TV = 6.67(249 lines)  AV=2.97  SV=4.44  BV=5.20
    {9983,1140,1072, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=4.51  BV=5.11
    {9983,1216,1072, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=4.60  BV=5.02
    {9983,1328,1056, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=4.71  BV=4.91
    {9983,1424,1056, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=4.81  BV=4.81
    {9983,1536,1048, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=4.91  BV=4.71
    {9983,1632,1064, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=5.01  BV=4.60
    {9983,1728,1072, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=5.11  BV=4.51
    {9983,1936,1024, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=5.21  BV=4.41
    {9983,2048,1040, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=5.31  BV=4.31
    {9983,2144,1064, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=5.41  BV=4.21
    {20006,1140,1064, 0, 0, 0},  //TV = 5.64(507 lines)  AV=2.97  SV=4.50  BV=4.12
    {20006,1216,1072, 0, 0, 0},  //TV = 5.64(507 lines)  AV=2.97  SV=4.60  BV=4.01
    {20006,1328,1048, 0, 0, 0},  //TV = 5.64(507 lines)  AV=2.97  SV=4.70  BV=3.92
    {20006,1424,1064, 0, 0, 0},  //TV = 5.64(507 lines)  AV=2.97  SV=4.82  BV=3.80
    {20006,1536,1048, 0, 0, 0},  //TV = 5.64(507 lines)  AV=2.97  SV=4.91  BV=3.71
    {20006,1632,1056, 0, 0, 0},  //TV = 5.64(507 lines)  AV=2.97  SV=5.00  BV=3.61
    {29989,1140,1080, 0, 0, 0},  //TV = 5.06(760 lines)  AV=2.97  SV=4.52  BV=3.51
    {29989,1216,1096, 0, 0, 0},  //TV = 5.06(760 lines)  AV=2.97  SV=4.63  BV=3.40
    {29989,1328,1064, 0, 0, 0},  //TV = 5.06(760 lines)  AV=2.97  SV=4.72  BV=3.31
    {29989,1424,1072, 0, 0, 0},  //TV = 5.06(760 lines)  AV=2.97  SV=4.83  BV=3.20
    {40011,1140,1064, 0, 0, 0},  //TV = 4.64(1014 lines)  AV=2.97  SV=4.50  BV=3.12
    {40011,1216,1072, 0, 0, 0},  //TV = 4.64(1014 lines)  AV=2.97  SV=4.60  BV=3.01
    {40011,1328,1048, 0, 0, 0},  //TV = 4.64(1014 lines)  AV=2.97  SV=4.70  BV=2.92
    {49994,1140,1056, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.49  BV=2.81
    {49994,1216,1064, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.59  BV=2.70
    {49994,1328,1040, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.68  BV=2.61
    {49994,1424,1040, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.78  BV=2.51
    {49994,1536,1032, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.88  BV=2.41
    {49994,1632,1040, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.98  BV=2.31
    {49994,1728,1056, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.09  BV=2.21
    {49994,1840,1064, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.19  BV=2.11
    {49994,2048,1024, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.29  BV=2.01
    {49994,2144,1048, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.39  BV=1.91
    {49994,2240,1072, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.48  BV=1.81
    {49994,2448,1048, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.58  BV=1.72
    {49994,2656,1040, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.68  BV=1.61
    {49994,2864,1032, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.78  BV=1.51
    {49994,3072,1032, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.88  BV=1.41
    {49994,3328,1024, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.99  BV=1.31
    {49994,3456,1056, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=6.09  BV=1.21
    {60017,3072,1064, 0, 0, 0},  //TV = 4.06(1521 lines)  AV=2.97  SV=5.93  BV=1.10
    {60017,3328,1048, 0, 0, 0},  //TV = 4.06(1521 lines)  AV=2.97  SV=6.02  BV=1.01
    {69999,3072,1048, 0, 0, 0},  //TV = 3.84(1774 lines)  AV=2.97  SV=5.91  BV=0.90
    {69999,3328,1032, 0, 0, 0},  //TV = 3.84(1774 lines)  AV=2.97  SV=6.00  BV=0.81
    {79982,3072,1048, 0, 0, 0},  //TV = 3.64(2027 lines)  AV=2.97  SV=5.91  BV=0.71
    {79982,3328,1032, 0, 0, 0},  //TV = 3.64(2027 lines)  AV=2.97  SV=6.00  BV=0.62
    {79982,3584,1032, 0, 0, 0},  //TV = 3.64(2027 lines)  AV=2.97  SV=6.11  BV=0.51
    {79982,3840,1032, 0, 0, 0},  //TV = 3.64(2027 lines)  AV=2.97  SV=6.20  BV=0.41
    {79982,4096,1040, 0, 0, 0},  //TV = 3.64(2027 lines)  AV=2.97  SV=6.31  BV=0.31
    {79982,4352,1040, 0, 0, 0},  //TV = 3.64(2027 lines)  AV=2.97  SV=6.40  BV=0.22
    {90005,4224,1024, 0, 0, 0},  //TV = 3.47(2281 lines)  AV=2.97  SV=6.33  BV=0.11
    {99987,4096,1032, 0, 0, 0},  //TV = 3.32(2534 lines)  AV=2.97  SV=6.30  BV=-0.01
    {99987,4352,1040, 0, 0, 0},  //TV = 3.32(2534 lines)  AV=2.97  SV=6.40  BV=-0.10
    {110010,4224,1040, 0, 0, 0},  //TV = 3.18(2788 lines)  AV=2.97  SV=6.35  BV=-0.20
    {119992,4096,1056, 0, 0, 0},  //TV = 3.06(3041 lines)  AV=2.97  SV=6.33  BV=-0.30
    {119992,4480,1032, 0, 0, 0},  //TV = 3.06(3041 lines)  AV=2.97  SV=6.43  BV=-0.40
    {119992,4736,1048, 0, 0, 0},  //TV = 3.06(3041 lines)  AV=2.97  SV=6.53  BV=-0.50
    {119992,5120,1040, 0, 0, 0},  //TV = 3.06(3041 lines)  AV=2.97  SV=6.63  BV=-0.60
    {119992,5504,1040, 0, 0, 0},  //TV = 3.06(3041 lines)  AV=2.97  SV=6.74  BV=-0.71
    {119992,5888,1040, 0, 0, 0},  //TV = 3.06(3041 lines)  AV=2.97  SV=6.83  BV=-0.80
    {119992,6144,1064, 0, 0, 0},  //TV = 3.06(3041 lines)  AV=2.97  SV=6.93  BV=-0.90
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
},
};

static strAETable g_AE_CaptureISO400Table =
{
    AETABLE_CAPTURE_ISO400,    //eAETableID
    119,    //u4TotalIndex
    20,    //u4StrobeTrigerBV
    109,    //i4MaxBV
    -9,    //i4MinBV
    90,    //i4EffectiveMaxBV
    0,      //i4EffectiveMinBV
    LIB3A_AE_ISO_SPEED_400, //ISO SPEED
    sCaptureISO400PLineTable_60Hz,
    sCaptureISO400PLineTable_50Hz,
    NULL,
};

static strEvPline sCaptureISO800PLineTable_60Hz =
{
{
    {198,1140,1024, 0, 0, 0},  //TV = 12.30(5 lines)  AV=2.97  SV=4.44  BV=10.83
    {198,1140,1040, 0, 0, 0},  //TV = 12.30(5 lines)  AV=2.97  SV=4.46  BV=10.81
    {198,1216,1048, 0, 0, 0},  //TV = 12.30(5 lines)  AV=2.97  SV=4.57  BV=10.70
    {198,1328,1024, 0, 0, 0},  //TV = 12.30(5 lines)  AV=2.97  SV=4.66  BV=10.61
    {237,1140,1064, 0, 0, 0},  //TV = 12.04(6 lines)  AV=2.97  SV=4.50  BV=10.52
    {237,1216,1072, 0, 0, 0},  //TV = 12.04(6 lines)  AV=2.97  SV=4.60  BV=10.41
    {277,1140,1056, 0, 0, 0},  //TV = 11.82(7 lines)  AV=2.97  SV=4.49  BV=10.30
    {277,1216,1056, 0, 0, 0},  //TV = 11.82(7 lines)  AV=2.97  SV=4.58  BV=10.21
    {316,1140,1056, 0, 0, 0},  //TV = 11.63(8 lines)  AV=2.97  SV=4.49  BV=10.11
    {316,1216,1064, 0, 0, 0},  //TV = 11.63(8 lines)  AV=2.97  SV=4.59  BV=10.01
    {356,1140,1080, 0, 0, 0},  //TV = 11.46(9 lines)  AV=2.97  SV=4.52  BV=9.91
    {395,1140,1040, 0, 0, 0},  //TV = 11.31(10 lines)  AV=2.97  SV=4.46  BV=9.81
    {435,1140,1024, 0, 0, 0},  //TV = 11.17(11 lines)  AV=2.97  SV=4.44  BV=9.70
    {435,1140,1088, 0, 0, 0},  //TV = 11.17(11 lines)  AV=2.97  SV=4.53  BV=9.61
    {474,1140,1064, 0, 0, 0},  //TV = 11.04(12 lines)  AV=2.97  SV=4.50  BV=9.52
    {513,1140,1056, 0, 0, 0},  //TV = 10.93(13 lines)  AV=2.97  SV=4.49  BV=9.41
    {553,1140,1056, 0, 0, 0},  //TV = 10.82(14 lines)  AV=2.97  SV=4.49  BV=9.31
    {592,1140,1056, 0, 0, 0},  //TV = 10.72(15 lines)  AV=2.97  SV=4.49  BV=9.21
    {632,1140,1056, 0, 0, 0},  //TV = 10.63(16 lines)  AV=2.97  SV=4.49  BV=9.11
    {671,1140,1064, 0, 0, 0},  //TV = 10.54(17 lines)  AV=2.97  SV=4.50  BV=9.02
    {750,1140,1024, 0, 0, 0},  //TV = 10.38(19 lines)  AV=2.97  SV=4.44  BV=8.91
    {790,1140,1040, 0, 0, 0},  //TV = 10.31(20 lines)  AV=2.97  SV=4.46  BV=8.81
    {869,1140,1024, 0, 0, 0},  //TV = 10.17(22 lines)  AV=2.97  SV=4.44  BV=8.70
    {908,1140,1040, 0, 0, 0},  //TV = 10.11(23 lines)  AV=2.97  SV=4.46  BV=8.61
    {987,1140,1024, 0, 0, 0},  //TV = 9.98(25 lines)  AV=2.97  SV=4.44  BV=8.51
    {1066,1140,1024, 0, 0, 0},  //TV = 9.87(27 lines)  AV=2.97  SV=4.44  BV=8.40
    {1105,1140,1056, 0, 0, 0},  //TV = 9.82(28 lines)  AV=2.97  SV=4.49  BV=8.31
    {1224,1140,1024, 0, 0, 0},  //TV = 9.67(31 lines)  AV=2.97  SV=4.44  BV=8.20
    {1303,1140,1024, 0, 0, 0},  //TV = 9.58(33 lines)  AV=2.97  SV=4.44  BV=8.11
    {1382,1140,1040, 0, 0, 0},  //TV = 9.50(35 lines)  AV=2.97  SV=4.46  BV=8.01
    {1500,1140,1024, 0, 0, 0},  //TV = 9.38(38 lines)  AV=2.97  SV=4.44  BV=7.91
    {1618,1140,1024, 0, 0, 0},  //TV = 9.27(41 lines)  AV=2.97  SV=4.44  BV=7.80
    {1697,1140,1040, 0, 0, 0},  //TV = 9.20(43 lines)  AV=2.97  SV=4.46  BV=7.71
    {1855,1140,1024, 0, 0, 0},  //TV = 9.07(47 lines)  AV=2.97  SV=4.44  BV=7.60
    {1973,1140,1024, 0, 0, 0},  //TV = 8.99(50 lines)  AV=2.97  SV=4.44  BV=7.51
    {2131,1140,1024, 0, 0, 0},  //TV = 8.87(54 lines)  AV=2.97  SV=4.44  BV=7.40
    {2250,1140,1032, 0, 0, 0},  //TV = 8.80(57 lines)  AV=2.97  SV=4.45  BV=7.31
    {2447,1140,1024, 0, 0, 0},  //TV = 8.67(62 lines)  AV=2.97  SV=4.44  BV=7.20
    {2605,1140,1024, 0, 0, 0},  //TV = 8.58(66 lines)  AV=2.97  SV=4.44  BV=7.11
    {2802,1140,1024, 0, 0, 0},  //TV = 8.48(71 lines)  AV=2.97  SV=4.44  BV=7.01
    {2999,1140,1024, 0, 0, 0},  //TV = 8.38(76 lines)  AV=2.97  SV=4.44  BV=6.91
    {3236,1140,1024, 0, 0, 0},  //TV = 8.27(82 lines)  AV=2.97  SV=4.44  BV=6.80
    {3473,1140,1024, 0, 0, 0},  //TV = 8.17(88 lines)  AV=2.97  SV=4.44  BV=6.70
    {3710,1140,1024, 0, 0, 0},  //TV = 8.07(94 lines)  AV=2.97  SV=4.44  BV=6.60
    {3986,1140,1024, 0, 0, 0},  //TV = 7.97(101 lines)  AV=2.97  SV=4.44  BV=6.50
    {4262,1140,1024, 0, 0, 0},  //TV = 7.87(108 lines)  AV=2.97  SV=4.44  BV=6.40
    {4538,1140,1024, 0, 0, 0},  //TV = 7.78(115 lines)  AV=2.97  SV=4.44  BV=6.31
    {4893,1140,1024, 0, 0, 0},  //TV = 7.68(124 lines)  AV=2.97  SV=4.44  BV=6.20
    {5209,1140,1024, 0, 0, 0},  //TV = 7.58(132 lines)  AV=2.97  SV=4.44  BV=6.11
    {5643,1140,1024, 0, 0, 0},  //TV = 7.47(143 lines)  AV=2.97  SV=4.44  BV=6.00
    {6038,1140,1024, 0, 0, 0},  //TV = 7.37(153 lines)  AV=2.97  SV=4.44  BV=5.90
    {6472,1140,1024, 0, 0, 0},  //TV = 7.27(164 lines)  AV=2.97  SV=4.44  BV=5.80
    {6906,1140,1024, 0, 0, 0},  //TV = 7.18(175 lines)  AV=2.97  SV=4.44  BV=5.71
    {7419,1140,1024, 0, 0, 0},  //TV = 7.07(188 lines)  AV=2.97  SV=4.44  BV=5.60
    {7971,1140,1024, 0, 0, 0},  //TV = 6.97(202 lines)  AV=2.97  SV=4.44  BV=5.50
    {8326,1140,1048, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=4.48  BV=5.40
    {8326,1216,1048, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=4.57  BV=5.31
    {8326,1328,1024, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=4.66  BV=5.22
    {8326,1424,1024, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=4.76  BV=5.12
    {8326,1536,1024, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=4.87  BV=5.01
    {8326,1632,1032, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=4.97  BV=4.91
    {8326,1728,1040, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=5.06  BV=4.81
    {8326,1840,1048, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=5.17  BV=4.71
    {8326,1936,1072, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=5.27  BV=4.61
    {8326,2144,1032, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=5.36  BV=4.51
    {16652,1140,1048, 0, 0, 0},  //TV = 5.91(422 lines)  AV=2.97  SV=4.48  BV=4.40
    {16652,1216,1048, 0, 0, 0},  //TV = 5.91(422 lines)  AV=2.97  SV=4.57  BV=4.31
    {16652,1328,1024, 0, 0, 0},  //TV = 5.91(422 lines)  AV=2.97  SV=4.66  BV=4.22
    {16652,1424,1024, 0, 0, 0},  //TV = 5.91(422 lines)  AV=2.97  SV=4.76  BV=4.12
    {16652,1424,1104, 0, 0, 0},  //TV = 5.91(422 lines)  AV=2.97  SV=4.87  BV=4.01
    {16652,1632,1032, 0, 0, 0},  //TV = 5.91(422 lines)  AV=2.97  SV=4.97  BV=3.91
    {25017,1140,1056, 0, 0, 0},  //TV = 5.32(634 lines)  AV=2.97  SV=4.49  BV=3.81
    {25017,1216,1064, 0, 0, 0},  //TV = 5.32(634 lines)  AV=2.97  SV=4.59  BV=3.70
    {25017,1328,1048, 0, 0, 0},  //TV = 5.32(634 lines)  AV=2.97  SV=4.70  BV=3.60
    {25017,1424,1040, 0, 0, 0},  //TV = 5.32(634 lines)  AV=2.97  SV=4.78  BV=3.51
    {33343,1140,1048, 0, 0, 0},  //TV = 4.91(845 lines)  AV=2.97  SV=4.48  BV=3.40
    {33343,1216,1048, 0, 0, 0},  //TV = 4.91(845 lines)  AV=2.97  SV=4.57  BV=3.31
    {33343,1328,1024, 0, 0, 0},  //TV = 4.91(845 lines)  AV=2.97  SV=4.66  BV=3.22
    {41669,1140,1024, 0, 0, 0},  //TV = 4.58(1056 lines)  AV=2.97  SV=4.44  BV=3.11
    {41669,1216,1032, 0, 0, 0},  //TV = 4.58(1056 lines)  AV=2.97  SV=4.55  BV=3.01
    {41669,1216,1104, 0, 0, 0},  //TV = 4.58(1056 lines)  AV=2.97  SV=4.64  BV=2.91
    {49994,1140,1056, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.49  BV=2.81
    {49994,1216,1064, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.59  BV=2.70
    {49994,1328,1040, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.68  BV=2.61
    {49994,1424,1040, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.78  BV=2.51
    {49994,1536,1032, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.88  BV=2.41
    {49994,1632,1040, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.98  BV=2.31
    {49994,1728,1056, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.09  BV=2.21
    {49994,1840,1064, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.19  BV=2.11
    {49994,2048,1024, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.29  BV=2.01
    {49994,2144,1048, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.39  BV=1.91
    {49994,2240,1072, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.48  BV=1.81
    {49994,2448,1048, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.58  BV=1.72
    {49994,2656,1040, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.68  BV=1.61
    {49994,2864,1032, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.78  BV=1.51
    {49994,3072,1032, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.88  BV=1.41
    {49994,3328,1024, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.99  BV=1.31
    {49994,3456,1056, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=6.09  BV=1.21
    {58319,3200,1048, 0, 0, 0},  //TV = 4.10(1478 lines)  AV=2.97  SV=5.96  BV=1.11
    {66645,3072,1024, 0, 0, 0},  //TV = 3.91(1689 lines)  AV=2.97  SV=5.87  BV=1.01
    {66645,3200,1056, 0, 0, 0},  //TV = 3.91(1689 lines)  AV=2.97  SV=5.98  BV=0.90
    {75011,3072,1048, 0, 0, 0},  //TV = 3.74(1901 lines)  AV=2.97  SV=5.91  BV=0.80
    {75011,3328,1032, 0, 0, 0},  //TV = 3.74(1901 lines)  AV=2.97  SV=6.00  BV=0.71
    {83336,3200,1032, 0, 0, 0},  //TV = 3.58(2112 lines)  AV=2.97  SV=5.94  BV=0.61
    {83336,3456,1024, 0, 0, 0},  //TV = 3.58(2112 lines)  AV=2.97  SV=6.04  BV=0.51
    {83336,3712,1024, 0, 0, 0},  //TV = 3.58(2112 lines)  AV=2.97  SV=6.14  BV=0.41
    {83336,3968,1032, 0, 0, 0},  //TV = 3.58(2112 lines)  AV=2.97  SV=6.25  BV=0.30
    {83336,4224,1032, 0, 0, 0},  //TV = 3.58(2112 lines)  AV=2.97  SV=6.34  BV=0.21
    {91661,4096,1040, 0, 0, 0},  //TV = 3.45(2323 lines)  AV=2.97  SV=6.31  BV=0.11
    {99987,4096,1032, 0, 0, 0},  //TV = 3.32(2534 lines)  AV=2.97  SV=6.30  BV=-0.01
    {108313,4096,1024, 0, 0, 0},  //TV = 3.21(2745 lines)  AV=2.97  SV=6.29  BV=-0.11
    {108313,4352,1032, 0, 0, 0},  //TV = 3.21(2745 lines)  AV=2.97  SV=6.39  BV=-0.21
    {116678,4224,1048, 0, 0, 0},  //TV = 3.10(2957 lines)  AV=2.97  SV=6.36  BV=-0.29
    {125004,4224,1048, 0, 0, 0},  //TV = 3.00(3168 lines)  AV=2.97  SV=6.36  BV=-0.39
    {125004,4608,1032, 0, 0, 0},  //TV = 3.00(3168 lines)  AV=2.97  SV=6.47  BV=-0.50
    {125004,4864,1048, 0, 0, 0},  //TV = 3.00(3168 lines)  AV=2.97  SV=6.57  BV=-0.60
    {125004,5376,1024, 0, 0, 0},  //TV = 3.00(3168 lines)  AV=2.97  SV=6.68  BV=-0.71
    {125004,5632,1040, 0, 0, 0},  //TV = 3.00(3168 lines)  AV=2.97  SV=6.77  BV=-0.80
    {125004,6016,1040, 0, 0, 0},  //TV = 3.00(3168 lines)  AV=2.97  SV=6.86  BV=-0.89
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
},
};

static strEvPline sCaptureISO800PLineTable_50Hz =
{
{
    {198,1140,1024, 0, 0, 0},  //TV = 12.30(5 lines)  AV=2.97  SV=4.44  BV=10.83
    {198,1140,1040, 0, 0, 0},  //TV = 12.30(5 lines)  AV=2.97  SV=4.46  BV=10.81
    {198,1216,1048, 0, 0, 0},  //TV = 12.30(5 lines)  AV=2.97  SV=4.57  BV=10.70
    {198,1328,1024, 0, 0, 0},  //TV = 12.30(5 lines)  AV=2.97  SV=4.66  BV=10.61
    {237,1140,1064, 0, 0, 0},  //TV = 12.04(6 lines)  AV=2.97  SV=4.50  BV=10.52
    {237,1216,1072, 0, 0, 0},  //TV = 12.04(6 lines)  AV=2.97  SV=4.60  BV=10.41
    {277,1140,1056, 0, 0, 0},  //TV = 11.82(7 lines)  AV=2.97  SV=4.49  BV=10.30
    {277,1216,1056, 0, 0, 0},  //TV = 11.82(7 lines)  AV=2.97  SV=4.58  BV=10.21
    {316,1140,1056, 0, 0, 0},  //TV = 11.63(8 lines)  AV=2.97  SV=4.49  BV=10.11
    {316,1216,1064, 0, 0, 0},  //TV = 11.63(8 lines)  AV=2.97  SV=4.59  BV=10.01
    {356,1140,1080, 0, 0, 0},  //TV = 11.46(9 lines)  AV=2.97  SV=4.52  BV=9.91
    {395,1140,1040, 0, 0, 0},  //TV = 11.31(10 lines)  AV=2.97  SV=4.46  BV=9.81
    {435,1140,1024, 0, 0, 0},  //TV = 11.17(11 lines)  AV=2.97  SV=4.44  BV=9.70
    {435,1140,1088, 0, 0, 0},  //TV = 11.17(11 lines)  AV=2.97  SV=4.53  BV=9.61
    {474,1140,1064, 0, 0, 0},  //TV = 11.04(12 lines)  AV=2.97  SV=4.50  BV=9.52
    {513,1140,1056, 0, 0, 0},  //TV = 10.93(13 lines)  AV=2.97  SV=4.49  BV=9.41
    {553,1140,1056, 0, 0, 0},  //TV = 10.82(14 lines)  AV=2.97  SV=4.49  BV=9.31
    {592,1140,1056, 0, 0, 0},  //TV = 10.72(15 lines)  AV=2.97  SV=4.49  BV=9.21
    {632,1140,1056, 0, 0, 0},  //TV = 10.63(16 lines)  AV=2.97  SV=4.49  BV=9.11
    {671,1140,1064, 0, 0, 0},  //TV = 10.54(17 lines)  AV=2.97  SV=4.50  BV=9.02
    {750,1140,1024, 0, 0, 0},  //TV = 10.38(19 lines)  AV=2.97  SV=4.44  BV=8.91
    {790,1140,1040, 0, 0, 0},  //TV = 10.31(20 lines)  AV=2.97  SV=4.46  BV=8.81
    {869,1140,1024, 0, 0, 0},  //TV = 10.17(22 lines)  AV=2.97  SV=4.44  BV=8.70
    {908,1140,1040, 0, 0, 0},  //TV = 10.11(23 lines)  AV=2.97  SV=4.46  BV=8.61
    {987,1140,1024, 0, 0, 0},  //TV = 9.98(25 lines)  AV=2.97  SV=4.44  BV=8.51
    {1066,1140,1024, 0, 0, 0},  //TV = 9.87(27 lines)  AV=2.97  SV=4.44  BV=8.40
    {1105,1140,1056, 0, 0, 0},  //TV = 9.82(28 lines)  AV=2.97  SV=4.49  BV=8.31
    {1224,1140,1024, 0, 0, 0},  //TV = 9.67(31 lines)  AV=2.97  SV=4.44  BV=8.20
    {1303,1140,1024, 0, 0, 0},  //TV = 9.58(33 lines)  AV=2.97  SV=4.44  BV=8.11
    {1382,1140,1040, 0, 0, 0},  //TV = 9.50(35 lines)  AV=2.97  SV=4.46  BV=8.01
    {1500,1140,1024, 0, 0, 0},  //TV = 9.38(38 lines)  AV=2.97  SV=4.44  BV=7.91
    {1618,1140,1024, 0, 0, 0},  //TV = 9.27(41 lines)  AV=2.97  SV=4.44  BV=7.80
    {1697,1140,1040, 0, 0, 0},  //TV = 9.20(43 lines)  AV=2.97  SV=4.46  BV=7.71
    {1855,1140,1024, 0, 0, 0},  //TV = 9.07(47 lines)  AV=2.97  SV=4.44  BV=7.60
    {1973,1140,1024, 0, 0, 0},  //TV = 8.99(50 lines)  AV=2.97  SV=4.44  BV=7.51
    {2131,1140,1024, 0, 0, 0},  //TV = 8.87(54 lines)  AV=2.97  SV=4.44  BV=7.40
    {2250,1140,1032, 0, 0, 0},  //TV = 8.80(57 lines)  AV=2.97  SV=4.45  BV=7.31
    {2447,1140,1024, 0, 0, 0},  //TV = 8.67(62 lines)  AV=2.97  SV=4.44  BV=7.20
    {2605,1140,1024, 0, 0, 0},  //TV = 8.58(66 lines)  AV=2.97  SV=4.44  BV=7.11
    {2802,1140,1024, 0, 0, 0},  //TV = 8.48(71 lines)  AV=2.97  SV=4.44  BV=7.01
    {2999,1140,1024, 0, 0, 0},  //TV = 8.38(76 lines)  AV=2.97  SV=4.44  BV=6.91
    {3236,1140,1024, 0, 0, 0},  //TV = 8.27(82 lines)  AV=2.97  SV=4.44  BV=6.80
    {3473,1140,1024, 0, 0, 0},  //TV = 8.17(88 lines)  AV=2.97  SV=4.44  BV=6.70
    {3710,1140,1024, 0, 0, 0},  //TV = 8.07(94 lines)  AV=2.97  SV=4.44  BV=6.60
    {3986,1140,1024, 0, 0, 0},  //TV = 7.97(101 lines)  AV=2.97  SV=4.44  BV=6.50
    {4262,1140,1024, 0, 0, 0},  //TV = 7.87(108 lines)  AV=2.97  SV=4.44  BV=6.40
    {4538,1140,1024, 0, 0, 0},  //TV = 7.78(115 lines)  AV=2.97  SV=4.44  BV=6.31
    {4893,1140,1024, 0, 0, 0},  //TV = 7.68(124 lines)  AV=2.97  SV=4.44  BV=6.20
    {5209,1140,1024, 0, 0, 0},  //TV = 7.58(132 lines)  AV=2.97  SV=4.44  BV=6.11
    {5643,1140,1024, 0, 0, 0},  //TV = 7.47(143 lines)  AV=2.97  SV=4.44  BV=6.00
    {6038,1140,1024, 0, 0, 0},  //TV = 7.37(153 lines)  AV=2.97  SV=4.44  BV=5.90
    {6472,1140,1024, 0, 0, 0},  //TV = 7.27(164 lines)  AV=2.97  SV=4.44  BV=5.80
    {6906,1140,1024, 0, 0, 0},  //TV = 7.18(175 lines)  AV=2.97  SV=4.44  BV=5.71
    {7419,1140,1024, 0, 0, 0},  //TV = 7.07(188 lines)  AV=2.97  SV=4.44  BV=5.60
    {7971,1140,1024, 0, 0, 0},  //TV = 6.97(202 lines)  AV=2.97  SV=4.44  BV=5.50
    {8484,1140,1024, 0, 0, 0},  //TV = 6.88(215 lines)  AV=2.97  SV=4.44  BV=5.41
    {9155,1140,1024, 0, 0, 0},  //TV = 6.77(232 lines)  AV=2.97  SV=4.44  BV=5.30
    {9826,1140,1024, 0, 0, 0},  //TV = 6.67(249 lines)  AV=2.97  SV=4.44  BV=5.20
    {9983,1140,1072, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=4.51  BV=5.11
    {9983,1216,1072, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=4.60  BV=5.02
    {9983,1328,1056, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=4.71  BV=4.91
    {9983,1424,1056, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=4.81  BV=4.81
    {9983,1536,1048, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=4.91  BV=4.71
    {9983,1632,1064, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=5.01  BV=4.60
    {9983,1728,1072, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=5.11  BV=4.51
    {9983,1936,1024, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=5.21  BV=4.41
    {9983,2048,1040, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=5.31  BV=4.31
    {9983,2144,1064, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=5.41  BV=4.21
    {20006,1140,1064, 0, 0, 0},  //TV = 5.64(507 lines)  AV=2.97  SV=4.50  BV=4.12
    {20006,1216,1072, 0, 0, 0},  //TV = 5.64(507 lines)  AV=2.97  SV=4.60  BV=4.01
    {20006,1328,1048, 0, 0, 0},  //TV = 5.64(507 lines)  AV=2.97  SV=4.70  BV=3.92
    {20006,1424,1064, 0, 0, 0},  //TV = 5.64(507 lines)  AV=2.97  SV=4.82  BV=3.80
    {20006,1536,1048, 0, 0, 0},  //TV = 5.64(507 lines)  AV=2.97  SV=4.91  BV=3.71
    {20006,1632,1056, 0, 0, 0},  //TV = 5.64(507 lines)  AV=2.97  SV=5.00  BV=3.61
    {29989,1140,1080, 0, 0, 0},  //TV = 5.06(760 lines)  AV=2.97  SV=4.52  BV=3.51
    {29989,1216,1096, 0, 0, 0},  //TV = 5.06(760 lines)  AV=2.97  SV=4.63  BV=3.40
    {29989,1328,1064, 0, 0, 0},  //TV = 5.06(760 lines)  AV=2.97  SV=4.72  BV=3.31
    {29989,1424,1072, 0, 0, 0},  //TV = 5.06(760 lines)  AV=2.97  SV=4.83  BV=3.20
    {40011,1140,1064, 0, 0, 0},  //TV = 4.64(1014 lines)  AV=2.97  SV=4.50  BV=3.12
    {40011,1216,1072, 0, 0, 0},  //TV = 4.64(1014 lines)  AV=2.97  SV=4.60  BV=3.01
    {40011,1328,1048, 0, 0, 0},  //TV = 4.64(1014 lines)  AV=2.97  SV=4.70  BV=2.92
    {49994,1140,1056, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.49  BV=2.81
    {49994,1216,1064, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.59  BV=2.70
    {49994,1328,1040, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.68  BV=2.61
    {49994,1424,1040, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.78  BV=2.51
    {49994,1536,1032, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.88  BV=2.41
    {49994,1632,1040, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.98  BV=2.31
    {49994,1728,1056, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.09  BV=2.21
    {49994,1840,1064, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.19  BV=2.11
    {49994,2048,1024, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.29  BV=2.01
    {49994,2144,1048, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.39  BV=1.91
    {49994,2240,1072, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.48  BV=1.81
    {49994,2448,1048, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.58  BV=1.72
    {49994,2656,1040, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.68  BV=1.61
    {49994,2864,1032, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.78  BV=1.51
    {49994,3072,1032, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.88  BV=1.41
    {49994,3328,1024, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.99  BV=1.31
    {49994,3456,1056, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=6.09  BV=1.21
    {60017,3072,1064, 0, 0, 0},  //TV = 4.06(1521 lines)  AV=2.97  SV=5.93  BV=1.10
    {60017,3328,1048, 0, 0, 0},  //TV = 4.06(1521 lines)  AV=2.97  SV=6.02  BV=1.01
    {69999,3072,1048, 0, 0, 0},  //TV = 3.84(1774 lines)  AV=2.97  SV=5.91  BV=0.90
    {69999,3328,1032, 0, 0, 0},  //TV = 3.84(1774 lines)  AV=2.97  SV=6.00  BV=0.81
    {79982,3072,1048, 0, 0, 0},  //TV = 3.64(2027 lines)  AV=2.97  SV=5.91  BV=0.71
    {79982,3328,1032, 0, 0, 0},  //TV = 3.64(2027 lines)  AV=2.97  SV=6.00  BV=0.62
    {79982,3584,1032, 0, 0, 0},  //TV = 3.64(2027 lines)  AV=2.97  SV=6.11  BV=0.51
    {79982,3840,1032, 0, 0, 0},  //TV = 3.64(2027 lines)  AV=2.97  SV=6.20  BV=0.41
    {79982,4096,1040, 0, 0, 0},  //TV = 3.64(2027 lines)  AV=2.97  SV=6.31  BV=0.31
    {79982,4352,1040, 0, 0, 0},  //TV = 3.64(2027 lines)  AV=2.97  SV=6.40  BV=0.22
    {90005,4224,1024, 0, 0, 0},  //TV = 3.47(2281 lines)  AV=2.97  SV=6.33  BV=0.11
    {99987,4096,1032, 0, 0, 0},  //TV = 3.32(2534 lines)  AV=2.97  SV=6.30  BV=-0.01
    {99987,4352,1040, 0, 0, 0},  //TV = 3.32(2534 lines)  AV=2.97  SV=6.40  BV=-0.10
    {110010,4224,1040, 0, 0, 0},  //TV = 3.18(2788 lines)  AV=2.97  SV=6.35  BV=-0.20
    {119992,4096,1056, 0, 0, 0},  //TV = 3.06(3041 lines)  AV=2.97  SV=6.33  BV=-0.30
    {119992,4480,1032, 0, 0, 0},  //TV = 3.06(3041 lines)  AV=2.97  SV=6.43  BV=-0.40
    {119992,4736,1048, 0, 0, 0},  //TV = 3.06(3041 lines)  AV=2.97  SV=6.53  BV=-0.50
    {119992,5120,1040, 0, 0, 0},  //TV = 3.06(3041 lines)  AV=2.97  SV=6.63  BV=-0.60
    {119992,5504,1040, 0, 0, 0},  //TV = 3.06(3041 lines)  AV=2.97  SV=6.74  BV=-0.71
    {119992,5888,1040, 0, 0, 0},  //TV = 3.06(3041 lines)  AV=2.97  SV=6.83  BV=-0.80
    {119992,6144,1064, 0, 0, 0},  //TV = 3.06(3041 lines)  AV=2.97  SV=6.93  BV=-0.90
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
},
};

static strAETable g_AE_CaptureISO800Table =
{
    AETABLE_CAPTURE_ISO800,    //eAETableID
    119,    //u4TotalIndex
    20,    //u4StrobeTrigerBV
    109,    //i4MaxBV
    -9,    //i4MinBV
    90,    //i4EffectiveMaxBV
    0,      //i4EffectiveMinBV
    LIB3A_AE_ISO_SPEED_800, //ISO SPEED
    sCaptureISO800PLineTable_60Hz,
    sCaptureISO800PLineTable_50Hz,
    NULL,
};

static strEvPline sCaptureISO1600PLineTable_60Hz =
{
{
    {198,1140,1024, 0, 0, 0},  //TV = 12.30(5 lines)  AV=2.97  SV=4.44  BV=10.83
    {198,1140,1040, 0, 0, 0},  //TV = 12.30(5 lines)  AV=2.97  SV=4.46  BV=10.81
    {198,1216,1048, 0, 0, 0},  //TV = 12.30(5 lines)  AV=2.97  SV=4.57  BV=10.70
    {198,1328,1024, 0, 0, 0},  //TV = 12.30(5 lines)  AV=2.97  SV=4.66  BV=10.61
    {237,1140,1064, 0, 0, 0},  //TV = 12.04(6 lines)  AV=2.97  SV=4.50  BV=10.52
    {237,1216,1072, 0, 0, 0},  //TV = 12.04(6 lines)  AV=2.97  SV=4.60  BV=10.41
    {277,1140,1056, 0, 0, 0},  //TV = 11.82(7 lines)  AV=2.97  SV=4.49  BV=10.30
    {277,1216,1056, 0, 0, 0},  //TV = 11.82(7 lines)  AV=2.97  SV=4.58  BV=10.21
    {316,1140,1056, 0, 0, 0},  //TV = 11.63(8 lines)  AV=2.97  SV=4.49  BV=10.11
    {316,1216,1064, 0, 0, 0},  //TV = 11.63(8 lines)  AV=2.97  SV=4.59  BV=10.01
    {356,1140,1080, 0, 0, 0},  //TV = 11.46(9 lines)  AV=2.97  SV=4.52  BV=9.91
    {395,1140,1040, 0, 0, 0},  //TV = 11.31(10 lines)  AV=2.97  SV=4.46  BV=9.81
    {435,1140,1024, 0, 0, 0},  //TV = 11.17(11 lines)  AV=2.97  SV=4.44  BV=9.70
    {435,1140,1088, 0, 0, 0},  //TV = 11.17(11 lines)  AV=2.97  SV=4.53  BV=9.61
    {474,1140,1064, 0, 0, 0},  //TV = 11.04(12 lines)  AV=2.97  SV=4.50  BV=9.52
    {513,1140,1056, 0, 0, 0},  //TV = 10.93(13 lines)  AV=2.97  SV=4.49  BV=9.41
    {553,1140,1056, 0, 0, 0},  //TV = 10.82(14 lines)  AV=2.97  SV=4.49  BV=9.31
    {592,1140,1056, 0, 0, 0},  //TV = 10.72(15 lines)  AV=2.97  SV=4.49  BV=9.21
    {632,1140,1056, 0, 0, 0},  //TV = 10.63(16 lines)  AV=2.97  SV=4.49  BV=9.11
    {671,1140,1064, 0, 0, 0},  //TV = 10.54(17 lines)  AV=2.97  SV=4.50  BV=9.02
    {750,1140,1024, 0, 0, 0},  //TV = 10.38(19 lines)  AV=2.97  SV=4.44  BV=8.91
    {790,1140,1040, 0, 0, 0},  //TV = 10.31(20 lines)  AV=2.97  SV=4.46  BV=8.81
    {869,1140,1024, 0, 0, 0},  //TV = 10.17(22 lines)  AV=2.97  SV=4.44  BV=8.70
    {908,1140,1040, 0, 0, 0},  //TV = 10.11(23 lines)  AV=2.97  SV=4.46  BV=8.61
    {987,1140,1024, 0, 0, 0},  //TV = 9.98(25 lines)  AV=2.97  SV=4.44  BV=8.51
    {1066,1140,1024, 0, 0, 0},  //TV = 9.87(27 lines)  AV=2.97  SV=4.44  BV=8.40
    {1105,1140,1056, 0, 0, 0},  //TV = 9.82(28 lines)  AV=2.97  SV=4.49  BV=8.31
    {1224,1140,1024, 0, 0, 0},  //TV = 9.67(31 lines)  AV=2.97  SV=4.44  BV=8.20
    {1303,1140,1024, 0, 0, 0},  //TV = 9.58(33 lines)  AV=2.97  SV=4.44  BV=8.11
    {1382,1140,1040, 0, 0, 0},  //TV = 9.50(35 lines)  AV=2.97  SV=4.46  BV=8.01
    {1500,1140,1024, 0, 0, 0},  //TV = 9.38(38 lines)  AV=2.97  SV=4.44  BV=7.91
    {1618,1140,1024, 0, 0, 0},  //TV = 9.27(41 lines)  AV=2.97  SV=4.44  BV=7.80
    {1697,1140,1040, 0, 0, 0},  //TV = 9.20(43 lines)  AV=2.97  SV=4.46  BV=7.71
    {1855,1140,1024, 0, 0, 0},  //TV = 9.07(47 lines)  AV=2.97  SV=4.44  BV=7.60
    {1973,1140,1024, 0, 0, 0},  //TV = 8.99(50 lines)  AV=2.97  SV=4.44  BV=7.51
    {2131,1140,1024, 0, 0, 0},  //TV = 8.87(54 lines)  AV=2.97  SV=4.44  BV=7.40
    {2250,1140,1032, 0, 0, 0},  //TV = 8.80(57 lines)  AV=2.97  SV=4.45  BV=7.31
    {2447,1140,1024, 0, 0, 0},  //TV = 8.67(62 lines)  AV=2.97  SV=4.44  BV=7.20
    {2605,1140,1024, 0, 0, 0},  //TV = 8.58(66 lines)  AV=2.97  SV=4.44  BV=7.11
    {2802,1140,1024, 0, 0, 0},  //TV = 8.48(71 lines)  AV=2.97  SV=4.44  BV=7.01
    {2999,1140,1024, 0, 0, 0},  //TV = 8.38(76 lines)  AV=2.97  SV=4.44  BV=6.91
    {3236,1140,1024, 0, 0, 0},  //TV = 8.27(82 lines)  AV=2.97  SV=4.44  BV=6.80
    {3473,1140,1024, 0, 0, 0},  //TV = 8.17(88 lines)  AV=2.97  SV=4.44  BV=6.70
    {3710,1140,1024, 0, 0, 0},  //TV = 8.07(94 lines)  AV=2.97  SV=4.44  BV=6.60
    {3986,1140,1024, 0, 0, 0},  //TV = 7.97(101 lines)  AV=2.97  SV=4.44  BV=6.50
    {4262,1140,1024, 0, 0, 0},  //TV = 7.87(108 lines)  AV=2.97  SV=4.44  BV=6.40
    {4538,1140,1024, 0, 0, 0},  //TV = 7.78(115 lines)  AV=2.97  SV=4.44  BV=6.31
    {4893,1140,1024, 0, 0, 0},  //TV = 7.68(124 lines)  AV=2.97  SV=4.44  BV=6.20
    {5209,1140,1024, 0, 0, 0},  //TV = 7.58(132 lines)  AV=2.97  SV=4.44  BV=6.11
    {5643,1140,1024, 0, 0, 0},  //TV = 7.47(143 lines)  AV=2.97  SV=4.44  BV=6.00
    {6038,1140,1024, 0, 0, 0},  //TV = 7.37(153 lines)  AV=2.97  SV=4.44  BV=5.90
    {6472,1140,1024, 0, 0, 0},  //TV = 7.27(164 lines)  AV=2.97  SV=4.44  BV=5.80
    {6906,1140,1024, 0, 0, 0},  //TV = 7.18(175 lines)  AV=2.97  SV=4.44  BV=5.71
    {7419,1140,1024, 0, 0, 0},  //TV = 7.07(188 lines)  AV=2.97  SV=4.44  BV=5.60
    {7971,1140,1024, 0, 0, 0},  //TV = 6.97(202 lines)  AV=2.97  SV=4.44  BV=5.50
    {8326,1140,1048, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=4.48  BV=5.40
    {8326,1216,1048, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=4.57  BV=5.31
    {8326,1328,1024, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=4.66  BV=5.22
    {8326,1424,1024, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=4.76  BV=5.12
    {8326,1536,1024, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=4.87  BV=5.01
    {8326,1632,1032, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=4.97  BV=4.91
    {8326,1728,1040, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=5.06  BV=4.81
    {8326,1840,1048, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=5.17  BV=4.71
    {8326,1936,1072, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=5.27  BV=4.61
    {8326,2144,1032, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=5.36  BV=4.51
    {16652,1140,1048, 0, 0, 0},  //TV = 5.91(422 lines)  AV=2.97  SV=4.48  BV=4.40
    {16652,1216,1048, 0, 0, 0},  //TV = 5.91(422 lines)  AV=2.97  SV=4.57  BV=4.31
    {16652,1328,1024, 0, 0, 0},  //TV = 5.91(422 lines)  AV=2.97  SV=4.66  BV=4.22
    {16652,1424,1024, 0, 0, 0},  //TV = 5.91(422 lines)  AV=2.97  SV=4.76  BV=4.12
    {16652,1424,1104, 0, 0, 0},  //TV = 5.91(422 lines)  AV=2.97  SV=4.87  BV=4.01
    {16652,1632,1032, 0, 0, 0},  //TV = 5.91(422 lines)  AV=2.97  SV=4.97  BV=3.91
    {25017,1140,1056, 0, 0, 0},  //TV = 5.32(634 lines)  AV=2.97  SV=4.49  BV=3.81
    {25017,1216,1064, 0, 0, 0},  //TV = 5.32(634 lines)  AV=2.97  SV=4.59  BV=3.70
    {25017,1328,1048, 0, 0, 0},  //TV = 5.32(634 lines)  AV=2.97  SV=4.70  BV=3.60
    {25017,1424,1040, 0, 0, 0},  //TV = 5.32(634 lines)  AV=2.97  SV=4.78  BV=3.51
    {33343,1140,1048, 0, 0, 0},  //TV = 4.91(845 lines)  AV=2.97  SV=4.48  BV=3.40
    {33343,1216,1048, 0, 0, 0},  //TV = 4.91(845 lines)  AV=2.97  SV=4.57  BV=3.31
    {33343,1328,1024, 0, 0, 0},  //TV = 4.91(845 lines)  AV=2.97  SV=4.66  BV=3.22
    {41669,1140,1024, 0, 0, 0},  //TV = 4.58(1056 lines)  AV=2.97  SV=4.44  BV=3.11
    {41669,1216,1032, 0, 0, 0},  //TV = 4.58(1056 lines)  AV=2.97  SV=4.55  BV=3.01
    {41669,1216,1104, 0, 0, 0},  //TV = 4.58(1056 lines)  AV=2.97  SV=4.64  BV=2.91
    {49994,1140,1056, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.49  BV=2.81
    {49994,1216,1064, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.59  BV=2.70
    {49994,1328,1040, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.68  BV=2.61
    {49994,1424,1040, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.78  BV=2.51
    {49994,1536,1032, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.88  BV=2.41
    {49994,1632,1040, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.98  BV=2.31
    {49994,1728,1056, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.09  BV=2.21
    {49994,1840,1064, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.19  BV=2.11
    {49994,2048,1024, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.29  BV=2.01
    {49994,2144,1048, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.39  BV=1.91
    {49994,2240,1072, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.48  BV=1.81
    {49994,2448,1048, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.58  BV=1.72
    {49994,2656,1040, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.68  BV=1.61
    {49994,2864,1032, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.78  BV=1.51
    {49994,3072,1032, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.88  BV=1.41
    {49994,3328,1024, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.99  BV=1.31
    {49994,3456,1056, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=6.09  BV=1.21
    {58319,3200,1048, 0, 0, 0},  //TV = 4.10(1478 lines)  AV=2.97  SV=5.96  BV=1.11
    {66645,3072,1024, 0, 0, 0},  //TV = 3.91(1689 lines)  AV=2.97  SV=5.87  BV=1.01
    {66645,3200,1056, 0, 0, 0},  //TV = 3.91(1689 lines)  AV=2.97  SV=5.98  BV=0.90
    {75011,3072,1048, 0, 0, 0},  //TV = 3.74(1901 lines)  AV=2.97  SV=5.91  BV=0.80
    {75011,3328,1032, 0, 0, 0},  //TV = 3.74(1901 lines)  AV=2.97  SV=6.00  BV=0.71
    {83336,3200,1032, 0, 0, 0},  //TV = 3.58(2112 lines)  AV=2.97  SV=5.94  BV=0.61
    {83336,3456,1024, 0, 0, 0},  //TV = 3.58(2112 lines)  AV=2.97  SV=6.04  BV=0.51
    {83336,3712,1024, 0, 0, 0},  //TV = 3.58(2112 lines)  AV=2.97  SV=6.14  BV=0.41
    {83336,3968,1032, 0, 0, 0},  //TV = 3.58(2112 lines)  AV=2.97  SV=6.25  BV=0.30
    {83336,4224,1032, 0, 0, 0},  //TV = 3.58(2112 lines)  AV=2.97  SV=6.34  BV=0.21
    {91661,4096,1040, 0, 0, 0},  //TV = 3.45(2323 lines)  AV=2.97  SV=6.31  BV=0.11
    {99987,4096,1032, 0, 0, 0},  //TV = 3.32(2534 lines)  AV=2.97  SV=6.30  BV=-0.01
    {108313,4096,1024, 0, 0, 0},  //TV = 3.21(2745 lines)  AV=2.97  SV=6.29  BV=-0.11
    {108313,4352,1032, 0, 0, 0},  //TV = 3.21(2745 lines)  AV=2.97  SV=6.39  BV=-0.21
    {116678,4224,1048, 0, 0, 0},  //TV = 3.10(2957 lines)  AV=2.97  SV=6.36  BV=-0.29
    {125004,4224,1048, 0, 0, 0},  //TV = 3.00(3168 lines)  AV=2.97  SV=6.36  BV=-0.39
    {125004,4608,1032, 0, 0, 0},  //TV = 3.00(3168 lines)  AV=2.97  SV=6.47  BV=-0.50
    {125004,4864,1048, 0, 0, 0},  //TV = 3.00(3168 lines)  AV=2.97  SV=6.57  BV=-0.60
    {125004,5376,1024, 0, 0, 0},  //TV = 3.00(3168 lines)  AV=2.97  SV=6.68  BV=-0.71
    {125004,5632,1040, 0, 0, 0},  //TV = 3.00(3168 lines)  AV=2.97  SV=6.77  BV=-0.80
    {125004,6016,1040, 0, 0, 0},  //TV = 3.00(3168 lines)  AV=2.97  SV=6.86  BV=-0.89
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
},
};

static strEvPline sCaptureISO1600PLineTable_50Hz =
{
{
    {198,1140,1024, 0, 0, 0},  //TV = 12.30(5 lines)  AV=2.97  SV=4.44  BV=10.83
    {198,1140,1040, 0, 0, 0},  //TV = 12.30(5 lines)  AV=2.97  SV=4.46  BV=10.81
    {198,1216,1048, 0, 0, 0},  //TV = 12.30(5 lines)  AV=2.97  SV=4.57  BV=10.70
    {198,1328,1024, 0, 0, 0},  //TV = 12.30(5 lines)  AV=2.97  SV=4.66  BV=10.61
    {237,1140,1064, 0, 0, 0},  //TV = 12.04(6 lines)  AV=2.97  SV=4.50  BV=10.52
    {237,1216,1072, 0, 0, 0},  //TV = 12.04(6 lines)  AV=2.97  SV=4.60  BV=10.41
    {277,1140,1056, 0, 0, 0},  //TV = 11.82(7 lines)  AV=2.97  SV=4.49  BV=10.30
    {277,1216,1056, 0, 0, 0},  //TV = 11.82(7 lines)  AV=2.97  SV=4.58  BV=10.21
    {316,1140,1056, 0, 0, 0},  //TV = 11.63(8 lines)  AV=2.97  SV=4.49  BV=10.11
    {316,1216,1064, 0, 0, 0},  //TV = 11.63(8 lines)  AV=2.97  SV=4.59  BV=10.01
    {356,1140,1080, 0, 0, 0},  //TV = 11.46(9 lines)  AV=2.97  SV=4.52  BV=9.91
    {395,1140,1040, 0, 0, 0},  //TV = 11.31(10 lines)  AV=2.97  SV=4.46  BV=9.81
    {435,1140,1024, 0, 0, 0},  //TV = 11.17(11 lines)  AV=2.97  SV=4.44  BV=9.70
    {435,1140,1088, 0, 0, 0},  //TV = 11.17(11 lines)  AV=2.97  SV=4.53  BV=9.61
    {474,1140,1064, 0, 0, 0},  //TV = 11.04(12 lines)  AV=2.97  SV=4.50  BV=9.52
    {513,1140,1056, 0, 0, 0},  //TV = 10.93(13 lines)  AV=2.97  SV=4.49  BV=9.41
    {553,1140,1056, 0, 0, 0},  //TV = 10.82(14 lines)  AV=2.97  SV=4.49  BV=9.31
    {592,1140,1056, 0, 0, 0},  //TV = 10.72(15 lines)  AV=2.97  SV=4.49  BV=9.21
    {632,1140,1056, 0, 0, 0},  //TV = 10.63(16 lines)  AV=2.97  SV=4.49  BV=9.11
    {671,1140,1064, 0, 0, 0},  //TV = 10.54(17 lines)  AV=2.97  SV=4.50  BV=9.02
    {750,1140,1024, 0, 0, 0},  //TV = 10.38(19 lines)  AV=2.97  SV=4.44  BV=8.91
    {790,1140,1040, 0, 0, 0},  //TV = 10.31(20 lines)  AV=2.97  SV=4.46  BV=8.81
    {869,1140,1024, 0, 0, 0},  //TV = 10.17(22 lines)  AV=2.97  SV=4.44  BV=8.70
    {908,1140,1040, 0, 0, 0},  //TV = 10.11(23 lines)  AV=2.97  SV=4.46  BV=8.61
    {987,1140,1024, 0, 0, 0},  //TV = 9.98(25 lines)  AV=2.97  SV=4.44  BV=8.51
    {1066,1140,1024, 0, 0, 0},  //TV = 9.87(27 lines)  AV=2.97  SV=4.44  BV=8.40
    {1105,1140,1056, 0, 0, 0},  //TV = 9.82(28 lines)  AV=2.97  SV=4.49  BV=8.31
    {1224,1140,1024, 0, 0, 0},  //TV = 9.67(31 lines)  AV=2.97  SV=4.44  BV=8.20
    {1303,1140,1024, 0, 0, 0},  //TV = 9.58(33 lines)  AV=2.97  SV=4.44  BV=8.11
    {1382,1140,1040, 0, 0, 0},  //TV = 9.50(35 lines)  AV=2.97  SV=4.46  BV=8.01
    {1500,1140,1024, 0, 0, 0},  //TV = 9.38(38 lines)  AV=2.97  SV=4.44  BV=7.91
    {1618,1140,1024, 0, 0, 0},  //TV = 9.27(41 lines)  AV=2.97  SV=4.44  BV=7.80
    {1697,1140,1040, 0, 0, 0},  //TV = 9.20(43 lines)  AV=2.97  SV=4.46  BV=7.71
    {1855,1140,1024, 0, 0, 0},  //TV = 9.07(47 lines)  AV=2.97  SV=4.44  BV=7.60
    {1973,1140,1024, 0, 0, 0},  //TV = 8.99(50 lines)  AV=2.97  SV=4.44  BV=7.51
    {2131,1140,1024, 0, 0, 0},  //TV = 8.87(54 lines)  AV=2.97  SV=4.44  BV=7.40
    {2250,1140,1032, 0, 0, 0},  //TV = 8.80(57 lines)  AV=2.97  SV=4.45  BV=7.31
    {2447,1140,1024, 0, 0, 0},  //TV = 8.67(62 lines)  AV=2.97  SV=4.44  BV=7.20
    {2605,1140,1024, 0, 0, 0},  //TV = 8.58(66 lines)  AV=2.97  SV=4.44  BV=7.11
    {2802,1140,1024, 0, 0, 0},  //TV = 8.48(71 lines)  AV=2.97  SV=4.44  BV=7.01
    {2999,1140,1024, 0, 0, 0},  //TV = 8.38(76 lines)  AV=2.97  SV=4.44  BV=6.91
    {3236,1140,1024, 0, 0, 0},  //TV = 8.27(82 lines)  AV=2.97  SV=4.44  BV=6.80
    {3473,1140,1024, 0, 0, 0},  //TV = 8.17(88 lines)  AV=2.97  SV=4.44  BV=6.70
    {3710,1140,1024, 0, 0, 0},  //TV = 8.07(94 lines)  AV=2.97  SV=4.44  BV=6.60
    {3986,1140,1024, 0, 0, 0},  //TV = 7.97(101 lines)  AV=2.97  SV=4.44  BV=6.50
    {4262,1140,1024, 0, 0, 0},  //TV = 7.87(108 lines)  AV=2.97  SV=4.44  BV=6.40
    {4538,1140,1024, 0, 0, 0},  //TV = 7.78(115 lines)  AV=2.97  SV=4.44  BV=6.31
    {4893,1140,1024, 0, 0, 0},  //TV = 7.68(124 lines)  AV=2.97  SV=4.44  BV=6.20
    {5209,1140,1024, 0, 0, 0},  //TV = 7.58(132 lines)  AV=2.97  SV=4.44  BV=6.11
    {5643,1140,1024, 0, 0, 0},  //TV = 7.47(143 lines)  AV=2.97  SV=4.44  BV=6.00
    {6038,1140,1024, 0, 0, 0},  //TV = 7.37(153 lines)  AV=2.97  SV=4.44  BV=5.90
    {6472,1140,1024, 0, 0, 0},  //TV = 7.27(164 lines)  AV=2.97  SV=4.44  BV=5.80
    {6906,1140,1024, 0, 0, 0},  //TV = 7.18(175 lines)  AV=2.97  SV=4.44  BV=5.71
    {7419,1140,1024, 0, 0, 0},  //TV = 7.07(188 lines)  AV=2.97  SV=4.44  BV=5.60
    {7971,1140,1024, 0, 0, 0},  //TV = 6.97(202 lines)  AV=2.97  SV=4.44  BV=5.50
    {8484,1140,1024, 0, 0, 0},  //TV = 6.88(215 lines)  AV=2.97  SV=4.44  BV=5.41
    {9155,1140,1024, 0, 0, 0},  //TV = 6.77(232 lines)  AV=2.97  SV=4.44  BV=5.30
    {9826,1140,1024, 0, 0, 0},  //TV = 6.67(249 lines)  AV=2.97  SV=4.44  BV=5.20
    {9983,1140,1072, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=4.51  BV=5.11
    {9983,1216,1072, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=4.60  BV=5.02
    {9983,1328,1056, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=4.71  BV=4.91
    {9983,1424,1056, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=4.81  BV=4.81
    {9983,1536,1048, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=4.91  BV=4.71
    {9983,1632,1064, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=5.01  BV=4.60
    {9983,1728,1072, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=5.11  BV=4.51
    {9983,1936,1024, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=5.21  BV=4.41
    {9983,2048,1040, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=5.31  BV=4.31
    {9983,2144,1064, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=5.41  BV=4.21
    {20006,1140,1064, 0, 0, 0},  //TV = 5.64(507 lines)  AV=2.97  SV=4.50  BV=4.12
    {20006,1216,1072, 0, 0, 0},  //TV = 5.64(507 lines)  AV=2.97  SV=4.60  BV=4.01
    {20006,1328,1048, 0, 0, 0},  //TV = 5.64(507 lines)  AV=2.97  SV=4.70  BV=3.92
    {20006,1424,1064, 0, 0, 0},  //TV = 5.64(507 lines)  AV=2.97  SV=4.82  BV=3.80
    {20006,1536,1048, 0, 0, 0},  //TV = 5.64(507 lines)  AV=2.97  SV=4.91  BV=3.71
    {20006,1632,1056, 0, 0, 0},  //TV = 5.64(507 lines)  AV=2.97  SV=5.00  BV=3.61
    {29989,1140,1080, 0, 0, 0},  //TV = 5.06(760 lines)  AV=2.97  SV=4.52  BV=3.51
    {29989,1216,1096, 0, 0, 0},  //TV = 5.06(760 lines)  AV=2.97  SV=4.63  BV=3.40
    {29989,1328,1064, 0, 0, 0},  //TV = 5.06(760 lines)  AV=2.97  SV=4.72  BV=3.31
    {29989,1424,1072, 0, 0, 0},  //TV = 5.06(760 lines)  AV=2.97  SV=4.83  BV=3.20
    {40011,1140,1064, 0, 0, 0},  //TV = 4.64(1014 lines)  AV=2.97  SV=4.50  BV=3.12
    {40011,1216,1072, 0, 0, 0},  //TV = 4.64(1014 lines)  AV=2.97  SV=4.60  BV=3.01
    {40011,1328,1048, 0, 0, 0},  //TV = 4.64(1014 lines)  AV=2.97  SV=4.70  BV=2.92
    {49994,1140,1056, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.49  BV=2.81
    {49994,1216,1064, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.59  BV=2.70
    {49994,1328,1040, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.68  BV=2.61
    {49994,1424,1040, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.78  BV=2.51
    {49994,1536,1032, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.88  BV=2.41
    {49994,1632,1040, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.98  BV=2.31
    {49994,1728,1056, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.09  BV=2.21
    {49994,1840,1064, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.19  BV=2.11
    {49994,2048,1024, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.29  BV=2.01
    {49994,2144,1048, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.39  BV=1.91
    {49994,2240,1072, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.48  BV=1.81
    {49994,2448,1048, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.58  BV=1.72
    {49994,2656,1040, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.68  BV=1.61
    {49994,2864,1032, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.78  BV=1.51
    {49994,3072,1032, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.88  BV=1.41
    {49994,3328,1024, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.99  BV=1.31
    {49994,3456,1056, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=6.09  BV=1.21
    {60017,3072,1064, 0, 0, 0},  //TV = 4.06(1521 lines)  AV=2.97  SV=5.93  BV=1.10
    {60017,3328,1048, 0, 0, 0},  //TV = 4.06(1521 lines)  AV=2.97  SV=6.02  BV=1.01
    {69999,3072,1048, 0, 0, 0},  //TV = 3.84(1774 lines)  AV=2.97  SV=5.91  BV=0.90
    {69999,3328,1032, 0, 0, 0},  //TV = 3.84(1774 lines)  AV=2.97  SV=6.00  BV=0.81
    {79982,3072,1048, 0, 0, 0},  //TV = 3.64(2027 lines)  AV=2.97  SV=5.91  BV=0.71
    {79982,3328,1032, 0, 0, 0},  //TV = 3.64(2027 lines)  AV=2.97  SV=6.00  BV=0.62
    {79982,3584,1032, 0, 0, 0},  //TV = 3.64(2027 lines)  AV=2.97  SV=6.11  BV=0.51
    {79982,3840,1032, 0, 0, 0},  //TV = 3.64(2027 lines)  AV=2.97  SV=6.20  BV=0.41
    {79982,4096,1040, 0, 0, 0},  //TV = 3.64(2027 lines)  AV=2.97  SV=6.31  BV=0.31
    {79982,4352,1040, 0, 0, 0},  //TV = 3.64(2027 lines)  AV=2.97  SV=6.40  BV=0.22
    {90005,4224,1024, 0, 0, 0},  //TV = 3.47(2281 lines)  AV=2.97  SV=6.33  BV=0.11
    {99987,4096,1032, 0, 0, 0},  //TV = 3.32(2534 lines)  AV=2.97  SV=6.30  BV=-0.01
    {99987,4352,1040, 0, 0, 0},  //TV = 3.32(2534 lines)  AV=2.97  SV=6.40  BV=-0.10
    {110010,4224,1040, 0, 0, 0},  //TV = 3.18(2788 lines)  AV=2.97  SV=6.35  BV=-0.20
    {119992,4096,1056, 0, 0, 0},  //TV = 3.06(3041 lines)  AV=2.97  SV=6.33  BV=-0.30
    {119992,4480,1032, 0, 0, 0},  //TV = 3.06(3041 lines)  AV=2.97  SV=6.43  BV=-0.40
    {119992,4736,1048, 0, 0, 0},  //TV = 3.06(3041 lines)  AV=2.97  SV=6.53  BV=-0.50
    {119992,5120,1040, 0, 0, 0},  //TV = 3.06(3041 lines)  AV=2.97  SV=6.63  BV=-0.60
    {119992,5504,1040, 0, 0, 0},  //TV = 3.06(3041 lines)  AV=2.97  SV=6.74  BV=-0.71
    {119992,5888,1040, 0, 0, 0},  //TV = 3.06(3041 lines)  AV=2.97  SV=6.83  BV=-0.80
    {119992,6144,1064, 0, 0, 0},  //TV = 3.06(3041 lines)  AV=2.97  SV=6.93  BV=-0.90
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
},
};

static strAETable g_AE_CaptureISO1600Table =
{
    AETABLE_CAPTURE_ISO1600,    //eAETableID
    119,    //u4TotalIndex
    20,    //u4StrobeTrigerBV
    109,    //i4MaxBV
    -9,    //i4MinBV
    90,    //i4EffectiveMaxBV
    0,      //i4EffectiveMinBV
    LIB3A_AE_ISO_SPEED_1600, //ISO SPEED
    sCaptureISO1600PLineTable_60Hz,
    sCaptureISO1600PLineTable_50Hz,
    NULL,
};

static strEvPline sCaptureStrobePLineTable_60Hz =
{
{
    {198,1140,1024, 0, 0, 0},  //TV = 12.30(5 lines)  AV=2.97  SV=4.44  BV=10.83
    {198,1140,1040, 0, 0, 0},  //TV = 12.30(5 lines)  AV=2.97  SV=4.46  BV=10.81
    {198,1216,1048, 0, 0, 0},  //TV = 12.30(5 lines)  AV=2.97  SV=4.57  BV=10.70
    {198,1328,1024, 0, 0, 0},  //TV = 12.30(5 lines)  AV=2.97  SV=4.66  BV=10.61
    {237,1140,1064, 0, 0, 0},  //TV = 12.04(6 lines)  AV=2.97  SV=4.50  BV=10.52
    {237,1216,1072, 0, 0, 0},  //TV = 12.04(6 lines)  AV=2.97  SV=4.60  BV=10.41
    {277,1140,1056, 0, 0, 0},  //TV = 11.82(7 lines)  AV=2.97  SV=4.49  BV=10.30
    {277,1216,1056, 0, 0, 0},  //TV = 11.82(7 lines)  AV=2.97  SV=4.58  BV=10.21
    {316,1140,1056, 0, 0, 0},  //TV = 11.63(8 lines)  AV=2.97  SV=4.49  BV=10.11
    {316,1216,1064, 0, 0, 0},  //TV = 11.63(8 lines)  AV=2.97  SV=4.59  BV=10.01
    {356,1140,1080, 0, 0, 0},  //TV = 11.46(9 lines)  AV=2.97  SV=4.52  BV=9.91
    {395,1140,1040, 0, 0, 0},  //TV = 11.31(10 lines)  AV=2.97  SV=4.46  BV=9.81
    {435,1140,1024, 0, 0, 0},  //TV = 11.17(11 lines)  AV=2.97  SV=4.44  BV=9.70
    {435,1140,1088, 0, 0, 0},  //TV = 11.17(11 lines)  AV=2.97  SV=4.53  BV=9.61
    {474,1140,1064, 0, 0, 0},  //TV = 11.04(12 lines)  AV=2.97  SV=4.50  BV=9.52
    {513,1140,1056, 0, 0, 0},  //TV = 10.93(13 lines)  AV=2.97  SV=4.49  BV=9.41
    {553,1140,1056, 0, 0, 0},  //TV = 10.82(14 lines)  AV=2.97  SV=4.49  BV=9.31
    {592,1140,1056, 0, 0, 0},  //TV = 10.72(15 lines)  AV=2.97  SV=4.49  BV=9.21
    {632,1140,1056, 0, 0, 0},  //TV = 10.63(16 lines)  AV=2.97  SV=4.49  BV=9.11
    {671,1140,1064, 0, 0, 0},  //TV = 10.54(17 lines)  AV=2.97  SV=4.50  BV=9.02
    {750,1140,1024, 0, 0, 0},  //TV = 10.38(19 lines)  AV=2.97  SV=4.44  BV=8.91
    {790,1140,1040, 0, 0, 0},  //TV = 10.31(20 lines)  AV=2.97  SV=4.46  BV=8.81
    {869,1140,1024, 0, 0, 0},  //TV = 10.17(22 lines)  AV=2.97  SV=4.44  BV=8.70
    {908,1140,1040, 0, 0, 0},  //TV = 10.11(23 lines)  AV=2.97  SV=4.46  BV=8.61
    {987,1140,1024, 0, 0, 0},  //TV = 9.98(25 lines)  AV=2.97  SV=4.44  BV=8.51
    {1066,1140,1024, 0, 0, 0},  //TV = 9.87(27 lines)  AV=2.97  SV=4.44  BV=8.40
    {1105,1140,1056, 0, 0, 0},  //TV = 9.82(28 lines)  AV=2.97  SV=4.49  BV=8.31
    {1224,1140,1024, 0, 0, 0},  //TV = 9.67(31 lines)  AV=2.97  SV=4.44  BV=8.20
    {1303,1140,1024, 0, 0, 0},  //TV = 9.58(33 lines)  AV=2.97  SV=4.44  BV=8.11
    {1382,1140,1040, 0, 0, 0},  //TV = 9.50(35 lines)  AV=2.97  SV=4.46  BV=8.01
    {1500,1140,1024, 0, 0, 0},  //TV = 9.38(38 lines)  AV=2.97  SV=4.44  BV=7.91
    {1618,1140,1024, 0, 0, 0},  //TV = 9.27(41 lines)  AV=2.97  SV=4.44  BV=7.80
    {1697,1140,1040, 0, 0, 0},  //TV = 9.20(43 lines)  AV=2.97  SV=4.46  BV=7.71
    {1855,1140,1024, 0, 0, 0},  //TV = 9.07(47 lines)  AV=2.97  SV=4.44  BV=7.60
    {1973,1140,1024, 0, 0, 0},  //TV = 8.99(50 lines)  AV=2.97  SV=4.44  BV=7.51
    {2131,1140,1024, 0, 0, 0},  //TV = 8.87(54 lines)  AV=2.97  SV=4.44  BV=7.40
    {2250,1140,1032, 0, 0, 0},  //TV = 8.80(57 lines)  AV=2.97  SV=4.45  BV=7.31
    {2447,1140,1024, 0, 0, 0},  //TV = 8.67(62 lines)  AV=2.97  SV=4.44  BV=7.20
    {2605,1140,1024, 0, 0, 0},  //TV = 8.58(66 lines)  AV=2.97  SV=4.44  BV=7.11
    {2802,1140,1024, 0, 0, 0},  //TV = 8.48(71 lines)  AV=2.97  SV=4.44  BV=7.01
    {2999,1140,1024, 0, 0, 0},  //TV = 8.38(76 lines)  AV=2.97  SV=4.44  BV=6.91
    {3236,1140,1024, 0, 0, 0},  //TV = 8.27(82 lines)  AV=2.97  SV=4.44  BV=6.80
    {3473,1140,1024, 0, 0, 0},  //TV = 8.17(88 lines)  AV=2.97  SV=4.44  BV=6.70
    {3710,1140,1024, 0, 0, 0},  //TV = 8.07(94 lines)  AV=2.97  SV=4.44  BV=6.60
    {3986,1140,1024, 0, 0, 0},  //TV = 7.97(101 lines)  AV=2.97  SV=4.44  BV=6.50
    {4262,1140,1024, 0, 0, 0},  //TV = 7.87(108 lines)  AV=2.97  SV=4.44  BV=6.40
    {4538,1140,1024, 0, 0, 0},  //TV = 7.78(115 lines)  AV=2.97  SV=4.44  BV=6.31
    {4893,1140,1024, 0, 0, 0},  //TV = 7.68(124 lines)  AV=2.97  SV=4.44  BV=6.20
    {5209,1140,1024, 0, 0, 0},  //TV = 7.58(132 lines)  AV=2.97  SV=4.44  BV=6.11
    {5643,1140,1024, 0, 0, 0},  //TV = 7.47(143 lines)  AV=2.97  SV=4.44  BV=6.00
    {6038,1140,1024, 0, 0, 0},  //TV = 7.37(153 lines)  AV=2.97  SV=4.44  BV=5.90
    {6472,1140,1024, 0, 0, 0},  //TV = 7.27(164 lines)  AV=2.97  SV=4.44  BV=5.80
    {6906,1140,1024, 0, 0, 0},  //TV = 7.18(175 lines)  AV=2.97  SV=4.44  BV=5.71
    {7419,1140,1024, 0, 0, 0},  //TV = 7.07(188 lines)  AV=2.97  SV=4.44  BV=5.60
    {7971,1140,1024, 0, 0, 0},  //TV = 6.97(202 lines)  AV=2.97  SV=4.44  BV=5.50
    {8326,1140,1048, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=4.48  BV=5.40
    {8326,1216,1048, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=4.57  BV=5.31
    {8326,1328,1024, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=4.66  BV=5.22
    {8326,1424,1024, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=4.76  BV=5.12
    {8326,1536,1024, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=4.87  BV=5.01
    {8326,1632,1032, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=4.97  BV=4.91
    {8326,1728,1040, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=5.06  BV=4.81
    {8326,1840,1048, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=5.17  BV=4.71
    {8326,1936,1072, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=5.27  BV=4.61
    {8326,2144,1032, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=5.36  BV=4.51
    {16652,1140,1048, 0, 0, 0},  //TV = 5.91(422 lines)  AV=2.97  SV=4.48  BV=4.40
    {16652,1216,1048, 0, 0, 0},  //TV = 5.91(422 lines)  AV=2.97  SV=4.57  BV=4.31
    {16652,1328,1024, 0, 0, 0},  //TV = 5.91(422 lines)  AV=2.97  SV=4.66  BV=4.22
    {16652,1424,1024, 0, 0, 0},  //TV = 5.91(422 lines)  AV=2.97  SV=4.76  BV=4.12
    {16652,1424,1104, 0, 0, 0},  //TV = 5.91(422 lines)  AV=2.97  SV=4.87  BV=4.01
    {16652,1632,1032, 0, 0, 0},  //TV = 5.91(422 lines)  AV=2.97  SV=4.97  BV=3.91
    {25017,1140,1056, 0, 0, 0},  //TV = 5.32(634 lines)  AV=2.97  SV=4.49  BV=3.81
    {25017,1216,1064, 0, 0, 0},  //TV = 5.32(634 lines)  AV=2.97  SV=4.59  BV=3.70
    {25017,1328,1048, 0, 0, 0},  //TV = 5.32(634 lines)  AV=2.97  SV=4.70  BV=3.60
    {25017,1424,1040, 0, 0, 0},  //TV = 5.32(634 lines)  AV=2.97  SV=4.78  BV=3.51
    {33343,1140,1048, 0, 0, 0},  //TV = 4.91(845 lines)  AV=2.97  SV=4.48  BV=3.40
    {33343,1216,1048, 0, 0, 0},  //TV = 4.91(845 lines)  AV=2.97  SV=4.57  BV=3.31
    {33343,1328,1024, 0, 0, 0},  //TV = 4.91(845 lines)  AV=2.97  SV=4.66  BV=3.22
    {41669,1140,1024, 0, 0, 0},  //TV = 4.58(1056 lines)  AV=2.97  SV=4.44  BV=3.11
    {41669,1216,1032, 0, 0, 0},  //TV = 4.58(1056 lines)  AV=2.97  SV=4.55  BV=3.01
    {41669,1216,1104, 0, 0, 0},  //TV = 4.58(1056 lines)  AV=2.97  SV=4.64  BV=2.91
    {49994,1140,1056, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.49  BV=2.81
    {49994,1216,1064, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.59  BV=2.70
    {49994,1328,1040, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.68  BV=2.61
    {49994,1424,1040, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.78  BV=2.51
    {49994,1536,1032, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.88  BV=2.41
    {49994,1632,1040, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.98  BV=2.31
    {49994,1728,1056, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.09  BV=2.21
    {49994,1840,1064, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.19  BV=2.11
    {49994,2048,1024, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.29  BV=2.01
    {49994,2144,1048, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.39  BV=1.91
    {49994,2240,1072, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.48  BV=1.81
    {49994,2448,1048, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.58  BV=1.72
    {49994,2656,1040, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.68  BV=1.61
    {49994,2864,1032, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.78  BV=1.51
    {49994,3072,1032, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.88  BV=1.41
    {49994,3328,1024, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.99  BV=1.31
    {49994,3456,1056, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=6.09  BV=1.21
    {58319,3200,1048, 0, 0, 0},  //TV = 4.10(1478 lines)  AV=2.97  SV=5.96  BV=1.11
    {66645,3072,1024, 0, 0, 0},  //TV = 3.91(1689 lines)  AV=2.97  SV=5.87  BV=1.01
    {66645,3200,1056, 0, 0, 0},  //TV = 3.91(1689 lines)  AV=2.97  SV=5.98  BV=0.90
    {75011,3072,1048, 0, 0, 0},  //TV = 3.74(1901 lines)  AV=2.97  SV=5.91  BV=0.80
    {75011,3328,1032, 0, 0, 0},  //TV = 3.74(1901 lines)  AV=2.97  SV=6.00  BV=0.71
    {83336,3200,1032, 0, 0, 0},  //TV = 3.58(2112 lines)  AV=2.97  SV=5.94  BV=0.61
    {83336,3456,1024, 0, 0, 0},  //TV = 3.58(2112 lines)  AV=2.97  SV=6.04  BV=0.51
    {83336,3712,1024, 0, 0, 0},  //TV = 3.58(2112 lines)  AV=2.97  SV=6.14  BV=0.41
    {83336,3968,1032, 0, 0, 0},  //TV = 3.58(2112 lines)  AV=2.97  SV=6.25  BV=0.30
    {83336,4224,1032, 0, 0, 0},  //TV = 3.58(2112 lines)  AV=2.97  SV=6.34  BV=0.21
    {91661,4096,1040, 0, 0, 0},  //TV = 3.45(2323 lines)  AV=2.97  SV=6.31  BV=0.11
    {99987,4096,1032, 0, 0, 0},  //TV = 3.32(2534 lines)  AV=2.97  SV=6.30  BV=-0.01
    {108313,4096,1024, 0, 0, 0},  //TV = 3.21(2745 lines)  AV=2.97  SV=6.29  BV=-0.11
    {108313,4352,1032, 0, 0, 0},  //TV = 3.21(2745 lines)  AV=2.97  SV=6.39  BV=-0.21
    {116678,4224,1048, 0, 0, 0},  //TV = 3.10(2957 lines)  AV=2.97  SV=6.36  BV=-0.29
    {125004,4224,1048, 0, 0, 0},  //TV = 3.00(3168 lines)  AV=2.97  SV=6.36  BV=-0.39
    {125004,4608,1032, 0, 0, 0},  //TV = 3.00(3168 lines)  AV=2.97  SV=6.47  BV=-0.50
    {125004,4864,1048, 0, 0, 0},  //TV = 3.00(3168 lines)  AV=2.97  SV=6.57  BV=-0.60
    {125004,5376,1024, 0, 0, 0},  //TV = 3.00(3168 lines)  AV=2.97  SV=6.68  BV=-0.71
    {125004,5632,1040, 0, 0, 0},  //TV = 3.00(3168 lines)  AV=2.97  SV=6.77  BV=-0.80
    {125004,6016,1040, 0, 0, 0},  //TV = 3.00(3168 lines)  AV=2.97  SV=6.86  BV=-0.89
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
},
};

static strEvPline sCaptureStrobePLineTable_50Hz =
{
{
    {198,1140,1024, 0, 0, 0},  //TV = 12.30(5 lines)  AV=2.97  SV=4.44  BV=10.83
    {198,1140,1040, 0, 0, 0},  //TV = 12.30(5 lines)  AV=2.97  SV=4.46  BV=10.81
    {198,1216,1048, 0, 0, 0},  //TV = 12.30(5 lines)  AV=2.97  SV=4.57  BV=10.70
    {198,1328,1024, 0, 0, 0},  //TV = 12.30(5 lines)  AV=2.97  SV=4.66  BV=10.61
    {237,1140,1064, 0, 0, 0},  //TV = 12.04(6 lines)  AV=2.97  SV=4.50  BV=10.52
    {237,1216,1072, 0, 0, 0},  //TV = 12.04(6 lines)  AV=2.97  SV=4.60  BV=10.41
    {277,1140,1056, 0, 0, 0},  //TV = 11.82(7 lines)  AV=2.97  SV=4.49  BV=10.30
    {277,1216,1056, 0, 0, 0},  //TV = 11.82(7 lines)  AV=2.97  SV=4.58  BV=10.21
    {316,1140,1056, 0, 0, 0},  //TV = 11.63(8 lines)  AV=2.97  SV=4.49  BV=10.11
    {316,1216,1064, 0, 0, 0},  //TV = 11.63(8 lines)  AV=2.97  SV=4.59  BV=10.01
    {356,1140,1080, 0, 0, 0},  //TV = 11.46(9 lines)  AV=2.97  SV=4.52  BV=9.91
    {395,1140,1040, 0, 0, 0},  //TV = 11.31(10 lines)  AV=2.97  SV=4.46  BV=9.81
    {435,1140,1024, 0, 0, 0},  //TV = 11.17(11 lines)  AV=2.97  SV=4.44  BV=9.70
    {435,1140,1088, 0, 0, 0},  //TV = 11.17(11 lines)  AV=2.97  SV=4.53  BV=9.61
    {474,1140,1064, 0, 0, 0},  //TV = 11.04(12 lines)  AV=2.97  SV=4.50  BV=9.52
    {513,1140,1056, 0, 0, 0},  //TV = 10.93(13 lines)  AV=2.97  SV=4.49  BV=9.41
    {553,1140,1056, 0, 0, 0},  //TV = 10.82(14 lines)  AV=2.97  SV=4.49  BV=9.31
    {592,1140,1056, 0, 0, 0},  //TV = 10.72(15 lines)  AV=2.97  SV=4.49  BV=9.21
    {632,1140,1056, 0, 0, 0},  //TV = 10.63(16 lines)  AV=2.97  SV=4.49  BV=9.11
    {671,1140,1064, 0, 0, 0},  //TV = 10.54(17 lines)  AV=2.97  SV=4.50  BV=9.02
    {750,1140,1024, 0, 0, 0},  //TV = 10.38(19 lines)  AV=2.97  SV=4.44  BV=8.91
    {790,1140,1040, 0, 0, 0},  //TV = 10.31(20 lines)  AV=2.97  SV=4.46  BV=8.81
    {869,1140,1024, 0, 0, 0},  //TV = 10.17(22 lines)  AV=2.97  SV=4.44  BV=8.70
    {908,1140,1040, 0, 0, 0},  //TV = 10.11(23 lines)  AV=2.97  SV=4.46  BV=8.61
    {987,1140,1024, 0, 0, 0},  //TV = 9.98(25 lines)  AV=2.97  SV=4.44  BV=8.51
    {1066,1140,1024, 0, 0, 0},  //TV = 9.87(27 lines)  AV=2.97  SV=4.44  BV=8.40
    {1105,1140,1056, 0, 0, 0},  //TV = 9.82(28 lines)  AV=2.97  SV=4.49  BV=8.31
    {1224,1140,1024, 0, 0, 0},  //TV = 9.67(31 lines)  AV=2.97  SV=4.44  BV=8.20
    {1303,1140,1024, 0, 0, 0},  //TV = 9.58(33 lines)  AV=2.97  SV=4.44  BV=8.11
    {1382,1140,1040, 0, 0, 0},  //TV = 9.50(35 lines)  AV=2.97  SV=4.46  BV=8.01
    {1500,1140,1024, 0, 0, 0},  //TV = 9.38(38 lines)  AV=2.97  SV=4.44  BV=7.91
    {1618,1140,1024, 0, 0, 0},  //TV = 9.27(41 lines)  AV=2.97  SV=4.44  BV=7.80
    {1697,1140,1040, 0, 0, 0},  //TV = 9.20(43 lines)  AV=2.97  SV=4.46  BV=7.71
    {1855,1140,1024, 0, 0, 0},  //TV = 9.07(47 lines)  AV=2.97  SV=4.44  BV=7.60
    {1973,1140,1024, 0, 0, 0},  //TV = 8.99(50 lines)  AV=2.97  SV=4.44  BV=7.51
    {2131,1140,1024, 0, 0, 0},  //TV = 8.87(54 lines)  AV=2.97  SV=4.44  BV=7.40
    {2250,1140,1032, 0, 0, 0},  //TV = 8.80(57 lines)  AV=2.97  SV=4.45  BV=7.31
    {2447,1140,1024, 0, 0, 0},  //TV = 8.67(62 lines)  AV=2.97  SV=4.44  BV=7.20
    {2605,1140,1024, 0, 0, 0},  //TV = 8.58(66 lines)  AV=2.97  SV=4.44  BV=7.11
    {2802,1140,1024, 0, 0, 0},  //TV = 8.48(71 lines)  AV=2.97  SV=4.44  BV=7.01
    {2999,1140,1024, 0, 0, 0},  //TV = 8.38(76 lines)  AV=2.97  SV=4.44  BV=6.91
    {3236,1140,1024, 0, 0, 0},  //TV = 8.27(82 lines)  AV=2.97  SV=4.44  BV=6.80
    {3473,1140,1024, 0, 0, 0},  //TV = 8.17(88 lines)  AV=2.97  SV=4.44  BV=6.70
    {3710,1140,1024, 0, 0, 0},  //TV = 8.07(94 lines)  AV=2.97  SV=4.44  BV=6.60
    {3986,1140,1024, 0, 0, 0},  //TV = 7.97(101 lines)  AV=2.97  SV=4.44  BV=6.50
    {4262,1140,1024, 0, 0, 0},  //TV = 7.87(108 lines)  AV=2.97  SV=4.44  BV=6.40
    {4538,1140,1024, 0, 0, 0},  //TV = 7.78(115 lines)  AV=2.97  SV=4.44  BV=6.31
    {4893,1140,1024, 0, 0, 0},  //TV = 7.68(124 lines)  AV=2.97  SV=4.44  BV=6.20
    {5209,1140,1024, 0, 0, 0},  //TV = 7.58(132 lines)  AV=2.97  SV=4.44  BV=6.11
    {5643,1140,1024, 0, 0, 0},  //TV = 7.47(143 lines)  AV=2.97  SV=4.44  BV=6.00
    {6038,1140,1024, 0, 0, 0},  //TV = 7.37(153 lines)  AV=2.97  SV=4.44  BV=5.90
    {6472,1140,1024, 0, 0, 0},  //TV = 7.27(164 lines)  AV=2.97  SV=4.44  BV=5.80
    {6906,1140,1024, 0, 0, 0},  //TV = 7.18(175 lines)  AV=2.97  SV=4.44  BV=5.71
    {7419,1140,1024, 0, 0, 0},  //TV = 7.07(188 lines)  AV=2.97  SV=4.44  BV=5.60
    {7971,1140,1024, 0, 0, 0},  //TV = 6.97(202 lines)  AV=2.97  SV=4.44  BV=5.50
    {8484,1140,1024, 0, 0, 0},  //TV = 6.88(215 lines)  AV=2.97  SV=4.44  BV=5.41
    {9155,1140,1024, 0, 0, 0},  //TV = 6.77(232 lines)  AV=2.97  SV=4.44  BV=5.30
    {9826,1140,1024, 0, 0, 0},  //TV = 6.67(249 lines)  AV=2.97  SV=4.44  BV=5.20
    {9983,1140,1072, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=4.51  BV=5.11
    {9983,1216,1072, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=4.60  BV=5.02
    {9983,1328,1056, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=4.71  BV=4.91
    {9983,1424,1056, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=4.81  BV=4.81
    {9983,1536,1048, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=4.91  BV=4.71
    {9983,1632,1064, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=5.01  BV=4.60
    {9983,1728,1072, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=5.11  BV=4.51
    {9983,1936,1024, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=5.21  BV=4.41
    {9983,2048,1040, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=5.31  BV=4.31
    {9983,2144,1064, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=5.41  BV=4.21
    {20006,1140,1064, 0, 0, 0},  //TV = 5.64(507 lines)  AV=2.97  SV=4.50  BV=4.12
    {20006,1216,1072, 0, 0, 0},  //TV = 5.64(507 lines)  AV=2.97  SV=4.60  BV=4.01
    {20006,1328,1048, 0, 0, 0},  //TV = 5.64(507 lines)  AV=2.97  SV=4.70  BV=3.92
    {20006,1424,1064, 0, 0, 0},  //TV = 5.64(507 lines)  AV=2.97  SV=4.82  BV=3.80
    {20006,1536,1048, 0, 0, 0},  //TV = 5.64(507 lines)  AV=2.97  SV=4.91  BV=3.71
    {20006,1632,1056, 0, 0, 0},  //TV = 5.64(507 lines)  AV=2.97  SV=5.00  BV=3.61
    {29989,1140,1080, 0, 0, 0},  //TV = 5.06(760 lines)  AV=2.97  SV=4.52  BV=3.51
    {29989,1216,1096, 0, 0, 0},  //TV = 5.06(760 lines)  AV=2.97  SV=4.63  BV=3.40
    {29989,1328,1064, 0, 0, 0},  //TV = 5.06(760 lines)  AV=2.97  SV=4.72  BV=3.31
    {29989,1424,1072, 0, 0, 0},  //TV = 5.06(760 lines)  AV=2.97  SV=4.83  BV=3.20
    {40011,1140,1064, 0, 0, 0},  //TV = 4.64(1014 lines)  AV=2.97  SV=4.50  BV=3.12
    {40011,1216,1072, 0, 0, 0},  //TV = 4.64(1014 lines)  AV=2.97  SV=4.60  BV=3.01
    {40011,1328,1048, 0, 0, 0},  //TV = 4.64(1014 lines)  AV=2.97  SV=4.70  BV=2.92
    {49994,1140,1056, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.49  BV=2.81
    {49994,1216,1064, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.59  BV=2.70
    {49994,1328,1040, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.68  BV=2.61
    {49994,1424,1040, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.78  BV=2.51
    {49994,1536,1032, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.88  BV=2.41
    {49994,1632,1040, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.98  BV=2.31
    {49994,1728,1056, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.09  BV=2.21
    {49994,1840,1064, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.19  BV=2.11
    {49994,2048,1024, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.29  BV=2.01
    {49994,2144,1048, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.39  BV=1.91
    {49994,2240,1072, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.48  BV=1.81
    {49994,2448,1048, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.58  BV=1.72
    {49994,2656,1040, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.68  BV=1.61
    {49994,2864,1032, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.78  BV=1.51
    {49994,3072,1032, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.88  BV=1.41
    {49994,3328,1024, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.99  BV=1.31
    {49994,3456,1056, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=6.09  BV=1.21
    {60017,3072,1064, 0, 0, 0},  //TV = 4.06(1521 lines)  AV=2.97  SV=5.93  BV=1.10
    {60017,3328,1048, 0, 0, 0},  //TV = 4.06(1521 lines)  AV=2.97  SV=6.02  BV=1.01
    {69999,3072,1048, 0, 0, 0},  //TV = 3.84(1774 lines)  AV=2.97  SV=5.91  BV=0.90
    {69999,3328,1032, 0, 0, 0},  //TV = 3.84(1774 lines)  AV=2.97  SV=6.00  BV=0.81
    {79982,3072,1048, 0, 0, 0},  //TV = 3.64(2027 lines)  AV=2.97  SV=5.91  BV=0.71
    {79982,3328,1032, 0, 0, 0},  //TV = 3.64(2027 lines)  AV=2.97  SV=6.00  BV=0.62
    {79982,3584,1032, 0, 0, 0},  //TV = 3.64(2027 lines)  AV=2.97  SV=6.11  BV=0.51
    {79982,3840,1032, 0, 0, 0},  //TV = 3.64(2027 lines)  AV=2.97  SV=6.20  BV=0.41
    {79982,4096,1040, 0, 0, 0},  //TV = 3.64(2027 lines)  AV=2.97  SV=6.31  BV=0.31
    {79982,4352,1040, 0, 0, 0},  //TV = 3.64(2027 lines)  AV=2.97  SV=6.40  BV=0.22
    {90005,4224,1024, 0, 0, 0},  //TV = 3.47(2281 lines)  AV=2.97  SV=6.33  BV=0.11
    {99987,4096,1032, 0, 0, 0},  //TV = 3.32(2534 lines)  AV=2.97  SV=6.30  BV=-0.01
    {99987,4352,1040, 0, 0, 0},  //TV = 3.32(2534 lines)  AV=2.97  SV=6.40  BV=-0.10
    {110010,4224,1040, 0, 0, 0},  //TV = 3.18(2788 lines)  AV=2.97  SV=6.35  BV=-0.20
    {119992,4096,1056, 0, 0, 0},  //TV = 3.06(3041 lines)  AV=2.97  SV=6.33  BV=-0.30
    {119992,4480,1032, 0, 0, 0},  //TV = 3.06(3041 lines)  AV=2.97  SV=6.43  BV=-0.40
    {119992,4736,1048, 0, 0, 0},  //TV = 3.06(3041 lines)  AV=2.97  SV=6.53  BV=-0.50
    {119992,5120,1040, 0, 0, 0},  //TV = 3.06(3041 lines)  AV=2.97  SV=6.63  BV=-0.60
    {119992,5504,1040, 0, 0, 0},  //TV = 3.06(3041 lines)  AV=2.97  SV=6.74  BV=-0.71
    {119992,5888,1040, 0, 0, 0},  //TV = 3.06(3041 lines)  AV=2.97  SV=6.83  BV=-0.80
    {119992,6144,1064, 0, 0, 0},  //TV = 3.06(3041 lines)  AV=2.97  SV=6.93  BV=-0.90
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
},
};

static strAETable g_AE_StrobeTable =
{
    AETABLE_STROBE,    //eAETableID
    119,    //u4TotalIndex
    20,    //u4StrobeTrigerBV
    109,    //i4MaxBV
    -9,    //i4MinBV
    90,    //i4EffectiveMaxBV
    0,      //i4EffectiveMinBV
    LIB3A_AE_ISO_SPEED_AUTO, //ISO SPEED
    sCaptureStrobePLineTable_60Hz,
    sCaptureStrobePLineTable_50Hz,
    NULL,
};

static strEvPline sAEScene1PLineTable_60Hz =
{
{
    {198,1140,1024, 0, 0, 0},  //TV = 12.30(5 lines)  AV=2.97  SV=4.44  BV=10.83
    {198,1140,1040, 0, 0, 0},  //TV = 12.30(5 lines)  AV=2.97  SV=4.46  BV=10.81
    {198,1216,1048, 0, 0, 0},  //TV = 12.30(5 lines)  AV=2.97  SV=4.57  BV=10.70
    {198,1328,1024, 0, 0, 0},  //TV = 12.30(5 lines)  AV=2.97  SV=4.66  BV=10.61
    {237,1140,1064, 0, 0, 0},  //TV = 12.04(6 lines)  AV=2.97  SV=4.50  BV=10.52
    {237,1216,1072, 0, 0, 0},  //TV = 12.04(6 lines)  AV=2.97  SV=4.60  BV=10.41
    {277,1140,1056, 0, 0, 0},  //TV = 11.82(7 lines)  AV=2.97  SV=4.49  BV=10.30
    {277,1216,1056, 0, 0, 0},  //TV = 11.82(7 lines)  AV=2.97  SV=4.58  BV=10.21
    {316,1140,1056, 0, 0, 0},  //TV = 11.63(8 lines)  AV=2.97  SV=4.49  BV=10.11
    {316,1216,1064, 0, 0, 0},  //TV = 11.63(8 lines)  AV=2.97  SV=4.59  BV=10.01
    {356,1140,1080, 0, 0, 0},  //TV = 11.46(9 lines)  AV=2.97  SV=4.52  BV=9.91
    {395,1140,1040, 0, 0, 0},  //TV = 11.31(10 lines)  AV=2.97  SV=4.46  BV=9.81
    {435,1140,1024, 0, 0, 0},  //TV = 11.17(11 lines)  AV=2.97  SV=4.44  BV=9.70
    {435,1140,1088, 0, 0, 0},  //TV = 11.17(11 lines)  AV=2.97  SV=4.53  BV=9.61
    {474,1140,1064, 0, 0, 0},  //TV = 11.04(12 lines)  AV=2.97  SV=4.50  BV=9.52
    {513,1140,1056, 0, 0, 0},  //TV = 10.93(13 lines)  AV=2.97  SV=4.49  BV=9.41
    {553,1140,1056, 0, 0, 0},  //TV = 10.82(14 lines)  AV=2.97  SV=4.49  BV=9.31
    {592,1140,1056, 0, 0, 0},  //TV = 10.72(15 lines)  AV=2.97  SV=4.49  BV=9.21
    {632,1140,1056, 0, 0, 0},  //TV = 10.63(16 lines)  AV=2.97  SV=4.49  BV=9.11
    {671,1140,1064, 0, 0, 0},  //TV = 10.54(17 lines)  AV=2.97  SV=4.50  BV=9.02
    {750,1140,1024, 0, 0, 0},  //TV = 10.38(19 lines)  AV=2.97  SV=4.44  BV=8.91
    {790,1140,1040, 0, 0, 0},  //TV = 10.31(20 lines)  AV=2.97  SV=4.46  BV=8.81
    {869,1140,1024, 0, 0, 0},  //TV = 10.17(22 lines)  AV=2.97  SV=4.44  BV=8.70
    {908,1140,1040, 0, 0, 0},  //TV = 10.11(23 lines)  AV=2.97  SV=4.46  BV=8.61
    {987,1140,1024, 0, 0, 0},  //TV = 9.98(25 lines)  AV=2.97  SV=4.44  BV=8.51
    {1066,1140,1024, 0, 0, 0},  //TV = 9.87(27 lines)  AV=2.97  SV=4.44  BV=8.40
    {1105,1140,1056, 0, 0, 0},  //TV = 9.82(28 lines)  AV=2.97  SV=4.49  BV=8.31
    {1224,1140,1024, 0, 0, 0},  //TV = 9.67(31 lines)  AV=2.97  SV=4.44  BV=8.20
    {1303,1140,1024, 0, 0, 0},  //TV = 9.58(33 lines)  AV=2.97  SV=4.44  BV=8.11
    {1382,1140,1040, 0, 0, 0},  //TV = 9.50(35 lines)  AV=2.97  SV=4.46  BV=8.01
    {1500,1140,1024, 0, 0, 0},  //TV = 9.38(38 lines)  AV=2.97  SV=4.44  BV=7.91
    {1618,1140,1024, 0, 0, 0},  //TV = 9.27(41 lines)  AV=2.97  SV=4.44  BV=7.80
    {1697,1140,1040, 0, 0, 0},  //TV = 9.20(43 lines)  AV=2.97  SV=4.46  BV=7.71
    {1855,1140,1024, 0, 0, 0},  //TV = 9.07(47 lines)  AV=2.97  SV=4.44  BV=7.60
    {1973,1140,1024, 0, 0, 0},  //TV = 8.99(50 lines)  AV=2.97  SV=4.44  BV=7.51
    {2131,1140,1024, 0, 0, 0},  //TV = 8.87(54 lines)  AV=2.97  SV=4.44  BV=7.40
    {2250,1140,1032, 0, 0, 0},  //TV = 8.80(57 lines)  AV=2.97  SV=4.45  BV=7.31
    {2447,1140,1024, 0, 0, 0},  //TV = 8.67(62 lines)  AV=2.97  SV=4.44  BV=7.20
    {2605,1140,1024, 0, 0, 0},  //TV = 8.58(66 lines)  AV=2.97  SV=4.44  BV=7.11
    {2802,1140,1024, 0, 0, 0},  //TV = 8.48(71 lines)  AV=2.97  SV=4.44  BV=7.01
    {2999,1140,1024, 0, 0, 0},  //TV = 8.38(76 lines)  AV=2.97  SV=4.44  BV=6.91
    {3236,1140,1024, 0, 0, 0},  //TV = 8.27(82 lines)  AV=2.97  SV=4.44  BV=6.80
    {3473,1140,1024, 0, 0, 0},  //TV = 8.17(88 lines)  AV=2.97  SV=4.44  BV=6.70
    {3710,1140,1024, 0, 0, 0},  //TV = 8.07(94 lines)  AV=2.97  SV=4.44  BV=6.60
    {3986,1140,1024, 0, 0, 0},  //TV = 7.97(101 lines)  AV=2.97  SV=4.44  BV=6.50
    {4262,1140,1024, 0, 0, 0},  //TV = 7.87(108 lines)  AV=2.97  SV=4.44  BV=6.40
    {4538,1140,1024, 0, 0, 0},  //TV = 7.78(115 lines)  AV=2.97  SV=4.44  BV=6.31
    {4893,1140,1024, 0, 0, 0},  //TV = 7.68(124 lines)  AV=2.97  SV=4.44  BV=6.20
    {5209,1140,1024, 0, 0, 0},  //TV = 7.58(132 lines)  AV=2.97  SV=4.44  BV=6.11
    {5643,1140,1024, 0, 0, 0},  //TV = 7.47(143 lines)  AV=2.97  SV=4.44  BV=6.00
    {6038,1140,1024, 0, 0, 0},  //TV = 7.37(153 lines)  AV=2.97  SV=4.44  BV=5.90
    {6472,1140,1024, 0, 0, 0},  //TV = 7.27(164 lines)  AV=2.97  SV=4.44  BV=5.80
    {6906,1140,1024, 0, 0, 0},  //TV = 7.18(175 lines)  AV=2.97  SV=4.44  BV=5.71
    {7419,1140,1024, 0, 0, 0},  //TV = 7.07(188 lines)  AV=2.97  SV=4.44  BV=5.60
    {7971,1140,1024, 0, 0, 0},  //TV = 6.97(202 lines)  AV=2.97  SV=4.44  BV=5.50
    {8326,1140,1048, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=4.48  BV=5.40
    {8326,1216,1048, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=4.57  BV=5.31
    {8326,1328,1024, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=4.66  BV=5.22
    {8326,1424,1024, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=4.76  BV=5.12
    {8326,1536,1024, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=4.87  BV=5.01
    {8326,1632,1032, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=4.97  BV=4.91
    {8326,1728,1040, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=5.06  BV=4.81
    {8326,1840,1048, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=5.17  BV=4.71
    {8326,1936,1072, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=5.27  BV=4.61
    {8326,2144,1032, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=5.36  BV=4.51
    {16652,1140,1048, 0, 0, 0},  //TV = 5.91(422 lines)  AV=2.97  SV=4.48  BV=4.40
    {16652,1216,1048, 0, 0, 0},  //TV = 5.91(422 lines)  AV=2.97  SV=4.57  BV=4.31
    {16652,1328,1024, 0, 0, 0},  //TV = 5.91(422 lines)  AV=2.97  SV=4.66  BV=4.22
    {16652,1424,1024, 0, 0, 0},  //TV = 5.91(422 lines)  AV=2.97  SV=4.76  BV=4.12
    {16652,1424,1104, 0, 0, 0},  //TV = 5.91(422 lines)  AV=2.97  SV=4.87  BV=4.01
    {16652,1632,1032, 0, 0, 0},  //TV = 5.91(422 lines)  AV=2.97  SV=4.97  BV=3.91
    {25017,1140,1056, 0, 0, 0},  //TV = 5.32(634 lines)  AV=2.97  SV=4.49  BV=3.81
    {25017,1216,1064, 0, 0, 0},  //TV = 5.32(634 lines)  AV=2.97  SV=4.59  BV=3.70
    {25017,1328,1048, 0, 0, 0},  //TV = 5.32(634 lines)  AV=2.97  SV=4.70  BV=3.60
    {25017,1424,1040, 0, 0, 0},  //TV = 5.32(634 lines)  AV=2.97  SV=4.78  BV=3.51
    {33343,1140,1048, 0, 0, 0},  //TV = 4.91(845 lines)  AV=2.97  SV=4.48  BV=3.40
    {33343,1216,1048, 0, 0, 0},  //TV = 4.91(845 lines)  AV=2.97  SV=4.57  BV=3.31
    {33343,1328,1024, 0, 0, 0},  //TV = 4.91(845 lines)  AV=2.97  SV=4.66  BV=3.22
    {41669,1140,1024, 0, 0, 0},  //TV = 4.58(1056 lines)  AV=2.97  SV=4.44  BV=3.11
    {41669,1216,1032, 0, 0, 0},  //TV = 4.58(1056 lines)  AV=2.97  SV=4.55  BV=3.01
    {41669,1216,1104, 0, 0, 0},  //TV = 4.58(1056 lines)  AV=2.97  SV=4.64  BV=2.91
    {49994,1140,1056, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.49  BV=2.81
    {49994,1216,1064, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.59  BV=2.70
    {49994,1328,1040, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.68  BV=2.61
    {49994,1424,1040, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.78  BV=2.51
    {49994,1536,1032, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.88  BV=2.41
    {49994,1632,1040, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.98  BV=2.31
    {49994,1728,1056, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.09  BV=2.21
    {49994,1840,1064, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.19  BV=2.11
    {49994,2048,1024, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.29  BV=2.01
    {49994,2144,1048, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.39  BV=1.91
    {49994,2240,1072, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.48  BV=1.81
    {49994,2448,1048, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.58  BV=1.72
    {49994,2656,1040, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.68  BV=1.61
    {49994,2864,1032, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.78  BV=1.51
    {49994,3072,1032, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.88  BV=1.41
    {49994,3328,1024, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.99  BV=1.31
    {49994,3456,1056, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=6.09  BV=1.21
    {58319,3200,1048, 0, 0, 0},  //TV = 4.10(1478 lines)  AV=2.97  SV=5.96  BV=1.11
    {66645,3072,1024, 0, 0, 0},  //TV = 3.91(1689 lines)  AV=2.97  SV=5.87  BV=1.01
    {66645,3200,1056, 0, 0, 0},  //TV = 3.91(1689 lines)  AV=2.97  SV=5.98  BV=0.90
    {75011,3072,1048, 0, 0, 0},  //TV = 3.74(1901 lines)  AV=2.97  SV=5.91  BV=0.80
    {75011,3328,1032, 0, 0, 0},  //TV = 3.74(1901 lines)  AV=2.97  SV=6.00  BV=0.71
    {83336,3200,1032, 0, 0, 0},  //TV = 3.58(2112 lines)  AV=2.97  SV=5.94  BV=0.61
    {83336,3456,1024, 0, 0, 0},  //TV = 3.58(2112 lines)  AV=2.97  SV=6.04  BV=0.51
    {83336,3712,1024, 0, 0, 0},  //TV = 3.58(2112 lines)  AV=2.97  SV=6.14  BV=0.41
    {83336,3968,1032, 0, 0, 0},  //TV = 3.58(2112 lines)  AV=2.97  SV=6.25  BV=0.30
    {83336,4224,1032, 0, 0, 0},  //TV = 3.58(2112 lines)  AV=2.97  SV=6.34  BV=0.21
    {91661,4096,1040, 0, 0, 0},  //TV = 3.45(2323 lines)  AV=2.97  SV=6.31  BV=0.11
    {99987,4096,1032, 0, 0, 0},  //TV = 3.32(2534 lines)  AV=2.97  SV=6.30  BV=-0.01
    {108313,4096,1024, 0, 0, 0},  //TV = 3.21(2745 lines)  AV=2.97  SV=6.29  BV=-0.11
    {108313,4352,1032, 0, 0, 0},  //TV = 3.21(2745 lines)  AV=2.97  SV=6.39  BV=-0.21
    {116678,4224,1048, 0, 0, 0},  //TV = 3.10(2957 lines)  AV=2.97  SV=6.36  BV=-0.29
    {125004,4224,1048, 0, 0, 0},  //TV = 3.00(3168 lines)  AV=2.97  SV=6.36  BV=-0.39
    {125004,4608,1032, 0, 0, 0},  //TV = 3.00(3168 lines)  AV=2.97  SV=6.47  BV=-0.50
    {125004,4864,1048, 0, 0, 0},  //TV = 3.00(3168 lines)  AV=2.97  SV=6.57  BV=-0.60
    {125004,5376,1024, 0, 0, 0},  //TV = 3.00(3168 lines)  AV=2.97  SV=6.68  BV=-0.71
    {125004,5632,1040, 0, 0, 0},  //TV = 3.00(3168 lines)  AV=2.97  SV=6.77  BV=-0.80
    {125004,6016,1040, 0, 0, 0},  //TV = 3.00(3168 lines)  AV=2.97  SV=6.86  BV=-0.89
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
},
};

static strEvPline sAEScene1PLineTable_50Hz =
{
{
    {198,1140,1024, 0, 0, 0},  //TV = 12.30(5 lines)  AV=2.97  SV=4.44  BV=10.83
    {198,1140,1040, 0, 0, 0},  //TV = 12.30(5 lines)  AV=2.97  SV=4.46  BV=10.81
    {198,1216,1048, 0, 0, 0},  //TV = 12.30(5 lines)  AV=2.97  SV=4.57  BV=10.70
    {198,1328,1024, 0, 0, 0},  //TV = 12.30(5 lines)  AV=2.97  SV=4.66  BV=10.61
    {237,1140,1064, 0, 0, 0},  //TV = 12.04(6 lines)  AV=2.97  SV=4.50  BV=10.52
    {237,1216,1072, 0, 0, 0},  //TV = 12.04(6 lines)  AV=2.97  SV=4.60  BV=10.41
    {277,1140,1056, 0, 0, 0},  //TV = 11.82(7 lines)  AV=2.97  SV=4.49  BV=10.30
    {277,1216,1056, 0, 0, 0},  //TV = 11.82(7 lines)  AV=2.97  SV=4.58  BV=10.21
    {316,1140,1056, 0, 0, 0},  //TV = 11.63(8 lines)  AV=2.97  SV=4.49  BV=10.11
    {316,1216,1064, 0, 0, 0},  //TV = 11.63(8 lines)  AV=2.97  SV=4.59  BV=10.01
    {356,1140,1080, 0, 0, 0},  //TV = 11.46(9 lines)  AV=2.97  SV=4.52  BV=9.91
    {395,1140,1040, 0, 0, 0},  //TV = 11.31(10 lines)  AV=2.97  SV=4.46  BV=9.81
    {435,1140,1024, 0, 0, 0},  //TV = 11.17(11 lines)  AV=2.97  SV=4.44  BV=9.70
    {435,1140,1088, 0, 0, 0},  //TV = 11.17(11 lines)  AV=2.97  SV=4.53  BV=9.61
    {474,1140,1064, 0, 0, 0},  //TV = 11.04(12 lines)  AV=2.97  SV=4.50  BV=9.52
    {513,1140,1056, 0, 0, 0},  //TV = 10.93(13 lines)  AV=2.97  SV=4.49  BV=9.41
    {553,1140,1056, 0, 0, 0},  //TV = 10.82(14 lines)  AV=2.97  SV=4.49  BV=9.31
    {592,1140,1056, 0, 0, 0},  //TV = 10.72(15 lines)  AV=2.97  SV=4.49  BV=9.21
    {632,1140,1056, 0, 0, 0},  //TV = 10.63(16 lines)  AV=2.97  SV=4.49  BV=9.11
    {671,1140,1064, 0, 0, 0},  //TV = 10.54(17 lines)  AV=2.97  SV=4.50  BV=9.02
    {750,1140,1024, 0, 0, 0},  //TV = 10.38(19 lines)  AV=2.97  SV=4.44  BV=8.91
    {790,1140,1040, 0, 0, 0},  //TV = 10.31(20 lines)  AV=2.97  SV=4.46  BV=8.81
    {869,1140,1024, 0, 0, 0},  //TV = 10.17(22 lines)  AV=2.97  SV=4.44  BV=8.70
    {908,1140,1040, 0, 0, 0},  //TV = 10.11(23 lines)  AV=2.97  SV=4.46  BV=8.61
    {987,1140,1024, 0, 0, 0},  //TV = 9.98(25 lines)  AV=2.97  SV=4.44  BV=8.51
    {1066,1140,1024, 0, 0, 0},  //TV = 9.87(27 lines)  AV=2.97  SV=4.44  BV=8.40
    {1105,1140,1056, 0, 0, 0},  //TV = 9.82(28 lines)  AV=2.97  SV=4.49  BV=8.31
    {1224,1140,1024, 0, 0, 0},  //TV = 9.67(31 lines)  AV=2.97  SV=4.44  BV=8.20
    {1303,1140,1024, 0, 0, 0},  //TV = 9.58(33 lines)  AV=2.97  SV=4.44  BV=8.11
    {1382,1140,1040, 0, 0, 0},  //TV = 9.50(35 lines)  AV=2.97  SV=4.46  BV=8.01
    {1500,1140,1024, 0, 0, 0},  //TV = 9.38(38 lines)  AV=2.97  SV=4.44  BV=7.91
    {1618,1140,1024, 0, 0, 0},  //TV = 9.27(41 lines)  AV=2.97  SV=4.44  BV=7.80
    {1697,1140,1040, 0, 0, 0},  //TV = 9.20(43 lines)  AV=2.97  SV=4.46  BV=7.71
    {1855,1140,1024, 0, 0, 0},  //TV = 9.07(47 lines)  AV=2.97  SV=4.44  BV=7.60
    {1973,1140,1024, 0, 0, 0},  //TV = 8.99(50 lines)  AV=2.97  SV=4.44  BV=7.51
    {2131,1140,1024, 0, 0, 0},  //TV = 8.87(54 lines)  AV=2.97  SV=4.44  BV=7.40
    {2250,1140,1032, 0, 0, 0},  //TV = 8.80(57 lines)  AV=2.97  SV=4.45  BV=7.31
    {2447,1140,1024, 0, 0, 0},  //TV = 8.67(62 lines)  AV=2.97  SV=4.44  BV=7.20
    {2605,1140,1024, 0, 0, 0},  //TV = 8.58(66 lines)  AV=2.97  SV=4.44  BV=7.11
    {2802,1140,1024, 0, 0, 0},  //TV = 8.48(71 lines)  AV=2.97  SV=4.44  BV=7.01
    {2999,1140,1024, 0, 0, 0},  //TV = 8.38(76 lines)  AV=2.97  SV=4.44  BV=6.91
    {3236,1140,1024, 0, 0, 0},  //TV = 8.27(82 lines)  AV=2.97  SV=4.44  BV=6.80
    {3473,1140,1024, 0, 0, 0},  //TV = 8.17(88 lines)  AV=2.97  SV=4.44  BV=6.70
    {3710,1140,1024, 0, 0, 0},  //TV = 8.07(94 lines)  AV=2.97  SV=4.44  BV=6.60
    {3986,1140,1024, 0, 0, 0},  //TV = 7.97(101 lines)  AV=2.97  SV=4.44  BV=6.50
    {4262,1140,1024, 0, 0, 0},  //TV = 7.87(108 lines)  AV=2.97  SV=4.44  BV=6.40
    {4538,1140,1024, 0, 0, 0},  //TV = 7.78(115 lines)  AV=2.97  SV=4.44  BV=6.31
    {4893,1140,1024, 0, 0, 0},  //TV = 7.68(124 lines)  AV=2.97  SV=4.44  BV=6.20
    {5209,1140,1024, 0, 0, 0},  //TV = 7.58(132 lines)  AV=2.97  SV=4.44  BV=6.11
    {5643,1140,1024, 0, 0, 0},  //TV = 7.47(143 lines)  AV=2.97  SV=4.44  BV=6.00
    {6038,1140,1024, 0, 0, 0},  //TV = 7.37(153 lines)  AV=2.97  SV=4.44  BV=5.90
    {6472,1140,1024, 0, 0, 0},  //TV = 7.27(164 lines)  AV=2.97  SV=4.44  BV=5.80
    {6906,1140,1024, 0, 0, 0},  //TV = 7.18(175 lines)  AV=2.97  SV=4.44  BV=5.71
    {7419,1140,1024, 0, 0, 0},  //TV = 7.07(188 lines)  AV=2.97  SV=4.44  BV=5.60
    {7971,1140,1024, 0, 0, 0},  //TV = 6.97(202 lines)  AV=2.97  SV=4.44  BV=5.50
    {8484,1140,1024, 0, 0, 0},  //TV = 6.88(215 lines)  AV=2.97  SV=4.44  BV=5.41
    {9155,1140,1024, 0, 0, 0},  //TV = 6.77(232 lines)  AV=2.97  SV=4.44  BV=5.30
    {9826,1140,1024, 0, 0, 0},  //TV = 6.67(249 lines)  AV=2.97  SV=4.44  BV=5.20
    {9983,1140,1072, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=4.51  BV=5.11
    {9983,1216,1072, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=4.60  BV=5.02
    {9983,1328,1056, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=4.71  BV=4.91
    {9983,1424,1056, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=4.81  BV=4.81
    {9983,1536,1048, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=4.91  BV=4.71
    {9983,1632,1064, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=5.01  BV=4.60
    {9983,1728,1072, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=5.11  BV=4.51
    {9983,1936,1024, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=5.21  BV=4.41
    {9983,2048,1040, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=5.31  BV=4.31
    {9983,2144,1064, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=5.41  BV=4.21
    {20006,1140,1064, 0, 0, 0},  //TV = 5.64(507 lines)  AV=2.97  SV=4.50  BV=4.12
    {20006,1216,1072, 0, 0, 0},  //TV = 5.64(507 lines)  AV=2.97  SV=4.60  BV=4.01
    {20006,1328,1048, 0, 0, 0},  //TV = 5.64(507 lines)  AV=2.97  SV=4.70  BV=3.92
    {20006,1424,1064, 0, 0, 0},  //TV = 5.64(507 lines)  AV=2.97  SV=4.82  BV=3.80
    {20006,1536,1048, 0, 0, 0},  //TV = 5.64(507 lines)  AV=2.97  SV=4.91  BV=3.71
    {20006,1632,1056, 0, 0, 0},  //TV = 5.64(507 lines)  AV=2.97  SV=5.00  BV=3.61
    {29989,1140,1080, 0, 0, 0},  //TV = 5.06(760 lines)  AV=2.97  SV=4.52  BV=3.51
    {29989,1216,1096, 0, 0, 0},  //TV = 5.06(760 lines)  AV=2.97  SV=4.63  BV=3.40
    {29989,1328,1064, 0, 0, 0},  //TV = 5.06(760 lines)  AV=2.97  SV=4.72  BV=3.31
    {29989,1424,1072, 0, 0, 0},  //TV = 5.06(760 lines)  AV=2.97  SV=4.83  BV=3.20
    {40011,1140,1064, 0, 0, 0},  //TV = 4.64(1014 lines)  AV=2.97  SV=4.50  BV=3.12
    {40011,1216,1072, 0, 0, 0},  //TV = 4.64(1014 lines)  AV=2.97  SV=4.60  BV=3.01
    {40011,1328,1048, 0, 0, 0},  //TV = 4.64(1014 lines)  AV=2.97  SV=4.70  BV=2.92
    {49994,1140,1056, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.49  BV=2.81
    {49994,1216,1064, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.59  BV=2.70
    {49994,1328,1040, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.68  BV=2.61
    {49994,1424,1040, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.78  BV=2.51
    {49994,1536,1032, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.88  BV=2.41
    {49994,1632,1040, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.98  BV=2.31
    {49994,1728,1056, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.09  BV=2.21
    {49994,1840,1064, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.19  BV=2.11
    {49994,2048,1024, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.29  BV=2.01
    {49994,2144,1048, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.39  BV=1.91
    {49994,2240,1072, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.48  BV=1.81
    {49994,2448,1048, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.58  BV=1.72
    {49994,2656,1040, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.68  BV=1.61
    {49994,2864,1032, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.78  BV=1.51
    {49994,3072,1032, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.88  BV=1.41
    {49994,3328,1024, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.99  BV=1.31
    {49994,3456,1056, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=6.09  BV=1.21
    {60017,3072,1064, 0, 0, 0},  //TV = 4.06(1521 lines)  AV=2.97  SV=5.93  BV=1.10
    {60017,3328,1048, 0, 0, 0},  //TV = 4.06(1521 lines)  AV=2.97  SV=6.02  BV=1.01
    {69999,3072,1048, 0, 0, 0},  //TV = 3.84(1774 lines)  AV=2.97  SV=5.91  BV=0.90
    {69999,3328,1032, 0, 0, 0},  //TV = 3.84(1774 lines)  AV=2.97  SV=6.00  BV=0.81
    {79982,3072,1048, 0, 0, 0},  //TV = 3.64(2027 lines)  AV=2.97  SV=5.91  BV=0.71
    {79982,3328,1032, 0, 0, 0},  //TV = 3.64(2027 lines)  AV=2.97  SV=6.00  BV=0.62
    {79982,3584,1032, 0, 0, 0},  //TV = 3.64(2027 lines)  AV=2.97  SV=6.11  BV=0.51
    {79982,3840,1032, 0, 0, 0},  //TV = 3.64(2027 lines)  AV=2.97  SV=6.20  BV=0.41
    {79982,4096,1040, 0, 0, 0},  //TV = 3.64(2027 lines)  AV=2.97  SV=6.31  BV=0.31
    {79982,4352,1040, 0, 0, 0},  //TV = 3.64(2027 lines)  AV=2.97  SV=6.40  BV=0.22
    {90005,4224,1024, 0, 0, 0},  //TV = 3.47(2281 lines)  AV=2.97  SV=6.33  BV=0.11
    {99987,4096,1032, 0, 0, 0},  //TV = 3.32(2534 lines)  AV=2.97  SV=6.30  BV=-0.01
    {99987,4352,1040, 0, 0, 0},  //TV = 3.32(2534 lines)  AV=2.97  SV=6.40  BV=-0.10
    {110010,4224,1040, 0, 0, 0},  //TV = 3.18(2788 lines)  AV=2.97  SV=6.35  BV=-0.20
    {119992,4096,1056, 0, 0, 0},  //TV = 3.06(3041 lines)  AV=2.97  SV=6.33  BV=-0.30
    {119992,4480,1032, 0, 0, 0},  //TV = 3.06(3041 lines)  AV=2.97  SV=6.43  BV=-0.40
    {119992,4736,1048, 0, 0, 0},  //TV = 3.06(3041 lines)  AV=2.97  SV=6.53  BV=-0.50
    {119992,5120,1040, 0, 0, 0},  //TV = 3.06(3041 lines)  AV=2.97  SV=6.63  BV=-0.60
    {119992,5504,1040, 0, 0, 0},  //TV = 3.06(3041 lines)  AV=2.97  SV=6.74  BV=-0.71
    {119992,5888,1040, 0, 0, 0},  //TV = 3.06(3041 lines)  AV=2.97  SV=6.83  BV=-0.80
    {119992,6144,1064, 0, 0, 0},  //TV = 3.06(3041 lines)  AV=2.97  SV=6.93  BV=-0.90
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
},
};

static strAETable g_AE_SceneTable1 =
{
    AETABLE_SCENE_INDEX1,    //eAETableID
    119,    //u4TotalIndex
    20,    //u4StrobeTrigerBV
    109,    //i4MaxBV
    -9,    //i4MinBV
    90,    //i4EffectiveMaxBV
    -10,      //i4EffectiveMinBV
    LIB3A_AE_ISO_SPEED_AUTO, //ISO SPEED
    sAEScene1PLineTable_60Hz,
    sAEScene1PLineTable_50Hz,
    NULL,
};

static strEvPline sAEScene2PLineTable_60Hz =
{
{
    {198,1140,1024, 0, 0, 0},  //TV = 12.30(5 lines)  AV=2.97  SV=4.44  BV=10.83
    {198,1140,1040, 0, 0, 0},  //TV = 12.30(5 lines)  AV=2.97  SV=4.46  BV=10.81
    {198,1216,1048, 0, 0, 0},  //TV = 12.30(5 lines)  AV=2.97  SV=4.57  BV=10.70
    {198,1328,1024, 0, 0, 0},  //TV = 12.30(5 lines)  AV=2.97  SV=4.66  BV=10.61
    {237,1140,1064, 0, 0, 0},  //TV = 12.04(6 lines)  AV=2.97  SV=4.50  BV=10.52
    {237,1216,1072, 0, 0, 0},  //TV = 12.04(6 lines)  AV=2.97  SV=4.60  BV=10.41
    {277,1140,1056, 0, 0, 0},  //TV = 11.82(7 lines)  AV=2.97  SV=4.49  BV=10.30
    {277,1216,1056, 0, 0, 0},  //TV = 11.82(7 lines)  AV=2.97  SV=4.58  BV=10.21
    {316,1140,1056, 0, 0, 0},  //TV = 11.63(8 lines)  AV=2.97  SV=4.49  BV=10.11
    {316,1216,1064, 0, 0, 0},  //TV = 11.63(8 lines)  AV=2.97  SV=4.59  BV=10.01
    {356,1140,1080, 0, 0, 0},  //TV = 11.46(9 lines)  AV=2.97  SV=4.52  BV=9.91
    {395,1140,1040, 0, 0, 0},  //TV = 11.31(10 lines)  AV=2.97  SV=4.46  BV=9.81
    {435,1140,1024, 0, 0, 0},  //TV = 11.17(11 lines)  AV=2.97  SV=4.44  BV=9.70
    {435,1140,1088, 0, 0, 0},  //TV = 11.17(11 lines)  AV=2.97  SV=4.53  BV=9.61
    {474,1140,1064, 0, 0, 0},  //TV = 11.04(12 lines)  AV=2.97  SV=4.50  BV=9.52
    {513,1140,1056, 0, 0, 0},  //TV = 10.93(13 lines)  AV=2.97  SV=4.49  BV=9.41
    {553,1140,1056, 0, 0, 0},  //TV = 10.82(14 lines)  AV=2.97  SV=4.49  BV=9.31
    {592,1140,1056, 0, 0, 0},  //TV = 10.72(15 lines)  AV=2.97  SV=4.49  BV=9.21
    {632,1140,1056, 0, 0, 0},  //TV = 10.63(16 lines)  AV=2.97  SV=4.49  BV=9.11
    {671,1140,1064, 0, 0, 0},  //TV = 10.54(17 lines)  AV=2.97  SV=4.50  BV=9.02
    {750,1140,1024, 0, 0, 0},  //TV = 10.38(19 lines)  AV=2.97  SV=4.44  BV=8.91
    {790,1140,1040, 0, 0, 0},  //TV = 10.31(20 lines)  AV=2.97  SV=4.46  BV=8.81
    {869,1140,1024, 0, 0, 0},  //TV = 10.17(22 lines)  AV=2.97  SV=4.44  BV=8.70
    {908,1140,1040, 0, 0, 0},  //TV = 10.11(23 lines)  AV=2.97  SV=4.46  BV=8.61
    {987,1140,1024, 0, 0, 0},  //TV = 9.98(25 lines)  AV=2.97  SV=4.44  BV=8.51
    {1066,1140,1024, 0, 0, 0},  //TV = 9.87(27 lines)  AV=2.97  SV=4.44  BV=8.40
    {1105,1140,1056, 0, 0, 0},  //TV = 9.82(28 lines)  AV=2.97  SV=4.49  BV=8.31
    {1224,1140,1024, 0, 0, 0},  //TV = 9.67(31 lines)  AV=2.97  SV=4.44  BV=8.20
    {1303,1140,1024, 0, 0, 0},  //TV = 9.58(33 lines)  AV=2.97  SV=4.44  BV=8.11
    {1382,1140,1040, 0, 0, 0},  //TV = 9.50(35 lines)  AV=2.97  SV=4.46  BV=8.01
    {1500,1140,1024, 0, 0, 0},  //TV = 9.38(38 lines)  AV=2.97  SV=4.44  BV=7.91
    {1618,1140,1024, 0, 0, 0},  //TV = 9.27(41 lines)  AV=2.97  SV=4.44  BV=7.80
    {1697,1140,1040, 0, 0, 0},  //TV = 9.20(43 lines)  AV=2.97  SV=4.46  BV=7.71
    {1855,1140,1024, 0, 0, 0},  //TV = 9.07(47 lines)  AV=2.97  SV=4.44  BV=7.60
    {1973,1140,1024, 0, 0, 0},  //TV = 8.99(50 lines)  AV=2.97  SV=4.44  BV=7.51
    {2131,1140,1024, 0, 0, 0},  //TV = 8.87(54 lines)  AV=2.97  SV=4.44  BV=7.40
    {2250,1140,1032, 0, 0, 0},  //TV = 8.80(57 lines)  AV=2.97  SV=4.45  BV=7.31
    {2447,1140,1024, 0, 0, 0},  //TV = 8.67(62 lines)  AV=2.97  SV=4.44  BV=7.20
    {2605,1140,1024, 0, 0, 0},  //TV = 8.58(66 lines)  AV=2.97  SV=4.44  BV=7.11
    {2802,1140,1024, 0, 0, 0},  //TV = 8.48(71 lines)  AV=2.97  SV=4.44  BV=7.01
    {2999,1140,1024, 0, 0, 0},  //TV = 8.38(76 lines)  AV=2.97  SV=4.44  BV=6.91
    {3236,1140,1024, 0, 0, 0},  //TV = 8.27(82 lines)  AV=2.97  SV=4.44  BV=6.80
    {3473,1140,1024, 0, 0, 0},  //TV = 8.17(88 lines)  AV=2.97  SV=4.44  BV=6.70
    {3710,1140,1024, 0, 0, 0},  //TV = 8.07(94 lines)  AV=2.97  SV=4.44  BV=6.60
    {3986,1140,1024, 0, 0, 0},  //TV = 7.97(101 lines)  AV=2.97  SV=4.44  BV=6.50
    {4262,1140,1024, 0, 0, 0},  //TV = 7.87(108 lines)  AV=2.97  SV=4.44  BV=6.40
    {4538,1140,1024, 0, 0, 0},  //TV = 7.78(115 lines)  AV=2.97  SV=4.44  BV=6.31
    {4893,1140,1024, 0, 0, 0},  //TV = 7.68(124 lines)  AV=2.97  SV=4.44  BV=6.20
    {5209,1140,1024, 0, 0, 0},  //TV = 7.58(132 lines)  AV=2.97  SV=4.44  BV=6.11
    {5643,1140,1024, 0, 0, 0},  //TV = 7.47(143 lines)  AV=2.97  SV=4.44  BV=6.00
    {6038,1140,1024, 0, 0, 0},  //TV = 7.37(153 lines)  AV=2.97  SV=4.44  BV=5.90
    {6472,1140,1024, 0, 0, 0},  //TV = 7.27(164 lines)  AV=2.97  SV=4.44  BV=5.80
    {6906,1140,1024, 0, 0, 0},  //TV = 7.18(175 lines)  AV=2.97  SV=4.44  BV=5.71
    {7419,1140,1024, 0, 0, 0},  //TV = 7.07(188 lines)  AV=2.97  SV=4.44  BV=5.60
    {7971,1140,1024, 0, 0, 0},  //TV = 6.97(202 lines)  AV=2.97  SV=4.44  BV=5.50
    {8326,1140,1048, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=4.48  BV=5.40
    {8326,1216,1048, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=4.57  BV=5.31
    {8326,1328,1024, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=4.66  BV=5.22
    {8326,1424,1024, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=4.76  BV=5.12
    {8326,1536,1024, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=4.87  BV=5.01
    {8326,1632,1032, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=4.97  BV=4.91
    {8326,1728,1040, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=5.06  BV=4.81
    {8326,1840,1048, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=5.17  BV=4.71
    {8326,1936,1072, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=5.27  BV=4.61
    {8326,2144,1032, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=5.36  BV=4.51
    {16652,1140,1048, 0, 0, 0},  //TV = 5.91(422 lines)  AV=2.97  SV=4.48  BV=4.40
    {16652,1216,1048, 0, 0, 0},  //TV = 5.91(422 lines)  AV=2.97  SV=4.57  BV=4.31
    {16652,1328,1024, 0, 0, 0},  //TV = 5.91(422 lines)  AV=2.97  SV=4.66  BV=4.22
    {16652,1424,1024, 0, 0, 0},  //TV = 5.91(422 lines)  AV=2.97  SV=4.76  BV=4.12
    {16652,1424,1104, 0, 0, 0},  //TV = 5.91(422 lines)  AV=2.97  SV=4.87  BV=4.01
    {16652,1632,1032, 0, 0, 0},  //TV = 5.91(422 lines)  AV=2.97  SV=4.97  BV=3.91
    {25017,1140,1056, 0, 0, 0},  //TV = 5.32(634 lines)  AV=2.97  SV=4.49  BV=3.81
    {25017,1216,1064, 0, 0, 0},  //TV = 5.32(634 lines)  AV=2.97  SV=4.59  BV=3.70
    {25017,1328,1048, 0, 0, 0},  //TV = 5.32(634 lines)  AV=2.97  SV=4.70  BV=3.60
    {25017,1424,1040, 0, 0, 0},  //TV = 5.32(634 lines)  AV=2.97  SV=4.78  BV=3.51
    {33343,1140,1048, 0, 0, 0},  //TV = 4.91(845 lines)  AV=2.97  SV=4.48  BV=3.40
    {33343,1216,1048, 0, 0, 0},  //TV = 4.91(845 lines)  AV=2.97  SV=4.57  BV=3.31
    {33343,1328,1024, 0, 0, 0},  //TV = 4.91(845 lines)  AV=2.97  SV=4.66  BV=3.22
    {41669,1140,1024, 0, 0, 0},  //TV = 4.58(1056 lines)  AV=2.97  SV=4.44  BV=3.11
    {41669,1216,1032, 0, 0, 0},  //TV = 4.58(1056 lines)  AV=2.97  SV=4.55  BV=3.01
    {41669,1216,1104, 0, 0, 0},  //TV = 4.58(1056 lines)  AV=2.97  SV=4.64  BV=2.91
    {49994,1140,1056, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.49  BV=2.81
    {49994,1216,1064, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.59  BV=2.70
    {49994,1328,1040, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.68  BV=2.61
    {49994,1424,1040, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.78  BV=2.51
    {49994,1536,1032, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.88  BV=2.41
    {49994,1632,1040, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.98  BV=2.31
    {49994,1728,1056, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.09  BV=2.21
    {49994,1840,1064, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.19  BV=2.11
    {49994,2048,1024, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.29  BV=2.01
    {49994,2144,1048, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.39  BV=1.91
    {49994,2240,1072, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.48  BV=1.81
    {49994,2448,1048, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.58  BV=1.72
    {49994,2656,1040, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.68  BV=1.61
    {49994,2864,1032, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.78  BV=1.51
    {49994,3072,1032, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.88  BV=1.41
    {49994,3328,1024, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.99  BV=1.31
    {49994,3456,1056, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=6.09  BV=1.21
    {58319,3200,1048, 0, 0, 0},  //TV = 4.10(1478 lines)  AV=2.97  SV=5.96  BV=1.11
    {66645,3072,1024, 0, 0, 0},  //TV = 3.91(1689 lines)  AV=2.97  SV=5.87  BV=1.01
    {66645,3200,1056, 0, 0, 0},  //TV = 3.91(1689 lines)  AV=2.97  SV=5.98  BV=0.90
    {75011,3072,1048, 0, 0, 0},  //TV = 3.74(1901 lines)  AV=2.97  SV=5.91  BV=0.80
    {75011,3328,1032, 0, 0, 0},  //TV = 3.74(1901 lines)  AV=2.97  SV=6.00  BV=0.71
    {83336,3200,1032, 0, 0, 0},  //TV = 3.58(2112 lines)  AV=2.97  SV=5.94  BV=0.61
    {83336,3456,1024, 0, 0, 0},  //TV = 3.58(2112 lines)  AV=2.97  SV=6.04  BV=0.51
    {83336,3712,1024, 0, 0, 0},  //TV = 3.58(2112 lines)  AV=2.97  SV=6.14  BV=0.41
    {83336,3968,1032, 0, 0, 0},  //TV = 3.58(2112 lines)  AV=2.97  SV=6.25  BV=0.30
    {83336,4224,1032, 0, 0, 0},  //TV = 3.58(2112 lines)  AV=2.97  SV=6.34  BV=0.21
    {91661,4096,1040, 0, 0, 0},  //TV = 3.45(2323 lines)  AV=2.97  SV=6.31  BV=0.11
    {99987,4096,1032, 0, 0, 0},  //TV = 3.32(2534 lines)  AV=2.97  SV=6.30  BV=-0.01
    {108313,4096,1024, 0, 0, 0},  //TV = 3.21(2745 lines)  AV=2.97  SV=6.29  BV=-0.11
    {108313,4352,1032, 0, 0, 0},  //TV = 3.21(2745 lines)  AV=2.97  SV=6.39  BV=-0.21
    {116678,4224,1048, 0, 0, 0},  //TV = 3.10(2957 lines)  AV=2.97  SV=6.36  BV=-0.29
    {125004,4224,1048, 0, 0, 0},  //TV = 3.00(3168 lines)  AV=2.97  SV=6.36  BV=-0.39
    {125004,4608,1032, 0, 0, 0},  //TV = 3.00(3168 lines)  AV=2.97  SV=6.47  BV=-0.50
    {125004,4864,1048, 0, 0, 0},  //TV = 3.00(3168 lines)  AV=2.97  SV=6.57  BV=-0.60
    {125004,5376,1024, 0, 0, 0},  //TV = 3.00(3168 lines)  AV=2.97  SV=6.68  BV=-0.71
    {125004,5632,1040, 0, 0, 0},  //TV = 3.00(3168 lines)  AV=2.97  SV=6.77  BV=-0.80
    {125004,6016,1040, 0, 0, 0},  //TV = 3.00(3168 lines)  AV=2.97  SV=6.86  BV=-0.89
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
},
};

static strEvPline sAEScene2PLineTable_50Hz =
{
{
    {198,1140,1024, 0, 0, 0},  //TV = 12.30(5 lines)  AV=2.97  SV=4.44  BV=10.83
    {198,1140,1040, 0, 0, 0},  //TV = 12.30(5 lines)  AV=2.97  SV=4.46  BV=10.81
    {198,1216,1048, 0, 0, 0},  //TV = 12.30(5 lines)  AV=2.97  SV=4.57  BV=10.70
    {198,1328,1024, 0, 0, 0},  //TV = 12.30(5 lines)  AV=2.97  SV=4.66  BV=10.61
    {237,1140,1064, 0, 0, 0},  //TV = 12.04(6 lines)  AV=2.97  SV=4.50  BV=10.52
    {237,1216,1072, 0, 0, 0},  //TV = 12.04(6 lines)  AV=2.97  SV=4.60  BV=10.41
    {277,1140,1056, 0, 0, 0},  //TV = 11.82(7 lines)  AV=2.97  SV=4.49  BV=10.30
    {277,1216,1056, 0, 0, 0},  //TV = 11.82(7 lines)  AV=2.97  SV=4.58  BV=10.21
    {316,1140,1056, 0, 0, 0},  //TV = 11.63(8 lines)  AV=2.97  SV=4.49  BV=10.11
    {316,1216,1064, 0, 0, 0},  //TV = 11.63(8 lines)  AV=2.97  SV=4.59  BV=10.01
    {356,1140,1080, 0, 0, 0},  //TV = 11.46(9 lines)  AV=2.97  SV=4.52  BV=9.91
    {395,1140,1040, 0, 0, 0},  //TV = 11.31(10 lines)  AV=2.97  SV=4.46  BV=9.81
    {435,1140,1024, 0, 0, 0},  //TV = 11.17(11 lines)  AV=2.97  SV=4.44  BV=9.70
    {435,1140,1088, 0, 0, 0},  //TV = 11.17(11 lines)  AV=2.97  SV=4.53  BV=9.61
    {474,1140,1064, 0, 0, 0},  //TV = 11.04(12 lines)  AV=2.97  SV=4.50  BV=9.52
    {513,1140,1056, 0, 0, 0},  //TV = 10.93(13 lines)  AV=2.97  SV=4.49  BV=9.41
    {553,1140,1056, 0, 0, 0},  //TV = 10.82(14 lines)  AV=2.97  SV=4.49  BV=9.31
    {592,1140,1056, 0, 0, 0},  //TV = 10.72(15 lines)  AV=2.97  SV=4.49  BV=9.21
    {632,1140,1056, 0, 0, 0},  //TV = 10.63(16 lines)  AV=2.97  SV=4.49  BV=9.11
    {671,1140,1064, 0, 0, 0},  //TV = 10.54(17 lines)  AV=2.97  SV=4.50  BV=9.02
    {750,1140,1024, 0, 0, 0},  //TV = 10.38(19 lines)  AV=2.97  SV=4.44  BV=8.91
    {790,1140,1040, 0, 0, 0},  //TV = 10.31(20 lines)  AV=2.97  SV=4.46  BV=8.81
    {869,1140,1024, 0, 0, 0},  //TV = 10.17(22 lines)  AV=2.97  SV=4.44  BV=8.70
    {908,1140,1040, 0, 0, 0},  //TV = 10.11(23 lines)  AV=2.97  SV=4.46  BV=8.61
    {987,1140,1024, 0, 0, 0},  //TV = 9.98(25 lines)  AV=2.97  SV=4.44  BV=8.51
    {1066,1140,1024, 0, 0, 0},  //TV = 9.87(27 lines)  AV=2.97  SV=4.44  BV=8.40
    {1105,1140,1056, 0, 0, 0},  //TV = 9.82(28 lines)  AV=2.97  SV=4.49  BV=8.31
    {1224,1140,1024, 0, 0, 0},  //TV = 9.67(31 lines)  AV=2.97  SV=4.44  BV=8.20
    {1303,1140,1024, 0, 0, 0},  //TV = 9.58(33 lines)  AV=2.97  SV=4.44  BV=8.11
    {1382,1140,1040, 0, 0, 0},  //TV = 9.50(35 lines)  AV=2.97  SV=4.46  BV=8.01
    {1500,1140,1024, 0, 0, 0},  //TV = 9.38(38 lines)  AV=2.97  SV=4.44  BV=7.91
    {1618,1140,1024, 0, 0, 0},  //TV = 9.27(41 lines)  AV=2.97  SV=4.44  BV=7.80
    {1697,1140,1040, 0, 0, 0},  //TV = 9.20(43 lines)  AV=2.97  SV=4.46  BV=7.71
    {1855,1140,1024, 0, 0, 0},  //TV = 9.07(47 lines)  AV=2.97  SV=4.44  BV=7.60
    {1973,1140,1024, 0, 0, 0},  //TV = 8.99(50 lines)  AV=2.97  SV=4.44  BV=7.51
    {2131,1140,1024, 0, 0, 0},  //TV = 8.87(54 lines)  AV=2.97  SV=4.44  BV=7.40
    {2250,1140,1032, 0, 0, 0},  //TV = 8.80(57 lines)  AV=2.97  SV=4.45  BV=7.31
    {2447,1140,1024, 0, 0, 0},  //TV = 8.67(62 lines)  AV=2.97  SV=4.44  BV=7.20
    {2605,1140,1024, 0, 0, 0},  //TV = 8.58(66 lines)  AV=2.97  SV=4.44  BV=7.11
    {2802,1140,1024, 0, 0, 0},  //TV = 8.48(71 lines)  AV=2.97  SV=4.44  BV=7.01
    {2999,1140,1024, 0, 0, 0},  //TV = 8.38(76 lines)  AV=2.97  SV=4.44  BV=6.91
    {3236,1140,1024, 0, 0, 0},  //TV = 8.27(82 lines)  AV=2.97  SV=4.44  BV=6.80
    {3473,1140,1024, 0, 0, 0},  //TV = 8.17(88 lines)  AV=2.97  SV=4.44  BV=6.70
    {3710,1140,1024, 0, 0, 0},  //TV = 8.07(94 lines)  AV=2.97  SV=4.44  BV=6.60
    {3986,1140,1024, 0, 0, 0},  //TV = 7.97(101 lines)  AV=2.97  SV=4.44  BV=6.50
    {4262,1140,1024, 0, 0, 0},  //TV = 7.87(108 lines)  AV=2.97  SV=4.44  BV=6.40
    {4538,1140,1024, 0, 0, 0},  //TV = 7.78(115 lines)  AV=2.97  SV=4.44  BV=6.31
    {4893,1140,1024, 0, 0, 0},  //TV = 7.68(124 lines)  AV=2.97  SV=4.44  BV=6.20
    {5209,1140,1024, 0, 0, 0},  //TV = 7.58(132 lines)  AV=2.97  SV=4.44  BV=6.11
    {5643,1140,1024, 0, 0, 0},  //TV = 7.47(143 lines)  AV=2.97  SV=4.44  BV=6.00
    {6038,1140,1024, 0, 0, 0},  //TV = 7.37(153 lines)  AV=2.97  SV=4.44  BV=5.90
    {6472,1140,1024, 0, 0, 0},  //TV = 7.27(164 lines)  AV=2.97  SV=4.44  BV=5.80
    {6906,1140,1024, 0, 0, 0},  //TV = 7.18(175 lines)  AV=2.97  SV=4.44  BV=5.71
    {7419,1140,1024, 0, 0, 0},  //TV = 7.07(188 lines)  AV=2.97  SV=4.44  BV=5.60
    {7971,1140,1024, 0, 0, 0},  //TV = 6.97(202 lines)  AV=2.97  SV=4.44  BV=5.50
    {8484,1140,1024, 0, 0, 0},  //TV = 6.88(215 lines)  AV=2.97  SV=4.44  BV=5.41
    {9155,1140,1024, 0, 0, 0},  //TV = 6.77(232 lines)  AV=2.97  SV=4.44  BV=5.30
    {9826,1140,1024, 0, 0, 0},  //TV = 6.67(249 lines)  AV=2.97  SV=4.44  BV=5.20
    {9983,1140,1072, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=4.51  BV=5.11
    {9983,1216,1072, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=4.60  BV=5.02
    {9983,1328,1056, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=4.71  BV=4.91
    {9983,1424,1056, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=4.81  BV=4.81
    {9983,1536,1048, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=4.91  BV=4.71
    {9983,1632,1064, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=5.01  BV=4.60
    {9983,1728,1072, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=5.11  BV=4.51
    {9983,1936,1024, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=5.21  BV=4.41
    {9983,2048,1040, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=5.31  BV=4.31
    {9983,2144,1064, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=5.41  BV=4.21
    {20006,1140,1064, 0, 0, 0},  //TV = 5.64(507 lines)  AV=2.97  SV=4.50  BV=4.12
    {20006,1216,1072, 0, 0, 0},  //TV = 5.64(507 lines)  AV=2.97  SV=4.60  BV=4.01
    {20006,1328,1048, 0, 0, 0},  //TV = 5.64(507 lines)  AV=2.97  SV=4.70  BV=3.92
    {20006,1424,1064, 0, 0, 0},  //TV = 5.64(507 lines)  AV=2.97  SV=4.82  BV=3.80
    {20006,1536,1048, 0, 0, 0},  //TV = 5.64(507 lines)  AV=2.97  SV=4.91  BV=3.71
    {20006,1632,1056, 0, 0, 0},  //TV = 5.64(507 lines)  AV=2.97  SV=5.00  BV=3.61
    {29989,1140,1080, 0, 0, 0},  //TV = 5.06(760 lines)  AV=2.97  SV=4.52  BV=3.51
    {29989,1216,1096, 0, 0, 0},  //TV = 5.06(760 lines)  AV=2.97  SV=4.63  BV=3.40
    {29989,1328,1064, 0, 0, 0},  //TV = 5.06(760 lines)  AV=2.97  SV=4.72  BV=3.31
    {29989,1424,1072, 0, 0, 0},  //TV = 5.06(760 lines)  AV=2.97  SV=4.83  BV=3.20
    {40011,1140,1064, 0, 0, 0},  //TV = 4.64(1014 lines)  AV=2.97  SV=4.50  BV=3.12
    {40011,1216,1072, 0, 0, 0},  //TV = 4.64(1014 lines)  AV=2.97  SV=4.60  BV=3.01
    {40011,1328,1048, 0, 0, 0},  //TV = 4.64(1014 lines)  AV=2.97  SV=4.70  BV=2.92
    {49994,1140,1056, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.49  BV=2.81
    {49994,1216,1064, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.59  BV=2.70
    {49994,1328,1040, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.68  BV=2.61
    {49994,1424,1040, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.78  BV=2.51
    {49994,1536,1032, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.88  BV=2.41
    {49994,1632,1040, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.98  BV=2.31
    {49994,1728,1056, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.09  BV=2.21
    {49994,1840,1064, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.19  BV=2.11
    {49994,2048,1024, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.29  BV=2.01
    {49994,2144,1048, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.39  BV=1.91
    {49994,2240,1072, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.48  BV=1.81
    {49994,2448,1048, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.58  BV=1.72
    {49994,2656,1040, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.68  BV=1.61
    {49994,2864,1032, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.78  BV=1.51
    {49994,3072,1032, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.88  BV=1.41
    {49994,3328,1024, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.99  BV=1.31
    {49994,3456,1056, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=6.09  BV=1.21
    {60017,3072,1064, 0, 0, 0},  //TV = 4.06(1521 lines)  AV=2.97  SV=5.93  BV=1.10
    {60017,3328,1048, 0, 0, 0},  //TV = 4.06(1521 lines)  AV=2.97  SV=6.02  BV=1.01
    {69999,3072,1048, 0, 0, 0},  //TV = 3.84(1774 lines)  AV=2.97  SV=5.91  BV=0.90
    {69999,3328,1032, 0, 0, 0},  //TV = 3.84(1774 lines)  AV=2.97  SV=6.00  BV=0.81
    {79982,3072,1048, 0, 0, 0},  //TV = 3.64(2027 lines)  AV=2.97  SV=5.91  BV=0.71
    {79982,3328,1032, 0, 0, 0},  //TV = 3.64(2027 lines)  AV=2.97  SV=6.00  BV=0.62
    {79982,3584,1032, 0, 0, 0},  //TV = 3.64(2027 lines)  AV=2.97  SV=6.11  BV=0.51
    {79982,3840,1032, 0, 0, 0},  //TV = 3.64(2027 lines)  AV=2.97  SV=6.20  BV=0.41
    {79982,4096,1040, 0, 0, 0},  //TV = 3.64(2027 lines)  AV=2.97  SV=6.31  BV=0.31
    {79982,4352,1040, 0, 0, 0},  //TV = 3.64(2027 lines)  AV=2.97  SV=6.40  BV=0.22
    {90005,4224,1024, 0, 0, 0},  //TV = 3.47(2281 lines)  AV=2.97  SV=6.33  BV=0.11
    {99987,4096,1032, 0, 0, 0},  //TV = 3.32(2534 lines)  AV=2.97  SV=6.30  BV=-0.01
    {99987,4352,1040, 0, 0, 0},  //TV = 3.32(2534 lines)  AV=2.97  SV=6.40  BV=-0.10
    {110010,4224,1040, 0, 0, 0},  //TV = 3.18(2788 lines)  AV=2.97  SV=6.35  BV=-0.20
    {119992,4096,1056, 0, 0, 0},  //TV = 3.06(3041 lines)  AV=2.97  SV=6.33  BV=-0.30
    {119992,4480,1032, 0, 0, 0},  //TV = 3.06(3041 lines)  AV=2.97  SV=6.43  BV=-0.40
    {119992,4736,1048, 0, 0, 0},  //TV = 3.06(3041 lines)  AV=2.97  SV=6.53  BV=-0.50
    {119992,5120,1040, 0, 0, 0},  //TV = 3.06(3041 lines)  AV=2.97  SV=6.63  BV=-0.60
    {119992,5504,1040, 0, 0, 0},  //TV = 3.06(3041 lines)  AV=2.97  SV=6.74  BV=-0.71
    {119992,5888,1040, 0, 0, 0},  //TV = 3.06(3041 lines)  AV=2.97  SV=6.83  BV=-0.80
    {119992,6144,1064, 0, 0, 0},  //TV = 3.06(3041 lines)  AV=2.97  SV=6.93  BV=-0.90
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
},
};

static strAETable g_AE_SceneTable2 =
{
    AETABLE_SCENE_INDEX2,    //eAETableID
    119,    //u4TotalIndex
    20,    //u4StrobeTrigerBV
    109,    //i4MaxBV
    -9,    //i4MinBV
    90,    //i4EffectiveMaxBV
    0,      //i4EffectiveMinBV
    LIB3A_AE_ISO_SPEED_AUTO, //ISO SPEED
    sAEScene2PLineTable_60Hz,
    sAEScene2PLineTable_50Hz,
    NULL,
};

static strEvPline sAEScene3PLineTable_60Hz =
{
{
    {198,1140,1024, 0, 0, 0},  //TV = 12.30(5 lines)  AV=2.97  SV=4.44  BV=10.83
    {198,1140,1040, 0, 0, 0},  //TV = 12.30(5 lines)  AV=2.97  SV=4.46  BV=10.81
    {198,1216,1048, 0, 0, 0},  //TV = 12.30(5 lines)  AV=2.97  SV=4.57  BV=10.70
    {198,1328,1024, 0, 0, 0},  //TV = 12.30(5 lines)  AV=2.97  SV=4.66  BV=10.61
    {237,1140,1064, 0, 0, 0},  //TV = 12.04(6 lines)  AV=2.97  SV=4.50  BV=10.52
    {237,1216,1072, 0, 0, 0},  //TV = 12.04(6 lines)  AV=2.97  SV=4.60  BV=10.41
    {277,1140,1056, 0, 0, 0},  //TV = 11.82(7 lines)  AV=2.97  SV=4.49  BV=10.30
    {277,1216,1056, 0, 0, 0},  //TV = 11.82(7 lines)  AV=2.97  SV=4.58  BV=10.21
    {316,1140,1056, 0, 0, 0},  //TV = 11.63(8 lines)  AV=2.97  SV=4.49  BV=10.11
    {316,1216,1064, 0, 0, 0},  //TV = 11.63(8 lines)  AV=2.97  SV=4.59  BV=10.01
    {356,1140,1080, 0, 0, 0},  //TV = 11.46(9 lines)  AV=2.97  SV=4.52  BV=9.91
    {395,1140,1040, 0, 0, 0},  //TV = 11.31(10 lines)  AV=2.97  SV=4.46  BV=9.81
    {435,1140,1024, 0, 0, 0},  //TV = 11.17(11 lines)  AV=2.97  SV=4.44  BV=9.70
    {435,1140,1088, 0, 0, 0},  //TV = 11.17(11 lines)  AV=2.97  SV=4.53  BV=9.61
    {474,1140,1064, 0, 0, 0},  //TV = 11.04(12 lines)  AV=2.97  SV=4.50  BV=9.52
    {513,1140,1056, 0, 0, 0},  //TV = 10.93(13 lines)  AV=2.97  SV=4.49  BV=9.41
    {553,1140,1056, 0, 0, 0},  //TV = 10.82(14 lines)  AV=2.97  SV=4.49  BV=9.31
    {592,1140,1056, 0, 0, 0},  //TV = 10.72(15 lines)  AV=2.97  SV=4.49  BV=9.21
    {632,1140,1056, 0, 0, 0},  //TV = 10.63(16 lines)  AV=2.97  SV=4.49  BV=9.11
    {671,1140,1064, 0, 0, 0},  //TV = 10.54(17 lines)  AV=2.97  SV=4.50  BV=9.02
    {750,1140,1024, 0, 0, 0},  //TV = 10.38(19 lines)  AV=2.97  SV=4.44  BV=8.91
    {790,1140,1040, 0, 0, 0},  //TV = 10.31(20 lines)  AV=2.97  SV=4.46  BV=8.81
    {869,1140,1024, 0, 0, 0},  //TV = 10.17(22 lines)  AV=2.97  SV=4.44  BV=8.70
    {908,1140,1040, 0, 0, 0},  //TV = 10.11(23 lines)  AV=2.97  SV=4.46  BV=8.61
    {987,1140,1024, 0, 0, 0},  //TV = 9.98(25 lines)  AV=2.97  SV=4.44  BV=8.51
    {1066,1140,1024, 0, 0, 0},  //TV = 9.87(27 lines)  AV=2.97  SV=4.44  BV=8.40
    {1105,1140,1056, 0, 0, 0},  //TV = 9.82(28 lines)  AV=2.97  SV=4.49  BV=8.31
    {1224,1140,1024, 0, 0, 0},  //TV = 9.67(31 lines)  AV=2.97  SV=4.44  BV=8.20
    {1303,1140,1024, 0, 0, 0},  //TV = 9.58(33 lines)  AV=2.97  SV=4.44  BV=8.11
    {1382,1140,1040, 0, 0, 0},  //TV = 9.50(35 lines)  AV=2.97  SV=4.46  BV=8.01
    {1500,1140,1024, 0, 0, 0},  //TV = 9.38(38 lines)  AV=2.97  SV=4.44  BV=7.91
    {1618,1140,1024, 0, 0, 0},  //TV = 9.27(41 lines)  AV=2.97  SV=4.44  BV=7.80
    {1697,1140,1040, 0, 0, 0},  //TV = 9.20(43 lines)  AV=2.97  SV=4.46  BV=7.71
    {1855,1140,1024, 0, 0, 0},  //TV = 9.07(47 lines)  AV=2.97  SV=4.44  BV=7.60
    {1973,1140,1024, 0, 0, 0},  //TV = 8.99(50 lines)  AV=2.97  SV=4.44  BV=7.51
    {2131,1140,1024, 0, 0, 0},  //TV = 8.87(54 lines)  AV=2.97  SV=4.44  BV=7.40
    {2250,1140,1032, 0, 0, 0},  //TV = 8.80(57 lines)  AV=2.97  SV=4.45  BV=7.31
    {2447,1140,1024, 0, 0, 0},  //TV = 8.67(62 lines)  AV=2.97  SV=4.44  BV=7.20
    {2605,1140,1024, 0, 0, 0},  //TV = 8.58(66 lines)  AV=2.97  SV=4.44  BV=7.11
    {2802,1140,1024, 0, 0, 0},  //TV = 8.48(71 lines)  AV=2.97  SV=4.44  BV=7.01
    {2999,1140,1024, 0, 0, 0},  //TV = 8.38(76 lines)  AV=2.97  SV=4.44  BV=6.91
    {3236,1140,1024, 0, 0, 0},  //TV = 8.27(82 lines)  AV=2.97  SV=4.44  BV=6.80
    {3473,1140,1024, 0, 0, 0},  //TV = 8.17(88 lines)  AV=2.97  SV=4.44  BV=6.70
    {3710,1140,1024, 0, 0, 0},  //TV = 8.07(94 lines)  AV=2.97  SV=4.44  BV=6.60
    {3986,1140,1024, 0, 0, 0},  //TV = 7.97(101 lines)  AV=2.97  SV=4.44  BV=6.50
    {4262,1140,1024, 0, 0, 0},  //TV = 7.87(108 lines)  AV=2.97  SV=4.44  BV=6.40
    {4538,1140,1024, 0, 0, 0},  //TV = 7.78(115 lines)  AV=2.97  SV=4.44  BV=6.31
    {4893,1140,1024, 0, 0, 0},  //TV = 7.68(124 lines)  AV=2.97  SV=4.44  BV=6.20
    {5209,1140,1024, 0, 0, 0},  //TV = 7.58(132 lines)  AV=2.97  SV=4.44  BV=6.11
    {5643,1140,1024, 0, 0, 0},  //TV = 7.47(143 lines)  AV=2.97  SV=4.44  BV=6.00
    {6038,1140,1024, 0, 0, 0},  //TV = 7.37(153 lines)  AV=2.97  SV=4.44  BV=5.90
    {6472,1140,1024, 0, 0, 0},  //TV = 7.27(164 lines)  AV=2.97  SV=4.44  BV=5.80
    {6906,1140,1024, 0, 0, 0},  //TV = 7.18(175 lines)  AV=2.97  SV=4.44  BV=5.71
    {7419,1140,1024, 0, 0, 0},  //TV = 7.07(188 lines)  AV=2.97  SV=4.44  BV=5.60
    {7971,1140,1024, 0, 0, 0},  //TV = 6.97(202 lines)  AV=2.97  SV=4.44  BV=5.50
    {8326,1140,1048, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=4.48  BV=5.40
    {8326,1216,1048, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=4.57  BV=5.31
    {8326,1328,1024, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=4.66  BV=5.22
    {8326,1424,1024, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=4.76  BV=5.12
    {8326,1536,1024, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=4.87  BV=5.01
    {8326,1632,1032, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=4.97  BV=4.91
    {8326,1728,1040, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=5.06  BV=4.81
    {8326,1840,1048, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=5.17  BV=4.71
    {8326,1936,1072, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=5.27  BV=4.61
    {8326,2144,1032, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=5.36  BV=4.51
    {16652,1140,1048, 0, 0, 0},  //TV = 5.91(422 lines)  AV=2.97  SV=4.48  BV=4.40
    {16652,1216,1048, 0, 0, 0},  //TV = 5.91(422 lines)  AV=2.97  SV=4.57  BV=4.31
    {16652,1328,1024, 0, 0, 0},  //TV = 5.91(422 lines)  AV=2.97  SV=4.66  BV=4.22
    {16652,1424,1024, 0, 0, 0},  //TV = 5.91(422 lines)  AV=2.97  SV=4.76  BV=4.12
    {16652,1424,1104, 0, 0, 0},  //TV = 5.91(422 lines)  AV=2.97  SV=4.87  BV=4.01
    {16652,1632,1032, 0, 0, 0},  //TV = 5.91(422 lines)  AV=2.97  SV=4.97  BV=3.91
    {25017,1140,1056, 0, 0, 0},  //TV = 5.32(634 lines)  AV=2.97  SV=4.49  BV=3.81
    {25017,1216,1064, 0, 0, 0},  //TV = 5.32(634 lines)  AV=2.97  SV=4.59  BV=3.70
    {25017,1328,1048, 0, 0, 0},  //TV = 5.32(634 lines)  AV=2.97  SV=4.70  BV=3.60
    {25017,1424,1040, 0, 0, 0},  //TV = 5.32(634 lines)  AV=2.97  SV=4.78  BV=3.51
    {33343,1140,1048, 0, 0, 0},  //TV = 4.91(845 lines)  AV=2.97  SV=4.48  BV=3.40
    {33343,1216,1048, 0, 0, 0},  //TV = 4.91(845 lines)  AV=2.97  SV=4.57  BV=3.31
    {33343,1328,1024, 0, 0, 0},  //TV = 4.91(845 lines)  AV=2.97  SV=4.66  BV=3.22
    {41669,1140,1024, 0, 0, 0},  //TV = 4.58(1056 lines)  AV=2.97  SV=4.44  BV=3.11
    {41669,1216,1032, 0, 0, 0},  //TV = 4.58(1056 lines)  AV=2.97  SV=4.55  BV=3.01
    {41669,1216,1104, 0, 0, 0},  //TV = 4.58(1056 lines)  AV=2.97  SV=4.64  BV=2.91
    {49994,1140,1056, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.49  BV=2.81
    {49994,1216,1064, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.59  BV=2.70
    {49994,1328,1040, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.68  BV=2.61
    {49994,1424,1040, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.78  BV=2.51
    {49994,1536,1032, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.88  BV=2.41
    {49994,1632,1040, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.98  BV=2.31
    {49994,1728,1056, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.09  BV=2.21
    {49994,1840,1064, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.19  BV=2.11
    {49994,2048,1024, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.29  BV=2.01
    {49994,2144,1048, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.39  BV=1.91
    {49994,2240,1072, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.48  BV=1.81
    {49994,2448,1048, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.58  BV=1.72
    {49994,2656,1040, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.68  BV=1.61
    {49994,2864,1032, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.78  BV=1.51
    {49994,3072,1032, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.88  BV=1.41
    {49994,3328,1024, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.99  BV=1.31
    {49994,3456,1056, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=6.09  BV=1.21
    {58319,3200,1048, 0, 0, 0},  //TV = 4.10(1478 lines)  AV=2.97  SV=5.96  BV=1.11
    {66645,3072,1024, 0, 0, 0},  //TV = 3.91(1689 lines)  AV=2.97  SV=5.87  BV=1.01
    {66645,3200,1056, 0, 0, 0},  //TV = 3.91(1689 lines)  AV=2.97  SV=5.98  BV=0.90
    {75011,3072,1048, 0, 0, 0},  //TV = 3.74(1901 lines)  AV=2.97  SV=5.91  BV=0.80
    {75011,3328,1032, 0, 0, 0},  //TV = 3.74(1901 lines)  AV=2.97  SV=6.00  BV=0.71
    {83336,3200,1032, 0, 0, 0},  //TV = 3.58(2112 lines)  AV=2.97  SV=5.94  BV=0.61
    {83336,3456,1024, 0, 0, 0},  //TV = 3.58(2112 lines)  AV=2.97  SV=6.04  BV=0.51
    {83336,3712,1024, 0, 0, 0},  //TV = 3.58(2112 lines)  AV=2.97  SV=6.14  BV=0.41
    {83336,3968,1032, 0, 0, 0},  //TV = 3.58(2112 lines)  AV=2.97  SV=6.25  BV=0.30
    {83336,4224,1032, 0, 0, 0},  //TV = 3.58(2112 lines)  AV=2.97  SV=6.34  BV=0.21
    {91661,4096,1040, 0, 0, 0},  //TV = 3.45(2323 lines)  AV=2.97  SV=6.31  BV=0.11
    {99987,4096,1032, 0, 0, 0},  //TV = 3.32(2534 lines)  AV=2.97  SV=6.30  BV=-0.01
    {108313,4096,1024, 0, 0, 0},  //TV = 3.21(2745 lines)  AV=2.97  SV=6.29  BV=-0.11
    {108313,4352,1032, 0, 0, 0},  //TV = 3.21(2745 lines)  AV=2.97  SV=6.39  BV=-0.21
    {116678,4224,1048, 0, 0, 0},  //TV = 3.10(2957 lines)  AV=2.97  SV=6.36  BV=-0.29
    {125004,4224,1048, 0, 0, 0},  //TV = 3.00(3168 lines)  AV=2.97  SV=6.36  BV=-0.39
    {125004,4608,1032, 0, 0, 0},  //TV = 3.00(3168 lines)  AV=2.97  SV=6.47  BV=-0.50
    {125004,4864,1048, 0, 0, 0},  //TV = 3.00(3168 lines)  AV=2.97  SV=6.57  BV=-0.60
    {125004,5376,1024, 0, 0, 0},  //TV = 3.00(3168 lines)  AV=2.97  SV=6.68  BV=-0.71
    {125004,5632,1040, 0, 0, 0},  //TV = 3.00(3168 lines)  AV=2.97  SV=6.77  BV=-0.80
    {125004,6016,1040, 0, 0, 0},  //TV = 3.00(3168 lines)  AV=2.97  SV=6.86  BV=-0.89
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
},
};

static strEvPline sAEScene3PLineTable_50Hz =
{
{
    {198,1140,1024, 0, 0, 0},  //TV = 12.30(5 lines)  AV=2.97  SV=4.44  BV=10.83
    {198,1140,1040, 0, 0, 0},  //TV = 12.30(5 lines)  AV=2.97  SV=4.46  BV=10.81
    {198,1216,1048, 0, 0, 0},  //TV = 12.30(5 lines)  AV=2.97  SV=4.57  BV=10.70
    {198,1328,1024, 0, 0, 0},  //TV = 12.30(5 lines)  AV=2.97  SV=4.66  BV=10.61
    {237,1140,1064, 0, 0, 0},  //TV = 12.04(6 lines)  AV=2.97  SV=4.50  BV=10.52
    {237,1216,1072, 0, 0, 0},  //TV = 12.04(6 lines)  AV=2.97  SV=4.60  BV=10.41
    {277,1140,1056, 0, 0, 0},  //TV = 11.82(7 lines)  AV=2.97  SV=4.49  BV=10.30
    {277,1216,1056, 0, 0, 0},  //TV = 11.82(7 lines)  AV=2.97  SV=4.58  BV=10.21
    {316,1140,1056, 0, 0, 0},  //TV = 11.63(8 lines)  AV=2.97  SV=4.49  BV=10.11
    {316,1216,1064, 0, 0, 0},  //TV = 11.63(8 lines)  AV=2.97  SV=4.59  BV=10.01
    {356,1140,1080, 0, 0, 0},  //TV = 11.46(9 lines)  AV=2.97  SV=4.52  BV=9.91
    {395,1140,1040, 0, 0, 0},  //TV = 11.31(10 lines)  AV=2.97  SV=4.46  BV=9.81
    {435,1140,1024, 0, 0, 0},  //TV = 11.17(11 lines)  AV=2.97  SV=4.44  BV=9.70
    {435,1140,1088, 0, 0, 0},  //TV = 11.17(11 lines)  AV=2.97  SV=4.53  BV=9.61
    {474,1140,1064, 0, 0, 0},  //TV = 11.04(12 lines)  AV=2.97  SV=4.50  BV=9.52
    {513,1140,1056, 0, 0, 0},  //TV = 10.93(13 lines)  AV=2.97  SV=4.49  BV=9.41
    {553,1140,1056, 0, 0, 0},  //TV = 10.82(14 lines)  AV=2.97  SV=4.49  BV=9.31
    {592,1140,1056, 0, 0, 0},  //TV = 10.72(15 lines)  AV=2.97  SV=4.49  BV=9.21
    {632,1140,1056, 0, 0, 0},  //TV = 10.63(16 lines)  AV=2.97  SV=4.49  BV=9.11
    {671,1140,1064, 0, 0, 0},  //TV = 10.54(17 lines)  AV=2.97  SV=4.50  BV=9.02
    {750,1140,1024, 0, 0, 0},  //TV = 10.38(19 lines)  AV=2.97  SV=4.44  BV=8.91
    {790,1140,1040, 0, 0, 0},  //TV = 10.31(20 lines)  AV=2.97  SV=4.46  BV=8.81
    {869,1140,1024, 0, 0, 0},  //TV = 10.17(22 lines)  AV=2.97  SV=4.44  BV=8.70
    {908,1140,1040, 0, 0, 0},  //TV = 10.11(23 lines)  AV=2.97  SV=4.46  BV=8.61
    {987,1140,1024, 0, 0, 0},  //TV = 9.98(25 lines)  AV=2.97  SV=4.44  BV=8.51
    {1066,1140,1024, 0, 0, 0},  //TV = 9.87(27 lines)  AV=2.97  SV=4.44  BV=8.40
    {1105,1140,1056, 0, 0, 0},  //TV = 9.82(28 lines)  AV=2.97  SV=4.49  BV=8.31
    {1224,1140,1024, 0, 0, 0},  //TV = 9.67(31 lines)  AV=2.97  SV=4.44  BV=8.20
    {1303,1140,1024, 0, 0, 0},  //TV = 9.58(33 lines)  AV=2.97  SV=4.44  BV=8.11
    {1382,1140,1040, 0, 0, 0},  //TV = 9.50(35 lines)  AV=2.97  SV=4.46  BV=8.01
    {1500,1140,1024, 0, 0, 0},  //TV = 9.38(38 lines)  AV=2.97  SV=4.44  BV=7.91
    {1618,1140,1024, 0, 0, 0},  //TV = 9.27(41 lines)  AV=2.97  SV=4.44  BV=7.80
    {1697,1140,1040, 0, 0, 0},  //TV = 9.20(43 lines)  AV=2.97  SV=4.46  BV=7.71
    {1855,1140,1024, 0, 0, 0},  //TV = 9.07(47 lines)  AV=2.97  SV=4.44  BV=7.60
    {1973,1140,1024, 0, 0, 0},  //TV = 8.99(50 lines)  AV=2.97  SV=4.44  BV=7.51
    {2131,1140,1024, 0, 0, 0},  //TV = 8.87(54 lines)  AV=2.97  SV=4.44  BV=7.40
    {2250,1140,1032, 0, 0, 0},  //TV = 8.80(57 lines)  AV=2.97  SV=4.45  BV=7.31
    {2447,1140,1024, 0, 0, 0},  //TV = 8.67(62 lines)  AV=2.97  SV=4.44  BV=7.20
    {2605,1140,1024, 0, 0, 0},  //TV = 8.58(66 lines)  AV=2.97  SV=4.44  BV=7.11
    {2802,1140,1024, 0, 0, 0},  //TV = 8.48(71 lines)  AV=2.97  SV=4.44  BV=7.01
    {2999,1140,1024, 0, 0, 0},  //TV = 8.38(76 lines)  AV=2.97  SV=4.44  BV=6.91
    {3236,1140,1024, 0, 0, 0},  //TV = 8.27(82 lines)  AV=2.97  SV=4.44  BV=6.80
    {3473,1140,1024, 0, 0, 0},  //TV = 8.17(88 lines)  AV=2.97  SV=4.44  BV=6.70
    {3710,1140,1024, 0, 0, 0},  //TV = 8.07(94 lines)  AV=2.97  SV=4.44  BV=6.60
    {3986,1140,1024, 0, 0, 0},  //TV = 7.97(101 lines)  AV=2.97  SV=4.44  BV=6.50
    {4262,1140,1024, 0, 0, 0},  //TV = 7.87(108 lines)  AV=2.97  SV=4.44  BV=6.40
    {4538,1140,1024, 0, 0, 0},  //TV = 7.78(115 lines)  AV=2.97  SV=4.44  BV=6.31
    {4893,1140,1024, 0, 0, 0},  //TV = 7.68(124 lines)  AV=2.97  SV=4.44  BV=6.20
    {5209,1140,1024, 0, 0, 0},  //TV = 7.58(132 lines)  AV=2.97  SV=4.44  BV=6.11
    {5643,1140,1024, 0, 0, 0},  //TV = 7.47(143 lines)  AV=2.97  SV=4.44  BV=6.00
    {6038,1140,1024, 0, 0, 0},  //TV = 7.37(153 lines)  AV=2.97  SV=4.44  BV=5.90
    {6472,1140,1024, 0, 0, 0},  //TV = 7.27(164 lines)  AV=2.97  SV=4.44  BV=5.80
    {6906,1140,1024, 0, 0, 0},  //TV = 7.18(175 lines)  AV=2.97  SV=4.44  BV=5.71
    {7419,1140,1024, 0, 0, 0},  //TV = 7.07(188 lines)  AV=2.97  SV=4.44  BV=5.60
    {7971,1140,1024, 0, 0, 0},  //TV = 6.97(202 lines)  AV=2.97  SV=4.44  BV=5.50
    {8484,1140,1024, 0, 0, 0},  //TV = 6.88(215 lines)  AV=2.97  SV=4.44  BV=5.41
    {9155,1140,1024, 0, 0, 0},  //TV = 6.77(232 lines)  AV=2.97  SV=4.44  BV=5.30
    {9826,1140,1024, 0, 0, 0},  //TV = 6.67(249 lines)  AV=2.97  SV=4.44  BV=5.20
    {9983,1140,1072, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=4.51  BV=5.11
    {9983,1216,1072, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=4.60  BV=5.02
    {9983,1328,1056, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=4.71  BV=4.91
    {9983,1424,1056, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=4.81  BV=4.81
    {9983,1536,1048, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=4.91  BV=4.71
    {9983,1632,1064, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=5.01  BV=4.60
    {9983,1728,1072, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=5.11  BV=4.51
    {9983,1936,1024, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=5.21  BV=4.41
    {9983,2048,1040, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=5.31  BV=4.31
    {9983,2144,1064, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=5.41  BV=4.21
    {20006,1140,1064, 0, 0, 0},  //TV = 5.64(507 lines)  AV=2.97  SV=4.50  BV=4.12
    {20006,1216,1072, 0, 0, 0},  //TV = 5.64(507 lines)  AV=2.97  SV=4.60  BV=4.01
    {20006,1328,1048, 0, 0, 0},  //TV = 5.64(507 lines)  AV=2.97  SV=4.70  BV=3.92
    {20006,1424,1064, 0, 0, 0},  //TV = 5.64(507 lines)  AV=2.97  SV=4.82  BV=3.80
    {20006,1536,1048, 0, 0, 0},  //TV = 5.64(507 lines)  AV=2.97  SV=4.91  BV=3.71
    {20006,1632,1056, 0, 0, 0},  //TV = 5.64(507 lines)  AV=2.97  SV=5.00  BV=3.61
    {29989,1140,1080, 0, 0, 0},  //TV = 5.06(760 lines)  AV=2.97  SV=4.52  BV=3.51
    {29989,1216,1096, 0, 0, 0},  //TV = 5.06(760 lines)  AV=2.97  SV=4.63  BV=3.40
    {29989,1328,1064, 0, 0, 0},  //TV = 5.06(760 lines)  AV=2.97  SV=4.72  BV=3.31
    {29989,1424,1072, 0, 0, 0},  //TV = 5.06(760 lines)  AV=2.97  SV=4.83  BV=3.20
    {40011,1140,1064, 0, 0, 0},  //TV = 4.64(1014 lines)  AV=2.97  SV=4.50  BV=3.12
    {40011,1216,1072, 0, 0, 0},  //TV = 4.64(1014 lines)  AV=2.97  SV=4.60  BV=3.01
    {40011,1328,1048, 0, 0, 0},  //TV = 4.64(1014 lines)  AV=2.97  SV=4.70  BV=2.92
    {49994,1140,1056, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.49  BV=2.81
    {49994,1216,1064, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.59  BV=2.70
    {49994,1328,1040, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.68  BV=2.61
    {49994,1424,1040, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.78  BV=2.51
    {49994,1536,1032, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.88  BV=2.41
    {49994,1632,1040, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.98  BV=2.31
    {49994,1728,1056, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.09  BV=2.21
    {49994,1840,1064, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.19  BV=2.11
    {49994,2048,1024, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.29  BV=2.01
    {49994,2144,1048, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.39  BV=1.91
    {49994,2240,1072, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.48  BV=1.81
    {49994,2448,1048, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.58  BV=1.72
    {49994,2656,1040, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.68  BV=1.61
    {49994,2864,1032, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.78  BV=1.51
    {49994,3072,1032, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.88  BV=1.41
    {49994,3328,1024, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.99  BV=1.31
    {49994,3456,1056, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=6.09  BV=1.21
    {60017,3072,1064, 0, 0, 0},  //TV = 4.06(1521 lines)  AV=2.97  SV=5.93  BV=1.10
    {60017,3328,1048, 0, 0, 0},  //TV = 4.06(1521 lines)  AV=2.97  SV=6.02  BV=1.01
    {69999,3072,1048, 0, 0, 0},  //TV = 3.84(1774 lines)  AV=2.97  SV=5.91  BV=0.90
    {69999,3328,1032, 0, 0, 0},  //TV = 3.84(1774 lines)  AV=2.97  SV=6.00  BV=0.81
    {79982,3072,1048, 0, 0, 0},  //TV = 3.64(2027 lines)  AV=2.97  SV=5.91  BV=0.71
    {79982,3328,1032, 0, 0, 0},  //TV = 3.64(2027 lines)  AV=2.97  SV=6.00  BV=0.62
    {79982,3584,1032, 0, 0, 0},  //TV = 3.64(2027 lines)  AV=2.97  SV=6.11  BV=0.51
    {79982,3840,1032, 0, 0, 0},  //TV = 3.64(2027 lines)  AV=2.97  SV=6.20  BV=0.41
    {79982,4096,1040, 0, 0, 0},  //TV = 3.64(2027 lines)  AV=2.97  SV=6.31  BV=0.31
    {79982,4352,1040, 0, 0, 0},  //TV = 3.64(2027 lines)  AV=2.97  SV=6.40  BV=0.22
    {90005,4224,1024, 0, 0, 0},  //TV = 3.47(2281 lines)  AV=2.97  SV=6.33  BV=0.11
    {99987,4096,1032, 0, 0, 0},  //TV = 3.32(2534 lines)  AV=2.97  SV=6.30  BV=-0.01
    {99987,4352,1040, 0, 0, 0},  //TV = 3.32(2534 lines)  AV=2.97  SV=6.40  BV=-0.10
    {110010,4224,1040, 0, 0, 0},  //TV = 3.18(2788 lines)  AV=2.97  SV=6.35  BV=-0.20
    {119992,4096,1056, 0, 0, 0},  //TV = 3.06(3041 lines)  AV=2.97  SV=6.33  BV=-0.30
    {119992,4480,1032, 0, 0, 0},  //TV = 3.06(3041 lines)  AV=2.97  SV=6.43  BV=-0.40
    {119992,4736,1048, 0, 0, 0},  //TV = 3.06(3041 lines)  AV=2.97  SV=6.53  BV=-0.50
    {119992,5120,1040, 0, 0, 0},  //TV = 3.06(3041 lines)  AV=2.97  SV=6.63  BV=-0.60
    {119992,5504,1040, 0, 0, 0},  //TV = 3.06(3041 lines)  AV=2.97  SV=6.74  BV=-0.71
    {119992,5888,1040, 0, 0, 0},  //TV = 3.06(3041 lines)  AV=2.97  SV=6.83  BV=-0.80
    {119992,6144,1064, 0, 0, 0},  //TV = 3.06(3041 lines)  AV=2.97  SV=6.93  BV=-0.90
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
},
};

static strAETable g_AE_SceneTable3 =
{
    AETABLE_SCENE_INDEX3,    //eAETableID
    119,    //u4TotalIndex
    20,    //u4StrobeTrigerBV
    109,    //i4MaxBV
    -9,    //i4MinBV
    90,    //i4EffectiveMaxBV
    0,      //i4EffectiveMinBV
    LIB3A_AE_ISO_SPEED_AUTO, //ISO SPEED
    sAEScene3PLineTable_60Hz,
    sAEScene3PLineTable_50Hz,
    NULL,
};

static strEvPline sAEScene4PLineTable_60Hz =
{
{
    {198,1140,1024, 0, 0, 0},  //TV = 12.30(5 lines)  AV=2.97  SV=4.44  BV=10.83
    {198,1140,1040, 0, 0, 0},  //TV = 12.30(5 lines)  AV=2.97  SV=4.46  BV=10.81
    {198,1216,1048, 0, 0, 0},  //TV = 12.30(5 lines)  AV=2.97  SV=4.57  BV=10.70
    {198,1328,1024, 0, 0, 0},  //TV = 12.30(5 lines)  AV=2.97  SV=4.66  BV=10.61
    {237,1140,1064, 0, 0, 0},  //TV = 12.04(6 lines)  AV=2.97  SV=4.50  BV=10.52
    {237,1216,1072, 0, 0, 0},  //TV = 12.04(6 lines)  AV=2.97  SV=4.60  BV=10.41
    {277,1140,1056, 0, 0, 0},  //TV = 11.82(7 lines)  AV=2.97  SV=4.49  BV=10.30
    {277,1216,1056, 0, 0, 0},  //TV = 11.82(7 lines)  AV=2.97  SV=4.58  BV=10.21
    {316,1140,1056, 0, 0, 0},  //TV = 11.63(8 lines)  AV=2.97  SV=4.49  BV=10.11
    {316,1216,1064, 0, 0, 0},  //TV = 11.63(8 lines)  AV=2.97  SV=4.59  BV=10.01
    {356,1140,1080, 0, 0, 0},  //TV = 11.46(9 lines)  AV=2.97  SV=4.52  BV=9.91
    {395,1140,1040, 0, 0, 0},  //TV = 11.31(10 lines)  AV=2.97  SV=4.46  BV=9.81
    {435,1140,1024, 0, 0, 0},  //TV = 11.17(11 lines)  AV=2.97  SV=4.44  BV=9.70
    {435,1140,1088, 0, 0, 0},  //TV = 11.17(11 lines)  AV=2.97  SV=4.53  BV=9.61
    {474,1140,1064, 0, 0, 0},  //TV = 11.04(12 lines)  AV=2.97  SV=4.50  BV=9.52
    {513,1140,1056, 0, 0, 0},  //TV = 10.93(13 lines)  AV=2.97  SV=4.49  BV=9.41
    {553,1140,1056, 0, 0, 0},  //TV = 10.82(14 lines)  AV=2.97  SV=4.49  BV=9.31
    {592,1140,1056, 0, 0, 0},  //TV = 10.72(15 lines)  AV=2.97  SV=4.49  BV=9.21
    {632,1140,1056, 0, 0, 0},  //TV = 10.63(16 lines)  AV=2.97  SV=4.49  BV=9.11
    {671,1140,1064, 0, 0, 0},  //TV = 10.54(17 lines)  AV=2.97  SV=4.50  BV=9.02
    {750,1140,1024, 0, 0, 0},  //TV = 10.38(19 lines)  AV=2.97  SV=4.44  BV=8.91
    {790,1140,1040, 0, 0, 0},  //TV = 10.31(20 lines)  AV=2.97  SV=4.46  BV=8.81
    {869,1140,1024, 0, 0, 0},  //TV = 10.17(22 lines)  AV=2.97  SV=4.44  BV=8.70
    {908,1140,1040, 0, 0, 0},  //TV = 10.11(23 lines)  AV=2.97  SV=4.46  BV=8.61
    {987,1140,1024, 0, 0, 0},  //TV = 9.98(25 lines)  AV=2.97  SV=4.44  BV=8.51
    {1066,1140,1024, 0, 0, 0},  //TV = 9.87(27 lines)  AV=2.97  SV=4.44  BV=8.40
    {1105,1140,1056, 0, 0, 0},  //TV = 9.82(28 lines)  AV=2.97  SV=4.49  BV=8.31
    {1224,1140,1024, 0, 0, 0},  //TV = 9.67(31 lines)  AV=2.97  SV=4.44  BV=8.20
    {1303,1140,1024, 0, 0, 0},  //TV = 9.58(33 lines)  AV=2.97  SV=4.44  BV=8.11
    {1382,1140,1040, 0, 0, 0},  //TV = 9.50(35 lines)  AV=2.97  SV=4.46  BV=8.01
    {1500,1140,1024, 0, 0, 0},  //TV = 9.38(38 lines)  AV=2.97  SV=4.44  BV=7.91
    {1618,1140,1024, 0, 0, 0},  //TV = 9.27(41 lines)  AV=2.97  SV=4.44  BV=7.80
    {1697,1140,1040, 0, 0, 0},  //TV = 9.20(43 lines)  AV=2.97  SV=4.46  BV=7.71
    {1855,1140,1024, 0, 0, 0},  //TV = 9.07(47 lines)  AV=2.97  SV=4.44  BV=7.60
    {1973,1140,1024, 0, 0, 0},  //TV = 8.99(50 lines)  AV=2.97  SV=4.44  BV=7.51
    {2131,1140,1024, 0, 0, 0},  //TV = 8.87(54 lines)  AV=2.97  SV=4.44  BV=7.40
    {2250,1140,1032, 0, 0, 0},  //TV = 8.80(57 lines)  AV=2.97  SV=4.45  BV=7.31
    {2447,1140,1024, 0, 0, 0},  //TV = 8.67(62 lines)  AV=2.97  SV=4.44  BV=7.20
    {2605,1140,1024, 0, 0, 0},  //TV = 8.58(66 lines)  AV=2.97  SV=4.44  BV=7.11
    {2802,1140,1024, 0, 0, 0},  //TV = 8.48(71 lines)  AV=2.97  SV=4.44  BV=7.01
    {2999,1140,1024, 0, 0, 0},  //TV = 8.38(76 lines)  AV=2.97  SV=4.44  BV=6.91
    {3236,1140,1024, 0, 0, 0},  //TV = 8.27(82 lines)  AV=2.97  SV=4.44  BV=6.80
    {3473,1140,1024, 0, 0, 0},  //TV = 8.17(88 lines)  AV=2.97  SV=4.44  BV=6.70
    {3710,1140,1024, 0, 0, 0},  //TV = 8.07(94 lines)  AV=2.97  SV=4.44  BV=6.60
    {3986,1140,1024, 0, 0, 0},  //TV = 7.97(101 lines)  AV=2.97  SV=4.44  BV=6.50
    {4262,1140,1024, 0, 0, 0},  //TV = 7.87(108 lines)  AV=2.97  SV=4.44  BV=6.40
    {4538,1140,1024, 0, 0, 0},  //TV = 7.78(115 lines)  AV=2.97  SV=4.44  BV=6.31
    {4893,1140,1024, 0, 0, 0},  //TV = 7.68(124 lines)  AV=2.97  SV=4.44  BV=6.20
    {5209,1140,1024, 0, 0, 0},  //TV = 7.58(132 lines)  AV=2.97  SV=4.44  BV=6.11
    {5643,1140,1024, 0, 0, 0},  //TV = 7.47(143 lines)  AV=2.97  SV=4.44  BV=6.00
    {6038,1140,1024, 0, 0, 0},  //TV = 7.37(153 lines)  AV=2.97  SV=4.44  BV=5.90
    {6472,1140,1024, 0, 0, 0},  //TV = 7.27(164 lines)  AV=2.97  SV=4.44  BV=5.80
    {6906,1140,1024, 0, 0, 0},  //TV = 7.18(175 lines)  AV=2.97  SV=4.44  BV=5.71
    {7419,1140,1024, 0, 0, 0},  //TV = 7.07(188 lines)  AV=2.97  SV=4.44  BV=5.60
    {7971,1140,1024, 0, 0, 0},  //TV = 6.97(202 lines)  AV=2.97  SV=4.44  BV=5.50
    {8326,1140,1048, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=4.48  BV=5.40
    {8326,1216,1048, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=4.57  BV=5.31
    {8326,1328,1024, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=4.66  BV=5.22
    {8326,1424,1024, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=4.76  BV=5.12
    {8326,1536,1024, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=4.87  BV=5.01
    {8326,1632,1032, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=4.97  BV=4.91
    {8326,1728,1040, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=5.06  BV=4.81
    {8326,1840,1048, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=5.17  BV=4.71
    {8326,1936,1072, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=5.27  BV=4.61
    {8326,2144,1032, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=5.36  BV=4.51
    {16652,1140,1048, 0, 0, 0},  //TV = 5.91(422 lines)  AV=2.97  SV=4.48  BV=4.40
    {16652,1216,1048, 0, 0, 0},  //TV = 5.91(422 lines)  AV=2.97  SV=4.57  BV=4.31
    {16652,1328,1024, 0, 0, 0},  //TV = 5.91(422 lines)  AV=2.97  SV=4.66  BV=4.22
    {16652,1424,1024, 0, 0, 0},  //TV = 5.91(422 lines)  AV=2.97  SV=4.76  BV=4.12
    {16652,1424,1104, 0, 0, 0},  //TV = 5.91(422 lines)  AV=2.97  SV=4.87  BV=4.01
    {16652,1632,1032, 0, 0, 0},  //TV = 5.91(422 lines)  AV=2.97  SV=4.97  BV=3.91
    {25017,1140,1056, 0, 0, 0},  //TV = 5.32(634 lines)  AV=2.97  SV=4.49  BV=3.81
    {25017,1216,1064, 0, 0, 0},  //TV = 5.32(634 lines)  AV=2.97  SV=4.59  BV=3.70
    {25017,1328,1048, 0, 0, 0},  //TV = 5.32(634 lines)  AV=2.97  SV=4.70  BV=3.60
    {25017,1424,1040, 0, 0, 0},  //TV = 5.32(634 lines)  AV=2.97  SV=4.78  BV=3.51
    {33343,1140,1048, 0, 0, 0},  //TV = 4.91(845 lines)  AV=2.97  SV=4.48  BV=3.40
    {33343,1216,1048, 0, 0, 0},  //TV = 4.91(845 lines)  AV=2.97  SV=4.57  BV=3.31
    {33343,1328,1024, 0, 0, 0},  //TV = 4.91(845 lines)  AV=2.97  SV=4.66  BV=3.22
    {41669,1140,1024, 0, 0, 0},  //TV = 4.58(1056 lines)  AV=2.97  SV=4.44  BV=3.11
    {41669,1216,1032, 0, 0, 0},  //TV = 4.58(1056 lines)  AV=2.97  SV=4.55  BV=3.01
    {41669,1216,1104, 0, 0, 0},  //TV = 4.58(1056 lines)  AV=2.97  SV=4.64  BV=2.91
    {49994,1140,1056, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.49  BV=2.81
    {49994,1216,1064, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.59  BV=2.70
    {49994,1328,1040, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.68  BV=2.61
    {49994,1424,1040, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.78  BV=2.51
    {49994,1536,1032, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.88  BV=2.41
    {49994,1632,1040, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.98  BV=2.31
    {49994,1728,1056, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.09  BV=2.21
    {49994,1840,1064, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.19  BV=2.11
    {49994,2048,1024, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.29  BV=2.01
    {49994,2144,1048, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.39  BV=1.91
    {49994,2240,1072, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.48  BV=1.81
    {49994,2448,1048, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.58  BV=1.72
    {49994,2656,1040, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.68  BV=1.61
    {49994,2864,1032, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.78  BV=1.51
    {49994,3072,1032, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.88  BV=1.41
    {49994,3328,1024, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.99  BV=1.31
    {49994,3456,1056, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=6.09  BV=1.21
    {58319,3200,1048, 0, 0, 0},  //TV = 4.10(1478 lines)  AV=2.97  SV=5.96  BV=1.11
    {66645,3072,1024, 0, 0, 0},  //TV = 3.91(1689 lines)  AV=2.97  SV=5.87  BV=1.01
    {66645,3200,1056, 0, 0, 0},  //TV = 3.91(1689 lines)  AV=2.97  SV=5.98  BV=0.90
    {75011,3072,1048, 0, 0, 0},  //TV = 3.74(1901 lines)  AV=2.97  SV=5.91  BV=0.80
    {75011,3328,1032, 0, 0, 0},  //TV = 3.74(1901 lines)  AV=2.97  SV=6.00  BV=0.71
    {83336,3200,1032, 0, 0, 0},  //TV = 3.58(2112 lines)  AV=2.97  SV=5.94  BV=0.61
    {83336,3456,1024, 0, 0, 0},  //TV = 3.58(2112 lines)  AV=2.97  SV=6.04  BV=0.51
    {83336,3712,1024, 0, 0, 0},  //TV = 3.58(2112 lines)  AV=2.97  SV=6.14  BV=0.41
    {83336,3968,1032, 0, 0, 0},  //TV = 3.58(2112 lines)  AV=2.97  SV=6.25  BV=0.30
    {83336,4224,1032, 0, 0, 0},  //TV = 3.58(2112 lines)  AV=2.97  SV=6.34  BV=0.21
    {91661,4096,1040, 0, 0, 0},  //TV = 3.45(2323 lines)  AV=2.97  SV=6.31  BV=0.11
    {99987,4096,1032, 0, 0, 0},  //TV = 3.32(2534 lines)  AV=2.97  SV=6.30  BV=-0.01
    {108313,4096,1024, 0, 0, 0},  //TV = 3.21(2745 lines)  AV=2.97  SV=6.29  BV=-0.11
    {108313,4352,1032, 0, 0, 0},  //TV = 3.21(2745 lines)  AV=2.97  SV=6.39  BV=-0.21
    {116678,4224,1048, 0, 0, 0},  //TV = 3.10(2957 lines)  AV=2.97  SV=6.36  BV=-0.29
    {125004,4224,1048, 0, 0, 0},  //TV = 3.00(3168 lines)  AV=2.97  SV=6.36  BV=-0.39
    {125004,4608,1032, 0, 0, 0},  //TV = 3.00(3168 lines)  AV=2.97  SV=6.47  BV=-0.50
    {125004,4864,1048, 0, 0, 0},  //TV = 3.00(3168 lines)  AV=2.97  SV=6.57  BV=-0.60
    {125004,5376,1024, 0, 0, 0},  //TV = 3.00(3168 lines)  AV=2.97  SV=6.68  BV=-0.71
    {125004,5632,1040, 0, 0, 0},  //TV = 3.00(3168 lines)  AV=2.97  SV=6.77  BV=-0.80
    {125004,6016,1040, 0, 0, 0},  //TV = 3.00(3168 lines)  AV=2.97  SV=6.86  BV=-0.89
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
},
};

static strEvPline sAEScene4PLineTable_50Hz =
{
{
    {198,1140,1024, 0, 0, 0},  //TV = 12.30(5 lines)  AV=2.97  SV=4.44  BV=10.83
    {198,1140,1040, 0, 0, 0},  //TV = 12.30(5 lines)  AV=2.97  SV=4.46  BV=10.81
    {198,1216,1048, 0, 0, 0},  //TV = 12.30(5 lines)  AV=2.97  SV=4.57  BV=10.70
    {198,1328,1024, 0, 0, 0},  //TV = 12.30(5 lines)  AV=2.97  SV=4.66  BV=10.61
    {237,1140,1064, 0, 0, 0},  //TV = 12.04(6 lines)  AV=2.97  SV=4.50  BV=10.52
    {237,1216,1072, 0, 0, 0},  //TV = 12.04(6 lines)  AV=2.97  SV=4.60  BV=10.41
    {277,1140,1056, 0, 0, 0},  //TV = 11.82(7 lines)  AV=2.97  SV=4.49  BV=10.30
    {277,1216,1056, 0, 0, 0},  //TV = 11.82(7 lines)  AV=2.97  SV=4.58  BV=10.21
    {316,1140,1056, 0, 0, 0},  //TV = 11.63(8 lines)  AV=2.97  SV=4.49  BV=10.11
    {316,1216,1064, 0, 0, 0},  //TV = 11.63(8 lines)  AV=2.97  SV=4.59  BV=10.01
    {356,1140,1080, 0, 0, 0},  //TV = 11.46(9 lines)  AV=2.97  SV=4.52  BV=9.91
    {395,1140,1040, 0, 0, 0},  //TV = 11.31(10 lines)  AV=2.97  SV=4.46  BV=9.81
    {435,1140,1024, 0, 0, 0},  //TV = 11.17(11 lines)  AV=2.97  SV=4.44  BV=9.70
    {435,1140,1088, 0, 0, 0},  //TV = 11.17(11 lines)  AV=2.97  SV=4.53  BV=9.61
    {474,1140,1064, 0, 0, 0},  //TV = 11.04(12 lines)  AV=2.97  SV=4.50  BV=9.52
    {513,1140,1056, 0, 0, 0},  //TV = 10.93(13 lines)  AV=2.97  SV=4.49  BV=9.41
    {553,1140,1056, 0, 0, 0},  //TV = 10.82(14 lines)  AV=2.97  SV=4.49  BV=9.31
    {592,1140,1056, 0, 0, 0},  //TV = 10.72(15 lines)  AV=2.97  SV=4.49  BV=9.21
    {632,1140,1056, 0, 0, 0},  //TV = 10.63(16 lines)  AV=2.97  SV=4.49  BV=9.11
    {671,1140,1064, 0, 0, 0},  //TV = 10.54(17 lines)  AV=2.97  SV=4.50  BV=9.02
    {750,1140,1024, 0, 0, 0},  //TV = 10.38(19 lines)  AV=2.97  SV=4.44  BV=8.91
    {790,1140,1040, 0, 0, 0},  //TV = 10.31(20 lines)  AV=2.97  SV=4.46  BV=8.81
    {869,1140,1024, 0, 0, 0},  //TV = 10.17(22 lines)  AV=2.97  SV=4.44  BV=8.70
    {908,1140,1040, 0, 0, 0},  //TV = 10.11(23 lines)  AV=2.97  SV=4.46  BV=8.61
    {987,1140,1024, 0, 0, 0},  //TV = 9.98(25 lines)  AV=2.97  SV=4.44  BV=8.51
    {1066,1140,1024, 0, 0, 0},  //TV = 9.87(27 lines)  AV=2.97  SV=4.44  BV=8.40
    {1105,1140,1056, 0, 0, 0},  //TV = 9.82(28 lines)  AV=2.97  SV=4.49  BV=8.31
    {1224,1140,1024, 0, 0, 0},  //TV = 9.67(31 lines)  AV=2.97  SV=4.44  BV=8.20
    {1303,1140,1024, 0, 0, 0},  //TV = 9.58(33 lines)  AV=2.97  SV=4.44  BV=8.11
    {1382,1140,1040, 0, 0, 0},  //TV = 9.50(35 lines)  AV=2.97  SV=4.46  BV=8.01
    {1500,1140,1024, 0, 0, 0},  //TV = 9.38(38 lines)  AV=2.97  SV=4.44  BV=7.91
    {1618,1140,1024, 0, 0, 0},  //TV = 9.27(41 lines)  AV=2.97  SV=4.44  BV=7.80
    {1697,1140,1040, 0, 0, 0},  //TV = 9.20(43 lines)  AV=2.97  SV=4.46  BV=7.71
    {1855,1140,1024, 0, 0, 0},  //TV = 9.07(47 lines)  AV=2.97  SV=4.44  BV=7.60
    {1973,1140,1024, 0, 0, 0},  //TV = 8.99(50 lines)  AV=2.97  SV=4.44  BV=7.51
    {2131,1140,1024, 0, 0, 0},  //TV = 8.87(54 lines)  AV=2.97  SV=4.44  BV=7.40
    {2250,1140,1032, 0, 0, 0},  //TV = 8.80(57 lines)  AV=2.97  SV=4.45  BV=7.31
    {2447,1140,1024, 0, 0, 0},  //TV = 8.67(62 lines)  AV=2.97  SV=4.44  BV=7.20
    {2605,1140,1024, 0, 0, 0},  //TV = 8.58(66 lines)  AV=2.97  SV=4.44  BV=7.11
    {2802,1140,1024, 0, 0, 0},  //TV = 8.48(71 lines)  AV=2.97  SV=4.44  BV=7.01
    {2999,1140,1024, 0, 0, 0},  //TV = 8.38(76 lines)  AV=2.97  SV=4.44  BV=6.91
    {3236,1140,1024, 0, 0, 0},  //TV = 8.27(82 lines)  AV=2.97  SV=4.44  BV=6.80
    {3473,1140,1024, 0, 0, 0},  //TV = 8.17(88 lines)  AV=2.97  SV=4.44  BV=6.70
    {3710,1140,1024, 0, 0, 0},  //TV = 8.07(94 lines)  AV=2.97  SV=4.44  BV=6.60
    {3986,1140,1024, 0, 0, 0},  //TV = 7.97(101 lines)  AV=2.97  SV=4.44  BV=6.50
    {4262,1140,1024, 0, 0, 0},  //TV = 7.87(108 lines)  AV=2.97  SV=4.44  BV=6.40
    {4538,1140,1024, 0, 0, 0},  //TV = 7.78(115 lines)  AV=2.97  SV=4.44  BV=6.31
    {4893,1140,1024, 0, 0, 0},  //TV = 7.68(124 lines)  AV=2.97  SV=4.44  BV=6.20
    {5209,1140,1024, 0, 0, 0},  //TV = 7.58(132 lines)  AV=2.97  SV=4.44  BV=6.11
    {5643,1140,1024, 0, 0, 0},  //TV = 7.47(143 lines)  AV=2.97  SV=4.44  BV=6.00
    {6038,1140,1024, 0, 0, 0},  //TV = 7.37(153 lines)  AV=2.97  SV=4.44  BV=5.90
    {6472,1140,1024, 0, 0, 0},  //TV = 7.27(164 lines)  AV=2.97  SV=4.44  BV=5.80
    {6906,1140,1024, 0, 0, 0},  //TV = 7.18(175 lines)  AV=2.97  SV=4.44  BV=5.71
    {7419,1140,1024, 0, 0, 0},  //TV = 7.07(188 lines)  AV=2.97  SV=4.44  BV=5.60
    {7971,1140,1024, 0, 0, 0},  //TV = 6.97(202 lines)  AV=2.97  SV=4.44  BV=5.50
    {8484,1140,1024, 0, 0, 0},  //TV = 6.88(215 lines)  AV=2.97  SV=4.44  BV=5.41
    {9155,1140,1024, 0, 0, 0},  //TV = 6.77(232 lines)  AV=2.97  SV=4.44  BV=5.30
    {9826,1140,1024, 0, 0, 0},  //TV = 6.67(249 lines)  AV=2.97  SV=4.44  BV=5.20
    {9983,1140,1072, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=4.51  BV=5.11
    {9983,1216,1072, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=4.60  BV=5.02
    {9983,1328,1056, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=4.71  BV=4.91
    {9983,1424,1056, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=4.81  BV=4.81
    {9983,1536,1048, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=4.91  BV=4.71
    {9983,1632,1064, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=5.01  BV=4.60
    {9983,1728,1072, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=5.11  BV=4.51
    {9983,1936,1024, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=5.21  BV=4.41
    {9983,2048,1040, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=5.31  BV=4.31
    {9983,2144,1064, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=5.41  BV=4.21
    {20006,1140,1064, 0, 0, 0},  //TV = 5.64(507 lines)  AV=2.97  SV=4.50  BV=4.12
    {20006,1216,1072, 0, 0, 0},  //TV = 5.64(507 lines)  AV=2.97  SV=4.60  BV=4.01
    {20006,1328,1048, 0, 0, 0},  //TV = 5.64(507 lines)  AV=2.97  SV=4.70  BV=3.92
    {20006,1424,1064, 0, 0, 0},  //TV = 5.64(507 lines)  AV=2.97  SV=4.82  BV=3.80
    {20006,1536,1048, 0, 0, 0},  //TV = 5.64(507 lines)  AV=2.97  SV=4.91  BV=3.71
    {20006,1632,1056, 0, 0, 0},  //TV = 5.64(507 lines)  AV=2.97  SV=5.00  BV=3.61
    {29989,1140,1080, 0, 0, 0},  //TV = 5.06(760 lines)  AV=2.97  SV=4.52  BV=3.51
    {29989,1216,1096, 0, 0, 0},  //TV = 5.06(760 lines)  AV=2.97  SV=4.63  BV=3.40
    {29989,1328,1064, 0, 0, 0},  //TV = 5.06(760 lines)  AV=2.97  SV=4.72  BV=3.31
    {29989,1424,1072, 0, 0, 0},  //TV = 5.06(760 lines)  AV=2.97  SV=4.83  BV=3.20
    {40011,1140,1064, 0, 0, 0},  //TV = 4.64(1014 lines)  AV=2.97  SV=4.50  BV=3.12
    {40011,1216,1072, 0, 0, 0},  //TV = 4.64(1014 lines)  AV=2.97  SV=4.60  BV=3.01
    {40011,1328,1048, 0, 0, 0},  //TV = 4.64(1014 lines)  AV=2.97  SV=4.70  BV=2.92
    {49994,1140,1056, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.49  BV=2.81
    {49994,1216,1064, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.59  BV=2.70
    {49994,1328,1040, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.68  BV=2.61
    {49994,1424,1040, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.78  BV=2.51
    {49994,1536,1032, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.88  BV=2.41
    {49994,1632,1040, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.98  BV=2.31
    {49994,1728,1056, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.09  BV=2.21
    {49994,1840,1064, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.19  BV=2.11
    {49994,2048,1024, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.29  BV=2.01
    {49994,2144,1048, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.39  BV=1.91
    {49994,2240,1072, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.48  BV=1.81
    {49994,2448,1048, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.58  BV=1.72
    {49994,2656,1040, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.68  BV=1.61
    {49994,2864,1032, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.78  BV=1.51
    {49994,3072,1032, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.88  BV=1.41
    {49994,3328,1024, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.99  BV=1.31
    {49994,3456,1056, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=6.09  BV=1.21
    {60017,3072,1064, 0, 0, 0},  //TV = 4.06(1521 lines)  AV=2.97  SV=5.93  BV=1.10
    {60017,3328,1048, 0, 0, 0},  //TV = 4.06(1521 lines)  AV=2.97  SV=6.02  BV=1.01
    {69999,3072,1048, 0, 0, 0},  //TV = 3.84(1774 lines)  AV=2.97  SV=5.91  BV=0.90
    {69999,3328,1032, 0, 0, 0},  //TV = 3.84(1774 lines)  AV=2.97  SV=6.00  BV=0.81
    {79982,3072,1048, 0, 0, 0},  //TV = 3.64(2027 lines)  AV=2.97  SV=5.91  BV=0.71
    {79982,3328,1032, 0, 0, 0},  //TV = 3.64(2027 lines)  AV=2.97  SV=6.00  BV=0.62
    {79982,3584,1032, 0, 0, 0},  //TV = 3.64(2027 lines)  AV=2.97  SV=6.11  BV=0.51
    {79982,3840,1032, 0, 0, 0},  //TV = 3.64(2027 lines)  AV=2.97  SV=6.20  BV=0.41
    {79982,4096,1040, 0, 0, 0},  //TV = 3.64(2027 lines)  AV=2.97  SV=6.31  BV=0.31
    {79982,4352,1040, 0, 0, 0},  //TV = 3.64(2027 lines)  AV=2.97  SV=6.40  BV=0.22
    {90005,4224,1024, 0, 0, 0},  //TV = 3.47(2281 lines)  AV=2.97  SV=6.33  BV=0.11
    {99987,4096,1032, 0, 0, 0},  //TV = 3.32(2534 lines)  AV=2.97  SV=6.30  BV=-0.01
    {99987,4352,1040, 0, 0, 0},  //TV = 3.32(2534 lines)  AV=2.97  SV=6.40  BV=-0.10
    {110010,4224,1040, 0, 0, 0},  //TV = 3.18(2788 lines)  AV=2.97  SV=6.35  BV=-0.20
    {119992,4096,1056, 0, 0, 0},  //TV = 3.06(3041 lines)  AV=2.97  SV=6.33  BV=-0.30
    {119992,4480,1032, 0, 0, 0},  //TV = 3.06(3041 lines)  AV=2.97  SV=6.43  BV=-0.40
    {119992,4736,1048, 0, 0, 0},  //TV = 3.06(3041 lines)  AV=2.97  SV=6.53  BV=-0.50
    {119992,5120,1040, 0, 0, 0},  //TV = 3.06(3041 lines)  AV=2.97  SV=6.63  BV=-0.60
    {119992,5504,1040, 0, 0, 0},  //TV = 3.06(3041 lines)  AV=2.97  SV=6.74  BV=-0.71
    {119992,5888,1040, 0, 0, 0},  //TV = 3.06(3041 lines)  AV=2.97  SV=6.83  BV=-0.80
    {119992,6144,1064, 0, 0, 0},  //TV = 3.06(3041 lines)  AV=2.97  SV=6.93  BV=-0.90
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
},
};

static strAETable g_AE_SceneTable4 =
{
    AETABLE_SCENE_INDEX4,    //eAETableID
    119,    //u4TotalIndex
    20,    //u4StrobeTrigerBV
    109,    //i4MaxBV
    -9,    //i4MinBV
    90,    //i4EffectiveMaxBV
    0,      //i4EffectiveMinBV
    LIB3A_AE_ISO_SPEED_AUTO, //ISO SPEED
    sAEScene4PLineTable_60Hz,
    sAEScene4PLineTable_50Hz,
    NULL,
};

static strEvPline sAEScene5PLineTable_60Hz =
{
{
    {198,1140,1024, 0, 0, 0},  //TV = 12.30(5 lines)  AV=2.97  SV=4.44  BV=10.83
    {198,1140,1040, 0, 0, 0},  //TV = 12.30(5 lines)  AV=2.97  SV=4.46  BV=10.81
    {198,1216,1048, 0, 0, 0},  //TV = 12.30(5 lines)  AV=2.97  SV=4.57  BV=10.70
    {198,1328,1024, 0, 0, 0},  //TV = 12.30(5 lines)  AV=2.97  SV=4.66  BV=10.61
    {237,1140,1064, 0, 0, 0},  //TV = 12.04(6 lines)  AV=2.97  SV=4.50  BV=10.52
    {237,1216,1072, 0, 0, 0},  //TV = 12.04(6 lines)  AV=2.97  SV=4.60  BV=10.41
    {277,1140,1056, 0, 0, 0},  //TV = 11.82(7 lines)  AV=2.97  SV=4.49  BV=10.30
    {277,1216,1056, 0, 0, 0},  //TV = 11.82(7 lines)  AV=2.97  SV=4.58  BV=10.21
    {316,1140,1056, 0, 0, 0},  //TV = 11.63(8 lines)  AV=2.97  SV=4.49  BV=10.11
    {316,1216,1064, 0, 0, 0},  //TV = 11.63(8 lines)  AV=2.97  SV=4.59  BV=10.01
    {356,1140,1080, 0, 0, 0},  //TV = 11.46(9 lines)  AV=2.97  SV=4.52  BV=9.91
    {395,1140,1040, 0, 0, 0},  //TV = 11.31(10 lines)  AV=2.97  SV=4.46  BV=9.81
    {435,1140,1024, 0, 0, 0},  //TV = 11.17(11 lines)  AV=2.97  SV=4.44  BV=9.70
    {435,1140,1088, 0, 0, 0},  //TV = 11.17(11 lines)  AV=2.97  SV=4.53  BV=9.61
    {474,1140,1064, 0, 0, 0},  //TV = 11.04(12 lines)  AV=2.97  SV=4.50  BV=9.52
    {513,1140,1056, 0, 0, 0},  //TV = 10.93(13 lines)  AV=2.97  SV=4.49  BV=9.41
    {553,1140,1056, 0, 0, 0},  //TV = 10.82(14 lines)  AV=2.97  SV=4.49  BV=9.31
    {592,1140,1056, 0, 0, 0},  //TV = 10.72(15 lines)  AV=2.97  SV=4.49  BV=9.21
    {632,1140,1056, 0, 0, 0},  //TV = 10.63(16 lines)  AV=2.97  SV=4.49  BV=9.11
    {671,1140,1064, 0, 0, 0},  //TV = 10.54(17 lines)  AV=2.97  SV=4.50  BV=9.02
    {750,1140,1024, 0, 0, 0},  //TV = 10.38(19 lines)  AV=2.97  SV=4.44  BV=8.91
    {790,1140,1040, 0, 0, 0},  //TV = 10.31(20 lines)  AV=2.97  SV=4.46  BV=8.81
    {869,1140,1024, 0, 0, 0},  //TV = 10.17(22 lines)  AV=2.97  SV=4.44  BV=8.70
    {908,1140,1040, 0, 0, 0},  //TV = 10.11(23 lines)  AV=2.97  SV=4.46  BV=8.61
    {987,1140,1024, 0, 0, 0},  //TV = 9.98(25 lines)  AV=2.97  SV=4.44  BV=8.51
    {1066,1140,1024, 0, 0, 0},  //TV = 9.87(27 lines)  AV=2.97  SV=4.44  BV=8.40
    {1105,1140,1056, 0, 0, 0},  //TV = 9.82(28 lines)  AV=2.97  SV=4.49  BV=8.31
    {1224,1140,1024, 0, 0, 0},  //TV = 9.67(31 lines)  AV=2.97  SV=4.44  BV=8.20
    {1303,1140,1024, 0, 0, 0},  //TV = 9.58(33 lines)  AV=2.97  SV=4.44  BV=8.11
    {1382,1140,1040, 0, 0, 0},  //TV = 9.50(35 lines)  AV=2.97  SV=4.46  BV=8.01
    {1500,1140,1024, 0, 0, 0},  //TV = 9.38(38 lines)  AV=2.97  SV=4.44  BV=7.91
    {1618,1140,1024, 0, 0, 0},  //TV = 9.27(41 lines)  AV=2.97  SV=4.44  BV=7.80
    {1697,1140,1040, 0, 0, 0},  //TV = 9.20(43 lines)  AV=2.97  SV=4.46  BV=7.71
    {1855,1140,1024, 0, 0, 0},  //TV = 9.07(47 lines)  AV=2.97  SV=4.44  BV=7.60
    {1973,1140,1024, 0, 0, 0},  //TV = 8.99(50 lines)  AV=2.97  SV=4.44  BV=7.51
    {2131,1140,1024, 0, 0, 0},  //TV = 8.87(54 lines)  AV=2.97  SV=4.44  BV=7.40
    {2250,1140,1032, 0, 0, 0},  //TV = 8.80(57 lines)  AV=2.97  SV=4.45  BV=7.31
    {2447,1140,1024, 0, 0, 0},  //TV = 8.67(62 lines)  AV=2.97  SV=4.44  BV=7.20
    {2605,1140,1024, 0, 0, 0},  //TV = 8.58(66 lines)  AV=2.97  SV=4.44  BV=7.11
    {2802,1140,1024, 0, 0, 0},  //TV = 8.48(71 lines)  AV=2.97  SV=4.44  BV=7.01
    {2999,1140,1024, 0, 0, 0},  //TV = 8.38(76 lines)  AV=2.97  SV=4.44  BV=6.91
    {3236,1140,1024, 0, 0, 0},  //TV = 8.27(82 lines)  AV=2.97  SV=4.44  BV=6.80
    {3473,1140,1024, 0, 0, 0},  //TV = 8.17(88 lines)  AV=2.97  SV=4.44  BV=6.70
    {3710,1140,1024, 0, 0, 0},  //TV = 8.07(94 lines)  AV=2.97  SV=4.44  BV=6.60
    {3986,1140,1024, 0, 0, 0},  //TV = 7.97(101 lines)  AV=2.97  SV=4.44  BV=6.50
    {4262,1140,1024, 0, 0, 0},  //TV = 7.87(108 lines)  AV=2.97  SV=4.44  BV=6.40
    {4538,1140,1024, 0, 0, 0},  //TV = 7.78(115 lines)  AV=2.97  SV=4.44  BV=6.31
    {4893,1140,1024, 0, 0, 0},  //TV = 7.68(124 lines)  AV=2.97  SV=4.44  BV=6.20
    {5209,1140,1024, 0, 0, 0},  //TV = 7.58(132 lines)  AV=2.97  SV=4.44  BV=6.11
    {5643,1140,1024, 0, 0, 0},  //TV = 7.47(143 lines)  AV=2.97  SV=4.44  BV=6.00
    {6038,1140,1024, 0, 0, 0},  //TV = 7.37(153 lines)  AV=2.97  SV=4.44  BV=5.90
    {6472,1140,1024, 0, 0, 0},  //TV = 7.27(164 lines)  AV=2.97  SV=4.44  BV=5.80
    {6906,1140,1024, 0, 0, 0},  //TV = 7.18(175 lines)  AV=2.97  SV=4.44  BV=5.71
    {7419,1140,1024, 0, 0, 0},  //TV = 7.07(188 lines)  AV=2.97  SV=4.44  BV=5.60
    {7971,1140,1024, 0, 0, 0},  //TV = 6.97(202 lines)  AV=2.97  SV=4.44  BV=5.50
    {8326,1140,1048, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=4.48  BV=5.40
    {8326,1216,1048, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=4.57  BV=5.31
    {8326,1328,1024, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=4.66  BV=5.22
    {8326,1424,1024, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=4.76  BV=5.12
    {8326,1536,1024, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=4.87  BV=5.01
    {8326,1632,1032, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=4.97  BV=4.91
    {8326,1728,1040, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=5.06  BV=4.81
    {8326,1840,1048, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=5.17  BV=4.71
    {8326,1936,1072, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=5.27  BV=4.61
    {8326,2144,1032, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=5.36  BV=4.51
    {16652,1140,1048, 0, 0, 0},  //TV = 5.91(422 lines)  AV=2.97  SV=4.48  BV=4.40
    {16652,1216,1048, 0, 0, 0},  //TV = 5.91(422 lines)  AV=2.97  SV=4.57  BV=4.31
    {16652,1328,1024, 0, 0, 0},  //TV = 5.91(422 lines)  AV=2.97  SV=4.66  BV=4.22
    {16652,1424,1024, 0, 0, 0},  //TV = 5.91(422 lines)  AV=2.97  SV=4.76  BV=4.12
    {16652,1424,1104, 0, 0, 0},  //TV = 5.91(422 lines)  AV=2.97  SV=4.87  BV=4.01
    {16652,1632,1032, 0, 0, 0},  //TV = 5.91(422 lines)  AV=2.97  SV=4.97  BV=3.91
    {25017,1140,1056, 0, 0, 0},  //TV = 5.32(634 lines)  AV=2.97  SV=4.49  BV=3.81
    {25017,1216,1064, 0, 0, 0},  //TV = 5.32(634 lines)  AV=2.97  SV=4.59  BV=3.70
    {25017,1328,1048, 0, 0, 0},  //TV = 5.32(634 lines)  AV=2.97  SV=4.70  BV=3.60
    {25017,1424,1040, 0, 0, 0},  //TV = 5.32(634 lines)  AV=2.97  SV=4.78  BV=3.51
    {33343,1140,1048, 0, 0, 0},  //TV = 4.91(845 lines)  AV=2.97  SV=4.48  BV=3.40
    {33343,1216,1048, 0, 0, 0},  //TV = 4.91(845 lines)  AV=2.97  SV=4.57  BV=3.31
    {33343,1328,1024, 0, 0, 0},  //TV = 4.91(845 lines)  AV=2.97  SV=4.66  BV=3.22
    {41669,1140,1024, 0, 0, 0},  //TV = 4.58(1056 lines)  AV=2.97  SV=4.44  BV=3.11
    {41669,1216,1032, 0, 0, 0},  //TV = 4.58(1056 lines)  AV=2.97  SV=4.55  BV=3.01
    {41669,1216,1104, 0, 0, 0},  //TV = 4.58(1056 lines)  AV=2.97  SV=4.64  BV=2.91
    {49994,1140,1056, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.49  BV=2.81
    {49994,1216,1064, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.59  BV=2.70
    {49994,1328,1040, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.68  BV=2.61
    {49994,1424,1040, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.78  BV=2.51
    {49994,1536,1032, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.88  BV=2.41
    {49994,1632,1040, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.98  BV=2.31
    {49994,1728,1056, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.09  BV=2.21
    {49994,1840,1064, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.19  BV=2.11
    {49994,2048,1024, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.29  BV=2.01
    {49994,2144,1048, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.39  BV=1.91
    {49994,2240,1072, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.48  BV=1.81
    {49994,2448,1048, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.58  BV=1.72
    {49994,2656,1040, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.68  BV=1.61
    {49994,2864,1032, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.78  BV=1.51
    {49994,3072,1032, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.88  BV=1.41
    {49994,3328,1024, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.99  BV=1.31
    {49994,3456,1056, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=6.09  BV=1.21
    {58319,3200,1048, 0, 0, 0},  //TV = 4.10(1478 lines)  AV=2.97  SV=5.96  BV=1.11
    {66645,3072,1024, 0, 0, 0},  //TV = 3.91(1689 lines)  AV=2.97  SV=5.87  BV=1.01
    {66645,3200,1056, 0, 0, 0},  //TV = 3.91(1689 lines)  AV=2.97  SV=5.98  BV=0.90
    {75011,3072,1048, 0, 0, 0},  //TV = 3.74(1901 lines)  AV=2.97  SV=5.91  BV=0.80
    {75011,3328,1032, 0, 0, 0},  //TV = 3.74(1901 lines)  AV=2.97  SV=6.00  BV=0.71
    {83336,3200,1032, 0, 0, 0},  //TV = 3.58(2112 lines)  AV=2.97  SV=5.94  BV=0.61
    {83336,3456,1024, 0, 0, 0},  //TV = 3.58(2112 lines)  AV=2.97  SV=6.04  BV=0.51
    {83336,3712,1024, 0, 0, 0},  //TV = 3.58(2112 lines)  AV=2.97  SV=6.14  BV=0.41
    {83336,3968,1032, 0, 0, 0},  //TV = 3.58(2112 lines)  AV=2.97  SV=6.25  BV=0.30
    {83336,4224,1032, 0, 0, 0},  //TV = 3.58(2112 lines)  AV=2.97  SV=6.34  BV=0.21
    {91661,4096,1040, 0, 0, 0},  //TV = 3.45(2323 lines)  AV=2.97  SV=6.31  BV=0.11
    {99987,4096,1032, 0, 0, 0},  //TV = 3.32(2534 lines)  AV=2.97  SV=6.30  BV=-0.01
    {108313,4096,1024, 0, 0, 0},  //TV = 3.21(2745 lines)  AV=2.97  SV=6.29  BV=-0.11
    {108313,4352,1032, 0, 0, 0},  //TV = 3.21(2745 lines)  AV=2.97  SV=6.39  BV=-0.21
    {116678,4224,1048, 0, 0, 0},  //TV = 3.10(2957 lines)  AV=2.97  SV=6.36  BV=-0.29
    {125004,4224,1048, 0, 0, 0},  //TV = 3.00(3168 lines)  AV=2.97  SV=6.36  BV=-0.39
    {125004,4608,1032, 0, 0, 0},  //TV = 3.00(3168 lines)  AV=2.97  SV=6.47  BV=-0.50
    {125004,4864,1048, 0, 0, 0},  //TV = 3.00(3168 lines)  AV=2.97  SV=6.57  BV=-0.60
    {125004,5376,1024, 0, 0, 0},  //TV = 3.00(3168 lines)  AV=2.97  SV=6.68  BV=-0.71
    {125004,5632,1040, 0, 0, 0},  //TV = 3.00(3168 lines)  AV=2.97  SV=6.77  BV=-0.80
    {125004,6016,1040, 0, 0, 0},  //TV = 3.00(3168 lines)  AV=2.97  SV=6.86  BV=-0.89
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
},
};

static strEvPline sAEScene5PLineTable_50Hz =
{
{
    {198,1140,1024, 0, 0, 0},  //TV = 12.30(5 lines)  AV=2.97  SV=4.44  BV=10.83
    {198,1140,1040, 0, 0, 0},  //TV = 12.30(5 lines)  AV=2.97  SV=4.46  BV=10.81
    {198,1216,1048, 0, 0, 0},  //TV = 12.30(5 lines)  AV=2.97  SV=4.57  BV=10.70
    {198,1328,1024, 0, 0, 0},  //TV = 12.30(5 lines)  AV=2.97  SV=4.66  BV=10.61
    {237,1140,1064, 0, 0, 0},  //TV = 12.04(6 lines)  AV=2.97  SV=4.50  BV=10.52
    {237,1216,1072, 0, 0, 0},  //TV = 12.04(6 lines)  AV=2.97  SV=4.60  BV=10.41
    {277,1140,1056, 0, 0, 0},  //TV = 11.82(7 lines)  AV=2.97  SV=4.49  BV=10.30
    {277,1216,1056, 0, 0, 0},  //TV = 11.82(7 lines)  AV=2.97  SV=4.58  BV=10.21
    {316,1140,1056, 0, 0, 0},  //TV = 11.63(8 lines)  AV=2.97  SV=4.49  BV=10.11
    {316,1216,1064, 0, 0, 0},  //TV = 11.63(8 lines)  AV=2.97  SV=4.59  BV=10.01
    {356,1140,1080, 0, 0, 0},  //TV = 11.46(9 lines)  AV=2.97  SV=4.52  BV=9.91
    {395,1140,1040, 0, 0, 0},  //TV = 11.31(10 lines)  AV=2.97  SV=4.46  BV=9.81
    {435,1140,1024, 0, 0, 0},  //TV = 11.17(11 lines)  AV=2.97  SV=4.44  BV=9.70
    {435,1140,1088, 0, 0, 0},  //TV = 11.17(11 lines)  AV=2.97  SV=4.53  BV=9.61
    {474,1140,1064, 0, 0, 0},  //TV = 11.04(12 lines)  AV=2.97  SV=4.50  BV=9.52
    {513,1140,1056, 0, 0, 0},  //TV = 10.93(13 lines)  AV=2.97  SV=4.49  BV=9.41
    {553,1140,1056, 0, 0, 0},  //TV = 10.82(14 lines)  AV=2.97  SV=4.49  BV=9.31
    {592,1140,1056, 0, 0, 0},  //TV = 10.72(15 lines)  AV=2.97  SV=4.49  BV=9.21
    {632,1140,1056, 0, 0, 0},  //TV = 10.63(16 lines)  AV=2.97  SV=4.49  BV=9.11
    {671,1140,1064, 0, 0, 0},  //TV = 10.54(17 lines)  AV=2.97  SV=4.50  BV=9.02
    {750,1140,1024, 0, 0, 0},  //TV = 10.38(19 lines)  AV=2.97  SV=4.44  BV=8.91
    {790,1140,1040, 0, 0, 0},  //TV = 10.31(20 lines)  AV=2.97  SV=4.46  BV=8.81
    {869,1140,1024, 0, 0, 0},  //TV = 10.17(22 lines)  AV=2.97  SV=4.44  BV=8.70
    {908,1140,1040, 0, 0, 0},  //TV = 10.11(23 lines)  AV=2.97  SV=4.46  BV=8.61
    {987,1140,1024, 0, 0, 0},  //TV = 9.98(25 lines)  AV=2.97  SV=4.44  BV=8.51
    {1066,1140,1024, 0, 0, 0},  //TV = 9.87(27 lines)  AV=2.97  SV=4.44  BV=8.40
    {1105,1140,1056, 0, 0, 0},  //TV = 9.82(28 lines)  AV=2.97  SV=4.49  BV=8.31
    {1224,1140,1024, 0, 0, 0},  //TV = 9.67(31 lines)  AV=2.97  SV=4.44  BV=8.20
    {1303,1140,1024, 0, 0, 0},  //TV = 9.58(33 lines)  AV=2.97  SV=4.44  BV=8.11
    {1382,1140,1040, 0, 0, 0},  //TV = 9.50(35 lines)  AV=2.97  SV=4.46  BV=8.01
    {1500,1140,1024, 0, 0, 0},  //TV = 9.38(38 lines)  AV=2.97  SV=4.44  BV=7.91
    {1618,1140,1024, 0, 0, 0},  //TV = 9.27(41 lines)  AV=2.97  SV=4.44  BV=7.80
    {1697,1140,1040, 0, 0, 0},  //TV = 9.20(43 lines)  AV=2.97  SV=4.46  BV=7.71
    {1855,1140,1024, 0, 0, 0},  //TV = 9.07(47 lines)  AV=2.97  SV=4.44  BV=7.60
    {1973,1140,1024, 0, 0, 0},  //TV = 8.99(50 lines)  AV=2.97  SV=4.44  BV=7.51
    {2131,1140,1024, 0, 0, 0},  //TV = 8.87(54 lines)  AV=2.97  SV=4.44  BV=7.40
    {2250,1140,1032, 0, 0, 0},  //TV = 8.80(57 lines)  AV=2.97  SV=4.45  BV=7.31
    {2447,1140,1024, 0, 0, 0},  //TV = 8.67(62 lines)  AV=2.97  SV=4.44  BV=7.20
    {2605,1140,1024, 0, 0, 0},  //TV = 8.58(66 lines)  AV=2.97  SV=4.44  BV=7.11
    {2802,1140,1024, 0, 0, 0},  //TV = 8.48(71 lines)  AV=2.97  SV=4.44  BV=7.01
    {2999,1140,1024, 0, 0, 0},  //TV = 8.38(76 lines)  AV=2.97  SV=4.44  BV=6.91
    {3236,1140,1024, 0, 0, 0},  //TV = 8.27(82 lines)  AV=2.97  SV=4.44  BV=6.80
    {3473,1140,1024, 0, 0, 0},  //TV = 8.17(88 lines)  AV=2.97  SV=4.44  BV=6.70
    {3710,1140,1024, 0, 0, 0},  //TV = 8.07(94 lines)  AV=2.97  SV=4.44  BV=6.60
    {3986,1140,1024, 0, 0, 0},  //TV = 7.97(101 lines)  AV=2.97  SV=4.44  BV=6.50
    {4262,1140,1024, 0, 0, 0},  //TV = 7.87(108 lines)  AV=2.97  SV=4.44  BV=6.40
    {4538,1140,1024, 0, 0, 0},  //TV = 7.78(115 lines)  AV=2.97  SV=4.44  BV=6.31
    {4893,1140,1024, 0, 0, 0},  //TV = 7.68(124 lines)  AV=2.97  SV=4.44  BV=6.20
    {5209,1140,1024, 0, 0, 0},  //TV = 7.58(132 lines)  AV=2.97  SV=4.44  BV=6.11
    {5643,1140,1024, 0, 0, 0},  //TV = 7.47(143 lines)  AV=2.97  SV=4.44  BV=6.00
    {6038,1140,1024, 0, 0, 0},  //TV = 7.37(153 lines)  AV=2.97  SV=4.44  BV=5.90
    {6472,1140,1024, 0, 0, 0},  //TV = 7.27(164 lines)  AV=2.97  SV=4.44  BV=5.80
    {6906,1140,1024, 0, 0, 0},  //TV = 7.18(175 lines)  AV=2.97  SV=4.44  BV=5.71
    {7419,1140,1024, 0, 0, 0},  //TV = 7.07(188 lines)  AV=2.97  SV=4.44  BV=5.60
    {7971,1140,1024, 0, 0, 0},  //TV = 6.97(202 lines)  AV=2.97  SV=4.44  BV=5.50
    {8484,1140,1024, 0, 0, 0},  //TV = 6.88(215 lines)  AV=2.97  SV=4.44  BV=5.41
    {9155,1140,1024, 0, 0, 0},  //TV = 6.77(232 lines)  AV=2.97  SV=4.44  BV=5.30
    {9826,1140,1024, 0, 0, 0},  //TV = 6.67(249 lines)  AV=2.97  SV=4.44  BV=5.20
    {9983,1140,1072, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=4.51  BV=5.11
    {9983,1216,1072, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=4.60  BV=5.02
    {9983,1328,1056, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=4.71  BV=4.91
    {9983,1424,1056, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=4.81  BV=4.81
    {9983,1536,1048, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=4.91  BV=4.71
    {9983,1632,1064, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=5.01  BV=4.60
    {9983,1728,1072, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=5.11  BV=4.51
    {9983,1936,1024, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=5.21  BV=4.41
    {9983,2048,1040, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=5.31  BV=4.31
    {9983,2144,1064, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=5.41  BV=4.21
    {20006,1140,1064, 0, 0, 0},  //TV = 5.64(507 lines)  AV=2.97  SV=4.50  BV=4.12
    {20006,1216,1072, 0, 0, 0},  //TV = 5.64(507 lines)  AV=2.97  SV=4.60  BV=4.01
    {20006,1328,1048, 0, 0, 0},  //TV = 5.64(507 lines)  AV=2.97  SV=4.70  BV=3.92
    {20006,1424,1064, 0, 0, 0},  //TV = 5.64(507 lines)  AV=2.97  SV=4.82  BV=3.80
    {20006,1536,1048, 0, 0, 0},  //TV = 5.64(507 lines)  AV=2.97  SV=4.91  BV=3.71
    {20006,1632,1056, 0, 0, 0},  //TV = 5.64(507 lines)  AV=2.97  SV=5.00  BV=3.61
    {29989,1140,1080, 0, 0, 0},  //TV = 5.06(760 lines)  AV=2.97  SV=4.52  BV=3.51
    {29989,1216,1096, 0, 0, 0},  //TV = 5.06(760 lines)  AV=2.97  SV=4.63  BV=3.40
    {29989,1328,1064, 0, 0, 0},  //TV = 5.06(760 lines)  AV=2.97  SV=4.72  BV=3.31
    {29989,1424,1072, 0, 0, 0},  //TV = 5.06(760 lines)  AV=2.97  SV=4.83  BV=3.20
    {40011,1140,1064, 0, 0, 0},  //TV = 4.64(1014 lines)  AV=2.97  SV=4.50  BV=3.12
    {40011,1216,1072, 0, 0, 0},  //TV = 4.64(1014 lines)  AV=2.97  SV=4.60  BV=3.01
    {40011,1328,1048, 0, 0, 0},  //TV = 4.64(1014 lines)  AV=2.97  SV=4.70  BV=2.92
    {49994,1140,1056, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.49  BV=2.81
    {49994,1216,1064, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.59  BV=2.70
    {49994,1328,1040, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.68  BV=2.61
    {49994,1424,1040, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.78  BV=2.51
    {49994,1536,1032, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.88  BV=2.41
    {49994,1632,1040, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.98  BV=2.31
    {49994,1728,1056, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.09  BV=2.21
    {49994,1840,1064, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.19  BV=2.11
    {49994,2048,1024, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.29  BV=2.01
    {49994,2144,1048, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.39  BV=1.91
    {49994,2240,1072, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.48  BV=1.81
    {49994,2448,1048, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.58  BV=1.72
    {49994,2656,1040, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.68  BV=1.61
    {49994,2864,1032, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.78  BV=1.51
    {49994,3072,1032, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.88  BV=1.41
    {49994,3328,1024, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.99  BV=1.31
    {49994,3456,1056, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=6.09  BV=1.21
    {60017,3072,1064, 0, 0, 0},  //TV = 4.06(1521 lines)  AV=2.97  SV=5.93  BV=1.10
    {60017,3328,1048, 0, 0, 0},  //TV = 4.06(1521 lines)  AV=2.97  SV=6.02  BV=1.01
    {69999,3072,1048, 0, 0, 0},  //TV = 3.84(1774 lines)  AV=2.97  SV=5.91  BV=0.90
    {69999,3328,1032, 0, 0, 0},  //TV = 3.84(1774 lines)  AV=2.97  SV=6.00  BV=0.81
    {79982,3072,1048, 0, 0, 0},  //TV = 3.64(2027 lines)  AV=2.97  SV=5.91  BV=0.71
    {79982,3328,1032, 0, 0, 0},  //TV = 3.64(2027 lines)  AV=2.97  SV=6.00  BV=0.62
    {79982,3584,1032, 0, 0, 0},  //TV = 3.64(2027 lines)  AV=2.97  SV=6.11  BV=0.51
    {79982,3840,1032, 0, 0, 0},  //TV = 3.64(2027 lines)  AV=2.97  SV=6.20  BV=0.41
    {79982,4096,1040, 0, 0, 0},  //TV = 3.64(2027 lines)  AV=2.97  SV=6.31  BV=0.31
    {79982,4352,1040, 0, 0, 0},  //TV = 3.64(2027 lines)  AV=2.97  SV=6.40  BV=0.22
    {90005,4224,1024, 0, 0, 0},  //TV = 3.47(2281 lines)  AV=2.97  SV=6.33  BV=0.11
    {99987,4096,1032, 0, 0, 0},  //TV = 3.32(2534 lines)  AV=2.97  SV=6.30  BV=-0.01
    {99987,4352,1040, 0, 0, 0},  //TV = 3.32(2534 lines)  AV=2.97  SV=6.40  BV=-0.10
    {110010,4224,1040, 0, 0, 0},  //TV = 3.18(2788 lines)  AV=2.97  SV=6.35  BV=-0.20
    {119992,4096,1056, 0, 0, 0},  //TV = 3.06(3041 lines)  AV=2.97  SV=6.33  BV=-0.30
    {119992,4480,1032, 0, 0, 0},  //TV = 3.06(3041 lines)  AV=2.97  SV=6.43  BV=-0.40
    {119992,4736,1048, 0, 0, 0},  //TV = 3.06(3041 lines)  AV=2.97  SV=6.53  BV=-0.50
    {119992,5120,1040, 0, 0, 0},  //TV = 3.06(3041 lines)  AV=2.97  SV=6.63  BV=-0.60
    {119992,5504,1040, 0, 0, 0},  //TV = 3.06(3041 lines)  AV=2.97  SV=6.74  BV=-0.71
    {119992,5888,1040, 0, 0, 0},  //TV = 3.06(3041 lines)  AV=2.97  SV=6.83  BV=-0.80
    {119992,6144,1064, 0, 0, 0},  //TV = 3.06(3041 lines)  AV=2.97  SV=6.93  BV=-0.90
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
},
};

static strAETable g_AE_SceneTable5 =
{
    AETABLE_SCENE_INDEX5,    //eAETableID
    119,    //u4TotalIndex
    20,    //u4StrobeTrigerBV
    109,    //i4MaxBV
    -9,    //i4MinBV
    90,    //i4EffectiveMaxBV
    0,      //i4EffectiveMinBV
    LIB3A_AE_ISO_SPEED_AUTO, //ISO SPEED
    sAEScene5PLineTable_60Hz,
    sAEScene5PLineTable_50Hz,
    NULL,
};

static strEvPline sAEScene6PLineTable_60Hz =
{
{
    {198,1140,1024, 0, 0, 0},  //TV = 12.30(5 lines)  AV=2.97  SV=4.44  BV=10.83
    {198,1140,1040, 0, 0, 0},  //TV = 12.30(5 lines)  AV=2.97  SV=4.46  BV=10.81
    {198,1216,1048, 0, 0, 0},  //TV = 12.30(5 lines)  AV=2.97  SV=4.57  BV=10.70
    {198,1328,1024, 0, 0, 0},  //TV = 12.30(5 lines)  AV=2.97  SV=4.66  BV=10.61
    {237,1140,1064, 0, 0, 0},  //TV = 12.04(6 lines)  AV=2.97  SV=4.50  BV=10.52
    {237,1216,1072, 0, 0, 0},  //TV = 12.04(6 lines)  AV=2.97  SV=4.60  BV=10.41
    {277,1140,1056, 0, 0, 0},  //TV = 11.82(7 lines)  AV=2.97  SV=4.49  BV=10.30
    {277,1216,1056, 0, 0, 0},  //TV = 11.82(7 lines)  AV=2.97  SV=4.58  BV=10.21
    {316,1140,1056, 0, 0, 0},  //TV = 11.63(8 lines)  AV=2.97  SV=4.49  BV=10.11
    {316,1216,1064, 0, 0, 0},  //TV = 11.63(8 lines)  AV=2.97  SV=4.59  BV=10.01
    {356,1140,1080, 0, 0, 0},  //TV = 11.46(9 lines)  AV=2.97  SV=4.52  BV=9.91
    {395,1140,1040, 0, 0, 0},  //TV = 11.31(10 lines)  AV=2.97  SV=4.46  BV=9.81
    {435,1140,1024, 0, 0, 0},  //TV = 11.17(11 lines)  AV=2.97  SV=4.44  BV=9.70
    {435,1140,1088, 0, 0, 0},  //TV = 11.17(11 lines)  AV=2.97  SV=4.53  BV=9.61
    {474,1140,1064, 0, 0, 0},  //TV = 11.04(12 lines)  AV=2.97  SV=4.50  BV=9.52
    {513,1140,1056, 0, 0, 0},  //TV = 10.93(13 lines)  AV=2.97  SV=4.49  BV=9.41
    {553,1140,1056, 0, 0, 0},  //TV = 10.82(14 lines)  AV=2.97  SV=4.49  BV=9.31
    {592,1140,1056, 0, 0, 0},  //TV = 10.72(15 lines)  AV=2.97  SV=4.49  BV=9.21
    {632,1140,1056, 0, 0, 0},  //TV = 10.63(16 lines)  AV=2.97  SV=4.49  BV=9.11
    {671,1140,1064, 0, 0, 0},  //TV = 10.54(17 lines)  AV=2.97  SV=4.50  BV=9.02
    {750,1140,1024, 0, 0, 0},  //TV = 10.38(19 lines)  AV=2.97  SV=4.44  BV=8.91
    {790,1140,1040, 0, 0, 0},  //TV = 10.31(20 lines)  AV=2.97  SV=4.46  BV=8.81
    {869,1140,1024, 0, 0, 0},  //TV = 10.17(22 lines)  AV=2.97  SV=4.44  BV=8.70
    {908,1140,1040, 0, 0, 0},  //TV = 10.11(23 lines)  AV=2.97  SV=4.46  BV=8.61
    {987,1140,1024, 0, 0, 0},  //TV = 9.98(25 lines)  AV=2.97  SV=4.44  BV=8.51
    {1066,1140,1024, 0, 0, 0},  //TV = 9.87(27 lines)  AV=2.97  SV=4.44  BV=8.40
    {1105,1140,1056, 0, 0, 0},  //TV = 9.82(28 lines)  AV=2.97  SV=4.49  BV=8.31
    {1224,1140,1024, 0, 0, 0},  //TV = 9.67(31 lines)  AV=2.97  SV=4.44  BV=8.20
    {1303,1140,1024, 0, 0, 0},  //TV = 9.58(33 lines)  AV=2.97  SV=4.44  BV=8.11
    {1382,1140,1040, 0, 0, 0},  //TV = 9.50(35 lines)  AV=2.97  SV=4.46  BV=8.01
    {1500,1140,1024, 0, 0, 0},  //TV = 9.38(38 lines)  AV=2.97  SV=4.44  BV=7.91
    {1618,1140,1024, 0, 0, 0},  //TV = 9.27(41 lines)  AV=2.97  SV=4.44  BV=7.80
    {1697,1140,1040, 0, 0, 0},  //TV = 9.20(43 lines)  AV=2.97  SV=4.46  BV=7.71
    {1855,1140,1024, 0, 0, 0},  //TV = 9.07(47 lines)  AV=2.97  SV=4.44  BV=7.60
    {1973,1140,1024, 0, 0, 0},  //TV = 8.99(50 lines)  AV=2.97  SV=4.44  BV=7.51
    {2131,1140,1024, 0, 0, 0},  //TV = 8.87(54 lines)  AV=2.97  SV=4.44  BV=7.40
    {2250,1140,1032, 0, 0, 0},  //TV = 8.80(57 lines)  AV=2.97  SV=4.45  BV=7.31
    {2447,1140,1024, 0, 0, 0},  //TV = 8.67(62 lines)  AV=2.97  SV=4.44  BV=7.20
    {2605,1140,1024, 0, 0, 0},  //TV = 8.58(66 lines)  AV=2.97  SV=4.44  BV=7.11
    {2802,1140,1024, 0, 0, 0},  //TV = 8.48(71 lines)  AV=2.97  SV=4.44  BV=7.01
    {2999,1140,1024, 0, 0, 0},  //TV = 8.38(76 lines)  AV=2.97  SV=4.44  BV=6.91
    {3236,1140,1024, 0, 0, 0},  //TV = 8.27(82 lines)  AV=2.97  SV=4.44  BV=6.80
    {3473,1140,1024, 0, 0, 0},  //TV = 8.17(88 lines)  AV=2.97  SV=4.44  BV=6.70
    {3710,1140,1024, 0, 0, 0},  //TV = 8.07(94 lines)  AV=2.97  SV=4.44  BV=6.60
    {3986,1140,1024, 0, 0, 0},  //TV = 7.97(101 lines)  AV=2.97  SV=4.44  BV=6.50
    {4262,1140,1024, 0, 0, 0},  //TV = 7.87(108 lines)  AV=2.97  SV=4.44  BV=6.40
    {4538,1140,1024, 0, 0, 0},  //TV = 7.78(115 lines)  AV=2.97  SV=4.44  BV=6.31
    {4893,1140,1024, 0, 0, 0},  //TV = 7.68(124 lines)  AV=2.97  SV=4.44  BV=6.20
    {5209,1140,1024, 0, 0, 0},  //TV = 7.58(132 lines)  AV=2.97  SV=4.44  BV=6.11
    {5643,1140,1024, 0, 0, 0},  //TV = 7.47(143 lines)  AV=2.97  SV=4.44  BV=6.00
    {6038,1140,1024, 0, 0, 0},  //TV = 7.37(153 lines)  AV=2.97  SV=4.44  BV=5.90
    {6472,1140,1024, 0, 0, 0},  //TV = 7.27(164 lines)  AV=2.97  SV=4.44  BV=5.80
    {6906,1140,1024, 0, 0, 0},  //TV = 7.18(175 lines)  AV=2.97  SV=4.44  BV=5.71
    {7419,1140,1024, 0, 0, 0},  //TV = 7.07(188 lines)  AV=2.97  SV=4.44  BV=5.60
    {7971,1140,1024, 0, 0, 0},  //TV = 6.97(202 lines)  AV=2.97  SV=4.44  BV=5.50
    {8326,1140,1048, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=4.48  BV=5.40
    {8326,1216,1048, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=4.57  BV=5.31
    {8326,1328,1024, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=4.66  BV=5.22
    {8326,1424,1024, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=4.76  BV=5.12
    {8326,1536,1024, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=4.87  BV=5.01
    {8326,1632,1032, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=4.97  BV=4.91
    {8326,1728,1040, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=5.06  BV=4.81
    {8326,1840,1048, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=5.17  BV=4.71
    {8326,1936,1072, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=5.27  BV=4.61
    {8326,2144,1032, 0, 0, 0},  //TV = 6.91(211 lines)  AV=2.97  SV=5.36  BV=4.51
    {16652,1140,1048, 0, 0, 0},  //TV = 5.91(422 lines)  AV=2.97  SV=4.48  BV=4.40
    {16652,1216,1048, 0, 0, 0},  //TV = 5.91(422 lines)  AV=2.97  SV=4.57  BV=4.31
    {16652,1328,1024, 0, 0, 0},  //TV = 5.91(422 lines)  AV=2.97  SV=4.66  BV=4.22
    {16652,1424,1024, 0, 0, 0},  //TV = 5.91(422 lines)  AV=2.97  SV=4.76  BV=4.12
    {16652,1424,1104, 0, 0, 0},  //TV = 5.91(422 lines)  AV=2.97  SV=4.87  BV=4.01
    {16652,1632,1032, 0, 0, 0},  //TV = 5.91(422 lines)  AV=2.97  SV=4.97  BV=3.91
    {25017,1140,1056, 0, 0, 0},  //TV = 5.32(634 lines)  AV=2.97  SV=4.49  BV=3.81
    {25017,1216,1064, 0, 0, 0},  //TV = 5.32(634 lines)  AV=2.97  SV=4.59  BV=3.70
    {25017,1328,1048, 0, 0, 0},  //TV = 5.32(634 lines)  AV=2.97  SV=4.70  BV=3.60
    {25017,1424,1040, 0, 0, 0},  //TV = 5.32(634 lines)  AV=2.97  SV=4.78  BV=3.51
    {33343,1140,1048, 0, 0, 0},  //TV = 4.91(845 lines)  AV=2.97  SV=4.48  BV=3.40
    {33343,1216,1048, 0, 0, 0},  //TV = 4.91(845 lines)  AV=2.97  SV=4.57  BV=3.31
    {33343,1328,1024, 0, 0, 0},  //TV = 4.91(845 lines)  AV=2.97  SV=4.66  BV=3.22
    {41669,1140,1024, 0, 0, 0},  //TV = 4.58(1056 lines)  AV=2.97  SV=4.44  BV=3.11
    {41669,1216,1032, 0, 0, 0},  //TV = 4.58(1056 lines)  AV=2.97  SV=4.55  BV=3.01
    {41669,1216,1104, 0, 0, 0},  //TV = 4.58(1056 lines)  AV=2.97  SV=4.64  BV=2.91
    {49994,1140,1056, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.49  BV=2.81
    {49994,1216,1064, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.59  BV=2.70
    {49994,1328,1040, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.68  BV=2.61
    {49994,1424,1040, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.78  BV=2.51
    {49994,1536,1032, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.88  BV=2.41
    {49994,1632,1040, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.98  BV=2.31
    {49994,1728,1056, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.09  BV=2.21
    {49994,1840,1064, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.19  BV=2.11
    {49994,2048,1024, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.29  BV=2.01
    {49994,2144,1048, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.39  BV=1.91
    {49994,2240,1072, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.48  BV=1.81
    {49994,2448,1048, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.58  BV=1.72
    {49994,2656,1040, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.68  BV=1.61
    {49994,2864,1032, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.78  BV=1.51
    {49994,3072,1032, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.88  BV=1.41
    {49994,3328,1024, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.99  BV=1.31
    {49994,3456,1056, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=6.09  BV=1.21
    {58319,3200,1048, 0, 0, 0},  //TV = 4.10(1478 lines)  AV=2.97  SV=5.96  BV=1.11
    {66645,3072,1024, 0, 0, 0},  //TV = 3.91(1689 lines)  AV=2.97  SV=5.87  BV=1.01
    {66645,3200,1056, 0, 0, 0},  //TV = 3.91(1689 lines)  AV=2.97  SV=5.98  BV=0.90
    {75011,3072,1048, 0, 0, 0},  //TV = 3.74(1901 lines)  AV=2.97  SV=5.91  BV=0.80
    {75011,3328,1032, 0, 0, 0},  //TV = 3.74(1901 lines)  AV=2.97  SV=6.00  BV=0.71
    {83336,3200,1032, 0, 0, 0},  //TV = 3.58(2112 lines)  AV=2.97  SV=5.94  BV=0.61
    {83336,3456,1024, 0, 0, 0},  //TV = 3.58(2112 lines)  AV=2.97  SV=6.04  BV=0.51
    {83336,3712,1024, 0, 0, 0},  //TV = 3.58(2112 lines)  AV=2.97  SV=6.14  BV=0.41
    {83336,3968,1032, 0, 0, 0},  //TV = 3.58(2112 lines)  AV=2.97  SV=6.25  BV=0.30
    {83336,4224,1032, 0, 0, 0},  //TV = 3.58(2112 lines)  AV=2.97  SV=6.34  BV=0.21
    {91661,4096,1040, 0, 0, 0},  //TV = 3.45(2323 lines)  AV=2.97  SV=6.31  BV=0.11
    {99987,4096,1032, 0, 0, 0},  //TV = 3.32(2534 lines)  AV=2.97  SV=6.30  BV=-0.01
    {108313,4096,1024, 0, 0, 0},  //TV = 3.21(2745 lines)  AV=2.97  SV=6.29  BV=-0.11
    {108313,4352,1032, 0, 0, 0},  //TV = 3.21(2745 lines)  AV=2.97  SV=6.39  BV=-0.21
    {116678,4224,1048, 0, 0, 0},  //TV = 3.10(2957 lines)  AV=2.97  SV=6.36  BV=-0.29
    {125004,4224,1048, 0, 0, 0},  //TV = 3.00(3168 lines)  AV=2.97  SV=6.36  BV=-0.39
    {141655,4096,1032, 0, 0, 0},  //TV = 2.82(3590 lines)  AV=2.97  SV=6.30  BV=-0.51
    {149980,4096,1040, 0, 0, 0},  //TV = 2.74(3801 lines)  AV=2.97  SV=6.31  BV=-0.60
    {158346,4096,1056, 0, 0, 0},  //TV = 2.66(4013 lines)  AV=2.97  SV=6.33  BV=-0.70
    {166672,4224,1048, 0, 0, 0},  //TV = 2.58(4224 lines)  AV=2.97  SV=6.36  BV=-0.81
    {166672,4608,1024, 0, 0, 0},  //TV = 2.58(4224 lines)  AV=2.97  SV=6.46  BV=-0.90
    {166672,4864,1040, 0, 0, 0},  //TV = 2.58(4224 lines)  AV=2.97  SV=6.56  BV=-1.00
    {166672,5248,1032, 0, 0, 0},  //TV = 2.58(4224 lines)  AV=2.97  SV=6.66  BV=-1.10
    {166672,5632,1032, 0, 0, 0},  //TV = 2.58(4224 lines)  AV=2.97  SV=6.76  BV=-1.20
    {166672,6016,1032, 0, 0, 0},  //TV = 2.58(4224 lines)  AV=2.97  SV=6.85  BV=-1.30
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
},
};

static strEvPline sAEScene6PLineTable_50Hz =
{
{
    {198,1140,1024, 0, 0, 0},  //TV = 12.30(5 lines)  AV=2.97  SV=4.44  BV=10.83
    {198,1140,1040, 0, 0, 0},  //TV = 12.30(5 lines)  AV=2.97  SV=4.46  BV=10.81
    {198,1216,1048, 0, 0, 0},  //TV = 12.30(5 lines)  AV=2.97  SV=4.57  BV=10.70
    {198,1328,1024, 0, 0, 0},  //TV = 12.30(5 lines)  AV=2.97  SV=4.66  BV=10.61
    {237,1140,1064, 0, 0, 0},  //TV = 12.04(6 lines)  AV=2.97  SV=4.50  BV=10.52
    {237,1216,1072, 0, 0, 0},  //TV = 12.04(6 lines)  AV=2.97  SV=4.60  BV=10.41
    {277,1140,1056, 0, 0, 0},  //TV = 11.82(7 lines)  AV=2.97  SV=4.49  BV=10.30
    {277,1216,1056, 0, 0, 0},  //TV = 11.82(7 lines)  AV=2.97  SV=4.58  BV=10.21
    {316,1140,1056, 0, 0, 0},  //TV = 11.63(8 lines)  AV=2.97  SV=4.49  BV=10.11
    {316,1216,1064, 0, 0, 0},  //TV = 11.63(8 lines)  AV=2.97  SV=4.59  BV=10.01
    {356,1140,1080, 0, 0, 0},  //TV = 11.46(9 lines)  AV=2.97  SV=4.52  BV=9.91
    {395,1140,1040, 0, 0, 0},  //TV = 11.31(10 lines)  AV=2.97  SV=4.46  BV=9.81
    {435,1140,1024, 0, 0, 0},  //TV = 11.17(11 lines)  AV=2.97  SV=4.44  BV=9.70
    {435,1140,1088, 0, 0, 0},  //TV = 11.17(11 lines)  AV=2.97  SV=4.53  BV=9.61
    {474,1140,1064, 0, 0, 0},  //TV = 11.04(12 lines)  AV=2.97  SV=4.50  BV=9.52
    {513,1140,1056, 0, 0, 0},  //TV = 10.93(13 lines)  AV=2.97  SV=4.49  BV=9.41
    {553,1140,1056, 0, 0, 0},  //TV = 10.82(14 lines)  AV=2.97  SV=4.49  BV=9.31
    {592,1140,1056, 0, 0, 0},  //TV = 10.72(15 lines)  AV=2.97  SV=4.49  BV=9.21
    {632,1140,1056, 0, 0, 0},  //TV = 10.63(16 lines)  AV=2.97  SV=4.49  BV=9.11
    {671,1140,1064, 0, 0, 0},  //TV = 10.54(17 lines)  AV=2.97  SV=4.50  BV=9.02
    {750,1140,1024, 0, 0, 0},  //TV = 10.38(19 lines)  AV=2.97  SV=4.44  BV=8.91
    {790,1140,1040, 0, 0, 0},  //TV = 10.31(20 lines)  AV=2.97  SV=4.46  BV=8.81
    {869,1140,1024, 0, 0, 0},  //TV = 10.17(22 lines)  AV=2.97  SV=4.44  BV=8.70
    {908,1140,1040, 0, 0, 0},  //TV = 10.11(23 lines)  AV=2.97  SV=4.46  BV=8.61
    {987,1140,1024, 0, 0, 0},  //TV = 9.98(25 lines)  AV=2.97  SV=4.44  BV=8.51
    {1066,1140,1024, 0, 0, 0},  //TV = 9.87(27 lines)  AV=2.97  SV=4.44  BV=8.40
    {1105,1140,1056, 0, 0, 0},  //TV = 9.82(28 lines)  AV=2.97  SV=4.49  BV=8.31
    {1224,1140,1024, 0, 0, 0},  //TV = 9.67(31 lines)  AV=2.97  SV=4.44  BV=8.20
    {1303,1140,1024, 0, 0, 0},  //TV = 9.58(33 lines)  AV=2.97  SV=4.44  BV=8.11
    {1382,1140,1040, 0, 0, 0},  //TV = 9.50(35 lines)  AV=2.97  SV=4.46  BV=8.01
    {1500,1140,1024, 0, 0, 0},  //TV = 9.38(38 lines)  AV=2.97  SV=4.44  BV=7.91
    {1618,1140,1024, 0, 0, 0},  //TV = 9.27(41 lines)  AV=2.97  SV=4.44  BV=7.80
    {1697,1140,1040, 0, 0, 0},  //TV = 9.20(43 lines)  AV=2.97  SV=4.46  BV=7.71
    {1855,1140,1024, 0, 0, 0},  //TV = 9.07(47 lines)  AV=2.97  SV=4.44  BV=7.60
    {1973,1140,1024, 0, 0, 0},  //TV = 8.99(50 lines)  AV=2.97  SV=4.44  BV=7.51
    {2131,1140,1024, 0, 0, 0},  //TV = 8.87(54 lines)  AV=2.97  SV=4.44  BV=7.40
    {2250,1140,1032, 0, 0, 0},  //TV = 8.80(57 lines)  AV=2.97  SV=4.45  BV=7.31
    {2447,1140,1024, 0, 0, 0},  //TV = 8.67(62 lines)  AV=2.97  SV=4.44  BV=7.20
    {2605,1140,1024, 0, 0, 0},  //TV = 8.58(66 lines)  AV=2.97  SV=4.44  BV=7.11
    {2802,1140,1024, 0, 0, 0},  //TV = 8.48(71 lines)  AV=2.97  SV=4.44  BV=7.01
    {2999,1140,1024, 0, 0, 0},  //TV = 8.38(76 lines)  AV=2.97  SV=4.44  BV=6.91
    {3236,1140,1024, 0, 0, 0},  //TV = 8.27(82 lines)  AV=2.97  SV=4.44  BV=6.80
    {3473,1140,1024, 0, 0, 0},  //TV = 8.17(88 lines)  AV=2.97  SV=4.44  BV=6.70
    {3710,1140,1024, 0, 0, 0},  //TV = 8.07(94 lines)  AV=2.97  SV=4.44  BV=6.60
    {3986,1140,1024, 0, 0, 0},  //TV = 7.97(101 lines)  AV=2.97  SV=4.44  BV=6.50
    {4262,1140,1024, 0, 0, 0},  //TV = 7.87(108 lines)  AV=2.97  SV=4.44  BV=6.40
    {4538,1140,1024, 0, 0, 0},  //TV = 7.78(115 lines)  AV=2.97  SV=4.44  BV=6.31
    {4893,1140,1024, 0, 0, 0},  //TV = 7.68(124 lines)  AV=2.97  SV=4.44  BV=6.20
    {5209,1140,1024, 0, 0, 0},  //TV = 7.58(132 lines)  AV=2.97  SV=4.44  BV=6.11
    {5643,1140,1024, 0, 0, 0},  //TV = 7.47(143 lines)  AV=2.97  SV=4.44  BV=6.00
    {6038,1140,1024, 0, 0, 0},  //TV = 7.37(153 lines)  AV=2.97  SV=4.44  BV=5.90
    {6472,1140,1024, 0, 0, 0},  //TV = 7.27(164 lines)  AV=2.97  SV=4.44  BV=5.80
    {6906,1140,1024, 0, 0, 0},  //TV = 7.18(175 lines)  AV=2.97  SV=4.44  BV=5.71
    {7419,1140,1024, 0, 0, 0},  //TV = 7.07(188 lines)  AV=2.97  SV=4.44  BV=5.60
    {7971,1140,1024, 0, 0, 0},  //TV = 6.97(202 lines)  AV=2.97  SV=4.44  BV=5.50
    {8484,1140,1024, 0, 0, 0},  //TV = 6.88(215 lines)  AV=2.97  SV=4.44  BV=5.41
    {9155,1140,1024, 0, 0, 0},  //TV = 6.77(232 lines)  AV=2.97  SV=4.44  BV=5.30
    {9826,1140,1024, 0, 0, 0},  //TV = 6.67(249 lines)  AV=2.97  SV=4.44  BV=5.20
    {9983,1140,1072, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=4.51  BV=5.11
    {9983,1216,1072, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=4.60  BV=5.02
    {9983,1328,1056, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=4.71  BV=4.91
    {9983,1424,1056, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=4.81  BV=4.81
    {9983,1536,1048, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=4.91  BV=4.71
    {9983,1632,1064, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=5.01  BV=4.60
    {9983,1728,1072, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=5.11  BV=4.51
    {9983,1936,1024, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=5.21  BV=4.41
    {9983,2048,1040, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=5.31  BV=4.31
    {9983,2144,1064, 0, 0, 0},  //TV = 6.65(253 lines)  AV=2.97  SV=5.41  BV=4.21
    {20006,1140,1064, 0, 0, 0},  //TV = 5.64(507 lines)  AV=2.97  SV=4.50  BV=4.12
    {20006,1216,1072, 0, 0, 0},  //TV = 5.64(507 lines)  AV=2.97  SV=4.60  BV=4.01
    {20006,1328,1048, 0, 0, 0},  //TV = 5.64(507 lines)  AV=2.97  SV=4.70  BV=3.92
    {20006,1424,1064, 0, 0, 0},  //TV = 5.64(507 lines)  AV=2.97  SV=4.82  BV=3.80
    {20006,1536,1048, 0, 0, 0},  //TV = 5.64(507 lines)  AV=2.97  SV=4.91  BV=3.71
    {20006,1632,1056, 0, 0, 0},  //TV = 5.64(507 lines)  AV=2.97  SV=5.00  BV=3.61
    {29989,1140,1080, 0, 0, 0},  //TV = 5.06(760 lines)  AV=2.97  SV=4.52  BV=3.51
    {29989,1216,1096, 0, 0, 0},  //TV = 5.06(760 lines)  AV=2.97  SV=4.63  BV=3.40
    {29989,1328,1064, 0, 0, 0},  //TV = 5.06(760 lines)  AV=2.97  SV=4.72  BV=3.31
    {29989,1424,1072, 0, 0, 0},  //TV = 5.06(760 lines)  AV=2.97  SV=4.83  BV=3.20
    {40011,1140,1064, 0, 0, 0},  //TV = 4.64(1014 lines)  AV=2.97  SV=4.50  BV=3.12
    {40011,1216,1072, 0, 0, 0},  //TV = 4.64(1014 lines)  AV=2.97  SV=4.60  BV=3.01
    {40011,1328,1048, 0, 0, 0},  //TV = 4.64(1014 lines)  AV=2.97  SV=4.70  BV=2.92
    {49994,1140,1056, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.49  BV=2.81
    {49994,1216,1064, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.59  BV=2.70
    {49994,1328,1040, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.68  BV=2.61
    {49994,1424,1040, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.78  BV=2.51
    {49994,1536,1032, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.88  BV=2.41
    {49994,1632,1040, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=4.98  BV=2.31
    {49994,1728,1056, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.09  BV=2.21
    {49994,1840,1064, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.19  BV=2.11
    {49994,2048,1024, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.29  BV=2.01
    {49994,2144,1048, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.39  BV=1.91
    {49994,2240,1072, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.48  BV=1.81
    {49994,2448,1048, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.58  BV=1.72
    {49994,2656,1040, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.68  BV=1.61
    {49994,2864,1032, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.78  BV=1.51
    {49994,3072,1032, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.88  BV=1.41
    {49994,3328,1024, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=5.99  BV=1.31
    {49994,3456,1056, 0, 0, 0},  //TV = 4.32(1267 lines)  AV=2.97  SV=6.09  BV=1.21
    {60017,3072,1064, 0, 0, 0},  //TV = 4.06(1521 lines)  AV=2.97  SV=5.93  BV=1.10
    {60017,3328,1048, 0, 0, 0},  //TV = 4.06(1521 lines)  AV=2.97  SV=6.02  BV=1.01
    {69999,3072,1048, 0, 0, 0},  //TV = 3.84(1774 lines)  AV=2.97  SV=5.91  BV=0.90
    {69999,3328,1032, 0, 0, 0},  //TV = 3.84(1774 lines)  AV=2.97  SV=6.00  BV=0.81
    {79982,3072,1048, 0, 0, 0},  //TV = 3.64(2027 lines)  AV=2.97  SV=5.91  BV=0.71
    {79982,3328,1032, 0, 0, 0},  //TV = 3.64(2027 lines)  AV=2.97  SV=6.00  BV=0.62
    {79982,3584,1032, 0, 0, 0},  //TV = 3.64(2027 lines)  AV=2.97  SV=6.11  BV=0.51
    {79982,3840,1032, 0, 0, 0},  //TV = 3.64(2027 lines)  AV=2.97  SV=6.20  BV=0.41
    {79982,4096,1040, 0, 0, 0},  //TV = 3.64(2027 lines)  AV=2.97  SV=6.31  BV=0.31
    {79982,4352,1040, 0, 0, 0},  //TV = 3.64(2027 lines)  AV=2.97  SV=6.40  BV=0.22
    {90005,4224,1024, 0, 0, 0},  //TV = 3.47(2281 lines)  AV=2.97  SV=6.33  BV=0.11
    {99987,4096,1032, 0, 0, 0},  //TV = 3.32(2534 lines)  AV=2.97  SV=6.30  BV=-0.01
    {99987,4352,1040, 0, 0, 0},  //TV = 3.32(2534 lines)  AV=2.97  SV=6.40  BV=-0.10
    {110010,4224,1040, 0, 0, 0},  //TV = 3.18(2788 lines)  AV=2.97  SV=6.35  BV=-0.20
    {119992,4096,1056, 0, 0, 0},  //TV = 3.06(3041 lines)  AV=2.97  SV=6.33  BV=-0.30
    {130015,4096,1040, 0, 0, 0},  //TV = 2.94(3295 lines)  AV=2.97  SV=6.31  BV=-0.40
    {139997,4096,1040, 0, 0, 0},  //TV = 2.84(3548 lines)  AV=2.97  SV=6.31  BV=-0.50
    {149980,4096,1040, 0, 0, 0},  //TV = 2.74(3801 lines)  AV=2.97  SV=6.31  BV=-0.60
    {160003,4096,1040, 0, 0, 0},  //TV = 2.64(4055 lines)  AV=2.97  SV=6.31  BV=-0.69
    {160003,4480,1024, 0, 0, 0},  //TV = 2.64(4055 lines)  AV=2.97  SV=6.42  BV=-0.80
    {160003,4736,1040, 0, 0, 0},  //TV = 2.64(4055 lines)  AV=2.97  SV=6.52  BV=-0.90
    {160003,5120,1032, 0, 0, 0},  //TV = 2.64(4055 lines)  AV=2.97  SV=6.62  BV=-1.01
    {160003,5504,1024, 0, 0, 0},  //TV = 2.64(4055 lines)  AV=2.97  SV=6.71  BV=-1.10
    {160003,5888,1024, 0, 0, 0},  //TV = 2.64(4055 lines)  AV=2.97  SV=6.81  BV=-1.20
    {160003,6144,1056, 0, 0, 0},  //TV = 2.64(4055 lines)  AV=2.97  SV=6.92  BV=-1.30
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
},
};

static strAETable g_AE_SceneTable6 =
{
    AETABLE_SCENE_INDEX6,    //eAETableID
    123,    //u4TotalIndex
    20,    //u4StrobeTrigerBV
    109,    //i4MaxBV
    -13,    //i4MinBV
    90,    //i4EffectiveMaxBV
    0,      //i4EffectiveMinBV
    LIB3A_AE_ISO_SPEED_AUTO, //ISO SPEED
    sAEScene6PLineTable_60Hz,
    sAEScene6PLineTable_50Hz,
    NULL,
};

static strEvPline sAESceneReservePLineTable =
{
{
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
    {0, 0, 0, 0, 0, 0},  //reserved no used
},
};
static strAETable g_AE_ReserveSceneTable =
{
	AETABLE_SCENE_MAX,    //eAETableID
	0,    //u4TotalIndex
	20,    //u4StrobeTrigerBV
	0,    //i4MaxBV
	0,    //i4MinBV
	90,    //i4EffectiveMaxBV
	0,      //i4EffectiveMinBV
	LIB3A_AE_ISO_SPEED_AUTO, //ISO SPEED
	sAESceneReservePLineTable,
	sAESceneReservePLineTable,
	NULL,
};
static strAESceneMapping g_AEScenePLineMapping = 
{
{
    {LIB3A_AE_SCENE_AUTO, {AETABLE_RPEVIEW_AUTO, AETABLE_CAPTURE_AUTO, AETABLE_VIDEO_AUTO, AETABLE_VIDEO1_AUTO, AETABLE_VIDEO2_AUTO, AETABLE_CUSTOM1_AUTO, AETABLE_CUSTOM2_AUTO, AETABLE_CUSTOM3_AUTO, AETABLE_CUSTOM4_AUTO, AETABLE_CUSTOM5_AUTO, AETABLE_SCENE_INDEX5}},
    {LIB3A_AE_SCENE_NIGHT, {AETABLE_SCENE_INDEX6, AETABLE_SCENE_INDEX6, AETABLE_VIDEO_NIGHT, AETABLE_VIDEO_NIGHT, AETABLE_VIDEO_NIGHT, AETABLE_CUSTOM1_AUTO, AETABLE_CUSTOM2_AUTO, AETABLE_CUSTOM3_AUTO, AETABLE_CUSTOM4_AUTO, AETABLE_CUSTOM5_AUTO, AETABLE_SCENE_INDEX1}},
    {LIB3A_AE_SCENE_ACTION, {AETABLE_RPEVIEW_AUTO, AETABLE_SCENE_INDEX2, AETABLE_VIDEO_AUTO, AETABLE_VIDEO1_AUTO, AETABLE_VIDEO2_AUTO, AETABLE_CUSTOM1_AUTO, AETABLE_CUSTOM2_AUTO, AETABLE_CUSTOM3_AUTO, AETABLE_CUSTOM4_AUTO, AETABLE_CUSTOM5_AUTO, AETABLE_SCENE_INDEX2}},
    {LIB3A_AE_SCENE_BEACH, {AETABLE_RPEVIEW_AUTO, AETABLE_SCENE_INDEX3, AETABLE_VIDEO_AUTO, AETABLE_VIDEO1_AUTO, AETABLE_VIDEO2_AUTO, AETABLE_CUSTOM1_AUTO, AETABLE_CUSTOM2_AUTO, AETABLE_CUSTOM3_AUTO, AETABLE_CUSTOM4_AUTO, AETABLE_CUSTOM5_AUTO, AETABLE_SCENE_INDEX3}},
    {LIB3A_AE_SCENE_CANDLELIGHT, {AETABLE_RPEVIEW_AUTO, AETABLE_SCENE_INDEX1, AETABLE_VIDEO_AUTO, AETABLE_VIDEO1_AUTO, AETABLE_VIDEO2_AUTO, AETABLE_CUSTOM1_AUTO, AETABLE_CUSTOM2_AUTO, AETABLE_CUSTOM3_AUTO, AETABLE_CUSTOM4_AUTO, AETABLE_CUSTOM5_AUTO, AETABLE_SCENE_INDEX1}},
    {LIB3A_AE_SCENE_FIREWORKS, {AETABLE_RPEVIEW_AUTO, AETABLE_SCENE_INDEX4, AETABLE_VIDEO_AUTO, AETABLE_VIDEO1_AUTO, AETABLE_VIDEO2_AUTO, AETABLE_CUSTOM1_AUTO, AETABLE_CUSTOM2_AUTO, AETABLE_CUSTOM3_AUTO, AETABLE_CUSTOM4_AUTO, AETABLE_CUSTOM5_AUTO, AETABLE_SCENE_INDEX4}},
    {LIB3A_AE_SCENE_LANDSCAPE, {AETABLE_RPEVIEW_AUTO, AETABLE_SCENE_INDEX3, AETABLE_VIDEO_AUTO, AETABLE_VIDEO1_AUTO, AETABLE_VIDEO2_AUTO, AETABLE_CUSTOM1_AUTO, AETABLE_CUSTOM2_AUTO, AETABLE_CUSTOM3_AUTO, AETABLE_CUSTOM4_AUTO, AETABLE_CUSTOM5_AUTO, AETABLE_SCENE_INDEX3}},
    {LIB3A_AE_SCENE_PORTRAIT, {AETABLE_RPEVIEW_AUTO, AETABLE_SCENE_INDEX2, AETABLE_VIDEO_AUTO, AETABLE_VIDEO1_AUTO, AETABLE_VIDEO2_AUTO, AETABLE_CUSTOM1_AUTO, AETABLE_CUSTOM2_AUTO, AETABLE_CUSTOM3_AUTO, AETABLE_CUSTOM4_AUTO, AETABLE_CUSTOM5_AUTO, AETABLE_SCENE_INDEX2}},
    {LIB3A_AE_SCENE_NIGHT_PORTRAIT, {AETABLE_SCENE_INDEX6, AETABLE_SCENE_INDEX6, AETABLE_VIDEO_NIGHT, AETABLE_VIDEO_NIGHT, AETABLE_VIDEO_NIGHT, AETABLE_CUSTOM1_AUTO, AETABLE_CUSTOM2_AUTO, AETABLE_CUSTOM3_AUTO, AETABLE_CUSTOM4_AUTO, AETABLE_CUSTOM5_AUTO, AETABLE_SCENE_INDEX1}},
    {LIB3A_AE_SCENE_PARTY, {AETABLE_RPEVIEW_AUTO, AETABLE_SCENE_INDEX1, AETABLE_VIDEO_AUTO, AETABLE_VIDEO1_AUTO, AETABLE_VIDEO2_AUTO, AETABLE_CUSTOM1_AUTO, AETABLE_CUSTOM2_AUTO, AETABLE_CUSTOM3_AUTO, AETABLE_CUSTOM4_AUTO, AETABLE_CUSTOM5_AUTO, AETABLE_SCENE_INDEX1}},
    {LIB3A_AE_SCENE_SNOW, {AETABLE_RPEVIEW_AUTO, AETABLE_SCENE_INDEX3, AETABLE_VIDEO_AUTO, AETABLE_VIDEO1_AUTO, AETABLE_VIDEO2_AUTO, AETABLE_CUSTOM1_AUTO, AETABLE_CUSTOM2_AUTO, AETABLE_CUSTOM3_AUTO, AETABLE_CUSTOM4_AUTO, AETABLE_CUSTOM5_AUTO, AETABLE_SCENE_INDEX3}},
    {LIB3A_AE_SCENE_SPORTS, {AETABLE_RPEVIEW_AUTO, AETABLE_SCENE_INDEX2, AETABLE_VIDEO_AUTO, AETABLE_VIDEO1_AUTO, AETABLE_VIDEO2_AUTO, AETABLE_CUSTOM1_AUTO, AETABLE_CUSTOM2_AUTO, AETABLE_CUSTOM3_AUTO, AETABLE_CUSTOM4_AUTO, AETABLE_CUSTOM5_AUTO, AETABLE_SCENE_INDEX2}},
    {LIB3A_AE_SCENE_STEADYPHOTO, {AETABLE_RPEVIEW_AUTO, AETABLE_SCENE_INDEX2, AETABLE_VIDEO_AUTO, AETABLE_VIDEO1_AUTO, AETABLE_VIDEO2_AUTO, AETABLE_CUSTOM1_AUTO, AETABLE_CUSTOM2_AUTO, AETABLE_CUSTOM3_AUTO, AETABLE_CUSTOM4_AUTO, AETABLE_CUSTOM5_AUTO, AETABLE_SCENE_INDEX2}},
    {LIB3A_AE_SCENE_SUNSET, {AETABLE_RPEVIEW_AUTO, AETABLE_SCENE_INDEX3, AETABLE_VIDEO_AUTO, AETABLE_VIDEO1_AUTO, AETABLE_VIDEO2_AUTO, AETABLE_CUSTOM1_AUTO, AETABLE_CUSTOM2_AUTO, AETABLE_CUSTOM3_AUTO, AETABLE_CUSTOM4_AUTO, AETABLE_CUSTOM5_AUTO, AETABLE_SCENE_INDEX3}},
    {LIB3A_AE_SCENE_THEATRE, {AETABLE_RPEVIEW_AUTO, AETABLE_SCENE_INDEX1, AETABLE_VIDEO_AUTO, AETABLE_VIDEO1_AUTO, AETABLE_VIDEO2_AUTO, AETABLE_CUSTOM1_AUTO, AETABLE_CUSTOM2_AUTO, AETABLE_CUSTOM3_AUTO, AETABLE_CUSTOM4_AUTO, AETABLE_CUSTOM5_AUTO, AETABLE_SCENE_INDEX1}},
    {LIB3A_AE_SCENE_ISO_ANTI_SHAKE, {AETABLE_RPEVIEW_AUTO, AETABLE_SCENE_INDEX2, AETABLE_VIDEO_AUTO, AETABLE_VIDEO1_AUTO, AETABLE_VIDEO2_AUTO, AETABLE_CUSTOM1_AUTO, AETABLE_CUSTOM2_AUTO, AETABLE_CUSTOM3_AUTO, AETABLE_CUSTOM4_AUTO, AETABLE_CUSTOM5_AUTO, AETABLE_SCENE_INDEX2}},
    {LIB3A_AE_SCENE_ISO100, {AETABLE_RPEVIEW_AUTO, AETABLE_CAPTURE_ISO100, AETABLE_VIDEO_AUTO, AETABLE_VIDEO1_AUTO, AETABLE_VIDEO2_AUTO, AETABLE_CUSTOM1_AUTO, AETABLE_CUSTOM2_AUTO, AETABLE_CUSTOM3_AUTO, AETABLE_CUSTOM4_AUTO, AETABLE_CUSTOM5_AUTO, AETABLE_CAPTURE_ISO100}},
    {LIB3A_AE_SCENE_ISO200, {AETABLE_RPEVIEW_AUTO, AETABLE_CAPTURE_ISO200, AETABLE_VIDEO_AUTO, AETABLE_VIDEO1_AUTO, AETABLE_VIDEO2_AUTO, AETABLE_CUSTOM1_AUTO, AETABLE_CUSTOM2_AUTO, AETABLE_CUSTOM3_AUTO, AETABLE_CUSTOM4_AUTO, AETABLE_CUSTOM5_AUTO, AETABLE_CAPTURE_ISO200}},
    {LIB3A_AE_SCENE_ISO400, {AETABLE_RPEVIEW_AUTO, AETABLE_CAPTURE_ISO400, AETABLE_VIDEO_AUTO, AETABLE_VIDEO1_AUTO, AETABLE_VIDEO2_AUTO, AETABLE_CUSTOM1_AUTO, AETABLE_CUSTOM2_AUTO, AETABLE_CUSTOM3_AUTO, AETABLE_CUSTOM4_AUTO, AETABLE_CUSTOM5_AUTO, AETABLE_CAPTURE_ISO400}},
    {LIB3A_AE_SCENE_ISO800, {AETABLE_RPEVIEW_AUTO, AETABLE_CAPTURE_ISO800, AETABLE_VIDEO_AUTO, AETABLE_VIDEO1_AUTO, AETABLE_VIDEO2_AUTO, AETABLE_CUSTOM1_AUTO, AETABLE_CUSTOM2_AUTO, AETABLE_CUSTOM3_AUTO, AETABLE_CUSTOM4_AUTO, AETABLE_CUSTOM5_AUTO, AETABLE_CAPTURE_ISO800}},
    {LIB3A_AE_SCENE_ISO1600 , {AETABLE_RPEVIEW_AUTO, AETABLE_CAPTURE_ISO1600, AETABLE_VIDEO_AUTO, AETABLE_VIDEO1_AUTO, AETABLE_VIDEO2_AUTO, AETABLE_CUSTOM1_AUTO, AETABLE_CUSTOM2_AUTO, AETABLE_CUSTOM3_AUTO, AETABLE_CUSTOM4_AUTO, AETABLE_CUSTOM5_AUTO, AETABLE_CAPTURE_ISO1600}},
    {LIB3A_AE_SCENE_UNSUPPORTED, {AETABLE_RPEVIEW_AUTO,AETABLE_CAPTURE_AUTO,AETABLE_VIDEO_AUTO,AETABLE_VIDEO1_AUTO,AETABLE_VIDEO2_AUTO,AETABLE_CUSTOM1_AUTO,AETABLE_CUSTOM2_AUTO,AETABLE_CUSTOM3_AUTO,AETABLE_CUSTOM4_AUTO,AETABLE_CUSTOM5_AUTO,AETABLE_CAPTURE_AUTO}},    //reserve for future
    {LIB3A_AE_SCENE_UNSUPPORTED, {AETABLE_RPEVIEW_AUTO,AETABLE_CAPTURE_AUTO,AETABLE_VIDEO_AUTO,AETABLE_VIDEO1_AUTO,AETABLE_VIDEO2_AUTO,AETABLE_CUSTOM1_AUTO,AETABLE_CUSTOM2_AUTO,AETABLE_CUSTOM3_AUTO,AETABLE_CUSTOM4_AUTO,AETABLE_CUSTOM5_AUTO,AETABLE_CAPTURE_AUTO}},    //reserve for future
    {LIB3A_AE_SCENE_UNSUPPORTED, {AETABLE_RPEVIEW_AUTO,AETABLE_CAPTURE_AUTO,AETABLE_VIDEO_AUTO,AETABLE_VIDEO1_AUTO,AETABLE_VIDEO2_AUTO,AETABLE_CUSTOM1_AUTO,AETABLE_CUSTOM2_AUTO,AETABLE_CUSTOM3_AUTO,AETABLE_CUSTOM4_AUTO,AETABLE_CUSTOM5_AUTO,AETABLE_CAPTURE_AUTO}},    //reserve for future
    {LIB3A_AE_SCENE_UNSUPPORTED, {AETABLE_RPEVIEW_AUTO,AETABLE_CAPTURE_AUTO,AETABLE_VIDEO_AUTO,AETABLE_VIDEO1_AUTO,AETABLE_VIDEO2_AUTO,AETABLE_CUSTOM1_AUTO,AETABLE_CUSTOM2_AUTO,AETABLE_CUSTOM3_AUTO,AETABLE_CUSTOM4_AUTO,AETABLE_CUSTOM5_AUTO,AETABLE_CAPTURE_AUTO}},    //reserve for future
    {LIB3A_AE_SCENE_UNSUPPORTED, {AETABLE_RPEVIEW_AUTO,AETABLE_CAPTURE_AUTO,AETABLE_VIDEO_AUTO,AETABLE_VIDEO1_AUTO,AETABLE_VIDEO2_AUTO,AETABLE_CUSTOM1_AUTO,AETABLE_CUSTOM2_AUTO,AETABLE_CUSTOM3_AUTO,AETABLE_CUSTOM4_AUTO,AETABLE_CUSTOM5_AUTO,AETABLE_CAPTURE_AUTO}},    //reserve for future
    {LIB3A_AE_SCENE_UNSUPPORTED, {AETABLE_RPEVIEW_AUTO,AETABLE_CAPTURE_AUTO,AETABLE_VIDEO_AUTO,AETABLE_VIDEO1_AUTO,AETABLE_VIDEO2_AUTO,AETABLE_CUSTOM1_AUTO,AETABLE_CUSTOM2_AUTO,AETABLE_CUSTOM3_AUTO,AETABLE_CUSTOM4_AUTO,AETABLE_CUSTOM5_AUTO,AETABLE_CAPTURE_AUTO}},    //reserve for future
    {LIB3A_AE_SCENE_UNSUPPORTED, {AETABLE_RPEVIEW_AUTO,AETABLE_CAPTURE_AUTO,AETABLE_VIDEO_AUTO,AETABLE_VIDEO1_AUTO,AETABLE_VIDEO2_AUTO,AETABLE_CUSTOM1_AUTO,AETABLE_CUSTOM2_AUTO,AETABLE_CUSTOM3_AUTO,AETABLE_CUSTOM4_AUTO,AETABLE_CUSTOM5_AUTO,AETABLE_CAPTURE_AUTO}},    //reserve for future
    {LIB3A_AE_SCENE_UNSUPPORTED, {AETABLE_RPEVIEW_AUTO,AETABLE_CAPTURE_AUTO,AETABLE_VIDEO_AUTO,AETABLE_VIDEO1_AUTO,AETABLE_VIDEO2_AUTO,AETABLE_CUSTOM1_AUTO,AETABLE_CUSTOM2_AUTO,AETABLE_CUSTOM3_AUTO,AETABLE_CUSTOM4_AUTO,AETABLE_CUSTOM5_AUTO,AETABLE_CAPTURE_AUTO}},    //reserve for future
    {LIB3A_AE_SCENE_UNSUPPORTED, {AETABLE_RPEVIEW_AUTO,AETABLE_CAPTURE_AUTO,AETABLE_VIDEO_AUTO,AETABLE_VIDEO1_AUTO,AETABLE_VIDEO2_AUTO,AETABLE_CUSTOM1_AUTO,AETABLE_CUSTOM2_AUTO,AETABLE_CUSTOM3_AUTO,AETABLE_CUSTOM4_AUTO,AETABLE_CUSTOM5_AUTO,AETABLE_CAPTURE_AUTO}},    //reserve for future
},
};

static strAEPLineTable g_strAEPlineTable =
{
{
// PLINE Table
g_AE_PreviewAutoTable,
g_AE_CaptureAutoTable,
g_AE_VideoAutoTable,
g_AE_Video1AutoTable,
g_AE_Video2AutoTable,
g_AE_Custom1AutoTable,
g_AE_Custom2AutoTable,
g_AE_Custom3AutoTable,
g_AE_Custom4AutoTable,
g_AE_Custom5AutoTable,
g_AE_VideoNightTable,
g_AE_CaptureISO100Table,
g_AE_CaptureISO200Table,
g_AE_CaptureISO400Table,
g_AE_CaptureISO800Table,
g_AE_CaptureISO1600Table,
g_AE_StrobeTable,
g_AE_SceneTable1,
g_AE_SceneTable2,
g_AE_SceneTable3,
g_AE_SceneTable4,
g_AE_SceneTable5,
g_AE_SceneTable6,
g_AE_ReserveSceneTable,
g_AE_ReserveSceneTable,
g_AE_ReserveSceneTable,
g_AE_ReserveSceneTable,
g_AE_ReserveSceneTable,
g_AE_ReserveSceneTable,
g_AE_ReserveSceneTable,
g_AE_ReserveSceneTable,
g_AE_ReserveSceneTable,
g_AE_ReserveSceneTable,
g_AE_ReserveSceneTable,
g_AE_ReserveSceneTable,
g_AE_ReserveSceneTable,
g_AE_ReserveSceneTable,
g_AE_ReserveSceneTable,
g_AE_ReserveSceneTable,
g_AE_ReserveSceneTable,
g_AE_ReserveSceneTable,
g_AE_ReserveSceneTable,
g_AE_ReserveSceneTable,
g_AE_ReserveSceneTable,
g_AE_ReserveSceneTable,
g_AE_ReserveSceneTable,
g_AE_ReserveSceneTable,
g_AE_ReserveSceneTable,
g_AE_ReserveSceneTable,
g_AE_ReserveSceneTable,
},
};

static strAEPLineNumInfo g_strAEPreviewAutoPLineInfo =
{
    AETABLE_RPEVIEW_AUTO,
    90,
    -10,
    {
        {1,5000,20,1136,1136},
        {2,20,20,1136,3072},
        {3,20,12,3072,3072},
        {4,12,12,3072,4096},
        {5,12,8,4096,4096},
        {6,8,8,4096,6144},
        {7,0,0,0,0},
        {8,0,0,0,0},
        {9,0,0,0,0},
        {10,0,0,0,0},
        {11,0,0,0,0},
        {12,0,0,0,0},
        {13,0,0,0,0},
        {14,0,0,0,0},
        {15,0,0,0,0},
    }
};

static strAEPLineNumInfo g_strAECaptureAutoPLineInfo =
{
    AETABLE_CAPTURE_AUTO,
    90,
    0,
    {
        {1,5000,20,1136,1136},
        {2,20,20,1136,3072},
        {3,20,12,3072,3072},
        {4,12,12,3072,4096},
        {5,12,8,4096,4096},
        {6,8,8,4096,6144},
        {7,0,0,0,0},
        {8,0,0,0,0},
        {9,0,0,0,0},
        {10,0,0,0,0},
        {11,0,0,0,0},
        {12,0,0,0,0},
        {13,0,0,0,0},
        {14,0,0,0,0},
        {15,0,0,0,0},
    }
};

static strAEPLineNumInfo g_strAEVideoAutoPLineInfo =
{
    AETABLE_VIDEO_AUTO,
    90,
    0,
    {
        {1,5000,20,1136,1136},
        {2,20,20,1136,3072},
        {3,20,12,3072,3072},
        {4,12,12,3072,4096},
        {5,12,8,4096,4096},
        {6,8,8,4096,6144},
        {7,0,0,0,0},
        {8,0,0,0,0},
        {9,0,0,0,0},
        {10,0,0,0,0},
        {11,0,0,0,0},
        {12,0,0,0,0},
        {13,0,0,0,0},
        {14,0,0,0,0},
        {15,0,0,0,0},
    }
};

static strAEPLineNumInfo g_strAEVideo1AutoPLineInfo =
{
    AETABLE_VIDEO1_AUTO,
    90,
    0,
    {
        {1,5000,20,1136,1136},
        {2,20,20,1136,3072},
        {3,20,12,3072,3072},
        {4,12,12,3072,4096},
        {5,12,8,4096,4096},
        {6,8,8,4096,6144},
        {7,0,0,0,0},
        {8,0,0,0,0},
        {9,0,0,0,0},
        {10,0,0,0,0},
        {11,0,0,0,0},
        {12,0,0,0,0},
        {13,0,0,0,0},
        {14,0,0,0,0},
        {15,0,0,0,0},
    }
};

static strAEPLineNumInfo g_strAEVideo2AutoPLineInfo =
{
    AETABLE_VIDEO2_AUTO,
    90,
    0,
    {
        {1,5000,20,1136,1136},
        {2,20,20,1136,3072},
        {3,20,12,3072,3072},
        {4,12,12,3072,4096},
        {5,12,8,4096,4096},
        {6,8,8,4096,6144},
        {7,0,0,0,0},
        {8,0,0,0,0},
        {9,0,0,0,0},
        {10,0,0,0,0},
        {11,0,0,0,0},
        {12,0,0,0,0},
        {13,0,0,0,0},
        {14,0,0,0,0},
        {15,0,0,0,0},
    }
};

static strAEPLineNumInfo g_strAECustom1AutoPLineInfo =
{
    AETABLE_CUSTOM1_AUTO,
    90,
    0,
    {
        {1,5000,20,1136,1136},
        {2,20,20,1136,3072},
        {3,20,12,3072,3072},
        {4,12,12,3072,4096},
        {5,12,8,4096,4096},
        {6,8,8,4096,6144},
        {7,0,0,0,0},
        {8,0,0,0,0},
        {9,0,0,0,0},
        {10,0,0,0,0},
        {11,0,0,0,0},
        {12,0,0,0,0},
        {13,0,0,0,0},
        {14,0,0,0,0},
        {15,0,0,0,0},
    }
};

static strAEPLineNumInfo g_strAECustom2AutoPLineInfo =
{
    AETABLE_CUSTOM2_AUTO,
    90,
    0,
    {
        {1,5000,20,1136,1136},
        {2,20,20,1136,3072},
        {3,20,12,3072,3072},
        {4,12,12,3072,4096},
        {5,12,8,4096,4096},
        {6,8,8,4096,6144},
        {7,0,0,0,0},
        {8,0,0,0,0},
        {9,0,0,0,0},
        {10,0,0,0,0},
        {11,0,0,0,0},
        {12,0,0,0,0},
        {13,0,0,0,0},
        {14,0,0,0,0},
        {15,0,0,0,0},
    }
};

static strAEPLineNumInfo g_strAECustom3AutoPLineInfo =
{
    AETABLE_CUSTOM3_AUTO,
    90,
    0,
    {
        {1,5000,20,1136,1136},
        {2,20,20,1136,3072},
        {3,20,12,3072,3072},
        {4,12,12,3072,4096},
        {5,12,8,4096,4096},
        {6,8,8,4096,6144},
        {7,0,0,0,0},
        {8,0,0,0,0},
        {9,0,0,0,0},
        {10,0,0,0,0},
        {11,0,0,0,0},
        {12,0,0,0,0},
        {13,0,0,0,0},
        {14,0,0,0,0},
        {15,0,0,0,0},
    }
};

static strAEPLineNumInfo g_strAECustom4AutoPLineInfo =
{
    AETABLE_CUSTOM4_AUTO,
    90,
    0,
    {
        {1,5000,20,1136,1136},
        {2,20,20,1136,3072},
        {3,20,12,3072,3072},
        {4,12,12,3072,4096},
        {5,12,8,4096,4096},
        {6,8,8,4096,6144},
        {7,0,0,0,0},
        {8,0,0,0,0},
        {9,0,0,0,0},
        {10,0,0,0,0},
        {11,0,0,0,0},
        {12,0,0,0,0},
        {13,0,0,0,0},
        {14,0,0,0,0},
        {15,0,0,0,0},
    }
};

static strAEPLineNumInfo g_strAECustom5AutoPLineInfo =
{
    AETABLE_CUSTOM5_AUTO,
    90,
    0,
    {
        {1,5000,20,1136,1136},
        {2,20,20,1136,3072},
        {3,20,12,3072,3072},
        {4,12,12,3072,4096},
        {5,12,8,4096,4096},
        {6,8,8,4096,6144},
        {7,0,0,0,0},
        {8,0,0,0,0},
        {9,0,0,0,0},
        {10,0,0,0,0},
        {11,0,0,0,0},
        {12,0,0,0,0},
        {13,0,0,0,0},
        {14,0,0,0,0},
        {15,0,0,0,0},
    }
};

static strAEPLineNumInfo g_strAEVideoNightPLineInfo =
{
    AETABLE_VIDEO_NIGHT,
    90,
    -10,
    {
        {1,5000,20,1136,1136},
        {2,20,20,1136,3072},
        {3,20,12,3072,3072},
        {4,12,12,3072,4096},
        {5,12,6,4096,4096},
        {6,6,6,4096,6144},
        {7,0,0,0,0},
        {8,0,0,0,0},
        {9,0,0,0,0},
        {10,0,0,0,0},
        {11,0,0,0,0},
        {12,0,0,0,0},
        {13,0,0,0,0},
        {14,0,0,0,0},
        {15,0,0,0,0},
    }
};

static strAEPLineNumInfo g_strAECaptureISO100PLineInfo =
{
    AETABLE_CAPTURE_ISO100,
    90,
    0,
    {
        {1,5000,20,1136,1136},
        {2,20,20,1136,3072},
        {3,20,12,3072,3072},
        {4,12,12,3072,4096},
        {5,12,8,4096,4096},
        {6,8,8,4096,6144},
        {7,0,0,0,0},
        {8,0,0,0,0},
        {9,0,0,0,0},
        {10,0,0,0,0},
        {11,0,0,0,0},
        {12,0,0,0,0},
        {13,0,0,0,0},
        {14,0,0,0,0},
        {15,0,0,0,0},
    }
};

static strAEPLineNumInfo g_strAECaptureISO200PLineInfo =
{
    AETABLE_CAPTURE_ISO200,
    90,
    0,
    {
        {1,5000,20,1136,1136},
        {2,20,20,1136,3072},
        {3,20,12,3072,3072},
        {4,12,12,3072,4096},
        {5,12,8,4096,4096},
        {6,8,8,4096,6144},
        {7,0,0,0,0},
        {8,0,0,0,0},
        {9,0,0,0,0},
        {10,0,0,0,0},
        {11,0,0,0,0},
        {12,0,0,0,0},
        {13,0,0,0,0},
        {14,0,0,0,0},
        {15,0,0,0,0},
    }
};

static strAEPLineNumInfo g_strAECaptureISO400PLineInfo =
{
    AETABLE_CAPTURE_ISO400,
    90,
    0,
    {
        {1,5000,20,1136,1136},
        {2,20,20,1136,3072},
        {3,20,12,3072,3072},
        {4,12,12,3072,4096},
        {5,12,8,4096,4096},
        {6,8,8,4096,6144},
        {7,0,0,0,0},
        {8,0,0,0,0},
        {9,0,0,0,0},
        {10,0,0,0,0},
        {11,0,0,0,0},
        {12,0,0,0,0},
        {13,0,0,0,0},
        {14,0,0,0,0},
        {15,0,0,0,0},
    }
};

static strAEPLineNumInfo g_strAECaptureISO800PLineInfo =
{
    AETABLE_CAPTURE_ISO800,
    90,
    0,
    {
        {1,5000,20,1136,1136},
        {2,20,20,1136,3072},
        {3,20,12,3072,3072},
        {4,12,12,3072,4096},
        {5,12,8,4096,4096},
        {6,8,8,4096,6144},
        {7,0,0,0,0},
        {8,0,0,0,0},
        {9,0,0,0,0},
        {10,0,0,0,0},
        {11,0,0,0,0},
        {12,0,0,0,0},
        {13,0,0,0,0},
        {14,0,0,0,0},
        {15,0,0,0,0},
    }
};

static strAEPLineNumInfo g_strAECaptureISO1600PLineInfo =
{
    AETABLE_CAPTURE_ISO1600,
    90,
    0,
    {
        {1,5000,20,1136,1136},
        {2,20,20,1136,3072},
        {3,20,12,3072,3072},
        {4,12,12,3072,4096},
        {5,12,8,4096,4096},
        {6,8,8,4096,6144},
        {7,0,0,0,0},
        {8,0,0,0,0},
        {9,0,0,0,0},
        {10,0,0,0,0},
        {11,0,0,0,0},
        {12,0,0,0,0},
        {13,0,0,0,0},
        {14,0,0,0,0},
        {15,0,0,0,0},
    }
};

static strAEPLineNumInfo g_strAEStrobePLineInfo =
{
    AETABLE_STROBE,
    90,
    0,
    {
        {1,5000,20,1136,1136},
        {2,20,20,1136,3072},
        {3,20,12,3072,3072},
        {4,12,12,3072,4096},
        {5,12,8,4096,4096},
        {6,8,8,4096,6144},
        {7,0,0,0,0},
        {8,0,0,0,0},
        {9,0,0,0,0},
        {10,0,0,0,0},
        {11,0,0,0,0},
        {12,0,0,0,0},
        {13,0,0,0,0},
        {14,0,0,0,0},
        {15,0,0,0,0},
    }
};

static strAEPLineNumInfo g_strAEScene1PLineInfo =
{
    AETABLE_SCENE_INDEX1,
    90,
    -10,
    {
        {1,5000,20,1136,1136},
        {2,20,20,1136,3072},
        {3,20,12,3072,3072},
        {4,12,12,3072,4096},
        {5,12,8,4096,4096},
        {6,8,8,4096,6144},
        {7,0,0,0,0},
        {8,0,0,0,0},
        {9,0,0,0,0},
        {10,0,0,0,0},
        {11,0,0,0,0},
        {12,0,0,0,0},
        {13,0,0,0,0},
        {14,0,0,0,0},
        {15,0,0,0,0},
    }
};

static strAEPLineNumInfo g_strAEScene2PLineInfo =
{
    AETABLE_SCENE_INDEX2,
    90,
    0,
    {
        {1,5000,20,1136,1136},
        {2,20,20,1136,3072},
        {3,20,12,3072,3072},
        {4,12,12,3072,4096},
        {5,12,8,4096,4096},
        {6,8,8,4096,6144},
        {7,0,0,0,0},
        {8,0,0,0,0},
        {9,0,0,0,0},
        {10,0,0,0,0},
        {11,0,0,0,0},
        {12,0,0,0,0},
        {13,0,0,0,0},
        {14,0,0,0,0},
        {15,0,0,0,0},
    }
};

static strAEPLineNumInfo g_strAEScene3PLineInfo =
{
    AETABLE_SCENE_INDEX3,
    90,
    0,
    {
        {1,5000,20,1136,1136},
        {2,20,20,1136,3072},
        {3,20,12,3072,3072},
        {4,12,12,3072,4096},
        {5,12,8,4096,4096},
        {6,8,8,4096,6144},
        {7,0,0,0,0},
        {8,0,0,0,0},
        {9,0,0,0,0},
        {10,0,0,0,0},
        {11,0,0,0,0},
        {12,0,0,0,0},
        {13,0,0,0,0},
        {14,0,0,0,0},
        {15,0,0,0,0},
    }
};

static strAEPLineNumInfo g_strAEScene4PLineInfo =
{
    AETABLE_SCENE_INDEX4,
    90,
    0,
    {
        {1,5000,20,1136,1136},
        {2,20,20,1136,3072},
        {3,20,12,3072,3072},
        {4,12,12,3072,4096},
        {5,12,8,4096,4096},
        {6,8,8,4096,6144},
        {7,0,0,0,0},
        {8,0,0,0,0},
        {9,0,0,0,0},
        {10,0,0,0,0},
        {11,0,0,0,0},
        {12,0,0,0,0},
        {13,0,0,0,0},
        {14,0,0,0,0},
        {15,0,0,0,0},
    }
};

static strAEPLineNumInfo g_strAEScene5PLineInfo =
{
    AETABLE_SCENE_INDEX5,
    90,
    0,
    {
        {1,5000,20,1136,1136},
        {2,20,20,1136,3072},
        {3,20,12,3072,3072},
        {4,12,12,3072,4096},
        {5,12,8,4096,4096},
        {6,8,8,4096,6144},
        {7,0,0,0,0},
        {8,0,0,0,0},
        {9,0,0,0,0},
        {10,0,0,0,0},
        {11,0,0,0,0},
        {12,0,0,0,0},
        {13,0,0,0,0},
        {14,0,0,0,0},
        {15,0,0,0,0},
    }
};

static strAEPLineNumInfo g_strAEScene6PLineInfo =
{
    AETABLE_SCENE_INDEX6,
    90,
    0,
    {
        {1,5000,20,1136,1136},
        {2,20,20,1136,3072},
        {3,20,12,3072,3072},
        {4,12,12,3072,4096},
        {5,12,6,4096,4096},
        {6,6,6,4096,6144},
        {7,0,0,0,0},
        {8,0,0,0,0},
        {9,0,0,0,0},
        {10,0,0,0,0},
        {11,0,0,0,0},
        {12,0,0,0,0},
        {13,0,0,0,0},
        {14,0,0,0,0},
        {15,0,0,0,0},
    }
};

static strAEPLineNumInfo g_strAEScene7PLineInfo =
{
    AETABLE_SCENE_INDEX7,
    90,
    0,
    {
        {1,5000,20,1136,1136},
        {2,20,20,1136,3072},
        {3,20,12,3072,3072},
        {4,12,12,3072,4096},
        {5,12,8,4096,4096},
        {6,8,8,4096,6144},
        {7,0,0,0,0},
        {8,0,0,0,0},
        {9,0,0,0,0},
        {10,0,0,0,0},
        {11,0,0,0,0},
        {12,0,0,0,0},
        {13,0,0,0,0},
        {14,0,0,0,0},
        {15,0,0,0,0},
    }
};

static strAEPLineNumInfo g_strAEScene8PLineInfo =
{
    AETABLE_SCENE_INDEX8,
    90,
    0,
    {
        {1,5000,20,1136,1136},
        {2,20,20,1136,3072},
        {3,20,12,3072,3072},
        {4,12,12,3072,4096},
        {5,12,8,4096,4096},
        {6,8,8,4096,6144},
        {7,0,0,0,0},
        {8,0,0,0,0},
        {9,0,0,0,0},
        {10,0,0,0,0},
        {11,0,0,0,0},
        {12,0,0,0,0},
        {13,0,0,0,0},
        {14,0,0,0,0},
        {15,0,0,0,0},
    }
};

static strAEPLineNumInfo g_strAEScene9PLineInfo =
{
    AETABLE_SCENE_INDEX9,
    90,
    0,
    {
        {1,5000,20,1136,1136},
        {2,20,20,1136,3072},
        {3,20,12,3072,3072},
        {4,12,12,3072,4096},
        {5,12,8,4096,4096},
        {6,8,8,4096,6144},
        {7,0,0,0,0},
        {8,0,0,0,0},
        {9,0,0,0,0},
        {10,0,0,0,0},
        {11,0,0,0,0},
        {12,0,0,0,0},
        {13,0,0,0,0},
        {14,0,0,0,0},
        {15,0,0,0,0},
    }
};

static strAEPLineNumInfo g_strAEScene10PLineInfo =
{
    AETABLE_SCENE_INDEX10,
    90,
    0,
    {
        {1,5000,20,1136,1136},
        {2,20,20,1136,3072},
        {3,20,12,3072,3072},
        {4,12,12,3072,4096},
        {5,12,8,4096,4096},
        {6,8,8,4096,6144},
        {7,0,0,0,0},
        {8,0,0,0,0},
        {9,0,0,0,0},
        {10,0,0,0,0},
        {11,0,0,0,0},
        {12,0,0,0,0},
        {13,0,0,0,0},
        {14,0,0,0,0},
        {15,0,0,0,0},
    }
};

static strAEPLineNumInfo g_strAEScene11PLineInfo =
{
    AETABLE_SCENE_INDEX11,
    90,
    0,
    {
        {1,5000,20,1136,1136},
        {2,20,20,1136,3072},
        {3,20,12,3072,3072},
        {4,12,12,3072,4096},
        {5,12,8,4096,4096},
        {6,8,8,4096,6144},
        {7,0,0,0,0},
        {8,0,0,0,0},
        {9,0,0,0,0},
        {10,0,0,0,0},
        {11,0,0,0,0},
        {12,0,0,0,0},
        {13,0,0,0,0},
        {14,0,0,0,0},
        {15,0,0,0,0},
    }
};

static strAEPLineNumInfo g_strAEScene12PLineInfo =
{
    AETABLE_SCENE_INDEX12,
    90,
    0,
    {
        {1,5000,20,1136,1136},
        {2,20,20,1136,3072},
        {3,20,12,3072,3072},
        {4,12,12,3072,4096},
        {5,12,8,4096,4096},
        {6,8,8,4096,6144},
        {7,0,0,0,0},
        {8,0,0,0,0},
        {9,0,0,0,0},
        {10,0,0,0,0},
        {11,0,0,0,0},
        {12,0,0,0,0},
        {13,0,0,0,0},
        {14,0,0,0,0},
        {15,0,0,0,0},
    }
};

static strAEPLineNumInfo g_strAEScene13PLineInfo =
{
    AETABLE_SCENE_INDEX13,
    90,
    0,
    {
        {1,5000,20,1136,1136},
        {2,20,20,1136,3072},
        {3,20,12,3072,3072},
        {4,12,12,3072,4096},
        {5,12,8,4096,4096},
        {6,8,8,4096,6144},
        {7,0,0,0,0},
        {8,0,0,0,0},
        {9,0,0,0,0},
        {10,0,0,0,0},
        {11,0,0,0,0},
        {12,0,0,0,0},
        {13,0,0,0,0},
        {14,0,0,0,0},
        {15,0,0,0,0},
    }
};

static strAEPLineNumInfo g_strAEScene14PLineInfo =
{
    AETABLE_SCENE_INDEX14,
    90,
    0,
    {
        {1,5000,20,1136,1136},
        {2,20,20,1136,3072},
        {3,20,12,3072,3072},
        {4,12,12,3072,4096},
        {5,12,8,4096,4096},
        {6,8,8,4096,6144},
        {7,0,0,0,0},
        {8,0,0,0,0},
        {9,0,0,0,0},
        {10,0,0,0,0},
        {11,0,0,0,0},
        {12,0,0,0,0},
        {13,0,0,0,0},
        {14,0,0,0,0},
        {15,0,0,0,0},
    }
};

static strAEPLineNumInfo g_strAEScene15PLineInfo =
{
    AETABLE_SCENE_INDEX15,
    90,
    0,
    {
        {1,5000,20,1136,1136},
        {2,20,20,1136,3072},
        {3,20,12,3072,3072},
        {4,12,12,3072,4096},
        {5,12,8,4096,4096},
        {6,8,8,4096,6144},
        {7,0,0,0,0},
        {8,0,0,0,0},
        {9,0,0,0,0},
        {10,0,0,0,0},
        {11,0,0,0,0},
        {12,0,0,0,0},
        {13,0,0,0,0},
        {14,0,0,0,0},
        {15,0,0,0,0},
    }
};

static strAEPLineNumInfo g_strAEScene16PLineInfo =
{
    AETABLE_SCENE_INDEX16,
    90,
    0,
    {
        {1,5000,20,1136,1136},
        {2,20,20,1136,3072},
        {3,20,12,3072,3072},
        {4,12,12,3072,4096},
        {5,12,8,4096,4096},
        {6,8,8,4096,6144},
        {7,0,0,0,0},
        {8,0,0,0,0},
        {9,0,0,0,0},
        {10,0,0,0,0},
        {11,0,0,0,0},
        {12,0,0,0,0},
        {13,0,0,0,0},
        {14,0,0,0,0},
        {15,0,0,0,0},
    }
};

static strAEPLineNumInfo g_strAEScene17PLineInfo =
{
    AETABLE_SCENE_INDEX17,
    90,
    0,
    {
        {1,5000,20,1136,1136},
        {2,20,20,1136,3072},
        {3,20,12,3072,3072},
        {4,12,12,3072,4096},
        {5,12,8,4096,4096},
        {6,8,8,4096,6144},
        {7,0,0,0,0},
        {8,0,0,0,0},
        {9,0,0,0,0},
        {10,0,0,0,0},
        {11,0,0,0,0},
        {12,0,0,0,0},
        {13,0,0,0,0},
        {14,0,0,0,0},
        {15,0,0,0,0},
    }
};

static strAEPLineNumInfo g_strAEScene18PLineInfo =
{
    AETABLE_SCENE_INDEX18,
    90,
    0,
    {
        {1,5000,20,1136,1136},
        {2,20,20,1136,3072},
        {3,20,12,3072,3072},
        {4,12,12,3072,4096},
        {5,12,8,4096,4096},
        {6,8,8,4096,6144},
        {7,0,0,0,0},
        {8,0,0,0,0},
        {9,0,0,0,0},
        {10,0,0,0,0},
        {11,0,0,0,0},
        {12,0,0,0,0},
        {13,0,0,0,0},
        {14,0,0,0,0},
        {15,0,0,0,0},
    }
};

static strAEPLineNumInfo g_strAEScene19PLineInfo =
{
    AETABLE_SCENE_INDEX19,
    90,
    0,
    {
        {1,5000,20,1136,1136},
        {2,20,20,1136,3072},
        {3,20,12,3072,3072},
        {4,12,12,3072,4096},
        {5,12,8,4096,4096},
        {6,8,8,4096,6144},
        {7,0,0,0,0},
        {8,0,0,0,0},
        {9,0,0,0,0},
        {10,0,0,0,0},
        {11,0,0,0,0},
        {12,0,0,0,0},
        {13,0,0,0,0},
        {14,0,0,0,0},
        {15,0,0,0,0},
    }
};

static strAEPLineNumInfo g_strAEScene20PLineInfo =
{
    AETABLE_SCENE_INDEX20,
    90,
    0,
    {
        {1,5000,20,1136,1136},
        {2,20,20,1136,3072},
        {3,20,12,3072,3072},
        {4,12,12,3072,4096},
        {5,12,8,4096,4096},
        {6,8,8,4096,6144},
        {7,0,0,0,0},
        {8,0,0,0,0},
        {9,0,0,0,0},
        {10,0,0,0,0},
        {11,0,0,0,0},
        {12,0,0,0,0},
        {13,0,0,0,0},
        {14,0,0,0,0},
        {15,0,0,0,0},
    }
};

static strAEPLineNumInfo g_strAEScene21PLineInfo =
{
    AETABLE_SCENE_INDEX21,
    90,
    0,
    {
        {1,5000,20,1136,1136},
        {2,20,20,1136,3072},
        {3,20,12,3072,3072},
        {4,12,12,3072,4096},
        {5,12,8,4096,4096},
        {6,8,8,4096,6144},
        {7,0,0,0,0},
        {8,0,0,0,0},
        {9,0,0,0,0},
        {10,0,0,0,0},
        {11,0,0,0,0},
        {12,0,0,0,0},
        {13,0,0,0,0},
        {14,0,0,0,0},
        {15,0,0,0,0},
    }
};

static strAEPLineNumInfo g_strAEScene22PLineInfo =
{
    AETABLE_SCENE_INDEX22,
    90,
    0,
    {
        {1,5000,20,1136,1136},
        {2,20,20,1136,3072},
        {3,20,12,3072,3072},
        {4,12,12,3072,4096},
        {5,12,8,4096,4096},
        {6,8,8,4096,6144},
        {7,0,0,0,0},
        {8,0,0,0,0},
        {9,0,0,0,0},
        {10,0,0,0,0},
        {11,0,0,0,0},
        {12,0,0,0,0},
        {13,0,0,0,0},
        {14,0,0,0,0},
        {15,0,0,0,0},
    }
};

static strAEPLineNumInfo g_strAEScene23PLineInfo =
{
    AETABLE_SCENE_INDEX23,
    90,
    0,
    {
        {1,5000,20,1136,1136},
        {2,20,20,1136,3072},
        {3,20,12,3072,3072},
        {4,12,12,3072,4096},
        {5,12,8,4096,4096},
        {6,8,8,4096,6144},
        {7,0,0,0,0},
        {8,0,0,0,0},
        {9,0,0,0,0},
        {10,0,0,0,0},
        {11,0,0,0,0},
        {12,0,0,0,0},
        {13,0,0,0,0},
        {14,0,0,0,0},
        {15,0,0,0,0},
    }
};

static strAEPLineNumInfo g_strAEScene24PLineInfo =
{
    AETABLE_SCENE_INDEX24,
    90,
    0,
    {
        {1,5000,20,1136,1136},
        {2,20,20,1136,3072},
        {3,20,12,3072,3072},
        {4,12,12,3072,4096},
        {5,12,8,4096,4096},
        {6,8,8,4096,6144},
        {7,0,0,0,0},
        {8,0,0,0,0},
        {9,0,0,0,0},
        {10,0,0,0,0},
        {11,0,0,0,0},
        {12,0,0,0,0},
        {13,0,0,0,0},
        {14,0,0,0,0},
        {15,0,0,0,0},
    }
};

static strAEPLineNumInfo g_strAEScene25PLineInfo =
{
    AETABLE_SCENE_INDEX25,
    90,
    0,
    {
        {1,5000,20,1136,1136},
        {2,20,20,1136,3072},
        {3,20,12,3072,3072},
        {4,12,12,3072,4096},
        {5,12,8,4096,4096},
        {6,8,8,4096,6144},
        {7,0,0,0,0},
        {8,0,0,0,0},
        {9,0,0,0,0},
        {10,0,0,0,0},
        {11,0,0,0,0},
        {12,0,0,0,0},
        {13,0,0,0,0},
        {14,0,0,0,0},
        {15,0,0,0,0},
    }
};

static strAEPLineNumInfo g_strAEScene26PLineInfo =
{
    AETABLE_SCENE_INDEX26,
    90,
    0,
    {
        {1,5000,20,1136,1136},
        {2,20,20,1136,3072},
        {3,20,12,3072,3072},
        {4,12,12,3072,4096},
        {5,12,8,4096,4096},
        {6,8,8,4096,6144},
        {7,0,0,0,0},
        {8,0,0,0,0},
        {9,0,0,0,0},
        {10,0,0,0,0},
        {11,0,0,0,0},
        {12,0,0,0,0},
        {13,0,0,0,0},
        {14,0,0,0,0},
        {15,0,0,0,0},
    }
};

static strAEPLineNumInfo g_strAEScene27PLineInfo =
{
    AETABLE_SCENE_INDEX27,
    90,
    0,
    {
        {1,5000,20,1136,1136},
        {2,20,20,1136,3072},
        {3,20,12,3072,3072},
        {4,12,12,3072,4096},
        {5,12,8,4096,4096},
        {6,8,8,4096,6144},
        {7,0,0,0,0},
        {8,0,0,0,0},
        {9,0,0,0,0},
        {10,0,0,0,0},
        {11,0,0,0,0},
        {12,0,0,0,0},
        {13,0,0,0,0},
        {14,0,0,0,0},
        {15,0,0,0,0},
    }
};

static strAEPLineNumInfo g_strAEScene28PLineInfo =
{
    AETABLE_SCENE_INDEX28,
    90,
    0,
    {
        {1,5000,20,1136,1136},
        {2,20,20,1136,3072},
        {3,20,12,3072,3072},
        {4,12,12,3072,4096},
        {5,12,8,4096,4096},
        {6,8,8,4096,6144},
        {7,0,0,0,0},
        {8,0,0,0,0},
        {9,0,0,0,0},
        {10,0,0,0,0},
        {11,0,0,0,0},
        {12,0,0,0,0},
        {13,0,0,0,0},
        {14,0,0,0,0},
        {15,0,0,0,0},
    }
};

static strAEPLineNumInfo g_strAEScene29PLineInfo =
{
    AETABLE_SCENE_INDEX29,
    90,
    0,
    {
        {1,5000,20,1136,1136},
        {2,20,20,1136,3072},
        {3,20,12,3072,3072},
        {4,12,12,3072,4096},
        {5,12,8,4096,4096},
        {6,8,8,4096,6144},
        {7,0,0,0,0},
        {8,0,0,0,0},
        {9,0,0,0,0},
        {10,0,0,0,0},
        {11,0,0,0,0},
        {12,0,0,0,0},
        {13,0,0,0,0},
        {14,0,0,0,0},
        {15,0,0,0,0},
    }
};

static strAEPLineNumInfo g_strAEScene30PLineInfo =
{
    AETABLE_SCENE_INDEX30,
    90,
    0,
    {
        {1,5000,20,1136,1136},
        {2,20,20,1136,3072},
        {3,20,12,3072,3072},
        {4,12,12,3072,4096},
        {5,12,8,4096,4096},
        {6,8,8,4096,6144},
        {7,0,0,0,0},
        {8,0,0,0,0},
        {9,0,0,0,0},
        {10,0,0,0,0},
        {11,0,0,0,0},
        {12,0,0,0,0},
        {13,0,0,0,0},
        {14,0,0,0,0},
        {15,0,0,0,0},
    }
};

static strAEPLineNumInfo g_strAEScene31PLineInfo =
{
    AETABLE_SCENE_INDEX31,
    90,
    0,
    {
        {1,5000,20,1136,1136},
        {2,20,20,1136,3072},
        {3,20,12,3072,3072},
        {4,12,12,3072,4096},
        {5,12,8,4096,4096},
        {6,8,8,4096,6144},
        {7,0,0,0,0},
        {8,0,0,0,0},
        {9,0,0,0,0},
        {10,0,0,0,0},
        {11,0,0,0,0},
        {12,0,0,0,0},
        {13,0,0,0,0},
        {14,0,0,0,0},
        {15,0,0,0,0},
    }
};



static strAEPLineNumInfo g_strAENoScenePLineInfo =
{
    AETABLE_SCENE_MAX,
    90,
    0,
    {
        {1,0,0,0,0},
        {2,0,0,0,0},
        {3,0,0,0,0},
        {4,0,0,0,0},
        {5,0,0,0,0},
        {6,0,0,0,0},
        {7,0,0,0,0},
        {8,0,0,0,0},
        {9,0,0,0,0},
        {10,0,0,0,0},
        {11,0,0,0,0},
        {12,0,0,0,0},
        {13,0,0,0,0},
        {14,0,0,0,0},
        {15,0,0,0,0},
    }
};
static strAEPLineInfomation g_strAEPlineInfo =
{
MTRUE,   // FALSE mean the sampling
{
// PLINE Info
g_strAEPreviewAutoPLineInfo,
g_strAECaptureAutoPLineInfo,
g_strAEVideoAutoPLineInfo,
g_strAEVideo1AutoPLineInfo,
g_strAEVideo2AutoPLineInfo,
g_strAECustom1AutoPLineInfo,
g_strAECustom2AutoPLineInfo,
g_strAECustom3AutoPLineInfo,
g_strAECustom4AutoPLineInfo,
g_strAECustom5AutoPLineInfo,
g_strAEVideoNightPLineInfo,
g_strAECaptureISO100PLineInfo,
g_strAECaptureISO200PLineInfo,
g_strAECaptureISO400PLineInfo,
g_strAECaptureISO800PLineInfo,
g_strAECaptureISO1600PLineInfo,
g_strAEStrobePLineInfo,
g_strAEScene1PLineInfo,
g_strAEScene2PLineInfo,
g_strAEScene3PLineInfo,
g_strAEScene4PLineInfo,
g_strAEScene5PLineInfo,
g_strAEScene6PLineInfo,
g_strAEScene7PLineInfo,
g_strAEScene8PLineInfo,
g_strAEScene9PLineInfo,
g_strAEScene10PLineInfo,
g_strAEScene11PLineInfo,
g_strAEScene12PLineInfo,
g_strAEScene13PLineInfo,
g_strAEScene14PLineInfo,
g_strAEScene15PLineInfo,
g_strAEScene16PLineInfo,
g_strAEScene17PLineInfo,
g_strAEScene18PLineInfo,
g_strAEScene19PLineInfo,
g_strAEScene20PLineInfo,
g_strAEScene21PLineInfo,
g_strAEScene22PLineInfo,
g_strAEScene23PLineInfo,
g_strAEScene24PLineInfo,
g_strAEScene25PLineInfo,
g_strAEScene26PLineInfo,
g_strAEScene27PLineInfo,
g_strAEScene28PLineInfo,
g_strAEScene29PLineInfo,
g_strAEScene30PLineInfo,
g_strAEScene31PLineInfo,
g_strAENoScenePLineInfo,
g_strAENoScenePLineInfo,
},
};

static strAEPLineGainList g_strAEGainList =
{
71,
{
{1140,100},
{1216,100},
{1328,100},
{1424,100},
{1536,100},
{1632,100},
{1728,100},
{1840,100},
{1936,100},
{2048,100},
{2144,100},
{2240,100},
{2352,100},
{2448,100},
{2560,100},
{2656,100},
{2752,100},
{2864,100},
{2960,100},
{3072,100},
{3200,100},
{3328,100},
{3456,100},
{3584,100},
{3712,100},
{3840,100},
{3968,100},
{4096,100},
{4224,100},
{4352,100},
{4480,100},
{4608,100},
{4736,100},
{4864,100},
{4992,100},
{5120,100},
{5248,100},
{5376,100},
{5504,100},
{5632,100},
{5760,100},
{5888,100},
{6016,100},
{6144,100},
{6240,100},
{6384,100},
{6544,100},
{6704,100},
{6896,100},
{7072,100},
{7280,100},
{7472,100},
{7696,100},
{7936,100},
{8192,100},
{8448,100},
{8720,100},
{9040,100},
{9344,100},
{9696,100},
{10080,100},
{10480,100},
{10912,100},
{11392,100},
{11904,100},
{12480,100},
{13104,100},
{13792,100},
{14560,100},
{15408,100},
{16384,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
{0,100},
}
};

static AE_PLINETABLE_T g_PlineTableMapping =
{
g_AEScenePLineMapping,
g_strAEPlineTable,
g_strAEPlineInfo,
g_strAEGainList
};
#endif


#if 0 //Save the P-line info to file for debug
MinGain,1136 
MaxGain,6144 
MiniISOGain,61 
GainStepUnitInTotalRange,128 
PreviewExposureLineUnit,39458 
PreviewMaxFrameRate,20 
VideoExposureLineUnit,39458 
VideoMaxFrameRate,20 
VideoToPreviewSensitivityRatio,1024 
CaptureExposureLineUnit,39458 
CaptureMaxFrameRate,20 
CaptureToPreviewSensitivityRatio,1024 
Video1ExposureLineUnit,39458 
Video1MaxFrameRate,20 
Video1ToPreviewSensitivityRatio,1024 
Video2ExposureLineUnit,39458 
Video2MaxFrameRate,20 
Video2ToPreviewSensitivityRatio,1024 
Custom1ExposureLineUnit,39458 
Custom1MaxFrameRate,20 
Custom1ToPreviewSensitivityRatio,1024 
Custom2ExposureLineUnit,39458 
Custom2MaxFrameRate,20 
Custom2ToPreviewSensitivityRatio,1024 
Custom3ExposureLineUnit,39458 
Custom3MaxFrameRate,20 
Custom3ToPreviewSensitivityRatio,1024 
Custom4ExposureLineUnit,39458 
Custom4MaxFrameRate,20 
Custom4ToPreviewSensitivityRatio,1024 
Custom5ExposureLineUnit,39458 
Custom5MaxFrameRate,20 
Custom5ToPreviewSensitivityRatio,1024 
FocusLength,350 
Fno,28 

// Preview table -- Use preview sensor setting
//Index MiniExp1/T MaxExp1/T MinGain MaxGain
AETABLE_RPEVIEW_AUTO
90,-10
1,5000,20,1136,1136
2,20,20,1136,3072
3,20,12,3072,3072
4,12,12,3072,4096
5,12,8,4096,4096
6,8,8,4096,6144
AETABLE_END

// Capture table -- Use capture sensor setting
//Index MiniExp1/T MaxExp1/T MinGain MaxGain
AETABLE_CAPTURE_AUTO
90,0
1,5000,20,1136,1136
2,20,20,1136,3072
3,20,12,3072,3072
4,12,12,3072,4096
5,12,8,4096,4096
6,8,8,4096,6144
AETABLE_END

// Video table -- Use video sensor setting
//Index MiniExp1/T MaxExp1/T MinGain MaxGain
AETABLE_VIDEO_AUTO
90,0
1,5000,20,1136,1136
2,20,20,1136,3072
3,20,12,3072,3072
4,12,12,3072,4096
5,12,8,4096,4096
6,8,8,4096,6144
AETABLE_END

// Video1 table -- Use Video1 sensor setting
//Index MiniExp1/T MaxExp1/T MinGain MaxGain
AETABLE_VIDEO1_AUTO
90,0
1,5000,20,1136,1136
2,20,20,1136,3072
3,20,12,3072,3072
4,12,12,3072,4096
5,12,8,4096,4096
6,8,8,4096,6144
AETABLE_END

// Video2 table -- Use Video2 sensor setting
//Index MiniExp1/T MaxExp1/T MinGain MaxGain
AETABLE_VIDEO2_AUTO
90,0
1,5000,20,1136,1136
2,20,20,1136,3072
3,20,12,3072,3072
4,12,12,3072,4096
5,12,8,4096,4096
6,8,8,4096,6144
AETABLE_END

// Custom1 table -- Use Custom1 sensor setting
//Index MiniExp1/T MaxExp1/T MinGain MaxGain
AETABLE_CUSTOM1_AUTO
90,0
1,5000,20,1136,1136
2,20,20,1136,3072
3,20,12,3072,3072
4,12,12,3072,4096
5,12,8,4096,4096
6,8,8,4096,6144
AETABLE_END

// Custom2 table -- Use Custom2 sensor setting
//Index MiniExp1/T MaxExp1/T MinGain MaxGain
AETABLE_CUSTOM2_AUTO
90,0
1,5000,20,1136,1136
2,20,20,1136,3072
3,20,12,3072,3072
4,12,12,3072,4096
5,12,8,4096,4096
6,8,8,4096,6144
AETABLE_END

// Custom3 table -- Use Custom3 sensor setting
//Index MiniExp1/T MaxExp1/T MinGain MaxGain
AETABLE_CUSTOM3_AUTO
90,0
1,5000,20,1136,1136
2,20,20,1136,3072
3,20,12,3072,3072
4,12,12,3072,4096
5,12,8,4096,4096
6,8,8,4096,6144
AETABLE_END

// Custom4 table -- Use Custom4 sensor setting
//Index MiniExp1/T MaxExp1/T MinGain MaxGain
AETABLE_CUSTOM4_AUTO
90,0
1,5000,20,1136,1136
2,20,20,1136,3072
3,20,12,3072,3072
4,12,12,3072,4096
5,12,8,4096,4096
6,8,8,4096,6144
AETABLE_END

// Custom5 table -- Use Custom5 sensor setting
//Index MiniExp1/T MaxExp1/T MinGain MaxGain
AETABLE_CUSTOM5_AUTO
90,0
1,5000,20,1136,1136
2,20,20,1136,3072
3,20,12,3072,3072
4,12,12,3072,4096
5,12,8,4096,4096
6,8,8,4096,6144
AETABLE_END

// Video Night table -- Use video sensor setting
//Index MiniExp1/T MaxExp1/T MinGain MaxGain
AETABLE_VIDEO_NIGHT
90,-10
1,5000,20,1136,1136
2,20,20,1136,3072
3,20,12,3072,3072
4,12,12,3072,4096
5,12,6,4096,4096
6,6,6,4096,6144
AETABLE_END

// Capture ISO100 -- Use capture sensor setting
//Index MiniExp1/T MaxExp1/T MinGain MaxGain
AETABLE_CAPTURE_ISO100
90,0
1,5000,20,1136,1136
2,20,20,1136,3072
3,20,12,3072,3072
4,12,12,3072,4096
5,12,8,4096,4096
6,8,8,4096,6144
AETABLE_END

// Capture ISO200 -- Use capture sensor setting
//Index MiniExp1/T MaxExp1/T MinGain MaxGain
AETABLE_CAPTURE_ISO200
90,0
1,5000,20,1136,1136
2,20,20,1136,3072
3,20,12,3072,3072
4,12,12,3072,4096
5,12,8,4096,4096
6,8,8,4096,6144
AETABLE_END

// Capture ISO400 -- Use capture sensor setting
//Index MiniExp1/T MaxExp1/T MinGain MaxGain
AETABLE_CAPTURE_ISO400
90,0
1,5000,20,1136,1136
2,20,20,1136,3072
3,20,12,3072,3072
4,12,12,3072,4096
5,12,8,4096,4096
6,8,8,4096,6144
AETABLE_END

// Capture ISO800 -- Use capture sensor setting
//Index MiniExp1/T MaxExp1/T MinGain MaxGain
AETABLE_CAPTURE_ISO800
90,0
1,5000,20,1136,1136
2,20,20,1136,3072
3,20,12,3072,3072
4,12,12,3072,4096
5,12,8,4096,4096
6,8,8,4096,6144
AETABLE_END

// Capture ISO1600 -- Use capture sensor setting
//Index MiniExp1/T MaxExp1/T MinGain MaxGain
AETABLE_CAPTURE_ISO1600
90,0
1,5000,20,1136,1136
2,20,20,1136,3072
3,20,12,3072,3072
4,12,12,3072,4096
5,12,8,4096,4096
6,8,8,4096,6144
AETABLE_END

// Strobe table -- Use capture sensor setting
//Index MiniExp1/T MaxExp1/T MinGain MaxGain
AETABLE_STROBE
90,0
1,5000,20,1136,1136
2,20,20,1136,3072
3,20,12,3072,3072
4,12,12,3072,4096
5,12,8,4096,4096
6,8,8,4096,6144
AETABLE_END


//Index MiniExp1/T MaxExp1/T MinGain MaxGain
AETABLE_SCENE_INDEX1
90,-10
1,5000,20,1136,1136
2,20,20,1136,3072
3,20,12,3072,3072
4,12,12,3072,4096
5,12,8,4096,4096
6,8,8,4096,6144
AETABLE_END


//Index MiniExp1/T MaxExp1/T MinGain MaxGain
AETABLE_SCENE_INDEX2
90,0
1,5000,20,1136,1136
2,20,20,1136,3072
3,20,12,3072,3072
4,12,12,3072,4096
5,12,8,4096,4096
6,8,8,4096,6144
AETABLE_END

// Table1 PLine -- Use capture sensor setting
//Index MiniExp1/T MaxExp1/T MinGain MaxGain
AETABLE_SCENE_INDEX3
90,0
1,5000,20,1136,1136
2,20,20,1136,3072
3,20,12,3072,3072
4,12,12,3072,4096
5,12,8,4096,4096
6,8,8,4096,6144
AETABLE_END

// Table2 PLine -- Use capture sensor setting
//Index MiniExp1/T MaxExp1/T MinGain MaxGain
AETABLE_SCENE_INDEX4
90,0
1,5000,20,1136,1136
2,20,20,1136,3072
3,20,12,3072,3072
4,12,12,3072,4096
5,12,8,4096,4096
6,8,8,4096,6144
AETABLE_END

// Table3 PLine -- Use capture sensor setting
//Index MiniExp1/T MaxExp1/T MinGain MaxGain
AETABLE_SCENE_INDEX5
90,0
1,5000,20,1136,1136
2,20,20,1136,3072
3,20,12,3072,3072
4,12,12,3072,4096
5,12,8,4096,4096
6,8,8,4096,6144
AETABLE_END

// Table4 PLine -- Use capture sensor setting
//Index MiniExp1/T MaxExp1/T MinGain MaxGain
AETABLE_SCENE_INDEX6
90,0
1,5000,20,1136,1136
2,20,20,1136,3072
3,20,12,3072,3072
4,12,12,3072,4096
5,12,6,4096,4096
6,6,6,4096,6144
AETABLE_END

// Table5 PLine -- Use capture sensor setting
//Index MiniExp1/T MaxExp1/T MinGain MaxGain
AETABLE_SCENE_INDEX7
90,0
1,5000,20,1136,1136
2,20,20,1136,3072
3,20,12,3072,3072
4,12,12,3072,4096
5,12,8,4096,4096
6,8,8,4096,6144
AETABLE_END

// Table6 PLine -- Use capture sensor setting
//Index MiniExp1/T MaxExp1/T MinGain MaxGain
AETABLE_SCENE_INDEX8
90,0
1,5000,20,1136,1136
2,20,20,1136,3072
3,20,12,3072,3072
4,12,12,3072,4096
5,12,8,4096,4096
6,8,8,4096,6144
AETABLE_END

// Table7 PLine -- Use capture sensor setting
//Index MiniExp1/T MaxExp1/T MinGain MaxGain
AETABLE_SCENE_INDEX9
90,0
1,5000,20,1136,1136
2,20,20,1136,3072
3,20,12,3072,3072
4,12,12,3072,4096
5,12,8,4096,4096
6,8,8,4096,6144
AETABLE_END

// Table8 PLine -- Use capture sensor setting
//Index MiniExp1/T MaxExp1/T MinGain MaxGain
AETABLE_SCENE_INDEX10
90,0
1,5000,20,1136,1136
2,20,20,1136,3072
3,20,12,3072,3072
4,12,12,3072,4096
5,12,8,4096,4096
6,8,8,4096,6144
AETABLE_END

// Table9 PLine -- Use capture sensor setting
//Index MiniExp1/T MaxExp1/T MinGain MaxGain
AETABLE_SCENE_INDEX11
90,0
1,5000,20,1136,1136
2,20,20,1136,3072
3,20,12,3072,3072
4,12,12,3072,4096
5,12,8,4096,4096
6,8,8,4096,6144
AETABLE_END

// Table10 PLine -- Use capture sensor setting
//Index MiniExp1/T MaxExp1/T MinGain MaxGain
AETABLE_SCENE_INDEX12
90,0
1,5000,20,1136,1136
2,20,20,1136,3072
3,20,12,3072,3072
4,12,12,3072,4096
5,12,8,4096,4096
6,8,8,4096,6144
AETABLE_END

// Table11 PLine -- Use capture sensor setting
//Index MiniExp1/T MaxExp1/T MinGain MaxGain
AETABLE_SCENE_INDEX13
90,0
1,5000,20,1136,1136
2,20,20,1136,3072
3,20,12,3072,3072
4,12,12,3072,4096
5,12,8,4096,4096
6,8,8,4096,6144
AETABLE_END

// Table12 PLine -- Use capture sensor setting
//Index MiniExp1/T MaxExp1/T MinGain MaxGain
AETABLE_SCENE_INDEX14
90,0
1,5000,20,1136,1136
2,20,20,1136,3072
3,20,12,3072,3072
4,12,12,3072,4096
5,12,8,4096,4096
6,8,8,4096,6144
AETABLE_END

// Table13 PLine -- Use capture sensor setting
//Index MiniExp1/T MaxExp1/T MinGain MaxGain
AETABLE_SCENE_INDEX15
90,0
1,5000,20,1136,1136
2,20,20,1136,3072
3,20,12,3072,3072
4,12,12,3072,4096
5,12,8,4096,4096
6,8,8,4096,6144
AETABLE_END

// Table14 PLine -- Use capture sensor setting
//Index MiniExp1/T MaxExp1/T MinGain MaxGain
AETABLE_SCENE_INDEX16
90,0
1,5000,20,1136,1136
2,20,20,1136,3072
3,20,12,3072,3072
4,12,12,3072,4096
5,12,8,4096,4096
6,8,8,4096,6144
AETABLE_END

// Table15 PLine -- Use capture sensor setting
//Index MiniExp1/T MaxExp1/T MinGain MaxGain
AETABLE_SCENE_INDEX17
90,0
1,5000,20,1136,1136
2,20,20,1136,3072
3,20,12,3072,3072
4,12,12,3072,4096
5,12,8,4096,4096
6,8,8,4096,6144
AETABLE_END

// Table16 PLine -- Use capture sensor setting
//Index MiniExp1/T MaxExp1/T MinGain MaxGain
AETABLE_SCENE_INDEX18
90,0
1,5000,20,1136,1136
2,20,20,1136,3072
3,20,12,3072,3072
4,12,12,3072,4096
5,12,8,4096,4096
6,8,8,4096,6144
AETABLE_END

// Table17 PLine -- Use capture sensor setting
//Index MiniExp1/T MaxExp1/T MinGain MaxGain
AETABLE_SCENE_INDEX19
90,0
1,5000,20,1136,1136
2,20,20,1136,3072
3,20,12,3072,3072
4,12,12,3072,4096
5,12,8,4096,4096
6,8,8,4096,6144
AETABLE_END

// Table18 PLine -- Use capture sensor setting
//Index MiniExp1/T MaxExp1/T MinGain MaxGain
AETABLE_SCENE_INDEX20
90,0
1,5000,20,1136,1136
2,20,20,1136,3072
3,20,12,3072,3072
4,12,12,3072,4096
5,12,8,4096,4096
6,8,8,4096,6144
AETABLE_END

// Table19 PLine -- Use capture sensor setting
//Index MiniExp1/T MaxExp1/T MinGain MaxGain
AETABLE_SCENE_INDEX21
90,0
1,5000,20,1136,1136
2,20,20,1136,3072
3,20,12,3072,3072
4,12,12,3072,4096
5,12,8,4096,4096
6,8,8,4096,6144
AETABLE_END

// Table20 PLine -- Use capture sensor setting
//Index MiniExp1/T MaxExp1/T MinGain MaxGain
AETABLE_SCENE_INDEX22
90,0
1,5000,20,1136,1136
2,20,20,1136,3072
3,20,12,3072,3072
4,12,12,3072,4096
5,12,8,4096,4096
6,8,8,4096,6144
AETABLE_END

// Table21 PLine -- Use capture sensor setting
//Index MiniExp1/T MaxExp1/T MinGain MaxGain
AETABLE_SCENE_INDEX23
90,0
1,5000,20,1136,1136
2,20,20,1136,3072
3,20,12,3072,3072
4,12,12,3072,4096
5,12,8,4096,4096
6,8,8,4096,6144
AETABLE_END

// Table22 PLine -- Use capture sensor setting
//Index MiniExp1/T MaxExp1/T MinGain MaxGain
AETABLE_SCENE_INDEX24
90,0
1,5000,20,1136,1136
2,20,20,1136,3072
3,20,12,3072,3072
4,12,12,3072,4096
5,12,8,4096,4096
6,8,8,4096,6144
AETABLE_END

// Table23 PLine -- Use capture sensor setting
//Index MiniExp1/T MaxExp1/T MinGain MaxGain
AETABLE_SCENE_INDEX25
90,0
1,5000,20,1136,1136
2,20,20,1136,3072
3,20,12,3072,3072
4,12,12,3072,4096
5,12,8,4096,4096
6,8,8,4096,6144
AETABLE_END

// Table24 PLine -- Use capture sensor setting
//Index MiniExp1/T MaxExp1/T MinGain MaxGain
AETABLE_SCENE_INDEX26
90,0
1,5000,20,1136,1136
2,20,20,1136,3072
3,20,12,3072,3072
4,12,12,3072,4096
5,12,8,4096,4096
6,8,8,4096,6144
AETABLE_END

// Table25 PLine -- Use capture sensor setting
//Index MiniExp1/T MaxExp1/T MinGain MaxGain
AETABLE_SCENE_INDEX27
90,0
1,5000,20,1136,1136
2,20,20,1136,3072
3,20,12,3072,3072
4,12,12,3072,4096
5,12,8,4096,4096
6,8,8,4096,6144
AETABLE_END

// Table26 PLine -- Use capture sensor setting
//Index MiniExp1/T MaxExp1/T MinGain MaxGain
AETABLE_SCENE_INDEX28
90,0
1,5000,20,1136,1136
2,20,20,1136,3072
3,20,12,3072,3072
4,12,12,3072,4096
5,12,8,4096,4096
6,8,8,4096,6144
AETABLE_END

// Table27 PLine -- Use capture sensor setting
//Index MiniExp1/T MaxExp1/T MinGain MaxGain
AETABLE_SCENE_INDEX29
90,0
1,5000,20,1136,1136
2,20,20,1136,3072
3,20,12,3072,3072
4,12,12,3072,4096
5,12,8,4096,4096
6,8,8,4096,6144
AETABLE_END

// Table28 PLine -- Use capture sensor setting
//Index MiniExp1/T MaxExp1/T MinGain MaxGain
AETABLE_SCENE_INDEX30
90,0
1,5000,20,1136,1136
2,20,20,1136,3072
3,20,12,3072,3072
4,12,12,3072,4096
5,12,8,4096,4096
6,8,8,4096,6144
AETABLE_END

// Table29 PLine -- Use capture sensor setting
//Index MiniExp1/T MaxExp1/T MinGain MaxGain
AETABLE_SCENE_INDEX31
90,0
1,5000,20,1136,1136
2,20,20,1136,3072
3,20,12,3072,3072
4,12,12,3072,4096
5,12,8,4096,4096
6,8,8,4096,6144
AETABLE_END

// Table30 PLine -- Use capture sensor setting

// Table31 PLine -- Use capture sensor setting

AE_SCENE_AUTO,AETABLE_RPEVIEW_AUTO,AETABLE_CAPTURE_AUTO,AETABLE_VIDEO_AUTO,AETABLE_VIDEO1_AUTO,AETABLE_VIDEO2_AUTO,AETABLE_CUSTOM1_AUTO,AETABLE_CUSTOM2_AUTO,AETABLE_CUSTOM3_AUTO,AETABLE_CUSTOM4_AUTO,AETABLE_CUSTOM5_AUTO,AETABLE_SCENE_INDEX5
AE_SCENE_NIGHT,AETABLE_SCENE_INDEX6,AETABLE_SCENE_INDEX6,AETABLE_VIDEO_NIGHT,AETABLE_VIDEO_NIGHT,AETABLE_VIDEO_NIGHT,AETABLE_CUSTOM1_AUTO,AETABLE_CUSTOM2_AUTO,AETABLE_CUSTOM3_AUTO,AETABLE_CUSTOM4_AUTO,AETABLE_CUSTOM5_AUTO,AETABLE_SCENE_INDEX1
AE_SCENE_ACTION,AETABLE_RPEVIEW_AUTO,AETABLE_SCENE_INDEX2,AETABLE_VIDEO_AUTO,AETABLE_VIDEO1_AUTO,AETABLE_VIDEO2_AUTO,AETABLE_CUSTOM1_AUTO,AETABLE_CUSTOM2_AUTO,AETABLE_CUSTOM3_AUTO,AETABLE_CUSTOM4_AUTO,AETABLE_CUSTOM5_AUTO,AETABLE_SCENE_INDEX2
AE_SCENE_BEACH,AETABLE_RPEVIEW_AUTO,AETABLE_SCENE_INDEX3,AETABLE_VIDEO_AUTO,AETABLE_VIDEO1_AUTO,AETABLE_VIDEO2_AUTO,AETABLE_CUSTOM1_AUTO,AETABLE_CUSTOM2_AUTO,AETABLE_CUSTOM3_AUTO,AETABLE_CUSTOM4_AUTO,AETABLE_CUSTOM5_AUTO,AETABLE_SCENE_INDEX3
AE_SCENE_CANDLELIGHT,AETABLE_RPEVIEW_AUTO,AETABLE_SCENE_INDEX1,AETABLE_VIDEO_AUTO,AETABLE_VIDEO1_AUTO,AETABLE_VIDEO2_AUTO,AETABLE_CUSTOM1_AUTO,AETABLE_CUSTOM2_AUTO,AETABLE_CUSTOM3_AUTO,AETABLE_CUSTOM4_AUTO,AETABLE_CUSTOM5_AUTO,AETABLE_SCENE_INDEX1
AE_SCENE_FIREWORKS,AETABLE_RPEVIEW_AUTO,AETABLE_SCENE_INDEX4,AETABLE_VIDEO_AUTO,AETABLE_VIDEO1_AUTO,AETABLE_VIDEO2_AUTO,AETABLE_CUSTOM1_AUTO,AETABLE_CUSTOM2_AUTO,AETABLE_CUSTOM3_AUTO,AETABLE_CUSTOM4_AUTO,AETABLE_CUSTOM5_AUTO,AETABLE_SCENE_INDEX4
AE_SCENE_LANDSCAPE,AETABLE_RPEVIEW_AUTO,AETABLE_SCENE_INDEX3,AETABLE_VIDEO_AUTO,AETABLE_VIDEO1_AUTO,AETABLE_VIDEO2_AUTO,AETABLE_CUSTOM1_AUTO,AETABLE_CUSTOM2_AUTO,AETABLE_CUSTOM3_AUTO,AETABLE_CUSTOM4_AUTO,AETABLE_CUSTOM5_AUTO,AETABLE_SCENE_INDEX3
AE_SCENE_PORTRAIT,AETABLE_RPEVIEW_AUTO,AETABLE_SCENE_INDEX2,AETABLE_VIDEO_AUTO,AETABLE_VIDEO1_AUTO,AETABLE_VIDEO2_AUTO,AETABLE_CUSTOM1_AUTO,AETABLE_CUSTOM2_AUTO,AETABLE_CUSTOM3_AUTO,AETABLE_CUSTOM4_AUTO,AETABLE_CUSTOM5_AUTO,AETABLE_SCENE_INDEX2
AE_SCENE_NIGHT_PORTRAIT,AETABLE_SCENE_INDEX6,AETABLE_SCENE_INDEX6,AETABLE_VIDEO_NIGHT,AETABLE_VIDEO_NIGHT,AETABLE_VIDEO_NIGHT,AETABLE_CUSTOM1_AUTO,AETABLE_CUSTOM2_AUTO,AETABLE_CUSTOM3_AUTO,AETABLE_CUSTOM4_AUTO,AETABLE_CUSTOM5_AUTO,AETABLE_SCENE_INDEX1
AE_SCENE_PARTY,AETABLE_RPEVIEW_AUTO,AETABLE_SCENE_INDEX1,AETABLE_VIDEO_AUTO,AETABLE_VIDEO1_AUTO,AETABLE_VIDEO2_AUTO,AETABLE_CUSTOM1_AUTO,AETABLE_CUSTOM2_AUTO,AETABLE_CUSTOM3_AUTO,AETABLE_CUSTOM4_AUTO,AETABLE_CUSTOM5_AUTO,AETABLE_SCENE_INDEX1
AE_SCENE_SNOW,AETABLE_RPEVIEW_AUTO,AETABLE_SCENE_INDEX3,AETABLE_VIDEO_AUTO,AETABLE_VIDEO1_AUTO,AETABLE_VIDEO2_AUTO,AETABLE_CUSTOM1_AUTO,AETABLE_CUSTOM2_AUTO,AETABLE_CUSTOM3_AUTO,AETABLE_CUSTOM4_AUTO,AETABLE_CUSTOM5_AUTO,AETABLE_SCENE_INDEX3
AE_SCENE_SPORTS,AETABLE_RPEVIEW_AUTO,AETABLE_SCENE_INDEX2,AETABLE_VIDEO_AUTO,AETABLE_VIDEO1_AUTO,AETABLE_VIDEO2_AUTO,AETABLE_CUSTOM1_AUTO,AETABLE_CUSTOM2_AUTO,AETABLE_CUSTOM3_AUTO,AETABLE_CUSTOM4_AUTO,AETABLE_CUSTOM5_AUTO,AETABLE_SCENE_INDEX2
AE_SCENE_STEADYPHOTO,AETABLE_RPEVIEW_AUTO,AETABLE_SCENE_INDEX2,AETABLE_VIDEO_AUTO,AETABLE_VIDEO1_AUTO,AETABLE_VIDEO2_AUTO,AETABLE_CUSTOM1_AUTO,AETABLE_CUSTOM2_AUTO,AETABLE_CUSTOM3_AUTO,AETABLE_CUSTOM4_AUTO,AETABLE_CUSTOM5_AUTO,AETABLE_SCENE_INDEX2
AE_SCENE_SUNSET,AETABLE_RPEVIEW_AUTO,AETABLE_SCENE_INDEX3,AETABLE_VIDEO_AUTO,AETABLE_VIDEO1_AUTO,AETABLE_VIDEO2_AUTO,AETABLE_CUSTOM1_AUTO,AETABLE_CUSTOM2_AUTO,AETABLE_CUSTOM3_AUTO,AETABLE_CUSTOM4_AUTO,AETABLE_CUSTOM5_AUTO,AETABLE_SCENE_INDEX3
AE_SCENE_THEATRE,AETABLE_RPEVIEW_AUTO,AETABLE_SCENE_INDEX1,AETABLE_VIDEO_AUTO,AETABLE_VIDEO1_AUTO,AETABLE_VIDEO2_AUTO,AETABLE_CUSTOM1_AUTO,AETABLE_CUSTOM2_AUTO,AETABLE_CUSTOM3_AUTO,AETABLE_CUSTOM4_AUTO,AETABLE_CUSTOM5_AUTO,AETABLE_SCENE_INDEX1
AE_SCENE_ISO_ANTI_SHAKE,AETABLE_RPEVIEW_AUTO,AETABLE_SCENE_INDEX2,AETABLE_VIDEO_AUTO,AETABLE_VIDEO1_AUTO,AETABLE_VIDEO2_AUTO,AETABLE_CUSTOM1_AUTO,AETABLE_CUSTOM2_AUTO,AETABLE_CUSTOM3_AUTO,AETABLE_CUSTOM4_AUTO,AETABLE_CUSTOM5_AUTO,AETABLE_SCENE_INDEX2
AE_SCENE_ISO100,AETABLE_RPEVIEW_AUTO,AETABLE_CAPTURE_ISO100,AETABLE_VIDEO_AUTO,AETABLE_VIDEO1_AUTO,AETABLE_VIDEO2_AUTO,AETABLE_CUSTOM1_AUTO,AETABLE_CUSTOM2_AUTO,AETABLE_CUSTOM3_AUTO,AETABLE_CUSTOM4_AUTO,AETABLE_CUSTOM5_AUTO,AETABLE_CAPTURE_ISO100
AE_SCENE_ISO200,AETABLE_RPEVIEW_AUTO,AETABLE_CAPTURE_ISO200,AETABLE_VIDEO_AUTO,AETABLE_VIDEO1_AUTO,AETABLE_VIDEO2_AUTO,AETABLE_CUSTOM1_AUTO,AETABLE_CUSTOM2_AUTO,AETABLE_CUSTOM3_AUTO,AETABLE_CUSTOM4_AUTO,AETABLE_CUSTOM5_AUTO,AETABLE_CAPTURE_ISO200
AE_SCENE_ISO400,AETABLE_RPEVIEW_AUTO,AETABLE_CAPTURE_ISO400,AETABLE_VIDEO_AUTO,AETABLE_VIDEO1_AUTO,AETABLE_VIDEO2_AUTO,AETABLE_CUSTOM1_AUTO,AETABLE_CUSTOM2_AUTO,AETABLE_CUSTOM3_AUTO,AETABLE_CUSTOM4_AUTO,AETABLE_CUSTOM5_AUTO,AETABLE_CAPTURE_ISO400
AE_SCENE_ISO800,AETABLE_RPEVIEW_AUTO,AETABLE_CAPTURE_ISO800,AETABLE_VIDEO_AUTO,AETABLE_VIDEO1_AUTO,AETABLE_VIDEO2_AUTO,AETABLE_CUSTOM1_AUTO,AETABLE_CUSTOM2_AUTO,AETABLE_CUSTOM3_AUTO,AETABLE_CUSTOM4_AUTO,AETABLE_CUSTOM5_AUTO,AETABLE_CAPTURE_ISO800
AE_SCENE_ISO1600 ,AETABLE_RPEVIEW_AUTO,AETABLE_CAPTURE_ISO1600,AETABLE_VIDEO_AUTO,AETABLE_VIDEO1_AUTO,AETABLE_VIDEO2_AUTO,AETABLE_CUSTOM1_AUTO,AETABLE_CUSTOM2_AUTO,AETABLE_CUSTOM3_AUTO,AETABLE_CUSTOM4_AUTO,AETABLE_CUSTOM5_AUTO,AETABLE_CAPTURE_ISO1600
,,,,,,,,,,,
,,,,,,,,,,,
,,,,,,,,,,,
,,,,,,,,,,,
,,,,,,,,,,,
,,,,,,,,,,,
,,,,,,,,,,,
,,,,,,,,,,,
,,,,,,,,,,,
#endif
