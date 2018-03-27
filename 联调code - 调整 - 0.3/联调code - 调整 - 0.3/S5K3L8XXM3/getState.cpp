#include <opencv2/core/core.hpp> 
#include <opencv2/highgui/highgui.hpp> 
#include "polyfit.h"
#include <iostream>
#include "AA_define_tool.h"

#include "AAalgorithm.h"

using namespace std;
using namespace cv;
using namespace czy;

extern  AA_Globle_Param g_AAParam;//引入全局变量



/*
%get the current state of the initial image including u, v and z
%============================================
%Author:Chame & Ocean(Using polyfit.h and vector)
%Date:2017.6.5
%============================================
%Input
% ima_ori: the original image in the initial position;
% ima_p: the positive image in the initial position;
% ima_m: the minus image in the initial position;
% deltaZ_p: the distance of image moving to the plus direction;
% deltaZ_m: the distance of image moving to the minus direction;
% pixel_size: the size of CCD pixel;
% scale: the image scale's scale;
%--------------------------------------------
%Output
% deltaZ: the distance of the initial image to the focal plane;
% x_offset, y_offset: horizontal(x) offset and vertical(y) offset;
% x_angle, y_angle: horizontal(x) angle and vertical(y) angle;
%============================================
*/

Mat Img_resize(Mat I, double scale){
	if (I.channels() == 3){
		cvtColor(I, I, CV_RGB2GRAY);
	}
	int height = I.rows * scale;
	int width = I.cols * scale;
	resize(I, I, Size(width, height));
	I.convertTo(I, CV_32F);
	I = I / 255;
	return I;
}
string ConvertToString(double value)
{
	stringstream ss;
	ss << value;
	return ss.str();
}

State getState(Mat img_ori, Mat img_p, Mat img_m, double deltaZ_p, double deltaZ_m, double pixel_size,double scale,int temType,int * flag)
{
	//参数设定，如有需要，可挪移至主函数
	double f = 3.31;                      //镜头焦距为3.31mm
	double d_obj = 500;					  //物距为600mm
	double size = 90, block = 140;        //选框窗口左上点在基准点左上90像素处，block长宽为140像素
	int edge_size = 15;                   //刃边框尺寸长宽为15像素
	int block_size = 19;                  //极大值抑制判断刃边位置尺寸长宽为19像素
	float thr = pow((2 * (float)block_size + 1), 2) / 2 * 0.2;   //极大值抑制判断抑制



	f= g_AAParam.focus;
	d_obj = g_AAParam.objdis;
	size =g_AAParam.size;
	block = g_AAParam.block;
	edge_size = g_AAParam.edge_size;
	block_size = g_AAParam.block_size;
	thr = pow((2 * (float)block_size + 1), 2) / 2 * 0.2;

	//计算三幅图像的laplace量
	State state= {0};
	State state1 = {0};
	int Laplace_Flag_o;
	int Laplace_Flag_p;
	int Laplace_Flag_m;
	HV_Lap hv_lap_o = getEdgeLaplace(img_ori,temType, &Laplace_Flag_o);
	HV_Lap hv_lap_p = getEdgeLaplace(img_p,temType, &Laplace_Flag_p);
	HV_Lap hv_lap_m = getEdgeLaplace(img_m,temType, &Laplace_Flag_m);
	if(Laplace_Flag_o<0 || Laplace_Flag_p<0 || Laplace_Flag_m<0)
	{
		*flag= AA_lAP_ERROR;
		//State state1;
		return state1;
	}
	else
	{
		*flag = AA_lAP_OK;
	}
	//存放在向量中的顺序为:中间-右边-左边   中间-上边-下边
	//原点位置
	vector<Point> x_pt_o = hv_lap_o.horizontal_pt;//水平3个点
	vector<Point> y_pt_o = hv_lap_o.vertical_pt;//垂直3个点
	vector<double> x_lap_o = hv_lap_o.horizontal_lap;//水平3个拉普拉斯
	vector<double> y_lap_o = hv_lap_o.vertical_lap;//垂直3个拉普拉斯

	//前面位置拉普
	vector<double> x_lap_p  = hv_lap_p.horizontal_lap;
	vector<double> y_lap_p = hv_lap_p.vertical_lap;
	//后面位置拉普
	vector<double> x_lap_m = hv_lap_m.horizontal_lap;
	vector<double> y_lap_m = hv_lap_m.vertical_lap;

    //判断单边情况
	if (x_lap_o[1] < x_lap_m[1] || x_lap_o[1] < x_lap_p[1]){//水平--位置
		*flag=  AA_lAP_SIGNEL;
		if (x_lap_m[1] <=  x_lap_p[1]) {state.OneSide_flag[0] = 1;}
		if (x_lap_m[1] >  x_lap_p[1]) {state.OneSide_flag[0] = -1;}
		cout<<"x_lap_o[1]:"<<x_lap_o[1]<<"x_lap_m[1]:"<<x_lap_m[1]<<"x_lap_p[1]:"<<x_lap_p[1]<<endl;
		cout << "center_x is error" << endl;
	}
	if (y_lap_o[1] < y_lap_m[1] || y_lap_o[1] < y_lap_p[1]){//垂直中间位置
		*flag= AA_lAP_SIGNEL;
		if (y_lap_m[1] <=  y_lap_p[1]) {state.OneSide_flag[1] = 1;}
		if (y_lap_m[1] >  y_lap_p[1]) {state.OneSide_flag[1] = -1;}
		cout<<"y_lap_o[1]:"<<y_lap_o[1]<<"y_lap_m[1]:"<<y_lap_m[1]<<"y_lap_p[1]:"<<y_lap_p[1]<<endl;
		cout << "center_y is error" << endl;
	}
	if (x_lap_o[0] < x_lap_m[0] || x_lap_o[0] < x_lap_p[0]){
		*flag= AA_lAP_SIGNEL;
		if (x_lap_m[0] <=  x_lap_p[0]) {state.OneSide_flag[2] = 1;}
		if (x_lap_m[0] >  x_lap_p[0]) {state.OneSide_flag[2] = -1;}
		cout<<"x_lap_o[0]:"<<x_lap_o[0]<<"x_lap_m[0]:"<<x_lap_m[0]<<"x_lap_p[0]:"<<x_lap_p[0]<<endl;
		cout << "left is error" << endl;
	
	}
	if (x_lap_o[2] < x_lap_m[2] || x_lap_o[2] < x_lap_p[2]){
		*flag= AA_lAP_SIGNEL;
		if (x_lap_m[2] <=  x_lap_p[2]) {state.OneSide_flag[3] = 1;}
		if (x_lap_m[2] >  x_lap_p[2]) {state.OneSide_flag[3] = -1;}
		cout<<"x_lap_o[2]:"<<x_lap_o[2]<<"x_lap_m[2]:"<<x_lap_m[2]<<"x_lap_p[2]:"<<x_lap_p[2]<<endl;
		cout << "right is error" << endl;
	
	}
	if (y_lap_o[0] < y_lap_m[0] || y_lap_o[0] < y_lap_p[0]){
		*flag= AA_lAP_SIGNEL;
		if (y_lap_m[0] <=  y_lap_p[0]) {state.OneSide_flag[4] = 1;}
		if (y_lap_m[0] >  y_lap_p[0]) {state.OneSide_flag[4] = -1;}
		cout<<"y_lap_o[0]:"<<y_lap_o[0]<<"y_lap_m[0]:"<<y_lap_m[0]<<"y_lap_p[0]:"<<y_lap_p[0]<<endl;
		cout << "top is error" << endl;
	}
	if (y_lap_o[2] < y_lap_m[2] || y_lap_o[2] < y_lap_p[2]){
		*flag= AA_lAP_SIGNEL;
		if (y_lap_m[2] <=  y_lap_p[2]) {state.OneSide_flag[5] = 1;}
		if (y_lap_m[2] >  y_lap_p[2]) {state.OneSide_flag[5] = -1;}
		cout<<"y_lap_o[2]:"<<y_lap_o[2]<<"y_lap_m[2]:"<<y_lap_m[2]<<"y_lap_p[2]:"<<y_lap_p[2]<<endl;
		cout << "bottom is error" << endl;
	}
	vector<double> Getangle_x;
	vector<double> Getangle_y;
	//*flag=0;
	//计算x方向的偏转角度
	vector<double> x_pt;
	for (int i = 0; i < 3; i++)
	{
		x_pt.push_back(x_pt_o[i].x);
	}
	cout << endl << "Fitting!" << endl;
	cout << endl << "Delta_Z：" ;
	Getangle_x = getangle(x_lap_o, x_lap_p, x_lap_m, x_pt, deltaZ_p, deltaZ_m, g_AAParam.pixel_size/g_AAParam.scale);
	state.x_angle = Getangle_x[0];
	//计算y方向的偏转角度
	vector<double> y_pt;
	for (int i = 0; i < 3; i++)
	{
		y_pt.push_back(y_pt_o[i].y);
	}
	Getangle_y = getangle(y_lap_o, y_lap_p, y_lap_m, y_pt, deltaZ_p, deltaZ_m, g_AAParam.pixel_size/g_AAParam.scale);
	state.y_angle = Getangle_y[0];
	state.deltaZ = (Getangle_x[2] + Getangle_y[2])/2;
	cout << state.deltaZ ;
	return state;
}
/*求解角度*/
vector<double> getangle(vector<double> lap_o, vector<double> lap_p, vector<double> lap_m, vector<double> pt_o, double deltaZ_p, double deltaZ_m, double pixel_size)
{
	vector<double> deltaZ;
	vector<double> x;   
	x.push_back(deltaZ_p); x.push_back(0); x.push_back(deltaZ_m);
	//将移动范围分割为100份，用来拟合二次曲线
	double dx = (deltaZ_m - deltaZ_p) / 100;
	double x_temp[101] = { 0 };
	double lap[101] = { 0 };
	for (int i = 0; i < 101; i++)
	{
		x_temp[i] = deltaZ_p + i * dx;
	}
	
	for (int i = 0; i < lap_o.size(); i++)
	{
		vector<double> y;
		czy::Fit fit;
		y.push_back(lap_p[i]);
		y.push_back(lap_o[i]);
		y.push_back(lap_m[i]);
		//利用三张图同一位置的模糊量，拟合二次曲线，得到峰值所在位置
		fit.polyfit(x, y, 2, true);
		for (int m = 0; m < 101; m++){
			lap[m] = fit.getY(x_temp[m]);
		}
		int max_loc = maxloc(lap, 101);
		deltaZ.push_back(deltaZ_p + max_loc * dx);
	}
	for (int i = 0; i < 3; i++)
	{
		pt_o[i] = pt_o[i] * pixel_size;
	}
	double deltaZZ = deltaZ[1];   //为了返回中心点deltaZ
	deltaZ[1] = 2 * deltaZ[1]; //为何*2?
	
	//利用离焦量拟合标靶倾斜的一次函数
	czy::Fit fit_l;
	fit_l.linearFit(pt_o, deltaZ);
	//利用拟合得到的斜率得到倾斜角度，单位为°
	double angle = atan(fit_l.getFactor(1)) / 3.14159265 * 180;
	vector<double> angleAZ;
	angleAZ.push_back(angle);
	angleAZ.push_back(deltaZ[0]);
	angleAZ.push_back(deltaZZ);
	angleAZ.push_back(deltaZ[2]);
	return angleAZ;
}

/*
得到数组中最大值的位置
*/
int maxloc(double a[], int size)
{
	double max = 0;
	int maxloc = 0;
	for (int i = 0; i < size; i++)
	{
		if (max < a[i])
		{
			max = a[i];
			maxloc = i;
		}
	}
	return maxloc;
}