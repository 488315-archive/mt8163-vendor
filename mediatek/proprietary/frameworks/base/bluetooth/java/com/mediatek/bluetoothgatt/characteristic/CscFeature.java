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
 * Public API for the CSC Feature Bluetooth GATT Characteristic.
 *
 * <p>This class provides CSC Feature Bluetooth GATT Characteristic
 * value encode/decode functions. Allowing applications easy and quick to
 * read/write characteristic field value.
 *
 * <p>Name: CSC Feature
 * Type: org.bluetooth.characteristic.csc_feature
 * UUID: 2A5C
 * Last Modified: 2012-04-05
 * Revision: None
 */
public class CscFeature extends CharacteristicBase {
    /**
     * CSC Feature UUID.
     */
    public static final UUID GATT_UUID =
            UUID.fromString(FormatUtils.uuid16ToUuid128("2A5C"));

    // Customized Start: Constant Definition .......................................................
    /**
     * CSC feature list.
     */
    public static final int WHEEL_REVOLUTION_DATA = 1 << 0;
    public static final int CRANK_REVOLUTION_DATA = 1 << 1;
    public static final int MULTI_SENSOR_LOCATIONS = 1 << 2;
    //....................................................... Customized End: Constant Definition //

    /*
     * Field: CSC Feature
     * Requirement: Mandatory
     * Format: bit16
     */
    private BfCscFeature mCscFeature = new BfCscFeature();


    /**
     * Create a CscFeature characteristic object.
     */
    public CscFeature() {
        setCharacteristic(null);
        setCscFeature(new BfCscFeature());
    }

    /**
     * Create a CscFeature characteristic object and init value.
     *
     * @param value Initial value
     */
    public CscFeature(byte[] value) {
        setCharacteristic(null);
        setValue(value);
    }

    /**
     * Create a CscFeature characteristic object and init value.
     *
     * @param value Initial value
     * @param characteristic the gatt-characteristic this characteristic belongs to.
     */
    public CscFeature(byte[] value, BluetoothGattCharacteristic characteristic) {
        setCharacteristic(characteristic);
        setValue(value);
    }

    /**
     * Create a CscFeature characteristic object.
     *
     * @param cscFeature CSC Feature
     */
    public CscFeature(
            BfCscFeature cscFeature) {
        setCharacteristic(null);
        setCscFeature(cscFeature);
    }

    /**
     * Create a CscFeature characteristic object.
     *
     * @param cscFeature CSC Feature
     * @param characteristic the gatt-characteristic this characteristic belongs to.
     */
    public CscFeature(
            BfCscFeature cscFeature,
            BluetoothGattCharacteristic characteristic) {
        setCharacteristic(characteristic);
        setCscFeature(cscFeature);
    }

    // Customized Start: Constructors ..............................................................
    /**
     * Create a CscFeature characteristic object and init value.
     *
     * @param cscFeature CSC Feature
     */
    public CscFeature(int cscFeature) {
        if ((cscFeature & WHEEL_REVOLUTION_DATA) != 0) {
            mCscFeature.setWheelRevolutionDataSupported(1);
        }
        if ((cscFeature & CRANK_REVOLUTION_DATA) != 0) {
            mCscFeature.setCrankRevolutionDataSupported(1);
        }
        if ((cscFeature & MULTI_SENSOR_LOCATIONS) != 0) {
            mCscFeature.setMultipleSensorLocationsSupported(1);
        }
    }
    //.............................................................. Customized End: Constructors //

    /**
     * Get CscFeature characteristic byte length.
     *
     * @return Byte length of this characteristic
     */
    public int length() {
        return (isSupportCscFeature()
                        ? mCscFeature.length() : 0);
    }

    /**
     * Get CscFeature characteristic UUID.
     *
     * @return Characteristic UUID
     */
    @Override
    public UUID getUuid() {
        return GATT_UUID;
    }

    /**
     * Get CscFeature characteristic value.
     *
     * @return Byte array value of this characteristic.
     */
    @Override
    public byte[] getValue() {
        byte[] value = new byte[length()];
        int dstPos = 0;

        if (isSupportCscFeature()) {
            int fieldLen = mCscFeature.length();
            System.arraycopy(mCscFeature.getByteArray(), 0, value, dstPos, fieldLen);
            dstPos += fieldLen;
        }

        return value;
    }

    /**
     * Set CscFeature characteristic value.
     *
     * @param value Byte array value
     * @return      True, if the value has been set successfully
     */
    @Override
    public boolean setValue(byte[] value) {
        int srcPos = 0;

        if (isSupportCscFeature()) {
            int fieldLen = mCscFeature.length();

            if (!setValueRangeCheck(value.length, srcPos, fieldLen)) {
                return false;
            }

            byte[] buf = new byte[fieldLen];

            System.arraycopy(value, srcPos, buf, 0, buf.length);
            srcPos += fieldLen;

            mCscFeature.setByteArray(buf);
        }

        updateGattCharacteristic();
        return true;
    }

    /**
     * Get CscFeature field value with BfCscFeature format.
     *
     * @return CscFeature field value
     */
    public BfCscFeature getCscFeature() {
        return mCscFeature;
    }

    /**
     * Set CscFeature field value by BfCscFeature format
     * and update the related Bluetooth GATT Characteristic.
     *
     * @param value Value to write to CscFeature field
     * @return      True
     */
    public boolean setCscFeature(BfCscFeature value) {
        mCscFeature = value;
        updateGattCharacteristic();
        return true;
    }

    /**
     * Return true if CscFeature support CscFeature field.
     *
     * @return  True, if CscFeature support CscFeature field.
     */
    public boolean isSupportCscFeature() {
        return true;
    }

    /**
     * This class provides CscFeature BitField operations based on
     * specific definition.
     */
    public class BfCscFeature extends BitField {
        private static final int sLength = FormatUtils.BIT16_SIZE;

        /**
         * Get BfCscFeature BitField byte length.
         *
         * @return Byte length of this BitField
         */
        @Override
        public int length() {
            return sLength;
        }

        /**
         * Create a BfCscFeature BitField object.
         */
        public BfCscFeature() { super(sLength * 8); }

        /**
         * Create a BfCscFeature BitField object and init value.
         *
         * @param value Initial value
         */
        public BfCscFeature(byte[] value) {
            super(sLength * 8);
            setByteArray(value);
        }

        /**
         * Get WheelRevolutionDataSupported bit field value.
         *
         * @return Bit field value
         */
        public int getWheelRevolutionDataSupported() {
            return getValue(0, 0);
        }

        /**
         * Set WheelRevolutionDataSupported bit field value
         * and update the related Bluetooth GATT Characteristic.
         *
         * @param value Value to write to WheelRevolutionDataSupported bit field
         * @return      True, if the value has been set successfully
         */
        public boolean setWheelRevolutionDataSupported(int value) {
            if (!setValue(0, 0, value)) {
                return false;
            }
            updateGattCharacteristic();
            return true;
        }

        /**
         * Get CrankRevolutionDataSupported bit field value.
         *
         * @return Bit field value
         */
        public int getCrankRevolutionDataSupported() {
            return getValue(1, 1);
        }

        /**
         * Set CrankRevolutionDataSupported bit field value
         * and update the related Bluetooth GATT Characteristic.
         *
         * @param value Value to write to CrankRevolutionDataSupported bit field
         * @return      True, if the value has been set successfully
         */
        public boolean setCrankRevolutionDataSupported(int value) {
            if (!setValue(1, 1, value)) {
                return false;
            }
            updateGattCharacteristic();
            return true;
        }

        /**
         * Get MultipleSensorLocationsSupported bit field value.
         *
         * @return Bit field value
         */
        public int getMultipleSensorLocationsSupported() {
            return getValue(2, 2);
        }

        /**
         * Set MultipleSensorLocationsSupported bit field value
         * and update the related Bluetooth GATT Characteristic.
         *
         * @param value Value to write to MultipleSensorLocationsSupported bit field
         * @return      True, if the value has been set successfully
         */
        public boolean setMultipleSensorLocationsSupported(int value) {
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

