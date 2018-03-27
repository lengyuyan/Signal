#include <string>
#include <sstream>
#include"AA_define_tool.h"
#include"Config.h"

using namespace std ;

AA_Globle_Param g_AAParam ={0} ;

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
	CFG_INSTANCE->modify_cfg_int(AA_Globle_param, AA_TEST_TYPE,g_AAParam.AATYPE,save);
	CFG_INSTANCE->modify_cfg_double(AA_Globle_param, AA_DOUBLEAASCALE,g_AAParam.doubleAAscale,save);
	CFG_INSTANCE->modify_cfg_double(AA_Globle_param, AA_DOUBLEAADIS,g_AAParam.doubleAAdis,save);

	CFG_INSTANCE->modify_cfg_double(AA_Globle_param, AA_DOUBLEAASIZE,g_AAParam.doubleAAsize,save);
	CFG_INSTANCE->modify_cfg_double(AA_Globle_param, AA_DOUBLEAADISWIDTH,g_AAParam.doubleAAmapdiswidth,save);
	CFG_INSTANCE->modify_cfg_double(AA_Globle_param, AA_DOUBLEAADISLENGTH,g_AAParam.doubleAAmapdislength,save);
	CFG_INSTANCE->modify_cfg_double(AA_Globle_param, AA_DOUBLEAAPICCENTERX,g_AAParam.doubleAApiccenterX,save);
	CFG_INSTANCE->modify_cfg_double(AA_Globle_param, AA_DOUBLEAAPICCENTERY,g_AAParam.doubleAApiccenterY,save);
	CFG_INSTANCE->modify_cfg_double(AA_Globle_param, AA_DOUBLEAACENTROIDTHRESHOLD,g_AAParam.doubleAACentroidthreshold,save);
		
}

void ReadAA_Globle_ParamValue()
{
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

	//aa测试类型AA_TEST_TYPE 0:单摄 1:双摄
	g_AAParam.AATYPE = CFG_INSTANCE->get_cfg_int(AA_Globle_param, AA_TEST_TYPE);
	g_AAParam.doubleAAdis = CFG_INSTANCE->get_cfg_double(AA_Globle_param, AA_DOUBLEAADIS);
	g_AAParam.doubleAAscale = CFG_INSTANCE->get_cfg_double(AA_Globle_param, AA_DOUBLEAASCALE);
	g_AAParam.doubleAAsize = CFG_INSTANCE->get_cfg_double(AA_Globle_param, AA_DOUBLEAASIZE);
	g_AAParam.doubleAAmapdiswidth = CFG_INSTANCE->get_cfg_double(AA_Globle_param, AA_DOUBLEAADISWIDTH);
	g_AAParam.doubleAAmapdislength = CFG_INSTANCE->get_cfg_double(AA_Globle_param, AA_DOUBLEAADISLENGTH);
	g_AAParam.doubleAApiccenterX = CFG_INSTANCE->get_cfg_double(AA_Globle_param, AA_DOUBLEAAPICCENTERX);
	g_AAParam.doubleAApiccenterY = CFG_INSTANCE->get_cfg_double(AA_Globle_param, AA_DOUBLEAAPICCENTERY);
	g_AAParam.doubleAACentroidthreshold = CFG_INSTANCE->get_cfg_double(AA_Globle_param, AA_DOUBLEAACENTROIDTHRESHOLD);



	

	
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

	Robot_RedCamPos_toDouble(robot1_initpos,r1);
	Robot_RedCamPos_toDouble(robot2_initpos,r2);
	Robot_RedCamPos_toDouble(robot3_initpos,r3);
	Robot_RedCamPos_toDouble(robot4_initpos,r4);

	memcpy(g_AAParam.robot1InitPos2,r1,sizeof(r1));
	memcpy(g_AAParam.robot2InitPos2,r2,sizeof(r1));
	memcpy(g_AAParam.robot3InitPos2,r3,sizeof(r1));
	memcpy(g_AAParam.robot4InitPos2,r4,sizeof(r1));

	//接镜头的位置
	//double r11[PI_AXIS_MAXNUM]= {165.0,		 0.0,		-15.3,		0.0,		0.0,		0.0};
	//double r21[PI_AXIS_MAXNUM] = {0.0,		-163.6,		-15.3,		0.0,		0.0,        90.0};
	//double r31[PI_AXIS_MAXNUM] = {0.0,		162.0,		-15.3,		0.0,		0.0,		90.0};
	//double r41[PI_AXIS_MAXNUM] = {-162.0,		0.0,		-15.3,		0.0,		0.0,		180.0};
	double r11[PI_AXIS_MAXNUM];
	double r21[PI_AXIS_MAXNUM];
	double r31[PI_AXIS_MAXNUM];
	double r41[PI_AXIS_MAXNUM];

	const char *robot1_redcampos=CFG_INSTANCE->get_cfg(AA_Robot_CameraPos,AA_Robot1_CameraPos);
	const char *robot2_redcampos=CFG_INSTANCE->get_cfg(AA_Robot_CameraPos,AA_Robot2_CameraPos);
	const char *robot3_redcampos=CFG_INSTANCE->get_cfg(AA_Robot_CameraPos,AA_Robot3_CameraPos);
	const char *robot4_redcampos=CFG_INSTANCE->get_cfg(AA_Robot_CameraPos,AA_Robot4_CameraPos);

	Robot_RedCamPos_toDouble(robot1_redcampos,r11);
	Robot_RedCamPos_toDouble(robot2_redcampos,r21);
	Robot_RedCamPos_toDouble(robot3_redcampos,r31);
	Robot_RedCamPos_toDouble(robot4_redcampos,r41);

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

int Robot_RedCamPos_toDouble(const char *robot_RedCamPos,double * robot_red_cam_pos )
{
	if (robot_RedCamPos == NULL || robot_red_cam_pos == NULL)
	{
		return -1;
	}
	char * AArebot_redcampos=new char[strlen(robot_RedCamPos)+1];
	strcpy(AArebot_redcampos,robot_RedCamPos);
	AArebot_redcampos=strtok(AArebot_redcampos,";");
	char seg[]=",";
	int num=0;

	char *substr=strtok(AArebot_redcampos,seg);
	while(substr!=NULL)
	{
		double temp=stringToDouble(substr);
		robot_red_cam_pos[num]=temp;
		num++;
		substr=strtok(NULL,seg);
	}
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