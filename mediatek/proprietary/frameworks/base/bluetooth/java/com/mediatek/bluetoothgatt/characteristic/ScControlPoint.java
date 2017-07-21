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
 * Public API for the SC Control Point Bluetooth GATT Characteristic.
 *
 * <p>This class provides SC Control Point Bluetooth GATT Characteristic
 * value encode/decode functions. Allowing applications easy and quick to
 * read/write characteristic field value.
 *
 * <p>Name: SC Control Point
 * Type: org.bluetooth.characteristic.sc_control_point
 * UUID: 2A55
 * Last Modified: 2012-05-29
 * Revision: None
 */
public class ScControlPoint extends CharacteristicBase {
    /**
     * SC Control Point UUID.
     */
    public static final UUID GATT_UUID =
            UUID.fromString(FormatUtils.uuid16ToUuid128("2A55"));

    // Customized Start: Constant Definition .......................................................
    public static final int OP_SET_CUMULATIVE_VALUE = 1;
    public static final int OP_START_SENSOR_CALIBRATION = 2;
    public static final int OP_UPDATE_SENSOR_LOCATION = 3;
    public static final int OP_REQUEST_SUPPORTED_LOCATIONS = 4;
    public static final int OP_RESPONSE_CODE = 16;

    public static final int RESPONSE_SUCCESS = 1;
    public static final int RESPONSE_OPCODE_NOT_SUPPORT = 2;
    public static final int RESPONSE_INVALID_PARAMETER = 3;
    public static final int RESPONSE_OPERATION_FAILED = 4;
    //....................................................... Customized End: Constant Definition //

    /*
     * Field: Op Code
     * Requirement: Mandatory
     * Format: uint8
     * Unit: None
     */
    private byte[] mOpCode = new byte[FormatUtils.UINT8_SIZE];

    /*
     * Field: Cumulative Value
     * Requirement: C3
     * Format: variable
     * Unit: None
     */
    private byte[] mCumulativeValue = new byte[0];

    /*
     * Field: Sensor Location Value
     * Requirement: C1
     * Format: uint8
     * Unit: None
     */
    private byte[] mSensorLocationValue = new byte[FormatUtils.UINT8_SIZE];

    /*
     * Field: Request Op Code
     * Requirement: C2
     * Format: uint8
     * Unit: None
     */
    private byte[] mRequestOpCode = new byte[FormatUtils.UINT8_SIZE];

    /*
     * Field: Response Value
     * Requirement: C2
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
     * Create a ScControlPoint characteristic object.
     */
    public ScControlPoint() {
        setCharacteristic(null);
        setOpCode(0);
        setCumulativeValue(new byte[0]);
        setSensorLocationValue(0);
        setRequestOpCode(0);
        setResponseValue(0);
        setResponseParameter(new byte[0]);
    }

    /**
     * Create a ScControlPoint characteristic object and init value.
     *
     * @param value Initial value
     */
    public ScControlPoint(byte[] value) {
        setCharacteristic(null);
        setValue(value);
    }

    /**
     * Create a ScControlPoint characteristic object and init value.
     *
     * @param value Initial value
     * @param characteristic the gatt-characteristic this characteristic belongs to.
     */
    public ScControlPoint(byte[] value, BluetoothGattCharacteristic characteristic) {
        setCharacteristic(characteristic);
        setValue(value);
    }

    /**
     * Create a ScControlPoint characteristic object.
     *
     * @param opCode Op Code
     * @param cumulativeValue Cumulative Value
     * @param sensorLocationValue Sensor Location Value
     * @param requestOpCode Request Op Code
     * @param responseValue Response Value
     * @param responseParameter Response Parameter
     */
    public ScControlPoint(
            int opCode,
            byte[] cumulativeValue,
            int sensorLocationValue,
            int requestOpCode,
            int responseValue,
            byte[] responseParameter) {
        setCharacteristic(null);
        setOpCode(opCode);
        setCumulativeValue(cumulativeValue);
        setSensorLocationValue(sensorLocationValue);
        setRequestOpCode(requestOpCode);
        setResponseValue(responseValue);
        setResponseParameter(responseParameter);
    }

    /**
     * Create a ScControlPoint characteristic object.
     *
     * @param opCode Op Code
     * @param cumulativeValue Cumulative Value
     * @param sensorLocationValue Sensor Location Value
     * @param requestOpCode Request Op Code
     * @param responseValue Response Value
     * @param responseParameter Response Parameter
     * @param characteristic the gatt-characteristic this characteristic belongs to.
     */
    public ScControlPoint(
            int opCode,
            byte[] cumulativeValue,
            int sensorLocationValue,
            int requestOpCode,
            int responseValue,
            byte[] responseParameter,
            BluetoothGattCharacteristic characteristic) {
        setCharacteristic(characteristic);
        setOpCode(opCode);
        setCumulativeValue(cumulativeValue);
        setSensorLocationValue(sensorLocationValue);
        setRequestOpCode(requestOpCode);
        setResponseValue(responseValue);
        setResponseParameter(responseParameter);
    }

    // Customized Start: Constructors ..............................................................

    //.............................................................. Customized End: Constructors //

    /**
     * Get ScControlPoint characteristic byte length.
     *
     * @return Byte length of this characteristic
     */
    public int length() {
        return (isSupportOpCode()
                        ? mOpCode.length : 0)
                + (isSupportCumulativeValue()
                        ? mCumulativeValue.length : 0)
                + (isSupportSensorLocationValue()
                        ? mSensorLocationValue.length : 0)
                + (isSupportRequestOpCode()
                        ? mRequestOpCode.length : 0)
                + (isSupportResponseValue()
                        ? mResponseValue.length : 0)
                + (isSupportResponseParameter()
                        ? mResponseParameter.length : 0);
    }

    /**
     * Get ScControlPoint characteristic UUID.
     *
     * @return Characteristic UUID
     */
    @Override
    public UUID getUuid() {
        return GATT_UUID;
    }

    /**
     * Get ScControlPoint characteristic value.
     *
     * @return Byte array value of this characteristic.
     */
    @Override
    public byte[] getValue() {
        byte[] value = new byte[length()];
        int dstPos = 0;

        if (isSupportOpCode()) {
            int fieldLen = mOpCode.length;
            System.arraycopy(mOpCode, 0, value, dstPos, fieldLen);
            dstPos += fieldLen;
        }

        if (isSupportCumulativeValue()) {
            int fieldLen = mCumulativeValue.length;
            System.arraycopy(mCumulativeValue, 0, value, dstPos, fieldLen);
            dstPos += fieldLen;
        }

        if (isSupportSensorLocationValue()) {
            int fieldLen = mSensorLocationValue.length;
            System.arraycopy(mSensorLocationValue, 0, value, dstPos, fieldLen);
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
     * Set ScControlPoint characteristic value.
     *
     * @param value Byte array value
     * @return      True, if the value has been set successfully
     */
    @Override
    public boolean setValue(byte[] value) {
        int srcPos = 0;

        if (isSupportOpCode()) {
            int fieldLen = mOpCode.length;

            if (!setValueRangeCheck(value.length, srcPos, fieldLen)) {
                return false;
            }

            System.arraycopy(value, srcPos, mOpCode, 0, fieldLen);
            srcPos += fieldLen;
        }

        if (isSupportCumulativeValue()) {
            mCumulativeValue = new byte[value.length - srcPos];
            int fieldLen = mCumulativeValue.length;
            System.arraycopy(value, srcPos, mCumulativeValue, 0, fieldLen);
            srcPos += fieldLen;
        }

        if (isSupportSensorLocationValue()) {
            int fieldLen = mSensorLocationValue.length;

            if (!setValueRangeCheck(value.length, srcPos, fieldLen)) {
                return false;
            }

            System.arraycopy(value, srcPos, mSensorLocationValue, 0, fieldLen);
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
     * Get OpCode field value with int format.
     *
     * @return OpCode field value
     */
    public int getOpCode() {
        return FormatUtils.uint8ToInt(mOpCode);
    }

    /**
     * Set OpCode field value by int format
     * and update the related Bluetooth GATT Characteristic.
     *
     * @param value Value to write to OpCode field
     * @return      True, if the value has been set successfully
     */
    public boolean setOpCode(int value) {
        if (!FormatUtils.uint8RangeCheck(value)) {
            return false;
        }
        mOpCode = FormatUtils.intToUint8(value);
        updateGattCharacteristic();
        return true;
    }

    /**
     * Return true if ScControlPoint support OpCode field.
     *
     * @return  True, if ScControlPoint support OpCode field.
     */
    public boolean isSupportOpCode() {
        return true;
    }

    /**
     * Get CumulativeValue field value with byte array format.
     *
     * @return CumulativeValue field value
     */
    public byte[] getCumulativeValue() {
        return mCumulativeValue;
    }

    /**
     * Set CumulativeValue field value by byte array format
     * and update the related Bluetooth GATT Characteristic.
     *
     * @param value Value to write to CumulativeValue field
     * @return  True
     */
    public boolean setCumulativeValue(byte[] value) {
        if (value == null) {
            mCumulativeValue = new byte[0];
        } else {
            mCumulativeValue = value;
        }
        updateGattCharacteristic();
        return true;
    }

    /**
     * Return true if ScControlPoint support CumulativeValue field.
     *
     * @return  True, if ScControlPoint support CumulativeValue field.
     */
    public boolean isSupportCumulativeValue() {
        return (getOpCode() == 1);
    }

    /**
     * Get SensorLocationValue field value with int format.
     *
     * @return SensorLocationValue field value
     */
    public int getSensorLocationValue() {
        return FormatUtils.uint8ToInt(mSensorLocationValue);
    }

    /**
     * Set SensorLocationValue field value by int format
     * and update the related Bluetooth GATT Characteristic.
     *
     * @param value Value to write to SensorLocationValue field
     * @return      True, if the value has been set successfully
     */
    public boolean setSensorLocationValue(int value) {
        if (!FormatUtils.uint8RangeCheck(value)) {
            return false;
        }
        mSensorLocationValue = FormatUtils.intToUint8(value);
        updateGattCharacteristic();
        return true;
    }

    /**
     * Return true if ScControlPoint support SensorLocationValue field.
     *
     * @return  True, if ScControlPoint support SensorLocationValue field.
     */
    public boolean isSupportSensorLocationValue() {
        return (getOpCode() == 3);
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
     * Return true if ScControlPoint support RequestOpCode field.
     *
     * @return  True, if ScControlPoint support RequestOpCode field.
     */
    public boolean isSupportRequestOpCode() {
        return (getOpCode() == 16);
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
     * Return true if ScControlPoint support ResponseValue field.
     *
     * @return  True, if ScControlPoint support ResponseValue field.
     */
    public boolean isSupportResponseValue() {
        return (getOpCode() == 16);
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
     * Return true if ScControlPoint support ResponseParameter field.
     *
     * @return  True, if ScControlPoint support ResponseParameter field.
     */
    public boolean isSupportResponseParameter() {
        return (getOpCode() == 16);
    }


    // Customized Start: Functions .................................................................

    //................................................................. Customized End: Functions //
}

