package com.mediatek.crossmountsettings.ui.touch;

import android.content.Intent;
import android.os.Bundle;
import android.preference.PreferenceActivity;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.Menu;
import android.view.MenuInflater;
import android.view.MenuItem;

import android.view.View;
import android.view.ViewGroup;
import android.widget.TextView;

import com.mediatek.crossmount.adapter.Service;
import com.mediatek.crossmount.adapter.Service.ServiceState;

import com.mediatek.crossmountsettings.R;
import com.mediatek.crossmountsettings.util.SettingsPreferenceFragment;
import com.mediatek.crossmountsettings.util.Utils;

/**
 * Activity to Control the touch for TV.
 */
public class TouchCtrlActivity extends PreferenceActivity {

    private static final String TAG = "TouchCtrlActivity";

    @Override
    public Intent getIntent() {
        Intent modIntent = new Intent(super.getIntent());
        modIntent.putExtra(EXTRA_SHOW_FRAGMENT, TouchFragment.class.getName());
        modIntent.putExtra(EXTRA_NO_HEADERS, true);
        return modIntent;
    }

/**
 * Crossmount touch fragment to control TV.
 */
    public  static class TouchFragment extends SettingsPreferenceFragment {
        private String mDeviceName;
        private String mDeviceId;
        private String mServiceName;
        private Service mSharedService;

        private TouchLayout mTouchZoom;
        private TextView mTextView;
        private MenuItem mDisconnectMenu;
        private MenuItem mExitMenu;

        @Override
        public void onCreate(Bundle icicle) {
            super.onCreate(icicle);
            Intent intent = getActivity().getIntent();
            mDeviceName = intent.getStringExtra(Utils.DEVICE_NAME);
            mDeviceId = intent.getStringExtra(Utils.DEVICE_ID);
            mServiceName = intent.getStringExtra(Utils.SERVICE_NAME);
            Log.d(TAG, "onCreate()333,deviceName = " + mDeviceName
                    + ", deviceId = " + mDeviceId
                    + ", serviceName = " + mServiceName);
            mSharedService = Utils.getSharedToService(mCrossAdapter.getMyDevice(), mServiceName);
        }
        @Override
        public View onCreateView(LayoutInflater inflater, ViewGroup container,
                Bundle savedInstanceState) {
            super.onCreateView(inflater, container, savedInstanceState);
            Log.d(TAG, "onCreateView");
            View view = inflater.inflate(R.layout.touch_main, null);
            mTouchZoom = (TouchLayout) view.findViewById(R.id.touchZoom);
            mTouchZoom.setAdapter(mCrossAdapter);
            mTextView = (TextView) view.findViewById(R.id.msg);
            mTextView.setText(getResources().getString(R.string.touch_msg, mDeviceName));
            mTextView.setVisibility(View.VISIBLE);
            // set View to get it's height&weight lately
            mTouchZoom.setView(view, mTextView);
            return view;
        }

        @Override
        public void onCreateOptionsMenu(Menu menu, MenuInflater inflater) {
            super.onCreateOptionsMenu(menu, inflater);
            inflater.inflate(R.menu.touch_menu, menu);
            mDisconnectMenu = menu.findItem(R.id.disconect);
            mExitMenu = menu.findItem(R.id.exit);
        }

        @Override
        public boolean onOptionsItemSelected(MenuItem item) {
            Log.d(TAG, "onOptionItemSelected = " + item.getTitle());
            switch (item.getItemId()) {
            case R.id.disconect:
                if (mSharedService != null) {
                    mSharedService.stopShareTo(mDeviceId);
                }
                getActivity().finish();
                return true;
            case R.id.exit:
                getActivity().finish();
                return true;
            default:
                return super.onOptionsItemSelected(item);
            }
        }

        @Override
        public void connectUpdate() {
            super.connectUpdate();
            if (mSharedService == null) {
                return;
            }
            ServiceState stat = mSharedService.getState(mDeviceId);
            // From connected to available state , mean it's disconnected
            Log.d(TAG, "state = " + stat);
            if (ServiceState.CONNECTED != stat) {
                getActivity().finish();
                Utils.launchActivity(mContext, Utils.TOUCH_EXIT_FROM_TV,
                        mDeviceName, mDeviceId, mServiceName, null);
            }
        }
    }
}
