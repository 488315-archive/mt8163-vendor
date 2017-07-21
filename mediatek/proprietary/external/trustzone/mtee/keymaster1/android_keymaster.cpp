/*
 * Copyright 2014 The Android Open Source Project
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

#include <keymaster/android_keymaster.h>
#include <assert.h>
#include <string.h>
#include <cstddef>
#include <UniquePtr.h>
#include <keymaster/android_keymaster_utils.h>
#include "keymaster_mtee_api.h"
#include <cutils/log.h>


namespace keymaster {

const uint8_t MAJOR_VER = 1;
const uint8_t MINOR_VER = 1;
const uint8_t SUBMINOR_VER = 0;

void AndroidKeymaster::GetVersion(const GetVersionRequest&, GetVersionResponse* rsp) {
    if (rsp == NULL)
        return;

    rsp->major_ver = MAJOR_VER;
    rsp->minor_ver = MINOR_VER;
    rsp->subminor_ver = SUBMINOR_VER;
    rsp->error = KM_ERROR_OK;
}

void AndroidKeymaster::SupportedAlgorithms(const SupportedAlgorithmsRequest& request,
                                           SupportedAlgorithmsResponse* response) {
	km_mtee_request(TZCMD_KEYMASTER_GET_SUPPORT_ALGORITHMS, request, response);
	ALOGE("SupportedAlgorithms:%d", response->error);
}

void AndroidKeymaster::SupportedBlockModes(const SupportedBlockModesRequest& request,
                                           SupportedBlockModesResponse* response) {
    km_mtee_request(TZCMD_KEYMASTER_GET_SUPPORT_BLOCK_MODES, request, response);
	ALOGE("SupportedBlockModes:%d", response->error);
}

void AndroidKeymaster::SupportedPaddingModes(const SupportedPaddingModesRequest& request,
                                             SupportedPaddingModesResponse* response) {
    km_mtee_request(TZCMD_KEYMASTER_GET_SUPPORT_PADDING_MODES, request, response);
	ALOGE("SupportedPaddingModes:%d", response->error);
}

void AndroidKeymaster::SupportedDigests(const SupportedDigestsRequest& request,
                                        SupportedDigestsResponse* response) {
    km_mtee_request(TZCMD_KEYMASTER_GET_SUPPORT_DIGESTS, request, response);
	ALOGE("SupportedDigests:%d", response->error);
}

void AndroidKeymaster::SupportedImportFormats(const SupportedImportFormatsRequest& request,
                                              SupportedImportFormatsResponse* response) {
    km_mtee_request(TZCMD_KEYMASTER_GET_SUPPORT_IMPORT_FORMATS, request, response);
	ALOGE("SupportedImportFormats:%d", response->error);
}

void AndroidKeymaster::SupportedExportFormats(const SupportedExportFormatsRequest& request,
                                              SupportedExportFormatsResponse* response) {
    km_mtee_request(TZCMD_KEYMASTER_GET_SUPPORT_EXPORT_FORMATS, request, response);
	ALOGE("SupportedExportFormats:%d", response->error);
}

void AndroidKeymaster::AddRngEntropy(const AddEntropyRequest& request,
                                     AddEntropyResponse* response) {
    km_mtee_request(TZCMD_KEYMASTER_ADD_RNG_ENTROPY, request, response);
	ALOGE("AddRngEntropy:%d", response->error);
}

void AndroidKeymaster::GenerateKey(const GenerateKeyRequest& request,
                                   GenerateKeyResponse* response) {
    km_mtee_request(TZCMD_KEYMASTER_GEN_KEY, request, response);
	ALOGE("GenerateKey:%d", response->error);
}

void AndroidKeymaster::GetKeyCharacteristics(const GetKeyCharacteristicsRequest& request,
                                             GetKeyCharacteristicsResponse* response) {
    km_mtee_request(TZCMD_KEYMASTER_GET_KEY_CHARACTERISTICS, request, response);
	ALOGE("GetKeyCharacteristics:%d", response->error);
}

void AndroidKeymaster::BeginOperation(const BeginOperationRequest& request,
                                      BeginOperationResponse* response) {
    km_mtee_request(TZCMD_KEYMASTER_BEGIN, request, response);
	ALOGE("BeginOperation:%d", response->error);
}

void AndroidKeymaster::UpdateOperation(const UpdateOperationRequest& request,
                                       UpdateOperationResponse* response) {
    km_mtee_request(TZCMD_KEYMASTER_UPDATE, request, response);
	ALOGE("UpdateOperation:%d", response->error);
}

void AndroidKeymaster::FinishOperation(const FinishOperationRequest& request,
                                       FinishOperationResponse* response) {
    km_mtee_request(TZCMD_KEYMASTER_FINISH, request, response);
	ALOGE("FinishOperation:%d", response->error);
}

void AndroidKeymaster::AbortOperation(const AbortOperationRequest& request,
                                      AbortOperationResponse* response) {
    km_mtee_request(TZCMD_KEYMASTER_ABORT, request, response);
	ALOGE("AbortOperation:%d", response->error);
}

void AndroidKeymaster::ExportKey(const ExportKeyRequest& request, ExportKeyResponse* response) {
    km_mtee_request(TZCMD_KEYMASTER_EXPORT_KEY, request, response);
	ALOGE("ExportKey:%d", response->error);
}

void AndroidKeymaster::ImportKey(const ImportKeyRequest& request, ImportKeyResponse* response) {
    km_mtee_request(TZCMD_KEYMASTER_IMPORT_KEY, request, response);
	ALOGE("ImportKey:%d", response->error);
}

void AndroidKeymaster::DeleteKey(const DeleteKeyRequest& request, DeleteKeyResponse* response) {
    //km_mtee_request(TZCMD_KEYMASTER_DELETE_KEY, request, response);
	response->error = KM_ERROR_OK;
	ALOGE("DeleteKey:%d", response->error);
}

void AndroidKeymaster::DeleteAllKeys(const DeleteAllKeysRequest& request, DeleteAllKeysResponse* response) {
    //km_mtee_request(TZCMD_KEYMASTER_DELETE_ALL_KEYS, request, response);
	response->error = KM_ERROR_OK;
	ALOGE("DeleteAllKeys:%d", response->error);
}
}  // namespace keymaster
