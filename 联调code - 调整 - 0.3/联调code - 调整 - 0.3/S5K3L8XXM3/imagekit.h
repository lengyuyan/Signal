#ifndef __IMAGEKIT_H__
#define __IMAGEKIT_H__


/**************************************************************************************** 
*
* image kit系列各种硬件版本定义
*
****************************************************************************************/

#define VER_HS100   0x1000
#define VER_HS200   0x1020

#define VER_HS120   0x1020
#define VER_HS128	0x1200
#define VER_HS230   0x1230

#define VER_HS280	0x1280

#define VER_HS130	0x1300
#define VER_HS300   0x1330
#define VER_HS320   0x1332

#define VER_HV810   0x1380
#define VER_HV820   0x1382
#define VER_HV910   0x1390
#define VER_HV920   0x1392

/**************************************************************************************** 
*
* SENSOR的初始化与控制相关
*
****************************************************************************************/

typedef struct _SensorTab
{
	/// @brief SENSOR宽度
	USHORT width;          ///<SENSOR宽度
	/// @brief SENSOR高度
	USHORT height;         ///<SENSOR高度
	/// @brief SENSOR数据类型
	BYTE type;             ///<SENSOR数据类型
	/// @brief SENSOR的RESET和PWDN引脚设置
	BYTE pin;              ///<SENSOR的RESET和PWDN引脚设置
	/// @brief SENSOR的器件地址
	BYTE SlaveID;          ///<SENSOR的器件地址
	/// @brief SENSOR的I2C模式
	BYTE mode;						 ///<SENSOR的I2C模式
	/// @brief SENSOR标志寄存器1.
	USHORT FlagReg;				 ///<SENSOR标志寄存器1.
	/// @brief SENSOR标志寄存器1的值
	USHORT FlagData;			 ///<SENSOR标志寄存器1的值
	/// @brief SENSOR标志寄存器1的掩码值
	USHORT FlagMask;			 ///<SENSOR标志寄存器1的掩码值
	/// @brief SENSOR标志寄存器2.
	USHORT FlagReg1;			 ///<SENSOR标志寄存器2.
	/// @brief SENSOR标志寄存器2的值
	USHORT FlagData1;			 ///<SENSOR标志寄存器2的值
	/// @brief SENSOR标志寄存器2的掩码值
	USHORT FlagMask1;			 ///<SENSOR标志寄存器2的掩码值
	/// @brief SENSOR的名称
	char name[64];				///<SENSOR的名称

	/// @brief 初始化SENSOR数据表
	USHORT* ParaList;			///<初始化SENSOR数据表
	/// @brief 初始化SENSOR数据表大小，单位字节
	USHORT  ParaListSize; ///<初始化SENSOR数据表大小，单位字节

	/// @brief SENSOR进入Sleep模式的参数表
	USHORT* SleepParaList;	///<SENSOR进入Sleep模式的参数表
	/// @brief SENSOR进入Sleep模式的参数表大小，单位字节
	USHORT  SleepParaListSize;///<SENSOR进入Sleep模式的参数表大小，单位字节

	/// @brief SENSOR输出数据格式，YUV//0:YCbYCr;	//1:YCrYCb;	//2:CbYCrY;	//3:CrYCbY.
	BYTE outformat;         ///<SENSOR输出数据格式，YUV//0:YCbYCr;	//1:YCrYCb;	//2:CbYCrY;	//3:CrYCbY.
	/// @brief SENSOR的输入时钟MCLK，0:12M; 1:24M; 2:48M.
	int mclk;               ///<SENSOR的输入时钟MCLK，0:12M; 1:24M; 2:48M.
	/// @brief SENSOR的AVDD电压值
	BYTE avdd;              ///<SENSOR的AVDD电压值
	/// @brief SENSOR的DOVDD电压值
	BYTE dovdd;             ///<SENSOR的DOVDD电压值
	/// @brief SENSOR的DVDD电压值		
	BYTE dvdd;							///<SENSOR的DVDD电压值

	/// @brief SENSOR的数据接口类型
	BYTE port; 							///<SENSOR的数据接口类型
	USHORT Ext0;
	USHORT Ext1;
	USHORT Ext2; 

	/// @brief AF初始化参数表
	USHORT* AF_InitParaList;        ///<AF初始化参数表
	/// @brief AF初始化参数表大小，单位字节
	USHORT  AF_InitParaListSize;		///<AF初始化参数表大小，单位字节

	/// @brief AF_AUTO参数表
	USHORT* AF_AutoParaList;				///<AF_AUTO参数表
	/// @brief AF_AUTO参数表大小，单位字节
	USHORT  AF_AutoParaListSize;		///<AF_AUTO参数表大小，单位字节

	/// @brief AF_FAR参数表
	USHORT* AF_FarParaList;					///<AF_FAR参数表
	/// @brief AF_FAR参数表大小，单位字节
	USHORT  AF_FarParaListSize;			///<AF_FAR参数表大小，单位字节

	/// @brief AF_NEAR参数表
	USHORT* AF_NearParaList;				///<AF_NEAR参数表
	/// @brief AF_NEAR参数表大小，单位字节
	USHORT  AF_NearParaListSize;		///<AF_NEAR参数表大小，单位字节

	/// @brief 曝光参数表
	USHORT* Exposure_ParaList;      ///<曝光参数表
	/// @brief 曝光参数表大小，单位字节
	USHORT  Exposure_ParaListSize;	///<曝光参数表大小，单位字节

	/// @brief 增益参数表
	USHORT* Gain_ParaList;          ///<增益参数表
	/// @brief 增益参数表大小，单位字节
	USHORT	Gain_ParaListSize;			///<增益参数表大小，单位字节 

	_SensorTab()
	{
		width=0;
		height=0;
		type=0;
		pin=0;
		SlaveID=0;
		mode=0;
		FlagReg=0;
		FlagData=0;
		FlagMask=0;
		FlagReg1=0;
		FlagData1=0;
		FlagMask1=0;
		memset(name,0,sizeof(name));

		ParaList=NULL;
		ParaListSize=0;
		SleepParaList=NULL;
		SleepParaListSize=0;

		outformat=0;
		mclk=0;               //0:12M; 1:24M; 2:48M.
		avdd=0;               // 
		dovdd=0;              //
		dvdd=0;

		port=0; 	
		Ext0=0;
		Ext1=0;
		Ext2=0; 

		AF_InitParaList=NULL;        //AF_InitParaList
		AF_InitParaListSize=0;

		AF_AutoParaList=NULL;
		AF_AutoParaListSize=0;

		AF_FarParaList=NULL;
		AF_FarParaListSize=0;

		AF_NearParaList=NULL;
		AF_NearParaListSize=0;

		Exposure_ParaList=NULL;      //曝光
		Exposure_ParaListSize=0;

		Gain_ParaList=NULL;          //增益
		Gain_ParaListSize=0;
	}
}SensorTab, *pSensorTab;

//紧随SensorTab, 增加SensorTab2定义
///////////////////////////////////////////////////////////
typedef struct _SensorTab2
{
	/// @brief SENSOR宽度
	UINT width;          ///<SENSOR宽度
	/// @brief SENSOR高度
	UINT height;         ///<SENSOR高度

	UINT Quick_w;		///<Quick View 宽度	
	UINT Quick_h;		///<Quick View 高度

	/// @brief SENSOR数据类型
	UINT type;             ///<SENSOR数据类型
	/// @brief SENSOR的RESET和PWDN引脚设置
	UINT pin;              ///<SENSOR的RESET和PWDN引脚设置
	/// @brief SENSOR的器件地址
	UINT SlaveID;          ///<SENSOR的器件地址
	/// @brief SENSOR的I2C模式
	UINT mode;						 ///<SENSOR的I2C模式
	/// @brief SENSOR标志寄存器1.
	UINT FlagReg;				 ///<SENSOR标志寄存器1.
	/// @brief SENSOR标志寄存器1的值
	UINT FlagData;			 ///<SENSOR标志寄存器1的值
	/// @brief SENSOR标志寄存器1的掩码值
	UINT FlagMask;			 ///<SENSOR标志寄存器1的掩码值
	/// @brief SENSOR标志寄存器2.
	UINT FlagReg1;			 ///<SENSOR标志寄存器2.
	/// @brief SENSOR标志寄存器2的值
	UINT FlagData1;			 ///<SENSOR标志寄存器2的值
	/// @brief SENSOR标志寄存器2的掩码值
	UINT FlagMask1;			 ///<SENSOR标志寄存器2的掩码值
	/// @brief SENSOR的名称
	char name[64];				///<SENSOR的名称

	/// @brief 初始化SENSOR数据表
	UINT* ParaList;			///<初始化SENSOR数据表
	/// @brief 初始化SENSOR数据表大小，单位字节
	UINT  ParaListSize; ///<初始化SENSOR数据表大小，单位字节

	/// @brief SENSOR进入Sleep模式的参数表
	UINT* SleepParaList;	///<SENSOR进入Sleep模式的参数表
	/// @brief SENSOR进入Sleep模式的参数表大小，单位字节
	UINT  SleepParaListSize;///<SENSOR进入Sleep模式的参数表大小，单位字节

	/// @brief SENSOR进入Quick View模式的参数表
	UINT* QuickParaList;	///<SENSOR进入Quick View模式的参数表
	/// @brief SENSOR进入Quick View模式的参数表大小，单位字节
	UINT  QuickParaListSize;///<SENSOR进入Quick View模式的参数表大小，单位字节

	/// @brief SENSOR输出数据格式，YUV//0:YCbYCr;	//1:YCrYCb;	//2:CbYCrY;	//3:CrYCbY.
	UINT outformat;         ///<SENSOR输出数据格式，YUV//0:YCbYCr;	//1:YCrYCb;	//2:CbYCrY;	//3:CrYCbY.
	/// @brief SENSOR的输入时钟MCLK，0:12M; 1:24M; 2:48M.
	UINT mclk;               ///<SENSOR的输入时钟MCLK，0:12M; 1:24M; 2:48M.
	/// @brief SENSOR的AVDD电压值
	UINT avdd;              ///<SENSOR的AVDD电压值
	/// @brief SENSOR的DOVDD电压值
	UINT dovdd;             ///<SENSOR的DOVDD电压值
	/// @brief SENSOR的DVDD电压值		
	UINT dvdd;							///<SENSOR的DVDD电压值

	/// @brief SENSOR的数据接口类型
	UINT port; 							///<SENSOR的数据接口类型
	UINT Ext0;
	UINT Ext1;
	UINT Ext2; 

	/// @brief AF初始化参数表
	UINT* AF_InitParaList;        ///<AF初始化参数表
	/// @brief AF初始化参数表大小，单位字节
	UINT  AF_InitParaListSize;		///<AF初始化参数表大小，单位字节

	/// @brief AF_AUTO参数表
	UINT* AF_AutoParaList;				///<AF_AUTO参数表
	/// @brief AF_AUTO参数表大小，单位字节
	UINT  AF_AutoParaListSize;		///<AF_AUTO参数表大小，单位字节

	/// @brief AF_FAR参数表
	UINT* AF_FarParaList;					///<AF_FAR参数表
	/// @brief AF_FAR参数表大小，单位字节
	UINT  AF_FarParaListSize;			///<AF_FAR参数表大小，单位字节

	/// @brief AF_NEAR参数表
	UINT* AF_NearParaList;				///<AF_NEAR参数表
	/// @brief AF_NEAR参数表大小，单位字节
	UINT  AF_NearParaListSize;		///<AF_NEAR参数表大小，单位字节

	/// @brief 曝光参数表
	UINT* Exposure_ParaList;      ///<曝光参数表
	/// @brief 曝光参数表大小，单位字节
	UINT  Exposure_ParaListSize;	///<曝光参数表大小，单位字节

	/// @brief 增益参数表
	UINT* Gain_ParaList;          ///<增益参数表
	/// @brief 增益参数表大小，单位字节
	UINT	Gain_ParaListSize;			///<增益参数表大小，单位字节 

	_SensorTab2()
	{
		width=0;
		height=0;
		Quick_w = 0; //20141031
		Quick_h = 0; //20141031
		type=0;
		pin=0;
		SlaveID=0;
		mode=0;
		FlagReg=0;
		FlagData=0;
		FlagMask=0;
		FlagReg1=0;
		FlagData1=0;
		FlagMask1=0;
		memset(name,0,sizeof(name));

		ParaList=NULL;
		ParaListSize=0;
		SleepParaList=NULL;
		SleepParaListSize=0;

		QuickParaList = NULL; //20141031
		QuickParaListSize = 0; //20141031

		outformat = 0;
		mclk= 0;               //0:12M; 1:24M; 2:48M.
		avdd= 0;               // 
		dovdd = 0;              //
		dvdd = 0;

		port=0; 	
		Ext0=0;
		Ext1=0;
		Ext2=0; 

		AF_InitParaList=NULL;        //AF_InitParaList
		AF_InitParaListSize=0;

		AF_AutoParaList=NULL;
		AF_AutoParaListSize=0;

		AF_FarParaList=NULL;
		AF_FarParaListSize=0;

		AF_NearParaList=NULL;
		AF_NearParaListSize=0;

		Exposure_ParaList=NULL;      //曝光
		Exposure_ParaListSize=0;

		Gain_ParaList=NULL;          //增益
		Gain_ParaListSize=0;
	}
}SensorTab2, *pSensorTab2;
/** @defgroup group3 SENSOR相关
@{

*/
/** @name SENSOR输出图像类型定义(SensorTab::type的取值定义)
@{

*/
/* SENSOR输出图像类型定义(SensorTab::type的取值定义) */
#define D_RAW10				0x00 
#define D_RAW8				0x01
#define D_MIPI_RAW8			0x01
#define D_YUV				0x02 
#define D_RAW16				0x03
#define D_MIPI_RAW16		0x03
#define D_RGB565			0x04
#define D_YUV_SPI			0x05
#define D_MIPI_RAW10		0x06    // 5bytes = 4 pixel...
#define D_MIPI_RAW12		0x07    // 3bytes = 2 pixel...
#define D_RAW12				0x07
#define D_YUV_MTK_S			0x08    //MTK output...
//#define D_HISPI_SP			0x09    //aptina hispi packet sp.
/** @} */

/** @name SENSOR寄存器参数表中附带的控制字定义
@{

*/
/* SENSOR寄存器参数表中附带的控制字定义 */
#define DTDELAY				0xffff
#define DTMODE				0xfffe	
#define DTOR				0xfffd
#define DTAND				0xfffc
#define DTPOLLT				0xfffb
#define DTPOLL1				0xfffa
#define DTPOLL0				0xfff9
#define DTI2CADDR			0xfff8
#define DTI2CREG			0xfff7
#define DTAFTYPE			0xfff6  //20121223 added... modify the AF Device type
#define DTAFADDR			0xfff5  //20121223 added... modify the AF Device Address..	
#define DTSPIMTKCTRL		0xfff4
#define DTEND				0xfeff	
#define DTMACRO_ON			0xfef0
#define DTMACRO_OFF			0xfef1 
#define DTSPIMTKCTRL		0xfff4

// XHS
// XVS
// V_START
// V_END
// H_START
// H_END
/** @} */

/** @name SENSOR电源电压选择定义
@{

*/
/* SENSOR电源电压选择定义 */
#define AVDD_28				0x00
#define AVDD_25				0x01
#define AVDD_18				0x02
#define AVDD_DEFAULT		0x03

#define DOVDD_28			0x00
#define DOVDD_25			0x01
#define DOVDD_18			0x02
#define DOVDD_DEFAULT		0x03

#define DVDD_18				0x00
#define DVDD_15				0x01
#define DVDD_12				0x02
#define DVDD_DEFAULT		0x03

#define AFVCC_33			0x00
#define AFVCC_28			0x01
#define AFVCC_18			0x02
#define AFVCC_DEFAULT		0x03
/** @} */

/** @name SENSOR输入时钟选择定义
@{

*/
/* SENSOR输入时钟选择定义 */
//you can use these enum type ,or use MHZ or hundred KHZ directly 
enum MCLKOUT
{
	MCLK_6M = 0,
	MCLK_8M,
	MCLK_10M,
	MCLK_11M4,
	MCLK_12M,
	MCLK_12M5,
	MCLK_13M5,
	MCLK_15M,
	MCLK_18M,
	MCLK_24M,
	MCLK_25M,
	MCLK_27M,
	MCLK_30M,
	MCLK_32M,
	MCLK_36M,
	MCLK_40M,
	MCLK_45M,
	MCLK_48M,
	MCLK_50M,
	MCLK_60M,
	MCLK_DEFAULT,
};
/** @} */

/** @name 多SENSOR模组通道定义(目前只在DTLC2中存在CHANNEL_B)
@{

*/
/* 多SENSOR模组通道定义(目前只在DTLC2中存在CHANNEL_B) **/
#define CHANNEL_A					0x01 /// 只使用A通道
#define CHANNEL_B					0x02 // 只是用B通道
#define CHANNEL_AB					0x03 // AB通道同时使用
/** @} */ 

/** @name SensorEnable函数中，使能SENSOR时，RESET/PWDN管脚的电平状态定义
@{

*/
/* SensorEnable函数中，使能SENSOR时，RESET/PWDN管脚的电平状态定义 */
#define RESET_H						0x02
#define RESET_L						0x00
#define PWDN_H						0x01
#define PWDN_L						0x00
#define PWDN2_H						0x04
#define PWDN2_L						0x00
/** @} */

/** @name 支持的SENSOR数据接口定义
@{

*/
/// 定义支持的SENSOR数据接口。
typedef enum
{
	SENSOR_PORT_MIPI = 0,		///<MIPI接口
	SENSOR_PORT_PARA,				///<并行同步接口
	SENSOR_PORT_MTK_SERIAL,	///<MTK公司的串行接口
	SENSOR_PORT_SPI,				///<SPI接口
	SENSOR_PORT_SIM,				///<模拟图像，用于测试
	SENSOR_PORT_HISPI,			///<Aptina的HISPI接口,支持packet sp格式
	SENSOR_PORT_ZX_SERIAL,		///<展讯的串行接口

	SENSOR_PORT_SONY_LVDS = 0x81,
}SENSOR_PORT;
/** @} */

/** @name 早期版本使用的宏定义
@{

*/
/* 早期版本使用的宏定义 */
#define PORT_MIPI			0   //MIPI output
#define PORT_PARALLEL		1   //Parallel output
#define PORT_MTK			2   //MTK output
#define PORT_SPI			3   //SPI output
#define PORT_TEST			4   //TEST ouput. FPGA output the image...
#define PORT_HISPI			5   //aptina HISPI packet sp...
#define PORT_ZX2_4			6   //zhanxun 2bit/4bit packet sp...
#define PORT_MAX			7   //maxium... can't support >=PORT_MAX
/** @} */

#define PORT_SONY_LVDS		0x81 
#define PORT_PANASONIC		0x85  
/** @name 定义柔性接口中的各种管脚功能
@{

*/
/* 定义柔性接口中的各种管脚功能 */
typedef enum
{
	PIN_D0 = 0,
	PIN_D1,
	PIN_D2,
	PIN_D3,
	PIN_D4,
	PIN_D5,
	PIN_D6,
	PIN_D7,
	PIN_D8,
	PIN_D9,
	PIN_PCLK,
	PIN_HSYNC,
	PIN_VSYNC,
	PIN_MCLK,
	PIN_RESET,
	PIN_PWDN,
	PIN_PWDN2,
	PIN_GPIO1,
	PIN_SDA,
	PIN_SCL,
	PIN_NC,
	PIN_GPIO2,
	PIN_GPIO3,
	PIN_GPIO4,
	PIN_NC1,
	PIN_NC2,
	PIN_D10,
	PIN_D11,
	PIN_SPI_SCK,	//柔性接口 新增SPI接口
	PIN_SPI_CS,
	PIN_SPI_SDI,
	PIN_SPI_SDO,
	PIN_SPI_SDA,
	PIN_CLK_ADJ_200K,
	PIN_CLK_ADJ_18M,
	PIN_GPIO5,
	PIN_GPIO6,
	PIN_GPIO7,
	PIN_GPIO8
}PIN_FUNC;
/** @} */

/** @name 定义柔性接口管脚名称(编号)
@{

*/
/* 定义柔性接口管脚名称(编号) */
typedef enum
{
	PIN_IO1 = 0,
	PIN_IO2,
	PIN_IO3,
	PIN_IO4,
	PIN_IO5,
	PIN_IO6,
	PIN_IO7,
	PIN_IO8,
	PIN_IO9,
	PIN_IO10,
	PIN_IO11,
	PIN_IO12,
	PIN_IO13,
	PIN_IO14,
	PIN_IO15,
	PIN_IO16,
	PIN_IO17,
	PIN_IO18,
	PIN_IO19,
	PIN_IO20,
	PIN_IO21,
	PIN_IO22,
	PIN_IO23,
	PIN_IO24,
	PIN_IO25,
	PIN_IO26,
}SOFT_PIN;
/** @} */

/** @name MIPI控制器特性的位定义
@{

*/
/* MIPI控制器特性的位定义 */
#define MIPI_CTRL_LP_EN				1		// 允许进入LP状态
#define MIPI_CTRL_AUTO_START		(1<<1)	// 出现差分信号后自动启动，用于OS测试
#define MIPI_CTRL_NON_CONT			(1<<2)	// 使用非连续时钟
#define MIPI_CTRL_FULL_CAP			(1<<3)	// 完整数据包获取，包括包头和CRC16校验，
#define MIPI_CTRL_CLK_LP_CHK		(1<<4)	// 对CLK Lane的LP状态进行检测，强制要求MIPI TX端的Clk Lane必须进入一次LP状态/** @} */
// 将导致每行图像数据增加6字节

/** @name 同步并行接口特性的位定义
@{

*/
/* 同步并行接口特性的位定义 */
#define PARA_PCLK_RVS				(1<<3)	// PCLK取反
#define PARA_VSYNC_RVS				(1<<4)	// VSYNC取反
#define PARA_HSYNC_RVS				(1<<5)	// HSYNC取反
#define PARA_AUTO_POL				(1<<6)	// VSYNC,HSYNC极性自动识别
/** @} */

/** @name 同步并行接口,3bit用于选择位宽
@{

*/
/* 3bit用于选择位宽 */
#define PARA_BW_8BIT				0		
#define PARA_BW_10BIT				1
#define PARA_BW_12BIT				2
#define PARA_BW_16BIT				3
/** @} */

/** @name HiSPI接口特性的位定义
@{

*/
/* HiSPI接口特性的位定义 */
// 2bit用于选择位宽
#define	HISPI_WW_10BIT				0
#define	HISPI_WW_12BIT				1
#define	HISPI_WW_14BIT				2
#define	HISPI_WW_16BIT				3
/** @} */

/** @name 模拟图像模块特性的位定义
@{

*/
/* 模拟图像模块特性的位定义 */
// 2bit用于选择模拟图像的样式
#define	SIM_STYLE1					0	// 输出固定颜色
#define	SIM_STYLE2					1	// 水平渐变
#define	SIM_STYLE3					2	// 垂直渐变
#define	SIM_STYLE4					3	// 每帧渐变
/** @} */

/** @} */ // end of group3

/** @defgroup group2 ISP相关
@{

*/
/** @name RAW转RGB算法定义
@{

*/
/* RAW转RGB算法定义 */
#define RAW2RGB_NORMAL			0
#define RAW2RGB_SMOOTH			1
#define RAW2RGB_SHARP			2
/** @} */

/** @name YUV图像4种输出格式定义
@{

*/
/* RAW、YUV图像4种输出格式定义(SensorTab::outformat的取值定义) */
/// YUV图像4种输出格式定义。
enum OUTFORMAT_YUV
{
	OUTFORMAT_YCbYCr = 0,///<YCbYCr输出格式
	OUTFORMAT_YCrYCb,///<YCrYCb输出格式
	OUTFORMAT_CbYCrY,///<CbYCrY输出格式
	OUTFORMAT_CrYCbY,///<CrYCbY输出格式
};
/** @} */

/** @name RAW图像4种输出格式定义
@{

*/
/// RAW图像4种输出格式定义。
enum OUTFORMAT_RGB
{
	OUTFORMAT_RGGB = 0,///<RGGB输出格式
	OUTFORMAT_GRBG,///<GRBG输出格式
	OUTFORMAT_GBRG,///<GBRG输出格式
	OUTFORMAT_BGGR,///<BGGR输出格式
};
/** @} */

/* 本系统支持的RAW格式、YUV格式定义 */
/** @name 支持的RAW格式定义
@{

*/
/// 支持的RAW格式。
enum RAW_FORMAT
{
	RAW_RGGB = 0,	///<RAW格式按RGGB排列
	RAW_GRBG,			///<RAW格式按GRBG排列
	RAW_GBRG,			///<RAW格式按GBRG排列
	RAW_BGGR,			///<RAW格式按BGGR排列
};
/** @} */

/** @name 支持的YUV格式定义
@{

*/
/// 支持的YUV格式。
enum YUV_FORMAT
{
	YUV_YCBYCR = 0,///<YUV格式按YCBYCR排列
	YUV_YCRYCB,		 ///<YUV格式按YCRYCB排列
	YUV_CBYCRY,		 ///<YUV格式按CBYCRY排列
	YUV_CRYCBY,		 ///<YUV格式按CRYCBY排列
};
/** @} */

/** @} */ // end of group2
/**************************************************************************************** 
*
* 图像采集相关 
*
****************************************************************************************/

/* FrameBuffer模式 */

#define	BUF_MODE_NORMAL		0			// 一般模式，缓存效果相当于FIFO；当缓存量超过；
										// 缓存上限设置时，新的帧将不会被写入到缓存；

#define BUF_MODE_SKIP		1			// 跳帧模式，缓存中的帧将不会出现“排队”现象；

#define	BUF_MODE_NEWEST		2			// NEWEST模式，目前只对PCI-E接口的机型有效；
										// GrabFrame将获取最新缓存到的帧；对于其他机型
										// 将等效于SKIP模


/* 用于配置FrameBuffer */
typedef struct _FrameBufferConfig
{
	ULONG	    uMode; 
	ULONG		uBufferSize;	// 设备中的帧缓存大小(字节)
	ULONG		uUpLimit;		// 缓存上限设置(字节)，缓存量超过这个上限时，新的帧将被丢弃
	ULONG		resv[16];		// 保留，填充0
}FrameBufferConfig;

/* 帧相关信息，与帧数据对应，对其进行描述 */
typedef struct sFrameInfo
{
	BYTE	byChannel;		
	USHORT	uWidth;	
	USHORT	uHeight;	
	UINT	uDataSize;	
	UINT64	uiTimeStamp;
}FrameInfo;

// 扩展的帧信息结构体
typedef struct sFrameInfoEx
{
	BYTE	byChannel;	
	BYTE    resvl[3];		// 保留3字节，填充0
	BYTE    byImgFormat;	// 图像格式，D_RAW8、D_RAW10...
	USHORT	uWidth;	
	USHORT	uHeight;	
	UINT	uDataSize;	
	UINT	uFrameTag;		// 功能升级标识
	double  uFSTimeStamp;	// 帧开始的时间戳
	double  uFETimeStamp;	// 帧结束的时间戳
	UINT	uEccErrorCnt;	// 每帧的ECC错误计数，只对MIPI接口有效
	UINT	uCrcErrorCnt;	// 每帧的CRC错误计数，只对MIPI接口有效
	UINT	resv[64];		// 保留，填充0
}FrameInfoEx;
/** @defgroup group4 图像数据采集相关
@{

*/
/** @name 预览窗口定义
@{

*/
/* 预览窗口定义 */
#define PREVIEW_ROI_B0    0x00
#define PREVIEW_ROI_B1    0x01
#define PREVIEW_ROI_B2    0x02
#define PREVIEW_ROI_B3    0x03
#define PREVIEW_ROI_B4    0x04
#define PREVIEW_ROI_GRID  0x05
#define PREVIEW_QUICK	  0x06 
#define PREVIEW_FULL      0x07
#define PREVIEW_NOTHING   0x08
/** @} */

/** @} */ // end of group4


/**************************************************************************************** 
*
* 电源管理相关
*
****************************************************************************************/
/** @defgroup group5 电源管理单元相关

@{

*/

/* 帧相关信息，与帧数据对应，对其进行描述 */
/** @name SENSOR需要的电源类型定义
@{

*/
/* 定义SENSOR需要的电源类型 */
///定义SENSOR需要的电源类型。
typedef enum
{
	/* A通道，或只有一个通道时 */
	POWER_AVDD = 0,	///<A通道AVDD
	POWER_DOVDD,		///<A通道DOVDD
	POWER_DVDD,			///<A通道DVDD
	POWER_AFVCC,		///<A通道AFVCC
	POWER_VPP,			///<A通道VPP

	/* B通道 */
	POWER_AVDD_B,		///<B通道AVDD
	POWER_DOVDD_B,	///<B通道DOVDD
	POWER_DVDD_B,		///<B通道DVDD
	POWER_AFVCC_B,	///<B通道AFVCC
	POWER_VPP_B,		///<B通道VPP
}SENSOR_POWER;
/** @} */

/** @name SENSOR系统电源类型定义
@{

*/
/* 定义系统电源类型 */
///定义系统电源类型。
typedef enum
{
	POWER_SYS_12V = 0,///<12V系统电源
	POWER_SYS_5V,			///<5V系统电源
	POWER_SYS_3_3V		///<3.3V系统电源
}SYS_POWER;
/** @} */

/** @name SENSOR电源模式定义
@{

*/
/* 定义电源模式 */
///定义电源模式。
typedef enum
{
	POWER_MODE_WORK = 0,///<SENSOR电源为工作模式
	POWER_MODE_STANDBY, ///<SENSOR电源为待机模式
	POWER_MODE_PWDN			///<SENSOR电源为掉电模式
}POWER_MODE;
/** @} */

/** @name 电流测试量程定义
@{

*/
/* 定义电流测试量程 */
///定义电流测试量程。
typedef enum
{
	CURRENT_RANGE_MA = 0,///<电流测试量程为mA
	CURRENT_RANGE_UA,		 ///<电流测试量程为uA
	CURRENT_RANGE_NA		 ///<电流测试量程为nA
}CURRENT_RANGE;
/** @} */
/** @} */ // end of group5


/**************************************************************************************** 
*
* OS/LC相关
*
****************************************************************************************/
/** @defgroup group8 LC/OS相关
@{

*/
/** @name OS/LC测试配置定义
@{

*/
/* OS/LC测试配置定义，在LC_OS_CommandConfig函数中使用 */
#define OS_CFG_CHANNEL_A					(1<<5)
#define OS_CFG_CHANNEL_B					(1<<4)	
#define OS_CFG_TEST_ENA						(1<<7)
#define LC_CFG_TEST_ENA						(1<<6)
#define OS_CFG_HIGH							(1<<3)
#define OS_CFG_LOW							(1<<2)
#define LC_CFG_HIGH							(1<<1)
#define LC_CFG_LOW							(1<<0)
/** @} */
/** @name OS/LC测试结果定义，OS_Read函数返回的结果
@{

*/
/* OS/LC测试结果定义，OS_Read函数返回的结果 */
#define	OS_TEST_RESULT_PASS					0	 // 通过测试
#define OS_TEST_RESULT_NG					1	 // 未通过测试
#define OS_TEST_RESULT_FAILED				0xfe // 测试失败
#define OS_TEST_RESULT_INVALID				0xff // 测试无效
/** @} */
/** @} */ // end of group8



/**************************************************************************************** 
*
* I2C总线相关
*
****************************************************************************************/
/** @defgroup group3 SENSOR相关
@{

*/
/*
* 常见的寄存器读写模式
* I2C mode definiton
* when read or write by I2c ,should use this definiton...
* Normal Mode:8 bit address,8 bit data,
* Samsung Mode:8 bit address,8 bit data,but has a stop between slave ID and addr...
* Micron:8 bit address,16bit data...
* Stmicro:16bit addr ,8bit data,such as eeprom and stmicro sensor...
*/
/** @name I2C模式定义
@{

*/
///I2C模式定义。
enum I2CMODE
{
	I2CMODE_NORMAL=0,		///< 8 bit addr,8 bit value 
	I2CMODE_SAMSUNG,		///< 8 bit addr,8 bit value,Stopen
	I2CMODE_MICRON,			///< 8 bit addr,16 bit value
	I2CMODE_STMICRO,		///< 16 bit addr,8 bit value, (eeprom also)
	I2CMODE_MICRON2,		///< 16 bit addr,16 bit value
};
/** @} */

/** @name SPI模式定义
@{

*/
///SPI模式定义
enum SPIMODE
{
	SPIMODE_SONY_A1_D1=0x81,	///< 8 bit addr,8 bit value 
	SPIMODE_SONY_A1_D2,			///< 8 bit addr,16 bit value
	SPIMODE_SONY_A2_D1,			///< 16 bit addr,8 bit value
	SPIMODE_SONY_A2_D2,			///< 16 bit addr,16 bit value

	/* panasonic lsb */
	SPIMODE_PANASONIC_A1_D1=0x91,		///< 8 bit addr,8 bit value 
	SPIMODE_PANASONIC_A1_D2,			///< 8 bit addr,16 bit value
	SPIMODE_PANASONIC_A2_D1,			///< 16 bit addr,8 bit value
	SPIMODE_PANASONIC_A2_D2,			///< 16 bit addr,16 bit value
	
	/* smartsens msb */
	SPIMODE_SMARTSENS_A2_D1=0xcb,		///< 16 bit addr,8 bit value  bit15-bit0
};
/** @} */

/** @} */ // end of group3




/**************************************************************************************** 
*
* 本系统支持的AF器件型号定义
*
****************************************************************************************/
/** @defgroup group7 AF相关
@{

*/
/** @name 支持的AF器件型号定义
@{

*/
#define AF_DRV_AD5820		0
#define AF_DRV_DW9710		0
#define AF_DRV_DW9714		0

#define AF_DRV_AD5823		1
#define AF_DRV_FP5512		2
#define AF_DRV_DW9718		3
#define AF_DRV_BU64241		4
#define AF_DRV_LV8498		5
#define AF_DRV_BU64291		6
#define AF_DRV_AD1457		7

#define AF_DRV_DW9761		8
#define AF_DRV_AD5816		8

#define AF_DRV_AK7345		9
#define AF_DRV_DW9800		10

#define AF_DRV_ZC533		11
#define AF_DRV_BU64295		12
#define AF_DRV_DW9719		13
//#define AF_DRV_SC9714		14

#define AF_DRV_MAX			30

/** @} */
/** @} */ // end of group7


/**************************************************************************************** 
*
* 按键功能定义
*
****************************************************************************************/
/** @defgroup group6 初始化控制相关
@{

*/
/** @name 按键功能定义
@{

*/ 
#define KEY_ROI_B0			0x100
#define KEY_ROI_B1			0x80
#define KEY_ROI_B2			0x40
#define KEY_ROI_B3			0x10
#define KEY_ROI_B4			0x20
#define KEY_ROI_GRID		0x04
#define KEY_FULL			0x08
#define KEY_PLAY			0x01
#define KEY_CAM				0x02
#define KEY_NOTHING			0x00 
/** @} */
/** @} */ // end of group6

/************************************************************************
*
*外部扩展IO定义
*
/************************************************************************/
/** @defgroup group9 扩展IO
@{

*/
/** @name 外部扩展IO管脚定义
@{

*/
typedef enum
{
	GPIO0=0,	///<GPIO0
	GPIO1,		///<GPIO1
	GPIO2,		///<GPIO2
	GPIO3,		///<GPIO3
	GPIO4,		///<GPIO4
	GPIO5,		///<GPIO5
	GPIO6,		///<GPIO6
	GPIO7,		///<GPIO7
	GPIO8,		///<GPIO8
	GPIO9,		///<GPIO9
	GPIO10,		///<GPIO10
	GPIO11,		///<GPIO11
}EXT_GPIO;
/** @} */

/** @name 外部扩展IO模式定义
@{

*/
typedef enum
{
	GPIO_INPUT=0,	///<输入模式
	GPIO_OUTPUT,	///<恒电平输出模式
	GPIO_OUTPUT_PP, ///<高低电平交互输出

}EXT_GPIO_MODE;
/** @} */
/** @} */ // end of group9
/**************************************************************************************** 
*
* 一些SDK接口函数中使用到的宏定义
*
****************************************************************************************/
//PMU range....
#define PMU1_1						0x11
#define PMU1_2						0x10
#define PMU1_3						0x12
#define PMU2_1						0x21
#define PMU2_2						0x20
#define PMU2_3						0x22
#define PMU3_1						0x31
#define PMU3_2						0x30
#define PMU3_3						0x32
#define PMU4_1						0x41
#define PMU4_2						0x40
#define PMU4_3						0X42
#define PMU5_1						0x51
#define PMU5_2						0x50
#define PMU5_3						0X52

#define I2C_400K					1
#define I2C_100K					0

#define I_MAX_100mA					1
#define I_MAX_300mA					0

#define PMU_ON						0
#define PMU_OFF						1

#define POWER_ON					1
#define POWER_OFF					0

#define CLK_ON						1
#define CLK_OFF						0

#define IO_PULLUP					1
#define IO_NOPULL					0

#define MULTICAM_NORMAL				0x00
#define MULTICAM_PWDN_NOT			0x01
#define MULTICAM_RESET_PWDN_OVERLAP	0x02
#define MULTICAM_SPECIAL			0x03




/**************************************************************************************** 
*
* 常用错误码
*
****************************************************************************************/
/** @defgroup group10 常用错误码
*  @{

*/
/// @brief 正忙(上一次操作还在进行中)，此次操作不能进行
#define DT_ERROR_BUSY					   	4		///<正忙(上一次操作还在进行中)，此次操作不能进行
/// @brief 需要等待(进行操作的条件不成立)，可以再次尝试
#define DT_ERROR_WAIT                	   	3		///<需要等待(进行操作的条件不成立)，可以再次尝试
/// @brief 正在进行，已经被操作过
#define DT_ERROR_IN_PROCESS               	2		///<正在进行，已经被操作过
/// @brief 操作成功
#define DT_ERROR_OK							1		///<操作成功
/// @brief 操作失败
#define DT_ERROR_FAILED						0		///<操作失败
/// @brief 内部错误
#define DT_ERROR_INTERNAL_ERROR				-1		///<内部错误
/// @brief 未知错误
#define DT_ERROR_UNKNOW						-1		///<未知错误
/// @brief 不支持该功能
#define DT_ERROR_NOT_SUPPORTED				-2		///<不支持该功能
/// @brief 初始化未完成
#define DT_ERROR_NOT_INITIALIZED         	-3		///<初始化未完成
/// @brief 参数无效
#define DT_ERROR_PARAMETER_INVALID       	-4		///<参数无效
/// @brief 参数越界
#define DT_ERROR_PARAMETER_OUT_OF_BOUND  	-5		///<参数越界
/// @brief 未使能
#define DT_ERROR_UNENABLED  				-6		///<未使能

/* IO，存储，设备相关  */
/// @brief 超时错误
#define DT_ERROR_TIME_OUT             	   	-1000	///<超时错误
/// @brief 硬件IO错误
#define DT_ERROR_IO_ERROR                 	-1001	///<硬件IO错误
/// @brief 通讯错误
#define DT_ERROR_COMM_ERROR					-1002	///<通讯错误
/// @brief 总线错误
#define DT_ERROR_BUS_ERROR				   	-1003	///<总线错误
/// @brief 没有发现设备
#define DT_ERROR_NO_DEVICE_FOUND			-1100	///<没有发现设备
/// @brief 未找到逻辑设备
#define DT_ERROR_NO_LOGIC_DEVICE_FOUND   	-1101	///<未找到逻辑设备
/// @brief 设备已经打开
#define DT_ERROR_DEVICE_IS_OPENED			-1102	///<设备已经打开
/// @brief 设备已经关闭
#define DT_ERROR_DEVICE_IS_CLOSED			-1103	///<设备已经关闭

#define DT_ERROR_DEVICE_IS_DISCONNECTED    	-1104  	// 设备已经断开连接
/// @brief 没有足够系统内存
#define DT_ERROR_NO_MEMORY  	   		   	-1200	///<没有足够系统内存

/// @brief 创建文件失败
#define DT_ERROR_FILE_CREATE_FAILED			-1300	///<创建文件失败
/// @brief 文件格式无效
#define DT_ERROR_FILE_INVALID             	-1301	///<文件格式无效

/// @brief 写保护，不可写
#define DT_ERROR_WRITE_PROTECTED			-1400	///<写保护，不可写

/// @brief 数据采集失败
#define DT_ERROR_GRAB_FAILED           	   	-1600	///<数据采集失败
/// @brief 数据丢失，不完整
#define DT_ERROR_LOST_DATA                	-1601	///<数据丢失，不完整
/// @brief 未接收到帧结束符
#define DT_ERROR_EOF_ERROR           	   	-1602	///<未接收到帧结束符
/// @brief 数据采集已经启动
#define DT_ERROR_GRAB_IS_STARTED         	-1605  	// 数据采集已经启动
/** @} */ // end of group10


/****************************************************************************************
*
* 用于方便程序编写，遇到错误时立即返回错误码 
*
****************************************************************************************/
#define CHECK_RETURN(_FUN_) \
{\
	int iChkRet = _FUN_; \
	if (iChkRet != DT_ERROR_OK) \
	return iChkRet; \
	}



#endif // __IMAGEKIT_H__
