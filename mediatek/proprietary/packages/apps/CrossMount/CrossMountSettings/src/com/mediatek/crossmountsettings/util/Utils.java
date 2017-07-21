package com.mediatek.crossmountsettings.util;

import android.content.ActivityNotFoundException;
import android.content.Context;
import android.content.Intent;
import android.content.res.Resources;
import android.util.Log;

import com.mediatek.crossmount.adapter.CrossMountAdapter;
import com.mediatek.crossmount.adapter.CrossMountAdapter.BuildInServiceType;
import com.mediatek.crossmount.adapter.Device;
import com.mediatek.crossmount.adapter.Device.Type;
import com.mediatek.crossmount.adapter.Service;
import com.mediatek.crossmount.adapter.Service.ServiceState;
import com.mediatek.crossmountsettings.R;

import java.text.Collator;
import java.util.ArrayList;
import java.util.Collections;
import java.util.Comparator;
import java.util.List;


public final class Utils {
    private static final String TAG = "CrossSettingsUtil";

    public static final String SERVICE_ACTION = "com.mediatek.crossmount.SERVICE_SHARE";

    public static final String SHARE_TO_ACTION = "com.mediatek.crossmount.SHARE_TO_OTHER";
    public static final String MOUNT_FROM_ACTION = "com.mediatek.crossmount.MOUNT_FROM_OTHER";
    
    public static final String TRUST_LIST_ACTION = "com.mediatek.crossmount.TRUST_LIST";
    
    public static final String TOUCH_SHARED_TO_TV = "com.mediatek.crossmount.TOUCH_SHARED_TO_TV";
    public static final String TOUCH_EXIT_FROM_TV = "com.mediatek.crossmount.TOUCH_EXIT_FROM_TV";
    
    public static final String LOCALE_DEVICE_RENAME_ACTION =
        "com.mediatek.crossmount.LOCALE_DEVICE_RENAME";

    public static final String SERVICE_NAME = "service_name";
    public static final String SERVICE_DISPLAY_NAME = "service_display_name";
    public static final String DEVICE_NAME = "device_name";
    public static final String DEVICE_ID = "device_id";
    
    public static final int SHARED_TO_ID = 0;
    public static final int MOUNT_FROM_ID = 1;

    /**deal with the available devices that service's is not null.
     * @param adapter CrossMount Adapter
     * @param devList Device List
     * */
    public static void sortAvailDevices(CrossMountAdapter adapter, ArrayList devList) {
        devList.clear();
        Device[] devices = adapter.getAvailableDevices();
        if (devices == null) {
			Log.d(TAG, "availableDevices is null");
			return;
		}
		for (Device dev : devices) {
			// only the sevices of this device is not null
			if (dev.getProviderServices() != null) {
                Log.d(TAG, "device = " + dev.getName()
                		+ " 's service is not null");
                devList.add(dev);
			}
		}
        Collections.sort(devList, DEVICE_COMPARATOR);
	}

    /**
     * get the shared to service.
     * @param device Device
     * @param  serName service name
     * @return the selected service to share to other device
     */
    public static Service getSharedToService(Device device, String serName) {
        Log.d(TAG, "device = " + device.getName() + ", serName = " + serName);
		if (device == null || serName == null) {
			return null;
		}
		Service[] serviceList = device.getProviderServices();
		if (serviceList == null) {
			Log.d(TAG, "getProviderServices is null");
			return null;
		}
		for (Service ser : serviceList) {
			// the service name is unique?
            if (serName.equals(ser.getName())) {
				Log.d(TAG, "found service");
				return ser;
			}
		}
		return null;
	}

    /**
     * get the device which will mount from.
     * @param availableDevices all the available devices
     * @param devId device ID which to match
     * @return the device which this self device will mount from
     */
    public static Device getMountFromDevice(Device[] availableDevices, String devId) {
		if (availableDevices == null || devId == null) {
			return null;
        }
        for (Device dev : availableDevices) {
            if (devId.equals(dev.getId())) {
                Log.d(TAG, "found device " + devId);
				return dev;
			}
		}
	   return null;
	}

/**
 * Get the service state description.
 * @param stat ServiceState
 * @param res Resources
 * @return the original state
 */
    public static String stateConvertToSummay(ServiceState stat, Resources res) {
        if (ServiceState.OCCUPIED == stat) {
            return res.getString(R.string.service_state_occupied);
        } else if (ServiceState.AVAILABLE == stat) {
            return res.getString(R.string.service_state_available);
        } else if (ServiceState.DIABLED == stat) {
            return res.getString(R.string.service_state_disable);
        } else if (ServiceState.ENABLED == stat) {
            return res.getString(R.string.service_state_enable);
        } else if (ServiceState.CONNECTING == stat) {
            return res.getString(R.string.service_state_connecting);
        } else if (ServiceState.CONNECTED ==  stat) {
            return res.getString(R.string.service_state_connected);
        } else {
            return null;
        }
    }

    /**
     * Get the device type drawable.
     * @param dev Deivce
     * @return the drawable of device
     */
    public static int getCrossmountClassDrawable(Device dev) {
        final Device device = dev;
        Type type = device.getType();
        switch (type) {
        case PHONE:
            return R.drawable.ic_device_type_phone;
        case TV:
            return R.drawable.ic_device_type_tv;
        default:
            return R.drawable.ic_device_type_common;
        }
    }

    /**
     * sort by connection count , then by alpha , such as ,A.B.C.D....
     */
    private static final Comparator<Device> DEVICE_COMPARATOR = new Comparator<Device>() {
		private final Collator mCollator = Collator.getInstance();
		@Override
		public int compare(Device dev1, Device dev2) {
            int count1 = dev1.getConnectCount();
            int count2 = dev2.getConnectCount();
            Log.d(TAG, "count1 = " + count1 + ",count2 = " + count2);
            if (count1 > count2) {
                return -1;
            } else if (count1 < count2) {
                return 1;
            } else {
                String devname1 = dev1.getName();
                String devname2 = dev2.getName();
                Log.d(TAG, "devname1 " + devname1 + " devname2 " + devname2);
                return mCollator.compare(devname1, devname2);
            }
		}
	};

	/**
	 * Judge it is Touch shared to TV or not.
	 *  @param adapter CrossMountAdapter
	 * @param ser Service
	 * @return currently is the status that Touch shared to TV
	 */
    public static boolean isController(CrossMountAdapter adapter, Service ser) {
		String serName = ser.getName();
		return serName.equals(adapter.
                getBuildInServiceName(BuildInServiceType.CONTROLLER));
	}

	/**
	 * Launch activity.
	 * @param context  Context
	 * @param action String , the intent's action
	 * @param deviceName String
	 * @param deviceId String
	 * @param serName String
     * @param serDisName String, used to show service name in UI, support MUI
	 */
    public static void launchActivity(Context context, String action, String deviceName,
            String deviceId, String serName, String serDisName) {
            try {
                Intent intent = new Intent();
                intent.setAction(action);
                intent.putExtra(DEVICE_NAME, deviceName);
                intent.putExtra(DEVICE_ID, deviceId);
                intent.putExtra(SERVICE_NAME, serName);
                intent.putExtra(SERVICE_DISPLAY_NAME, serDisName);
                context.startActivity(intent);
            } catch (ActivityNotFoundException e) {
                Log.e(TAG, "fail to start activity");
            }
    }
}
