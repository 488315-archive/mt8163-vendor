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
 * Public API for the RSC Feature Bluetooth GATT Characteristic.
 *
 * <p>This class provides RSC Feature Bluetooth GATT Characteristic
 * value encode/decode functions. Allowing applications easy and quick to
 * read/write characteristic field value.
 *
 * <p>Name: RSC Feature
 * Type: org.bluetooth.characteristic.rsc_feature
 * UUID: 2A54
 * Last Modified: 2012-03-22
 * Revision: None
 */
public class RscFeature extends CharacteristicBase {
    /**
     * RSC Feature UUID.
     */
    public static final UUID GATT_UUID =
            UUID.fromString(FormatUtils.uuid16ToUuid128("2A54"));

    // Customized Start: Constant Definition .......................................................

    //....................................................... Customized End: Constant Definition //

    /*
     * Field: RSC Feature
     * Requirement: Mandatory
     * Format: bit16
     */
    private BfRscFeature mRscFeature = new BfRscFeature();


    /**
     * Create a RscFeature characteristic object.
     */
    public RscFeature() {
        setCharacteristic(null);
        setRscFeature(new BfRscFeature());
    }

    /**
     * Create a RscFeature characteristic object and init value.
     *
     * @param value Initial value
     */
    public RscFeature(byte[] value) {
        setCharacteristic(null);
        setValue(value);
    }

    /**
     * Create a RscFeature characteristic object and init value.
     *
     * @param value Initial value
     * @param characteristic the gatt-characteristic this characteristic belongs to.
     */
    public RscFeature(byte[] value, BluetoothGattCharacteristic characteristic) {
        setCharacteristic(characteristic);
        setValue(value);
    }

    /**
     * Create a RscFeature characteristic object.
     *
     * @param rscFeature RSC Feature
     */
    public RscFeature(
            BfRscFeature rscFeature) {
        setCharacteristic(null);
        setRscFeature(rscFeature);
    }

    /**
     * Create a RscFeature characteristic object.
     *
     * @param rscFeature RSC Feature
     * @param characteristic the gatt-characteristic this characteristic belongs to.
     */
    public RscFeature(
            BfRscFeature rscFeature,
            BluetoothGattCharacteristic characteristic) {
        setCharacteristic(characteristic);
        setRscFeature(rscFeature);
    }

    // Customized Start: Constructors ..............................................................

    //.............................................................. Customized End: Constructors //

    /**
     * Get RscFeature characteristic byte length.
     *
     * @return Byte length of this characteristic
     */
    public int length() {
        return (isSupportRscFeature()
                        ? mRscFeature.length() : 0);
    }

    /**
     * Get RscFeature characteristic UUID.
     *
     * @return Characteristic UUID
     */
    @Override
    public UUID getUuid() {
        return GATT_UUID;
    }

    /**
     * Get RscFeature characteristic value.
     *
     * @return Byte array value of this characteristic.
     */
    @Override
    public byte[] getValue() {
        byte[] value = new byte[length()];
        int dstPos = 0;

        if (isSupportRscFeature()) {
            int fieldLen = mRscFeature.length();
            System.arraycopy(mRscFeature.getByteArray(), 0, value, dstPos, fieldLen);
            dstPos += fieldLen;
        }

        return value;
    }

    /**
     * Set RscFeature characteristic value.
     *
     * @param value Byte array value
     * @return      True, if the value has been set successfully
     */
    @Override
    public boolean setValue(byte[] value) {
        int srcPos = 0;

        if (isSupportRscFeature()) {
            int fieldLen = mRscFeature.length();

            if (!setValueRangeCheck(value.length, srcPos, fieldLen)) {
                return false;
            }

            byte[] buf = new byte[fieldLen];

            System.arraycopy(value, srcPos, buf, 0, buf.length);
            srcPos += fieldLen;

            mRscFeature.setByteArray(buf);
        }

        updateGattCharacteristic();
        return true;
    }

    /**
     * Get RscFeature field value with BfRscFeature format.
     *
     * @return RscFeature field value
     */
    public BfRscFeature getRscFeature() {
        return mRscFeature;
    }

    /**
     * Set RscFeature field value by BfRscFeature format
     * and update the related Bluetooth GATT Characteristic.
     *
     * @param value Value to write to RscFeature field
     * @return      True
     */
    public boolean setRscFeature(BfRscFeature value) {
        mRscFeature = value;
        updateGattCharacteristic();
        return true;
    }

    /**
     * Return true if RscFeature support RscFeature field.
     *
     * @return  True, if RscFeature support RscFeature field.
     */
    public boolean isSupportRscFeature() {
        return true;
    }

    /**
     * This class provides RscFeature BitField operations based on
     * specific definition.
     */
    public class BfRscFeature extends BitField {
        private static final int sLength = FormatUtils.BIT16_SIZE;

        /**
         * Get BfRscFeature BitField byte length.
         *
         * @return Byte length of this BitField
         */
        @Override
        public int length() {
            return sLength;
        }

        /**
         * Create a BfRscFeature BitField object.
         */
        public BfRscFeature() { super(sLength * 8); }

        /**
         * Create a BfRscFeature BitField object and init value.
         *
         * @param value Initial value
         */
        public BfRscFeature(byte[] value) {
            super(sLength * 8);
            setByteArray(value);
        }

        /**
         * Get InstantaneousStrideLengthMeasurementSupported bit field value.
         *
         * @return Bit field value
         */
        public int getInstantaneousStrideLengthMeasurementSupported() {
            return getValue(0, 0);
        }

        /**
         * Set InstantaneousStrideLengthMeasurementSupported bit field value
         * and update the related Bluetooth GATT Characteristic.
         *
         * @param value Value to write to InstantaneousStrideLengthMeasurementSupported bit field
         * @return      True, if the value has been set successfully
         */
        public boolean setInstantaneousStrideLengthMeasurementSupported(int value) {
            if (!setValue(0, 0, value)) {
                return false;
            }
            updateGattCharacteristic();
            return true;
        }

        /**
         * Get TotalDistanceMeasurementSupported bit field value.
         *
         * @return Bit field value
         */
        public int getTotalDistanceMeasurementSupported() {
            return getValue(1, 1);
        }

        /**
         * Set TotalDistanceMeasurementSupported bit field value
         * and update the related Bluetooth GATT Characteristic.
         *
         * @param value Value to write to TotalDistanceMeasurementSupported bit field
         * @return      True, if the value has been set successfully
         */
        public boolean setTotalDistanceMeasurementSupported(int value) {
            if (!setValue(1, 1, value)) {
                return false;
            }
            updateGattCharacteristic();
            return true;
        }

        /**
         * Get WalkingOrRunningStatusSupported bit field value.
         *
         * @return Bit field value
         */
        public int getWalkingOrRunningStatusSupported() {
            return getValue(2, 2);
        }

        /**
         * Set WalkingOrRunningStatusSupported bit field value
         * and update the related Bluetooth GATT Characteristic.
         *
         * @param value Value to write to WalkingOrRunningStatusSupported bit field
         * @return      True, if the value has been set successfully
         */
        public boolean setWalkingOrRunningStatusSupported(int value) {
            if (!setValue(2, 2, value)) {
                return false;
            }
            updateGattCharacteristic();
            return true;
        }

        /**
         * Get CalibrationProcedureSupported bit field value.
         *
         * @return Bit field value
         */
        public int getCalibrationProcedureSupported() {
            return getValue(3, 3);
        }

        /**
         * Set CalibrationProcedureSupported bit field value
         * and update the related Bluetooth GATT Characteristic.
         *
         * @param value Value to write to CalibrationProcedureSupported bit field
         * @return      True, if the value has been set successfully
         */
        public boolean setCalibrationProcedureSupported(int value) {
            if (!setValue(3, 3, value)) {
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
            return getValue(4, 4);
        }

        /**
         * Set MultipleSensorLocationsSupported bit field value
         * and update the related Bluetooth GATT Characteristic.
         *
         * @param value Value to write to MultipleSensorLocationsSupported bit field
         * @return      True, if the value has been set successfully
         */
        public boolean setMultipleSensorLocationsSupported(int value) {
            if (!setValue(4, 4, value)) {
                return false;
            }
            updateGattCharacteristic();
            return true;
        }
    }

    // Customized Start: Functions .................................................................

    //................................................................. Customized End: Functions //
}

