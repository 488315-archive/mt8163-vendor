#pragma once

#ifdef WIN32
#define	DEF_CriticalSection_Win(cs) CRITICAL_SECTION cs;
#define	DEF_CriticalSection_Linux(cs)
#define	DEF_AutoLock(cs) AutoLock lock(cs)
#define	DEF_InitCs(cs) InitializeCriticalSection(&cs)
#else

#include <utils/threads.h>
using namespace android;
#define	DEF_CriticalSection_Win(cs)
#define	DEF_CriticalSection_Linux(cs) mutable Mutex cs;
#define	DEF_AutoLock(cs) Mutex::Autolock lock(cs)
#define	DEF_InitCs(cs)
#endif


//

class NvBufUtil
{
public:
	enum
	{
		e_SensorDevWrong=-1000,
		e_NvramIdWrong,
		e_NV_SensorDevWrong,
		e_NV_SensorIdNull,
	};
public:
	static NvBufUtil& getInstance();
	void setAndroidMode(int isAndroid=1); //anroid mode: android:1, meta(cct, factory):0

	/*
	nvRamId:
    CAMERA_NVRAM_DATA_ISP,
	CAMERA_NVRAM_DATA_3A,
	CAMERA_NVRAM_DATA_SHADING,
	CAMERA_NVRAM_DATA_LENS,
	CAMERA_DATA_AE_PLINETABLE,
	CAMERA_NVRAM_DATA_STROBE,
	CAMERA_NVRAM_DATA_N3D3A,
	CAMERA_NVRAM_DATA_GEOMETRY,
	CAMERA_NVRAM_VERSION,
	*/

	int getBuf(CAMERA_DATA_TYPE_ENUM nvRamId, int sensorDev, void*& p);
	int getBufAndRead(CAMERA_DATA_TYPE_ENUM nvRamId, int sensorDev, void*& p, int bForceRead=0);
	int getBufAndReadNoDefault(CAMERA_DATA_TYPE_ENUM nvRamId, int sensorDev, void*& p, int bForceRead=0);
	int write(CAMERA_DATA_TYPE_ENUM nvRamId, int sensorDev);

	// note: please provide memory to call the function.
	// For sync the buf data with NvRam data, the internal buf can't be used in the function.
	int readDefault(CAMERA_DATA_TYPE_ENUM nvRamId, int sensorDev, void* p);

	int readDefault(CAMERA_DATA_TYPE_ENUM nvRamId, int sensorDev);

private:
	DEF_CriticalSection_Linux(m_cs);
	NvBufUtil();
};

int nvbufUtil_getSensorId(int sensorDev, int& sensorId);



