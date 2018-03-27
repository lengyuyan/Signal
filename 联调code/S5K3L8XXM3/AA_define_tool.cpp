#include <string>
#include <sstream>
#include"AA_define_tool.h"
#include"Config.h"
#include "AAalgorithm.h"

using namespace std ;
using namespace cv;

bool g_debugwindow = false;
AA_Globle_Param g_AAParam ={0} ;
string g_reginalchannel[] =
{
	"通道1",
	"通道2",
	"通道3",
	"通道4",
	"通道5",
	"通道6",
	"通道7",
	"通道8",
};
string g_reginallocation[] =
{
	"top",
	"bottom",
	"center",
	"left",
	"right",
	"left_loc",
	"right_loc"
};
string g_reginaltype[] =
{
	"矩形",
	/*"矩形2",
	"圆",*/

};

string g_funselect[] =
{
	"功能选择",
	"画区域",
	"计算区域特征点",
	"图像参数设置",
	"halcon过程计算",
	"计算区域面积",
	"计算区域MTF",
	"计算区域角点",
	"更多",

};
string g_picProcesstype[] =
{
	"SelectShape",
	"BinaryThreshold",
	"ScaleImage",
	"更多",

};

string DoubleAA_Bind[AA_MAXVISION][2] = {	
	{Camera_1,Camera_2},
	{Camera_3,Camera_4},
	{Camera_5,Camera_6}, 
	{Camera_7,Camera_8},
	{Camera_2,Camera_1},
	{Camera_4,Camera_3},
	{Camera_6,Camera_5}, 
	{Camera_8,Camera_7}
};

void SaveAA_Globle_ParamValue(bool save)//(AA_Globle_Param& g_aaparam)
{
	CFG_INSTANCE->modify_cfg(AA_Globle_param,"halconprocess",g_AAParam.halconprocess.c_str(),save);
	CFG_INSTANCE->modify_cfg_int(AA_Globle_param, AA_block,g_AAParam.block,save);
	CFG_INSTANCE->modify_cfg_int(AA_Globle_param, AA_block_size,g_AAParam.block_size,save);
	CFG_INSTANCE->modify_cfg_int(AA_Globle_param, AA_edge_size,g_AAParam.edge_size,save);
	CFG_INSTANCE->modify_cfg_double(AA_Globle_param, AA_focus,g_AAParam.focus,save);
	CFG_INSTANCE->modify_cfg_int(AA_Globle_param, AA_t_mapdislength,g_AAParam.mapdislength,save);
	CFG_INSTANCE->modify_cfg_int(AA_Globle_param, AA_t_mapdiswidth,g_AAParam.mapdiswidth,save);
	CFG_INSTANCE->modify_cfg_int(AA_Globle_param, AA_t_mapsize,g_AAParam.mapsize,save);
	CFG_INSTANCE->modify_cfg_double(AA_Globle_param, AA_objdis,g_AAParam.objdis,save);
	CFG_INSTANCE->modify_cfg_int(AA_Globle_param, AA_pic_length,g_AAParam.pic_lenth,save);
	CFG_INSTANCE->modify_cfg_int(AA_Globle_param, AA_pic_width,g_AAParam.pic_width,save);
    CFG_INSTANCE->modify_cfg_double(AA_Globle_param, AA_scale,g_AAParam.scale,save);
	CFG_INSTANCE->modify_cfg_double(AA_Globle_param, AA_pixel_size,g_AAParam.pixel_size,save);
	
	CFG_INSTANCE->modify_cfg_int(AA_Globle_param, AA_size,g_AAParam.size,save);
	//保存每个通道的模板类型
	CFG_INSTANCE->modify_cfg_int(AAVISION_TEMPLET, Camera_1,g_AAParam.temPlate[0],save);
	CFG_INSTANCE->modify_cfg_int(AAVISION_TEMPLET, Camera_2,g_AAParam.temPlate[1],save);
	CFG_INSTANCE->modify_cfg_int(AAVISION_TEMPLET, Camera_3,g_AAParam.temPlate[2],save);
	CFG_INSTANCE->modify_cfg_int(AAVISION_TEMPLET, Camera_4,g_AAParam.temPlate[3],save);
	CFG_INSTANCE->modify_cfg_int(AAVISION_TEMPLET, Camera_5,g_AAParam.temPlate[4],save);
	CFG_INSTANCE->modify_cfg_int(AAVISION_TEMPLET, Camera_6,g_AAParam.temPlate[5],save);
	CFG_INSTANCE->modify_cfg_int(AAVISION_TEMPLET, Camera_7,g_AAParam.temPlate[6],save);
	CFG_INSTANCE->modify_cfg_int(AAVISION_TEMPLET, Camera_8,g_AAParam.temPlate[7],save);

	//保存aa类型
	CFG_INSTANCE->modify_cfg_int(AA_Globle_param, isRobot,g_AAParam.IsRobot,save);
	CFG_INSTANCE->modify_cfg_int(AA_Globle_param, Adjuest_Method,g_AAParam.AdjuestMethod,save);
	CFG_INSTANCE->modify_cfg_int(AA_Globle_param, doubleAA_Unum,g_AAParam.doubleAAUnum,save);
	CFG_INSTANCE->modify_cfg_int(AA_Globle_param, doubleAA_Vnum,g_AAParam.doubleAAVnum,save);
	CFG_INSTANCE->modify_cfg_int(AA_Globle_param, doubleAA_Wnum,g_AAParam.doubleAAWnum,save);
	
	
	
	CFG_INSTANCE->modify_cfg_int(AA_Globle_param, AA_TEST_TYPE,g_AAParam.AATYPE,save);

	CFG_INSTANCE->modify_cfg_double(AA_Globle_param, AA_DOUBLEAAW_LOCDIS,g_AAParam.double_locdis,save);
	CFG_INSTANCE->modify_cfg_double(AA_Globle_param, AA_DOUBLEAAW_LOCANGLE,g_AAParam.double_locangle,save);

	CFG_INSTANCE->modify_cfg_double(AA_Globle_param, doubleAA_Wth,g_AAParam.doubleAAWth,save);
	CFG_INSTANCE->modify_cfg_double(AA_Globle_param, doubleAA_Wth,g_AAParam.doubleAAUth,save);
	CFG_INSTANCE->modify_cfg_double(AA_Globle_param, doubleAA_Wth,g_AAParam.doubleAAVth,save);

	CFG_INSTANCE->modify_cfg_double(AA_Globle_param, AA_DOUBLEAASCALE,g_AAParam.doubleAAscale,save);
	CFG_INSTANCE->modify_cfg_double(AA_Globle_param, AA_DOUBLEAADIS,g_AAParam.doubleAAdis,save);

	CFG_INSTANCE->modify_cfg_double(AA_Globle_param, AA_DOUBLEAASIZE,g_AAParam.doubleAAsize,save);
	CFG_INSTANCE->modify_cfg_double(AA_Globle_param, AA_DOUBLEAADISWIDTH,g_AAParam.doubleAAmapdiswidth,save);
	CFG_INSTANCE->modify_cfg_double(AA_Globle_param, AA_DOUBLEAADISLENGTH,g_AAParam.doubleAAmapdislength,save);
	CFG_INSTANCE->modify_cfg_double(AA_Globle_param, AA_DOUBLEAAPICCENTERX,g_AAParam.doubleAApiccenterX,save);
	CFG_INSTANCE->modify_cfg_double(AA_Globle_param, AA_DOUBLEAAPICCENTERY,g_AAParam.doubleAApiccenterY,save);
	CFG_INSTANCE->modify_cfg_double(AA_Globle_param, AA_DOUBLEAACENTROIDTHRESHOLD,g_AAParam.doubleAACentroidthreshold,save);

	//保存单摄像头的画画中心
	SavePaintParam(0);

	//保存区域选择由全局到 配置内存

}

void ReadAA_Globle_ParamValue()
{
	g_AAParam.iscall_halprocess = false;
	const char* p = NULL;
	p  = CFG_INSTANCE->get_cfg(AA_Globle_param,"halconprocess");
	if (p)
	{
		g_AAParam.halconprocess = p;
	}
	else 
	{
		g_AAParam.halconprocess = "";
	}
	g_AAParam.block = CFG_INSTANCE->get_cfg_int(AA_Globle_param, AA_block);
	g_AAParam.block_size = CFG_INSTANCE->get_cfg_int(AA_Globle_param, AA_block_size);
	g_AAParam.edge_size  = CFG_INSTANCE->get_cfg_int(AA_Globle_param, AA_edge_size);
	g_AAParam.focus   = CFG_INSTANCE->get_cfg_double(AA_Globle_param, AA_focus);
	g_AAParam.mapdislength   = CFG_INSTANCE->get_cfg_int(AA_Globle_param,AA_t_mapdislength);
	g_AAParam.mapdiswidth  = CFG_INSTANCE->get_cfg_int(AA_Globle_param, AA_t_mapdiswidth);
	g_AAParam.mapsize = CFG_INSTANCE->get_cfg_int(AA_Globle_param, AA_t_mapsize);
	g_AAParam.objdis   = CFG_INSTANCE->get_cfg_double(AA_Globle_param, AA_objdis);
	g_AAParam.pic_lenth   = CFG_INSTANCE->get_cfg_int(AA_Globle_param, AA_pic_length);
	g_AAParam.pic_width = CFG_INSTANCE->get_cfg_int(AA_Globle_param, AA_pic_width);
	g_AAParam.scale  = CFG_INSTANCE->get_cfg_double(AA_Globle_param, AA_scale);
	g_AAParam.pixel_size  = CFG_INSTANCE->get_cfg_double(AA_Globle_param, AA_pixel_size);
	g_AAParam.size  = CFG_INSTANCE->get_cfg_int(AA_Globle_param, AA_size);
	
	//读模板模式
	g_AAParam.temPlate[0]  = CFG_INSTANCE->get_cfg_int(AAVISION_TEMPLET, Camera_1);
	g_AAParam.temPlate[1]  = CFG_INSTANCE->get_cfg_int(AAVISION_TEMPLET, Camera_2);
	g_AAParam.temPlate[2]  = CFG_INSTANCE->get_cfg_int(AAVISION_TEMPLET, Camera_3);
	g_AAParam.temPlate[3]  = CFG_INSTANCE->get_cfg_int(AAVISION_TEMPLET, Camera_4);
	g_AAParam.temPlate[4]  = CFG_INSTANCE->get_cfg_int(AAVISION_TEMPLET, Camera_5);
	g_AAParam.temPlate[5]  = CFG_INSTANCE->get_cfg_int(AAVISION_TEMPLET, Camera_6);
	g_AAParam.temPlate[6]  = CFG_INSTANCE->get_cfg_int(AAVISION_TEMPLET, Camera_7);
	g_AAParam.temPlate[7]  = CFG_INSTANCE->get_cfg_int(AAVISION_TEMPLET, Camera_8);

	ReadAA_InitPos_ParamValue();

	//读取区域，暂时只有矩形
	ReadAA_Reginal_ParamValue();

	//aa测试类型AA_TEST_TYPE 0:单摄 1:双摄
	g_AAParam.IsRobot = CFG_INSTANCE->get_cfg_int(AA_Globle_param, isRobot);
	g_AAParam.AATYPE = CFG_INSTANCE->get_cfg_int(AA_Globle_param, AA_TEST_TYPE);
	g_AAParam.doubleAAdis = CFG_INSTANCE->get_cfg_double(AA_Globle_param, AA_DOUBLEAADIS);
	g_AAParam.doubleAAscale = CFG_INSTANCE->get_cfg_double(AA_Globle_param, AA_DOUBLEAASCALE);
	g_AAParam.doubleAAsize = CFG_INSTANCE->get_cfg_double(AA_Globle_param, AA_DOUBLEAASIZE);
	g_AAParam.doubleAAmapdiswidth = CFG_INSTANCE->get_cfg_double(AA_Globle_param, AA_DOUBLEAADISWIDTH);
	g_AAParam.doubleAAmapdislength = CFG_INSTANCE->get_cfg_double(AA_Globle_param, AA_DOUBLEAADISLENGTH);
	g_AAParam.doubleAApiccenterX = CFG_INSTANCE->get_cfg_double(AA_Globle_param, AA_DOUBLEAAPICCENTERX);
	g_AAParam.doubleAApiccenterY = CFG_INSTANCE->get_cfg_double(AA_Globle_param, AA_DOUBLEAAPICCENTERY);
	g_AAParam.doubleAACentroidthreshold = CFG_INSTANCE->get_cfg_double(AA_Globle_param, AA_DOUBLEAACENTROIDTHRESHOLD);
	g_AAParam.doubleAAUth  = CFG_INSTANCE->get_cfg_double(AA_Globle_param, AA_DOUBLEAAU_THRESHOLD);
	g_AAParam.doubleAAVth  = CFG_INSTANCE->get_cfg_double(AA_Globle_param, AA_DOUBLEAAV_THRESHOLD);
	g_AAParam.doubleAAWth  = CFG_INSTANCE->get_cfg_double(AA_Globle_param, AA_DOUBLEAAW_THRESHOLD);

	g_AAParam.double_locdis  = CFG_INSTANCE->get_cfg_double(AA_Globle_param, AA_DOUBLEAAW_LOCDIS);
	g_AAParam.double_locangle  = CFG_INSTANCE->get_cfg_double(AA_Globle_param, AA_DOUBLEAAW_LOCANGLE);


	
	g_AAParam.AdjuestMethod  = CFG_INSTANCE->get_cfg_int(AA_Globle_param, AA_AdjuestMethod);

	g_AAParam.doubleAAUnum  = CFG_INSTANCE->get_cfg_int(AA_Globle_param, AA_DOUBLEAAU_NUM);
	g_AAParam.doubleAAVnum  = CFG_INSTANCE->get_cfg_int(AA_Globle_param, AA_DOUBLEAAV_NUM);
	g_AAParam.doubleAAWnum  = CFG_INSTANCE->get_cfg_int(AA_Globle_param, AA_DOUBLEAAW_NUM);

	//读取款选中心点
	ReadPaintParam();

	

	
}

void ReadAA_Reginal_ParamValue()
{

	double r[4*AA_VISION_RETAGLELOC] = {0};
	const char *pos[AA_MAXVISION] = {0};
	//pos[0]=CFG_INSTANCE->get_cfg(AA_Reginal_param,Channel_Retagle);
	//pos[1]=CFG_INSTANCE->get_cfg(AA_Reginal_param,Channe2_Retagle);
	//pos[2]=CFG_INSTANCE->get_cfg(AA_Reginal_param,Channe3_Retagle);
	//pos[3]=CFG_INSTANCE->get_cfg(AA_Reginal_param,Channe4_Retagle);
	//pos[4]=CFG_INSTANCE->get_cfg(AA_Reginal_param,Channe5_Retagle);
	//pos[5]=CFG_INSTANCE->get_cfg(AA_Reginal_param,Channe6_Retagle);
	//pos[6]=CFG_INSTANCE->get_cfg(AA_Reginal_param,Channe7_Retagle);
	//pos[7]=CFG_INSTANCE->get_cfg(AA_Reginal_param,Channe8_Retagle);
	for (int i = 0;i<AA_MAXVISION;i++)
	{
		string regiontype = "Channe";//1_Retagle
		char indexbuf[10] = {0};	
		itoa(i+1,indexbuf,10);
		regiontype+=indexbuf;
		regiontype += "_Retagle";
		pos[i]=CFG_INSTANCE->get_cfg(AA_Reginal_param,regiontype.c_str());
		ReadPosStr_toDouble(pos[i],r);//把20个数读到r数组中

		/*int count = 0;*/
		for (int loc=0;loc<AA_VISION_RETAGLELOC;loc++)
		{
			g_AAParam.channel1Retangle[i][loc].ltx = r[4*loc];
			g_AAParam.channel1Retangle[i][loc].lty = r[4*loc+1];
			g_AAParam.channel1Retangle[i][loc].rbx = r[4*loc+2];
			g_AAParam.channel1Retangle[i][loc].rby = r[4*loc+3];
			/*count++;*/

		}
		
	}
}

//参数分别表示: 点位位置,点位数量，通道索引，区域类型(暂时只有矩形),是否立即保存
void SaveAA_Reginal_ParamValue(double *pos,int num,int channel_idx,int type,bool save)
{
	//pos-->strpos
	if (pos == NULL)
	{
		return ;
	}

	string strpos;
	char buf[256] = {0};
	for(int i=0;i<num;i++)
	{
		sprintf_s(buf,"%.1f",pos[i]);
		strpos.append(buf);
		strpos.append(",");
	}
	strpos.replace(strpos.find_last_of(","),1,";");

	string regiontype = "Channe";//1_Retagle
	char indexbuf[10] = {0};
	itoa(channel_idx,indexbuf,10);
	regiontype+=indexbuf;
	if (type == AATYPE_RETANGLE1)
	{
		regiontype += "_Retagle";
		CFG_INSTANCE->modify_cfg(AA_Reginal_param, regiontype.c_str(),strpos.c_str(),save);
	}
	else if (type == 1)
	{
		regiontype += "_Circle";
		CFG_INSTANCE->modify_cfg(AA_Reginal_param, regiontype.c_str(),strpos.c_str(),save);
	}

}

void ReadAA_InitPos_ParamValue()
{	//机器人初始位置,先放在程序里 单位mm
	//有两个初始位置,第一个接镜头的位置,第二个调整之前的位置
	//double r1[PI_AXIS_MAXNUM]= {165.0,		 0.0,		-15.3,		0.0,		0.0,		0.0};
	//double r2[PI_AXIS_MAXNUM] = {0.0,		-163.6,		-15.3,		0.0,		0.0,		90.0};
	//double r3[PI_AXIS_MAXNUM] = {0.0,		162.0,		-15.3,		0.0,		0.0,		90.0};
	//double r4[PI_AXIS_MAXNUM] = {-162.0,		0.0,		-15.3,		0.0,		0.0,		180.0};
	double r1[PI_AXIS_MAXNUM];
	double r2[PI_AXIS_MAXNUM];
	double r3[PI_AXIS_MAXNUM];
	double r4[PI_AXIS_MAXNUM];
	const char *robot1_initpos=CFG_INSTANCE->get_cfg(AA_Robot_InitialPos,AA_Robot1_InitPos);
	const char *robot2_initpos=CFG_INSTANCE->get_cfg(AA_Robot_InitialPos,AA_Robot2_InitPos);
	const char *robot3_initpos=CFG_INSTANCE->get_cfg(AA_Robot_InitialPos,AA_Robot3_InitPos);
	const char *robot4_initpos=CFG_INSTANCE->get_cfg(AA_Robot_InitialPos,AA_Robot4_InitPos);

	ReadPosStr_toDouble(robot1_initpos,r1);
	ReadPosStr_toDouble(robot2_initpos,r2);
	ReadPosStr_toDouble(robot3_initpos,r3);
	ReadPosStr_toDouble(robot4_initpos,r4);

	memcpy(g_AAParam.robot1InitPos2,r1,sizeof(r1));
	memcpy(g_AAParam.robot2InitPos2,r2,sizeof(r1));
	memcpy(g_AAParam.robot3InitPos2,r3,sizeof(r1));
	memcpy(g_AAParam.robot4InitPos2,r4,sizeof(r1));


	double r11[PI_AXIS_MAXNUM];
	double r21[PI_AXIS_MAXNUM];
	double r31[PI_AXIS_MAXNUM];
	double r41[PI_AXIS_MAXNUM];

	const char *robot1_redcampos=CFG_INSTANCE->get_cfg(AA_Robot_CameraPos,AA_Robot1_CameraPos);
	const char *robot2_redcampos=CFG_INSTANCE->get_cfg(AA_Robot_CameraPos,AA_Robot2_CameraPos);
	const char *robot3_redcampos=CFG_INSTANCE->get_cfg(AA_Robot_CameraPos,AA_Robot3_CameraPos);
	const char *robot4_redcampos=CFG_INSTANCE->get_cfg(AA_Robot_CameraPos,AA_Robot4_CameraPos);

	ReadPosStr_toDouble(robot1_redcampos,r11);
	ReadPosStr_toDouble(robot2_redcampos,r21);
	ReadPosStr_toDouble(robot3_redcampos,r31);
	ReadPosStr_toDouble(robot4_redcampos,r41);

	memcpy(g_AAParam.robot1InitPos1,r11,sizeof(r11));
	memcpy(g_AAParam.robot2InitPos1,r21,sizeof(r11));
	memcpy(g_AAParam.robot3InitPos1,r31,sizeof(r11));
	memcpy(g_AAParam.robot4InitPos1,r41,sizeof(r11));

}
void SaveAA_InitPos_ParamValue(double *pos,int num,int robot_idx,int index,bool save)
{
	//pos-->strpos
	if (pos == NULL || num > PI_AXIS_MAXNUM)
	{
		return ;
	}
	string strpos;
	char buf[64] = {0};
	for(int i=0;i<num;i++)
	{
		sprintf_s(buf,"%.6f",pos[i]);
		strpos.append(buf);
		strpos.append(",");
	}
	strpos.replace(strpos.find_last_of(","),1,";");
	if (index == AAINDEX2)
	{
		switch(robot_idx)
		{
		case 0:
			CFG_INSTANCE->modify_cfg(AA_Robot_InitialPos, AA_Robot1_InitPos,strpos.c_str(),save);
			break;
		case 1:
			CFG_INSTANCE->modify_cfg(AA_Robot_InitialPos, AA_Robot2_InitPos,strpos.c_str(),save);
			break;
		case 2:
			CFG_INSTANCE->modify_cfg(AA_Robot_InitialPos, AA_Robot3_InitPos,strpos.c_str(),save);
			break;
		case 3:
			CFG_INSTANCE->modify_cfg(AA_Robot_InitialPos, AA_Robot4_InitPos,strpos.c_str(),save);
			break;
		default:
			break;

		}
	}
	else if (index == AAINDEX1)
	{
		switch(robot_idx)
		{
		case 0:
			CFG_INSTANCE->modify_cfg(AA_Robot_CameraPos, AA_Robot1_CameraPos,strpos.c_str(),save);
			break;
		case 1:
			CFG_INSTANCE->modify_cfg(AA_Robot_CameraPos, AA_Robot2_CameraPos,strpos.c_str(),save);
			break;
		case 2:
			CFG_INSTANCE->modify_cfg(AA_Robot_CameraPos, AA_Robot3_CameraPos,strpos.c_str(),save);
			break;
		case 3:
			CFG_INSTANCE->modify_cfg(AA_Robot_CameraPos, AA_Robot4_CameraPos,strpos.c_str(),save);
			break;
		default:
			break;

		}

	}
}
int ReadPaintParam()
{
	string visioncenter = AA_VISION_CENTET;
	string temp;
	char buf[64] = {0};

	const char *visionP = NULL;
	char * visiontemp = NULL;
	for (int  i =0;i < AA_MAXVISION;i++)
	{
		itoa(i+1,buf,10);
		temp=  buf;
		visioncenter += temp;

		visionP=CFG_INSTANCE->get_cfg(AA_Paint,visioncenter.c_str());

		visiontemp=new char[strlen(visionP)+1];
		strcpy(visiontemp,visionP);
		visiontemp = strtok(visiontemp,";");
		char seg[]=",";
		int num=0;

		char *substr=strtok(visiontemp,seg);
		while(substr!=NULL)
		{
			double temp=stringToDouble(substr);
			if (num==0)
			{
				g_AAParam.paintCenter[i].X=temp;
			}
			else if(num==1)
			{
				g_AAParam.paintCenter[i].Y=temp;
			}			
			num++;
			substr=strtok(NULL,seg);
		}	
		visioncenter = AA_VISION_CENTET;
		delete []visiontemp;
	}
	
	return 0;
}

void SavePaintParam(bool save)
{
	string visioncenter = AA_VISION_CENTET;
	string temp;
	char buf[64] = {0};
	string strvision;
	for (int  i =0;i<AA_MAXVISION;i++)
	{
		itoa(i+1,buf,10);
		temp=  buf;
		visioncenter += temp;		
		sprintf_s(buf,"%.1f",g_AAParam.paintCenter[i].X);
		strvision.append(buf);
		strvision.append(",");
		sprintf_s(buf,"%.1f",g_AAParam.paintCenter[i].Y);
		strvision.append(buf);
		strvision.append(";");

		CFG_INSTANCE->modify_cfg(AA_Paint, visioncenter.c_str(),strvision.c_str(),save);
		visioncenter = AA_VISION_CENTET;
		strvision.clear();
	}
	return ;

}

void ReadAAAdjustZValue(double *z,int num)
{
	string aaz = AA_Z;
	string temp;
	char buf[10] = {0};
	for (int  i =0;i<num;i++)
	{
		itoa(i,buf,10);
		temp=  buf;
		aaz += temp;
		z[i] = CFG_INSTANCE->get_cfg_double(AA_Adjustparam, aaz.c_str());
		aaz = AA_Z;
	}


	return ;
}

void SaveAAAdjustZValue(double *z,int num,bool save)
{
	string aaz = AA_Z;
	string temp;
	char buf[10] = {0};
	for (int  i =0;i<num;i++)
	{
		itoa(i,buf,10);
		temp=  buf;
		aaz += temp;
		CFG_INSTANCE->modify_cfg_double(AA_Adjustparam, aaz.c_str(),z[i],save);
		aaz = AA_Z;
	}
	return ;
}

void ReadAAAdjustUVValue(double *uv,int num)
{
	string aauv = AA_UV;
	string temp;
	char buf[10] = {0};
	for (int  i =0;i<num;i++)
	{
		itoa(i,buf,10);
		temp=  buf;
		aauv += temp;
		uv[i] = CFG_INSTANCE->get_cfg_double(AA_Adjustparam, aauv.c_str());
		aauv = AA_UV;
	}


	return ;
}

void SaveAAAdjustUVValue(double *uv,int num,bool save)
{
	string aauv = AA_UV;
	string temp;
	char buf[10] = {0};
	for (int  i =0;i<num;i++)
	{
		itoa(i,buf,10);
		temp=  buf;
		aauv += temp;
		CFG_INSTANCE->modify_cfg_double(AA_Adjustparam, aauv.c_str(),uv[i],save);
		aauv = AA_UV;
	}
	return ;
}

int ParsePIipAndPort(string szdevbuf,PI_NET_PARAM &pinetparam)//依赖pi机器人字符串的格式
{
	if (szdevbuf.empty())
	{
		return AA_INVALID_INPUT;
	}
	int start = szdevbuf.find_last_of("(");
	int ipend = szdevbuf.find_last_of(":");
	int portend = szdevbuf.find_last_of(")");

	if (start == -1 || ipend == -1 ||portend == -1 || start>=ipend || start>=portend)
	{
		return AA_INVALID_INPUT;
	}
	pinetparam.ip = szdevbuf.substr(start+1,ipend-start-1);
	pinetparam.port = stoi(szdevbuf.substr(ipend+1,portend-ipend-1));	
	return AA_OK;
}

int ReadPosStr_toDouble(const char *strPos,double * dpos )
{
	if (strPos == NULL || dpos == NULL)
	{
		return -1;
	}
	char * AArebot_redcampos=new char[strlen(strPos)+1];
	strcpy(AArebot_redcampos,strPos);
	AArebot_redcampos=strtok(AArebot_redcampos,";");
	char seg[]=",";
	int num=0;

	char *substr=strtok(AArebot_redcampos,seg);
	while(substr!=NULL)
	{
		double temp=stringToDouble(substr);
		dpos[num]=temp;
		num++;
		substr=strtok(NULL,seg);
	}

	delete []AArebot_redcampos;
	return 0;
}
double stringToDouble(const string& str)  
{  
	istringstream iss(str);  
	double num;  
	iss >> num;  
	return num;      
} 

string FindbinderUsernameByName(string username)
{
	string bindername;
	for (int i=0;i<AA_MAXVISION;i++)
	{
		for (int j=0;j<2;j++)
		{
			if (DoubleAA_Bind[i][j] == username)
			{
				if (j== 0)
				{
					bindername = DoubleAA_Bind[i][1];
				}
				else
				{
					bindername = DoubleAA_Bind[i][0];
				}
				
			}
		}
	}

	return bindername;

}

//DoubleAA.cpp中的 复制中心坐标到全局变量
void copy_variable(int a,int b,vector<MyPoints> *A,vector<MyPoints> *B)
{
	for( int i=0;i<AA_VISION_RETAGLELOC;i++ )
	{
		g_AAParam.centerpoint[a][2*i] = (*A)[i].row;
		g_AAParam.centerpoint[a][2*i+1] = (*A)[i].col;
	}
	for( int i=0;i<AA_VISION_RETAGLELOC;i++ )
	{
		g_AAParam.centerpoint[b][2*i] = (*B)[i].row;
		g_AAParam.centerpoint[b][2*i+1] = (*B)[i].col;
	}
}