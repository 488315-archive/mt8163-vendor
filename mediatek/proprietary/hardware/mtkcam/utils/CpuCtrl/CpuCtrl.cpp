#define LOG_TAG "MtkCam/CpuCtrl"
//
#include <dlfcn.h>
#include <semaphore.h>
#include <sys/prctl.h>
#include <sys/resource.h>
#include <fcntl.h>
#include <sys/mman.h>
//
#if (!MTK_BASIC_PACKAGE)
#include <PerfServiceNative.h>
#endif
//
#include <utils/Errors.h>
#include <utils/threads.h>
#include <utils/List.h>
//
#include <cutils/atomic.h>
#include <cutils/log.h>
#include <cutils/log.h>
#include <cutils/properties.h>
//
using namespace android;
//
#include <Log.h>
#include <common.h>
#include <CpuCtrl.h>
//-----------------------------------------------------------------------------
#define LOG_MSG(fmt, arg...)        CAM_LOGD("[%s] " fmt, __FUNCTION__, ##arg)
#define LOG_WRN(fmt, arg...)        CAM_LOGW("[%s] " fmt, __FUNCTION__, ##arg)
#define LOG_ERR(fmt, arg...)        CAM_LOGE("[%s] " fmt, __FUNCTION__, ##arg)
//
#define LOG_MSG_IF(cond, arg...)    if (cond) { LOG_MSG(arg); }
#define LOG_WRN_IF(cond, arg...)    if (cond) { LOG_WRN(arg); }
#define LOG_ERR_IF(cond, arg...)    if (cond) { LOG_ERR(arg); }
//
#define FUNCTION_NAME               LOG_MSG("")
#define FUNCTION_IN                 LOG_MSG("+")
#define FUNCTION_OUT                LOG_MSG("-")
//-----------------------------------------------------------------------------
#define PERF_LIB_FULLNAME       "/system/lib/libperfservicenative.so"
//
#define STR_FUNC_REG            "PerfServiceNative_userRegBigLittle"
typedef int (*funcPerfRegBL)(int, int, int, int);
//
#define STR_FUNC_UNREG          "PerfServiceNative_userUnreg"
typedef int (*funcPerfUnreg)(int);
//
#define STR_FUNC_ENABLETIMEOUT  "PerfServiceNative_userEnableTimeout"
typedef void (*funcEnableTimeout)(int, int);
//
#define STR_FUNC_ENABLE         "PerfServiceNative_userEnable"
typedef void (*funcEnable)(int);
//
#define STR_FUNC_DISABLE        "PerfServiceNative_userDisable"
typedef void (*funcDisable)(int);
//
#define STR_FUNC_REGSCN         "PerfServiceNative_userRegScn"
typedef int (*funcRegScn)(void);
//
#define STR_FUNC_REGSCNCONFIG   "PerfServiceNative_userRegScnConfig"
typedef void (*funcRegScnConfig)(int, int, int, int, int, int);
//
#define STR_FUNC_GETCAPABILITY  "PerfServiceNative_userGetCapability"
typedef int (*funcGetCapability)(int);

//-----------------------------------------------------------------------------
class CpuCtrlImp : public CpuCtrl
{
    public:
        CpuCtrlImp();
        ~CpuCtrlImp();
        //
        virtual void    destroyInstance(void);
        virtual MBOOL   init(void);
        virtual MBOOL   uninit(void);
        virtual MBOOL   enable(CPU_CTRL_INFO &cpuInfo);
        virtual MBOOL   enable(CPU_CTRL_RANGE_INFO &cpuRangeInfo);
        virtual MBOOL   enableBoost(MINT timeout);
        virtual MBOOL   disable(void);
        //
    private:
        void*                   mPerfLib;
        funcPerfRegBL           mPerfReg;
        //
        funcEnableTimeout       mPerfEnableTimeout;
        funcEnable              mPerfEnable;
        funcDisable             mPerfDisable;
        //
        funcRegScn              mPerfRegScn;
        funcRegScnConfig        mPerfRegScnConfig;
        //
        funcGetCapability       mPerfGetCapability;
        //
        mutable Mutex           mLock;
        volatile MINT32         mUser;
        MBOOL                   mEnable;
        MINT32                  mPerfHandle;
        CPU_CTRL_INFO           mCpuCtrlInfo;
        CPU_CTRL_RANGE_INFO     mCpuCtrlRangeInfo;
};
//-----------------------------------------------------------------------------
CpuCtrlImp::
CpuCtrlImp()
{
    FUNCTION_NAME;
    mPerfLib = NULL;
    mPerfReg = NULL;
    //
    mPerfEnableTimeout = NULL;
    mPerfEnable = NULL;
    mPerfDisable = NULL;
    //
    mPerfRegScn = NULL;
    mPerfRegScnConfig = NULL;
    //
    mPerfGetCapability = NULL;
    //
    mUser = 0;
    mEnable = MFALSE;
    mPerfHandle = -1;
}
//----------------------------------------------------------------------------
CpuCtrlImp::
~CpuCtrlImp()
{
    FUNCTION_NAME;
}
//-----------------------------------------------------------------------------
CpuCtrl*
CpuCtrl::
createInstance(void)
{
    #if MTK_BASIC_PACKAGE
    LOG_WRN("Not Support for basic package");
    return NULL;
    #endif
    //
    FUNCTION_NAME;
    //
    CpuCtrlImp* pInstance = new CpuCtrlImp();
    return pInstance;
}
//----------------------------------------------------------------------------
MVOID
CpuCtrlImp::
destroyInstance(void)
{
    FUNCTION_IN;
    //
    delete this;
    //
    FUNCTION_OUT;
}
//----------------------------------------------------------------------------
MBOOL
CpuCtrlImp::
enable(CPU_CTRL_INFO &cpuInfo)
{
    CAM_TRACE_NAME("CpuCtrl::enable");
    FUNCTION_IN;
    //
    MBOOL ret = MTRUE;
    //
    {
        Mutex::Autolock templock(mLock);
        //
        if(mUser <= 0)
        {
            LOG_WRN("No user");
            ret = MFALSE;
            FUNCTION_OUT;
            return ret;
        }
    }
    //
    if(mEnable)
    {
        disable();
    }
    //
    Mutex::Autolock lock(mLock);
    //
    LOG_MSG("big(%d/%d),little(%d/%d),timeout(%d)",
            cpuInfo.bigCore,
            cpuInfo.bigFreq,
            cpuInfo.littleCore,
            cpuInfo.littleFreq,
            cpuInfo.timeout);
    //
    mCpuCtrlInfo = cpuInfo;
    //
    mPerfHandle = mPerfReg(
                    mCpuCtrlInfo.bigCore,
                    mCpuCtrlInfo.bigFreq,
                    mCpuCtrlInfo.littleCore,
                    mCpuCtrlInfo.littleFreq);
    if(mCpuCtrlInfo.timeout > 0)
    {
        mPerfEnableTimeout(
            mPerfHandle,
            mCpuCtrlInfo.timeout);
    }
    else
    {
        mPerfEnable(mPerfHandle);
    }
    mEnable = MTRUE;
    //
    EXIT:
    FUNCTION_OUT;
    return ret;
}
//----------------------------------------------------------------------------
MBOOL
CpuCtrlImp::
enable(CPU_CTRL_RANGE_INFO &cpuRangeInfo)
{
    CAM_TRACE_NAME("CpuCtrl::enable");
    FUNCTION_IN;
    //
    MBOOL ret = MTRUE;
    //
    {
        Mutex::Autolock templock(mLock);
        //
        if(mUser <= 0)
        {
            LOG_WRN("No user");
            ret = MFALSE;
            FUNCTION_OUT;
            return ret;
        }
    }
    //
    if(mEnable)
    {
        disable();
    }
    //
    Mutex::Autolock lock(mLock);
    //
    LOG_MSG("MIN CORE:big/little(%d/%d), MAX CORE:big/little(%d/%d)",
            cpuRangeInfo.bigCoreMin,
            cpuRangeInfo.littleCoreMin,
            cpuRangeInfo.bigCoreMax,
            cpuRangeInfo.littleCoreMax);
    LOG_MSG("MIN FREQ:big/little(%d/%d), MAX FREQ:big/little(%d/%d)",
            cpuRangeInfo.bigFreqMin,
            cpuRangeInfo.bigFreqMax,
            cpuRangeInfo.littleFreqMin,
            cpuRangeInfo.littleFreqMax);
    LOG_MSG("timeout(%d)",cpuRangeInfo.timeout);
    //
    mCpuCtrlRangeInfo = cpuRangeInfo;
    //
    mPerfHandle = mPerfRegScn();
    //
    #if (!MTK_BASIC_PACKAGE)
    if( mCpuCtrlRangeInfo.bigCoreMin >= 0 &&
        mCpuCtrlRangeInfo.littleCoreMin >= 0)
    {
        LOG_MSG("CMD_SET_CPU_CORE_BIG_LITTLE_MIN");
        mPerfRegScnConfig(
            mPerfHandle,
            CMD_SET_CPU_CORE_BIG_LITTLE_MIN,
            mCpuCtrlRangeInfo.bigCoreMin,
            mCpuCtrlRangeInfo.littleCoreMin,
            0,
            0);
    }
    if( mCpuCtrlRangeInfo.bigCoreMax >= 0 &&
        mCpuCtrlRangeInfo.littleCoreMax >= 0)
    {
        LOG_MSG("CMD_SET_CPU_CORE_BIG_LITTLE_MAX");
        mPerfRegScnConfig(
            mPerfHandle,
            CMD_SET_CPU_CORE_BIG_LITTLE_MAX,
            mCpuCtrlRangeInfo.bigCoreMax,
            mCpuCtrlRangeInfo.littleCoreMax,
            0,
            0);
    }
    if( mCpuCtrlRangeInfo.bigFreqMin >= 0 &&
        mCpuCtrlRangeInfo.littleFreqMin >= 0)
    {
        LOG_MSG("CMD_SET_CPU_FREQ_BIG_LITTLE_MIN");
        mPerfRegScnConfig(
            mPerfHandle,
            CMD_SET_CPU_FREQ_BIG_LITTLE_MIN,
            mCpuCtrlRangeInfo.bigFreqMin,
            mCpuCtrlRangeInfo.littleFreqMin,
            0,
            0);
    }
    if( mCpuCtrlRangeInfo.bigFreqMax >= 0 &&
        mCpuCtrlRangeInfo.littleFreqMax >= 0)
    {
        LOG_MSG("CMD_SET_CPU_FREQ_BIG_LITTLE_MAX");
        mPerfRegScnConfig(
            mPerfHandle,
            CMD_SET_CPU_FREQ_BIG_LITTLE_MAX,
            mCpuCtrlRangeInfo.bigFreqMax,
            mCpuCtrlRangeInfo.littleFreqMax,
            0,
            0);
    }
    #endif
    if(mCpuCtrlRangeInfo.timeout > 0)
    {
        mPerfEnableTimeout(
            mPerfHandle,
            mCpuCtrlRangeInfo.timeout);
    }
    else
    {
        mPerfEnable(mPerfHandle);
    }
    mEnable = MTRUE;
    //
    EXIT:
    FUNCTION_OUT;
    return ret;
}
//----------------------------------------------------------------------------
MBOOL
CpuCtrlImp::
enableBoost(MINT timeout)
{
    CAM_TRACE_NAME("CpuCtrl::enableBoost");
    FUNCTION_IN;
    //
    MBOOL ret = MTRUE;
    //
    {
        Mutex::Autolock templock(mLock);
        //
        if(mUser <= 0)
        {
            LOG_WRN("No user");
            ret = MFALSE;
            FUNCTION_OUT;
            return ret;
        }
    }
    //
    if(mEnable)
    {
        disable();
    }
    //
    Mutex::Autolock lock(mLock);
    //
    mPerfHandle = mPerfRegScn();
    #if (!MTK_BASIC_PACKAGE)
    mPerfRegScnConfig(
        mPerfHandle,
        CMD_SET_NORMALIZED_PERF_INDEX,
        mPerfGetCapability(CMD_GET_PERF_NORMALIZED_INDEX_MAX),
        0,
        0,
        0);
    #endif
    //
    if(timeout > 0)
    {
        mPerfEnableTimeout(
            mPerfHandle,
            timeout);
    }
    else
    {
        mPerfEnable(mPerfHandle);
    }
    //
    mEnable = MTRUE;
    //
    EXIT:
    FUNCTION_OUT;
    return ret;
}

//----------------------------------------------------------------------------
MBOOL
CpuCtrlImp::
disable()
{
    CAM_TRACE_NAME("CpuCtrl::disable");
    FUNCTION_IN;
    //
    MBOOL ret = MTRUE;
    //
    Mutex::Autolock lock(mLock);
    //
    if(mUser <= 0)
    {
        LOG_WRN("No user");
        ret = MFALSE;
        goto EXIT;
    }
    //
    if(mEnable)
    {
        mPerfDisable(mPerfHandle);
        mEnable = MFALSE;
    }
    else
    {
        LOG_MSG("Not thing to disable");
    }
    //
    EXIT:
    FUNCTION_OUT;
    return ret;
}
//----------------------------------------------------------------------------
MBOOL
CpuCtrlImp::
init(void)
{
    CAM_TRACE_NAME("CpuCtrl::init");
    //
    MBOOL ret = MTRUE;
    MUINT32 i;
    //
    Mutex::Autolock lock(mLock);
    //
    if(mUser == 0)
    {
        LOG_MSG("First user(%d)",mUser);
    }
    else
    {
        LOG_MSG("More user(%d)",mUser);
        android_atomic_inc(&mUser);
        goto EXIT;
    }
    //
    mPerfLib = NULL;
    //
    mPerfEnableTimeout = NULL;
    mPerfEnable = NULL;
    mPerfDisable = NULL;
    //
    mPerfRegScn = NULL;
    mPerfRegScnConfig = NULL;
    //
    mPerfGetCapability = NULL;
    //
    mEnable = MFALSE;
    mPerfHandle = -1;
    //
    if(!mPerfLib)
    {
        mPerfLib = ::dlopen(PERF_LIB_FULLNAME, RTLD_NOW);
        if(! mPerfLib)
        {
            char const *err_str = ::dlerror();
            LOG_ERR("dlopen: %s error=%s", PERF_LIB_FULLNAME, (err_str ? err_str : "unknown"));
            ret = MFALSE;
            goto EXIT;
        }
    }
    //
    mPerfEnableTimeout = reinterpret_cast<funcEnableTimeout>(dlsym(mPerfLib, STR_FUNC_ENABLETIMEOUT));
    if(mPerfEnableTimeout == NULL)
    {
        LOG_WRN("cannot get %s", STR_FUNC_ENABLETIMEOUT);
    }
    //
    mPerfEnable = reinterpret_cast<funcEnable>(dlsym(mPerfLib, STR_FUNC_ENABLE));
    if(mPerfEnable == NULL)
    {
        LOG_WRN("cannot get %s", STR_FUNC_ENABLE);
    }
    //
    mPerfDisable = reinterpret_cast<funcDisable>(dlsym(mPerfLib, STR_FUNC_DISABLE));
    if(mPerfDisable == NULL)
    {
        LOG_WRN("cannot get %s", STR_FUNC_DISABLE);
    }
    //
    mPerfRegScn = reinterpret_cast<funcRegScn>(dlsym(mPerfLib, STR_FUNC_REGSCN));
    if(mPerfRegScn == NULL)
    {
        LOG_WRN("cannot get %s", STR_FUNC_REGSCN);
    }
    //
    mPerfRegScnConfig = reinterpret_cast<funcRegScnConfig>(dlsym(mPerfLib, STR_FUNC_REGSCNCONFIG));
    if(mPerfRegScnConfig == NULL)
    {
        LOG_WRN("cannot get %s", STR_FUNC_REGSCNCONFIG);
    }
    //
    mPerfReg = reinterpret_cast<funcPerfRegBL>(dlsym(mPerfLib, STR_FUNC_REG));
    if(mPerfReg == NULL)
    {
        LOG_WRN("cannot get %s", STR_FUNC_REG);
        ret = MFALSE;
        goto EXIT;
    }
    //
    mPerfGetCapability = reinterpret_cast<funcGetCapability>(dlsym(mPerfLib, STR_FUNC_GETCAPABILITY));
    if(mPerfGetCapability == NULL)
    {
        LOG_WRN("cannot get %s", STR_FUNC_GETCAPABILITY);
        return MFALSE;
    }
    //
    android_atomic_inc(&mUser);
    //
    EXIT:
    FUNCTION_OUT;
    return ret;
}
//----------------------------------------------------------------------------
MBOOL
CpuCtrlImp::
uninit(void)
{
    CAM_TRACE_NAME("CpuCtrl::uninit");
    //
    MBOOL ret = MTRUE;
    MUINT32 i;
    status_t status;
    funcPerfUnreg pUnreg = NULL;
    //
    {
        Mutex::Autolock templock(mLock);
        //
        if(mUser <= 0)
        {
            LOG_WRN("No user(%d)",mUser);
            FUNCTION_OUT;
            return ret;
        }
    }
    //
    if(mEnable)
    {
        disable();
    }
    //
    Mutex::Autolock lock(mLock);
    //
    android_atomic_dec(&mUser);
    //
    if(mUser == 0)
    {
        LOG_MSG("Last user(%d)",mUser);
    }
    else
    {
        LOG_MSG("More user(%d)",mUser);
        goto EXIT;
    }
    //
    pUnreg = reinterpret_cast<funcPerfUnreg>(dlsym(mPerfLib, STR_FUNC_UNREG));
    if(pUnreg != NULL)
    {
        pUnreg(mPerfHandle);
        mPerfHandle = -1;
    }
    else
    {
        LOG_WRN("cannot get %s", STR_FUNC_UNREG);
    }
    //
    if(!mPerfLib)
    {
        LOG_WRN("no lib");
        ret = MFALSE;
        goto EXIT;
    }
    //
    mPerfEnableTimeout = NULL;
    mPerfEnable = NULL;
    mPerfDisable = NULL;
    //
    mPerfRegScn = NULL;
    mPerfRegScnConfig = NULL;
    //
    mPerfGetCapability = NULL;
    //
    ::dlclose(mPerfLib);
    mPerfLib = NULL;
    //
    EXIT:
    FUNCTION_OUT;
    return ret;
}
//----------------------------------------------------------------------------


