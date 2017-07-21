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
 * Public API for the Location and Speed Characteristic Bluetooth GATT Characteristic.
 *
 * <p>This class provides Location and Speed Characteristic Bluetooth GATT Characteristic
 * value encode/decode functions. Allowing applications easy and quick to
 * read/write characteristic field value.
 *
 * <p>Name: Location and Speed Characteristic
 * Type: org.bluetooth.characteristic.location_and_speed
 * UUID: 2A67
 * Last Modified: 2013-05-13
 * Revision: None
 */
public class LocationAndSpeed extends CharacteristicBase {
    /**
     * Location and Speed Characteristic UUID.
     */
    public static final UUID GATT_UUID =
            UUID.fromString(FormatUtils.uuid16ToUuid128("2A67"));

    // Customized Start: Constant Definition .......................................................

    //....................................................... Customized End: Constant Definition //

    /*
     * Field: Flags
     * Requirement: Mandatory
     * Format: bit16
     */
    private BfFlags mFlags = new BfFlags();

    /*
     * Field: Instantaneous Speed
     * Requirement: Optional
     * Format: uint16
     * Unit: org.bluetooth.unit.velocity.metres_per_second
     */
    private byte[] mInstantaneousSpeed = new byte[FormatUtils.UINT16_SIZE];

    /*
     * Field: Total Distance
     * Requirement: Optional
     * Format: uint24
     * Unit: org.bluetooth.unit.length.meter
     */
    private byte[] mTotalDistance = new byte[FormatUtils.UINT24_SIZE];

    /*
     * Field: Location - Latitude
     * Requirement: C1
     * Format: sint32
     * Unit: org.bluetooth.unit.plane_angle.degree
     */
    private byte[] mLocationLatitude = new byte[FormatUtils.SINT32_SIZE];

    /*
     * Field: Location - Longitude
     * Requirement: C1
     * Format: sint32
     * Unit: org.bluetooth.unit.plane_angle.degree
     */
    private byte[] mLocationLongitude = new byte[FormatUtils.SINT32_SIZE];

    /*
     * Field: Elevation
     * Requirement: Optional
     * Format: sint24
     * Unit: org.bluetooth.unit.length.meter
     */
    private byte[] mElevation = new byte[FormatUtils.SINT24_SIZE];

    /*
     * Field: Heading
     * Requirement: Optional
     * Format: uint16
     * Unit: org.bluetooth.unit.plane_angle.degree
     */
    private byte[] mHeading = new byte[FormatUtils.UINT16_SIZE];

    /*
     * Field: Rolling Time
     * Requirement: Optional
     * Format: uint8
     * Unit: org.bluetooth.unit.time.second
     */
    private byte[] mRollingTime = new byte[FormatUtils.UINT8_SIZE];

    /*
     * Field: UTC Time
     * Requirement: Optional
     * Reference: org.bluetooth.characteristic.date_time
     */
    private DateTime mUtcTime = new DateTime();


    /**
     * Create a LocationAndSpeed characteristic object.
     */
    public LocationAndSpeed() {
        setCharacteristic(null);
        setFlags(new BfFlags());
        setInstantaneousSpeed(0);
        setTotalDistance(0);
        setLocationLatitude(0);
        setLocationLongitude(0);
        setElevation(0);
        setHeading(0);
        setRollingTime(0);
    }

    /**
     * Create a LocationAndSpeed characteristic object and init value.
     *
     * @param value Initial value
     */
    public LocationAndSpeed(byte[] value) {
        setCharacteristic(null);
        setValue(value);
    }

    /**
     * Create a LocationAndSpeed characteristic object and init value.
     *
     * @param value Initial value
     * @param characteristic the gatt-characteristic this characteristic belongs to.
     */
    public LocationAndSpeed(byte[] value, BluetoothGattCharacteristic characteristic) {
        setCharacteristic(characteristic);
        setValue(value);
    }

    /**
     * Create a LocationAndSpeed characteristic object.
     *
     * @param flags Flags
     * @param instantaneousSpeed Instantaneous Speed
     * @param totalDistance Total Distance
     * @param locationLatitude Location - Latitude
     * @param locationLongitude Location - Longitude
     * @param elevation Elevation
     * @param heading Heading
     * @param rollingTime Rolling Time
     * @param utcTime UTC Time
     */
    public LocationAndSpeed(
            BfFlags flags,
            int instantaneousSpeed,
            int totalDistance,
            int locationLatitude,
            int locationLongitude,
            int elevation,
            int heading,
            int rollingTime,
            DateTime utcTime) {
        setCharacteristic(null);
        setFlags(flags);
        setInstantaneousSpeed(instantaneousSpeed);
        setTotalDistance(totalDistance);
        setLocationLatitude(locationLatitude);
        setLocationLongitude(locationLongitude);
        setElevation(elevation);
        setHeading(heading);
        setRollingTime(rollingTime);
        setUtcTime(utcTime);
    }

    /**
     * Create a LocationAndSpeed characteristic object.
     *
     * @param flags Flags
     * @param instantaneousSpeed Instantaneous Speed
     * @param totalDistance Total Distance
     * @param locationLatitude Location - Latitude
     * @param locationLongitude Location - Longitude
     * @param elevation Elevation
     * @param heading Heading
     * @param rollingTime Rolling Time
     * @param utcTime UTC Time
     * @param characteristic the gatt-characteristic this characteristic belongs to.
     */
    public LocationAndSpeed(
            BfFlags flags,
            int instantaneousSpeed,
            int totalDistance,
            int locationLatitude,
            int locationLongitude,
            int elevation,
            int heading,
            int rollingTime,
            DateTime utcTime,
            BluetoothGattCharacteristic characteristic) {
        setCharacteristic(characteristic);
        setFlags(flags);
        setInstantaneousSpeed(instantaneousSpeed);
        setTotalDistance(totalDistance);
        setLocationLatitude(locationLatitude);
        setLocationLongitude(locationLongitude);
        setElevation(elevation);
        setHeading(heading);
        setRollingTime(rollingTime);
        setUtcTime(utcTime);
    }

    // Customized Start: Constructors ..............................................................

    //.............................................................. Customized End: Constructors //

    /**
     * Get LocationAndSpeed characteristic byte length.
     *
     * @return Byte length of this characteristic
     */
    public int length() {
        return (isSupportFlags()
                        ? mFlags.length() : 0)
                + (isSupportInstantaneousSpeed()
                        ? mInstantaneousSpeed.length : 0)
                + (isSupportTotalDistance()
                        ? mTotalDistance.length : 0)
                + (isSupportLocationLatitude()
                        ? mLocationLatitude.length : 0)
                + (isSupportLocationLongitude()
                        ? mLocationLongitude.length : 0)
                + (isSupportElevation()
                        ? mElevation.length : 0)
                + (isSupportHeading()
                        ? mHeading.length : 0)
                + (isSupportRollingTime()
                        ? mRollingTime.length : 0)
                + (isSupportUtcTime()
                        ? mUtcTime.length() : 0);
    }

    /**
     * Get LocationAndSpeed characteristic UUID.
     *
     * @return Characteristic UUID
     */
    @Override
    public UUID getUuid() {
        return GATT_UUID;
    }

    /**
     * Get LocationAndSpeed characteristic value.
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

        if (isSupportTotalDistance()) {
            int fieldLen = mTotalDistance.length;
            System.arraycopy(mTotalDistance, 0, value, dstPos, fieldLen);
            dstPos += fieldLen;
        }

        if (isSupportLocationLatitude()) {
            int fieldLen = mLocationLatitude.length;
            System.arraycopy(mLocationLatitude, 0, value, dstPos, fieldLen);
            dstPos += fieldLen;
        }

        if (isSupportLocationLongitude()) {
            int fieldLen = mLocationLongitude.length;
            System.arraycopy(mLocationLongitude, 0, value, dstPos, fieldLen);
            dstPos += fieldLen;
        }

        if (isSupportElevation()) {
            int fieldLen = mElevation.length;
            System.arraycopy(mElevation, 0, value, dstPos, fieldLen);
            dstPos += fieldLen;
        }

        if (isSupportHeading()) {
            int fieldLen = mHeading.length;
            System.arraycopy(mHeading, 0, value, dstPos, fieldLen);
            dstPos += fieldLen;
        }

        if (isSupportRollingTime()) {
            int fieldLen = mRollingTime.length;
            System.arraycopy(mRollingTime, 0, value, dstPos, fieldLen);
            dstPos += fieldLen;
        }

        if (isSupportUtcTime()) {
            int fieldLen = mUtcTime.length();
            System.arraycopy(mUtcTime.getValue(), 0, value, dstPos, fieldLen);
            dstPos += fieldLen;
        }

        return value;
    }

    /**
     * Set LocationAndSpeed characteristic value.
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

        if (isSupportTotalDistance()) {
            int fieldLen = mTotalDistance.length;

            if (!setValueRangeCheck(value.length, srcPos, fieldLen)) {
                return false;
            }

            System.arraycopy(value, srcPos, mTotalDistance, 0, fieldLen);
            srcPos += fieldLen;
        }

        if (isSupportLocationLatitude()) {
            int fieldLen = mLocationLatitude.length;

            if (!setValueRangeCheck(value.length, srcPos, fieldLen)) {
                return false;
            }

            System.arraycopy(value, srcPos, mLocationLatitude, 0, fieldLen);
            srcPos += fieldLen;
        }

        if (isSupportLocationLongitude()) {
            int fieldLen = mLocationLongitude.length;

            if (!setValueRangeCheck(value.length, srcPos, fieldLen)) {
                return false;
            }

            System.arraycopy(value, srcPos, mLocationLongitude, 0, fieldLen);
            srcPos += fieldLen;
        }

        if (isSupportElevation()) {
            int fieldLen = mElevation.length;

            if (!setValueRangeCheck(value.length, srcPos, fieldLen)) {
                return false;
            }

            System.arraycopy(value, srcPos, mElevation, 0, fieldLen);
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

        if (isSupportRollingTime()) {
            int fieldLen = mRollingTime.length;

            if (!setValueRangeCheck(value.length, srcPos, fieldLen)) {
                return false;
            }

            System.arraycopy(value, srcPos, mRollingTime, 0, fieldLen);
            srcPos += fieldLen;
        }

        if (isSupportUtcTime()) {
            int fieldLen = mUtcTime.length();

            if (!setValueRangeCheck(value.length, srcPos, fieldLen)) {
                return false;
            }

            byte[] buf = new byte[fieldLen];

            System.arraycopy(value, srcPos, buf, 0, buf.length);
            srcPos += fieldLen;

            mUtcTime.setValue(buf);
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
     * Return true if LocationAndSpeed support Flags field.
     *
     * @return  True, if LocationAndSpeed support Flags field.
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
     * Return true if LocationAndSpeed support InstantaneousSpeed field.
     *
     * @return  True, if LocationAndSpeed support InstantaneousSpeed field.
     */
    public boolean isSupportInstantaneousSpeed() {
        return (mFlags.getInstantaneousSpeedPresent() == 1);
    }

    /**
     * Get TotalDistance field value with int format.
     *
     * @return TotalDistance field value
     */
    public int getTotalDistance() {
        return FormatUtils.uint24ToInt(mTotalDistance);
    }

    /**
     * Set TotalDistance field value by int format
     * and update the related Bluetooth GATT Characteristic.
     *
     * @param value Value to write to TotalDistance field
     * @return      True, if the value has been set successfully
     */
    public boolean setTotalDistance(int value) {
        if (!FormatUtils.uint24RangeCheck(value)) {
            return false;
        }
        mTotalDistance = FormatUtils.intToUint24(value);
        updateGattCharacteristic();
        return true;
    }

    /**
     * Return true if LocationAndSpeed support TotalDistance field.
     *
     * @return  True, if LocationAndSpeed support TotalDistance field.
     */
    public boolean isSupportTotalDistance() {
        return (mFlags.getTotalDistancePresent() == 1);
    }

    /**
     * Get LocationLatitude field value with int format.
     *
     * @return LocationLatitude field value
     */
    public int getLocationLatitude() {
        return FormatUtils.sint32ToInt(mLocationLatitude);
    }

    /**
     * Set LocationLatitude field value by int format
     * and update the related Bluetooth GATT Characteristic.
     *
     * @param value Value to write to LocationLatitude field
     * @return      True, if the value has been set successfully
     */
    public boolean setLocationLatitude(int value) {
        if (!FormatUtils.sint32RangeCheck(value)) {
            return false;
        }
        mLocationLatitude = FormatUtils.intToSint32(value);
        updateGattCharacteristic();
        return true;
    }

    /**
     * Return true if LocationAndSpeed support LocationLatitude field.
     *
     * @return  True, if LocationAndSpeed support LocationLatitude field.
     */
    public boolean isSupportLocationLatitude() {
        return (mFlags.getLocationPresent() == 1);
    }

    /**
     * Get LocationLongitude field value with int format.
     *
     * @return LocationLongitude field value
     */
    public int getLocationLongitude() {
        return FormatUtils.sint32ToInt(mLocationLongitude);
    }

    /**
     * Set LocationLongitude field value by int format
     * and update the related Bluetooth GATT Characteristic.
     *
     * @param value Value to write to LocationLongitude field
     * @return      True, if the value has been set successfully
     */
    public boolean setLocationLongitude(int value) {
        if (!FormatUtils.sint32RangeCheck(value)) {
            return false;
        }
        mLocationLongitude = FormatUtils.intToSint32(value);
        updateGattCharacteristic();
        return true;
    }

    /**
     * Return true if LocationAndSpeed support LocationLongitude field.
     *
     * @return  True, if LocationAndSpeed support LocationLongitude field.
     */
    public boolean isSupportLocationLongitude() {
        return (mFlags.getLocationPresent() == 1);
    }

    /**
     * Get Elevation field value with int format.
     *
     * @return Elevation field value
     */
    public int getElevation() {
        return FormatUtils.sint24ToInt(mElevation);
    }

    /**
     * Set Elevation field value by int format
     * and update the related Bluetooth GATT Characteristic.
     *
     * @param value Value to write to Elevation field
     * @return      True, if the value has been set successfully
     */
    public boolean setElevation(int value) {
        if (!FormatUtils.sint24RangeCheck(value)) {
            return false;
        }
        mElevation = FormatUtils.intToSint24(value);
        updateGattCharacteristic();
        return true;
    }

    /**
     * Return true if LocationAndSpeed support Elevation field.
     *
     * @return  True, if LocationAndSpeed support Elevation field.
     */
    public boolean isSupportElevation() {
        return (mFlags.getElevationPresent() == 1);
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
     * Return true if LocationAndSpeed support Heading field.
     *
     * @return  True, if LocationAndSpeed support Heading field.
     */
    public boolean isSupportHeading() {
        return (mFlags.getHeadingPresent() == 1);
    }

    /**
     * Get RollingTime field value with int format.
     *
     * @return RollingTime field value
     */
    public int getRollingTime() {
        return FormatUtils.uint8ToInt(mRollingTime);
    }

    /**
     * Set RollingTime field value by int format
     * and update the related Bluetooth GATT Characteristic.
     *
     * @param value Value to write to RollingTime field
     * @return      True, if the value has been set successfully
     */
    public boolean setRollingTime(int value) {
        if (!FormatUtils.uint8RangeCheck(value)) {
            return false;
        }
        mRollingTime = FormatUtils.intToUint8(value);
        updateGattCharacteristic();
        return true;
    }

    /**
     * Return true if LocationAndSpeed support RollingTime field.
     *
     * @return  True, if LocationAndSpeed support RollingTime field.
     */
    public boolean isSupportRollingTime() {
        return (mFlags.getRollingTimePresent() == 1);
    }

    /**
     * Get UtcTime field value with DateTime format.
     *
     * @return UtcTime field value
     */
    public DateTime getUtcTime() {
        return mUtcTime;
    }

    /**
     * Set UtcTime field value by byte array format
     * and update the related Bluetooth GATT Characteristic.
     *
     * @param value Value to write to UtcTime field
     * @return      True, if the value has been set successfully
     */
    public boolean setUtcTime(byte[] value) {
        if (!mUtcTime.setValue(value)) {
            return false;
        }
        updateGattCharacteristic();
        return true;
    }

    /**
     * Set UtcTime field value by DateTime format
     * and update the related Bluetooth GATT Characteristic.
     *
     * @param value Value to write to UtcTime field
     * @return      True, if the value has been set successfully
     */
    public boolean setUtcTime(DateTime value) {
        if (!mUtcTime.setValue(value.getValue())) {
            return false;
        }
        updateGattCharacteristic();
        return true;
    }

    /**
     * Return true if LocationAndSpeed support UtcTime field.
     *
     * @return  True, if LocationAndSpeed support UtcTime field.
     */
    public boolean isSupportUtcTime() {
        return (mFlags.getUtcTimePresent() == 1);
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
         * Get InstantaneousSpeedPresent bit field value.
         *
         * @return Bit field value
         */
        public int getInstantaneousSpeedPresent() {
            return getValue(0, 0);
        }

        /**
         * Set InstantaneousSpeedPresent bit field value
         * and update the related Bluetooth GATT Characteristic.
         *
         * @param value Value to write to InstantaneousSpeedPresent bit field
         * @return      True, if the value has been set successfully
         */
        public boolean setInstantaneousSpeedPresent(int value) {
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
         * Get LocationPresent bit field value.
         *
         * @return Bit field value
         */
        public int getLocationPresent() {
            return getValue(2, 2);
        }

        /**
         * Set LocationPresent bit field value
         * and update the related Bluetooth GATT Characteristic.
         *
         * @param value Value to write to LocationPresent bit field
         * @return      True, if the value has been set successfully
         */
        public boolean setLocationPresent(int value) {
            if (!setValue(2, 2, value)) {
                return false;
            }
            updateGattCharacteristic();
            return true;
        }

        /**
         * Get ElevationPresent bit field value.
         *
         * @return Bit field value
         */
        public int getElevationPresent() {
            return getValue(3, 3);
        }

        /**
         * Set ElevationPresent bit field value
         * and update the related Bluetooth GATT Characteristic.
         *
         * @param value Value to write to ElevationPresent bit field
         * @return      True, if the value has been set successfully
         */
        public boolean setElevationPresent(int value) {
            if (!setValue(3, 3, value)) {
                return false;
            }
            updateGattCharacteristic();
            return true;
        }

        /**
         * Get HeadingPresent bit field value.
         *
         * @return Bit field value
         */
        public int getHeadingPresent() {
            return getValue(4, 4);
        }

        /**
         * Set HeadingPresent bit field value
         * and update the related Bluetooth GATT Characteristic.
         *
         * @param value Value to write to HeadingPresent bit field
         * @return      True, if the value has been set successfully
         */
        public boolean setHeadingPresent(int value) {
            if (!setValue(4, 4, value)) {
                return false;
            }
            updateGattCharacteristic();
            return true;
        }

        /**
         * Get RollingTimePresent bit field value.
         *
         * @return Bit field value
         */
        public int getRollingTimePresent() {
            return getValue(5, 5);
        }

        /**
         * Set RollingTimePresent bit field value
         * and update the related Bluetooth GATT Characteristic.
         *
         * @param value Value to write to RollingTimePresent bit field
         * @return      True, if the value has been set successfully
         */
        public boolean setRollingTimePresent(int value) {
            if (!setValue(5, 5, value)) {
                return false;
            }
            updateGattCharacteristic();
            return true;
        }

        /**
         * Get UtcTimePresent bit field value.
         *
         * @return Bit field value
         */
        public int getUtcTimePresent() {
            return getValue(6, 6);
        }

        /**
         * Set UtcTimePresent bit field value
         * and update the related Bluetooth GATT Characteristic.
         *
         * @param value Value to write to UtcTimePresent bit field
         * @return      True, if the value has been set successfully
         */
        public boolean setUtcTimePresent(int value) {
            if (!setValue(6, 6, value)) {
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
            return getValue(7, 8);
        }

        /**
         * Set PositionStatus bit field value
         * and update the related Bluetooth GATT Characteristic.
         *
         * @param value Value to write to PositionStatus bit field
         * @return      True, if the value has been set successfully
         */
        public boolean setPositionStatus(int value) {
            if (!setValue(7, 8, value)) {
                return false;
            }
            updateGattCharacteristic();
            return true;
        }

        /**
         * Get SpeedAndDistanceFormat bit field value.
         *
         * @return Bit field value
         */
        public int getSpeedAndDistanceFormat() {
            return getValue(9, 9);
        }

        /**
         * Set SpeedAndDistanceFormat bit field value
         * and update the related Bluetooth GATT Characteristic.
         *
         * @param value Value to write to SpeedAndDistanceFormat bit field
         * @return      True, if the value has been set successfully
         */
        public boolean setSpeedAndDistanceFormat(int value) {
            if (!setValue(9, 9, value)) {
                return false;
            }
            updateGattCharacteristic();
            return true;
        }

        /**
         * Get ElevationSource bit field value.
         *
         * @return Bit field value
         */
        public int getElevationSource() {
            return getValue(10, 11);
        }

        /**
         * Set ElevationSource bit field value
         * and update the related Bluetooth GATT Characteristic.
         *
         * @param value Value to write to ElevationSource bit field
         * @return      True, if the value has been set successfully
         */
        public boolean setElevationSource(int value) {
            if (!setValue(10, 11, value)) {
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
            return getValue(12, 12);
        }

        /**
         * Set HeadingSource bit field value
         * and update the related Bluetooth GATT Characteristic.
         *
         * @param value Value to write to HeadingSource bit field
         * @return      True, if the value has been set successfully
         */
        public boolean setHeadingSource(int value) {
            if (!setValue(12, 12, value)) {
                return false;
            }
            updateGattCharacteristic();
            return true;
        }
    }

    // Customized Start: Functions .................................................................

    //................................................................. Customized End: Functions //
}

