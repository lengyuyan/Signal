#include <iostream>
#include <opencv2\opencv.hpp>
#include "math.h"
#include <numeric>

using namespace cv;
using namespace std;

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
	//选框初始位置设置，CAD制作靶图后可通过公式计算
	Point center, left, right, top, bottom;
	center.x = 1800; center.y = 1400;
	left.x = 200; left.y = 1400;
	right.x = 3500; right.y = 1400;
	top.x = 1800; top.y = 300;
	bottom.x = 1800; bottom.y = 2500;
	vector<Point> loc;
	loc.push_back(center); loc.push_back(left); loc.push_back(right); loc.push_back(top); loc.push_back(bottom);
	//参数设定
	double size = 500;
	//选出五个子区域
	vector<Mat> I_block;
	for (int i = 0; i < 5; i++){
		I_block.push_back(I(Rect(loc[i].x, loc[i].y, size, size)));
	}
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
		th_x = (max_thx - min_thx) * 0.3 + min_thx;
		th_y = (max_thy - min_thy) * 0.3 + min_thy;
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
	}
	return PointI;
}