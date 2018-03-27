// AA_PICPARAM.cpp : 实现文件
//

#include "stdafx.h"
#include "HSFoxx.h"
#include "AA_PICPARAM.h"
#include "afxdialogex.h"

#include "AA_pic_param.h"
#include "AA_define_tool.h"

//#include "str_tool.h"

extern string g_reginalchannel[];
extern string g_picProcesstype[];
extern AA_Globle_Pic_Param g_AA_PicParam;
extern AA_Globle_Param g_AA_Param;

// AA_PICPARAM 对话框

IMPLEMENT_DYNAMIC(AA_PICPARAM, CDialogEx)

AA_PICPARAM::AA_PICPARAM(CWnd* pParent /*=NULL*/)
	: CDialogEx(AA_PICPARAM::IDD, pParent)
{

}

AA_PICPARAM::~AA_PICPARAM()
{
}

void AA_PICPARAM::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_PARAM, m_L_INPUT);
}


BEGIN_MESSAGE_MAP(AA_PICPARAM, CDialogEx)
	/*ON_BN_CLICKED(IDOK, &AA_PICPARAM::OnBnClickedOk)*/
	ON_CBN_SELCHANGE(IDC_PICCHANNEL, &AA_PICPARAM::OnCbnSelchangePicchannel)
	ON_CBN_SELCHANGE(IDC_PICPROCESSTYPE, &AA_PICPARAM::OnCbnSelchangePicprocesstype)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_PARAM, &AA_PICPARAM::OnNMDblclkListParam)
	ON_BN_CLICKED(IDC_SAVEPICPARAM, &AA_PICPARAM::OnBnClickedSavepicparam)
END_MESSAGE_MAP()


// AA_PICPARAM 消息处理程序


BOOL AA_PICPARAM::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	InitCombChannel();
	InitPicType();
	InitInputList();





	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}


//void AA_PICPARAM::OnBnClickedOk()
//{
//	// TODO: 在此添加控件通知处理程序代码
//
//	//CDialogEx::OnOK();
//}

void AA_PICPARAM::UpdateCombProcessParam()
{
	int nChannelCur = ((CComboBox*)GetDlgItem(IDC_PICCHANNEL))->GetCurSel();
	ZERO_CHK(nChannelCur >= 0);
	int nTypeCur = ((CComboBox*)GetDlgItem(IDC_PICPROCESSTYPE))->GetCurSel();
	ZERO_CHK(nTypeCur >= 0);

	InitInputList();



}

void AA_PICPARAM::OnCbnSelchangePicchannel()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateCombProcessParam();
}
void AA_PICPARAM::InitCombChannel()
{
	CComboBox* pComb = (CComboBox*)GetDlgItem(IDC_PICCHANNEL);
	pComb->ResetContent();
	//pComb->InsertString(0, "");

	for (int i= 0;i<AA_MAXVISION;i++)
	{
		pComb->InsertString(i, g_reginalchannel[i].c_str());
	}

	pComb->SetCurSel(0);
}
void AA_PICPARAM::InitPicType()
{
	CComboBox* pComb = (CComboBox*)GetDlgItem(IDC_PICPROCESSTYPE);
	pComb->ResetContent();
	int itemnum = 0;
	for (int i =0;i<100;i++)
	{
		if (g_picProcesstype[i] == "更多")
		{
			break;
		}
		itemnum++;
	}

	for (int i= 0;i<itemnum;i++)
	{
		pComb->InsertString(i, g_picProcesstype[i].c_str());
	}

	pComb->SetCurSel(0);
}


void AA_PICPARAM::OnCbnSelchangePicprocesstype()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateCombProcessParam();

}

void  AA_PICPARAM::InitInputList()
{
	DWORD listStyle = m_L_INPUT.GetExStyle();
	listStyle = listStyle|LVS_EX_GRIDLINES;
	listStyle = listStyle|LVS_EX_FULLROWSELECT;
	m_L_INPUT.SetExtendedStyle(listStyle);

	RECT ListRect;
	int margin = 30;
	m_L_INPUT.GetWindowRect(&ListRect);
	m_L_INPUT.InsertColumn(0,"参数类型");
	m_L_INPUT.SetColumnWidth(0,(ListRect.right-ListRect.left)/2);
	m_L_INPUT.InsertColumn(1,"参数值");
	m_L_INPUT.SetColumnWidth(1,(ListRect.right-ListRect.left)/2);
	//m_L_INPUT.InsertColumn(2,"参数值");
	//m_L_INPUT.SetColumnWidth(2,(ListRect.right-ListRect.left)/3);

	//显示数据
	char szText[256] = {0};
	m_L_INPUT.DeleteAllItems();
	m_SelMoveRow = -1;
	m_SelMoveCol = -1;

	CString str;
	int nCur = ((CComboBox*)GetDlgItem(IDC_PICCHANNEL))->GetCurSel();
	if (nCur <0)
	{
		MessageBox("没有选择!");
		return;
	}
	//((CComboBox*)GetDlgItem(IDC_PICCHANNEL))->GetLBText(nCur,str);
	//if (!str.Compare("计算区域特征点"))
	//{
	//	//Calcpoint();
	//}
	int ntypeCur = ((CComboBox*)GetDlgItem(IDC_PICPROCESSTYPE))->GetCurSel();
	if (nCur <0)
	{
		MessageBox("没有选择类型!");
		return;
	}
	((CComboBox*)GetDlgItem(IDC_PICPROCESSTYPE))->GetLBText(ntypeCur,str);
	if (!str.Compare("SelectShape"))
	{
		//Calcpoint();
		sprintf_s(szText,"%s","Features");
		m_L_INPUT.InsertItem(0,szText);
		//m_L_INPUT.SetItemText(i,0,szText);
		memset(szText,0,sizeof(szText));
		sprintf_s(szText,"%s",g_AA_PicParam.selectShape[nCur].Features);
		m_L_INPUT.SetItemText(0,1,szText);

		sprintf_s(szText,"%s","Operation");
		m_L_INPUT.InsertItem(1,szText);
		memset(szText,0,sizeof(szText));
		sprintf_s(szText,"%s",g_AA_PicParam.selectShape[nCur].Operation);
		m_L_INPUT.SetItemText(1,1,szText);

		sprintf_s(szText,"%s","min");
		m_L_INPUT.InsertItem(2,szText);
		memset(szText,0,sizeof(szText));
		sprintf_s(szText,"%.1f",g_AA_PicParam.selectShape[nCur].min);
		m_L_INPUT.SetItemText(2,1,szText);

		sprintf_s(szText,"%s","max");
		m_L_INPUT.InsertItem(3,szText);
		memset(szText,0,sizeof(szText));
		sprintf_s(szText,"%.1f",g_AA_PicParam.selectShape[nCur].max);
		m_L_INPUT.SetItemText(3,1,szText);



	}
	else if (!str.Compare("BinaryThreshold"))
	{
		sprintf_s(szText,"%s","Method");
		m_L_INPUT.InsertItem(0,szText);
		memset(szText,0,sizeof(szText));
		sprintf_s(szText,"%s",g_AA_PicParam.binarythreshold[nCur].Method);
		m_L_INPUT.SetItemText(0,1,szText);

		sprintf_s(szText,"%s","LightDark");
		m_L_INPUT.InsertItem(1,szText);
		memset(szText,0,sizeof(szText));
		sprintf_s(szText,"%s",g_AA_PicParam.binarythreshold[nCur].LightDark);
		m_L_INPUT.SetItemText(1,1,szText);
	}
	else if(!str.Compare("ScaleImage"))
	{
		sprintf_s(szText,"%s","Mult");
		m_L_INPUT.InsertItem(0,szText);
		memset(szText,0,sizeof(szText));
		sprintf_s(szText,"%lf",g_AA_PicParam.scaleImage[nCur].mul);
		m_L_INPUT.SetItemText(0,1,szText);

		sprintf_s(szText,"%s","Add");
		m_L_INPUT.InsertItem(1,szText);
		memset(szText,0,sizeof(szText));
		sprintf_s(szText,"%lf",g_AA_PicParam.scaleImage[nCur].add);
		m_L_INPUT.SetItemText(1,1,szText);
		
	}



	pCommonEdit = NULL;

}


void AA_PICPARAM::OnNMDblclkListParam(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);

	m_SelMoveRow = pNMItemActivate->iItem;
	m_SelMoveCol = pNMItemActivate->iSubItem;
	CRect SubRect;
	//编辑对应的子项内容
	if (m_SelMoveRow>=0&&m_SelMoveCol>0)  //
	{
		//m_MapList.GetColumnWidth(ColIndex)  ;
		m_L_INPUT.GetSubItemRect(m_SelMoveRow,m_SelMoveCol,0,SubRect);
		if (pCommonEdit)
		{
			delete	pCommonEdit;
			pCommonEdit = NULL;
		}

		pCommonEdit = new CEdit; //不按回车会有内存泄漏
		pCommonEdit->Create(WS_CHILD|WS_VISIBLE|WS_BORDER,SubRect,&m_L_INPUT,MOVEDATAEDITID);
		pCommonEdit->SetWindowText(m_L_INPUT.GetItemText(m_SelMoveRow,m_SelMoveCol));

	}

	*pResult = 0;

}


void AA_PICPARAM::OnOK()
{
	// TODO: 在此添加专用代码和/或调用基类

	CString str;
	if (NULL == pCommonEdit)
	{
		return;
	}
	pCommonEdit->GetWindowText(str);
	m_L_INPUT.SetItemText(m_SelMoveRow,m_SelMoveCol,str);

	delete	pCommonEdit;
	pCommonEdit = NULL;
}


void AA_PICPARAM::OnBnClickedSavepicparam()
{
	CString str;
	int count =0;
	char tempchar[32] ={0};
	double dtemp =0.0;
	count = m_L_INPUT.GetItemCount();


	char szText[256] = {0};
	int nCur = ((CComboBox*)GetDlgItem(IDC_PICCHANNEL))->GetCurSel();
	if (nCur <0)
	{
		MessageBox("没有选择!");
		return;
	}
	//((CComboBox*)GetDlgItem(IDC_PICCHANNEL))->GetLBText(nCur,str);
	//if (!str.Compare("计算区域特征点"))
	//{
	//	//Calcpoint();
	//}
	int ntypeCur = ((CComboBox*)GetDlgItem(IDC_PICPROCESSTYPE))->GetCurSel();
	if (nCur <0)
	{
		MessageBox("没有选择类型!");
		return;
	}
	((CComboBox*)GetDlgItem(IDC_PICPROCESSTYPE))->GetLBText(ntypeCur,str);
	if (!str.Compare("SelectShape"))
	{
		if (count < 4 )
		{
			MessageBox("参数项错误");
		}

		//str = m_L_INPUT.GetItemText(0, 1);//获取第2列
		strcpy_s(tempchar, m_L_INPUT.GetItemText(0, 1));
		strcpy_s(g_AA_PicParam.selectShape[nCur].Features,tempchar);

		strcpy_s(tempchar, m_L_INPUT.GetItemText(1, 1));
		/*g_AA_PicParam.selectShape[nCur].Operation = tempchar;*/
		strcpy_s(g_AA_PicParam.selectShape[nCur].Operation,tempchar);

		strcpy_s(tempchar, m_L_INPUT.GetItemText(2, 1));
		//int ty = get_str_type(tempchar);
		//if (2 != ty && 1 != ty)
		//{
		//	AfxMessageBox("min参数不是数字!");
		//	return;
		//}
		dtemp = atof(tempchar);
		g_AA_PicParam.selectShape[nCur].min = dtemp;

		strcpy_s(tempchar, m_L_INPUT.GetItemText(3, 1));
		//ty = get_str_type(tempchar);
		//if (2 != ty && 1 != ty)
		//{
		//	AfxMessageBox("max参数不是数字!");
		//	return;
		//}
		dtemp = atof(tempchar);
		g_AA_PicParam.selectShape[nCur].max = dtemp;
	}
	else if (!str.Compare("BinaryThreshold"))
	{
		strcpy_s(tempchar, m_L_INPUT.GetItemText(0, 1));
		strcpy_s(g_AA_PicParam.binarythreshold[nCur].Method,tempchar);
		strcpy_s(tempchar, m_L_INPUT.GetItemText(1, 1));
		strcpy_s(g_AA_PicParam.binarythreshold[nCur].LightDark,tempchar);
	}
	else if (!str.Compare("ScaleImage"))
	{
		strcpy_s(tempchar, m_L_INPUT.GetItemText(0, 1));
		dtemp = atof(tempchar);
		g_AA_PicParam.scaleImage[nCur].mul = dtemp;
		strcpy_s(tempchar, m_L_INPUT.GetItemText(1, 1));
		dtemp = atof(tempchar);
		g_AA_PicParam.scaleImage[nCur].add = dtemp;
	}

	//保存全局函数到配置内存
	SaveAA_Globle_Pic_ParamValue(false);
}
