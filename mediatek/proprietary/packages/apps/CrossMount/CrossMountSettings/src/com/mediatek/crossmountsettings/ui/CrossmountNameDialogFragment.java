package com.mediatek.crossmountsettings.ui;

import android.app.AlertDialog;
import android.app.Dialog;
import android.app.DialogFragment;
import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.os.Bundle;
import android.text.Editable;
import android.text.InputFilter;
import android.text.TextWatcher;
import android.util.Log;
import android.view.KeyEvent;
import android.view.LayoutInflater;
import android.view.View;
import android.view.WindowManager;
import android.view.inputmethod.EditorInfo;
import android.widget.Button;
import android.widget.EditText;
import android.widget.TextView;

import com.mediatek.crossmount.adapter.CrossMountAdapter;
import com.mediatek.crossmount.adapter.Device;

import com.mediatek.crossmountsettings.R;
import com.mediatek.crossmountsettings.util.Utf8ByteLengthFilter;
import com.mediatek.crossmountsettings.util.Utils;

/**
 * Dialog fragment for renaming the local Crossmount device.
 */
public final class CrossmountNameDialogFragment extends DialogFragment implements TextWatcher {
    private int mMaxLength;
    String mTmp = "";
    // the filtered string
    String mFilterStr = "@";
    private AlertDialog mAlertDialog;
    private Button mOkButton;

    // accessed from inner class (not private to avoid thunks)
    static final String TAG = "CrossmountRename";
    private CrossMountAdapter mCrossAdapter;
    private Device mSelfDevice;
    EditText mDeviceNameView;

    // This flag is set when the name is updated by code, to distinguish from user changes
    private boolean mDeviceNameUpdated;

    // This flag is set when the user edits the name (preserved on rotation)
    private boolean mDeviceNameEdited;
    // Key to save the edited name and edit status for restoring after rotation
    private static final String KEY_NAME = "device_name";
    private static final String KEY_NAME_EDITED = "device_name_edited";

/**
 * Constructor, the rename dialog fragment.
 */
    public CrossmountNameDialogFragment() {
        Log.d(TAG, "constructor");
    }

    @Override
    public Dialog onCreateDialog(Bundle savedInstanceState) {
        Log.d(TAG, "getActivity() is " + getActivity());
        mCrossAdapter  = CrossMountAdapter.getDefaultAdapter(getActivity(), null) ;
        if (mCrossAdapter == null) {
            Log.e(TAG, "mCrossAdapter is null");
            return null;
        }
        mSelfDevice =  mCrossAdapter.getMyDevice();
        mMaxLength = mSelfDevice.getMaxNameLength();
        Log.d(TAG, "mMaxLength = " + mMaxLength);
        String deviceName = mSelfDevice.getName();
        if (savedInstanceState != null) {
            deviceName = savedInstanceState.getString(KEY_NAME, deviceName);
            mDeviceNameEdited = savedInstanceState.getBoolean(KEY_NAME_EDITED, false);
        }
        mAlertDialog = new AlertDialog.Builder(getActivity())
                .setTitle(R.string.rename_device_menu_item)
                .setView(createDialogView(deviceName))
                .setPositiveButton(R.string.bluetooth_rename_button,
                        new DialogInterface.OnClickListener() {
                            public void onClick(DialogInterface dialog, int which) {
                                String deviceName = mDeviceNameView.getText().toString();
                                setDeviceName(deviceName);
                            }
                        })
                .setNegativeButton(android.R.string.cancel, null)
                .create();
        mAlertDialog.getWindow().setSoftInputMode(
                WindowManager.LayoutParams.SOFT_INPUT_STATE_ALWAYS_VISIBLE);

        return mAlertDialog;
    }

    /**
     * Set device's name.
     * @param deviceName the new device name.
     */
    private void setDeviceName(String deviceName) {
        Log.d(TAG, "change device name to " + deviceName);
        // if rename successfully, send broadcast
        if (mSelfDevice.setName(deviceName)) {
            Log.d(TAG, "change successfully,send broadcast");
            Intent intent = new Intent(Utils.LOCALE_DEVICE_RENAME_ACTION);
            getActivity().sendBroadcast(intent);
        }
    }

    @Override
    public void onSaveInstanceState(Bundle outState) {
        outState.putString(KEY_NAME, mDeviceNameView.getText().toString());
        outState.putBoolean(KEY_NAME_EDITED, mDeviceNameEdited);
    }

    /**
     * Create dialog's view.
     * @param deviceName device name.
     * @return View
     */
    private View createDialogView(String deviceName) {
        final LayoutInflater layoutInflater = (LayoutInflater)getActivity()
            .getSystemService(Context.LAYOUT_INFLATER_SERVICE);
        View view = layoutInflater.inflate(R.layout.dialog_edittext, null);
        mDeviceNameView = (EditText) view.findViewById(R.id.edittext);
        mDeviceNameView.setFilters(new InputFilter[] {
                new Utf8ByteLengthFilter(mMaxLength)
        });
        mDeviceNameView.setText(deviceName);    // set initial value before adding listener
        mDeviceNameView.addTextChangedListener(this);
        mDeviceNameView.setOnEditorActionListener(new TextView.OnEditorActionListener() {
            @Override
            public boolean onEditorAction(TextView v, int actionId, KeyEvent event) {
                if (actionId == EditorInfo.IME_ACTION_DONE) {
                    setDeviceName(v.getText().toString());
                    mAlertDialog.dismiss();
                    return true;    // action handled
                } else {
                    return false;   // not handled
                }
            }
        });
        return view;
    }

    @Override
    public void onDestroy() {
        super.onDestroy();
        mAlertDialog = null;
        mDeviceNameView = null;
        mOkButton = null;
    }

    @Override
    public void onResume() {
        super.onResume();
        if (mOkButton == null) {
            mOkButton = mAlertDialog.getButton(DialogInterface.BUTTON_POSITIVE);
            mOkButton.setEnabled(mDeviceNameEdited);
        }
    }

    /**
     * Response text's change.
     * @param s Editable
     */
    public void afterTextChanged(Editable s) {
        if (mDeviceNameUpdated) {
            mDeviceNameUpdated = false;
            mOkButton.setEnabled(false);
        } else {
            mDeviceNameEdited = true;
            if (mOkButton != null) {
                mOkButton.setEnabled(s.length() != 0);
            }
        }

        // filter the special character, as it's used by other.
        String str = s.toString();
        if (str.equals(mTmp)) {
            return;
            }
        StringBuffer sb = new StringBuffer();
        for (int i = 0; i < str.length(); i++) {
            if (mFilterStr.indexOf(str.charAt(i)) < 0) {
                sb.append(str.charAt(i));
            }
        }
        mTmp = sb.toString();
        mDeviceNameView.setText(mTmp);
        mDeviceNameView.setSelection(mTmp.length());
    }

    /* Not used */
    public void beforeTextChanged(CharSequence s, int start, int count, int after) {
    }

    /* Not used */
    public void onTextChanged(CharSequence s, int start, int before, int count) {
    }
}
