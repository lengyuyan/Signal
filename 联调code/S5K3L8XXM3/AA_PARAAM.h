#pragma once
#include "afxwin.h"

#include "MyStatic.h"
#include "HalconCpp.h"
using namespace HalconCpp;
#include <vector>
using namespace std;;

#include "IAVisionAlgorithm.h"

// CAA_PARAAM 对话框

class CAA_PARAAM : public CDialogEx
{
	DECLARE_DYNAMIC(CAA_PARAAM)

public:
	CAA_PARAAM(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CAA_PARAAM();

// 对话框数据
	enum { IDD = IDD_DLG_AA_PARAM };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	int bindOpencvWindow();
	virtual BOOL OnInitDialog();
	void DisplayAAglobleParam2EDit();
	void SaveAAglobleParam(bool save =0);

	void SaveAARegionParam(bool save =0);

private:	
	CEdit m_editzadjustnum;
	CEdit m_editz_adjustthr;
	CEdit   m_edtzadjustVal[10];  

	CEdit m_edituvadjustnum;
	CEdit m_edituv_adjustthr;
	CEdit m_edtuvadjustVal[10];

	//模板类型
	CEdit m_edttemplateType[8];

	CEdit m_edtpaintcenter[8];

public:

	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedAatesttype();
	CButton m_AAtesttype;
	afx_msg void OnBnClickedSigtype();
	afx_msg void OnBnClickedDoutype();

	//区域选择相关
	afx_msg void OnCbnSelchangeChannel();
	afx_msg void OnCbnSelchangeRegionallocation();
	afx_msg void OnCbnSelchangeRegionaltype();

protected:
	//先做成固定8通道的，后续改为根据实际情况来
	void InitCombChannel();
	void InitCombRegionallocation();
	void InitCombRegionaltype();

	void InitCombFunSelect();

	void UpdateCombRegional();
public:
	CMyStatic m_debugvideo;
	afx_msg void OnBnClickedGrabpic();
	long halconWinID ;//halcon窗口
	HObject m_pImage[8];

	char m_OpencvWinstr[32];
	Mat m_opImage[8];
	int RegionLocation;//记录上一次的ROI区域位置
	int ImageChannel;       //记录上一次选择的通道序号
	int RegionType;   //记录上一次选择的ROI区域类型
	int Calcpoint();
	int CalcRegionArea(vector<double>&area);
	int CalcHalconProcess(int vida,int vidb);

	int CalcRegionMTF();
	int CalcRegionRightHarryPoint();

	afx_msg void OnBnClickedReadpic1();
	afx_msg void OnCbnSelchangeFunselet();
	afx_msg void OnBnClickedCheckHalcon();
	afx_msg void OnBnClickedBtnLoadhal();
	afx_msg void OnStnDblclickDebugVideo();
};
