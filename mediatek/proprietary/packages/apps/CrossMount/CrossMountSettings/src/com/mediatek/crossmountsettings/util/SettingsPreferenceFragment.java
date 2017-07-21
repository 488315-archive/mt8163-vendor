package com.mediatek.crossmountsettings.util;

import android.app.Dialog;
import android.app.DialogFragment;
import android.app.Fragment;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.ContentResolver;
import android.content.DialogInterface;
import android.content.Intent;
import android.content.IntentFilter;
import android.content.pm.PackageManager;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.preference.Preference;
import android.preference.PreferenceActivity;
import android.preference.PreferenceFragment;
import android.preference.PreferenceGroup;
import android.util.Log;

import com.mediatek.crossmount.adapter.CrossMountAdapter;
import com.mediatek.crossmount.adapter.CrossMountAdapter.FeatureState;
import com.mediatek.crossmount.adapter.Device;
import com.mediatek.crossmountsettings.CrossMountApplication;
import com.mediatek.crossmountsettings.R;
import com.mediatek.crossmountsettings.ui.CrossMountProgressCategory;

/**
 * Base class for Settings fragments, with some helper functions and dialog
 * management.
 */
public class SettingsPreferenceFragment extends PreferenceFragment
             implements DialogCreatable , CrossMountAdapter.CallBack {

    private static final String TAG = "CrossMountSettings";

    private SettingsDialogFragment mDialogFragment;
    public CrossMountAdapter mCrossAdapter;
    public Context mContext;
    private CrossMountApplication mApp;
    public PreferenceGroup mDescCategory;

    public static final int MSG_FEATURE_STATE_UPDATE = 1;
    public static final int MSG_CONNECT_UPDATE = 2;
    public static final int MSG_DISCOVER_COMPLETE = 3;
    public static final int MSG_PROVIDE_SERVICE_UPDATE = 4;
    public static final int MSG_PLUGIN_SERVICE_ICON_READY = 5;
    public static final int MSG_TRUST_DEVICE_UPDATE = 6;

/**
 * remove preference.
 * @param key String
 */
    protected void removePreference(String key) {
        Preference pref = findPreference(key);
        if (pref != null) {
            getPreferenceScreen().removePreference(pref);
        }
    }

    /**
     * The name is intentionally made different from Activity#finish().
     *  so that users won't misunderstand its meaning.
     */
    public final void finishFragment() {
        getActivity().onBackPressed();
    }

    /**
     * Returns the ContentResolver from the owning Activity.
     */
    protected ContentResolver getContentResolver() {
        return getActivity().getContentResolver();
    }

    /**
     * Returns the specified system service from the owning Activity.
     */
    protected Object getSystemService(final String name) {
        return getActivity().getSystemService(name);
    }

    /**
     * Returns the PackageManager from the owning Activity.
     */
    protected PackageManager getPackageManager() {
        return getActivity().getPackageManager();
    }

    @Override
    public void onDetach() {
        if (isRemoving()) {
            if (mDialogFragment != null) {
                mDialogFragment.dismiss();
                mDialogFragment = null;
            }
        }
        super.onDetach();
    }

    @Override
    public void onCreate(Bundle icicle) {
        super.onCreate(icicle);
        mContext = getActivity();
        mCrossAdapter = CrossMountAdapter.getDefaultAdapter(mContext, null);
         if (mCrossAdapter == null) {
            Log.e(TAG, "mCrossAdapter is null,finish");
            getActivity().finish();
            return;
         }
         setHasOptionsMenu(true);
    }


    @Override
    public void onActivityCreated(Bundle savedInstanceState) {
        super.onActivityCreated(savedInstanceState);
    }

    @Override
    public void onStart() {
        super.onStart();
        mApp = (CrossMountApplication) getActivity().getApplication();
        mApp.addActiveActivity();
        // if enable ,start discover
        boolean isDiscovering = mCrossAdapter.isDiscovering();
        Log.d(TAG, "onStart(),count+1 , isDiscovering = " + isDiscovering);
        if (isEnabled() && !isDiscovering) {
            Log.d(TAG, "onStart() startDiscover");
            mCrossAdapter.startDiscover(null);
        }
    }


    @Override
    public void onStop() {
        super.onStop();
        mApp.deCreaseActiveActivity();
        int count = mApp.getActiveActivityCount();
        Log.d(TAG, "onStop(),count = " + count);
        if (count == 0) {
          Log.d(TAG, "all activities is onStop , stop discover");
          mCrossAdapter.stopDiscover();
        }
    }

    @Override
    public void onResume() {
        super.onResume();
        Log.d(TAG, "onResume,set CallBack");
        mCrossAdapter.setCallBack(this);
        // register receiver to update device name
        IntentFilter filter = new IntentFilter();
        filter.addAction(Utils.LOCALE_DEVICE_RENAME_ACTION);
        getActivity().registerReceiver(mReceiver, filter);
    }

    @Override
    public void onPause() {
        super.onPause();
        Log.d(TAG, "onPause,release CallBack");
        getActivity().unregisterReceiver(mReceiver);
        mCrossAdapter.releaseCallBack(this);
    }

    @Override
    public void onDestroy() {
        super.onDestroy();
        Log.d(TAG, "onDestory()");
        if (mDescCategory != null) {
            mDescCategory.removeAll();
        }
    }

    @Override
    public void onFeatureStateUpdate(FeatureState featureState) {
      Log.d(TAG, "onFeatureStateUpdate, featureState = " + featureState);
      mMainHandler.sendEmptyMessage(MSG_FEATURE_STATE_UPDATE);
    }
    @Override
    public void onDiscoverResultUpdate(int reason) {
        Log.d(TAG, "onDiscoverResultUpdate,refresh UI as reason = " + reason);
        mMainHandler.sendEmptyMessage(MSG_DISCOVER_COMPLETE);
    }

    @Override
    public void onConnectionResultUpdate() {
        Log.d(TAG, "onConnectionResultUpdate");
        mMainHandler.sendEmptyMessage(MSG_CONNECT_UPDATE);
    }

    @Override
    public void onTrustDeviceUpdated() {
        Log.d(TAG, "onTrustDeviceUpdated()");
        mMainHandler.sendEmptyMessage(MSG_TRUST_DEVICE_UPDATE);
    }

    @Override
    public void onMyProviderServicesUpdate() {
        Log.d(TAG, "onMyProviderServicesUpdate()");
        mMainHandler.sendEmptyMessage(MSG_PROVIDE_SERVICE_UPDATE);
    }

    @Override
    public void onPlugInServiceIconReady() {
        Log.d(TAG, "onPlugInServiceIconReady()");
        mMainHandler.sendEmptyMessage(MSG_PLUGIN_SERVICE_ICON_READY);
    }

    /**
     * Handler to update UI.
     */
     private final MainHandler mMainHandler = new MainHandler();
     /**
      * Handler implements.
      */
    class MainHandler extends Handler {
        public void handleMessage(Message msg) {
            super.handleMessage(msg);
            Log.d(TAG, "handleMessage, msg = " + msg.what);
            switch (msg.what) {
            case MSG_FEATURE_STATE_UPDATE:
                stateUpdate();
                break;
            case MSG_CONNECT_UPDATE:
                connectUpdate();
                break;
            case MSG_DISCOVER_COMPLETE:
                discoverComplete();
                break;
            case MSG_PROVIDE_SERVICE_UPDATE:
                provideServiceUpdate();
                break;
            case MSG_PLUGIN_SERVICE_ICON_READY:
                pluginServiceIconReady();
                break;
             case MSG_TRUST_DEVICE_UPDATE:
                trustUpdate();
                break;
             default:
                 break;
            }
        }
    }

    /**
     * update UI as feature state update.
     */
    public void stateUpdate() {
        if (!isEnabled()) {
            Log.d(TAG, "not enabled,finish itself");
            getActivity().finish();
        }
    }

    /**
     * update UI as connection state update.
     */
    public void connectUpdate() {

    }

    /**
     * update UI as discovery complete.
     */
    public void discoverComplete() {

    }
    /**
     * update UI as plugin service icon can be found.
     */
    public void pluginServiceIconReady() {

    }

    /**
     * update UI as plugin sevice state update.
     */
    public void provideServiceUpdate() {

    }

    /**
     * update UI as trust manage update.
     */
    public void trustUpdate() {

    }

    /**
     * update UI as device name changed.
     * @param enabled CrossMount state
     */
    public void refreshAsRename(boolean enabled) {

    }

    /**
     * Create the category to describe the ui.
     */
    public void createDescriptionCategory() {
        // Available service category
        if (mDescCategory == null) {
            mDescCategory = new CrossMountProgressCategory(mContext);
            mDescCategory.setSelectable(false);
        } else {
            mDescCategory.removeAll();
        }
        ((ProgressCategory) mDescCategory).setProgress(false);
    }
    /**
     * add preference category.
     * @param preferenceGroup PreferenceGroup
     * @param titleId group's title
     */
    public void addDeviceCategory(PreferenceGroup preferenceGroup, int titleId) {
        preferenceGroup.setTitle(titleId);
        getPreferenceScreen().addPreference(preferenceGroup);
        preferenceGroup.setEnabled(true);
    }

    /**
     * BroadcastReceiver to update UI.
     */
    private final BroadcastReceiver mReceiver = new BroadcastReceiver() {
        @Override
        public void onReceive(Context context, Intent intent) {
            String action = intent.getAction();
            Log.d(TAG, "onReceive,action = " + action);
            if (action.equals(Utils.LOCALE_DEVICE_RENAME_ACTION)) {
                refreshAsRename(isEnabled());
            }
        }
    };
    /**judge CrossMount state , is ENABLED or not.
     * @return true , is enabled, false, not enabled
     * */
    public boolean isEnabled() {
    	boolean enabled = false;
    	if (mCrossAdapter == null) {
    		Log.d(TAG,"isEnabled(), return false");
    		return enabled;
    	}
        FeatureState featureState = mCrossAdapter.getFeatureState();
        Log.d(TAG, "isEnabled(), state = " + featureState);
        enabled = FeatureState.ENABLED == featureState;
        return enabled;
    }

    // Dialog management
    protected void showDialog(int dialogId) {
        if (mDialogFragment != null) {
            Log.e(TAG, "Old dialog fragment not null!");
        }
        mDialogFragment = new SettingsDialogFragment(this, dialogId);
        mDialogFragment.show(getActivity().getFragmentManager(), Integer.toString(dialogId));
    }

    public Dialog onCreateDialog(int dialogId) {
        return null;
    }

    protected void removeDialog(int dialogId) {
        // mDialogFragment may not be visible yet in parent fragment's
        // onResume().
        // To be able to dismiss dialog at that time, don't check
        // mDialogFragment.isVisible().
        if (mDialogFragment != null && mDialogFragment.getDialogId() == dialogId) {
            mDialogFragment.dismiss();
        }
        mDialogFragment = null;
    }

    /**
     * judge dialog is showing or not.
     * @param  dialogId the id of dialog
     * @return The dialog with id is showing
     */
    public boolean isDialogShowing(int dialogId) {
        if (mDialogFragment == null || mDialogFragment.getDialogId() != dialogId) {
            return false;
        }

        Dialog dialog = mDialogFragment.getDialog();
        if (dialog == null) {
            return false;
        }
        return dialog.isShowing();
    }

    /**
     * Control whether the shown Dialog is cancelable.
     *
     * @param cancelable
     *            If true, the dialog is cancelable. The default is true.
     */
    protected void setCancelable(boolean cancelable) {
        if (mDialogFragment != null) {
            mDialogFragment.setCancelable(cancelable);
        }
    }

    /**
     * Sets the OnCancelListener of the dialog shown. This method can only be
     * called after showDialog(int) and before removeDialog(int). The method
     * does nothing otherwise.
     */
    protected void setOnCancelListener(DialogInterface.OnCancelListener listener) {
        if (mDialogFragment != null) {
            mDialogFragment.mOnCancelListener = listener;
        }
    }

    /**
     * Sets the OnDismissListener of the dialog shown. This method can only be
     * called after showDialog(int) and before removeDialog(int). The method
     * does nothing otherwise.
     */
    protected void setOnDismissListener(DialogInterface.OnDismissListener listener) {
        if (mDialogFragment != null) {
            mDialogFragment.mOnDismissListener = listener;
        }
    }

    public void onDialogShowing() {
        // override in subclass to attach a dismiss listener, for instance
    }

    public static class SettingsDialogFragment extends DialogFragment {
        private static final String KEY_DIALOG_ID = "key_dialog_id";
        private static final String KEY_PARENT_FRAGMENT_ID = "key_parent_fragment_id";

        private int mDialogId;

        private Fragment mParentFragment;

        private DialogInterface.OnCancelListener mOnCancelListener;
        private DialogInterface.OnDismissListener mOnDismissListener;

        public SettingsDialogFragment() {
            /* do nothing */
        }

        public SettingsDialogFragment(DialogCreatable fragment, int dialogId) {
            mDialogId = dialogId;
            if (!(fragment instanceof Fragment)) {
                throw new IllegalArgumentException("fragment argument must be an instance of "
                        + Fragment.class.getName());
            }
            mParentFragment = (Fragment) fragment;
        }

        @Override
        public void onSaveInstanceState(Bundle outState) {
            super.onSaveInstanceState(outState);
            if (mParentFragment != null) {
                outState.putInt(KEY_DIALOG_ID, mDialogId);
                outState.putInt(KEY_PARENT_FRAGMENT_ID, mParentFragment.getId());
            }
        }

        @Override
        public void onStart() {
            super.onStart();

            if (mParentFragment != null && mParentFragment instanceof SettingsPreferenceFragment) {
                ((SettingsPreferenceFragment) mParentFragment).onDialogShowing();
            }
        }

        @Override
        public Dialog onCreateDialog(Bundle savedInstanceState) {
            if (savedInstanceState != null) {
                mDialogId = savedInstanceState.getInt(KEY_DIALOG_ID, 0);
                int mParentFragmentId = savedInstanceState.getInt(KEY_PARENT_FRAGMENT_ID, -1);
                if (mParentFragmentId > -1) {
                    mParentFragment = getFragmentManager().findFragmentById(mParentFragmentId);
                    if (!(mParentFragment instanceof DialogCreatable)) {
                        throw new IllegalArgumentException(KEY_PARENT_FRAGMENT_ID
                                + " must implement " + DialogCreatable.class.getName());
                    }
                }
                // This dialog fragment could be created from
                // non-SettingsPreferenceFragment
                if (mParentFragment instanceof SettingsPreferenceFragment) {
                    // restore mDialogFragment in mParentFragment
                    ((SettingsPreferenceFragment) mParentFragment).mDialogFragment = this;
                }
            }
            return ((DialogCreatable) mParentFragment).onCreateDialog(mDialogId);
        }

        @Override
        public void onCancel(DialogInterface dialog) {
            super.onCancel(dialog);
            if (mOnCancelListener != null) {
                mOnCancelListener.onCancel(dialog);
            }
        }

        @Override
        public void onDismiss(DialogInterface dialog) {
            super.onDismiss(dialog);
            if (mOnDismissListener != null) {
                mOnDismissListener.onDismiss(dialog);
            }
        }

        public int getDialogId() {
            return mDialogId;
        }

        @Override
        public void onDetach() {
            super.onDetach();

            // This dialog fragment could be created from
            // non-SettingsPreferenceFragment
            if (mParentFragment instanceof SettingsPreferenceFragment) {
                // in case the dialog is not explicitly removed by
                // removeDialog()
                if (((SettingsPreferenceFragment) mParentFragment).mDialogFragment == this) {
                    ((SettingsPreferenceFragment) mParentFragment).mDialogFragment = null;
                }
            }
        }
    }

    public void finish() {
        getActivity().onBackPressed();
    }

    public boolean startFragment(Fragment caller, String fragmentClass, int requestCode,
            Bundle extras) {
        if (getActivity() instanceof PreferenceActivity) {
            PreferenceActivity preferenceActivity = (PreferenceActivity) getActivity();
            preferenceActivity.startPreferencePanel(fragmentClass, extras,
                    R.string.app_name, null, caller, requestCode);
            return true;
        } else {
            Log.w(TAG, "Parent isn't PreferenceActivity, thus there's no way to launch the "
                    + "given Fragment (name: " + fragmentClass + ", requestCode: " + requestCode
                    + ")");
            return false;
        }
    }

    /*
     * start fragment with set resource
     *
     * @return
     */
    public boolean startFragment(Fragment caller, String fragmentClass, int requestCode,
            Bundle extras, int resID) {
        if (getActivity() instanceof PreferenceActivity) {
            PreferenceActivity preferenceActivity = (PreferenceActivity) getActivity();
            preferenceActivity.startPreferencePanel(fragmentClass, extras, resID, null, caller,
                    requestCode);
            return true;
        } else {
            Log.w(TAG, "Parent isn't PreferenceActivity, thus there's no way to launch the "
                    + "given Fragment (name: " + fragmentClass + ", requestCode: " + requestCode
                    + ")");
            return false;
        }
    }

}
