package com.mediatek.multiwindow.service;

import com.mediatek.multiwindow.service.MultiWindowApplication;
import com.mediatek.common.multiwindow.IMultiWindowManager;
import com.mediatek.common.multiwindow.IMWAmsCallback;
import com.mediatek.common.multiwindow.IMWWmsCallback;
import com.mediatek.common.multiwindow.IMWSystemUiCallback;

import android.app.Activity;
import android.app.ActivityManager;
import android.app.ActivityManagerNative;
import android.app.ActivityManager.RunningTaskInfo;
import android.app.ActivityManager.RunningAppProcessInfo;
import android.app.ActivityManager.RecentTaskInfo;
import android.app.IProcessObserver;
import android.app.PendingIntent;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.content.pm.PackageManager;
import android.content.ComponentName;
import android.os.Binder;
import android.os.Bundle;
import android.os.Handler;
import android.os.IBinder;
import android.os.Message;
import android.os.RemoteException;
import android.os.ServiceManager;
import android.os.SystemProperties;
import android.provider.Settings;
import android.text.TextUtils;
import android.util.ArrayMap;
import android.util.Log;
import android.util.Slog;
import android.util.SparseArray;
import android.util.SparseBooleanArray;
import android.util.TimeUtils;
import java.io.IOException;
import java.io.ByteArrayOutputStream;
import java.io.FileDescriptor;
import java.io.PrintWriter;
import java.text.SimpleDateFormat;
import java.text.DateFormat;
import java.util.ArrayList;
import java.util.List;
import java.util.Arrays;
import java.util.Calendar;
import java.util.Collections;
import java.util.Comparator;
import java.util.Date;
import java.util.HashMap;
import java.util.Iterator;
import java.util.LinkedList;
import java.util.Locale;
import java.util.TimeZone;
import java.util.List;
import android.os.Handler;
import android.os.HandlerThread;
import android.graphics.Rect;
public class MultiWindowManager extends IMultiWindowManager.Stub {

    private static final String TAG = "MultiWindowManager";
    private static boolean DBG_MWS = false;

    private Context mContext;
    private Object mLock = new Object();
    private ActivityManager mActivityManager;

    // Callbacks
    private IMWAmsCallback mAMSCb;
    private IMWWmsCallback mWMSCb;
    private IMWSystemUiCallback mSystemUiCb;

    private final SparseArray<StackInfo> mStackInfos = new SparseArray<StackInfo>();
    private final SparseArray<TaskInfo> mTaskInfos = new SparseArray<TaskInfo>();
    private final HashMap<IBinder, ActivityInfo> mActivityInfoMap = new HashMap<>();

    // winToken, appToken
    private final HashMap<IBinder, IBinder> mTokenMap = new HashMap<>();

    // Add for Split-Screen Mode
    private final StackPos[] mStackPosInfos = new StackPos[2];
    private boolean mIsSplitMode = false;

    public MultiWindowManager(Context context) {
        if (true) Slog.v(TAG, "MultiWindowManager init");
        mContext = context;
        mActivityManager = (ActivityManager) mContext
            .getSystemService(Context.ACTIVITY_SERVICE);
        for (int i = 0; i < mStackPosInfos.length; i++) {
            mStackPosInfos[i] = new StackPos();
        }
    }

    /**
     * By this cb, we can access AMS directly.
     */
    @Override
    public void setAMSCallback(IMWAmsCallback cb) {
        mAMSCb = cb;
    }

    /**
     * By this cb, we can access WMS directly.
     */
    @Override
    public void setWMSCallback(IMWWmsCallback cb) {
        if (mWMSCb == null) {
            mWMSCb = cb;
        }
    }

    /**
     * By this cb, we can access SystemUI directly.
     */
    @Override
    public void setSystemUiCallback(IMWSystemUiCallback cb) {
        mSystemUiCb = cb;
    }

    /**
     * Moves an activity, and all of the other activities within the same task, to
     * the front of the history stack. The activity's order within the task is
     * unchanged.
     *
     * @param token A reference to the activity we wish to move.
     */
    @Override
    public void moveActivityTaskToFront(IBinder appToken) {
        if (mAMSCb == null) {
            Slog.e(TAG, "moveActivityTaskToFront, mAMSCb is null");
            return;
        }
        /// M: [ALPS01891295] If Task is being removed, do not move it to
        /// front. @{
        synchronized (mLock) {
            TaskInfo taskInfo = getTaskInfoLocked(appToken);
            if (taskInfo != null && taskInfo.mPendingRemove) {
                Slog.e(TAG, "moveActivityTaskToFront: task "
                        + taskInfo.mTaskId + " is pending remove.");
                return;
            }
        }
        /// @}
        if (DBG_MWS) Slog.v(TAG, "moveActivityTaskToFront appToken = "
                + appToken);
        try {
            mAMSCb.moveActivityTaskToFront(appToken);
        } catch (RemoteException e) {
            Slog.e(TAG, "moveActivityTaskToFront" + e);
        }
    }

    /**
     * Completely remove the activity's task. TODO Wish to remove all the tasks of
     * the Activity's stack.
     *
     * @param token A reference to the activity we wish to close.
     */
    @Override
    public void closeWindow(IBinder appToken) {
        TaskInfo taskInfo;
        synchronized (mLock) {
            /// M: [ALPS01891295] If Task is being removed, do not move it to
            /// front.
            taskInfo = getTaskInfoLocked(appToken);
            if (taskInfo == null) {
                Slog.e(TAG, "closeWindow: not found task by appToken:"
                        + appToken);
                return;
            }
            taskInfo.mPendingRemove = true;
        }

        /// M: [ALPS01885359] Music control panel still alive in notification
        /// list.
        long ident = Binder.clearCallingIdentity();
        try {
            mActivityManager.removeTask(taskInfo.mTaskId
                /*,  0 ActivityManager.REMOVE_TASK_KILL_PROCESS*/);
        } finally {
            Binder.restoreCallingIdentity(ident);
        }
        if (DBG_MWS) Slog.w(TAG, "closeWindow remove task! taskInfo.id = "
                    + taskInfo.mTaskId);

    }

    /**
     * Restore or Max the activity's stack.
     *
     * @param token The Binder token referencing the Activity we want to
     *            restore/max.
     * @param toMax If true, move floating task to normal stack. Otherwise, move
     *            normal task to a new floating stack
     */
    @Override
    public void restoreWindow(IBinder token, boolean toMax) {
        if (mAMSCb == null) {
            Slog.e(TAG, "restoreWindow, mAMSCb is null");
            return;
        }
        try {
            mAMSCb.restoreStack(token, toMax);
        } catch (RemoteException e) {
            Slog.e(TAG, "restoreWindow" + e);
        }
    }

    /**
     * Stick Stack, this is a toggle function let this stack can always keep in
     * top.
     *
     * @param token A reference to the activity we want to stick.
     * @param isSticky If true, always keep the stack in top.
     */
    @Override
    public void stickWindow(IBinder appToken, boolean isSticky) {
        synchronized (mLock) {
            StackInfo stackInfo = getStackInfoLocked(appToken);
            if (stackInfo == null) {
                Slog.e(TAG, "stickWindow: not found stack by appToken: "
                        + appToken);
                return;
            }
            stackInfo.mSticky = isSticky;
        }
    }

    /**
     * Check the stack is floating or not.
     */
    @Override
    public boolean isFloatingStack(int stackId) {
        synchronized (mLock) {
            StackInfo stackInfo = mStackInfos.get(stackId);
            if (stackInfo != null) {
                return stackInfo.mFloating;
            }
        }
        return false;
    }

    /**
     * Check the stack is sticky or not.
     */
    @Override
    public boolean isStickStack(int stackId) {
        synchronized (mLock) {
            StackInfo stackInfo = mStackInfos.get(stackId);
            if (stackInfo != null) {
                return stackInfo.mSticky;
            }
        }
        return false;
    }

    /**
     * Check whether the task is in Mini/Max status.
     */
    @Override
    public boolean isInMiniMax(int taskId) {
        synchronized (mLock) {
            TaskInfo taskInfo = mTaskInfos.get(taskId);
            if (taskInfo != null) {
                return taskInfo.mInMiniMax;
            }
        }
        return false;
    }

    @Override
    public void moveFloatingWindow(int disX, int disY) {
        if (mWMSCb == null) {
            Slog.e(TAG, "moveFloatingWindow, mWMSCb is null");
            return;
        }

        try {
            mWMSCb.moveFloatingWindow(disX, disY);
        } catch (RemoteException e) {
            Slog.e(TAG, "moveFloatingWindow" + e);
        }
    }

    @Override
    public void resizeFloatingWindow(int direction, int deltaX,
            int deltaY) {
        if (mWMSCb == null) {
            Slog.e(TAG, "resizeFloatingWindow, mWMSCb is null");
            return;
        }

        try {
            mWMSCb.resizeFloatingWindow(direction, deltaX, deltaY);
        } catch (RemoteException e) {
            Slog.e(TAG, "resizeFloatingWindow" + e);
        }
    }

    @Override
    public void enableFocusedFrame(boolean enable) {
        if (mWMSCb == null) {
            Slog.e(TAG, "enableFocusedFrame, mWMSCb is null");
            return;
        }

        try {
            mWMSCb.enableFocusedFrame(enable);
        } catch (RemoteException e) {
            Slog.e(TAG, "enableFocusedFrame" + e);
        }
    }

    /**
     * Record the mini/max status for the task, and notify WMS.
     */
    @Override
    public void miniMaxTask(int taskId) {
        if (mWMSCb == null) {
            Slog.e(TAG, "miniMaxTask, mWMSCb is null");
            return;
        }

        if (DBG_MWS)
            Slog.v(TAG, "miniMaxTask taskId = " + taskId);
        synchronized (mLock) {
            TaskInfo taskInfo = mTaskInfos.get(taskId);
            if (taskInfo != null) {
                taskInfo.mInMiniMax = true;
            }
        }

        try {
            mWMSCb.miniMaxTask(taskId);
        } catch (RemoteException e) {
            Slog.e(TAG, "miniMaxTask" + e);
        }
    }

    /**
     * Called by WindowManagerService to contorl restore button on systemUI
     * module.
     */
    @Override
    public void showRestoreButton(boolean flag) {
        try {
            Slog.v(TAG, "showRestoreButton flag " + flag);
            mSystemUiCb.showRestoreButton(flag);
        } catch (RemoteException e) {
            Slog.e(TAG, "showRestoreButton" + e);
        }
    }

    /**
     * Check the activity is sticy or not.
     *
     * @param token A referencing to the Activity that we want to check.
     */
    @Override
    public boolean isSticky(IBinder appToken) {
        if (DBG_MWS)
            Slog.v(TAG, "isSticky appToken = " + appToken);
        synchronized (mLock) {
            StackInfo stackInfo = getStackInfoLocked(appToken);
            if (stackInfo != null)
                return stackInfo.mSticky;
        }
        return false;
    }

    /**
     * Called by ActivityThread to Tell the MultiWindowService we have created.
     * MultiWindowService need to do operations for the activity at this point.
     *
     * @param token A referencing to the Activity that has created.
     */
    @Override
    public void activityCreated(IBinder appToken) {
        if (DBG_MWS) Slog.v(TAG, "activityCreated appToken = " + appToken);
        synchronized (mLock) {
            TaskInfo taskInfo = getTaskInfoLocked(appToken);
            if (taskInfo != null) {
                taskInfo.mInMiniMax = false;
            }
        }
    }

    /**
     * Called by ActivityManagerService when task has been added.
     * MultiWindowService need to maintain the all running task infos, so, we
     * should synchronize task infos for it.
     *
     * @param taskId Unique ID of the added task.
     */
    @Override
    public void onTaskAdded(int taskId, int stackId) {
        Slog.v(TAG, "onTaskAdded taskId=" + taskId + ",stackId=" + stackId);
        synchronized (mLock) {
            TaskInfo task = mTaskInfos.get(taskId);
            if (task == null) {
                task = new TaskInfo(taskId, stackId);
                mTaskInfos.put(taskId, task);
            } else {
                task.mStackId = stackId;
            }
        }
    }

    /**
     * Called by ActivityManagerService when task has been removed.
     * MultiWindowService need to maintain the all running task infos, so, we
     * should synchronize task infos for it.
     *
     * @param taskId Unique ID of the removed task.
     */
    @Override
    public void onTaskRemoved(int taskId, int stackId) {
        synchronized (mLock) {
            TaskInfo task = mTaskInfos.get(taskId);
            if (task != null && task.mInMiniMax) {
                Slog.e(TAG, "onTaskRemoved:Task is in MiniMax State,"
                        + "Not to remove:" + task);
                return;
            }
            mTaskInfos.remove(taskId);
        }
    }

    @Override
    public void onAppTokenAdded(IBinder appToken, int taskId) {
        synchronized (mLock) {
            ActivityInfo ac = new ActivityInfo(appToken, taskId);
            mActivityInfoMap.put(appToken, ac);
        }
    }

    @Override
    public void onAppTokenRemoved(IBinder appToken, int taskId) {
        synchronized (mLock) {
            mActivityInfoMap.remove(appToken);
        }
    }

    @Override
    public void addToken(IBinder winToken, IBinder appToken) {
        synchronized (mLock) {
            mTokenMap.put(winToken, appToken);
        }
    }

    @Override
    public void removeToken(IBinder winToken) {
        synchronized (mLock) {
            mTokenMap.remove(winToken);
        }
    }

    /**
     * Set the stack floating, when create a new floating stack.
     *
     * @param stackId The unique identifier of the stack.
     */
    @Override
    public void addStack(int stackId, boolean isFloating) {
        Slog.e(TAG, "addStack stackId = " + stackId);
        synchronized (mLock) {
            StackInfo stack = mStackInfos.get(stackId);
            if (stack != null) {
                stack.mFloating = isFloating;
            } else {
                stack = new StackInfo(stackId, isFloating);
                mStackInfos.put(stackId, stack);
            }
        }
    }

    @Override
    public int getStackPosition(int stackId) {
        Slog.e(TAG, "getStackPosition stackId = " + stackId);
        synchronized (mLock) {
            for (int i = 0; i < mStackPosInfos.length; i++) {
                if (mStackPosInfos[i].stackId == stackId) {
                    return i;
                }
            }
            return -1;
        }
    }

    @Override
    public int computeStackPosition(int stackId) {
        synchronized (mLock) {
            for (int i = 0; i < mStackPosInfos.length; i++) {
                if (mStackPosInfos[i].stackId == stackId) {
                    if (DBG_MWS) Slog.e(TAG, "computeStackPosition 1 stackId = " + stackId
                            + ", i = " + i);
                    return i;
                }
            }
            for (int i = 0; i < mStackPosInfos.length; i++) {
                if (mStackPosInfos[i].stackId == -1) {
                    mStackPosInfos[i].stackId = stackId;
                    if (DBG_MWS) Slog.e(TAG, "computeStackPosition 2 stackId = " + stackId
                            + ", i = " + i);
                    return i;
                }
            }
            if (DBG_MWS) Slog.e(TAG, "computeStackPosition stackId = " + stackId);
        }
        return -1;
    }

    @Override
    public int getNextFloatStack(int curStackId) {
        synchronized (mLock) {
            for (int i = 0; i < mStackPosInfos.length; i++) {
                int nextStackId = mStackPosInfos[i].stackId;
                if (nextStackId != curStackId) {
                    return nextStackId;
                }
            }
        }
        return -1;
    }


    @Override
    public void resetStackPosition(int stackId) {
        synchronized (mLock) {
            for (int i = 0; i < mStackPosInfos.length; i++) {
                if (mStackPosInfos[i].stackId == stackId) {
                    mStackPosInfos[i].stackId = -1;
                }
            }
            Slog.e(TAG, "resetStackPosition stackId = " + stackId);
        }
    }

    @Override
    public void removeStack(int stackId, boolean isFloating) {
        synchronized (mLock) {
            mStackInfos.remove(stackId);
            for (int i = 0; i < mStackPosInfos.length; i++) {
                if (mStackPosInfos[i].stackId == stackId) {
                    mStackPosInfos[i].stackId = -1;
                    return;
                }
            }
        }
    }
    @Override
    public boolean isFloatingByAppToken(IBinder appToken){
        synchronized (mLock) {
            return isFloatingByAppTokenLocked(appToken);
        }
    }

    private boolean  isFloatingByAppTokenLocked(IBinder appToken) {
        ActivityInfo ac = mActivityInfoMap.get(appToken);
        if (ac == null)
            return false;

        int taskId = ac.mTaskId;
        TaskInfo task = mTaskInfos.get(taskId);
        if (task == null)
            return false;

        int stackId = task.mStackId;
        StackInfo stack = mStackInfos.get(stackId);
        if (stack == null)
            return false;
        return stack.mFloating;
    }

    @Override
    public boolean isFloatingByWinToken(IBinder winToken){
        synchronized (mLock) {
            IBinder appToken = mTokenMap.get(winToken);
            return isFloatingByAppTokenLocked(appToken);
        }
    }

    @Override
    public void resizeAndMoveStack(int stackId) {
        try {
            mWMSCb.resizeAndMoveStack(stackId);
        } catch (RemoteException e) {
            Slog.v(TAG, "resizeAndMoveStack e" + e);
        }
    }

    @Override
    public boolean isSplitMode() {
        synchronized (mLock) {
            return mIsSplitMode;
        }
    }

    @Override
    public void switchMode(boolean isSplit) {
        synchronized (mLock) {

            if (mIsSplitMode == isSplit) {
                return;
            }
            mIsSplitMode = isSplit;
        }
        try {
            mAMSCb.switchMultiWindowMode();
        } catch (RemoteException e) {
            Slog.v(TAG, "switchMultiWindowMode e" + e);
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

        pw.println("MULTI WINDOW MANAGER (dumpsys multiwindow)");
        // Print the current date and time
        String currentDateTimeString = DateFormat.getDateTimeInstance()
                .format(new Date());
        pw.println("Dump time : " + currentDateTimeString);

        boolean dumpAll = false;

        int opti = 0;
        while (opti < args.length) {
            String opt = args[opti];
            if (opt == null || opt.length() <= 0 || opt.charAt(0) != '-') {
                break;
            }
            mArgs = args;
            mNextArg = 1;

            opti++;
            if ("-h".equals(opt)) {
                showUsage(pw);
                return;
            } else if ("-log".equals(opt)) {
                runDebug(fd, pw);
                return;
            } else {
                pw.println("Unknown argument: " + opt + "; use -h for help");
            }
        }

        pw.println("mAMSCb:" + mAMSCb);
        pw.println("mWMSCb:" + mWMSCb);
        pw.println("mSystemUiCb:" + mSystemUiCb);
        pw.println();

        synchronized (mLock) {
            dumpActivityInfos(fd, pw, "    ");
            dumpTaskInfos(fd, pw, "    ");
            dumpStackInfos(fd, pw, "    ");
            dumpTokenMap(fd, pw, "    ");
            dumpStackPosInfo(fd, pw, "    ");
        }
        pw.println();
    }


/*
    void addTaskInfo(int taskId, int stackId) {
        synchronized (mLock) {
            if (mTaskInfos.get(taskId) == null) {
                TaskInfo taskInfo = new TaskInfo(taskId, stackId);
                mTaskInfos.put(taskId, taskInfo);
                Slog.v(TAG, "addTaskInfo taskId=" + taskId + ",stackId=" + stackId);
            }
        }
    }

    void removeTaskInfo(int taskId, int stackId) {
        synchronized (mLock) {
            mTaskInfos.remove(taskId);
        }
        Slog.v(TAG, "onTaskRemoved taskId=" + taskId + ",stackId=" + stackId);
    }

    void resetMiniMax(int taskId) {
        if (DBG_MWS) Slog.v(TAG, "resetMiniMax taskId = " + taskId);
        synchronized (mLock) {
            TaskInfo taskInfo = mTaskInfos.get(taskId);
            if (taskInfo != null) {
                taskInfo.mInMiniMax = false;
            }
        }
    }
*/
    private String[] mArgs;
    private int mNextArg;

    private String nextArg() {
        if (mNextArg >= mArgs.length) {
            return null;
        }
        String arg = mArgs[mNextArg];
        mNextArg++;
        return arg;
    }

    private static void showUsage(PrintWriter pw) {
        pw.println("Multi Window Service dump options:");
        pw.println("  sync: sync black list immediately");
        pw.println("  monitor [on/off]: monitor the package removed/updated, and remove from black listxxx.");
        pw.println("  log [on/off]");
    }

    private void runDebug(FileDescriptor fd, PrintWriter pw) {
        String type = nextArg();
        if (type == null) {
            System.err.println("Error: didn't specify type of data to list");
            showUsage(pw);
            return;
        }
        if ("on".equals(type)) {
            DBG_MWS = true;
            pw.println("Debug Log:on");
        } else if ("off".equals(type)) {
            DBG_MWS = false;
            pw.println("Debug Log:off");
        } else {
            pw.println("Error argument: " + type + "; use -h for help");
        }
    }

    private void dumpTaskInfos(FileDescriptor fd, PrintWriter pw, String prefix) {
        // Dump task info
        pw.println("Task Info:");
        int NT = mTaskInfos.size();
        for (int taskNdx = 0; taskNdx < NT; taskNdx++) {
            TaskInfo task = mTaskInfos.valueAt(taskNdx);
            pw.print(prefix);
            pw.print("TaskInfo #"); pw.print(task.mTaskId);
            pw.print(" stackId:"); pw.println(task.mStackId);
            pw.print(prefix);
            pw.print(prefix);
            pw.print(" mInMiniMax=");pw.print(task.mInMiniMax);
            pw.println();
            //dumpActivityInfos(fd, pw, mActivityInfoMap, "        ");
        }
        pw.println();
    }

    private void dumpActivityInfos(FileDescriptor fd, PrintWriter pw,
             String prefix) {
        // Dump activity info
        pw.println("Activity Info:");
        for (IBinder appToken : mActivityInfoMap.keySet()) {
            ActivityInfo activity = mActivityInfoMap.get(appToken);
            pw.print(prefix);
            pw.print("ActivityInfo #");
            pw.println(activity.mAppToken);
            pw.print(prefix);
            pw.print(prefix);
            pw.print(" taskId:"); pw.print(activity.mTaskId);
            pw.print(" mRestoring:");
            pw.print(activity.mRestoring);
            pw.print(" mFloating:");
            pw.print(activity.mFloating);
            pw.print(" mFullScreen:");
            pw.print(activity.mFullScreen);
            pw.print(" mTranslucent:");
            pw.print(activity.mTranslucent);
            pw.println();
        }
        pw.println();
    }

    private void dumpStackInfos(FileDescriptor fd, PrintWriter pw, String prefix) {
        pw.println("Stack Info:");
        // Dump stack infos
        int size = mStackInfos.size();
        for (int stackNdx = 0; stackNdx < size; stackNdx++) {
            StackInfo stack = mStackInfos.valueAt(stackNdx);
            pw.print(prefix);
            pw.print("StackInfo #"); pw.print(stack.mStackId);
            pw.print(" mFloating:"); pw.print(stack.mFloating);
            pw.print(prefix);
            pw.print(prefix);
            pw.print(" mSticky:");
            pw.print(stack.mSticky);
            pw.print(" mNeedKeepFocusActivity:");
            pw.print(stack.mNeedKeepFocusActivity);
            pw.println();
        }
        pw.println();
    }

    private void dumpTokenMap(FileDescriptor fd, PrintWriter pw, String prefix) {
        pw.println("TokenMap:");
        // Dump stack infos
        int size = mTokenMap.size();
        for (IBinder winToken : mTokenMap.keySet()) {
            pw.print(prefix);
            pw.print("Token   win#"); pw.println(winToken);
            pw.print(prefix);
            pw.print("        app#:"); pw.println(mTokenMap.get(winToken));
        }
        pw.println();
    }

    private void dumpStackPosInfo(FileDescriptor fd, PrintWriter pw, String prefix) {
        pw.println("StackPosInfo:");
        // Dump stack infos
        for (int i = 0; i < mStackPosInfos.length; i ++) {
            pw.print(prefix);
            pw.print("Pos #"); pw.print(i);
            pw.print(" :stack id#"); pw.println(mStackPosInfos[i].stackId);
        }
        pw.println();
    }

    TaskInfo getTaskInfoLocked(IBinder appToken) {
        ActivityInfo ac = mActivityInfoMap.get(appToken);
        if (ac == null)
            return null;
        return mTaskInfos.get(ac.mTaskId);
    }

    StackInfo getStackInfoLocked(IBinder appToken) {
        ActivityInfo ac = mActivityInfoMap.get(appToken);
        if (ac == null)
            return null;
        return getStackInfoLocked(ac.mTaskId);
    }

    StackInfo getStackInfoLocked(int taskId) {
        TaskInfo task = mTaskInfos.get(taskId);
        if (task == null)
            return null;
        return mStackInfos.get(task.mStackId);
    }


    // For TaskInfo
    public class TaskInfo {
        int mTaskId;
        int mStackId;
        boolean mPendingRemove;
        boolean mInMiniMax;
        //final ArrayList<ActivityInfo> mActivityInfos = new ArrayList<ActivityInfo>();

        public TaskInfo(int taskId, int stackId) {
            mTaskId = taskId;
            mStackId = stackId;
            mInMiniMax = false;
            mPendingRemove = false;
        }
/*
        ArrayList<ActivityInfo> getActivityInfos() {
            return mActivityInfos;
        }

        ActivityInfo getActivityInfo(IBinder appToken) {
            for (ActivityInfo ac : mActivityInfos) {
                if (ac.mAppToken == appToken) {
                    return ac;
                }
            }
            return null;
        }

        void addActivityInfo(IBinder appToken) {
            if (getActivityInfo(appToken) != null) {
                Slog.w(TAG, "addActivityInfo taskid:" + mTaskId
                        + ", appToken:" + appToken + " exist!");
                return;
            }
            ActivityInfo ac = new ActivityInfo(appToken);
            mActivityInfos.add(ac);
        }

        void removeActivityInfo(IBinder appToken) {
            for (int i = mActivityInfos.size()-1; i >= 0; --i) {
                ActivityInfo ac = mActivityInfos.get(i);
                if (ac.mAppToken == appToken) {
                    mActivityInfos.remove(ac);
                    return;
                }
            }
        }*/
    }

    // For ActivityInfo
    public class ActivityInfo {
        IBinder mAppToken;
        int mTaskId;
        IBinder mWinToken;
        boolean mRestoring;
        boolean mFloating;
        boolean mFullScreen;
        boolean mTranslucent;

        ActivityInfo(IBinder token, int taskId) {
            mAppToken = token;
            mTaskId = taskId;
        }
    }

    // For StackInfo
    public class StackInfo {
        int mStackId;
        boolean mFloating;
        boolean mSticky;
        boolean mNeedKeepFocusActivity;

        public StackInfo(int StackId, boolean isFloating) {
            mStackId = StackId;
            mFloating = isFloating;
            mSticky = false;
            mNeedKeepFocusActivity = false;
        }
    }

    class StackPos {
        int stackId;
        Rect bound;
        public StackPos() {
            stackId = -1;
            bound = null;
        }
    }
}

