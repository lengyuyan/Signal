#include <iostream>
#include <opencv2\opencv.hpp>
#include "harris.h"
#include "math.h"

using namespace cv;
using namespace std;

typedef struct {
	vector<Point> PointB, PointI;
}Points;

/*
%getCorner()
%get 5 corners of the image
%============================================
%Author:Ocean
%Date:2017.6.6
%============================================
%Nargin Set
% I:The unfixed image;
% B:The fixed image;
%--------------------------------------------
%Output
% Points of I and B;
%============================================
*/

Points getCorner(Mat B, Mat I){
	//Harris¼ì²â½Çµã
	Points Points;
	harris HarrisB, HarrisI;
	vector<Point> TempB, TempI, PointB, PointI;
	HarrisB.detect(B);
	HarrisI.detect(I);
	HarrisB.getCorners(TempB, 0.01);
	HarrisI.getCorners(TempI, 0.01);
	//É¸Ñ¡ÓÒÏÂ½Ç½Çµã
	for (int i = 0; i < TempB.size(); i++){
		if (B.at<float>(TempB[i].y - 10, TempB[i].x) < 0.15 ){
			PointB.push_back(TempB[i]);
			//cout <<TempB[i] << endl;
		}
	}
	//&& B.at<float>(TempB[i].y, TempB[i].x - 10) > 0.5 && B.at<float>(TempB[i].y, TempB[i].x + 10) > 0.5
	for (int i = 0; i < TempI.size(); i++){
		if (I.at<float>(TempI[i].y - 10, TempI[i].x) < 0.15){
			PointI.push_back(TempI[i]);
			//cout <<TempI[i] << endl;
		}
	}
	if (PointB.size() != 5 || PointI.size() != 5){
		cout << "Default in finding points£¡" << endl;
		cout << "Default in finding points£¡" << endl;
		cout << "Default in finding points£¡" << endl;
	}
	/*
	Point P; P.x = 2; P.y = 2;
	for (int i = 0; i < PointI.size(); i++){
	rectangle(B, PointB[i], PointB[i] + P, cvScalar(255, 0, 0), 2);
	}
	imshow("123", B);
	waitKey(0);
	system("pause");
	*/
	Points.PointB = PointB;
	Points.PointI = PointI;
	return Points;
}