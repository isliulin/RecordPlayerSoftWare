// SettingDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "RecordPlayerSoftWare.h"
#include "SettingDlg.h"
#include "FileDialogST.h"
#include "Define.h"
// CSettingDlg �Ի���

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


// CSettingDlg ��Ϣ�������

BOOL CSettingDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	//��ȡ�����ļ�
	//m_gGlobalVF.ReadConfigFile();

	//����
	InitConfigGUI();
    
	m_gGlobalVF.WriteLog("�����öԻ���");

	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
}

void CSettingDlg::InitConfigGUI()
{

	//����Ĭ�ϵĴ洢·��
	if (m_gGlobalVF.m_FileSavePath==_T(""))
	{
		//�����Զ��Ĵ洢·��
		CString appPath=GetApplicationPath();

		m_FileSavePath=appPath+"RecordFile\\";

		//�����ļ���
		if (!::PathIsDirectory(m_FileSavePath))
		{
			//�����ļ���
			if (CreateDirectory(m_FileSavePath,NULL)==FALSE)
			{
				AfxMessageBox("��������¼���ļ���ʧ��",MB_OK,MB_ICONWARNING);
				m_gGlobalVF.WriteLog("��������¼���ļ���ʧ��");
				return;
			}

		}
	}
	else
	{
            m_FileSavePath=m_gGlobalVF.m_FileSavePath;
	}

	//���÷�����
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
	//�������
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
	// ѡ���ļ��洢·��
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
	//��IP��ַ�ؼ�ת��ΪCString IP��ַ

	DWORD dwIPAddress=0;
	unsigned char* pIP=NULL;
	CString strIP;

	serverIPCtrl.GetAddress(dwIPAddress);

	pIP=(unsigned char*)&dwIPAddress;

	strIP.Format("%u.%u.%u.%u",*(pIP+3),*(pIP+2),*(pIP+1),*(pIP));


	return strIP;


}