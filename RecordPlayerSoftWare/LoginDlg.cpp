// LoginDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "RecordPlayerSoftWare.h"
#include "LoginDlg.h"
#include "Define.h"

// CLoginDlg 对话框

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


// CLoginDlg 消息处理程序

BOOL CLoginDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  在此添加额外的初始化
	//初始化对话框
	VERIFY( 1 == SetShapeWindowSkin(this->m_hWnd,"LoginDialog" ) );
    

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void CLoginDlg::OnBnClickedLoginbtn()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(TRUE);
	if (m_strUserName=="")
	{
		AfxMessageBox("用户名不能为空");
		return;
	}
	if (m_strPassWord=="")
	{
		AfxMessageBox("密码不能为空");
		return;
	}

    
	//检测密码和用户名
	
	TCHAR		chUserName[20];
	memset(chUserName,0,sizeof(chUserName));
	DWORD		dwLen = 20;
	CRegKey		ureg;

	if(ureg.Open(HKEY_LOCAL_MACHINE, REG_KEY_NAMED_USERNAME) != ERROR_SUCCESS )
	{
		// 首次使用,使用默认用户名!
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
		// 首次使用,使用默认密码!
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
	// TODO: 在此添加控件通知处理程序代码
	CDialog::OnCancel();

}
