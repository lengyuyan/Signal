#pragma once
#include <iostream>
#include <opencv2\opencv.hpp>
using namespace cv;
using namespace std;


//
//class Mat;
//4种方法来遍历图像像素
int ScanImageEfficiet(Mat & image);
int ScanImageIterator(Mat & image);
int ScanImageRandomAccess(Mat & image);
int ScanImageLUT(Mat & image);

int ScanImage(Mat & image);//调用上面4中方法