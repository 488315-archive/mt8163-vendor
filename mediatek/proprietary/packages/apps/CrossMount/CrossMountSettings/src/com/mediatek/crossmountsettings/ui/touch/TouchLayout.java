package com.mediatek.crossmountsettings.ui.touch;

import android.content.Context;
import android.util.AttributeSet;
import android.util.Log;
import android.view.InputDevice;
import android.view.KeyEvent;
import android.view.MotionEvent;
import android.view.View;
import android.widget.LinearLayout;
import android.widget.TextView;

import com.mediatek.crossmount.adapter.CrossMountAdapter;

/**
 * Touch layout to response the motion event.
 */
public class TouchLayout extends LinearLayout {
    private static final String TAG = "TouchLayout";

    private CrossMountAdapter mCrossAdapter;
    private View mView;
    private TextView mTextView;

    private static final int GENERIC_INPUT_TYPE_ID_TOUCHPAD_RECT = 0;
    private static final int GENERIC_INPUT_TYPE_ID_TOUCH_DOWN = 1;
    private static final int GENERIC_INPUT_TYPE_ID_TOUCH_UP = 2;
    private static final int GENERIC_INPUT_TYPE_ID_TOUCH_MOVE = 3;
    private static final int GENERIC_INPUT_TYPE_ID_KEY_DOWN = 4;
    private static final int GENERIC_INPUT_TYPE_ID_KEY_UP = 5;
    private static final int GENERIC_INPUT_TYPE_ID_ZOOM = 6;

    private boolean mRectEvents = false;

    /**
     * Constructor.
     * @param context
     */
    public TouchLayout(Context context) {
        super(context);
    }

    /**
     *  Constructor.
     * @param context Context
     * @param attrs AttributeSet
     */
    public TouchLayout(Context context, AttributeSet attrs) {
        this(context, attrs, 0);
    }

    /**
     * Constructor.
     * @param context Context
     * @param attrs AttributeSet
     * @param defStyle int
     */
    public TouchLayout(Context context, AttributeSet attrs, int defStyle) {
        super(context, attrs, defStyle);
    }

    @Override
    public boolean onTouchEvent(MotionEvent ev) {
        // firstly send View's Width and height once
        if (!mRectEvents) {
            int width = mView.getWidth();
            int height = mView.getHeight();
            sendTouchRect(width, height);
            mRectEvents = true;
        }
        // set Text GONE once touch
        if (mTextView != null && mTextView.getVisibility() == View.VISIBLE) {
            Log.d(TAG, "set text GONE");
            mTextView.setVisibility(View.GONE);
        }

        final int action = ev.getAction();
        int type = -1;
        Log.d(TAG, "onTouchEvent action=" + action);
        switch (action & MotionEvent.ACTION_MASK) {
        case MotionEvent.ACTION_DOWN:
            type = GENERIC_INPUT_TYPE_ID_TOUCH_DOWN;
            break;
        case MotionEvent.ACTION_UP:
            type = GENERIC_INPUT_TYPE_ID_TOUCH_UP;
            break;
        case MotionEvent.ACTION_MOVE:
            type = GENERIC_INPUT_TYPE_ID_TOUCH_MOVE;
            break;
        default:
            break;
        }
        sendEventMsg(type, ev);
        return true;
    }


    /**
     * Set the adapter.
     * @param adapter CrossMountAdapter
     */
    public void setAdapter(CrossMountAdapter adapter) {
        mCrossAdapter = adapter;
    }

    /**
     * Set the widget.
     * @param view the Control Zoom
     * @param textView the hint message
     */
    public void setView(View view, TextView textView) {
        mView = view;
        mTextView = textView;
    }

    /**
     * Send the touch zoom's width and height.
     * @param width the view's width
     * @param height the view's height
     */
    private void sendTouchRect(int width, int height) {
        StringBuilder eventDesc = new StringBuilder();
        eventDesc.append(String.valueOf(GENERIC_INPUT_TYPE_ID_TOUCHPAD_RECT))
                .append(",");
        eventDesc.append(String.valueOf(width)).append(",");
        eventDesc.append(String.valueOf(height));
        String msg = eventDesc.toString();
        Log.d(TAG, "sendTouchRect " + msg);
        mCrossAdapter.sendControlMessage(msg);
    }

    /**
     * Send the touch event message.
     * @param eventType int
     * @param ev MotionEvent
     */
    private void sendEventMsg(int eventType, MotionEvent ev) {
        StringBuilder eventDesc = new StringBuilder();
        eventDesc.append(String.valueOf(eventType)).append(",");
        eventDesc.append(getTouchEventDesc(ev));
        String msg = eventDesc.toString();
        Log.d(TAG, "send msg = " + msg);
        mCrossAdapter.sendControlMessage(msg);
    }

    /**
     * get the motion event description string.
     * @param ev MotionEvent
     * @return the touch event string
     */
    private String getTouchEventDesc(MotionEvent ev) {
        final int pointerCount = ev.getPointerCount();
        String ret;
        StringBuilder eventDesc = new StringBuilder();
		
        eventDesc.append(String.valueOf(System.currentTimeMillis())).append(",");
        eventDesc.append(String.valueOf(pointerCount)).append(",");
        for (int p = 0; p < pointerCount; p++) {
            eventDesc.append(String.valueOf(ev.getPointerId(p)))
            .append(",")
            .append(String.valueOf((int) (ev.getXPrecision() * ev.getX(p))))
            .append(",")
            .append(String.valueOf((int) (ev.getYPrecision() * ev.getY(p))))
            .append(",");
        }
        ret = eventDesc.toString();
        return ret.substring(0, ret.length() - 1);
    }
}
