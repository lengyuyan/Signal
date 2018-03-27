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
%Date:2017.6.5
%============================================
%Input
% img: input image
% pixel_size: the size of CCD pixel
%--------------------------------------------
%Output
% x_offset, y_offset: horizontal(x) offset and vertical(y) offset
%============================================
*/
OFF_set getOffcenter(Mat img, double pixel_size)
{
	int height = img.rows;
	int width = img.cols;
	int roi_size = 200;
	int center_y = height / 2;
	int center_x = width / 2;
	Mat roi_center(img, Rect(center_x - roi_size, center_y - roi_size, 2 * roi_size + 1, 2 * roi_size + 1));
	OFF_set offset;
	harris Harris;
	//利用harris算法检测角点
	Harris.detect(roi_center);
	vector<Point> Points;
	Harris.getCorners(Points, 0.01);

	//筛选黑色方块右下角的角点
	int points_size = Points.size();
	int n = 10;
	for (int i = 0; i < Points.size(); i++)
	{
		if (Points[i].x - n < 0 || Points[i].x + n > 2 * roi_size || Points[i].y - n < 0 || Points[i].y + n > 2 * roi_size)
		{
			Points[i].x = 0;
			Points[i].y = 0;
			points_size --;
		}
		else
		{
			Mat l_t(roi_center, Rect(Points[i].x - n, Points[i].y - n, n, n));
			Mat r_b(roi_center, Rect(Points[i].x, Points[i].y, n, n));
			//Mat r_t(roi_center, Rect(Points[i].x, Points[i].y - n, n, n));
			//Mat l_b(roi_center, Rect(Points[i].x - n, Points[i].y, n, n));
			double sum_lap = 0; double temp;
			for (int p = 0; p < n; p++)
			{
				for (int q = 0; q < n; q++)
				{
					temp = r_b.at<float>(p, q) - l_t.at<float>(p, q) - 10.0 / 255.0;
					sum_lap = sum_lap + (temp > 0);
				}
			}
			if (sum_lap < 0.8 * n * n)
			{
				Points[i].x = 0;
				Points[i].y = 0;
				points_size --;
			}
		}
	}
	
	//计算图像的角点的平均偏移量
	double x_distance = 0;
	double y_distance = 0;
	for (int i = 0; i < Points.size(); i++)
	{
		x_distance = x_distance + Points[i].x;
		y_distance = y_distance + Points[i].y;
	}
	x_distance = x_distance / points_size;
	y_distance = y_distance / points_size;

	/*
	for (int k = 0; k < Points.size(); k++)
	{
		circle(roi_center, Points[k], 2, cvScalar(255, 0, 0), 2);
	}
	Point center;
	center.x = x_distance;
	center.y = y_distance;
 	circle(roi_center, center, 3, cvScalar(0, 255, 0), 2);
	imshow("角点区域", roi_center);
	waitKey();
	*/
	x_distance = x_distance - roi_size;
	y_distance = y_distance - roi_size;

	offset.x_offset = x_distance * pixel_size;
	offset.y_offset = y_distance * pixel_size;

	return offset;
}