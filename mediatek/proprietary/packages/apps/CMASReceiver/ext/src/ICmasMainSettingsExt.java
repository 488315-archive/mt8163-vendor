
package com.mediatek.cmas.ext;

import android.preference.PreferenceActivity;

public interface ICmasMainSettingsExt {
    
    /**
     * Get Alert Volume value
     * @internal
     */
    public float getAlertVolume(int msgId);

    /**
     * Get Alert Vibration value
     * @internal
     */
    public boolean getAlertVibration(int msgId);

    /**
     * Set Alert Volume and Vibration value
     * @internal
     */
    public boolean setAlertVolumeVibrate(int msgId, boolean currentValue);

    /**
     * Stop mediplayer when press Home key
     * @internal
     */
    public void stopMediaPlayer();

    /**
     * Add Alert Volume and Vibration in Main Setting
     * @internal
     */
    public void addAlertSoundVolumeAndVibration(PreferenceActivity prefActivity);

    /**
     * Add Spanish Alert Option in Main Setting
     * @param prefActivity Current Preference Activity
     * @return void
     */
    public void activateSpanishAlertOption(PreferenceActivity prefActivity);

    /**
     * Get Spanish Alert value from Main Setting
     * @param languageCode of Alert
     * @param msgId CMAS Channel ID
     * @return boolean whether alert is Spanish or not
     */
    public boolean isSpanishAlert(String languageCode, int msgId);
}
