#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <cutils/log.h>
#include <hardware/keymaster1.h>

extern "C" {
#include <tz_cross/ta_mem.h>
#include <uree/system.h>
#include <uree/mem.h>
#include <tz_cross/ta_keymaster.h>
#include <tz_cross/trustzone.h>
}

keymaster_error_t 
km_mtee_call(int cmd, uint8_t * input,size_t input_size, 
             uint8_t ** output, size_t * output_size) {
	int ret;
	uint32_t paramTypes;
    uint32_t output_handle;
	MTEEC_PARAM param[4] = {0};
    UREE_SESSION_HANDLE ta_keymaster_session;
    UREE_SESSION_HANDLE ta_mem_session;
    UREE_SHAREDMEM_PARAM shm_param;
    UREE_SHAREDMEM_HANDLE input_shm_handle = 0;
    UREE_SHAREDMEM_HANDLE output_shm_handle = 0;

	ret = UREE_CreateSession(TZ_TA_KEYMASTER_UUID, &ta_keymaster_session);
    if (ret != TZ_RESULT_SUCCESS) {
        ALOGE("fail to creat session %d\n", ret);
        return KM_ERROR_SECURE_HW_COMMUNICATION_FAILED;
    }

    ret = UREE_CreateSession(TZ_TA_MEM_UUID, &ta_mem_session);
    if (ret != TZ_RESULT_SUCCESS) {
        UREE_CloseSession(ta_keymaster_session);
        ALOGE("CreateSession Memory Session Error: %s\n", TZ_GetErrorString(ret));
        return KM_ERROR_SECURE_HW_COMMUNICATION_FAILED;
    }

    if(!input || !input_size) {
		uint8_t empty[4] = {0};
        input = empty;
        input_size = (uint32_t)sizeof(empty);
	}

    shm_param.buffer = input;
    shm_param.size = (uint32_t)input_size;

    ret = UREE_RegisterSharedmem(ta_mem_session, &input_shm_handle, &shm_param);
    if (ret != TZ_RESULT_SUCCESS) {
        UREE_CloseSession(ta_keymaster_session);
        UREE_CloseSession(ta_mem_session);
        ALOGE("RegisterInputSharedMemory Error: %s\n", TZ_GetErrorString(ret));
        return KM_ERROR_SECURE_HW_COMMUNICATION_FAILED;
    }

    paramTypes = TZ_ParamTypes3(TZPT_MEMREF_INPUT,   /* input  */
                                TZPT_VALUE_OUTPUT,   /* output_handle */
                                TZPT_VALUE_OUTPUT);  /* output_length */

    param[0].memref.handle = input_shm_handle;
    param[0].memref.offset = 0;
    param[0].memref.size = input_size;

	ALOGE("km_tz_call: <cmd:%d>, input:%p, inputsize:%d", cmd, input, input_size);

	ret = UREE_TeeServiceCall(ta_keymaster_session, cmd, paramTypes, param);

    UREE_UnregisterSharedmem(ta_mem_session, input_shm_handle);

    if (ret != TZ_RESULT_SUCCESS) {
		UREE_CloseSession(ta_keymaster_session);
        UREE_CloseSession(ta_mem_session);
        ALOGE("failed to excute command,ret %d\n", ret);
        return ret > 0 ? KM_ERROR_SECURE_HW_COMMUNICATION_FAILED : (keymaster_error_t)ret;
    }

    output_handle = param[1].value.a;
	*output_size = (size_t)param[2].value.a;
    *output = (uint8_t*)malloc(*output_size);
    if (!(*output)) {
		UREE_CloseSession(ta_keymaster_session);
        UREE_CloseSession(ta_mem_session);
        ALOGE("failed to allocate buffer,size %d\n", *output_size);
		return KM_ERROR_SECURE_HW_COMMUNICATION_FAILED;
	}

    shm_param.buffer = (void *)*output;
    shm_param.size = (uint32_t)*output_size;

    ret = UREE_RegisterSharedmem(ta_mem_session, &output_shm_handle, &shm_param);
    if (ret != TZ_RESULT_SUCCESS) {
        free(*output);
        *output = NULL;
        *output_size = 0;
        UREE_CloseSession(ta_keymaster_session);
        UREE_CloseSession(ta_mem_session);
        ALOGE("RegisterInputSharedMemory Error: %s\n", TZ_GetErrorString(ret));
        return KM_ERROR_SECURE_HW_COMMUNICATION_FAILED;
    }

    param[0].value.a = output_handle;
    param[1].memref.handle = output_shm_handle;
    param[1].memref.offset = 0;
    param[1].memref.size = *output_size;

    paramTypes = TZ_ParamTypes2(TZPT_VALUE_INPUT, TZPT_MEMREF_OUTPUT);

    ret = UREE_TeeServiceCall(ta_keymaster_session,
							  TZCMD_KEYMASTER_GET_OUTPUT_DATA,
							  paramTypes, param);

    UREE_UnregisterSharedmem(ta_mem_session, output_shm_handle);

    if (ret != TZ_RESULT_SUCCESS) {
        free(*output);
        *output = NULL;
        *output_size = 0;
		UREE_CloseSession(ta_keymaster_session);
        UREE_CloseSession(ta_mem_session);
        ALOGE("failed to excute command,ret %d\n", ret);
        return ret > 0 ? KM_ERROR_SECURE_HW_COMMUNICATION_FAILED : (keymaster_error_t)ret;
    }

    UREE_CloseSession(ta_keymaster_session);
    UREE_CloseSession(ta_mem_session);

	return KM_ERROR_OK;
}


