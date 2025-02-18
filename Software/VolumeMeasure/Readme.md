## Volume Measurement Algorithm

The algorithm supports measuring the length, width, and height of cartons on a test plane.

### Supported Devices

- NYX Camera
  - NYX650 (NYX650_R_20241203_B26  and later )
- Vzense Camera
  - DS86 (DS86_R_20241206_B21 and later )
### Supported platforms
- Windows 10/11


### How to use PrecompiledSamples

1. Set up the camera, ensuring that the camera’s front panel is parallel to the test platform.

2. Switch to directory Software/VolumeMeasure/Windows/PrecompiledSamples/, double-click to start Sample.exe, and press B/b to perform background calibration.

   <img src="assets/calibration.png" style="zoom:50%;" />

3. After the background calibration is successful, place the object to be measured on the test platform, and try to position the object in the center of the image as much as possible.

4. Press T/t to perform the measurement of the target size.

5. In the image preview window, check the results of the size measurement.

   <img src="assets/sample.png" style="zoom:50%;" />

### How to compile  Sample

1. Open Sample/Sample.vcxproj with visual studio 2019;
2. Set the compilation options to Release and x64;
3. Compile, and the generated file is in the Bin/x64 directory;

### FAQ

**Q1:** During background calibration, the message "The calibration has failed: x" is displayed.

**A:** Adjust the mounting angle of the camera to make sure that the front cover of the camera is parallel to the test platform. Then try again. If the recalibration still fails after multiple adjustments, please provide us with the following documents for analysis.

- bg.bin
- VolMeasure.txt
- WrapperAlgAPI.txt
- alg_volmeasure.json (probably not exists)

**Q2:** How to save offline data?

**A:** Follow the steps below to save your offline data and send it to us for analysis.

1. Press the R/r button to enable the offline save function;
2. Press the T/t button to measure;
3. Find the RECYYYYMMDD_HHMMSS_MS (e.g. REC20250212_145340_790) folder in the current directory and send us the entire folder.

### Resources

- [measurement data](./Measurement data.md)
