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


package com.mediatek.bluetoothgatt.profile;

// Customized Start: Import ........................................................................
import android.bluetooth.BluetoothGatt;
import android.content.Context;
import android.util.Log;

import com.mediatek.bluetoothgatt.GattUuid;
import com.mediatek.bluetoothgatt.characteristic.CharacteristicBase;
//........................................................................ Customized End: Import //

/**
 * Public API for the GATT Cycling Power Profile(Client).
 *
 * Name: Cycling Power
 * Type: org.bluetooth.profile.cycling_power
 * Last Modified: None
 * Revision: None
 * Role: Collector
 */
public class CppCollector extends ClientBase {
    private static final boolean DBG = true;
    private static final String TAG = "CppCollector";

    /**
     * Create a CppCollector object and init value.
     *
     * @param context App context
     */
    public CppCollector(Context context) {
        super(context);

        if (DBG) {
            Log.d(TAG, "CppCollector()");
        }
    }

    @Override
    protected boolean handleServicesDiscovered(BluetoothGatt gatt, int status) {
        // Check mandatory service is exist.
        return true &&
                (gatt.getService(GattUuid.SRVC_CPS) != null);
    }

    /**
     * Reads Cps:CyclingPowerFeature characteristic
     * from the associated remote device.
     *
     * @return true, if the read operation was initiated successfully
     */
    public boolean readCpsCyclingPowerFeature() {
        if (DBG) {
            Log.d(TAG, "readCpsCyclingPowerFeature()");
        }
        return readCharacteristic(
                GattUuid.SRVC_CPS,
                GattUuid.CHAR_CYCLING_POWER_FEATURE);
    }

    /**
     * Reads Cps:SensorLocation characteristic
     * from the associated remote device.
     *
     * @return true, if the read operation was initiated successfully
     */
    public boolean readCpsSensorLocation() {
        if (DBG) {
            Log.d(TAG, "readCpsSensorLocation()");
        }
        return readCharacteristic(
                GattUuid.SRVC_CPS,
                GattUuid.CHAR_SENSOR_LOCATION);
    }

    /**
     * Reads Dis:ManufacturerNameString characteristic
     * from the associated remote device.
     *
     * @return true, if the read operation was initiated successfully
     */
    public boolean readDisManufacturerNameString() {
        if (DBG) {
            Log.d(TAG, "readDisManufacturerNameString()");
        }
        return readCharacteristic(
                GattUuid.SRVC_DIS,
                GattUuid.CHAR_MANUFACTURER_NAME_STRING);
    }

    /**
     * Reads Dis:ModelNumberString characteristic
     * from the associated remote device.
     *
     * @return true, if the read operation was initiated successfully
     */
    public boolean readDisModelNumberString() {
        if (DBG) {
            Log.d(TAG, "readDisModelNumberString()");
        }
        return readCharacteristic(
                GattUuid.SRVC_DIS,
                GattUuid.CHAR_MODEL_NUMBER_STRING);
    }

    /**
     * Reads Dis:SerialNumberString characteristic
     * from the associated remote device.
     *
     * @return true, if the read operation was initiated successfully
     */
    public boolean readDisSerialNumberString() {
        if (DBG) {
            Log.d(TAG, "readDisSerialNumberString()");
        }
        return readCharacteristic(
                GattUuid.SRVC_DIS,
                GattUuid.CHAR_SERIAL_NUMBER_STRING);
    }

    /**
     * Reads Dis:HardwareRevisionString characteristic
     * from the associated remote device.
     *
     * @return true, if the read operation was initiated successfully
     */
    public boolean readDisHardwareRevisionString() {
        if (DBG) {
            Log.d(TAG, "readDisHardwareRevisionString()");
        }
        return readCharacteristic(
                GattUuid.SRVC_DIS,
                GattUuid.CHAR_HARDWARE_REVISION_STRING);
    }

    /**
     * Reads Dis:FirmwareRevisionString characteristic
     * from the associated remote device.
     *
     * @return true, if the read operation was initiated successfully
     */
    public boolean readDisFirmwareRevisionString() {
        if (DBG) {
            Log.d(TAG, "readDisFirmwareRevisionString()");
        }
        return readCharacteristic(
                GattUuid.SRVC_DIS,
                GattUuid.CHAR_FIRMWARE_REVISION_STRING);
    }

    /**
     * Reads Dis:SoftwareRevisionString characteristic
     * from the associated remote device.
     *
     * @return true, if the read operation was initiated successfully
     */
    public boolean readDisSoftwareRevisionString() {
        if (DBG) {
            Log.d(TAG, "readDisSoftwareRevisionString()");
        }
        return readCharacteristic(
                GattUuid.SRVC_DIS,
                GattUuid.CHAR_SOFTWARE_REVISION_STRING);
    }

    /**
     * Reads Dis:SystemId characteristic
     * from the associated remote device.
     *
     * @return true, if the read operation was initiated successfully
     */
    public boolean readDisSystemId() {
        if (DBG) {
            Log.d(TAG, "readDisSystemId()");
        }
        return readCharacteristic(
                GattUuid.SRVC_DIS,
                GattUuid.CHAR_SYSTEM_ID);
    }

    /**
     * Reads Dis:RegCertDataList characteristic
     * from the associated remote device.
     *
     * @return true, if the read operation was initiated successfully
     */
    public boolean readDisRegCertDataList() {
        if (DBG) {
            Log.d(TAG, "readDisRegCertDataList()");
        }
        return readCharacteristic(
                GattUuid.SRVC_DIS,
                GattUuid.CHAR_REG_CERT_DATA_LIST);
    }

    /**
     * Reads Dis:PnpId characteristic
     * from the associated remote device.
     *
     * @return true, if the read operation was initiated successfully
     */
    public boolean readDisPnpId() {
        if (DBG) {
            Log.d(TAG, "readDisPnpId()");
        }
        return readCharacteristic(
                GattUuid.SRVC_DIS,
                GattUuid.CHAR_PNP_ID);
    }

    /**
     * Reads Bas:BatteryLevel characteristic
     * from the associated remote device.
     *
     * @return true, if the read operation was initiated successfully
     */
    public boolean readBasBatteryLevel() {
        if (DBG) {
            Log.d(TAG, "readBasBatteryLevel()");
        }
        return readCharacteristic(
                GattUuid.SRVC_BAS,
                GattUuid.CHAR_BATTERY_LEVEL);
    }


    /**
     * Writes Cps:CyclingPowerControlPoint characteristic and its values
     * to the associated remote device.
     *
     * @param characteristic Characteristic to write on the remote device
     * @return true, if the write operation was initiated successfully
     */
    public boolean writeCpsCyclingPowerControlPoint(CharacteristicBase characteristic) {
        if (DBG) {
            Log.d(TAG, "writeCpsCyclingPowerControlPoint()");
        }
        return writeCharacteristic(
                GattUuid.SRVC_CPS,
                characteristic);
    }


    /**
     * Reads Cps:CyclingPowerMeasurement:Cccd descriptor
     * from the associated remote device.
     *
     * @return true, if the read operation was initiated successfully
     */
    public boolean readCpsCyclingPowerMeasurementCccd() {
        if (DBG) {
            Log.d(TAG, "readCpsCyclingPowerMeasurementCccd()");
        }
        return readDescriptor(
                GattUuid.SRVC_CPS,
                GattUuid.CHAR_CYCLING_POWER_MEASUREMENT,
                GattUuid.DESCR_CLIENT_CHARACTERISTIC_CONFIGURATION);
    }

    /**
     * Reads Cps:CyclingPowerMeasurement:Sccd descriptor
     * from the associated remote device.
     *
     * @return true, if the read operation was initiated successfully
     */
    public boolean readCpsCyclingPowerMeasurementSccd() {
        if (DBG) {
            Log.d(TAG, "readCpsCyclingPowerMeasurementSccd()");
        }
        return readDescriptor(
                GattUuid.SRVC_CPS,
                GattUuid.CHAR_CYCLING_POWER_MEASUREMENT,
                GattUuid.DESCR_SERVER_CHARACTERISTIC_CONFIGURATION);
    }

    /**
     * Reads Cps:CyclingPowerVector:Cccd descriptor
     * from the associated remote device.
     *
     * @return true, if the read operation was initiated successfully
     */
    public boolean readCpsCyclingPowerVectorCccd() {
        if (DBG) {
            Log.d(TAG, "readCpsCyclingPowerVectorCccd()");
        }
        return readDescriptor(
                GattUuid.SRVC_CPS,
                GattUuid.CHAR_CYCLING_POWER_VECTOR,
                GattUuid.DESCR_CLIENT_CHARACTERISTIC_CONFIGURATION);
    }

    /**
     * Reads Cps:CyclingPowerControlPoint:Cccd descriptor
     * from the associated remote device.
     *
     * @return true, if the read operation was initiated successfully
     */
    public boolean readCpsCyclingPowerControlPointCccd() {
        if (DBG) {
            Log.d(TAG, "readCpsCyclingPowerControlPointCccd()");
        }
        return readDescriptor(
                GattUuid.SRVC_CPS,
                GattUuid.CHAR_CYCLING_POWER_CONTROL_POINT,
                GattUuid.DESCR_CLIENT_CHARACTERISTIC_CONFIGURATION);
    }

    /**
     * Reads Bas:BatteryLevel:Cpfd descriptor
     * from the associated remote device.
     *
     * @return true, if the read operation was initiated successfully
     */
    public boolean readBasBatteryLevelCpfd() {
        if (DBG) {
            Log.d(TAG, "readBasBatteryLevelCpfd()");
        }
        return readDescriptor(
                GattUuid.SRVC_BAS,
                GattUuid.CHAR_BATTERY_LEVEL,
                GattUuid.DESCR_CHARACTERISTIC_PRESENTATION_FORMAT);
    }

    /**
     * Reads Bas:BatteryLevel:Cccd descriptor
     * from the associated remote device.
     *
     * @return true, if the read operation was initiated successfully
     */
    public boolean readBasBatteryLevelCccd() {
        if (DBG) {
            Log.d(TAG, "readBasBatteryLevelCccd()");
        }
        return readDescriptor(
                GattUuid.SRVC_BAS,
                GattUuid.CHAR_BATTERY_LEVEL,
                GattUuid.DESCR_CLIENT_CHARACTERISTIC_CONFIGURATION);
    }


    /**
     * Writes Cps:CyclingPowerMeasurement:Cccd descriptor and
     * its values to the associated remote device.
     *
     * @param value Descriptor value to write on the remote device
     * @return true, if the write operation was initiated successfully
     */
    public boolean writeCpsCyclingPowerMeasurementCccd(byte[] value) {
        if (DBG) {
            Log.d(TAG, "writeCpsCyclingPowerMeasurementCccd()");
        }
        return writeDescriptor(
                GattUuid.SRVC_CPS,
                GattUuid.CHAR_CYCLING_POWER_MEASUREMENT,
                GattUuid.DESCR_CLIENT_CHARACTERISTIC_CONFIGURATION,
                value);
    }

    /**
     * Writes Cps:CyclingPowerMeasurement:Sccd descriptor and
     * its values to the associated remote device.
     *
     * @param value Descriptor value to write on the remote device
     * @return true, if the write operation was initiated successfully
     */
    public boolean writeCpsCyclingPowerMeasurementSccd(byte[] value) {
        if (DBG) {
            Log.d(TAG, "writeCpsCyclingPowerMeasurementSccd()");
        }
        return writeDescriptor(
                GattUuid.SRVC_CPS,
                GattUuid.CHAR_CYCLING_POWER_MEASUREMENT,
                GattUuid.DESCR_SERVER_CHARACTERISTIC_CONFIGURATION,
                value);
    }

    /**
     * Writes Cps:CyclingPowerVector:Cccd descriptor and
     * its values to the associated remote device.
     *
     * @param value Descriptor value to write on the remote device
     * @return true, if the write operation was initiated successfully
     */
    public boolean writeCpsCyclingPowerVectorCccd(byte[] value) {
        if (DBG) {
            Log.d(TAG, "writeCpsCyclingPowerVectorCccd()");
        }
        return writeDescriptor(
                GattUuid.SRVC_CPS,
                GattUuid.CHAR_CYCLING_POWER_VECTOR,
                GattUuid.DESCR_CLIENT_CHARACTERISTIC_CONFIGURATION,
                value);
    }

    /**
     * Writes Cps:CyclingPowerControlPoint:Cccd descriptor and
     * its values to the associated remote device.
     *
     * @param value Descriptor value to write on the remote device
     * @return true, if the write operation was initiated successfully
     */
    public boolean writeCpsCyclingPowerControlPointCccd(byte[] value) {
        if (DBG) {
            Log.d(TAG, "writeCpsCyclingPowerControlPointCccd()");
        }
        return writeDescriptor(
                GattUuid.SRVC_CPS,
                GattUuid.CHAR_CYCLING_POWER_CONTROL_POINT,
                GattUuid.DESCR_CLIENT_CHARACTERISTIC_CONFIGURATION,
                value);
    }

    /**
     * Writes Bas:BatteryLevel:Cccd descriptor and
     * its values to the associated remote device.
     *
     * @param value Descriptor value to write on the remote device
     * @return true, if the write operation was initiated successfully
     */
    public boolean writeBasBatteryLevelCccd(byte[] value) {
        if (DBG) {
            Log.d(TAG, "writeBasBatteryLevelCccd()");
        }
        return writeDescriptor(
                GattUuid.SRVC_BAS,
                GattUuid.CHAR_BATTERY_LEVEL,
                GattUuid.DESCR_CLIENT_CHARACTERISTIC_CONFIGURATION,
                value);
    }

}


