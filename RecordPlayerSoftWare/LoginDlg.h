#pragma once
#include "afxwin.h"


// CLoginDlg �Ի���

class CLoginDlg : public CDialog
{
	DECLARE_DYNAMIC(CLoginDlg)

public:
	CLoginDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CLoginDlg();

// �Ի�������
	enum { IDD = IDD_LOGINDLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedLoginbtn();
	afx_msg void OnBnClickedCancelbtn();
	// �û���
	CString m_strUserName;
	// ����
	CString m_strPassWord;
	CEdit m_editPassword;
	CEdit m_editUsername;
};
