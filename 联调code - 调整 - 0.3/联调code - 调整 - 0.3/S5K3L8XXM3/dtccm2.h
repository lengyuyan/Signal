/******************************************************************************
DTCCM第二版
******************************************************************************/

#ifndef _DTCCM2_H_
#define _DTCCM2_H_

//#define _DTCALL_ _stdcall
#define _DTCALL_ 

#include "imagekit.h"

#ifdef DTCCM2_EXPORTS

#define DTCCM_API extern "C"  __declspec(dllexport)

#else


#define DTCCM_API extern "C"  __declspec(dllimport)

#endif

/*! \mainpage DOTHINKEY ImageKit API Manual
*
* \section 产品介绍
*
* - USB2.0系列
* -# HS128
* -# HS280
* -# HS300
* -# HS300D
* -# HV810
* -# HV810D
* -# HV910
* -# HV910D
* - PCIE系列
* -# PE300
* -# PE300D
* -# PE810
* -# PE810D
* -# PE910
* -# PE910D
* -# PE350
* -# PE950
* - USB3.0系列
* -# UT300
* -# UV910
* -# UH910
* -# DTLC2
* -# UH920
* -# UF920
*
*
* \section  公司网址
* http://www.dothinkey.com
* 
* 
*
* \section 文档发布版本记录
* -# 2013/8/22 生成DTCC2APIGuide；\n
*			   DTCCM2APIGuide版本为：1.0.0.0；
*
* -# 2014/5/13 新增加ExtI2cWrite和ExtI2cRead函数接口用于外部扩展的I2C读写；\n
*    		   DTCCM2APIGuide版本为：1.0.2.2； 
*
* -# 2014/5/15 归类整理，将EEPROM这个章节整合到设备信息章节；\n
*			   DTCCM2APIGuide版本为：1.0.3.3；
*
*/

/* 定义默认打开的设备ID */
#define DEFAULT_DEV_ID		0

	
/******************************************************************************
设备信息相关
******************************************************************************/
/** @defgroup group1 设备信息相关
@{

*/


/// @brief 枚举设备，获得设备名及设备个数。
///
/// @param DeviceName：枚举的设备名
/// @param iDeviceNumMax：指定枚举设备的最大个数
/// @param pDeviceNum：枚举的设备个数
///
/// @retval DT_ERROR_OK：枚举操作成功
/// @retval DT_ERROR_FAILED:枚举操作失败
/// @retval DT_ERROR_INTERNAL_ERROR:内部错误
///
/// @note 获取的设备名称字符串需要用户程序调用GlobalFree()逐个释放。
DTCCM_API int _DTCALL_ EnumerateDevice(char *DeviceName[],int iDeviceNumMax,int *pDeviceNum);

/// @brief 获取本类的版本编号，派生类可以返回自己的版本编号。
///
/// @param Version:本类的版本号
///
/// @retval DT_ERROR_OK：获取本类的版本号成功
DTCCM_API int _DTCALL_ GetLibVersion(DWORD Version[4],int iDevID=DEFAULT_DEV_ID);

/// @brief 获取固件版本,iChip指定哪个芯片的固件。
///
/// @param iChip：芯片编号
/// @param Version：固件的版本号
///
/// @retval DT_ERROR_OK：获取指定的芯片固件版本号成功
/// @retval DT_ERROR_FAILED：获取指定的芯片固件版本号失败
/// @retval DT_ERROR_PARAMETER_OUT_OF_BOUND：指定的芯片固件编号超出了范围
/// @retval DT_ERROR_COMM_ERROR：通讯错误
/// @retval DT_ERROR_TIME_OUT：获取版本号超时
DTCCM_API int _DTCALL_ GetFwVersion(int iChip,DWORD Version[4],int iDevID);

/// @brief 获取设备的序列号。
///
/// @param pSN：返回的设备序列号
/// @param iBufferSize：设置要获取序列号字节长度
/// @param pRetLen：返回实际设备序列号字节长度
/// 
/// @retval DT_ERROR_OK：获取设备的序列号成功
/// @retval DT_ERROR_FAILED：获取设备的序列号失败
/// @retval DT_ERROR_COMM_ERROR：通讯错误
DTCCM_API int _DTCALL_ GetDeviceSN(BYTE *pSN,int iBufferSize,int *pRetLen,int iDevID=DEFAULT_DEV_ID);

/// @brief 获取设备的硬件版本。
///
/// @param Version：硬件版本号
///
/// @retval DT_ERROR_OK：获取设备的硬件版本号成功
/// @retval DT_ERROR_COMM_ERROR：通讯错误
DTCCM_API int _DTCALL_ GetHardwareVersion(DWORD Version[4],int iDevID=DEFAULT_DEV_ID);

/// @bfief 获取设备的硬件信息，可读的字符串。 
///
/// @param pBuf：返回的设备硬件信息
/// @param iBufferSize：设置要获取的设备硬件信息字节长度
/// @param pRetLen：返回的设备序列号字节长度
///
/// @retval DT_ERROR_OK：获取设备的硬件信息成功
/// @retval DT_ERROR_FAILED：获取设备的硬件信息失败
/// @retval DT_ERROR_COMM_ERROR：通讯错误 
DTCCM_API int _DTCALL_ GetHardwareInfo(BYTE* pBuf,int iBufferSize,int *pRetLen,int iDevID=DEFAULT_DEV_ID);

/// @brief 写用户自定义名称
///
/// @param pName：用户定义的名称
/// @param iSize：用户名称字节长度，最大126字节
///
/// @retval DT_ERROR_OK：设置用户自定义名称成功
/// @retval DT_ERROR_FAILED：设置用户自定义名称失败
/// @retval DT_ERROR_COMM_ERROR：通讯错误 
/// @retval DT_ERROR_PARAMETER_OUT_OF_BOUND：iSize参数越界
DTCCM_API int _DTCALL_ WriteUserDefinedName(char *pName, int iSize, int iDevID=DEFAULT_DEV_ID);

/// @brief 写用户自定义名称
///
/// @param pName：用户定义的名称
/// @param iSize：用户名称字节长度，最大126字节
/// @param bMaster: 为1设置当前设备为主机，注意不要将两台设备都设为主机或者从机，必须一主一从
///
/// @retval DT_ERROR_OK：设置用户自定义名称成功
/// @retval DT_ERROR_FAILED：设置用户自定义名称失败
/// @retval DT_ERROR_COMM_ERROR：通讯错误 
/// @retval DT_ERROR_PARAMETER_OUT_OF_BOUND：iSize参数越界
DTCCM_API int _DTCALL_ WriteUserDefinedNameEx(char *pName, int iSize ,BOOL bMaster, int iDevID=DEFAULT_DEV_ID);

/// @brief 读用户自定义名称
///
/// @param pName：返回用户定义的名称
/// @param iBufferSize：设置要获取的用户名称字节长度
/// @param pRetLen：返回的用户名称字节长度
///
/// @retval DT_ERROR_OK：获取用户定义名称成功
/// @retval DT_ERROR_FAILED：获取用户定义名称失败
/// @retval DT_ERROR_COMM_ERROR：通讯错误 
DTCCM_API int _DTCALL_ ReadUserDefinedName(char *pName, int iBufferSize,int *pRetLen,int iDevID=DEFAULT_DEV_ID);

/// @brief 判断设备是否打开。 
///
/// @retval DT_ERROR_OK：设备已经连接打开
/// @retval DT_ERROR_FAILED：设备没有连接成功
/// @retval DT_ERROR_COMM_ERROR：通讯错误
DTCCM_API int _DTCALL_ IsDevConnect(int iDevID=DEFAULT_DEV_ID);

/// @brief 返回设备的型号，区分不同的测试板。
///
/// @retval 0x0010：HS128测试板
/// @retval 0x0020：HS230测试板
/// @retval 0x0030：HS300测试板
/// @retval 0x0031：HS300D测试板
/// @retval 0x0092：HV910测试板
/// @retval 0x0093：HV910D测试板
/// @retval 0x0082：HV810测试板
/// @retval 0x0083：HV810D测试板
///
/// @retval 0x0130：PE300测试板
/// @retval 0x0131：PE300D测试板
/// @retval 0x0190：PE910测试板
///	@retval 0x0191：PE910D测试板
/// @retval 0x0180：PE810测试板
///	@retval 0x0181：PE810D测试板
/// @retval 0x0132：PE350测试板
/// @retval 0x0192：PE950测试板
/// @retval 0x0193：MP950测试板
///
///	@retval 0x0231：UT300测试板
/// @retval 0x0232：UO300测试板
/// @retval 0x0233: UM330测试板
///	@retval 0x0292：UV910测试板
///	@retval 0x0293：UH910测试板
///	@retval 0x02A1：DTLC2测试板
/// @retval 0x0295：UF920测试板
/// @retval 0x0295：UM900测试板
/// @retval 0x0296：MU950测试板
/// @retval 0x0297：DMU956测试板
/// @retval 0x0239：ULV330测试板
/// @retval 0x0298: CMU958测试板
/// @retval 0x0299：ULV913测试板
/// @retval 0x029a: ULV966测试板
/// @retval 0x029b: ULM928测试板

///	@retval 0x0294：UH920测试板
DTCCM_API DWORD _DTCALL_ GetKitType(int iDevID=DEFAULT_DEV_ID);

/******************************************************************************
EEPROM相关
******************************************************************************/

/// @brief 从EEPROM读一个字。
///
/// @param uAddr：EEPROM的寄存器地址
/// @param pValue：向EEPROM读到字
///
/// @retval DT_ERROR_OK：读EEPROM成功
/// @retval DT_ERROR_FAILD：读EEPROM失败
/// @retval DT_ERROR_INTERNAL_ERROR：内部错误
/// @retval DT_ERROR_COMM_ERROR：通讯错误
DTCCM_API int _DTCALL_ ReadWordFromEEProm(USHORT uAddr, USHORT *pValue,int iDevID=DEFAULT_DEV_ID);

/** @} */ // end of group1



/******************************************************************************
ISP相关
******************************************************************************/
/** @defgroup group2 ISP相关


* @{

*/

/// @brief 获取GAMMA设置值。
/// 
/// @param pGamma：返回的GAMMA设置值
DTCCM_API int _DTCALL_ GetGamma(int *pGamma,int iDevID=DEFAULT_DEV_ID);

/// @brief 设置GAMMA值。
///
/// @param iGamma：设置的GAMMA值
DTCCM_API int _DTCALL_ SetGamma(int iGamma,int iDevID=DEFAULT_DEV_ID);

/// @brief 获取对比度设置值。
///
/// @param pContrast：返回的对比度设置值
DTCCM_API int _DTCALL_ GetContrast(int *pContrast,int iDevID=DEFAULT_DEV_ID);

/// @brief 设置对比度。
///
/// @param iContrast：设置对比度值
DTCCM_API int _DTCALL_ SetContrast(int iContrast,int iDevID=DEFAULT_DEV_ID);

/// @brief 获取饱和度设置值。
///
/// @param pSaturation：返回的饱和度设置值
DTCCM_API int _DTCALL_ GetSaturation(int *pSaturation,int iDevID=DEFAULT_DEV_ID);

/// @brief 设置饱和度。
///
/// @param iSaturation：设置饱和度值
DTCCM_API int _DTCALL_ SetSaturation(int iSaturation,int iDevID=DEFAULT_DEV_ID);

/// @brief 设置锐度。
///
/// @param iSharpness：设置锐度值
DTCCM_API int _DTCALL_ SetSharpness(int iSharpness, int iDevID=DEFAULT_DEV_ID);

/// @brief 获取锐度设置值。
///
/// @param pSharpness：返回的锐度设置值
DTCCM_API int _DTCALL_ GetSharpness(int *pSharpness, int iDevID=DEFAULT_DEV_ID);

/// @brief 设置噪声。
///
/// @param iLevel：设置噪声值
DTCCM_API int _DTCALL_ SetNoiseReduction(int iLevel, int iDevID=DEFAULT_DEV_ID);

/// @brief 获取设置噪声值。
/// 
/// @param pLevel：返回设置的噪声值
DTCCM_API int _DTCALL_ GetNoiseReduction(int *pLevel, int iDevID=DEFAULT_DEV_ID);

/// @brief 开启或关闭消死点。
/// 
/// @param bDeadPixCleanEn：为TRUE开启消死点，为FALSE关闭消死点
DTCCM_API int _DTCALL_ SetDeadPixelsClean(BOOL bDeadPixCleanEn,int iDevID=DEFAULT_DEV_ID);

/// @brief 获取消死点开启或关闭状态。
/// 
/// @param pbDeadPixCleanEn：返回消死点消死点开启或关闭状态
DTCCM_API int _DTCALL_ GetDeadPixelsClean(BOOL *pbDeadPixCleanEn, int iDevID=DEFAULT_DEV_ID);

/// @brief 设置彩点阀值
/// 
/// @param iHotCpth：设置的彩点阀值，大于阀值的像素点判断为彩点
DTCCM_API int _DTCALL_ SetHotCpth(int iHotCpth, int iDevID=DEFAULT_DEV_ID);

/// @brief 获取设置的彩点阀值
/// 
/// @param pHotCpth：返回的彩点阀值设置值
DTCCM_API int _DTCALL_ GetHotCpth(int *pHotCpth, int iDevID=DEFAULT_DEV_ID);

/// @brief 设置暗点阀值
/// 
/// @param iDeadCpth：设置的暗点阀值，小于阀值的像素点判断为暗点
DTCCM_API int _DTCALL_ SetDeadCpth(int iDeadCpth, int iDevID=DEFAULT_DEV_ID);

/// @brief 获取设置的暗点阀值
/// 
/// @param pHotCpth：返回的暗点阀值设置值
DTCCM_API int _DTCALL_ GetDeadCpth(int *pDeadCpth, int iDevID=DEFAULT_DEV_ID);

/// @brief 设置raw转rgb算法。
///
/// @param Algrithm：ram转rgb算法设置值,参见宏定义“RAW转RGB算法定义”
DTCCM_API int _DTCALL_ SetRaw2RgbAlgorithm(BYTE Algrithm, int iDevID=DEFAULT_DEV_ID);

/// @brief 获取raw转rgb算法设置值。
///
/// @param 返回raw转rgb算法设置值
DTCCM_API int _DTCALL_ GetRaw2RgbAlgorithm(BYTE *pAlgrithm, int iDevID=DEFAULT_DEV_ID);

/// @brief 设置RGB数字增益。
///
/// @param fRGain：R增益值
/// @param fGGain：G增益值
/// @param fBGain：B增益值
DTCCM_API int _DTCALL_ SetDigitalGain(float fRGain,float fGGain,float fBGain,int iDevID=DEFAULT_DEV_ID);

/// @brief 开启或关闭色彩矩阵
/// 
/// @param bEnable：为TRUE开启矩阵，为FALSE关闭矩阵
DTCCM_API int _DTCALL_ SetMatrixEnable(BOOL bEnable, int iDevID=DEFAULT_DEV_ID);

/// @brief 获取RGB数字增益。
/// 
/// @param pRGain：返回的R数字增益值
/// @param pGGain：返回的G数字增益值
/// @param pBGain：返回的B数字增益值
DTCCM_API int _DTCALL_ GetDigitalGain(float *pRGain,float *pGGain,float *pBGain,int iDevID=DEFAULT_DEV_ID);

/// @brief 开启或关闭白平衡。
///
/// @param bAWBEn：白平衡使能，为TRUE开启白平衡，为FALSE关闭白平衡
DTCCM_API int _DTCALL_ SetAWB(BOOL bAWBEn,int iDevID=DEFAULT_DEV_ID);

/// @brief 获取白平衡开启状态。
///
/// @param pAwb：返回的白平衡状态
DTCCM_API int _DTCALL_ GetAWB(BOOL *pAwb,int iDevID=DEFAULT_DEV_ID);

/// @brief 设置颜色矩阵。
///
/// @param Matrix：设置的颜色矩阵
DTCCM_API int _DTCALL_ SetMatrixData(float Matrix[3][3],int iDevID=DEFAULT_DEV_ID);

/// @brief 获取颜色矩阵。
///
/// @param Matrix：返回的颜色矩阵
DTCCM_API int _DTCALL_ GetMatrixData(float Matrix[3][3],int iDevID=DEFAULT_DEV_ID);

/// @brief 开启或关闭MONO模式。
///
/// @param bEnable：MONO模式使能，为TRUE开启MONO模式，为FALSE关闭MONO模式
DTCCM_API int _DTCALL_ SetMonoMode(BOOL bEnable,int iDevID=DEFAULT_DEV_ID);

/// @brief 开启或关闭图像水平反向。
///
/// @param bEnable：图像水平反向使能，为TRUE开启水平反向，为FALSE关闭水平反向
DTCCM_API int _DTCALL_ SetHFlip(BOOL bEnable,int iDevID=DEFAULT_DEV_ID);

/// @brief 开启或关闭图像垂直反向。
///
/// @param bEnable：图像垂直反向使能，为TRUE开启垂直反向，为FALSE关闭垂直反向
DTCCM_API int _DTCALL_ SetVFlip(BOOL bEnable,int iDevID=DEFAULT_DEV_ID);

/// @brief 开启或关闭“十”字线。
///
/// @param bEnable：“十”字线使能，为TRUE开启十字线，为FALSE关闭十字线
DTCCM_API int _DTCALL_ SetCrossOn(BOOL bEnable,int iDevID=DEFAULT_DEV_ID);

//RGB24 to YUV422 (YcbYcr mode)
//pBmp24 :RGB24 data pointer;
//pOut   :YUV422 pointer
//width  :Image width;
//height :Image height;

/// @brief RGB24转YUV422。
///
/// @param pBmp24：RGB24数据
/// @param pOut：YUV422数据
/// @param width：图像数据宽度
/// @param height：图像数据高度
DTCCM_API int _DTCALL_ RGB24ToYUV422(BYTE* pBmp24, BYTE* pOut, USHORT uWidth, USHORT uHeight, int iDevID=DEFAULT_DEV_ID);

//RGB24 to YUV444 (Ycbcr )
//pBmp24 :RGB24 data pointer;
//pOut   :YUV422 pointer
//width  :Image width;
//height :Image height;

/// @brief RGB24转YUV444。
///
/// @param pBmp24：RGB24数据
/// @param pOut：YUV422数据
/// @param width：图像数据宽度
/// @param height：图像数据高度
DTCCM_API int _DTCALL_ RGB24ToYUV444(BYTE *pBmp24, BYTE *pOut, USHORT uWidth, USHORT uHeight, int iDevID=DEFAULT_DEV_ID);


/// @brief 显示RGB图像数据。
///
/// @param pBmp24：待显示的RGB24格式的数据
/// @param pInfo：帧信息，参见结构体“FrameInfo”
///
/// @retval DT_ERROR_OK：显示RGB图像成功
/// @retval DT_ERROR_FAILD：显示RGB图像失败 
DTCCM_API int _DTCALL_ DisplayRGB24(BYTE *pBmp24,FrameInfo *pInfo=NULL,int iDevID=DEFAULT_DEV_ID);

/// @brief 显示RGB图像数据。
///
/// @param pBmp24：待显示的RGB24格式的数据
/// @param pInfo：帧信息，参见结构体“FrameInfoEx”
///
/// @retval DT_ERROR_OK：显示RGB图像成功
/// @retval DT_ERROR_FAILD：显示RGB图像失败 
DTCCM_API int _DTCALL_ DisplayRGB24Ex(BYTE *pBmp24,FrameInfoEx *pInfo,int iDevID=DEFAULT_DEV_ID);

/// @brief RAW/YUV转RGB，源数据格式由ImgFormat指定。
///
/// @param pIn：源图像数据
/// @param pOut：转为RGB24的数据
/// @param uWidth：图像数据宽度
/// @param uHeight：图像数据高度
/// @param byImgFormat：源数据的格式
DTCCM_API int _DTCALL_ DataToRGB24(BYTE *pIn, BYTE *pOut, USHORT uWidth, USHORT uHeight, BYTE byImgFormat,int iDevID=DEFAULT_DEV_ID);


/// @brief RAW/YUV转RGB，源数据格式由ImgFormat指定。(双通道产品UH920使用时要指定通道)
///
/// @param pIn：源数据
/// @param pOut：转为RGB24的数据
/// @param uWidth：图像数据宽度
/// @param uHeight：图像数据高度
/// @param byImgFormat：源数据的格式
/// @param byChannel: 指定通道
DTCCM_API int _DTCALL_ DataToRGB24Ex(BYTE *pIn, BYTE *pOut, USHORT uWidth, USHORT uHeight, BYTE byImgFormat,BYTE byChannel,int iDevID=DEFAULT_DEV_ID);

/// @brief 显示RAW/YUV图像数据，源数据格式由Init函数中的byImgFormat参数指定。
///
/// @param pData：待显示的图像数据
/// @param pInfo：帧信息，参见结构体“FrameInfo”
///
/// @retval DT_ERROR_OK：显示图像成功
/// @retval DT_ERROR_FAILD：显示图像失败 
/// @retval DT_ERROR_NOT_INITIALIZED：没有初始化
DTCCM_API int _DTCALL_ DisplayVideo(BYTE *pData,FrameInfo *pInfo=NULL,int iDevID=DEFAULT_DEV_ID);

/// @brief 显示RAW/YUV图像数据，源数据格式由Init函数中的byImgFormat参数指定。
///
/// @param pData：待显示的图像数据
/// @param pInfo：帧信息，参见结构体“FrameInfoEx”
///
/// @retval DT_ERROR_OK：显示图像成功
/// @retval DT_ERROR_FAILD：显示图像失败 
/// @retval DT_ERROR_NOT_INITIALIZED：没有初始化
DTCCM_API int _DTCALL_ DisplayVideoEx(BYTE *pData,FrameInfoEx *pInfo,int iDevID=DEFAULT_DEV_ID);

/// @brief 对RAW图像数据进行图像处理(MONO,WB,ColorChange,Gamma,Contrast)。
///
/// @param pImage：RAW图像数据
/// @param pBmp24：经过图像处理后的数据
/// @param uWidth：图像数据宽度
/// @param uHeight：图像数据高度
/// @param pInfo：帧信息，参见结构体“FrameInfo”
/// 
/// @retval DT_ERROR_OK：图像处理成功
/// @retval DT_ERROR_PARAMETER_INVALID：pData无效的参数
/// @retval DT_ERROR_INTERNAL_ERROR：内部错误
DTCCM_API int _DTCALL_ ImageProcess(BYTE *pImage, BYTE *pBmp24, int nWidth, int nHeight,FrameInfo *pInfo,int iDevID=DEFAULT_DEV_ID);

/// @brief 对RAW图像数据进行图像处理(MONO,WB,ColorChange,Gamma,Contrast)。
///
/// @param pImage：RAW图像数据
/// @param pBmp24：经过图像处理后的数据
/// @param uWidth：图像数据宽度
/// @param uHeight：图像数据高度
/// @param pInfo：帧信息，参见结构体“FrameInfoEx”
/// 
/// @retval DT_ERROR_OK：图像处理成功
/// @retval DT_ERROR_PARAMETER_INVALID：pData无效的参数
/// @retval DT_ERROR_INTERNAL_ERROR：内部错误
DTCCM_API int _DTCALL_ ImageProcessEx(BYTE *pImage, BYTE *pBmp24, int nWidth, int nHeight,FrameInfoEx *pInfo,int iDevID=DEFAULT_DEV_ID);

/// @brief 设置RAW格式，参见枚举类型“RAW_FORMAT”。
///
/// @param byRawMode：RAW格式设置
///
/// @see RAW_FORMAT
DTCCM_API int _DTCALL_ SetRawFormat(BYTE byRawMode,int iDevID=DEFAULT_DEV_ID);

/// @brief 设置YUV格式，参见枚举类型“YUV_FORMAT”。
///
/// @param byYuvMode：YUV格式设置
///
/// @see YUV_FORMAT
DTCCM_API int _DTCALL_ SetYUV422Format(BYTE byYuvMode,int iDevID=DEFAULT_DEV_ID);

/// @brief 设置YUV转RGB时的一些系数。
///
///	@param Y=(Y-yoff)*ycoef
///	@param r=Y+(cr-128)*bdif
///	@param g=Y-cbcoef*(cb-128)-crcoef*(cr-128)
///	@param b=Y+(cb-128)*rdif
DTCCM_API int _DTCALL_ SetYuvCoefficient(int yoff, 
										 float ycoef, 
										 float cbcoef, 
										 float crcoef, 
										 float rdif, 
										 float bdif,
										 int iDevID=DEFAULT_DEV_ID);

/// @brief 初始化显示，支持2个窗口显示，如果使用2个sensor，须要使用hWndEx指定第二个窗口。
///
/// @param hWnd：显示A通道图像的窗口句柄
/// @param uImgWidth：图像数据宽度
/// @param uHeight：图像数据高度
/// @param byImgFormat：图像数据格式，如：RAW/YUV
/// @param hWndEx：hWndEx：显示B通道图像的窗口句柄
DTCCM_API int _DTCALL_ InitDisplay(HWND hWnd, 
								   USHORT uImgWidth, 
								   USHORT uImgHeight,
								   BYTE byImgFormat,
								   BYTE byChannel=CHANNEL_A,
								   HWND hWndEx=NULL,
								   int iDevID=DEFAULT_DEV_ID);

DTCCM_API int _DTCALL_ GrabDataToRaw8(BYTE *pIn, BYTE *pOut,int uWidth ,int uHeight,BYTE ImgFormat,int iDevID=DEFAULT_DEV_ID);

DTCCM_API int _DTCALL_ GrabDataToRaw16(BYTE *pIn, USHORT *pOut,int uWidth ,int uHeight,BYTE ImgFormat,int iDevID=DEFAULT_DEV_ID);


/// @brief 对RAW图像数据进行图像处理(MONO,WB,ColorChange,Gamma,Contrast)。
///
/// @param pImage：RAW图像数据
/// @param pBmp24：经过图像处理后的数据
/// @param uWidth：图像数据宽度
/// @param uHeight：图像数据高度
/// @param pInfo：帧信息，参见结构体“FrameInfo”
/// 
/// @retval DT_ERROR_OK：图像处理成功
/// @retval DT_ERROR_PARAMETER_INVALID：pData无效的参数
/// @retval DT_ERROR_INTERNAL_ERROR：内部错误
DTCCM_API int _DTCALL_ ImageProcessForRaw8(BYTE *pImage, BYTE *pBmp24, int nWidth, int nHeight,FrameInfo *pInfo,int iDevID=DEFAULT_DEV_ID);

/** @} */ // end of group2


/******************************************************************************
SENSOR相关
*******************************************************************************/
/** @defgroup group3 SENSOR相关


* @{

*/
/// @brief 设置与sensor通讯的总线类型，目前有I2C和SPI
///
/// @param iChannel：sensor通讯总线类型设置，为0是I2C，为1是SPI
//DTCCM_API int _DTCALL_ SetSensorBusChannel(int iChannel, int iDevID=DEFAULT_DEV_ID);


//DTCCM_API int _DTCALL_ SetSensorSpiBusCtrl(int );

//DTCCM_API int _DTCALL_ SetSensorSpi()

/// @brief 复位与Sensor通讯的I2C总线。
///
/// @retval DT_ERROR_OK：复位I2C操作成功
/// @retval DT_ERROR_FAILED：复位I2C操作失败
DTCCM_API int _DTCALL_ ResetSensorI2cBus(int iDevID=DEFAULT_DEV_ID);

/// @brief 设置I2C的字节间隔(HS系列,PE950支持)
///
/// @brief uInterval：字节间隔设置,单位us
DTCCM_API int _DTCALL_ SetI2CInterval(UINT uInterval, int iDevID=DEFAULT_DEV_ID);

/// @brief 设置I2C通讯ACK超时等待时间。
/// 
/// @param nUsWait：设置ACK超时等待时间，单位us
///
/// @retval DT_ERROR_OK：设置I2C通讯ACK超时等待时间成功
DTCCM_API int _DTCALL_ SetSensorI2cAckWait(UINT uAckWait,int iDevID=DEFAULT_DEV_ID);

/// @brief 设置与SENSOR通讯的I2C总线速率，400Kbps或100Kbps。
///
/// @param b400K：b400K=TURE，400Kbps；b400K=FALSE,100Kbps
///							 
/// @retval DT_ERROR_OK：设置总线速率操作成功
DTCCM_API int _DTCALL_ SetSensorI2cRate(BOOL b400K,int iDevID=DEFAULT_DEV_ID);

/// @brief 设置与SENSOR通讯的I2C总线速率，范围10Khz-2Mhz。
///
/// @param uKpbs：设置I2C总线速率，范围值为10-2000
///							 
/// @retval DT_ERROR_OK：设置总线速率操作成功
DTCCM_API int _DTCALL_ SetSensorI2cRateEx(UINT uKpbs,int iDevID=DEFAULT_DEV_ID);

/// @brief 使能与SENSOR通讯的I2C总线为Rapid模式。
///
/// @param  bRapid=1表示，强制灌电流输出高电平;=0，I2C管脚为输入状态，借助外部上拉变成高电平
///							 
/// @retval DT_ERROR_OK：设置I2C总线Rapid模式成功
DTCCM_API int _DTCALL_ SetSensorI2cRapid(BOOL bRapid,int iDevID=DEFAULT_DEV_ID);

/// @brief 写SENSOR寄存器,I2C通讯模式byI2cMode的设置值见I2CMODE定义。
/// 
/// @param uAddr：从器件地址
/// @param uReg：寄存器地址
/// @param uValue：写入寄存器的值
/// @param byMode：I2C模式
///
/// @retval DT_ERROR_OK：写SENSOR寄存器操作成功
/// @retval DT_ERROR_COMM_ERROR：通讯错误
/// @retval DT_ERROR_PARAMETER_INVALID：byMode参数无效
/// @retval DT_ERROR_TIME_OUT：通讯超时
/// @retval DT_ERROR_INTERNAL_ERROR：内部错误
///
/// @see I2CMODE
DTCCM_API int _DTCALL_ WriteSensorReg(UCHAR uAddr, USHORT uReg, USHORT uValue, BYTE byMode,int iDevID=DEFAULT_DEV_ID);

/// @brief 读SESNOR寄存器,I2C通讯模式byI2cMode的设置值见I2CMODE定义。
///
/// @param uAddr：从器件地址
/// @param uReg：寄存器地址
/// @param pValue：读到的寄存器的值
/// @param byMode：I2C模式
///
/// @retval DT_ERROR_OK：读SENSOR寄存器操作成功
/// @retval DT_ERROR_COMM_ERROR：通讯错误
/// @retval DT_ERROR_PARAMETER_INVALID：byMode参数无效
/// @retval DT_ERROR_TIME_OUT：通讯超时
/// @retval DT_ERROR_INTERNAL_ERROR：内部错误
///
/// @see I2CMODE
DTCCM_API int _DTCALL_ ReadSensorReg(UCHAR uAddr, USHORT uReg, USHORT *pValue,BYTE byMode,int iDevID=DEFAULT_DEV_ID);

/// @brief 写SENSOR寄存器，支持向多个寄存器写入数据（最大支持16个寄存器）。
///
/// @param uAddr：从器件地址
/// @param byI2cMode：I2C模式
/// @param uRegNum：写入寄存器个数，最大16
/// @param RegAddr[]：寄存器地址数组
/// @param RegData[]：写入寄存器的数据
/// @param RegDelay[]：写完一组寄存器与下一组寄存器之间的延时，单位us
///
/// @retval DT_ERROR_OK：完成写SENSOR寄存器操作成功
/// @retval DT_ERROR_COMM_ERROR：通讯错误
/// @retval DT_ERROR_PARAMETER_INVALID：uSize参数无效
/// @retval DT_ERROR_TIME_OUT：通讯超时
/// @retval DT_ERROR_INTERNAL_ERROR：内部错误
DTCCM_API int _DTCALL_ WriteSensorMultiRegsWithDelay(UCHAR uAddr, BYTE byI2cMode, USHORT uRegNum, USHORT RegAddr[], USHORT RegData[],USHORT RegDelay[],int iDevID=DEFAULT_DEV_ID);

/// @brief 写SENSOR寄存器，支持向一个寄存器写入一个数据块（不超过255字节）。
///
/// @param uDevAddr：从器件地址
/// @param uRegAddr：寄存器地址
/// @param uRegAddrSize：寄存器地址的字节数
/// @param pData：写入寄存器的数据块
/// @param uSize：写入寄存器的数据块的字节数（不超过255字节(HS300/HS300D/HV910/HV910D一次不能超过253字节)）
///
/// @retval DT_ERROR_OK：完成写SENSOR寄存器块操作成功
/// @retval DT_ERROR_COMM_ERROR：通讯错误
/// @retval DT_ERROR_PARAMETER_INVALID：uSize参数无效
/// @retval DT_ERROR_TIME_OUT：通讯超时
/// @retval DT_ERROR_INTERNAL_ERROR：内部错误
DTCCM_API int _DTCALL_ WriteSensorI2c(UCHAR uDevAddr,USHORT uRegAddr,UCHAR uRegAddrSize,BYTE *pData,USHORT uSize,int iDevID=DEFAULT_DEV_ID);

/// @brief 读SENSOR寄存器，支持向一个寄存器读出一个数据块（不超过255字节）。
///
/// @param uDevAddr：从器件地址
/// @param uRegAddr：寄存器地址
/// @param uRegAddrSize：寄存器地址的字节数
/// @param pData：读到寄存器的值
/// @param uSize：读出寄存器的数据块的字节数（不超过255字节）
/// @param bNoStop：是否发出I2C的STOP命令，一般情况下默认为FALSE，bNoStop=TRUE表示写的过程不会有I2C的停止命令，bNoStop=FALSE有I2C的停止命令
///
/// @retval DT_ERROR_OK：完成读SENSOR寄存器块操作成功
/// @retval DT_ERROR_COMM_ERROR：通讯错误
/// @retval DT_ERROR_PARAMETER_INVALID：uSize参数无效
/// @retval DT_ERROR_TIME_OUT：通讯超时
/// @retval DT_ERROR_INTERNAL_ERROR：内部错误
DTCCM_API int _DTCALL_ ReadSensorI2c(UCHAR uDevAddr,USHORT uRegAddr,UCHAR uRegAddrSize,BYTE *pData,USHORT uSize,BOOL bNoStop=FALSE,int iDevID=DEFAULT_DEV_ID);

/// @brief 设置SENSOR的电流值，100mA或300mA。
///
/// @param iLimit：电流值（只能设置为100或者300）
/// 
/// @retval DT_ERROR_OK：设置电流值成功
/// @retval DT_ERROR_FAILED：设置电流值失败
/// @retval DT_ERROR_INTERNAL_ERROR：内部错误
///
/// @note 此函数功能仅HS和PE系列设置有效
DTCCM_API int _DTCALL_ SetSensorCurrentLimit(int iLimit,int iDevID=DEFAULT_DEV_ID);

/// @brief 通过Reset,PWDN管脚开启或关闭SENSOR。
///
/// @param byPin：Reset，PWDN，PWDN2
/// @param bEnable：开启或关闭SENSOR
///
/// @retval DT_ERROR_OK：开启或关闭SENSOR操作成功
/// @retval DT_ERROR_FAILED：开启或关闭SENSOR操作失败
/// @retval DT_ERROR_COMM_ERROR：通讯错误
DTCCM_API int _DTCALL_ SensorEnable(BYTE byPin, BOOL bEnable,int iDevID=DEFAULT_DEV_ID);

/// @brief 检查Reset,PWDN管脚是否对SENSOR起作用。
///
/// @param pInfo：SENSOR的参数信息
/// @param bRstChkEn：使能检查Reset
/// @param bPwdnChkEn：使能检查PWDN
/// @param byChannel：A/B通道选择
/// @param pErrorx：返回的Reset和PWDN的检查信息
///
/// @retval DT_ERROR_OK：操作成功
DTCCM_API int _DTCALL_ CheckRstPwdnPin(SensorTab *pInfo, BOOL bRstChkEn, BOOL bPwdnChkEn, BYTE byChannel,BYTE *pErrorx,int iDevID=DEFAULT_DEV_ID);

/// @brief 检查Reset,PWDN管脚是否对SENSOR起作用。
///
/// @param pInfo：SENSOR的参数信息
/// @param bRstChkEn：使能检查Reset
/// @param bPwdnChkEn：使能检查PWDN
/// @param byChannel：A/B通道选择
/// @param pErrorx：返回的Reset和PWDN的检查信息
///
/// @retval DT_ERROR_OK：操作成功
DTCCM_API int _DTCALL_ CheckRstPwdnPin2(SensorTab2 *pInfo, BOOL bRstChkEn, BOOL bPwdnChkEn, BYTE byChannel,BYTE *pErrorx,int iDevID=DEFAULT_DEV_ID);


/// @brief 设置SENSOR的输入时钟。
///
/// @param bOnOff：使能SENSOR的输入时钟，为TRUE开启输入时钟，为FALSE关闭输入时钟
/// @param uHundKhz：SENSOR的输入时钟值，单位为100Khz
///
/// @retval DT_ERROR_OK：设置SENSOR输入时钟成功
/// @retval DT_ERROR_FAILED：设置SENSOR输入时钟失败
/// @retval DT_ERROR_COMM_ERROR：通讯错误
DTCCM_API int _DTCALL_ SetSensorClock(BOOL bOnOff,USHORT uHundKhz,int iDevID=DEFAULT_DEV_ID);

/// @brief 获取MIPI接口的同步时钟频率。
///
/// @param pFreq：返回MIPI接口的同步时钟频率值
/// @param byChannel：A通道/B通道
///
/// @retval DT_ERROR_OK：获取MIPI接口的同步时钟频率成功
/// @retval DT_ERROR_FAILED：获取MIPI接口的同步时钟频率失败
/// @retval DT_ERROR_PARAMETER_INVALID：通道参数无效
/// @retval DT_ERROR_COMM_ERROR：通讯错误
DTCCM_API int _DTCALL_ GetMipiClkFrequency(ULONG *pFreq,BYTE byChannel=CHANNEL_A,int iDevID=DEFAULT_DEV_ID);

/// @brief 设置MIPI接口接收器时钟相位。
///
/// @param byPhase：MIPI接口接收器时钟相位（可以设置的值是0-7）
///
/// @retval DT_ERROR_OK：设置MIPI接口接收器时钟相位成功
/// @retval DT_ERROR_FAILED：设置MIPI接口接收器时钟相位失败
/// @retval DT_ERROR_TIME_OUT：设置超时
/// @retval DT_ERROR_COMM_ERROR：通讯错误
DTCCM_API int _DTCALL_ SetMipiClkPhase(BYTE byPhase,int iDevID=DEFAULT_DEV_ID);

/// @brief 设置MIPI管脚使能信号
///
/// @param bEnable：为False时MIPI进入HS状态，为TRUE进入LP状态
///
/// @retval DT_ERROR_OK：ROI设置成功
/// @retval DT_ERROR_FAILD：ROI设置失败
/// @retval DT_ERROR_COMM_ERROR：通讯错误
DTCCM_API int _DTCALL_ SetMipiEnable(BOOL bEnable, int iDevID=DEFAULT_DEV_ID);

/// @brief 过滤MIPI包,指定要过滤的包的ID号及过滤的包个数
///
/// @param PackID[]：设置要过滤的包的ID号
/// @param iCount：过滤的包ID的个数，DMU950最大64
///
/// @retval DT_ERROR_OK：ROI设置成功
/// @retval DT_ERROR_FAILD：ROI设置失败
/// @retval DT_ERROR_COMM_ERROR：通讯错误
DTCCM_API int _DTCALL_ SetMipiPacketFilter(int PackID[],int iCount,int iDevID=DEFAULT_DEV_ID);

/// @brief 获取过滤的包的ID号
///
/// @param PackID[]：返回的过滤的包的ID号
/// @param pCount：返回过滤包的个数
/// @param MaxCount：设置要获取的包ID的最大数，DMU950最大64
///
/// @retval DT_ERROR_OK：ROI设置成功
/// @retval DT_ERROR_FAILD：ROI设置失败
/// @retval DT_ERROR_COMM_ERROR：通讯错误
DTCCM_API int _DTCALL_ GetMipiPacketFilter(int PackID[], int *pCount, int MaxCount,int iDevID=DEFAULT_DEV_ID);

/// @brief 获取MIPI接口接收器时钟相位值。
///
/// @param pPhase：返回MIPI接口接收器时钟相位值
///
/// @retval DT_ERROR_OK：获取MIPI接口接收器时钟相位值成功
/// @retval DT_ERROR_FAILED：获取MIPI接口接收器时钟相位值失败
/// @retval DT_ERROR_COMM_ERROR：通讯错误
DTCCM_API int _DTCALL_ GetMipiClkPhase(BYTE *pPhase,int iDevID=DEFAULT_DEV_ID);

/// @brief 获取MIPI接口接收器CRC错误计数。
///
/// @param pCount：返回MIPI接口接收器CRC错误计数值
/// @param byChannel：A/B通道选择
///
/// @retval DT_ERROR_OK：获取MIPI接口接收器CRC错误计数值成功
/// @retval DT_ERROR_FAILED：获取MIPI接口接收器CRC错误计数值失败
/// @retval DT_ERROR_COMM_ERROR：通讯错误
DTCCM_API int _DTCALL_ GetMipiCrcErrorCount(UINT *pCount,BYTE byChannel=CHANNEL_A,int iDevID=DEFAULT_DEV_ID);

/// @brief 获取MIPI接口接收器ECC错误计数。
///
/// @param pCount：返回MIPI接口接收器ECC错误计数值
/// @param byChannel：A/B通道选择
///
/// @retval DT_ERROR_OK：获取MIPI接口接收器ECC错误计数值成功
/// @retval DT_ERROR_FAILED：获取MIPI接口接收器ECC错误计数值失败
/// @retval DT_ERROR_COMM_ERROR：通讯错误
DTCCM_API int _DTCALL_ GetMipiEccErrorCount(UINT *pCount,BYTE byChannel=CHANNEL_A,int iDevID=DEFAULT_DEV_ID);


/// @brief 获取MIPI接口的LOCK状态。
///
/// @param pMipiLockState：获取MIPI接口的LOCK状态，从bit0-bit3依次表示LANE1、LANE2、LANE3、LANE4
///
/// @retval DT_ERROR_OK：获取MIPI接口LOCK状态成功
/// @retval DT_ERROR_FAILED：获取LOCK状态失败
/// @retval DT_ERROR_COMM_ERROR：通讯错误
DTCCM_API int _DTCALL_ GetMipiLockState(DWORD *pMipiLockState,BYTE byChannel,int iDevID=DEFAULT_DEV_ID);

/// @brief 设置MIPI接口控制器。
///
/// @param dwCtrl：MIPI接口控制器操作码，参见宏定义“MIPI控制器特性的位定义”
///
/// @retval DT_ERROR_OK：设置MIPI接口控制器成功
/// @retval DT_ERROR_FAILED：设置MIPI接口控制器失败
/// @retval DT_ERROR_COMM_ERROR：通讯错误
DTCCM_API int _DTCALL_ SetMipiCtrl(DWORD dwCtrl,int iDevID=DEFAULT_DEV_ID);

/// @brief 设置虚拟通道号
/// 
/// @param uVC：设置接收的虚拟通道号，0/1/2/3
/// @param bVCFileterEn：使能过滤其他的虚拟通道
///
/// @retval DT_ERROR_OK：设置MIPI接口控制器成功
/// @retval DT_ERROR_FAILED：设置MIPI接口控制器失败
/// @retval DT_ERROR_COMM_ERROR：通讯错误
DTCCM_API int _DTCALL_ SetMipiImageVC(UINT uVC, BOOL bVCFilterEn, BYTE byChannel=CHANNEL_A, int iDevID=DEFAULT_DEV_ID);


/// @brief 获取MIPI接口控制码
/// 
/// @param pdwCtrl：返回MIPI接口控制码
///
/// @retval DT_ERRORO_OK： 获取成功
DTCCM_API int _DTCALL_ GetMipiCtrl(DWORD *pdwCtrl,int iDevID=DEFAULT_DEV_ID);


/// @brief 设置并行接口控制器。
///
/// @param dwCtrl：并行接口控制器操作码，参见宏定义“同步并行接口特性的位定义”
///
/// @retval DT_ERROR_OK：设置并行接口控制器成功
/// @retval DT_ERROR_FAILED：设置并行接口控制器失败
/// @retval DT_ERROR_COMM_ERROR：通讯错误
DTCCM_API int _DTCALL_ SetParaCtrl(DWORD dwCtrl,int iDevID=DEFAULT_DEV_ID);

/// @brief 设置HiSPI接口控制器。
///
/// @param dwCtrl：HiSPI接口控制器操作码，参见宏定义“HiSPI接口特性的位定义”
///
/// @retval DT_ERROR_OK：设置HiSPI接口控制器成功
/// @retval DT_ERROR_FAILED：设置HiSPI接口控制器失败
/// @retval DT_ERROR_COMM_ERROR：通讯错误
DTCCM_API int _DTCALL_ SetHispiCtrl(DWORD dwCtrl,int iDevID=DEFAULT_DEV_ID);

/// @brief 设置模拟图像控制器。
///
/// @param dwCtrl：模拟图像控制器操作码
/// @param byRefByte：模拟图像数据定义
///
/// @retval DT_ERROR_OK：设置模拟图像控制器成功
/// @retval DT_ERROR_FAILED：设置模拟图像控制器失败
/// @retval DT_ERROR_COMM_ERROR：通讯错误
DTCCM_API int _DTCALL_ SetSimCtrl(DWORD dwCtrl,BYTE byRefByte,int iDevID=DEFAULT_DEV_ID);

/// @brief 初始化SENSOR。
///
/// @param uDevAddr：SENSOR器件地址
/// @param pParaList：SENSOR的参数列表
/// @param uLength：pParaList的大小
/// @param byI2cMode：访问SENSOR的I2C模式，参见枚举类型I2CMODE
///
/// @retval DT_ERROR_OK：初始化SENSOR成功
/// @retval DT_ERROR_FAILED：初始化SENSOR失败
///
/// @see I2CMODE
DTCCM_API int _DTCALL_ InitSensor(UCHAR uDevAddr,USHORT *pParaList,USHORT uLength,BYTE byI2cMode,int iDevID=DEFAULT_DEV_ID);

/// @brief 初始化SENSOR。
///
/// @param uDevAddr：SENSOR器件地址
/// @param pParaList：SENSOR的参数列表
/// @param uLength：pParaList的大小
/// @param byI2cMode：访问SENSOR的I2C模式，参见枚举类型I2CMODE
///
/// @retval DT_ERROR_OK：初始化SENSOR成功
/// @retval DT_ERROR_FAILED：初始化SENSOR失败
///
/// @see I2CMODE
DTCCM_API int _DTCALL_ InitSensor2(UINT uDevAddr,UINT *pParaList,UINT uLength,UINT byI2cMode,int iDevID=DEFAULT_DEV_ID);


/// @brief 检查插上的SENSOR是不是当前指定的，并且可以同时给SENSOR进行一次复位。
///
/// @param pInfo：SENSOR信息，参见SensorTab结构体
/// @param byChannel：通道选择，A/B通道，参见宏定义“多SENSOR模组通道定义”
/// @param bReset：给SENSOR复位
///
/// @retval DT_ERROR_OK：找到SENSOR
/// @retval DT_ERROR_FAILED：没有找到SENSOR
/// @retval DT_ERROR_COMM_ERROR：通讯错误
DTCCM_API int _DTCALL_ SensorIsMe(SensorTab *pInfo,BYTE byChannel,BOOL bReset,int iDevID=DEFAULT_DEV_ID);

/// @brief 检查插上的SENSOR是不是当前指定的，并且可以同时给SENSOR进行一次复位。
///
/// @param pInfo：SENSOR信息，参见SensorTab2结构体
/// @param byChannel：通道选择，A/B通道，参见宏定义“多SENSOR模组通道定义”
/// @param bReset：给SENSOR复位
///
/// @retval DT_ERROR_OK：找到SENSOR
/// @retval DT_ERROR_FAILED：没有找到SENSOR
/// @retval DT_ERROR_COMM_ERROR：通讯错误
DTCCM_API int _DTCALL_ SensorIsMe2(SensorTab2 *pInfo,BYTE byChannel,BOOL bReset,int iDevID=DEFAULT_DEV_ID);


/// @brief 通道选择
/// 
/// @param byI2cChannel：通道选择，A/B通道，参见宏定义“多SENSOR模组通道定义”
/// 
/// @retval DT_ERROR_OK：设置成功
/// @retval DT_ERROR_FAILED：设置失败
/// @retval DT_ERROR_COMM_ERROR：通讯错误
DTCCM_API int _DTCALL_ SetChannelSel(BYTE byI2cChannel,int iDevID=DEFAULT_DEV_ID);

/// @brief 图像通道使能
/// 
/// @param byI2cChannel：通道选择，A/B通道，参见宏定义“多SENSOR模组通道定义”
/// 
/// @retval DT_ERROR_OK：设置成功
/// @retval DT_ERROR_FAILED：设置失败
/// @retval DT_ERROR_COMM_ERROR：通讯错误
DTCCM_API int _DTCALL_ SetGrabChannelEnable(BYTE byGrabChannelEnabel,int iDevID=DEFAULT_DEV_ID);

/// @brief 设置柔性接口。
///
/// @param PinConfig：柔性接口配置定义
///
/// @retval DT_ERROR_OK：柔性接口配置成功
/// @retval DT_ERROR_FAILED：柔性接口配置失败
/// @retval DT_ERROR_COMM_ERROR：通讯错误
DTCCM_API int _DTCALL_ SetSoftPin(BYTE PinConfig[26],int iDevID=DEFAULT_DEV_ID);

/// @brief 设置柔性接口是否使能上拉电阻。
///
/// @param bPullup：柔性接口上拉使能，bPullup=TRUE使能上拉电阻，bPullup=FALSE关闭上拉电阻
///
/// @retval DT_ERROR_OK：设置成功
/// @retval DT_ERROR_FAILED：设置失败
/// @retval DT_ERROR_COMM_ERROR：通讯错误
DTCCM_API int _DTCALL_ SetSoftPinPullUp(BOOL bPullup,int iDevID=DEFAULT_DEV_ID);

/// @brief 设置是否使能柔性接口，没使能时为高阻状态。
///
/// @param bEnable：柔性接口使能
///
/// @retval DT_ERROR_OK：设置成功
/// @retval DT_ERROR_FAILED：设置失败
/// @retval DT_ERROR_COMM_ERROR：通讯错误
DTCCM_API int _DTCALL_ EnableSoftPin(BOOL bEnable,int iDevID=DEFAULT_DEV_ID);

/// @brief 获取柔性接口电平，每BIT对应一个管脚，bit0对应PIN_1。
/// 
/// @param pPinLevel：柔性接口电平
///
/// @retval DT_ERROR_OK：获取柔性接口电平成功
/// @retval DT_ERROR_FAILED：获取柔性接口电平失败
/// @retval DT_ERROR_COMM_ERROR：通讯错误
DTCCM_API int _DTCALL_ GetSoftPinLevel(DWORD *pPinLevel,int iDevID=DEFAULT_DEV_ID);

/// @brief 使能GPIO。
///
/// @param bEnable：使能GPIO
///
/// @retval DT_ERROR_OK：设置成功
/// @retval DT_ERROR_FAILED：设置失败
/// @retval DT_ERROR_COMM_ERROR：通讯错误
DTCCM_API int _DTCALL_ EnableGpio(BOOL bEnable,int iDevID=DEFAULT_DEV_ID);

/// @brief 设置单个GPIO的电平，iPin的编号值从1开始，1表示GPIO1。
///
/// @param iPin：GPIO编号值，iPin的编号值从1开始，1表示GPIO1
/// @param bLevel：设置GPIO的电平
///
/// @retval DT_ERROR_OK：设置单个GPIO的电平值成功
/// @retval DT_ERROR_FAILED：设置单个GPIO的电平值失败
/// @retval DT_ERROR_PARAMETER_INVALID：GPIO编号值参数无效
/// @retval DT_ERROR_COMM_ERROR：通讯错误
DTCCM_API int _DTCALL_ SetGpioPinLevel(int iPin,BOOL bLevel,int iDevID=DEFAULT_DEV_ID);

/// @brief 获取某个GPIO的电平，如果是输入，则是外部电平；如果是输出，则为设置电平。  
///
/// @param iPin：GPIO编号值，iPin的编号值从1开始，1表示GPIO1
/// @param pLevel：GPIO的电平值
///
/// @retval DT_ERROR_OK：获取GPIO的电平值成功
/// @retval DT_ERROR_FAILED：获取GPIO的电平值失败
/// @retval DT_ERROR_PARAMETER_INVALID：GPIO编号值参数无效
/// @retval DT_ERROR_COMM_ERROR：通讯错误
DTCCM_API int _DTCALL_ GetGpioPinLevel(int iPin,BOOL *pLevel,int iDevID=DEFAULT_DEV_ID);

/// @brief 设置多个GPIO的电平，Pin中存储的编号值从1开始，1表示GPIO1。
///
/// @param Pin：GPIO编号
/// @param Level：GPIO电平值
/// @param iCount：要设置的GPIO数量
///
/// @retval DT_ERROR_OK：设置多个GPIO的电平成功
/// @retval DT_ERROR_FAILED：设置多个GPIO的电平失败
/// @retval DT_ERROR_PARAMETER_INVALID：GPIO编号值参数无效
/// @retval DT_ERROR_COMM_ERROR：通讯错误
DTCCM_API int _DTCALL_ SetMultiGpioPinLevel(int Pin[],BOOL Level[],int iCount,int iDevID=DEFAULT_DEV_ID);

/// @brief 设置某个GPIO的IO管脚方向，bDir：TRUE为输入，FALSE为输出。
///
/// @param iPin：GPIO编号
/// @param bDir：GPIO管脚方向,bDir：TRUE为输入，FALSE为输出
///
/// @retval DT_ERROR_OK：设置GPIO的IO管脚方向成功
/// @retval DT_ERROR_FAILED：设置GPIO的IO管脚方向失败
/// @retval DT_ERROR_PARAMETER_INVALID：GPIO编号值参数无效
/// @retval DT_ERROR_COMM_ERROR：通讯错误
DTCCM_API int _DTCALL_ SetGpioPinDir(int iPin, BOOL bDir,int iDevID=DEFAULT_DEV_ID);

/// @brief 设置多个GPIO的IO管脚方向，Dir：TRUE为输入，FALSE为输出 。
///
/// @param Pin：GPIO编号
/// @param Dir：TRUE为输入，FALSE为输出
/// @param iCount：要设置的GPIO数量
///
/// @retval DT_ERROR_OK：设置多个GPIO的IO管脚方向成功
/// @retval DT_ERROR_FAILED：设置多个GPIO的IO管脚方向失败
/// @retval DT_ERROR_PARAMETER_INVALID：GPIO编号值参数无效
/// @retval DT_ERROR_COMM_ERROR：通讯错误
DTCCM_API int _DTCALL_ SetMultiGpioPinDir(int Pin[], BOOL Dir[],int iCount,int iDevID=DEFAULT_DEV_ID);

/// @brief 获取某个GPIO的电平，如果是输入，则是外部电平；如果是输出，则为设置电平。
///
/// @param Pin：GPIO管脚编号
/// @param Level：GPIO电平
/// @param iCount：要设置的GPIO数量
///
/// @retval DT_ERROR_OK：获取某个GPIO的IO管脚方向成功
/// @retval DT_ERROR_FAILED：获取某个GPIO的IO管脚方向失败
/// @retval DT_ERROR_PARAMETER_INVALID：GPIO编号值参数无效
/// @retval DT_ERROR_COMM_ERROR：通讯错误
DTCCM_API int _DTCALL_ GetMultiGpioPinLevel(int Pin[],BOOL Level[],int iCount,int iDevID=DEFAULT_DEV_ID);
/****************************************************************************************
SPI总线
*****************************************************************************************/
/// @brief SPI控制器使能、速度、模式设置（UT300/UV910），注意：在操作SensorSpiRW之前必须使能SPI总线，如果不操作必须关闭SPI总线，以免影响SENSOR i2c的访问。
/// 
/// @param bEnable：为True是开启SPI总线，为False是关闭SPI总线
/// @param iRate：为True是500Kbps 为False是250Kbps
/// @param bType：三线或四线通讯选择，为False是三线SPI通讯，为True是四线SPI通讯
///
/// @retval DT_ERROR_OK：操作成功
///
/// @note 在操作SensorSpiRW之前必须使能SPI总线，如果不操作必须关闭SPI总线，以免影响SENSOR i2c的访问
DTCCM_API int _DTCALL_ SensorSpiInit(BOOL bEnable, int iRate=0, BOOL bType=FALSE, int iDevID=DEFAULT_DEV_ID);

/// @brief SPI控制器接口，支持三线或四线通讯，支持单字节传输和多字节连续传输，支持读写要求。
///
/// @param bStart：为True片选信号拉低，为False片选信号不拉低
/// @param bStop：为True片选信号拉高，为False片选信号不拉高
/// @param bMsb：为True是高位先出，为False是低位先出
/// @param TxData：写入的数据BUFFER
/// @param RxData：读回的数据BUFFER
/// @param TxLen：写入数据的大小，字节数
/// @param RxLen：读取数据的大小，字节数

/// @retval DT_ERROR_OK：操作成功
/// @retval DT_ERROR_FAILD：操作失败
/// @retval DT_ERROR_COMM_ERROR：通讯错误
DTCCM_API int _DTCALL_ SensorSpiRW(BOOL bStart,BOOL bStop,BOOL bMsb,UCHAR *TxData,UCHAR *RxData,UCHAR TxLen,UCHAR RxLen,int iDevID=DEFAULT_DEV_ID);



/****************************************************************************************
柔性接口时钟输出
*****************************************************************************************/
/// @brief 柔性接口可调节时钟，支持柔性管脚输出0-200Khz可调节时钟。
///
/// @param uHz：输出时钟大小，单位Hz，0-200Khz可调节
/// @param bOnOff：可调节时钟输出使能，True为输出使能，False为关闭输出
///
/// @retval DT_ERROR_OK：设置时钟输出成功
/// @retval DT_ERROR_FAILD：设置时钟输出失败
/// @retval DT_ERROR_COMM_ERROR：通讯错误
DTCCM_API int _DTCALL_ SetSoftPinAdjClk1(UINT uHz,BOOL bOnOff,int iDevID=DEFAULT_DEV_ID);

/// @brief 柔性接口可调节时钟，支持柔性管脚输出0-18Mhz可调节时钟。
/// 
/// @param uHundkHz：设置输出时钟大小，单位100KHz，0-18Mhz可调节
/// @param bOnOff：可调节时钟输出使能，True为输出使能，False为关闭输出
///
/// @retval DT_ERROR_OK：设置时钟输出成功
/// @retval DT_ERROR_FAILD：设置时钟输出失败
/// @retval DT_ERROR_COMM_ERROR：通讯错误
DTCCM_API int _DTCALL_ SetSoftPinAdjClk2(UINT uHundkHz,BOOL bOnOff,int iDevID=DEFAULT_DEV_ID);

/** @} */ // end of group3

/******************************************************************************
图像数据采集相关
******************************************************************************/
/** @defgroup group4 图像数据采集相关


* @{

*/
/// @brief 开启图像数据采集。
///
/// @param uImgBytes：图像数据大小，单位字节
///
/// @retval DT_ERROR_OK：开启图像数据采集成功
/// @retval DT_ERROR_FAILD：开启图像数据采集失败
/// @retval DT_ERROR_INTERNAL_ERROR：内部错误
/// @retval DT_ERROR_COMM_ERROR：通讯错误
DTCCM_API int _DTCALL_ OpenVideo(UINT uImgBytes,int iDevID=DEFAULT_DEV_ID);

/// @brief 关闭图像数据采集。
///
/// @retval DT_ERROR_OK：关闭图像数据采集成功
/// @retval DT_ERROR_FAILD：关闭图像数据采集失败
/// @retval DT_ERROR_COMM_ERROR：通讯错误
DTCCM_API int _DTCALL_ CloseVideo(int iDevID=DEFAULT_DEV_ID);

/// @brief 设置采集一帧数据量
/// 
/// @param uGrabFrameSize：设置一帧的数据量大小，单位字节
DTCCM_API int _DTCALL_ SetGrabFrameSize(ULONG uGrabFrameSize,int iDevID=DEFAULT_DEV_ID);


/// @brief 设置SENSOR图像数据接口类型。
///
/// @param byPort：SENSOR图像数据接口类型，参见枚举类型“SENSOR_PORT”
/// @param uWidth：图像数据宽度
/// @param uHeight：图像数据高度
///
/// @retval DT_ERROR_OK：设置SENSOR图像数据接口类型成功
/// @retval DT_ERROR_FAILD：设置SENSOR图像数据接口类型失败 
/// @retval DT_ERROR_PARAMETER_INVALID：无效的图像数据接口类型参数
///
/// @see SENSOR_PORT
DTCCM_API int _DTCALL_ SetSensorPort(BYTE byPort,USHORT uWidth,USHORT uHeight,int iDevID=DEFAULT_DEV_ID);

/// @brief 复位SENSOR图像数据接口
/// 
/// @param byPort：SENSOR图像数据接口类型，参见枚举类型“SENSOR_PORT”
///
/// @retval DT_ERROR_OK：设置成功
/// @retval DT_ERROR_FAILD：设置失败
/// @retval DT_ERROR_COMM_ERROR：通讯错误
///
/// @see SENSOR_PORT
DTCCM_API int _DTCALL_ ResetSensorPort(BYTE byPort,int iDevID=DEFAULT_DEV_ID);

/// @brief 配置FrameBuffer
/// 
/// @param pConfig：FrameBuffer配置结构体,该结构体说明参见imagekit.h头文件
DTCCM_API int _DTCALL_ SetFrameBufferConfig(FrameBufferConfig *pConfig,int iDevID=DEFAULT_DEV_ID);

/// @brief 获取FrameBuffer的配置信息
/// 
/// @param pConfig：FrameBuffer配置结构体,该结构体说明参见imagekit.h头文件
DTCCM_API int _DTCALL_ GetFrameBufferConfig(FrameBufferConfig *pConfig,int iDevID=DEFAULT_DEV_ID);

DTCCM_API int _DTCALL_ ResetFrameBuffer(int iDevID=DEFAULT_DEV_ID);

/// @brief 设置ROI。
///
/// @param roi_x0：起始点水平坐标，单位字节
/// @param roi_y0：起始点垂直坐标，单位字节
/// @param roi_hw：水平方向ROI图像宽度，单位字节
/// @param roi_vw：垂直方向ROI图像高度，单位字节
/// @param roi_hb：水平方向blank宽度，单位字节
/// @param roi_vb：水平方向blank高度，单位字节
/// @param roi_hnum：水平方向ROI数量，单位字节
/// @param roi_vnum：垂直方向ROI数量，单位字节
/// @param roi_en：ROI使能
///
/// @retval DT_ERROR_OK：ROI设置成功
/// @retval DT_ERROR_FAILD：ROI设置失败
/// @retval DT_ERROR_COMM_ERROR：通讯错误
///
/// @note 该函数中指定宽度和水平位置是以字节为单位，并且保证宽度为4字节的整数倍。
///
/// @image html roi.png

DTCCM_API int _DTCALL_ SetRoi(USHORT roi_x0, 
							  USHORT roi_y0, 
							  USHORT roi_hw, 
							  USHORT roi_vw, 
							  USHORT roi_hb, 
							  USHORT roi_vb, 
							  USHORT roi_hnum, 
							  USHORT roi_vnum, 
							  BOOL roi_en,
							  int iDevID=DEFAULT_DEV_ID);

/// @brief 设置ROI。
///
/// @param roi_x0：起始点水平坐标，单位像素
/// @param roi_y0：起始点垂直坐标，单位像素
/// @param roi_hw：水平方向ROI图像宽度，单位像素
/// @param roi_vw：垂直方向ROI图像高度，单位像素
/// @param roi_hb：水平方向blank宽度，单位像素
/// @param roi_vb：水平方向blank高度，单位像素
/// @param roi_hnum：水平方向ROI数量，单位像素
/// @param roi_vnum：垂直方向ROI数量，单位像素
/// @param roi_en：ROI使能
///
/// @retval DT_ERROR_OK：ROI设置成功
/// @retval DT_ERROR_FAILD：ROI设置失败
/// @retval DT_ERROR_COMM_ERROR：通讯错误
///
/// @note 该函数中指定宽度和水平位置是以像素为单位，并且要保证宽度转为字节后是16字节的整数倍。
///
/// @image html roi.png
DTCCM_API int _DTCALL_ SetRoiEx(USHORT roi_x0, 
								USHORT roi_y0, 
								USHORT roi_hw, 
								USHORT roi_vw, 
								USHORT roi_hb, 
								USHORT roi_vb, 
								USHORT roi_hnum, 
								USHORT roi_vnum, 
								BOOL roi_en,
								int iDevID=DEFAULT_DEV_ID);

/// @brief 设置采集数据超时，单位：ms，如果该事件内没有采集到图像数据，GrabFrame函数将返回超时错误。
///
/// @param uTimeOut：采集数据超时时间设置，单位ms
DTCCM_API int _DTCALL_ SetGrabTimeout(ULONG uTimeOut,int iDevID=DEFAULT_DEV_ID);

/// @brief 采集一帧图像，并且返回帧的一些信息，A通道和B通道都是使用GrabFrame函数获取图像数据，通过帧信息可以区分图像数据所属的通道。
///
/// @param pInBuffer：采集图像BUFFER
/// @param uBufferSize：采集图像BUFFER大小，单位字节
/// @param pGrabSize：实际抓取的图像数据大小，单位字节
/// @param pInfo：返回的图像数据信息
///
/// @retval DT_ERROR_OK：采集一帧图像成功
/// @retval DT_ERROR_FAILD：采集一帧图像失败，可能不是完整的一帧图像数据
/// @retval DT_ERROR_TIME_OUT：采集超时
/// @retval DT_ERROR_INTERNAL_ERROR：内部错误 
/// 
/// @note 调用该函数之前，请先根据图像大小获取到足够大的缓存区用于装载图像数据。\n
/// 同时，缓存区的大小也需要作为参数传入到GrabFrame函数，以防止异常情况下导致的内存操作越界问题。 
DTCCM_API int _DTCALL_ GrabFrame(BYTE *pInBuffer, ULONG uBufferSize,ULONG *pGrabSize,FrameInfo *pInfo,int iDevID=DEFAULT_DEV_ID);


/// @brief 采集一帧图像，并且返回帧的一些信息，通过帧信息结构体可以获取帧的时间戳、ECC、CRC等。
///
/// @param pInBuffer：采集图像BUFFER
/// @param uBufferSize：采集图像BUFFER大小，单位字节
/// @param pGrabSize：实际抓取的图像数据大小，单位字节
/// @param pInfo：返回的图像数据信息
///
/// @retval DT_ERROR_OK：采集一帧图像成功
/// @retval DT_ERROR_FAILD：采集一帧图像失败，可能不是完整的一帧图像数据
/// @retval DT_ERROR_TIME_OUT：采集超时
/// @retval DT_ERROR_INTERNAL_ERROR：内部错误 
/// 
/// @note 调用该函数之前，请先根据图像大小获取到足够大的缓存区用于装载图像数据。\n
/// 同时，缓存区的大小也需要作为参数传入到GrabFrameEx函数，以防止异常情况下导致的内存操作越界问题。 
DTCCM_API int _DTCALL_ GrabFrameEx(BYTE *pInBuffer, ULONG uBufferSize,ULONG *pGrabSize,FrameInfoEx *pInfo,int iDevID=DEFAULT_DEV_ID);

/// @brief 图像抓拍设置，设置使能和一次抓拍的帧数。
///
/// @param bSnapEn：图像抓拍使能
/// @param uSnapCount：一次抓拍的帧数
///
/// @retval DT_ERROR_OK：图像抓拍设置成功
/// @retval DT_ERROR_FAILD：图像抓拍设置失败 
/// @retval DT_ERROR_COMM_ERROR：通讯错误 
DTCCM_API int _DTCALL_ SetSnap(BOOL bSnapEn, UINT uSnapCount,int iDevID=DEFAULT_DEV_ID);

/// @brief 启动一次抓拍。
///
/// @retval DT_ERROR_OK：设置启动一次抓拍成功
/// @retval DT_ERROR_FAILD：设置启动一次抓拍失败 
/// @retval DT_ERROR_COMM_ERROR：通讯错误 
DTCCM_API int _DTCALL_ TriggerSnap(int iDevID=DEFAULT_DEV_ID);

/// @brief 重发上一帧图像。
///
/// @retval DT_ERROR_OK：设置重发上一帧图像成功
/// @retval DT_ERROR_FAILD：设置重发上一帧图像失败 
/// @retval DT_ERROR_COMM_ERROR：通讯错误
DTCCM_API int _DTCALL_ ReSendImage(int iDevID=DEFAULT_DEV_ID);

/// @brief 获取采集图像的宽高信息，注意：*pWidth的单位为字节。
///
/// @param pWidth：获取的采集图像宽度信息
/// @param pHeight：获取的采集图像高度信息
/// @param byChannel：A/B通道选择
///
/// @retval DT_ERROR_OK：获取采集图像的宽高信息成功
/// @retval DT_ERROR_FAILD：获取采集图像的宽高信息失败 
/// @retval DT_ERROR_COMM_ERROR：通讯错误
DTCCM_API int _DTCALL_ GetGrabImageSize(USHORT *pWidth,USHORT *pHeight,BYTE byChannel,int iDevID=DEFAULT_DEV_ID);

/// @brief 返回实际抓取图像数据的大小（单位字节）。
/// 
/// @param pGrabSize：返回实际抓取图像数据大小
DTCCM_API int _DTCALL_ CalculateGrabSize(ULONG *pGrabSize,int iDevID=DEFAULT_DEV_ID);


/// @brief 获像素位置数据中的字节位置。
///
/// @param iPixelPos：像素位置
/// @param byImgFormat：图像数据格式
///
/// @retval 返回所给像素位置数据中的字节位置
DTCCM_API int _DTCALL_ GetByteOffset(int iPixelPos,BYTE byImgFormat,int iDevID=DEFAULT_DEV_ID);

/// @brief 设置时间点，设置成功后，设备的时间戳值将从该时间点开始计时（单位us）
/// 
/// @param uTimeStamp: 时间点
/// 
/// @retval DT_ERROR_OK：设置成功
/// @retval DT_ERROR_FAILD：设置失败 
/// @retval DT_ERROR_COMM_ERROR：通讯错误
DTCCM_API int _DTCALL_ SetTimerClock(double uTime, int iDevID = DEFAULT_DEV_ID);

/// @brief 获取时间戳(单位us)
/// 
/// @param pTimeStamp: 时间戳值
/// 
/// @retval DT_ERROR_OK：设置成功
/// @retval DT_ERROR_FAILD：设置失败 
/// @retval DT_ERROR_COMM_ERROR：通讯错误
DTCCM_API int _DTCALL_ GetTimeStamp(double *pTimeStamp, int iDevID = DEFAULT_DEV_ID);
/** @} */ // end of group4


/** @defgroup group5 电源管理单元相关


* @{

*/
/// @brief PMU复位，有可能导致系统电源重启，所有模块重新上电。
///
/// @retval DT_ERROR_OK：PMU复位成功
/// @retval DT_ERROR_FAILD：PMU复位失败
/// @retval DT_ERROR_INTERNAL_ERROR：内部错误
/// @retval DT_ERROR_COMM_ERROR：通讯错误
DTCCM_API int _DTCALL_ PmuReset(int iDevID=DEFAULT_DEV_ID);

/// @brief 获取系统电源状态(电压，电流)。
///
/// @param Power：系统电源类型，参见枚举类型“SYS_POWER”
/// @param Voltage：获取的系统电压值，单位mV
/// @param Current：获取的系统电流值，单位mA
/// @param iCount：要获取的系统电源的路数
///
/// @retval DT_ERROR_OK：获取系统电源状态成功
///	@retval DT_ERROR_COMM_ERROR：通讯错误
///
/// @see SYS_POWER
/// 
/// @note 该函数仅UV910/DTLC2/UH910/UH920/UF920/PE350/PE950支持。
DTCCM_API int _DTCALL_ PmuGetSysPowerVA(SYS_POWER Power[],int Voltage[],int Current[],int iCount,int iDevID=DEFAULT_DEV_ID);

/// @brief 设置电源模式。
///
/// @param Power：电源类型，参见枚举类型“SENSOR_POWER”
/// @param Mode：电源模式，参见枚举类型“POWER_MODE”
/// @param iCount：电源路数
/// 
/// @retval DT_ERROR_OK：设置电源模式成功
/// @retval DT_ERROR_FAILD：设置电源模式失败
/// @retval DT_ERROR_COMM_ERROR：通讯错误
/// @retval DT_ERROR_PARAMETER_OUT_OF_BOUND：参数超出了范围
///
/// @see SENSOR_POWER
/// @see POWER_MODE
/// 
/// @note 该函数仅UV910/DTLC2/UH910/UH920/UF920/PE350/PE950支持。
DTCCM_API int _DTCALL_ PmuSetMode(SENSOR_POWER Power[],POWER_MODE Mode[],int iCount,int iDevID=DEFAULT_DEV_ID);

/// @brief 设置电源电压。
///
/// @param Power：电源类型，参见枚举类型“SENSOR_POWER”
/// @param Voltage：设置的电源电压值，单位mV
/// @param iCount：电源路数
///
/// @retval DT_ERROR_OK：设置电源电压成功
/// @retval DT_ERROR_FAILD：设置电源电压失败
/// @retval DT_ERROR_COMM_ERROR：通讯错误
/// @retval DT_ERROR_PARAMETER_OUT_OF_BOUND：参数超出了范围
///
/// @see SENSOR_POWER
DTCCM_API int _DTCALL_ PmuSetVoltage(SENSOR_POWER Power[],int Voltage[],int iCount,int iDevID=DEFAULT_DEV_ID);

/// @brief 获取电源电压，如果能获取检测到的，尽量使用检测到的数据，否则返回电压设置值。
///
/// @param Power：电源类型，参见枚举类型“SENSOR_POWER”
/// @param Voltage：获取的电源电压值，单位mV
/// @param iCount：电源路数
///
/// @retval DT_ERROR_OK：设置电源电压成功
/// @retval DT_ERROR_FAILD：设置电源电压失败
/// @retval DT_ERROR_COMM_ERROR：通讯错误
/// @retval DT_ERROR_PARAMETER_OUT_OF_BOUND：参数超出了范围
///
/// @see SENSOR_POWER
DTCCM_API int _DTCALL_ PmuGetVoltage(SENSOR_POWER Power[],int Voltage[],int iCount,int iDevID=DEFAULT_DEV_ID);

/// @brief 设置电源电流量程。
///
/// @param Power：电源类型，参见枚举类型“SENSOR_POWER”
/// @param Range：电源电流量程，参见枚举类型“CURRENT_RANGE”
/// @param iCount：电源路数
///
/// @retval DT_ERROR_OK：设置电源电流量程成功
/// @retval DT_ERROR_FAILD：设置电源电流量程失败
///
/// @see SENSOR_POWER
/// @see CURRENT_RANGE
///
/// @note 该函数仅UV910/DTLC2/UH910/UH920/UF920/PE350/PE950支持。
DTCCM_API int _DTCALL_ PmuSetCurrentRange(SENSOR_POWER Power[],CURRENT_RANGE Range[],int iCount,int iDevID=DEFAULT_DEV_ID);

/// @brief 获取电源电流。
///
/// @param Power：电源类型，参见枚举类型“SENSOR_POWER”
/// @param Current：电流值,单位nA
/// @param iCount：电源路数
///
/// @retval DT_ERROR_OK：获取电源电流成功
/// @retval DT_ERROR_FAILD：获取电源电流失败
/// @retval DT_ERROR_COMM_ERROR：通讯错误
/// @retval DT_ERROR_PARAMETER_OUT_OF_BOUND：参数超出了范围
///
/// @see SENSOR_POWER
///
/// @note 该函数仅UV910/DTLC2/UH910/UH920/UF920/PE350/PE950支持。
DTCCM_API int _DTCALL_ PmuGetCurrent(SENSOR_POWER Power[],int Current[],int iCount,int iDevID=DEFAULT_DEV_ID);

/// @brief 设置电压上升沿，设定值(Rise)单位:mV/mS。
///
/// @param Power：电源类型，参见枚举类型“SENSOR_POWER”
/// @param Rise：电压上升沿值，单位:mV/mS
/// @param iCount：电源路数
///
/// @retval DT_ERROR_OK：设置电压上升沿成功
/// @retval DT_ERROR_FAILD：设置电压上升沿失败
/// @retval DT_ERROR_COMM_ERROR：通讯错误
/// @retval DT_ERROR_PARAMETER_OUT_OF_BOUND：参数超出了范围
///
/// @see SENSOR_POWER
/// 
/// @note 该函数仅UV910/DTLC2/UH910/UH920/UF920/PE350/PE950支持。
DTCCM_API int _DTCALL_ PmuSetRise(SENSOR_POWER Power[],int Rise[],int iCount,int iDevID=DEFAULT_DEV_ID);

/// @brief 设置电流采集速度，设定值(SampleSpeed)10-200。
///
/// @param Power：电源类型，参见枚举类型“SENSOR_POWER”
/// @param SampleSpeed：电流采集速度，范围10-200
/// @param iCount：电源路数
///
/// @retval DT_ERROR_OK：设置电流采集速度成功
/// @retval DT_ERROR_FAILD：设置电流采集速度失败
/// @retval DT_ERROR_COMM_ERROR：通讯错误
/// @retval DT_ERROR_PARAMETER_OUT_OF_BOUND：参数超出了范围
///
/// @see SENSOR_POWER
/// 
/// @note 该函数仅UV910/DTLC2/UH910/UH920/UF920/PE350/PE950支持。
DTCCM_API int _DTCALL_ PmuSetSampleSpeed(SENSOR_POWER Power[],int SampleSpeed[],int iCount,int iDevID=DEFAULT_DEV_ID);


/// @brief 设置电源开关状态。
///
/// @param Power：电源类型，参见枚举类型“SENSOR_POWER”
/// @param OnOff：设置电源开关状态，TRUE为开启，FALSE为关闭
/// @param iCount：电源路数
///
/// @retval DT_ERROR_OK：设置电源开关状态成功
/// @retval DT_ERROR_FAILD：设置电源开关状态失败
/// @retval DT_ERROR_COMM_ERROR：通讯错误
/// @retval DT_ERROR_PARAMETER_OUT_OF_BOUND：参数超出了范围
///
/// @see SENSOR_POWER
DTCCM_API int _DTCALL_ PmuSetOnOff(SENSOR_POWER Power[],BOOL OnOff[],int iCount,int iDevID=DEFAULT_DEV_ID);

/// @brief 获取电源开关状态。
///
/// @param Power：电源类型，参见枚举类型“SENSOR_POWER”
/// @param OnOff：获取电源开关状态，TRUE为开启，FALSE为关闭
/// @param iCount：电源路数
///
/// @retval DT_ERROR_OK：获取电源开关状态成功
/// @retval DT_ERROR_FAILD：获取电源开关状态失败
/// @retval DT_ERROR_COMM_ERROR：通讯错误
/// @retval DT_ERROR_PARAMETER_OUT_OF_BOUND：参数超出了范围
///
/// @see SENSOR_POWER
DTCCM_API int _DTCALL_ PmuGetOnOff(SENSOR_POWER Power[],BOOL OnOff[],int iCount,int iDevID=DEFAULT_DEV_ID);

/// @brief 设置过流保护的电流限制,设定值(CurrentLimit)单位:mA。
///
/// @param Power：电源类型，参见枚举类型“SENSOR_POWER”
/// @param CurrentLimit：设置过流保护的电流限制值，单位mA
/// @param iCount：电源路数
///
/// @retval DT_ERROR_OK：设置过流保护的电流限制成功
/// @retval DT_ERROR_FAILD：设置过流保护的电流限制失败
/// @retval DT_ERROR_COMM_ERROR：通讯错误
/// @retval DT_ERROR_PARAMETER_OUT_OF_BOUND：参数超出了范围
///
/// @see SENSOR_POWER
DTCCM_API int _DTCALL_ PmuSetOcpCurrentLimit(SENSOR_POWER Power[],int CurrentLimit[],int iCount,int iDevID=DEFAULT_DEV_ID);

/// @brief 设置OCP延时(过流时间的累加),设定值(Delay)单位:mS。
///
/// @param Power：电源类型，参见枚举类型“SENSOR_POWER”
/// @param Delay：OCP延时设定值
/// @param iCount：电源路数
///
/// @retval DT_ERROR_OK：设置OCP延时成功
/// @retval DT_ERROR_FAILD：设置OCP延时失败
/// @retval DT_ERROR_COMM_ERROR：通讯错误
/// @retval DT_ERROR_PARAMETER_OUT_OF_BOUND：参数超出了范围
///
/// @see SENSOR_POWER
/// 
/// @note 该函数仅UV910/DTLC2/UH910/UH920/UF920/PE350/PE950支持。
DTCCM_API int _DTCALL_ PmuSetOcpDelay(SENSOR_POWER Power[],int Delay[],int iCount,int iDevID=DEFAULT_DEV_ID);

/// @brief 设置OCP关断时间(被判断过流保护后，关断一段时间),设定值(Hold)单位:mS。
///
/// @param Power：电源类型，参见枚举类型“SENSOR_POWER”
/// @param Hold：OCP关断时间
/// @param iCount：电源路数
///
/// @retval DT_ERROR_OK：设置OCP关断时间成功
/// @retval DT_ERROR_FAILD：设置OCP关断时间失败
/// @retval DT_ERROR_COMM_ERROR：通讯错误
/// @retval DT_ERROR_PARAMETER_OUT_OF_BOUND：参数超出了范围
///
/// @see SENSOR_POWER
/// 
/// @note 该函数仅UV910/DTLC2/UH910/UH920/UF920/PE350/PE950支持。
DTCCM_API int _DTCALL_ PmuSetOcpHold(SENSOR_POWER Power[],int Hold[],int iCount,int iDevID=DEFAULT_DEV_ID);

/// @brief 获取过流状态,返回信息(State)TRUE:过流保护,FALSE:正常。
///
/// @param Power：电源类型，参见枚举类型“SENSOR_POWER”
/// @param State：获取到的过流状态
/// @param iCount：电源路数
///
/// @retval DT_ERROR_OK：设置OCP关断时间成功
/// @retval DT_ERROR_FAILD：设置OCP关断时间失败
/// @retval DT_ERROR_COMM_ERROR：通讯错误
/// @retval DT_ERROR_PARAMETER_OUT_OF_BOUND：参数超出了范围
///
/// @see SENSOR_POWER
/// 
/// @note 该函数仅UV910/DTLC2/UH910/UH920/UF920/PE350/PE950支持。
DTCCM_API int _DTCALL_ PmuGetOcpState(SENSOR_POWER Power[],BOOL State[],int iCount,int iDevID=DEFAULT_DEV_ID);

/// @brief 设置SENSOR电源开关。
///
/// @param bOnOff：SENSOR电源开关设置值，TRUE为开启，FALSE为关闭
///
/// @retval DT_ERROR_OK：设置SENSOR电源开关成功
/// @retval DT_ERROR_FAILD：设置SENSOR电源开关失败
/// @retval DT_ERROR_COMM_ERROR：通讯错误
/// @retval DT_ERROR_PARAMETER_OUT_OF_BOUND：参数超出了范围
/// 
/// @note 该函数仅UV910/DTLC2/UH910/UH920/UF920/PE350/PE950支持。
DTCCM_API int _DTCALL_ SetSensorPowerOnOff(BOOL bOnOff,int iDevID=DEFAULT_DEV_ID);


/** @} */ // end of group5


/******************************************************************************
初始化，控制相关
******************************************************************************/
/** @defgroup group6 初始化控制相关


* @{

*/
/// @brief 打开设备，只有打开成功后，设备才能操作;设备对象跟给的ID对应起来iDevID=1 则创建设备对象m_device[1]，iDevID=0 则创建设备对象m_device[0]；
///
/// @param pszDeviceName：打开设备的名称
/// @param pDevID：返回打开设备的ID号
///
/// @retval DT_ERROR_OK：打开设备成功
/// @retval DT_ERROR_FAILD：打开设备失败
/// @retval DT_ERROR_INTERNAL_ERROR：内部错误
/// @retval DT_ERROR_PARAMETER_INVALID：参数无效
DTCCM_API int _DTCALL_ OpenDevice(const char *pszDeviceName,int *pDevID,int iDevID=DEFAULT_DEV_ID);

/// @brief 重新连接设备，usb3.0设备有效
///
/// @retval DT_ERROR_OK：打开设备成功
/// @retval DT_ERROR_FAILD：打开设备失败
DTCCM_API int _DTCALL_ DeviceReset(int iDevID=DEFAULT_DEV_ID);


/// @brief 以升级固件模式打开设备，只有打开成功后，设备才能操作。
///
/// @param pszDeviceName：打开设备的名称
/// @param pDevID：返回打开设备的ID号
///
/// @retval DT_ERROR_OK：打开设备成功
/// @retval DT_ERROR_FAILD：打开设备失败
/// @retval DT_ERROR_INTERNAL_ERROR：内部错误
/// @retval DT_ERROR_PARAMETER_INVALID：参数无效
DTCCM_API int _DTCALL_ OpenDeviceUpgradeMode(const char *pszDeviceName,int *pDevID,int iDevID=DEFAULT_DEV_ID);

/// @brief 关闭设备，关闭设备后，不能再操作。
///
/// @retval DT_ERROR_OK：关闭设备成功
/// @retval DT_ERROR_FAILD：关闭设备失败
DTCCM_API int _DTCALL_ CloseDevice(int iDevID=DEFAULT_DEV_ID);

/// @brief 初始化设备，该函数主要用于初始化设备的SENSOR接口类型，图像格式，图像宽高信息，同时还要求用户传入用于视频显示的窗口句柄。
///
/// @param hWnd：显示A通道图像的窗口句柄
/// @param uImgWidth，uImgHeight：设置SENSOR输出的宽高信息（单位：像素，可能ROI之后的结果）
/// @param bySensorPort：SENSOR输出接口类型，如：MIPI/并行
/// @param byImgFormat：图像数据格式，如：RAW/YUV
/// @param byChannel：A通道/B通道/AB同时工作
/// @param hWndEx：显示B通道图像的窗口句柄
///
/// @retval DT_ERROR_OK：初始化成功
/// @retval DT_ERROR_FAILD：初始化失败
/// @retval DT_ERROR_PARAMETER_INVALID：bySensorPort参数无效
///
/// @note InitDevice函数支持初始化双通道测试板（如DTLC2/UH910），如果须要使用这类测试板的B通道，请做如下额外操作：
/// @note byChannel参数传入CHANNEL_A|CHANNEL_B；hWndEx参数传入用于B通道视频显示的窗口句柄。
DTCCM_API int _DTCALL_ InitDevice(HWND hWnd, 
								  USHORT uImgWidth, 
								  USHORT uImgHeight,
								  BYTE bySensorPort,
								  BYTE byImgFormat,
								  BYTE byChannel=CHANNEL_A,
								  HWND hWndEx=NULL,
								  int iDevID=DEFAULT_DEV_ID);

/// @brief 获取设备上按钮的按键值。
///
/// @param pKey：获取到的按键值
///
/// @retval DT_ERROR_OK：获取按键值成功
/// @retval DT_ERROR_FAILED：获取按键值失败
/// @retval DT_ERROR_COMM_ERROR：通讯错误
/// @retval DT_ERROR_INTERNAL_ERROR：内部错误
DTCCM_API int _DTCALL_ GetKey(DWORD *pKey,int iDevID=DEFAULT_DEV_ID);

/// @brief 设置报警喇叭。
///
/// @param uBeepTime：蜂鸣器鸣叫时间，单位为mS
/// @param iBeepCount：蜂鸣器鸣叫次数
///
/// @retval DT_ERROR_OK：设置报警喇叭成功
/// @retval DT_ERROR_FAILED：设置报警喇叭失败
/// @retval DT_ERROR_INTERNAL_ERROR：内部错误
DTCCM_API int _DTCALL_ SetAlarm(USHORT uBeepTime, int iBeepCount,int iDevID=DEFAULT_DEV_ID);
/** @} */ // end of group6

/// @brief 初始化ISP
/// 
/// @param uImgWidth：图像数据宽度
/// @param uHeight：图像数据高度
/// @param byImgFormat：图像数据格式，如：RAW/YUV
/// @param byChannel：A/B通道选择
DTCCM_API int _DTCALL_ InitIsp(USHORT uImgWidth,USHORT uImgHeight,BYTE byImgFormat,BYTE byChannel=CHANNEL_A,int iDevID=DEFAULT_DEV_ID);

/// @brief 设置ROI。
///
/// @param roi_x0：起始点水平坐标，单位像素
/// @param roi_y0：起始点垂直坐标，单位像素
/// @param roi_hw：水平方向ROI图像宽度，单位像素
/// @param roi_vw：垂直方向ROI图像高度，单位像素
/// @param roi_hb：水平方向blank宽度，单位像素
/// @param roi_vb：水平方向blank高度，单位像素
/// @param roi_hnum：水平方向ROI数量，单位像素
/// @param roi_vnum：垂直方向ROI数量，单位像素
/// @param byImgFormat：图像数据格式，如：RAW/YUV
/// @param roi_en：ROI使能
///
/// @retval DT_ERROR_OK：ROI设置成功
/// @retval DT_ERROR_FAILD：ROI设置失败
/// @retval DT_ERROR_COMM_ERROR：通讯错误
///
/// @note 该函数中指定宽度和水平位置是以像素为单位，并且要保证宽度转为字节后是16字节的整数倍。
///
/// @image html roi.png
DTCCM_API int InitRoi(USHORT roi_x0, 
					  USHORT roi_y0, 
					  USHORT roi_hw, 
					  USHORT roi_vw, 
					  USHORT roi_hb, 
					  USHORT roi_vb, 
					  USHORT roi_hnum, 
					  USHORT roi_vnum, 
					  BYTE byImgFormat,
					  BOOL roi_en,
					  int iDevID=DEFAULT_DEV_ID);
/******************************************************************************
AF相关
******************************************************************************/

/** @defgroup group7 AF相关


* @{

*/
/// @brief 获取AF器件的D/A芯片型号和器件地址。
///
/// @param pDevType：获取AF器件的D/A芯片型号
/// @param pDevAddr：获取AF器件的D/A器件地址
DTCCM_API int _DTCALL_ GetAfDevType(UCHAR *pDevType, UCHAR *pDevAddr, int iDevID=DEFAULT_DEV_ID);


/// @brief 设置AF器件的D/A芯片型号和器件地址。
///
/// @param uDevType：设置AF器件的D/A芯片型号
/// @param uDevAddr：设置AF器件的D/A器件地址
DTCCM_API int _DTCALL_ SetAfDevType(UCHAR uDevType, UCHAR uDevAddr, int iDevID=DEFAULT_DEV_ID);

/// @brief 设置AF器件的D/A的输出值。
///
/// @param uValue：设置的AF器件的D/A输出值
/// @param uMode：AF器件IC的模式
///
/// @retval DT_ERROR_OK：设置AF器件的D/A的输出值成功
/// @retval DT_ERROR_COMM_ERROR：通讯错误
/// @retval DT_ERROR_PARAMETER_INVALID：I2C模式无效
/// @retval DT_ERROR_TIME_OUT：通讯超时
/// @retval DT_ERROR_INTERNAL_ERROR：内部错误
/// @retval DT_ERROR_NOT_SUPPORTED：不支持的AF器件类型
/// @retval DT_ERROR_NOT_INITIALIZED：没有初始化AF器件的D/A器件地址
DTCCM_API int _DTCALL_ WriteAfValue(USHORT uValue, UCHAR uMode, int iDevID=DEFAULT_DEV_ID);


/// @brief 搜索AF器件中的D/A芯片。
///
/// @retval DT_ERROR_OK：搜索AF器件中的D/A芯片成功
/// @retval DT_ERROR_FAILD：搜索AF器件中的D/A芯片失败
DTCCM_API int _DTCALL_ FindAfDevice(int iDevID=DEFAULT_DEV_ID);
/** @} */ // end of group7


/****************************************************************************************
LC/OS相关
****************************************************************************************/

/** @defgroup group8 LC/OS相关


* @{

*/
/// @brief LC/OS测试操作配置。
/// 
/// @param Command：操作码，参见宏定义“OS/LC测试配置定义”
/// @param IoMask：有效管脚标识位，每字节的每bit对应一个管脚，如果这些bit为1，表示对应的管脚将参与测试
/// @param PinNum：管脚数，这个决定IoMask数组大小，一般情况下IoMask的字节数为：PinNum/8+(PinNum%8!=0)
///
/// @retval DT_ERROR_OK：LC/OS测试操作配置成功
/// @retval DT_ERROR_FAILD：LC/OS测试操作配置失败
/// @retval DT_ERROR_COMM_ERROR：通讯错误
DTCCM_API int _DTCALL_ LC_OS_CommandConfig(DWORD Command, UCHAR IoMask[], int PinNum, int iDevID=DEFAULT_DEV_ID);

/// @brief OS测试参数配置。
///
/// @param Voltage：测试电压，单位uV
/// @param HighLimit：Open测试标准数组，测试之前应该把每一个测试pin的开路标准初始化好，单位uV
/// @param LowLimit：Short测试标准数组，测试之前应该把每一个测试pin的开路标准初始化好，单位uV
/// @param PinNum：管脚数，这个决定HighLimit、LowLimit数组大小
/// @param PowerCurrent：电源pin电流，单位uA
/// @param GpioCurrent：GPIOpin电流，单位uA
///
/// @retval DT_ERROR_OK：OS测试参数配置成功
/// @retval DT_ERROR_FAILD：OS测试参数配置失败
/// @retval DT_ERROR_COMM_ERROR：通讯错误
DTCCM_API int _DTCALL_ OS_Config(int Voltage, int HighLimit[], int LowLimit[], int PinNum, int PowerCurrent, int GpioCurrent, int iDevID=DEFAULT_DEV_ID);

/// @brief OS测试结果读取。
///
/// @param VoltageH：正向pos测试结果，单位uV
/// @param VoltageH：反向pos测试结果，单位uV
/// @param Result：开短路测试结果，参见宏定义“OS测试结果定义”
/// @param PosEn：正向测试使能 
/// @param NegEn：反向测试使能
/// @param PinNum：管脚数，这个决定VoltageH、VoltageL，Result数组大小
/// 
/// @retval DT_ERROR_OK：OS测试结果读取成功
/// @retval DT_ERROR_FAILD：OS测试结果读取失败
/// @retval DT_ERROR_COMM_ERROR：通讯错误
DTCCM_API int _DTCALL_ OS_Read(int VoltageH[], int VoltageL[], UCHAR Result[], BOOL PosEn, BOOL NegEn, int PinNum, int iDevID=DEFAULT_DEV_ID);

/// @brief LC测试参数配置。
///
/// @param Voltage：LC测试电压，单位mV
/// @param Range：LC精度设置，Range为0精度是1uA，Range为1精度是10nA
///
/// @retval DT_ERROR_OK：LC参数参数配置成功
/// @retval DT_ERROR_FAILD：LC测试参数配置失败
/// @retval DT_ERROR_COMM_ERROR：通讯错误
DTCCM_API int _DTCALL_ LC_Config(USHORT Voltage, UCHAR Range, int iDevID=DEFAULT_DEV_ID);

/// @brief LC测试结果读取。
///
/// @param CurrentH：正向测试结果单位nA
/// @param CurrentL：反向测试结果单位nA
/// @param PinNum：管脚数，这个决定CurrentH、CurrentL，数组大小
///
/// @retval DT_ERROR_OK：LC测试结果读取成功
/// @retval DT_ERROR_FAILD：LC测试结果读取失败
/// @retval DT_ERROR_COMM_ERROR：通讯错误
DTCCM_API int _DTCALL_ LC_Read(ULONG CurrentH[],ULONG CurrentL[],int PinNum,int iDevID=DEFAULT_DEV_ID);

/// @brief 开关状态值读取。
///
/// @param Result：返回开关状态值
/// @param PinNum：管脚数，这个决定CurrentH、CurrentL，数组大小
///
/// @retval DT_ERROR_OK：LC开关状态值读取成功
/// @retval DT_ERROR_FAILD：LC开关状态值读取失败
/// @retval DT_ERROR_COMM_ERROR：通讯错误
DTCCM_API int _DTCALL_ LC_ReadSwitch(UCHAR Result[],int PinNum,int iDevID=DEFAULT_DEV_ID);
/** @} */ // end of group8

/** @defgroup group9 扩展IO


* @{

*/
/************************************************************************
 扩展IO                                                                     
************************************************************************/
/// @brief 设置外部扩展的3.3V电压输出。
///
/// @param bEnable：扩展电压输出使能
///
/// @retval DT_ERROR_OK：设置外部扩展的3.3V电压输出成功
/// @retval DT_ERROR_FAILD：设置外部扩展的3.3V电压输出失败
/// @retval DT_ERROR_COMM_ERROR：通讯错误
DTCCM_API int _DTCALL_ SetExtPowerEnable(BOOL bEnable,int iDevID=DEFAULT_DEV_ID);

/// @brief 设置外部扩展的时钟输出。
///
/// @param bOnOff：使能时钟输出
/// @param uHundKhz：设置时钟输出值,单位Khz
///
/// @retval DT_ERROR_OK：设置外部扩展时钟输出成功
/// @retval DT_ERROR_FAILD：设置外部扩展时钟输出失败
/// @retval DT_ERROR_COMM_ERROR：通讯错误
DTCCM_API int _DTCALL_ SetExtClock(BOOL bOnOff,USHORT uHundKhz,int iDevID=DEFAULT_DEV_ID);

/// @brief 设置外部扩展GPIO模式。
///
/// @param Pin[]：GPIO编号，参见宏定义“EXT_GPIO”
/// @param Mode[]：模式设置，参见枚举类型“EXT_GPIO_MODE”
/// @param iCount：设置IO个数
///
/// @retval DT_ERROR_OK：设置外部扩展GPIO模式成功
/// @retval DT_ERROR_FAILD：设置外部扩展GPIO模式失败
/// @retval DT_ERROR_COMM_ERROR：通讯错误
DTCCM_API int _DTCALL_ SetExtIoMode(EXT_GPIO Pin[],EXT_GPIO_MODE Mode[],int iCount,int iDevID=DEFAULT_DEV_ID);

/// @brief 恒定电平输出模式时设置外部扩展GPIO电平。
///
/// @param Pin[]：GPIO编号，参见宏定义“EXT_GPIO”
/// @param Level[]：GPIO的电平值，FALSE为低电平，TRUE为高电平
/// @param iCount：设置IO个数
///
/// @retval DT_ERROR_OK：设置外部扩展GPIO电平成功
/// @retval DT_ERROR_FAILD：设置外部扩展GPIO电平失败
/// @retval DT_ERROR_COMM_ERROR：通讯错误
DTCCM_API int _DTCALL_ SetExtIoLevel(EXT_GPIO Pin[],BOOL Level[],int iCount,int iDevID=DEFAULT_DEV_ID);

/// @brief 获取外部扩展GPIO的电平。
/// 
/// @param Pin[]：GPIO编号，参见宏定义“EXT_GPIO”
/// @param Level[]：GPIO的电平值，FALSE为低电平，TRUE为高电平
/// @param iCount：设置IO个数
///
/// @retval DT_ERROR_OK：获取外部扩展GPIO电平成功
/// @retval DT_ERROR_FAILD：获取外部扩展GPIO电平失败
/// @retval DT_ERROR_COMM_ERROR：通讯错误
DTCCM_API int _DTCALL_ GetExtIoLevel(EXT_GPIO Pin[],BOOL Level[],int iCount,int iDevID=DEFAULT_DEV_ID);

/// @brief 扩展GPIO为高低电平交互输出时，高、低电平时间设置。
/// 
/// @param Pin[]：GPIO编号，参见宏定义“EXT_GPIO”
/// @param HighLevelTime[]：对应扩展GPIO高电平时间，单位us
/// @param LowLevelTime[]：对应扩展GPIO低电平时间，单位us
/// @param iCount：设置IO个数
///
/// @retval DT_ERROR_OK：设置外部扩展GPIO高低电平时间成功
/// @DT_ERROR_FAILD设置外部扩展GPIO高低电平时间失败
/// @retval DT_ERROR_COMM_ERROR：通讯错误
DTCCM_API int _DTCALL_ SetExtIoLevelTime(EXT_GPIO Pin[],int HighLevelTime[],int LowLevelTime[],int iCount,int iDevID=DEFAULT_DEV_ID);

/// @brief 外部扩展I2C写
///
/// @param uDevAddr：设备地址
/// @param uRegAddr：设备寄存器
/// @param uRegAddrSize：寄存器地址的字节数
/// @param pData：写入寄存器的数据块
/// @param uSize：写入寄存器的数据块大小,最大为60个字节
/// 
/// @retval DT_ERROR_OK：写成功
/// @retval DT_ERROR_FAILD：写失败
/// @retval DT_ERROR_COMM_ERROR：通讯错误
DTCCM_API int _DTCALL_ ExtI2cWrite(UCHAR uDevAddr,USHORT uRegAddr,UCHAR uRegAddrSize,BYTE *pData,USHORT uSize,int iDevID=DEFAULT_DEV_ID);

/// @brief 外部扩展I2C读
///
/// @param uDevAddr：设备地址
/// @param uRegAddr：设备寄存器
/// @param uRegAddrSize：寄存器地址的字节数
/// @param pData：读寄存器的数据块
/// @param uSize：读寄存器的数据块大小,最大为60个字节
/// @param bNoStop：是否发出I2C的STOP命令，一般情况下默认为FALSE，bNoStop=TRUE表示写的过程不会有I2C的停止命令，bNoStop=FALSE有I2C的停止命令
/// 
/// @retval DT_ERROR_OK：读成功
/// @retval DT_ERROR_FAILD：读失败
/// @retval DT_ERROR_COMM_ERROR：通讯错误
DTCCM_API int _DTCALL_ ExtI2cRead(UCHAR uDevAddr,USHORT uRegAddr,UCHAR uRegAddrSize,BYTE *pData,USHORT uSize,BOOL bNoStop=FALSE,int iDevID=DEFAULT_DEV_ID);

/// @brief 设置外部触发输出接口电平
/// 
/// @param iTriggerOutPin：触发输出接口编号（当前美鸥力产品提供一个触发输出IO）
/// @param bTriggerOutFollowEnable：触发输出接口跟随使能信号，若为1则跟随TriggerIn信号电平，为0则为设置电平值
/// @param bLevel：电平值（0为输出低电平，1为输出高电平）
///
/// @retval DT_ERROR_OK：读成功
/// @retval DT_ERROR_FAILD：读失败
/// @retval DT_ERROR_COMM_ERROR：通讯错误
DTCCM_API int _DTCALL_ SetTriggerOutLevel(int iTriggerOutPin,BOOL bFollowTrigIn,BOOL bLevel,int iDevID=DEFAULT_DEV_ID);

/// @brief 获取外部触发输入接口电平
/// 
/// @param iTriggerInPin：触发输入接口编号（当前美鸥力产品提供一个触发输出IO）
/// @param pLevel：返回的电平值（0为输出低电平，1为输出高电平）
///
/// @retval DT_ERROR_OK：读成功
/// @retval DT_ERROR_FAILD：读失败
/// @retval DT_ERROR_COMM_ERROR：通讯错误
DTCCM_API int _DTCALL_ GetTriggerInLevel(int iTriggerInPin,BOOL *pLevel,int iDevID=DEFAULT_DEV_ID);

/** @} */ // end of group9

/*************************************************************************
温度传感器相关
*************************************************************************/

// 设置温度传感器上下限温度
DTCCM_API int _DTCALL_ SetTempRange(int iUpperLimit, int iLowerLimit, BOOL bEnable,int iDevID=DEFAULT_DEV_ID);

// 读取当前温度值
DTCCM_API int _DTCALL_ GetCurrentTemp(int *pTemp,int iDevID=DEFAULT_DEV_ID);

// 读取当前设置的温度范围
DTCCM_API int _DTCALL_ GetTempRange(int *pUpperLimit,int *pLowLimit,int iDevID=DEFAULT_DEV_ID);

/*************************************************************************
状态框
*************************************************************************/
// 状态信息界面
DTCCM_API int _DTCALL_ ShowInternalStatusDialog(HWND hwnd,int iDevID=DEFAULT_DEV_ID);

DTCCM_API int _DTCALL_ ShowInternalStatusDialogEx(HWND hwnd ,HWND *pRetHwnd,int iDevID=DEFAULT_DEV_ID);

DTCCM_API int _DTCALL_ ShowMipiControllerDialog(HWND hwnd ,HWND *pRetHwnd,int iDevID=DEFAULT_DEV_ID);

DTCCM_API int _DTCALL_ ShowFrameBufferControlDialog(HWND hwnd ,HWND *pRetHwnd,int iDevID=DEFAULT_DEV_ID);

//DTCCM_API int _DTCALL_ SaveParameter(int iDevID=DEFAULT_DEV_ID);

//DTCCM_API int _DTCALL_ LoadParameter(int iDevID=DEFAULT_DEV_ID);
/****************************************************************************************
其他
****************************************************************************************/
// 扩展控制接口
// dwCtrl:	控制码，可能由派生类定义
// dwParam:	控制码相关参数
// pIn:		读取的数据
// pInSize:	读取数据的大小，字节数，调用者指定要求读取的字节数，如果成功，返回实际读取的字节数
// pOut:	写入的数据
// pOutSize:写入数据的大小，字节数，调用者指定要求写入的字节数，如果成功，返回实际写入的字节数
DTCCM_API int _DTCALL_ ExCtrl(DWORD dwCtrl,DWORD dwParam,PVOID pIn,int *pInSize,PVOID pOut,int *pOutSize,int iDevID=DEFAULT_DEV_ID);


#endif