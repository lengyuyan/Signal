/***********************************************************************
CAAvision类：封装的视觉类
***********************************************************************/

#pragma once
#include <string>
#include <Windows.h>
#include "imagekit.h"

using namespace std;


/*enum VISIONSTATUS
{
	TESTOR_START =1,
	TESTOR_STOP

};*/
#define TESTOR_START				1
#define TESTOR_STOP					2

#define threadNUM					8

typedef  struct 
{
	int x;
	int y;
}AAVISIONPOINT,*PAAVISIONPOINT;

#define GRAPPITURENUM  10 //抓图时抓几次
class CAAVision
{
	
public:
	CAAVision(string name,int defineid/*,string inifilePath*/);
	virtual ~CAAVision();
	static int EnumerateAAVision(char *DeviceName[],int iDeviceNumMax,int *pDeviceNum);
	int OpenVisionDevice();
	int CloseVisionDevice();

	BOOL TripleBufferFlipA(LPBYTE *pBuf, BOOL bPrimaryBuffer);


	int InitCamera();//打开视频要初始化相机,抓图也要开视频

	int UinitCamera();//打开视频要初始化相机,抓图也要开视频
	
	int OpenVisionVideo(HWND hWnd);
	int CloseVisionVideo();

	LPBYTE GrapOnePicture();
	int SaveOnePicture(); //暂时不能实现,把图线抓取返回到dlg去保存

	int GetVisionDeviceSN(BYTE *pSN,int iBufferSize,int *pRetLen);

	int SetSensorTab(SensorTab& pCurrentSensor);

	static BOOL SetPaintFlag(BOOL flag);


	//static void PaintLine(BYTE* pBmp,FrameInfo& FrameInfo,int lineLength = 50,int lineWidth = 10);//画水平或者竖直线
	
	void PaintCrossCenter(BYTE* pBmp,FrameInfo& FrameInfo,int lineLength = 50,int lineWidth = 10);//在中心画十字交叉线
	 void PaintSpecialRetangle(BYTE* pBmp,FrameInfo& FrameInfo,int lineLength = 50,int lineWidth = 10);//
	 void PaintSpecialLine(BYTE* pBmp,FrameInfo& FrameInfo,int x,int y,int lineLength ,int lineWidth ,int color);//画水平或者竖直线,方向都增加
	//1,blue;2,green,3,red

	//type =0 水平线，type = 1竖直线 color:1,2,3 blue green red
	void PaintLine(BYTE* pBmp,FrameInfo& FrameInfo,int x,int y,int Length ,int lineWidth ,int type,int color);//
	void PaintRetangle(BYTE* pBmp,FrameInfo& FrameInfo,int x,int y,int Length ,int width,int lineWidth ,int color);//
	void DecisionRectByTemplateType(int temPtype,int smallRectsize,int bigRectsize,int piclength,int picwidth,int &x,int &y,int &bigx,int &bigy);
	static int savepic;

	//新增线程中的几个全局变量
	FrameInfo m_FrameInfoA;
	UINT AAFrameCnt;
	UINT AAPlayCnt;
	//为简单起见,这些变量先都定义为public，后续在优化
	static BOOL        m_paintflag;   //画图
	int			m_iDevIDA;		// 用户指定窗口左边设备的ID号

	int			m_iCurIDA; //定义的id
	
	int			m_iWorkStatusA; //标识已经开了视频
	BYTE		pSN[32];
	SensorTab	CurrentSensorA;
	string		m_pSensorNameA;
	BOOL  m_isTV;

	LPBYTE		m_pTripleBufferA;  //buffer between display and camera 
	LPBYTE		m_pDisplayBufferA; //display buffer
	LPBYTE		m_pCameraBufferA;  //camera buffer.
	USHORT		m_PreviewWidthA;
	USHORT		m_PreviewHeightA;
	ULONG		m_GrabSizeA;
	float		m_fMclkA;
	float		m_fAvddA;
	float		m_fDvddA;
	float		m_fDovddA;
	float		m_fAfvccA;
	float       m_vppA;
	UINT		m_uFocusPosA;




	//int			m_iCurIDA;		// 分配给窗口左边设备的ID号
	
	BOOL		m_bEnableOpenA;
	

	string		m_sFwFileFolderA;
	string		m_strUserNameA;/* 用户自定义名称 */
	
	USHORT*		m_pSensorParaA;
	USHORT*		m_pSensorSleepParaA;
	USHORT*		m_pAF_InitParaListA;
	USHORT*		m_pAF_AutoParaListA;
	USHORT*		m_pAF_FarParaListA;
	USHORT*		m_pAF_NearParaListA;
	USHORT*		m_pExposure_ParaListA;
	USHORT*		m_pGain_ParaListA;

	

	HANDLE		m_hCameraThreadEventDeadA; //thread for video 
	HANDLE		m_hDisplayThreadEventDeadA;//thread for display
	BOOL		m_bTripleBufferDirtyA;     //video update flag

	//创建8个线程
	HANDLE CameraHandleA[threadNUM] ;
	HANDLE DisplayHandleA[threadNUM] ;


	string		m_strIniPathName;
	string		m_sADInfoA;



	float		m_fFrameFpsA;
	int			m_iFrameErrA;
	int			m_iFrameCntA;

	CRITICAL_SECTION	m_csLock; 

	//Cstring m_strCurrentDeviceA;


	


	



private:
	void KillDataBuffer(BYTE byChannel = CHANNEL_A);
	int SetPinDefinition();//first set pin definition...
	BOOL SetVoltageMclk(int iDevID,float Mclk,float Avdd,float Dvdd,float Dovdd, float Afvcc, float vpp);

	void InitSomeParam();

	LPBYTE GrapOnePictureWhenVideo();

	void PaintColor(BYTE* pBmp,FrameInfo& FrameInfo,int x,int y,int width ,int height ,int type,int color);//

	//BOOL LoadLibIniFile(pSensorTab pCurrentSensor,BYTE byChannel);//先不用
};