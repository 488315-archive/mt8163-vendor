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
 * Public API for the CSC Measurement Bluetooth GATT Characteristic.
 *
 * <p>This class provides CSC Measurement Bluetooth GATT Characteristic
 * value encode/decode functions. Allowing applications easy and quick to
 * read/write characteristic field value.
 *
 * <p>Name: CSC Measurement
 * Type: org.bluetooth.characteristic.csc_measurement
 * UUID: 2A5B
 * Last Modified: 2012-04-12
 * Revision: None
 */
public class CscMeasurement extends CharacteristicBase {
    /**
     * CSC Measurement UUID.
     */
    public static final UUID GATT_UUID =
            UUID.fromString(FormatUtils.uuid16ToUuid128("2A5B"));

    // Customized Start: Constant Definition .......................................................
    public static final int FLAGS_WHEEL_REVOLUTION_DATA_PRESENT = 1 << 0;
    public static final int FLAGS_CRANK_REVOLUTION_DATA_PRESENT = 1 << 1;
    //....................................................... Customized End: Constant Definition //

    /*
     * Field: Flags
     * Requirement: Mandatory
     * Format: bit8
     */
    private BfFlags mFlags = new BfFlags();

    /*
     * Field: Cumulative Wheel Revolutions
     * Requirement: C1
     * Format: uint32
     * Unit: org.bluetooth.unit.unitless
     */
    private byte[] mCumulativeWheelRevolutions = new byte[FormatUtils.UINT32_SIZE];

    /*
     * Field: Last Wheel Event Time
     * Requirement: C1
     * Format: uint16
     * Unit: org.bluetooth.unit.time.second
     */
    private byte[] mLastWheelEventTime = new byte[FormatUtils.UINT16_SIZE];

    /*
     * Field: Cumulative Crank Revolutions
     * Requirement: C2
     * Format: uint16
     * Unit: org.bluetooth.unit.unitless
     */
    private byte[] mCumulativeCrankRevolutions = new byte[FormatUtils.UINT16_SIZE];

    /*
     * Field: Last Crank Event Time
     * Requirement: C2
     * Format: uint16
     * Unit: org.bluetooth.unit.time.second
     */
    private byte[] mLastCrankEventTime = new byte[FormatUtils.UINT16_SIZE];


    /**
     * Create a CscMeasurement characteristic object.
     */
    public CscMeasurement() {
        setCharacteristic(null);
        setFlags(new BfFlags());
        setCumulativeWheelRevolutions(0);
        setLastWheelEventTime(0);
        setCumulativeCrankRevolutions(0);
        setLastCrankEventTime(0);
    }

    /**
     * Create a CscMeasurement characteristic object and init value.
     *
     * @param value Initial value
     */
    public CscMeasurement(byte[] value) {
        setCharacteristic(null);
        setValue(value);
    }

    /**
     * Create a CscMeasurement characteristic object and init value.
     *
     * @param value Initial value
     * @param characteristic the gatt-characteristic this characteristic belongs to.
     */
    public CscMeasurement(byte[] value, BluetoothGattCharacteristic characteristic) {
        setCharacteristic(characteristic);
        setValue(value);
    }

    /**
     * Create a CscMeasurement characteristic object.
     *
     * @param flags Flags
     * @param cumulativeWheelRevolutions Cumulative Wheel Revolutions
     * @param lastWheelEventTime Last Wheel Event Time
     * @param cumulativeCrankRevolutions Cumulative Crank Revolutions
     * @param lastCrankEventTime Last Crank Event Time
     */
    public CscMeasurement(
            BfFlags flags,
            long cumulativeWheelRevolutions,
            int lastWheelEventTime,
            int cumulativeCrankRevolutions,
            int lastCrankEventTime) {
        setCharacteristic(null);
        setFlags(flags);
        setCumulativeWheelRevolutions(cumulativeWheelRevolutions);
        setLastWheelEventTime(lastWheelEventTime);
        setCumulativeCrankRevolutions(cumulativeCrankRevolutions);
        setLastCrankEventTime(lastCrankEventTime);
    }

    /**
     * Create a CscMeasurement characteristic object.
     *
     * @param flags Flags
     * @param cumulativeWheelRevolutions Cumulative Wheel Revolutions
     * @param lastWheelEventTime Last Wheel Event Time
     * @param cumulativeCrankRevolutions Cumulative Crank Revolutions
     * @param lastCrankEventTime Last Crank Event Time
     * @param characteristic the gatt-characteristic this characteristic belongs to.
     */
    public CscMeasurement(
            BfFlags flags,
            long cumulativeWheelRevolutions,
            int lastWheelEventTime,
            int cumulativeCrankRevolutions,
            int lastCrankEventTime,
            BluetoothGattCharacteristic characteristic) {
        setCharacteristic(characteristic);
        setFlags(flags);
        setCumulativeWheelRevolutions(cumulativeWheelRevolutions);
        setLastWheelEventTime(lastWheelEventTime);
        setCumulativeCrankRevolutions(cumulativeCrankRevolutions);
        setLastCrankEventTime(lastCrankEventTime);
    }

    // Customized Start: Constructors ..............................................................
    /**
     * Create a CscMeasurement characteristic object.
     *
     * @param flags Flags
     * @param cumulativeWheelRevolutions Cumulative Wheel Revolutions
     * @param lastWheelEventTime Last Wheel Event Time
     * @param cumulativeCrankRevolutions Cumulative Crank Revolutions
     * @param lastCrankEventTime Last Crank Event Time
     */
    public CscMeasurement(
            int flags,
            long cumulativeWheelRevolutions,
            int lastWheelEventTime,
            int cumulativeCrankRevolutions,
            int lastCrankEventTime) {
        setCharacteristic(null);
        if ((flags & FLAGS_WHEEL_REVOLUTION_DATA_PRESENT) != 0) {
            getFlags().setWheelRevolutionDataPresent(1);
        }
        if ((flags & FLAGS_CRANK_REVOLUTION_DATA_PRESENT) != 0) {
            getFlags().setCrankRevolutionDataPresent(1);
        }
        setCumulativeWheelRevolutions(cumulativeWheelRevolutions);
        setLastWheelEventTime(lastWheelEventTime);
        setCumulativeCrankRevolutions(cumulativeCrankRevolutions);
        setLastCrankEventTime(lastCrankEventTime);
    }
    //.............................................................. Customized End: Constructors //

    /**
     * Get CscMeasurement characteristic byte length.
     *
     * @return Byte length of this characteristic
     */
    public int length() {
        return (isSupportFlags()
                        ? mFlags.length() : 0)
                + (isSupportCumulativeWheelRevolutions()
                        ? mCumulativeWheelRevolutions.length : 0)
                + (isSupportLastWheelEventTime()
                        ? mLastWheelEventTime.length : 0)
                + (isSupportCumulativeCrankRevolutions()
                        ? mCumulativeCrankRevolutions.length : 0)
                + (isSupportLastCrankEventTime()
                        ? mLastCrankEventTime.length : 0);
    }

    /**
     * Get CscMeasurement characteristic UUID.
     *
     * @return Characteristic UUID
     */
    @Override
    public UUID getUuid() {
        return GATT_UUID;
    }

    /**
     * Get CscMeasurement characteristic value.
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

        if (isSupportCumulativeWheelRevolutions()) {
            int fieldLen = mCumulativeWheelRevolutions.length;
            System.arraycopy(mCumulativeWheelRevolutions, 0, value, dstPos, fieldLen);
            dstPos += fieldLen;
        }

        if (isSupportLastWheelEventTime()) {
            int fieldLen = mLastWheelEventTime.length;
            System.arraycopy(mLastWheelEventTime, 0, value, dstPos, fieldLen);
            dstPos += fieldLen;
        }

        if (isSupportCumulativeCrankRevolutions()) {
            int fieldLen = mCumulativeCrankRevolutions.length;
            System.arraycopy(mCumulativeCrankRevolutions, 0, value, dstPos, fieldLen);
            dstPos += fieldLen;
        }

        if (isSupportLastCrankEventTime()) {
            int fieldLen = mLastCrankEventTime.length;
            System.arraycopy(mLastCrankEventTime, 0, value, dstPos, fieldLen);
            dstPos += fieldLen;
        }

        return value;
    }

    /**
     * Set CscMeasurement characteristic value.
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

        if (isSupportCumulativeWheelRevolutions()) {
            int fieldLen = mCumulativeWheelRevolutions.length;

            if (!setValueRangeCheck(value.length, srcPos, fieldLen)) {
                return false;
            }

            System.arraycopy(value, srcPos, mCumulativeWheelRevolutions, 0, fieldLen);
            srcPos += fieldLen;
        }

        if (isSupportLastWheelEventTime()) {
            int fieldLen = mLastWheelEventTime.length;

            if (!setValueRangeCheck(value.length, srcPos, fieldLen)) {
                return false;
            }

            System.arraycopy(value, srcPos, mLastWheelEventTime, 0, fieldLen);
            srcPos += fieldLen;
        }

        if (isSupportCumulativeCrankRevolutions()) {
            int fieldLen = mCumulativeCrankRevolutions.length;

            if (!setValueRangeCheck(value.length, srcPos, fieldLen)) {
                return false;
            }

            System.arraycopy(value, srcPos, mCumulativeCrankRevolutions, 0, fieldLen);
            srcPos += fieldLen;
        }

        if (isSupportLastCrankEventTime()) {
            int fieldLen = mLastCrankEventTime.length;

            if (!setValueRangeCheck(value.length, srcPos, fieldLen)) {
                return false;
            }

            System.arraycopy(value, srcPos, mLastCrankEventTime, 0, fieldLen);
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
     * Return true if CscMeasurement support Flags field.
     *
     * @return  True, if CscMeasurement support Flags field.
     */
    public boolean isSupportFlags() {
        return true;
    }

    /**
     * Get CumulativeWheelRevolutions field value with long format.
     *
     * @return CumulativeWheelRevolutions field value
     */
    public long getCumulativeWheelRevolutions() {
        return FormatUtils.uint32ToLong(mCumulativeWheelRevolutions);
    }

    /**
     * Set CumulativeWheelRevolutions field value by long format
     * and update the related Bluetooth GATT Characteristic.
     *
     * @param value Value to write to CumulativeWheelRevolutions field
     * @return      True, if the value has been set successfully
     */
    public boolean setCumulativeWheelRevolutions(long value) {
        if (!FormatUtils.uint32RangeCheck(value)) {
            return false;
        }
        mCumulativeWheelRevolutions = FormatUtils.longToUint32(value);
        updateGattCharacteristic();
        return true;
    }

    /**
     * Return true if CscMeasurement support CumulativeWheelRevolutions field.
     *
     * @return  True, if CscMeasurement support CumulativeWheelRevolutions field.
     */
    public boolean isSupportCumulativeWheelRevolutions() {
        return (mFlags.getWheelRevolutionDataPresent() == 1);
    }

    /**
     * Get LastWheelEventTime field value with int format.
     *
     * @return LastWheelEventTime field value
     */
    public int getLastWheelEventTime() {
        return FormatUtils.uint16ToInt(mLastWheelEventTime);
    }

    /**
     * Set LastWheelEventTime field value by int format
     * and update the related Bluetooth GATT Characteristic.
     *
     * @param value Value to write to LastWheelEventTime field
     * @return      True, if the value has been set successfully
     */
    public boolean setLastWheelEventTime(int value) {
        if (!FormatUtils.uint16RangeCheck(value)) {
            return false;
        }
        mLastWheelEventTime = FormatUtils.intToUint16(value);
        updateGattCharacteristic();
        return true;
    }

    /**
     * Return true if CscMeasurement support LastWheelEventTime field.
     *
     * @return  True, if CscMeasurement support LastWheelEventTime field.
     */
    public boolean isSupportLastWheelEventTime() {
        return (mFlags.getWheelRevolutionDataPresent() == 1);
    }

    /**
     * Get CumulativeCrankRevolutions field value with int format.
     *
     * @return CumulativeCrankRevolutions field value
     */
    public int getCumulativeCrankRevolutions() {
        return FormatUtils.uint16ToInt(mCumulativeCrankRevolutions);
    }

    /**
     * Set CumulativeCrankRevolutions field value by int format
     * and update the related Bluetooth GATT Characteristic.
     *
     * @param value Value to write to CumulativeCrankRevolutions field
     * @return      True, if the value has been set successfully
     */
    public boolean setCumulativeCrankRevolutions(int value) {
        if (!FormatUtils.uint16RangeCheck(value)) {
            return false;
        }
        mCumulativeCrankRevolutions = FormatUtils.intToUint16(value);
        updateGattCharacteristic();
        return true;
    }

    /**
     * Return true if CscMeasurement support CumulativeCrankRevolutions field.
     *
     * @return  True, if CscMeasurement support CumulativeCrankRevolutions field.
     */
    public boolean isSupportCumulativeCrankRevolutions() {
        return (mFlags.getCrankRevolutionDataPresent() == 1);
    }

    /**
     * Get LastCrankEventTime field value with int format.
     *
     * @return LastCrankEventTime field value
     */
    public int getLastCrankEventTime() {
        return FormatUtils.uint16ToInt(mLastCrankEventTime);
    }

    /**
     * Set LastCrankEventTime field value by int format
     * and update the related Bluetooth GATT Characteristic.
     *
     * @param value Value to write to LastCrankEventTime field
     * @return      True, if the value has been set successfully
     */
    public boolean setLastCrankEventTime(int value) {
        if (!FormatUtils.uint16RangeCheck(value)) {
            return false;
        }
        mLastCrankEventTime = FormatUtils.intToUint16(value);
        updateGattCharacteristic();
        return true;
    }

    /**
     * Return true if CscMeasurement support LastCrankEventTime field.
     *
     * @return  True, if CscMeasurement support LastCrankEventTime field.
     */
    public boolean isSupportLastCrankEventTime() {
        return (mFlags.getCrankRevolutionDataPresent() == 1);
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
         * Get WheelRevolutionDataPresent bit field value.
         *
         * @return Bit field value
         */
        public int getWheelRevolutionDataPresent() {
            return getValue(0, 0);
        }

        /**
         * Set WheelRevolutionDataPresent bit field value
         * and update the related Bluetooth GATT Characteristic.
         *
         * @param value Value to write to WheelRevolutionDataPresent bit field
         * @return      True, if the value has been set successfully
         */
        public boolean setWheelRevolutionDataPresent(int value) {
            if (!setValue(0, 0, value)) {
                return false;
            }
            updateGattCharacteristic();
            return true;
        }

        /**
         * Get CrankRevolutionDataPresent bit field value.
         *
         * @return Bit field value
         */
        public int getCrankRevolutionDataPresent() {
            return getValue(1, 1);
        }

        /**
         * Set CrankRevolutionDataPresent bit field value
         * and update the related Bluetooth GATT Characteristic.
         *
         * @param value Value to write to CrankRevolutionDataPresent bit field
         * @return      True, if the value has been set successfully
         */
        public boolean setCrankRevolutionDataPresent(int value) {
            if (!setValue(1, 1, value)) {
                return false;
            }
            updateGattCharacteristic();
            return true;
        }
    }

    // Customized Start: Functions .................................................................

    //................................................................. Customized End: Functions //
}

