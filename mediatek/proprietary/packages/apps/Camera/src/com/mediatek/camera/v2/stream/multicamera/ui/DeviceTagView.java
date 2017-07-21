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
 * MediaTek Inc. (C) 2015. All rights reserved.
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
package com.mediatek.camera.v2.stream.multicamera.ui;

import android.content.Context;
import android.util.AttributeSet;
import android.util.Log;
import android.view.Gravity;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.AdapterView;
import android.widget.AdapterView.OnItemClickListener;
import android.widget.BaseAdapter;
import android.widget.LinearLayout;
import android.widget.ListAdapter;
import android.widget.ListView;
import android.widget.TextView;

import com.android.camera.R;

import com.mediatek.camera.v2.ui.UIRotateLayout;

import java.util.ArrayList;
import java.util.LinkedHashMap;
import java.util.Map;

/**
 * DeviceTagView is a tag view for current preview phone device.
 */
public class DeviceTagView extends UIRotateLayout {
    /**
     * Device tag view listener.
     */
    public interface IDeviceTagViewListener {
        /**
         * When a device is selected, invoke the listener to notify.
         * @param deviceKey Selected device key value.
         */
        public void onDeviceCameraSelected(String deviceKey);
    }
    public static final int DEVICE_TAG_TYPE_MULTI = 0;
    public static final int DEVICE_TAG_TYPE_SINGLE = 1;

    /**
     * Device tag type, the tag view has two type.
     */
    public enum DeviceTagType {
        MULTI,
        SINGLE,
    };


    private static final int LIST_MAX_LENGHT = 3;

    private static final int ORIENTATION_0 = 0;
    private static final int ORIENTATION_90 = 90;
    private static final int ORIENTATION_180 = 180;
    private static final int ORIENTATION_270 = 270;

    private String mTAG = "DeviceTagView";
    private LinearLayout mListWrapper;
    private ListView mListview;
    private TextView mTopText;
    private TextView mBottomText;
    private MyAdapter mDeviceAdapter;
    private int mDisplayRotation;
    private int mOrientation;
    private DeviceTagType mTagType;
    private String mTagName;

    private IDeviceTagViewListener mListener;

    /**
     * Constructor that is called when inflating a InLineSettingVirtual view from XML.
     * @param context The Context the view is running in.
     * @param attrs The attributes of the XML tag that is inflating the view.
     */
    public DeviceTagView(Context context, AttributeSet attrs) {
        super(context, attrs);
        mDeviceAdapter = new MyAdapter(context);
    }

    /**
     * Hide tag list view.
     */
    public void hideListView() {
        Log.i(mTAG, "hideListView");
        mListview.setVisibility(View.GONE);
    }


    /**
     * When activity display orientation change, invoke it to notity the event.
     * @param displayRotation The new display orientation.
     */
    public void onDisplayChanged(int displayRotation) {
        Log.i(mTAG, "onDisplayChanged = " + displayRotation);
        mDisplayRotation = displayRotation;
        if (mTagType == DeviceTagType.MULTI) {
            updateMultiTagView();
        } else {
            updateSingleTagView();
        }
    }


    /**
     * The device orientation change.
     * @param gsensorOrientation New orientation.
     */
    public void onOrientationChanged(int gsensorOrientation) {
        Log.i(mTAG, "onOrientationChanged gsensorOrientation = "
                + gsensorOrientation);
        Log.i(mTAG, " onOrientationChanged mDisplayRotation = "
                + mDisplayRotation);
        super.setOrientation(gsensorOrientation, true);
        mOrientation = gsensorOrientation;
        if (mTagType == DeviceTagType.MULTI) {
            updateMultiTagView();
        } else {
            updateSingleTagView();
        }
    }

    /**
     * Set data of candidate devices.
     * @param data Candidate devices, Device key-Device name pair.
     */
    public void setData(LinkedHashMap<String, String> data) {
        Log.i(mTAG, "setData");
        mDeviceAdapter.setData(data);
        setListViewHeightBasedOnChildren(mListview);
    }

    /**
     * Set current display device name.
     * @param name Device name.
     */
    public void setDeviceName(String name) {
        mTAG = DeviceTagView.class.getSimpleName() + "(" + name + ")" + "(" + mTagType + ")";
        Log.i(mTAG, "setDeviceName name " + name);
        mTopText.setText(name);
        mBottomText.setText(name);
        mTagName = name;
    }

    /**
     * Set the tag type.
     * @param type Tag type.
     */
    public void setTagType(DeviceTagType type) {
        mTAG = DeviceTagView.class.getSimpleName() + "(" + mTagName + ")" + "(" + type + ")";
        Log.i(mTAG, "setTagType type " + type);
        mTagType = type;
    }

    /**
     * Set the listener.
     * @param listener The listener instance.
     */
    public void setListener(IDeviceTagViewListener listener) {
        mListener = listener;
    }

    /**
     * Clear the listener.
     */
    public void clearListener() {
        mListener = null;
    }

    @Override
    protected void onFinishInflate() {
        super.onFinishInflate();
        mListWrapper = (LinearLayout) findViewById(R.id.list_wrapper);
        mListview = (ListView) findViewById(R.id.listview);
        mListview.setAdapter(mDeviceAdapter);
        mListview.setOnItemClickListener(new ListItemClickListener());
        mTopText = (TextView) findViewById(R.id.current_device1);
        mBottomText = (TextView) findViewById(R.id.current_device2);
        mTopText.setOnClickListener(new OnClickListener() {

            @Override
            public void onClick(View v) {
                toggleListVisible();
            }
        });

        mBottomText.setOnClickListener(new OnClickListener() {
            @Override
            public void onClick(View v) {
                toggleListVisible();
            }
        });
        Log.i(mTAG, "onFinishInflate");
    }

    /**
     * A callback to be invoked when an item of adapterview has been clicked.
     */
    private class ListItemClickListener implements OnItemClickListener {

        @Override
        public void onItemClick(AdapterView<?> parent, View view, int position,
                long id) {
            Log.i(mTAG, "onItemClick position " + position);
            Map.Entry<String, String> item = mDeviceAdapter.getItem(position);
            Log.i(mTAG, "onItemClick devie name  " + item.getValue());
            mListview.setVisibility(View.GONE);
            if (mListener != null) {
                mListener.onDeviceCameraSelected(item.getKey());
            }
        }
    }

    private void updateMultiTagView() {
        if (mListWrapper != null) {
            if (mDisplayRotation == ORIENTATION_0 || mDisplayRotation == ORIENTATION_180) {
                // just for portrait layout
                if (mOrientation == ORIENTATION_90 || mOrientation == ORIENTATION_0) {
                    mTopText.setVisibility(View.GONE);
                    mBottomText.setVisibility(View.VISIBLE);
                } else {
                    mTopText.setVisibility(View.VISIBLE);
                    mBottomText.setVisibility(View.GONE);
                }
                if (mOrientation == ORIENTATION_90 || mOrientation == ORIENTATION_180) {
                    mListWrapper.setGravity(Gravity.LEFT);
                } else {
                    mListWrapper.setGravity(Gravity.RIGHT);
                }
            } else {
                // just for landscape layout.
                if (mOrientation == ORIENTATION_0 || mOrientation == ORIENTATION_270) {
                    mTopText.setVisibility(View.VISIBLE);
                    mBottomText.setVisibility(View.GONE);
                } else {
                    mTopText.setVisibility(View.GONE);
                    mBottomText.setVisibility(View.VISIBLE);
                }

                if (mOrientation == ORIENTATION_0 || mOrientation == ORIENTATION_90) {
                    mListWrapper.setGravity(Gravity.RIGHT);
                } else {
                    mListWrapper.setGravity(Gravity.LEFT);
                }
            }
        }
    }

    private void updateSingleTagView() {
        if (mListWrapper != null) {
            if (mDisplayRotation == ORIENTATION_0 || mDisplayRotation == ORIENTATION_180) {
                // just for portrait layout
                if (mOrientation == ORIENTATION_270 || mOrientation == ORIENTATION_0) {
                    mTopText.setVisibility(View.VISIBLE);
                    mBottomText.setVisibility(View.GONE);
                } else {
                    mTopText.setVisibility(View.GONE);
                    mBottomText.setVisibility(View.VISIBLE);
                }
                if (mOrientation == ORIENTATION_90 || mOrientation == ORIENTATION_180) {
                    mListWrapper.setGravity(Gravity.LEFT);
                } else {
                    mListWrapper.setGravity(Gravity.RIGHT);
                }
            } else {
                // just for landscape layout.
                if (mOrientation == ORIENTATION_0 || mOrientation == ORIENTATION_90) {
                    mTopText.setVisibility(View.VISIBLE);
                    mBottomText.setVisibility(View.GONE);
                } else {
                    mTopText.setVisibility(View.GONE);
                    mBottomText.setVisibility(View.VISIBLE);
                }

                if (mOrientation == ORIENTATION_0 || mOrientation == ORIENTATION_90) {
                    mListWrapper.setGravity(Gravity.RIGHT);
                } else {
                    mListWrapper.setGravity(Gravity.LEFT);
                }
            }
        }
    }

    private void setListViewHeightBasedOnChildren(ListView listView) {
        // get listview adapter
        Log.i(mTAG, "setListViewHeightBasedOnChildren listView " + listView);
        if (listView == null) {
            return;
        }
        ListAdapter listadapter = listView.getAdapter();
        if (listadapter == null) {
            return;
        }

        int totalHeight = 0;
        int count = listadapter.getCount();
        if (count > LIST_MAX_LENGHT) {
            count = LIST_MAX_LENGHT;
        }
        for (int i = 0; i < count; i++) {
            View listItem = listadapter.getView(i, null, listView);
            listItem.measure(0, 0); // calculate the child view width and height.
            totalHeight += listItem.getMeasuredHeight();
            Log.i(mTAG, "setListViewHeightBasedOnChildren getMeasuredHeight "
                    + listItem.getMeasuredHeight());
        }
        Log.i(mTAG, "setListViewHeightBasedOnChildren getDividerHeight "
                + listView.getDividerHeight());
        ViewGroup.LayoutParams params = listView.getLayoutParams();
        params.height = totalHeight
                + (listView.getDividerHeight() * (count - 1));
        listView.setLayoutParams(params);
        Log.i(mTAG, "setListViewHeightBasedOnChildren totalHeight "
                + totalHeight);
    }

    private void toggleListVisible() {
        if (mListview.getVisibility() == View.VISIBLE) {
            mListview.setVisibility(View.GONE);
        } else {
            mListview.setVisibility(View.VISIBLE);
        }
    }

    /**
     * A view holder for adpater view.
     */
   private static class ViewHolder {
        public TextView title;
    }

   /**
    * A adapter for list view to show the device name list.
    */
    private class MyAdapter extends BaseAdapter {

        private ArrayList<Map.Entry<String, String>> mList;

        @Override
        public boolean isEmpty() {
            return false;
        }

        public void setData(LinkedHashMap<String, String> data) {
            mList.clear();
            mList.addAll(data.entrySet());
            notifyDataSetChanged();
        }

        private LayoutInflater mInflater = null;

        private MyAdapter(Context context) {
            this.mInflater = LayoutInflater.from(context);
            mList = new ArrayList<Map.Entry<String, String>>();
        }

        @Override
        public int getCount() {
            Log.i(mTAG, "MyAdapter getCount size = " + mList.size());
            return mList.size();
        }

        @Override
        public Map.Entry<String, String> getItem(int position) {
            return (Map.Entry<String, String>) mList.get(position);
        }

        @Override
        public long getItemId(int position) {
            return position;
        }

        @Override
        public View getView(int position, View convertView, ViewGroup parent) {
            if (mList != null) {
                ViewHolder holder = null;
                if (convertView == null) {
                    holder = new ViewHolder();
                    convertView = mInflater.inflate(R.layout.device_list_item,
                            null);
                    holder.title = (TextView) convertView
                            .findViewById(R.id.device_name);
                    convertView.setTag(holder);
                } else {
                    holder = (ViewHolder) convertView.getTag();
                }

                Map.Entry<String, String> item = getItem(position);
                Log.i(mTAG, "MyAdapter getView position = " + position);
                Log.i(mTAG, "MyAdapter getView name = " + item.getValue());
                holder.title.setText(item.getValue());
                return convertView;
            } else {
                return null;
            }
        }
    }
}