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
//#include <iostream>
#include <string>
#include<map>
using namespace cv;
using namespace std;
enum RUNMODE
{
	RUNMODE_PLAY=0,
	RUNMODE_PAUSE,
	RUNMODE_STOP,
};

enum EM_PIROBOT_MVTYPE
{
	EMOVE_CP = 0,       //连续运动
	EMOVE_LONG_STEP,    //长距离
	EMOVE_MEDIU_STEP,   //中等距离
	EMOVE_SHORT_STEP,   //短距离
};

#define PI_NUM 8
#define PI_MAX_AXIS 6


#define AAROBOTPROCESSNUM 4


/////////////////////////////////////////////////////////////////////////////
// CHSFoxxDlg dialog
class CAARobot ;//申明一个类
struct AANotifyData;

class CHSFoxxDlg : public CDialog
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

	bool m_dlgExit;
	int AARobotProcessResult[AAROBOTPROCESSNUM];//处理结果
	HANDLE AARobotWakeResult[AAROBOTPROCESSNUM];//处理事件
	HANDLE AARobotProcessEvent[AAROBOTPROCESSNUM];//处理事件
	HANDLE AARobotProcesshandle[AAROBOTPROCESSNUM];//处理线程句柄

	map<int,string >robotIDmap;//装配置文件中的ip与序号的

public:

	static int WINAPI AARobotProcess1(void* param);
	static int WINAPI AARobotProcess2(void* param);
	static int WINAPI AARobotProcess3(void* param);
	static int WINAPI AARobotProcess4(void* param);
	static vector<int> WINAPI WorkNotify(int evt, AANotifyData data, void* lparam);

	LRESULT OnMsgNotify(WPARAM wParam, LPARAM lParam);//

    afx_msg LRESULT OnMsgTimer(WPARAM wParam, LPARAM lParam);



	BOOL bGetI2CDataFromLibFile(CString filename, pSensorTab pSensor);
	BOOL bLoadLibIniFile(CString sfilename);
	BOOL TripleBufferFlip(LPBYTE *pBuf, BOOL bPrimaryBuffer);
	void GetADValue();
	int DTOS_Test();
	void KillDataBuffer();
	BOOL bCloseCamera();
	BOOL bOpenCamera();
	BOOL bOpenDevice();

	int EnumerateDothinkeyDevice();


///////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////
	BOOL	m_isTV;
	char*      m_pSensorName;
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

	SensorTab CurrentSensor;

	CRITICAL_SECTION m_csLock;                //use for display update data

	CStatic m_wndVideo;
	int m_nDevID;
	int m_roi_state;

///////////////////////////////////////////////////////////////////////
	CHSFoxxDlg(CWnd* pParent = NULL);	// standard constructor
	~CHSFoxxDlg();

	enum { IDD = IDD_HSFOXX_DIALOG };
	CListBox	m_show;

	
	//CString m_PInetName[PI_NUM];
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
	CString m_sInfo;
	CEdit m_editMsg;
	afx_msg void OnBnClickedSavepicture();
	BOOL bSaveBmpFile(CString sfilename, BYTE* pBuffer, UINT width, UINT height);
	float m_fAvdd;
	float m_fDovdd;
	float m_fDvdd;
	float m_fAfvcc;
	float m_fVpp;
	float m_fMclk;
	afx_msg void OnBnClickedSetpower();
	afx_msg void OnBnClickedSetmclk();
	afx_msg void OnBnClickedTestRoi();
	CComboBox m_KitListCtrl;
	CString m_DevName[8];
	afx_msg void OnBnClickedEnum();
	afx_msg void OnBnClickedFocusPos();
	UINT m_FocusPos;
	UINT m_FocusPosTemp;
	afx_msg void OnBnClickedStartaa();

	int StartAA(int robotid,int visonid);

	int AdjustZvalue(int robotID,int cameraID);
	int AdjustUVvalue(int robotID,int cameraID);
	int FiveAdjustvalue(int robotID,int cameraID);
	BOOL SavePicture(CString sfilename);
	afx_msg void OnClickedInitialDevice();

	map<string ,CAARobot*>mapRobot;//ip 和机器人,先public这个map，后续加上 get set
	//后续加上锁,可能多线程操作map
	//map<string ,CAAvision*>mapVision; //名字跟视觉对象

	afx_msg void OnClickedGoHome();

	

	int m_ComInitResult; //连接值
	void DeleteFolder(CString sPath);
//	Mat GetCurrentPictureMat(void);
	BOOL GetPictureMat(Mat * PictureMat,int caramID);
	afx_msg void OnEnChangeInitY();
	CComboBox m_PiList;
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
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_HSFOXXDLG_H__045BC92C_F0AA_438B_B102_C789FAC4F43D__INCLUDED_)
