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
%Date:2017.6.6
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
	Points Points;
	Para Para;
	Points = getCorner(B, I);
	vector<Point> PointB = Points.PointB;
	vector<Point> PointI = Points.PointI;
	Point Btl, Btr, Bbl, Bbr, Bc, Itl, Itr, Ibl, Ibr, Ic;
	double d = 1 / (1 / f - 1 / D); 
	double dis = Dis * d / D;
	double dis_pixel = dis / pixel_size;
	//角点归类
	for (int i = 0; i < PointB.size(); i++){
		if (PointB[i].x<160 && PointB[i].y<100){
			Btl = PointB[i];
		}
		else if (PointB[i].x>300 && PointB[i].y<160){
			Btr = PointB[i];
		}
		else if (PointB[i].x<160 && PointB[i].y>200){
			Bbl = PointB[i];
		}
		else if (PointB[i].x>300 && PointB[i].y>200){
			Bbr = PointB[i];
		}
		else{
			Bc = PointB[i];
		}
	}
	for (int i = 0; i < PointI.size(); i++){
		if (PointI[i].x<160 && PointI[i].y<100){
			Itl = PointI[i];
		}
		else if (PointI[i].x>300 && PointI[i].y<160){
			Itr = PointI[i];
		}
		else if (PointI[i].x<160 && PointI[i].y>200){
			Ibl = PointI[i];
		}
		else if (PointI[i].x>300 && PointI[i].y>200){
			Ibr = PointI[i];
		}
		else{
			Ic = PointI[i];
		}
	}
	//bw角度
	double kb1 = (double)(Btl.y - Btr.y) / (double)(Btl.x - Btr.x);
	double kb2 = (double)(Bbl.y - Bbr.y) / (double)(Bbl.x - Bbr.x);
	double kb3 = -(double)(Btr.x - Bbr.x) / (double)(Btr.y - Bbr.y);
	double kb4 = -(double)(Btl.x - Bbl.x) / (double)(Btl.y - Bbl.y);
	double kB = (kb1 + kb2 + kb3 + kb4) / 4;
	double angleB = atan(kB) / 3.1415926 * 180;
	//lw角度
	double kI1 = (double)(Itl.y - Itr.y) / (double)(Itl.x - Itr.x);
	double kI2 = (double)(Ibl.y - Ibr.y) / (double)(Ibl.x - Ibr.x);
	double kI3 = -(double)(Itr.x - Ibr.x) / (double)(Itr.y - Ibr.y);
	double kI4 = -(double)(Itl.x - Ibl.x) / (double)(Itl.y - Ibl.y);
	double kI = (kI1 + kI2 + kI3 + kI4) / 4;
	double angleI = atan(kI) / 3.1415926 * 180;
	Para.w = angleI - angleB;
	
	//计算uv
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


