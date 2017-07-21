/* Copyright Statement:
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein is
 * confidential and proprietary to MediaTek Inc. and/or its licensors. Without
 * the prior written permission of MediaTek inc. and/or its licensors, any
 * reproduction, modification, use or disclosure of MediaTek Software, and
 * information contained herein, in whole or in part, shall be strictly
 * prohibited.
 *
 * MediaTek Inc. (C) 2010. All rights reserved.
 *
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE")
 * RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO RECEIVER
 * ON AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL
 * WARRANTIES, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR
 * NONINFRINGEMENT. NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH
 * RESPECT TO THE SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY,
 * INCORPORATED IN, OR SUPPLIED WITH THE MEDIATEK SOFTWARE, AND RECEIVER AGREES
 * TO LOOK ONLY TO SUCH THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO.
 * RECEIVER EXPRESSLY ACKNOWLEDGES THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO
 * OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES CONTAINED IN MEDIATEK
 * SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK SOFTWARE
 * RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S
 * ENTIRE AND CUMULATIVE LIABILITY WITH RESPECT TO THE MEDIATEK SOFTWARE
 * RELEASED HEREUNDER WILL BE, AT MEDIATEK'S OPTION, TO REVISE OR REPLACE THE
 * MEDIATEK SOFTWARE AT ISSUE, OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE
 * CHARGE PAID BY RECEIVER TO MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 *
 * The following software/firmware and/or related documentation ("MediaTek
 * Software") have been modified by MediaTek Inc. All revisions are subject to
 * any receiver's applicable license agreements with MediaTek Inc.
 */

package com.mediatek.engineermode.desenseat;

import android.annotation.SuppressLint;
import android.app.Activity;
import android.app.AlertDialog;
import android.app.Dialog;
import android.content.Context;
import android.content.DialogInterface;
import android.content.DialogInterface.OnClickListener;
import android.content.Intent;
import android.content.SharedPreferences;
import android.location.Location;
import android.location.LocationListener;
import android.location.LocationManager;
import android.os.AsyncTask;
import android.os.Bundle;
import android.provider.Settings;
import android.text.Html;
import android.util.DisplayMetrics;
import android.view.KeyEvent;
import android.view.LayoutInflater;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.view.ViewGroup;
import android.widget.AdapterView;
import android.widget.AdapterView.OnItemClickListener;
import android.widget.ArrayAdapter;
import android.widget.Button;
import android.widget.CheckBox;
import android.widget.CompoundButton;
import android.widget.LinearLayout.LayoutParams;
import android.widget.ListView;
import android.widget.RadioButton;
import android.widget.Spinner;
import android.widget.TableLayout;
import android.widget.TableRow;
import android.widget.TextView;
import android.widget.Toast;

import com.mediatek.engineermode.Elog;
import com.mediatek.engineermode.FeatureSupport;
import com.mediatek.engineermode.ModemCategory;
import com.mediatek.engineermode.R;


import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.text.DateFormat;
import java.text.SimpleDateFormat;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Collections;
import java.util.Comparator;
import java.util.Date;
import java.util.HashMap;
import java.util.List;



/**
 * GPS de-sense auto test Activity.
 */
public class DesenseAtActivity extends Activity implements OnItemClickListener,
        View.OnClickListener, CheckBox.OnCheckedChangeListener {
    protected static final String TAG = "EM/DesenseAT";
    private static final int DIALOG_TEST_ITEMS = 1;
    private static final int DIALOG_CURVE = 2;
    private static final int DIALOG_SV_IDS = 3;
    private static final int DIALOG_ENABLE_GPS = 4;
    private static final int DIALOG_OMIT_WARNING = 5;
    private static final int DIALOG_EXIT = 6;
    private static final String PREF_FILE = "prefs";
    private static final String KEY_ITEM = "item";
    private static final String KEY_MODE = "mode";
    private static final String KEY_SVID = "svid";
    private static final String KEY_SELECTED_ALL = "selectedAll";
    private static final String NEW_LINE = System.getProperty("line.separator");
    private static final CharSequence TESTING =
            Html.fromHtml("<font color='#FFFF00'>Testing</font>");
    private static final int SV_GPS = 0;
    private static final int SV_GLONASS_L = 1;
    private static final int SV_GLONASS_M = 2;
    private static final int SV_GLONASS_H = 3;
    private static final int SV_BEIDOU = 4;
    private static final int MAX_SV_COUNT = 5;
    public static final int INVALID_SV_ID = 0;
    private static final String GSM_BASEBAND = "gsm.baseband.capability";
    private static final int WCDMA = 0x04;
    private static final int TDSCDMA = 0x08;
    private static final int FDD_BAND_MIN = 1;
    private static final int FDD_BAND_MAX = 31;
    private static final int FILE_NUMBER = 5;
    private static final int SV_TYPE_NUMBER = 5;
    private boolean mSelectedAll = false;
    private static boolean sTestCancelled = false;

    // UI components
    private Button mTestItemButton;
    private Button mSvIdButton;
    private Button mStartButton;
    private Button mStopButton;
    private TableLayout mTestModeRadio;
    private ListView mResultList;
    private MyAdapter mAdapter;
    private List<RadioButton> mRadioButtons;
    private ClientSocket mSocketClient = null;
    private boolean mIsRunning = false;

    private int[] mSelectedSvid = new int[MAX_SV_COUNT];
    private List<List<String>> mSvIdLists;

    private List<TestItem> mTestItems;
    private List<TestItem> mSelectedTestItems;
    private List<TestItem> mGenTestItems;
    private List<TestMode> mTestModes;
    public static List<BandItem> sGsmItems;
    public static List<BandItem> sTddItems;
    public static List<BandItem> sFddItems;
    public static List<BandItem> sLteItems;
    public static List<BandItem> sCdmaItems;
    private static HashMap<String, String> sItemsToCheck;
    private List<String> mItemsToOmit = new ArrayList<String>();

    private TestMode mCurrentMode;
    private FunctionTask mTask;
    private HashMap<TestItem, TestResult> mTestResults;
    private LocationManager mLocationManager;
    private boolean mInitGPSState;
    private String mOmitWarning = new String();
    private FileOutputStream mOutputData = null;
    private static final DateFormat DATE_FORMAT = new SimpleDateFormat(
            "yyyyMMddhhmmss");

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.desense_at);

        mTestItems = TestItem.getTestItems(this);
        mTestModes = TestMode.getTestModes();
        mSelectedTestItems = new ArrayList<TestItem>();
        mGenTestItems =  new ArrayList<TestItem>();
        mTestResults = new HashMap<TestItem, TestResult>();

        initRfConfigList();
        initComponents();
        initSvIdLists();
        restoreSettings();
        updateListView();
        enableButtons(true);
        initItemToCheck();
        initResolution();
        mLocationManager = (LocationManager) getSystemService(Context.LOCATION_SERVICE);
        try {
            mLocationManager = (LocationManager) getSystemService(Context.LOCATION_SERVICE);
            if (mLocationManager != null) {
                mLocationManager.requestLocationUpdates(
                        LocationManager.GPS_PROVIDER, 0, 0, mLocListener);

                if (mLocationManager
                        .isProviderEnabled(LocationManager.GPS_PROVIDER)) {
                   Elog.w(TAG, "provider enabled");

                }
            } else {
                Elog.w(TAG, "new mLocationManager failed");
            }
        } catch (SecurityException e) {
            Toast.makeText(this, "security exception", Toast.LENGTH_LONG)
                    .show();
            Elog.w(TAG, "Exception: " + e.getMessage());
        } catch (IllegalArgumentException e) {
            Elog.w(TAG, "Exception: " + e.getMessage());
        }

    }

    @Override
    protected void onDestroy() {
        // TODO Auto-generated method stub
        mLocationManager.removeUpdates(mLocListener);
        super.onDestroy();
    }

    private void initResolution() {
        DisplayMetrics metric = new DisplayMetrics();
        getWindowManager().getDefaultDisplay().getMetrics(metric);

        int width = metric.widthPixels;
        int height = metric.heightPixels;
        Util.setResolution(width, height);
    }

    private void initItemToCheck() {
        sItemsToCheck = new HashMap<String, String>();
        sItemsToCheck.put(new Display3DTest().toString(),
                getResources().getString(R.string.desense_at_no_3d_display_apk));
        Elog.v(TAG, "sItemsToCheck size = " + sItemsToCheck.size());
        sItemsToCheck.put(new CpuFullLoadingTest().toString(),
                getResources().getString(R.string.desense_at_no_3d_display_apk));
        Elog.v(TAG, "sItemsToCheck size = " + sItemsToCheck.size());
        sItemsToCheck.put(new Mp3PlayTest().toString(),
                getResources().getString(R.string.desense_at_no_mp3_warning));
        Elog.v(TAG, "sItemsToCheck size = " + sItemsToCheck.size());
        sItemsToCheck.put(new VideoPlayTest().toString(),
                getResources().getString(R.string.desense_at_no_3gp_warning));
        Elog.v(TAG, "sItemsToCheck size = " + sItemsToCheck.size());
        sItemsToCheck.put(new ExternalSdCardReadTest().toString(),
                getResources().getString(R.string.desense_at_no_sd_warning));
        Elog.v(TAG, "sItemsToCheck size = " + sItemsToCheck.size());
        sItemsToCheck.put(new ExternalSdCardWriteTest().toString(),
                getResources().getString(R.string.desense_at_no_sd_warning));
        Elog.v(TAG, "sItemsToCheck size = " + sItemsToCheck.size());
        Elog.v(TAG, "Display3DTest key = " + new Display3DTest().toString()
                + "value = " + sItemsToCheck.get(new Display3DTest().toString()));
        Elog.v(TAG, "CpuFullLoadingTest key = " + new CpuFullLoadingTest().toString()
                + "value = " + sItemsToCheck.get(new CpuFullLoadingTest().toString()));
    }

    private final LocationListener mLocListener = new LocationListener() {

        // @Override
        public void onLocationChanged(Location location) {
            Elog.v(TAG, "Enter onLocationChanged function");
        }

        // @Override
        public void onProviderDisabled(String provider) {
            Elog.v(TAG, "Enter onProviderDisabled function");
        }

        // @Override
        public void onProviderEnabled(String provider) {
            Elog.v(TAG, "Enter onProviderEnabled function");
        }

        // @Override
        public void onStatusChanged(String provider, int status, Bundle extras) {
            Elog.v(TAG, "Enter onStatusChanged function");
        }
    };

    @SuppressWarnings("deprecation")
    @Override
    public void onClick(View view) {
        if (view == mTestItemButton) {
            showDialog(DIALOG_TEST_ITEMS);
        } else if (view == mSvIdButton) {
            showDialog(DIALOG_SV_IDS);
        } else if (view.getId() == R.id.desense_at_start) {

            Elog.d(TAG, "desense_at_start click: " + mCurrentMode);

            if (mSelectedTestItems.size() <= 0) {
                Toast.makeText(this, R.string.desense_at_test_item_warning,
                        Toast.LENGTH_LONG).show();
                return;
            }
            if (mCurrentMode == null) {
                Toast.makeText(this, R.string.desense_at_test_mode_warning,
                        Toast.LENGTH_LONG).show();
                return;
            }
            if (("Signal".equals(mCurrentMode.toString())) && (!validSvIdSelected())) {
                Toast.makeText(this, R.string.desense_at_sv_id_warning,
                        Toast.LENGTH_LONG).show();
                return;
            }

            showReminderItems();



        } else if (view.getId() == R.id.desense_at_stop) {
            // stop test
            Elog.d(TAG, "cancel click");
            Toast.makeText(this, R.string.desense_at_test_stop_warning,
                    Toast.LENGTH_LONG).show();
            if (mCurrentMode != null) {
                Elog.d(TAG, "task cancel");
                setCancelled(true);
            }
        }
    }

    private void startTest() {
        // clear current result list
        mTestResults.clear();
        mAdapter.notifyDataSetChanged();
        // start test
        mTask = new FunctionTask();
        mTask.execute();
        enableButtons(false);
    }

    @SuppressWarnings("deprecation")
    private void showReminderItems() {

        mItemsToOmit.clear();
        mOmitWarning = this.getResources().getString(R.string.desense_at_test_start_warning_pre);
        StringBuilder strWarning = new StringBuilder("------ Following items will be omitted:\r\n");
        int k = 0;
        boolean hasOmitItem = false;
        for (TestItem testitem: mSelectedTestItems) {
            String strName = testitem.toString();
            String strToast = sItemsToCheck.get(strName);
            Elog.d(TAG, "strName: " + strName);
            Elog.d(TAG, "strToast: " + strToast);
            if ((strToast != null)
                    && (!testitem.doApiTest())) {

                mItemsToOmit.add(strName);
                strWarning.append(++k).append("  ").
                append(strName).append(": ").append(strToast).append("\r\n");
                hasOmitItem = true;
                Elog.d(TAG, "strWarning: " + strWarning);
            }
        }

        if (hasOmitItem) {
            mOmitWarning = mOmitWarning + strWarning.toString();
        }
        removeDialog(DIALOG_OMIT_WARNING);
        showDialog(DIALOG_OMIT_WARNING);
    }

    @SuppressWarnings("deprecation")
    @Override
    public void onItemClick(AdapterView<?> parent, View view, int position, long id) {
        Bundle bundle = new Bundle();
        bundle.putInt("position", position);
        removeDialog(DIALOG_CURVE);
        showDialog(DIALOG_CURVE, bundle);
    }

    @Override
    public void onCheckedChanged(CompoundButton button, boolean checked) {
        if (checked) {
            mCurrentMode = (TestMode) button.getTag();
            // uncheck other radio buttons
            for (RadioButton r : mRadioButtons) {
                if (r != button) {
                    r.setChecked(false);
                }
            }
            saveSettings();
            Elog.d(TAG, "GpsCwMode: " + new GpsCwMode() + "mCurrentMode:" + mCurrentMode);
            if ((new GpsCwMode()).toString().equals(mCurrentMode.toString())) {
                Elog.d(TAG, "mSvIdButton.setEnabled(false): ");
                mSvIdButton.setEnabled(false);
            } else {
                mSvIdButton.setEnabled(true);
            }
        }
    }

    @Override
    protected Dialog onCreateDialog(int id, Bundle bundle) {
        switch (id) {
        case DIALOG_TEST_ITEMS:
            return createTestItemDialog();
        case DIALOG_SV_IDS:
            return createSvIdDialog();
        case DIALOG_CURVE:
            return createCurveDialog(bundle);
        case DIALOG_OMIT_WARNING:
            return new AlertDialog.Builder(this)
                    .setTitle(R.string.desense_at_warning_title)
                    .setMessage(mOmitWarning)
                    .setPositiveButton(android.R.string.yes,
                            new DialogInterface.OnClickListener() {
                            @SuppressWarnings("deprecation")
                            public void onClick(DialogInterface dialog, int which) {
                                startTest();

                            }
                        })
                     .setNegativeButton(android.R.string.no,
                            new DialogInterface.OnClickListener() {
                            @SuppressWarnings("deprecation")
                            public void onClick(DialogInterface dialog, int which) {
                                return;

                            }
                        }).create();
        case DIALOG_EXIT: {
            return new AlertDialog.Builder(this)
            .setTitle(R.string.desense_at_warning_title)
            .setMessage(R.string.desense_at_exit_warning)
            .setPositiveButton(android.R.string.yes,
                    new DialogInterface.OnClickListener() {
                    @SuppressWarnings("deprecation")
                    public void onClick(DialogInterface dialog, int which) {
                        mTask.cancel(true);

                    }
                })
             .setNegativeButton(android.R.string.no,
                    new DialogInterface.OnClickListener() {
                    @SuppressWarnings("deprecation")
                    public void onClick(DialogInterface dialog, int which) {
                        return;

                    }
                }).create();

        }
        case DIALOG_ENABLE_GPS:
            AlertDialog.Builder builder = new AlertDialog.Builder(this);
            builder.setTitle(R.string.desense_at);
            builder.setMessage(R.string.desense_at_enable_gps);
            builder.setPositiveButton(android.R.string.yes,
                    new DialogInterface.OnClickListener() {
                        @SuppressWarnings("deprecation")
                        public void onClick(DialogInterface dialog, int which) {
                            Settings.Secure.setLocationProviderEnabled(
                                    DesenseAtActivity.this.getContentResolver(),
                                    LocationManager.GPS_PROVIDER, true);

                        }
                    });
            builder.setNegativeButton(android.R.string.no,
                    new DialogInterface.OnClickListener() {
                        public void onClick(DialogInterface dialog, int which) {
                            finish();
                        }
                    });
            return builder.create();
        default:
            return null;
        }
    }


    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        // TODO Auto-generated method stub
        menu.add(Menu.NONE, 0, 0, R.string.desense_at_rf_band_config);
        menu.add(Menu.NONE, 1, 0, R.string.desense_at_api_test);
        menu.add(Menu.NONE, 2, 0, R.string.desense_at_show_history);
        return super.onCreateOptionsMenu(menu);
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        switch (item.getItemId()) {
        case 0:
            startActivity(new Intent(this, RfBandConfigActivity.class));
            return true;

        case 1:
            startActivity(new Intent(this, ApiTestActivity.class));
            return true;

        case 2:
            startActivity(new Intent(this, HistoryListActivity.class));
            return true;
        default:
            break;
        }
        return false;
    }

    @SuppressWarnings("deprecation")
    @Override
    public void onBackPressed() {
        // TODO Auto-generated method stub
        if (mIsRunning) {
            showDialog(DIALOG_EXIT);
        }
        super.onBackPressed();
    }

    private void initComponents() {
        mTestItemButton = (Button) findViewById(R.id.desense_at_test_items);
        mTestItemButton.setOnClickListener(this);
        mSvIdButton = (Button) findViewById(R.id.desense_at_sv_ids);
        mSvIdButton.setOnClickListener(this);
        mStartButton = (Button) findViewById(R.id.desense_at_start);
        mStopButton = (Button) findViewById(R.id.desense_at_stop);
        mTestModeRadio = (TableLayout) findViewById(R.id.desense_at_test_modes);
        mAdapter = new MyAdapter(this);
        mResultList = (ListView) findViewById(R.id.desense_at_test_result);
        mResultList.setAdapter(mAdapter);
        mResultList.setOnItemClickListener(this);

        // Radio buttons for test modes
        TableRow row = new TableRow(this);
        mRadioButtons = new ArrayList<RadioButton>();
        for (int i = 0; i < mTestModes.size(); i++) {
            TestMode m = mTestModes.get(i);
            RadioButton button = new RadioButton(this);
            button.setText(m.toString());
            button.setTag(m);
            button.setOnCheckedChangeListener(this);
            TableRow.LayoutParams layoutParams =
                    new TableRow.LayoutParams(0, LayoutParams.WRAP_CONTENT, 1);
            TableLayout.LayoutParams layoutParams2 =
                    new TableLayout.LayoutParams(LayoutParams.MATCH_PARENT,
                            LayoutParams.WRAP_CONTENT);
            if (i % 3 == 0) {
                row = new TableRow(this);
                mTestModeRadio.addView(row, layoutParams2);
            }
            row.addView(button, layoutParams);
            mRadioButtons.add(button);
        }
    }

    private void initSvIdLists() {
        mSvIdLists = new ArrayList<List<String>>();
        for (int i = 0; i < MAX_SV_COUNT; i++) {
            mSvIdLists.add(new ArrayList<String>());
            mSvIdLists.get(i).add("NA");
        }
        // GPS sv IDs: 1 ~ 32
        for (int i = 1; i <= 32; i++) {
            mSvIdLists.get(0).add(String.valueOf(i));
        }
        // Glonass(L) sv IDs
        for (int i = 65; i <= 74; i++) {
            mSvIdLists.get(1).add(String.valueOf(i));
        }
        // Glonass(M) sv IDs
        for (int i = 75; i <= 85; i++) {
            mSvIdLists.get(2).add(String.valueOf(i));
        }
        // Glonass(H) sv IDs
        for (int i = 86; i <= 96; i++) {
            mSvIdLists.get(3).add(String.valueOf(i));
        }
        // Beidou sv IDs
        for (int i = 1; i <= 32; i++) {
            mSvIdLists.get(4).add(String.valueOf(i));
        }
    }

    private void initRfConfigList() {

        BandItem.initDefaultData();
        initGsmItems();
        if (ModemCategory.getModemType() == ModemCategory.MODEM_TD) {
            initTddItems();
        } else if (ModemCategory.getModemType() == ModemCategory.MODEM_FDD) {
            initFddItems();
        }
        if (FeatureSupport.isSupported(FeatureSupport.FK_LTE_SUPPORT)) {
            initLteItems();
        }
        if (ModemCategory.isCdma()) {
            initCdmaItems();
        }
    }

    private void initGsmItems() {
        String[] bandNameList = getResources().getStringArray(R.array.rf_desense_tx_test_gsm_band);
        String[] bandValue = getResources().
                getStringArray(R.array.rf_desense_tx_test_gsm_band_values);

        sGsmItems = new ArrayList<BandItem>();

        int size = bandNameList.length;
        Elog.d(TAG, "sGsmItems: add size = " + size);

        for (int k = 0; k < size; k++) {
            Elog.d(TAG, "sGsmItems: add " + bandNameList[k] + " " + bandValue[k]);
            sGsmItems.add(new BandItem(bandNameList[k], bandValue[k], BandItem.BandType.BAND_GSM));

        }

        for (BandItem item:sGsmItems) {
            Elog.d(TAG, "item content: " + item.getBandName() + " " + item.getSummary());
        }

    }

    private void initTddItems() {
        String[] bandNameList = getResources().getStringArray(R.array.rf_desense_tx_test_td_band);
        String[] bandValue = getResources().
                getStringArray(R.array.rf_desense_tx_test_td_band_values);

        sTddItems = new ArrayList<BandItem>();

        int size = bandNameList.length;
        Elog.d(TAG, "sTddItems: add size = " + size);

        for (int k = 0; k < size; k++) {
             sTddItems.add(new BandItem(bandNameList[k], bandValue[k], BandItem.BandType.BAND_TD));
        }

    }

    private void initFddItems() {
        String[] bandNameList = getResources().getStringArray(R.array.rf_desense_tx_test_fd_band);
        String[] bandValue = getResources().
                getStringArray(R.array.rf_desense_tx_test_fd_band_values);


        sFddItems = new ArrayList<BandItem>();

        int size = bandNameList.length;
        Elog.d(TAG, "sFddItems: add size = " + size);

        for (int k = 0; k < size; k++) {
            sFddItems.add(new BandItem(
                    "WCDMA " + bandNameList[k], bandValue[k], BandItem.BandType.BAND_WCDMA));
        }
    }

    private void initLteItems() {
        sLteItems = new ArrayList<BandItem>();
        Elog.d(TAG, "sLteItems: add size = ");
        for (int i = FDD_BAND_MIN; i <= FDD_BAND_MAX; i++) {
            sLteItems.add(new BandItem("LTE Band " +
        String.valueOf(i), String.valueOf(i - 1), BandItem.BandType.BAND_LTE, true));
        }
    }

    private void initCdmaItems() {

        String[] bandNameList = getResources().getStringArray(R.array.rf_desense_tx_test_cdma_band);
        String[] bandValue = getResources().
                getStringArray(R.array.rf_desense_tx_test_cdma_band_values);

        sCdmaItems = new ArrayList<BandItem>();

        int size = bandNameList.length;
        Elog.d(TAG, "sCdmaItems: add size = " + size);

        for (int k = 0; k < size; k++) {
            sCdmaItems.add(new BandItem(bandNameList[k],
                    bandValue[k], BandItem.BandType.BAND_CDMA));
        }
    }

    // read settings from preference
    private void restoreSettings() {
        // selected test items
        SharedPreferences pref = getSharedPreferences(PREF_FILE, MODE_PRIVATE);
        String[] items = pref.getString(KEY_ITEM, "").split(",");
        mSelectedTestItems.clear();
        for (int i = 0; i < items.length; i++) {
            for (TestItem item : mTestItems) {
                if (items[i].equals(item.toString())) {
                    mSelectedTestItems.add(item);
                }
            }
        }
        mSelectedAll = pref.getBoolean(KEY_SELECTED_ALL, false);
        // selected sv id
        for (int i = 0; i < mSelectedSvid.length; i++) {
            mSelectedSvid[i] = INVALID_SV_ID;
        }
        String[] svids = pref.getString(KEY_SVID, "").split(",");
        for (int i = 0; i < svids.length; i++) {
            try {
                mSelectedSvid[i] = Integer.parseInt(svids[i]);
            } catch (NumberFormatException e) {
                mSelectedSvid[i] = INVALID_SV_ID;
            }
        }
        for (TestMode m : mTestModes) {
            m.setSvIds(mSelectedSvid);
        }

        // selected test mode
        String mode = pref.getString(KEY_MODE, "");
        for (RadioButton r : mRadioButtons) {
            r.setChecked(r.getText().toString().equals(mode));
        }
        if ((new GpsCwMode()).toString().equals(mode)) {
            Elog.d(TAG, "mSvIdButton.setEnabled(false): ");
            mSvIdButton.setEnabled(false);
        }

    }

    // write settings to preference
    private void saveSettings() {
        // selected test items
        SharedPreferences.Editor pref = getSharedPreferences(PREF_FILE, MODE_PRIVATE).edit();
        String str = "";
        for (TestItem item : mSelectedTestItems) {
            str += item.toString() + ",";
        }
        pref.putString(KEY_ITEM, str);

        // selected sv id
        str = "";
        for (int i = 0; i < MAX_SV_COUNT; i++) {
            str += String.valueOf(mSelectedSvid[i]) + ",";
        }
        pref.putString(KEY_SVID, str);

        // selected test mode
        if (mCurrentMode != null) {
            pref.putString(KEY_MODE, mCurrentMode.toString());
        }

        Elog.d(TAG, "putBoolean: " + mSelectedAll);
        pref.putBoolean(KEY_SELECTED_ALL, mSelectedAll);
        pref.commit();
    }

    // Show check box list with all test items
    private Dialog createTestItemDialog() {
        final String[] labels = new String[mTestItems.size() + 1];
        final boolean[] values = new boolean[mTestItems.size() + 1];
        labels[0] = "Select All";
        values[0] = mSelectedAll;
        for (int i = 1; i < labels.length; i++) {
            labels[i] = mTestItems.get(i - 1).toString();
            values[i] = mSelectedTestItems.contains(mTestItems.get(i - 1));
        }
        AlertDialog dialog = new AlertDialog.Builder(this)
            .setTitle("Test Items")
            .setMultiChoiceItems(labels, values,
                new DialogInterface.OnMultiChoiceClickListener() {
                    @Override
                    public void onClick(DialogInterface dialog, int which, boolean isChecked) {
                        if (which == 0) {
                            values[which] = isChecked;
                            Elog.d(TAG, "values[which]: " + values[which]);
                            ListView lv = ((AlertDialog) dialog).getListView();
                            for (int i = 1; i < labels.length; i++) {
                                lv.setItemChecked(i, isChecked);
                                values[i] = isChecked;
                            }
                            return;
                        } else {
                            if (isChecked) {
                                checkTestItemValid(which - 1);
                            } else {
                                ListView lv = ((AlertDialog) dialog).getListView();
                                lv.setItemChecked(0, isChecked);
                                values[0] = isChecked;
                            }
                            values[which] = isChecked;
                        }

                    }
             })
            .setPositiveButton(android.R.string.ok, new OnClickListener() {
                public void onClick(DialogInterface dialog, int whichButton) {
                    mSelectedTestItems.clear();
                    for (int i = 1; i < labels.length; i++) {
                        if (values[i]) {
                            mSelectedTestItems.add(mTestItems.get(i - 1));
                        }
                    }

                    Elog.d(TAG, "values[which]: " + values[0]);
                    mSelectedAll = values[0];

                    // Need to refresh ListView because test items changed
//                    updateListView();
                    saveSettings();
                }
            })
            .setNegativeButton(android.R.string.cancel, null)
            .create();
        return dialog;
    }

    private boolean checkTestItemValid(int itemIndex) {

        TestItem testItem = mTestItems.get(itemIndex);
        String strToast = sItemsToCheck.get(testItem.toString());
         if ((strToast != null)
                 && (!testItem.doApiTest())) {
             Toast.makeText(this, strToast,
                     Toast.LENGTH_LONG).show();
             Elog.d(TAG, "show toast: " + strToast);
             return false;
         }
         return true;
    }
    // user can select at most 5 SV IDs, show 5 Spinners.
    @SuppressLint("InflateParams")
    private Dialog createSvIdDialog() {
        View view = getLayoutInflater().inflate(R.layout.desense_at_sv_id, null);
        final Spinner[] spinners = new Spinner[MAX_SV_COUNT];
        spinners[SV_GPS] = (Spinner) view.findViewById(R.id.spinner_gps);
        spinners[SV_GLONASS_L] = (Spinner) view.findViewById(R.id.spinner_glonass_l);
        spinners[SV_GLONASS_M] = (Spinner) view.findViewById(R.id.spinner_glonass_m);
        spinners[SV_GLONASS_H] = (Spinner) view.findViewById(R.id.spinner_glonass_h);
        spinners[SV_BEIDOU] = (Spinner) view.findViewById(R.id.spinner_bd);

        for (int i = 0; i < MAX_SV_COUNT; i++) {
            List<String> svIdList = mSvIdLists.get(i);
            ArrayAdapter<String> adatper = new ArrayAdapter<String>(this,
                    android.R.layout.simple_spinner_item, svIdList);
            adatper.setDropDownViewResource(android.R.layout.simple_spinner_dropdown_item);
            spinners[i].setAdapter(adatper);

            int index = svIdList.indexOf(String.valueOf(mSelectedSvid[i]));
            if (index > 0) {
                spinners[i].setSelection(index);
            } else {
                spinners[i].setSelection(0); // Select "NA"
            }
        }

        AlertDialog dialog = new AlertDialog.Builder(this)
            .setTitle("GNSS Band")
            .setView(view)
            .setPositiveButton(android.R.string.ok, new OnClickListener() {
                public void onClick(DialogInterface dialog, int whichButton) {
                    for (int i = 0; i < spinners.length; i++) {
                        try {
                            int id = Integer.parseInt(spinners[i].getSelectedItem().toString());
                            mSelectedSvid[i] = id;
                        } catch (NumberFormatException e) {
                            mSelectedSvid[i] = INVALID_SV_ID;
                        }
                    }
                    for (TestMode m : mTestModes) {
                        m.setSvIds(mSelectedSvid);
                        Elog.d(TAG, "mSelectedSvid: " + mSelectedSvid);
                    }
                    saveSettings();
                }
            })
            .setNegativeButton(android.R.string.cancel, null)
            .create();
        return dialog;
    }

    // show curve for clicked test item
    @SuppressLint("InflateParams")
    private Dialog createCurveDialog(Bundle bundle) {
        int position = bundle.getInt("position", 0);
        View view = getLayoutInflater().inflate(R.layout.desense_at_curve, null);
        TextView title = (TextView) view.findViewById(R.id.desense_at_curve_title);
        TextView info = (TextView) view.findViewById(R.id.desense_at_curve_info);
        TextView x = (TextView) view.findViewById(R.id.desense_at_curve_x_label);
        TextView y = (TextView) view.findViewById(R.id.desense_at_curve_y_label);
        CurveView curve = (CurveView) view.findViewById(R.id.desense_at_curve);
        TestItem item = mAdapter.getItem(position);
        TestResult r = mTestResults.get(item);
        if (r == null) {
            return null;
        }
        title.setText(item.toString());
        info.setText(r.getSummary());
        x.setText(r.x);
        y.setText(r.y);
        curve.setData(r);
        AlertDialog dialog = new AlertDialog.Builder(this).setView(view).create();
        return dialog;
    }

    private void updateListView() {
        mAdapter.clear();
        mAdapter.addAll(mGenTestItems);
        mAdapter.notifyDataSetChanged();
    }

    private void enableButtons(boolean enable) {
        mStartButton.setEnabled(enable);
        mStopButton.setEnabled(!enable);
        mTestItemButton.setEnabled(enable);
        mTestModeRadio.setEnabled(enable);
        mSvIdButton.setEnabled(enable);
        for (RadioButton r : mRadioButtons) {
            r.setEnabled(enable);
        }
    }

    private boolean validSvIdSelected() {
        boolean ret = false;
        Elog.d(TAG, "validSvIdSelected");
        for (int id : mSelectedSvid) {
            Elog.d(TAG, "id:" + id);
            if (id != INVALID_SV_ID) {
                ret = true;
            }
        }
        return ret;
    }

    /**
     * @return if user cancel the test
     */
    public boolean isCancelled() {
        boolean ret = mTask.isCancelled();
        Elog.d(TAG, "isCancelled: " + ret);
        return ret;
    }

    @SuppressWarnings("deprecation")
    private void checkGPSState() {
        if (mLocationManager != null) {
            if (mLocationManager
                    .isProviderEnabled(LocationManager.GPS_PROVIDER)) {
                mInitGPSState = true;
            } else {
                mInitGPSState = false;
                showDialog(DIALOG_ENABLE_GPS);
            }
        }
    }

    /**
     * Adapter for ListView.
     */
    private class MyAdapter extends ArrayAdapter<TestItem> {
        /**
         * Default constructor.
         *
         * @param activity
         *              the context
         */
        public MyAdapter(Context activity) {
            super(activity, 0);
        }

        /**
         * ViewHolder.
         */
        private class ViewHolder {
            public TextView label;
            public TextView result;
            public TextView[] values;
            @Override
            public String toString() {
                return "ViewHolder [label=" + label + ", result=" + result
                        + ", values=" + Arrays.toString(values) + "]";
            }

        }

        @SuppressLint("InflateParams")
        @Override
        public View getView(int position, View convertView, ViewGroup parent) {
            ViewHolder holder;
            LayoutInflater inflater = DesenseAtActivity.this.getLayoutInflater();
            if (convertView == null) {
                convertView = inflater.inflate(R.layout.desense_at_result_entry, null);
                holder = new ViewHolder();
                holder.label = (TextView) convertView.findViewById(R.id.column1);
                holder.result = (TextView) convertView.findViewById(R.id.column2);
                holder.values = new TextView[SV_TYPE_NUMBER];
                holder.values[SV_GPS] = (TextView) convertView.findViewById(R.id.column3);
                holder.values[SV_GLONASS_L] = (TextView) convertView.findViewById(R.id.column4);
                holder.values[SV_GLONASS_M] = (TextView) convertView.findViewById(R.id.column5);
                holder.values[SV_GLONASS_H] = (TextView) convertView.findViewById(R.id.column6);
                holder.values[SV_BEIDOU] = (TextView) convertView.findViewById(R.id.column7);
                convertView.setTag(holder);
            } else {
                holder = (ViewHolder) convertView.getTag();
            }
            TestItem testItem = getItem(position);
            TestResult r = mTestResults.get(testItem);
            holder.label.setText(testItem.toString());

            if (r == null) {
                holder.result.setText("-");
            } else {
                switch (r.result) {
                case TestResult.NONE:
                    holder.result.setText(TESTING);
                    break;
                case TestResult.PASS:
                    holder.result.setText(Util.PASS);
                    break;
                case TestResult.FAIL:
                    holder.result.setText(Util.FAIL);
                    break;
                case TestResult.CONNECTION_FAIL:
                    holder.result.setText(Util.CONN_FAIL);
                    break;
                default:
                    break;
                }
            }
            // show desense values
            for (int i = 0; i < holder.values.length; i++) {
                holder.values[i].setText("-");
            }
            if (r != null && r.result != TestResult.NONE) {
                if (mCurrentMode.toString().equals("Signal")) {
                    for (int i = 0; i < holder.values.length; i++) {
                        Float desense = r.getDesenseValue(i);
                        if (desense != null) {
                            holder.values[i].setText(String.valueOf(desense));
                        }
                    }
                } else {
                    if (r.subResults.get(0) != null) {
                        Float desense = r.getDesenseValue(0);
                        if (desense != null) {
                            holder.values[0].setText(String.valueOf(desense));
                        }
                    }
                }
            }

            return convertView;
        }
    }

    private void genRealTestItems() {
        mGenTestItems.clear();
        for (TestItem testitem: mSelectedTestItems) {
            if (mItemsToOmit.contains(testitem.toString())) {
                continue;
            }
            if (GsmBandVirtualTest.ITEM_TAG.equals(testitem.toString())) {
                genGsmTestItem();
            } else if (FddBandVirtualTest.ITEM_TAG.equals(testitem.toString())) {
                genFddTestItem();
            } else if (TddBandVirtualTest.ITEM_TAG.equals(testitem.toString())) {
                genTddTestItem();
            } else if (LteBandVirtualTest.ITEM_TAG.equals(testitem.toString())) {
                genLteTestItem();
            } else if (CdmaBandVirtualTest.ITEM_TAG.equals(testitem.toString())) {
                genCdmaTestItem();
            } else {
                mGenTestItems.add(testitem);
            }
        }

        int size = mGenTestItems.size();
        for (int k = 0; k < size; k++) {
            Elog.d(TAG, "TestItem = " + mGenTestItems.get(k).toString());
            Elog.d(TAG, "TestItem index = " + (k + 1));
            mGenTestItems.get(k).setItemIndex(k + 1);
        }
    }

    private void genFddTestItem() {
        for (BandItem banditem: sFddItems) {
            if (!banditem.isSelected()) {
                continue;
            }
            int power = banditem.getPower();
            if (power != -1) {
                int[] channel = banditem.getChannel();
                for (int k = 0; k < 3; k++) {
                    if (channel[k] != -1) {
                         mGenTestItems.add(new FddBandTest(
                                 banditem.getBandName(), channel[k], power, this));
                    }
                }
            }
        }
    }

    private void genTddTestItem() {
        for (BandItem banditem: sTddItems) {
            if (!banditem.isSelected()) {
                continue;
            }
             int power = banditem.getPower();
            if (power != -1) {
                int[] channel = banditem.getChannel();
                for (int k = 0; k < 3; k++) {
                    if (channel[k] != -1) {
                        mGenTestItems.add(new TddBandTest(
                                banditem.getBandName(), channel[k], power, this));
                    }
                }
            }
        }
    }

    private void genGsmTestItem() {
        for (BandItem banditem: sGsmItems) {
            if (!banditem.isSelected()) {
                continue;
            }
            int power = banditem.getPower();
            if (power != -1) {
                int[] channel = banditem.getChannel();
                for (int k = 0; k < 3; k++) {
                    if (channel[k] != -1) {
                        mGenTestItems.add(new GsmBandTest(
                                banditem.getBandName(), channel[k], power, this));
                    }
                }
            }
        }
    }

    private void genLteTestItem() {
        for (BandItem banditem: sLteItems) {
            if (!banditem.isSelected()) {
                continue;
            }
            int power = banditem.getPower();
            if (power != -1) {
                int[] channel = banditem.getChannel();
                for (int k = 0; k < 3; k++) {
                    if (channel[k] != -1) {
                        mGenTestItems.add(new LteBandTest(
                                banditem.getBandName(), channel[k], power, this));
                    }
                }
            }
        }
    }

    private void genCdmaTestItem() {
        for (BandItem banditem: sCdmaItems) {
            if (!banditem.isSelected()) {
                continue;
            }
            int power = banditem.getPower();
            if (power != -1) {
                int[] channel = banditem.getChannel();
                for (int k = 0; k < 3; k++) {
                    if (channel[k] != -1) {
                        mGenTestItems.add(new CdmaBandTest(
                                banditem.getBandName(), channel[k], power, this));
                    }
                }
            }
        }
    }



    @Override
    public boolean onKeyDown(int keyCode, KeyEvent event) {
        // TODO Auto-generated method stub
        switch (keyCode) {
            case KeyEvent.KEYCODE_VOLUME_UP: {
                if (mCurrentMode != null) {
                    Elog.d(TAG, "task cancel");
                    setCancelled(true);
                }
                return true;
            }
        default:
            break;
        }
        return super.onKeyDown(keyCode, event);
    }

    /**
     * Task to run test items.
     */
    public class FunctionTask extends AsyncTask<Void, Void, Boolean> {
        @Override
        protected Boolean doInBackground(Void... params) {

            int size = mGenTestItems.size();
            StringBuilder strCmd = new StringBuilder(String.valueOf(size));

            for (TestItem item:mGenTestItems) {
                strCmd.append(',').append(item.getDbSpec());
            }
            if (!mCurrentMode.setSpec(strCmd.toString())) {
                runOnUiThread(new Runnable() {
                    public void run() {
                        Toast.makeText(DesenseAtActivity.this,
                                R.string.desense_at_set_spec_warning, Toast.LENGTH_LONG)
                        .show();
                    }
                });
            }


            setCancelled(false);
            initRecordData();
            for (int k = 0; k < size; k++) {
                TestItem i = mGenTestItems.get(k);
                if (getCancelled()) {
                    Elog.d(TAG, "cancel and break");
                    break;
                }
                mTestResults.put(i, new TestResult());
                runOnUiThread(new Runnable() {
                    public void run() {
                        mAdapter.notifyDataSetChanged();
                    }
                });
                // run the test item
                Elog.d(TAG, "run " + i.toString());
                i.setTestMode(mCurrentMode);
                TestResult r = i.run();
                mTestResults.put(i, r);
                Elog.d(TAG, "TestResult = " + r);
                saveResult(i.toString() , r);
                final int index = k;
                runOnUiThread(new Runnable() {
                    public void run() {
                        mResultList.requestFocusFromTouch();
                        mResultList.setSelection(index);
                        mAdapter.notifyDataSetChanged();
                    }
                });
            }
            endRecordData();
            return true;
        }

        @Override
        protected void onPostExecute(Boolean result) {
            enableButtons(true);
            mIsRunning = false;

            Util.notifyFinish(DesenseAtActivity.this);
        }

        @Override
        protected void onPreExecute() {
            // TODO Auto-generated method stub
            super.onPreExecute();
            mIsRunning = true;
            genRealTestItems();
            updateListView();
        }

        @Override
        protected void onCancelled(Boolean result) {
            enableButtons(true);
        }

    }

    synchronized static void setCancelled(boolean cancelled) {
        sTestCancelled = cancelled;
    }

    public synchronized static boolean getCancelled() {
        return sTestCancelled;
    }

    private void initRecordData() {
        String folderPath = Util.getTestFilePath(this, Util.HISTORY_PATH);
        File parentFolder = new File(folderPath);
        if (!parentFolder.exists()) {
            parentFolder.mkdirs();
        }
        File[] fileArray = parentFolder.listFiles();
        if (fileArray.length >= FILE_NUMBER) {
            List<File> fileList = Arrays.asList(fileArray);
            Collections.sort(fileList, new Comparator<File>() {

                @Override
                public int compare(File lhs, File rhs) {
                    // TODO Auto-generated method stub
                    return lhs.getName().compareTo(rhs.getName());
                }

            });
            int filesToDeleted = fileArray.length - FILE_NUMBER + 1;
            for (int k = 0; k < filesToDeleted; k++) {
                File file = fileList.get(k);
                file.delete();
            }
        }

        String strTime = DATE_FORMAT
                .format(new Date(System.currentTimeMillis()));

        String filePath = folderPath + "/" + strTime;
        Elog.d(TAG, "filePath = " + filePath);
        File logFile = new File(filePath);
        if (logFile != null) {
            try {
                logFile.createNewFile();
                mOutputData = new FileOutputStream(logFile);
                String settings = "Mode: " + mCurrentMode.toString() + " BAND: ";
                for (int id : mSelectedSvid) {
                    if (id != INVALID_SV_ID) {
                        settings = settings + id + ' ';
                    }
                }
                mOutputData.write(settings.getBytes());
                mOutputData.write(NEW_LINE.getBytes());
            } catch (IOException e) {
                mOutputData = null;
            }

        } else {
            mOutputData = null;
        }
    }


    private void saveResult(String testItem, TestResult result) {
        if ((mOutputData == null) || (result == null)) {
            return;
        }
        String title = testItem + "--" + result.result;
        String value = new String();

        if (mCurrentMode.toString().equals("Signal")) {
            for (int i = 0; i < SV_TYPE_NUMBER; i++) {
                Float desense = result.getDesenseValue(i);
                value = value + String.valueOf(desense) + ' ';
            }
        } else {
            Float desense = result.getDesenseValue(0);
            value = String.valueOf(desense);
        }

        ArrayList<float[]> dataArray = new ArrayList<float[]>();
        for (TestResult.Entry entry : result.subResults.values()) {
            float[] data = new float[entry.data.size()];
            for (int i = 0; i < data.length; i++) {
                data[i] = entry.data.get(i);
            }
            dataArray.add(data);
        }
        String strData = new String();
        for (float[] data : dataArray) {
            for (float detailData : data) {
                strData = strData + detailData + ' ';
            }
            strData.trim();
            strData = strData + ';';
        }
        try {
            mOutputData.write(title.getBytes());
            mOutputData.write(NEW_LINE.getBytes());


            mOutputData.write(value.getBytes());
            mOutputData.write(NEW_LINE.getBytes());

            mOutputData.write(result.getSummary().getBytes());
            mOutputData.write(NEW_LINE.getBytes());

            mOutputData.write(strData.getBytes());
            mOutputData.write(NEW_LINE.getBytes());

            mOutputData.flush();
        } catch (IOException e) {
            e.printStackTrace();
        }


    }

    private void endRecordData() {
        if (mOutputData != null) {
            try {
                mOutputData.close();
            } catch (IOException e) {
                e.printStackTrace();
            }
            mOutputData = null;
        }

    }
}
