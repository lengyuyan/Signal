#pragma once
#include "mongoose.h"//这个库有很多跟标准库重定义的东西，放在最上方，或者改掉它的重定义的东西
#include <windows.h> 
#include <vector>
#include <list>

#include"devnetwork.h"

using namespace std;

#define  AAROBOTPROCESSNUM 2

enum ConnectStatus{
	DISCONNECT,
	CONNECTED,
};

struct NotifyWork
{
	WorkNotifyFun fun;
	void* param;
};

class CWork
{
public:
	CWork();
	virtual ~CWork(void);

	int Init();
	int parseJsonFile(string filename);
	static CWork* GetInstance();
	static void ReleaseInstance();

	int RegistWorkNotify(WorkNotifyFun fun, void* param);
	void CWork::ReleaseRegistData();
	vector<int> NotifyCall(int evt, AANotifyData *pdata);

	void StartWork();   //开启网络服务
	void StopWork();    //停止工作

	int Process(); 
	static int WINAPI WorkThread(void* param);
	static int WINAPI AARobotProcess1(void* param);
	static int WINAPI AARobotProcess2(void* param);
	

	static void eventHandler(struct mg_connection *nc, int ev, void *ev_data);

	int parseAAMsg(int& notemsg,string msg,int len);
	int composeResult(vector<int>& result,string& msg,int index,int evttype);//index标识组装的是1，2还是3，4
	struct mg_mgr *getMgr();
	int bindAsServer(int type,int port);
	//int connectToServer(int type,char*ip,int port);

	list<struct mg_connection *> &getNCList();
	void setStatus(ConnectStatus sts);
	int tryToReconnectOrReBind(struct mg_connection *nc);

	void  test();

private:
	static CWork* m_sInstance;
	vector<NotifyWork*>     m_vecNotifyWork;    //监控消息注册
	CRITICAL_SECTION vecCS;

	bool    m_bRun;         //是否在运行
	HANDLE  m_hEvtQuit;     //退出信号

	//
	struct mg_mgr mgr;
	list<struct mg_connection *> ncList;
	ConnectStatus status;

	HANDLE devnethandle;
	HANDLE AARobotProcesshandle[AAROBOTPROCESSNUM];

	AANotifyData noteMsg[AAROBOTPROCESSNUM];//StartAA 消息

	AANotifyData resetMsg;//ResetAA 消息

	HANDLE AARobotProcessEvent[AAROBOTPROCESSNUM];//机器人指令处理线程
};