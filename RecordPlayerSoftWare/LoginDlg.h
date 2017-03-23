#pragma once
#include "afxwin.h"


// CLoginDlg 对话框

class CLoginDlg : public CDialog
{
	DECLARE_DYNAMIC(CLoginDlg)

public:
	CLoginDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CLoginDlg();

// 对话框数据
	enum { IDD = IDD_LOGINDLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedLoginbtn();
	afx_msg void OnBnClickedCancelbtn();
	// 用户名
	CString m_strUserName;
	// 密码
	CString m_strPassWord;
	CEdit m_editPassword;
	CEdit m_editUsername;
};
