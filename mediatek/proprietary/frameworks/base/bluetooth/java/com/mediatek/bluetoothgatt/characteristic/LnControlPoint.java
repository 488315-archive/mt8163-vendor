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
 * MediaTek Inc. (C) 2015. All rights reserved.
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

package com.mediatek.bluetoothgatt.characteristic;

import android.bluetooth.BluetoothGattCharacteristic;
// Customized Start: Import ........................................................................

//........................................................................ Customized End: Import //
import java.util.UUID;

/**
 * Public API for the LN Control Point Bluetooth GATT Characteristic.
 *
 * <p>This class provides LN Control Point Bluetooth GATT Characteristic
 * value encode/decode functions. Allowing applications easy and quick to
 * read/write characteristic field value.
 *
 * <p>Name: LN Control Point
 * Type: org.bluetooth.characteristic.ln_control_point
 * UUID: 2A6B
 * Last Modified: 2013-05-13
 * Revision: None
 */
public class LnControlPoint extends CharacteristicBase {
    /**
     * LN Control Point UUID.
     */
    public static final UUID GATT_UUID =
            UUID.fromString(FormatUtils.uuid16ToUuid128("2A6B"));

    // Customized Start: Constant Definition .......................................................
    /**
     * Op code enumerations.
     */
    public static final int OP_SET_CUMULATIVE_VALUE = 1;
    public static final int OP_MASK_CHARACTERISTIC_CONTENT = 2;
    public static final int OP_NAVIGATION_CONTROL = 3;
    public static final int OP_REQUEST_NUMBER_OF_ROUTES = 4;
    public static final int OP_REQUEST_NAME_OF_ROUTE = 5;
    public static final int OP_SELECT_ROUTE = 6;
    public static final int OP_SET_FIX_RATE = 7;
    public static final int OP_SET_ELEVATION = 8;
    public static final int OP_RESPONSE_CODE = 32;

    /**
     * Response Value enumerations.
     */
    public static final int RESP_SUCCESS = 1;
    public static final int RESP_OP_CODE_NOT_SUPPORTED = 2;
    public static final int RESP_INVALID_PARAMETER = 3;
    public static final int RESP_OPERATION_FAILED = 4;
    //....................................................... Customized End: Constant Definition //

    /*
     * Field: Op Codes
     * Requirement: Mandatory
     * Format: uint8
     * Unit: None
     */
    private byte[] mOpCodes = new byte[FormatUtils.UINT8_SIZE];

    /*
     * Field: Parameter Value
     * Requirement: Optional
     * Format: variable
     * Unit: None
     */
    private byte[] mParameterValue = new byte[0];

    /*
     * Field: Request Op Code
     * Requirement: C1
     * Format: uint8
     * Unit: None
     */
    private byte[] mRequestOpCode = new byte[FormatUtils.UINT8_SIZE];

    /*
     * Field: Response Value
     * Requirement: C1
     * Format: uint8
     * Unit: None
     */
    private byte[] mResponseValue = new byte[FormatUtils.UINT8_SIZE];

    /*
     * Field: Response Parameter
     * Requirement: C2
     * Format: variable
     * Unit: None
     */
    private byte[] mResponseParameter = new byte[0];


    /**
     * Create a LnControlPoint characteristic object.
     */
    public LnControlPoint() {
        setCharacteristic(null);
        setOpCodes(1);
        setParameterValue(new byte[0]);
        setRequestOpCode(0);
        setResponseValue(1);
        setResponseParameter(new byte[0]);
    }

    /**
     * Create a LnControlPoint characteristic object and init value.
     *
     * @param value Initial value
     */
    public LnControlPoint(byte[] value) {
        setCharacteristic(null);
        setValue(value);
    }

    /**
     * Create a LnControlPoint characteristic object and init value.
     *
     * @param value Initial value
     * @param characteristic the gatt-characteristic this characteristic belongs to.
     */
    public LnControlPoint(byte[] value, BluetoothGattCharacteristic characteristic) {
        setCharacteristic(characteristic);
        setValue(value);
    }

    /**
     * Create a LnControlPoint characteristic object.
     *
     * @param opCodes Op Codes
     * @param parameterValue Parameter Value
     * @param requestOpCode Request Op Code
     * @param responseValue Response Value
     * @param responseParameter Response Parameter
     */
    public LnControlPoint(
            int opCodes,
            byte[] parameterValue,
            int requestOpCode,
            int responseValue,
            byte[] responseParameter) {
        setCharacteristic(null);
        setOpCodes(opCodes);
        setParameterValue(parameterValue);
        setRequestOpCode(requestOpCode);
        setResponseValue(responseValue);
        setResponseParameter(responseParameter);
    }

    /**
     * Create a LnControlPoint characteristic object.
     *
     * @param opCodes Op Codes
     * @param parameterValue Parameter Value
     * @param requestOpCode Request Op Code
     * @param responseValue Response Value
     * @param responseParameter Response Parameter
     * @param characteristic the gatt-characteristic this characteristic belongs to.
     */
    public LnControlPoint(
            int opCodes,
            byte[] parameterValue,
            int requestOpCode,
            int responseValue,
            byte[] responseParameter,
            BluetoothGattCharacteristic characteristic) {
        setCharacteristic(characteristic);
        setOpCodes(opCodes);
        setParameterValue(parameterValue);
        setRequestOpCode(requestOpCode);
        setResponseValue(responseValue);
        setResponseParameter(responseParameter);
    }

    // Customized Start: Constructors ..............................................................

    //.............................................................. Customized End: Constructors //

    /**
     * Get LnControlPoint characteristic byte length.
     *
     * @return Byte length of this characteristic
     */
    public int length() {
        return (isSupportOpCodes()
                        ? mOpCodes.length : 0)
                + (isSupportParameterValue()
                        ? mParameterValue.length : 0)
                + (isSupportRequestOpCode()
                        ? mRequestOpCode.length : 0)
                + (isSupportResponseValue()
                        ? mResponseValue.length : 0)
                + (isSupportResponseParameter()
                        ? mResponseParameter.length : 0);
    }

    /**
     * Get LnControlPoint characteristic UUID.
     *
     * @return Characteristic UUID
     */
    @Override
    public UUID getUuid() {
        return GATT_UUID;
    }

    /**
     * Get LnControlPoint characteristic value.
     *
     * @return Byte array value of this characteristic.
     */
    @Override
    public byte[] getValue() {
        byte[] value = new byte[length()];
        int dstPos = 0;

        if (isSupportOpCodes()) {
            int fieldLen = mOpCodes.length;
            System.arraycopy(mOpCodes, 0, value, dstPos, fieldLen);
            dstPos += fieldLen;
        }

        if (isSupportParameterValue()) {
            int fieldLen = mParameterValue.length;
            System.arraycopy(mParameterValue, 0, value, dstPos, fieldLen);
            dstPos += fieldLen;
        }

        if (isSupportRequestOpCode()) {
            int fieldLen = mRequestOpCode.length;
            System.arraycopy(mRequestOpCode, 0, value, dstPos, fieldLen);
            dstPos += fieldLen;
        }

        if (isSupportResponseValue()) {
            int fieldLen = mResponseValue.length;
            System.arraycopy(mResponseValue, 0, value, dstPos, fieldLen);
            dstPos += fieldLen;
        }

        if (isSupportResponseParameter()) {
            int fieldLen = mResponseParameter.length;
            System.arraycopy(mResponseParameter, 0, value, dstPos, fieldLen);
            dstPos += fieldLen;
        }

        return value;
    }

    /**
     * Set LnControlPoint characteristic value.
     *
     * @param value Byte array value
     * @return      True, if the value has been set successfully
     */
    @Override
    public boolean setValue(byte[] value) {
        int srcPos = 0;

        if (isSupportOpCodes()) {
            int fieldLen = mOpCodes.length;

            if (!setValueRangeCheck(value.length, srcPos, fieldLen)) {
                return false;
            }

            System.arraycopy(value, srcPos, mOpCodes, 0, fieldLen);
            srcPos += fieldLen;
        }

        if (isSupportParameterValue()) {
            mParameterValue = new byte[value.length - srcPos];
            int fieldLen = mParameterValue.length;
            System.arraycopy(value, srcPos, mParameterValue, 0, fieldLen);
            srcPos += fieldLen;
        }

        if (isSupportRequestOpCode()) {
            int fieldLen = mRequestOpCode.length;

            if (!setValueRangeCheck(value.length, srcPos, fieldLen)) {
                return false;
            }

            System.arraycopy(value, srcPos, mRequestOpCode, 0, fieldLen);
            srcPos += fieldLen;
        }

        if (isSupportResponseValue()) {
            int fieldLen = mResponseValue.length;

            if (!setValueRangeCheck(value.length, srcPos, fieldLen)) {
                return false;
            }

            System.arraycopy(value, srcPos, mResponseValue, 0, fieldLen);
            srcPos += fieldLen;
        }

        if (isSupportResponseParameter()) {
            mResponseParameter = new byte[value.length - srcPos];
            int fieldLen = mResponseParameter.length;
            System.arraycopy(value, srcPos, mResponseParameter, 0, fieldLen);
            srcPos += fieldLen;
        }

        updateGattCharacteristic();
        return true;
    }

    /**
     * Get OpCodes field value with int format.
     *
     * @return OpCodes field value
     */
    public int getOpCodes() {
        return FormatUtils.uint8ToInt(mOpCodes);
    }

    /**
     * Set OpCodes field value by int format
     * and update the related Bluetooth GATT Characteristic.
     *
     * @param value Value to write to OpCodes field
     * @return      True, if the value has been set successfully
     */
    public boolean setOpCodes(int value) {
        if (!FormatUtils.uint8RangeCheck(value)) {
            return false;
        }
        mOpCodes = FormatUtils.intToUint8(value);
        updateGattCharacteristic();
        return true;
    }

    /**
     * Return true if LnControlPoint support OpCodes field.
     *
     * @return  True, if LnControlPoint support OpCodes field.
     */
    public boolean isSupportOpCodes() {
        return true;
    }

    /**
     * Get ParameterValue field value with byte array format.
     *
     * @return ParameterValue field value
     */
    public byte[] getParameterValue() {
        return mParameterValue;
    }

    /**
     * Set ParameterValue field value by byte array format
     * and update the related Bluetooth GATT Characteristic.
     *
     * @param value Value to write to ParameterValue field
     * @return  True
     */
    public boolean setParameterValue(byte[] value) {
        if (value == null) {
            mParameterValue = new byte[0];
        } else {
            mParameterValue = value;
        }
        updateGattCharacteristic();
        return true;
    }

    /**
     * Return true if LnControlPoint support ParameterValue field.
     *
     * @return  True, if LnControlPoint support ParameterValue field.
     */
    public boolean isSupportParameterValue() {
        return (getOpCodes() == OP_SET_CUMULATIVE_VALUE ||
                getOpCodes() == OP_MASK_CHARACTERISTIC_CONTENT ||
                getOpCodes() == OP_NAVIGATION_CONTROL ||
                getOpCodes() == OP_REQUEST_NAME_OF_ROUTE ||
                getOpCodes() == OP_SELECT_ROUTE ||
                getOpCodes() == OP_SET_FIX_RATE ||
                getOpCodes() == OP_SET_ELEVATION);
    }

    /**
     * Get RequestOpCode field value with int format.
     *
     * @return RequestOpCode field value
     */
    public int getRequestOpCode() {
        return FormatUtils.uint8ToInt(mRequestOpCode);
    }

    /**
     * Set RequestOpCode field value by int format
     * and update the related Bluetooth GATT Characteristic.
     *
     * @param value Value to write to RequestOpCode field
     * @return      True, if the value has been set successfully
     */
    public boolean setRequestOpCode(int value) {
        if (!FormatUtils.uint8RangeCheck(value)) {
            return false;
        }
        mRequestOpCode = FormatUtils.intToUint8(value);
        updateGattCharacteristic();
        return true;
    }

    /**
     * Return true if LnControlPoint support RequestOpCode field.
     *
     * @return  True, if LnControlPoint support RequestOpCode field.
     */
    public boolean isSupportRequestOpCode() {
        return (getOpCodes() == OP_RESPONSE_CODE);
    }

    /**
     * Get ResponseValue field value with int format.
     *
     * @return ResponseValue field value
     */
    public int getResponseValue() {
        return FormatUtils.uint8ToInt(mResponseValue);
    }

    /**
     * Set ResponseValue field value by int format
     * and update the related Bluetooth GATT Characteristic.
     *
     * @param value Value to write to ResponseValue field
     * @return      True, if the value has been set successfully
     */
    public boolean setResponseValue(int value) {
        if (!FormatUtils.uint8RangeCheck(value)) {
            return false;
        }
        mResponseValue = FormatUtils.intToUint8(value);
        updateGattCharacteristic();
        return true;
    }

    /**
     * Return true if LnControlPoint support ResponseValue field.
     *
     * @return  True, if LnControlPoint support ResponseValue field.
     */
    public boolean isSupportResponseValue() {
        return (getOpCodes() == OP_RESPONSE_CODE);
    }

    /**
     * Get ResponseParameter field value with byte array format.
     *
     * @return ResponseParameter field value
     */
    public byte[] getResponseParameter() {
        return mResponseParameter;
    }

    /**
     * Set ResponseParameter field value by byte array format
     * and update the related Bluetooth GATT Characteristic.
     *
     * @param value Value to write to ResponseParameter field
     * @return  True
     */
    public boolean setResponseParameter(byte[] value) {
        if (value == null) {
            mResponseParameter = new byte[0];
        } else {
            mResponseParameter = value;
        }
        updateGattCharacteristic();
        return true;
    }

    /**
     * Return true if LnControlPoint support ResponseParameter field.
     *
     * @return  True, if LnControlPoint support ResponseParameter field.
     */
    public boolean isSupportResponseParameter() {
        return ((getOpCodes() == OP_RESPONSE_CODE) &&
                (getResponseValue() == RESP_SUCCESS) &&
                (getRequestOpCode() == OP_REQUEST_NUMBER_OF_ROUTES ||
                 getRequestOpCode() == OP_REQUEST_NAME_OF_ROUTE));
    }


    // Customized Start: Functions .................................................................

    //................................................................. Customized End: Functions //
}

