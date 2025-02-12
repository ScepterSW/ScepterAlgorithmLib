#pragma once
#include <functional>
#include <stdint.h>

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

// Specifies the type of image frame.
typedef enum FrameTypeEnum
{
    FRAME_DEPTH = 0,                               // Depth frame with 16 bits per pixel in millimeters.
    FRAME_IR = 1,                                  // IR frame with 8 bits per pixel.
    FRAME_RGB = 2,                                 // Color frame with 24 bits per pixel in RGB format.
    FRAME_TRANSFORM_DEPTH_IMG_TO_COLOR_SENSOR = 3, // Depth frame with 16 bits per pixel, in millimeters, that is transformed to color sensor
                                                   // space where the resolution is same as the color frame's resolution.
    FRAME_TRANSFORM_COLOR_IMG_TO_DEPTH_SENSOR = 4, // Color frame with 24 bits per pixel in RGB format, that is transformed to depth
                                                   // sensor space where the resolution is the same as the depth frame's resolution.

} FRAME_TYPE_E;

// Specifies the image pixel format.
typedef enum PixelFormatEnum
{
    PIXEL_FORMAT_DEPTH_MM16 = 0, // Depth image pixel format, 16 bits per pixel in mm.
    PIXEL_FORMAT_GRAY_8 = 2,     // Gray image pixel format, 8 bits per pixel
    PIXEL_FORMAT_RGB_888 = 5,    // Without compress, color image pixel format, 24 bits per pixel RGB format.
} PIXEL_FORMAT_E;

typedef enum AlgorithmReturnEnum
{
    ALGO_RET_OK = 0,
    ALGO_RET_FAIL = 1,
    ALGO_RET_CONFIG_FILE_NOT_FOUND = 2,
    ALGO_RET_CONFIG_FILE_LOAD_NG = 3,
    ALGO_RET_DETECTED_PLANE_NG = 4,
    ALGO_RET_BG_CALIBRATION_PARAMS_NOT_FOUND = 5,
    ALGO_RET_BG_CALIBRATION_PARAMS_LOAD_NG = 6,
    ALGO_RET_TOO_MUCH_DIFFERENCE_IN_FRAMES = 7,
    ALGO_RET_GET_FRAME_TIMEOUT = 8,
    ALGO_RET_ETHERNET_CALLBACK_NG = 9,
    ALGO_RET_RS485_CALLBACK_NG = 10,
    ALGO_RET_GPIO_CALLBACK_NG = 11,
    ALGO_RET_LED_CALLBACK_NG = 12,
    ALGO_RET_INPUT_PARAMS_INVALID = 13,
    ALGO_RET_NOT_INIT = 14,
    ALGO_RET_NOT_SUPPORT = 15,
    ALGO_RET_NOT_START = 16,
    ALGO_RET_SAVE_PARAMS_NG = 17,
    // TODO: more err_code ...
} ALGO_RET_E;

typedef enum AlgorithmStateEnum
{
    ALGO_STATE_RUNNING = 0,        //
    ALGO_STATE_ERROR = 1,          //
    ALGO_STATE_IDLE = 2,           //
    ALGO_STATE_BG_CALIBRATING = 3, //
    // TODO: more state ...
} ALGO_STATE_E;

typedef enum ParamID
{
    PARAM_VERSION = 0x00,                      // The version of the Algorithm library
    PARAM_STATE = 0x01,                        // The state of the Algorithm library
    PARAM_DATA_TYPE = 0x02,                    //
    PARAM_SHAPE_TYPE = 0x03,                   //
    PARAM_GROUND_HALF_SIZE = 0x04,             //
    PARAM_GROUND_PLANE_PARAMS = 0x05,          //
    PARAM_SEPARATE_THRESHOLD = 0x06,           //
    PARAM_CONNECTED_THRESHOLD = 0x07,          //
    PARAM_MOUNTING_ANGLE_THRESHOLD = 0x08,     //
    PARAM_OBJECT_POINTS_THRESHOLD = 0x09,      //
    PARAM_ROI = 0x0A,                          //
    PARAM_CONNECTED_BORDER_THRESHOLD = 0x0B,   //
    PARAM_CONTOUR_POINTS_THRESHOLD = 0x0C,     //
    PARAM_CANDIDATE_POINT_ANGLE = 0x0D,        //
    PARAM_CANDIDATE_POINTS_THRESHOLD = 0x0E,   //
    PARAM_DELETE_POINT_RATIO = 0x0F,           //
    PARAM_LINE_MEAN_DISTANCE_THRESHOLD = 0x10, //
    PARAM_CUBE_JUDGE_ANGLE = 0x11,             //
    PARAM_IRREGULAR_HEIGHT_MEAN_COUNT = 0x12,  //

    PARAM_BG_CALIBRATION_START = 0x30, // Start background calibration
    PARAM_BG_CALIBRATION_STATE = 0x31, // The state of background calibration
    PARAM_BG_THRESHOLD = 0x32,         // The Threshold when the input image is separated from the background
    PARAM_BG_ENABLE = 0x33,            // Sets the background image to enable or disable
    PARAM_BG_DELETE = 0x34,            // Delete the background calibration parameter
} PARAM_ID_E;

// value can be combined.eg:DEBUG_STATE|DEBUG_OBJECTINFO
typedef enum DataType
{
    DATA_NONE = 0x0,
    DATA_RESULT = 0x1,
    DATA_OBJECTINFO = 0x2,
    DATA_DEPTH = 0x4, // U16@320*240
    DATA_ALL = DATA_RESULT | DATA_OBJECTINFO | DATA_DEPTH,
} DATA_TYPE_E;

// The application type depends on the use of the safety camera sensor.
typedef enum ShapeType
{
    SHAPE_UNKNOWN = -1,
    SHAPE_NONE = 0,
    SHAPE_CUBE = 1,      // The position of the safety camera sensor is fixed. The safety camera sensor is used for hazardous area protection, hazardous point protection, or access protection.
    SHAPE_IRREGULAR = 2, // In mobile applications, the safety camera sensor is mounted on the AGV (auto‐matic guided vehicle). The safety camera sensor is used for mobile hazardous area protection, fall protection, or to protect against collisions.
    SHAPE_AUTO = SHAPE_CUBE | SHAPE_IRREGULAR,
    SHAPE_MAX = SHAPE_AUTO,
} SHAPE_TYPE_E;

#pragma pack(push, 1)

struct Point2U16
{
    uint16_t x;
    uint16_t y;
};

struct RectTLBT
{
    Point2U16 topLeft;
    Point2U16 bottomRight;
};

typedef struct Vect3s
{
    int16_t x;
    int16_t y;
    int16_t z;
} VECT3S_T;

typedef struct
{
    uint32_t x;
    uint32_t y;
    uint32_t z;
} MarkPoint;

typedef struct
{
    uint32_t length; // the length of the object: unit is mm
    uint32_t width;  // the width of the object: unit is mm
    uint32_t height; // the height of the object: unit is mm
    ShapeType type;  // the object type: 0 - cube; 1 - irregular
    MarkPoint point0;
    MarkPoint point1;
    MarkPoint point2;
    MarkPoint point3;
} VolResult;

typedef struct
{
    // ax + by + cz + d = 0 , this is the plane formula
    float fa;
    float fb;
    float fc;
    float fd;
} PlaneParams;

// image frame data
typedef struct CameraSingleFrame
{
    uint32_t frame_no;
    uint64_t timestamp;
    FRAME_TYPE_E frame_type;
    PIXEL_FORMAT_E pixel_format;
    uint32_t width;
    uint32_t height;
    uint32_t data_len;
    uint8_t *p_data;
} CAM_SINGLE_FRAME_T;

// intrinsic parameters and external parameters for tof lens and color lens
typedef struct LensParameters
{
    float tofIntrinsic[4];   // intrinsic parameters for tof lens: cx,cy,fx,fy
    float colorIntrinsic[4]; // intrinsic parameters for color lens:cx,cy,fx,fy
    float rotation[9];       // Orientation stored as an array of 9 float representing a 3x3 rotation matrix. The rotation matrix from ToF lens to color lens.
    float translation[3];    // Location stored as an array of 3 float representing a 3-D translation vector. The translation matrix from ToF lens to color lens.
} LENS_PARAMETERS_T;

// image frame data and lens's parameters
// When multiple frames of synchronized images are transmitted, the images are stored in image type (@FrameTypeEnum) order.
// For example, when both the color image and the transformed depth image are transferred,
// p_frame[0] holds the color image and p_frame[1] holds the transformed depth image.
typedef struct CameraJointFrame
{
    uint8_t frame_cnt;                 // the count of frames
    CAM_SINGLE_FRAME_T *p_frame;       // frame data array
    LENS_PARAMETERS_T lens_parameters; // the Parameters of Lens
} CAM_JOINT_FRAME_T;

typedef struct AlgorithmModuleState
{
    uint8_t state; // refer to AlgorithmStateEnum
    uint8_t errorCode;
} ALGO_MODULE_STATE_T;

// Initialization parameters of the algorithm
typedef struct AlgorithmInitParam
{
    /*The root directory of the algorithm module. The default root is /userdata/algorithm_data.
    A typical directory structure is as follows:

    ./
    |-- lib
    |   `-- libalg.so
    `-- config
        `-- safety.json
        `-- background.bin
    */
    char *p_alg_root_dir;

    using ALGO_PROCESS_CB_FUNC_T = std::function<void(uint64_t timestamp, const char *p_proc_result, uint32_t result_len)>;
    ALGO_PROCESS_CB_FUNC_T process_cb_func;
} ALGO_INIT_PARAM_T;

#pragma pack(pop)