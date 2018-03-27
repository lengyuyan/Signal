#include <opencv2/core/core.hpp> 
#include <opencv2/highgui/highgui.hpp> 
#include <opencv2/imgproc/imgproc.hpp> 
#include <opencv2/features2d/features2d.hpp>
#include <iostream>
#include <math.h>

using namespace std;
using namespace cv;

typedef struct {
	vector<Point> x_edge;
	vector<Point> y_edge;
}HV_Edge; // get horizontal and vertical edge positions

HV_Edge getEdgePosition(Mat img, int block_size, float thr);
/*
%get all edge position
%============================================
%Author:Chame & Ocean(Get the pixel value using pointer)
%Date : 2017.4.10
%============================================
%Input
% img : input image
% block_size : 1 / 2 size of block to determine edge;
% thr: if value > thr, it will be edge;
%--------------------------------------------
%Output
% x_edge:the positions of horizontal edges
% y_edge : the positions of vertical edges
%============================================
*/
HV_Edge getEdgePosition(Mat img, int block_size, float thr)
{
	int height = img.rows;
	int width = img.cols;
	//定义模板
	Mat h_x = Mat::ones(block_size * 2 + 1, block_size * 2 + 1, CV_32F);
	h_x.rowRange(0, block_size - 1) = -1;
	Mat h_y = Mat::ones(block_size * 2 + 1, block_size * 2 + 1, CV_32F);
	h_y.colRange(0, block_size - 1) = -1;
	Mat value_x, value_y;
	Point Pblock_size; Pblock_size.x = block_size; Pblock_size.y = block_size;
	//通过模板做相关运算
	filter2D(img, value_x, img.depth(), h_x, Point(-1, -1), (0, 0), BORDER_REFLECT);
	filter2D(img, value_y, img.depth(), h_y, Point(-1, -1), (0, 0), BORDER_REFLECT);
	//找到相关性大于阈值的的点的位置，并做非极大抑制，确定刃边的位置
	HV_Edge edge;
	for (int j = block_size ; j < height - block_size ; j++)
	{
		float* data_x = value_x.ptr<float>(j);
		float* data_y = value_y.ptr<float>(j);
		for (int i = block_size ; i < width - block_size ; i++)
		{
			if (data_x[i] > thr)
			{
				Mat roi(value_x, Rect(i - block_size, j - block_size, 2 * block_size + 1, 2 * block_size + 1));
				Point maxPoint;
				minMaxLoc(roi, 0, 0, 0, &maxPoint);
				maxPoint.x = maxPoint.x + i - block_size;
				maxPoint.y = maxPoint.y + j - block_size;
				if ( i == maxPoint.x && j == maxPoint.y ){
					edge.x_edge.push_back(maxPoint);
					//rectangle(img, maxPoint - Pblock_size, maxPoint + Pblock_size, cvScalar(255, 0, 0), 2);
				}
				
			}
			if (data_y[i] > thr)
			{
				Mat roi(value_y, Rect(i - block_size, j - block_size, 2 * block_size + 1, 2 * block_size + 1));
				Point maxPoint;
				minMaxLoc(roi, 0, 0, 0, &maxPoint);
				maxPoint.x = maxPoint.x + i - block_size;
				maxPoint.y = maxPoint.y + j - block_size;
				if (i == maxPoint.x && j == maxPoint.y){
					edge.y_edge.push_back(maxPoint);
					//rectangle(img, maxPoint - Pblock_size, maxPoint + Pblock_size, cvScalar(255, 0, 0), 2);
				}
			}
		}
	}
	//cout << edge.x_edge.size()<<"shuliang"<<edge.y_edge.size();
	return edge;
}