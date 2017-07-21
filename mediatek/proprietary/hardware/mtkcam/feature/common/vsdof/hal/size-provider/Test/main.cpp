#include <limits.h>
#include <gtest/gtest.h>

#include <IHalSensor.h>
#include <metadata/IMetadataProvider.h>
#include <vsdof/hal/stereo_setting_provider.h>

#define MY_LOGD(fmt, arg...)    printf("[D][%s]" fmt"\n", __func__, ##arg)
#define MY_LOGI(fmt, arg...)    printf("[I][%s]" fmt"\n", __func__, ##arg)
#define MY_LOGW(fmt, arg...)    printf("[W][%s] WRN(%5d):" fmt"\n", __func__, __LINE__, ##arg)
#define MY_LOGE(fmt, arg...)    printf("[E][%s] %s ERROR(%5d):" fmt"\n", __func__,__FILE__, __LINE__, ##arg)

#define USER_NAME "HAL_UT"

int main(int argc, char **argv)
{
    testing::InitGoogleTest(&argc, argv);

    //======== Power on sensor ==========
    NSCam::IHalSensorList* pHalSensorList = NSCam::IHalSensorList::get();
    pHalSensorList->searchSensors();
    int sensorCount = pHalSensorList->queryNumberOfSensors();

    int main1Idx, main2Idx;
    StereoSettingProvider::getStereoSensorIndex(main1Idx, main2Idx);
    MUINT pIndex[2] = { (MUINT)main1Idx, (MUINT)main2Idx };
    MUINT const main1Index = pIndex[0];
    MUINT const main2Index = pIndex[1];
    if(!pHalSensorList)
    {
        MY_LOGE("pHalSensorList == NULL");
    }
    //
    IHalSensor *pHalSensor = pHalSensorList->createSensor(
                                            USER_NAME,
                                            2,
                                            pIndex);
    if(pHalSensor == NULL)
    {
       MY_LOGE("pHalSensor is NULL");
    }
    // In stereo mode, Main1 needs power on first.
    // Power on main1 and main2 successively one after another.
    if( !pHalSensor->powerOn(USER_NAME, 1, &main1Index) )
    {
        MY_LOGE("sensor power on failed: %d", main1Index);
    }
    if( !pHalSensor->powerOn(USER_NAME, 1, &main2Index) )
    {
        MY_LOGE("sensor power on failed: %d", main2Index);
    }

    sp<IMetadataProvider> pMetadataProvider = IMetadataProvider::create(main1Idx);
    NSMetadataProviderManager::add(main1Idx, pMetadataProvider.get());
    pMetadataProvider = IMetadataProvider::create(main2Idx);
    NSMetadataProviderManager::add(main2Idx, pMetadataProvider.get());
    //
    int result = RUN_ALL_TESTS();
    //
    NSMetadataProviderManager::clear();

    if( !pHalSensor->powerOff(USER_NAME, 1, &main1Index) )
    {
        MY_LOGE("sensor power off failed: %d", main1Index);
    }
    if( !pHalSensor->powerOff(USER_NAME, 1, &main2Index) )
    {
        MY_LOGE("sensor power off failed: %d", main2Index);
    }

    return result;
}