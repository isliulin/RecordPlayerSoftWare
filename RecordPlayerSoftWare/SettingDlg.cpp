// SettingDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "RecordPlayerSoftWare.h"
#include "SettingDlg.h"
#include "FileDialogST.h"
#include "Define.h"
// CSettingDlg 对话框

IMPLEMENT_DYNAMIC(CSettingDlg, CDialog)

CSettingDlg::CSettingDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSettingDlg::IDD, pParent)
	, m_FileSavePath(_T(""))
	, m_nServerPort(6788)
{

}

CSettingDlg::~CSettingDlg()
{
}

void CSettingDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_FileSavePath, m_FileSavePath);
	DDX_Control(pDX, IDC_IPADDRESS1, m_IPAddrCtrl);
	DDX_Text(pDX, IDC_EDIT2, m_nServerPort);
}


BEGIN_MESSAGE_MAP(CSettingDlg, CDialog)
	ON_BN_CLICKED(IDOK, &CSettingDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDC_FileOpenBtn, &CSettingDlg::OnBnClickedFileopenbtn)
END_MESSAGE_MAP()


// CSettingDlg 消息处理程序

BOOL CSettingDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	//读取配置文件
	//m_gGlobalVF.ReadConfigFile();

	//设置
	InitConfigGUI();
    
	m_gGlobalVF.WriteLog("打开设置对话框");

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void CSettingDlg::InitConfigGUI()
{

	//设置默认的存储路径
	if (m_gGlobalVF.m_FileSavePath==_T(""))
	{
		//采用自动的存储路径
		CString appPath=GetApplicationPath();

		m_FileSavePath=appPath+"RecordFile\\";

		//生成文件夹
		if (!::PathIsDirectory(m_FileSavePath))
		{
			//创建文件夹
			if (CreateDirectory(m_FileSavePath,NULL)==FALSE)
			{
				AfxMessageBox("创建下载录音文件夹失败",MB_OK,MB_ICONWARNING);
				m_gGlobalVF.WriteLog("创建下载录音文件夹失败");
				return;
			}

		}
	}
	else
	{
            m_FileSavePath=m_gGlobalVF.m_FileSavePath;
	}

	//设置服务器
	DWORD dwIP=0;
	m_strServerIP=m_gGlobalVF.m_ServerIP;
	if (m_nServerPort!=m_gGlobalVF.m_nServerPort)
	{
		m_nServerPort=m_gGlobalVF.m_nServerPort;
	}
	if (m_strServerIP!=_T(""))
	{
		dwIP=ntohl(inet_addr(m_strServerIP));

	}
	else
	{
		m_strServerIP=AutoSetServerIPAddr();
		dwIP=ntohl(inet_addr(m_strServerIP));
	}

	m_IPAddrCtrl.SetAddress(dwIP);

	UpdateData(FALSE);

}

void CSettingDlg::OnBnClickedOk()
{
	//设置完成
	UpdateData(TRUE);

	m_strServerIP=IPAddressToCString(m_IPAddrCtrl);

	m_gGlobalVF.m_FileSavePath=m_FileSavePath;
	m_gGlobalVF.m_nServerPort=m_nServerPort;
	m_gGlobalVF.m_ServerIP=m_strServerIP;

    m_gGlobalVF.WriteConfigFile();

	OnOK();
}

void CSettingDlg::OnBnClickedFileopenbtn()
{
	// 选择文件存储路径
	CFileDialogST dlg;
	int nResponse = dlg.SelectFolder(CString((LPCTSTR)IDS_SETTINGDLG_FILEPATH_STR), m_FileSavePath,
		0x40 | BIF_RETURNFSANCESTORS | BIF_RETURNONLYFSDIRS, this);
	if (nResponse == IDOK)
	{
		m_FileSavePath = dlg.GetSelectedFolder() + "\\";
		GetDlgItem(IDC_FileSavePath)->SetWindowText(m_FileSavePath);
	}

	UpdateData(FALSE);



}

CString CSettingDlg::AutoSetServerIPAddr()
{
	char lpName[128];
	gethostname(lpName, sizeof(lpName));		
	HOSTENT *pHostent = gethostbyname(lpName);
	if (pHostent != NULL)
		m_strServerIP = inet_ntoa(*(in_addr *)pHostent->h_addr_list[0]);
	else
		m_strServerIP = _T("192.168.1.1");


	return m_strServerIP;
}

CString CSettingDlg::IPAddressToCString(CIPAddressCtrl& serverIPCtrl)
{
	//将IP地址控件转换为CString IP地址

	DWORD dwIPAddress=0;
	unsigned char* pIP=NULL;
	CString strIP;

	serverIPCtrl.GetAddress(dwIPAddress);

	pIP=(unsigned char*)&dwIPAddress;

	strIP.Format("%u.%u.%u.%u",*(pIP+3),*(pIP+2),*(pIP+1),*(pIP));


	return strIP;


}