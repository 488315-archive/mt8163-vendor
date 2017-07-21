#include <stdlib.h>
#include <typedefs.h>
#include <platform.h>
#include <dramc_common.h>
#include <emi_hw.h>



// Using some good value for pass DLE calibration firstly
//#define DLE_USING_RX_TX_FIX_VALUE
//#define DLE_FIXED  "14"

#define DLE_CAL_RETRY_WL

#define DLE_MAX 16

extern int RANK_CURR;
char *opt_dle_value = "4";
int dle_result[DLE_MAX];
static int global_dle_value;

#define TEST_PASS (0)
#define DRAM_START (0x40000000)
#define RANK_SIZE (0x20000000)
//#define NEW_RX_FORMAT
int dramc_dma_test(unsigned int start, unsigned int len, void *ext_arg){
    int err =  TEST_PASS;
    int check_result = (int)ext_arg;
    unsigned int data;
    int i;

    unsigned int *src_buffp1;
    unsigned int *dst_buffp1;;

    src_buffp1 = DRAM_START;
    dst_buffp1 = DRAM_START+0x00001000;
    
    for (i = 0 ; i < (len/sizeof(unsigned int)) ; i++) {
        *((unsigned int *)dst_buffp1+i) = 0;
    }
    for (i = 0 ; i < (len/sizeof(unsigned int)) ; i++) {
        *((unsigned int *)src_buffp1+i) = i;
    }
    //memset(dst_buffp1, 0, len);

     *((volatile unsigned int *)(0x11000098)) = 0x00070000; //BURST_LEN:7-8,R/W
     *((volatile unsigned int *)(0x1100009C)) = src_buffp1;
     *((volatile unsigned int *)(0x110000A0)) = dst_buffp1;
     *((volatile unsigned int *)(0x110000A4)) = len;
     *((volatile unsigned int *)(0x11000088)) = 0x1; //start dma

    while(*((volatile unsigned int *)(0x11000088)));

    for (i = 0 ; i < (len/sizeof(unsigned int)) ; i++) {
        if (*((unsigned int *)dst_buffp1+i) != i) {
     //       print("0x%p: 0x%x != 0x%x\n", (unsigned int *)dst_buffp1+i, *((unsigned int *)dst_buffp1+i), i);
            err = -1;
            break;
        }
    }

    if ((int)ext_arg == 0xFFFFFFFF)
    {
        return err;
    }
    return err;

}

/*in MT6589 test agent 1 only used in DLE calibration*/
int dramc_ta1(unsigned int start, unsigned int len, void *ext_arg){
    int err =  TEST_PASS;
    int check_result = (int)ext_arg;
    unsigned int data;
    /* set test patern length*/
    data = DRAMC_READ_REG(0x40);
    DRAMC_WRITE_REG((data & 0xFF000000) | len, 0x40);

    //dbg_print("dramc_ta1 enable\n");
    DRAMC_WRITE_SET((1 << 29) ,DRAMC_CONF2); //Test Agent1

    //dbg_print("0x3C:%x\n",DRAMC_READ_REG(0x3c));
    //dbg_print("0x40:%x\n",DRAMC_READ_REG(0x40));
    //dbg_print("DRAMC_CONF2:%x\n",DRAMC_READ_REG(DRAMC_CONF2));
    while(!(DRAMC_READ_REG(DRAMC_TESTRPT)&(1 << 10)));

    /*
     * NoteXXX: Need to wait for at least 400 ns
     *          After checking the simulation result,
     *          there will be a delay on DLE_CNT_OK/DM_CMP_ERR updates after getting DM_CMP_CPT.
     *          i.e; After getting the complete status, need to wait for a while before reading DLE_CNT_OK/DM_CMP_ERR in the TESTRPT register.
     */
    delay_a_while(400);
    //dbg_print("dramc_ta1 done\n");
    
    //dbg_print("DRAMC_TESTRPT:%x\n",DRAMC_READ_REG(DRAMC_TESTRPT));
    if (check_result) {
        if (DRAMC_READ_REG(DRAMC_TESTRPT) & (1 << 14)) {
            err = -1;
        }
        /*} else if (!(DRAMC_READ_REG(DRAMC_TESTRPT) & (1 << 18))) {
            err = -1;
        }*/
    }

    DRAMC_WRITE_CLEAR((1 << 29) ,DRAMC_CONF2); //disable test agent1


    //dbg_print("ext_arg:%x,err:%x\n",ext_arg,err);
    if ((int)ext_arg == 0xFFFFFFFF)
    {
        return err;
    }

    return;

}
char *dram_driving_tbl[] =
{
	/* DRAM I/O Driving */
	//"1",  /* 34.3	ohm:	0001 */
	//"2",  /* 40	ohm:	0010 */
	//"3",  /* 48	ohm:	0011 */
	"4",  /* 60	ohm:	0100 */
	//"5",  /* 68.6	ohm:	0101 */
	//"6",  /* 80	ohm:	0110 */
	//"7",  /* 120	ohm:	0111 */
	NULL,
};

void dram_driving_factor_handler(char **factor_value, char mode) {
    int curr_val = atoi(*factor_value);

}



char *dle_tbl[] =
{
	/* DLE: 0x0~0xf */
	//"0", "1", "2", "3",
	"4", "5", "6", "7", "8", "9", "10", "11",
	"12", "13", "14", "15", 
	NULL,
};

void dle_factor_handler(char **factor_value, char mode) {
    int curr_val = 0;
    
    //dbg_print("dle_factor_handler, dle_tbl:0x%x, %s\n", dle_tbl, dle_tbl[0]); 

    if (NULL == factor_value)
    {
        dbg_print("dle_factor_handler, factor_value:0x%x null\n", factor_value); 
        return;
    }

    if (4 > atoi(*factor_value))
        dbg_print("dle_factor_handler, factor_value:0x%x value:%d, too little\n", factor_value, atoi(*factor_value)); 

    curr_val = atoi(*factor_value);
    //dbg_print("dle_factor_handler, curr_val:0x%x\n", curr_val);

    #if 1 //MT8163
    DRAMC_WRITE_REG((DRAMC_READ_REG(DRAMC_DATLATCTL) & 0xFFE0E0FF)
		| ((curr_val & 0x1F) << 16)
		| (((curr_val & 0x1F) - 0x4) << 8),	//Notice: Need ensure curr_val( from dle_tbl) >= 4 !!!		
		DRAMC_DATLATCTL);

    //dbg_print("dle_factor_handler, factor_value:0x%x\n", factor_value); 
    //dbg_print("dle_factor_handler, dle_tbl:0x%x\n", dle_tbl); 
    //dbg_print("dle_factor_handler, *factor_value:%s\n", *factor_value); 

    //dbg_print("dle_factor_handler, DRAMC_DATLATCTL:0x%x, curr_val:%d\n", DRAMC_READ_REG(DRAMC_DATLATCTL), curr_val);    
    #else
    DRAMC_WRITE_REG((DRAMC_READ_REG(DRAMC_DDR2CTL/* 0x7C */) & 0xFFFFFF8F)	/* Reserve original values for DRAMC_DDR2CTL[0:3, 7:31] */
		| ((curr_val & 0x7) << 4),			/* DATLAT: DRAMC_DDR2CTL[4:6], 3 bits */
		DRAMC_DDR2CTL/* 0x7C */);

    DRAMC_WRITE_REG((DRAMC_READ_REG(DRAMC_PADCTL4/* 0xE4 */) & 0xFFFFFFEF)	/* Reserve original values for DRAMC_DDR2CTL[0:3, 5:31] */
		| (((curr_val >> 3) & 0x1) << 4),			/* DATLAT3: DRAMC_PADCTL1[4], 1 bit */
		DRAMC_PADCTL4/* 0xE4 */);
    #endif
}

void driving_factor_handler(char **factor_value, char mode);

#if defined(NEW_RX_FORMAT) /*MT6583*/
tuning_factor rx_tuning_factors[] = {
    /* {"<-DQ->|<-DQS-> Input Delay",		dqi_dqs_dly_tbl,		NULL, NULL, dqi_dqsi_dly_factor_handler},
    {"Tx I/O Driving (DRVP, DRVN)",     driving_tbl_for_rx, NULL, NULL, driving_factor_handler},
    */
    {"DRAM Driving Strength",			dram_driving_tbl,	NULL, NULL, dram_driving_factor_handler},
    {"DLE",					dle_tbl,		NULL, NULL, dle_factor_handler},
};
#else
tuning_factor rx_tuning_factors[] = {
   /*   {"DQ Input Delay",				dqi_dly_tbl,		NULL, NULL, dqi_dly_factor_handler},
    {"DQS Input Delay",				dqsi_dly_tbl,		NULL, NULL, dqsi_dly_factor_handler},
    {"DRAM Driving Strength",			dram_driving_tbl,	NULL, NULL, dram_driving_factor_handler},*/
    {"DLE",					dle_tbl,		NULL, NULL, dle_factor_handler},
 /*   {"Tx I/O Driving (DRVP, DRVN)",		driving_tbl_for_rx,	NULL, NULL, driving_factor_handler}, */
};
#endif


void dqso_dly_factor_handler(char **factor_value, char mode) 
{
    int curr_val = atoi(*factor_value);
    curr_val = curr_val;

    DRAMC_WRITE_REG((DRAMC_READ_REG(DRAMC_PADCTL3/* 0x14 */) & 0xFFFF0000)	/* Reserve original values for __DRAMC_PADCTL3[16:31] */
		| ((curr_val & 0xF) << 0)					/* DQS0DLY: __DRAMC_PADCTL3[0:3],	4 bits */
		| ((curr_val & 0xF) << 4)					/* DQS1DLY: __DRAMC_PADCTL3[4:7],	4 bits */
		| ((curr_val & 0xF) << 8)					/* DQS2DLY: __DRAMC_PADCTL3[8:11],	4 bits */
		| ((curr_val & 0xF) << 12),				/* DQS3DLY: __DRAMC_PADCTL3[12:15],	4 bits */
		DRAMC_PADCTL3/* 0x14 */);
}

char *driving_tbl[] =
{
	/* Tx I/O Driving */
#if LPDDR == 2
        "8", "9", "10", "11", "12", 
#elif LPDDR == 3
        "6", "7","8", "9", "10",
        //"4", "5", "6", "7", "8", "9", "10", "11", "12", "15",
#else
        "6", "7", "8", "9", "10",

#endif
	NULL,
};





void ett_print_dram_driving(char *name, int curr_val) {
}


#if defined(NEW_RX_FORMAT) /*MT6583*/
tuning_factor dle_tuning_factors[] = 
{
    { .name = "DRAM Driving Strength",
      .factor_tbl = dram_driving_tbl,
      .curr_val = NULL, 
      .opt_val = NULL, 
      .factor_handler = dram_driving_factor_handler,
    },
    {
     .name = "DLE",
     .factor_tbl = dle_tbl,
      .curr_val = NULL, 
      .opt_val = NULL, 
      .factor_handler = dle_factor_handler,
    },
};
#else
tuning_factor dle_tuning_factors[] = 
{
    {
     .name = "DLE",
     .factor_tbl = dle_tbl,
      .curr_val = NULL, 
      .opt_val = NULL, 
      .factor_handler = dle_factor_handler,
    },
};
#endif

int dramc_ta2_for_dle(unsigned int start, unsigned int len, void *ext_arg)
{
    int ret =  TEST_PASS;
    unsigned int data;
    /* set test patern length*/
    DRAMC_WRITE_REG(0x55000000,0x3C);
    data = DRAMC_READ_REG(0x40);
	dbg_print("dramc_ta2_for_dle 0x40:0x%x\n", data);
    DRAMC_WRITE_REG((data & 0xAA000000) | len, 0x40);
    dbg_print("dramc_ta2_for_dle enable\n");
    //Test Agent 2 write enabling, Test Agent 2 read enabling
    DRAMC_WRITE_SET((1 << 30) | (1 << 31),DRAMC_CONF2); 
    
    while(!(DRAMC_READ_REG(DRAMC_TESTRPT)&(1 << 10)));

    delay_a_while(400);

    data = DRAMC_READ_REG(DRAMC_CMP_ERR);
    dbg_print("dramc_ta2_for_dle DRAMC_CMP_ERR:0x%x\n", data);
    dbg_print("dramc_ta2_for_dle done\n");
    DRAMC_WRITE_CLEAR(((1 << 30) | (1 << 31)),DRAMC_CONF2); //disable test agent2 r/w
    if (data != 0x0)
    {
        ret = -1;
    }

    return ret;
}


int dramc_cpu_rw_test(unsigned int start, unsigned int len, void *ext_arg)
{
    int ret =  TEST_PASS;
    unsigned int i, j;
    unsigned int cur_pattern;
    unsigned int all_pattern[] = {0x00000000, 0xffffffff, 0x12345678, 0x87654321};
    volatile unsigned int *p;

    for(i=0; i<sizeof(all_pattern)/sizeof(all_pattern[0]); i++)
    {
           cur_pattern = all_pattern[i];
           
           p = (volatile unsigned int *)start;
           for(j=0; j<len; j++,p++)
           {
                *p = cur_pattern;
           }

           p = (volatile unsigned int *)start;
           for(j=0; j<len; j++,p++)
           {
                if(*p != cur_pattern)
                {
                   ret = -1;
                   break;
                }
           }

           if(ret != TEST_PASS)
                break;
        
    }

    return ret;
}

test_case dle_test_cases[] = 
{
    //{dramc_ta1, 0x0, 0x7FF, 0xFFFFFFFF}
    //{dramc_dma_test, 0x0, 0x80, 0xFFFFFFFF} 
    //{dramc_ta2_for_dle, 0x0, 0x100, 0xFFFFFFFF}
    {dramc_cpu_rw_test, DRAM_START, 0x800, 0xFFFFFFFF}

};


void ett_print_dle_banner(unsigned int n) 
{
    unsigned int i;

    if (n == 1)
    {
#ifdef RELEASE

        dbg_print("=============================================\n");
        dbg_print("(");
        for ( i = 2 ; i < ETT_TUNING_FACTOR_NUMS(dle_tuning_factors) ; i++)
        {
            dbg_print("%d ", atoi(dle_tuning_factors[i].curr_val));
        }
        dbg_print(")\n");
        dbg_print("=============================================\n");
#else

        dbg_print("=============================================\n");
        for ( i = 2 ; i < ETT_TUNING_FACTOR_NUMS(dle_tuning_factors) ; i++)
        {
            if (dle_tuning_factors[i].factor_tbl == dram_driving_tbl)
            {
                ett_print_dram_driving(dle_tuning_factors[i].name, atoi(dle_tuning_factors[i].curr_val));
            }
            else 
            {
                dbg_print("%s = %d\n", dle_tuning_factors[i].name, atoi(dle_tuning_factors[i].curr_val));
            }
        }

        dbg_print("X-axis: %s\n", dle_tuning_factors[0].name);
        dbg_print("Y-axis: %s\n", dle_tuning_factors[1].name);
        dbg_print("=============================================\n");
#ifdef NEW_RX_FORMAT
        dbg_print("    F  C   8   4   0   4   8   C   10  14  18  1C  20  24  28  2C  30  34  38  3C  40  44  48  4C  50  54  58  5C  60  64  68  6C  70  74  78  7C 7F\n");
        dbg_print("    <--*---*---*-->|<--*---*---*---*---*---*---*---*---*---*---*---*---*---*---*---*---*---*---*---*---*---*---*---*---*---*---*---*---*---*---*-->\n");
//        dbg_print("        F    E    D    C    B    A    9    8    7    6    5    4    3    2    1    0    4    8    C   10   14   18   1C   20   24   28   2C   30   34   38   3C   40   44   48   4C   50   54   58   5C   60   64   68   6C   70   74   78   7C\n");
//        dbg_print("    -------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n");
#else
        dbg_print("        0    1    2    3    4    5    6    7    8    9    A    B    C    D    E    F\n");
        dbg_print("    --------------------------------------------------------------------------------\n");
#endif
#endif
    }
}

unsigned int ett_print_dle_result() 
{
    unsigned int i, score = 1;
    int result;

    for ( i = 0 ; i < ETT_TEST_CASE_NUMS(dle_test_cases) ; i++) {
        /* fixup the test start_address */
        dle_test_cases[i].start = DRAM_START; 
        if (dle_test_cases[i].test_case(dle_test_cases[i].start, dle_test_cases[i].range, dle_test_cases[i].ext_arg) == TEST_PASS) {
            if (i == (ETT_TEST_CASE_NUMS(dle_test_cases)-1)) {
#ifdef NEW_RX_FORMAT
                dbg_print("1");
#else
                dbg_print("    1");
#endif
            }
        dle_result[global_dle_value++] = score; 
        } else {
#ifdef NEW_RX_FORMAT
            dbg_print("0");
#else
            dbg_print("    0");
#endif
            score = 0;
            dle_result[global_dle_value++] = score; 
            break;
        }

    }

    return score;
}

void ett_print_dle_before_each_round_of_loop_zero() 
{
    DDR_PHY_RESET();
    //opt_dle_value = 0;
}

void ett_print_dle_after_each_round_of_loop_zero() 
{
    delay_a_while(200);
}

void ett_print_dle_before_start_loop_zero() 
{
#if !defined(RELEASE)
    dbg_print("%B:|", atoi(dle_tuning_factors[1].curr_val));
#endif
#if defined(NEW_RX_FORMAT)
    //dle_factor_handler("0");
#endif
}

void ett_print_dle_after_finish_loop_n(unsigned int n) 
{
    if (n == 0) {
        dbg_print("\n");
    }
}

void ett_calc_dle_opt_value(unsigned int n, unsigned int *score, unsigned int *high_score) 
{
    
}

 /*
     *     Related Registers (Latency between DRAMC and PHY)
     *             - PADCTL4  bit4        DATLAT3
     *             - DDR2CTL  bit 6~4  DATLAT
     *     Algorithm
     *             -  Set DLE from 4 to 14 and check if data is correct.>
     */
#ifdef DLE_RETRY_AFTER_RX_TX
unsigned int iDLECalibCnt = 0;
#endif
int do_dle_calib(void) {
    int ix;
    int result = -1;
#ifdef DLE_CAL_RETRY_WL
    int retrycnt = 0;
#endif
    
    global_dle_value = 0;
    print_callbacks cbs = {
        .ett_print_banner = ett_print_dle_banner,
        .ett_print_before_start_loop_zero = ett_print_dle_before_start_loop_zero,
        .ett_print_before_each_round_of_loop_zero = ett_print_dle_before_each_round_of_loop_zero,
        .ett_print_result = ett_print_dle_result,
        .ett_print_after_each_round_of_loop_zero = ett_print_dle_after_each_round_of_loop_zero,
        .ett_calc_opt_value = ett_calc_dle_opt_value,
        .ett_print_after_finish_loop_n = ett_print_dle_after_finish_loop_n,
    };

    #if 1
    dbg_print("%s, 0x210: %x\n", __FUNCTION__, DRAMC_READ_REG(0x0210));
    dbg_print("%s, 0x214: %x\n", __FUNCTION__, DRAMC_READ_REG(0x0214));
    dbg_print("%s, 0x218: %x\n", __FUNCTION__, DRAMC_READ_REG(0x0218));
    dbg_print("%s, 0x21c: %x\n", __FUNCTION__, DRAMC_READ_REG(0x021c));
    dbg_print("%s, 0x220: %x\n", __FUNCTION__, DRAMC_READ_REG(0x0220));
    dbg_print("%s, 0x224: %x\n", __FUNCTION__, DRAMC_READ_REG(0x0224));
    dbg_print("%s, 0x228: %x\n", __FUNCTION__, DRAMC_READ_REG(0x0228));
    dbg_print("%s, 0x22c: %x\n", __FUNCTION__, DRAMC_READ_REG(0x022c));
    #endif
    
/*
   * Main function
   *     - Create dle_result[]
   * Output
   *     -   |    0    0    1    1    1    1    0    0    0    0    0 >
   */
#ifdef DLE_FIXED
    #ifdef DLE_RETRY_AFTER_RX_TX
    iDLECalibCnt++;
    
    if (iDLECalibCnt <= 1)
    #endif
    {
        opt_dle_value = DLE_FIXED;
        dle_factor_handler(&opt_dle_value, 1);
        print("Fix DLE : %s\n", opt_dle_value);
        return;
    }
#endif

#ifdef DLE_USING_RX_TX_FIX_VALUE
    print("*DLE_USING_RX_TX_FIX_VALUE\n");

    // RX value setting
    DRAMC_WRITE_REG(0x16171516, DRAMC_R0DELDLY); 
    DRAMC_WRITE_REG(0xE0C0D0E, DRAMC_DQIDLY1);
    DRAMC_WRITE_REG(0x8060908, DRAMC_DQIDLY2);
    DRAMC_WRITE_REG(0x6070807, DRAMC_DQIDLY3); 
    DRAMC_WRITE_REG(0x8070807, DRAMC_DQIDLY4);
    DRAMC_WRITE_REG(0x909090A, DRAMC_DQIDLY5);
    DRAMC_WRITE_REG(0xA090C08, DRAMC_DQIDLY6); 
    DRAMC_WRITE_REG(0xC0D0E0F, DRAMC_DQIDLY7);
    DRAMC_WRITE_REG(0x9080B08, DRAMC_DQIDLY8);

    // TX value setting
    DRAMC_WRITE_REG(0x6645, DRAMC_PADCTL2);    
    DRAMC_WRITE_REG(0x55555555, DRAMC_DQODLY1);
    DRAMC_WRITE_REG(0x44444444, DRAMC_DQODLY2);
    DRAMC_WRITE_REG(0x66666666, DRAMC_DQODLY3);
    DRAMC_WRITE_REG(0x66666666, DRAMC_DQODLY4);
    DRAMC_WRITE_REG(0x6666, DRAMC_PADCTL3);
#endif

#ifdef DLE_CAL_RETRY_WL
RETRY:
    if (0 < retrycnt)
    {        
        // WL: 0x1000f07c[30:28]
        
        int wl = (DRAMC_READ_REG(DRAMC_DDR2CTL)>>28) & 0x7;
        wl--;
        
        print("Retry DLE calibration, current WL:%d\n", wl);

        wl <<= 28;

        DRAMC_WRITE_REG((DRAMC_READ_REG(DRAMC_DDR2CTL) & 0x8FFFFFFF) | wl, DRAMC_DDR2CTL);   

		//Reset info
		global_dle_value = 0;
		for(ix = 0; ix < DLE_MAX; ix++){
        	dle_result[ix] = 0;
        }	
    }
#endif

    
    print("opt_dle value:%x, %s\n", dle_tuning_factors[0].factor_tbl[0], dle_tuning_factors[0].factor_tbl[0]);

    if (ETT_TUNING_FACTOR_NUMS(dle_tuning_factors) > 0) {
        ett_recursive_factor_tuning(ETT_TUNING_FACTOR_NUMS(dle_tuning_factors)-1, dle_tuning_factors, &cbs);
    }
#if 1
    for(ix = 0; (ix < DLE_MAX) && (dle_tbl[ix+1] !=NULL); ix++){
        if (dle_result[ix] == 1 && dle_result[ix+1] == 1){
            result = 0;      	
            if (atoi(dle_tbl[ix+1]) > atoi(opt_dle_value))
                //opt_dle_value = dle_tbl[ix];
				opt_dle_value = dle_tbl[ix+1];
            print("opt_dle value:%s\n",opt_dle_value);

            break;
        }
    }

	dbg_print("%s, opt dle:%s, result:0x%x\n", __FUNCTION__, opt_dle_value, result);

    /* setup the opt dle value according to calibration result*/
    if (*opt_dle_value!=0x30)
    { //0x30 == "0"
        dle_factor_handler(&opt_dle_value, 1);
        //return 0;
    }
    else
    {
        dbg_print("cannot find opt_dle value\n");
        #ifdef DLE_CAL_RETRY_WL
        if ((0 != result) && (0 == retrycnt))
        {
            retrycnt++;
            goto RETRY;
        }
        #endif
        //return -1;
    }    
    
    //result = dramc_cpu_rw_test(0x0, 0x800, 0xFFFFFFFF);

    //dbg_print("%s, cpu rw test result:%x\n", __FUNCTION__, result);

    return result;
#endif
}

