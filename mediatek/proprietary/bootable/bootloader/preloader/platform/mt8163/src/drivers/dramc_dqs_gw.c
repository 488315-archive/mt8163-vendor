#include <stdlib.h>
#include <typedefs.h>
#include <platform.h>
#include <dramc_common.h>
#include <emi_hw.h>
//#include "custom_emi.h"

#define DRAM_START (0x40000000)
#define RANK_SIZE (0x20000000)

//#define DQS_GW_COARSE_FIXED  "17"
//#define DQS_GW_FINE_FIXED    "56"

#define DQS_GW_COARSE_MAX 32

#define DQS_STROBE_LIMIT (0)
#define DQS_GW_FINE_PER_BYTE (1)

#if DQS_GW_FINE_PER_BYTE
#define GW_PRINT //print
#define DQS_NUM 4
static unsigned int gwCntArr[2]={0, 0}; //record DRAMC_DQSGNWCNT0 and DRAMC_DQSGNWCNT1
static unsigned int dqs_gw_byte[DQS_GW_COARSE_MAX][DQS_NUM];
char *opt_gw_fine_value_byte[DQS_NUM];
#endif
extern int RANK_CURR;
extern int DQS_PRINT;
char **opt_gw_coarse_value, **opt_gw_fine_value;
char **opt_gw_coarse_value0, **opt_gw_fine_value0;
char **opt_gw_coarse_value1, **opt_gw_fine_value1;
void dqs_print(LPCSTR sz, ...)
{
    va_list args;

	//if (DQS_PRINT == 1)
	{
	   va_start(args, sz);
	   vprint(sz, args);
	   va_end(args);
	}   
}

unsigned int score, high_score;
int ett_recursive_factor_tuning(unsigned int n, tuning_factor *tuning_factors, print_callbacks *cbs)
{
    unsigned int i;
    int result;


    //Print banner.
    cbs->ett_print_banner(n);

    if (n == 0) {
        //Before starting the most inner loop, print something.
        cbs->ett_print_before_start_loop_zero();
    }
    
    //print("%s, n:%d, factor_tbl:0x%x, [0]:0x%x, %s\n", __FUNCTION__, n, tuning_factors[n].factor_tbl, tuning_factors[n].factor_tbl[0], (tuning_factors[n].factor_tbl[0]));    

    for ( i = 0 ; tuning_factors[n].factor_tbl[i] != NULL ; i++) {
        //adjust factor steps
        tuning_factors[n].factor_handler(&(tuning_factors[n].factor_tbl[i]), 1);
        //set the current factor steps
        tuning_factors[n].curr_val = tuning_factors[n].factor_tbl[i];
        if (n == 0) {//The most inner loop
            //Before each round of the most inner loop, print something.
            cbs->ett_print_before_each_round_of_loop_zero();
            //run test code
            score += cbs->ett_print_result();
            //After each round of the most inner loop, print something.
            cbs->ett_print_after_each_round_of_loop_zero();
        } else {//Other loops. Call this function recursively.
            ett_recursive_factor_tuning(n-1, tuning_factors, cbs);
        }
    }

    cbs->ett_calc_opt_value(n, &score, &high_score);

    //After finishing the each loop, print something.
    cbs->ett_print_after_finish_loop_n(n);
}


/* DQS gating window (coarse) */
char *dqsi_gw_dly_coarse_tbl[] =
{
	"0","1", "2", "3", "4", "5", 
	"6", "7", "8", "9", "10", "11", 
	"12", "13", "14", "15", "16","17", "18", "19", "20", "21", "22",
	"23", "24", "25", "26", "27", "28", "29", "30", "31",
	NULL,
};

void dqsi_gw_dly_coarse_factor_handler_rank1(char **factor_value, char mode) 
{
    int curr_val = atoi(*factor_value);
    
#if DQS_GW_FINE_PER_BYTE
    GW_PRINT("%s, %x, %s\n", __FUNCTION__, factor_value, *factor_value);
#endif

    DRAMC_WRITE_REG((DRAMC_READ_REG(DRAMC_DQSGCTL/* 0x124 */) & 0xFFFFFF33)	/* Reserve original values for DRAMC_DQSGCTL/ */
      | ((curr_val & 0x3) << 2)			/* R1DQSG_COARSE_DLY_COM0: DRAMC_DQSGCTL[3:2],        2 bits */
      | ((curr_val & 0x3) << 6),			/* R1DQSG_COARSE_DLY_COM1: DRAMC_DQSGCTL[7:6],       2 bits */
      DRAMC_DQSGCTL/* 0x124 */);
}
void dqsi_gw_dly_coarse_factor_handler(char **factor_value, char mode) 
{
    int curr_val = atoi(*factor_value);
    
#if DQS_GW_FINE_PER_BYTE
    GW_PRINT("%s, %x, %s\n", __FUNCTION__, factor_value, *factor_value);
#endif

    //print("dqsi_gw_dly_coarse_factor_handler, curr_val:0x%x\n", curr_val);

    DRAMC_WRITE_REG((DRAMC_READ_REG(DRAMC_DQSCTL1/* 0xE0 */) & 0xF8FFFFFF)	/* Reserve original values for DRAMC_DQSCTL1 */
		| (((curr_val >> 2) & 7) << 24),			/* DQSINCTL: DRAMC_DQSCTL1[26:24],       3bits */
		DRAMC_DQSCTL1/* 0xE0 */);

    DRAMC_WRITE_REG((DRAMC_READ_REG(DRAMC_DQSGCTL/* 0x124 */) & 0xFFFFFFCC)	/* Reserve original values for DRAMC_DQSGCTL/ */
      | ((curr_val & 0x3) << 0)			/* R0DQSG_COARSE_DLY_COM0: DRAMC_DQSGCTL[1:0],        2 bits */
      | ((curr_val & 0x3) << 4),			/* R0DQSG_COARSE_DLY_COM1: DRAMC_DQSGCTL[5:4],       2 bits */
      DRAMC_DQSGCTL/* 0x124 */);
}

/* DQS gating window (fine) */
char *dqsi_gw_dly_fine_tbl[] =
{
	"0",/* "1", "2", "3", "4", "5", "6", "7",*/ "8",/* "9", "10", "11", "12", "13", "14", "15",*/
	"16",/* "17", "18", "19", "20", "21", "22", "23",*/ "24",/* "25", "26", "27", "28", "29", "30", "31",*/
	"32",/* "33", "34", "35", "36", "37", "38", "39",*/ "40",/* "41", "42", "43", "44", "45", "46", "47",*/
	"48",/* "49", "50", "51", "52", "53", "54", "55",*/ "56",/* "57", "58", "59", "60", "61", "62", "63",*/
	"64",/* "65", "66", "67", "68", "69", "70", "71",*/ "72", /* "73", "74", "75", "76", "77", "78", "79",*/
	"80",/* "81", "82", "83", "84", "85", "86", "87",*/ "88",/* "89", "90", "91", "92", "93", "94", "95",*/
	"96",/* "97", "98", "99", "100", "101", "102", "103",*/ "104",/* "105", "106", "107", "108", "109", "110", "111",*/
	"112",/* "113", "114", "115", "116", "117", "118", "119",*/ "120",/* "121", "122", "123", "124", "125", "126", "127",*/
	NULL,
};


void dqsi_gw_dly_fine_factor_handler_rank1(char **factor_value, char mode) 
{
#if DQS_GW_FINE_PER_BYTE
    if (4 == mode)
    {
        DRAMC_WRITE_REG((DRAMC_READ_REG(DRAMC_R1DQSIEN) & 0x80000000)  /* Reserve original values for DRAMC_DQSIEN[31] */
          | ((atoi(*factor_value) & 0x7F) << 0)        /* DQS0IEN: DRAMC_DQSIEN[0:6],   7 bits */
          | ((atoi(*(factor_value + 1)) & 0x7F) << 8)    /* DQS1IEN: DRAMC_DQSIEN[8:14],  7 bits */
          | ((atoi(*(factor_value + 2)) & 0x7F) << 16)   /* DQS2IEN: DRAMC_DQSIEN[16:22], 7 bits */
          | ((atoi(*(factor_value + 3)) & 0x7F) << 24),  /* DQS3IEN: DRAMC_DQSIEN[24:30], 7 bits */
          DRAMC_R1DQSIEN);
        
        GW_PRINT("GW fine hdl 1, factor_value:0x%x, f:0x%x, g:0x%x\n", factor_value, opt_gw_fine_value_byte, dqs_gw_byte);

        GW_PRINT("GW fine hdl 1, BYTE0, %s\n", *factor_value);
        GW_PRINT("GW fine hdl 1, BYTE1, %s\n", *(factor_value+1));
        GW_PRINT("GW fine hdl 1, BYTE2, %s\n", *(factor_value+2));
        GW_PRINT("GW fine hdl 1, BYTE3, %s\n", *(factor_value+3));

        GW_PRINT("GW fine hdl 1, DRAMC_R1DQSIEN:0x%x\n", DRAMC_READ_REG(DRAMC_R1DQSIEN));
    }
    else
#endif
    {
        int curr_val = atoi(*factor_value);
        DRAMC_WRITE_REG((DRAMC_READ_REG(DRAMC_R1DQSIEN/* 0x98 */) & 0x80000000)  /* Reserve original values for DRAMC_DQSIEN[31] */
          | ((curr_val & 0x7F) << 0)		/* DQS0IEN: DRAMC_DQSIEN[0:6],   7 bits */
          | ((curr_val & 0x7F) << 8)	/* DQS1IEN: DRAMC_DQSIEN[8:14],  7 bits */
          | ((curr_val & 0x7F) << 16)	/* DQS2IEN: DRAMC_DQSIEN[16:22], 7 bits */
          | ((curr_val & 0x7F) << 24),	/* DQS3IEN: DRAMC_DQSIEN[24:30], 7 bits */
          DRAMC_R1DQSIEN/* 0x98 */);
    }
}
void dqsi_gw_dly_fine_factor_handler(char **factor_value, char mode) 
{
#if DQS_GW_FINE_PER_BYTE
    if (4 == mode)
    {
        DRAMC_WRITE_REG((DRAMC_READ_REG(DRAMC_R0DQSIEN) & 0x80000000)  /* Reserve original values for DRAMC_DQSIEN[31] */
          | ((atoi(*factor_value) & 0x7F) << 0)        /* DQS0IEN: DRAMC_DQSIEN[0:6],   7 bits */
          | ((atoi(*(factor_value + 1)) & 0x7F) << 8)    /* DQS1IEN: DRAMC_DQSIEN[8:14],  7 bits */
          | ((atoi(*(factor_value + 2)) & 0x7F) << 16)   /* DQS2IEN: DRAMC_DQSIEN[16:22], 7 bits */
          | ((atoi(*(factor_value + 3)) & 0x7F) << 24),  /* DQS3IEN: DRAMC_DQSIEN[24:30], 7 bits */
          DRAMC_R0DQSIEN);
        
        GW_PRINT("GW fine hdl 0, factor_value:0x%x, f:0x%x, g:0x%x\n", __FUNCTION__, factor_value, opt_gw_fine_value_byte, dqs_gw_byte);

        GW_PRINT("GW fine hdl 0, BYTE0, %s\n", *factor_value);
        GW_PRINT("GW fine hdl 0, BYTE1, %s\n", *(factor_value+1));
        GW_PRINT("GW fine hdl 0, BYTE2, %s\n", *(factor_value+2));
        GW_PRINT("GW fine hdl 0, BYTE3, %s\n", *(factor_value+3));

        GW_PRINT("GW fine hdl 0, DRAMC_R0DQSIEN:0x%x\n", DRAMC_READ_REG(DRAMC_R0DQSIEN));
    }
    else
#endif
    {
        int curr_val = atoi(*factor_value);
        
        //print("dqsi_gw_dly_fine_factor_handler, curr_val:0x%x\n", curr_val);

        DRAMC_WRITE_REG((DRAMC_READ_REG(DRAMC_R0DQSIEN/* 0x94 */) & 0x80000000)  /* Reserve original values for DRAMC_DQSIEN[31] */
          | ((curr_val & 0x7F) << 0)		/* DQS0IEN: DRAMC_DQSIEN[0:6],   7 bits */
          | ((curr_val & 0x7F) << 8)	/* DQS1IEN: DRAMC_DQSIEN[8:14],  7 bits */
          | ((curr_val & 0x7F) << 16)	/* DQS2IEN: DRAMC_DQSIEN[16:22], 7 bits */
          | ((curr_val & 0x7F) << 24),	/* DQS3IEN: DRAMC_DQSIEN[24:30], 7 bits */
          DRAMC_R0DQSIEN/* 0x94 */);
    }
}

struct dqs_gw_pass_win
{
    int coarse_end;
    int fine_end;
    int size;   /* gating window's size in this range */
};

void Sequence_Read(unsigned int start, unsigned int len, void *ext_arg)
{
     int i;
    volatile int rval;	
        /* DQS gating window counter reset */
    DRAMC_WRITE_SET((1 << 9),DRAMC_SPCMD);
    DRAMC_WRITE_CLEAR((1 << 9),DRAMC_SPCMD);

     DDR_PHY_RESET_NEW();
     for (i = 0 ; i < len ; i++) {
        rval = *(volatile unsigned int *)(start);
    }

}

//#ifdef MTK_DDR3_SUPPORT

int Do_Read_Test_DDR3(unsigned int start, unsigned int len, void *ext_arg)
{//for DDR3 x16 and DDR3 x32 gating counter
#if 1
	 int err = 0;
	 unsigned int DQSCounter0 = DRAMC_DQSGNWCNT0;
	 unsigned int DQSCounter1 = DRAMC_DQSGNWCNT1;

	 if(((*(volatile unsigned int *)(0x10004004)) & 0x01) == 0x00)
	 {//16bit
		  DQSCounter0 = DRAMC_DQSGNWCNT1;
	 }
	 Sequence_Read(start,len,ext_arg);

#if DQS_GW_FINE_PER_BYTE
      GW_PRINT("\n[1]GNWCNT0:%x, GNWCNT1:%x\n",DRAMC_READ_REG(DRAMC_DQSGNWCNT0),DRAMC_READ_REG(DRAMC_DQSGNWCNT1));
      
      if (NULL != ext_arg)
      {
          *(unsigned int *)ext_arg = DRAMC_READ_REG(DRAMC_DQSGNWCNT0);
          *(unsigned int *)(ext_arg+4) = DRAMC_READ_REG(DRAMC_DQSGNWCNT1);
      }
      else
      {
          GW_PRINT("%s, line%d, ext_art null!!\n", __FUNCTION__, __LINE__);
      }
#endif
     
	  //print("[1]DRAMC_DQSGNWCNT0:%x,DRAMC_DQSGNWCNT1:%x\n",DRAMC_READ_REG(DRAMC_DQSGNWCNT0),DRAMC_READ_REG(DRAMC_DQSGNWCNT1));
	  if (DRAMC_READ_REG(DQSCounter0/* 0x3C0 */) == 0x02020202) {
		  Sequence_Read(start,len,ext_arg);
		 if (DRAMC_READ_REG(DQSCounter1/* 0x3C0 */) == 0x02020202) {
			  err = 0;
		  } else {
			 err = -1;
			 //print("DRAMC_DQSGNWCNT0:%x,DRAMC_DQSGNWCNT1:%x\n",DRAMC_READ_REG(DRAMC_DQSGNWCNT0),DRAMC_READ_REG(DRAMC_DQSGNWCNT1));
		  }
	  } else {
			 err = -1;
			 //print("DRAMC_DQSGNWCNT0:%x,DRAMC_DQSGNWCNT1:%x\n",DRAMC_READ_REG(DRAMC_DQSGNWCNT0),DRAMC_READ_REG(DRAMC_DQSGNWCNT1));
	  }
	  return err;
#else
    int err = 0;
    int check_result = (int)ext_arg;
    unsigned int data;
    unsigned int cmp_err;
    /* set test patern length*/
    DRAMC_WRITE_REG(0x55000000,0x3C);
    data = DRAMC_READ_REG(0x40);
    DRAMC_WRITE_REG((data & 0xAA000000) | 0x3FF, 0x40);


    DRAMC_WRITE_SET((1 << 31),DRAMC_CONF2); //Test Agent 2 write enabling
    DRAMC_WRITE_SET((1 << 30) | (1 << 31),DRAMC_CONF2); //Test Agent 2 write enabling, Test Agent 2 read enabling

    
    while(!(DRAMC_READ_REG(DRAMC_TESTRPT)&(1 << 10)));

    /* 
     * NoteXXX: Need to wait for at least 400 ns 
     *          After checking the simulation result, 
     *          there will be a delay on DLE_CNT_OK/DM_CMP_ERR updates after getting DM_CMP_CPT.
     *          i.e; After getting the complete status, need to wait for a while before reading DLE_CNT_OK/DM_CMP_ERR in the TESTRPT register.
     */
    delay_a_while(400);
	check_result = 0;

   // if (check_result)
	if(1)
	{
        //if (check_result == 0xEEEEEEEE)
			if(1)
			{
            //test agent2 called by tx_factor_tuning
            //do per byte check
             cmp_err = DRAMC_READ_REG(DRAMC_CMP_ERR);           
             //print("ta2 cmp_err:%x\n",cmp_err);
             data = DRAMC_READ_REG(DRAMC_PADCTL2);
             //if (cmp_err)	err = -1;
        }
        else
        {
            if (DRAMC_READ_REG(DRAMC_TESTRPT) & (1 << 14)) {
                err = -1;
            } else if (!(DRAMC_READ_REG(DRAMC_TESTRPT) & (1 << 18))) {
                err = -1;
            }
        }
    }

    DRAMC_WRITE_CLEAR(((1 << 30) | (1 << 31)),DRAMC_CONF2); //disable test agent2 r/w

    if ((int)ext_arg == 0xEEEEEEEE )
    {
        if (!err)
            return 0;
        else
            return -1;
    }
    
    //dbg_print("ext_arg:%x,err:%x\n",ext_arg,err);
#ifdef RX_TEST_BY_BIT
    if (((int)ext_arg == 0xFFFFFFFF) || ((int)ext_arg == 0xeeeeeee0))
#else
    if ((int)ext_arg == 0xFFFFFFFF)
#endif
    {
        return err;
    }

   //print("[2]DRAMC_DQSGNWCNT0:%x,DRAMC_DQSGNWCNT1,%x\n",DRAMC_READ_REG(DRAMC_DQSGNWCNT0),DRAMC_READ_REG(DRAMC_DQSGNWCNT1));
    DDR_PHY_RESET();
    if (!err) {
        //if ((DRAMC_READ_REG(DRAMC_DQSGNWCNT0) == HW_DQS_GW_COUNTER)&& (DRAMC_READ_REG(DRAMC_DQSGNWCNT1) == HW_DQS_GW_COUNTER)) 
		//dbg_print("[3]DRAMC_DQSGNWCNT0:%x,DRAMC_DQSGNWCNT1,%x\n",DRAMC_READ_REG(DRAMC_DQSGNWCNT0),DRAMC_READ_REG(DRAMC_DQSGNWCNT1));

		if((DRAMC_READ_REG(DRAMC_DQSGNWCNT1) == 0xF8F8F8F8)) 
		{
            err = 0;
        } else {
            err = -1;
        }
    }

	//DDR_PHY_RESET();
    /* DQS gating window counter reset */
      DRAMC_WRITE_SET((1 << 9),DRAMC_SPCMD);
      DRAMC_WRITE_CLEAR((1 << 9),DRAMC_SPCMD);
/* 
    //redundant check?? ask CC Hwang
           dbg_print("[3]DRAMC_DQSGNWCNT0:%x,DRAMC_DQSGNWCNT1,%x\n",DRAMC_READ_REG(DRAMC_DQSGNWCNT0),DRAMC_READ_REG(DRAMC_DQSGNWCNT1));
    DDR_PHY_RESET();
    if (!err) {
        if ((DRAMC_READ_REG(DRAMC_DQSGNWCNT0) == HW_DQS_GW_COUNTER)
            && (DRAMC_READ_REG(DRAMC_DQSGNWCNT1) == HW_DQS_GW_COUNTER)) {
            err = 0;
        } else {
            err = -1;
        }
    }
    */
    
dramc_ta2_exit:
    return err;


#endif
}
int Do_Read_Test_DDR2(unsigned int start, unsigned int len, void *ext_arg)
{
    int err = 0;
   
    Sequence_Read(start,len,ext_arg);
    
    #if DQS_GW_FINE_PER_BYTE
    GW_PRINT("\n[1]GNWCNT0:%x, GNWCNT1:%x\n",DRAMC_READ_REG(DRAMC_DQSGNWCNT0),DRAMC_READ_REG(DRAMC_DQSGNWCNT1));
    
    if (NULL != ext_arg)
    {
        *(unsigned int *)ext_arg = DRAMC_READ_REG(DRAMC_DQSGNWCNT0);
        *(unsigned int *)(ext_arg+4) = DRAMC_READ_REG(DRAMC_DQSGNWCNT1);
    }
    else
    {
        GW_PRINT("%s, line%d, ext_art null!!\n", __FUNCTION__, __LINE__);
    }
    #endif
    
    if (DRAMC_READ_REG(DRAMC_DQSGNWCNT0/* 0x3C0 */) == 0x04040404) {
        Sequence_Read(start,len,ext_arg);
       if (DRAMC_READ_REG(DRAMC_DQSGNWCNT0/* 0x3C0 */) == 0x04040404) {
            err = 0;
        } else {
           err = -1;
           //print("[2]GNWCNT0:%x, GNWCNT1:%x\n",DRAMC_READ_REG(DRAMC_DQSGNWCNT0),DRAMC_READ_REG(DRAMC_DQSGNWCNT1));
        }
    } else {
           err = -1;
           //print("[3]GNWCNT0:%x, GNWCNT1:%x\n",DRAMC_READ_REG(DRAMC_DQSGNWCNT0),DRAMC_READ_REG(DRAMC_DQSGNWCNT1));
    }

    return err;
}

//Define how many steps we have in coarse tune, fine tune
//check the number of  dqsi_gw_dly_fine_tbl and dqsi_gw_dly_coarse_tbl
//To-be-porting
//#define DQS_GW_COARSE_MAX 32
#define DQS_GW_FINE_MAX 16
#define DQS_GW_FINE_CHK_RANGE 4
static const int HW_DQS_GW_COUNTER = 0x80808080; 
static unsigned int dqs_gw[DQS_GW_COARSE_MAX];
static int dqs_gw_coarse, dqs_gw_fine;
static struct dqs_gw_pass_win cur_pwin, max_pwin;

/*in MT6589 cpu read test only used in DQS gating window calibration*/
int cpu_read_test(unsigned int start, unsigned int len, void *ext_arg){
   
    int err = 0;
    int check_result = (int)ext_arg;
    unsigned int data;
    /* cpu read test */

    if (mt_get_dram_type()  == TYPE_LPDDR2)    //LPDDR2
        err = Do_Read_Test_DDR2(start, 1, ext_arg);
    else if (mt_get_dram_type() == TYPE_LPDDR3)//LPDDR3
        err = Do_Read_Test_DDR2(start, 1, ext_arg);
    else if (mt_get_dram_type() == TYPE_PCDDR3)//PCDDR3
        err = Do_Read_Test_DDR3(start, 1, ext_arg);
    else
        err = Do_Read_Test_DDR2(start, 1, ext_arg);


    //err = Do_Read_Test(start, len, ext_arg);
    DRAMC_WRITE_SET((1 << 9),DRAMC_SPCMD);
    DRAMC_WRITE_CLEAR((1 << 9),DRAMC_SPCMD);

    DDR_PHY_RESET_NEW();
    if(err !=0)
    {
        return err;
    }

    return 0;
 
}
#if 0
/*in MT6589 test agent 1 only used in DLE calibration*/
int dramc_ta1(unsigned int start, unsigned int len, void *ext_arg){
    int err =  TEST_PASS;
    int check_result = (int)ext_arg;
    unsigned int data;
    /* set test patern length*/
    data = DRAMC_READ_REG(0x40);
    DRAMC_WRITE_REG((data & 0xFF000000) | len, 0x40);


    DRAMC_WRITE_SET((1 << 29) ,DRAMC_CONF2); //Test Agent1 

    
    //print("0x3C:%x\n",DRAMC_READ_REG(0x3c));
    //print("0x40:%x\n",DRAMC_READ_REG(0x40));
    //print("DRAMC_CONF2:%x\n",DRAMC_READ_REG(DRAMC_CONF2));
    while(!(DRAMC_READ_REG(DRAMC_TESTRPT)&(1 << 10)));

    /* 
     * NoteXXX: Need to wait for at least 400 ns 
     *          After checking the simulation result, 
     *          there will be a delay on DLE_CNT_OK/DM_CMP_ERR updates after getting DM_CMP_CPT.
     *          i.e; After getting the complete status, need to wait for a while before reading DLE_CNT_OK/DM_CMP_ERR in the TESTRPT register.
     */
    delay_a_while(400);
    //print("DRAMC_TESTRPT:%x\n",DRAMC_READ_REG(DRAMC_TESTRPT));
    if (check_result) {
        if (DRAMC_READ_REG(DRAMC_TESTRPT) & (1 << 14)) {
            err = -1;
        }
        /*} else if (!(DRAMC_READ_REG(DRAMC_TESTRPT) & (1 << 18))) {
            err = -1;
        }*/
    }

    DRAMC_WRITE_CLEAR((1 << 29) ,DRAMC_CONF2); //disable test agent1

    
    //print("ext_arg:%x,err:%x\n",ext_arg,err);
    if ((int)ext_arg == 0xFFFFFFFF)
    {
        return err;
    }

    return;

}
#endif
/*
 * dramc_ta2: Run DRAMC test agent 2.
 * @start: test start address
 * @len: test length
 * @ext_arg: extend argument (0: don't check read/write results; 1: check)
 * Return error code.
 */
int dramc_ta2(unsigned int start, unsigned int len, void *ext_arg)
{
    int err = 0;
    int check_result = (int)ext_arg;
    unsigned int data;
    /* set test patern length*/
    data = DRAMC_READ_REG(0x40);
    DRAMC_WRITE_REG((data & 0xFF000000) | 0x3FF, 0x40);


    dbg_print("dramc_ta2 enable\n");
    //DRAMC_WRITE_SET((1 << 30) | (1 << 31),DRAMC_CONF2); //Test Agent 2 write enabling, Test Agent 2 read enabling
    DRAMC_WRITE_SET((1 << 30) | (1 << 31),DRAMC_CONF2); //Test Agent 2 write enabling, Test Agent 2 read enabling

    
    while(!(DRAMC_READ_REG(DRAMC_TESTRPT)&(1 << 10)));

    /* 
     * NoteXXX: Need to wait for at least 400 ns 
     *          After checking the simulation result, 
     *          there will be a delay on DLE_CNT_OK/DM_CMP_ERR updates after getting DM_CMP_CPT.
     *          i.e; After getting the complete status, need to wait for a while before reading DLE_CNT_OK/DM_CMP_ERR in the TESTRPT register.
     */
    delay_a_while(400);
    dbg_print("dramc_ta2 done\n");
     
    if (check_result) {
        if (DRAMC_READ_REG(DRAMC_TESTRPT) & (1 << 14)) {
            err = -1;
        } else if (!(DRAMC_READ_REG(DRAMC_TESTRPT) & (1 << 18))) {
            err = -1;
        }
    }

    DRAMC_WRITE_CLEAR(((1 << 30) | (1 << 31)),DRAMC_CONF2); //disable test agent2 r/w

    
    //print("ext_arg:%x,err:%x\n",ext_arg,err);
    if ((int)ext_arg == 0xFFFFFFFF)
    {
        return err;
    }

    print("[2]DRAMC_DQSGNWCNT0:%x,DRAMC_DQSGNWCNT1,%x\n",DRAMC_READ_REG(DRAMC_DQSGNWCNT0),DRAMC_READ_REG(DRAMC_DQSGNWCNT1));
    print("HW_DQS_GW_COUNTER need to be make sure\n");

    DDR_PHY_RESET();
    if (!err) {
        if ((DRAMC_READ_REG(DRAMC_DQSGNWCNT0) == HW_DQS_GW_COUNTER)
            && (DRAMC_READ_REG(DRAMC_DQSGNWCNT1) == HW_DQS_GW_COUNTER)) {
            err = 0;
        } else {
            err = -1;
        }
    }
    /* DQS gating window counter reset */
      DRAMC_WRITE_SET((1 << 9),DRAMC_SPCMD);
      DRAMC_WRITE_CLEAR((1 << 9),DRAMC_SPCMD);
/* 
    //redundant check?? ask CC Hwang
           print("[3]DRAMC_DQSGNWCNT0:%x,DRAMC_DQSGNWCNT1,%x\n",DRAMC_READ_REG(DRAMC_DQSGNWCNT0),DRAMC_READ_REG(DRAMC_DQSGNWCNT1));
    DDR_PHY_RESET();
    if (!err) {
        if ((DRAMC_READ_REG(DRAMC_DQSGNWCNT0) == HW_DQS_GW_COUNTER)
            && (DRAMC_READ_REG(DRAMC_DQSGNWCNT1) == HW_DQS_GW_COUNTER)) {
            err = 0;
        } else {
            err = -1;
        }
    }
    */
dramc_ta2_exit:
    return err;
}

tuning_factor dqs_gw_tuning_factors[] =
{
    {
        .name = "DQS Gating Window Delay (Fine Scale)",	
        .factor_tbl = dqsi_gw_dly_fine_tbl,
        .curr_val = NULL, 
        .opt_val = NULL, 
        .factor_handler = dqsi_gw_dly_fine_factor_handler,
    },
    {
        .name = "DQS Gating Window Delay (Coarse Scale)",
        .factor_tbl = dqsi_gw_dly_coarse_tbl,
        .curr_val = NULL, 
        .opt_val = NULL, 
        .factor_handler = dqsi_gw_dly_coarse_factor_handler,
    },
};
test_case dqs_gw_test_cases_1[] = 
{
    {
        .test_case = cpu_read_test, //use dramc test agent 
        .start = DRAM_START, 
        .range = 0xA, 
        #if DQS_GW_FINE_PER_BYTE
        .ext_arg = (void *)gwCntArr, //check_result, 1st run, no need to check the r/w value
        #else
        .ext_arg = (void *)0,
        #endif
    },
};

test_case dqs_gw_test_cases_2[] = 
{
    {
        .test_case = dramc_ta2, //use dramc test agent 
        .start = DRAM_START, 
        .range = 0xA, 
        .ext_arg = (void *)1, //check_result, 2nd run, it's need to check the r/w value
    },
};
#if 0
test_case dqs_gw_test_cases_1[] =
{
    {
        .test_case = dramc_ta2,
        .start = RANK_SIZE,
        .range = 0xA,
        .ext_arg = (void *)0,
    },
};

test_case dqs_gw_test_cases_2[] =
{
    {
        .test_case = dramc_ta2,
        .start = RANK_SIZE,
        .range = 0xA,
        .ext_arg = (void *)1,
    },
};
#endif

void ett_print_dqs_gw_banner(unsigned int n)
{
    if (n == 1) {
#if 1
        unsigned int i;
        dqs_print("=============================================\n");
        for ( i = 2 ; i < ETT_TUNING_FACTOR_NUMS(dqs_gw_tuning_factors) ; i++) {
            dqs_print("%s = %d\n", dqs_gw_tuning_factors[i].name, atoi(dqs_gw_tuning_factors[i].curr_val));
        }
        dqs_print("X-axis: %s\n", dqs_gw_tuning_factors[0].name);
        if (ETT_TUNING_FACTOR_NUMS(dqs_gw_tuning_factors) > 1) {
            dqs_print("Y-axis: %s\n", dqs_gw_tuning_factors[1].name);
        }
        dqs_print("=============================================\n");
#endif
#if 1 // !defined(RELEASE)
        dqs_print("          0    8   16   24   32   40   48   56   64   72   80   88   96  104  112  120\n");
#endif
        dqs_print("      --------------------------------------------------------------------------------\n");
    }
}

void ett_print_dqs_gw_before_start_loop_zero(void) 
{
#if !defined(RELEASE)
    dqs_print("%H:|", atoi(dqs_gw_tuning_factors[1].curr_val));
#endif
}

void ett_print_dqs_gw_before_each_round_of_loop_zero(void) 
{
    /* DQS gating window counter reset */
    DRAMC_WRITE_SET((1 << 9),DRAMC_SPCMD);
    DRAMC_WRITE_CLEAR((1 << 9),DRAMC_SPCMD);
#if 0
    *(volatile unsigned int *)DRAMC_SPCMD |= (1 << 9);
    *(volatile unsigned int *)DRAMC_SPCMD &= ~(1 << 9);
#endif
    DDR_PHY_RESET();
}

static unsigned int __ett_print_dqs_gw_result(test_case *test_cases, int nr_ts)
{
	unsigned int i, score = 1;

    for (i = 0; i < nr_ts; i++) {
        /* fixup the test start_address due to dual rank*/
        test_cases[i].start = DRAM_START;
        if (test_cases[i].test_case(test_cases[i].start, test_cases[i].range, test_cases[i].ext_arg) < 0) {
            dqs_print("    0");
            score = 0;
            break;
        }

    }
    /*
     * DQS GW calibration rule 1: Identify a pass-window with the max gw.
     */
#if 0
    if (score != 0) {
        cur_pwin.size++;
        //print("(%d)",cur_pwin.size);
    } else if (cur_pwin.size != 0) {
        /* end of the pass-window */
        if (dqs_gw_fine) {
            print("(%d)",dqs_gw_coarse);
            cur_pwin.coarse_end = dqs_gw_coarse;
            cur_pwin.fine_end = dqs_gw_fine - 1;
        } else {
            if (dqs_gw_coarse == 0) {
                /* never happen */ 
                print("Critical error! dqs_gw_coarse = 0 but dqs_gw_fine = 0!\n");
            }
            cur_pwin.coarse_end = dqs_gw_coarse - 1;
            cur_pwin.fine_end = DQS_GW_FINE_MAX - 1;
        }

        /* update the max pass-window */
        if (cur_pwin.size > max_pwin.size) {
            memcpy((void *)&max_pwin, (void *)&cur_pwin, sizeof(struct dqs_gw_pass_win));
        }

        memset((void *)&cur_pwin, 0, sizeof(struct dqs_gw_pass_win));
    }

#endif

    if (score != 0) {
        dqs_print("    1");
        dqs_gw[dqs_gw_coarse] |= (1 << dqs_gw_fine); //attension:dqs_gw_fine steps must less then 32
    }

#if DQS_GW_FINE_PER_BYTE    
    // Record fine tune value of every byte, ignore score(4 byte together)
    {
    unsigned int data = 0x0404;
        
    if (mt_get_dram_type() == TYPE_PCDDR3)
    {
        data = 0x0202;
    }
    
    for (i = 0; i < DQS_NUM; i++) {
        if (data == (((gwCntArr[(i*16)/32]) >> ((i%2)*16)) & 0xFFFF))
        {
            dqs_gw_byte[dqs_gw_coarse][i] |= (1 << dqs_gw_fine); //attension:dqs_gw_fine steps must less then 32            
        }
    }    

    #if 0
    GW_PRINT("\ncoarse:%d, fine:%s, %d%d%d%d\n", dqs_gw_coarse, dqsi_gw_dly_fine_tbl[dqs_gw_fine], 
        ((dqs_gw_byte[dqs_gw_coarse][3] >> dqs_gw_fine) & 1),
        ((dqs_gw_byte[dqs_gw_coarse][2] >> dqs_gw_fine) & 1),
        ((dqs_gw_byte[dqs_gw_coarse][1] >> dqs_gw_fine) & 1),
        ((dqs_gw_byte[dqs_gw_coarse][0] >> dqs_gw_fine) & 1)
        );
    #endif
    }
#endif

    dqs_gw_fine++;
    if (dqs_gw_fine >= DQS_GW_FINE_MAX) { //dqs_gw_fine >= 16
        dqs_gw_coarse++; 
        dqs_gw_fine &= (DQS_GW_FINE_MAX - 1);
    }
    if (dqs_gw_coarse > DQS_GW_COARSE_MAX) {
        dqs_print("Critical error!! dqs_gw_coarse > DQS_GW_COARSE_MAX\n");
    }

    return score;
}

static unsigned int ett_print_dqs_gw_result_1(void)
{
   return __ett_print_dqs_gw_result(dqs_gw_test_cases_1, ETT_TEST_CASE_NUMS(dqs_gw_test_cases_1)); 
}

void ett_print_dqs_gw_after_each_round_of_loop_zero(void) 
{
}

void ett_calc_dqs_gw_opt_value(unsigned int n, unsigned int *score, unsigned int *high_score)
{
}

void ett_print_dqs_gw_after_finish_loop_n(unsigned int n) 
{
    if (n == 0) {
        dqs_print("\n");
    }
}

void ett_print_dqs_gw_fine_after_finish_loop_zero(int n)
{
    if (n == 0){
        dqs_print("\n");
    }
}

/*
 * nr_bit_set: Get the number of bits set in the given value.
 * @val: the gieven value
 * Return the number of bits set.
 */
static int nr_bit_set(unsigned int val)
{
    int i, cnt;

    for (cnt = 0, i = 0; i < (8 * sizeof(unsigned int)); i++) {
        if (val & (1 << i)) {
            cnt++;
        }
    }

    return cnt;
#if 0
    /*
    * Hamming Weight algorithm
    * http://graphics.stanford.edu/~seander/bithacks.html#CountBitsSetParallel
    */
    val = val - ((val >> 1) & 0x55555555);
    val = (val & 0x33333333) + ((val >> 2) & 0x33333333);
    return (((val + (val >> 4)) & 0x0F0F0F0F) * 0x01010101) >> 24;
#endif

}

/*
 * first_bit_set: Get the first bit set in the given value.
 * @val: the gieven value
 * Return the first bit set.
 */
static int first_bit_set(unsigned int val)
{
    int i;

    for (i = 0; i < (8 * sizeof(unsigned int)); i++) {
        if (val & (1 << i)) {
            return i;
        }
    }

    return -1;
#if 0
    /*
    * using arm support instruction CLZ to find leading set
    * CLZ user guide:
    * The result value is 32 if no bits are set in the source register, and zero if bit 31 is set.
    * http://infocenter.arm.com/help/index.jsp?topic=/com.arm.doc.dui0204h/Cihjgjed.html
    */
    int ret;
    volatile asm("clz\t%0, %1" : "=r" (ret) : "r" (val));
    ret = 31 -ret;
    return ret;
#endif

}

/*
 * __do_dqs_gw_calib: do DQS gating window calibration.
 * @cbs: pointer to the print_callbacks structure.
 * Return error code.
 */
static int __do_dqs_gw_calib(print_callbacks *cbs)
{
    int err;
    int i, c, f, cnt, max=0;
    int mid = 0;

    err = -1;

    dqs_gw_coarse = 0; //from begin of coarse tune, reset to 0
    dqs_gw_fine = 0;   //from begin of fine tune, reset to 0
    for (i = 0; i < DQS_GW_COARSE_MAX; i++) {
        dqs_gw[i] = 0;
    }

    #if DQS_GW_FINE_PER_BYTE    
    GW_PRINT("%s, dqs_gw_byte space:%d\n", __FUNCTION__, sizeof(dqs_gw_byte));
    
    memset((void *)dqs_gw_byte, 0, sizeof(dqs_gw_byte));
    #endif
    
    memset((void *)&cur_pwin, 0, sizeof(struct dqs_gw_pass_win));
    memset((void *)&max_pwin, 0, sizeof(struct dqs_gw_pass_win));

    /* 1.enable burst mode for gating window */
    /*   enable DQS gating window counter */
    DRAMC_WRITE_SET((1 << 28),DRAMC_DQSCTL1);
    DRAMC_WRITE_SET((1 << 8),DRAMC_SPCMD);
    
    #if DQS_GW_FINE_PER_BYTE    
    GW_PRINT("%s, calib...\n", __FUNCTION__);
    #endif

    if (ETT_TUNING_FACTOR_NUMS(dqs_gw_tuning_factors) > 0) {
        ett_recursive_factor_tuning(ETT_TUNING_FACTOR_NUMS(dqs_gw_tuning_factors)-1, dqs_gw_tuning_factors, cbs);
    }

    #if DQS_GW_FINE_PER_BYTE    
    GW_PRINT("%s, get coarse/fine value\n", __FUNCTION__);
    #endif
    
    for (i = 0; i < DQS_GW_COARSE_MAX; i++) {
            // find the max passed window
            cnt = nr_bit_set(dqs_gw[i]);
#if 0
            print("nr_bit_set(dqs_gw[%d]) = %d\n", i, cnt);
#endif
            if (cnt > (max+1)) {
                max = cnt;                
                c = i;
                mid = first_bit_set(dqs_gw[i]) + cnt / 2;
                GW_PRINT("i:%d, cnt:%d, mid:%d\n", i, cnt, mid);
            }
            else if ((cnt != 0) && ((cnt+1) >= max)) {
                int curMid = first_bit_set(dqs_gw[i]) + cnt / 2;
                int dist = (mid>7)?(mid-7):(7-mid);
                int curDist = (curMid>7)?(curMid-7):(7-curMid);                 

                GW_PRINT("i:%d, cnt:%d, curmid:%d\n", i, cnt, curMid);
                
                // Judge which is near with value 56(total fine tune window in every coarse value)
                if ((cnt+1) == max)
                {
                    if ((curDist+2)<=dist)
                    {
                        GW_PRINT("dist:%d, curDist:%d\n", dist, curDist);
                        max = cnt;
                        c = i;
                        mid = first_bit_set(dqs_gw[i]) + cnt / 2;
                    }
                    else
                    {
                        //do nothing
                    }
                }                    
                else if (curDist<dist)
                {   
                    GW_PRINT("dist:%d, curDist:%d\n", dist, curDist);
                    max = cnt;
                    c = i;
                    mid = first_bit_set(dqs_gw[i]) + cnt / 2;
                }
            }            
    }
    
    //if ((RANK_CURR == 1) && ((atoi(*opt_gw_coarse_value0) & 0x1c) != (atoi(dqsi_gw_dly_coarse_tbl[c]) & 0x1c))){    
    if ((RANK_CURR == 1) && ((atoi(*opt_gw_coarse_value0) & 0x1F) != (atoi(dqsi_gw_dly_coarse_tbl[c]) & 0x1F))){  
        //fix rank 0 coarse value not match rank 1 coarse value
        print("[EMI] Fix Rank 1 coarse value from %d to %d\n", atoi(*opt_gw_coarse_value0), atoi(dqsi_gw_dly_coarse_tbl[c]));
        c = atoi(*opt_gw_coarse_value0) - atoi(dqsi_gw_dly_coarse_tbl[0]);        
    }
    dqs_print("Rank %d coarse tune value selection : %d, %s\n", RANK_CURR, c, dqsi_gw_dly_coarse_tbl[c]);
      
    cnt = nr_bit_set(dqs_gw[c]);

	if(mt_get_dram_type() == TYPE_PCDDR3)
	{
		if(cnt > 10)
		{
			cnt = 10;
		}
	}

    if (cnt) {
        //print("first_bit_set(dqs_gw[c]):%d,f:%d\n",first_bit_set(dqs_gw[c]),cnt/2);
        f = first_bit_set(dqs_gw[c]) + cnt / 2;
#ifdef RELEASE
        dbg_print("%s\n", opt_gw_coarse_value = &dqsi_gw_dly_coarse_tbl[c]);
        dbg_print("%s\n", opt_gw_fine_value = &dqsi_gw_dly_fine_tbl[f]);
#else
        opt_gw_coarse_value = &dqsi_gw_dly_coarse_tbl[c];
        opt_gw_fine_value = &dqsi_gw_dly_fine_tbl[f];
#endif

        /* setup the opt coarse value and fine value according to calibration result*/
        print("%s\n", *opt_gw_coarse_value);
        print("%s\n", *opt_gw_fine_value);

        /*FIX GW setting*/
#if (defined(DQS_GW_COARSE_FIXED) && defined(DQS_GW_FINE_FIXED))
        //todo:
        opt_gw_coarse_value = DQS_GW_COARSE_FIXED;
        opt_gw_fine_value = DQS_GW_FINE_FIXED;
        
        print("FIX GW setting, Coarse:%s, Fine:%s\n", DQS_GW_COARSE_FIXED, DQS_GW_FINE_FIXED);
#endif

        dqsi_gw_dly_coarse_factor_handler(opt_gw_coarse_value, 1);        

        #if DQS_GW_FINE_PER_BYTE
        //todo: find opt fine value for every byte (condition@opt_gw_coarse_value)
        //
        for (i = 0; i < DQS_NUM; i++) {
            f = first_bit_set(dqs_gw_byte[atoi(*opt_gw_coarse_value)][i]) + nr_bit_set(dqs_gw_byte[atoi(*opt_gw_coarse_value)][i]) / 2;        
            opt_gw_fine_value_byte[i] = dqsi_gw_dly_fine_tbl[f];
            print("%s, BYTE%d, Fine: %s[%s, %s]\n", __FUNCTION__, i, 
                dqsi_gw_dly_fine_tbl[f], 
                dqsi_gw_dly_fine_tbl[first_bit_set(dqs_gw_byte[atoi(*opt_gw_coarse_value)][i])], 
                dqsi_gw_dly_fine_tbl[first_bit_set(dqs_gw_byte[atoi(*opt_gw_coarse_value)][i]) + nr_bit_set(dqs_gw_byte[atoi(*opt_gw_coarse_value)][i])]);         
        }

        opt_gw_fine_value = opt_gw_fine_value_byte;

        GW_PRINT("%s, %x, %x, %x, %x, %x\n", __FUNCTION__, opt_gw_fine_value, opt_gw_fine_value_byte[0], opt_gw_fine_value_byte[1], &opt_gw_fine_value_byte[0], &opt_gw_fine_value_byte[1]); 
        GW_PRINT("%s, %x, %x\n", __FUNCTION__, &opt_gw_fine_value_byte[0], opt_gw_fine_value_byte); 

        dqsi_gw_dly_fine_factor_handler(opt_gw_fine_value, 4);
        #else
        dqsi_gw_dly_fine_factor_handler(opt_gw_fine_value, 1);
        #endif

        #if DQS_STROBE_LIMIT
        {
        u32 u4Min = 0;
        u32 u4Max = 0;
        
        // Get min&max
        u4Min = atoi(dqsi_gw_dly_fine_tbl[first_bit_set(dqs_gw[c])]);
        u4Max = atoi(dqsi_gw_dly_fine_tbl[first_bit_set(dqs_gw[c]) + nr_bit_set(dqs_gw[c]) - 1]);

        print("Rank %d gating, DQS limit[%d, %d]\n", RANK_CURR, u4Min, u4Max);
        
        if (0 == RANK_CURR)
        {
            //0x100041C0
            DRAMC_WRITE_CLEAR(0xFFFF, DRAMC_DQSCAL0);
            DRAMC_WRITE_SET((u4Min&0x7F) | (0x1<<7) | ((u4Max&0x7F)<<8) | (0x1<<15), DRAMC_DQSCAL0);
        }
        else if (1 == RANK_CURR)
        {
            //0x100041C4
            DRAMC_WRITE_CLEAR(0xFFFF, DRAMC_DQSCAL1);
            DRAMC_WRITE_SET((u4Min&0x7F) | (0x1<<7) | ((u4Max&0x7F)<<8) | (0x1<<15), DRAMC_DQSCAL1);
        }
        else
        {
            print("Invalid rank %d, error !!!\n", RANK_CURR);
        }
        }
        #endif
        
        err = 0;
    }
    else {
        dqs_print("Cannot find any pass-window\n");
    }
    /* disable DQS gating window counter */
    DRAMC_WRITE_CLEAR((1 << 8),DRAMC_SPCMD);
__do_dqs_gw_calib_exit:
 
    return err;
}

/*
 * do_dqs_gw_calib_1: do DQS gating window calibration (phase 1).
 * Return error code.
 */
int do_dqs_gw_calib_1(void)
{
    print_callbacks cbs = {
        .ett_print_banner = ett_print_dqs_gw_banner,
        .ett_print_before_start_loop_zero = ett_print_dqs_gw_before_start_loop_zero,
        .ett_print_before_each_round_of_loop_zero = ett_print_dqs_gw_before_each_round_of_loop_zero,
        .ett_print_result = ett_print_dqs_gw_result_1, //difference between do_dqs_gw_calib_2
        .ett_print_after_each_round_of_loop_zero = ett_print_dqs_gw_after_each_round_of_loop_zero,
        .ett_calc_opt_value = ett_calc_dqs_gw_opt_value,
        .ett_print_after_finish_loop_n = ett_print_dqs_gw_fine_after_finish_loop_zero,
    };

    return __do_dqs_gw_calib(&cbs);
}

