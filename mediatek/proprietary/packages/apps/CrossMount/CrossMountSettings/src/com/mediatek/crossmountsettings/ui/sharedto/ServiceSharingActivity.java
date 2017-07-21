package com.mediatek.crossmountsettings.ui.sharedto;


import android.content.Intent;
import android.os.Bundle;
import android.preference.Preference;
import android.preference.PreferenceActivity;
import android.preference.PreferenceGroup;
import android.preference.PreferenceScreen;
import android.util.Log;

import com.mediatek.crossmount.adapter.Device;
import com.mediatek.crossmount.adapter.Service;

import com.mediatek.crossmountsettings.R;
import com.mediatek.crossmountsettings.util.SettingsPreferenceFragment;
import com.mediatek.crossmountsettings.util.Utils;


/**
 * Activity to service sharing of this self device.
 */
public class ServiceSharingActivity extends PreferenceActivity {

    static final String TAG = "ServiceSharingActivity";

    @Override
    public Intent getIntent() {
        Intent modIntent = new Intent(super.getIntent());
        modIntent.putExtra(EXTRA_SHOW_FRAGMENT, ServiceFragment.class.getName());
        modIntent.putExtra(EXTRA_NO_HEADERS, true);
        return modIntent;
    }

    /**
     * UI for service sharing to other.
     */
    public static class ServiceFragment extends SettingsPreferenceFragment {
        private Preference mMyServicePreference;

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
                Log.d(TAG, "onPreferenceTreeClick");
                serPref.onClicked();
                return true;
            }
             return super.onPreferenceTreeClick(preferenceScreen, preference);
        }

        /**
         * refresh UI.
         */
        private void refreshUI() {
            // clear UI fisrtly
            PreferenceScreen preferenceScreen = getPreferenceScreen();
            preferenceScreen.removeAll();
            // add my service of this device description
            if (mMyServicePreference == null) {
                mMyServicePreference = new Preference(getActivity());
            }
            mMyServicePreference.setSummary(getActivity().getResources()
                    .getString(R.string.service_sharing_message));
            mMyServicePreference.setSelectable(false);
            preferenceScreen.addPreference(mMyServicePreference);
            // add category
            super.createDescriptionCategory();
            addDeviceCategory(mDescCategory, R.string.service_category_desc);
        }

        @Override
        public void addDeviceCategory(PreferenceGroup preferenceGroup, int titleId) {
            super.addDeviceCategory(preferenceGroup, titleId);
            // add the services of this self device
            Device selfDevice = mCrossAdapter.getMyDevice();
            Service[] serviceList = selfDevice.getProviderServices();
            if (serviceList == null) {
               Log.e(TAG, "getProviderServices is null");
               return;
            }

            for (Service ser : serviceList) {
                ServicePreference pref = new ServicePreference(getActivity(), mCrossAdapter,
                        ser, Utils.SHARED_TO_ID);
                preferenceGroup.addPreference(pref);
            }
        }

        @Override
        public void connectUpdate() {
            super.connectUpdate();
            refreshUI();
        }

        @Override
        public void provideServiceUpdate() {
            super.provideServiceUpdate();
            refreshUI();
        }

        @Override
        public void pluginServiceIconReady() {
            super.pluginServiceIconReady();
            refreshUI();
        }
    }
}
