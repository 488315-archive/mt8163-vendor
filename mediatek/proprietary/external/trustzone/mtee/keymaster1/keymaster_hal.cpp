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

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <utils/Timers.h>
#include <stddef.h>
#include <algorithm>
#include <type_traits>
#include <openssl/x509.h>
#include <hardware/keymaster1.h>
#include <cutils/log.h>
#include <keymaster/android_keymaster.h>
#include "keymaster_logger.h"
#include <UniquePtr.h>
#include <keymaster/android_keymaster_messages.h>
#include <keymaster/authorization_set.h>
#include "openssl_utils.h"
#include "keymaster_hal.h"

using namespace keymaster;

static
void StoreDefaultNewKeyParams(keymaster_algorithm_t algorithm,
                              AuthorizationSet* auth_set) {
    auth_set->push_back(TAG_PURPOSE, KM_PURPOSE_SIGN);
    auth_set->push_back(TAG_PURPOSE, KM_PURPOSE_VERIFY);
    auth_set->push_back(TAG_ALL_USERS);
    auth_set->push_back(TAG_NO_AUTH_REQUIRED);
	auth_set->push_back(TAG_CREATION_DATETIME, (uint64_t)systemTime());

    // All digests.
    auth_set->push_back(TAG_DIGEST, KM_DIGEST_NONE);
    auth_set->push_back(TAG_DIGEST, KM_DIGEST_MD5);
    auth_set->push_back(TAG_DIGEST, KM_DIGEST_SHA1);
    auth_set->push_back(TAG_DIGEST, KM_DIGEST_SHA_2_224);
    auth_set->push_back(TAG_DIGEST, KM_DIGEST_SHA_2_256);
    auth_set->push_back(TAG_DIGEST, KM_DIGEST_SHA_2_384);
    auth_set->push_back(TAG_DIGEST, KM_DIGEST_SHA_2_512);

    if (algorithm == KM_ALGORITHM_RSA) {
        auth_set->push_back(TAG_PURPOSE, KM_PURPOSE_ENCRYPT);
        auth_set->push_back(TAG_PURPOSE, KM_PURPOSE_DECRYPT);
        auth_set->push_back(TAG_PADDING, KM_PAD_NONE);
        auth_set->push_back(TAG_PADDING, KM_PAD_RSA_PKCS1_1_5_SIGN);
        auth_set->push_back(TAG_PADDING, KM_PAD_RSA_PKCS1_1_5_ENCRYPT);
        auth_set->push_back(TAG_PADDING, KM_PAD_RSA_PSS);
        auth_set->push_back(TAG_PADDING, KM_PAD_RSA_OAEP);
    }
}

static keymaster_key_characteristics_t* 
BuildCharacteristics(const AuthorizationSet& hw_enforced,
                     const AuthorizationSet& sw_enforced) {
    keymaster_key_characteristics_t* characteristics =
        reinterpret_cast<keymaster_key_characteristics_t*>(
            malloc(sizeof(keymaster_key_characteristics_t)));
    if (characteristics) {
        hw_enforced.CopyToParamSet(&characteristics->hw_enforced);
        sw_enforced.CopyToParamSet(&characteristics->sw_enforced);
    }
    return characteristics;
}

template <typename RequestType>
static void
AddClientAndAppData(const keymaster_blob_t* client_id,
					const keymaster_blob_t* app_data,
                    RequestType* request) {
    request->additional_params.Clear();
    if (client_id)
        request->additional_params.push_back(TAG_APPLICATION_ID, *client_id);
    if (app_data)
        request->additional_params.push_back(TAG_APPLICATION_DATA, *app_data);
}

static inline KeymasterHal* 
convert_device(const keymaster1_device_t* dev) {
    return reinterpret_cast<KeymasterHal*>(const_cast<keymaster1_device_t*>(dev));
}

KeymasterHal::KeymasterHal()
    : impl_(new AndroidKeymaster()) {
}

keymaster1_device_t*
KeymasterHal::keymaster_device() {
    return &device_;
}

int KeymasterHal::close_device(hw_device_t* dev) {
    delete reinterpret_cast<KeymasterHal*>(dev);
    return 0;
}

int KeymasterHal::generate_keypair(const keymaster1_device_t* dev,
                                          const keymaster_keypair_t key_type,
                                          const void* key_params, uint8_t** key_blob,
                                          size_t* key_blob_length) {
    LOG_D("Device received generate_keypair");
    if (!dev || !key_params)
        return KM_ERROR_UNEXPECTED_NULL_POINTER;

    if (!key_blob || !key_blob_length)
        return KM_ERROR_OUTPUT_PARAMETER_NULL;

    GenerateKeyRequest req;

    switch (key_type) {
    case TYPE_RSA: {
        req.key_description.push_back(TAG_ALGORITHM, KM_ALGORITHM_RSA);
        StoreDefaultNewKeyParams(KM_ALGORITHM_RSA, &req.key_description);
        const keymaster_rsa_keygen_params_t* rsa_params =
            static_cast<const keymaster_rsa_keygen_params_t*>(key_params);
        LOG_D("Generating RSA pair, modulus size: %u, public exponent: %lu",
              rsa_params->modulus_size, rsa_params->public_exponent);
        req.key_description.push_back(TAG_KEY_SIZE, rsa_params->modulus_size);
        req.key_description.push_back(TAG_RSA_PUBLIC_EXPONENT, rsa_params->public_exponent);
        break;
    }

    case TYPE_EC: {
        req.key_description.push_back(TAG_ALGORITHM, KM_ALGORITHM_EC);
        StoreDefaultNewKeyParams(KM_ALGORITHM_EC, &req.key_description);
        const keymaster_ec_keygen_params_t* ec_params =
            static_cast<const keymaster_ec_keygen_params_t*>(key_params);
        LOG_D("Generating ECDSA pair, key size: %u", ec_params->field_size);
        req.key_description.push_back(TAG_KEY_SIZE, ec_params->field_size);
        break;
    }

    default:
        LOG_D("Received request for unsuported key type %d", key_type);
        return KM_ERROR_UNSUPPORTED_ALGORITHM;
    }

    GenerateKeyResponse rsp;
    convert_device(dev)->impl_->GenerateKey(req, &rsp);
    if (rsp.error != KM_ERROR_OK) {
        LOG_E("Key generation failed with error: %d", rsp.error);
        return rsp.error;
    }

    *key_blob_length = rsp.key_blob.key_material_size;
    *key_blob = static_cast<uint8_t*>(malloc(*key_blob_length));
    if (!*key_blob) {
        LOG_E("Failed to allocate %d bytes", *key_blob_length);
        return KM_ERROR_MEMORY_ALLOCATION_FAILED;
    }
    memcpy(*key_blob, rsp.key_blob.key_material, *key_blob_length);
    LOG_D("Returning %d bytes in key blob\n", (int)*key_blob_length);

    return KM_ERROR_OK;
}

int KeymasterHal::import_keypair(const keymaster1_device_t* dev, const uint8_t* key,
                                        const size_t key_length, uint8_t** key_blob,
                                        size_t* key_blob_length) {
    LOG_D("Device received import_keypair");

    if (!dev || !key)
        return KM_ERROR_UNEXPECTED_NULL_POINTER;

    if (!key_blob || !key_blob_length)
        return KM_ERROR_OUTPUT_PARAMETER_NULL;

    ImportKeyRequest request;
    keymaster_algorithm_t algorithm;
    keymaster_error_t err = GetPkcs8KeyAlgorithm(key, key_length, &algorithm);
    if (err != KM_ERROR_OK)
        return err;
    request.key_description.push_back(TAG_ALGORITHM, algorithm);
    StoreDefaultNewKeyParams(algorithm, &request.key_description);
    request.SetKeyMaterial(key, key_length);
    request.key_format = KM_KEY_FORMAT_PKCS8;

    ImportKeyResponse response;
    convert_device(dev)->impl_->ImportKey(request, &response);
    if (response.error != KM_ERROR_OK) {
        LOG_E("Key import failed with error: %d", response.error);
        return response.error;
    }

    *key_blob_length = response.key_blob.key_material_size;
    *key_blob = static_cast<uint8_t*>(malloc(*key_blob_length));
    if (!*key_blob) {
        LOG_E("Failed to allocate %d bytes", *key_blob_length);
        return KM_ERROR_MEMORY_ALLOCATION_FAILED;
    }
    memcpy(*key_blob, response.key_blob.key_material, *key_blob_length);
    LOG_D("Returning %d bytes in key blob\n", (int)*key_blob_length);

    return KM_ERROR_OK;
}

/* static */
keymaster_error_t KeymasterHal::GetPkcs8KeyAlgorithm(const uint8_t* key, size_t key_length,
                                                            keymaster_algorithm_t* algorithm) {
    if (key == NULL) {
        LOG_E("No key specified for import");
        return KM_ERROR_UNEXPECTED_NULL_POINTER;
    }

    UniquePtr<PKCS8_PRIV_KEY_INFO, PKCS8_PRIV_KEY_INFO_Delete> pkcs8(
        d2i_PKCS8_PRIV_KEY_INFO(NULL, &key, key_length));
    if (pkcs8.get() == NULL) {
        LOG_E("Could not parse PKCS8 key blob");
        return KM_ERROR_INVALID_KEY_BLOB;
    }

    UniquePtr<EVP_PKEY, EVP_PKEY_Delete> pkey(EVP_PKCS82PKEY(pkcs8.get()));
    if (pkey.get() == NULL) {
        LOG_E("Could not extract key from PKCS8 key blob");
        return KM_ERROR_INVALID_KEY_BLOB;
    }

    switch (EVP_PKEY_type(pkey->type)) {
    case EVP_PKEY_RSA:
        *algorithm = KM_ALGORITHM_RSA;
        break;
    case EVP_PKEY_EC:
        *algorithm = KM_ALGORITHM_EC;
        break;
    default:
        LOG_E("Unsupported algorithm %d", EVP_PKEY_type(pkey->type));
        return KM_ERROR_UNSUPPORTED_ALGORITHM;
    }

    return KM_ERROR_OK;
}

/* static */
int KeymasterHal::get_keypair_public(const struct keymaster1_device* dev,
                                            const uint8_t* key_blob, const size_t key_blob_length,
                                            uint8_t** x509_data, size_t* x509_data_length) {
    LOG_D("Device received get_keypair_public");

    if (!dev || !key_blob)
        return KM_ERROR_UNEXPECTED_NULL_POINTER;

    if (!x509_data || !x509_data_length)
        return KM_ERROR_OUTPUT_PARAMETER_NULL;

    ExportKeyRequest req;
    req.SetKeyMaterial(key_blob, key_blob_length);
    req.key_format = KM_KEY_FORMAT_X509;

    ExportKeyResponse rsp;
    convert_device(dev)->impl_->ExportKey(req, &rsp);
    if (rsp.error != KM_ERROR_OK) {
        LOG_E("get_keypair_public failed with error: %d", rsp.error);
        return rsp.error;
    }

    *x509_data_length = rsp.key_data_length;
    *x509_data = static_cast<uint8_t*>(malloc(*x509_data_length));
    if (!*x509_data)
        return KM_ERROR_MEMORY_ALLOCATION_FAILED;
    memcpy(*x509_data, rsp.key_data, *x509_data_length);
    LOG_D("Returning %d bytes in x509 key\n", (int)*x509_data_length);

    return KM_ERROR_OK;
}

int KeymasterHal::sign_data(const keymaster1_device_t* dev, const void* params,
                                   const uint8_t* key_blob, const size_t key_blob_length,
                                   const uint8_t* data, const size_t data_length,
                                   uint8_t** signed_data, size_t* signed_data_length) {
    LOG_D("Device received sign_data");

    if (!dev || !params || !key_blob ||!data || !data_length)
        return KM_ERROR_UNEXPECTED_NULL_POINTER;

    if (!signed_data || !signed_data_length)
        return KM_ERROR_OUTPUT_PARAMETER_NULL;

    *signed_data_length = 0;

    BeginOperationRequest begin_request;
    begin_request.purpose = KM_PURPOSE_SIGN;
    begin_request.SetKeyMaterial(key_blob, key_blob_length);
    begin_request.additional_params.push_back(TAG_DIGEST, KM_DIGEST_NONE);
    begin_request.additional_params.push_back(TAG_PADDING, KM_PAD_NONE);

    BeginOperationResponse begin_response;
    convert_device(dev)->impl_->BeginOperation(begin_request, &begin_response);
    if (begin_response.error != KM_ERROR_OK) {
        LOG_E("sign_data begin operation failed with error: %d", begin_response.error);
        return begin_response.error;
    }

    UpdateOperationRequest update_request;
    update_request.op_handle = begin_response.op_handle;
    update_request.input.Reinitialize(data, data_length);
    UpdateOperationResponse update_response;
    convert_device(dev)->impl_->UpdateOperation(update_request, &update_response);
    if (update_response.error != KM_ERROR_OK) {
        LOG_E("sign_data update operation failed with error: %d", update_response.error);
        return update_response.error;
    }

    FinishOperationRequest finish_request;
    finish_request.op_handle = begin_response.op_handle;
    FinishOperationResponse finish_response;
    convert_device(dev)->impl_->FinishOperation(finish_request, &finish_response);
    if (finish_response.error != KM_ERROR_OK) {
        LOG_E("sign_data finish operation failed with error: %d", finish_response.error);
        return finish_response.error;
    }

    *signed_data_length = finish_response.output.available_read();
    *signed_data = static_cast<uint8_t*>(malloc(*signed_data_length));
    if (!*signed_data)
        return KM_ERROR_MEMORY_ALLOCATION_FAILED;
    if (!finish_response.output.read(*signed_data, *signed_data_length))
        return KM_ERROR_UNKNOWN_ERROR;
    return KM_ERROR_OK;
}

int KeymasterHal::verify_data(const keymaster1_device_t* dev, const void* params,
                                     const uint8_t* key_blob, const size_t key_blob_length,
                                     const uint8_t* signed_data, const size_t signed_data_length,
                                     const uint8_t* signature, const size_t signature_length) {
    LOG_D("Device received verify_data");

    if (!dev || !params || !key_blob || !signed_data || !signature)
        return KM_ERROR_UNEXPECTED_NULL_POINTER;

    BeginOperationRequest begin_request;
    begin_request.purpose = KM_PURPOSE_VERIFY;
    begin_request.SetKeyMaterial(key_blob, key_blob_length);
    begin_request.additional_params.push_back(TAG_DIGEST, KM_DIGEST_NONE);
    begin_request.additional_params.push_back(TAG_PADDING, KM_PAD_NONE);

    BeginOperationResponse begin_response;
    convert_device(dev)->impl_->BeginOperation(begin_request, &begin_response);
    if (begin_response.error != KM_ERROR_OK) {
        LOG_E("verify_data begin operation failed with error: %d", begin_response.error);
        return begin_response.error;
    }

    UpdateOperationRequest update_request;
    update_request.op_handle = begin_response.op_handle;
    update_request.input.Reinitialize(signed_data, signed_data_length);
    UpdateOperationResponse update_response;
    convert_device(dev)->impl_->UpdateOperation(update_request, &update_response);
    if (update_response.error != KM_ERROR_OK) {
        LOG_E("verify_data update operation failed with error: %d", update_response.error);
        return update_response.error;
    }

    FinishOperationRequest finish_request;
    finish_request.op_handle = begin_response.op_handle;
    finish_request.signature.Reinitialize(signature, signature_length);
    FinishOperationResponse finish_response;
    convert_device(dev)->impl_->FinishOperation(finish_request, &finish_response);
    if (finish_response.error != KM_ERROR_OK) {
        LOG_E("verify_data finish operation failed with error: %d", finish_response.error);
        return finish_response.error;
    }
    return KM_ERROR_OK;
}

keymaster_error_t KeymasterHal::get_supported_algorithms(const keymaster1_device_t* dev,
                                                                keymaster_algorithm_t** algorithms,
                                                                size_t* algorithms_length) {
    if (!dev)
        return KM_ERROR_UNEXPECTED_NULL_POINTER;

    if (!algorithms || !algorithms_length)
        return KM_ERROR_OUTPUT_PARAMETER_NULL;

    SupportedAlgorithmsRequest request;
    SupportedAlgorithmsResponse response;
    convert_device(dev)->impl_->SupportedAlgorithms(request, &response);
    if (response.error != KM_ERROR_OK) {
        LOG_E("get_supported_algorithms failed with %d", response.error);

        return response.error;
    }

    *algorithms_length = response.results_length;
    *algorithms =
        reinterpret_cast<keymaster_algorithm_t*>(malloc(*algorithms_length * sizeof(**algorithms)));
    if (!*algorithms)
        return KM_ERROR_MEMORY_ALLOCATION_FAILED;
    std::copy(response.results, response.results + response.results_length, *algorithms);

    return KM_ERROR_OK;
}

keymaster_error_t KeymasterHal::get_supported_block_modes(const keymaster1_device_t* dev,
                                                                 keymaster_algorithm_t algorithm,
                                                                 keymaster_purpose_t purpose,
                                                                 keymaster_block_mode_t** modes,
                                                                 size_t* modes_length) {
    if (!dev)
        return KM_ERROR_UNEXPECTED_NULL_POINTER;

    if (!modes || !modes_length)
        return KM_ERROR_OUTPUT_PARAMETER_NULL;

    SupportedBlockModesRequest request;
    request.algorithm = algorithm;
    request.purpose = purpose;
    SupportedBlockModesResponse response;
    convert_device(dev)->impl_->SupportedBlockModes(request, &response);

    if (response.error != KM_ERROR_OK) {
        LOG_E("get_supported_block_modes failed with %d", response.error);

        return response.error;
    }

    *modes_length = response.results_length;
    *modes = reinterpret_cast<keymaster_block_mode_t*>(malloc(*modes_length * sizeof(**modes)));
    if (!*modes)
        return KM_ERROR_MEMORY_ALLOCATION_FAILED;
    std::copy(response.results, response.results + response.results_length, *modes);
    return KM_ERROR_OK;
}

keymaster_error_t KeymasterHal::get_supported_padding_modes(const keymaster1_device_t* dev,
                                                                   keymaster_algorithm_t algorithm,
                                                                   keymaster_purpose_t purpose,
                                                                   keymaster_padding_t** modes,
                                                                   size_t* modes_length) {
    if (!dev)
        return KM_ERROR_UNEXPECTED_NULL_POINTER;

    if (!modes || !modes_length)
        return KM_ERROR_OUTPUT_PARAMETER_NULL;

    SupportedPaddingModesRequest request;
    request.algorithm = algorithm;
    request.purpose = purpose;
    SupportedPaddingModesResponse response;
    convert_device(dev)->impl_->SupportedPaddingModes(request, &response);

    if (response.error != KM_ERROR_OK) {
        LOG_E("get_supported_padding_modes failed with %d", response.error);
        return response.error;
    }

    *modes_length = response.results_length;
    *modes = reinterpret_cast<keymaster_padding_t*>(malloc(*modes_length * sizeof(**modes)));
    if (!*modes)
        return KM_ERROR_MEMORY_ALLOCATION_FAILED;
    std::copy(response.results, response.results + response.results_length, *modes);
    return KM_ERROR_OK;
}

keymaster_error_t KeymasterHal::get_supported_digests(const keymaster1_device_t* dev,
                                                             keymaster_algorithm_t algorithm,
                                                             keymaster_purpose_t purpose,
                                                             keymaster_digest_t** digests,
                                                             size_t* digests_length) {
    if (!dev)
        return KM_ERROR_UNEXPECTED_NULL_POINTER;

    if (!digests || !digests_length)
        return KM_ERROR_OUTPUT_PARAMETER_NULL;

    SupportedDigestsRequest request;
    request.algorithm = algorithm;
    request.purpose = purpose;
    SupportedDigestsResponse response;
    convert_device(dev)->impl_->SupportedDigests(request, &response);

    if (response.error != KM_ERROR_OK) {
        LOG_E("get_supported_digests failed with %d", response.error);
        return response.error;
    }

    *digests_length = response.results_length;
    *digests = reinterpret_cast<keymaster_digest_t*>(malloc(*digests_length * sizeof(**digests)));
    if (!*digests)
        return KM_ERROR_MEMORY_ALLOCATION_FAILED;
    std::copy(response.results, response.results + response.results_length, *digests);
    return KM_ERROR_OK;
}

keymaster_error_t KeymasterHal::get_supported_import_formats(
    const keymaster1_device_t* dev, keymaster_algorithm_t algorithm,
    keymaster_key_format_t** formats, size_t* formats_length) {
    if (!dev)
        return KM_ERROR_UNEXPECTED_NULL_POINTER;

    if (!formats || !formats_length)
        return KM_ERROR_OUTPUT_PARAMETER_NULL;

    SupportedImportFormatsRequest request;
    request.algorithm = algorithm;
    SupportedImportFormatsResponse response;
    convert_device(dev)->impl_->SupportedImportFormats(request, &response);

    if (response.error != KM_ERROR_OK) {
        LOG_E("get_supported_import_formats failed with %d", response.error);
        return response.error;
    }

    *formats_length = response.results_length;
    *formats =
        reinterpret_cast<keymaster_key_format_t*>(malloc(*formats_length * sizeof(**formats)));
    if (!*formats)
        return KM_ERROR_MEMORY_ALLOCATION_FAILED;
    std::copy(response.results, response.results + response.results_length, *formats);
    return KM_ERROR_OK;
}

keymaster_error_t KeymasterHal::get_supported_export_formats(
    const keymaster1_device_t* dev, keymaster_algorithm_t algorithm,
    keymaster_key_format_t** formats, size_t* formats_length) {
    if (!dev)
        return KM_ERROR_UNEXPECTED_NULL_POINTER;

    if (!formats || !formats_length)
        return KM_ERROR_OUTPUT_PARAMETER_NULL;

    SupportedExportFormatsRequest request;
    request.algorithm = algorithm;
    SupportedExportFormatsResponse response;
    convert_device(dev)->impl_->SupportedExportFormats(request, &response);

    if (response.error != KM_ERROR_OK) {
        LOG_E("get_supported_export_formats failed with %d", response.error);
        return response.error;
    }

    *formats_length = response.results_length;
    *formats =
        reinterpret_cast<keymaster_key_format_t*>(malloc(*formats_length * sizeof(**formats)));
    if (!*formats)
        return KM_ERROR_MEMORY_ALLOCATION_FAILED;
    std::copy(response.results, response.results + *formats_length, *formats);
    return KM_ERROR_OK;
}

keymaster_error_t KeymasterHal::add_rng_entropy(const keymaster1_device_t* dev,
                                                       const uint8_t* data, size_t data_length) {
    if (!dev)
        return KM_ERROR_UNEXPECTED_NULL_POINTER;

    AddEntropyRequest request;
    request.random_data.Reinitialize(data, data_length);
    AddEntropyResponse response;
    convert_device(dev)->impl_->AddRngEntropy(request, &response);
    if (response.error != KM_ERROR_OK)
        LOG_E("add_rng_entropy failed with %d", response.error);
    return response.error;
}

keymaster_error_t KeymasterHal::generate_key(
    const keymaster1_device_t* dev, const keymaster_key_param_set_t* params,
    keymaster_key_blob_t* key_blob, keymaster_key_characteristics_t** characteristics) {
    if (!dev || !params)
        return KM_ERROR_UNEXPECTED_NULL_POINTER;

    if (!key_blob)
        return KM_ERROR_OUTPUT_PARAMETER_NULL;

    GenerateKeyRequest request;
    request.key_description.Reinitialize(*params);

    GenerateKeyResponse response;
    convert_device(dev)->impl_->GenerateKey(request, &response);
    if (response.error != KM_ERROR_OK)
        return response.error;

    key_blob->key_material_size = response.key_blob.key_material_size;
    uint8_t* tmp = reinterpret_cast<uint8_t*>(malloc(key_blob->key_material_size));
    if (!tmp)
        return KM_ERROR_MEMORY_ALLOCATION_FAILED;
    memcpy(tmp, response.key_blob.key_material, response.key_blob.key_material_size);
    key_blob->key_material = tmp;

    if (characteristics) {
        *characteristics = BuildCharacteristics(response.enforced, response.unenforced);
        if (!*characteristics)
            return KM_ERROR_MEMORY_ALLOCATION_FAILED;
    }

    return KM_ERROR_OK;
}

keymaster_error_t KeymasterHal::get_key_characteristics(
    const keymaster1_device_t* dev, const keymaster_key_blob_t* key_blob,
    const keymaster_blob_t* client_id, const keymaster_blob_t* app_data,
    keymaster_key_characteristics_t** characteristics) {
    if (!dev || !key_blob || !key_blob->key_material)
        return KM_ERROR_UNEXPECTED_NULL_POINTER;

    if (!characteristics)
        return KM_ERROR_OUTPUT_PARAMETER_NULL;

    GetKeyCharacteristicsRequest request;
    request.SetKeyMaterial(*key_blob);
    AddClientAndAppData(client_id, app_data, &request);

    GetKeyCharacteristicsResponse response;
    convert_device(dev)->impl_->GetKeyCharacteristics(request, &response);
    if (response.error != KM_ERROR_OK)
        return response.error;

    *characteristics = BuildCharacteristics(response.enforced, response.unenforced);
    if (!*characteristics)
        return KM_ERROR_MEMORY_ALLOCATION_FAILED;
    return KM_ERROR_OK;
}

keymaster_error_t KeymasterHal::import_key(
    const keymaster1_device_t* dev, const keymaster_key_param_set_t* params,
    keymaster_key_format_t key_format, const keymaster_blob_t* key_data,
    keymaster_key_blob_t* key_blob, keymaster_key_characteristics_t** characteristics) {
    if (!params || !key_data)
        return KM_ERROR_UNEXPECTED_NULL_POINTER;

    if (!key_blob)
        return KM_ERROR_OUTPUT_PARAMETER_NULL;

    *characteristics = nullptr;

    ImportKeyRequest request;
    request.key_description.Reinitialize(*params);
    request.key_format = key_format;
    request.SetKeyMaterial(key_data->data, key_data->data_length);

    ImportKeyResponse response;
    convert_device(dev)->impl_->ImportKey(request, &response);
    if (response.error != KM_ERROR_OK)
        return response.error;

    key_blob->key_material_size = response.key_blob.key_material_size;
    key_blob->key_material = reinterpret_cast<uint8_t*>(malloc(key_blob->key_material_size));
    if (!key_blob->key_material)
        return KM_ERROR_MEMORY_ALLOCATION_FAILED;
    memcpy(const_cast<uint8_t*>(key_blob->key_material), response.key_blob.key_material,
           response.key_blob.key_material_size);

    if (characteristics) {
        *characteristics = BuildCharacteristics(response.enforced, response.unenforced);
        if (!*characteristics)
            return KM_ERROR_MEMORY_ALLOCATION_FAILED;
    }
    return KM_ERROR_OK;
}

keymaster_error_t KeymasterHal::export_key(const keymaster1_device_t* dev,
                                                  keymaster_key_format_t export_format,
                                                  const keymaster_key_blob_t* key_to_export,
                                                  const keymaster_blob_t* client_id,
                                                  const keymaster_blob_t* app_data,
                                                  keymaster_blob_t* export_data) {
    if (!key_to_export || !key_to_export->key_material)
        return KM_ERROR_UNEXPECTED_NULL_POINTER;

    if (!export_data)
        return KM_ERROR_OUTPUT_PARAMETER_NULL;

    export_data->data = nullptr;
    export_data->data_length = 0;

    ExportKeyRequest request;
    request.key_format = export_format;
    request.SetKeyMaterial(*key_to_export);
    AddClientAndAppData(client_id, app_data, &request);

    ExportKeyResponse response;
    convert_device(dev)->impl_->ExportKey(request, &response);
    if (response.error != KM_ERROR_OK)
        return response.error;

    export_data->data_length = response.key_data_length;
    uint8_t* tmp = reinterpret_cast<uint8_t*>(malloc(export_data->data_length));
    if (!tmp)
        return KM_ERROR_MEMORY_ALLOCATION_FAILED;
    memcpy(tmp, response.key_data, export_data->data_length);
    export_data->data = tmp;
    return KM_ERROR_OK;
}


keymaster_error_t KeymasterHal::begin(const keymaster1_device_t* dev,
                                             keymaster_purpose_t purpose,
                                             const keymaster_key_blob_t* key,
                                             const keymaster_key_param_set_t* in_params,
                                             keymaster_key_param_set_t* out_params,
                                             keymaster_operation_handle_t* operation_handle) {
    if (!key || !key->key_material)
        return KM_ERROR_UNEXPECTED_NULL_POINTER;

    if (!operation_handle)
        return KM_ERROR_OUTPUT_PARAMETER_NULL;

    if(out_params) {
        out_params->params = nullptr;
        out_params->length = 0;
    }

    BeginOperationRequest request;
    request.purpose = purpose;
    request.SetKeyMaterial(*key);
    request.additional_params.Reinitialize(*in_params);

    BeginOperationResponse response;
    convert_device(dev)->impl_->BeginOperation(request, &response);
    if (response.error != KM_ERROR_OK)
        return response.error;

    if (response.output_params.size() > 0 && out_params)
        response.output_params.CopyToParamSet(out_params);

    *operation_handle = response.op_handle;
    return KM_ERROR_OK;
}

keymaster_error_t KeymasterHal::update(const keymaster1_device_t* dev,
                                              keymaster_operation_handle_t operation_handle,
                                              const keymaster_key_param_set_t* in_params,
                                              const keymaster_blob_t* input, size_t* input_consumed,
                                              keymaster_key_param_set_t* out_params,
                                              keymaster_blob_t* output) {
    if (!input)
        return KM_ERROR_UNEXPECTED_NULL_POINTER;

    if (!input_consumed)
        return KM_ERROR_OUTPUT_PARAMETER_NULL;

    if (out_params) {
        out_params->params = nullptr;
        out_params->length = 0;
    }

    if (output) {
        output->data = nullptr;
        output->data_length = 0;
    }

    UpdateOperationRequest request;
    request.op_handle = operation_handle;
    if (input)
        request.input.Reinitialize(input->data, input->data_length);
    if (in_params)
        request.additional_params.Reinitialize(*in_params);

    UpdateOperationResponse response;
    convert_device(dev)->impl_->UpdateOperation(request, &response);
    if (response.error != KM_ERROR_OK)
        return response.error;

    if (response.output_params.size() > 0 && out_params)
        response.output_params.CopyToParamSet(out_params);

    *input_consumed = response.input_consumed;

    if (output) {
        output->data_length = response.output.available_read();
        uint8_t* tmp = reinterpret_cast<uint8_t*>(malloc(output->data_length));
        if (!tmp)
            return KM_ERROR_MEMORY_ALLOCATION_FAILED;
        memcpy(tmp, response.output.peek_read(), output->data_length);
        output->data = tmp;
    }

    return KM_ERROR_OK;
}

keymaster_error_t KeymasterHal::finish(const keymaster1_device_t* dev,
                                              keymaster_operation_handle_t operation_handle,
                                              const keymaster_key_param_set_t* params,
                                              const keymaster_blob_t* signature,
                                              keymaster_key_param_set_t* out_params,
                                              keymaster_blob_t* output) {
    if (!output)
        return KM_ERROR_OUTPUT_PARAMETER_NULL;

    if (out_params) {
        out_params->params = nullptr;
        out_params->length = 0;
    }

    if (output) {
        output->data = nullptr;
        output->data_length = 0;
    }

    FinishOperationRequest request;
    request.op_handle = operation_handle;
    if (signature)
        request.signature.Reinitialize(signature->data, signature->data_length);

    if(params)
        request.additional_params.Reinitialize(*params);

    FinishOperationResponse response;
    convert_device(dev)->impl_->FinishOperation(request, &response);
    if (response.error != KM_ERROR_OK)
        return response.error;

    if (response.output_params.size() > 0 && out_params) {
        response.output_params.CopyToParamSet(out_params);
    }

    if(output) {
        output->data_length = response.output.available_read();
        uint8_t* tmp = reinterpret_cast<uint8_t*>(malloc(output->data_length));
        if (!tmp)
            return KM_ERROR_MEMORY_ALLOCATION_FAILED;
        memcpy(tmp, response.output.peek_read(), output->data_length);
        output->data = tmp;
    }

    return KM_ERROR_OK;
}

keymaster_error_t KeymasterHal::abort(const keymaster1_device_t* dev,
                                             keymaster_operation_handle_t operation_handle) {
    AbortOperationRequest request;
    request.op_handle = operation_handle;
    AbortOperationResponse response;
    convert_device(dev)->impl_->AbortOperation(request, &response);
    return response.error;
}

int KeymasterHal::open_keymaster_hal(const struct hw_module_t* hw_module,
											const char* name,
											struct hw_device_t** hw_device_out) {
    LOG_I("open keymaster device");

	KeymasterHal *inst = new KeymasterHal();
	keymaster1_device_t *keymaster_device = inst->keymaster_device();

	memset(keymaster_device, 0, sizeof(keymaster1_device_t));

    keymaster_device->common.tag = HARDWARE_DEVICE_TAG;
    keymaster_device->common.version = 1;
    keymaster_device->common.module = const_cast<hw_module_t*>(hw_module);
    keymaster_device->common.close = &close_device;

    keymaster_device->flags = KEYMASTER_BLOBS_ARE_STANDALONE | KEYMASTER_SUPPORTS_EC;
    keymaster_device->generate_keypair = generate_keypair;
    keymaster_device->import_keypair = import_keypair;
    keymaster_device->get_keypair_public = get_keypair_public;
    keymaster_device->delete_keypair = nullptr;
    keymaster_device->delete_all = nullptr;
    keymaster_device->sign_data = sign_data;
    keymaster_device->verify_data = verify_data;

    keymaster_device->get_supported_algorithms = get_supported_algorithms;
    keymaster_device->get_supported_block_modes = get_supported_block_modes;
    keymaster_device->get_supported_padding_modes = get_supported_padding_modes;
    keymaster_device->get_supported_digests = get_supported_digests;
    keymaster_device->get_supported_import_formats = get_supported_import_formats;
    keymaster_device->get_supported_export_formats = get_supported_export_formats;
    keymaster_device->add_rng_entropy = add_rng_entropy;
    keymaster_device->generate_key = generate_key;
    keymaster_device->get_key_characteristics = get_key_characteristics;
    keymaster_device->import_key = import_key;
    keymaster_device->export_key = export_key;
    keymaster_device->delete_key = nullptr;
    keymaster_device->delete_all_keys = nullptr;
    keymaster_device->begin = begin;
    keymaster_device->update = update;
    keymaster_device->finish = finish;
    keymaster_device->abort = abort;

    keymaster_device->context = NULL;

	*hw_device_out = &keymaster_device->common;

	LOG_I("open keymaster device.. done");
	return 0;
}

static struct hw_module_methods_t keymaster_hal_methods = {
	open : KeymasterHal::open_keymaster_hal
};

__attribute__((visibility("default")))struct keystore_module HAL_MODULE_INFO_SYM = {
    .common =
    {
        .tag = HARDWARE_MODULE_TAG,
        .module_api_version = KEYMASTER_MODULE_API_VERSION_1_0,
        .hal_api_version = HARDWARE_HAL_API_VERSION,
        .id = KEYSTORE_HARDWARE_MODULE_ID,
        .name = "Keymaster MTEE based HAL",
        .author = "Mediatek",
        .methods = &keymaster_hal_methods,
        .dso = 0,
        .reserved = {},
    }
};
