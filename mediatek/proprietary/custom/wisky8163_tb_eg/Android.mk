LOCAL_PATH:= $(call my-dir)
ifeq ($(MTK_PROJECT), wisky8163_tb_eg)
include $(call all-makefiles-under,$(LOCAL_PATH))
endif
