#include <opencv2/core/core.hpp> 
#include <opencv2/highgui/highgui.hpp> 
#include "polyfit.h"
#include <iostream>
#include "AA_define_tool.h"

#include "AAalgorithm.h"

using namespace std;
using namespace cv;
using namespace czy;

extern  AA_Globle_Param g_AAParam;//引入全局变量



/*
%get the current state of the initial image including u, v and z
%============================================
%Author:Chame & Ocean(Using polyfit.h and vector)
%Date:2017.6.5
%============================================
%Input
% ima_ori: the original image in the initial position;
% ima_p: the positive image in the initial position;
% ima_m: the minus image in the initial position;
% deltaZ_p: the distance of image moving to the plus direction;
% deltaZ_m: the distance of image moving to the minus direction;
% pixel_size: the size of CCD pixel;
% scale: the image scale's scale;
%--------------------------------------------
%Output
% deltaZ: the distance of the initial image to the focal plane;
% x_offset, y_offset: horizontal(x) offset and vertical(y) offset;
% x_angle, y_angle: horizontal(x) angle and vertical(y) angle;
%============================================
*/

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
string ConvertToString(double value)
{
	stringstream ss;
	ss << value;
	return ss.str();
}

int calZUV(Mat img_ori, Mat img_p, Mat img_m, double deltaZ_p, double deltaZ_m,int vid,State &result)
{	
	//1,初始化结果
	int ret = 0;
	HV_Lap hv_lap_o,hv_lap_p,hv_lap_m;
	memset(&result,0,sizeof(State));
	memset(&hv_lap_o,0,sizeof(HV_Lap));
	memset(&hv_lap_o,0,sizeof(HV_Lap));
	memset(&hv_lap_o,0,sizeof(HV_Lap));

	//2，计算每张图片
	ret = calHarryPointsAndMTFValueInOnePic(img_ori,vid,hv_lap_o);
	if (0 != ret)
	{
		msg("获取vid:%d MTF失败!,ret = %d",vid,ret);
		return ret;
	}
	ret = calHarryPointsAndMTFValueInOnePic(img_p,vid,hv_lap_p);
	if (0 != ret)
	{
		msg("获取vid:%d MTF失败!,ret = %d",vid,ret);
		return ret;
	}
	ret = calHarryPointsAndMTFValueInOnePic(img_m,vid,hv_lap_m);
	if (0 != ret)
	{
		msg("获取vid:%d MTF失败!,ret = %d",vid,ret);
		return ret;
	}

	//3，计算
	//存放在向量中的顺序为:左边-中间-右边-  上边 -中间--下边
	//原点位置
	vector<Point> x_pt_o = hv_lap_o.horizontal_pt;//水平3个点
	vector<Point> y_pt_o = hv_lap_o.vertical_pt;//垂直3个点

	vector<double> x_lap_o = hv_lap_o.horizontal_lap;//水平3个拉普拉斯
	vector<double> y_lap_o = hv_lap_o.vertical_lap;//垂直3个拉普拉斯

	//下面位置
	vector<double> x_lap_p  = hv_lap_p.horizontal_lap;
	vector<double> y_lap_p = hv_lap_p.vertical_lap;
	//上面位置
	vector<double> x_lap_m = hv_lap_m.horizontal_lap;
	vector<double> y_lap_m = hv_lap_m.vertical_lap;

	vector<double> Getangle_x;
	vector<double> Getangle_y;
	//计算x方向的偏转角度
	vector<double> x_pt;
	for (int i = 0; i < 3; i++)
	{
		x_pt.push_back(x_pt_o[i].x);
	}
	cout << endl << "Fitting!" << endl;
	cout << endl << "Delta_Z：" ;
	Getangle_x = getangle(x_lap_o, x_lap_p, x_lap_m, x_pt, deltaZ_p, deltaZ_m, g_AAParam.pixel_size/g_AAParam.scale);
	result.x_angle = Getangle_x[0];
	//计算y方向的偏转角度
	vector<double> y_pt;
	for (int i = 0; i < 3; i++)
	{
		y_pt.push_back(y_pt_o[i].y);
	}
	Getangle_y = getangle(y_lap_o, y_lap_p, y_lap_m, y_pt, deltaZ_p, deltaZ_m, g_AAParam.pixel_size/g_AAParam.scale);
	result.y_angle = Getangle_y[0];
	result.deltaZ = (Getangle_x[2] + Getangle_y[2])/2;
	cout << result.deltaZ ;
	return ret;

}

State getState(Mat img_ori, Mat img_p, Mat img_m, double deltaZ_p, double deltaZ_m, double pixel_size,double scale,int cameraID,int * flag)
{
	//参数设定，如有需要，可挪移至主函数
	double f = 3.31;                      //镜头焦距为3.31mm
	double d_obj = 500;					  //物距为600mm
	double size = 90, block = 140;        //选框窗口左上点在基准点左上90像素处，block长宽为140像素
	int edge_size = 15;                   //刃边框尺寸长宽为15像素
	int block_size = 19;                  //极大值抑制判断刃边位置尺寸长宽为19像素
	float thr = pow((2 * (float)block_size + 1), 2) / 2 * 0.2;   //极大值抑制判断抑制

	int temType = g_AAParam.temPlate[cameraID];

	f= g_AAParam.focus;
	d_obj = g_AAParam.objdis;
	size =g_AAParam.size;
	block = g_AAParam.block;
	edge_size = g_AAParam.edge_size;
	block_size = g_AAParam.block_size;
	thr = pow((2 * (float)block_size + 1), 2) / 2 * 0.2;

	//计算三幅图像的laplace量
	State state= {0};
	State state1 = {0};
	int Laplace_Flag_o = -1;
	int Laplace_Flag_p = -1;
	int Laplace_Flag_m = -1;
	HV_Lap hv_lap_o = getEdgeLaplace(img_ori,cameraID, &Laplace_Flag_o);
	HV_Lap hv_lap_p = getEdgeLaplace(img_p,cameraID, &Laplace_Flag_p);
	HV_Lap hv_lap_m = getEdgeLaplace(img_m, cameraID,&Laplace_Flag_m);
	if(Laplace_Flag_o<0 || Laplace_Flag_p<0 || Laplace_Flag_m<0)
	{
		*flag= AA_lAP_ERROR;
		return state1;
	}
	else
	{
		*flag = AA_lAP_OK;
	}
	//存放在向量中的顺序为:左边-中间-右边-  上边 -中间--下边
	//原点位置
	vector<Point> x_pt_o = hv_lap_o.horizontal_pt;//水平3个点
	vector<Point> y_pt_o = hv_lap_o.vertical_pt;//垂直3个点
	vector<double> x_lap_o = hv_lap_o.horizontal_lap;//水平3个拉普拉斯
	vector<double> y_lap_o = hv_lap_o.vertical_lap;//垂直3个拉普拉斯

	//前面位置拉普
	vector<double> x_lap_p  = hv_lap_p.horizontal_lap;
	vector<double> y_lap_p = hv_lap_p.vertical_lap;
	//后面位置拉普
	vector<double> x_lap_m = hv_lap_m.horizontal_lap;
	vector<double> y_lap_m = hv_lap_m.vertical_lap;

    //判断单边情况
	if (x_lap_o[1] < x_lap_m[1] || x_lap_o[1] < x_lap_p[1]){//水平--位置
		*flag=  AA_lAP_SIGNEL;
		if (x_lap_m[1] <=  x_lap_p[1]) {state.OneSide_flag[0] = 1;}
		if (x_lap_m[1] >  x_lap_p[1]) {state.OneSide_flag[0] = -1;}
		cout<<"x_lap_o[1]:"<<x_lap_o[1]<<"x_lap_m[1]:"<<x_lap_m[1]<<"x_lap_p[1]:"<<x_lap_p[1]<<endl;
		cout << "center_x is error" << endl;
	}
	if (y_lap_o[1] < y_lap_m[1] || y_lap_o[1] < y_lap_p[1]){//垂直中间位置
		*flag= AA_lAP_SIGNEL;
		if (y_lap_m[1] <=  y_lap_p[1]) {state.OneSide_flag[1] = 1;}
		if (y_lap_m[1] >  y_lap_p[1]) {state.OneSide_flag[1] = -1;}
		cout<<"y_lap_o[1]:"<<y_lap_o[1]<<"y_lap_m[1]:"<<y_lap_m[1]<<"y_lap_p[1]:"<<y_lap_p[1]<<endl;
		cout << "center_y is error" << endl;
	}
	if (x_lap_o[0] < x_lap_m[0] || x_lap_o[0] < x_lap_p[0]){
		*flag= AA_lAP_SIGNEL;
		if (x_lap_m[0] <=  x_lap_p[0]) {state.OneSide_flag[2] = 1;}
		if (x_lap_m[0] >  x_lap_p[0]) {state.OneSide_flag[2] = -1;}
		cout<<"x_lap_o[0]:"<<x_lap_o[0]<<"x_lap_m[0]:"<<x_lap_m[0]<<"x_lap_p[0]:"<<x_lap_p[0]<<endl;
		cout << "left is error" << endl;
	
	}
	if (x_lap_o[2] < x_lap_m[2] || x_lap_o[2] < x_lap_p[2]){
		*flag= AA_lAP_SIGNEL;
		if (x_lap_m[2] <=  x_lap_p[2]) {state.OneSide_flag[3] = 1;}
		if (x_lap_m[2] >  x_lap_p[2]) {state.OneSide_flag[3] = -1;}
		cout<<"x_lap_o[2]:"<<x_lap_o[2]<<"x_lap_m[2]:"<<x_lap_m[2]<<"x_lap_p[2]:"<<x_lap_p[2]<<endl;
		cout << "right is error" << endl;
	
	}
	if (y_lap_o[0] < y_lap_m[0] || y_lap_o[0] < y_lap_p[0]){
		*flag= AA_lAP_SIGNEL;
		if (y_lap_m[0] <=  y_lap_p[0]) {state.OneSide_flag[4] = 1;}
		if (y_lap_m[0] >  y_lap_p[0]) {state.OneSide_flag[4] = -1;}
		cout<<"y_lap_o[0]:"<<y_lap_o[0]<<"y_lap_m[0]:"<<y_lap_m[0]<<"y_lap_p[0]:"<<y_lap_p[0]<<endl;
		cout << "top is error" << endl;
	}
	if (y_lap_o[2] < y_lap_m[2] || y_lap_o[2] < y_lap_p[2]){
		*flag= AA_lAP_SIGNEL;
		if (y_lap_m[2] <=  y_lap_p[2]) {state.OneSide_flag[5] = 1;}
		if (y_lap_m[2] >  y_lap_p[2]) {state.OneSide_flag[5] = -1;}
		cout<<"y_lap_o[2]:"<<y_lap_o[2]<<"y_lap_m[2]:"<<y_lap_m[2]<<"y_lap_p[2]:"<<y_lap_p[2]<<endl;
		cout << "bottom is error" << endl;
	}
	vector<double> Getangle_x;
	vector<double> Getangle_y;
	//*flag=0;
	//计算x方向的偏转角度
	vector<double> x_pt;
	for (int i = 0; i < 3; i++)
	{
		x_pt.push_back(x_pt_o[i].x);
	}
	cout << endl << "Fitting!" << endl;
	cout << endl << "Delta_Z：" ;
	Getangle_x = getangle(x_lap_o, x_lap_p, x_lap_m, x_pt, deltaZ_p, deltaZ_m, g_AAParam.pixel_size/g_AAParam.scale);
	state.x_angle = Getangle_x[0];
	//计算y方向的偏转角度
	vector<double> y_pt;
	for (int i = 0; i < 3; i++)
	{
		y_pt.push_back(y_pt_o[i].y);
	}
	Getangle_y = getangle(y_lap_o, y_lap_p, y_lap_m, y_pt, deltaZ_p, deltaZ_m, g_AAParam.pixel_size/g_AAParam.scale);
	state.y_angle = Getangle_y[0];
	state.deltaZ = (Getangle_x[2] + Getangle_y[2])/2;
	cout << state.deltaZ ;
	return state;
}
/*求解角度*/
vector<double> getangle(vector<double> lap_o, vector<double> lap_p, vector<double> lap_m, vector<double> pt_o, double deltaZ_p, double deltaZ_m, double pixel_size)
{
	vector<double> deltaZ;
	vector<double> x;   
	x.push_back(deltaZ_p); x.push_back(0); x.push_back(deltaZ_m);
	//将移动范围分割为100份，用来拟合二次曲线
	double dx = (deltaZ_m - deltaZ_p) / 100;
	double x_temp[101] = { 0 };
	double lap[101] = { 0 };
	for (int i = 0; i < 101; i++)
	{
		x_temp[i] = deltaZ_p + i * dx;
	}
	
	for (int i = 0; i < lap_o.size(); i++)
	{
		vector<double> y;
		czy::Fit fit;
		y.push_back(lap_p[i]);
		y.push_back(lap_o[i]);
		y.push_back(lap_m[i]);
		//利用三张图同一位置的模糊量，拟合二次曲线，得到峰值所在位置
		fit.polyfit(x, y, 2, true);
		for (int m = 0; m < 101; m++){
			lap[m] = fit.getY(x_temp[m]);
		}
		int max_loc = maxloc(lap, 101);
		deltaZ.push_back(deltaZ_p + max_loc * dx);
	}
	for (int i = 0; i < 3; i++)
	{
		pt_o[i] = pt_o[i] * pixel_size;
	}
	double deltaZZ = deltaZ[1];   //为了返回中心点deltaZ
	deltaZ[1] = 2 * deltaZ[1]; //为何*2?
	
	//利用离焦量拟合标靶倾斜的一次函数
	czy::Fit fit_l;
	fit_l.linearFit(pt_o, deltaZ);
	//利用拟合得到的斜率得到倾斜角度，单位为°
	double angle = atan(fit_l.getFactor(1)) / 3.14159265 * 180;
	vector<double> angleAZ;
	angleAZ.push_back(angle);
	angleAZ.push_back(deltaZ[0]);
	angleAZ.push_back(deltaZZ);
	angleAZ.push_back(deltaZ[2]);
	return angleAZ;
}

/*
得到数组中最大值的位置
*/
int maxloc(double a[], int size)
{
	double max = 0;
	int maxloc = 0;
	for (int i = 0; i < size; i++)
	{
		if (max < a[i])
		{
			max = a[i];
			maxloc = i;
		}
	}
	return maxloc;
}

int calHarryPointsAndMTFValueInOnePic(Mat img,int vid,HV_Lap &result)
{
	int ret = 0;
	Point edgePoint;
	double lapVaule = 0.0;

	ret = calEdgePointInRegion(img,LEFT,vid,edgePoint,lapVaule);
	if (!ret)
	{	
		result.horizontal_lap.push_back(lapVaule);
		result.horizontal_pt.push_back(edgePoint);
	}
	else
	{
		ret= -1;
		msg("获取通道%d，区域:%d 失败",vid,LEFT);
		return ret;
	}


	ret = calEdgePointInRegion(img,CENTERX,vid,edgePoint,lapVaule);
	if (!ret)
	{	
		result.horizontal_lap.push_back(lapVaule);
		result.horizontal_pt.push_back(edgePoint);
	}
	else
	{
		ret= -1;
		msg("获取通道%d，区域:%d 失败",vid,CENTERX);
		return ret;
	}

	ret = calEdgePointInRegion(img,RIGHT,vid,edgePoint,lapVaule);
	if (!ret)
	{	
		result.horizontal_lap.push_back(lapVaule);
		result.horizontal_pt.push_back(edgePoint);
	}
	else
	{
		ret= -1;
		msg("获取通道%d，区域:%d 失败",vid,RIGHT);
		return ret;
	}

	ret = calEdgePointInRegion(img,TOP,vid,edgePoint,lapVaule);
	if (!ret)
	{	
		result.vertical_lap.push_back(lapVaule);
		result.vertical_pt.push_back(edgePoint);
	}
	else
	{
		ret= -1;
		msg("获取通道%d，区域:%d 失败",vid,TOP);
		return ret;
	}


	ret = calEdgePointInRegion(img,CENTERY,vid,edgePoint,lapVaule);
	if (!ret)
	{	
		result.vertical_lap.push_back(lapVaule);
		result.vertical_pt.push_back(edgePoint);
	}
	else
	{
		ret= -1;
		msg("获取通道%d，区域:%d 失败",vid,CENTERY);
		return ret;
	}

	ret = calEdgePointInRegion(img,BOTTOM,vid,edgePoint,lapVaule);
	if (!ret)
	{	
		result.vertical_lap.push_back(lapVaule);
		result.vertical_pt.push_back(edgePoint);
	}
	else
	{
		ret= -1;
		msg("获取通道%d，区域:%d 失败",vid,BOTTOM);
		return ret;
	}
	return ret;
}

int calEdgePointInRegion(Mat img,int vid,int regiontype,Point &edgePoint,double &lapVaule)
{
	int ret = 0;

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


	//定义模板:(原则:选择的中心为：0,左上点(上白下黑,左白右黑),1,右上点(上白下黑,左黑右白)，
	//2,左下点(上黑下白,左白右黑),3,右下点(上黑下白,左白右黑)

	int temPlate = g_AAParam.temPlate[vid];
	Mat h_x = Mat::ones(g_AAParam.block_size * 2 + 1, g_AAParam.block_size * 2 + 1, CV_32F);
	Mat h_y = Mat::ones(g_AAParam.block_size * 2 + 1, g_AAParam.block_size * 2 + 1, CV_32F);
	SelectTemplateType(temPlate,block_size,  h_x,h_y);//选择模板类型
	//是否模板反了:左右模板 取水平点，上下模板取垂直点，还是没有影响
	Mat temp;
	temp = h_x;
	h_x = h_y;
	h_y = temp;
	savePicture(h_x,"水平方向模板.jpg");
	savePicture(h_y,"垂直方向模板.jpg");

	//Center刃边选取与计算
	double col ,row;
	//画图中心不一定在中央,解决无法对准靶图中心问题，注意
	col = g_AAParam.paintCenter[vid].X*g_AAParam.scale,row = g_AAParam.paintCenter[vid].Y  * g_AAParam.scale;
	Mat value;
	int x_center_flag=0;
	int x =0,y= 0;
	double i_left = ceil(col - g_AAParam.mapdislength * beta / pixel_size); //可能每个图像顺序不一样
	double i_right = ceil(col + g_AAParam.mapdislength * beta / pixel_size);
	double j_top = ceil(row - g_AAParam.mapdiswidth * beta / pixel_size);
	double  j_bottom = ceil(row + g_AAParam.mapdiswidth * beta / pixel_size);

	//区域: 根据区域类型来确定上下左右的中心
	//AA_TOP,
	//	AA_BOTTOM,
	//	AA_CENTER,
	//	AA_LEFT,
	//	AA_RIGHT,
	//	AA_LEFT_LOC,
	//	AA_RIGHT_LOC,

	switch(regiontype)
	{
	case CENTERX:
		break;
	case CENTERY: //暂时把AA_LEFT_LOC映射为垂直方向的y
		h_x = h_y;
		break;
	case LEFT:
		col = ceil(col - g_AAParam.mapdislength * beta / pixel_size);
		break;
	case RIGHT:
		col = ceil(col + g_AAParam.mapdislength * beta / pixel_size);
		break;
	case TOP:
		h_x = h_y;
		row = ceil(row - g_AAParam.mapdiswidth * beta / pixel_size);
		break;
	case BOTTOM:
		h_x = h_y;
		row = ceil(row + g_AAParam.mapdiswidth * beta / pixel_size);
		break;
	}
	//对图像中值滤波
	medianBlur(img, img, 3);

	//int x = g_AAParam.channel1Retangle[vid][regiontype].lty* g_AAParam.scale +20;
	//int y = g_AAParam.channel1Retangle[vid][regiontype].ltx* g_AAParam.scale;
	//int rheight = g_AAParam.channel1Retangle[vid][regiontype].rbx* g_AAParam.scale - g_AAParam.channel1Retangle[vid][regiontype].ltx* g_AAParam.scale;
	//int rwid = g_AAParam.channel1Retangle[vid][regiontype].rby * g_AAParam.scale- g_AAParam.channel1Retangle[vid][regiontype].lty* g_AAParam.scale;

	IAVO_ShowPicture("滤波前完整图",img);

	IAVO_SavePicture("./image//滤波前完整图.jpg",img);

	//filter2D(img(cvRect(x, y, rwid, rheight)), value, img.depth(), temPlate, Point(-1, -1), (0, 0), BORDER_REFLECT);
	//filter2D(img(cvRect(x, y, rwid, rheight)), value, img.depth(), h_x, Point(-1, -1), (0, 0), BORDER_REFLECT);
	getFilterValue(temPlate,img,h_x,col,row,size,block,value,regiontype);

	//Mat blockimg = img(cvRect(col, row, block, block));
	//int a1 = blockimg.rows;
	//a1 = blockimg.cols;

	//Mat sizeimg = img(cvRect(col, row, block, block));
	//a1 = sizeimg.rows;
	//a1 = sizeimg.cols;

	//IAVO_ShowPicture("滤波前",blockimg);

	
	IAVO_SavePicture("./image//value.jpg",value);
	//getFilterValue(temPlate,img,h_x,i_center,j_center,size,block,value_x,regiontype);
	IAVO_SavePicture("./image//滤波前.jpg",img(cvRect(x, y, block, block)));
	for (int j = block_size; j < block - block_size; j++)
	{
		float* data_x = value.ptr<float>(j);
		for (int i = block_size ; i < block - block_size ; i++)
		{
			double p = data_x[i];
			Point maxP,minP;
			double maxVal,minVal;
			minMaxLoc(value, &minVal, &maxVal, &minP, &maxP);
			if (i==110)
			{
				p = data_x[i];
			}
			if (data_x[i] > thr)
			{
				Mat roi(value, Rect(i - block_size, j - block_size, 2 * block_size + 1, 2 * block_size + 1));
				Point maxPoint,maxPoint_Img;
				//IAVO_ShowPicture("test",roi);
				
				int a = roi.channels();
				minMaxLoc(roi, 0, 0, 0, &maxPoint);
				maxPoint.x = maxPoint.x + i - block_size;
				maxPoint.y = maxPoint.y + j - block_size;
				if ( i == maxPoint.x && j == maxPoint.y ){
					maxPoint_Img.x = i + col - size;
					maxPoint_Img.y = j + row - size;
					edgePoint = (maxPoint_Img);
					x_center_flag=1;

				}

			}
		}
	}

	Mat imgpaint = img.clone();
	circle(imgpaint, edgePoint, 10, cvScalar(255, 0, 0), 2);
	//保存图片不改变原来的图像
	string filename = "刃边点_";
	char buf[64] ={0};
	sprintf(buf,"%d_%d",vid,regiontype);
	filename.append(buf);
	filename.append(".jpg") ;
	IAVO_SavePicture(filename,imgpaint);
	//IAVO_ShowPicture("原图",img);

	IAVO_ShowPicture("刃边点",imgpaint);
	//计算拉普拉斯值
	lapVaule = laplace(img, edgePoint, edge_size);
	return ret;
}