package com.mediatek.crossmount.adapter;

import android.content.Context;
import android.graphics.drawable.Drawable;
import android.os.IBinder;
import android.os.RemoteException;


import com.mediatek.crossmount.adapter.CrossMountAdapter;
import com.mediatek.crossmount.adapter.Device;

/**
 * Represents the local HotKnot adapter.
 * <p>
 * Use the helper {@link #getDefaultAdapter(Context)} to get the default HotKnot
 * adapter for the Android device.
 *
 */
public final class Service {
    static final String TAG = "[CrossMount]Service";


    private CrossMountAdapter mAdapter;
    private String mServiceId;
    private Device mDevice;

    public enum ServiceState {
        INVALID,

        /* Others */
        OCCUPIED,
        AVAILABLE,

        /* Self */
        DIABLED,
        ENABLED,

        /* Both */
        CONNECTING,
        CONNECTED
    }

    Service(String serviceId, Device device, CrossMountAdapter adapter) {
        mServiceId = serviceId;
        mDevice = device;
        mAdapter = adapter;
    }

    public String getName() {

        return mAdapter.getServiceName(mDevice.getId(), mServiceId);

    }

    public int getSerialNumber() {
        return mAdapter.getServiceSerialNumber(mDevice.getId(), mServiceId);
    }

    public ServiceState getState(String sharedDeviceId) {


        String state = mAdapter.getServiceStatus(mDevice.getId(), mServiceId, sharedDeviceId);

        if (state == null){
            return ServiceState.INVALID;
        } else{
            //String enumString = myEnum.name()

            return ServiceState.valueOf(state);
        }

    }

    public boolean setAllow(boolean allow){

        return mAdapter.setServiceAllow(mServiceId, allow);
    }

    public boolean getAllow(){

        return mAdapter.getServiceAllow(mServiceId);

    }

    public boolean isPlugIn(){
        return mAdapter.isPlugInService(mServiceId, mDevice.getId());
    }

    public Drawable getPlugInIcon(){
        return mAdapter.getPlugInIcon(mServiceId, mDevice.getId());
    }


    public Device[] getSharedDevices(){

        String[] deviceIds = mAdapter.getSharedDevices(mServiceId);
        Device[] devices = new Device[deviceIds.length];

        int i = 0;
        for (String deviceId : deviceIds) {
            String deviceName = mAdapter.getDeviceName(deviceId);
            String deviceType = mAdapter.getDeviceType(deviceId);

            devices[i] = new Device(deviceId, deviceName, deviceType, mAdapter);
            i ++;
        }
        return devices;
    }

    public boolean shareTo(String deviceId){
        return mAdapter.shareProviderService(mServiceId, deviceId);
    }

    public boolean stopShareTo(String deviceId){
        return mAdapter.stopShareProviderService(mServiceId, deviceId);
    }

    public boolean mountFrom(){
        return mAdapter.mountProviderService(mServiceId, mDevice.getId());
    }

    public boolean stopMountFrom(){
        return mAdapter.stopMountProviderService(mServiceId, mDevice.getId());
    }

    /*
     * Specific for camera service
     */
    public int getCameraId(){
        return mAdapter.getRemoteCameraId(mDevice.getId());
    }

}
