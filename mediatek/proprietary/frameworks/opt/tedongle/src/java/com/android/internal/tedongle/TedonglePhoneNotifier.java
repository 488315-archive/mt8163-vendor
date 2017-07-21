/*
 * Copyright (C) 2006 The Android Open Source Project
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

package com.android.internal.tedongle;

import android.net.LinkProperties;
import android.net.NetworkCapabilities;
import android.os.Bundle;
import android.os.RemoteException;
import android.os.ServiceManager;
import android.tedongle.CellInfo;
import android.tedongle.DataConnectionRealTimeInfo;
import android.tedongle.Rlog;
import android.tedongle.VoLteServiceState;
import android.tedongle.ServiceState;
import android.tedongle.SubscriptionManager;
import android.tedongle.TelephonyManager;
import android.tedongle.PreciseCallState;
import android.tedongle.DisconnectCause;

import com.android.internal.tedongle.Call;
import com.android.internal.tedongle.CallManager;
import com.android.internal.tedongle.Phone;
import com.android.internal.tedongle.ITelephonyRegistry;
import com.android.internal.tedongle.PhoneConstants;
import android.content.Context;
import android.content.Intent;

import java.util.List;

/**
 * broadcast intents
 */
public class TedonglePhoneNotifier implements PhoneNotifier {
    static final String LOG_TAG = "3GD-GSM";
    private static final boolean DBG = true;
    //protected ITelephonyRegistry mRegistry;
    private ITedongle mTedongleService;
    private Context mContext;
    /*package*/
    protected TedonglePhoneNotifier(Context mct) {

        mContext = mct;
    }

    @Override
    public void notifyPhoneState(Phone sender) {

    }

    @Override
    public void notifyServiceState(Phone sender) {

    }

    @Override
    public void notifySignalStrength(Phone sender) {

        mTedongleService = ITedongle.Stub.asInterface(ServiceManager.getService("tedongleservice"));
        if (mTedongleService != null) {
            try {
                log("tedonglePhoneNotifier->notifySignalStrength!");
                mTedongleService.NotifySignalStrength(sender.getSignalStrength());
            } catch (RemoteException ex) {
                //something wrong
            }
        }

    }

    @Override
    public void notifyMessageWaitingChanged(Phone sender) {
        int phoneId = sender.getPhoneId();
        int subId = sender.getSubId();

        /*try {
            if (mRegistry != null) {
                mRegistry.notifyMessageWaitingChangedForPhoneId(phoneId, subId,
                        sender.getMessageWaitingIndicator());
            }
        } catch (RemoteException ex) {
            // system process is dead
        }*/
    }

    @Override
    public void notifyCallForwardingChanged(Phone sender) {
        int subId = sender.getSubId();
        /*try {
            if (mRegistry != null) {
                mRegistry.notifyCallForwardingChangedForSubscriber(subId,
                        sender.getCallForwardingIndicator());
            }
        } catch (RemoteException ex) {
            // system process is dead
        }*/
    }

    @Override
    public void notifyDataActivity(Phone sender) {
        int subId = sender.getSubId();
        /*try {
            if (mRegistry != null) {
                mRegistry.notifyDataActivityForSubscriber(subId,
                        convertDataActivityState(sender.getDataActivityState()));
            }
        } catch (RemoteException ex) {
            // system process is dead
        }*/
    }

    @Override
    public void notifyDataConnection(Phone sender, String reason, String apnType,
            PhoneConstants.DataState state) {
        if (apnType.equals("default")) {
            
            doNotifyDataConnection(sender, reason, apnType, state);
        }
    }

    private void doNotifyDataConnection(Phone sender, String reason, String apnType,
            PhoneConstants.DataState state) {
        int subId = sender.getSubId();
        long dds = SubscriptionManager.getDefaultDataSubId();
        if (DBG) log("subId = " + subId + ", DDS = " + dds);

        // TODO
        // use apnType as the key to which connection we're talking about.
        // pass apnType back up to fetch particular for this one.
        //TelephonyManager telephony = TelephonyManager.getDefault();
        LinkProperties linkProperties = null;
        NetworkCapabilities networkCapabilities = null;
        boolean roaming = false;

        if (state == PhoneConstants.DataState.CONNECTED) {
            linkProperties = sender.getLinkProperties(apnType);
            networkCapabilities = sender.getNetworkCapabilities(apnType);
        }
        ServiceState ss = sender.getServiceState();
        if (ss != null) roaming = ss.getDataRoaming();

        broadcastDataConnectionStateChanged(convertDataState(state),
                    sender.isDataConnectivityPossible(apnType), 
                    reason,
                    sender.getActiveApnHost(apnType),
                    apnType,
                    linkProperties,
                    networkCapabilities,
                    roaming);


    }

    private void broadcastDataConnectionStateChanged(int state,
            boolean isDataConnectivityPossible,
            String reason, String apn, String apnType, LinkProperties linkProperties,
            NetworkCapabilities linkCapabilities, boolean roaming) {
        // Note: not reporting to the battery stats service here, because the
        // status bar takes care of that after taking into account all of the
        // required info.
        Intent intent = new Intent(TelephonyIntents.ACTION_ANY_DATA_CONNECTION_STATE_CHANGED);
        intent.putExtra(PhoneConstants.STATE_KEY, TedonglePhoneNotifier.convertDataState(state).toString());
        if (!isDataConnectivityPossible) {
            intent.putExtra(PhoneConstants.NETWORK_UNAVAILABLE_KEY, true);
        }
        if (reason != null) {
            intent.putExtra(PhoneConstants.STATE_CHANGE_REASON_KEY, reason);
        }
        if (linkProperties != null) {
            intent.putExtra(PhoneConstants.DATA_LINK_PROPERTIES_KEY, linkProperties);
            String iface = linkProperties.getInterfaceName();
            if (iface != null) {
                intent.putExtra(PhoneConstants.DATA_IFACE_NAME_KEY, iface);
            }
        }
        if (linkCapabilities != null) {
            intent.putExtra(PhoneConstants.DATA_NETWORK_CAPABILITIES_KEY, linkCapabilities);
        }
        if (roaming) intent.putExtra(PhoneConstants.DATA_NETWORK_ROAMING_KEY, true);

        intent.putExtra(PhoneConstants.DATA_APN_KEY, apn);
        intent.putExtra(PhoneConstants.DATA_APN_TYPE_KEY, apnType);

        mContext.sendBroadcast(intent);
        
    }

    @Override
    public void notifyDataConnectionFailed(Phone sender, String reason, String apnType) {
        int subId = sender.getSubId();
        /*try {
            if (mRegistry != null) {
                mRegistry.notifyDataConnectionFailedForSubscriber(subId, reason, apnType);
            }
        } catch (RemoteException ex) {
            // system process is dead
        }*/
    }

    @Override
    public void notifyCellLocation(Phone sender) {
        int subId = sender.getSubId();
        Bundle data = new Bundle();
        sender.getCellLocation().fillInNotifierBundle(data);
        /*try {
            if (mRegistry != null) {
                mRegistry.notifyCellLocationForSubscriber(subId, data);
            }
        } catch (RemoteException ex) {
            // system process is dead
        }*/
    }

    @Override
    public void notifyCellInfo(Phone sender, List<CellInfo> cellInfo) {
        int subId = sender.getSubId();
        /*try {
            if (mRegistry != null) {
                mRegistry.notifyCellInfoForSubscriber(subId, cellInfo);
            }
        } catch (RemoteException ex) {

        }*/
    }

    @Override
    public void notifyDataConnectionRealTimeInfo(Phone sender,
                                                 DataConnectionRealTimeInfo dcRtInfo) {
        /*try {
            mRegistry.notifyDataConnectionRealTimeInfo(dcRtInfo);
        } catch (RemoteException ex) {

        }*/
    }

    @Override
    public void notifyOtaspChanged(Phone sender, int otaspMode) {
        // FIXME: subId?
        /*try {
            if (mRegistry != null) {
                mRegistry.notifyOtaspChanged(otaspMode);
            }
        } catch (RemoteException ex) {
            // system process is dead
        }*/
    }

    public void notifyPreciseCallState(Phone sender) {
        // FIXME: subId?
        Call ringingCall = sender.getRingingCall();
        Call foregroundCall = sender.getForegroundCall();
        Call backgroundCall = sender.getBackgroundCall();
        if (ringingCall != null && foregroundCall != null && backgroundCall != null) {
            /*try {
                mRegistry.notifyPreciseCallState(
                        convertPreciseCallState(ringingCall.getState()),
                        convertPreciseCallState(foregroundCall.getState()),
                        convertPreciseCallState(backgroundCall.getState()));
            } catch (RemoteException ex) {
                // system process is dead
            }*/
        }
    }

    public void notifyDisconnectCause(int cause, int preciseCause) {
        // FIXME: subId?
        /*try {
            mRegistry.notifyDisconnectCause(cause, preciseCause);
        } catch (RemoteException ex) {
            // system process is dead
        }*/
    }

    public void notifyPreciseDataConnectionFailed(Phone sender, String reason, String apnType,
            String apn, String failCause) {
        // FIXME: subId?
        /*try {
            mRegistry.notifyPreciseDataConnectionFailed(reason, apnType, apn, failCause);
        } catch (RemoteException ex) {
            // system process is dead
        }*/
    }

    @Override
    public void notifyVoLteServiceStateChanged(Phone sender, VoLteServiceState lteState) {
        // FIXME: subID
        /*try {
            mRegistry.notifyVoLteServiceStateChanged(lteState);
        } catch (RemoteException ex) {
            // system process is dead
        }*/
    }

    @Override
    public void notifyOemHookRawEventForSubscriber(int subId, byte[] rawData) {
        /*try {
            mRegistry.notifyOemHookRawEventForSubscriber(subId, rawData);
        } catch (RemoteException ex) {
            // system process is dead
        }*/
    }

    /**
     * Convert the {@link PhoneConstants.State} enum into the TelephonyManager.CALL_STATE_*
     * constants for the public API.
     */
    public static int convertCallState(PhoneConstants.State state) {
        switch (state) {
            case RINGING:
                return TelephonyManager.CALL_STATE_RINGING;
            case OFFHOOK:
                return TelephonyManager.CALL_STATE_OFFHOOK;
            default:
                return TelephonyManager.CALL_STATE_IDLE;
        }
    }

    /**
     * Convert the TelephonyManager.CALL_STATE_* constants into the
     * {@link PhoneConstants.State} enum for the public API.
     */
    public static PhoneConstants.State convertCallState(int state) {
        switch (state) {
            case TelephonyManager.CALL_STATE_RINGING:
                return PhoneConstants.State.RINGING;
            case TelephonyManager.CALL_STATE_OFFHOOK:
                return PhoneConstants.State.OFFHOOK;
            default:
                return PhoneConstants.State.IDLE;
        }
    }

    /**
     * Convert the {@link PhoneConstants.DataState} enum into the TelephonyManager.DATA_* constants
     * for the public API.
     */
    public static int convertDataState(PhoneConstants.DataState state) {
        switch (state) {
            case CONNECTING:
                return TelephonyManager.DATA_CONNECTING;
            case CONNECTED:
                return TelephonyManager.DATA_CONNECTED;
            case SUSPENDED:
                return TelephonyManager.DATA_SUSPENDED;
            default:
                return TelephonyManager.DATA_DISCONNECTED;
        }
    }

    /**
     * Convert the TelephonyManager.DATA_* constants into {@link PhoneConstants.DataState} enum
     * for the public API.
     */
    public static PhoneConstants.DataState convertDataState(int state) {
        switch (state) {
            case TelephonyManager.DATA_CONNECTING:
                return PhoneConstants.DataState.CONNECTING;
            case TelephonyManager.DATA_CONNECTED:
                return PhoneConstants.DataState.CONNECTED;
            case TelephonyManager.DATA_SUSPENDED:
                return PhoneConstants.DataState.SUSPENDED;
            default:
                return PhoneConstants.DataState.DISCONNECTED;
        }
    }

    /**
     * Convert the {@link Phone.DataActivityState} enum into the TelephonyManager.DATA_* constants
     * for the public API.
     */
    public static int convertDataActivityState(Phone.DataActivityState state) {
        switch (state) {
            case DATAIN:
                return TelephonyManager.DATA_ACTIVITY_IN;
            case DATAOUT:
                return TelephonyManager.DATA_ACTIVITY_OUT;
            case DATAINANDOUT:
                return TelephonyManager.DATA_ACTIVITY_INOUT;
            case DORMANT:
                return TelephonyManager.DATA_ACTIVITY_DORMANT;
            default:
                return TelephonyManager.DATA_ACTIVITY_NONE;
        }
    }

    /**
     * Convert the TelephonyManager.DATA_* constants into the {@link Phone.DataActivityState} enum
     * for the public API.
     */
    public static Phone.DataActivityState convertDataActivityState(int state) {
        switch (state) {
            case TelephonyManager.DATA_ACTIVITY_IN:
                return Phone.DataActivityState.DATAIN;
            case TelephonyManager.DATA_ACTIVITY_OUT:
                return Phone.DataActivityState.DATAOUT;
            case TelephonyManager.DATA_ACTIVITY_INOUT:
                return Phone.DataActivityState.DATAINANDOUT;
            case TelephonyManager.DATA_ACTIVITY_DORMANT:
                return Phone.DataActivityState.DORMANT;
            default:
                return Phone.DataActivityState.NONE;
        }
    }

    /**
     * Convert the {@link State} enum into the PreciseCallState.PRECISE_CALL_STATE_* constants
     * for the public API.
     */
    public static int convertPreciseCallState(Call.State state) {
        switch (state) {
            case ACTIVE:
                return PreciseCallState.PRECISE_CALL_STATE_ACTIVE;
            case HOLDING:
                return PreciseCallState.PRECISE_CALL_STATE_HOLDING;
            case DIALING:
                return PreciseCallState.PRECISE_CALL_STATE_DIALING;
            case ALERTING:
                return PreciseCallState.PRECISE_CALL_STATE_ALERTING;
            case INCOMING:
                return PreciseCallState.PRECISE_CALL_STATE_INCOMING;
            case WAITING:
                return PreciseCallState.PRECISE_CALL_STATE_WAITING;
            case DISCONNECTED:
                return PreciseCallState.PRECISE_CALL_STATE_DISCONNECTED;
            case DISCONNECTING:
                return PreciseCallState.PRECISE_CALL_STATE_DISCONNECTING;
            default:
                return PreciseCallState.PRECISE_CALL_STATE_IDLE;
        }
    }

    /**
     * Convert the Call.State.* constants into the {@link State} enum
     * for the public API.
     */
    public static Call.State convertPreciseCallState(int state) {
        switch (state) {
            case PreciseCallState.PRECISE_CALL_STATE_ACTIVE:
                return Call.State.ACTIVE;
            case PreciseCallState.PRECISE_CALL_STATE_HOLDING:
                return Call.State.HOLDING;
            case PreciseCallState.PRECISE_CALL_STATE_DIALING:
                return Call.State.DIALING;
            case PreciseCallState.PRECISE_CALL_STATE_ALERTING:
                return Call.State.ALERTING;
            case PreciseCallState.PRECISE_CALL_STATE_INCOMING:
                return Call.State.INCOMING;
            case PreciseCallState.PRECISE_CALL_STATE_WAITING:
                return Call.State.WAITING;
            case PreciseCallState.PRECISE_CALL_STATE_DISCONNECTED:
                return Call.State.DISCONNECTED;
            case PreciseCallState.PRECISE_CALL_STATE_DISCONNECTING:
                return Call.State.DISCONNECTING;
            default:
                return Call.State.IDLE;
        }
    }

    public interface IDataStateChangedCallback {
        void onDataStateChanged(int subId, String state, String reason, String apnName,
            String apnType, boolean unavailable);
    }

    private void log(String s) {
        Rlog.d(LOG_TAG, s);
    }
}
