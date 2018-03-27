#include <opencv2\opencv.hpp>
#include <iostream>
#include <string>
#include <math.h>
#include <sstream>
#include <limits> 
using namespace cv;
using namespace std;

typedef struct {
	double x_angle;
	double y_angle;
	double deltaZ;
}State; // get the current state of the initial image including u, v and z
State getState(Mat img_ori, Mat img_p, Mat img_m, double deltaZ_p, double deltaZ_m, double pixel_size, double scale,int * flag);
Mat Img_resize(Mat I, double scale);
string ConvertToString(double value);
/*
%Main program
%============================================
%Author:Ocean
%Date:2017.6.5
%============================================
%Nargin Set
% ori_num: the number of the original image, related to the "img_ori";
% p_num: the number of the positive image, related to the "img_p";
% m_num: the number of the minus image, related to the "img_m";
% pixel size: set as 1.12μm;
%--------------------------------------------
%Output
% Output by getState();
%============================================
*/

void test1()
{
	Mat image(Size(500,500),CV_8UC3);  
	image.at<Vec3b>(100,250)[0]=0;  
	image.at<Vec3b>(100,250)[1]=0;  
	image.at<Vec3b>(100,250)[2]=255;  
	putText(image,"at(100,250) is Here!",Point(250,100),0,0.7,Scalar(255,0,0));  //250列100行 ，点,字体 大小,颜色

	image.at<Vec3b>(Point(100,250))[0]=0;  
	image.at<Vec3b>(Point(100,250))[1]=0;  
	image.at<Vec3b>(Point(100,250))[2]=255;  
	putText(image,"at(Point(100,250)) is Here!",Point(100,250),0,0.7,Scalar(255,0,0));  

	imshow("Test Function at",image);  
	waitKey();
	return ;

}
void main(){

	//基础测试
	test1();
	return ;

	State state = {0};//x,y旋转角度，z轴偏移
	int flag = 0;//转换标志

	string ori_file;
	string p_file;
	string m_file;

	double ori_num = 0.578;
	double p_num = 0.528;
	double m_num = 0.628;

	ori_file  = "-16.22";
    p_file = "-16.24";
	 m_file = "-16.00";
	
	ori_file = "1";
	p_file = "2";
	m_file = "3";

	cout << "Author：Ocean   2017.4.12 " << endl;
	cout << "You can use this program to get the adjust_U V Z" << endl<<endl;

	 
	ori_num = atof(ori_file.c_str());
	 p_num = atof(p_file.c_str());
	 m_num = atof(m_file.c_str());

	double time = static_cast<double>(getTickCount());

	//参数输入
	double scale = 0.2;
	double pixel_size = 0.00112 / scale;///这个像素值是怎么得出来的?

	double deltaZ_p = p_num - ori_num;//离目标近
	double deltaZ_m = m_num - ori_num;//离目标远


	


	Mat img_ori = imread(ori_file + ".jpg");
	Mat img_p = imread(p_file + ".jpg");
	Mat img_m = imread(m_file + ".jpg");

	cout<<"初始原点图像通道数："<<img_ori.channels()<<endl;
	cout<<"初始原点图像深度："<<img_ori.depth()<<endl;
	
	  //img_ori = imread(ori_file + ".bmp");
	  //img_p = imread(p_file + ".bmp");
	  //img_m = imread(m_file + ".bmp");
	
	img_ori = Img_resize(img_ori, scale);
	img_p = Img_resize(img_p, scale);
	img_m = Img_resize(img_m, scale);//



	cout<<"resize后原点图像通道数："<<img_ori.channels()<<endl;
	cout<<"resize后原点图像深度："<<img_ori.depth()<<endl;
	
	state = getState(img_ori,img_p,img_m,deltaZ_p,deltaZ_m,pixel_size,scale,&flag);

	double adjust_z = state.deltaZ;
	double adjust_u = state.x_angle * -1;//角度为何要取反
	double adjust_v = state.y_angle * -1;

	time = ((double)getTickCount() - time) / getTickFrequency();
	cout << endl << "Time： " << time << "s" << endl;
	cout << endl << "Result:" <<(flag == 0?"OK":"NG")<< endl;
	cout << "Adjust_Z = " << adjust_z << endl;
	cout << "Adjust_U = " << adjust_u << endl;
	cout << "Adjust_V = " << adjust_v << endl;
	system("pause");
}
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
	 int prec = numeric_limits<double>::digits10;
	ss.precision(prec);
	ss << value;
	return ss.str();
}