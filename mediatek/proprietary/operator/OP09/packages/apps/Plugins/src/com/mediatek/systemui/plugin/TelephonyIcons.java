package com.mediatek.systemui.plugin;

import com.mediatek.op09.plugin.R;

/**
 * M: This class define the OP09 constants of telephony icons.
 */
final class TelephonyIcons {

    /// For QuickSettings customization @{.
    static final int QS_MOBILE_DISABLE = R.drawable.ic_qs_mobile_disable;
    static final int QS_MOBILE_ENABLE = R.drawable.ic_qs_mobile_enable;

    static final int QS_DUAL_SIM_SETTINGS_DISABLE = R.drawable.ic_qs_dual_sim_settings_disable;
    static final int QS_DUAL_SIM_SETTINGS_ENABLE = R.drawable.ic_qs_dual_sim_settings_enable;

    static final int QS_APN_SETTINGS_DISABLE = R.drawable.ic_qs_apn_settings_disable;
    static final int QS_APN_SETTINGS_ENABLE = R.drawable.ic_qs_apn_settings_enable;

    /// Add for SIM Conn.
    public static final int[] IC_SIM_CONNECT_INDICATOR = {
        R.drawable.ic_qs_mobile_sim1_enable_disable,
        R.drawable.ic_qs_mobile_sim1_enable_enable,
        R.drawable.ic_qs_mobile_sim1_disable_disable,
        R.drawable.ic_qs_mobile_sim1_disable_enable,
        R.drawable.ic_qs_mobile_sim2_enable_disable,
        R.drawable.ic_qs_mobile_sim2_enable_enable,
        R.drawable.ic_qs_mobile_sim2_disable_disable,
        R.drawable.ic_qs_mobile_sim2_disable_enable,
        R.drawable.ic_qs_mobile_all_disable_disable,
        R.drawable.ic_qs_mobile_sim1_enable_off,
        R.drawable.ic_qs_mobile_sim1_disable_off,
        R.drawable.ic_qs_mobile_sim2_enable_off,
        R.drawable.ic_qs_mobile_sim2_disable_off
    };
    /// For QuickSettings customization @}.

}
