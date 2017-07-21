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
 * Public API for the Cycling Power Feature Bluetooth GATT Characteristic.
 *
 * <p>This class provides Cycling Power Feature Bluetooth GATT Characteristic
 * value encode/decode functions. Allowing applications easy and quick to
 * read/write characteristic field value.
 *
 * <p>Name: Cycling Power Feature
 * Type: org.bluetooth.characteristic.cycling_power_feature
 * UUID: 2A65
 * Last Modified: 2013-05-13
 * Revision: None
 */
public class CyclingPowerFeature extends CharacteristicBase {
    /**
     * Cycling Power Feature UUID.
     */
    public static final UUID GATT_UUID =
            UUID.fromString(FormatUtils.uuid16ToUuid128("2A65"));

    // Customized Start: Constant Definition .......................................................
    /**
     * Cycling Power Feature list.
     */
    public static final int PEDAL_POWER_BALANCE = 1 << 0;
    public static final int ACCUMULATED_TORQUE = 1 << 1;
    public static final int WHEEL_REVOLUTION_DATA = 1 << 2;
    public static final int CRANK_REVOLUTION_DATA = 1 << 3;
    public static final int EXTREME_MAGNITUDES = 1 << 4;
    public static final int EXTREME_ANGLES = 1 << 5;
    public static final int TOP_AND_BOTTOM_DEAD_SPOT_ANGLES = 1 << 6;
    public static final int ACCUMULATED_ENERGY = 1 << 7;
    public static final int OFFSET_COMPENSATION_INDICATOR = 1 << 8;
    public static final int OFFSET_COMPENSATION = 1 << 9;
    public static final int CHARACTERISTIC_CONTENT_MASKING = 1 << 10;
    public static final int MULTIPLE_SENSOR_LOCATIONS = 1 << 11;
    public static final int CRANK_LENGTH_ADJUSTMENT = 1 << 12;
    public static final int CHAIN_LENGTH_ADJUSTMENT = 1 << 13;
    public static final int CHAIN_WEIGHT_ADJUSTMENT = 1 << 14;
    public static final int SPAN_LENGTH_ADJUSTMENT = 1 << 15;
    public static final int SENSOR_MEASUREMENT_FORCE_BASED = 0 << 16;
    public static final int SENSOR_MEASUREMENT_TORQUE_BASED = 1 << 16;
    public static final int INSTANTANEOUS_MEASUREMENT_DIRECTION = 1 << 17;
    public static final int FACTORY_CALIBRATION_DATE = 1 << 18;
    //....................................................... Customized End: Constant Definition //

    /*
     * Field: Cycling Power Feature
     * Requirement: Mandatory
     * Format: bit32
     */
    private BfCyclingPowerFeature mCyclingPowerFeature = new BfCyclingPowerFeature();


    /**
     * Create a CyclingPowerFeature characteristic object.
     */
    public CyclingPowerFeature() {
        setCharacteristic(null);
        setCyclingPowerFeature(new BfCyclingPowerFeature());
    }

    /**
     * Create a CyclingPowerFeature characteristic object and init value.
     *
     * @param value Initial value
     */
    public CyclingPowerFeature(byte[] value) {
        setCharacteristic(null);
        setValue(value);
    }

    /**
     * Create a CyclingPowerFeature characteristic object and init value.
     *
     * @param value Initial value
     * @param characteristic the gatt-characteristic this characteristic belongs to.
     */
    public CyclingPowerFeature(byte[] value, BluetoothGattCharacteristic characteristic) {
        setCharacteristic(characteristic);
        setValue(value);
    }

    /**
     * Create a CyclingPowerFeature characteristic object.
     *
     * @param cyclingPowerFeature Cycling Power Feature
     */
    public CyclingPowerFeature(
            BfCyclingPowerFeature cyclingPowerFeature) {
        setCharacteristic(null);
        setCyclingPowerFeature(cyclingPowerFeature);
    }

    /**
     * Create a CyclingPowerFeature characteristic object.
     *
     * @param cyclingPowerFeature Cycling Power Feature
     * @param characteristic the gatt-characteristic this characteristic belongs to.
     */
    public CyclingPowerFeature(
            BfCyclingPowerFeature cyclingPowerFeature,
            BluetoothGattCharacteristic characteristic) {
        setCharacteristic(characteristic);
        setCyclingPowerFeature(cyclingPowerFeature);
    }

    // Customized Start: Constructors ..............................................................

    /**
     * Create a CyclingPowerFeature characteristic object and init value.
     *
     * @param cyclingPowerFeature Cycling Power Feature
     */
    public CyclingPowerFeature(int cyclingPowerFeature) {
        if ((cyclingPowerFeature & PEDAL_POWER_BALANCE) != 0) {
            mCyclingPowerFeature.setPedalPowerBalanceSupported(1);
        }
        if ((cyclingPowerFeature & ACCUMULATED_TORQUE) != 0) {
            mCyclingPowerFeature.setAccumulatedTorqueSupported(1);
        }
        if ((cyclingPowerFeature & WHEEL_REVOLUTION_DATA) != 0) {
            mCyclingPowerFeature.setWheelRevolutionDataSupported(1);
        }
        if ((cyclingPowerFeature & CRANK_REVOLUTION_DATA) != 0) {
            mCyclingPowerFeature.setCrankRevolutionDataSupported(1);
        }
        if ((cyclingPowerFeature & EXTREME_MAGNITUDES) != 0) {
            mCyclingPowerFeature.setExtremeMagnitudesSupported(1);
        }
        if ((cyclingPowerFeature & TOP_AND_BOTTOM_DEAD_SPOT_ANGLES) != 0) {
            mCyclingPowerFeature.setTopAndBottomDeadSpotAnglesSupported(1);
        }
        if ((cyclingPowerFeature & ACCUMULATED_ENERGY) != 0) {
            mCyclingPowerFeature.setAccumulatedEnergySupported(1);
        }
        if ((cyclingPowerFeature & OFFSET_COMPENSATION_INDICATOR) != 0) {
            mCyclingPowerFeature.setOffsetCompensationIndicatorSupported(1);
        }
        if ((cyclingPowerFeature & OFFSET_COMPENSATION) != 0) {
            mCyclingPowerFeature.setOffsetCompensationSupported(1);
        }
        if ((cyclingPowerFeature & CHARACTERISTIC_CONTENT_MASKING) != 0) {
            mCyclingPowerFeature.setCharacteristicContentMaskingSupported(1);
        }
        if ((cyclingPowerFeature & MULTIPLE_SENSOR_LOCATIONS) != 0) {
            mCyclingPowerFeature.setMultipleSensorLocationsSupported(1);
        }
        if ((cyclingPowerFeature & CRANK_LENGTH_ADJUSTMENT) != 0) {
            mCyclingPowerFeature.setCrankLengthAdjustmentSupported(1);
        }
        if ((cyclingPowerFeature & CHAIN_LENGTH_ADJUSTMENT) != 0) {
            mCyclingPowerFeature.setChainLengthAdjustmentSupported(1);
        }
        if ((cyclingPowerFeature & CHAIN_WEIGHT_ADJUSTMENT) != 0) {
            mCyclingPowerFeature.setChainWeightAdjustmentSupported(1);
        }
        if ((cyclingPowerFeature & SPAN_LENGTH_ADJUSTMENT) != 0) {
            mCyclingPowerFeature.setSpanLengthAdjustmentSupported(1);
        }
        if ((cyclingPowerFeature & SENSOR_MEASUREMENT_TORQUE_BASED) != 0) {
            mCyclingPowerFeature.setSensorMeasurementContext(1);
        }
        if ((cyclingPowerFeature & INSTANTANEOUS_MEASUREMENT_DIRECTION) != 0) {
            mCyclingPowerFeature.setInstantaneousMeasurementDirectionSupported(1);
        }
        if ((cyclingPowerFeature & FACTORY_CALIBRATION_DATE) != 0) {
            mCyclingPowerFeature.setFactoryCalibrationDateSupported(1);
        }
    }
    //.............................................................. Customized End: Constructors //

    /**
     * Get CyclingPowerFeature characteristic byte length.
     *
     * @return Byte length of this characteristic
     */
    public int length() {
        return (isSupportCyclingPowerFeature()
                        ? mCyclingPowerFeature.length() : 0);
    }

    /**
     * Get CyclingPowerFeature characteristic UUID.
     *
     * @return Characteristic UUID
     */
    @Override
    public UUID getUuid() {
        return GATT_UUID;
    }

    /**
     * Get CyclingPowerFeature characteristic value.
     *
     * @return Byte array value of this characteristic.
     */
    @Override
    public byte[] getValue() {
        byte[] value = new byte[length()];
        int dstPos = 0;

        if (isSupportCyclingPowerFeature()) {
            int fieldLen = mCyclingPowerFeature.length();
            System.arraycopy(mCyclingPowerFeature.getByteArray(), 0, value, dstPos, fieldLen);
            dstPos += fieldLen;
        }

        return value;
    }

    /**
     * Set CyclingPowerFeature characteristic value.
     *
     * @param value Byte array value
     * @return      True, if the value has been set successfully
     */
    @Override
    public boolean setValue(byte[] value) {
        int srcPos = 0;

        if (isSupportCyclingPowerFeature()) {
            int fieldLen = mCyclingPowerFeature.length();

            if (!setValueRangeCheck(value.length, srcPos, fieldLen)) {
                return false;
            }

            byte[] buf = new byte[fieldLen];

            System.arraycopy(value, srcPos, buf, 0, buf.length);
            srcPos += fieldLen;

            mCyclingPowerFeature.setByteArray(buf);
        }

        updateGattCharacteristic();
        return true;
    }

    /**
     * Get CyclingPowerFeature field value with BfCyclingPowerFeature format.
     *
     * @return CyclingPowerFeature field value
     */
    public BfCyclingPowerFeature getCyclingPowerFeature() {
        return mCyclingPowerFeature;
    }

    /**
     * Set CyclingPowerFeature field value by BfCyclingPowerFeature format
     * and update the related Bluetooth GATT Characteristic.
     *
     * @param value Value to write to CyclingPowerFeature field
     * @return      True
     */
    public boolean setCyclingPowerFeature(BfCyclingPowerFeature value) {
        mCyclingPowerFeature = value;
        updateGattCharacteristic();
        return true;
    }

    /**
     * Return true if CyclingPowerFeature support CyclingPowerFeature field.
     *
     * @return  True, if CyclingPowerFeature support CyclingPowerFeature field.
     */
    public boolean isSupportCyclingPowerFeature() {
        return true;
    }

    /**
     * This class provides CyclingPowerFeature BitField operations based on
     * specific definition.
     */
    public class BfCyclingPowerFeature extends BitField {
        private static final int sLength = FormatUtils.BIT32_SIZE;

        /**
         * Get BfCyclingPowerFeature BitField byte length.
         *
         * @return Byte length of this BitField
         */
        @Override
        public int length() {
            return sLength;
        }

        /**
         * Create a BfCyclingPowerFeature BitField object.
         */
        public BfCyclingPowerFeature() { super(sLength * 8); }

        /**
         * Create a BfCyclingPowerFeature BitField object and init value.
         *
         * @param value Initial value
         */
        public BfCyclingPowerFeature(byte[] value) {
            super(sLength * 8);
            setByteArray(value);
        }

        /**
         * Get PedalPowerBalanceSupported bit field value.
         *
         * @return Bit field value
         */
        public int getPedalPowerBalanceSupported() {
            return getValue(0, 0);
        }

        /**
         * Set PedalPowerBalanceSupported bit field value
         * and update the related Bluetooth GATT Characteristic.
         *
         * @param value Value to write to PedalPowerBalanceSupported bit field
         * @return      True, if the value has been set successfully
         */
        public boolean setPedalPowerBalanceSupported(int value) {
            if (!setValue(0, 0, value)) {
                return false;
            }
            updateGattCharacteristic();
            return true;
        }

        /**
         * Get AccumulatedTorqueSupported bit field value.
         *
         * @return Bit field value
         */
        public int getAccumulatedTorqueSupported() {
            return getValue(1, 1);
        }

        /**
         * Set AccumulatedTorqueSupported bit field value
         * and update the related Bluetooth GATT Characteristic.
         *
         * @param value Value to write to AccumulatedTorqueSupported bit field
         * @return      True, if the value has been set successfully
         */
        public boolean setAccumulatedTorqueSupported(int value) {
            if (!setValue(1, 1, value)) {
                return false;
            }
            updateGattCharacteristic();
            return true;
        }

        /**
         * Get WheelRevolutionDataSupported bit field value.
         *
         * @return Bit field value
         */
        public int getWheelRevolutionDataSupported() {
            return getValue(2, 2);
        }

        /**
         * Set WheelRevolutionDataSupported bit field value
         * and update the related Bluetooth GATT Characteristic.
         *
         * @param value Value to write to WheelRevolutionDataSupported bit field
         * @return      True, if the value has been set successfully
         */
        public boolean setWheelRevolutionDataSupported(int value) {
            if (!setValue(2, 2, value)) {
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
            return getValue(3, 3);
        }

        /**
         * Set CrankRevolutionDataSupported bit field value
         * and update the related Bluetooth GATT Characteristic.
         *
         * @param value Value to write to CrankRevolutionDataSupported bit field
         * @return      True, if the value has been set successfully
         */
        public boolean setCrankRevolutionDataSupported(int value) {
            if (!setValue(3, 3, value)) {
                return false;
            }
            updateGattCharacteristic();
            return true;
        }

        /**
         * Get ExtremeMagnitudesSupported bit field value.
         *
         * @return Bit field value
         */
        public int getExtremeMagnitudesSupported() {
            return getValue(4, 4);
        }

        /**
         * Set ExtremeMagnitudesSupported bit field value
         * and update the related Bluetooth GATT Characteristic.
         *
         * @param value Value to write to ExtremeMagnitudesSupported bit field
         * @return      True, if the value has been set successfully
         */
        public boolean setExtremeMagnitudesSupported(int value) {
            if (!setValue(4, 4, value)) {
                return false;
            }
            updateGattCharacteristic();
            return true;
        }

        /**
         * Get ExtremeAnglesSupported bit field value.
         *
         * @return Bit field value
         */
        public int getExtremeAnglesSupported() {
            return getValue(5, 5);
        }

        /**
         * Set ExtremeAnglesSupported bit field value
         * and update the related Bluetooth GATT Characteristic.
         *
         * @param value Value to write to ExtremeAnglesSupported bit field
         * @return      True, if the value has been set successfully
         */
        public boolean setExtremeAnglesSupported(int value) {
            if (!setValue(5, 5, value)) {
                return false;
            }
            updateGattCharacteristic();
            return true;
        }

        /**
         * Get TopAndBottomDeadSpotAnglesSupported bit field value.
         *
         * @return Bit field value
         */
        public int getTopAndBottomDeadSpotAnglesSupported() {
            return getValue(6, 6);
        }

        /**
         * Set TopAndBottomDeadSpotAnglesSupported bit field value
         * and update the related Bluetooth GATT Characteristic.
         *
         * @param value Value to write to TopAndBottomDeadSpotAnglesSupported bit field
         * @return      True, if the value has been set successfully
         */
        public boolean setTopAndBottomDeadSpotAnglesSupported(int value) {
            if (!setValue(6, 6, value)) {
                return false;
            }
            updateGattCharacteristic();
            return true;
        }

        /**
         * Get AccumulatedEnergySupported bit field value.
         *
         * @return Bit field value
         */
        public int getAccumulatedEnergySupported() {
            return getValue(7, 7);
        }

        /**
         * Set AccumulatedEnergySupported bit field value
         * and update the related Bluetooth GATT Characteristic.
         *
         * @param value Value to write to AccumulatedEnergySupported bit field
         * @return      True, if the value has been set successfully
         */
        public boolean setAccumulatedEnergySupported(int value) {
            if (!setValue(7, 7, value)) {
                return false;
            }
            updateGattCharacteristic();
            return true;
        }

        /**
         * Get OffsetCompensationIndicatorSupported bit field value.
         *
         * @return Bit field value
         */
        public int getOffsetCompensationIndicatorSupported() {
            return getValue(8, 8);
        }

        /**
         * Set OffsetCompensationIndicatorSupported bit field value
         * and update the related Bluetooth GATT Characteristic.
         *
         * @param value Value to write to OffsetCompensationIndicatorSupported bit field
         * @return      True, if the value has been set successfully
         */
        public boolean setOffsetCompensationIndicatorSupported(int value) {
            if (!setValue(8, 8, value)) {
                return false;
            }
            updateGattCharacteristic();
            return true;
        }

        /**
         * Get OffsetCompensationSupported bit field value.
         *
         * @return Bit field value
         */
        public int getOffsetCompensationSupported() {
            return getValue(9, 9);
        }

        /**
         * Set OffsetCompensationSupported bit field value
         * and update the related Bluetooth GATT Characteristic.
         *
         * @param value Value to write to OffsetCompensationSupported bit field
         * @return      True, if the value has been set successfully
         */
        public boolean setOffsetCompensationSupported(int value) {
            if (!setValue(9, 9, value)) {
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
            return getValue(10, 10);
        }

        /**
         * Set CharacteristicContentMaskingSupported bit field value
         * and update the related Bluetooth GATT Characteristic.
         *
         * @param value Value to write to CharacteristicContentMaskingSupported bit field
         * @return      True, if the value has been set successfully
         */
        public boolean setCharacteristicContentMaskingSupported(int value) {
            if (!setValue(10, 10, value)) {
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
            return getValue(11, 11);
        }

        /**
         * Set MultipleSensorLocationsSupported bit field value
         * and update the related Bluetooth GATT Characteristic.
         *
         * @param value Value to write to MultipleSensorLocationsSupported bit field
         * @return      True, if the value has been set successfully
         */
        public boolean setMultipleSensorLocationsSupported(int value) {
            if (!setValue(11, 11, value)) {
                return false;
            }
            updateGattCharacteristic();
            return true;
        }

        /**
         * Get CrankLengthAdjustmentSupported bit field value.
         *
         * @return Bit field value
         */
        public int getCrankLengthAdjustmentSupported() {
            return getValue(12, 12);
        }

        /**
         * Set CrankLengthAdjustmentSupported bit field value
         * and update the related Bluetooth GATT Characteristic.
         *
         * @param value Value to write to CrankLengthAdjustmentSupported bit field
         * @return      True, if the value has been set successfully
         */
        public boolean setCrankLengthAdjustmentSupported(int value) {
            if (!setValue(12, 12, value)) {
                return false;
            }
            updateGattCharacteristic();
            return true;
        }

        /**
         * Get ChainLengthAdjustmentSupported bit field value.
         *
         * @return Bit field value
         */
        public int getChainLengthAdjustmentSupported() {
            return getValue(13, 13);
        }

        /**
         * Set ChainLengthAdjustmentSupported bit field value
         * and update the related Bluetooth GATT Characteristic.
         *
         * @param value Value to write to ChainLengthAdjustmentSupported bit field
         * @return      True, if the value has been set successfully
         */
        public boolean setChainLengthAdjustmentSupported(int value) {
            if (!setValue(13, 13, value)) {
                return false;
            }
            updateGattCharacteristic();
            return true;
        }

        /**
         * Get ChainWeightAdjustmentSupported bit field value.
         *
         * @return Bit field value
         */
        public int getChainWeightAdjustmentSupported() {
            return getValue(14, 14);
        }

        /**
         * Set ChainWeightAdjustmentSupported bit field value
         * and update the related Bluetooth GATT Characteristic.
         *
         * @param value Value to write to ChainWeightAdjustmentSupported bit field
         * @return      True, if the value has been set successfully
         */
        public boolean setChainWeightAdjustmentSupported(int value) {
            if (!setValue(14, 14, value)) {
                return false;
            }
            updateGattCharacteristic();
            return true;
        }

        /**
         * Get SpanLengthAdjustmentSupported bit field value.
         *
         * @return Bit field value
         */
        public int getSpanLengthAdjustmentSupported() {
            return getValue(15, 15);
        }

        /**
         * Set SpanLengthAdjustmentSupported bit field value
         * and update the related Bluetooth GATT Characteristic.
         *
         * @param value Value to write to SpanLengthAdjustmentSupported bit field
         * @return      True, if the value has been set successfully
         */
        public boolean setSpanLengthAdjustmentSupported(int value) {
            if (!setValue(15, 15, value)) {
                return false;
            }
            updateGattCharacteristic();
            return true;
        }

        /**
         * Get SensorMeasurementContext bit field value.
         *
         * @return Bit field value
         */
        public int getSensorMeasurementContext() {
            return getValue(16, 16);
        }

        /**
         * Set SensorMeasurementContext bit field value
         * and update the related Bluetooth GATT Characteristic.
         *
         * @param value Value to write to SensorMeasurementContext bit field
         * @return      True, if the value has been set successfully
         */
        public boolean setSensorMeasurementContext(int value) {
            if (!setValue(16, 16, value)) {
                return false;
            }
            updateGattCharacteristic();
            return true;
        }

        /**
         * Get InstantaneousMeasurementDirectionSupported bit field value.
         *
         * @return Bit field value
         */
        public int getInstantaneousMeasurementDirectionSupported() {
            return getValue(17, 17);
        }

        /**
         * Set InstantaneousMeasurementDirectionSupported bit field value
         * and update the related Bluetooth GATT Characteristic.
         *
         * @param value Value to write to InstantaneousMeasurementDirectionSupported bit field
         * @return      True, if the value has been set successfully
         */
        public boolean setInstantaneousMeasurementDirectionSupported(int value) {
            if (!setValue(17, 17, value)) {
                return false;
            }
            updateGattCharacteristic();
            return true;
        }

        /**
         * Get FactoryCalibrationDateSupported bit field value.
         *
         * @return Bit field value
         */
        public int getFactoryCalibrationDateSupported() {
            return getValue(18, 18);
        }

        /**
         * Set FactoryCalibrationDateSupported bit field value
         * and update the related Bluetooth GATT Characteristic.
         *
         * @param value Value to write to FactoryCalibrationDateSupported bit field
         * @return      True, if the value has been set successfully
         */
        public boolean setFactoryCalibrationDateSupported(int value) {
            if (!setValue(18, 18, value)) {
                return false;
            }
            updateGattCharacteristic();
            return true;
        }
    }

    // Customized Start: Functions .................................................................

    //................................................................. Customized End: Functions //
}

