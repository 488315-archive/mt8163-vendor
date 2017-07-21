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
 * Public API for the Cycling Power Measurement Bluetooth GATT Characteristic.
 *
 * <p>This class provides Cycling Power Measurement Bluetooth GATT Characteristic
 * value encode/decode functions. Allowing applications easy and quick to
 * read/write characteristic field value.
 *
 * <p>Name: Cycling Power Measurement
 * Type: org.bluetooth.characteristic.cycling_power_measurement
 * UUID: 2A63
 * Last Modified: 2014-07-02
 * Revision: None
 */
public class CyclingPowerMeasurement extends CharacteristicBase {
    /**
     * Cycling Power Measurement UUID.
     */
    public static final UUID GATT_UUID =
            UUID.fromString(FormatUtils.uuid16ToUuid128("2A63"));

    // Customized Start: Constant Definition .......................................................

    //....................................................... Customized End: Constant Definition //

    /*
     * Field: Flags
     * Requirement: Mandatory
     * Format: bit16
     */
    private BfFlags mFlags = new BfFlags();

    /*
     * Field: Instantaneous Power
     * Requirement: Mandatory
     * Format: sint16
     * Unit: org.bluetooth.unit.power.watt
     */
    private byte[] mInstantaneousPower = new byte[FormatUtils.SINT16_SIZE];

    /*
     * Field: Pedal Power Balance
     * Requirement: Optional
     * Format: uint8
     * Unit: org.bluetooth.unit.percentage
     */
    private byte[] mPedalPowerBalance = new byte[FormatUtils.UINT8_SIZE];

    /*
     * Field: Accumulated Torque
     * Requirement: Optional
     * Format: uint16
     * Unit: org.bluetooth.unit.moment_of_force.newton_metre
     */
    private byte[] mAccumulatedTorque = new byte[FormatUtils.UINT16_SIZE];

    /*
     * Field: Wheel Revolution Data - Cumulative Wheel Revolutions
     * Requirement: C1
     * Format: uint32
     * Unit: org.bluetooth.unit.unitless
     */
    private byte[] mWheelCumulativeWheelRevolutions = new byte[FormatUtils.UINT32_SIZE];

    /*
     * Field: Wheel Revolution Data - Last Wheel Event Time
     * Requirement: C1
     * Format: uint16
     * Unit: org.bluetooth.unit.time.second
     */
    private byte[] mWheelLastWheelEventTime = new byte[FormatUtils.UINT16_SIZE];

    /*
     * Field: Crank Revolution Data- Cumulative Crank Revolutions
     * Requirement: C2
     * Format: uint16
     * Unit: org.bluetooth.unit.unitless
     */
    private byte[] mCrankCumulativeCrankRevolutions = new byte[FormatUtils.UINT16_SIZE];

    /*
     * Field: Crank Revolution Data- Last Crank Event Time
     * Requirement: C2
     * Format: uint16
     * Unit: org.bluetooth.unit.time.second
     */
    private byte[] mCrankLastCrankEventTime = new byte[FormatUtils.UINT16_SIZE];

    /*
     * Field: Extreme Force Magnitudes - Maximum Force Magnitude
     * Requirement: C3
     * Format: sint16
     * Unit: org.bluetooth.unit.force.newton
     */
    private byte[] mExtremeMaximumForceMagnitude = new byte[FormatUtils.SINT16_SIZE];

    /*
     * Field: Extreme Force Magnitudes - Minimum Force Magnitude
     * Requirement: C3
     * Format: sint16
     * Unit: org.bluetooth.unit.force.newton
     */
    private byte[] mExtremeMinimumForceMagnitude = new byte[FormatUtils.SINT16_SIZE];

    /*
     * Field: Extreme Torque Magnitudes- Maximum Torque Magnitude
     * Requirement: C4
     * Format: sint16
     * Unit: org.bluetooth.unit.moment_of_force.newton_metre
     */
    private byte[] mExtremeMaximumTorqueMagnitude = new byte[FormatUtils.SINT16_SIZE];

    /*
     * Field: Extreme Torque Magnitudes- Minimum Torque Magnitude
     * Requirement: C4
     * Format: sint16
     * Unit: org.bluetooth.unit.moment_of_force.newton_metre
     */
    private byte[] mExtremeMinimumTorqueMagnitude = new byte[FormatUtils.SINT16_SIZE];

    /*
     * Field: Extreme Angles - Maximum Angle
     * Requirement: C5
     * Format: uint12
     * Unit: org.bluetooth.unit.plane_angle.degree
     */
    private byte[] mExtremeAnglesMaximumAngle = new byte[FormatUtils.UINT12_SIZE];

    /*
     * Field: Extreme Angles - Minimum Angle
     * Requirement: C5
     * Format: uint12
     * Unit: org.bluetooth.unit.plane_angle.degree
     */
    private byte[] mExtremeAnglesMinimumAngle = new byte[FormatUtils.UINT12_SIZE];

    /*
     * Field: Top Dead Spot Angle
     * Requirement: Optional
     * Format: uint16
     * Unit: org.bluetooth.unit.plane_angle.degree
     */
    private byte[] mTopDeadSpotAngle = new byte[FormatUtils.UINT16_SIZE];

    /*
     * Field: Bottom Dead Spot Angle
     * Requirement: Optional
     * Format: uint16
     * Unit: org.bluetooth.unit.plane_angle.degree
     */
    private byte[] mBottomDeadSpotAngle = new byte[FormatUtils.UINT16_SIZE];

    /*
     * Field: Accumulated Energy
     * Requirement: Optional
     * Format: uint16
     * Unit: org.bluetooth.unit.energy.joule
     */
    private byte[] mAccumulatedEnergy = new byte[FormatUtils.UINT16_SIZE];


    /**
     * Create a CyclingPowerMeasurement characteristic object.
     */
    public CyclingPowerMeasurement() {
        setCharacteristic(null);
        setFlags(new BfFlags());
        setInstantaneousPower(0);
        setPedalPowerBalance(0);
        setAccumulatedTorque(0);
        setWheelCumulativeWheelRevolutions(0);
        setWheelLastWheelEventTime(0);
        setCrankCumulativeCrankRevolutions(0);
        setCrankLastCrankEventTime(0);
        setExtremeMaximumForceMagnitude(0);
        setExtremeMinimumForceMagnitude(0);
        setExtremeMaximumTorqueMagnitude(0);
        setExtremeMinimumTorqueMagnitude(0);
        setExtremeAnglesMaximumAngle(0);
        setExtremeAnglesMinimumAngle(0);
        setTopDeadSpotAngle(0);
        setBottomDeadSpotAngle(0);
        setAccumulatedEnergy(0);
    }

    /**
     * Create a CyclingPowerMeasurement characteristic object and init value.
     *
     * @param value Initial value
     */
    public CyclingPowerMeasurement(byte[] value) {
        setCharacteristic(null);
        setValue(value);
    }

    /**
     * Create a CyclingPowerMeasurement characteristic object and init value.
     *
     * @param value Initial value
     * @param characteristic the gatt-characteristic this characteristic belongs to.
     */
    public CyclingPowerMeasurement(byte[] value, BluetoothGattCharacteristic characteristic) {
        setCharacteristic(characteristic);
        setValue(value);
    }

    /**
     * Create a CyclingPowerMeasurement characteristic object.
     *
     * @param flags Flags
     * @param instantaneousPower Instantaneous Power
     * @param pedalPowerBalance Pedal Power Balance
     * @param accumulatedTorque Accumulated Torque
     * @param wheelCumulativeWheelRevolutions Wheel Revolution Data - Cumulative Wheel Revolutions
     * @param wheelLastWheelEventTime Wheel Revolution Data - Last Wheel Event Time
     * @param crankCumulativeCrankRevolutions Crank Revolution Data- Cumulative Crank Revolutions
     * @param crankLastCrankEventTime Crank Revolution Data- Last Crank Event Time
     * @param extremeMaximumForceMagnitude Extreme Force Magnitudes - Maximum Force Magnitude
     * @param extremeMinimumForceMagnitude Extreme Force Magnitudes - Minimum Force Magnitude
     * @param extremeMaximumTorqueMagnitude Extreme Torque Magnitudes- Maximum Torque Magnitude
     * @param extremeMinimumTorqueMagnitude Extreme Torque Magnitudes- Minimum Torque Magnitude
     * @param extremeAnglesMaximumAngle Extreme Angles - Maximum Angle
     * @param extremeAnglesMinimumAngle Extreme Angles - Minimum Angle
     * @param topDeadSpotAngle Top Dead Spot Angle
     * @param bottomDeadSpotAngle Bottom Dead Spot Angle
     * @param accumulatedEnergy Accumulated Energy
     */
    public CyclingPowerMeasurement(
            BfFlags flags,
            int instantaneousPower,
            int pedalPowerBalance,
            int accumulatedTorque,
            long wheelCumulativeWheelRevolutions,
            int wheelLastWheelEventTime,
            int crankCumulativeCrankRevolutions,
            int crankLastCrankEventTime,
            int extremeMaximumForceMagnitude,
            int extremeMinimumForceMagnitude,
            int extremeMaximumTorqueMagnitude,
            int extremeMinimumTorqueMagnitude,
            int extremeAnglesMaximumAngle,
            int extremeAnglesMinimumAngle,
            int topDeadSpotAngle,
            int bottomDeadSpotAngle,
            int accumulatedEnergy) {
        setCharacteristic(null);
        setFlags(flags);
        setInstantaneousPower(instantaneousPower);
        setPedalPowerBalance(pedalPowerBalance);
        setAccumulatedTorque(accumulatedTorque);
        setWheelCumulativeWheelRevolutions(wheelCumulativeWheelRevolutions);
        setWheelLastWheelEventTime(wheelLastWheelEventTime);
        setCrankCumulativeCrankRevolutions(crankCumulativeCrankRevolutions);
        setCrankLastCrankEventTime(crankLastCrankEventTime);
        setExtremeMaximumForceMagnitude(extremeMaximumForceMagnitude);
        setExtremeMinimumForceMagnitude(extremeMinimumForceMagnitude);
        setExtremeMaximumTorqueMagnitude(extremeMaximumTorqueMagnitude);
        setExtremeMinimumTorqueMagnitude(extremeMinimumTorqueMagnitude);
        setExtremeAnglesMaximumAngle(extremeAnglesMaximumAngle);
        setExtremeAnglesMinimumAngle(extremeAnglesMinimumAngle);
        setTopDeadSpotAngle(topDeadSpotAngle);
        setBottomDeadSpotAngle(bottomDeadSpotAngle);
        setAccumulatedEnergy(accumulatedEnergy);
    }

    /**
     * Create a CyclingPowerMeasurement characteristic object.
     *
     * @param flags Flags
     * @param instantaneousPower Instantaneous Power
     * @param pedalPowerBalance Pedal Power Balance
     * @param accumulatedTorque Accumulated Torque
     * @param wheelCumulativeWheelRevolutions Wheel Revolution Data - Cumulative Wheel Revolutions
     * @param wheelLastWheelEventTime Wheel Revolution Data - Last Wheel Event Time
     * @param crankCumulativeCrankRevolutions Crank Revolution Data- Cumulative Crank Revolutions
     * @param crankLastCrankEventTime Crank Revolution Data- Last Crank Event Time
     * @param extremeMaximumForceMagnitude Extreme Force Magnitudes - Maximum Force Magnitude
     * @param extremeMinimumForceMagnitude Extreme Force Magnitudes - Minimum Force Magnitude
     * @param extremeMaximumTorqueMagnitude Extreme Torque Magnitudes- Maximum Torque Magnitude
     * @param extremeMinimumTorqueMagnitude Extreme Torque Magnitudes- Minimum Torque Magnitude
     * @param extremeAnglesMaximumAngle Extreme Angles - Maximum Angle
     * @param extremeAnglesMinimumAngle Extreme Angles - Minimum Angle
     * @param topDeadSpotAngle Top Dead Spot Angle
     * @param bottomDeadSpotAngle Bottom Dead Spot Angle
     * @param accumulatedEnergy Accumulated Energy
     * @param characteristic the gatt-characteristic this characteristic belongs to.
     */
    public CyclingPowerMeasurement(
            BfFlags flags,
            int instantaneousPower,
            int pedalPowerBalance,
            int accumulatedTorque,
            long wheelCumulativeWheelRevolutions,
            int wheelLastWheelEventTime,
            int crankCumulativeCrankRevolutions,
            int crankLastCrankEventTime,
            int extremeMaximumForceMagnitude,
            int extremeMinimumForceMagnitude,
            int extremeMaximumTorqueMagnitude,
            int extremeMinimumTorqueMagnitude,
            int extremeAnglesMaximumAngle,
            int extremeAnglesMinimumAngle,
            int topDeadSpotAngle,
            int bottomDeadSpotAngle,
            int accumulatedEnergy,
            BluetoothGattCharacteristic characteristic) {
        setCharacteristic(characteristic);
        setFlags(flags);
        setInstantaneousPower(instantaneousPower);
        setPedalPowerBalance(pedalPowerBalance);
        setAccumulatedTorque(accumulatedTorque);
        setWheelCumulativeWheelRevolutions(wheelCumulativeWheelRevolutions);
        setWheelLastWheelEventTime(wheelLastWheelEventTime);
        setCrankCumulativeCrankRevolutions(crankCumulativeCrankRevolutions);
        setCrankLastCrankEventTime(crankLastCrankEventTime);
        setExtremeMaximumForceMagnitude(extremeMaximumForceMagnitude);
        setExtremeMinimumForceMagnitude(extremeMinimumForceMagnitude);
        setExtremeMaximumTorqueMagnitude(extremeMaximumTorqueMagnitude);
        setExtremeMinimumTorqueMagnitude(extremeMinimumTorqueMagnitude);
        setExtremeAnglesMaximumAngle(extremeAnglesMaximumAngle);
        setExtremeAnglesMinimumAngle(extremeAnglesMinimumAngle);
        setTopDeadSpotAngle(topDeadSpotAngle);
        setBottomDeadSpotAngle(bottomDeadSpotAngle);
        setAccumulatedEnergy(accumulatedEnergy);
    }

    // Customized Start: Constructors ..............................................................

    //.............................................................. Customized End: Constructors //

    /**
     * Get CyclingPowerMeasurement characteristic byte length.
     *
     * @return Byte length of this characteristic
     */
    public int length() {
        return (isSupportFlags()
                        ? mFlags.length() : 0)
                + (isSupportInstantaneousPower()
                        ? mInstantaneousPower.length : 0)
                + (isSupportPedalPowerBalance()
                        ? mPedalPowerBalance.length : 0)
                + (isSupportAccumulatedTorque()
                        ? mAccumulatedTorque.length : 0)
                + (isSupportWheelCumulativeWheelRevolutions()
                        ? mWheelCumulativeWheelRevolutions.length : 0)
                + (isSupportWheelLastWheelEventTime()
                        ? mWheelLastWheelEventTime.length : 0)
                + (isSupportCrankCumulativeCrankRevolutions()
                        ? mCrankCumulativeCrankRevolutions.length : 0)
                + (isSupportCrankLastCrankEventTime()
                        ? mCrankLastCrankEventTime.length : 0)
                + (isSupportExtremeMaximumForceMagnitude()
                        ? mExtremeMaximumForceMagnitude.length : 0)
                + (isSupportExtremeMinimumForceMagnitude()
                        ? mExtremeMinimumForceMagnitude.length : 0)
                + (isSupportExtremeMaximumTorqueMagnitude()
                        ? mExtremeMaximumTorqueMagnitude.length : 0)
                + (isSupportExtremeMinimumTorqueMagnitude()
                        ? mExtremeMinimumTorqueMagnitude.length : 0)
                + (isSupportExtremeAnglesMaximumAngle()
                        ? mExtremeAnglesMaximumAngle.length : 0)
                + (isSupportExtremeAnglesMinimumAngle()
                        ? mExtremeAnglesMinimumAngle.length : 0)
                + (isSupportTopDeadSpotAngle()
                        ? mTopDeadSpotAngle.length : 0)
                + (isSupportBottomDeadSpotAngle()
                        ? mBottomDeadSpotAngle.length : 0)
                + (isSupportAccumulatedEnergy()
                        ? mAccumulatedEnergy.length : 0);
    }

    /**
     * Get CyclingPowerMeasurement characteristic UUID.
     *
     * @return Characteristic UUID
     */
    @Override
    public UUID getUuid() {
        return GATT_UUID;
    }

    /**
     * Get CyclingPowerMeasurement characteristic value.
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

        if (isSupportInstantaneousPower()) {
            int fieldLen = mInstantaneousPower.length;
            System.arraycopy(mInstantaneousPower, 0, value, dstPos, fieldLen);
            dstPos += fieldLen;
        }

        if (isSupportPedalPowerBalance()) {
            int fieldLen = mPedalPowerBalance.length;
            System.arraycopy(mPedalPowerBalance, 0, value, dstPos, fieldLen);
            dstPos += fieldLen;
        }

        if (isSupportAccumulatedTorque()) {
            int fieldLen = mAccumulatedTorque.length;
            System.arraycopy(mAccumulatedTorque, 0, value, dstPos, fieldLen);
            dstPos += fieldLen;
        }

        if (isSupportWheelCumulativeWheelRevolutions()) {
            int fieldLen = mWheelCumulativeWheelRevolutions.length;
            System.arraycopy(mWheelCumulativeWheelRevolutions, 0, value, dstPos, fieldLen);
            dstPos += fieldLen;
        }

        if (isSupportWheelLastWheelEventTime()) {
            int fieldLen = mWheelLastWheelEventTime.length;
            System.arraycopy(mWheelLastWheelEventTime, 0, value, dstPos, fieldLen);
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

        if (isSupportExtremeMaximumForceMagnitude()) {
            int fieldLen = mExtremeMaximumForceMagnitude.length;
            System.arraycopy(mExtremeMaximumForceMagnitude, 0, value, dstPos, fieldLen);
            dstPos += fieldLen;
        }

        if (isSupportExtremeMinimumForceMagnitude()) {
            int fieldLen = mExtremeMinimumForceMagnitude.length;
            System.arraycopy(mExtremeMinimumForceMagnitude, 0, value, dstPos, fieldLen);
            dstPos += fieldLen;
        }

        if (isSupportExtremeMaximumTorqueMagnitude()) {
            int fieldLen = mExtremeMaximumTorqueMagnitude.length;
            System.arraycopy(mExtremeMaximumTorqueMagnitude, 0, value, dstPos, fieldLen);
            dstPos += fieldLen;
        }

        if (isSupportExtremeMinimumTorqueMagnitude()) {
            int fieldLen = mExtremeMinimumTorqueMagnitude.length;
            System.arraycopy(mExtremeMinimumTorqueMagnitude, 0, value, dstPos, fieldLen);
            dstPos += fieldLen;
        }

        if (isSupportExtremeAnglesMaximumAngle()) {
            int fieldLen = mExtremeAnglesMaximumAngle.length;
            System.arraycopy(mExtremeAnglesMaximumAngle, 0, value, dstPos, fieldLen);
            dstPos += fieldLen;
        }

        if (isSupportExtremeAnglesMinimumAngle()) {
            int fieldLen = mExtremeAnglesMinimumAngle.length;
            System.arraycopy(mExtremeAnglesMinimumAngle, 0, value, dstPos, fieldLen);
            dstPos += fieldLen;
        }

        if (isSupportTopDeadSpotAngle()) {
            int fieldLen = mTopDeadSpotAngle.length;
            System.arraycopy(mTopDeadSpotAngle, 0, value, dstPos, fieldLen);
            dstPos += fieldLen;
        }

        if (isSupportBottomDeadSpotAngle()) {
            int fieldLen = mBottomDeadSpotAngle.length;
            System.arraycopy(mBottomDeadSpotAngle, 0, value, dstPos, fieldLen);
            dstPos += fieldLen;
        }

        if (isSupportAccumulatedEnergy()) {
            int fieldLen = mAccumulatedEnergy.length;
            System.arraycopy(mAccumulatedEnergy, 0, value, dstPos, fieldLen);
            dstPos += fieldLen;
        }

        return value;
    }

    /**
     * Set CyclingPowerMeasurement characteristic value.
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

        if (isSupportInstantaneousPower()) {
            int fieldLen = mInstantaneousPower.length;

            if (!setValueRangeCheck(value.length, srcPos, fieldLen)) {
                return false;
            }

            System.arraycopy(value, srcPos, mInstantaneousPower, 0, fieldLen);
            srcPos += fieldLen;
        }

        if (isSupportPedalPowerBalance()) {
            int fieldLen = mPedalPowerBalance.length;

            if (!setValueRangeCheck(value.length, srcPos, fieldLen)) {
                return false;
            }

            System.arraycopy(value, srcPos, mPedalPowerBalance, 0, fieldLen);
            srcPos += fieldLen;
        }

        if (isSupportAccumulatedTorque()) {
            int fieldLen = mAccumulatedTorque.length;

            if (!setValueRangeCheck(value.length, srcPos, fieldLen)) {
                return false;
            }

            System.arraycopy(value, srcPos, mAccumulatedTorque, 0, fieldLen);
            srcPos += fieldLen;
        }

        if (isSupportWheelCumulativeWheelRevolutions()) {
            int fieldLen = mWheelCumulativeWheelRevolutions.length;

            if (!setValueRangeCheck(value.length, srcPos, fieldLen)) {
                return false;
            }

            System.arraycopy(value, srcPos, mWheelCumulativeWheelRevolutions, 0, fieldLen);
            srcPos += fieldLen;
        }

        if (isSupportWheelLastWheelEventTime()) {
            int fieldLen = mWheelLastWheelEventTime.length;

            if (!setValueRangeCheck(value.length, srcPos, fieldLen)) {
                return false;
            }

            System.arraycopy(value, srcPos, mWheelLastWheelEventTime, 0, fieldLen);
            srcPos += fieldLen;
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

        if (isSupportExtremeMaximumForceMagnitude()) {
            int fieldLen = mExtremeMaximumForceMagnitude.length;

            if (!setValueRangeCheck(value.length, srcPos, fieldLen)) {
                return false;
            }

            System.arraycopy(value, srcPos, mExtremeMaximumForceMagnitude, 0, fieldLen);
            srcPos += fieldLen;
        }

        if (isSupportExtremeMinimumForceMagnitude()) {
            int fieldLen = mExtremeMinimumForceMagnitude.length;

            if (!setValueRangeCheck(value.length, srcPos, fieldLen)) {
                return false;
            }

            System.arraycopy(value, srcPos, mExtremeMinimumForceMagnitude, 0, fieldLen);
            srcPos += fieldLen;
        }

        if (isSupportExtremeMaximumTorqueMagnitude()) {
            int fieldLen = mExtremeMaximumTorqueMagnitude.length;

            if (!setValueRangeCheck(value.length, srcPos, fieldLen)) {
                return false;
            }

            System.arraycopy(value, srcPos, mExtremeMaximumTorqueMagnitude, 0, fieldLen);
            srcPos += fieldLen;
        }

        if (isSupportExtremeMinimumTorqueMagnitude()) {
            int fieldLen = mExtremeMinimumTorqueMagnitude.length;

            if (!setValueRangeCheck(value.length, srcPos, fieldLen)) {
                return false;
            }

            System.arraycopy(value, srcPos, mExtremeMinimumTorqueMagnitude, 0, fieldLen);
            srcPos += fieldLen;
        }

        if (isSupportExtremeAnglesMaximumAngle()) {
            int fieldLen = mExtremeAnglesMaximumAngle.length;

            if (!setValueRangeCheck(value.length, srcPos, fieldLen)) {
                return false;
            }

            System.arraycopy(value, srcPos, mExtremeAnglesMaximumAngle, 0, fieldLen);
            srcPos += fieldLen;
        }

        if (isSupportExtremeAnglesMinimumAngle()) {
            int fieldLen = mExtremeAnglesMinimumAngle.length;

            if (!setValueRangeCheck(value.length, srcPos, fieldLen)) {
                return false;
            }

            System.arraycopy(value, srcPos, mExtremeAnglesMinimumAngle, 0, fieldLen);
            srcPos += fieldLen;
        }

        if (isSupportTopDeadSpotAngle()) {
            int fieldLen = mTopDeadSpotAngle.length;

            if (!setValueRangeCheck(value.length, srcPos, fieldLen)) {
                return false;
            }

            System.arraycopy(value, srcPos, mTopDeadSpotAngle, 0, fieldLen);
            srcPos += fieldLen;
        }

        if (isSupportBottomDeadSpotAngle()) {
            int fieldLen = mBottomDeadSpotAngle.length;

            if (!setValueRangeCheck(value.length, srcPos, fieldLen)) {
                return false;
            }

            System.arraycopy(value, srcPos, mBottomDeadSpotAngle, 0, fieldLen);
            srcPos += fieldLen;
        }

        if (isSupportAccumulatedEnergy()) {
            int fieldLen = mAccumulatedEnergy.length;

            if (!setValueRangeCheck(value.length, srcPos, fieldLen)) {
                return false;
            }

            System.arraycopy(value, srcPos, mAccumulatedEnergy, 0, fieldLen);
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
     * Return true if CyclingPowerMeasurement support Flags field.
     *
     * @return  True, if CyclingPowerMeasurement support Flags field.
     */
    public boolean isSupportFlags() {
        return true;
    }

    /**
     * Get InstantaneousPower field value with int format.
     *
     * @return InstantaneousPower field value
     */
    public int getInstantaneousPower() {
        return FormatUtils.sint16ToInt(mInstantaneousPower);
    }

    /**
     * Set InstantaneousPower field value by int format
     * and update the related Bluetooth GATT Characteristic.
     *
     * @param value Value to write to InstantaneousPower field
     * @return      True, if the value has been set successfully
     */
    public boolean setInstantaneousPower(int value) {
        if (!FormatUtils.sint16RangeCheck(value)) {
            return false;
        }
        mInstantaneousPower = FormatUtils.intToSint16(value);
        updateGattCharacteristic();
        return true;
    }

    /**
     * Return true if CyclingPowerMeasurement support InstantaneousPower field.
     *
     * @return  True, if CyclingPowerMeasurement support InstantaneousPower field.
     */
    public boolean isSupportInstantaneousPower() {
        return true;
    }

    /**
     * Get PedalPowerBalance field value with int format.
     *
     * @return PedalPowerBalance field value
     */
    public int getPedalPowerBalance() {
        return FormatUtils.uint8ToInt(mPedalPowerBalance);
    }

    /**
     * Set PedalPowerBalance field value by int format
     * and update the related Bluetooth GATT Characteristic.
     *
     * @param value Value to write to PedalPowerBalance field
     * @return      True, if the value has been set successfully
     */
    public boolean setPedalPowerBalance(int value) {
        if (!FormatUtils.uint8RangeCheck(value)) {
            return false;
        }
        mPedalPowerBalance = FormatUtils.intToUint8(value);
        updateGattCharacteristic();
        return true;
    }

    /**
     * Return true if CyclingPowerMeasurement support PedalPowerBalance field.
     *
     * @return  True, if CyclingPowerMeasurement support PedalPowerBalance field.
     */
    public boolean isSupportPedalPowerBalance() {
        return (mFlags.getPedalPowerBalancePresent() == 1);
    }

    /**
     * Get AccumulatedTorque field value with int format.
     *
     * @return AccumulatedTorque field value
     */
    public int getAccumulatedTorque() {
        return FormatUtils.uint16ToInt(mAccumulatedTorque);
    }

    /**
     * Set AccumulatedTorque field value by int format
     * and update the related Bluetooth GATT Characteristic.
     *
     * @param value Value to write to AccumulatedTorque field
     * @return      True, if the value has been set successfully
     */
    public boolean setAccumulatedTorque(int value) {
        if (!FormatUtils.uint16RangeCheck(value)) {
            return false;
        }
        mAccumulatedTorque = FormatUtils.intToUint16(value);
        updateGattCharacteristic();
        return true;
    }

    /**
     * Return true if CyclingPowerMeasurement support AccumulatedTorque field.
     *
     * @return  True, if CyclingPowerMeasurement support AccumulatedTorque field.
     */
    public boolean isSupportAccumulatedTorque() {
        return (mFlags.getAccumulatedTorquePresent() == 1);
    }

    /**
     * Get WheelCumulativeWheelRevolutions field value with long format.
     *
     * @return WheelCumulativeWheelRevolutions field value
     */
    public long getWheelCumulativeWheelRevolutions() {
        return FormatUtils.uint32ToLong(mWheelCumulativeWheelRevolutions);
    }

    /**
     * Set WheelCumulativeWheelRevolutions field value by long format
     * and update the related Bluetooth GATT Characteristic.
     *
     * @param value Value to write to WheelCumulativeWheelRevolutions field
     * @return      True, if the value has been set successfully
     */
    public boolean setWheelCumulativeWheelRevolutions(long value) {
        if (!FormatUtils.uint32RangeCheck(value)) {
            return false;
        }
        mWheelCumulativeWheelRevolutions = FormatUtils.longToUint32(value);
        updateGattCharacteristic();
        return true;
    }

    /**
     * Return true if CyclingPowerMeasurement support WheelCumulativeWheelRevolutions field.
     *
     * @return  True, if CyclingPowerMeasurement support WheelCumulativeWheelRevolutions field.
     */
    public boolean isSupportWheelCumulativeWheelRevolutions() {
        return (mFlags.getWheelRevolutionDataPresent() == 1);
    }

    /**
     * Get WheelLastWheelEventTime field value with int format.
     *
     * @return WheelLastWheelEventTime field value
     */
    public int getWheelLastWheelEventTime() {
        return FormatUtils.uint16ToInt(mWheelLastWheelEventTime);
    }

    /**
     * Set WheelLastWheelEventTime field value by int format
     * and update the related Bluetooth GATT Characteristic.
     *
     * @param value Value to write to WheelLastWheelEventTime field
     * @return      True, if the value has been set successfully
     */
    public boolean setWheelLastWheelEventTime(int value) {
        if (!FormatUtils.uint16RangeCheck(value)) {
            return false;
        }
        mWheelLastWheelEventTime = FormatUtils.intToUint16(value);
        updateGattCharacteristic();
        return true;
    }

    /**
     * Return true if CyclingPowerMeasurement support WheelLastWheelEventTime field.
     *
     * @return  True, if CyclingPowerMeasurement support WheelLastWheelEventTime field.
     */
    public boolean isSupportWheelLastWheelEventTime() {
        return (mFlags.getWheelRevolutionDataPresent() == 1);
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
     * Return true if CyclingPowerMeasurement support CrankCumulativeCrankRevolutions field.
     *
     * @return  True, if CyclingPowerMeasurement support CrankCumulativeCrankRevolutions field.
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
     * Return true if CyclingPowerMeasurement support CrankLastCrankEventTime field.
     *
     * @return  True, if CyclingPowerMeasurement support CrankLastCrankEventTime field.
     */
    public boolean isSupportCrankLastCrankEventTime() {
        return (mFlags.getCrankRevolutionDataPresent() == 1);
    }

    /**
     * Get ExtremeMaximumForceMagnitude field value with int format.
     *
     * @return ExtremeMaximumForceMagnitude field value
     */
    public int getExtremeMaximumForceMagnitude() {
        return FormatUtils.sint16ToInt(mExtremeMaximumForceMagnitude);
    }

    /**
     * Set ExtremeMaximumForceMagnitude field value by int format
     * and update the related Bluetooth GATT Characteristic.
     *
     * @param value Value to write to ExtremeMaximumForceMagnitude field
     * @return      True, if the value has been set successfully
     */
    public boolean setExtremeMaximumForceMagnitude(int value) {
        if (!FormatUtils.sint16RangeCheck(value)) {
            return false;
        }
        mExtremeMaximumForceMagnitude = FormatUtils.intToSint16(value);
        updateGattCharacteristic();
        return true;
    }

    /**
     * Return true if CyclingPowerMeasurement support ExtremeMaximumForceMagnitude field.
     *
     * @return  True, if CyclingPowerMeasurement support ExtremeMaximumForceMagnitude field.
     */
    public boolean isSupportExtremeMaximumForceMagnitude() {
        return (mFlags.getExtremeForceMagnitudesPresent() == 1);
    }

    /**
     * Get ExtremeMinimumForceMagnitude field value with int format.
     *
     * @return ExtremeMinimumForceMagnitude field value
     */
    public int getExtremeMinimumForceMagnitude() {
        return FormatUtils.sint16ToInt(mExtremeMinimumForceMagnitude);
    }

    /**
     * Set ExtremeMinimumForceMagnitude field value by int format
     * and update the related Bluetooth GATT Characteristic.
     *
     * @param value Value to write to ExtremeMinimumForceMagnitude field
     * @return      True, if the value has been set successfully
     */
    public boolean setExtremeMinimumForceMagnitude(int value) {
        if (!FormatUtils.sint16RangeCheck(value)) {
            return false;
        }
        mExtremeMinimumForceMagnitude = FormatUtils.intToSint16(value);
        updateGattCharacteristic();
        return true;
    }

    /**
     * Return true if CyclingPowerMeasurement support ExtremeMinimumForceMagnitude field.
     *
     * @return  True, if CyclingPowerMeasurement support ExtremeMinimumForceMagnitude field.
     */
    public boolean isSupportExtremeMinimumForceMagnitude() {
        return (mFlags.getExtremeForceMagnitudesPresent() == 1);
    }

    /**
     * Get ExtremeMaximumTorqueMagnitude field value with int format.
     *
     * @return ExtremeMaximumTorqueMagnitude field value
     */
    public int getExtremeMaximumTorqueMagnitude() {
        return FormatUtils.sint16ToInt(mExtremeMaximumTorqueMagnitude);
    }

    /**
     * Set ExtremeMaximumTorqueMagnitude field value by int format
     * and update the related Bluetooth GATT Characteristic.
     *
     * @param value Value to write to ExtremeMaximumTorqueMagnitude field
     * @return      True, if the value has been set successfully
     */
    public boolean setExtremeMaximumTorqueMagnitude(int value) {
        if (!FormatUtils.sint16RangeCheck(value)) {
            return false;
        }
        mExtremeMaximumTorqueMagnitude = FormatUtils.intToSint16(value);
        updateGattCharacteristic();
        return true;
    }

    /**
     * Return true if CyclingPowerMeasurement support ExtremeMaximumTorqueMagnitude field.
     *
     * @return  True, if CyclingPowerMeasurement support ExtremeMaximumTorqueMagnitude field.
     */
    public boolean isSupportExtremeMaximumTorqueMagnitude() {
        return (mFlags.getExtremeTorqueMagnitudesPresent() == 1);
    }

    /**
     * Get ExtremeMinimumTorqueMagnitude field value with int format.
     *
     * @return ExtremeMinimumTorqueMagnitude field value
     */
    public int getExtremeMinimumTorqueMagnitude() {
        return FormatUtils.sint16ToInt(mExtremeMinimumTorqueMagnitude);
    }

    /**
     * Set ExtremeMinimumTorqueMagnitude field value by int format
     * and update the related Bluetooth GATT Characteristic.
     *
     * @param value Value to write to ExtremeMinimumTorqueMagnitude field
     * @return      True, if the value has been set successfully
     */
    public boolean setExtremeMinimumTorqueMagnitude(int value) {
        if (!FormatUtils.sint16RangeCheck(value)) {
            return false;
        }
        mExtremeMinimumTorqueMagnitude = FormatUtils.intToSint16(value);
        updateGattCharacteristic();
        return true;
    }

    /**
     * Return true if CyclingPowerMeasurement support ExtremeMinimumTorqueMagnitude field.
     *
     * @return  True, if CyclingPowerMeasurement support ExtremeMinimumTorqueMagnitude field.
     */
    public boolean isSupportExtremeMinimumTorqueMagnitude() {
        return (mFlags.getExtremeTorqueMagnitudesPresent() == 1);
    }

    /**
     * Get ExtremeAnglesMaximumAngle field value with int format.
     *
     * @return ExtremeAnglesMaximumAngle field value
     */
    public int getExtremeAnglesMaximumAngle() {
        return FormatUtils.uint12ToInt(mExtremeAnglesMaximumAngle);
    }

    /**
     * Set ExtremeAnglesMaximumAngle field value by int format
     * and update the related Bluetooth GATT Characteristic.
     *
     * @param value Value to write to ExtremeAnglesMaximumAngle field
     * @return      True, if the value has been set successfully
     */
    public boolean setExtremeAnglesMaximumAngle(int value) {
        if (!FormatUtils.uint12RangeCheck(value)) {
            return false;
        }
        mExtremeAnglesMaximumAngle = FormatUtils.intToUint12(value);
        updateGattCharacteristic();
        return true;
    }

    /**
     * Return true if CyclingPowerMeasurement support ExtremeAnglesMaximumAngle field.
     *
     * @return  True, if CyclingPowerMeasurement support ExtremeAnglesMaximumAngle field.
     */
    public boolean isSupportExtremeAnglesMaximumAngle() {
        return (mFlags.getExtremeAnglesPresent() == 1);
    }

    /**
     * Get ExtremeAnglesMinimumAngle field value with int format.
     *
     * @return ExtremeAnglesMinimumAngle field value
     */
    public int getExtremeAnglesMinimumAngle() {
        return FormatUtils.uint12ToInt(mExtremeAnglesMinimumAngle);
    }

    /**
     * Set ExtremeAnglesMinimumAngle field value by int format
     * and update the related Bluetooth GATT Characteristic.
     *
     * @param value Value to write to ExtremeAnglesMinimumAngle field
     * @return      True, if the value has been set successfully
     */
    public boolean setExtremeAnglesMinimumAngle(int value) {
        if (!FormatUtils.uint12RangeCheck(value)) {
            return false;
        }
        mExtremeAnglesMinimumAngle = FormatUtils.intToUint12(value);
        updateGattCharacteristic();
        return true;
    }

    /**
     * Return true if CyclingPowerMeasurement support ExtremeAnglesMinimumAngle field.
     *
     * @return  True, if CyclingPowerMeasurement support ExtremeAnglesMinimumAngle field.
     */
    public boolean isSupportExtremeAnglesMinimumAngle() {
        return (mFlags.getExtremeAnglesPresent() == 1);
    }

    /**
     * Get TopDeadSpotAngle field value with int format.
     *
     * @return TopDeadSpotAngle field value
     */
    public int getTopDeadSpotAngle() {
        return FormatUtils.uint16ToInt(mTopDeadSpotAngle);
    }

    /**
     * Set TopDeadSpotAngle field value by int format
     * and update the related Bluetooth GATT Characteristic.
     *
     * @param value Value to write to TopDeadSpotAngle field
     * @return      True, if the value has been set successfully
     */
    public boolean setTopDeadSpotAngle(int value) {
        if (!FormatUtils.uint16RangeCheck(value)) {
            return false;
        }
        mTopDeadSpotAngle = FormatUtils.intToUint16(value);
        updateGattCharacteristic();
        return true;
    }

    /**
     * Return true if CyclingPowerMeasurement support TopDeadSpotAngle field.
     *
     * @return  True, if CyclingPowerMeasurement support TopDeadSpotAngle field.
     */
    public boolean isSupportTopDeadSpotAngle() {
        return (mFlags.getTopDeadSpotAnglePresent() == 1);
    }

    /**
     * Get BottomDeadSpotAngle field value with int format.
     *
     * @return BottomDeadSpotAngle field value
     */
    public int getBottomDeadSpotAngle() {
        return FormatUtils.uint16ToInt(mBottomDeadSpotAngle);
    }

    /**
     * Set BottomDeadSpotAngle field value by int format
     * and update the related Bluetooth GATT Characteristic.
     *
     * @param value Value to write to BottomDeadSpotAngle field
     * @return      True, if the value has been set successfully
     */
    public boolean setBottomDeadSpotAngle(int value) {
        if (!FormatUtils.uint16RangeCheck(value)) {
            return false;
        }
        mBottomDeadSpotAngle = FormatUtils.intToUint16(value);
        updateGattCharacteristic();
        return true;
    }

    /**
     * Return true if CyclingPowerMeasurement support BottomDeadSpotAngle field.
     *
     * @return  True, if CyclingPowerMeasurement support BottomDeadSpotAngle field.
     */
    public boolean isSupportBottomDeadSpotAngle() {
        return (mFlags.getBottomDeadSpotAnglePresent() == 1);
    }

    /**
     * Get AccumulatedEnergy field value with int format.
     *
     * @return AccumulatedEnergy field value
     */
    public int getAccumulatedEnergy() {
        return FormatUtils.uint16ToInt(mAccumulatedEnergy);
    }

    /**
     * Set AccumulatedEnergy field value by int format
     * and update the related Bluetooth GATT Characteristic.
     *
     * @param value Value to write to AccumulatedEnergy field
     * @return      True, if the value has been set successfully
     */
    public boolean setAccumulatedEnergy(int value) {
        if (!FormatUtils.uint16RangeCheck(value)) {
            return false;
        }
        mAccumulatedEnergy = FormatUtils.intToUint16(value);
        updateGattCharacteristic();
        return true;
    }

    /**
     * Return true if CyclingPowerMeasurement support AccumulatedEnergy field.
     *
     * @return  True, if CyclingPowerMeasurement support AccumulatedEnergy field.
     */
    public boolean isSupportAccumulatedEnergy() {
        return (mFlags.getAccumulatedEnergyPresent() == 1);
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
         * Get PedalPowerBalancePresent bit field value.
         *
         * @return Bit field value
         */
        public int getPedalPowerBalancePresent() {
            return getValue(0, 0);
        }

        /**
         * Set PedalPowerBalancePresent bit field value
         * and update the related Bluetooth GATT Characteristic.
         *
         * @param value Value to write to PedalPowerBalancePresent bit field
         * @return      True, if the value has been set successfully
         */
        public boolean setPedalPowerBalancePresent(int value) {
            if (!setValue(0, 0, value)) {
                return false;
            }
            updateGattCharacteristic();
            return true;
        }

        /**
         * Get PedalPowerBalanceReference bit field value.
         *
         * @return Bit field value
         */
        public int getPedalPowerBalanceReference() {
            return getValue(1, 1);
        }

        /**
         * Set PedalPowerBalanceReference bit field value
         * and update the related Bluetooth GATT Characteristic.
         *
         * @param value Value to write to PedalPowerBalanceReference bit field
         * @return      True, if the value has been set successfully
         */
        public boolean setPedalPowerBalanceReference(int value) {
            if (!setValue(1, 1, value)) {
                return false;
            }
            updateGattCharacteristic();
            return true;
        }

        /**
         * Get AccumulatedTorquePresent bit field value.
         *
         * @return Bit field value
         */
        public int getAccumulatedTorquePresent() {
            return getValue(2, 2);
        }

        /**
         * Set AccumulatedTorquePresent bit field value
         * and update the related Bluetooth GATT Characteristic.
         *
         * @param value Value to write to AccumulatedTorquePresent bit field
         * @return      True, if the value has been set successfully
         */
        public boolean setAccumulatedTorquePresent(int value) {
            if (!setValue(2, 2, value)) {
                return false;
            }
            updateGattCharacteristic();
            return true;
        }

        /**
         * Get AccumulatedTorqueSource bit field value.
         *
         * @return Bit field value
         */
        public int getAccumulatedTorqueSource() {
            return getValue(3, 3);
        }

        /**
         * Set AccumulatedTorqueSource bit field value
         * and update the related Bluetooth GATT Characteristic.
         *
         * @param value Value to write to AccumulatedTorqueSource bit field
         * @return      True, if the value has been set successfully
         */
        public boolean setAccumulatedTorqueSource(int value) {
            if (!setValue(3, 3, value)) {
                return false;
            }
            updateGattCharacteristic();
            return true;
        }

        /**
         * Get WheelRevolutionDataPresent bit field value.
         *
         * @return Bit field value
         */
        public int getWheelRevolutionDataPresent() {
            return getValue(4, 4);
        }

        /**
         * Set WheelRevolutionDataPresent bit field value
         * and update the related Bluetooth GATT Characteristic.
         *
         * @param value Value to write to WheelRevolutionDataPresent bit field
         * @return      True, if the value has been set successfully
         */
        public boolean setWheelRevolutionDataPresent(int value) {
            if (!setValue(4, 4, value)) {
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
            return getValue(5, 5);
        }

        /**
         * Set CrankRevolutionDataPresent bit field value
         * and update the related Bluetooth GATT Characteristic.
         *
         * @param value Value to write to CrankRevolutionDataPresent bit field
         * @return      True, if the value has been set successfully
         */
        public boolean setCrankRevolutionDataPresent(int value) {
            if (!setValue(5, 5, value)) {
                return false;
            }
            updateGattCharacteristic();
            return true;
        }

        /**
         * Get ExtremeForceMagnitudesPresent bit field value.
         *
         * @return Bit field value
         */
        public int getExtremeForceMagnitudesPresent() {
            return getValue(6, 6);
        }

        /**
         * Set ExtremeForceMagnitudesPresent bit field value
         * and update the related Bluetooth GATT Characteristic.
         *
         * @param value Value to write to ExtremeForceMagnitudesPresent bit field
         * @return      True, if the value has been set successfully
         */
        public boolean setExtremeForceMagnitudesPresent(int value) {
            if (!setValue(6, 6, value)) {
                return false;
            }
            updateGattCharacteristic();
            return true;
        }

        /**
         * Get ExtremeTorqueMagnitudesPresent bit field value.
         *
         * @return Bit field value
         */
        public int getExtremeTorqueMagnitudesPresent() {
            return getValue(7, 7);
        }

        /**
         * Set ExtremeTorqueMagnitudesPresent bit field value
         * and update the related Bluetooth GATT Characteristic.
         *
         * @param value Value to write to ExtremeTorqueMagnitudesPresent bit field
         * @return      True, if the value has been set successfully
         */
        public boolean setExtremeTorqueMagnitudesPresent(int value) {
            if (!setValue(7, 7, value)) {
                return false;
            }
            updateGattCharacteristic();
            return true;
        }

        /**
         * Get ExtremeAnglesPresent bit field value.
         *
         * @return Bit field value
         */
        public int getExtremeAnglesPresent() {
            return getValue(8, 8);
        }

        /**
         * Set ExtremeAnglesPresent bit field value
         * and update the related Bluetooth GATT Characteristic.
         *
         * @param value Value to write to ExtremeAnglesPresent bit field
         * @return      True, if the value has been set successfully
         */
        public boolean setExtremeAnglesPresent(int value) {
            if (!setValue(8, 8, value)) {
                return false;
            }
            updateGattCharacteristic();
            return true;
        }

        /**
         * Get TopDeadSpotAnglePresent bit field value.
         *
         * @return Bit field value
         */
        public int getTopDeadSpotAnglePresent() {
            return getValue(9, 9);
        }

        /**
         * Set TopDeadSpotAnglePresent bit field value
         * and update the related Bluetooth GATT Characteristic.
         *
         * @param value Value to write to TopDeadSpotAnglePresent bit field
         * @return      True, if the value has been set successfully
         */
        public boolean setTopDeadSpotAnglePresent(int value) {
            if (!setValue(9, 9, value)) {
                return false;
            }
            updateGattCharacteristic();
            return true;
        }

        /**
         * Get BottomDeadSpotAnglePresent bit field value.
         *
         * @return Bit field value
         */
        public int getBottomDeadSpotAnglePresent() {
            return getValue(10, 10);
        }

        /**
         * Set BottomDeadSpotAnglePresent bit field value
         * and update the related Bluetooth GATT Characteristic.
         *
         * @param value Value to write to BottomDeadSpotAnglePresent bit field
         * @return      True, if the value has been set successfully
         */
        public boolean setBottomDeadSpotAnglePresent(int value) {
            if (!setValue(10, 10, value)) {
                return false;
            }
            updateGattCharacteristic();
            return true;
        }

        /**
         * Get AccumulatedEnergyPresent bit field value.
         *
         * @return Bit field value
         */
        public int getAccumulatedEnergyPresent() {
            return getValue(11, 11);
        }

        /**
         * Set AccumulatedEnergyPresent bit field value
         * and update the related Bluetooth GATT Characteristic.
         *
         * @param value Value to write to AccumulatedEnergyPresent bit field
         * @return      True, if the value has been set successfully
         */
        public boolean setAccumulatedEnergyPresent(int value) {
            if (!setValue(11, 11, value)) {
                return false;
            }
            updateGattCharacteristic();
            return true;
        }

        /**
         * Get OffsetCompensationIndicator bit field value.
         *
         * @return Bit field value
         */
        public int getOffsetCompensationIndicator() {
            return getValue(12, 12);
        }

        /**
         * Set OffsetCompensationIndicator bit field value
         * and update the related Bluetooth GATT Characteristic.
         *
         * @param value Value to write to OffsetCompensationIndicator bit field
         * @return      True, if the value has been set successfully
         */
        public boolean setOffsetCompensationIndicator(int value) {
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

