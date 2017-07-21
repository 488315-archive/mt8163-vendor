package com.mediatek.crossmount.adapter;

import android.content.Context;
import android.os.IBinder;
import android.os.RemoteException;
import android.util.Log;

import com.mediatek.crossmount.adapter.CrossMountAdapter;
import com.mediatek.crossmount.adapter.Service;


/**
 * Represents the local HotKnot adapter.
 * <p>
 * Use the helper {@link #getDefaultAdapter(Context)} to get the default HotKnot
 * adapter for the Android device.
 *
 */
public final class Device {
    static final String TAG = "CrossMountDevice";


    private CrossMountAdapter mAdapter;
    private String mDeviceId;
    private String mDeviceName;
    private Type mDeviceType;

    public enum Type {
        INVALID,
        PHONE,
        TV
    }

    public interface circleChangedCallBack {

        public void onCircleResultUpdate(int reason);
    }

    Device(String deviceId, String deviceName, String deviceType, CrossMountAdapter adapter) {
        //Log.d(TAG, "Device, id:" + deviceId + ",name:" + deviceName + ", type:" + deviceType);
        mDeviceId = deviceId;
        mDeviceName = deviceName;

        if (deviceType != null){
            mDeviceType = Type.valueOf(deviceType);
        } else {
            mDeviceType = Type.INVALID;
        }

        mAdapter = adapter;
    }

    public String getId() {
        return mDeviceId;
    }

    public String getName() {
        return mDeviceName; //mAdapter.getDeviceName(mDeviceId);
    }

    public Type getType() {

        return mDeviceType;
        /*
        String type = mAdapter.getDeviceType(mDeviceId);
        if (type != null){
            return Type.valueOf(type);
        } else {
            return Type.INVALID;
        }
        */
    }

    public boolean setName(String name) {

        return mAdapter.setDeviceName(name);
    }

    public Service[] getProviderServices() {

        String[] serviceId = mAdapter.getProviderServices(mDeviceId);

        Service[] services = new Service[serviceId.length];

        int i = 0;
        for (String id : serviceId) {
            services[i] = new Service(id, this, mAdapter);
            i++;
        }
        return services;
    }

    public boolean removeFromTrust() {

        return mAdapter.removeFromTrust(mDeviceId);
    }

    public int getConnectCount(){
        return mAdapter.getConnectCount(mDeviceId);
    }

    public int getMaxNameLength(){
        //TODO: query from framework
        return 20;
    }



}
