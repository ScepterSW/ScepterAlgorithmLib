#pragma once

#include "wrapperTypes.h"

/**
 * @brief       Initializes the API on the algorithm library. This function must be invoked before any other APIs.
 * @param[out]  pAlgHandle:    The pointer to the algorithm object.
 * @param[in]   deviceHandle:   The pointer to the camera device.
 * @param[in]   pDeviceInfo:    The pointer to the information of the camera device.
 * @param[in]   pAlgRootDir:    The pointer to the algorithm root directory.
 * @return      ::ALG_OK if the function succeeded, or one of the error values defined by ::AlgStatus.
 */
ALG_C_API_EXPORT AlgStatus algInitialize(AlgVolMeasureHandle *pAlgHandle, ScDeviceHandle deviceHandle, const ScDeviceInfo* pDeviceInfo, const char *pAlgRootDir);

/**
 * @brief       Shuts down the API on the algorithm library and clears all resources allocated by the API. After invoking this function, no other APIs can be invoked.
 * @param[in]   pAlgHandle:    The pointer to the algorithm object.
 * @return      ::ALG_OK if the function succeeded, or one of the error values defined by ::AlgStatus.
 */
ALG_C_API_EXPORT AlgStatus algShutdown(AlgVolMeasureHandle *pAlgHandle);

/**
 * @brief       Start the processing thread of the algorithm.
 * @param[in]   algHandle:    The handle to the algorithm object.
 * @return      ::ALG_OK if the function succeeded, or one of the error values defined by ::AlgStatus.
 */
ALG_C_API_EXPORT AlgStatus algStart(AlgVolMeasureHandle algHandle);

/**
 * @brief       Stop the processing thread of the algorithm.
 * @param[in]   algHandle:    The handle to the algorithm object.
 * @return      ::ALG_OK if the function succeeded, or one of the error values defined by ::AlgStatus.
 */
ALG_C_API_EXPORT AlgStatus algStop(AlgVolMeasureHandle algHandle);

/**
 * @brief        Set the parameters of the algorithm
 * @param[in]    algHandle:     The handle to the algorithm object.
 * @param[in]    param_id       The id of parameter
 * @param[in]    p_in_param     The pointer of the parameter buffer
 * @param[in]    param_len      The length of the parameter buffer
 * @return      ::ALG_OK if the function succeeded, or one of the error values defined by ::AlgStatus.
 */
ALG_C_API_EXPORT AlgStatus algSetParam(AlgVolMeasureHandle algHandle, uint32_t param_id, const char *p_in_param, uint16_t param_len);

/**
 * @brief        Get the parameters of the algorithm
 * @param[in]    algHandle:       The handle to the algorithm object.
 * @param[in]    param_id         The id of parameter
 * @param[out]   pp_out_param     The pointer of the parameter buffer
 * @param[out]   p_param_len      The length of the parameter buffer
 * @return      ::ALG_OK if the function succeeded, or one of the error values defined by ::AlgStatus.
 */
ALG_C_API_EXPORT AlgStatus algGetParam(AlgVolMeasureHandle algHandle, uint32_t param_id, const char **pp_out_param, uint16_t *p_param_len);

/**
 * @brief       Get the result of the algorithm.
 * @param[in]   algHandle:         The handle to the algorithm object.
 * @param[out]  pAlgResult:        The Pointer to a buffer in which to store the result of the algorithm.
 * @return:     ::ALG_OK if the function succeeded, or one of the error values defined by ::AlgStatus.
 */
ALG_C_API_EXPORT AlgStatus algGetResult(const AlgVolMeasureHandle algHandle, AlgResult *pAlgResult);

/**
 * @brief       Triggers one algorithm operation.
 * @param[in]   algHandle:         The handle to the algorithm object.
 * @return:     ::ALG_OK if the function succeeded, or one of the error values defined by ::AlgStatus.
 */
ALG_C_API_EXPORT AlgStatus algTrigger(const AlgVolMeasureHandle algHandle);

/**
 * @brief       Sets the state to save offline data.
 * @param[in]   algHandle:       The handle to the algorithm object.
 * @param[in]   isSaved:         Whether to save offline files: true: saved; false: no.
 * @return:     ::ALG_OK if the function succeeded, or one of the error values defined by ::AlgStatus.
 */
ALG_C_API_EXPORT AlgStatus algSetSaveOfflineDataState(const AlgVolMeasureHandle algHandle, bool isSaved);