package com.mediatek.crossmountsettings.ui.touch;

import android.content.Intent;
import android.os.Bundle;
import android.preference.PreferenceActivity;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.Button;
import android.widget.TextView;

import com.mediatek.crossmount.adapter.Service;
import com.mediatek.crossmount.adapter.Service.ServiceState;

import com.mediatek.crossmountsettings.R;
import com.mediatek.crossmountsettings.util.SettingsPreferenceFragment;
import com.mediatek.crossmountsettings.util.Utils;

/**
 * Activity to exit the touch control for TV.
 */
public class TouchExitActivity extends PreferenceActivity {

    private static final String TAG = "TouchExitActivity";

    @Override
    public Intent getIntent() {
        Intent modIntent = new Intent(super.getIntent());
        modIntent.putExtra(EXTRA_SHOW_FRAGMENT, ExitFragment.class.getName());
        modIntent.putExtra(EXTRA_NO_HEADERS, true);
        return modIntent;
    }

    /**
     * Crossmount touch fragment to exit controlling TV.
     */
    public  static class ExitFragment extends SettingsPreferenceFragment {

        private String mDeviceName;
        private String mDeviceId;
        private String mServiceName;
        private Service mSharedService;

        private TextView mTextView;
        private Button mExitBtn;
        @Override
        public void onCreate(Bundle icicle) {
            super.onCreate(icicle);
            Intent intent = getActivity().getIntent();
            mDeviceName = intent.getStringExtra(Utils.DEVICE_NAME);
            mDeviceId = intent.getStringExtra(Utils.DEVICE_ID);
            mServiceName = intent.getStringExtra(Utils.SERVICE_NAME);
            Log.d(TAG, "onCreate(),deviceName = " + mDeviceName
                    + ", deviceId = " + mDeviceId
                    + ", serviceName = " + mServiceName);
            mSharedService = Utils.getSharedToService(mCrossAdapter.getMyDevice(), mServiceName);
            setHasOptionsMenu(true);
        }

        @Override
        public View onCreateView(LayoutInflater inflater, ViewGroup container,
                Bundle savedInstanceState) {
            View view = inflater.inflate(R.layout.touch_exit, null);
            mTextView = (TextView) view.findViewById(R.id.exit_msg);
            mTextView.setText(getResources().getString(R.string.touch_exit_msg, mDeviceName));
            mExitBtn = (Button) view.findViewById(R.id.exit_btn);
            mExitBtn.setOnClickListener(mExitClickListener);
            return view;
        }

        @Override
        public void connectUpdate() {
            super.connectUpdate();
            if (mSharedService == null) {
                return;
            }
            ServiceState stat = mSharedService.getState(mDeviceId);
            Log.d(TAG, "stat = " + stat);
            if (ServiceState.CONNECTED == stat) {
                getActivity().finish();
            }
        }

        /**
         * the exit button's listener.
         */
        private Button.OnClickListener mExitClickListener = new Button.OnClickListener() {
            public void onClick(View v) {
                Log.d(TAG, "onClick, finish it");
                getActivity().finish();
            }
        };
    }

}
