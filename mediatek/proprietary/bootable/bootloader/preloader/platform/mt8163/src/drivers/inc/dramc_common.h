//
//  DRAMC_COMMON.H    
//

#ifndef _DRAMC_COMMON_H_
#define _DRAMC_COMMON_H_

#include "dramc_register.h"

/***********************************************************************/
/*                  Public Types                                       */
/***********************************************************************/
#if 1
#include <typedefs.h>
#else
typedef unsigned char   UCHAR;
typedef unsigned char   UINT8;
typedef unsigned short  UINT16;
typedef signed char     INT8;
typedef signed int    INT16;

typedef signed char            S8;
typedef signed short           S16;
typedef signed int            S32;
typedef unsigned char   U8;
typedef unsigned short  U16;
typedef unsigned int   U32;

#ifndef UINT32P
typedef volatile unsigned int* UINT32P; 
#endif //UINT32P
#endif

/*------------------------------------------------------------*/
/*                  macros, defines, typedefs, enums          */
/*------------------------------------------------------------*/
/************************ Bit Process *************************/
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

#define mcSET_FIELD0(var, value, mask, pos) mcSET_MASK(var, (((U32)value) << (pos)) & (mask))

#define mcSET_FIELD(var, value, mask, pos)  \
{                                           \
    mcCLR_MASK(var, mask);                  \
    mcSET_FIELD0(var, value, mask, pos);    \
}

#define mcGET_FIELD(var, mask, pos)     (((var) & (mask)) >> (pos))

#ifndef BIT0
#define BIT0                    mcBIT(0)
#define BIT1                    mcBIT(1)
#define BIT2                    mcBIT(2)
#define BIT3                    mcBIT(3)
#define BIT4                    mcBIT(4)
#define BIT5                    mcBIT(5)
#define BIT6                    mcBIT(6)
#define BIT7                    mcBIT(7)
#endif

#define mcLOW_MASK(u16)         ((UCHAR) ((u16) & mcMASK(8)))
#define mcHIGH_MASK(u16)        ((UCHAR) ((u16) & ~mcMASK(8)))
#define mcLOW2HIGH(u16)         (((UINT16) (u16)) << 8)
/* (1234, 5678) -> 7834 */
#define mc2BYTE(ch1L, ch2H)     (mcLOW_MASK(ch1L) | mcLOW2HIGH(ch2H))
/* (1234) -> 0034 */
//#define mcLOW_BYTE(u16)         ((UCHAR)(u16))
#define mcLOW_BYTE(u16)         mcLOW_MASK(u16)
/* (1234) -> 0012 */
#define mcHIGH_BYTE(u16)        ((UCHAR) (((UINT16) (u16)) >> 8))

/* xxxHHHHH + LLLLLxxx */
#define mcUNSIGN_2BYTE(u8h, u8l, hmsk, lmsk)                \
    (((mcLOW2HIGH((u8h) & mcMASK(hmsk)) | (u8l))            \
        << (8 - (hmsk))) >> ((8 - (hmsk)) + (8 - (lmsk))))

#define mcSIGN_2BYTE(u8h, u8l, hmsk, lmsk)                  \
    (((INT16) ((mcLOW2HIGH((u8h) & mcMASK(hmsk)) | (u8l))     \
        << (8 - (hmsk)))) >> ((8 - (hmsk)) + (8 - (lmsk))))

#define mcSET_MASKS(par)                mcMASKS(cw##par, cp##par)
#define mcFIELD_CMB(val, par)           mcFIELD(val, cm##par, cp##par)
#define mcGET_FIELD_CMB(var, par)       mcGET_FIELD(var, cm##par, cp##par)
#define mcSET_FIELD_CMB(var, val, par)  mcSET_FIELD(var, val, cm##par, cp##par)


#define ETT_TEST_CASE_NUMS(x)	(sizeof(x)/sizeof(test_case))

#define DDR_PHY_RESET() do { \
} while(0)
#define DDR_PHY_RESET_NEW() do { \
	 __asm__ __volatile__ ("dsb" : : : "memory"); \
	 DRAMC_WRITE_REG(DRAMC_READ_REG(0x04) | (0x1<<26), 0x04); \
	 while ( (DRAMC_READ_REG(0x3b8) & (0x01<<16))==0); \
    DRAMC_WRITE_REG((DRAMC_READ_REG(DRAMC_PHYCTL1)) \
		| (1 << 28), \
		DRAMC_PHYCTL1); \
    DRAMC_WRITE_REG((DRAMC_READ_REG(DRAMC_GDDR3CTL1)) \
		| (1 << 25),	\
		DRAMC_GDDR3CTL1); \
    delay_a_while(1000); \
    DRAMC_WRITE_REG((DRAMC_READ_REG(DRAMC_PHYCTL1)) \
		& (~(1 << 28)),	\
		DRAMC_PHYCTL1); \
    DRAMC_WRITE_REG((DRAMC_READ_REG(DRAMC_GDDR3CTL1)) \
		& (~(1 << 25)),	\
		DRAMC_GDDR3CTL1); \
	DRAMC_WRITE_REG(DRAMC_READ_REG(0x04) & ~(0x1<<26), 0x04); \
	while ( (DRAMC_READ_REG(0x3b8) & (0x01<<16))==1); \
	__asm__ __volatile__ ("dsb" : : : "memory"); \
} while(0)

/* define supported DRAM types */
enum
{
  TYPE_mDDR = 1,
  TYPE_LPDDR2,
  TYPE_LPDDR3,
  TYPE_PCDDR3,
};

#define CEIL_A_OVER_B(_A, _B)	(((_A)-(_B)*((_A)/(_B))) > 0? (_A)/(_B)+1:(_A)/(_B))

#define DRAMC_WRITE_REG(val,offset)  do{ \
                                      (*(volatile unsigned int *)(DRAMC0_BASE + (offset))) = (unsigned int)(val); \
                                      (*(volatile unsigned int *)(DDRPHY_BASE + (offset))) = (unsigned int)(val); \
                                      (*(volatile unsigned int *)(DRAMC_NAO_BASE + (offset))) = (unsigned int)(val); \
                                      }while(0)

#define DRAMC_WRITE_REG_W(val,offset)     do{ \
                                      (*(volatile unsigned int *)(DRAMC0_BASE + (offset))) = (unsigned int)(val); \
                                      (*(volatile unsigned int *)(DDRPHY_BASE + (offset))) = (unsigned int)(val); \
                                      (*(volatile unsigned int *)(DRAMC_NAO_BASE + (offset))) = (unsigned int)(val); \
                                      }while(0)

#define DRAMC_WRITE_REG_H(val,offset)     do{ \
                                      (*(volatile unsigned short *)(DRAMC0_BASE + (offset))) = (unsigned short)(val); \
                                      (*(volatile unsigned short *)(DDRPHY_BASE + (offset))) = (unsigned short)(val); \
                                      (*(volatile unsigned short *)(DDRPHY_BASE + (offset))) = (unsigned short)(val); \
                                      }while(0)
#define DRAMC_WRITE_REG_B(val,offset)     do{ \
                                      (*(volatile unsigned char *)(DRAMC0_BASE + (offset))) = (unsigned char)(val); \
                                      (*(volatile unsigned char *)(DDRPHY_BASE + (offset))) = (unsigned char)(val); \
                                      (*(volatile unsigned char *)(DDRPHY_BASE + (offset))) = (unsigned char)(val); \
                                      }while(0)
#define DRAMC_READ_REG(offset)         ( \
                                        (*(volatile unsigned int *)(DRAMC0_BASE + (offset))) |\
                                        (*(volatile unsigned int *)(DDRPHY_BASE + (offset))) |\
                                        (*(volatile unsigned int *)(DRAMC_NAO_BASE + (offset))) \
                                       )
#define DRAMC_WRITE_SET(val,offset)     do{ \
                                      (*(volatile unsigned int *)(DRAMC0_BASE + (offset))) |= (unsigned int)(val); \
                                      (*(volatile unsigned int *)(DDRPHY_BASE + (offset))) |= (unsigned int)(val); \
                                      (*(volatile unsigned int *)(DRAMC_NAO_BASE + (offset))) |= (unsigned int)(val); \
                                      }while(0)

#define DRAMC_WRITE_CLEAR(val,offset)     do{ \
                                      (*(volatile unsigned int *)(DRAMC0_BASE + (offset))) &= ~(unsigned int)(val); \
                                      (*(volatile unsigned int *)(DDRPHY_BASE + (offset))) &= ~(unsigned int)(val); \
                                      (*(volatile unsigned int *)(DRAMC_NAO_BASE + (offset))) &= ~(unsigned int)(val); \
                                      }while(0)

#define DDRPHY_WRITE_REG(val,offset)    __raw_writel(val, (DDRPHY_BASE + (offset)))
#define DRAMC0_WRITE_REG(val,offset)    __raw_writel(val, (DRAMC0_BASE + (offset)))
#define DRAMC_NAO_WRITE_REG(val,offset) __raw_writel(val, (DRAMC_NAO_BASE + (offset)))


typedef struct {
    char *name;
    char **factor_tbl;
    char *curr_val;
    char *opt_val;
    void (*factor_handler) (char **, char);
} tuning_factor;

typedef struct {
    void (*ett_print_banner) (unsigned int);
    void (*ett_print_before_start_loop_zero) (void);
    void (*ett_print_before_each_round_of_loop_zero) (void);
    unsigned int (*ett_print_result) (void);
    void (*ett_print_after_each_round_of_loop_zero) (void);
    void (*ett_calc_opt_value) (unsigned int, unsigned int *, unsigned int *);
    void (*ett_print_after_finish_loop_n) (int);
} print_callbacks;

#define ETT_TUNING_FACTOR_NUMS(x)	(sizeof(x)/sizeof(tuning_factor))

typedef struct {
    int (*test_case) (unsigned int, unsigned int, void *);
    unsigned int start;
    unsigned int range;
    void *ext_arg;
} test_case;



// Edward?? : The following needs to be porting.
/************************** Common Macro *********************/
#define delay_a_while(count) \
    do {                                   \
           register unsigned int delay;        \
           asm volatile ("mov %0, %1\n\t"      \
                         "1:\n\t"              \
                         "subs %0, %0, #1\n\t" \
                         "bne 1b\n\t"          \
                         : "+r" (delay)        \
                         : "r" (count)         \
                         : "cc");              \
        } while (0)

#define dsb() __asm__ __volatile__ ("dsb" : : : "memory")

// K2?? : The following needs to be porting.

#if 0
//#define mcDELAY_US(x)           delay_a_while((U32) (x*200*10))
//#define mcDELAY_MS(x)           delay_a_while((U32) (x*200*1000))
#define mcDELAY_US(x)           delay_a_while((U32) (x*10))
#define mcDELAY_MS(x)           delay_a_while((U32) (x*10*1000))
#else
#define mcDELAY_US(x)           gpt_busy_wait_us(x)
//#define mcDELAY_US(x)           gpt_busy_wait_us(10*x)
#define mcDELAY_MS(x)           gpt_busy_wait_us(x*1000)
#endif

#ifdef DDR_INIT_TIME_PROFILING    
#define mcSHOW_HW_MSG(_x_)     
#define mcSHOW_DBG_MSG(_x_) 
#define mcSHOW_DBG_MSG2(_x_)   
#define mcSHOW_DBG_MSG3(_x_) 
#define mcSHOW_DBG_MSG4(_x_) 
#define mcSHOW_USER_MSG(_x_)    
#define mcSHOW_DRVAPI_MSG(_x_) 
#define mcSHOW_ERR_MSG(_x_)  
#define mcFPRINTF(_x_)          
#else
    #if defined(TARGET_BUILD_VARIANT_ENG)
    //U8 RXPERBIT_LOG_PRINT = 1;
    #define mcSHOW_HW_MSG(_x_)     
    #define mcSHOW_DBG_MSG(_x_)     //print _x_    
    #define mcSHOW_DBG_MSG2(_x_)    //print _x_     
    #define mcSHOW_DBG_MSG3(_x_)    //print _x_ //if (RXPERBIT_LOG_PRINT) {print _x_;}
    #define mcSHOW_DBG_MSG4(_x_)    //print _x_ //if (RXPERBIT_LOG_PRINT) {print _x_;}
    #define mcSHOW_USER_MSG(_x_)    
    #define mcSHOW_DRVAPI_MSG(_x_)  print _x_
    #define mcSHOW_ERR_MSG(_x_)     print _x_
    #define mcFPRINTF(_x_)          
    #else
    #define mcSHOW_HW_MSG(_x_)     
    #define mcSHOW_DBG_MSG(_x_) 
    #define mcSHOW_DBG_MSG2(_x_)   
    #define mcSHOW_DBG_MSG3(_x_) 
    #define mcSHOW_DBG_MSG4(_x_) 
    #define mcSHOW_USER_MSG(_x_)    
    #define mcSHOW_DRVAPI_MSG(_x_) 
    #define mcSHOW_ERR_MSG(_x_)  
    #define mcFPRINTF(_x_)         
    #endif     
#endif

//#define DLE_RETRY_AFTER_RX_TX

#endif   // _DRAMC_COMMON_H_
