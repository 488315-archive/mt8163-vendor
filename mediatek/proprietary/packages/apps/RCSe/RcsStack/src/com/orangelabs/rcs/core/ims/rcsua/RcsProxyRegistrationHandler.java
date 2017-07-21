package com.orangelabs.rcs.core.ims.rcsua;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.os.AsyncResult;
import android.os.Handler;
import android.os.Message;
import android.os.SystemProperties;
import android.util.Log;

import com.orangelabs.rcs.utils.logger.Logger;
import com.orangelabs.rcs.core.ims.rcsua.RcsUaAdapter.RcsUaEvent;
import com.orangelabs.rcs.core.ims.rcsua.RcsUaAdapter.RcsUaSocketIO;

import org.xbill.DNS.MRRecord;

/**
 * The Class RcsProxyRegistrationHandler.
 */
public class RcsProxyRegistrationHandler implements
        RcsUaEventDispatcher.RCSEventDispatcher {

    /**
     * The logger
     */
    private Logger logger = Logger.getLogger(this.getClass().getName());
    
    private static final int REGISTRATION_TIMER = 20000;
    private static final String TAG = "RcsProxyRegistrationHandler";
    private Context mContext;
    private static RcsUaAdapter mRcsuaAdapt = null;
    private static boolean mIsRegistrationSuccessful = false;
    private static boolean mIsRegistrationRequestSent = false;
    private static boolean mIsDeRegistrationRequestSent = false;
    /**
     * Wait user answer for session invitation
     */
    private static Object mWaitRegisterResponse = new Object();

    /**
     * Wait user answer for session invitation
     */
    private static Object mWaitDeRegisterResponse = new Object();

    /**
     * Instantiates a new rcs proxy registration handler.
     *
     * @param context the context
     */
    public RcsProxyRegistrationHandler(Context context) {
        mContext = context;
        if (mRcsuaAdapt == null) {
            mRcsuaAdapt = RcsUaAdapter.getInstance();
        }
    }

    /**
     * Enable request.
     */
    public void enableRequest() {
        //log("enableRequest()");
    }

    /**
     * Disable request.
     */
    public void disableRequest() {
        //log("disableRequest()");
    }

    /**
     * Event callback.
     *
     * @param event the event
     */
    public void EventCallback(RcsUaEvent event) {

        try {
            int requestId;
            int len;
            String data;
            int transactionId;
            int type;
            String state ;
            requestId = event.getRequestID();
            switch (requestId) {
            case RcsUaAdapter.RCS_PROXY_EVENT_RSP_RCS_REGISTERING:
                logger.debug(
                         "EventCallback : event RCS_PROXY_EVENT_RSP_RCS_REGISTERING");
                 
                 //GET THE STATE
                 state = event.getString(event.getDataLen());
                 
                logger.debug("registering state : " +state);
                 
                 //registering successfully
                 if(state.equals("1")){
                     //do nothing
                    logger.debug("RCS_UA registering currently. wait for REG Success update");
                 }else{
                     //do nothing
                    logger.debug("RCS_UA couldnt add capab successfully. stop registering process");
                     
                     synchronized (mWaitRegisterResponse) {

                         //set the value based on registration state 
                         mIsRegistrationSuccessful = false;
                        logger.debug( "setRegistrationStatus : " + mIsRegistrationSuccessful);
                         //set the registration status 
                         mRcsuaAdapt.setRegistrationStatus(mIsRegistrationSuccessful);
                         //resove the semaphore..
                         mWaitRegisterResponse.notify();
                     }
                     
                 }
                 
                 
                break;
            case RcsUaAdapter.RCS_PROXY_EVENT_RSP_RCS_REGISTER:

               logger.debug(
                        "EventCallback : event RCS_PROXY_EVENT_RSP_RCS_REGISTER");
              
                //GET THE STATE
                state = event.getString(event.getDataLen());
               logger.debug("register state : " +state);
                
                if(state.equals("1")){
                        //do nothing
                       logger.debug("RCS_UA REGISTERED SUCCESS.");
                        //set the value based on registration state 
                     mIsRegistrationSuccessful = true;
                   }
                else{
                     //do nothing
                      logger.debug("RCS_UA REGISTERED FAILURE."); 
                      //set the value based on registration state 
                     mIsRegistrationSuccessful = false;
                }
                
                //NOTIFY LISTENING MODULE
                
                //read the content and based on that set the value                     
                synchronized (mWaitRegisterResponse) {

                    //set the value based on registration state 
                    mIsRegistrationSuccessful = true;
                    logger.debug("setRegistrationStatus : "
                            + mIsRegistrationSuccessful);
                    //set the registration status 
                    mRcsuaAdapt.setRegistrationStatus(mIsRegistrationSuccessful);
                    //resove the semaphore..
                    mWaitRegisterResponse.notify();
                }

                break;

            case RcsUaAdapter.RCS_PROXY_EVENT_RSP_RCS_DEREGISTER:
                logger.debug( "Deregistered successfully");
                //set the value based on registration state 
               

                //read the content and based on that set the value                     
                synchronized (mWaitDeRegisterResponse) {

                    //set the value based on registration state 
                mIsRegistrationSuccessful = false;
                     logger.debug("setRegistrationStatus : "
                            + mIsRegistrationSuccessful);
                    //set the registration status 
                    mRcsuaAdapt.setRegistrationStatus(mIsRegistrationSuccessful);
                    //resove the semaphore..
                    mWaitDeRegisterResponse.notify();
                }

                //set the registration status 
                mRcsuaAdapt.setRegistrationStatus(mIsRegistrationSuccessful);
                break;

            case 2://MSG_ID_REQUEST_QUERY_SIM_STATUS:
                RcsUaEvent responseEvent = new RcsUaAdapter.RcsUaEvent(1);
                transactionId = event.getByte();
                //transaction_id
                responseEvent.putByte(transactionId);
                //Session ID
                responseEvent.putByte(0);
                //Pad
                responseEvent.putByte(0);
                // send the event to va
                mRcsuaAdapt.writeEvent(responseEvent);
                break;
            default:
                break;
            }
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    /**
     * Register.
     *
     * @return true, if successful
     */
    public boolean register() {
        boolean regStatus = false;
        try {
            synchronized (mWaitRegisterResponse) {
                mIsRegistrationRequestSent = true;
                logger.debug( "register : send request");
               
                //send register request
                sendRegisterRequest();
                
                //wait for 10 sec
                mWaitRegisterResponse.wait(REGISTRATION_TIMER);
                logger.debug("register : after register request response ; registration status : "+ mIsRegistrationSuccessful);
                regStatus = mIsRegistrationSuccessful;
            }
        } catch (InterruptedException e) {
            regStatus = false;
        }
        return regStatus;
    }

    /**
     * Unregister.
     *
     * @return true, if successful
     */
    public boolean unregister() {
        boolean status = false;
        try {
            synchronized (mWaitDeRegisterResponse) {
                mIsDeRegistrationRequestSent = true;
                logger.debug( "deregister : send request");
                //send register request
                sendDeRegisterRequest();
                //wait for 3 sec
                mWaitDeRegisterResponse.wait(REGISTRATION_TIMER);
                logger.debug( "after deregister request response ; deregistration status : "
                                + mIsDeRegistrationRequestSent);
                status = mIsRegistrationSuccessful;
            }
        } catch (InterruptedException e) {
            status = false;
        }
        return status;
    }

    /**
     * Send register request.
     */
    private void sendRegisterRequest() {
        logger.debug( "sendRegisterRequest");
        String rcsCapabilityFeatureTags = RcsUaAdapter.getInstance()
                .getRCSFeatureTag();
        rcsCapabilityFeatureTags+="\0";
        int length = rcsCapabilityFeatureTags.length();
        RcsUaEvent event = new RcsUaAdapter.RcsUaEvent(
                mRcsuaAdapt.RCS_PROXY_EVENT_RCS_REGISTER);
        event.putString(rcsCapabilityFeatureTags, length);
        mRcsuaAdapt.writeEvent(event);
    }

    /**
     * Send de-register request.
     */
    private void sendDeRegisterRequest() {
        logger.debug( "sendDeRegisterRequest");
        String rcsCapabilityFeatureTags = RcsUaAdapter.getInstance()
                .getRCSFeatureTag();
        
        rcsCapabilityFeatureTags+="\0";
        
        int length = rcsCapabilityFeatureTags.length();
        RcsUaEvent event = new RcsUaAdapter.RcsUaEvent(
                mRcsuaAdapt.RCS_PROXY_EVENT_RCS_DEREGISTER);
        //put capability
        event.putString(rcsCapabilityFeatureTags, length);
        mRcsuaAdapt.writeEvent(event);

    }
}
