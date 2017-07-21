/*
 * Copyright (C) 2012 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef PQ_IBLUELIGHTFILTER_H
#define PQ_IBLUELIGHTFILTER_H

#include <utils/Singleton.h>
#include <utils/StrongPointer.h>

#define BLUE_LIGHT_FILTER_ENABLE_STRING 	"persist.sys.pq.blf.en"
#define BLUE_LIGHT_FILTER_LEVEL_STRING 		"persist.sys.pq.blf.level"
#define BLUE_LIGHT_FILTER_COEF_STRING 		"persist.sys.pq.blf.coef"
#define BLUE_LIGHT_FILTER_ENABLE_DEFAULT 	"0"
#define BLUE_LIGHT_FILTER_LEVEL_DEFAULT 	"0"
#define BLUE_LIGHT_FILTER_COEF_DEFAULT 	"102400000000000010240000000000001024"

#define BLUE_LIGHT_FILTER_LEVEL_RANGE 		255

#define TUNING_FIELD_EN 			0
#define TUNING_FIELD_STRENGTH 		1
#define TUNING_FIELD_STRENGTH_LIMIT 2
#define TUNING_FIELD_CCORR_0_0 		3
#define TUNING_FIELD_CCORR_0_1 		4
#define TUNING_FIELD_CCORR_0_2 		5
#define TUNING_FIELD_CCORR_1_0 		6
#define TUNING_FIELD_CCORR_1_1 		7
#define TUNING_FIELD_CCORR_1_2 		8
#define TUNING_FIELD_CCORR_2_0 		9
#define TUNING_FIELD_CCORR_2_1 		10
#define TUNING_FIELD_CCORR_2_2 		11
#define TUNING_FIELD_CT2DEG 		32
#define TUNING_FIELD_CT1DEG 		33
#define TUNING_FIELD_CTORICT 		34
#define TUNING_FIELD_COL_XYZ_R_X 	35
#define TUNING_FIELD_COL_XYZ_R_Y 	36
#define TUNING_FIELD_COL_XYZ_R_Z 	37
#define TUNING_FIELD_COL_XYZ_G_X 	38
#define TUNING_FIELD_COL_XYZ_G_Y 	39
#define TUNING_FIELD_COL_XYZ_G_Z 	40
#define TUNING_FIELD_COL_XYZ_B_X 	41
#define TUNING_FIELD_COL_XYZ_B_Y 	42
#define TUNING_FIELD_COL_XYZ_B_Z 	43
#define TUNING_FIELD_TUNE_DX 		44
#define TUNING_FIELD_TUNE_DY 		45
#define TUNING_FIELD_REMAIN_SHIFTEN 46
#define TUNING_FIELD_SHIFT_DX 		47
#define TUNING_FIELD_SHIFT_DY 		48

#define TUNING_FIELD_NUM 20


namespace android
{

struct TuningFields
{
	int field[TUNING_FIELD_NUM];
	unsigned int value[TUNING_FIELD_NUM];
};

class BlueLightFilter;

class IBlueLightFilter : public Singleton<IBlueLightFilter>
{
friend class Singleton<IBlueLightFilter>;
private:
	BlueLightFilter *blueLightFilter;
	IBlueLightFilter();
public:
	bool SetBlueLightFilterStrength(int strength);
	int GetBlueLightFilterStrength(void);
	bool EnableBlueLightFilter(bool enable);
	bool IsBlueLightFilterEnabled(void);
	int GetBlueLightFilterStrengthRange(void);
	bool BlueLightFilterInit(void);
	//bool BlueLightFilterPowerOn(void);
	bool BlueLightFilterPowerOn(int coef[3][3]);
	bool BlueLightFilterTuningSet(struct TuningFields tfs);
	bool BlueLightFilterTuningGet(struct TuningFields *tfs);
	bool BlueLightFilterTuning(void);
};

};
#endif
