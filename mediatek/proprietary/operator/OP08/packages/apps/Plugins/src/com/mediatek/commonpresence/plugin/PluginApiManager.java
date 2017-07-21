/* Copyright Statement:
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein
 * is confidential and proprietary to MediaTek Inc. and/or its licensors.
 * Without the prior written permission of MediaTek inc. and/or its licensors,
 * any reproduction, modification, use or disclosure of MediaTek Software,
 * and information contained herein, in whole or in part, shall be strictly prohibited.
 *
 * MediaTek Inc. (C) 2012. All rights reserved.
 *
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE")
 * RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO RECEIVER ON
 * AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL WARRANTIES,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
 * NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
 * SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
 * SUPPLIED WITH THE MEDIATEK SOFTWARE, AND RECEIVER AGREES TO LOOK ONLY TO SUCH
 * THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES
 * THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES
 * CONTAINED IN MEDIATEK SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK
 * SOFTWARE RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND
 * CUMULATIVE LIABILITY WITH RESPECT TO THE MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
 * AT MEDIATEK'S OPTION, TO REVISE OR REPLACE THE MEDIATEK SOFTWARE AT ISSUE,
 * OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY RECEIVER TO
 * MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 *
 * The following software/firmware and/or related documentation ("MediaTek Software")
 * have been modified by MediaTek Inc. All revisions are subject to any receiver's
 * applicable license agreements with MediaTek Inc.
 */

package com.mediatek.commonpresence.plugin;

import android.content.BroadcastReceiver;
import android.content.ComponentName;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.content.ServiceConnection;
import android.content.pm.PackageManager;
import android.content.pm.ResolveInfo;
import android.database.ContentObserver;
import android.database.Cursor;
import android.os.AsyncTask;
import android.os.Handler;
import android.os.IBinder;
import android.os.Looper;
import android.os.RemoteException;
import android.provider.ContactsContract.CommonDataKinds.Phone;
import android.provider.ContactsContract.Contacts;
import android.provider.ContactsContract.Data;
import android.text.TextUtils;
import android.util.Log;
import android.util.LruCache;

import java.util.ArrayList;
import java.util.List;
import java.util.Map;
import java.util.Set;
import java.util.concurrent.ConcurrentHashMap;
import java.util.concurrent.CopyOnWriteArrayList;

import org.gsma.joyn.Intents;
import org.gsma.joyn.JoynServiceException;
import org.gsma.joyn.JoynServiceListener;
import org.gsma.joyn.capability.Capabilities;
import org.gsma.joyn.capability.CapabilitiesListener;
import org.gsma.joyn.capability.CapabilityService;
import org.gsma.joyn.contacts.ContactsProvider;

/**
 * This class manages the APIs which are used by plug-in, providing a convenient
 * way for API invocations.
 */
public class PluginApiManager {
    public static final String TAG = "PluginApiManager";
    private static PluginApiManager sInstance = null;
    private MyCapabilitiesListener mMyCapabilitiesListener = null;
    private CapabilityService mCapabilitiesApi = null;
    private Context mContext = null;
    private static final int MAX_CACHE_SIZE = 2048;
    private final LruCache<String, VideoCapabilityContactInformation> mContactsCache =
        new LruCache<String, VideoCapabilityContactInformation>(MAX_CACHE_SIZE);
    private final List<CapabilitiesChangeListener> mCapabilitiesChangeListenerList =
        new ArrayList<CapabilitiesChangeListener>();

    private static final String INTENT_VIDEO_CAPABILITIES =
        "com.mediatek.commonpresence.capability.VIDEO_CAPABILITIES";
    /**
     * MIME type for RCSE capabilities.
     */
    private static final String MIMETYPE_VIDEO_SHARING = ContactsProvider.MIME_TYPE_IR94_VIDEO_CALL;
    private static final int CONTACT_VIDEO_CAPABLE = 1;
    private static final int CONTACT_VIDEO_CAPABLE_DUPLEX = 2;

    /**
     * The CapabilitiesChangeListener defined as a listener to notify the
     * specify observer that the capabilities has been changed.
     *
     * @see CapabilitiesChangeEvent
     */
    public interface CapabilitiesChangeListener {

        /**
         * On capabilities changed.
         *
         * @param contact
         *            the contact
         * @param contactInformation
         *            the contact information
         */
        void onCapabilitiesChanged(String contact,
                VideoCapabilityContactInformation info);

    }

    /**
     * Register the CapabilitiesChangeListener.
     *
     * @param listener
     *            The CapabilitiesChangeListener used to register
     */
    public void addCapabilitiesChangeListener(
            CapabilitiesChangeListener listener) {
        Log.d(TAG, "addCapabilitiesChangeListener(), listener = " + listener);
        mCapabilitiesChangeListenerList.add(listener);
    }

    /**
     * Unregister the CapabilitiesChangeListener.
     *
     * @param listener
     *            The CapabilitiesChangeListener used to unregister
     */
    public void removeCapabilitiesChangeListener(
            CapabilitiesChangeListener listener) {
        Log.d(TAG, "removeCapabilitiesChangeListener(), listener = " + listener);
        mCapabilitiesChangeListenerList.remove(listener);
    }

    /**
     * Get the instance of CapabilityApi.
     *
     * @return The instance of CapabilityApi
     */
    public CapabilityService getCapabilityApi() {
        Log.d(TAG, "getCapabilityApi(), mCapabilitiesApi = " + mCapabilitiesApi);
        return mCapabilitiesApi;
    }

    /**
     * The class including some informations of contact: whether contact is
     * video capable etc.
     */
    public static class VideoCapabilityContactInformation {
        public boolean isVideoCapable = false;
        // may contain some more info like duplex video call etc in future.

    }

    /**
     * Init the data.
     */
    private void initData() {
        Log.d(TAG, "initData entry");
        Thread thread = new Thread() {
            public void run() {
                Looper.prepare();
                queryContactsPresence();
            }
        };
        thread.start();
        IntentFilter filter = new IntentFilter();
        filter.addAction(INTENT_VIDEO_CAPABILITIES);
        mContext.registerReceiver(mBroadcastReceiver, filter);
        Log.d(TAG, "initData exit");
    }

    /**
     * Query contacts presence. Fill contacts cache with video capability
     * information.
     */
    private void queryContactsPresence() {
        Log.d(TAG, "queryContactsPresence entry");
        List<String> rcsNumbers = new ArrayList<String>();
        String[] projection = { Phone.CONTACT_ID, Phone._ID, Data.MIMETYPE,
                Data.DATA1, Data.DATA2 };
        String selection = Data.MIMETYPE + "=? AND " + "(" + Data.DATA2
                + "=? OR " + Data.DATA2 + "=? )";
        String[] selectionArgs = { MIMETYPE_VIDEO_SHARING,
                Long.toString(CONTACT_VIDEO_CAPABLE),
                Long.toString(CONTACT_VIDEO_CAPABLE_DUPLEX) };
        Cursor cursor = mContext.getContentResolver().query(Data.CONTENT_URI,
                projection, selection, selectionArgs, null);
        try {
            if (cursor != null) {
                while (cursor.moveToNext()) {
                    String number = cursor.getString(cursor
                            .getColumnIndex(Data.DATA1));
                    long phoneId = cursor.getLong(cursor
                            .getColumnIndex(Phone._ID));
                    long contactId = cursor.getLong(cursor
                            .getColumnIndex(Phone.CONTACT_ID));
                    Log.d(TAG, "queryContactsPresence(), number  = " + number
                            + " phoneId = " + phoneId + " contactId = "
                            + contactId);
                    VideoCapabilityContactInformation info =
                    new VideoCapabilityContactInformation();
                    info.isVideoCapable = true;
                    synchronized (mContactsCache) {
                        mContactsCache.put(number, info);
                    }
                }
            }
        } finally {
            if (cursor != null) {
                cursor.close();
            }
        }
    }

    /**
     * Query contact presence.
     *
     * @param number
     *            the number
     */
    public void requestContactPresence(final String number) {
        Log.d(TAG, "requestContactPresence entry, number is " + number);
        AsyncTask.execute(new Runnable() {
            @Override
            public void run() {
                Log.d(TAG, "requestContactPresence() in async task");
                if (mCapabilitiesApi == null) {
                    Log.d(TAG,
                            "queryContactPresence mCapabilitiesApi is null, number is "
                                    + number);
                } else {
                    try {
                        mCapabilitiesApi.requestContactCapabilities(number);

                    } catch (JoynServiceException e) {
                        e.printStackTrace();
                    }
                }
                Log.d(TAG, "queryContactPresence() leave async task");
            }
        });
        Log.d(TAG, "queryContactPresence exit");
    }

    /**
     * Query a series of phone number.
     *
     * @param numbers
     *            The phone numbers list need to query
     */
    public void requestNumbersPresence(final List<String> numbers) {
        Log.d(TAG, "requestNumbersPresence entry, numbers: " + numbers
                + ", mCapabilitiesApi= " + mCapabilitiesApi);
        AsyncTask.execute(new Runnable() {
            @Override
            public void run() {
                if (mCapabilitiesApi != null) {
                    for (String number : numbers) {
                        Log.d(TAG, "requestNumbersPresence number: " + number);

                        try {

                            mCapabilitiesApi.requestContactCapabilities(number);

                        } catch (JoynServiceException e) {
                            e.printStackTrace();
                        }
                    }
                }
            }
        });
    }

    /**
     * Obtain the phone numbers from a specific contact id.
     *
     * @param contactId
     *            The contact id
     * @return The phone numbers of the contact id
     */
    public List<String> getNumbersByContactId(long contactId) {
        Log.d(TAG, "getNumbersByContactId entry, contact id is: " + contactId);
        List<String> list = new ArrayList<String>();
        String[] projection = { Phone.NUMBER };
        String selection = Phone.CONTACT_ID + "=? ";
        String[] selectionArgs = { Long.toString(contactId) };
        Cursor cur = mContext.getContentResolver().query(Phone.CONTENT_URI,
                projection, selection, selectionArgs, null);
        try {
            if (cur != null) {
                while (cur.moveToNext()) {
                    String number = cur.getString(0);
                    if (!TextUtils.isEmpty(number)) {
                        list.add(number.replace(" ", ""));
                    } else {
                        Log.w(TAG, "getNumbersByContactId() invalid number: "
                                + number);
                    }
                }
            }
        } finally {
            if (cur != null) {
                cur.close();
            }
        }
        Log.d(TAG, "getNumbersByContactId exit, list: " + list);
        return list;
    }

    /**
     * This method should only be called from ApiService, for APIs
     * initialization.
     *
     * @param context
     *            The Context of this application.
     * @return true If initialize successfully, otherwise false.
     */
    public static synchronized boolean initialize(Context context) {
        Log.d(TAG, "initialize() entry");
        if (null != sInstance) {
            Log.w(TAG, "initialize() sInstance has existed, "
                    + "is it really the first time you call this method?");
            return true;
        } else {
            if (null != context) {
                PluginApiManager apiManager = new PluginApiManager(context);
                sInstance = apiManager;
                return true;
            } else {
                Log.e(TAG, "initialize() the context is null");
                return false;
            }
        }
    }

    /**
     * Get the context.
     *
     * @return Context
     */
    public Context getContext() {
        return mContext;
    }

    /**
     * Get the instance of PluginApiManager.
     *
     * @return The instance of ApiManager, or null if the instance has not been
     *         initialized.
     */
    public static PluginApiManager getInstance() {
        if (null == sInstance) {
            throw new RuntimeException(
                    "Please call initialize() before calling this method");
        }
        return sInstance;
    }

    /**
     * Instantiates a new plugin api manager.
     *
     * @param context
     *            the context
     */
    private PluginApiManager(Context context) {
        Log.d(TAG, "PluginApiManager(), context = " + context);
        mContext = context;
        // connect to capability TAPI
        mMyCapabilitiesListener = new MyCapabilitiesListener();
        MyJoynServiceListener myJoynServiceListener = new MyJoynServiceListener();
        mCapabilitiesApi = new CapabilityService(context, myJoynServiceListener);
        mCapabilitiesApi.connect();
        // fill cache information for video capability
        initData();
    }

    /**
     * The listener interface for receiving myJoynService events. The class that
     * is interested in processing a myJoynService event implements this
     * interface, and the object created with that class is registered with a
     * component using the component's addMyJoynServiceListener method. When the
     * myJoynService event occurs, that object's appropriate method is invoked.
     *
     * @see MyJoynServiceEvent
     */
    public class MyJoynServiceListener implements JoynServiceListener {

        /**
         * On service connected.
         */
        @Override
        public void onServiceConnected() {
            try {
                Log.d(TAG, "onServiceConnected");
                PluginApiManager.this.mCapabilitiesApi
                        .addCapabilitiesListener(mMyCapabilitiesListener);
            } catch (JoynServiceException e) {
                e.printStackTrace();
            }

        }

        /**
         * On service disconnected.
         *
         * @param error
         *            the error
         */
        @Override
        public void onServiceDisconnected(int error) {
            try {
                Log.d(TAG, "onServiceDisConnected");
                if (PluginApiManager.this.mCapabilitiesApi != null) {
                    PluginApiManager.this.mCapabilitiesApi
                            .removeCapabilitiesListener(mMyCapabilitiesListener);
                    PluginApiManager.this.mCapabilitiesApi = null;
                }
            } catch (JoynServiceException e) {
                e.printStackTrace();
            }

        }

    }

    /**
     * The listener interface for receiving myCapabilities events. The class
     * that is interested in processing a myCapabilities event implements this
     * interface, and the object created with that class is registered with a
     * component using the component's addMyCapabilitiesListener method. When
     * the myCapabilities event occurs, that object's appropriate method is
     * invoked.
     *
     * @see MyCapabilitiesEvent
     */
    public class MyCapabilitiesListener extends CapabilitiesListener {

        /**
         * On capabilities received.
         *
         * @param contact
         *            the contact
         * @param capabilities
         *            the capabilities
         */
        @Override
        public void onCapabilitiesReceived(final String contact,
                Capabilities capabilities) {

            Log.d(TAG, "onCapabilityChanged(), contact = " + contact
                    + ", capabilities = " + capabilities + ", video sharing= "
                    + capabilities.isIR94_VideoCallSupported());
            if (null != contact && capabilities != null) {
                VideoCapabilityContactInformation info = mContactsCache
                        .get(contact);
                if (info == null) {
                    Log.d(TAG, "cache does not exist, so create a object.");
                    info = new VideoCapabilityContactInformation();
                    if (capabilities.isIR94_VideoCallSupported() == true) {
                        info.isVideoCapable = true;
                        synchronized (mContactsCache) {
                            mContactsCache.put(contact, info);
                        }
                        for(CapabilitiesChangeListener listener : mCapabilitiesChangeListenerList){
                            if (listener != null) {
                                Log.d(TAG, "Notify the listener");
                                listener.onCapabilitiesChanged(contact, info);
                            }
                        }
                    }

                } else {
                    Log.d(TAG, "cache exists");
                    if (info.isVideoCapable != capabilities
                            .isIR94_VideoCallSupported()) {
                        info.isVideoCapable = capabilities
                                .isIR94_VideoCallSupported();
                        synchronized (mContactsCache) {
                            mContactsCache.put(contact, info);
                        }
                        Log.d(TAG, "put capability into cache");
                        for(CapabilitiesChangeListener listener : mCapabilitiesChangeListenerList){
                            if (listener != null) {
                                Log.d(TAG, "Notify the listener");
                                listener.onCapabilitiesChanged(contact, info);
                            }
                        }
                    }
                }

            } else {
                Log.d(TAG,
                        "onCapabilityChanged()-invalid contact or capabilities");
            }
        }
    }

    /**
     * Clear all the information in the mContactsCache.
     */
    public void cleanContactCache() {
        Log.d(TAG, "cleanContactCache() entry");
        mContactsCache.evictAll();
    }

    /**
     * Check whether a number is video call capable or not.
     *
     * @param number
     *            The number to query
     * @return True if number is a video capable contact, otherwise return
     *         false.
     */
    public boolean isVideoCallCapable(String number) {
        Log.d(TAG, "isVideoCallCapable entry: " + number);
        if (number == null) {
            Log.w(TAG, "number is null");
            return false;
        }
        VideoCapabilityContactInformation info = mContactsCache.get(number);
        if (info != null) {
            Log.d(TAG, "video call capable true");
            return true;
        } else {
            return false;
        }
    }
    //currently for Auto test case
    private BroadcastReceiver mBroadcastReceiver = new BroadcastReceiver() {
        @Override
        public void onReceive(Context context, final Intent intent) {
            String action = intent.getAction();
            Log.d(TAG, "Intent received:" + action);
            if (INTENT_VIDEO_CAPABILITIES.equals(action)) {

                String number = intent.getStringExtra("number");
                Log.d(TAG, "Intent received:" + action + ",number:" + number);
                VideoCapabilityContactInformation info = new VideoCapabilityContactInformation();
                info.isVideoCapable = true;
                synchronized (mContactsCache) {
                    mContactsCache.put(number, info);
                }
            }
        }
    };

}
