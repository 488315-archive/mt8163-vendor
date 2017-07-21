package com.mediatek.crossmountsettings.ui.trustlist;

import android.content.Intent;
import android.os.Bundle;
import android.preference.Preference;
import android.preference.PreferenceActivity;
import android.preference.PreferenceGroup;
import android.preference.PreferenceScreen;
import android.util.Log;

import com.mediatek.crossmount.adapter.CrossMountAdapter;
import com.mediatek.crossmount.adapter.Device;
import com.mediatek.crossmountsettings.util.SettingsPreferenceFragment;
import com.mediatek.crossmountsettings.R;

/**
 * Activity to service sharing of this self device.
 */
public class TrustListActivity extends PreferenceActivity {

    static final String TAG = "TrustListActivity";

    @Override
    public Intent getIntent() {
        Intent modIntent = new Intent(super.getIntent());
        modIntent.putExtra(EXTRA_SHOW_FRAGMENT, TrustlistFragment.class.getName());
        modIntent.putExtra(EXTRA_NO_HEADERS, true);
        return modIntent;
    }

    public static class TrustlistFragment extends SettingsPreferenceFragment {
        private Preference mTrustPreference;
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
        public void onDestroy() {
            Log.d(TAG, "onDestroy()");
            removeNonTrust(mCrossAdapter);
            super.onDestroy();
        }

        /**
         * refresh UI.
         */
		private void refreshUI() {
			// clear UI fisrtly
	        PreferenceScreen preferenceScreen = getPreferenceScreen();
	        preferenceScreen.removeAll();
	        // add this feature description
            if (mTrustPreference == null) {
                mTrustPreference = new Preference(getActivity());
            }
            mTrustPreference.setSummary(getResources().getString(
                        R.string.trust_list_manage_message_desc));
            mTrustPreference.setSelectable(false);
            preferenceScreen.addPreference(mTrustPreference);
            // Available service category
            super.createDescriptionCategory();
            addDeviceCategory(mDescCategory, R.string.trust_list_category_desc);
		}

        @Override
        public void addDeviceCategory(PreferenceGroup preferenceGroup, int titleId) {
            super.addDeviceCategory(preferenceGroup, titleId);
            Device[]  trustList =  mCrossAdapter.getTrustDevices();
            if (trustList == null || trustList.length == 0) {
                Log.d(TAG, "getTrustDevices() is null");
                return;
            }
            for (Device dev : trustList) {
                TrustDevicePreference pref = new TrustDevicePreference(getActivity(), dev);
                String id = dev.getId();
                Log.d(TAG, "addPrf, device name " + dev.getName() + "," + id);
                pref.setKey(id);
	        	preferenceGroup.addPreference(pref);
	        }
	    }

        /**
         * Remove the non-trust device from list.
         * @param adapter CrossMountAdapter
         */
        private void removeNonTrust(CrossMountAdapter adapter) {
            if (adapter == null) {
                Log.d(TAG, "removeNonTrust adapter is null");
                return;
            }
            Device[] trustList = adapter.getTrustDevices();
            if (trustList == null || trustList.length == 0) {
                Log.d(TAG, "removeNonTrust is null");
                return;
            }
            for (Device dev : trustList) {
                if (mDescCategory != null) {
                    String id = dev.getId();
                    Log.d(TAG, "to findPrf, device name " + dev.getName() + ","
                            + id);
                    TrustDevicePreference trustPrf = (TrustDevicePreference) mDescCategory
                            .findPreference(id);
                    if (trustPrf != null && trustPrf.isRemoved()) {
                        Log.d(TAG, id + " is removeFromTrust");
                        dev.removeFromTrust();
                    }
                    if (trustPrf != null) {
                        Log.d(TAG, "clear sharedpreference data");
                        trustPrf.clearSharedPreference();
                    }
                }
            }
        }

        @Override
        public void refreshAsRename(boolean enabled) {
            super.refreshAsRename(enabled);
            refreshUI();
        }

        @Override
        public void trustUpdate() {
            super.trustUpdate();
            refreshUI();
        }
    }

}
