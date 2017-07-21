/*
* This software/firmware and related documentation ("MediaTek Software") are
* protected under relevant copyright laws. The information contained herein
* is confidential and proprietary to MediaTek Inc. and/or its licensors.
* Without the prior written permission of MediaTek inc. and/or its licensors,
* any reproduction, modification, use or disclosure of MediaTek Software,
* and information contained herein, in whole or in part, shall be strictly prohibited.
*/
/* MediaTek Inc. (C) 2014. All rights reserved.
*
* BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
* THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE")
* RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO RECEIVER ON
* AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL WARRANTIES,
* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
* NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
* SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
* SUPPLIED WITH THE MEDIATEK SOFTWARE, AND RECEIVER AGREES TO LOOK ONLY TO SUCH
* THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES
* THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES
* CONTAINED IN MEDIATEK SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK
* SOFTWARE RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
* STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND
* CUMULATIVE LIABILITY WITH RESPECT TO THE MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
* AT MEDIATEK'S OPTION, TO REVISE OR REPLACE THE MEDIATEK SOFTWARE AT ISSUE,
* OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY RECEIVER TO
* MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
*
* The following software/firmware and/or related documentation ("MediaTek Software")
* have been modified by MediaTek Inc. All revisions are subject to any receiver's
* applicable license agreements with MediaTek Inc.
*/

package com.mediatek.rcs.incallui.ext;

import android.app.Activity;
import android.content.Context;
import android.content.Intent;
import android.content.res.Resources;
import android.graphics.drawable.Drawable;
import android.graphics.drawable.LayerDrawable;
import android.graphics.drawable.RippleDrawable;
import android.net.Uri;
import android.telecom.Call;
import android.telecom.Call.Details;
import android.util.Log;
import android.view.Menu;
import android.view.MenuItem;
import android.view.MenuItem.OnMenuItemClickListener;
import android.view.View;
import android.view.View.OnClickListener;
import android.view.ViewGroup;
import android.widget.ImageButton;
import android.widget.LinearLayout;

import com.mediatek.common.PluginImpl;
import com.mediatek.incallui.ext.DefaultRCSeCallButtonExt;
import com.mediatek.incallui.ext.IRCSeCallButtonExt;
import com.mediatek.rcs.incallui.RichCallController;
import com.mediatek.rcs.incallui.utils.RichCallInsController;

import com.mediatek.rcs.phone.R;

import java.util.HashMap;

@PluginImpl(interfaceName="com.mediatek.incallui.ext.IRCSeCallButtonExt")
public class RCSCallButtonExtension extends DefaultRCSeCallButtonExt
    implements OnClickListener {

    private static final String TAG = "RCSCallButtonExtension";
    private static final int VERTICAL_SPACE_WIDTH = 20;
    private static final int VIEW_SEND_MESSAGE = 0x100;
    private RichCallController mRichCallController;
    private RCSInCallUIPlugin  mRCSInCallUIPlugin;
    private ImageButton mMessageButton;
    private Context mContext;
    private int     mMenuId;

    public RCSCallButtonExtension(Context context) {
        super();
        Log.d(TAG, "RCSCallButtonExtension");
        mContext = context;
        mMenuId = 0x100;
    }


    /**
      * Interface to add message button to hostlayout
      *
      * @param context the host context
      * @param view the host callbutton view
      */
    @Override
    public void onViewCreated(Context context, View view) {
        Resources resource = context.getResources();
        String packageName = context.getPackageName();
        ViewGroup buttonLayout = (ViewGroup) view.findViewById(
                resource.getIdentifier("callButtonContainer", "id", packageName));

        mMessageButton = new ImageButton(context);
        LinearLayout.LayoutParams linearParams = new LinearLayout.LayoutParams(
                dip2dx(context, R.dimen.in_call_button_dimension),
                dip2dx(context, R.dimen.in_call_button_dimension));

        linearParams.setMargins(dip2dx(context, R.dimen.call_button_margin_horizontal), 0,
                dip2dx(context, R.dimen.call_button_margin_horizontal),
                dip2dx(context, R.dimen.call_button_margin_vertical));

        mMessageButton.setLayoutParams(linearParams);
        mMessageButton.setId(VIEW_SEND_MESSAGE);
        Drawable messageDrawable = mContext.getResources()
                .getDrawable(R.drawable.btn_send_message);
        mMessageButton.setBackgroundDrawable(messageDrawable);
        mMessageButton.setEnabled(false);
        mMessageButton.setVisibility(View.VISIBLE);
        mMessageButton.setClickable(false);
        mMessageButton.setOnClickListener(this);
        buttonLayout.addView(mMessageButton, 4);
    }


    @Override
    public void onClick(View view) {
        int id = view.getId();
        if (VIEW_SEND_MESSAGE == id) {
            refrehInstance();
            Call call = mRichCallController.onMenuItemSelected();
            if (call != null) {
                Details details = call.getDetails();
                Uri handle = details == null ? null : details.getHandle();
                if (handle != null) {
                    String scheme = handle.getScheme();
                    String uriString = details.getHandle().getSchemeSpecificPart();
                    if (scheme != null && scheme.equals("tel")) {
                        launchMessage(uriString);
                    }
                }
            }
        }
    }

    /**
      * Interface to updated CallMap to service presenter to control the state of calls
      *
      * @param call the incallactivity call
      * @param callMap the incallactivity callMap
      */
    @Override
    public void onStateChange(Call call, HashMap<String, Call> callMap) {
        Log.d(TAG, "onStateChange.");
        refrehInstance();
        refreshButton();
        mRichCallController.onCallStatusChange(call, callMap);
    }


    /**
      * Interface to updated background color of sendMessage button
      *
      * @param rpDrawable the drawable will be used by sendMessage button
      */
    @Override
    public void updateNormalBgDrawable(RippleDrawable rpDrawable) {
        if (mMessageButton == null) {
            return;
        }

        final LayerDrawable layers = (LayerDrawable) mMessageButton.getBackground();
        layers.setDrawableByLayerId(R.id.backgroundItem, rpDrawable);
        rpDrawable.setState(layers.getState());
        layers.invalidateSelf();
    }

    private void launchMessage(String number) {
        Log.i(TAG, "launchMessage, number = " + number);
        Uri uri = Uri.parse("smsto:" + number);
        Intent intent = new Intent(Intent.ACTION_SENDTO, uri);
        intent.setClassName("com.android.mms", "com.android.mms.ui.ComposeMessageActivity");
        intent.putExtra("rcsmode", 1);

        //Using host context to start activity, and clear FLAG_ACTIVITY_NEW_TASK flags~
        //intent.setFlags(Intent.FLAG_ACTIVITY_NEW_TASK);
        Context hostContext = (Context) mRCSInCallUIPlugin.getInCallActivity();
        if (hostContext != null) {
            hostContext.startActivity(intent);
        }
    }

    private void refrehInstance() {
        Activity currActivity = RichCallInsController.getCurrentActivity();
        mRichCallController = RichCallInsController.getController(currActivity);
        mRCSInCallUIPlugin = RichCallInsController.getInCallUIPlugin(currActivity);
    }

    private void refreshButton() {
        if (mMessageButton == null) {
            Log.i(TAG, "refreshButton, mMessageButton is null.");
            return;
        }

        boolean enable = mRichCallController.isNeedShowMenuItem();
        if (enable) {
            mMessageButton.setEnabled(true);
            mMessageButton.setClickable(true);
        } else {
            mMessageButton.setEnabled(false);
            mMessageButton.setClickable(false);
        }
    }

    private int dip2dx(Context cnx, int dimension) {
        float dpValue = mContext.getResources().getDimension(dimension);
        //final float scale = cnx.getResources().getDisplayMetrics().density;
        //return (int) (dpValue * scale + 0.5);
        return (int) (dpValue + 0.5);
    }
}