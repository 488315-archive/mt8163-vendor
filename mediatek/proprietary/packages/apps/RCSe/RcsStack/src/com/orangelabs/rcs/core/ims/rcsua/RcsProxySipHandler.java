package com.orangelabs.rcs.core.ims.rcsua;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.os.AsyncResult;
import android.os.Handler;
import android.os.Message;
import android.os.SystemProperties;
import android.net.NetworkInfo;
import android.net.ConnectivityManager;
import android.util.Log;

import com.orangelabs.rcs.utils.logger.Logger;
import com.orangelabs.rcs.core.ims.rcsua.RcsUaAdapter.RcsUaEvent;
import com.orangelabs.rcs.core.ims.rcsua.RcsUaAdapter.RcsUaSocketIO;
import com.orangelabs.rcs.core.ims.protocol.sip.SipMessage;
import com.orangelabs.rcs.platform.AndroidFactory;
import com.orangelabs.rcs.platform.network.NetworkFactory;
import com.orangelabs.rcs.provider.settings.RcsSettings;

import gov2.nist.javax2.sip.stack.MessageChannel;

import java.net.*;
import java.io.OutputStream;
import java.util.HashMap;
import java.util.Map;

/**
 * The Class RcsProxySipHandler.
 */
public class RcsProxySipHandler implements
        RcsUaEventDispatcher.RCSEventDispatcher {

    /**
     * The logger
     */
    private Logger logger = Logger.getLogger(this.getClass().getName());
    
    private Logger SIPlogger = Logger.getLogger("System.out.rcs_adapter");
    private static final String TAG = "[RcsProxySipDispatcher]";
    private Context mContext;
    //private RcsUaSocketIO mSocket;
    private static RcsUaAdapter mRcsuaAdapt = null;
    //socket to send SIP data to RCS SIP stack
    private static DatagramSocket mUDPSIPSocket = null;
    private static Socket mSIPSocket = null;
    RcsSIPEventListener mSIPEvtListener = null;
    private static HashMap<String, sipTransaction> sipTransactTable = new HashMap<String, sipTransaction>();

    //sip logs
    private boolean sipTraceEnabled = RcsSettings.getInstance().isSipTraceActivated();
    private final static String TRACE_SEPARATOR = "-----------------------------------------------------------------------------";
    private final int INCOMING_MSG = 0;
    private final int OUTGOING_MSG = 1;
    /**
     * RcsSIPEventListener
     */
    public static abstract class RcsSIPEventListener {

        /**
         * Notify sip message.
         *
         * @param sipMsgResponse the sip msg response
         * @param address the address
         * @param port the port
         */
        public abstract void notifySIPMessage(byte[] sipMsgResponse,
                InetAddress address, int port);
    }

    /**
     * The Class sipTransaction.
     */
    private class sipTransaction {
        int requestId;
        int connId;
        String callId;

        /**
         * Instantiates a new sip transaction.
         *
         * @param reqId the req id
         * @param connId the conn id
         * @param callId the call id
         */
        sipTransaction(int reqId, int connId, String callId) {
            this.requestId = reqId;
            this.connId = connId;
            this.callId = callId;
        }

        /**
         * Instantiates a new sip transaction.
         *
         * @param callId the call id
         */
        sipTransaction(String callId) {
            this.requestId = 0;
            this.connId = 0;
            this.callId = callId;
        }

    };

    /**
     * Instantiates a new rcs proxy sip handler.
     *
     * @param context the context
     */
    public RcsProxySipHandler(Context context) {
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
        logger.debug("disableRequest");
        
        //clean the haspmap for request mapping
        sipTransactTable.clear();
        //log("disableRequest()");
    }

    
    /**
     * Adds the rcs sip event listener.
     *
     * @param listener the listener
     */
    public void addRCSSipEventListener(RcsSIPEventListener listener) {
        logger.debug( "addRCSSipEventListener");
        mSIPEvtListener = listener;
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
            requestId = event.getRequestID();

            switch (requestId) {
            case 1://MSG_ID_GET_SIP_RSP:
                int randLen = 0;
                randLen = event.getByte();
                byte[] sipMsgResponse = event.getBytes(randLen);
                break;

            case RcsUaAdapter.RCS_PROXY_EVENT_RSP_SIP_Send:
                logger.debug(
                        "EventCallback : event RCS_PROXY_EVENT_RES_SIP_Send");
                int dtalength = event.getDataLen();
                int messageId = event.getInt();
                int connId = event.getInt();
                int sipMessageLength = event.getInt();
                String sipMsgResponse2 = "";
                sipMsgResponse2 = event.getString(sipMessageLength);
                String callID = "";
                callID = getCallIDFromSIPMessage(sipMsgResponse2);
                logger.debug( "INCOMING SIP MESSAGE, ID : " + messageId
                        + " ; CONN_ID : " + connId + "; Call_ID: " + callID
                        + " ;SIP_MESSAGE_LENGTH : " + sipMessageLength);
                
                printSIPLog(sipMsgResponse2.toString(),INCOMING_MSG);
                
                //create new SIP transaction
                sipTransaction sipTransact = new sipTransaction(messageId,
                        connId, callID);
                
                //update SIP transaction map
                updateSIPTranscationTable(sipTransact);
                
                String ipaddrss = mRcsuaAdapt.getHostAddress();
                InetAddress inetAddress = InetAddress.getByName(ipaddrss);
                int sipPort = mRcsuaAdapt.getRCSSipStackPort();
                
                
                byte[] SIPdata = null;
                SIPdata = sipMsgResponse2.getBytes();
                int sipmsglength = SIPdata.length;
                
                if (mSIPEvtListener != null) {
                    logger.debug( "mSIPEvtListener.notifySIPMessage");
                    mSIPEvtListener.notifySIPMessage(
                            sipMsgResponse2.getBytes(), inetAddress, sipPort);
                }else{
                     logger.error("ERROR : SIPEvtListener for SIP stack is null");
                }
                
                break;
            default:
                break;
            }
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    /**
     * Send sip msg.
     *
     * @param sipMsgBuffer the sip msg buffer
     * @return true, if successful
     */
    public boolean sendSipMsg(byte[] sipMsgBuffer) {

        boolean temfalg = true;
        logger.debug( "sendSipMsg");
        String sipMessage = new String(sipMsgBuffer);
        
        printSIPLog(sipMessage.toString(),OUTGOING_MSG);
        
        //append string terminator for rcs_ua
        sipMessage += "\0";
        
        //get the callid for the sip message 
        String callID = getCallIDFromSIPMessage(sipMessage);
        sipTransaction sipTxn = getTransactionDetails(callID);
        logger.debug( "sendSipMsg ; call_id: " + callID + " ; request_id : "
                + sipTxn.requestId + " conn_id :  " + sipTxn.connId);
        
        RcsUaEvent event = new RcsUaAdapter.RcsUaEvent(
                RcsUaAdapter.RCS_PROXY_EVENT_REQ_SIP_Send);
        
        //sip msg buffer
        event.putInt(sipTxn.requestId);
        event.putInt(sipTxn.connId);
        event.putInt(sipMessage.length());
        event.putString(sipMessage, sipMessage.length());
        //write event
        mRcsuaAdapt.writeEvent(event);
        return true;
    }

    /**
     * The Class SipDispatchThread.
     */
    class SipDispatchThread extends Thread {

        private byte[] mSipMessage;
        private InetAddress mInetAddress;
        private int mPort;

        /**
         * Instantiates a new sip dispatch thread.
         *
         * @param sipMessage the sip message
         * @param inetAddress the inet address
         * @param port the port
         */
        public SipDispatchThread(byte[] sipMessage, InetAddress inetAddress,
                int port) {
            mSipMessage = sipMessage;
            mInetAddress = inetAddress;
            mPort = port;
        }

        /**
         * Run.
         */
        @Override
        public void run() {
            mSIPEvtListener.notifySIPMessage(mSipMessage, mInetAddress, mPort);
        }
    }

    String CALL_ID_HEADER = "Call-ID";

    /**
     * Gets the call id from sip message.
     *
     * @param sipMessage the sip message
     * @return the call id from sip message
     */
    String getCallIDFromSIPMessage(String sipMessage) {

        logger.debug( "getCallIDFromSIPMessage");
        String callID = "";
        String[] sipHeader = sipMessage.split(System
                .getProperty("line.separator"));
        for (int i = 0; i < sipHeader.length; i++) {
            String header = sipHeader[i];
            if (header.contains(CALL_ID_HEADER)) {
                //.substring(0, s.indexOf(")"));
                header = header.substring(header.indexOf(":") + 1);
                callID = header.substring(0, header.indexOf("@"));
                callID = callID.trim();
                logger.debug( "CALl-ID found : " + callID);
                break;
            }

        }

        return callID;
    }

    /**
     * THIS FUNCTION UPDATES THE REQUEST ID AND CONNECT ID AND CALL-ID FOR 
     * INCOMING SIP MESSAGES BASED ON CALL ID.
     * WHEN RESPONSE FOR THAT REQUEST ID IS SENT, THE ENTRY IS DELETED.
     * THE KEY IS MAINTED FOR ONE REQUEST-RESPONSE TRANSACTION.
     *
     * @param sipTxn the sip txn
     */
    void updateSIPTranscationTable(sipTransaction sipTxn) {

        String callId = "" + sipTxn.callId;
        if (sipTransactTable.containsKey(callId)) {
            //update the conn id
            sipTransactTable.put(callId, sipTxn);
        } else {
            sipTransactTable.put(callId, sipTxn);
        }
    }

    /**
     * get transaction details from map.
     *
     * @param callId the call id
     * @return the transaction details
     */
    sipTransaction getTransactionDetails(String callId) {
        sipTransaction sipTxtn = null;
        if (sipTransactTable.containsKey(callId)) {
            sipTxtn = sipTransactTable.get(callId);
        } else {
            sipTxtn = new sipTransaction(callId);
            sipTransactTable.put(callId, sipTxtn);
        }
        return sipTxtn;
    }
    
    /*
     * print sip log by rcs_adapter
     */
    private void printSIPLog(String log , int mode)
    {
        if(sipTraceEnabled)
        {
            if(mode == INCOMING_MSG)
                log = "[<<<<]" + log;
            else if(mode == OUTGOING_MSG)
                log = "[>>>>]" + log;
            SIPlogger.debug(log);
            SIPlogger.debug(TRACE_SEPARATOR);
            
        }
    }
}
