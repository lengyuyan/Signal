#pragma once

//#ifndef _THREADS_H_
//#define _THREADS_H_


#include "HDevEngineCpp.h"
using namespace HDevEngineCpp;
#include <HalconCpp.h>
using namespace HalconCpp;

void NewImage(HObject *newhimg, BYTE* pBmp,int w,int h);
void NewSigChannelImage(HObject *newhimg, unsigned char * pBmp,int w,int h);
bool dispImage(int winID, int index);

void Camera_ThreadA(void *param);
void Display_ThreadA(void *param);






//#endif