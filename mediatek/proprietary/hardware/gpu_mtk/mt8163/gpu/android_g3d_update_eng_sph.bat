set PROJ=tb8163m1_64_sph
rem set PROJ=tb8163p3_64_sph
rem set PROJ=tb8163m1_64_sph_qxga
rem set PROJ=tb8163m1_64_sph_wuxga

adb root

rem wait adbd reboot
sleep 5

adb remount

rem adb shell mkdir -p /system/lib/modules

adb shell rm /system/lib/egl/libEGL_yoli.so
adb shell rm /system/lib/egl/libGLESv1_CM_yoli.so
adb shell rm /system/lib/egl/libGLESv2_yoli.so
adb shell rm /system/lib64/egl/libEGL_yoli.so
adb shell rm /system/lib64/egl/libGLESv1_CM_yoli.so
adb shell rm /system/lib64/egl/libGLESv2_yoli.so

adb push ./eng/libEGL_yoli.so /system/lib/egl
adb push ./eng/libGLESv1_CM_yoli.so /system/lib/egl
adb push ./eng/libGLESv2_yoli.so /system/lib/egl
adb push ./eng64/libEGL_yoli.so /system/lib64/egl
adb push ./eng64/libGLESv1_CM_yoli.so /system/lib64/egl
adb push ./eng64/libGLESv2_yoli.so /system/lib64/egl

adb shell chmod 755 /system/lib/egl/libEGL_yoli.so
adb shell chmod 755 /system/lib/egl/libGLESv1_CM_yoli.so
adb shell chmod 755 /system/lib/egl/libGLESv2_yoli.so
adb shell chmod 755 /system/lib64/egl/libEGL_yoli.so
adb shell chmod 755 /system/lib64/egl/libGLESv1_CM_yoli.so
adb shell chmod 755 /system/lib64/egl/libGLESv2_yoli.so

adb shell rm /system/lib/libG3D.so
adb shell rm /system/lib/libGLAPI.so
adb shell rm /system/lib/libgl_compiler.so
adb shell rm /system/lib/libgpu_llvm.so
adb shell rm /system/lib64/libG3D.so
adb shell rm /system/lib64/libGLAPI.so
adb shell rm /system/lib64/libgl_compiler.so
adb shell rm /system/lib64/libgpu_llvm.so

adb push ./eng/libG3D.so /system/lib
adb push ./eng/libGLAPI.so /system/lib
adb push ./eng/libgl_compiler.so /system/lib
adb push ./eng/libgpu_llvm.so /system/lib
adb push ./eng64/libG3D.so /system/lib64
adb push ./eng64/libGLAPI.so /system/lib64
adb push ./eng64/libgl_compiler.so /system/lib64
adb push ./eng64/libgpu_llvm.so /system/lib64

adb shell chmod 755 /system/lib/libG3D.so
adb shell chmod 755 /system/lib/libGLAPI.so
adb shell chmod 755 /system/lib/libgl_compiler.so
adb shell chmod 755 /system/lib/libgpu_llvm.so
adb shell chmod 755 /system/lib64/libG3D.so
adb shell chmod 755 /system/lib64/libGLAPI.so
adb shell chmod 755 /system/lib64/libgl_compiler.so
adb shell chmod 755 /system/lib64/libgpu_llvm.so

adb shell rm /system/lib/hw/gralloc.mt8163.so
adb shell rm /system/lib64/hw/gralloc.mt8163.so

adb push ./eng/gralloc.mt8163.so /system/lib/hw
adb push ./eng64/gralloc.mt8163.so /system/lib64/hw

adb shell chmod 755 /system/lib/hw/gralloc.mt8163.so
adb shell chmod 755 /system/lib64/hw/gralloc.mt8163.so

