// ShowInfoDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "RecordPlayerSoftWare.h"
#include "ShowInfoDlg.h"

#define DLG_BK_COLOR  RGB(255,255,255)
// CShowInfoDlg 对话框

IMPLEMENT_DYNAMIC(CShowInfoDlg, CDialog)

CShowInfoDlg::CShowInfoDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CShowInfoDlg::IDD, pParent)
{


	m_brBrush.m_hObject=NULL;
}

CShowInfoDlg::~CShowInfoDlg()
{
	m_brBrush.DeleteObject();
}

void CShowInfoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//DDX_Control(pDX, IDC_InfoText, m_ShowInfoText);
	
}


BEGIN_MESSAGE_MAP(CShowInfoDlg, CDialog)
	ON_WM_CTLCOLOR()
	ON_BN_CLICKED(IDOK, &CShowInfoDlg::OnBnClickedOk)
	ON_WM_CLOSE()
END_MESSAGE_MAP()


// CShowInfoDlg 消息处理程序

BOOL CShowInfoDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	
	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

HBRUSH CShowInfoDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);

	if (m_brBrush.m_hObject!=NULL)
	{
		m_brBrush.DeleteObject();
		
	}
	else
	{
        m_brBrush.CreateSolidBrush(RGB(255,255,255));
	}

	//更改背景
	switch(nCtlColor)
	{
	case CTLCOLOR_DLG:
	case CTLCOLOR_STATIC:
		{

			pDC-> SetBkColor(DLG_BK_COLOR);       //字体背景色 ，注意SetBkColor函数设的是字体的背景色，不是控件的背景色
			pDC->SetBkMode(TRANSPARENT); 
		}

		return m_brBrush;
	}


	return hbr;
}

void CShowInfoDlg::OnBnClickedOk()
{
	
	CDialog::OnOK();
}

void CShowInfoDlg::OnCloseDlg()
{

	CDialog::OnOK();
}
void CShowInfoDlg::OnClose()
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	CDialog::OnClose();
}
