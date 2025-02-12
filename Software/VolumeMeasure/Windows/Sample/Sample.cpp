#include <opencv2/opencv.hpp>
#include <thread>
#include <inttypes.h>
#include "wrapperAPI.h"
#include "Scepter_api.h"

using cv::Point;
using cv::Scalar;
using cv::Size;

void ShowMenu(void);
bool InitDevice(ScDeviceHandle &deviceHandle, ScDeviceInfo& deviceInfo);
void ShowInfo(const ScDeviceHandle &deviceHandle, const AlgResult &algResult, bool isSavingImg);
AlgStatus GetCalibrationState(const AlgVolMeasureHandle &algHandle, uint8_t &state);

int32_t main(int32_t argc, char *argv[])
{
    ScStatus status = scInitialize();
    if (status != ScStatus::SC_OK)
    {
        printf("scInitialize has failed:%d!\n", status);
        system("pause");
        return -1;
    }

    ScDeviceHandle deviceHandle = 0;
    ScDeviceInfo deviceInfo = { 0 };
OPEN:
    if (false == InitDevice(deviceHandle, deviceInfo))
    {
        std::this_thread::sleep_for(std::chrono::seconds(1));
        goto OPEN;
    }

    AlgVolMeasureHandle algHandle = nullptr;
    AlgStatus result = algInitialize(&algHandle, deviceHandle, &deviceInfo, "./");
    result = algStart(algHandle);
    
    bool isRunning = false;
    if (AlgStatus::ALG_OK != result)
    {
        printf("scInitialize has failed:%d.\n", result);
        system("pause");
        return -1;
    }
    else
    {
        isRunning = true;
    }

    ShowMenu();

    VolResult lastVolResult = { 0 };
    bool isSavingImg = false;
    while (isRunning)
    {
        AlgResult algResult = {0};

        result = algGetResult(algHandle, &algResult);
        if (AlgStatus::ALG_OK == result)
        {
            ShowInfo(deviceHandle, algResult, isSavingImg);
            lastVolResult = algResult.result;
        }
        else if (AlgStatus::ALG_UNMEASURED == result)
        {
            algResult.result = lastVolResult;
            ShowInfo(deviceHandle, algResult, isSavingImg);
        }
        else 
        {
            //do nothing
        }

        unsigned char key = cv::waitKey(1);
        switch (key)
        {
        case 'M':
        case 'm':
        {
            ShowMenu();
        }
        break;
        case 'R':
        case 'r':
        {
            isSavingImg = !isSavingImg;
            algSetSaveOfflineDataState(algHandle, isSavingImg);
            printf("%s saving offline data.\n", ((true == isSavingImg) ? "start" : "stop"));
        }
        break;
        case 'B':
        case 'b':
        {
            algSetParam(algHandle, PARAM_BG_CALIBRATION_START, nullptr, 0);
            bool isDone = false;
            std::thread t([&]
                          {
                              while (!isDone)
                              {
                                  uint8_t calibrationState = 0;
                                  if (AlgStatus::ALG_OK == GetCalibrationState(algHandle, calibrationState))
                                  {
                                      switch (calibrationState)
                                      {
                                      case 0:
                                          printf("The calibration is successful.\n");
                                          isDone = true;
                                          break;
                                      case 2:
                                          printf("The calibration is in progress.\n");
                                          isDone = false;
                                          break;
                                      default:
                                          printf("The calibration has failed:%d.\n", calibrationState);
                                          isDone = true;
                                          break;
                                      }
                                  }
                                  std::this_thread::sleep_for(std::chrono::milliseconds(33));
                              }
                          });
            t.detach();
        }
        break;
        case 'T':
        case 't':
        {
            algTrigger(algHandle);
        }
        break;
        case 27: // ESC
            isRunning = false;
            break;
        default:
            break;
        }
    }

    cv::destroyAllWindows();
    algStop(algHandle);
    algShutdown(&algHandle);

    status = scCloseDevice(&deviceHandle);
    printf("CloseDevice status:%d.\n", status);

    status = scShutdown();
    printf("Shutdown status:%d.\n", status);

    return 0;
}

void ShowMenu(void)
{
    printf("Press following key to set corresponding feature:\n");
    printf("T/t: Trigger a volume measurement.\n");
    printf("B/b: Recalibrate the background.\n");
    printf("R/r: Start or stop saving offline data.\n");
    printf("M/m: Show the menu.\n");
    return;
}

bool InitDevice(ScDeviceHandle &deviceHandle, ScDeviceInfo& deviceInfo)
{
    uint32_t deviceCount = 0;
    ScStatus status = scGetDeviceCount(&deviceCount, 3000);
    if (status != ScStatus::SC_OK)
    {
        printf("scGetDeviceCount failed!\n");
        system("pause");
        return false;
    }
    printf("Get device count:%d.\n", deviceCount);
    if (0 == deviceCount)
    {
        printf("scGetDeviceCount scans for 3000ms and then returns the device count is 0. Make sure the device is on the network before running the samples.\n");
        system("pause");
        return false;
    }

    ScDeviceInfo *pDeviceListInfos = new ScDeviceInfo[deviceCount];
    status = scGetDeviceInfoList(deviceCount, pDeviceListInfos);
    status = scOpenDeviceBySN(pDeviceListInfos[0].serialNumber, &deviceHandle);
    deviceInfo = pDeviceListInfos[0];
    delete[] pDeviceListInfos;
    pDeviceListInfos = nullptr;
    if (status != ScStatus::SC_OK)
    {
        printf("OpenDevice failed:%d!\n", status);
        system("pause");
        return false;
    }

    return true;
}

void ShowInfo(const ScDeviceHandle &deviceHandle, const AlgResult &algResult, bool isSavingImg)
{
    static const uint32_t BUFLEN = 100;
    char temp[BUFLEN] = {0};

    if (0 != algResult.frame.height && 0 != algResult.frame.width)
    {
        cv::Mat img = cv::Mat(algResult.frame.height, algResult.frame.width, CV_8UC3, algResult.frame.pFrameData);

        // show hint
        {
            std::sprintf(temp, "Please align the center of the object with the mark point.");
            cv::putText(img, temp, Point(0, 20), cv::FONT_HERSHEY_SIMPLEX, 0.6, Scalar(0, 255, 255), 1, 6);
            cv::drawMarker(img, Point(algResult.frame.width / 2, algResult.frame.height /2), Scalar(0, 255, 255), cv::MARKER_CROSS, 20, 2);
        }

        if (true == isSavingImg)
        {
            static const uint32_t BUFLEN = 50;
            std::sprintf(temp, "REC");
            cv::putText(img, temp, Point(algResult.frame.width - 40, 20), cv::FONT_HERSHEY_SIMPLEX, 0.6, Scalar(0, 0, 255), 2, 9);
        }

        if (0 != algResult.result.type)
        {
            std::string shape;
            Scalar resultColor;
            switch (algResult.result.type)
            {
            case 1:
                resultColor = Scalar(0, 255, 0);
                shape = "cube";
                break;
            case 2:
                resultColor = Scalar(0, 255, 255);
                shape = "irregular";
                break;
            default:
                resultColor = Scalar(255, 0, 0);
                shape = "unknown";
                break;
            }


            Point points[4] = { Point(algResult.result.point0.x, algResult.result.point0.y)
                               ,Point(algResult.result.point1.x, algResult.result.point1.y)
                               ,Point(algResult.result.point2.x, algResult.result.point2.y)
                               ,Point(algResult.result.point3.x, algResult.result.point3.y) };

            cv::line(img, points[0], points[1], resultColor, 2);
            cv::line(img, points[1], points[2], resultColor, 2);
            cv::line(img, points[2], points[3], resultColor, 2);
            cv::line(img, points[3], points[0], resultColor, 2);

            ::sprintf(temp, "L: %d mm", algResult.result.length);
            cv::putText(img, temp, Point(10, 40), cv::FONT_HERSHEY_DUPLEX, 0.7, resultColor);
            ::sprintf(temp, "W: %d mm", algResult.result.width);
            cv::putText(img, temp, Point(10, 60), cv::FONT_HERSHEY_DUPLEX, 0.7, resultColor);
            ::sprintf(temp, "H: %d mm", algResult.result.height);
            cv::putText(img, temp, Point(10, 80), cv::FONT_HERSHEY_DUPLEX, 0.7, resultColor);
            cv::putText(img, shape, Point(10, 100), cv::FONT_HERSHEY_DUPLEX, 0.7, resultColor);
        }
        cv::imshow("img", img);
    }    
}

AlgStatus GetCalibrationState(const AlgVolMeasureHandle &algHandle, uint8_t &state)
{
    uint16_t len = 0;
    const uint8_t *pCalibrationState = nullptr;
    AlgStatus ret = algGetParam(algHandle, PARAM_BG_CALIBRATION_STATE, (const char **)&pCalibrationState, &len);
    state = *pCalibrationState;
    return ret;
}