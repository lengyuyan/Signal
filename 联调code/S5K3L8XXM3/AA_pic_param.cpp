#include "AA_pic_param.h"
#include"Config.h"

AA_Globle_Pic_Param g_AA_PicParam ;

void ReadAA_Globle_Pic_ParamValue()
{
	ReadAA_selectShape_ParamValue();
	ReadAA_binarythreshold_ParamValue();


}

void SaveAA_Globle_Pic_ParamValue(bool save)
{
	SaveAA_selectShape_ParamValue(save);
	SaveAA_binarythreshold_ParamValue(save);
	SaveAA_ScaleImage_ParamValue(save);
}

//读取形状区域选择参数
void ReadAA_selectShape_ParamValue()
{	
	const char *strPos=CFG_INSTANCE->get_cfg(HALCON_PARAMSETTING,SelectShapeP);
	if (strPos == NULL )
	{
		return ;
	}
	char * paramP=new char[strlen(strPos)+1];
	strcpy(paramP,strPos);
	paramP=strtok(paramP,";");
	char seg[]=",";
	int num=0;
	double temp = 0.0;
	char *substr=strtok(paramP,seg);
	//对于形状选择参数类型固定为char*，char*,double,double,
	int paramNum =4;
	while(substr!=NULL)
	{
		switch(num % paramNum)
		{
		case 0:  //获取形状area,and,5000,200000;
			/*g_AA_PicParam.selectShape[(num / AA_MAXVISION)].Features = substr;*/
			strcpy_s(g_AA_PicParam.selectShape[(num / paramNum)].Features,substr);
			break;
		case 1:
			/*g_AA_PicParam.selectShape[(num / AA_MAXVISION)].Operation = substr;*/
			strcpy_s(g_AA_PicParam.selectShape[(num / paramNum)].Operation,substr);
			break;
		case 2:
			temp=stringToDouble(substr);
			g_AA_PicParam.selectShape[(num / paramNum)].min = temp;
			break;
		case 3:
			temp=stringToDouble(substr);
			g_AA_PicParam.selectShape[(num / paramNum)].max = temp;
			break;
		default:
			break;
		}
		num++;
		substr=strtok(NULL,seg);
	}
	delete []paramP;
}
void SaveAA_selectShape_ParamValue(bool save)
{
	string strpos;
	char temp[128] = {0};
	//填充strpos
	//只需要把g_AA_PicParam.selectShape 放到cfg就行了
	for (int i = 0;i<AA_MAXVISION;i++)
	{
		strpos.append(g_AA_PicParam.selectShape[i].Features);
		strpos.append(",");
		strpos.append(g_AA_PicParam.selectShape[i].Operation);
		strpos.append(",");
		sprintf_s(temp,"%.1f",g_AA_PicParam.selectShape[i].min);
		strpos.append(temp);
		strpos.append(",");
		sprintf_s(temp,"%.1f",g_AA_PicParam.selectShape[i].max);
		strpos.append(temp);
		if (i < AA_MAXVISION-1)
		{
			strpos.append(",");
		}
		
	}
	strpos.append(";");
	CFG_INSTANCE->modify_cfg(HALCON_PARAMSETTING, SelectShapeP,strpos.c_str(),save);
}

void ReadAA_binarythreshold_ParamValue()
{
	const char *strPos=CFG_INSTANCE->get_cfg(HALCON_PARAMSETTING,BinaryThresholdP);
	if (strPos == NULL )
	{
		return ;
	}
	char * paramP=new char[strlen(strPos)+1];
	strcpy(paramP,strPos);
	paramP=strtok(paramP,";");
	char seg[]=",";
	int num=0;
	double temp = 0.0;
	char *substr=strtok(paramP,seg);
	//对于形状选择参数类型固定为char*，char*,double,double,
	int paramNum = 2;
	while(substr!=NULL)
	{
		switch(num % 2)
		{
		case 0:  //max_separability,dark;
			strcpy_s(g_AA_PicParam.binarythreshold[(num / paramNum)].Method,substr);
			break;
		case 1:
			strcpy_s(g_AA_PicParam.binarythreshold[(num / paramNum)].LightDark,substr);
			break;
		default:
			break;
		}
		num++;
		substr=strtok(NULL,seg);
	}
	delete []paramP;

}
void SaveAA_binarythreshold_ParamValue(bool save)
{
	string strpos;
	char temp[128] = {0};
	//填充strpos
	//只需要把g_AA_PicParam.selectShape 放到cfg就行了
	for (int i = 0;i<AA_MAXVISION;i++)
	{
		strpos.append(g_AA_PicParam.binarythreshold[i].Method);
		strpos.append(",");
		strpos.append(g_AA_PicParam.binarythreshold[i].LightDark);
		if (i < AA_MAXVISION-1)
		{
			strpos.append(",");
		}
	}
	strpos.append(";");
	CFG_INSTANCE->modify_cfg(HALCON_PARAMSETTING, BinaryThresholdP,strpos.c_str(),save);
}

void ReadAA_ScaleImage_ParamValue()
{
	const char *strPos=CFG_INSTANCE->get_cfg(HALCON_PARAMSETTING,ScaleImageP);
	if (strPos == NULL )
	{
		return ;
	}
	char * paramP=new char[strlen(strPos)+1];
	strcpy(paramP,strPos);
	paramP=strtok(paramP,";");
	char seg[]=",";
	int num=0;
	double temp = 0.0;
	char *substr=strtok(paramP,seg);
	//对于灰度拉伸选择参数类型固定为double,double,
	int paramNum =2;
	while(substr!=NULL)
	{
		switch(num % paramNum)
		{
		case 0:
			temp=stringToDouble(substr);
			g_AA_PicParam.scaleImage[(num / paramNum)].mul = temp;
			break;
		case 1:
			temp=stringToDouble(substr);
			g_AA_PicParam.scaleImage[(num / paramNum)].add = temp;
			break;
		default:
			break;
		}
		num++;
		substr=strtok(NULL,seg);
	}
	delete []paramP;
}
void SaveAA_ScaleImage_ParamValue(bool save)
{
	string strpos;
	char temp[128] = {0};
	//填充strpos
	//只需要把g_AA_PicParam.selectShape 放到cfg就行了
	for (int i = 0;i<AA_MAXVISION;i++)
	{
		sprintf_s(temp,"%.11f",g_AA_PicParam.scaleImage[i].mul);
		strpos.append(temp);
		strpos.append(",");
		sprintf_s(temp,"%.11f",g_AA_PicParam.scaleImage[i].add);
		strpos.append(temp);
		if (i < AA_MAXVISION-1)
		{
			strpos.append(",");
		}

	}
	strpos.append(";");
	CFG_INSTANCE->modify_cfg(HALCON_PARAMSETTING, ScaleImageP,strpos.c_str(),save);

}