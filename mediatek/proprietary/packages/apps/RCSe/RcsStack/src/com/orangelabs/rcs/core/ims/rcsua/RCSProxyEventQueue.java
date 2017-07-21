package com.orangelabs.rcs.core.ims.rcsua;

import android.content.Context;

import com.orangelabs.rcs.utils.logger.Logger;
import com.orangelabs.rcs.core.ims.rcsua.RcsUaAdapter.RcsUaEvent;

import java.util.Iterator;
import java.util.LinkedList;
import java.util.Queue;

/**
 * The Class RCSProxyEventQueue, for managing incoming(send to dispatcher) or
 * outgoing(write to socket) messages.
 */
public class RCSProxyEventQueue implements Runnable {

    
     /**
     * The logger
     */
    private Logger logger = Logger.getLogger(this.getClass().getName());
    
    public static final int INCOMING_EVENT_MSG = 0;
    public static final int OUTGOING_EVENT_MSG = 1;
    private static final String TAG = "RCSProxyEventQueue";
    private Object mObj = new Object();
    private Queue<RCSQueueElement> mEventQueue = null;
    Iterator mEventQueueIterator = null;
    private static RCSProxyEventQueue mInstance = null;
    private static RcsUaAdapter mRCSUAAdapter = null;
    
    /**
     * IMS polling thread Id
    */
    private long mEventPollingThreadID = -1;
    /**
      * IMS polling thread
    */
    private Thread mEventPollingThread = null;

    /**
     * The Class RCSQueueElement.
     */
    private class RCSQueueElement {
        RcsUaEvent rcsEvent;
        int direction;

        /**
         * Instantiates a new RCS queue element.
         *
         * @param rcsEvent the rcs event
         * @param dir the dir
         */
        public RCSQueueElement(RcsUaEvent rcsEvent, int dir) {
            this.rcsEvent = rcsEvent;
            this.direction = dir;
        }
    };
    
    /**
     * Instantiates a new RCS proxy event queue.
     *
     * @param rcsUAAdapter the rcs ua adapter
     */
    public RCSProxyEventQueue(RcsUaAdapter rcsUAAdapter) {
        mRCSUAAdapter = rcsUAAdapter;
        mEventQueue = new LinkedList<RCSQueueElement>();
        mEventQueueIterator = mEventQueue.iterator();
    }

    

    /**
     * Start event queue polling.
     */
    protected void startEventQueuePolling() {

        if (mEventPollingThreadID >= 0) {
            return;
        }
        try {

            logger.debug("startEventQueuePolling");
            mEventPollingThread = new Thread(this);
            mEventPollingThreadID = mEventPollingThread.getId();
            mEventPollingThread.start();
        } catch (Exception e) {

        }
    }

    /**
     * Stop event queue polling.
     */
    protected void stopEventQueuePolling() {
        if (mEventPollingThreadID == -1) {
            return;
        }
        try {
            logger.debug("stopEventQueuePolling");
            mEventPollingThreadID = -1;
            mEventPollingThread.interrupt();
            mEventPollingThread = null;
            mEventQueue.clear();
        } catch (Exception e) {

        }
    }

    /**
     * Adds the event.
     *
     * @param direction the direction
     * @param event the event
     */
    public void addEvent(int direction, RcsUaEvent event) {
        RCSQueueElement element = new RCSQueueElement(event, direction);
        synchronized (mObj) {
            mEventQueue.add(element);
        }
    }

    /**
     * Run.
     */
    public void run() {
        while (true) {
            if (mEventQueue.size() == 0) {
                continue;
            }
            RCSQueueElement event = null;
            synchronized (mObj) {
                event = mEventQueue.poll();
            }

            if (event != null) {
                //from RCS ua proxy to adapter
                if (event.direction == INCOMING_EVENT_MSG) {
                    logger.debug("event to dispatcher");
                    mRCSUAAdapter.sendMsgToDispatcher(event.rcsEvent);
                    
                }
                //from adapter to RCS_uaproxy
                else if (event.direction == OUTGOING_EVENT_MSG) {
                    // send the event to va
                    logger.debug("event to RCS_UA_Proxy");
                    mRCSUAAdapter.sendMsgToRCSUAProxy(event.rcsEvent);
                }
            }
        }
    }

}
