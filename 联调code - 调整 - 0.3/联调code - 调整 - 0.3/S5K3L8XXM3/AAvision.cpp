#include "AAvision.h"
#include "dtccm2.h" 

#include "AA_define_tool.h"
#include "AAvisionthreads.h"
#include <process.h>


//#include "IniFileRW.h"//这个读写配置文件是否合适,后续改善
#include "Config.h"
int CAAVision::savepic = 0;

BOOL   CAAVision::m_paintflag = FALSE;
 CAAVision::~CAAVision()
{
	//
	//清除
	if (CurrentSensorA.ParaList)
	{
		delete CurrentSensorA.ParaList;
	}
	//其他的参数好像没有用到,后续完善

	DeleteCriticalSection(&m_csLock);
	
}

 CAAVision::CAAVision(string name,int defineid/*,string inifilePath*/)
 {
	 AAFrameCnt = 0;
	 AAPlayCnt =0;
	 m_pSensorNameA = name;
	 m_iWorkStatusA = TESTOR_STOP;
	 m_iDevIDA =-1;
	 m_iCurIDA = defineid;

	 m_pTripleBufferA = NULL;
	 m_pDisplayBufferA = NULL;
	 m_pCameraBufferA = NULL;
	 m_GrabSizeA = 0;

	 m_isTV = FALSE ;//显示或者抓图是图宽度小一半,没什么作用,保留

	 savepic = CFG_INSTANCE->get_cfg_int(AACoreCfg,savepicture); //是否保存图片
	 InitializeCriticalSection(&m_csLock);
	// m_sFwFileFolderA = inifilePath; 
	 //m_sFwFileFolderA = "C:\\Users\\Think\\Desktop\\采图程序\\S5K3L8.ini"
	 /*
		m_fAvddA  = 2.8f;
		m_fDovddA = 1.8f;
		 m_fDvddA = 1.2f;
		 m_fAfvccA =1.2f;
		 m_vppA = 0.0f;
		 m_fMclkA= 12.0f;
		 m_uFocusPosA =0;*/

	 //

	 InitSomeParam();
		 

 }
 void CAAVision::InitSomeParam()
 {
	string temp = m_pSensorNameA.substr(m_pSensorNameA.length()-1,1);
	string str;
	str = Mclk;
	str.append(temp);
	m_fMclkA = CFG_INSTANCE->get_cfg_double(AAVISION_MCLK,str.c_str());

	str = Avdd;
	str.append(temp);
	m_fAvddA = CFG_INSTANCE->get_cfg_double(AAVISION_POWER,str.c_str())/1000;
	str = Dvdd;
	str.append(temp);
	m_fDvddA = CFG_INSTANCE->get_cfg_double(AAVISION_POWER,str.c_str())/1000;
	str = Dovdd;
	str.append(temp);
	m_fDovddA = CFG_INSTANCE->get_cfg_double(AAVISION_POWER,str.c_str())/1000;
	str = Afvcc;
	str.append(temp);
	m_fAfvccA = CFG_INSTANCE->get_cfg_double(AAVISION_POWER,str.c_str())/1000;
	str = Vpp;
	str.append(temp);
	m_vppA = CFG_INSTANCE->get_cfg_double(AAVISION_POWER,str.c_str())/1000;
	str = FocusPos;
	str.append(temp);
	m_uFocusPosA = CFG_INSTANCE->get_cfg_int(AAVISION_AF,str.c_str());
 }

int CAAVision::EnumerateAAVision(char *DeviceName[],int iDeviceNumMax,int *pDeviceNum)
{

	return EnumerateDevice(DeviceName,iDeviceNumMax,pDeviceNum);

}

int CAAVision::OpenVisionDevice()
{
	int ret = DT_ERROR_OK;
	int iDevIDA = -1;;
	if (m_iWorkStatusA == TESTOR_START) //表明已经开视频了,那肯定是打开了设备
	{
		return ret;
	}
	//
	if (m_iDevIDA>=0)
	{
		if (IsDevConnect(m_iDevIDA) == DT_ERROR_OK)
		{
			return ret;
		}	
	}
	if (m_iCurIDA<0)
	{
		return -1;
	}
	if ((ret = OpenDevice(m_pSensorNameA.c_str(),&iDevIDA,m_iCurIDA)) != DT_ERROR_OK)
	{
		PRINTF("Open %s failed.\r\n",m_pSensorNameA.c_str());
		return ret;
	}
	if (iDevIDA!=m_iCurIDA)
	{
		PRINTF("Open %s failed，打开时id跟要求的id不一样.\r\n",m_pSensorNameA.c_str());
	}
	m_iDevIDA = iDevIDA;
	return ret;
}
int CAAVision::CloseVisionDevice()
{
	m_iWorkStatusA = TESTOR_STOP;

	m_pTripleBufferA = NULL;
	m_pDisplayBufferA = NULL;
	m_pCameraBufferA = NULL;

	return CloseDevice(m_iDevIDA);
}

BOOL CAAVision::TripleBufferFlipA(LPBYTE *pBuf, BOOL bPrimaryBuffer)
{
	BOOL bFlipped = FALSE;

	EnterCriticalSection(&m_csLock);
	{
		if (bPrimaryBuffer || m_bTripleBufferDirtyA)
		{
			LPBYTE pTmp = *pBuf;
			*pBuf = m_pTripleBufferA;
			m_pTripleBufferA = pTmp;
			bFlipped = TRUE;
		}
		m_bTripleBufferDirtyA = bPrimaryBuffer;
	}
	LeaveCriticalSection(&m_csLock);

	return bFlipped;	
}
void CAAVision::KillDataBuffer(BYTE byChannel)//通道到底有没有用还不清楚,暂时询问说是只用chanlA
{
	//delete the data buffer.
	//if (byChannel == CHANNEL_A)
	{
		if (m_pTripleBufferA)
		{
			free(m_pTripleBufferA);
		}
		if (m_pCameraBufferA)
		{
			free(m_pCameraBufferA);
		}
		if (m_pDisplayBufferA)
		{
			free(m_pDisplayBufferA);
		}

		m_pTripleBufferA = NULL;
		m_pCameraBufferA = NULL;
		m_pDisplayBufferA = NULL;
	}
}
BOOL CAAVision::SetVoltageMclk(int iDevID,float mclk,float avdd,float dvdd,float dovdd,float afvcc,float vpp)
{
	//int ret = DT_ERROR_OK;

	//int iDevID = m_iDevIDA;

	SENSOR_POWER Power[10] = {POWER_AVDD, POWER_DOVDD, POWER_DVDD, POWER_AFVCC, POWER_VPP};
	int Volt[10] = {0};
	int Current[10] = {300, 300, 300, 300, 100};//300mA
	BOOL OnOff[10] = {FALSE};
	CURRENT_RANGE range[5] = {CURRENT_RANGE_MA, CURRENT_RANGE_MA, CURRENT_RANGE_MA, CURRENT_RANGE_MA, CURRENT_RANGE_MA};
	//set power to 0V
	if (PmuSetVoltage(Power, Volt,5, iDevID) != DT_ERROR_OK)
	{
		CloseDevice(iDevID);
		PRINTF("Set Voltage Failed!");
		//return FALSE;
	}
	//wait for the power is all to zero....
	Sleep(50);
	//设置电压开关 switch to on...
	if (PmuSetOnOff(Power, OnOff, 5, iDevID) != DT_ERROR_OK)
	{
		CloseDevice(iDevID);
	
		PRINTF("Open PowerOnOff Failed!");
		//return FALSE;
	}
	//20160103 power on the sensor...////		//first power the avdd.
	// 1. set power switch to on and power the avdd
	OnOff[POWER_AVDD]=TRUE;
	if (PmuSetOnOff(Power, OnOff, 5, iDevID) != DT_ERROR_OK)
	{
		CloseDevice(iDevID);
		PRINTF("Open PowerOnOff Failed!");
		//return FALSE;
	}
	Volt[POWER_AVDD] = (int)(avdd * 1000); // 2.8V
	if(PmuSetVoltage(Power, Volt, 5, iDevID)!=DT_ERROR_OK)
	{
		PRINTF("Open PowerOn Failed!");
		//return DT_ERROR_FAILED;
	}
	Sleep(1);
	// 2, set power switch to on and power the dvdd 
	OnOff[POWER_DVDD]=TRUE;
	if (PmuSetOnOff(Power, OnOff, 5, iDevID) != DT_ERROR_OK)
	{
		CloseDevice(iDevID);
		PRINTF("Open PowerOnOff Failed!");
		//return FALSE;
	}
	Volt[POWER_DVDD] = (int)(dvdd * 1000); // 2.8V
	if(PmuSetVoltage(Power, Volt, 5, iDevID)!=DT_ERROR_OK)
	{
		PRINTF("Open PowerOn Failed!");
		//return DT_ERROR_FAILED;
	}
	Sleep(2);

	// 3, set power switch to on and power the dovdd
	OnOff[POWER_DOVDD]=TRUE;
	if (PmuSetOnOff(Power, OnOff, 5, iDevID)!= DT_ERROR_OK)
	{
		CloseDevice(iDevID);
		//AfxMessageBox("Close Power Failed!");
		PRINTF("Open PowerOnOff Failed!");
		//return FALSE;
	}
	Volt[POWER_DOVDD] = (int)(dovdd * 1000); // 2.8V
	if(PmuSetVoltage(Power, Volt, 5, iDevID)!=DT_ERROR_OK)
	{
		PRINTF("Open PowerOn Failed!");
		//return DT_ERROR_FAILED;
	}
	Sleep(2);
	//4. set power switch to on and power the afvcc and vpp
	OnOff[POWER_AFVCC]=TRUE;
	OnOff[POWER_VPP]=TRUE;
	if (PmuSetOnOff(Power, OnOff, 5, iDevID) != DT_ERROR_OK)
	{
		CloseDevice(iDevID);
		//AfxMessageBox("Close Power Failed!");
		PRINTF("Open PowerOnOff Failed!");
		//return FALSE;
	}
	Volt[POWER_AFVCC] = (int)(afvcc * 1000); // 2.8V
	Volt[POWER_VPP] = (int)(vpp * 1000); // 2.8V 
	if(PmuSetVoltage(Power, Volt, 5, iDevID)!=DT_ERROR_OK)
	{
		PRINTF("Open PowerOn Failed!");
		return DT_ERROR_FAILED;
	}
	//should wait for 50ms to be ready...
	Sleep(50);
	//设置采集速率
	int SampleSpeed[5]={100,100,100,100,100};
	PmuSetSampleSpeed(Power,SampleSpeed,5,iDevID);
	Sleep(10);
	//为了兼容HM不能设置量程故不能判断返回值
	{
		// 设置量程
		PmuSetCurrentRange(Power,range,5,iDevID); 
		//设置电流
		PmuSetOcpCurrentLimit(Power,Current,5,iDevID);
	}
	if (SetSensorClock(TRUE,(USHORT)(mclk*10), iDevID) != DT_ERROR_OK)
	{
		//AfxMessageBox("Set Mclk Failed!");
		PRINTF("Set Mclk Failed!");
		return DT_ERROR_FAILED;
	}
	//开启IO上拉电阻
	SetSoftPinPullUp(IO_PULLUP, iDevID);
	return TRUE;

}
int CAAVision::UinitCamera()
{
	int ret = DT_ERROR_OK;
	int iDevID = m_iDevIDA;
	//close
	{
		CloseVideo(m_iDevIDA);
		if(1)
		{
			ResetSensorI2cBus(m_iDevIDA);
			SensorEnable(CurrentSensorA.pin ^ 2, 1, m_iDevIDA);       //reset sensor....
			Sleep(20);
			SensorEnable(CurrentSensorA.pin ^ 1, 1, m_iDevIDA);       //power down sensor....
			Sleep(2);
			if(ReadSensorReg(CurrentSensorA.SlaveID, CurrentSensorA.FlagReg, &(CurrentSensorA.FlagData),  CurrentSensorA.mode))
			{
				//if pwdn not work...,just set to reset state...
				SensorEnable(CurrentSensorA.pin^2,1,m_iDevIDA);       //reset sensor....
			}
		}

		//close all power....
		{
			//close the clock
			SetSensorClock(0, 24*10, m_iDevIDA);
			//close the IO power...
			SetSoftPinPullUp(IO_NOPULL, m_iDevIDA);
			//close reset and power down pin...
			SensorEnable(0, 1, m_iDevIDA);
			//关闭电压
			SENSOR_POWER Power[10]={POWER_AVDD,POWER_DOVDD,POWER_DVDD,POWER_AFVCC,POWER_VPP};
			BOOL OnOff[10] = {0};	
			int Volt[10] = {0};
			//close the power switch...
			//if necessory, you can set the power voltage to 0
			PmuSetVoltage(Power, Volt, 5, m_iDevIDA);
			Sleep(50);
			PmuSetOnOff(Power,OnOff,5,m_iDevIDA);
		}
	}
	KillDataBuffer(CHANNEL_A);

	return ret;

}
int CAAVision::InitCamera()
{
	int ret = DT_ERROR_OK;
	int iDevID = m_iDevIDA;
	KillDataBuffer();//清除缓存
	ret = SetSoftPinPullUp(IO_NOPULL, iDevID);//关闭使能上拉电阻
	if ((ret = SetSensorClock(FALSE,(USHORT)(0*10), iDevID)) != DT_ERROR_OK)//关闭输入时钟 10没有用
	{
		CloseDevice(iDevID);
		PRINTF("Set Mclk Failed!");
		return FALSE;
	}
	Sleep(1);
	ret = SetPinDefinition();//		设置柔性接口
	ret = EnableSoftPin(TRUE,iDevID);//使能柔性接口和gpio
	ret = EnableGpio(TRUE,iDevID);
	ret = SetVoltageMclk(iDevID,m_fMclkA, m_fAvddA,m_fDvddA, m_fDovddA, m_fAfvccA, m_vppA);
	Sleep(10);
	//i2C init....
	//设置SENSOR I2C的速率为400Kbps,允许从设备为Streching mode（从设备端可以拉低scl和sda电平来表示busy）
	ret = SetSensorI2cRate(I2C_400K, iDevID);
	ret = SetSensorI2cRapid(TRUE, iDevID);

	//check sensor is on line or not ,if on line,init sensor to work....
	ret = SensorEnable(CurrentSensorA.pin ^ 0x02, 1, iDevID); //reset
	Sleep(20);
	ret = SensorEnable(CurrentSensorA.pin, 1, iDevID);
	Sleep(50);
	//check sensoris on line...
	if(SensorIsMe(&CurrentSensorA, CHANNEL_A, 0,iDevID) != DT_ERROR_OK)//为什么不是他自己
	{
		PRINTF("Sensor is not ok!");
		//return DT_ERROR_FAILED;			
	}
	//init sensor....
	if(InitSensor(CurrentSensorA.SlaveID,
		CurrentSensorA.ParaList,
		CurrentSensorA.ParaListSize,
		CurrentSensorA.mode,iDevID) != DT_ERROR_OK)
	{
		PRINTF("Init Sensor Failed!");
		return DT_ERROR_FAILED;			
	}

	if(CurrentSensorA.type == D_YUV || CurrentSensorA.type == D_YUV_SPI || CurrentSensorA.type == D_YUV_MTK_S)
		SetYUV422Format(CurrentSensorA.outformat, iDevID);
	else
		SetRawFormat(CurrentSensorA.outformat, iDevID);

	//if (byChannel == CHANNEL_A)
	{
		m_PreviewWidthA = CurrentSensorA.width;
		m_PreviewHeightA = CurrentSensorA.height;//& 0xfffe;
	}
	/*
	if (byChannel == CHANNEL_B)
	{
		m_PreviewWidthB = CurrentSensor.width;
		m_PreviewHeightB = CurrentSensor.height;//& 0xfffe;
	}*/

	USHORT roi_x0 = CurrentSensorA.width >> 2;
	USHORT roi_y0 = CurrentSensorA.height >> 2;
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
	ret = InitRoi(0, 0, CurrentSensorA.width, CurrentSensorA.height , 0, 0, 1, 1, CurrentSensorA.type, TRUE, iDevID);	//以像素为单位，全分辨率显示关闭ROI使能
	ret = SetSensorPort(CurrentSensorA.port, CurrentSensorA.width, CurrentSensorA.height, iDevID);
	return DT_ERROR_OK;//先返回ok
}
int CAAVision::OpenVisionVideo(HWND hWnd)
{
	int ret = DT_ERROR_OK;
	if (m_iWorkStatusA == TESTOR_START) //表明已经开视频了
	{
		return ret;
	}
	if (m_iDevIDA <0) //先打开设备
	{
		return -1;
	}
	ret = InitCamera();
	ret = CalculateGrabSize(&m_GrabSizeA, m_iDevIDA);
	ret = OpenVideo(m_GrabSizeA,m_iDevIDA);

	ret = InitDisplay(hWnd,CurrentSensorA.width,CurrentSensorA.height,CurrentSensorA.type, CHANNEL_A, NULL, m_iDevIDA);
	ret = InitIsp(CurrentSensorA.width, CurrentSensorA.height, CurrentSensorA.type, CHANNEL_A , m_iDevIDA);

	//SetTimer(m_iDevIDA+1,500,NULL);定时器要不要在类里面设置,主要是显示消息
	{
		m_iWorkStatusA = TESTOR_START;
		UINT nSize = CurrentSensorA.width * CurrentSensorA.height * 3 + 1024;
		m_pTripleBufferA = (LPBYTE)malloc(nSize);
		m_pCameraBufferA = (LPBYTE)malloc(nSize);
		m_pDisplayBufferA = (LPBYTE)malloc(nSize);

		if (m_pTripleBufferA  == NULL  || 
			m_pCameraBufferA == NULL  || 
			m_pDisplayBufferA == NULL 
			)
		{
			PRINTF("Memory error!");
			//return FALSE;
		}
		memset(m_pTripleBufferA, 0, nSize);
		memset(m_pCameraBufferA, 0, nSize);
		memset(m_pDisplayBufferA, 0, nSize);
	}

	if (m_iWorkStatusA ==  TESTOR_START)//开视频
	{
		m_hCameraThreadEventDeadA = CreateEvent(NULL, TRUE, FALSE, NULL);
		m_hDisplayThreadEventDeadA = CreateEvent(NULL, TRUE, FALSE, NULL);
		//_beginthread( Camera_ThreadA, 0, (void*)this);
		//_beginthread( Display_ThreadA, 0, (void*)this);

		_beginthreadex( NULL,0,(unsigned int (__stdcall *)(void *))Camera_ThreadA, (void*)this,0 ,NULL);
		_beginthreadex( NULL,0,(unsigned int (__stdcall *)(void *))Display_ThreadA, (void*)this,0,NULL);

	}

	return ret;

}
int CAAVision::CloseVisionVideo()
{
	int ret = DT_ERROR_OK;

	if(m_iWorkStatusA == TESTOR_STOP || (DT_ERROR_OK != IsDevConnect(m_iDevIDA)))
		return ret;
	m_iWorkStatusA = TESTOR_STOP;

	if(m_hCameraThreadEventDeadA)
	{
		WaitForSingleObject(m_hCameraThreadEventDeadA, INFINITE);
		CloseHandle(m_hCameraThreadEventDeadA);
		m_hCameraThreadEventDeadA = NULL;
	}

	if(m_hDisplayThreadEventDeadA)
	{
		WaitForSingleObject(m_hDisplayThreadEventDeadA, INFINITE);
		CloseHandle(m_hDisplayThreadEventDeadA);
		m_hDisplayThreadEventDeadA = NULL;
	}

	ret = UinitCamera();
	//KillTimer(1);

	return ret;
}
LPBYTE CAAVision::GrapOnePictureWhenVideo()
{
	int ret = DT_ERROR_OK;
	//wait the thread to end
	BYTE mode = m_iWorkStatusA;;

	if(m_iWorkStatusA == TESTOR_STOP)
	{
		PRINTF("Camera %s,is closed!Please Open it.",m_pSensorNameA);
		return NULL;
	}
	else //stop the thread and stop capture image and display...
	{
		//m_iWorkStatusA = TESTOR_STOP;
	}
	//wait the thread to end
	/*if(mode == TESTOR_START)
	{
		WaitForSingleObject(m_hCameraThreadEventDeadA, INFINITE);
		WaitForSingleObject(m_hDisplayThreadEventDeadA, INFINITE);
		if(m_hCameraThreadEventDeadA)
		{
			CloseHandle(m_hCameraThreadEventDeadA);
			m_hCameraThreadEventDeadA = NULL;
		}
		if(m_hDisplayThreadEventDeadA)
		{
			CloseHandle(m_hDisplayThreadEventDeadA);
			m_hDisplayThreadEventDeadA = NULL;
		}

	}*/

	USHORT width  = m_PreviewWidthA;
	USHORT height = m_PreviewHeightA;
	BYTE   type   =CurrentSensorA.type;
	LPBYTE BmpBuffer = NULL;
	BmpBuffer = (LPBYTE)malloc(width*height*4);
	if((BmpBuffer == NULL) )
	{
		PRINTF("Memory allocate error!");
		m_iWorkStatusA=mode;	
	/*	if(mode==TESTOR_START)
		{
			//SetDlgItemText(IDC_PLAY,"&PAUSE");
			m_hCameraThreadEventDeadA = CreateEvent(NULL, TRUE, FALSE, NULL);
			_beginthread( Camera_ThreadA, 0, (void*)this);
			m_hDisplayThreadEventDeadA = CreateEvent(NULL, TRUE, FALSE, NULL);
			_beginthread( Display_ThreadA, 0, (void*)this); 

		}*/
		return NULL;
	}
	//allocate the bmp buffer.
	UINT nSize = width*height*3 + 1024*1024;
	LPBYTE CameraBuffer = NULL;
	CameraBuffer = (LPBYTE)malloc(nSize);
	if((CameraBuffer == NULL) )
	{
		PRINTF("Memory allocate error!");
		free(BmpBuffer);
		m_iWorkStatusA=mode;	
	/*	if(mode==TESTOR_START)
		{
			//SetDlgItemText(IDC_PLAY,"&PAUSE");
			m_hCameraThreadEventDeadA = CreateEvent(NULL, TRUE, FALSE, NULL);
			_beginthread( Camera_ThreadA, 0, (void*)this);
			m_hDisplayThreadEventDeadA = CreateEvent(NULL, TRUE, FALSE, NULL);
			_beginthread( Display_ThreadA, 0, (void*)this); 

		}*/
		return NULL;
	}
	memset(CameraBuffer, 0, nSize);
	ULONG RetSize = 0;
	UINT i;
	FrameInfo Info;
	int bRet = 0;
	for( i=0; i<GRAPPITURENUM; i++)
	{
		if(m_isTV)//电视模式，就是 感兴趣的区域的宽度变小一半:m_isTV ? CurrentSensor.height >> 1 :CurrentSensor.height
		{
			bRet = GrabFrame(CameraBuffer, m_GrabSizeA,&RetSize,&Info,m_iDevIDA);
			if(bRet == DT_ERROR_OK)
			{
				bRet = GrabFrame(CameraBuffer+m_GrabSizeA, m_GrabSizeA,&RetSize,&Info,m_iDevIDA);
				if(bRet == DT_ERROR_OK)
				{
					break;
				}
			}
		}
		else
		{
			if (GrabFrame(CameraBuffer,m_GrabSizeA, &RetSize,&Info, m_iDevIDA) == DT_ERROR_OK)
			{
				break;
			}
		}
	}
	if(i == GRAPPITURENUM)
	{
		PRINTF("failed to save!");
	}
	else
	{
		ImageProcess(CameraBuffer,BmpBuffer,width,height,&Info,m_iDevIDA);
	}

	m_iWorkStatusA=mode;	
/*	if(mode==TESTOR_START)
	{
		//SetDlgItemText(IDC_PLAY,"&PAUSE");
		m_hCameraThreadEventDeadA = CreateEvent(NULL, TRUE, FALSE, NULL);
		_beginthread( Camera_ThreadA, 0, (void*)this);
		m_hDisplayThreadEventDeadA = CreateEvent(NULL, TRUE, FALSE, NULL);
		_beginthread( Display_ThreadA, 0, (void*)this); 

	}*/


	if (CameraBuffer)
	{
		free(CameraBuffer);
		CameraBuffer = NULL;
	}
	return BmpBuffer ; //调用者释放
}
LPBYTE CAAVision::GrapOnePicture()
{
	//抓图分两种,一种是在开了视频的情况下,一种是在没有开视频的情况
	int ret = DT_ERROR_OK;
	//第一种：
	if (m_iWorkStatusA == TESTOR_START )
	{
		return GrapOnePictureWhenVideo();
	}
	//第二种：
	if (InitCamera() != DT_ERROR_OK)
	{
		PRINTF("InitCamera is Failed  !");
		return NULL;
	}

	ret = OpenVideo(m_GrabSizeA,m_iDevIDA);
    ret = InitIsp(CurrentSensorA.width, CurrentSensorA.height, CurrentSensorA.type, CHANNEL_A , m_iDevIDA);

	UINT nSize = CurrentSensorA.width*CurrentSensorA.height*3 + 1024*1024;
	LPBYTE CameraBuffer = NULL;
	CameraBuffer = (LPBYTE)malloc(nSize);
	if((CameraBuffer == NULL) )
	{
		PRINTF("Memory allocate error!");
		return NULL;
	}
	memset(CameraBuffer, 0, nSize);

	ULONG RetSize = 0;
	FrameInfo m_FrameInfoA;
	ret = CalculateGrabSize(&m_GrabSizeA, m_iDevIDA);
	for (int i =0;i<GRAPPITURENUM;i++)
	{
		ret = GrabFrame(CameraBuffer, m_GrabSizeA,&RetSize,&m_FrameInfoA,m_iDevIDA);
		if (DT_ERROR_OK == ret)
		{
			ret = GrabFrame(CameraBuffer, m_GrabSizeA,&RetSize,&m_FrameInfoA,m_iDevIDA);
			if (DT_ERROR_OK == ret)
			{
				break;
			}
		}
	}
	ret = CloseVideo(m_iDevIDA);
	LPBYTE BmpBuffer = NULL;
	//MallocBuffer(&BmpBuffer, width * height * 4);
	BmpBuffer = (LPBYTE)malloc(CurrentSensorA.width*CurrentSensorA.height*4);

	ret = ImageProcess(CameraBuffer,BmpBuffer,CurrentSensorA.width,CurrentSensorA.height,&m_FrameInfoA,m_iDevIDA);

	if (CameraBuffer!= NULL)
	{
		free(CameraBuffer);
	}
	return BmpBuffer;
}
int CAAVision::SaveOnePicture()
{
	int ret = DT_ERROR_OK;

	return ret;
}

int CAAVision::GetVisionDeviceSN(BYTE *pSN,int iBufferSize,int *pRetLen)
{
	return GetDeviceSN(pSN,iBufferSize,pRetLen,m_iDevIDA);
}
 BOOL CAAVision::SetPaintFlag(BOOL flag)
{
	m_paintflag =  flag;

	return m_paintflag;
}
 
 void CAAVision::PaintColor(BYTE* pBmp,FrameInfo& frameInfo,int x,int y,int width ,int height ,int type,int color)
 {
	 for (int i= x;i<x+width;i++)
	 {
		 for (int j= y ;j<y +height;j++)
		 {
			 switch(color)
			 {		
			 case 1://blue
				 pBmp[3*(i + j*frameInfo.uWidth) +0] = 255;
				 pBmp[3*(i + j*frameInfo.uWidth) +1] = 0;
				 pBmp[3*(i + j*frameInfo.uWidth) +2] = 0;	
				 break;
			 case 2://green
				 pBmp[3*(i + j*frameInfo.uWidth) +0] = 0;
				 pBmp[3*(i + j*frameInfo.uWidth) +1] = 255;
				 pBmp[3*(i + j*frameInfo.uWidth) +2] = 0;	
				 break;
			 case 3://red
				 pBmp[3*(i + j*frameInfo.uWidth) +0] = 0;
				 pBmp[3*(i + j*frameInfo.uWidth) +1] = 0;
				 pBmp[3*(i + j*frameInfo.uWidth) +2] = 255;	
				 break;
			 default:
				 pBmp[3*(i + j*frameInfo.uWidth) +0] = 0;
				 pBmp[3*(i + j*frameInfo.uWidth) +1] = 0;
				 pBmp[3*(i + j*frameInfo.uWidth) +2] = 0;	
				 break;

			 }

		 }
	 }

 }

 void CAAVision::PaintLine(BYTE* pBmp,FrameInfo& frameInfo,int x,int y,int Length ,int lineWidth ,int type,int color)
 {
	 if (type == 1)//竖直线
	 {
		 PaintColor(pBmp,frameInfo,x,y,lineWidth,Length,type,color);
	 }
	 else if(type == 0)//水平线
	 {
		 PaintColor(pBmp,frameInfo,x,y,Length,lineWidth,type,color);
	 }	
	 else
	 {
		 //斜线
	 }
 }

 void CAAVision::PaintRetangle(BYTE* pBmp,FrameInfo& frameInfo,int x,int y,int Length ,int width,int lineWidth ,int color)

 {
	 PaintLine(pBmp,frameInfo,x,y,Length,lineWidth,0,color);//top
	 PaintLine(pBmp,frameInfo,x,y,width,lineWidth,1,color);//left
	 PaintLine(pBmp,frameInfo,x+Length,y,width,lineWidth,1,color);//right
	 PaintLine(pBmp,frameInfo,x,y+width,Length,lineWidth,0,color);//bottom


 }
void CAAVision::PaintCrossCenter(BYTE* pBmp,FrameInfo &frameInfo,int lineLength ,int lineWidth )
{
	for (int i= frameInfo.uWidth/2 -lineWidth;i<frameInfo.uWidth/2+lineWidth;i++)
	{
		for (int j= frameInfo.uHeight/2 -lineLength;j<frameInfo.uHeight/2 +lineLength;j++)
		{
			pBmp[3*(i + j*frameInfo.uWidth) +0] = 0;
			pBmp[3*(i + j*frameInfo.uWidth) +1] = 0;
			pBmp[3*(i + j*frameInfo.uWidth) +2] = 255;						
		}
	}
	for (int j= frameInfo.uHeight/2 -lineWidth;j<frameInfo.uHeight/2+lineWidth;j++)
	{
		for (int i= frameInfo.uWidth/2-lineLength;i<frameInfo.uWidth/2+lineLength;i++)
		{
			pBmp[3*(i + j*frameInfo.uWidth) +0] = 0;
			pBmp[3*(i + j*frameInfo.uWidth) +1] = 0;
			pBmp[3*(i + j*frameInfo.uWidth) +2] = 255;
		}
	}
}
void CAAVision::PaintSpecialLine(BYTE* pBmp,FrameInfo& frameInfo,int x,int y,int lineLength,int lineWidth ,int color)//画水平或者竖直线
{
	for (int i= x;i<x+lineWidth;i++)
	{
		for (int j= y ;j<y +lineLength;j++)
		{
			switch(color)
			{		
			case 1://blue
				pBmp[3*(i + j*frameInfo.uWidth) +0] = 255;
				pBmp[3*(i + j*frameInfo.uWidth) +1] = 0;
				pBmp[3*(i + j*frameInfo.uWidth) +2] = 0;	
				break;
			case 2://green
				pBmp[3*(i + j*frameInfo.uWidth) +0] = 0;
				pBmp[3*(i + j*frameInfo.uWidth) +1] = 255;
				pBmp[3*(i + j*frameInfo.uWidth) +2] = 0;	
				break;
			case 3://red
				pBmp[3*(i + j*frameInfo.uWidth) +0] = 0;
				pBmp[3*(i + j*frameInfo.uWidth) +1] = 0;
				pBmp[3*(i + j*frameInfo.uWidth) +2] = 255;	
			break;
			default:
				pBmp[3*(i + j*frameInfo.uWidth) +0] = 0;
				pBmp[3*(i + j*frameInfo.uWidth) +1] = 0;
				pBmp[3*(i + j*frameInfo.uWidth) +2] = 0;	
			break;

			}
					
		}
	}
}
void CAAVision::PaintSpecialRetangle(BYTE* pBmp,FrameInfo& frameInfo,int lineLength ,int lineWidth )
{
	int size = CFG_INSTANCE->get_cfg_int(AA_Globle_param,AA_size);
	int block = CFG_INSTANCE->get_cfg_int(AA_Globle_param,AA_block);
	double lenLarge = CFG_INSTANCE->get_cfg_double(AA_Globle_param,AA_t_mapdislength);
	double lenSmall = CFG_INSTANCE->get_cfg_double(AA_Globle_param,AA_t_mapdiswidth);
	double pixel_size = CFG_INSTANCE->get_cfg_double(AA_Globle_param,AA_pixel_size);
	double scale = CFG_INSTANCE->get_cfg_double(AA_Globle_param,AA_scale);

	//在原始图像是
	//pixel_size = pixel_size/scale;
	


	//中心的竖线
	lineLength = size/scale; //垂直方向上的长度
	lineWidth = 10;//水平反方向上的长度


	PaintSpecialLine(pBmp,frameInfo,frameInfo.uWidth/2-lineWidth/2,frameInfo.uHeight/2,lineLength,lineWidth,1);//右边线
	PaintSpecialLine(pBmp,frameInfo,frameInfo.uWidth/2-lineLength-lineWidth/2,frameInfo.uHeight/2,lineLength,lineWidth,1);//left
	PaintSpecialLine(pBmp,frameInfo,frameInfo.uWidth/2-lineLength,frameInfo.uHeight/2-lineWidth/2,lineWidth,lineLength,1);//top
	PaintSpecialLine(pBmp,frameInfo,frameInfo.uWidth/2-lineLength,frameInfo.uHeight/2+lineLength-lineWidth/2,lineWidth,lineLength,1);
	
}

void CAAVision::DecisionRectByTemplateType(int temPtype,int lengthS,int lengthB,int picwidth,int picheight,int &x,int &y,int &bigx,int &bigy)
{
	//temPtype  = 3 ;//还是以中心为顶点画图
	switch(temPtype)
	{
	case 0:
		//找小矩形左上点
		x = picwidth/2;
		y = picheight/2 ;
		//找大矩形的左上点
		bigx = picwidth/2 - lengthB + lengthS ;
		bigy = picheight/2 -lengthB + lengthS;
		break;
	case 2:
		//找小矩形左上点
		x = picwidth/2 ;
		y = picheight/2 -lengthS;
		//找大矩形的左上点
		bigx = picwidth/2 - lengthB+ lengthS;
		bigy = picheight/2 -  lengthS;
		break;
	case 3:
	default:
		//找小矩形左上点
		x = picwidth/2-lengthS;
		y = picheight/2 -lengthS;
		//找大矩形的左上点
		bigx = picwidth/2-lengthS;
		bigy = picheight/2 -lengthS;
		break;
	case 1:
		//找小矩形左上点
		x = picwidth/2-lengthS;
		y = picheight/2 ;
		//找大矩形的左上点
		bigx = picwidth/2 -  lengthS;
		bigy = picheight/2 - lengthB+ lengthS;
		break;
	}

}
int CAAVision::SetSensorTab(SensorTab& pCurrentSensor)
{
	memcpy(&CurrentSensorA,&pCurrentSensor,sizeof(SensorTab));
	strcpy_s(CurrentSensorA.name,pCurrentSensor.name);
	CurrentSensorA.ParaList = new USHORT[pCurrentSensor.ParaListSize];
	memcpy(CurrentSensorA.ParaList,pCurrentSensor.ParaList,sizeof(USHORT)*pCurrentSensor.ParaListSize);

	//其他的参数好像没有用到,后续完善


	return 0;

}
int CAAVision::SetPinDefinition()
{
	int ret = DT_ERROR_OK;
	int iDevID = m_iDevIDA;
	BYTE  pinDef[40] = {0};
	if(CurrentSensorA.port == PORT_MIPI || CurrentSensorA.port == PORT_HISPI)
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

		pinDef[0] = 16;
		pinDef[1] = 0;
		pinDef[2] = 2;
		pinDef[3] = 1;
		pinDef[4] = 3;
		pinDef[5] = 4;
		pinDef[6] = 5;
		pinDef[7] = 6;
		pinDef[8] = 7;
		pinDef[9] = 8;
		pinDef[10] = 9 ;
		pinDef[11] = 20;
		pinDef[12] = 10;
		pinDef[13] = 11;
		pinDef[14] = 12;
		pinDef[15] = 20;
		pinDef[16] = 20;
		pinDef[17] = 20;
		pinDef[18] = 20;
		pinDef[19] = 20;
		pinDef[20] = 13;
		pinDef[21] = 20;
		pinDef[22] = 14;
		pinDef[23] = 15;
		pinDef[24] = 18;
		pinDef[25] = 19;
	}
	//配置柔性接口
	ret = SetSoftPin(pinDef,iDevID);

	return ret;
}


/* 暂时废弃
//内部私有函数
BOOL CAAVision::LoadLibIniFile(pSensorTab pCurrentSensor,BYTE byChannel)
{
	
	string sfilename ;  

	if (byChannel ==0 )
	{
		//SetIniFileName(m_sFwFileFolderA);

		sfilename = m_sFwFileFolderA;
	}





	

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

	GetI2CDataFromLibFile(sfilename, pCurrentSensor,byChannel);
	if( (pCurrentSensor->width==0)       ||
		(pCurrentSensor->height==0)      ||
		(pCurrentSensor->ParaList==NULL) ||
		(pCurrentSensor->ParaListSize==0)	  )
	{
		return FALSE;
	}
	

	return TRUE;
}
*/