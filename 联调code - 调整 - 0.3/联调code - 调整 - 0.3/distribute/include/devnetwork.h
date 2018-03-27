
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
	int visionid;
	int robotid1;//通知数据机器人id
	int robotid2;//通知数据机器人id
};


enum notifyEvent
{
	STARTAA,
	RESETAA
};
typedef vector<int> (WINAPI *WorkNotifyFun)(int, AANotifyData, void*);


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