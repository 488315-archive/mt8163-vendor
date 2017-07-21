 /*
 * Copyright (C) 2009 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package com.mediatek.rcs.pam.util;

import java.util.HashSet;
import java.util.Set;

import android.content.Context;
import android.database.Cursor;
import android.provider.Telephony.MmsSms;
import android.provider.Telephony.Sms.Conversations;
import android.util.Log;

/**
 * Cache for information about draft messages on conversations.
 */
public class DraftCache {
    private static final String TAG = "DraftCache";

    private static DraftCache sInstance;

    private final Context mContext;

    private boolean mSavingDraft;   // true when we're in the process of saving a draft. Check this
                                    // before deleting any empty threads from the db.
    private final Object mSavingDraftLock = new Object();

    private HashSet<Long> mDraftSet = new HashSet<Long>(4);
    private final Object mDraftSetLock = new Object();
    private final HashSet<OnDraftChangedListener> mChangeListeners
            = new HashSet<OnDraftChangedListener>(1);
    private final Object mChangeListenersLock = new Object();

    public interface OnDraftChangedListener {
        void onDraftChanged(long threadId, boolean hasDraft);
    }

    private DraftCache(Context context) {

        mContext = context;
        refresh();
    }

    static final String[] DRAFT_PROJECTION = new String[] {
        Conversations.THREAD_ID           // 0
    };

    static final int COLUMN_DRAFT_THREAD_ID = 0;

    /** To be called whenever the draft state might have changed.
     *  Dispatches work to a thread and returns immediately.
     */
    public void refresh() {

        Thread thread = new Thread(new Runnable() {
            @Override
            public void run() {
                rebuildCache();
            }
        }, "DraftCache.refresh");
        thread.setPriority(Thread.MIN_PRIORITY);
        thread.start();
    }

    /** Does the actual work of rebuilding the draft cache.
     */
    private void rebuildCache() {
    	
    }

    /** Updates the has-draft status of a particular thread on
     *  a piecemeal basis, to be called when a draft has appeared
     *  or disappeared.
     */
    public void setDraftState(long threadId, boolean hasDraft) {
    	
    }

    /** Returns true if the given thread ID has a draft associated
     *  with it, false if not.
     */
    public boolean hasDraft(long threadId) {
        synchronized (mDraftSetLock) {
            return mDraftSet.contains(threadId);
        }
    }

    public void addOnDraftChangedListener(OnDraftChangedListener l) {
        synchronized (mChangeListenersLock) {
            mChangeListeners.add(l);
        }
    }

    public void removeOnDraftChangedListener(OnDraftChangedListener l) {
        synchronized (mChangeListenersLock) {
            mChangeListeners.remove(l);
        }
    }

    public void setSavingDraft(final boolean savingDraft) {
        synchronized (mSavingDraftLock) {
            mSavingDraft = savingDraft;
        }
    }

    public boolean getSavingDraft() {
        synchronized (mSavingDraftLock) {
            return mSavingDraft;
        }
    }

    /**
     * Initialize the global instance. Should call only once.
     */
    public static void init(Context context) {
        sInstance = new DraftCache(context);
    }

    /**
     * Get the global instance.
     */
    public static DraftCache getInstance() {
        return sInstance;
    }

    public void dump() {
        Log.i(TAG, "dump:");
        for (Long threadId : mDraftSet) {
            Log.i(TAG, "  tid: " + threadId);
        }
    }

    private void log(String format, Object... args) {
        String s = String.format(format, args);
        Log.d(TAG, "[DraftCache/" + Thread.currentThread().getId() + "] " + s);
    }
}
