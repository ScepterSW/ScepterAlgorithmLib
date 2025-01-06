#pragma once

#ifdef ALG_EXPORT_ON
#ifdef _WIN32
#define ALG_API_EXPORT __declspec(dllexport)
#else
#define ALG_API_EXPORT __attribute__((visibility("default")))
#endif
#else
#ifdef _WIN32
#define ALG_API_EXPORT __declspec(dllimport)
#else
#define ALG_API_EXPORT __attribute__((visibility("default")))
#endif
#endif

#ifdef __cplusplus
#define ALG_C_API_EXPORT extern "C" ALG_API_EXPORT
#else
#define ALG_C_API_EXPORT ALG_API_EXPORT
#endif

#include <stdint.h>
#include "wrapperTypes.h"
#include "alg_define.h"
#include "Scepter_api.h"

/**
 * @brief Return status codes for all APIs.\n
 * 		  <code>VzRetOK = 0</code> means the API successfully completed its operation.\n
 * 		  All other codes indicate a device, parameter, or API usage error.
 */
enum class AlgStatus
{
        ALG_OK = 0,                                 //The function completed successfully.
        ALG_DEVICEPOINTERISNULL = -1,               //The device structure pointer is null.
        ALG_TIMEOUT = -2,                           //Capture the next image frame time out.
        ALG_UNMEASURED = -3,                        //The measurement was not triggered.
        ALG_BG_CALIBRATION_PARAMS_NOT_FOUND = -4,   //The background calibration parameter was not found.
        ALG_BG_BG_CALIBRATION_PARAMS_LOAD_NG = -5,  //The background calibration parameters were not loaded successfully.
        ALG_OTHERS = -255,                          //An unknown error occurred.
};

typedef void *AlgVolMeasureHandle;

#pragma pack(push, 1)

struct AlgResult
{
        VolResult result; // Information(Up to 20) about the person being identified from the im-age.Reference VzPeopleInfo.
        ScFrame frame;    // The Image information is used for debugging. Available only if Vz_PCSetShowImg is set to true, otherwise empty.
};
#pragma pack(pop)