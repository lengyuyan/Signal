
#include "work.h"
#include "devnetwork.h"


#define INSTANCE CWork::GetInstance()
bool g_bInit = false;
EXPORT_C int WINAPI init_devnetwork()
{
	int ret = 0;

	if (g_bInit == true)
	{
		return 0;
	}


	ret = INSTANCE->Init();
	if (!ret)
	{
		g_bInit = true;
	}

	return ret;
}
EXPORT_C void WINAPI uninit_devnetwork()
{
	if(g_bInit == false) return;
	g_bInit = false;
	CWork::ReleaseInstance();
}

EXPORT_C int WINAPI regist_work_notify(WorkNotifyFun fun, void* param)
{
	if (g_bInit!=true)
	{
		return -1;
	}
	return INSTANCE->RegistWorkNotify(fun, param);
}

EXPORT_C void WINAPI test()
{
	return INSTANCE->test();
}