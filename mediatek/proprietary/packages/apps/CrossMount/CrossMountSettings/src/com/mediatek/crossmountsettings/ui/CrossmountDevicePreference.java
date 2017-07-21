package com.mediatek.crossmountsettings.ui;

import android.content.Context;
import android.content.res.Resources;
import android.preference.Preference;
import android.util.Log;

import com.mediatek.crossmount.adapter.CrossMountAdapter;
import com.mediatek.crossmount.adapter.CrossMountAdapter.BuildInServiceType;
import com.mediatek.crossmount.adapter.Device;
import com.mediatek.crossmount.adapter.Service;
import com.mediatek.crossmountsettings.R;
import com.mediatek.crossmountsettings.util.Utils;


/**
 * CrossmountDevicePreference is the preference type used to display each remote
 * Crossmount device in the CrossMount Settings screen.
 */
public final class CrossmountDevicePreference extends Preference {
    private static final String TAG = "CrossmountDevPrf";
    private Context mContext;
    private CrossMountAdapter mAdapter;
    private Resources mRes;

    private final Device mDevice;
    private final Service mSharedService;

    private int mDriect;
    private final String endStr =  "/";

    /**
     * Constructor.
     * @param context Context
     * @param adapter CrossMountAdapter
     * @param dev Device
     * @param service Service
     * @param direct shared to or mount from
     **/
    public CrossmountDevicePreference(Context context, CrossMountAdapter adapter,
    		Device dev, Service service, int direct) {
        super(context);
        mContext = context;
        mAdapter = adapter;
        mDevice = dev;
        mSharedService = service;
        mDriect = direct;
        mRes = getContext().getResources();
        setLayoutResource(R.layout.preference_crossmount_icon);
        onDeviceAttributesChanged();
    }

    /**
     * get the Device object.
     * @return Device  this preference stands for
     */
   public Device getDevice() {
       return mDevice;
   }

   /**
    * response the preference's click.
    * */
   public void onClicked() {
	   String name = mDevice.getName();
	   String id = mDevice.getId();
       if (Utils.MOUNT_FROM_ID == mDriect) {
            Log.d(TAG, "start mountfrom device " + name + " , " + id);
            Utils.launchActivity(getContext(), Utils.MOUNT_FROM_ACTION,
                    name, id, null, null);
	   }
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
        setTitle(mDevice.getName());
        String summary = null;
        if (Utils.SHARED_TO_ID == mDriect) {
            summary = getSharedToServiceState();
            setSummary(summary);
        } else if (Utils.MOUNT_FROM_ID == mDriect) {
            summary = getMountedServiceState();
            if (summary != null) {
                setSummary(getContext().getResources().getString(
                        R.string.service_is_mounted, summary));
            } else {
                setSummary(null);
            }
        } else {
           Log.d(TAG, "nothing");
        }

        setIcon(Utils.getCrossmountClassDrawable(mDevice));
        // This could affect ordering, so notify that
        notifyHierarchyChanged();
    }

    /**
     * get all the mounted service's summary.
     * @return service state string
     */
    private String getMountedServiceState() {
    	StringBuilder name = new StringBuilder();
        final Device device = mDevice;
        Service[] serviceList = device.getProviderServices();
        for (Service ser: serviceList) {
            Service.ServiceState stat = ser.getState(null);
            Log.d(TAG, "mounted state is = " + stat);
            if (Service.ServiceState.CONNECTED == stat) {
                String serviceName = getServiceDisplayName(ser.getName());
                Log.d(TAG, "mounted serviceName of connected = " + serviceName);
        		name.append(serviceName);
        		name.append(endStr);
        	}
        }
        if (name != null && name.toString().endsWith(endStr)) {
            String endName = name.substring(0, name.length() - 1);
            Log.d(TAG, "mount service endName = " + endName);
        	return endName;
        }
        Log.d(TAG, "mount service name is null");
        return null;
    }

    /**
     * get all the shared service's summary.
     * @return service state string
     */
    private String getSharedToServiceState() {
        StringBuilder name = new StringBuilder();
        Device[] sharedDevice = mSharedService.getSharedDevices();
        if (sharedDevice == null || sharedDevice.length == 0) {
            Log.d(TAG, "sharedDevice is null");
            return null;
        }

        final Device device = mDevice;
        String id = device.getId();
        // param is the other device's id
        Service.ServiceState stat = mSharedService.getState(id);
        String summary = Utils.stateConvertToSummay(stat, mRes);
        Log.d(TAG, "service " + mSharedService.getName()
                + " sharedTo device" + id
                + " state is " + summary);
        return summary;
    }

    /**
     * get the local service display name.
     * As the name it used to show UI, so call
     * getBuildInServiceDisplayName() which support MUI.
     * @param serviceName String
     * @return String service display name.
     */
    public String getServiceDisplayName(String serviceName) {
        int size = mLocalSerInfos.length;
        for (int i = 0; i < size; i++) {
            if (serviceName.equals(mAdapter.getBuildInServiceName(
                    mLocalSerInfos[i].getType()))) {
                      String displayName = mAdapter.getBuildInServiceDisplayName(
                              mLocalSerInfos[i].getType());
                      return displayName;
                    }
        }
        return serviceName;
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
