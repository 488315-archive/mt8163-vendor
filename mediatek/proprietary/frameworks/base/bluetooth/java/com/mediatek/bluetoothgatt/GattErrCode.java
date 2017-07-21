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

package com.mediatek.bluetoothgatt;


/**
 * Provide GATT error codes.
 */
public class GattErrCode {

    /**
     * A GATT operation completed successfully.
     */
    public static final int GATT_SUCCESS = 0;

    /**
     * A GATT operation failed, errors other than the above.
     */
    public static final int GATT_FAILURE = 0x101;

    /**
     * The attribute handle given was not valid on this server.
     */
    public static final int GATT_INVALID_HANDLE = 0x01;

    /**
     * The attribute cannot be read.
     */
    public static final int GATT_READ_NOT_PERMITTED = 0x02;

    /**
     * The attribute cannot be written.
     */
    public static final int GATT_WRITE_NOT_PERMITTED = 0x03;

    /**
     * The attribute PDU was invalid.
     */
    public static final int GATT_INVALID_PDU = 0x04;

    /**
     * The attribute requires authentication before it can be read or written.
     */
    public static final int GATT_INSUFFICIENT_AUTHENTICATION = 0x05;

    /**
     * Attribute server does not support the request received from the client.
     */
    public static final int GATT_REQUEST_NOT_SUPPORTED = 0x06;

    /**
     * Offset specified was past the end of the attribute.
     */
    public static final int GATT_INVALID_OFFSET = 0x07;

    /**
     * The attribute requires authorization before it can be read or written.
     */
    public static final int GATT_INSUFFICIENT_AUTHORIZATION = 0x08;

    /**
     * Too many prepare writes have been queued.
     */
    public static final int GATT_PREPARE_QUEUE_FULL = 0x09;

    /**
     * No attribute found within the given attribute handle range.
     */
    public static final int GATT_ATTRIBUTE_NOT_FOUND = 0x0A;

    /**
     * The attribute cannot be read or written using the Read Blob Request.
     */
    public static final int GATT_ATTRIBUTE_NOT_LONG = 0x0B;

    /**
     * The Encryption Key Size used for encrypting this link is insufficient.
     */
    public static final int GATT_INSUFFICIENT_ENCRYPTION_KEY_SIZE = 0x0C;

    /**
     * The attribute value length is invalid for the operation.
     */
    public static final int GATT_INVALID_ATTRIBUTE_VALUE_LENGTH = 0x0D;

    /**
     * The attribute request that was requested has encountered an error that was unlikely,
     * and therefore could not be completed as requested.
     */
    public static final int GATT_UNLIKELY_ERROR = 0x0E;

    /**
     * The attribute requires encryption before it can be read or written.
     */
    public static final int GATT_INSUFFICIENT_ENCRYPTION = 0x0F;

    /**
     * The attribute type is not a supported grouping attribute as defined by a higher layer
     * specification.
     */
    public static final int GATT_UNSUPPORTED_GROUP_TYPE = 0x10;

    /**
     * Insufficient Resources to complete the request.
     */
    public static final int GATT_INSUFFICIENT_RESOURCES = 0x11;


    /***********************************************************************************************
     * Application error code defined by a higher layer specification
     **********************************************************************************************/
    /**
     * Cycling Speed and Cadence Service error code.
     *
     * A SC Control Point request cannot be serviced because a previously triggered SC Control
     * Point operation is still in progress.
     */
    public static final int GATT_CSCS_PROCEDURE_ALREADY_IN_PROGRESS = 0x80;

    /**
     * Cycling Speed and Cadence Service error code.
     *
     * The Client Characteristic Configuration descriptor is not configured according to the
     * requirements of the service.
     */
    public static final int GATT_CSCS_CCCD_IMPROPERLY_CONFIGURED = 0x81;


    /***********************************************************************************************
     * Common Profile and Service error code descriptions.
     * Define in "Supplement to the Bluetooth Core Specification, Version 5".
     **********************************************************************************************/

    /**
     * The Out of Range error code is used when an attribute value is out of range as defined
     * by a profile or service specification.
     */
    public static final int GATT_OUT_OF_RANGE = 0xFF;

    public static final int GATT_PROCEDURE_ALREADY_IN_PROGRESS = 0xFE;
}
