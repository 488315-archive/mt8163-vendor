package com.mediatek.crossmount.adapter;

import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.drawable.Drawable;
import android.graphics.drawable.BitmapDrawable;
import android.os.IBinder;
import android.os.RemoteException;
import android.os.ServiceManager;
import android.util.Log;

import java.util.ArrayList;

/**
 * Represents the local HotKnot adapter.
 * <p>
 * Use the helper {@link #getDefaultAdapter(Context)} to get the default HotKnot
 * adapter for the Android device.
 *
 */
public final class CrossMountAdapter extends ICrossMountCallback.Stub{
    static final String TAG = "CrossMountAdapter";

    static final String CROSSMOUNT_MANAGER_SERVICE = "CrossMountManagerService";


    public enum FeatureState {
        DISABLED,
        ENABLING,
        ENABLED
    }

    public enum BuildInServiceType{
        CAMERA,
        SPEAKER,
        MICROPHONE,
        CONTROLLER,
        INVALID
    }

    // Default adapter
    private static CrossMountAdapter sDefaultAdapter;

    // Locker
    private static Object sLock = new Object();

    // Service link
    private ICrossMountAdapter mService;

    private final Context mContext;

    private final ArrayList<CallBack> mCallerCallbacks = new ArrayList<CallBack>();


    public interface CallBack {

        public void onFeatureStateUpdate(CrossMountAdapter.FeatureState featureState);

        public void onDiscoverResultUpdate(int reason);

        public void onConnectionResultUpdate();

        public void onTrustDeviceUpdated();

        public void onMyProviderServicesUpdate();

        public void onPlugInServiceIconReady();
    }

    /**
     * Helpers to get the default CrossMount adapter.
     */
    public static CrossMountAdapter getDefaultAdapter(
        Context context, CrossMountAdapter.CallBack listener) {

        Log.d(TAG, "getDefaultAdapter(), listener: " + listener);

        if (context == null) {
            Log.d(TAG, "context is null!");
            throw new IllegalArgumentException("context cannot be null");
        }

        context = context.getApplicationContext();
        if (context == null) {
            Log.d(TAG, "application context is null!");
            throw new IllegalArgumentException(
                "context not associated with any application (using a mock context?)");
        }

        synchronized (sLock) {
            if (sDefaultAdapter == null) {
                IBinder binder = ServiceManager.getService(CROSSMOUNT_MANAGER_SERVICE);
                if (binder == null){
                    Log.d(TAG, "binder is null!");
                    return null;
                }

                ICrossMountAdapter service = ICrossMountAdapter.Stub.asInterface(binder);
                if (service == null){
                    Log.d(TAG, "service is null!");
                    return null;
                }

                sDefaultAdapter = new CrossMountAdapter(context);
                sDefaultAdapter.mService = service;
            }
        }

        sDefaultAdapter.setCallBack(listener);
        //Log.d(TAG, "getDefaultAdapter() return, sDefaultAdapter:" + sDefaultAdapter);

        return sDefaultAdapter;
    }

    CrossMountAdapter(Context context) {
        Log.d(TAG, "CrossMountAdapter constructor");
        mContext = context;
    }

    public boolean setCallBack(CrossMountAdapter.CallBack listener){
        Log.d(TAG, "setCallBack(), listener: " + listener);

        if (listener != null && !mCallerCallbacks.contains(listener)){
            mCallerCallbacks.add(listener);

            int size = mCallerCallbacks.size();
            Log.d(TAG, "mCallerCallbacks, size:" + size);

            if (size == 1) {
                // Register service callback
                try {
                    mService.setCrossMountCallback(sDefaultAdapter);
                } catch (RemoteException e) {
                    Log.e(TAG, "RemoteException()!");
                }
            }
            return true;
        }

        if (listener == null) {
            Log.d(TAG, "listener is null!");
        } else {
            Log.d(TAG, "listener is the same!");
        }
        return false;
    }

    public boolean releaseCallBack(CrossMountAdapter.CallBack listener){
        Log.d(TAG, "releaseCallBack(), listener: " + listener);
        if (mCallerCallbacks.contains(listener)){
            mCallerCallbacks.remove(listener);

            int size = mCallerCallbacks.size();
            Log.d(TAG, "mCallerCallbacks, size:" + size);

            if (size == 0) {
                // unRegister service callback
                try {
                    mService.releaseCrossMountCallback(sDefaultAdapter);
                } catch (RemoteException e) {
                    Log.e(TAG, "RemoteException()!");
                }
            }
            return true;
        }
        Log.d(TAG, "listener is not found !");
        return false;
    }


    public FeatureState getFeatureState() {
        Log.d(TAG, "getFeatureState()");
        try {
            return FeatureState.valueOf(mService.getFeatureState());
        } catch (RemoteException e) {
            Log.d(TAG, "remoteException!");
            return FeatureState.DISABLED;
        }
    }

    public void enable() {
        Log.d(TAG, "enable()");
        try {
            mService.enable();
        } catch (RemoteException e) {
            Log.e(TAG, "RemoteException");
        }
    }

    public void disable() {
        Log.d(TAG, "disable()");
        try {
            mService.disable();
        } catch (RemoteException e) {
            Log.e(TAG, "RemoteException");
        }
    }

    public boolean startDiscover(String[] serviceId) {
        Log.d(TAG, "startDiscover()");
        try {
            return mService.startDiscover(serviceId);

        } catch (RemoteException e) {
            return false;
        }
    }

    public void stopDiscover() {
        Log.d(TAG, "stopDiscover()");
        try {
            mService.stopDiscover();
        } catch (RemoteException e) {

        }
    }

    public boolean isDiscovering(){
        try {
            return mService.isDiscovering();
        } catch (RemoteException e) {
            return false;
        }

    }

    public Device getMyDevice(){
        Log.d(TAG, "getMyDevice()");
        try {
            String deviceId = mService.getMyDeviceId();
            String deviceName = mService.getDeviceName(deviceId);
            String deviceType = mService.getDeviceType(deviceId);

            return new Device(deviceId, deviceName, deviceType, this);
        } catch (RemoteException e) {

        }
        return null;

    }

    public Device[] getTrustDevices(){
        Log.d(TAG, "getTrustDevices()");
        try {
            String[] deviceIds = mService.getTrustDevices();
            if (deviceIds == null){
                Log.d(TAG, "deviceId is null!");
                return null;
            }

            Device[] devices = new Device[deviceIds.length];

            int i = 0;
            for (String deviceId : deviceIds) {

                String deviceName = mService.getTrustDeviceName(deviceId);
                String deviceType = mService.getTrustDeviceType(deviceId);

                devices[i] = new Device(deviceId, deviceName, deviceType, this);
                i ++;
            }
            return devices;
        } catch (RemoteException e) {
            return new Device[0];
        }
    }

    public Device[] getAvailableDevices(){
        Log.d(TAG, "getAvailableDevices()");
        try {
            String[] deviceIds = mService.getAvailableDevices();
            if (deviceIds == null){
                Log.d(TAG, "deviceId is null!");
                return null;
            }
            Device[] devices = new Device[deviceIds.length];

            int i = 0;
            for (String deviceId : deviceIds) {
                String deviceName = mService.getDeviceName(deviceId);
                String deviceType = mService.getDeviceType(deviceId);

                devices[i] = new Device(deviceId, deviceName, deviceType, this);
                i ++;
            }
            return devices;
        } catch (RemoteException e) {
            return new Device[0];
        }


    }

    public boolean sendControlMessage(String msg){
        try {
            return mService.sendControlMessage(msg);
        } catch (RemoteException e) {
            return false;
        }
    }

    public String getBuildInServiceName(BuildInServiceType type){
        try {
            int value = type.ordinal();
            return mService.getBuildInServiceName(value);
        } catch (RemoteException e) {
            return null;
        }
    }

    public String getBuildInServiceDisplayName(BuildInServiceType type){
        try {
            int value = type.ordinal();
            return mService.getBuildInServiceDisplayName(value);
        } catch (RemoteException e) {
            return null;
        }
    }


     /** Callback from CrossMount service, on binder thread */
    @Override
    public void onFeatureStateUpdate(String featureState) {
        Log.d(TAG, "onFeatureStateUpdate(), featureState:" + featureState);

        FeatureState state = FeatureState.valueOf(featureState);
        for (CallBack callback : mCallerCallbacks) {
            Log.d(TAG, "call caller's onFeatureStateUpdate()");
            callback.onFeatureStateUpdate(state);
        }
    }

    @Override
    public void onDiscoverResultUpdate(int reason) {
        Log.d(TAG, "onDiscoverResultUpdate():" + reason);

        for (CallBack callback : mCallerCallbacks) {
            Log.d(TAG, "call caller's onDiscoverResultUpdate()");
            callback.onDiscoverResultUpdate(reason);
        }
    }

    @Override
    public void onConnectionResultUpdate(){
        for (CallBack callback : mCallerCallbacks) {
            Log.d(TAG, "call caller's onConnectionResultUpdate()");
            callback.onConnectionResultUpdate();
        }
    }

    @Override
    public void onTrustDeviceUpdated() {
        Log.d(TAG, "onTrustDeviceUpdated()");
        for (CallBack callback : mCallerCallbacks) {
            Log.d(TAG, "call caller's onTrustDeviceUpdated()");
            callback.onTrustDeviceUpdated();
        }
    }

    @Override
    public void onMyProviderServicesUpdate(){
        Log.d(TAG, "onMyProviderServicesUpdate()");
        for (CallBack callback : mCallerCallbacks) {
            Log.d(TAG, "call caller's onMyProviderServicesUpdate()");
            callback.onMyProviderServicesUpdate();
        }
    }

    @Override
    public void onPlugInServiceIconReady(){
        Log.d(TAG, "onPlugInServiceIconReady()");
        for (CallBack callback : mCallerCallbacks) {
            Log.d(TAG, "call caller's onPlugInServiceIconReady()");
            callback.onPlugInServiceIconReady();
        }
    }


    // ******************************************************************
    // package private methods
    // ******************************************************************

    /* Device.java */
    String getDeviceName(String deviceId){
        try {
            return mService.getDeviceName(deviceId);
        } catch (RemoteException e) {
            return null;
        }
    }
    String getDeviceType(String deviceId){
        try {
            return mService.getDeviceType(deviceId);
        } catch (RemoteException e) {
            return null;
        }
    }
    boolean setDeviceName(String name){
        try {
            return mService.setDeviceName(name);
        } catch (RemoteException e) {
            return false;
        }
    }

    String[] getProviderServices(String deviceId){
        try {
            return mService.getProviderServices(deviceId);
        } catch (RemoteException e) {
            return new String[0];
        }
    }

    boolean removeFromTrust(String deviceId){
        try {
            return mService.removeFromTrust(deviceId);
        } catch (RemoteException e) {
            return false;
        }
    }

    int getConnectCount(String deviceId){
        try {
            return mService.getConnectCount(deviceId);
        } catch (RemoteException e) {
            return 0;
        }
    }

    /* Service.java */
    String getServiceName(String deviceId, String serviceId){
        try {
            return mService.getServiceName(deviceId, serviceId);
        } catch (RemoteException e) {
            return null;
        }

    }

    int getServiceSerialNumber(String deviceId, String serviceId){
        try {
            return mService.getServiceSerialNumber(deviceId, serviceId);
        } catch (RemoteException e) {
            return -1;
        }

    }

    String getServiceStatus(String deviceId, String serviceId, String sharedDeviceId){
        try {
            return mService.getServiceStatus(deviceId, serviceId, sharedDeviceId);
        } catch (RemoteException e) {
            return null;
        }
    }
    boolean setServiceAllow(String serviceId, boolean allow){
        try {
            return mService.setServiceAllow(serviceId, allow);
        } catch (RemoteException e) {
            return false;
        }
    }
    boolean getServiceAllow(String serviceId) {
        try {
            return mService.getServiceAllow(serviceId);
        } catch (RemoteException e) {
            return false;
        }
    }

    boolean isPlugInService(String serviceId, String deviceId) {
        try {
            return mService.isPlugInService(serviceId, deviceId);
        } catch (RemoteException e) {
            return false;
        }
    }

    Drawable getPlugInIcon(String serviceId, String deviceId) {
        try {
            Bitmap bitmap = mService.getPlugInServiceIcon(serviceId, deviceId);
            if (bitmap != null){
                return new BitmapDrawable(bitmap);
            } else {
                return null;
            }
        } catch (RemoteException e) {
            return null;
        }
    }

    String[] getSharedDevices(String serviceId){
        try {
            return mService.getSharedDevices(serviceId);
        } catch (RemoteException e) {
            return new String[0];
        }
    }

    boolean shareProviderService(String serviceId, String deviceId){
        try {
            return mService.shareProviderService(serviceId, deviceId);
        } catch (RemoteException e) {
            return false;
        }
    }

    boolean stopShareProviderService(String serviceId, String deviceId){
        try {
            return mService.stopShareProviderService(serviceId, deviceId);
        } catch (RemoteException e) {
            return false;
        }
    }

    boolean mountProviderService(String serviceId, String deviceId){
        try {
            return mService.mountProviderService(serviceId, deviceId);
        } catch (RemoteException e) {
            return false;
        }
    }

    boolean stopMountProviderService(String serviceId, String deviceId){
        try {
            return mService.stopMountProviderService(serviceId, deviceId);
        } catch (RemoteException e) {
            return false;
        }
    }

    int getRemoteCameraId(String deviceId){
        try {
            return mService.getRemoteCameraId(deviceId);
        } catch (RemoteException e) {
            return -1;
        }
    }

}
