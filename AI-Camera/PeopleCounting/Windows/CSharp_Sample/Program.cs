using System;
using System.Threading;
using Scepter_enums;
using Scepter_types;
using Scepter_api;
using System.Runtime.InteropServices;
using System.Collections.Generic;
using OpenCvSharp;
using System.Text.Json;

[StructLayout(LayoutKind.Sequential, Pack = 1)]
public struct AlgHeader
{
    /*
	//Start identifer:STX A L G
	pBuf[0] = 0x02; //STX
	pBuf[1] = 0x41; //A
	pBuf[2] = 0x4C; //L
	pBuf[3] = 0x47; //G
	uint32_t startIdentifer = 0x474C4102;
	*/
    public uint startIdentifer;
    public ushort sizeOfValidData;
    public byte checksum;
}

namespace PeopleCountingCameraGetResult
{
    using ScDeviceHandle = System.IntPtr;
    class Program
    {
        static int Main(string[] args)
        {
            Console.WriteLine("---PeopleCountingCameraGetResult---");

            ScepterAPI VNAPI = new ScepterAPI();
            UInt32 deviceCount = 0;
            ScDeviceHandle deviceHandle = new IntPtr();
            ScStatus status = ScStatus.SC_OTHERS;

            //Initialize the ScepterSDK.
            status = VNAPI.VN_Initialize();
            if (status == ScStatus.SC_OK)
            {
                Console.WriteLine("[VN_Initialize] success ScStatus:(" + status + ").");
            }
            else
            {
                Console.WriteLine("[VN_Initialize] fail ScStatus:(" + status + ").");
                Console.ReadKey(true);
                return 1;
            }

            //Get the count of devices.
            status = VNAPI.VN_GetDeviceCount(ref deviceCount, 3000);
            if (status == ScStatus.SC_OK)
            {
                Console.WriteLine("[VN_GetDeviceCount] success ScStatus:(" + status + ").");
            }
            else
            {
                Console.WriteLine("[VN_GetDeviceCount] fail ScStatus:(" + status + ").");
                Console.ReadKey(true);
                return 1;
            }
            if (0 == deviceCount)
            {
                Console.WriteLine("[VN_GetDeviceCount] scans for 3000ms and then returns the device count is 0. Make sure the device is on the network before running the samples.");
                Console.ReadKey(true);
                return 1;
            }

            ScDeviceInfo[] pDeviceListInfo = new ScDeviceInfo[deviceCount];
            status = VNAPI.VN_GetDeviceInfoList(deviceCount, pDeviceListInfo);
            if (status == ScStatus.SC_OK)
            {
                Console.WriteLine("[VN_GetDeviceInfoList] success status:(" + status + ").");
                if (ScConnectStatus.SC_CONNECTABLE != pDeviceListInfo[0].status)
                {
                    Console.WriteLine("connect status" + pDeviceListInfo[0].status + "The device state does not support connection.");
                    return 1;
                }
            }
            else
            {
                Console.WriteLine("[VN_GetDeviceInfoList] fail status:" + status);
                return 1;
            }

            Console.WriteLine("The first deviceInfo, <serialNumber>:" + pDeviceListInfo[0].serialNumber + ", <ip>:" + pDeviceListInfo[0].ip + ", <status>:" + pDeviceListInfo[0].status);

            status = VNAPI.VN_OpenDeviceBySN(pDeviceListInfo[0].serialNumber, ref deviceHandle);
            if (status == ScStatus.SC_OK)
            {
                Console.WriteLine("[VN_OpenDeviceBySN] success status:(" + status + ").");
            }
            else
            {
                Console.WriteLine("[VN_OpenDeviceBySN] fail status:(" + status + ").");
                return 1;
            }

            //Set the preview images enable:Depth and Color.
            status = VNAPI.VN_AIModuleSetPreviewFrameTypeEnabled(deviceHandle, ScFrameType.SC_DEPTH_FRAME, 1);
            if (status != ScStatus.SC_OK)
            {
                Console.WriteLine("[VN_AIModuleSetPreviewFrameTypeEnabled SC_DEPTH_FRAME] fail status:(" + status + ").");
                return 1;
            }

            //Start the algorithm result parsing thread
            DealThread dealThread = new DealThread(deviceHandle, ref VNAPI);
            Thread algResultThread = new Thread(dealThread.Deal);
            algResultThread.Start();

            //Start the data stream.
            status = VNAPI.VN_StartStream(deviceHandle);
            if (status == ScStatus.SC_OK)
            {
                Console.WriteLine("[VN_StartStream] success status:(" + status + ").");
            }
            else
            {
                Console.WriteLine("[VN_StartStream] fail status:(" + status + ").");
                return 1;
            }

            //Get the preview images and display them
            while (true)
            {
                ScFrame depthFrame = new ScFrame();
                ScFrameReady frameReady = new ScFrameReady();
                status = VNAPI.VN_GetFrameReady(deviceHandle, 1000, ref frameReady);
                if (status != ScStatus.SC_OK)
                {
                    Console.WriteLine("[VN_GetFrameReady] fail status:(" + status + ").");
                    continue;
                }

                if (1 == frameReady.depth)
                {
                    status = VNAPI.VN_GetFrame(deviceHandle, ScFrameType.SC_DEPTH_FRAME, ref depthFrame);
                    if (depthFrame.pFrameData != IntPtr.Zero)
                    {
                        Mat depthImg;
                        ProcessDepth(4096, depthFrame.height, depthFrame.width, depthFrame.pFrameData, out depthImg, dealThread.GetMarkData());
                        Cv2.ImShow("Img", depthImg);

                        int key = Cv2.WaitKey(10);
                        if (27 == key)//'ESC'
                        {
                            break;
                        }

                    }
                    else
                    {
                        Console.WriteLine("[VN_GetFrameReady] fail frameReady.color:(" + frameReady.color + "),frameReady.depth:(" + frameReady.depth + ").");
                    }
                }
            };

            Cv2.DestroyAllWindows();

            //Wait for algResultThread done.
            dealThread.SetRunning(false);
            dealThread.WaitForTestDone();
            algResultThread.Abort();

            //Stop capturing the image stream
            status = VNAPI.VN_StopStream(deviceHandle);
            if (status == ScStatus.SC_OK)
            {
                Console.WriteLine("[VN_StopStream] success status:(" + status + ").");
            }
            else
            {
                Console.WriteLine("[VN_StopStream] fail status:(" + status + ").");
                return 1;
            }

            //1.close device
            //2.SDK shutdown
            status = VNAPI.VN_CloseDevice(ref deviceHandle);
            if (status == ScStatus.SC_OK)
            {
                Console.WriteLine("[VN_CloseDevice] success status:(" + status + ").");
            }
            else
            {
                Console.WriteLine("[VN_CloseDevice] fail status:(" + status + ").");
                return 1;
            }
            status = VNAPI.VN_Shutdown();
            if (status == ScStatus.SC_OK)
            {
                Console.WriteLine("[VN_Shutdown] success status:(" + status + ").");
            }
            else
            {
                Console.WriteLine("[VN_Shutdown] fail status:(" + status + ").");
                return 1;
            }
            Console.WriteLine("---End---");

            return 0;
        }

        public static void ProcessDepth(uint slope, int height, int width, IntPtr pData, out Mat dispImg, MarkData markData)
        {
            dispImg = Mat.FromPixelData(height, width, MatType.CV_16UC1, pData);

            // Convert depth map to 8 bits and apply pseudo-color
            Mat temp = new Mat();
            dispImg.ConvertTo(temp, MatType.CV_8UC1, 255.0 / slope);
            Cv2.ApplyColorMap(temp, dispImg, ColormapTypes.Rainbow);
            temp.Dispose();

            if(markData != null)
            {
                //show title
                Cv2.PutText(dispImg, markData.Title.Text, new Point(markData.Title.Origin[0], markData.Title.Origin[1]), HersheyFonts.HersheySimplex, 1.0, Scalar.White);

                //show label
                if(markData.Label != null)
                {
                    foreach (var label in markData.Label)
                    {
                        Cv2.PutText(dispImg, label.Text, new Point(label.Origin[0], label.Origin[1]), HersheyFonts.HersheySimplex, 1.0, Scalar.White);
                    }
                }
                
                //show line
                for (int i = 0; i < markData.Line.Count; i++)
                {
                    Cv2.Line(dispImg, new Point(markData.Line[i][0], markData.Line[i][1]), new Point(markData.Line[i][2], markData.Line[i][3]), Scalar.White);
                }
            }
            
        }

        class DealThread
        {
            public DealThread(ScDeviceHandle device, ref ScepterAPI pVNAPI)
            {
                device_ = device;
                running_ = true;
                VNAPI = pVNAPI;
            }
            public void Deal()
            {
                lock (this)
                {
                    while (running_)
                    {
                        ScAIResult aiResult = new ScAIResult();
                        ScStatus status = VNAPI.VN_AIModuleGetResult(device_, 1200, ref aiResult);
                        if (status != ScStatus.SC_OK)
                        {
                            Console.WriteLine("[VN_AIModuleGetResult] fail status:(" + status + ").");
                            Thread.Sleep(5);
                            continue;
                        }

                        if (aiResult.pResultData == IntPtr.Zero || aiResult.dataLen == 0)
                        {
                            Console.WriteLine("The result is invalid.");
                            continue;
                        }

                        try
                        {
                            string jsonStr = Marshal.PtrToStringAnsi(aiResult.pResultData);
                            var data = JsonSerializer.Deserialize<RootObject>(jsonStr, new JsonSerializerOptions { PropertyNameCaseInsensitive = true });
                            markData = data.Mark;
                        }
                        catch (JsonException ex)
                        {
                            Console.WriteLine($"JSON Parsing Error: {ex.Message}");
                            Console.WriteLine($"StackTrace: {ex.StackTrace}");
                        }
                        catch (Exception ex)
                        {
                            Console.WriteLine($"Unexpected Error: {ex.Message}");
                        }

                    };
                    isTestDone_ = true;
                }
                return;
            }
            public bool WaitForTestDone()
            {
                while (!isTestDone_)
                {
                    Thread.Sleep(1000);
                }
                return true;
            }
            public void SetRunning(bool running)
            {
                running_ = running;
            }

            public MarkData GetMarkData()
            {
                return markData;
            }

            ScDeviceHandle device_;
            bool isTestDone_ = false;
            static ScepterAPI VNAPI;
            MarkData markData;
            bool running_;
        }
    }
    public class RootObject
    {
        public MarkData Mark { get; set; }
        public List<PersonInfo> PeopleInfo { get; set; }
        public StatisticsData Statistics { get; set; }
    }
    public class MarkData
    {
        public List<LabelData> Label { get; set; }
        public List<int[]> Line { get; set; }
        public TitleData Title { get; set; }
    }
    public class LabelData
    {
        public int[] Origin { get; set; }
        public string Text { get; set; }
    }

    public class TitleData
    {
        public int[] Origin { get; set; }
        public string Text { get; set; }
    }
    public class PersonInfo
    {
        public int Height { get; set; }
        public string Id { get; set; }
        public int[] PosInWorld { get; set; }
    }

    public class StatisticsData
    {
        public int In { get; set; }
        public int Out { get; set; }
    }
}
