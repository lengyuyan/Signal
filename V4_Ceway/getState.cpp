#include <opencv2/core/core.hpp> 
#include <opencv2/highgui/highgui.hpp> 
#include "polyfit.h"
#include <iostream>

using namespace std;
using namespace cv;
using namespace czy;

typedef struct {
	double x_angle;
	double y_angle;
	double deltaZ;
}State; // get the current state of the initial image including u, v and z
typedef struct{
	vector<double> horizontal_lap;
	vector<double> vertical_lap;
	vector<Point> horizontal_pt;
	vector<Point> vertical_pt;
}HV_Lap;

HV_Lap getEdgeLaplace(Mat img, int block_size, float thr, int edge_size, int error, double f, double scale, double pixel_size, double d_obj, double size, double block);
vector<double> getangle(vector<double> lap_o, vector<double> lap_p, vector<double> lap_m, vector<double> pt_o, double deltaZ_p, double deltaZ_m, double pixel_size);
int maxloc(double a[], int size);
/*
%get the current state of the initial image including u, v and z
%============================================
%Author:Chame & Ocean(Using polyfit.h and vector)
%Date:2017.4.10
%============================================
%Input
% ima_ori: the original image in the initial position;
% deltaZ_p: the distance of image moving to the plus direction;
% deltaZ_m: the distance of image moving to the minus direction;
% img_p: the image moved deltaZ_p distance to the plus direction;
% img_m: the image moved deltaZ_m distance to the minus direction;
% pixel_size: the size of CCD pixel;
% scale: the image scale's scale;
%--------------------------------------------
%Output
% deltaZ: the distance of the initial image to the focal plane;
% x_offset, y_offset: horizontal(x) offset and vertical(y) offset;
% x_angle, y_angle: horizontal(x) angle and vertical(y) angle;
%============================================
*/
State getState(Mat img_ori, Mat img_p, Mat img_m, double deltaZ_p, double deltaZ_m, double pixel_size, double scale)
{
	//参数设定，如有需要，可挪移至主函数
	double f = 3.31;                      //镜头焦距为3.31mm
	double d_obj = 600;					  //物距为600mm
	double size = 90, block = 140;        //选框窗口左上点在基准点左上90像素处，block长宽为140像素
	int edge_size = 15;                   //刃边框尺寸长宽为15像素
	int error = 40;                       //选框error阈值为40
	int block_size = 19;                  //极大值抑制判断刃边位置尺寸长宽为19像素
	float thr = pow((2 * (float)block_size + 1), 2) / 2 * 0.2;   //极大值抑制判断抑制


	//计算三幅图像的laplace量
	State state;
	HV_Lap hv_lap_o = getEdgeLaplace(img_ori, block_size, thr, edge_size, error, f, scale, pixel_size, d_obj, size, block);
	HV_Lap hv_lap_p = getEdgeLaplace(img_p, block_size, thr, edge_size, error, f, scale, pixel_size, d_obj, size, block);
	HV_Lap hv_lap_m = getEdgeLaplace(img_m, block_size, thr, edge_size, error, f, scale, pixel_size, d_obj, size, block);

	vector<Point> x_pt_o = hv_lap_o.horizontal_pt;
	vector<Point> y_pt_o = hv_lap_o.vertical_pt;
	vector<double> x_lap_o = hv_lap_o.horizontal_lap;
	vector<double> y_lap_o = hv_lap_o.vertical_lap;
	vector<double> x_lap_p  = hv_lap_p.horizontal_lap;
	vector<double> y_lap_p = hv_lap_p.vertical_lap;
	vector<double> x_lap_m = hv_lap_m.horizontal_lap;
	vector<double> y_lap_m = hv_lap_m.vertical_lap;

    //判断单边情况
	if (x_lap_o[1] < x_lap_m[1] || x_lap_o[1] < x_lap_p[1]){
		cout << "center_x is error" << endl;
	}
	if (y_lap_o[1] < y_lap_m[1] || y_lap_o[1] < y_lap_p[1]){
		cout << "center_y is error" << endl;
	}
	if (x_lap_o[0] < x_lap_m[0] || x_lap_o[0] < x_lap_p[0]){
		cout << "left is error" << endl;
	}
	if (x_lap_o[2] < x_lap_m[2] || x_lap_o[2] < x_lap_p[2]){
		cout << "right is error" << endl;
	}
	if (y_lap_o[0] < y_lap_m[0] || y_lap_o[0] < y_lap_p[0]){
		cout << "top is error" << endl;
	}
	if (y_lap_o[2] < y_lap_m[2] || y_lap_o[2] < y_lap_p[2]){
		cout << "bottom is error" << endl;
	}
	//计算x方向的偏转角度
	vector<double> x_pt;
	for (int i = 0; i < 3; i++)
	{
		x_pt.push_back(x_pt_o[i].x);
	}
	cout << endl << "Fitting!" << endl;
	cout << endl << "Delta_Z：" << endl;
	vector<double> Getangle_x = getangle(x_lap_o, x_lap_p, x_lap_m, x_pt, deltaZ_p, deltaZ_m, pixel_size);
	state.x_angle = Getangle_x[0];
	//计算y方向的偏转角度
	vector<double> y_pt;
	for (int i = 0; i < 3; i++)
	{
		y_pt.push_back(y_pt_o[i].y);
	}
	vector<double> Getangle_y = getangle(y_lap_o, y_lap_p, y_lap_m, y_pt, deltaZ_p, deltaZ_m, pixel_size);
	state.y_angle = Getangle_y[0];
	state.deltaZ = (Getangle_x[2] + Getangle_y[2])/2;
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
	deltaZ[1] = 2 * deltaZ[1];
	
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