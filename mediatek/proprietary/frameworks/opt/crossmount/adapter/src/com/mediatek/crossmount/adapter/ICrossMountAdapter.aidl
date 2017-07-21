package com.mediatek.crossmount.adapter;

import android.graphics.Bitmap;
import android.net.Uri;
import com.mediatek.crossmount.adapter.ICrossMountCallback;
import com.mediatek.crossmount.adapter.ICrossMountCameraCallback;
import android.hardware.camera2.impl.CameraMetadataNative;

interface ICrossMountAdapter {

    void setCrossMountCallback(ICrossMountCallback callback);
    void releaseCrossMountCallback(ICrossMountCallback callback);

    String getFeatureState();
    void disable();
    void enable();
    boolean startDiscover(in String[] serviceId);
    void stopDiscover();
    boolean isDiscovering();
    String getMyDeviceId();
    String[] getTrustDevices();
    String[] getAvailableDevices();

    String getTrustDeviceName(String deviceId);
    String getTrustDeviceType(String deviceId);

    boolean setDeviceName(String deviceName);
    String getDeviceName(String deviceId);
    String getDeviceType(String deviceId);
    String[] getProviderServices(String deviceId);
    boolean removeFromTrust(String deviceId);
    int getConnectCount(String deviceId);


    String getServiceName(String deviceId, String serviceId);
    int getServiceSerialNumber(String deviceId, String serviceId);
    String getServiceStatus(String deviceId, String serviceId, String sharedDeviceId);

    boolean setServiceAllow(String serviceId, boolean allow);
    boolean getServiceAllow(String serviceId);

    boolean isPlugInService(String serviceId, String deviceId);
    Bitmap getPlugInServiceIcon(String serviceId, String deviceId);

    String[] getSharedDevices(String serviceId);

    boolean shareProviderService(String serviceId, String deviceId);
    boolean mountProviderService(String serviceId, String deviceId);
    boolean stopShareProviderService(String serviceId, String deviceId);
    boolean stopMountProviderService(String serviceId, String deviceId);

    boolean sendControlMessage(String msg);

    String getBuildInServiceName(int serviceType);
    String getBuildInServiceDisplayName(int serviceType);

    int getRemoteCameraId(String deviceId);

    //CameraAdapter
    void setCrossMountCameraCallback(ICrossMountCameraCallback callback);
    boolean sendCameraRepeatResult(in CameraMetadataNative metaNative);
    boolean sendCameraNonRepeatResult(in Uri uri, in CameraMetadataNative metaNative);

}
