#pragma once
#include <opencv2\opencv.hpp>
using namespace cv;

#include "IAVisionAlgorithm.h"

typedef struct {
	double x_angle;
	double y_angle;
	double deltaZ;
	int OneSide_flag[6];
}State; // get the current state of the initial image including u, v and z

typedef struct {
	double x_offset;
	double y_offset;
}OFF_set;


typedef struct{
	vector<double> horizontal_lap;
	vector<double> vertical_lap;
	vector<Point> horizontal_pt;
	vector<Point> vertical_pt;
}HV_Lap;

enum AADIR
{
	CENTERX,
	CENTERY,
	LEFT,
	RIGHT,
	TOP,
	BOTTOM
};

enum AA_lAP_RESULT
{
	AA_lAP_ERROR = -2,
	AA_lAP_SIGNEL , //单边
	AA_lAP_OK //OK
};

enum AA_POINT_INDEX //选取的点的索引,小黑块的4个点,从左往右,从下往上
{
	AA_FIRST = 0,
	AA_SECOND , 
	AA_THIRD, 
	AA_FOUTH,
	AA_DEFAULT
};

typedef struct {
	double row;
	double col;
}MyPoints;


State getState(Mat img_ori, Mat img_p, Mat img_m, double deltaZ_p, double deltaZ_m, double pixel_size, double scale,int cameraID,int * flag);
State getState5(Mat img_1, Mat img_2, Mat img_3, Mat img_4, Mat img_5, double pixel_size, double scale,int cameraID);
OFF_set getOffcenter(Mat img, double pixel_size);
Mat Img_resize(Mat I, double scale);
string ConvertToString(double value);

void SelectTemplateType(int temPlate,int blocksize, Mat& h_x,Mat& h_y);
void getFilterValue(int temPlatetype,Mat &img,Mat& temPlate, int Rectx,int Recty,int &size,int &block,Mat &value,int dir);//dir:01234 center left right top bottom

void getFilterAreaMaxValue(int temPlatetype,Point& max_value, int Rectx,int Recty,int &size,int &block,Mat &value,int dir);//获取过滤部分的最大值


bool isRectOverPicture(int piclength,int picwidth,int& leftx,int &lefty,int &size,int &block,int temPlate);



//pravite
HV_Lap getEdgeLaplace(Mat img,int cameraID, int * Laplace_Flag);
vector<double> getangle(vector<double> lap_o, vector<double> lap_p, vector<double> lap_m, vector<double> pt_o, double deltaZ_p, double deltaZ_m, double pixel_size);
int maxloc(double a[], int size);


//重新分解单设的计算
//计算刃点还不能用halcon实现，只能用opencv来实现

//计算3张图的清晰度:img_ori为当前位置，img_p为向下移动,img_m为向上移动，deltaZ_p为向下移动距离，vid为视觉id，flag为是否有峰值
int calZUV(Mat img_ori, Mat img_p, Mat img_m, double deltaZ_p, double deltaZ_m,int vid,State &result);
//获取所有的点和MTF值
int calHarryPointsAndMTFValueInOnePic(Mat img,int vid,HV_Lap &result);
//根据图像计算清晰点和清晰值
int calEdgePointInRegion(Mat img,int vid,int regiontype,Point& edgePoint,double& lapVaule);



//getState5.cpp中的getangle的函数名方法改成getangleFiveMethod
double getangleFiveMethod(vector<double> pt_o, vector<double> Y);
int showPicture(Mat &img,const string windowname);
int savePicture(Mat &img, string filename);
double laplace(Mat I, Point Loc, int edge_size);






///DoubleAA STUCT AND FUN
typedef struct {
	double u, v, w;
}Para;


Para calPara(Mat B, Mat I, double Dis, double pixel_size, double f, double D);
vector<Point> getCorner(Mat I);//内部使用

//角点法
typedef struct {
	vector<Point> PointB, PointI;
}Points;
Points getCorner2(Mat B, Mat I);
Para calPara2(Mat B, Mat I, double Dis, double pixel_size, double f, double D);

Points getCorner3(Mat B, Mat I);
Para calPara3(Mat B, Mat I, double Dis, double pixel_size, double f, double D);


///halocn计算算子


Para calUVW(int A,int B,HObject &ObjectA,HObject& ObjectB);
Para calUVWByPoint(vector<MyPoints>& centerA,vector<MyPoints>& centerB);
int getAllRegionPoints(int vid,HObject Object,vector<MyPoints>& center);
//获取指定区域的某个中心
int calCenterPoints(int vid,HObject Object,int regiontype,vector<MyPoints>& center);

