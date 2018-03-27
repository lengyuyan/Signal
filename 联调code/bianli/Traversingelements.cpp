#include "Traversing elements.h"

#include "IAVisionAlgorithm.h"


const int divideWidth = 128;
// converting table for reducing color space
uchar table[256];

int ScanImageEfficiet(Mat & image)
{
	// channels of the image
	int iChannels = image.channels();
	// rows(height) of the image
	int iRows = image.rows;
	// cols(width) of the image
	int iCols = image.cols * iChannels;

	// check if the image data is stored continuous
	if (image.isContinuous())
	{
		iCols *= iRows;
		iRows = 1;
	}

	uchar* p;
	for (int i = 0; i < iRows; i++)
	{
		// get the pointer to the ith row
		p = image.ptr<uchar>(i);
		// operates on each pixel
		for (int j = 0; j < iCols; j++)
		{
			// assigns new value
			p[j] = table[p[j]];
		}
	}

	return 0;
}

/** @Method 2: the iterator(safe) method
 accept grayscale image and RGB image */
int ScanImageIterator(Mat & image)
{
    // channels of the image
    int iChannels = image.channels();
 
    switch (iChannels)
    {
    case 1:
    {
        MatIterator_<uchar> it, end;
        for (it = image.begin<uchar>(), end = image.end<uchar>(); it != end; it++)
        {
            *it = table[*it];
        }
        break;
    }
    case 3:
    {
        MatIterator_<Vec3b> it, end;
        for (it = image.begin<Vec3b>(), end = image.end<Vec3b>(); it != end; it++)
        {
            (*it)[0] = table[(*it)[0]];
            (*it)[1] = table[(*it)[1]];
            (*it)[2] = table[(*it)[2]];
        }
        break;
    }
    }
 
    return 0;
}

/** @Method 3: random access method
 accept grayscale image and RGB image */
int ScanImageRandomAccess(Mat & image)
{
    // channels of the image
    int iChannels = image.channels();
    // rows(height) of the image
    int iRows = image.rows;
    // cols(width) of the image
    int iCols = image.cols;
     
    switch (iChannels)
    {
    // grayscale
    case 1:
    {
        for (int i = 0; i < iRows; i++)
        {
            for (int j = 0; j < iCols; j++)
            {
                image.at<uchar>(i, j) = table[image.at<uchar>(i, j)];
            }
        }
        break;
    }
    // RGB
    case 3:
    {
        Mat_<Vec3b> _image = image;
        for (int i = 0; i < iRows; i++)
        {
            for (int j = 0; j < iCols; j++)
            {
                _image(i, j)[0] = table[_image(i, j)[0]];
                _image(i, j)[1] = table[_image(i, j)[1]];
                _image(i, j)[2] = table[_image(i, j)[2]];
            }
        }
        image = _image;
        break;
    }
    }
 
    return 0;
}

int ScanImageLUT(Mat & image)
{
	
	// do something


	// build a Mat type of the lookup table
	Mat lookupTable(1, 256, CV_8U);
	uchar* p = lookupTable.data;
	for (int i = 0; i < 256; i++)
	{
		p[i] = table[i];
	}
	// call the function
	LUT(image, lookupTable, image);


    return 0;
}

int ScanImage(Mat & img)
{
	int ret = 0;
	// color space divide width

	// first, we should build the converting table
	for (int i = 0; i < 256; i++)
	{
		table[i] = (uchar)(divideWidth * (i / divideWidth));
	}
	Mat image;

	image = imread("test.jpg",IMREAD_UNCHANGED);

	namedWindow("in",WINDOW_NORMAL);
	imshow("in",image);
	waitKey();
	destroyWindow("in");
	

	double dtime = (double) getTickCount();
	//ScanImageEfficiet(image);
	ret =  ScanImageLUT(image);
	dtime = ((double)getTickCount() - dtime)/getTickFrequency();
	imshow("out",image);

	IAVO_ShowPicture("out",image);
	
	cout<<"处理时间:"<<dtime<<endl;
	int a = image.channels();

	a = image.rows;
	a= image.cols;
	//a= image.rowRange(image);
	a= image.depth();
	a= image.step;
	a= image.elemSize();

	return ret;
}

