package com.mediatek.crossmount.adapter;

import android.content.Context;
import android.hardware.camera2.CaptureRequest;
import android.hardware.camera2.CaptureResult;
import android.hardware.camera2.impl.CameraMetadataNative;

import android.net.Uri;
import android.os.IBinder;
import android.os.RemoteException;
import android.os.ServiceManager;
import android.util.Log;


/**
 * Represents the local camera adapter.
 * <p>
 * Use the helper {@link #getDefaultAdapter(Context)} to get the default HotKnot
 * adapter for the Android device.
 *
 */
public final class CrossMountCameraAdapter extends ICrossMountCameraCallback.Stub{
    static final String TAG = "CrossMountCameraAdapter";

    static final String CROSSMOUNT_MANAGER_SERVICE = "CrossMountManagerService";

    public interface CallBack {
        public void onRepeatRequest(CaptureRequest.Builder builder);

        public void onNonRepeatRequest(CaptureRequest.Builder builder, int[] imageInfo);
    }
    CallBack mCallerCallback;

    // Locker
    private static Object sLock = new Object();

    // service link
    private ICrossMountAdapter mService;
    private final Context mContext;


    public CrossMountCameraAdapter(Context context, CrossMountCameraAdapter.CallBack listener) {
        Log.d(TAG, "CrossMountCameraAdapter constructor");

        //Save the application context
        mContext = context;

        IBinder binder = ServiceManager.getService(CROSSMOUNT_MANAGER_SERVICE);
        if (binder == null){
            Log.e(TAG, "binder is null !");
            return;
        }

        mService = ICrossMountAdapter.Stub.asInterface(binder);

        if (mService == null) {
            Log.e(TAG, "mService is null !");
            return;
        }

        // Register service callback
        try {
            mService.setCrossMountCameraCallback(this);
        } catch (RemoteException e) {
            Log.e(TAG, "RemoteExceptions():", e);
        }

        if (listener != null){
            Log.d(TAG, "save listener to mCallerCallback");
            mCallerCallback = listener;
        }
    }

    public boolean releaseCallBack(CrossMountCameraAdapter.CallBack listener){
        Log.d(TAG, "releaseCallBack()");
        if (mCallerCallback == listener){
            mCallerCallback = null;
            return true;
        } else {
            Log.d(TAG, "callback is wrong !!");
            return false;
        }
    }

    public boolean sendRepeatResult(CaptureResult result){
        try {
            return mService.sendCameraRepeatResult(result.getNativeCopy());
        } catch (RemoteException e) {
            Log.e(TAG, "RemoteException !");
            return false;
        }
    }

    public boolean sendNonRepeatResult(Uri uri, CaptureResult result){

        try {
            return mService.sendCameraNonRepeatResult(uri, result.getNativeCopy());
        } catch (RemoteException e) {
            Log.e(TAG, "RemoteException !");
            return false;
        }
    }



    /** Callback from CrossMount service, usually on binder thread */
    @Override
    public void onRepeatRequest(CameraMetadataNative metaNative) {
        Log.d(TAG, "onRepeatRequest()");

        if (mCallerCallback != null) {

            CaptureRequest.Builder builder = new CaptureRequest.Builder(metaNative, false, 0);

            Log.d(TAG, "callback caller's onRepeatRequest()");
            mCallerCallback.onRepeatRequest(builder);
        }
    }

    @Override
    public void onNonRepeatRequest(CameraMetadataNative metaNative, int[] imageInfo) {
        Log.d(TAG, "onRepeatRequest()");

        if (mCallerCallback != null) {

            CaptureRequest.Builder builder = new CaptureRequest.Builder(metaNative, false, 0);

            Log.d(TAG, "callback caller's onNonRepeatRequest()");
            mCallerCallback.onNonRepeatRequest(builder, imageInfo);
        }
    }

    // ******************************************************************
    // package private methods
    // ******************************************************************




}
