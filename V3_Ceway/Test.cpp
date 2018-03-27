#include <opencv2\opencv.hpp>
#include <iostream>
#include <string>
#include <math.h>
#include <sstream>
using namespace cv;
using namespace std;

typedef struct{
	vector<double> horizontal_lap;
	vector<double> vertical_lap;
	vector<Point> horizontal_pt;
	vector<Point> vertical_pt;
	double time_Edge;
}HV_Lap;
HV_Lap getAllLaplace(Mat img, int edge_size, int error);
Mat Img_resize(Mat I, double scale);


vector<HV_Lap> Test(double num, vector<string> filename){
	vector<HV_Lap> lap;
	HV_Lap temp2;
	double scale = 0.2;
	double pixel_size = 0.00112 / scale;
	double std_error = 0.03;
	for (int i = 0; i < num; i++){
		Mat temp = imread(filename[i]);
		temp = Img_resize(temp, scale);
		temp2 = getAllLaplace(temp, 15, 40);
		lap.push_back(temp2);
	}
	return lap;
}


