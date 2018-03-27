
#pragma once

#include <string>
using namespace std;
#define PI_AXIS_MAXNUM 6
#define PI_ROBOT_MAXNUM 4
#define NETDEVICESLEN 1000
#define TIMER_EDIT_TEXT         3   //更新编辑框字符串
#define TIMER_SHOW_MSGBOX       9   //延迟弹出消息提示框
#define OnePIcharMaxLen         64

#define PRINTF(msg, ...) printf(msg, __VA_ARGS__)


#define LOG_MAX_BUFF    1024
#define AAROBOTBIND "AAROBOTBIND"
#define AAROBOT1 "robot1"
#define AAROBOT2 "robot2"
#define AAROBOT3 "robot3"
#define AAROBOT4 "robot4"

#define  AA_Z "AAadjust_z"

#define AA_Adjustparam "AA_Adjustparam"
#define AAadjust_zMaxNum "AAadjust_zMaxNum"
#define AA_zzMaxnum  10
#define AAadjust_zthr "AAadjust_zthr"//z阈值

#define  AA_UV "AAadjust_uv"
#define AAadjust_uvMaxNum "AAadjust_uvMaxNum"
#define AA_uvMaxnum  10
#define AAadjust_uvthr "AAadjust_uvthr"//uv阈值

//算法用到的相对固定的尺寸
#define AA_Globle_param "AA_Globle_param" //段
#define AA_focus  "focus" //焦距
#define AA_objdis "objdis" //物距
#define AA_size	"size"    //选框小块尺寸
#define AA_block	"block" //选框大块尺寸
#define AA_edge_size	"edge_size" //拉普拉斯值边距
#define AA_block_size	"block_size" //模板边长的一半
#define AA_pic_width	"pic_width"  //图像大小，是否需要放这里
#define AA_pic_length	"pic_length" //
#define AA_scale	"scale"         //图像缩放比例
#define AA_pixel_size "pixel_size"  // 像素大小
#define AA_t_mapsize	"t_mapsize"  //靶图小黑快大小
#define AA_t_mapdiswidth	"t_mapdiswidth" //靶图长边方向上两个黑快之间的距离
#define AA_t_mapdislength	"t_mapdislength" //靶图短边方向上两个黑快之间的距离


//5点法精调时的z间隔
#define AA_Mirco_disBig 0.006
#define AA_Mirco_disSmall 0.003


//视觉对象定义
#define AA_MAXVISION 8

#define AAVISION_PATH "AAVISION_PATH"
#define AACARERA       "Camera_"

#define AAVISION_MCLK "AAVISION_MCLK"
#define Mclk    "Mclk"

#define AAVISION_POWER "AAVISION_POWER"
#define Avdd "Avdd"
#define Dvdd "Dvdd"
#define Dovdd "Dovdd"
#define Afvcc "Afvcc"
#define Vpp  "Vpp"

#define AAVISION_AF "AAVISION_AF"
#define FocusPos "FocusPos"


#define AAVISION_USERNAME "AAVISION_USERNAME"
#define UserName8 "UserName8"
#define UserName7 "UserName7"
#define UserName6 "UserName6"
#define UserName5 "UserName5"
#define UserName4 "UserName4"
#define UserName3 "UserName3"
#define UserName2 "UserName2"
#define UserName1 "UserName1"

//每个视觉通道的模板可能不一样,也就是选取的中心点不一样,做成界面可配置的形式
#define AAVISION_TEMPLET "AAVISION_TEMPLET"
#define Camera_1 "Camera_1"
#define Camera_2 "Camera_2"
#define Camera_3 "Camera_3"
#define Camera_4 "Camera_4"
#define Camera_5 "Camera_5"
#define Camera_6 "Camera_6"
#define Camera_7 "Camera_7"
#define Camera_8 "Camera_8"



//机器人初始位置设置
#define AA_Robot_InitialPos "AA_Robot_InitialPos" //段
#define AA_Robot1_InitPos  "Robot1_InitPos" //机器人1
#define AA_Robot2_InitPos  "Robot2_InitPos" //机器人1
#define AA_Robot3_InitPos  "Robot3_InitPos" //机器人1
#define AA_Robot4_InitPos  "Robot4_InitPos" //机器人1
//机器人抓取镜头位置
#define AA_Robot_CameraPos "AARobot_CameraPos"
#define AA_Robot1_CameraPos "Robot1_CameraPos"
#define AA_Robot2_CameraPos "Robot2_CameraPos"
#define AA_Robot3_CameraPos "Robot3_CameraPos"
#define AA_Robot4_CameraPos "Robot4_CameraPos"



#define AACoreCfg "AACoreCfg"
#define savepicture "savepicture" //包保存图片也应放在全局参数里去,同AA_TEST_TYPE:0:单摄,1:双摄

#define AA_TEST_TYPE "AA_TEST_TYPE"  //AA_TEST_TYPE:0:单摄,1:双摄

#define AA_DOUBLEAASCALE "doubleAAscale" //双摄的缩小比例
#define AA_DOUBLEAADIS "doubleAAdis" //双摄调整时两个芯片的距离
#define AA_DOUBLEAASIZE "doubleAAsize"  //选框大小
#define AA_DOUBLEAADISWIDTH "doubleAAmapdiswidth" //上下方向距离
#define AA_DOUBLEAADISLENGTH "doubleAAmapdislength" //左右方向距离
#define AA_DOUBLEAAPICCENTERX "doubleAApiccenterX" //中心小块的中心x
#define AA_DOUBLEAAPICCENTERY "doubleAApiccenterY" //中心小块的中心y
#define AA_DOUBLEAACENTROIDTHRESHOLD "doubleAACentroidthreshold" //质心阈值

//图像框布局:1,3行 第一二行3个，第3行1个；上面留5公分,左右下1公分，间距2公分
#define AAVISONTOP 5
#define AAVISONLR 0 //左右 下没有
#define AAVISONDIS 2

enum AATESTTYPE //双摄还是单摄
{
	SINGLEAA,//单摄
	DOUBLEAA//双摄
};

enum INITPOSINDEX //初始位置
{
	AAINDEX1,//接镜头前初始位置
	AAINDEX2//调整前初始位置
};

enum ROBOTINDEX
{
	robotindex1,
	robotindex2,
	robotindex3,
	robotindex4
};


//定义错误码
#define  INVALID_RESULT -0xFFFF
#define AA_OK 0
#define AA_BASE_ERROR_CODE -100
#define AA_GET_CURRENTPT_FAIL AA_BASE_ERROR_CODE-1 //获取当前位置失败
#define AA_GO_RERATIVEPT_FAIL AA_BASE_ERROR_CODE-2  //相对位置失败

#define AA_Z_ADJUST_BIG_FAIL      AA_BASE_ERROR_CODE-20  //Z轴范围太大

//相机错误码
#define AA_GET_PICTIRE_FAIL AA_BASE_ERROR_CODE-100 //获取图像失败

//其他错误码
#define AA_INVALID_INPUT AA_BASE_ERROR_CODE-200 //输入参数错误
#define AA_ADJUST_FAIL AA_BASE_ERROR_CODE-201


typedef struct{ 
	int mapsize;
	int	size;
	int	block;
	int	edge_size;
	int	block_size;
	int	pic_width;
	int	pic_lenth;		
	double focus;
	double objdis;
	double	scale;
	double pixel_size;
	double	mapdiswidth;
	double	mapdislength;
	int temPlate[AA_MAXVISION];
	double robot1InitPos2[PI_AXIS_MAXNUM];
	double robot2InitPos2[PI_AXIS_MAXNUM];
	double robot3InitPos2[PI_AXIS_MAXNUM];
	double robot4InitPos2[PI_AXIS_MAXNUM];

	double robot1InitPos1[PI_AXIS_MAXNUM];
	double robot2InitPos1[PI_AXIS_MAXNUM];
	double robot3InitPos1[PI_AXIS_MAXNUM];
	double robot4InitPos1[PI_AXIS_MAXNUM];

	int AATYPE;
	//双摄调整参数
	double doubleAAscale;
	double doubleAAdis;
	double doubleAACentroidthreshold;
	double doubleAAsize;//选框大小
	double doubleAAmapdiswidth; //离中心点的距离1
	double doubleAAmapdislength;//离中心点的距离2
	double doubleAApiccenterX; //靶图中心x
	double doubleAApiccenterY;//靶图中心y

}AA_Globle_Param;

//pi 的ip和port
typedef struct{ 
	string ip;
	int	port;

}PI_NET_PARAM;

//公用函数：

void ReadAA_Globle_ParamValue();
void SaveAA_Globle_ParamValue(bool save=0);


void ReadAA_InitPos_ParamValue();
void SaveAA_InitPos_ParamValue(double *pos,int num,int robot_idx,int index,bool save=0);

void ReadAAAdjustZValue(double *z,int num);//读取AA调整的z值
void SaveAAAdjustZValue(double *z,int num,bool save= 0);

void ReadAAAdjustUVValue(double *z,int num);//读取AA调整的UV值
void SaveAAAdjustUVValue(double *z,int num,bool save= 0);


 int ParsePIipAndPort(string szdevbuf,PI_NET_PARAM &pinetparam);//解析pi的ip和端口

 int Robot_RedCamPos_toDouble(const char *robot_RedCamPos,double * robot_red_cam_pos );
 double stringToDouble(const string& str);

// void ReadRobotBind(map<int,string >&robotIDmap);
 //void SaveRobotBind(map<int,string >&robotIDmap,bool save= 0);


 string FindbinderUsernameByName(string username);

 void msg(const char* lpszFmt,...);