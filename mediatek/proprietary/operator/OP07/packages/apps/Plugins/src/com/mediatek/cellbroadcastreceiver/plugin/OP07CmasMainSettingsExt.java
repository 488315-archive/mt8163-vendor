
package com.mediatek.cellbroadcastreceiver.plugin;

import android.content.Context;
import android.content.SharedPreferences;
import android.preference.CheckBoxPreference;
import android.preference.Preference;
import android.preference.Preference.OnPreferenceChangeListener;
import android.preference.PreferenceActivity;
import android.preference.PreferenceManager;
import android.preference.PreferenceScreen;
import android.util.Log;

import com.mediatek.cellbroadcastreceiver.CellBroadcastMainSettings;
import com.mediatek.cmas.ext.DefaultCmasMainSettingsExt;
import com.mediatek.common.PluginImpl;
import com.mediatek.op07.plugin.R;



@PluginImpl(interfaceName = "com.mediatek.cmas.ext.ICmasMainSettingsExt")
public class OP07CmasMainSettingsExt extends DefaultCmasMainSettingsExt {

    private static final String TAG = "CellBroadcastReceiver/OP07CmasMainSettingsExt";
    private static final int PRESIDENT_ALERT_ID = 4370;

    public static final String KEY_SPANISH_ALERT = "enable_spanish_alert";

    private Context mContext;

    public PreferenceActivity mprefActivity;


    public OP07CmasMainSettingsExt(Context context) {
        super(context);
        mContext = context;
    }

    /**
     * Add Spanish Alert Option in Main Setting
     * @param prefActivity Current Preference Activity
     * @return void
     */
    public void activateSpanishAlertOption(PreferenceActivity prefActivity) {
        Log.i(TAG, "Plugin activateSpanishAlertOption");
        mprefActivity = prefActivity;
        PreferenceScreen prefSet = prefActivity.getPreferenceScreen();

        CheckBoxPreference spanishAlert = new CheckBoxPreference(
        (CellBroadcastMainSettings) mprefActivity);
        spanishAlert.setKey(KEY_SPANISH_ALERT);
        spanishAlert.setChecked(true);
        spanishAlert.setDefaultValue(true);
        spanishAlert.setTitle(mContext.getString(R.string.spanish_alert_title));
        spanishAlert.setSummary(mContext.getString(R.string.spanish_alert_title_summary));

        prefSet.addPreference(spanishAlert);
        Log.i(TAG, "Spanish alert checkbox added");

        spanishAlert.setOnPreferenceChangeListener(new OnPreferenceChangeListener() {
            @Override
            public boolean onPreferenceChange(Preference preference, Object newValue) {

                boolean value = (Boolean) newValue;
                SharedPreferences prefs = PreferenceManager.getDefaultSharedPreferences(mContext);
                SharedPreferences.Editor editor = prefs.edit();

                editor.putBoolean(KEY_SPANISH_ALERT, value);
                editor.commit();
                return true;
            }
        });
    }

    /**
     * Get Spanish Alert value from Main Setting
     * @param languageCode of Alert
     * @param msgId
     * @return boolean whether alert is Spanish or not
     */
    public boolean isSpanishAlert(String languageCode, int msgId) {
        Log.i(TAG, "Plugin checkSpanishAlert");
        SharedPreferences prefs = PreferenceManager.getDefaultSharedPreferences(mContext);
        boolean spanishAlert = prefs.getBoolean(KEY_SPANISH_ALERT, true);
        Log.i(TAG, "Language code:" + languageCode + "  SpanishAlert option:" + spanishAlert);

        if (msgId == PRESIDENT_ALERT_ID) {
            return true;
        } else if (languageCode.contains("es")) {
            return spanishAlert;
        }
        else {
            return true;
        }
    }

}
