package com.mediatek.common.multiwindow;

import com.mediatek.common.multiwindow.IMWAmsCallback;
import com.mediatek.common.multiwindow.IMWWmsCallback;
import com.mediatek.common.multiwindow.IMWSystemUiCallback;

import android.os.IBinder;
import android.os.WorkSource;
import android.content.ComponentName;
import android.app.PendingIntent;

/**
 * @hide
 */
interface IMultiWindowManager {
    void moveActivityTaskToFront(in IBinder token);
    void closeWindow(in IBinder token);
    void restoreWindow(in IBinder token, in boolean toMax);
    void setAMSCallback(IMWAmsCallback cb);
    void setSystemUiCallback(IMWSystemUiCallback cb);
    void stickWindow(in IBinder token, in boolean isSticky);
    boolean isFloatingStack(in int stackId);

    // for Window Manager Service
    void setWMSCallback(IMWWmsCallback cb);
    boolean isStickStack(in int stackId);
    boolean isInMiniMax(in int taskId);

    /// called  by window manager policy
    void moveFloatingWindow(in int disX, in int disY);
    void resizeFloatingWindow(in int direction, in int deltaX, in int deltaY);
    void enableFocusedFrame(in boolean enable);

    /// called by ActivityStackSupervisor
    void miniMaxTask(in int taskId);

    // for showing restore button on systemUI module
    void showRestoreButton(in boolean flag);

    boolean isSticky(in IBinder token);
    void activityCreated(in IBinder token);

    void onTaskAdded(in int taskId, in int stackId);
    void onTaskRemoved(in int taskId, in int stackId);

    void addToken(in IBinder winToken, in IBinder appToken);
    void removeToken(in IBinder winToken);

    void onAppTokenAdded(in IBinder appToken, in int taskId);
    void onAppTokenRemoved(in IBinder appToken, in int taskId);

    void addStack(int stackId, boolean isFloating);
    void removeStack(int stackId, boolean isFloating);

    boolean isFloatingByAppToken(in IBinder appToken);
    boolean isFloatingByWinToken(in IBinder winToken);

    // Add for split-screen mode
    int getStackPosition(int stackId);
    int computeStackPosition(int stackId);
    void resetStackPosition(int stackId);
    boolean isSplitMode();
    void switchMode(boolean isSplit);
    void resizeAndMoveStack(int stackId);
    int getNextFloatStack(int curStackId);
}