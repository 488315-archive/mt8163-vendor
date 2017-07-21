/* Copyright Statement:
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein is
 * confidential and proprietary to MediaTek Inc. and/or its licensors. Without
 * the prior written permission of MediaTek inc. and/or its licensors, any
 * reproduction, modification, use or disclosure of MediaTek Software, and
 * information contained herein, in whole or in part, shall be strictly
 * prohibited.
 *
 * MediaTek Inc. (C) 2010. All rights reserved.
 *
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE")
 * RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO RECEIVER
 * ON AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL
 * WARRANTIES, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR
 * NONINFRINGEMENT. NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH
 * RESPECT TO THE SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY,
 * INCORPORATED IN, OR SUPPLIED WITH THE MEDIATEK SOFTWARE, AND RECEIVER AGREES
 * TO LOOK ONLY TO SUCH THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO.
 * RECEIVER EXPRESSLY ACKNOWLEDGES THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO
 * OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES CONTAINED IN MEDIATEK
 * SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK SOFTWARE
 * RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S
 * ENTIRE AND CUMULATIVE LIABILITY WITH RESPECT TO THE MEDIATEK SOFTWARE
 * RELEASED HEREUNDER WILL BE, AT MEDIATEK'S OPTION, TO REVISE OR REPLACE THE
 * MEDIATEK SOFTWARE AT ISSUE, OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE
 * CHARGE PAID BY RECEIVER TO MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 *
 * The following software/firmware and/or related documentation ("MediaTek
 * Software") have been modified by MediaTek Inc. All revisions are subject to
 * any receiver's applicable license agreements with MediaTek Inc.
 */

#include "typedefs.h"
#include "platform.h"

#include "pll.h"
#include "timer.h"
#include "spm.h"
#include "spm_mtcmos.h"

//#include "dramc_pi_api.h"
#include "dramc_common.h"
#include "dramc_register.h"
#include "wdt.h"
#include "emi.h"

#include "../security/inc/sec_devinfo.h"


#ifdef MEMPLL_CLK_793
#define ACD_TEST_MODE_1600
#else
//#define ACD_TEST_MODE
#endif


#define DRAMC_WRITE_REG(val,offset) \
        do { \
            (*(volatile unsigned int *)(DRAMC0_BASE + (offset))) = (unsigned int)(val); \
            (*(volatile unsigned int *)(DDRPHY_BASE + (offset))) = (unsigned int)(val); \
        } while(0)

/***********************
 * MEMPLL Configuration
 ***********************/

#define r_bias_en_stb_time            (0x00000000 << 24)
#define r_bias_lpf_en_stb_time        (0x00000000 << 16)
#define r_mempll_en_stb_time          (0x00000000 << 8)
#define r_dmall_ck_en_stb_time        (0x00000000 << 0)

#define r_dds_en_stb_time             (0x00000000 << 24)
#define r_div_en_stb_time             (0x00000000 << 16)
#define r_dmpll2_ck_en_stb_time       (0x00000000 << 8)
#define r_iso_en_stb_time             (0x00000000 << 0)

#define r_bias_en_stb_dis             (0x00000001 << 28)
#define r_bias_en_src_sel             (0x00000001 << 24)
#define r_bias_lpf_en_stb_dis         (0x00000001 << 20)
#define r_bias_lpf_en_src_sel         (0x00000001 << 16)
#define r_mempll4_en_stb_dis          (0x00000001 << 15)
#define r_mempll3_en_stb_dis          (0x00000001 << 14)
#define r_mempll2_en_stb_dis          (0x00000001 << 13)
#define r_mempll_en_stb_dis           (0x00000001 << 12)
#define r_mempll4_en_src_sel          (0x00000001 << 11)
#define r_mempll3_en_src_sel          (0x00000001 << 10)
#define r_mempll2_en_src_sel          (0x00000001 << 9)
#define r_mempll_en_src_sel           (0x00000001 << 8)
#define r_dmall_ck_en_stb_dis         (0x00000001 << 4)
#define r_dmall_ck_en_src_sel         (0x00000001 << 0)

#define r_dds_en_stb_dis              (0x00000001 << 28)
#define r_dds_en_src_sel              (0x00000001 << 24)
#define r_div_en_stb_dis              (0x00000001 << 20)
#define r_div_en_src_sel              (0x00000001 << 16)
#define r_dmpll2_ck_en_stb_dis        (0x00000001 << 12)
#define r_dmpll2_ck_en_src_sel        (0x00000001 << 8)
#define r_iso_en_stb_dis              (0x00000001 << 4)
#define r_iso_en_src_sel              (0x00000001 << 0)

#define r_dmbyp_pll4                  (0x00000001 << 0)
#define r_dmbyp_pll3                  (0x00000001 << 1)
#define r_dm1pll_sync_mode            (0x00000001 << 2)
#define r_dmall_ck_en                 (0x00000001 << 4)
#define r_dmpll2_clk_en               (0x00000001 << 5)
// common setting for 0x180<<2
unsigned int pllc1_prediv_1_0_n1           = 0x00000000 << 16;//180[17:16]
unsigned int pllc1_postdiv_1_0_n5          = 0x00000000 << 14;//180[15:14]
unsigned int pllc1_blp                     = 0x00000001 << 12;//180[12]
// end of common setting

#if 0
#ifdef MEMPLL_CLK_500 //DDR1000
    unsigned int pllc1_fbksel_1_0_n3           = 0x00000000 << 26; //180[17:16]
    unsigned int pllc1_dmss_pcw_ncpo_30_24_n4  = 0x0000004e << 25; //180[12]
    unsigned int pllc1_dmss_pcw_ncpo_23_0_n4   = 0x00d89d89 <<  1;
#else
    #ifdef MEMPLL_CLK_400 //DDR800
        unsigned int pllc1_fbksel_1_0_n3           = 0x00000000 << 26;
        unsigned int pllc1_dmss_pcw_ncpo_30_24_n4  = 0x0000004e << 25;
        unsigned int pllc1_dmss_pcw_ncpo_23_0_n4   = 0x00d89d89 <<  1;
    #else
        #ifdef MEMPLL_CLK_250 //DDR500
            unsigned int pllc1_fbksel_1_0_n3           = 0x00000000 << 26;
            unsigned int pllc1_dmss_pcw_ncpo_30_24_n4  = 0x0000004e << 25;
            unsigned int pllc1_dmss_pcw_ncpo_23_0_n4   = 0x00d89d89 <<  1;
        #else
            #ifdef MEMPLL_CLK_266 //DDR533
               #ifdef DDRPHY_3PLL_MODE
                unsigned int pllc1_fbksel_1_0_n3           = 0x00000000 << 26;
                unsigned int pllc1_dmss_pcw_ncpo_30_24_n4  = 0x00000054 << 25; //0x00000046 << 25;
                unsigned int pllc1_dmss_pcw_ncpo_23_0_n4   = 0x000ccccc <<  1; //0x000aacd9 <<  1;
               #else
                unsigned int pllc1_fbksel_1_0_n3           = 0x00000000 << 26;
                unsigned int pllc1_dmss_pcw_ncpo_30_24_n4  = 0x00000052 << 25;
                unsigned int pllc1_dmss_pcw_ncpo_23_0_n4   = 0x00000000 <<  1;
               #endif
            #else
                #ifdef MEMPLL_CLK_150 //DDR300
                    unsigned int pllc1_fbksel_1_0_n3           = 0x00000000 << 26;
                    unsigned int pllc1_dmss_pcw_ncpo_30_24_n4  = 0x0000004e << 25;
                    unsigned int pllc1_dmss_pcw_ncpo_23_0_n4   = 0x00d89d89 <<  1;
                #else //DDR1172.6
                    #ifdef MEMPLL_CLK_586
                        unsigned int pllc1_fbksel_1_0_n3           = 0x00000000 << 26;
                        unsigned int pllc1_dmss_pcw_ncpo_30_24_n4  = 0x0000004d << 25;
                        unsigned int pllc1_dmss_pcw_ncpo_23_0_n4   = 0x000bbbbb <<  1;
                    #else //DDR1066
                     #ifdef MEMPLL_CLK_533
                       #ifdef DDRPHY_3PLL_MODE
                        unsigned int pllc1_fbksel_1_0_n3           = 0x00000000 << 26;
                        unsigned int pllc1_dmss_pcw_ncpo_30_24_n4  = 0x00000054 << 25; //0x0000004c << 25;
                        unsigned int pllc1_dmss_pcw_ncpo_23_0_n4   = 0x000ccccc <<  1; //0x0068bac7 <<  1;
                       #else //DDRPHY_3PLL_MODE
                        unsigned int pllc1_fbksel_1_0_n3           = 0x00000000 << 26;
                        unsigned int pllc1_dmss_pcw_ncpo_30_24_n4  = 0x00000052 << 25;
                        unsigned int pllc1_dmss_pcw_ncpo_23_0_n4   = 0x00000000 <<  1;
                       #endif //  DDRPHY_3PLL_MODE
                     #else
                     #ifdef MEMPLL_CLK_700
                       #ifdef DDRPHY_3PLL_MODE
                        unsigned int pllc1_fbksel_1_0_n3           = 0x00000000 << 26;
                        unsigned int pllc1_dmss_pcw_ncpo_30_24_n4  = 0x0000004e << 25;
                        unsigned int pllc1_dmss_pcw_ncpo_23_0_n4   = 0x00d8a090 <<  1;
                       #else //DDRPHY_3PLL_MODE
                        unsigned int pllc1_fbksel_1_0_n3           = 0x00000000 << 26;
                        unsigned int pllc1_dmss_pcw_ncpo_30_24_n4  = 0x00000053 << 25;
                        unsigned int pllc1_dmss_pcw_ncpo_23_0_n4   = 0x004f212d <<  1;
                        #endif //DDRPHY_3PLL_MODE
                     #else //1586
                     #ifdef MEMPLL_CLK_793
                       #ifdef DDRPHY_3PLL_MODE
                        unsigned int pllc1_fbksel_1_0_n3           = 0x00000000 << 26;
                        unsigned int pllc1_dmss_pcw_ncpo_30_24_n4  = 0x0000004e << 25;
                        unsigned int pllc1_dmss_pcw_ncpo_23_0_n4   = 0x00280346 <<  1;
                       #else //DDRPHY_3PLL_MODE
                        unsigned int pllc1_fbksel_1_0_n3           = 0x00000000 << 26;
                        unsigned int pllc1_dmss_pcw_ncpo_30_24_n4  = 0x00000052 << 25;
                        unsigned int pllc1_dmss_pcw_ncpo_23_0_n4   = 0x00000000 <<  1;
                        #endif //DDRPHY_3PLL_MODE
                     #else //1664
                     #ifdef MEMPLL_CLK_832
                        #ifdef DDRPHY_3PLL_MODE
                        unsigned int pllc1_fbksel_1_0_n3           = 0x00000000 << 26;
                        unsigned int pllc1_dmss_pcw_ncpo_30_24_n4  = 0x0000004d << 25;
                        unsigned int pllc1_dmss_pcw_ncpo_23_0_n4   = 0x002d2f1a <<  1;
                        #else //DDRPHY_3PLL_MODE
                        unsigned int pllc1_fbksel_1_0_n3           = 0x00000000 << 26;
                        unsigned int pllc1_dmss_pcw_ncpo_30_24_n4  = 0x00000052 << 25;
                        unsigned int pllc1_dmss_pcw_ncpo_23_0_n4   = 0x00000000 <<  1;
                        #endif //DDRPHY_3PLL_MODE
                     #else //1333
						#ifdef MEMPLL_CLK_666
							#ifdef DDRPHY_3PLL_MODE
	                        unsigned int pllc1_fbksel_1_0_n3           = 0x00000000 << 26;
	                        unsigned int pllc1_dmss_pcw_ncpo_30_24_n4  = 0x00000050 << 25;
	                        unsigned int pllc1_dmss_pcw_ncpo_23_0_n4   = 0x00d8fe7c <<  1; //0x00d8fc50 <<  1;
							#else //DDRPHY_3PLL_MODE
	                        unsigned int pllc1_fbksel_1_0_n3           = 0x00000000 << 26;
	                        unsigned int pllc1_dmss_pcw_ncpo_30_24_n4  = 0x00000052 << 25;
	                        unsigned int pllc1_dmss_pcw_ncpo_23_0_n4   = 0x006ed288 <<  1;
							#endif //DDRPHY_3PLL_MODE
						#else //1600, todo:
							#ifdef DDRPHY_3PLL_MODE
							unsigned int pllc1_fbksel_1_0_n3           = 0x00000000 << 26;
							unsigned int pllc1_dmss_pcw_ncpo_30_24_n4  = 0x00000054 << 25;
							unsigned int pllc1_dmss_pcw_ncpo_23_0_n4   = 0x001a41a4 <<  1;
							#else //DDRPHY_3PLL_MODE
							unsigned int pllc1_fbksel_1_0_n3           = 0x00000000 << 26;
							unsigned int pllc1_dmss_pcw_ncpo_30_24_n4  = 0x00000052 << 25;
							unsigned int pllc1_dmss_pcw_ncpo_23_0_n4   = 0x006ed288 <<  1;
							#endif //DDRPHY_3PLL_MODE
						#endif
                     #endif
                     #endif
                     #endif
                    #endif
                    #endif
                #endif
            #endif
        #endif
    #endif
#endif
#endif

#define pllc1_dmss_pcw_ncpo_chg       (0x00000001 << 0)
#define pllc1_mempll_div_en           (0x00000001 <<24)
#define pllc1_mempll_div_6_0          (0x00000052 <<25)
#ifdef BYPASS_MEMPLL1
    #define pllc1_mempll_refck_en     (0x00000000 <<9)
#else
    #define pllc1_mempll_refck_en     (0x00000001 <<9)
#endif
#define pllc1_mempll_top_reserve_8_0  (0x00000000 <<0) // 0x181<<4 if 8163, unused currently

#define pllc1_mempll_bias_en          (0x00000001 <<14)
#define pllc1_mempll_bias_lpf_en      (0x00000001 <<15)
#define pllc1_mempll_en               (0x00000001 << 2)
#define pllc1_dmss_ncpo_en            (0x00000001 << 4)
#define pllc1_dmss_fifo_start_man     (0x00000001 <<11) // 0x18a<<4 if 8163, unused currently
#define pllc1_mempll_dds_en           (0x00000001 <<25)

#define mempll2_prediv_1_0            (0x00000000 << 0)
/*#define mempll2_vco_div_sel           (0x00000001 <<29)*/
#if 0
#ifdef MEMPLL_CLK_500 //DDR1000
    unsigned int mempll2_m4pdiv_1_0    = (0x00000001 << 10);
    #ifdef DDRPHY_3PLL_MODE
        unsigned int mempll2_fbdiv_6_0 = (0x00000009 <<  2);
    #else  //DDRPHY_3PLL_MODE
        unsigned int mempll2_fbdiv_6_0 = (0x00000050 <<  2);
    #endif //DDRPHY_3PLL_MODE
#else
    #ifdef MEMPLL_CLK_400 //DDR800
        unsigned int mempll2_m4pdiv_1_0    = (0x00000001 << 10);
        #ifdef DDRPHY_3PLL_MODE
            unsigned int mempll2_fbdiv_6_0 = (0x00000007 <<  2);
        #else //DDRPHY_3PLL_MODE
            unsigned int mempll2_fbdiv_6_0 = (0x00000040 <<  2);
        #endif //DDRPHY_3PLL_MODE
    #else
        #ifdef MEMPLL_CLK_250 //DDR500
        unsigned int mempll2_m4pdiv_1_0        = (0x00000002 << 10);
            #ifdef DDRPHY_3PLL_MODE
                unsigned int mempll2_fbdiv_6_0 = (0x00000004 <<  2);
            #else //DDRPHY_3PLL_MODE
                unsigned int mempll2_fbdiv_6_0 = (0x00000050 <<  2);
            #endif //DDRPHY_3PLL_MODE
        #else
            #ifdef MEMPLL_CLK_266 //DDR533
                #define mempll2_vco_div_sel           (0x00000001 <<29)
                unsigned int mempll2_m4pdiv_1_0    = (0x00000001 << 10);
                #ifdef DDRPHY_3PLL_MODE
                    unsigned int mempll2_fbdiv_6_0 = (0x00000005 <<  2);
                #else //DDRPHY_3PLL_MODE
                    unsigned int mempll2_fbdiv_6_0 = (0x00000029 <<  2);//
                #endif //DDRPHY_3PLL_MODE
            #else //DDR1066
                #ifdef MEMPLL_CLK_150 //DDR300
                    unsigned int mempll2_m4pdiv_1_0    = (0x00000002 << 10);
                    #ifdef DDRPHY_3PLL_MODE
                        unsigned int mempll2_fbdiv_6_0 = (0x00000002 <<  2);
                    #else //DDRPHY_3PLL_MODE
                        unsigned int mempll2_fbdiv_6_0 = (0x00000060 <<  2);
                    #endif //DDRPHY_3PLL_MODE
                #else
                    #ifdef MEMPLL_CLK_586 //DDD1172.6
                        unsigned int mempll2_m4pdiv_1_0    = (0x00000001 << 10);
                        #ifdef DDRPHY_3PLL_MODE
                            unsigned int mempll2_fbdiv_6_0 = (0x0000000b <<  2);
                        #else //DDRPHY_3PLL_MODE
                            unsigned int mempll2_fbdiv_6_0 = (0x00000058 <<  2);
                        #endif //DDRPHY_3PLL_MODE
                    #else //DDR1066
                       #ifdef MEMPLL_CLK_533
                        #define mempll2_vco_div_sel           (0x00000001 <<29)
                        unsigned int mempll2_m4pdiv_1_0    = (0x00000000 << 10);//
                        #ifdef DDRPHY_3PLL_MODE
                            unsigned int mempll2_fbdiv_6_0 = (0x0000000a <<  2);
                        #else //DDRPHY_3PLL_MODE
                            unsigned int mempll2_fbdiv_6_0 = (0x00000029 <<  2);
                        #endif //DDRPHY_3PLL_MODE
                       #else
                        #ifdef MEMPLL_CLK_700
                        #define mempll2_vco_div_sel           (0x00000000 <<29)
                        unsigned int mempll2_m4pdiv_1_0    = (0x00000000 << 10);//
                        #ifdef DDRPHY_3PLL_MODE
                            unsigned int mempll2_fbdiv_6_0 = (0x0000000d <<  2);
                        #else //DDRPHY_3PLL_MODE
                            unsigned int mempll2_fbdiv_6_0 = (0x00000035 <<  2);
                        #endif //DDRPHY_3PLL_MODE

                        #else
                        #ifdef MEMPLL_CLK_793
                       #define mempll2_vco_div_sel           (0x00000000 <<29)
                        unsigned int mempll2_m4pdiv_1_0    = (0x00000000 << 10);//
                        #ifdef DDRPHY_3PLL_MODE
                            unsigned int mempll2_fbdiv_6_0 = (0x0000000f <<  2);
                        #else //DDRPHY_3PLL_MODE
                            unsigned int mempll2_fbdiv_6_0 = (0x0000003d <<  2);
                        #endif //DDRPHY_3PLL_MODE
                       #else
                       #ifdef MEMPLL_CLK_832
                       #define mempll2_vco_div_sel           (0x00000000 <<29)
                        unsigned int mempll2_m4pdiv_1_0    = (0x00000000 << 10);//
                        #ifdef DDRPHY_3PLL_MODE
                            unsigned int mempll2_fbdiv_6_0 = (0x00000010 <<  2);
                        #else //DDRPHY_3PLL_MODE
                            unsigned int mempll2_fbdiv_6_0 = (0x00000040 <<  2);
                        #endif //DDRPHY_3PLL_MODE
                       #else //1333
                       #ifdef MEMPLL_CLK_666
	                       #define mempll2_vco_div_sel           (0x00000000 <<29)
	                        unsigned int mempll2_m4pdiv_1_0    = (0x00000000 << 10);//
	                        #ifdef DDRPHY_3PLL_MODE
	                            unsigned int mempll2_fbdiv_6_0 = (0x0000000d <<  2); //(0x0000000c <<  2);
	                        #else //DDRPHY_3PLL_MODE
	                            unsigned int mempll2_fbdiv_6_0 = (0x00000033 <<  2);
	                        #endif //DDRPHY_3PLL_MODE
                        #else //todo:1600
                            #define mempll2_vco_div_sel           (0x00000000 <<29)
                            unsigned int mempll2_m4pdiv_1_0    = (0x00000000 << 10);//
	                        #ifdef DDRPHY_3PLL_MODE
                            unsigned int mempll2_fbdiv_6_0 = (0x0000000f <<  2);
	                        #else //DDRPHY_3PLL_MODE
                            unsigned int mempll2_fbdiv_6_0 = (0x00000033 <<  2);
	                        #endif //DDRPHY_3PLL_MODE
                        #endif
                    #endif
                #endif
            #endif
        #endif
    #endif
#endif
            #endif
        #endif
    #endif
#endif
#endif

#ifdef DDRPHY_3PLL_MODE
    unsigned int mempll2_fb_mck_sel = (0x00000001 << 9);
#else  //DDRPHY_3PLL_MODE
    unsigned int mempll2_fb_mck_sel = (0x00000000 << 9);
#endif //DDRPHY_3PLL_MODE

#define mempll2_fbksel_1_0     (0x00000000 << 10)
#define mempll2_posdiv_1_0     (0x00000000 << 30)
#define mempll2_ref_dl_4_0     (0x00000000 << 27)
#define mempll2_fb_dl_4_0      (0x00000000 << 22)
#define mempll2_en             (0x00000001 << 18)

#define mempll3_prediv_1_0     (0x00000000 <<  0)
/*#define mempll3_vco_div_sel    (0x00000001 << 29)*/
#if 0
#ifdef MEMPLL_CLK_500 //DDR1000
        unsigned int mempll3_m4pdiv_1_0 = (0x00000001 << 10);
    #ifdef DDRPHY_3PLL_MODE
        unsigned int mempll3_fbdiv_6_0  = (0x00000009 <<  2);
    #else //DDRPHY_3PLL_MODE
        unsigned int mempll3_fbdiv_6_0  = (0x00000050 <<  2);
    #endif //DDRPHY_3PLL_MODE
#else
    #ifdef MEMPLL_CLK_400 //DDR800
        unsigned int mempll3_m4pdiv_1_0    = (0x00000001 << 10);
        #ifdef DDRPHY_3PLL_MODE
            unsigned int mempll3_fbdiv_6_0 = (0x00000007 <<  2);
        #else //DDRPHY_3PLL_MODE
            unsigned int mempll3_fbdiv_6_0 = (0x00000040 <<  2);
        #endif //DDRPHY_3PLL_MODE
    #else
        #ifdef MEMPLL_CLK_250 //DDR500
            unsigned int mempll3_m4pdiv_1_0    = (0x00000002 << 10);
            #ifdef DDRPHY_3PLL_MODE
                unsigned int mempll3_fbdiv_6_0 = (0x00000004 <<  2);
            #else //DDRPHY_3PLL_MODE
                unsigned int mempll3_fbdiv_6_0 = (0x00000050 <<  2);
            #endif //DDRPHY_3PLL_MODE
        #else
            #ifdef MEMPLL_CLK_266 //DDR533
                #define mempll3_vco_div_sel    (0x00000001 << 29) //
                unsigned int mempll3_m4pdiv_1_0    = (0x00000001 << 10);
                #ifdef DDRPHY_3PLL_MODE
                    unsigned int mempll3_fbdiv_6_0 = (0x00000005 <<  2);
                #else //DDRPHY_3PLL_MODE
                    unsigned int mempll3_fbdiv_6_0 = (0x00000029 <<  2);
                #endif //DDRPHY_3PLL_MODE
            #else
                #ifdef MEMPLL_CLK_150 //DDR300
                    unsigned int mempll3_m4pdiv_1_0    = (0x00000002 << 10);
                    #ifdef DDRPHY_3PLL_MODE
                        unsigned int mempll3_fbdiv_6_0 = (0x00000002 <<  2);
                    #else //DDRPHY_3PLL_MODE
                        unsigned int mempll3_fbdiv_6_0 = (0x00000060 <<  2);
                    #endif //DDRPHY_3PLL_MODE
                #else
                    #ifdef MEMPLL_CLK_586 //DDD1172.6
                        unsigned int mempll3_m4pdiv_1_0    = (0x00000001 << 10);
                        #ifdef DDRPHY_3PLL_MODE
                            unsigned int mempll3_fbdiv_6_0 = (0x0000000b <<  2);
                        #else //DDRPHY_3PLL_MODE
                            unsigned int mempll3_fbdiv_6_0 = (0x00000058 <<  2);
                        #endif //DDRPHY_3PLL_MODE
                    #else //DDR1066
                         #ifdef MEMPLL_CLK_533
                          #define mempll3_vco_div_sel    (0x00000001 << 29) //
                        unsigned int mempll3_m4pdiv_1_0    = (0x00000000 << 10);
                        #ifdef DDRPHY_3PLL_MODE
                            unsigned int mempll3_fbdiv_6_0 = (0x0000000a <<  2);
                        #else //DDRPHY_3PLL_MODE
                            unsigned int mempll3_fbdiv_6_0 = (0x00000029<<  2);
                        #endif //DDRPHY_3PLL_MODE
                         #else
                       #ifdef MEMPLL_CLK_700 //1400
                        #define mempll3_vco_div_sel    (0x00000000 << 29) //
                        unsigned int mempll3_m4pdiv_1_0    = (0x00000000 << 10);
                        #ifdef DDRPHY_3PLL_MODE
                            unsigned int mempll3_fbdiv_6_0 = (0x0000000d<<  2);
                        #else //DDRPHY_3PLL_MODE
                            unsigned int mempll3_fbdiv_6_0 = (0x00000035<<  2);
                        #endif //DDRPHY_3PLL_MODE
                        #else
                       #ifdef MEMPLL_CLK_793
                       #define mempll3_vco_div_sel    (0x00000000 << 29) //
                        unsigned int mempll3_m4pdiv_1_0    = (0x00000000 << 10);
                        #ifdef DDRPHY_3PLL_MODE
                            unsigned int mempll3_fbdiv_6_0 = (0x0000000f<<  2);
                        #else //DDRPHY_3PLL_MODE
                            unsigned int mempll3_fbdiv_6_0 = (0x0000003d<<  2);
                        #endif //DDRPHY_3PLL_MODE
                         #else
                       #ifdef MEMPLL_CLK_832
                       #define mempll3_vco_div_sel    (0x00000000 << 29) //
                        unsigned int mempll3_m4pdiv_1_0    = (0x00000000 << 10);
                        #ifdef DDRPHY_3PLL_MODE
                            unsigned int mempll3_fbdiv_6_0 = (0x00000010<<  2);
                        #else //DDRPHY_3PLL_MODE
                            unsigned int mempll3_fbdiv_6_0 = (0x00000040<<  2);
                        #endif //DDRPHY_3PLL_MODE
                        #else //1333
                        #ifdef MEMPLL_CLK_666
	                       #define mempll3_vco_div_sel    (0x00000000 << 29) //
	                        unsigned int mempll3_m4pdiv_1_0    = (0x00000000 << 10);
	                        #ifdef DDRPHY_3PLL_MODE
	                            unsigned int mempll3_fbdiv_6_0 = (0x0000000d<<  2); //(0x0000000c<<  2);
	                        #else //DDRPHY_3PLL_MODE
	                            unsigned int mempll3_fbdiv_6_0 = (0x00000033<<  2);
	                        #endif //DDRPHY_3PLL_MODE
                        #else //todo:1600
                            #define mempll3_vco_div_sel    (0x00000000 << 29) //
                            unsigned int mempll3_m4pdiv_1_0    = (0x00000000 << 10);
	                        #ifdef DDRPHY_3PLL_MODE
                            unsigned int mempll3_fbdiv_6_0 = (0x0000000f<<  2);
	                        #else //DDRPHY_3PLL_MODE
                            unsigned int mempll3_fbdiv_6_0 = (0x00000033<<  2);
	                        #endif //DDRPHY_3PLL_MODE
                        #endif
                    #endif
                #endif
            #endif
        #endif
    #endif
#endif
            #endif
        #endif
    #endif
#endif
#endif

#ifdef DDRPHY_3PLL_MODE
    unsigned int mempll3_fb_mck_sel = (0x00000001 << 9);
#else  //DDRPHY_3PLL_MODE
    unsigned int mempll3_fb_mck_sel = (0x00000000 << 9);
#endif //DDRPHY_3PLL_MODE

#define mempll3_fbksel_1_0   (0x00000000 << 10)
#define mempll3_posdiv_1_0   (0x00000000 << 30)
#define mempll3_ref_dl_4_0   (0x00000000 << 27)
#define mempll3_fb_dl_4_0    (0x00000000 << 22)
#define mempll3_en           (0x00000001 << 18)

#define mempll4_prediv_1_0   (0x00000000 <<  0)
/*#define mempll4_vco_div_sel  (0x00000001 << 29)*/
#if 0
#ifdef MEMPLL_CLK_500 //DDR1000
    unsigned int mempll4_m4pdiv_1_0    = (0x00000001 << 10);
    #ifdef DDRPHY_3PLL_MODE
        unsigned int mempll4_fbdiv_6_0 = (0x00000009 <<  2);
    #else //DDRPHY_3PLL_MODE
        unsigned int mempll4_fbdiv_6_0 = (0x00000050 <<  2);
    #endif //DDRPHY_3PLL_MODE
#else
    #ifdef MEMPLL_CLK_400 //DDR800
        unsigned int mempll4_m4pdiv_1_0    = (0x00000001 << 10);
        #ifdef DDRPHY_3PLL_MODE
            unsigned int mempll4_fbdiv_6_0 = (0x00000007 <<  2);
        #else //DDRPHY_3PLL_MODE
            unsigned int mempll4_fbdiv_6_0 = (0x00000040 <<  2);
        #endif //DDRPHY_3PLL_MODE
    #else
        #ifdef MEMPLL_CLK_250 //DDR500
            unsigned int mempll4_m4pdiv_1_0    = (0x00000002 << 10);
            #ifdef DDRPHY_3PLL_MODE
                unsigned int mempll4_fbdiv_6_0 = (0x00000004 <<  2);
            #else //DDRPHY_3PLL_MODE
                unsigned int mempll4_fbdiv_6_0 = (0x00000050 <<  2);
            #endif //DDRPHY_3PLL_MODE
        #else
            #ifdef MEMPLL_CLK_266 //DDR533
               #define mempll4_vco_div_sel  (0x00000001 << 29) //
                unsigned int mempll4_m4pdiv_1_0    = (0x00000001 << 10);
                #ifdef DDRPHY_3PLL_MODE
                    unsigned int mempll4_fbdiv_6_0 = (0x00000005 <<  2);
                #else //DDRPHY_3PLL_MODE
                    unsigned int mempll4_fbdiv_6_0 = (0x00000029<<  2);
                #endif //DDRPHY_3PLL_MODE
            #else //DDR1066
                #ifdef MEMPLL_CLK_150 //DDR300
                    unsigned int mempll4_m4pdiv_1_0    = (0x00000002 << 10);
                    #ifdef DDRPHY_3PLL_MODE
                        unsigned int mempll4_fbdiv_6_0 = (0x00000002 <<  2);
                    #else //DDRPHY_3PLL_MODE
                        unsigned int mempll4_fbdiv_6_0 = (0x00000060 <<  2);
                    #endif //DDRPHY_3PLL_MODE
                #else
                    #ifdef MEMPLL_CLK_586 //DDD1172.6
                        unsigned int mempll4_m4pdiv_1_0    = (0x00000001 << 10);
                        #ifdef DDRPHY_3PLL_MODE
                            unsigned int mempll4_fbdiv_6_0 = (0x0000000b <<  2);
                        #else //DDRPHY_3PLL_MODE
                            unsigned int mempll4_fbdiv_6_0 = (0x00000058 <<  2);
                        #endif //DDRPHY_3PLL_MODE
                    #else //DDR1066
                      #ifdef MEMPLL_CLK_533
                        #define mempll4_vco_div_sel  (0x00000001 << 29) //
                        unsigned int mempll4_m4pdiv_1_0    = (0x00000000 << 10);
                        #ifdef DDRPHY_3PLL_MODE
                            unsigned int mempll4_fbdiv_6_0 = (0x0000000a <<  2);
                        #else //DDRPHY_3PLL_MODE
                            unsigned int mempll4_fbdiv_6_0 = (0x00000029<<  2);
                        #endif //DDRPHY_3PLL_MODE
                      #else
                      #ifdef MEMPLL_CLK_700
                        #define mempll4_vco_div_sel  (0x00000000 << 29) //
                        unsigned int mempll4_m4pdiv_1_0    = (0x00000000 << 10);
                        #ifdef DDRPHY_3PLL_MODE
                            unsigned int mempll4_fbdiv_6_0 = (0x0000000d<<  2);
                        #else //DDRPHY_3PLL_MODE
                            unsigned int mempll4_fbdiv_6_0 = (0x00000035<< 2);
                        #endif //DDRPHY_3PLL_MODE
                       #else
                       #ifdef MEMPLL_CLK_793
                        #define mempll4_vco_div_sel  (0x00000000 << 29) //
                        unsigned int mempll4_m4pdiv_1_0    = (0x00000000 << 10);
                        #ifdef DDRPHY_3PLL_MODE
                            unsigned int mempll4_fbdiv_6_0 = (0x0000000f<<  2);
                        #else //DDRPHY_3PLL_MODE
                            unsigned int mempll4_fbdiv_6_0 = (0x0000003d<< 2);
                        #endif //DDRPHY_3PLL_MODE
                         #else
                       #ifdef MEMPLL_CLK_832
                        #define mempll4_vco_div_sel  (0x00000000 << 29) //
                        unsigned int mempll4_m4pdiv_1_0    = (0x00000000 << 10);
                        #ifdef DDRPHY_3PLL_MODE
                            unsigned int mempll4_fbdiv_6_0 = (0x00000010<<  2);
                        #else //DDRPHY_3PLL_MODE
                            unsigned int mempll4_fbdiv_6_0 = (0x00000040<< 2);
                        #endif //DDRPHY_3PLL_MODE
                      #else //1333
                      #ifdef MEMPLL_CLK_666
	                        #define mempll4_vco_div_sel  (0x00000000 << 29) //
	                        unsigned int mempll4_m4pdiv_1_0    = (0x00000000 << 10);
	                        #ifdef DDRPHY_3PLL_MODE
	                            unsigned int mempll4_fbdiv_6_0 = (0x0000000d<<  2); //(0x0000000c<<  2);
	                        #else //DDRPHY_3PLL_MODE
	                            unsigned int mempll4_fbdiv_6_0 = (0x00000033<< 2);
	                        #endif //DDRPHY_3PLL_MODE
                       #else //todo:1600
                            #define mempll4_vco_div_sel  (0x00000000 << 29) //
                            unsigned int mempll4_m4pdiv_1_0    = (0x00000000 << 10);
	                        #ifdef DDRPHY_3PLL_MODE
                            unsigned int mempll4_fbdiv_6_0 = (0x0000000f<<  2);
	                        #else //DDRPHY_3PLL_MODE
                            unsigned int mempll4_fbdiv_6_0 = (0x00000033<< 2);
	                        #endif //DDRPHY_3PLL_MODE
                       #endif
                    #endif
                #endif
            #endif
        #endif
    #endif
#endif
            #endif
        #endif
    #endif
#endif
#endif

#ifdef DDRPHY_3PLL_MODE
    unsigned int mempll4_fb_mck_sel = (0x00000001 << 9);
#else  //DDRPHY_3PLL_MODE
    unsigned int mempll4_fb_mck_sel = (0x00000000 << 9);
#endif //DDRPHY_3PLL_MODE

#define mempll4_fbksel_1_0  (0x00000000 << 10)
#define mempll4_posdiv_1_0  (0x00000000 << 30)
#define mempll4_ref_dl_4_0  (0x00000000 << 27)
#define mempll4_fb_dl_4_0   (0x00000000 << 22)
#define mempll4_en          (0x00000001 << 18)

// new add
#define mempll2_bp   (0x00000001 << 27)  //183[27]
#define mempll2_br   (0x00000001 << 26)  //183[26]

#define mempll3_bp   (0x00000001 << 27)  //185[27]
#define mempll3_br   (0x00000001 << 26)  //185[26]

#define mempll4_bp   (0x00000001 << 27)  //187[27]
#define mempll4_br   (0x00000001 << 26)  //187[26]

#define DRAMC_READ_REG(offset)         (*(volatile unsigned int *)(DDRPHY_BASE + (offset)))

// todo: pll_mode control all related setting really
void mt_mempll_init_new(int type, int pll_mode)
{
    unsigned int temp;

    unsigned int pllc1_fbksel_1_0_n3;
    unsigned int pllc1_dmss_pcw_ncpo_30_24_n4;
    unsigned int pllc1_dmss_pcw_ncpo_23_0_n4;

    unsigned int mempll2_vco_div_sel;
    unsigned int mempll2_m4pdiv_1_0;
    unsigned int mempll2_fbdiv_6_0;
    
    unsigned int mempll3_vco_div_sel;
    unsigned int mempll3_m4pdiv_1_0;
    unsigned int mempll3_fbdiv_6_0;

    unsigned int mempll4_vco_div_sel;
    unsigned int mempll4_m4pdiv_1_0;
    unsigned int mempll4_fbdiv_6_0;

    // todo: ifndef DDRPHY_3PLL_MODE, need give different value for following setting:
    //unsigned int pllc1_fbksel_1_0_n3;
    //unsigned int pllc1_dmss_pcw_ncpo_30_24_n4;
    //unsigned int pllc1_dmss_pcw_ncpo_23_0_n4;
    //unsigned int mempll2_fbdiv_6_0;
    //unsigned int mempll3_fbdiv_6_0;
    //unsigned int mempll4_fbdiv_6_0;
    //mempll2_fb_mck_sel
    //mempll3_fb_mck_sel
    //mempll4_fb_mck_sel
    
    printf("%s, type:%d, pll_mode:%d\n", __FUNCTION__, type, pll_mode);
	
    switch(type)
    {
    case 533:
        pllc1_fbksel_1_0_n3 = 0x00000000 << 26;
        pllc1_dmss_pcw_ncpo_30_24_n4 = 0x00000054 << 25;
        pllc1_dmss_pcw_ncpo_23_0_n4 = 0x000ccccc << 1;

        mempll2_vco_div_sel = (0x00000001 << 29);
        mempll2_m4pdiv_1_0 = (0x00000001 << 10);
        mempll2_fbdiv_6_0 = (0x00000005 <<  2);

        mempll3_vco_div_sel = (0x00000001 << 29);
        mempll3_m4pdiv_1_0 = (0x00000001 << 10);
        mempll3_fbdiv_6_0 = (0x00000005 << 2);

        mempll4_vco_div_sel = (0x00000001 << 29);
        mempll4_m4pdiv_1_0 = (0x00000001 << 10);
        mempll4_fbdiv_6_0 = (0x00000005 << 2);
        break;

    case 667:
        pllc1_fbksel_1_0_n3 = 0x00000000 << 26;
        pllc1_dmss_pcw_ncpo_30_24_n4 = 0x00000057 << 25;
        pllc1_dmss_pcw_ncpo_23_0_n4 = 0x00266666 << 1;

        mempll2_vco_div_sel = (0x00000001 << 29);
        mempll2_m4pdiv_1_0 = (0x00000001 << 10);
        mempll2_fbdiv_6_0 = (0x00000006 <<  2);

        mempll3_vco_div_sel = (0x00000001 << 29);
        mempll3_m4pdiv_1_0 = (0x00000001 << 10);
        mempll3_fbdiv_6_0 = (0x00000006 << 2);

        mempll4_vco_div_sel = (0x00000001 << 29);
        mempll4_m4pdiv_1_0 = (0x00000001 << 10);
        mempll4_fbdiv_6_0 = (0x00000006 << 2);  
        break;

    case 800:
        pllc1_fbksel_1_0_n3 = 0x00000000 << 26;
        pllc1_dmss_pcw_ncpo_30_24_n4 = 0x0000004e << 25;
        pllc1_dmss_pcw_ncpo_23_0_n4 = 0x00599999 << 1;

        mempll2_vco_div_sel = (0x00000001 << 29);
        mempll2_m4pdiv_1_0 = (0x00000001 << 10);
        mempll2_fbdiv_6_0 = (0x00000008 <<  2);

        mempll3_vco_div_sel = (0x00000001 << 29);
        mempll3_m4pdiv_1_0 = (0x00000001 << 10);
        mempll3_fbdiv_6_0 = (0x00000008 << 2);

        mempll4_vco_div_sel = (0x00000001 << 29);
        mempll4_m4pdiv_1_0 = (0x00000001 << 10);
        mempll4_fbdiv_6_0 = (0x00000008 << 2);  
        break;

    case 1066:
        pllc1_fbksel_1_0_n3 = 0x00000000 << 26;
        pllc1_dmss_pcw_ncpo_30_24_n4 = 0x00000054 << 25;
        pllc1_dmss_pcw_ncpo_23_0_n4 = 0x000ccccc << 1;

        mempll2_vco_div_sel = (0x00000001 << 29);
        mempll2_m4pdiv_1_0 = (0x00000000 << 10);
        mempll2_fbdiv_6_0 = (0x0000000a <<  2);

        mempll3_vco_div_sel = (0x00000001 << 29);
        mempll3_m4pdiv_1_0 = (0x00000000 << 10);
        mempll3_fbdiv_6_0 = (0x0000000a << 2);

        mempll4_vco_div_sel = (0x00000001 << 29);
        mempll4_m4pdiv_1_0 = (0x00000000 << 10);
        mempll4_fbdiv_6_0 = (0x0000000a << 2);      
        break;

    case 1333:
        pllc1_fbksel_1_0_n3 = 0x00000000 << 26;
        pllc1_dmss_pcw_ncpo_30_24_n4 = 0x00000050 << 25;
        pllc1_dmss_pcw_ncpo_23_0_n4 = 0x00d8fe7c << 1;

        mempll2_vco_div_sel = (0x00000000 << 29);
        mempll2_m4pdiv_1_0 = (0x00000000 << 10);
        mempll2_fbdiv_6_0 = (0x0000000d <<  2);

        mempll3_vco_div_sel = (0x00000000 << 29);
        mempll3_m4pdiv_1_0 = (0x00000000 << 10);
        mempll3_fbdiv_6_0 = (0x0000000d << 2);

        mempll4_vco_div_sel = (0x00000000 << 29);
        mempll4_m4pdiv_1_0 = (0x00000000 << 10);
        mempll4_fbdiv_6_0 = (0x0000000d << 2);  
        break;

    case 1500:
        pllc1_fbksel_1_0_n3 = 0x00000000 << 26;
        pllc1_dmss_pcw_ncpo_30_24_n4 = 0x00000054 << 25;
        pllc1_dmss_pcw_ncpo_23_0_n4 = 0x001a41a4 << 1;

        mempll2_vco_div_sel = (0x00000000 << 29);
        mempll2_m4pdiv_1_0 = (0x00000000 << 10);
        mempll2_fbdiv_6_0 = (0x0000000e <<  2);

        mempll3_vco_div_sel = (0x00000000 << 29);
        mempll3_m4pdiv_1_0 = (0x00000000 << 10);
        mempll3_fbdiv_6_0 = (0x0000000e << 2);

        mempll4_vco_div_sel = (0x00000000 << 29);
        mempll4_m4pdiv_1_0 = (0x00000000 << 10);
        mempll4_fbdiv_6_0 = (0x0000000e << 2);    
        break;
		
    case 1600:      
    default:
        pllc1_fbksel_1_0_n3 = 0x00000000 << 26;
        pllc1_dmss_pcw_ncpo_30_24_n4 = 0x00000054 << 25;
        pllc1_dmss_pcw_ncpo_23_0_n4 = 0x001a41a4 << 1;

        mempll2_vco_div_sel = (0x00000000 << 29);
        mempll2_m4pdiv_1_0 = (0x00000000 << 10);
        mempll2_fbdiv_6_0 = (0x0000000f <<  2);

        mempll3_vco_div_sel = (0x00000000 << 29);
        mempll3_m4pdiv_1_0 = (0x00000000 << 10);
        mempll3_fbdiv_6_0 = (0x0000000f << 2);

        mempll4_vco_div_sel = (0x00000000 << 29);
        mempll4_m4pdiv_1_0 = (0x00000000 << 10);
        mempll4_fbdiv_6_0 = (0x0000000f << 2);          
        break;
    }

    
    /*********************************
        * (1) Setup DDRPHY operation mode
        **********************************/
    temp = 0x00000001;  //set div2 mode  before mempll setting
    DRAMC_WRITE_REG(temp, (0x001f << 2));
        
#ifdef DRAMC_ASYNC
    #ifdef DDRPHY_3PLL_MODE
        #ifdef DDRY2PLL_ASYNC_MODE
        temp = (0x00000011); // 3PLL async mode bypass PLL4
        #else
        temp = (0x00000010 ); // 3PLL mode
        #endif
        DRAMC_WRITE_REG(temp, (0x0190 << 2));
    #else
        temp = (0x00000003); // 1PLL async mode
        DRAMC_WRITE_REG(temp, (0x0190 << 2));
    #endif
#else
    #ifdef DDRPHY_3PLL_MODE
        #ifdef DDRY2PLL_SYNC_MODE
        temp = (0x00000012 ); // 3PLL sync mode bypass PLL3
        #else
        temp = (0x00000010 ); // 3PLL sync mode
        #endif
        DRAMC_WRITE_REG(temp, (0x0190 << 2));
    #else
        temp = (0x00000007); // 1PLL sync mode
        DRAMC_WRITE_REG(temp, (0x0190 << 2));
    #endif
#endif


    /*****************************************************************************************
        * (2) Setup MEMPLL operation case & frequency. May set according to dram type & frequency
        ******************************************************************************************/
    
    temp = r_bias_en_stb_time | r_bias_lpf_en_stb_time | r_mempll_en_stb_time | r_dmall_ck_en_stb_time;
    DRAMC_WRITE_REG(temp, (0x0170 << 2));
    
    temp = r_dds_en_stb_time | r_div_en_stb_time | r_dmpll2_ck_en_stb_time | r_iso_en_stb_time;
    DRAMC_WRITE_REG(temp, (0x0171 << 2));
    
    temp = r_bias_en_stb_dis | r_bias_en_src_sel | r_bias_lpf_en_stb_dis | r_bias_lpf_en_src_sel | r_mempll4_en_stb_dis | r_mempll3_en_stb_dis |
           r_mempll2_en_stb_dis | r_mempll_en_stb_dis| r_mempll4_en_src_sel | r_mempll3_en_src_sel | r_mempll2_en_src_sel | r_mempll_en_src_sel |
           r_dmall_ck_en_stb_dis | r_dmall_ck_en_src_sel;
    DRAMC_WRITE_REG(temp, (0x0172 << 2));
    
    temp = r_dds_en_stb_dis | r_dds_en_src_sel | r_div_en_stb_dis| r_div_en_src_sel | r_dmpll2_ck_en_stb_dis | r_dmpll2_ck_en_src_sel |
           r_iso_en_stb_dis | r_iso_en_src_sel | r_dmbyp_pll4 | r_dmbyp_pll3 | r_dm1pll_sync_mode | r_dmall_ck_en | r_dmpll2_clk_en;
    DRAMC_WRITE_REG(temp, (0x0173 << 2));
    
    temp = pllc1_prediv_1_0_n1  | pllc1_blp | pllc1_fbksel_1_0_n3 | pllc1_postdiv_1_0_n5;
    DRAMC_WRITE_REG(temp, (0x0180 << 2));
    
    temp = pllc1_mempll_div_6_0 | pllc1_mempll_refck_en;
    DRAMC_WRITE_REG(temp, (0x0181 << 2));
    
    temp = mempll2_vco_div_sel | mempll2_m4pdiv_1_0 | mempll2_fb_mck_sel | mempll2_posdiv_1_0 | mempll2_bp | mempll2_br;
    DRAMC_WRITE_REG(temp, (0x0183 << 2));
    
    temp = mempll2_prediv_1_0 | mempll2_fbdiv_6_0 | mempll2_fbksel_1_0;
    temp = temp &(0xFFF7FFFF);// For 8135 MEMPLL top_reserve_3 bit19=0
    DRAMC_WRITE_REG(temp, (0x0182 << 2));
    
    temp = mempll3_vco_div_sel | mempll3_m4pdiv_1_0 | mempll3_fb_mck_sel | mempll3_posdiv_1_0| mempll3_bp | mempll3_br;
    DRAMC_WRITE_REG(temp, (0x0185 << 2));
    
    temp = mempll2_ref_dl_4_0 | mempll2_fb_dl_4_0 | mempll3_prediv_1_0 | mempll3_fbdiv_6_0 | mempll3_fbksel_1_0;
    DRAMC_WRITE_REG(temp, (0x0184 << 2));
    
    temp = mempll4_vco_div_sel | mempll4_m4pdiv_1_0 | mempll4_fb_mck_sel | mempll4_posdiv_1_0 | mempll4_bp | mempll4_br;
    DRAMC_WRITE_REG(temp, (0x0187 << 2));
    
    temp = mempll3_fb_dl_4_0 | mempll3_ref_dl_4_0 | mempll4_prediv_1_0 | mempll4_fbdiv_6_0 | mempll4_fbksel_1_0;
    DRAMC_WRITE_REG(temp, (0x0186 << 2));
    
    temp = mempll4_ref_dl_4_0 | mempll4_fb_dl_4_0;
    DRAMC_WRITE_REG(temp, (0x0188 << 2));
    
    temp = pllc1_dmss_pcw_ncpo_23_0_n4 | pllc1_dmss_pcw_ncpo_30_24_n4  | pllc1_dmss_pcw_ncpo_chg;
    DRAMC_WRITE_REG(temp, (0x0189 << 2));
    
    //DRAMC_WRITE_REG(0x003F0000, (0x018C << 2)); // SDM PLL tie high default value
	DRAMC_WRITE_REG(0x003A0000, (0x018C << 2)); 

    /***********************************
        * (3) Setup MEMPLL power on sequence
        ************************************/
        
    gpt_busy_wait_us(5); // min delay is 2us
    
    temp = pllc1_mempll_div_6_0 | pllc1_mempll_refck_en | (pllc1_mempll_bias_en );
    DRAMC_WRITE_REG(temp, (0x0181 << 2));
    
    gpt_busy_wait_us(5); // min delay is 1us
    
    temp = pllc1_mempll_div_6_0 | pllc1_mempll_refck_en | (pllc1_mempll_bias_en | pllc1_mempll_bias_lpf_en);
    DRAMC_WRITE_REG(temp, (0x0181 << 2));
    
    gpt_busy_wait_ms(5); // min delay is 1ms
    
    temp = pllc1_prediv_1_0_n1  | pllc1_blp | pllc1_fbksel_1_0_n3 | pllc1_postdiv_1_0_n5  | (pllc1_mempll_en);
    DRAMC_WRITE_REG(temp, (0x0180 << 2));
    
    gpt_busy_wait_us(50); // min delay is 20us
    
    temp = 0x10000000;
    DRAMC_WRITE_REG(temp, (0x018a << 2));
    
    gpt_busy_wait_us(5); // min delay is 2us
    
    temp = 0x18000000;
    DRAMC_WRITE_REG(temp, (0x018a << 2));
    
    temp = mempll4_ref_dl_4_0 | mempll4_fb_dl_4_0 | (pllc1_dmss_ncpo_en);
    DRAMC_WRITE_REG(temp, (0x0188 << 2));
    
    gpt_busy_wait_us(5); // min delay is 2us
    
    temp = pllc1_mempll_dds_en | (pllc1_prediv_1_0_n1  | pllc1_blp | pllc1_fbksel_1_0_n3 | pllc1_postdiv_1_0_n5  | (pllc1_mempll_en));
    DRAMC_WRITE_REG(temp, (0x0180 << 2));
    
    temp = pllc1_mempll_div_en | (pllc1_mempll_div_6_0 | pllc1_mempll_refck_en | (pllc1_mempll_bias_en | pllc1_mempll_bias_lpf_en));
    DRAMC_WRITE_REG(temp, (0x0181 << 2));
    
    gpt_busy_wait_us(50); // min delay is 23us
    
    temp = mempll2_en | mempll2_vco_div_sel | mempll2_m4pdiv_1_0 | mempll2_fb_mck_sel | mempll2_posdiv_1_0 | mempll2_bp | mempll2_br;
    DRAMC_WRITE_REG(temp, (0x0183 << 2));
    
    temp = mempll3_en | mempll3_vco_div_sel | mempll3_m4pdiv_1_0 | mempll3_fb_mck_sel | mempll3_posdiv_1_0 | mempll3_bp | mempll3_br;
    DRAMC_WRITE_REG(temp, (0x0185 << 2));
    
    temp = mempll4_en | mempll4_vco_div_sel | mempll4_m4pdiv_1_0 | mempll4_fb_mck_sel | mempll4_posdiv_1_0 | mempll4_bp | mempll4_br;
    DRAMC_WRITE_REG(temp, (0x0187 << 2));
    
    gpt_busy_wait_us(30); // min delay is 23us
    
#ifdef DRAMC_ASYNC
    #ifdef DDRPHY_3PLL_MODE
        #ifdef DDRY2PLL_ASYNC_MODE
        temp = 0x00003f21 | r_dmpll2_clk_en | r_dmall_ck_en; // 3PLL mode bypass PLL4
        #else
        temp = 0x00003f20 | r_dmpll2_clk_en | r_dmall_ck_en; // 3PLL mode
        #endif
        DRAMC_WRITE_REG(temp, (0x0190 << 2));
    #else
        temp = 0x00003f03 | r_dmpll2_clk_en | r_dmall_ck_en; // 1PLL async mode
        DRAMC_WRITE_REG(temp, (0x0190 << 2));
    #endif
#else
    #ifdef DDRPHY_3PLL_MODE
        #ifdef DDRY2PLL_SYNC_MODE
        temp = 0x00003f22 | r_dmpll2_clk_en | r_dmall_ck_en; // 3PLL sync mode bypss PLL3
        #else
        temp = 0x00003f20 | r_dmpll2_clk_en | r_dmall_ck_en; // 3PLL sync mode
        #endif
        DRAMC_WRITE_REG(temp, (0x0190 << 2));
    #else
        temp = 0x00003f07 | r_dmpll2_clk_en | r_dmall_ck_en; // 1PLL sync mode
        DRAMC_WRITE_REG(temp, (0x0190 << 2));
    #endif
#endif

 
    /*****************************************************
     * MEMPLL control switch to SPM for ISO_EN zero delay
     *****************************************************/   
#ifdef DDRPHY_3PLL_MODE
    temp = 0x00101010; // [0]ISO_EN_SRC=0, [22]DIV_EN_SC_SRC=0 (pll2off), [20]DIV_EN_DLY=1 (no delay), [16]DIV_EN_SRC=0, [8]PLL2_CK_EN_SRC=1(1pll), [8]PLL2_CK_EN_SRC=0(3pll)
    DRAMC_WRITE_REG(temp, (0x0173 << 2));
#else
    temp = 0x00101010; // [0]ISO_EN_SRC=0, [22]DIV_EN_SC_SRC=0 (pll2off), [20]DIV_EN_DLY=1 (no delay), [16]DIV_EN_SRC=0, [8]PLL2_CK_EN_SRC=1(1pll), [8]PLL2_CK_EN_SRC=0(3pll)
    DRAMC_WRITE_REG(temp, (0x0173 << 2));
#endif

    
    /***************************************************
     * Setting for MEMPLL_EN control by sc_mempll3_off
     ***************************************************/
    
    temp = 0x0000F010; // [24]BIAS_EN_SRC=0, [16]BIAS_LPF_EN_SRC=0, [8]MEMPLL_EN, [9][10][11]MEMPLL2,3,4_EN_SRC, [0]ALL_CK_EN_SRC=0
    DRAMC_WRITE_REG(temp, (0x0172 << 2));
    
    temp = 0x00021B96; // Setting for delay time, BIAS_LPF_EN delay time=2T
    DRAMC_WRITE_REG(temp, (0x0170 << 2));  

    #if 0
    DRAMC_WRITE_REG(DRAMC_READ_REG(0x0190<<2) & ~(1<<17), (0x0190 << 2));
    DRAMC_WRITE_REG(DRAMC_READ_REG(0x0190<<2) & ~(1<<18), (0x0190 << 2));
    DRAMC_WRITE_REG(DRAMC_READ_REG(0x0190<<2) | (1<<1), (0x0190 << 2));
    #endif
}


#if 0
void mt_mempll_init(void)
{
    unsigned int temp;

    /****************************************
     * (1) Setup DDRPHY operation mode
     ****************************************/
    temp = 0x00000001;  //set div2 mode  before mempll setting
    DRAMC_WRITE_REG(temp, (0x001f << 2));

    #ifdef DRAMC_ASYNC
        #ifdef DDRPHY_3PLL_MODE
            #ifdef DDRY2PLL_ASYNC_MODE
             temp = (0x00000011); // 3PLL async mode bypass PLL4
            #else
            temp = (0x00000010 ); // 3PLL mode
            #endif
            DRAMC_WRITE_REG(temp, (0x0190 << 2));
        #else
            temp = (0x00000003); // 1PLL async mode
            DRAMC_WRITE_REG(temp, (0x0190 << 2));
        #endif
    #else
        #ifdef DDRPHY_3PLL_MODE
            #ifdef DDRY2PLL_SYNC_MODE
            temp = (0x00000012 ); // 3PLL sync mode bypass PLL3
            #else
            temp = (0x00000010 ); // 3PLL sync mode
            #endif
            DRAMC_WRITE_REG(temp, (0x0190 << 2));
        #else
            temp = (0x00000007); // 1PLL sync mode
            DRAMC_WRITE_REG(temp, (0x0190 << 2));
        #endif
    #endif

    /*******************************************************************************************
     * (2) Setup MEMPLL operation case & frequency, May set according to dram type & frequency
     *******************************************************************************************/

    temp = r_bias_en_stb_time | r_bias_lpf_en_stb_time | r_mempll_en_stb_time | r_dmall_ck_en_stb_time;
    DRAMC_WRITE_REG(temp, (0x0170 << 2));

    temp = r_dds_en_stb_time | r_div_en_stb_time | r_dmpll2_ck_en_stb_time | r_iso_en_stb_time;
    DRAMC_WRITE_REG(temp, (0x0171 << 2));

    temp = r_bias_en_stb_dis | r_bias_en_src_sel | r_bias_lpf_en_stb_dis | r_bias_lpf_en_src_sel | r_mempll4_en_stb_dis | r_mempll3_en_stb_dis |
           r_mempll2_en_stb_dis | r_mempll_en_stb_dis| r_mempll4_en_src_sel | r_mempll3_en_src_sel | r_mempll2_en_src_sel | r_mempll_en_src_sel |
           r_dmall_ck_en_stb_dis | r_dmall_ck_en_src_sel;
    DRAMC_WRITE_REG(temp, (0x0172 << 2));

    temp = r_dds_en_stb_dis | r_dds_en_src_sel | r_div_en_stb_dis| r_div_en_src_sel | r_dmpll2_ck_en_stb_dis | r_dmpll2_ck_en_src_sel |
           r_iso_en_stb_dis | r_iso_en_src_sel | r_dmbyp_pll4 | r_dmbyp_pll3 | r_dm1pll_sync_mode | r_dmall_ck_en | r_dmpll2_clk_en;
    DRAMC_WRITE_REG(temp, (0x0173 << 2));

    temp = pllc1_prediv_1_0_n1  | pllc1_blp | pllc1_fbksel_1_0_n3 | pllc1_postdiv_1_0_n5;
    DRAMC_WRITE_REG(temp, (0x0180 << 2));

    temp = pllc1_mempll_div_6_0 | pllc1_mempll_refck_en;
    DRAMC_WRITE_REG(temp, (0x0181 << 2));

    temp = mempll2_vco_div_sel | mempll2_m4pdiv_1_0 | mempll2_fb_mck_sel | mempll2_posdiv_1_0 | mempll2_bp | mempll2_br;
    DRAMC_WRITE_REG(temp, (0x0183 << 2));

    temp = mempll2_prediv_1_0 | mempll2_fbdiv_6_0 | mempll2_fbksel_1_0;
    temp = temp &(0xFFF7FFFF);// For 8135 MEMPLL top_reserve_3 bit19=0
    DRAMC_WRITE_REG(temp, (0x0182 << 2));

    temp = mempll3_vco_div_sel | mempll3_m4pdiv_1_0 | mempll3_fb_mck_sel | mempll3_posdiv_1_0| mempll3_bp | mempll3_br;
    DRAMC_WRITE_REG(temp, (0x0185 << 2));

    temp = mempll2_ref_dl_4_0 | mempll2_fb_dl_4_0 | mempll3_prediv_1_0 | mempll3_fbdiv_6_0 | mempll3_fbksel_1_0;
    DRAMC_WRITE_REG(temp, (0x0184 << 2));

    temp = mempll4_vco_div_sel | mempll4_m4pdiv_1_0 | mempll4_fb_mck_sel | mempll4_posdiv_1_0 | mempll4_bp | mempll4_br;
    DRAMC_WRITE_REG(temp, (0x0187 << 2));

    temp = mempll3_fb_dl_4_0 | mempll3_ref_dl_4_0 | mempll4_prediv_1_0 | mempll4_fbdiv_6_0 | mempll4_fbksel_1_0;
    DRAMC_WRITE_REG(temp, (0x0186 << 2));

    temp = mempll4_ref_dl_4_0 | mempll4_fb_dl_4_0;
    DRAMC_WRITE_REG(temp, (0x0188 << 2));

    temp = pllc1_dmss_pcw_ncpo_23_0_n4 | pllc1_dmss_pcw_ncpo_30_24_n4  | pllc1_dmss_pcw_ncpo_chg;
    DRAMC_WRITE_REG(temp, (0x0189 << 2));

    DRAMC_WRITE_REG(0x003F0000, (0x018C << 2)); // SDM PLL tie high default value

    /*************************************
     * (3) Setup MEMPLL power on sequence
     *************************************/

    gpt_busy_wait_us(5); // min delay is 2us

    temp = pllc1_mempll_div_6_0 | pllc1_mempll_refck_en | (pllc1_mempll_bias_en );
    DRAMC_WRITE_REG(temp, (0x0181 << 2));

    gpt_busy_wait_us(5); // min delay is 1us

    temp = pllc1_mempll_div_6_0 | pllc1_mempll_refck_en | (pllc1_mempll_bias_en | pllc1_mempll_bias_lpf_en);
    DRAMC_WRITE_REG(temp, (0x0181 << 2));

    gpt_busy_wait_ms(5); // min delay is 1ms

    temp = pllc1_prediv_1_0_n1  | pllc1_blp | pllc1_fbksel_1_0_n3 | pllc1_postdiv_1_0_n5  | (pllc1_mempll_en);
    DRAMC_WRITE_REG(temp, (0x0180 << 2));

    gpt_busy_wait_us(50); // min delay is 20us

    temp = 0x10000000;
    DRAMC_WRITE_REG(temp, (0x018a << 2));

    gpt_busy_wait_us(5); // min delay is 2us

    temp = 0x18000000;
    DRAMC_WRITE_REG(temp, (0x018a << 2));

    temp = mempll4_ref_dl_4_0 | mempll4_fb_dl_4_0 | (pllc1_dmss_ncpo_en);
    DRAMC_WRITE_REG(temp, (0x0188 << 2));

    gpt_busy_wait_us(5); // min delay is 2us

    temp = pllc1_mempll_dds_en | (pllc1_prediv_1_0_n1  | pllc1_blp | pllc1_fbksel_1_0_n3 | pllc1_postdiv_1_0_n5  | (pllc1_mempll_en));
    DRAMC_WRITE_REG(temp, (0x0180 << 2));

    temp = pllc1_mempll_div_en | (pllc1_mempll_div_6_0 | pllc1_mempll_refck_en | (pllc1_mempll_bias_en | pllc1_mempll_bias_lpf_en));
    DRAMC_WRITE_REG(temp, (0x0181 << 2));

    gpt_busy_wait_us(50); // min delay is 23us

    temp = mempll2_en | mempll2_vco_div_sel | mempll2_m4pdiv_1_0 | mempll2_fb_mck_sel | mempll2_posdiv_1_0 | mempll2_bp | mempll2_br;
    DRAMC_WRITE_REG(temp, (0x0183 << 2));

    temp = mempll3_en | mempll3_vco_div_sel | mempll3_m4pdiv_1_0 | mempll3_fb_mck_sel | mempll3_posdiv_1_0 | mempll3_bp | mempll3_br;
    DRAMC_WRITE_REG(temp, (0x0185 << 2));

    temp = mempll4_en | mempll4_vco_div_sel | mempll4_m4pdiv_1_0 | mempll4_fb_mck_sel | mempll4_posdiv_1_0 | mempll4_bp | mempll4_br;
    DRAMC_WRITE_REG(temp, (0x0187 << 2));

    gpt_busy_wait_us(30); // min delay is 23us

    #ifdef DRAMC_ASYNC
        #ifdef DDRPHY_3PLL_MODE
            #ifdef DDRY2PLL_ASYNC_MODE
            temp = 0x00003f21 | r_dmpll2_clk_en | r_dmall_ck_en; // 3PLL mode bypass PLL4
            #else
            temp = 0x00003f20 | r_dmpll2_clk_en | r_dmall_ck_en; // 3PLL mode
            #endif
            DRAMC_WRITE_REG(temp, (0x0190 << 2));
        #else
            temp = 0x00003f03 | r_dmpll2_clk_en | r_dmall_ck_en; // 1PLL async mode
            DRAMC_WRITE_REG(temp, (0x0190 << 2));
        #endif
    #else
        #ifdef DDRPHY_3PLL_MODE
            #ifdef DDRY2PLL_SYNC_MODE
            temp = 0x00003f22 | r_dmpll2_clk_en | r_dmall_ck_en; // 3PLL sync mode bypss PLL3
            #else
            temp = 0x00003f20 | r_dmpll2_clk_en | r_dmall_ck_en; // 3PLL sync mode
            #endif
            DRAMC_WRITE_REG(temp, (0x0190 << 2));
        #else
            temp = 0x00003f07 | r_dmpll2_clk_en | r_dmall_ck_en; // 1PLL sync mode
            DRAMC_WRITE_REG(temp, (0x0190 << 2));
        #endif
    #endif

    /*****************************************************
     * MEMPLL control switch to SPM for ISO_EN zero delay
     *****************************************************/

    #ifdef DDRPHY_3PLL_MODE
        temp = 0x00101010; // [0]ISO_EN_SRC=0, [22]DIV_EN_SC_SRC=0 (pll2off), [20]DIV_EN_DLY=1 (no delay), [16]DIV_EN_SRC=0, [8]PLL2_CK_EN_SRC=1(1pll), [8]PLL2_CK_EN_SRC=0(3pll)
        DRAMC_WRITE_REG(temp, (0x0173 << 2));
    #else
        temp = 0x00101010; // [0]ISO_EN_SRC=0, [22]DIV_EN_SC_SRC=0 (pll2off), [20]DIV_EN_DLY=1 (no delay), [16]DIV_EN_SRC=0, [8]PLL2_CK_EN_SRC=1(1pll), [8]PLL2_CK_EN_SRC=0(3pll)
        DRAMC_WRITE_REG(temp, (0x0173 << 2));
    #endif

    /***************************************************
     * Setting for MEMPLL_EN control by sc_mempll3_off
     ***************************************************/

    temp = 0x0000F010; // [24]BIAS_EN_SRC=0, [16]BIAS_LPF_EN_SRC=0, [8]MEMPLL_EN, [9][10][11]MEMPLL2,3,4_EN_SRC, [0]ALL_CK_EN_SRC=0
    DRAMC_WRITE_REG(temp, (0x0172 << 2));

    temp = 0x00021B96; // Setting for delay time, BIAS_LPF_EN delay time=2T
    DRAMC_WRITE_REG(temp, (0x0170 << 2));

    #ifdef ACD_TEST_MODE
         temp=DRAMC_READ_REG(0x0183<<2);
         temp=(temp &0xE3FFFFFF);
         DRAMC_WRITE_REG(temp, (0x0183<<2));

         temp=DRAMC_READ_REG(0x0181<<2);
         temp=(temp &0x01FFFFFF)|0x52000000;
         DRAMC_WRITE_REG(temp, (0x0181<<2));

         temp=DRAMC_READ_REG(0x0182<<2);
         temp=(temp &0xFFFFFE03)|0x00000018;
         DRAMC_WRITE_REG(temp, (0x0182<<2));

         temp=DRAMC_READ_REG(0x0189<<2);
         temp=(temp &0x00000001)|0x96254654;
         DRAMC_WRITE_REG(temp, (0x0189<<2));

         temp=DRAMC_READ_REG(0x0185<<2);
         temp=(temp &0xE3FFFFFF);
         DRAMC_WRITE_REG(temp, (0x0185<<2));

         temp=DRAMC_READ_REG(0x0184<<2);
         temp=(temp &0xFFFFFE03)|0x00000018;
         DRAMC_WRITE_REG(temp, (0x0184<<2));

         temp=DRAMC_READ_REG(0x0187<<2);
         temp=(temp &0xE3FFFFFF);
         DRAMC_WRITE_REG(temp, (0x0187<<2));

         temp=DRAMC_READ_REG(0x0186<<2);
         temp=(temp &0xFFFFFE03)|0x00000018;
         DRAMC_WRITE_REG(temp, (0x0186<<2));

         temp=DRAMC_READ_REG(0x0189<<2);
         temp=(temp |0x00000001);
         DRAMC_WRITE_REG(temp, (0x0189<<2));

         temp=DRAMC_READ_REG(0x0189<<2);
         temp=(temp &0xFFFFFFFE);
         DRAMC_WRITE_REG(temp, (0x0189<<2));
    #endif

    #ifdef MEMPLL_CLK_733MHZ
         temp=0xA5205A04;
         DRAMC_WRITE_REG(temp, (0x0189<<2));

         temp=DRAMC_READ_REG(0x0187<<2);
         temp=(temp |0x00000400)&0xFFFFF7FF;
         DRAMC_WRITE_REG(temp, (0x0187<<2));

         temp=0xA5205A05;
         DRAMC_WRITE_REG(temp, (0x0189<<2));
    #endif
    #ifdef ACD_TEST_MODE_1600
         temp=DRAMC_READ_REG(0x0183<<2);
         temp=(temp &0xE3FFFFFF);
         DRAMC_WRITE_REG(temp, (0x0183<<2));

         temp=DRAMC_READ_REG(0x0181<<2);
         temp=(temp &0x01FFFFFF)|0x52000000;
         DRAMC_WRITE_REG(temp, (0x0181<<2));

         temp=DRAMC_READ_REG(0x0182<<2);
         //temp=(temp &0xFFFFFE03)|0x00000018;
         temp=(temp &0xFFFFFE03)|0x0000001c;
         DRAMC_WRITE_REG(temp, (0x0182<<2));

         // [0]?
         //temp=DRAMC_READ_REG(0x0189<<2);
         //temp=(temp &0x00000001)|0x96254654;
         temp = 0x9db13b12;
         DRAMC_WRITE_REG(temp, (0x0189<<2));

         temp=DRAMC_READ_REG(0x0185<<2);
         temp=(temp &0xE3FFFFFF);
         DRAMC_WRITE_REG(temp, (0x0185<<2));

         temp=DRAMC_READ_REG(0x0184<<2);
         //temp=(temp &0xFFFFFE03)|0x00000018;
         temp=(temp &0xFFFFFE03)|0x0000001c;
         DRAMC_WRITE_REG(temp, (0x0184<<2));

         // [28:26]?
         temp=DRAMC_READ_REG(0x0187<<2);
         //temp=(temp &0xE3FFFFFF);
         temp=(temp &0xFFFFF3FF)|0x00000400;
         DRAMC_WRITE_REG(temp, (0x0187<<2));

         temp=DRAMC_READ_REG(0x0186<<2);
         //temp=(temp &0xFFFFFE03)|0x00000018;
         temp=(temp &0xFFFFFE03)|0x0000001c;
         DRAMC_WRITE_REG(temp, (0x0186<<2));

         // [1, 0]?
         //temp=DRAMC_READ_REG(0x0189<<2);
         //temp=(temp |0x00000001);
         //DRAMC_WRITE_REG(temp, (0x0189<<2));

         //temp=DRAMC_READ_REG(0x0189<<2);
         //temp=(temp &0xFFFFFFFE);
         //DRAMC_WRITE_REG(temp, (0x0189<<2));
         temp = 0x9db13b13;
         DRAMC_WRITE_REG(temp, (0x0189<<2));
    #endif
		#if 0
    DRAMC_WRITE_REG(DRAMC_READ_REG(0x0190<<2) & ~(1<<17), (0x0190 << 2));
    DRAMC_WRITE_REG(DRAMC_READ_REG(0x0190<<2) & ~(1<<18), (0x0190 << 2));
    DRAMC_WRITE_REG(DRAMC_READ_REG(0x0190<<2) | (1<<1), (0x0190 << 2));
    #endif
}
#endif

int mt_mempll_cali(void)
{
    int one_count = 0, zero_count = 0;
    int pll2_done = 0, pll3_done = 0, pll4_done = 0, ret = 0;

    unsigned int temp = 0, pll2_dl = 0, pll3_dl = 0, pll4_dl = 0;

    /***********************************************
    * 1. Set jitter meter clock to internal FB path
    ************************************************/
    //temp = DRV_Reg32(0x1000f60C);
    //DRV_WriteReg32(0x1000f60C, temp & ~0x200); // 0x1000f60C[9] = 0 PLL2

    //temp = DRV_Reg32(0x1000f614);
    //DRV_WriteReg32(0x1000f614, temp & ~0x200); // 0x1000f614[9] = 0 PLL3

    //temp = DRV_Reg32(0x1000f61C);
    //DRV_WriteReg32(0x1000f61C, temp & ~0x200); // 0x1000f61C[9] = 0 PLL4

    /***********************************************
    * 2. Set jitter meter count number
    ************************************************/

    temp = DRV_Reg32(0x1000f1CC) & 0x0000FFFF;
    DRV_WriteReg32(0x1000f1CC, (temp | 0x04000000)); // 0x1000f1CC[31:16] PLL2 0x400 = 1024 count

    temp = DRV_Reg32(0x1000f1D0) & 0x0000FFFF;
    DRV_WriteReg32(0x1000f1D0, (temp | 0x04000000)); // 0x1000f1D0[31:16] PLL3 0x400 = 1024 count

    temp = DRV_Reg32(0x1000f1D4) & 0x0000FFFF;
    DRV_WriteReg32(0x1000f1D4, (temp | 0x04000000)); // 0x1000f1D4[31:16] PLL4 0x400 = 1024 count

    while(1)
    {
        /***********************************************
        * 3. Adjust delay chain tap number
        ************************************************/

        if (!pll2_done)
        {
            temp = DRV_Reg32(0x1000f610) & ~0xF8000000;
            DRV_WriteReg32(0x1000f610, (temp | (0x10 << 27 ))); // 0x1000f610[31:27] PLL2 REF 0x10 fix

            temp = DRV_Reg32(0x1000f610) & ~0x07C00000;
            DRV_WriteReg32(0x1000f610, (temp | (pll2_dl << 22))); // 0x1000f610[26:22] PLL2 FB inc 1
        }

        if (!pll3_done)
        {
            temp = DRV_Reg32(0x1000f618) & ~0xF8000000;
            DRV_WriteReg32(0x1000f618, (temp | (0x10 << 27 ))); // 0x1000f618[31:27] PLL3 REF 0x10 fix

            temp = DRV_Reg32(0x1000f618) & ~0x07C00000;
            DRV_WriteReg32(0x1000f618, (temp | (pll3_dl << 22))); // 0x1000f618[26:22] PLL3 FB inc 1
        }

        if (!pll4_done)
        {
            temp = DRV_Reg32(0x1000f620) & ~0xF8000000;
            DRV_WriteReg32(0x1000f620, (temp | (0x10 << 27 ))); // 0x1000f620[31:27] PLL4 REF 0x10 fix

            temp = DRV_Reg32(0x1000f620) & ~0x07C00000;
            DRV_WriteReg32(0x1000f620, (temp | (pll4_dl << 22))); // 0x1000f620[26:22] PLL4 FB inc 1
        }

        /***********************************************
        * 4. Enable jitter meter
        ************************************************/

        if (!pll2_done)
        {
            temp = DRV_Reg32(0x1000f1CC);
            DRV_WriteReg32(0x1000f1CC, temp | 0x1); // 0x1000f1CC[0]=1 PLL2
        }

        if (!pll3_done)
        {
            temp = DRV_Reg32(0x1000f1D0);
            DRV_WriteReg32(0x1000f1D0, temp | 0x1); // 0x1000f1D0[0]=1 PLL3
        }

        if (!pll4_done)
        {
            temp = DRV_Reg32(0x1000f1D4);
            DRV_WriteReg32(0x1000f1D4, temp | 0x1); // 0x1000f1D4[0]=1 PLL4
        }

        gpt_busy_wait_us(400); // wait for jitter meter complete

        /***********************************************
        * 5. Check jitter meter counter value
        ************************************************/

        if (!pll2_done)
        {
            one_count = DRV_Reg32(0x1000f320) >> 16; // 0x1000f320[31:16] PLL2 one count
            zero_count = DRV_Reg32(0x1000f320) & 0x0000FFFF; // 0x1000f320[15:0] PLL2 zero count

            if (zero_count > 512)
            {
              //  printf("[mt_pll_init] PLL2 FB_DL: 0x%x, 1/0 = %d/%d\n", ((DRV_Reg32(0x1000f610)& 0x07C00000) >> 22), one_count, zero_count);
                pll2_done = 1;
            }
        }

        if (!pll3_done)
        {
            one_count = DRV_Reg32(0x1000f324) >> 16; // 0x1000f324[31:16] PLL3 one count
            zero_count = DRV_Reg32(0x1000f324) & 0x0000FFFF; // 0x1000f324[15:0] PLL3 zero count

            if (zero_count > 512)
            {
             //   printf("[mt_pll_init] PLL3 FB_DL: 0x%x, 1/0 = %d/%d\n", ((DRV_Reg32(0x1000f618) & 0x07C00000) >> 22), one_count, zero_count);
                pll3_done = 1;
            }
        }

        if (!pll4_done)
        {
            one_count = DRV_Reg32(0x1000f328) >> 16; // 0x1000f328[31:16] PLL4 one count
            zero_count = DRV_Reg32(0x1000f328) & 0x0000FFFF; // 0x1000f328[15:0] PLL4 zero count

            if (zero_count > 512)
            {
            //    printf("[mt_pll_init] PLL4 FB_DL: 0x%x, 1/0 = %d/%d\n", ((DRV_Reg32(0x1000f620) & 0x07C00000) >> 22), one_count, zero_count);
                pll4_done = 1;
            }
        }

        /***********************************************
        * 6. Reset jitter meter value
        ************************************************/

        if (!pll2_done)
        {
            pll2_dl++;
            temp = DRV_Reg32(0x1000f1CC);
            DRV_WriteReg32(0x1000f1CC, temp & ~0x1); // 0x1000f1CC[0]=0 PLL2
        }

        if (!pll3_done)
        {
            pll3_dl++;
            temp = DRV_Reg32(0x1000f1D0);
            DRV_WriteReg32(0x1000f1D0, temp & ~0x1); // 0x1000f1D0[0]=0 PLL3
        }

        if (!pll4_done)
        {
            pll4_dl++;
            temp = DRV_Reg32(0x1000f1D4);
            DRV_WriteReg32(0x1000f1D4, temp & ~0x1); // 0x1000f1D4[0]=0 PLL4
        }

        /*************************************************************
        * Then return to step 1 to adjust next delay chain tap value.
        * Until we have ~ 50% of one or zero count on jitter meter
        **************************************************************/

        if (pll2_done && pll3_done && pll4_done)
        {
            ret = 0;
            break;
        }

        if (pll2_dl >= 32 || pll3_dl >= 32 || pll4_dl >= 32)
        {
            ret = -1;			
            break;
        }
    }

    #if 1
    printf("[mt_pll_init] pll2_dl:%d, pll3_dl:%d, pll4_dl:%d\n", pll2_dl, pll3_dl, pll4_dl);
    
    if (ret != 0)
        printf("[mt_pll_init] MEMPLL calibration fail\n");
    else
        printf("[mt_pll_init] MEMPLL calibration pass\n");
    #endif

    /***********************************************
    * 7. Set jitter meter clock to external FB path
    ************************************************/

    temp = DRV_Reg32(0x1000f60C);
    DRV_WriteReg32(0x1000f60C, temp | 0x200); // 0x1000f60C[9] = 1 PLL2

    temp = DRV_Reg32(0x1000f614);
    DRV_WriteReg32(0x1000f614, temp | 0x200); // 0x1000f614[9] = 1 PLL3

    temp = DRV_Reg32(0x1000f61C);
    DRV_WriteReg32(0x1000f61C, temp | 0x200); // 0x1000f61C[9] = 1 PLL4

    return ret;
}


#define MEMPLL_JMETER_CNT               1024
#define MEMPLL_JMETER_CONFIDENCE_CNT    (MEMPLL_JMETER_CNT/10)
#define MEMPLL_JMETER_WAIT_TIME_BASE    10 // time base
#define MEMPLL_JMETER_WAIT_TIMEOUT      1000/MEMPLL_JMETER_WAIT_TIME_BASE // timeout ~ 1000us

int mt_mempll_recover_cali(void)
{
    int one_count = 0, zero_count = 0;
    int pll2_done = 0, pll3_done = 0, pll4_done = 0, ret = 0;

    unsigned int temp = 0, pll2_dl = 0, pll3_dl = 0, pll4_dl = 0;
    int pll2_phase=0, pll3_phase=0, pll4_phase=0;
    unsigned int jmeter_wait_count;

    /***********************************************
    * 1. Set jitter meter clock to internal FB path
    ************************************************/
    //temp = DRV_Reg32(0x1000f60C);
    //DRV_WriteReg32(0x1000f60C, temp & ~0x200); // 0x1000f60C[9] = 0 PLL2

    //temp = DRV_Reg32(0x1000f614);
    //DRV_WriteReg32(0x1000f614, temp & ~0x200); // 0x1000f614[9] = 0 PLL3

    //temp = DRV_Reg32(0x1000f61C);
    //DRV_WriteReg32(0x1000f61C, temp & ~0x200); // 0x1000f61C[9] = 0 PLL4

    /***********************************************
    * 2. Set jitter meter count number
    ************************************************/

    temp = DRV_Reg32(0x1000f1CC) & 0x0000FFFF;
    DRV_WriteReg32(0x1000f1CC, (temp | (MEMPLL_JMETER_CNT<<16))); // 0x1000f1CC[31:16] PLL2 0x400 = 1024 count

    temp = DRV_Reg32(0x1000f1D0) & 0x0000FFFF;
    DRV_WriteReg32(0x1000f1D0, (temp | (MEMPLL_JMETER_CNT<<16))); // 0x1000f1D0[31:16] PLL3 0x400 = 1024 count

    temp = DRV_Reg32(0x1000f1D4) & 0x0000FFFF;
    DRV_WriteReg32(0x1000f1D4, (temp | (MEMPLL_JMETER_CNT<<16))); // 0x1000f1D4[31:16] PLL4 0x400 = 1024 count

    while(1)
    {
        /***********************************************
        * 3. Adjust delay chain tap number
        ************************************************/

        if (!pll2_done)
        {
            if (pll2_phase == 0)    // initial phase set to 0 for REF and FBK
            {
                temp = DRV_Reg32(0x1000f610) & ~0xF8000000;
                DRV_WriteReg32(0x1000f610, (temp | (0x00 << 27 )));

                temp = DRV_Reg32(0x1000f610) & ~0x07C00000;
                DRV_WriteReg32(0x1000f610, (temp | (0x00 << 22)));
            }
            else if (pll2_phase == 1)   // REF lag FBK, delay FBK
            {
                temp = DRV_Reg32(0x1000f610) & ~0xF8000000;
                DRV_WriteReg32(0x1000f610, (temp | (0x00 << 27 )));

                temp = DRV_Reg32(0x1000f610) & ~0x07C00000;
                DRV_WriteReg32(0x1000f610, (temp | (pll2_dl << 22)));
            }
            else   // REF lead FBK, delay REF
            {
                temp = DRV_Reg32(0x1000f610) & ~0xF8000000;
                DRV_WriteReg32(0x1000f610, (temp | (pll2_dl << 27 )));

                temp = DRV_Reg32(0x1000f610) & ~0x07C00000;
                DRV_WriteReg32(0x1000f610, (temp | (0x00 << 22)));
            }
        }

        if (!pll3_done)
        {
            if (pll3_phase == 0)    // initial phase set to 0 for REF and FBK
            {
                temp = DRV_Reg32(0x1000f618) & ~0xF8000000;
                DRV_WriteReg32(0x1000f618, (temp | (0x00 << 27 )));

                temp = DRV_Reg32(0x1000f618) & ~0x07C00000;
                DRV_WriteReg32(0x1000f618, (temp | (0x00 << 22)));
            }
            else if (pll3_phase == 1)   // REF lag FBK, delay FBK
            {
                temp = DRV_Reg32(0x1000f618) & ~0xF8000000;
                DRV_WriteReg32(0x1000f618, (temp | (0x00 << 27 )));

                temp = DRV_Reg32(0x1000f618) & ~0x07C00000;
                DRV_WriteReg32(0x1000f618, (temp | (pll3_dl << 22)));
            }
            else   // REF lead FBK, delay REF
            {
                temp = DRV_Reg32(0x1000f618) & ~0xF8000000;
                DRV_WriteReg32(0x1000f618, (temp | (pll3_dl << 27 )));

                temp = DRV_Reg32(0x1000f618) & ~0x07C00000;
                DRV_WriteReg32(0x1000f618, (temp | (0x00 << 22)));
            }
        }

        if (!pll4_done)
        {
            if (pll4_phase == 0)    // initial phase set to 0 for REF and FBK
            {
                temp = DRV_Reg32(0x1000f620) & ~0xF8000000;
                DRV_WriteReg32(0x1000f620, (temp | (0x00 << 27 )));

                temp = DRV_Reg32(0x1000f620) & ~0x07C00000;
                DRV_WriteReg32(0x1000f620, (temp | (0x00 << 22)));
            }
            else if (pll4_phase == 1)   // REF lag FBK, delay FBK
            {
                temp = DRV_Reg32(0x1000f620) & ~0xF8000000;
                DRV_WriteReg32(0x1000f620, (temp | (0x00 << 27 )));

            temp = DRV_Reg32(0x1000f620) & ~0x07C00000;
            DRV_WriteReg32(0x1000f620, (temp | (pll4_dl << 22)));
            }
            else   // REF lead FBK, delay REF
            {
                temp = DRV_Reg32(0x1000f620) & ~0xF8000000;
                DRV_WriteReg32(0x1000f620, (temp | (pll4_dl << 27 )));

                temp = DRV_Reg32(0x1000f620) & ~0x07C00000;
                DRV_WriteReg32(0x1000f620, (temp | (0x00 << 22)));
            }
        }

        gpt_busy_wait_us(20); // wait for external loop ready

        /***********************************************
        * 4. Enable jitter meter
        ************************************************/

        if (!pll2_done)
        {
            temp = DRV_Reg32(0x1000f1CC);
            DRV_WriteReg32(0x1000f1CC, temp | 0x1); // 0x1000f1CC[0]=1 PLL2
        }

        if (!pll3_done)
        {
            temp = DRV_Reg32(0x1000f1D0);
            DRV_WriteReg32(0x1000f1D0, temp | 0x1); // 0x1000f1D0[0]=1 PLL3
        }

        if (!pll4_done)
        {
            temp = DRV_Reg32(0x1000f1D4);
            DRV_WriteReg32(0x1000f1D4, temp | 0x1); // 0x1000f1D4[0]=1 PLL4
        }

        //gpt_busy_wait_us(40); // wait for jitter meter complete

        // 1001132C[2:0] : PLL4 PLL3 PLL2

        jmeter_wait_count = 0;
        if (!pll2_done)
        {
            while (!(DRV_Reg32(0x1000f32C)&0x00000001))
            {
                gpt_busy_wait_us(MEMPLL_JMETER_WAIT_TIME_BASE);
                jmeter_wait_count++;
                if (jmeter_wait_count>MEMPLL_JMETER_WAIT_TIMEOUT)
                {
                  //  mcSHOW_ERR_MSG(("[ERROR] PLL2 Jeter Meter Count Timeout > %d us!!\n", MEMPLL_JMETER_WAIT_TIME_BASE*MEMPLL_JMETER_WAIT_TIMEOUT));
                    break;
                }
            }
        }

        jmeter_wait_count = 0;
        if (!pll3_done)
        {
            while (!(DRV_Reg32(0x1000f32C)&0x00000002))
            {
                gpt_busy_wait_us(MEMPLL_JMETER_WAIT_TIME_BASE);
                jmeter_wait_count++;
                if (jmeter_wait_count>MEMPLL_JMETER_WAIT_TIMEOUT)
                {
                 //   mcSHOW_ERR_MSG(("[ERROR] PLL3 Jeter Meter Count Timeout > %d us!!\n", MEMPLL_JMETER_WAIT_TIME_BASE*MEMPLL_JMETER_WAIT_TIMEOUT));
                    break;
                }
            }
        }

        jmeter_wait_count = 0;
        if (!pll4_done)
        {
            while (!(DRV_Reg32(0x1000f32C)&0x00000004))
            {
                gpt_busy_wait_us(MEMPLL_JMETER_WAIT_TIME_BASE);
                jmeter_wait_count++;
                if (jmeter_wait_count>MEMPLL_JMETER_WAIT_TIMEOUT)
                {
                 //   mcSHOW_ERR_MSG(("[ERROR] PLL4 Jeter Meter Count Timeout > %d us!!\n", MEMPLL_JMETER_WAIT_TIME_BASE*MEMPLL_JMETER_WAIT_TIMEOUT));
                    break;
                }
            }
        }

        /***********************************************
        * 5. Check jitter meter counter value
        ************************************************/

        if (!pll2_done)
        {
            one_count = DRV_Reg32(0x1000f320) >> 16; // 0x1000f320[31:16] PLL2 one count
            zero_count = DRV_Reg32(0x1000f320) & 0x0000FFFF; // 0x1000f320[15:0] PLL2 zero count

            if (pll2_phase == 0)
            {
                if (one_count > (zero_count+MEMPLL_JMETER_CONFIDENCE_CNT))
                {
                    // REF lag FBK
                    pll2_phase = 1;
                    pll2_dl++;

                   // mcSHOW_DBG_MSG(("[PLL_Phase_Calib] PLL2 initial phase: REF lag FBK, one_cnt/zero_cnt = %d/%d\n", one_count, zero_count));
                }
                else if (zero_count > (one_count+MEMPLL_JMETER_CONFIDENCE_CNT))
                {
                    // REF lead FBK
                    pll2_phase = 2;
                    pll2_dl++;

                 //   mcSHOW_DBG_MSG(("[PLL_Phase_Calib] PLL2 initial phase: REF lead FBK, one_cnt/zero_cnt = %d/%d\n", one_count, zero_count));
                }
                else
                {
                    // in phase at initial
                    pll2_done = 1;

                    //mcSHOW_DBG_MSG(("[PLL_Phase_Calib] PLL2 initial phase: REF in-phase FBK, one_cnt/zero_cnt = %d/%d\n", one_count, zero_count));
                }
            }
            else if (pll2_phase == 1)
            {
                if ((zero_count+MEMPLL_JMETER_CONFIDENCE_CNT) >= one_count)
                {
                    pll2_done = 1;
                   // mcSHOW_DBG_MSG(("[PLL_Phase_Calib] PLL2 REF_DL: 0x0, FBK_DL: 0x%x, one_cnt/zero_cnt = %d/%d\n", pll2_dl, one_count, zero_count));
                }
                else
                {
                    pll2_dl++;
                }
            }
            else
            {
                if ((one_count+MEMPLL_JMETER_CONFIDENCE_CNT) >= zero_count)
                {
                    pll2_done = 1;
                  //  mcSHOW_DBG_MSG(("[PLL_Phase_Calib] PLL2 REF_DL: 0x%x, FBK_DL: 0x0, one_cnt/zero_cnt = %d/%d\n", pll2_dl, one_count, zero_count));
                }
                else
                {
                    pll2_dl++;
                }
            }
        }

        if (!pll3_done)
        {
            one_count = DRV_Reg32(0x1000f324) >> 16; // 0x1000f324[31:16] PLL3 one count
            zero_count = DRV_Reg32(0x1000f324) & 0x0000FFFF; // 0x1000f324[15:0] PLL3 zero count

            if (pll3_phase == 0)
            {
                if (one_count > (zero_count+MEMPLL_JMETER_CONFIDENCE_CNT))
                {
                    // REF lag FBK
                    pll3_phase = 1;
                    pll3_dl++;

                  //  mcSHOW_DBG_MSG(("[PLL_Phase_Calib] PLL3 initial phase: REF lag FBK, one_cnt/zero_cnt = %d/%d\n", one_count, zero_count));
                }
                else if (zero_count > (one_count+MEMPLL_JMETER_CONFIDENCE_CNT))
                {
                    // REF lead FBK
                    pll3_phase = 2;
                    pll3_dl++;

                  //  mcSHOW_DBG_MSG(("[PLL_Phase_Calib] PLL3 initial phase: REF lead FBK, one_cnt/zero_cnt = %d/%d\n", one_count, zero_count));
                }
                else
                {
                    // in phase at initial
                    pll3_done = 1;

                   // mcSHOW_DBG_MSG(("[PLL_Phase_Calib] PLL3 initial phase: REF in-phase FBK, one_cnt/zero_cnt = %d/%d\n", one_count, zero_count));
                }
            }
            else if (pll3_phase == 1)
            {
                if ((zero_count+MEMPLL_JMETER_CONFIDENCE_CNT) >= one_count)
                {
                    pll3_done = 1;
                  //  mcSHOW_DBG_MSG(("[PLL_Phase_Calib] PLL3 REF_DL: 0x0, FBK_DL: 0x%x, one_cnt/zero_cnt = %d/%d\n", pll3_dl, one_count, zero_count));
                }
                else
                {
                    pll3_dl++;
                }
            }
            else
            {
                if ((one_count+MEMPLL_JMETER_CONFIDENCE_CNT) >= zero_count)
                {
                    pll3_done = 1;
                  //  mcSHOW_DBG_MSG(("[PLL_Phase_Calib] PLL3 REF_DL: 0x%x, FBK_DL: 0x0, one_cnt/zero_cnt = %d/%d\n", pll3_dl, one_count, zero_count));
                }
                else
                {
                    pll3_dl++;
                }
            }
        }

        if (!pll4_done)
        {
            one_count = DRV_Reg32(0x1000f328) >> 16; // 0x1000f328[31:16] PLL4 one count
            zero_count = DRV_Reg32(0x1000f328) & 0x0000FFFF; // 0x1000f328[15:0] PLL4 zero count

            if (pll4_phase == 0)
            {
                if (one_count > (zero_count+MEMPLL_JMETER_CONFIDENCE_CNT))
                {
                    // REF lag FBK
                    pll4_phase = 1;
                    pll4_dl++;

                  //  mcSHOW_DBG_MSG(("[PLL_Phase_Calib] PLL4 initial phase: REF lag FBK, one_cnt/zero_cnt = %d/%d\n", one_count, zero_count));
                }
                else if (zero_count > (one_count+MEMPLL_JMETER_CONFIDENCE_CNT))
                {
                    // REF lead FBK
                    pll4_phase = 2;
                    pll4_dl++;

                  //  mcSHOW_DBG_MSG(("[PLL_Phase_Calib] PLL4 initial phase: REF lead FBK, one_cnt/zero_cnt = %d/%d\n", one_count, zero_count));
                }
                else
                {
                    // in phase at initial
                    pll4_done = 1;

                  //  mcSHOW_DBG_MSG(("[PLL_Phase_Calib] PLL4 initial phase: REF in-phase FBK, one_cnt/zero_cnt = %d/%d\n", one_count, zero_count));
                }
            }
            else if (pll4_phase == 1)
            {
                if ((zero_count+MEMPLL_JMETER_CONFIDENCE_CNT) >= one_count)
                {
                    pll4_done = 1;
                  //  mcSHOW_DBG_MSG(("[PLL_Phase_Calib] PLL4 REF_DL: 0x0, FBK_DL: 0x%x, one_cnt/zero_cnt = %d/%d\n", pll4_dl, one_count, zero_count));
                }
                else
                {
                    pll4_dl++;
                }
            }
            else
            {
                if ((one_count+MEMPLL_JMETER_CONFIDENCE_CNT) >= zero_count)
                {
                    pll4_done = 1;
                  //  mcSHOW_DBG_MSG(("[PLL_Phase_Calib] PLL4 REF_DL: 0x%x, FBK_DL: 0x0, one_cnt/zero_cnt = %d/%d\n", pll4_dl, one_count, zero_count));
                }
                else
                {
                    pll4_dl++;
                }
            }
        }

        /***********************************************
        * 6. Reset jitter meter value
        ************************************************/

        if (!pll2_done)
        {
            pll2_dl++;
            temp = DRV_Reg32(0x1000f1CC);
            DRV_WriteReg32(0x1000f1CC, temp & ~0x1); // 0x1000f1CC[0]=0 PLL2
        }

        if (!pll3_done)
        {
            pll3_dl++;
            temp = DRV_Reg32(0x1000f1D0);
            DRV_WriteReg32(0x1000f1D0, temp & ~0x1); // 0x1000f1D0[0]=0 PLL3
        }

        if (!pll4_done)
        {
            pll4_dl++;
            temp = DRV_Reg32(0x1000f1D4);
            DRV_WriteReg32(0x1000f1D4, temp & ~0x1); // 0x1000f1D4[0]=0 PLL4
        }

        /*************************************************************
        * Then return to step 1 to adjust next delay chain tap value.
        * Until we have ~ 50% of one or zero count on jitter meter
        **************************************************************/

        if (pll2_done && pll3_done && pll4_done)
        {
            ret = 0;
            break;
        }

        if (pll2_dl >= 32 || pll3_dl >= 32 || pll4_dl >= 32)
        {
            ret = -1;
            break;
        }
    }

#if 0 // for FT, enable it...
    if (ret != 0)
    {
        //print("[mt_pll_init] MEMPLL 3PLL mode calibration fail\n");
        while(1); // TBD
    }
#endif

    /***********************************************
    * 7. Set jitter meter clock to external FB path
    ************************************************/

    temp = DRV_Reg32(0x1000f60C);
    DRV_WriteReg32(0x1000f60C, temp | 0x200); // 0x1000f60C[9] = 1 PLL2

    temp = DRV_Reg32(0x1000f614);
    DRV_WriteReg32(0x1000f614, temp | 0x200); // 0x1000f614[9] = 1 PLL3

    temp = DRV_Reg32(0x1000f61C);
    DRV_WriteReg32(0x1000f61C, temp | 0x200); // 0x1000f61C[9] = 1 PLL4

    return ret;
}


#if 1
unsigned int mt_get_cpu_freq(void)
{
	int output = 0, i = 0;
    unsigned int temp, clk26cali_0, clk_dbg_cfg, clk_misc_cfg_0, clk26cali_1;

    clk_dbg_cfg = DRV_Reg32(CLK_DBG_CFG);
    DRV_WriteReg32(CLK_DBG_CFG, 2<<16); //sel abist_cksw and enable freq meter sel abist

    clk_misc_cfg_0 = DRV_Reg32(CLK_MISC_CFG_0);
    DRV_WriteReg32(CLK_MISC_CFG_0, (clk_misc_cfg_0 & 0x0000FFFF) | (0x07 << 16)); // select divider

    clk26cali_1 = DRV_Reg32(CLK26CALI_1);
    DRV_WriteReg32(CLK26CALI_1, 0x00ff0000); //

    //temp = DRV_Reg32(CLK26CALI_0);
    DRV_WriteReg32(CLK26CALI_0, 0x1000);
    DRV_WriteReg32(CLK26CALI_0, 0x1010);

    /* wait frequency meter finish */
    while (DRV_Reg32(CLK26CALI_0) & 0x10)
    {
        mdelay(10);
        i++;
        if(i > 10)
        	break;
    }

    temp = DRV_Reg32(CLK26CALI_1) & 0xFFFF;

    output = (((temp * 26000) ) / 256)*8; // Khz

    DRV_WriteReg32(CLK_DBG_CFG, clk_dbg_cfg);
    DRV_WriteReg32(CLK_MISC_CFG_0, clk_misc_cfg_0);
    DRV_WriteReg32(CLK26CALI_0, clk26cali_0);
    DRV_WriteReg32(CLK26CALI_1, clk26cali_1);

    //print("freq = %d\n", output);

    if(i>10)
        return 0;
    else
        return output;
}


unsigned int mt_get_mem_freq(void)
{
    int output = 0;
    unsigned int temp, clk26cali_0, clk_dbg_cfg, clk_misc_cfg_0, clk26cali_1;

    clk_dbg_cfg = DRV_Reg32(CLK_DBG_CFG);
    DRV_WriteReg32(CLK_DBG_CFG, 0x1901); //sel ckgen_cksw and enable freq meter sel ckgen

    clk_misc_cfg_0 = DRV_Reg32(CLK_MISC_CFG_0);
    DRV_WriteReg32(CLK_MISC_CFG_0, (clk_misc_cfg_0 & 0x00FFFFFF) | (0x07 << 24)); // select divider

    clk26cali_1 = DRV_Reg32(CLK26CALI_1);
    DRV_WriteReg32(CLK26CALI_1, 0x00ff0000); //

    //temp = DRV_Reg32(CLK26CALI_0);
    DRV_WriteReg32(CLK26CALI_0, 0x1000);
    DRV_WriteReg32(CLK26CALI_0, 0x1010);

    /* wait frequency meter finish */
    while (DRV_Reg32(CLK26CALI_0) & 0x10)
    {
        print("wait for frequency meter finish, CLK26CALI = 0x%x\n", DRV_Reg32(CLK26CALI_0));
        //mdelay(10);
    }

    temp = DRV_Reg32(CLK26CALI_1) & 0xFFFF;

    output = (((temp * 26000) ) / 256)*8; // Khz

    DRV_WriteReg32(CLK_DBG_CFG, clk_dbg_cfg);
    DRV_WriteReg32(CLK_MISC_CFG_0, clk_misc_cfg_0);
    DRV_WriteReg32(CLK26CALI_0, clk26cali_0);
    DRV_WriteReg32(CLK26CALI_1, clk26cali_1);

    //print("CLK26CALI = 0x%x, bus frequency = %d Khz\n", temp, output);

    return output;
}

unsigned int mt_get_bus_freq(void)
{
    int output = 0;
    unsigned int temp, clk26cali_0, clk_dbg_cfg, clk_misc_cfg_0, clk26cali_1;

    clk_dbg_cfg = DRV_Reg32(CLK_DBG_CFG);
    DRV_WriteReg32(CLK_DBG_CFG, 0x101); //sel ckgen_cksw and enable freq meter sel ckgen

    clk_misc_cfg_0 = DRV_Reg32(CLK_MISC_CFG_0);
    DRV_WriteReg32(CLK_MISC_CFG_0, (clk_misc_cfg_0 & 0x00FFFFFF) | (0x07 << 24)); // select divider

    clk26cali_1 = DRV_Reg32(CLK26CALI_1);
    DRV_WriteReg32(CLK26CALI_1, 0x00ff0000); //

    //temp = DRV_Reg32(CLK26CALI_0);
    DRV_WriteReg32(CLK26CALI_0, 0x1000);
    DRV_WriteReg32(CLK26CALI_0, 0x1010);

    /* wait frequency meter finish */
    while (DRV_Reg32(CLK26CALI_0) & 0x10)
    {
        print("wait for frequency meter finish, CLK26CALI = 0x%x\n", DRV_Reg32(CLK26CALI_0));
        //mdelay(10);
    }

    temp = DRV_Reg32(CLK26CALI_1) & 0xFFFF;

    output = (((temp * 26000) ) / 256)*8; // Khz

    DRV_WriteReg32(CLK_DBG_CFG, clk_dbg_cfg);
    DRV_WriteReg32(CLK_MISC_CFG_0, clk_misc_cfg_0);
    DRV_WriteReg32(CLK26CALI_0, clk26cali_0);
    DRV_WriteReg32(CLK26CALI_1, clk26cali_1);

    //print("CLK26CALI = 0x%x, bus frequency = %d Khz\n", temp, output);

    return output;
}
#endif

#define FMETER_EN	0

#if FMETER_EN

#define PLL_HP_EN	0x1000CF00

#ifndef GENMASK
#define GENMASK(h, l)	(((1U << ((h) - (l) + 1)) - 1) << (l))
#endif

#define ALT_BITS(o, h, l, v) \
	(((o) & ~GENMASK(h, l)) | (((v) << (l)) & GENMASK(h, l)))

#define BIT(_bit_)		(u32)(1U << (_bit_))
#define BITMASK(_bits_)		GENMASK(1? _bits_, 0? _bits_)
#define BITS(_bits_, _val_)	(BITMASK(_bits_) & (_val_ << (0? _bits_)))

#define clk_readl(addr)				DRV_Reg32(addr)
#define clk_writel(addr, val)			DRV_WriteReg32(addr, val)
#define clk_setl(addr, val)			clk_writel(addr, clk_readl(addr) | (val))
#define clk_clrl(addr, val)			clk_writel(addr, clk_readl(addr) & ~(val))
#define clk_writel_mask(addr, h, l, v)	\
	clk_writel(addr, (clk_readl(addr) & ~GENMASK(h, l)) | ((v) << (l)));

#define ARRAY_SIZE(a)	(sizeof(a) / sizeof(a[0]))
#define ABS_DIFF(a, b)	((a) > (b)? (a) - (b) : (b) - (a))

#define TRACE(fmt, args...) \
	print("%s():%d: " fmt, __func__, __LINE__, ##args)

#define udelay(us)				\
	do {					\
		volatile int count = us * 1000;	\
		while (count--);		\
	} while(0)

typedef unsigned int size_t;

typedef enum FMETER_TYPE {
	ABIST,
	CKGEN
} FMETER_TYPE;

typedef enum ABIST_CLK
{
	ABIST_CLK_NULL,

	AD_ARMCA7PLL_754M_CORE_CK	=  2,
	AD_OSC_CK			=  3,
	AD_MAIN_H546M_CK		=  4,
	AD_MAIN_H364M_CK		=  5,
	AD_MAIN_H218P4M_CK		=  6,
	AD_MAIN_H156M_CK		=  7,
	AD_UNIV_178P3M_CK		=  8,
	AD_UNIV_48M_CK			=  9,
	AD_UNIV_624M_CK			= 10,
	AD_UNIV_416M_CK			= 11,
	AD_UNIV_249P6M_CK		= 12,
	AD_APLL1_180P6336M_CK		= 13,
	AD_APLL2_196P608M_CK		= 14,
	AD_LTEPLL_FS26M_CK		= 15,
	RTC32K_CK_I			= 16,
	AD_MMPLL_700M_CK		= 17,
	AD_VENCPLL_410M_CK		= 18,
	AD_TVDPLL_594M_CK		= 19,
	AD_MPLL_208M_CK			= 20,
	AD_MSDCPLL_806M_CK		= 21,
	AD_USB_48M_CK			= 22,
	MIPI_DSI_TST_CK			= 24,
	AD_PLLGP_TST_CK			= 25,
	AD_LVDSTX_MONCLK		= 26,
	AD_DSI0_LNTC_DSICLK		= 27,
	AD_DSI0_MPPLL_TST_CK		= 28,
	AD_CSI0_DELAY_TSTCLK		= 29,
	AD_CSI1_DELAY_TSTCLK		= 30,
	AD_HDMITX_MON_CK		= 31,
	AD_ARMPLL_1508M_CK		= 32,
	AD_MAINPLL_1092M_CK		= 33,
	AD_MAINPLL_PRO_CK		= 34,
	AD_UNIVPLL_1248M_CK		= 35,
	AD_LVDSPLL_150M_CK		= 36,
	AD_LVDSPLL_ETH_CK		= 37,
	AD_SSUSB_48M_CK			= 38,
	AD_MIPI_26M_CK			= 39,
	AD_MEM_26M_CK			= 40,
	AD_MEMPLL_MONCLK		= 41,
	AD_MEMPLL2_MONCLK		= 42,
	AD_MEMPLL3_MONCLK		= 43,
	AD_MEMPLL4_MONCLK		= 44,
	AD_MEMPLL_REFCLK_BUF		= 45,
	AD_MEMPLL_FBCLK_BUF		= 46,
	AD_MEMPLL2_REFCLK_BUF		= 47,
	AD_MEMPLL2_FBCLK_BUF		= 48,
	AD_MEMPLL3_REFCLK_BUF		= 49,
	AD_MEMPLL3_FBCLK_BUF		= 50,
	AD_MEMPLL4_REFCLK_BUF		= 51,
	AD_MEMPLL4_FBCLK_BUF		= 52,
	AD_USB20_MONCLK			= 53,
	AD_USB20_MONCLK_1P		= 54,
	AD_MONREF_CK			= 55,
	AD_MONFBK_CK			= 56,

	ABIST_CLK_END,
} ABIST_CLK;

const char* ABIST_CLK_NAME[] =
{
	[AD_ARMCA7PLL_754M_CORE_CK]	= "AD_ARMCA7PLL_754M_CORE_CK",
	[AD_OSC_CK]			= "AD_OSC_CK",
	[AD_MAIN_H546M_CK]		= "AD_MAIN_H546M_CK",
	[AD_MAIN_H364M_CK]		= "AD_MAIN_H364M_CK",
	[AD_MAIN_H218P4M_CK]		= "AD_MAIN_H218P4M_CK",
	[AD_MAIN_H156M_CK]		= "AD_MAIN_H156M_CK",
	[AD_UNIV_178P3M_CK]		= "AD_UNIV_178P3M_CK",
	[AD_UNIV_48M_CK]		= "AD_UNIV_48M_CK",
	[AD_UNIV_624M_CK]		= "AD_UNIV_624M_CK",
	[AD_UNIV_416M_CK]		= "AD_UNIV_416M_CK",
	[AD_UNIV_249P6M_CK]		= "AD_UNIV_249P6M_CK",
	[AD_APLL1_180P6336M_CK]		= "AD_APLL1_180P6336M_CK",
	[AD_APLL2_196P608M_CK]		= "AD_APLL2_196P608M_CK",
	[AD_LTEPLL_FS26M_CK]		= "AD_LTEPLL_FS26M_CK",
	[RTC32K_CK_I]			= "rtc32k_ck_i",
	[AD_MMPLL_700M_CK]		= "AD_MMPLL_700M_CK",
	[AD_VENCPLL_410M_CK]		= "AD_VENCPLL_410M_CK",
	[AD_TVDPLL_594M_CK]		= "AD_TVDPLL_594M_CK",
	[AD_MPLL_208M_CK]		= "AD_MPLL_208M_CK",
	[AD_MSDCPLL_806M_CK]		= "AD_MSDCPLL_806M_CK",
	[AD_USB_48M_CK]			= "AD_USB_48M_CK",
	[MIPI_DSI_TST_CK]		= "MIPI_DSI_TST_CK",
	[AD_PLLGP_TST_CK]		= "AD_PLLGP_TST_CK",
	[AD_LVDSTX_MONCLK]		= "AD_LVDSTX_MONCLK",
	[AD_DSI0_LNTC_DSICLK]		= "AD_DSI0_LNTC_DSICLK",
	[AD_DSI0_MPPLL_TST_CK]		= "AD_DSI0_MPPLL_TST_CK",
	[AD_CSI0_DELAY_TSTCLK]		= "AD_CSI0_DELAY_TSTCLK",
	[AD_CSI1_DELAY_TSTCLK]		= "AD_CSI1_DELAY_TSTCLK",
	[AD_HDMITX_MON_CK]		= "AD_HDMITX_MON_CK",
	[AD_ARMPLL_1508M_CK]		= "AD_ARMPLL_1508M_CK",
	[AD_MAINPLL_1092M_CK]		= "AD_MAINPLL_1092M_CK",
	[AD_MAINPLL_PRO_CK]		= "AD_MAINPLL_PRO_CK",
	[AD_UNIVPLL_1248M_CK]		= "AD_UNIVPLL_1248M_CK",
	[AD_LVDSPLL_150M_CK]		= "AD_LVDSPLL_150M_CK",
	[AD_LVDSPLL_ETH_CK]		= "AD_LVDSPLL_ETH_CK",
	[AD_SSUSB_48M_CK]		= "AD_SSUSB_48M_CK",
	[AD_MIPI_26M_CK]		= "AD_MIPI_26M_CK",
	[AD_MEM_26M_CK]			= "AD_MEM_26M_CK",
	[AD_MEMPLL_MONCLK]		= "AD_MEMPLL_MONCLK",
	[AD_MEMPLL2_MONCLK]		= "AD_MEMPLL2_MONCLK",
	[AD_MEMPLL3_MONCLK]		= "AD_MEMPLL3_MONCLK",
	[AD_MEMPLL4_MONCLK]		= "AD_MEMPLL4_MONCLK",
	[AD_MEMPLL_REFCLK_BUF]		= "AD_MEMPLL_REFCLK_BUF",
	[AD_MEMPLL_FBCLK_BUF]		= "AD_MEMPLL_FBCLK_BUF",
	[AD_MEMPLL2_REFCLK_BUF]		= "AD_MEMPLL2_REFCLK_BUF",
	[AD_MEMPLL2_FBCLK_BUF]		= "AD_MEMPLL2_FBCLK_BUF",
	[AD_MEMPLL3_REFCLK_BUF]		= "AD_MEMPLL3_REFCLK_BUF",
	[AD_MEMPLL3_FBCLK_BUF]		= "AD_MEMPLL3_FBCLK_BUF",
	[AD_MEMPLL4_REFCLK_BUF]		= "AD_MEMPLL4_REFCLK_BUF",
	[AD_MEMPLL4_FBCLK_BUF]		= "AD_MEMPLL4_FBCLK_BUF",
	[AD_USB20_MONCLK]		= "AD_USB20_MONCLK",
	[AD_USB20_MONCLK_1P]		= "AD_USB20_MONCLK_1P",
	[AD_MONREF_CK]			= "AD_MONREF_CK",
	[AD_MONFBK_CK]			= "AD_MONFBK_CK",
};

typedef enum CKGEN_CLK {
	CKGEN_CLK_NULL,

	HD_FAXI_CK		=  1,
	HF_FDDRPHYCFG_CK	=  2,
	F_FPWM_CK		=  3,
	HF_FVDEC_CK		=  4,
	HF_FMM_CK		=  5,
	HF_FCAMTG_CK		=  6,
	F_FUART_CK		=  7,
	HF_FSPI_CK		=  8,
	HF_FMSDC_30_0_CK	=  9,
	HF_FMSDC_30_1_CK	= 10,
	HF_FMSDC_30_2_CK	= 11,
	HF_FMSDC_50_3_CK	= 12,
	HF_FMSDC_50_3_HCLK_CK	= 13,
	HF_FAUDIO_CK		= 14,
	HF_FAUD_INTBUS_CK	= 15,
	HF_FPMICSPI_CK		= 16,
	HF_FSCP_CK		= 17,
	HF_FATB_CK		= 18,
	HF_FSNFI_CK		= 19,
	HF_FDPI0_CK		= 20,
	HF_FAUD_1_CK_PRE	= 21,
	HF_FAUD_2_CK_PRE	= 22,
	HF_FSCAM_CK		= 23,
	HF_FMFG_CK		= 24,
	ECO_AO12_MEM_CLKMUX_CK	= 25,
	ECO_A012_MEM_DCM_CK	= 26,
	HF_FDPI1_CK		= 27,
	HF_FUFOENC_CK		= 28,
	HF_FUFODEC_CK		= 29,
	HF_FETH_CK		= 30,
	HF_FONFI_CK		= 31,

	CKGEN_CLK_END,
} CKGEN_CLK;

const char *CKGEN_CLK_NAME[] = {
	[HD_FAXI_CK]			= "hd_faxi_ck",
	[HF_FDDRPHYCFG_CK]		= "hf_fddrphycfg_ck",
	[F_FPWM_CK]			= "f_fpwm_ck",
	[HF_FVDEC_CK]			= "hf_fvdec_ck",
	[HF_FMM_CK]			= "hf_fmm_ck",
	[HF_FCAMTG_CK]			= "hf_fcamtg_ck",
	[F_FUART_CK]			= "f_fuart_ck",
	[HF_FSPI_CK]			= "hf_fspi_ck",
	[HF_FMSDC_30_0_CK]		= "hf_fmsdc_30_0_ck",
	[HF_FMSDC_30_1_CK]		= "hf_fmsdc_30_1_ck",
	[HF_FMSDC_30_2_CK]		= "hf_fmsdc_30_2_ck",
	[HF_FMSDC_50_3_CK]		= "hf_fmsdc_50_3_ck",
	[HF_FMSDC_50_3_HCLK_CK]		= "hf_fmsdc_50_3_hclk_ck",
	[HF_FAUDIO_CK]			= "hf_faudio_ck",
	[HF_FAUD_INTBUS_CK]		= "hf_faud_intbus_ck",
	[HF_FPMICSPI_CK]		= "hf_fpmicspi_ck",
	[HF_FSCP_CK]			= "hf_fscp_ck",
	[HF_FATB_CK]			= "hf_fatb_ck",
	[HF_FSNFI_CK]			= "hf_fsnfi_ck",
	[HF_FDPI0_CK]			= "hf_fdpi0_ck",
	[HF_FAUD_1_CK_PRE]		= "hf_faud_1_ck_pre",
	[HF_FAUD_2_CK_PRE]		= "hf_faud_2_ck_pre",
	[HF_FSCAM_CK]			= "hf_fscam_ck",
	[HF_FMFG_CK]			= "hf_fmfg_ck",
	[ECO_AO12_MEM_CLKMUX_CK]	= "eco_ao12_mem_clkmux_ck",
	[ECO_A012_MEM_DCM_CK]		= "eco_a012_mem_dcm_ck",
	[HF_FDPI1_CK]			= "hf_fdpi1_ck",
	[HF_FUFOENC_CK]			= "hf_fufoenc_ck",
	[HF_FUFODEC_CK]			= "hf_fufodec_ck",
	[HF_FETH_CK]			= "hf_feth_ck",
	[HF_FONFI_CK]			= "hf_fonfi_ck",
};

static void set_fmeter_divider_ca7(u32 k1)
{
	u32 v = clk_readl(CLK_MISC_CFG_0);

	v = ALT_BITS(v, 23, 16, k1);
	clk_writel(CLK_MISC_CFG_0, v);
}

static void set_fmeter_divider(u32 k1)
{
	u32 v = clk_readl(CLK_MISC_CFG_0);

	v = ALT_BITS(v, 31, 24, k1);
	clk_writel(CLK_MISC_CFG_0, v);
}

static BOOL wait_fmeter_done(u32 tri_bit)
{
	static int max_wait_count = 0;
	int wait_count = (max_wait_count > 0)? (max_wait_count * 2 + 2) : 100;
	int i;

	/* wait fmeter */
	for (i = 0; i < wait_count && (clk_readl(CLK26CALI_0) & tri_bit); i++)
		udelay(20);

	if (!(clk_readl(CLK26CALI_0) & tri_bit)) {
		max_wait_count = max(max_wait_count, i);
		return TRUE;
	}

	return FALSE;
}

static u32 fmeter_freq(FMETER_TYPE type, int k1, int clk)
{
	u32 cksw_ckgen[] = {15, 8, clk};
	u32 cksw_abist[] = {23, 16, clk};
	u32 fqmtr_ck =		(type == CKGEN) ? 1 : 0;
	u32 clk_cfg_reg =	(type == CKGEN) ? CLK_DBG_CFG	: CLK_DBG_CFG;
	u32 cnt_reg =		(type == CKGEN) ? CLK26CALI_1	: CLK26CALI_1;
	u32 *cksw_hlv =		(type == CKGEN) ? cksw_ckgen	: cksw_abist;
	u32 tri_bit =		(type == CKGEN) ? BIT(4)	: BIT(4);

	/* setup fmeter */
	clk_writel_mask(CLK_DBG_CFG, 1, 0, fqmtr_ck);	/* fqmtr_ck_sel */
	clk_setl(CLK26CALI_0, BIT(12));			/* enable fmeter_en */

	u32 clk_misc_cfg_0 = clk_readl(CLK_MISC_CFG_0);	/* backup reg value */
	u32 clk_cfg_val = clk_readl(clk_cfg_reg);

	set_fmeter_divider(k1);				/* set div (0 = /1) */
	set_fmeter_divider_ca7(k1);
	clk_writel_mask(clk_cfg_reg, cksw_hlv[0], cksw_hlv[1], cksw_hlv[2]);

	clk_setl(CLK26CALI_0, tri_bit);			/* start fmeter */

	u32 freq = 0;

	if (wait_fmeter_done(tri_bit)) {
		u32 cnt = clk_readl(cnt_reg) & 0xFFFF;

		/* freq = counter * 26M / 1024 (KHz) */
		freq = (cnt * 26000) * (k1 + 1) / 1024;
	}

	/* restore register settings */
	clk_writel(clk_cfg_reg, clk_cfg_val);
	clk_writel(CLK_MISC_CFG_0, clk_misc_cfg_0);

	return freq;
}

static u32 measure_stable_fmeter_freq(FMETER_TYPE type, int k1, int clk, BOOL f32k)
{
	u32 (*fmeter)(FMETER_TYPE, int, int);
	fmeter = fmeter_freq;

	u32 last_freq = 0;
	u32 freq = fmeter(type, k1, clk);
	u32 maxfreq = max(freq, last_freq);

	while (maxfreq > 0 && ABS_DIFF(freq, last_freq) * 100 / maxfreq > 10) {
		last_freq = freq;
		freq = fmeter(type, k1, clk);
		maxfreq = max(freq, last_freq);
	}

	return freq;
}

static u32 measure_abist_freq(enum ABIST_CLK clk)
{
	return measure_stable_fmeter_freq(ABIST, 0, clk, FALSE);
}

static u32 measure_ckgen_freq(enum CKGEN_CLK clk)
{
	return measure_stable_fmeter_freq(CKGEN, 0, clk, FALSE);
}

static u32 measure_ckgen_freq_32k(enum CKGEN_CLK clk)
{
	return measure_stable_fmeter_freq(CKGEN, 0, clk, TRUE);
}

static void measure_abist_clock(enum ABIST_CLK clk)
{
	u32 freq = measure_abist_freq(clk);
	TRACE("%d: %s: %u\n", clk, ABIST_CLK_NAME[clk], freq);
}

static void measure_ckgen_clock(enum CKGEN_CLK clk)
{
	u32 freq = measure_ckgen_freq(clk);
	TRACE("%d: %s: %u\n", clk, CKGEN_CLK_NAME[clk], freq);
}

static void measure_all_clocks(void)
{
	enum ABIST_CLK abist_clk[] = {
		AD_ARMCA7PLL_754M_CORE_CK,
		AD_OSC_CK,
		AD_MAIN_H546M_CK,
		AD_MAIN_H364M_CK,
		AD_MAIN_H218P4M_CK,
		AD_MAIN_H156M_CK,
		AD_UNIV_178P3M_CK,
		AD_UNIV_48M_CK,
		AD_UNIV_624M_CK,
		AD_UNIV_416M_CK,
		AD_UNIV_249P6M_CK,
		AD_APLL1_180P6336M_CK,
		AD_APLL2_196P608M_CK,
		AD_LTEPLL_FS26M_CK,
		RTC32K_CK_I,
		AD_MMPLL_700M_CK,
		AD_VENCPLL_410M_CK,
		AD_TVDPLL_594M_CK,
		AD_MPLL_208M_CK,
		AD_MSDCPLL_806M_CK,
		AD_USB_48M_CK,
		MIPI_DSI_TST_CK,
		AD_PLLGP_TST_CK,
		AD_LVDSTX_MONCLK,
		AD_DSI0_LNTC_DSICLK,
		AD_DSI0_MPPLL_TST_CK,
		AD_CSI0_DELAY_TSTCLK,
		AD_CSI1_DELAY_TSTCLK,
		AD_HDMITX_MON_CK,
		AD_ARMPLL_1508M_CK,
		AD_MAINPLL_1092M_CK,
		AD_MAINPLL_PRO_CK,
		AD_UNIVPLL_1248M_CK,
		AD_LVDSPLL_150M_CK,
		AD_LVDSPLL_ETH_CK,
		AD_SSUSB_48M_CK,
		AD_MIPI_26M_CK,
		AD_MEM_26M_CK,
		AD_MEMPLL_MONCLK,
		AD_MEMPLL2_MONCLK,
		AD_MEMPLL3_MONCLK,
		AD_MEMPLL4_MONCLK,
		AD_MEMPLL_REFCLK_BUF,
		AD_MEMPLL_FBCLK_BUF,
		AD_MEMPLL2_REFCLK_BUF,
		AD_MEMPLL2_FBCLK_BUF,
		AD_MEMPLL3_REFCLK_BUF,
		AD_MEMPLL3_FBCLK_BUF,
		AD_MEMPLL4_REFCLK_BUF,
		AD_MEMPLL4_FBCLK_BUF,
		AD_USB20_MONCLK,
		AD_USB20_MONCLK_1P,
		AD_MONREF_CK,
		AD_MONFBK_CK,
	};

	enum CKGEN_CLK ckgen_clk[] = {
		HD_FAXI_CK,
		HF_FDDRPHYCFG_CK,
		F_FPWM_CK,
		HF_FVDEC_CK,
		HF_FMM_CK,
		HF_FCAMTG_CK,
		F_FUART_CK,
		HF_FSPI_CK,
		HF_FMSDC_30_0_CK,
		HF_FMSDC_30_1_CK,
		HF_FMSDC_30_2_CK,
		HF_FMSDC_50_3_CK,
		HF_FMSDC_50_3_HCLK_CK,
		HF_FAUDIO_CK,
		HF_FAUD_INTBUS_CK,
		HF_FPMICSPI_CK,
		HF_FSCP_CK,
		HF_FATB_CK,
		HF_FSNFI_CK,
		HF_FDPI0_CK,
		HF_FAUD_1_CK_PRE,
		HF_FAUD_2_CK_PRE,
		HF_FSCAM_CK,
		HF_FMFG_CK,
		ECO_AO12_MEM_CLKMUX_CK,
		ECO_A012_MEM_DCM_CK,
		HF_FDPI1_CK,
		HF_FUFOENC_CK,
		HF_FUFODEC_CK,
		HF_FETH_CK,
		HF_FONFI_CK,
	};

	size_t i;
	u32 old_pll_hp_en = clk_readl(PLL_HP_EN);

	clk_writel(PLL_HP_EN, 0x0);		/* disable PLL hopping */
	udelay(10);

	for (i = 0; i < ARRAY_SIZE(abist_clk); i++)
		measure_abist_clock(abist_clk[i]);

	for (i = 0; i < ARRAY_SIZE(ckgen_clk); i++)
		measure_ckgen_clock(ckgen_clk[i]);

	/* restore old setting */
	clk_writel(PLL_HP_EN, old_pll_hp_en);
}

#endif /* FMETER_EN */

//after pmic_init
void mt_pll_post_init(void)
{
    unsigned int temp;

    //mt_mempll_init();
    mt_mempll_init_new(DDR_MEMPLL, DDR_PLL_MODE);
    mt_mempll_cali();

    //set mem_clk
    DRV_WriteReg32(CLK_CFG_0, 0x01000101); //mem_ck =mempll
    DRV_WriteReg32(CLK_CFG_UPDATE, 0x2);

    //step 48
    temp = DRV_Reg32(AP_PLL_CON3);
    DRV_WriteReg32(AP_PLL_CON3, temp & 0xFFF44440); // Only UNIVPLL SW Control

    //step 49
    temp = DRV_Reg32(AP_PLL_CON4);
    DRV_WriteReg32(AP_PLL_CON4, temp & 0xFFFFFFF4); // Only UNIVPLL SW Control

//    print("mt_pll_post_init: mt_get_cpu_freq = %dKhz\n", mt_get_cpu_freq());
//    print("mt_pll_post_init: mt_get_bus_freq = %dKhz\n", mt_get_bus_freq());
//    print("mt_pll_post_init: mt_get_mem_freq = %dKhz\n", mt_get_mem_freq());

#if FMETER_EN
    measure_all_clocks();
#endif

#if 0
    print("mt_pll_post_init: AP_PLL_CON3        = 0x%x, GS = 0x00000000\n", DRV_Reg32(AP_PLL_CON3));
    print("mt_pll_post_init: AP_PLL_CON4        = 0x%x, GS = 0x00000000\n", DRV_Reg32(AP_PLL_CON4));
    print("mt_pll_post_init: AP_PLL_CON6        = 0x%x, GS = 0x00000000\n", DRV_Reg32(AP_PLL_CON6));
    print("mt_pll_post_init: CLKSQ_STB_CON0     = 0x%x, GS = 0x05010501\n", DRV_Reg32(CLKSQ_STB_CON0));
    print("mt_pll_post_init: PLL_ISO_CON0       = 0x%x, GS = 0x00080008\n", DRV_Reg32(PLL_ISO_CON0));
    print("mt_pll_post_init: ARMPLL_CON0        = 0x%x, GS = 0xF1000101\n", DRV_Reg32(ARMPLL_CON0));
    print("mt_pll_post_init: ARMPLL_CON1        = 0x%x, GS = 0x800E8000\n", DRV_Reg32(ARMPLL_CON1));
    print("mt_pll_post_init: ARMPLL_PWR_CON0    = 0x%x, GS = 0x00000001\n", DRV_Reg32(ARMPLL_PWR_CON0));
    print("mt_pll_post_init: MAINPLL_CON0       = 0x%x, GS = 0xF1000101\n", DRV_Reg32(MAINPLL_CON0));
    print("mt_pll_post_init: MAINPLL_CON1       = 0x%x, GS = 0x800A8000\n", DRV_Reg32(MAINPLL_CON1));
    print("mt_pll_post_init: MAINPLL_PWR_CON0   = 0x%x, GS = 0x00000001\n", DRV_Reg32(MAINPLL_PWR_CON0));
    print("mt_pll_post_init: UNIVPLL_CON0       = 0x%x, GS = 0xFF000011\n", DRV_Reg32(UNIVPLL_CON0));
    print("mt_pll_post_init: UNIVPLL_CON1       = 0x%x, GS = 0x80180000\n", DRV_Reg32(UNIVPLL_CON1));
    print("mt_pll_post_init: UNIVPLL_PWR_CON0   = 0x%x, GS = 0x00000001\n", DRV_Reg32(UNIVPLL_PWR_CON0));
    print("mt_pll_post_init: MMPLL_CON0         = 0x%x, GS = 0x00000101\n", DRV_Reg32(MMPLL_CON0));
    print("mt_pll_post_init: MMPLL_CON1         = 0x%x, GS = 0x820D8000\n", DRV_Reg32(MMPLL_CON1));
    print("mt_pll_post_init: MMPLL_PWR_CON0     = 0x%x, GS = 0x00000001\n", DRV_Reg32(MMPLL_PWR_CON0));
    print("mt_pll_post_init: MSDCPLL_CON0       = 0x%x, GS = 0x00000111\n", DRV_Reg32(MSDCPLL_CON0));
    print("mt_pll_post_init: MSDCPLL_CON1       = 0x%x, GS = 0x800F6276\n", DRV_Reg32(MSDCPLL_CON1));
    print("mt_pll_post_init: MSDCPLL_PWR_CON0   = 0x%x, GS = 0x00000001\n", DRV_Reg32(MSDCPLL_PWR_CON0));
    print("mt_pll_post_init: TVDPLL_CON0        = 0x%x, GS = 0x00000101\n", DRV_Reg32(TVDPLL_CON0));
    print("mt_pll_post_init: TVDPLL_CON1        = 0x%x, GS = 0x80112276\n", DRV_Reg32(TVDPLL_CON1));
    print("mt_pll_post_init: TVDPLL_PWR_CON0    = 0x%x, GS = 0x00000001\n", DRV_Reg32(TVDPLL_PWR_CON0));
    print("mt_pll_post_init: VENCPLL_CON0       = 0x%x, GS = 0x00000111\n", DRV_Reg32(VENCPLL_CON0));
    print("mt_pll_post_init: VENCPLL_CON1       = 0x%x, GS = 0x800E989E\n", DRV_Reg32(VENCPLL_CON1));
    print("mt_pll_post_init: VENCPLL_PWR_CON0   = 0x%x, GS = 0x00000001\n", DRV_Reg32(VENCPLL_PWR_CON0));
    print("mt_pll_post_init: MPLL_CON0          = 0x%x, GS = 0x00010111\n", DRV_Reg32(MPLL_CON0));
    print("mt_pll_post_init: MPLL_CON1          = 0x%x, GS = 0x801C0000\n", DRV_Reg32(MPLL_CON1));
    print("mt_pll_post_init: MPLL_PWR_CON0      = 0x%x, GS = 0x00000001\n", DRV_Reg32(MPLL_PWR_CON0));
    print("mt_pll_post_init: AUD1PLL_CON0       = 0x%x, GS = 0xF0000131\n", DRV_Reg32(AUD1PLL_CON0));
    print("mt_pll_post_init: AUD1PLL_CON1       = 0x%x, GS = 0xB7945EA6\n", DRV_Reg32(AUD1PLL_CON1));
    print("mt_pll_post_init: AUD1PLL_PWR_CON0   = 0x%x, GS = 0x00000001\n", DRV_Reg32(AUD1PLL_PWR_CON0));
    print("mt_pll_post_init: AUD2PLL_CON0       = 0x%x, GS = 0x00000131\n", DRV_Reg32(AUD2PLL_CON0));
    print("mt_pll_post_init: AUD2PLL_CON1       = 0x%x, GS = 0xBC7EA932\n", DRV_Reg32(AUD2PLL_CON1));
    print("mt_pll_post_init: AUD2PLL_PWR_CON0   = 0x%x, GS = 0x00000001\n", DRV_Reg32(AUD2PLL_PWR_CON0));

    print("mt_pll_post_init:  SPM_PWR_STATUS    = 0x%x, \n", DRV_Reg32(SPM_PWR_STATUS));
    print("mt_pll_post_init:  DISP_CG_CON0    = 0x%x, \n", DRV_Reg32(DISP_CG_CON0));
    print("mt_pll_post_init:  DISP_CG_CON1    = 0x%x, \n", DRV_Reg32(DISP_CG_CON1));
#endif
}

void mt_pll_init(void)
{
    int ret = 0;
    unsigned int temp;

    //MT8163 workaround
    #define NFI_1X_INFRA_SEL 0x10001098
    temp = DRV_Reg32(NFI_1X_INFRA_SEL);
    temp &= (0x1 << 10);
    DRV_WriteReg32(NFI_1X_INFRA_SEL, temp);

    DRV_WriteReg32(ACLKEN_DIV, 0x12); // MCU Bus DIV2

    //step 1
    DRV_WriteReg32(CLKSQ_STB_CON0, 0x05010501); // reduce CLKSQ disable time

    //step 2
    DRV_WriteReg32(PLL_ISO_CON0, 0x00080008); // extend PWR/ISO control timing to 1us

    //step 3
    DRV_WriteReg32(AP_PLL_CON6, 0x00000000); //

    /*************
    * xPLL PWR ON
    **************/
    //step 4
    temp = DRV_Reg32(ARMPLL_PWR_CON0);
    DRV_WriteReg32(ARMPLL_PWR_CON0, temp | 0x1);

    //step 5
    temp = DRV_Reg32(MAINPLL_PWR_CON0);
    DRV_WriteReg32(MAINPLL_PWR_CON0, temp | 0x1);

    //step 6
    temp = DRV_Reg32(UNIVPLL_PWR_CON0);
    DRV_WriteReg32(UNIVPLL_PWR_CON0, temp | 0x1);

    //step 7
    temp = DRV_Reg32(MMPLL_PWR_CON0);
    DRV_WriteReg32(MMPLL_PWR_CON0, temp | 0x1);

    //step 8
    temp = DRV_Reg32(MSDCPLL_PWR_CON0);
    DRV_WriteReg32(MSDCPLL_PWR_CON0, temp | 0x1);

    //step 9
    temp = DRV_Reg32(VENCPLL_PWR_CON0);
    DRV_WriteReg32(VENCPLL_PWR_CON0, temp | 0x1);

    //step 10
    temp = DRV_Reg32(TVDPLL_PWR_CON0);
    DRV_WriteReg32(TVDPLL_PWR_CON0, temp | 0x1);

    //step 11
    temp = DRV_Reg32(MPLL_PWR_CON0);
    DRV_WriteReg32(MPLL_PWR_CON0, temp | 0x1);

    //step 12
    temp = DRV_Reg32(LVDSPLL_PWR_CON0);
    DRV_WriteReg32(LVDSPLL_PWR_CON0, temp | 0x1);

    //step 13
    temp = DRV_Reg32(AUD1PLL_PWR_CON0);
    DRV_WriteReg32(AUD1PLL_PWR_CON0, temp | 0x1);

    //step 14
    temp = DRV_Reg32(AUD2PLL_PWR_CON0);
    DRV_WriteReg32(AUD2PLL_PWR_CON0, temp | 0x1);

    gpt_busy_wait_us(5); // wait for xPLL_PWR_ON ready (min delay is 1us)

    /******************
    * xPLL ISO Disable
    *******************/
    //step 15
    temp = DRV_Reg32(ARMPLL_PWR_CON0);
    DRV_WriteReg32(ARMPLL_PWR_CON0, temp & 0xFFFFFFFD);

    //step 16
    temp = DRV_Reg32(MAINPLL_PWR_CON0);
    DRV_WriteReg32(MAINPLL_PWR_CON0, temp & 0xFFFFFFFD);

    //step 17
    temp = DRV_Reg32(UNIVPLL_PWR_CON0);
    DRV_WriteReg32(UNIVPLL_PWR_CON0, temp & 0xFFFFFFFD);

    //step 18
    temp = DRV_Reg32(MMPLL_PWR_CON0);
    DRV_WriteReg32(MMPLL_PWR_CON0, temp & 0xFFFFFFFD);

    //step 19
    temp = DRV_Reg32(MSDCPLL_PWR_CON0);
    DRV_WriteReg32(MSDCPLL_PWR_CON0, temp & 0xFFFFFFFD);

    //step 20
    temp = DRV_Reg32(VENCPLL_PWR_CON0);
    DRV_WriteReg32(VENCPLL_PWR_CON0, temp & 0xFFFFFFFD);

    //step 21
    temp = DRV_Reg32(TVDPLL_PWR_CON0);
    DRV_WriteReg32(TVDPLL_PWR_CON0, temp & 0xFFFFFFFD);

    //step 22
    temp = DRV_Reg32(MPLL_PWR_CON0);
    DRV_WriteReg32(MPLL_PWR_CON0, temp & 0xFFFFFFFD);

    //step 23
    temp = DRV_Reg32(LVDSPLL_PWR_CON0);
    DRV_WriteReg32(LVDSPLL_PWR_CON0, temp & 0xFFFFFFFD);

    //step 24
    temp = DRV_Reg32(AUD1PLL_PWR_CON0);
    DRV_WriteReg32(AUD1PLL_PWR_CON0, temp & 0xFFFFFFFD);

    //step 25
    temp = DRV_Reg32(AUD2PLL_PWR_CON0);
    DRV_WriteReg32(AUD2PLL_PWR_CON0, temp & 0xFFFFFFFD);

    /********************
    * xPLL Frequency Set
    *********************/
    //step 26
    // DRV_WriteReg32(ARMPLL_CON1, 0x800E8000); // 1508 MHz
    DRV_WriteReg32(ARMPLL_CON1, 0x8009A000); // 1001 MHz

    //step 27
    DRV_WriteReg32(MAINPLL_CON1, 0x800A8000); // 1092 MHz

    //step 28
    DRV_WriteReg32(MMPLL_CON1, 0x820E9D89); // 380 MHz

    //step 29
    DRV_WriteReg32(MSDCPLL_CON1, 0x800F6276); // 400 MHz

    //step 30
    DRV_WriteReg32(VENCPLL_CON1, 0x800E0000); // 364 MHz

    //step 31
    DRV_WriteReg32(TVDPLL_CON1, 0x8016D89E); // 594 MHz

    //step 32
    DRV_WriteReg32(MPLL_CON1, 0x80100000); // 208 MHz

    //step 33
    DRV_WriteReg32(LVDSPLL_CON1, 0x800B89D8); // 150 MHz

    //APLL1 and APLL2 use the default setting
    DRV_WriteReg32(AUD1PLL_CON1, 0xB7945EA5); // 90.3168 MHz
    DRV_WriteReg32(AUD1PLL_CON0, 0x00000140); 
    DRV_WriteReg32(AUD2PLL_CON1, 0xBC7EA932); // 98.3040 MHz
    DRV_WriteReg32(AUD2PLL_CON0, 0x00000140); 

    /***********************
    * xPLL Frequency Enable
    ************************/
    //step 34
    temp = DRV_Reg32(ARMPLL_CON0);
    DRV_WriteReg32(ARMPLL_CON0, temp | 0x1);

    //step 35
    temp = DRV_Reg32(MAINPLL_CON0);
    DRV_WriteReg32(MAINPLL_CON0, temp | 0x1);

    //step 36
    temp = DRV_Reg32(UNIVPLL_CON0);
    DRV_WriteReg32(UNIVPLL_CON0, temp | 0x1);

    //step 37
    temp = DRV_Reg32(MMPLL_CON0);
    DRV_WriteReg32(MMPLL_CON0, temp | 0x1);

    //step 38
    temp = DRV_Reg32(MSDCPLL_CON0);
    DRV_WriteReg32(MSDCPLL_CON0, temp | 0x1);

    //step 39
    temp = DRV_Reg32(VENCPLL_CON0);
    DRV_WriteReg32(VENCPLL_CON0, temp | 0x1);

    //step 40
    temp = DRV_Reg32(TVDPLL_CON0);
    DRV_WriteReg32(TVDPLL_CON0, temp | 0x1);

    //step 41
    temp = DRV_Reg32(MPLL_CON0);
    DRV_WriteReg32(MPLL_CON0, temp | 0x1);

    //step 42
    temp = DRV_Reg32(LVDSPLL_CON0);
    DRV_WriteReg32(LVDSPLL_CON0, temp | 0x1);

    //step 43
    temp = DRV_Reg32(AUD1PLL_CON0);
    DRV_WriteReg32(AUD1PLL_CON0, temp | 0x1);

    //step 44
    temp = DRV_Reg32(AUD2PLL_CON0);
    DRV_WriteReg32(AUD2PLL_CON0, temp | 0x1);

    gpt_busy_wait_us(40); // wait for PLL stable (min delay is 20us)

    /***************
    * xPLL DIV RSTB
    ****************/
    //step 45
    temp = DRV_Reg32(ARMPLL_CON0);
    DRV_WriteReg32(ARMPLL_CON0, temp | 0x01000000);

    //step 46
    temp = DRV_Reg32(MAINPLL_CON0);
    DRV_WriteReg32(MAINPLL_CON0, temp | 0x01000000);

    //step 47
    temp = DRV_Reg32(UNIVPLL_CON0);
    DRV_WriteReg32(UNIVPLL_CON0, temp | 0x01000000);

    /*****************
    * xPLL HW Control
    ******************/
    // default is SW mode, set HW mode after MEMPLL caribration

    /*************
    * MEMPLL Init
    **************/

//    mt_mempll_pre();

    /**************
    * INFRA CLKMUX
    ***************/

    //temp = DRV_Reg32(TOP_DCMCTL);
    //DRV_WriteReg32(TOP_DCMCTL, temp | 0x1); // enable infrasys DCM

    DRV_WriteReg32(INFRA_BUS_DCM_CTRL, 0x001f0603);
    DRV_WriteReg32(PERI_BUS_DCM_CTRL, 0xb01f0603);
    temp = DRV_Reg32(INFRA_BUS_DCM_CTRL);
    DRV_WriteReg32(INFRA_BUS_DCM_CTRL, temp | (0x1<<22));

    //CA7: INFRA_TOPCKGEN_CKDIV1[4:0](0x10001008)
    temp = DRV_Reg32(TOP_CKDIV1);
    DRV_WriteReg32(TOP_CKDIV1, temp & 0xFFFFFFE0); // CPU clock divide by 1

    //CA7: INFRA_TOPCKGEN_CKMUXSEL[1:0] (0x10001000) =1
    temp = DRV_Reg32(TOP_CKMUXSEL);
    DRV_WriteReg32(TOP_CKMUXSEL, temp | 0x1); // switch CA7_ck to ARMCA7PLL

    /************
    * TOP CLKMUX
    *************/

    DRV_WriteReg32(CLK_CFG_0, 0x01000001); //mm_ck=syspll_d3, ddrphycfg_ck=26M, mem_ck=26M, axi=syspll1_d4

    DRV_WriteReg32(CLK_CFG_1, 0x01000100); //mfg_ck=mmpll_ck, Null, vdec_ck=syspll_d3, pwm_ck=26Mhz

    DRV_WriteReg32(CLK_CFG_2, 0x00010000); //Null, spi_ck=syspll3_d2, uart=26M, camtg=26M

    DRV_WriteReg32(CLK_CFG_3, 0x00020202); //Null, msdc30_2_ck=msdcpll_d2, msdc30_1_ck=msdcpll_d2, msdc30_0_ck=msdcpll_d2

    DRV_WriteReg32(CLK_CFG_4, 0x01000101); //aud_intbus=syspll1_d4, aud=26M, msdc50_3=msdcpll, msdc50_3_hclk=syspll1_d2

    DRV_WriteReg32(CLK_CFG_5, 0x01010100); //mjc_ck=syspll_d5, atb_ck=syspll1_d2, scp_ck=mpll, pmicspi=26MHz

    DRV_WriteReg32(CLK_CFG_6, 0x01010101); //aud2_ck=apll2 , aud1_ck=apll1, scam_ck=syspll3_d2, dpi0_ck=lvdspll

    DRV_WriteReg32(CLK_CFG_7, 0x00040501); //eth_ck=26M , ufodec_ck=univpll_d3, ufoenc_ck=univpll_d3, dpi1_ck=tvdpll_d2

    DRV_WriteReg32(CLK_CFG_8, 0x01010000); //clk_rtc=external , hdmi_ck=fhdmi_cts_ck, snfi_ck=26M, onfi_ck=26M

    DRV_WriteReg32(CLK_CFG_UPDATE, 0xffffffff);
    DRV_WriteReg32(CLK_CFG_UPDATE_1, 0x00000001);

    DRV_WriteReg32(CLK_SCP_CFG_0, 0x3FF); // enable scpsys clock off control
    DRV_WriteReg32(CLK_SCP_CFG_1, 0x7); // enable scpsys clock off control

    /*for MTCMOS*/
    spm_write(SPM_POWERON_CONFIG_SET, (SPM_PROJECT_CODE << 16) | (1U << 0));
    spm_mtcmos_ctrl_disp(STA_POWER_ON);
    spm_mtcmos_ctrl_vdec(STA_POWER_ON);
    spm_mtcmos_ctrl_venc(STA_POWER_ON);
    spm_mtcmos_ctrl_isp(STA_POWER_ON);
    spm_mtcmos_ctrl_aud(STA_POWER_ON);
    spm_mtcmos_ctrl_mfg_ASYNC(STA_POWER_ON);
    spm_mtcmos_ctrl_mfg(STA_POWER_ON);
    spm_mtcmos_ctrl_connsys(STA_POWER_ON);

    /*for CG*/
    DRV_WriteReg32(INFRA_PDN_CLR0, 0xFFFFFFFF);
    DRV_WriteReg32(INFRA_PDN_CLR1, 0xFFFFFFFF);
    /*DISP CG*/
    //DRV_WriteReg32(DISP_CG_CLR0, 0xFFFFFFFF);
    //DRV_WriteReg32(DISP_CG_CLR1, 0x3F);
    //AUDIO
    DRV_WriteReg32(AUDIO_TOP_CON0, 0);
    //MFG
    DRV_WriteReg32(MFG_CG_CLR, 0x0000000F);
    //ISP
    DRV_WriteReg32(IMG_CG_CLR, 0x000003F1);
    //VDE
    DRV_WriteReg32(VDEC_CKEN_SET, 0x00000001);
    DRV_WriteReg32(LARB_CKEN_SET, 0x00000001);
    //VENC
    DRV_WriteReg32(VENC_CG_SET, 0x00001111);
}
