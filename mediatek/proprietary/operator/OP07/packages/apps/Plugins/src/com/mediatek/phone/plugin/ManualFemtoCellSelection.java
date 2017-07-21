/* Copyright Statement:
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein
 * is confidential and proprietary to MediaTek Inc. and/or its licensors.
 * Without the prior written permission of MediaTek inc. and/or its licensors,
 * any reproduction, modification, use or disclosure of MediaTek Software,
 * and information contained herein, in whole or in part, shall be strictly prohibited.
 */
/* MediaTek Inc. (C) 2010. All rights reserved.
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

package com.mediatek.phone.plugin;

import android.app.ProgressDialog;
import android.content.Context;
import android.content.DialogInterface;
import android.os.AsyncResult;
import android.os.Handler;
import android.os.Message;
import android.preference.Preference;
import android.preference.PreferenceScreen;
import android.util.Log;
import android.widget.Toast;

import com.android.internal.telephony.CommandException;
import com.android.internal.telephony.Phone;
import com.android.internal.telephony.PhoneFactory;

import com.mediatek.internal.telephony.FemtoCellInfo;
import com.mediatek.op07.plugin.R;

import java.util.ArrayList;

/**
 * Femtocell selection handling.
 */
public class ManualFemtoCellSelection extends Preference {
    private static final String LOG_TAG = "ManualFemtoCellSelection";

    private final Context mContext;
    private Phone mPhone;
    private MyHandler mHandler;
    private ProgressDialog mPd = null;
    private PreferenceScreen mPrefSet = null;
    private ArrayList<FemtoCellInfo> mFemtoList;
    private boolean mFemtocellSelected = false;
    private boolean mFemtocellListFetching = false;
    private final Object mLock = new Object();
    /**
     * Default constructor for Femtocell selection.
     * @param context context of host app
     * @param prefSet gost app prefernce screen
     */
    public ManualFemtoCellSelection(Context context, PreferenceScreen prefSet) {
        super(prefSet.getContext());
        mFemtocellSelected = false;
        mFemtocellListFetching = false;
        mPhone = PhoneFactory.getDefaultPhone();
        mHandler = new MyHandler();
        mContext = context;
        mPrefSet = prefSet;
    }


    private void showProgressDialog() {
        // TODO Auto-generated method stub
        Log.d(LOG_TAG, "showProgressDialog");
        mPd = new ProgressDialog(mPrefSet.getContext());
        mPd.setMessage(mContext.getText(R.string.fetching_femtocell_list));
        mPd.setCancelable(true);
        mPd.setCanceledOnTouchOutside(false);
        mPd.setOnDismissListener(new DialogInterface.OnDismissListener() {
            @Override
            public void onDismiss(DialogInterface dialog) {
                if (mFemtocellListFetching) {
                   mFemtocellListFetching = false;
                   Log.d(LOG_TAG, "showProgressDialog ondismiss");
                   mPhone.abortFemtoCellList(
                        mHandler.obtainMessage(MyHandler.MESSAGE_ABORT_FEMTOCELL_LIST));
                }
                if (mPd != null) {
                    mPd = null;
                }
            }
        });
        mPd.setIndeterminate(true);
        mPd.show();
    }

    @Override
    protected void  onClick() {
        super.onClick();
        Log.d(LOG_TAG, "showProgressDialog onClick");
        showProgressDialog();
        mPhone.getFemtoCellList(null, 0,
                    mHandler.obtainMessage(MyHandler.MESSAGE_GET_FEMTOCELL_LIST));
        mFemtocellListFetching = true;
    }

    /**
     * Handler class to handle Femtocell selection.
     */
    private class MyHandler extends Handler {

        private static final int MESSAGE_GET_FEMTOCELL_LIST = 0;
        private static final int MESSAGE_SELECT_FEMTOCELL = 1;
        private static final int MESSAGE_ABORT_FEMTOCELL_LIST = 2;
        private static final int MESSAGE_ABORT_FEMTOCELL_SELECTION = 3;

        @Override
        public void handleMessage(Message msg) {
            switch (msg.what) {
                case MESSAGE_GET_FEMTOCELL_LIST:
                    handleGetFemtocellList(msg);
                    break;

                case MESSAGE_SELECT_FEMTOCELL:
                    handleSelectFemtoCell(msg);
                    break;
                case MESSAGE_ABORT_FEMTOCELL_LIST:
                    handleAbortFemtocellList(msg);
                    break;
                case MESSAGE_ABORT_FEMTOCELL_SELECTION:
                    handleAbortFemtocellSelecton(msg);
                    break;
                default:
                    break;
            }
        }

        private void handleGetFemtocellList(Message msg) {
             Log.d(LOG_TAG, "Handle getFemtoCellList done.");
            //setScreenEnabled(true);
            mFemtocellListFetching = false;
            Log.d(LOG_TAG, "handleGetFemtocellList");
            AsyncResult ar = (AsyncResult) msg.obj;
            if (ar.exception != null) {
                Log.d(LOG_TAG, "handleGetFemtocellList with exception = " + ar.exception);
                if ((ar.exception instanceof CommandException)
                        && (((CommandException) (ar.exception)).getCommandError()
                                != CommandException.Error.REQUEST_CANCELLED)) {
                    if ((mPd != null) && mPd.isShowing()) {
                    mPd.dismiss();
                }
                }
            } else {
                Log.d(LOG_TAG, "handleGetFemtocellList no exception");
                mFemtoList = (ArrayList<FemtoCellInfo>) ar.result;
                if (mFemtoList.size() != 0) {
                    startFemtocellSelection();
                } else {
                    showFailToast();
                }
            }
        }

        private void startFemtocellSelection() {
            Log.d(LOG_TAG, "startFemtocellSelection");
            for (FemtoCellInfo femtoCell : mFemtoList) {
                if (mFemtocellSelected == false) {
                   mPhone.selectFemtoCell(femtoCell,
                        mHandler.obtainMessage(MyHandler.MESSAGE_SELECT_FEMTOCELL));
                } else {
                    break;
                }
            }
            if (mFemtocellSelected == true) {
                showPassToast();
            } else {
                showFailToast();
            }
        }

        private void handleSelectFemtoCell(Message msg) {
             Log.d(LOG_TAG, "Handle selectFemtoCell done.");
            AsyncResult ar = (AsyncResult) msg.obj;
            if (ar.exception != null) {
                showFailToast();
                 Log.d(LOG_TAG, "handleSelectFemtoCellResponse with exception = " + ar.exception);
            } else {
                synchronized (mLock) {
                    mFemtocellSelected = true;
                }
                 Log.d(LOG_TAG, "handleSelectFemtoCellResponse with OK result!");
            }
        }

        private void handleAbortFemtocellList(Message msg) {
            //TODO: Handle Abort message from framework.
             Log.d(LOG_TAG, "handleAbortFemtoCellList ");
        }
        private void handleAbortFemtocellSelecton(Message msg) {
            //TODO: Handle Abort message from framework.
             Log.d(LOG_TAG, "handleAbortFemtocellSelecton ");
        }
    }
    private void showFailToast() {
        if ((mPd != null) && mPd.isShowing()) {
        mPd.dismiss();
        }
        Toast.makeText(mContext, mContext.getString(R.string.femto_fail), Toast.LENGTH_LONG).show();
    }
    private void showPassToast() {
        if ((mPd != null) && mPd.isShowing()) {
        mPd.dismiss();
        }
        Toast.makeText(mContext, mContext.getString(R.string.femto_pass), Toast.LENGTH_LONG).show();
    }
}
