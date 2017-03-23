// RecordDevDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "RecordPlayerSoftWare.h"
#include "RecordDevDlg.h"
#include "GlobalVF.h"

// CRecordDevDlg 对话框

IMPLEMENT_DYNAMIC(CRecordDevDlg, CDialog)

CRecordDevDlg::CRecordDevDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CRecordDevDlg::IDD, pParent)
	, m_sDevName(_T("录音设备"))
	, m_nDevID(1)
	, m_dDevIP(0)
    , m_sDevIP(_T("192.168.1.101"))
	, m_nDevPort(5081)
{
    //将IP转换为DWORD
	
	m_dDevIP= ntohl(inet_addr(m_sDevIP));   
	m_bAddFlag=TRUE; //默认为添加
	
}

CRecordDevDlg::~CRecordDevDlg()
{
}

void CRecordDevDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_DevName, m_sDevName);
	DDX_Text(pDX, IDC_DevID, m_nDevID);
	DDX_IPAddress(pDX, IDC_DevIP, m_dDevIP);
	DDX_Text(pDX, IDC_DevPort, m_nDevPort);
}


BEGIN_MESSAGE_MAP(CRecordDevDlg, CDialog)
	ON_BN_CLICKED(IDOK, &CRecordDevDlg::OnBnClickedOk)
	ON_WM_CTLCOLOR()
END_MESSAGE_MAP()


// CRecordDevDlg 消息处理程序

BOOL CRecordDevDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	//初始化
	CEdit* pEdit=(CEdit*)GetDlgItem(IDC_DevID);
	if (m_bAddFlag)
	{
		
		pEdit->SetReadOnly(FALSE);

	}
	else
	{
		
		pEdit->SetReadOnly(TRUE);
	}

	m_brBrush.CreateSolidBrush(RGB(255,255,255));

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void CRecordDevDlg::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(TRUE);
    
	m_gGlobalVF.m_sDevInfo.m_sDevName=m_sDevName;
	m_gGlobalVF.m_sDevInfo.m_nDevID=m_nDevID;
	m_gGlobalVF.m_sDevInfo.m_dDevIP=m_dDevIP;
	m_gGlobalVF.m_sDevInfo.m_nDevPort=m_nDevPort;

	if(m_bAddFlag)
	{
		//添加设备写链表
		SRecordDevInfo* pDevInfo=new SRecordDevInfo;
		if (pDevInfo==NULL)
		{
			m_gGlobalVF.WriteLog("分配设备链表错误!");

			return;

		}

		pDevInfo->m_sDevName=m_sDevName;
		pDevInfo->m_nDevID=m_nDevID;
		pDevInfo->m_dDevIP=m_dDevIP;
		pDevInfo->m_nDevPort=m_nDevPort;



		if(!m_gGlobalVF.IsSameDevID(m_nDevID))
		{
			m_gGlobalVF.m_lDevInfoList.AddTail(pDevInfo);
		}
		else
		{
			AfxMessageBox("设备ID已存在！",MB_OK,MB_ICONWARNING);
			return;
		}

	}
	else{
		//修改链表
		//找到该ID的设备信息
		SRecordDevInfo* pDevInfo=m_gGlobalVF.FindDevInfo(m_nDevID);

		if (pDevInfo!=NULL)
		{

			pDevInfo->m_sDevName=m_sDevName;
			pDevInfo->m_nDevID=m_nDevID;
			pDevInfo->m_dDevIP=m_dDevIP;
			pDevInfo->m_nDevPort=m_nDevPort;

		}
		
	}
	
	
	OnOK();
}

HBRUSH CRecordDevDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);

	switch(nCtlColor)
	{
		case CTLCOLOR_DLG:
		case CTLCOLOR_STATIC:
			{

				pDC->SetBkColor(RGB(255,255,255));       //字体背景色 ，注意SetBkColor函数设的是字体的背景色，不是控件的背景色
				pDC->SetBkMode(TRANSPARENT); 
			}
		return m_brBrush;
	}
	
	return hbr;
}
