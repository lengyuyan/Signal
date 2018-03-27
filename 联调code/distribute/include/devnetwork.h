
#pragma once
#ifndef DLL_INCLUDE_SDK_DEVNETWORK_H
#define DLL_INCLUDE_SDK_DEVNETWORK_H


#ifndef WINAPI
#define WINAPI __stdcall
#endif

#ifndef EXPORT_C
#define EXPORT_C extern "C"
#endif

#ifndef NULL
#ifdef __cplusplus
#define NULL    0
#else
#define NULL    ((void *)0)
#endif
#endif

#include<vector>
using namespace std;

struct AANotifyData
{//上报数据，根据NotifyEvent 决定数据格式
	int funCode;
	int robotid1;//通知数据机器人id
	int robotid2;//通知数据机器人id
	//double th[3];//结果: u，v，w
};

struct AANotifyResult
{//上报数据，根据NotifyEvent 决定数据格式
	int funID;  //度信盒编号，或者说是约定的功能号，功能号是比较正常的，但是怕有两个盒子对应一个工位，那么发过来的功能好就不能区别是那个盒子
	int result; //是否异常
	int type;   //调整类型，为了兼容是机器人调整还是轴调整，在hsldlg里面会修改此值
	int isfinsh; //是否还需调整
	double tha[3];//结果: u，v，w
	double thb[3];//结果: u，v，w
};


enum notifyEvent
{
	LIGHT, //点亮模组
	DAA,    //双摄调整
	//下面为旧指令
	STARTAA,
	RESETAA
};

#define INVALID_VALUE -1
#define DATA_SEG_FLAG   ","            //解析符号
#define SPLIT_FLAG      "|"            //分割符号
#define END_CHAR        '#'
#define REPLY_TIMEOUT   5000

#define SENDHEAD "C|"
#define RECVHEAD "S|"


//指令定义

#define LightA "Light"
#define DAAA "AA"
#define StartAAA "StartAA"
#define ResetAAA "ResetAA"





typedef vector<AANotifyResult> (WINAPI *WorkNotifyFun)(int, AANotifyData, void*);


EXPORT_C int WINAPI init_devnetwork();
EXPORT_C void WINAPI uninit_devnetwork();

EXPORT_C void WINAPI test();



/****************************************************
    *Function: 注册数据主动上报回调函数
    *Intput: fun 回调函数地址
             param 附加参数
    *Output: NULL
    *Return: 成功返回0, 失败返回错误码
******************************************************/
EXPORT_C int WINAPI regist_work_notify(WorkNotifyFun fun, void* param);





#endif