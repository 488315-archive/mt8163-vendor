package com.mediatek.hotknot.common.ui;

import java.util.ArrayList;

import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.OutputStreamWriter;
import java.nio.charset.Charset;
import java.util.List;

import android.app.Activity;
import android.app.ActivityManager;
import android.app.ActivityManager.RunningTaskInfo;
import android.app.AlertDialog;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.content.ComponentName;
import android.content.IntentFilter;
import android.net.Uri;
import android.os.BatteryManager;
import android.os.Bundle;
import android.os.Parcelable;
import android.os.Handler;
import android.os.Message;
import android.util.Log;
import android.util.Patterns;
import com.mediatek.hotknot.HotKnotAdapter;
import com.mediatek.hotknot.HotKnotMessage;

import java.io.UnsupportedEncodingException;
import java.net.URLDecoder;

class Utility {
    static boolean isCharging(Context context) {
        Intent intent = context.registerReceiver(null, new IntentFilter(
                            Intent.ACTION_BATTERY_CHANGED));
        int plugged = intent.getIntExtra(BatteryManager.EXTRA_PLUGGED, -1);
        return plugged == BatteryManager.BATTERY_PLUGGED_AC
                            || plugged == BatteryManager.BATTERY_PLUGGED_USB;
    }
}

public class HotKnotSendActivity extends Activity implements
                    HotKnotAdapter.OnHotKnotCompleteCallback, Handler.Callback {

    private static final String TAG = "HotKnotCommonUI";

    public static final String ACTION_SHARE = "com.mediatek.hotknot.action.SHARE";
    public static final String EXTRA_SHARE_MSG = "com.mediatek.hotknot.extra.SHARE_MSG";
    public static final String EXTRA_SHARE_URIS = "com.mediatek.hotknot.extra.SHARE_URIS";

    private static final String HOTKNOT_URI_MIME_TYPE = "com.mediatek.hotknot.default/url";

    private static final int MSG_HOTKNOT_SEND_COMPLETE = 0;
    private static HotKnotAdapter sAdapter;
    private HotKnotMessage mMessage;
    private Uri[] mUris;
    private Handler mHandler;
    private EnableDialog mEnabledDialog;

    private boolean createHotKnotAdapter(Context context) {
        if (sAdapter == null) {
            sAdapter = HotKnotAdapter.getDefaultAdapter(context);
        }
        return sAdapter != null;
    }

    private Uri[] resizeUris(Uri[] uris) {
        if (uris == null) {
            return uris;
        }

        boolean resize = false;
        int i = 0;
        for (i = 0; i < uris.length; i++) {
            if (uris[i] == null) {
                resize = true;
                break;
            }
        }

        if (resize) {
            ArrayList<Uri> items = new ArrayList<Uri>();
            for (i = 0; i < uris.length; i++) {
                if (uris[i] == null) {
                    // Do Nothing
                } else {
                    items.add(uris[i]);
                }
            }

            if (items.size() > 0) {
                Uri[] result = new Uri[items.size()];
                items.toArray(result);
                return result;
            }
        } else {
            return uris;
        }

        return null;
    }

    private Uri extractFileUri(Uri uri) {

        String schema = uri.getScheme();
        Uri result = uri;
        String strUri = uri.toString();

        if ("content".equals(schema)
                            && "com.google.android.apps.photos.contentprovider".equals(uri
                                                .getAuthority())) {
            String path = uri.getPath();
            if (path == null || path.length() == 0) {

            } else {
                int index = path.indexOf("file://");
                if (index < 0) {
                } else {
                    int endIndex = path.indexOf("/ACTUAL", index);
                    if (endIndex < 0) {
                    } else {
                        String newUri = path.substring(index, endIndex);
                        try {
                            result = Uri.parse(newUri);
                            if (result == null) {
                                result = uri;
                            }
                        } catch (Exception e) {
                            e.printStackTrace();
                        }
                    }
                }
            }
        } else if (strUri != null && strUri.length() >= 2 && strUri.charAt(0) == '/'
                            && strUri.charAt(1) != '/') {

            try {
                File file = new File(strUri);
                if (file.exists()) {
                    result = Uri.fromFile(file);
                } else {
                    Log.e(TAG, "extractFileUri(), file not exist");
                    result = null;
                }
            } catch (Exception e) {
                e.printStackTrace();
                result = null;
            }

        }
        return result;
    }

    private void checkSpecailTypes(Uri[] mUris) {
        for (int i = 0; i < mUris.length; i++) {
            mUris[i] = extractFileUri(mUris[i]);
            if (mUris[i] == null) {
            } else {
                grantUriPermission("com.mediatek.hotknotbeam", mUris[i],
                                    Intent.FLAG_GRANT_READ_URI_PERMISSION);
            }
        }
    }

    private boolean handleShareIntent(Intent intent, String type) {

        Log.d(TAG, "handleShareIntent()");
        if (type == null) {
            return false;
        }

        Log.d(TAG, "handleShareIntent(), type = " + type);
        Uri uriExtra = (Uri) intent.getParcelableExtra(Intent.EXTRA_STREAM);
        String textExtra = intent.getStringExtra(Intent.EXTRA_TEXT);
        if (uriExtra == null && textExtra == null) {
            return false;
        }

        if (textExtra == null) {
            Log.d(TAG, "handleShareIntent(), textExtra = NULL");
            mUris = new Uri[1];
            mUris[0] = extractFileUri(uriExtra);
            if (mUris[0] == null) {
            } else {
                grantUriPermission("com.mediatek.hotknotbeam", mUris[0],
                                    Intent.FLAG_GRANT_READ_URI_PERMISSION);
            }

        } else {
            if (uriExtra == null) {
                Log.d(TAG, "handleShareIntent(), uriExtra = NULL");
                if ("text/plain".equalsIgnoreCase(type)) {
                    Charset charset = Charset.forName("UTF-8");
                    if (Patterns.WEB_URL.matcher(textExtra).matches()) {
                        Log.d(TAG, "Uri message");
                        mMessage = new HotKnotMessage(HOTKNOT_URI_MIME_TYPE,
                                            textExtra.getBytes(charset));
                    } else {
                        Log.d(TAG, "Plain text message");
                        File cacheDir = getExternalCacheDir();
                        String fileName = getString(R.string.hotknot_content_share_file);
                        File cachePlain = new File(cacheDir, fileName);
                        if (cachePlain.exists()) {
                            cachePlain.delete();
                            cachePlain = new File(cacheDir, fileName);
                        }

                        try {
                            FileOutputStream out = new FileOutputStream(cachePlain);
                            try {
                                OutputStreamWriter writer = new OutputStreamWriter(out);
                                try {
                                    writer.write(textExtra);
                                } finally {
                                    writer.close();
                                }
                            } finally {
                                out.close();
                            }
                            mUris = new Uri[1];
                            mUris[0] = Uri.fromFile(cachePlain);
                            Log.d(TAG, "URL:" + mUris[0].toString());
                        } catch (IOException e) {
                            e.printStackTrace();
                            return false;
                        }
                    }
                } else {
                    Log.d(TAG, "handleShareIntent(), textExtra = " + textExtra);
                }
            } else {
                Log.d(TAG, "handleShareIntent(), Uri is not NULL, textExtra = " + textExtra);
                mUris = new Uri[] { uriExtra };
                mUris[0] = extractFileUri(mUris[0]);
                if (mUris[0] == null) {
                } else {
                    grantUriPermission("com.mediatek.hotknotbeam", mUris[0],
                                        Intent.FLAG_GRANT_READ_URI_PERMISSION);
                }
            }
        }
        return true;
    }

    private boolean retrieveShareDataFromIntent(Intent intent) {
        try {
            Log.d(TAG, "retrieveShareDataFromIntent()");
            if (intent == null) {
                return false;
            }

            String action = intent.getAction();
            String type = intent.getType();
            Log.d(TAG, "retrieveShareDataFromIntent(), action = " + action);

            mMessage = null;
            mUris = null;
            if (HotKnotSendActivity.ACTION_SHARE.equals(action)) {
                mMessage = (HotKnotMessage) intent.getParcelableExtra(EXTRA_SHARE_MSG);
                Parcelable parcels[] = intent.getParcelableArrayExtra(EXTRA_SHARE_URIS);
                if (parcels != null && parcels.length > 0) {
                    mUris = new Uri[parcels.length];
                    for (int i = 0; i < parcels.length; i++) {
                        mUris[i] = (Uri) parcels[i];
                        mUris[i] = extractFileUri(mUris[i]);
                        if (mUris[i] == null) {
                        } else {
                            grantUriPermission("com.mediatek.hotknotbeam", mUris[i],
                                                Intent.FLAG_GRANT_READ_URI_PERMISSION);
                        }
                    }
                }
            } else if (Intent.ACTION_SEND.equals(action) && type != null) {
                return handleShareIntent(intent, type);
            } else if (Intent.ACTION_SEND_MULTIPLE.equals(action) && type != null) {
                ArrayList<Uri> uris = intent.getParcelableArrayListExtra(Intent.EXTRA_STREAM);
                mUris = uris.toArray(new Uri[0]);
                checkSpecailTypes(mUris);
            }
        } catch (ArrayStoreException e) {
            e.printStackTrace();
        } catch (NullPointerException e) {
            e.printStackTrace();
        }
        Log.d(TAG, "retrieveShareDataFromIntent(), status = " + (mMessage != null || mUris != null));
        return (mMessage != null || mUris != null);
    }

    private void closeCurrentActivity() {
        finish();
    }

    private void registerShareData() {
        Log.d(TAG, "registerShareData()");
        if (mMessage != null) {
            Log.d(TAG, "registerShareData(), register message");
            sAdapter.setHotKnotMessage(mMessage, this);
        }

        mUris = resizeUris(mUris);
        if (mUris != null) {
            Log.d(TAG, "registerShareData(), register URI");
            sAdapter.setHotKnotBeamUris(mUris, this);
        }
        sAdapter.setOnHotKnotCompleteCallback(this, this);
    }

    private void onConfirmEnableHotKnot(boolean enable) {
        if (enable) {
            sAdapter.enable();
        } else {
            finish();
        }
    }

    private void onHotKnotEnabled(boolean enabled) {
        if (enabled) {
            registerShareData();
        }
    }

    private class EnableDialog implements DialogInterface.OnClickListener,
                        DialogInterface.OnDismissListener {
        private boolean mIsAllow;
        private AlertDialog mDialog;

        EnableDialog(Context context) {
            mDialog = new AlertDialog.Builder(context, AlertDialog.THEME_HOLO_DARK)
                                .setTitle(R.string.hotknot).setIcon(R.drawable.ic_settings_hotknot)
                                .setMessage(getDisplayMessage(context))
                                .setPositiveButton(R.string.allow, this)
                                .setNegativeButton(R.string.deny, this).setOnDismissListener(this)
                                .create();
        }

        String getDisplayMessage(Context context) {
            if (Utility.isCharging(context)) {
                return context.getString(R.string.use_hotknot) + "\n"
                                    + context.getString(R.string.usb_warning);
            } else {
                return context.getString(R.string.use_hotknot);
            }
        }

        void show(Context context) {
            mDialog.setMessage(getDisplayMessage(context));
            mDialog.show();
        }

        @Override
        public void onDismiss(DialogInterface dialog) {
            if (!mIsAllow) {
                finish();
            }
        }

        @Override
        public void onClick(DialogInterface dialog, int which) {
            if (which == DialogInterface.BUTTON_POSITIVE) {
                mIsAllow = true;
                onConfirmEnableHotKnot(true);
            } else {
                onConfirmEnableHotKnot(false);
            }
        }
    }

    private class RetryDialog implements DialogInterface.OnClickListener,
                        DialogInterface.OnDismissListener {
        private AlertDialog mDialog;
        private boolean mShouldRetry;

        RetryDialog(Context context) {
            mDialog = new AlertDialog.Builder(context, AlertDialog.THEME_HOLO_DARK)
                                .setTitle(R.string.fail_to_share)
                                .setIcon(R.drawable.ic_settings_hotknot)
                                .setMessage(R.string.fail_to_share_conn_error)
                                .setPositiveButton(R.string.try_again, this)
                                .setNegativeButton(android.R.string.cancel, this)
                                .setOnDismissListener(this).create();
        }

        void show() {
            mDialog.show();
        }

        @Override
        public void onDismiss(DialogInterface dialog) {
            if (!mShouldRetry) {
                finish();
            }
        }

        @Override
        public void onClick(DialogInterface dialog, int which) {
            if (which == DialogInterface.BUTTON_POSITIVE) {
                mShouldRetry = true;
            } else {
                finish();
            }
        }
    }

    private BroadcastReceiver mReceiver = new BroadcastReceiver() {
        @Override
        public void onReceive(Context context, Intent intent) {
            String action = intent.getAction();
            if (action == null) {
                return;
            }

            if (action.equals(HotKnotAdapter.ACTION_ADAPTER_STATE_CHANGED)) {
                int state = intent.getIntExtra(HotKnotAdapter.EXTRA_ADAPTER_STATE,
                                    HotKnotAdapter.STATE_DISABLED);
                onHotKnotEnabled(state == HotKnotAdapter.STATE_ENABLED);
            } else if (action.equals(Intent.ACTION_USER_PRESENT)) {
                Log.d(HotKnotCommonUi.TAG, "resume from keyguard");
                // finish();
            }
        }

    };

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        mHandler = new Handler(this);

        setContentView(R.layout.activity_hotknotsend);

        IntentFilter filter = new IntentFilter();
        filter.addAction(HotKnotAdapter.ACTION_ADAPTER_STATE_CHANGED);
        filter.addAction(Intent.ACTION_USER_PRESENT);
        registerReceiver(mReceiver, filter);

        Log.d(HotKnotCommonUi.TAG, "onCreate");
        if (!createHotKnotAdapter(this)) {
            closeCurrentActivity();
        }

        if (!retrieveShareDataFromIntent(getIntent())) {
            closeCurrentActivity();
        }
    }

    @Override
    public void onNewIntent(Intent intent) {
        if (!retrieveShareDataFromIntent(intent)) {
            closeCurrentActivity();
        }
    }

    @Override
    public void onResume() {
        super.onResume();
        if (sAdapter.isEnabled()) {
            registerShareData();
        } else {
            if (mEnabledDialog == null) {
                mEnabledDialog = new EnableDialog(this);
            }
            mEnabledDialog.show(this);
        }
    }

    @Override
    public void onStop() {
        super.onStop();
        Log.d(HotKnotCommonUi.TAG, "onStop");
        if (isApplicationSentToBackground(this)) {
            Log.d(HotKnotCommonUi.TAG, "activity is sent to back");
            finish();
        }
    }

    @Override
    public void onDestroy() {
        super.onDestroy();
        unregisterReceiver(mReceiver);
    }

    @Override
    public void onHotKnotComplete(int reason) {
        Log.d(HotKnotCommonUi.TAG, "onHotKnotComplete, reason = " + reason);
        mHandler.sendMessage(mHandler.obtainMessage(MSG_HOTKNOT_SEND_COMPLETE, reason, 0));
    }

    @Override
    public boolean handleMessage(Message msg) {
        if (msg.what == MSG_HOTKNOT_SEND_COMPLETE) {
            int sendResult = msg.arg1;
            if (sendResult == HotKnotAdapter.ERROR_SUCCESS) {
                finish();
            } else {
                new RetryDialog(this).show();
            }
        } else {
            return false;
        }
        return true;
    }

    public boolean isApplicationSentToBackground(final Context context) {
        ActivityManager am = (ActivityManager) context.getSystemService(Context.ACTIVITY_SERVICE);
        List<RunningTaskInfo> tasks = am.getRunningTasks(1);
        if (tasks == null || tasks.isEmpty()) {
        } else {
            ComponentName topActivity = tasks.get(0).topActivity;
            if (!topActivity.getPackageName().equals(context.getPackageName())) {
                return true;
            }
        }
        return false;
    }

}
