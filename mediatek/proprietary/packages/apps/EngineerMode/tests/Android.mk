LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

# We only want this apk build for tests.
LOCAL_MODULE_TAGS := tests
LOCAL_CERTIFICATE := platform

LOCAL_JAVA_LIBRARIES := android.test.runner \
                        mediatek-framework \
                        telephony-common

LOCAL_STATIC_JAVA_LIBRARIES := librobotium4 \
                               engineermodelibannotations


# Include all test java files.
LOCAL_SRC_FILES := $(call all-java-files-under, src)

LOCAL_PACKAGE_NAME := EngineerModeTests

LOCAL_INSTRUMENTATION_FOR := EngineerMode

include $(BUILD_PACKAGE)

include $(CLEAR_VARS)
LOCAL_PREBUILT_STATIC_JAVA_LIBRARIES := engineermodelibannotations:libs/mtkatannotations.jar
include $(BUILD_MULTI_PREBUILT)
