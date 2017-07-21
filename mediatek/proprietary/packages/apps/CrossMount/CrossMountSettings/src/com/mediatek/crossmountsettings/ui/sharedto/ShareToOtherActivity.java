package com.mediatek.crossmountsettings.ui.sharedto;

import android.app.AlertDialog;
import android.app.Dialog;
import android.content.DialogInterface;
import android.content.Intent;
import android.os.Bundle;
import android.preference.CheckBoxPreference;
import android.preference.Preference;
import android.preference.PreferenceActivity;
import android.preference.PreferenceGroup;
import android.preference.PreferenceScreen;
import android.util.Log;
import android.view.Menu;

import com.mediatek.crossmount.adapter.Device;
import com.mediatek.crossmount.adapter.Service;

import com.mediatek.crossmountsettings.R;
import com.mediatek.crossmountsettings.ui.CrossmountDevicePreference;
import com.mediatek.crossmountsettings.util.SettingsPreferenceFragment;
import com.mediatek.crossmountsettings.util.Utils;

/**
 * Activity to service sharing of this self device.
 */
public class ShareToOtherActivity extends PreferenceActivity {

    private static final String TAG = "ShareToOtherActivity";
    private static String sServiceName;
    private static String sServiceDisplayName;
    @Override
    public Intent getIntent() {
        Intent modIntent = new Intent(super.getIntent());
        modIntent.putExtra(EXTRA_SHOW_FRAGMENT, ShareToFragment.class.getName());
        modIntent.putExtra(EXTRA_NO_HEADERS, true);
        return modIntent;
    }

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        sServiceName = getIntent().getStringExtra(Utils.SERVICE_NAME);
        Log.d(TAG, "get service name from intent = " + sServiceName);
        sServiceDisplayName =  getIntent().getStringExtra(Utils.SERVICE_DISPLAY_NAME);
        showBreadCrumbs(sServiceDisplayName, sServiceDisplayName);
    }

    /**
     * UI for share to.
     */
    public static class ShareToFragment extends SettingsPreferenceFragment {
        private PreferenceScreen mPrefScreen;

        private Service mService;
        private String mSelectedDevId;
        private String mSelectedDevName;
        private CheckBoxPreference mAllowPreference;
        private boolean mAllowed;

        private static final int MENUID_DISCONNECT = Menu.FIRST;
        private static final int DISCONNECT_SHARED_DLG = 0;
        @Override
        public void onCreate(Bundle icicle) {
            super.onCreate(icicle);
            addPreferencesFromResource(R.xml.crossmount_settings);
            mPrefScreen = getPreferenceScreen();
        }

        @Override
        public void onResume() {
            super.onResume();
            mService = Utils.getSharedToService(mCrossAdapter.getMyDevice(), sServiceName);
            if (mService == null) {
                Log.e(TAG, "cannot find sevice,finish itself");
                getActivity().finish();
                return;
            }
            mAllowed = mService.getAllow();
            Log.d(TAG, "onResume ,service allowed = " + mAllowed);
            refreshUI(mAllowed);
        }

        @Override
        public boolean onPreferenceTreeClick(PreferenceScreen preferenceScreen,
                Preference preference) {
            if (preference instanceof CrossmountDevicePreference) {
                CrossmountDevicePreference devPref = (CrossmountDevicePreference) preference;
                onClicked(devPref);
                return true;
            } else if (preference instanceof CheckBoxPreference) {
                boolean isAllowed = ((CheckBoxPreference) preference).isChecked();
                Log.d(TAG, "CheckboxPreference, isAllowed = " + isAllowed);
                mService.setAllow(isAllowed);
                addDeviceForShareTo(isAllowed);
                return true;
            }
             return super.onPreferenceTreeClick(preferenceScreen, preference);
        }

        /**
         * response this preference's click.
         * @param pref This preference
         */
        private void onClicked(CrossmountDevicePreference pref) {
            Device dev = pref.getDevice();
            mSelectedDevId = dev.getId();
            mSelectedDevName = dev.getName();

            Service.ServiceState stat = mService.getState(mSelectedDevId);
            Log.d(TAG, "onClicked(), " + mSelectedDevId + ", " + mSelectedDevName + ", " + stat);
            if (Service.ServiceState.CONNECTED == stat
                    || Service.ServiceState.CONNECTING == stat) {
                showDialog(DISCONNECT_SHARED_DLG);
            } else {
                boolean sharedResult = mService.shareTo(mSelectedDevId);
                Log.d(TAG, "sharedResult = " + sharedResult);
            }
        }

        @Override
        public Dialog onCreateDialog(int dialogId) {
            switch (dialogId) {
            case DISCONNECT_SHARED_DLG:
                return new AlertDialog.Builder(getActivity())
                            .setTitle(sServiceDisplayName)
                            .setMessage(R.string.disconnect_msg)
                            .setPositiveButton(android.R.string.yes,
                                    new DialogInterface.OnClickListener() {
                                @Override
                                public void onClick(DialogInterface dialog, int id) {
                                    Log.d(TAG, "disconnect menu,call stopShareTo");
                                    mService.stopShareTo(mSelectedDevId);
                                }
                            })
                            .setNegativeButton(android.R.string.no, null)
                            .create();
               default:
                   break;
        }
        return super.onCreateDialog(dialogId);
        }

        /**
         * refresh UI.
         * @param allow the Checkbox's state
         */
        private void refreshUI(boolean allow) {
            addAllowCheckBox(allow);
            addDeviceForShareTo(allow);
        }

        /**
         * add the Checkbox to control the allow or not.
         * @param allow the Checkbox's state
         */
        private void addAllowCheckBox(boolean allow) {
            Log.d(TAG, "addAllowCheckBox , checked = " + allow);
            // clear UI fisrtly
            if (mPrefScreen != null) {  mPrefScreen.removeAll(); }
            // add the allow preference
            if (mAllowPreference == null) {
                mAllowPreference = new CheckBoxPreference(getActivity());
            }
            mAllowPreference.setTitle(getActivity().getResources()
                    .getString(R.string.share_to_allow_pref));
            mAllowPreference.setChecked(allow);
            mPrefScreen.addPreference(mAllowPreference);
        }

        /**
         *  add device stands for shared object.
         * @param allowed the Checkbox's state
         */
        private void addDeviceForShareTo(boolean allowed) {
            if (allowed) {
                super.createDescriptionCategory();
                addDeviceCategory(mDescCategory, R.string.share_to_category_desc);
            } else {
                if (mPrefScreen != null && mDescCategory != null) {
                    mPrefScreen.removePreference(mDescCategory);
                }
            }
        }

        @Override
        public void addDeviceCategory(PreferenceGroup preferenceGroup, int titleId) {
            super.addDeviceCategory(preferenceGroup, titleId);
            // add all the available preferences
            Device[]  availableDevices =  mCrossAdapter.getAvailableDevices();
            if (availableDevices == null) {
                Log.e(TAG, "getAvailableDevices is null");
                return;
            }
            for (Device dev : availableDevices) {
            	if (Utils.isController(mCrossAdapter, mService) 
            			&& Device.Type.PHONE == dev.getType()) {
            		Log.d(TAG,"controller PHONE, not add");
            		continue;
            	}
                CrossmountDevicePreference pref = new CrossmountDevicePreference(getActivity(),
                        mCrossAdapter, dev, mService, Utils.SHARED_TO_ID);
                preferenceGroup.addPreference(pref);
            }
        }

        @Override
        public void discoverComplete() {
            super.discoverComplete();
            mAllowed = mService.getAllow();
            Log.d(TAG, "MSG_DISCOVER_COMPLETE,service allowed ? "
                    + mAllowed);
            refreshUI(mAllowed);
        }

          @Override
          public void connectUpdate() {
              super.connectUpdate();
              mAllowed = mService.getAllow();
              refreshUI(mAllowed);
          }
     }
}
