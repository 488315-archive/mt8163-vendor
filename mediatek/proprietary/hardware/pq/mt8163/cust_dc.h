#ifndef __CUST_DC_H__
#define __CUST_DC_H__

#include "ddp_drv.h"


#define DC_FLAG_NORMAL           (0x00000000)
#define DC_FLAG_TUNING           (0x00000001)
#define DC_FLAG_DC_TUNING        (0x00000002)
#define DC_FLAG_NCS_SHP_TUNING   (0x00000004)

// for debug
#define PQ_DBG_DC_TUNING_DEFAULT                   "0" // 0: disable, 1: enable, 2: default
#define PQ_DBG_DC_TUNING_STR                       "debug.pq.dc.tuning"

extern const DISPLAY_DC_T dcindex;

#endif

