/*
 * Copyright 2015 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef _MTK_KEYMASTER_HAL_H_
#define _MTK_KEYMASTER_HAL_H_

#include <stdlib.h>

#include <hardware/keymaster0.h>
#include <hardware/keymaster1.h>

#include <keymaster/android_keymaster.h>
#include "keymaster_logger.h"

#include <UniquePtr.h>

using namespace keymaster;

/**
 * Keymaster1 device implementation.
 *
 * This is a hybrid software/hardware implementation which wraps a keymaster0_device_t, forwarding
 * RSA operations to secure hardware and doing everything else in software.
 *
 * IMPORTANT MAINTAINER NOTE: Pointers to instances of this class must be castable to hw_device_t
 * and keymaster_device. This means it must remain a standard layout class (no virtual functions and
 * no data members which aren't standard layout), and device_ must be the first data member.
 * Assertions in the constructor validate compliance with those constraints.
 */
class KeymasterHal {
  public:
    KeymasterHal();
    keymaster1_device_t* keymaster_device();

	static int open_keymaster_hal(const struct hw_module_t* hw_module,
							  	  const char* name,
							      struct hw_device_t** hw_device_out);
  private:

    static keymaster_error_t GetPkcs8KeyAlgorithm(const uint8_t* key, size_t key_length,
                                                  keymaster_algorithm_t* algorithm);

    static int close_device(hw_device_t* dev);

    static int generate_keypair(const keymaster1_device_t* dev, const keymaster_keypair_t key_type,
                                const void* key_params, uint8_t** keyBlob, size_t* keyBlobLength);
    static int import_keypair(const struct keymaster1_device* dev, const uint8_t* key,
                              const size_t key_length, uint8_t** key_blob, size_t* key_blob_length);
    static int get_keypair_public(const keymaster1_device_t* dev, const uint8_t* key_blob,
                                  const size_t key_blob_length, uint8_t** x509_data,
                                  size_t* x509_data_length);
    static int sign_data(const keymaster1_device_t* dev, const void* signing_params,
                         const uint8_t* key_blob, const size_t key_blob_length, const uint8_t* data,
                         const size_t data_length, uint8_t** signed_data,
                         size_t* signed_data_length);
    static int verify_data(const keymaster1_device_t* dev, const void* signing_params,
                           const uint8_t* key_blob, const size_t key_blob_length,
                           const uint8_t* signed_data, const size_t signed_data_length,
                           const uint8_t* signature, const size_t signature_length);

    static keymaster_error_t get_supported_algorithms(const keymaster1_device_t* dev,
                                                      keymaster_algorithm_t** algorithms,
                                                      size_t* algorithms_length);
    static keymaster_error_t get_supported_block_modes(const keymaster1_device_t* dev,
                                                       keymaster_algorithm_t algorithm,
                                                       keymaster_purpose_t purpose,
                                                       keymaster_block_mode_t** modes,
                                                       size_t* modes_length);
    static keymaster_error_t get_supported_padding_modes(const keymaster1_device_t* dev,
                                                         keymaster_algorithm_t algorithm,
                                                         keymaster_purpose_t purpose,
                                                         keymaster_padding_t** modes,
                                                         size_t* modes_length);
    static keymaster_error_t get_supported_digests(const keymaster1_device_t* dev,
                                                   keymaster_algorithm_t algorithm,
                                                   keymaster_purpose_t purpose,
                                                   keymaster_digest_t** digests,
                                                   size_t* digests_length);
    static keymaster_error_t get_supported_import_formats(const keymaster1_device_t* dev,
                                                          keymaster_algorithm_t algorithm,
                                                          keymaster_key_format_t** formats,
                                                          size_t* formats_length);
    static keymaster_error_t get_supported_export_formats(const keymaster1_device_t* dev,
                                                          keymaster_algorithm_t algorithm,
                                                          keymaster_key_format_t** formats,
                                                          size_t* formats_length);
    static keymaster_error_t add_rng_entropy(const keymaster1_device_t* dev, const uint8_t* data,
                                             size_t data_length);
    static keymaster_error_t generate_key(const keymaster1_device_t* dev,
                                          const keymaster_key_param_set_t* params,
                                          keymaster_key_blob_t* key_blob,
                                          keymaster_key_characteristics_t** characteristics);
    static keymaster_error_t get_key_characteristics(const keymaster1_device_t* dev,
                                                     const keymaster_key_blob_t* key_blob,
                                                     const keymaster_blob_t* client_id,
                                                     const keymaster_blob_t* app_data,
                                                     keymaster_key_characteristics_t** character);
    static keymaster_error_t import_key(const keymaster1_device_t* dev,  //
                                        const keymaster_key_param_set_t* params,
                                        keymaster_key_format_t key_format,
                                        const keymaster_blob_t* key_data,
                                        keymaster_key_blob_t* key_blob,
                                        keymaster_key_characteristics_t** characteristics);
    static keymaster_error_t export_key(const keymaster1_device_t* dev,  //
                                        keymaster_key_format_t export_format,
                                        const keymaster_key_blob_t* key_to_export,
                                        const keymaster_blob_t* client_id,
                                        const keymaster_blob_t* app_data,
                                        keymaster_blob_t* export_data);

    static keymaster_error_t begin(const keymaster1_device_t* dev, keymaster_purpose_t purpose,
                                   const keymaster_key_blob_t* key,
                                   const keymaster_key_param_set_t* in_params,
                                   keymaster_key_param_set_t* out_params,
                                   keymaster_operation_handle_t* operation_handle);
    static keymaster_error_t
    update(const keymaster1_device_t* dev, keymaster_operation_handle_t operation_handle,
           const keymaster_key_param_set_t* in_params, const keymaster_blob_t* input,
           size_t* input_consumed, keymaster_key_param_set_t* out_params, keymaster_blob_t* output);
    static keymaster_error_t
    finish(const keymaster1_device_t* dev, keymaster_operation_handle_t operation_handle,
           const keymaster_key_param_set_t* in_params, const keymaster_blob_t* signature,
           keymaster_key_param_set_t* out_params, keymaster_blob_t* output);
    static keymaster_error_t abort(const keymaster1_device_t* dev,
                                   keymaster_operation_handle_t operation_handle);

    keymaster1_device_t device_;
    UniquePtr<AndroidKeymaster> impl_;
};

#endif  // _MTK_KEYMASTER_HAL_H_
