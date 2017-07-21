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
 * Public API for the RSC Measurement Bluetooth GATT Characteristic.
 *
 * <p>This class provides RSC Measurement Bluetooth GATT Characteristic
 * value encode/decode functions. Allowing applications easy and quick to
 * read/write characteristic field value.
 *
 * <p>Name: RSC Measurement
 * Type: org.bluetooth.characteristic.rsc_measurement
 * UUID: 2A53
 * Last Modified: 2012-09-29
 * Revision: None
 */
public class RscMeasurement extends CharacteristicBase {
    /**
     * RSC Measurement UUID.
     */
    public static final UUID GATT_UUID =
            UUID.fromString(FormatUtils.uuid16ToUuid128("2A53"));

    // Customized Start: Constant Definition .......................................................

    //....................................................... Customized End: Constant Definition //

    /*
     * Field: Flags
     * Requirement: Mandatory
     * Format: bit8
     */
    private BfFlags mFlags = new BfFlags();

    /*
     * Field: Instantaneous Speed
     * Requirement: Mandatory
     * Format: uint16
     * Unit: org.bluetooth.unit.velocity.metres_per_second
     */
    private byte[] mInstantaneousSpeed = new byte[FormatUtils.UINT16_SIZE];

    /*
     * Field: Instantaneous Cadence
     * Requirement: Mandatory
     * Format: uint8
     * Unit: org.bluetooth.unit.angular_velocity.revolution_per_minute
     */
    private byte[] mInstantaneousCadence = new byte[FormatUtils.UINT8_SIZE];

    /*
     * Field: Instantaneous Stride Length
     * Requirement: C1
     * Format: uint16
     * Unit: org.bluetooth.unit.length.meter
     */
    private byte[] mInstantaneousStrideLength = new byte[FormatUtils.UINT16_SIZE];

    /*
     * Field: Total Distance
     * Requirement: C2
     * Format: uint32
     * Unit: org.bluetooth.unit.length.meter
     */
    private byte[] mTotalDistance = new byte[FormatUtils.UINT32_SIZE];


    /**
     * Create a RscMeasurement characteristic object.
     */
    public RscMeasurement() {
        setCharacteristic(null);
        setFlags(new BfFlags());
        setInstantaneousSpeed(0);
        setInstantaneousCadence(0);
        setInstantaneousStrideLength(0);
        setTotalDistance(0);
    }

    /**
     * Create a RscMeasurement characteristic object and init value.
     *
     * @param value Initial value
     */
    public RscMeasurement(byte[] value) {
        setCharacteristic(null);
        setValue(value);
    }

    /**
     * Create a RscMeasurement characteristic object and init value.
     *
     * @param value Initial value
     * @param characteristic the gatt-characteristic this characteristic belongs to.
     */
    public RscMeasurement(byte[] value, BluetoothGattCharacteristic characteristic) {
        setCharacteristic(characteristic);
        setValue(value);
    }

    /**
     * Create a RscMeasurement characteristic object.
     *
     * @param flags Flags
     * @param instantaneousSpeed Instantaneous Speed
     * @param instantaneousCadence Instantaneous Cadence
     * @param instantaneousStrideLength Instantaneous Stride Length
     * @param totalDistance Total Distance
     */
    public RscMeasurement(
            BfFlags flags,
            int instantaneousSpeed,
            int instantaneousCadence,
            int instantaneousStrideLength,
            long totalDistance) {
        setCharacteristic(null);
        setFlags(flags);
        setInstantaneousSpeed(instantaneousSpeed);
        setInstantaneousCadence(instantaneousCadence);
        setInstantaneousStrideLength(instantaneousStrideLength);
        setTotalDistance(totalDistance);
    }

    /**
     * Create a RscMeasurement characteristic object.
     *
     * @param flags Flags
     * @param instantaneousSpeed Instantaneous Speed
     * @param instantaneousCadence Instantaneous Cadence
     * @param instantaneousStrideLength Instantaneous Stride Length
     * @param totalDistance Total Distance
     * @param characteristic the gatt-characteristic this characteristic belongs to.
     */
    public RscMeasurement(
            BfFlags flags,
            int instantaneousSpeed,
            int instantaneousCadence,
            int instantaneousStrideLength,
            long totalDistance,
            BluetoothGattCharacteristic characteristic) {
        setCharacteristic(characteristic);
        setFlags(flags);
        setInstantaneousSpeed(instantaneousSpeed);
        setInstantaneousCadence(instantaneousCadence);
        setInstantaneousStrideLength(instantaneousStrideLength);
        setTotalDistance(totalDistance);
    }

    // Customized Start: Constructors ..............................................................

    //.............................................................. Customized End: Constructors //

    /**
     * Get RscMeasurement characteristic byte length.
     *
     * @return Byte length of this characteristic
     */
    public int length() {
        return (isSupportFlags()
                        ? mFlags.length() : 0)
                + (isSupportInstantaneousSpeed()
                        ? mInstantaneousSpeed.length : 0)
                + (isSupportInstantaneousCadence()
                        ? mInstantaneousCadence.length : 0)
                + (isSupportInstantaneousStrideLength()
                        ? mInstantaneousStrideLength.length : 0)
                + (isSupportTotalDistance()
                        ? mTotalDistance.length : 0);
    }

    /**
     * Get RscMeasurement characteristic UUID.
     *
     * @return Characteristic UUID
     */
    @Override
    public UUID getUuid() {
        return GATT_UUID;
    }

    /**
     * Get RscMeasurement characteristic value.
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

        if (isSupportInstantaneousSpeed()) {
            int fieldLen = mInstantaneousSpeed.length;
            System.arraycopy(mInstantaneousSpeed, 0, value, dstPos, fieldLen);
            dstPos += fieldLen;
        }

        if (isSupportInstantaneousCadence()) {
            int fieldLen = mInstantaneousCadence.length;
            System.arraycopy(mInstantaneousCadence, 0, value, dstPos, fieldLen);
            dstPos += fieldLen;
        }

        if (isSupportInstantaneousStrideLength()) {
            int fieldLen = mInstantaneousStrideLength.length;
            System.arraycopy(mInstantaneousStrideLength, 0, value, dstPos, fieldLen);
            dstPos += fieldLen;
        }

        if (isSupportTotalDistance()) {
            int fieldLen = mTotalDistance.length;
            System.arraycopy(mTotalDistance, 0, value, dstPos, fieldLen);
            dstPos += fieldLen;
        }

        return value;
    }

    /**
     * Set RscMeasurement characteristic value.
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

        if (isSupportInstantaneousSpeed()) {
            int fieldLen = mInstantaneousSpeed.length;

            if (!setValueRangeCheck(value.length, srcPos, fieldLen)) {
                return false;
            }

            System.arraycopy(value, srcPos, mInstantaneousSpeed, 0, fieldLen);
            srcPos += fieldLen;
        }

        if (isSupportInstantaneousCadence()) {
            int fieldLen = mInstantaneousCadence.length;

            if (!setValueRangeCheck(value.length, srcPos, fieldLen)) {
                return false;
            }

            System.arraycopy(value, srcPos, mInstantaneousCadence, 0, fieldLen);
            srcPos += fieldLen;
        }

        if (isSupportInstantaneousStrideLength()) {
            int fieldLen = mInstantaneousStrideLength.length;

            if (!setValueRangeCheck(value.length, srcPos, fieldLen)) {
                return false;
            }

            System.arraycopy(value, srcPos, mInstantaneousStrideLength, 0, fieldLen);
            srcPos += fieldLen;
        }

        if (isSupportTotalDistance()) {
            int fieldLen = mTotalDistance.length;

            if (!setValueRangeCheck(value.length, srcPos, fieldLen)) {
                return false;
            }

            System.arraycopy(value, srcPos, mTotalDistance, 0, fieldLen);
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
     * Return true if RscMeasurement support Flags field.
     *
     * @return  True, if RscMeasurement support Flags field.
     */
    public boolean isSupportFlags() {
        return true;
    }

    /**
     * Get InstantaneousSpeed field value with int format.
     *
     * @return InstantaneousSpeed field value
     */
    public int getInstantaneousSpeed() {
        return FormatUtils.uint16ToInt(mInstantaneousSpeed);
    }

    /**
     * Set InstantaneousSpeed field value by int format
     * and update the related Bluetooth GATT Characteristic.
     *
     * @param value Value to write to InstantaneousSpeed field
     * @return      True, if the value has been set successfully
     */
    public boolean setInstantaneousSpeed(int value) {
        if (!FormatUtils.uint16RangeCheck(value)) {
            return false;
        }
        mInstantaneousSpeed = FormatUtils.intToUint16(value);
        updateGattCharacteristic();
        return true;
    }

    /**
     * Return true if RscMeasurement support InstantaneousSpeed field.
     *
     * @return  True, if RscMeasurement support InstantaneousSpeed field.
     */
    public boolean isSupportInstantaneousSpeed() {
        return true;
    }

    /**
     * Get InstantaneousCadence field value with int format.
     *
     * @return InstantaneousCadence field value
     */
    public int getInstantaneousCadence() {
        return FormatUtils.uint8ToInt(mInstantaneousCadence);
    }

    /**
     * Set InstantaneousCadence field value by int format
     * and update the related Bluetooth GATT Characteristic.
     *
     * @param value Value to write to InstantaneousCadence field
     * @return      True, if the value has been set successfully
     */
    public boolean setInstantaneousCadence(int value) {
        if (!FormatUtils.uint8RangeCheck(value)) {
            return false;
        }
        mInstantaneousCadence = FormatUtils.intToUint8(value);
        updateGattCharacteristic();
        return true;
    }

    /**
     * Return true if RscMeasurement support InstantaneousCadence field.
     *
     * @return  True, if RscMeasurement support InstantaneousCadence field.
     */
    public boolean isSupportInstantaneousCadence() {
        return true;
    }

    /**
     * Get InstantaneousStrideLength field value with int format.
     *
     * @return InstantaneousStrideLength field value
     */
    public int getInstantaneousStrideLength() {
        return FormatUtils.uint16ToInt(mInstantaneousStrideLength);
    }

    /**
     * Set InstantaneousStrideLength field value by int format
     * and update the related Bluetooth GATT Characteristic.
     *
     * @param value Value to write to InstantaneousStrideLength field
     * @return      True, if the value has been set successfully
     */
    public boolean setInstantaneousStrideLength(int value) {
        if (!FormatUtils.uint16RangeCheck(value)) {
            return false;
        }
        mInstantaneousStrideLength = FormatUtils.intToUint16(value);
        updateGattCharacteristic();
        return true;
    }

    /**
     * Return true if RscMeasurement support InstantaneousStrideLength field.
     *
     * @return  True, if RscMeasurement support InstantaneousStrideLength field.
     */
    public boolean isSupportInstantaneousStrideLength() {
        return (mFlags.getInstantaneousStrideLengthPresent() == 1);
    }

    /**
     * Get TotalDistance field value with long format.
     *
     * @return TotalDistance field value
     */
    public long getTotalDistance() {
        return FormatUtils.uint32ToLong(mTotalDistance);
    }

    /**
     * Set TotalDistance field value by long format
     * and update the related Bluetooth GATT Characteristic.
     *
     * @param value Value to write to TotalDistance field
     * @return      True, if the value has been set successfully
     */
    public boolean setTotalDistance(long value) {
        if (!FormatUtils.uint32RangeCheck(value)) {
            return false;
        }
        mTotalDistance = FormatUtils.longToUint32(value);
        updateGattCharacteristic();
        return true;
    }

    /**
     * Return true if RscMeasurement support TotalDistance field.
     *
     * @return  True, if RscMeasurement support TotalDistance field.
     */
    public boolean isSupportTotalDistance() {
        return (mFlags.getTotalDistancePresent() == 1);
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
         * Get InstantaneousStrideLengthPresent bit field value.
         *
         * @return Bit field value
         */
        public int getInstantaneousStrideLengthPresent() {
            return getValue(0, 0);
        }

        /**
         * Set InstantaneousStrideLengthPresent bit field value
         * and update the related Bluetooth GATT Characteristic.
         *
         * @param value Value to write to InstantaneousStrideLengthPresent bit field
         * @return      True, if the value has been set successfully
         */
        public boolean setInstantaneousStrideLengthPresent(int value) {
            if (!setValue(0, 0, value)) {
                return false;
            }
            updateGattCharacteristic();
            return true;
        }

        /**
         * Get TotalDistancePresent bit field value.
         *
         * @return Bit field value
         */
        public int getTotalDistancePresent() {
            return getValue(1, 1);
        }

        /**
         * Set TotalDistancePresent bit field value
         * and update the related Bluetooth GATT Characteristic.
         *
         * @param value Value to write to TotalDistancePresent bit field
         * @return      True, if the value has been set successfully
         */
        public boolean setTotalDistancePresent(int value) {
            if (!setValue(1, 1, value)) {
                return false;
            }
            updateGattCharacteristic();
            return true;
        }

        /**
         * Get WalkingOrRunningStatusBits bit field value.
         *
         * @return Bit field value
         */
        public int getWalkingOrRunningStatusBits() {
            return getValue(2, 2);
        }

        /**
         * Set WalkingOrRunningStatusBits bit field value
         * and update the related Bluetooth GATT Characteristic.
         *
         * @param value Value to write to WalkingOrRunningStatusBits bit field
         * @return      True, if the value has been set successfully
         */
        public boolean setWalkingOrRunningStatusBits(int value) {
            if (!setValue(2, 2, value)) {
                return false;
            }
            updateGattCharacteristic();
            return true;
        }
    }

    // Customized Start: Functions .................................................................

    //................................................................. Customized End: Functions //
}

