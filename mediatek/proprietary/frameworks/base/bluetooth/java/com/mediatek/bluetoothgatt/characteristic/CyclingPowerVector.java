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
 * Public API for the Cycling Power Vector Bluetooth GATT Characteristic.
 *
 * <p>This class provides Cycling Power Vector Bluetooth GATT Characteristic
 * value encode/decode functions. Allowing applications easy and quick to
 * read/write characteristic field value.
 *
 * <p>Name: Cycling Power Vector
 * Type: org.bluetooth.characteristic.cycling_power_vector
 * UUID: 2A64
 * Last Modified: 2014-07-02
 * Revision: None
 */
public class CyclingPowerVector extends CharacteristicBase {
    /**
     * Cycling Power Vector UUID.
     */
    public static final UUID GATT_UUID =
            UUID.fromString(FormatUtils.uuid16ToUuid128("2A64"));

    // Customized Start: Constant Definition .......................................................

    //....................................................... Customized End: Constant Definition //

    /*
     * Field: Flags
     * Requirement: Mandatory
     * Format: bit8
     */
    private BfFlags mFlags = new BfFlags();

    /*
     * Field: Crank Revolution Data - Cumulative Crank Revolutions
     * Requirement: C1
     * Format: uint16
     * Unit: org.bluetooth.unit.unitless
     */
    private byte[] mCrankCumulativeCrankRevolutions = new byte[FormatUtils.UINT16_SIZE];

    /*
     * Field: Crank Revolution Data - Last Crank Event Time
     * Requirement: C1
     * Format: uint16
     * Unit: org.bluetooth.unit.time.second
     */
    private byte[] mCrankLastCrankEventTime = new byte[FormatUtils.UINT16_SIZE];

    /*
     * Field: First Crank Measurement Angle
     * Requirement: Optional
     * Format: uint16
     * Unit: org.bluetooth.unit.plane_angle.degree
     */
    private byte[] mFirstCrankMeasurementAngle = new byte[FormatUtils.UINT16_SIZE];

    /*
     * Field: Instantaneous Force Magnitude Array
     * Requirement: C2
     * Format: variable
     * Unit: org.bluetooth.unit.force.newton
     */
    private byte[] mInstantaneousForceMagnitudeArray = new byte[0];

    /*
     * Field: Instantaneous Torque Magnitude Array
     * Requirement: C2
     * Format: variable
     * Unit: org.bluetooth.unit.moment_of_force.newton_metre
     */
    private byte[] mInstantaneousTorqueMagnitudeArray = new byte[0];


    /**
     * Create a CyclingPowerVector characteristic object.
     */
    public CyclingPowerVector() {
        setCharacteristic(null);
        setFlags(new BfFlags());
        setCrankCumulativeCrankRevolutions(0);
        setCrankLastCrankEventTime(0);
        setFirstCrankMeasurementAngle(0);
        setInstantaneousForceMagnitudeArray(new byte[0]);
        setInstantaneousTorqueMagnitudeArray(new byte[0]);
    }

    /**
     * Create a CyclingPowerVector characteristic object and init value.
     *
     * @param value Initial value
     */
    public CyclingPowerVector(byte[] value) {
        setCharacteristic(null);
        setValue(value);
    }

    /**
     * Create a CyclingPowerVector characteristic object and init value.
     *
     * @param value Initial value
     * @param characteristic the gatt-characteristic this characteristic belongs to.
     */
    public CyclingPowerVector(byte[] value, BluetoothGattCharacteristic characteristic) {
        setCharacteristic(characteristic);
        setValue(value);
    }

    /**
     * Create a CyclingPowerVector characteristic object.
     *
     * @param flags Flags
     * @param crankCumulativeCrankRevolutions Crank Revolution Data - Cumulative Crank Revolutions
     * @param crankLastCrankEventTime Crank Revolution Data - Last Crank Event Time
     * @param firstCrankMeasurementAngle First Crank Measurement Angle
     * @param instantaneousForceMagnitudeArray Instantaneous Force Magnitude Array
     * @param instantaneousTorqueMagnitudeArray Instantaneous Torque Magnitude Array
     */
    public CyclingPowerVector(
            BfFlags flags,
            int crankCumulativeCrankRevolutions,
            int crankLastCrankEventTime,
            int firstCrankMeasurementAngle,
            byte[] instantaneousForceMagnitudeArray,
            byte[] instantaneousTorqueMagnitudeArray) {
        setCharacteristic(null);
        setFlags(flags);
        setCrankCumulativeCrankRevolutions(crankCumulativeCrankRevolutions);
        setCrankLastCrankEventTime(crankLastCrankEventTime);
        setFirstCrankMeasurementAngle(firstCrankMeasurementAngle);
        setInstantaneousForceMagnitudeArray(instantaneousForceMagnitudeArray);
        setInstantaneousTorqueMagnitudeArray(instantaneousTorqueMagnitudeArray);
    }

    /**
     * Create a CyclingPowerVector characteristic object.
     *
     * @param flags Flags
     * @param crankCumulativeCrankRevolutions Crank Revolution Data - Cumulative Crank Revolutions
     * @param crankLastCrankEventTime Crank Revolution Data - Last Crank Event Time
     * @param firstCrankMeasurementAngle First Crank Measurement Angle
     * @param instantaneousForceMagnitudeArray Instantaneous Force Magnitude Array
     * @param instantaneousTorqueMagnitudeArray Instantaneous Torque Magnitude Array
     * @param characteristic the gatt-characteristic this characteristic belongs to.
     */
    public CyclingPowerVector(
            BfFlags flags,
            int crankCumulativeCrankRevolutions,
            int crankLastCrankEventTime,
            int firstCrankMeasurementAngle,
            byte[] instantaneousForceMagnitudeArray,
            byte[] instantaneousTorqueMagnitudeArray,
            BluetoothGattCharacteristic characteristic) {
        setCharacteristic(characteristic);
        setFlags(flags);
        setCrankCumulativeCrankRevolutions(crankCumulativeCrankRevolutions);
        setCrankLastCrankEventTime(crankLastCrankEventTime);
        setFirstCrankMeasurementAngle(firstCrankMeasurementAngle);
        setInstantaneousForceMagnitudeArray(instantaneousForceMagnitudeArray);
        setInstantaneousTorqueMagnitudeArray(instantaneousTorqueMagnitudeArray);
    }

    // Customized Start: Constructors ..............................................................

    //.............................................................. Customized End: Constructors //

    /**
     * Get CyclingPowerVector characteristic byte length.
     *
     * @return Byte length of this characteristic
     */
    public int length() {
        return (isSupportFlags()
                        ? mFlags.length() : 0)
                + (isSupportCrankCumulativeCrankRevolutions()
                        ? mCrankCumulativeCrankRevolutions.length : 0)
                + (isSupportCrankLastCrankEventTime()
                        ? mCrankLastCrankEventTime.length : 0)
                + (isSupportFirstCrankMeasurementAngle()
                        ? mFirstCrankMeasurementAngle.length : 0)
                + (isSupportInstantaneousForceMagnitudeArray()
                        ? mInstantaneousForceMagnitudeArray.length : 0)
                + (isSupportInstantaneousTorqueMagnitudeArray()
                        ? mInstantaneousTorqueMagnitudeArray.length : 0);
    }

    /**
     * Get CyclingPowerVector characteristic UUID.
     *
     * @return Characteristic UUID
     */
    @Override
    public UUID getUuid() {
        return GATT_UUID;
    }

    /**
     * Get CyclingPowerVector characteristic value.
     *
     * @return Byte array value of this characteristic.
     */
    @Override
    public byte[] getValue() {
        byte[] value = new byte[length()];
        int dstPos = 0;

        if (isSupportFlags()) {
            int fieldLen = mFlags.length();
            System.arraycopy(mFlags.getByteArray(), 0, value, dstPos, fieldLen);
            dstPos += fieldLen;
        }

        if (isSupportCrankCumulativeCrankRevolutions()) {
            int fieldLen = mCrankCumulativeCrankRevolutions.length;
            System.arraycopy(mCrankCumulativeCrankRevolutions, 0, value, dstPos, fieldLen);
            dstPos += fieldLen;
        }

        if (isSupportCrankLastCrankEventTime()) {
            int fieldLen = mCrankLastCrankEventTime.length;
            System.arraycopy(mCrankLastCrankEventTime, 0, value, dstPos, fieldLen);
            dstPos += fieldLen;
        }

        if (isSupportFirstCrankMeasurementAngle()) {
            int fieldLen = mFirstCrankMeasurementAngle.length;
            System.arraycopy(mFirstCrankMeasurementAngle, 0, value, dstPos, fieldLen);
            dstPos += fieldLen;
        }

        if (isSupportInstantaneousForceMagnitudeArray()) {
            int fieldLen = mInstantaneousForceMagnitudeArray.length;
            System.arraycopy(mInstantaneousForceMagnitudeArray, 0, value, dstPos, fieldLen);
            dstPos += fieldLen;
        }

        if (isSupportInstantaneousTorqueMagnitudeArray()) {
            int fieldLen = mInstantaneousTorqueMagnitudeArray.length;
            System.arraycopy(mInstantaneousTorqueMagnitudeArray, 0, value, dstPos, fieldLen);
            dstPos += fieldLen;
        }

        return value;
    }

    /**
     * Set CyclingPowerVector characteristic value.
     *
     * @param value Byte array value
     * @return      True, if the value has been set successfully
     */
    @Override
    public boolean setValue(byte[] value) {
        int srcPos = 0;

        if (isSupportFlags()) {
            int fieldLen = mFlags.length();

            if (!setValueRangeCheck(value.length, srcPos, fieldLen)) {
                return false;
            }

            byte[] buf = new byte[fieldLen];

            System.arraycopy(value, srcPos, buf, 0, buf.length);
            srcPos += fieldLen;

            mFlags.setByteArray(buf);
        }

        if (isSupportCrankCumulativeCrankRevolutions()) {
            int fieldLen = mCrankCumulativeCrankRevolutions.length;

            if (!setValueRangeCheck(value.length, srcPos, fieldLen)) {
                return false;
            }

            System.arraycopy(value, srcPos, mCrankCumulativeCrankRevolutions, 0, fieldLen);
            srcPos += fieldLen;
        }

        if (isSupportCrankLastCrankEventTime()) {
            int fieldLen = mCrankLastCrankEventTime.length;

            if (!setValueRangeCheck(value.length, srcPos, fieldLen)) {
                return false;
            }

            System.arraycopy(value, srcPos, mCrankLastCrankEventTime, 0, fieldLen);
            srcPos += fieldLen;
        }

        if (isSupportFirstCrankMeasurementAngle()) {
            int fieldLen = mFirstCrankMeasurementAngle.length;

            if (!setValueRangeCheck(value.length, srcPos, fieldLen)) {
                return false;
            }

            System.arraycopy(value, srcPos, mFirstCrankMeasurementAngle, 0, fieldLen);
            srcPos += fieldLen;
        }

        if (isSupportInstantaneousForceMagnitudeArray()) {
            mInstantaneousForceMagnitudeArray = new byte[value.length - srcPos];
            int fieldLen = mInstantaneousForceMagnitudeArray.length;
            System.arraycopy(value, srcPos, mInstantaneousForceMagnitudeArray, 0, fieldLen);
            srcPos += fieldLen;
        }

        if (isSupportInstantaneousTorqueMagnitudeArray()) {
            mInstantaneousTorqueMagnitudeArray = new byte[value.length - srcPos];
            int fieldLen = mInstantaneousTorqueMagnitudeArray.length;
            System.arraycopy(value, srcPos, mInstantaneousTorqueMagnitudeArray, 0, fieldLen);
            srcPos += fieldLen;
        }

        updateGattCharacteristic();
        return true;
    }

    /**
     * Get Flags field value with BfFlags format.
     *
     * @return Flags field value
     */
    public BfFlags getFlags() {
        return mFlags;
    }

    /**
     * Set Flags field value by BfFlags format
     * and update the related Bluetooth GATT Characteristic.
     *
     * @param value Value to write to Flags field
     * @return      True
     */
    public boolean setFlags(BfFlags value) {
        mFlags = value;
        updateGattCharacteristic();
        return true;
    }

    /**
     * Return true if CyclingPowerVector support Flags field.
     *
     * @return  True, if CyclingPowerVector support Flags field.
     */
    public boolean isSupportFlags() {
        return true;
    }

    /**
     * Get CrankCumulativeCrankRevolutions field value with int format.
     *
     * @return CrankCumulativeCrankRevolutions field value
     */
    public int getCrankCumulativeCrankRevolutions() {
        return FormatUtils.uint16ToInt(mCrankCumulativeCrankRevolutions);
    }

    /**
     * Set CrankCumulativeCrankRevolutions field value by int format
     * and update the related Bluetooth GATT Characteristic.
     *
     * @param value Value to write to CrankCumulativeCrankRevolutions field
     * @return      True, if the value has been set successfully
     */
    public boolean setCrankCumulativeCrankRevolutions(int value) {
        if (!FormatUtils.uint16RangeCheck(value)) {
            return false;
        }
        mCrankCumulativeCrankRevolutions = FormatUtils.intToUint16(value);
        updateGattCharacteristic();
        return true;
    }

    /**
     * Return true if CyclingPowerVector support CrankCumulativeCrankRevolutions field.
     *
     * @return  True, if CyclingPowerVector support CrankCumulativeCrankRevolutions field.
     */
    public boolean isSupportCrankCumulativeCrankRevolutions() {
        return (mFlags.getCrankRevolutionDataPresent() == 1);
    }

    /**
     * Get CrankLastCrankEventTime field value with int format.
     *
     * @return CrankLastCrankEventTime field value
     */
    public int getCrankLastCrankEventTime() {
        return FormatUtils.uint16ToInt(mCrankLastCrankEventTime);
    }

    /**
     * Set CrankLastCrankEventTime field value by int format
     * and update the related Bluetooth GATT Characteristic.
     *
     * @param value Value to write to CrankLastCrankEventTime field
     * @return      True, if the value has been set successfully
     */
    public boolean setCrankLastCrankEventTime(int value) {
        if (!FormatUtils.uint16RangeCheck(value)) {
            return false;
        }
        mCrankLastCrankEventTime = FormatUtils.intToUint16(value);
        updateGattCharacteristic();
        return true;
    }

    /**
     * Return true if CyclingPowerVector support CrankLastCrankEventTime field.
     *
     * @return  True, if CyclingPowerVector support CrankLastCrankEventTime field.
     */
    public boolean isSupportCrankLastCrankEventTime() {
        return (mFlags.getCrankRevolutionDataPresent() == 1);
    }

    /**
     * Get FirstCrankMeasurementAngle field value with int format.
     *
     * @return FirstCrankMeasurementAngle field value
     */
    public int getFirstCrankMeasurementAngle() {
        return FormatUtils.uint16ToInt(mFirstCrankMeasurementAngle);
    }

    /**
     * Set FirstCrankMeasurementAngle field value by int format
     * and update the related Bluetooth GATT Characteristic.
     *
     * @param value Value to write to FirstCrankMeasurementAngle field
     * @return      True, if the value has been set successfully
     */
    public boolean setFirstCrankMeasurementAngle(int value) {
        if (!FormatUtils.uint16RangeCheck(value)) {
            return false;
        }
        mFirstCrankMeasurementAngle = FormatUtils.intToUint16(value);
        updateGattCharacteristic();
        return true;
    }

    /**
     * Return true if CyclingPowerVector support FirstCrankMeasurementAngle field.
     *
     * @return  True, if CyclingPowerVector support FirstCrankMeasurementAngle field.
     */
    public boolean isSupportFirstCrankMeasurementAngle() {
        return (mFlags.getFirstCrankMeasurementAnglePresent() == 1);
    }

    /**
     * Get InstantaneousForceMagnitudeArray field value with byte array format.
     *
     * @return InstantaneousForceMagnitudeArray field value
     */
    public byte[] getInstantaneousForceMagnitudeArray() {
        return mInstantaneousForceMagnitudeArray;
    }

    /**
     * Set InstantaneousForceMagnitudeArray field value by byte array format
     * and update the related Bluetooth GATT Characteristic.
     *
     * @param value Value to write to InstantaneousForceMagnitudeArray field
     * @return  True
     */
    public boolean setInstantaneousForceMagnitudeArray(byte[] value) {
        if (value == null) {
            mInstantaneousForceMagnitudeArray = new byte[0];
        } else {
            mInstantaneousForceMagnitudeArray = value;
        }
        updateGattCharacteristic();
        return true;
    }

    /**
     * Return true if CyclingPowerVector support InstantaneousForceMagnitudeArray field.
     *
     * @return  True, if CyclingPowerVector support InstantaneousForceMagnitudeArray field.
     */
    public boolean isSupportInstantaneousForceMagnitudeArray() {
        return (mFlags.getInstantaneousForceMagnitudeArrayPresent() == 1);
    }

    /**
     * Get InstantaneousTorqueMagnitudeArray field value with byte array format.
     *
     * @return InstantaneousTorqueMagnitudeArray field value
     */
    public byte[] getInstantaneousTorqueMagnitudeArray() {
        return mInstantaneousTorqueMagnitudeArray;
    }

    /**
     * Set InstantaneousTorqueMagnitudeArray field value by byte array format
     * and update the related Bluetooth GATT Characteristic.
     *
     * @param value Value to write to InstantaneousTorqueMagnitudeArray field
     * @return  True
     */
    public boolean setInstantaneousTorqueMagnitudeArray(byte[] value) {
        if (value == null) {
            mInstantaneousTorqueMagnitudeArray = new byte[0];
        } else {
            mInstantaneousTorqueMagnitudeArray = value;
        }
        updateGattCharacteristic();
        return true;
    }

    /**
     * Return true if CyclingPowerVector support InstantaneousTorqueMagnitudeArray field.
     *
     * @return  True, if CyclingPowerVector support InstantaneousTorqueMagnitudeArray field.
     */
    public boolean isSupportInstantaneousTorqueMagnitudeArray() {
        return (mFlags.getInstantaneousTorqueMagnitudeArrayPresent() == 1);
    }

    /**
     * This class provides Flags BitField operations based on
     * specific definition.
     */
    public class BfFlags extends BitField {
        private static final int sLength = FormatUtils.BIT8_SIZE;

        /**
         * Get BfFlags BitField byte length.
         *
         * @return Byte length of this BitField
         */
        @Override
        public int length() {
            return sLength;
        }

        /**
         * Create a BfFlags BitField object.
         */
        public BfFlags() { super(sLength * 8); }

        /**
         * Create a BfFlags BitField object and init value.
         *
         * @param value Initial value
         */
        public BfFlags(byte[] value) {
            super(sLength * 8);
            setByteArray(value);
        }

        /**
         * Get CrankRevolutionDataPresent bit field value.
         *
         * @return Bit field value
         */
        public int getCrankRevolutionDataPresent() {
            return getValue(0, 0);
        }

        /**
         * Set CrankRevolutionDataPresent bit field value
         * and update the related Bluetooth GATT Characteristic.
         *
         * @param value Value to write to CrankRevolutionDataPresent bit field
         * @return      True, if the value has been set successfully
         */
        public boolean setCrankRevolutionDataPresent(int value) {
            if (!setValue(0, 0, value)) {
                return false;
            }
            updateGattCharacteristic();
            return true;
        }

        /**
         * Get FirstCrankMeasurementAnglePresent bit field value.
         *
         * @return Bit field value
         */
        public int getFirstCrankMeasurementAnglePresent() {
            return getValue(1, 1);
        }

        /**
         * Set FirstCrankMeasurementAnglePresent bit field value
         * and update the related Bluetooth GATT Characteristic.
         *
         * @param value Value to write to FirstCrankMeasurementAnglePresent bit field
         * @return      True, if the value has been set successfully
         */
        public boolean setFirstCrankMeasurementAnglePresent(int value) {
            if (!setValue(1, 1, value)) {
                return false;
            }
            updateGattCharacteristic();
            return true;
        }

        /**
         * Get InstantaneousForceMagnitudeArrayPresent bit field value.
         *
         * @return Bit field value
         */
        public int getInstantaneousForceMagnitudeArrayPresent() {
            return getValue(2, 2);
        }

        /**
         * Set InstantaneousForceMagnitudeArrayPresent bit field value
         * and update the related Bluetooth GATT Characteristic.
         *
         * @param value Value to write to InstantaneousForceMagnitudeArrayPresent bit field
         * @return      True, if the value has been set successfully
         */
        public boolean setInstantaneousForceMagnitudeArrayPresent(int value) {
            if (!setValue(2, 2, value)) {
                return false;
            }
            updateGattCharacteristic();
            return true;
        }

        /**
         * Get InstantaneousTorqueMagnitudeArrayPresent bit field value.
         *
         * @return Bit field value
         */
        public int getInstantaneousTorqueMagnitudeArrayPresent() {
            return getValue(3, 3);
        }

        /**
         * Set InstantaneousTorqueMagnitudeArrayPresent bit field value
         * and update the related Bluetooth GATT Characteristic.
         *
         * @param value Value to write to InstantaneousTorqueMagnitudeArrayPresent bit field
         * @return      True, if the value has been set successfully
         */
        public boolean setInstantaneousTorqueMagnitudeArrayPresent(int value) {
            if (!setValue(3, 3, value)) {
                return false;
            }
            updateGattCharacteristic();
            return true;
        }

        /**
         * Get InstantaneousMeasurementDirection bit field value.
         *
         * @return Bit field value
         */
        public int getInstantaneousMeasurementDirection() {
            return getValue(4, 5);
        }

        /**
         * Set InstantaneousMeasurementDirection bit field value
         * and update the related Bluetooth GATT Characteristic.
         *
         * @param value Value to write to InstantaneousMeasurementDirection bit field
         * @return      True, if the value has been set successfully
         */
        public boolean setInstantaneousMeasurementDirection(int value) {
            if (!setValue(4, 5, value)) {
                return false;
            }
            updateGattCharacteristic();
            return true;
        }
    }

    // Customized Start: Functions .................................................................

    //................................................................. Customized End: Functions //
}

