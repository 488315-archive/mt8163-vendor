package com.mediatek.crossmountsettings.ui.sharedto;

import android.content.Context;
import android.content.res.Resources;
import android.graphics.drawable.Drawable;
import android.preference.Preference;
import android.util.Log;

import com.mediatek.crossmount.adapter.CrossMountAdapter;
import com.mediatek.crossmount.adapter.CrossMountAdapter.BuildInServiceType;
import com.mediatek.crossmount.adapter.Device;
import com.mediatek.crossmount.adapter.Service;
import com.mediatek.crossmount.adapter.Service.ServiceState;

import com.mediatek.crossmountsettings.R;
import com.mediatek.crossmountsettings.util.Utils;

/**
 * ServicePreference is the preference type used to show the service of device.
 */
public final class ServicePreference extends Preference {
    private static final String TAG = "ServicePreference";

    private final Service mService;

    private Resources mRes;
    private CrossMountAdapter mCrossAdapter;
    private int mDirect;

    /**
     * Constructor,A preference that stands for service information.
     * @param context Context
     * @param adapter CrossMountAdapter
     * @param service Service
     * @param direct shared to or mount from
     **/
    public ServicePreference(Context context, CrossMountAdapter adapter,
                             Service service, int direct) {
        super(context);
        mCrossAdapter = adapter;
        mDirect = direct;
        mService = service;
        mRes = getContext().getResources();
        setLayoutResource(R.layout.preference_crossmount_icon);
        onDeviceAttributesChanged();
    }

    /**
     * response the preference's click.
     * */
    public void onClicked() {
        if (Utils.SHARED_TO_ID == mDirect) {
            String name = mService.getName();
            Log.d(TAG, "share " + name + " to other device");
            Utils.launchActivity(getContext(), Utils.SHARE_TO_ACTION, null,
                    null, name, getServiceDisplayName());
        }
    }

    /**
     * get the Service object.
     * @return Service  this preference stands for
     */
    public Service getService() {
        return mService;
    }

    /**
     * update the attributes.
     */
    public void onDeviceAttributesChanged() {
        /*
         * The preference framework takes care of making sure the value has
         * changed before proceeding. It will also call notifyChanged() if
         * any preference info has changed from the previous value.
         */
        String name = getServiceDisplayName();
        Log.d(TAG, "service name = " + name);
        setTitle(name);
        String summary = null;
        if (Utils.MOUNT_FROM_ID == mDirect) {
            summary = getMountedServiceState();
        } else if (Utils.SHARED_TO_ID == mDirect) {
            summary = getSharedServiceState();
        }
        setSummary(summary);
        setIcon();

        // This could affect ordering, so notify that
        notifyHierarchyChanged();
    }

    /**
     * get the summary stands for mounted from service.
     * @return mounted service state
     */
    private String getMountedServiceState() {
        final Service ser = mService;
        ServiceState stat = ser.getState(null);
        Log.d(TAG, "mount state is = " + stat);
        String summary = Utils.stateConvertToSummay(stat, mRes);
        return summary;
    }

    /**
     * get the summary stands for shared to service state.
     * @return shared service state
     */
    private String getSharedServiceState() {
        String summary = null;
        final Service ser = mService;
        Device[] devList = ser.getSharedDevices();

        if (devList == null || devList.length == 0) {
            Log.d(TAG, "shared device is null");
            summary = getMountedServiceState();
            return summary;
        }

        StringBuilder tmp = new StringBuilder();
        String endStr =  "/";
        for (Device dev : devList) {
            String sharedId = dev.getId();
            Log.d(TAG, "shared to device Id = " + sharedId);
            ServiceState stat = ser.getState(sharedId);
            Log.d(TAG, "shared state = " + stat);
            if (ServiceState.CONNECTED == stat) {
                // get other device name of mounted this service
                String name = dev.getName();
                tmp.append(name);
                tmp.append(endStr);
            } else {
                summary = Utils.stateConvertToSummay(stat, mRes);
            }
        }

        if (tmp != null) {
            if (tmp.toString().endsWith(endStr)) {
                // remove the last "/"
                String endName = tmp.substring(0, tmp.length() - 1);
                Log.d(TAG, "endName = " + endName);
                // the summary like "Used by ***"
                summary = mRes.getString(R.string.device_mounted_service, endName);
            } else if (tmp.length() != 0) {
                Log.d(TAG, "tmp name is " + tmp);
                summary = mRes.getString(R.string.device_mounted_service, tmp);
            }
        }

        Log.d(TAG, "shared service summary = " + summary);
        return summary;
    }

    /**
     * set the icon of service.
     */
    private void setIcon() {
        final Service ser = mService;
        boolean isPlugin = ser.isPlugIn();
        Log.d(TAG, "isPlugin sevice? " + isPlugin);
        if (isPlugin) {
            Drawable icon = ser.getPlugInIcon();
            if (icon == null) {
                icon = mRes.getDrawable(R.drawable.ic_service_type_common);
                Log.d(TAG, "getPlugInIcon is null , return default");
            }
            setIcon(icon);
        } else {
            int iconResId = getLocalSerIcon();
            if (iconResId != 0) {
                setIcon(iconResId);
            }
        }
    }

    /**
     * get the local service icon.
     * @return icon
     */
    public int getLocalSerIcon() {
        int size = mLocalSerInfos.length;
         String name = mService.getName();
         for (int i = 0; i < size; i++) {
             if (name.equals(mCrossAdapter.getBuildInServiceName(
                     mLocalSerInfos[i].getType()))) {
                       return  mLocalSerInfos[i].getIconId();
                     }
         }
         return R.drawable.ic_service_type_common;
    }

    /**
     * get the local service display name.
     * As the name it used to show UI, so call
     * getBuildInServiceDisplayName() which support MUI.
     * @return String service display name.
     */
    public String getServiceDisplayName() {
        int size = mLocalSerInfos.length;
        String name = mService.getName();
        try {
            for (int i = 0; i < size; i++) {
                if (name.equals(mCrossAdapter.getBuildInServiceName(
                        mLocalSerInfos[i].getType()))) {
                    String displayName = mCrossAdapter.getBuildInServiceDisplayName(
                            mLocalSerInfos[i].getType());
                    return displayName;
                }
            }
        } catch (NullPointerException e) {
            Log.e(TAG, "NullPointerException name: " + name);
        }
        return name;
    }
    /**
     * A class stands for service info.
     * Such as type, drawable icon.
     *
     */
      class ServiceInfo {
            private BuildInServiceType mType;
            private int mIconId;
            public ServiceInfo(BuildInServiceType type, int iconId) {
                mType = type;
                mIconId = iconId;
            }
            public BuildInServiceType getType() {
                return mType;
            }
            public int getIconId() {
                return mIconId;
            }
        }

        private ServiceInfo[] mLocalSerInfos = {
                new ServiceInfo(BuildInServiceType.CAMERA,
                       R.drawable.ic_service_type_camera),
                new ServiceInfo(BuildInServiceType.SPEAKER,
                        R.drawable.ic_service_type_speaker),
                new ServiceInfo(BuildInServiceType.MICROPHONE,
                        R.drawable.ic_service_type_microphone),
                new ServiceInfo(BuildInServiceType.CONTROLLER,
                        R.drawable.ic_service_type_touch)
                };
}
