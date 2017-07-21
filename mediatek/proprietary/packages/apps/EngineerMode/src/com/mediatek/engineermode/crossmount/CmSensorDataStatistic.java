package com.mediatek.engineermode.crossmount;

import android.content.Context;
import android.hardware.Sensor;
import android.hardware.SensorEvent;
import android.hardware.SensorEventListener;
import android.util.Log;
import android.widget.CheckBox;
import android.widget.TextView;
import android.widget.Toast;

import java.text.NumberFormat;

/**
 * Class for CmSensor listener.
 *
 */
public class CmSensorDataStatistic implements SensorEventListener {

    private String mSensorName = null;
    private Context mContext = null;
    private TextView mDataView = null;
    private TextView mStatView = null;
    private CheckBox mChkBox = null;

    private long mStatisticTime = 0;
    private float mFrequency = 0;
    private int mStatCnt = 0;
    private float mMean = 0;
    private float mMin = 0;
    private float mMax = 0;
    private double mDev = 0;
    private int mPrintPeriod = 1;
    private static NumberFormat sNfInstance = NumberFormat.getNumberInstance();
    static {
        sNfInstance.setMaximumFractionDigits(2);
        sNfInstance.setMinimumFractionDigits(1);
    }

    /**
     * Constructor function.
     * @param context Interface to global information about an application environment
     * @param name Sensor name
     * @param dataView TextView for data
     * @param statView TextView for state
     * @param chkBox Checkbox
     */
    public CmSensorDataStatistic(Context context, String name,
            TextView dataView, TextView statView, CheckBox chkBox) {
        mSensorName = name;
        mContext = context;
        mDataView = dataView;
        mStatView = statView;
        mChkBox = chkBox;
    }

    @Override
    public void onSensorChanged(SensorEvent event) {
        long deltaTime = 0;
        long currentTime = System.nanoTime();
        float prevMean = 0;

        if (event.timestamp == 0) {
            mChkBox.setChecked(false);
            Toast.makeText(mContext,
                    mSensorName + "sensor is not available now",
                    Toast.LENGTH_LONG).show();
        }

        if (mStatisticTime == 0) {
            mStatisticTime = System.nanoTime();
        }

        mFrequency += 1;

        deltaTime = currentTime - mStatisticTime;

        if (mFrequency % mPrintPeriod == 0) {
            mDataView.setText("x: " + event.values[0] + "\ny: "
                    + event.values[1] + "\nz: " + event.values[2]);
        }

        Log.d("SENSOR_TEST", " onSensorChanged2, deltaTime:" + deltaTime);

        if (deltaTime >= 500000000L) {
            mStatCnt += 1;
            // calculate the frequency.
            mFrequency = (float) (mFrequency * 1000000000L / deltaTime);

            if (mFrequency > 60) {
                mPrintPeriod = 5;
            } else if (mFrequency > 30) {
                mPrintPeriod = 3;
            } else if (mFrequency > 10) {
                mPrintPeriod = 2;
                mFrequency += 5;
            } else {
                mPrintPeriod = 1;
            }

            // calculate the mean of frequency.
            prevMean = mMean;
            mMean = ((mStatCnt - 1) * prevMean + mFrequency) / mStatCnt;

            // calculate the max value of frequency.
            mMax = mFrequency > mMax ? mFrequency : mMax;

            // calculate the minimum value of frequency
            mMin = mMin > 0 ? mMin : mFrequency;
            mMin = mFrequency < mMin ? mFrequency : mMin;

            // calculate the deviation of frequency
            mDev = ((mStatCnt - 1) * (mDev + prevMean * prevMean) + mFrequency
                    * mFrequency)
                    / mStatCnt - mMean * mMean;

            mStatView.setText("Current: " + sNfInstance.format(mFrequency)
                    + "HZ\nMean: " + sNfInstance.format(mMean) + " HZ\nMin: "
                    + sNfInstance.format(mMin) + " HZ\nMax: " + sNfInstance.format(mMax)
                    + " HZ\nStdDev: " + sNfInstance.format(Math.sqrt(mDev)) + " HZ");

            mStatisticTime = currentTime;
            mFrequency = 0;
        }
    }

    @Override
    public void onAccuracyChanged(Sensor sensor, int accuracy) {
        Log.d("SENSOR_TEST", "onAccuracyChanged");
        mStatisticTime = 0;
        mFrequency = 0;
        mStatCnt = 0;
        mMean = 0;
        mMin = 0;
        mMax = 0;
        mDev = 0;
    }
}
