/*
 * Copyright 2015 The Android Open Source Project
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

#ifndef _TA_KEYMASTER_H
#define _TA_KEYMASTER_H

#define TZ_TA_KEYMASTER_UUID   "0d5fe516-1634-43e2-bdb4-d485645c4310"

/* Command for Test TA */
#define TZCMD_KEYMASTER_GET_SUPPORT_ALGORITHMS		0
#define TZCMD_KEYMASTER_GET_SUPPORT_BLOCK_MODES		1
#define TZCMD_KEYMASTER_GET_SUPPORT_PADDING_MODES	2
#define TZCMD_KEYMASTER_GET_SUPPORT_DIGESTS			3
#define TZCMD_KEYMASTER_GET_SUPPORT_IMPORT_FORMATS	4
#define TZCMD_KEYMASTER_GET_SUPPORT_EXPORT_FORMATS	5
#define TZCMD_KEYMASTER_ADD_RNG_ENTROPY				6
#define TZCMD_KEYMASTER_GEN_KEY						7
#define TZCMD_KEYMASTER_GET_KEY_CHARACTERISTICS		8
#define TZCMD_KEYMASTER_IMPORT_KEY					9
#define TZCMD_KEYMASTER_EXPORT_KEY					10
#define TZCMD_KEYMASTER_DELETE_KEY					11
#define TZCMD_KEYMASTER_DELETE_ALL_KEYS				12
#define TZCMD_KEYMASTER_BEGIN						13
#define TZCMD_KEYMASTER_UPDATE						14
#define TZCMD_KEYMASTER_FINISH						15
#define TZCMD_KEYMASTER_ABORT						16
#define TZCMD_KEYMASTER_GET_OUTPUT_DATA				17
#define TZCMD_KEYMASTER_MAX_CMD                     18

#endif  /* _TA_KEYMASTER_H */
