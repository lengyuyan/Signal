
#include <iostream>  
#include <opencv2\opencv.hpp>

using namespace cv;   
using namespace std;   
  

int test1()
{
	Mat src;   
	Mat src_gray;   
	int thresh = 45;   
	int max_thresh = 255;  
	//src = imread( "fixMat.jpg" ,CV_LOAD_IMAGE_COLOR );    //注意路径得换成自己的 
	src = imread( "movMat.jpg" ,CV_LOAD_IMAGE_COLOR ); 
	cvtColor( src, src_gray, CV_BGR2GRAY );//灰度化      
	GaussianBlur( src, src, Size(3,3), 0.1, 0, BORDER_DEFAULT );      
	blur( src_gray, src_gray, Size(3,3) ); //滤波       
	namedWindow( "image", CV_WINDOW_AUTOSIZE );       
	imshow( "image", src );       
	moveWindow("image",20,20); 


	//定义Canny边缘检测图像       
	Mat canny_output;     
	vector<vector<Point> > contours;      
	vector<Vec4i> hierarchy;    
	//利用canny算法检测边缘       
	Canny( src_gray, canny_output, thresh, thresh*3, 3 );     
	namedWindow( "canny", CV_WINDOW_AUTOSIZE );       
	imshow( "canny", canny_output );      
	moveWindow("canny",550,20);       
	//查找轮廓    
	//findContours( canny_output, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0) ); 

	findContours( canny_output, contours, hierarchy, CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE, Point(0, 0) ); 


	//vector<vector<Point> >::iterator it;
	//vector<Point>
	//for( it = contours.begin(); i != contours.end(); it++ )    
	//{    
	//	 double tmparea = fabs(contourArea(it));
	//	 if (tmparea <500)
	//	 {
	//		 contours;
	//	 }
	//	 
	//}
	//计算轮廓矩       
	vector<Moments> mu(contours.size() );       
	for( int i = 0; i < contours.size(); i++ )     
	{   
		mu[i] = moments( contours[i], false );   
	}     
	//计算轮廓的质心     
	vector<Point2f> mc( contours.size() );      
	for( int i = 0; i < contours.size(); i++ )     
	{   
		mc[i] = Point2d( mu[i].m10/mu[i].m00 , mu[i].m01/mu[i].m00 );   
	}     
	//画轮廓及其质心并显示      
	Mat drawing = Mat::zeros( canny_output.size(), CV_8UC3 );         
	for( int i = 0; i< contours.size(); i++ )      
	{         
		Scalar color = Scalar( 255, 0, 0);        
		drawContours( drawing, contours, i, color, 2, 8, hierarchy, 0, Point() );         
		circle( drawing, mc[i], 5, Scalar( 0, 0, 255), -1, 8, 0 );                
		rectangle(drawing, boundingRect(contours.at(i)), cvScalar(0,255,0));              
		char tam[100];   
		sprintf(tam, "(%0.0f,%0.0f)",mc[i].x,mc[i].y);   
		putText(drawing, tam, Point(mc[i].x, mc[i].y), FONT_HERSHEY_SIMPLEX, 0.4, cvScalar(255,0,255),1);     
	}     
	namedWindow( "Contours", CV_WINDOW_AUTOSIZE );    
	imshow( "Contours", drawing );    
	moveWindow("Contours",1100,20);       
	waitKey(0);       
	src.release();    
	src_gray.release();       
	return 0;   

}
int test2()
{
	IplImage* src = cvLoadImage("movMat.jpg", CV_LOAD_IMAGE_GRAYSCALE);  
	IplImage* dst = cvCreateImage(cvGetSize(src), 8, 3);  
	CvMemStorage* storage = cvCreateMemStorage(0);  
	CvSeq* contour = 0;  
	cvThreshold(src, src,120, 255, CV_THRESH_BINARY);   // 二值化  
	cvNamedWindow("Source", 1);  
	cvShowImage("Source", src);  
	// 提取轮廓  
	int contour_num = cvFindContours(src, storage, &contour, sizeof(CvContour), CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE);  
	cvZero(dst);        // 清空数组  
	CvSeq *_contour = contour;   
	double maxarea = 0;  
	double minarea = 100;  
	int m = 0;  
	for( ; contour != 0; contour = contour->h_next )    
	{    

		double tmparea = fabs(cvContourArea(contour));  
		if(tmparea < minarea)     
		{    
			cvSeqRemove(contour, 0); // 删除面积小于设定值的轮廓  
			continue;  
		}    
		CvRect aRect = cvBoundingRect( contour, 0 );   
		if ((aRect.width/aRect.height)<1)    
		{    
			cvSeqRemove(contour, 0); //删除宽高比例小于设定值的轮廓  
			continue;  
		}    
		if(tmparea > maxarea)    
		{    
			maxarea = tmparea;  
		}    
		m++;  
		// 创建一个色彩值  
		CvScalar color = CV_RGB( 0, 255, 255 );  

		//max_level 绘制轮廓的最大等级。如果等级为0，绘制单独的轮廓。如果为1，绘制轮廓及在其后的相同的级别下轮廓  
		//如果值为2，所有的轮廓。如果等级为2，绘制所有同级轮廓及所有低一级轮廓，诸此种种  
		//如果值为负数，函数不绘制同级轮廓，但会升序绘制直到级别为abs(max_level)-1的子轮廓  
		cvDrawContours(dst, contour, color, color, -1, 1, 8);   //绘制外部和内部的轮廓  
	}    
	contour = _contour;  
	int count = 0;  
	for(; contour != 0; contour = contour->h_next)  
	{    
		count++;  
		double tmparea = fabs(cvContourArea(contour));  
		
		if (tmparea == maxarea)    
		{    
			CvScalar color = CV_RGB( 255, 0, 0);  
			cvDrawContours(dst, contour, color, color, -1, 1, 8);  
		}    
	}    
	printf("The total number of contours is:%d", count);  
	cvNamedWindow("Components", 1);  
	cvShowImage("Components", dst);  
	cvWaitKey(0);  
	cvDestroyWindow("Source");  
	cvReleaseImage(&src);  
	cvDestroyWindow("Components");  
	cvReleaseImage(&dst);  
	return 0;
}

//角点
int test3()
{
#define MAX_CORNERS 100
int cornersCount = MAX_CORNERS;
CvPoint2D32f corners[MAX_CORNERS];
IplImage *srcImage = 0, *grayImage = 0, *corners1 = 0, *corners2 = 0;
int i;
CvScalar color = CV_RGB(255, 0, 0);
char *filename = "movMat.jpg";
cvNamedWindow("image", 1);

srcImage = cvLoadImage(filename, 1);
grayImage = cvCreateImage(cvGetSize(srcImage), IPL_DEPTH_8U, 1);
cvCvtColor(srcImage, grayImage, CV_BGR2GRAY);//转换为灰度图

corners1 = cvCreateImage(cvGetSize(srcImage), IPL_DEPTH_32F, 1);
corners2 = cvCreateImage(cvGetSize(srcImage), IPL_DEPTH_32F, 1);//先创立两个图像，在函数中药包含这两个参数

cvGoodFeaturesToTrack(grayImage, corners1, corners2, corners, &cornersCount, 0.05, 30, 0, 3, 0, 0.4);//调用OpenCV函数
cout << "num corners found: " << cornersCount << endl;

if (cornersCount > 0)
{
	for (i = 0; i < cornersCount; ++i)
	{
		cvCircle(srcImage, cvPoint((int)(corners[i].x), (int)(corners[i].y)),2,color,2,CV_AA,0);

	}
}//将认为是角点的画出来
cvShowImage("image", srcImage);
cvSaveImage("imagedst.png", srcImage);

cvReleaseImage(&srcImage);
cvReleaseImage(&grayImage);
cvReleaseImage(&corners1);
cvReleaseImage(&corners2);

cvWaitKey(0);
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
Mat Img_resize2(Mat I, double scale){
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


void cornerShiTomasi_demo()  
{  
	int maxCorners = 100;
	Mat src,src_gray,src2,src_gray2;
	string corners_window = "corn";
	string source_window = "src";
	//src = imread("1.jpg");
	//src = imread("movMat.jpg");
	src = imread("fixMat.jpg");

	src2 = imread("0.520.jpg");

	cvtColor(src,src_gray,CV_BGR2GRAY);

	cvtColor(src2,src_gray2,CV_BGR2GRAY);
	src_gray2 = Img_resize2(src_gray2,0.2);

	showPicture(src_gray2,"o");
	int a =0,b =0;; 
	a = src_gray2.rows;
	b = src_gray2.cols;
	for (int x = 0; x < src_gray2.rows; x++)
	{
		for (int y = 0; y < src_gray2.cols; y++)
		{
			float a = src_gray2.at<float>(x ,y);
			if ( x <100 || x< 200 )//在100 - 200行为a
			{
				src_gray2.at<float>(x , y)= a;
			}
		}
	}

	showPicture(src_gray2,"o2");

	//src.convertTo(src_gray,CV_8UC1);

	// src_gray = cv::Mat(1200,1920,CV_8UC1);

	Mat dst, dst_norm,dst_norm_scaled;


	if( maxCorners < 1 ) { maxCorners = 1; }  
	/// Parameters for Shi-Tomasi algorithm  
	vector<Point2f> corners;  
	double qualityLevel = 0.01;  
	double minDistance = 10;  
	int blockSize = 3;  
	bool useHarrisDetector = false;  
	double k = 0.04;  
	/// Copy the source image  
	Mat cormat;  

	showPicture(src_gray,"1");
	equalizeHist(src_gray,src_gray);

	showPicture(src_gray,"2");

	/// Apply corner detection :Determines strong corners on an image.  
	//goodFeaturesToTrack( src_gray,corners,  maxCorners,  qualityLevel,  minDistance,  Mat(),  blockSize,  useHarrisDetector,  k );  
	goodFeaturesToTrack( src_gray,corners,  maxCorners,   0.05, 30, Mat(), 3, 0, 0.4); 
	/// Draw corners detected  
	for( int i = 0; i < corners.size(); i++ ){   
		circle( dst_norm_scaled,  corners[i], 5,  Scalar(255), 2, 8, 0 );   
		circle( src, corners[i], 4, Scalar(0,255,0), 2, 8, 0 );   
	}  

	/// Show what you got  
	//imshow( corners_window, dst_norm_scaled ); 
	showPicture(src,source_window  );
	//showPicture(dst_norm_scaled,corners_window );

}  
int main()  
{   
	//test1();
	cornerShiTomasi_demo();

	int ret =0;;
	cin>>ret;
	return 0;

}