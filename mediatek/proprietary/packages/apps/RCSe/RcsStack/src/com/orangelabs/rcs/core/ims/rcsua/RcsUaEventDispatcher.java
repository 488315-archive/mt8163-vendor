package com.orangelabs.rcs.core.ims.rcsua;

import android.content.Context;
import android.os.Handler;
import android.os.Message;
import android.os.Looper;

import com.orangelabs.rcs.utils.logger.Logger;
import com.orangelabs.rcs.core.ims.rcsua.RcsUaAdapter;
import com.orangelabs.rcs.core.ims.rcsua.RcsUaAdapter.RcsUaSocketIO;
import com.orangelabs.rcs.core.ims.rcsua.RcsUaAdapter.RcsUaEvent;

import java.util.ArrayList;

/**
 * The Class RcsUaEventDispatcher.
 */
public class RcsUaEventDispatcher extends Handler {

    private Context mContext;
    private ArrayList<RCSEventDispatcher> mRCSEventDispatcher = new ArrayList<RCSEventDispatcher>();
    private static final String TAG = "RcsUaEventDispatcher";
    private RcsProxySipHandler mRCSProxySiphandler;
    private RcsProxyRegistrationHandler mRCSRegistrationHandler;

    /**
     * The logger
     */
    private Logger logger = Logger.getLogger(this.getClass().getName());


    
    /**
     * Instantiates a new rcs ua event dispatcher.
     *
     * @param context the context
     */
    public RcsUaEventDispatcher(Context context) {
        mContext = context;
        createDispatcher();
    }

    /**
     * The Interface RCSEventDispatcher.
     */
    public interface RCSEventDispatcher {

        /**
         * Event callback.
         *
         * @param event the event
         */
        void EventCallback(RcsUaEvent event);

        /**
         * Enable request.
         */
        void enableRequest();

        /**
         * Disable request.
         */
        void disableRequest();
    }

    /**
     * Enable request.
     */
    void enableRequest() {
        for (RCSEventDispatcher dispatcher : mRCSEventDispatcher) {
            dispatcher.enableRequest();
        }
    }

    /**
     * Disable request.
     */
    void disableRequest() {
        for (RCSEventDispatcher dispatcher : mRCSEventDispatcher) {
            dispatcher.disableRequest();
        }
    }

    /**
     * Creates the dispatcher.
     */
    private void createDispatcher() {

        logger.debug( "Initialize the handlers ");
        //add the RCS Registartion handler
        mRCSRegistrationHandler = new RcsProxyRegistrationHandler(mContext);
        mRCSEventDispatcher.add(mRCSRegistrationHandler);
        //add the RCS proxy SIP handler
        mRCSProxySiphandler = new RcsProxySipHandler(mContext);
        mRCSEventDispatcher.add(mRCSProxySiphandler);
    }

    /**
     * HANDLE MESSAGES FROM RCS PROXY.
     *
     * @param msg the msg
     */
    @Override
    public void handleMessage(Message msg) {
        dispatchCallback((RcsUaEvent) msg.obj);
    }

    /**
     * Gets the sip event dispatcher.
     *
     * @return the sip event dispatcher
     */
    public RCSEventDispatcher getSipEventDispatcher() {
        return mRCSProxySiphandler;
    }

    /**
     * Gets the registration event handler.
     *
     * @return the registration event handler
     */
    public RCSEventDispatcher getRegistrationEventHandler() {
        return mRCSRegistrationHandler;
    }

    /**
     * Dispatch callback.
     *
     * @param event the event
     */
    void dispatchCallback(RcsUaEvent event) {
        logger.debug( "Event received : " + event.getRequestID());
        switch (event.getRequestID()) {
        case RcsUaAdapter.RCS_PROXY_EVENT_RES_REG_INFO:
            RcsUaAdapter.getInstance().handleRegistrationInfo(event);
            break;
        case RcsUaAdapter.RCS_PROXY_EVENT_RSP_RCS_REGISTER:
        case RcsUaAdapter.RCS_PROXY_EVENT_RSP_RCS_REGISTERING:
        case RcsUaAdapter.RCS_PROXY_EVENT_RSP_RCS_DEREGISTER:
        case RcsUaAdapter.RCS_PROXY_EVENT_RSP_RCS_DEREGISTERING:
            mRCSRegistrationHandler.EventCallback(event);
            break;
        case RcsUaAdapter.RCS_PROXY_EVENT_RSP_SIP_Send:
            mRCSProxySiphandler.EventCallback(event);
            break;
        }
    }

}
