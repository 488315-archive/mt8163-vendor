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
import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothGatt;
import android.bluetooth.BluetoothGattCharacteristic;
import android.bluetooth.BluetoothGattDescriptor;
import android.util.Log;

import com.mediatek.bluetoothgatt.GattUuid;
import com.mediatek.bluetoothgatt.characteristic.BatteryLevel;
import com.mediatek.bluetoothgatt.characteristic.CharacteristicBase;
import com.mediatek.bluetoothgatt.characteristic.FirmwareRevisionString;
import com.mediatek.bluetoothgatt.characteristic.HardwareRevisionString;
import com.mediatek.bluetoothgatt.characteristic.LnControlPoint;
import com.mediatek.bluetoothgatt.characteristic.LnFeature;
import com.mediatek.bluetoothgatt.characteristic.ManufacturerNameString;
import com.mediatek.bluetoothgatt.characteristic.ModelNumberString;
import com.mediatek.bluetoothgatt.characteristic.PnpId;
import com.mediatek.bluetoothgatt.characteristic.PositionQuality;
import com.mediatek.bluetoothgatt.characteristic.RegCertDataList;
import com.mediatek.bluetoothgatt.characteristic.SerialNumberString;
import com.mediatek.bluetoothgatt.characteristic.SoftwareRevisionString;
import com.mediatek.bluetoothgatt.characteristic.SystemId;

import java.util.UUID;
//........................................................................ Customized End: Import //

/**
 * This class is used to implement {@link LnpLnSensor} callbacks.
 */
public class LnpLnSensorCallback extends ServerBaseCallback {
    private static final boolean DBG = true;
    private static final String TAG = "LnpLnSensorCallback";

    @Override
    void dispatchCharacteristicReadRequest(BluetoothDevice device, int requestId, int offset,
            BluetoothGattCharacteristic characteristic) {
        final UUID charUuid = characteristic.getUuid();
        final UUID srvcUuid = characteristic.getService().getUuid();
        CharacteristicBase base;

        if (srvcUuid.equals(GattUuid.SRVC_LNS)) {
             if (charUuid.equals(GattUuid.CHAR_LN_FEATURE)) {
                base = new LnFeature(characteristic.getValue(), characteristic);
                this.onLnsLnFeatureReadRequest(
                        device, requestId, offset, (LnFeature) base);
                return;
            } else if (charUuid.equals(GattUuid.CHAR_POSITION_QUALITY)) {
                base = new PositionQuality(characteristic.getValue(), characteristic);
                this.onLnsPositionQualityReadRequest(
                        device, requestId, offset, (PositionQuality) base);
                return;
            }
        }
        if (srvcUuid.equals(GattUuid.SRVC_DIS)) {
             if (charUuid.equals(GattUuid.CHAR_MANUFACTURER_NAME_STRING)) {
                base = new ManufacturerNameString(characteristic.getValue(), characteristic);
                this.onDisManufacturerNameStringReadRequest(
                        device, requestId, offset, (ManufacturerNameString) base);
                return;
            } else if (charUuid.equals(GattUuid.CHAR_MODEL_NUMBER_STRING)) {
                base = new ModelNumberString(characteristic.getValue(), characteristic);
                this.onDisModelNumberStringReadRequest(
                        device, requestId, offset, (ModelNumberString) base);
                return;
            } else if (charUuid.equals(GattUuid.CHAR_SERIAL_NUMBER_STRING)) {
                base = new SerialNumberString(characteristic.getValue(), characteristic);
                this.onDisSerialNumberStringReadRequest(
                        device, requestId, offset, (SerialNumberString) base);
                return;
            } else if (charUuid.equals(GattUuid.CHAR_HARDWARE_REVISION_STRING)) {
                base = new HardwareRevisionString(characteristic.getValue(), characteristic);
                this.onDisHardwareRevisionStringReadRequest(
                        device, requestId, offset, (HardwareRevisionString) base);
                return;
            } else if (charUuid.equals(GattUuid.CHAR_FIRMWARE_REVISION_STRING)) {
                base = new FirmwareRevisionString(characteristic.getValue(), characteristic);
                this.onDisFirmwareRevisionStringReadRequest(
                        device, requestId, offset, (FirmwareRevisionString) base);
                return;
            } else if (charUuid.equals(GattUuid.CHAR_SOFTWARE_REVISION_STRING)) {
                base = new SoftwareRevisionString(characteristic.getValue(), characteristic);
                this.onDisSoftwareRevisionStringReadRequest(
                        device, requestId, offset, (SoftwareRevisionString) base);
                return;
            } else if (charUuid.equals(GattUuid.CHAR_SYSTEM_ID)) {
                base = new SystemId(characteristic.getValue(), characteristic);
                this.onDisSystemIdReadRequest(
                        device, requestId, offset, (SystemId) base);
                return;
            } else if (charUuid.equals(GattUuid.CHAR_REG_CERT_DATA_LIST)) {
                base = new RegCertDataList(characteristic.getValue(), characteristic);
                this.onDisRegCertDataListReadRequest(
                        device, requestId, offset, (RegCertDataList) base);
                return;
            } else if (charUuid.equals(GattUuid.CHAR_PNP_ID)) {
                base = new PnpId(characteristic.getValue(), characteristic);
                this.onDisPnpIdReadRequest(
                        device, requestId, offset, (PnpId) base);
                return;
            }
        }
        if (srvcUuid.equals(GattUuid.SRVC_BAS)) {
             if (charUuid.equals(GattUuid.CHAR_BATTERY_LEVEL)) {
                base = new BatteryLevel(characteristic.getValue(), characteristic);
                this.onBasBatteryLevelReadRequest(
                        device, requestId, offset, (BatteryLevel) base);
                return;
            }
        }

        sendErrorResponse(device, requestId, BluetoothGatt.GATT_READ_NOT_PERMITTED);
    }

    @Override
    void dispatchCharacteristicWriteRequest(BluetoothDevice device, int requestId,
            BluetoothGattCharacteristic characteristic, boolean preparedWrite,
            boolean responseNeeded, int offset, byte[] value) {
        final UUID charUuid = characteristic.getUuid();
        final UUID srvcUuid = characteristic.getService().getUuid();
        CharacteristicBase base;

        if (srvcUuid.equals(GattUuid.SRVC_LNS)) {
             if (charUuid.equals(GattUuid.CHAR_LN_CONTROL_POINT)) {
                base = new LnControlPoint(characteristic.getValue(), characteristic);
                this.onLnsLnControlPointWriteRequest(
                        device, requestId, (LnControlPoint) base,
                        preparedWrite, responseNeeded, offset, value);
                return;
            }
        }

        sendErrorResponse(device, requestId, BluetoothGatt.GATT_WRITE_NOT_PERMITTED);
    }

    @Override
    void dispatchDescriptorReadRequest(BluetoothDevice device, int requestId, int offset,
            BluetoothGattDescriptor descriptor) {
        final UUID charUuid = descriptor.getCharacteristic().getUuid();
        final UUID srvcUuid = descriptor.getCharacteristic().getService().getUuid();
        final UUID descrUuid = descriptor.getUuid();

        if (srvcUuid.equals(GattUuid.SRVC_LNS)) {
             if (charUuid.equals(GattUuid.CHAR_LOCATION_AND_SPEED) &&
                    descrUuid.equals(GattUuid.DESCR_CLIENT_CHARACTERISTIC_CONFIGURATION)) {
                this.onLnsLocationAndSpeedCccdReadRequest(
                        device, requestId, offset, descriptor);
                return;
            } else if (charUuid.equals(GattUuid.CHAR_LN_CONTROL_POINT) &&
                    descrUuid.equals(GattUuid.DESCR_CLIENT_CHARACTERISTIC_CONFIGURATION)) {
                this.onLnsLnControlPointCccdReadRequest(
                        device, requestId, offset, descriptor);
                return;
            } else if (charUuid.equals(GattUuid.CHAR_NAVIGATION) &&
                    descrUuid.equals(GattUuid.DESCR_CLIENT_CHARACTERISTIC_CONFIGURATION)) {
                this.onLnsNavigationCccdReadRequest(
                        device, requestId, offset, descriptor);
                return;
            }
        }
        if (srvcUuid.equals(GattUuid.SRVC_BAS)) {
             if (charUuid.equals(GattUuid.CHAR_BATTERY_LEVEL) &&
                    descrUuid.equals(GattUuid.DESCR_CHARACTERISTIC_PRESENTATION_FORMAT)) {
                this.onBasBatteryLevelCpfdReadRequest(
                        device, requestId, offset, descriptor);
                return;
            } else if (charUuid.equals(GattUuid.CHAR_BATTERY_LEVEL) &&
                    descrUuid.equals(GattUuid.DESCR_CLIENT_CHARACTERISTIC_CONFIGURATION)) {
                this.onBasBatteryLevelCccdReadRequest(
                        device, requestId, offset, descriptor);
                return;
            }
        }

        sendErrorResponse(device, requestId, BluetoothGatt.GATT_READ_NOT_PERMITTED);
    }

    @Override
    void dispatchDescriptorWriteRequest(BluetoothDevice device, int requestId,
            BluetoothGattDescriptor descriptor, boolean preparedWrite, boolean responseNeeded,
            int offset, byte[] value) {
        final UUID charUuid = descriptor.getCharacteristic().getUuid();
        final UUID srvcUuid = descriptor.getCharacteristic().getService().getUuid();
        final UUID descrUuid = descriptor.getUuid();

        if (srvcUuid.equals(GattUuid.SRVC_LNS)) {
             if (charUuid.equals(GattUuid.CHAR_LOCATION_AND_SPEED) &&
                    descrUuid.equals(GattUuid.DESCR_CLIENT_CHARACTERISTIC_CONFIGURATION)) {
                this.onLnsLocationAndSpeedCccdWriteRequest(
                        device, requestId, descriptor, preparedWrite, responseNeeded, offset,
                        value);
                return;
            } else if (charUuid.equals(GattUuid.CHAR_LN_CONTROL_POINT) &&
                    descrUuid.equals(GattUuid.DESCR_CLIENT_CHARACTERISTIC_CONFIGURATION)) {
                this.onLnsLnControlPointCccdWriteRequest(
                        device, requestId, descriptor, preparedWrite, responseNeeded, offset,
                        value);
                return;
            } else if (charUuid.equals(GattUuid.CHAR_NAVIGATION) &&
                    descrUuid.equals(GattUuid.DESCR_CLIENT_CHARACTERISTIC_CONFIGURATION)) {
                this.onLnsNavigationCccdWriteRequest(
                        device, requestId, descriptor, preparedWrite, responseNeeded, offset,
                        value);
                return;
            }
        }
        if (srvcUuid.equals(GattUuid.SRVC_BAS)) {
             if (charUuid.equals(GattUuid.CHAR_BATTERY_LEVEL) &&
                    descrUuid.equals(GattUuid.DESCR_CLIENT_CHARACTERISTIC_CONFIGURATION)) {
                this.onBasBatteryLevelCccdWriteRequest(
                        device, requestId, descriptor, preparedWrite, responseNeeded, offset,
                        value);
                return;
            }
        }

        sendErrorResponse(device, requestId, BluetoothGatt.GATT_WRITE_NOT_PERMITTED);
    }

    /**
     * A remote client has requested to read Lns:LnFeature
     * local characteristic.
     *
     * @param device The remote device that has requested the read operation
     * @param requestId The Id of the request
     * @param offset Offset into the value of the characteristic
     * @param lnFeature Characteristic to be read
     */
    public void onLnsLnFeatureReadRequest(
            BluetoothDevice device, int requestId, int offset,
            LnFeature lnFeature) {
        if (DBG) {
            Log.d(TAG, "onLnsLnFeatureReadRequest(): offset=" + offset);
        }

        sendResponse(device, requestId, offset,
                lnFeature.getValue(offset));
    }

    /**
     * A remote client has requested to read Lns:PositionQuality
     * local characteristic.
     *
     * @param device The remote device that has requested the read operation
     * @param requestId The Id of the request
     * @param offset Offset into the value of the characteristic
     * @param positionQuality Characteristic to be read
     */
    public void onLnsPositionQualityReadRequest(
            BluetoothDevice device, int requestId, int offset,
            PositionQuality positionQuality) {
        if (DBG) {
            Log.d(TAG, "onLnsPositionQualityReadRequest(): offset=" + offset);
        }

        sendResponse(device, requestId, offset,
                positionQuality.getValue(offset));
    }

    /**
     * A remote client has requested to read Dis:ManufacturerNameString
     * local characteristic.
     *
     * @param device The remote device that has requested the read operation
     * @param requestId The Id of the request
     * @param offset Offset into the value of the characteristic
     * @param manufacturerNameString Characteristic to be read
     */
    public void onDisManufacturerNameStringReadRequest(
            BluetoothDevice device, int requestId, int offset,
            ManufacturerNameString manufacturerNameString) {
        if (DBG) {
            Log.d(TAG, "onDisManufacturerNameStringReadRequest(): offset=" + offset);
        }

        sendResponse(device, requestId, offset,
                manufacturerNameString.getValue(offset));
    }

    /**
     * A remote client has requested to read Dis:ModelNumberString
     * local characteristic.
     *
     * @param device The remote device that has requested the read operation
     * @param requestId The Id of the request
     * @param offset Offset into the value of the characteristic
     * @param modelNumberString Characteristic to be read
     */
    public void onDisModelNumberStringReadRequest(
            BluetoothDevice device, int requestId, int offset,
            ModelNumberString modelNumberString) {
        if (DBG) {
            Log.d(TAG, "onDisModelNumberStringReadRequest(): offset=" + offset);
        }

        sendResponse(device, requestId, offset,
                modelNumberString.getValue(offset));
    }

    /**
     * A remote client has requested to read Dis:SerialNumberString
     * local characteristic.
     *
     * @param device The remote device that has requested the read operation
     * @param requestId The Id of the request
     * @param offset Offset into the value of the characteristic
     * @param serialNumberString Characteristic to be read
     */
    public void onDisSerialNumberStringReadRequest(
            BluetoothDevice device, int requestId, int offset,
            SerialNumberString serialNumberString) {
        if (DBG) {
            Log.d(TAG, "onDisSerialNumberStringReadRequest(): offset=" + offset);
        }

        sendResponse(device, requestId, offset,
                serialNumberString.getValue(offset));
    }

    /**
     * A remote client has requested to read Dis:HardwareRevisionString
     * local characteristic.
     *
     * @param device The remote device that has requested the read operation
     * @param requestId The Id of the request
     * @param offset Offset into the value of the characteristic
     * @param hardwareRevisionString Characteristic to be read
     */
    public void onDisHardwareRevisionStringReadRequest(
            BluetoothDevice device, int requestId, int offset,
            HardwareRevisionString hardwareRevisionString) {
        if (DBG) {
            Log.d(TAG, "onDisHardwareRevisionStringReadRequest(): offset=" + offset);
        }

        sendResponse(device, requestId, offset,
                hardwareRevisionString.getValue(offset));
    }

    /**
     * A remote client has requested to read Dis:FirmwareRevisionString
     * local characteristic.
     *
     * @param device The remote device that has requested the read operation
     * @param requestId The Id of the request
     * @param offset Offset into the value of the characteristic
     * @param firmwareRevisionString Characteristic to be read
     */
    public void onDisFirmwareRevisionStringReadRequest(
            BluetoothDevice device, int requestId, int offset,
            FirmwareRevisionString firmwareRevisionString) {
        if (DBG) {
            Log.d(TAG, "onDisFirmwareRevisionStringReadRequest(): offset=" + offset);
        }

        sendResponse(device, requestId, offset,
                firmwareRevisionString.getValue(offset));
    }

    /**
     * A remote client has requested to read Dis:SoftwareRevisionString
     * local characteristic.
     *
     * @param device The remote device that has requested the read operation
     * @param requestId The Id of the request
     * @param offset Offset into the value of the characteristic
     * @param softwareRevisionString Characteristic to be read
     */
    public void onDisSoftwareRevisionStringReadRequest(
            BluetoothDevice device, int requestId, int offset,
            SoftwareRevisionString softwareRevisionString) {
        if (DBG) {
            Log.d(TAG, "onDisSoftwareRevisionStringReadRequest(): offset=" + offset);
        }

        sendResponse(device, requestId, offset,
                softwareRevisionString.getValue(offset));
    }

    /**
     * A remote client has requested to read Dis:SystemId
     * local characteristic.
     *
     * @param device The remote device that has requested the read operation
     * @param requestId The Id of the request
     * @param offset Offset into the value of the characteristic
     * @param systemId Characteristic to be read
     */
    public void onDisSystemIdReadRequest(
            BluetoothDevice device, int requestId, int offset,
            SystemId systemId) {
        if (DBG) {
            Log.d(TAG, "onDisSystemIdReadRequest(): offset=" + offset);
        }

        sendResponse(device, requestId, offset,
                systemId.getValue(offset));
    }

    /**
     * A remote client has requested to read Dis:RegCertDataList
     * local characteristic.
     *
     * @param device The remote device that has requested the read operation
     * @param requestId The Id of the request
     * @param offset Offset into the value of the characteristic
     * @param regCertDataList Characteristic to be read
     */
    public void onDisRegCertDataListReadRequest(
            BluetoothDevice device, int requestId, int offset,
            RegCertDataList regCertDataList) {
        if (DBG) {
            Log.d(TAG, "onDisRegCertDataListReadRequest(): offset=" + offset);
        }

        sendResponse(device, requestId, offset,
                regCertDataList.getValue(offset));
    }

    /**
     * A remote client has requested to read Dis:PnpId
     * local characteristic.
     *
     * @param device The remote device that has requested the read operation
     * @param requestId The Id of the request
     * @param offset Offset into the value of the characteristic
     * @param pnpId Characteristic to be read
     */
    public void onDisPnpIdReadRequest(
            BluetoothDevice device, int requestId, int offset,
            PnpId pnpId) {
        if (DBG) {
            Log.d(TAG, "onDisPnpIdReadRequest(): offset=" + offset);
        }

        sendResponse(device, requestId, offset,
                pnpId.getValue(offset));
    }

    /**
     * A remote client has requested to read Bas:BatteryLevel
     * local characteristic.
     *
     * @param device The remote device that has requested the read operation
     * @param requestId The Id of the request
     * @param offset Offset into the value of the characteristic
     * @param batteryLevel Characteristic to be read
     */
    public void onBasBatteryLevelReadRequest(
            BluetoothDevice device, int requestId, int offset,
            BatteryLevel batteryLevel) {
        if (DBG) {
            Log.d(TAG, "onBasBatteryLevelReadRequest(): offset=" + offset);
        }

        sendResponse(device, requestId, offset,
                batteryLevel.getValue(offset));
    }


    /**
     * A remote client has requested to write to Lns:LnControlPoint
     * local characteristic.
     *
     * @param device The remote device that has requested the write operation
     * @param requestId The Id of the request
     * @param lnControlPoint Characteristic to be written to.
     * @param preparedWrite true, if this write operation should be queued for
     *                      later execution.
     * @param responseNeeded true, if the remote device requires a response
     * @param offset The offset given for the value
     * @param value The value the client wants to assign to the characteristic
     */
    public void onLnsLnControlPointWriteRequest(
            BluetoothDevice device, int requestId,
            LnControlPoint lnControlPoint,
            boolean preparedWrite, boolean responseNeeded, int offset, byte[] value) {
        if (DBG) {
            Log.d(TAG, "onLnsLnControlPointWriteRequest()");
        }

        if (preparedWrite) {
            prepareWrite(device, lnControlPoint, offset, value, false);
            if (responseNeeded) {
                sendResponse(device, requestId, offset, value);
            }
            return;
        }

        lnControlPoint.setValue(offset, value);
        if (responseNeeded) {
            sendResponse(device, requestId, offset, null);
        }
    }


    /**
     * A remote client has requested to read Lns:LocationAndSpeed:
     * Cccd local descriptor.
     *
     * @param device The remote device that has requested the read operation
     * @param requestId The Id of the request
     * @param offset Offset into the value of the characteristic
     * @param descriptor Descriptor to be read
     */
    public void onLnsLocationAndSpeedCccdReadRequest(
            BluetoothDevice device, int requestId, int offset, BluetoothGattDescriptor descriptor) {
        if (DBG) {
            Log.d(TAG, "onLnsLocationAndSpeedCccdReadRequest()");
        }

        sendResponse(device, requestId, offset, descriptor.getValue());
    }

    /**
     * A remote client has requested to read Lns:LnControlPoint:
     * Cccd local descriptor.
     *
     * @param device The remote device that has requested the read operation
     * @param requestId The Id of the request
     * @param offset Offset into the value of the characteristic
     * @param descriptor Descriptor to be read
     */
    public void onLnsLnControlPointCccdReadRequest(
            BluetoothDevice device, int requestId, int offset, BluetoothGattDescriptor descriptor) {
        if (DBG) {
            Log.d(TAG, "onLnsLnControlPointCccdReadRequest()");
        }

        sendResponse(device, requestId, offset, descriptor.getValue());
    }

    /**
     * A remote client has requested to read Lns:Navigation:
     * Cccd local descriptor.
     *
     * @param device The remote device that has requested the read operation
     * @param requestId The Id of the request
     * @param offset Offset into the value of the characteristic
     * @param descriptor Descriptor to be read
     */
    public void onLnsNavigationCccdReadRequest(
            BluetoothDevice device, int requestId, int offset, BluetoothGattDescriptor descriptor) {
        if (DBG) {
            Log.d(TAG, "onLnsNavigationCccdReadRequest()");
        }

        sendResponse(device, requestId, offset, descriptor.getValue());
    }

    /**
     * A remote client has requested to read Bas:BatteryLevel:
     * Cpfd local descriptor.
     *
     * @param device The remote device that has requested the read operation
     * @param requestId The Id of the request
     * @param offset Offset into the value of the characteristic
     * @param descriptor Descriptor to be read
     */
    public void onBasBatteryLevelCpfdReadRequest(
            BluetoothDevice device, int requestId, int offset, BluetoothGattDescriptor descriptor) {
        if (DBG) {
            Log.d(TAG, "onBasBatteryLevelCpfdReadRequest()");
        }

        sendResponse(device, requestId, offset, descriptor.getValue());
    }

    /**
     * A remote client has requested to read Bas:BatteryLevel:
     * Cccd local descriptor.
     *
     * @param device The remote device that has requested the read operation
     * @param requestId The Id of the request
     * @param offset Offset into the value of the characteristic
     * @param descriptor Descriptor to be read
     */
    public void onBasBatteryLevelCccdReadRequest(
            BluetoothDevice device, int requestId, int offset, BluetoothGattDescriptor descriptor) {
        if (DBG) {
            Log.d(TAG, "onBasBatteryLevelCccdReadRequest()");
        }

        sendResponse(device, requestId, offset, descriptor.getValue());
    }


    /**
     * A remote client has requested to write Lns:LocationAndSpeed:
     * Cccd local descriptor.
     *
     * @param device The remote device that has requested the write operation
     * @param requestId The Id of the request
     * @param descriptor Descriptor to be written to.
     * @param preparedWrite true, if this write operation should be queued for
     *                      later execution.
     * @param responseNeeded true, if the remote device requires a response
     * @param offset The offset given for the value
     * @param value The value the client wants to assign to the descriptor
     */
    public void onLnsLocationAndSpeedCccdWriteRequest(
            BluetoothDevice device, int requestId, BluetoothGattDescriptor descriptor,
            boolean preparedWrite, boolean responseNeeded, int offset, byte[] value) {
        if (DBG) {
            Log.d(TAG, "onLnsLocationAndSpeedCccdWriteRequest()");
        }

        if (preparedWrite) {
            prepareWrite(device, descriptor, offset, value, true);
            if (responseNeeded) {
                sendResponse(device, requestId, offset, value);
            }
            return;
        }

        if (!updateCccd(device, descriptor, value)) {
            if (responseNeeded) {
                sendErrorResponse(device, requestId, BluetoothGatt.GATT_FAILURE);
            }
            return;
        }

        descriptor.setValue(value);
        if (responseNeeded) {
            sendResponse(device, requestId, offset, null);
        }
    }

    /**
     * A remote client has requested to write Lns:LnControlPoint:
     * Cccd local descriptor.
     *
     * @param device The remote device that has requested the write operation
     * @param requestId The Id of the request
     * @param descriptor Descriptor to be written to.
     * @param preparedWrite true, if this write operation should be queued for
     *                      later execution.
     * @param responseNeeded true, if the remote device requires a response
     * @param offset The offset given for the value
     * @param value The value the client wants to assign to the descriptor
     */
    public void onLnsLnControlPointCccdWriteRequest(
            BluetoothDevice device, int requestId, BluetoothGattDescriptor descriptor,
            boolean preparedWrite, boolean responseNeeded, int offset, byte[] value) {
        if (DBG) {
            Log.d(TAG, "onLnsLnControlPointCccdWriteRequest()");
        }

        if (preparedWrite) {
            prepareWrite(device, descriptor, offset, value, true);
            if (responseNeeded) {
                sendResponse(device, requestId, offset, value);
            }
            return;
        }

        if (!updateCccd(device, descriptor, value)) {
            if (responseNeeded) {
                sendErrorResponse(device, requestId, BluetoothGatt.GATT_FAILURE);
            }
            return;
        }

        descriptor.setValue(value);
        if (responseNeeded) {
            sendResponse(device, requestId, offset, null);
        }
    }

    /**
     * A remote client has requested to write Lns:Navigation:
     * Cccd local descriptor.
     *
     * @param device The remote device that has requested the write operation
     * @param requestId The Id of the request
     * @param descriptor Descriptor to be written to.
     * @param preparedWrite true, if this write operation should be queued for
     *                      later execution.
     * @param responseNeeded true, if the remote device requires a response
     * @param offset The offset given for the value
     * @param value The value the client wants to assign to the descriptor
     */
    public void onLnsNavigationCccdWriteRequest(
            BluetoothDevice device, int requestId, BluetoothGattDescriptor descriptor,
            boolean preparedWrite, boolean responseNeeded, int offset, byte[] value) {
        if (DBG) {
            Log.d(TAG, "onLnsNavigationCccdWriteRequest()");
        }

        if (preparedWrite) {
            prepareWrite(device, descriptor, offset, value, true);
            if (responseNeeded) {
                sendResponse(device, requestId, offset, value);
            }
            return;
        }

        if (!updateCccd(device, descriptor, value)) {
            if (responseNeeded) {
                sendErrorResponse(device, requestId, BluetoothGatt.GATT_FAILURE);
            }
            return;
        }

        descriptor.setValue(value);
        if (responseNeeded) {
            sendResponse(device, requestId, offset, null);
        }
    }

    /**
     * A remote client has requested to write Bas:BatteryLevel:
     * Cccd local descriptor.
     *
     * @param device The remote device that has requested the write operation
     * @param requestId The Id of the request
     * @param descriptor Descriptor to be written to.
     * @param preparedWrite true, if this write operation should be queued for
     *                      later execution.
     * @param responseNeeded true, if the remote device requires a response
     * @param offset The offset given for the value
     * @param value The value the client wants to assign to the descriptor
     */
    public void onBasBatteryLevelCccdWriteRequest(
            BluetoothDevice device, int requestId, BluetoothGattDescriptor descriptor,
            boolean preparedWrite, boolean responseNeeded, int offset, byte[] value) {
        if (DBG) {
            Log.d(TAG, "onBasBatteryLevelCccdWriteRequest()");
        }

        if (preparedWrite) {
            prepareWrite(device, descriptor, offset, value, true);
            if (responseNeeded) {
                sendResponse(device, requestId, offset, value);
            }
            return;
        }

        if (!updateCccd(device, descriptor, value)) {
            if (responseNeeded) {
                sendErrorResponse(device, requestId, BluetoothGatt.GATT_FAILURE);
            }
            return;
        }

        descriptor.setValue(value);
        if (responseNeeded) {
            sendResponse(device, requestId, offset, null);
        }
    }

}

