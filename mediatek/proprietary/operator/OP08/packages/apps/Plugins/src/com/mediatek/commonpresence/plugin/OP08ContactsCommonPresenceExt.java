package com.mediatek.commonpresence.plugin;

import java.util.List;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.content.res.Resources;
import android.graphics.PorterDuff.Mode;
import android.graphics.drawable.Drawable;

import android.telephony.PhoneNumberUtils;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ImageView;
import android.widget.TextView;

import com.mediatek.common.PluginImpl;
import com.mediatek.contacts.ext.DefaultContactsCommonPresenceExtension;
import com.mediatek.op08.plugin.R;

/**
 * Plugin implementation for Contacts Common Presence.
 */
@PluginImpl(interfaceName = "com.mediatek.contacts.ext.IContactsCommonPresenceExtension")
public class OP08ContactsCommonPresenceExt extends
        DefaultContactsCommonPresenceExtension {
    private static final String TAG = "OP08ContactsCommonPresenceExt";
    private Context mContext;
    private PluginApiManager mInstance = null;

    private ImageView mVideoIconView = null;
    private View mVideoIconLayout = null;
    private int mVideoIconViewWidth;
    private int mVideoIconViewHeight;
    // random integer ID, generated automatically for static view,
    // we need to add dynamic view
    public static final int VIDEO_ICON_ID = 101;

    /**
     * Constructor.
     * @param context context
     */
    public OP08ContactsCommonPresenceExt(Context context) {
        Log.d(TAG, "[OP08ContactsCommonPresenceExt] constuctor entry "
                + context);
        mContext = context;
        PluginApiManager.initialize(context);
        mInstance = PluginApiManager.getInstance();
    }

    /**
     * Checks if contact is video call capable
     *
     * @param contactId
     *            Contact Id.
     * @return true if contact is video call capable.
     */
    @Override
    public boolean isVideoCallCapable(String number) {
        Log.d(TAG, "isVideoCallCapable entry :" + number);
        if (number == null || number == "") {
            return false;
        }
        number = number.trim();
        String phoneNumber = PhoneNumberUtils.stripSeparators(number);
        boolean videoCallCapability = false;
        if (mInstance != null) {
            videoCallCapability = mInstance.isVideoCallCapable(phoneNumber);
            // return true;
            return videoCallCapability;
        }
        return videoCallCapability;
    }

    /**
     * Add video icon view in contact list view
     *
     * @param contactId Contact Id.
     * @param viewGroup host viewgroup.
     */
    @Override
    public void addVideoCallView(long contactId, ViewGroup viewGroup) {
        Log.d(TAG, "[addVideoCallView] contactId:" + contactId);
        createVideoView(contactId);
        View view = viewGroup.findViewById(VIDEO_ICON_ID);
        if (view != null) {
            // view is already added
            if (mVideoIconLayout == null) {
                // need to remove view
                Log.d(TAG, "[addVideoCallView] remove view");
                viewGroup.removeView(view);
            }
            return;
        }

        if (mVideoIconLayout != null) {
            Log.d(TAG, "[addVideoCallView] add view");
            viewGroup.addView(mVideoIconLayout);
        }
    }

    /**
     * OnMeasure
     *
     * @param widthMeasureSpec width.
     * @param heightMeasureSpec height.
     */
    @Override
    public void onMeasure(int widthMeasureSpec, int heightMeasureSpec) {
        Log.d(TAG, "[onMeasure] mVideoIconView:" + mVideoIconView
                + ",mVideoIconLayout:" + mVideoIconLayout);
        if (mVideoIconLayout == null || mInstance == null) {
            return;
        }
        if (isVisible(mVideoIconView)) {

            if (mInstance != null) {
                Drawable a = getAppIcon();
                if (a != null) {
                    mVideoIconViewWidth = a.getIntrinsicWidth();
                    mVideoIconViewHeight = a.getIntrinsicHeight();
                    mVideoIconLayout.measure(mVideoIconViewWidth,
                            mVideoIconViewHeight);
                } else {
                    mVideoIconViewWidth = 0;
                    mVideoIconViewHeight = 0;
                }
            } else {
                mVideoIconViewWidth = 0;
                mVideoIconViewHeight = 0;
            }
            Log.d(TAG, "[onMeasure]  mVideoIconViewWidth : "
                    + mVideoIconViewWidth + " | mVideoIconViewHeight : "
                    + mVideoIconViewHeight);
        } else {
            Log.d(TAG, "[onMeasure] mVideoIconView not visible");
        }

    }

    /**
     * OnLayout
     *
     * @param changed .
     * @param leftBound .
     * @param topBount .
     * @param rightBound .
     * @param bottomBound .
     */
    @Override
    public void onLayout(boolean changed, int leftBound, int topBound,
            int rightBound, int bottomBound) {
        Log.d(TAG, "[onLayout] mVideoIconView:" + mVideoIconView
                + "mVideoIconLayout:" + mVideoIconLayout);
        if (mVideoIconLayout == null || mInstance == null) {
            return;
        }
        if (isVisible(mVideoIconView)) {
            int photoTop1 = topBound
                    + (bottomBound - topBound - mVideoIconViewHeight) / 2;
            mVideoIconLayout.layout(rightBound - (mVideoIconViewWidth),
                    photoTop1, rightBound, photoTop1 + mVideoIconViewHeight);
        }

    }

    /**
     * getWidthWithPadding
     * @return width
     */
    @Override
    public int getWidthWithPadding() {
        Log.d(TAG, "[getWidthWithPadding] mVideoIconLayout:" + mVideoIconLayout);
        return mVideoIconLayout == null ? 0 : mVideoIconLayout
                .getMeasuredWidth() + mVideoIconLayout.getPaddingLeft();
    }


    //Helper APIs
    protected boolean isVisible(View view) {
        return view != null && view.getVisibility() == View.VISIBLE;
    }

    public Drawable getAppIcon() {
        Log.d(TAG, "getAppIcon entry");
        Resources resources = mContext.getResources();
        Drawable drawable = null;
        Log.i(TAG, " resources : " + resources);
        if (resources != null) {
            drawable = resources.getDrawable(R.drawable.video_icon_indicaton);
            drawable.setColorFilter(0xff000000, Mode.MULTIPLY);
        } else {
            Log.d(TAG, "getAppIcon resources is null");
        }
        Log.d(TAG, "getAppIcon exit");
        return drawable;
    }

    public Drawable getContactVideoPresence(long contactId) {
        Log.d(TAG, "getContactPresence entry, contact id is: " + contactId);
        Drawable drawable = null;
        boolean presence = false;
        if (mInstance == null) {
            return null;
        }
        List<String> numbers = mInstance.getNumbersByContactId(contactId);
        Log.d(TAG, "getContactPresence numbers: " + numbers);
        if (numbers != null && numbers.size() > 0) {
            for (String number : numbers) {
                if (presence == true) {
                    break;
                }
                presence = mInstance.isVideoCallCapable(number);
                if (presence == true) {
                    Resources resources = mContext.getResources();
                    if (resources != null) {
                        drawable = resources
                                .getDrawable(R.drawable.video_icon_indicaton);
                    }
                }
            }
        }
        Log.d(TAG, "getContactPresence exit, presence: " + presence);
        return drawable;
    }

    /**
     * Inflates the view from xml, set the image view
     *
     * @param contactId
     *            contactId of the contact in list
     */
    private void createVideoView(long contactId) {
        Log.d(TAG, "[createVideoView] contactId:" + contactId);
        Drawable videoIcon = null;
        mVideoIconLayout = null;
        mVideoIconView = null;
        boolean canSetVideoIcon = false;
        if (mInstance == null) {
            Log.d(TAG, "[createVideoView] mInstacne is null");
            return;
        }
        if (mInstance != null) {
            videoIcon = getContactVideoPresence(contactId);
            if (videoIcon != null) {
                canSetVideoIcon = true;
            } else {
                Log.d(TAG, "[createVideoView] icon : " + videoIcon);
                canSetVideoIcon = false;
            }
        } else {
            Log.e(TAG, "[createVideoView] mInstance is null ");
            canSetVideoIcon = false;
        }

        // canSetVideoIcon true means contact is video enabled contact, need to
        // add imageview to viewgroup
        if (canSetVideoIcon) {
            try {
                Log.d(TAG, "[createVideoView] inflating icon view");
                LayoutInflater mInflater;
                mInflater = LayoutInflater.from(mContext);
                mVideoIconLayout = mInflater.inflate(
                        R.layout.video_icon_plugin, null);
                mVideoIconLayout.setId(VIDEO_ICON_ID);
                mVideoIconView = (ImageView) mVideoIconLayout
                        .findViewById(R.id.video_icon);
                mVideoIconView.setVisibility(View.VISIBLE);
                mVideoIconView.setImageDrawable(getAppIcon());
            } catch (Exception e) {
                e.printStackTrace();
            }
        }
    }
}
