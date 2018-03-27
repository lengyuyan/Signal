// HSFoxxDlg.cpp : implementation file
//4.240 20151116 changed the power on code(bOpenCamera)... to resolve the ov sensor(ov5670)
//4.221 20141202 added the auto load file and recored to hssimple.ini.
//4.220 20141128 only when some wrong or exit, run closedevice()
//4.210 20141127 add disk info check and new dll for enum device....
//4.200 20141119 add device sn read...
//4.100 20141119 add enumrate dothinkey device function EnumerateDothinkeyDevice()
//               test many device ,add the device number to all function parameter...

//4.010 version...
//20141029 add TV code and I2C init function

#include "stdafx.h"
#include "HSFoxx.h"
#include "HSFoxxDlg.h"
#include "threads.h"

#include <windows.h>
#include <winioctl.h>
#include <setupapi.h>
#include <math.h>
#include <process.h>
#include "IniFileRW.h"
#include "jpegfile.h"
#include "AARobotLib.h"


//#include <iostream>
#include <string>
#include "str_tool.h"

#include "Config.h"

#include"AA_PARAAM.h"

#include "AARobot.h"//包括机器人类
//#include <math.h>
//#include <sstream>

#include "devnetwork.h"

#include "AAVision.h"

#include "AAalgorithm.h"
using namespace cv;
using namespace std;



#include"AA_define_tool.h"
 
#pragma comment (lib,"dtccm2.lib")
#pragma comment (lib,"devnetwork.lib")//网络命令通知

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define WM_MSG	WM_USER+1
#define WM_USER_TIMER   WM_USER + 1004  //TIMER消息
#define WM_MSG_NOTIFY WM_USER + 1005



#define ERROR_REUTRN(fmt,...) (msg(fmt,__VA_ARGS__);return;)

extern  AA_Globle_Param g_AAParam;//引入全局变量
extern string DoubleAA_Bind[AA_MAXVISION][2];
//void msg(LPCSTR lpszFmt,...)
void msg(const char* lpszFmt,...)
{
	static CHSFoxxDlg* pDlg = NULL;

	char szTmp[LOG_MAX_BUFF] = {0};
	char szTimeBuf[32] = {0};
	char *szOutBuf = (char*)malloc(LOG_MAX_BUFF*2);

	if (szTmp != NULL)
	{
		int len = 0;
		_SYSTEMTIME sysTime;
		GetLocalTime(&sysTime);
		len  = sprintf_s(szTimeBuf,"%04d-%02d-%02d-%02d:%02d:%02d: ",sysTime.wYear, sysTime.wMonth, sysTime.wDay, sysTime.wHour, sysTime.wMinute, sysTime.wSecond);
		
		va_list argList;
		va_start(argList,lpszFmt);
		len +=_vsnprintf(szTmp,LOG_MAX_BUFF-1,lpszFmt,argList); 
		va_end(argList);



		//len = _snprintf_s(szOutBuf, LOG_MAX_BUFF*2-1, "%s %s %s\n", szTimeBuf, "<INFO>", szTmp);
		len = sprintf_s(szOutBuf,  LOG_MAX_BUFF*2-1,"%s  %s\r\n", szTimeBuf, szTmp);
		if (pDlg == NULL)
		{
			pDlg = (CHSFoxxDlg*)AfxGetMainWnd();
		}

		if (pDlg != NULL)
		{
			if (!PostMessage(pDlg->GetSafeHwnd(),WM_MSG,(WPARAM)szOutBuf,0))
			{
				free(szOutBuf);
			}
		}
		else
		{
			free(szOutBuf);
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

	enum { IDD = IDD_ABOUTBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	
}






BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CHSFoxxDlg dialog

CHSFoxxDlg::CHSFoxxDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CHSFoxxDlg::IDD, pParent)
	, m_fAvdd(2.8f)
	, m_fDovdd(1.8f)
	, m_fDvdd(1.2f)
	, m_fAfvcc(1.2f)
	, m_fVpp(0.0f)
	, m_fMclk(12.0f)
	, m_FocusPos(0)
{
	
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);


	CFG_INSTANCE;//读取配文件
	ReadAA_Globle_ParamValue();


	//双摄同步涉及到的
	InitializeCriticalSection(&m_csRoot12); 
	InitializeCriticalSection(&m_csRoot34); 
	m_intRobot12x = INVALID_RESULT;
	m_intRobot34x = INVALID_RESULT;





	for (int i=0;i<DAAROBOTPROCESSNUM;i++)//机器人处理结果
	{	
		dlgAARobotProcessEvent[i] = CreateEvent(NULL, false, false, NULL);	//不是手动触发,初始状态为false
		
		AARobotWakeResult[i] = CreateEvent(NULL, false, false, NULL);	//不是手动触发,初始状态为false
		AARobotProcessResult[i] = -1;//结果是-1;
		AARobotProcessParam[i] = -1;

		//复位结果事件
		AARobotResetAAWakeResult[i] = CreateEvent(NULL, false, false, NULL);	//不是手动触发,初始状态为false
	}


	

	init_devnetwork();
	m_dlgExit = false;
	mapRobot.clear();

	m_nDevID = -1;
	m_nPiDevID = -1;//当前机器人id

	m_iMoveType = EMOVE_SHORT_STEP;

}

CHSFoxxDlg::~CHSFoxxDlg()
{
	m_dlgExit = true;


	//双摄同步涉及到的
	DeleteCriticalSection(&m_csRoot12); 
	DeleteCriticalSection(&m_csRoot34); 


	//销毁机器人
	auto iter = mapRobot.begin();
	while (iter != mapRobot.end())
	{
		if (iter->second != NULL) 
		{
			delete iter->second ;
			mapRobot.erase(iter++);
		}
		else {
			++iter;
		}
	}
	mapRobot.clear();

	//关闭相机视频
	CloseAAVision();
	DeleteCriticalSection(&m_csLock);
	uninit_devnetwork();
	for (int i = 0;i< DAAROBOTPROCESSNUM;i++)//退出线程
	{
		SetEvent(dlgAARobotProcessEvent[i]);
	}
	for (int i = 0;i< DAAROBOTPROCESSNUM;i++)//等待线程退出,关闭线程句柄
	{
		WaitForSingleObject(AARobotProcesshandle[i], INFINITE);
		CloseHandle(AARobotProcesshandle[i]);
	}
	for (int i = 0;i< DAAROBOTPROCESSNUM;i++)//是否需要
	{
		CloseHandle(dlgAARobotProcessEvent[i]);
		CloseHandle(AARobotWakeResult[i]);

		CloseHandle(AARobotResetAAWakeResult[i]);
	}

	CConfig::ReleaseInstance();



}

void CHSFoxxDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	//视觉的电压值和时钟
	DDX_Text(pDX, IDC_EDITAVDD, m_fAvdd);
	DDX_Text(pDX, IDC_EDITDOVDD, m_fDovdd);
	DDX_Text(pDX, IDC_EDITDVDD, m_fDvdd);
	DDX_Text(pDX, IDC_EDITAFVCC, m_fAfvcc);
	DDX_Text(pDX, IDC_EDITVPP, m_fVpp);
	DDX_Text(pDX, IDC_EDITMCLK, m_fMclk);
	DDX_Text(pDX, IDC_AF_POS, m_FocusPos);

	DDX_Control(pDX, IDC_KITLIST, m_KitListCtrl);//视觉
	DDX_Control(pDX, IDC_PITLIST, m_PiList);//机器人

	//步进值
	DDX_Control(pDX, IDC_STEP_X, m_edtStep[0]);
	DDX_Control(pDX, IDC_STEP_Y, m_edtStep[1]);
	DDX_Control(pDX, IDC_STEP_Z, m_edtStep[2]);
	DDX_Control(pDX, IDC_STEP_U, m_edtStep[3]);
	DDX_Control(pDX, IDC_STEP_V, m_edtStep[4]);
	DDX_Control(pDX, IDC_STEP_W, m_edtStep[5]);

	//当前位置值
	DDX_Control(pDX, IDC_CURX, m_edtVal[0]);
	DDX_Control(pDX, IDC_CURY, m_edtVal[1]);
	DDX_Control(pDX, IDC_CURZ, m_edtVal[2]);
	DDX_Control(pDX, IDC_CURU, m_edtVal[3]);
	DDX_Control(pDX, IDC_CURV, m_edtVal[4]);
	DDX_Control(pDX, IDC_CURW, m_edtVal[5]);

	//方向按钮
	DDX_Control(pDX, IDC_ZX, m_btMoveR[0]);
	DDX_Control(pDX, IDC_FX, m_btMoveL[0]);
	DDX_Control(pDX, IDC_ZY, m_btMoveR[1]);
	DDX_Control(pDX, IDC_FY, m_btMoveL[1]);
	DDX_Control(pDX, IDC_ZZ, m_btMoveR[2]);
	DDX_Control(pDX, IDC_FZ, m_btMoveL[2]);
	DDX_Control(pDX, IDC_ZU, m_btMoveR[3]);
	DDX_Control(pDX, IDC_FU, m_btMoveL[3]);
	DDX_Control(pDX, IDC_ZV, m_btMoveR[4]);
	DDX_Control(pDX, IDC_FV, m_btMoveL[4]);
	DDX_Control(pDX, IDC_ZW, m_btMoveR[5]);
	DDX_Control(pDX, IDC_FW, m_btMoveL[5]);

	//显示日志
	DDX_Control(pDX, IDC_EDIT_LOG, m_editMsg);
	//开视频的几个选项
	DDX_Control(pDX, IDC_PLAY_VISION0, m_playvision[0]);
	DDX_Control(pDX, IDC_PLAY_VISION1, m_playvision[1]);
	DDX_Control(pDX, IDC_PLAY_VISION2, m_playvision[2]);
	DDX_Control(pDX, IDC_PLAY_VISION3, m_playvision[3]);
	DDX_Control(pDX, IDC_PLAY_VISION4, m_playvision[4]);
	DDX_Control(pDX, IDC_PLAY_VISION5, m_playvision[5]);
	DDX_Control(pDX, IDC_PLAY_VISION6, m_playvision[6]);
	DDX_Control(pDX, IDC_PLAY_VISION7, m_playvision[7]);

	DDX_Control(pDX, IDC_VIDEO0, m_wndVideo[0]);
	DDX_Control(pDX, IDC_VIDEO1, m_wndVideo[1]);
	DDX_Control(pDX, IDC_VIDEO2, m_wndVideo[2]);
	DDX_Control(pDX, IDC_VIDEO3, m_wndVideo[3]);
	DDX_Control(pDX, IDC_VIDEO4, m_wndVideo[4]);
	DDX_Control(pDX, IDC_VIDEO5, m_wndVideo[5]);
	DDX_Control(pDX, IDC_VIDEO6, m_wndVideo[6]);
	DDX_Control(pDX, IDC_VIDEO7, m_wndVideo[7]);
	DDX_Control(pDX, IDC_GRAIN, m_grain);
	DDX_Control(pDX, IDC_EXP, m_exp);
}

BEGIN_MESSAGE_MAP(CHSFoxxDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_PLAY, OnPlay)
	ON_BN_CLICKED(IDC_LOAD, OnLoad)
	ON_BN_CLICKED(IDC_OSEASY, OnOseasy)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_SAVEPICTURE, &CHSFoxxDlg::OnBnClickedSavepicture)
	ON_BN_CLICKED(IDC_SETPOWER, &CHSFoxxDlg::OnBnClickedSetpower)
	ON_BN_CLICKED(IDC_SETMCLK, &CHSFoxxDlg::OnBnClickedSetmclk)
	ON_BN_CLICKED(IDC_TEST_ROI, &CHSFoxxDlg::OnBnClickedTestRoi)
	ON_BN_CLICKED(IDC_ENUM, &CHSFoxxDlg::OnBnClickedEnum)
	ON_BN_CLICKED(IDC_FOCUS_POS, &CHSFoxxDlg::OnBnClickedFocusPos)
	ON_BN_CLICKED(IDC_STARTAA, &CHSFoxxDlg::OnBnClickedStartaa)
	
	ON_BN_CLICKED(IDC_GOHOME, &CHSFoxxDlg::OnClickedGoHome)
	ON_EN_CHANGE(IDC_INIT_Y, &CHSFoxxDlg::OnEnChangeInitY)
	ON_BN_CLICKED(IDC_ENUMNETDEVICE, &CHSFoxxDlg::OnBnClickedEnumnetdevice)
	ON_BN_CLICKED(IDC_CONNECT, &CHSFoxxDlg::OnBnClickedConnect)
	ON_BN_CLICKED(IDC_LONGDIS, &CHSFoxxDlg::OnBnClickedLongdis)
	ON_BN_CLICKED(IDC_MIDDIS, &CHSFoxxDlg::OnBnClickedMiddis)
	ON_BN_CLICKED(IDC_SHORTDIS, &CHSFoxxDlg::OnBnClickedShortdis)

	ON_MESSAGE(WM_USER_TIMER, &CHSFoxxDlg::OnMsgTimer)
	ON_MESSAGE(WM_MSG, OnDisplayMsg)
	ON_BN_CLICKED(IDC_CURPOSITION, &CHSFoxxDlg::OnBnClickedCurposition)
	
	ON_BN_CLICKED(IDC_BTN_AAPARAM, &CHSFoxxDlg::OnBnClickedBtnAaparam)
	ON_CBN_SELCHANGE(IDC_PITLIST, &CHSFoxxDlg::OnCbnSelchangePitlist)

	ON_MESSAGE(WM_MSG_NOTIFY, &CHSFoxxDlg::OnMsgNotify)
	ON_BN_CLICKED(IDC_PLAY_VISION0, &CHSFoxxDlg::OnBnClickedPlayVision0)
	ON_BN_CLICKED(IDC_PLAY_VISION1, &CHSFoxxDlg::OnBnClickedPlayVision1)
	ON_BN_CLICKED(IDC_PLAY_VISION2, &CHSFoxxDlg::OnBnClickedPlayVision2)
	ON_BN_CLICKED(IDC_PLAY_VISION3, &CHSFoxxDlg::OnBnClickedPlayVision3)
	ON_BN_CLICKED(IDC_PLAY_VISION4, &CHSFoxxDlg::OnBnClickedPlayVision4)
	ON_BN_CLICKED(IDC_PLAY_VISION5, &CHSFoxxDlg::OnBnClickedPlayVision5)
	ON_BN_CLICKED(IDC_PLAY_VISION6, &CHSFoxxDlg::OnBnClickedPlayVision6)
	ON_BN_CLICKED(IDC_PLAY_VISION7, &CHSFoxxDlg::OnBnClickedPlayVision7)
	
	ON_STN_CLICKED(IDC_VIDEO0, &CHSFoxxDlg::OnStnClickedVideo0)
	ON_STN_CLICKED(IDC_VIDEO1, &CHSFoxxDlg::OnStnClickedVideo1)
	ON_STN_CLICKED(IDC_VIDEO2, &CHSFoxxDlg::OnStnClickedVideo2)
	ON_STN_CLICKED(IDC_VIDEO3, &CHSFoxxDlg::OnStnClickedVideo3)
	ON_STN_CLICKED(IDC_VIDEO4, &CHSFoxxDlg::OnStnClickedVideo4)
	ON_STN_CLICKED(IDC_VIDEO5, &CHSFoxxDlg::OnStnClickedVideo5)
	ON_STN_CLICKED(IDC_VIDEO6, &CHSFoxxDlg::OnStnClickedVideo6)
	ON_STN_CLICKED(IDC_VIDEO7, &CHSFoxxDlg::OnStnClickedVideo7)
	ON_BN_CLICKED(IDC_CONNECTCLOSE, &CHSFoxxDlg::OnBnClickedConnectclose)
	
	ON_BN_CLICKED(IDC_PICTURECENTER, &CHSFoxxDlg::OnBnClickedPicturecenter)
	ON_BN_CLICKED(IDC_READPIC_TEST, &CHSFoxxDlg::OnBnClickedReadpicTest)
	ON_BN_CLICKED(IDC_PI_HOME, &CHSFoxxDlg::OnBnClickedPiHome)
	ON_BN_CLICKED(IDC_SETINITPOS, &CHSFoxxDlg::OnBnClickedSetinitpos)
	ON_BN_CLICKED(IDC_PI_HOME1, &CHSFoxxDlg::OnBnClickedPiHome1)
	ON_BN_CLICKED(IDC_SETINITPOS1, &CHSFoxxDlg::OnBnClickedSetinitpos1)
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_BUTTON1, &CHSFoxxDlg::OnBnClickedButton1)
	ON_WM_HSCROLL()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CHSFoxxDlg message handlers

BOOL CHSFoxxDlg::OnInitDialog()
{
	//CDialog::OnInitDialog();
	CDialog::OnInitDialog();
	// SetBackgroundColor(RGB(180,200,224));
	// Add "About..." menu item to system menu.
	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}
	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here
	m_pSensorPara = new USHORT [8192*4];   //(USHORT*) malloc(8192*sizeof(USHORT));
	m_pSensorSleepPara = new USHORT [2048];   //(USHORT*) malloc(1024*2);
	m_pAF_InitParaList = new USHORT [8192];
	m_pAF_AutoParaList = new USHORT [2048];
	m_pAF_FarParaList = new USHORT [2048];
	m_pAF_NearParaList = new USHORT [2048];
	m_pExposure_ParaList = new USHORT [2048];
	m_pGain_ParaList = new USHORT [2048];
	memset(m_pSensorPara, 0, 8192*4);
	memset(m_pSensorSleepPara, 0, 2048);
	
	memset(&CurrentSensor, 0, sizeof(SensorTab));
//////////////////////////////////////////////////////
	m_RunMode = RUNMODE_STOP;

    m_pTripleBuffer = NULL;
    m_pCameraBuffer = NULL;
    m_pDisplayBuffer = NULL;
	m_hCameraThreadEventDead = NULL;
	m_hDisplayThreadEventDead = NULL;
	m_bTripleBufferDirty = FALSE;

	m_FocusPosTemp=0;
	m_GrabSize = 0;
	m_roi_state = 0;

	EnumerateDothinkeyDevice();
	InitializeCriticalSection(&m_csLock);

	//设置增益 曝光滚动条
	m_grain.SetRange(0, 0x0100);
	m_exp.SetRange(0, 0x005ff);

	m_grain.SetPos(0x0c6);
	m_exp.SetPos(0x0088);

	OnBnClickedEnumnetdevice();

	OnBnClickedConnect();
	// 加载机器人相关内容
	for (int i=0;i<PI_AXIS_MAXNUM;i++)
	{
		m_edtStep[i].SetWindowText("0.01");
	}

	for (int i = 0;i<AA_MAXVISION;i++)
	{
			InitZoomPicture(i);
	}
	
	AARobotProcesshandle[0] = (HANDLE)_beginthreadex( NULL, 0, (unsigned int (__stdcall *)(void *))AARobotProcess1, (LPVOID)this, 0, 0);	
	AARobotProcesshandle[1] = (HANDLE)_beginthreadex( NULL, 0, (unsigned int (__stdcall *)(void *))AARobotProcess2, (LPVOID)this, 0, 0);	
	AARobotProcesshandle[2] = (HANDLE)_beginthreadex( NULL, 0, (unsigned int (__stdcall *)(void *))AARobotProcess3, (LPVOID)this, 0, 0);	
	AARobotProcesshandle[3] = (HANDLE)_beginthreadex( NULL, 0, (unsigned int (__stdcall *)(void *))AARobotProcess4, (LPVOID)this, 0, 0);	
	regist_work_notify(WorkNotify, this);
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CHSFoxxDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CHSFoxxDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CHSFoxxDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}
HWND  CHSFoxxDlg::GetHWndByVisionId(int visionID)
{
	HWND  h = NULL;
	switch(visionID)
	{
	case 0:
		h = m_wndVideo[0].GetSafeHwnd();
		break;
	case 1:
		h = m_wndVideo[1].GetSafeHwnd();
		break;
	case 2:
		h = m_wndVideo[2].GetSafeHwnd();
		break;
	case 3:
		h = m_wndVideo[3].GetSafeHwnd();
		break;
	case 4:
		h = m_wndVideo[4].GetSafeHwnd();
		break;
	case 5:
		h = m_wndVideo[5].GetSafeHwnd();
		break;
	case 6:
		h = m_wndVideo[6].GetSafeHwnd();
		break;
	case 7:
		h = m_wndVideo[7].GetSafeHwnd();
		break;
	default:
		break;

	}
	return h;
}
void CHSFoxxDlg::OnPlay() 
{
	// TODO: Add your control notification handler code here
	int ret = DT_ERROR_OK;
	int nIndex = m_KitListCtrl.GetCurSel();
	int nCount = m_KitListCtrl.GetCount();
	CString str;
	if((nIndex != LB_ERR) && (nCount >0))
	{
		m_KitListCtrl.GetLBText(nIndex,str);
	}
	else
	{
		msg("没有设备！");
		return;
	}
	string strTemp = str.GetBuffer();

	map<string,CAAVision*>::iterator it;
	it = mapVision.find(strTemp);
	if (it != mapVision.end())
	{
		if (m_nDevID>=0 && m_nDevID != it->second->m_iCurIDA)
		{
			//先关闭先前这个视频，在多个窗口显示也可以不关闭试试
			if (it->second->m_iWorkStatusA == TESTOR_START)
			{
				ret = it->second->CloseVisionVideo();
			}

		}
		m_nDevID = it->second->m_iCurIDA;
	}
	else
	{
		msg("vision(%s)新插上去的，要新生成一个对象,后续在完善！",strTemp.c_str());
		return ;
	}
	CAAVision *vision = it->second;
	CString caption;
	GetDlgItemText(IDC_PLAY,caption);
	if (caption.CompareNoCase("Play") == 0)//
	{	
		if (vision->m_iWorkStatusA ==  TESTOR_START)//视频已经打开
		{
			msg("vision(%s)视频已经打开！",strTemp.c_str());
			SetDlgItemText(IDC_PLAY, "Stop");
			return;
		}

		if(DT_ERROR_OK == IsDevConnect(m_nDevID))  //if old device is online,close it...
		{
			msg("vision(%s)设备已经打开！",strTemp.c_str());

			//根据m_nDevID来传递不同的hwnd;
			HWND h = GetHWndByVisionId(m_nDevID);
			ret = vision->OpenVisionVideo(h);

			SetDlgItemText(IDC_PLAY, "Stop");
			//CloseDevice(m_nDevID);
		}
		else
		{
			HWND h = GetHWndByVisionId(m_nDevID);
			ret = vision->OpenVisionDevice();
			ret = vision->OpenVisionVideo(h);
			SetDlgItemText(IDC_PLAY, "Stop");
		}

	}
	else
	{
		SetDlgItemText(IDC_PLAY, "Play");
		if (vision->m_iWorkStatusA ==  TESTOR_START)//视频已经打开
		{
			ret = vision->CloseVisionVideo();
			msg("vision(%s)视频被关闭！",strTemp.c_str());
			return;
		}
		else
		{
			msg("vision(%s)视频已经关闭！",strTemp.c_str());
		}	

	}

}
//20141119 added by leedo.
//tell you how to enumrate the dothinkey device...
int CHSFoxxDlg::EnumerateDothinkeyDevice()
{
	int i;
	int DevNum = 0;
	char snname[256] = {0};
	CString stmp;
	char* pDeviceName[AA_MAXVISION] = {NULL};

	if(CAAVision::EnumerateAAVision(pDeviceName, AA_MAXVISION, &DevNum) != DT_ERROR_OK)
	{
		MessageBox("枚举操作失败!");
		return -1;
	}
	for(i = 0; i < DevNum; i++)
	{
		m_DevName[i].Empty();
		if(pDeviceName[i])
		{
			map<string,CAAVision*>::iterator it;
			string visionName = pDeviceName[i];//是否名字改为自定义的名字
			it = mapVision.find(visionName);
			if (it == mapVision.end())
			{
				
				string definename = visionName.substr(visionName.find("@")+1);//找到定义的名字
				
				int index  = definename.find_last_of("_");//提前设定每个通道的名字为xx@camera_i的形式
				string temp = definename.substr(index+1);
				int defineid = std::stoi(temp);
				CAAVision *vision = new CAAVision(visionName,defineid-1);
				//SensorTab pCurrentSensor;
				string path = CFG_INSTANCE->get_cfg(AAVISION_PATH,definename.c_str());
				msg("视觉%s, 参数文件路径:%s",definename.c_str(),path.c_str());
				if(!LoadLibIniFile(&CurrentSensor,path))//暂时用CurrentSensor
				{
					msg("Load Current Sensor Failed ,name:%s!",visionName.c_str());
				}
				else
				{
					vision->SetSensorTab(CurrentSensor);
				}		
				mapVision.insert(map<string,CAAVision*>::value_type(visionName,vision));			
			}

			m_DevName[i].Format("%s", pDeviceName[i]);
			GlobalFree(pDeviceName[i]);
			pDeviceName[i] = NULL;
		}
	}
	m_KitListCtrl.ResetContent();
	if(DevNum >0)
	{
		for(i = 0; i < DevNum; i++)
		{
			stmp.Format("%s", m_DevName[i]);

			m_KitListCtrl.AddString(stmp);	
		}
		m_KitListCtrl.SetCurSel(0);
	}

	UpdateData(FALSE);
	return DevNum;
}
/*
枚举设备，并且打开设备。
*/
BOOL CHSFoxxDlg::bOpenDevice()
{
	if(m_KitListCtrl.GetCount() <= 0)
	{
		EnumerateDothinkeyDevice();
		if(m_KitListCtrl.GetCount() <= 0)
		{
			return FALSE;
		}
	}

	if(m_nDevID != m_KitListCtrl.GetCurSel())
	{
		if (m_nDevID >= 0)
		{
			if(DT_ERROR_OK == IsDevConnect(m_nDevID))  //if old device is online,close it...
			{
				CloseDevice(m_nDevID);
			}
		}
	}
	else if (m_nDevID >= 0)
	{
		if(DT_ERROR_OK == IsDevConnect(m_nDevID))  //if current device is online ,returan directly...
		{
			return TRUE;
		}
	}
	int nCurDevID = m_KitListCtrl.GetCurSel();
	int nRetID = 0;


	//打开当前选择的。
	if (OpenDevice(m_DevName[nCurDevID], &nRetID, nCurDevID) != DT_ERROR_OK)
	{
		CloseDevice(nCurDevID);
		AfxMessageBox("Open Device failed...");
		return FALSE;
	}
	m_nDevID = nCurDevID;
	return TRUE;
}


BOOL CHSFoxxDlg::CloseAAVision()
{
	int ret = DT_ERROR_OK;
	auto iter = mapVision.begin();
	while (iter != mapVision.end())
	{
		if (iter->second != NULL) 
		{
			if (iter->second->m_iWorkStatusA == TESTOR_START)
			{
				ret = iter->second->CloseVisionVideo();
				if (DT_ERROR_OK != ret)
				{
					//打印日志
				}
				iter->second->CloseVisionDevice();
				if (DT_ERROR_OK != ret)
				{
					//打印日志
				}

			}else
			{
				iter->second->CloseVisionDevice();
				if (DT_ERROR_OK != ret)
				{
					//打印日志
				}

			}
			iter++;
		}
	}

	//销毁视觉对象
	iter = mapVision.begin();
	while (iter != mapVision.end())
	{
		if (iter->second != NULL) 
		{
			delete iter->second ;
			mapVision.erase(iter++);
		}
		else {
			++iter;
		}
	}
	mapVision.clear();

	return ret;
}

/*
执行关闭视频的一些操作
*/
BOOL CHSFoxxDlg::bCloseCamera()
{
	//firstly close device:set fx2 to sleep.
	if(m_RunMode == RUNMODE_STOP)
		return FALSE;
	m_RunMode = RUNMODE_STOP;

	KillTimer(1);
	   
	if(m_hCameraThreadEventDead)
	{
		WaitForSingleObject(m_hCameraThreadEventDead, INFINITE);
		CloseHandle(m_hCameraThreadEventDead);
		m_hCameraThreadEventDead = NULL;
	}

	if(m_hDisplayThreadEventDead)
	{
		WaitForSingleObject(m_hDisplayThreadEventDead, INFINITE);
		CloseHandle(m_hDisplayThreadEventDead);
		m_hDisplayThreadEventDead = NULL;
	}
	
	if(1)
	{
		ResetSensorI2cBus(m_nDevID);
		SensorEnable(CurrentSensor.pin ^ 2, 1, m_nDevID);       //reset sensor....
		Sleep(2);
		CloseVideo(m_nDevID);
		
		SensorEnable(CurrentSensor.pin ^ 1, 1, m_nDevID);       //power down sensor....
	}
	//close all power....
	{
		//close the clock
		SetSensorClock(0, 24*10, m_nDevID);
		//close the IO power...
		SetSoftPinPullUp(IO_NOPULL, m_nDevID);
		//close reset and power down pin...
		SensorEnable(0, 1, m_nDevID);

		//关闭电压
		SENSOR_POWER Power[10];
		BOOL OnOff[10] = {0};	
		int Volt[10] = {0};

		Power[0] = POWER_AVDD;
		Power[1] = POWER_DOVDD;
		Power[2] = POWER_DVDD;
		Power[3] = POWER_AFVCC;
		Power[4] = POWER_VPP;

		Volt[0] = 0;
		Volt[1] = 0;
		Volt[2] = 0;
		Volt[3] = 0;
		Volt[4] = 0;


		OnOff[0] = 0;
		OnOff[1] = 0;
		OnOff[2] = 0;
		OnOff[3] = 0;
		OnOff[4] = 0;


		//close the power switch...
		//if necessory, you can set the power voltage to 0
		PmuSetVoltage(Power, Volt, 5, m_nDevID);
		Sleep(50);
		PmuSetOnOff(Power,OnOff,5,m_nDevID);

		//以下将所有跟sensor有关的电源包括IO口，MIPI口的电压都关闭。
# if 1  
		int HighLimit[45] = {0};
		int LowLimit[45] = {0};
		int VoltageH[45] = {0};
		int VoltageL[45] = {0};

		//Here　is value...
		USHORT OpenStd = 0x1ff-50;  //0x400 equal to 3V...
		USHORT ShortStd = 50;

		//这里完成一个转换。其实这里可以每一个PIN都设立一个标准。。
		int i;

		for(i = 0; i < 45; i++)
		{
			HighLimit[i] = (int)(OpenStd *1.5f *1000000/512) ; //转变为uV信号	
			LowLimit[i] =  (int)(ShortStd *1.5f*1000000/512) ;
		}

		//dtkdemo.exe use 2*1000 uA for os test. so maybe some different.

		//以下将测试板置于OS模式 未知，先注释gpz
		//OS_Config((int)(1.5f*1000*1000), HighLimit, LowLimit, 45, 2*1000, 1*1000, m_nDevID);	
#endif
	}
	
    KillDataBuffer();

	return TRUE;
}

void CHSFoxxDlg::KillDataBuffer()
{
	//delete the data buffer.
	if (m_pTripleBuffer)
	{
		free(m_pTripleBuffer);
	}
	if (m_pCameraBuffer)
	{
		free(m_pCameraBuffer);
	}
	if (m_pDisplayBuffer)
	{
		free(m_pDisplayBuffer);
	}
	
	m_pTripleBuffer = NULL;
	m_pCameraBuffer = NULL;
	m_pDisplayBuffer = NULL;
	
}

int CHSFoxxDlg::DTOS_Test()
{
	DWORD dwKitType = (GetKitType(m_nDevID)&0xFF);
	if(dwKitType  < 0x90)
		return -1;

	//设置电压，电流
	SENSOR_POWER Power[10];
	int Volt[10];
	BOOL OnOff[10];

	Power[0] = POWER_AVDD;
	Volt[0] = 1500; // 2.8V
	OnOff[0] = FALSE;

	Power[1] = POWER_DOVDD;
	Volt[1] = 1500; // 1.8V
	OnOff[1] = FALSE;

	Power[2] = POWER_DVDD;
	Volt[2] = 1500;// 1.2V
	OnOff[2] = FALSE;

	Power[3] = POWER_AFVCC;
	Volt[3] = 1500; // 2.8V
	OnOff[3] = FALSE;

	Power[4] = POWER_VPP;
	Volt[4] = 1500; 
	OnOff[4] = FALSE;
	//设置5路电压值
	PmuSetVoltage(Power, Volt, 5, m_nDevID);
	PmuSetOnOff(Power, OnOff, 5, m_nDevID);
//20130703 changed....
	//os test....
	//开短路测试是否通过的结果存到Result中,对应值非0代表NG。
	UCHAR Result[45] = {0};
	//传递给设备的IOMask值。
	//IoMask[0]  PIN7:0;
	//IoMask[1]  PIN15:8;
	//IoMask[2]  PIN23:16;
	//IoMask[3]  PIN31:24;
	//IoMask[4]  ....
	//
	UCHAR IoMask[10] = {0};
	//这里对应你要设置的PIN。
	//m_PinMask[31:0] PIN31:0 enable or not. 1:enalbe
	//m_PinMaskH[44:32] PIN44:32 enable or not...
	ULONG m_PinMask = 0xffffffff;  //you can setup this...
	ULONG m_PinMaskH = 0x01fff;

	//Here　is value...
	USHORT m_OpenStd = 0x1ff-50;  //0x400 equal to 3V...
	USHORT m_ShortStd = 50;

	int HighLimit[45] = {0};
	int LowLimit[45] = {0};
	int VoltageH[45] = {0};
	int VoltageL[45] = {0};

    //这里完成一个转换。其实这里可以每一个PIN都设立一个标准。。
	int i, j;
	for(i = 0; i < 45; i++)
	{
		HighLimit[i] = (int)(m_OpenStd *1.5f *1000000/512) ; //转变为uV信号	
		LowLimit[i] =  (int)(m_ShortStd *1.5f*1000000/512) ;
	}
	IoMask[0] = (UCHAR)((m_PinMask) & 0xff);
	IoMask[1] = (UCHAR)((m_PinMask>>8) & 0xff);
	IoMask[2] = (UCHAR)((m_PinMask>>16) & 0xff);
	IoMask[3] = (UCHAR)((m_PinMask>>24) & 0xff);

	IoMask[4] = (UCHAR)((m_PinMaskH) & 0xff);
	IoMask[5] = (UCHAR)((m_PinMaskH>>8) & 0xff);
	IoMask[6] = (UCHAR)((m_PinMaskH>>16) & 0xff);
	IoMask[7] = (UCHAR)((m_PinMaskH>>24) & 0xff);
	//dtkdemo.exe use 20*1000 uA for os test. so maybe some different.
	OS_Config((int)(1.5f*1000*1000), HighLimit, LowLimit, 45, 2*1000, 1*1000, m_nDevID);	
	//OS 测试，进行该PIN为low的测试。
	//command bit7 :enalble os ; bit3:high test; bit2:low test...
	LC_OS_CommandConfig(0x80 | 0x08 | 0x04, IoMask, 10, m_nDevID);
	//读取OS测试的结果
	if(DT_ERROR_OK == OS_Read(VoltageH, VoltageL, Result, 1, 1, 45 , m_nDevID))
	{
		CString sOSInfo, stmp;
		sOSInfo.Empty();

		//GND1,2,3,4 short error.normally is because of GND connected. not signals.
		for(i = 5; i < 9; i++)
		{
			if(Result[i] && (VoltageL[i] <= LowLimit[i]))
			{
				Result[i] = 0;
			}
		}
		//to show the the 
		j = 0;
		for(i = 0; i <45; i++)
		{
			if((VoltageL[i] != 0xffff*1000  ) /*&& Result[i]*/)
			{
				stmp.Format("%02d, Vol=%.3f (v)\n", i, VoltageL[i]/1000000.0f);	

				sOSInfo += stmp;

				stmp.Format("%02d, Vol=%.3f (v)\n", i, VoltageH[i]/1000000.0f);	
	
				sOSInfo += stmp;
				j++;
			}
		}
		
		if(j)
		{
			//m_show.AddString("OS test: fail");
		}
		else
		{
			//m_show.AddString("OS test: PASS");
		}
		
		return (j == 0);
	}
	return -1;

}
//////////////////////////////////////////////////////////////////////////
// Get AD value from demokit....
//
//
//////////////////////////////////////////////////////////////////////////

/*
定时器查询，将获取到的值显示；
*/
void CHSFoxxDlg::OnTimer(UINT_PTR nIDEvent)
{
	CHSFoxxDlg* pDlg = NULL;
	DWORD dwKitType = (GetKitType(m_nDevID)&0xFF);
	if(dwKitType  < 0x90)
		return ;
	if (nIDEvent == 1)
	{
		GetADValue();
	}
	else if(nIDEvent == 5)
	{

	}
	CDialog::OnTimer(nIDEvent);
}

/*
获取每个电压通道的电流值；
*/
void CHSFoxxDlg::GetADValue()
{
	CHSFoxxDlg* pDlg = NULL;
	DWORD dwKitType = (GetKitType(m_nDevID)&0xFF);
	if(dwKitType  < 0x90)
		return ;
	CString m_sInfo;

	SENSOR_POWER Power[5];
	int Current[5] = {0};

	Power[0] = POWER_AVDD;
	Power[1] = POWER_DOVDD;
	Power[2] = POWER_DVDD;
	Power[3] = POWER_AFVCC;
	Power[4] = POWER_VPP;
	PmuGetCurrent(Power, Current, 5, m_nDevID);

	if(m_RunMode == RUNMODE_STOP)
	{
		m_sInfo.Format("Iav:%.1f;Ido:%.1f;Idv:%.1f;Iaf:%.1f;Ivp:%.1f(uA)", Current[0]/1000.0f, Current[1]/1000.0f, Current[2]/1000.0f, Current[3]/1000.0f, Current[4]/1000.0f);
	}
	else
	{
		m_sInfo.Format("Iav:%.1f;Ido:%.1f;Idv:%.1f;Iaf:%.1f;Ivp:%.1f(mA)", Current[0]/1000000.0f, Current[1]/1000000.0f, Current[2]/1000000.0f, Current[3]/1000000.0f, Current[4]/1000000.0f);
	}


	//GetDlgItem(IDC_INFO)->SetWindowText(m_sInfo);
	//when you get string.. you can show it....

}

BOOL CHSFoxxDlg::TripleBufferFlip(LPBYTE *pBuf, BOOL bPrimaryBuffer)
{
    BOOL bFlipped = FALSE;
	
    EnterCriticalSection(&m_csLock);
    {
        if (bPrimaryBuffer || m_bTripleBufferDirty)
        {
            LPBYTE pTmp = *pBuf;
            *pBuf = m_pTripleBuffer;
            m_pTripleBuffer = pTmp;
            bFlipped = TRUE;
        }
        m_bTripleBufferDirty = bPrimaryBuffer;
    }
    LeaveCriticalSection(&m_csLock);
	
    return bFlipped;	
}

void CHSFoxxDlg::OnLoad() 
{
	// TODO: Add your control notification handler code here
	if(!bLoadLibIniFile(""))
	{
		AfxMessageBox("Load Failed!");
	}
}

BOOL CHSFoxxDlg::LoadLibIniFile(pSensorTab pCurrentSensor,string filename)
{
	CString sfilename = _T("");  

	if (filename == "")
	{
		return AA_INVALID_INPUT;
	}
	CString cstringtemp;
	cstringtemp.Format("%s", filename.c_str());    
	SetIniFileName(cstringtemp);
		sfilename = cstringtemp;


	pCurrentSensor->width    = ReadIniData("Sensor","width",0);
	pCurrentSensor->height   = ReadIniData("Sensor","height",0);
	pCurrentSensor->type     = ReadIniData("Sensor","type",2);

	pCurrentSensor->port = ReadIniData("Sensor", "port", 0);
	pCurrentSensor->pin = ReadIniData("Sensor", "pin", 0);

	pCurrentSensor->SlaveID  = ReadIniData("Sensor", "SlaveID", 0);
	pCurrentSensor->mode     = ReadIniData("Sensor", "mode", 0);
	pCurrentSensor->FlagReg  = ReadIniData("Sensor", "FlagReg", 0);
	pCurrentSensor->FlagMask = ReadIniData("Sensor", "FlagMask", 0xff);
	pCurrentSensor->FlagData = ReadIniData("Sensor", "FlagData", 0);

	pCurrentSensor->FlagReg1  = ReadIniData("Sensor", "FlagReg1", 0);
	pCurrentSensor->FlagMask1 = ReadIniData("Sensor", "FlagMask1", 0x0);
	pCurrentSensor->FlagData1 = ReadIniData("Sensor", "FlagData1", 0);

	pCurrentSensor->outformat= ReadIniData("Sensor", "outformat", 0x00);
	pCurrentSensor->mclk     = ReadIniData("Sensor", "mclk", 0x01);

	pCurrentSensor->avdd     = ReadIniData("Sensor", "avdd", 0x00);
	pCurrentSensor->dovdd     = ReadIniData("Sensor", "dovdd", 0x00);
	pCurrentSensor->dvdd     = ReadIniData("Sensor", "dvdd", 0x00);

	ClearIniFileName();

	pCurrentSensor->ParaList = NULL;
	pCurrentSensor->ParaListSize = 0;
	pCurrentSensor->SleepParaList = NULL;
	pCurrentSensor->SleepParaListSize = NULL;

	bGetI2CDataFromLibFile(sfilename, pCurrentSensor);
	if( (pCurrentSensor->width==0)       ||
		(pCurrentSensor->height==0)      ||
		(pCurrentSensor->ParaList==NULL) ||
		(pCurrentSensor->ParaListSize==0)	  )
	{
		return FALSE;
	}

	return TRUE;
}

BOOL CHSFoxxDlg::bLoadLibIniFile(CString sfilename)
{
	sfilename.Trim();
	if(sfilename.IsEmpty())
	{
		CString sfilter="Ini File(*.ini)|*.ini";
		CFileDialog dlg(TRUE, NULL, sfilename, OFN_HIDEREADONLY, sfilter);
		if(dlg.DoModal() != IDOK)
		{
			return FALSE;
		}
		else
		{
			sfilename = dlg.GetPathName();
			if(sfilename.IsEmpty())
			{
				return FALSE;
			}
		}
	}
	SetIniFileNameInExePath("hisimple.ini");
	WriteIniString("Last Load File", "FileName", sfilename);
	ClearIniFileName();

	//20141202 added....
	CString sWindowText;
	CString sbuf;
	this->GetWindowText(sWindowText);
	AfxExtractSubString(sbuf, sWindowText, 0,  '(');
	sWindowText.Format("%s(%s)", sbuf, sfilename);
	this->SetWindowText(sWindowText);
	//end added...

	SensorTab NewSensor;
	memset(&NewSensor,0,sizeof(SensorTab));
	SetIniFileName(sfilename);


	NewSensor.width    = ReadIniData("Sensor","width",0);
	NewSensor.height   = ReadIniData("Sensor","height",0);
	NewSensor.type     = ReadIniData("Sensor","type",2);

	NewSensor.port = ReadIniData("Sensor", "port", 0);
	NewSensor.pin = ReadIniData("Sensor", "pin", 0);

	NewSensor.SlaveID  = ReadIniData("Sensor", "SlaveID", 0);
	NewSensor.mode     = ReadIniData("Sensor", "mode", 0);
	NewSensor.FlagReg  = ReadIniData("Sensor", "FlagReg", 0);
	NewSensor.FlagMask = ReadIniData("Sensor", "FlagMask", 0xff);
	NewSensor.FlagData = ReadIniData("Sensor", "FlagData", 0);

	NewSensor.FlagReg1  = ReadIniData("Sensor", "FlagReg1", 0);
	NewSensor.FlagMask1 = ReadIniData("Sensor", "FlagMask1", 0x0);
	NewSensor.FlagData1 = ReadIniData("Sensor", "FlagData1", 0);

	NewSensor.outformat= ReadIniData("Sensor", "outformat", 0x00);
	NewSensor.mclk     = ReadIniData("Sensor", "mclk", 0x01);

	NewSensor.avdd     = ReadIniData("Sensor", "avdd", 0x00);
	NewSensor.dovdd     = ReadIniData("Sensor", "dovdd", 0x00);
	NewSensor.dvdd     = ReadIniData("Sensor", "dvdd", 0x00);

	ClearIniFileName();

	NewSensor.ParaList = NULL;
	NewSensor.ParaListSize = 0;
	NewSensor.SleepParaList = NULL;
	NewSensor.SleepParaListSize = NULL;

	bGetI2CDataFromLibFile(sfilename, &NewSensor);
	if( (NewSensor.width==0)       ||
		(NewSensor.height==0)      ||
		(NewSensor.ParaList==NULL) ||
		(NewSensor.ParaListSize==0)	  )
	{
		return FALSE;
	}
	memcpy(&CurrentSensor,&NewSensor,sizeof(SensorTab));
	return TRUE;
}
BOOL CHSFoxxDlg::bGetI2CDataFromLibFile(CString filename, pSensorTab pSensor)
{
	CStdioFile file;
	if (!file.Open((filename), CFile::modeRead | CFile::typeText))
	{
		return FALSE;
	}
	CString szLine = _T("");
	UINT addr =0, reg=0, value=0;
	BYTE i2cmode = 0;
	USHORT *pParaList= m_pSensorPara;
	USHORT *pSleepParaList = m_pSensorSleepPara;
	USHORT *pAF_InitParaList = m_pAF_InitParaList;
	USHORT *pAF_AutoParaList = m_pAF_AutoParaList;
	USHORT *pAF_FarParaList = m_pAF_FarParaList;
	USHORT *pAF_NearParaList = m_pAF_NearParaList;
	USHORT *pExposure_ParaList = m_pExposure_ParaList;
	USHORT *pGain_ParaList = m_pGain_ParaList;

	USHORT ParaListSize=0;
	USHORT SleepParaListSize = 0;
	USHORT AF_InitParaListSize = 0;
	USHORT AF_AutoParaListSize = 0;
	USHORT AF_FarParaListSize = 0;
	USHORT AF_NearParaListSize = 0;
	USHORT Exposure_ParaListSize = 0;
	USHORT Gain_ParaListSize = 0;

	
	CString sReg, sVal;
	CString strTmp[10];
	int tmp = 0;
	strTmp[0] = "[ParaList]";
	strTmp[1] = "[SleepParaList]";
	strTmp[2] = "[AF_InitParaList]";
	strTmp[3] = "[AF_AutoParaList]";
	strTmp[4] = "[AF_FarParaList]";
	strTmp[5] = "[AF_NearParaList]";
	strTmp[6] = "[Exposure_ParaList]";
	strTmp[7] = "[Gain_ParaList]";

	for(int i = 0; i <10; i++)
	{
		strTmp[i].MakeLower();
		strTmp[i].Trim();
	}
	int state = -1;
	while(file.ReadString(szLine))
	{
		CString Textout;
		//寻找注释符号或者']',如果有这样的，只取前面的，
		tmp = szLine.FindOneOf("//"); 
		if( tmp == 0)
		{
			continue;
		}
		else if(tmp > 0)
		{
			szLine = szLine.Left(tmp);
		}
		tmp = szLine.FindOneOf("]"); 
		if( tmp == 0)
		{
			continue;
		}
		else if(tmp > 0)
		{
			szLine = szLine.Left(tmp+1);
		}
		szLine.MakeLower();
		szLine.TrimLeft();
		szLine.TrimRight();		

		if(szLine == strTmp[0]) 
		{
			state = 0;
			ParaListSize = 0;
			continue;
		}
		else if(szLine == strTmp[1])
		{
			state = 1;
			SleepParaListSize = 0;
			continue;
		}
		else if(szLine == strTmp[2])
		{
			state = 2;
			AF_InitParaListSize = 0;
			continue;
		}
		else if(szLine == strTmp[3])
		{
			state = 3;
			AF_AutoParaListSize = 0;
			continue;
		}
		else if(szLine == strTmp[4])
		{
			state = 4;
			AF_FarParaListSize = 0;
			continue;
		}
		else if(szLine == strTmp[5])
		{
			state = 5;
			AF_NearParaListSize = 0;
			continue;
		}
		else if(szLine == strTmp[6])
		{
			state = 6;
			Exposure_ParaListSize = 0;
			continue;
		}
		else if(szLine == strTmp[7])
		{
			state = 7;
			Gain_ParaListSize = 0;
			continue;
		}

		if(szLine.IsEmpty())
			continue;
		if(szLine.Left(1) == ",")
			continue;
		if(szLine.Left(1) == ";")
			continue;
		if(szLine.Left(1) == "/")
			continue;

		if(szLine.Left(1) == "[")
		{
			state = -1;
			continue;
		}


		AfxExtractSubString(sReg, szLine, 0, ',');
		AfxExtractSubString(sVal, szLine, 1, ',');
		sReg.TrimLeft();   
		sReg.TrimRight();
		sVal.TrimRight();  
		sVal.TrimLeft();
				
		if(!sscanf_s(sReg, "0x%x", &reg)) //读取键值对数据	
			sscanf_s(sReg, "%d", &reg);
		
		if(!sscanf_s(sVal, "0x%x", &value)) //读取键值对数据	
			sscanf_s(sVal, "%d", &value);

		if(state == 0)
		{
			*(pParaList+ParaListSize) = reg;
			*(pParaList+ParaListSize+1) = value;
			ParaListSize += 2;
		}
		else if(state == 1)
		{
			*(pSleepParaList+SleepParaListSize) = reg;
			*(pSleepParaList+SleepParaListSize+1) = value;
			SleepParaListSize += 2;			
		}
		else if(state == 2)
		{
			*(pAF_InitParaList+AF_InitParaListSize) = reg;
			*(pAF_InitParaList+AF_InitParaListSize+1) = value;
			AF_InitParaListSize += 2;			
		}
		else if(state == 3)
		{
			*(pAF_AutoParaList+AF_AutoParaListSize) = reg;
			*(pAF_AutoParaList+AF_AutoParaListSize+1) = value;
			AF_AutoParaListSize += 2;			
		}
		else if(state == 4)
		{
			*(pAF_FarParaList+AF_FarParaListSize) = reg;
			*(pAF_FarParaList+AF_FarParaListSize+1) = value;
			AF_FarParaListSize += 2;			
		}
		else if(state == 5)
		{
			*(pAF_NearParaList+AF_NearParaListSize) = reg;
			*(pAF_NearParaList+AF_NearParaListSize+1) = value;
			AF_NearParaListSize += 2;			
		}
		else if(state == 6)
		{
			*(pExposure_ParaList+Exposure_ParaListSize) = reg;
			*(pExposure_ParaList+Exposure_ParaListSize+1) = value;
			Exposure_ParaListSize += 2;			
		}
		else if(state == 7)
		{
			*(pGain_ParaList+Gain_ParaListSize) = reg;
			*(pGain_ParaList+Gain_ParaListSize+1) = value;
			Gain_ParaListSize += 2;			
		}
		
	}
	file.Close();

	if(ParaListSize)
	{
		pSensor->ParaListSize = ParaListSize ;// + 2; //datasize...
		pSensor->ParaList = m_pSensorPara;

		if(SleepParaListSize)
		{
			pSensor->SleepParaListSize = SleepParaListSize;
			pSensor->SleepParaList = m_pSensorSleepPara;
		}
		if(AF_InitParaListSize)
		{
			pSensor->AF_InitParaListSize = AF_InitParaListSize;
			pSensor->AF_InitParaList = m_pAF_InitParaList;
		}
		if(AF_AutoParaListSize)
		{
			pSensor->AF_AutoParaListSize = AF_AutoParaListSize;
			pSensor->AF_AutoParaList = m_pAF_AutoParaList;
		}
		if(AF_FarParaListSize)
		{
			pSensor->AF_FarParaListSize = AF_FarParaListSize;
			pSensor->AF_FarParaList = m_pAF_FarParaList;
		}
		if(AF_NearParaListSize)
		{
			pSensor->AF_NearParaListSize = AF_NearParaListSize;
			pSensor->AF_NearParaList = m_pAF_NearParaList;
		}
		if(Exposure_ParaListSize)
		{
			pSensor->Exposure_ParaListSize = Exposure_ParaListSize;
			pSensor->Exposure_ParaList = m_pExposure_ParaList;
		}
		if(Gain_ParaListSize)
		{
			pSensor->Gain_ParaListSize = Gain_ParaListSize;
			pSensor->Gain_ParaList = m_pGain_ParaList;
		}
		return TRUE;
	}

	return FALSE;
}

////////////////////////////////////////////////////////////////
//Get the OS all test data...
////////////////////////////////////////////////////////////////

void CHSFoxxDlg::OnOseasy() 
{
	// TODO: Add your control notification handler code here
	//first kill the data buffer...


	if(m_RunMode != RUNMODE_STOP)
	{
		return;
	}
	KillDataBuffer();
	//CloseDevice(m_nDevID);

	if(!bOpenDevice())
	{
		return;
	}
	//20140504 added 

	//OS测试
	DTOS_Test();




	//close the sensor power...
	//设置电压，电流
	SENSOR_POWER Power[10];
	BOOL OnOff[10];

	Power[0] = POWER_AVDD;
	OnOff[0] = FALSE;

	Power[1] = POWER_DOVDD;
	OnOff[1] = FALSE;

	Power[2] = POWER_DVDD;
	OnOff[2] = FALSE;

	Power[3] = POWER_AFVCC;
	OnOff[3] = FALSE;

	Power[4] = POWER_VPP;
	OnOff[4] = FALSE;

	//close the power...
	PmuSetOnOff(Power, OnOff, 5, m_nDevID);


	//close the device link...
// 	if (m_nDevID >= 0)
// 	{
// 		CloseDevice(m_nDevID);
// 	}
}

void CHSFoxxDlg::OnDestroy() 
{
	CDialog::OnDestroy();
	
	// TODO: Add your message handler code here
}

BOOL CHSFoxxDlg::DestroyWindow() 
{
	// TODO: Add your specialized code here and/or call the base class
	KillTimer(1);

	//20141029 added... 
	if(m_RunMode != RUNMODE_STOP)
	{
		bCloseCamera();
	}

	if(m_pSensorPara)
		delete [] m_pSensorPara;  //free (m_pSensorPara);
	if(m_pSensorSleepPara)
		delete [] m_pSensorSleepPara;  //free (m_pSensorSleepPara);
	//20130823 added..
	if(m_pAF_InitParaList)
		delete [] m_pAF_InitParaList; 
	if(m_pAF_AutoParaList)
		delete [] m_pAF_AutoParaList; 
	if(m_pAF_FarParaList)
		delete [] m_pAF_FarParaList; 
	if(m_pAF_NearParaList)
		delete [] m_pAF_NearParaList; 
	if(m_pExposure_ParaList)
		delete [] m_pExposure_ParaList; 
	if(m_pGain_ParaList)
		delete [] m_pGain_ParaList; 
	
	return CDialog::DestroyWindow();
}

BOOL CHSFoxxDlg::bSaveBmpFile(CString sfilename, BYTE *pBuffer, UINT width, UINT height)
{
	int				 OffBits;
	HFILE			 bmpFile;
	BITMAPFILEHEADER bmpHeader; // Header for Bitmap file
	BITMAPINFO		 bmpInfo;

	OffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
	DWORD dwSizeImage = width*height*3;//IMAGESIZE_X*IMAGESIZE_Y*3;
	JpegFile::VertFlipBuf(pBuffer, width*3, height);

	bmpHeader.bfType = ( (WORD)('M'<<8)|'B' );  
	bmpHeader.bfSize = OffBits + dwSizeImage;
	bmpHeader.bfReserved1 = 0;
	bmpHeader.bfReserved2 = 0;
	bmpHeader.bfOffBits = OffBits;

	bmpInfo.bmiHeader.biSize            = sizeof(BITMAPINFOHEADER);
	bmpInfo.bmiHeader.biWidth           = width;
	bmpInfo.bmiHeader.biHeight          = height;
	bmpInfo.bmiHeader.biPlanes          = 1;
	bmpInfo.bmiHeader.biBitCount        = 24;
	bmpInfo.bmiHeader.biCompression     = BI_RGB;
	bmpInfo.bmiHeader.biSizeImage       = 0; 
	bmpInfo.bmiHeader.biXPelsPerMeter   = 0;
	bmpInfo.bmiHeader.biYPelsPerMeter   = 0;
	bmpInfo.bmiHeader.biClrUsed         = 0;
	bmpInfo.bmiHeader.biClrImportant    = 0;


	bmpFile = _lcreat(sfilename, FALSE); 
	if (bmpFile < 0) 
	{
		AfxMessageBox("can't create file");
		return FALSE;
	}

	UINT len;
	len = _lwrite(bmpFile, (LPSTR)&bmpHeader, sizeof(BITMAPFILEHEADER));
	len = _lwrite(bmpFile, (LPSTR)&bmpInfo, sizeof(BITMAPINFOHEADER));
	len = _lwrite(bmpFile, (LPSTR)pBuffer, bmpHeader.bfSize - sizeof(bmpHeader) - sizeof(bmpInfo)+4);  //+4 is for exact filesize
	_lclose(bmpFile);

	return TRUE;
}

BOOL CHSFoxxDlg::ReadPictureMat(Mat * PictureMat,string filename,int caramID)
{
	BOOL ret = TRUE;
	//filename = ".\\image\\" + filename;
	*PictureMat = imread(filename);
	if(!PictureMat->data)
	{
		ret = FALSE;
		msg("读取%s失败！",filename.c_str());
	}
	return ret;

}

BOOL CHSFoxxDlg::GetPictureMat(Mat * PictureMat,int caramID)
{
	LPBYTE BmpBuffer = NULL;
	CAAVision *vision = NULL;
	map<string,CAAVision*>::iterator it;
	//在map中找当前视觉id
	for (it = mapVision.begin();it != mapVision.end();it++)
	{
		if (it->second->m_iCurIDA == caramID)
		{
			vision = it->second;
			break;
		}
	}
	if (vision == NULL )
	{
		msg("当前视觉不存在,视觉id:%d",caramID);
		return FALSE;
	}
	BmpBuffer = AAVisionGrabPicture(caramID);
	if (BmpBuffer== NULL)
	{
		msg("获取图像失败,视觉id:%d",caramID);
		return  FALSE;
	}
	Mat img(vision->m_PreviewHeightA,vision->m_PreviewWidthA,CV_8UC3,BmpBuffer);
	*PictureMat=img.clone();	
	free(BmpBuffer);
	return TRUE;
}


//获取当前帧文件，并保存到其父目录的父目录下的image文件夹
BOOL CHSFoxxDlg::SavePicture(CString sfilename,int cameraID)
{

	CAAVision *vision = NULL;
	LPBYTE BmpBuffer = NULL;

	map<string,CAAVision*>::iterator it;
	//在map中找有字符串为camera_0的项
	for (it = mapVision.begin();it != mapVision.end();it++)
	{
		if (it->second->m_iDevIDA == cameraID)
		{
			vision = it->second;
			break;
		}
	}
	if (vision == NULL )
	{
		msg("当前视觉不存在,视觉id:%d",cameraID);
		return FALSE;
	}

	BmpBuffer = AAVisionGrabPicture(cameraID);

	if (BmpBuffer== NULL)
	{
		msg("获取图像失败,视觉id:%d",cameraID);
		return  FALSE;
	}
	//获得了图片指针，把他保存起来


	JpegFile::SaveJpeg(sfilename,BmpBuffer,vision->m_PreviewWidthA,vision->m_PreviewHeightA);


	free(BmpBuffer);
	
	return TRUE;
}

LPBYTE CHSFoxxDlg::AAVisionGrabPicture(int visionID)
{
	map<string,CAAVision*>::iterator it;
	CAAVision *vision = NULL;
	LPBYTE pic = NULL;
	//在map中找有字符串为camera_0的项
	for (it = mapVision.begin();it != mapVision.end();it++)
	{
		if (it->second->m_iDevIDA == visionID )
		{
			vision =  it->second;
			break;
		}
	}

	if (vision != NULL)
	{
		pic = vision->GrapOnePicture();
	}

	return pic;
}
void CHSFoxxDlg::OnBnClickedSavepicture()
{
	LPBYTE BmpBuffer = NULL;
	CAAVision *vision = NULL;

	map<string,CAAVision*>::iterator it;
	//在map中找有字符串为camera_0的项
	for (it = mapVision.begin();it != mapVision.end();it++)
	{
		if (it->second->m_iDevIDA == m_nDevID)
		{
			vision = it->second;
			break;
		}
	}
	if (vision == NULL )
	{
		msg("当前视觉不存在,视觉id:%d",m_nDevID);
		return ;
	}


	BmpBuffer = AAVisionGrabPicture(m_nDevID);

	if (BmpBuffer== NULL)
	{
		msg("获取图像失败,视觉id:%d",m_nDevID);
		return  ;
	}
	//获得了图片指针，把他保存起来

	CString sfilename ; 
	CTime time = CTime::GetCurrentTime();
	sfilename.Format("P%02d%02d%02d%02d%02d_%s_%d.jpg",time.GetMonth(),time.GetDay(),time.GetHour(),
		time.GetMinute(),time.GetSecond(),vision->m_pSensorNameA.c_str(),m_nDevID);
	JpegFile::SaveJpeg(sfilename,BmpBuffer,vision->m_PreviewWidthA,vision->m_PreviewHeightA);


	free(BmpBuffer);

}

void CHSFoxxDlg::OnBnClickedSetpower()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(TRUE);

	//设置电压，电流
	SENSOR_POWER Power[10];
	int Volt[10];
	int Current[10];
	BOOL OnOff[10];
	CURRENT_RANGE range[5];

	Power[0] = POWER_AVDD;
	Volt[0] = (int)(m_fAvdd * 1000); // 2.8V
	Current[0] = 300; // 300mA
	OnOff[0] = TRUE;
	range[0] = CURRENT_RANGE_MA;

	Power[1] = POWER_DOVDD;
	Volt[1] = (int)(m_fDovdd * 1000); // 1.8V
	Current[1] = 300; // 300mA
	OnOff[1] = TRUE;
	range[1] = CURRENT_RANGE_MA;	

	Power[2] = POWER_DVDD;
	Volt[2] = (int)(m_fDvdd * 1000);// 1.2V
	Current[2] =  300;// 300mA
	OnOff[2] = TRUE;
	range[2] = CURRENT_RANGE_MA;

	Power[3] = POWER_AFVCC;
	Volt[3] = (int)(m_fAfvcc * 1000); // 2.8V
	Current[3] = 300; // 300mA
	OnOff[3] = TRUE;
	range[3] = CURRENT_RANGE_MA;

	Power[4] = POWER_VPP;
	Volt[4] = (int)(m_fVpp * 1000); 
	Current[4] = 300; // 300mA
	OnOff[4] = FALSE;
	range[4] = CURRENT_RANGE_MA;

	//设置5路电压值
	if (PmuSetVoltage(Power,Volt,5,m_nDevID) != DT_ERROR_OK)
	{
		AfxMessageBox("Set Voltage Failed!");
	}
}

void CHSFoxxDlg::OnBnClickedSetmclk()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData();

	if (m_nDevID >= 0 != NULL)
	{
		SetSensorClock(TRUE,(USHORT)(m_fMclk*10),m_nDevID);
	}
}

void CHSFoxxDlg::OnBnClickedTestRoi()
{
	// TODO: 在此添加控件通知处理程序代码
/*	if(m_RunMode != RUNMODE_PLAY)
	{
		m_roi_state = 0;
		return;
	}
	if(CurrentSensor.width < 1600 || CurrentSensor.height < 1200)
	{
		return;
	}
	ULONG x = GetTickCount();
	m_RunMode = RUNMODE_STOP;
	//Sleep(50);
	if(m_hCameraThreadEventDead)
	{
		WaitForSingleObject(m_hCameraThreadEventDead, INFINITE);
		CloseHandle(m_hCameraThreadEventDead);
		m_hCameraThreadEventDead = NULL;
	}

	if(m_hDisplayThreadEventDead)
	{
		WaitForSingleObject(m_hDisplayThreadEventDead, INFINITE);
		CloseHandle(m_hDisplayThreadEventDead);
		m_hDisplayThreadEventDead = NULL;
	}


	m_bTripleBufferDirty = 0;

	if(m_roi_state == 1)
	{
		InitRoi(0, 0, CurrentSensor.width, CurrentSensor.height, 0, 0, 1, 1, CurrentSensor.type,TRUE, m_nDevID);	//全分辨率
#ifndef _ONLY_GRAB_IMAGE_
		InitDisplay(m_wndVideo[6].GetSafeHwnd(),CurrentSensor.width,CurrentSensor.height,CurrentSensor.type, m_nDevID);
		InitIsp(CurrentSensor.width, CurrentSensor.height, CurrentSensor.type, CHANNEL_A, m_nDevID );
#endif
		m_roi_state = 0;
	}
	else
	{
		InitRoi( (CurrentSensor.width/2-320), (CurrentSensor.height/2 - 240), 640, 480, 0, 0, 1, 1, CurrentSensor.type, TRUE, m_nDevID);	//ROI 640*480
#ifndef _ONLY_GRAB_IMAGE_
		InitDisplay(m_wndVideo[6].GetSafeHwnd(),640,480,CurrentSensor.type, m_nDevID);
		InitIsp(640, 480, CurrentSensor.type, CHANNEL_A , m_nDevID);
#endif
		m_roi_state = 1;
	}

	CalculateGrabSize(&m_GrabSize, m_nDevID);


	m_RunMode = RUNMODE_PLAY;

	m_hCameraThreadEventDead = CreateEvent(NULL, TRUE, FALSE, NULL);
	m_hDisplayThreadEventDead = CreateEvent(NULL, TRUE, FALSE, NULL);
	_beginthread( Camera_Thread, 0, (void*)this);
	_beginthread( Display_Thread, 0, (void*)this); */

}

void CHSFoxxDlg::OnBnClickedEnum()
{
	// TODO: 在此添加控件通知处理程序代码
		EnumerateDothinkeyDevice();
}

void CHSFoxxDlg::OnBnClickedFocusPos()
{
	// TODO: 在此添加控件通知处理程序代码
	if(m_RunMode != RUNMODE_STOP)
	{
		UpdateData(TRUE);
		CString stmp;
		m_FocusPosTemp=(1023<m_FocusPos)? 1023 : m_FocusPos;
		m_FocusPos = (0>m_FocusPosTemp)? 0 : m_FocusPos;
		stmp.Format("AF pos = %d", m_FocusPos);

		if(FindAfDevice(m_nDevID))
		{
			WriteAfValue(m_FocusPos << 4, 0, m_nDevID);
			AfxMessageBox(stmp);
		}
	}
}
int CHSFoxxDlg::AdjustZvalue(int robotID,int cameraID)
{
	//第一步：首先需要调整Z轴
	int temp_number=0;
	int  z_adjustnum =0;
	double zadjust_zthr =0;
	double distanceToOriginal[10]={0};
	double AAadjust_z = -100.0;
    Mat Ori_PictureMat;
	Mat Plus_PictureMat;
	Mat Minus_PictureMat;
	const char axisString[5]="Z ";

	z_adjustnum = CFG_INSTANCE->get_cfg_int(AA_Adjustparam,AAadjust_zMaxNum);
	zadjust_zthr = CFG_INSTANCE->get_cfg_double(AA_Adjustparam,AAadjust_zthr);
	ReadAAAdjustZValue(distanceToOriginal,z_adjustnum);

	while(!(( fabs(AAadjust_z)<zadjust_zthr)||(temp_number>=z_adjustnum)))
	{
	    //step1:保存当前帧的图片以及该图像正负向调整的像.......	
		int Ori_flag = GetPictureMat(&Ori_PictureMat,cameraID);
		if(Ori_flag != TRUE)
		{
			msg("(robotid:%d,visonid:%d)获取原始图片失败,",robotID,cameraID);return AA_GET_PICTIRE_FAIL;
		}
		if(AARobotMoveRelative(robotID,axisString, &distanceToOriginal[temp_number])<0)
		{
			msg("机器正向移动失败");
			temp_number++;
			continue;
		}
		int Plus_flag = GetPictureMat(&Plus_PictureMat,cameraID);
		if(Plus_flag != TRUE)
		{
			msg("获取距离为:%.6f原始图片失败,调整次数:%d",distanceToOriginal[temp_number],temp_number+1);
			return AA_GET_PICTIRE_FAIL;
		}
		double ww = -2*distanceToOriginal[temp_number];  
		if(AARobotMoveRelative(robotID,axisString, &ww)<0)
		{
			msg("机器负向移动失败");
			temp_number++;
			continue;
		}
		int Minus_flag=GetPictureMat(& Minus_PictureMat,cameraID);
		if(Minus_flag != TRUE)
		{
			msg("获取原始图片失败");return AA_GET_PICTIRE_FAIL;
		}
		if(AARobotMoveRelative(robotID,axisString, &(distanceToOriginal[temp_number]))<0)
		{
	 		msg("机器返回初始位置失败");return AA_GO_RERATIVEPT_FAIL;
		}
	    //step2.计算移动的距离.
		Mat img_ori =Ori_PictureMat;
		Mat img_p =Plus_PictureMat;
		Mat img_m =Minus_PictureMat;
	    img_ori = Img_resize(img_ori, g_AAParam.scale);
	    img_p = Img_resize(img_p, g_AAParam.scale);
	    img_m = Img_resize(img_m, g_AAParam.scale);
		State state;
		int OneSide_flag;
		state = getState(img_ori,img_p,img_m,distanceToOriginal[temp_number],-distanceToOriginal[temp_number],g_AAParam.pixel_size/g_AAParam.scale,g_AAParam.scale,
			cameraID,&OneSide_flag);
		double adjust_z = state.deltaZ; //
	    //step3..根据计算结果进行移动..
		const char posStr[16] = "Z U V";
		double pos[3] ={0.0};
		if(OneSide_flag>=0)
		{
			pos[0]=adjust_z;
		}
		else if(OneSide_flag==-1)
		{
			pos[0]=adjust_z;
			msg("(robotid:%d,visonid:%d)单边情况!继续调整",robotID,cameraID);
		}
		else
		{
			msg("(robotid:%d,visonid:%d)调整z轴时,初始位置误差范围太大,返回!OneSide_flag:%d,",robotID,cameraID,OneSide_flag);
			return AA_Z_ADJUST_BIG_FAIL;
		}
		//调整之前测试是否在可调范围内,是否超出软限位
		const char allCurrAex[16] = "X Y Z U V W";
	    double allCurrPos[6] = {0,0,0,0,0,0}; 
		if(AARobotCurrentPosition(robotID,allCurrAex, allCurrPos)<0)
		{
			msg("获取当前位置失败");return AA_GET_CURRENTPT_FAIL;
		}
		int result=AARobotMoveRelative(robotID,posStr, pos);
		if(result<0)
		{
			msg("(robotid:%d,visonid:%d)机器AA调整运动相对位置:%.6f失败,当前位置z:%.6f,调整次数:%d",robotID,cameraID,pos[0],allCurrPos[2],temp_number+1);
			temp_number++;
			continue;
		}
		temp_number++;
		msg("(robotid:%d,visonid:%d)z轴第%d次调整,调整距离为:%.6lf,阈值为:%.6lf,flag:%d",robotID,cameraID,temp_number,pos[0],zadjust_zthr,OneSide_flag);
		
		AAadjust_z = adjust_z;	
	}
	msg("(robotid:%d,visonid:%d)调整Z轴完毕！调整次数:%d",robotID,cameraID,temp_number);
	return 		temp_number-1;
}
int CHSFoxxDlg::AdjustUVvalue2(int robotID,int cameraID)
{
	double AAadjust_u=-100.0;
	double AAadjust_v=-100.0;
	int temp_number=0;
	double distanceToOriginalUV[10]={0.0};
	Mat Ori_PictureMat;
	Mat Plus_PictureMat;
	Mat Minus_PictureMat;
	const char axisString[5]="Z ";
	int uv_adjustnum = CFG_INSTANCE->get_cfg_int(AA_Adjustparam,AAadjust_uvMaxNum);
	double adjust_uvthr = CFG_INSTANCE->get_cfg_double(AA_Adjustparam,AAadjust_uvthr);
	ReadAAAdjustUVValue(distanceToOriginalUV,uv_adjustnum);
	while(!(( fabs(AAadjust_u)<adjust_uvthr && fabs(AAadjust_v)<adjust_uvthr)||(temp_number>=uv_adjustnum)))
	{
		//step1:获取当前帧的图片以及该图像正负向调整的图像..
		int Ori_flag=GetPictureMat(& Ori_PictureMat,cameraID);
		if(Ori_flag != TRUE)
		{
			msg("获取原始图片失败,返回");
			return AA_GET_PICTIRE_FAIL;
		}
		double temp_dis =distanceToOriginalUV[temp_number];
		if(AARobotMoveRelative(robotID,axisString, &temp_dis)<0)
		{
			msg("机器正向移动失败,移动的z轴距离为:%.6lf",distanceToOriginalUV[temp_number]);	
			temp_number++;
			continue;
		}
		int Plus_flag=GetPictureMat(& Plus_PictureMat,m_nDevID);
		if(Plus_flag != TRUE)
		{
			msg("获取正向原始图片失败,返回！");
			return AA_GET_PICTIRE_FAIL;
		}
		temp_dis =-2*distanceToOriginalUV[temp_number];
		if(AARobotMoveRelative(robotID,axisString, &temp_dis)<0)
		{
			msg("(robotid:%d,visonid:%d)机器负向移动失败,移动的z轴距离为:%.6lf",robotID,cameraID,-distanceToOriginalUV[temp_number]);
			AARobotMoveRelative(robotID,axisString,&distanceToOriginalUV[temp_number]);
			temp_number++;
			continue;
		}
		int Minus_flag=GetPictureMat(& Minus_PictureMat,m_nDevID);
		if(Minus_flag != TRUE)
		{
			msg("获取负向原始图片失败,返回！");
			return AA_GET_PICTIRE_FAIL;
		}

		if(AARobotMoveRelative(robotID,axisString, &distanceToOriginalUV[temp_number])<0)
		{
			msg("(robotid:%d,visonid:%d)机器返回初始位置失败",robotID,cameraID);
			return AA_GO_RERATIVEPT_FAIL;
		}
		//step2:..计算移动的距离.
		Ori_PictureMat = Img_resize(Ori_PictureMat, g_AAParam.scale);
		Plus_PictureMat = Img_resize(Plus_PictureMat, g_AAParam.scale);
		Minus_PictureMat = Img_resize(Minus_PictureMat, g_AAParam.scale);
		State state ={0};
		int OneSide_flag = -3;
		state = getState(Ori_PictureMat,Plus_PictureMat,Minus_PictureMat,distanceToOriginalUV[temp_number],-distanceToOriginalUV[temp_number],g_AAParam.pixel_size/g_AAParam.scale,g_AAParam.scale,
			cameraID,&OneSide_flag);
		double adjust_u = -state.y_angle ;
		double adjust_v = -state.x_angle ;

		//step3:根据计算结果进行移动..
		const char posStr[16] = "Z U V";
		double pos[3];
		pos[0]=0;

		if(OneSide_flag==0)
		{
			pos[1]=adjust_u;
			pos[2]=adjust_v;
		}
		else if(OneSide_flag==-1)
		{
			pos[1]=adjust_u;
			pos[2]=adjust_v;
			msg("(robotid:%d,visonid:%d)uv计算出单边继续!",robotID,cameraID);
		}
		else
		{
			msg("(robotid:%d,visonid:%d)调整时,初始位置误差范围太大,或者找不到点,返回!OneSide_flag:%d,",robotID,cameraID,OneSide_flag);
			return AA_Z_ADJUST_BIG_FAIL;
		}

		const char posStrV[16] = "V";
		int result =0;
		result=AARobotMoveRelative(robotID,posStrV,pos + 2);
		if(result<0)
		{
			msg("(robotid:%d,visonid:%d)先转V,UV旋转角度为:%.6lf,%.6lf,超出机器运动范围,继续",robotID,cameraID,pos[1],pos[2]);
			//temp_number++;
			//continue;
		}

		const char posStrU[16] = "U";
		result=AARobotMoveRelative(robotID,posStrU, pos +1);
		if(result<0)
		{
			msg("(robotid:%d,visonid:%d)再转U,UV旋转角度为:%.6lf,%.6lf,超出机器运动范围,继续",robotID,cameraID,pos[1],pos[2]);
			//temp_number++;
			//continue;
		}


		temp_number++;
		msg("(robotid:%d,visonid:%d)uv轴第%d次调整,调整距离为du:%.6lf,dv:%.6lf,阈值为:%.6lf,flag:%d",robotID,cameraID,temp_number,pos[1],pos[2],adjust_uvthr,OneSide_flag);

		AAadjust_u = adjust_u;	
		AAadjust_v = adjust_v ;
	}
	msg("(robotid:%d,visonid:%d)调整uv轴完毕！调整次数:%d",robotID,cameraID,temp_number);
	return temp_number;

}

int CHSFoxxDlg::AdjustUVvalue(int robotID,int cameraID)
{
	double AAadjust_u=-100.0;
	double AAadjust_v=-100.0;
	int temp_number=0;
	double distanceToOriginalUV[10]={0};
	Mat Ori_PictureMat;
	Mat Plus_PictureMat;
	Mat Minus_PictureMat;
	const char axisString[5]="Z ";
	int uv_adjustnum = CFG_INSTANCE->get_cfg_int(AA_Adjustparam,AAadjust_uvMaxNum);
	double adjust_uvthr = CFG_INSTANCE->get_cfg_double(AA_Adjustparam,AAadjust_uvthr);
	ReadAAAdjustUVValue(distanceToOriginalUV,uv_adjustnum);
	while(!(( fabs(AAadjust_u)<adjust_uvthr && fabs(AAadjust_v)<adjust_uvthr)||(temp_number>=uv_adjustnum)))
	{
		//step1:获取当前帧的图片以及该图像正负向调整的图像..
		int Ori_flag=GetPictureMat(& Ori_PictureMat,cameraID);
		if(Ori_flag != TRUE)
		{
			msg("获取原始图片失败,返回");
			return AA_GET_PICTIRE_FAIL;
		}
		double temp_dis =distanceToOriginalUV[temp_number];
		if(AARobotMoveRelative(robotID,axisString, &temp_dis)<0)
		{
			msg("(robotid:%d,visonid:%d)机器正向移动失败,移动的z轴距离为:%.6lf",robotID,cameraID,distanceToOriginalUV[temp_number]);	
			temp_number++;
			continue;
		}
		int Plus_flag=GetPictureMat(& Plus_PictureMat,m_nDevID);
		if(Plus_flag != TRUE)
		{
			msg("获取正向原始图片失败,返回！");
			return AA_GET_PICTIRE_FAIL;
		}
		temp_dis =-2*distanceToOriginalUV[temp_number];
		if(AARobotMoveRelative(robotID,axisString, &temp_dis)<0)
		{
			msg("(robotid:%d,visonid:%d)机器负向移动失败,移动的z轴距离为:%.6lf",robotID,cameraID,-distanceToOriginalUV[temp_number]);
			AARobotMoveRelative(robotID,axisString,&distanceToOriginalUV[temp_number]);
			temp_number++;
			continue;
		}
		int Minus_flag=GetPictureMat(& Minus_PictureMat,m_nDevID);
		if(Minus_flag != TRUE)
		{
			msg("获取负向原始图片失败,返回！");
			return AA_GET_PICTIRE_FAIL;
		}

		if(AARobotMoveRelative(robotID,axisString, &distanceToOriginalUV[temp_number])<0)
		{
			msg("机器返回初始位置失败");
			return AA_GO_RERATIVEPT_FAIL;
		}
		//step2:..计算移动的距离.
		Ori_PictureMat = Img_resize(Ori_PictureMat, g_AAParam.scale);
		Plus_PictureMat = Img_resize(Plus_PictureMat, g_AAParam.scale);
		Minus_PictureMat = Img_resize(Minus_PictureMat, g_AAParam.scale);
		State state ={0};
		int OneSide_flag = -3;
		state = getState(Ori_PictureMat,Plus_PictureMat,Minus_PictureMat,distanceToOriginalUV[temp_number],-distanceToOriginalUV[temp_number],g_AAParam.pixel_size/g_AAParam.scale,g_AAParam.scale,
			cameraID,&OneSide_flag);
		double adjust_u = -state.y_angle ;
		double adjust_v = -state.x_angle ;

		//step3:根据计算结果进行移动..
		const char posStr[16] = "Z U V";
		double pos[3];
		pos[0]=0;

		if(OneSide_flag==0)
		{
			pos[1]=adjust_u;
			pos[2]=adjust_v;
		}
		else if(OneSide_flag==-1)
		{
			pos[1]=adjust_u;
			pos[2]=adjust_v;
			msg("uv计算出单边继续!");
		}
		else
		{
			msg("(robotid:%d,visonid:%d)调整时,初始位置误差范围太大,或者找不到点,返回!OneSide_flag:%d,",robotID,cameraID,OneSide_flag);
			return AA_Z_ADJUST_BIG_FAIL;
		}

		int result=AARobotMoveRelative(robotID,posStr, pos);
		if(result<0)
		{
			msg("(robotid:%d,visonid:%d)uv旋转角度为:%.6lf,%.6lf,超出机器运动范围,继续",robotID,cameraID,pos[1],pos[2]);
			temp_number++;
			continue;
		}
		temp_number++;
		msg("(robotid:%d,visonid:%d)uv轴第%d次调整,调整距离为du:%.6lf,dv:%.6lf,阈值为:%.6lf,flag:%d",robotID,cameraID,temp_number,pos[1],pos[2],adjust_uvthr,OneSide_flag);
		
		AAadjust_u = pos[1];	
		AAadjust_v = pos[2];
	}
	msg("(robotid:%d,visonid:%d)调整uv轴完毕！调整次数:%d",robotID,cameraID,temp_number);
	return temp_number-1;
		
}
int CHSFoxxDlg::AdjustXYvalue(int robotID,int cameraID)
{
	int ret = AA_OK;

	Mat Ori_PictureMat1;
	int Ori_flag1=GetPictureMat(&Ori_PictureMat1,cameraID);
	if(Ori_flag1 != TRUE)
	{
		msg("获取原始图片失败");
		return AA_GET_PICTIRE_FAIL;
	}
	Ori_PictureMat1 = Img_resize(Ori_PictureMat1, g_AAParam.scale);
	OFF_set offset;
	offset =  getOffcenter(Ori_PictureMat1, g_AAParam.pixel_size/g_AAParam.scale);
	double adjust_x = -offset.x_offset;
	double adjust_y = -offset.y_offset;
	//START3.......................根据计算结果进行移动............................................
	const char posStr_xy[6] = "X Y";
	double pos_xy[2];
	pos_xy[0]=adjust_x;
	pos_xy[1]=adjust_y;	

	//调整之前测试是否在可调范围内,是否超出软限位
	//x:[-1,1] y:[-1,1] z;[0.3,0.8] u:[-3,3] v:[-3,3] w:[-3,3]


	int result_xy=AARobotMoveRelative(m_nPiDevID,posStr_xy, pos_xy);
	if(result_xy<0)
	{
		msg("(robotid:%d,visonid:%d)机器AA调整xy运动失败,超出机器运动范围",robotID,cameraID);
		ret = AA_GO_RERATIVEPT_FAIL;

	}

	return ret;

}
int CHSFoxxDlg::FiveAdjustvalue(int robotID,int cameraID)
{
	//第三步：五点法确定
	//step1:保存当前帧的图片以及该图像正负向调整的图像..
	const char axisStringQ[15]="X Y Z U V W";
	const char axisString[15]="Z";
	double Ori_ImageCurrentQ[6] ={ 0.0};
	double Ori_ImageCurrent = 0.0;
	double DistanceToOriginal = 0.0;
	Mat Ori_PictureMat;
	Mat Plus_PictureMat1;
	Mat Plus_PictureMat2;
	Mat Minus_PictureMat1;
	Mat Minus_PictureMat2;
	if(AARobotCurrentPosition(robotID,axisStringQ, Ori_ImageCurrentQ)<0)
	{
		msg("5点法,获取当前位置失败");
		return AA_GET_CURRENTPT_FAIL;
	}
	msg("当前位置:x:%.6lf,y:%.6lf,z:%.6lf,u:%.6lf,v:%.6lf,z:%.6lf",Ori_ImageCurrentQ[0],Ori_ImageCurrentQ[1],Ori_ImageCurrentQ[2],Ori_ImageCurrentQ[3]
	,Ori_ImageCurrentQ[4],Ori_ImageCurrentQ[5]);
	
	int Ori_flag=GetPictureMat(& Ori_PictureMat,cameraID);
	if(Ori_flag != TRUE)
	{
		msg("获取原始图片失败");
		return AA_GET_PICTIRE_FAIL;
	}
	//正向移动第一张图，0.003

	DistanceToOriginal = AA_Mirco_disSmall;
	if(AARobotMoveRelative(robotID,axisString, &DistanceToOriginal)<0)
	{
		msg("机器正向移动失败,移动距离:%.6lf",DistanceToOriginal);
		return AA_GO_RERATIVEPT_FAIL;

	}
	int Plus_flag=GetPictureMat(& Plus_PictureMat1,m_nDevID);
	if(Plus_flag != TRUE)
	{
		msg("获取正向原始图片失败");
		return AA_GET_PICTIRE_FAIL;
	}

	//正向移动第一张图，0.006
	
	if(AARobotMoveRelative(robotID,axisString, &DistanceToOriginal)<0)
	{
		msg("机器正向移动失败,移动距离:%.6lf",2*DistanceToOriginal);
		return AA_GO_RERATIVEPT_FAIL;
	}
	int Plus_flag2=GetPictureMat(& Plus_PictureMat2,cameraID);
	if(Plus_flag2 != TRUE)
	{
		msg("获取正向原始图片失败");
		return AA_GET_PICTIRE_FAIL;
	}

	////负向移动第一张图，0.003
	double ww1 =-3*DistanceToOriginal;
	if(AARobotMoveRelative(robotID,axisString, &ww1)<0)
	{
		msg("机器负向移动失败,移动距离:%.6lf",-DistanceToOriginal);
		return AA_GO_RERATIVEPT_FAIL;
	}
	
	int Minus_flag1=GetPictureMat(& Minus_PictureMat1,cameraID);
	if(Minus_flag1 != TRUE)
	{
		msg("获取负向原始图片失败");
		return AA_GET_PICTIRE_FAIL;
	}

	//负向移动第2张图，0.006
	ww1 = -DistanceToOriginal;
	if(AARobotMoveRelative(robotID,axisString, &ww1)<0)
	{
		msg("机器负向移动失败,移动距离:%.6lf",-2*DistanceToOriginal);
		return AA_GO_RERATIVEPT_FAIL;
	}

	int Minus_flag2=GetPictureMat(& Minus_PictureMat2,cameraID);
	if(Minus_flag2 != TRUE)
	{
		msg("获取负向原始图片失败");
		return AA_GET_PICTIRE_FAIL;
	}
	double ww3 =2*DistanceToOriginal;
	if(AARobotMoveRelative(robotID,axisString, &ww3)<0)
	{
		msg("机器回起始位失败,移动距离:%.6lf",ww3);
		return AA_GO_RERATIVEPT_FAIL;
	}
	//step2:计算移动的距离.


	Ori_PictureMat = Img_resize(Ori_PictureMat, g_AAParam.scale);
	Minus_PictureMat1 = Img_resize(Minus_PictureMat1, g_AAParam.scale);
	Minus_PictureMat2 = Img_resize(Minus_PictureMat2, g_AAParam.scale);
	Plus_PictureMat1 = Img_resize(Plus_PictureMat1, g_AAParam.scale);
	Plus_PictureMat2 = Img_resize(Plus_PictureMat2, g_AAParam.scale);
	State state = {0};
	state = getState5(Minus_PictureMat2, Minus_PictureMat1, Ori_PictureMat, Plus_PictureMat1, Plus_PictureMat2, g_AAParam.pixel_size/g_AAParam.scale, g_AAParam.scale,cameraID);
	double adjust_z = state.deltaZ;
	double adjust_u =  0;//state.y_angle;
	double adjust_v = 0;//state.x_angle;

	//step3:.根据计算结果进行移动..
	const char posStr[16] = "Z U V";
	double pos[3];
	pos[0]=adjust_z;
	pos[1]=0;
	pos[2]=0;
	int t = 0;
	//if (fabs(m_AAadjust_u)>0.11)
	if (fabs(adjust_u)>t)
	{
		pos[1]=-adjust_v;

	}
	//if(fabs(m_AAadjust_v)>0.11)
	if (fabs(adjust_v)>t)
	{
		pos[2]=-adjust_u;
	}


	int result=AARobotMoveRelative(robotID,posStr, pos);
	if(result<0)
	{
		msg("机器调整失败,移动距离u:%.6lf,v:%.6lf",pos[1],pos[2]);
		return AA_GO_RERATIVEPT_FAIL;
	}
	//step4:判断是否调到位置...若调整的z轴超过AA_Mirco_disSmall*2 继续调整
	if (fabs(adjust_z) >= AA_Mirco_disSmall*2)
	{
		//step:保存当前帧的图片以及该图像正负向调整的图像.................
		const char axisString[5]="Z ";
		vector<Mat> PictureMat;
		int direction = ((adjust_z > 0) - (adjust_z < 0));
		double DistanceToOriginal = AA_Mirco_disSmall * direction;
		//向偏移方向移动四张张图，间隔0.003
		for (int i = 0; i < 4; i++)
		{
			if(AARobotMoveRelative(robotID,axisString, &DistanceToOriginal)<0)
			{
				msg("机器正向移动失败,行:%d",__LINE__);
				DistanceToOriginal=0;
				return AA_GO_RERATIVEPT_FAIL;
			}
			Mat Plus_PictureMat;
			int Plus_flag1=GetPictureMat(& Plus_PictureMat,cameraID);
			if(Plus_flag1 != TRUE)
			{
				msg("获取原始图片失败,行:%d",__LINE__);
				return AA_GET_PICTIRE_FAIL;
			}
			Plus_PictureMat = Img_resize(Plus_PictureMat, g_AAParam.scale);

			PictureMat.push_back(Plus_PictureMat);
		}

		double ww3 = -2*DistanceToOriginal;
		if(AARobotMoveRelative(m_nPiDevID,axisString, &ww3)<0)
		{
			msg("机器返回初始位置失败行:%d",__LINE__);
			return AA_GO_RERATIVEPT_FAIL;
		}
		//srep2:计算移动的距离..
		Mat img_ori, img_p1, img_p2, img_m1, img_m2;
		if(adjust_z > 0)
		{
			img_ori =PictureMat[1];
			img_p1 =PictureMat[2];
			img_p2 =PictureMat[3];
			img_m1 =PictureMat[0];
			img_m2 =Plus_PictureMat2;
		}
		else
		{
			img_ori =PictureMat[1];
			img_p1 =PictureMat[0];
			img_p2 =Minus_PictureMat2;
			img_m1 =PictureMat[2];
			img_m2 =PictureMat[3];
		}
		/*img_ori = Img_resize(img_ori, g_AAParam.scale);
		img_p1 = Img_resize(img_p1, g_AAParam.scale);
		img_p2 = Img_resize(img_p2, g_AAParam.scale);
		img_m1 = Img_resize(img_m1, g_AAParam.scale);
		img_m2 = Img_resize(img_m2, g_AAParam.scale);*/
		State state;
		state = getState5(img_m2, img_m1, img_ori, img_p1, img_p2, g_AAParam.pixel_size/g_AAParam.scale, g_AAParam.scale,g_AAParam.temPlate[cameraID]);
		double adjust_z = state.deltaZ;
		double adjust_u = 0;// state.y_angle ;
		double adjust_v = 0;//state.x_angle;

		//START3.......................根据计算结果进行移动............................................
		const char posStr[16] = "Z U V";
		double pos[3];
		pos[0]=adjust_z;
		pos[1]=0;
		pos[2]=0;
		if (fabs(adjust_u)>t)
		{
			pos[1]=-adjust_v;

		}
		if (fabs(adjust_v)>t)
		{
			pos[2]=-adjust_u;
		}



		int result=AARobotMoveRelative(robotID,posStr, pos);
		if(result<0)
		{
			//AfxMessageBox("机器AA调整运动失败");
			msg("5点法超出机器运动范围,z:%.6lf,u:%.6lf,v:%.6lf",pos[0],pos[1],pos[2]);
			return AA_GO_RERATIVEPT_FAIL;
		}
	}


	return 0;
}
int CHSFoxxDlg::GoInitPosRobot(int robotid,const char* axisstring,int posIndex,double* pos)
{
	int ret = 0;
	map<string,CAARobot*>::iterator it;
	for (it= mapRobot.begin();it!= mapRobot.end();it++)
	{
		if (it->second->GetAARobotID() == robotid)
		{			
			break;
		}
	}

	string ip1 = CFG_INSTANCE->get_cfg(AAROBOTBIND,AAROBOT1);
	string ip2 = CFG_INSTANCE->get_cfg(AAROBOTBIND,AAROBOT2);
	string ip3 = CFG_INSTANCE->get_cfg(AAROBOTBIND,AAROBOT3);
	string ip4 = CFG_INSTANCE->get_cfg(AAROBOTBIND,AAROBOT4);
	if (it != mapRobot.end())
	{
		if (it->second->GetAARobotConnected() == false)
		{
			msg("机器人(id:%d)没连接!",robotid);
			return -1;
		}
		if (pos != NULL)
		{
			return it->second->MoveAAAbsolute(robotid,axisstring,pos);
		}

		if (it->first == ip1 )
		{
			
		     ret = it->second->MoveAAAbsolute(robotid,axisstring,(posIndex == AAINDEX1?g_AAParam.robot1InitPos1:g_AAParam.robot1InitPos2));
		}else if (it->first == ip2 )
		{
			ret = it->second->MoveAAAbsolute(robotid,axisstring,(posIndex == AAINDEX1?g_AAParam.robot2InitPos1:g_AAParam.robot2InitPos2));
		}else if (it->first == ip3 )
	   {
			ret = it->second->MoveAAAbsolute(robotid,axisstring,(posIndex == AAINDEX1?g_AAParam.robot3InitPos1:g_AAParam.robot3InitPos2));
		}else if (it->first == ip4 )
		{
			ret = it->second->MoveAAAbsolute(robotid,axisstring,(posIndex == AAINDEX1?g_AAParam.robot3InitPos1:g_AAParam.robot4InitPos2));
		}
		else
		{
			msg("请检查配置文件有没有配置好!");
			return -1;
		}		
	}
	else
	{
		msg("机器人(id:%d)没连接!",robotid);
		return -1;
	}
	return ret = (ret ==TRUE?0:-1);
}

void CHSFoxxDlg::SetInitPosRobot(int robotid,const char* axisstring,int posIndex,double* pos )
{
	int ret = TRUE;
	map<string,CAARobot*>::iterator it;
	double *tempinitpos = NULL;
	for (it= mapRobot.begin();it!= mapRobot.end();it++)
	{
		if (it->second->GetAARobotID() == robotid)
		{			
			break;
		}
	}
	if (it == mapRobot.end())
	{
		msg("机器人(id:%d)没连接!",robotid);
		return ;
	}
	string ip1 = CFG_INSTANCE->get_cfg(AAROBOTBIND,AAROBOT1);
	string ip2 = CFG_INSTANCE->get_cfg(AAROBOTBIND,AAROBOT2);
	string ip3 = CFG_INSTANCE->get_cfg(AAROBOTBIND,AAROBOT3);
	string ip4 = CFG_INSTANCE->get_cfg(AAROBOTBIND,AAROBOT4);
	if (it->first == ip1 )
	{
		tempinitpos = (posIndex == AAINDEX1?g_AAParam.robot1InitPos1:g_AAParam.robot1InitPos2);
		memcpy(tempinitpos,pos,sizeof(double)* PI_AXIS_MAXNUM);
	}else if (it->first == ip2 )
	{
		tempinitpos = (posIndex == AAINDEX1?g_AAParam.robot2InitPos1:g_AAParam.robot2InitPos2);
		memcpy(tempinitpos,pos,sizeof(double)* PI_AXIS_MAXNUM);
	}else if (it->first == ip3 )
	{
		tempinitpos = (posIndex == AAINDEX1?g_AAParam.robot3InitPos1:g_AAParam.robot3InitPos2);
		memcpy(tempinitpos,pos,sizeof(double)* PI_AXIS_MAXNUM);
	}else if (it->first == ip4 )
	{
		tempinitpos = (posIndex == AAINDEX1?g_AAParam.robot4InitPos1:g_AAParam.robot4InitPos2);
		memcpy(tempinitpos,pos,sizeof(double)* PI_AXIS_MAXNUM);
	}
	else
	{
		msg("请检查配置文件有没有配置好!");
		return ;
	}		


}
int CHSFoxxDlg::StartDoubleAA(int robotid,int visonidfixID,int visonidmovID)
{
	//步骤:抓两个图像,然后以visonidfixID为基准来调节visonidmovID
	//绑定关系为:(Camera_1,Camera_2),(Camera_3,Camera_4),(Camera_5,Camera_6),(Camera_7,Camera_8)
	int ret = AA_OK;
	Mat movMat,fixMat;
	int flag = FALSE;
	const char axisString[16]="U V W";
	double movpos[3] ={0.0};

	if (visonidfixID <0 || visonidmovID<0 || robotid <0)
	{
		msg("没有连接。visonidfixID：%d,visonidmovID：%d,robotid：%d.",visonidfixID,visonidmovID,robotid);
		return -1;
	}

	flag = GetPictureMat(&fixMat,visonidfixID);
	if(flag != TRUE)
	{
		return AA_GET_PICTIRE_FAIL;
	}

	flag = GetPictureMat(&movMat,visonidmovID);
	if(flag != TRUE)
	{
		return AA_GET_PICTIRE_FAIL;
	}

	//movMat = imread("image\\fixMat.jpg");//读图

		//缩小图小,提高速度
	movMat = Img_resize(movMat, g_AAParam.doubleAAscale);
	fixMat = Img_resize(fixMat, g_AAParam.doubleAAscale);

	savePicture(movMat,"movMat.jpg");
	savePicture(fixMat,"fixMat.jpg");

	Para param;
	param = calPara(fixMat, movMat, g_AAParam.doubleAAdis, g_AAParam.pixel_size/g_AAParam.doubleAAscale, g_AAParam.focus, g_AAParam.objdis);
	double adjust_w = param.w;
	double adjust_u = param.v;
	double adjust_v = -param.u;

	movpos[0] = adjust_u;
	movpos[1] = adjust_v;
	movpos[2] = adjust_w;

	msg("机器移动u:%.6lf,v:%.6lf,w:%.6lf失败",movpos[0],movpos[1],movpos[2]);
	ret = AARobotMoveRelative(robotid,axisString, movpos);
	if(ret <0)
	{
		msg("机器移动u:%.6lf,v:%.6lf,w:%.6lf失败",movpos[0],movpos[1],movpos[2]);
	}
	else
	{
		ret = AA_OK;

	}

	flag = GetPictureMat(&movMat,visonidmovID);
	if(flag != TRUE)
	{
		return AA_GET_PICTIRE_FAIL;
	}
	savePicture(movMat,"movMat后.jpg");


	return ret;

}
int CHSFoxxDlg::StartAA(int robotid,int visonid)
{
	int ret = AA_OK; //成功返回0;
	CString ALLDynamicList=".\\image\\";
	const char axisString1[5]="Z";
	double First_ImageCurrent = 0.0;

	//return -2;///同意返回-1;


	//暂时只支持一个机器人
	//robotid = m_nPiDevID;
	//visonid = m_nDevID ;

	if(AARobotCurrentPosition(robotid,axisString1, &First_ImageCurrent)<0)
	{
		msg("(robotid:%d,visonid:%d)获取当前位置失败",robotid,visonid);return AA_GET_CURRENTPT_FAIL;
	}
	CString FirstImage;
	FirstImage.Format("%.3lf",First_ImageCurrent);
	//SavePicture(ALLDynamicList+FirstImage+"_First.jpg",visonid);
	double time = static_cast<double>(getTickCount());


	double distanceToOriginal[10]={0};
	int  first_z_number = AdjustZvalue(robotid,visonid);
	if (first_z_number<0)
	{
		return AA_ADJUST_FAIL;
	}
	//第二步：然后需再调整U/V轴
	int  adjust_uv_number = AdjustUVvalue2(robotid,visonid);
	if (adjust_uv_number<0)
	{
		return AA_ADJUST_FAIL;
	}




	return ret ;


}

void CHSFoxxDlg::OnBnClickedStartaa()
{
	int ret = AA_OK;
	const char axisString1[5]="Z";
	double First_ImageCurrent = 0.0;


	if (m_nPiDevID<0 || m_nDevID <0)
	{
		msg("没有连接,返回!当前机器人id:%d,视觉id:%d",m_nPiDevID,m_nDevID);return;
	}
	 CreateDirectory(".\\image",NULL);//创建文件夹

	 if (g_AAParam.AATYPE == DOUBLEAA)//双摄像头处理
	 {
		 if (IDYES == MessageBox(_T("你确定调整双摄吗?请确保已经调整了基准的图像"),_T("双摄像头"),MB_YESNO))
		 { 
			 //步骤:抓两个图像,然后以visonidfixID为基准来调节visonidmovID,首先确保固定的通道已经调整好
			 //绑定关系为:(Camera_1,Camera_2),(Camera_3,Camera_4),(Camera_5,Camera_6),(Camera_7,Camera_8)

				int fixedvisionID= FindBinderIDByID(m_nDevID);
				ret =   StartDoubleAA(m_nPiDevID,fixedvisionID,m_nDevID);
				return ;
		 }
		 return ;
	 }

	if(AARobotCurrentPosition(m_nPiDevID,axisString1, &First_ImageCurrent)<0)
	{
		msg("获取当前位置失败");return;
	}
	CString FirstImage;
	FirstImage.Format("%.3lf",First_ImageCurrent);
	if (CFG_INSTANCE->get_cfg_int(AACoreCfg,savepicture))
	{
		SavePicture(".\\image\\"+FirstImage+"_First.jpg",m_nDevID);
	}
	
	double time = static_cast<double>(getTickCount());	

	//一，调整z

	double distanceToOriginal[10]={0};
	int  first_z_number = AdjustZvalue(m_nPiDevID,m_nDevID);
	if (first_z_number<0)
	{
		return ;
	}
	int  adjust_uv_number =0;
	//二：然后需再调整U/V轴
	adjust_uv_number = AdjustUVvalue2(m_nPiDevID,m_nDevID);
	if (adjust_uv_number<0)
	{
		return ;
	}

	int  adjust_zuv_number = 0;
	//三：5点法
	adjust_zuv_number = FiveAdjustvalue( m_nPiDevID,m_nDevID);
	//************************************************************************************************************
	//四：调整x,y
	//ret = AdjustXYvalue( m_nPiDevID,m_nDevID);

	//五,获取当前位置
	const char allCurrAex[16] = "X Y Z U V W";
	double allCurrPos[6] = {0,0,0,0,0,0}; 
	if(AARobotCurrentPosition(m_nPiDevID,allCurrAex, allCurrPos)<0)
	{
		msg("获取当前位置失败");
		return;
	}
	double absDistance[6];
	absDistance[0]=allCurrPos[0];
	absDistance[1]=allCurrPos[1];
	absDistance[2]=allCurrPos[2];
	absDistance[3]=allCurrPos[3];
	absDistance[4]=allCurrPos[4];


	//第六步：保存最终结果的图片	
	time = ((double)getTickCount() - time) / getTickFrequency();//计算所用时间
	CString m_TempName;
	m_TempName.Format(_T("所用时间:%lfs.\n\n z轴调整次数:%d; UV轴调整次数:%d 最后z又调整次数:%d.\n\n总共调节次数:%d, 最终z位置:%f,\n\n 最终u位置:%f, 最终v位置:%f"),time,first_z_number,adjust_uv_number,adjust_zuv_number,first_z_number+adjust_uv_number+adjust_zuv_number,absDistance[2],absDistance[3],absDistance[4]);
	msg(m_TempName);

	if (CFG_INSTANCE->get_cfg_int(AACoreCfg,savepicture))
	{
 		SavePicture(".\\image\\"+FirstImage+".jpg",m_nDevID);//_Last
	}
	return;

}






void CHSFoxxDlg::OnClickedGoHome()
{
	// TODO: 在此添加控件通知处理程序代码
	const char allAex[16] = "X Y Z U V W";;
	double allPos[6] = {0,0,0,0,0,0}; 
	if(AARobotCurrentPosition(m_nPiDevID,allAex, allPos)<0)
	{
		MessageBox("获取当前位置失败");
		return;
	}

	CString Initstr_x;
	CString Initstr_y;
	CString Initstr_z;
	CString Initstr_u;
	CString Initstr_v;
	CString Initstr_w;
	 
	double Initdou_x;
	double Initdou_y;
	double Initdou_z;
	double Initdou_u;
	double Initdou_v;
	double Initdou_w;

	GetDlgItem(IDC_STEP_X)->GetWindowText(Initstr_x);
	GetDlgItem(IDC_STEP_Y)->GetWindowText(Initstr_y);
	GetDlgItem(IDC_STEP_Z)->GetWindowText(Initstr_z);
	GetDlgItem(IDC_STEP_U)->GetWindowText(Initstr_u);
	GetDlgItem(IDC_STEP_V)->GetWindowText(Initstr_v);
	GetDlgItem(IDC_STEP_W)->GetWindowText(Initstr_w);

	Initdou_x=_tstof(Initstr_x);
	Initdou_y=_tstof(Initstr_y);
	Initdou_z=_tstof(Initstr_z);
	Initdou_u=_tstof(Initstr_u);
	Initdou_v=_tstof(Initstr_v);
	Initdou_w=_tstof(Initstr_w);

	/*Test
	CString adjust_xyzuvw0;
	adjust_xyzuvw0.Format(_T("%lf %lf %lf %lf %lf %lf\n"),allPos[0],allPos[1],allPos[2],allPos[3],allPos[4],allPos[5]);
	*/

	//allPos[0]=-allPos[0]+double(Initdou_x);
	//allPos[1]=-allPos[1]+double(Initdou_y);
	//allPos[2]=-allPos[2]+double(Initdou_z);
	//allPos[3]=-allPos[3]+double(Initdou_u);
	//allPos[4]=-allPos[4]+double(Initdou_v);
	//allPos[5]=-allPos[5]+double(Initdou_w);

	allPos[0]=double(Initdou_x);
	allPos[1]=double(Initdou_y);
	allPos[2]=double(Initdou_z);
	allPos[3]=double(Initdou_u);
	allPos[4]=double(Initdou_v);
	allPos[5]=double(Initdou_w);
	
	/*CString adjust_xyzuvw;
	CString adjust_xyzuvw1;
	adjust_xyzuvw.Format(_T("%lf %lf %lf %lf %lf %lf\n"),Initdou_x,Initdou_y,Initdou_z,Initdou_u,Initdou_v,Initdou_w);
	adjust_xyzuvw1.Format(_T("%lf %lf %lf %lf %lf %lf\n"),allPos[0],allPos[1],allPos[2],allPos[3],allPos[4],allPos[5]);
	AfxMessageBox(adjust_xyzuvw0+adjust_xyzuvw+adjust_xyzuvw1);
	
	int Aresult=AARobotCurrentPosition(m_nPiDevID,allAex, allPos);
	if(Aresult<0){
		CString Aresult1;
		Aresult1.Format(_T("result:\n%d",Aresult));
		AfxMessageBox(Aresult1);
		AfxMessageBox("机器回初始位置失败");
		return;}
	*/
	
	if(AARobotMoveRelative(m_nPiDevID,allAex, allPos)<0)
	{
		
		MessageBox("获取当前位置失败");
		return;
	
	}
	//AfxMessageBox("机器回初始位置成功");
	return;

}





void CHSFoxxDlg::OnEnChangeInitY()
{
	// TODO:  如果该控件是 RICHEDIT 控件，它将不
	// 发送此通知，除非重写 CDialog::OnInitDialog()
	// 函数并调用 CRichEditCtrl().SetEventMask()，
	// 同时将 ENM_CHANGE 标志“或”运算到掩码中。

	// TODO:  在此添加控件通知处理程序代码
}





void CHSFoxxDlg::OnBnClickedEnumnetdevice()
{
	// 枚举网络设备
	char szFoundNETDevices[NETDEVICESLEN] = {0 };
	int devNum =0;
	char *buf = NULL;  
	char *p[8] = {NULL};
	int in = 0;
	char *outer_ptr = NULL;  
	char *inner_ptr = NULL; 
	CString str;

	m_PiList.ResetContent();
	PI_NET_PARAM curpinetparam ;
	PI_NET_PARAM curpinetparam2 ;
	memset(&curpinetparam,0,sizeof(PI_NET_PARAM));
	memset(&curpinetparam2,0,sizeof(PI_NET_PARAM));
	UpdateData(TRUE);
	int nCount = m_PiList.GetCount();
	int nindex  = m_PiList.GetCurSel();
	if (nindex != LB_ERR)
	{
		m_PiList.GetLBText(nindex,str);
		ParsePIipAndPort(str.GetBuffer(),curpinetparam);
	}
	else
	{
		nindex = 0;
	}

	devNum = CAARobot::EnumerateAATCPIPDevices(szFoundNETDevices, NETDEVICESLEN-1, "");
	if (devNum<=0)
	{
		msg("枚举网络PI设备失败!返回值:%d",devNum);
		return;
	}
	else
	{
		buf = szFoundNETDevices;
		while((p[in] = strtok(buf, "\n")) != NULL)   
		{      
			in++;
			buf = NULL;  
		} 
		if (in != devNum)
		{
			msg("枚举PI后解析错误!");
			return ;
		}
		for (int i = 0;i<in;i++)
		{
			m_PiList.AddString(p[i]);

			//m_PInetName[i].Format("%s",p[i]);		
		}

		nCount = m_PiList.GetCount();
		for (int i= 0;i<nCount;i++)
		{		
			m_PiList.GetLBText(i,str);
			ParsePIipAndPort(str.GetBuffer(),curpinetparam2);
			if (curpinetparam2.ip == curpinetparam.ip)
			{
				nindex = i;
				break;
			}
		}
		if (nCount>0)
		{
			m_PiList.SetCurSel(nindex);//第一次枚举时是否不要选择，不选择是否是 m_PiList.SetCurSel(LB_ERR)
		}
		
	}
	return;
}


void CHSFoxxDlg::OnBnClickedConnect()//把遍历到的都连接上
{
	int curPiDevID = -1;	
	CString str;
	int curID =-1;
	int nindex = -1;
	PI_NET_PARAM pinetparam ;
	PI_NET_PARAM curpinetparam ;

	memset(&pinetparam,0,sizeof(PI_NET_PARAM));
	memset(&curpinetparam,0,sizeof(PI_NET_PARAM));
	UpdateData(TRUE);
	int nCount = m_PiList.GetCount();
	curPiDevID = m_PiList.GetCurSel();
	if (curPiDevID != LB_ERR)
	{
		m_PiList.GetLBText(curPiDevID,str);
		ParsePIipAndPort(str.GetBuffer(),curpinetparam);
	}

	if(nCount <= 0)
	{
		OnBnClickedEnumnetdevice();
		if(m_PiList.GetCount() <= 0)
		{
			msg("没有PI设备连接！");
			return ;
		}
	}
	else
	{
		for (int i = 0;i<nCount;i++)
		{
			m_PiList.GetLBText(i,str);
			string strTemp = str.GetBuffer();
			curID = ConnectPIRobot(strTemp);
			/*if (i == curPiDevID && curID != m_nPiDevID && curID>=0 && curPiDevID != LB_ERR)
			{
				m_nPiDevID = curID;
			}*/

		}
		//更新下拉框
		//OnBnClickedEnumnetdevice();
		nCount = m_PiList.GetCount();
		for (int i = 0;i<nCount;i++)
		{
			m_PiList.GetLBText(i,str);
			ParsePIipAndPort(str.GetBuffer(),pinetparam);
			if (pinetparam.ip == curpinetparam.ip)
			{
				nindex = i;
				m_nPiDevID = FindRobotIDByIp(curpinetparam.ip);
				break;
			}

		}
	}
    
	UpdateData(FALSE);
	msg("连接成功!,当前选择的设备(id:%d,ip:%s)",m_nPiDevID,curpinetparam.ip.c_str());
	return;
}

//运动模式设置
void CHSFoxxDlg::OnBnClickedLongdis()
{
	SetMoveType(EMOVE_LONG_STEP);
}


void CHSFoxxDlg::OnBnClickedMiddis()
{
	SetMoveType(EMOVE_MEDIU_STEP);
}


void CHSFoxxDlg::OnBnClickedShortdis()
{
	 SetMoveType(EMOVE_SHORT_STEP);
}


void CHSFoxxDlg::SetMoveType(int mvType)  //设置运动模式 m_iMoveType
{
	BOOL bEnable = TRUE;
	CString str = "";

	

	m_iMoveType = mvType;
	switch(m_iMoveType)
	{
	case EMOVE_CP:
		bEnable = FALSE;
		break;
	case EMOVE_LONG_STEP:
		str = "1.0000";
		break;
	case EMOVE_MEDIU_STEP:
		str = "0.10000";
		break;
	case EMOVE_SHORT_STEP:
		str = "0.01000";
		break;
	default:
		break;
	}
	for(int i = 0; i < PI_MAX_AXIS; i++)
	{
		m_edtStep[i].EnableWindow(bEnable);
		m_edtStep[i].SetWindowText(str);
	}
}

BOOL CHSFoxxDlg::PreTranslateMessage(MSG* pMsg)
{
	int dir = 0;   //默认负
	int iselect = 0;

	if (pMsg->message != WM_LBUTTONDOWN /*&& pMsg->message != WM_LBUTTONDBLCLK*/)
	{
		return CDialog::PreTranslateMessage(pMsg);
	}

/*	if(pMsg->message == WM_LBUTTONDBLCLK )
	{
		for(int index = 0; index < AA_MAXVISION; index++)
		{
			if (m_wndVideo[index].m_hWnd != pMsg->hwnd )
			{//不符合，继续查找
				continue;
			}
			ZoomPicture(index);
		}

	}
	else*/
	{ //方向盘

	for(int iAxis = 0; iAxis < PI_MAX_AXIS; iAxis++)
	{	
		if (m_btMoveL[iAxis].m_hWnd != pMsg->hwnd &&
			m_btMoveR[iAxis].m_hWnd != pMsg->hwnd)
		{//不符合，继续查找
			continue;
		}
		if (pMsg->message == WM_LBUTTONDOWN)
		{			
			if (m_btMoveR[iAxis].m_hWnd == pMsg->hwnd)		
			{//确定方向
				dir = 1;	
			}
			MoveOffset(iAxis, dir ? 1 : -1);
		}

	}
	}
	return CDialog::PreTranslateMessage(pMsg);
}

void CHSFoxxDlg::InitZoomPicture(int index )
{
	CRect rectFrame;
	GetDlgItem(IDC_VIDEOFRME)->GetWindowRect(&rectFrame);          
	ScreenToClient(&rectFrame); 

	//图像框布局:1,3行 第一二行3个，第3行1个；上面留5公分,左右下0公分，间距2公分
	{
		m_pictureFlag[index] = false;

		int width =  (rectFrame.Width()-2*AAVISONDIS )/3;
		int height = (rectFrame.Height()-2*AAVISONDIS -AAVISONTOP)/3;

		switch(index)
		{
		case 0:

			m_wndVideo[index].MoveWindow(rectFrame.left , rectFrame.top + AAVISONTOP ,width,  height, true);
			break;
		case 1:
			m_wndVideo[index].MoveWindow(rectFrame.left + AAVISONDIS +  width, rectFrame.top + AAVISONTOP  , width, height , true);
			break;
		case 2:
			m_wndVideo[index].MoveWindow(rectFrame.left + 2*AAVISONDIS +2*width , rectFrame.top + AAVISONTOP   , width, height , true);
			break;
		case 3:
			m_wndVideo[index].MoveWindow(rectFrame.left , rectFrame.top + AAVISONTOP + height, width, height , true);

			break;
		case 4:
			m_wndVideo[index].MoveWindow(rectFrame.left+ AAVISONDIS +  width , rectFrame.top + AAVISONTOP + height, width, height , true);
			break;
		case 5:
			m_wndVideo[index].MoveWindow(rectFrame.left+ 2*AAVISONDIS +  2*width , rectFrame.top + AAVISONTOP + height, width, height , true);
			break;
		case 6:
			width = (rectFrame.Width()-AAVISONDIS )/2;//最后一行只有2个框
			m_wndVideo[index].MoveWindow(rectFrame.left , rectFrame.top + 2*AAVISONTOP + 2*height, width, height , true);
			break;
		case 7:
			width = (rectFrame.Width()-AAVISONDIS )/2;//最后一行只有2个框
			m_wndVideo[index].MoveWindow(rectFrame.left + AAVISONDIS +  width , rectFrame.top + 2*AAVISONTOP + 2*height, width, height , true);
			break;
		default:
			break;
		}
	}

}
void CHSFoxxDlg::ZoomPicture(int index )
{
	CRect rectFrame;
	GetDlgItem(IDC_VIDEOFRME)->GetWindowRect(&rectFrame);          
	ScreenToClient(&rectFrame); 

	//图像框布局:1,3行 第一二行3个，第3行1个；上面留5公分,左右下0公分，间距2公分
	if (m_pictureFlag[index] == false)
	{
		m_pictureFlag[index] = true;
		for (int i= 0;i<AA_MAXVISION;i++)
		{
			if(index != i)
			{
				CRect rectDD;
				GetDlgItem(IDC_TEST_TEST)->GetWindowRect(&rectDD);           //IDC_WAVE_DRAW为Picture Control的ID  
				ScreenToClient(&rectDD);  
				m_wndVideo[index].MoveWindow(rectDD.left, rectDD.top, rectDD.Width(), rectDD.Height(), true);
				//m_wndVideo[index].MoveWindow(rectFrame.Width(), rectFrame.Height() , 2, 2, true);
			}
		}
		m_wndVideo[index].MoveWindow(rectFrame.left, rectFrame.top + AAVISONTOP, rectFrame.Width(), rectFrame.Height()-AAVISONTOP, true);

	
	}
	else
	{
		m_pictureFlag[index] = false;

		int width =  (rectFrame.Width()-2*AAVISONDIS )/3;
		int height = (rectFrame.Height()-2*AAVISONDIS -AAVISONTOP)/3;

		switch(index)
		{
		case 0:

			m_wndVideo[index].MoveWindow(rectFrame.left , rectFrame.top + AAVISONTOP ,width,  height, true);
			break;
		case 1:
			m_wndVideo[index].MoveWindow(rectFrame.left + AAVISONDIS +  width, rectFrame.top + AAVISONTOP  , width, height , true);
			break;
		case 2:
			m_wndVideo[index].MoveWindow(rectFrame.left + 2*AAVISONDIS +2*width , rectFrame.top + AAVISONTOP   , width, height , true);
			break;
		case 3:
			m_wndVideo[index].MoveWindow(rectFrame.left , rectFrame.top + AAVISONTOP + height, width, height , true);

			break;
		case 4:
			m_wndVideo[index].MoveWindow(rectFrame.left+ AAVISONDIS +  width , rectFrame.top + AAVISONTOP + height, width, height , true);
			break;
		case 5:
			m_wndVideo[index].MoveWindow(rectFrame.left+ 2*AAVISONDIS +  2*width , rectFrame.top + AAVISONTOP + height, width, height , true);
			break;
		case 6:
			 width = (rectFrame.Width()-AAVISONDIS )/2;//最后一行只有2个框
			m_wndVideo[index].MoveWindow(rectFrame.left , rectFrame.top + 2*AAVISONTOP + 2*height, width, height , true);
			break;
		case 7:
			width = (rectFrame.Width()-AAVISONDIS )/2;//最后一行只有2个框
			m_wndVideo[index].MoveWindow(rectFrame.left + AAVISONDIS +  width , rectFrame.top + 2*AAVISONTOP + 2*height, width, height , true);
			break;
		default:
			break;
		}
		 for (int i= 0;i<AA_MAXVISION;i++)
		 {
			 InitZoomPicture(i);

		 }
	}

}

LRESULT CHSFoxxDlg::OnMsgTimer(WPARAM wParam, LPARAM lParam)
{
	int type = wParam & 0x0f;   //type 占4位
	int val = wParam>>4;    //余下32位为值
	CString str;
	switch (type)
	{
	case TIMER_EDIT_TEXT:
		{

		double *p = (double*)lParam;
		str.Format("%.6lf", *p );
		GetDlgItem(val)->SetWindowText(str);
		}
		break;
	case  TIMER_SHOW_MSGBOX:          //延迟弹出消息提示框:
		 MessageBox((const char*)lParam, "Information", MB_ICONWARNING | MB_OK);
		 break;
     default:
		 break;
	}
	return 0;
}

void CHSFoxxDlg::MoveOffset(int iAxis, int iFlag)
{
	CString str;
	int i =0;

	//for( i = 0; i <PI_MAX_AXIS ; i++)
	i = iAxis;	
	{
		m_edtStep[i].GetWindowText(str);
		if (str == "")
		{
			PostMessage(WM_USER_TIMER, TIMER_SHOW_MSGBOX, (LPARAM)"请输入步进距离..");
			m_edtStep[i].SetFocus();
			return;
		}

		if (!get_str_type(str.GetBuffer()))
		{
			PostMessage(WM_USER_TIMER, TIMER_SHOW_MSGBOX, (LPARAM)"请输入数字..");
			m_edtStep[i].SetFocus();
			m_edtStep[i].SetSel(0, -1);
			return;
		}

		m_stepVal[i] = atof(str.GetBuffer());
		if (m_stepVal[i] < 0 || m_stepVal[i] > 10)
		{
			PostMessage(WM_USER_TIMER, TIMER_SHOW_MSGBOX, (LPARAM)"步进距离不能大于10mm..");
			m_edtStep[i].SetFocus();
			m_edtStep[i].SetSel(0, -1);
			return;
		}
	}

	const char* axis = NULL;
	if (iAxis == 0){axis = "X";}
	else if (iAxis == 1){axis = "Y";}
	else if (iAxis == 2){axis = "Z";}
	else if (iAxis == 3){axis = "U";}
	else if (iAxis == 4){axis = "V";}
	else if (iAxis == 5){axis = "W";}
	else {return;}

	double Parray = m_stepVal[iAxis] * iFlag;
	if (0 !=AARobotMoveRelative(m_nPiDevID, axis,&Parray))
	{
		PostMessage(WM_USER_TIMER, TIMER_SHOW_MSGBOX, (LPARAM)"移动失败..");
		
	}
	return;
}


void CHSFoxxDlg::OnBnClickedCurposition()
{
	char axis[] = "X Y Z U V W";
	double curPos[6] = {0};
	CString str;
	if (0 !=AARobotCurrentPosition(m_nPiDevID, axis,curPos))
	{
		PostMessage(WM_USER_TIMER, TIMER_SHOW_MSGBOX, (LPARAM)"获取位置失败..");

	}
	else
	{
		for(int i = 0; i < PI_MAX_AXIS; i++)
		{
			if (curPos[i] != m_posVal[i])
			{
				m_posVal[i] = curPos[i];
				str.Format("%.6lf", curPos[i] );
				m_edtVal[i].SetWindowText(str);
				//PostMessage(WM_USER_TIMER, (m_edtVal[i].GetDlgCtrlID()<<4) | TIMER_EDIT_TEXT, (LPARAM)&(curPos[i]));
			}
		}
	}

	return;
}

LRESULT CHSFoxxDlg::OnDisplayMsg(WPARAM wP, LPARAM lP)
{
	static int nLineCount = 0;

	char* lpszMsg = (LPSTR)wP;

	if (m_editMsg.GetSafeHwnd() != NULL)
	{
		if (nLineCount >= 10000)
		{
			nLineCount = 0;
			m_editMsg.Clear();
		}

		int len = m_editMsg.GetWindowTextLength();
		if (len >= 0)
		{
			if (len > 10000)
			{
				m_editMsg.Clear();
				m_editMsg.SetWindowText(lpszMsg);
			}
			else
			{
				m_editMsg.SetSel(len,len);
				//m_editMsg.SetSel(-1,-1);
				m_editMsg.ReplaceSel(lpszMsg);
			}
		}
	}
	if (wP != NULL)
	{
		free(lpszMsg);
	}
	return 0;
}





void CHSFoxxDlg::OnBnClickedBtnAaparam()
{
	CAA_PARAAM dlg;
	//m_pMainWnd = &dlg;
	int nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with OK
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with Cancel
	}
}


int CHSFoxxDlg::ConnectPIRobot(string &str)
{
	PI_NET_PARAM pinetparam ;
	memset(&pinetparam,0,sizeof(PI_NET_PARAM));
	int curRobotID = -1;
	if (AA_OK == ParsePIipAndPort(str,pinetparam))
	{
		map<string,CAARobot*>::iterator it;
		it = mapRobot.find(pinetparam.ip);
		if (it != mapRobot.end())
		{
			curRobotID = it->second->GetAARobotID();
			bool flag = it->second->IsAAConnected(curRobotID);
			if (flag == false)
			{
				curRobotID = it->second->ConnectAARobotByIPAndPort(pinetparam.ip.c_str(),pinetparam.port);
				if (curRobotID <0)
				{
					msg("选择的robot(ip为：%s)失去了连接,当前rootID:%d",pinetparam.ip.c_str(),m_nPiDevID);
				}
			}
			else
			{
				msg("选择的robot(ip为：%s)已经连接,当前rootID:%d",pinetparam.ip.c_str(),m_nPiDevID);
			}
		}
		else
		{
			//生成robot
			CAARobot *robot = new CAARobot(pinetparam.ip,pinetparam.port,curRobotID,true); 
			
			curRobotID = robot->ConnectAARobotByIPAndPort(pinetparam.ip.c_str(),pinetparam.port);
			if (curRobotID <0)
			{
				msg("没能连接上选择的robot(ip为：%s),当前rootID:%d",pinetparam.ip.c_str(),m_nPiDevID);
			}
			mapRobot.insert(map<string,CAARobot*>::value_type(pinetparam.ip,robot));

		}
	}


	/*
		if(true == CAARobot::IsAAConnectedByIpAndPort(pinetparam.ip.c_str(),pinetparam.port))
		{
			map<string,CAARobot*>::iterator it;
			it = mapRobot.find(pinetparam.ip);
			if (it != mapRobot.end())
			{
				curRobotID = it->second->GetAARobotID();
			}
			else
			{
				msg("robot(%s)是都被别的计算机连上了！",pinetparam.ip.c_str());
			}
		}
		else
		{		
			curRobotID = CAARobot::ConnectAARobotByIPAndPort(pinetparam.ip.c_str(),pinetparam.port);
			if (curRobotID <0)
			{
				msg("没能连接上选择的robot(ip为：%s)",pinetparam.ip.c_str());
			}
			else
			{	//把他重放到map中		
				map<string,CAARobot*>::iterator it;
				it = mapRobot.find(pinetparam.ip);
				if (it == mapRobot.end())
				{
					CAARobot *robot = new CAARobot(pinetparam.ip,pinetparam.port,curRobotID,true); 
					mapRobot.insert(map<string,CAARobot*>::value_type(pinetparam.ip,robot));			
				}
				else
				{
					if (it->second->GetAARobotID() != m_nPiDevID)
					{
						CAARobot *robot = new CAARobot(pinetparam.ip,pinetparam.port,curRobotID,true); 
						delete it->second;
						mapRobot.erase(it);
						mapRobot.insert(map<string,CAARobot*>::value_type(pinetparam.ip,robot));	
					}
					else
					{
						curRobotID = it->second->GetAARobotID();
					}			
				}
			}
		}	
	}*/
	return curRobotID;
}

void CHSFoxxDlg::OnCbnSelchangePitlist()
{
	// TODO: 在此添加控件通知处理程序代码
	int nIndex = m_PiList.GetCurSel();
	int nCount = m_PiList.GetCount();
	CString str;
	PI_NET_PARAM pinetparam ;
	memset(&pinetparam,0,sizeof(PI_NET_PARAM));
	if((nIndex != LB_ERR) && (nCount >0))
	{
		m_PiList.GetLBText(nIndex,str);
	}
	else
	{
		msg("没有设备！");
		return;
	}
	string strTemp = str.GetBuffer();
	int id = ConnectPIRobot(strTemp);
	if (id >=0 && id != m_nPiDevID)
	{
		m_nPiDevID = id;
		msg("更新的rootID:%d",m_nPiDevID);
	}
}

int CHSFoxxDlg::IsconnectedInMapRobot(string ip, int port)
{
	//port先预留
	int flag = -1;
	map<string,CAARobot*>::iterator it;
	it = mapRobot.find(ip);
	if (it != mapRobot.end())
	{
		flag = it->second->GetAARobotConnected();
	}
	return flag;
}

int CHSFoxxDlg::GetRobotIDByIndex(string strindex)
{
	string ip = CFG_INSTANCE->get_cfg(AAROBOTBIND,strindex.c_str());
	return FindRobotIDByIp(ip);
}

int CHSFoxxDlg::GetVisionIDByIndex(int index,string& cameraName)//根据视觉通道序号找视觉id,规定序号1->camera_1,依次类推,起始可以把这个序号当作id;
{
	CAAVision* vision = NULL;
	string name  = GetVisionUserNameByIndex(index);
	string uservisionName = CFG_INSTANCE->get_cfg(AAVISION_USERNAME,name.c_str());

	map<string,CAAVision*>::iterator it;

	for (it = mapVision.begin();it!=mapVision.end();it++)
	{
		if (it->first.find(uservisionName) != std::string::npos)
		{
			break;
		}
	}
	//it = mapVision.find(uservisionName);
	if (it != mapVision.end())
	{
		vision = it->second;
		cameraName = vision->m_pSensorNameA;
		return vision->m_iCurIDA;
	}
	else
	{
		return -1;
	}



}
string CHSFoxxDlg::GetVisionUserNameByIndex(int index)
{
	string  name ;
	switch(index)
	{
	case 1:
		name = UserName1;
		break;
	case 2:
		name = UserName2;
		break;
	case 3:
		name = UserName3;
		break;
	case 4:
		name = UserName4;
		break;
	case 5:
		name = UserName5;
		break;
	case 6:
		name = UserName6;
		break;
	case 7:
		name = UserName7;
		break;
	case 8:
		name = UserName8;
		break;
	default:
		name = "";
		break;

	}
	return name;

}

int CHSFoxxDlg::FindRobotIDByIp(string ip)
{
	int robotID = -1;

	map<string,CAARobot*>::iterator it;
	it = mapRobot.find(ip);
	if (it != mapRobot.end())
	{
		robotID = it->second->GetAARobotID();
	}
	if (robotID == -1)
	{
		msg("机器人(%s)没加载进来!",ip.c_str());
	}
	return robotID;

}

int CHSFoxxDlg::FindRobotIndexByID(int id)
{
	int index = -1;

	string ip1 = CFG_INSTANCE->get_cfg(AAROBOTBIND,AAROBOT1);
	string ip2 = CFG_INSTANCE->get_cfg(AAROBOTBIND,AAROBOT2);
	string ip3 = CFG_INSTANCE->get_cfg(AAROBOTBIND,AAROBOT3);
	string ip4 = CFG_INSTANCE->get_cfg(AAROBOTBIND,AAROBOT4);

	map<string,CAARobot*>::iterator it;

	for (it = mapRobot.begin();it != mapRobot.end();it++)
	{
		if (it->second->GetAARobotID() == id)
		{
			string ip = it->second->GetAARobotIP();
			if ( ip == ip1)
			{
				index = 0;
				break;

			}
			else if (ip == ip2)
			{
				index = 1;
				break;
			}
			else if (ip == ip3)
			{
				index = 2;
				break;
			}
			else if (ip == ip4)
			{
				index = 3;
				break;
			}

			
		}
	}
	if (it == mapRobot.end())
	{
		msg("机器人(id:%d)没加载进来!",id);
	}

	return index ;

}

vector<int> WINAPI CHSFoxxDlg::WorkNotify(int evt, AANotifyData data, void* lparam)
{
	CHSFoxxDlg* pDlg = (CHSFoxxDlg*)lparam;
	vector<int>result;

	pDlg->SendMessage(WM_MSG_NOTIFY, (WPARAM)evt, (LPARAM)&data);//静态函数调用成员函数是否需要发送消息的方式

	if (STARTAA == evt )
	{	switch(data.robotid1)
		{
			case 1:
			WaitForSingleObject(pDlg->AARobotWakeResult[0], INFINITE);//
			WaitForSingleObject(pDlg->AARobotWakeResult[1], INFINITE);//
			result.push_back(pDlg->AARobotProcessResult[0]);
			result.push_back(pDlg->AARobotProcessResult[1]);
			break;
			case 2:
			break;
			case 3:
			WaitForSingleObject(pDlg->AARobotWakeResult[2], INFINITE);//
			WaitForSingleObject(pDlg->AARobotWakeResult[3], INFINITE);//
			result.push_back(pDlg->AARobotProcessResult[2]);
			result.push_back(pDlg->AARobotProcessResult[3]);
			break;
			case 4:
			break;
			default:
			break;
		}
	}
	else if (RESETAA == evt)
	{
		WaitForSingleObject(pDlg->AARobotResetAAWakeResult[0], INFINITE);//等待复位结果
		WaitForSingleObject(pDlg->AARobotResetAAWakeResult[1], INFINITE);//等待复位结果
		result.push_back(pDlg->AARobotResetProcessResult[0]);
		result.push_back(pDlg->AARobotResetProcessResult[1]);

	}


	return result;
}
LRESULT CHSFoxxDlg::OnMsgNotify(WPARAM wParam, LPARAM lParam)
{
	int evt = (int)wParam;
	AANotifyData* pData = (AANotifyData*)lParam;
	int ret = 0;
	char *axisString = "X Y Z U V W";
	switch (evt)//事件类型,暂时只有一种
	{
	case STARTAA:

		if (pData->visionid==1 || pData->visionid==2)//序号为1，2的视觉对应1，2好机器人
		{
			AARobotProcessResult[0] = INVALID_RESULT;
			AARobotProcessResult[1] = INVALID_RESULT;
			AARobotProcessParam[0]= 2*pData->visionid-1;
			AARobotProcessParam[1]= 2*pData->visionid;

			if (DOUBLEAA ==  g_AAParam.AATYPE)
			{	
				EnterCriticalSection(&m_csRoot12); 
				m_intRobot12x = INVALID_RESULT;
				LeaveCriticalSection(&m_csRoot12);  
			}
			SetEvent(dlgAARobotProcessEvent[0]);
			SetEvent(dlgAARobotProcessEvent[1]);
		}
		else if (pData->visionid==3 || pData->visionid==4)
		{
			AARobotProcessResult[2] = INVALID_RESULT;
			AARobotProcessResult[3] = INVALID_RESULT;
			AARobotProcessParam[2]= 2*pData->visionid-1;
			AARobotProcessParam[3]= 2*pData->visionid;

			if (DOUBLEAA ==  g_AAParam.AATYPE)
			{	
				EnterCriticalSection(&m_csRoot34); 
				m_intRobot34x = INVALID_RESULT;
				LeaveCriticalSection(&m_csRoot34);  
			}
			SetEvent(dlgAARobotProcessEvent[2]);
			SetEvent(dlgAARobotProcessEvent[3]);
		}
		
		break;

	case RESETAA:
		//先阻塞这执行,接收到指令后,在主线程里阻塞执行,但依然保留数组的形式,以防外面修改
		int robotID1,robotID2;
		if (pData->visionid == 1 || pData->visionid == 2)
		{
			robotID1 = GetRobotIDByIndex(AAROBOT1);//在StartAA中判断rootID的正确性
			robotID2 = GetRobotIDByIndex(AAROBOT2);//在StartAA中判断rootID的正确性
		}
		else if(pData->visionid == 3 || pData->visionid == 4)
		{
			robotID1 = GetRobotIDByIndex(AAROBOT3);//在StartAA中判断rootID的正确性
			robotID2 = GetRobotIDByIndex(AAROBOT4);//在StartAA中判断rootID的正确性

		}
		AARobotResetProcessResult[0] = GoInitPosRobot(robotID1,axisString,AAINDEX1,NULL);
		AARobotResetProcessResult[1] = GoInitPosRobot(robotID2,axisString,AAINDEX1,NULL);
		SetEvent(AARobotResetAAWakeResult[0]);
		SetEvent(AARobotResetAAWakeResult[1]);
		break;
	default:
		break;
	}
	return ret;
}

int WINAPI CHSFoxxDlg::AARobotProcess1(void* param)//先启动线程
{
	CHSFoxxDlg* pWork = (CHSFoxxDlg*)param;
	char *axisString = "X Y Z U V W";
	string cameraName;
	int ret ;
	while (!pWork->m_dlgExit)
	{
		WaitForSingleObject(pWork->dlgAARobotProcessEvent[0], INFINITE);
		if (pWork->m_dlgExit != false)
		{
			return 0;
		}
		//....处理....每个线程对应的机器人和视觉通道都已知，可以写在配置文件中

		int robotID = pWork->GetRobotIDByIndex(AAROBOT1);//在StartAA中判断rootID的正确性

		//调整之前先把机器人移动到初始位置
		
		pWork->AARobotProcessResult[0] =  pWork->GoInitPosRobot(robotID,axisString,AAINDEX2,NULL);
		int visionIndex = pWork->AARobotProcessParam[0] ;
			//规定视觉序号为1，2，3，4;发序号1:表示启动机器人1,2,视觉通道1,2, 但是视觉通道1是都对应机器人1不知道，那就改掉名字，使得
			//机器人1对应camera_1,cameram_3,机器人2对应camera_2,cameram_4
		
		int visionID = pWork->GetVisionIDByIndex(visionIndex,cameraName);

		ret = pWork->__OpenAAVision(cameraName,pWork->m_playvision[visionID]);
		if (0 == pWork->AARobotProcessResult[0] && ret == DT_ERROR_OK)
		{
			
			pWork->AARobotProcessResult[0] = pWork->StartAA(robotID,visionID);
			
			msg("线程1,视觉通道:%d,机器人id:%d,视觉id:%d,调整结果:%d",visionIndex,robotID,visionID,pWork->AARobotProcessResult[0]);
		}
		else
		{
			msg("线程1,视觉通道:%d,机器人id:%d,视觉id:%d 回初始位置失败!",visionIndex,robotID,visionID);
		}

		//看是否是双摄：后完成调整的要以前面完成调整的作为基准进行再次调整	
		//对于双摄像头暂定策略为: 两个摄像头同时aa调整,调整完后,以第一个摄像头为基础再次调整第二个摄像头
		//即192.168.0.141/143为基准,调整192.168.0.142/144
		if (DOUBLEAA ==  g_AAParam.AATYPE)
		{	
			EnterCriticalSection(&(pWork->m_csRoot12)); 
			if ( INVALID_RESULT == pWork->m_intRobot12x)
			{
				pWork->m_intRobot12x = 1;  
			
			}
			else
			{
				int robotindex = pWork->FindRobotIndexByID(robotID);
				int fixedvisionID= pWork->FindBinderIDByID(visionID);
				ret = pWork->StartDoubleAA(robotID,fixedvisionID,visionID);
			}

			LeaveCriticalSection(&(pWork->m_csRoot12));  
		}

		SetEvent(pWork->AARobotWakeResult[0]);
		WaitForSingleObject(pWork->dlgAARobotProcessEvent[0], 0);//立即返回,这里只是测试,可要可不要
		Sleep(50);
	}

	return 0;


}
int WINAPI CHSFoxxDlg::AARobotProcess2(void* param)
{
	CHSFoxxDlg* pWork = (CHSFoxxDlg*)param;
	char *axisString = "X Y Z U V W";
	string cameraName;
	while (!pWork->m_dlgExit)
	{
		WaitForSingleObject(pWork->dlgAARobotProcessEvent[1], INFINITE);
		if (pWork->m_dlgExit != false)
		{
			return 0;
		}
		//....处理....每个线程对应的机器人和视觉通道都已知，可以写在配置文件中
		int robotID = pWork->GetRobotIDByIndex(AAROBOT2);//在StartAA中判断rootID的正确性
		pWork->AARobotProcessResult[1] =  pWork->GoInitPosRobot(robotID,axisString,AAINDEX2,NULL);
		int visionIndex = pWork->AARobotProcessParam[1];
		int visionID = pWork->GetVisionIDByIndex(visionIndex,cameraName);

		int ret = pWork->__OpenAAVision(cameraName,pWork->m_playvision[visionID]);
		if (0 == pWork->AARobotProcessResult[1] && ret == DT_ERROR_OK)
		{
			pWork->AARobotProcessResult[1] = pWork->StartAA(robotID,visionID);
			msg("线程2,视觉通道:%d,机器人id:%d,视觉id:%d,调整结果:%d",visionIndex,robotID,visionID,pWork->AARobotProcessResult[1]);
		}
		else
		{
			msg("线程2,视觉通道:%d,机器人id:%d,视觉id:%d 回初始位置失败!",visionIndex,robotID,visionID);
		}
		//看是否是双摄：后完成调整的要以前面完成调整的作为基准进行再次调整	
		if (DOUBLEAA ==  g_AAParam.AATYPE)
		{	
			EnterCriticalSection(&(pWork->m_csRoot12)); 
			if ( INVALID_RESULT == pWork->m_intRobot12x)
			{
				pWork->m_intRobot12x = 1;  

			}
			else
			{
				int robotindex = pWork->FindRobotIndexByID(robotID);
				int fixedvisionID= pWork->FindBinderIDByID(visionID);
				ret = pWork->StartDoubleAA(robotID,fixedvisionID,visionID);
			}

			LeaveCriticalSection(&(pWork->m_csRoot12));  
		}
		SetEvent(pWork->AARobotWakeResult[1]);
		WaitForSingleObject(pWork->dlgAARobotProcessEvent[1], 0);//立即返回,这里只是测试,可要可不要
		Sleep(50);
	}
	return 0;
}
int WINAPI CHSFoxxDlg::AARobotProcess3(void* param)
{
	CHSFoxxDlg* pWork = (CHSFoxxDlg*)param;
	char *axisString = "X Y Z U V W";
	string cameraName;
	while (!pWork->m_dlgExit)
	{
		WaitForSingleObject(pWork->dlgAARobotProcessEvent[2], INFINITE);
		if (pWork->m_dlgExit != false)
		{
			return 0;
		}
		int robotID = pWork->GetRobotIDByIndex(AAROBOT3);//在StartAA中判断rootID的正确性
		pWork->AARobotProcessResult[2] =  pWork->GoInitPosRobot(robotID,axisString,AAINDEX2,NULL);	
		int visionIndex = pWork->AARobotProcessParam[2];
		int visionID = pWork->GetVisionIDByIndex(visionIndex,cameraName);

		int ret = pWork->__OpenAAVision(cameraName,pWork->m_playvision[visionID]);
		if (0 == pWork->AARobotProcessResult[2] && ret == DT_ERROR_OK)
		{
			pWork->AARobotProcessResult[2] = pWork->StartAA(robotID,visionID);
			msg("线程3,视觉通道:%d,机器人id:%d,视觉id:%d,调整结果:%d",visionIndex,robotID,visionID,pWork->AARobotProcessResult[2]);
		}
		else
		{
			msg("线程3,视觉通道:%d,机器人id:%d,视觉id:%d 回初始位置失败!",visionIndex,robotID,visionID);
		}
		//看是否是双摄：后完成调整的要以前面完成调整的作为基准进行再次调整	
		if (DOUBLEAA ==  g_AAParam.AATYPE)
		{	
			EnterCriticalSection(&(pWork->m_csRoot34)); 
			if ( INVALID_RESULT == pWork->m_intRobot34x)
			{
				pWork->m_intRobot34x = 1;  

			}
			else
			{
				int robotindex = pWork->FindRobotIndexByID(robotID);
				int fixedvisionID= pWork->FindBinderIDByID(visionID);
				ret = pWork->StartDoubleAA(robotID,fixedvisionID,visionID);
			}

			LeaveCriticalSection(&(pWork->m_csRoot34));  
		}
		SetEvent(pWork->AARobotWakeResult[2]);
		WaitForSingleObject(pWork->dlgAARobotProcessEvent[2], 0);//立即返回,这里只是测试,可要可不要
		Sleep(50);
	}
	return 0;
}
int WINAPI CHSFoxxDlg::AARobotProcess4(void* param)
{
	CHSFoxxDlg* pWork = (CHSFoxxDlg*)param;
	char *axisString = "X Y Z U V W";
	string cameraName;
	while (!pWork->m_dlgExit)
	{
		WaitForSingleObject(pWork->dlgAARobotProcessEvent[3], INFINITE);
		if (pWork->m_dlgExit != false)
		{
			return 0;
		}
		int robotID = pWork->GetRobotIDByIndex(AAROBOT4);//在StartAA中判断rootID的正确性
		pWork->AARobotProcessResult[3] =  pWork->GoInitPosRobot(robotID,axisString,AAINDEX2,NULL);
	
			int visionIndex = pWork->AARobotProcessParam[3];
			int visionID = pWork->GetVisionIDByIndex(visionIndex,cameraName);

			int ret = pWork->__OpenAAVision(cameraName,pWork->m_playvision[visionID]);
			if (0 == pWork->AARobotProcessResult[3] && ret == DT_ERROR_OK)
		{
			pWork->AARobotProcessResult[3] = pWork->StartAA(robotID,visionID);
			msg("线程4,视觉通道:%d,机器人id:%d,视觉id:%d,调整结果:%d",visionIndex,robotID,visionID,pWork->AARobotProcessResult[3]);
		}
		else
		{
			msg("线程4,视觉通道:%d,机器人id:%d,视觉id:%d 回初始位置失败!",visionIndex,robotID,visionID);
		}
			//看是否是双摄：后完成调整的要以前面完成调整的作为基准进行再次调整	
			if (DOUBLEAA ==  g_AAParam.AATYPE)
			{	
				EnterCriticalSection(&(pWork->m_csRoot34)); 
				if ( INVALID_RESULT == pWork->m_intRobot34x)
				{
					pWork->m_intRobot34x = 1;  

				}
				else
				{
					int robotindex = pWork->FindRobotIndexByID(robotID);
					int fixedvisionID= pWork->FindBinderIDByID(visionID);
					ret = pWork->StartDoubleAA(robotID,fixedvisionID,visionID);
				}

				LeaveCriticalSection(&(pWork->m_csRoot34));  
			}
		SetEvent(pWork->AARobotWakeResult[3]);
		WaitForSingleObject(pWork->dlgAARobotProcessEvent[3], 0);//立即返回,这里只是测试,可要可不要
		Sleep(50);
	}
	return 0;
}

int CHSFoxxDlg::FindBinderIDByID(int visionID)
{
	int ret = -1;
	string username  = FindVisionUsernameByID(visionID);
	CAAVision *vision = NULL;
	if (username.empty())
	{
		msg("找不到视觉ID:%d的定义的名字",visionID);
		return -1;
	}
	string bindername = FindbinderUsernameByName( username);

	if (!bindername.empty())//找到了
	{
		vision = FindVisionInMap(bindername);
		return vision->m_iDevIDA;
	}
	return -1;
}

string CHSFoxxDlg::FindVisionUsernameByID(int visionID)
{
	string username;
	map<string,CAAVision*>::iterator it;
	CAAVision *vision = NULL;
	//在map中找有字符串为camera_0的项
	for (it = mapVision.begin();it != mapVision.end();it++)
	{
		if (it->second->m_iDevIDA == visionID )
		{
			vision =  it->second;
			break;
		}
	}
	if (NULL != vision)
	{
		username = vision->m_pSensorNameA.substr(vision->m_pSensorNameA.find("@")+1);
	}

	return username;

}
CAAVision* CHSFoxxDlg::FindVisionByID(int visionID)
{
	CAAVision *vision = NULL;
	map<string,CAAVision*>::iterator it;
	//在map中找当前视觉id
	for (it = mapVision.begin();it != mapVision.end();it++)
	{
		if (it->second->m_iCurIDA == m_nDevID)
		{
			vision = it->second;
			break;
		}
	}
	if (vision == NULL )
	{
		msg("当前视觉不存在,视觉id:%d",m_nDevID);
	}

	return vision;

}
CAAVision*  CHSFoxxDlg::FindVisionInMap(string username)
{
	
	map<string,CAAVision*>::iterator it;
	//在map中找有字符串为camera_0这样的项
	for (it = mapVision.begin();it != mapVision.end();it++)
	{
		if (it->first.find(username) !=std::string::npos )
		{
			return it->second;
		}
	}
	return NULL;
}
int CHSFoxxDlg::__OpenAAVision(string visionName,CButton &bt)
{
	int ret = DT_ERROR_OK;
	if (visionName.empty())
	{
		return  -1;
	}
	CAAVision *vision = FindVisionInMap(visionName);
	if (!vision)
	{
		msg("通道:%s不存在 map中!",visionName.c_str());
		bt.SetCheck(FALSE);
		return -1;
	}
		if (vision->m_iWorkStatusA ==  TESTOR_START)//视频已经打开
		{
			msg("vision(%s)视频已经打开！",visionName.c_str());
			return ret;
		}
		if((ret = vision->OpenVisionDevice()) != DT_ERROR_OK)
		{
			msg("vision(%s)设备打开失败！",visionName.c_str());
			bt.SetCheck(FALSE);
			return -1;
		}
		HWND h = GetHWndByVisionId(vision->m_iDevIDA);
		if((ret = vision->OpenVisionVideo(h)) != DT_ERROR_OK)
		{
			msg("vision(%s)视频打开失败！",visionName.c_str());
			bt.SetCheck(FALSE);
			return -1;
		}

		msg("vision(%s)视频打开成功,前id为:%d,当前id为:%d !",visionName.c_str(),m_nDevID,vision->m_iDevIDA);
		m_nDevID = vision->m_iDevIDA;//
		//把下拉框中的选项选为当前vision;
		int nIndex = m_KitListCtrl.GetCurSel();
		int nCount = m_KitListCtrl.GetCount();
		CString str;
		string strTemp;
		if(nCount <= 0)
		{
			msg("没有设备！");
		}
		for (int i=0;i<nCount;i++)
		{
			m_KitListCtrl.GetLBText(nIndex,str);
			strTemp = str.GetBuffer();
			if (strTemp.find(visionName) !=std::string::npos)
			{
				m_KitListCtrl.SetCurSel(i);
				break;
			}
		}

		Sleep(5000);//等待抓图线程跟显示线程准备
		bt.SetCheck(TRUE);

		return ret;
}

int CHSFoxxDlg::OpenAAVision(string visionName,CButton &bt)
{
	int ret = DT_ERROR_OK;
	CAAVision *vision = FindVisionInMap(visionName);
	if (!vision)
	{
		msg("通道:%s不存在 map中!",visionName.c_str());
		bt.SetCheck(FALSE);
		return -1;
	}

	if (bt.GetCheck() == TRUE)//打开视频
	{
		if (vision->m_iWorkStatusA ==  TESTOR_START)//视频已经打开
		{
			msg("vision(%s)视频已经打开！",visionName.c_str());
			return ret;
		}
		if((ret = vision->OpenVisionDevice()) != DT_ERROR_OK)
		{
			msg("vision(%s)设备打开失败！",visionName.c_str());
			m_playvision[0].SetCheck(FALSE);
			return -1;
		}
		HWND h = GetHWndByVisionId(vision->m_iDevIDA);
		if((ret = vision->OpenVisionVideo(h)) != DT_ERROR_OK)
		{
			msg("vision(%s)视频打开失败！",visionName.c_str());
			bt.SetCheck(FALSE);
			return -1;
		}

		msg("vision(%s)视频打开成功,前id为:%d,当前id为:%d !",visionName.c_str(),m_nDevID,vision->m_iDevIDA);
		m_nDevID = vision->m_iDevIDA;//
		//把下拉框中的选项选为当前vision;
		int nIndex = m_KitListCtrl.GetCurSel();
		int nCount = m_KitListCtrl.GetCount();
		CString str;
		string strTemp;
		if(nCount <= 0)
		{
			msg("没有设备！");
		}
		for (int i=0;i<nCount;i++)
		{
			m_KitListCtrl.GetLBText(nIndex,str);
			strTemp = str.GetBuffer();
			if (strTemp.find(visionName) !=std::string::npos)
			{
				m_KitListCtrl.SetCurSel(i);
				break;
			}
		}
	}
	else//关闭
	{
		if((ret = vision->CloseVisionVideo()) == DT_ERROR_OK)
		{
			if (vision->m_iDevIDA == m_nDevID)
			{
				//m_KitListCtrl.SetCurSel(0);//关闭后选择第一项
				//m_nDevID = 0;//简单处理一下
			}

		}

	}
	return ret;
}
void CHSFoxxDlg::OnBnClickedPlayVision0()
{
	// 检查是否有枚举到对应的通道,视频0对应camera_1
	int ret = DT_ERROR_OK;
	string visionName = CFG_INSTANCE->GetCfg(AAVISION_USERNAME,UserName1);//"Camera_1";//是否名字改为自定义的名字
	ret = OpenAAVision(visionName,m_playvision[0]);

}


void CHSFoxxDlg::OnBnClickedPlayVision1()
{
	int ret = DT_ERROR_OK;
	string visionName = CFG_INSTANCE->GetCfg(AAVISION_USERNAME,UserName2);
	ret = OpenAAVision(visionName,m_playvision[1]);
}


void CHSFoxxDlg::OnBnClickedPlayVision2()
{
	int ret = DT_ERROR_OK;
	string visionName = CFG_INSTANCE->GetCfg(AAVISION_USERNAME,UserName3);
	ret = OpenAAVision(visionName,m_playvision[2]);
}


void CHSFoxxDlg::OnBnClickedPlayVision3()
{
	int ret = DT_ERROR_OK;
	string visionName = CFG_INSTANCE->GetCfg(AAVISION_USERNAME,UserName4);
		ret = OpenAAVision(visionName,m_playvision[3]);
}


void CHSFoxxDlg::OnBnClickedPlayVision4()
{
	int ret = DT_ERROR_OK;
	string visionName = CFG_INSTANCE->GetCfg(AAVISION_USERNAME,UserName5);
		ret = OpenAAVision(visionName,m_playvision[4]);
}


void CHSFoxxDlg::OnBnClickedPlayVision5()
{
	int ret = DT_ERROR_OK;
	string visionName = CFG_INSTANCE->GetCfg(AAVISION_USERNAME,UserName6);
		ret = OpenAAVision(visionName,m_playvision[5]);
}


void CHSFoxxDlg::OnBnClickedPlayVision6()
{
	int ret = DT_ERROR_OK;
	string visionName = CFG_INSTANCE->GetCfg(AAVISION_USERNAME,UserName7);
	ret = OpenAAVision(visionName,m_playvision[6]);
}


void CHSFoxxDlg::OnBnClickedPlayVision7()
{
	int ret = DT_ERROR_OK;
	string visionName = CFG_INSTANCE->GetCfg(AAVISION_USERNAME,UserName8);//"Camera_1";//是否名字改为自定义的名字
	ret = OpenAAVision(visionName,m_playvision[7]);
}





void CHSFoxxDlg::OnStnClickedVideo0()
{
	// TODO: 在此添加控件通知处理程序代码
	ZoomPicture(0);

}


void CHSFoxxDlg::OnStnClickedVideo1()
{
	// TODO: 在此添加控件通知处理程序代码
	ZoomPicture(1);
}


void CHSFoxxDlg::OnStnClickedVideo2()
{
	// TODO: 在此添加控件通知处理程序代码
	ZoomPicture(2);
}


void CHSFoxxDlg::OnStnClickedVideo3()
{
	// TODO: 在此添加控件通知处理程序代码
	ZoomPicture(3);
}


void CHSFoxxDlg::OnStnClickedVideo4()
{
	// TODO: 在此添加控件通知处理程序代码
	ZoomPicture(4);
}


void CHSFoxxDlg::OnStnClickedVideo5()
{
	// TODO: 在此添加控件通知处理程序代码
	ZoomPicture(5);
}


void CHSFoxxDlg::OnStnClickedVideo6()
{
	// TODO: 在此添加控件通知处理程序代码
	ZoomPicture(6);
}


void CHSFoxxDlg::OnStnClickedVideo7()
{
	// TODO: 在此添加控件通知处理程序代码
	ZoomPicture(7);
}


void CHSFoxxDlg::OnBnClickedConnectclose()
{
	// TODO: 在此添加控件通知处理程序代码

	int nIndex = m_PiList.GetCurSel();
	int nCount = m_PiList.GetCount();
	CString str;
	PI_NET_PARAM pinetparam ;
	memset(&pinetparam,0,sizeof(PI_NET_PARAM));
	int curRobotID = -1;
	if((nIndex != LB_ERR) && (nCount >0))
	{
		m_PiList.GetLBText(nIndex,str);
	}
	else
	{
		msg("没有设备！");
		return;
	}
	string strTemp = str.GetBuffer();
	
	if (AA_OK == ParsePIipAndPort(strTemp,pinetparam))
	{

		map<string,CAARobot*>::iterator it;
		it = mapRobot.find(pinetparam.ip);
		if (it != mapRobot.end())
		{
			curRobotID = it->second->GetAARobotID();
			bool flag = it->second->IsAAConnected(curRobotID);
			if (flag == false)
			{
				it->second->CloseAAConnection(curRobotID);

			}
			else
			{
				msg("选择的robot(ip为：%s)已经断开",pinetparam.ip.c_str());
			}
		}

	}
	else
	{
		msg("ip解析错误");
	}

	if((nIndex != LB_ERR) && (nCount >0))
	{
		m_PiList.GetLBText(0,str);
	    strTemp = str.GetBuffer();

		ParsePIipAndPort(strTemp,pinetparam);
		map<string,CAARobot*>::iterator it;
		it = mapRobot.find(pinetparam.ip);
		if (it != mapRobot.end())
		{
			curRobotID = it->second->GetAARobotID();
		}
		else
		{
			m_nPiDevID = -1;
		}

	}
	else
	{
		m_nPiDevID = -1;
	}
}





void CHSFoxxDlg::OnBnClickedPicturecenter()
{
	// TODO: 在此添加控件通知处理程序代码

	CButton* pBtn = (CButton*)GetDlgItem(IDC_PICTURECENTER);
	int state = pBtn->GetCheck();
	if (state == TRUE)
	{
		CAAVision::SetPaintFlag(TRUE);
		 
	}else{
		CAAVision::SetPaintFlag(FALSE);

	}


}
int CHSFoxxDlg::DobuleAATestReadPic(int robotID,int cameraID)
{
	Mat  movMat,fixMat;
	int ret = AA_OK;

	msg("请注意双摄靶图是不是符合要求!!!!!");

	msg("请在在.\\image目录下放入fix.jpg,mov.jpg两张图片！");
	int flag = ReadPictureMat(&movMat,".\\image\\mov.jpg",cameraID);
	if(flag != TRUE)
	{
		msg("获取原始图片失败，请在在.\\image目录下放入fix.jpg,mov.jpg");
		return AA_GET_PICTIRE_FAIL;
	}
	flag = ReadPictureMat(&fixMat,".\\image\\fix.jpg",cameraID);
	if(flag != TRUE)
	{
		msg("获取原始图片失败，请在在.\\image目录下放入fix.jpg,mov.jpg");
		return AA_GET_PICTIRE_FAIL;
	}
	//缩小图小,提高速度
	movMat = Img_resize(movMat, g_AAParam.scale);
	fixMat = Img_resize(fixMat, g_AAParam.scale);

/*
	double dis = 18;///两个芯片之间的距离
	double scale = 1; //缩小的倍数, 考虑是否跟单摄像头的缩小倍数一致
	double pixel_size = 0.00112 / scale;
	double f = 3.31;
	double D = 600;
	*/


	Para Para;
	Para = calPara(fixMat, movMat, g_AAParam.doubleAAdis, g_AAParam.pixel_size/g_AAParam.doubleAAscale, g_AAParam.focus, g_AAParam.objdis);
	double adjust_w = Para.w;
	double adjust_u = Para.v;
	double adjust_v = -Para.u;

	return ret;



}

void CHSFoxxDlg::OnBnClickedReadpicTest()
{
	int ret = AA_OK;
	if (g_AAParam.AATYPE == DOUBLEAA)
	{
		
		ret=  DobuleAATestReadPic(m_nPiDevID,m_nDevID);
		return ;

	}

	//在image中放置3或者5张图片,命名为p1 o m1 或者 p2,p1 ,o,m1,m2进行测试
	//一，调整z

	msg("请在在.\\image目录下放入p2.jpg,p1.jpg,o.jpg,m1.jpg,m2.jpg五张图片！");
	double distanceToOriginal[10]={0};
	int  first_z_number = AdjustZvalueReadPic(m_nPiDevID,m_nDevID);
	if (first_z_number<0)
	{
		return ;
	}
	//二：然后需再调整U/V轴
	int  adjust_uv_number = AdjustUVvalueReadPic(m_nPiDevID,m_nDevID);
	if (adjust_uv_number<0)
	{
		return ;
	}

	int  adjust_zuv_number = 0;
	//三：5点法
	//adjust_zuv_number = FiveAdjustvalueReadPic( m_nPiDevID,m_nDevID);
	//************************************************************************************************************
	//四：调整x,y
	//ret = AdjustXYvalueReadPic( m_nPiDevID,m_nDevID);


}

int CHSFoxxDlg::AdjustZvalueReadPic(int robotID,int cameraID)
{
	//第一步：首先需要调整Z轴
	int temp_number=0;
	int  z_adjustnum =0;
	double zadjust_zthr =0;
	double distanceToOriginal[10]={0};
	double AAadjust_z = -100.0;
	Mat Ori_PictureMat;
	Mat Plus_PictureMat;
	Mat Minus_PictureMat;
	const char axisString[5]="Z ";

	z_adjustnum = CFG_INSTANCE->get_cfg_int(AA_Adjustparam,AAadjust_zMaxNum);
	zadjust_zthr = CFG_INSTANCE->get_cfg_double(AA_Adjustparam,AAadjust_zthr);
	ReadAAAdjustZValue(distanceToOriginal,z_adjustnum);

	//while(!(( fabs(AAadjust_z)<zadjust_zthr)||(temp_number>=z_adjustnum)))
	{
		//step1:保存当前帧的图片以及该图像正负向调整的像.......	
		int Ori_flag = ReadPictureMat(&Ori_PictureMat,".\\image\\o.jpg",cameraID);
		if(Ori_flag != TRUE)
		{
			msg("获取原始图片失败，请在在.\\image目录下放入p2.jpg,p1.jpg,o.jpg,m1.jpg,m2.jpg");return AA_GET_PICTIRE_FAIL;
		}

		int Plus_flag = ReadPictureMat(&Plus_PictureMat,".\\image\\p1.jpg",cameraID);
		if(Plus_flag != TRUE)
		{
			msg("获取距离为:%.6f原始图片失败,调整次数:%d",distanceToOriginal[temp_number],temp_number+1);
			return AA_GET_PICTIRE_FAIL;
		}

		int Minus_flag=ReadPictureMat(& Minus_PictureMat,".\\image\\m1.jpg",cameraID);
		if(Minus_flag != TRUE)
		{
			msg("获取原始图片失败");return AA_GET_PICTIRE_FAIL;
		}

		//step2.计算移动的距离.
		Mat img_ori =Ori_PictureMat;
		Mat img_p =Plus_PictureMat;
		Mat img_m =Minus_PictureMat;
		img_ori = Img_resize(img_ori, g_AAParam.scale);
		img_p = Img_resize(img_p, g_AAParam.scale);
		img_m = Img_resize(img_m, g_AAParam.scale);
		State state;
		int OneSide_flag;
		state = getState(img_ori,img_p,img_m,distanceToOriginal[temp_number],-distanceToOriginal[temp_number],g_AAParam.pixel_size/g_AAParam.scale,g_AAParam.scale,
			cameraID,&OneSide_flag);
		double adjust_z = state.deltaZ; //
		//step3..根据计算结果进行移动..
		const char posStr[16] = "Z U V";
		double pos[3] ={0.0};
		if(OneSide_flag>=0)
		{
			pos[0]=adjust_z;
		}
		else if(OneSide_flag==-1)
		{
			pos[0]=adjust_z;
			msg("单边情况!继续调整");
		}
		else
		{
			msg("找不到特征点,返回!OneSide_flag:%d,",OneSide_flag);
			
		}
		//调整之前测试是否在可调范围内,是否超出软限位

		msg("z轴第%d次调整,调整距离为:%.6lf,阈值为:%.6lf,flag:%d",temp_number,pos[0],zadjust_zthr,OneSide_flag);

		AAadjust_z = adjust_z;	
	}
	msg("调整Z轴完毕！调整次数:%d",temp_number);
	return 		temp_number;

}
int CHSFoxxDlg::AdjustUVvalueReadPic(int robotID,int cameraID)
{
	double AAadjust_u=-100.0;
	double AAadjust_v=-100.0;
	int temp_number=0;
	double distanceToOriginalUV[10]={0};
	Mat Ori_PictureMat;
	Mat Plus_PictureMat;
	Mat Minus_PictureMat;
	const char axisString[5]="Z ";
	int uv_adjustnum = CFG_INSTANCE->get_cfg_int(AA_Adjustparam,AAadjust_uvMaxNum);
	double adjust_uvthr = CFG_INSTANCE->get_cfg_double(AA_Adjustparam,AAadjust_uvthr);
	ReadAAAdjustUVValue(distanceToOriginalUV,uv_adjustnum);
	while(!(( fabs(AAadjust_u)<adjust_uvthr && fabs(AAadjust_v)<adjust_uvthr)||(temp_number>=uv_adjustnum)))
	{
		//step1:获取当前帧的图片以及该图像正负向调整的图像..
		int Ori_flag=ReadPictureMat(&Ori_PictureMat,".\\image\\o.jpg",cameraID);
		if(Ori_flag != TRUE)
		{
			msg("获取原始图片失败,返回");
			return AA_GET_PICTIRE_FAIL;
		}

		int Plus_flag=ReadPictureMat(&Plus_PictureMat,".\\image\\p1.jpg",cameraID);
		if(Plus_flag != TRUE)
		{
			msg("获取正向原始图片失败,返回！");
			return AA_GET_PICTIRE_FAIL;
		}

		int Minus_flag= ReadPictureMat(& Minus_PictureMat,".\\image\\m1.jpg",cameraID);
		if(Minus_flag != TRUE)
		{
			msg("获取负向原始图片失败,返回！");
			return AA_GET_PICTIRE_FAIL;
		}


		//step2:..计算移动的距离.
		Ori_PictureMat = Img_resize(Ori_PictureMat, g_AAParam.scale);
		Plus_PictureMat = Img_resize(Plus_PictureMat, g_AAParam.scale);
		Minus_PictureMat = Img_resize(Minus_PictureMat, g_AAParam.scale);
		State state ={0};
		int OneSide_flag = -3;
		state = getState(Ori_PictureMat,Plus_PictureMat,Minus_PictureMat,distanceToOriginalUV[temp_number],-distanceToOriginalUV[temp_number],g_AAParam.pixel_size/g_AAParam.scale,g_AAParam.scale,
			cameraID,&OneSide_flag);
		double adjust_u = state.y_angle ;
		double adjust_v = state.x_angle ;

		//step3:根据计算结果进行移动..
		const char posStr[16] = "Z U V";
		double pos[3];
		pos[0]=0;

		if(OneSide_flag==0)
		{
			pos[1]=-adjust_u;
			pos[2]=-adjust_v;
		}
		else if(OneSide_flag==-1)
		{
			pos[1]=-adjust_u;
			pos[2]=-adjust_v;
			msg("uv计算出单边继续!");
		}
		else
		{
			msg("找不到特征点!OneSide_flag:%d,",OneSide_flag);
			//return AA_Z_ADJUST_BIG_FAIL;
		}

		const char posStrV[16] = "V";
		int result =0;
		//int result=AARobotMoveRelative(robotID,posStrV,pos + 2);
		if(result<0)
		{
			msg("V旋转角度为:%.6lf,%.6lf,超出机器运动范围,继续",pos[1],pos[2]);
			//temp_number++;
			//continue;
		}

		const char posStrU[16] = "U";
		result=AARobotMoveRelative(robotID,posStrU, pos +1);

		//result=AARobotMoveRelative(robotID,posStr, pos);
		if(result<0)
		{
			msg("U旋转角度为:%.6lf,%.6lf,超出机器运动范围,继续",pos[1],pos[2]);
			//temp_number++;
			//continue;
		}


		temp_number++;
		msg("uv轴第%d次调整,调整距离为du:%.6lf,dv:%.6lf,阈值为:%.6lf,flag:%d",temp_number,pos[1],pos[2],adjust_uvthr,OneSide_flag);

		AAadjust_u = pos[1]  ;	
		AAadjust_v = pos[2]=0;
	}
	msg("调整uv轴完毕！调整次数:%d",temp_number);
	return temp_number-1;

}
int CHSFoxxDlg::FiveAdjustvalueReadPic(int robotID,int cameraID)
{
	//第三步：五点法确定
	//step1:保存当前帧的图片以及该图像正负向调整的图像..
	const char axisStringQ[15]="X Y Z U V W";
	const char axisString[15]="Z";
	double Ori_ImageCurrentQ[6] ={ 0.0};
	double Ori_ImageCurrent = 0.0;
	double DistanceToOriginal = 0.0;
	Mat Ori_PictureMat;
	Mat Plus_PictureMat1;
	Mat Plus_PictureMat2;
	Mat Minus_PictureMat1;
	Mat Minus_PictureMat2;



	int Ori_flag=ReadPictureMat(&Ori_PictureMat,".\\image\\o.jpg",cameraID);
	if(Ori_flag != TRUE)
	{
		msg("获取原始图片失败");
		return AA_GET_PICTIRE_FAIL;
	}
	//正向移动第一张图，0.003

	DistanceToOriginal = AA_Mirco_disSmall;

	int Plus_flag=ReadPictureMat(&Plus_PictureMat1,".\\image\\p1.jpg",cameraID);
	if(Plus_flag != TRUE)
	{
		msg("获取正向原始图片失败");
		return AA_GET_PICTIRE_FAIL;
	}

	//正向移动第一张图，0.006


	int Plus_flag2=ReadPictureMat(&Plus_PictureMat2,".\\image\\p2.jpg",cameraID);
	if(Plus_flag2 != TRUE)
	{
		msg("获取正向原始图片失败");
		return AA_GET_PICTIRE_FAIL;
	}

	////负向移动第一张图，0.003


	int Minus_flag1=ReadPictureMat(&Minus_PictureMat1,".\\image\\m1.jpg",cameraID);
	if(Minus_flag1 != TRUE)
	{
		msg("获取负向原始图片失败");
		return AA_GET_PICTIRE_FAIL;
	}

	//负向移动第2张图，0.006


	int Minus_flag2=ReadPictureMat(&Minus_PictureMat2,".\\image\\m2.jpg",cameraID);
	if(Minus_flag2 != TRUE)
	{
		msg("获取负向原始图片失败");
		return AA_GET_PICTIRE_FAIL;
	}

	//step2:计算移动的距离.


	Ori_PictureMat = Img_resize(Ori_PictureMat, g_AAParam.scale);
	Minus_PictureMat1 = Img_resize(Minus_PictureMat1, g_AAParam.scale);
	Minus_PictureMat2 = Img_resize(Minus_PictureMat2, g_AAParam.scale);
	Plus_PictureMat1 = Img_resize(Plus_PictureMat1, g_AAParam.scale);
	Plus_PictureMat2 = Img_resize(Plus_PictureMat2, g_AAParam.scale);
	State state = {0};
	state = getState5(Minus_PictureMat2, Minus_PictureMat1, Ori_PictureMat, Plus_PictureMat1, Plus_PictureMat2, g_AAParam.pixel_size/g_AAParam.scale, g_AAParam.scale,g_AAParam.temPlate[cameraID]);
	double adjust_z = state.deltaZ;
	double adjust_u =  0;//state.y_angle;
	double adjust_v = 0;//state.x_angle;

	//step3:.根据计算结果进行移动..
	const char posStr[16] = "Z U V";
	double pos[3];
	pos[0]=adjust_z;
	pos[1]=0;
	pos[2]=0;
	int t = 0;
	//if (fabs(m_AAadjust_u)>0.11)
	if (fabs(adjust_u)>t)
	{
		pos[1]=-adjust_u;

	}
	//if(fabs(m_AAadjust_v)>0.11)
	if (fabs(adjust_v)>t)
	{
		pos[2]=-adjust_v;
	}

	msg("z:%.6lf,u:%.6lf,v:%.6lf",pos[0],pos[1],pos[2]);

	return 0;

}
int CHSFoxxDlg::AdjustXYvalueReadPic(int robotID,int cameraID)
{
	int ret = AA_OK;

	Mat Ori_PictureMat;
	int Ori_flag1= ReadPictureMat(&Ori_PictureMat,".\\image\\o.jpg",cameraID);
	if(Ori_flag1 != TRUE)
	{
		msg("获取原始图片失败");
		return AA_GET_PICTIRE_FAIL;
	}
	Ori_PictureMat = Img_resize(Ori_PictureMat, g_AAParam.scale);
	OFF_set offset;
	offset =  getOffcenter(Ori_PictureMat, g_AAParam.pixel_size/g_AAParam.scale);
	double adjust_x = offset.x_offset;
	double adjust_y = offset.y_offset;
	//START3.......................根据计算结果进行移动............................................
	const char posStr_xy[6] = "X Y";
	double pos_xy[2];
	pos_xy[0]=-adjust_y;
	pos_xy[1]=adjust_x;	

	//调整之前测试是否在可调范围内,是否超出软限位
	//x:[-1,1] y:[-1,1] z;[0.3,0.8] u:[-3,3] v:[-3,3] w:[-3,3]

	msg("x:%.6lf,y:%.6lf",pos_xy[0],pos_xy[1]);

	return ret;

}


void CHSFoxxDlg::OnBnClickedPiHome()
{
	// TODO: 在此添加控件通知处理程序代码
	BOOL ret = 0;
	char *axisString = "X Y Z U V W";
	

	ret = GoInitPosRobot(m_nPiDevID,axisString,AAINDEX2,NULL);


}


void CHSFoxxDlg::OnBnClickedSetinitpos()
{
	// TODO: 在此添加控件通知处理程序代码
	//先获取当前位置,然后把当前位置设置为初始位置
	char axis[] = "X Y Z U V W";
	double curPos[6] = {0};
	CString str;
	int ret = TRUE;
	map<string,CAARobot*>::iterator it;
	for (it= mapRobot.begin();it!= mapRobot.end();it++)
	{
		if (it->second->GetAARobotID() == m_nPiDevID)
		{			
			break;
		}
	}

	if (it != mapRobot.end())
	{
		if (it->second->GetAARobotConnected() == false)
		{
			msg("机器人(id:%d)没连接!",m_nPiDevID);
			return ;
		}

		if (0 !=it->second->GetAAAbsCurrentPosition(m_nPiDevID, axis,curPos))
		{
			PostMessage(WM_USER_TIMER, TIMER_SHOW_MSGBOX, (LPARAM)"获取位置失败..");

		}
		else
		{
			SetInitPosRobot(m_nPiDevID,axis,AAINDEX2,curPos);
			int robotindex = FindRobotIndexByID(m_nPiDevID);
			if (robotindex>=0)
			{
				SaveAA_InitPos_ParamValue(curPos,PI_AXIS_MAXNUM,robotindex,AAINDEX2);
			}
			
		}
		
	}
}


void CHSFoxxDlg::OnBnClickedPiHome1()
{
	// TODO: 在此添加控件通知处理程序代码
	BOOL ret = 0;
	char *axisString = "X Y Z U V W";


	ret = GoInitPosRobot(m_nPiDevID,axisString,AAINDEX1,NULL);

}


void CHSFoxxDlg::OnBnClickedSetinitpos1()
{
	// TODO: 在此添加控件通知处理程序代码
	char axis[] = "X Y Z U V W";
	double curPos[6] = {0};
	CString str;
	int ret = TRUE;
	map<string,CAARobot*>::iterator it;
	for (it= mapRobot.begin();it!= mapRobot.end();it++)
	{
		if (it->second->GetAARobotID() == m_nPiDevID)
		{			
			break;
		}
	}

	if (it != mapRobot.end())
	{
		if (it->second->GetAARobotConnected() == false)
		{
			msg("机器人(id:%d)没连接!",m_nPiDevID);
			return ;
		}

		if (0 !=it->second->GetAAAbsCurrentPosition(m_nPiDevID, axis,curPos))
		{
			PostMessage(WM_USER_TIMER, TIMER_SHOW_MSGBOX, (LPARAM)"获取位置失败..");

		}
		else
		{
			SetInitPosRobot(m_nPiDevID,axis,AAINDEX1,curPos);
			int robotindex = FindRobotIndexByID(m_nPiDevID);
			if (robotindex>0)
			{
				SaveAA_InitPos_ParamValue(curPos,PI_AXIS_MAXNUM,robotindex,AAINDEX1);
			}
		}

	}
}


void CHSFoxxDlg::OnClose()
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	CloseAAVision();
	CDialog::OnClose();
}


void CHSFoxxDlg::OnBnClickedButton1()
{
	// TODO: 在此添加控件通知处理程序代码
	CAAVision *vision = NULL;
	map<string,CAAVision*>::iterator it;
	//在map中找当前视觉id
	for (it = mapVision.begin();it != mapVision.end();it++)
	{
		if (it->second->m_iCurIDA == m_nDevID)
		{
			vision = it->second;
			break;
		}
	}
	if (vision == NULL )
	{
		msg("当前视觉不存在,视觉id:%d",m_nDevID);
		return ;
	}

	int vaule = vision->GetSensorRegValue(AA_EXP);

	msg("Exp:%d ",vaule);

	vaule = vision->SetSensorRegValue(AA_EXP,0x0020);

	msg("set exp:%d ",vaule);


}

int CHSFoxxDlg::SetSensorRegValue(int visionID,int type,USHORT value)
{
	int result = 0;
	CAAVision *vision = NULL;
	vision = FindVisionByID(visionID);
	if (NULL == vision)
	{
		return -1;
	}
	result = vision->SetSensorRegValue(type,value);//0x00C6);

	return result;
}
int CHSFoxxDlg::GetSensorRegValue(int visionID,int type)
{
	int result = 0;
	CAAVision *vision = NULL;
	vision = FindVisionByID(visionID);
	if (NULL == vision)
	{
		return -1;
	}
	result = vision->GetSensorRegValue(type);//0x00C6);

	return result;
}
void CHSFoxxDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	char buff[128] = {0};
	int value = 0;
	int result = 0;
	CAAVision *vision = NULL;
	if (pScrollBar->GetDlgCtrlID() == IDC_GRAIN)
	{
		value = m_grain.GetPos();
		//sprintf_s(buff, "增益: %d%%", value);
		//GetDlgItem(IDC_SGRAIN)->SetWindowText(buff);	
		
		msg("set GRAIN:%x ",value);
		result = SetSensorRegValue(m_nDevID,AA_GRAIN,value);//0x00C6);
		value = GetSensorRegValue(m_nDevID,AA_GRAIN);
		msg("get GRAIN:%x ",value);
	}
	else if(pScrollBar->GetDlgCtrlID() == IDC_EXP)
	{
		value = m_exp.GetPos();

		msg("set Exp:%x",value);
		result = SetSensorRegValue(m_nDevID,AA_EXP,value);//0x00C6);
		value = GetSensorRegValue(m_nDevID,AA_EXP);
		msg("get Exp:%x ",value);
	}
	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}
