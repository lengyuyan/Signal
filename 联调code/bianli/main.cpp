#include <iostream>
#include <opencv2\opencv.hpp>
using namespace cv;
using namespace std;

#include "Traversing elements.h"//测试遍历
#include "checkHa.h"            //检测角点

//测试工程
int test1()
{
	Mat a(2,3,CV_32F);
	Mat I;
	IplImage* pI=&I.operator IplImage();//mat---》ip


	int row = a.rows;
	int col = a.cols;
	bool flag = a.isContinuous();
	float* p =NULL;
	for (int i=0;i<row;i++)
	{
		p = a.ptr<float>(i);//行指针
		for (int j= 0;j<col;j++)
		{
			//unsigned int a = atoi((char*)(p));
			//float a = p[j]-'0'+'0';
			char buf[100] ={0};
			sprintf(buf,"%f",p[j]);
			cout<<p[j]<<"	"<<buf<<endl;
		}

	}
	if(flag)
	{

		col *=row;
		row = 1;
	}
	//for (int i=0;i<row;i++)
	//{
	//	p = a.ptr<uchar>(i);//行指针
	//	for (int j= 0;j<col;j++)
	//	{
	//		cout<<p[j]<<"	"<<endl;
	//	}

	//}
	return 0;
}

int main()
{
	int ret = 0;

	//测试指针操作
	//test1();
	//测试遍历
	Mat image;
	ret = ScanImage(image);

	//测试找角点
	//testcheckHa();
	

	cin>>ret;
	return 0;
}