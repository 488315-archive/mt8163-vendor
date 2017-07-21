#ifeq ($(MTK_CROSSMOUNT_SUPPORT), yes)

LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := com.mediatek.crossmount.adapter.xml
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_CLASS := ETC
LOCAL_MODULE_PATH := $(TARGET_OUT_ETC)/permissions
LOCAL_SRC_FILES := $(LOCAL_MODULE)

include $(BUILD_PREBUILT)

include $(CLEAR_VARS)

LOCAL_SRC_FILES := $(call all-java-files-under, src)
LOCAL_SRC_FILES += \
      src/com/mediatek/crossmount/adapter/ICrossMountCallback.aidl \
      src/com/mediatek/crossmount/adapter/ICrossMountCameraCallback.aidl \
      src/com/mediatek/crossmount/adapter/ICrossMountAdapter.aidl

LOCAL_AIDL_INCLUDES := protect-app/packages/apps/CrossMount/adapter/src/com/mediatek/crossmount/adapter

LOCAL_MODULE:= com.mediatek.crossmount.adapter
LOCAL_MODULE_TAGS := optional

include $(BUILD_JAVA_LIBRARY)

# put the classes.jar, with full class files instead of classes.dex inside, into the dist directory
$(call dist-for-goals, droidcore, $(full_classes_jar):com.mediatek.crossmount.adapter.jar)

#endif

