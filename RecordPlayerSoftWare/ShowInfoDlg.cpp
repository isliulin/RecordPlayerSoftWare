// ShowInfoDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "RecordPlayerSoftWare.h"
#include "ShowInfoDlg.h"

#define DLG_BK_COLOR  RGB(255,255,255)
// CShowInfoDlg �Ի���

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


// CShowInfoDlg ��Ϣ�������

BOOL CShowInfoDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	
	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
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

	//���ı���
	switch(nCtlColor)
	{
	case CTLCOLOR_DLG:
	case CTLCOLOR_STATIC:
		{

			pDC-> SetBkColor(DLG_BK_COLOR);       //���屳��ɫ ��ע��SetBkColor�������������ı���ɫ�����ǿؼ��ı���ɫ
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
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ

	CDialog::OnClose();
}
