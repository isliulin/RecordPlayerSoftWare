// QueryRecordDBDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "RecordPlayerSoftWare.h"
#include "QueryRecordDBDlg.h"
#include "Define.h"

// CQueryRecordDBDlg �Ի���

IMPLEMENT_DYNAMIC(CQueryRecordDBDlg, CDialog)

CQueryRecordDBDlg::CQueryRecordDBDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CQueryRecordDBDlg::IDD, pParent)
	, m_StartDate(COleDateTime::GetCurrentTime())
	, m_StartTime(COleDateTime::GetCurrentTime())
	, m_EndDate(COleDateTime::GetCurrentTime())
	,m_EndTime(COleDateTime::GetCurrentTime())
{

}

CQueryRecordDBDlg::~CQueryRecordDBDlg()
{
}

void CQueryRecordDBDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_DateTimeCtrl(pDX, IDC_StartDate, m_StartDate);
	DDX_DateTimeCtrl(pDX, IDC_StartTime, m_StartTime);
	DDX_DateTimeCtrl(pDX, IDC_EndDate, m_EndDate);
	DDX_DateTimeCtrl(pDX, IDC_EndTime, m_EndTime);
}


BEGIN_MESSAGE_MAP(CQueryRecordDBDlg, CDialog)
	ON_BN_CLICKED(IDOK, &CQueryRecordDBDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// CQueryRecordDBDlg ��Ϣ�������

BOOL CQueryRecordDBDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  �ڴ���Ӷ���ĳ�ʼ��

	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
}

void CQueryRecordDBDlg::OnBnClickedOk()
{
	
	UpdateData(TRUE);

	//��ȡ��ѯʱ��
    m_stStartTime.wYear=m_StartDate.GetYear();
	m_stStartTime.wMonth=m_StartDate.GetMonth();
    m_stStartTime.wDay=m_StartDate.GetDay();

	m_stStartTime.wHour=m_StartTime.GetHour();
	m_stStartTime.wMinute=m_StartTime.GetMinute();
	m_stStartTime.wSecond=m_StartTime.GetSecond();

	m_stEndTime.wYear=m_EndDate.GetYear();
	m_stEndTime.wMonth=m_EndDate.GetMonth();
	m_stEndTime.wDay=m_EndDate.GetDay();

	m_stEndTime.wHour=m_EndTime.GetHour();
	m_stEndTime.wMinute=m_EndTime.GetMinute();
	m_stEndTime.wSecond=m_EndTime.GetSecond();

	//���ʱ��
	if ((m_stStartTime.wYear>m_stEndTime.wYear)||(m_stStartTime.wMonth>m_stEndTime.wMonth)\
		||(m_stStartTime.wDay>m_stEndTime.wDay)||(m_stStartTime.wHour>m_stEndTime.wHour)\
		||(m_stStartTime.wMinute>m_stEndTime.wMinute)||(m_stStartTime.wSecond>m_stEndTime.wSecond))
	{
		AfxMessageBox("¼������ʱ�䲻��С��¼����ʼʱ�䣡",MB_OK,MB_ICONWARNING);
		return;
	}


	OnOK();
}
