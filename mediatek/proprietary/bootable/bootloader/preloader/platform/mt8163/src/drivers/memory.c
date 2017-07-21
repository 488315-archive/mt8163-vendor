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


#include "typedefs.h"
#include "platform.h"

#include "memory.h"
#include "emi.h"
#include "uart.h"

#define MOD "MEM"

#include "wdt.h"
#include "emi_hw.h"

#if 1 // for using mcDELAY_MS
#include "dramc_common.h"
#endif

extern u32 g_ddr_reserve_enable;
extern  u32 g_ddr_reserve_success;

static void __clear_emi_mpu_vio(void)
{
    u32 dbg_s, dbg_t;

    /* clear violation status */
    DRV_WriteReg32(EMI_MPUP, 0x00FF03FF);
    DRV_WriteReg32(EMI_MPUQ, 0x00FF03FF);
    DRV_WriteReg32(EMI_MPUR, 0x00FF03FF);
    DRV_WriteReg32(EMI_MPUY, 0x00FF03FF);
    DRV_WriteReg32(EMI_MPUP2, 0x00FF03FF);
    DRV_WriteReg32(EMI_MPUQ2, 0x00FF03FF);
    DRV_WriteReg32(EMI_MPUR2, 0x00FF03FF);
    DRV_WriteReg32(EMI_MPUY2, 0x00FF03FF);

    /* clear debug info */
    DRV_WriteReg32(EMI_MPUS, 0x80000000);

    dbg_s = DRV_Reg32(EMI_MPUS);
    dbg_t = DRV_Reg32(EMI_MPUT);

    /* MT6582 EMI hw bug that EMI_MPUS[10:0] and EMI_MPUT can't be cleared */
    dbg_s &= 0xFFFF0000;
    if (dbg_s) {
        print("Fail to clear EMI MPU violation\n");
        print("EMI_MPUS = %x, EMI_MPUT = %x", dbg_s, dbg_t);
    }
}

extern void print_DBG_info();

#if MEM_TEST
int complex_mem_test (unsigned int start, unsigned int len);
#endif

// --------------------------------------------------------
// init EMI
// --------------------------------------------------------
void
mt_mem_init (void)
{
  unsigned int emi_cona;

  /* DDR reserve mode no need to enable memory & test */
  //if((mtk_wdt_boot_check() == WDT_BY_PASS_PWK_REBOOT) && (g_ddr_reserve_enable==1) && (g_ddr_reserve_success==1))
  /*Note: factory reset failed workaround*/
  if((g_ddr_reserve_enable==1) && (g_ddr_reserve_success==1))
  {
      /*EMI register dummy read. Give clock to EMI APB register to avoid DRAM access hang...*/
      emi_cona = *(volatile unsigned *)(EMI_CONA);
      print("[DDR Reserve mode] EMI dummy read CONA = 0x%x\n", emi_cona);
      /* Reset EMI MPU protect setting - otherwise we can not use protected region during boot-up time */
      DRV_WriteReg32(EMI_MPUA, 0x0);
      DRV_WriteReg32(EMI_MPUB, 0x0);
      DRV_WriteReg32(EMI_MPUC, 0x0);
      DRV_WriteReg32(EMI_MPUD, 0x0);
      DRV_WriteReg32(EMI_MPUE, 0x0);
      DRV_WriteReg32(EMI_MPUF, 0x0);
      DRV_WriteReg32(EMI_MPUG, 0x0);
      DRV_WriteReg32(EMI_MPUH, 0x0);
      DRV_WriteReg32(EMI_MPUA2, 0x0);
      DRV_WriteReg32(EMI_MPUB2, 0x0);
      DRV_WriteReg32(EMI_MPUC2, 0x0);
      DRV_WriteReg32(EMI_MPUD2, 0x0);
      DRV_WriteReg32(EMI_MPUE2, 0x0);
      DRV_WriteReg32(EMI_MPUF2, 0x0);
      DRV_WriteReg32(EMI_MPUG2, 0x0);
      DRV_WriteReg32(EMI_MPUH2, 0x0);
      DRV_WriteReg32(EMI_MPUI, 0x0);
      DRV_WriteReg32(EMI_MPUJ, 0x0);
      DRV_WriteReg32(EMI_MPUK, 0x0);
      DRV_WriteReg32(EMI_MPUL, 0x0);
      DRV_WriteReg32(EMI_MPUI_2ND, 0x0);
      DRV_WriteReg32(EMI_MPUJ_2ND, 0x0);
      DRV_WriteReg32(EMI_MPUK_2ND, 0x0);
      DRV_WriteReg32(EMI_MPUL_2ND, 0x0);
      DRV_WriteReg32(EMI_MPUI2, 0x0);
      DRV_WriteReg32(EMI_MPUJ2, 0x0);
      DRV_WriteReg32(EMI_MPUK2, 0x0);
      DRV_WriteReg32(EMI_MPUL2, 0x0);
      DRV_WriteReg32(EMI_MPUI2_2ND, 0x0);
      DRV_WriteReg32(EMI_MPUJ2_2ND, 0x0);
      DRV_WriteReg32(EMI_MPUK2_2ND, 0x0);
      DRV_WriteReg32(EMI_MPUL2_2ND, 0x0);
  }
  else /* normal boot */
  {
//#if !(CFG_FPGA_PLATFORM)

    //print("MPUA(1) = 0x%x\n", *(volatile unsigned *)(EMI_MPUA));
    //print("MPUB(1) = 0x%x\n", *(volatile unsigned *)(EMI_MPUB));

    /* Reset EMI MPU protect setting - otherwise we can not use protected region during boot-up time */
    DRV_WriteReg32(EMI_MPUA, 0x0);
    DRV_WriteReg32(EMI_MPUB, 0x0);
    //print("MPUA(2) = 0x%x\n", *(volatile unsigned *)(EMI_MPUA));
    //print("MPUB(2) = 0x%x\n", *(volatile unsigned *)(EMI_MPUB));

    mt_set_emi ();
//#endif

#if MEM_TEST
    {
        int i = 0;
        if ((i = complex_mem_test (0x40000000, MEM_TEST_SIZE)) == 0)
        {
            print ("[%s] complex R/W mem test pass\n", MOD);
            //print_DBG_info();

            #if 0 // TA1: w/r loop test, adjust DQM to find read error
            {
                unsigned int data, dqm0dly, dqm1dly, dqm2dly, dqm3dly, loop;
                unsigned int val =0;

                print ("[%s] TA1 r/w, begin\n", MOD);

                #if 0 // TX: DQS+3, DQ+3 //todo:CLKDLY+3, CMDDLY+3
                print ("[%s] REG 0x%x: 0x%x\n", MOD, DRAMC_DQODLY1, DRAMC_READ_REG(DRAMC_DQODLY1));
                print ("[%s] REG 0x%x: 0x%x\n", MOD, DRAMC_DQODLY2, DRAMC_READ_REG(DRAMC_DQODLY2));
                print ("[%s] REG 0x%x: 0x%x\n", MOD, DRAMC_DQODLY3, DRAMC_READ_REG(DRAMC_DQODLY3));
                print ("[%s] REG 0x%x: 0x%x\n", MOD, DRAMC_DQODLY4, DRAMC_READ_REG(DRAMC_DQODLY4));
                print ("[%s] REG 0x%x: 0x%x\n", MOD, DRAMC_PADCTL3, DRAMC_READ_REG(DRAMC_PADCTL3));

                val =0;
                for (loop = 0; loop < 8; loop++)
                {
                    data = ((DRAMC_READ_REG(DRAMC_DQODLY1)) >> (4*loop)) & 0xF;
                    data += 3;

                    if (data > 0xF)
                        data = 0xF;

                    val |= (data <<(4*loop));
                }
                DRAMC_WRITE_REG(val,DRAMC_DQODLY1);

                val =0;
                for (loop = 0; loop < 8; loop++)
                {
                    data = ((DRAMC_READ_REG(DRAMC_DQODLY2)) >> (4*loop)) & 0xF;
                    data += 3;

                    if (data > 0xF)
                        data = 0xF;

                    val |= (data <<(4*loop));
                }
                DRAMC_WRITE_REG(val,DRAMC_DQODLY2);

                val =0;
                for (loop = 0; loop < 8; loop++)
                {
                    data = ((DRAMC_READ_REG(DRAMC_DQODLY3)) >> (4*loop)) & 0xF;
                    data += 3;

                    if (data > 0xF)
                        data = 0xF;

                    val |= (data <<(4*loop));
                }
                DRAMC_WRITE_REG(val,DRAMC_DQODLY3);

                val =0;
                for (loop = 0; loop < 8; loop++)
                {
                    data = ((DRAMC_READ_REG(DRAMC_DQODLY4)) >> (4*loop)) & 0xF;
                    data += 3;

                    if (data > 0xF)
                        data = 0xF;

                    val |= (data <<(4*loop));
                }
                DRAMC_WRITE_REG(val,DRAMC_DQODLY4);

                val =0;
                for (loop = 0; loop < 4; loop++)
                {
                    data = ((DRAMC_READ_REG(DRAMC_PADCTL3)) >> (4*loop)) & 0xF;
                    data += 3;

                    if (data > 0xF)
                        data = 0xF;

                    val |= (data <<(4*loop));
                }
                DRAMC_WRITE_REG(val,DRAMC_PADCTL3);

                print ("[%s] === After +3 ===\n", MOD);
                print ("[%s] REG 0x%x: 0x%x\n", MOD, DRAMC_DQODLY1, DRAMC_READ_REG(DRAMC_DQODLY1));
                print ("[%s] REG 0x%x: 0x%x\n", MOD, DRAMC_DQODLY2, DRAMC_READ_REG(DRAMC_DQODLY2));
                print ("[%s] REG 0x%x: 0x%x\n", MOD, DRAMC_DQODLY3, DRAMC_READ_REG(DRAMC_DQODLY3));
                print ("[%s] REG 0x%x: 0x%x\n", MOD, DRAMC_DQODLY4, DRAMC_READ_REG(DRAMC_DQODLY4));
                print ("[%s] REG 0x%x: 0x%x\n", MOD, DRAMC_PADCTL3, DRAMC_READ_REG(DRAMC_PADCTL3));
                #endif

                /* set test patern and length*/
                DRAMC_WRITE_REG(0x55000000,0x3C);
                DRAMC_WRITE_REG(0xFFFFFF, 0x40);

                for (dqm0dly = 0; dqm0dly <= 0xF; dqm0dly=dqm0dly+2)
                {
                    for (dqm1dly = 0; dqm1dly <= 0xF; dqm1dly=dqm1dly+2)
                    {
                        for (dqm2dly = 0; dqm2dly <= 0xF; dqm2dly=dqm2dly+2)
                        {
                            for (dqm3dly = 0; dqm3dly <= 0xF; dqm3dly=dqm3dly+2)
                            {
                                DRAMC_WRITE_REG(dqm0dly | (dqm1dly<<4) | (dqm2dly<<8) | (dqm3dly<<12), DRAMC_PADCTL2);

                                print ("[%s] REG 0x10: 0x%x\n", MOD, DRAMC_READ_REG(DRAMC_PADCTL2));

                                for (loop = 0; loop < 2; loop++)
                                {
                                    //Enable TA1
                                    DRAMC_WRITE_SET((1 << 29),DRAMC_CONF2);

                                    //Enable TA2
                                    //DRAMC_WRITE_SET((3 << 30),DRAMC_CONF2);

                                    //Check finish
                                    while(!(DRAMC_READ_REG(DRAMC_TESTRPT)&(1 << 10)));

                                    data = DRAMC_READ_REG(DRAMC_TESTRPT);

                                    if (0 != (data & (1<<14)))
                                    {
                                        print ("[%s] Fail, DQM:0x%x\n", MOD, DRAMC_READ_REG(DRAMC_PADCTL2));
                                    }
                                    else
                                    {
                                        print ("[%s] Pass, DQM:0x%x\n", MOD, DRAMC_READ_REG(DRAMC_PADCTL2));
                                    }

                                    delay_a_while(2);

                                    //Disable TA1
                                    DRAMC_WRITE_CLEAR((1 << 29),DRAMC_CONF2);

                                    // Disable TA2
                                    //DRAMC_WRITE_CLEAR((3 << 30),DRAMC_CONF2);
                                }
                            }
                        }
                    }
                }

                print ("[%s] TA1 r/w, end\n", MOD);
            }
            #endif

            #if 0 // TA1 or TA2: w/r loop test, to find read error
            {
                unsigned int data, loop;

                print ("[%s] TA1 r/w, begin\n", MOD);

                /* set test patern and length*/
                DRAMC_WRITE_REG(0x55000000,0x3C);
                DRAMC_WRITE_REG(0xFFFFFF, 0x40);

                loop =0;

                while (1)
                {
                	if (0 == (loop%2))
                	{
                		//Enable TA1
                		DRAMC_WRITE_SET((1 << 29),DRAMC_CONF2);
                	}
					else
					{
		                //Enable TA2
		                DRAMC_WRITE_SET((3 << 30),DRAMC_CONF2);
					}

                    //Check finish
                    while(!(DRAMC_READ_REG(DRAMC_TESTRPT)&(1 << 10)));

                    data = DRAMC_READ_REG(DRAMC_TESTRPT);

                    if (0 != (data & (1<<14)))
                    {
                        print ("[%s] Fail, DQM:0x%x, %d\n", MOD, DRAMC_READ_REG(DRAMC_PADCTL2), loop);
                    }
                    else
                    {
                        print ("[%s] Pass, DQM:0x%x, %d\n", MOD, DRAMC_READ_REG(DRAMC_PADCTL2), loop);
                    }

                    delay_a_while(2);

					if (0 == (loop%2))
                	{
	                    //Disable TA1
	                    DRAMC_WRITE_CLEAR((1 << 29),DRAMC_CONF2);
					}
					else
					{
		                //Disable TA2
		                DRAMC_WRITE_CLEAR((3 << 30),DRAMC_CONF2);
					}

                    loop++;

                    if (50 == loop)
                    {
                        print ("[%s] === loop ===\n", MOD);
                        loop = 0;
                    }
                }

                print ("[%s] TA1 r/w, end\n", MOD);
            }
            #endif

            #if 0 // TA2: one write, loop read, to find read error
            {
                unsigned int data, cmp_err, loop;

                print ("[%s] 1 write, n read, begin\n", MOD);

                /* set test patern and length*/
                DRAMC_WRITE_REG(0x55000000,0x3C);
                DRAMC_WRITE_REG(0xFFFFFF, 0x40);

                //Test Agent 2 write enabling
                DRAMC_WRITE_SET((1 << 31),DRAMC_CONF2);
                //check write finish
                while(!(DRAMC_READ_REG(DRAMC_TESTRPT)&(1 << 10)));
                //Disable write
                DRAMC_WRITE_CLEAR((1 << 31),DRAMC_CONF2);

                loop =0;

                while (1)
                {
                    //Test Agent 2 read enabling
                    DRAMC_WRITE_SET((1 << 30),DRAMC_CONF2);
                    //check read finish
                    while(!(DRAMC_READ_REG(DRAMC_TESTRPT)&(1 << 10)));

                    delay_a_while(2);

                    cmp_err = DRAMC_READ_REG(DRAMC_CMP_ERR);
                    //Disable read
                    DRAMC_WRITE_CLEAR((1 << 30),DRAMC_CONF2); //disable test agent2 read

                    if (0 != cmp_err)
                    {
                        print ("[%s] cmp_err:0x%x\n", MOD, cmp_err);
                    }

                    loop++;

                    if (50 == loop)
                    {
                        print ("[%s] === loop ===\n", MOD);
                        loop = 0;
                    }
                }

                print ("[%s] 1 write, n read, end\n", MOD);
            }
            #endif

            #if 0 // For collect tDQSS
            while (1)
            {
                i = complex_mem_test (0x40000000, MEM_TEST_SIZE);

                if (i != 0)
                {
                    print ("[%s] complex R/W mem test pass, loop, fail:%x\n", MOD, i);
                }
            }
            #endif

			//while (1);

            #if 0 // For 8163 IO power case, need enter self-refresh state then while loop hang
            print ("[%s] complex R/W mem test pass, REG 0x10004004: 0x%x, test IO power\n", MOD, DRV_Reg32(0x10004004));

            DRV_WriteReg32(0x10004004, DRV_Reg32(0x10004004) | (0x1<<26));

            print ("[%s] complex R/W mem test pass, REG 0x10004004: 0x%x\n", MOD, DRV_Reg32(0x10004004));

            #if 0 // Delay, then disable self refresh
            // wait for a while
            mcDELAY_MS(1000*10);

            DRV_WriteReg32(0x10004004, DRV_Reg32(0x10004004) & ~(0x1<<26));

            print ("[%s] complex R/W mem test pass, REG 0x10004004: 0x%x\n", MOD, DRV_Reg32(0x10004004));

            print ("[%s] complex R/W mem test pass, end IO power\n", MOD);
            #endif

            while (1);

            print ("[%s] complex R/W mem test pass, end 2\n", MOD);
            #endif
       }
        else
        {
            print ("[%s] complex R/W mem test fail :%x\n", MOD, i);
            print_DBG_info();
            ASSERT(0);
        }
    }
#else
	// Tmp test
	*(unsigned int *)0x40000000 = 0x33333333;
	*(unsigned int *)0x40000004 = 0x33333333;
	*(unsigned int *)0x40000008 = 0x33333333;
	*(unsigned int *)0x4000000c = 0x33333333;
	*(unsigned int *)0x40000010 = 0x55555555;
	*(unsigned int *)0x40000014 = 0x55555555;
	*(unsigned int *)0x40000018 = 0x55555555;
	*(unsigned int *)0x4000001c = 0x55555555;
	*(unsigned int *)0x40000020 = 0xaaaaaaaa;
	*(unsigned int *)0x40000024 = 0xaaaaaaaa;
	*(unsigned int *)0x40000028 = 0xaaaaaaaa;
	*(unsigned int *)0x4000002c = 0xaaaaaaaa;
	*(unsigned int *)0x40000030 = 0xffffffff;
	*(unsigned int *)0x40000034 = 0xffffffff;
	*(unsigned int *)0x40000038 = 0xffffffff;
	*(unsigned int *)0x4000003c = 0xffffffff;
	print ("[%s] self test done\n", MOD);
#endif
  }

    __clear_emi_mpu_vio();
#ifdef MTK_LASTPC_SUPPORT
    //while (1);
//#ifdef DDR_RESERVE_MODE
  /* Always enable DDR-reserve mode */
  rgu_dram_reserved(1);
    print ("[%s] WDT 0x10007040:%x\n", MOD, DRV_Reg32(0x10007040));
#endif

}



#if MEM_TEST
// --------------------------------------------------------
// do memory test
// --------------------------------------------------------
#define PATTERN1 0x5A5A5A5A
#define PATTERN2 0xA5A5A5A5

int
complex_mem_test (unsigned int start, unsigned int len)
{
    unsigned char *MEM8_BASE = (unsigned char *) start;
    unsigned short *MEM16_BASE = (unsigned short *) start;
    unsigned int *MEM32_BASE = (unsigned int *) start;
    unsigned int *MEM_BASE = (unsigned int *) start;
    unsigned char pattern8;
    unsigned short pattern16;
    unsigned int i, j, size, pattern32;
    unsigned int value;

    size = len >> 2;

    /* === Verify the tied bits (tied high) === */
    for (i = 0; i < size; i++)
    {
        MEM32_BASE[i] = 0;
    }

    for (i = 0; i < size; i++)
    {
        if (MEM32_BASE[i] != 0)
        {
            return -1;
        }
        else
        {
            MEM32_BASE[i] = 0xffffffff;
        }
    }

    /* === Verify the tied bits (tied low) === */
    for (i = 0; i < size; i++)
    {
        if (MEM32_BASE[i] != 0xffffffff)
        {
            return -2;
        }
        else
            MEM32_BASE[i] = 0x00;
    }

    /* === Verify pattern 1 (0x00~0xff) === */
    pattern8 = 0x00;
    for (i = 0; i < len; i++)
        MEM8_BASE[i] = pattern8++;
    pattern8 = 0x00;
    for (i = 0; i < len; i++)
    {
        if (MEM8_BASE[i] != pattern8++)
        {
            return -3;
        }
    }

    /* === Verify pattern 2 (0x00~0xff) === */
    pattern8 = 0x00;
    for (i = j = 0; i < len; i += 2, j++)
    {
        if (MEM8_BASE[i] == pattern8)
            MEM16_BASE[j] = pattern8;
        if (MEM16_BASE[j] != pattern8)
        {
            return -4;
        }
        pattern8 += 2;
    }

    /* === Verify pattern 3 (0x00~0xffff) === */
    pattern16 = 0x00;
    for (i = 0; i < (len >> 1); i++)
        MEM16_BASE[i] = pattern16++;
    pattern16 = 0x00;
    for (i = 0; i < (len >> 1); i++)
    {
        if (MEM16_BASE[i] != pattern16++)
        {
            return -5;
        }
    }

    /* === Verify pattern 4 (0x00~0xffffffff) === */
    pattern32 = 0x00;
    for (i = 0; i < (len >> 2); i++)
        MEM32_BASE[i] = pattern32++;
    pattern32 = 0x00;
    for (i = 0; i < (len >> 2); i++)
    {
        if (MEM32_BASE[i] != pattern32++)
        {
            return -6;
        }
    }

    /* === Pattern 5: Filling memory range with 0x44332211 === */
    for (i = 0; i < size; i++)
        MEM32_BASE[i] = 0x44332211;

    /* === Read Check then Fill Memory with a5a5a5a5 Pattern === */
    for (i = 0; i < size; i++)
    {
        if (MEM32_BASE[i] != 0x44332211)
        {
            return -7;
        }
        else
        {
            MEM32_BASE[i] = 0xa5a5a5a5;
        }
    }

    /* === Read Check then Fill Memory with 00 Byte Pattern at offset 0h === */
    for (i = 0; i < size; i++)
    {
        if (MEM32_BASE[i] != 0xa5a5a5a5)
        {
            return -8;
        }
        else
        {
            MEM8_BASE[i * 4] = 0x00;
        }
    }

    /* === Read Check then Fill Memory with 00 Byte Pattern at offset 2h === */
    for (i = 0; i < size; i++)
    {
        if (MEM32_BASE[i] != 0xa5a5a500)
        {
            return -9;
        }
        else
        {
            MEM8_BASE[i * 4 + 2] = 0x00;
        }
    }

    /* === Read Check then Fill Memory with 00 Byte Pattern at offset 1h === */
    for (i = 0; i < size; i++)
    {
        if (MEM32_BASE[i] != 0xa500a500)
        {
            return -10;
        }
        else
        {
            MEM8_BASE[i * 4 + 1] = 0x00;
        }
    }

    /* === Read Check then Fill Memory with 00 Byte Pattern at offset 3h === */
    for (i = 0; i < size; i++)
    {
        if (MEM32_BASE[i] != 0xa5000000)
        {
            return -11;
        }
        else
        {
            MEM8_BASE[i * 4 + 3] = 0x00;
        }
    }

    /* === Read Check then Fill Memory with ffff Word Pattern at offset 1h == */
    for (i = 0; i < size; i++)
    {
        if (MEM32_BASE[i] != 0x00000000)
        {
            return -12;
        }
        else
        {
            MEM16_BASE[i * 2 + 1] = 0xffff;
        }
    }


    /* === Read Check then Fill Memory with ffff Word Pattern at offset 0h == */
    for (i = 0; i < size; i++)
    {
        if (MEM32_BASE[i] != 0xffff0000)
        {
            return -13;
        }
        else
        {
            MEM16_BASE[i * 2] = 0xffff;
        }
    }


    /*===  Read Check === */
    for (i = 0; i < size; i++)
    {
        if (MEM32_BASE[i] != 0xffffffff)
        {
            return -14;
        }
    }


    /************************************************
    * Additional verification
    ************************************************/
    /* === stage 1 => write 0 === */

    for (i = 0; i < size; i++)
    {
        MEM_BASE[i] = PATTERN1;
    }


    /* === stage 2 => read 0, write 0xF === */
    for (i = 0; i < size; i++)
    {
        value = MEM_BASE[i];

        if (value != PATTERN1)
        {
            return -15;
        }
        MEM_BASE[i] = PATTERN2;
    }


    /* === stage 3 => read 0xF, write 0 === */
    for (i = 0; i < size; i++)
    {
        value = MEM_BASE[i];
        if (value != PATTERN2)
        {
            return -16;
        }
        MEM_BASE[i] = PATTERN1;
    }


    /* === stage 4 => read 0, write 0xF === */
    for (i = 0; i < size; i++)
    {
        value = MEM_BASE[i];
        if (value != PATTERN1)
        {
            return -17;
        }
        MEM_BASE[i] = PATTERN2;
    }


    /* === stage 5 => read 0xF, write 0 === */
    for (i = 0; i < size; i++)
    {
        value = MEM_BASE[i];
        if (value != PATTERN2)
        {
            return -18;
        }
        MEM_BASE[i] = PATTERN1;
    }


    /* === stage 6 => read 0 === */
    for (i = 0; i < size; i++)
    {
        value = MEM_BASE[i];
        if (value != PATTERN1)
        {
            return -19;
        }
    }


    /* === 1/2/4-byte combination test === */
    i = (unsigned int) MEM_BASE;

    while (i < (unsigned int) MEM_BASE + (size << 2))
    {
        *((unsigned char *) i) = 0x78;
        i += 1;
        *((unsigned char *) i) = 0x56;
        i += 1;
        *((unsigned short *) i) = 0x1234;
        i += 2;
        *((unsigned int *) i) = 0x12345678;
        i += 4;
        *((unsigned short *) i) = 0x5678;
        i += 2;
        *((unsigned char *) i) = 0x34;
        i += 1;
        *((unsigned char *) i) = 0x12;
        i += 1;
        *((unsigned int *) i) = 0x12345678;
        i += 4;
        *((unsigned char *) i) = 0x78;
        i += 1;
        *((unsigned char *) i) = 0x56;
        i += 1;
        *((unsigned short *) i) = 0x1234;
        i += 2;
        *((unsigned int *) i) = 0x12345678;
        i += 4;
        *((unsigned short *) i) = 0x5678;
        i += 2;
        *((unsigned char *) i) = 0x34;
        i += 1;
        *((unsigned char *) i) = 0x12;
        i += 1;
        *((unsigned int *) i) = 0x12345678;
        i += 4;
    }
    for (i = 0; i < size; i++)
    {
        value = MEM_BASE[i];
        if (value != 0x12345678)
        {
            return -20;
        }
    }


    /* === Verify pattern 1 (0x00~0xff) === */
    pattern8 = 0x00;
    MEM8_BASE[0] = pattern8;
    for (i = 0; i < size * 4; i++)
    {
        unsigned char waddr8, raddr8;
        waddr8 = i + 1;
        raddr8 = i;
        if (i < size * 4 - 1)
            MEM8_BASE[waddr8] = pattern8 + 1;
        if (MEM8_BASE[raddr8] != pattern8)
        {
            return -21;
        }
        pattern8++;
    }


    /* === Verify pattern 2 (0x00~0xffff) === */
    pattern16 = 0x00;
    MEM16_BASE[0] = pattern16;
    for (i = 0; i < size * 2; i++)
    {
        if (i < size * 2 - 1)
            MEM16_BASE[i + 1] = pattern16 + 1;
        if (MEM16_BASE[i] != pattern16)
        {
            return -22;
        }
        pattern16++;
    }


    /* === Verify pattern 3 (0x00~0xffffffff) === */
    pattern32 = 0x00;
    MEM32_BASE[0] = pattern32;
    for (i = 0; i < size; i++)
    {
        if (i < size - 1)
            MEM32_BASE[i + 1] = pattern32 + 1;
        if (MEM32_BASE[i] != pattern32)
        {
            return -23;
        }
        pattern32++;
    }

    return 0;
}

#endif
