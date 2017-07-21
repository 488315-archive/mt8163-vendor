/* Copyright Statement:
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein
 * is confidential and proprietary to MediaTek Inc. and/or its licensors.
 * Without the prior written permission of MediaTek inc. and/or its licensors,
 * any reproduction, modification, use or disclosure of MediaTek Software,
 * and information contained herein, in whole or in part, shall be strictly prohibited.
 */
/* MediaTek Inc. (C) 2010. All rights reserved.
 *
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE")
 * RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO RECEIVER ON
 * AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL WARRANTIES,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
 * NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
 * SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
 * SUPPLIED WITH THE MEDIATEK SOFTWARE, AND RECEIVER AGREES TO LOOK ONLY TO SUCH
 * THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES
 * THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES
 * CONTAINED IN MEDIATEK SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK
 * SOFTWARE RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND
 * CUMULATIVE LIABILITY WITH RESPECT TO THE MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
 * AT MEDIATEK'S OPTION, TO REVISE OR REPLACE THE MEDIATEK SOFTWARE AT ISSUE,
 * OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY RECEIVER TO
 * MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 *
 * The following software/firmware and/or related documentation ("MediaTek Software")
 * have been modified by MediaTek Inc. All revisions are subject to any receiver's
 * applicable license agreements with MediaTek Inc.
 */

package com.mediatek.engineermode.eminfo;

import android.app.Activity;

import android.os.AsyncResult;
import android.os.Handler;
import android.os.Message;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ArrayAdapter;
import android.widget.LinearLayout;
import android.widget.ListView;
import android.widget.TextView;

import com.mediatek.engineermode.Elog;
import com.mediatek.engineermode.R;
import com.mediatek.engineermode.eminfo.UrcParser.Parser;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.Random;

abstract public class Component implements Comparable<Component> {
    private static final int TYPE_NORMAL = 1;
    private static final int TYPE_ARRAY = 2;
    private static final int TYPE_GRAPH = 4;
    protected static final String TAG = "EmInfo";
    protected Activity mActivity;

    static private List<Component> mComponents;

    @Override
    public int compareTo(Component another) {
        return getName().compareTo(another.getName());
    }

    static List<Component> getComponents(Activity context) {
        mComponents = new ArrayList<Component>();
        mComponents.add(new RRCState(context));
        mComponents.add(new RRState(context));
        mComponents.add(new ERRCState(context));
        mComponents.add(new HighPriorityPLMNSearch(context));
        mComponents.add(new CarrierRSSIServing(context));
        mComponents.add(new GsmServingCellInfo(context));
        mComponents.add(new EUtranNeighborCellInfo(context));
        mComponents.add(new TddServingCellInfo(context));
        mComponents.add(new LaiRaiUmtsTdd(context));
        mComponents.add(new GeranNeighborCellInfoUmtsTdd(context));
        mComponents.add(new EUtranNeighborCellInfoUmtsTdd(context));
        mComponents.add(new LteServingCellInfo(context));
        mComponents.add(new LteSCellInfo(context));
        mComponents.add(new PrimaryCellDlBlockErrorRate(context));
        mComponents.add(new PrimaryCellDownlinkBandwidth(context));
        mComponents.add(new PrimaryCellUplinkBandwidth(context));
        mComponents.add(new PrimaryCellUplinkDownlinkConfiguration(context));
        mComponents.add(new PrimaryCellSpecialSubframeConfiguration(context));
        mComponents.add(new PrimaryCellTransmissionMode(context));
        mComponents.add(new SecondaryCellDlBlockErrorRate(context));
        mComponents.add(new SecondaryCellDownlinkBandwidth(context));
        mComponents.add(new SecondaryCellUplinkBandwidth(context));
        mComponents.add(new SecondaryCellUplinkDownlinkConfiguration(context));
        mComponents.add(new SecondaryCellSpecialSubframeConfiguration(context));
        mComponents.add(new SecondaryCellTransmissionMode(context));
        mComponents.add(new Tai(context));
        mComponents.add(new EmIrReport(context));
        mComponents.add(new UmtsRrcMeasurementControlForE3a(context));
        mComponents.add(new UmtsRrcMeasurementControlForE3b(context));
        mComponents.add(new UmtsRrcMeasurementControlForE3c(context));
        mComponents.add(new UmtsRrcMeasurementControlForE3d(context));
        mComponents.add(new UmtsRrcMeasurementReportForE3a(context));
        mComponents.add(new UmtsRrcMeasurementReportForE3b(context));
        mComponents.add(new UmtsRrcMeasurementReportForE3c(context));
        mComponents.add(new UmtsRrcMeasurementReportForE3d(context));
        mComponents.add(new UmtsNeighborCellInfoGsmTdd(context));
        mComponents.add(new LTENeighborCellInfoGSM(context));
        mComponents.add(new LteErrcMeasurementConfigForEventB1(context));
        mComponents.add(new LteErrcMeasurementConfigForEventB2(context));
        mComponents.add(new LteErrcMeasurementReportForEventB1(context));
        mComponents.add(new LteErrcMeasurementReportForEventB2(context));
        mComponents.add(new EDchTtiConfigured(context));
        mComponents.add(new HsdpaConfiguredUmtsFdd(context));
        mComponents.add(new DcHsdpaConfiguredUmtsFdd(context));
        mComponents.add(new HsDschModulationPrimaryCell(context));
        mComponents.add(new HsupaConfiguredUmtsFdd(context));
        mComponents.add(new WcdmaHsupaCapability(context));
        mComponents.add(new WcdmaHsdpaCapability(context));
        mComponents.add(new Fdd3gSpeechCodec(context));
        mComponents.add(new PlmnSearchStatusUmtsFdd(context));
        mComponents.add(new CellSupportPsInfo(context));
        mComponents.add(new DtxConfiguredUmtsFdd(context));
        mComponents.add(new DrxConfiguredUmtsFdd(context));
        mComponents.add(new FastDormancyConfiguration(context));
        mComponents.add(new IntraFrequencyMonitoredSetUmtsFdd(context));
        mComponents.add(new IntraFrequencyDetectedSetUmtsFdd(context));
        mComponents.add(new ActiveSetUmtsFdd(context));
        mComponents.add(new CsOverHspaUmtsFdd(context));
        mComponents.add(new ShareNetworkPlmnInfo(context));
        mComponents.add(new ServingCellPlmnIdentityUmtsFdd(context));
        mComponents.add(new PrimaryCellCqi(context));
        mComponents.add(new PrimaryCellDlImcs(context));
        mComponents.add(new PrimaryCellDlResourceBlock(context));
        mComponents.add(new PrimaryCellAntennaPort(context));
        mComponents.add(new PrimaryCellDlThroughput(context));
        mComponents.add(new PrimaryCellDlBlockRate(context));
        mComponents.add(new SecondaryCellCqi(context));
        mComponents.add(new SecondaryCellDlImcs(context));
        mComponents.add(new SecondaryCellDlResourceBlock(context));
        mComponents.add(new SecondaryCellAntennaPort(context));
        mComponents.add(new SecondaryCellDlThroughput(context));
        mComponents.add(new SecondaryCellDlBlockRate(context));
        mComponents.add(new DownlinkDataThroughput(context));
        mComponents.add(new UplinkDataThroughput(context));
        mComponents.add(new HandoverIntraLte(context));
        mComponents.add(new RatAndServiceStatus(context));
        mComponents.add(new HsDschServingCellUmtsFdd(context));
        mComponents.add(new SecondHsDschServingCell(context));
        mComponents.add(new BasicInfoServingGsm(context));
        mComponents.add(new CaInfo(context));
        mComponents.add(new RsrpLteCandidateCellUmtsFdd(context));
        mComponents.add(new RsrqLteCandidateCellUmtsFdd(context));
        mComponents.add(new UmtsRscp(context));
        mComponents.add(new UmtsEcn0(context));
        mComponents.add(new IntraFrequencyNeighbouringCellInformationLteTdd(context));
        mComponents.add(new InterFrequencyNeighbouringCellInformationLteTdd(context));
        mComponents.add(new GeranNeighbouringCellInformation(context));
        mComponents.add(new UtraTddNeighbouringCellInformation(context));
        mComponents.add(new ActivePdpContextInformation(context));
        mComponents.add(new WcdmaTasInfo(context));
        mComponents.add(new GSMTasInfo(context));
        mComponents.add(new PrimaryCellRsrpRx(context));
        mComponents.add(new PrimaryCellRsrp(context));
        mComponents.add(new PrimaryCellRsrq(context));
        mComponents.add(new PrimaryCellRssiRx(context));
        mComponents.add(new PrimaryCellSnrRx(context));
        mComponents.add(new PrimaryCellOsSnr(context));
        mComponents.add(new SecondaryCellRsrpRx(context));
        mComponents.add(new SecondaryCellRsrp(context));
        mComponents.add(new SecondaryCellRsrq(context));
        mComponents.add(new SecondaryCellRssiRx(context));
        mComponents.add(new SecondaryCellSnrRx(context));
        mComponents.add(new SecondaryCellOsSnr(context));
        return mComponents;
    }

    /**
     * @param context
     *            the context
     */
    public Component(Activity context) {
        mActivity = context;
    }

    abstract View getView();
    abstract void removeView();
    abstract String getName();
    abstract String getGroup();
    abstract int[] getEmType();
    abstract void update(int type, Parser data);

    boolean hasEmType(int type) {
        int[] types = getEmType();
        if (types != null) {
            for (int i = 0; i < types.length; i++) {
                if (types[i] == type) {
                    return true;
                }
            }
        }
        return false;
    }
}

class TableInfoAdapter extends ArrayAdapter<String[]> {
    private Activity mActivity;

    public TableInfoAdapter(Activity activity) {
        super(activity, 0);
        mActivity = activity;
    }

    private class ViewHolder {
        public TextView[] texts;
    }

    @Override
    public View getView(int position, View convertView, ViewGroup parent) {
        ViewHolder holder;
        LayoutInflater inflater = mActivity.getLayoutInflater();
        if (convertView == null) {
            convertView = inflater.inflate(R.layout.em_info_entry, null);
            holder = new ViewHolder();
            holder.texts = new TextView[10];
            holder.texts[0] = (TextView) convertView.findViewById(R.id.info1);
            holder.texts[1] = (TextView) convertView.findViewById(R.id.info2);
            holder.texts[2] = (TextView) convertView.findViewById(R.id.info3);
            holder.texts[3] = (TextView) convertView.findViewById(R.id.info4);
            holder.texts[4] = (TextView) convertView.findViewById(R.id.info5);
            holder.texts[5] = (TextView) convertView.findViewById(R.id.info6);
            holder.texts[6] = (TextView) convertView.findViewById(R.id.info7);
            holder.texts[7] = (TextView) convertView.findViewById(R.id.info8);
            holder.texts[8] = (TextView) convertView.findViewById(R.id.info9);
            holder.texts[9] = (TextView) convertView.findViewById(R.id.info10);
            convertView.setTag(holder);
        } else {
            holder = (ViewHolder) convertView.getTag();
        }
        String[] text = getItem(position);
        for (int i = 0; i < text.length; i++) {
            holder.texts[i].setText(text[i]);
            holder.texts[i].setVisibility(View.VISIBLE);
        }
        for (int i = text.length; i < holder.texts.length; i++) {
            holder.texts[i].setVisibility(View.GONE);
        }
        return convertView;
    }
}

abstract class NormalTableComponent extends Component {
    ListView mListView;
    TableInfoAdapter mAdapter;
    String[] mLabels;

    public NormalTableComponent(Activity context) {
        super(context);
        if (mAdapter == null) {
            mAdapter = new TableInfoAdapter(mActivity);
        }
        if (mListView == null) {
            mListView = new ListView(mActivity);
        }
    }

    @Override
    View getView() {
        Elog.d(TAG, "getView");
        if (mLabels == null) {
            mLabels = getLabels();
        }
        if (mAdapter.getCount() == 0) {
            // Show placeholder to avoid showing empty list
            for (int i = 0; i < mLabels.length; i++) {
                mAdapter.add(new String[] {mLabels[i], ""});
            }
        }
        mListView.setAdapter(mAdapter);
        mAdapter.notifyDataSetChanged();
        return mListView;
    }

    @Override
    void removeView() {
        Elog.d(TAG, "removeView");
        mListView.setAdapter(null);
    }

    void clearData() {
        mAdapter.clear();
    }

    void addData(Object... data) {
        for (int i = 0; i < data.length; i++) {
            addData(String.valueOf(data[i]));
        }
    }

    void addDataAtPostion(int pos, Object... data) {
        for (int i = 0; i < data.length; i++) {
            addDataAtPostion(pos, String.valueOf(data[i]));
        }
    }

    void addData(String data) {
        if (mLabels == null) {
            mLabels = getLabels();
        }
        int position = mAdapter.getCount();
        mAdapter.add(new String[] {mLabels[position % mLabels.length], data == null ? "" : data});
        mAdapter.notifyDataSetChanged();
    }

    void addDataAtPostion(int pos, String data) {
        if (mLabels == null) {
            mLabels = getLabels();
        }
        for (int i = mAdapter.getCount(); i <= pos; i++) {
            mAdapter.add(new String[] {mLabels[i], ""});
        }
        String[] item = mAdapter.getItem(pos);
        item[1] = data;
        mAdapter.notifyDataSetChanged();
    }

    void notifyDataSetChanged() {
        if (mLabels == null) {
            mLabels = getLabels();
        }
        if (mAdapter.getCount() < mLabels.length) {
            // Show placeholder to avoid showing incomplete list
            for (int i = mAdapter.getCount(); i < mLabels.length; i++) {
                mAdapter.add(new String[] {mLabels[i], ""});
            }
        }
        mAdapter.notifyDataSetChanged();
    }

    abstract String[] getLabels();
}

abstract class ArrayTableComponent extends Component {
    ListView mListView;
    TableInfoAdapter mAdapter;
    String[] mLabels;

    public ArrayTableComponent(Activity context) {
        super(context);
        if (mAdapter == null) {
            mAdapter = new TableInfoAdapter(mActivity);
        }
        if (mListView == null) {
            mListView = new ListView(mActivity);
        }
    }

    @Override
    View getView() {
        Elog.d(TAG, "getView");
        if (mLabels == null) {
            mLabels = getLabels();
        }
        if (mAdapter.getCount() == 0) {
            // Show placeholder to avoid showing empty list
            mAdapter.add(mLabels);
        }
        mListView.setAdapter(mAdapter);
        mAdapter.notifyDataSetChanged();
        return mListView;
    }

    @Override
    void removeView() {
        Elog.d(TAG, "removeView");
        mListView.setAdapter(null);
    }

    void clearData() {
        if (mLabels == null) {
            mLabels = getLabels();
        }
        mAdapter.clear();
        mAdapter.add(mLabels);
        mAdapter.notifyDataSetChanged();
    }

    void addData(Object... data) {
        String[] strings = new String[data.length];
        for (int i = 0; i < data.length; i++) {
            strings[i] = String.valueOf(data[i]);
        }
        addData(strings);
    }

    void addData(String[] data) {
        mAdapter.add(data);
        mAdapter.notifyDataSetChanged();
    }

    abstract String[] getLabels();
}

abstract class CurveComponent extends Component {
    CurveView mCurveView;
    View mView;
    TextView mXLabel;
    TextView mYLabel;
    HashMap<Integer, float[]> mData = new HashMap<Integer, float[]>();
    Random rand = new Random();
    long mStartTime;
    boolean mStarted;

    public CurveComponent(Activity context) {
        super(context);
    }

    @Override
    View getView() {
        Elog.d(TAG, "getView");
        if (mView == null) {
            mView = mActivity.getLayoutInflater().inflate(R.layout.em_info_curve, null);
            mXLabel = (TextView) mView.findViewById(R.id.em_info_curve_x_label);
            mYLabel = (TextView) mView.findViewById(R.id.em_info_curve_y_label);
            mCurveView = (CurveView) mView.findViewById(R.id.em_info_curve);
            if (!mStarted) {
                mStartTime = System.currentTimeMillis() / 10000 * 10;
                mStarted = true;
            }

            mXLabel.setText("Time");
            CurveView.AxisConfig xConfig = new CurveView.AxisConfig();
            xConfig.base = mStartTime;
            xConfig.min = 0;
            xConfig.max = 200;
            xConfig.step = 1;
            xConfig.configMin = true;
            xConfig.configMax = true;
            xConfig.configStep = true;
            xConfig.type = CurveView.AxisConfig.TYPE_TIME;
            CurveView.AxisConfig yConfig = configY();
            mCurveView.setAxisConfig(xConfig, yConfig);
        }
        return mView;
    }

    CurveView.AxisConfig configY() {
        mYLabel.setText("dBm");

        CurveView.Config[] configs = new CurveView.Config[1];
        configs[0] = new CurveView.Config();
        configs[0].color = 0xFF880000;
        configs[0].lineWidth = 2;
        configs[0].nodeType = CurveView.Config.TYPE_CIRCLE;
        mCurveView.setConfig(configs);

        CurveView.AxisConfig yConfig = new CurveView.AxisConfig();
        yConfig.min = 0;
        yConfig.max = 30;
        yConfig.step = 2;
        yConfig.configMin = true;
        yConfig.configMax = true;
        yConfig.configStep = true;
        return yConfig;
    }

    @Override
    void removeView() {
        Elog.d(TAG, "removeView");
//        mCurveView = null;
    }

    void addData(int index, float data) {
        if (!mStarted) {
            mStartTime = System.currentTimeMillis() / 10000 * 10;
            mStarted = true;
        }

        long time = System.currentTimeMillis() / 1000;

        // TODO: unit test
//        data = ((long) data) % 30;
        // TODO: unit test end

        float[] d = mData.get(index);
        if (d == null) {
            float[] tmp = new float[2];
            tmp[0] = time - mStartTime;
            tmp[1] = data;
            mData.put(index, tmp);
        } else {
            // TODO: unit test
//            if (d.length > 100) return;
//            if ((long) data == (long) d[d.length - 1]) return;
            // TODO: unit test end

            float[] tmp = new float[d.length + 2];
            for (int i = 0; i < d.length; i++) {
                tmp[i] = d[i];
            }
            tmp[d.length] = time - mStartTime;
            tmp[d.length + 1] = data;
            mData.put(index, tmp);
        }
        if (mCurveView != null) {
            mCurveView.setData(index, mData.get(index));
        }
    }
}

class RRCState extends NormalTableComponent {
    private static final int[] EM_TYPES = new int[] {Content.FDD_EM_URR_3G_GENERAL_STATUS_IND};
    private final HashMap<Integer, String> mStateMapping = new HashMap<Integer, String>() { {
        put(0, "Idle");
        put(1, "Cell FACH");
        put(2, "Cell PCH");
        put(3, "URA PCH");
        put(4, "Cell DCH");
        put(5, "");
        put(6, "NO_CHANGE");
        put(7, "Cell FACH");
        put(8, "Cell PCH");
        put(9, "Cell PCH");
        put(10, "URA PCH");
        put(11, "Cell FACH");
        put(12, "Cell PCH");
        put(13, "Cell PCH");
    } };

    public RRCState(Activity context) {
        super(context);
    }

    @Override
    String getName() {
        return "RRC State";
    }

    @Override
    String getGroup() {
        return "3. UMTS FDD EM Component";
    }

    @Override
    int[] getEmType() {
        return EM_TYPES;
    }

    @Override
    String[] getLabels() {
        return new String[] {"RRC State"};
    }

    @Override
    void update(int type, Parser data) {
        int state = data.getInt("umts_rrc_state");
        if (state != 6) {
            String stateString = mStateMapping.get(state);
            clearData();
            addData(stateString);
        }
    }
}

class RRState extends NormalTableComponent {
    private static final int[] EM_TYPES = new int[] {Content.RR_EM_MEASUREMENT_REPORT_INFO};
    private static final String[] LABELS = new String[] {"RR State"};
    private final Map<Integer, String> mRrStateMapping = new HashMap<Integer, String>() { {
        put(3, "Idle");
        put(5, "Packet Transfer");
        put(6, "Dedicated");
    } };

    public RRState(Activity context) {
        super(context);
    }

    @Override
    String getName() {
        return "RR State";
    }

    @Override
    String getGroup() {
        return "2. GSM EM Component";
    }

    @Override
    int[] getEmType() {
        return EM_TYPES;
    }

    @Override
    String[] getLabels() {
        return LABELS;
    }

    @Override
    void update(int type, Parser data) {
        String state = mRrStateMapping.get(data.getInt("rr_state"));
        clearData();
        addData(state);
    }
}

class ERRCState extends NormalTableComponent {
    private static final int[] EM_TYPES = new int[] {Content.ERRC_EM_ERRC_STATE_IND};
    HashMap<Integer, String> mMapping = new HashMap<Integer, String>() { {
        put(1, "");
        put(2, "Idle");
        put(3, "Connected");
        put(4, "");
        put(5, "Idle");
        put(6, "Connected");
    } };

    public ERRCState(Activity context) {
        super(context);
    }

    @Override
    String getName() {
        return "ERRC State";
    }

    @Override
    String getGroup() {
        return "5. LTE EM Component";
    }

    @Override
    int[] getEmType() {
        return EM_TYPES;
    }

    String[] getLabels() {
        return new String[] {"RRC State"};
    }

    @Override
    void update(int type, Parser data) {
        String state = mMapping.get(data.getInt("errc_sts"));
        clearData();
        addData(state);
    }
}

class HighPriorityPLMNSearch extends NormalTableComponent {
    private static final int[] EM_TYPES = new int[] {Content.MMRR_EM_PLMN_INFO_STRUCT_INFO};

    public HighPriorityPLMNSearch(Activity context) {
        super(context);
    }

    @Override
    String getName() {
        return "High Priority PLMN Search";
    }

    @Override
    String getGroup() {
        return "1. General EM Component";
    }

    @Override
    int[] getEmType() {
        return EM_TYPES;
    }

    String[] getLabels() {
        return new String[] {"High Priority PLMN Search"};
    }

    @Override
    void update(int type, Parser data) {
        int value = data.getInt("is_higher_pri_plmn_srch");
        clearData();
        addData(value);
    }
}

class CarrierRSSIServing extends NormalTableComponent {
    private static final int[] EM_TYPES = new int[] {Content.RR_EM_MEASUREMENT_REPORT_INFO};

    public CarrierRSSIServing(Activity context) {
        super(context);
    }

    @Override
    String getName() {
        return "Carrier RSSI (Serving)";
    }

    @Override
    String getGroup() {
        return "2. GSM EM Component";
    }

    @Override
    int[] getEmType() {
        return EM_TYPES;
    }

    String[] getLabels() {
        return new String[] {"BCCH RLA(Dedicated)", "RLA", "Reported", "RX level full",
                             "RX quality sub", "RX quality full"};
    }

    @Override
    void update(int type, Parser data) {
        int rrState = data.getInt("rr_state");
        int rla = data.getInt("serv_rla_in_quarter_dbm");
        int reported = data.getInt("serv_rla_reported_value");
        boolean bcchRlaValid = data.getBoolean("is_serv_BCCH_rla_valid");
        int bcchRla = data.getInt("serv_BCCH_rla_in_dedi_state");
        int full = data.getInt("serv_rla_full_value_in_quater_dbm");
        int rxSub = data.getInt("rxqual_sub");
        int rxFull = data.getInt("rxqual_full");

        clearData();
        if (rrState >= 3 && rrState <= 7) {
            if (bcchRlaValid) {
                if (bcchRla == -1000) {
                    addData("-");
                } else {
                    float value = (float) bcchRla / 4;
                    addData(String.format("%.2f", value));
                }
            } else {
                addData("-");
            }

            if (rla == -1000) {
                addData("-");
            } else {
                float value = (float) rla / 4;
                addData(String.format("%.2f", value));
            }

            if (rla == -1000) {
                addData("-");
            } else {
                addData(reported);
            }

            if (rrState != 6) {
                addData("-");
            } else {
                float value = (float) full / 4;
                addData(String.format("%.2f", value));
            }

            if (rxSub == 0xFF) {
                addData("-");
            } else {
                addData(rxSub);
            }

            if (rxFull == 0xFF) {
                addData("-");
            } else {
                addData(rxFull);
            }
        }
        notifyDataSetChanged();
    }
}

class GSMTasInfo extends NormalTableComponent {
    private static final int[] EM_TYPES = new int[] {Content.EM_GSM_TAS_INFO_IND};

    public GSMTasInfo(Activity context) {
        super(context);
    }

    @Override
    String getName() {
        return "GSM TAS Info";
    }

    @Override
    String getGroup() {
        return "2. GSM EM Component";
    }

    @Override
    int[] getEmType() {
        return EM_TYPES;
    }

    String[] getLabels() {
        return new String[] {"Antenna Index", "Current Antenna Power", "Other Antenna Power"};
    }

    @Override
    void update(int type, Parser data) {
        int antennaIdx = data.getInt("gsm_antenna");
        int currentAntRxLevel = data.getInt("gsm_current_antenna_rxLevel");
        int otherAntRxLevel = data.getInt("gsm_other_antenna_rxLevel");

        clearData();

        addData(antennaIdx);
        addData(currentAntRxLevel);
        addData(otherAntRxLevel);

        notifyDataSetChanged();
    }
}
class GsmServingCellInfo extends NormalTableComponent {
    private static final int[] EM_TYPES = new int[] {Content.RR_EM_MEASUREMENT_REPORT_INFO};
    HashMap<Integer, String> mMapping = new HashMap<Integer, String>() { {
        put(0, "PGSM");
        put(1, "EGSM");
        put(2, "RGSM");
        put(3, "DCS1800");
        put(4, "PCS1900");
        put(5, "GSM450");
        put(6, "GSM480");
        put(7, "GSM850");
    } };

    public GsmServingCellInfo(Activity context) {
        super(context);
    }

    @Override
    String getName() {
        return "GSM Serving Cell Info";
    }

    @Override
    String getGroup() {
        return "2. GSM EM Component";
    }

    @Override
    int[] getEmType() {
        return EM_TYPES;
    }

    String[] getLabels() {
        return new String[] {"Band", "ARFCN", "BSIC"};
    }

    @Override
    void update(int type, Parser data) {
        int rrState = data.getInt("rr_state");
        clearData();
        if (rrState >= 3 && rrState <= 7) {
            String arfcn = mMapping.get(data.getInt("serving_arfcn"));
            int bsic = data.getInt("serving_bsic");
            int band = data.getInt("serving_current_band");
            addData(arfcn == null ? "-" : arfcn);
            addData(bsic);
            addData(band);
        }
        notifyDataSetChanged();
    }
}

class EUtranNeighborCellInfo extends NormalTableComponent {
    private static final int[] EM_TYPES = new int[] {
               Content.RRM_EM_IR_4G_NEIGHBOR_MEAS_STATUS_IND_STRUCT_INFO};
    HashMap<Integer, String> mCellTypeMapping = new HashMap<Integer, String>() { {
        put(0, "Normal");
        put(1, "CSG");
        put(2, "Hybrid");
    } };
    HashMap<Integer, String> mBandTypeMapping = new HashMap<Integer, String>() { {
        put(0, "-");
        put(1, "FDD");
        put(2, "TDD");
    } };
    HashMap<Integer, String> mFailTypeMapping = new HashMap<Integer, String>() { {
        put(0, "None");
        put(1, "Never");
        put(2, "PLMN");
        put(3, "LA");
        put(4, "C_BAR");
        put(5, "TEMP");
        put(6, "CRI3");
        put(7, "TA");
        put(8, "FREQ");
        put(9, "RES_OP");
        put(10, "RES_OP_FRE");
        put(11, "CSG");
        put(12, "GEMINI");
        put(13, "NAS");
    } };

    public EUtranNeighborCellInfo(Activity context) {
        super(context);
    }

    @Override
    String getName() {
        return "E-UTRAN Neighbor Cell Info";
    }

    @Override
    String getGroup() {
        return "2. GSM EM Component";
    }

    @Override
    int[] getEmType() {
        return EM_TYPES;
    }

    String[] getLabels() {
        return new String[] {"Type", "EARFCN", "PCI", "Band", "RSRP", "RSRQ", "Report",
                             "Non-Report", "Barred"};
    }

    @Override
    void update(int type, Parser data) {
        List<Parser> status = data.getArray("ir_4g_neighbor_meas_status");
        clearData();
        for (int i = 0; i < status.size(); i++) {
            data = status.get(i);
            boolean isValid = data.getBoolean("is_valid");
            int cellType = data.getInt("cell_type");
            long earfcn = data.getLong("earfcn");
            int pci = data.getInt("pci");
            int bandType = data.getInt("band_type");
            int rsrp = data.getInt("rsrp");
            int rsrq = data.getInt("rsrq");
            int repValue = data.getInt("rep_value");
            int nonRepValue = data.getInt("non_rep_value");
            int failType = data.getInt("fail_type");
            if (isValid) {
                String cell = mCellTypeMapping.get(cellType);
                String band = mBandTypeMapping.get(bandType);
                String fail = mFailTypeMapping.get(failType);
                addData(cell);
                addData(earfcn);
                addData(pci);
                addData(band);
                addData(String.format("%.2f", (float) rsrp / 4));
                addData(String.format("%.2f", (float) rsrq / 4));
                addData(repValue);
                addData(nonRepValue);
                addData(fail);
            }
        }
        notifyDataSetChanged();
    }
}

class TddServingCellInfo extends ArrayTableComponent {
    private static final int[] EM_TYPES = new int[] {
            Content.EM_TDD_MEME_INFO_DCH_UMTS_CELL_INFO,
            Content.TDD_EM_CSCE_SERV_CELL_IND_STRUCT_INFO};

    public TddServingCellInfo(Activity context) {
        super(context);
    }

    @Override
    String getName() {
        return "TD-SCDMA Serving Cell Info (UMTS TDD)";
    }

    @Override
    String getGroup() {
        return "4. UMTS TDD EM Component";
    }

    @Override
    int[] getEmType() {
        return EM_TYPES;
    }

    String[] getLabels() {
        return new String[] {"UARFCN", "CellParamId", "RSCP"};
    }

    @Override
    void update(int type, Parser data) {
        if (type == Content.EM_TDD_MEME_INFO_DCH_UMTS_CELL_INFO) {
            clearData();
            List<Parser> cellList = data.getArray("umts_cell_list");
            for (int i = 0; i < cellList.size(); i++) {
                data = cellList.get(i);
                int uarfcn = data.getInt("UARFCN");
                int cellParaId = data.getInt("CELLPARAID");
                long rscp = data.getLong("RSCP");
                boolean isServingCell = data.getBoolean("isServingCell");
                if (isServingCell && rscp > -120) {
                    addData(uarfcn, cellParaId, rscp);
                }
            }
        } else if (type == Content.TDD_EM_CSCE_SERV_CELL_IND_STRUCT_INFO) {
            clearData();
            int uarfcn = data.getInt("uarfcn_DL");
            int cellParaId = data.getInt("psc");
            long rscp = data.getLong("rscp");
            addData(uarfcn, cellParaId, (float) rscp / 4096);
        }
    }
}

class LaiRaiUmtsTdd extends NormalTableComponent {
    private static final int[] EM_TYPES = new int[] {Content.MM_EM_INFO};

    public LaiRaiUmtsTdd(Activity context) {
        super(context);
    }

    @Override
    String getName() {
        return "LAI and RAI (UMTS TDD)";
    }

    @Override
    String getGroup() {
        return "4. UMTS TDD EM Component";
    }

    @Override
    int[] getEmType() {
        return EM_TYPES;
    }

    String[] getLabels() {
        return new String[] {"PLMN", "LAI", "RAI"};
    }

    @Override
    void update(int type, Parser data) {
        List<Integer> mcc = data.getIntArray("mcc");
        List<Integer> mnc = data.getIntArray("mnc");
        List<Integer> loc = data.getIntArray("loc");
        int rac = data.getInt("rac");
        String plmn = "";
        String lac = "";
        if (mcc != null && mnc != null) {
            for (int i = 0; i < mcc.size(); i++) {
                plmn += mcc.get(i) + " ";
            }
            for (int i = 0; i < mnc.size(); i++) {
                plmn += mnc.get(i) + " ";
            }
        }
        if (loc != null) {
            for (int i = 0; i < loc.size(); i++) {
                lac += loc.get(i) + " ";
            }
        }
        clearData();
        addData(plmn, lac, rac);
    }
}

class GeranNeighborCellInfoUmtsTdd extends ArrayTableComponent {
    private static final int[] EM_TYPES = new int[] {
            Content.EM_TDD_MEME_INFO_DCH_GSM_CELL_INFO,
            Content.TDD_EM_CSCE_NEIGH_CELL_IND_STRUCT_INFO};
    HashMap<Integer, String> mMapping = new HashMap<Integer, String>() { {
        put(0, "dcs1800");
        put(1, "pcs1900");
    } };

    public GeranNeighborCellInfoUmtsTdd(Activity context) {
        super(context);
    }

    @Override
    String getName() {
        return "GERAN Neighbor Cell Info (UMTS TDD)";
    }

    @Override
    String getGroup() {
        return "4. UMTS TDD EM Component";
    }

    @Override
    int[] getEmType() {
        return EM_TYPES;
    }

    String[] getLabels() {
        return new String[] {"BSIC", "Frequency band", "BCCH ARFCN"};
    }

    @Override
    void update(int type, Parser data) {
        if (type == Content.EM_TDD_MEME_INFO_DCH_GSM_CELL_INFO) {
            int num = data.getInt("num_cells");
            clearData();
            List<Parser> cellList = data.getArray("gsm_cell_list");
            for (int i = 0; i < num && i < cellList.size(); i++) {
                data = cellList.get(i);
                int band = data.getInt("frequency_band");
                int arfcn = data.getInt("arfcn");
                int bsic = data.getInt("bsic");
                int bcc = bsic & 0x7;
                int ncc = (bsic >> 3) & 0x7;
                String bandString = mMapping.get(band);
                addData(new String[] {
                    "bcc: " + bcc + " ncc: " + ncc,
                    bandString == null ? "" : bandString,
                    String.valueOf(arfcn),
                });
            }
        } else if (type == Content.TDD_EM_CSCE_NEIGH_CELL_IND_STRUCT_INFO) {
            int num = data.getInt("neigh_cell_count");
            int rat = data.getInt("RAT_type");
            if (rat == 1) {
                clearData();
                data = data.get("choice");
                List<Parser> cellList = data.getArray("GSM_neigh_cells");
                for (int i = 0; i < num && i < cellList.size(); i++) {
                    data = cellList.get(i);
                    int arfcn = data.getInt("arfcn");
                    int bsic = data.getInt("bsic");
                    int band = data.getInt("freq_band");
                    int bcc = bsic & 0x7;
                    int ncc = (bsic >> 3) & 0x7;
                    String bandString = mMapping.get(band);
                    addData(new String[] {
                        "bcc: " + bcc + " ncc: " + ncc,
                        bandString == null ? "" : bandString,
                        String.valueOf(arfcn),
                    });
                }
            }
        }
    }
}

class EUtranNeighborCellInfoUmtsTdd extends ArrayTableComponent {
    private static final int[] EM_TYPES = new int[] {
            Content.EM_TDD_MEME_INFO_DCH_LTE_CELL_INFO,
            Content.TDD_EM_CSCE_NEIGH_CELL_IND_STRUCT_INFO};
    public EUtranNeighborCellInfoUmtsTdd(Activity context) {
        super(context);
    }

    @Override
    String getName() {
        return "E-UTRAN Neighbor Cell Info (UMTS TDD)";
    }

    @Override
    String getGroup() {
        return "4. UMTS TDD EM Component";
    }

    @Override
    int[] getEmType() {
        return EM_TYPES;
    }

    String[] getLabels() {
        return new String[] {"EARFCN", "PCI", "RSRP", "RSRQ"};
    }

    @Override
    void update(int type, Parser data) {
        if (type == Content.EM_TDD_MEME_INFO_DCH_LTE_CELL_INFO) {
            int num = data.getInt("num_cells");
            clearData();
            List<Parser> cellList = data.getArray("lte_cell_list");
            for (int i = 0; i < num && i < cellList.size(); i++) {
                data = cellList.get(i);
                long arfcn = data.getLong("EARFCN");
                int pci = data.getInt("PCI");
                long rsrp = data.getLong("RSRP");
                long rsrq = data.getLong("RSRQ");
                addData(arfcn, pci, rsrp, rsrq);
            }
        } else if (type == Content.TDD_EM_CSCE_NEIGH_CELL_IND_STRUCT_INFO) {
            int num = data.getInt("neigh_cell_count");
            int rat = data.getInt("RAT_type");
            if (rat == 2) {
                clearData();
                data = data.get("choice");
                List<Parser> cellList = data.getArray("LTE_neigh_cells");
                for (int i = 0; i < num && i < cellList.size(); i++) {
                    data = cellList.get(i);
                    int arfcn = data.getInt("earfcn");
                    int pci = data.getInt("pci");
                    long rsrp = data.getLong("rsrp");
                    long rsrq = data.getLong("rsrq");
                    addData(arfcn, pci, (float) rsrp / 4, (float) rsrq / 4);
                }
            }
        }
    }
}

class LteServingCellInfo extends NormalTableComponent {
    private static final int[] EM_TYPES = new int[] {
            Content.ERRC_EM_MOB_MEAS_INTRARAT_INFO,
            Content.ERRC_EM_CONN_INFO};
    public LteServingCellInfo(Activity context) {
        super(context);
    }

    @Override
    String getName() {
        return "Serving Cell(Primary Cell) Info";
    }

    @Override
    String getGroup() {
        return "5. LTE EM Component";
    }

    @Override
    int[] getEmType() {
        return EM_TYPES;
    }

    String[] getLabels() {
        return new String[] {"PCI", "EARFCN (Band)", "SINR", "RSRP", "RSRQ"};
    }

    @Override
    void update(int type, Parser data) {
        if (type == Content.ERRC_EM_CONN_INFO) {
            long earfcn = data.getLong("pcell_earfcn");
            int band = data.getInt("pcell_band");
            int pci = data.getInt("pcell_pci");
            addDataAtPostion(0, earfcn == 0xFFFFFFFF ? "" : pci);
            addDataAtPostion(1, earfcn == 0xFFFFFFFF ? "" : "EARFCN: " +
                             earfcn + " (Band " + band + ")");
        } else if (type == Content.ERRC_EM_MOB_MEAS_INTRARAT_INFO) {
            data = data.get("serving_info");
            int rsrp = data.getInt("rsrp");
            int rsrq = data.getInt("rsrq");
            int sinr = data.getInt("rs_snr_in_qdb");
            addDataAtPostion(2, sinr == 0xFFFFFFFF ? "" : (float) sinr / 4);
            addDataAtPostion(3, rsrp == 0xFFFFFFFF ? "" : (float) rsrp / 4);
            addDataAtPostion(4, rsrq == 0xFFFFFFFF ? "" : (float) rsrq / 4);
        }
    }
}

class LteSCellInfo extends NormalTableComponent {
    private static final int[] EM_TYPES = new int[] {Content.ERRC_EM_MOB_MEAS_INTRARAT_INFO};
    public LteSCellInfo(Activity context) {
        super(context);
    }

    @Override
    String getName() {
        return "Secondary Cell Info";
    }

    @Override
    String getGroup() {
        return "5. LTE EM Component";
    }

    @Override
    int[] getEmType() {
        return EM_TYPES;
    }

    String[] getLabels() {
        return new String[] {"PCI", "EARFCN (Band)", "SINR", "RSRP", "RSRQ"};
    }

    @Override
    void update(int type, Parser data) {
        data = data.get("scell_info_list");
        List<Parser> scellInfoList = data.getArray("scell_info");
        data = scellInfoList.get(0);
        long earfcn = data.getLong("earfcn");
        int pci = data.getInt("pci");
        int rsrp = data.getInt("rsrp");
        int rsrq = data.getInt("rsrq");
        int sinr = data.getInt("rs_snr_in_qdb");
        int band = data.getInt("serv_lte_band");
        clearData();
        addData(earfcn == 0xFFFFFFFF ? "" : pci);
        addData(earfcn == 0xFFFFFFFF ? "" : "EARFCN: " + earfcn + " (Band " + band + ")");
        addData(rsrp == 0xFFFFFFFF ? "" : (float) rsrp / 4);
        addData(rsrq == 0xFFFFFFFF ? "" : (float) rsrq / 4);
        addData(sinr == 0xFFFFFFFF ? "" : (float) sinr / 4);
    }
}

class PrimaryCellDlBlockErrorRate extends CurveComponent {
    private static final int[] EM_TYPES = new int[] {Content.EM_EL1_INFO};

    public PrimaryCellDlBlockErrorRate(Activity context) {
        super(context);
    }

    @Override
    String getName() {
        return "Primary Cell DL Block Error Rate";
    }

    @Override
    String getGroup() {
        return "5. LTE EM Component";
    }

    @Override
    int[] getEmType() {
        return EM_TYPES;
    }

    @Override
    CurveView.AxisConfig configY() {
//        mYLabel.setText("DL BLER");
        mYLabel.setText("D");

        CurveView.Config[] configs = new CurveView.Config[1];
        configs[0] = new CurveView.Config();
        configs[0].color = 0xFF0000FF;
        configs[0].lineWidth = 2;
        configs[0].nodeType = CurveView.Config.TYPE_CIRCLE;
        configs[0].name = "DL BLER";
        mCurveView.setConfig(configs);

        CurveView.AxisConfig yConfig = new CurveView.AxisConfig();
        yConfig.min = -100;
        yConfig.max = 100;
        yConfig.step = 10;
        yConfig.configMin = true;
        yConfig.configMax = true;
        yConfig.configStep = true;
        return yConfig;
    }

    @Override
    void update(int type, Parser data) {
        List<Parser> dlInfo = data.getArray("dl_info");
        int bler = dlInfo.get(0).getInt("DL_bler");
        addData(0, bler);
    }
}

class PrimaryCellDownlinkBandwidth extends NormalTableComponent {
    private static final int[] EM_TYPES = new int[] {Content.EM_EL1_INFO};

    public PrimaryCellDownlinkBandwidth(Activity context) {
        super(context);
    }

    @Override
    String getName() {
        return "Primary Cell Downlink Bandwidth";
    }

    @Override
    String getGroup() {
        return "5. LTE EM Component";
    }

    @Override
    int[] getEmType() {
        return EM_TYPES;
    }

    String[] getLabels() {
        return new String[] {"DL_BW"};
    }

    @Override
    void update(int type, Parser data) {
        List<Parser> dlInfo = data.getArray("cell_info");
        int bw = dlInfo.get(0).getInt("dl_bw");
        clearData();
        addData(bw);
    }
}

class PrimaryCellUplinkBandwidth extends NormalTableComponent {
    private static final int[] EM_TYPES = new int[] {Content.EM_EL1_INFO};

    public PrimaryCellUplinkBandwidth(Activity context) {
        super(context);
    }

    @Override
    String getName() {
        return "Primary Cell Uplink Bandwidth";
    }

    @Override
    String getGroup() {
        return "5. LTE EM Component";
    }

    @Override
    int[] getEmType() {
        return EM_TYPES;
    }

    String[] getLabels() {
        return new String[] {"UL_BW"};
    }

    @Override
    void update(int type, Parser data) {
        List<Parser> dlInfo = data.getArray("cell_info");
        int bw = dlInfo.get(0).getInt("ul_bw");
        clearData();
        addData(bw);
    }
}

class PrimaryCellUplinkDownlinkConfiguration extends NormalTableComponent {
    private static final int[] EM_TYPES = new int[] {Content.EM_EL1_INFO};

    public PrimaryCellUplinkDownlinkConfiguration(Activity context) {
        super(context);
    }

    @Override
    String getName() {
        return "Primary Cell Uplink-Downlink Configuration";
    }

    @Override
    String getGroup() {
        return "5. LTE EM Component";
    }

    @Override
    int[] getEmType() {
        return EM_TYPES;
    }

    String[] getLabels() {
        return new String[] {"UDC"};
    }

    @Override
    void update(int type, Parser data) {
        List<Parser> dlInfo = data.getArray("cell_info");
        int cfg = dlInfo.get(0).getInt("tdd_cfg");
        clearData();
        addData(cfg);
    }
}

class PrimaryCellSpecialSubframeConfiguration extends NormalTableComponent {
    private static final int[] EM_TYPES = new int[] {Content.EM_EL1_INFO};

    public PrimaryCellSpecialSubframeConfiguration(Activity context) {
        super(context);
    }

    @Override
    String getName() {
        return "Primary Cell Special Subframe Configuration";
    }

    @Override
    String getGroup() {
        return "5. LTE EM Component";
    }

    @Override
    int[] getEmType() {
        return EM_TYPES;
    }

    String[] getLabels() {
        return new String[] {"SSC"};
    }

    @Override
    void update(int type, Parser data) {
        List<Parser> dlInfo = data.getArray("cell_info");
        int value = dlInfo.get(0).getInt("sp_cfg");
        clearData();
        addData(value);
    }
}

class PrimaryCellTransmissionMode extends NormalTableComponent {
    private static final int[] EM_TYPES = new int[] {Content.EM_EL1_INFO};

    public PrimaryCellTransmissionMode(Activity context) {
        super(context);
    }

    @Override
    String getName() {
        return "Primary Cell Transmission Mode";
    }

    @Override
    String getGroup() {
        return "5. LTE EM Component";
    }

    @Override
    int[] getEmType() {
        return EM_TYPES;
    }

    String[] getLabels() {
        return new String[] {"TM"};
    }

    @Override
    void update(int type, Parser data) {
        List<Parser> dlInfo = data.getArray("cell_info");
        int value = dlInfo.get(0).getInt("tm");
        clearData();
        addData(value);
    }
}

class SecondaryCellDlBlockErrorRate extends CurveComponent {
    private static final int[] EM_TYPES = new int[] {Content.EM_EL1_INFO};

    public SecondaryCellDlBlockErrorRate(Activity context) {
        super(context);
    }

    @Override
    String getName() {
        return "Secondary Cell DL Block Error Rate";
    }

    @Override
    String getGroup() {
        return "5. LTE EM Component";
    }

    @Override
    int[] getEmType() {
        return EM_TYPES;
    }

    @Override
    CurveView.AxisConfig configY() {
//        mYLabel.setText("DL BLER");
        mYLabel.setText("D");

        CurveView.Config[] configs = new CurveView.Config[1];
        configs[0] = new CurveView.Config();
        configs[0].color = 0xFF0000FF;
        configs[0].lineWidth = 2;
        configs[0].nodeType = CurveView.Config.TYPE_CIRCLE;
        configs[0].name = "UL BLER";
        mCurveView.setConfig(configs);

        CurveView.AxisConfig yConfig = new CurveView.AxisConfig();
        yConfig.min = -100;
        yConfig.max = 100;
        yConfig.step = 10;
        yConfig.configMin = true;
        yConfig.configMax = true;
        yConfig.configStep = true;
        return yConfig;
    }

    @Override
    void update(int type, Parser data) {
        List<Parser> dlInfo = data.getArray("dl_info");
        int bler = dlInfo.get(1).getInt("DL_bler");
        addData(0, bler);
    }
}

class SecondaryCellDownlinkBandwidth extends NormalTableComponent {
    private static final int[] EM_TYPES = new int[] {Content.EM_EL1_INFO};

    public SecondaryCellDownlinkBandwidth(Activity context) {
        super(context);
    }

    @Override
    String getName() {
        return "Secondary Cell Downlink Bandwidth";
    }

    @Override
    String getGroup() {
        return "5. LTE EM Component";
    }

    @Override
    int[] getEmType() {
        return EM_TYPES;
    }

    String[] getLabels() {
        return new String[] {"DL_BW"};
    }

    @Override
    void update(int type, Parser data) {
        List<Parser> dlInfo = data.getArray("cell_info");
        int bw = dlInfo.get(1).getInt("dl_bw");
        clearData();
        addData(bw);
    }
}

class SecondaryCellUplinkBandwidth extends NormalTableComponent {
    private static final int[] EM_TYPES = new int[] {Content.EM_EL1_INFO};

    public SecondaryCellUplinkBandwidth(Activity context) {
        super(context);
    }

    @Override
    String getName() {
        return "Secondary Cell Uplink Bandwidth";
    }

    @Override
    String getGroup() {
        return "5. LTE EM Component";
    }

    @Override
    int[] getEmType() {
        return EM_TYPES;
    }

    String[] getLabels() {
        return new String[] {"UL_BW"};
    }

    @Override
    void update(int type, Parser data) {
        List<Parser> dlInfo = data.getArray("cell_info");
        int bw = dlInfo.get(1).getInt("ul_bw");
        clearData();
        addData(bw);
    }
}

class SecondaryCellUplinkDownlinkConfiguration extends NormalTableComponent {
    private static final int[] EM_TYPES = new int[] {Content.EM_EL1_INFO};

    public SecondaryCellUplinkDownlinkConfiguration(Activity context) {
        super(context);
    }

    @Override
    String getName() {
        return "Secondary Cell Uplink-Downlink Configuration";
    }

    @Override
    String getGroup() {
        return "5. LTE EM Component";
    }

    @Override
    int[] getEmType() {
        return EM_TYPES;
    }

    String[] getLabels() {
        return new String[] {"UDC"};
    }

    @Override
    void update(int type, Parser data) {
        List<Parser> dlInfo = data.getArray("cell_info");
        int value = dlInfo.get(1).getInt("tdd_cfg");
        clearData();
        addData(value);
    }
}

class SecondaryCellSpecialSubframeConfiguration extends NormalTableComponent {
    private static final int[] EM_TYPES = new int[] {Content.EM_EL1_INFO};

    public SecondaryCellSpecialSubframeConfiguration(Activity context) {
        super(context);
    }

    @Override
    String getName() {
        return "Secondary Cell Special Subframe Configuration";
    }

    @Override
    String getGroup() {
        return "5. LTE EM Component";
    }

    @Override
    int[] getEmType() {
        return EM_TYPES;
    }

    String[] getLabels() {
        return new String[] {"SSC"};
    }

    @Override
    void update(int type, Parser data) {
        List<Parser> dlInfo = data.getArray("cell_info");
        int value = dlInfo.get(1).getInt("sp_cfg");
        clearData();
        addData(value);
    }
}

class SecondaryCellTransmissionMode extends NormalTableComponent {
    private static final int[] EM_TYPES = new int[] {Content.EM_EL1_INFO};

    public SecondaryCellTransmissionMode(Activity context) {
        super(context);
    }

    @Override
    String getName() {
        return "Secondary Cell Transmission Mode";
    }

    @Override
    String getGroup() {
        return "5. LTE EM Component";
    }

    @Override
    int[] getEmType() {
        return EM_TYPES;
    }

    String[] getLabels() {
        return new String[] {"TM"};
    }

    @Override
    void update(int type, Parser data) {
        List<Parser> dlInfo = data.getArray("cell_info");
        int value = dlInfo.get(1).getInt("tm");
        clearData();
        addData(value);
    }
}

class Tai extends NormalTableComponent {
    private static final int[] EM_TYPES = new int[] {Content.EMM_L4C_EMM_INFO};

    public Tai(Activity context) {
        super(context);
    }

    @Override
    String getName() {
        return "TAI";
    }

    @Override
    String getGroup() {
        return "5. LTE EM Component";
    }

    @Override
    int[] getEmType() {
        return EM_TYPES;
    }

    String[] getLabels() {
        return new String[] {"PLMN", "TAC"};
    }

    @Override
    void update(int type, Parser data) {
        data = data.get("emm_plmnsel_para");
        Parser plmn = data.get("selected_plmn");
        int mcc1 = plmn.getInt("mcc1");
        int mcc2 = plmn.getInt("mcc2");
        int mcc3 = plmn.getInt("mcc3");
        int mnc1 = plmn.getInt("mcc1");
        int mnc2 = plmn.getInt("mnc2");
        int mnc3 = plmn.getInt("mnc3");
        int tac = data.getInt("tac");
//        List<Parser> dlInfo = data.getArray("cell_info");
//        int value = dlInfo.get(1).getInt("tm");
        clearData();
        if (mcc1 == 0xFF && mcc2 == 0xFF && mcc3 == 0xFF && mnc1 == 0xFF &&
            mnc2 == 0xFF && mnc3 == 0xFF) {
            addData("-");
        } else {
            addData("" + mcc1 + mcc2 + mcc3 + mnc1 + mnc2 + (mnc3 == 0xFF ? "" : mnc3));
        }
        if (tac == 0xFFFE || tac == 0) {
            addData("-");
        } else {
            addData(String.format("0x%X", tac));
        }
    }
}

class EmIrReport extends ArrayTableComponent {
    private static final int[] EM_TYPES = new int[] {
            Content.RRM_EM_SUCCESS_RATE_KPI_INFO,
            Content.EM_RRCE_KPI_STATUS_IND,
            Content.ERRC_EM_ERRC_KPI_INFO};

    private static final int EM_ERRC_SUCCESS_RATE_KPI_INTRA_LTE_CR = 0;
    private static final int EM_ERRC_SUCCESS_RATE_KPI_IRAT_4G3_CR = 1;
    private static final int EM_ERRC_SUCCESS_RATE_KPI_IRAT_4G2_CR = 2;
    private static final int EM_ERRC_SUCCESS_RATE_KPI_IRAT_3G4_CR = 3;
    private static final int EM_ERRC_SUCCESS_RATE_KPI_IRAT_2G4_CR = 4;

    private static final int EM_ERRC_SUCCESS_RATE_KPI_INTRA_CELL_HO = 5;
    private static final int EM_ERRC_SUCCESS_RATE_KPI_INTER_CELL_HO = 6;
    private static final int EM_ERRC_SUCCESS_RATE_KPI_IRAT_4G3_HO = 7;
    private static final int EM_ERRC_SUCCESS_RATE_KPI_IRAT_4G2_HO = 8;
    private static final int EM_ERRC_SUCCESS_RATE_KPI_IRAT_3G4_HO = 9;

    private static final int EM_ERRC_SUCCESS_RATE_KPI_LTE_FDD_TDD_CELL_HO = 10;
    private static final int EM_ERRC_SUCCESS_RATE_KPI_LTE_TDD_FDD_CELL_HO = 11;
    private static final int EM_ERRC_SUCCESS_RATE_KPI_IRAT_4GTDD_3G_HO = 12;
    private static final int EM_ERRC_SUCCESS_RATE_KPI_IRAT_4GFDD_3G_HO = 13;
    private static final int EM_ERRC_SUCCESS_RATE_KPI_IRAT_4GTDD_2G_HO = 14;
    private static final int EM_ERRC_SUCCESS_RATE_KPI_IRAT_4GFDD_2G_HO = 15;
    private static final int EM_ERRC_SUCCESS_RATE_KPI_IRAT_3G_4GFDD_HO = 16;
    private static final int EM_ERRC_SUCCESS_RATE_KPI_IRAT_3G_4GTDD_HO = 17;

    private static final int EM_ERRC_SUCCESS_RATE_KPI_INTRA_LTE_FDDTDD_CR = 18;
    private static final int EM_ERRC_SUCCESS_RATE_KPI_INTRA_LTE_TDDFDD_CR = 19;
    private static final int EM_ERRC_SUCCESS_RATE_KPI_IRAT_FDD4G3_CR = 20;
    private static final int EM_ERRC_SUCCESS_RATE_KPI_IRAT_TDD4G3_CR = 21;
    private static final int EM_ERRC_SUCCESS_RATE_KPI_IRAT_FDD4G2_CR = 22;
    private static final int EM_ERRC_SUCCESS_RATE_KPI_IRAT_TDD4G2_CR = 23;
    private static final int EM_ERRC_SUCCESS_RATE_KPI_IRAT_FDD3G4_CR = 24;
    private static final int EM_ERRC_SUCCESS_RATE_KPI_IRAT_TDD3G4_CR = 25;
    private static final int EM_ERRC_SUCCESS_RATE_KPI_IRAT_FDD2G4_CR = 26;
    private static final int EM_ERRC_SUCCESS_RATE_KPI_IRAT_TDD2G4_CR = 27;

    private static final int EM_ERRC_SUCCESS_RATE_KPI_INTRA_LTE_FDDTDD_REDIRECT = 28;
    private static final int EM_ERRC_SUCCESS_RATE_KPI_INTRA_LTE_TDDFDD_REDIRECT = 29;
    private static final int EM_ERRC_SUCCESS_RATE_KPI_IRAT_FDD4G3_R8_REDIRECT = 30;
    private static final int EM_ERRC_SUCCESS_RATE_KPI_IRAT_FDD4G3_R9_REDIRECT = 31;
    private static final int EM_ERRC_SUCCESS_RATE_KPI_IRAT_FDD4G3_R10_REDIRECT = 32;
    private static final int EM_ERRC_SUCCESS_RATE_KPI_IRAT_TDD4G3_R8_REDIRECT = 33;
    private static final int EM_ERRC_SUCCESS_RATE_KPI_IRAT_TDD4G3_R9_REDIRECT = 34;
    private static final int EM_ERRC_SUCCESS_RATE_KPI_IRAT_TDD4G3_R10_REDIRECT = 35;
    private static final int EM_ERRC_SUCCESS_RATE_KPI_IRAT_FDD4G2_R8_REDIRECT = 36;
    private static final int EM_ERRC_SUCCESS_RATE_KPI_IRAT_FDD4G2_R9_REDIRECT = 37;
    private static final int EM_ERRC_SUCCESS_RATE_KPI_IRAT_FDD4G2_R10_REDIRECT = 38;
    private static final int EM_ERRC_SUCCESS_RATE_KPI_IRAT_TDD4G2_R8_REDIRECT = 39;
    private static final int EM_ERRC_SUCCESS_RATE_KPI_IRAT_TDD4G2_R9_REDIRECT = 40;
    private static final int EM_ERRC_SUCCESS_RATE_KPI_IRAT_TDD4G2_R10_REDIRECT = 41;
    private static final int EM_ERRC_SUCCESS_RATE_KPI_IRAT_FDD3G4_REDIRECT = 42;
    private static final int EM_ERRC_SUCCESS_RATE_KPI_IRAT_TDD3G4_REDIRECT = 43;
    private static final int EM_ERRC_SUCCESS_RATE_KPI_IRAT_FDD2G4_REDIRECT = 44;
    private static final int EM_ERRC_SUCCESS_RATE_KPI_IRAT_TDD2G4_REDIRECT = 45;

    private static final int EM_ERRC_SUCCESS_RATE_KPI_IRAT_FDD3G4_AFR = 46;
    private static final int EM_ERRC_SUCCESS_RATE_KPI_IRAT_TDD3G4_AFR = 47;
    private static final int EM_ERRC_SUCCESS_RATE_KPI_IRAT_FDD2G4_AFR = 48;
    private static final int EM_ERRC_SUCCESS_RATE_KPI_IRAT_TDD2G4_AFR = 49;

    private static final int EM_ERRC_SUCCESS_RATE_KPI_IRAT_FDD3G4_BG_SRCH = 50;
    private static final int EM_ERRC_SUCCESS_RATE_KPI_IRAT_TDD3G4_BG_SRCH = 51;
    private static final int EM_ERRC_SUCCESS_RATE_KPI_IRAT_FDD2G4_BG_SRCH = 52;
    private static final int EM_ERRC_SUCCESS_RATE_KPI_IRAT_TDD2G4_BG_SRCH = 53;

    HashMap<Integer, String> mRrmDirectMapping = new HashMap<Integer, String>() { {
        put(0, "2G3");
        put(1, "2G3");
        put(2, "2G3");
        put(3, "2G3");
    } };
    HashMap<Integer, String> mRrmTypeMapping = new HashMap<Integer, String>() { {
        put(0, "Cell Reselection");
        put(1, "CCO");
        put(2, "Handover");
        put(3, "Redirection");
    } };
    HashMap<Integer, String> mRrceDirectMapping = new HashMap<Integer, String>() { {
        put(0, "3G2");
        put(1, "3G2");
        put(2, "3G2");
        put(3, "3G2");
        put(4, "3G2");
        put(5, "3G2");
    } };
    HashMap<Integer, String> mRrceTypeMapping = new HashMap<Integer, String>() { {
        put(0, "Handover");
        put(1, "Cell Reselection");
        put(2, "Cell Reselection");
        put(3, "CCO");
        put(4, "Redirection");
        put(5, "Redirection");
    } };
    HashMap<Integer, String> mErrcDirectMapping = new HashMap<Integer, String>() { {
        put(EM_ERRC_SUCCESS_RATE_KPI_INTRA_LTE_CR, "");
        put(EM_ERRC_SUCCESS_RATE_KPI_IRAT_4G3_CR, "");
        put(EM_ERRC_SUCCESS_RATE_KPI_IRAT_4G2_CR, "");
        put(EM_ERRC_SUCCESS_RATE_KPI_IRAT_3G4_CR, "");
        put(EM_ERRC_SUCCESS_RATE_KPI_IRAT_2G4_CR, "");
        put(EM_ERRC_SUCCESS_RATE_KPI_INTRA_CELL_HO, "");
        put(EM_ERRC_SUCCESS_RATE_KPI_INTER_CELL_HO, "");
        put(EM_ERRC_SUCCESS_RATE_KPI_IRAT_4G3_HO, "");
        put(EM_ERRC_SUCCESS_RATE_KPI_IRAT_4G2_HO, "");
        put(EM_ERRC_SUCCESS_RATE_KPI_IRAT_3G4_HO, "");
        put(EM_ERRC_SUCCESS_RATE_KPI_LTE_FDD_TDD_CELL_HO, "");
        put(EM_ERRC_SUCCESS_RATE_KPI_LTE_TDD_FDD_CELL_HO, "");
        put(EM_ERRC_SUCCESS_RATE_KPI_IRAT_4GTDD_3G_HO, "4GTDD to 3G");
        put(EM_ERRC_SUCCESS_RATE_KPI_IRAT_4GFDD_3G_HO, "4GFDD to 3G");
        put(EM_ERRC_SUCCESS_RATE_KPI_IRAT_4GTDD_2G_HO, "4GTDD to 2G");
        put(EM_ERRC_SUCCESS_RATE_KPI_IRAT_4GFDD_2G_HO, "4GFDD to 2G");
        put(EM_ERRC_SUCCESS_RATE_KPI_IRAT_3G_4GFDD_HO, "3G to 4GFDD");
        put(EM_ERRC_SUCCESS_RATE_KPI_IRAT_3G_4GTDD_HO, "3G to 4GTDD");
        put(EM_ERRC_SUCCESS_RATE_KPI_INTRA_LTE_FDDTDD_CR, "");
        put(EM_ERRC_SUCCESS_RATE_KPI_INTRA_LTE_TDDFDD_CR, "");
        put(EM_ERRC_SUCCESS_RATE_KPI_IRAT_FDD4G3_CR, "4GFDD to 3G");
        put(EM_ERRC_SUCCESS_RATE_KPI_IRAT_TDD4G3_CR, "4GTDD to 3G");
        put(EM_ERRC_SUCCESS_RATE_KPI_IRAT_FDD4G2_CR, "4GFDD to 2G");
        put(EM_ERRC_SUCCESS_RATE_KPI_IRAT_TDD4G2_CR, "4GTDD to 2G");
        put(EM_ERRC_SUCCESS_RATE_KPI_IRAT_FDD3G4_CR, "3G to 4GFDD");
        put(EM_ERRC_SUCCESS_RATE_KPI_IRAT_TDD3G4_CR, "3G to 4GTDD");
        put(EM_ERRC_SUCCESS_RATE_KPI_IRAT_FDD2G4_CR, "2G to 4GFDD");
        put(EM_ERRC_SUCCESS_RATE_KPI_IRAT_TDD2G4_CR, "2G to 4GTDD");
        put(EM_ERRC_SUCCESS_RATE_KPI_INTRA_LTE_FDDTDD_REDIRECT, "");
        put(EM_ERRC_SUCCESS_RATE_KPI_INTRA_LTE_TDDFDD_REDIRECT, "");
        put(EM_ERRC_SUCCESS_RATE_KPI_IRAT_FDD4G3_R8_REDIRECT, "4GFDD to 3G");
        put(EM_ERRC_SUCCESS_RATE_KPI_IRAT_FDD4G3_R9_REDIRECT, "4GFDD to 3G");
        put(EM_ERRC_SUCCESS_RATE_KPI_IRAT_FDD4G3_R10_REDIRECT, "4GFDD to 3G");
        put(EM_ERRC_SUCCESS_RATE_KPI_IRAT_TDD4G3_R8_REDIRECT, "4GTDD to 3G");
        put(EM_ERRC_SUCCESS_RATE_KPI_IRAT_TDD4G3_R9_REDIRECT, "4GTDD to 3G");
        put(EM_ERRC_SUCCESS_RATE_KPI_IRAT_TDD4G3_R10_REDIRECT, "4GTDD to 3Gf");
        put(EM_ERRC_SUCCESS_RATE_KPI_IRAT_FDD4G2_R8_REDIRECT, "4GFDD to 2G");
        put(EM_ERRC_SUCCESS_RATE_KPI_IRAT_FDD4G2_R9_REDIRECT, "4GFDD to 2G");
        put(EM_ERRC_SUCCESS_RATE_KPI_IRAT_FDD4G2_R10_REDIRECT, "4GFDD to 2G");
        put(EM_ERRC_SUCCESS_RATE_KPI_IRAT_TDD4G2_R8_REDIRECT, "4GTDD to 2G");
        put(EM_ERRC_SUCCESS_RATE_KPI_IRAT_TDD4G2_R9_REDIRECT, "4GTDD to 2G");
        put(EM_ERRC_SUCCESS_RATE_KPI_IRAT_TDD4G2_R10_REDIRECT, "4GTDD to 2G");
        put(EM_ERRC_SUCCESS_RATE_KPI_IRAT_FDD3G4_REDIRECT, "3G to 4GFDD");
        put(EM_ERRC_SUCCESS_RATE_KPI_IRAT_TDD3G4_REDIRECT, "3G to 4GTDD");
        put(EM_ERRC_SUCCESS_RATE_KPI_IRAT_FDD2G4_REDIRECT, "2G to 4GFDD");
        put(EM_ERRC_SUCCESS_RATE_KPI_IRAT_TDD2G4_REDIRECT, "2G to 4GTDD");
        put(EM_ERRC_SUCCESS_RATE_KPI_IRAT_FDD3G4_AFR, "3G to 4GFDD");
        put(EM_ERRC_SUCCESS_RATE_KPI_IRAT_TDD3G4_AFR, "3G to 4GTDD");
        put(EM_ERRC_SUCCESS_RATE_KPI_IRAT_FDD2G4_AFR, "2G to 4GFDD");
        put(EM_ERRC_SUCCESS_RATE_KPI_IRAT_TDD2G4_AFR, "2G to 4GTDD");
        put(EM_ERRC_SUCCESS_RATE_KPI_IRAT_FDD3G4_BG_SRCH, "3G to 4GFDD");
        put(EM_ERRC_SUCCESS_RATE_KPI_IRAT_TDD3G4_BG_SRCH, "3G to 4GTDD");
        put(EM_ERRC_SUCCESS_RATE_KPI_IRAT_FDD2G4_BG_SRCH, "2G to 4GFDD");
        put(EM_ERRC_SUCCESS_RATE_KPI_IRAT_TDD2G4_BG_SRCH, "2G to 4GTDD");
    } };
    HashMap<Integer, String> mErrcTypeMapping = new HashMap<Integer, String>() { {
        put(EM_ERRC_SUCCESS_RATE_KPI_INTRA_LTE_CR, "");
        put(EM_ERRC_SUCCESS_RATE_KPI_IRAT_4G3_CR, "");
        put(EM_ERRC_SUCCESS_RATE_KPI_IRAT_4G2_CR, "");
        put(EM_ERRC_SUCCESS_RATE_KPI_IRAT_3G4_CR, "");
        put(EM_ERRC_SUCCESS_RATE_KPI_IRAT_2G4_CR, "");
        put(EM_ERRC_SUCCESS_RATE_KPI_INTRA_CELL_HO, "");
        put(EM_ERRC_SUCCESS_RATE_KPI_INTER_CELL_HO, "");
        put(EM_ERRC_SUCCESS_RATE_KPI_IRAT_4G3_HO, "");
        put(EM_ERRC_SUCCESS_RATE_KPI_IRAT_4G2_HO, "");
        put(EM_ERRC_SUCCESS_RATE_KPI_IRAT_3G4_HO, "");
        put(EM_ERRC_SUCCESS_RATE_KPI_LTE_FDD_TDD_CELL_HO, "");
        put(EM_ERRC_SUCCESS_RATE_KPI_LTE_TDD_FDD_CELL_HO, "");
        put(EM_ERRC_SUCCESS_RATE_KPI_IRAT_4GTDD_3G_HO, "Handover");
        put(EM_ERRC_SUCCESS_RATE_KPI_IRAT_4GFDD_3G_HO, "Handover");
        put(EM_ERRC_SUCCESS_RATE_KPI_IRAT_4GTDD_2G_HO, "Handover");
        put(EM_ERRC_SUCCESS_RATE_KPI_IRAT_4GFDD_2G_HO, "Handover");
        put(EM_ERRC_SUCCESS_RATE_KPI_IRAT_3G_4GFDD_HO, "Handover");
        put(EM_ERRC_SUCCESS_RATE_KPI_IRAT_3G_4GTDD_HO, "Handover");
        put(EM_ERRC_SUCCESS_RATE_KPI_INTRA_LTE_FDDTDD_CR, "");
        put(EM_ERRC_SUCCESS_RATE_KPI_INTRA_LTE_TDDFDD_CR, "");
        put(EM_ERRC_SUCCESS_RATE_KPI_IRAT_FDD4G3_CR, "Cell Reselection");
        put(EM_ERRC_SUCCESS_RATE_KPI_IRAT_TDD4G3_CR, "Cell Reselection");
        put(EM_ERRC_SUCCESS_RATE_KPI_IRAT_FDD4G2_CR, "Cell Reselection");
        put(EM_ERRC_SUCCESS_RATE_KPI_IRAT_TDD4G2_CR, "Cell Reselection");
        put(EM_ERRC_SUCCESS_RATE_KPI_IRAT_FDD3G4_CR, "Cell Reselection");
        put(EM_ERRC_SUCCESS_RATE_KPI_IRAT_TDD3G4_CR, "Cell Reselection");
        put(EM_ERRC_SUCCESS_RATE_KPI_IRAT_FDD2G4_CR, "Cell Reselection");
        put(EM_ERRC_SUCCESS_RATE_KPI_IRAT_TDD2G4_CR, "Cell Reselection");
        put(EM_ERRC_SUCCESS_RATE_KPI_INTRA_LTE_FDDTDD_REDIRECT, "");
        put(EM_ERRC_SUCCESS_RATE_KPI_INTRA_LTE_TDDFDD_REDIRECT, "");
        put(EM_ERRC_SUCCESS_RATE_KPI_IRAT_FDD4G3_R8_REDIRECT, "Rediretion Rel8");
        put(EM_ERRC_SUCCESS_RATE_KPI_IRAT_FDD4G3_R9_REDIRECT, "Rediretion Rel9");
        put(EM_ERRC_SUCCESS_RATE_KPI_IRAT_FDD4G3_R10_REDIRECT, "Rediretion Rel10");
        put(EM_ERRC_SUCCESS_RATE_KPI_IRAT_TDD4G3_R8_REDIRECT, "Rediretion Rel8");
        put(EM_ERRC_SUCCESS_RATE_KPI_IRAT_TDD4G3_R9_REDIRECT, "Rediretion Rel9");
        put(EM_ERRC_SUCCESS_RATE_KPI_IRAT_TDD4G3_R10_REDIRECT, "Rediretion Rel10");
        put(EM_ERRC_SUCCESS_RATE_KPI_IRAT_FDD4G2_R8_REDIRECT, "Rediretion Rel8");
        put(EM_ERRC_SUCCESS_RATE_KPI_IRAT_FDD4G2_R9_REDIRECT, "Rediretion Rel9");
        put(EM_ERRC_SUCCESS_RATE_KPI_IRAT_FDD4G2_R10_REDIRECT, "Rediretion Rel10");
        put(EM_ERRC_SUCCESS_RATE_KPI_IRAT_TDD4G2_R8_REDIRECT, "Rediretion Rel8");
        put(EM_ERRC_SUCCESS_RATE_KPI_IRAT_TDD4G2_R9_REDIRECT, "Rediretion Rel9");
        put(EM_ERRC_SUCCESS_RATE_KPI_IRAT_TDD4G2_R10_REDIRECT, "Rediretion Rel10");
        put(EM_ERRC_SUCCESS_RATE_KPI_IRAT_FDD3G4_REDIRECT, "Rediretion");
        put(EM_ERRC_SUCCESS_RATE_KPI_IRAT_TDD3G4_REDIRECT, "Rediretion");
        put(EM_ERRC_SUCCESS_RATE_KPI_IRAT_FDD2G4_REDIRECT, "Rediretion");
        put(EM_ERRC_SUCCESS_RATE_KPI_IRAT_TDD2G4_REDIRECT, "Rediretion");
        put(EM_ERRC_SUCCESS_RATE_KPI_IRAT_FDD3G4_AFR, "Auto Fast Return");
        put(EM_ERRC_SUCCESS_RATE_KPI_IRAT_TDD3G4_AFR, "Auto Fast Return");
        put(EM_ERRC_SUCCESS_RATE_KPI_IRAT_FDD2G4_AFR, "Auto Fast Return");
        put(EM_ERRC_SUCCESS_RATE_KPI_IRAT_TDD2G4_AFR, "Auto Fast Return");
        put(EM_ERRC_SUCCESS_RATE_KPI_IRAT_FDD3G4_BG_SRCH, "Background Search");
        put(EM_ERRC_SUCCESS_RATE_KPI_IRAT_TDD3G4_BG_SRCH, "Background Search");
        put(EM_ERRC_SUCCESS_RATE_KPI_IRAT_FDD2G4_BG_SRCH, "Background Search");
        put(EM_ERRC_SUCCESS_RATE_KPI_IRAT_TDD2G4_BG_SRCH, "Background Search");
    } };
    HashMap<Integer, String> mStatusMapping = new HashMap<Integer, String>() { {
        put(0, "Ongoing");
        put(1, "Success");
        put(2, "Failure");
    } };

    public EmIrReport(Activity context) {
        super(context);
    }

    @Override
    String getName() {
        return "EM IR Report";
    }

    @Override
    String getGroup() {
        return "6. Inter-RAT EM Component";
    }

    @Override
    int[] getEmType() {
        return EM_TYPES;
    }

    String[] getLabels() {
        return new String[] {"IR Direction", "IR Type", "Status"};
    }

    @Override
    void update(int type, Parser data) {
        if (type == Content.RRM_EM_SUCCESS_RATE_KPI_INFO) {
            int procId = data.getInt("proc_id");
            int status = data.getInt("status");
            String direction = mRrmDirectMapping.get(procId);
            String procType = mRrmTypeMapping.get(procId);
            clearData();
            addData(new String[] {direction, procType, mStatusMapping.get(status)});
        } else if (type == Content.EM_RRCE_KPI_STATUS_IND) {
            int procId = data.getInt("proc_id");
            int status = data.getInt("status");
            String direction = mRrmDirectMapping.get(procId);
            String procType = mRrmTypeMapping.get(procId);
            clearData();
            addData(new String[] {direction, procType, mStatusMapping.get(status)});
        } else if (type == Content.ERRC_EM_ERRC_KPI_INFO) {
            int procId = data.getInt("proc_id");
            int status = data.getInt("status");
            String direction = mRrmDirectMapping.get(procId);
            String procType = mRrmTypeMapping.get(procId);
            clearData();
            addData(new String[] {direction, procType, mStatusMapping.get(status)});
        }
    }
}

class UmtsRrcMeasurementControlForE3a extends NormalTableComponent {
    private static final int[] EM_TYPES = new int[] {
             Content.EM_TDD_MEME_INFO_EVENT_TYPE_3_PARAMETER_STRUCT_INFO};

    HashMap<Integer, String> mQtyMapping = new HashMap<Integer, String>() { {
        put(0, "-");
        put(1, "UMTS RSCP");
        put(2, "UMTS ECN0");
        put(3, "UMTS PATHLOSS");
    } };
    HashMap<Integer, String> mQtyOtherRatMapping = new HashMap<Integer, String>() { {
        put(0, "-");
        put(1, "GSM RSSI");
        put(2, "LTE RSRP");
        put(3, "LTE RSRQ");
    } };

    public UmtsRrcMeasurementControlForE3a(Activity context) {
        super(context);
    }

    @Override
    String getName() {
        return "UMTS RRC Measurement control for e3a (UMTS TDD)";
    }

    @Override
    String getGroup() {
        return "4. UMTS TDD EM Component";
    }

    @Override
    int[] getEmType() {
        return EM_TYPES;
    }

    String[] getLabels() {
        return new String[] {"e3a meas ID", "UMTS RAT meas quantity", "Other RAT meas quantity",
                             "UMTS RAT threshold", "Other RAT threshold"};
    }

    @Override
    void update(int type, Parser data) {
        int num = data.getInt("num_event_info");
        List<Parser> info = data.getArray("em_event_info");
        clearData();
        for (int i = 0; i < num && i < info.size(); i++) {
            data = info.get(i);
            int measId = data.getInt("MeasId");
            int eventType = data.getInt("EventType");
            int measQty = data.getInt("MeasQty");
            int measQtyOtherRAT = data.getInt("MeasQtyOtherRAT");
            int thresholdOwnSystem = data.getInt("ThresholdOwnSystem");
            int thresholdOtherSystem = data.getInt("ThresholdOtherSystem");
            if (eventType == 1) {
                if (measId == 0) {
                    addData("-");
                } else {
                    addData(measId);
                }
                addData(mQtyMapping.get(measQty));
                addData(mQtyOtherRatMapping.get(measQtyOtherRAT));
                if (thresholdOwnSystem == 0xFFFF) {
                    addData("-");
                } else {
                    addData((float) thresholdOwnSystem / 4);
                }
                if (thresholdOtherSystem == 0xFFFF) {
                    addData("-");
                } else {
                    addData((float) thresholdOtherSystem / 4);
                }
            }
        }
        notifyDataSetChanged();
    }
}

class UmtsRrcMeasurementControlForE3b extends NormalTableComponent {
    private static final int[] EM_TYPES = new int[] {Content.EM_TDD_MEME_INFO_DCH_3G_BLER_INFO};

    HashMap<Integer, String> mQtyMapping = new HashMap<Integer, String>() { {
        put(0, "-");
        put(1, "UMTS RSCP");
        put(2, "UMTS ECN0");
        put(3, "UMTS PATHLOSS");
    } };
    HashMap<Integer, String> mQtyOtherRatMapping = new HashMap<Integer, String>() { {
        put(0, "-");
        put(1, "GSM RSSI");
        put(2, "LTE RSRP");
        put(3, "LTE RSRQ");
    } };

    public UmtsRrcMeasurementControlForE3b(Activity context) {
        super(context);
    }

    @Override
    String getName() {
        return "UMTS RRC Measurement control for e3b (UMTS TDD)";
    }

    @Override
    String getGroup() {
        return "4. UMTS TDD EM Component";
    }

    @Override
    int[] getEmType() {
        return EM_TYPES;
    }

    String[] getLabels() {
        return new String[] {"e3b meas ID", "UMTS RAT meas quantity", "Other RAT meas quantity",
                             "UMTS RAT threshold", "Other RAT threshold"};
    }

    @Override
    void update(int type, Parser data) {
        int num = data.getInt("num_event_info");
        List<Parser> info = data.getArray("em_event_info");
        clearData();
        for (int i = 0; i < num && i < info.size(); i++) {
            data = info.get(i);
            int measId = data.getInt("MeasId");
            int eventType = data.getInt("EventType");
            int measQty = data.getInt("MeasQty");
            int measQtyOtherRAT = data.getInt("MeasQtyOtherRAT");
            int thresholdOwnSystem = data.getInt("ThresholdOwnSystem");
            int thresholdOtherSystem = data.getInt("ThresholdOtherSystem");
            if (eventType == 2) {
                if (measId == 0) {
                    addData("-");
                } else {
                    addData(measId);
                }
                addData(mQtyMapping.get(measQty));
                addData(mQtyOtherRatMapping.get(measQtyOtherRAT));
                if (thresholdOwnSystem == 0xFFFF) {
                    addData("-");
                } else {
                    addData((float) thresholdOwnSystem / 4);
                }
                if (thresholdOtherSystem == 0xFFFF) {
                    addData("-");
                } else {
                    addData((float) thresholdOtherSystem / 4);
                }
            }
        }
        notifyDataSetChanged();
    }
}

class UmtsRrcMeasurementControlForE3c extends NormalTableComponent {
    private static final int[] EM_TYPES = new int[] {Content.EM_TDD_MEME_INFO_GSM_CELL_INFO};

    HashMap<Integer, String> mQtyMapping = new HashMap<Integer, String>() { {
        put(0, "-");
        put(1, "UMTS RSCP");
        put(2, "UMTS ECN0");
        put(3, "UMTS PATHLOSS");
    } };
    HashMap<Integer, String> mQtyOtherRatMapping = new HashMap<Integer, String>() { {
        put(0, "-");
        put(1, "GSM RSSI");
        put(2, "LTE RSRP");
        put(3, "LTE RSRQ");
    } };

    public UmtsRrcMeasurementControlForE3c(Activity context) {
        super(context);
    }

    @Override
    String getName() {
        return "UMTS RRC Measurement control for e3c (UMTS TDD)";
    }

    @Override
    String getGroup() {
        return "4. UMTS TDD EM Component";
    }

    @Override
    int[] getEmType() {
        return EM_TYPES;
    }

    String[] getLabels() {
        return new String[] {"e3c meas ID", "UMTS RAT meas quantity", "Other RAT meas quantity",
                             "UMTS RAT threshold", "Other RAT threshold"};
    }

    @Override
    void update(int type, Parser data) {
        int num = data.getInt("num_event_info");
        List<Parser> info = data.getArray("em_event_info");
        clearData();
        for (int i = 0; i < num && i < info.size(); i++) {
            data = info.get(i);
            int measId = data.getInt("MeasId");
            int eventType = data.getInt("EventType");
            int measQty = data.getInt("MeasQty");
            int measQtyOtherRAT = data.getInt("MeasQtyOtherRAT");
            int thresholdOwnSystem = data.getInt("ThresholdOwnSystem");
            int thresholdOtherSystem = data.getInt("ThresholdOtherSystem");
            if (eventType == 3) {
                if (measId == 0) {
                    addData("-");
                } else {
                    addData(measId);
                }
                addData(mQtyMapping.get(measQty));
                addData(mQtyOtherRatMapping.get(measQtyOtherRAT));
                if (thresholdOwnSystem == 0xFFFF) {
                    addData("-");
                } else {
                    addData((float) thresholdOwnSystem / 4);
                }
                if (thresholdOtherSystem == 0xFFFF) {
                    addData("-");
                } else {
                    addData((float) thresholdOtherSystem / 4);
                }
            }
        }
        notifyDataSetChanged();
    }
}

class UmtsRrcMeasurementControlForE3d extends NormalTableComponent {
    private static final int[] EM_TYPES = new int[] {Content.EM_TDD_MEME_INFO_LTE_CELL_INFO};

    HashMap<Integer, String> mQtyMapping = new HashMap<Integer, String>() { {
        put(0, "-");
        put(1, "UMTS RSCP");
        put(2, "UMTS ECN0");
        put(3, "UMTS PATHLOSS");
    } };
    HashMap<Integer, String> mQtyOtherRatMapping = new HashMap<Integer, String>() { {
        put(0, "-");
        put(1, "GSM RSSI");
        put(2, "LTE RSRP");
        put(3, "LTE RSRQ");
    } };

    public UmtsRrcMeasurementControlForE3d(Activity context) {
        super(context);
    }

    @Override
    String getName() {
        return "UMTS RRC Measurement control for e3d (UMTS TDD)";
    }

    @Override
    String getGroup() {
        return "4. UMTS TDD EM Component";
    }

    @Override
    int[] getEmType() {
        return EM_TYPES;
    }

    String[] getLabels() {
        return new String[] {"e3d meas ID", "UMTS RAT meas quantity", "Other RAT meas quantity",
                             "UMTS RAT threshold", "Other RAT threshold"};
    }

    @Override
    void update(int type, Parser data) {
        int num = data.getInt("num_event_info");
        List<Parser> info = data.getArray("em_event_info");
        clearData();
        for (int i = 0; i < num && i < info.size(); i++) {
            data = info.get(i);
            int measId = data.getInt("MeasId");
            int eventType = data.getInt("EventType");
            int measQty = data.getInt("MeasQty");
            int measQtyOtherRAT = data.getInt("MeasQtyOtherRAT");
            int thresholdOwnSystem = data.getInt("ThresholdOwnSystem");
            int thresholdOtherSystem = data.getInt("ThresholdOtherSystem");
            if (eventType == 4) {
                if (measId == 0) {
                    addData("-");
                } else {
                    addData(measId);
                }
                addData(mQtyMapping.get(measQty));
                addData(mQtyOtherRatMapping.get(measQtyOtherRAT));
                if (thresholdOwnSystem == 0xFFFF) {
                    addData("-");
                } else {
                    addData((float) thresholdOwnSystem / 4);
                }
                if (thresholdOtherSystem == 0xFFFF) {
                    addData("-");
                } else {
                    addData((float) thresholdOtherSystem / 4);
                }
            }
        }
        notifyDataSetChanged();
    }
}

class UmtsRrcMeasurementReportForE3a extends NormalTableComponent {
    private static final int[] EM_TYPES = new int[] {Content.EM_TDD_MEME_INFO_REPORT_INFO};

    public UmtsRrcMeasurementReportForE3a(Activity context) {
        super(context);
    }

    @Override
    String getName() {
        return "UMTS RRC Measurement report for e3a (UMTS TDD)";
    }

    @Override
    String getGroup() {
        return "4. UMTS TDD EM Component";
    }

    @Override
    int[] getEmType() {
        return EM_TYPES;
    }

    String[] getLabels() {
        return new String[] {"e3a meas ID"};
    }

    @Override
    void update(int type, Parser data) {
        int measId = data.getInt("MeasId");
        int eventType = data.getInt("EventType");
        clearData();
        if (eventType == 1) {
            if (measId == 0) {
                addData("-");
            } else {
                addData(measId);
            }
        }
        notifyDataSetChanged();
    }
}


class UmtsRrcMeasurementReportForE3b extends NormalTableComponent {
    private static final int[] EM_TYPES = new int[] {Content.EM_TDD_MEME_INFO_REPORT_INFO};

    public UmtsRrcMeasurementReportForE3b(Activity context) {
        super(context);
    }

    @Override
    String getName() {
        return "UMTS RRC Measurement report for e3b (UMTS TDD)";
    }

    @Override
    String getGroup() {
        return "4. UMTS TDD EM Component";
    }

    @Override
    int[] getEmType() {
        return EM_TYPES;
    }

    String[] getLabels() {
        return new String[] {"e3b meas ID"};
    }

    @Override
    void update(int type, Parser data) {
        int measId = data.getInt("MeasId");
        int eventType = data.getInt("EventType");
        clearData();
        if (eventType == 2) {
            if (measId == 0) {
                addData("-");
            } else {
                addData(measId);
            }
        }
        notifyDataSetChanged();
    }
}

class UmtsRrcMeasurementReportForE3c extends NormalTableComponent {
    private static final int[] EM_TYPES = new int[] {Content.EM_TDD_MEME_INFO_REPORT_INFO};

    public UmtsRrcMeasurementReportForE3c(Activity context) {
        super(context);
    }

    @Override
    String getName() {
        return "UMTS RRC Measurement report for e3c (UMTS TDD)";
    }

    @Override
    String getGroup() {
        return "4. UMTS TDD EM Component";
    }

    @Override
    int[] getEmType() {
        return EM_TYPES;
    }

    String[] getLabels() {
        return new String[] {"e3c meas ID"};
    }

    @Override
    void update(int type, Parser data) {
        int measId = data.getInt("MeasId");
        int eventType = data.getInt("EventType");
        clearData();
        if (eventType == 3) {
            if (measId == 0) {
                addData("-");
            } else {
                addData(measId);
            }
        }
        notifyDataSetChanged();
    }
}

class UmtsRrcMeasurementReportForE3d extends NormalTableComponent {
    private static final int[] EM_TYPES = new int[] {Content.EM_TDD_MEME_INFO_REPORT_INFO};

    public UmtsRrcMeasurementReportForE3d(Activity context) {
        super(context);
    }

    @Override
    String getName() {
        return "UMTS RRC Measurement report for e3d (UMTS TDD)";
    }

    @Override
    String getGroup() {
        return "4. UMTS TDD EM Component";
    }

    @Override
    int[] getEmType() {
        return EM_TYPES;
    }

    String[] getLabels() {
        return new String[] {"e3d meas ID"};
    }

    @Override
    void update(int type, Parser data) {
        int measId = data.getInt("MeasId");
        int eventType = data.getInt("EventType");
        clearData();
        if (eventType == 1) {
            if (measId == 0) {
                addData("-");
            } else {
                addData(measId);
            }
        }
        notifyDataSetChanged();
    }
}

class UmtsNeighborCellInfoGsmTdd extends NormalTableComponent {
    private static final int[] EM_TYPES = new int[] {
            Content.RRM_EM_IR_3G_NEIGHBOR_MEAS_STATUS_IND_STRUCT_INFO};
    HashMap<Integer, String> mCellTypeMapping = new HashMap<Integer, String>() { {
        put(0, "NORMAL");
        put(1, "CSG");
        put(2, "HYBRID");
    } };
    HashMap<Integer, String> mFailTypeMapping = new HashMap<Integer, String>() { {
        put(0, "IR_INITIAL_VALUE");
        put(1, "NEVER_ALLOWED");
        put(2, "PLMN_ID_MISMATCHED");
        put(3, "LA_NOT_ALLOWED");
        put(4, "CELL_BARRED");
        put(5, "TEMP_FAILURE");
        put(6, "CRITERIA3_NOT_SATISFIED");
        put(7, "TA_NOT_ALLOWED");
        put(8, "CELL_BARRED_FREQ");
        put(9, "CELL_BARRED_RESV_OPERATOR");
        put(10, "CELL_BARRED_RESV_OPERATOR_FREQ");
        put(11, "CSG_NOT_ALLOWED");
    } };

    public UmtsNeighborCellInfoGsmTdd(Activity context) {
        super(context);
    }

    @Override
    String getName() {
        return "UMTS Neighbor Cell Info (GSM TDD)";
    }

    @Override
    String getGroup() {
        return "2. GSM EM Component";
    }

    @Override
    int[] getEmType() {
        return EM_TYPES;
    }

    String[] getLabels() {
        return new String[] {"cell_type", "uarfcn", "phy_id", "strength", "quailty", "rep_value",
                             "non_rep_value", "fail_type"};
    }

    @Override
    void update(int type, Parser data) {
        List<Parser> status = data.getArray("ir_3g_neighbor_meas_status");
        clearData();
        for (int i = 0; i < status.size(); i++) {
            data = status.get(i);
            boolean isValid = data.getBoolean("is_valid");
            if (isValid) {
                int cellType = data.getInt("cell_type");
                int failType = data.getInt("fail_type");
                String cell = mCellTypeMapping.get(cellType);
                String fail = mFailTypeMapping.get(failType);
                addData(cell);
                addData(data.getInt("uarfcn"));
                addData(data.getInt("phy_id"));
                addData(data.getInt("strength"));
                addData(data.getInt("quailty"));
                addData(data.getInt("rep_value"));
                addData(data.getInt("non_rep_value"));
                addData(fail);
            }
        }
        notifyDataSetChanged();
    }
}

class LTENeighborCellInfoGSM extends NormalTableComponent {
    private static final int[] EM_TYPES = new int[] {
            Content.RRM_EM_IR_4G_NEIGHBOR_MEAS_STATUS_IND_STRUCT_INFO};
    HashMap<Integer, String> mCellTypeMapping = new HashMap<Integer, String>() { {
        put(0, "Normal");
        put(1, "CSG");
        put(2, "Hybrid");
    } };
    HashMap<Integer, String> mBandTypeMapping = new HashMap<Integer, String>() { {
        put(0, "-");
        put(1, "FDD");
        put(2, "TDD");
    } };
    HashMap<Integer, String> mFailTypeMapping = new HashMap<Integer, String>() { {
        put(0, "None");
        put(1, "Never");
        put(2, "PLMN");
        put(3, "LA");
        put(4, "C_BAR");
        put(5, "TEMP");
        put(6, "CRI3");
        put(7, "TA");
        put(8, "FREQ");
        put(9, "RES_OP");
        put(10, "RES_OP_FRE");
        put(11, "CSG");
        put(12, "GEMINI");
        put(13, "NAS");
    } };

    public LTENeighborCellInfoGSM(Activity context) {
        super(context);
    }

    @Override
    String getName() {
        return "LTE Neighbor Cell Info (GSM)";
    }

    @Override
    String getGroup() {
        return "2. GSM EM Component";
    }

    @Override
    int[] getEmType() {
        return EM_TYPES;
    }

    String[] getLabels() {
        return new String[] {"cell_type", "earfcn", "pci", "band_type", "rsrp", "rsrq",
                             "rep_value", "non_rep_value", "fail_type"};
    }

    @Override
    void update(int type, Parser data) {
        List<Parser> status = data.getArray("ir_4g_neighbor_meas_status");
        clearData();
        for (int i = 0; i < status.size(); i++) {
            data = status.get(i);
            boolean isValid = data.getBoolean("is_valid");
            int cellType = data.getInt("cell_type");
            long earfcn = data.getLong("earfcn");
            int pci = data.getInt("pci");
            int bandType = data.getInt("band_type");
            int rsrp = data.getInt("rsrp");
            int rsrq = data.getInt("rsrq");
            int repValue = data.getInt("rep_value");
            int nonRepValue = data.getInt("non_rep_value");
            int failType = data.getInt("fail_type");
            if (isValid) {
                String cell = mCellTypeMapping.get(cellType);
                String band = mBandTypeMapping.get(bandType);
                String fail = mFailTypeMapping.get(failType);
                addData(cell);
                addData(earfcn);
                addData(pci);
                addData(band);
                addData(rsrp);
                addData(rsrq);
                addData(repValue);
                addData(nonRepValue);
                addData(fail);
            }
        }
        notifyDataSetChanged();
    }
}

class LteErrcMeasurementConfigForEventB1 extends ArrayTableComponent {
    private static final int[] EM_TYPES = new int[] {Content.ERRC_EM_MOB_MEAS_CONFIG_INFO_IND};
    HashMap<Integer, String> mMapping = new HashMap<Integer, String>() { {
        put(0, "-");
        put(1, "GSM RSSI");
        put(2, "UMTS RSRP");
        put(3, "UMTS ECN0");
    } };

    public LteErrcMeasurementConfigForEventB1(Activity context) {
        super(context);
    }

    @Override
    String getName() {
        return "LTE ERRC Measurement config for event B1";
    }

    @Override
    String getGroup() {
        return "5. LTE EM Component";
    }

    @Override
    int[] getEmType() {
        return EM_TYPES;
    }

    String[] getLabels() {
        return new String[] {"Event B1 meas ID", "Other RAT meas quantity", "Other RAT threshold"};
    }

    @Override
    void update(int type, Parser data) {
        int num = data.getInt("num_event_info");
        List<Parser> eventList = data.getArray("em_event_info");
        clearData();
        for (int i = 0; i < num && i < eventList.size(); i++) {
            data = eventList.get(i);
            int eventType = data.getInt("EventType");
            if (eventType == 1) {
                int measId = data.getInt("MeasId");
                int measQty = data.getInt("MeasQtyOtherRAT");
                int threshold = data.getInt("ThresholdOtherSystem");
                addData(measId == 0 ? "-" : measId, mMapping.get(measQty),
                        threshold == 0 ? "-" : (float) threshold / 4);
            }
        }
    }
}

class LteErrcMeasurementConfigForEventB2 extends ArrayTableComponent {
    private static final int[] EM_TYPES = new int[] {Content.ERRC_EM_MOB_MEAS_CONFIG_INFO_IND};
    HashMap<Integer, String> mMeasQtyMapping = new HashMap<Integer, String>() { {
        put(0, "-");
        put(1, "LTE RSRP");
        put(2, "LTE RSRQ");
    } };
    HashMap<Integer, String> mMeasQtyOtherMapping = new HashMap<Integer, String>() { {
        put(0, "-");
        put(1, "GSM RSSI");
        put(2, "UMTS RSRP");
        put(3, "UMTS ECN0");
    } };

    public LteErrcMeasurementConfigForEventB2(Activity context) {
        super(context);
    }

    @Override
    String getName() {
        return "LTE ERRC Measurement config for event B2";
    }

    @Override
    String getGroup() {
        return "5. LTE EM Component";
    }

    @Override
    int[] getEmType() {
        return EM_TYPES;
    }

    String[] getLabels() {
        return new String[] {"Event B2 meas ID", "LTE RAT meas quantity",
                "Other RAT meas quantity", "LTE RAT threshold", "Other RAT threshold"};
    }

    @Override
    void update(int type, Parser data) {
        int num = data.getInt("num_event_info");
        List<Parser> eventList = data.getArray("em_event_info");
        clearData();
        for (int i = 0; i < num && i < eventList.size(); i++) {
            data = eventList.get(i);
            int eventType = data.getInt("EventType");
            if (eventType == 2) {
                int measId = data.getInt("MeasId");
                int measQty = data.getInt("MeasQty");
                int measQtyOtherRat = data.getInt("MeasQtyOtherRAT");
                int threshold = data.getInt("ThresholdOwnSystem");
                int thresholdOtherSystem = data.getInt("ThresholdOtherSystem");
                addData(
                    measId == 0 ? "-" : measId,
                    mMeasQtyMapping.get(measQty),
                    mMeasQtyOtherMapping.get(measQtyOtherRat),
                    threshold == 0 ? "-" : (float) threshold / 4,
                    thresholdOtherSystem == 0 ? "-" : (float) thresholdOtherSystem / 4
                );
            }
        }
    }
}

class LteErrcMeasurementReportForEventB1 extends NormalTableComponent {
    private static final int[] EM_TYPES = new int[] {Content.ERRC_EM_MOB_MEAS_REPORT_INFO_IND};

    public LteErrcMeasurementReportForEventB1(Activity context) {
        super(context);
    }

    @Override
    String getName() {
        return "LTE ERRC Measurement report for event B1";
    }

    @Override
    String getGroup() {
        return "5. LTE EM Component";
    }

    @Override
    int[] getEmType() {
        return EM_TYPES;
    }

    String[] getLabels() {
        return new String[] {"Event B1 meas ID"};
    }

    @Override
    void update(int type, Parser data) {
        clearData();
        int eventType = data.getInt("EventType");
        if (eventType == 1) {
            int measId = data.getInt("MeasId");
            addData(measId == 0 ? "-" : measId);
        }
    }
}

class LteErrcMeasurementReportForEventB2 extends NormalTableComponent {
    private static final int[] EM_TYPES = new int[] {Content.ERRC_EM_MOB_MEAS_REPORT_INFO_IND};

    public LteErrcMeasurementReportForEventB2(Activity context) {
        super(context);
    }

    @Override
    String getName() {
        return "LTE ERRC Measurement report for event B2";
    }

    @Override
    String getGroup() {
        return "5. LTE EM Component";
    }

    @Override
    int[] getEmType() {
        return EM_TYPES;
    }

    String[] getLabels() {
        return new String[] {"Event B2 meas ID"};
    }

    @Override
    void update(int type, Parser data) {
        clearData();
        int eventType = data.getInt("EventType");
        if (eventType == 2) {
            int measId = data.getInt("MeasId");
            addData(measId == 0 ? "-" : measId);
        }
    }
}

class EDchTtiConfigured extends NormalTableComponent {
    private static final int[] EM_TYPES = new int[] {Content.EM_RRCE_HSPA_CONFIG_IND};
    HashMap<Integer, String> mMapping = new HashMap<Integer, String>() { {
        put(0, "2 ms");
        put(1, "10 ms");
        put(255, "N/A");
    } };

    public EDchTtiConfigured(Activity context) {
        super(context);
    }

    @Override
    String getName() {
        return "E-DCH TTI Configured";
    }

    @Override
    String getGroup() {
        return "3. UMTS FDD EM Component";
    }

    @Override
    int[] getEmType() {
        return EM_TYPES;
    }

    String[] getLabels() {
        return new String[] {"E-DCH TTI"};
    }

    @Override
    void update(int type, Parser data) {
        String tti = mMapping.get(data.getInt("e_dch_tti"));
        clearData();
        addData(tti == null ? "N/A" : tti);
    }
}

class HsdpaConfiguredUmtsFdd extends NormalTableComponent {
    private static final int[] EM_TYPES = new int[] {Content.EM_RRCE_HSPA_CONFIG_IND};
    HashMap<Integer, String> mMapping = new HashMap<Integer, String>() { {
        put(0, "FALSE");
        put(1, "TRUE");
    } };

    public HsdpaConfiguredUmtsFdd(Activity context) {
        super(context);
    }

    @Override
    String getName() {
        return "HSDPA configured (UMTS FDD)";
    }

    @Override
    String getGroup() {
        return "3. UMTS FDD EM Component";
    }

    @Override
    int[] getEmType() {
        return EM_TYPES;
    }

    String[] getLabels() {
        return new String[] {"HSDPA configured"};
    }

    @Override
    void update(int type, Parser data) {
        String tti = mMapping.get(data.getInt("hsdpa_configured"));
        clearData();
        addData(tti == null ? "FALSE" : tti);
    }
}

class DcHsdpaConfiguredUmtsFdd extends NormalTableComponent {
    private static final int[] EM_TYPES = new int[] {Content.EM_RRCE_HSPA_CONFIG_IND};
    HashMap<Integer, String> mMapping = new HashMap<Integer, String>() { {
        put(0, "FALSE");
        put(1, "TRUE");
    } };

    public DcHsdpaConfiguredUmtsFdd(Activity context) {
        super(context);
    }

    @Override
    String getName() {
        return "DC-HSDPA configured (UMTS FDD)";
    }

    @Override
    String getGroup() {
        return "3. UMTS FDD EM Component";
    }

    @Override
    int[] getEmType() {
        return EM_TYPES;
    }

    String[] getLabels() {
        return new String[] {"DC-HSDPA configured"};
    }

    @Override
    void update(int type, Parser data) {
        String tti = mMapping.get(data.getInt("dc_hsdpa_configured"));
        clearData();
        addData(tti == null ? "FALSE" : tti);
    }
}

class HsDschModulationPrimaryCell extends NormalTableComponent {
    private static final int[] EM_TYPES = new int[] {Content.UL1_EM_HSPA_INFO_GROUP};
    HashMap<Integer, String> mMapping = new HashMap<Integer, String>() { {
        put(0, "QPSK (0)");
        put(1, "16QAM (1)");
        put(2, "64QAM (2)");
    } };

    public HsDschModulationPrimaryCell(Activity context) {
        super(context);
    }

    @Override
    String getName() {
        return "HS-DSCH Modulation (Primary Cell)";
    }

    @Override
    String getGroup() {
        return "3. UMTS FDD EM Component";
    }

    @Override
    int[] getEmType() {
        return EM_TYPES;
    }

    String[] getLabels() {
        return new String[] {"Modulation (Pri)"};
    }

    @Override
    void update(int type, Parser data) {
        Parser bler = data.get("primary_hs_dsch_bler");
        int mod = bler.getInt("dsch_curr_mod");
        clearData();
        addData(mMapping.get(mod));
    }
}

class HsupaConfiguredUmtsFdd extends NormalTableComponent {
    private static final int[] EM_TYPES = new int[] {Content.EM_RRCE_HSPA_CONFIG_IND};
    HashMap<Integer, String> mMapping = new HashMap<Integer, String>() { {
        put(0, "FALSE");
        put(1, "TRUE");
    } };

    public HsupaConfiguredUmtsFdd(Activity context) {
        super(context);
    }

    @Override
    String getName() {
        return "HSUPA configured (UMTS FDD)";
    }

    @Override
    String getGroup() {
        return "3. UMTS FDD EM Component";
    }

    @Override
    int[] getEmType() {
        return EM_TYPES;
    }

    String[] getLabels() {
        return new String[] {"HSUPA configured"};
    }

    @Override
    void update(int type, Parser data) {
        String tti = mMapping.get(data.getInt("hsupa_configured"));
        clearData();
        addData(tti == null ? "FALSE" : tti);
    }
}

class WcdmaHsupaCapability extends NormalTableComponent {
    private static final int[] EM_TYPES = new int[] {Content.USIME_EM_INFO_CAPABILITY};
    HashMap<Integer, String> mMapping = new HashMap<Integer, String>() { {
        put(0, "ON");
        put(1, "OFF");
    } };

    public WcdmaHsupaCapability(Activity context) {
        super(context);
    }

    @Override
    String getName() {
        return "WCDMA HSUPA Capability";
    }

    @Override
    String getGroup() {
        return "3. UMTS FDD EM Component";
    }

    @Override
    int[] getEmType() {
        return EM_TYPES;
    }

    String[] getLabels() {
        return new String[] {"HSUPA Support"};
    }

    @Override
    void update(int type, Parser data) {
        clearData();
        addData(mMapping.get(data.getInt("hsupa_enable")));
    }
}

class WcdmaHsdpaCapability extends NormalTableComponent {
    private static final int[] EM_TYPES = new int[] {Content.USIME_EM_INFO_CAPABILITY};
    HashMap<Integer, String> mMapping = new HashMap<Integer, String>() { {
        put(0, "ON");
        put(1, "OFF");
    } };

    public WcdmaHsdpaCapability(Activity context) {
        super(context);
    }

    @Override
    String getName() {
        return "WCDMA HSDPA Capability";
    }

    @Override
    String getGroup() {
        return "3. UMTS FDD EM Component";
    }

    @Override
    int[] getEmType() {
        return EM_TYPES;
    }

    String[] getLabels() {
        return new String[] {"HSDPA Support"};
    }

    @Override
    void update(int type, Parser data) {
        clearData();
        addData(mMapping.get(data.getInt("hsdpa_enable")));
    }
}

class WcdmaTasInfo extends NormalTableComponent {
    private static final int[] EM_TYPES = new int[] {Content.EM_UL1_TAS_INFO_IND};

    public WcdmaTasInfo(Activity context) {
        super(context);
    }

    @Override
    String getName() {
        return "WCDMA TAS Info";
    }

    @Override
    String getGroup() {
        return "3. UMTS FDD EM Component";
    }

    @Override
    int[] getEmType() {
        return EM_TYPES;
    }

    String[] getLabels() {
        return new String[] {"Antenna Index", "RSCP Max", "RSCP Ant0","RSCP Ant1",
                             "TX Power", "DPCCH TX Power"};
    }

    @Override
    void update(int type, Parser data) {
        Parser ul1tas = data.get("EmUl1Tas");
        int antidx = ul1tas.getInt("main_ant_idx");
        int rscp_max = ul1tas.getInt("rscp_max");
        int rscp_0 = ul1tas.getInt("rscp_0");
        int rscp_1 = ul1tas.getInt("rscp_1");
        int tx_pwr = ul1tas.getInt("tx_pwr");
        int dpcch_tx_pwr = ul1tas.getInt("dpcch_tx_pwr");
        clearData();
        addData(antidx, rscp_max, rscp_0, rscp_1, tx_pwr, dpcch_tx_pwr);
    }
}


class Fdd3gSpeechCodec extends NormalTableComponent {
    private static final int[] EM_TYPES = new int[] {Content.EM_CSR_STATUS_IND};
    HashMap<Integer, String> mMapping = new HashMap<Integer, String>() { {
        put(0, "AMR_4_75");
        put(1, "AMR_5_15");
        put(2, "AMR_5_90");
        put(3, "AMR_6_70");
        put(4, "AMR_7_40");
        put(5, "AMR_7_95");
        put(6, "AMR_10_20");
        put(7, "AMR_12_20");
        put(8, "AMR_SID");
        put(9, "GSM_EFR_SID");
        put(10, "TDMA_EFR_SID");
        put(11, "PDC_EFR_SID");
        put(12, "RESERVE_1");
        put(13, "RESERVE_2");
        put(14, "RESERVE_3");
        put(15, "AMR_NODATA");
        put(16, "AMRWB_6_60");
        put(17, "AMRWB_8_85");
        put(18, "AMRWB_12_65");
        put(19, "AMRWB_14_25");
        put(20, "AMRWB_15_85");
        put(21, "AMRWB_18_25");
        put(22, "AMRWB_19_85");
        put(23, "AMRWB_23_05");
        put(24, "AMRWB_23_85");
        put(25, "AMRWB_SID");
    } };

    public Fdd3gSpeechCodec(Activity context) {
        super(context);
    }

    @Override
    String getName() {
        return "3G FDD Speech Codec";
    }

    @Override
    String getGroup() {
        return "3. UMTS FDD EM Component";
    }

    @Override
    int[] getEmType() {
        return EM_TYPES;
    }

    String[] getLabels() {
        return new String[] {"UL speech codec", "DL speech codec"};
    }

    @Override
    void update(int type, Parser data) {
        clearData();
        addData(mMapping.get(data.getInt("ULAMRType")));
        addData(mMapping.get(data.getInt("DLAMRType")));
    }
}

class PlmnSearchStatusUmtsFdd extends NormalTableComponent {
    private static final int[] EM_TYPES = new int[] {Content.FDD_EM_URR_3G_GENERAL_STATUS_IND};
    HashMap<Integer, String> mMapping = new HashMap<Integer, String>() { {
        put(0, "");
        put(1, "Search any PLMN");
        put(2, "Search given PLMN");
        put(3, "Search any PLMN success");
        put(4, "Search any PLMN failure");
        put(5, "Search given PLMN success");
        put(6, "Search given PLMN failure");
        put(7, "Search PLMN abort");
    } };

    public PlmnSearchStatusUmtsFdd(Activity context) {
        super(context);
    }

    @Override
    String getName() {
        return "PLMN Search status (UMTS FDD)";
    }

    @Override
    String getGroup() {
        return "3. UMTS FDD EM Component";
    }

    @Override
    int[] getEmType() {
        return EM_TYPES;
    }

    String[] getLabels() {
        return new String[] {"PLMN search status"};
    }

    @Override
    void update(int type, Parser data) {
        clearData();
        addData(mMapping.get(data.getInt("plmn_search_status")));
    }
}

class CellSupportPsInfo extends NormalTableComponent {
    private static final int[] EM_TYPES = new int[] {Content.GMM_EM_INFO};

    public CellSupportPsInfo(Activity context) {
        super(context);
    }

    @Override
    String getName() {
        return "Cell Support PS INFO";
    }

    @Override
    String getGroup() {
        return "1. General EM Component";
    }

    @Override
    int[] getEmType() {
        return EM_TYPES;
    }

    String[] getLabels() {
        return new String[] {"cell support PS"};
    }

    @Override
    void update(int type, Parser data) {
        clearData();
        addData(data.getBoolean("cell_support_ps"));
    }
}

class DtxConfiguredUmtsFdd extends NormalTableComponent {
    private static final int[] EM_TYPES = new int[] {Content.EM_RRCE_HSPA_CONFIG_IND};

    HashMap<Integer, String> mMapping = new HashMap<Integer, String>() { {
        put(0, "FALSE");
        put(1, "TRUE");
    } };

    public DtxConfiguredUmtsFdd(Activity context) {
        super(context);
    }

    @Override
    String getName() {
        return "DTX configured (UMTS FDD)";
    }

    @Override
    String getGroup() {
        return "3. UMTS FDD EM Component";
    }

    @Override
    int[] getEmType() {
        return EM_TYPES;
    }

    String[] getLabels() {
        return new String[] {"DTX configured"};
    }

    @Override
    void update(int type, Parser data) {
        String tti = mMapping.get(data.getInt("dtx_configured"));
        clearData();
        addData(tti == null ? "FALSE" : tti);
    }
}

class DrxConfiguredUmtsFdd extends NormalTableComponent {
    private static final int[] EM_TYPES = new int[] {Content.EM_RRCE_HSPA_CONFIG_IND};

    HashMap<Integer, String> mMapping = new HashMap<Integer, String>() { {
        put(0, "FALSE");
        put(1, "TRUE");
    } };

    public DrxConfiguredUmtsFdd(Activity context) {
        super(context);
    }

    @Override
    String getName() {
        return "DRX configured (UMTS FDD)";
    }

    @Override
    String getGroup() {
        return "3. UMTS FDD EM Component";
    }

    @Override
    int[] getEmType() {
        return EM_TYPES;
    }

    String[] getLabels() {
        return new String[] {"DRX configured"};
    }

    @Override
    void update(int type, Parser data) {
        String tti = mMapping.get(data.getInt("drx_configured"));
        clearData();
        addData(tti == null ? "FALSE" : tti);
    }
}

class FastDormancyConfiguration extends NormalTableComponent {
    private static final int[] EM_TYPES = new int[] {Content.EM_RRCE_FD_CONFIGURATION_STATUS_IND};

    HashMap<Boolean, String> mMapping = new HashMap<Boolean, String>() { {
        put(false, "OFF");
        put(true, "ON");
    } };

    public FastDormancyConfiguration(Activity context) {
        super(context);
    }

    @Override
    String getName() {
        return "Fast Dormancy Configuration";
    }

    @Override
    String getGroup() {
        return "3. UMTS FDD EM Component";
    }

    @Override
    int[] getEmType() {
        return EM_TYPES;
    }

    String[] getLabels() {
        return new String[] {"Fast Dormancy configured"};
    }

    @Override
    void update(int type, Parser data) {
        String value = mMapping.get(data.getBoolean("FDcfgStatus"));
        clearData();
        addData(value == null ? "OFF" : value);
    }
}

class IntraFrequencyMonitoredSetUmtsFdd extends ArrayTableComponent {
    private static final int[] EM_TYPES = new int[] {Content.EM_FDD_MEME_INFO_DCH_UMTS_CELL_INFO};

    public IntraFrequencyMonitoredSetUmtsFdd(Activity context) {
        super(context);
    }

    @Override
    String getName() {
        return "Intra-frequency monitored set (UMTS FDD)";
    }

    @Override
    String getGroup() {
        return "3. UMTS FDD EM Component";
    }

    @Override
    int[] getEmType() {
        return EM_TYPES;
    }

    String[] getLabels() {
        return new String[] {"UARFCN", "PSC", "RSCP", "ECN0"};
    }

    @Override
    void update(int type, Parser data) {
        int num = data.getInt("num_cells");
        List<Parser> cellList = data.getArray("umts_cell_list");
        clearData();
        for (int i = 0; i < num && i < cellList.size(); i++) {
            data = cellList.get(i);
            int cellType = data.getInt("cell_type");
            if (cellType == 1) {
                int urafcn = data.getInt("UARFCN");
                int psc = data.getInt("PSC");
                long rscp = data.getLong("RSCP");
                long ecn0 = data.getLong("ECN0");
                addData(urafcn, psc, rscp, ecn0);
            }
        }
    }
}

class IntraFrequencyDetectedSetUmtsFdd extends ArrayTableComponent {
    private static final int[] EM_TYPES = new int[] {Content.EM_FDD_MEME_INFO_DCH_UMTS_CELL_INFO};

    public IntraFrequencyDetectedSetUmtsFdd(Activity context) {
        super(context);
    }

    @Override
    String getName() {
        return "Intra-frequency detected set (UMTS FDD)";
    }

    @Override
    String getGroup() {
        return "3. UMTS FDD EM Component";
    }

    @Override
    int[] getEmType() {
        return EM_TYPES;
    }

    String[] getLabels() {
        return new String[] {"UARFCN", "PSC", "RSCP", "ECN0"};
    }

    @Override
    void update(int type, Parser data) {
        int num = data.getInt("num_cells");
        List<Parser> cellList = data.getArray("umts_cell_list");
        clearData();
        for (int i = 0; i < num && i < cellList.size(); i++) {
            data = cellList.get(i);
            int cellType = data.getInt("cell_type");
            if (cellType == 2) {
                int urafcn = data.getInt("UARFCN");
                int psc = data.getInt("PSC");
                long rscp = data.getLong("RSCP");
                long ecn0 = data.getLong("ECN0");
                addData(urafcn, psc, rscp, ecn0);
            }
        }
    }
}

class ActiveSetUmtsFdd extends ArrayTableComponent {
    private static final int[] EM_TYPES = new int[] {
        Content.EM_FDD_MEME_INFO_DCH_UMTS_CELL_INFO,
        Content.FDD_EM_CSCE_SERV_CELL_IND_STRUCT_INFO};

    public ActiveSetUmtsFdd(Activity context) {
        super(context);
    }

    @Override
    String getName() {
        return "Active Set (UMTS FDD)";
    }

    @Override
    String getGroup() {
        return "3. UMTS FDD EM Component";
    }

    @Override
    int[] getEmType() {
        return EM_TYPES;
    }

    String[] getLabels() {
        return new String[] {"UARFCN", "PSC", "RSCP", "ECN0"};
    }

    @Override
    void update(int type, Parser data) {
        if (type == Content.EM_FDD_MEME_INFO_DCH_UMTS_CELL_INFO) {
            int num = data.getInt("num_cells");
            List<Parser> cellList = data.getArray("umts_cell_list");
            clearData();
            for (int i = 0; i < num && i < cellList.size(); i++) {
                data = cellList.get(i);
                int cellType = data.getInt("cell_type");
                if (cellType == 0) {
                    int urafcn = data.getInt("UARFCN");
                    int psc = data.getInt("PSC");
                    long rscp = data.getLong("RSCP");
                    long ecn0 = data.getLong("ECN0");
                    addData(urafcn, psc, rscp, ecn0);
                }
            }
        } else if (type == Content.FDD_EM_CSCE_SERV_CELL_IND_STRUCT_INFO) {
            int urafcn = data.getInt("uarfcn_DL");
            int psc = data.getInt("psc");
            float rscp = (float) data.getLong("rscp") / 4096;
            float ecn0 = (float) data.getLong("ec_no") / 4096;
            if (rscp > -120 && ecn0 > -25) {
                clearData();
                addData(urafcn, psc, rscp, ecn0);
            }
        }
    }
}

class CsOverHspaUmtsFdd extends NormalTableComponent {
    private static final int[] EM_TYPES = new int[] {Content.EM_RRCE_CS_OVER_HSPA_STATUS_IND};

    HashMap<Boolean, String> mMapping = new HashMap<Boolean, String>() { {
        put(false, "OFF");
        put(true, "ON");
    } };

    public CsOverHspaUmtsFdd(Activity context) {
        super(context);
    }

    @Override
    String getName() {
        return "CS over HSPA (UMTS FDD)";
    }

    @Override
    String getGroup() {
        return "3. UMTS FDD EM Component";
    }

    @Override
    int[] getEmType() {
        return EM_TYPES;
    }

    String[] getLabels() {
        return new String[] {"CS over HSPA"};
    }

    @Override
    void update(int type, Parser data) {
        String status = mMapping.get(data.getBoolean("cs_over_hspa_status"));
        clearData();
        addData(status == null ? "FALSE" : status);
    }
}

class ShareNetworkPlmnInfo extends ArrayTableComponent {
    private static final int[] EM_TYPES = new int[] {Content.MMRR_EM_PLMN_INFO_STRUCT_INFO};

    HashMap<Integer, String> mMapping = new HashMap<Integer, String>() { {
        put(0, "CELL_TYPE_SUITABLE");
        put(1, "CELL_TYPE_ACCEPTABLE");
        put(2, "CELL_TYPE_CAMPED_NOT_ALLOWED");
        put(3, "CELL_TYPE_NOT_APPLICABLE");
    } };

    public ShareNetworkPlmnInfo(Activity context) {
        super(context);
    }

    @Override
    String getName() {
        return "Share Network PLMN Info";
    }

    @Override
    String getGroup() {
        return "1. General EM Component";
    }

    @Override
    int[] getEmType() {
        return EM_TYPES;
    }

    String[] getLabels() {
        return new String[] {"", "MCC1", "MCC2", "MCC3", "MNC1", "MNC2", "MNC3", "CELL TYPE"};
    }

    @Override
    void update(int type, Parser data) {
        int count = data.getInt("multi_plmn_count");
        List<Parser> plmnList = data.getArray("multi_plmn_id");
        clearData();
        for (int i = 0; i < count && i < plmnList.size(); i++) {
            data = plmnList.get(i);
            addData(
                "PLMN" + (i + 1),
                String.format("%X", data.getInt("mcc1")),
                String.format("%X", data.getInt("mcc2")),
                String.format("%X", data.getInt("mcc3")),
                String.format("%X", data.getInt("mnc1")),
                String.format("%X", data.getInt("mnc2")),
                String.format("%X", data.getInt("mnc3")),
                mMapping.get(data.getInt("cell_type"))
            );
        }
    }
}

class ServingCellPlmnIdentityUmtsFdd extends ArrayTableComponent {
    private static final int[] EM_TYPES = new int[] {Content.FDD_EM_CSCE_SERV_CELL_IND_STRUCT_INFO};

    HashMap<Integer, String> mMapping = new HashMap<Integer, String>() { {
        put(0, "CELL_TYPE_SUITABLE");
        put(1, "CELL_TYPE_ACCEPTABLE");
        put(2, "CELL_TYPE_CAMPED_NOT_ALLOWED");
        put(3, "CELL_TYPE_NOT_APPLICABLE");
    } };

    public ServingCellPlmnIdentityUmtsFdd(Activity context) {
        super(context);
    }

    @Override
    String getName() {
        return "Serving cell PLMN identity (UMTS FDD)";
    }

    @Override
    String getGroup() {
        return "3. UMTS FDD EM Component";
    }

    @Override
    int[] getEmType() {
        return EM_TYPES;
    }

    String[] getLabels() {
        return new String[] {"", "Serving cell PLMN identity"};
    }

    @Override
    void update(int type, Parser data) {
        int count = data.getInt("multi_plmn_count");
        List<Parser> plmnList = data.getArray("multi_plmn_id");
        clearData();
        for (int i = 0; i < count && i < plmnList.size(); i++) {
            data = plmnList.get(i);
            String plmn = "";
            plmn += data.getInt("mcc1");
            plmn += data.getInt("mcc2");
            plmn += data.getInt("mcc3");
            plmn += data.getInt("mnc1");
            plmn += data.getInt("mnc2");
            plmn += data.getInt("mnc3") == 0xF ? "" : data.getInt("mnc3");
            addData(i + 1, plmn);
        }
    }
}

class PrimaryCellCqi extends CurveComponent {
    private static final int[] EM_TYPES = new int[] {Content.EM_EL1_INFO};

    public PrimaryCellCqi(Activity context) {
        super(context);
    }

    @Override
    String getName() {
        return "Primary Cell CQI";
    }

    @Override
    String getGroup() {
        return "5. LTE EM Component";
    }

    @Override
    int[] getEmType() {
        return EM_TYPES;
    }

    @Override
    CurveView.AxisConfig configY() {
//        mYLabel.setText("RSCP (dBm)");
        mYLabel.setText("R");

        CurveView.Config[] configs = new CurveView.Config[2];
        configs[0] = new CurveView.Config();
        configs[0].color = 0xFF0000FF;
        configs[0].lineWidth = 3;
        configs[0].nodeType = CurveView.Config.TYPE_CIRCLE;
        configs[0].name = "CQI_CW0";

        configs[1] = new CurveView.Config();
        configs[1].color = 0xFF00FF00;
        configs[1].lineWidth = 3;
        configs[1].lineType = CurveView.Config.LINE_DASH;
        configs[1].nodeType = CurveView.Config.TYPE_CROSS;
        configs[1].name = "CQI_CW1";
        mCurveView.setConfig(configs);

        CurveView.AxisConfig yConfig = new CurveView.AxisConfig();
        yConfig.min = -100;
        yConfig.max = 100;
        yConfig.step = 10;
        yConfig.configMin = true;
        yConfig.configMax = true;
        yConfig.configStep = true;
        return yConfig;
    }

    @Override
    void update(int type, Parser data) {
        List<Parser> dlInfo = data.getArray("dl_info");
        int cw0 = dlInfo.get(0).getInt("cqi_cw0");
        int cw1 = dlInfo.get(0).getInt("cqi_cw1");
        addData(0, cw0);
        addData(0, cw1);
    }
}

class PrimaryCellDlImcs extends NormalTableComponent {
    private static final int[] EM_TYPES = new int[] {Content.EM_EL1_INFO};

    public PrimaryCellDlImcs(Activity context) {
        super(context);
    }

    @Override
    String getName() {
        return "Primary Cell DL Imcs";
    }

    @Override
    String getGroup() {
        return "5. LTE EM Component";
    }

    @Override
    int[] getEmType() {
        return EM_TYPES;
    }

    String[] getLabels() {
        return new String[] {"DL_Imcs"};
    }

    @Override
    void update(int type, Parser data) {
        List<Parser> dlInfo = data.getArray("dl_info");
        int value = dlInfo.get(0).getInt("DL_Imcs");
        clearData();
        addData(value);
    }
}

class PrimaryCellDlResourceBlock extends CurveComponent {
    private static final int[] EM_TYPES = new int[] {Content.EM_EL1_INFO};

    public PrimaryCellDlResourceBlock(Activity context) {
        super(context);
    }

    @Override
    String getName() {
        return "Primary Cell DL Resource Block";
    }

    @Override
    String getGroup() {
        return "5. LTE EM Component";
    }

    @Override
    int[] getEmType() {
        return EM_TYPES;
    }

    @Override
    CurveView.AxisConfig configY() {
//        mYLabel.setText("RSCP (dBm)");
        mYLabel.setText("R");

        CurveView.Config[] configs = new CurveView.Config[1];
        configs[0] = new CurveView.Config();
        configs[0].color = 0xFF0000FF;
        configs[0].lineWidth = 3;
        configs[0].nodeType = CurveView.Config.TYPE_CIRCLE;
        configs[0].name = "DL Resource Block";
        mCurveView.setConfig(configs);

        CurveView.AxisConfig yConfig = new CurveView.AxisConfig();
        yConfig.min = -100;
        yConfig.max = 100;
        yConfig.step = 10;
        yConfig.configMin = true;
        yConfig.configMax = true;
        yConfig.configStep = true;
        return yConfig;
    }

    @Override
    void update(int type, Parser data) {
        List<Parser> dlInfo = data.getArray("dl_info");
        int value = dlInfo.get(0).getInt("DL_rb");
        addData(0, value);
    }
}

class PrimaryCellAntennaPort extends NormalTableComponent {
    private static final int[] EM_TYPES = new int[] {Content.EM_EL1_INFO};

    public PrimaryCellAntennaPort(Activity context) {
        super(context);
    }

    @Override
    String getName() {
        return "Primary Cell Antenna Port";
    }

    @Override
    String getGroup() {
        return "5. LTE EM Component";
    }

    @Override
    int[] getEmType() {
        return EM_TYPES;
    }

    String[] getLabels() {
        return new String[] {"ANT_PORT"};
    }

    @Override
    void update(int type, Parser data) {
        List<Parser> dlInfo = data.getArray("cell_info");
        int value = dlInfo.get(0).getInt("ant_port");
        clearData();
        addData(value);
    }
}

class PrimaryCellDlThroughput extends CurveComponent {
    private static final int[] EM_TYPES = new int[] {Content.EM_EL1_INFO};

    public PrimaryCellDlThroughput(Activity context) {
        super(context);
    }

    @Override
    String getName() {
        return "Primary Cell DL Throughput";
    }

    @Override
    String getGroup() {
        return "5. LTE EM Component";
    }

    @Override
    int[] getEmType() {
        return EM_TYPES;
    }

    @Override
    CurveView.AxisConfig configY() {
//        mYLabel.setText("RSCP (dBm)");
        mYLabel.setText("R");

        CurveView.Config[] configs = new CurveView.Config[1];
        configs[0] = new CurveView.Config();
        configs[0].color = 0xFF0000FF;
        configs[0].lineWidth = 3;
        configs[0].nodeType = CurveView.Config.TYPE_CIRCLE;
        configs[0].name = "DL Resource Block";
        mCurveView.setConfig(configs);

        CurveView.AxisConfig yConfig = new CurveView.AxisConfig();
        yConfig.min = -100;
        yConfig.max = 100;
        yConfig.step = 10;
        yConfig.configMin = true;
        yConfig.configMax = true;
        yConfig.configStep = true;
        return yConfig;
    }

    @Override
    void update(int type, Parser data) {
        List<Parser> dlInfo = data.getArray("dl_info");
        long value = dlInfo.get(0).getLong("DL_Tput");
        addData(0, value);
    }
}

class PrimaryCellDlBlockRate extends CurveComponent {
    private static final int[] EM_TYPES = new int[] {Content.EM_EL1_INFO};

    public PrimaryCellDlBlockRate(Activity context) {
        super(context);
    }

    @Override
    String getName() {
        return "Primary Cell DL Block Rate";
    }

    @Override
    String getGroup() {
        return "5. LTE EM Component";
    }

    @Override
    int[] getEmType() {
        return EM_TYPES;
    }

    @Override
    CurveView.AxisConfig configY() {
//        mYLabel.setText("RSCP (dBm)");
        mYLabel.setText("R");

        CurveView.Config[] configs = new CurveView.Config[1];
        configs[0] = new CurveView.Config();
        configs[0].color = 0xFF0000FF;
        configs[0].lineWidth = 3;
        configs[0].nodeType = CurveView.Config.TYPE_CIRCLE;
        configs[0].name = "DL Resource Block";
        mCurveView.setConfig(configs);

        CurveView.AxisConfig yConfig = new CurveView.AxisConfig();
        yConfig.min = -100;
        yConfig.max = 100;
        yConfig.step = 10;
        yConfig.configMin = true;
        yConfig.configMax = true;
        yConfig.configStep = true;
        return yConfig;
    }

    @Override
    void update(int type, Parser data) {
        List<Parser> dlInfo = data.getArray("dl_info");
        int value = dlInfo.get(0).getInt("DL_block");
        addData(0, value);
    }
}

class PrimaryCellRsrpRx extends NormalTableComponent {
    private static final int[] EM_TYPES = new int[] {Content.EM_EL1_INFO};

    public PrimaryCellRsrpRx(Activity context) {
        super(context);
    }

    @Override
    String getName() {
        return "Primary Cell RSRP RX 0/1";
    }

    @Override
    String getGroup() {
        return "5. LTE EM Component";
    }

    @Override
    int[] getEmType() {
        return EM_TYPES;
    }

    String[] getLabels() {
        return new String[] {"RSRP[0]", "RSRP[1]"};
    }

    @Override
    void update(int type, Parser data) {
        List<Parser> dlInfo = data.getArray("dl_info");
        List<Integer> dlRsrpList = dlInfo.get(0).getIntArray("dl_rsrp");
        int rsrp0 = dlRsrpList.get(0);
        int rsrp1 = dlRsrpList.get(1);
        clearData();
        addData(rsrp0);
        addData(rsrp1);
    }
}

class PrimaryCellRsrp extends NormalTableComponent {
    private static final int[] EM_TYPES = new int[] {Content.EM_EL1_INFO};

    public PrimaryCellRsrp(Activity context) {
        super(context);
    }

    @Override
    String getName() {
        return "Primary Cell RSRP";
    }

    @Override
    String getGroup() {
        return "5. LTE EM Component";
    }

    @Override
    int[] getEmType() {
        return EM_TYPES;
    }

    String[] getLabels() {
        return new String[] {"RSRP"};
    }

    @Override
    void update(int type, Parser data) {
        List<Parser> dlInfo = data.getArray("dl_info");
        int rsrp = dlInfo.get(0).getInt("rsrp");
        clearData();
        addData(rsrp);
    }
}

class PrimaryCellRsrq extends NormalTableComponent {
    private static final int[] EM_TYPES = new int[] {Content.EM_EL1_INFO};

    public PrimaryCellRsrq(Activity context) {
        super(context);
    }

    @Override
    String getName() {
        return "Primary Cell RSRQ";
    }

    @Override
    String getGroup() {
        return "5. LTE EM Component";
    }

    @Override
    int[] getEmType() {
        return EM_TYPES;
    }

    String[] getLabels() {
        return new String[] {"RSRQ"};
    }

    @Override
    void update(int type, Parser data) {
        List<Parser> dlInfo = data.getArray("dl_info");
        int rsrq = dlInfo.get(0).getInt("rsrq");
        clearData();
        addData(rsrq);
    }
}

class PrimaryCellRssiRx extends NormalTableComponent {
    private static final int[] EM_TYPES = new int[] {Content.EM_EL1_INFO};

    public PrimaryCellRssiRx(Activity context) {
        super(context);
    }

    @Override
    String getName() {
        return "Primary Cell RSSI RX 0/1";
    }

    @Override
    String getGroup() {
        return "5. LTE EM Component";
    }

    @Override
    int[] getEmType() {
        return EM_TYPES;
    }

    String[] getLabels() {
        return new String[] {"RSSI[0]", "RSSI[1]"};
    }

    @Override
    void update(int type, Parser data) {
        List<Parser> dlInfo = data.getArray("dl_info");
        List<Integer> dlRssiList = dlInfo.get(0).getIntArray("dl_rssi");
        int rssi0 = dlRssiList.get(0);
        int rssi1 = dlRssiList.get(1);
        clearData();
        addData(rssi0);
        addData(rssi1);
    }
}

class PrimaryCellSnrRx extends NormalTableComponent {
    private static final int[] EM_TYPES = new int[] {Content.EM_EL1_INFO};

    public PrimaryCellSnrRx(Activity context) {
        super(context);
    }

    @Override
    String getName() {
        return "Primary Cell SNR RX 0/1";
    }

    @Override
    String getGroup() {
        return "5. LTE EM Component";
    }

    @Override
    int[] getEmType() {
        return EM_TYPES;
    }

    String[] getLabels() {
        return new String[] {"SINR[0]", "SINR[1]"};
    }

    @Override
    void update(int type, Parser data) {
        List<Parser> dlInfo = data.getArray("dl_info");
        List<Integer> dlSnrList = dlInfo.get(0).getIntArray("dl_sinr");
        int snr0 = dlSnrList.get(0);
        int snr1 = dlSnrList.get(1);
        clearData();
        addData(snr0);
        addData(snr1);
    }
}

class PrimaryCellOsSnr extends NormalTableComponent {
    private static final int[] EM_TYPES = new int[] {Content.EM_EL1_INFO};

    public PrimaryCellOsSnr(Activity context) {
        super(context);
    }

    @Override
    String getName() {
        return "Primary Cell OS SNR";
    }

    @Override
    String getGroup() {
        return "5. LTE EM Component";
    }

    @Override
    int[] getEmType() {
        return EM_TYPES;
    }

    String[] getLabels() {
        return new String[] {"OS SNR"};
    }

    @Override
    void update(int type, Parser data) {
        List<Parser> dlInfo = data.getArray("dl_info");
        int osSnr = dlInfo.get(0).getInt("rsSNR");
        clearData();
        addData(osSnr);
    }
}


class SecondaryCellRsrpRx extends NormalTableComponent {
    private static final int[] EM_TYPES = new int[] {Content.EM_EL1_INFO};

    public SecondaryCellRsrpRx(Activity context) {
        super(context);
    }

    @Override
    String getName() {
        return "Secondary Cell RSRP RX 0/1";
    }

    @Override
    String getGroup() {
        return "5. LTE EM Component";
    }

    @Override
    int[] getEmType() {
        return EM_TYPES;
    }

    String[] getLabels() {
        return new String[] {"RSRP[0]", "RSRP[1]"};
    }

    @Override
    void update(int type, Parser data) {
        List<Parser> dlInfo = data.getArray("dl_info");
        List<Integer> dlRsrpList = dlInfo.get(1).getIntArray("dl_rsrp");
        int rsrp0 = dlRsrpList.get(0);
        int rsrp1 = dlRsrpList.get(1);
        clearData();
        addData(rsrp0);
        addData(rsrp1);
    }
}

class SecondaryCellRsrp extends NormalTableComponent {
    private static final int[] EM_TYPES = new int[] {Content.EM_EL1_INFO};

    public SecondaryCellRsrp(Activity context) {
        super(context);
    }

    @Override
    String getName() {
        return "Secondary Cell RSRP";
    }

    @Override
    String getGroup() {
        return "5. LTE EM Component";
    }

    @Override
    int[] getEmType() {
        return EM_TYPES;
    }

    String[] getLabels() {
        return new String[] {"RSRP"};
    }

    @Override
    void update(int type, Parser data) {
        List<Parser> dlInfo = data.getArray("dl_info");
        int rsrp = dlInfo.get(1).getInt("rsrp");
        clearData();
        addData(rsrp);
    }
}

class SecondaryCellRsrq extends NormalTableComponent {
    private static final int[] EM_TYPES = new int[] {Content.EM_EL1_INFO};

    public SecondaryCellRsrq(Activity context) {
        super(context);
    }

    @Override
    String getName() {
        return "Secondary Cell RSRQ";
    }

    @Override
    String getGroup() {
        return "5. LTE EM Component";
    }

    @Override
    int[] getEmType() {
        return EM_TYPES;
    }

    String[] getLabels() {
        return new String[] {"RSRQ"};
    }

    @Override
    void update(int type, Parser data) {
        List<Parser> dlInfo = data.getArray("dl_info");
        int rsrq = dlInfo.get(1).getInt("rsrq");
        clearData();
        addData(rsrq);
    }
}

class SecondaryCellRssiRx extends NormalTableComponent {
    private static final int[] EM_TYPES = new int[] {Content.EM_EL1_INFO};

    public SecondaryCellRssiRx(Activity context) {
        super(context);
    }

    @Override
    String getName() {
        return "Secondary Cell RSSI RX 0/1";
    }

    @Override
    String getGroup() {
        return "5. LTE EM Component";
    }

    @Override
    int[] getEmType() {
        return EM_TYPES;
    }

    String[] getLabels() {
        return new String[] {"RSSI[0]", "RSSI[1]"};
    }

    @Override
    void update(int type, Parser data) {
        List<Parser> dlInfo = data.getArray("dl_info");
        List<Integer> dlRssiList = dlInfo.get(1).getIntArray("dl_rssi");
        int rssi0 = dlRssiList.get(0);
        int rssi1 = dlRssiList.get(1);
        clearData();
        addData(rssi0);
        addData(rssi1);
    }
}

class SecondaryCellSnrRx extends NormalTableComponent {
    private static final int[] EM_TYPES = new int[] {Content.EM_EL1_INFO};

    public SecondaryCellSnrRx(Activity context) {
        super(context);
    }

    @Override
    String getName() {
        return "Secondary Cell SNR RX 0/1";
    }

    @Override
    String getGroup() {
        return "5. LTE EM Component";
    }

    @Override
    int[] getEmType() {
        return EM_TYPES;
    }

    String[] getLabels() {
        return new String[] {"SINR[0]", "SINR[1]"};
    }

    @Override
    void update(int type, Parser data) {
        List<Parser> dlInfo = data.getArray("dl_info");
        List<Integer> dlSnrList = dlInfo.get(1).getIntArray("dl_sinr");
        int snr0 = dlSnrList.get(0);
        int snr1 = dlSnrList.get(1);
        clearData();
        addData(snr0);
        addData(snr1);
    }
}

class SecondaryCellOsSnr extends NormalTableComponent {
    private static final int[] EM_TYPES = new int[] {Content.EM_EL1_INFO};

    public SecondaryCellOsSnr(Activity context) {
        super(context);
    }

    @Override
    String getName() {
        return "Secondary Cell OS SNR";
    }

    @Override
    String getGroup() {
        return "5. LTE EM Component";
    }

    @Override
    int[] getEmType() {
        return EM_TYPES;
    }

    String[] getLabels() {
        return new String[] {"OS SNR"};
    }

    @Override
    void update(int type, Parser data) {
        List<Parser> dlInfo = data.getArray("dl_info");
        int osSnr = dlInfo.get(1).getInt("rsSNR");
        clearData();
        addData(osSnr);
    }
}

class SecondaryCellCqi extends CurveComponent {
    private static final int[] EM_TYPES = new int[] {Content.EM_EL1_INFO};

    public SecondaryCellCqi(Activity context) {
        super(context);
    }

    @Override
    String getName() {
        return "Secondary Cell CQI";
    }

    @Override
    String getGroup() {
        return "5. LTE EM Component";
    }

    @Override
    int[] getEmType() {
        return EM_TYPES;
    }

    @Override
    CurveView.AxisConfig configY() {
//        mYLabel.setText("RSCP (dBm)");
        mYLabel.setText("R");

        CurveView.Config[] configs = new CurveView.Config[2];
        configs[0] = new CurveView.Config();
        configs[0].color = 0xFF0000FF;
        configs[0].lineWidth = 3;
        configs[0].nodeType = CurveView.Config.TYPE_CIRCLE;
        configs[0].name = "CQI_CW0";

        configs[1] = new CurveView.Config();
        configs[1].color = 0xFF00FF00;
        configs[1].lineWidth = 3;
        configs[1].lineType = CurveView.Config.LINE_DASH;
        configs[1].nodeType = CurveView.Config.TYPE_CROSS;
        configs[1].name = "CQI_CW1";
        mCurveView.setConfig(configs);

        CurveView.AxisConfig yConfig = new CurveView.AxisConfig();
        yConfig.min = -100;
        yConfig.max = 100;
        yConfig.step = 10;
        yConfig.configMin = true;
        yConfig.configMax = true;
        yConfig.configStep = true;
        return yConfig;
    }

    @Override
    void update(int type, Parser data) {
        List<Parser> dlInfo = data.getArray("dl_info");
        int cw0 = dlInfo.get(1).getInt("cqi_cw0");
        int cw1 = dlInfo.get(1).getInt("cqi_cw1");
        addData(0, cw0);
        addData(0, cw1);
    }
}

class SecondaryCellDlImcs extends NormalTableComponent {
    private static final int[] EM_TYPES = new int[] {Content.EM_EL1_INFO};

    public SecondaryCellDlImcs(Activity context) {
        super(context);
    }

    @Override
    String getName() {
        return "Secondary Cell DL Imcs";
    }

    @Override
    String getGroup() {
        return "5. LTE EM Component";
    }

    @Override
    int[] getEmType() {
        return EM_TYPES;
    }

    String[] getLabels() {
        return new String[] {"DL_Imcs"};
    }

    @Override
    void update(int type, Parser data) {
        List<Parser> dlInfo = data.getArray("dl_info");
        int value = dlInfo.get(1).getInt("DL_Imcs");
        clearData();
        addData(value);
    }
}

class SecondaryCellDlResourceBlock extends CurveComponent {
    private static final int[] EM_TYPES = new int[] {Content.EM_EL1_INFO};

    public SecondaryCellDlResourceBlock(Activity context) {
        super(context);
    }

    @Override
    String getName() {
        return "Secondary Cell DL Resource Block";
    }

    @Override
    String getGroup() {
        return "5. LTE EM Component";
    }

    @Override
    int[] getEmType() {
        return EM_TYPES;
    }

    @Override
    CurveView.AxisConfig configY() {
//        mYLabel.setText("RSCP (dBm)");
        mYLabel.setText("R");

        CurveView.Config[] configs = new CurveView.Config[1];
        configs[0] = new CurveView.Config();
        configs[0].color = 0xFF0000FF;
        configs[0].lineWidth = 3;
        configs[0].nodeType = CurveView.Config.TYPE_CIRCLE;
        configs[0].name = "DL Resource Block";
        mCurveView.setConfig(configs);

        CurveView.AxisConfig yConfig = new CurveView.AxisConfig();
        yConfig.min = -100;
        yConfig.max = 100;
        yConfig.step = 10;
        yConfig.configMin = true;
        yConfig.configMax = true;
        yConfig.configStep = true;
        return yConfig;
    }

    @Override
    void update(int type, Parser data) {
        List<Parser> dlInfo = data.getArray("dl_info");
        int value = dlInfo.get(1).getInt("DL_rb");
        addData(0, value);
    }
}

class SecondaryCellAntennaPort extends NormalTableComponent {
    private static final int[] EM_TYPES = new int[] {Content.EM_EL1_INFO};

    public SecondaryCellAntennaPort(Activity context) {
        super(context);
    }

    @Override
    String getName() {
        return "Secondary Cell Antenna Port";
    }

    @Override
    String getGroup() {
        return "5. LTE EM Component";
    }

    @Override
    int[] getEmType() {
        return EM_TYPES;
    }

    String[] getLabels() {
        return new String[] {"ANT_PORT"};
    }

    @Override
    void update(int type, Parser data) {
        List<Parser> dlInfo = data.getArray("cell_info");
        int value = dlInfo.get(1).getInt("ant_port");
        clearData();
        addData(value);
    }
}

class SecondaryCellDlThroughput extends CurveComponent {
    private static final int[] EM_TYPES = new int[] {Content.EM_EL1_INFO};

    public SecondaryCellDlThroughput(Activity context) {
        super(context);
    }

    @Override
    String getName() {
        return "Secondary Cell DL Throughput";
    }

    @Override
    String getGroup() {
        return "5. LTE EM Component";
    }

    @Override
    int[] getEmType() {
        return EM_TYPES;
    }

    @Override
    CurveView.AxisConfig configY() {
//        mYLabel.setText("RSCP (dBm)");
        mYLabel.setText("R");

        CurveView.Config[] configs = new CurveView.Config[1];
        configs[0] = new CurveView.Config();
        configs[0].color = 0xFF0000FF;
        configs[0].lineWidth = 3;
        configs[0].nodeType = CurveView.Config.TYPE_CIRCLE;
        configs[0].name = "DL Resource Block";
        mCurveView.setConfig(configs);

        CurveView.AxisConfig yConfig = new CurveView.AxisConfig();
        yConfig.min = -100;
        yConfig.max = 100;
        yConfig.step = 10;
        yConfig.configMin = true;
        yConfig.configMax = true;
        yConfig.configStep = true;
        return yConfig;
    }

    @Override
    void update(int type, Parser data) {
        List<Parser> dlInfo = data.getArray("dl_info");
        long value = dlInfo.get(1).getLong("DL_Tput");
        addData(0, value);
    }
}

class SecondaryCellDlBlockRate extends CurveComponent {
    private static final int[] EM_TYPES = new int[] {Content.EM_EL1_INFO};

    public SecondaryCellDlBlockRate(Activity context) {
        super(context);
    }

    @Override
    String getName() {
        return "Secondary Cell DL Block Rate";
    }

    @Override
    String getGroup() {
        return "5. LTE EM Component";
    }

    @Override
    int[] getEmType() {
        return EM_TYPES;
    }

    @Override
    CurveView.AxisConfig configY() {
//        mYLabel.setText("RSCP (dBm)");
        mYLabel.setText("R");

        CurveView.Config[] configs = new CurveView.Config[1];
        configs[0] = new CurveView.Config();
        configs[0].color = 0xFF0000FF;
        configs[0].lineWidth = 3;
        configs[0].nodeType = CurveView.Config.TYPE_CIRCLE;
        configs[0].name = "DL Resource Block";
        mCurveView.setConfig(configs);

        CurveView.AxisConfig yConfig = new CurveView.AxisConfig();
        yConfig.min = -100;
        yConfig.max = 100;
        yConfig.step = 10;
        yConfig.configMin = true;
        yConfig.configMax = true;
        yConfig.configStep = true;
        return yConfig;
    }

    @Override
    void update(int type, Parser data) {
        List<Parser> dlInfo = data.getArray("dl_info");
        int value = dlInfo.get(1).getInt("DL_block");
        addData(0, value);
    }
}

class DownlinkDataThroughput extends NormalTableComponent {
    private static final int[] EM_TYPES = new int[] {Content.EM_UPCM_PS_TPUT_INFO};

    public DownlinkDataThroughput(Activity context) {
        super(context);
    }

    @Override
    String getName() {
        return "Downlink Data Throughput";
    }

    @Override
    String getGroup() {
        return "5. LTE EM Component";
    }

    @Override
    int[] getEmType() {
        return EM_TYPES;
    }

    String[] getLabels() {
        return new String[] {"IP Rate DL (bytes/s)"};
    }

    @Override
    void update(int type, Parser data) {
        long value = data.getLong("total_rx_byte_per_second");
        clearData();
        addData(value);
    }
}

class UplinkDataThroughput extends NormalTableComponent {
    private static final int[] EM_TYPES = new int[] {Content.EM_UPCM_PS_TPUT_INFO};

    public UplinkDataThroughput(Activity context) {
        super(context);
    }

    @Override
    String getName() {
        return "Uplink Data Throughput";
    }

    @Override
    String getGroup() {
        return "5. LTE EM Component";
    }

    @Override
    int[] getEmType() {
        return EM_TYPES;
    }

    String[] getLabels() {
        return new String[] {"IP Rate UL (bytes/s)"};
    }

    @Override
    void update(int type, Parser data) {
        long value = data.getLong("total_tx_byte_per_second");
        clearData();
        addData(value);
    }
}

class HandoverIntraLte extends NormalTableComponent {
    private static final int[] EM_TYPES = new int[] {Content.ERRC_EM_ERRC_KPI_INFO};

    public HandoverIntraLte(Activity context) {
        super(context);
    }

    @Override
    String getName() {
        return "Handover (Intra-LTE)";
    }

    @Override
    String getGroup() {
        return "5. LTE EM Component";
    }

    @Override
    int[] getEmType() {
        return EM_TYPES;
    }

    String[] getLabels() {
        return new String[] {"Attempt", "Success", "Fail"};
    }

    @Override
    void update(int type, Parser data) {
        long attempt = data.getLong("attempt");
        long success = data.getLong("success");
        clearData();
        addData(attempt, success, attempt - success);
    }
}
/*
class RatAndServiceStatusTable extends NormalTableComponent {
    private static final int[] EM_TYPES = new int[] {Content.RAC_EM_INFO};
    HashMap<Integer, String> mMapping = new HashMap<Integer, String>() {{
        put(0, "NO SERVICE");
        put(1, "LIMITED SERVICE");
        put(2, "GSM");
        put(3, "UMTS FDD");
        put(4, "UMTS TDD");
        put(5, "LTE  FDD");
        put(6, "LTE  TDD");
    }};

    public RatAndServiceStatusTable(Activity context) {
        super(context);
    }

    @Override
    String getName() {
        return "RAT and Service Status";
    }

    @Override
    String getGroup() {
        return "5. LTE EM Component";
    }

    @Override
    int[] getEmType() {
        return EM_TYPES;
    }

    String[] getLabels() {
        return new String[] {"Service status"};
    }

    @Override
    void update(int type, Parser data) {
        String ratInfo = mMapping.get(data.getInt("active_rat_info"));
        clearData();
        addData(ratInfo);
    }
}
*/
class RatAndServiceStatus extends CurveComponent {
    private static final int[] EM_TYPES = new int[] {Content.RAC_EM_INFO};
    HashMap<Integer, String> mMapping = new HashMap<Integer, String>() { {
        put(0, "NO SERVICE");
        put(1, "LIMITED SERVICE");
        put(2, "GSM");
        put(3, "UMTS FDD");
        put(4, "UMTS TDD");
        put(5, "LTE  FDD");
        put(6, "LTE  TDD");
    } };

    public RatAndServiceStatus(Activity context) {
        super(context);
    }

    @Override
    String getName() {
        return "RAT and Service Status";
    }

    @Override
    String getGroup() {
        return "5. LTE EM Component";
    }

    @Override
    int[] getEmType() {
        return EM_TYPES;
    }

    @Override
    CurveView.AxisConfig configY() {
//        mYLabel.setText("RSCP (dBm)");
        mYLabel.setText("R");

        CurveView.Config[] configs = new CurveView.Config[1];
        configs[0] = new CurveView.Config();
        configs[0].color = 0xFF0000FF;
        configs[0].lineWidth = 3;
        configs[0].nodeType = CurveView.Config.TYPE_CIRCLE;
        configs[0].name = "Service status";
        mCurveView.setConfig(configs);

        CurveView.AxisConfig yConfig = new CurveView.AxisConfig();
        yConfig.min = 0;
        yConfig.max = 7;
        yConfig.step = 1;
        yConfig.configMin = true;
        yConfig.configMax = true;
        yConfig.configStep = true;
        yConfig.customLabel = mMapping;
        return yConfig;
    }

    @Override
    void update(int type, Parser data) {
//        String ratInfo = mMapping.get(data.getInt("active_rat_info"));
        addData(0, data.getInt("active_rat_info"));
    }
}

class HsDschServingCellUmtsFdd extends NormalTableComponent {
    private static final int[] EM_TYPES = new int[] {
            Content.EM_FDD_MEME_INFO_DCH_H_SERVING_CELL_INFO};

    public HsDschServingCellUmtsFdd(Activity context) {
        super(context);
    }

    @Override
    String getName() {
        return "HS-DSCH Serving cell (UMTS FDD)";
    }

    @Override
    String getGroup() {
        return "5. LTE EM Component";
    }

    @Override
    int[] getEmType() {
        return EM_TYPES;
    }

    String[] getLabels() {
        return new String[] {"HS-DSCH Serving Cell"};
    }

    @Override
    void update(int type, Parser data) {
        int uarfcn = data.getInt("HSDSCH_Serving_UARFCN");
        int psc = data.getInt("HSDSCH_Serving_PSC");
        String value = "";
        value += (uarfcn == 0xFFFF ? "-" : uarfcn);
        value += " / " + (psc == 0xFFFF ? "-" : psc);
        clearData();
        addData(value);
    }
}

class SecondHsDschServingCell extends NormalTableComponent {
    private static final int[] EM_TYPES = new int[] {Content.UL1_EM_HSPA_INFO_GROUP};

    public SecondHsDschServingCell(Activity context) {
        super(context);
    }

    @Override
    String getName() {
        return "2nd HS-DSCH Serving Cell";
    }

    @Override
    String getGroup() {
        return "5. LTE EM Component";
    }

    @Override
    int[] getEmType() {
        return EM_TYPES;
    }

    String[] getLabels() {
        return new String[] {"Dual Cell", "Freq", "PSC", "64 QAM"};
    }

    @Override
    void update(int type, Parser data) {
        data = data.get("secondary_hs_dsch_configuration_status");
        boolean dcOn = data.getBoolean("dc_on");
        int freq = data.getInt("dl_freq");
        int psc = data.getInt("psc");
        boolean dlOn = data.getBoolean("dl_64QAM_on");
        clearData();
        addData(dcOn ? 1 : 0);
        addData(freq);
        addData(psc);
        addData(dlOn ? 1 : 0);
    }
}

class BasicInfoServingGsm extends NormalTableComponent {
    private static final int[] EM_TYPES = new int[] {Content.RR_EM_MEASUREMENT_REPORT_INFO};
    HashMap<Integer, String> mMapping = new HashMap<Integer, String>() { {
        put(0, "PGSM");
        put(1, "EGSM");
        put(2, "RGSM");
        put(3, "DCS1800");
        put(4, "PCS1900");
        put(5, "GSM450");
        put(6, "GSM480");
        put(7, "GSM850");
    } };
    HashMap<Integer, String> mGprsMapping = new HashMap<Integer, String>() { {
        put(0, "PGSM");
        put(1, "EGSM");
    } };
    HashMap<Integer, String> mPbcchMapping = new HashMap<Integer, String>() { {
        put(0, "PGSM");
        put(1, "EGSM");
    } };

    public BasicInfoServingGsm(Activity context) {
        super(context);
    }

    @Override
    String getName() {
        return "Basic Info (Serving) (GSM)";
    }

    @Override
    String getGroup() {
        return "2. GSM EM Component";
    }

    @Override
    int[] getEmType() {
        return EM_TYPES;
    }

    String[] getLabels() {
        return new String[] {"Band", "ARFCN", "BSIC", "GPRS supported", "PBCCH present"};
    }

    @Override
    void update(int type, Parser data) {
        int rrState = data.getInt("rr_state");
        clearData();
        if (rrState >= 3 && rrState <= 7) {
            String arfcn = mMapping.get(data.getInt("serving_arfcn"));
            int bsic = data.getInt("serving_bsic");
            int band = data.getInt("serving_current_band");
            int gprs = data.getInt("serv_gprs_supported");
            boolean pbcch = data.getBoolean("gprs_pbcch_present");
            addData(arfcn == null ? "-" : arfcn);
            addData(bsic);
            addData(band);
            addData(mGprsMapping.get(gprs));
            addData(mPbcchMapping.get(pbcch));
        }
        notifyDataSetChanged();
    }
}

class CaInfo extends ArrayTableComponent {
    private static final int[] EM_TYPES = new int[] {Content.ERRC_EM_CONN_INFO};

    public CaInfo(Activity context) {
        super(context);
    }

    @Override
    String getName() {
        return "Secondary Cell";
    }

    @Override
    String getGroup() {
        return "5. LTE EM Component";
    }

    @Override
    int[] getEmType() {
        return EM_TYPES;
    }

    String[] getLabels() {
        return new String[] {"EARFCN", "PSC", "Band", "Bandwidth"};
    }

    @Override
    void update(int type, Parser data) {
        clearData();
        List<Boolean> configured = data.getBooleanArray("is_scell_configured");
        List<Long> earfcn = data.getLongArray("scell_earfcn");
        List<Integer> band = data.getIntArray("scell_band");
        List<Integer> pci = data.getIntArray("scell_pci");
        List<Integer> bw = data.getIntArray("scell_bw");
        for (int i = 0; i < configured.size(); i++) {
            if (configured.get(i)) {
                addData(earfcn.get(i), pci.get(i), band.get(i), (float) bw.get(i) / 10);
            } else {
                addData("-", "", "", "");
            }
        }
    }
}

class RsrpLteCandidateCellUmtsFddArray extends ArrayTableComponent {
    private static final int[] EM_TYPES = new int[] {
            Content.FDD_EM_CSCE_NEIGH_CELL_IND_STRUCT_INFO};

    public RsrpLteCandidateCellUmtsFddArray(Activity context) {
        super(context);
    }

    @Override
    String getName() {
        return "RSRP (LTE candidate cell)(UMTS FDD)";
    }

    @Override
    String getGroup() {
        return "5. LTE EM Component";
    }

    @Override
    int[] getEmType() {
        return EM_TYPES;
    }

    String[] getLabels() {
        return new String[] {"RSRP (LTE)", "Earfcn", "PCI", "RSRP"};
    }

    @Override
    void update(int type, Parser data) {
        int rat = data.getInt("RAT_type");
        if (rat == 2) {
            int num = data.getInt("neigh_cell_count");
            data = data.get("choice");
            List<Parser> cellList = data.getArray("LTE_neigh_cells");
            for (int i = 0; i < num && i < cellList.size(); i++) {
                data = cellList.get(i);
                long earfcn = data.getLong("earfcn");
                int pci = data.getInt("pci");
                float rsrp = (float) data.getLong("rsrp") / 4096;
                addData(earfcn, pci, rsrp);
            }
        }
    }
}

class RsrpLteCandidateCellUmtsFddCurve extends CurveComponent {
    private static final int[] EM_TYPES = new int[] {
            Content.FDD_EM_CSCE_NEIGH_CELL_IND_STRUCT_INFO};

    public RsrpLteCandidateCellUmtsFddCurve(Activity context) {
        super(context);
    }

    @Override
    String getName() {
        return "RSRP (LTE candidate cell)(UMTS FDD)";
    }

    @Override
    String getGroup() {
        return "5. LTE EM Component";
    }

    @Override
    int[] getEmType() {
        return EM_TYPES;
    }

    @Override
    CurveView.AxisConfig configY() {
//        mYLabel.setText("RSCP (dBm)");
        mYLabel.setText("R");

        int[] colors = new int[] {
                0xFF0000FF, 0xFF00FF00, 0xFFFF0000, 0xFF00FFFF,
                0xFFFF00FF, 0xFFFFFF00, 0xFF000088, 0xFF008800,
                0xFF880000, 0xFF008888, 0xFF880088, 0xFF888800,
                0xFF000044, 0xFF004400, 0xFF440000, 0xFF000000,
        };
        CurveView.Config[] configs = new CurveView.Config[16];
        for (int i = 0; i < 16; i++) {
            configs[i] = new CurveView.Config();
            configs[i].color = colors[i];
            configs[i].lineWidth = 3;
            configs[i].nodeType = CurveView.Config.TYPE_CIRCLE;
            configs[i].name = "";
        }
        mCurveView.setConfig(configs);

        CurveView.AxisConfig yConfig = new CurveView.AxisConfig();
        yConfig.min = -100;
        yConfig.max = 100;
        yConfig.step = 10;
        yConfig.configMin = true;
        yConfig.configMax = true;
        yConfig.configStep = true;
        return yConfig;
    }

    @Override
    void update(int type, Parser data) {
        int rat = data.getInt("RAT_type");
        if (rat == 2) {
            int num = data.getInt("neigh_cell_count");
            data = data.get("choice");
            List<Parser> cellList = data.getArray("LTE_neigh_cells");
            for (int i = 0; i < num && i < cellList.size(); i++) {
                float rsrp = (float) data.getLong("rsrp") / 4096;
                addData(i, rsrp);
            }
        }
    }
}

class RsrpLteCandidateCellUmtsFdd extends Component {
    private static final int[] EM_TYPES = new int[] {
            Content.FDD_EM_CSCE_NEIGH_CELL_IND_STRUCT_INFO};
    private RsrpLteCandidateCellUmtsFddArray mArrayComponent;
    private RsrpLteCandidateCellUmtsFddCurve mCurveComponent;
    private LinearLayout mComponentView;

    public RsrpLteCandidateCellUmtsFdd(Activity context) {
        super(context);
        mArrayComponent = new RsrpLteCandidateCellUmtsFddArray(context);
        mCurveComponent = new RsrpLteCandidateCellUmtsFddCurve(context);
    }

    @Override
    View getView() {
        if (mComponentView == null) {
            mComponentView = new LinearLayout(mActivity);
            mComponentView.setOrientation(LinearLayout.VERTICAL);
        }
        LinearLayout.LayoutParams layoutParams = new LinearLayout.LayoutParams(
                LinearLayout.LayoutParams.MATCH_PARENT, 0, 1);
        LinearLayout.LayoutParams layoutParams2 = new LinearLayout.LayoutParams(
                LinearLayout.LayoutParams.MATCH_PARENT, 0, 2);
        mComponentView.addView(mArrayComponent.getView(), layoutParams);
        mComponentView.addView(mCurveComponent.getView(), layoutParams2);
        return mComponentView;
    }

    @Override
    void removeView() {
        mArrayComponent.removeView();
        mCurveComponent.removeView();
        mComponentView.removeAllViews();
    }

    @Override
    String getName() {
        return "RSRP (LTE candidate cell)(UMTS FDD)";
    }

    @Override
    String getGroup() {
        return "5. LTE EM Component";
    }

    @Override
    int[] getEmType() {
        return EM_TYPES;
    }

    @Override
    void update(int type, Parser data) {
        mArrayComponent.update(type, data);
        mCurveComponent.update(type, data);
    }
}

class RsrqLteCandidateCellUmtsFddArray extends ArrayTableComponent {
    private static final int[] EM_TYPES = new int[] {
            Content.FDD_EM_CSCE_NEIGH_CELL_IND_STRUCT_INFO};

    public RsrqLteCandidateCellUmtsFddArray(Activity context) {
        super(context);
    }

    @Override
    String getName() {
        return "RSRQ (LTE candidate cell)(UMTS FDD)";
    }

    @Override
    String getGroup() {
        return "5. LTE EM Component";
    }

    @Override
    int[] getEmType() {
        return EM_TYPES;
    }

    String[] getLabels() {
        return new String[] {"RSRP (LTE)", "Earfcn", "PCI", "RSRQ"};
    }

    @Override
    void update(int type, Parser data) {
        int rat = data.getInt("RAT_type");
        if (rat == 2) {
            int num = data.getInt("neigh_cell_count");
            data = data.get("choice");
            List<Parser> cellList = data.getArray("LTE_neigh_cells");
            for (int i = 0; i < num && i < cellList.size(); i++) {
                data = cellList.get(i);
                long earfcn = data.getLong("earfcn");
                int pci = data.getInt("pci");
                float rsrq = (float) data.getLong("rsrq") / 4096;
                addData(earfcn, pci, rsrq);
            }
        }
    }
}

class RsrqLteCandidateCellUmtsFddCurve extends CurveComponent {
    private static final int[] EM_TYPES = new int[] {
            Content.FDD_EM_CSCE_NEIGH_CELL_IND_STRUCT_INFO};

    public RsrqLteCandidateCellUmtsFddCurve(Activity context) {
        super(context);
    }

    @Override
    String getName() {
        return "RSRQ (LTE candidate cell)(UMTS FDD)";
    }

    @Override
    String getGroup() {
        return "5. LTE EM Component";
    }

    @Override
    int[] getEmType() {
        return EM_TYPES;
    }

    @Override
    CurveView.AxisConfig configY() {
//        mYLabel.setText("RSCP (dBm)");
        mYLabel.setText("R");

        int[] colors = new int[] {
                0xFF0000FF, 0xFF00FF00, 0xFFFF0000, 0xFF00FFFF,
                0xFFFF00FF, 0xFFFFFF00, 0xFF000088, 0xFF008800,
                0xFF880000, 0xFF008888, 0xFF880088, 0xFF888800,
                0xFF000044, 0xFF004400, 0xFF440000, 0xFF000000,
        };
        CurveView.Config[] configs = new CurveView.Config[16];
        for (int i = 0; i < 16; i++) {
            configs[i] = new CurveView.Config();
            configs[i].color = colors[i];
            configs[i].lineWidth = 3;
            configs[i].nodeType = CurveView.Config.TYPE_CIRCLE;
            configs[i].name = "";
        }
        mCurveView.setConfig(configs);

        CurveView.AxisConfig yConfig = new CurveView.AxisConfig();
        yConfig.min = -100;
        yConfig.max = 100;
        yConfig.step = 10;
        yConfig.configMin = true;
        yConfig.configMax = true;
        yConfig.configStep = true;
        return yConfig;
    }

    @Override
    void update(int type, Parser data) {
        int rat = data.getInt("RAT_type");
        if (rat == 2) {
            int num = data.getInt("neigh_cell_count");
            data = data.get("choice");
            List<Parser> cellList = data.getArray("LTE_neigh_cells");
            for (int i = 0; i < num && i < cellList.size(); i++) {
                float rsrq = (float) data.getLong("rsrq") / 4096;
                addData(i, rsrq);
            }
        }
    }
}

class RsrqLteCandidateCellUmtsFdd extends Component {
    private static final int[] EM_TYPES = new int[] {
            Content.FDD_EM_CSCE_NEIGH_CELL_IND_STRUCT_INFO};
    private RsrqLteCandidateCellUmtsFddArray mArrayComponent;
    private RsrqLteCandidateCellUmtsFddCurve mCurveComponent;
    private LinearLayout mComponentView;

    public RsrqLteCandidateCellUmtsFdd(Activity context) {
        super(context);
        mArrayComponent = new RsrqLteCandidateCellUmtsFddArray(context);
        mCurveComponent = new RsrqLteCandidateCellUmtsFddCurve(context);
    }

    @Override
    View getView() {
        if (mComponentView == null) {
            mComponentView = new LinearLayout(mActivity);
            mComponentView.setOrientation(LinearLayout.VERTICAL);
        }
        LinearLayout.LayoutParams layoutParams = new LinearLayout.LayoutParams(
                LinearLayout.LayoutParams.MATCH_PARENT, 0, 1);
        LinearLayout.LayoutParams layoutParams2 = new LinearLayout.LayoutParams(
                LinearLayout.LayoutParams.MATCH_PARENT, 0, 2);
        mComponentView.addView(mArrayComponent.getView(), layoutParams);
        mComponentView.addView(mCurveComponent.getView(), layoutParams2);
        return mComponentView;
    }

    @Override
    void removeView() {
        mArrayComponent.removeView();
        mCurveComponent.removeView();
        mComponentView.removeAllViews();
    }

    @Override
    String getName() {
        return "RSRQ (LTE candidate cell)(UMTS FDD)";
    }

    @Override
    String getGroup() {
        return "5. LTE EM Component";
    }

    @Override
    int[] getEmType() {
        return EM_TYPES;
    }

    @Override
    void update(int type, Parser data) {
        mArrayComponent.update(type, data);
        mCurveComponent.update(type, data);
    }
}

class UmtsRscp extends CurveComponent {
    private static final int[] EM_TYPES = new int[] {
            Content.FDD_EM_CSCE_SERV_CELL_IND_STRUCT_INFO,
            Content.FDD_EM_CSCE_NEIGH_CELL_IND_STRUCT_INFO,
            Content.EM_FDD_MEME_INFO_DCH_UMTS_CELL_INFO};

    public UmtsRscp(Activity context) {
        super(context);
    }

    @Override
    String getName() {
        return "UMTS RSCP (UMTS FDD)";
    }

    @Override
    String getGroup() {
        return "3. UMTS FDD EM Component";
    }

    @Override
    int[] getEmType() {
        return EM_TYPES;
    }

    @Override
    CurveView.AxisConfig configY() {
//        mYLabel.setText("RSCP (dBm)");
        mYLabel.setText("R");

        CurveView.Config[] configs = new CurveView.Config[3];
        configs[0] = new CurveView.Config();
        configs[0].color = 0xFF0000FF;
        configs[0].lineWidth = 3;
        configs[0].nodeType = CurveView.Config.TYPE_CIRCLE;
        configs[0].name = "RSCP(active)";
        configs[0].newLineThreadshold = 11;
        configs[1] = new CurveView.Config();
        configs[1].color = 0xFF0000FF;
        configs[1].lineWidth = 0;
        configs[1].nodeType = CurveView.Config.TYPE_CIRCLE;
        configs[1].name = "RSCP(monitored)";
        configs[1].newLineThreadshold = 11;
        configs[2] = new CurveView.Config();
        configs[2].color = 0xFF0000FF;
        configs[2].lineWidth = 1;
        configs[2].lineType = CurveView.Config.LINE_DASH;
        configs[2].nodeType = CurveView.Config.TYPE_CIRCLE;
        configs[2].name = "RSCP(detected)";
        configs[2].newLineThreadshold = 11;
        mCurveView.setConfig(configs);

        CurveView.AxisConfig yConfig = new CurveView.AxisConfig();
        yConfig.min = -100;
        yConfig.max = 100;
        yConfig.step = 10;
        yConfig.configMin = true;
        yConfig.configMax = true;
        yConfig.configStep = true;
        return yConfig;
    }

    @Override
    void update(int type, Parser data) {
        if (type == 75) {
            float rscp = (float) data.getLong("rscp") / 4096;
            float ecno = (float) data.getLong("ec_no") / 4096;
            if (rscp > -120 && ecno > -25) {
                addData(0, rscp);
            }
        } else if (type == 76) {
            int operation = data.getInt("operation");
            int rat = data.getInt("RAT_type");
            if (operation == 1 && rat == 0) {
                int num = data.getInt("neigh_cell_count");
                data = data.get("choice");
                List<Parser> cellList = data.getArray("neigh_cells");
                for (int i = 0; i < num && i < cellList.size(); i++) {
                    data = cellList.get(i);
                    float rscp = (float) data.getLong("rscp") / 4096;
                    float ecno = (float) data.getLong("ec_no") / 4096;
                    if (rscp > -120 && ecno > -25) {
                        addData(1, rscp);
                    }
                }
            }
        } else if (type == 500) {
            int num = data.getInt("num_cells");
            List<Parser> cellList = data.getArray("umts_cell_list");
            for (int i = 0; i < num && i < cellList.size(); i++) {
                data = cellList.get(i);
                int cellType = data.getInt("cell_type");
                long rscp = data.getLong("RSCP");
                if (cellType == 0) {
                    addData(0, rscp);
                } else if (cellType == 1 || cellType == 3) {
                    addData(1, rscp);
                } else if (cellType == 2) {
                    addData(2, rscp);
                }
            }
        }
    }
}

class UmtsEcn0 extends CurveComponent {
    private static final int[] EM_TYPES = new int[] {
            Content.FDD_EM_CSCE_SERV_CELL_IND_STRUCT_INFO,
            Content.FDD_EM_CSCE_NEIGH_CELL_IND_STRUCT_INFO,
            Content.EM_FDD_MEME_INFO_DCH_UMTS_CELL_INFO};

    public UmtsEcn0(Activity context) {
        super(context);
    }

    @Override
    String getName() {
        return "UMTS ECN0 (UMTS FDD)";
    }

    @Override
    String getGroup() {
        return "3. UMTS FDD EM Component";
    }

    @Override
    int[] getEmType() {
        return EM_TYPES;
    }

    @Override
    CurveView.AxisConfig configY() {
//        mYLabel.setText("RSCP (dBm)");
        mYLabel.setText("R");

        CurveView.Config[] configs = new CurveView.Config[3];
        configs[0] = new CurveView.Config();
        configs[0].color = 0xFF0000FF;
        configs[0].lineWidth = 3;
        configs[0].nodeType = CurveView.Config.TYPE_CIRCLE;
        configs[0].name = "EcN0(active)";
        configs[0].newLineThreadshold = 11;
        configs[1] = new CurveView.Config();
        configs[1].color = 0xFF0000FF;
        configs[1].lineWidth = 0;
        configs[1].nodeType = CurveView.Config.TYPE_CIRCLE;
        configs[1].name = "EcN0(monitored)";
        configs[1].newLineThreadshold = 11;
        configs[2] = new CurveView.Config();
        configs[2].color = 0xFF0000FF;
        configs[2].lineWidth = 1;
        configs[2].lineType = CurveView.Config.LINE_DASH;
        configs[2].nodeType = CurveView.Config.TYPE_CIRCLE;
        configs[2].name = "EcN0(detected)";
        configs[2].newLineThreadshold = 11;
        mCurveView.setConfig(configs);

        CurveView.AxisConfig yConfig = new CurveView.AxisConfig();
        yConfig.min = -100;
        yConfig.max = 100;
        yConfig.step = 10;
        yConfig.configMin = true;
        yConfig.configMax = true;
        yConfig.configStep = true;
        return yConfig;
    }

    @Override
    void update(int type, Parser data) {
        if (type == 75) {
            float rscp = (float) data.getLong("rscp") / 4096;
            float ecn0 = (float) data.getLong("ec_no") / 4096;
            if (rscp > -120 && ecn0 > -25) {
                addData(0, ecn0);
            }
        } else if (type == 76) {
            int operation = data.getInt("operation");
            int rat = data.getInt("RAT_type");
            if (operation == 1 && rat == 0) {
                int num = data.getInt("neigh_cell_count");
                data = data.get("choice");
                List<Parser> cellList = data.getArray("neigh_cells");
                for (int i = 0; i < num && i < cellList.size(); i++) {
                    data = cellList.get(i);
                    float rscp = (float) data.getLong("rscp") / 4096;
                    float ecn0 = (float) data.getLong("ec_no") / 4096;
                    if (rscp > -120 && ecn0 > -25) {
                        addData(1, ecn0);
                    }
                }
            }
        } else if (type == 500) {
            int num = data.getInt("num_cells");
            List<Parser> cellList = data.getArray("umts_cell_list");
            for (int i = 0; i < num && i < cellList.size(); i++) {
                data = cellList.get(i);
                int cellType = data.getInt("cell_type");
                long ecn0 = data.getLong("RSCP");
                if (cellType == 0) {
                    addData(0, ecn0);
                } else if (cellType == 1 || cellType == 3) {
                    addData(1, ecn0);
                } else if (cellType == 2) {
                    addData(2, ecn0);
                }
            }
        }
    }
}

class IntraFrequencyNeighbouringCellInformationLteTdd extends ArrayTableComponent {
    private static final int[] EM_TYPES = new int[] {Content.ERRC_EM_MOB_MEAS_INTRARAT_INFO};

    public IntraFrequencyNeighbouringCellInformationLteTdd(Activity context) {
        super(context);
    }

    @Override
    String getName() {
        return "Intra-frequency neighbouring cell information (LTE TDD)";
    }

    @Override
    String getGroup() {
        return "5. LTE EM Component";
    }

    @Override
    int[] getEmType() {
        return EM_TYPES;
    }

    String[] getLabels() {
        return new String[] {"PCI", "RSCP", "ECN0"};
    }

    @Override
    void update(int type, Parser data) {
        clearData();
        data = data.get("intra_info");
        int cellNum = data.getInt("cell_num");
        List<Parser> cellList = data.getArray("intra_cell");
        for (int i = 0; i < cellNum && i < cellList.size(); i++) {
            data = cellList.get(i);
            boolean valid = data.getBoolean("valid");
            int pci = data.getInt("pci");
            int rsrp = data.getInt("rsrp");
            int rsrq = data.getInt("rsrq");
            addData((valid ? pci : ""),
                    (valid && rsrp != 0xFFFFFFFF ? (float) rsrp / 4 : ""),
                    (valid && rsrq != 0xFFFFFFFF ? (float) rsrq / 4 : ""));
        }
    }
}

class InterFrequencyNeighbouringCellInformationLteTdd extends ArrayTableComponent {
    private static final int[] EM_TYPES = new int[] {Content.ERRC_EM_MOB_MEAS_INTRARAT_INFO};

    public InterFrequencyNeighbouringCellInformationLteTdd(Activity context) {
        super(context);
    }

    @Override
    String getName() {
        return "Inter-frequency neighbouring cell information (LTE TDD)";
    }

    @Override
    String getGroup() {
        return "5. LTE EM Component";
    }

    @Override
    int[] getEmType() {
        return EM_TYPES;
    }

    String[] getLabels() {
        return new String[] {"EARFCN", "PCI", "RSCP", "ECN0"};
    }

    @Override
    void update(int type, Parser data) {
        clearData();
        data = data.get("inter_info");
        int freqNum = data.getInt("freq_num");
        List<Parser> freqList = data.getArray("inter_freq");
        for (int i = 0; i < freqNum && i < freqList.size(); i++) {
            data = freqList.get(i);
            boolean valid = data.getBoolean("valid");
            long earfcn = data.getLong("earfcn");
            int cellNum = data.getInt("cell_num");
            List<Parser> cellList = data.getArray("inter_cell");
            for (int j = 0; i < cellNum && j < cellList.size(); j++) {
                data = cellList.get(j);
                int pci = data.getInt("pci");
                int rsrp = data.getInt("rsrp");
                int rsrq = data.getInt("rsrq");
                addData((valid ? earfcn : ""),
                        (valid ? pci : ""),
                        (valid && rsrp != 0xFFFFFFFF ? (float) rsrp / 4 : ""),
                        (valid && rsrq != 0xFFFFFFFF ? (float) rsrq / 4 : ""));
            }
        }
    }
}

class GeranNeighbouringCellInformation extends ArrayTableComponent {
    private static final int[] EM_TYPES = new int[] {Content.ERRC_EM_MOB_MEAS_INTERRAT_GERAN_INFO};

    public GeranNeighbouringCellInformation(Activity context) {
        super(context);
    }

    @Override
    String getName() {
        return "GERAN neighbouring cell information";
    }

    @Override
    String getGroup() {
        return "5. LTE EM Component";
    }

    @Override
    int[] getEmType() {
        return EM_TYPES;
    }

    String[] getLabels() {
        return new String[] {"BAND INDICATOR", "ARFCN", "BSIC", "RSSI"};
    }

    @Override
    void update(int type, Parser data) {
        clearData();
        int cellNum = data.getInt("total_gcell_num");
        List<Parser> cellList = data.getArray("gcell");
        for (int i = 0; i < cellNum && i < cellList.size(); i++) {
            data = cellList.get(i);
            boolean valid = data.getBoolean("valid");
            boolean band = data.getBoolean("band_ind");
            int arfcn = data.getInt("arfcn");
            int bsic = data.getInt("bsic");
            int rssi = data.getInt("rssi");
            addData((valid ? band : ""),
                    (valid ? arfcn : ""),
                    (valid ? bsic : ""),
                    (valid ? (float) rssi / 4 : ""));
        }
    }
}

class UtraTddNeighbouringCellInformation extends ArrayTableComponent {
    private static final int[] EM_TYPES = new int[] {Content.ERRC_EM_MOB_MEAS_INTERRAT_UTRAN_INFO};

    public UtraTddNeighbouringCellInformation(Activity context) {
        super(context);
    }

    @Override
    String getName() {
        return "UTRA-TDD neighbouring cell information";
    }

    @Override
    String getGroup() {
        return "5. LTE EM Component";
    }

    @Override
    int[] getEmType() {
        return EM_TYPES;
    }

    String[] getLabels() {
        return new String[] {"UARFCN", "PSC", "RSCP", "EcN0"};
    }

    @Override
    void update(int type, Parser data) {
        clearData();
        int freqNum = data.getInt("freq_num");
        List<Parser> freqList = data.getArray("inter_freq");
        for (int i = 0; i < freqNum && i < freqList.size(); i++) {
            data = freqList.get(i);
            boolean valid = data.getBoolean("valid");
            int uarfcn = data.getInt("uarfcn");
            int cellNum = data.getInt("ucell_num");
            List<Parser> cellList = data.getArray("ucell");
            for (int j = 0; j < cellNum && j < cellList.size(); j++) {
                data = cellList.get(i);
                boolean valid2 = data.getBoolean("valid");
                int psc = data.getInt("psc");
                int rscp = data.getInt("rscp");
                int ecn0 = data.getInt("ec_n0");
                addData((valid ? uarfcn : ""),
                        (valid2 ? psc : ""),
                        (valid2 && rscp != 0xFFFFFFFF ? (float) rscp / 4 : ""),
                        (valid2 && ecn0 != 0xFFFFFFFF ? (float) ecn0 / 4 : ""));
            }
        }
    }
}

class ActivePdpContextInformation extends NormalTableComponent {
    private static final int MSG_QUERY_CID = 1;
    private static final int MSG_ADDR = 2;
    private static final int MSG_QOS = 3;
    private static final int MSG_APN = 4;

    public ActivePdpContextInformation(Activity context) {
        super(context);
    }

    @Override
    String getName() {
        return "Active PDP Context Information";
    }

    @Override
    String getGroup() {
        return "1. General EM Component";
    }

    @Override
    int[] getEmType() {
        return null;
    }

    String[] getLabels() {
        return new String[] {"CID", "PDP_addr_1", "PDP_addr_2", "Traffic class",
                "Maximum bitrate UL", "Maximum bitrate DL", "Guaranteed bitrate UL",
                "Guaranteed bitrate DL", "Delivery order", "Maximum SDU size",
                "SDU error ratio", "Residual bit error ratio", "Delivery of erroneous SDUs",
                "Transfer delay", "Traffic handling priority", "APN"};
    }

    @Override
    View getView() {
        ((ComponentDetailActivity) mActivity).sendATCommand(
                new String[] {"AT+CGEQNEG?", "+CGEQNEG:"},
                mATCmdHander.obtainMessage(MSG_QUERY_CID));
        return super.getView();
    }

    HashMap<String, Integer> mPositions = new HashMap<String, Integer>();
    private Handler mATCmdHander = new Handler() {
        public void handleMessage(Message msg) {
            AsyncResult ar;
            switch (msg.what) {
            case MSG_QUERY_CID:
                ar = (AsyncResult) msg.obj;
                if (ar.exception == null) {
                    String[] data = (String[]) ar.result;
                    if (data == null || data.length < 1 ||
                        data[0].length() < "+CGEQNEG:".length()) {
                        Elog.e(TAG, "Wrong data");
                        break;
                    }
                    String str = data[0].substring("+CGEQNEG:".length()).trim();
                    String[] splited = str.split(",");
                    if (splited == null || splited.length < 1) {
                        Elog.e(TAG, "Wrong data: " + data[0]);
                        break;
                    }

                    Elog.e(TAG, "data: " + data[0]);
                    clearData();
                    mPositions.clear();
                    for (int i = 0; i < splited.length; i++) {
                        addData(splited[i]);
                        for (int j = 1; j < getLabels().length; j++) {
                            addData("");
                        }
                        mPositions.put(splited[i], getLabels().length * i);
                        ((ComponentDetailActivity) mActivity).sendATCommand(
                                new String[] {"AT+CGPADDR=" + splited[i], "+CGPADDR:"},
                                mATCmdHander.obtainMessage(MSG_ADDR));
                        ((ComponentDetailActivity) mActivity).sendATCommand(
                                new String[] {"AT+CGEQNEG=" + splited[i], "+CGEQNEG:"},
                                mATCmdHander.obtainMessage(MSG_QOS));
                        ((ComponentDetailActivity) mActivity).sendATCommand(
                                new String[] {"AT+CGCONTRDP=" + splited[i], "+CGCONTRDP:"},
                                mATCmdHander.obtainMessage(MSG_APN));
                    }
                }
                break;
            case MSG_ADDR:
                ar = (AsyncResult) msg.obj;
                if (ar.exception == null) {
                    String[] data = (String[]) ar.result;
                    if (data == null || data.length < 1 ||
                        data[0].length() < "+CGPADDR:".length()) {
                        Elog.e(TAG, "Wrong data");
                        break;
                    }
                    Elog.e(TAG, "data: " + data[0]);
                    String str = data[0].substring("+CGPADDR:".length()).trim();
                    String[] splited = str.split(",");
                    int pos = mPositions.get(splited[0]);
                    for (int i = 1; i < splited.length; i++) {
                        addDataAtPostion(pos + i, splited[i]);
                    }
                }
                break;
            case MSG_QOS:
                ar = (AsyncResult) msg.obj;
                if (ar.exception == null) {
                    String[] data = (String[]) ar.result;
                    if (data == null || data.length < 1 ||
                        data[0].length() < "+CGEQNEG:".length()) {
                        Elog.e(TAG, "Wrong data");
                        break;
                    }
                    Elog.e(TAG, "data: " + data[0]);
                    String str = data[0].substring("+CGEQNEG:".length()).trim();
                    String[] splited = str.split(",");
                    int pos = mPositions.get(splited[0]);
                    for (int i = 1; i < splited.length; i++) {
                        addDataAtPostion(pos + i + 3, splited[i]);
                    }
                }
                break;
            case MSG_APN:
                ar = (AsyncResult) msg.obj;
                if (ar.exception == null) {
                    String[] data = (String[]) ar.result;
                    if (data == null || data.length < 1 ||
                        data[0].length() < "+CGCONTRDP:".length()) {
                        Elog.e(TAG, "Wrong data");
                        break;
                    }
                    String str = data[0].substring("+CGCONTRDP:".length()).trim();
                    String[] splited = str.split(",");
                    int pos = mPositions.get(splited[0]);
                    addDataAtPostion(pos + 15, splited[2]);
                }
                break;
            default:
                break;
            }
        }
    };

    @Override
    void update(int type, Parser data) {
    }
}
