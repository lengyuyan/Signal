

#include <iostream>
#include <opencv2\opencv.hpp>
#include "math.h"
#include <numeric>

#include "AAalgorithm.h"

#include "AA_define_tool.h"
using namespace cv;
using namespace std;

extern  AA_Globle_Param g_AAParam;//引入全局变量

/*
%calPara()
%============================================
%Author:Ocean
%Date:2017.6.15
%============================================
%Nargin Set
% I:The unfixed image;
% B:The fixed image;
% pixel size: set as 1.12μm;
% Dis:The distance of the two cameras;
% f: The focus of the camera;
% D: The objective distance;
%--------------------------------------------
%Output
% adjust_u/v/w
%============================================
*/

Para calPara(Mat B, Mat I, double Dis, double pixel_size, double f, double D){
	vector<Point> PointI, PointB;
	Para Para;
	PointI = getCorner(I);
	PointB = getCorner(B);
	double d = 1 / (1 / f - 1 / D);
	double dis = Dis * d / D;
	double dis_pixel = dis / pixel_size;
	//B角度
	double kb1 = (double)(PointB[1].y - PointB[2].y) / (double)(PointB[1].x - PointB[2].x);
	double kb2 = (double)(PointB[3].x - PointB[4].x) / (double)(PointB[3].y - PointB[4].y) * -1;
	double angleB1 = atan(kb1) / 3.1415926 * 180;
	double angleB2 = atan(kb2) / 3.1415926 * 180;
	//I角度
	double kI1 = (double)(PointI[1].y - PointI[2].y) / (double)(PointI[1].x - PointI[2].x);
	double kI2 = (double)(PointI[3].x - PointI[4].x) / (double)(PointI[3].y - PointI[4].y) * -1;
	double angleI1 = atan(kI1) / 3.1415926 * 180;
	double angleI2 = atan(kI2) / 3.1415926 * 180;
	Para.w = ((angleI1 - angleB1) + (angleI2 - angleB2))/2;
	//计算uv
	Point centerB, centerI; 
	centerB.x = 0; centerB.y = 0;
	centerI.x = 0; centerI.y = 0;
	for (int i = 0; i < PointB.size(); i++){
		centerB.x = centerB.x + PointB[i].x;
		centerB.y = centerB.y + PointB[i].y;
		centerI.x = centerI.x + PointI[i].x;
		centerI.y = centerI.y + PointI[i].y;
	}
	centerB.x = centerB.x / PointB.size();
	centerB.y = centerB.y / PointB.size();
	centerI.x = centerI.x / PointI.size();
	centerI.y = centerI.y / PointI.size();
	int delta_x = centerI.x - centerB.x - dis_pixel;
	int delta_y = centerI.y - centerB.y;
	Para.v = (delta_y * pixel_size / f) / 3.1415926 * 180;
	Para.u = (delta_x * pixel_size / f) / 3.1415926 * 180;
	return Para;
}



/*
%getCorner()
%质心法
%============================================
%Author:Ocean
%Date:2017.6.15
%============================================
%Nargin Set
% I:The unfixed image;
% B:The fixed image;
%--------------------------------------------
%Output
% Points of I and B;
%============================================
*/
//求质心得方法跟开源的求质心得方法有什么不同
vector<Point> getCorner(Mat I){
	vector<Point> PointI;


	//选框初始位置设置，CAD制作靶图后可通过公式计算，左上点位置  长间距32.5cm 短间距22cm
	Point center, left, right, top, bottom;
	center.x = 1800; center.y = 1400;
	left.x = 200; left.y = 1400;
	right.x = 3500; right.y = 1400;
	top.x = 1800; top.y = 300;
	bottom.x = 1800; bottom.y = 2500;
	//参数设定
	double size = 500;//选框大小
	double Centroidthreshold =0.5; //质心阈值,由0.3-->0.5

	double scale = g_AAParam.doubleAAscale;	
	double pixel_size = g_AAParam.pixel_size/g_AAParam.doubleAAscale;
	double d_img = 1 / (1 / g_AAParam.focus - 1 / g_AAParam.objdis);
	double beta = d_img / g_AAParam.objdis; 


	center.x = g_AAParam.doubleAApiccenterX*scale;
	center.y = g_AAParam.doubleAApiccenterY*scale;
	//center.x = g_AAParam.pic_lenth / 2 * scale, center.y = g_AAParam.pic_width / 2 * scale;
	left.x= ceil(center.x - g_AAParam.doubleAAmapdislength * beta / pixel_size);
	right.x = ceil(center.x + g_AAParam.doubleAAmapdiswidth * beta / pixel_size);
	left.y =  right.y = center.y;

	top.y = ceil(center.y - g_AAParam.mapdiswidth * beta / pixel_size);
	bottom.y = ceil(center.y + g_AAParam.mapdiswidth * beta / pixel_size);
	top.x =bottom.x = center.x ;

	center.x = 1800; center.y = 1200;
	left.x = 200; left.y = 1200;
	right.x = 3500; right.y = 1200;
	top.x = 1800; top.y = 100;
	bottom.x = 1800; bottom.y = 2300;


	size = g_AAParam.doubleAAsize;
	Centroidthreshold = g_AAParam.doubleAACentroidthreshold;


	vector<Point> loc;
	loc.push_back(center); loc.push_back(left); loc.push_back(right); loc.push_back(top); loc.push_back(bottom);

	//选出五个子区域
	vector<Mat> I_block;
	for (int i = 0; i < 5; i++){
		I_block.push_back(I(Rect(loc[i].x, loc[i].y, size, size)));
		/*
		CvPoint pt,pt2;
		pt.x = loc[i].x;
		pt.y = loc[i].y;
		pt2.x = loc[i].x +size;
		pt2.y = loc[i].y +size;
	    rectangle(I,pt,pt2,cvScalar(255, 0, 0),1,8,0);
		*/
	}
	/*
	Mat d;
	resize(I, d, Size(1600, 1200));
	imshow("d",d);
	*/
	
	//计算得到fix图的质心
	double max_thx = 0;
	double min_thx = size * 255;
	double max_thy = 0;
	double min_thy = size * 255;
	double th_x, th_y;
	//计算得到图像各个区域的质心
	for (int i = 0; i < 5; i++){
		Point temp, temp1;
		temp1.x = 1; temp1.y = 1;
		double sum_x[500], sum_y[500];          //需要按照size的值设定一下
		vector<double> x_th, x_thh, y_th, y_thh;
		//求质心
		for (int j = 0; j < size; j++){
			sum_x[j] = 0;
			sum_y[j] = 0;
			for (int k = 0; k < size; k++){
				sum_x[j] = sum_x[j] + I_block[i].at<float>(j, k);
				sum_y[j] = sum_y[j] + I_block[i].at<float>(k, j);
			}
			if (sum_x[j] > max_thx){
				max_thx = sum_x[j];
			}
			if (sum_x[j] < min_thx){
				min_thx = sum_x[j];
			}
			if (sum_y[j] > max_thy){
				max_thy = sum_y[j];
			}
			if (sum_y[j] < min_thy){
				min_thy = sum_y[j];
			}
		}
		th_x = (max_thx - min_thx) * Centroidthreshold + min_thx;
		th_y = (max_thy - min_thy) * Centroidthreshold + min_thy;
		for (int j = 0; j < size; j++){
			if (sum_x[j] > th_x){
				x_th.push_back(sum_x[j]);
				x_thh.push_back(sum_x[j] * j);
			}
			if (sum_y[j] > th_y){
				y_th.push_back(sum_y[j]);
				y_thh.push_back(sum_y[j] * j);
			}
		}
		double sum_xth = accumulate(x_th.begin(), x_th.end(), 0);
		double sum_xhh = accumulate(x_thh.begin(), x_thh.end(), 0);
		double sum_yth = accumulate(y_th.begin(), y_th.end(), 0);
		double sum_yhh = accumulate(y_thh.begin(), y_thh.end(), 0);
		temp.y = sum_xhh / sum_xth;
		temp.x = sum_yhh / sum_yth;
		PointI.push_back(temp + loc[i] - temp1);
		circle(I, PointI[i],3,Scalar(255,0,0),-1);
		//msg("质心:%d,%d,%d,%")
	}
	
	

	Mat d;
	resize(I, d, Size(1600, 1200));
	imshow("d",d);
	
	return PointI;
}