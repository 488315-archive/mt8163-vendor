package com.mediatek.engineermode.atci;

import java.io.IOException;

import android.app.Activity;
import android.content.Intent;
import android.os.Bundle;
import android.os.SystemClock;
import android.os.SystemProperties;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.Button;
import android.widget.Toast;
import android.provider.Settings;
import com.mediatek.engineermode.R;
import com.mediatek.engineermode.Elog;

public class atciActivity extends Activity {
    /** Called when the activity is first created. */

    private static final String TAG = "EM/ATCI";

    private Button mEnableButton;
    private Button mAlwaysEnableButton;
    private Button mDisableButton;

    private static final String ADB_ENABLE = "persist.sys.usb.config";
    private static final String ATCI_USERMODE = "persist.service.atci.usermode";
    private static final String RO_BUILD_TYPE = "ro.build.type";
    private static final String ATCI_AUTO_START = "persist.service.atci.autostart";

    private static final int ENABLE_ONCE = 0;
    private static final int ENABLE_ALWAYS = 1;
    private static final int DISABLE_ATCI = 2;

    private void enable_ATCI(int action) {
        SystemProperties.set(ATCI_USERMODE, "1");
        //SystemProperties.set(ADB_ENABLE, "mass_storage,adb,acm");
        SystemProperties.set("persist.radio.port_index", "1");
        Settings.Global.putInt(getContentResolver(), Settings.Global.ACM_ENABLED, 0);
        Settings.Global.putInt(getContentResolver(), Settings.Global.ACM_ENABLED, 1);

        String type = SystemProperties.get(RO_BUILD_TYPE, "unknown");
        String isAutoStart = SystemProperties.get(ATCI_AUTO_START);

        Elog.v(TAG, "action: " + action + ", build type: " + type);
        if (!type.equals("eng")) {
            try {
                if (DISABLE_ATCI == action) {
                    Process proc = Runtime.getRuntime().exec("stop atcid-daemon-u");
                    Elog.v(TAG, "stop atci_service");
                    proc = Runtime.getRuntime().exec("stop atci_service");
                    Toast.makeText(atciActivity.this, "Disable ATCI Success",
                            Toast.LENGTH_LONG).show();
                    return;
                } else if (ENABLE_ALWAYS == action) {
                    if (isAutoStart.equals("1") == true) {
                        Elog.v(TAG, "atuo start enabled.");
                        Toast.makeText(atciActivity.this, "You have enabled ATCI",
                                Toast.LENGTH_LONG).show();
                        return;
                    } else {
                        SystemProperties.set(ATCI_AUTO_START, "1");
                        Toast.makeText(atciActivity.this, "Always enable ATCI Success",
                                Toast.LENGTH_LONG).show();
                    }
                } else if (ENABLE_ONCE == action) {
                    SystemProperties.set(ATCI_AUTO_START, "0");
                } else {
                    Elog.v(TAG, "unknown action.");
                    return;
                }
                Elog.v(TAG, "start atcid-daemon-u");
                Process proc = Runtime.getRuntime().exec("start atcid-daemon-u");
                Elog.v(TAG, "start atci_service");
                proc = Runtime.getRuntime().exec("start atci_service");
            } catch (IOException e) {
                e.printStackTrace();
            }
        } else {
            Toast.makeText(atciActivity.this, "Eng load, ATCI is enabled",
                    Toast.LENGTH_LONG).show();
        }

        Intent intent = new Intent("com.mediatek.atci.service.startup");
        sendBroadcast(intent);
        Toast.makeText(atciActivity.this, "Enable ATCI Success", Toast.LENGTH_LONG).show();
    }

    private OnClickListener mEnableListener = new Button.OnClickListener() {

        @Override
        public void onClick(View v) {
            enable_ATCI(ENABLE_ONCE);
        }
    };

    private OnClickListener mAlwaysEnableListener = new Button.OnClickListener() {

        @Override
        public void onClick(View v) {
            enable_ATCI(ENABLE_ALWAYS);
        }
    };

    private OnClickListener mDisableListener = new Button.OnClickListener() {

        @Override
        public void onClick(View v) {
            enable_ATCI(DISABLE_ATCI);
        }
    };

    protected void findViews() {
        this.mEnableButton = (Button) this.findViewById(R.id.ATCI_enable);
        this.mAlwaysEnableButton = (Button) this.findViewById(R.id.ATCI_enable_always);
        this.mDisableButton = (Button) this.findViewById(R.id.ATCI_disable);
    }

    protected void setActionListener() {
        this.mEnableButton.setOnClickListener(this.mEnableListener);
        this.mAlwaysEnableButton.setOnClickListener(this.mAlwaysEnableListener);
        this.mDisableButton.setOnClickListener(this.mDisableListener);
    }

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.atci);
        this.findViews();
        this.setActionListener();
    }

}
