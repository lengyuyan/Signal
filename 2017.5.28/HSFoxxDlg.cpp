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
//#include <math.h>
//#include <sstream>
using namespace cv;
using namespace std;


#include "../RobotLib/RobotLib.h"

 
#pragma comment (lib,"dtccm2.lib")

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define WM_MSG	WM_USER+1

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
	, m_fMclk(24.0f)
	, m_FocusPos(0)
	, m_AlignmentNumber(0)
	, m_CurrentImageZData(0)
	, m_AAadjust_z(-100)
	, m_AAadjust_u(-1000)
	, m_AAadjust_v(-100)
	, m_ComInitResult(-1)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_nDevID = -1;

}

CHSFoxxDlg::~CHSFoxxDlg()
{
	if(m_RunMode != RUNMODE_STOP)
	{
		bCloseCamera();
	}

	if ( m_nDevID >=0  && DT_ERROR_OK == IsDevConnect(m_nDevID))
	{
		CloseDevice(m_nDevID);
	}

	DeleteCriticalSection(&m_csLock);
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

	//................数据初始化.............................................
	m_AlignmentNumber=1;            //机器调整的次数
	m_DistanceToOriginal=0.1;       //获取当前图像正负向一定间隔的图像，出事间隔设置为0.1，之后一直缩小
	m_CurrentImageZData=0;         //保存当前位置图像的z轴的值
	m_ID=-1;                       //保存控制器的号

	m_OriginalImage=0;            //原位置图像
	m_PlusImage=0;                //正向位置图像值
	m_MinusImage=0;              //负向位置图像值
	m_deltaZ_p=0;                //原位置图像与正向位置图像值距离
	m_deltaZ_m=0;                //原位置图像与负向位置图像值距离
	
	m_scale = 0.2;
	m_pixel_size = 0.00112 / m_scale;//像素大小
	m_std_error = 0.03;

	m_AAadjust_z=-100; // 保存每次AA需要调整的z轴的值
	m_AAadjust_u=-100; // 保存每次AA需要调整的u轴的值
	m_AAadjust_v=-100; // 保存每次AA需要调整的v轴的值

	m_ComInitResult=-1;

	GetDlgItem(IDC_INIT_X)->SetWindowText("-0.45");
	GetDlgItem(IDC_INIT_Y)->SetWindowText("0.3");
	GetDlgItem(IDC_INIT_Z)->SetWindowText("0.41");
	GetDlgItem(IDC_INIT_U)->SetWindowText("0");
	GetDlgItem(IDC_INIT_V)->SetWindowText("0");
	GetDlgItem(IDC_INIT_W)->SetWindowText("0");

	//........................................................................

	//20141127 added by leedo
	CheckDiskInfo();
	//20141119 added...
	//enumrate all the connected device to pc...
	EnumerateDothinkeyDevice();

	InitializeCriticalSection(&m_csLock);

#if 1
	DWORD tt[4] = {0};
	
	int retx = GetLibVersion(tt);
	if(1 != retx)
	{
		AfxMessageBox("Get lib version error, maybe device is not linkded...!");
	}
#endif

	SetIniFileNameInExePath("hisimple.ini");
	CString sfilename = ReadIniString("Last Load File", "FileName", "");
	ClearIniFileName();

	bLoadLibIniFile(sfilename);


	// 加载机器人相关内容
	//unitTest();


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

	EnumerateDevice(pDeviceName, 8, &DevNum);

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

	//m_GrabSize = 10543604;
	//open video....
	OpenVideo(m_GrabSize,m_nDevID);
#ifndef _ONLY_GRAB_IMAGE_
	InitDisplay(m_wndVideo.GetSafeHwnd(),CurrentSensor.width,CurrentSensor.height,CurrentSensor.type, CHANNEL_A, NULL, m_nDevID);
	InitIsp(CurrentSensor.width, CurrentSensor.height, CurrentSensor.type, CHANNEL_A , m_nDevID);

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

		//以下将测试板置于OS模式
		OS_Config((int)(1.5f*1000*1000), HighLimit, LowLimit, 45, 2*1000, 1*1000, m_nDevID);	
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

BOOL CHSFoxxDlg::GetPictureMat(Mat * PictureMat)
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
		*PictureMat=img.clone();

		/*double time1 = static_cast<double>(getTickCount());
		
		time1 = ((double)getTickCount() - time1) / getTickFrequency();
		CString m_TempName1;
		m_TempName1.Format(_T("%lf"),time1);
		MessageBox("所用时间mat2："+m_TempName1+"s");*/
	/*	namedWindow("clone:",CV_WINDOW_NORMAL);
		imshow("clone:",*PictureMat);
		waitKey();*/

		//Mat * imgP;
		//imgp=&img;
		
		/*IplImage qImg;
		qImg = IplImage(img); // cv::Mat -> IplImage
		cvSaveImage(sfilename, &qImg);*/
    	/*double time1 = static_cast<double>(getTickCount());
		time1= ((double)getTickCount() - time1) / getTickFrequency();
    	CString m_TempName1;
	    m_TempName1.Format(_T("%lf"),time1);
	    MessageBox("所用时间："+m_TempName1+"s");*/
		//bSaveBmpFile(sfilename,BmpBuffer,width,height);
		
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
	//MallocBuffer(&BmpBuffer, width * height * 4);
	BmpBuffer = (LPBYTE)malloc(width*height*4);
	if((BmpBuffer == NULL) )
	{
		AfxMessageBox("Memory allocate error!");
//		GetDlgItem(IDC_PLAY)->EnableWindow(TRUE);
	//	GetDlgItem(IDC_SAVEPICTURE)->EnableWindow(TRUE);
		return -1;
	}


	//allocate the bmp buffer.
	UINT nSize = width*height*3 + 1024*1024;
	LPBYTE CameraBuffer = NULL;
	CameraBuffer = (LPBYTE)malloc(nSize);
	if((CameraBuffer == NULL) )
	{
		AfxMessageBox("Memory allocate error!");
	//	GetDlgItem(IDC_PLAY)->EnableWindow(TRUE);
	//	GetDlgItem(IDC_SAVEPICTURE)->EnableWindow(TRUE);
		return -1;
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
		//Mat * imgP;
		//imgp=&img;
		/*namedWindow("ccc",WINDOW_AUTOSIZE);
		imshow("ccc",img);*/
		
		IplImage qImg;
		qImg = IplImage(img); // cv::Mat -> IplImage
		cvSaveImage(sfilename, &qImg);
		
    	/*double time1 = static_cast<double>(getTickCount());
		time1= ((double)getTickCount() - time1) / getTickFrequency();
    	CString m_TempName1;
	    m_TempName1.Format(_T("%lf"),time1);
	    MessageBox("所用时间："+m_TempName1+"s");*/
		//bSaveBmpFile(sfilename,BmpBuffer,width,height);
		
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


void CHSFoxxDlg::OnBnClickedStartaa()
{
	// TODO: 在这里开始一次AA的调整过程
	// 包括取图，调用算法库进行分析，调用机器人库进行运动，然后再次取图，再次分析
		
	//DeleteFolder("..\\..\\image\\*.*");//清空文件夹
	//RemoveDirectory("..\\..\\image\\");
	 CreateDirectory("..\\..\\image",NULL);//创建文件夹
	 CString ALLDynamicList="..\\..\\image\\";
	 const char axisString1[5]="Z ";
	 double First_ImageCurrent;
	    //...............................
	if(AARobotCurrentPosition(axisString1, &First_ImageCurrent)<0)
	{
		MessageBox("获取当前位置失败");
		return;
	}
	CString FirstImage;
	FirstImage.Format("%.3lf",First_ImageCurrent);
	SavePicture(ALLDynamicList+FirstImage+"_First.bmp");

	double time = static_cast<double>(getTickCount());

	
	
	/*
	double time = static_cast<double>(getTickCount());
	time = ((double)getTickCount() - time) / getTickFrequency();
	CString m_TempName;
	m_TempName.Format(_T("%lf"),time);
	MessageBox("所用时间："+m_TempName+"s");
	*/
	//************************************************************************************************************
	//************************************************************************************************************
    //第一步：首先需要调整Z轴
	/*int Active_number=0;
	CString  m_Active_number;
	m_Active_number.Format("%d",Active_number);
	MessageBox("第一次调整Z轴："+m_Active_number);
	*/
	//double time1 = static_cast<double>(getTickCount());
	
	int temp_file_number=0;
	CString TempName;
	int temp_number=0;
	double distanceToOriginal[10]={0.06,0.05,0.04,0.03,0.03,0.03,0.03,0.03,0.03, 0.03};
	m_DistanceToOriginal=distanceToOriginal[temp_number];
	//while(!(( fabs(m_AAadjust_z)<0.003 && fabs(m_AAadjust_u)<0.1 && fabs(m_AAadjust_v)<0.1)||(temp_number>6)))
	int first_z_number=0;
	while(!(( fabs(m_AAadjust_z)<0.0049)||(temp_number>9)))
	{

	    //START1.....................保存当前帧的图片以及该图像正负向调整的图像........................................
		const char axisString[5]="Z ";
		double Ori_ImageCurrent;
	    //...............................
	    if(AARobotCurrentPosition(axisString, &Ori_ImageCurrent)<0)
		{
			MessageBox("获取当前位置失败");
			return;
		}
		
		/*CString filename_o="..//..//image//m_OriginalImage.bmp";	
	    if(SavePicture(filename_o)<0)
		{
			AfxMessageBox("保存原始图片失败");
			return;
		}*/
		
		
		
		Mat Ori_PictureMat;
		int Ori_flag=GetPictureMat(& Ori_PictureMat);
		if(Ori_flag<0)
		{
			AfxMessageBox("保存原始图片失败");
			return;
		}

		//正向移动一定间隔获取图像
		m_deltaZ_p=m_DistanceToOriginal;
		m_PlusImage=m_CurrentImageZData+m_DistanceToOriginal;
		//移动机器人+m_DistanceToOriginal，并获取图像
		if(AARobotMoveRelative(axisString, &m_DistanceToOriginal)<0)
		{
			AfxMessageBox("机器正向移动失败");
			temp_number++;
			m_DistanceToOriginal=distanceToOriginal[temp_number];
			continue;
		}
	

		Mat Plus_PictureMat;
		int Plus_flag=GetPictureMat(& Plus_PictureMat);
		if(Plus_flag<0)
		{
			AfxMessageBox("保存原始图片失败");
			return;
		}

	/*	namedWindow("5",CV_WINDOW_NORMAL);
		imshow("5",Plus_PictureMat);
		//waitKey();
		
		vector<Mat> channels;
		split(Plus_PictureMat,channels);
		Mat I1=channels.at(0);
		Mat I2=channels.at(1);
		Mat I3=channels.at(2);

		namedWindow("1",CV_WINDOW_NORMAL);
		imshow("1",I1);
		//waitKey();

		namedWindow("2",CV_WINDOW_NORMAL);
		imshow("2",I2);
		//waitKey();

		namedWindow("3",CV_WINDOW_NORMAL);
		imshow("3",I3);
		//waitKey();*/

		//............................
		//负向移动一定间隔获取图像
		m_deltaZ_m=-m_DistanceToOriginal;
		m_MinusImage=m_CurrentImageZData-m_DistanceToOriginal;
		//移动机器人+m_DistanceToOriginal，并获取图像
		double ww=-2*m_DistanceToOriginal;
		if(AARobotMoveRelative(axisString, &ww)<0)
		{
			AfxMessageBox("机器负向移动失败");//若出错返回原来位置
			AARobotMoveRelative(axisString,&m_deltaZ_p);
			temp_number++;
			m_DistanceToOriginal=distanceToOriginal[temp_number];
			continue;
		}
		Mat Minus_PictureMat;
		int Minus_flag=GetPictureMat(& Minus_PictureMat);
		if(Minus_flag<0)
		{
			AfxMessageBox("保存原始图片失败");
			return;
		}
		if(AARobotMoveRelative(axisString, &m_DistanceToOriginal)<0)
		{
			AfxMessageBox("机器返回初始位置失败");
			return;
		}

	    //START2......................计算移动的距离................................................
		//CString str = ……
		//const std::string &OriginalPath = (LPCSTR)filename_o;
		//const std::string &PlusPath = (LPCSTR)filename_p;
		//const std::string &MinusPath = (LPCSTR)filename_m;

		//Mat img_ori = imread(OriginalPath);
	   // Mat img_p = imread(PlusPath);
	   // Mat img_m = imread(MinusPath);

		Mat img_ori =Ori_PictureMat;
		Mat img_p =Plus_PictureMat;
		Mat img_m =Minus_PictureMat;
	    img_ori = Img_resize(img_ori, m_scale);
	    img_p = Img_resize(img_p, m_scale);
	    img_m = Img_resize(img_m, m_scale);
		State state;
		int OneSide_flag;
		state = getState(img_ori,img_p,img_m,m_deltaZ_p,m_deltaZ_m,m_pixel_size,m_scale,&OneSide_flag);

		double adjust_z = state.deltaZ;
		double adjust_u = state.x_angle * -1;
		double adjust_v = state.y_angle * -1;
		first_z_number++;
		//CString adjust_z_u_v;
		//adjust_z_u_v.Format(_T("一z：az:%lf au:%lf av:%lf"),adjust_z,0,0);
		//AfxMessageBox(adjust_z_u_v);

	    //START3.......................根据计算结果进行移动............................................
		const char posStr[16] = "z u v";
		double pos[3];
		if(OneSide_flag>=0)
		{
			pos[0]=adjust_z;
		}
		else if(OneSide_flag==-1)
		{
			//pos[0]=adjust_z/2;
			pos[0]=adjust_z;
			MessageBox("一：z one side");
		}
		else
		{
			MessageBox("初始位置误差范围太大");
			return;
		}
		
		pos[1]=0;
		pos[2]=0;	

		//调整之前测试是否在可调范围内,是否超出软限位
		//x:[-1,1] y:[-1,1] z;[0.3,0.8] u:[-3,3] v:[-3,3] w:[-3,3]
		const char allCurrAex[16] = "X Y Z U V W";
	    double allCurrPos[6] = {0,0,0,0,0,0}; 
		if(AARobotCurrentPosition(allCurrAex, allCurrPos)<0)
		{
			MessageBox("获取当前位置失败");
			return;
		}
		double absDistance[6];
		absDistance[2]=pos[0]+allCurrPos[2];
		absDistance[3]=pos[1]+allCurrPos[3];
		absDistance[4]=pos[2]+allCurrPos[4];
		/*CString adjust_z_u_v12;
		adjust_z_u_v12.Format(_T("absz:%lf absu:%lf absv:%lf\n"),absDistance[2],absDistance[3],absDistance[4]);
		AfxMessageBox(adjust_z_u_v12);*/

		int result=AARobotMoveRelative(posStr, pos);
		if(result<0)
		{
			//AfxMessageBox("机器AA调整运动失败");
			MessageBox("超出机器运动范围");
			temp_number++;
			m_DistanceToOriginal=distanceToOriginal[temp_number];
			continue;
		}
		
		//m_AlignmentNumber++;
		//m_DistanceToOriginal=m_DistanceToOriginal/(pow(double(2),int(m_AlignmentNumber)));
		temp_number++;
		m_DistanceToOriginal=distanceToOriginal[temp_number];
		m_AAadjust_z=adjust_z;
		m_AAadjust_u=0;
		m_AAadjust_v=0;
		
	}

	
	//************************************************************************************************************
	//************************************************************************************************************
	//第二步：然后需再调整U/V轴
	int first_uv_number=0;
	m_AAadjust_z=-100;m_AAadjust_u=-100;m_AAadjust_v=-100;
	temp_number=0;
	double distanceToOriginalUV[9]={0.06,0.05,0.04,0.035,0.03,0.03,0.03,0.03,0.03};
	m_DistanceToOriginal=distanceToOriginalUV[temp_number];
	double t = 0.15;
	while(!(( fabs(m_AAadjust_u)<t && fabs(m_AAadjust_v)<t)||(temp_number>8)))
	{
		
	   //START1.....................获取当前帧的图片以及该图像正负向调整的图像........................
		const char axisString[5]="Z ";
		double Ori_ImageCurrent;
	    //1 原位置图片...............................
	    if(AARobotCurrentPosition(axisString, &Ori_ImageCurrent)<0)
		{
			MessageBox("获取当前位置失败");
			return;
		}
		/*CString filename_o="..//..//image//m_OriginalImage.bmp";
	    if(SavePicture(filename_o)<0)
		{
			AfxMessageBox("保存原图片失败");
			return;
		}*/
		Mat Ori_PictureMat;
		int Ori_flag=GetPictureMat(& Ori_PictureMat);
		if(Ori_flag<0)
		{
			AfxMessageBox("保存原始图片失败");
			return;
		}
		

		//2 正向位置图片.................................
		//正向移动一定间隔获取图像//移动机器人+m_DistanceToOriginal，并获取图像
		m_deltaZ_p=m_DistanceToOriginal;
		m_PlusImage=m_CurrentImageZData+m_DistanceToOriginal;
		if(AARobotMoveRelative(axisString, &m_DistanceToOriginal)<0)
		{
			AfxMessageBox("机器正向移动失败");
			temp_number++;
			m_DistanceToOriginal=distanceToOriginal[temp_number];
			continue;
		}
		//CString filename_p="..//..//image//m_PlusImage.bmp";
		/*if(SavePicture(filename_p)<0)
		{
			AfxMessageBox("保存正向图片失败");
			temp_number++;
			m_DistanceToOriginal=distanceToOriginal[temp_number];
			continue;
		}*/
		Mat Plus_PictureMat;
		int Plus_flag=GetPictureMat(& Plus_PictureMat);
		if(Plus_flag<0)
		{
			AfxMessageBox("保存原始图片失败");
			return;
		}


		//3 负向位置图片............................
		//负向移动一定间隔获取图像//移动机器人+m_DistanceToOriginal，并获取图像
		m_deltaZ_m=-m_DistanceToOriginal;
		m_MinusImage=m_CurrentImageZData-m_DistanceToOriginal;
		double ww=-2*m_DistanceToOriginal;
		if(AARobotMoveRelative(axisString, &ww)<0)
		{
			AfxMessageBox("机器负向移动失败");//若出错返回原来位置
			AARobotMoveRelative(axisString,&m_deltaZ_p);
			temp_number++;
			m_DistanceToOriginal=distanceToOriginal[temp_number];
			continue;
		}
		/*CString filename_m="..//..//image//m_MinusImage.bmp";
		if(SavePicture(filename_m)<0)
	    {
			AfxMessageBox("保存负向图片失败");
			return;
		}*/
		Mat Minus_PictureMat;
		int Minus_flag=GetPictureMat(& Minus_PictureMat);
		if(Minus_flag<0)
		{
			AfxMessageBox("保存原始图片失败");
			return;
		}

		if(AARobotMoveRelative(axisString, &m_DistanceToOriginal)<0)
		{
			AfxMessageBox("机器返回初始位置失败");
			return;
		}
	//END1

	//START2......................计算移动的距离................................................
		//const std::string &OriginalPath = (LPCSTR)filename_o;
		//const std::string &PlusPath = (LPCSTR)filename_p;
		//const std::string &MinusPath = (LPCSTR)filename_m;

		//Mat img_ori = imread(OriginalPath);
	   // Mat img_p = imread(PlusPath);
	  //  Mat img_m = imread(MinusPath);

		Mat img_ori =Ori_PictureMat;
		Mat img_p =Plus_PictureMat;
		Mat img_m =Minus_PictureMat;

	    img_ori = Img_resize(img_ori, m_scale);
	    img_p = Img_resize(img_p, m_scale);
	    img_m = Img_resize(img_m, m_scale);
		State state;
		int OneSide_flag;
		state = getState(img_ori,img_p,img_m,m_deltaZ_p,m_deltaZ_m,m_pixel_size,m_scale,&OneSide_flag);
		double adjust_z = state.deltaZ;
		double adjust_u = state.x_angle * -1;
		double adjust_v = state.y_angle * -1;
		first_uv_number++;
		/*
		CString adjust_z_u_v1;
		adjust_z_u_v1.Format(_T("二：uv：az:%lf au:%lf av:%lf"),adjust_z,adjust_u,adjust_v);
		AfxMessageBox(adjust_z_u_v1);
		*/
	    //START3.......................根据计算结果进行移动............................................
		const char posStr[16] = "z u v";
		double pos[3];
		pos[0]=0;

		if(OneSide_flag>-1)
		{
			pos[1]=adjust_u;
			pos[2]=adjust_v;
		}
		else if(OneSide_flag==-1)
		{
			//pos[1]=adjust_u/3;
			//pos[2]=adjust_v/3;
			pos[1]=adjust_u;
			pos[2]=adjust_v;
			MessageBox("二：UV one side");
		}
		else
		{
			MessageBox("初始位置误差范围太大");
			return;
		}

		//调整之前测试是否在可调范围内,是否超出软限位
		//x:[-1,1] y:[-1,1] z;[0.3,0.8] u:[-3,3] v:[-3,3] w:[-3,3]
		const char allCurrAex[16] = "X Y Z U V W";
	    double allCurrPos[6] = {0,0,0,0,0,0}; 
		if(AARobotCurrentPosition(allCurrAex, allCurrPos)<0)
		{
			MessageBox("获取当前位置失败");
			return;
		}
		double absDistance[6];
		absDistance[2]=pos[0]+allCurrPos[2];
		absDistance[3]=pos[1]+allCurrPos[3];
		absDistance[4]=pos[2]+allCurrPos[4];
		
		int result=AARobotMoveRelative(posStr, pos);
		if(result<0)
		{
			MessageBox("超出机器运动范围");
			temp_number++;
			m_DistanceToOriginal=distanceToOriginal[temp_number];
			continue;
		}
		
		temp_number++;
		m_DistanceToOriginal=distanceToOriginalUV[temp_number];
		m_AAadjust_z=0;
		m_AAadjust_u=adjust_u;
		m_AAadjust_v=adjust_v;
	}

	//************************************************************************************************************
	//************************************************************************************************************
	//第三步：然后需再调整Z轴
	int first_uvz_number=1;
	/*
	m_AAadjust_z=-100;m_AAadjust_u=-100;m_AAadjust_v=-100;
	temp_number=0;
	double distanceToOriginalUVz[9]={0.05,0.04,0.03,0.03,0.03,0.03,0.03,0.02,0.02};
	m_DistanceToOriginal=distanceToOriginalUVz[temp_number];
	while(!(( fabs(m_AAadjust_z)<0.005)||(temp_number>8)))
	{
	    //START1.....................保存当前帧的图片以及该图像正负向调整的图像........................................
		const char axisString[5]="Z ";
		double Ori_ImageCurrent;
	    //...............................
	    if(AARobotCurrentPosition(axisString, &Ori_ImageCurrent)<0)
		{
			MessageBox("获取当前位置失败");
			return;
		}
		Mat Ori_PictureMat;
		int Ori_flag=GetPictureMat(& Ori_PictureMat);
		if(Ori_flag<0)
		{
			AfxMessageBox("保存原始图片失败");
			return;
		}

		//正向移动一定间隔获取图像//移动机器人+m_DistanceToOriginal，并获取图像
		m_deltaZ_p=m_DistanceToOriginal;
		m_PlusImage=m_CurrentImageZData+m_DistanceToOriginal;
		if(AARobotMoveRelative(axisString, &m_DistanceToOriginal)<0)
		{
			AfxMessageBox("机器正向移动失败");
			temp_number++;
			m_DistanceToOriginal=distanceToOriginal[temp_number];
			continue;
		}
		Mat Plus_PictureMat;
		int Plus_flag=GetPictureMat(& Plus_PictureMat);
		if(Plus_flag<0)
		{
			AfxMessageBox("保存原始图片失败");
			return;
		}

		//负向移动一定间隔获取图像//移动机器人-m_DistanceToOriginal，并获取图像
		m_deltaZ_m=-m_DistanceToOriginal;
		m_MinusImage=m_CurrentImageZData-m_DistanceToOriginal;
		double ww=-2*m_DistanceToOriginal;
		if(AARobotMoveRelative(axisString, &ww)<0)
		{
			AfxMessageBox("机器负向移动失败");//若出错返回原来位置
			AARobotMoveRelative(axisString,&m_deltaZ_p);
			temp_number++;
			m_DistanceToOriginal=distanceToOriginal[temp_number];
			continue;
		}
		Mat Minus_PictureMat;
		int Minus_flag=GetPictureMat(& Minus_PictureMat);
		if(Minus_flag<0)
		{
			AfxMessageBox("保存原始图片失败");
			return;
		}
		if(AARobotMoveRelative(axisString, &m_DistanceToOriginal)<0)
		{
			AfxMessageBox("机器返回初始位置失败");
			return;
		}

	    //START2......................计算移动的距离................................................

		Mat img_ori =Ori_PictureMat;
		Mat img_p =Plus_PictureMat;
		Mat img_m =Minus_PictureMat;
	    img_ori = Img_resize(img_ori, m_scale);
	    img_p = Img_resize(img_p, m_scale);
	    img_m = Img_resize(img_m, m_scale);
		State state;
		int OneSide_flag;
		state = getState(img_ori,img_p,img_m,m_deltaZ_p,m_deltaZ_m,m_pixel_size,m_scale,&OneSide_flag);

		double adjust_z = state.deltaZ;
		double adjust_u = state.x_angle * -1;
		double adjust_v = state.y_angle * -1;
		first_uvz_number++;
		//CString adjust_z_u_v;
		//adjust_z_u_v.Format(_T("三：z：az:%lf au:%lf av:%lf"),adjust_z,0,0);
		//AfxMessageBox(adjust_z_u_v);

	    //START3.......................根据计算结果进行移动............................................
		const char posStr[16] = "z u v";
		double pos[3];
		if(OneSide_flag>-1)
		{
			pos[0]=adjust_z;
		}
		else if(OneSide_flag==-1)
		{
			//pos[0]=adjust_z/2;
			pos[0]=adjust_z;
			MessageBox("三：z one side");
		}
		else
		{
			MessageBox("初始位置误差范围太大");
			return;
		}
		
		pos[1]=0;
		pos[2]=0;	

		//调整之前测试是否在可调范围内,是否超出软限位
		//x:[-1,1] y:[-1,1] z;[0.3,0.8] u:[-3,3] v:[-3,3] w:[-3,3]
		const char allCurrAex[16] = "X Y Z U V W";
	    double allCurrPos[6] = {0,0,0,0,0,0}; 
		if(AARobotCurrentPosition(allCurrAex, allCurrPos)<0)
		{
			MessageBox("获取当前位置失败");
			return;
		}
		double absDistance[6];
		absDistance[2]=pos[0]+allCurrPos[2];
		absDistance[3]=pos[1]+allCurrPos[3];
		absDistance[4]=pos[2]+allCurrPos[4];
	

		int result=AARobotMoveRelative(posStr, pos);
		if(result<0)
		{
			//AfxMessageBox("机器AA调整运动失败");
			MessageBox("超出机器运动范围");
			temp_number++;
			m_DistanceToOriginal=distanceToOriginal[temp_number];
			continue;
		}
		
		temp_number++;
		m_DistanceToOriginal=distanceToOriginal[temp_number];
		m_AAadjust_z=adjust_z;
		m_AAadjust_u=0;
		m_AAadjust_v=0;
	}
	*/
	//************************************************************************************************************
	//************************************************************************************************************
	//第四步：五点法确定
	    //START1.....................保存当前帧的图片以及该图像正负向调整的图像........................................
		const char axisString[5]="Z ";
		double Ori_ImageCurrent;
	    //...............................
	    if(AARobotCurrentPosition(axisString, &Ori_ImageCurrent)<0)
		{
			MessageBox("获取当前位置失败");
			return;
		}
		Mat Ori_PictureMat;
		int Ori_flag=GetPictureMat(& Ori_PictureMat);
		if(Ori_flag<0)
		{
			AfxMessageBox("保存原始图片失败");
			return;
		}

		//正向移动第一张图，0.003
		m_DistanceToOriginal = 0.003;
		if(AARobotMoveRelative(axisString, &m_DistanceToOriginal)<0)
		{
			AfxMessageBox("机器正向移动失败");
			m_DistanceToOriginal=0;
		}
		Mat Plus_PictureMat1;
		int Plus_flag1=GetPictureMat(& Plus_PictureMat1);
		if(Plus_flag1<0)
		{
			AfxMessageBox("保存原始图片失败");
			return;
		}

		//正向移动第一张图，0.006
		if(AARobotMoveRelative(axisString, &m_DistanceToOriginal)<0)
		{
			AfxMessageBox("机器正向移动失败");
			m_DistanceToOriginal=0;
		}
		Mat Plus_PictureMat2;
		int Plus_flag2=GetPictureMat(& Plus_PictureMat2);
		if(Plus_flag2<0)
		{
			AfxMessageBox("保存原始图片失败");
			return;
		}

		////负向移动第一张图，0.003
		double ww1 =-3*m_DistanceToOriginal;
		if(AARobotMoveRelative(axisString, &ww1)<0)
		{
			AfxMessageBox("机器负向移动失败");//若出错返回原来位置
			m_DistanceToOriginal=0;
		}
		Mat Minus_PictureMat1;
		int Minus_flag1=GetPictureMat(& Minus_PictureMat1);
		if(Minus_flag1<0)
		{
			AfxMessageBox("保存原始图片失败");
			return;
		}
		
	    //负向移动第一张图，0.006
		double ww2 =-m_DistanceToOriginal;
		if(AARobotMoveRelative(axisString, &ww2)<0)
		{
			AfxMessageBox("机器负向移动失败");//若出错返回原来位置
			m_DistanceToOriginal=0;
		}
		Mat Minus_PictureMat2;
		int Minus_flag2=GetPictureMat(& Minus_PictureMat2);
		if(Minus_flag2<0)
		{
			AfxMessageBox("保存原始图片失败");
			return;
		}
		double ww3 =2*m_DistanceToOriginal;
		if(AARobotMoveRelative(axisString, &ww3)<0)
		{
			AfxMessageBox("机器返回初始位置失败");
			return;
		}
	    //START2......................计算移动的距离................................................

		Mat img_ori =Ori_PictureMat;
		Mat img_p1 =Plus_PictureMat1;
		Mat img_p2 =Plus_PictureMat2;
		Mat img_m1 =Minus_PictureMat1;
		Mat img_m2 =Minus_PictureMat2;

	    img_ori = Img_resize(img_ori, m_scale);
	    img_p1 = Img_resize(img_p1, m_scale);
		img_p2 = Img_resize(img_p2, m_scale);
	    img_m1 = Img_resize(img_m1, m_scale);
		img_m2 = Img_resize(img_m2, m_scale);
		State state;
		state = getState5(img_m2, img_m1, img_ori, img_p1, img_p2, m_pixel_size, m_scale);
		double adjust_z = state.deltaZ;
		double adjust_u = state.x_angle * -1;
		double adjust_v = state.y_angle * -1;

	    //START3.......................根据计算结果进行移动............................................
		const char posStr[16] = "z u v";
		double pos[3];
		pos[0]=adjust_z;
		pos[1]=0;
		pos[2]=0;
		t = 0;
		//if (fabs(m_AAadjust_u)>0.11)
		if (fabs(adjust_u)>t)
		{
			pos[1]=adjust_u;
		    
		}
		//if(fabs(m_AAadjust_v)>0.11)
		if (fabs(adjust_v)>t)
		{
			pos[2]=adjust_v;
		}
		/*
		CString adjust_z_u_v;
		adjust_z_u_v.Format(_T("三：z：az:%lf au:%lf av:%lf"),pos[0],pos[1],pos[2]);
		AfxMessageBox(adjust_z_u_v);
		*/	
		//调整之前测试是否在可调范围内,是否超出软限位
		//x:[-1,1] y:[-1,1] z;[0.3,0.8] u:[-3,3] v:[-3,3] w:[-3,3]
	
		int result=AARobotMoveRelative(posStr, pos);
		if(result<0)
		{
			//AfxMessageBox("机器AA调整运动失败");
			MessageBox("超出机器运动范围");
			m_DistanceToOriginal=0;
		}
		//START4......................判断是否调到位置............................................
	if (fabs(adjust_z) >= 0.006)
	{
		//START1.....................保存当前帧的图片以及该图像正负向调整的图像........................................
		const char axisString[5]="Z ";
		double Ori_ImageCurrent;
		vector<Mat> PictureMat;
		int direction = ((adjust_z > 0) - (adjust_z < 0));
		m_DistanceToOriginal = 0.003 * direction;
	    //...............................
		//向偏移方向移动四张张图，间隔0.003
	    for (int i = 0; i < 4; i++)
		{
			if(AARobotMoveRelative(axisString, &m_DistanceToOriginal)<0)
		    {
			    AfxMessageBox("机器正向移动失败");
			    m_DistanceToOriginal=0;
		    }
		    Mat Plus_PictureMat;
		    int Plus_flag1=GetPictureMat(& Plus_PictureMat);
		    if(Plus_flag1<0)
		    {
			    AfxMessageBox("保存原始图片失败");
			    return;
		    }
			PictureMat.push_back(Plus_PictureMat);
		}
		
		double ww3 = -2*m_DistanceToOriginal;
		if(AARobotMoveRelative(axisString, &ww3)<0)
		{
			AfxMessageBox("机器返回初始位置失败");
			return;
		}
	    //START2......................计算移动的距离................................................
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
	    img_ori = Img_resize(img_ori, m_scale);
	    img_p1 = Img_resize(img_p1, m_scale);
		img_p2 = Img_resize(img_p2, m_scale);
	    img_m1 = Img_resize(img_m1, m_scale);
		img_m2 = Img_resize(img_m2, m_scale);
		State state;
		state = getState5(img_m2, img_m1, img_ori, img_p1, img_p2, m_pixel_size, m_scale);
		double adjust_z = state.deltaZ;
		double adjust_u = state.x_angle * -1;
		double adjust_v = state.y_angle * -1;
		
	    //START3.......................根据计算结果进行移动............................................
		const char posStr[16] = "z u v";
		double pos[3];
		pos[0]=adjust_z;
		pos[1]=0;
		pos[2]=0;
		//if (fabs(m_AAadjust_u)>0.11)
		if (fabs(adjust_u)>t)
		{
			pos[1]=adjust_u;
		    
		}
		//if(fabs(m_AAadjust_v)>0.11)
		if (fabs(adjust_v)>t)
		{
			pos[2]=adjust_v;
		}
		/*
		CString adjust_z_u_v;
		adjust_z_u_v.Format(_T("三：z：az:%lf au:%lf av:%lf"),(pos[0]+(0.006*direction)),pos[1],pos[2]);
		AfxMessageBox(adjust_z_u_v);
		*/	
		//调整之前测试是否在可调范围内,是否超出软限位
		//x:[-1,1] y:[-1,1] z;[0.3,0.8] u:[-3,3] v:[-3,3] w:[-3,3]
	
		int result=AARobotMoveRelative(posStr, pos);
		if(result<0)
		{
			//AfxMessageBox("机器AA调整运动失败");
			MessageBox("超出机器运动范围");
			m_DistanceToOriginal=0;
		}
		first_uvz_number++;
	}
	//************************************************************************************************************
	//第五步：调整x,y
	Mat Ori_PictureMat1;
	int Ori_flag1=GetPictureMat(& Ori_PictureMat1);
	if(Ori_flag1<0)
	{
		AfxMessageBox("保存原始图片失败");
		return;
	}
	Ori_PictureMat1 = Img_resize(Ori_PictureMat1, m_scale);
	OFF_set offset;
	offset =  getOffcenter(Ori_PictureMat1, m_pixel_size);
	double adjust_x = offset.x_offset;
	double adjust_y = offset.y_offset;
	//START3.......................根据计算结果进行移动............................................
	const char posStr_xy[6] = "X Y";
	double pos_xy[2];
	pos_xy[0]=-adjust_y;
	pos_xy[1]=adjust_x;	

	//调整之前测试是否在可调范围内,是否超出软限位
	//x:[-1,1] y:[-1,1] z;[0.3,0.8] u:[-3,3] v:[-3,3] w:[-3,3]
	const char allCurrAex[16] = "X Y Z U V W";
	double allCurrPos[6] = {0,0,0,0,0,0}; 
	if(AARobotCurrentPosition(allCurrAex, allCurrPos)<0)
	{
		MessageBox("获取当前位置失败");
		return;
	}
	double absDistance[6];
	absDistance[0]=pos_xy[0]+allCurrPos[0];
	absDistance[1]=pos_xy[1]+allCurrPos[1];
	absDistance[2]=allCurrPos[2];
	absDistance[3]=allCurrPos[3];
	absDistance[4]=allCurrPos[4];

	int result_xy=AARobotMoveRelative(posStr_xy, pos_xy);
	if(result_xy<0)
	{
		//AfxMessageBox("机器AA调整运动失败");
		MessageBox("超出机器运动范围");
		m_DistanceToOriginal=0;
	}
	//************************************************************************************************************
	//************************************************************************************************************
	//第六步：保存最终结果的图片	
	time = ((double)getTickCount() - time) / getTickFrequency();//计算所用时间
	CString m_TempName;
	m_TempName.Format(_T("所用时间:%lfs.\n\n z轴调整次数:%d; UV轴调整次数:%d 最后z又调整次数:%d.\n\n总共调节次数:%d, 最终z位置:%f,\n\n 最终u位置:%f, 最终v位置:%f"),time,first_z_number,first_uv_number,first_uvz_number,first_z_number+first_uv_number+first_uvz_number,absDistance[2],absDistance[3],absDistance[4]);
	MessageBox(m_TempName);
 	SavePicture(ALLDynamicList+FirstImage+".bmp");//_Last
	//返回初始设置值
	m_AAadjust_z=100;
	m_AAadjust_u=100;
	m_AAadjust_v=100;
	m_DistanceToOriginal=-0.1;
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
    //	MessageBox(ComPortValue);
	//m_ID值更新
	if(m_ComInitResult<0)
	{
		m_ComInitResult=AARobotInit(ComPortValue);
		//m_ComInitResult= AARobotTCPIPInit();
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
	if(AARobotCurrentPosition(allAex, allPos)<0)
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

	GetDlgItem(IDC_INIT_X)->GetWindowText(Initstr_x);
	GetDlgItem(IDC_INIT_Y)->GetWindowText(Initstr_y);
	GetDlgItem(IDC_INIT_Z)->GetWindowText(Initstr_z);
	GetDlgItem(IDC_INIT_U)->GetWindowText(Initstr_u);
	GetDlgItem(IDC_INIT_V)->GetWindowText(Initstr_v);
	GetDlgItem(IDC_INIT_W)->GetWindowText(Initstr_w);

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

	allPos[0]=-allPos[0]+double(Initdou_x);
	allPos[1]=-allPos[1]+double(Initdou_y);
	allPos[2]=-allPos[2]+double(Initdou_z);
	allPos[3]=-allPos[3]+double(Initdou_u);
	allPos[4]=-allPos[4]+double(Initdou_v);
	allPos[5]=-allPos[5]+double(Initdou_w);
	
	/*CString adjust_xyzuvw;
	CString adjust_xyzuvw1;
	adjust_xyzuvw.Format(_T("%lf %lf %lf %lf %lf %lf\n"),Initdou_x,Initdou_y,Initdou_z,Initdou_u,Initdou_v,Initdou_w);
	adjust_xyzuvw1.Format(_T("%lf %lf %lf %lf %lf %lf\n"),allPos[0],allPos[1],allPos[2],allPos[3],allPos[4],allPos[5]);
	AfxMessageBox(adjust_xyzuvw0+adjust_xyzuvw+adjust_xyzuvw1);
	
	int Aresult=AARobotMoveRelative(allAex, allPos);
	if(Aresult<0){
		CString Aresult1;
		Aresult1.Format(_T("result:\n%d",Aresult));
		AfxMessageBox(Aresult1);
		AfxMessageBox("机器回初始位置失败");
		return;}
	*/
	
	if(AARobotMoveRelative(allAex, allPos)<0)
	{
		
		MessageBox("获取当前位置失败");
		return;
	
	}
	AfxMessageBox("机器回初始位置成功");
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
