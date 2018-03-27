// AA_PARAAM.cpp : 实现文件
//

#include "stdafx.h"
#include "HSFoxx.h"
#include "AA_PARAAM.h"
#include "afxdialogex.h"

#include"AA_define_tool.h"
#include "Config.h"

extern  AA_Globle_Param g_AAParam;//引入全局变量
// CAA_PARAAM 对话框

IMPLEMENT_DYNAMIC(CAA_PARAAM, CDialogEx)

CAA_PARAAM::CAA_PARAAM(CWnd* pParent /*=NULL*/)
	: CDialogEx(CAA_PARAAM::IDD, pParent)
{

}

CAA_PARAAM::~CAA_PARAAM()
{
}

void CAA_PARAAM::DoDataExchange(CDataExchange* pDX)
{
	//Z值	
	DDX_Control(pDX, IDC_EDIT_ZADJUST, m_editzadjustnum);
	DDX_Control(pDX, IDC_EDIT_THRO, m_editz_adjustthr);
	DDX_Control(pDX, IDC_EDIT0_Z, m_edtzadjustVal[0]);
	DDX_Control(pDX, IDC_EDIT1_Z, m_edtzadjustVal[1]);
	DDX_Control(pDX, IDC_EDIT2_Z, m_edtzadjustVal[2]);
	DDX_Control(pDX, IDC_EDIT3_Z, m_edtzadjustVal[3]);
	DDX_Control(pDX, IDC_EDIT4_Z, m_edtzadjustVal[4]);
	DDX_Control(pDX, IDC_EDIT5_Z, m_edtzadjustVal[5]);
	DDX_Control(pDX, IDC_EDIT6_Z, m_edtzadjustVal[6]);
	DDX_Control(pDX, IDC_EDIT7_Z, m_edtzadjustVal[7]);
	DDX_Control(pDX, IDC_EDIT8_Z, m_edtzadjustVal[8]);
	DDX_Control(pDX, IDC_EDIT9_Z, m_edtzadjustVal[9]);

	//UV
	DDX_Control(pDX, IDC_EDIT_UVADJUST, m_edituvadjustnum);
	DDX_Control(pDX, IDC_EDIT_UVTHRO, m_edituv_adjustthr);
	DDX_Control(pDX, IDC_EDIT0_UV, m_edtuvadjustVal[0]);
	DDX_Control(pDX, IDC_EDIT1_UV, m_edtuvadjustVal[1]);
	DDX_Control(pDX, IDC_EDIT2_UV, m_edtuvadjustVal[2]);
	DDX_Control(pDX, IDC_EDIT3_UV, m_edtuvadjustVal[3]);
	DDX_Control(pDX, IDC_EDIT4_UV, m_edtuvadjustVal[4]);
	DDX_Control(pDX, IDC_EDIT5_UV, m_edtuvadjustVal[5]);
	DDX_Control(pDX, IDC_EDIT6_UV, m_edtuvadjustVal[6]);
	DDX_Control(pDX, IDC_EDIT7_UV, m_edtuvadjustVal[7]);
	DDX_Control(pDX, IDC_EDIT8_UV, m_edtuvadjustVal[8]);
	DDX_Control(pDX, IDC_EDIT9_UV, m_edtuvadjustVal[9]);

	//模板类型
	DDX_Control(pDX, IDC_EDIT_CHANNEL1, m_edttemplateType[0]);
	DDX_Control(pDX, IDC_EDIT_CHANNEL2, m_edttemplateType[1]);
	DDX_Control(pDX, IDC_EDIT_CHANNEL3, m_edttemplateType[2]);
	DDX_Control(pDX, IDC_EDIT_CHANNEL4, m_edttemplateType[3]);
	DDX_Control(pDX, IDC_EDIT_CHANNEL5, m_edttemplateType[4]);
	DDX_Control(pDX, IDC_EDIT_CHANNEL6, m_edttemplateType[5]);
	DDX_Control(pDX, IDC_EDIT_CHANNEL7, m_edttemplateType[6]);
	DDX_Control(pDX, IDC_EDIT_CHANNEL8, m_edttemplateType[7]);



	CDialogEx::DoDataExchange(pDX);

}


BEGIN_MESSAGE_MAP(CAA_PARAAM, CDialogEx)
	ON_BN_CLICKED(IDOK, &CAA_PARAAM::OnBnClickedOk)
	
	ON_BN_CLICKED(IDC_SIGTYPE, &CAA_PARAAM::OnBnClickedSigtype)
	ON_BN_CLICKED(IDC_DOUTYPE, &CAA_PARAAM::OnBnClickedDoutype)
END_MESSAGE_MAP()


// CAA_PARAAM 消息处理程序


BOOL CAA_PARAAM::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	//初始化z值
	CString str;
	int  adjustnum =0;
	double adjust_thr =0;
	double adjustvalue[10]={0};
	adjustnum = CFG_INSTANCE->get_cfg_int(AA_Adjustparam,AAadjust_zMaxNum);
	adjust_thr = CFG_INSTANCE->get_cfg_double(AA_Adjustparam,AAadjust_zthr);
	ReadAAAdjustZValue(adjustvalue,adjustnum);
	str.Format("%d", adjustnum );
	m_editzadjustnum.SetWindowText(str);
	str.Format("%.6lf", adjust_thr );
	m_editz_adjustthr.SetWindowText(str);

	for(int i=0;i<adjustnum;i++)
	{	
		str.Format("%.6lf", adjustvalue[i] );
		m_edtzadjustVal[i].SetWindowText(str);
	}

	//初始化uv值
	adjustnum = CFG_INSTANCE->get_cfg_int(AA_Adjustparam,AAadjust_uvMaxNum);
	adjust_thr = CFG_INSTANCE->get_cfg_double(AA_Adjustparam,AAadjust_uvthr);
	ReadAAAdjustUVValue(adjustvalue,adjustnum);
	str.Format("%d", adjustnum );
	m_edituvadjustnum.SetWindowText(str);
	str.Format("%.6lf", adjust_thr );
	m_edituv_adjustthr.SetWindowText(str);
	for(int i=0;i<adjustnum;i++)
	{	
		str.Format("%.6lf", adjustvalue[i] );
		m_edtuvadjustVal[i].SetWindowText(str);
	}


	//显示全局变量的值
	DisplayAAglobleParam2EDit();

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}



void CAA_PARAAM::OnBnClickedOk()
{
	// 保存z
	CString str;
	int adjustnum =0;
	m_editzadjustnum.GetWindowText(str);
	adjustnum = atoi(str.GetBuffer());
	CFG_INSTANCE->modify_cfg_int(AA_Adjustparam,AAadjust_zMaxNum,adjustnum);
	double adjustthr =0;
	m_editz_adjustthr.GetWindowText(str);
	 adjustthr = atof(str.GetBuffer());
	CFG_INSTANCE->modify_cfg_double(AA_Adjustparam,AAadjust_zthr,adjustthr);
	double adjustvalue[AA_zzMaxnum] ={0.0};
	for(int i=0;i<AA_zzMaxnum;i++)
	{
		m_edtzadjustVal[i].GetWindowText(str);
		adjustvalue[i] = atof(str.GetBuffer());

	}
	SaveAAAdjustZValue(adjustvalue,adjustnum);

	//uv
	m_edituvadjustnum.GetWindowText(str);
	adjustnum = atoi(str.GetBuffer());
	CFG_INSTANCE->modify_cfg_int(AA_Adjustparam,AAadjust_uvMaxNum,adjustnum);
	m_edituv_adjustthr.GetWindowText(str);
	adjustthr = atof(str.GetBuffer());
	CFG_INSTANCE->modify_cfg_double(AA_Adjustparam,AAadjust_uvthr,adjustthr);
	for(int i=0;i<AA_uvMaxnum;i++)
	{
		m_edtuvadjustVal[i].GetWindowText(str);
		adjustvalue[i] = atof(str.GetBuffer());
	}
	SaveAAAdjustUVValue(adjustvalue,adjustnum);

	//保存全局参数
	SaveAAglobleParam();

	CDialogEx::OnOK();
}
void CAA_PARAAM::DisplayAAglobleParam2EDit()
{
	CString str;
	str.Format("%.6lf", g_AAParam.focus);
	GetDlgItem(IDC_EDIT_focus)->SetWindowText(str);
	str.Format("%.6lf", g_AAParam.objdis);
	GetDlgItem(IDC_EDIT_objdis)->SetWindowText(str);
	str.Format("%.6lf", g_AAParam.pixel_size);
	GetDlgItem(IDC_EDIT_pixel_size)->SetWindowText(str);
	str.Format("%.6lf", g_AAParam.scale);
	GetDlgItem(IDC_EDIT_scale)->SetWindowText(str);
	str.Format("%d", g_AAParam.mapsize);
	GetDlgItem(IDC_EDIT_mapsize)->SetWindowText(str);
	str.Format("%d", g_AAParam.pic_lenth);
	GetDlgItem(IDC_EDIT_pic_length)->SetWindowText(str);
	str.Format("%d", g_AAParam.pic_width);
	GetDlgItem(IDC_EDIT_pic_height)->SetWindowText(str);
	str.Format("%.6lf", g_AAParam.mapdislength);
	GetDlgItem(IDC_EDIT_mapdislength)->SetWindowText(str);
	str.Format("%.6lf", g_AAParam.mapdiswidth);
	GetDlgItem(IDC_EDIT_mapdiswidth)->SetWindowText(str);
	str.Format("%d", g_AAParam.size);
	GetDlgItem(IDC_EDIT_size)->SetWindowText(str);
	str.Format("%d", g_AAParam.block);
	GetDlgItem(IDC_EDIT_block)->SetWindowText(str);
	str.Format("%d", g_AAParam.edge_size);
	GetDlgItem(IDC_EDIT_edgesize)->SetWindowText(str);
	str.Format("%d", g_AAParam.block_size);
	GetDlgItem(IDC_EDIT_blocksize)->SetWindowText(str);

	//显示模板类型

	for(int i=0;i<AA_MAXVISION;i++)
	{
		str.Format("%d", g_AAParam.temPlate[i]);
		m_edttemplateType[i].SetWindowText(str);
	}
	//说明:
	str = "定义模板:(原则:选择的中心为：0,左上点(上白下黑,左白右黑),1,右上点(上白下黑,左黑右白) 2,左下点(上黑下白,左白右黑) 3,右下点(上黑下白,左白右黑)";
	GetDlgItem(IDC_BEIZHU)->SetWindowText(str);
	
	



	//aa类型选择
	if (SINGLEAA == g_AAParam.AATYPE)
	{
		
		((CButton*)GetDlgItem(IDC_SIGTYPE))->SetCheck(TRUE); 
	}
	else
	{
		((CButton*)GetDlgItem(IDC_DOUTYPE))->SetCheck(TRUE); 
	}
	
	//双摄参数
	str.Format("%.6lf", g_AAParam.doubleAAdis);
	GetDlgItem(IDC_EDIT_DOUBLEDIS)->SetWindowText(str);
	str.Format("%.6lf", g_AAParam.doubleAAscale);
	GetDlgItem(IDC_EDIT_DOUBLESCALE)->SetWindowText(str);

	str.Format("%.6lf", g_AAParam.doubleAAmapdislength);
	GetDlgItem(IDC_EDIT_DOUBLEDISLENGTH)->SetWindowText(str);
	str.Format("%.6lf", g_AAParam.doubleAAmapdiswidth);
	GetDlgItem(IDC_EDIT_DOUBLEDISWIHTH)->SetWindowText(str);
	str.Format("%.6lf", g_AAParam.doubleAAsize);
	GetDlgItem(IDC_EDIT_DOUBLESIZE)->SetWindowText(str);
	str.Format("%.6lf", g_AAParam.doubleAApiccenterX);
	GetDlgItem(IDC_EDIT_DOUBLECENTERX)->SetWindowText(str);
	str.Format("%.6lf", g_AAParam.doubleAApiccenterY);
	GetDlgItem(IDC_EDIT_DOUBLECENTERY)->SetWindowText(str);
	str.Format("%.6lf", g_AAParam.doubleAACentroidthreshold);
	GetDlgItem(IDC_EDIT_DOUBLECENTHRES)->SetWindowText(str);



}

void CAA_PARAAM::SaveAAglobleParam(bool save)
{
	CString str;
	GetDlgItem(IDC_EDIT_focus)->GetWindowText(str);
	g_AAParam.focus = atof(str.GetBuffer());

	GetDlgItem(IDC_EDIT_objdis)->GetWindowText(str);
	g_AAParam.objdis = atof(str.GetBuffer());

	GetDlgItem(IDC_EDIT_pixel_size)->GetWindowText(str);
	g_AAParam.pixel_size = atof(str.GetBuffer());

	GetDlgItem(IDC_EDIT_scale)->GetWindowText(str);
	g_AAParam.scale = atof(str.GetBuffer());

	GetDlgItem(IDC_EDIT_mapsize)->GetWindowText(str);
	g_AAParam.mapsize= atoi(str.GetBuffer());

    GetDlgItem(IDC_EDIT_pic_length)->GetWindowText(str);
	g_AAParam.pic_lenth = atoi(str.GetBuffer());
	
	
	GetDlgItem(IDC_EDIT_pic_height)->GetWindowText(str);
	g_AAParam.pic_width = atoi(str.GetBuffer());
	g_AAParam.mapdiswidth = atof(str.GetBuffer());

	
	GetDlgItem(IDC_EDIT_mapdislength)->GetWindowText(str);
	g_AAParam.mapdislength = atof(str.GetBuffer());
	
	GetDlgItem(IDC_EDIT_mapdiswidth)->GetWindowText(str);
	g_AAParam.mapdiswidth = atof(str.GetBuffer());

	
	GetDlgItem(IDC_EDIT_size)->GetWindowText(str);
	g_AAParam.size = atoi(str.GetBuffer());
	

	GetDlgItem(IDC_EDIT_block)->GetWindowText(str);
	g_AAParam.block = atoi(str.GetBuffer());
	
	GetDlgItem(IDC_EDIT_edgesize)->GetWindowText(str);
	g_AAParam.edge_size = atoi(str.GetBuffer());
	
	GetDlgItem(IDC_EDIT_blocksize)->GetWindowText(str);
    g_AAParam.block_size = atoi(str.GetBuffer());


	for(int i=0;i<AA_MAXVISION;i++)
	{
		m_edttemplateType[i].GetWindowText(str);
		g_AAParam.temPlate[i] = atoi(str.GetBuffer());
	}




	//双摄
	GetDlgItem(IDC_EDIT_DOUBLESCALE)->GetWindowText(str);
	g_AAParam.doubleAAscale = atof(str.GetBuffer());
	GetDlgItem(IDC_EDIT_DOUBLEDIS)->GetWindowText(str);
	g_AAParam.doubleAAdis = atof(str.GetBuffer());

	GetDlgItem(IDC_EDIT_DOUBLEDISLENGTH)->GetWindowText(str);
	g_AAParam.doubleAAmapdislength = atof(str.GetBuffer());
	GetDlgItem(IDC_EDIT_DOUBLEDISWIHTH)->GetWindowText(str);
	g_AAParam.doubleAAmapdiswidth = atof(str.GetBuffer());
	GetDlgItem(IDC_EDIT_DOUBLESIZE)->GetWindowText(str);
	g_AAParam.doubleAAsize = atof(str.GetBuffer());
	GetDlgItem(IDC_EDIT_DOUBLECENTERX)->GetWindowText(str);
	g_AAParam.doubleAApiccenterX = atof(str.GetBuffer());
	GetDlgItem(IDC_EDIT_DOUBLECENTERY)->GetWindowText(str);
	g_AAParam.doubleAApiccenterY = atof(str.GetBuffer());

	GetDlgItem(IDC_EDIT_DOUBLECENTHRES)->GetWindowText(str);
	g_AAParam.doubleAACentroidthreshold = atof(str.GetBuffer());
	






	SaveAA_Globle_ParamValue(true);//保存到hash表里还要立即保存到文件

}




void CAA_PARAAM::OnBnClickedSigtype()
{
	// TODO: 在此添加控件通知处理程序代码
	g_AAParam.AATYPE = SINGLEAA;
}


void CAA_PARAAM::OnBnClickedDoutype()
{
	// TODO: 在此添加控件通知处理程序代码
	g_AAParam.AATYPE = DOUBLEAA;
}
