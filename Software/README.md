## Softeware's Algorithm Library

This directory stores algorithm libraries based on ScepterSDK/BaseSDK. 

### Software architecture

<img src="assets/architecture.png" alt="architecture" style="zoom:80%;" />

The algorithm library runs on the host machine (Windows/Ubuntu/AArch64 Linux), interacts with the camera through [ScepterSDK](https://github.com/ScepterSW/ScepterSDK/BaseSDK), and provides interfaces for applications to call.

### Directory Structure

- **[VolumeMeasure](VolumeMeasure/Readme.md)**: used to measure the length, width, and height of cartons.
