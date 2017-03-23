// QueryRecordDBDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "RecordPlayerSoftWare.h"
#include "QueryRecordDBDlg.h"
#include "Define.h"

// CQueryRecordDBDlg 对话框

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


// CQueryRecordDBDlg 消息处理程序

BOOL CQueryRecordDBDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  在此添加额外的初始化

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void CQueryRecordDBDlg::OnBnClickedOk()
{
	
	UpdateData(TRUE);

	//获取查询时间
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

	//检查时间
	if ((m_stStartTime.wYear>m_stEndTime.wYear)||(m_stStartTime.wMonth>m_stEndTime.wMonth)\
		||(m_stStartTime.wDay>m_stEndTime.wDay)||(m_stStartTime.wHour>m_stEndTime.wHour)\
		||(m_stStartTime.wMinute>m_stEndTime.wMinute)||(m_stStartTime.wSecond>m_stEndTime.wSecond))
	{
		AfxMessageBox("录音结束时间不能小于录音起始时间！",MB_OK,MB_ICONWARNING);
		return;
	}


	OnOK();
}
