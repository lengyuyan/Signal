#include "IAVisionAlgorithm.h"

#define MAXGRAY 255

//工具接口

int IAVO_ShowPicture(const string windowname,Mat &img )
{	
	namedWindow(windowname,WINDOW_NORMAL);
	imshow(windowname.c_str(),img);
	waitKey();
	destroyWindow(windowname);
	return IAV_OK;
}

int  IAVO_ShowWindow(const string windowname,Mat &img,int windoww,int windowh)
{
	int ret = IAV_OK;
	try
	{
		
		Mat showImg;
		if (windoww <= 0 || windowh <= 0)
		{
			imshow(windowname.c_str(),img);
		}
		else
		{
			resize (img, showImg ,Size(windoww,windowh) );
			imshow(windowname.c_str(),showImg);
		}
			
	}
	catch (...)
	{
		ret = IAV_ERROR;
	}
	return ret;

}
int IAVO_SavePicture(string filename,Mat &img)
{
	///暂时认为输入img都为cvflaot_32
	//Mat I = img.clone();
//	I = I*MAXGRAY;  
	//I.convertTo(I, CV_8U);
	//imwrite(filename,I);
	//I.release();
	//imwrite(filename,img);
	

	Mat I = img.clone();
	if (CV_32F == img.type())
	{	
		I = I*255;  
		I.convertTo(I, CV_8U);

	}
	imwrite(filename,I);
	I.release();
	return IAV_OK;
}
int IAVO_ReadPicture(string filename,Mat &img)
{
	int ret = IAV_OK;
	try
	{
		img = imread(filename);
		
	}
	catch (...)
	{
		ret = IAV_ERROR;
	}
	return ret;
	
}
Mat IAVO_ImgResize(Mat& img, double scale)
{
	int ret = IAV_OK;
	Mat result;
	try
	{
		if (img.channels() == 3){
			cvtColor(img, result, CV_RGB2GRAY);
		}
		int height = img.rows * scale;
		int width = img.cols * scale;
		resize(result,result, Size(width, height));
		result.convertTo(result, CV_32F);
		result = result / 255;

	}
	catch (...)
	{
		ret = IAV_ERROR;
	}

	return result;

}



vector<Point2d> IAV_GetCornerInOnePic(Mat &B)
{
	vector<Point2d> PT;

	return PT;

}


