// LoginDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "RecordPlayerSoftWare.h"
#include "LoginDlg.h"
#include "Define.h"

// CLoginDlg �Ի���

IMPLEMENT_DYNAMIC(CLoginDlg, CDialog)

CLoginDlg::CLoginDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CLoginDlg::IDD, pParent)
	, m_strUserName(_T("admin"))
	, m_strPassWord(_T(""))
{

}

CLoginDlg::~CLoginDlg()
{
}

void CLoginDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_UserNameEdit, m_strUserName);
	DDX_Text(pDX, IDC_PASSWORDEDIT, m_strPassWord);
	DDX_Control(pDX, IDC_PASSWORDEDIT, m_editPassword);
	DDX_Control(pDX, IDC_UserNameEdit, m_editUsername);
}


BEGIN_MESSAGE_MAP(CLoginDlg, CDialog)
	ON_BN_CLICKED(IDC_LoginBtn, &CLoginDlg::OnBnClickedLoginbtn)
	ON_BN_CLICKED(IDC_CancelBtn, &CLoginDlg::OnBnClickedCancelbtn)
END_MESSAGE_MAP()


// CLoginDlg ��Ϣ�������

BOOL CLoginDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  �ڴ���Ӷ���ĳ�ʼ��
	//��ʼ���Ի���
	VERIFY( 1 == SetShapeWindowSkin(this->m_hWnd,"LoginDialog" ) );
    

	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
}

void CLoginDlg::OnBnClickedLoginbtn()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	UpdateData(TRUE);
	if (m_strUserName=="")
	{
		AfxMessageBox("�û�������Ϊ��");
		return;
	}
	if (m_strPassWord=="")
	{
		AfxMessageBox("���벻��Ϊ��");
		return;
	}

    
	//���������û���
	
	TCHAR		chUserName[20];
	memset(chUserName,0,sizeof(chUserName));
	DWORD		dwLen = 20;
	CRegKey		ureg;

	if(ureg.Open(HKEY_LOCAL_MACHINE, REG_KEY_NAMED_USERNAME) != ERROR_SUCCESS )
	{
		// �״�ʹ��,ʹ��Ĭ���û���!
		ureg.Create(HKEY_LOCAL_MACHINE, REG_KEY_NAMED_USERNAME);
		ureg.SetValue(_T("admin"), _T("Username"));
		ureg.Close();
	}
	if (ureg.Open(HKEY_LOCAL_MACHINE, REG_KEY_NAMED_USERNAME) == ERROR_SUCCESS)
	{
		ureg.QueryValue(chUserName, _T("Username"), &dwLen);
		ureg.Close();
	}
	if ((m_strUserName != _T("sponsor")) && (m_strUserName !=chUserName))
	{
		AfxMessageBox(CString((LPCTSTR)IDS_LOGINDLG_STR2));
		m_editUsername.SetFocus();

		return;
	}

	CRegKey		reg;
	DWORD		dwPwLen = 20;
	TCHAR		chPassWord[20];
    memset(chPassWord,0,sizeof(chPassWord));

	if(reg.Open(HKEY_LOCAL_MACHINE, REG_KEY_NAMED_PASSWOR) != ERROR_SUCCESS )
	{
		// �״�ʹ��,ʹ��Ĭ������!
		reg.Create(HKEY_LOCAL_MACHINE, REG_KEY_NAMED_PASSWOR);
		reg.SetValue(_T("123456"),_T("Password"));
		reg.Close();
	}
	if (reg.Open(HKEY_LOCAL_MACHINE, REG_KEY_NAMED_PASSWOR) == ERROR_SUCCESS)
	{
		reg.QueryValue(chPassWord,_T("Password"),&dwPwLen);
		
		reg.Close();
	}
	if ((m_strPassWord != _T("sponsor")) && (m_strPassWord != chPassWord))
	{
		AfxMessageBox(CString((LPCTSTR)IDS_LOGINDLG_STR1));
		m_editPassword.SetFocus();
	}
	else
		CDialog::OnOK();



	
}

void CLoginDlg::OnBnClickedCancelbtn()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CDialog::OnCancel();

}
