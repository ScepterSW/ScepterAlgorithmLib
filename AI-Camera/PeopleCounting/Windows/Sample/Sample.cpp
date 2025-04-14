#include <iostream>
#include <thread>
#include <memory>
#include <mutex>
#include <opencv2/opencv.hpp>
#include "Scepter_api.h"
#include "Scepter_Morph_api.h"
#include "json.h"

using namespace std;
using namespace cv;

struct ScAIResult_
{
    uint32_t resultIndex;       //!< The index of the AI result.
    uint64_t resultTimestamp;   //!< The timestamp(in milliseconds) when the AI result be generated on the device AI mode.
    vector<uint8_t> resultData;
};

class ProcessAIResult
{
public:
    ProcessAIResult() :
        mIsRunning(true),
        mAIResult{ 0 },
        mMutex()
    {}

    void Run(const ScDeviceHandle& deviceHandle)
    {
        while (true == mIsRunning)
        {
            ScAIResult aiResult = { 0 };
            ScStatus status = scAIModuleGetResult(deviceHandle, 1000, &aiResult);
            if (status != SC_OK)
            {
                cout << " scAIModuleGetResult failed status:" << status << endl;
                continue;
            }
            else
            {
                lock_guard<mutex> lk(mMutex);
                mAIResult.resultIndex = aiResult.resultIndex;
                mAIResult.resultTimestamp = aiResult.resultTimestamp;
                mAIResult.resultData.resize(aiResult.dataLen);
                memcpy(mAIResult.resultData.data(), aiResult.pResultData, aiResult.dataLen);
            }
        }
    }

    ScAIResult_ GetResult()
    {
        lock_guard<mutex> lk(mMutex);
        return mAIResult;
    }

    void Stop()
    {
        mIsRunning = false;
    }

private:
    bool mIsRunning;
    ScAIResult_ mAIResult;
    mutex mMutex;
};

void ShowMenu(void);
bool InitDevice(ScDeviceHandle& deviceHandle);
void ShowAIResult(Mat img, const ScAIResult_& mAIResult);
static void Opencv_Depth(uint32_t slope, const ScFrame& frame, cv::Mat& dispImg);

int main(int argc, char *argv[])
{
    ScStatus status = scInitialize();
	if (status != ScStatus::SC_OK)
	{
		cout << "scInitialize failed!" << endl;
		system("pause");
		return -1;
	}

    ScDeviceHandle deviceHandle = nullptr;
	if (true != InitDevice(deviceHandle))
	{
		return -1;
	}
    ShowMenu();

    ProcessAIResult processAIResult;
    thread aiResultProcessThread([&processAIResult, deviceHandle]() {
        processAIResult.Run(deviceHandle);
    });

    bool isRunning = true;
    while (true == isRunning)
	{
		ScFrame depthFrame = { 0 };

		// Read one frame before call ScGetFrame
		ScFrameReady frameReady = {0};
		status = scGetFrameReady(deviceHandle, 1200, &frameReady);
		
		//Get depth frame, depth frame only output in following data mode
		if (1 == frameReady.depth)
		{
			status = scGetFrame(deviceHandle, SC_DEPTH_FRAME, &depthFrame);

			if (depthFrame.pFrameData != NULL)
			{
                static int index = 0;
                static float fps = 0;
                static int64 start = cv::getTickCount();

                int64 current = cv::getTickCount();
                int64 diff = current - start;
                index++;
                if (diff > cv::getTickFrequency())
                {
                    fps = index * cv::getTickFrequency() / diff;
                    index = 0;
                    start = current;
                }

				//Display the Depth Image
                cv::Mat imageMat;
				Opencv_Depth(3000, depthFrame, imageMat);
                char text[30] = "";
                sprintf(text, "%.2f", fps);
                putText(imageMat, text, Point(0, 15), FONT_HERSHEY_PLAIN, 1, Scalar(255, 255, 255));
                ShowAIResult(imageMat, processAIResult.GetResult());
				cv::imshow("Show", imageMat);
			}
			else
			{
				cout << "scGetFrame SC_DEPTH_FRAME status:" << status << " pFrameData is NULL " << endl;
			}
		}

		unsigned char key = waitKey(1);
        switch (key)
        {
        case 'C':
        case 'c':
            {
                scAIModuleSetEnabled(deviceHandle, false);
                scAIModuleSetEnabled(deviceHandle, true);
            }
            break;
        case 'M':
        case 'm':
            {
                ShowMenu();
            }
            break;
        case 27://ESC
            isRunning = false;
            processAIResult.Stop();
            break;
        default:
            break;
        }
	}
    aiResultProcessThread.join();

	status = scStopStream(deviceHandle);
    cout << "scStopStream status: " << status << endl;

    status = scCloseDevice(&deviceHandle);
    cout << "CloseDevice status: " << status << endl;

    status = scShutdown();
    cout << "Shutdown status: " << status << endl;
	cv::destroyAllWindows();

    return 0;
}

bool InitDevice(ScDeviceHandle& deviceHandle)
{
    uint32_t deviceCount = 0;
    ScStatus status = scGetDeviceCount(&deviceCount, 3000);
    if (status != ScStatus::SC_OK)
    {
        cout << "scGetDeviceCount failed!" << endl;
        system("pause");
        return false;
    }
    cout << "Get device count: " << deviceCount << endl;
    if (0 == deviceCount)
    {
        cout << "scGetDeviceCount scans for 3000ms and then returns the device count is 0. Make sure the device is on the network before running the samples." << endl;
        system("pause");
        return false;
    }

    unique_ptr<ScDeviceInfo[]> ptrDeviceListInfo(new ScDeviceInfo[deviceCount]);
    status = scGetDeviceInfoList(deviceCount, ptrDeviceListInfo.get());
	deviceHandle = 0;
	status = scOpenDeviceBySN(ptrDeviceListInfo.get()[0].serialNumber, &deviceHandle);
	if (status != ScStatus::SC_OK)
	{
		cout << "OpenDevice failed!" << endl;
		system("pause");
		return false;
	}
    cout << "sn  ==  " << ptrDeviceListInfo.get()[0].serialNumber << endl;

    const int BufLen = 64;
	char fw[BufLen] = { 0 };
	scGetFirmwareVersion(deviceHandle, fw, BufLen);
	cout << "fw  ==  " << fw << endl;

    char* pAIModuleName = nullptr;
    uint16_t dataLen = 0;
    status = scAIModuleGetParam(deviceHandle, 1, (void**)&pAIModuleName, &dataLen);
    if (status == ScStatus::SC_OK)
    {
        cout << "AIModuleName  ==  " << pAIModuleName << endl;
    }

    char* pAIModuleVersion = nullptr;
    dataLen = 0;
    status = scAIModuleGetParam(deviceHandle, 0, (void**)&pAIModuleVersion, &dataLen);
    if (status == ScStatus::SC_OK)
    {
        cout << "AIModuleVersion  ==  " << pAIModuleVersion << endl;
    }

	scStartStream(deviceHandle);
    //Wait for the device to upload image data
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));

 	return true;
}

void ShowMenu(void)
{
    printf("Press following key to set corresponding feature:\n");
    printf("C/c: Resetting algorithm counts.\n");
    printf("M/m: Show the menu.\n");
    return;
}

static void Opencv_Depth(uint32_t slope, const ScFrame& frame, cv::Mat& dispImg)
{
	dispImg = cv::Mat(frame.height, frame.width, CV_16UC1, frame.pFrameData);
	dispImg.convertTo(dispImg, CV_8U, 255.0 / slope);
	applyColorMap(dispImg, dispImg, cv::COLORMAP_RAINBOW);
}

void ShowAIResult(Mat img, const ScAIResult_& mAIResult)
{
    static const int BUFLEN = 50;
    char temp[BUFLEN] = { 0 };

    const char* jsonStr = (const char*)mAIResult.resultData.data();
    Json::CharReaderBuilder reader;
    reader["collectComments"] = false;

    JSONCPP_STRING errs;
    std::istringstream in(jsonStr);
    Json::Value root;
    if (false == parseFromStream(reader, in, &root, &errs))
    {
        return;
    }
    Json::Value markJson = root["mark"];
    Json::Value titleJson = markJson["title"];

    //show title
    Json::Value titlePosJson = titleJson["origin"];
    Point titlePos(titlePosJson[0].asInt(), titlePosJson[1].asInt());
    sprintf(temp, "%s", titleJson["text"].asCString());
    cv::putText(img, temp, titlePos, cv::FONT_HERSHEY_SIMPLEX, 1, Scalar(255, 255, 255), 2, 9);

    //show label
    const Json::Value labelJson = markJson["label"];
    for (auto i = 0; i < labelJson.size(); i++)
    {
        const Json::Value labelJson_ = labelJson[i];
        Json::Value lablePosJson = labelJson_["origin"];
        Point lablePos(lablePosJson[0].asInt(), lablePosJson[1].asInt());
        sprintf(temp, "%s", labelJson_["text"].asCString());
        cv::putText(img, temp, lablePos, cv::FONT_HERSHEY_SIMPLEX, 1, Scalar(255, 255, 255), 2, 9);
    }

    //show line
    Json::Value lineJson = markJson["line"];
    for (auto i = 0; i < lineJson.size(); i++)
    {
        const Json::Value lineJson_ = lineJson[i];
        Point start(lineJson_[0].asInt(), lineJson_[1].asInt());
        Point end(lineJson_[2].asInt(), lineJson_[3].asInt());
        cv::line(img, start, end, Scalar(255, 255, 255), 1);
    }
}