package com.mediatek.crossmountsettings.ui.mountfrom;

import android.app.AlertDialog;
import android.app.Dialog;
import android.content.DialogInterface;
import android.content.Intent;
import android.os.Bundle;
import android.preference.Preference;
import android.preference.PreferenceActivity;
import android.preference.PreferenceGroup;
import android.preference.PreferenceScreen;
import android.util.Log;
import android.view.Menu;
import android.widget.TextView;

import com.mediatek.crossmount.adapter.Device;
import com.mediatek.crossmount.adapter.Service;
import com.mediatek.crossmount.adapter.Service.ServiceState;

import com.mediatek.crossmountsettings.ui.sharedto.ServicePreference;
import com.mediatek.crossmountsettings.util.SettingsPreferenceFragment;
import com.mediatek.crossmountsettings.util.Utils;



import com.mediatek.crossmountsettings.R;

/**
 * Activity to service sharing of this self device.
 */
public class MountFromActivity extends PreferenceActivity {

    static final String TAG = "MountFromActivity";
    private String mDeviceName;
    private static String mDeviceId;

    @Override
    public Intent getIntent() {
        Intent modIntent = new Intent(super.getIntent());
        modIntent.putExtra(EXTRA_SHOW_FRAGMENT, MountfromFragment.class.getName());
        modIntent.putExtra(EXTRA_NO_HEADERS, true);
        return modIntent;
    }

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        mDeviceName = getIntent().getStringExtra(Utils.DEVICE_NAME);
        mDeviceId = getIntent().getStringExtra(Utils.DEVICE_ID);
        Log.d(TAG,"onCreate(), from intent " + mDeviceName + "," + mDeviceId);
        showBreadCrumbs(mDeviceName, mDeviceName);
    }

    /**
     * Mount from fragment.
     */
    public static class MountfromFragment extends SettingsPreferenceFragment {
        private Service mSelectedService;
        private String mDisConnectDlgTitle;
        private static final int MENUID_DISCONNECT = Menu.FIRST;
        private static final int DISCONNECT_MOUNTED_DLG = 0;

        @Override
        public void onCreate(Bundle icicle) {
            super.onCreate(icicle);
            addPreferencesFromResource(R.xml.crossmount_settings);
        }

        @Override
        public void onResume() {
            super.onResume();
            refreshUI();
        }

        @Override
        public boolean onPreferenceTreeClick(PreferenceScreen preferenceScreen,
                Preference preference) {
            if (preference instanceof ServicePreference) {
            	ServicePreference serPref = (ServicePreference) preference;
                onClicked(serPref);
                return true;
            }
        	 return super.onPreferenceTreeClick(preferenceScreen, preference);
        }

        /**
         * refresh UI.
         */
		private void refreshUI() {
            super.createDescriptionCategory();
            addDeviceCategory(mDescCategory, R.string.mount_service_category_desc);
		}

        @Override
        public void addDeviceCategory(PreferenceGroup preferenceGroup, int titleId) {
            super.addDeviceCategory(preferenceGroup, titleId);
            // add the services of this self device
	        Device[]  availableDevices =  mCrossAdapter.getAvailableDevices();
            Device device = Utils.getMountFromDevice(availableDevices, mDeviceId);
            Log.d(TAG, "addDeviceCategory(),mount from device " + mDeviceId);
            if (device == null) {
                Log.e(TAG, "error , device is null");
                return;
            }
	        Service[] serviceList = device.getProviderServices();
	        for (Service ser : serviceList) {
                ServicePreference pref = new ServicePreference(getActivity(), mCrossAdapter,
                        ser, Utils.MOUNT_FROM_ID);
                ServiceState stat = pref.getService().getState(null);
                Log.d(TAG, "pref enabled stat? " + stat);
                pref.setEnabled(ServiceState.OCCUPIED == stat ? false : true);
	        	preferenceGroup.addPreference(pref);
	        }
	    }

        /**
         * response the Service preference's click.
         * @param prf ServicePreference
         */
	    private void onClicked(ServicePreference prf) {
	    	mSelectedService = prf.getService();
            ServiceState stat = mSelectedService.getState(null);
            mDisConnectDlgTitle = prf.getServiceDisplayName();
			Log.d(TAG, "onClicked() " + mDisConnectDlgTitle + ",stat = " + stat);
            if (ServiceState.CONNECTED == stat || ServiceState.CONNECTING == stat) {
            	showDialog(DISCONNECT_MOUNTED_DLG);
            } else {
            	mSelectedService.mountFrom();
            }
	    }

	    @Override
	    public Dialog onCreateDialog(int dialogId) {
	        switch (dialogId) {
            case DISCONNECT_MOUNTED_DLG:
                return new AlertDialog.Builder(getActivity())
                            .setTitle(mDisConnectDlgTitle)
                            .setMessage(R.string.disconnect_msg)
                            .setPositiveButton(android.R.string.yes,
                                    new DialogInterface.OnClickListener() {
                                @Override
                                public void onClick(DialogInterface dialog, int id) {
                                	Log.d(TAG,"disconnect, stop mount from");
                                	mSelectedService.stopMountFrom();
                                }
                            })
                            .setNegativeButton(android.R.string.no, null)
                            .create();
             default:
                 break;
        }
        return super.onCreateDialog(dialogId);
	    }


        @Override
        public void connectUpdate() {
          super.connectUpdate();
          refreshUI();
        }
        @Override
        public void discoverComplete() {
          super.discoverComplete();
          refreshUI();
        }
        @Override
        public void pluginServiceIconReady() {
            super.pluginServiceIconReady();
            refreshUI();
        }
    }
}
