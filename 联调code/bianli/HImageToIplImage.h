/**********************
²âÊÔ¹¦ÄÜ:
mat---->Hobject
Hobject---->mat
***********************/

#pragma once
#include <iostream>
#include <opencv2\opencv.hpp>
using namespace cv;
using namespace std;


#include <HalconCpp.h>
using namespace HalconCpp;

Mat HImageToIplImage(HObject &Hobj)  ;
HObject IplImageToHImage(Mat& pImage)  ;

int test_HImageToIplImage();
int test_IplImageToHImage();
