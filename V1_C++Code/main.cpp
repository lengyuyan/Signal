#include <opencv2\opencv.hpp>
#include <iostream>
#include <string>
#include <math.h>
#include <sstream>
using namespace cv;
using namespace std;

typedef struct {
	double x_angle;
	double y_angle;
	double deltaZ;
}State; // get the current state of the initial image including u, v and z
State getState(Mat img_ori, Mat img_p, Mat img_m, double deltaZ_p, double deltaZ_m, double pixel_size);
Mat Img_resize(Mat I, double scale);
string ConvertToString(double value);
/*
%Main program
%============================================
%Author:Ocean
%Date:2017.4.10
%============================================
%Nargin Set
% ori_num: the number of the original image, related to the "img_ori";
% p_num: the number of the positive image, related to the "img_p";
% m_num: the number of the minus image, related to the "img_m";
% pixel size: set as 1.12¶Ãm;
% std_error: set as 0.03;
%--------------------------------------------
%Output
% Output by getState();
%============================================
*/
void main(){
	double ori_num;
	double p_num;
	double m_num;
	cout << "Author£∫Ocean   2017.4.12 " << endl;
	cout << "You can use this program to get the adjust_U V Z" << endl<<endl;
	cout << "Please set the ori_num!" << endl;
	cin >> ori_num;
	cout << "Please set the p_num! (Smaller)" << endl;
	cin >> p_num;
	cout << "Please set the m_num! (Bigger)" << endl;
	cin >> m_num;
	cout << endl << "Get! Running!"<< endl << endl;

	double time = static_cast<double>(getTickCount());
	//≤Œ ˝ ‰»Î
	double scale = 0.2;
	double pixel_size = 0.00112 / scale;
	double std_error = 0.03;

	double deltaZ_p = ori_num - p_num;
	double deltaZ_m = ori_num - m_num;
	string ori_file = ConvertToString(ori_num);
	string p_file = ConvertToString(p_num);
	string m_file = ConvertToString(m_num);
	Mat img_ori = imread(ori_file + ".bmp");
	Mat img_p = imread(p_file + ".bmp");
	Mat img_m = imread(m_file + ".bmp");
	img_ori = Img_resize(img_ori, scale);
	img_p = Img_resize(img_p, scale);
	img_m = Img_resize(img_m, scale);
	State state;
	state = getState(img_ori,img_p,img_m,deltaZ_p,deltaZ_m,pixel_size);
	double adjust_z = state.deltaZ;
	double adjust_u = state.x_angle * -1;
	double adjust_v = state.y_angle * -1;
	time = ((double)getTickCount() - time) / getTickFrequency();
	cout << endl << "Time£∫ " << time << "s" << endl;
	cout << endl << "Result:" << endl;
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
	ss << value;
	return ss.str();
}