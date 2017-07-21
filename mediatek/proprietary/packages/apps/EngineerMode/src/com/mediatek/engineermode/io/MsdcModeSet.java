package com.mediatek.engineermode.io;

import android.content.res.Resources;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.AdapterView;
import android.widget.AdapterView.OnItemSelectedListener;
import android.widget.ArrayAdapter;
import android.widget.Button;
import android.widget.Spinner;
import android.widget.TextView;

import com.mediatek.engineermode.R;
import com.mediatek.engineermode.ShellExe;

import java.io.IOException;

/**
 * Class for MSDC mode set
 *
 */
public class MsdcModeSet extends MsdcTest implements OnClickListener  {

    private static final String TAG = "MsdcModeSet";
    private static final String CMD_SET_PRE = "echo b 1 ";
    private static final String CMD_SPACE = " ";
    private static final String CMD_WRITE_TO = ">";
    private static final String CMD_TAR_FILE = "/proc/msdc_debug";
    private static final String CMD_READ_RESULT = "cat /proc/msdc_debug";
    private static final String SPEED_MODE_HS400 = "a";
    private Button mBtnSet;
    private TextView mTvResult;

    private Spinner mHostIdSpinner;
    private int mHostIdIndex = 0;

    private Spinner mSpeedModeSpinner;
    private int mSpeedModeIndex = 0;

    private Spinner mCmdQueueSpinner;
    private int mCmdQueueIndex = 0;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        // TODO Auto-generated method stub
        super.onCreate(savedInstanceState);
        setContentView(R.layout.msdc_mode_set);

        mBtnSet = (Button) findViewById(R.id.mode_set_btn);
        mTvResult = (TextView) findViewById(R.id.result_text);

        mHostIdSpinner = (Spinner) findViewById(R.id.host_id_sppiner);

        mSpeedModeSpinner = (Spinner) findViewById(R.id.speed_mode_spinner);
        mCmdQueueSpinner = (Spinner) findViewById(R.id.cmd_queue_spinner);

        mBtnSet.setOnClickListener(this);

        Resources res = getResources();
        ArrayAdapter<String> hostIdAdapter = new ArrayAdapter<String>(this,
                android.R.layout.simple_spinner_item, res
                .getStringArray(R.array.host_id_number));
        hostIdAdapter
                .setDropDownViewResource(android.R.layout.simple_spinner_dropdown_item);
        mHostIdSpinner.setAdapter(hostIdAdapter);

        mHostIdSpinner.setOnItemSelectedListener(mSpinnerListener);

        ArrayAdapter<String> speedModeAdapter = new ArrayAdapter<String>(this,
                android.R.layout.simple_spinner_item, res
                        .getStringArray(R.array.msdc_speed_mode));
        speedModeAdapter
                .setDropDownViewResource(android.R.layout.simple_spinner_dropdown_item);

        mSpeedModeSpinner.setAdapter(speedModeAdapter);
        mSpeedModeSpinner.setOnItemSelectedListener(mSpinnerListener);

        ArrayAdapter<String> cmdQueueAdapter = new ArrayAdapter<String>(this,
                android.R.layout.simple_spinner_item, res
                        .getStringArray(R.array.msdc_cmd_queue));
        cmdQueueAdapter
                .setDropDownViewResource(android.R.layout.simple_spinner_dropdown_item);

        mCmdQueueSpinner.setAdapter(cmdQueueAdapter);
        mCmdQueueSpinner.setOnItemSelectedListener(mSpinnerListener);
    }

    @SuppressWarnings("deprecation")
    @Override
    public void onClick(View v) {
        // TODO Auto-generated method stub
        String strSpeedMode = (mSpeedModeIndex == 10)
                ? SPEED_MODE_HS400 : String.valueOf(mSpeedModeIndex);
        StringBuilder strCmd = new StringBuilder(CMD_SET_PRE);
        strCmd.append(mHostIdIndex).append(CMD_SPACE).append(strSpeedMode).
        append(CMD_SPACE).append(mCmdQueueIndex).append(CMD_SPACE)
        .append(CMD_WRITE_TO).append(CMD_SPACE).append(CMD_TAR_FILE);
        Log.i(TAG, "Set CMD: " + strCmd);
        try {
            if (ShellExe.execCommand(strCmd.toString()) != ShellExe.RESULT_SUCCESS) {
                showDialog(EVENT_SET_FAIL_ID);
            }
            if (ShellExe.execCommand(CMD_READ_RESULT) == ShellExe.RESULT_SUCCESS) {
                String result = ShellExe.getOutput().trim();
                mTvResult.setText(result);
            } else {
                showDialog(EVENT_READ_RESULT_FAIL_ID);
            }
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    private final OnItemSelectedListener mSpinnerListener = new OnItemSelectedListener() {

        public void onItemSelected(AdapterView<?> arg0, View arg1, int arg2,
                long arg3) {
            switch (arg0.getId()) {
            case R.id.host_id_sppiner:
                mHostIdIndex = arg2;
                break;
            case R.id.speed_mode_spinner:
                mSpeedModeIndex = arg2;
                break;
            case R.id.cmd_queue_spinner:
                mCmdQueueIndex = arg2;
                break;
            default:
                break;
            }
        }

        public void onNothingSelected(AdapterView<?> arg0) {
        }

    };

}
