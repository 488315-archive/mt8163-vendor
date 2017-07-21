package com.mediatek.settings.plugin;

import android.content.ContentUris;
import android.content.ContentValues;
import android.content.Context;
import android.database.Cursor;
import android.net.Uri;
import android.provider.Telephony;
import android.util.Log;

import com.mediatek.common.PluginImpl;
import com.mediatek.settings.ext.DefaultApnSettingsExt;

/**
 * CU feature , CU apn settings can not edit.
 */
@PluginImpl(interfaceName = "com.mediatek.settings.ext.IApnSettingsExt")
public class OP02ApnSettingsExt extends DefaultApnSettingsExt {

    private static final String TAG = "OP02ApnSettingsExt";
    private static final String CU_NUMERIC_1 = "46001";
    private static final String CU_NUMERIC_2 = "46009";
    /// CU sim in Hongkong
    private static final String CU_NUMERIC_3 = "45407";

    // -1 stands for the apn inserted fail
    private static final long APN_NO_UPDATE = -1;


    @Override
    public boolean isAllowEditPresetApn(String type, String apn, String numeric, int sourcetype) {
        return (!isCUNumeric(numeric) || sourcetype != 0);
    }

    private boolean isCUNumeric(String numeric) {
        if (numeric != null) {
            return numeric.equals(CU_NUMERIC_1)
                || numeric.equals(CU_NUMERIC_2)
                || numeric.equals(CU_NUMERIC_3);
        } else {
            return false;
        }
    }

    /**
     * Check if the same name(apn item)exists, if it exists, replace it.
     * @param defaultReplacedNum the default replace number.
     * @param context ApnSettings context
     * @param uri to access database
     * @param apn profile apn
     * @param name profile carrier name
     * @param values new profile values to update
     * @param numeric selected numeric
     * @return the replaced profile id
     */
    @Override
    public long replaceApn(long defaultReplaceNum, Context context, Uri uri, String apn, String name,
            ContentValues values, String numeric) {
        long numReplaced = APN_NO_UPDATE;
        Log.d("@M_" + TAG, "params: apn = " + apn + " numeric = " + numeric);
        if (isCUNumeric(numeric)) {
            String where = "numeric=\"" + numeric + "\"";
            Cursor cursor = null;
            try {
                cursor = context.getContentResolver().query(uri,
                        new String[] {  Telephony.Carriers._ID, Telephony.Carriers.APN },
                        where, null, Telephony.Carriers.DEFAULT_SORT_ORDER);
                if (cursor == null || cursor.getCount() == 0) {
                    Log.d("@M_" + TAG, "cu card ,cursor is null ,return");
                    return APN_NO_UPDATE;
                }
                cursor.moveToFirst();
                while (!cursor.isAfterLast()) {
                    Log.d("@M_" + TAG, "apn = " + apn + " getApn = " + cursor.getString(1));
                    if (apn.equals(cursor.getString(1))) {
                        numReplaced = Integer.parseInt(cursor.getString(0));
                        Uri newUri = ContentUris.withAppendedId(uri, numReplaced);
                        context.getContentResolver().update(newUri, values, null, null);
                        break;
                    }
                    cursor.moveToNext();
                }
            } finally {
                if (cursor != null) {
                    cursor.close();
                }
            }
        } else {
            numReplaced = super.replaceApn(defaultReplaceNum, context, uri, apn, name, values,
                    numeric);
        }
        return numReplaced;
    }

    /**
     * if sort APN by the name.
     * @param order sort by the name.
     * @return the sort string.
     */
    @Override
    public String getApnSortOrder(String order) {
        return null;
    }
}

