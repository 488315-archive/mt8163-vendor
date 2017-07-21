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
 * Public API for the Navigation Bluetooth GATT Characteristic.
 *
 * <p>This class provides Navigation Bluetooth GATT Characteristic
 * value encode/decode functions. Allowing applications easy and quick to
 * read/write characteristic field value.
 *
 * <p>Name: Navigation
 * Type: org.bluetooth.characteristic.navigation
 * UUID: 2A68
 * Last Modified: 2013-05-13
 * Revision: None
 */
public class Navigation extends CharacteristicBase {
    /**
     * Navigation UUID.
     */
    public static final UUID GATT_UUID =
            UUID.fromString(FormatUtils.uuid16ToUuid128("2A68"));

    // Customized Start: Constant Definition .......................................................

    //....................................................... Customized End: Constant Definition //

    /*
     * Field: Flags
     * Requirement: Mandatory
     * Format: bit16
     */
    private BfFlags mFlags = new BfFlags();

    /*
     * Field: Bearing
     * Requirement: Mandatory
     * Format: uint16
     * Unit: org.bluetooth.unit.plane_angle.degree
     */
    private byte[] mBearing = new byte[FormatUtils.UINT16_SIZE];

    /*
     * Field: Heading
     * Requirement: Mandatory
     * Format: uint16
     * Unit: org.bluetooth.unit.plane_angle.degree
     */
    private byte[] mHeading = new byte[FormatUtils.UINT16_SIZE];

    /*
     * Field: Remaining Distance
     * Requirement: Optional
     * Format: uint24
     * Unit: org.bluetooth.unit.length.meter
     */
    private byte[] mRemainingDistance = new byte[FormatUtils.UINT24_SIZE];

    /*
     * Field: Remaining Vertical Distance
     * Requirement: Optional
     * Format: sint24
     * Unit: org.bluetooth.unit.length.meter
     */
    private byte[] mRemainingVerticalDistance = new byte[FormatUtils.SINT24_SIZE];

    /*
     * Field: Estimated Time of Arrival
     * Requirement: Optional
     * Reference: org.bluetooth.characteristic.date_time
     */
    private DateTime mEstimatedTimeOfArrival = new DateTime();


    /**
     * Create a Navigation characteristic object.
     */
    public Navigation() {
        setCharacteristic(null);
        setFlags(new BfFlags());
        setBearing(0);
        setHeading(0);
        setRemainingDistance(0);
        setRemainingVerticalDistance(0);
    }

    /**
     * Create a Navigation characteristic object and init value.
     *
     * @param value Initial value
     */
    public Navigation(byte[] value) {
        setCharacteristic(null);
        setValue(value);
    }

    /**
     * Create a Navigation characteristic object and init value.
     *
     * @param value Initial value
     * @param characteristic the gatt-characteristic this characteristic belongs to.
     */
    public Navigation(byte[] value, BluetoothGattCharacteristic characteristic) {
        setCharacteristic(characteristic);
        setValue(value);
    }

    /**
     * Create a Navigation characteristic object.
     *
     * @param flags Flags
     * @param bearing Bearing
     * @param heading Heading
     * @param remainingDistance Remaining Distance
     * @param remainingVerticalDistance Remaining Vertical Distance
     * @param estimatedTimeOfArrival Estimated Time of Arrival
     */
    public Navigation(
            BfFlags flags,
            int bearing,
            int heading,
            int remainingDistance,
            int remainingVerticalDistance,
            DateTime estimatedTimeOfArrival) {
        setCharacteristic(null);
        setFlags(flags);
        setBearing(bearing);
        setHeading(heading);
        setRemainingDistance(remainingDistance);
        setRemainingVerticalDistance(remainingVerticalDistance);
        setEstimatedTimeOfArrival(estimatedTimeOfArrival);
    }

    /**
     * Create a Navigation characteristic object.
     *
     * @param flags Flags
     * @param bearing Bearing
     * @param heading Heading
     * @param remainingDistance Remaining Distance
     * @param remainingVerticalDistance Remaining Vertical Distance
     * @param estimatedTimeOfArrival Estimated Time of Arrival
     * @param characteristic the gatt-characteristic this characteristic belongs to.
     */
    public Navigation(
            BfFlags flags,
            int bearing,
            int heading,
            int remainingDistance,
            int remainingVerticalDistance,
            DateTime estimatedTimeOfArrival,
            BluetoothGattCharacteristic characteristic) {
        setCharacteristic(characteristic);
        setFlags(flags);
        setBearing(bearing);
        setHeading(heading);
        setRemainingDistance(remainingDistance);
        setRemainingVerticalDistance(remainingVerticalDistance);
        setEstimatedTimeOfArrival(estimatedTimeOfArrival);
    }

    // Customized Start: Constructors ..............................................................

    //.............................................................. Customized End: Constructors //

    /**
     * Get Navigation characteristic byte length.
     *
     * @return Byte length of this characteristic
     */
    public int length() {
        return (isSupportFlags()
                        ? mFlags.length() : 0)
                + (isSupportBearing()
                        ? mBearing.length : 0)
                + (isSupportHeading()
                        ? mHeading.length : 0)
                + (isSupportRemainingDistance()
                        ? mRemainingDistance.length : 0)
                + (isSupportRemainingVerticalDistance()
                        ? mRemainingVerticalDistance.length : 0)
                + (isSupportEstimatedTimeOfArrival()
                        ? mEstimatedTimeOfArrival.length() : 0);
    }

    /**
     * Get Navigation characteristic UUID.
     *
     * @return Characteristic UUID
     */
    @Override
    public UUID getUuid() {
        return GATT_UUID;
    }

    /**
     * Get Navigation characteristic value.
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

        if (isSupportBearing()) {
            int fieldLen = mBearing.length;
            System.arraycopy(mBearing, 0, value, dstPos, fieldLen);
            dstPos += fieldLen;
        }

        if (isSupportHeading()) {
            int fieldLen = mHeading.length;
            System.arraycopy(mHeading, 0, value, dstPos, fieldLen);
            dstPos += fieldLen;
        }

        if (isSupportRemainingDistance()) {
            int fieldLen = mRemainingDistance.length;
            System.arraycopy(mRemainingDistance, 0, value, dstPos, fieldLen);
            dstPos += fieldLen;
        }

        if (isSupportRemainingVerticalDistance()) {
            int fieldLen = mRemainingVerticalDistance.length;
            System.arraycopy(mRemainingVerticalDistance, 0, value, dstPos, fieldLen);
            dstPos += fieldLen;
        }

        if (isSupportEstimatedTimeOfArrival()) {
            int fieldLen = mEstimatedTimeOfArrival.length();
            System.arraycopy(mEstimatedTimeOfArrival.getValue(), 0, value, dstPos, fieldLen);
            dstPos += fieldLen;
        }

        return value;
    }

    /**
     * Set Navigation characteristic value.
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

        if (isSupportBearing()) {
            int fieldLen = mBearing.length;

            if (!setValueRangeCheck(value.length, srcPos, fieldLen)) {
                return false;
            }

            System.arraycopy(value, srcPos, mBearing, 0, fieldLen);
            srcPos += fieldLen;
        }

        if (isSupportHeading()) {
            int fieldLen = mHeading.length;

            if (!setValueRangeCheck(value.length, srcPos, fieldLen)) {
                return false;
            }

            System.arraycopy(value, srcPos, mHeading, 0, fieldLen);
            srcPos += fieldLen;
        }

        if (isSupportRemainingDistance()) {
            int fieldLen = mRemainingDistance.length;

            if (!setValueRangeCheck(value.length, srcPos, fieldLen)) {
                return false;
            }

            System.arraycopy(value, srcPos, mRemainingDistance, 0, fieldLen);
            srcPos += fieldLen;
        }

        if (isSupportRemainingVerticalDistance()) {
            int fieldLen = mRemainingVerticalDistance.length;

            if (!setValueRangeCheck(value.length, srcPos, fieldLen)) {
                return false;
            }

            System.arraycopy(value, srcPos, mRemainingVerticalDistance, 0, fieldLen);
            srcPos += fieldLen;
        }

        if (isSupportEstimatedTimeOfArrival()) {
            int fieldLen = mEstimatedTimeOfArrival.length();

            if (!setValueRangeCheck(value.length, srcPos, fieldLen)) {
                return false;
            }

            byte[] buf = new byte[fieldLen];

            System.arraycopy(value, srcPos, buf, 0, buf.length);
            srcPos += fieldLen;

            mEstimatedTimeOfArrival.setValue(buf);
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
     * Return true if Navigation support Flags field.
     *
     * @return  True, if Navigation support Flags field.
     */
    public boolean isSupportFlags() {
        return true;
    }

    /**
     * Get Bearing field value with int format.
     *
     * @return Bearing field value
     */
    public int getBearing() {
        return FormatUtils.uint16ToInt(mBearing);
    }

    /**
     * Set Bearing field value by int format
     * and update the related Bluetooth GATT Characteristic.
     *
     * @param value Value to write to Bearing field
     * @return      True, if the value has been set successfully
     */
    public boolean setBearing(int value) {
        if (!FormatUtils.uint16RangeCheck(value)) {
            return false;
        }
        mBearing = FormatUtils.intToUint16(value);
        updateGattCharacteristic();
        return true;
    }

    /**
     * Return true if Navigation support Bearing field.
     *
     * @return  True, if Navigation support Bearing field.
     */
    public boolean isSupportBearing() {
        return true;
    }

    /**
     * Get Heading field value with int format.
     *
     * @return Heading field value
     */
    public int getHeading() {
        return FormatUtils.uint16ToInt(mHeading);
    }

    /**
     * Set Heading field value by int format
     * and update the related Bluetooth GATT Characteristic.
     *
     * @param value Value to write to Heading field
     * @return      True, if the value has been set successfully
     */
    public boolean setHeading(int value) {
        if (!FormatUtils.uint16RangeCheck(value)) {
            return false;
        }
        mHeading = FormatUtils.intToUint16(value);
        updateGattCharacteristic();
        return true;
    }

    /**
     * Return true if Navigation support Heading field.
     *
     * @return  True, if Navigation support Heading field.
     */
    public boolean isSupportHeading() {
        return true;
    }

    /**
     * Get RemainingDistance field value with int format.
     *
     * @return RemainingDistance field value
     */
    public int getRemainingDistance() {
        return FormatUtils.uint24ToInt(mRemainingDistance);
    }

    /**
     * Set RemainingDistance field value by int format
     * and update the related Bluetooth GATT Characteristic.
     *
     * @param value Value to write to RemainingDistance field
     * @return      True, if the value has been set successfully
     */
    public boolean setRemainingDistance(int value) {
        if (!FormatUtils.uint24RangeCheck(value)) {
            return false;
        }
        mRemainingDistance = FormatUtils.intToUint24(value);
        updateGattCharacteristic();
        return true;
    }

    /**
     * Return true if Navigation support RemainingDistance field.
     *
     * @return  True, if Navigation support RemainingDistance field.
     */
    public boolean isSupportRemainingDistance() {
        return (mFlags.getRemainingDistancePresent() == 1);
    }

    /**
     * Get RemainingVerticalDistance field value with int format.
     *
     * @return RemainingVerticalDistance field value
     */
    public int getRemainingVerticalDistance() {
        return FormatUtils.sint24ToInt(mRemainingVerticalDistance);
    }

    /**
     * Set RemainingVerticalDistance field value by int format
     * and update the related Bluetooth GATT Characteristic.
     *
     * @param value Value to write to RemainingVerticalDistance field
     * @return      True, if the value has been set successfully
     */
    public boolean setRemainingVerticalDistance(int value) {
        if (!FormatUtils.sint24RangeCheck(value)) {
            return false;
        }
        mRemainingVerticalDistance = FormatUtils.intToSint24(value);
        updateGattCharacteristic();
        return true;
    }

    /**
     * Return true if Navigation support RemainingVerticalDistance field.
     *
     * @return  True, if Navigation support RemainingVerticalDistance field.
     */
    public boolean isSupportRemainingVerticalDistance() {
        return (mFlags.getRemainingVerticalDistancePresent() == 1);
    }

    /**
     * Get EstimatedTimeOfArrival field value with DateTime format.
     *
     * @return EstimatedTimeOfArrival field value
     */
    public DateTime getEstimatedTimeOfArrival() {
        return mEstimatedTimeOfArrival;
    }

    /**
     * Set EstimatedTimeOfArrival field value by byte array format
     * and update the related Bluetooth GATT Characteristic.
     *
     * @param value Value to write to EstimatedTimeOfArrival field
     * @return      True, if the value has been set successfully
     */
    public boolean setEstimatedTimeOfArrival(byte[] value) {
        if (!mEstimatedTimeOfArrival.setValue(value)) {
            return false;
        }
        updateGattCharacteristic();
        return true;
    }

    /**
     * Set EstimatedTimeOfArrival field value by DateTime format
     * and update the related Bluetooth GATT Characteristic.
     *
     * @param value Value to write to EstimatedTimeOfArrival field
     * @return      True, if the value has been set successfully
     */
    public boolean setEstimatedTimeOfArrival(DateTime value) {
        if (!mEstimatedTimeOfArrival.setValue(value.getValue())) {
            return false;
        }
        updateGattCharacteristic();
        return true;
    }

    /**
     * Return true if Navigation support EstimatedTimeOfArrival field.
     *
     * @return  True, if Navigation support EstimatedTimeOfArrival field.
     */
    public boolean isSupportEstimatedTimeOfArrival() {
        return (mFlags.getEstimatedTimeOfArrivalPresent() == 1);
    }

    /**
     * This class provides Flags BitField operations based on
     * specific definition.
     */
    public class BfFlags extends BitField {
        private static final int sLength = FormatUtils.BIT16_SIZE;

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
         * Get RemainingDistancePresent bit field value.
         *
         * @return Bit field value
         */
        public int getRemainingDistancePresent() {
            return getValue(0, 0);
        }

        /**
         * Set RemainingDistancePresent bit field value
         * and update the related Bluetooth GATT Characteristic.
         *
         * @param value Value to write to RemainingDistancePresent bit field
         * @return      True, if the value has been set successfully
         */
        public boolean setRemainingDistancePresent(int value) {
            if (!setValue(0, 0, value)) {
                return false;
            }
            updateGattCharacteristic();
            return true;
        }

        /**
         * Get RemainingVerticalDistancePresent bit field value.
         *
         * @return Bit field value
         */
        public int getRemainingVerticalDistancePresent() {
            return getValue(1, 1);
        }

        /**
         * Set RemainingVerticalDistancePresent bit field value
         * and update the related Bluetooth GATT Characteristic.
         *
         * @param value Value to write to RemainingVerticalDistancePresent bit field
         * @return      True, if the value has been set successfully
         */
        public boolean setRemainingVerticalDistancePresent(int value) {
            if (!setValue(1, 1, value)) {
                return false;
            }
            updateGattCharacteristic();
            return true;
        }

        /**
         * Get EstimatedTimeOfArrivalPresent bit field value.
         *
         * @return Bit field value
         */
        public int getEstimatedTimeOfArrivalPresent() {
            return getValue(2, 2);
        }

        /**
         * Set EstimatedTimeOfArrivalPresent bit field value
         * and update the related Bluetooth GATT Characteristic.
         *
         * @param value Value to write to EstimatedTimeOfArrivalPresent bit field
         * @return      True, if the value has been set successfully
         */
        public boolean setEstimatedTimeOfArrivalPresent(int value) {
            if (!setValue(2, 2, value)) {
                return false;
            }
            updateGattCharacteristic();
            return true;
        }

        /**
         * Get PositionStatus bit field value.
         *
         * @return Bit field value
         */
        public int getPositionStatus() {
            return getValue(3, 4);
        }

        /**
         * Set PositionStatus bit field value
         * and update the related Bluetooth GATT Characteristic.
         *
         * @param value Value to write to PositionStatus bit field
         * @return      True, if the value has been set successfully
         */
        public boolean setPositionStatus(int value) {
            if (!setValue(3, 4, value)) {
                return false;
            }
            updateGattCharacteristic();
            return true;
        }

        /**
         * Get HeadingSource bit field value.
         *
         * @return Bit field value
         */
        public int getHeadingSource() {
            return getValue(5, 5);
        }

        /**
         * Set HeadingSource bit field value
         * and update the related Bluetooth GATT Characteristic.
         *
         * @param value Value to write to HeadingSource bit field
         * @return      True, if the value has been set successfully
         */
        public boolean setHeadingSource(int value) {
            if (!setValue(5, 5, value)) {
                return false;
            }
            updateGattCharacteristic();
            return true;
        }

        /**
         * Get NavigationIndicatorType bit field value.
         *
         * @return Bit field value
         */
        public int getNavigationIndicatorType() {
            return getValue(6, 6);
        }

        /**
         * Set NavigationIndicatorType bit field value
         * and update the related Bluetooth GATT Characteristic.
         *
         * @param value Value to write to NavigationIndicatorType bit field
         * @return      True, if the value has been set successfully
         */
        public boolean setNavigationIndicatorType(int value) {
            if (!setValue(6, 6, value)) {
                return false;
            }
            updateGattCharacteristic();
            return true;
        }

        /**
         * Get WaypointReached bit field value.
         *
         * @return Bit field value
         */
        public int getWaypointReached() {
            return getValue(7, 7);
        }

        /**
         * Set WaypointReached bit field value
         * and update the related Bluetooth GATT Characteristic.
         *
         * @param value Value to write to WaypointReached bit field
         * @return      True, if the value has been set successfully
         */
        public boolean setWaypointReached(int value) {
            if (!setValue(7, 7, value)) {
                return false;
            }
            updateGattCharacteristic();
            return true;
        }

        /**
         * Get DestinationReached bit field value.
         *
         * @return Bit field value
         */
        public int getDestinationReached() {
            return getValue(8, 8);
        }

        /**
         * Set DestinationReached bit field value
         * and update the related Bluetooth GATT Characteristic.
         *
         * @param value Value to write to DestinationReached bit field
         * @return      True, if the value has been set successfully
         */
        public boolean setDestinationReached(int value) {
            if (!setValue(8, 8, value)) {
                return false;
            }
            updateGattCharacteristic();
            return true;
        }
    }

    // Customized Start: Functions .................................................................

    //................................................................. Customized End: Functions //
}

