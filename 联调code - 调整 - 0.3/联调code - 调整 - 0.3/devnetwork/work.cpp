

#include <process.h>//开线程函数
#include "work.h"
#include <iostream>
#include <fstream>

#define DEVNETWORKCONFIG devnetwork.json
#define SERVERPORT  50001


NotifyWork g_call; //dlg提供的回调函数



CWork* CWork::m_sInstance = NULL;
CWork* CWork::GetInstance()
{
	if(NULL == m_sInstance)
	{
		m_sInstance = new CWork();
	}
	return m_sInstance;
}

void CWork::ReleaseInstance()
{
	if (m_sInstance)
	{
		delete m_sInstance;
	}

}

CWork::CWork()
{
	InitializeCriticalSection(&vecCS);
	m_hEvtQuit = CreateEvent(NULL,FALSE, TRUE, NULL);//是否手动充值,初始化true,名字

	for (int i=0;i<AAROBOTPROCESSNUM;i++)//两个机器人线程
	{	
		AARobotProcessEvent[i] = CreateEvent(NULL, false, false, NULL);	//不是手动触发,初始状态为false
	}
	//初始化变量,应该用初始化列表的方式
	m_bRun = false;
}

CWork::~CWork(void)
{
	StopWork();
	ReleaseRegistData();



	
	DeleteCriticalSection(&vecCS);  
}

int CWork::parseJsonFile(string filename)
{
	int result = 0;
	long size = 0;
	char *buffer = nullptr;
	struct json_token tokens[100];

	// step 1.1 读配置文件
	ifstream in(filename);
	in.seekg(0, ios::end); //设置文件指针到文件流的尾部
	size = (long)in.tellg();
	if (size <= 0){
		return -1;
	}
	in.seekg (0, ios::beg);

	buffer = new char[size];
	in.read (buffer, size);
	in.close();

	result = parse_json(buffer, size, tokens, sizeof(tokens) / sizeof(tokens[0]));
	if (result < 0){
		return result;
	}

	return result;
}

int CWork::Init()
{
	//开线程处理
	StartWork();


	return 0;

}

int CWork::RegistWorkNotify(WorkNotifyFun fun, void* param)
{
	NotifyWork* pNotify = NULL;
	vector<NotifyWork*>::iterator it;



	if(fun == NULL)return -1;

	EnterCriticalSection(&vecCS);
	for(it = m_vecNotifyWork.begin(); it != m_vecNotifyWork.end(); it++)
	{
		if (fun == (*it)->fun)
		{
			pNotify = *it;
			pNotify->param = param;
		}
	}
	if (!pNotify)
	{
		pNotify = new NotifyWork;
		pNotify->fun = fun;
		pNotify->param = param;
		m_vecNotifyWork.push_back(pNotify);
	}
	LeaveCriticalSection(&vecCS);
	return 0;
}

void CWork::ReleaseRegistData()
{
	vector<NotifyWork*>::iterator it;

	EnterCriticalSection(&vecCS);
	for(it = m_vecNotifyWork.begin(); it != m_vecNotifyWork.end(); it++)
	{
		delete *it;
	}
	m_vecNotifyWork.clear();
	LeaveCriticalSection(&vecCS);
}

vector<int> CWork::NotifyCall(int evt, AANotifyData *pdata)
{
	vector<int>ret ;

	vector<NotifyWork*>::iterator it;

	EnterCriticalSection(&vecCS);
	for (it = m_vecNotifyWork.begin(); it != m_vecNotifyWork.end(); it++)//这里只有一个
	{
		ret = (*it)->fun(evt, *pdata, (*it)->param);
	}	
	LeaveCriticalSection(&vecCS);

	return ret ;
}

void CWork::StartWork()
{
	if(true == m_bRun) return;
	m_bRun = true;
	if (0 == WaitForSingleObject(m_hEvtQuit,INFINITE))
	{
		//pool_thread(WorkThread, this);	

		//第一个线程:启动网络服务
		mg_mgr_init(&mgr, NULL);
		devnethandle = (HANDLE)_beginthreadex( NULL, 0, (unsigned int (__stdcall *)(void *))WorkThread, (LPVOID)this, 0, 0);	
		//第二个线程:1 2号机器人收到指令后启动
		AARobotProcesshandle[0] = (HANDLE)_beginthreadex( NULL, 0, (unsigned int (__stdcall *)(void *))AARobotProcess1, (LPVOID)this, 0, 0);	
		//第二个线程:3 4号机器人收到指令后启动
		AARobotProcesshandle[1] = (HANDLE)_beginthreadex( NULL, 0, (unsigned int (__stdcall *)(void *))AARobotProcess2, (LPVOID)this, 0, 0);	

		//分开开线程是否更好

	}
}

//停止工作，退出
void CWork::StopWork()
{
	if(m_bRun == false)return ;

	m_bRun = false;

	SetEvent(m_hEvtQuit);   //有退出信号	
	//唤醒处理线程，退出
	for (int i = 0;i< AAROBOTPROCESSNUM;i++)
	{
		SetEvent(AARobotProcessEvent[i]);
	}



	for (int i = 0;i< AAROBOTPROCESSNUM;i++)
	{
		WaitForSingleObject(AARobotProcesshandle[i], INFINITE);
		CloseHandle(AARobotProcesshandle[i]);
	}
	
	for (int i = 0;i< AAROBOTPROCESSNUM;i++)//是否需要
	{
		CloseHandle(AARobotProcessEvent[i]);
	}

	WaitForSingleObject(m_hEvtQuit,INFINITE);
	CloseHandle(m_hEvtQuit);

	WaitForSingleObject(devnethandle, INFINITE);
	CloseHandle(devnethandle);

	Sleep(1000);//temp
}

int CWork::Process()
{

	bindAsServer(0,SERVERPORT);//开启网络服务
	while (m_bRun) {

		mg_mgr_poll(getMgr(), 100);
	
		Sleep(50);
	}	
	mg_mgr_free(getMgr());
	

	return 0;
}
int WINAPI CWork::AARobotProcess1(void* param)//先启动线程
{
	CWork* pWork = (CWork*)param;
	string msg;
	int ret =0 ;
	vector<int>result;
	while (pWork->m_bRun)
	{
		WaitForSingleObject(pWork->AARobotProcessEvent[0], INFINITE);
		if (pWork->m_bRun == false)
		{
			return 0;
		}
		//....处理....
		result = pWork->NotifyCall(0, &(pWork->noteMsg[0]));
		struct mg_connection *nc = pWork->getNCList().front();
	
		ret = pWork->composeResult(result,msg,0,STARTAA);
		if (ret == 0)
		{
			list<struct mg_connection *>::iterator iter;
			for(iter = pWork->getNCList().begin() ; iter != pWork->getNCList().end() ; iter++)//把消息发给链接到他上面的客户
			{
				mg_send(*iter,msg.c_str(),msg.length());
			}
			
		}
		

		WaitForSingleObject(pWork->AARobotProcessEvent[0], 0);//立即返回,这里只是测试,可要可不要
		Sleep(50);
	}

	return 0;


}
int WINAPI CWork::AARobotProcess2(void* param)//先启动线程
{

	CWork* pWork = (CWork*)param;
	string msg;
	int ret =0 ;
	vector<int>result;
	while (pWork->m_bRun)
	{


		WaitForSingleObject(pWork->AARobotProcessEvent[1], INFINITE);
		if (pWork->m_bRun == false)
		{
			return 0;
		}
		//....处理....
		result = pWork->NotifyCall(0, &(pWork->noteMsg[1]));

		struct mg_connection *nc = pWork->getNCList().front();
	
		ret = pWork->composeResult(result,msg,1,STARTAA);
		if (ret == 0)
		{
			list<struct mg_connection *>::iterator iter;
			for(iter = pWork->getNCList().begin() ; iter != pWork->getNCList().end() ; iter++)//把消息发给链接到他上面的客户
			{
				mg_send(*iter,msg.c_str(),msg.length());
			}
		}

		WaitForSingleObject(pWork->AARobotProcessEvent[1], 0);//立即返回,这里只是测试,可要可不要
		Sleep(50);
	}

	return 0;
}

int WINAPI CWork::WorkThread(void* param)
{
	CWork* pWork = (CWork*)param;
	return pWork->Process();
}

struct mg_mgr *CWork::getMgr()
{
	return &this->mgr;
}
int CWork::bindAsServer(int type,int port)
{
	struct mg_bind_opts bindOpts = {0};
	struct mg_connection *nc = nullptr;

	memset(&bindOpts, 0, sizeof(bindOpts));

	char cport[10] = {0};
	sprintf(cport,"%d",port);

	switch(type)
	{
	case 0://tcp服务器
		bindOpts.user_data = this;//把自身绑定进去
		nc = mg_bind_opt(&mgr,cport,eventHandler,bindOpts);
		if (NULL == nc)
		{
			return -1;
		}
		break;
	case 1:
		break;
	default:
		break;

	}

	return 0;

}
list<struct mg_connection *> &CWork::getNCList()
{
	return this->ncList;
}
void CWork::setStatus(ConnectStatus sts)
{
	this->status = sts;
}
int CWork::tryToReconnectOrReBind(struct mg_connection *nc)
{
	CWork *work = (CWork *)nc->user_data;

	//if (0 == mg_ncasecmp(CWork->getRole().c_str(), "server", strlen("server")))
	{
		//connectToServer(commDMO);
	}
	//else
	{
		// 对方是client，那这边是server，断开了并不意味着这边是监听的nc断了
		if (nc->flags & MG_F_LISTENING)
		{
			bindAsServer(0,SERVERPORT);//先写死，后续改为配置文件
		}
	}

	return 0;
}
int CWork::composeResult(vector<int>& result,string& msg,int index,int evttype)
{
	int ret = 0;
	msg.clear();
	string res[2];
	
	if (result.size() != 2 || index<0 || index >4)
	{
		return -1;
	}
	for (int i=0;i<result.size();i++)
	{
		if (result[i]== 0)
		{
			res[i] = "1";
		}
		else
		{
			res[i] = "0";
		}
	}

	char visionid[10] = {0};
	

	if (evttype == STARTAA)
	{
		sprintf_s(visionid,"%d",noteMsg[index].visionid);
		msg.append("StartAA,");
	}
	else if (evttype == RESETAA)
	{
		sprintf_s(visionid,"%d",resetMsg.visionid);
		
		msg.append("ResetAA,");
	}
	

	msg.append(visionid);
	msg.append(",");
	msg.append(res[0]);
	msg.append(",");
	msg.append(res[1]);
	msg.append(";");

	/*
	if (noteMsg->visionid>=1 && noteMsg->visionid <= 2)
	{
		msg.append("1,");
		msg.append(res[0]);
		msg.append(",2,");
		msg.append(res[1]);
		msg.append(";");
		
	}
	else	if (noteMsg->visionid>=3 && noteMsg->visionid <=4)
	{
		msg.append("3,");
		msg.append(res[0]);
		msg.append(",4,");
		msg.append(res[1]);
		msg.append(";");

	}*/

	return ret;

}

int CWork::parseAAMsg(int& notemsg,string msg,int len)
{
	//消息格式:StrartAA,1;
	//消息回复格式:StrartAA,1,1,1; 表示1 ok,2 ok
	int ret = 0;
	std::size_t found1 = msg.find(";"); ///先简单验证一下
	if (found1==std::string::npos || (found1 != len-1  && found1 != len-2))  
	{
		ret = -1;
		return ret;
	}
	//找第一个机器人的值
	std::size_t found2 = msg.find(",");
	if (found2==std::string::npos )
	{
		ret = -1;
		return ret;
	}



	string msg2 = msg.substr(found2+1,found1-found2-1);
	notemsg = atoi(msg2.c_str());

	return ret;
}
void CWork::eventHandler(struct mg_connection *nc, int ev, void *ev_data)
{
	char buf[256] = {0};
	int size = 0;	
	struct mbuf *io = &nc->recv_mbuf;
	struct websocket_message *wm = (struct websocket_message *) ev_data;
	CWork *work = (CWork *)nc->user_data;
	
	switch (ev) {
	case MG_EV_POLL:
		break;
	case MG_EV_ACCEPT:
		socket_address *sa;
		sa = (socket_address *)ev_data;
		mg_sock_addr_to_str(sa, buf, sizeof(buf),  MG_SOCK_STRINGIFY_IP | MG_SOCK_STRINGIFY_PORT);
		//可能有别的计算机连上
		printf("accept connection  %#0x from %s.\n", nc, buf);
		work->setStatus(CONNECTED);
		work->getNCList().push_back(nc);
		break;
	case MG_EV_CONNECT:
		if (* (int *) ev_data != 0) {
			
		}
		else{	
			work->setStatus(CONNECTED);
			work->getNCList().push_back(nc);
		}
		break;
	case MG_EV_RECV: //tcp消息
		{
			//定协议：开线程
			//收到消息后发mg_send(nc, msg, msgLen);				

			if (0== strncmp(io->buf,"StartAA",strlen("StartAA")))
			{
				//mg_send(nc, "ok",3);
				char msgTemp[256] = {0};
				int iolen = io->len;
				strncpy_s(msgTemp,io->buf,io->len);

				mbuf_remove(io, io->len);
				//再次验证传过来的数据的正确性
				int notemsg = 0;
				int ret = work->parseAAMsg(notemsg,msgTemp,iolen);

				//mg_send(nc,"StartAA,1,1,2,1;",strlen("StartAA,1,1,2,1;"));
				
				if (ret == 0)
				{			
					if (notemsg ==1 || notemsg ==2)
					{
						work->noteMsg[0].visionid = notemsg;
						work->noteMsg[0].robotid1 = 1;
						work->noteMsg[0].robotid2 = 2;
						SetEvent(work->AARobotProcessEvent[0]);
					}
					else if (notemsg ==3 || notemsg ==4)
					{
						work->noteMsg[1].visionid = notemsg;
						work->noteMsg[1].robotid1 = 3;
						work->noteMsg[1].robotid2 = 4;
						SetEvent(work->AARobotProcessEvent[1]);
					}
					else
					{
						//test
					}
					
				}
				else//格式不合格
				{
					mbuf_remove(io, io->len);
					mg_send(nc, "格式不合格", strlen("格式不合格"));	
				}
				

			}
			if (0== strncmp(io->buf,"ResetAA",strlen("ResetAA")))
			{
				char msgTemp[256] = {0};
				int iolen = io->len;
				strncpy_s(msgTemp,io->buf,io->len);
				mbuf_remove(io, io->len);
				//再次验证传过来的数据的正确性
				int notemsg = 0;
				int ret = work->parseAAMsg(notemsg,msgTemp,iolen);
				if (ret == 0)
				{		//送镜头之前发送一个数字过来,调试这个数字对应的两个机器人使得他们都回原,暂且阻塞的
						work->resetMsg.visionid = notemsg;//无意义,

						if (notemsg == 1 || notemsg ==2)
						{
								work->resetMsg.robotid1 = 1;
								work->resetMsg.robotid2 = 2;
						}
						else if (notemsg == 3 || notemsg ==4)
						{
							work->resetMsg.robotid1 = 3;
							work->resetMsg.robotid2 = 4;
						}
						else
						{
							//test
						}
											
						vector<int>result = work->NotifyCall(RESETAA, &(work->resetMsg));
						struct mg_connection *nc = work->getNCList().front();
						string msg;
						ret = work->composeResult(result,msg,0,RESETAA);
						if (ret == 0)
						{
							list<struct mg_connection *>::iterator iter;
							for(iter = work->getNCList().begin() ; iter != work->getNCList().end() ; iter++)//把消息发给链接到他上面的客户
							{
								mg_send(*iter,msg.c_str(),msg.length());
							}

						}


				}
				else//格式不合格
				{
					mbuf_remove(io, io->len);
					mg_send(nc, "格式不合格", strlen("格式不合格"));	
				}

			}
			else //不接收的命令
			{
				mbuf_remove(io, io->len);
			}

		}
		
		break;
	case MG_EV_CLOSE:
		printf("close connection %#0x.\n", nc);
		work->setStatus(DISCONNECT);
		work->getNCList().remove(nc);
		work->tryToReconnectOrReBind(nc);
		break;	
	case MG_EV_WEBSOCKET_FRAME:
		SYSTEMTIME sys;
		GetLocalTime(&sys);
		memcpy(buf, wm->data, wm->size);
		printf("<< %s  [%02d:%02d;%02d]\n", buf, sys.wHour, sys.wMinute, sys.wSecond);

		if (0 == strncmp((char *)wm->data, "TTT", wm->size))
		{						
			string str = "";
			char temp[256] = {0};
			memset(buf, 0, sizeof(buf));

			if (0)
			{			
			}
			else
			{
				int magic[4];
				char buf[32];
				memset(buf, 0, sizeof(buf));
				for(int i = 0; i < 4; i++){
					magic[i] = rand()%256;
					//magic[i] = magic[i] < 16 ? (magic[i] + 16) : magic[i];
				}
				sprintf(buf, "%02x%02x%02x%02x", magic[0], magic[1], magic[2], magic[3]);
				//str.append("11223344;");
				str.append(buf);
			}

			printf(">> %s\n", str.c_str());
			mg_send_websocket_frame(nc, WEBSOCKET_OP_TEXT, str.c_str(), str.length()); 			
		}		
		break;
	default:
		break;
	}
	
}
void  CWork::test()
{
	/*NotifyData t;
	t.robotid1 = 1;
	NotifyCall(0, &t);*/
}

