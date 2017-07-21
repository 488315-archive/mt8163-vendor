/* Copyright Statement:
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein is
 * confidential and proprietary to MediaTek Inc. and/or its licensors. Without
 * the prior written permission of MediaTek inc. and/or its licensors, any
 * reproduction, modification, use or disclosure of MediaTek Software, and
 * information contained herein, in whole or in part, shall be strictly
 * prohibited.
 *
 * MediaTek Inc. (C) 2010. All rights reserved.
 *
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE")
 * RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO RECEIVER
 * ON AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL
 * WARRANTIES, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR
 * NONINFRINGEMENT. NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH
 * RESPECT TO THE SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY,
 * INCORPORATED IN, OR SUPPLIED WITH THE MEDIATEK SOFTWARE, AND RECEIVER AGREES
 * TO LOOK ONLY TO SUCH THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO.
 * RECEIVER EXPRESSLY ACKNOWLEDGES THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO
 * OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES CONTAINED IN MEDIATEK
 * SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK SOFTWARE
 * RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S
 * ENTIRE AND CUMULATIVE LIABILITY WITH RESPECT TO THE MEDIATEK SOFTWARE
 * RELEASED HEREUNDER WILL BE, AT MEDIATEK'S OPTION, TO REVISE OR REPLACE THE
 * MEDIATEK SOFTWARE AT ISSUE, OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE
 * CHARGE PAID BY RECEIVER TO MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 *
 * The following software/firmware and/or related documentation ("MediaTek
 * Software") have been modified by MediaTek Inc. All revisions are subject to
 * any receiver's applicable license agreements with MediaTek Inc.
 */

package com.mediatek.connectivity;


import android.app.Activity;
import android.content.Context;
import android.net.TrafficStats;
import android.os.Bundle;
import android.util.Log;
import android.widget.TextView;

/**
 * Class for traffic statistic information.
 *
 */
public class CdsTrafficStatActivity extends Activity {
    private static final String TAG = "CDSINFO/CdsTrafficStatActivity";
    private Context mContext;
    private TextView mTotalTxEdit;
    private TextView mTotalRxEdit;
    private TextView mTotalMobileTxEdit;
    private TextView mTotalMobileRxEdit;
    private TextView mWlanEdit;
    private TextView mCcmni0Edit;
    private TextView mCcmni1Edit;
    private TextView mCcmni2Edit;


    private static final String[] NETWORK_INTERFACES = {"wlan0", "ccmni0", "ccmni1" , "ccmni2"};


    @Override
    public void onCreate(Bundle icicle) {
        super.onCreate(icicle);

        setContentView(R.layout.cds_traffic_info);

        mContext = this.getBaseContext();

        if (mContext == null) {
            Log.e(TAG, "Could not get Conext of this activity");
            return;
        }

        mTotalTxEdit = (TextView) findViewById(R.id.tx_info);
        mTotalRxEdit = (TextView) findViewById(R.id.rx_info);
        mTotalMobileTxEdit = (TextView) findViewById(R.id.tx_mobile_info);
        mTotalMobileRxEdit = (TextView) findViewById(R.id.rx_mobile_info);
        mWlanEdit = (TextView) findViewById(R.id.wlan_info);
        mCcmni0Edit = (TextView) findViewById(R.id.ccmni0_info);
        mCcmni1Edit = (TextView) findViewById(R.id.ccmni1_info);
        mCcmni2Edit = (TextView) findViewById(R.id.ccmni2_info);

        updateTrafficInfo();

        Log.i(TAG, "CdsTrafficStatActivity is started");
    }



    private void updateTrafficInfo() {

        String infoTxString;
        String infoRxString;
        String iface;
        Long packetTxSize = (long) 0;
        Long byteTxSize = (long) 0;
        Long packetRxSize = (long) 0;
        Long byteRxSize = (long) 0;

        packetTxSize = TrafficStats.getTotalTxPackets();
        byteTxSize = TrafficStats.getTotalTxBytes();
        infoTxString = "TX pacekts:" + packetTxSize
                        + " bytes:" + byteTxSize + " (" + byteTxSize / 1024 + " KBytes)";
        packetRxSize = TrafficStats.getTotalRxPackets();
        byteRxSize = TrafficStats.getTotalRxBytes();
        infoRxString = "RX pacekts:" + packetRxSize
                        + " bytes:" + byteRxSize + " (" + byteRxSize / 1024 + " KBytes)";

        mTotalTxEdit.setText(infoTxString);
        mTotalRxEdit.setText(infoRxString);

        packetTxSize = TrafficStats.getMobileTxPackets();
        byteTxSize = TrafficStats.getMobileTxBytes();
        infoTxString = "Mobile TX pacekts:" + packetTxSize
                        + " bytes:" + byteTxSize + " (" + byteTxSize / 1024 + " KBytes)";
        packetRxSize = TrafficStats.getMobileRxPackets();
        byteRxSize = TrafficStats.getMobileRxBytes();
        infoRxString = "Mobile RX pacekts:" + packetRxSize
                        + " bytes:" + byteRxSize + " (" + byteRxSize / 1024 + " KBytes)";

        mTotalMobileTxEdit.setText(infoTxString);
        mTotalMobileRxEdit.setText(infoRxString);

        //for wlan 0

        iface = NETWORK_INTERFACES[0];
        mWlanEdit.setText(getTxInfoByIface(iface));

        iface = NETWORK_INTERFACES[1];
        mCcmni0Edit.setText(getTxInfoByIface(iface));
        iface = NETWORK_INTERFACES[2];
        mCcmni1Edit.setText(getTxInfoByIface(iface));
        iface = NETWORK_INTERFACES[3];
        mCcmni2Edit.setText(getTxInfoByIface(iface));

    }

    private String getTxInfoByIface(String iface) {

        String infoTxString = "";
        String infoRxString = "";
        Long packetTxSize = (long) 0;
        Long byteTxSize = (long) 0;
        Long packetRxSize = (long) 0;
        Long byteRxSize = (long) 0;


        packetTxSize = TrafficStats.getTxPackets(iface);
        byteTxSize = TrafficStats.getTxBytes(iface);
        infoTxString = "TX pacekts:" + packetTxSize
                        + " bytes:" + byteTxSize + " (" + byteTxSize / 1024 + " KBytes)";
        Log.i(TAG, infoTxString);

        packetRxSize = TrafficStats.getRxPackets(iface);
        byteRxSize = TrafficStats.getRxBytes(iface);
        infoRxString = "RX pacekts:" + packetRxSize
                        + " bytes:" + byteRxSize + " (" + byteRxSize / 1024 + " KBytes)";

        Log.i(TAG, infoTxString);

        return infoTxString + "\r\n" + infoRxString;
    }

    @Override
    protected void onResume() {
        super.onResume();

        updateTrafficInfo();

    }

}