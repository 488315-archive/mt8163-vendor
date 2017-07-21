/***************************************************************************** 
 * Include
 *****************************************************************************/
#include "RpAgpsSlotController.h"
#include "RpAgpsHandler.h"
#include "RpAgpsdAdapter.h"
#include "RpAgpsNSlotController.h"
#include <cutils/properties.h>
#include <stdlib.h>
#include <string.h>
#include "RpAgpsThread.h"
#include "RpAgpsLog.h"

/*****************************************************************************
 * Name Space Declarations
 *****************************************************************************/
using ::android::Parcel;

/***************************************************************************** 
 * Class RfxController
 *****************************************************************************/
RFX_IMPLEMENT_CLASS("RpAgpsSlotController",
        RpAgpsSlotController, RfxController);


RpAgpsSlotController::RpAgpsSlotController()
    :m_settingApn(false),
    m_testMode(false),
    m_mockOp09(false) {
    AGPS_LOGD("RpAgpsSlotController::Constructor 0x%x", this);
}


RpAgpsSlotController::~RpAgpsSlotController() {
    AGPS_LOGD("RpAgpsSlotController::Destructor 0x%x", this);
}


void RpAgpsSlotController::onInit() {
    RfxController::onInit();
    AGPS_LOGD("RpAgpsSlotController::onInit slotID = %d, this = 0x%x",
            getSlotId(), this);
    const int urcList[] = {
        RIL_UNSOL_VIA_GPS_EVENT,
    };
    registerToHandleUrc(urcList, sizeof(urcList) / sizeof(urcList));
    registerDataConnectionStateChanged();
}


void RpAgpsSlotController::onDeinit() {
    AGPS_LOGD("RpAgpsSlotController::onDeinit this = 0x%x", this);
    const int urcList[] = {
        RIL_UNSOL_VIA_GPS_EVENT,
    };
    unregisterDataConnectionStateChanged();
    unregisterToHandleUrc(urcList, sizeof(urcList) / sizeof(urcList));
    RfxController::onDeinit();
}


void RpAgpsSlotController::registerDataConnectionStateChanged() {
    getStatusManager()->registerStatusChanged(
            RFX_STATUS_KEY_DATA_CONNECTION,
            RfxStatusChangeCallback(this,
                    &RpAgpsSlotController::onDataConnectionChanged));
}


void RpAgpsSlotController::unregisterDataConnectionStateChanged() {
    getStatusManager()->unRegisterStatusChanged(
            RFX_STATUS_KEY_DATA_CONNECTION,
            RfxStatusChangeCallback(this,
                    &RpAgpsSlotController::onDataConnectionChanged));
}


void RpAgpsSlotController::onDataConnectionChanged(
    RfxStatusKeyEnum key,RfxVariant oldValue,RfxVariant newValue) {

    RFX_ASSERT(RFX_STATUS_KEY_DATA_CONNECTION == key);
    int status = newValue.asInt();
    AGPS_LOGD("RpAgpsSlotController::onDataConnectionChanged %d, this = 0x%x",
        status, this);
    if (status == DATA_STATE_DISCONNECTED) {
        RFX_ASSERT(oldValue.asInt() == DATA_STATE_CONNECTED);
        controlApn(EVENT_AGPS_DESTROY_APN);
    }
}


bool RpAgpsSlotController::isDataOff() {
    RfxStatusManager *statusManager = getStatusManager();
    int status = statusManager->getIntValue(RFX_STATUS_KEY_DATA_CONNECTION,
                         DATA_STATE_DISCONNECTED);
    AGPS_LOGD("RpAgpsSlotController::isDataOff status = %d", status);
    return (status == DATA_STATE_DISCONNECTED) ? true : false;
}


void RpAgpsSlotController::setConnectionState(int state) {
    AGPS_LOGW("RpAgpsSlotController::setConnectionState state = %d", state);
    sp<RfxMessage> requestMsg =
        RfxMessage::obtainRequest(
            getSlotId(),
            RADIO_TECH_GROUP_C2K,
            RIL_REQUEST_AGPS_TCP_CONNIND);
    Parcel *q = requestMsg->getParcel();
    q->writeInt32(1);    // count is 1
    q->writeInt32(state);
    requestToRild(requestMsg);
}


void RpAgpsSlotController::controlApn(int event) {
    bool checkValue = (event == EVENT_AGPS_SET_APN) ? false : true;
    AGPS_LOGD("RpAgpsSlotController::controlApn event = %d, m_settingApn = %d",
                event, m_settingApn);
    if (m_settingApn == checkValue) {
        Parcel *p = new Parcel();
        p->writeInt32(APN_TYPE_CTWAP);
        sp<RpAgpsMessage> msg = RpAgpsMessage::obtainMessage(event, p);
        sp<RpAgpsWorkingThreadHandler> handler
                = new RpAgpsWorkingThreadHandler(msg);
        RpAgpsNSlotController *ctrl =
            (RpAgpsNSlotController *)findController(
                    RFX_OBJ_CLASS_INFO(RpAgpsNSlotController));
        if (ctrl != NULL) {
            sp<RpAgpsThread> t =ctrl->getAgpsThread();
            handler->sendMessage(t->getLooper());
            m_settingApn = !checkValue;
        }
    }
}


bool RpAgpsSlotController::onHandleUrc(const sp<RfxMessage>& message) {
    int msg_id = message->getId();
    if (msg_id == RIL_UNSOL_VIA_GPS_EVENT) {
        Parcel *p = message->getParcel();
        int32_t len;
        int32_t event;
        int32_t status;
        p->readInt32(&len);
        p->readInt32(&event);
        p->readInt32(&status);
        RFX_ASSERT(len == 2);
        RFX_ASSERT(status == 0);
        AGPS_LOGW("RpAgpsSlotController::onHandleUrc event = %d", event);
        if (event == REQUEST_DATA_CONNECTION) {
            if (isDataOff()) {
                setConnectionState(0);
            } else if (isOp09()){
                controlApn(EVENT_AGPS_SET_APN);
            } else {
                setConnectionState(1);
            }

        } else if (event == CLOSE_DATA_CONNECTION) {
            controlApn(EVENT_AGPS_DESTROY_APN);
        } else {
            AGPS_LOGE("RpAgpsSlotController::onHandleUrc unkown event");
        }
        return true;
    } else {
        AGPS_LOGE("RpAgpsSlotController::onHandleUrc error msg = %d", msg_id);
        return false;
    }
}


bool RpAgpsSlotController::isOp09() {
    if (m_testMode) {
        return m_mockOp09;
    }
    char property_value[PROPERTY_VALUE_MAX] = { 0 };
    int ct6m = 0;
    int op09 = 0;
    property_get("ro.ct6m_support", property_value, "0");
    ct6m = atoi(property_value);
    property_get("ro.operator.optr", property_value, "none");
    op09 = (strcmp("OP09", property_value) == 0) ? 1 : 0;
    AGPS_LOGD("RpAgpsSlotController::onHandleUrc op09(%d), ct6m(%d)",
            op09, ct6m);
    return (ct6m || op09) ? true : false;
}


void RpAgpsSlotController::onApnSetResult(bool result) {
    AGPS_LOGW("RpAgpsSlotController::onApnSetResult (%d, %d)",
        result, m_settingApn);
    if (m_settingApn) {
        int status = result? 1 : 0;
        setConnectionState(status);
    }
}