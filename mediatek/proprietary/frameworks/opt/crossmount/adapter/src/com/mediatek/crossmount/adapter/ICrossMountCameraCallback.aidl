package com.mediatek.crossmount.adapter;

import android.hardware.camera2.impl.CameraMetadataNative;

oneway interface ICrossMountCameraCallback
{
  void onRepeatRequest(in CameraMetadataNative metaNative);

  void onNonRepeatRequest(in CameraMetadataNative metaNative, in int[] imageInfo);
}
