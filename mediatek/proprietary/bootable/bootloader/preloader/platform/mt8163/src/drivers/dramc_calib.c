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

#include <dramc_common.h>
#include <typedefs.h>
#include <platform.h>
#include <emi_hw.h>
#include <emi.h>
//#include "custom_emi.h"
#include <device_apc.h>

#define DEC_DQS_VALUE (0)
#define DEFAULT_DQSSTEP_VALUE 4
#define HW_DQ_DQS_CALIB_TIMEOUT 10000
#define DQ_DQS_DQM_REMAPPING

#if 1 // Support catraining and writeleveling
#define WL_CLKADJUST

#ifdef WL_CLKADJUST
static S8 CATrain_ClkDelay[1]; // one channel only for 8163
#endif

#ifdef WL_CLKADJUST
		#define CATRAINING_STEP	    9
#else
	#ifdef fcMEMPLL_SEAL_RING
		#define CATRAINING_STEP	    5
	#else // CTS
		#define CATRAINING_STEP	    0
	#endif
#endif

#define CA_TRAINING
//#define WRITE_LEVELING

#ifdef CA_TRAINING
int DramcCATraining(void);
int DramcCATraining_178Ball(void);
#endif

#ifdef WRITE_LEVELING
int DramcWriteLeveling(void);
#define WRITE_LEVELING_NO_SETTING
#endif
#endif

//#define RX_DELAY_MID_VAL_IF_ALWAYS_PASS


//#define DEBUG_DRAMC_CALIB
//#define DEBUG
#ifdef DEBUG
#define dbg_print print
#else
#define dbg_print
#endif

#define RX_PRINT //print
#define TX_PRINT //print
#define CA_PRINT //print
#define WL_PRINT //print
#define TA_MAX_LEN 0xFFFFFF //tmp code


#define DRAM_BASE 0x40000000

#ifdef  DQ_DQS_DQM_REMAPPING
//-------------------------------------------------------------------------------------
/*MT8127_DDR_PINMUX*/
//--------------------------------------------------------------------------------------------

unsigned char Bit_DQO_Mapping_no_4bitmux[32] =
{
    0,  1,  2,  3,  4,  5,  6,  7, //Bit0~7
    8,  9,  10, 11, 12, 13, 14, 15, //Bit8~15
    16, 17, 18, 19, 20, 21, 22, 23, //Bit16~23
    24, 25, 26, 27, 28, 29, 30, 31  //Bit24~31
};

unsigned char Bit_DQO_Mapping_4bitmux[32] =
{

	0,  1,  2,  3,  8,  9,  10, 11,  //Bit0~7
    4,  5,  6,  7,  12, 13, 14, 15,  //Bit8~15
    16, 17, 18, 19, 24, 25, 26, 27,  //Bit16~23
    20, 21, 22, 23, 28, 29, 30, 31   //Bit24~31
};
unsigned char DQSO_Mapping[4] = {0, 1, 3, 2}; 	// This could be by DQSx for CPU/CMP_ERR bit 0~7, bit 8~15, bit 16~23, bit 24~31
unsigned char DQM_Mapping[4] = {0, 1, 3, 2};
//--------------------------------------------------------------------------------------------
#endif /*#ifdef DQ_DQS_DQM_REMAPPING*/

int RANK_CURR = 0;

#ifdef DDR3_AUTO_DETECT_SUPPORT
extern EMI_SETTINGS emi_settings[];
extern int DDR_index;
#include "device_apc.h"
#define ABORT_EMI               0x00400000 //DEVAPC0_D0_VIO_STA_3, idx:13
unsigned int ddr3_autodetect_is_enable(void)
{
	EMI_SETTINGS *emi_set = &emi_settings[DDR_index];
	if((mt_get_dram_type() == TYPE_PCDDR3) && (emi_set->PCDDR3_MODE_REG5 & (0x1 << 31)))
	{
		return 1; //ddr3 auto detect is enabled
	}
	else
	{
		return 0;
	}
}


void ddr3_rank_size_detect(void)
{
	unsigned int i;
	unsigned int start_addr, test_addr;
	unsigned int start_value, test_value;
	unsigned int u4pattern = 0x12345678;
	unsigned int real_rank_size;

	print("[EMI]rank size auto detect\n");

	/*to detect size , we have to make sure it's single rank and it has maximum addressing region*/
	*(volatile unsigned *)(EMI_CONA) = 0x3012;
	start_addr = DRAM_BASE;
	*((volatile unsigned int *)start_addr) = u4pattern;
	if(*((volatile unsigned int *)start_addr) != u4pattern)
	{
		print("[EMI]detect rank%d size error!!\n", RANK_CURR);
		ASSERT(0);
	}

	for (i=0; i< 5; i++)
	{
		test_addr = start_addr + (0x8000000 << i);
		*((volatile unsigned int *)test_addr) = ~u4pattern;
		DDR_PHY_RESET_NEW();

		start_value = *((volatile unsigned int *)start_addr);
		test_value = *((volatile unsigned int *)test_addr);
		//print("[EMI]start_addr[0x%x]=0x%x, test_addr[0x%x]= 0x%x\n", start_addr, start_value, test_addr, test_value);
		if ((test_value != ~u4pattern) || (test_value == start_value))
		{
			break;
		}
	}

	real_rank_size = 0x8000000 << i;
	print("[EMI]rank%d size: 0x%x\n", RANK_CURR, real_rank_size);
    emi_settings[DDR_index].DRAM_RANK_SIZE[RANK_CURR]= real_rank_size;

    if(i == 0 || i == 1)
    { // i=0(128MB),i=1(256MB)
	  print("[EMI]rank size too small error!!!\n");
	  ASSERT(0);
    }
	else if(i == 2)
	{ //  i=2(512MB) only one rank is supported  in this case
	   *(volatile unsigned *)(EMI_CONA) = 0x1012;
	}
	else if(i == 3)
	{// i=3(1GB,2GB) if 1GB, it is maybe  1 rank or 2 ranks
	  // anyway ,we try 2 ranks firstly....if succeeds , set 2 ranks,else 1 rank
	  *(volatile unsigned *)(EMI_CONA) =  0x2A052;
	  //separate cs0 with cs1
	  *((volatile unsigned int *)(DRAMC0_BASE + 0x01ec)) &= 0xffffefff;
	  *((volatile unsigned int *)(DDRPHY_BASE + 0x01ec)) &= 0xffffefff;

	}

	DDR_PHY_RESET_NEW();

  DRV_WriteReg32(EMI_MPUP, 0x000003FF);
  DRV_WriteReg32(EMI_MPUQ, 0x000003FF);
  DRV_WriteReg32(EMI_MPUR, 0x000003FF);
  DRV_WriteReg32(EMI_MPUY, 0x000003FF);

  /* clear debug info */
  DRV_WriteReg32(EMI_MPUS, 0x80000000);

  reg_write32(DEVAPC0_PD_APC_CON, 0);
  reg_write32(DEVAPC0_D0_VIO_STA_4, 0x00400000);


}
#endif


extern char **opt_gw_coarse_value, **opt_gw_fine_value;
extern char **opt_gw_coarse_value0, **opt_gw_fine_value0;
extern char **opt_gw_coarse_value1, **opt_gw_fine_value1;
extern char* opt_dle_value;
extern void dqsi_gw_dly_coarse_factor_handler(char **, char);
extern void dqsi_gw_dly_coarse_factor_handler_rank1(char **, char);
extern void dqsi_gw_dly_fine_factor_handler(char **, char);
extern void dqsi_gw_dly_fine_factor_handler_rank1(char **, char);
unsigned int opt_rx_dqs0;
unsigned int opt_rx_dqs1;
unsigned int opt_tx_dq[4];
unsigned int opt_tx_dqs;
unsigned int opt_tx_dqm;
int DQS_PRINT = 1;
typedef struct _RXDQS_PERBIT_DLY_T
{
    S8 min_cur;
    S8 max_cur;
    S8 min_best;
    S8 max_best;
    U8 center;
    U8 dq_dly_last;
} RXDQS_PERBIT_DLY_T;
#define DQ_DATA_WIDTH 32
#define MAX_RX_DQSDLY_TAPS 128
#define MAX_RX_DQDLY_TAPS 16
#define DQS_NUMBER 4
#define DQS_BIT_NUMBER (DQ_DATA_WIDTH/DQS_NUMBER)

/* Description
  *	RX DQ/DQS per bit calibration.
  * Registers
  *	- DQIDLY[1:8] : one register had 4 settings (4bits: 0~15, unit 20ps) with corresponding DQx
  *	- R0DELDLY : 4 settings for rank 0 DQS0~DQS3. 7 bits (0~127) with unit 30ps.
  *	- R1DELDLY : 4 settings for rank 1 DQS0~DQS3. 7 bits (0~127) with unit 30ps.
  * Algorithm
  *	- Set DQS/DQ input delay to 0.
  *	- Delay all DQs from 0 to 15 until all failed.
  *	- Delay DQSs from 0 to 127 to find the pass range (min & max) of each DQ. Further find the largest pass range.
  *	- For each DQS, find the largest middle value of corresponding DQ byte. Then use this value to set each DQS input delay.
  *	- For each DQ, find the difference between original middle DQS delay and max DQS delay per byte. Then delay the difference more to align the middle of DQS per byte.
  */
#define RX_UPDATE 1
#if RX_UPDATE
#if 0
char Perbit_DQ_Ofst[32] =
{
    3, 3, 2, 3, 2, 2, 2, 3,
    1, 1, 1, 1, 2, 1, 1, 1,
    1, 0, 2, 1, 0, 2, 0, 0,
    3, 4, 3, 4, 4, 3, 4, 5
};
#else
char Perbit_DQ_Ofst[32] =
{
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0
};
#endif
#endif
int do_sw_rx_dq_dqs_calib(void)
{
#if 1
    int result;
    unsigned int data, backup;
    int temp, timeout;
    unsigned int dqsi_dly0, dqsi_dly1, dqsi_dly2, dqsi_dly3;
    unsigned int test_len = 0xFFF; //0x100;
    unsigned int dqidly1, dqidly2, dqidly3, dqidly4,dqidly5,dqidly6,dqidly7;
    unsigned int i,j;
    unsigned int dqs_input_delay;
    unsigned int cmp_err;
    unsigned int max;

    unsigned int dq_dly_max;
    char dqs_delay[DQS_NUMBER];
    char dq_delay_per_bit[DQ_DATA_WIDTH];
    #if RX_UPDATE
    char dq_delay_per_bit_start[DQ_DATA_WIDTH];
    char dq_delay_per_bit_end[DQ_DATA_WIDTH];
    char dqs_delay_per_bit_start[DQ_DATA_WIDTH];
    char dqs_delay_per_bit_end[DQ_DATA_WIDTH];
    char dqs_delay_done[DQ_DATA_WIDTH];

    char diff = 0; //Per byte, Max(DQ right margin - DQ left margin)
    char total = 0; //Per byte, Max(DQ right margin + DQ left margin)
    #endif
    unsigned int dqidly[DQ_DATA_WIDTH/DQS_NUMBER];
    unsigned int tap;
    char dq_delay_done[DQ_DATA_WIDTH];
    RXDQS_PERBIT_DLY_T dqs_perbit_dly[DQ_DATA_WIDTH];

    result = 0;

    RX_PRINT("in do_sw_rx_dq_dqs_calib()\n");

#ifndef RELEASE
    RX_PRINT("*RX default:\n");
    RX_PRINT("*DQIDLY1 = 0x%x\n", DRAMC_READ_REG(DRAMC_DQIDLY1));
    RX_PRINT("*DQIDLY2 = 0x%x\n", DRAMC_READ_REG(DRAMC_DQIDLY2));
    RX_PRINT("*DQIDLY3 = 0x%x\n", DRAMC_READ_REG(DRAMC_DQIDLY3));
    RX_PRINT("*DQIDLY4 = 0x%x\n", DRAMC_READ_REG(DRAMC_DQIDLY4));
    RX_PRINT("*DQIDLY5 = 0x%x\n", DRAMC_READ_REG(DRAMC_DQIDLY5));
    RX_PRINT("*DQIDLY6 = 0x%x\n", DRAMC_READ_REG(DRAMC_DQIDLY6));
    RX_PRINT("*DQIDLY7 = 0x%x\n", DRAMC_READ_REG(DRAMC_DQIDLY7));
    RX_PRINT("*DQIDLY8 = 0x%x\n", DRAMC_READ_REG(DRAMC_DQIDLY8));
    RX_PRINT("*DRAMC_R0DELDLY = 0x%x\n", DRAMC_READ_REG(DRAMC_R0DELDLY));
#endif

    /*1. set DQS delay to 0 first*/
	DRAMC_WRITE_REG(0x0,DRAMC_R0DELDLY);

    // set DQ delay to 0x0.
    for (i = 0; i < 8; i++)
    {
        DRAMC_WRITE_REG(0x0,DRAMC_DQIDLY1+4*i);
    }

    // set DQ delay structure to 0x0.
    for (i = 0; i < DQ_DATA_WIDTH; i++)
    {
        dq_delay_per_bit[i] = 0x0;
        dq_delay_done[i] = 0x0;
        #if RX_UPDATE //todo: tmp code
        dq_delay_per_bit_start[i] = MAX_RX_DQDLY_TAPS;
        dq_delay_per_bit_end[i] = MAX_RX_DQDLY_TAPS;
        dqs_delay_per_bit_start[i] = MAX_RX_DQSDLY_TAPS;
        dqs_delay_per_bit_end[i] = MAX_RX_DQSDLY_TAPS;
        dqs_delay_done[i] = 0x0;
        #endif
    }

#if 1
	RX_PRINT("TX DQS:%x\n", DRAMC_READ_REG(0x0010));
	RX_PRINT("TX DQM:%x\n", DRAMC_READ_REG(0x0014));
	RX_PRINT("TX DQ 0x200:%x\n", DRAMC_READ_REG(0x0200));
	RX_PRINT("TX DQ 0x204:%x\n", DRAMC_READ_REG(0x0204));
	RX_PRINT("TX DQ 0x208:%x\n", DRAMC_READ_REG(0x0208));
	RX_PRINT("TX DQ 0x20c:%x\n", DRAMC_READ_REG(0x020c));
#endif

#if RX_UPDATE //todo: tmp code
    if (CHIP_VER_E1 == platform_chip_ver())
    {
	// delay DQ to find pass window
    for(tap = 0 ; tap < MAX_RX_DQDLY_TAPS; tap++ ){

        for (i = 0; i < 8; i++)
        {
            DRAMC_WRITE_REG(tap + (tap << 8) + (tap << 16) + (tap << 24), DRAMC_DQIDLY1+4*i);
        }

        /* set test patern length*/
        DRAMC_WRITE_REG(0x55000000,0x3C);

        data = DRAMC_READ_REG(0x40);
        DRAMC_WRITE_REG((data & 0xAA000000) | test_len, 0x40);

        //Test Agent 2 write enabling, Test Agent 2 read enabling
        DRAMC_WRITE_SET((1 << 30) | (1 << 31),DRAMC_CONF2);

        while(!(DRAMC_READ_REG(DRAMC_TESTRPT)&(1 << 10)));

        delay_a_while(400);

        cmp_err = DRAMC_READ_REG(DRAMC_CMP_ERR);
        //RX_PRINT("cmp_err:%x\n",cmp_err);
        DRAMC_WRITE_CLEAR(((1 << 30) | (1 << 31)),DRAMC_CONF2); //disable test agent2 r/w

        /* Bit i compare result
                * -Compare success & never fail before, record the delay value. (dq_delay_per_bit[i] = delay value)
                * -Compare fail. Record fail. (dq_delay_done[i] = 1)
             */

        for (i = 0; i < DQ_DATA_WIDTH; i++)
        {
            if (!(cmp_err&(0x1<<i)) && dq_delay_per_bit_start[i] == MAX_RX_DQDLY_TAPS)
            {
                dq_delay_per_bit_start[i] = tap;
                dq_delay_per_bit_end[i] = tap;
            }
            else if (!(cmp_err&(0x1<<i)) && dq_delay_done[i] == 0) //
            {
                dq_delay_per_bit_end[i] = tap;
            }
            else // error
            {
                if (dq_delay_per_bit_end[i] != MAX_RX_DQDLY_TAPS)
                {
                    //
                    dq_delay_done[i] = 1;
                    RX_PRINT("dq_delay_per_bit%d[%d:%d], tap:%d error again\n", i, dq_delay_per_bit_start[i], dq_delay_per_bit_end[i], tap);
                }
            }
        }
    }

    print("======================================\n");

    for (i = 0; i < DQ_DATA_WIDTH; i++)
    {
        print("DQS fix 0, Tune DQ%d, [%d, %d]\n", i, dq_delay_per_bit_start[i], dq_delay_per_bit_end[i]);
    }

    print("MAX_RX_DQSDLY_TAPS: %d\n", MAX_RX_DQSDLY_TAPS);

    // set DQ delay to 0x0.
    for (i = 0; i < 8; i++)
    {
        DRAMC_WRITE_REG(0x0,DRAMC_DQIDLY1+4*i);
    }

    for (tap = 0; tap < MAX_RX_DQSDLY_TAPS; tap++)
    {
        dqs_input_delay = (tap) + (tap << 8) + (tap << 16) + (tap << 24);

        //RX_PRINT("DQ fix 0, Tune DQS, %d\n", tap);

        DRAMC_WRITE_REG(dqs_input_delay,DRAMC_R0DELDLY);

        /* set test patern length*/
        DRAMC_WRITE_REG(0x55000000,0x3C);

        data = DRAMC_READ_REG(0x40);
        DRAMC_WRITE_REG((data & 0xAA000000) | test_len, 0x40);

        //Test Agent 2 write enabling, Test Agent 2 read enabling
        DRAMC_WRITE_SET((1 << 30) | (1 << 31),DRAMC_CONF2);

        while(!(DRAMC_READ_REG(DRAMC_TESTRPT)&(1 << 10)));

        delay_a_while(400);

        cmp_err = DRAMC_READ_REG(DRAMC_CMP_ERR);
        //RX_PRINT("cmp_err:%x\n",cmp_err);
        DRAMC_WRITE_CLEAR(((1 << 30) | (1 << 31)),DRAMC_CONF2); //disable test agent2 r/w

        for (j = 0; j < DQ_DATA_WIDTH; j++)
        {
            if (!(cmp_err&(0x1<<j)) && dqs_delay_per_bit_start[j] == MAX_RX_DQSDLY_TAPS)
            {
                dqs_delay_per_bit_start[j] = tap;
                dqs_delay_per_bit_end[j] = tap;
            }
            else if (!(cmp_err&(0x1<<j)) && dqs_delay_done[j] == 0) //
            {
                dqs_delay_per_bit_end[j] = tap;
            }
            else // error
            {
                if (dqs_delay_per_bit_end[j] != MAX_RX_DQSDLY_TAPS)
                {
                    //
                    dqs_delay_done[j] = 1;
                    //print("dqs_delay_per_bit%d[%d:%d], tap:%d error again\n", j, dqs_delay_per_bit_start[j], dqs_delay_per_bit_end[j], tap);
                }
            }
        }
    }

    #if 0
    print("======================================\n");

    for (i = 0; i < DQ_DATA_WIDTH; i++)
    {
        print("DQ%d fix 0, Tune DQS, [%d, %d]\n", i, dqs_delay_per_bit_start[i], dqs_delay_per_bit_end[i]);
    }
    #endif

    // todo: tmp code, dqs delay 16~23 swap with 24~31 according to Shengcheng advice
    {
    char tmp = 0;

    for (i = 16; i < 24; i++)
    {
        tmp = dqs_delay_per_bit_start[i];
        dqs_delay_per_bit_start[i] = dqs_delay_per_bit_start[i+8];
        dqs_delay_per_bit_start[i+8] = tmp;

        tmp = dqs_delay_per_bit_end[i];
        dqs_delay_per_bit_end[i] = dqs_delay_per_bit_end[i+8];
        dqs_delay_per_bit_end[i+8] = tmp;

        print("Swap %d<->%d done, new, %d:[%d, %d], %d:[%d, %d]\n",
            i, i+8,
            i, dqs_delay_per_bit_start[i], dqs_delay_per_bit_end[i],
            i+8, dqs_delay_per_bit_start[i+8], dqs_delay_per_bit_end[i+8]);
    }
    }

    print("======================================\n");

    for (i = 0; i < DQ_DATA_WIDTH; i++)
    {
        print("DQ%d fix 0, Tune DQS, [%d, %d]\n", i, dqs_delay_per_bit_start[i], dqs_delay_per_bit_end[i]);
    }    dqs_input_delay = 0;

    // Consider DQ delay and DQS delay
    for (i = 0; i < DQ_DATA_WIDTH; i++)
    {
        char left = 0;
        char right = 0;

        if ((0 != dq_delay_per_bit_start[i]) ||
            (0 != dqs_delay_per_bit_start[i]))
        {
            print("bit%d, DQ start:%d, DQS start:%d, error!!!\n", i, dq_delay_per_bit_start[i], dqs_delay_per_bit_start[i]);
            //todo:
        }

        left = dq_delay_per_bit_end[i] - dq_delay_per_bit_start[i] + 1;
        right = dqs_delay_per_bit_end[i] - dqs_delay_per_bit_start[i] + 1;

        if (right > left) //need give DQS delay
        {
            // Collect max difference in per byte
            if (diff < (right - left))
            {
                diff = right - left;
                print("Bit%d, new diff = %d\n", i, diff);
            }

            // Collect max window in per byte
            if (total < (right + left - 1))
            {
                total = right + left - 1;
            }
        }

        //Consider per byte
        if (0 == ((i + 1) % 8))
        {
            char offset = ((diff + 1) / 2);

            print("BYTE%d, offset = %d\n", i/8, offset);

            // DQS offset with (Max diff + 1)/2
            dqs_input_delay |= (offset << ((i + 1) - 8));

            // DQ
            for (j = (i + 1) - 8; j <= i; j++)
            {
                if (dq_delay_per_bit_end[j] >= dqs_delay_per_bit_end[j]) //Condition: Need ensure dq_delay_per_bit_start[i]==0 and dqs_delay_per_bit_start[i] ==0 !!!
                {
                    dq_delay_per_bit[j] = offset + (dq_delay_per_bit_end[j] - dqs_delay_per_bit_end[j]) / 2;
                }
                else
                {
                    dq_delay_per_bit[j] = offset - (dqs_delay_per_bit_end[j] - dq_delay_per_bit_end[j]) / 2;
                }

                #if 1 // Adjust
                if ((MAX_RX_DQDLY_TAPS-1) == dq_delay_per_bit_end[j])
                {
                    //dq_delay_per_bit[j] += 2;

                    //Condition: Need ensure dq_delay_per_bit_start[i]==0 and dqs_delay_per_bit_start[i] ==0 !!!
                    //(total - (dq_delay_per_bit_end[j] + dqs_delay_per_bit_end[j] + 1)) / 2; //Need adjust for unknown left margin -> need DQS delay

                    //print("DQ%d, cannot find DQ delay margin, adjust + %d\n", j, (total - (dq_delay_per_bit_end[j] + dqs_delay_per_bit_end[j] + 1)) / 2);
                }
                #endif
            }

            // Reset diff for next per byte processing later
            diff = 0;
            total = 0;
        }
    }

    DRAMC_WRITE_REG(dqs_input_delay,DRAMC_R0DELDLY);

    for (i = 0; i < DQ_DATA_WIDTH; i+=4)
    {
        dqidly[i/4] = (dq_delay_per_bit[i]) + (dq_delay_per_bit[i+1] << 8) + (dq_delay_per_bit[i+2] << 16) + (dq_delay_per_bit[i+3] << 24);

        //RX_PRINT("dqidly[%d]=0x%x\n",i/4,dqidly[i/4]);
    }

    // Notice: get 0/1/3/2 setting before after 1st swap, 2nd swap for write register
    #ifdef  DQ_DQS_DQM_REMAPPING
    for (i = 0; i < 8; i++)
    {
        DRAMC_WRITE_REG(dqidly[i], DRAMC_DQIDLY1+4*(2*DQSO_Mapping[i/2] + i%2));
    }
    #else
    for (i = 0; i < 8; i++)
    {
        DRAMC_WRITE_REG(dqidly[i],DRAMC_DQIDLY1+4*i);
    }
    #endif

    RX_PRINT("*DQIDLY1 = 0x%x\n", DRAMC_READ_REG(DRAMC_DQIDLY1));
    RX_PRINT("*DQIDLY2 = 0x%x\n", DRAMC_READ_REG(DRAMC_DQIDLY2));
    RX_PRINT("*DQIDLY3 = 0x%x\n", DRAMC_READ_REG(DRAMC_DQIDLY3));
    RX_PRINT("*DQIDLY4 = 0x%x\n", DRAMC_READ_REG(DRAMC_DQIDLY4));
    RX_PRINT("*DQIDLY5 = 0x%x\n", DRAMC_READ_REG(DRAMC_DQIDLY5));
    RX_PRINT("*DQIDLY6 = 0x%x\n", DRAMC_READ_REG(DRAMC_DQIDLY6));
    RX_PRINT("*DQIDLY7 = 0x%x\n", DRAMC_READ_REG(DRAMC_DQIDLY7));
    RX_PRINT("*DQIDLY8 = 0x%x\n", DRAMC_READ_REG(DRAMC_DQIDLY8));
    RX_PRINT("*DRAMC_R0DELDLY = 0x%x\n", DRAMC_READ_REG(DRAMC_R0DELDLY));


    #if 1 //todo: tmp code, give offset for LV
    for (i = 0; i < DQ_DATA_WIDTH; i += 4)
    {
        data = ((((DRAMC_READ_REG(DRAMC_DQIDLY1+i) >> 0) & 0xFF) + Perbit_DQ_Ofst[i]) << 0) |
            ((((DRAMC_READ_REG(DRAMC_DQIDLY1+i) >> 8) & 0xFF) + Perbit_DQ_Ofst[i+1]) << 8) |
            ((((DRAMC_READ_REG(DRAMC_DQIDLY1+i) >> 16) & 0xFF) + Perbit_DQ_Ofst[i+2]) << 16) |
            ((((DRAMC_READ_REG(DRAMC_DQIDLY1+i) >> 24) & 0xFF) + Perbit_DQ_Ofst[i+3]) << 24);

        DRAMC_WRITE_REG(data, DRAMC_DQIDLY1+i);
    }

    print("After offset:\n");
    print("*DQIDLY1 = 0x%x\n", DRAMC_READ_REG(DRAMC_DQIDLY1));
    print("*DQIDLY2 = 0x%x\n", DRAMC_READ_REG(DRAMC_DQIDLY2));
    print("*DQIDLY3 = 0x%x\n", DRAMC_READ_REG(DRAMC_DQIDLY3));
    print("*DQIDLY4 = 0x%x\n", DRAMC_READ_REG(DRAMC_DQIDLY4));
    print("*DQIDLY5 = 0x%x\n", DRAMC_READ_REG(DRAMC_DQIDLY5));
    print("*DQIDLY6 = 0x%x\n", DRAMC_READ_REG(DRAMC_DQIDLY6));
    print("*DQIDLY7 = 0x%x\n", DRAMC_READ_REG(DRAMC_DQIDLY7));
    print("*DQIDLY8 = 0x%x\n", DRAMC_READ_REG(DRAMC_DQIDLY8));
    print("*DRAMC_R0DELDLY = 0x%x\n", DRAMC_READ_REG(DRAMC_R0DELDLY));
    #endif
    }
    else
#endif
//#else
    {
    // delay DQ to find all failed
    for(tap = 0 ; tap < MAX_RX_DQDLY_TAPS; tap++ ){
        /* set test patern length*/

        DRAMC_WRITE_REG(0x55000000,0x3C);


        data = DRAMC_READ_REG(0x40);
        DRAMC_WRITE_REG((data & 0xAA000000) | test_len, 0x40);
        //Test Agent 2 write enabling, Test Agent 2 read enabling
        if(tap==0)
        {
            //RX_PRINT("do_sw_rx_dq_dqs_calib Test Agent 2 enable\n");
        }
        DRAMC_WRITE_SET((1 << 30) | (1 << 31),DRAMC_CONF2);

        while(!(DRAMC_READ_REG(DRAMC_TESTRPT)&(1 << 10)));

        delay_a_while(400);
        if(tap==0)
        {
            //RX_PRINT("do_sw_rx_dq_dqs_calib Test Agent 2 done\n");
        }

        cmp_err = DRAMC_READ_REG(DRAMC_CMP_ERR);
        RX_PRINT("cmp_err:%x\n",cmp_err);
        DRAMC_WRITE_CLEAR(((1 << 30) | (1 << 31)),DRAMC_CONF2); //disable test agent2 r/w
        if (cmp_err == 0xFFFFFFFF) break;


	/* Bit i compare result
	  * 	-Compare success & never fail before, record the delay value. (dq_delay_per_bit[i] = delay value)
	  *	-Compare fail. Record fail. (dq_delay_done[i] = 1)
           */

        for (i = 0; i < DQ_DATA_WIDTH; i++)
        {
            if (!(cmp_err&(0x1<<i)) && dq_delay_done[i] == 0)
            {
                dq_delay_per_bit[i] = tap;
            }
            else
            {
                dq_delay_done[i] = 1;
            }

            //RX_PRINT("%d)0x%x, done:%d\n",i,dq_delay_per_bit[i], dq_delay_done[i]);
        }
        //RX_PRINT("\n");

        for (i = 0; i < DQ_DATA_WIDTH; i+=4)
        {
            dqidly[i/4] = (dq_delay_per_bit[i]) + (dq_delay_per_bit[i+1] << 8) + (dq_delay_per_bit[i+2] << 16) + (dq_delay_per_bit[i+3] << 24);

            //RX_PRINT("dqidly[%d]=0x%x\n",i/4,dqidly[i/4]);
        }

        for (i = 0; i < 8; i++)
        {
            DRAMC_WRITE_REG(dqidly[i],DRAMC_DQIDLY1+4*i);
        }
        RX_PRINT("*DQIDLY1 = 0x%x\n", DRAMC_READ_REG(DRAMC_DQIDLY1));
        RX_PRINT("*DQIDLY2 = 0x%x\n", DRAMC_READ_REG(DRAMC_DQIDLY2));
        RX_PRINT("*DQIDLY3 = 0x%x\n", DRAMC_READ_REG(DRAMC_DQIDLY3));
        RX_PRINT("*DQIDLY4 = 0x%x\n", DRAMC_READ_REG(DRAMC_DQIDLY4));
        RX_PRINT("*DQIDLY5 = 0x%x\n", DRAMC_READ_REG(DRAMC_DQIDLY5));
        RX_PRINT("*DQIDLY6 = 0x%x\n", DRAMC_READ_REG(DRAMC_DQIDLY6));
        RX_PRINT("*DQIDLY7 = 0x%x\n", DRAMC_READ_REG(DRAMC_DQIDLY7));
        RX_PRINT("*DQIDLY8 = 0x%x\n", DRAMC_READ_REG(DRAMC_DQIDLY8));

    }

    // After loop, dq_delay_per_bit[0:31] value non-zero mean the last valid settings when DQS input delay is 0. dq_delay_per_bit[0:31] value 0 means it is already  failed when DQS input delay is 0. Also, current DQIDLY[1:8] settings is the setting of dq_delay_per_bit[0:31].
    // We got the dq input delay in dq_delay_per_bit[i]
    /* 2. initialize parameters */
    for (i = 0; i < DQ_DATA_WIDTH; i++)
    {
        dqs_perbit_dly[i].min_cur = -1;
        dqs_perbit_dly[i].max_cur = -1;
        dqs_perbit_dly[i].min_best = -1;
        dqs_perbit_dly[i].max_best = -1;
        dqs_perbit_dly[i].center = 0;
        dqs_perbit_dly[i].dq_dly_last = dq_delay_per_bit[i];
    }
    /* find the minimum and maximum DQS input delay*/
    for (i = 0; i < MAX_RX_DQSDLY_TAPS; i++)
    {
        dqs_input_delay = (i) + (i << 8) + (i << 16) + (i << 24);

        DRAMC_WRITE_REG(dqs_input_delay,DRAMC_R0DELDLY);


        /* set test patern length*/
        data = DRAMC_READ_REG(0x40);
        DRAMC_WRITE_REG((data & 0xFF000000) | test_len, 0x40);
        //Test Agent 2 write enabling, Test Agent 2 read enabling
        DRAMC_WRITE_SET((1 << 30) | (1 << 31),DRAMC_CONF2);

        while(!(DRAMC_READ_REG(DRAMC_TESTRPT)&(1 << 10)));

        delay_a_while(400);

        cmp_err = DRAMC_READ_REG(DRAMC_CMP_ERR);
        DRAMC_WRITE_CLEAR(((1 << 30) | (1 << 31)),DRAMC_CONF2); //disable test agent2 r/w


	/* if bit x test pass the first time, record to min input delay. (dqs_per_bit[x].min_cur = delay value.)
	  * If bit x already had min value and no max value and pass fail => max value is this delay-1. (dqs_per_bit[x].max_cur = delay value-1)
	  * If bit x already had min value and no max value and pass and delay value = 127 => max value = 127 (dqs_per_bit[x].max_cur = 127)
           */

        for (j = 0; j < DQ_DATA_WIDTH; j++)
        {
            if ((dqs_perbit_dly[j].min_cur == -1) && ((cmp_err&((U32)1<<j)) == 0x0))
            {
                // min pass delay
                dqs_perbit_dly[j].min_cur = i;
            }
            if ((dqs_perbit_dly[j].min_cur != -1) && (dqs_perbit_dly[j].max_cur == -1) && (((cmp_err&((U32)1<<j)) != 0x0) || (i == (MAX_RX_DQSDLY_TAPS-1))) )
            {
                // we get the dqs_perbit_dly pass max
                if ((i == (MAX_RX_DQSDLY_TAPS-1)) && ((cmp_err&((U32)1<<j)) == 0x0))
                {
                    dqs_perbit_dly[j].max_cur = MAX_RX_DQSDLY_TAPS-1;
                }
                else
                {
                    dqs_perbit_dly[j].max_cur = i - 1;
                }

                // there may be more than 1 pass range, find the max range
                // ex: x00xxxxxx00000000000000xx...(get the second one)
                if ((dqs_perbit_dly[j].max_cur-dqs_perbit_dly[j].min_cur) > (dqs_perbit_dly[j].max_best-dqs_perbit_dly[j].min_best))
                {
                    dqs_perbit_dly[j].max_best = dqs_perbit_dly[j].max_cur;
                    dqs_perbit_dly[j].min_best = dqs_perbit_dly[j].min_cur;
                }
                // clear to find the next pass range if it has
                dqs_perbit_dly[j].max_cur = -1;
                dqs_perbit_dly[j].min_cur = -1;
            }

        }
    }
    // 3
    // get dqs delay center per bit
    for (j = 0; j < DQ_DATA_WIDTH; j++)
    {
        if ((dqs_perbit_dly[j].max_best != -1) && (dqs_perbit_dly[j].min_best != -1))
        {
            dqs_perbit_dly[j].center = (dqs_perbit_dly[j].max_best + dqs_perbit_dly[j].min_best) / 2;
            //RX_PRINT("dqs_perbit_dly[%d].center=0x%x\n",j,dqs_perbit_dly[j].center);
        }
    }

    // we get the delay value of the 4 DQS (min of center)
    for (i = 0; i < DQS_NUMBER; i++)
    {
        max = 0;
        // find the max of center
        for (j = 0; j < DQS_BIT_NUMBER; j++)
        {
            if (dqs_perbit_dly[i*DQS_BIT_NUMBER+j].center > max)
            {
                max = dqs_perbit_dly[i*DQS_BIT_NUMBER+j].center;
            }
        }
        // save dqs delay
        dqs_delay[i] = max;
        //RX_PRINT("dqs_delay[%d]=0x%x\n",i,max);
    }

	// Notice: get 0/1/3/2 setting before after 1st swap, 2nd swap for write register
    #ifdef  DQ_DQS_DQM_REMAPPING
	data = ((U32) dqs_delay[0]) + (((U32)dqs_delay[1])<<8) + (((U32)dqs_delay[3])<<16) + (((U32)dqs_delay[2])<<24);
    #else
    data = ((U32) dqs_delay[0]) + (((U32)dqs_delay[1])<<8) + (((U32)dqs_delay[2])<<16) + (((U32)dqs_delay[3])<<24);
	#endif

    /*set dqs input delay*/

    DRAMC_WRITE_REG(data,DRAMC_R0DELDLY);

    // delay DQ ,let dqsdly_ok_center == DQS_delay
    for (i = 0; i < DQ_DATA_WIDTH; i = i+4)
    {
        // every 4-bit dq have the same delay register address
        // dq_dly_max: taps for dq delay to be add
        for (j = 0; j < 4; j++)
        {
            dq_dly_max =  dqs_delay[i/DQS_BIT_NUMBER] - dqs_perbit_dly[i+j].center;
            //RX_PRINT("1.bit:%d)dq_per_bit_dly:0x%x,dq_dly:0x%x\n",i+j,dqs_perbit_dly[i+j].dq_dly_last,dq_dly_max);
            data = dqs_perbit_dly[i+j].dq_dly_last + dq_dly_max;
            data = ((data > (MAX_RX_DQDLY_TAPS-1)) ? (MAX_RX_DQDLY_TAPS-1) : data);
            dqs_perbit_dly[i+j].dq_dly_last = data;

            //RX_PRINT("2.bit:%d)dq_per_bit_dly:0x%x\n",i+j,dqs_perbit_dly[i+j].dq_dly_last);
        }

        data = ((U32) dqs_perbit_dly[i].dq_dly_last) + (((U32)dqs_perbit_dly[i+1].dq_dly_last)<<8) + (((U32)dqs_perbit_dly[i+2].dq_dly_last)<<16) + (((U32)dqs_perbit_dly[i+3].dq_dly_last)<<24);

        DRAMC_WRITE_REG(data,DRAMC_DQIDLY1+i);
    }

    for (j = 0; j < DQ_DATA_WIDTH; j++)
    {
        //RX_PRINT("%d)min:0x%x,max:0x%x\n",j, dqs_perbit_dly[j].min_best, dqs_perbit_dly[j].max_best);
    }

    RX_PRINT("*DQIDLY1 = 0x%x\n", DRAMC_READ_REG(DRAMC_DQIDLY1));
    RX_PRINT("*DQIDLY2 = 0x%x\n", DRAMC_READ_REG(DRAMC_DQIDLY2));
    RX_PRINT("*DQIDLY3 = 0x%x\n", DRAMC_READ_REG(DRAMC_DQIDLY3));
    RX_PRINT("*DQIDLY4 = 0x%x\n", DRAMC_READ_REG(DRAMC_DQIDLY4));
    RX_PRINT("*DQIDLY5 = 0x%x\n", DRAMC_READ_REG(DRAMC_DQIDLY5));
    RX_PRINT("*DQIDLY6 = 0x%x\n", DRAMC_READ_REG(DRAMC_DQIDLY6));
    RX_PRINT("*DQIDLY7 = 0x%x\n", DRAMC_READ_REG(DRAMC_DQIDLY7));
    RX_PRINT("*DQIDLY8 = 0x%x\n", DRAMC_READ_REG(DRAMC_DQIDLY8));
    RX_PRINT("*DRAMC_R0DELDLY = 0x%x\n", DRAMC_READ_REG(DRAMC_R0DELDLY));

    //RX_PRINT("*DQIDLY1 = 0x%x\n", DRAMC_READ_REG(DRAMC_DQIDLY1));
    // finish we can put result now .
    RX_PRINT("==================================================================\n");
    RX_PRINT("		RX	DQS perbit delay software calibration \n");
    RX_PRINT("==================================================================\n");
    RX_PRINT("1.0-31 bit dq delay value\n");
    RX_PRINT("==================================================================\n");
    RX_PRINT("bit|     0  1  2  3  4  5  6  7  8  9\n");
    RX_PRINT("--------------------------------------");
    for (i = 0; i < DQ_DATA_WIDTH; i++)
    {
        j = i / 10;
        if (i == (j*10))
        {
            RX_PRINT("\n");
            RX_PRINT("%d |    ", i);
        }
        RX_PRINT("%d ", dq_delay_per_bit[i]);
    }
    RX_PRINT("\n--------------------------------------\n\n");
    RX_PRINT("==================================================================\n");
    RX_PRINT("2.dqs window\nx=pass dqs delay value (min~max)center \ny=0-7bit DQ of every group\n");
    RX_PRINT("input delay:DQS0 =%d DQS1 = %d DQS2 =%d DQS3 = %d\n", dqs_delay[0], dqs_delay[1], dqs_delay[2], dqs_delay[3]);
    RX_PRINT("==================================================================\n");
    RX_PRINT("bit	DQS0	 bit      DQS1     bit     DQS2     bit     DQS3\n");
    for (i = 0; i < DQS_BIT_NUMBER; i++)
    {
        RX_PRINT("%d  (%d~%d)%d  %d  (%d~%d)%d  %d  (%d~%d)%d  %d  (%d~%d)%d\n", \
            i,    dqs_perbit_dly[i].min_best, dqs_perbit_dly[i].max_best, dqs_perbit_dly[i].center, \
            i+8,  dqs_perbit_dly[i+8].min_best, dqs_perbit_dly[i+8].max_best, dqs_perbit_dly[i+8].center, \
            i+16, dqs_perbit_dly[i+16].min_best, dqs_perbit_dly[i+16].max_best, dqs_perbit_dly[i+16].center, \
            i+24, dqs_perbit_dly[i+24].min_best, dqs_perbit_dly[i+24].max_best, dqs_perbit_dly[i+24].center);
    }
    RX_PRINT("==================================================================\n");
    RX_PRINT("3.dq delay value last\n");
    RX_PRINT("==================================================================\n");
    RX_PRINT("bit|    0  1  2  3  4  5  6  7  8   9\n");
    RX_PRINT("--------------------------------------");
    for (i = 0; i < DQ_DATA_WIDTH; i++)
    {
        j = i / 10;
        if (i == (j*10))
        {
            RX_PRINT("\n");
            RX_PRINT("%d |    ", i);
        }
        RX_PRINT("%d ", dqs_perbit_dly[i].dq_dly_last);
    }
    RX_PRINT("\n==================================================================\n");
    }
//#endif


#endif

    return 0;

}

#if 1
//----------------------------------------------------------------------------------
typedef struct _TXDQS_PERBYTE_DLY_T
{
    S8 first_dqdly_pass;
    S8 last_dqdly_pass;
    S8 total_dqdly_pass;
    S8 first_dqsdly_pass;
    S8 last_dqsdly_pass;
    S8 total_dqsdly_pass;
    U8 best_dqdly;
    U8 best_dqsdly;
    U8 dq;
    U8 dqs;
} TXDQS_PERBYTE_DLY_T;
static void  calib_clk_output_dly_factor_handler(unsigned int value)
{
        unsigned int data;
        //adjust CLK output delay
        dbg_print("Change CLK output delay  = %d \n",value);
        data = DRAMC_READ_REG(DRAMC_PADCTL1);
        data = (data & 0xF0FFFFFF) | (value << 24);
        DRAMC_WRITE_REG(data,DRAMC_PADCTL1);
}

static void  calib_cmd_addr_output_dly_factor_handler(unsigned int value)
{
        unsigned int data;
        //adjust CMD delay
        dbg_print("Change CMD/ADDR output delay  = %d \n",value);
        data = DRAMC_READ_REG(DRAMC_CMDDLY0);
        data &= 0xE0E0E0E0;
        data |= (value  << 24) | (value << 16) | (value << 8) | value;
        DRAMC_WRITE_REG(data, DRAMC_CMDDLY0);

        data = DRAMC_READ_REG(DRAMC_CMDDLY1);
        data &= 0xE0E0E0E0;
        data |= (value  << 24) | (value << 16) | (value << 8) | value;
        DRAMC_WRITE_REG(data, DRAMC_CMDDLY1);

        data = DRAMC_READ_REG(DRAMC_CMDDLY2);
        data &= 0xE0E0E0E0;
        data |= (value  << 24) | (value << 16) | (value << 8) | value;
        DRAMC_WRITE_REG(data, DRAMC_CMDDLY2);

        data = DRAMC_READ_REG(DRAMC_CMDDLY3);
        data &= 0xE0E0E0E0;
        data |= (value  << 24) | (value << 16) | (value << 8) | value;
        DRAMC_WRITE_REG(data, DRAMC_CMDDLY3);

        data = DRAMC_READ_REG(DRAMC_CMDDLY4);
        data &= 0xE0E0E0E0;
        data |= (value  << 24) | (value << 16) | (value << 8) | value;
        DRAMC_WRITE_REG(data, DRAMC_CMDDLY4);

        data = DRAMC_READ_REG(DRAMC_CMDDLY5);
        data &= 0xE0E0E0EF;
        data |= (value << 24) | (value << 16) | (value << 8);
        DRAMC_WRITE_REG(data, DRAMC_CMDDLY5);

        data = DRAMC_READ_REG(DRAMC_DQSCAL0);
        data &= 0xF0F0FFFF;
        data |= (value << 24) | (value << 16);
        DRAMC_WRITE_REG(data, DRAMC_DQSCAL0);

}

unsigned int ta2_enable_write_read(unsigned int len)
{
    unsigned int data, cmp_err;

    /* set test patern length*/
    DRAMC_WRITE_REG(0x55000000,0x3C);
    data = DRAMC_READ_REG(0x40);
    DRAMC_WRITE_REG((data & 0xAA000000) | len, 0x40);
    //Test Agent 2 write enabling
    DRAMC_WRITE_SET((1 << 31),DRAMC_CONF2);
    //check write finish
    while(!(DRAMC_READ_REG(DRAMC_TESTRPT)&(1 << 10)));
    //Disable write
    DRAMC_WRITE_CLEAR((1 << 31),DRAMC_CONF2);

    //Test Agent 2 read enabling
    DRAMC_WRITE_SET((1 << 30),DRAMC_CONF2);
    //check read finish
    while(!(DRAMC_READ_REG(DRAMC_TESTRPT)&(1 << 10)));

    delay_a_while(400);

    cmp_err = DRAMC_READ_REG(DRAMC_CMP_ERR);
    //Disable read
    DRAMC_WRITE_CLEAR((1 << 30),DRAMC_CONF2); //disable test agent2 read

    return cmp_err;
}

#define TX_PERBIT_WIN
#ifndef CA_TRAINING
#define ENABLE_TX_CMD_CLK_SHIFT
#endif

#define MAX_TX_DQDLY_TAPS 16
#define MAX_TX_DQSDLY_TAPS 16

#ifdef WRITE_LEVELING
// If write leveling, TX_STEPS must be 0
#define TX_STEPS 0

unsigned int wrlevel_dqs_final_delay[DQS_NUMBER];
#else
 #if COMBO_PCDDR3 > 0
  #define TX_STEPS 6   // This value is obtained by measurement of waveform for the relationship between DQS and CLK.
 #else
  #define TX_STEPS 4   // This value is obtained by measurement of waveform for the relationship between DQS and CLK.
 #endif
#endif

/* Description
  *	TX DQ/DQS per byte calibration
  * Related registers
  *	- PADCTL3 : DQS[0:3]ODLY (0x10011014)
  *		- 4 bits (0~15)
  *		- 1 step 20ps.
  *	- PADCTL2 : DQM[0:3]DLY (0x10011010)
  *		- 4 bits (0~15)
  *		- 1 step 20ps.
  *	- DQODLY[1:4] : DQ[0:31]DLY (0x10011200~0x1001120c)
  *		- 4 bits (0~15)
  *		- 1 step 20ps
  *	- DRAMC_PADCTL1
  * 		- bit 27~bit 24 : clock delay.
  *	- DRAMC_CMDDLY[0:5], DRAMC_DQSCAL0
  *		- Address/cmd delay
  * Algorithm
  *	- Set DQ/DQM output delay to 0 and loop DQS output delay value from 0 to 15. Check the pass range.
  *	- Set DQS output delay value to 0 and loop DQ/DQM output delay value from 0 to 15. Check the pass range.
  *	- If two pass range equal, set both output delay to 8.
  *	- If not equal, add (delay difference)/2 to the one with larger pass range.
  */
unsigned int tx_flag;

int do_sw_tx_dq_dqs_calib(void)
{
    TXDQS_PERBYTE_DLY_T dqs_perbyte_dly[4];
#ifdef TX_PERBIT_WIN
    TXDQS_PERBYTE_DLY_T tx_perbit_dly[32];
    unsigned int uiBit;
    unsigned int ucmax_dqsdly_byte[DQS_NUMBER], ucdqmdly_byte[DQS_NUMBER];
#endif
    unsigned int data;
    unsigned int byte;
    int i,j;
    unsigned int mask;
    unsigned int test_len = 0xFFF; //0x3FF;
    unsigned int cmp_err_1,cmp_err_2;
    unsigned int dq,dqs;
    unsigned int finish_count, uiWinSize=0;
    int tx_tmp;
    int tx_steps;
#if ((MAX_TX_DQDLY_TAPS > 16) || (MAX_TX_DQSDLY_TAPS > 16))
    unsigned int delay_bit4;
#endif

#if defined(DQ_DQS_DQM_REMAPPING)
    unsigned int byte_remap;
    unsigned char *Bit_DQO_Mapping;

    if((DRAMC_READ_REG(0x00f0)>>31)==0x01)
    {//DM4BITMUX enable
        Bit_DQO_Mapping = (unsigned char *)Bit_DQO_Mapping_4bitmux;
    }
    else
    {
        Bit_DQO_Mapping = (unsigned char *)Bit_DQO_Mapping_no_4bitmux;
    }
#endif

    if (TYPE_LPDDR3 == mt_get_dram_type())
    	tx_steps = TX_STEPS;
    else
	tx_steps = 6;

    TX_PRINT("in do_sw_tx_dq_dqs_calib(), step:%d\n", tx_steps);
    for(i = 0; i < 4; i++)
    {
        //dqs_perbyte_dly[i].first_dqdly_pass = -1;
        dqs_perbyte_dly[i].first_dqdly_pass = 0;
        dqs_perbyte_dly[i].last_dqdly_pass = -1;
        //dqs_perbyte_dly[i].first_dqsdly_pass = -1;
        dqs_perbyte_dly[i].first_dqsdly_pass = 0;
        dqs_perbyte_dly[i].last_dqsdly_pass = -1;
        dqs_perbyte_dly[i].best_dqdly= -1;
        dqs_perbyte_dly[i].best_dqsdly= -1;
        dqs_perbyte_dly[i].dq= 0;
        dqs_perbyte_dly[i].dqs= 0;
        dqs_perbyte_dly[i].total_dqdly_pass= 0;
        dqs_perbyte_dly[i].total_dqsdly_pass= 0;
    }

#ifdef TX_PERBIT_WIN
    for (i=0; i<32; i++) {
        tx_perbit_dly[i].first_dqdly_pass = 0;
        tx_perbit_dly[i].last_dqdly_pass = -1;
        tx_perbit_dly[i].first_dqsdly_pass = 0;
        tx_perbit_dly[i].last_dqsdly_pass = -1;
        tx_perbit_dly[i].total_dqdly_pass= 0;
        tx_perbit_dly[i].total_dqsdly_pass= 0;
     }
#endif

    // used for early break
    finish_count = 0;

    /* 1. Tx DQM/DQ all zero*/
    /* 1.1 DQM*/
    //DRAMC_WRITE_CLEAR(0x0000FFFF,DRAMC_PADCTL3);
    DRAMC_WRITE_REG(0x0,DRAMC_PADCTL2);
#if MAX_TX_DQDLY_TAPS > 16
    DRAMC_WRITE_CLEAR(0x0F, 0x238);
#endif
    /* 1.2 DQ*/
    for (i = 0 ; i < 4; i ++)
    {
        //dbg_print("addr:%x\n",DRAMC_DQODLY1+4*i);
        DRAMC_WRITE_REG(0x0, (DRAMC_DQODLY1+4*i));

    }
#if MAX_TX_DQDLY_TAPS > 16
    DRAMC_WRITE_REG(0x0, 0x234);
#endif

    //dqs_perbyte_dly[byte].first_dqsdly_pass = 0;
    //dbg_print("dqs_perbyte_dly.first_dqsdly_pass=%x \n",dqs_perbyte_dly[byte].first_dqsdly_pass);
    //dqs_perbyte_dly[byte].last_dqsdly_pass =  MAX_TX_DQSDLY_TAPS - 1;
    /* 2. fix DQ delay = 0, delay DQS to find the pass range  */
    for (i = MAX_TX_DQSDLY_TAPS-1; i >= 0 ; i--)
    {
        data = DRAMC_READ_REG(DRAMC_PADCTL3);
        mask = (0xFFFF0000);
    #if MAX_TX_DQSDLY_TAPS > 16
        tx_tmp = i & 0x0F;
        data = (data & mask) | ((tx_tmp <<0) | (tx_tmp<<4) | (tx_tmp <<8) | (tx_tmp <<12));
        DRAMC_WRITE_REG(data,DRAMC_PADCTL3);

        delay_bit4= DRAMC_READ_REG(0x238);
        mask = 0xFFFFFF0F;
        tx_tmp = i>>4;
        delay_bit4 = (delay_bit4 & mask) | ((tx_tmp <<4) | (tx_tmp<<5) | (tx_tmp <<6) | (tx_tmp <<7));
        DRAMC_WRITE_REG(delay_bit4, 0x238);
    #else
        data = (data & mask) | ((i <<0) | (i<<4) | (i <<8) | (i <<12));
        //dbg_print("mask:%x, data:%x\n",mask,data);
        DRAMC_WRITE_REG(data,DRAMC_PADCTL3);
    #endif

#ifdef ENABLE_TX_CMD_CLK_SHIFT
        if(mt_get_dram_type() != TYPE_PCDDR3)
        {//PCDDR3 no need
            //adjust CMD addr output delay
            calib_cmd_addr_output_dly_factor_handler(i);
            //adjust CLK delay
            calib_clk_output_dly_factor_handler(i);
            //gating window calibration
        #ifdef GW_TA2
                do_dqs_gw_calib_2();
        #else
                do_dqs_gw_calib_1();
        #endif
        }
#endif
        /* 2.2 use test agent to find the pass range */
        cmp_err_1 = ta2_enable_write_read(test_len);
        TX_PRINT("DQS loop = %d, cmp_err_1 = %x \n",i, cmp_err_1);

        for (byte=0; byte < 4; byte++)
        {
            if (dqs_perbyte_dly[byte].last_dqsdly_pass == -1)
            {
                if (!((cmp_err_1) & (0xFF<<(byte*8))))
                {
                    dqs_perbyte_dly[byte].last_dqsdly_pass = i;
                    finish_count++;
                    //print("dqs_perbyte_dly.last_dqsdly_pass[%d]=%d,  finish count=%d \n",byte, dqs_perbyte_dly[byte].last_dqsdly_pass, finish_count);
                    //break;
                }
            }

#ifdef TX_PERBIT_WIN
          for (uiBit=0; uiBit<8; uiBit++)
          {
              if (tx_perbit_dly[(byte<<3)+uiBit].last_dqsdly_pass == -1)
              {
                  if (!((cmp_err_1) & (0x01<<((byte<<3)+uiBit))))
                  {
                      tx_perbit_dly[(byte<<3)+uiBit].last_dqsdly_pass = i;
                  }
              }
          }
#endif
        }

#ifndef TX_PERBIT_WIN
        if (finish_count==4)
        {
            //ask JC, why finish count = 4
            break;
        }
#endif
    }

    /* 3. fix DQS delay = 0, delay DQ to find the pass range  */
    DRAMC_WRITE_CLEAR(0xFFFF ,DRAMC_PADCTL3);
#if MAX_TX_DQSDLY_TAPS > 16
    DRAMC_WRITE_CLEAR(0x000000F0 ,0x238);
#endif

#ifdef ENABLE_TX_CMD_CLK_SHIFT
    if(mt_get_dram_type() != TYPE_PCDDR3)
    {//PCDDR3 no need
        //adjust CMD addr output delay
        calib_cmd_addr_output_dly_factor_handler(0);
        //adjust CLK delay
        calib_clk_output_dly_factor_handler(0);
        //gating window calibration
    #ifdef GW_TA2
            do_dqs_gw_calib_2();
    #else
            do_dqs_gw_calib_1();
    #endif
    }
#endif

        //dqs_perbyte_dly[byte].first_dqdly_pass = 0;
        //dbg_print("dqs_perbyte_dly.first_dqdly_pass=%x \n",dqs_perbyte_dly[byte].first_dqdly_pass);

    finish_count = 0;

    for (i = MAX_TX_DQDLY_TAPS-1; i >= 0; i--)
    {
        /* 3.1 delay DQ output delay */
        data =
            ((i & 0xF) << 0)       /* DQM0DLY: DRAMC_PADCTL2[0:3],         4 bits */
          | ((i & 0xF) << 4)       /* DQM1DLY: DRAMC_PADCTL2[4:7], 4 bits */
          | ((i & 0xF) << 8)       /* DQM2DLY: DRAMC_PADCTL2[8:11],        4 bits */
          | ((i & 0xF) << 12);
        DRAMC_WRITE_REG(data , DRAMC_PADCTL2/* 0x10 */);
        data =
            ((i & 0xF) << 0)             /* DQ0DLY: DRAMC_DQODLY1[0:3],          4 bits */
          | ((i & 0xF) << 4)               /* DQ1DLY: DRAMC_DQODLY1[4:7],          4 bits */
          | ((i & 0xF) << 8)               /* DQ2DLY: DRAMC_DQODLY1[8:11],         4 bits */
          | ((i & 0xF) << 12)      /* DQ3DLY: DRAMC_DQODLY1[12:15],        4 bits */
          | ((i & 0xF) << 16)      /* DQ4DLY: DRAMC_DQODLY1[16:19],        4 bits */
          | ((i & 0xF) << 20)      /* DQ5DLY: DRAMC_DQODLY1[20:23],        4 bits */
          | ((i & 0xF) << 24)      /* DQ6DLY: DRAMC_DQODLY1[24:27],        4 bits */
          | ((i & 0xF) << 28);
        DRAMC_WRITE_REG(data,DRAMC_DQODLY1);
        DRAMC_WRITE_REG(data,DRAMC_DQODLY2);
        DRAMC_WRITE_REG(data,DRAMC_DQODLY3);
        DRAMC_WRITE_REG(data,DRAMC_DQODLY4);
#if MAX_TX_DQDLY_TAPS > 16
        if(i>0x0f)
            delay_bit4 = 0xFFFFFFFF;
        else
            delay_bit4 = 0x0;

        DRAMC_WRITE_REG(delay_bit4, 0x234);
#endif
        //dbg_print("%x = %x\n",DRAMC_DQODLY1+(4*byte),DRAMC_READ_REG(DRAMC_DQODLY1+(4*byte)));
        /* 3.2 use test agent to find the pass range */
        cmp_err_1 = ta2_enable_write_read(test_len);
        TX_PRINT("DQ loop=%d, cmp_err_1 = %x\n",i, cmp_err_1);

        for (byte=0; byte < 4; byte++)
        {
            if (dqs_perbyte_dly[byte].last_dqdly_pass == -1)
            {
                if (!(cmp_err_1 &(0xFF<<(byte*8))))
                {
                    dqs_perbyte_dly[byte].last_dqdly_pass = i;
                    finish_count++;
                    //print("dqs_perbyte_dly.last_dqdly_pass[%d]=%d,  finish count=%d \n",byte, dqs_perbyte_dly[byte].last_dqdly_pass, finish_count);
                    //break;
                }
            }

#ifdef TX_PERBIT_WIN
          for (uiBit=0; uiBit<8; uiBit++)
          {
              if (tx_perbit_dly[(byte<<3)+uiBit].last_dqdly_pass == -1)
              {
                  if (!((cmp_err_1) & (0x01<<((byte<<3)+uiBit))))
                  {
                      tx_perbit_dly[(byte<<3)+uiBit].last_dqdly_pass = i;
                  }
              }
          }
#endif
        }

#ifndef TX_PERBIT_WIN
        if (finish_count==4)
        {
            break;
        }
#endif
    }

    for (byte=0; byte < 4; byte++)
    {
        dqs_perbyte_dly[byte].total_dqsdly_pass = dqs_perbyte_dly[byte].last_dqsdly_pass - dqs_perbyte_dly[byte].first_dqsdly_pass + 1;
        //dbg_print("total_dqsdly_pass:%x\n", dqs_perbyte_dly[byte].total_dqsdly_pass);
        dqs_perbyte_dly[byte].total_dqdly_pass = dqs_perbyte_dly[byte].last_dqdly_pass - dqs_perbyte_dly[byte].first_dqdly_pass + 1;
        //dbg_print("total_dqdly_pass:%x\n", dqs_perbyte_dly[byte].total_dqdly_pass);
        /* 4. find the middle of the pass range of DQ and DQS*/
        /* 5. if the middle of the pass range is in the DQ, the delay of (DQS,DQ) is (0,DQ),
         *    if the middle of the pass range is in the DQS the delay of (DQS,DQ) is (DQS,0)*/
        if (dqs_perbyte_dly[byte].total_dqdly_pass == dqs_perbyte_dly[byte].total_dqsdly_pass)
        {
        //#ifdef WRITE_LEVELING
        //    dqs_perbyte_dly[byte].dqs = dqs_perbyte_dly[byte].dq = wrlevel_dqs_final_delay[byte];
        //#else
            dqs_perbyte_dly[byte].dqs = dqs_perbyte_dly[byte].dq = tx_steps;
        //#endif
        }
        else if (dqs_perbyte_dly[byte].total_dqdly_pass > dqs_perbyte_dly[byte].total_dqsdly_pass)
        {
        //#ifdef WRITE_LEVELING
        //    dqs_perbyte_dly[byte].dqs = 0 + wrlevel_dqs_final_delay[byte];
        //    dqs_perbyte_dly[byte].dq += (dqs_perbyte_dly[byte].total_dqdly_pass - dqs_perbyte_dly[byte].total_dqsdly_pass)/2 + wrlevel_dqs_final_delay[byte];
        //#else
            dqs_perbyte_dly[byte].dqs = 0 + tx_steps;
            dqs_perbyte_dly[byte].dq += (dqs_perbyte_dly[byte].total_dqdly_pass - dqs_perbyte_dly[byte].total_dqsdly_pass)/2 + tx_steps;
        //#endif
            if (dqs_perbyte_dly[byte].dq > (MAX_TX_DQDLY_TAPS-1))
            {
                dqs_perbyte_dly[byte].dq = MAX_TX_DQDLY_TAPS-1;
                TX_PRINT("warning:byte %d,dq:%x",byte,dqs_perbyte_dly[byte].dq);
            }
        }
        else
        {
    #if 0
    #ifdef WRITE_LEVELING
            dqs_perbyte_dly[byte].dq = 0;
            dqs_perbyte_dly[byte].dqs += (dqs_perbyte_dly[byte].total_dqsdly_pass - dqs_perbyte_dly[byte].total_dqdly_pass)/2;
            if (dqs_perbyte_dly[byte].dqs <= wrlevel_dqs_final_delay[byte])
            {
                dqs_perbyte_dly[byte].dq = 0 + (wrlevel_dqs_final_delay[byte] - dqs_perbyte_dly[byte].dqs);
                dqs_perbyte_dly[byte].dqs = wrlevel_dqs_final_delay[byte];
            }
            else
            {
                // dqs_perbyte_dly[byte].dqs > wrlevel_dqs_final_delay[byte])
                // Originally should move clk delay and CA delay accordingly. Then GW calibration again. Too complicated.
                // DQ/DQS skew should not be large according to DE. So sacrifice the Clk/DQS margin by keeping the clk out delay.
                dbg_print("[Warning] DQSO %d in TX per-bit = %d > DQSO %d in WL = %d  \n",
                    byte, dqs_perbyte_dly[byte].dqs, byte, wrlevel_dqs_final_delay[byte]);
            }
    #else
            dqs_perbyte_dly[byte].dq = 0 + TX_STEPS;
            dqs_perbyte_dly[byte].dqs += (dqs_perbyte_dly[byte].total_dqsdly_pass - dqs_perbyte_dly[byte].total_dqdly_pass)/2 + TX_STEPS;
    #endif
    #endif
            dqs_perbyte_dly[byte].dqs = (dqs_perbyte_dly[byte].total_dqsdly_pass - dqs_perbyte_dly[byte].total_dqdly_pass)/2;
            if(dqs_perbyte_dly[byte].dqs > tx_steps)
            {
                dqs_perbyte_dly[byte].dq = 0;
            }
            else
            {
               dqs_perbyte_dly[byte].dq = tx_steps - dqs_perbyte_dly[byte].dqs;
               dqs_perbyte_dly[byte].dqs = tx_steps;
            }
       /*
            dqs_perbyte_dly[byte].dq = 0 + tx_steps;
            dqs_perbyte_dly[byte].dqs += (dqs_perbyte_dly[byte].total_dqsdly_pass - dqs_perbyte_dly[byte].total_dqdly_pass)/2 + tx_steps;

            if (dqs_perbyte_dly[byte].dqs > (MAX_TX_DQSDLY_TAPS-1))
            {
                dqs_perbyte_dly[byte].dqs = MAX_TX_DQSDLY_TAPS-1;
                dbg_print("warning:byte %d,dqs:%x",byte,dqs_perbyte_dly[byte].dqs);
            }
            */
        }
        TX_PRINT("byte:%x, (DQS,DQ)=(%x,%x)\n",byte,dqs_perbyte_dly[byte].dqs, dqs_perbyte_dly[byte].dq);
        /* 6. fix the (DQS,DQ) for this byte, find the next byte */
        uiWinSize += (dqs_perbyte_dly[byte].total_dqdly_pass+dqs_perbyte_dly[byte].total_dqsdly_pass-1);
    }
    uiWinSize = uiWinSize/4;
    TX_PRINT("===== Average TX window size=%d\n",uiWinSize);

#ifdef TX_PERBIT_WIN
    TX_PRINT("Bit : TX total window size   DQ window size  DQS window size\n");
    TX_PRINT("============================================================================\n");
    for (byte=0; byte<4; byte++)
    {
        ucmax_dqsdly_byte[byte] = ucdqmdly_byte[byte] = 0;
    }
    for (uiBit=0; uiBit<32; uiBit++)
    {
        tx_perbit_dly[uiBit].total_dqdly_pass = tx_perbit_dly[uiBit].last_dqdly_pass - tx_perbit_dly[uiBit].first_dqdly_pass + 1;
        tx_perbit_dly[uiBit].total_dqsdly_pass = tx_perbit_dly[uiBit].last_dqsdly_pass - tx_perbit_dly[uiBit].first_dqsdly_pass + 1;
        TX_PRINT("%d : Total=%d     DQ=%d  DQS=%d",
                uiBit,
                tx_perbit_dly[uiBit].total_dqdly_pass+ tx_perbit_dly[uiBit].total_dqsdly_pass-1,
                 tx_perbit_dly[uiBit].total_dqdly_pass,
                 tx_perbit_dly[uiBit].total_dqsdly_pass);
        if (tx_perbit_dly[uiBit].total_dqsdly_pass > tx_perbit_dly[uiBit].total_dqdly_pass)
        {
             tx_perbit_dly[uiBit].best_dqsdly = (tx_perbit_dly[uiBit].total_dqsdly_pass-tx_perbit_dly[uiBit].total_dqdly_pass)/2;
             if(tx_perbit_dly[uiBit].best_dqsdly > tx_steps)
             {
                 tx_perbit_dly[uiBit].best_dqdly = 0;
             }
             else
             {
                tx_perbit_dly[uiBit].best_dqdly = tx_steps - tx_perbit_dly[uiBit].best_dqsdly;
                tx_perbit_dly[uiBit].best_dqsdly = tx_steps;
             }
             //tx_perbit_dly[uiBit].best_dqdly = 0 + tx_steps;
             //tx_perbit_dly[uiBit].best_dqsdly = (tx_perbit_dly[uiBit].total_dqsdly_pass-tx_perbit_dly[uiBit].total_dqdly_pass)/2+tx_steps;
        }
        else if (tx_perbit_dly[uiBit].total_dqdly_pass > tx_perbit_dly[uiBit].total_dqsdly_pass)
        {
            tx_perbit_dly[uiBit].best_dqsdly = 0 + tx_steps;
            tx_perbit_dly[uiBit].best_dqdly = (tx_perbit_dly[uiBit].total_dqdly_pass - tx_perbit_dly[uiBit].total_dqsdly_pass)/2 + tx_steps;
        }
        else
        {
            tx_perbit_dly[uiBit].best_dqsdly = 0 + tx_steps;
            tx_perbit_dly[uiBit].best_dqdly = 0 + tx_steps;
        }

        if (tx_perbit_dly[uiBit].best_dqsdly > ucmax_dqsdly_byte[uiBit>>3])
        {
            ucmax_dqsdly_byte[uiBit>>3] = tx_perbit_dly[uiBit].best_dqsdly;
        }
//For Denali pass criterion start
        tx_tmp = tx_perbit_dly[uiBit].total_dqdly_pass+ tx_perbit_dly[uiBit].total_dqsdly_pass-1;
        if (tx_tmp <= 0)
        {
            tx_flag = 1;
        }
//For Denali pass criterion end
        TX_PRINT("   Best   DQ=%d  DQS=%d\n", tx_perbit_dly[uiBit].best_dqdly, tx_perbit_dly[uiBit].best_dqsdly);

    }

    for (byte=0; byte<4 ; byte++)
    {
        TX_PRINT("DQS %d Final Delay=%d (%xh)\n",byte,ucmax_dqsdly_byte[byte], ucmax_dqsdly_byte[byte]);
    }

    for (uiBit=0; uiBit<32 ; uiBit++) {
        if (tx_perbit_dly[uiBit].best_dqsdly < ucmax_dqsdly_byte[uiBit>>3]) {
            tx_perbit_dly[uiBit].best_dqdly += (ucmax_dqsdly_byte[uiBit>>3] - tx_perbit_dly[uiBit].best_dqsdly);
            if (tx_perbit_dly[uiBit].best_dqdly >= (MAX_TX_DQDLY_TAPS-1)) {
                tx_perbit_dly[uiBit].best_dqdly = MAX_TX_DQDLY_TAPS-1;
            }
        }
        TX_PRINT("DQ %d Final Delay=%d (%xh)\n",uiBit,tx_perbit_dly[uiBit].best_dqdly,tx_perbit_dly[uiBit].best_dqdly);
        ucdqmdly_byte[uiBit>>3] += tx_perbit_dly[uiBit].best_dqdly;
    }
    for (byte=0; byte<4 ; byte++) {
        ucdqmdly_byte[byte] = ucdqmdly_byte[byte] /8;
        TX_PRINT("DQM %d Final Delay=%d(%xh)\n",byte,ucdqmdly_byte[byte], ucdqmdly_byte[byte]);
    }

    if (TYPE_LPDDR3 == mt_get_dram_type())
    {
#ifdef WRITE_LEVELING
        for (byte=0; byte<4 ; byte++)
        {
#ifdef DQ_DQS_DQM_REMAPPING
            byte_remap = DQSO_Mapping[byte];
#else
            byte_remap = byte;
#endif
        if (ucmax_dqsdly_byte[byte] <= wrlevel_dqs_final_delay[byte_remap])
        {
            TX_PRINT("DQSO %d in TX per-bit = %d <= DQSO %d in WL = %d  \n",
                byte, ucmax_dqsdly_byte[byte], byte_remap, wrlevel_dqs_final_delay[byte_remap]);

                // DQ
           	//#ifdef DQ_DQS_DQM_REMAPPING
        	//    for (uiBit=Bit_DQO_Mapping[8*byte]; uiBit<=Bit_DQO_Mapping[8*byte+7]; uiBit++)
        	//#else
            	for (uiBit=8*byte; uiBit<=8*byte+7; uiBit++)
        	//#endif
            	{
                	tx_perbit_dly[uiBit].best_dqdly += (wrlevel_dqs_final_delay[byte_remap] - ucmax_dqsdly_byte[byte]);
            	}
            	// DQM
            	ucdqmdly_byte[byte] += (wrlevel_dqs_final_delay[byte_remap] - ucmax_dqsdly_byte[byte]);

            // DQS
            ucmax_dqsdly_byte[byte] = wrlevel_dqs_final_delay[byte_remap];
        }
        else
        {
            // ucmax_dqsdly_byte[byte] > wrlevel_dqs_final_delay[byte])
            // Originally should move clk delay and CA delay accordingly. Then GW calibration again. Too complicated.
            // DQ/DQS skew should not be large according to DE. So sacrifice the Clk/DQS margin by keeping the clk out delay.
            TX_PRINT("[Warning] DQSO %d in TX per-bit = %d > DQSO %d in WL = %d  \n",
                byte, ucmax_dqsdly_byte[byte], byte_remap, wrlevel_dqs_final_delay[byte_remap]);
        }
    }

        for (byte = 0; byte < 4; byte++)
        {
        	TX_PRINT("------------------------------------\n");
        	TX_PRINT("DQS %d Final Delay (after WL)=%d (%xh)\n",byte,ucmax_dqsdly_byte[byte], ucmax_dqsdly_byte[byte]);
        	for (uiBit=8*byte; uiBit<=8*byte+7; uiBit++)
           		TX_PRINT("DQ %d Final Delay (after WL)=%d (%xh)\n",uiBit,tx_perbit_dly[uiBit].best_dqdly,tx_perbit_dly[uiBit].best_dqdly);
        	TX_PRINT("DQM %d Final Delay (after WL)=%d(%xh)\n",byte,ucdqmdly_byte[byte], ucdqmdly_byte[byte]);
        	TX_PRINT("------------------------------------\n");
    	}
#endif    // WRITE_LEVELING
    }

#endif    // TX_PERBIT_WIN

#ifdef DQ_DQS_DQM_REMAPPING
    data = 0;
    for (byte = 0 ; byte < 4; byte ++)
    {
#if MAX_TX_DQDLY_TAPS > 16
        delay_bit4 = DRAMC_READ_REG(0x238) & (~0xF);
 	#ifdef TX_PERBIT_WIN
        data |= ucdqmdly_byte[byte] << (DQM_Mapping[byte]*4);
        if(ucdqmdly_byte[byte]>0x0F)
            delay_bit4 |= 0x1 << (DQM_Mapping[byte]*4);

	#else

        data |= (dqs_perbyte_dly[byte].dq << (DQM_Mapping[byte]*4));
        if(dqs_perbyte_dly[byte].dq > 0x0F)
            delay_bit4 |= 0x1 << (DQM_Mapping[byte]*4);
	#endif
#else
    #ifdef TX_PERBIT_WIN
        data |= ucdqmdly_byte[byte] << (DQM_Mapping[byte]*4);
    #else
        data |= (dqs_perbyte_dly[byte].dq << (DQM_Mapping[byte]*4));
    #endif
#endif
    }

    DRAMC_WRITE_REG(data , DRAMC_PADCTL2/* 0x10 */);
    opt_tx_dqm = data;
    TX_PRINT("Tx DQM dly  = 0x%x\n", opt_tx_dqm);
#if MAX_TX_DQDLY_TAPS > 16
    DRAMC_WRITE_REG(delay_bit4 , 0x238);
    TX_PRINT("Tx DQM dly bit4 = 0x%x\n", delay_bit4 & 0x0F);
#endif

    for (byte = 0 ; byte < 4; byte ++)
    {
        opt_tx_dq[byte] = 0;
    }

#ifdef TX_PERBIT_WIN
#if MAX_TX_DQDLY_TAPS > 16
    delay_bit4 = 0;
#endif

    for (uiBit=0; uiBit<32; uiBit++) {
        unsigned int uiRemapDQ;
        uiRemapDQ = Bit_DQO_Mapping[uiBit];
        opt_tx_dq[uiRemapDQ>>3] |= (tx_perbit_dly[uiBit].best_dqdly & 0x0F) << ((uiRemapDQ & 0x07)<<2);
#if MAX_TX_DQDLY_TAPS > 16
        if(tx_perbit_dly[uiBit].best_dqdly > 0x0F)
            delay_bit4 |= 0x01 << uiRemapDQ;
#endif

    }
#else
    for (byte = 0 ; byte < 4; byte ++)
    {
         unsigned int uiRemapDQ;
         dq = dqs_perbyte_dly[byte].dq;
        for (i=0; i<8; i++) {
            uiRemapDQ = Bit_DQO_Mapping[(byte<<3)+i];
            opt_tx_dq[uiRemapDQ>>3] |= ((dq & 0xF) << ((uiRemapDQ & 0x07)<<2));
        }
    }
#endif

    for (byte = 0 ; byte < 4; byte ++)
    {
         DRAMC_WRITE_REG(opt_tx_dq[byte], DRAMC_DQODLY1+(4*byte));
         TX_PRINT("DRAMC_DQODLY%d=%xh\n", byte+1, opt_tx_dq[byte]);
    }
#if MAX_TX_DQDLY_TAPS > 16
    DRAMC_WRITE_REG(delay_bit4, 0x234);
    TX_PRINT("Tx DQ dly bit4 = 0x%x\n", delay_bit4);
#endif

    data = 0;
#if MAX_TX_DQDLY_TAPS > 16
    delay_bit4 = DRAMC_READ_REG(0x238) & (~0xf0);
#endif

	if (TYPE_LPDDR3 == mt_get_dram_type())
    {
        for (byte = 0; byte < 4; byte++)
        {
            ucmax_dqsdly_byte[byte] = ucmax_dqsdly_byte[byte] + 0x0002;
            if (ucmax_dqsdly_byte[byte] > 0x000f)
                ucmax_dqsdly_byte[byte] = 0x000f;
        }
    }
    print("tune dqs delay + 2\n");

    for (byte = 0 ; byte < 4; byte++)
    {
#if MAX_TX_DQDLY_TAPS > 16
#ifdef TX_PERBIT_WIN
    data |= ((ucmax_dqsdly_byte[byte] & 0x0F) << (DQSO_Mapping[byte]*4));
    if(ucmax_dqsdly_byte[byte] > 0x0f)
        delay_bit4 |= 0x01 <<  (DQSO_Mapping[byte] + 4);
#else
    data |= ((dqs_perbyte_dly[byte].dqs & 0x0F) << (DQSO_Mapping[byte]*4));
    if(dqs_perbyte_dly[byte].dqs > 0x0f)
        delay_bit4 |= 0x01 <<  (DQSO_Mapping[byte] + 4);
#endif
#else
#ifdef TX_PERBIT_WIN
        data |= (ucmax_dqsdly_byte[byte] << (DQSO_Mapping[byte]*4));
#else
        data |= (dqs_perbyte_dly[byte].dqs << (DQSO_Mapping[byte]*4));
#endif
#endif
    }

    DRAMC_WRITE_REG(data , DRAMC_PADCTL3/* 0x14 */);
    opt_tx_dqs = data;
    TX_PRINT("Tx DQS dly = 0x%x\n", data);

#if MAX_TX_DQDLY_TAPS > 16
    DRAMC_WRITE_REG(delay_bit4 , 0x238);
    TX_PRINT("Tx DQS dly bit4 = 0x%x\n", (delay_bit4 >> 4) & 0x0F);
#endif

#if 1 // tmp code
	TX_PRINT("Tx DQM adjust: 0x%x -> DQS+1\n", opt_tx_dqm);

	data = ((((opt_tx_dqs >> 0) & 0xF) + 1) << 0) |
		((((opt_tx_dqs >> 4) & 0xF) + 1) << 4) |
		((((opt_tx_dqs >> 8) & 0xF) + 1) << 8) |
		((((opt_tx_dqs >> 12) & 0xF) + 1) << 12);

	DRAMC_WRITE_REG(data , DRAMC_PADCTL2/* 0x10 */);
	opt_tx_dqm = data;
	TX_PRINT("Tx DQM dly  = 0x%x after refer to DQS\n", opt_tx_dqm);
#endif

#else /*no DQ DQS remapping*/
#ifdef TX_PERBIT_WIN
    data =
        ((ucdqmdly_byte[0] & 0xF) << 0)       /* DQM0DLY: DRAMC_PADCTL2[0:3], 4 bits */
      | ((ucdqmdly_byte[1] & 0xF) << 4)       /* DQM1DLY: DRAMC_PADCTL2[4:7], 4 bits */
      | ((ucdqmdly_byte[2] & 0xF) << 8)       /* DQM2DLY: DRAMC_PADCTL2[8:11], 4 bits */
      | ((ucdqmdly_byte[3] & 0xF) << 12);     /*  DQM3DLY: DRAMC_PADCTL2[12:15], 4 bits */
#else
    data =
        ((dqs_perbyte_dly[0].dq & 0xF) << 0)       /* DQM0DLY: DRAMC_PADCTL2[0:3], 4 bits */
      | ((dqs_perbyte_dly[1].dq & 0xF) << 4)       /* DQM1DLY: DRAMC_PADCTL2[4:7], 4 bits */
      | ((dqs_perbyte_dly[2].dq & 0xF) << 8)       /* DQM2DLY: DRAMC_PADCTL2[8:11], 4 bits */
      | ((dqs_perbyte_dly[3].dq & 0xF) << 12);     /*  DQM3DLY: DRAMC_PADCTL2[12:15], 4 bits */
#endif

    DRAMC_WRITE_REG(data , DRAMC_PADCTL2/* 0x10 */);
    opt_tx_dqm = data;

    for (byte = 0 ; byte < 4; byte ++)
    {
    #ifdef TX_PERBIT_WIN
            uiBit = 8*byte;
            data =
                ((tx_perbit_dly[uiBit+0].best_dqdly & 0xF) << 0)        /* DQ0DLY: DRAMC_DQODLY1[0:3],          4 bits */
              | ((tx_perbit_dly[uiBit+1].best_dqdly & 0xF) << 4)       /* DQ1DLY: DRAMC_DQODLY1[4:7],          4 bits */
              | ((tx_perbit_dly[uiBit+2].best_dqdly & 0xF) << 8)       /* DQ2DLY: DRAMC_DQODLY1[8:11],         4 bits */
              | ((tx_perbit_dly[uiBit+3].best_dqdly & 0xF) << 12)      /* DQ3DLY: DRAMC_DQODLY1[12:15],        4 bits */
              | ((tx_perbit_dly[uiBit+4].best_dqdly & 0xF) << 16)      /* DQ4DLY: DRAMC_DQODLY1[16:19],        4 bits */
              | ((tx_perbit_dly[uiBit+5].best_dqdly & 0xF) << 20)      /* DQ5DLY: DRAMC_DQODLY1[20:23],        4 bits */
              | ((tx_perbit_dly[uiBit+6].best_dqdly & 0xF) << 24)      /* DQ6DLY: DRAMC_DQODLY1[24:27],        4 bits */
              | ((tx_perbit_dly[uiBit+7].best_dqdly & 0xF) << 28);     /*  DQ7DLY: DRAMC_DQODLY1[28:31],        4 bits */
    #else
            dq = dqs_perbyte_dly[byte].dq;
            data =
                ((dq & 0xF) << 0)        /* DQ0DLY: DRAMC_DQODLY1[0:3],          4 bits */
              | ((dq & 0xF) << 4)       /* DQ1DLY: DRAMC_DQODLY1[4:7],          4 bits */
              | ((dq & 0xF) << 8)       /* DQ2DLY: DRAMC_DQODLY1[8:11],         4 bits */
              | ((dq & 0xF) << 12)      /* DQ3DLY: DRAMC_DQODLY1[12:15],        4 bits */
              | ((dq & 0xF) << 16)      /* DQ4DLY: DRAMC_DQODLY1[16:19],        4 bits */
              | ((dq & 0xF) << 20)      /* DQ5DLY: DRAMC_DQODLY1[20:23],        4 bits */
              | ((dq & 0xF) << 24)      /* DQ6DLY: DRAMC_DQODLY1[24:27],        4 bits */
              | ((dq & 0xF) << 28);     /*  DQ7DLY: DRAMC_DQODLY1[28:31],        4 bits */
    #endif
            DRAMC_WRITE_REG(data,DRAMC_DQODLY1+(4*byte));
            opt_tx_dq[byte] = data;

    }
#ifdef TX_PERBIT_WIN
    data =(ucmax_dqsdly_byte[0]) | (ucmax_dqsdly_byte[1]<<4 | ucmax_dqsdly_byte[2] << 8) | (ucmax_dqsdly_byte[3] << 12);
#else
    data =(dqs_perbyte_dly[0].dqs) | (dqs_perbyte_dly[1].dqs<<4 | dqs_perbyte_dly[2].dqs << 8) | (dqs_perbyte_dly[3].dqs << 12);
#endif
    DRAMC_WRITE_REG(data , DRAMC_PADCTL3/* 0x14 */);
    opt_tx_dqs = data;
    TX_PRINT("*(%d):%x\n",DRAMC_PADCTL3,data);
    //DRAMC_WRITE_REG(data,DRAMC_PADCTL3);
#endif

    TX_PRINT("\n\n");
    for (byte = 0; byte < 4; byte++)
    {
#if defined(DEBUG_DRAMC_CALIB)
        TX_PRINT("TX Byte%d: DQ - %d, DQS - %d. win_sum= %d\n", byte, dqs_perbyte_dly[byte].total_dqdly_pass, dqs_perbyte_dly[byte].total_dqsdly_pass, dqs_perbyte_dly[byte].total_dqdly_pass+dqs_perbyte_dly[byte].total_dqsdly_pass-1);
#endif
    //For Denali pass criterion start
        tx_tmp = dqs_perbyte_dly[byte].total_dqdly_pass+dqs_perbyte_dly[byte].total_dqsdly_pass-1;
        if (tx_tmp <= 0)
        {
            tx_flag = 1;
        }
    //For Denali pass criterion end
    }

    TX_PRINT("0x1a8=%xh\n", DRAMC_READ_REG(0x1a8));
    TX_PRINT("0x1ac=%xh\n", DRAMC_READ_REG(0x1ac));
    TX_PRINT("0x1b0=%xh\n", DRAMC_READ_REG(0x1b0));
    TX_PRINT("0x1b4=%xh\n", DRAMC_READ_REG(0x1b4));
    TX_PRINT("0x1b8=%xh\n", DRAMC_READ_REG(0x1b8));
    TX_PRINT("0x1bc=%xh\n", DRAMC_READ_REG(0x1bc));
    TX_PRINT("0x1c0=%xh\n", DRAMC_READ_REG(0x1c0));
    TX_PRINT("0x1c4=%xh\n", DRAMC_READ_REG(0x1c4));
    TX_PRINT("0x00c=%xh\n", DRAMC_READ_REG(0xc));

//For Denali pass criterion start
    if (tx_flag == 1)
        TX_PRINT("\n\nTX FAIL \n\n");
    else
        TX_PRINT("\n\nTX PASS \n\n");
    tx_flag = 0;
//For Denali pass criterion end

    return tx_flag;
}
#else
typedef struct _TXDQS_PERBYTE_DLY_T
{
    S8 first_dqdly_pass;
    S8 last_dqdly_pass;
    S8 total_dqdly_pass;
    S8 first_dqsdly_pass;
    S8 last_dqsdly_pass;
    S8 total_dqsdly_pass;
    U8 best_dqdly;
    U8 best_dqsdly;
    U8 dq;
    U8 dqs;
} TXDQS_PERBYTE_DLY_T;
static void  calib_clk_output_dly_factor_handler(unsigned int value)
{
        unsigned int data;
        //adjust CLK output delay
        print("Change CLK output delay  = %d \n",value);
        data = DRAMC_READ_REG(DRAMC_PADCTL1);
        data = (data & 0xF0FFFFFF) | (value << 24);
        DRAMC_WRITE_REG(data,DRAMC_PADCTL1);
}

static void  calib_cmd_addr_output_dly_factor_handler(unsigned int value)
{
        unsigned int data;
        //adjust CMD delay
        print("Change CMD/ADDR output delay  = %d \n",value);
        data = DRAMC_READ_REG(DRAMC_CMDDLY0);
        data &= 0xE0E0E0E0;
        data |= (value  << 24) | (value << 16) | (value << 8) | value;
        DRAMC_WRITE_REG(data, DRAMC_CMDDLY0);

        data = DRAMC_READ_REG(DRAMC_CMDDLY1);
        data &= 0xE0E0E0E0;
        data |= (value  << 24) | (value << 16) | (value << 8) | value;
        DRAMC_WRITE_REG(data, DRAMC_CMDDLY1);

        data = DRAMC_READ_REG(DRAMC_CMDDLY2);
        data &= 0xE0E0E0E0;
        data |= (value  << 24) | (value << 16) | (value << 8) | value;
        DRAMC_WRITE_REG(data, DRAMC_CMDDLY2);

        data = DRAMC_READ_REG(DRAMC_CMDDLY3);
        data &= 0xE0E0E0E0;
        data |= (value  << 24) | (value << 16) | (value << 8) | value;
        DRAMC_WRITE_REG(data, DRAMC_CMDDLY3);

        data = DRAMC_READ_REG(DRAMC_CMDDLY4);
        data &= 0xE0E0E0E0;
        data |= (value  << 24) | (value << 16) | (value << 8) | value;
        DRAMC_WRITE_REG(data, DRAMC_CMDDLY4);

        data = DRAMC_READ_REG(DRAMC_CMDDLY5);
        data &= 0xE0E0E0EF;
        data |= (value << 24) | (value << 16) | (value << 8);
        DRAMC_WRITE_REG(data, DRAMC_CMDDLY5);

        data = DRAMC_READ_REG(DRAMC_DQSCAL0);
        data &= 0xF0F0FFFF;
        data |= (value << 24) | (value << 16);
        DRAMC_WRITE_REG(data, DRAMC_DQSCAL0);

}


#define MAX_TX_DQDLY_TAPS 16
#define MAX_TX_DQSDLY_TAPS 16
#define TX_STEPS_LPDDR2 1   //8 // This value is obtained by measurement of waveform for the relationship between DQS and CLK.
#define TX_STEPS_DDR3 6

/* Description
  *	TX DQ/DQS per byte calibration
  * Related registers
  *	- PADCTL3 : DQS[0:3]ODLY (0x10011014)
  *		- 4 bits (0~15)
  *		- 1 step 20ps.
  *	- PADCTL2 : DQM[0:3]DLY (0x10011010)
  *		- 4 bits (0~15)
  *		- 1 step 20ps.
  *	- DQODLY[1:4] : DQ[0:31]DLY (0x10011200~0x1001120c)
  *		- 4 bits (0~15)
  *		- 1 step 20ps
  *	- DRAMC_PADCTL1
  * 		- bit 27~bit 24 : clock delay.
  *	- DRAMC_CMDDLY[0:5], DRAMC_DQSCAL0
  *		- Address/cmd delay
  * Algorithm
  *	- Set DQ/DQM output delay to 0 and loop DQS output delay value from 0 to 15. Check the pass range.
  *	- Set DQS output delay value to 0 and loop DQ/DQM output delay value from 0 to 15. Check the pass range.
  *	- If two pass range equal, set both output delay to 8.
  *	- If not equal, add (delay difference)/2 to the one with larger pass range.
  */
int do_sw_tx_dq_dqs_calib(void){
#if 1
    TXDQS_PERBYTE_DLY_T dqs_perbyte_dly[4];
    unsigned int data;
    unsigned int byte;
    int i,j;
    unsigned int mask;
    unsigned int test_len = 0x3FF;
    unsigned int cmp_err_1,cmp_err_2;
    unsigned int dq,dqs;
    unsigned int finish_count;
    unsigned int tx_steps ;
    unsigned int reg_dramc_padctl2;
#if (defined(DQ_DQS_DQM_REMAPPING))
    unsigned char *Bit_DQO_Mapping;

    if((DRAMC_READ_REG(0x00f0)>>31)==0x01)// 4bit mux
    {
        Bit_DQO_Mapping = (unsigned char *)Bit_DQO_Mapping_4bitmux;
    }
    else
    {
        Bit_DQO_Mapping = (unsigned char *)Bit_DQO_Mapping_no_4bitmux;

    }
#endif

    if((*(V_UINT32P)(DRAMC0_BASE + (0x00e4)) >> 7) & 0x1) {
        tx_steps = TX_STEPS_DDR3;
    } else {
        tx_steps = TX_STEPS_LPDDR2;
    }

    dbg_print("in do_sw_tx_dq_dqs_calib(), dram type:%d\n", mt_get_dram_type());
    for(i = 0; i < 4; i++)
    {
        //dqs_perbyte_dly[i].first_dqdly_pass = -1;
        dqs_perbyte_dly[i].first_dqdly_pass = 0;
        dqs_perbyte_dly[i].last_dqdly_pass = -1;
        //dqs_perbyte_dly[i].first_dqsdly_pass = -1;
        dqs_perbyte_dly[i].first_dqsdly_pass = 0;
        dqs_perbyte_dly[i].last_dqsdly_pass = -1;
        dqs_perbyte_dly[i].best_dqdly= -1;
        dqs_perbyte_dly[i].best_dqsdly= -1;
        dqs_perbyte_dly[i].dq= 0;
        dqs_perbyte_dly[i].dqs= 0;
        dqs_perbyte_dly[i].total_dqdly_pass= 0;
        dqs_perbyte_dly[i].total_dqsdly_pass= 0;
    }
    // used for early break
    finish_count = 0;

    /* 1. Tx DQS/DQ all zero*/
    /* 1.1 DQM*/
    //DRAMC_WRITE_CLEAR(0x0000FFFF,DRAMC_PADCTL3);
    reg_dramc_padctl2 = DRAMC_READ_REG(DRAMC_PADCTL2);
    reg_dramc_padctl2 &= 0xFFFF0000;
    DRAMC_WRITE_REG(reg_dramc_padctl2,DRAMC_PADCTL2);

    //DRAMC_WRITE_REG(0x0,DRAMC_PADCTL2);
    /* 1.2 DQ*/
    for (i = 0 ; i < 4; i ++)
    {
        //dbg_print("addr:%x\n",DRAMC_DQODLY1+4*i);
        DRAMC_WRITE_REG(0x0, (DRAMC_DQODLY1+4*i));

    }
    //for (byte = 0; byte < 4; byte ++)
    //{
        /*for (i = 0 ; i < 4; i ++)
        {
            //dbg_print("addr:%x\n",DRAMC_DQODLY1+4*i);
            DRAMC_WRITE_REG(0x0, (DRAMC_DQODLY1+4*i));

        }*/
        //dqs_perbyte_dly[byte].first_dqsdly_pass = 0;
        //dbg_print("dqs_perbyte_dly.first_dqsdly_pass=%x \n",dqs_perbyte_dly[byte].first_dqsdly_pass);
        //dqs_perbyte_dly[byte].last_dqsdly_pass =  MAX_TX_DQSDLY_TAPS - 1;
        /* 2. fix DQ delay = 0, delay DQS to find the pass range  */
        for (i = MAX_TX_DQSDLY_TAPS-1; i >= 0 ; i--)
        {
            data = DRAMC_READ_REG(DRAMC_PADCTL3);
            mask = (0xFFFF0000);
            data = (data & mask) | ((i <<0) | (i<<4) | (i <<8) | (i <<12));
            //dbg_print("mask:%x, data:%x\n",mask,data);
            DRAMC_WRITE_REG(data,DRAMC_PADCTL3);

            #ifndef CA_TRAINING
			if(mt_get_dram_type() != TYPE_PCDDR3)
			{//PCDDR3 no need
            //adjust CMD addr output delay
            calib_cmd_addr_output_dly_factor_handler(i);
            //adjust CLK delay
            calib_clk_output_dly_factor_handler(i);
            //gating window calibration
            do_dqs_gw_calib_1();
			}
            #endif

            /* 2.2 use test agent to find the pass range */
            /* set test patern length*/
            DRAMC_WRITE_REG(0x55000000,0x3C);
            data = DRAMC_READ_REG(0x40);
            DRAMC_WRITE_REG((data & 0xAA000000) | test_len, 0x40);

            //Test Agent 2 write enabling
            DRAMC_WRITE_SET( (1 << 31),DRAMC_CONF2); //enable write
            DRAMC_WRITE_SET( (1 << 30)| (1 << 31),DRAMC_CONF2); //enable read
            while(!(DRAMC_READ_REG(DRAMC_TESTRPT)&(1 << 10)));
            delay_a_while(400);
            cmp_err_1 = DRAMC_READ_REG(DRAMC_CMP_ERR);
            DRAMC_WRITE_CLEAR(((1 << 30) | (1 << 31)),DRAMC_CONF2); //disable test agent2 r/w
            //print("DQS loop = %d, cmp_err_1 = %x \n",i, cmp_err_1);

            for (byte=0; byte < 4; byte++)
            {
                if (dqs_perbyte_dly[byte].last_dqsdly_pass == -1)
                {
                    if (!((cmp_err_1) & (0xFF<<(byte*8))))
                    {
                        dqs_perbyte_dly[byte].last_dqsdly_pass = i;
                        finish_count++;
                        //print("dqs_perbyte_dly.last_dqsdly_pass[%d]=%d,  finish count=%d \n",byte, dqs_perbyte_dly[byte].last_dqsdly_pass, finish_count);
                        //break;
                    }
                }
            }

            if (finish_count==4)
            {
                break;
            }
        }

        /* 3. fix DQS delay = 0, delay DQ to find the pass range  */
        DRAMC_WRITE_CLEAR(0xFFFF ,DRAMC_PADCTL3);

        #ifndef CA_TRAINING
		if(mt_get_dram_type() != TYPE_PCDDR3)
		{//PCDDR3 no need
            //adjust CMD addr output delay
            calib_cmd_addr_output_dly_factor_handler(0);
            //adjust CLK delay
            calib_clk_output_dly_factor_handler(0);
            //gating window calibration
            do_dqs_gw_calib_1();
		}
        #endif

        //dqs_perbyte_dly[byte].first_dqdly_pass = 0;
        //dbg_print("dqs_perbyte_dly.first_dqdly_pass=%x \n",dqs_perbyte_dly[byte].first_dqdly_pass);

        finish_count = 0;

        for (i = MAX_TX_DQDLY_TAPS-1; i >= 0; i--)
        {

            /* 3.1 delay DQ output delay */
            data =
                ((i & 0xF) << 0)       /* DQM0DLY: DRAMC_PADCTL2[0:3],         4 bits */
              | ((i & 0xF) << 4)       /* DQM1DLY: DRAMC_PADCTL2[4:7], 4 bits */
              | ((i & 0xF) << 8)       /* DQM2DLY: DRAMC_PADCTL2[8:11],        4 bits */
              | ((i & 0xF) << 12);

            reg_dramc_padctl2 = DRAMC_READ_REG(DRAMC_PADCTL2);
            reg_dramc_padctl2 &= 0xFFFF0000;
            reg_dramc_padctl2 |= data;
            DRAMC_WRITE_REG(reg_dramc_padctl2 , DRAMC_PADCTL2/* 0x10 */);

            //DRAMC_WRITE_REG(data , DRAMC_PADCTL2/* 0x10 */);
            data =
                ((i & 0xF) << 0)             /* DQ0DLY: DRAMC_DQODLY1[0:3],          4 bits */
              | ((i & 0xF) << 4)               /* DQ1DLY: DRAMC_DQODLY1[4:7],          4 bits */
              | ((i & 0xF) << 8)               /* DQ2DLY: DRAMC_DQODLY1[8:11],         4 bits */
              | ((i & 0xF) << 12)      /* DQ3DLY: DRAMC_DQODLY1[12:15],        4 bits */
              | ((i & 0xF) << 16)      /* DQ4DLY: DRAMC_DQODLY1[16:19],        4 bits */
              | ((i & 0xF) << 20)      /* DQ5DLY: DRAMC_DQODLY1[20:23],        4 bits */
              | ((i & 0xF) << 24)      /* DQ6DLY: DRAMC_DQODLY1[24:27],        4 bits */
              | ((i & 0xF) << 28);
            DRAMC_WRITE_REG(data,DRAMC_DQODLY1);
            DRAMC_WRITE_REG(data,DRAMC_DQODLY2);
            DRAMC_WRITE_REG(data,DRAMC_DQODLY3);
            DRAMC_WRITE_REG(data,DRAMC_DQODLY4);


            //dbg_print("%x = %x\n",DRAMC_DQODLY1+(4*byte),DRAMC_READ_REG(DRAMC_DQODLY1+(4*byte)));
            /* 3.2 use test agent to find the pass range */
            /* set test patern length*/
            DRAMC_WRITE_REG(0x55000000,0x3C);
            data = DRAMC_READ_REG(0x40);
            DRAMC_WRITE_REG((data & 0xAA000000) | test_len, 0x40);
            //Test Agent 2 write enabling
            DRAMC_WRITE_SET( (1 << 31),DRAMC_CONF2); //enable write
            DRAMC_WRITE_SET( (1 << 30)| (1 << 31),DRAMC_CONF2); //enable read
            while(!(DRAMC_READ_REG(DRAMC_TESTRPT)&(1 << 10)));
            delay_a_while(400);

            cmp_err_1 = DRAMC_READ_REG(DRAMC_CMP_ERR);
            //print("DQ loop=%d, cmp_err_1 = %x\n",i, cmp_err_1);
            DRAMC_WRITE_CLEAR(((1 << 30) | (1 << 31)),DRAMC_CONF2); //disable test agent2 r/w

            for (byte=0; byte < 4; byte++)
            {
                if (dqs_perbyte_dly[byte].last_dqdly_pass == -1)
                {
                    if (!(cmp_err_1 &(0xFF<<(byte*8))))
                    {
                        dqs_perbyte_dly[byte].last_dqdly_pass = i;
                        finish_count++;
                        //print("dqs_perbyte_dly.last_dqdly_pass[%d]=%d,  finish count=%d \n",byte, dqs_perbyte_dly[byte].last_dqdly_pass, finish_count);
                        //break;
                    }
                }
            }

            if (finish_count==4)
            {
                break;
            }
        }

        for (byte=0; byte < 4; byte++)
        {

        dqs_perbyte_dly[byte].total_dqsdly_pass = dqs_perbyte_dly[byte].last_dqsdly_pass - dqs_perbyte_dly[byte].first_dqsdly_pass + 1;
        //print("total_dqsdly_pass:%x\n", dqs_perbyte_dly[byte].total_dqsdly_pass);
        dqs_perbyte_dly[byte].total_dqdly_pass = dqs_perbyte_dly[byte].last_dqdly_pass - dqs_perbyte_dly[byte].first_dqdly_pass + 1;
        //dbg_print("total_dqdly_pass:%x\n", dqs_perbyte_dly[byte].total_dqdly_pass);
        /* 4. find the middle of the pass range of DQ and DQS*/
        /* 5. if the middle of the pass range is in the DQ, the delay of (DQS,DQ) is (0,DQ),
         *    if the middle of the pass range is in the DQS the delay of (DQS,DQ) is (DQS,0)*/
        if (dqs_perbyte_dly[byte].total_dqdly_pass == dqs_perbyte_dly[byte].total_dqsdly_pass)
        {
            dqs_perbyte_dly[byte].dqs = dqs_perbyte_dly[byte].dq = tx_steps;
        }
        else if (dqs_perbyte_dly[byte].total_dqdly_pass > dqs_perbyte_dly[byte].total_dqsdly_pass)
        {
            dqs_perbyte_dly[byte].dqs = 0 + tx_steps;
            dqs_perbyte_dly[byte].dq += (dqs_perbyte_dly[byte].total_dqdly_pass - dqs_perbyte_dly[byte].total_dqsdly_pass)/2 + tx_steps;
            if (dqs_perbyte_dly[byte].dq > 0xf){
				dbg_print("warning:byte %d,dq:%x",byte,dqs_perbyte_dly[byte].dq);
				dqs_perbyte_dly[byte].dq = 0xf;
			}
        }
        else
        {
            #if 1
            dqs_perbyte_dly[byte].dqs = (dqs_perbyte_dly[byte].total_dqsdly_pass - dqs_perbyte_dly[byte].total_dqdly_pass)/2;
            if(tx_steps < dqs_perbyte_dly[byte].dqs)
            {
                dqs_perbyte_dly[byte].dq = 0;
                dbg_print("warning:byte %d,dqs:%x, TX_STEPS:%x \n",byte,dqs_perbyte_dly[byte].dqs, tx_steps);
            }
            else
            {
                dqs_perbyte_dly[byte].dqs = 0 + tx_steps;
                dqs_perbyte_dly[byte].dq = tx_steps - ((dqs_perbyte_dly[byte].total_dqsdly_pass - dqs_perbyte_dly[byte].total_dqdly_pass)/2);
                if (dqs_perbyte_dly[byte].dq < 0)
                {
                    dbg_print("warning:byte %d,dq:%x \n",byte,dqs_perbyte_dly[byte].dq);
                    dqs_perbyte_dly[byte].dq = 0;
                }
            }

            #else
            dqs_perbyte_dly[byte].dqs += (dqs_perbyte_dly[byte].total_dqsdly_pass - dqs_perbyte_dly[byte].total_dqdly_pass)/2 + tx_steps;
            if (dqs_perbyte_dly[byte].dqs > 0xf) {
				dbg_print("warning:byte %d,dqs:%x",byte,dqs_perbyte_dly[byte].dqs);
				dqs_perbyte_dly[byte].dqs = 0xf;
			}
            dqs_perbyte_dly[byte].dq = 0 + tx_steps;
            #endif
        }
        print("byte:%x, (DQS,DQ)=(%x,%x)\n",byte,dqs_perbyte_dly[byte].dqs, dqs_perbyte_dly[byte].dq);
        /* 6. fix the (DQS,DQ) for this byte, find the next byte */

    }

    //}

#ifdef DQ_DQS_DQM_REMAPPING
    data = 0;
    for (byte = 0 ; byte < 4; byte ++)
    {
    	data |= (dqs_perbyte_dly[byte].dq << (DQM_Mapping[byte]*4));
    }

    reg_dramc_padctl2 = DRAMC_READ_REG(DRAMC_PADCTL2);
    reg_dramc_padctl2 &= 0xFFFF0000;
    reg_dramc_padctl2 |= data;
    DRAMC_WRITE_REG(reg_dramc_padctl2 , DRAMC_PADCTL2/* 0x10 */);

    //DRAMC_WRITE_REG(data , DRAMC_PADCTL2/* 0x10 */);
    opt_tx_dqm = data;

    for (byte = 0 ; byte < 4; byte ++)
    {
    	opt_tx_dq[byte] = 0;
    }
    for (byte = 0 ; byte < 4; byte ++)
    {
         unsigned int uiRemapDQ;
         dq = dqs_perbyte_dly[byte].dq;
    	for (i=0; i<8; i++) {
    		uiRemapDQ = Bit_DQO_Mapping[(byte<<3)+i];
    		opt_tx_dq[uiRemapDQ>>3] |= ((dq & 0xF) << ((uiRemapDQ & 0x07)<<2));
    	}
    }

    for (byte = 0 ; byte < 4; byte ++)
    {
    	   //dbg_print("DRAMC_DQODLY%d=%xh\n", byte+1, opt_tx_dq[byte]);
            DRAMC_WRITE_REG(opt_tx_dq[byte], DRAMC_DQODLY1+(4*byte));
    }

    data = 0;
    for (byte = 0 ; byte < 4; byte ++)
    {
    	data |= (dqs_perbyte_dly[byte].dqs << (DQSO_Mapping[byte]*4));
    }

    DRAMC_WRITE_REG(data , DRAMC_PADCTL3/* 0x14 */);
    opt_tx_dqs = data;
    //print("*(%d):%x\n",DRAMC_PADCTL3,data);
    DRAMC_WRITE_REG(data,DRAMC_PADCTL3);

#else
    data =
        ((dqs_perbyte_dly[0].dq & 0xF) << 0)             /* DQM0DLY: DRAMC_PADCTL2[0:3],         4 bits */
      | ((dqs_perbyte_dly[1].dq & 0xF) << 4)       /* DQM1DLY: DRAMC_PADCTL2[4:7], 4 bits */
      | ((dqs_perbyte_dly[2].dq & 0xF) << 8)       /* DQM2DLY: DRAMC_PADCTL2[8:11],        4 bits */
      | ((dqs_perbyte_dly[3].dq & 0xF) << 12);

    reg_dramc_padctl2 = DRAMC_READ_REG(DRAMC_PADCTL2);
    reg_dramc_padctl2 &= 0xFFFF0000;
    reg_dramc_padctl2 |= data;
    DRAMC_WRITE_REG(reg_dramc_padctl2 , DRAMC_PADCTL2/* 0x10 */);

    //DRAMC_WRITE_REG(data , DRAMC_PADCTL2/* 0x10 */);
    opt_tx_dqm = data;
    for (byte = 0 ; byte < 4; byte ++)
    {
            dq = dqs_perbyte_dly[byte].dq;
            data =
                ((dq & 0xF) << 0)             /* DQ0DLY: DRAMC_DQODLY1[0:3],          4 bits */
              | ((dq & 0xF) << 4)               /* DQ1DLY: DRAMC_DQODLY1[4:7],          4 bits */
              | ((dq & 0xF) << 8)               /* DQ2DLY: DRAMC_DQODLY1[8:11],         4 bits */
              | ((dq & 0xF) << 12)      /* DQ3DLY: DRAMC_DQODLY1[12:15],        4 bits */
              | ((dq & 0xF) << 16)      /* DQ4DLY: DRAMC_DQODLY1[16:19],        4 bits */
              | ((dq & 0xF) << 20)      /* DQ5DLY: DRAMC_DQODLY1[20:23],        4 bits */
              | ((dq & 0xF) << 24)      /* DQ6DLY: DRAMC_DQODLY1[24:27],        4 bits */
              | ((dq & 0xF) << 28);
            DRAMC_WRITE_REG(data,DRAMC_DQODLY1+(4*byte));
            opt_tx_dq[byte] = data;

    }
    data =(dqs_perbyte_dly[0].dqs) | (dqs_perbyte_dly[1].dqs<<4 | dqs_perbyte_dly[2].dqs << 8) | (dqs_perbyte_dly[3].dqs << 12);
    DRAMC_WRITE_REG(data , DRAMC_PADCTL3/* 0x14 */);
    opt_tx_dqs = data;
    //print("%d,data:%x\n",DRAMC_PADCTL3,data); DRAMC_WRITE_REG(data,DRAMC_PADCTL3);

#endif
#endif

    mcSHOW_DBG_MSG3(("0x1a8=%xh\n", DRAMC_READ_REG(0x1a8)));
    mcSHOW_DBG_MSG3(("0x1ac=%xh\n", DRAMC_READ_REG(0x1ac)));
    mcSHOW_DBG_MSG3(("0x1b0=%xh\n", DRAMC_READ_REG(0x1b0)));
    mcSHOW_DBG_MSG3(("0x1b4=%xh\n", DRAMC_READ_REG(0x1b4)));
    mcSHOW_DBG_MSG3(("0x1b8=%xh\n", DRAMC_READ_REG(0x1b8)));
    mcSHOW_DBG_MSG3(("0x1bc=%xh\n", DRAMC_READ_REG(0x1bc)));
    mcSHOW_DBG_MSG3(("0x1c0=%xh\n", DRAMC_READ_REG(0x1c0)));
    mcSHOW_DBG_MSG3(("0x1c4=%xh\n", DRAMC_READ_REG(0x1c4)));
    mcSHOW_DBG_MSG3(("0x00c=%xh\n", DRAMC_READ_REG(0xc)));

    return 0;
}
#endif


/*
 * dramc_calib: Do DRAMC calibration.
 * Return error code;
 */
int dramc_calib(void)
{
    int err;
    int reg_val;
    int rank0_col,rank1_col,bak_cona,bak_conf1;
//#define DEBUG_DRAMC_CALIB
#if defined(DEBUG_DRAMC_CALIB)
    int temp;

    /* enable ARM CPU PMU */
    asm volatile(
        "MRC p15, 0, %0, c9, c12, 0\n"
        "BIC %0, %0, #1 << 0\n"   /* disable */
        "ORR %0, %0, #1 << 2\n"   /* reset cycle count */
        "BIC %0, %0, #1 << 3\n"   /* count every clock cycle */
        "MCR p15, 0, %0, c9, c12, 0\n"
        : "+r"(temp)
        :
        : "cc"
    );
    asm volatile(
        "MRC p15, 0, %0, c9, c12, 0\n"
        "ORR %0, %0, #1 << 0\n"   /* enable */
        "MCR p15, 0, %0, c9, c12, 0\n"
        "MRC p15, 0, %0, c9, c12, 1\n"
        "ORR %0, %0, #1 << 31\n"
        "MCR p15, 0, %0, c9, c12, 1\n"
        : "+r"(temp)
        :
        : "cc"
    );
#endif

    RANK_CURR = 0;
    /*modify MA type*/
    bak_cona = *(volatile unsigned *)(EMI_CONA);
    bak_conf1 = *((volatile unsigned *)(DRAMC0_BASE + 0x04));

    //printf("before:0x04:%x\n",*((volatile unsigned *)(DRAMC0_BASE + 0x04)));
    rank0_col = (*(volatile unsigned *)(EMI_CONA) & (0x3<<4))>>4;
    *((volatile unsigned *)(DRAMC0_BASE + 0x04)) = (*((volatile unsigned *)(DRAMC0_BASE + 0x04)) & (~(0x3 << 8)))| (rank0_col << 8);
    //printf("after:reg_val:%x,0x04:%x\n",rank0_col,*((volatile unsigned *)(DRAMC0_BASE + 0x04)));

#if 0 // todo: tmp code, test duty setting
    DRAMC_WRITE_REG(0xC00, DRAMC_TDSEL0);
#endif

#ifdef CA_TRAINING
{
    //int drvp = 0;
    //int drvn = 0;

    //for (; drvp <= 0xF; drvp++, drvn++)
    {
        #if 0
        print("Do CATraining, drvp:%d, drvn:%d\n", drvp, drvn);

        *((V_UINT32P)(DRAMC0_BASE + 0x00b4)) = 0x00220022 | ((drvp<<28) + (drvn<<24) + (drvp<<12) + (drvn<<8));
        *((V_UINT32P)(DDRPHY_BASE + 0x00b4)) = 0x00220022 | ((drvp<<28) + (drvn<<24) + (drvp<<12) + (drvn<<8));
        *((V_UINT32P)(DRAMC0_BASE + 0x00b8)) = 0x00220022 | ((drvp<<28) + (drvn<<24) + (drvp<<12) + (drvn<<8));
        *((V_UINT32P)(DDRPHY_BASE + 0x00b8)) = 0x00220022 | ((drvp<<28) + (drvn<<24) + (drvp<<12) + (drvn<<8));
        *((V_UINT32P)(DRAMC0_BASE + 0x00bc)) = 0x00220022 | ((drvp<<28) + (drvn<<24) + (drvp<<12) + (drvn<<8));
        *((V_UINT32P)(DDRPHY_BASE + 0x00bc)) = 0x00220022 | ((drvp<<28) + (drvn<<24) + (drvp<<12) + (drvn<<8));
        #endif
      if(*((V_UINT32P)(DRAMC0_BASE + 0x00d8)) >> 30 == 0x01)
      {
        print("LPDDR3 178Ball CA training start...\n");
        DramcCATraining_178Ball();
      }
      else
      {
            print("LPDDR3 168Ball CA training start...\n");
            DramcCATraining();
      }
    }
}
#endif

#ifdef WRITE_LEVELING
    if (TYPE_LPDDR3 == mt_get_dram_type()) {
	DramcWriteLeveling();
    }
#endif

    /* do calibration for DQS gating window (phase 1) for rank 0*/
    DQS_PRINT=1;
    err = do_dqs_gw_calib_1();
    if (err < 0) {
        goto dramc_calib_exit;
    }
    DQS_PRINT=0;
    opt_gw_coarse_value0 = opt_gw_coarse_value;
    opt_gw_fine_value0 = opt_gw_fine_value;

    dbg_print("rank 0 c = %x, %s\n", opt_gw_coarse_value, *opt_gw_coarse_value);
    dbg_print("rank 0 f = %x, %s\n", opt_gw_fine_value, *opt_gw_fine_value);

    dbg_print("rank 0 coarse = %s\n", *opt_gw_coarse_value0);
    dbg_print("rank 0 fine = %s\n", *opt_gw_fine_value0);
    /* do DLE calibration */
    err = do_dle_calib();
    if (err < 0) {
        goto dramc_calib_exit;
    }
    /* do SW RX calibration for DQ/DQS input delay */
    err = do_sw_rx_dq_dqs_calib();
    if (err < 0) {
        goto dramc_calib_exit;
    }

#ifdef DDR3_AUTO_DETECT_SUPPORT
    if(ddr3_autodetect_is_enable())
    {
        /*cona & conf1 maybe change because size detect*/
		ddr3_rank_size_detect();
        bak_cona = *(volatile unsigned *)(EMI_CONA);
        bak_conf1 = *((volatile unsigned *)(DRAMC0_BASE + 0x04));
    }
#endif

    opt_rx_dqs0 = DRAMC_READ_REG(DRAMC_R0DELDLY);
    if ( *(volatile unsigned *)(EMI_CONA)& 0x20000)      // Check dual ranks.
    {
        /*modify MA type*/
        //printf("before:0x04:%x\n",*((volatile unsigned *)(DRAMC0_BASE + 0x04)));
        RANK_CURR = 1;
        rank1_col = (*(volatile unsigned *)(EMI_CONA) & (0x3<<6))>>6;
        *((volatile unsigned *)(DRAMC0_BASE + 0x04)) = (*((volatile unsigned *)(DRAMC0_BASE + 0x04)) & (~(0x3 << 8)))| (rank1_col << 8);
        *((volatile unsigned *)(EMI_CONA)) = (*((volatile unsigned *)(EMI_CONA)) & (~(0x3 << 4)))| (rank1_col << 4);
        //print("after:rank1_col:%x,0x04:%x,%x\n",rank1_col,*((volatile unsigned *)(DRAMC0_BASE + 0x04)), *((volatile unsigned *)(EMI_CONA)));

        //make sure we in chip select 1. Swap CS0 and CS1.
        *((volatile unsigned *)(DRAMC0_BASE + 0x0110)) |= (0x8);
        *((volatile unsigned *)(DDRPHY_BASE + 0x0110)) |= (0x8);

    #ifdef DDR3_AUTO_DETECT_SUPPORT
    if(ddr3_autodetect_is_enable())
    {
        /*apply rank0 gating to rank1*/
        dqsi_gw_dly_coarse_factor_handler_rank1(opt_gw_coarse_value0, 1);
        dqsi_gw_dly_fine_factor_handler_rank1(opt_gw_fine_value0, 4);
        DRAMC_WRITE_REG(opt_rx_dqs0,DRAMC_R1DELDLY);
		print("[EMI]rank auto detect\n");

        *(volatile unsigned int *)(DRAM_BASE) = 0x12345678;
        *(volatile unsigned int *)(DRAM_BASE+0x00000004) = ~0x12345678;
        if((*(volatile unsigned int *)(DRAM_BASE) == 0x12345678) &&
           (*(volatile unsigned int *)(DRAM_BASE+0x00000004) == ~0x12345678))
        {
            print("[EMI]rank number auto detect:==dual rank==\n");
			emi_settings[DDR_index].DRAM_RANK_SIZE[1] = emi_settings[DDR_index].DRAM_RANK_SIZE[0];
			emi_settings[DDR_index].EMI_CONA_VAL = *(volatile unsigned *)(EMI_CONA);
        }
        else
        {
            print("[EMI]rank auto detect:==single rank==\n");
            *(volatile unsigned *)(EMI_CONA) &=  ~0x20000;
            bak_cona = *(volatile unsigned *)(EMI_CONA);
            emi_settings[DDR_index].EMI_CONA_VAL = bak_cona;
            emi_settings[DDR_index].DRAM_RANK_SIZE[RANK_CURR] = 0;
			//apply cs0 to cs1
			*((volatile unsigned int *)(DRAMC0_BASE + 0x01ec)) |= 0x01<<12;
			*((volatile unsigned int *)(DDRPHY_BASE + 0x01ec)) |= 0x01<<12;

            *((volatile unsigned int *)(DRAMC0_BASE + 0x0110)) &= ~0x11;
            *((volatile unsigned int *)(DDRPHY_BASE + 0x0110)) &= ~0x11;
            DDR_PHY_RESET_NEW();

            goto dramc_calib_back_to_rank0;
        }
    }
#endif

        /* do calibration for DQS gating window (phase 1) for rank 1*/
        DQS_PRINT=1;
        err = do_dqs_gw_calib_1();
        if (err < 0) {
            goto dramc_calib_exit;
        }
        DQS_PRINT=0;
        opt_gw_coarse_value1 = opt_gw_coarse_value;
        opt_gw_fine_value1 = opt_gw_fine_value;
        print("rank 1 coarse = %s\n", *opt_gw_coarse_value1 );
        print("rank 1 fine = %s\n", *opt_gw_fine_value1  );

            /* do DLE calibration */
        err = do_dle_calib();
        if (err < 0) {
            goto dramc_calib_exit;
        }
        /* do SW RX calibration for DQ/DQS input delay */
        err = do_sw_rx_dq_dqs_calib();
        if (err < 0) {
            goto dramc_calib_exit;
        }
        opt_rx_dqs1 = DRAMC_READ_REG(DRAMC_R0DELDLY);

        #if 0
        print("%s, Coarse, %x, %s\n", __FUNCTION__, opt_gw_coarse_value1, *opt_gw_coarse_value1);
        print("%s, Fine, BYTE0, %x, %s\n", __FUNCTION__, opt_gw_fine_value1, *opt_gw_fine_value1);
        print("%s, Fine, BYTE1, %x, %s\n", __FUNCTION__, opt_gw_fine_value1+1, *(opt_gw_fine_value1+1));
        print("%s, Fine, BYTE2, %x, %s\n", __FUNCTION__, opt_gw_fine_value1+2, *(opt_gw_fine_value1+2));
        print("%s, Fine, BYTE3, %x, %s\n", __FUNCTION__, opt_gw_fine_value1+3, *(opt_gw_fine_value1+3));
        #endif

        dqsi_gw_dly_coarse_factor_handler_rank1(opt_gw_coarse_value1, 1);
        dqsi_gw_dly_fine_factor_handler_rank1(opt_gw_fine_value1, 4);

        DRAMC_WRITE_REG(opt_rx_dqs1,DRAMC_R1DELDLY);

#ifdef DDR3_AUTO_DETECT_SUPPORT
dramc_calib_back_to_rank0:
#endif
        //swap back
        //make sure we in chip select 0 now
        *((volatile unsigned *)(DRAMC0_BASE + 0x0110)) &= (~0x8);
        *((volatile unsigned *)(DDRPHY_BASE + 0x0110)) &= (~0x8);
     #ifdef	DDR3_AUTO_DETECT_SUPPORT
	    if(ddr3_autodetect_is_enable())
		 DDR_PHY_RESET_NEW();
     #endif

    }

    RANK_CURR = 0;
    *(volatile unsigned *)(EMI_CONA) = bak_cona;
    *((volatile unsigned *)(DRAMC0_BASE + 0x04))= bak_conf1;
    dqsi_gw_dly_coarse_factor_handler(opt_gw_coarse_value0, 1);
    dqsi_gw_dly_fine_factor_handler(opt_gw_fine_value0, 4);
    DRAMC_WRITE_REG(opt_rx_dqs0,DRAMC_R0DELDLY);

    print("[MEM]CONA:%x,conf1:%x\n",*((volatile unsigned *)(EMI_CONA)),*((volatile unsigned *)(DRAMC0_BASE + 0x04)));
     /* do SW TX calibration for DQ/DQS output delay */
    err = do_sw_tx_dq_dqs_calib();
    if (err < 0) {
        goto dramc_calib_exit;
    }
    dqsi_gw_dly_coarse_factor_handler(opt_gw_coarse_value0, 1);
    dqsi_gw_dly_fine_factor_handler(opt_gw_fine_value0, 4);

    #ifdef ASYMMETRIC_FEATURE
    {
        opt_rx_dqs1 = DRAMC_READ_REG(DRAMC_R0DELDLY);

        dqsi_gw_dly_coarse_factor_handler_rank1(opt_gw_coarse_value0, 1);
        dqsi_gw_dly_fine_factor_handler_rank1(opt_gw_fine_value0, 4);

        DRAMC_WRITE_REG(opt_rx_dqs1,DRAMC_R1DELDLY);
    }
    #endif
	#ifdef DLE_RETRY_AFTER_RX_TX // Todo: Tmp code
	/* do DLE calibration again*/
    err = do_dle_calib();
    if (err < 0) {
        goto dramc_calib_exit;
    }
	#endif

dramc_calib_exit:

#if defined(DEBUG_DRAMC_CALIB)
    /* get CPU cycle count from the ARM CPU PMU */
    asm volatile(
        "MRC p15, 0, %0, c9, c12, 0\n"
        "BIC %0, %0, #1 << 0\n"   /* disable */
        "MCR p15, 0, %0, c9, c12, 0\n"
        "MRC p15, 0, %0, c9, c13, 0\n"
        : "+r"(temp)
        :
        : "cc"
    );
    print("DRAMC calibration takes %d CPU cycles\n\r", temp);
#endif



#if defined(DEBUG_DRAMC_CALIB)
        print("eDRAMC_RX REG\n");

        print("*DQIDLY1 = 0x%x\n", DRAMC_READ_REG(DRAMC_DQIDLY1));
        print("*DQIDLY2 = 0x%x\n", DRAMC_READ_REG(DRAMC_DQIDLY2));
        print("*DQIDLY3 = 0x%x\n", DRAMC_READ_REG(DRAMC_DQIDLY3));
        print("*DQIDLY4 = 0x%x\n", DRAMC_READ_REG(DRAMC_DQIDLY4));
        print("*DQIDLY5 = 0x%x\n", DRAMC_READ_REG(DRAMC_DQIDLY5));
        print("*DQIDLY6 = 0x%x\n", DRAMC_READ_REG(DRAMC_DQIDLY6));
        print("*DQIDLY7 = 0x%x\n", DRAMC_READ_REG(DRAMC_DQIDLY7));
        print("*DQIDLY8 = 0x%x\n", DRAMC_READ_REG(DRAMC_DQIDLY8));
        print("*DRAMC_R0DELDLY = 0x%x\n", DRAMC_READ_REG(DRAMC_R0DELDLY));
        print("*DRAMC_R1DELDLY = 0x%x\n", DRAMC_READ_REG(DRAMC_R1DELDLY));
#endif


    return err;
}


//----------------------------------------------------------------------------------
/***************************write leveling***************************/
#if defined(WRITE_LEVELING) || defined(CA_TRAINING)
#define mcBITL(b)               (1L << (b))
//#define mcBIT(b)                (1 << (b))
#define mcBIT(b)                mcBITL(b)
#define mcMASK(w)               (mcBIT(w) - 1)
#define mcMASKS(w, b)           (mcMASK(w) << (b))
//#define mcCLR_BIT_C(a)          ((-1 - (Bit_#a)) & 0xFFFF)
#define mcCLR_BIT_C(a)          ((~Bit_##a) & 0xFFFF)

#define mcSET_MASK(a, b)        ((a) |= (b))
#define mcCLR_MASK(a, b)        ((a) &= (~(b)))
//#define mcCLR_SET_MASK(a, b, c)  ((a) = ((a) & (~(b))) | (c))
//          \_ same as mcCLR_MASK + mcSET_MASK at optimized code
#define mcSET_BIT(a, b)         mcSET_MASK(a, mcBIT(b))
#define mcCLR_BIT(a, b)         mcCLR_MASK(a, mcBIT(b))
#define mcCHK_BIT1(a, b)        ((a) & mcBIT(b))
#define mcCHK_BITM(a, b, m)     (((a) >> (b)) & (m))
#define mcCHK_BITS(a, b, w)     mcCHK_BITM(a, b, mcMASK(w))
//#define mcTEST_BIT(a, b)        mcCHK_BITM(a, b, 1)
#define mcTEST_BIT(a, b)        mcCHK_BIT1(a, b)
#define mcCHG_BIT(a, b)         ((a) ^= mcBIT(b))
//#define mcSET_FIELD0(var, value, mask, pos) mcSET_MASK(var, ((value) << (pos)) & (mask))

#define mcSET_FIELD(var, value, mask, pos)  \
{                                           \
    mcCLR_MASK(var, mask);                  \
    mcSET_FIELD0(var, value, mask, pos);    \
}

#define mcGET_FIELD(var, mask, pos)     (((var) & (mask)) >> (pos))
#define mcSET_DRAMC_REG_ADDR(offset)    (offset)

//#define mcDELAY_US(Delay)  	    delay_a_while(Delay*1000)
#define SHOW_DBG_MSG          dbg_print

//#define MAX_TX_DQSDLY_TAPS 16
//#define DQS_NUMBER 4
//#define DQS_BIT_NUMBER 8

U8 ucDram_Register_Write(U32 u4reg_addr, U32 u4reg_value)
{
	U8 ucstatus = 0;
	DRAMC_WRITE_REG(u4reg_value, u4reg_addr);
	return ucstatus;
}

U8 ucDram_Register_Read(U32 u4reg_addr, U32 *pu4reg_value)
{
	U8 ucstatus = 0;
	*pu4reg_value = DRAMC_READ_REG(u4reg_addr);
	return ucstatus;
}

// LPDDR DQ -> PHY DQ mapping
#if 1
const U32 uiLPDDR_PHY_Mapping_POP_CHA[32] = {4, 6, 5, 7, 15, 13, 14, 12,
                                             31, 28, 29, 30, 22, 21, 20, 23,
                                             8, 10, 9, 11, 1, 3, 0, 2,
                                             16, 18, 19, 17, 27, 24, 26, 25};
#else
const U32 uiLPDDR_PHY_Mapping_POP_CHA[32] = {7, 4, 5, 6, 14, 12, 15, 13,
                                             20, 29, 28, 30, 23, 31, 22, 21,
                                             9, 10, 3, 8, 2, 0, 1, 11,
                                             27, 19, 17, 18, 26, 24, 16, 25};
#endif


#define DRAMC_REG_MRS 0x088
#endif


#ifdef CA_TRAINING
//#define CA_DELAY_CHAIN

void DramcEnterSelfRefresh(U8 op)
{
U8 ucstatus = 0;
U32 uiTemp;

    if (op == 1) // enter self refresh
    {
        ucstatus |= ucDram_Register_Read(mcSET_DRAMC_REG_ADDR(DRAMC_REG_CONF1), &uiTemp);
        mcSET_BIT(uiTemp, POS_CONF1_SELFREF);
        ucstatus |= ucDram_Register_Write(mcSET_DRAMC_REG_ADDR(DRAMC_REG_CONF1), uiTemp);
        mcDELAY_US(2);
        ucstatus |= ucDram_Register_Read(mcSET_DRAMC_REG_ADDR(DRAMC_REG_SPCMDRESP), &uiTemp);
        while ( (mcTEST_BIT(uiTemp, POS_SPCMDRESP_SREF_STATE))==0)
        {
            mcSHOW_DBG_MSG3(("Still not enter self refresh...\n"));
    	    ucstatus |= ucDram_Register_Read(mcSET_DRAMC_REG_ADDR(DRAMC_REG_SPCMDRESP), &uiTemp);
        }
    }
    else // exit self refresh
    {
        ucstatus |= ucDram_Register_Read(mcSET_DRAMC_REG_ADDR(DRAMC_REG_CONF1), &uiTemp);
        mcCLR_BIT(uiTemp, POS_CONF1_SELFREF);
        ucstatus |= ucDram_Register_Write(mcSET_DRAMC_REG_ADDR(DRAMC_REG_CONF1), uiTemp);
        mcDELAY_US(2);
        ucstatus |= ucDram_Register_Read(mcSET_DRAMC_REG_ADDR(DRAMC_REG_SPCMDRESP), &uiTemp);
        while ( (mcTEST_BIT(uiTemp, POS_SPCMDRESP_SREF_STATE))!=0)
        {
            mcSHOW_DBG_MSG3(("Still not exit self refresh...\n"));
    	    ucstatus |= ucDram_Register_Read(mcSET_DRAMC_REG_ADDR(DRAMC_REG_SPCMDRESP), &uiTemp);
        }
    }

    // tREFI/4 may cause self refresh fail. Set to tREFI manual first. After SF, switch back.
//    ucstatus |= ucDram_Register_Write(mcSET_DRAMC_REG_ADDR(0x114), uiReg114h);
}


//-------------------------------------------------------------------------
/** DramcCATraining
 *  start the calibrate the skew between Clk pin and CAx pins.
 *  @param p                Pointer of context created by DramcCtxCreate.
 *  @retval status          (DRAM_STATUS_T): DRAM_OK or DRAM_FAIL
 */
//-------------------------------------------------------------------------
// For 8163:
// CA0 - RA8 - 0x1b0 0~3
// CA1 - RA4 - 0x1ac 0~3
// CA2 - RA0 - 0x1a8 0~3
// CA3 - RA1 - 0x1a8 8~11
// CA4 - RA3 - 0x1a8 24~27
// CA5 - RA5 - 0x1ac 8~11
// CA6 - RA9 - 0x1b0 8~11
// CA7 - RA13 - 0x1bc 16~20
// CA8 - BA0 - 0x1b4 8~11
// CA9 - RA15 - 0x1c0 16~20
#define MAX_CLKO_DELAY         15
#define CATRAINING_NUM        10
int DramcCATraining(void)
{
    U8 ucstatus = 0;
    U32 uiTemp, uiDelay, uiFinishCount, uiCA, uiMR41=1, uiReg1DCh, uiReg1E0h, uiRisingEdge, uiFallingEdge;
    U32 u4prv_register_0fc, u4prv_register_044, u4prv_register_63c;

    S8 iCenter[CATRAINING_NUM],  iFirstClkPass[CATRAINING_NUM], iLastClkPass[CATRAINING_NUM];
    S8 iFirstCAPass[CATRAINING_NUM], iLastCAPass[CATRAINING_NUM], iMaxCenter;
    S8 iCAShift[CATRAINING_NUM];
#ifdef CKE_CS_DLY_SETTING
    S8 CAShift_Avg = 0;
    U32 u4CAshift_Avg = 0x00;
#endif
    S8 iBestFirstClkPass[CATRAINING_NUM], iBestLastClkPass[CATRAINING_NUM];
    S8 iBestFirstCAPass[CATRAINING_NUM], iBestLastCAPass[CATRAINING_NUM];
    S32 iPass, iClkWinSize, iCAWinSize;
    U32 *uiLPDDR_PHY_Mapping;
    U32 u4MRSValue = 0;

    // error handling
    if (TYPE_LPDDR3 != mt_get_dram_type())
    {
        CA_PRINT("Wrong DRAM TYPE. Only support LPDDR3 in CA training!!\n");
        return -1;
    }

    ucDram_Register_Read(mcSET_DRAMC_REG_ADDR(DRAMC_REG_MRS), &u4MRSValue);
    CA_PRINT("MRS Register 0x%x value: 0x%x\n", DRAMC_REG_MRS, u4MRSValue);

    // Disable clock gating to prevent DDRPHY enter idle.
    ucstatus |= ucDram_Register_Read(mcSET_DRAMC_REG_ADDR(0x63c), &uiTemp); //todo: 0x63c
    u4prv_register_63c = uiTemp;
    mcCLR_BIT(uiTemp, 2);
    mcCLR_BIT(uiTemp, 1);
    ucstatus |= ucDram_Register_Write(mcSET_DRAMC_REG_ADDR(0x63c), uiTemp);

    // Edward : no idea why TXP>1 will cause CA training fail. Now set it after CA training.
    ucstatus |= ucDram_Register_Read(mcSET_DRAMC_REG_ADDR(0x0fc), &u4prv_register_0fc);
    ucstatus |= ucDram_Register_Write(mcSET_DRAMC_REG_ADDR(0x0fc), (u4prv_register_0fc & 0x8fffffff));

    //disable auto refresh: REFCNT_FR_CLK = 0 (0x1dc[23:16]), ADVREFEN = 0 (0x44[30])
    ucstatus |= ucDram_Register_Read(mcSET_DRAMC_REG_ADDR(DRAMC_REG_TEST2_3), &uiTemp);
    u4prv_register_044 = uiTemp;
    mcCLR_BIT(uiTemp, POS_TEST2_3_ADVREFEN);
    ucstatus |= ucDram_Register_Write(mcSET_DRAMC_REG_ADDR(DRAMC_REG_TEST2_3), uiTemp);

	// todo: map
  	uiLPDDR_PHY_Mapping = (U32 *)uiLPDDR_PHY_Mapping_POP_CHA;

    for (uiCA=0; uiCA<CATRAINING_NUM; uiCA++)
    {
        iLastClkPass[uiCA] = iLastCAPass[uiCA] = -1;
        iFirstClkPass[uiCA] = iFirstCAPass[uiCA] = -1;
        iBestLastClkPass[uiCA] = iBestLastCAPass[uiCA] = -1;
        iBestFirstClkPass[uiCA] = iBestFirstCAPass[uiCA] = -1;
    }

    // Sweep clock output delay first.

    // Keep DQ input always ON.
    ucstatus |= ucDram_Register_Read(mcSET_DRAMC_REG_ADDR(0xd8), &uiTemp);

	mcSET_MASK(uiTemp, MASK_MCKDLY_FIXDQIEN);

    ucstatus |= ucDram_Register_Write(mcSET_DRAMC_REG_ADDR(0xd8), uiTemp);

    // Let MIO_CK always ON.
    // Disable auto refresh: REFCNT_FR_CLK = 0 (0x1dc[23:16])
    ucstatus |= ucDram_Register_Read(mcSET_DRAMC_REG_ADDR(0x1dc), &uiTemp);
    uiReg1DCh = uiTemp;
    mcSET_BIT(uiTemp, 26);
    mcCLR_MASK(uiTemp, MASK_DRAMC_PD_CTRL_REFCNT_FR_CLK);
    ucstatus |= ucDram_Register_Write(mcSET_DRAMC_REG_ADDR(0x1dc), uiTemp);

#if 0
    //FIXDQIEN = 1111 (0xd8[15:12])
    ucstatus |= ucDram_Register_Read(mcSET_DRAMC_REG_ADDR(DRAMC_REG_MCKDLY), &uiTemp);
    uiRegD8h = uiTemp;
    mcSET_MASK(uiTemp, MASK_MCKDLY_FIXDQIEN);
    ucstatus |= ucDram_Register_Write(mcSET_DRAMC_REG_ADDR(DRAMC_REG_MCKDLY), uiTemp);
#endif

    //Enable DQ_O1, SELO1ASO=1
    ucstatus |= ucDram_Register_Read(mcSET_DRAMC_REG_ADDR(DRAMC_REG_LPDDR2), &uiTemp);
    uiReg1E0h = uiTemp;
    mcSET_BIT(uiTemp, POS_LPDDR2_SELO1ASO);
    ucstatus |= ucDram_Register_Write(mcSET_DRAMC_REG_ADDR(DRAMC_REG_LPDDR2), uiTemp);

    CA_TRAINING_BEGIN:
    // For 8163:
    // CA0 - RA8 - 0x1b0 0~3
    // CA1 - RA4 - 0x1ac 0~3
    // CA2 - RA0 - 0x1a8 0~3
    // CA3 - RA3 - 0x1a8 24~27
    // CA4 - RA1 - 0x1a8 8~11
    // CA5 - RA5 - 0x1ac 8~11
    // CA6 - RA9 - 0x1b0 8~11
    // CA7 - RA13 - 0x1bc 16~20
    // CA8 - BA0 - 0x1b4 8~11
    // CA9 - RA15 - 0x1c0 16~20

    // Set CA0~CA3, CA5~CA8 output delay to 0.
    //ucstatus |= ucDram_Register_Write(mcSET_DRAMC_REG_ADDR(0x1a8), 0);    // CA0~CA3
    //ucstatus |= ucDram_Register_Write(mcSET_DRAMC_REG_ADDR(0x1ac), 0);    // CA4~CA7
    //ucstatus |= ucDram_Register_Write(mcSET_DRAMC_REG_ADDR(0x1b0), 0);    // CA8~CA11 //todo:

    // Set CA0 output delay
    ucstatus |= ucDram_Register_Read(mcSET_DRAMC_REG_ADDR(0x1b0), &uiTemp);
    mcCLR_MASK(uiTemp, 0xF);
    mcSET_MASK(uiTemp, 0);
    ucstatus |= ucDram_Register_Write(mcSET_DRAMC_REG_ADDR(0x1b0), uiTemp);

    // Set CA1 output delay
    ucstatus |= ucDram_Register_Read(mcSET_DRAMC_REG_ADDR(0x1ac), &uiTemp);
    mcCLR_MASK(uiTemp, 0xF);
    mcSET_MASK(uiTemp, 0);
    ucstatus |= ucDram_Register_Write(mcSET_DRAMC_REG_ADDR(0x1ac), uiTemp);

    // Set CA2 output delay
    ucstatus |= ucDram_Register_Read(mcSET_DRAMC_REG_ADDR(0x1a8), &uiTemp);
    mcCLR_MASK(uiTemp, 0xF);
    mcSET_MASK(uiTemp, 0);
    ucstatus |= ucDram_Register_Write(mcSET_DRAMC_REG_ADDR(0x1a8), uiTemp);

    // Set CA3 output delay
    ucstatus |= ucDram_Register_Read(mcSET_DRAMC_REG_ADDR(0x1a8), &uiTemp);
    mcCLR_MASK(uiTemp, 0xF<<24);
    mcSET_MASK(uiTemp, 0<<24);
    ucstatus |= ucDram_Register_Write(mcSET_DRAMC_REG_ADDR(0x1a8), uiTemp);

    #if 1 // Set CA4 output delay
    ucstatus |= ucDram_Register_Read(mcSET_DRAMC_REG_ADDR(0x1a8), &uiTemp);
    mcCLR_MASK(uiTemp, 0xF<<8);
    mcSET_MASK(uiTemp, 0<<8);
    ucstatus |= ucDram_Register_Write(mcSET_DRAMC_REG_ADDR(0x1a8), uiTemp);
    #endif

    // Set CA5 output delay
    ucstatus |= ucDram_Register_Read(mcSET_DRAMC_REG_ADDR(0x1ac), &uiTemp);
    mcCLR_MASK(uiTemp, 0xF<<8);
    mcSET_MASK(uiTemp, 0<<8);
    ucstatus |= ucDram_Register_Write(mcSET_DRAMC_REG_ADDR(0x1ac), uiTemp);

    // Set CA6 output delay
    ucstatus |= ucDram_Register_Read(mcSET_DRAMC_REG_ADDR(0x1b0), &uiTemp);
    mcCLR_MASK(uiTemp, 0xF<<8);
    mcSET_MASK(uiTemp, 0<<8);
    ucstatus |= ucDram_Register_Write(mcSET_DRAMC_REG_ADDR(0x1b0), uiTemp);

    // Set CA7 output delay
    ucstatus |= ucDram_Register_Read(mcSET_DRAMC_REG_ADDR(0x1bc), &uiTemp);
    mcCLR_MASK(uiTemp, 0xF<<16);
    mcSET_MASK(uiTemp, 0<<16);
    ucstatus |= ucDram_Register_Write(mcSET_DRAMC_REG_ADDR(0x1bc), uiTemp);

    // Set CA8 output delay
    ucstatus |= ucDram_Register_Read(mcSET_DRAMC_REG_ADDR(0x1b4), &uiTemp);
    mcCLR_MASK(uiTemp, 0xF<<8);
    mcSET_MASK(uiTemp, 0<<8);
    ucstatus |= ucDram_Register_Write(mcSET_DRAMC_REG_ADDR(0x1b4), uiTemp);

    #if 1 // Set CA9 output delay
    ucstatus |= ucDram_Register_Read(mcSET_DRAMC_REG_ADDR(0x1c0), &uiTemp);
    mcCLR_MASK(uiTemp, 0xF<<16);
    mcSET_MASK(uiTemp, 0<<16);
    ucstatus |= ucDram_Register_Write(mcSET_DRAMC_REG_ADDR(0x1c0), uiTemp);
    #endif

    // CS extent enable (need DRAM to support)
    // for testing

    ucstatus |= ucDram_Register_Read(mcSET_DRAMC_REG_ADDR(0x80), &uiTemp);
    mcSET_BIT(uiTemp, 13);
    ucstatus |= ucDram_Register_Write(mcSET_DRAMC_REG_ADDR(0x80), uiTemp);

    CA_PRINT("[CA Training] REG 0x80 value: 0x%x\n", uiTemp);

    // Enter MR 41/MR48
    // Set MA & OP.
    if (uiMR41)
    {
        ucstatus |= ucDram_Register_Write(mcSET_DRAMC_REG_ADDR(DRAMC_REG_MRS), 0x00a40029);
    }
    else
    {
        ucstatus |= ucDram_Register_Write(mcSET_DRAMC_REG_ADDR(DRAMC_REG_MRS), 0x00c00030);
    }
    // Hold the CA bus stable for at least one cycle.
    ucstatus |= ucDram_Register_Read(mcSET_DRAMC_REG_ADDR(0x4c), &uiTemp); //todo:
    CA_PRINT("[CA Training] REG 0x4C value: 0x%x, 1\n", uiTemp);
    mcSET_BIT(uiTemp, 2);
    ucstatus |= ucDram_Register_Write(mcSET_DRAMC_REG_ADDR(0x4c), uiTemp);

    ucDram_Register_Read(mcSET_DRAMC_REG_ADDR(0x4c), &uiTemp);
    CA_PRINT("[CA Training] REG 0x4C value: 0x%x, 2\n", uiTemp);

    // MRW
    ucstatus |= ucDram_Register_Read(mcSET_DRAMC_REG_ADDR(0x1e4), &uiTemp);
    mcSET_BIT(uiTemp, 0);
    ucstatus |= ucDram_Register_Write(mcSET_DRAMC_REG_ADDR(0x1e4), uiTemp);
    mcDELAY_US(1);
    mcCLR_BIT(uiTemp, 0);
    ucstatus |= ucDram_Register_Write(mcSET_DRAMC_REG_ADDR(0x1e4), uiTemp);
    // Disable CA bus stable.
    ucstatus |= ucDram_Register_Read(mcSET_DRAMC_REG_ADDR(0x4c), &uiTemp);
    mcCLR_BIT(uiTemp, 2);
    ucstatus |= ucDram_Register_Write(mcSET_DRAMC_REG_ADDR(0x4c), uiTemp);

    // Wait tCACKEL(10 tck) before CKE low
    mcDELAY_US(1);

    // CKE low
    ucstatus |= ucDram_Register_Read(mcSET_DRAMC_REG_ADDR(0xe4), &uiTemp);
    mcSET_BIT(uiTemp, 3);
    mcCLR_BIT(uiTemp, 2);
    ucstatus |= ucDram_Register_Write(mcSET_DRAMC_REG_ADDR(0xe4), uiTemp);

    ucDram_Register_Read(mcSET_DRAMC_REG_ADDR(0x50), &uiTemp);
    CA_PRINT("[CA Training] REG 0x50 value: 0x%x, 1, new\n", uiTemp);

    // Set CA0~CA3, CA5~CA8 rising/falling golden value.
    if (uiMR41)
    {
        //  01010101b -> 10101010b : Golden value = 1001100110011001b=0x9999
        //  11111111b -> 00000000b : Golden value = 0101010101010101b=0x5555
        ucstatus |= ucDram_Register_Write(mcSET_DRAMC_REG_ADDR(0x50), 0x55555555); //todo:
        //ucstatus |= ucDram_Register_Write(mcSET_DRAMC_REG_ADDR(0x50), 0x00040004);
        //ucstatus |= ucDram_Register_Write(mcSET_DRAMC_REG_ADDR(0x50), 0x0000ffff);
    }
    else
    {
        //  00010001b -> 00000000b : Golden value = 0000000100000001b=0x0101
        ucstatus |= ucDram_Register_Write(mcSET_DRAMC_REG_ADDR(0x50), 0x01010101);
        //ucstatus |= ucDram_Register_Write(mcSET_DRAMC_REG_ADDR(0x50), 0x00010001);
        //ucstatus |= ucDram_Register_Write(mcSET_DRAMC_REG_ADDR(0x50), 0x0000ffff);
    }

    ucDram_Register_Read(mcSET_DRAMC_REG_ADDR(0x50), &uiTemp);
    CA_PRINT("[CA Training] REG 0x50 value: 0x%x, 2\n", uiTemp);

    // Wait tCAENT(10 tck) before pattern output
    mcDELAY_US(1);

    //while(1);

    // Delay clock output delay to do CA training in order to get the pass window.
    uiFinishCount = 0;
    for (uiDelay=0; uiDelay<=MAX_CLKO_DELAY; uiDelay++)
    {
        //DramcEnterSelfRefresh(p, 1);
        // Set Clk output delay
        ucstatus |= ucDram_Register_Read(mcSET_DRAMC_REG_ADDR(0x0c), &uiTemp);
        mcSET_FIELD(uiTemp, uiDelay, 0x0f000000, 24);
        ucstatus |= ucDram_Register_Write(mcSET_DRAMC_REG_ADDR(0x0c), uiTemp);
        //DramcEnterSelfRefresh(p, 0);

        // CA training pattern output enable
        ucstatus |= ucDram_Register_Read(mcSET_DRAMC_REG_ADDR(0x4c), &uiTemp);
        mcSET_BIT(uiTemp, 1);
        ucstatus |= ucDram_Register_Write(mcSET_DRAMC_REG_ADDR(0x4c), uiTemp);
        // delay 2 DRAM clock cycle
        mcDELAY_US(1);
        mcCLR_BIT(uiTemp, 1);
        ucstatus |= ucDram_Register_Write(mcSET_DRAMC_REG_ADDR(0x4c), uiTemp);

        // Wait tADR(20ns) before CA sampled values available in DQ.
        mcDELAY_US(1);

        // Get DQ value.
        ucstatus |= ucDram_Register_Read(mcSET_DRAMC_REG_ADDR(0x380), &uiTemp);

        //mcSHOW_DBG_MSG2(("[CA Training] CLK delay -- %d, MR41 -- %d, DQ_O1 -- 0x%x\n", uiDelay, uiMR41, uiTemp));

        // Compare with golden value.
        if (uiMR41)
        {
            for (uiCA=0; uiCA<CATRAINING_NUM; uiCA++)
            {
                if ((uiCA==4) || (uiCA==9))
                {
                    continue;
                }
                if ( (iFirstClkPass[uiCA]==-1) || (iLastClkPass[uiCA]==-1))
                {
                    if (uiCA<4)
                    {
                        uiRisingEdge = uiTemp & (0x01 << uiLPDDR_PHY_Mapping[uiCA<<1]);
                        uiFallingEdge = uiTemp & (0x01 << uiLPDDR_PHY_Mapping[(uiCA<<1)+1]);
                    }
                    else
                    {
                        uiRisingEdge = uiTemp & (0x01 << uiLPDDR_PHY_Mapping[(uiCA-1)<<1]);
                        uiFallingEdge = uiTemp & (0x01 << uiLPDDR_PHY_Mapping[((uiCA-1)<<1)+1]);
                    }

                    //mcSHOW_DBG_MSG2(("[CA Training] CLK, R - %x, F - %x\n", uiRisingEdge, uiFallingEdge));

                    // Check 1->0 transition.
                    if ((uiRisingEdge!=0) && (uiFallingEdge==0))
                    {
                        iPass = 1;
                        //mcSHOW_DBG_MSG2(("CLK, CA%d, [%x, %x], Pass 1\n", uiCA, uiRisingEdge, uiFallingEdge));
                    }
                    else
                    {
                        iPass = 0;
                    }


                    if (iFirstClkPass[uiCA]==-1)
                    {
                        if (iPass == 1)
                        {
                            iFirstClkPass[uiCA] = uiDelay;
                        }
                    }
                    else
                    {
                        if (iLastClkPass[uiCA]==-1)
                        {
                            if (iPass == 0)
                            {
                                iLastClkPass[uiCA] = uiDelay-1;
                                uiFinishCount++;
                            }
                            else
                            {
                                if (uiDelay==MAX_CLKO_DELAY)
                                {
                                    iLastClkPass[uiCA] = uiDelay;
                                    uiFinishCount++;
                                }
                            }
                            if (iLastClkPass[uiCA]!=-1)
                            {
                            	if ( (iLastClkPass[uiCA]-iFirstClkPass[uiCA]) > (iBestLastClkPass[uiCA]-iBestFirstClkPass[uiCA]))
                            	{
                            		iBestLastClkPass[uiCA] = iLastClkPass[uiCA];
                            		iBestFirstClkPass[uiCA] = iFirstClkPass[uiCA];
                            	}
                            	iLastClkPass[uiCA] = iFirstClkPass[uiCA] = -1;
                            }
                        }
                    }
                }
            }

            // Wait tCACD(22clk) before output CA pattern to DDR again..
            mcDELAY_US(1);

            //if (uiFinishCount==8) {
            //    break;
            //}
        }
        else
        {
            // MR 48
            uiCA = 4;

MR48_CHECKRESULT_CLK:

            if ((iFirstClkPass[uiCA]==-1) || (iLastClkPass[uiCA]==-1))
            {
                uiRisingEdge = uiTemp & (0x01 << uiLPDDR_PHY_Mapping[(uiCA==4) ? 0 : 8]);
                uiFallingEdge = uiTemp & (0x01 << uiLPDDR_PHY_Mapping[(uiCA==4) ? 1 : 9]);

                // Check 1->0 transition.
                if ((uiRisingEdge!=0) && (uiFallingEdge==0))
                {
                    iPass = 1;
                    //mcSHOW_DBG_MSG2(("CLK, CA%d, [%x, %x], Pass 1\n", uiCA, uiRisingEdge, uiFallingEdge));
                }
                else
                {
                    iPass = 0;
                }
                if (iFirstClkPass[uiCA]==-1)
                {
                    if (iPass==1)
                    {
                        iFirstClkPass[uiCA] = uiDelay;
                    }
                }
                else
                {
                    if (iLastClkPass[uiCA]==-1)
                    {
                        if (iPass==0)
                        {
                            iLastClkPass[uiCA] = uiDelay-1;
                            uiFinishCount++;
                        }
                        else
                        {
                            if (uiDelay==MAX_CLKO_DELAY)
                            {
                                iLastClkPass[uiCA] = uiDelay;
                                uiFinishCount++;
                            }
                        }
                        if (iLastClkPass[uiCA]!=-1)
                        {
                        	if ( (iLastClkPass[uiCA]-iFirstClkPass[uiCA]) > (iBestLastClkPass[uiCA]-iBestFirstClkPass[uiCA]))
                        	{
                        		iBestLastClkPass[uiCA] = iLastClkPass[uiCA];
                        		iBestFirstClkPass[uiCA] = iFirstClkPass[uiCA];
                        	}
                        	iLastClkPass[uiCA] = iFirstClkPass[uiCA] = -1;
                        }
                    }
                }
            }

            if (uiCA==4)
            {
                uiCA=9;
                goto MR48_CHECKRESULT_CLK;
            }

            // Wait tCACD(22clk) before output CA pattern to DDR again..
            mcDELAY_US(1);

            //if (uiFinishCount==2) {
            //    break;
            //}
        }
    }

    //DramcEnterSelfRefresh(p, 1);
    // Set Clk output delay to 0.
    ucstatus |= ucDram_Register_Read(mcSET_DRAMC_REG_ADDR(0x0c), &uiTemp);
    mcSET_FIELD(uiTemp, 0, 0x0f000000, 24);
    ucstatus |= ucDram_Register_Write(mcSET_DRAMC_REG_ADDR(0x0c), uiTemp);
    //DramcEnterSelfRefresh(p, 0);

    // Delay CA output delay to do CA training in order to get the pass window.
    uiFinishCount = 0;
    for (uiDelay=0; uiDelay<=MAX_CLKO_DELAY; uiDelay++)
    {
        // Set CA0~CA3, CA5~CA8 output delay.
        #ifdef CA_DELAY_CHAIN
        uiTemp = uiDelay | (uiDelay<<8) | (uiDelay<<16) | (uiDelay<<24);
        ucstatus |= ucDram_Register_Write(mcSET_DRAMC_REG_ADDR(0x1a8), uiTemp | DRAMC_READ_REG(0x1a8));    // CA0~CA3
        ucstatus |= ucDram_Register_Write(mcSET_DRAMC_REG_ADDR(0x1ac), uiTemp | DRAMC_READ_REG(0x1ac));    // CA4~CA7
        ucstatus |= ucDram_Register_Write(mcSET_DRAMC_REG_ADDR(0x1b0), uiTemp | DRAMC_READ_REG(0x1b0));    // CA8~CA11
        ucstatus |= ucDram_Register_Write(mcSET_DRAMC_REG_ADDR(0x1b4), uiTemp | DRAMC_READ_REG(0x1b4));
        ucstatus |= ucDram_Register_Write(mcSET_DRAMC_REG_ADDR(0x1b8), uiTemp | DRAMC_READ_REG(0x1b8));
        ucstatus |= ucDram_Register_Write(mcSET_DRAMC_REG_ADDR(0x1bc), ((uiDelay<<8) | (uiDelay<<16) | (uiDelay<<24)) | DRAMC_READ_REG(0x1bc));
        ucstatus |= ucDram_Register_Write(mcSET_DRAMC_REG_ADDR(0x1c0), ((uiDelay<<16) | (uiDelay<<24)) | DRAMC_READ_REG(0x1c0));
        CA_PRINT("[CA Training] CA delay -- %d, set all\n", uiDelay);
       #else
        #if 0
       ucstatus |= ucDram_Register_Read(mcSET_DRAMC_REG_ADDR(0x1A8), &uiTemp);
       mcSET_FIELD(uiTemp, uiDelay, 0x000f0000, 16);
       ucstatus |= ucDram_Register_Write(mcSET_DRAMC_REG_ADDR(0x1A8), uiTemp);

       ucstatus |= ucDram_Register_Read(mcSET_DRAMC_REG_ADDR(0x1AC), &uiTemp);
       mcSET_FIELD(uiTemp, uiDelay, 0x0f000000, 24);
       ucstatus |= ucDram_Register_Write(mcSET_DRAMC_REG_ADDR(0x1AC), uiTemp);

       ucstatus |= ucDram_Register_Read(mcSET_DRAMC_REG_ADDR(0x1B4), &uiTemp);
       mcSET_FIELD(uiTemp, uiDelay, 0x0f000000, 24);
       mcSET_FIELD(uiTemp, uiDelay, 0x000f0000, 16);
       mcSET_FIELD(uiTemp, uiDelay, 0x00000f00, 8);
       ucstatus |= ucDram_Register_Write(mcSET_DRAMC_REG_ADDR(0x1B4), uiTemp);

       ucstatus |= ucDram_Register_Read(mcSET_DRAMC_REG_ADDR(0x1B8), &uiTemp);
       mcSET_FIELD(uiTemp, uiDelay, 0x0f000000, 24);
       mcSET_FIELD(uiTemp, uiDelay, 0x000f0000, 16);
       ucstatus |= ucDram_Register_Write(mcSET_DRAMC_REG_ADDR(0x1B8), uiTemp);

       ucstatus |= ucDram_Register_Read(mcSET_DRAMC_REG_ADDR(0x1BC), &uiTemp);
       mcSET_FIELD(uiTemp, uiDelay, 0x000f0000, 16);
       mcSET_FIELD(uiTemp, uiDelay, 0x00000f00, 8);
       ucstatus |= ucDram_Register_Write(mcSET_DRAMC_REG_ADDR(0x1BC), uiTemp);

       ucstatus |= ucDram_Register_Read(mcSET_DRAMC_REG_ADDR(0x1C0), &uiTemp);
       mcSET_FIELD(uiTemp, uiDelay, 0x0f000000, 24);
       ucstatus |= ucDram_Register_Write(mcSET_DRAMC_REG_ADDR(0x1C0), uiTemp);
       #endif

        // Set CA0 output delay
        ucstatus |= ucDram_Register_Read(mcSET_DRAMC_REG_ADDR(0x1b0), &uiTemp);
        mcCLR_MASK(uiTemp, 0xF<<0);
        mcSET_MASK(uiTemp, uiDelay<<0);
        ucstatus |= ucDram_Register_Write(mcSET_DRAMC_REG_ADDR(0x1b0), uiTemp);

        // Set CA1 output delay
        ucstatus |= ucDram_Register_Read(mcSET_DRAMC_REG_ADDR(0x1ac), &uiTemp);
        mcCLR_MASK(uiTemp, 0xF<<0);
        mcSET_MASK(uiTemp, uiDelay<<0);
        ucstatus |= ucDram_Register_Write(mcSET_DRAMC_REG_ADDR(0x1ac), uiTemp);

        // Set CA2 output delay
        ucstatus |= ucDram_Register_Read(mcSET_DRAMC_REG_ADDR(0x1a8), &uiTemp);
        mcCLR_MASK(uiTemp, 0xF<<0);
        mcSET_MASK(uiTemp, uiDelay<<0);
        ucstatus |= ucDram_Register_Write(mcSET_DRAMC_REG_ADDR(0x1a8), uiTemp);

        // Set CA3 output delay
        ucstatus |= ucDram_Register_Read(mcSET_DRAMC_REG_ADDR(0x1a8), &uiTemp);
        mcCLR_MASK(uiTemp, 0xF<<24);
        mcSET_MASK(uiTemp, uiDelay<<24);
        ucstatus |= ucDram_Register_Write(mcSET_DRAMC_REG_ADDR(0x1a8), uiTemp);

        #if 1 // Set CA4 output delay
        ucstatus |= ucDram_Register_Read(mcSET_DRAMC_REG_ADDR(0x1a8), &uiTemp);
        mcCLR_MASK(uiTemp, 0xF<<8);
        mcSET_MASK(uiTemp, uiDelay<<8);
        ucstatus |= ucDram_Register_Write(mcSET_DRAMC_REG_ADDR(0x1a8), uiTemp);
        #endif

        // Set CA5 output delay
        ucstatus |= ucDram_Register_Read(mcSET_DRAMC_REG_ADDR(0x1ac), &uiTemp);
        mcCLR_MASK(uiTemp, 0xF<<8);
        mcSET_MASK(uiTemp, uiDelay<<8);
        ucstatus |= ucDram_Register_Write(mcSET_DRAMC_REG_ADDR(0x1ac), uiTemp);

        // Set CA6 output delay
        ucstatus |= ucDram_Register_Read(mcSET_DRAMC_REG_ADDR(0x1b0), &uiTemp);
        mcCLR_MASK(uiTemp, 0xF<<8);
        mcSET_MASK(uiTemp, uiDelay<<8);
        ucstatus |= ucDram_Register_Write(mcSET_DRAMC_REG_ADDR(0x1b0), uiTemp);

        // Set CA7 output delay
        ucstatus |= ucDram_Register_Read(mcSET_DRAMC_REG_ADDR(0x1bc), &uiTemp);
        mcCLR_MASK(uiTemp, 0xF<<16);
        mcSET_MASK(uiTemp, uiDelay<<16);
        ucstatus |= ucDram_Register_Write(mcSET_DRAMC_REG_ADDR(0x1bc), uiTemp);

        // Set CA8 output delay
        ucstatus |= ucDram_Register_Read(mcSET_DRAMC_REG_ADDR(0x1b4), &uiTemp);
        mcCLR_MASK(uiTemp, 0xF<<8);
        mcSET_MASK(uiTemp, uiDelay<<8);
        ucstatus |= ucDram_Register_Write(mcSET_DRAMC_REG_ADDR(0x1b4), uiTemp);

        #if 1 // Set CA9 output delay
        ucstatus |= ucDram_Register_Read(mcSET_DRAMC_REG_ADDR(0x1c0), &uiTemp);
        mcCLR_MASK(uiTemp, 0xF<<16);
        mcSET_MASK(uiTemp, uiDelay<<16);
        ucstatus |= ucDram_Register_Write(mcSET_DRAMC_REG_ADDR(0x1c0), uiTemp);
        #endif
        #endif

        #if 0
        if (1 == uiDelay)
        {
            mcSHOW_DBG_MSG2(("[CA Training] 0x1a8:0x%x\n", DRAMC_READ_REG(0x1a8)));
            mcSHOW_DBG_MSG2(("[CA Training] 0x1ac:0x%x\n", DRAMC_READ_REG(0x1ac)));
            mcSHOW_DBG_MSG2(("[CA Training] 0x1b0:0x%x\n", DRAMC_READ_REG(0x1b0)));
            mcSHOW_DBG_MSG2(("[CA Training] 0x1b4:0x%x\n", DRAMC_READ_REG(0x1b4)));
            mcSHOW_DBG_MSG2(("[CA Training] 0x1b8:0x%x\n", DRAMC_READ_REG(0x1b8)));
            mcSHOW_DBG_MSG2(("[CA Training] 0x1bc:0x%x\n", DRAMC_READ_REG(0x1bc)));
            mcSHOW_DBG_MSG2(("[CA Training] 0x1c0:0x%x\n", DRAMC_READ_REG(0x1c0)));
        }
        #endif

        // CA training pattern output enable
        ucstatus |= ucDram_Register_Read(mcSET_DRAMC_REG_ADDR(0x4c), &uiTemp);
        mcSET_BIT(uiTemp, 1);
        ucstatus |= ucDram_Register_Write(mcSET_DRAMC_REG_ADDR(0x4c), uiTemp);
        // delay 2 DRAM clock cycle
        mcDELAY_US(10);
        mcCLR_BIT(uiTemp, 1);
        ucstatus |= ucDram_Register_Write(mcSET_DRAMC_REG_ADDR(0x4c), uiTemp);

        // Wait tADR(20ns) before CA sampled values available in DQ.
        mcDELAY_US(10);

        // Get DQ value.
        ucstatus |= ucDram_Register_Read(mcSET_DRAMC_REG_ADDR(0x380), &uiTemp);

        //mcSHOW_DBG_MSG2(("[CA Training] CA delay -- %d, MR41 -- %d, DQ_O1 -- 0x%x\n", uiDelay, uiMR41, uiTemp));

        // Compare with golden value.
        if (uiMR41)
        {
            for (uiCA=0; uiCA<CATRAINING_NUM; uiCA++)
            {
                if ((uiCA==4) || (uiCA==9))
                {
                    continue;
                }
                if ( (iFirstCAPass[uiCA]==-1) || (iLastCAPass[uiCA]==-1))
                {
                    if (uiCA<4)
                    {
                        uiRisingEdge = uiTemp & (0x01 << uiLPDDR_PHY_Mapping[uiCA<<1]);
                        uiFallingEdge = uiTemp & (0x01 << uiLPDDR_PHY_Mapping[(uiCA<<1)+1]);
                    }
                    else
                    {
                        uiRisingEdge = uiTemp & (0x01 << uiLPDDR_PHY_Mapping[(uiCA-1)<<1]);
                        uiFallingEdge = uiTemp & (0x01 << uiLPDDR_PHY_Mapping[((uiCA-1)<<1)+1]);
                    }

                    //mcSHOW_DBG_MSG2(("[CA Training] CA, R -- %x, F -- %x\n", uiRisingEdge, uiFallingEdge));

                    // Check 1->0 transition.
                    if ((uiRisingEdge!=0) && (uiFallingEdge==0))
                    {
                        iPass = 1;
                        //mcSHOW_DBG_MSG2(("CA, CA%d, [%x, %x], Pass 1\n", uiCA, uiRisingEdge, uiFallingEdge));
                    }
                    else
                    {
                        iPass = 0;
                    }

                    if (iFirstCAPass[uiCA]==-1)
                    {
                        if (iPass == 1)
                        {
                            iFirstCAPass[uiCA] = uiDelay;
                        }
                    }
                    else
                    {
                        if (iLastCAPass[uiCA]==-1)
                        {
                            if (iPass == 0)
                            {
                                iLastCAPass[uiCA] = uiDelay-1;
                                uiFinishCount++;
                            }
                            else
                            {
                                if (uiDelay==MAX_CLKO_DELAY)
                                {
                                    iLastCAPass[uiCA] = uiDelay;
                                    uiFinishCount++;
                                }
                            }
                            if (iLastCAPass[uiCA]!=-1)
                            {
	                            if ( (iLastCAPass[uiCA]-iFirstCAPass[uiCA]) > (iBestLastCAPass[uiCA]-iBestFirstCAPass[uiCA]) )
	                            {
	                            	iBestLastCAPass[uiCA] = iLastCAPass[uiCA];
	                            	iBestFirstCAPass[uiCA] = iFirstCAPass[uiCA];
	                            }
	                            iLastCAPass[uiCA] = iFirstCAPass[uiCA] = -1;
                            }
                        }
                    }
                }
            }

            // Wait tCACD(22clk) before output CA pattern to DDR again..
            mcDELAY_US(10);

            //if (uiFinishCount==8) {
            //    break;
            //}
        }
        else
        {
            // MR 48
            uiCA = 4;

MR48_CHECKRESULT_CA:

            if ((iFirstCAPass[uiCA]==-1) || (iLastCAPass[uiCA]==-1))
            {
                uiRisingEdge = uiTemp & (0x01 << uiLPDDR_PHY_Mapping[(uiCA==4) ? 0 : 8]);
                uiFallingEdge = uiTemp & (0x01 << uiLPDDR_PHY_Mapping[(uiCA==4) ? 1 : 9]);

                // Check 1->0 transition.
                if ((uiRisingEdge!=0) && (uiFallingEdge==0))
                {
                    iPass = 1;
                    //mcSHOW_DBG_MSG2(("CA, CA%d, [%x, %x], Pass 1\n", uiCA, uiRisingEdge, uiFallingEdge));
                }
                else
                {
                    iPass = 0;
                }

                if (iFirstCAPass[uiCA]==-1)
                {
                    if (iPass==1)
                    {
                        iFirstCAPass[uiCA] = uiDelay;
                    }
                }
                else
                {
                    if (iLastCAPass[uiCA]==-1)
                    {
                        if (iPass==0)
                        {
                            iLastCAPass[uiCA] = uiDelay-1;
                            uiFinishCount++;
                        }
                        else
                        {
                            if (uiDelay==MAX_CLKO_DELAY)
                            {
                                iLastCAPass[uiCA] = uiDelay;
                                uiFinishCount++;
                            }
                        }
                        if (iLastCAPass[uiCA]!=-1)
                        {
                            if ( (iLastCAPass[uiCA]-iFirstCAPass[uiCA]) > (iBestLastCAPass[uiCA]-iBestFirstCAPass[uiCA]) )
                            {
                            	iBestLastCAPass[uiCA] = iLastCAPass[uiCA];
                            	iBestFirstCAPass[uiCA] = iFirstCAPass[uiCA];
                            }
                            iLastCAPass[uiCA] = iFirstCAPass[uiCA] = -1;
                        }
                    }
                }
            }

            if (uiCA==4)
            {
                uiCA=9;
                goto MR48_CHECKRESULT_CA;
            }

            // Wait tCACD(22clk) before output CA pattern to DDR again..
            mcDELAY_US(10);

            //if (uiFinishCount==2) {
            //    break;
            //}
        }
    }

    // CS extent disable
    // for testing
    ucstatus |= ucDram_Register_Read(mcSET_DRAMC_REG_ADDR(0x80), &uiTemp);
    mcCLR_BIT(uiTemp, 13);
    ucstatus |= ucDram_Register_Write(mcSET_DRAMC_REG_ADDR(0x80), uiTemp);

    if (uiMR41==0)
    {
        // Disable fix DQ input enable.
        ucstatus |= ucDram_Register_Read(mcSET_DRAMC_REG_ADDR(0xd8), &uiTemp);
        uiTemp = uiTemp & 0xffff0fff;
        ucstatus |= ucDram_Register_Write(mcSET_DRAMC_REG_ADDR(0xd8), uiTemp);
    }

    // Wait tCACKEN (10ck)
    mcDELAY_US(10);

    // CKE high
    ucstatus |= ucDram_Register_Read(mcSET_DRAMC_REG_ADDR(0xe4), &uiTemp);
    mcCLR_BIT(uiTemp, 3);
    mcSET_BIT(uiTemp, 2);
    ucstatus |= ucDram_Register_Write(mcSET_DRAMC_REG_ADDR(0xe4), uiTemp);

    if (uiMR41)
    {
        uiMR41 = 0;
        goto CA_TRAINING_BEGIN;
    }

    // CS extent enable
    // for testing
    ucstatus |= ucDram_Register_Read(mcSET_DRAMC_REG_ADDR(0x80), &uiTemp);
    mcSET_BIT(uiTemp, 13);
    ucstatus |= ucDram_Register_Write(mcSET_DRAMC_REG_ADDR(0x80), uiTemp);

    // MR42 to leave CA training.
    ucstatus |= ucDram_Register_Write(mcSET_DRAMC_REG_ADDR(DRAMC_REG_MRS), 0x00a8002a);
    // Hold the CA bus stable for at least one cycle.
    ucstatus |= ucDram_Register_Read(mcSET_DRAMC_REG_ADDR(0x4c), &uiTemp);
    mcSET_BIT(uiTemp, 2);
    ucstatus |= ucDram_Register_Write(mcSET_DRAMC_REG_ADDR(0x4c), uiTemp);
    // MRW
    ucstatus |= ucDram_Register_Read(mcSET_DRAMC_REG_ADDR(0x1e4), &uiTemp);
    mcSET_BIT(uiTemp, 0);
    ucstatus |= ucDram_Register_Write(mcSET_DRAMC_REG_ADDR(0x1e4), uiTemp);
    mcDELAY_US(1);
    mcCLR_BIT(uiTemp, 0);
    ucstatus |= ucDram_Register_Write(mcSET_DRAMC_REG_ADDR(0x1e4), uiTemp);
    // Disable the hold the CA bus stable for at least one cycle.
    ucstatus |= ucDram_Register_Read(mcSET_DRAMC_REG_ADDR(0x4c), &uiTemp);
    mcCLR_BIT(uiTemp, 2);
    ucstatus |= ucDram_Register_Write(mcSET_DRAMC_REG_ADDR(0x4c), uiTemp);

    // CS extent disable
    // for testing
    ucstatus |= ucDram_Register_Read(mcSET_DRAMC_REG_ADDR(0x80), &uiTemp);
    mcCLR_BIT(uiTemp, 13);
    ucstatus |= ucDram_Register_Write(mcSET_DRAMC_REG_ADDR(0x80), uiTemp);

    // Disable CKE high
    ucstatus |= ucDram_Register_Read(mcSET_DRAMC_REG_ADDR(0xe4), &uiTemp);
    mcCLR_BIT(uiTemp, 2);
    ucstatus |= ucDram_Register_Write(mcSET_DRAMC_REG_ADDR(0xe4), uiTemp);

    // Calculate the middle range & max middle.
    CA_PRINT("=========================================\n");
    CA_PRINT("1. CA training window before adjustment. \n");
    CA_PRINT("x=Pass window CA(max~min) Clk(min~max) center. \n");
    CA_PRINT("y=CA0~CA9\n");
    CA_PRINT("=========================================\n");

    mcFPRINTF((fp_A60808, "=========================================\n"));
    mcFPRINTF((fp_A60808, "1. CA training window before adjustment.\n"));
    mcFPRINTF((fp_A60808, "x=Pass window CA(max~min) Clk(min~max) center. \n"));
    mcFPRINTF((fp_A60808, "y=CA0~CA9\n"));
    mcFPRINTF((fp_A60808, "=========================================\n"));

    iMaxCenter = 0 - MAX_CLKO_DELAY;
    for (uiCA=0; uiCA<CATRAINING_NUM; uiCA++)
    {
        iClkWinSize = iBestLastClkPass[uiCA] - iBestFirstClkPass[uiCA];
        iCAWinSize = iBestLastCAPass[uiCA] - iBestFirstCAPass[uiCA];
        if (iClkWinSize >= iCAWinSize)
        {
            if (iCAWinSize>0)
            {
                iCenter[uiCA] =  (iClkWinSize - iCAWinSize)>>1;
            }
            else
            {
                iCenter[uiCA] =  iBestFirstClkPass[uiCA] + (iClkWinSize>>1);
            }
        }
        else
        {
            if (iClkWinSize>0)
            {
                iCenter[uiCA] =  (iClkWinSize - iCAWinSize)/2;
            }
            else
            {
                iCenter[uiCA] =  0-(iBestFirstCAPass[uiCA] + (iCAWinSize>>1));
            }
        }
        CA_PRINT("CA%d     CA(%d~%d) Clk(%d~%d) %d\n", uiCA, iBestLastCAPass[uiCA],  iBestFirstCAPass[uiCA],
            iBestFirstClkPass[uiCA], iBestLastClkPass[uiCA], iCenter[uiCA]);

        mcFPRINTF((fp_A60808, "CA%d     CA(%d~%d) Clk(%d~%d) %d\n", uiCA, iBestLastCAPass[uiCA],  iBestFirstCAPass[uiCA],
            iBestFirstClkPass[uiCA], iBestLastClkPass[uiCA], iCenter[uiCA]));

        if (iCenter[uiCA]  > iMaxCenter)
        {
            iMaxCenter = iCenter[uiCA];
        }
#ifdef EYE_SCAN
	EyeScanWin[uiCA].ucsetup_pass_number = iCAWinSize;
	EyeScanWin[uiCA].uchold_pass_number = iClkWinSize;
#endif

    #ifdef DDR_FT_LOAD_BOARD
        if ((iCAWinSize==0) && (iClkWinSize == 0))
        {
            LoadBoardShowResult(FLAG_CA_CALIBRATION, FLAG_CALIBRATION_FAIL, p->channel, FLAG_NOT_COMPLETE_OR_FAIL);
            while(1);
        }
        else if ((iCAWinSize+iClkWinSize)<=CA_TRAINING_BOUND)
        {
            LoadBoardShowResult(FLAG_CA_CALIBRATION, FLAG_WINDOW_TOO_SMALL, p->channel, FLAG_NOT_COMPLETE_OR_FAIL);
            while(1);
        }
    #endif
    }
    CA_PRINT("=========================================\n");
    CA_PRINT("Max center = %d\n\n", iMaxCenter);
    mcFPRINTF((fp_A60808, "=========================================\n"));
    mcFPRINTF((fp_A60808, "Max center = %d\n\n", iMaxCenter));

    // Calculate the shift value
    CA_PRINT("=========================================\n");
    CA_PRINT("2. CA training window after adjustment.\n");
    CA_PRINT("x=CA shift     Adjusted Pass window CA(max~min) Clk(min~max) center.\n");
    CA_PRINT("y=CA0~CA9\n");
    CA_PRINT("=========================================\n");

    mcFPRINTF((fp_A60808, "=========================================\n"));
    mcFPRINTF((fp_A60808, "2. [Channel %d] CA training window after adjustment.\n", p->channel));
    mcFPRINTF((fp_A60808, "x=CA shift     Adjusted Pass window CA(max~min) Clk(min~max) center.\n"));
    mcFPRINTF((fp_A60808, "y=CA0~CA9\n"));
    mcFPRINTF((fp_A60808, "=========================================\n"));


    if (iMaxCenter < 0)
    {
        // Clk output delay could not be negative. Need to adjust into 0.
        iMaxCenter = 0;
        CA_PRINT(("Max center < 0. Adjust to 0. \n\n"));
    }

    for (uiCA=0; uiCA<CATRAINING_NUM; uiCA++)
    {
        iCAShift[uiCA] = iMaxCenter - iCenter[uiCA]+CATRAINING_STEP;
        if (iCAShift[uiCA]>=MAX_CLKO_DELAY)
        {
            iCAShift[uiCA] = MAX_CLKO_DELAY;
        }
#ifdef CKE_CS_DLY_SETTING
        CAShift_Avg += iCAShift[uiCA];
#endif
        CA_PRINT("CA%d     Shift %d     CA(%d~%d) Clk(%d~%d) %d\n", uiCA, iCAShift[uiCA],
            iBestLastCAPass[uiCA]-iCAShift[uiCA], iBestFirstCAPass[uiCA],
            iBestFirstClkPass[uiCA], iBestLastClkPass[uiCA]+iCAShift[uiCA], iCenter[uiCA]+iCAShift[uiCA]);

        mcFPRINTF((fp_A60808, "CA%d     Shift %d     CA(%d~%d) Clk(%d~%d) %d\n", uiCA, iCAShift[uiCA],
            iBestLastCAPass[uiCA]-iCAShift[uiCA], iBestFirstCAPass[uiCA],
            iBestFirstClkPass[uiCA], iBestLastClkPass[uiCA]+iCAShift[uiCA], iCenter[uiCA]+iCAShift[uiCA]));
    }
    CA_PRINT("=========================================\n");
    mcFPRINTF((fp_A60808, "=========================================\n"));

    // Restore the registers' values.
    ucstatus |= ucDram_Register_Write(mcSET_DRAMC_REG_ADDR(0x1dc), uiReg1DCh);
    ucstatus |= ucDram_Register_Write(mcSET_DRAMC_REG_ADDR(DRAMC_REG_LPDDR2), uiReg1E0h);
    //ucstatus |= ucDram_Register_Write(mcSET_DRAMC_REG_ADDR(DRAMC_REG_MCKDLY), uiRegD8h);

    // Write shift value into CA output delay.
#ifdef CA_DELAY_CHAIN
    uiTemp = iCAShift[0] + (iCAShift[1]<<8) + (iCAShift[2]<<16) + (iCAShift[3]<<24);
    ucstatus |= ucDram_Register_Write(mcSET_DRAMC_REG_ADDR(0x1a8), uiTemp);    // CA0~CA3
    CA_PRINT("Reg.1A8h=%xh\n", uiTemp);
    mcFPRINTF((fp_A60808, "Reg.1A8h=%xh\n", uiTemp));
    uiTemp = iCAShift[4] + (iCAShift[5]<<8) + (iCAShift[6]<<16) + (iCAShift[7]<<24);
    ucstatus |= ucDram_Register_Write(mcSET_DRAMC_REG_ADDR(0x1ac), uiTemp);    // CA4~CA7
    CA_PRINT("Reg.1ACh=%xh\n", uiTemp);
    mcFPRINTF((fp_A60808, "Reg.1ACh=%xh\n", uiTemp));
    uiTemp = iCAShift[8] + (iCAShift[9]<<8);
    ucstatus |= ucDram_Register_Write(mcSET_DRAMC_REG_ADDR(0x1b0), uiTemp);    // CA8~CA11
    CA_PRINT("Reg.1B0h=%xh\n", uiTemp);
    mcFPRINTF((fp_A60808, "Reg.1B0h=%xh\n", uiTemp));
#else

    #if 1 //todo: tmp code
    for (uiCA=0; uiCA<CATRAINING_NUM; uiCA++)
    {
        print("CA%d: %x ->", uiCA, iCAShift[uiCA]);

            //iCAShift[uiCA] += 2;

        print("%x\n", iCAShift[uiCA]);
    }
    #endif

    #if 1 //8163
    // Set CA0 output delay
    ucstatus |= ucDram_Register_Read(mcSET_DRAMC_REG_ADDR(0x1b0), &uiTemp);
    mcCLR_MASK(uiTemp, 0xF<<0);
    mcSET_MASK(uiTemp, iCAShift[0]<<0);
    ucstatus |= ucDram_Register_Write(mcSET_DRAMC_REG_ADDR(0x1b0), uiTemp);

    // Set CA1 output delay
    ucstatus |= ucDram_Register_Read(mcSET_DRAMC_REG_ADDR(0x1ac), &uiTemp);
    mcCLR_MASK(uiTemp, 0xF<<0);
    mcSET_MASK(uiTemp, iCAShift[1]<<0);
    ucstatus |= ucDram_Register_Write(mcSET_DRAMC_REG_ADDR(0x1ac), uiTemp);

    // Set CA2 output delay
    ucstatus |= ucDram_Register_Read(mcSET_DRAMC_REG_ADDR(0x1a8), &uiTemp);
    mcCLR_MASK(uiTemp, 0xF<<0);
    mcSET_MASK(uiTemp, iCAShift[2]<<0);
    ucstatus |= ucDram_Register_Write(mcSET_DRAMC_REG_ADDR(0x1a8), uiTemp);

    // Set CA3 output delay
    ucstatus |= ucDram_Register_Read(mcSET_DRAMC_REG_ADDR(0x1a8), &uiTemp);
    mcCLR_MASK(uiTemp, 0xF<<24);
    mcSET_MASK(uiTemp, iCAShift[3]<<24);
    ucstatus |= ucDram_Register_Write(mcSET_DRAMC_REG_ADDR(0x1a8), uiTemp);

    // Set CA4 output delay
    ucstatus |= ucDram_Register_Read(mcSET_DRAMC_REG_ADDR(0x1a8), &uiTemp);
    mcCLR_MASK(uiTemp, 0xF<<8);
    mcSET_MASK(uiTemp, iCAShift[4]<<8);
    ucstatus |= ucDram_Register_Write(mcSET_DRAMC_REG_ADDR(0x1a8), uiTemp);

    // Set CA5 output delay
    ucstatus |= ucDram_Register_Read(mcSET_DRAMC_REG_ADDR(0x1ac), &uiTemp);
    mcCLR_MASK(uiTemp, 0xF<<8);
    mcSET_MASK(uiTemp, iCAShift[5]<<8);
    ucstatus |= ucDram_Register_Write(mcSET_DRAMC_REG_ADDR(0x1ac), uiTemp);

    // Set CA6 output delay
    ucstatus |= ucDram_Register_Read(mcSET_DRAMC_REG_ADDR(0x1b0), &uiTemp);
    mcCLR_MASK(uiTemp, 0xF<<8);
    mcSET_MASK(uiTemp, iCAShift[6]<<8);
    ucstatus |= ucDram_Register_Write(mcSET_DRAMC_REG_ADDR(0x1b0), uiTemp);

    // Set CA7 output delay
    ucstatus |= ucDram_Register_Read(mcSET_DRAMC_REG_ADDR(0x1bc), &uiTemp);
    mcCLR_MASK(uiTemp, 0xF<<16);
    mcSET_MASK(uiTemp, iCAShift[7]<<16);
    ucstatus |= ucDram_Register_Write(mcSET_DRAMC_REG_ADDR(0x1bc), uiTemp);

    // Set CA8 output delay
    ucstatus |= ucDram_Register_Read(mcSET_DRAMC_REG_ADDR(0x1b4), &uiTemp);
    mcCLR_MASK(uiTemp, 0xF<<8);
    mcSET_MASK(uiTemp, iCAShift[8]<<8);
    ucstatus |= ucDram_Register_Write(mcSET_DRAMC_REG_ADDR(0x1b4), uiTemp);

    // Set CA9 output delay
    ucstatus |= ucDram_Register_Read(mcSET_DRAMC_REG_ADDR(0x1c0), &uiTemp);
    mcCLR_MASK(uiTemp, 0xF<<16);
    mcSET_MASK(uiTemp, iCAShift[9]<<16);
    ucstatus |= ucDram_Register_Write(mcSET_DRAMC_REG_ADDR(0x1c0), uiTemp);
    #else
	ucstatus |= ucDram_Register_Read(mcSET_DRAMC_REG_ADDR(0x1A8), &uiTemp);
	mcSET_FIELD(uiTemp, iCAShift[7] , 0x000f0000, 16);
	ucstatus |= ucDram_Register_Write(mcSET_DRAMC_REG_ADDR(0x1A8), uiTemp);

	ucstatus |= ucDram_Register_Read(mcSET_DRAMC_REG_ADDR(0x1AC), &uiTemp);
	mcSET_FIELD(uiTemp, iCAShift[5] , 0x0f000000, 24);
	ucstatus |= ucDram_Register_Write(mcSET_DRAMC_REG_ADDR(0x1AC), uiTemp);

	ucstatus |= ucDram_Register_Read(mcSET_DRAMC_REG_ADDR(0x1B4), &uiTemp);
	mcSET_FIELD(uiTemp, iCAShift[6] , 0x00000f00, 8);
	mcSET_FIELD(uiTemp, iCAShift[1] , 0x000f0000, 16);
	mcSET_FIELD(uiTemp, iCAShift[3] , 0x0f000000, 24);
	ucstatus |= ucDram_Register_Write(mcSET_DRAMC_REG_ADDR(0x1B4), uiTemp);

	ucstatus |= ucDram_Register_Read(mcSET_DRAMC_REG_ADDR(0x1B8), &uiTemp);
	mcSET_FIELD(uiTemp, iCAShift[0] , 0x000f0000, 16);
	mcSET_FIELD(uiTemp, iCAShift[9] , 0x0f000000, 24);
	ucstatus |= ucDram_Register_Write(mcSET_DRAMC_REG_ADDR(0x1B8), uiTemp);

	ucstatus |= ucDram_Register_Read(mcSET_DRAMC_REG_ADDR(0x1BC), &uiTemp);
	mcSET_FIELD(uiTemp, iCAShift[8] , 0x000f0000, 16);
	mcSET_FIELD(uiTemp, iCAShift[2] , 0x00000f00, 8);
	ucstatus |= ucDram_Register_Write(mcSET_DRAMC_REG_ADDR(0x1BC), uiTemp);

	ucstatus |= ucDram_Register_Read(mcSET_DRAMC_REG_ADDR(0x1C0), &uiTemp);
	mcSET_FIELD(uiTemp, iCAShift[4] , 0x0f000000, 24);
	ucstatus |= ucDram_Register_Write(mcSET_DRAMC_REG_ADDR(0x1C0), uiTemp);
    #endif
#endif

#ifdef CKE_CS_DLY_SETTING
    //CAShift_Avg = (CAShift_Avg + (CATRAINING_NUM>>1)) /CATRAINING_NUM;
    //u4CAshift_Avg = (CAShift_Avg + (CATRAINING_NUM>>1))/CATRAINING_NUM;
	u4CAshift_Avg = (U32)(CAShift_Avg + (CATRAINING_NUM>>1)) /CATRAINING_NUM;

    // CKEDLY : Reg.1B8h[12:8].
    // CSDLY : Reg.1B8h[4:0]
    ucstatus |= ucDram_Register_Read(mcSET_DRAMC_REG_ADDR(0x1b8), &uiTemp);
    uiTemp = (uiTemp & 0xffffe0e0) | (u4CAshift_Avg << 8) | (u4CAshift_Avg << 0);
    ucstatus |= ucDram_Register_Write(mcSET_DRAMC_REG_ADDR(0x1b8), uiTemp);
    CA_PRINT("Reg.1B8h=%xh\n", uiTemp);

    // CKE1DLY : Reg.1C4h[28:24]
    ucstatus |= ucDram_Register_Read(mcSET_DRAMC_REG_ADDR(0x1c4), &uiTemp);
    uiTemp = (uiTemp & 0xe0ffffff) | (u4CAshift_Avg << 24);
    ucstatus |= ucDram_Register_Write(mcSET_DRAMC_REG_ADDR(0x1c4), uiTemp);
    CA_PRINT("Reg.1C4h=%xh\n", uiTemp);

    // CS1DLY : Reg.0Ch[31:28]
    ucstatus |= ucDram_Register_Read(mcSET_DRAMC_REG_ADDR(0x00c), &uiTemp);
    uiTemp = (uiTemp & 0x0fffffff) | (u4CAshift_Avg << 28);
    ucstatus |= ucDram_Register_Write(mcSET_DRAMC_REG_ADDR(0x00c), uiTemp);
    CA_PRINT("Reg.00ch=%xh\n", uiTemp);
#endif

    DramcEnterSelfRefresh(1);
    // Write max center value into Clk output delay.
    ucstatus |= ucDram_Register_Read(mcSET_DRAMC_REG_ADDR(0x0c), &uiTemp);
    mcSET_FIELD(uiTemp, iMaxCenter, 0x0f000000, 24);
    ucstatus |= ucDram_Register_Write(mcSET_DRAMC_REG_ADDR(0x0c), uiTemp);
    CA_PRINT("Reg.0Ch=%xh\n", uiTemp);
    mcFPRINTF((fp_A60808, "Reg.0Ch=%xh\n", uiTemp));
    DramcEnterSelfRefresh(0);
#ifdef WL_CLKADJUST
    CATrain_ClkDelay[0] = iMaxCenter;
    CA_PRINT(("CATrain_ClkDelay=%d...\n", CATrain_ClkDelay[0]));
#endif

    CA_PRINT("0x1a8=%xh\n", DRAMC_READ_REG(0x1a8));
    CA_PRINT("0x1ac=%xh\n", DRAMC_READ_REG(0x1ac));
    CA_PRINT("0x1b0=%xh\n", DRAMC_READ_REG(0x1b0));
    CA_PRINT("0x1b4=%xh\n", DRAMC_READ_REG(0x1b4));
    CA_PRINT("0x1b8=%xh\n", DRAMC_READ_REG(0x1b8));
    CA_PRINT("0x1bc=%xh\n", DRAMC_READ_REG(0x1bc));
    CA_PRINT("0x1c0=%xh\n", DRAMC_READ_REG(0x1c0));
    CA_PRINT("0x1c4=%xh\n", DRAMC_READ_REG(0x1c4));
    CA_PRINT("0x00c=%xh\n", DRAMC_READ_REG(0xc));

    ucstatus |= ucDram_Register_Write(mcSET_DRAMC_REG_ADDR(0x0fc), u4prv_register_0fc);
    ucstatus |= ucDram_Register_Write(mcSET_DRAMC_REG_ADDR(DRAMC_REG_TEST2_3), u4prv_register_044);
    ucstatus |= ucDram_Register_Write(mcSET_DRAMC_REG_ADDR(0x63c), u4prv_register_63c);

    ucDram_Register_Write(mcSET_DRAMC_REG_ADDR(DRAMC_REG_MRS), u4MRSValue);

    if (ucstatus)
    {
        CA_PRINT("register access fail!\n");
        return -1;
    }
    else
    {
        return 0;
    }
}


const U32 uiLPDDR_PHY_Mapping_POP_178Ball[16] = {11, 9, 13, 15, 10, 8, 14, 12,
                                             31, 28, 26, 25, 29, 30, 27, 24,
                                            };

int DramcCATraining_178Ball(void)
{
    U8 ucstatus = 0;
    U32 uiTemp, uiDelay, uiFinishCount, uiCA, uiMR41=1, uiReg1DCh, uiReg1E0h, uiRisingEdge, uiFallingEdge;
    U32 u4prv_register_0fc, u4prv_register_044, u4prv_register_63c;

    S8 iCenter[CATRAINING_NUM],  iFirstClkPass[CATRAINING_NUM], iLastClkPass[CATRAINING_NUM];
    S8 iFirstCAPass[CATRAINING_NUM], iLastCAPass[CATRAINING_NUM], iMaxCenter;
    S8 iCAShift[CATRAINING_NUM];
#ifdef CKE_CS_DLY_SETTING
    S8 CAShift_Avg = 0;
    U32 u4CAshift_Avg = 0x00;
#endif
    S8 iBestFirstClkPass[CATRAINING_NUM], iBestLastClkPass[CATRAINING_NUM];
    S8 iBestFirstCAPass[CATRAINING_NUM], iBestLastCAPass[CATRAINING_NUM];
    S32 iPass, iClkWinSize, iCAWinSize;
    U32 *uiLPDDR_PHY_Mapping;
    U32 u4MRSValue = 0;

    // error handling
    if (TYPE_LPDDR3 != mt_get_dram_type())
    {
        CA_PRINT("Wrong DRAM TYPE. Only support LPDDR3 in CA training!!\n");
        return -1;
    }

    ucDram_Register_Read(mcSET_DRAMC_REG_ADDR(DRAMC_REG_MRS), &u4MRSValue);
    CA_PRINT("MRS Register 0x%x value: 0x%x\n", DRAMC_REG_MRS, u4MRSValue);

    // Disable clock gating to prevent DDRPHY enter idle.
    ucstatus |= ucDram_Register_Read(mcSET_DRAMC_REG_ADDR(0x63c), &uiTemp); //todo: 0x63c
    u4prv_register_63c = uiTemp;
    mcCLR_BIT(uiTemp, 2);
    mcCLR_BIT(uiTemp, 1);
    ucstatus |= ucDram_Register_Write(mcSET_DRAMC_REG_ADDR(0x63c), uiTemp);

    // Edward : no idea why TXP>1 will cause CA training fail. Now set it after CA training.
    ucstatus |= ucDram_Register_Read(mcSET_DRAMC_REG_ADDR(0x0fc), &u4prv_register_0fc);
    ucstatus |= ucDram_Register_Write(mcSET_DRAMC_REG_ADDR(0x0fc), (u4prv_register_0fc & 0x8fffffff));

    //disable auto refresh: REFCNT_FR_CLK = 0 (0x1dc[23:16]), ADVREFEN = 0 (0x44[30])
    ucstatus |= ucDram_Register_Read(mcSET_DRAMC_REG_ADDR(DRAMC_REG_TEST2_3), &uiTemp);
    u4prv_register_044 = uiTemp;
    mcCLR_BIT(uiTemp, POS_TEST2_3_ADVREFEN);
    ucstatus |= ucDram_Register_Write(mcSET_DRAMC_REG_ADDR(DRAMC_REG_TEST2_3), uiTemp);

	// todo: map
  	uiLPDDR_PHY_Mapping = (U32 *)uiLPDDR_PHY_Mapping_POP_178Ball;

    for (uiCA=0; uiCA<CATRAINING_NUM; uiCA++)
    {
        iLastClkPass[uiCA] = iLastCAPass[uiCA] = -1;
        iFirstClkPass[uiCA] = iFirstCAPass[uiCA] = -1;
        iBestLastClkPass[uiCA] = iBestLastCAPass[uiCA] = -1;
        iBestFirstClkPass[uiCA] = iBestFirstCAPass[uiCA] = -1;
    }

    // Sweep clock output delay first.

    // Keep DQ input always ON.
    ucstatus |= ucDram_Register_Read(mcSET_DRAMC_REG_ADDR(0xd8), &uiTemp);

	mcSET_MASK(uiTemp, MASK_MCKDLY_FIXDQIEN);

    ucstatus |= ucDram_Register_Write(mcSET_DRAMC_REG_ADDR(0xd8), uiTemp);

    // Let MIO_CK always ON.
    // Disable auto refresh: REFCNT_FR_CLK = 0 (0x1dc[23:16])
    ucstatus |= ucDram_Register_Read(mcSET_DRAMC_REG_ADDR(0x1dc), &uiTemp);
    uiReg1DCh = uiTemp;
    mcSET_BIT(uiTemp, 26);
    mcCLR_MASK(uiTemp, MASK_DRAMC_PD_CTRL_REFCNT_FR_CLK);
    ucstatus |= ucDram_Register_Write(mcSET_DRAMC_REG_ADDR(0x1dc), uiTemp);

#if 0
    //FIXDQIEN = 1111 (0xd8[15:12])
    ucstatus |= ucDram_Register_Read(mcSET_DRAMC_REG_ADDR(DRAMC_REG_MCKDLY), &uiTemp);
    uiRegD8h = uiTemp;
    mcSET_MASK(uiTemp, MASK_MCKDLY_FIXDQIEN);
    ucstatus |= ucDram_Register_Write(mcSET_DRAMC_REG_ADDR(DRAMC_REG_MCKDLY), uiTemp);
#endif

    //Enable DQ_O1, SELO1ASO=1
    ucstatus |= ucDram_Register_Read(mcSET_DRAMC_REG_ADDR(DRAMC_REG_LPDDR2), &uiTemp);
    uiReg1E0h = uiTemp;
    mcSET_BIT(uiTemp, POS_LPDDR2_SELO1ASO);
    ucstatus |= ucDram_Register_Write(mcSET_DRAMC_REG_ADDR(DRAMC_REG_LPDDR2), uiTemp);

    CA_TRAINING_BEGIN:
    // For 8163 178ball:
    // CA0 - RA12 - 0x1b4 0~3
    // CA1 - RA7 - 0x1ac 24~27
    // CA2 - RA11 - 0x1b0 24~27
    // CA3 - RA3 - 0x1a8 24~27
    // CA4 - RA0 - 0x1a8 0~3
    // CA5 - RA5 - 0x1ac 8~11
    // CA6 - RA9 - 0x1b0 8~11
    // CA7 - RA13 - 0x1bc 16~20
    // CA8 - RA15- 0x1c0 16~20
    // CA9 - RA2 - 0x1a8 16~19



    // Set CA0~CA3, CA5~CA8 output delay to 0.
    //ucstatus |= ucDram_Register_Write(mcSET_DRAMC_REG_ADDR(0x1a8), 0);    // CA0~CA3
    //ucstatus |= ucDram_Register_Write(mcSET_DRAMC_REG_ADDR(0x1ac), 0);    // CA4~CA7
    //ucstatus |= ucDram_Register_Write(mcSET_DRAMC_REG_ADDR(0x1b0), 0);    // CA8~CA11 //todo:

    // Set CA0 output delay
    // CA0 - RA12 - 0x1b4 0~3
    ucstatus |= ucDram_Register_Read(mcSET_DRAMC_REG_ADDR(0x1b4), &uiTemp);
    mcCLR_MASK(uiTemp, 0xF);
    mcSET_MASK(uiTemp, 0);
    ucstatus |= ucDram_Register_Write(mcSET_DRAMC_REG_ADDR(0x1b4), uiTemp);

    // Set CA1 output delay
    // CA1 - RA7 - 0x1ac 24~27
    ucstatus |= ucDram_Register_Read(mcSET_DRAMC_REG_ADDR(0x1ac), &uiTemp);
    mcCLR_MASK(uiTemp, 0xF<<24);
    mcSET_MASK(uiTemp, 0<<24);
    ucstatus |= ucDram_Register_Write(mcSET_DRAMC_REG_ADDR(0x1ac), uiTemp);

    // Set CA2 output delay
    // CA2 - RA11 - 0x1b0 24~27
    ucstatus |= ucDram_Register_Read(mcSET_DRAMC_REG_ADDR(0x1b0), &uiTemp);
    mcCLR_MASK(uiTemp, 0xF<<24);
    mcSET_MASK(uiTemp, 0<<24);
    ucstatus |= ucDram_Register_Write(mcSET_DRAMC_REG_ADDR(0x1b0), uiTemp);

    // Set CA3 output delay
    // CA3 - RA3 - 0x1a8 24~27
    ucstatus |= ucDram_Register_Read(mcSET_DRAMC_REG_ADDR(0x1a8), &uiTemp);
    mcCLR_MASK(uiTemp, 0xF<<24);
    mcSET_MASK(uiTemp, 0<<24);
    ucstatus |= ucDram_Register_Write(mcSET_DRAMC_REG_ADDR(0x1a8), uiTemp);

    #if 1 // Set CA4 output delay
    // CA4 - RA0 - 0x1a8 0~3
    ucstatus |= ucDram_Register_Read(mcSET_DRAMC_REG_ADDR(0x1a8), &uiTemp);
    mcCLR_MASK(uiTemp, 0xF);
    mcSET_MASK(uiTemp, 0);
    ucstatus |= ucDram_Register_Write(mcSET_DRAMC_REG_ADDR(0x1a8), uiTemp);
    #endif

    // Set CA5 output delay
    // CA5 - RA5 - 0x1ac 8~11
    ucstatus |= ucDram_Register_Read(mcSET_DRAMC_REG_ADDR(0x1ac), &uiTemp);
    mcCLR_MASK(uiTemp, 0xF<<8);
    mcSET_MASK(uiTemp, 0<<8);
    ucstatus |= ucDram_Register_Write(mcSET_DRAMC_REG_ADDR(0x1ac), uiTemp);

    // Set CA6 output delay
    // CA6 - RA9 - 0x1b0 8~11
    ucstatus |= ucDram_Register_Read(mcSET_DRAMC_REG_ADDR(0x1b0), &uiTemp);
    mcCLR_MASK(uiTemp, 0xF<<8);
    mcSET_MASK(uiTemp, 0<<8);
    ucstatus |= ucDram_Register_Write(mcSET_DRAMC_REG_ADDR(0x1b0), uiTemp);

    // Set CA7 output delay
    // CA7 - RA13 - 0x1bc 16~20
    ucstatus |= ucDram_Register_Read(mcSET_DRAMC_REG_ADDR(0x1bc), &uiTemp);
    mcCLR_MASK(uiTemp, 0xF<<16);
    mcSET_MASK(uiTemp, 0<<16);
    ucstatus |= ucDram_Register_Write(mcSET_DRAMC_REG_ADDR(0x1bc), uiTemp);

    // Set CA8 output delay
    // CA8 - RA15- 0x1c0 16~20
    ucstatus |= ucDram_Register_Read(mcSET_DRAMC_REG_ADDR(0x1c0), &uiTemp);
    mcCLR_MASK(uiTemp, 0xF<<16);
    mcSET_MASK(uiTemp, 0<<16);
    ucstatus |= ucDram_Register_Write(mcSET_DRAMC_REG_ADDR(0x1c0), uiTemp);

    #if 1 // Set CA9 output delay
    // CA9 - RA2 - 0x1a8 16~19
    ucstatus |= ucDram_Register_Read(mcSET_DRAMC_REG_ADDR(0x1a8), &uiTemp);
    mcCLR_MASK(uiTemp, 0xF<<16);
    mcSET_MASK(uiTemp, 0<<16);
    ucstatus |= ucDram_Register_Write(mcSET_DRAMC_REG_ADDR(0x1a8), uiTemp);
    #endif

    // CS extent enable (need DRAM to support)
    // for testing

    ucstatus |= ucDram_Register_Read(mcSET_DRAMC_REG_ADDR(0x80), &uiTemp);
    mcSET_BIT(uiTemp, 13);
    ucstatus |= ucDram_Register_Write(mcSET_DRAMC_REG_ADDR(0x80), uiTemp);

    CA_PRINT("[CA Training] REG 0x80 value: 0x%x\n", uiTemp);

    // Enter MR 41/MR48
    // Set MA & OP.
    if (uiMR41)
    {
        ucstatus |= ucDram_Register_Write(mcSET_DRAMC_REG_ADDR(DRAMC_REG_MRS), 0x00a40029);
    }
    else
    {
        ucstatus |= ucDram_Register_Write(mcSET_DRAMC_REG_ADDR(DRAMC_REG_MRS), 0x00c00030);
    }
    // Hold the CA bus stable for at least one cycle.
    ucstatus |= ucDram_Register_Read(mcSET_DRAMC_REG_ADDR(0x4c), &uiTemp); //todo:
    CA_PRINT("[CA Training] REG 0x4C value: 0x%x, 1\n", uiTemp);
    mcSET_BIT(uiTemp, 2);
    ucstatus |= ucDram_Register_Write(mcSET_DRAMC_REG_ADDR(0x4c), uiTemp);

    ucDram_Register_Read(mcSET_DRAMC_REG_ADDR(0x4c), &uiTemp);
    CA_PRINT("[CA Training] REG 0x4C value: 0x%x, 2\n", uiTemp);

    // MRW
    ucstatus |= ucDram_Register_Read(mcSET_DRAMC_REG_ADDR(0x1e4), &uiTemp);
    mcSET_BIT(uiTemp, 0);
    ucstatus |= ucDram_Register_Write(mcSET_DRAMC_REG_ADDR(0x1e4), uiTemp);
    mcDELAY_US(1);
    mcCLR_BIT(uiTemp, 0);
    ucstatus |= ucDram_Register_Write(mcSET_DRAMC_REG_ADDR(0x1e4), uiTemp);
    // Disable CA bus stable.
    ucstatus |= ucDram_Register_Read(mcSET_DRAMC_REG_ADDR(0x4c), &uiTemp);
    mcCLR_BIT(uiTemp, 2);
    ucstatus |= ucDram_Register_Write(mcSET_DRAMC_REG_ADDR(0x4c), uiTemp);

    // Wait tCACKEL(10 tck) before CKE low
    mcDELAY_US(1);

    // CKE low
    ucstatus |= ucDram_Register_Read(mcSET_DRAMC_REG_ADDR(0xe4), &uiTemp);
    mcSET_BIT(uiTemp, 3);
    mcCLR_BIT(uiTemp, 2);
    ucstatus |= ucDram_Register_Write(mcSET_DRAMC_REG_ADDR(0xe4), uiTemp);

    ucDram_Register_Read(mcSET_DRAMC_REG_ADDR(0x50), &uiTemp);
    CA_PRINT("[CA Training] REG 0x50 value: 0x%x, 1, new\n", uiTemp);

    // Set CA0~CA3, CA5~CA8 rising/falling golden value.
    if (uiMR41)
    {
        //  01010101b -> 10101010b : Golden value = 1001100110011001b=0x9999
        //  11111111b -> 00000000b : Golden value = 0101010101010101b=0x5555
        ucstatus |= ucDram_Register_Write(mcSET_DRAMC_REG_ADDR(0x50), 0x55555555); //todo:
        //ucstatus |= ucDram_Register_Write(mcSET_DRAMC_REG_ADDR(0x50), 0x00040004);
        //ucstatus |= ucDram_Register_Write(mcSET_DRAMC_REG_ADDR(0x50), 0x0000ffff);
    }
    else
    {
        //  00010001b -> 00000000b : Golden value = 0000000100000001b=0x0101
        ucstatus |= ucDram_Register_Write(mcSET_DRAMC_REG_ADDR(0x50), 0x01010101);
        //ucstatus |= ucDram_Register_Write(mcSET_DRAMC_REG_ADDR(0x50), 0x00010001);
        //ucstatus |= ucDram_Register_Write(mcSET_DRAMC_REG_ADDR(0x50), 0x0000ffff);
    }

    ucDram_Register_Read(mcSET_DRAMC_REG_ADDR(0x50), &uiTemp);
    CA_PRINT("[CA Training] REG 0x50 value: 0x%x, 2\n", uiTemp);

    // Wait tCAENT(10 tck) before pattern output
    mcDELAY_US(1);

    //while(1);

    // Delay clock output delay to do CA training in order to get the pass window.
    uiFinishCount = 0;
    for (uiDelay=0; uiDelay<=MAX_CLKO_DELAY; uiDelay++)
    {
        //DramcEnterSelfRefresh(p, 1);
        // Set Clk output delay
        ucstatus |= ucDram_Register_Read(mcSET_DRAMC_REG_ADDR(0x0c), &uiTemp);
        mcSET_FIELD(uiTemp, uiDelay, 0x0f000000, 24);
        ucstatus |= ucDram_Register_Write(mcSET_DRAMC_REG_ADDR(0x0c), uiTemp);
        //DramcEnterSelfRefresh(p, 0);

        // CA training pattern output enable
        ucstatus |= ucDram_Register_Read(mcSET_DRAMC_REG_ADDR(0x4c), &uiTemp);
        mcSET_BIT(uiTemp, 1);
        ucstatus |= ucDram_Register_Write(mcSET_DRAMC_REG_ADDR(0x4c), uiTemp);
        // delay 2 DRAM clock cycle
        mcDELAY_US(1);
        mcCLR_BIT(uiTemp, 1);
        ucstatus |= ucDram_Register_Write(mcSET_DRAMC_REG_ADDR(0x4c), uiTemp);

        // Wait tADR(20ns) before CA sampled values available in DQ.
        mcDELAY_US(1);

        // Get DQ value.
        ucstatus |= ucDram_Register_Read(mcSET_DRAMC_REG_ADDR(0x380), &uiTemp);

        //mcSHOW_DBG_MSG2(("[CA Training] CLK delay -- %d, MR41 -- %d, DQ_O1 -- 0x%x\n", uiDelay, uiMR41, uiTemp));

        // Compare with golden value.
        if (uiMR41)
        {
            for (uiCA=0; uiCA<CATRAINING_NUM; uiCA++)
            {
                if ((uiCA==4) || (uiCA==9))
                {
                    continue;
                }
                if ( (iFirstClkPass[uiCA]==-1) || (iLastClkPass[uiCA]==-1))
                {
                    if (uiCA<4)
                    {
                        uiRisingEdge = uiTemp & (0x01 << uiLPDDR_PHY_Mapping[uiCA<<1]);
                        uiFallingEdge = uiTemp & (0x01 << uiLPDDR_PHY_Mapping[(uiCA<<1)+1]);
                    }
                    else
                    {
                        uiRisingEdge = uiTemp & (0x01 << uiLPDDR_PHY_Mapping[(uiCA-1)<<1]);
                        uiFallingEdge = uiTemp & (0x01 << uiLPDDR_PHY_Mapping[((uiCA-1)<<1)+1]);
                    }

                    //mcSHOW_DBG_MSG2(("[CA Training] CLK, R - %x, F - %x\n", uiRisingEdge, uiFallingEdge));

                    // Check 1->0 transition.
                    if ((uiRisingEdge!=0) && (uiFallingEdge==0))
                    {
                        iPass = 1;
                        //mcSHOW_DBG_MSG2(("CLK, CA%d, [%x, %x], Pass 1\n", uiCA, uiRisingEdge, uiFallingEdge));
                    }
                    else
                    {
                        iPass = 0;
                    }


                    if (iFirstClkPass[uiCA]==-1)
                    {
                        if (iPass == 1)
                        {
                            iFirstClkPass[uiCA] = uiDelay;
                        }
                    }
                    else
                    {
                        if (iLastClkPass[uiCA]==-1)
                        {
                            if (iPass == 0)
                            {
                                iLastClkPass[uiCA] = uiDelay-1;
                                uiFinishCount++;
                            }
                            else
                            {
                                if (uiDelay==MAX_CLKO_DELAY)
                                {
                                    iLastClkPass[uiCA] = uiDelay;
                                    uiFinishCount++;
                                }
                            }
                            if (iLastClkPass[uiCA]!=-1)
                            {
                            	if ( (iLastClkPass[uiCA]-iFirstClkPass[uiCA]) > (iBestLastClkPass[uiCA]-iBestFirstClkPass[uiCA]))
                            	{
                            		iBestLastClkPass[uiCA] = iLastClkPass[uiCA];
                            		iBestFirstClkPass[uiCA] = iFirstClkPass[uiCA];
                            	}
                            	iLastClkPass[uiCA] = iFirstClkPass[uiCA] = -1;
                            }
                        }
                    }
                }
            }

            // Wait tCACD(22clk) before output CA pattern to DDR again..
            mcDELAY_US(1);

            //if (uiFinishCount==8) {
            //    break;
            //}
        }
        else
        {
            // MR 48
            uiCA = 4;

MR48_CHECKRESULT_CLK:

            if ((iFirstClkPass[uiCA]==-1) || (iLastClkPass[uiCA]==-1))
            {
                uiRisingEdge = uiTemp & (0x01 << uiLPDDR_PHY_Mapping[(uiCA==4) ? 0 : 8]);
                uiFallingEdge = uiTemp & (0x01 << uiLPDDR_PHY_Mapping[(uiCA==4) ? 1 : 9]);

                // Check 1->0 transition.
                if ((uiRisingEdge!=0) && (uiFallingEdge==0))
                {
                    iPass = 1;
                    //mcSHOW_DBG_MSG2(("CLK, CA%d, [%x, %x], Pass 1\n", uiCA, uiRisingEdge, uiFallingEdge));
                }
                else
                {
                    iPass = 0;
                }
                if (iFirstClkPass[uiCA]==-1)
                {
                    if (iPass==1)
                    {
                        iFirstClkPass[uiCA] = uiDelay;
                    }
                }
                else
                {
                    if (iLastClkPass[uiCA]==-1)
                    {
                        if (iPass==0)
                        {
                            iLastClkPass[uiCA] = uiDelay-1;
                            uiFinishCount++;
                        }
                        else
                        {
                            if (uiDelay==MAX_CLKO_DELAY)
                            {
                                iLastClkPass[uiCA] = uiDelay;
                                uiFinishCount++;
                            }
                        }
                        if (iLastClkPass[uiCA]!=-1)
                        {
                        	if ( (iLastClkPass[uiCA]-iFirstClkPass[uiCA]) > (iBestLastClkPass[uiCA]-iBestFirstClkPass[uiCA]))
                        	{
                        		iBestLastClkPass[uiCA] = iLastClkPass[uiCA];
                        		iBestFirstClkPass[uiCA] = iFirstClkPass[uiCA];
                        	}
                        	iLastClkPass[uiCA] = iFirstClkPass[uiCA] = -1;
                        }
                    }
                }
            }

            if (uiCA==4)
            {
                uiCA=9;
                goto MR48_CHECKRESULT_CLK;
            }

            // Wait tCACD(22clk) before output CA pattern to DDR again..
            mcDELAY_US(1);

            //if (uiFinishCount==2) {
            //    break;
            //}
        }
    }

    //DramcEnterSelfRefresh(p, 1);
    // Set Clk output delay to 0.
    ucstatus |= ucDram_Register_Read(mcSET_DRAMC_REG_ADDR(0x0c), &uiTemp);
    mcSET_FIELD(uiTemp, 0, 0x0f000000, 24);
    ucstatus |= ucDram_Register_Write(mcSET_DRAMC_REG_ADDR(0x0c), uiTemp);
    //DramcEnterSelfRefresh(p, 0);

    // Delay CA output delay to do CA training in order to get the pass window.
    uiFinishCount = 0;
    for (uiDelay=0; uiDelay<=MAX_CLKO_DELAY; uiDelay++)
    {
        // Set CA0~CA3, CA5~CA8 output delay.
        #ifdef CA_DELAY_CHAIN
        uiTemp = uiDelay | (uiDelay<<8) | (uiDelay<<16) | (uiDelay<<24);
        ucstatus |= ucDram_Register_Write(mcSET_DRAMC_REG_ADDR(0x1a8), uiTemp | DRAMC_READ_REG(0x1a8));    // CA0~CA3
        ucstatus |= ucDram_Register_Write(mcSET_DRAMC_REG_ADDR(0x1ac), uiTemp | DRAMC_READ_REG(0x1ac));    // CA4~CA7
        ucstatus |= ucDram_Register_Write(mcSET_DRAMC_REG_ADDR(0x1b0), uiTemp | DRAMC_READ_REG(0x1b0));    // CA8~CA11
        ucstatus |= ucDram_Register_Write(mcSET_DRAMC_REG_ADDR(0x1b4), uiTemp | DRAMC_READ_REG(0x1b4));
        ucstatus |= ucDram_Register_Write(mcSET_DRAMC_REG_ADDR(0x1b8), uiTemp | DRAMC_READ_REG(0x1b8));
        ucstatus |= ucDram_Register_Write(mcSET_DRAMC_REG_ADDR(0x1bc), ((uiDelay<<8) | (uiDelay<<16) | (uiDelay<<24)) | DRAMC_READ_REG(0x1bc));
        ucstatus |= ucDram_Register_Write(mcSET_DRAMC_REG_ADDR(0x1c0), ((uiDelay<<16) | (uiDelay<<24)) | DRAMC_READ_REG(0x1c0));
        CA_PRINT("[CA Training] CA delay -- %d, set all\n", uiDelay);
       #else

       // Set CA0 output delay
       // CA0 - RA12 - 0x1b4 0~3
       ucstatus |= ucDram_Register_Read(mcSET_DRAMC_REG_ADDR(0x1b4), &uiTemp);
       mcCLR_MASK(uiTemp, 0xF);
       mcSET_MASK(uiTemp, uiDelay);
       ucstatus |= ucDram_Register_Write(mcSET_DRAMC_REG_ADDR(0x1b4), uiTemp);

       // Set CA1 output delay
       // CA1 - RA7 - 0x1ac 24~27
       ucstatus |= ucDram_Register_Read(mcSET_DRAMC_REG_ADDR(0x1ac), &uiTemp);
       mcCLR_MASK(uiTemp, 0xF<<24);
       mcSET_MASK(uiTemp, uiDelay<<24);
       ucstatus |= ucDram_Register_Write(mcSET_DRAMC_REG_ADDR(0x1ac), uiTemp);

       // Set CA2 output delay
       // CA2 - RA11 - 0x1b0 24~27
       ucstatus |= ucDram_Register_Read(mcSET_DRAMC_REG_ADDR(0x1b0), &uiTemp);
       mcCLR_MASK(uiTemp, 0xF<<24);
       mcSET_MASK(uiTemp, uiDelay<<24);
       ucstatus |= ucDram_Register_Write(mcSET_DRAMC_REG_ADDR(0x1b0), uiTemp);

       // Set CA3 output delay
       // CA3 - RA3 - 0x1a8 24~27
       ucstatus |= ucDram_Register_Read(mcSET_DRAMC_REG_ADDR(0x1a8), &uiTemp);
       mcCLR_MASK(uiTemp, 0xF<<24);
       mcSET_MASK(uiTemp, uiDelay<<24);
       ucstatus |= ucDram_Register_Write(mcSET_DRAMC_REG_ADDR(0x1a8), uiTemp);

       // Set CA4 output delay
       // CA4 - RA0 - 0x1a8 0~3
       ucstatus |= ucDram_Register_Read(mcSET_DRAMC_REG_ADDR(0x1a8), &uiTemp);
       mcCLR_MASK(uiTemp, 0xF);
       mcSET_MASK(uiTemp, uiDelay);
       ucstatus |= ucDram_Register_Write(mcSET_DRAMC_REG_ADDR(0x1a8), uiTemp);

       // Set CA5 output delay
       // CA5 - RA5 - 0x1ac 8~11
       ucstatus |= ucDram_Register_Read(mcSET_DRAMC_REG_ADDR(0x1ac), &uiTemp);
       mcCLR_MASK(uiTemp, 0xF<<8);
       mcSET_MASK(uiTemp, uiDelay<<8);
       ucstatus |= ucDram_Register_Write(mcSET_DRAMC_REG_ADDR(0x1ac), uiTemp);

       // Set CA6 output delay
       // CA6 - RA9 - 0x1b0 8~11
       ucstatus |= ucDram_Register_Read(mcSET_DRAMC_REG_ADDR(0x1b0), &uiTemp);
       mcCLR_MASK(uiTemp, 0xF<<8);
       mcSET_MASK(uiTemp, uiDelay<<8);
       ucstatus |= ucDram_Register_Write(mcSET_DRAMC_REG_ADDR(0x1b0), uiTemp);

       // Set CA7 output delay
       // CA7 - RA13 - 0x1bc 16~20
       ucstatus |= ucDram_Register_Read(mcSET_DRAMC_REG_ADDR(0x1bc), &uiTemp);
       mcCLR_MASK(uiTemp, 0xF<<16);
       mcSET_MASK(uiTemp, uiDelay<<16);
       ucstatus |= ucDram_Register_Write(mcSET_DRAMC_REG_ADDR(0x1bc), uiTemp);

       // Set CA8 output delay
       // CA8 - RA15- 0x1c0 16~20
       ucstatus |= ucDram_Register_Read(mcSET_DRAMC_REG_ADDR(0x1c0), &uiTemp);
       mcCLR_MASK(uiTemp, 0xF<<16);
       mcSET_MASK(uiTemp, uiDelay<<16);
       ucstatus |= ucDram_Register_Write(mcSET_DRAMC_REG_ADDR(0x1c0), uiTemp);

       // Set CA9 output delay
       // CA9 - RA2 - 0x1a8 16~19
       ucstatus |= ucDram_Register_Read(mcSET_DRAMC_REG_ADDR(0x1a8), &uiTemp);
       mcCLR_MASK(uiTemp, 0xF<<16);
       mcSET_MASK(uiTemp, uiDelay<<16);
       ucstatus |= ucDram_Register_Write(mcSET_DRAMC_REG_ADDR(0x1a8), uiTemp);
       #endif

        #if 0
        if (1 == uiDelay)
        {
            mcSHOW_DBG_MSG2(("[CA Training] 0x1a8:0x%x\n", DRAMC_READ_REG(0x1a8)));
            mcSHOW_DBG_MSG2(("[CA Training] 0x1ac:0x%x\n", DRAMC_READ_REG(0x1ac)));
            mcSHOW_DBG_MSG2(("[CA Training] 0x1b0:0x%x\n", DRAMC_READ_REG(0x1b0)));
            mcSHOW_DBG_MSG2(("[CA Training] 0x1b4:0x%x\n", DRAMC_READ_REG(0x1b4)));
            mcSHOW_DBG_MSG2(("[CA Training] 0x1b8:0x%x\n", DRAMC_READ_REG(0x1b8)));
            mcSHOW_DBG_MSG2(("[CA Training] 0x1bc:0x%x\n", DRAMC_READ_REG(0x1bc)));
            mcSHOW_DBG_MSG2(("[CA Training] 0x1c0:0x%x\n", DRAMC_READ_REG(0x1c0)));
        }
        #endif

        // CA training pattern output enable
        ucstatus |= ucDram_Register_Read(mcSET_DRAMC_REG_ADDR(0x4c), &uiTemp);
        mcSET_BIT(uiTemp, 1);
        ucstatus |= ucDram_Register_Write(mcSET_DRAMC_REG_ADDR(0x4c), uiTemp);
        // delay 2 DRAM clock cycle
        mcDELAY_US(10);
        mcCLR_BIT(uiTemp, 1);
        ucstatus |= ucDram_Register_Write(mcSET_DRAMC_REG_ADDR(0x4c), uiTemp);

        // Wait tADR(20ns) before CA sampled values available in DQ.
        mcDELAY_US(10);

        // Get DQ value.
        ucstatus |= ucDram_Register_Read(mcSET_DRAMC_REG_ADDR(0x380), &uiTemp);

        //mcSHOW_DBG_MSG2(("[CA Training] CA delay -- %d, MR41 -- %d, DQ_O1 -- 0x%x\n", uiDelay, uiMR41, uiTemp));

        // Compare with golden value.
        if (uiMR41)
        {
            for (uiCA=0; uiCA<CATRAINING_NUM; uiCA++)
            {
                if ((uiCA==4) || (uiCA==9))
                {
                    continue;
                }
                if ( (iFirstCAPass[uiCA]==-1) || (iLastCAPass[uiCA]==-1))
                {
                    if (uiCA<4)
                    {
                        uiRisingEdge = uiTemp & (0x01 << uiLPDDR_PHY_Mapping[uiCA<<1]);
                        uiFallingEdge = uiTemp & (0x01 << uiLPDDR_PHY_Mapping[(uiCA<<1)+1]);
                    }
                    else
                    {
                        uiRisingEdge = uiTemp & (0x01 << uiLPDDR_PHY_Mapping[(uiCA-1)<<1]);
                        uiFallingEdge = uiTemp & (0x01 << uiLPDDR_PHY_Mapping[((uiCA-1)<<1)+1]);
                    }

                    //mcSHOW_DBG_MSG2(("[CA Training] CA, R -- %x, F -- %x\n", uiRisingEdge, uiFallingEdge));

                    // Check 1->0 transition.
                    if ((uiRisingEdge!=0) && (uiFallingEdge==0))
                    {
                        iPass = 1;
                        //mcSHOW_DBG_MSG2(("CA, CA%d, [%x, %x], Pass 1\n", uiCA, uiRisingEdge, uiFallingEdge));
                    }
                    else
                    {
                        iPass = 0;
                    }

                    if (iFirstCAPass[uiCA]==-1)
                    {
                        if (iPass == 1)
                        {
                            iFirstCAPass[uiCA] = uiDelay;
                        }
                    }
                    else
                    {
                        if (iLastCAPass[uiCA]==-1)
                        {
                            if (iPass == 0)
                            {
                                iLastCAPass[uiCA] = uiDelay-1;
                                uiFinishCount++;
                            }
                            else
                            {
                                if (uiDelay==MAX_CLKO_DELAY)
                                {
                                    iLastCAPass[uiCA] = uiDelay;
                                    uiFinishCount++;
                                }
                            }
                            if (iLastCAPass[uiCA]!=-1)
                            {
	                            if ( (iLastCAPass[uiCA]-iFirstCAPass[uiCA]) > (iBestLastCAPass[uiCA]-iBestFirstCAPass[uiCA]) )
	                            {
	                            	iBestLastCAPass[uiCA] = iLastCAPass[uiCA];
	                            	iBestFirstCAPass[uiCA] = iFirstCAPass[uiCA];
	                            }
	                            iLastCAPass[uiCA] = iFirstCAPass[uiCA] = -1;
                            }
                        }
                    }
                }
            }

            // Wait tCACD(22clk) before output CA pattern to DDR again..
            mcDELAY_US(10);

            //if (uiFinishCount==8) {
            //    break;
            //}
        }
        else
        {
            // MR 48
            uiCA = 4;

MR48_CHECKRESULT_CA:

            if ((iFirstCAPass[uiCA]==-1) || (iLastCAPass[uiCA]==-1))
            {
                uiRisingEdge = uiTemp & (0x01 << uiLPDDR_PHY_Mapping[(uiCA==4) ? 0 : 8]);
                uiFallingEdge = uiTemp & (0x01 << uiLPDDR_PHY_Mapping[(uiCA==4) ? 1 : 9]);

                // Check 1->0 transition.
                if ((uiRisingEdge!=0) && (uiFallingEdge==0))
                {
                    iPass = 1;
                    //mcSHOW_DBG_MSG2(("CA, CA%d, [%x, %x], Pass 1\n", uiCA, uiRisingEdge, uiFallingEdge));
                }
                else
                {
                    iPass = 0;
                }

                if (iFirstCAPass[uiCA]==-1)
                {
                    if (iPass==1)
                    {
                        iFirstCAPass[uiCA] = uiDelay;
                    }
                }
                else
                {
                    if (iLastCAPass[uiCA]==-1)
                    {
                        if (iPass==0)
                        {
                            iLastCAPass[uiCA] = uiDelay-1;
                            uiFinishCount++;
                        }
                        else
                        {
                            if (uiDelay==MAX_CLKO_DELAY)
                            {
                                iLastCAPass[uiCA] = uiDelay;
                                uiFinishCount++;
                            }
                        }
                        if (iLastCAPass[uiCA]!=-1)
                        {
                            if ( (iLastCAPass[uiCA]-iFirstCAPass[uiCA]) > (iBestLastCAPass[uiCA]-iBestFirstCAPass[uiCA]) )
                            {
                            	iBestLastCAPass[uiCA] = iLastCAPass[uiCA];
                            	iBestFirstCAPass[uiCA] = iFirstCAPass[uiCA];
                            }
                            iLastCAPass[uiCA] = iFirstCAPass[uiCA] = -1;
                        }
                    }
                }
            }

            if (uiCA==4)
            {
                uiCA=9;
                goto MR48_CHECKRESULT_CA;
            }

            // Wait tCACD(22clk) before output CA pattern to DDR again..
            mcDELAY_US(10);

            //if (uiFinishCount==2) {
            //    break;
            //}
        }
    }

    // CS extent disable
    // for testing
    ucstatus |= ucDram_Register_Read(mcSET_DRAMC_REG_ADDR(0x80), &uiTemp);
    mcCLR_BIT(uiTemp, 13);
    ucstatus |= ucDram_Register_Write(mcSET_DRAMC_REG_ADDR(0x80), uiTemp);

    if (uiMR41==0)
    {
        // Disable fix DQ input enable.
        ucstatus |= ucDram_Register_Read(mcSET_DRAMC_REG_ADDR(0xd8), &uiTemp);
        uiTemp = uiTemp & 0xffff0fff;
        ucstatus |= ucDram_Register_Write(mcSET_DRAMC_REG_ADDR(0xd8), uiTemp);
    }

    // Wait tCACKEN (10ck)
    mcDELAY_US(10);

    // CKE high
    ucstatus |= ucDram_Register_Read(mcSET_DRAMC_REG_ADDR(0xe4), &uiTemp);
    mcCLR_BIT(uiTemp, 3);
    mcSET_BIT(uiTemp, 2);
    ucstatus |= ucDram_Register_Write(mcSET_DRAMC_REG_ADDR(0xe4), uiTemp);

    if (uiMR41)
    {
        uiMR41 = 0;
        goto CA_TRAINING_BEGIN;
    }

    // CS extent enable
    // for testing
    ucstatus |= ucDram_Register_Read(mcSET_DRAMC_REG_ADDR(0x80), &uiTemp);
    mcSET_BIT(uiTemp, 13);
    ucstatus |= ucDram_Register_Write(mcSET_DRAMC_REG_ADDR(0x80), uiTemp);

    // MR42 to leave CA training.
    ucstatus |= ucDram_Register_Write(mcSET_DRAMC_REG_ADDR(DRAMC_REG_MRS), 0x00a8002a);
    // Hold the CA bus stable for at least one cycle.
    ucstatus |= ucDram_Register_Read(mcSET_DRAMC_REG_ADDR(0x4c), &uiTemp);
    mcSET_BIT(uiTemp, 2);
    ucstatus |= ucDram_Register_Write(mcSET_DRAMC_REG_ADDR(0x4c), uiTemp);
    // MRW
    ucstatus |= ucDram_Register_Read(mcSET_DRAMC_REG_ADDR(0x1e4), &uiTemp);
    mcSET_BIT(uiTemp, 0);
    ucstatus |= ucDram_Register_Write(mcSET_DRAMC_REG_ADDR(0x1e4), uiTemp);
    mcDELAY_US(1);
    mcCLR_BIT(uiTemp, 0);
    ucstatus |= ucDram_Register_Write(mcSET_DRAMC_REG_ADDR(0x1e4), uiTemp);
    // Disable the hold the CA bus stable for at least one cycle.
    ucstatus |= ucDram_Register_Read(mcSET_DRAMC_REG_ADDR(0x4c), &uiTemp);
    mcCLR_BIT(uiTemp, 2);
    ucstatus |= ucDram_Register_Write(mcSET_DRAMC_REG_ADDR(0x4c), uiTemp);

    // CS extent disable
    // for testing
    ucstatus |= ucDram_Register_Read(mcSET_DRAMC_REG_ADDR(0x80), &uiTemp);
    mcCLR_BIT(uiTemp, 13);
    ucstatus |= ucDram_Register_Write(mcSET_DRAMC_REG_ADDR(0x80), uiTemp);

    // Disable CKE high
    ucstatus |= ucDram_Register_Read(mcSET_DRAMC_REG_ADDR(0xe4), &uiTemp);
    mcCLR_BIT(uiTemp, 2);
    ucstatus |= ucDram_Register_Write(mcSET_DRAMC_REG_ADDR(0xe4), uiTemp);

    // Calculate the middle range & max middle.
    CA_PRINT("=========================================\n");
    CA_PRINT("1. CA training window before adjustment. \n");
    CA_PRINT("x=Pass window CA(max~min) Clk(min~max) center. \n");
    CA_PRINT("y=CA0~CA9\n");
    CA_PRINT("=========================================\n");

    mcFPRINTF((fp_A60808, "=========================================\n"));
    mcFPRINTF((fp_A60808, "1. CA training window before adjustment.\n"));
    mcFPRINTF((fp_A60808, "x=Pass window CA(max~min) Clk(min~max) center. \n"));
    mcFPRINTF((fp_A60808, "y=CA0~CA9\n"));
    mcFPRINTF((fp_A60808, "=========================================\n"));

    iMaxCenter = 0 - MAX_CLKO_DELAY;
    for (uiCA=0; uiCA<CATRAINING_NUM; uiCA++)
    {
        iClkWinSize = iBestLastClkPass[uiCA] - iBestFirstClkPass[uiCA];
        iCAWinSize = iBestLastCAPass[uiCA] - iBestFirstCAPass[uiCA];
        if (iClkWinSize >= iCAWinSize)
        {
            if (iCAWinSize>0)
            {
                iCenter[uiCA] =  (iClkWinSize - iCAWinSize)>>1;
            }
            else
            {
                iCenter[uiCA] =  iBestFirstClkPass[uiCA] + (iClkWinSize>>1);
            }
        }
        else
        {
            if (iClkWinSize>0)
            {
                iCenter[uiCA] =  (iClkWinSize - iCAWinSize)/2;
            }
            else
            {
                iCenter[uiCA] =  0-(iBestFirstCAPass[uiCA] + (iCAWinSize>>1));
            }
        }
        print("CA%d     CA(%d~%d) Clk(%d~%d) %d\n", uiCA, iBestLastCAPass[uiCA],  iBestFirstCAPass[uiCA],
            iBestFirstClkPass[uiCA], iBestLastClkPass[uiCA], iCenter[uiCA]);

        mcFPRINTF((fp_A60808, "CA%d     CA(%d~%d) Clk(%d~%d) %d\n", uiCA, iBestLastCAPass[uiCA],  iBestFirstCAPass[uiCA],
            iBestFirstClkPass[uiCA], iBestLastClkPass[uiCA], iCenter[uiCA]));

        if (iCenter[uiCA]  > iMaxCenter)
        {
            iMaxCenter = iCenter[uiCA];
        }
#ifdef EYE_SCAN
	EyeScanWin[uiCA].ucsetup_pass_number = iCAWinSize;
	EyeScanWin[uiCA].uchold_pass_number = iClkWinSize;
#endif

    #ifdef DDR_FT_LOAD_BOARD
        if ((iCAWinSize==0) && (iClkWinSize == 0))
        {
            LoadBoardShowResult(FLAG_CA_CALIBRATION, FLAG_CALIBRATION_FAIL, p->channel, FLAG_NOT_COMPLETE_OR_FAIL);
            while(1);
        }
        else if ((iCAWinSize+iClkWinSize)<=CA_TRAINING_BOUND)
        {
            LoadBoardShowResult(FLAG_CA_CALIBRATION, FLAG_WINDOW_TOO_SMALL, p->channel, FLAG_NOT_COMPLETE_OR_FAIL);
            while(1);
        }
    #endif
    }
    CA_PRINT("=========================================\n");
    CA_PRINT("Max center = %d\n\n", iMaxCenter);
    mcFPRINTF((fp_A60808, "=========================================\n"));
    mcFPRINTF((fp_A60808, "Max center = %d\n\n", iMaxCenter));

    // Calculate the shift value
    CA_PRINT("=========================================\n");
    CA_PRINT("2. CA training window after adjustment.\n");
    CA_PRINT("x=CA shift     Adjusted Pass window CA(max~min) Clk(min~max) center.\n");
    CA_PRINT("y=CA0~CA9\n");
    CA_PRINT("=========================================\n");

    mcFPRINTF((fp_A60808, "=========================================\n"));
    mcFPRINTF((fp_A60808, "2. [Channel %d] CA training window after adjustment.\n", p->channel));
    mcFPRINTF((fp_A60808, "x=CA shift     Adjusted Pass window CA(max~min) Clk(min~max) center.\n"));
    mcFPRINTF((fp_A60808, "y=CA0~CA9\n"));
    mcFPRINTF((fp_A60808, "=========================================\n"));


    if (iMaxCenter < 0)
    {
        // Clk output delay could not be negative. Need to adjust into 0.
        iMaxCenter = 0;
        CA_PRINT(("Max center < 0. Adjust to 0. \n\n"));
    }

    for (uiCA=0; uiCA<CATRAINING_NUM; uiCA++)
    {
        iCAShift[uiCA] = iMaxCenter - iCenter[uiCA]+CATRAINING_STEP;
        if (iCAShift[uiCA]>=MAX_CLKO_DELAY)
        {
            iCAShift[uiCA] = MAX_CLKO_DELAY;
        }
#ifdef CKE_CS_DLY_SETTING
        CAShift_Avg += iCAShift[uiCA];
#endif
        CA_PRINT("CA%d     Shift %d     CA(%d~%d) Clk(%d~%d) %d\n", uiCA, iCAShift[uiCA],
            iBestLastCAPass[uiCA]-iCAShift[uiCA], iBestFirstCAPass[uiCA],
            iBestFirstClkPass[uiCA], iBestLastClkPass[uiCA]+iCAShift[uiCA], iCenter[uiCA]+iCAShift[uiCA]);

        mcFPRINTF((fp_A60808, "CA%d     Shift %d     CA(%d~%d) Clk(%d~%d) %d\n", uiCA, iCAShift[uiCA],
            iBestLastCAPass[uiCA]-iCAShift[uiCA], iBestFirstCAPass[uiCA],
            iBestFirstClkPass[uiCA], iBestLastClkPass[uiCA]+iCAShift[uiCA], iCenter[uiCA]+iCAShift[uiCA]));
    }
    CA_PRINT("=========================================\n");
    mcFPRINTF((fp_A60808, "=========================================\n"));

    // Restore the registers' values.
    ucstatus |= ucDram_Register_Write(mcSET_DRAMC_REG_ADDR(0x1dc), uiReg1DCh);
    ucstatus |= ucDram_Register_Write(mcSET_DRAMC_REG_ADDR(DRAMC_REG_LPDDR2), uiReg1E0h);
    //ucstatus |= ucDram_Register_Write(mcSET_DRAMC_REG_ADDR(DRAMC_REG_MCKDLY), uiRegD8h);

    // Write shift value into CA output delay.
#ifdef CA_DELAY_CHAIN
    uiTemp = iCAShift[0] + (iCAShift[1]<<8) + (iCAShift[2]<<16) + (iCAShift[3]<<24);
    ucstatus |= ucDram_Register_Write(mcSET_DRAMC_REG_ADDR(0x1a8), uiTemp);    // CA0~CA3
    CA_PRINT("Reg.1A8h=%xh\n", uiTemp);
    mcFPRINTF((fp_A60808, "Reg.1A8h=%xh\n", uiTemp));
    uiTemp = iCAShift[4] + (iCAShift[5]<<8) + (iCAShift[6]<<16) + (iCAShift[7]<<24);
    ucstatus |= ucDram_Register_Write(mcSET_DRAMC_REG_ADDR(0x1ac), uiTemp);    // CA4~CA7
    CA_PRINT("Reg.1ACh=%xh\n", uiTemp);
    mcFPRINTF((fp_A60808, "Reg.1ACh=%xh\n", uiTemp));
    uiTemp = iCAShift[8] + (iCAShift[9]<<8);
    ucstatus |= ucDram_Register_Write(mcSET_DRAMC_REG_ADDR(0x1b0), uiTemp);    // CA8~CA11
    CA_PRINT("Reg.1B0h=%xh\n", uiTemp);
    mcFPRINTF((fp_A60808, "Reg.1B0h=%xh\n", uiTemp));
#else

    #if 0 //todo: tmp code
    for (uiCA=0; uiCA<CATRAINING_NUM; uiCA++)
    {
        print("CA%d: %x ->", uiCA, iCAShift[uiCA]);

            //iCAShift[uiCA] += 2;

        print("%x\n", iCAShift[uiCA]);
    }
    #endif

    #if 1 //8163
    // Set CA0 output delay
    // CA0 - RA12 - 0x1b4 0~3
    ucstatus |= ucDram_Register_Read(mcSET_DRAMC_REG_ADDR(0x1b4), &uiTemp);
    mcCLR_MASK(uiTemp, 0xF);
    mcSET_MASK(uiTemp, iCAShift[0]);
    ucstatus |= ucDram_Register_Write(mcSET_DRAMC_REG_ADDR(0x1b4), uiTemp);

    // Set CA1 output delay
    // CA1 - RA7 - 0x1ac 24~27
    ucstatus |= ucDram_Register_Read(mcSET_DRAMC_REG_ADDR(0x1ac), &uiTemp);
    mcCLR_MASK(uiTemp, 0xF<<24);
    mcSET_MASK(uiTemp, iCAShift[1]<<24);
    ucstatus |= ucDram_Register_Write(mcSET_DRAMC_REG_ADDR(0x1ac), uiTemp);

    // Set CA2 output delay
    // CA2 - RA11 - 0x1b0 24~27
    ucstatus |= ucDram_Register_Read(mcSET_DRAMC_REG_ADDR(0x1b0), &uiTemp);
    mcCLR_MASK(uiTemp, 0xF<<24);
    mcSET_MASK(uiTemp, iCAShift[2]<<24);
    ucstatus |= ucDram_Register_Write(mcSET_DRAMC_REG_ADDR(0x1b0), uiTemp);

    // Set CA3 output delay
    // CA3 - RA3 - 0x1a8 24~27
    ucstatus |= ucDram_Register_Read(mcSET_DRAMC_REG_ADDR(0x1a8), &uiTemp);
    mcCLR_MASK(uiTemp, 0xF<<24);
    mcSET_MASK(uiTemp, iCAShift[3]<<24);
    ucstatus |= ucDram_Register_Write(mcSET_DRAMC_REG_ADDR(0x1a8), uiTemp);

    // Set CA4 output delay
    // CA4 - RA0 - 0x1a8 0~3
    ucstatus |= ucDram_Register_Read(mcSET_DRAMC_REG_ADDR(0x1a8), &uiTemp);
    mcCLR_MASK(uiTemp, 0xF);
    mcSET_MASK(uiTemp, iCAShift[4]);
    ucstatus |= ucDram_Register_Write(mcSET_DRAMC_REG_ADDR(0x1a8), uiTemp);

    // Set CA5 output delay
    // CA5 - RA5 - 0x1ac 8~11
    ucstatus |= ucDram_Register_Read(mcSET_DRAMC_REG_ADDR(0x1ac), &uiTemp);
    mcCLR_MASK(uiTemp, 0xF<<8);
    mcSET_MASK(uiTemp, iCAShift[5]<<8);
    ucstatus |= ucDram_Register_Write(mcSET_DRAMC_REG_ADDR(0x1ac), uiTemp);

    // Set CA6 output delay
    // CA6 - RA9 - 0x1b0 8~11
    ucstatus |= ucDram_Register_Read(mcSET_DRAMC_REG_ADDR(0x1b0), &uiTemp);
    mcCLR_MASK(uiTemp, 0xF<<8);
    mcSET_MASK(uiTemp, iCAShift[6]<<8);
    ucstatus |= ucDram_Register_Write(mcSET_DRAMC_REG_ADDR(0x1b0), uiTemp);

    // Set CA7 output delay
    // CA7 - RA13 - 0x1bc 16~20
    ucstatus |= ucDram_Register_Read(mcSET_DRAMC_REG_ADDR(0x1bc), &uiTemp);
    mcCLR_MASK(uiTemp, 0xF<<16);
    mcSET_MASK(uiTemp, iCAShift[7]<<16);
    ucstatus |= ucDram_Register_Write(mcSET_DRAMC_REG_ADDR(0x1bc), uiTemp);

    // Set CA8 output delay
    // CA8 - RA15- 0x1c0 16~20
    ucstatus |= ucDram_Register_Read(mcSET_DRAMC_REG_ADDR(0x1c0), &uiTemp);
    mcCLR_MASK(uiTemp, 0xF<<16);
    mcSET_MASK(uiTemp, iCAShift[8]<<16);
    ucstatus |= ucDram_Register_Write(mcSET_DRAMC_REG_ADDR(0x1c0), uiTemp);

    // Set CA9 output delay
    // CA9 - RA2 - 0x1a8 16~19
    ucstatus |= ucDram_Register_Read(mcSET_DRAMC_REG_ADDR(0x1a8), &uiTemp);
    mcCLR_MASK(uiTemp, 0xF<<16);
    mcSET_MASK(uiTemp, iCAShift[9]<<16);
    ucstatus |= ucDram_Register_Write(mcSET_DRAMC_REG_ADDR(0x1a8), uiTemp);
    #endif
#endif

#ifdef CKE_CS_DLY_SETTING
    //CAShift_Avg = (CAShift_Avg + (CATRAINING_NUM>>1)) /CATRAINING_NUM;
    //u4CAshift_Avg = (CAShift_Avg + (CATRAINING_NUM>>1))/CATRAINING_NUM;
	u4CAshift_Avg = (U32)(CAShift_Avg + (CATRAINING_NUM>>1)) /CATRAINING_NUM;

    // CKEDLY : Reg.1B8h[12:8].
    // CSDLY : Reg.1B8h[4:0]
    ucstatus |= ucDram_Register_Read(mcSET_DRAMC_REG_ADDR(0x1b8), &uiTemp);
    uiTemp = (uiTemp & 0xffffe0e0) | (u4CAshift_Avg << 8) | (u4CAshift_Avg << 0);
    ucstatus |= ucDram_Register_Write(mcSET_DRAMC_REG_ADDR(0x1b8), uiTemp);
    CA_PRINT("Reg.1B8h=%xh\n", uiTemp);

    // CKE1DLY : Reg.1C4h[28:24]
    ucstatus |= ucDram_Register_Read(mcSET_DRAMC_REG_ADDR(0x1c4), &uiTemp);
    uiTemp = (uiTemp & 0xe0ffffff) | (u4CAshift_Avg << 24);
    ucstatus |= ucDram_Register_Write(mcSET_DRAMC_REG_ADDR(0x1c4), uiTemp);
    CA_PRINT("Reg.1C4h=%xh\n", uiTemp);

    // CS1DLY : Reg.0Ch[31:28]
    ucstatus |= ucDram_Register_Read(mcSET_DRAMC_REG_ADDR(0x00c), &uiTemp);
    uiTemp = (uiTemp & 0x0fffffff) | (u4CAshift_Avg << 28);
    ucstatus |= ucDram_Register_Write(mcSET_DRAMC_REG_ADDR(0x00c), uiTemp);
    CA_PRINT("Reg.00ch=%xh\n", uiTemp);
#endif

    DramcEnterSelfRefresh(1);
    // Write max center value into Clk output delay.
    ucstatus |= ucDram_Register_Read(mcSET_DRAMC_REG_ADDR(0x0c), &uiTemp);
    mcSET_FIELD(uiTemp, iMaxCenter, 0x0f000000, 24);
    ucstatus |= ucDram_Register_Write(mcSET_DRAMC_REG_ADDR(0x0c), uiTemp);
    CA_PRINT("Reg.0Ch=%xh\n", uiTemp);
    mcFPRINTF((fp_A60808, "Reg.0Ch=%xh\n", uiTemp));
    DramcEnterSelfRefresh(0);
#ifdef WL_CLKADJUST
    CATrain_ClkDelay[0] = iMaxCenter;
    CA_PRINT(("CATrain_ClkDelay=%d...\n", CATrain_ClkDelay[0]));
#endif

    CA_PRINT("0x1a8=%xh\n", DRAMC_READ_REG(0x1a8));
    CA_PRINT("0x1ac=%xh\n", DRAMC_READ_REG(0x1ac));
    CA_PRINT("0x1b0=%xh\n", DRAMC_READ_REG(0x1b0));
    CA_PRINT("0x1b4=%xh\n", DRAMC_READ_REG(0x1b4));
    CA_PRINT("0x1b8=%xh\n", DRAMC_READ_REG(0x1b8));
    CA_PRINT("0x1bc=%xh\n", DRAMC_READ_REG(0x1bc));
    CA_PRINT("0x1c0=%xh\n", DRAMC_READ_REG(0x1c0));
    CA_PRINT("0x1c4=%xh\n", DRAMC_READ_REG(0x1c4));
    CA_PRINT("0x00c=%xh\n", DRAMC_READ_REG(0xc));

    ucstatus |= ucDram_Register_Write(mcSET_DRAMC_REG_ADDR(0x0fc), u4prv_register_0fc);
    ucstatus |= ucDram_Register_Write(mcSET_DRAMC_REG_ADDR(DRAMC_REG_TEST2_3), u4prv_register_044);
    ucstatus |= ucDram_Register_Write(mcSET_DRAMC_REG_ADDR(0x63c), u4prv_register_63c);

    ucDram_Register_Write(mcSET_DRAMC_REG_ADDR(DRAMC_REG_MRS), u4MRSValue);

    if (ucstatus)
    {
        CA_PRINT("register access fail!\n");
        return -1;
    }
    else
    {
        return 0;
    }
}

#endif


#ifdef WRITE_LEVELING
#define fcWL_ALL   // Do Write Leveling with all DQS together //8163: todo: need enable this config

// DRAMC registers
#define DRAMC_REG_PADCTL2 0x010
#define DRAMC_REG_PADCTL3 0x014
#define DRAMC_REG_DDR2CTL 0x07c
    //#define MASK_DDR2CTL_DATLAT 0x00000070 //todo:0x80 if 8163
    //#define POS_DDR2CTL_DTALAT 4 //todo:0x80 if 8163
    #define POS_DDR2CTL_WOEN 3

#define DRAMC_REG_MCKDLY 0x0d8
    //bit 23
    //#define POS_MCKDLY_FIXODT 22 //todo:bit 23 is error, bit 23->22
    //bit 12~15
    #define POS_MCKDLY_FIXDQIEN 12
    #define MASK_MCKDLY_FIXDQIEN 0x0000f000

#define DRAMC_REG_PADCTL4 0x0e4
    #define MASK_PADCTL4_DATLAT3 0x00000010
    #define POS_PADCTL4_DATLAT3 4
    #define POS_PADCTL4_CKEFIXON 2

#define DRAMC_REG_WRLEV 0x13c
    //bit 12
    #define POS_WRLEV_DQS_SEL 12
    #define MASK_WRLEV_DQS_SEL 0x0000f000
    //bit 8
    #define POS_WRLEV_DQS_WLEV 8
    //bit 1~4
    #define POS_WRLEV_DQS_Bx_G 1
    #define MASK_WRLEV_DQS_Bx_G 0x0000001e
    //bit 0
    #define POS_WRLEV_WRITE_LEVEL_EN 0
#define DRAMC_REG_LPDDR2 0x1e0
    #define POS_LPDDR2_ADRDECEN 31
    #define POS_LPDDR2_SELO1ASO 30
#define DRAMC_REG_SPCMD 0x1e4
    #define POS_SPCMD_MRWEN 0
    #define POS_SPCMD_DQSGCNTEN 8
    #define POS_SPCMD_DQSGCNTRST 9
#define DRAMC_REG_DQODLY1 0x200

int DramcWriteLeveling(void)
{
// Note that below procedure is based on "ODT off"
    unsigned int ucstatus = 0;
    unsigned int u4value, u4dq_o1;
    unsigned int u4prv_register_0e4, u4prv_register_13c, u4prv_register_0d8, u4prv_register_008;
    unsigned int ucsample_status[DQS_NUMBER], ucdq_o1_perbyte[DQS_NUMBER], ucdq_o1_index[DQS_NUMBER];
    unsigned int ii, byte_i, ucsample_count;
    unsigned int fgwrlevel_done = 0;
    //unsigned int ucwrlevel_dqs_final_delay[DQS_NUMBER];
    //unsigned int wrlevel_dqs_final_delay[DQS_NUMBER];
#ifdef DQ_DQS_DQM_REMAPPING
    unsigned int uiRemapByte;
    unsigned int uiRemapDQ;
    unsigned int uiRemapDQValue[4];
#endif
    U32 u4MRSValue = 0;


    if (TYPE_LPDDR2== mt_get_dram_type())
    {
        WL_PRINT(("Wrong DRAM TYPE.LPDDR2 not support WriteLeveling!!\n"));
        return -1;
    }

    // mt8163 PCDDR3 close Write Leveling
    if (TYPE_LPDDR3 != mt_get_dram_type())
    {
        mcSHOW_ERR_MSG(("Wrong DRAM TYPE. Only support LPDDR3 in Write Leveling!!\n"));
        return -1;
    }

#if defined(DQ_DQS_DQM_REMAPPING)
    unsigned char *Bit_DQO_Mapping;
    if((DRAMC_READ_REG(0x00f0)>>31)==0x01)
    {//DM4BITMUX enable
        Bit_DQO_Mapping = (unsigned char *)Bit_DQO_Mapping_4bitmux;
    }
    else
    {
        Bit_DQO_Mapping = (unsigned char *)Bit_DQO_Mapping_no_4bitmux;
    }
#endif

    fgwrlevel_done = 0;

    // this depends on pinmux
    // select first bit of each byte
    // dq_o1 is @ DDRPHY
    if (TYPE_LPDDR3 == mt_get_dram_type())
    {
        ucdq_o1_index[0]=(U8) uiLPDDR_PHY_Mapping_POP_CHA[0];
        ucdq_o1_index[1]=(U8) uiLPDDR_PHY_Mapping_POP_CHA[8];
        ucdq_o1_index[2]=(U8) uiLPDDR_PHY_Mapping_POP_CHA[16];
        ucdq_o1_index[3]=(U8) uiLPDDR_PHY_Mapping_POP_CHA[24];
    }
    else
    {
		ucdq_o1_index[0]=7;
		ucdq_o1_index[1]=8;
		ucdq_o1_index[2]=17;
		ucdq_o1_index[3]=28;
    }

    ucDram_Register_Read(mcSET_DRAMC_REG_ADDR(DRAMC_REG_MRS), &u4MRSValue);
    WL_PRINT("MRS Register 0x%x value: 0x%x\n", DRAMC_REG_MRS, u4MRSValue);

    // backup mode settings
    //ucstatus |= ucDram_Register_Read(mcSET_DRAMC_REG_ADDR(DRAMC_REG_DRAMC_PD_CTRL), &u4prv_register_1dc);
    //ucstatus |= ucDram_Register_Read(mcSET_DRAMC_REG_ADDR(DRAMC_REG_TEST2_3), &u4prv_register_044);
    ucstatus |= ucDram_Register_Read(mcSET_DRAMC_REG_ADDR(DRAMC_REG_PADCTL4), &u4prv_register_0e4);
    ucstatus |= ucDram_Register_Read(mcSET_DRAMC_REG_ADDR(DRAMC_REG_WRLEV), &u4prv_register_13c);
    ucstatus |= ucDram_Register_Read(mcSET_DRAMC_REG_ADDR(DRAMC_REG_MCKDLY), &u4prv_register_0d8);
    ucstatus |= ucDram_Register_Read(mcSET_DRAMC_REG_ADDR(0x0008), &u4prv_register_008);

    //write leveling mode initialization
    //Make CKE fixed at 1 (Put this before issuing MRS): CKEFIXON = 1 (0xe4[2])
    ucstatus |= ucDram_Register_Read(mcSET_DRAMC_REG_ADDR(DRAMC_REG_PADCTL4), &u4value);
    mcSET_BIT(u4value, POS_PADCTL4_CKEFIXON);
    ucstatus |= ucDram_Register_Write(mcSET_DRAMC_REG_ADDR(DRAMC_REG_PADCTL4), u4value);

    //Disable auto refresh: REFCNT_FR_CLK = 0 (0x1dc[23:16]), ADVREFEN = 0 (0x44[30])
    // After Denali-1, only need to set DISREF (0x0008[28]=1)
    ucstatus |= ucDram_Register_Read(mcSET_DRAMC_REG_ADDR(0x0008), &u4value); //todo:
    //mcCLR_MASK(u4value, 0x000000ff);
    mcSET_BIT(u4value, 28);
    ucstatus |= ucDram_Register_Write(mcSET_DRAMC_REG_ADDR(0x0008), u4value);

    //ucstatus |= ucDram_Register_Read(mcSET_DRAMC_REG_ADDR(DRAMC_REG_DRAMC_PD_CTRL), &u4value);
    //mcCLR_MASK(u4value, MASK_DRAMC_PD_CTRL_REFCNT_FR_CLK);
    //ucstatus |= ucDram_Register_Write(mcSET_DRAMC_REG_ADDR(DRAMC_REG_DRAMC_PD_CTRL), u4value);

    //ucstatus |= ucDram_Register_Read(mcSET_DRAMC_REG_ADDR(DRAMC_REG_TEST2_3), &u4value);
    //mcCLR_BIT(u4value, POS_TEST2_3_ADVREFEN);
    //ucstatus |= ucDram_Register_Write(mcSET_DRAMC_REG_ADDR(DRAMC_REG_TEST2_3), u4value);

/*
    //Enable Write ODT: WOEN = 1 (0x7c[3])
    //may no need to set here, initial value
    ucstatus |= ucDram_Register_Read(mcSET_DRAMC_REG_ADDR(DRAMC_REG_DDR2CTL), &u4value);
    mcSET_BIT(u4value, POS_DDR2CTL_WOEN);
    ucstatus |= ucDram_Register_Write(mcSET_DRAMC_REG_ADDR(DRAMC_REG_DDR2CTL), u4value);

    //ODT, DQIEN fixed at 1; FIXODT = 1 (0xd8[23]), FIXDQIEN = 1111 (0xd8[15:12])
    ucstatus |= ucDram_Register_Read(mcSET_DRAMC_REG_ADDR(DRAMC_REG_MCKDLY), &u4value);
    mcSET_BIT(u4value, POS_MCKDLY_FIXODT);
    mcSET_MASK(u4value, MASK_MCKDLY_FIXDQIEN);
    ucstatus |= ucDram_Register_Write(mcSET_DRAMC_REG_ADDR(DRAMC_REG_MCKDLY), u4value);
*/
    //FIXDQIEN = 1111 (0xd8[15:12]), keep dq input always on
    ucstatus |= ucDram_Register_Read(mcSET_DRAMC_REG_ADDR(DRAMC_REG_MCKDLY), &u4value);
    mcSET_MASK(u4value, MASK_MCKDLY_FIXDQIEN);
    ucstatus |= ucDram_Register_Write(mcSET_DRAMC_REG_ADDR(DRAMC_REG_MCKDLY), u4value);

    //Enable DQ_O1, SELO1ASO=1 (0x1e0[30]=1), select IO O1 as output
    ucstatus |= ucDram_Register_Read(mcSET_DRAMC_REG_ADDR(DRAMC_REG_LPDDR2), &u4value);
    mcSET_BIT(u4value, POS_LPDDR2_SELO1ASO);
    ucstatus |= ucDram_Register_Write(mcSET_DRAMC_REG_ADDR(DRAMC_REG_LPDDR2), u4value);

    // enable DDR write leveling mode
    if (TYPE_PCDDR3 == mt_get_dram_type())
    {
        //issue MR1[7] to enable write leveling
        ucstatus |= ucDram_Register_Write(mcSET_DRAMC_REG_ADDR(DRAMC_REG_MRS), u4MRSValue | 0x00000080);
        ucstatus |= ucDram_Register_Read(mcSET_DRAMC_REG_ADDR(DRAMC_REG_SPCMD), &u4value);
        mcSET_BIT(u4value, POS_SPCMD_MRWEN);
        ucstatus |= ucDram_Register_Write(mcSET_DRAMC_REG_ADDR(DRAMC_REG_SPCMD), u4value);
        mcDELAY_US(1);
        mcCLR_BIT(u4value, POS_SPCMD_MRWEN);
        ucstatus |= ucDram_Register_Write(mcSET_DRAMC_REG_ADDR(DRAMC_REG_SPCMD), u4value);
    }
    else if (TYPE_LPDDR3 == mt_get_dram_type())
    {
        // issue MR2[7] to enable write leveling (refer to DEFAULT MR2 value)
        ucstatus |= ucDram_Register_Write(mcSET_DRAMC_REG_ADDR(DRAMC_REG_MRS), u4MRSValue | 0x00800000);
        ucstatus |= ucDram_Register_Read(mcSET_DRAMC_REG_ADDR(DRAMC_REG_SPCMD), &u4value);
        mcSET_BIT(u4value, POS_SPCMD_MRWEN);
        ucstatus |= ucDram_Register_Write(mcSET_DRAMC_REG_ADDR(DRAMC_REG_SPCMD), u4value);
        mcDELAY_US(1);
        mcCLR_BIT(u4value, POS_SPCMD_MRWEN);
        ucstatus |= ucDram_Register_Write(mcSET_DRAMC_REG_ADDR(DRAMC_REG_SPCMD), u4value);
    }

    //wait tWLDQSEN (25 nCK / 25ns) after enabling write leveling mode (DDR3 / LPDDDR3)
    mcDELAY_US(1);

    //Set {R_DQS_B3_G R_DQS_B2_G R_DQS_B1_G R_DQS_B0_G}=1010: 0x13c[4:1] (this depends on sel_ph setting)
    //Enable Write leveling: 0x13c[0]
    ucstatus |= ucDram_Register_Read(mcSET_DRAMC_REG_ADDR(DRAMC_REG_WRLEV), &u4value);
    mcSET_FIELD(u4value, 0xa, MASK_WRLEV_DQS_Bx_G, POS_WRLEV_DQS_Bx_G);
    mcSET_BIT(u4value, POS_WRLEV_WRITE_LEVEL_EN);
    ucstatus |= ucDram_Register_Write(mcSET_DRAMC_REG_ADDR(DRAMC_REG_WRLEV), u4value);

    // wait tWLMRD (40 nCL / 40 ns) before DQS pulse (DDR3 / LPDDR3)
    mcDELAY_US(1);

    //Proceed write leveling...
    //Initilize sw parameters
    for (ii=0; ii < (32/DQS_BIT_NUMBER); ii++)
    {
        ucsample_status[ii] = 0;
        wrlevel_dqs_final_delay[ii] = 0;
        //ucwrlevel_dqs_final_delay[ii] = 0;
    }
    //used for early break
    ucsample_count = 0;

    WL_PRINT("===============================================================================\n");
    WL_PRINT("\n        dramc_write_leveling_swcal\n");
    WL_PRINT("===============================================================================\n");
    WL_PRINT("delay  byte0  byte1  byte2  byte3\n");
    WL_PRINT("-----------------------------\n");

    //not sure LP3 can be WL together
#ifndef fcWL_ALL
    for (byte_i = 0; byte_i < (32/DQS_BIT_NUMBER);  byte_i++)
#endif
    {
    #ifndef fcWL_ALL
        // select respective DQS
        ucstatus |= ucDram_Register_Read(mcSET_DRAMC_REG_ADDR(DRAMC_REG_WRLEV), &u4value);
        mcSET_FIELD(u4value, ((U8)1<<byte_i), MASK_WRLEV_DQS_SEL, POS_WRLEV_DQS_SEL);
        ucstatus |= ucDram_Register_Write(mcSET_DRAMC_REG_ADDR(DRAMC_REG_WRLEV), u4value);
    #else
        // select all DQS (0x13c[15:12]=1111)
        ucstatus |= ucDram_Register_Read(mcSET_DRAMC_REG_ADDR(DRAMC_REG_WRLEV), &u4value);
        mcSET_FIELD(u4value, 0xf, MASK_WRLEV_DQS_SEL, POS_WRLEV_DQS_SEL);
        ucstatus |= ucDram_Register_Write(mcSET_DRAMC_REG_ADDR(DRAMC_REG_WRLEV), u4value);
    #endif

        for (ii=0; ii<MAX_TX_DQSDLY_TAPS; ii++)
        {
        #ifndef fcWL_ALL
            // set DQS delay
            ucstatus |= ucDram_Register_Read(mcSET_DRAMC_REG_ADDR(DRAMC_REG_PADCTL3), &u4value);
            mcSET_FIELD(u4value, ii, ((U32)0xf)<<(4*byte_i), 4*byte_i);
            ucstatus |= ucDram_Register_Write(mcSET_DRAMC_REG_ADDR(DRAMC_REG_PADCTL3), u4value);
        #else
            u4value = 0;
            for (byte_i= 0; byte_i < (32/DQS_BIT_NUMBER); byte_i++)
            {
                u4value += (((U32)ii)<<(4*byte_i));
            }
            ucstatus |= ucDram_Register_Write(mcSET_DRAMC_REG_ADDR(DRAMC_REG_PADCTL3), u4value);
        #endif

            //Trigger DQS pulse, R_DQS_WLEV: 0x13c[8] from 1 to 0
            ucstatus |= ucDram_Register_Read(mcSET_DRAMC_REG_ADDR(DRAMC_REG_WRLEV), &u4value);
            mcSET_BIT(u4value, POS_WRLEV_DQS_WLEV);
            ucstatus |= ucDram_Register_Write(mcSET_DRAMC_REG_ADDR(DRAMC_REG_WRLEV), u4value);
            mcCLR_BIT(u4value, POS_WRLEV_DQS_WLEV);
            ucstatus |= ucDram_Register_Write(mcSET_DRAMC_REG_ADDR(DRAMC_REG_WRLEV), u4value);

            //wait tWLO (7.5ns / 20ns) before output (DDR3 / LPDDR3)
            mcDELAY_US(1);

            //Read DQ_O1 from register, 0x380
            ucstatus |= ucDram_Register_Read(mcSET_DRAMC_REG_ADDR(0x380), &u4dq_o1);
            WL_PRINT("loop: %d DQ_O1: 0x%x \n", ii, u4dq_o1);

        #ifdef fcWL_ALL
            WL_PRINT("%d    ", ii);
            for (byte_i = 0; byte_i < (32/DQS_BIT_NUMBER);  byte_i++)
        #endif
            {
                ucdq_o1_perbyte[byte_i] = (U8)((u4dq_o1>>ucdq_o1_index[byte_i])&0x00000001);

            #ifndef fcWL_ALL
                WL_PRINT("%d    %d\n", ii,  ucdq_o1_perbyte[byte_i]);
            #else
                WL_PRINT("%d    ", ucdq_o1_perbyte[byte_i]);
            #endif

                //sample from 0 to 1
                if ((ucsample_status[byte_i]==0) && (ucdq_o1_perbyte[byte_i]==1))
                {
                    ucsample_status[byte_i] = 2;
                    //record delay value
                    wrlevel_dqs_final_delay[byte_i] = 0;
                    //ucwrlevel_dqs_final_delay[byte_i] = 0;
                #ifndef fcWL_ALL
                    // to the next byte
                    //break;
                #else
                    //used for early break
                    ucsample_count++;
                #endif
                }
                else if ((ucsample_status[byte_i]==0) && (ucdq_o1_perbyte[byte_i]==0))
                {
                    ucsample_status[byte_i] = 1;
                }
                else if ((ucsample_status[byte_i]==1) && (ucdq_o1_perbyte[byte_i]==1))
                {
                    ucsample_status[byte_i] = 2;
                    //record delay value
                    wrlevel_dqs_final_delay[byte_i] = ii;
                    //ucwrlevel_dqs_final_delay[byte_i] = ii;
                #ifndef fcWL_ALL
                    // to the next byte
                    //break;
                #else
                    //used for early break
                    ucsample_count++;
                #endif
                }
            }
        #ifdef fcWL_ALL
            WL_PRINT("\n");
            //early break, may be marked for debug use
            if (ucsample_count==(32/DQS_BIT_NUMBER))
            {
                fgwrlevel_done = 1;
                //break;
            }
        #endif
        }
    }

#ifdef fcWL_ALL
    WL_PRINT("pass bytecount = %d\n", ucsample_count);
#endif
    WL_PRINT("byte_i    status    best delay\n");
    for (byte_i = 0; byte_i < (32/DQS_BIT_NUMBER);  byte_i++)
    {
        SHOW_DBG_MSG("%d    %d    %d\n", byte_i, ucsample_status[byte_i], wrlevel_dqs_final_delay[byte_i]);
        //SHOW_DBG_MSG(("%d    %d    %d\n", byte_i, ucsample_status[byte_i], ucwrlevel_dqs_final_delay[byte_i]));
    }
    WL_PRINT("========================================\n");

    // write leveling done, mode settings recovery if necessary
    // recover mode registers
    if (TYPE_PCDDR3 == mt_get_dram_type())
    {
        //issue MR1[7] to enable write leveling
        ucstatus |= ucDram_Register_Write(mcSET_DRAMC_REG_ADDR(DRAMC_REG_MRS), u4MRSValue);
        ucstatus |= ucDram_Register_Read(mcSET_DRAMC_REG_ADDR(DRAMC_REG_SPCMD), &u4value);
        mcSET_BIT(u4value, POS_SPCMD_MRWEN);
        ucstatus |= ucDram_Register_Write(mcSET_DRAMC_REG_ADDR(DRAMC_REG_SPCMD), u4value);
        mcDELAY_US(1);
        mcCLR_BIT(u4value, POS_SPCMD_MRWEN);
        ucstatus |= ucDram_Register_Write(mcSET_DRAMC_REG_ADDR(DRAMC_REG_SPCMD), u4value);
    }
    else if (TYPE_LPDDR3 == mt_get_dram_type())
    {
        // issue MR2[7] to enable write leveling (refer to DEFAULT MR2 value)
        ucstatus |= ucDram_Register_Write(mcSET_DRAMC_REG_ADDR(DRAMC_REG_MRS), u4MRSValue);
        ucstatus |= ucDram_Register_Read(mcSET_DRAMC_REG_ADDR(DRAMC_REG_SPCMD), &u4value);
        mcSET_BIT(u4value, POS_SPCMD_MRWEN);
        ucstatus |= ucDram_Register_Write(mcSET_DRAMC_REG_ADDR(DRAMC_REG_SPCMD), u4value);
        mcDELAY_US(1);
        mcCLR_BIT(u4value, POS_SPCMD_MRWEN);
        ucstatus |= ucDram_Register_Write(mcSET_DRAMC_REG_ADDR(DRAMC_REG_SPCMD), u4value);
    }

    ucstatus |= ucDram_Register_Write(mcSET_DRAMC_REG_ADDR(DRAMC_REG_WRLEV), u4prv_register_13c);
    //ucstatus |= ucDram_Register_Write(mcSET_DRAMC_REG_ADDR(DRAMC_REG_DRAMC_PD_CTRL), u4prv_register_1dc);
    //ucstatus |= ucDram_Register_Write(mcSET_DRAMC_REG_ADDR(DRAMC_REG_TEST2_3), u4prv_register_044);
    ucstatus |= ucDram_Register_Write(mcSET_DRAMC_REG_ADDR(DRAMC_REG_PADCTL4), u4prv_register_0e4);
    ucstatus |= ucDram_Register_Write(mcSET_DRAMC_REG_ADDR(DRAMC_REG_MCKDLY), u4prv_register_0d8);
    ucstatus |= ucDram_Register_Write(mcSET_DRAMC_REG_ADDR(0x0008), u4prv_register_008);

    //Disable DQ_O1, SELO1ASO=0 for power saving
    ucstatus |= ucDram_Register_Read(mcSET_DRAMC_REG_ADDR(DRAMC_REG_LPDDR2), &u4value);
    mcCLR_BIT(u4value, POS_LPDDR2_SELO1ASO);
    ucstatus |= ucDram_Register_Write(mcSET_DRAMC_REG_ADDR(DRAMC_REG_LPDDR2), u4value);

    // set to best values
    // DQS
    u4value = 0;
    for (byte_i= 0; byte_i < (32/DQS_BIT_NUMBER); byte_i++)
    {
        u4value += (((U32)wrlevel_dqs_final_delay[byte_i])<<(4*byte_i));
        //u4value += (((U32)ucwrlevel_dqs_final_delay[byte_i])<<(4*byte_i));
    }

	#ifndef WRITE_LEVELING_NO_SETTING
    ucstatus |= ucDram_Register_Write(mcSET_DRAMC_REG_ADDR(DRAMC_REG_PADCTL3), u4value);
	#endif

    // DQM
    #ifndef WRITE_LEVELING_NO_SETTING
    ucstatus |= ucDram_Register_Write(mcSET_DRAMC_REG_ADDR(DRAMC_REG_PADCTL2), u4value);

    WL_PRINT("[write leveling]DQS: 0x%x, DQM: 0x%x\n", u4value, u4value);
	#endif

    // DQ delay, each DQ has 4 bits. Each register contains 8-bit DQ's
#ifdef DQ_DQS_DQM_REMAPPING
    for (byte_i = 0 ; byte_i < 4; byte_i ++)
    {
         uiRemapDQValue[byte_i]=0;
    }

    for (byte_i = 0; byte_i < 4; byte_i++)
    {
        uiRemapByte = DQSO_Mapping[byte_i];
        for (ii = 0; ii < DQS_BIT_NUMBER; ii++)
        {
            uiRemapDQ = Bit_DQO_Mapping[(byte_i<<3)+ii];
            uiRemapDQValue[uiRemapDQ>>3] |= (((U32) wrlevel_dqs_final_delay[uiRemapByte]) << (4*(uiRemapDQ & 0x07)));
        }
    }

    for (byte_i = 0; byte_i < 4; byte_i++)
    {
    	#ifndef WRITE_LEVELING_NO_SETTING
        ucstatus |= ucDram_Register_Write(mcSET_DRAMC_REG_ADDR(DRAMC_REG_DQODLY1+4*byte_i), uiRemapDQValue[byte_i]);
        WL_PRINT("[write leveling after remap]DQ byte%d reg: 0x%x val: 0x%x\n", byte_i, mcSET_DRAMC_REG_ADDR(DRAMC_REG_DQODLY1+4*byte_i), uiRemapDQValue[byte_i]);
		#endif
    }
#else
    for (byte_i = 0; byte_i < (32/DQS_BIT_NUMBER); byte_i++)
    {
        u4value = 0;
        for (ii = 0; ii < DQS_BIT_NUMBER; ii++)
        {
            u4value += (((U32) wrlevel_dqs_final_delay[byte_i]) << (4*ii));
            //u4value += (((U32) ucwrlevel_dqs_final_delay[byte_i]) << (4*ii));
        }

		#ifndef WRITE_LEVELING_NO_SETTING
        ucstatus |= ucDram_Register_Write(mcSET_DRAMC_REG_ADDR(DRAMC_REG_DQODLY1+4*byte_i), u4value);
        WL_PRINT("[write leveling]DQ byte%d reg: 0x%x val: 0x%x\n", byte_i, mcSET_DRAMC_REG_ADDR(DRAMC_REG_DQODLY1+4*byte_i), u4value);
		#endif
    }
#endif

    if (ucstatus)
    {
        WL_PRINT("register access fail!\n");
        return 1;
    }
    else
    {
        return 0;
    }

    // log example
/*
===================================================================

                dramc_write_leveling_swcal
                apply=1 channel=2(2:cha, 3:chb)
===================================================================
delay  byte0  byte1  byte2  byte3
-----------------------------
  0    0    0    0    1
  1    0    0    0    1
  2    0    0    1    1
  3    0    0    1    1
  4    0    0    1    1
  5    0    0    1    1
  6    0    0    1    1
  7    0    0    1    1
  8    0    0    1    1
  9    0    0    1    1
 10    0    0    1    1
 11    1    1    1    1
pass bytecount = 4
byte_i    status    best delay
0         2         11
1         2         11
2         2         2
3         2         0
*/
    return 0;
}

#endif



