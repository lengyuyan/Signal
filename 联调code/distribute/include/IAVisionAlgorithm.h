/************************************************************************
名称:视觉算法业务接口
功能:整合OPENCV HALCON USERDEFINE等基础算法库为较为通用的业务算法，适用于
	 AA等其他设备的算法调试
说明:函数可以重载,接口尽可能通用。
IAVO_ 标识Opencv接口
IAVH_ 标识Halcon接口
IAVU_ 标识自定义接口

************************************************************************/
#ifndef _I_IAVISIONALGORITHM__H_
#define _I_IAVISIONALGORITHM__H_

#ifdef IAVISIONALGORITHM_EXPORTS
#define IAVISION_API __declspec(dllexport)
#else
#define IAVISION_API __declspec(dllimport)
#endif

#include <opencv2\opencv.hpp>
using namespace cv;

#include <HalconCpp.h>
using namespace HalconCpp;


#include "HDevEngineCpp.h" //自定义的文件，看halcon中是否有这个函数
using namespace HDevEngineCpp;

#ifndef WINAPI
#define WINAPI __stdcall
#endif



#define IAV_PRINTF(logmsg, ...) printf(logmsg, __VA_ARGS__)


typedef enum _IAV_ERROR_CODE
{
	         
	IAV_ERROR =-1 ,
	IAV_OK = 0,                       
}IAV_ERROR_CODE;

//ROI类型
typedef enum _IAVH_ROI_TYPE
{
	IAVH_RETANGLE = 0,         
	IAVH_CIRCLE ,
	IAVH_INVALID
}IAVH_ROI_TYPE;
//ROI 矩形1结构
typedef struct _IAVH_ROI_RETANGLE1{
	double Row1;
	double Column1;
	double Row2;
	double Column2;

}PIAVH_ROI_RETANGLE1,*PIAVH_RETANGLE1;
//ROI 圆1结构
typedef struct _IAVH_ROI_CIRCLE{
	double Row;
	double Column;
	double rad;

}PIAVH_ROI_CIRCLE,*PIAVH_CIRCLE;

//ROI联合
typedef struct _IAVH_ROI_STRUCT{
	IAVH_ROI_TYPE type;
	union {  
		PIAVH_ROI_RETANGLE1 rect;  
		PIAVH_ROI_CIRCLE cir;
	};
}IAVH_ROI_STRUCT,*PIAVH_ROI_STRUCT;




//HALCON参数 
#define IAVH_PARAM_LEN 32
//形状参数
typedef struct _SelectShapePARAM{
	char Features[IAVH_PARAM_LEN];
	char Operation[IAVH_PARAM_LEN];
	double min;
	double max;
	_SelectShapePARAM()
	{
		strcpy_s(Features,"area");
		strcpy_s(Operation,"and");

		//Operation= "and";
		min = 1000;
		max = 2000;
	}
}SelectShapePARAM,*PSelectShapePARAM;


//自动阈值参数
typedef struct _BinaryThresholdPARAM{
	char Method[IAVH_PARAM_LEN];
	char LightDark[IAVH_PARAM_LEN];
	_BinaryThresholdPARAM()
	{
		strcpy_s(Method,"max_separability");
		strcpy_s(LightDark,"dark");
	}
}BinaryThresholdPARAM,*PBinaryThresholdPARAM;

//自动阈值参数
typedef struct _ScaleImagePARAM{
	double add;
	double mul;
	_ScaleImagePARAM()
	{
		add = 0;
		mul = 1;
	}
}ScaleImagePARAM,*PScaleImagePARAM;



typedef struct _IAVH_PARAM_STRUCT{
	SelectShapePARAM selectShape;  
	BinaryThresholdPARAM binarythreshold;
	ScaleImagePARAM scaleImage;


}IAVH_PARAM_STRUCT,*PIAVH_PARAM_STRUCT;




//工具接口 Opencv
/************************************************************************
功能:	在opencv窗口显示图像
输入:	显示图像矩阵: Mat &img
		窗口名字:     const string windowname
输出:	无
返回值:	成功
说明:	显示后按任意键继续运行,调试用
*************************************************************************/
IAVISION_API int IAVO_ShowPicture(const string windowname,Mat &img);
IAVISION_API int IAVO_SavePicture(string filename,Mat &img);
IAVISION_API Mat IAVO_ImgResize(Mat& img, double scale);
IAVISION_API int IAVO_ReadPicture(string filename,Mat &img);

/************************************************************************
功能:	在opencv窗口显示图像
输入:	显示图像矩阵: Mat &img
		窗口名字:     const string windowname
输出:	无
返回值:	成功
说明:	显示后按任意键继续运行,调试用
*************************************************************************/
IAVISION_API int IAVO_ShowWindow(const string windowname,Mat &img,int windoww =0,int windowh = 0);

//IAVISION_API int IAV_ADD(int a,int b);
//IAVISION_API int IAV_ADD(float a,float b);


//AA算法 Opencv
/************************************************************************
名称:视觉算法业务接口
功能:整合OPENCV HALCON USERDEFINE等基础算法库为较为通用的业务算法，适用于
	 AA等其他设备的算法调试
************************************************************************/
IAVISION_API vector<Point2d> IAV_GetCornerInOnePic(Mat &B);



//工具接口 halcon
/************************************************************************
功能:	在halcon窗口显示图像
输入:	显示图像矩阵: HObject& Object
输出:	无
返回值:	成功0
说明:	显示后按任意键继续运行,调试用,调试用
		此函数个性化太强了,参数先不开放出去，后续有要求再开放
*************************************************************************/
IAVISION_API int IAVH_ShowPicture(HObject& Object);

/************************************************************************
功能:	在halcon绑定的窗口显示图像
输入:	显示图像矩阵: 窗口halconid,HObject& Object
输出:	无
返回值:	成功0
说明:	显示后按任意键继续运行,调试用,调试用
	
*************************************************************************/
IAVISION_API int IAVH_DispImage(int hwinID,HObject& Object);

/************************************************************************
功能:	读取图像
输入:	显示图像矩阵: HObject& Object
输出:	无
返回值:	成功0
说明:	
	
*************************************************************************/
IAVISION_API int IAVH_ReadImage(HObject& Object,char *filename);


/************************************************************************
功能:	在halcon绑定的窗口显示指定位置画十字叉
输入:	窗口halconid,
		double* pt
		double size,
		double angle
输出:	无
返回值:	成功0
说明:	
	
*************************************************************************/
IAVISION_API int IAVH_PaintCross(int hwinID,double* pt,double size = 6.0,double angle = 0.0);


/************************************************************************
功能:	在指定的窗口上用鼠标绘制矩形框
输入:	窗口句柄             : int WindowHandle

输出:	矩形框左上角的Row坐标: double Row1
		矩形框左上角的Col坐标: double Column1
		矩形框右下角的Row坐标: double Row2
		矩形框右下角的Col坐标: double Column2

返回值:	成功0
说明:	按鼠标左键确定矩形的一个角，下拉形成矩形框。在创建矩形的中间再单击鼠标后，可以移动它。
        点击接近一边“抓住”它来修改矩形的尺寸在垂直方向或者在水平方向。
*************************************************************************/
IAVISION_API int IAVH_DrawRectangle1(int WindowHandle,double *Row1, double *Column1, double *Row2, double *Column2);

/************************************************************************
功能:	保存图像
输入:	图像矩阵: HObject& Object
输出:	无
返回值:	成功0
说明:	请提供完整路径及文件名到后缀,如.//iamge//1.jpg,调试用,调试用
		暂时提供.jpg和,bmp两种格式
*************************************************************************/
IAVISION_API int IAVH_SavePicture(HObject& Object,string filename);

/************************************************************************
功能:	保存图像
输入:	图像矩阵: unsigned char* buff
输出:	无
返回值:	成功0
说明:	请提供完整路径及文件名到后缀,如.//iamge//1.jpg,调试用,调试用
		暂时提供.jpg和,bmp两种格式
*************************************************************************/
IAVISION_API int IAVH_SavePictureInByte(unsigned char* buff,string filename,int w,int h,int chNum);


/************************************************************************
功能:	保存窗口图像
输入:	图像矩阵: HObject& Object
输出:	无
返回值:	成功
说明:	请提供完整路径及文件名到后缀,如.//iamge//1.jpg,调试用,调试用
		暂时提供.jpg和,bmp两种格式
*************************************************************************/
IAVISION_API int IAVH_SaveWindowPicture(long winID,string filename);

/************************************************************************
功能:	缩放图像
输入:	图像矩阵: HObject& Object
		输出图像: HObject& OutObject
		是否是比例缩放: bool isfactor ture,是w,h标识比例系数，否则为缩放后的长度和宽度
		图像比例或者大小:double w,double h
		缩放差值类型: interp ="constant"
输出:	无
返回值:	成功0
说明:	请提供完整路径及文件名到后缀,如.//iamge//1.jpg,调试用,调试用
		暂时提供.jpg和,bmp两种格式
*************************************************************************/
IAVISION_API int IAVH_ImgResize(HObject& Object,HObject& OutObject, double w,double h,bool isfactor = true,string  interp ="constant");

/************************************************************************
功能:	获取图像矩形ROI
输入:	图像矩阵: HObject& Object
		输出ROI: HObject& OutObject
		ROI类型: 0矩形，1,圆
		是否显示窗口(调试用):bool showWid
输出:	无
返回值:	成功0
说明:	请提供完整路径及文件名到后缀,如.//iamge//1.jpg,调试用,调试用
		暂时提供.jpg和,bmp两种格式
*************************************************************************/
IAVISION_API int IAVH_GetImgRetangleROI(HObject& Object,HObject& OutObject, IAVH_ROI_STRUCT roiP,bool showWid);

//AA算法  halcon
/************************************************************************
功能:	把byte的图像转化为HOject格式的图像
输入:	图像指针: unsigned char * pBmp
		图像尺寸: int w,int h
		通道数量：int chNum
输出:	输出ROI:  HObject& Object
返回值:	成功0
说明:	暂时只支持单/3通道图像
*************************************************************************/
IAVISION_API int IAVH_ImgByte2HObject(HObject &Object, unsigned char * pBmp,int w,int h,int chNum = 1);


/************************************************************************
功能:	获取图像矩形ROI中一个图像的中心
输入:	图像矩阵: HObject& Object
		参数:     IAVH_PARAM_STRUCT param 
		是否显示窗口(调试用):bool showWid
输出:	输出: double* center
返回值:	成功0
说明:	
*************************************************************************/
IAVISION_API int IAVH_GetImgROICenterByArea(HObject& Object,double* center,IAVH_PARAM_STRUCT param,bool showWid);

/************************************************************************
功能:	获取图像矩形ROI中所有联通区域的面积
输入:	图像矩阵: HObject& Object
		参数:     IAVH_PARAM_STRUCT param 
		是否显示窗口(调试用):bool showWid
输出:	输出: double* center
返回值:	成功0
说明:	
*************************************************************************/
IAVISION_API int IAVH_GetImgROIAreas(HObject& Object,vector<double>&area,IAVH_PARAM_STRUCT param,bool showWid);

/************************************************************************
功能:	获取双摄调整的角度u v w
输入:	一组双摄的5个角点: vector<Point> pointA
		一组双摄的5个角点: vector<Point> pointB
		两摄像头物方间距(mm): double Dis
		一个像素的大小: double pixel_size
		镜头焦距: double f
		镜头物距: double D
		图像的行数: rows
		图像的列数: cols

输出:	调整的角度u v w: double u v w
返回值:	成功0
说明:	
*************************************************************************/
IAVISION_API int IAVH_GetPara(	vector<Point> pointA,
								vector<Point> pointB,
								double Dis, 
								double pixel_size, 
								double f, 
								double D, 
								int rows,
								int cols,
								double u,
								double v,
								double w         );


/************************************************************************
功能:	中值滤波，去除椒盐噪声
输入:	原灰度图像      : HObject &HOBJECT
		滤波窗口形状    : char* MaskType
		滤波窗口的大小  : int Radius
		边缘处理模式    : char* Margin

输出:	滤波后的图像    : HObject &OutHobject
返回值:	成功0
说明:	“窗口形状”、“边缘处理模式”是有固定类型可选择
         窗口形状: "circle"、"square"只有两种模式
		 边缘处理模式: "continued"、"cyclic"、"mirrored"
		 "continued": 使用边缘像素填充
		 "cyclic"   : 使用边界值循环填充
		 "mirrored":  边界像素使用镜像对称
*************************************************************************/
IAVISION_API int IAVH_Median_Filter(HObject &Hobject, char* MaskType, int Radius, char* Margin, HObject &OutHobject);


/************************************************************************
功能:	调用halcon过程来处理
输入:	窗口id,long iWinIDA,iWinIDB
		图像矩阵: HObject& ImageA  HObject& ImageB
		图片的ROI矩形框坐标: double arr[][] 
		路径：string ProcedurePath
		输入参数:    vector<double> &inparam
		是否显示:int display
输出:	vector<double> &outparam
		错误码:int error
		错误信息：hmessage
返回值:	成功0
说明:	
*************************************************************************/
IAVISION_API int IAVH_AAProcedureProcess(long iWinIDA,long iWinIDB,HObject& ImageA,HObject& ImageB,double arr[][7*4],string ProcedurePath,vector<double> &inparam,vector<double> &outparam,int display,int &error,string &hmessage);


/************************************************************************
功能:	获取图像矩形ROI中所有联通区域的右边区域角点
输入:	图像矩阵: HObject& Object
		参数:     IAVH_PARAM_STRUCT param 
		是否显示窗口(调试用):bool showWid
输出:	输出: double* center
返回值:	成功0
说明:	
*************************************************************************/
IAVISION_API int IAVH_GetImgROIRightHarryPoint(HObject& Object,vector<double>&point,IAVH_PARAM_STRUCT param,bool showWid);




///以下为自定义模块
/************************************************************************
功能:	Hobject---->Mat
输入:
		图像矩阵: HObject& Hobj
输出:	
返回值:	成功Mat
说明:	
*************************************************************************/
IAVISION_API Mat HImageToMat(HObject &Hobj)  ;


/************************************************************************
功能:	Mat ---->Hobject
输入:
		图像矩阵: HObject& Hobj
输出:	
返回值:	成功HObject
说明:	
*************************************************************************/
IAVISION_API HObject MatToHImage(Mat& pImage)  ;

#endif 