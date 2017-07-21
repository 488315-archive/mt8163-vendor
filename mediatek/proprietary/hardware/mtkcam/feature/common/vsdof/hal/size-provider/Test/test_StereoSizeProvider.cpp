#include <limits.h>
#include <gtest/gtest.h>
#include <vsdof/hal/stereo_size_provider.h>
#include <metadata/IMetadataProvider.h>

//Use define since sensor must be powered on before using StereoSettingProvider
#define FE_RESIZE_RATIO StereoSettingProvider::getStereoCameraFOVRatio()
#define PADDING_RATIO   (StereoSettingProvider::getStereoCameraFOVRatio()-1.0f)

//=============================================================================
//  PASS 1
//=============================================================================
TEST(PASS1_TEST, PREVIEW_SCENRATIO)
{
    ENUM_STEREO_SCENARIO scenario = eSTEREO_SCENARIO_PREVIEW;
    int index1, index2;
    StereoSettingProvider::getStereoSensorIndex(index1, index2);

    MUINT32 junkStride;
    MRect tgCropRect;
    MSize szRRZO;
    StereoSizeProvider::getInstance()->getPass1Size( eSTEREO_SENSOR_MAIN1,
                                                     eImgFmt_FG_BAYER10,
                                                     EPortIndex_RRZO,
                                                     scenario,
                                                     //below are outputs
                                                     tgCropRect,
                                                     szRRZO,
                                                     junkStride);
    EXPECT_TRUE( tgCropRect == MRect(MPoint(0, 377), MSize(4208, 2366)) );
    EXPECT_TRUE( szRRZO == MSize(2100, 1180) );

    StereoSizeProvider::getInstance()->getPass1Size( eSTEREO_SENSOR_MAIN2,
                                                     eImgFmt_FG_BAYER10,
                                                     EPortIndex_RRZO,
                                                     scenario,
                                                     //below are outputs
                                                     tgCropRect,
                                                     szRRZO,
                                                     junkStride);
    EXPECT_TRUE( tgCropRect == MRect(MPoint(0, 240), MSize(2560, 1440)) );
    EXPECT_TRUE( szRRZO == MSize(1280, 720) );

    StereoSizeProvider::getInstance()->getPass1ActiveArrayCrop(eSTEREO_SENSOR_MAIN1, tgCropRect);
    EXPECT_TRUE(tgCropRect == MRect(MPoint(0, 377), MSize(4208, 2366)));
    StereoSizeProvider::getInstance()->getPass1ActiveArrayCrop(eSTEREO_SENSOR_MAIN2, tgCropRect);
    EXPECT_TRUE(tgCropRect == MRect(MPoint(0, 240), MSize(2560, 1440)));
}

TEST(PASS1_TEST, RECORD_SCENRATIO)
{
    ENUM_STEREO_SCENARIO scenario = eSTEREO_SCENARIO_RECORD;
    int index1, index2;
    StereoSettingProvider::getStereoSensorIndex(index1, index2);

    MUINT32 junkStride;
    MRect tgCropRect;
    MSize szRRZO;
    StereoSizeProvider::getInstance()->getPass1Size( eSTEREO_SENSOR_MAIN1,
                                                     eImgFmt_FG_BAYER10,
                                                     EPortIndex_RRZO,
                                                     scenario,
                                                     //below are outputs
                                                     tgCropRect,
                                                     szRRZO,
                                                     junkStride);
    EXPECT_TRUE( tgCropRect == MRect(MPoint(0, 377), MSize(4208, 2366)) );
    EXPECT_TRUE( szRRZO == MSize(2100, 1180) );

    StereoSizeProvider::getInstance()->getPass1Size( eSTEREO_SENSOR_MAIN2,
                                                     eImgFmt_FG_BAYER10,
                                                     EPortIndex_RRZO,
                                                     scenario,
                                                     //below are outputs
                                                     tgCropRect,
                                                     szRRZO,
                                                     junkStride);
    EXPECT_TRUE( tgCropRect == MRect(MPoint(0, 240), MSize(2560, 1440)) );
    EXPECT_TRUE( szRRZO == MSize(1280, 720) );
}

TEST(PASS1_TEST, CAPTURE_SCENRATIO)
{
    ENUM_STEREO_SCENARIO scenario = eSTEREO_SCENARIO_CAPTURE;
    int index1, index2;
    StereoSettingProvider::getStereoSensorIndex(index1, index2);

    MUINT32 junkStride;
    MRect tgCropRect;
    MSize szRRZO;
    StereoSizeProvider::getInstance()->getPass1Size( eSTEREO_SENSOR_MAIN1,
                                                     eImgFmt_FG_BAYER10,
                                                     EPortIndex_RRZO,
                                                     scenario,
                                                     //below are outputs
                                                     tgCropRect,
                                                     szRRZO,
                                                     junkStride);
    EXPECT_TRUE( tgCropRect == MRect(MPoint(0, 377), MSize(4208, 2366)) );
    EXPECT_TRUE( szRRZO == MSize(2100, 1180) );

    StereoSizeProvider::getInstance()->getPass1Size( eSTEREO_SENSOR_MAIN2,
                                                     eImgFmt_FG_BAYER10,
                                                     EPortIndex_RRZO,
                                                     scenario,
                                                     //below are outputs
                                                     tgCropRect,
                                                     szRRZO,
                                                     junkStride);
    EXPECT_TRUE( tgCropRect == MRect(MPoint(0, 240), MSize(2560, 1440)) );
    EXPECT_TRUE( szRRZO == MSize(1280, 720) );

    MSize szMainIMGO;
    StereoSizeProvider::getInstance()->getPass1Size( eSTEREO_SENSOR_MAIN1,
                                                     eImgFmt_BAYER10,
                                                     EPortIndex_IMGO,
                                                     scenario,
                                                     //below are outputs
                                                     tgCropRect,
                                                     szMainIMGO,
                                                     junkStride);
    EXPECT_TRUE( tgCropRect == MRect(MPoint(0, 0), MSize(4208, 3120)) );
    EXPECT_TRUE( szMainIMGO == MSize(4208, 3120) );

    EXPECT_TRUE(StereoSizeProvider::getInstance()->getSBSImageSize() == MSize(4352, 1152));
}
//=============================================================================
//  PASS 2
//=============================================================================
TEST(PASS2_TEST, PREVIEW_SCENRATIO)
{
    ENUM_STEREO_SCENARIO scenario = eSTEREO_SCENARIO_PREVIEW;

    int index1, index2;
    StereoSettingProvider::getStereoSensorIndex(index1, index2);

    StereoSizeProvider *sizeProvider = StereoSizeProvider::getInstance();
    Pass2SizeInfo pass2SizeInfo;

    // PASS2A
    sizeProvider->getPass2SizeInfo(PASS2A, scenario, pass2SizeInfo);
    EXPECT_TRUE(pass2SizeInfo.areaWDMA.size == MSize(1920, 1080));
    EXPECT_TRUE(pass2SizeInfo.szWROT == MSize(960, 544));
    EXPECT_TRUE(pass2SizeInfo.areaFEO.size == MSize(1600, 900));
    EXPECT_TRUE(pass2SizeInfo.szIMG2O == MSize(640, 360));

    // PASS2A'
    sizeProvider->getPass2SizeInfo(PASS2A_P, scenario, pass2SizeInfo);
    MSize szPass2AP_WROT(960, 544);
    szPass2AP_WROT.w = szPass2AP_WROT.w * FE_RESIZE_RATIO + 15;
    szPass2AP_WROT.h = szPass2AP_WROT.h * FE_RESIZE_RATIO + 15;
    szPass2AP_WROT = (szPass2AP_WROT >> 4) << 4;
    EXPECT_TRUE(pass2SizeInfo.szWROT == szPass2AP_WROT);
    EXPECT_TRUE(pass2SizeInfo.areaFEO.size == MSize(1600, 900));

    // PASS2A-2
    sizeProvider->getPass2SizeInfo(PASS2A_2, scenario, pass2SizeInfo);
    EXPECT_TRUE(pass2SizeInfo.areaWDMA.size == MSize(240, 136));
    EXPECT_TRUE(pass2SizeInfo.szIMG2O == MSize(480, 272));
    EXPECT_TRUE(pass2SizeInfo.areaFEO.size == MSize(960, 544));

    // PASS2A'-2
    sizeProvider->getPass2SizeInfo(PASS2A_P_2, scenario, pass2SizeInfo);
    EXPECT_TRUE(pass2SizeInfo.areaWDMA.size == MSize(240, 136));
    MSize szPass2AP2_IMG2O(480, 272);
    szPass2AP2_IMG2O.w = szPass2AP2_IMG2O.w * FE_RESIZE_RATIO + 15;
    szPass2AP2_IMG2O.h = szPass2AP2_IMG2O.h * FE_RESIZE_RATIO + 15;
    szPass2AP2_IMG2O = (szPass2AP2_IMG2O >> 4) << 4;
    EXPECT_TRUE(pass2SizeInfo.szIMG2O == szPass2AP2_IMG2O);

    MSize padding = MSize(szPass2AP_WROT.w-960, szPass2AP_WROT.h - 544);
    EXPECT_TRUE(pass2SizeInfo.areaFEO == StereoArea(szPass2AP_WROT, padding, MPoint(padding.w>>1, padding.h>>1)));

    // PASS2A-3
    sizeProvider->getPass2SizeInfo(PASS2A_3, scenario, pass2SizeInfo);
    EXPECT_TRUE(pass2SizeInfo.szIMG2O == MSize(128, 72));
    EXPECT_TRUE(pass2SizeInfo.areaFEO.size == MSize(480, 272));

    // PASS2A'-3
    sizeProvider->getPass2SizeInfo(PASS2A_P_3, scenario, pass2SizeInfo);
    EXPECT_TRUE(pass2SizeInfo.szIMG2O == MSize(128, 72));

    padding = MSize(szPass2AP2_IMG2O.w - 480, szPass2AP2_IMG2O.h - 272);
    EXPECT_TRUE(pass2SizeInfo.areaFEO == StereoArea(szPass2AP2_IMG2O, padding, MPoint(padding.w>>1, padding.h>>1)));
}

TEST(PASS2_TEST, CAPTURE_SCENRATIO)
{
    ENUM_STEREO_SCENARIO scenario = eSTEREO_SCENARIO_CAPTURE;
    int index1, index2;
    StereoSettingProvider::getStereoSensorIndex(index1, index2);

    StereoSizeProvider *sizeProvider = StereoSizeProvider::getInstance();
    Pass2SizeInfo pass2SizeInfo;

    // PASS2A
    sizeProvider->getPass2SizeInfo(PASS2A, scenario, pass2SizeInfo);
    EXPECT_TRUE(pass2SizeInfo.areaWDMA.size == MSize(3072, 1728));
    EXPECT_TRUE(pass2SizeInfo.szWROT == MSize(960, 544));
    EXPECT_TRUE(pass2SizeInfo.areaFEO.size == MSize(1600, 900));

    // PASS2A'
    sizeProvider->getPass2SizeInfo(PASS2A_P, scenario, pass2SizeInfo);
    MSize szPass2AP_WROT(960, 544);
    szPass2AP_WROT.w = szPass2AP_WROT.w * FE_RESIZE_RATIO + 15;
    szPass2AP_WROT.h = szPass2AP_WROT.h * FE_RESIZE_RATIO + 15;
    szPass2AP_WROT = (szPass2AP_WROT >> 4) << 4;
    EXPECT_TRUE(pass2SizeInfo.szWROT == szPass2AP_WROT);
    EXPECT_TRUE(pass2SizeInfo.areaFEO.size == MSize(1600, 900));

    // PASS2A-2
    sizeProvider->getPass2SizeInfo(PASS2A_2, scenario, pass2SizeInfo);
    EXPECT_TRUE(pass2SizeInfo.areaWDMA == StereoArea(2176, 1152, 256, 72, 128, 36));
    EXPECT_TRUE(pass2SizeInfo.szIMG2O == MSize(480, 272));
    EXPECT_TRUE(pass2SizeInfo.areaFEO.size == MSize(960, 544));

    // PASS2A'-2
    sizeProvider->getPass2SizeInfo(PASS2A_P_2, scenario, pass2SizeInfo);
    EXPECT_TRUE(pass2SizeInfo.areaWDMA == StereoArea(2176, 1152, 256, 72, 128, 36));
    MSize szPass2AP2_IMG2O(480, 272);
    szPass2AP2_IMG2O.w = szPass2AP2_IMG2O.w * FE_RESIZE_RATIO + 15;
    szPass2AP2_IMG2O.h = szPass2AP2_IMG2O.h * FE_RESIZE_RATIO + 15;
    szPass2AP2_IMG2O = (szPass2AP2_IMG2O >> 4) << 4;
    EXPECT_TRUE(pass2SizeInfo.szIMG2O == szPass2AP2_IMG2O);

    MSize padding = MSize(szPass2AP_WROT.w-960, szPass2AP_WROT.h - 544);
    EXPECT_TRUE(pass2SizeInfo.areaFEO == StereoArea(szPass2AP_WROT, padding, MPoint(padding.w>>1, padding.h>>1)));

    // PASS2A-3
    sizeProvider->getPass2SizeInfo(PASS2A_3, scenario, pass2SizeInfo);
    EXPECT_TRUE(pass2SizeInfo.szIMG2O == MSize(128, 72));
    EXPECT_TRUE(pass2SizeInfo.areaFEO.size == MSize(480, 272));

    // PASS2A'-3
    sizeProvider->getPass2SizeInfo(PASS2A_P_3, scenario, pass2SizeInfo);
    EXPECT_TRUE(pass2SizeInfo.szIMG2O == MSize(128, 72));

    padding = MSize(szPass2AP2_IMG2O.w - 480, szPass2AP2_IMG2O.h - 272);
    EXPECT_TRUE(pass2SizeInfo.areaFEO == StereoArea(szPass2AP2_IMG2O, padding, MPoint(padding.w>>1, padding.h>>1)));
}

TEST(PASS2_TEST, RECORD_SCENRATIO)
{
    ENUM_STEREO_SCENARIO scenario = eSTEREO_SCENARIO_RECORD;
    int index1, index2;
    StereoSettingProvider::getStereoSensorIndex(index1, index2);

    StereoSizeProvider *sizeProvider = StereoSizeProvider::getInstance();
    Pass2SizeInfo pass2SizeInfo;

    // PASS2A
    sizeProvider->getPass2SizeInfo(PASS2A, scenario, pass2SizeInfo);
    EXPECT_TRUE(pass2SizeInfo.areaWDMA.size == MSize(1920, 1080));
    EXPECT_TRUE(pass2SizeInfo.szWROT == MSize(960, 544));
    EXPECT_TRUE(pass2SizeInfo.szIMG2O == MSize(640, 360));
    EXPECT_TRUE(pass2SizeInfo.areaFEO.size == MSize(1600, 900));
    EXPECT_TRUE(pass2SizeInfo.szIMG2O == MSize(640, 360));

    // PASS2A'
    sizeProvider->getPass2SizeInfo(PASS2A_P, scenario, pass2SizeInfo);
    MSize szPass2AP_WROT(960, 544);
    szPass2AP_WROT.w = szPass2AP_WROT.w * FE_RESIZE_RATIO + 15;
    szPass2AP_WROT.h = szPass2AP_WROT.h * FE_RESIZE_RATIO + 15;
    szPass2AP_WROT = (szPass2AP_WROT >> 4) << 4;
    EXPECT_TRUE(pass2SizeInfo.szWROT == szPass2AP_WROT);
    EXPECT_TRUE(pass2SizeInfo.areaFEO.size == MSize(1600, 900));

    // PASS2A-2
    sizeProvider->getPass2SizeInfo(PASS2A_2, scenario, pass2SizeInfo);
    EXPECT_TRUE(pass2SizeInfo.areaWDMA.size == MSize(288, 162));
    EXPECT_TRUE(pass2SizeInfo.szIMG2O == MSize(480, 272));
    EXPECT_TRUE(pass2SizeInfo.areaFEO.size == MSize(960, 544));

    // PASS2A'-2
    sizeProvider->getPass2SizeInfo(PASS2A_P_2, scenario, pass2SizeInfo);
    EXPECT_TRUE(pass2SizeInfo.areaWDMA.size == MSize(288, 162));
    MSize szPass2AP2_IMG2O(480, 272);
    szPass2AP2_IMG2O.w = szPass2AP2_IMG2O.w * FE_RESIZE_RATIO + 15;
    szPass2AP2_IMG2O.h = szPass2AP2_IMG2O.h * FE_RESIZE_RATIO + 15;
    szPass2AP2_IMG2O = (szPass2AP2_IMG2O >> 4) << 4;
    EXPECT_TRUE(pass2SizeInfo.szIMG2O == szPass2AP2_IMG2O);

    MSize padding = MSize(szPass2AP_WROT.w-960, szPass2AP_WROT.h - 544);
    EXPECT_TRUE(pass2SizeInfo.areaFEO == StereoArea(szPass2AP_WROT, padding, MPoint(padding.w>>1, padding.h>>1)));

    // PASS2A-3
    sizeProvider->getPass2SizeInfo(PASS2A_3, scenario, pass2SizeInfo);
    EXPECT_TRUE(pass2SizeInfo.areaFEO.size == MSize(480, 272));

    // PASS2A'-3
    sizeProvider->getPass2SizeInfo(PASS2A_P_3, scenario, pass2SizeInfo);
    EXPECT_TRUE(pass2SizeInfo.szIMG2O == MSize(128, 72));

    padding = MSize(szPass2AP2_IMG2O.w - 480, szPass2AP2_IMG2O.h - 272);
    EXPECT_TRUE(pass2SizeInfo.areaFEO == StereoArea(szPass2AP2_IMG2O, padding, MPoint(padding.w>>1, padding.h>>1)));
}