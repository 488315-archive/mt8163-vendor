#ifndef __MFLLLOG_H__
#define __MFLLLOG_H__

#ifdef MTK_LOG_ENABLE
#undef MTK_LOG_ENABLE
#endif
#define MTK_LOG_ENABLE 1
#include "MfllDefs.h"

#include <cutils/log.h> // XLOG
#include <utils/CallStack.h>

#ifndef MFLL_LOG_KEYWORD
    #define MFLL_LOG_KEYWORD "{Mfll}" // Original name!
#endif

#ifdef MFLL_LOG_STDOUT
    #include <stdio.h>
    #define __LOGV printf
    #define __LOGD printf
    #define __LOGI printf
    #define __LOGW printf
    #define __LOGE printf
#else
    #define __LOGV ALOGV
    #define __LOGD ALOGD
    #define __LOGI ALOGI
    #define __LOGW ALOGW
    #define __LOGE ALOGE
#endif


#if MFLL_LOG
    #define mfllLogV(fmt, arg...)   do { __LOGV (MFLL_LOG_KEYWORD fmt "\n", ##arg);} while(0)
    #define mfllLogD(fmt, arg...)   do { __LOGD (MFLL_LOG_KEYWORD fmt "\n", ##arg);} while(0)
    #define mfllLogI(fmt, arg...)   do { __LOGI (MFLL_LOG_KEYWORD fmt "\n", ##arg);} while(0)
    #define mfllLogW(fmt, arg...)   do { __LOGW (MFLL_LOG_KEYWORD "warning:" fmt "\n", ##arg);} while(0)
    #define mfllLogE(fmt, arg...)   do { __LOGE (MFLL_LOG_KEYWORD "@@@ error @@@:" fmt "\n", ##arg); } while(0)
    #define mfllFunctionIn()        do { mfllLogD("[+] %s", __FUNCTION__);} while(0)
    #define mfllFunctionInInt(x)    do { mfllLogD("[+] %s:%d", __FUNCTION__, x);} while(0)
    #define mfllFunctionOut()       do { mfllLogD("[-] %s", __FUNCTION__);} while(0)
    #define mfllDumpStack(x)        do { android::CallStack s; s.update(); s.log(MFLL_LOG_KEYWORD);} while(0)
#else
    #define mfllLogV(fmt, arg...)
    #define mfllLogD(fmt, arg...)
    #define mfllLogI(fmt, arg...)
    #define mfllLogW(fmt, arg...)
    #define mfllLogE(fmt, arg...)
    #define mfllFunctionIn()
    #define mfllFunctionInInt(x)
    #define mfllFunctionOut()
    #define mfllDumpStack(x)
#endif

#define mfllAutoLog(x)              MfllAutoLog _____l(x)
#define mfllAutoLogFunc()           MfllAutoLog _____l(__FUNCTION__)
#define mfllAutoTrace(x)
#define mfllAutoTraceFunc()

class MfllAutoLog{
public:
    MfllAutoLog(const char *t)
    {
        mfllLogD("%s +", t);
        this->txt = t;
    }
    virtual ~MfllAutoLog(void)
    {
        mfllLogD("%s -", this->txt);
    }
private:
    const char *txt;
};


#endif /* __MFLLLOG_H__ */

