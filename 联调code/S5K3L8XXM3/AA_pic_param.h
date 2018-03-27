#pragma once
//图像参数配置结构设置

#include "AA_define_tool.h"
#include "IAVisionAlgorithm.h"


//Halcon图像参数设置
#define HALCON_PARAMSETTING "HALCON_PARAMSETTING"
#define SelectShapeP "SelectShapeP"
#define BinaryThresholdP "BinaryThresholdP"
#define ScaleImageP "ScaleImageP"

//一般参数格式如下
//SelectShapeP=area,and,5000,200000;
//BinaryThresholdP=max_separability,dark;

typedef struct{ 
	SelectShapePARAM selectShape[AA_MAXVISION];  
	BinaryThresholdPARAM binarythreshold[AA_MAXVISION];
	ScaleImagePARAM scaleImage[AA_MAXVISION];


}AA_Globle_Pic_Param;

//公用函数：

void ReadAA_Globle_Pic_ParamValue();
void SaveAA_Globle_Pic_ParamValue(bool save=0);

void ReadAA_selectShape_ParamValue();
void SaveAA_selectShape_ParamValue(bool save=0);

void ReadAA_binarythreshold_ParamValue();
void SaveAA_binarythreshold_ParamValue(bool save=0);

void ReadAA_ScaleImage_ParamValue();
void SaveAA_ScaleImage_ParamValue(bool save=0);


