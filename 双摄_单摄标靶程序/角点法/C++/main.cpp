#include <iostream>
#include <opencv2\opencv.hpp>
#include "harris.h"
#include "math.h"

using namespace cv;
using namespace std;

typedef struct {
	double u, v, w;
}Para;
Mat Img_resize(Mat I, double scale);
Para calPara(Mat B, Mat I, double Dis, double pixel_size, double f, double D);

/*
%Main program
%============================================
%Author:Ocean
%Date:2017.6.6
%============================================
%Nargin Set
% I:The unfixed image;
% B:The fixed image;
% pixel size: set as 1.12μm;
% scale: The resize scale;
% Dis:The distance of the two cameras;
% f: The focus of the camera;
% D: The objective distance;
%--------------------------------------------
%Output
% adjust_u/v/w;
%============================================
*/

void main(){
	//图像预处理
	Mat I, B;
	I = imread("UNFIX.jpg");
	I = Img_resize(I, 0.2);
	I = I(Rect(200, 120, 420, 300));
	B = imread("FIX.jpg");
	B = Img_resize(B, 0.2);
	B = B(Rect(200, 120, 420, 300));
	//基准距离设置
	double Dis = 18;
	double scale = 0.2;
	double pixel_size = 0.00112 / scale;
	double f = 3.31;
	double D = 600;
	//计算两幅图偏移量
	Para Para;
	Para = calPara(B, I, Dis, pixel_size, f, D);
	double adjust_w = Para.w;
	double adjust_u = Para.v;
	double adjust_v = -Para.u;
	cout << "adjust_w:" << adjust_w << endl;
	cout << "adjust_u:" << adjust_u << endl;
	cout << "adjust_v:" << adjust_v << endl;
	system("pause");
}

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