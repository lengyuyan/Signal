#include <opencv2\opencv.hpp>
#include <opencv2\highgui\highgui.hpp>
#include "harris.h"

using namespace cv;
using namespace std;

typedef struct {
	double x_offset;
	double y_offset;
}OFF_set;

/*
%get off-center distance
%============================================
%Author:Chame
%Date:2017.4.10
%============================================
%Input
% img: input image
% standard_img: standard image;
% pixel_size: the size of CCD pixel
%--------------------------------------------
%Output
% x_offset, y_offset: horizontal(x) offset and vertical(y) offset
%============================================
*/
OFF_set getOffcenter(Mat img, Mat standard_ing, double pixel_size)
{
	OFF_set offset;
	harris Harris1, Harris2;
	//利用harris算法检测角点
	Harris1.detect(standard_ing);
	vector<Point> fixedPoints;
	Harris1.getCorners(fixedPoints, 0.01);

	Harris2.detect(img);
	vector<Point> movingPoints;
	Harris2.getCorners(movingPoints, 0.01);
	//计算图像与标准图的角点的平均偏移量
	double x_distance = 0;
    double y_distance = 0;
	for (int i = 0; i < fixedPoints.size(); i++)
	{
		x_distance = x_distance + movingPoints[i].x - fixedPoints[i].x;
		y_distance = y_distance + movingPoints[i].y - fixedPoints[i].y;
	}
	offset.x_offset = x_distance / fixedPoints.size();
	offset.y_offset = y_distance / fixedPoints.size();

	return offset;
}