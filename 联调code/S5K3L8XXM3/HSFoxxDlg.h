// HSFoxxDlg.h : header file
//

#if !defined(AFX_HSFOXXDLG_H__045BC92C_F0AA_438B_B102_C789FAC4F43D__INCLUDED_)
#define AFX_HSFOXXDLG_H__045BC92C_F0AA_438B_B102_C789FAC4F43D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
//20141203 added..
//if don't need display ,please use this...
//#define _ONLY_GRAB_IMAGE_

#include "dtccm2.h" //dtccm2 lib header files..
#include "imagekit.h" //
#include "afxwin.h"
#include <opencv2\opencv.hpp>
#include <HalconCpp.h>
#include <iostream>
#include <string>
#include<map>
#include "afxcmn.h"
#include "MyStatic.h" 
#include "AA_define_tool.h"
#include "devnetwork.h"
using namespace cv;
using namespace std;
using namespace HalconCpp;
enum RUNMODE //老的
{
	RUNMODE_PLAY=0,
	RUNMODE_PAUSE,
	RUNMODE_STOP,
};

enum NEWRUNMODE
{
	TESTOR_START=1,
	TESTOR_STOP,
};

#define  AA_MAXVISION 8

enum EM_PIROBOT_MVTYPE
{
	EMOVE_CP = 0,       //连续运动
	EMOVE_LONG_STEP,    //长距离
	EMOVE_MEDIU_STEP,   //中等距离
	EMOVE_SHORT_STEP,   //短距离
};

#define PI_NUM 8
#define PI_MAX_AXIS 6


#define DAAROBOTPROCESSNUM 4


/////////////////////////////////////////////////////////////////////////////
// CHSFoxxDlg dialog
class CAARobot ;//申明一个类
class CAAVision ;
struct AANotifyData;

#define MAINWINDOW_INSTANCE CHSFoxxDlg::GetInstance()

class CHSFoxxDlg :  public CDialog //public CDialogEx
{
// Construction
private:
	void SetMoveType(int mvType);   //设置运动模式 m_iMoveType
	int             m_iMoveType;        //当前运动模式 EM_STATION_MVTYPE
	CEdit   m_edtStep[PI_MAX_AXIS];  //步进距离控件
	double  m_stepVal[PI_MAX_AXIS];  //步进值
	CButton m_btMoveL[PI_MAX_AXIS];  //负向移动轴控件
	CButton m_btMoveR[PI_MAX_AXIS];  //正向移动
	double             m_posVal[PI_MAX_AXIS];        //cur pos
	CEdit   m_edtVal[PI_MAX_AXIS];   //当前位置坐标
	void MoveOffset(int iAxis, int iFlag = 1);        //1正向 -1 反向

	void ZoomPicture(int index );
	void InitZoomPicture(int index );
	void ChangeVideoWind(int index );

	bool m_dlgExit;

	int AARobotProcessParam[DAAROBOTPROCESSNUM*2];//处理线程参数,暂时定义为整形，只需要传递视觉通道索引，一个功能号对应两个视觉索引
	AANotifyResult AARobotProcessResult[DAAROBOTPROCESSNUM];//处理结果

	HANDLE AARobotWakeResult[DAAROBOTPROCESSNUM];//等待STARTAA 处理结果事件
	HANDLE dlgAARobotProcessEvent[DAAROBOTPROCESSNUM];//处理事件
	HANDLE AARobotProcesshandle[DAAROBOTPROCESSNUM];//处理线程句柄

	HANDLE AARobotResetAAWakeResult[DAAROBOTPROCESSNUM];//复位结果事件
	int AARobotResetProcessResult[DAAROBOTPROCESSNUM];//复位处理结果

	map<int,string >robotIDmap;//装配置文件中的ip与序号的

	//控制picture 的缩放
	bool m_pictureFlag[8];

	//双摄 同步涉及到的
	CRITICAL_SECTION m_csRoot12;
	CRITICAL_SECTION m_csRoot34;
	//HANDLE m_eventRobot12;
	//HANDLE m_eventRobot34;
	volatile int m_intRobot12x;
	volatile int m_intRobot34x;
	


public:
	HWND  GetHWndByVisionId(int visionID);
	int GetRobotIDByIndex(string strindex);
	int GetVisionIDByIndex(int index,string& cameraName);
	string GetVisionUserNameByIndex(int strindex);
	static int WINAPI AARobotProcess1(void* param);
	static int WINAPI AARobotProcess2(void* param);
	static int WINAPI AARobotProcess3(void* param);
	static int WINAPI AARobotProcess4(void* param);
	static vector<AANotifyResult> WINAPI WorkNotify(int evt, AANotifyData data, void* lparam);
	LRESULT OnMsgNotify(WPARAM wParam, LPARAM lParam);//
    afx_msg LRESULT OnMsgTimer(WPARAM wParam, LPARAM lParam);
	BOOL bGetI2CDataFromLibFile(CString filename, pSensorTab pSensor);
	BOOL bLoadLibIniFile(CString sfilename);
	BOOL LoadLibIniFile(pSensorTab pCurrentSensor,string sfilename);//为视觉类提供的
	BOOL TripleBufferFlip(LPBYTE *pBuf, BOOL bPrimaryBuffer);
	void GetADValue();
	int DTOS_Test();
	void KillDataBuffer();
	BOOL bCloseCamera();

	BOOL CloseAAVision();//用到的

	BOOL bOpenDevice();

	int EnumerateDothinkeyDevice();
	BOOL	m_isTV;

	USHORT*    m_pSensorPara;
	USHORT*    m_pSensorSleepPara;

	//20130823 added..
	USHORT*		m_pAF_InitParaList;
	USHORT*		m_pAF_AutoParaList;
	USHORT*		m_pAF_FarParaList;
	USHORT*		m_pAF_NearParaList;
	USHORT*		m_pExposure_ParaList;
	USHORT*		m_pGain_ParaList;

///////////////////////////////////////////////////////////////////////
	BYTE m_RunMode;
	USHORT m_PreviewWidth;
	USHORT m_PreviewHeight;
	LPBYTE     m_pTripleBuffer;  //buffer between display and camera 
 	LPBYTE     m_pDisplayBuffer; //display buffer
	LPBYTE     m_pCameraBuffer;  //camera buffer.
    HANDLE    m_hCameraThreadEventDead; //thread for video 
    HANDLE    m_hDisplayThreadEventDead;//thread for display
	BOOL      m_bTripleBufferDirty;     //video update flag

	ULONG m_GrabSize;
	

	CRITICAL_SECTION m_csLock;                //use for display update data

	CMyStatic m_wndVideo[AA_MAXVISION];
	SensorTab CurrentSensor;   ///作为各个视觉通道的中转站

	int m_nVisionDevID;
	int m_roi_state;

///////////////////////////////////////////////////////////////////////
	CHSFoxxDlg(CWnd* pParent = NULL);	// standard constructor
	~CHSFoxxDlg();

	enum { IDD = IDD_HSFOXX_DIALOG };

	int m_nPiDevID;//当前机器人id

public:
	virtual BOOL DestroyWindow();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnPlay();
	afx_msg void OnLoad();
	afx_msg void OnOseasy();
	afx_msg void OnDestroy();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	DECLARE_MESSAGE_MAP()
public:
	CEdit m_editMsg;
	afx_msg void OnBnClickedSavepicture();
	LPBYTE AAVisionGrabPicture(int visionID);
	BOOL bSaveBmpFile(CString sfilename, BYTE* pBuffer, UINT width, UINT height);
	float m_fAvdd;
	float m_fDovdd;
	float m_fDvdd;
	float m_fAfvcc;
	float m_fVpp;
	float m_fMclk;
	UINT m_FocusPos;
	UINT m_FocusPosTemp;
	afx_msg void OnBnClickedSetpower();
	afx_msg void OnBnClickedSetmclk();
	afx_msg void OnBnClickedTestRoi();

	afx_msg void OnBnClickedEnum();
	afx_msg void OnBnClickedFocusPos();

	afx_msg void OnBnClickedStartaa();

	int StartAA(int robotid,int visonid);
	int StartDoubleAA(int robotid,int visonidfixID,int visonidmovID);
	int StartDoubleAAByOpenCV(int robotid,int visonidfixID,int visonidmovID);

	AANotifyResult  NewStartDoubleAA(int vaid,int vbid);


	int GoInitPosRobot(int robotid,const char* axisstring,int posIndex,double* pos = NULL);
	
	

	void SetInitPosRobot(int robotid,const char* axisstring,int posIndex,double* pos );

	int AdjustZvalue(int robotID,int cameraID);
	int AdjustUVvalue(int robotID,int cameraID);
	int AdjustUVvalue2(int robotID,int cameraID);
	int FiveAdjustvalue(int robotID,int cameraID);
	int AdjustXYvalue(int robotID,int cameraID);

	//下面几个都是读图测试,可以合并为一个
	int AdjustZvalueReadPic(int robotID,int cameraID);
	int AdjustUVvalueReadPic(int robotID,int cameraID);
	int FiveAdjustvalueReadPic(int robotID,int cameraID);
	int AdjustXYvalueReadPic(int robotID,int cameraID);

	//双摄读图测试
	int DobuleAATestReadPic(int robotID,int cameraID);
	//图片尺寸
	int pictureCols;
	int pictureRows;
	LPBYTE GetPicture(int caramID);
	BOOL GetPictureMat(Mat * PictureMat,int caramID);
	BOOL GetPictureHObject(HObject &PictureHobject,int caramID);
	BOOL ReadPictureMat(Mat * PictureMat,string filename, int caramID);//先保留caramID,可能相机采集的图像都不一样
	BOOL SavePicture(CString sfilename,int cameraID);

	map<string ,CAARobot*>mapRobot;//ip 和机器人,先public这个map，后续加上 get set
	//后续加上锁,可能多线程操作map
	map<string ,CAAVision*>mapVision; //名字跟视觉对象
	map<string ,CAAVision*>& GetVisionMap();
	static CHSFoxxDlg* GetInstance();
	CComboBox m_KitListCtrl;
	CComboBox m_PiList;
	CString m_DevName[AA_MAXVISION];
	CButton m_playvision[AA_MAXVISION];

	CButton m_spincenter;

	//
	int GetSensorRegValue(int visionID,int type);//获取值
	int SetSensorRegValue(int visionID,int type,USHORT value);//设置

	CAAVision* FindVisionByID(int visionID);
	CAAVision* FindVisionInMap(string username); //定义的视觉是否在map中，先考虑一开始就插好usb接口,不中途插拔
	string FindVisionUsernameByID(int visionID);
	int FindBinderIDByID(int visionID);//通过id找到双摄绑定的另外一个视觉id

	//8幅图，每幅图片保存5个ROI区域的中心坐标
	Point coordinate[AA_MAXVISION][AA_VISION_RETAGLELOC];
	//获取一组指定通道中的ROI矩形的特征区域的中心坐标,每个通道5个坐标
	void CalcCenterPoint(int vaid, int vbid, HObject &pirtureA, HObject &pictureB, int graymin,int graymax,int areamin,int areamax);

	//机器人
	CAARobot* FindRobotByID(int robotID);
	int AdjustSpinCenter(int robotID,int whichaxis,double value);

	afx_msg void OnClickedGoHome();
	
	afx_msg void OnEnChangeInitY();
	
	afx_msg void OnBnClickedEnumnetdevice();
	afx_msg void OnBnClickedConnect();
	afx_msg void OnBnClickedLongdis();
	afx_msg void OnBnClickedMiddis();
	afx_msg void OnBnClickedShortdis();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnBnClickedCurposition();
	afx_msg LRESULT OnDisplayMsg(WPARAM wP, LPARAM lP);

	afx_msg void OnBnClickedBtnAaparam();
	afx_msg void OnCbnSelchangePitlist();
	int ConnectPIRobot(string &str);
	int IsconnectedInMapRobot(string ip, int port);//迅速找机器人是否连接,无需遍历 1,已经连接；0,没有连接，-1,没有在map中
	int FindRobotIDByIp(string ip);
	int FindRobotIndexByID(int id);
	afx_msg void OnBnClickedPlayVision0();
	afx_msg void OnBnClickedPlayVision1();
	afx_msg void OnBnClickedPlayVision2();
	afx_msg void OnBnClickedPlayVision3();
	afx_msg void OnBnClickedPlayVision4();
	afx_msg void OnBnClickedPlayVision5();
	afx_msg void OnBnClickedPlayVision6();
	afx_msg void OnBnClickedPlayVision7();

	int OpenAAVision(string visionName,CButton &bt);
	int __OpenAAVision(string visionName,CButton &bt);
	
	
	afx_msg void OnStnClickedVideo0();
	afx_msg void OnStnClickedVideo1();
	afx_msg void OnStnClickedVideo2();
	afx_msg void OnStnClickedVideo3();
	afx_msg void OnStnClickedVideo4();
	afx_msg void OnStnClickedVideo5();
	afx_msg void OnStnClickedVideo6();
	afx_msg void OnStnClickedVideo7();
	afx_msg void OnBnClickedConnectclose();
	
	afx_msg void OnBnClickedPicturecenter();
	afx_msg void OnBnClickedReadpicTest();
	afx_msg void OnBnClickedPiHome();
	afx_msg void OnBnClickedSetinitpos();
	afx_msg void OnBnClickedPiHome1();
	afx_msg void OnBnClickedSetinitpos1();
	afx_msg void OnClose();
	afx_msg void OnBnClickedButton1();
	CSliderCtrl m_grain;
	CSliderCtrl m_exp;
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnBnClickedSpinCenter();

	afx_msg void OnThemechangedVideo0(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedDrawcenterpoint();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_HSFOXXDLG_H__045BC92C_F0AA_438B_B102_C789FAC4F43D__INCLUDED_)
