package com.mediatek.incallui.plugin;

import android.content.Context;
import android.telecom.Call;
import android.telecom.VideoProfile;
import android.util.Log;
import android.widget.Toast;

import com.mediatek.common.PluginImpl;
import com.mediatek.op08.plugin.R;
import com.mediatek.incallui.ext.DefaultVideoCallExt;
import com.mediatek.incallui.ext.IVideoCallExt;

/**
 * Plugin implementation for VideoCall.
 */
@PluginImpl(interfaceName="com.mediatek.incallui.ext.IVideoCallExt")
public class OP08VideoCallExt extends DefaultVideoCallExt {
    private static final String TAG = "OP08VideoCallExt";
    private Context mContext;
    private int SESSION_EVENT_BAD_DATA_BITRATE = 4008;
    /** Constructor.
     * @param context context
     */
    public OP08VideoCallExt(Context context) {
        super();
        mContext = context;
    }

    @Override
    public void onCallSessionEvent(Call call, int event) {
        Log.d(TAG, "[onCallSessionEvent]downgrade due to insufficient bandwidth event"
                + event);
        if (event == SESSION_EVENT_BAD_DATA_BITRATE) {
            Toast.makeText(mContext,
                    mContext.getString(R.string.video_call_downgrade_insufficient_bandwidth),
                    Toast.LENGTH_SHORT).show();
            call.getVideoCall().sendSessionModifyRequest(
                    new VideoProfile(VideoProfile.STATE_AUDIO_ONLY));
        }
    }

}
