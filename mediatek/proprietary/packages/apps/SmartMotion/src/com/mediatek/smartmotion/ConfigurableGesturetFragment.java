package com.mediatek.smartmotion;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.ResourceBundle;

import android.R.integer;
import android.app.Activity;
import android.content.Context;
import android.content.DialogInterface;
import android.content.res.Resources;
import android.hardware.Sensor;
import android.hardware.SensorEvent;
import android.hardware.SensorEventListener;
import android.hardware.SensorManager;
import android.os.Bundle;
import android.preference.PreferenceFragment;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.AdapterView;
import android.widget.AdapterView.OnItemClickListener;
import android.widget.ArrayAdapter;
import android.widget.Button;
import android.widget.ListView;
import android.widget.RadioButton;
import android.widget.RadioGroup;
import android.widget.TextView;
import android.widget.Toast;

import com.mediatek.sensorhub.ContextInfo;
import com.mediatek.sensorhub.SensorHubManager;
import com.mediatek.smartmotion.enabler.Preferences;
import com.mediatek.smartmotion.utils.LogUtils;

public class ConfigurableGesturetFragment extends PreferenceFragment
        implements View.OnClickListener, SensorEventListener {
    private static String TAG = "GesturetFragment";
    private static String DELIMITER = "->";
    private String mSeletedString;
    private Preferences mPreferences;
    private Activity mActivity;
    private ListView mListView;
    private List<String> mArrayList;
    private Button mAddButton;
    private Button mDeleteButton;
    private RadioGroup mGestureTypeGroup;
    private RadioGroup mSensorTypeGroup;
    private ArrayAdapter<String> mArrayAdapter;
    private HashMap< Integer, String> mResourceIdGestureNameMap;
    private HashMap< String, Integer> mGestureNameGestureTypeMap;
    private HashMap< Integer, Integer> mGestureTypeContextTypeMap;
    private SensorHubManager mSensorHubManager;
    private SensorManager mSensorManager;

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        mActivity = getActivity();
        mPreferences = Preferences.getPreferences(mActivity);
        mArrayList = mPreferences.getConfiguredGesture();
        mArrayAdapter = new ArrayAdapter<String>(getActivity(), R.layout.gesture_list_item,
                mArrayList);
        mSensorHubManager = (SensorHubManager)mActivity.getSystemService(
                SensorHubManager.SENSORHUB_SERVICE);
        mSensorManager = (SensorManager)mActivity.getSystemService(Context.SENSOR_SERVICE);
        initResourceIdGestureNameMap();
        initGestureNameGestureTypeMap();
        initGestureTypeContextTypeMap();
        LogUtils.d(TAG, "mArrayList :" + mArrayList.toString());
    }

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container,
            Bundle savedInstanceState) {
        View view = inflater.inflate(R.layout.configurable_gesture_fragment, container, false);
        mListView = (ListView) view.findViewById(R.id.list);
        mAddButton = (Button)view.findViewById(R.id.add);
        mDeleteButton = (Button)view.findViewById(R.id.delete);
        mGestureTypeGroup = (RadioGroup)view.findViewById(R.id.gesture_type);
        mSensorTypeGroup = (RadioGroup)view.findViewById(R.id.sensor_type);

        mListView.setAdapter(mArrayAdapter);
        mListView.setOnItemClickListener(new OnItemClickListener() {

            @Override
            public void onItemClick(AdapterView<?> parent, View view, int position,
                    long id) {
                mSeletedString = (String) ((TextView)view).getText();
                view.setSelected(true);
                LogUtils.d(TAG, "onItemClick mSeletedString = " + mSeletedString);
            }
        });

        mAddButton.setOnClickListener(this);
        mDeleteButton.setOnClickListener(this);

        return view;
    }

    @Override
    public void onActivityCreated(Bundle savedInstanceState) {
        super.onActivityCreated(savedInstanceState);
        ((MainActivity) mActivity).attachFragment(true);
        mActivity.invalidateOptionsMenu();
    }

    @Override
    public void onDestroy() {
        LogUtils.d(TAG, "onDestroy mArrayList: " + mArrayList.toString());
        mPreferences.setConfiguredGesture(mArrayList);
        ((MainActivity)mActivity).attachFragment(false);
        mActivity.invalidateOptionsMenu();
        super.onDestroy();
    }

    @Override
    public void onClick(View v) {
        String gestureTypeName;
        String sensorTypeName;
        int gestureType;
        int sensorType;

        switch (v.getId()) {
        case R.id.add:
            int gestureTypeId = mGestureTypeGroup.getCheckedRadioButtonId();
            int sensorTypeId = mSensorTypeGroup.getCheckedRadioButtonId();
            gestureTypeName = mResourceIdGestureNameMap.get(gestureTypeId);
            sensorTypeName = mResourceIdGestureNameMap.get(sensorTypeId);
            gestureType = mGestureNameGestureTypeMap.get(gestureTypeName);
            sensorType = mGestureNameGestureTypeMap.get(sensorTypeName);
            String gesture = null;
            if (gestureTypeName != null && sensorTypeName != null) {
                gesture = gestureTypeName + DELIMITER + sensorTypeName;
            }
            // No need to judge whether the gesture has been added, it can be added repeatedly
            if (gesture!= null /*&& !mArrayList.contains(gesture)*/) {
                // set configurable gesture, refresh gesture list if success
                int contextType = mGestureTypeContextTypeMap.get(gestureType);
              if (addConfigurableGesture(contextType, gestureType, sensorType)) {
                    mArrayList.add(gesture);
                    mArrayAdapter.notifyDataSetChanged();
                    LogUtils.d(TAG, "add gesture: " + gesture);
                }
            } else {
                Toast.makeText(mActivity, mActivity.getString(R.string.add_gesture_alert1),
                        Toast.LENGTH_SHORT).show();
            }
            break;
        case R.id.delete:
            if (!mArrayList.isEmpty() && mSeletedString != null) {
                String[] ss = mSeletedString.split(DELIMITER);
                gestureTypeName = ss[0];
                sensorTypeName = ss[1];
                gestureType = mGestureNameGestureTypeMap.get(ss[0]);
                sensorType = mGestureNameGestureTypeMap.get(ss[1]);
                // Cancel gesture configuration, refresh list if success
                cancelConfigurableGesture(gestureType, sensorType);
                mArrayList.remove(mSeletedString);
                mArrayAdapter.notifyDataSetChanged();
                LogUtils.d(TAG, "delete gesture: " + mSeletedString);
                mSeletedString = null;
            }
            break;
        }
    }

    private boolean addConfigurableGesture(int contextType, int gestureType, int sensorType) {
        Sensor sensor = mSensorManager.getDefaultSensor(sensorType);
        if (mSensorHubManager != null && mSensorManager != null &&
                mSensorHubManager.isCGestureSupported(contextType)) {
            mSensorHubManager.addConfigurableGesture(gestureType, sensorType);
            mSensorManager.registerListener(this, sensor, mSensorManager.SENSOR_DELAY_NORMAL);
            return true;
        } else {
            LogUtils.d(TAG, "Failed to addConfigurableGesture, mSensorHubManager is null");
            return false;
        }
    }

    private void cancelConfigurableGesture(int gestureType, int sensorType) {
        Sensor sensor = mSensorManager.getDefaultSensor(sensorType);
        if (mSensorHubManager != null && mSensorManager != null) {
            mSensorHubManager.cancelConfigurableGesture(gestureType, sensorType);
            mSensorManager.unregisterListener(this, sensor);
        }
    }

    private void initResourceIdGestureNameMap() {
        Resources res = mActivity.getResources();
        mResourceIdGestureNameMap = new HashMap<Integer, String>();
        mResourceIdGestureNameMap.put(R.id.shake, res.getString(R.string.shake));
        mResourceIdGestureNameMap.put(R.id.pickup, res.getString(R.string.pickup));
        mResourceIdGestureNameMap.put(R.id.flip, res.getString(R.string.flip));
        mResourceIdGestureNameMap.put(R.id.snapshot, res.getString(R.string.snapshot));
        mResourceIdGestureNameMap.put(R.id.wake_gesture, res.getString(R.string.wake_gesture));
        mResourceIdGestureNameMap.put(R.id.glance_gesture, res.getString(R.string.glance_gesture));
        mResourceIdGestureNameMap.put(R.id.pickup_gesture, res.getString(R.string.pickup_gesture));
    }

    private void initGestureNameGestureTypeMap() {
        Resources res = mActivity.getResources();
        mGestureNameGestureTypeMap = new HashMap<String, Integer>();
        mGestureNameGestureTypeMap.put(res.getString(R.string.shake), ContextInfo.Shake.VALUE);
        mGestureNameGestureTypeMap.put(res.getString(R.string.pickup), ContextInfo.Pickup.VALUE);
        mGestureNameGestureTypeMap.put(res.getString(R.string.flip), ContextInfo.Facing.FACE_DOWN);
        mGestureNameGestureTypeMap.put(res.getString(R.string.snapshot),
                ContextInfo.Snapshot.VALUE);
        mGestureNameGestureTypeMap.put(res.getString(R.string.wake_gesture),
                Sensor.TYPE_WAKE_GESTURE);
        mGestureNameGestureTypeMap.put(res.getString(R.string.glance_gesture),
                Sensor.TYPE_GLANCE_GESTURE);
        mGestureNameGestureTypeMap.put(res.getString(R.string.pickup_gesture),
                Sensor.TYPE_PICK_UP_GESTURE);
    }

    private void initGestureTypeContextTypeMap() {
        mGestureTypeContextTypeMap = new HashMap<Integer, Integer>();
        mGestureTypeContextTypeMap.put(ContextInfo.Shake.VALUE, ContextInfo.Type.SHAKE);
        mGestureTypeContextTypeMap.put(ContextInfo.Pickup.VALUE, ContextInfo.Type.PICK_UP);
        mGestureTypeContextTypeMap.put(ContextInfo.Facing.FACE_DOWN, ContextInfo.Type.FACING);
        mGestureTypeContextTypeMap.put(ContextInfo.Snapshot.VALUE, ContextInfo.Type.SNAPSHOT);
    }

    @Override
    public void onAccuracyChanged(Sensor arg0, int arg1) {

    }

    @Override
    public void onSensorChanged(SensorEvent arg0) {

    }
}
