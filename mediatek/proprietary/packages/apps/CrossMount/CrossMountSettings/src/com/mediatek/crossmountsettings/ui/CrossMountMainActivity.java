
package com.mediatek.crossmountsettings.ui;

import android.content.Intent;
import android.os.Bundle;
import android.preference.Preference;
import android.preference.PreferenceActivity;
import android.preference.PreferenceGroup;
import android.preference.PreferenceScreen;
import android.util.Log;
import android.view.Menu;
import android.view.MenuInflater;
import android.view.MenuItem;
import android.widget.TextView;

import com.mediatek.crossmount.adapter.Device;
import com.mediatek.crossmountsettings.R;
import com.mediatek.crossmountsettings.util.SettingsPreferenceFragment;
import com.mediatek.crossmountsettings.util.Utils;

import java.util.ArrayList;
import java.util.List;

 /**
 * Activity to CrossMount Settings entrance.
 */
public class CrossMountMainActivity extends PreferenceActivity {

    private static final String TAG = "CrossMountMainActivity";

    @Override
    public Intent getIntent() {
        Intent modIntent = new Intent(super.getIntent());
        modIntent.putExtra(EXTRA_SHOW_FRAGMENT, MainFragment.class.getName());
        modIntent.putExtra(EXTRA_NO_HEADERS, true);
        return modIntent;
    }

/**
 * The main UI fragment.
 *
 */
    public static class MainFragment extends SettingsPreferenceFragment {
        private TextView mEmptyView;

        private MenuItem mEnableMenuItem;
        private MenuItem mRefreshMenuItem;
        private MenuItem mRenameMenuItem;
        private MenuItem mServiceMenuItem;
        private MenuItem mTrustListMenuItem;

        private Preference mMyDevicePreference;

        private ArrayList mAvailList;

        @Override
        public void onCreate(Bundle icicle) {     
        	super.onCreate(icicle);
            addPreferencesFromResource(R.xml.crossmount_settings);
            mAvailList = new ArrayList();
        }

        @Override
        public void onActivityCreated(Bundle savedInstanceState) {
            super.onActivityCreated(savedInstanceState);
            if (!isEnabled()) {
                Log.d(TAG, "add empty view");
                mEmptyView = (TextView) getView().findViewById(android.R.id.empty);
                mEmptyView.setText(R.string.crossmount_no_devices_found);
                getListView().setEmptyView(mEmptyView);
            }
        }

        @Override
        public void onResume() {
            super.onResume();
            boolean enable = isEnabled();
            // refresh UI
            refreshUI(enable);
            getActivity().invalidateOptionsMenu();
        }

        @Override
        public boolean onPreferenceTreeClick(PreferenceScreen preferenceScreen, 
        		Preference preference) {
            if (preference instanceof CrossmountDevicePreference) {
            	CrossmountDevicePreference devPref = (CrossmountDevicePreference) preference;
                devPref.onClicked();
                return true;
            }
            return super.onPreferenceTreeClick(preferenceScreen, preference);
        }

        @Override
        public void onCreateOptionsMenu(Menu menu, MenuInflater inflater) {
        	super.onCreateOptionsMenu(menu, inflater);
            inflater.inflate(R.menu.options_menu, menu);

            Log.d(TAG, "onCreateOptionsMenu()");
            boolean checked = isEnabled();
            mEnableMenuItem = menu.findItem(R.id.enable_crossmount);
			mEnableMenuItem.setChecked(checked);

			// when enable menu is not checked , the other menu is disable
			mRefreshMenuItem = menu.findItem(R.id.refresh_crossmount);
			mRefreshMenuItem.setEnabled(checked);
            // As UI visibility , always is "start discovery" state.
            mRefreshMenuItem.setVisible(false);
			
			mRenameMenuItem = menu.findItem(R.id.rename_device);
			mRenameMenuItem.setEnabled(checked);
			
			mServiceMenuItem = menu.findItem(R.id.service_sharing);
			mServiceMenuItem.setEnabled(checked);

			mTrustListMenuItem = menu.findItem(R.id.manage_trust_list);
			mTrustListMenuItem.setEnabled(checked);

            refreshTrustMenuVisibility();
        }

		@Override
		public boolean onOptionsItemSelected(MenuItem item) {
            Log.d(TAG, "onOptionItemSelected = " + item.getTitle());
			switch (item.getItemId()) {
			case R.id.enable_crossmount:
                boolean checked = !item.isChecked();
                mEnableMenuItem.setChecked(checked);
				Log.d(TAG, "enable CrossMount menu ? " + checked);
				if (checked) {
					mCrossAdapter.enable();
				} else {
                    mCrossAdapter.stopDiscover();
                    updateProgressUi(false);
					mCrossAdapter.disable();
				}
				return true;
			case R.id.refresh_crossmount:
				mCrossAdapter.startDiscover(null);
                updateProgressUi(true);
				return true;
			case R.id.rename_device:
				new CrossmountNameDialogFragment().show(getFragmentManager(),
						"rename device");
				return true;
			case R.id.service_sharing:
                Utils.launchActivity(mContext, Utils.SERVICE_ACTION, null, null, null, null);
				return true;
			case R.id.manage_trust_list:
                Utils.launchActivity(mContext, Utils.TRUST_LIST_ACTION, null, null, null, null);
				return true;
			default:
				return super.onOptionsItemSelected(item);
			}
		}

        /**
         * refresh UI.
         * @param enabled the CrossMount state
         */
        private void refreshUI(boolean enabled) {
			// clear UI fisrtly
	        PreferenceScreen preferenceScreen = getPreferenceScreen();
	        preferenceScreen.removeAll();

            Log.d(TAG, "refreshUI(), enabled = " + enabled);
            if (enabled) {
                // add myself device description
                if (mMyDevicePreference == null) {
                    mMyDevicePreference = new Preference(getActivity());
                }
                mMyDevicePreference.setSummary(getResources().getString(
                            R.string.crossmount_is_visible_message,
                            mCrossAdapter.getMyDevice().getName()));
                mMyDevicePreference.setSelectable(false);
                preferenceScreen.addPreference(mMyDevicePreference);

                super.createDescriptionCategory();
                boolean isDiscovering = mCrossAdapter.isDiscovering();
                Log.d(TAG, "refreshUI(), isDiscovering = " + isDiscovering);
                updateProgressUi(isDiscovering);
                addDeviceCategory(mDescCategory,
                          R.string.crossmount_preference_found_devices);
            } else {
                mEmptyView = (TextView) getView().findViewById(android.R.id.empty);
                mEmptyView.setText(R.string.crossmount_no_devices_found);
                getListView().setEmptyView(mEmptyView);
            }
		}

        @Override
        public void addDeviceCategory(PreferenceGroup preferenceGroup, int titleId) {
            super.addDeviceCategory(preferenceGroup, titleId);
	        // add available preferences
            Utils.sortAvailDevices(mCrossAdapter, mAvailList);
            addDevices(preferenceGroup, mAvailList);
	    }

        /**
         * add each device.
         * @param preferenceGroup PreferenceGroup
         * @param deviceList List<Device>
         */
	    private void addDevices(PreferenceGroup preferenceGroup,List<Device> deviceList) {
	        for (Device dev : deviceList) {
                CrossmountDevicePreference pref = new CrossmountDevicePreference(getActivity(),
                        mCrossAdapter, dev, null, Utils.MOUNT_FROM_ID);
	        	preferenceGroup.addPreference(pref);
	        }
	        preferenceGroup.setEnabled(true);
	    }

        /**
         * Set the progress bar visibility.
         * @param start boolean.
         */
        private void updateProgressUi(boolean start) {
            if (mDescCategory != null &&
                    mDescCategory instanceof CrossMountProgressCategory) {
                ((CrossMountProgressCategory) mDescCategory).setProgress(start);
                 Log.d(TAG, "setProgress " + start);
            }
        }

        /**
         * Refresh the trust menu visibility.
         */
       private void refreshTrustMenuVisibility() {
            Device[] trustList = mCrossAdapter.getTrustDevices();
            if (trustList == null || trustList.length == 0) {
                mTrustListMenuItem.setVisible(false);
            } else {
                mTrustListMenuItem.setVisible(true);
            }
       }

       @Override
      public void stateUpdate() {
          boolean isEnable = isEnabled();
          // update checkbox's status
           if (mEnableMenuItem != null) {
               mEnableMenuItem.setChecked(isEnable);
           }
           if (isEnable) {
               Log.d(TAG, "callback,enabled ,then startDiscover");
               mCrossAdapter.startDiscover(null);
           }
           refreshUI(isEnable);
           getActivity().invalidateOptionsMenu();
       }

      @Override
      public void connectUpdate() {
          super.connectUpdate();
          boolean isEnable = isEnabled();
          refreshUI(isEnable);
      }

      @Override
      public void discoverComplete() {
          super.connectUpdate();
          boolean isEnable = isEnabled();
          refreshUI(isEnable);
      }

      @Override
      public void trustUpdate() {
          super.trustUpdate();
          refreshTrustMenuVisibility();
      }

      @Override
      public void refreshAsRename(boolean enabled) {
          super.refreshAsRename(enabled);
          refreshUI(enabled);
      }
      }
}
