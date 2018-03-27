#include <opencv2\opencv.hpp>
#include <iostream>
#include <math.h>
#include "AA_define_tool.h"

#include "AAalgorithm.h"
#include<tCHAR.H>

using namespace std;
using namespace cv;

extern  AA_Globle_Param g_AAParam;//引入全局变量

void SelectTemplateType(int temPlate,int block_size, Mat& h_x,Mat& h_y)
{
	
	switch(temPlate)
	{
	case AA_FIRST:			
		h_x.rowRange( block_size + 1,2*block_size+1) = -1;//上白下黑	
		h_y.colRange( block_size + 1,2*block_size+1) = -1;//左白右黑	
		break;
	case AA_SECOND:
		h_x.rowRange( block_size + 1,2*block_size+1) = -1;//上白下黑
		h_y.colRange(0, block_size + 1) = -1;//左黑右白
		break;
	case AA_THIRD:
		h_x.rowRange(0, block_size+1 ) = -1;//上黑下白
		h_y.colRange( block_size + 1,2*block_size+1) = -1;//左白右黑
		break;
	case AA_FOUTH:
		h_x.rowRange(0, block_size+1 ) = -1;//上黑下白
		h_y.colRange(0, block_size + 1) = -1;//左黑右白
		break;
	default://默认右下 
		h_x.rowRange(0, block_size+1 ) = -1;//上黑下白
		h_y.colRange(0, block_size + 1) = -1;//左黑右白
		break;
	}
}

void getFilterValue(int temPlatetype,Mat &img,Mat& temPlate, int Rectx,int Recty,int &size,int &block,Mat &value,int dir)
{
	int x =0;
	int y = 0;

	//temPlatetype = 3;//还是以中心向左上为矩形定点吧

	switch(temPlatetype)
	{
	case AA_FIRST:
		//滤波前先判断是否超出范围
		 x = Rectx - block + size; //以小黑快的第一个点为中心,来选取大选框和小选框
		 y = Recty- block + size;
		isRectOverPicture(img.cols,img.rows,x,y,size,block,temPlatetype);
		if (x==0 || y == 0)
		{
			size = Rectx<Recty?Rectx:Recty;
		}
		filter2D(img(cvRect(x, y, block, block)), value, img.depth(), temPlate, Point(-1, -1), (0, 0), BORDER_REFLECT);


		break;
	case AA_SECOND:
		 x = Rectx - size;
		 y = Recty- block + size;
		isRectOverPicture(img.cols,img.rows,x,y,size,block,temPlatetype);
		if (x==0 || y == 0)
		{
			size = Rectx<Recty?Rectx:Recty;
		}
		filter2D(img(cvRect(x, y, block, block)), value, img.depth(), temPlate, Point(-1, -1), (0, 0), BORDER_REFLECT);
		break;
	case AA_THIRD:
		x = Rectx - block + size;
		 y = Recty- size;
		isRectOverPicture(img.cols,img.rows,x,y,size,block,temPlatetype);
		if (x==0 || y == 0)
		{
			size = Rectx<Recty?Rectx:Recty;
		}
		filter2D(img(cvRect(x, y, block, block)), value, img.depth(), temPlate, Point(-1, -1), (0, 0), BORDER_REFLECT);
		break;
	case AA_FOUTH:
	default:

		 x = Rectx -  size;
		y = Recty- size;
		isRectOverPicture(img.cols,img.rows,x,y,size,block,temPlatetype);
		if (x==0 || y == 0)
		{
			size = Rectx<Recty?Rectx:Recty;
		}
		filter2D(img(cvRect(x,y, block, block)), value, img.depth(), temPlate, Point(-1, -1), (0, 0), BORDER_REFLECT);
		break;
/*	default:
		 x = Rectx - size;
		y = Recty- block + size;
		isRectOverPicture(img.cols,img.rows,x,y,size,block,temPlatetype);
		if (x==0 || y == 0)
		{
			size = Rectx<Recty?Rectx:Recty;
		}
		filter2D(img(cvRect(x, y, block, block)), value, img.depth(), temPlate, Point(-1, -1), (0, 0), BORDER_REFLECT);
		break;*/
	}

	//保存图片
	string picname;
	string picnamefilter;
	switch(dir)
	{
	case CENTERX:
		picname = _T("中心小图X.jpg");
		picnamefilter = _T("x方向中心滤波图.jpg");

		break;
	case CENTERY:
		picname = _T("中心小图Y.jpg");
		picnamefilter = _T("y方向中心滤波图.jpg");
		break;

	case LEFT:
		picname = _T("左边小图.jpg");
		picnamefilter = _T("左边小图滤波图.jpg");
		break;
	case RIGHT:
		picname = _T("右边小图.jpg");
		picnamefilter = _T("右边小图滤波图.jpg");
		break;
	case TOP:
		picname = _T("顶边小图.jpg");
		picnamefilter = _T("顶边小图滤波图.jpg");
		break;
	case BOTTOM:
		picname = _T("底边小图.jpg");
		picnamefilter = _T("底边小图滤波图.jpg");
		break;
	default:
		break;

	}
	savePicture(img(cvRect(x , y , block, block)),picname);
	savePicture(value,picnamefilter);

}

void getFilterAreaMaxValue(int temPlatetype,Point& max_value, int Rectx,int Recty,int &size,int &block,Mat &value,int dir)//获取过滤部分的最大值
{
	int x =0;
	int y = 0;		
	minMaxLoc(value, 0, 0, 0, &max_value);
	if (block <= size )
	{
		size = block;
	}
	switch(temPlatetype)
	{
	case AA_FIRST:
		//滤波前先判断是否超出范围
		x = Rectx - block + size; //以小黑快的第一个点为中心,来选取大选框和小选框
		y = Recty- block + size;

		break;
	case AA_SECOND:
		x = Rectx - size;
		y = Recty- block + size;

		break;
	case AA_THIRD:
		x = Rectx - block + size;
		y = Recty- size;
		break;
	case AA_FOUTH:

		x = Rectx -  size;
		y = Recty- size;
		break;
	default:
		x = Rectx -  size;
		y = Recty- size;
		break;
	}

	max_value.x +=x;
	max_value.y +=y;

}
bool isRectOverPicture(int piclength,int picwidth,int& leftx,int& lefty,int &size,int &block,int temPlate)
{
	bool flag = true;
	int block1 = block;
	int size1 = size;
	if (leftx<0)
	{

		leftx = 0;

	}		
	if (lefty<0)
	{
		lefty = 0;
	}
	if (leftx + block >piclength )
	{
		block1 = piclength -leftx;
	}
	if (lefty + block >picwidth )
	{
		block = picwidth -lefty;
	}
	block = block<block1?block:block1;

	return flag;
}
/*
%get all edge laplace
%============================================
%Author:Chame & Ocean(Get the pixel value using pointer)
%Date : 2017.6.5
%============================================
%Input
% img : input image
% block_size : 1 / 2 size of block to determine edge;
% thr: if value > thr, it will be edge;
% edge_size: 1/2 size of block to determine edge;
% f: the focal length of the lens;
% scale: the image scale's scale;
% pixel_size: the size of CCD pixel;
% d_obj: the object distance of the target;
% size: to determine the image window;
% block: to determine the image window;
%--------------------------------------------
%Output
% horizontal_lap: laplace of horizontal edges;
% vertical_lap: laplace of vertical edges;
% horizontal_pt: locations of horizontal points;
% vertical_pt: locations of vertical points;
%============================================
*/
HV_Lap getEdgeLaplace(Mat img,int cameraID,int * Laplace_Flag)
{
	/*double f = 3.31;                      //镜头焦距为3.31mm
	double d_obj = 500;					  //物距为600mm
	double size = 90, block = 140;        //选框窗口左上点在基准点左上90像素处，block长宽为140像素
	int edge_size = 15;                   //刃边框尺寸长宽为15像素
	int block_size = 19;                  //极大值抑制判断刃边位置尺寸长宽为19像素
	float thr = pow((2 * (float)block_size + 1), 2) / 2 * 0.2;   //极大值抑制判断抑制
	*/
	//msg("dd:%d",1);
	double f= g_AAParam.focus;
	double d_obj = g_AAParam.objdis;
	int size =g_AAParam.size;
	int block = g_AAParam.block;
	int edge_size = g_AAParam.edge_size;
	int block_size = g_AAParam.block_size;
	double thr = pow((2 * (float)block_size + 1), 2) / 2 * 0.2;
	double scale = g_AAParam.scale;	
	double d_img = 1 / (1 / g_AAParam.focus - 1 / g_AAParam.objdis);
	double beta = d_img / g_AAParam.objdis; 
	double pixel_size = g_AAParam.pixel_size/g_AAParam.scale;
	//定义五个选框基准位置
	vector<Point> x_edge, y_edge;
	double i_center = g_AAParam.pic_lenth / 2 * scale, j_center = g_AAParam.pic_width / 2 * scale;

	//画图中心不一定在中央,解决无法对准靶图中心问题，注意
	i_center = g_AAParam.paintCenter[cameraID].X*scale,j_center = g_AAParam.paintCenter[cameraID].Y  * scale;

	double i_left = ceil(i_center - g_AAParam.mapdislength * beta / pixel_size); //可能每个图像顺序不一样
	double i_right = ceil(i_center + g_AAParam.mapdislength * beta / pixel_size);
	double j_top = ceil(j_center - g_AAParam.mapdiswidth * beta / pixel_size);
	double  j_bottom = ceil(j_center + g_AAParam.mapdiswidth * beta / pixel_size);


	int temPlate = g_AAParam.temPlate[cameraID];

	//定义模板:(原则:选择的中心为：0,左上点(上白下黑,左白右黑),1,右上点(上白下黑,左黑右白)，
	//2,左下点(上黑下白,左白右黑),3,右下点(上黑下白,左白右黑)

	Mat h_x = Mat::ones(block_size * 2 + 1, block_size * 2 + 1, CV_32F);
	Mat h_y = Mat::ones(block_size * 2 + 1, block_size * 2 + 1, CV_32F);

	SelectTemplateType(temPlate,block_size,  h_x,h_y);//选择模板类型

	//是否模板反了:左右模板 取水平点，上下模板取垂直点，还是没有影响
	Mat temp;
	temp = h_x;
	h_x = h_y;
	h_y = temp;
	savePicture(h_x,"x方向模板.jpg");
	savePicture(h_y,"y方向模板.jpg");
	//flip(img, img, 0);//镜像需不需要
	//Center刃边选取与计算
	Mat value_x, value_y;
	getFilterValue(temPlate,img,h_x,i_center,j_center,size,block,value_x,CENTERX);
	getFilterValue(temPlate,img,h_y,i_center,j_center,size,block,value_y,CENTERY);

//	filter2D(img(cvRect(i_center - size, j_center - size, block, block)), value_x, img.depth(), h_x, Point(-1, -1), (0, 0), BORDER_REFLECT);
//	filter2D(img(cvRect(i_center - size, j_center - size, block, block)), value_y, img.depth(), h_y, Point(-1, -1), (0, 0), BORDER_REFLECT);
	
	//savePicture(img(cvRect(i_center-size , j_center-size , block, block)),"中心小图.jpg");
	//savePicture(value_x,"x方向中心滤波图.jpg");
	//savePicture(value_y,"y方向中心滤波图.jpg");

	//找到相关性大于阈值的的点的位置，并做非极大抑制，确定刃边的位置

	int x_center_flag=0;
	int y_center_flag=0;
	///*****************************************************************************************
	//找到value_x中最大值点的位置，以及value_x去边之后的最大值的点

	Point max_value_x_center;
	/*double maxalue,minalue;
	minMaxLoc(value_x, &minalue, &maxalue, 0, &max_value_x_center);
	max_value_x_center.x=max_value_x_center.x+i_center - size;
	max_value_x_center.y=max_value_x_center.y+j_center - size;
	*/
	getFilterAreaMaxValue(temPlate,max_value_x_center,i_center,j_center,size,block,value_x,CENTERX);

	///*****************************************************************************************
	Point max_value_y_center;
	/*minMaxLoc(value_y, 0, 0, 0, &max_value_y_center);
	max_value_y_center.x=max_value_y_center.x+i_center - size;
	max_value_y_center.y=max_value_y_center.y+j_center - size;
	*/
	getFilterAreaMaxValue(temPlate,max_value_y_center,i_center,j_center,size,block,value_y,CENTERY);

	///*****************************************************************************************

	///*****************************************************************************************

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
					x_edge.push_back(maxPoint_Img);
					x_center_flag=1;

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
					y_edge.push_back(maxPoint_Img);
					y_center_flag=1;
				}
			}
		}
	}
	
	if(x_center_flag==0)
	{
		x_edge.push_back(max_value_x_center);
	}
	if(y_center_flag==0)
	{
		y_edge.push_back(max_value_y_center);
	}

	//Right刃边选取
	
	size =g_AAParam.size;
	block = g_AAParam.block;


	getFilterValue(temPlate,img,h_x,i_right,j_center,size,block,value_x,RIGHT);
	//filter2D(img(cvRect(i_right - size, j_center - size, block, block)), value_x, img.depth(), h_x, Point(-1, -1), (0, 0), BORDER_REFLECT);
	int x_right_flag=0;
	Point max_value_x_right;
	/*minMaxLoc(value_x, 0, 0, 0, &max_value_x_right);
	max_value_x_right.x=max_value_x_right.x+i_right - size;
	max_value_x_right.y=max_value_x_right.y+j_center - size;*/

	getFilterAreaMaxValue(temPlate,max_value_x_right,i_right,j_center,size,block,value_x,RIGHT);
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
					x_edge.push_back(maxPoint_Img);
					x_right_flag=1;
				}

			}
		}
	}
	if(x_right_flag==0)
	{
		x_edge.push_back(max_value_x_right);
	}

	//savePicture(img(cvRect(i_right-size , j_center-size , block, block)),"右边小图.jpg");
	//savePicture(value_x,"x方向右边滤波图.jpg");

	//Left刃边选取
	
	size =g_AAParam.size;
	block = g_AAParam.block;


	getFilterValue(temPlate,img,h_x,i_left,j_center,size,block,value_x,LEFT);
	//filter2D(img(cvRect(i_left - size, j_center - size, block, block)), value_x, img.depth(), h_x, Point(-1, -1), (0, 0), BORDER_REFLECT);
	///*******************************************************************************************************************
	int x_left_flag=0;
	Point max_value_x_left;

	/*minMaxLoc(value_x, 0, 0, 0, &max_value_x_left);
	max_value_x_left.x=max_value_x_left.x+i_left - size;
	max_value_x_left.y=max_value_x_left.y+j_center - size;*/
	getFilterAreaMaxValue(temPlate,max_value_x_left,i_left,j_center,size,block,value_x,LEFT);
	///********************************************************************************************************************

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
					x_edge.push_back(maxPoint_Img);
					x_left_flag=1;
				}
			}
		}
	}
	if(x_left_flag==0)
	{
		x_edge.push_back(max_value_x_left);
	}

	//savePicture(img(cvRect(i_left-size , j_center-size , block, block)),"左边小图.jpg");
	//savePicture(value_x,"x方向左边滤波图.jpg");
	//Top刃边选取
	int y_top_flag=0;

	size =g_AAParam.size;
	block = g_AAParam.block;


	getFilterValue(temPlate,img,h_y,i_center,j_top,size,block,value_y,TOP);
	//filter2D(img(cvRect(i_center - size, j_top - size, block, block)), value_y, img.depth(), h_y, Point(-1, -1), (0, 0), BORDER_REFLECT);
	
	///*******************************************************************************************************************
//	int x_left_flag=0;
	Point max_value_y_top;
	/*minMaxLoc(value_y, 0, 0, 0, &max_value_y_top);
	max_value_y_top.x=max_value_y_top.x+i_center - size;
	max_value_y_top.y=max_value_y_top.y+j_top - size;
	*/

	getFilterAreaMaxValue(temPlate,max_value_y_top,i_center,j_top,size,block,value_y,TOP);
	///********************************************************************************************************************

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
					y_top_flag=1;
					y_edge.push_back(maxPoint_Img);
				}
			}
		}
	}
	if(y_top_flag==0)
	{
		y_edge.push_back(max_value_y_top);
	}

	//savePicture(img(cvRect(i_center-size , j_top-size , block, block)),"顶边小图.jpg");
	//savePicture(value_y,"x方向顶边滤波图.jpg");
	
	//Bottom刃边选取
	int y_bottom_flag=0;
	size =g_AAParam.size;
	block = g_AAParam.block;
	getFilterValue(temPlate,img,h_y,i_center,j_bottom,size,block,value_y,BOTTOM);
	//filter2D(img(cvRect(i_center - size, j_bottom - size, block, block)), value_y, img.depth(), h_y, Point(-1, -1), (0, 0), BORDER_REFLECT);
	Point max_value_y_bottom;
	/*minMaxLoc(value_y, 0, 0, 0, &max_value_y_bottom);
	max_value_y_bottom.x=max_value_y_bottom.x+i_center - size;
	max_value_y_bottom.y=max_value_y_bottom.y+ j_bottom- size;*/

	getFilterAreaMaxValue(temPlate,max_value_y_bottom,i_center,j_bottom,size,block,value_y,BOTTOM);
	//max_value_y_bottom.y = 610;
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
					y_bottom_flag=1;
					y_edge.push_back(maxPoint_Img);
				}
			}
		}
	}
	if(y_bottom_flag==0)
	{
		y_edge.push_back(max_value_y_bottom);
	}

	//savePicture(img(cvRect(i_center-size , j_bottom-size , block, block)),"底边小图.jpg");
	//savePicture(value_y,"x方向底边滤波图.jpg");

	//判断选框是否准确
	if (x_edge.size() != 3 || y_edge.size() != 3){
		cout << "Error in getting edge!" << endl;
		* Laplace_Flag=-1;
		HV_Lap str1;
		return str1;
	}
	Mat imgpaint = img.clone();
	


	for (int a = 0;a<x_edge.size();a++)
	{

		circle(imgpaint, x_edge[a], 10, cvScalar(255, 0, 0), 2);
	}
	for (int a = 0;a<y_edge.size();a++)
	{
		circle(imgpaint, y_edge[a], 10, cvScalar(255, 0, 0), 2);
	}
	
	//保存图片不改变原来的图像
	string filename = "刃边点_";
	static int index = 1;
	char buf[64] ={0};
	sprintf(buf,"%d",index++);

	if (index > 3)
	{
		index = 1;
	}
	filename.append(buf);
	filename.append(".jpg") ;
	savePicture(imgpaint,filename);
	//showPicture(imgpaint,"刃边点");
	imgpaint.release();
	
	HV_Lap str;
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
	//cout<<img<<endl;
	//计算刃边区域的laplace值
	str.horizontal_lap.push_back(laplace(img, Loc_left, edge_size));
	str.horizontal_lap.push_back(laplace(img, Loc_center_x, edge_size));
	str.horizontal_lap.push_back(laplace(img, Loc_right, edge_size));
	str.vertical_lap.push_back(laplace(img, Loc_top, edge_size));
	str.vertical_lap.push_back(laplace(img, Loc_center_y, edge_size));
	str.vertical_lap.push_back(laplace(img, Loc_bottom, edge_size));


	* Laplace_Flag=0;
	return str;
}

int savePicture(Mat &img, string filename)
{
	///暂时认为输入img都为cvflaot_32
	Mat I = img.clone();
	I = I*255;  
	I.convertTo(I, CV_8U);
	filename="image\\"+filename;
	imwrite(filename,I);
	I.release();
	return 0;

}
int showPicture(Mat &img, const string windowname)
{
	cvNamedWindow(windowname.c_str(),WINDOW_NORMAL);
	imshow(windowname.c_str(),img);
	waitKey();
	cvDestroyWindow(windowname.c_str());
	return 0;
}



double laplace(Mat I, Point Loc, int edge_size)
{
	
	Mat myKernel = (Mat_<float>(3, 3) << -1, -1, -1, -1, 8, -1, -1, -1, -1);
	int x = Loc.x - edge_size;
	int y = Loc.y - edge_size;

	if (x<0 || y<0)
	{
		return -1;
	}

	if (y+ 2 * edge_size + 1 >= I.rows ||  x+ 2 * edge_size + 1 >= I.cols)//新增参数检测
	{
		return -1;
	}
	 Mat lap_temp = I(cvRect(x, y, 2 * edge_size + 1, 2 * edge_size + 1));

	//cout<<lap_temp<<endl;
	Mat lap_temp2; double sum_lap = 0;
	filter2D(lap_temp, lap_temp2, lap_temp.depth(), myKernel, Point(-1, -1), (0, 0), BORDER_DEFAULT);
	for (int k = 0; k < lap_temp2.rows; k++){
		float* data = lap_temp2.ptr<float>(k);
		for (int n = 0; n < lap_temp2.cols; n++){
			sum_lap = sum_lap + pow(data[n], 2);
		}
	}
	//cout<<I<<endl;
	//cout<<lap_temp<<endl;
//	showPicture(lap_temp,"la1");
//	showPicture(lap_temp2,"la2");

	savePicture(lap_temp,"laplace前.jpg");

	savePicture(lap_temp2,"laplace后.jpg");
	double lap = sum_lap / pow(double(2 * edge_size - 1), int(2));
	return lap;
}