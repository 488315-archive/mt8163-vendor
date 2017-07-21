# Copyright 2006 The Android Open Source Project

# XXX using libutils for simulator build only...
#
LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_SRC_FILES:= \
    ril_callbacks.c \
    atchannel.c \
    misc.c \
    at_tok.c \
    socket_channel.cpp \
    socket_util.cpp \
    framework/base/RfxVariant.cpp \
    framework/base/RfxMessage.cpp \
    framework/base/RfxSocketState.cpp \
    framework/core/Rfx.cpp \
    framework/core/RfxObject.cpp \
    framework/core/RfxClassInfo.cpp \
    framework/core/RfxAction.cpp \
    framework/core/RfxAsyncSignal.cpp \
    framework/core/RfxSignal.cpp \
    framework/core/RfxTimer.cpp \
    framework/core/RfxMainThread.cpp \
    framework/core/RfxController.cpp \
    framework/core/RfxRootController.cpp \
    framework/core/RfxSlotRootController.cpp \
    framework/core/RfxControllerFactory.cpp \
    framework/core/RfxStatusManager.cpp \
    framework/core/RfxRilAdapter.cpp \
    framework/core/RfxDispatchThread.cpp \
    framework/core/RfxSocketStateManager.cpp \
    framework/base/RfxDebugInfo.cpp \
    framework/base/RfxDefaultDestUtils.cpp \
    framework/base/RfxDataToParcelUtils.cpp \
    telephony/RfxHelloController.cpp \
    telephony/RfxSampleController.cpp \
    telephony/sim/RpSimController.cpp \
    telephony/sim/RpSimControllerBase.cpp \
    telephony/sim/RpGsmSimController.cpp \
    telephony/sim/RpC2kSimController.cpp \
    telephony/ss/RpSsController.cpp \
    telephony/call/RpCallController.cpp \
    telephony/call/RpCallCenterController.cpp \
    telephony/call/RpRedialHandler.cpp \
    telephony/cat/RpCatController.cpp \
    telephony/sms/RpSmsCtrlBase.cpp \
    telephony/sms/RpSmsCtrl.cpp \
    telephony/sms/RpGsmSmsCtrl.cpp \
    telephony/sms/RpCdmaSmsCtrl.cpp \
    telephony/nw/RpNwController.cpp \
    telephony/nw/RpNwRatController.cpp \
    telephony/nw/RpBaseNwRatSwitchHandler.cpp \
    telephony/nw/RpGsmNwRatSwitchHandler.cpp \
    telephony/nw/RpCdmaNwRatSwitchHandler.cpp \
    telephony/nw/RpCdmaLteNwRatSwitchHandler.cpp \
    telephony/nw/RpNwStateController.cpp \
    telephony/nw/RpNwPhoneTypeUpdater.cpp \
    framework/base/RfxNwServiceState.cpp \
    telephony/data/RpDataControllerProxy.cpp \
    telephony/data/RpDataController.cpp \
    telephony/data/RpCdmaLteDataController.cpp \
    telephony/data/RpIratController.cpp \
    telephony/data/RpDataUtils.cpp \
    telephony/data/RpBaseApnHandler.cpp \
    telephony/data/RpCnomApnHandler.cpp \
    telephony/data/RpWwomApnHandler.cpp \
    telephony/data/RpDataConnectionInfo.cpp \
    telephony/capabilityswitch/RpCapabilitySwitchController.cpp \
    telephony/power/RpModemController.cpp \
    telephony/power/RpRadioController.cpp \
    telephony/power/RpCdmaRadioController.cpp \
    telephony/power/RadioMessage.cpp \
    telephony/power/ModemMessage.cpp \
    telephony/modecontroller/RpCardModeControllerFactory.cpp \
    telephony/modecontroller/RpCardTypeReadyController.cpp \
    telephony/modecontroller/RpCdmaLteModeController.cpp \
    telephony/modecontroller/RpModeInitController.cpp \
    telephony/modecontroller/RpOMCardModeController.cpp \
    telephony/modecontroller/RpOP09CardModeController.cpp \
    telephony/modecontroller/RpOP01CardModeController.cpp \
    telephony/oem/RpCdmaOemController.cpp \
    framework/core/RfxTestBasicController.cpp \
    framework/core/RfxTestSuitController.cpp \
    telephony/phb/RpPhbController.cpp \
    telephony/atci/RpAtciController.cpp \
    telephony/ir/RpIrBaseHandler.cpp \
    telephony/ir/RpIrCdmaHandler.cpp \
    telephony/ir/RpIrNwRatSwitchCallback.cpp \
    telephony/ir/RpIrOP09CdmaStatusListener.cpp \
    telephony/ir/RpIrController.cpp \
    telephony/ir/RpIrLwgHandler.cpp \
    telephony/ir/RpIrOP09LwgStatusListener.cpp \
    telephony/ir/RpIrStrategyOP094M.cpp \
    telephony/ir/RpIrStrategyOP095M.cpp \
    telephony/ir/RpIrStrategy.cpp \
    telephony/ir/RpIrStrategyOm.cpp \
    telephony/ir/RpIrMccTable.cpp \
    telephony/ir/utils/RpIrMccConvertMappingTables.cpp \
    telephony/ir/utils/RpIrInvalidMccConvert.cpp \
    telephony/agps/RpAgpsSlotController.cpp \
    telephony/agps/RpAgpsNSlotController.cpp \
    telephony/agps/RpAgpsThread.cpp \
    telephony/agps/RpAgpsMessage.cpp \
    telephony/agps/RpAgpsdAdapter.cpp \
    telephony/agps/RpAgpsHandler.cpp \
    telephony/agps/RpAgpsLog.cpp \
    telephony/mal/RilClient.cpp \
    telephony/mal/RilClientQueue.cpp \
    telephony/mal/RpRilClientController.cpp \
    telephony/mal/RpMalController.cpp \
    telephony/wp/RpWpController.cpp \

LOCAL_SHARED_LIBRARIES := \
    liblog libbinder libcutils libutils librilproxy librilproxyutils

LOCAL_STATIC_LIBRARIES := \
    libprotobuf-c-nano-enable_malloc \

# for asprinf
LOCAL_CFLAGS := -D_GNU_SOURCE -DANDROID_MULTI_SIM

ifeq ($(MTK_SHARE_MODEM_SUPPORT), 2)
    LOCAL_CFLAGS += -DANDROID_SIM_COUNT_2
endif

ifeq ($(MTK_SHARE_MODEM_SUPPORT), 3)
    LOCAL_CFLAGS += -DANDROID_SIM_COUNT_3
endif

ifeq ($(MTK_SHARE_MODEM_SUPPORT), 4)
    LOCAL_CFLAGS += -DANDROID_SIM_COUNT_4
endif

LOCAL_C_INCLUDES := \
  $(TOP)/system/core/include/utils \
  $(TOP)/frameworks/native/include/binder \
  $(TOP)/$(MTK_PATH_SOURCE)/hardware/ril/rilproxy/mtk-rilproxy/framework/include \
  $(TOP)/$(MTK_PATH_SOURCE)/hardware/ril/rilproxy/mtk-rilproxy/framework/include/base \
  $(TOP)/$(MTK_PATH_SOURCE)/hardware/ril/rilproxy/mtk-rilproxy/framework/include/core \
  $(TOP)/$(MTK_PATH_SOURCE)/hardware/ril/rilproxy/mtk-rilproxy/telephony \
  $(TOP)/$(MTK_PATH_SOURCE)/hardware/ril/rilproxy/mtk-rilproxy/telephony/ir/include \
  $(TOP)/$(MTK_PATH_SOURCE)/hardware/ril/rilproxy/mtk-rilproxy/telephony/nw \
  $(TOP)/$(MTK_PATH_SOURCE)/hardware/ril/rilproxy/mtk-rilproxy/telephony/wp \

LOCAL_C_INCLUDES += $(TARGET_OUT_HEADER)/librilproxyutils
LOCAL_C_INCLUDES += external/nanopb-c

ifeq ($(TARGET_DEVICE),sooner)
  LOCAL_CFLAGS += -DUSE_TI_COMMANDS
endif

ifeq ($(TARGET_DEVICE),surf)
  LOCAL_CFLAGS += -DPOLL_CALL_STATE -DUSE_QMI
endif

ifeq ($(TARGET_DEVICE),dream)
  LOCAL_CFLAGS += -DPOLL_CALL_STATE -DUSE_QMI
endif

LOCAL_STATIC_LIBRARIES := \
    libprotobuf-c-nano-enable_malloc \

ifeq (foo,foo)
  #build shared library
  LOCAL_SHARED_LIBRARIES += \
      libcutils libutils
  LOCAL_CFLAGS += -DRIL_SHLIB
  LOCAL_MODULE:= mtk-rilproxy
  include $(BUILD_SHARED_LIBRARY)
else
  #build executable
  LOCAL_SHARED_LIBRARIES += \
      libril
  LOCAL_MODULE:= mtk-rilproxy
  include $(BUILD_EXECUTABLE)
endif
