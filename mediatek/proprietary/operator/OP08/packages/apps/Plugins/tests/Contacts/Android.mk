LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

# We only want this apk build for tests.
LOCAL_MODULE_TAGS := tests
LOCAL_CERTIFICATE := shared

LOCAL_JAVA_LIBRARIES := android.test.runner
LOCAL_JAVA_LIBRARIES += ims-common


# Include all test java files.
LOCAL_SRC_FILES := $(call all-java-files-under, src)
LOCAL_JACK_ENABLED := disabled
LOCAL_STATIC_JAVA_LIBRARIES := librobotium4

LOCAL_PACKAGE_NAME := CommonPresenceContactTests

LOCAL_INSTRUMENTATION_FOR := Contacts

include $(BUILD_PACKAGE)
