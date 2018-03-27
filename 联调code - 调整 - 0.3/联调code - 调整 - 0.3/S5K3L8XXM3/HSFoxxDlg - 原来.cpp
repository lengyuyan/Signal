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

#include <opencv2\opencv.hpp>
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
void msg(LPCSTR lpszFmt,...)
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


typedef struct {
	double x_angle;
	double y_angle;
	double deltaZ;
}State; // get the current state of the initial image including u, v and z
typedef struct {
	double x_offset;
	double y_offset;
}OFF_set;
State getState(Mat img_ori, Mat img_p, Mat img_m, double deltaZ_p, double deltaZ_m, double pixel_size, double scale,int * flag);
State getState5(Mat img_1, Mat img_2, Mat img_3, Mat img_4, Mat img_5, double pixel_size, double scale);
OFF_set getOffcenter(Mat img, double pixel_size);
Mat Img_resize(Mat I, double scale);
string ConvertToString(double value);



BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CHSFoxxDlg dialog

CHSFoxxDlg::CHSFoxxDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CHSFoxxDlg::IDD, pParent)
	, m_sInfo(_T(""))
	, m_fAvdd(2.8f)
	, m_fDovdd(1.8f)
	, m_fDvdd(1.2f)
	, m_fAfvcc(1.2f)
	, m_fVpp(0.0f)
	, m_fMclk(12.0f)
	, m_FocusPos(0)
	, m_ComInitResult(-1)
{
	
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	for (int i=0;i<AAROBOTPROCESSNUM;i++)//机器人处理结果
	{	
		AARobotProcessEvent[i] = CreateEvent(NULL, false, false, NULL);	//不是手动触发,初始状态为false
		
		AARobotWakeResult[i] = CreateEvent(NULL, false, false, NULL);	//不是手动触发,初始状态为false
		AARobotProcessResult[i] = -1;//出事结果是-1;
	}
	

	init_devnetwork();
	m_dlgExit = false;
	mapRobot.clear();
	CFG_INSTANCE;//读取配文件

	ReadAA_Globle_ParamValue();
	m_nDevID = -1;
	m_nPiDevID = -1;//当前机器人id

	m_iMoveType = EMOVE_SHORT_STEP;

}

CHSFoxxDlg::~CHSFoxxDlg()
{
	m_dlgExit = true;
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

	//关闭相机
	if(m_RunMode != RUNMODE_STOP)
	{
		bCloseCamera();
	}

	if ( m_nDevID >=0  && DT_ERROR_OK == IsDevConnect(m_nDevID))
	{
		CloseDevice(m_nDevID);
	}

	CConfig::ReleaseInstance();

	DeleteCriticalSection(&m_csLock);

	uninit_devnetwork();

	for (int i = 0;i< AAROBOTPROCESSNUM;i++)//退出线程
	{
		SetEvent(AARobotProcessEvent[i]);
	}
	for (int i = 0;i< AAROBOTPROCESSNUM;i++)//等待线程退出,关闭线程句柄
	{
		WaitForSingleObject(AARobotProcesshandle[i], INFINITE);
		CloseHandle(AARobotProcesshandle[i]);
	}
	for (int i = 0;i< AAROBOTPROCESSNUM;i++)//是否需要
	{
		CloseHandle(AARobotProcessEvent[i]);
		CloseHandle(AARobotWakeResult[i]);
	}

}

void CHSFoxxDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_show);
	DDX_Control(pDX, IDC_VIDEO, m_wndVideo);
	DDX_Text(pDX, IDC_EDITAVDD, m_fAvdd);
	DDX_Text(pDX, IDC_EDITDOVDD, m_fDovdd);
	DDX_Text(pDX, IDC_EDITDVDD, m_fDvdd);
	DDX_Text(pDX, IDC_EDITAFVCC, m_fAfvcc);
	DDX_Text(pDX, IDC_EDITVPP, m_fVpp);
	DDX_Text(pDX, IDC_EDITMCLK, m_fMclk);
	DDX_Control(pDX, IDC_KITLIST, m_KitListCtrl);
	DDX_Text(pDX, IDC_AF_POS, m_FocusPos);
	DDX_Control(pDX, IDC_PITLIST, m_PiList);

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
	ON_BN_CLICKED(IDC_INITIALDEVICE, &CHSFoxxDlg::OnClickedInitialDevice)
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
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CHSFoxxDlg message handlers

BOOL CHSFoxxDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

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
//////////////////////////////////////////////////////
//20120615
	////////////////////////////////////////////
	m_pSensorName = new char [256];   //(char*) malloc(256*sizeof(char));
	m_pSensorPara = new USHORT [8192*4];   //(USHORT*) malloc(8192*sizeof(USHORT));
	m_pSensorSleepPara = new USHORT [2048];   //(USHORT*) malloc(1024*2);

	//20130823 added..
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
	m_nDevID = -1;

	//20141127 added by leedo
	CheckDiskInfo();
	//20141119 added...
	//enumrate all the connected device to pc...
	EnumerateDothinkeyDevice();

	InitializeCriticalSection(&m_csLock);
	SetIniFileNameInExePath("hisimple.ini");
	CString sfilename = ReadIniString("Last Load File", "FileName", "");
	ClearIniFileName();

	bLoadLibIniFile(sfilename);

	// 加载机器人相关内容
	for (int i=0;i<PI_AXIS_MAXNUM;i++)
	{
		m_edtStep[i].SetWindowText("0.01");
	}
	OnBnClickedEnumnetdevice();



	AARobotProcesshandle[0] = (HANDLE)_beginthreadex( NULL, 0, (unsigned int (__stdcall *)(void *))AARobotProcess1, (LPVOID)this, 0, 0);	
	AARobotProcesshandle[1] = (HANDLE)_beginthreadex( NULL, 0, (unsigned int (__stdcall *)(void *))AARobotProcess2, (LPVOID)this, 0, 0);	
	AARobotProcesshandle[0] = (HANDLE)_beginthreadex( NULL, 0, (unsigned int (__stdcall *)(void *))AARobotProcess3, (LPVOID)this, 0, 0);	
	AARobotProcesshandle[1] = (HANDLE)_beginthreadex( NULL, 0, (unsigned int (__stdcall *)(void *))AARobotProcess4, (LPVOID)this, 0, 0);	


	regist_work_notify(WorkNotify, this);

	test();

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

void CHSFoxxDlg::OnPlay() 
{
	// TODO: Add your control notification handler code here
	if(m_RunMode == RUNMODE_STOP)
	{
		m_roi_state = 0;
		if(bOpenCamera())
		{
			SetDlgItemText(IDC_PLAY, "Stop");
		}
		else
		{
			AfxMessageBox("Open Camera Fail!");
		}
	}
	else
	{
		bCloseCamera();
		SetDlgItemText(IDC_PLAY, "Play");
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
	char* pDeviceName[8] = {NULL};

	CAAVision::EnumerateAAVision(pDeviceName, 8, &DevNum);
	//EnumerateDevice(pDeviceName, 8, &DevNum);

	for(i = 0; i < 8; i++)
	{
		m_DevName[i].Empty();
		if(pDeviceName[i])
		{
			m_DevName[i].Format("%s", pDeviceName[i]);
			GlobalFree(pDeviceName[i]);
			pDeviceName[i] = NULL;
		}
	}
	//add to the device list....
	//20141202 changed...
	m_KitListCtrl.ResetContent();
	if(DevNum)
	{
		for(i = 0; i < DevNum; i++)
		{
			stmp.Format("Device #%d: %s", i, m_DevName[i]);
			m_KitListCtrl.AddString(stmp);	
		}
		m_KitListCtrl.SetCurSel(0);
	}
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

BOOL CHSFoxxDlg::bOpenCamera()
{
	//first kill the data buffer...
	UpdateData(TRUE);
	KillDataBuffer();
	//create the usb device link
	//if you need dtos test.should add like this...

	//打开已经正常连接的设备
	if(!bOpenDevice())
	{
		return FALSE;
	}

		//if you need os test. please enable this.
#if 0
		if(0 == DTOS_Test())
		{
			return FALSE;
		}
#endif

	//set io pin (reset/pwdn,scl, sda, mclk to low)
	SensorEnable(0, 1, m_nDevID);
	//关闭IO上拉电阻
	SetSoftPinPullUp(IO_NOPULL, m_nDevID);
	//20151116 added to close the mclk...
	SetSensorClock(FALSE,(USHORT)(m_fMclk*10), m_nDevID); 

	/*first set sensor working condition....*/
	{
		//first set pin definition...
		{
			BYTE  pinDef[40] = {0};
			//mipi....
			//if(m_PortSel.GetCurSel() == 0)
			if(CurrentSensor.port == PORT_MIPI || CurrentSensor.port == PORT_HISPI)
			{
				pinDef[0] = 20;
				pinDef[1] = 0;
				pinDef[2] = 2;
				pinDef[3] = 1;
				pinDef[4] = 3;
				pinDef[5] = 4;
				pinDef[6] = 5;
				pinDef[7] = 6;
				pinDef[8] = 7;
				pinDef[9] = 8;
				pinDef[10] = 9;
				pinDef[11] = 20;
				pinDef[12] = 10;
				pinDef[13] = 11;
				pinDef[14] = 12;
				pinDef[15] = 20;
				pinDef[16] = 20;
				pinDef[17] = 13;
				pinDef[18] = 15;
				pinDef[19] = 14;
				pinDef[20] = 19;
				pinDef[21] = 18;
				pinDef[22] = 20;
				pinDef[23] = 16;
				pinDef[24] = 20;
				pinDef[25] = 20;
				
			}
			else  //standard parallel..
			{
				//20140317 closed .这个是标准的。。。

				pinDef[0]=16;
				pinDef[1]=0;
				pinDef[2]=2;
				pinDef[3]=1;
				pinDef[4]=3;
				pinDef[5]=4;
				pinDef[6]=5;
				pinDef[7]=6;
				pinDef[8]=7;
				pinDef[9]=8;
				pinDef[10]=9;
				pinDef[11]=20;
				pinDef[12]=10;
				pinDef[13]=11;
				pinDef[14]=12;
				pinDef[15]=20;
				pinDef[16]=20;
				pinDef[17]=20;
				pinDef[18]=20;
				pinDef[19]=20;
				pinDef[20]=13;
				pinDef[21]=20;
				pinDef[22]=14;
				pinDef[23]=15;
				pinDef[24]=18;
				pinDef[25]=19;
			}
			//配置柔性接口
			SetSoftPin(pinDef,m_nDevID);
		}

		//使能柔性接口
		EnableSoftPin(TRUE,m_nDevID);
		EnableGpio(TRUE,m_nDevID);
		//set voltage and mclk.....

		////20160103 changed the power on code... to resolve the ov sensor(ov5670)
		//设置电压，电流
		SENSOR_POWER Power[10] = {POWER_AVDD, POWER_DOVDD, POWER_DVDD, POWER_AFVCC, POWER_VPP};
		int Volt[10] = {0};
		int Current[10] = {300, 300, 300, 300, 300};//300mA
		BOOL OnOff[10] = {TRUE, TRUE, TRUE, TRUE, TRUE};
		CURRENT_RANGE range[5] = {CURRENT_RANGE_MA, CURRENT_RANGE_MA, CURRENT_RANGE_MA, CURRENT_RANGE_MA, CURRENT_RANGE_MA};

		//设置电压开关 switch to on...
		if (PmuSetOnOff(Power, OnOff, 5, m_nDevID) != DT_ERROR_OK)
		{
			CloseDevice(m_nDevID);
			AfxMessageBox("Close Power Failed!");
			return FALSE;
		}

		// 		//设置5路电压值0, close the power firstly, voltage set to zero....
		if (PmuSetVoltage(Power, Volt,5, m_nDevID) != DT_ERROR_OK)
		{
			CloseDevice(m_nDevID);
			AfxMessageBox("Set Voltage Failed!");
			return FALSE;
		}
		//wait for the power is all to zero....
		Sleep(50);

		if (SetSensorClock(TRUE,(USHORT)(m_fMclk*10), m_nDevID) != DT_ERROR_OK)
		{
			CloseDevice(m_nDevID);
			AfxMessageBox("Set Mclk Failed!");
			return FALSE;
		}
		Sleep(1);
			
		//20160103 power on the sensor...////		//first power the avdd.
		// 1. power the avdd. 
		Volt[POWER_AVDD] = (int)(m_fAvdd * 1000); // 2.8V
		PmuSetVoltage(Power, Volt,5, m_nDevID);
		Sleep(1);

		// 2, power on the  dvdd 
		Volt[POWER_DVDD] = (int)(m_fDvdd * 1000);// 1.2V
		PmuSetVoltage(Power, Volt, 5, m_nDevID);
		Sleep(2);

		// 3, power the dovdd...
		Volt[POWER_DOVDD] = (int)(m_fDovdd * 1000); // 1.8V
		PmuSetVoltage(Power, Volt, 5, m_nDevID);
		Sleep(2);

		//4. power the afvcc and vpp...
		Volt[POWER_AFVCC] = (int)(m_fAfvcc * 1000); // 2.8V
		Volt[POWER_VPP] = (int)(m_fVpp * 1000); 
		PmuSetVoltage(Power, Volt, 5, m_nDevID);

		//should wait for 50ms to be ready...
		Sleep(50);

		// 设置量程
		PmuSetCurrentRange(Power,range,5,m_nDevID);
		//设置电流
		PmuSetOcpCurrentLimit(Power,Current,5,m_nDevID);


		//开启IO上拉电阻
		SetSoftPinPullUp(IO_PULLUP, m_nDevID);
	}
	Sleep(10);
	//i2C init....

	//设置SENSOR I2C的速率为400Kbps,允许从设备为Streching mode（从设备端可以拉低scl和sda电平来表示busy）
	SetSensorI2cRate(I2C_400K, m_nDevID);
	SetSensorI2cRapid(1, m_nDevID);
	SetI2CInterval(0, m_nDevID); //I2C byte to byte delay
	//check sensor is on line or not ,if on line,init sensor to work....
	{
		SensorEnable(CurrentSensor.pin ^ 0x02, 1, m_nDevID); //reset
		Sleep(20);
		SensorEnable(CurrentSensor.pin, 1, m_nDevID);
		Sleep(50);

		//check sensoris on line...
/*
		if(SensorIsMe(&CurrentSensor, CHANNEL_A, 0,m_nDevID) != DT_ERROR_OK)
		{
			AfxMessageBox("Sensor is not ok");
			return FALSE;			
		}
*/
		//init sensor....
		if(InitSensor(CurrentSensor.SlaveID,
			CurrentSensor.ParaList,
			CurrentSensor.ParaListSize,
			CurrentSensor.mode,m_nDevID) != DT_ERROR_OK)
			{
				AfxMessageBox("Init Sensor Failed!");
				return FALSE;			
			}
	}
	m_isTV = FALSE;
	USHORT TVBoard_Flag = 0;

/*	ReadSensorReg(0xba, 0x80, &TVBoard_Flag, I2CMODE_MICRON, m_nDevID);
	if(TVBoard_Flag == 0x5150)
	{
		m_isTV = 1;
	}*/	
	//set image property...
#if 0
	//for external eeprom write and read check....
	BYTE myData[8] = {0, 2, 4, 6, 8, 5, 6, 7};
	BYTE RetData[8] = {0};
	UCHAR SlaveID = 0xa2;
	int nRet = WriteSensorI2c(SlaveID, 0x00, 2, myData, 8);
	if(DT_ERROR_OK == nRet)
	{
		//wait write bytes ended....
		int i;
		for(i = 0; i < 100; i++)
		{
			if(DT_ERROR_OK == WriteSensorI2c(SlaveID, 0x00, 0, 0, 0))
			{
				break;
			}
			//Sleep(1);
		}
// 		CString stmp;
// 		stmp.Format("i = %d", i);
// 		AfxMessageBox(stmp);
		if(i >= 100)
		{
			AfxMessageBox("i2c busy!");
			return FALSE;
		}
		//end wait....

		nRet = ReadSensorI2c(SlaveID, 0x00, 2, RetData, 8, 1);
		if(nRet == DT_ERROR_OK)
		{
			for(i = 0; i < 8; i++)
			{
				if(RetData[i] != myData[i])
				{
					AfxMessageBox("check error!");
					return FALSE;
				}
			}
			if(i == 8)
			{
				AfxMessageBox("check ok!");
			}
		}
		else
		{
			AfxMessageBox("read error!");
			return FALSE;
		}
	}
	else
	{
		AfxMessageBox("write error!");
		return FALSE;
	}
#endif

	//end check....
	if(CurrentSensor.type == D_YUV || CurrentSensor.type == D_YUV_SPI || CurrentSensor.type == D_YUV_MTK_S)
		SetYUV422Format(CurrentSensor.outformat, m_nDevID);
	else
		SetRawFormat(CurrentSensor.outformat, m_nDevID);


	m_PreviewWidth = CurrentSensor.width;
	m_PreviewHeight = CurrentSensor.height;//& 0xfffe;

	USHORT roi_x0 = CurrentSensor.width >> 2;
	USHORT roi_y0 = CurrentSensor.height >> 2;
	USHORT roi_hb = 0;
	USHORT roi_vb = 0;
	USHORT roi_hnum = 1;
	USHORT roi_vnum = 1;

	USHORT fifo_div = 2;	
	roi_x0 = 0;
	roi_y0 = 0;
	roi_hb = 0;
	roi_vb = 0;
	roi_hnum = 1;
	roi_vnum = 1;
	//初始化设备
	InitRoi(0, 0, CurrentSensor.width, m_isTV ? CurrentSensor.height >> 1 :CurrentSensor.height , 0, 0, 1, 1, CurrentSensor.type, TRUE, m_nDevID);	//以像素为单位，全分辨率显示关闭ROI使能
	SetSensorPort(CurrentSensor.port, CurrentSensor.width, CurrentSensor.height, m_nDevID);


	//only useful for pe300/pe810/pe910 series....

	//调整MIPI RX CLOCK相位
	SetMipiClkPhase(0,m_nDevID);
		
	Sleep(10);
	CalculateGrabSize(&m_GrabSize, m_nDevID);

	
	//open video....
	OpenVideo(m_GrabSize,m_nDevID);
#ifndef _ONLY_GRAB_IMAGE_
	int a = InitDisplay(m_wndVideo.GetSafeHwnd(),CurrentSensor.width,CurrentSensor.height,CurrentSensor.type, CHANNEL_A, NULL, m_nDevID);
	msg("initdisplay 返回值:%d",a);
	a = InitIsp(CurrentSensor.width, CurrentSensor.height, CurrentSensor.type, CHANNEL_A , m_nDevID);
	msg("InitIsp 返回值:%d",a);
	SetGamma(100, m_nDevID);  //Gamma  :default 100 is no gamma change....
	SetContrast(100), m_nDevID; //Contrast :default 100 is no contrast change...
	SetSaturation(128, m_nDevID);//Saturation: default 128 is no saturation change...
	SetDigitalGain(1.0, 1.0, 1.0, m_nDevID); //AWB digital gian R G B
#endif
	SetTimer(1,500,NULL);

	//malloc memory and create the thread....
	{
		m_RunMode = RUNMODE_PLAY;
		UINT nSize = CurrentSensor.width * CurrentSensor.height * 3 + 1024;
		
		m_pTripleBuffer = (LPBYTE)malloc(nSize);
		m_pCameraBuffer = (LPBYTE)malloc(nSize);
		m_pDisplayBuffer = (LPBYTE)malloc(nSize);

		if (m_pTripleBuffer  == NULL  || 
			m_pCameraBuffer  == NULL  || 
			m_pDisplayBuffer == NULL 
			)
		{
			AfxMessageBox("Memory error!");
			return FALSE;
		}
		memset(m_pTripleBuffer, 0, nSize);
		memset(m_pCameraBuffer, 0, nSize);
		memset(m_pDisplayBuffer, 0, nSize);
	}

	//thread....
	m_hCameraThreadEventDead = CreateEvent(NULL, TRUE, FALSE, NULL);
	m_hDisplayThreadEventDead = CreateEvent(NULL, TRUE, FALSE, NULL);
	_beginthread( Camera_Thread, 0, (void*)this);
	_beginthread( Display_Thread, 0, (void*)this); 
	return TRUE;
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
				m_show.AddString(stmp);
				sOSInfo += stmp;

				stmp.Format("%02d, Vol=%.3f (v)\n", i, VoltageH[i]/1000000.0f);	
				m_show.AddString(stmp);
				sOSInfo += stmp;
				j++;
			}
		}
		
		if(j)
		{
			m_show.AddString("OS test: fail");
		}
		else
		{
			m_show.AddString("OS test: PASS");
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
	m_show.ResetContent();

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
	if(m_pSensorName)
		delete [] m_pSensorName;  //free (m_pSensorName);
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

BOOL CHSFoxxDlg::GetPictureMat(Mat * PictureMat,int caramID)
{
	//wait the thread to end
	BYTE mode = m_RunMode;
	caramID = m_nDevID ;
	if(m_RunMode == RUNMODE_STOP)
	{
		AfxMessageBox("Camera is closed!Please Open it.");
		return -1;
	}
	else //stop the thread and stop capture image and display...
	{
		m_RunMode = RUNMODE_STOP;
	}
	//wait the thread to end
	if(mode == RUNMODE_PLAY)
	{
		WaitForSingleObject(m_hCameraThreadEventDead, INFINITE);
		WaitForSingleObject(m_hDisplayThreadEventDead, INFINITE);
		if(m_hCameraThreadEventDead)
		{
			CloseHandle(m_hCameraThreadEventDead);
			m_hCameraThreadEventDead = NULL;
		}
		if(m_hDisplayThreadEventDead)
		{
			CloseHandle(m_hDisplayThreadEventDead);
			m_hDisplayThreadEventDead = NULL;
		}

	}
	USHORT width  = m_PreviewWidth;
	USHORT height = m_PreviewHeight;
	BYTE   type   =CurrentSensor.type;
	LPBYTE BmpBuffer = NULL;
	BmpBuffer = (LPBYTE)malloc(width*height*4);
	if((BmpBuffer == NULL) )
	{
		AfxMessageBox("Memory allocate error!");
		return -1;
	}
	//allocate the bmp buffer.
	UINT nSize = width*height*3 + 1024*1024;
	LPBYTE CameraBuffer = NULL;
	CameraBuffer = (LPBYTE)malloc(nSize);
	if((CameraBuffer == NULL) )
	{
		AfxMessageBox("Memory allocate error!");
		GetDlgItem(IDC_PLAY)->EnableWindow(TRUE);
		GetDlgItem(IDC_SAVEPICTURE)->EnableWindow(TRUE);
		return -1;
	}
	memset(CameraBuffer, 0, nSize);
	ULONG RetSize = 0;
	UINT i;
	FrameInfo Info;
	int bRet = 0;
	for( i=0; i<10; i++)
	{
		//20130306 changed...
		if(m_isTV)
		{
			bRet = GrabFrame(CameraBuffer, m_GrabSize,&RetSize,&Info,caramID);
			if(bRet == 1)
			{
				bRet = GrabFrame(CameraBuffer+m_GrabSize, m_GrabSize,&RetSize,&Info,caramID);
				if(bRet == 1)
				{
					break;
				}
			}
		}
		else
		if (GrabFrame(CameraBuffer,m_GrabSize, &RetSize,&Info, caramID))
		{
			break;
		}
	}
	
	if(i == 10)
	{
		AfxMessageBox("failed to save!");
	}
	else
	{
		height = m_PreviewHeight;	
		ImageProcess(CameraBuffer,BmpBuffer,width,height,&Info,caramID);	
		Mat img(height,width,CV_8UC3,BmpBuffer);
		*PictureMat=img.clone();
	}
	 m_RunMode=mode;	
	 if(mode==RUNMODE_PLAY)
	{
		m_hCameraThreadEventDead = CreateEvent(NULL, TRUE, FALSE, NULL);
		_beginthread( Camera_Thread, 0, (void*)this);
		m_hDisplayThreadEventDead = CreateEvent(NULL, TRUE, FALSE, NULL);
		_beginthread( Display_Thread, 0, (void*)this); 

	}
	if (CameraBuffer)
	{
		delete(CameraBuffer);
		CameraBuffer = NULL;
	}
	if (BmpBuffer)
	{
		delete(BmpBuffer);
		BmpBuffer = NULL;
	}

	GetDlgItem(IDC_PLAY)->EnableWindow(TRUE);
	GetDlgItem(IDC_SAVEPICTURE)->EnableWindow(TRUE);
	GetDlgItem(IDC_SAVEPICTURE)->SetFocus();
	return 0;
}


//获取当前帧文件，并保存到其父目录的父目录下的image文件夹
BOOL CHSFoxxDlg::SavePicture(CString sfilename)
{
	
	//wait the thread to end
	BYTE mode = m_RunMode;

	if(m_RunMode == RUNMODE_STOP)
	{
		//AfxMessageBox("Camera is closed!\nPlease Open it.");
		AfxMessageBox("Camera is closed!Please Open it.");
		//SetMyTimer(200, 100);
		return -1;
	}
	else //stop the thread and stop capture image and display...
	{
		m_RunMode = RUNMODE_STOP;
	}

	//GetDlgItem(IDC_PLAY)->EnableWindow(FALSE);
	//GetDlgItem(IDC_SAVEPICTURE)->EnableWindow(FALSE);

	//wait the thread to end
	if(mode == RUNMODE_PLAY)
	{
		WaitForSingleObject(m_hCameraThreadEventDead, INFINITE);
		WaitForSingleObject(m_hDisplayThreadEventDead, INFINITE);
		if(m_hCameraThreadEventDead)
		{
			CloseHandle(m_hCameraThreadEventDead);
			m_hCameraThreadEventDead = NULL;
		}
		if(m_hDisplayThreadEventDead)
		{
			CloseHandle(m_hDisplayThreadEventDead);
			m_hDisplayThreadEventDead = NULL;
		}

	}

	USHORT width  = m_PreviewWidth;
	USHORT height = m_PreviewHeight;
	BYTE   type   =CurrentSensor.type;

	LPBYTE BmpBuffer = NULL;
	BmpBuffer = (LPBYTE)malloc(width*height*4);
	if((BmpBuffer == NULL) )
	{
		AfxMessageBox("Memory allocate error!");
		return -1;
	}


	//allocate the bmp buffer.
	UINT nSize = width*height*3 + 1024*1024;
	LPBYTE CameraBuffer = NULL;
	CameraBuffer = (LPBYTE)malloc(nSize);
	if((CameraBuffer == NULL) )
	{
		AfxMessageBox("Memory allocate error!");
		return -1;
	}
	memset(CameraBuffer, 0, nSize);
	ULONG RetSize = 0;
	UINT i;
	FrameInfo Info;
	int bRet = 0;
	for( i=0; i<10; i++)
	{
		//20130306 changed...
		if(m_isTV)
		{
			bRet = GrabFrame(CameraBuffer, m_GrabSize,&RetSize,&Info,m_nDevID);
			if(bRet == 1)
			{
				bRet = GrabFrame(CameraBuffer+m_GrabSize, m_GrabSize,&RetSize,&Info,m_nDevID);
				if(bRet == 1)
				{
					break;
				}
			}
		}
		else
		if (GrabFrame(CameraBuffer,m_GrabSize, &RetSize,&Info, m_nDevID))
		{
			break;
		}
	}
	
	if(i == 10)
	{
		AfxMessageBox("failed to save!");
	}
	else
	{
		height = m_PreviewHeight;
		
		ImageProcess(CameraBuffer,BmpBuffer,width,height,&Info,m_nDevID);
		/*double time = static_cast<double>(getTickCount());
		time = ((double)getTickCount() - time) / getTickFrequency();
        CString m_TempName;
        m_TempName.Format(_T("%lf"),time);
	    MessageBox("4321111时间："+m_TempName+"s");*/
		
		
		Mat img(height,width,CV_8UC3,BmpBuffer);
		
		IplImage qImg;
		qImg = IplImage(img); // cv::Mat -> IplImage
		cvSaveImage(sfilename, &qImg);		
	}

	 m_RunMode=mode;	
	// m_UpdateROI = TRUE;
	 if(mode==RUNMODE_PLAY)
	{
		//SetDlgItemText(IDC_PLAY,"&PAUSE");
		m_hCameraThreadEventDead = CreateEvent(NULL, TRUE, FALSE, NULL);
		_beginthread( Camera_Thread, 0, (void*)this);
		m_hDisplayThreadEventDead = CreateEvent(NULL, TRUE, FALSE, NULL);
		_beginthread( Display_Thread, 0, (void*)this); 

	}


	if (CameraBuffer)
	{
		delete(CameraBuffer);
		CameraBuffer = NULL;
	}

	if (BmpBuffer)
	{
		delete(BmpBuffer);
		BmpBuffer = NULL;
	}

//	GetDlgItem(IDC_PLAY)->EnableWindow(TRUE);
//	GetDlgItem(IDC_SAVEPICTURE)->EnableWindow(TRUE);
	GetDlgItem(IDC_SAVEPICTURE)->SetFocus();
	return 0;
}


void CHSFoxxDlg::OnBnClickedSavepicture()
{
	//wait the thread to end
	BYTE mode = m_RunMode;

	if(m_RunMode == RUNMODE_STOP)
	{
		//AfxMessageBox("Camera is closed!\nPlease Open it.");
		AfxMessageBox("Camera is closed!Please Open it.");
		//SetMyTimer(200, 100);
		return;
	}
	else //stop the thread and stop capture image and display...
	{
		m_RunMode = RUNMODE_STOP;
	}

	GetDlgItem(IDC_PLAY)->EnableWindow(FALSE);
	GetDlgItem(IDC_SAVEPICTURE)->EnableWindow(FALSE);

	//wait the thread to end
	if(mode == RUNMODE_PLAY)
	{
		WaitForSingleObject(m_hCameraThreadEventDead, INFINITE);
		WaitForSingleObject(m_hDisplayThreadEventDead, INFINITE);
		if(m_hCameraThreadEventDead)
		{
			CloseHandle(m_hCameraThreadEventDead);
			m_hCameraThreadEventDead = NULL;
		}
		if(m_hDisplayThreadEventDead)
		{
			CloseHandle(m_hDisplayThreadEventDead);
			m_hDisplayThreadEventDead = NULL;
		}

	}

	USHORT width  = m_PreviewWidth;
	USHORT height = m_PreviewHeight;
	BYTE   type   =CurrentSensor.type;

	LPBYTE BmpBuffer = NULL;
	//MallocBuffer(&BmpBuffer, width * height * 4);
	BmpBuffer = (LPBYTE)malloc(width*height*4);
	if((BmpBuffer == NULL) )
	{
		AfxMessageBox("Memory allocate error!");
		GetDlgItem(IDC_PLAY)->EnableWindow(TRUE);
		GetDlgItem(IDC_SAVEPICTURE)->EnableWindow(TRUE);
		return;
	}


	//allocate the bmp buffer.
	UINT nSize = width*height*3 + 1024*1024;
	LPBYTE CameraBuffer = NULL;
	CameraBuffer = (LPBYTE)malloc(nSize);
	if((CameraBuffer == NULL) )
	{
		AfxMessageBox("Memory allocate error!");
		GetDlgItem(IDC_PLAY)->EnableWindow(TRUE);
		GetDlgItem(IDC_SAVEPICTURE)->EnableWindow(TRUE);
		return;
	}
	//MallocBuffer(&CameraBuffer, nSize);
	memset(CameraBuffer, 0, nSize);


	ULONG RetSize = 0;
	UINT i;
	FrameInfo Info;
	int bRet = 0;
	for( i=0; i<10; i++)
	{
		//20130306 changed...
		if(m_isTV)
		{
			bRet = GrabFrame(CameraBuffer, m_GrabSize,&RetSize,&Info,m_nDevID);
			if(bRet == 1)
			{
				bRet = GrabFrame(CameraBuffer+m_GrabSize, m_GrabSize,&RetSize,&Info,m_nDevID);
				if(bRet == 1)
				{
					break;
				}
			}
		}
		else
		//if(bGrabFrame(CameraBuffer, m_GrabSize))
		if (GrabFrame(CameraBuffer,m_GrabSize, &RetSize,&Info, m_nDevID))
		{
			break;
		}
	}

	if(i == 10)
	{
		AfxMessageBox("failed to save!");
	}
	else
	{
		//ImageProcess(CameraBuffer,BmpBuffer,width,height,&Info,m_nDevID);
		//height = m_PreviewHeight;
		//BitUnSwizzling(CameraBuffer, m_GrabSize);
		//USBDataToRGB24(CameraBuffer, BmpBuffer, m_PreviewWidth, m_PreviewHeight, CurrentSensor.type);
		//ImageISP(BmpBuffer, m_PreviewWidth, m_PreviewHeight, CurrentSensor.type);	
		//DTHSDataToRGB24(CameraBuffer, BmpBuffer, width, height, type,0);
		//v1.0.0.1 Sanvy.He

		//restore the orignal state.
	


		//transfer to jpg file and save the file
		height = m_PreviewHeight;
		CTime time = CTime::GetCurrentTime();
		CString sfilename ;  
		sfilename.Format("P%02d%02d%02d%02d%02d",time.GetMonth(),time.GetDay(),time.GetHour(),time.GetMinute(),time.GetSecond());
		CString sfileext;
		CString sfilter="Jpeg File(*.jpg)|*.jpg|Bitmap File(*.bmp)|*.bmp|Raw File(*.raw)|*.raw||";
		CFileDialog dlg(FALSE,NULL,sfilename,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,sfilter);
		if(dlg.DoModal()== IDOK)
		{
			sfilename = dlg.GetPathName();
			//if (dlg.m_ofn.nFileExtension == 0) 
			{ 
				switch(dlg.m_ofn.nFilterIndex) 
				{	  
				case 1: 
					sfileext = ".jpg"; 
					break; 
				case 2: 
					sfileext = ".bmp"; 
					break;
				case 3:
					sfileext = ".raw";
					break;
				default: 
					break; 
				}
				sfilename += sfileext;
			} 

			//if (dlg.m_ofn.nFileExtension == 0) 



			sfileext = sfilename.Right(4);
			sfileext.MakeLower();

			//save raw8 file
			if(sfileext==".raw")
			{
				HFILE rawFile = _lcreat(sfilename, FALSE); 
				if (rawFile >= 0) 
				{
					UINT len;
					UINT imagesize = m_GrabSize;
					//UINT imagesize = m_PreviewWidth*m_PreviewHeight*2;
					len = _lwrite(rawFile, (LPSTR)CameraBuffer,imagesize ); 
					_lclose(rawFile);
				}
			}
			ImageProcess(CameraBuffer,BmpBuffer,width,height,&Info,m_nDevID);

			if(sfileext==".jpg")
			{
				//VertFlipBuf(BmpBuffer,width*3,height); //to correct the vertical flip problem.
				//RGB24 BMP to JPG
				//JpegFile::SaveJpeg(sfilename,BmpBuffer,width,height);
				JpegFile::SaveJpeg(sfilename,BmpBuffer,width,height);
			}
			else if(sfileext==".bmp")
			{
				////AfxMessageBox("bmp file");
				bSaveBmpFile(sfilename,BmpBuffer,width,height);
			}
			//AddString("SaveTo:"+sfilename,-1,RGB(0,0,255));

		}
	}

	m_RunMode=mode;	
	//m_UpdateROI = TRUE;
	if(mode==RUNMODE_PLAY)
	{
		//SetDlgItemText(IDC_PLAY,"&PAUSE");
		m_hCameraThreadEventDead = CreateEvent(NULL, TRUE, FALSE, NULL);
		_beginthread( Camera_Thread, 0, (void*)this);
		m_hDisplayThreadEventDead = CreateEvent(NULL, TRUE, FALSE, NULL);
		_beginthread( Display_Thread, 0, (void*)this); 

	}


	if (CameraBuffer)
	{
		delete(CameraBuffer);
		CameraBuffer = NULL;
	}

	if (BmpBuffer)
	{
		delete(BmpBuffer);
		BmpBuffer = NULL;
	}

	GetDlgItem(IDC_PLAY)->EnableWindow(TRUE);
	GetDlgItem(IDC_SAVEPICTURE)->EnableWindow(TRUE);
	GetDlgItem(IDC_SAVEPICTURE)->SetFocus();

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
	if(m_RunMode != RUNMODE_PLAY)
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

//	CloseVideo();

	m_bTripleBufferDirty = 0;

	if(m_roi_state == 1)
	{
		InitRoi(0, 0, CurrentSensor.width, CurrentSensor.height, 0, 0, 1, 1, CurrentSensor.type,TRUE, m_nDevID);	//全分辨率
#ifndef _ONLY_GRAB_IMAGE_
		InitDisplay(m_wndVideo.GetSafeHwnd(),CurrentSensor.width,CurrentSensor.height,CurrentSensor.type, m_nDevID);
		InitIsp(CurrentSensor.width, CurrentSensor.height, CurrentSensor.type, CHANNEL_A, m_nDevID );
#endif
		m_roi_state = 0;
	}
	else
	{
		InitRoi( (CurrentSensor.width/2-320), (CurrentSensor.height/2 - 240), 640, 480, 0, 0, 1, 1, CurrentSensor.type, TRUE, m_nDevID);	//ROI 640*480
#ifndef _ONLY_GRAB_IMAGE_
		InitDisplay(m_wndVideo.GetSafeHwnd(),640,480,CurrentSensor.type, m_nDevID);
		InitIsp(640, 480, CurrentSensor.type, CHANNEL_A , m_nDevID);
#endif
		m_roi_state = 1;
	}

	CalculateGrabSize(&m_GrabSize, m_nDevID);


	m_RunMode = RUNMODE_PLAY;

	m_hCameraThreadEventDead = CreateEvent(NULL, TRUE, FALSE, NULL);
	m_hDisplayThreadEventDead = CreateEvent(NULL, TRUE, FALSE, NULL);
	_beginthread( Camera_Thread, 0, (void*)this);
	_beginthread( Display_Thread, 0, (void*)this); 

// 	x = GetTickCount()-x;	
// 	CString stmp;
// 	stmp.Format("time = %d", x);
// 	GetDlgItem(IDC_INFO)->SetWindowText(stmp);
	//AfxMessageBox(stmp);
}

void CHSFoxxDlg::OnBnClickedEnum()
{
	// TODO: 在此添加控件通知处理程序代码
	if(m_RunMode == RUNMODE_STOP)
	{
		EnumerateDothinkeyDevice();
	}
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
		int Ori_flag=GetPictureMat(&Ori_PictureMat,cameraID);
		if(Ori_flag<0)
		{
			msg("获取原始图片失败");return AA_GET_PICTIRE_FAIL;
		}
		if(AARobotMoveRelative(robotID,axisString, &distanceToOriginal[temp_number])<0)
		{
			msg("机器正向移动失败");
			temp_number++;
			continue;
		}
		int Plus_flag=GetPictureMat(& Plus_PictureMat,cameraID);
		if(Plus_flag<0)
		{
			msg("获取距离为:%.6f原始图片失败,调整次数:%d",distanceToOriginal[temp_number],temp_number+1);
			return AA_GET_PICTIRE_FAIL;
		}
		double ww=-2*distanceToOriginal[temp_number];  
		if(AARobotMoveRelative(robotID,axisString, &ww)<0)
		{
			msg("获取距离为:%.6f原始图片失败,调整次数:%d",-distanceToOriginal[temp_number],temp_number+1);
			temp_number++;
			continue;
		}
		int Minus_flag=GetPictureMat(& Minus_PictureMat,cameraID);
		if(Minus_flag<0)
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
		state = getState(img_ori,img_p,img_m,distanceToOriginal[temp_number],-distanceToOriginal[temp_number],g_AAParam.pixel_size/g_AAParam.scale,g_AAParam.scale,&OneSide_flag);
		double adjust_z = state.deltaZ; //
	    //step3..根据计算结果进行移动..
		const char posStr[16] = "z u v";
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
			msg("调整z轴时,初始位置误差范围太大,返回!OneSide_flag:%d,",OneSide_flag);
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
			msg("机器AA调整运动相对位置:%.6f失败,当前位置z:%.6f,调整次数:%d",pos[0],allCurrPos[2],temp_number+1);
			temp_number++;
			continue;
		}
		temp_number++;
		msg("z轴第%d次调整,调整距离为:%.6lf,阈值为:%.6lf,flag:%d",temp_number,pos[0],zadjust_zthr,OneSide_flag);
		
		AAadjust_z = adjust_z;	
	}
	msg("调整Z轴完毕！调整次数:%d",temp_number);
	return 		temp_number-1;
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
		if(Ori_flag<0)
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
		if(Plus_flag<0)
		{
			msg("获取正向原始图片失败,返回！");
			return AA_GET_PICTIRE_FAIL;
		}
		temp_dis =-2*distanceToOriginalUV[temp_number];
		if(AARobotMoveRelative(robotID,axisString, &temp_dis)<0)
		{
			msg("机器负向移动失败,移动的z轴距离为:%.6lf",-distanceToOriginalUV[temp_number]);
			AARobotMoveRelative(robotID,axisString,&distanceToOriginalUV[temp_number]);
			temp_number++;
			continue;
		}
		int Minus_flag=GetPictureMat(& Minus_PictureMat,m_nDevID);
		if(Minus_flag<0)
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
		state = getState(Ori_PictureMat,Plus_PictureMat,Minus_PictureMat,distanceToOriginalUV[temp_number],-distanceToOriginalUV[temp_number],g_AAParam.pixel_size/g_AAParam.scale,g_AAParam.scale,&OneSide_flag);
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
			msg("调整时,初始位置误差范围太大,或者找不到点,返回!OneSide_flag:%d,",OneSide_flag);
			return AA_Z_ADJUST_BIG_FAIL;
		}

		int result=AARobotMoveRelative(robotID,posStr, pos);
		if(result<0)
		{
			msg("uv旋转角度为:%.6lf,%.6lf,超出机器运动范围,继续",pos[1],pos[2]);
			temp_number++;
			continue;
		}
		temp_number++;
		msg("uv轴第%d次调整,调整距离为du:%.6lf,dv:%.6lf,阈值为:%.6lf,flag:%d",temp_number,pos[1],pos[2],adjust_uvthr,OneSide_flag);
		
		AAadjust_u = pos[1];	
		AAadjust_v = pos[2];
	}
	msg("调整uv轴完毕！调整次数:%d",temp_number);
	return temp_number-1;
		
}

int CHSFoxxDlg::FiveAdjustvalue(int robotID,int cameraID)
{
	//第三步：五点法确定
	//step1:保存当前帧的图片以及该图像正负向调整的图像..
	const char axisString[5]="Z ";
	double Ori_ImageCurrent = 0.0;
	double DistanceToOriginal = 0.0;
	Mat Ori_PictureMat;
	Mat Plus_PictureMat1;
	Mat Plus_PictureMat2;
	Mat Minus_PictureMat1;
	Mat Minus_PictureMat2;
	if(AARobotCurrentPosition(robotID,axisString, &Ori_ImageCurrent)<0)
	{
		msg("5点法,获取当前位置失败");
		return AA_GET_CURRENTPT_FAIL;
	}
	msg("当前位置:x:%.6lf,y:%.6lf,z:%.6lf,u:%.6lf,v:%.6lf,z:%.6lf");
	
	int Ori_flag=GetPictureMat(& Ori_PictureMat,cameraID);
	if(Ori_flag<0)
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
	if(Plus_flag<0)
	{
		msg("获取正向原始图片失败");
		return AA_GET_PICTIRE_FAIL;
	}

	//正向移动第一张图，0.006
	
	if(AARobotCurrentPosition(robotID,axisString, &DistanceToOriginal)<0)
	{
		msg("机器正向移动失败,移动距离:%.6lf",2*DistanceToOriginal);
		return AA_GO_RERATIVEPT_FAIL;
	}
	int Plus_flag2=GetPictureMat(& Plus_PictureMat2,cameraID);
	if(Plus_flag2<0)
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
	if(Minus_flag1<0)
	{
		msg("获取负向原始图片失败");
		return AA_GET_PICTIRE_FAIL;
	}

	//负向移动第2张图，0.006
	if(AARobotMoveRelative(robotID,axisString, &DistanceToOriginal)<0)
	{
		msg("机器负向移动失败,移动距离:%.6lf",-2*DistanceToOriginal);
		return AA_GO_RERATIVEPT_FAIL;
	}

	int Minus_flag2=GetPictureMat(& Minus_PictureMat2,cameraID);
	if(Minus_flag2<0)
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
	state = getState5(Minus_PictureMat2, Minus_PictureMat1, Ori_PictureMat, Plus_PictureMat1, Plus_PictureMat2, g_AAParam.pixel_size/g_AAParam.scale, g_AAParam.scale);
	double adjust_z = state.deltaZ;
	double adjust_u =  state.y_angle;
	double adjust_v = state.x_angle;

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
			if(AARobotMoveRelative(m_nPiDevID,axisString, &DistanceToOriginal)<0)
			{
				msg("机器正向移动失败,行:%d",__LINE__);
				DistanceToOriginal=0;
				return AA_GO_RERATIVEPT_FAIL;
			}
			Mat Plus_PictureMat;
			int Plus_flag1=GetPictureMat(& Plus_PictureMat,cameraID);
			if(Plus_flag1<0)
			{
				msg("获取原始图片失败,行:%d",__LINE__);
				return AA_GET_PICTIRE_FAIL;
			}
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
		img_ori = Img_resize(img_ori, g_AAParam.scale);
		img_p1 = Img_resize(img_p1, g_AAParam.scale);
		img_p2 = Img_resize(img_p2, g_AAParam.scale);
		img_m1 = Img_resize(img_m1, g_AAParam.scale);
		img_m2 = Img_resize(img_m2, g_AAParam.scale);
		State state;
		state = getState5(img_m2, img_m1, img_ori, img_p1, img_p2, g_AAParam.pixel_size/g_AAParam.scale, g_AAParam.scale);
		double adjust_z = state.deltaZ;
		double adjust_u = state.y_angle ;
		double adjust_v = state.x_angle;

		//START3.......................根据计算结果进行移动............................................
		const char posStr[16] = "Z U V";
		double pos[3];
		pos[0]=adjust_z;
		pos[1]=0;
		pos[2]=0;
		if (fabs(adjust_u)>t)
		{
			pos[1]=adjust_u;

		}
		if (fabs(adjust_v)>t)
		{
			pos[2]=adjust_v;
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

int CHSFoxxDlg::StartAA(int robotid,int visonid)
{
	
	CString ALLDynamicList="..\\..\\image\\";
	const char axisString1[5]="Z";
	double First_ImageCurrent = 0.0;
	//...............................

	return -1;///同意返回-1;


	//暂时只支持一个机器人
	robotid = m_nPiDevID;
	visonid = m_nDevID ;

	if(AARobotCurrentPosition(robotid,axisString1, &First_ImageCurrent)<0)
	{
		msg("获取当前位置失败");return AA_GET_CURRENTPT_FAIL;
	}
	CString FirstImage;
	FirstImage.Format("%.3lf",First_ImageCurrent);
	SavePicture(ALLDynamicList+FirstImage+"_First.jpg");
	double time = static_cast<double>(getTickCount());


	double distanceToOriginal[10]={0};
	int  first_z_number = AdjustZvalue(robotid,visonid);
	if (first_z_number<0)
	{
		return AA_ADJUST_FAIL;
	}
	//第二步：然后需再调整U/V轴
	int  adjust_uv_number = AdjustUVvalue(robotid,visonid);
	if (adjust_uv_number<0)
	{
		return AA_ADJUST_FAIL;
	}


	return AA_OK;


}

void CHSFoxxDlg::OnBnClickedStartaa()
{
	if (m_nPiDevID<0)
	{
		msg("没有连接机器人,返回!当前机器人id:%d",m_nPiDevID);return;
	}
	// 包括取图，调用算法库进行分析，调用机器人库进行运动，然后再次取图，再次分析
	 CreateDirectory("..\\..\\image",NULL);//创建文件夹
	 CString ALLDynamicList="..\\..\\image\\";
	 const char axisString1[5]="Z";
	 double First_ImageCurrent = 0.0;
	    //...............................
	if(AARobotCurrentPosition(m_nPiDevID,axisString1, &First_ImageCurrent)<0)
	{
		msg("获取当前位置失败");return;
	}
	CString FirstImage;
	FirstImage.Format("%.3lf",First_ImageCurrent);
	SavePicture(ALLDynamicList+FirstImage+"_First.jpg");
	double time = static_cast<double>(getTickCount());	

	//一，调整z

	double distanceToOriginal[10]={0};
	int  first_z_number = AdjustZvalue(m_nPiDevID,m_nDevID);
	if (first_z_number<0)
	{
		return ;
	}
	//第二步：然后需再调整U/V轴
	int  adjust_uv_number = AdjustUVvalue(m_nPiDevID,m_nDevID);
	if (adjust_uv_number<0)
	{
		return ;
	}
	return;

	int  adjust_zuv_number = 0;
	//adjust_zuv_number = FiveAdjustvalue( m_nPiDevID,m_nDevID);
	//************************************************************************************************************
	//第五步：调整x,y
	/*
	Mat Ori_PictureMat1;
	int Ori_flag1=GetPictureMat(& Ori_PictureMat1);
	if(Ori_flag1<0)
	{
		msg("获取原始图片失败");
		return;
	}
	Ori_PictureMat1 = Img_resize(Ori_PictureMat1, g_AAParam.scale);
	OFF_set offset;
	offset =  getOffcenter(Ori_PictureMat1, g_AAParam.pixel_size/g_AAParam.scale);
	double adjust_x = offset.x_offset;
	double adjust_y = offset.y_offset;
	//START3.......................根据计算结果进行移动............................................
	const char posStr_xy[6] = "X Y";
	double pos_xy[2];
	pos_xy[0]=-adjust_y;
	pos_xy[1]=adjust_x;	

	//调整之前测试是否在可调范围内,是否超出软限位
	//x:[-1,1] y:[-1,1] z;[0.3,0.8] u:[-3,3] v:[-3,3] w:[-3,3]


	int result_xy=AARobotMoveRelative(m_nPiDevID,posStr_xy, pos_xy);
	if(result_xy<0)
	{
		//AfxMessageBox("机器AA调整运动失败");
		//MessageBox("超出机器运动范围");
	}*/

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
 	SavePicture(ALLDynamicList+FirstImage+".jpg");//_Last
	return;

}



void CHSFoxxDlg::OnClickedInitialDevice()
{
	// TODO: 在此添加控件通知处理程序代码

	//获取串口值
	CString ComPort;
	int ComPortValue;
	GetDlgItem(IDC_EDITCOMPORT)->GetWindowTextA(ComPort);
	ComPortValue=_tstoi(ComPort);

	//m_ID值更新
	//if(m_ComInitResult<0)
	{
		m_ComInitResult= AARobotTCPIPInitOne(ComPort.GetBuffer(),50000);
		if(m_ComInitResult<0)
		{
			MessageBox("初始化失败");
			return;
		}

	}
	MessageBox("初始化成功");
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


void CHSFoxxDlg::DeleteFolder(CString sPath)
{
	/*CFileFind ff;  
    BOOL bFound;  
    bFound = ff.FindFile(sPath + "*.*");  
    while(bFound)  
    {  
        bFound = ff.FindNextFile();  
        CString sFilePath = ff.GetFilePath();  
          
        if(ff.IsDirectory())  
        {  
            if(!ff.IsDots())  
            {  
                DeleteFolder(sFilePath);  
            }  
        }  
        else  
        {  
            if(ff.IsReadOnly())  
            {  
                SetFileAttributes(sFilePath, FILE_ATTRIBUTE_NORMAL);  
            }  
            DeleteFile(sFilePath);  
        }  
    }  
    ff.Close(); */
}


/*Bool CHSFoxxDlg::GetCurrentPictureMat(void)
{
	return Mat();
}*/





void CHSFoxxDlg::OnEnChangeInitY()
{
	// TODO:  如果该控件是 RICHEDIT 控件，它将不
	// 发送此通知，除非重写 CDialog::OnInitDialog()
	// 函数并调用 CRichEditCtrl().SetEventMask()，
	// 同时将 ENM_CHANGE 标志“或”运算到掩码中。

	// TODO:  在此添加控件通知处理程序代码
}


Mat Img_resize(Mat I, double scale){
	if (I.channels() == 3){
		cvtColor(I, I, CV_RGB2GRAY);
	}
	int height = I.rows * scale;
	int width = I.cols * scale;
	resize(I, I, Size(width, height));
	I.convertTo(I, CV_32F);
	I = I / 255;
	return I;
}
string ConvertToString(double value)
{
	stringstream ss;
	ss << value;
	return ss.str();
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
	/*
	for (int i = 0;i<PI_NUM;i++)
	{
		m_PInetName[i].Empty();
	}*/
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
		OnBnClickedEnumnetdevice();
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
	for(int iAxis = 0; iAxis < PI_MAX_AXIS; iAxis++)
	{	
		//if (0 >= m_nPiDevID)
		//{
		//	break;
		//}
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
	return CDialog::PreTranslateMessage(pMsg);
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
	}
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
		msg("下拉框异常！");
		return;
	}
	string strTemp = str.GetBuffer();
	int id = ConnectPIRobot(strTemp);
	if (id >=0 && id != m_nPiDevID)
	{
		m_nPiDevID = id;
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

int CHSFoxxDlg::FindRobotIDByIp(string ip)
{
	int robotID = -1;
	map<string,CAARobot*>::iterator it;
	it = mapRobot.find(ip);
	if (it != mapRobot.end())
	{
		robotID = it->second->GetAARobotID();
	}
	return robotID;

}

vector<int> WINAPI CHSFoxxDlg::WorkNotify(int evt, AANotifyData data, void* lparam)
{
	CHSFoxxDlg* pDlg = (CHSFoxxDlg*)lparam;
	vector<int>result;

	pDlg->SendMessage(WM_MSG_NOTIFY, (WPARAM)evt, (LPARAM)&data);//静态函数调用成员函数是否需要发送消息的方式

	switch(data.robotid1)
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

	return result;
}
LRESULT CHSFoxxDlg::OnMsgNotify(WPARAM wParam, LPARAM lParam)
{
	int evt = (int)wParam;
	AANotifyData* pData = (AANotifyData*)lParam;
	int ret = 0;
	switch (evt)//事件类型,暂时只有一种
	{
	case 0:
		if (pData->robotid1==1)//机器人对应不同视觉,两个机器人要并行,切要解析机器人id跟ip的绑定关系,视觉
		{
			AARobotProcessResult[0] = -1;
			AARobotProcessResult[1] = -1;
			SetEvent(AARobotProcessEvent[0]);
			SetEvent(AARobotProcessEvent[1]);
		}
		else if (pData->robotid1==3)
		{
			AARobotProcessResult[2] = -1;
			AARobotProcessResult[3] = -1;
			SetEvent(AARobotProcessEvent[2]);
			SetEvent(AARobotProcessEvent[3]);
		}
		
		break;
	default:
		break;
	}
	return ret;
}

int WINAPI CHSFoxxDlg::AARobotProcess1(void* param)//先启动线程
{
	CHSFoxxDlg* pWork = (CHSFoxxDlg*)param;
	while (!pWork->m_dlgExit)
	{
		WaitForSingleObject(pWork->AARobotProcessEvent[0], INFINITE);
		if (pWork->m_dlgExit != false)
		{
			return 0;
		}
		//....处理....每个线程对应的机器人和视觉通道都已知，可以写在配置文件中
		pWork->AARobotProcessResult[0] = pWork->StartAA(1,1);
		SetEvent(pWork->AARobotWakeResult[0]);
		WaitForSingleObject(pWork->AARobotProcessEvent[0], 0);//立即返回,这里只是测试,可要可不要
		Sleep(50);
	}

	return 0;


}
int WINAPI CHSFoxxDlg::AARobotProcess2(void* param)
{
	CHSFoxxDlg* pWork = (CHSFoxxDlg*)param;
	while (!pWork->m_dlgExit)
	{
		WaitForSingleObject(pWork->AARobotProcessEvent[1], INFINITE);
		if (pWork->m_dlgExit != false)
		{
			return 0;
		}
		//....处理....每个线程对应的机器人和视觉通道都已知，可以写在配置文件中
		pWork->AARobotProcessResult[1] = pWork->StartAA(2,1);
		SetEvent(pWork->AARobotWakeResult[1]);
		WaitForSingleObject(pWork->AARobotProcessEvent[1], 0);//立即返回,这里只是测试,可要可不要
		Sleep(50);
	}
	return 0;
}
int WINAPI CHSFoxxDlg::AARobotProcess3(void* param)
{
	CHSFoxxDlg* pWork = (CHSFoxxDlg*)param;
	while (!pWork->m_dlgExit)
	{
		WaitForSingleObject(pWork->AARobotProcessEvent[2], INFINITE);
		if (pWork->m_dlgExit != false)
		{
			return 0;
		}
		//....处理....每个线程对应的机器人和视觉通道都已知，可以写在配置文件中
		pWork->AARobotProcessResult[2] = pWork->StartAA(2,0);
		SetEvent(pWork->AARobotWakeResult[2]);
		WaitForSingleObject(pWork->AARobotProcessEvent[2], 0);//立即返回,这里只是测试,可要可不要
		Sleep(50);
	}
	return 0;
}
int WINAPI CHSFoxxDlg::AARobotProcess4(void* param)
{
	CHSFoxxDlg* pWork = (CHSFoxxDlg*)param;
	while (!pWork->m_dlgExit)
	{
		WaitForSingleObject(pWork->AARobotProcessEvent[3], INFINITE);
		if (!pWork->m_dlgExit != false)
		{
			return 0;
		}
		//....处理....每个线程对应的机器人和视觉通道都已知，可以写在配置文件中
		pWork->AARobotProcessResult[3] = pWork->StartAA(3,0);
		SetEvent(pWork->AARobotWakeResult[3]);
		WaitForSingleObject(pWork->AARobotProcessEvent[3], 0);//立即返回,这里只是测试,可要可不要
		Sleep(50);
	}
	return 0;
}
