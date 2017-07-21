package com.mediatek.engineermode.crossmount;

import android.app.Activity;
import android.content.Context;
import android.hardware.Sensor;
import android.hardware.SensorManager;
import android.os.Bundle;
import android.os.Handler;
import android.os.SystemClock;
import android.util.Log;
import android.view.KeyEvent;
import android.view.Menu;
import android.view.MenuItem;

import android.widget.CheckBox;
import android.widget.CompoundButton;
import android.widget.CompoundButton.OnCheckedChangeListener;
import android.widget.RadioButton;
import android.widget.RadioGroup;
import android.widget.TextView;
import android.widget.Toast;

import com.mediatek.engineermode.R;

import java.net.Inet4Address;
import java.net.InetAddress;
import java.net.NetworkInterface;
import java.net.SocketException;
import java.util.Enumeration;

/**
 * Class for CmSensor activity.
 *
 */
@SuppressWarnings("deprecation")
public class CmSensorTest extends Activity {

    private Handler mHandler = null;
    private SensorManager mSensorManager = null;
    private Sensor mAccSensor = null;
    private Sensor mGyroSensor = null;

    private CheckBox mAccCheckBox = null;
    private CheckBox mGyroCheckBox = null;
    private RadioGroup mRateGroup = null;
    private RadioButton mFastestBtn = null;
    private RadioButton mGameBtn = null;
    private RadioButton mUiBtn = null;
    private RadioButton mNormalBtn = null;

    private TextView mAccTxt = null;
    private TextView mGyroTxt = null;

    private TextView mAccStatTxt = null;
    private TextView mGyroStatTxt = null;

    private int mCurRateUs = SensorManager.SENSOR_DELAY_NORMAL;

    private CmSensorDataStatistic mAccStatistic = null;
    private CmSensorDataStatistic mGyroStatistic = null;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        // requestWindowFeature(Window.FEATURE_CUSTOM_TITLE);
        setContentView(R.layout.cm_sensor_test);
        // getWindow().setFeatureInt(Window.FEATURE_CUSTOM_TITLE,
        // R.layout.cm_sensor_test_title);

        TextView title = (TextView) findViewById(R.id.cm_sensor_title_text);
        title.setText("Sensor Test App (" + this.getLocalIp() + ")");

        mHandler = new Handler();
        mSensorManager = (SensorManager) this
                .getSystemService(Context.SENSOR_SERVICE);
        mAccSensor = mSensorManager.getDefaultSensor(Sensor.TYPE_ACCELEROMETER);
        mGyroSensor = mSensorManager.getDefaultSensor(Sensor.TYPE_GYROSCOPE);

        mAccCheckBox = (CheckBox) findViewById(R.id.cm_sensor_acc_check_box);
        mGyroCheckBox = (CheckBox) findViewById(R.id.cm_sensor_gyro_check_box);

        mRateGroup = (RadioGroup) findViewById(R.id.cm_sensor_rateRadioGroup);
        mFastestBtn = (RadioButton) findViewById(R.id.cm_sensor_fastest);
        mGameBtn = (RadioButton) findViewById(R.id.cm_sensor_game);
        mUiBtn = (RadioButton) findViewById(R.id.cm_sensor_ui);
        mNormalBtn = (RadioButton) findViewById(R.id.cm_sensor_normal);
        mNormalBtn.setChecked(true);

        mAccTxt = (TextView) findViewById(R.id.cm_sensor_acc_data_txt);
        mGyroTxt = (TextView) findViewById(R.id.cm_sensor_gyro_data_txt);

        mAccStatTxt = (TextView) findViewById(R.id.cm_sensor_acc_stat);
        mGyroStatTxt = (TextView) findViewById(R.id.cm_sensor_gyro_stat);

        mAccStatistic = new CmSensorDataStatistic(this, "Acc", mAccTxt,
                mAccStatTxt, mAccCheckBox);
        mGyroStatistic = new CmSensorDataStatistic(this, "Gryo", mGyroTxt,
                mGyroStatTxt, mGyroCheckBox);

        mAccCheckBox.setOnCheckedChangeListener(new OnCheckedChangeListener() {

            @Override
            public void onCheckedChanged(CompoundButton buttonView,
                    boolean isChecked) {
                if (isChecked == true) {
                    enableSensor(mAccSensor, mCurRateUs);
                } else {
                    disableSensor(mAccSensor);
                }
            }
        });

        mGyroCheckBox.setOnCheckedChangeListener(new OnCheckedChangeListener() {

            @Override
            public void onCheckedChanged(CompoundButton buttonView,
                    boolean isChecked) {
                if (isChecked == true) {
                    enableSensor(mGyroSensor, mCurRateUs);
                } else {
                    disableSensor(mGyroSensor);
                }
            }

        });

        mRateGroup
                .setOnCheckedChangeListener(new RadioGroup.OnCheckedChangeListener() {

                    @Override
                    public void onCheckedChanged(RadioGroup group, int checkedId) {
                        if (mFastestBtn.getId() == checkedId) {
                            Log.d("SENSOR_TEST", "FASTEST button is checked");
                            mCurRateUs = SensorManager.SENSOR_DELAY_FASTEST;
                        } else if (mGameBtn.getId() == checkedId) {
                            Log.d("SENSOR_TEST", "GAME button is checked");
                            mCurRateUs = SensorManager.SENSOR_DELAY_GAME;
                        } else if (mUiBtn.getId() == checkedId) {
                            Log.d("SENSOR_TEST", "UI button is checked");
                            mCurRateUs = SensorManager.SENSOR_DELAY_UI;
                        } else if (mNormalBtn.getId() == checkedId) {
                            Log.d("SENSOR_TEST", "NORAML button is checked");
                            mCurRateUs = SensorManager.SENSOR_DELAY_NORMAL;
                        }

                        Log.d("SENSOR_TEST", "delay is" + mCurRateUs);

                        if (mAccCheckBox.isChecked()) {
                            Log.d("SENSOR_TEST", "ACC button is checked");
                            disableSensor(mAccSensor);
                            enableSensor(mAccSensor, mCurRateUs);
                        }

                        if (mGyroCheckBox.isChecked()) {
                            Log.d("SENSOR_TEST", "GYRO button is checked");
                            disableSensor(mGyroSensor);
                            enableSensor(mGyroSensor, mCurRateUs);
                        }
                    }
                });

    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        // Inflate the menu; this adds items to the action bar if it is present.
        // getMenuInflater().inflate(R.menu.main, menu);
        return true;
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        // Handle action bar item clicks here. The action bar will
        // automatically handle clicks on the Home/Up button, so long
        // as you specify a parent activity in AndroidManifest.xml.
        /*
         * int id = item.getItemId(); if (id == R.id.cm_sensor_action_settings)
         * { return true; }
         */
        return super.onOptionsItemSelected(item);
    }

    private void enableSensor(Sensor sensor, int rateUs) {
        if (sensor == mAccSensor) {
            Log.d("SENSOR_TEST", "ACC registerListener");
            if (false == mSensorManager.registerListener(mAccStatistic, sensor,
                    rateUs, mHandler)) {
                Log.e("SENSOR_TEST", "Acc registerListener failed");
                mAccCheckBox.setChecked(false);
                Toast.makeText(this, "Acc sensor is not available now",
                        Toast.LENGTH_LONG).show();
            }
        } else if (sensor == mGyroSensor) {
            Log.d("SENSOR_TEST", "GYRO registerListener");
            if (false == mSensorManager.registerListener(mGyroStatistic,
                    sensor, rateUs, mHandler)) {
                Log.e("SENSOR_TEST", "GYRO registerListener failed ");

                mGyroCheckBox.setChecked(false);
                Toast.makeText(this, "Gyro sensor is not available now",
                        Toast.LENGTH_LONG).show();
            }
        }
    }

    private void disableSensor(Sensor sensor) {
        if (sensor == mAccSensor) {
            Log.d("SENSOR_TEST", "ACC unregisterListener");
            mSensorManager.unregisterListener(mAccStatistic, sensor);
        } else if (sensor == mGyroSensor) {
            Log.d("SENSOR_TEST", "GYRO unregisterListener");
            mSensorManager.unregisterListener(mGyroStatistic, sensor);
        }
    }

    @Override
    public boolean onKeyUp(int keyCode, KeyEvent event) {
        if (keyCode == KeyEvent.KEYCODE_BACK
                && event.getAction() == KeyEvent.ACTION_UP) {
            Log.d("SENSOR_TEST", "exit");
            SystemClock.sleep(500);
            disableSensor(mAccSensor);
            disableSensor(mGyroSensor);
            finish();
            System.exit(0);
        }

        return super.onKeyUp(keyCode, event);
    }

    private String getLocalIp() {

        try {
            Enumeration<NetworkInterface> en = NetworkInterface
                    .getNetworkInterfaces();

            while (en.hasMoreElements()) {
                NetworkInterface nif = en.nextElement();
                Enumeration<InetAddress> inet = nif.getInetAddresses();

                while (inet.hasMoreElements()) {
                    InetAddress ip = inet.nextElement();
                    if ((!ip.isLoopbackAddress())
                            && (ip instanceof Inet4Address)
                            && ip.isSiteLocalAddress()) {
                        return ip.getHostAddress();
                    }
                }
            }
        } catch (SocketException e) {
            e.printStackTrace();
        }
        return "";
    }
}
