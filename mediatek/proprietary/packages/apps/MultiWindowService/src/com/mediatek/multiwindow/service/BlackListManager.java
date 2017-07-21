package com.mediatek.multiwindow.service;

import com.mediatek.common.multiwindow.IMWBlackList;

import android.util.Slog;
import java.io.FileDescriptor;
import java.io.PrintWriter;
import java.util.List;
import android.util.Log;
import android.os.SystemProperties;
import android.content.Context;
import android.content.BroadcastReceiver;
import android.content.IntentFilter;
import android.content.Intent;
import android.os.Handler;
import android.os.IBinder;
import android.os.Message;
import android.os.Bundle;
import android.content.pm.PackageManager;
import android.os.Binder;
import java.text.DateFormat;
import java.util.Date;

public class BlackListManager extends IMWBlackList.Stub {
    private Context mContext;
    private static boolean DBG = false;
    private static final String TAG = "MW.BlackListManager";

    private static final int MSG_WRITE_TO_XML = 1;
    private static final int MSG_READ_FROM_XML = 2;
    static final int WRITE_TO_XML_DELAY = 10 * 1000;  // 10 seconds

    BlackListParser mBlackListParser;

    final Handler mHandler = new Handler() {
        @Override
        public void handleMessage(Message msg) {
            switch (msg.what) {
                case MSG_WRITE_TO_XML: {
                    mBlackListParser.updateBlackListToXml();
                    break;
                }
                case MSG_READ_FROM_XML: {
                    mBlackListParser.LoadBlackList();
                    break;
                }
            }
        }
    };

      BlackListManager(Context context) {
        Slog.v(TAG, "BlackListManager init");
          mContext = context;
          mBlackListParser = new BlackListParser(context);
          mBlackListParser.LoadBlackList();
      }

      private void schedulewriteToXml() {
        if (!mHandler.hasMessages(MSG_WRITE_TO_XML)) {
            mHandler.sendEmptyMessageDelayed(MSG_WRITE_TO_XML, WRITE_TO_XML_DELAY);
        }
    }

    private void scheduleReadFromXml() {
        if (!mHandler.hasMessages(MSG_READ_FROM_XML)) {
            mHandler.sendEmptyMessage(MSG_READ_FROM_XML);
        }
    }

    @Override
    public void dump(FileDescriptor fd, PrintWriter pw, String[] args) {
        if (mContext
                .checkCallingOrSelfPermission("android.permission.DUMP") != PackageManager.PERMISSION_GRANTED) {
            pw.println("Permission Denial: can't dump MultiWindow from from pid="
                    + Binder.getCallingPid()
                    + ", uid="
                    + Binder.getCallingUid());
            return;
        }

        pw.println("MW.BlackList");
        // Print the current date and time
        String currentDateTimeString = DateFormat.getDateTimeInstance()
                .format(new Date());
        pw.println("Dump time : " + currentDateTimeString);
        pw.println("Dump Black List: ");
        mBlackListParser.dumpAllList(fd, pw);
        pw.println();
    }

    @Override
    public boolean shouldChangeConfig(String packageName) {
        return mBlackListParser.shouldChangeConfig(packageName);
    }

    @Override
    public boolean shouldRestartWhenMiniMax(String packageName) {
        return mBlackListParser.shouldRestartWhenMiniMax(packageName);
    }

    @Override
    public boolean inWhiteList(String packageName) {
        return mBlackListParser.inWhiteList(packageName);
    }

    @Override
    public List<String> getWhiteList() {
        return mBlackListParser.getWhiteList();
    }

    @Override
    public void addIntoWhiteList(String packageName) {
          mBlackListParser.addIntoWhiteList(packageName);
    }

    @Override
    public void addMoreIntoWhiteList(List<String> packageList) {
          for (String pkg : packageList) {
              mBlackListParser.addIntoWhiteList(pkg);
          }
    }

    @Override
    public void removeFromWhiteList(String packageName) {
          mBlackListParser.removeFromWhiteList(packageName);
    }
}