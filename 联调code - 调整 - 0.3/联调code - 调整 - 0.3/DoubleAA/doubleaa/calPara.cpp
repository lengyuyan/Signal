#include <iostream>
#include <opencv2\opencv.hpp>
#include "harris.h"
#include "math.h"

using namespace cv;
using namespace std;

typedef struct {
	vector<Point> PointB, PointI;
}Points;
typedef struct {
	double u, v, w;
}Para;
Points getCorner(Mat B, Mat I);

/*
%calPara()
%============================================
%Author:Ocean
%Date:2017.6.15
%============================================
%Nargin Set
% I:The unfixed image;
% B:The fixed image;
% pixel size: set as 1.12¶Ãm;
% Dis:The distance of the two cameras;
% f: The focus of the camera;
% D: The objective distance;
%--------------------------------------------
%Output
% adjust_u/v/w
%============================================
*/

Para calPara(Mat B, Mat I, double Dis, double pixel_size, double f, double D){
	Points Points;
	Para Para;
	Points = getCorner(B, I);
	vector<Point> PointB = Points.PointB;
	vector<Point> PointI = Points.PointI;
	double d = 1 / (1 / f - 1 / D);
	double dis = Dis * d / D;
	double dis_pixel = dis / pixel_size;
	//BΩ«∂»
	double kb1 = (double)(PointB[1].y - PointB[2].y) / (double)(PointB[1].x - PointB[2].x);
	double kb2 = (double)(PointB[3].x - PointB[4].x) / (double)(PointB[3].y - PointB[4].y) * -1;
	double angleB1 = atan(kb1) / 3.1415926 * 180;
	double angleB2 = atan(kb2) / 3.1415926 * 180;
	//IΩ«∂»
	double kI1 = (double)(PointI[1].y - PointI[2].y) / (double)(PointI[1].x - PointI[2].x);
	double kI2 = (double)(PointI[3].x - PointI[4].x) / (double)(PointI[3].y - PointI[4].y) * -1;
	double angleI1 = atan(kI1) / 3.1415926 * 180;
	double angleI2 = atan(kI2) / 3.1415926 * 180;
	Para.w = ((angleI1 - angleB1) + (angleI2 - angleB2))/2;
	//º∆À„uv
	Point centerB, centerI; 
	centerB.x = 0; centerB.y = 0;
	centerI.x = 0; centerI.y = 0;
	for (int i = 0; i < Points.PointB.size(); i++){
		centerB.x = centerB.x + Points.PointB[i].x;
		centerB.y = centerB.y + Points.PointB[i].y;
		centerI.x = centerI.x + Points.PointI[i].x;
		centerI.y = centerI.y + Points.PointI[i].y;
	}
	centerB.x = centerB.x / Points.PointB.size();
	centerB.y = centerB.y / Points.PointB.size();
	centerI.x = centerI.x / Points.PointB.size();
	centerI.y = centerI.y / Points.PointB.size();
	int delta_x = centerI.x - centerB.x - dis_pixel;
	int delta_y = centerI.y - centerB.y;
	Para.v = (delta_y * pixel_size / f) / 3.1415926 * 180;
	Para.u = (delta_x * pixel_size / f) / 3.1415926 * 180;
	return Para;
}


