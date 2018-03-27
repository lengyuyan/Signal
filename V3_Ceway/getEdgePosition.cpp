#include <opencv2\opencv.hpp>
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

	double f = 3.31;
	double scale = 0.2;
	double pixel_size = 0.00112 / scale;
	double d_obj = 600;
	double d_img = 1 / (1 / f - 1 / d_obj);
	double beta = d_img / d_obj;
	double size = 90, block = 140;
	//center
	double i_center = 4208 / 2 * scale, j_center = 3120 / 2 * scale;
	Mat I_center = img(cvRect(i_center - size, j_center - size, block, block));
	//left
	double i_left = ceil(i_center - 280 * beta / pixel_size);
	//Mat I_left = img(cvRect(i_left - size, j_center - size, block, block));
	//right
	double i_right = ceil(i_center + 280 * beta / pixel_size);
	//Mat I_right = img(cvRect(i_right - size, j_center - size, block, block));
	//Top
	double j_top = ceil(j_center - 200 * beta / pixel_size);
	//Mat I_top = img(cvRect(i_center - size, j_top - size, block, block));
	//Bottom
	double j_bottom = ceil(j_center + 200 * beta / pixel_size);
	//Mat I_bottom = img(cvRect(i_center - size, j_bottom - size, block, block));


	//定义模板
	Mat h_x = Mat::ones(block_size * 2 + 1, block_size * 2 + 1, CV_32F);
	h_x.rowRange(0, block_size - 1) = -1;
	Mat h_y = Mat::ones(block_size * 2 + 1, block_size * 2 + 1, CV_32F);
	h_y.colRange(0, block_size - 1) = -1;

	//Center刃边选取与计算
	Mat value_x, value_y;
	filter2D(img(cvRect(i_center - size, j_center - size, block, block)), value_x, I_center.depth(), h_x, Point(-1, -1), (0, 0), BORDER_REFLECT);
	filter2D(img(cvRect(i_center - size, j_center - size, block, block)), value_y, I_center.depth(), h_y, Point(-1, -1), (0, 0), BORDER_REFLECT);
	//找到相关性大于阈值的的点的位置，并做非极大抑制，确定刃边的位置
	HV_Edge edge; 
	//Point Pblock_size; Pblock_size.x = 15; Pblock_size.y = 15;
	for (int j = block_size; j < block - block_size; j++)
	{
		float* data_x = value_x.ptr<float>(j);
		float* data_y = value_y.ptr<float>(j);
		for (int i = block_size ; i < block - block_size ; i++)
		{
			if (data_x[i] > thr)
			{
				Mat roi(value_x, Rect(i - block_size, j - block_size, 2 * block_size + 1, 2 * block_size + 1));
				Point maxPoint,maxPoint_Img;
				minMaxLoc(roi, 0, 0, 0, &maxPoint);
				maxPoint.x = maxPoint.x + i - block_size;
				maxPoint.y = maxPoint.y + j - block_size;
				if ( i == maxPoint.x && j == maxPoint.y ){
					maxPoint_Img.x = i + i_center - size;
					maxPoint_Img.y = j + j_center - size;
					edge.x_edge.push_back(maxPoint_Img);
					//rectangle(img, maxPoint_Img - Pblock_size, maxPoint_Img + Pblock_size, cvScalar(255, 0, 0), 2);
				}
				
			}
			if (data_y[i] > thr)
			{
				Mat roi(value_y, Rect(i - block_size, j - block_size, 2 * block_size + 1, 2 * block_size + 1));
				Point maxPoint, maxPoint_Img;
				minMaxLoc(roi, 0, 0, 0, &maxPoint);
				maxPoint.x = maxPoint.x + i - block_size;
				maxPoint.y = maxPoint.y + j - block_size;
				if (i == maxPoint.x && j == maxPoint.y){
					maxPoint_Img.x = i + i_center - size;
					maxPoint_Img.y = j + j_center - size;
					edge.y_edge.push_back(maxPoint_Img);
					//rectangle(img, maxPoint_Img - Pblock_size, maxPoint_Img + Pblock_size, cvScalar(255, 0, 0), 2);
				}
			}
		}
	}
	//Right刃边选取
	filter2D(img(cvRect(i_right - size, j_center - size, block, block)), value_x, I_center.depth(), h_x, Point(-1, -1), (0, 0), BORDER_REFLECT);
	for (int j = block_size; j < block - block_size; j++)
	{
		float* data_x = value_x.ptr<float>(j);
		for (int i = block_size; i < block - block_size; i++)
		{
			if (data_x[i] > thr)
			{
				Mat roi(value_x, Rect(i - block_size, j - block_size, 2 * block_size + 1, 2 * block_size + 1));
				Point maxPoint, maxPoint_Img;
				minMaxLoc(roi, 0, 0, 0, &maxPoint);
				maxPoint.x = maxPoint.x + i - block_size;
				maxPoint.y = maxPoint.y + j - block_size;
				if (i == maxPoint.x && j == maxPoint.y){
					maxPoint_Img.x = i + i_right - size;
					maxPoint_Img.y = j + j_center - size;
					edge.x_edge.push_back(maxPoint_Img);
					//rectangle(img, maxPoint_Img - Pblock_size, maxPoint_Img + Pblock_size, cvScalar(255, 0, 0), 2);
				}

			}
		}
	}
	//Left刃边选取
	filter2D(img(cvRect(i_left -size, j_center - size, block, block)), value_x, I_center.depth(), h_x, Point(-1, -1), (0, 0), BORDER_REFLECT);
	for (int j = block_size; j < block - block_size; j++)
	{
		float* data_x = value_x.ptr<float>(j);
		for (int i = block_size; i < block - block_size; i++)
		{
			if (data_x[i] > thr)
			{
				Mat roi(value_x, Rect(i - block_size, j - block_size, 2 * block_size + 1, 2 * block_size + 1));
				Point maxPoint, maxPoint_Img;
				minMaxLoc(roi, 0, 0, 0, &maxPoint);
				maxPoint.x = maxPoint.x + i - block_size;
				maxPoint.y = maxPoint.y + j - block_size;
				if (i == maxPoint.x && j == maxPoint.y){
					maxPoint_Img.x = i + i_left - size;
					maxPoint_Img.y = j + j_center - size;
					edge.x_edge.push_back(maxPoint_Img);
					//rectangle(img, maxPoint_Img - Pblock_size, maxPoint_Img + Pblock_size, cvScalar(255, 0, 0), 2);
				}

			}
		}
	}
	//Top刃边选取
	filter2D(img(cvRect(i_center - size, j_top - size, block, block)), value_y, I_center.depth(), h_y, Point(-1, -1), (0, 0), BORDER_REFLECT);
	for (int j = block_size; j < block - block_size; j++)
	{
		float* data_y = value_y.ptr<float>(j);
		for (int i = block_size; i < block - block_size; i++)
		{
			if (data_y[i] > thr)
			{
				Mat roi(value_y, Rect(i - block_size, j - block_size, 2 * block_size + 1, 2 * block_size + 1));
				Point maxPoint, maxPoint_Img;
				minMaxLoc(roi, 0, 0, 0, &maxPoint);
				maxPoint.x = maxPoint.x + i - block_size;
				maxPoint.y = maxPoint.y + j - block_size;
				if (i == maxPoint.x && j == maxPoint.y){
					maxPoint_Img.x = i + i_center - size;
					maxPoint_Img.y = j + j_top - size;
					edge.y_edge.push_back(maxPoint_Img);
					//rectangle(img, maxPoint_Img - Pblock_size, maxPoint_Img + Pblock_size, cvScalar(255, 0, 0), 2);
				}

			}
		}
	}
	//Bottom刃边选取
	filter2D(img(cvRect(i_center - size, j_bottom - size, block, block)), value_y, I_center.depth(), h_y, Point(-1, -1), (0, 0), BORDER_REFLECT);
	for (int j = block_size; j < block - block_size; j++)
	{
		float* data_y = value_y.ptr<float>(j);
		for (int i = block_size; i < block - block_size; i++)
		{
			if (data_y[i] > thr)
			{
				Mat roi(value_y, Rect(i - block_size, j - block_size, 2 * block_size + 1, 2 * block_size + 1));
				Point maxPoint, maxPoint_Img;
				minMaxLoc(roi, 0, 0, 0, &maxPoint);
				maxPoint.x = maxPoint.x + i - block_size;
				maxPoint.y = maxPoint.y + j - block_size;
				if (i == maxPoint.x && j == maxPoint.y){
					maxPoint_Img.x = i + i_center - size;
					maxPoint_Img.y = j + j_bottom - size;
					edge.y_edge.push_back(maxPoint_Img);
					//rectangle(img, maxPoint_Img - Pblock_size, maxPoint_Img + Pblock_size, cvScalar(255, 0, 0), 2);
				}

			}
		}
	}
	//cout << edge.x_edge.size()<<"shuliang"<<edge.y_edge.size();
	return edge;
}