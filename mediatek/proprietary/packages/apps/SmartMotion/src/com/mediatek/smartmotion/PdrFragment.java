package com.mediatek.smartmotion;

import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.PrintStream;

import com.mediatek.smartmotion.enabler.PdrEnabler;
import com.mediatek.smartmotion.enabler.PedometerEnabler;
import com.mediatek.smartmotion.enabler.Preferences;
import com.mediatek.smartmotion.sensor.SensorHubClient;
import com.mediatek.smartmotion.utils.LogUtils;

import android.R.integer;
import android.app.Activity;
import android.app.AlertDialog;
import android.content.DialogInterface;
import android.os.Bundle;
import android.preference.PreferenceFragment;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.Button;
import android.widget.CheckBox;
import android.widget.EditText;

public class PdrFragment extends ParentFragment{
    private static final String TAG = "PdrFragment";
    private EditText mStepLengthEdit;
    private Button mSetButton;

    @Override
    public void onActivityCreated(Bundle savedInstanceState) {
        super.onActivityCreated(savedInstanceState);
        PdrEnabler.registerSwitch(mActivity, mActionBarSwitch);
    }

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container,
            Bundle savedInstanceState) {
        View view = inflater.inflate(R.layout.pdr_fragment, container, false);
        mStepLengthEdit = (EditText)view.findViewById(R.id.stepLength);
        mSetButton = (Button)view.findViewById(R.id.set);

        // Set step length
        mSetButton.setOnClickListener(new View.OnClickListener() {

            @Override
            public void onClick(View arg0) {
                if (mStepLengthEdit.getText().toString().isEmpty()) {
                    AlertDialog.Builder builder = new AlertDialog.Builder(getActivity());
                    builder.setMessage(R.string.pdr_alert_message);
                    builder.setPositiveButton(R.string.yes, new DialogInterface.OnClickListener() {

                        @Override
                        public void onClick(DialogInterface arg0, int arg1) {
                            // Do nothing, use the default steplength
                        }
                    });
                    builder.setNegativeButton(R.string.no, null);
                    AlertDialog diag = builder.create();
                    diag.show();
                } else {
                    int stepLength = Integer.valueOf(mStepLengthEdit.getText().toString());
                    setStepLength(stepLength);
                }
            }
        });

        return view;
    }

    private void setStepLength(int stepLength) {
        String filePath = "/sys/class/misc/m_step_c_misc/step_length";
        try {
            FileOutputStream out = new FileOutputStream(new File(filePath));
            PrintStream ps = new PrintStream(out);
            ps.print(stepLength);
            out.close();
        } catch (FileNotFoundException e) {
            e.printStackTrace();
            LogUtils.d(TAG, "setStepLength, can't find file " + filePath);
        } catch (IOException e) {
            e.printStackTrace();
            LogUtils.d(TAG, "setStepLength, IOException happens. ");
        }
    }

    @Override
    public void onDestroy() {
        LogUtils.d(TAG, "onDestroy");
        super.onDestroy();
        PdrEnabler.unregisterSwitch(mActionBarSwitch);
    }

}
