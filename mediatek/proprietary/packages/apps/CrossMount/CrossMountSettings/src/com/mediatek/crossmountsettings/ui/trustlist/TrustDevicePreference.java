package com.mediatek.crossmountsettings.ui.trustlist;

import android.content.Context;
import android.content.SharedPreferences;
import android.preference.Preference;
import android.util.Log;
import android.view.View;
import android.widget.Switch;

import com.mediatek.crossmount.adapter.Device;
import com.mediatek.crossmountsettings.R;
import com.mediatek.crossmountsettings.util.Utils;


/**
 * TrustDevicePreference is the preference type.
 * used to display in the Trust list manage UI.
 */
public final class TrustDevicePreference extends Preference implements View.OnClickListener {
    private static final String TAG = "TrustDevicePref";
    private final Device mDevice;
    private final String endStr =  "/";
    private boolean mSwitchState;
    private Switch mSwitch;

    private final SharedPreferences mSharedPrf;
    private static final String SHARED_PREFERENCES_NAME = "crossmount_settings";

    public TrustDevicePreference(Context context,Device dev) {
        super(context);
        mDevice = dev;
        mSharedPrf = context.getSharedPreferences(SHARED_PREFERENCES_NAME, Context.MODE_PRIVATE);
        setLayoutResource(R.layout.preference_trust_list);
        // add switch
        setWidgetLayoutResource(R.layout.preference_switch);
        onDeviceAttributesChanged();
    }


    @Override
    protected void onBindView(View view) {
        mSwitchState = getSharePrefBoolenValue(mDevice.getId());
        Log.d(TAG, "onBindView, mSwitchState= " + mSwitchState);
        super.onBindView(view);
        mSwitch = (Switch) view.findViewById(R.id.status);
        mSwitch.setChecked(mSwitchState);
        view.setOnClickListener(this);
    }

    /**get the state of trust device.
     * @return the preference switch state
     * */
    public boolean isRemoved() {
        boolean isRemoved = !mSwitch.isChecked();
        Log.d(TAG, "isRemoved = " + isRemoved);
        return isRemoved;
    }

    @Override
    public void onClick(View v) {
        mSwitchState = !mSwitch.isChecked();
        String deviceId = mDevice.getId();
        mSwitch.setChecked(mSwitchState);
        setSharePrefBoolenValue(deviceId, mSwitchState);
        Log.d(TAG, "onClick(),"  + deviceId + ", isTrust " + mSwitchState);
    }

    /**
     * update the attributes.
     */
    public void onDeviceAttributesChanged() {
        Log.d(TAG, "onDeviceAttributesChanged");
        setTitle(mDevice.getName());
        setIcon(Utils.getCrossmountClassDrawable(mDevice));
    }


    private boolean getSharePrefBoolenValue(String key) {
        return mSharedPrf.getBoolean(key, true);
    }

    private void setSharePrefBoolenValue(String key, boolean value) {
        SharedPreferences.Editor editor = mSharedPrf.edit();
        editor.putBoolean(key, value);
        editor.commit();
    }

    public void clearSharedPreference() {
        SharedPreferences.Editor editor = mSharedPrf.edit();
        editor.clear();
        editor.commit();
    }
}
