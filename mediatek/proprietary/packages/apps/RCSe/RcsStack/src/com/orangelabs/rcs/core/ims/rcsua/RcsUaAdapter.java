package com.orangelabs.rcs.core.ims.rcsua;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.net.ConnectivityManager;
import android.net.LocalSocket;
import android.net.LocalSocketAddress;
import android.net.NetworkInfo;
import android.os.Looper;
import android.os.Messenger;
import android.os.RemoteException;
import android.os.SystemProperties;
import android.os.Handler;
import android.os.Message;

import com.android.ims.ImsManager;
import com.orangelabs.rcs.core.ims.network.sip.FeatureTags;
import com.orangelabs.rcs.core.ims.protocol.sip.SipException;
import com.orangelabs.rcs.platform.AndroidFactory;
import com.orangelabs.rcs.platform.network.NetworkFactory;
import com.orangelabs.rcs.provider.settings.RcsSettings;
import com.orangelabs.rcs.provider.settings.RcsSettingsData;
import com.orangelabs.rcs.platform.AndroidFactory;
import com.orangelabs.rcs.platform.network.NetworkFactory;

import java.lang.Thread.State;
import java.util.List;
import java.util.Scanner;
import java.io.InterruptedIOException;
import java.io.IOException;
import java.io.OutputStream;
import java.io.DataInputStream;
import java.io.BufferedOutputStream;
import javax2.sip.ListeningPoint;
import android.telephony.ServiceState;
import com.android.ims.ImsConfig;
import com.orangelabs.rcs.utils.logger.Logger;

/**
 * The Class RcsUaAdapter.
 */
public class RcsUaAdapter {

    private static int MSG_ID_FOR_RCS_PROXY_TEST = 1;
    private static int MSG_ID_FOR_RCS_PROXY_TEST_RSP = 2;

    final static public int RCS_PROXY_EVENT_TO_RCSPROXY_START_CODE = 3;
    final static public int RCS_PROXY_EVENT_REQ_REG_INFO = RCS_PROXY_EVENT_TO_RCSPROXY_START_CODE + 1; // registartion ino required
    final static public int RCS_PROXY_EVENT_RES_REG_INFO = RCS_PROXY_EVENT_TO_RCSPROXY_START_CODE + 2; //reg info response
    final static public int RCS_PROXY_EVENT_RCS_REGISTER = RCS_PROXY_EVENT_TO_RCSPROXY_START_CODE + 3; //reg info response
    final static public int RCS_PROXY_EVENT_RCS_DEREGISTER = RCS_PROXY_EVENT_TO_RCSPROXY_START_CODE + 4; //reg info response
    final static public int RCS_PROXY_EVENT_REQ_SIP_Send = RCS_PROXY_EVENT_TO_RCSPROXY_START_CODE + 5; //sip request sent
    final static public int RCS_PROXY_EVENT_RSP_SIP_Send = RCS_PROXY_EVENT_TO_RCSPROXY_START_CODE + 6; //SIp request response
    final static public int RCS_PROXY_EVENT_RSP_RCS_REGISTER = RCS_PROXY_EVENT_TO_RCSPROXY_START_CODE + 7; //reg info response
    final static public int RCS_PROXY_EVENT_RSP_RCS_DEREGISTER = RCS_PROXY_EVENT_TO_RCSPROXY_START_CODE + 8;
    final static public int RCS_PROXY_EVENT_RSP_RCS_REGISTERING = RCS_PROXY_EVENT_TO_RCSPROXY_START_CODE + 9; //reg info response
    final static public int RCS_PROXY_EVENT_RSP_RCS_DEREGISTERING = RCS_PROXY_EVENT_TO_RCSPROXY_START_CODE + 10;
    

    //IMS specific information
    private static String IMSProxyAddr = "";
    private static int IMSProxyPort = 0;
    private static String SIPDefaultProtocolForVoLTE = "UDP";
    private static String mLocalIPAddr = "";
    private static String mPrivateID = "";
    private static String mUserAgent = "";

    //RCS SIP stack related details 
    private static int RCSSIPStackPort = 0;

    //intent 
    public static final String VOLTE_SERVICE_NOTIFY_INTENT = "COM.MEDIATEK.RCS.IMS.VOLTE_SERVICE_NOTIFICATION";

    
    private Object mListenerLock = new Object();

    /**
     * The logger
     */
    private  Logger logger = Logger.getLogger(this.getClass().getName());
    
    
    //enums for registration state 
    
    public enum IMSRegState {
        REGISTERED(1),
        UNREGISTERING(2),
        UNREGISTERED(3),
        REGISTERING(4),
        DISCONNECTED(5),
        AUTHENTICATING(6);
        
        private int state;

        private IMSRegState(int state) {
            this.state = state;
        }

        public int getState() {
            return this.state;
        }

        @Override
        public String toString() {
            return "" + this.state;
        }

        public boolean equals(String state) {
            return this.toString().equals(state);
        }
    }
    
    //RCS UA event
    /**
     * The Class RcsUaEvent.
     */
    public static class RcsUaEvent {
        public static final int MAX_DATA_LENGTH = 70960;
        private int requestId;
        private int dataLen;
        private int readOffset;
        private byte data[];
        private int eventmaxdataLen = MAX_DATA_LENGTH;

        /**
         * Instantiates a new rcs ua event.
         *
         * @param rid the request id
         */
        public RcsUaEvent(int rid) {
            requestId = rid;
            data = new byte[eventmaxdataLen];
            dataLen = 0;
            readOffset = 0;
        }

        /**
         * Instantiates a new rcs ua event.
         *
         * @param rid the rid
         * @param length the length
         */
        public RcsUaEvent(int rid, int length) {
            requestId = rid;
            eventmaxdataLen = length;
            data = new byte[eventmaxdataLen];
            dataLen = 0;
            readOffset = 0;
        }

        /**
         * Put int.
         *
         * @param value the value
         * @return the int
         */
        public int putInt(int value) {
            if (dataLen > eventmaxdataLen - 4) {
                return -1;
            }

            synchronized (this) {
                for (int i = 0; i < 4; ++i) {
                    data[dataLen] = (byte) ((value >> (8 * i)) & 0xFF);
                    dataLen++;
                }
            }
            return 0;
        }

        /**
         * Put short.
         *
         * @param value the value
         * @return the int
         */
        public int putShort(int value) {
            if (dataLen > eventmaxdataLen - 2) {
                return -1;
            }

            synchronized (this) {
                for (int i = 0; i < 2; ++i) {
                    data[dataLen] = (byte) ((value >> (8 * i)) & 0xFF);
                    dataLen++;
                }
            }

            return 0;
        }

        /**
         * Put byte.
         *
         * @param value the value
         * @return the int
         */
        public int putByte(int value) {
            if (dataLen > eventmaxdataLen - 1) {
                return -1;
            }

            synchronized (this) {
                data[dataLen] = (byte) (value & 0xFF);
                dataLen++;
            }

            return 0;
        }

        /**
         * Put string.
         *
         * @param str the str
         * @param len the len
         * @return the int
         */
        public int putString(String str, int len) {

            
            if (dataLen > eventmaxdataLen - len) {
                return -1;
            }

            synchronized (this) {
                byte s[] = str.getBytes();
                if (len < str.length()) {
                    System.arraycopy(s, 0, data, dataLen, len);
                    dataLen += len;
                } else {
                    int remain = len - str.length();
                    System.arraycopy(s, 0, data, dataLen, s.length);
                    dataLen += s.length;
                    /*
                    for (int i=0 ; i<remain ; i++) {
                        data[data_len] = 0;
                        data_len++;
                    }*/
                }
            }

            return 0;
        }

        /**
         * Put bytes.
         *
         * @param value the value
         * @return the int
         */
        public int putBytes(byte[] value) {
            int len = value.length;

            if (len > eventmaxdataLen) {
                return -1;
            }

            synchronized (this) {
                System.arraycopy(value, 0, data, dataLen, len);
                dataLen += len;
            }

            return 0;
        }

        /**
         * Gets the data.
         *
         * @return the data
         */
        public byte[] getData() {
            return data;
        }

        /**
         * Gets the data len.
         *
         * @return the data len
         */
        public int getDataLen() {
            return dataLen;
        }

        /**
         * Gets the request id.
         *
         * @return the request id
         */
        public int getRequestID() {
            return requestId;
        }

        /**
         * Gets the int.
         *
         * @return the int
         */
        public int getInt() {
            int ret = 0;
            synchronized (this) {
                ret = ((data[readOffset + 3] & 0xff) << 24
                        | (data[readOffset + 2] & 0xff) << 16
                        | (data[readOffset + 1] & 0xff) << 8 | (data[readOffset] & 0xff));
                readOffset += 4;
            }
            return ret;
        }

        /**
         * Gets the short.
         *
         * @return the short
         */
        public int getShort() {
            int ret = 0;
            synchronized (this) {
                ret = ((data[readOffset + 1] & 0xff) << 8 | (data[readOffset] & 0xff));
                readOffset += 2;
            }
            return ret;
        }

        // Notice: getByte is to get int8 type from VA, not get one byte.
        /**
         * Gets the byte.
         *
         * @return the byte
         */
        public int getByte() {
            int ret = 0;
            synchronized (this) {
                ret = (data[readOffset] & 0xff);
                readOffset += 1;
            }
            return ret;
        }

        /**
         * Gets the bytes.
         *
         * @param length the length
         * @return the bytes
         */
        public byte[] getBytes(int length) {
            if (length > dataLen - readOffset) {
                return null;
            }

            byte[] ret = new byte[length];

            synchronized (this) {
                for (int i = 0; i < length; i++) {
                    ret[i] = data[readOffset];
                    readOffset++;
                }
                return ret;
            }
        }

        /**
         * Gets the string.
         *
         * @param len the len
         * @return the string
         */
        public String getString(int len) {
            byte buf[] = new byte[len];

            synchronized (this) {
                System.arraycopy(data, readOffset, buf, 0, len);
                readOffset += len;
            }

            return (new String(buf)).trim();
        }
    }

    /**
     * The Class RcsUaSocketIO.
     */
    public class RcsUaSocketIO extends Thread {
        private byte buf[];

        private int mTyp = -1;
        private int mId = -1;
        private String mSocketName = null;
        private LocalSocket mSocket = null;
        private OutputStream mOut = null;
        private DataInputStream mDin = null;

        /**
         * Instantiates a new rcs ua socket io.
         *
         * @param socket_name the socket_name
         */
        public RcsUaSocketIO(String socket_name) {
            mSocketName = socket_name;
            // TODO: buffer size confirm
            buf = new byte[8];
            //Xlog.d(TAG, "VaSocketIO(): Enter");
        }

        /**
         * Run.
         */
        public void run() {

            logger.debug("RCS socket reading thread started");
            while (true) {
                if (mIsRCSUAAdapterEnabled) {
                    try {
                        if (mDin != null) {
                            // read the Event from mIO
                            RcsUaEvent event = readEvent();

                            // TODO: need to confirm if event is null or not
                            if (event != null) {

                                //put event in queue
                                mEventQueue.addEvent(
                                        RCSProxyEventQueue.INCOMING_EVENT_MSG,
                                        event);
                            }
                        }
                    } catch (InterruptedIOException e) {
                        disconnectSocket();
                        e.printStackTrace();
                    } catch (Exception e) {
                        disconnectSocket();
                        e.printStackTrace();
                    }
                }
            }
        }

        /**
         * Connect socket.
         *
         * @return true, if successful
         */
        public boolean connectSocket() {
            logger.debug( "connectSocket");
            if (mSocket != null) {
                mSocket = null; // reset to null, create the new one
            }
            try {
                mSocket = new LocalSocket();
                logger.debug("RcsUaSocketIO : try to conect with socket - "+
                        mSocketName);
                LocalSocketAddress addr = new LocalSocketAddress(mSocketName,
                        LocalSocketAddress.Namespace.RESERVED);
                mSocket.connect(addr);
                mOut = new BufferedOutputStream(mSocket.getOutputStream(), 4096);
                mDin = new DataInputStream(mSocket.getInputStream());
                int sendBufferSize = 0;
                sendBufferSize = mSocket.getSendBufferSize();
                mSocket.setSendBufferSize(512);
                sendBufferSize = mSocket.getSendBufferSize();
            } catch (IOException e) {
                logger.debug( "connectSocket fail");
                e.printStackTrace();
                disconnectSocket();
                return false;
            }
            logger.debug("connectSocket success");
            return true;
        }

        /**
         * Disconnect socket.
         */
        public void disconnectSocket() {
            try {
                if (mSocket != null)
                    mSocket.close();
                if (mOut != null)
                    mOut.close();
                if (mDin != null)
                    mDin.close();
            } catch (IOException e) {
                e.printStackTrace();
            } finally {
                mSocket = null;
                mOut = null;
                mDin = null;
            }
        }

        /**
         * Write bytes.
         *
         * @param value the value
         * @param len the len
         * @throws IOException Signals that an I/O exception has occurred.
         */
        private void writeBytes(byte[] value, int len) throws IOException {
            mOut.write(value, 0, len);
        }

        /**
         * Write int.
         *
         * @param value the value
         * @throws IOException Signals that an I/O exception has occurred.
         */
        private void writeInt(int value) throws IOException {
            for (int i = 0; i < 4; ++i) {
                mOut.write((value >> (8 * i)) & 0xff);
            }
        }

        /**
         * Write event.
         *
         * @param event the event
         * @return the int
         */
        public int writeEvent(RcsUaEvent event) {
            int ret = -1;
            try {
                synchronized (this) {
                    if (mOut != null) {
                        dumpEvent(event);

                        writeInt(event.getRequestID());
                        writeInt(event.getDataLen());
                        writeBytes(event.getData(), event.getDataLen());

                        mOut.flush();
                        ret = 0;
                    } else {
                        //Xlog.d(TAG, "mOut is null, socket is not setup");
                    }
                }
            } catch (IOException e) {
                return -1;
            }

            return ret;
        }

        /**
         * Read int.
         *
         * @return the int
         * @throws IOException Signals that an I/O exception has occurred.
         */
        private int readInt() throws IOException {
            mDin.readFully(buf, 0, 4);
            return ((buf[3]) << 24 | (buf[2] & 0xff) << 16
                    | (buf[1] & 0xff) << 8 | (buf[0] & 0xff));
        }

        /**
         * Read fully.
         *
         * @param b the b
         * @param off the off
         * @param len the len
         * @throws IOException Signals that an I/O exception has occurred.
         */
        private void readFully(byte b[], int off, int len) throws IOException {
            mDin.readFully(b, off, len);
        }

        /**
         * Read event.
         *
         * @return the rcs ua event
         * @throws IOException Signals that an I/O exception has occurred.
         */
        private RcsUaEvent readEvent() throws IOException {
            //Xlog.d(TAG, "readEvent Enter");
            int request_id;
            int data_len;
            byte buf[];
            RcsUaEvent event;

            request_id = readInt();
            data_len = readInt();

            buf = new byte[data_len];
            readFully(buf, 0, data_len);

            event = new RcsUaEvent(request_id);
            event.putBytes(buf);

            dumpEvent(event);
            return event;
        }

        /**
         * Dump event.
         *
         * @param event the event
         */
        private void dumpEvent(RcsUaEvent event) {
            logger.debug(
                    "dumpEvent: reqiest_id: " + event.getRequestID()
                            + "data_len: " + event.getDataLen() + ",event: "
                            + event.getData());
        }

    }

    //socket name decided on rcs_ua_proxy
    private static final String SOCKET_NAME = "rcs_ua_proxy";

    private static final String TAG = "RcsUaAdapter";
    private static Context mContext;
    private RcsUaSocketIO mIO;
    private RCSProxyEventQueue mEventQueue;
    private static RcsUaEventDispatcher mRCSUAEventDispatcher;

    private static RcsUaAdapter mInstance;
    private static boolean mIsRCSUAAdapterEnabled = false;
    private static boolean mIsRCSUAAdapterInit = false;

    private static boolean mIsSingleRegistrationSupported = false;
    private static boolean mIsRegistered = false;
    private Messenger mMessanger;

    private static boolean isServiceStarted = false;
    /**
     * Gets the single instance of RcsUaAdapter.
     *
     * @return single instance of RcsUaAdapter
     */
    public static RcsUaAdapter getInstance() {
        return mInstance;
    }

    /**
     * Creates the instance.
     *
     * @param context the context
     */
    public static synchronized void createInstance(Context context) {
        if (mInstance == null) {
            mInstance = new RcsUaAdapter(context);
        }
    }

    /**
     * Instantiates a new rcs ua adapter.
     *
     * @param context the context
     */
    private RcsUaAdapter(Context context) {
        mContext = context;
        if (mInstance == null) {
            mInstance = this;
        }
        logger.debug( "RcsUaAdapter constructor");
        mIO = new RcsUaSocketIO(SOCKET_NAME);
        mEventQueue = new RCSProxyEventQueue(mInstance);
        logger.debug("RcsUaAdapter before Looper");
        Thread t1 = new Thread() {
            @Override
            public void run() {
                Looper.prepare();
                mRCSUAEventDispatcher = new RcsUaEventDispatcher(mContext);
                mHandler = new Handler();
                Looper.loop();
            }
        };
        t1.start();
        logger.debug("RcsUaAdapter after Looper.loop");
    }

    /**
     * Initialize.
     */
    public void initialize() {

        if(isServiceStarted()){
            return;
        }
        
        logger.debug("initialize");
        try {
            boolean imsRegState = isIMSRegistered();
            if (imsRegState) {
                logger.debug("IMS registered");
                //enable RCS UA
                Thread t = new Thread() {
                    @Override
                    public void run() {
                        
                        enableRCSProxyAdapter();
                    }
                };
                t.start();
            } else {
                
                logger.debug("IMS not registered");
                
                //register listeneer for when 
                
            }

            
            //register receivers for volte changes
            
            //for IMS State changes 
            IntentFilter filter = new IntentFilter(ImsManager.ACTION_IMS_STATE_CHANGED);
            //filer capability changed
           // filter.addAction(ImsManager.ACTION_IMS_CAPABILITIES_CHANGED);
            filter.addAction(ImsManager.ACTION_IMS_SERVICE_DOWN);

    
            if(volteServiceUpListener==null){
                synchronized (mListenerLock) {
                    volteServiceUpListener = new BroadcastReceiver() {
                         @Override
        public void onReceive(Context context, Intent intent) {
            
            logger.debug("volteServiceUpListener : onReceive");
            

            //IMS STATE UPDATE
             if (intent.getAction().equals(ImsManager.ACTION_IMS_STATE_CHANGED)) {
                 
                 logger.debug("INTENT : ACTION_IMS_STATE_CHANGED");
                     //int get ims state chnage status
                     int IMSState = intent.getIntExtra(ImsManager.EXTRA_IMS_REG_STATE_KEY,ServiceState.STATE_OUT_OF_SERVICE);
                     
                     //IMS is REGISTERED
                     if (IMSState == ServiceState.STATE_IN_SERVICE){
                         if(isServiceStarted()){
                             return;
                          }
                      
                           enableRCSProxyAdapter();
                           
                     }else if(IMSState == ServiceState.STATE_OUT_OF_SERVICE) {
                        //IGNORE AS IMS DEREGISTERD UPDATE WILL BE RECIEVD AND HANDLED BY RCS_UA 
                     }
             } 
             
             //IMS CAPABILITY CHNAGES UPDATE [DIFFERENTIATING BETWEEN WFC / VOLTE REGISTERED IMS]
             else if (intent.getAction().equals(ImsManager.ACTION_IMS_STATE_CHANGED)) {
                 logger.debug("INTENT : ACTION_IMS_CAPABILITIES_CHANGED");
                 int[] enabledFeatures = intent.getIntArrayExtra(ImsManager.EXTRA_IMS_ENABLE_CAP_KEY);
                 if (enabledFeatures[ImsConfig.FeatureConstants.FEATURE_TYPE_VOICE_OVER_WIFI] ==
                         ImsConfig.FeatureConstants.FEATURE_TYPE_VOICE_OVER_WIFI) {
                     
                     logger.debug("IMS enabled via WFC");
                 }else{
                     logger.debug("IMS not enabled via WFC");
                 }
             } 
             
             //INTENT IF VOLTE/ WFC IS DISABLED FROM UI
             else if (intent.getAction().equals(ImsManager.ACTION_IMS_SERVICE_DOWN)) {
                // VOLTE / WFC SERVICE OFF BUTTON WAS PRESSED
                 logger.debug("INTENT : ACTION_IMS_SERVICE_DOWN");
             }
            
        }
    };
    
  AndroidFactory.getApplicationContext().registerReceiver(volteServiceUpListener,filter); 
                }
            }

        } catch (Exception e) {

        }
     }
 
    
    
  /**
     * Initialize.
     */
    public void terminate() {

        if(!isServiceStarted()){
            return;
            }

        try {  
                //enable RCS UA
                Thread t = new Thread() {
                    @Override
                    public void run() {
                        disableRCSUAAdapter(true);
                    }
                };
                t.start();
            
        } catch (Exception e) {

        }
    }
    
    public void unregisterIMSStateUpdates(){
        if(volteServiceUpListener != null){
            synchronized (mListenerLock) {
                if(volteServiceUpListener !=null){
                    AndroidFactory.getApplicationContext().unregisterReceiver(volteServiceUpListener);
                    volteServiceUpListener = null;
                }
                
            }
        }
        
    }

  
    
 /**
     * Battery level listener
     */
    private volatile BroadcastReceiver volteServiceUpListener = null; 
    
    
    public boolean isServiceStarted(){
        return isServiceStarted;
    }
    
    public void setServiceStatus(boolean status){
        isServiceStarted  = status;
    }
    
    private static Handler mHandler;
    private EnableRCSUARunnable mEnableRCSUARunnable;

    
    
    private boolean isIMSRegistered(){
        boolean status = false;
        try{
            status =  ImsManager.getInstance(mContext, 0).getImsRegInfo();    
        }catch(Exception e){
            
            
        }
        return status;
    }
    
    /**
     * The Class EnableRCSUARunnable.
     */
    private class EnableRCSUARunnable implements Runnable {

        /**
         * Run.
         */
        public void run() {
            while (true) {
                if (mIO.connectSocket() == true) {

                    logger.debug("socket connected successfully");
                    // Xlog.d(TAG, "EnableImsRunnable(): connectSocket success");
                    // start the receive thread
                    // start domain event dispatcher to recieve broadcast
                    mRCSUAEventDispatcher.enableRequest();
                    mIsRCSUAAdapterEnabled = true;
                    mIsRCSUAAdapterInit = false;
                    sendRegistrationInfoRequest();
                    break;

                } else {
                    logger.debug("socket connected failure");
                }
            }
        }
    };

    /**
     * Disable rcsua adapter.
     *
     * @param isNormalDisable the is normal disable
     */
    public void disableRCSUAAdapter(boolean isNormalDisable) {

        logger.debug("disableRCSUAAdapter");
        // Xlog.d(TAG, "disableImsAdapter(): misImsAdapterEnabled=" + misImsAdapterEnabled + ", isNormalDisable="+ isNormalDisable);

        
        //in case of not normal disable 
        /*if(!isNormalDisable)
        setIsSingleRegistrationSupported(false);*/
        
        setIsSingleRegistrationSupported(false);
        setRegistrationStatus(false);
        
        if (mIsRCSUAAdapterEnabled) {
            mIsRCSUAAdapterEnabled = false;
            if (mEnableRCSUARunnable != null) {
                mHandler.removeCallbacks(mEnableRCSUARunnable);
                mEnableRCSUARunnable = null;
            }
            //stop the mio thread
            if (mIO != null) {
                mIO.interrupt();
            }
            mRCSUAEventDispatcher.disableRequest();
            mEventQueue.stopEventQueuePolling();
            // TODO: wait time out
            //  mIO.disconnectSocket();
            stopRCSProxyProcess();

        }

        //set the rcs adapter service as false
        setServiceStatus(false);

    }

    // for AP side UT, set event and call ImsAdapter.sendTestEvent(event)
    /**
     * Send test event.
     *
     * @param event the event
     */
    public void sendTestEvent(RcsUaEvent event) {
        // Sample Code:
        // new the event object for Test Event
        // VaEvent event = new VaEvent(MSG_ID_IMSA_IMCB_TEST_A);                            
        // event.putInt(2);                
        // event.putInt(3);        
        mRCSUAEventDispatcher.dispatchCallback(event);
    }

    /**
     * Enable rcs proxy adapter.
     */
    public void enableRCSProxyAdapter() {
        // Xlog.d(TAG, "enableImsAdapter2ndStage()Enter");
        logger.debug("enableRCSProxyAdapter");
        
      //set the service status to true
        setServiceStatus(true);
        
        //start the RCS ua proxy process
        SystemProperties.set("ril.volte.stack.rcsuaproxy", "1");
        
        
        if (mEnableRCSUARunnable != null) {
            mHandler.removeCallbacks(mEnableRCSUARunnable);
            mEnableRCSUARunnable = null;
        }
        if (mIO != null) {
            if (!mIO.isAlive()) {
                logger.debug(
                        "RCS IO thread is not alive ; starting the thread");
                mIO.start();
            } else {
                logger.debug("RCS IO thread is already alive");
            }
        }
        mEnableRCSUARunnable = new EnableRCSUARunnable();
        // mEnableRCSUARunnable.run();
        logger.debug("mEnableRCSUARunnable starts in 3 sec");
        while (mHandler == null) {
            continue;
        }
        mHandler.postDelayed(mEnableRCSUARunnable, 4000);
        mEventQueue.startEventQueuePolling();
    }

    /**
     * Stop rcs proxy process.
     */
    private void stopRCSProxyProcess() {

        logger.debug("stopRCSProxyProcess");
       
        SystemProperties.set("ril.volte.stack.rcsuaproxy", "0");
       
    }

    /**
     * Gets the SIP event dispatcher.
     *
     * @return the SIP event dispatcher
     */
    public RcsUaEventDispatcher.RCSEventDispatcher getSIPEventDispatcher() {
        return mRCSUAEventDispatcher.getSipEventDispatcher();
    }

    /**
     * Gets the registration event dispatcher.
     *
     * @return the registration event dispatcher
     */
    public RcsUaEventDispatcher.RCSEventDispatcher getRegistrationEventDispatcher() {
        return mRCSUAEventDispatcher.getRegistrationEventHandler();
    }

    /**
     * Checks if is single registration supported.
     *
     * @return true, if is single registration supported
     */
    public boolean isSingleRegistrationSupported() {
        logger.debug("isSingleRegistrationSupported : "
                + mIsSingleRegistrationSupported);
        return mIsSingleRegistrationSupported;
    }


    
    private void setIsSingleRegistrationSupported(boolean status){
         logger.debug("setIsSingleRegistrationSupported : "
                 + status);
         
         mIsSingleRegistrationSupported = status;
    }
    


    /**
     * Checks if is registered.
     *
     * @return true, if is registered
     */
    public boolean isRegistered() {
        logger.debug("isRegistered : " + mIsRegistered);
        return mIsRegistered;
    }
    
    /**
     * Sets the registration status.
     *
     * @param status the new registration status
     */
    public void setRegistrationStatus(boolean status) {
        mIsRegistered = status;
        logger.debug("setRegistrationStatus : " + mIsRegistered);
       
    }

    

    /**
     * query the current registration state and information.
     */
    void queryRegistrationState() {

    }

    /**
     * Send test request.
     */
    public void sendTestRequest() {

        sendRegistrationInfoRequest();
    }

    /**
     * Write event to socket.
     *
     * @param event the event
     */
    void writeEvent(RcsUaEvent event) {
        if (event != null) {
            mEventQueue.addEvent(RCSProxyEventQueue.OUTGOING_EVENT_MSG, event);
        }
    }

    /**
     * Send request to RCS_proxy to get the current info about the registration.
     */
    void sendRegistrationInfoRequest() {
        logger.debug("sendRegistrationInfoRequest");
        RcsUaEvent event = new RcsUaAdapter.RcsUaEvent(
                RCS_PROXY_EVENT_REQ_REG_INFO);
        mIO.writeEvent(event);

    }

    /**
     * Send register request.
     */
    private void sendRegisterRequest() {
        logger.debug("sendRegisterRequest with Feature tag :  "
                + getRCSFeatureTag());
        String rcsCapabilityFeatureTags = getRCSFeatureTag();
        int length = rcsCapabilityFeatureTags.length();
        RcsUaEvent event = new RcsUaAdapter.RcsUaEvent(
                RCS_PROXY_EVENT_RCS_REGISTER);
        event.putString(rcsCapabilityFeatureTags, length - 1);
        mIO.writeEvent(event);

    }

    /**
     * handle the registration info send by rcs_proxy and save it based on state.
     *
     * @param event the event
     */
    public void handleRegistrationInfo(RcsUaEvent event) {

        /* REGISTRATION STATES: 
         *     
        VoLTE_Event_Reg_State_Registered        = 1,
        VoLTE_Event_Reg_State_Unregistered      = 2,
        VoLTE_Event_Reg_State_Registering       = 3,
        VoLTE_Event_Reg_State_Deregistering     = 4,
        VoLTE_Event_Reg_State_Disconnected      = 5,
        VoLTE_Event_Reg_State_Authenticating    = 6,
        VoLTE_Event_Reg_State_OOS               = 7,
         * 
         * */

        //INT32 id;           /* /< the account id */
        //INT32 state;        /* /< refer to ::VoLTE_Event_Reg_State_e */
        //INT32 cause;        /* /< refer to ::VoLTE_Event_Reg_Cause_e */
        //UINT32 conn_info;   /* /< the connection information for the others UA to create the connection */

        /* account information */
        // INT8 local_address[VOLTE_MAX_ADDRESS_LENGTH];       /* /< local ip address */
        // INT32 local_port;                                   /* /< local port number */
        // INT32 protocol_type;                                /* /< refer to ::VoLTE_Stack_Protocol_Type_e */
        // INT32 protocol_version;                             /* /< refer to ::VoLTE_Stack_Protocol_Version_e */
        // INT8 public_uid[VOLTE_MAX_REG_UID_LENGTH];          /* /< the public user identity */
        // INT8 private_uid[VOLTE_MAX_REG_UID_LENGTH];         /* /< the private user identity */
        // INT8 home_uri[VOLTE_MAX_URI_LENGTH];                /* /< the domain name of the home network */
        // INT8 pcscf_address[VOLTE_MAX_ADDRESS_LIST_LENGTH];  /* /< the current used PCSCF ip address */
        // INT32 pcscf_port;                                   /* /< the current used PCSCF port number */
        // INT8 imei[VOLTE_MAX_IMEI_LENGTH];                   /* /< the IMEI */
        // INT8 associated_uri[VOLTE_MAX_ASSOCIATED_URI];      /* /< the list of the associated URI */
        // INT8 pub_gruu[VOLTE_MAX_GRUU_LENGTH];               /* /< the public gruu */
        // INT8 temp_gruu[VOLTE_MAX_GRUU_LENGTH];              /* /< the temp gruu */
        // INT8 service_route[VOLTE_MAX_URI_LENGTH];           /* /< the service route */
        // INT8 path[VOLTE_MAX_URI_LENGTH];                    /* /< the path */
        // INT32 net_type;                                     /* /< refer to ::VoLTE_Event_Network_Type_e */
        // INT32 emergency_type;                               /* /< refer to ::VoLTE_Event_Emergency_Type_e */
        // INT8 reg_timestamp[VOLTE_MAX_TIMESTAMP_LENGTH];

        /*
        //INT32 id;           
        INT32 state;                   
        INT8 local_address[VOLTE_MAX_ADDRESS_LENGTH];       
        INT32 local_port;                                   
        INT32 protocol_type;   // 0 -> udp ; 1 -> tcp
        INT32 protocol_version;  //0 -> ipv4 ; 1 ->ipv6                                   
        INT8 public_uid[VOLTE_MAX_REG_UID_LENGTH];          
        INT8 pcscf_address[VOLTE_MAX_ADDRESS_LIST_LENGTH];  
        INT32 pcscf_port;                                  
        INT8  user_agent[VOLTE_MAX_USER_AGENT_LENGTH];                             
        */

        logger.debug("handleRegistrationInfo");

        int length = event.getDataLen();

        int id = 0;
        int state = 0;
        int cause = 0;
        String localaddress = "";
        int localPort = 0;
        int protocolType = 0;
        int protocolVersion = 0;
        String publicUid = "";
        String privateUid = "";
        String homeUri = "";
        String pcscfAddress = "";
        int pcscfPort = 0;
        String imei = "";
        String associatedUri = "";
        String pubGruu = "";
        String tempGruu = "";
        String serviceRoute = "";
        String path = "";
        int netType = 0;
        int emergencyType = 0;
        String regTimestamp = "";
        String userAgent = "";

        id = event.getInt();
        state = event.getInt();
        localaddress = event.getString(64);
        localPort = event.getInt();
        protocolType = event.getInt(); // 0 -> udp ; 1 -> tcp
        
        protocolVersion = event.getInt();//0 -> ipv4 ; 1 ->ipv6   
        
        publicUid = event.getString(64);
        pcscfAddress = event.getString(256);
        pcscfPort = event.getShort();
        
        userAgent = event.getString(128);
 
        logger.debug("data length : " + length + ", " + " reg state info : "
                + state + " , cause :" + cause + " local_address : "
                + localaddress + " local_port : " + localPort
                + " , public_uid : " + publicUid + " , private_uid : "
                + privateUid + " , pcscf_address : " + pcscfAddress
                + "pcscf_port : " + pcscfPort + ", pub_gruu : " + pubGruu
                + " , service_route : " + serviceRoute);

         logger.debug("protocol_type : " + protocolType
                + ", protocol_version : " + protocolVersion
                + ", user-agent : " + userAgent
        		);

        
        String regState = "" + state;

        //check the registration state
            if (IMSRegState.REGISTERED.equals(regState)) {
                
                setIsSingleRegistrationSupported(true);

                IMSProxyAddr = pcscfAddress;
                IMSProxyPort = pcscfPort;
                mLocalIPAddr = localaddress;
                mPrivateID = privateUid;
                mUserAgent = userAgent;

                switch (protocolType) {
                case 1:
                    SIPDefaultProtocolForVoLTE = "UDP";
                    break;
                case 2:
                    SIPDefaultProtocolForVoLTE = "TCP";
                    break;
                default:
                    SIPDefaultProtocolForVoLTE = "UDP";
                }

            }else {
                 logger.debug("IMS Reg state : Not Registered");
                 setIsSingleRegistrationSupported(false);
          
            }

        
        if(
            ((!isRegistered() && (IMSRegState.REGISTERED.equals(regState)))) 
            || 
            ((isRegistered() && (!IMSRegState.REGISTERED.equals(regState))))    
            ){
            
                logger.debug(
                        "send VOLTE_SERVICE_NOTIFY_INTENT to connection manager");
                Intent intent = new Intent(
                        RcsUaAdapter.VOLTE_SERVICE_NOTIFY_INTENT);
                mContext.sendBroadcast(intent);
            }

        /*
        THE response contains like this:
        
         RCS[0] 
        local address  = 192.168.43.1 ,
        local port = 5060,
        protocol_type = 2 ,
        protocol_version = 1,
        public_uid = sip:404119102654903@ims.mnc011.mcc404.3gppnetwork.org,
        private_uid = 404119102654903@ims.mnc011.mcc404.3gppnetwork.org ,
        home_uri = ims.mnc011.mcc404.3gppnetwork.org ,
        pcscf_address = 192.168.43.223 ,
        pcscf_port =5060 , 
        associated_uri = 864855010002295 ,
        pub_gruu = sip:+SIPP@192.168.43.1,tel:+123456789 ,  
        temp_gruu = "sip:callee@example.com;gr=urn:uuid:f81d4fae-7dec-11d0-a765-00a0c91e6bf6" ,
        service_route = "sip:tgruu.7hs==jd7vnzga5w7fajsc7-ajd6fabz0f8g5@example.com;gr" , 
        path = <sip:orig@10.185.16.6:30244;lr;Dpt=7624_246;ca=c2lwOis4NjI4Njg2NTkwODBAMTAuMTg1LjE2LjE2OjI2NjY1> ,
        net_type  = -1204917360 , 
        emergency_type = 7  ,
        reg_timestamp = (null)
        */
    }

    /**
     * Send msg to dispatcher.
     *
     * @param event the event
     */
    protected void sendMsgToDispatcher(RcsUaEvent event) {
        Message msg = new Message();
        msg.obj = event;
        mRCSUAEventDispatcher.sendMessage(msg);
    }

    /**
     * Send msg to rcsua proxy.
     *
     * @param event the event
     */
    protected void sendMsgToRCSUAProxy(RcsUaEvent event) {
        // send the event to va
        mIO.writeEvent(event);
    }

    /**
     * Gets the RCS feature tag.
     *
     * @return the RCS feature tag
     */
    protected String getRCSFeatureTag() {
        String data = "";
        //data = FeatureTags.FEATURE_RCSE;
        data = FeatureTags.FEATURE_CPM_SESSION +","+FeatureTags.FEATURE_CPM_FT;
              
        return data;
    }

    /**
     * Notify ims connection manager connect event.
     */
    public void notifyIMSConnectionManagerConnectEvent() {

    }

    /**
     * Gets the ims proxy addr for vo lte.
     *
     * @return the ims proxy addr for vo lte
     */
    public String[] getImsProxyAddrForVoLTE() {
        String data[] = { IMSProxyAddr };
        data[0] = IMSProxyAddr;
        return data;
    }

    /**
     * Gets the ims proxy port for vo lte.
     *
     * @return the ims proxy port for vo lte
     */
    public int[] getImsProxyPortForVoLTE() {
        int data[] = { IMSProxyPort };
        return data;
    }

    /**
     * Gets the vo lte stack ip address.
     *
     * @return the vo lte stack ip address
     */
    public String getVoLTEStackIPAddress() {
        return mLocalIPAddr;
    }

    /**
     * Gets the local ip address.
     *
     * @return the local ip address
     */
    private String getLocalIPAddress() {
        String localIpAddr = "";
        ConnectivityManager connectivityMgr = (ConnectivityManager) AndroidFactory
                .getApplicationContext().getSystemService(
                        Context.CONNECTIVITY_SERVICE);
        NetworkInfo networkInfo = connectivityMgr.getActiveNetworkInfo();

        if (networkInfo != null) {
            // Get the current local IP address
            localIpAddr = NetworkFactory.getFactory().getLocalIpV6Address(
                    networkInfo.getType());
            logger.debug("Local IPv6 address  " + localIpAddr);
        }

        return localIpAddr;
    }

    /**
     * Gets the host address.
     *
     * @return the host address
     */
    public String getHostAddress() {
        return getVoLTEStackIPAddress();
    }

    public String getUserAgent(){
    	return mUserAgent;
    }

    /**
     * Gets the SIP default protocol for vo lte.
     *
     * @return the SIP default protocol for vo lte
     */
    public String getSIPDefaultProtocolForVoLTE() {
        String data = RcsSettings.getInstance()
                .getSipDefaultProtocolForMobile();
        data = "TCP";//String data = SIPDefaultProtocolForVoLTE;
        //data =   SIPDefaultProtocolForVoLTE;
        return data;
    }

    /**
     * Gets the IMS private id.
     *
     * @return the IMS private id
     */
    public String getIMSPrivateID() {
        String data = "";
        return data;
    }

    /**
     * Sets the RCS sip stack port.
     *
     * @param port the new RCS sip stack port
     */
    public void setRCSSipStackPort(int port) {
        RCSSIPStackPort = port;
    }

    /**
     * Gets the RCS sip stack port.
     *
     * @return the RCS sip stack port
     */
    public int getRCSSipStackPort() {
        return RCSSIPStackPort;
    }

    /**
     * Checks if is IMS via wfc.
     *
     * @return true, if is IMS via wfc
     */
    public boolean isIMSViaWFC() {

        boolean imsWFC = false;//ImsManager.getInstance(mContext, 0).getImsRegInfo();
        return imsWFC;
    }

    /* 
     *  RCS can do single registration only whne 
     *  1) Platform has rcs_ua support
     *  2) Operators allow single registration
     */
    public boolean isSingleRegistrationFeasible(){
        boolean status = false;
        //check rcs_ua support
        //read system property "ro.mtk_rcs_ua_support"
        
        if(SystemProperties.get("ro.mtk_rcs_ua_support").toString().equalsIgnoreCase("1"))
          status = true;
        
        logger.debug("rcs_ua present on platform");
        
        //temp pathc
      //  if(status) return status;
        
    	if(!status)return status;
    	
    	int rcsVolteSingleRegistrationType = RcsSettings.getInstance().getRcsVolteSingleRegistrationType();
    	logger.debug("Operator single registration allowed state :  "+rcsVolteSingleRegistrationType);
    	
    	if(rcsVolteSingleRegistrationType == RcsSettingsData.RCS_VOLTE_SINGLE_REGISTRATION_MODE){  
    		logger.debug("Operator allows single registration in all states ");
    	    status = true;
    	}
    	else if(rcsVolteSingleRegistrationType == RcsSettingsData.RCS_VOLTE_DUAL_REGISTRATION_ROAMING_MODE){
    		
    	    //if phone in roaming , then dont connect via single registration
    	    boolean isPhoneinRoaming = false;
    	    ConnectivityManager connectivityMgr = (ConnectivityManager) AndroidFactory
            .getApplicationContext().getSystemService(
                    Context.CONNECTIVITY_SERVICE);
    	    NetworkInfo networkInfo = connectivityMgr.getActiveNetworkInfo();
    	    
    	    if(networkInfo != null) {
    	        isPhoneinRoaming =  networkInfo.isRoaming();
    	    }else{
    	        isPhoneinRoaming = false;
    	    }
    	    
    	    logger.debug("Phone roaming state : "+isPhoneinRoaming);
    	    //if in romaing , is not feasible
    	    if(isPhoneinRoaming){
    	    	logger.error("single registration not feasible in roaming state");
    	    	status = false;
    	    }else{
    	    	status = true;
    	    }
    	}
    	else if(rcsVolteSingleRegistrationType == RcsSettingsData.RCS_VOLTE_DUAL_REGISTRATION_MODE){
    		 logger.debug("Operator doesnt allow single registration");
    	    status = false;
    	}
        return status;
    }
}
