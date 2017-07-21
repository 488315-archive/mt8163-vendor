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
 * Public API for the Position Quality Bluetooth GATT Characteristic.
 *
 * <p>This class provides Position Quality Bluetooth GATT Characteristic
 * value encode/decode functions. Allowing applications easy and quick to
 * read/write characteristic field value.
 *
 * <p>Name: Position Quality
 * Type: org.bluetooth.characteristic.position_quality
 * UUID: 2A69
 * Last Modified: 2013-05-13
 * Revision: None
 */
public class PositionQuality extends CharacteristicBase {
    /**
     * Position Quality UUID.
     */
    public static final UUID GATT_UUID =
            UUID.fromString(FormatUtils.uuid16ToUuid128("2A69"));

    // Customized Start: Constant Definition .......................................................

    //....................................................... Customized End: Constant Definition //

    /*
     * Field: Flags
     * Requirement: Mandatory
     * Format: bit16
     */
    private BfFlags mFlags = new BfFlags();

    /*
     * Field: Number of Beacons in Solution
     * Requirement: Optional
     * Format: uint8
     * Unit: org.bluetooth.unit.unitless
     */
    private byte[] mNumberOfBeaconsInSolution = new byte[FormatUtils.UINT8_SIZE];

    /*
     * Field: Number of Beacons in View
     * Requirement: Optional
     * Format: uint8
     * Unit: org.bluetooth.unit.unitless
     */
    private byte[] mNumberOfBeaconsInView = new byte[FormatUtils.UINT8_SIZE];

    /*
     * Field: Time to First Fix
     * Requirement: Optional
     * Format: uint16
     * Unit: org.bluetooth.unit.time.second
     */
    private byte[] mTimeToFirstFix = new byte[FormatUtils.UINT16_SIZE];

    /*
     * Field: EHPE
     * Requirement: Optional
     * Format: uint32
     * Unit: org.bluetooth.unit.length.meter
     */
    private byte[] mEhpe = new byte[FormatUtils.UINT32_SIZE];

    /*
     * Field: EVPE
     * Requirement: Optional
     * Format: uint32
     * Unit: org.bluetooth.unit.length.meter
     */
    private byte[] mEvpe = new byte[FormatUtils.UINT32_SIZE];

    /*
     * Field: HDOP
     * Requirement: Optional
     * Format: uint8
     * Unit: org.bluetooth.unit.unitless
     */
    private byte[] mHdop = new byte[FormatUtils.UINT8_SIZE];

    /*
     * Field: VDOP
     * Requirement: Optional
     * Format: uint8
     * Unit: org.bluetooth.unit.unitless
     */
    private byte[] mVdop = new byte[FormatUtils.UINT8_SIZE];


    /**
     * Create a PositionQuality characteristic object.
     */
    public PositionQuality() {
        setCharacteristic(null);
        setFlags(new BfFlags());
        setNumberOfBeaconsInSolution(0);
        setNumberOfBeaconsInView(0);
        setTimeToFirstFix(0);
        setEhpe(0);
        setEvpe(0);
        setHdop(0);
        setVdop(0);
    }

    /**
     * Create a PositionQuality characteristic object and init value.
     *
     * @param value Initial value
     */
    public PositionQuality(byte[] value) {
        setCharacteristic(null);
        setValue(value);
    }

    /**
     * Create a PositionQuality characteristic object and init value.
     *
     * @param value Initial value
     * @param characteristic the gatt-characteristic this characteristic belongs to.
     */
    public PositionQuality(byte[] value, BluetoothGattCharacteristic characteristic) {
        setCharacteristic(characteristic);
        setValue(value);
    }

    /**
     * Create a PositionQuality characteristic object.
     *
     * @param flags Flags
     * @param numberOfBeaconsInSolution Number of Beacons in Solution
     * @param numberOfBeaconsInView Number of Beacons in View
     * @param timeToFirstFix Time to First Fix
     * @param ehpe EHPE
     * @param evpe EVPE
     * @param hdop HDOP
     * @param vdop VDOP
     */
    public PositionQuality(
            BfFlags flags,
            int numberOfBeaconsInSolution,
            int numberOfBeaconsInView,
            int timeToFirstFix,
            long ehpe,
            long evpe,
            int hdop,
            int vdop) {
        setCharacteristic(null);
        setFlags(flags);
        setNumberOfBeaconsInSolution(numberOfBeaconsInSolution);
        setNumberOfBeaconsInView(numberOfBeaconsInView);
        setTimeToFirstFix(timeToFirstFix);
        setEhpe(ehpe);
        setEvpe(evpe);
        setHdop(hdop);
        setVdop(vdop);
    }

    /**
     * Create a PositionQuality characteristic object.
     *
     * @param flags Flags
     * @param numberOfBeaconsInSolution Number of Beacons in Solution
     * @param numberOfBeaconsInView Number of Beacons in View
     * @param timeToFirstFix Time to First Fix
     * @param ehpe EHPE
     * @param evpe EVPE
     * @param hdop HDOP
     * @param vdop VDOP
     * @param characteristic the gatt-characteristic this characteristic belongs to.
     */
    public PositionQuality(
            BfFlags flags,
            int numberOfBeaconsInSolution,
            int numberOfBeaconsInView,
            int timeToFirstFix,
            long ehpe,
            long evpe,
            int hdop,
            int vdop,
            BluetoothGattCharacteristic characteristic) {
        setCharacteristic(characteristic);
        setFlags(flags);
        setNumberOfBeaconsInSolution(numberOfBeaconsInSolution);
        setNumberOfBeaconsInView(numberOfBeaconsInView);
        setTimeToFirstFix(timeToFirstFix);
        setEhpe(ehpe);
        setEvpe(evpe);
        setHdop(hdop);
        setVdop(vdop);
    }

    // Customized Start: Constructors ..............................................................

    //.............................................................. Customized End: Constructors //

    /**
     * Get PositionQuality characteristic byte length.
     *
     * @return Byte length of this characteristic
     */
    public int length() {
        return (isSupportFlags()
                        ? mFlags.length() : 0)
                + (isSupportNumberOfBeaconsInSolution()
                        ? mNumberOfBeaconsInSolution.length : 0)
                + (isSupportNumberOfBeaconsInView()
                        ? mNumberOfBeaconsInView.length : 0)
                + (isSupportTimeToFirstFix()
                        ? mTimeToFirstFix.length : 0)
                + (isSupportEhpe()
                        ? mEhpe.length : 0)
                + (isSupportEvpe()
                        ? mEvpe.length : 0)
                + (isSupportHdop()
                        ? mHdop.length : 0)
                + (isSupportVdop()
                        ? mVdop.length : 0);
    }

    /**
     * Get PositionQuality characteristic UUID.
     *
     * @return Characteristic UUID
     */
    @Override
    public UUID getUuid() {
        return GATT_UUID;
    }

    /**
     * Get PositionQuality characteristic value.
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

        if (isSupportNumberOfBeaconsInSolution()) {
            int fieldLen = mNumberOfBeaconsInSolution.length;
            System.arraycopy(mNumberOfBeaconsInSolution, 0, value, dstPos, fieldLen);
            dstPos += fieldLen;
        }

        if (isSupportNumberOfBeaconsInView()) {
            int fieldLen = mNumberOfBeaconsInView.length;
            System.arraycopy(mNumberOfBeaconsInView, 0, value, dstPos, fieldLen);
            dstPos += fieldLen;
        }

        if (isSupportTimeToFirstFix()) {
            int fieldLen = mTimeToFirstFix.length;
            System.arraycopy(mTimeToFirstFix, 0, value, dstPos, fieldLen);
            dstPos += fieldLen;
        }

        if (isSupportEhpe()) {
            int fieldLen = mEhpe.length;
            System.arraycopy(mEhpe, 0, value, dstPos, fieldLen);
            dstPos += fieldLen;
        }

        if (isSupportEvpe()) {
            int fieldLen = mEvpe.length;
            System.arraycopy(mEvpe, 0, value, dstPos, fieldLen);
            dstPos += fieldLen;
        }

        if (isSupportHdop()) {
            int fieldLen = mHdop.length;
            System.arraycopy(mHdop, 0, value, dstPos, fieldLen);
            dstPos += fieldLen;
        }

        if (isSupportVdop()) {
            int fieldLen = mVdop.length;
            System.arraycopy(mVdop, 0, value, dstPos, fieldLen);
            dstPos += fieldLen;
        }

        return value;
    }

    /**
     * Set PositionQuality characteristic value.
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

        if (isSupportNumberOfBeaconsInSolution()) {
            int fieldLen = mNumberOfBeaconsInSolution.length;

            if (!setValueRangeCheck(value.length, srcPos, fieldLen)) {
                return false;
            }

            System.arraycopy(value, srcPos, mNumberOfBeaconsInSolution, 0, fieldLen);
            srcPos += fieldLen;
        }

        if (isSupportNumberOfBeaconsInView()) {
            int fieldLen = mNumberOfBeaconsInView.length;

            if (!setValueRangeCheck(value.length, srcPos, fieldLen)) {
                return false;
            }

            System.arraycopy(value, srcPos, mNumberOfBeaconsInView, 0, fieldLen);
            srcPos += fieldLen;
        }

        if (isSupportTimeToFirstFix()) {
            int fieldLen = mTimeToFirstFix.length;

            if (!setValueRangeCheck(value.length, srcPos, fieldLen)) {
                return false;
            }

            System.arraycopy(value, srcPos, mTimeToFirstFix, 0, fieldLen);
            srcPos += fieldLen;
        }

        if (isSupportEhpe()) {
            int fieldLen = mEhpe.length;

            if (!setValueRangeCheck(value.length, srcPos, fieldLen)) {
                return false;
            }

            System.arraycopy(value, srcPos, mEhpe, 0, fieldLen);
            srcPos += fieldLen;
        }

        if (isSupportEvpe()) {
            int fieldLen = mEvpe.length;

            if (!setValueRangeCheck(value.length, srcPos, fieldLen)) {
                return false;
            }

            System.arraycopy(value, srcPos, mEvpe, 0, fieldLen);
            srcPos += fieldLen;
        }

        if (isSupportHdop()) {
            int fieldLen = mHdop.length;

            if (!setValueRangeCheck(value.length, srcPos, fieldLen)) {
                return false;
            }

            System.arraycopy(value, srcPos, mHdop, 0, fieldLen);
            srcPos += fieldLen;
        }

        if (isSupportVdop()) {
            int fieldLen = mVdop.length;

            if (!setValueRangeCheck(value.length, srcPos, fieldLen)) {
                return false;
            }

            System.arraycopy(value, srcPos, mVdop, 0, fieldLen);
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
     * Return true if PositionQuality support Flags field.
     *
     * @return  True, if PositionQuality support Flags field.
     */
    public boolean isSupportFlags() {
        return true;
    }

    /**
     * Get NumberOfBeaconsInSolution field value with int format.
     *
     * @return NumberOfBeaconsInSolution field value
     */
    public int getNumberOfBeaconsInSolution() {
        return FormatUtils.uint8ToInt(mNumberOfBeaconsInSolution);
    }

    /**
     * Set NumberOfBeaconsInSolution field value by int format
     * and update the related Bluetooth GATT Characteristic.
     *
     * @param value Value to write to NumberOfBeaconsInSolution field
     * @return      True, if the value has been set successfully
     */
    public boolean setNumberOfBeaconsInSolution(int value) {
        if (!FormatUtils.uint8RangeCheck(value)) {
            return false;
        }
        mNumberOfBeaconsInSolution = FormatUtils.intToUint8(value);
        updateGattCharacteristic();
        return true;
    }

    /**
     * Return true if PositionQuality support NumberOfBeaconsInSolution field.
     *
     * @return  True, if PositionQuality support NumberOfBeaconsInSolution field.
     */
    public boolean isSupportNumberOfBeaconsInSolution() {
        return (mFlags.getNumberOfBeaconsInSolutionPresent() == 1);
    }

    /**
     * Get NumberOfBeaconsInView field value with int format.
     *
     * @return NumberOfBeaconsInView field value
     */
    public int getNumberOfBeaconsInView() {
        return FormatUtils.uint8ToInt(mNumberOfBeaconsInView);
    }

    /**
     * Set NumberOfBeaconsInView field value by int format
     * and update the related Bluetooth GATT Characteristic.
     *
     * @param value Value to write to NumberOfBeaconsInView field
     * @return      True, if the value has been set successfully
     */
    public boolean setNumberOfBeaconsInView(int value) {
        if (!FormatUtils.uint8RangeCheck(value)) {
            return false;
        }
        mNumberOfBeaconsInView = FormatUtils.intToUint8(value);
        updateGattCharacteristic();
        return true;
    }

    /**
     * Return true if PositionQuality support NumberOfBeaconsInView field.
     *
     * @return  True, if PositionQuality support NumberOfBeaconsInView field.
     */
    public boolean isSupportNumberOfBeaconsInView() {
        return (mFlags.getNumberOfBeaconsInViewPresent() == 1);
    }

    /**
     * Get TimeToFirstFix field value with int format.
     *
     * @return TimeToFirstFix field value
     */
    public int getTimeToFirstFix() {
        return FormatUtils.uint16ToInt(mTimeToFirstFix);
    }

    /**
     * Set TimeToFirstFix field value by int format
     * and update the related Bluetooth GATT Characteristic.
     *
     * @param value Value to write to TimeToFirstFix field
     * @return      True, if the value has been set successfully
     */
    public boolean setTimeToFirstFix(int value) {
        if (!FormatUtils.uint16RangeCheck(value)) {
            return false;
        }
        mTimeToFirstFix = FormatUtils.intToUint16(value);
        updateGattCharacteristic();
        return true;
    }

    /**
     * Return true if PositionQuality support TimeToFirstFix field.
     *
     * @return  True, if PositionQuality support TimeToFirstFix field.
     */
    public boolean isSupportTimeToFirstFix() {
        return (mFlags.getTimeToFirstFixPresent() == 1);
    }

    /**
     * Get Ehpe field value with long format.
     *
     * @return Ehpe field value
     */
    public long getEhpe() {
        return FormatUtils.uint32ToLong(mEhpe);
    }

    /**
     * Set Ehpe field value by long format
     * and update the related Bluetooth GATT Characteristic.
     *
     * @param value Value to write to Ehpe field
     * @return      True, if the value has been set successfully
     */
    public boolean setEhpe(long value) {
        if (!FormatUtils.uint32RangeCheck(value)) {
            return false;
        }
        mEhpe = FormatUtils.longToUint32(value);
        updateGattCharacteristic();
        return true;
    }

    /**
     * Return true if PositionQuality support Ehpe field.
     *
     * @return  True, if PositionQuality support Ehpe field.
     */
    public boolean isSupportEhpe() {
        return (mFlags.getEhpePresent() == 1);
    }

    /**
     * Get Evpe field value with long format.
     *
     * @return Evpe field value
     */
    public long getEvpe() {
        return FormatUtils.uint32ToLong(mEvpe);
    }

    /**
     * Set Evpe field value by long format
     * and update the related Bluetooth GATT Characteristic.
     *
     * @param value Value to write to Evpe field
     * @return      True, if the value has been set successfully
     */
    public boolean setEvpe(long value) {
        if (!FormatUtils.uint32RangeCheck(value)) {
            return false;
        }
        mEvpe = FormatUtils.longToUint32(value);
        updateGattCharacteristic();
        return true;
    }

    /**
     * Return true if PositionQuality support Evpe field.
     *
     * @return  True, if PositionQuality support Evpe field.
     */
    public boolean isSupportEvpe() {
        return (mFlags.getEvpePresent() == 1);
    }

    /**
     * Get Hdop field value with int format.
     *
     * @return Hdop field value
     */
    public int getHdop() {
        return FormatUtils.uint8ToInt(mHdop);
    }

    /**
     * Set Hdop field value by int format
     * and update the related Bluetooth GATT Characteristic.
     *
     * @param value Value to write to Hdop field
     * @return      True, if the value has been set successfully
     */
    public boolean setHdop(int value) {
        if (!FormatUtils.uint8RangeCheck(value)) {
            return false;
        }
        mHdop = FormatUtils.intToUint8(value);
        updateGattCharacteristic();
        return true;
    }

    /**
     * Return true if PositionQuality support Hdop field.
     *
     * @return  True, if PositionQuality support Hdop field.
     */
    public boolean isSupportHdop() {
        return (mFlags.getHdopPresent() == 1);
    }

    /**
     * Get Vdop field value with int format.
     *
     * @return Vdop field value
     */
    public int getVdop() {
        return FormatUtils.uint8ToInt(mVdop);
    }

    /**
     * Set Vdop field value by int format
     * and update the related Bluetooth GATT Characteristic.
     *
     * @param value Value to write to Vdop field
     * @return      True, if the value has been set successfully
     */
    public boolean setVdop(int value) {
        if (!FormatUtils.uint8RangeCheck(value)) {
            return false;
        }
        mVdop = FormatUtils.intToUint8(value);
        updateGattCharacteristic();
        return true;
    }

    /**
     * Return true if PositionQuality support Vdop field.
     *
     * @return  True, if PositionQuality support Vdop field.
     */
    public boolean isSupportVdop() {
        return (mFlags.getVdopPresent() == 1);
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
         * Get NumberOfBeaconsInSolutionPresent bit field value.
         *
         * @return Bit field value
         */
        public int getNumberOfBeaconsInSolutionPresent() {
            return getValue(0, 0);
        }

        /**
         * Set NumberOfBeaconsInSolutionPresent bit field value
         * and update the related Bluetooth GATT Characteristic.
         *
         * @param value Value to write to NumberOfBeaconsInSolutionPresent bit field
         * @return      True, if the value has been set successfully
         */
        public boolean setNumberOfBeaconsInSolutionPresent(int value) {
            if (!setValue(0, 0, value)) {
                return false;
            }
            updateGattCharacteristic();
            return true;
        }

        /**
         * Get NumberOfBeaconsInViewPresent bit field value.
         *
         * @return Bit field value
         */
        public int getNumberOfBeaconsInViewPresent() {
            return getValue(1, 1);
        }

        /**
         * Set NumberOfBeaconsInViewPresent bit field value
         * and update the related Bluetooth GATT Characteristic.
         *
         * @param value Value to write to NumberOfBeaconsInViewPresent bit field
         * @return      True, if the value has been set successfully
         */
        public boolean setNumberOfBeaconsInViewPresent(int value) {
            if (!setValue(1, 1, value)) {
                return false;
            }
            updateGattCharacteristic();
            return true;
        }

        /**
         * Get TimeToFirstFixPresent bit field value.
         *
         * @return Bit field value
         */
        public int getTimeToFirstFixPresent() {
            return getValue(2, 2);
        }

        /**
         * Set TimeToFirstFixPresent bit field value
         * and update the related Bluetooth GATT Characteristic.
         *
         * @param value Value to write to TimeToFirstFixPresent bit field
         * @return      True, if the value has been set successfully
         */
        public boolean setTimeToFirstFixPresent(int value) {
            if (!setValue(2, 2, value)) {
                return false;
            }
            updateGattCharacteristic();
            return true;
        }

        /**
         * Get EhpePresent bit field value.
         *
         * @return Bit field value
         */
        public int getEhpePresent() {
            return getValue(3, 3);
        }

        /**
         * Set EhpePresent bit field value
         * and update the related Bluetooth GATT Characteristic.
         *
         * @param value Value to write to EhpePresent bit field
         * @return      True, if the value has been set successfully
         */
        public boolean setEhpePresent(int value) {
            if (!setValue(3, 3, value)) {
                return false;
            }
            updateGattCharacteristic();
            return true;
        }

        /**
         * Get EvpePresent bit field value.
         *
         * @return Bit field value
         */
        public int getEvpePresent() {
            return getValue(4, 4);
        }

        /**
         * Set EvpePresent bit field value
         * and update the related Bluetooth GATT Characteristic.
         *
         * @param value Value to write to EvpePresent bit field
         * @return      True, if the value has been set successfully
         */
        public boolean setEvpePresent(int value) {
            if (!setValue(4, 4, value)) {
                return false;
            }
            updateGattCharacteristic();
            return true;
        }

        /**
         * Get HdopPresent bit field value.
         *
         * @return Bit field value
         */
        public int getHdopPresent() {
            return getValue(5, 5);
        }

        /**
         * Set HdopPresent bit field value
         * and update the related Bluetooth GATT Characteristic.
         *
         * @param value Value to write to HdopPresent bit field
         * @return      True, if the value has been set successfully
         */
        public boolean setHdopPresent(int value) {
            if (!setValue(5, 5, value)) {
                return false;
            }
            updateGattCharacteristic();
            return true;
        }

        /**
         * Get VdopPresent bit field value.
         *
         * @return Bit field value
         */
        public int getVdopPresent() {
            return getValue(6, 6);
        }

        /**
         * Set VdopPresent bit field value
         * and update the related Bluetooth GATT Characteristic.
         *
         * @param value Value to write to VdopPresent bit field
         * @return      True, if the value has been set successfully
         */
        public boolean setVdopPresent(int value) {
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
    }

    // Customized Start: Functions .................................................................

    //................................................................. Customized End: Functions //
}

