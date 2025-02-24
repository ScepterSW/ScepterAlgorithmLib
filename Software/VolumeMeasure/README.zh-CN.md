## 三维体积测量算法

[English](README.md)

本算法基于平面基准实现纸箱类物体的长、宽、高三维尺寸精确测量。

### 支持的产品

- NYX Camera
  - NYX650 （NYX650_R_20241203_B26 及以上）
- Vzense Camera
  - DS86（DS86_R_20241206_B21 及以上）
### 支持的平台
- Arm-Linux （AArch64 架构）：待定
- Ubuntu （x86_64 架构)）：待定
- Windows 10/11（x86_64 架构)）：已支持


### 预编译示例使用方法

1. **设备安装**

   a. 将相机前盖板与测试平面尽量保持平行（夹角小于3度）安装

   b. 建议使用水平仪验证安装平面度

2. **背景校准**

   a. 进入目录：Software/VolumeMeasure/Windows/PrecompiledSamples/

   b. 双击运行 Sample.exe

   c. 按下 **B/b** 键执行背景标定流程

   <img src="assets/calibration.png" style="zoom:50%;" />

3. **测量操作**

   a. 标定成功后，将待测物体置于测试平面中心区域

   b. 按下 **T/t** 键执行单次测量
   
   c. 在实时预览窗口查看测量结果

   <img src="assets/sample.png" style="zoom:50%;" />
   
4. **配置文件的使用**

   算法默认只支持测量立方体形状的物体，用户如果想要测量不规则物体，可以参考以下说明修改配置文件alg_volmeasure.json。配置文件修改后，需要重启Sample.exe才能生效。

   配置文件示例：
   
   ```json
   {
   	"ALG" : 
   	{
   		"groundPlaneParams" : 
   		[
   			0.012808113358914852,
   			0.003450226504355669,
   			0.99991202354431152,
   			-1015.4520874023438
   		],
   		"shapeType":2
   	}
   }
   ```
   
   配置项说明：
   
   | 配置项            | 说明                                                         |
   | ----------------- | ------------------------------------------------------------ |
   | groundPlaneParams | 背景标定生成的参数，用户不可手动修改。                       |
   | shapeType         | 算法支持的被测物体的形状：<br>1：默认值，只支持测量立方体形状的物体；<br>2：只支持测量不规则形状的物体；<br>3：自动识别物体形状，支持测量立方体和不规则形状的物体。（自动识别形状存在误差，如果确定物体的形状是立方体，建议优先设置为1） |

### 示例代码编译指南

1. 使用 Visual Studio 打开Sample/Sample.vcxproj文件;
2. 将编译选项设置为 **Release** 和 **x64**;
3. 执行编译，生成文件位于 Bin/x64 目录。

### 常见问题

**问题1：** 运行Sample.exe背景标定失败，提示："The calibration has failed: x" 。

按以下步骤处理：

1. 检测相机安装平面度；
2. 检查测试区域是否有干扰物；
3. 收集以下文件提交技术支持：
   - bg.bin
   - VolMeasure.txt
   - WrapperAlgAPI.txt
   - alg_volmeasure.json

**问题2：** 如何使用Sample.exe保存离线数据？

按以下步骤保存：

1. 按下 **R/r** 键启用离线保存功能，软件自动创建保存目录；
2. 按下 **T/t** 键执行测量，按一次**T/t**保存一次离线数据；
3. 测量结束时，按下**R/r** 键关闭离线保存功能，在当前目录查找 RECYYYYMMDD_HHMMSS_MS 格式文件夹，此文件夹即为本次保存的离线数据。
3. 把完整的保存离线数据的文件夹提交技术支持。

### 实测数据参考

- 使用不同产品相机对不同尺寸纸盒的测量数据：[measurement data](Measurement_data.zh-CN.md)
