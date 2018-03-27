#pragma once
#include <opencv2\opencv.hpp>
using namespace cv;

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


State getState(Mat img_ori, Mat img_p, Mat img_m, double deltaZ_p, double deltaZ_m, double pixel_size, double scale, int temType,int *flag);
State getState5(Mat img_1, Mat img_2, Mat img_3, Mat img_4, Mat img_5, double pixel_size, double scale,int temType);
OFF_set getOffcenter(Mat img, double pixel_size);
Mat Img_resize(Mat I, double scale);
string ConvertToString(double value);

void SelectTemplateType(int temPlate,int blocksize, Mat& h_x,Mat& h_y);
void getFilterValue(int temPlatetype,Mat &img,Mat& temPlate, int Rectx,int Recty,int &size,int &block,Mat &value,int dir);//dir:01234 center left right top bottom

void getFilterAreaMaxValue(int temPlatetype,Point& max_value, int Rectx,int Recty,int &size,int &block,Mat &value,int dir);//获取过滤部分的最大值


bool isRectOverPicture(int piclength,int picwidth,int& leftx,int &lefty,int &size,int &block,int temPlate);



//pravite
HV_Lap getEdgeLaplace(Mat img,int temPlate, int * Laplace_Flag);
vector<double> getangle(vector<double> lap_o, vector<double> lap_p, vector<double> lap_m, vector<double> pt_o, double deltaZ_p, double deltaZ_m, double pixel_size);
int maxloc(double a[], int size);



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

