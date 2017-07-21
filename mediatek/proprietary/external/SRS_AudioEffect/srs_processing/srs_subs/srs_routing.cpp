/*======================================================================*
 DTS, Inc.
 5220 Las Virgenes Road
 Calabasas, CA 91302  USA

 CONFIDENTIAL: CONTAINS CONFIDENTIAL PROPRIETARY INFORMATION OWNED BY
 DTS, INC. AND/OR ITS AFFILIATES ("DTS"), INCLUDING BUT NOT LIMITED TO
 TRADE SECRETS, KNOW-HOW, TECHNICAL AND BUSINESS INFORMATION. USE,
 DISCLOSURE OR DISTRIBUTION OF THE SOFTWARE IN ANY FORM IS LIMITED TO
 SPECIFICALLY AUTHORIZED LICENSEES OF DTS.  ANY UNAUTHORIZED
 DISCLOSURE IS A VIOLATION OF STATE, FEDERAL, AND INTERNATIONAL LAWS.
 BOTH CIVIL AND CRIMINAL PENALTIES APPLY.

 DO NOT DUPLICATE. COPYRIGHT 2014, DTS, INC. ALL RIGHTS RESERVED.
 UNAUTHORIZED DUPLICATION IS A VIOLATION OF STATE, FEDERAL AND
 INTERNATIONAL LAWS.

 ALGORITHMS, DATA STRUCTURES AND METHODS CONTAINED IN THIS SOFTWARE
 MAY BE PROTECTED BY ONE OR MORE PATENTS OR PATENT APPLICATIONS.
 UNLESS OTHERWISE PROVIDED UNDER THE TERMS OF A FULLY-EXECUTED WRITTEN
 AGREEMENT BY AND BETWEEN THE RECIPIENT HEREOF AND DTS, THE FOLLOWING
 TERMS SHALL APPLY TO ANY USE OF THE SOFTWARE (THE "PRODUCT") AND, AS
 APPLICABLE, ANY RELATED DOCUMENTATION:  (i) ANY USE OF THE PRODUCT
 AND ANY RELATED DOCUMENTATION IS AT THE RECIPIENT’S SOLE RISK:
 (ii) THE PRODUCT AND ANY RELATED DOCUMENTATION ARE PROVIDED "AS IS"
 AND WITHOUT WARRANTY OF ANY KIND AND DTS EXPRESSLY DISCLAIMS ALL
 WARRANTIES, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO ANY
 IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 PURPOSE, REGARDLESS OF WHETHER DTS KNOWS OR HAS REASON TO KNOW OF THE
 USER'S PARTICULAR NEEDS; (iii) DTS DOES NOT WARRANT THAT THE PRODUCT
 OR ANY RELATED DOCUMENTATION WILL MEET USER'S REQUIREMENTS, OR THAT
 DEFECTS IN THE PRODUCT OR ANY RELATED DOCUMENTATION WILL BE
 CORRECTED; (iv) DTS DOES NOT WARRANT THAT THE OPERATION OF ANY
 HARDWARE OR SOFTWARE ASSOCIATED WITH THIS DOCUMENT WILL BE
 UNINTERRUPTED OR ERROR-FREE; AND (v) UNDER NO CIRCUMSTANCES,
 INCLUDING NEGLIGENCE, SHALL DTS OR THE DIRECTORS, OFFICERS, EMPLOYEES,
 OR AGENTS OF DTS, BE LIABLE TO USER FOR ANY INCIDENTAL, INDIRECT,
 SPECIAL, OR CONSEQUENTIAL DAMAGES (INCLUDING BUT NOT LIMITED TO
 DAMAGES FOR LOSS OF BUSINESS PROFITS, BUSINESS INTERRUPTION, AND LOSS
 OF BUSINESS INFORMATION) ARISING OUT OF THE USE, MISUSE, OR INABILITY
 TO USE THE PRODUCT OR ANY RELATED DOCUMENTATION.
*======================================================================*/

#include "srs_techs/srs_tech_headers.h"

#include <media/AudioSystem.h>

#undef LOG_TAG
#define LOG_TAG "SRS_Routing"

namespace android {

SRS_Param gRoute_Params[] = {
    { -1,       SRS_PTYP_INFO,  SRS_PFMT_STATIC,    0.0f,   0.0f,   1.0f,   "routing_info", "Routing Values", "0 = int, 1 = ext, -1 = disallow srs processing, -2 = int dsp, -3 = ext dsp", "", 0},
    { 1000,     SRS_PTYP_CFG,   SRS_PFMT_INT,       -1.0f,  -16.0f, 16.0f,  "force_route", "Hard Lock Routing to a specific setting", "", "", 0},
    { 0,        SRS_PTYP_CFG,   SRS_PFMT_INT,       0.0f,   0.0f,   16.0f,  "earpiece", "DEVICE_OUT_EARPIECE", "", "", 0},
    { 1,        SRS_PTYP_CFG,   SRS_PFMT_INT,       0.0f,   0.0f,   16.0f,  "speaker", "DEVICE_OUT_SPEAKER", "", "", 0},
    { 2,        SRS_PTYP_CFG,   SRS_PFMT_INT,       1.0f,   0.0f,   16.0f,  "wired_headset", "DEVICE_OUT_WIRED_HEADSET", "", "", 0},
    { 3,        SRS_PTYP_CFG,   SRS_PFMT_INT,       1.0f,   0.0f,   16.0f,  "wired_headphone", "DEVICE_OUT_WIRED_HEADPHONE", "", "", 0},
    { 4,        SRS_PTYP_CFG,   SRS_PFMT_INT,       1.0f,   0.0f,   16.0f,  "bt_sco", "DEVICE_OUT_BLUETOOTH_SCO", "", "", 0},
    { 5,        SRS_PTYP_CFG,   SRS_PFMT_INT,       1.0f,   0.0f,   16.0f,  "bt_sco_headset", "DEVICE_OUT_BLUETOOTH_SCO_HEADSET", "", "", 0},
    { 6,        SRS_PTYP_CFG,   SRS_PFMT_INT,       1.0f,   0.0f,   16.0f,  "bt_sco_carkit", "DEVICE_OUT_BLUETOOTH_SCO_CARKIT", "", "", 0},
    { 7,        SRS_PTYP_CFG,   SRS_PFMT_INT,       1.0f,   0.0f,   16.0f,  "bt_a2dp", "DEVICE_OUT_BLUETOOTH_A2DP", "", "", 0},
    { 8,        SRS_PTYP_CFG,   SRS_PFMT_INT,       1.0f,   0.0f,   16.0f,  "bt_a2dp_headphones", "DEVICE_OUT_BLUETOOTH_A2DP_HEADPHONES", "", "", 0},
    { 9,        SRS_PTYP_CFG,   SRS_PFMT_INT,       1.0f,   0.0f,   16.0f,  "bt_a2dp_speaker", "DEVICE_OUT_BLUETOOTH_A2DP_SPEAKER", "", "", 0},
    { 10,       SRS_PTYP_CFG,   SRS_PFMT_INT,       1.0f,   0.0f,   16.0f,  "aux_digital", "DEVICE_OUT_AUX_DIGITAL", "", "", 0},
    { 11,       SRS_PTYP_CFG,   SRS_PFMT_INT,       1.0f,   0.0f,   16.0f,  "dock_analog_headset", "DEVICE_OUT_ANLG_DOCK_HEADSET", "", "", 0},
    { 12,       SRS_PTYP_CFG,   SRS_PFMT_INT,       1.0f,   0.0f,   16.0f,  "dock_digital_headset", "DEVICE_OUT_DGTL_DOCK_HEADSET", "", "", 0},
    { 13,       SRS_PTYP_CFG,   SRS_PFMT_INT,       1.0f,   0.0f,   16.0f,  "usb_accessory", "DEVICE_OUT_USB_ACCESSORY", "", "", 0},
    { 14,       SRS_PTYP_CFG,   SRS_PFMT_INT,       1.0f,   0.0f,   16.0f,  "usb_device", "DEVICE_OUT_USB_DEVICE", "", "", 0},
};

SRS_RouteMap::SRS_RouteMap(){
    unsigned int i;

    for (i=0; i<SRS_ROUTEMAP_MAXROUTES; i++){
        if (i < 2) RouteTable[i] = 0;
        else RouteTable[i] = 1;
    }

    ForceRoute = -1;
    AllowBT = 1;
}

SRS_Param* SRS_RouteMap::RouteParams(){ return gRoute_Params; }
int SRS_RouteMap::RouteParamCount(){ return sizeof(gRoute_Params)/sizeof(SRS_Param); }

void SRS_RouteMap::RouteMapSet(int index, const char* pValue){
    HELP_ParamIn In;

    if (index < 0) return;
    if (index >= SRS_ROUTEMAP_MAXROUTES){
        if (index == 1000) ForceRoute = In.GetInt(pValue);
        return;
    }


    RouteTable[index] = In.GetInt(pValue);
}

const char* SRS_RouteMap::RouteMapGet(int index){
    HELP_ParamOut Out;

    if (index < 0) return "";
    if (index >= SRS_ROUTEMAP_MAXROUTES){
        if (index == 1000) return Out.FromInt(ForceRoute);
        return "";
    }

    return Out.FromInt(RouteTable[index]);
}

int SRS_RouteMap::ResolveRoute(int routeFlags, int* pFoundFlags){
    int tRetRoute = -1;             // If the device is not present, srs processing will not happen
    int tRoute = routeFlags;

    SRS_LOG("ResolveRoute 1: %d %d", routeFlags, tRoute);
/*
    if (AllowBT == 1){
        // Ignore Route - and scan for the active device...
        if (AudioSystem::getDeviceConnectionState(AUDIO_DEVICE_OUT_BLUETOOTH_SCO_HEADSET, "") != AUDIO_POLICY_DEVICE_STATE_UNAVAILABLE){ tRoute = AUDIO_DEVICE_OUT_BLUETOOTH_SCO_HEADSET; SRS_LOG("BT SCO HEAD"); }
        if (AudioSystem::getDeviceConnectionState(AUDIO_DEVICE_OUT_BLUETOOTH_SCO_CARKIT, "") != AUDIO_POLICY_DEVICE_STATE_UNAVAILABLE){ tRoute = AUDIO_DEVICE_OUT_BLUETOOTH_SCO_CARKIT; SRS_LOG("BT SCO CAR"); }
        if (AudioSystem::getDeviceConnectionState(AUDIO_DEVICE_OUT_BLUETOOTH_SCO, "") != AUDIO_POLICY_DEVICE_STATE_UNAVAILABLE){ tRoute = AUDIO_DEVICE_OUT_BLUETOOTH_SCO; SRS_LOG("BT SCO"); }
        if (AudioSystem::getDeviceConnectionState(AUDIO_DEVICE_OUT_BLUETOOTH_A2DP_HEADPHONES, "") != AUDIO_POLICY_DEVICE_STATE_UNAVAILABLE){ tRoute = AUDIO_DEVICE_OUT_BLUETOOTH_A2DP_HEADPHONES; SRS_LOG("BT A2DP HP"); }
        if (AudioSystem::getDeviceConnectionState(AUDIO_DEVICE_OUT_BLUETOOTH_A2DP_SPEAKER, "") != AUDIO_POLICY_DEVICE_STATE_UNAVAILABLE){ tRoute = AUDIO_DEVICE_OUT_BLUETOOTH_A2DP_SPEAKER; SRS_LOG("BT A2DP SPK"); }
        if (AudioSystem::getDeviceConnectionState(AUDIO_DEVICE_OUT_BLUETOOTH_A2DP, "") != AUDIO_POLICY_DEVICE_STATE_UNAVAILABLE){ tRoute = AUDIO_DEVICE_OUT_BLUETOOTH_A2DP; SRS_LOG("BT A2DP"); }
    }
*/
    if (ForceRoute != -1){
        SRS_LOG("Route Forced To %d", ForceRoute);
        tRoute = ForceRoute;
    }

    SRS_LOG("ResolveRoute 2: %d %d", routeFlags, tRoute);

    // Ugly, but the only 'right' way to do it.
    if (tRoute > 0){
        if (tRoute&AUDIO_DEVICE_OUT_EARPIECE) tRetRoute = RouteTable[0];
        if (tRoute&AUDIO_DEVICE_OUT_SPEAKER) tRetRoute = RouteTable[1];
        if (tRoute&AUDIO_DEVICE_OUT_WIRED_HEADSET) tRetRoute = RouteTable[2];
        if (tRoute&AUDIO_DEVICE_OUT_WIRED_HEADPHONE) tRetRoute = RouteTable[3];
        if (tRoute&AUDIO_DEVICE_OUT_BLUETOOTH_SCO) tRetRoute = RouteTable[4];
        if (tRoute&AUDIO_DEVICE_OUT_BLUETOOTH_SCO_HEADSET) tRetRoute = RouteTable[5];
        if (tRoute&AUDIO_DEVICE_OUT_BLUETOOTH_SCO_CARKIT) tRetRoute = RouteTable[6];
        if (tRoute&AUDIO_DEVICE_OUT_BLUETOOTH_A2DP) tRetRoute = RouteTable[7];
        if (tRoute&AUDIO_DEVICE_OUT_BLUETOOTH_A2DP_HEADPHONES) tRetRoute = RouteTable[8];
        if (tRoute&AUDIO_DEVICE_OUT_BLUETOOTH_A2DP_SPEAKER) tRetRoute = RouteTable[9];
        if (tRoute&AUDIO_DEVICE_OUT_AUX_DIGITAL) tRetRoute = RouteTable[10];
        if (tRoute&AUDIO_DEVICE_OUT_ANLG_DOCK_HEADSET) tRetRoute = RouteTable[11];
        if (tRoute&AUDIO_DEVICE_OUT_DGTL_DOCK_HEADSET) tRetRoute = RouteTable[12];
        if (tRoute&AUDIO_DEVICE_OUT_USB_ACCESSORY) tRetRoute = RouteTable[13];
        if (tRoute&AUDIO_DEVICE_OUT_USB_DEVICE) tRetRoute = RouteTable[14];
        /// M: ALPS02083661: Support Remote Submix @{
#ifdef MTK_AUDIO
        if (tRoute&AUDIO_DEVICE_OUT_REMOTE_SUBMIX) tRetRoute = RouteTable[15];
#endif
        /// @}
    } else {
        tRetRoute = -1;
    }

    SRS_LOG("ResolveRoute 3: %d %d", routeFlags, tRoute);
    SRS_LOG("ResolveRoute: tRetRoute = %d", tRetRoute);

    if (pFoundFlags != NULL) *pFoundFlags = tRoute;

    return tRetRoute;
}

};  // namespace android

