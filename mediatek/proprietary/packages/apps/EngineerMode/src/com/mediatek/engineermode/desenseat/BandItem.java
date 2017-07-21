package com.mediatek.engineermode.desenseat;

import com.mediatek.engineermode.Elog;

import java.util.HashMap;


/**
 * Band item information.
 *
 */
/**
 * @author mtk80357
 *
 */
public class BandItem {
    private String mBandName;
    private static final String TAG = "DesenseAT/BandItem";
    private int mPower = -1;
    private int[] mChannleList = {-1 , -1 , -1};
    private String mBandValue;
    private static HashMap<String, Integer[]> sDefaultData;
    private boolean mSelected = false;
    private boolean mIsULFreq = false;
    private BandType mBandType = BandType.BAND_NULL;

    /**
     * Telephony band type.
     *
     */
    enum BandType {
        BAND_NULL,
        BAND_GSM,
        BAND_WCDMA,
        BAND_TD,
        BAND_LTE,
        BAND_CDMA
    };

    /**
     * Constructor function.
     * @param bandName name for user to understand.
     * @param bandvalue value for real use with modem.
     * @param bandtype telephony band type.
     */
    public BandItem(String bandName, String bandvalue, BandType bandtype) {
        // TODO Auto-generated constructor stub
        mBandName = bandName;
        mBandValue = bandvalue;
        mBandType = bandtype;
        Elog.d(TAG, "add " + mBandName + "  " + mBandName);
        addDefaultValue(mBandName);

    }

    /**
     * Constructor function.
     * @param bandName name for user to understand
     * @param bandvalue value for real use with modem
     * @param bandtype telephony band type
     * @param isULFreq special param for Lte
     */
    public BandItem(String bandName, String bandvalue, BandType bandtype, boolean isULFreq) {
        // TODO Auto-generated constructor stub
        this(bandName, bandvalue, bandtype);
        mIsULFreq = isULFreq;
    }

    public boolean isUlFreq() {
        return mIsULFreq;
    }
    public BandType getType() {
        return mBandType;
    }
    public String getBandName() {
        return mBandName;
    }

    public void setSelected(boolean selected) {
        mSelected = selected;
    }

    public int getPower() {
        return mPower;
    }

    public void setPower(int power) {
        mPower = power;
    }


    /**
     * @param ch1 channel 1
     * @param ch2 channel 2
     * @param ch3 channel 3
     */
    public void setChannel(int ch1, int ch2, int ch3) {
        mChannleList[0] = ch1;
        mChannleList[1] = ch2;
        mChannleList[2] = ch3;
    }

    public int[] getChannel() {
        return mChannleList;
    }

    /**
     * @return get information summary to show
     */
    public String getSummary() {
        StringBuilder summary = new StringBuilder(mIsULFreq ? "ULFreq: " : "Channel: ");
        boolean notSet = true;
        for (int k = 0 ; k < 3 ; k++) {
            if (mChannleList[k] != -1) {
                summary.append(mChannleList[k]).append("   ");
                notSet = false;
            }
        }
        if (notSet) {
            summary.append("NA    ");
        }

        summary.append((mPower == -1) ? "PWR:NA" : ("PWR: " + mPower));

        return summary.toString();
    }

    public boolean isSelected() {
        return mSelected;
    }

    static void initDefaultData() {
        sDefaultData = new HashMap<String, Integer[]>();
        sDefaultData.put("GSM 850", new Integer[]{128, 192, 251, 33});
        sDefaultData.put("DCS 1800", new Integer[]{512, -1, -1, 33});
        sDefaultData.put("WCDMA Band 2", new Integer[]{9262, 9400, 9538, 24});
        sDefaultData.put("WCDMA Band 5", new Integer[]{4132, 4175, 4233, 24});
        sDefaultData.put("Band F", new Integer[]{9404, -1, -1, 24});
        sDefaultData.put("LTE Band 3", new Integer[]{17100, -1, -1, 24});
        sDefaultData.put("LTE Band 5", new Integer[]{8240, 8365, 8489, 24});
        sDefaultData.put("LTE Band 13", new Integer[]{7770, 7869, -1, 24});
        sDefaultData.put("LTE Band 14", new Integer[]{7880, 7930, 7979, 24});
        sDefaultData.put("LTE Band 17", new Integer[]{7040, 7100, 7159, 24});
        sDefaultData.put("LTE Band 20", new Integer[]{8320, 8469, 8619, 24});

    }

    private void addDefaultValue(String bandName) {
        Elog.d(TAG, "addDefaultValue");
        Integer[] data = sDefaultData.get(bandName);
        if (data != null) {
            Elog.d(TAG, "data = " + data[0] + " "  + data[1] + " " + data[2] + " " + data[3]);
            mChannleList[0] = data[0];
            mChannleList[1] = data[1];
            mChannleList[2] = data[2];
            mPower = data[3];
            mSelected = true;
        }
    }

    @Override
    public String toString() {
        // TODO Auto-generated method stub
        return "mBandName:" + mBandName + " Summary: " + getSummary();
    }

}
