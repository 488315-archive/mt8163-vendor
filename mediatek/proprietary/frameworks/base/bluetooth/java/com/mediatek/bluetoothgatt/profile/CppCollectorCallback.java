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
import android.bluetooth.BluetoothGattCharacteristic;
import android.bluetooth.BluetoothGattDescriptor;
import android.util.Log;

import com.mediatek.bluetoothgatt.GattUuid;
import com.mediatek.bluetoothgatt.characteristic.BatteryLevel;
import com.mediatek.bluetoothgatt.characteristic.CharacteristicBase;
import com.mediatek.bluetoothgatt.characteristic.CyclingPowerControlPoint;
import com.mediatek.bluetoothgatt.characteristic.CyclingPowerFeature;
import com.mediatek.bluetoothgatt.characteristic.CyclingPowerMeasurement;
import com.mediatek.bluetoothgatt.characteristic.CyclingPowerVector;
import com.mediatek.bluetoothgatt.characteristic.FirmwareRevisionString;
import com.mediatek.bluetoothgatt.characteristic.HardwareRevisionString;
import com.mediatek.bluetoothgatt.characteristic.ManufacturerNameString;
import com.mediatek.bluetoothgatt.characteristic.ModelNumberString;
import com.mediatek.bluetoothgatt.characteristic.PnpId;
import com.mediatek.bluetoothgatt.characteristic.RegCertDataList;
import com.mediatek.bluetoothgatt.characteristic.SensorLocation;
import com.mediatek.bluetoothgatt.characteristic.SerialNumberString;
import com.mediatek.bluetoothgatt.characteristic.SoftwareRevisionString;
import com.mediatek.bluetoothgatt.characteristic.SystemId;

import java.util.UUID;
//........................................................................ Customized End: Import //

/**
 * This class is used to implement {@link CppCollector} callbacks.
 */
public class CppCollectorCallback extends ClientBaseCallback {
    private static final boolean DBG = true;
    private static final String TAG = "CppCollectorCallback";

    @Override
    void dispatchCharacteristicRead(BluetoothGatt gatt, BluetoothGattCharacteristic characteristic,
            int status) {
        final UUID charUuid = characteristic.getUuid();
        final UUID srvcUuid = characteristic.getService().getUuid();
        CharacteristicBase base;

        if (srvcUuid.equals(GattUuid.SRVC_CPS)) {
             if (charUuid.equals(GattUuid.CHAR_CYCLING_POWER_FEATURE)) {
                base = new CyclingPowerFeature();
                base.setCharacteristic(characteristic);
                if (status == BluetoothGatt.GATT_SUCCESS) {
                    base.setValue(characteristic.getValue());
                }
                this.onCpsCyclingPowerFeatureReadResponse(
                        (CyclingPowerFeature) base, status);
                return;
            } else if (charUuid.equals(GattUuid.CHAR_SENSOR_LOCATION)) {
                base = new SensorLocation();
                base.setCharacteristic(characteristic);
                if (status == BluetoothGatt.GATT_SUCCESS) {
                    base.setValue(characteristic.getValue());
                }
                this.onCpsSensorLocationReadResponse(
                        (SensorLocation) base, status);
                return;
            }
        }
        if (srvcUuid.equals(GattUuid.SRVC_DIS)) {
             if (charUuid.equals(GattUuid.CHAR_MANUFACTURER_NAME_STRING)) {
                base = new ManufacturerNameString();
                base.setCharacteristic(characteristic);
                if (status == BluetoothGatt.GATT_SUCCESS) {
                    base.setValue(characteristic.getValue());
                }
                this.onDisManufacturerNameStringReadResponse(
                        (ManufacturerNameString) base, status);
                return;
            } else if (charUuid.equals(GattUuid.CHAR_MODEL_NUMBER_STRING)) {
                base = new ModelNumberString();
                base.setCharacteristic(characteristic);
                if (status == BluetoothGatt.GATT_SUCCESS) {
                    base.setValue(characteristic.getValue());
                }
                this.onDisModelNumberStringReadResponse(
                        (ModelNumberString) base, status);
                return;
            } else if (charUuid.equals(GattUuid.CHAR_SERIAL_NUMBER_STRING)) {
                base = new SerialNumberString();
                base.setCharacteristic(characteristic);
                if (status == BluetoothGatt.GATT_SUCCESS) {
                    base.setValue(characteristic.getValue());
                }
                this.onDisSerialNumberStringReadResponse(
                        (SerialNumberString) base, status);
                return;
            } else if (charUuid.equals(GattUuid.CHAR_HARDWARE_REVISION_STRING)) {
                base = new HardwareRevisionString();
                base.setCharacteristic(characteristic);
                if (status == BluetoothGatt.GATT_SUCCESS) {
                    base.setValue(characteristic.getValue());
                }
                this.onDisHardwareRevisionStringReadResponse(
                        (HardwareRevisionString) base, status);
                return;
            } else if (charUuid.equals(GattUuid.CHAR_FIRMWARE_REVISION_STRING)) {
                base = new FirmwareRevisionString();
                base.setCharacteristic(characteristic);
                if (status == BluetoothGatt.GATT_SUCCESS) {
                    base.setValue(characteristic.getValue());
                }
                this.onDisFirmwareRevisionStringReadResponse(
                        (FirmwareRevisionString) base, status);
                return;
            } else if (charUuid.equals(GattUuid.CHAR_SOFTWARE_REVISION_STRING)) {
                base = new SoftwareRevisionString();
                base.setCharacteristic(characteristic);
                if (status == BluetoothGatt.GATT_SUCCESS) {
                    base.setValue(characteristic.getValue());
                }
                this.onDisSoftwareRevisionStringReadResponse(
                        (SoftwareRevisionString) base, status);
                return;
            } else if (charUuid.equals(GattUuid.CHAR_SYSTEM_ID)) {
                base = new SystemId();
                base.setCharacteristic(characteristic);
                if (status == BluetoothGatt.GATT_SUCCESS) {
                    base.setValue(characteristic.getValue());
                }
                this.onDisSystemIdReadResponse(
                        (SystemId) base, status);
                return;
            } else if (charUuid.equals(GattUuid.CHAR_REG_CERT_DATA_LIST)) {
                base = new RegCertDataList();
                base.setCharacteristic(characteristic);
                if (status == BluetoothGatt.GATT_SUCCESS) {
                    base.setValue(characteristic.getValue());
                }
                this.onDisRegCertDataListReadResponse(
                        (RegCertDataList) base, status);
                return;
            } else if (charUuid.equals(GattUuid.CHAR_PNP_ID)) {
                base = new PnpId();
                base.setCharacteristic(characteristic);
                if (status == BluetoothGatt.GATT_SUCCESS) {
                    base.setValue(characteristic.getValue());
                }
                this.onDisPnpIdReadResponse(
                        (PnpId) base, status);
                return;
            }
        }
        if (srvcUuid.equals(GattUuid.SRVC_BAS)) {
             if (charUuid.equals(GattUuid.CHAR_BATTERY_LEVEL)) {
                base = new BatteryLevel();
                base.setCharacteristic(characteristic);
                if (status == BluetoothGatt.GATT_SUCCESS) {
                    base.setValue(characteristic.getValue());
                }
                this.onBasBatteryLevelReadResponse(
                        (BatteryLevel) base, status);
                return;
            }
        }

        if (DBG) {
            Log.d(TAG, "Unknown Characteristic UUID=" + charUuid);
        }
    }

    @Override
    void dispatchCharacteristicWrite(BluetoothGatt gatt, BluetoothGattCharacteristic characteristic,
            int status) {
        final UUID charUuid = characteristic.getUuid();
        final UUID srvcUuid = characteristic.getService().getUuid();
        CharacteristicBase base;

        if (srvcUuid.equals(GattUuid.SRVC_CPS)) {
             if (charUuid.equals(GattUuid.CHAR_CYCLING_POWER_CONTROL_POINT)) {
                base = new CyclingPowerControlPoint();
                base.setCharacteristic(characteristic);
                if (status == BluetoothGatt.GATT_SUCCESS) {
                    base.setValue(characteristic.getValue());
                }
                this.onCpsCyclingPowerControlPointWriteResponse(
                        (CyclingPowerControlPoint) base, status);
                return;
            }
        }

        if (DBG) {
            Log.d(TAG, "Unknown Characteristic UUID=" + charUuid);
        }
    }

    @Override
    void dispatchCharacteristicChanged(BluetoothGatt gatt,
            BluetoothGattCharacteristic characteristic) {
        final UUID charUuid = characteristic.getUuid();
        final UUID srvcUuid = characteristic.getService().getUuid();
        CharacteristicBase base;

        if (srvcUuid.equals(GattUuid.SRVC_CPS)) {
             if (charUuid.equals(GattUuid.CHAR_CYCLING_POWER_MEASUREMENT)) {
                base = new CyclingPowerMeasurement(characteristic.getValue(), characteristic);
                this.onCpsCyclingPowerMeasurementNotify(
                        (CyclingPowerMeasurement) base);
                return;
            } else if (charUuid.equals(GattUuid.CHAR_CYCLING_POWER_VECTOR)) {
                base = new CyclingPowerVector(characteristic.getValue(), characteristic);
                this.onCpsCyclingPowerVectorNotify(
                        (CyclingPowerVector) base);
                return;
            } else if (charUuid.equals(GattUuid.CHAR_CYCLING_POWER_CONTROL_POINT)) {
                base = new CyclingPowerControlPoint(characteristic.getValue(), characteristic);
                this.onCpsCyclingPowerControlPointNotify(
                        (CyclingPowerControlPoint) base);
                return;
            }
        }
        if (srvcUuid.equals(GattUuid.SRVC_BAS)) {
             if (charUuid.equals(GattUuid.CHAR_BATTERY_LEVEL)) {
                base = new BatteryLevel(characteristic.getValue(), characteristic);
                this.onBasBatteryLevelNotify(
                        (BatteryLevel) base);
                return;
            }
        }

        if (DBG) {
            Log.d(TAG, "Unknown Characteristic UUID=" + charUuid);
        }
    }

    @Override
    void dispatchDescriptorRead(BluetoothGatt gatt,
            BluetoothGattDescriptor descriptor, int status) {
        final UUID charUuid = descriptor.getCharacteristic().getUuid();
        final UUID srvcUuid = descriptor.getCharacteristic().getService().getUuid();
        final UUID descrUuid = descriptor.getUuid();

        if (srvcUuid.equals(GattUuid.SRVC_CPS)) {
             if (charUuid.equals(GattUuid.CHAR_CYCLING_POWER_MEASUREMENT) &&
                    descrUuid.equals(GattUuid.DESCR_CLIENT_CHARACTERISTIC_CONFIGURATION)) {
                this.onCpsCyclingPowerMeasurementCccdReadResponse(descriptor, status);
                return;
            } else if (charUuid.equals(GattUuid.CHAR_CYCLING_POWER_MEASUREMENT) &&
                    descrUuid.equals(GattUuid.DESCR_SERVER_CHARACTERISTIC_CONFIGURATION)) {
                this.onCpsCyclingPowerMeasurementSccdReadResponse(descriptor, status);
                return;
            } else if (charUuid.equals(GattUuid.CHAR_CYCLING_POWER_VECTOR) &&
                    descrUuid.equals(GattUuid.DESCR_CLIENT_CHARACTERISTIC_CONFIGURATION)) {
                this.onCpsCyclingPowerVectorCccdReadResponse(descriptor, status);
                return;
            } else if (charUuid.equals(GattUuid.CHAR_CYCLING_POWER_CONTROL_POINT) &&
                    descrUuid.equals(GattUuid.DESCR_CLIENT_CHARACTERISTIC_CONFIGURATION)) {
                this.onCpsCyclingPowerControlPointCccdReadResponse(descriptor, status);
                return;
            }
        }
        if (srvcUuid.equals(GattUuid.SRVC_BAS)) {
             if (charUuid.equals(GattUuid.CHAR_BATTERY_LEVEL) &&
                    descrUuid.equals(GattUuid.DESCR_CHARACTERISTIC_PRESENTATION_FORMAT)) {
                this.onBasBatteryLevelCpfdReadResponse(descriptor, status);
                return;
            } else if (charUuid.equals(GattUuid.CHAR_BATTERY_LEVEL) &&
                    descrUuid.equals(GattUuid.DESCR_CLIENT_CHARACTERISTIC_CONFIGURATION)) {
                this.onBasBatteryLevelCccdReadResponse(descriptor, status);
                return;
            }
        }

        if (DBG) {
            Log.d(TAG, "Unknown Descriptor UUID=" + descrUuid);
        }
    }

    @Override
    void dispatchDescriptorWrite(BluetoothGatt gatt, BluetoothGattDescriptor descriptor,
            int status) {
        final UUID charUuid = descriptor.getCharacteristic().getUuid();
        final UUID srvcUuid = descriptor.getCharacteristic().getService().getUuid();
        final UUID descrUuid = descriptor.getUuid();

        if (srvcUuid.equals(GattUuid.SRVC_CPS)) {
             if (charUuid.equals(GattUuid.CHAR_CYCLING_POWER_MEASUREMENT) &&
                    descrUuid.equals(GattUuid.DESCR_CLIENT_CHARACTERISTIC_CONFIGURATION)) {
                this.onCpsCyclingPowerMeasurementCccdWriteResponse(descriptor, status);
                return;
            } else if (charUuid.equals(GattUuid.CHAR_CYCLING_POWER_MEASUREMENT) &&
                    descrUuid.equals(GattUuid.DESCR_SERVER_CHARACTERISTIC_CONFIGURATION)) {
                this.onCpsCyclingPowerMeasurementSccdWriteResponse(descriptor, status);
                return;
            } else if (charUuid.equals(GattUuid.CHAR_CYCLING_POWER_VECTOR) &&
                    descrUuid.equals(GattUuid.DESCR_CLIENT_CHARACTERISTIC_CONFIGURATION)) {
                this.onCpsCyclingPowerVectorCccdWriteResponse(descriptor, status);
                return;
            } else if (charUuid.equals(GattUuid.CHAR_CYCLING_POWER_CONTROL_POINT) &&
                    descrUuid.equals(GattUuid.DESCR_CLIENT_CHARACTERISTIC_CONFIGURATION)) {
                this.onCpsCyclingPowerControlPointCccdWriteResponse(descriptor, status);
                return;
            }
        }
        if (srvcUuid.equals(GattUuid.SRVC_BAS)) {
             if (charUuid.equals(GattUuid.CHAR_BATTERY_LEVEL) &&
                    descrUuid.equals(GattUuid.DESCR_CLIENT_CHARACTERISTIC_CONFIGURATION)) {
                this.onBasBatteryLevelCccdWriteResponse(descriptor, status);
                return;
            }
        }

        if (DBG) {
            Log.d(TAG, "Unknown Descriptor UUID=" + descrUuid);
        }
    }

    /**
     * Callback reporting the result of a
     * Cps:CyclingPowerFeature characteristic read operation.
     *
     * @param cyclingPowerFeature Cps:CyclingPowerFeature characteristic
     *                       that was read from the associated remote device.
     * @param status {@link BluetoothGatt#GATT_SUCCESS} if the read operation
     *               was completed successfully.
     */
    public void onCpsCyclingPowerFeatureReadResponse(
            CyclingPowerFeature cyclingPowerFeature, int status) {
    }

    /**
     * Callback reporting the result of a
     * Cps:SensorLocation characteristic read operation.
     *
     * @param sensorLocation Cps:SensorLocation characteristic
     *                       that was read from the associated remote device.
     * @param status {@link BluetoothGatt#GATT_SUCCESS} if the read operation
     *               was completed successfully.
     */
    public void onCpsSensorLocationReadResponse(
            SensorLocation sensorLocation, int status) {
    }

    /**
     * Callback reporting the result of a
     * Dis:ManufacturerNameString characteristic read operation.
     *
     * @param manufacturerNameString Dis:ManufacturerNameString characteristic
     *                       that was read from the associated remote device.
     * @param status {@link BluetoothGatt#GATT_SUCCESS} if the read operation
     *               was completed successfully.
     */
    public void onDisManufacturerNameStringReadResponse(
            ManufacturerNameString manufacturerNameString, int status) {
    }

    /**
     * Callback reporting the result of a
     * Dis:ModelNumberString characteristic read operation.
     *
     * @param modelNumberString Dis:ModelNumberString characteristic
     *                       that was read from the associated remote device.
     * @param status {@link BluetoothGatt#GATT_SUCCESS} if the read operation
     *               was completed successfully.
     */
    public void onDisModelNumberStringReadResponse(
            ModelNumberString modelNumberString, int status) {
    }

    /**
     * Callback reporting the result of a
     * Dis:SerialNumberString characteristic read operation.
     *
     * @param serialNumberString Dis:SerialNumberString characteristic
     *                       that was read from the associated remote device.
     * @param status {@link BluetoothGatt#GATT_SUCCESS} if the read operation
     *               was completed successfully.
     */
    public void onDisSerialNumberStringReadResponse(
            SerialNumberString serialNumberString, int status) {
    }

    /**
     * Callback reporting the result of a
     * Dis:HardwareRevisionString characteristic read operation.
     *
     * @param hardwareRevisionString Dis:HardwareRevisionString characteristic
     *                       that was read from the associated remote device.
     * @param status {@link BluetoothGatt#GATT_SUCCESS} if the read operation
     *               was completed successfully.
     */
    public void onDisHardwareRevisionStringReadResponse(
            HardwareRevisionString hardwareRevisionString, int status) {
    }

    /**
     * Callback reporting the result of a
     * Dis:FirmwareRevisionString characteristic read operation.
     *
     * @param firmwareRevisionString Dis:FirmwareRevisionString characteristic
     *                       that was read from the associated remote device.
     * @param status {@link BluetoothGatt#GATT_SUCCESS} if the read operation
     *               was completed successfully.
     */
    public void onDisFirmwareRevisionStringReadResponse(
            FirmwareRevisionString firmwareRevisionString, int status) {
    }

    /**
     * Callback reporting the result of a
     * Dis:SoftwareRevisionString characteristic read operation.
     *
     * @param softwareRevisionString Dis:SoftwareRevisionString characteristic
     *                       that was read from the associated remote device.
     * @param status {@link BluetoothGatt#GATT_SUCCESS} if the read operation
     *               was completed successfully.
     */
    public void onDisSoftwareRevisionStringReadResponse(
            SoftwareRevisionString softwareRevisionString, int status) {
    }

    /**
     * Callback reporting the result of a
     * Dis:SystemId characteristic read operation.
     *
     * @param systemId Dis:SystemId characteristic
     *                       that was read from the associated remote device.
     * @param status {@link BluetoothGatt#GATT_SUCCESS} if the read operation
     *               was completed successfully.
     */
    public void onDisSystemIdReadResponse(
            SystemId systemId, int status) {
    }

    /**
     * Callback reporting the result of a
     * Dis:RegCertDataList characteristic read operation.
     *
     * @param regCertDataList Dis:RegCertDataList characteristic
     *                       that was read from the associated remote device.
     * @param status {@link BluetoothGatt#GATT_SUCCESS} if the read operation
     *               was completed successfully.
     */
    public void onDisRegCertDataListReadResponse(
            RegCertDataList regCertDataList, int status) {
    }

    /**
     * Callback reporting the result of a
     * Dis:PnpId characteristic read operation.
     *
     * @param pnpId Dis:PnpId characteristic
     *                       that was read from the associated remote device.
     * @param status {@link BluetoothGatt#GATT_SUCCESS} if the read operation
     *               was completed successfully.
     */
    public void onDisPnpIdReadResponse(
            PnpId pnpId, int status) {
    }

    /**
     * Callback reporting the result of a
     * Bas:BatteryLevel characteristic read operation.
     *
     * @param batteryLevel Bas:BatteryLevel characteristic
     *                       that was read from the associated remote device.
     * @param status {@link BluetoothGatt#GATT_SUCCESS} if the read operation
     *               was completed successfully.
     */
    public void onBasBatteryLevelReadResponse(
            BatteryLevel batteryLevel, int status) {
    }


    /**
     * Callback indicating the result of a
     * Cps:CyclingPowerControlPoint characteristic write operation.
     *
     * @param cyclingPowerControlPoint Cps:CyclingPowerControlPoint characteristic
     *                       that was written to the associated remote device.
     * @param status The result of the write operation
     *               {@link BluetoothGatt#GATT_SUCCESS} if the operation succeeds.
     */
    public void onCpsCyclingPowerControlPointWriteResponse(
            CyclingPowerControlPoint cyclingPowerControlPoint, int status) {
    }


    /**
     * Callback reporting the result of a
     * Cps:CyclingPowerMeasurement:Cccd descriptor read operation.
     *
     * @param descriptor Descriptor that was read from the associated
     *                   remote device.
     * @param status {@link BluetoothGatt#GATT_SUCCESS} if the read operation
     *               was completed successfully
     */
    public void onCpsCyclingPowerMeasurementCccdReadResponse(
            BluetoothGattDescriptor descriptor, int status) {
    }

    /**
     * Callback reporting the result of a
     * Cps:CyclingPowerMeasurement:Sccd descriptor read operation.
     *
     * @param descriptor Descriptor that was read from the associated
     *                   remote device.
     * @param status {@link BluetoothGatt#GATT_SUCCESS} if the read operation
     *               was completed successfully
     */
    public void onCpsCyclingPowerMeasurementSccdReadResponse(
            BluetoothGattDescriptor descriptor, int status) {
    }

    /**
     * Callback reporting the result of a
     * Cps:CyclingPowerVector:Cccd descriptor read operation.
     *
     * @param descriptor Descriptor that was read from the associated
     *                   remote device.
     * @param status {@link BluetoothGatt#GATT_SUCCESS} if the read operation
     *               was completed successfully
     */
    public void onCpsCyclingPowerVectorCccdReadResponse(
            BluetoothGattDescriptor descriptor, int status) {
    }

    /**
     * Callback reporting the result of a
     * Cps:CyclingPowerControlPoint:Cccd descriptor read operation.
     *
     * @param descriptor Descriptor that was read from the associated
     *                   remote device.
     * @param status {@link BluetoothGatt#GATT_SUCCESS} if the read operation
     *               was completed successfully
     */
    public void onCpsCyclingPowerControlPointCccdReadResponse(
            BluetoothGattDescriptor descriptor, int status) {
    }

    /**
     * Callback reporting the result of a
     * Bas:BatteryLevel:Cpfd descriptor read operation.
     *
     * @param descriptor Descriptor that was read from the associated
     *                   remote device.
     * @param status {@link BluetoothGatt#GATT_SUCCESS} if the read operation
     *               was completed successfully
     */
    public void onBasBatteryLevelCpfdReadResponse(
            BluetoothGattDescriptor descriptor, int status) {
    }

    /**
     * Callback reporting the result of a
     * Bas:BatteryLevel:Cccd descriptor read operation.
     *
     * @param descriptor Descriptor that was read from the associated
     *                   remote device.
     * @param status {@link BluetoothGatt#GATT_SUCCESS} if the read operation
     *               was completed successfully
     */
    public void onBasBatteryLevelCccdReadResponse(
            BluetoothGattDescriptor descriptor, int status) {
    }


    /**
     * Callback indicating the result of a
     * Cps:CyclingPowerMeasurement:Cccd descriptor write operation.
     *
     * @param descriptor Descriptor that was written to the associated
     *                   remote device.
     * @param status The result of the write operation
     *               {@link BluetoothGatt#GATT_SUCCESS} if the operation succeeds.
     */
    public void onCpsCyclingPowerMeasurementCccdWriteResponse(
            BluetoothGattDescriptor descriptor, int status) {
    }

    /**
     * Callback indicating the result of a
     * Cps:CyclingPowerMeasurement:Sccd descriptor write operation.
     *
     * @param descriptor Descriptor that was written to the associated
     *                   remote device.
     * @param status The result of the write operation
     *               {@link BluetoothGatt#GATT_SUCCESS} if the operation succeeds.
     */
    public void onCpsCyclingPowerMeasurementSccdWriteResponse(
            BluetoothGattDescriptor descriptor, int status) {
    }

    /**
     * Callback indicating the result of a
     * Cps:CyclingPowerVector:Cccd descriptor write operation.
     *
     * @param descriptor Descriptor that was written to the associated
     *                   remote device.
     * @param status The result of the write operation
     *               {@link BluetoothGatt#GATT_SUCCESS} if the operation succeeds.
     */
    public void onCpsCyclingPowerVectorCccdWriteResponse(
            BluetoothGattDescriptor descriptor, int status) {
    }

    /**
     * Callback indicating the result of a
     * Cps:CyclingPowerControlPoint:Cccd descriptor write operation.
     *
     * @param descriptor Descriptor that was written to the associated
     *                   remote device.
     * @param status The result of the write operation
     *               {@link BluetoothGatt#GATT_SUCCESS} if the operation succeeds.
     */
    public void onCpsCyclingPowerControlPointCccdWriteResponse(
            BluetoothGattDescriptor descriptor, int status) {
    }

    /**
     * Callback indicating the result of a
     * Bas:BatteryLevel:Cccd descriptor write operation.
     *
     * @param descriptor Descriptor that was written to the associated
     *                   remote device.
     * @param status The result of the write operation
     *               {@link BluetoothGatt#GATT_SUCCESS} if the operation succeeds.
     */
    public void onBasBatteryLevelCccdWriteResponse(
            BluetoothGattDescriptor descriptor, int status) {
    }


    /**
     * Callback triggered as a result of a remote
     * Cps:CyclingPowerMeasurement characteristic notification.
     *
     * @param cyclingPowerMeasurement Characteristic that has been updated as a result
     *                       of a remote notification event.
     */
    public void onCpsCyclingPowerMeasurementNotify(
            CyclingPowerMeasurement cyclingPowerMeasurement) {
    }

    /**
     * Callback triggered as a result of a remote
     * Cps:CyclingPowerVector characteristic notification.
     *
     * @param cyclingPowerVector Characteristic that has been updated as a result
     *                       of a remote notification event.
     */
    public void onCpsCyclingPowerVectorNotify(
            CyclingPowerVector cyclingPowerVector) {
    }

    /**
     * Callback triggered as a result of a remote
     * Cps:CyclingPowerControlPoint characteristic notification.
     *
     * @param cyclingPowerControlPoint Characteristic that has been updated as a result
     *                       of a remote notification event.
     */
    public void onCpsCyclingPowerControlPointNotify(
            CyclingPowerControlPoint cyclingPowerControlPoint) {
    }

    /**
     * Callback triggered as a result of a remote
     * Bas:BatteryLevel characteristic notification.
     *
     * @param batteryLevel Characteristic that has been updated as a result
     *                       of a remote notification event.
     */
    public void onBasBatteryLevelNotify(
            BatteryLevel batteryLevel) {
    }

}

