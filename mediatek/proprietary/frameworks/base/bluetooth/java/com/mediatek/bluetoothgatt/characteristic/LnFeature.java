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
 * Public API for the LN Feature Bluetooth GATT Characteristic.
 *
 * <p>This class provides LN Feature Bluetooth GATT Characteristic
 * value encode/decode functions. Allowing applications easy and quick to
 * read/write characteristic field value.
 *
 * <p>Name: LN Feature
 * Type: org.bluetooth.characteristic.ln_feature
 * UUID: 2A6A
 * Last Modified: 2013-05-13
 * Revision: None
 */
public class LnFeature extends CharacteristicBase {
    /**
     * LN Feature UUID.
     */
    public static final UUID GATT_UUID =
            UUID.fromString(FormatUtils.uuid16ToUuid128("2A6A"));

    // Customized Start: Constant Definition .......................................................

    //....................................................... Customized End: Constant Definition //

    /*
     * Field: LN Feature
     * Requirement: Mandatory
     * Format: bit32
     */
    private BfLnFeature mLnFeature = new BfLnFeature();


    /**
     * Create a LnFeature characteristic object.
     */
    public LnFeature() {
        setCharacteristic(null);
        setLnFeature(new BfLnFeature());
    }

    /**
     * Create a LnFeature characteristic object and init value.
     *
     * @param value Initial value
     */
    public LnFeature(byte[] value) {
        setCharacteristic(null);
        setValue(value);
    }

    /**
     * Create a LnFeature characteristic object and init value.
     *
     * @param value Initial value
     * @param characteristic the gatt-characteristic this characteristic belongs to.
     */
    public LnFeature(byte[] value, BluetoothGattCharacteristic characteristic) {
        setCharacteristic(characteristic);
        setValue(value);
    }

    /**
     * Create a LnFeature characteristic object.
     *
     * @param lnFeature LN Feature
     */
    public LnFeature(
            BfLnFeature lnFeature) {
        setCharacteristic(null);
        setLnFeature(lnFeature);
    }

    /**
     * Create a LnFeature characteristic object.
     *
     * @param lnFeature LN Feature
     * @param characteristic the gatt-characteristic this characteristic belongs to.
     */
    public LnFeature(
            BfLnFeature lnFeature,
            BluetoothGattCharacteristic characteristic) {
        setCharacteristic(characteristic);
        setLnFeature(lnFeature);
    }

    // Customized Start: Constructors ..............................................................

    //.............................................................. Customized End: Constructors //

    /**
     * Get LnFeature characteristic byte length.
     *
     * @return Byte length of this characteristic
     */
    public int length() {
        return (isSupportLnFeature()
                        ? mLnFeature.length() : 0);
    }

    /**
     * Get LnFeature characteristic UUID.
     *
     * @return Characteristic UUID
     */
    @Override
    public UUID getUuid() {
        return GATT_UUID;
    }

    /**
     * Get LnFeature characteristic value.
     *
     * @return Byte array value of this characteristic.
     */
    @Override
    public byte[] getValue() {
        byte[] value = new byte[length()];
        int dstPos = 0;

        if (isSupportLnFeature()) {
            int fieldLen = mLnFeature.length();
            System.arraycopy(mLnFeature.getByteArray(), 0, value, dstPos, fieldLen);
            dstPos += fieldLen;
        }

        return value;
    }

    /**
     * Set LnFeature characteristic value.
     *
     * @param value Byte array value
     * @return      True, if the value has been set successfully
     */
    @Override
    public boolean setValue(byte[] value) {
        int srcPos = 0;

        if (isSupportLnFeature()) {
            int fieldLen = mLnFeature.length();

            if (!setValueRangeCheck(value.length, srcPos, fieldLen)) {
                return false;
            }

            byte[] buf = new byte[fieldLen];

            System.arraycopy(value, srcPos, buf, 0, buf.length);
            srcPos += fieldLen;

            mLnFeature.setByteArray(buf);
        }

        updateGattCharacteristic();
        return true;
    }

    /**
     * Get LnFeature field value with BfLnFeature format.
     *
     * @return LnFeature field value
     */
    public BfLnFeature getLnFeature() {
        return mLnFeature;
    }

    /**
     * Set LnFeature field value by BfLnFeature format
     * and update the related Bluetooth GATT Characteristic.
     *
     * @param value Value to write to LnFeature field
     * @return      True
     */
    public boolean setLnFeature(BfLnFeature value) {
        mLnFeature = value;
        updateGattCharacteristic();
        return true;
    }

    /**
     * Return true if LnFeature support LnFeature field.
     *
     * @return  True, if LnFeature support LnFeature field.
     */
    public boolean isSupportLnFeature() {
        return true;
    }

    /**
     * This class provides LnFeature BitField operations based on
     * specific definition.
     */
    public class BfLnFeature extends BitField {
        private static final int sLength = FormatUtils.BIT32_SIZE;

        /**
         * Get BfLnFeature BitField byte length.
         *
         * @return Byte length of this BitField
         */
        @Override
        public int length() {
            return sLength;
        }

        /**
         * Create a BfLnFeature BitField object.
         */
        public BfLnFeature() { super(sLength * 8); }

        /**
         * Create a BfLnFeature BitField object and init value.
         *
         * @param value Initial value
         */
        public BfLnFeature(byte[] value) {
            super(sLength * 8);
            setByteArray(value);
        }

        /**
         * Get InstantaneousSpeedSupported bit field value.
         *
         * @return Bit field value
         */
        public int getInstantaneousSpeedSupported() {
            return getValue(0, 0);
        }

        /**
         * Set InstantaneousSpeedSupported bit field value
         * and update the related Bluetooth GATT Characteristic.
         *
         * @param value Value to write to InstantaneousSpeedSupported bit field
         * @return      True, if the value has been set successfully
         */
        public boolean setInstantaneousSpeedSupported(int value) {
            if (!setValue(0, 0, value)) {
                return false;
            }
            updateGattCharacteristic();
            return true;
        }

        /**
         * Get TotalDistanceSupported bit field value.
         *
         * @return Bit field value
         */
        public int getTotalDistanceSupported() {
            return getValue(1, 1);
        }

        /**
         * Set TotalDistanceSupported bit field value
         * and update the related Bluetooth GATT Characteristic.
         *
         * @param value Value to write to TotalDistanceSupported bit field
         * @return      True, if the value has been set successfully
         */
        public boolean setTotalDistanceSupported(int value) {
            if (!setValue(1, 1, value)) {
                return false;
            }
            updateGattCharacteristic();
            return true;
        }

        /**
         * Get LocationSupported bit field value.
         *
         * @return Bit field value
         */
        public int getLocationSupported() {
            return getValue(2, 2);
        }

        /**
         * Set LocationSupported bit field value
         * and update the related Bluetooth GATT Characteristic.
         *
         * @param value Value to write to LocationSupported bit field
         * @return      True, if the value has been set successfully
         */
        public boolean setLocationSupported(int value) {
            if (!setValue(2, 2, value)) {
                return false;
            }
            updateGattCharacteristic();
            return true;
        }

        /**
         * Get ElevationSupported bit field value.
         *
         * @return Bit field value
         */
        public int getElevationSupported() {
            return getValue(3, 3);
        }

        /**
         * Set ElevationSupported bit field value
         * and update the related Bluetooth GATT Characteristic.
         *
         * @param value Value to write to ElevationSupported bit field
         * @return      True, if the value has been set successfully
         */
        public boolean setElevationSupported(int value) {
            if (!setValue(3, 3, value)) {
                return false;
            }
            updateGattCharacteristic();
            return true;
        }

        /**
         * Get HeadingSupported bit field value.
         *
         * @return Bit field value
         */
        public int getHeadingSupported() {
            return getValue(4, 4);
        }

        /**
         * Set HeadingSupported bit field value
         * and update the related Bluetooth GATT Characteristic.
         *
         * @param value Value to write to HeadingSupported bit field
         * @return      True, if the value has been set successfully
         */
        public boolean setHeadingSupported(int value) {
            if (!setValue(4, 4, value)) {
                return false;
            }
            updateGattCharacteristic();
            return true;
        }

        /**
         * Get RollingTimeSupported bit field value.
         *
         * @return Bit field value
         */
        public int getRollingTimeSupported() {
            return getValue(5, 5);
        }

        /**
         * Set RollingTimeSupported bit field value
         * and update the related Bluetooth GATT Characteristic.
         *
         * @param value Value to write to RollingTimeSupported bit field
         * @return      True, if the value has been set successfully
         */
        public boolean setRollingTimeSupported(int value) {
            if (!setValue(5, 5, value)) {
                return false;
            }
            updateGattCharacteristic();
            return true;
        }

        /**
         * Get UtcTimeSupported bit field value.
         *
         * @return Bit field value
         */
        public int getUtcTimeSupported() {
            return getValue(6, 6);
        }

        /**
         * Set UtcTimeSupported bit field value
         * and update the related Bluetooth GATT Characteristic.
         *
         * @param value Value to write to UtcTimeSupported bit field
         * @return      True, if the value has been set successfully
         */
        public boolean setUtcTimeSupported(int value) {
            if (!setValue(6, 6, value)) {
                return false;
            }
            updateGattCharacteristic();
            return true;
        }

        /**
         * Get RemainingDistanceSupported bit field value.
         *
         * @return Bit field value
         */
        public int getRemainingDistanceSupported() {
            return getValue(7, 7);
        }

        /**
         * Set RemainingDistanceSupported bit field value
         * and update the related Bluetooth GATT Characteristic.
         *
         * @param value Value to write to RemainingDistanceSupported bit field
         * @return      True, if the value has been set successfully
         */
        public boolean setRemainingDistanceSupported(int value) {
            if (!setValue(7, 7, value)) {
                return false;
            }
            updateGattCharacteristic();
            return true;
        }

        /**
         * Get RemainingVerticalDistanceSupported bit field value.
         *
         * @return Bit field value
         */
        public int getRemainingVerticalDistanceSupported() {
            return getValue(8, 8);
        }

        /**
         * Set RemainingVerticalDistanceSupported bit field value
         * and update the related Bluetooth GATT Characteristic.
         *
         * @param value Value to write to RemainingVerticalDistanceSupported bit field
         * @return      True, if the value has been set successfully
         */
        public boolean setRemainingVerticalDistanceSupported(int value) {
            if (!setValue(8, 8, value)) {
                return false;
            }
            updateGattCharacteristic();
            return true;
        }

        /**
         * Get EstimatedTimeOfArrivalSupported bit field value.
         *
         * @return Bit field value
         */
        public int getEstimatedTimeOfArrivalSupported() {
            return getValue(9, 9);
        }

        /**
         * Set EstimatedTimeOfArrivalSupported bit field value
         * and update the related Bluetooth GATT Characteristic.
         *
         * @param value Value to write to EstimatedTimeOfArrivalSupported bit field
         * @return      True, if the value has been set successfully
         */
        public boolean setEstimatedTimeOfArrivalSupported(int value) {
            if (!setValue(9, 9, value)) {
                return false;
            }
            updateGattCharacteristic();
            return true;
        }

        /**
         * Get NumberOfBeaconsInSolutionSupported bit field value.
         *
         * @return Bit field value
         */
        public int getNumberOfBeaconsInSolutionSupported() {
            return getValue(10, 10);
        }

        /**
         * Set NumberOfBeaconsInSolutionSupported bit field value
         * and update the related Bluetooth GATT Characteristic.
         *
         * @param value Value to write to NumberOfBeaconsInSolutionSupported bit field
         * @return      True, if the value has been set successfully
         */
        public boolean setNumberOfBeaconsInSolutionSupported(int value) {
            if (!setValue(10, 10, value)) {
                return false;
            }
            updateGattCharacteristic();
            return true;
        }

        /**
         * Get NumberOfBeaconsInViewSupported bit field value.
         *
         * @return Bit field value
         */
        public int getNumberOfBeaconsInViewSupported() {
            return getValue(11, 11);
        }

        /**
         * Set NumberOfBeaconsInViewSupported bit field value
         * and update the related Bluetooth GATT Characteristic.
         *
         * @param value Value to write to NumberOfBeaconsInViewSupported bit field
         * @return      True, if the value has been set successfully
         */
        public boolean setNumberOfBeaconsInViewSupported(int value) {
            if (!setValue(11, 11, value)) {
                return false;
            }
            updateGattCharacteristic();
            return true;
        }

        /**
         * Get TimeToFirstFixSupported bit field value.
         *
         * @return Bit field value
         */
        public int getTimeToFirstFixSupported() {
            return getValue(12, 12);
        }

        /**
         * Set TimeToFirstFixSupported bit field value
         * and update the related Bluetooth GATT Characteristic.
         *
         * @param value Value to write to TimeToFirstFixSupported bit field
         * @return      True, if the value has been set successfully
         */
        public boolean setTimeToFirstFixSupported(int value) {
            if (!setValue(12, 12, value)) {
                return false;
            }
            updateGattCharacteristic();
            return true;
        }

        /**
         * Get EstimatedHorizontalPositionErrorSupported bit field value.
         *
         * @return Bit field value
         */
        public int getEstimatedHorizontalPositionErrorSupported() {
            return getValue(13, 13);
        }

        /**
         * Set EstimatedHorizontalPositionErrorSupported bit field value
         * and update the related Bluetooth GATT Characteristic.
         *
         * @param value Value to write to EstimatedHorizontalPositionErrorSupported bit field
         * @return      True, if the value has been set successfully
         */
        public boolean setEstimatedHorizontalPositionErrorSupported(int value) {
            if (!setValue(13, 13, value)) {
                return false;
            }
            updateGattCharacteristic();
            return true;
        }

        /**
         * Get EstimatedVerticalPositionErrorSupported bit field value.
         *
         * @return Bit field value
         */
        public int getEstimatedVerticalPositionErrorSupported() {
            return getValue(14, 14);
        }

        /**
         * Set EstimatedVerticalPositionErrorSupported bit field value
         * and update the related Bluetooth GATT Characteristic.
         *
         * @param value Value to write to EstimatedVerticalPositionErrorSupported bit field
         * @return      True, if the value has been set successfully
         */
        public boolean setEstimatedVerticalPositionErrorSupported(int value) {
            if (!setValue(14, 14, value)) {
                return false;
            }
            updateGattCharacteristic();
            return true;
        }

        /**
         * Get HorizontalDilutionOfPrecisionSupported bit field value.
         *
         * @return Bit field value
         */
        public int getHorizontalDilutionOfPrecisionSupported() {
            return getValue(15, 15);
        }

        /**
         * Set HorizontalDilutionOfPrecisionSupported bit field value
         * and update the related Bluetooth GATT Characteristic.
         *
         * @param value Value to write to HorizontalDilutionOfPrecisionSupported bit field
         * @return      True, if the value has been set successfully
         */
        public boolean setHorizontalDilutionOfPrecisionSupported(int value) {
            if (!setValue(15, 15, value)) {
                return false;
            }
            updateGattCharacteristic();
            return true;
        }

        /**
         * Get VerticalDilutionOfPrecisionSupported bit field value.
         *
         * @return Bit field value
         */
        public int getVerticalDilutionOfPrecisionSupported() {
            return getValue(16, 16);
        }

        /**
         * Set VerticalDilutionOfPrecisionSupported bit field value
         * and update the related Bluetooth GATT Characteristic.
         *
         * @param value Value to write to VerticalDilutionOfPrecisionSupported bit field
         * @return      True, if the value has been set successfully
         */
        public boolean setVerticalDilutionOfPrecisionSupported(int value) {
            if (!setValue(16, 16, value)) {
                return false;
            }
            updateGattCharacteristic();
            return true;
        }

        /**
         * Get CharacteristicContentMaskingSupported bit field value.
         *
         * @return Bit field value
         */
        public int getCharacteristicContentMaskingSupported() {
            return getValue(17, 17);
        }

        /**
         * Set CharacteristicContentMaskingSupported bit field value
         * and update the related Bluetooth GATT Characteristic.
         *
         * @param value Value to write to CharacteristicContentMaskingSupported bit field
         * @return      True, if the value has been set successfully
         */
        public boolean setCharacteristicContentMaskingSupported(int value) {
            if (!setValue(17, 17, value)) {
                return false;
            }
            updateGattCharacteristic();
            return true;
        }

        /**
         * Get FixRateSettingSupported bit field value.
         *
         * @return Bit field value
         */
        public int getFixRateSettingSupported() {
            return getValue(18, 18);
        }

        /**
         * Set FixRateSettingSupported bit field value
         * and update the related Bluetooth GATT Characteristic.
         *
         * @param value Value to write to FixRateSettingSupported bit field
         * @return      True, if the value has been set successfully
         */
        public boolean setFixRateSettingSupported(int value) {
            if (!setValue(18, 18, value)) {
                return false;
            }
            updateGattCharacteristic();
            return true;
        }

        /**
         * Get ElevationSettingSupported bit field value.
         *
         * @return Bit field value
         */
        public int getElevationSettingSupported() {
            return getValue(19, 19);
        }

        /**
         * Set ElevationSettingSupported bit field value
         * and update the related Bluetooth GATT Characteristic.
         *
         * @param value Value to write to ElevationSettingSupported bit field
         * @return      True, if the value has been set successfully
         */
        public boolean setElevationSettingSupported(int value) {
            if (!setValue(19, 19, value)) {
                return false;
            }
            updateGattCharacteristic();
            return true;
        }

        /**
         * Get PositionStatusSupported bit field value.
         *
         * @return Bit field value
         */
        public int getPositionStatusSupported() {
            return getValue(20, 20);
        }

        /**
         * Set PositionStatusSupported bit field value
         * and update the related Bluetooth GATT Characteristic.
         *
         * @param value Value to write to PositionStatusSupported bit field
         * @return      True, if the value has been set successfully
         */
        public boolean setPositionStatusSupported(int value) {
            if (!setValue(20, 20, value)) {
                return false;
            }
            updateGattCharacteristic();
            return true;
        }
    }

    // Customized Start: Functions .................................................................

    //................................................................. Customized End: Functions //
}

