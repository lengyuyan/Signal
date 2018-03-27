#include <opencv2\opencv.hpp>
#include <iostream>
#include <math.h>
using namespace cv;
using namespace std;

//定义函数输出结构体
typedef struct{
	vector<double> horizontal_lap;
	vector<double> vertical_lap;
	vector<Point> horizontal_pt;
	vector<Point> vertical_pt;
	double time_Edge;
}HV_Lap;
typedef struct {
	vector<Point> x_edge;
	vector<Point> y_edge;
}HV_Edge; // get horizontal and vertical edge positions

HV_Edge getEdgePosition(Mat img, int block_size, float thr);
double laplace(Mat I, Point Loc, int edge_size);
HV_Lap getAllLaplace(Mat I, int edge_size, int error);
/*
%get all edge laplace
%============================================
%Date:2017.4.10
%Author:Ocean
%============================================
%Input
% I: input image
% edge_size: 1/2 size of block to determine edge;
% error: error of distance of points
%--------------------------------------------
%Output
% horizontal_lap: laplace of horizontal edges;
% vertical_lap: laplace of vertical edges;
% horizontal_pt: locations of horizontal points;
% vertical_pt: locations of vertical points;
%============================================
*/
HV_Lap getAllLaplace(Mat I, int edge_size, int error)
{
	//得到刃边区域位置，计算得到中心刃边位置
	HV_Lap str;
	int block_size = 19;
	float thr = pow((2 * (float)block_size + 1), 2) / 2 * 0.2;
	double time_Edge = static_cast<double>(getTickCount());
	HV_Edge edge = getEdgePosition(I, block_size, thr);
	time_Edge = ((double)getTickCount() - time_Edge) / getTickFrequency(); 
	vector<Point> x_edge = edge.x_edge;
	vector<Point> y_edge = edge.y_edge;
	/*
	//计算center_x坐标
	Point sum_x;  sum_x.x = 0;   sum_x.y = 0;
	for (int i = 0; i < x_edge.size(); i++){
		sum_x = sum_x + x_edge[i];
	}
	Point center_x;
	center_x.x = sum_x.x / x_edge.size();
	center_x.y = sum_x.y / x_edge.size();
	//计算center_y坐标
	Point sum_y;  sum_y.x = 0;   sum_y.y = 0;
	for (int i = 0; i < y_edge.size(); i++){
		sum_y = sum_y + y_edge[i];
	}
	Point center_y;
	center_y.x = sum_y.x / y_edge.size();
	center_y.y = sum_y.y / y_edge.size();
	Point esize; esize.x = edge_size; esize.y = edge_size;
	*/

	Point Loc_center_x, Loc_left, Loc_right, Loc_center_y, Loc_top, Loc_bottom;
	Loc_center_x = x_edge[0];
	Loc_center_y = y_edge[0];
	Loc_right = x_edge[1];
	Loc_left = x_edge[2];
	Loc_top = y_edge[1];
	Loc_bottom = y_edge[2];

	str.horizontal_pt.push_back(Loc_left);
	str.horizontal_pt.push_back(Loc_center_x);
	str.horizontal_pt.push_back(Loc_right);
	str.vertical_pt.push_back(Loc_top);
	str.vertical_pt.push_back(Loc_center_y);
	str.vertical_pt.push_back(Loc_bottom);
	str.time_Edge = time_Edge;
	
	//计算刃边区域的laplace值
	str.horizontal_lap.push_back(laplace(I, Loc_left, edge_size));
	str.horizontal_lap.push_back(laplace(I, Loc_center_x, edge_size));
	str.horizontal_lap.push_back(laplace(I, Loc_right, edge_size));
	str.vertical_lap.push_back(laplace(I, Loc_top, edge_size));
	str.vertical_lap.push_back(laplace(I, Loc_center_y, edge_size));
	str.vertical_lap.push_back(laplace(I, Loc_bottom, edge_size));

	/*
	//画出六个刃边框
	rectangle(I, Loc_center_x - esize, Loc_center_x + esize, cvScalar(255, 0, 0), 2);
	rectangle(I, Loc_bottom - esize, Loc_bottom + esize, cvScalar(255, 0, 0), 2);
	rectangle(I, Loc_top - esize, Loc_top + esize, cvScalar(255, 0, 0), 2);
	rectangle(I, Loc_center_y - esize, Loc_center_y + esize, cvScalar(255, 0, 0), 2);
	rectangle(I, Loc_right - esize, Loc_right + esize, cvScalar(255, 0, 0), 2);
	rectangle(I, Loc_left - esize, Loc_left + esize, cvScalar(255, 0, 0), 2);
	*/
	return str;
}

double laplace(Mat I, Point Loc, int edge_size)
{
	Mat myKernel = (Mat_<float>(3, 3) << -1, -1, -1, -1, 8, -1, -1, -1, -1);
	int x = Loc.x - edge_size;
	int y = Loc.y - edge_size;
	Mat lap_temp = I(cvRect(x, y, 2 * edge_size + 1, 2 * edge_size + 1));
	Mat lap_temp2; double sum_lap = 0;
	filter2D(lap_temp, lap_temp2, lap_temp.depth(), myKernel, Point(-1, -1), (0, 0), BORDER_DEFAULT);
	for (int k = 0; k < lap_temp2.rows; k++){
		float* data = lap_temp2.ptr<float>(k);
		for (int n = 0; n < lap_temp2.cols; n++){
			sum_lap = sum_lap + pow(data[n], 2);
		}
	}
	double lap = sum_lap / pow((2 * edge_size - 1), 2);
	return lap;
}
/*
double laplace(Mat I, Point Loc, int edge_size)
{
	double sum_lap = 0; double temp;
	for (int i = Loc.y - edge_size + 1; i < Loc.y + edge_size; i++)
	{
		for (int j = Loc.x - edge_size + 1; j < Loc.x + edge_size; j++)
		{
			temp = 8 * I.at<float>(i, j) - I.at<float>(i - 1, j - 1) - I.at<float>(i - 1, j) - I.at<float>(i - 1, j + 1)- I.at<float>(i, j - 1) - I.at<float>(i, j + 1) - I.at<float>(i + 1, j - 1) - I.at<float>(i + 1, j) - I.at<float>(i + 1, j + 1);
			temp = pow(temp, 2);
			sum_lap = sum_lap + temp;
		}
	}
	double lap = sum_lap / pow((2 * edge_size - 1), 2);
	return lap;
}
*/