#pragma once
#include "afxcmn.h"


// CSettingDlg �Ի���

class CSettingDlg : public CDialog
{
	DECLARE_DYNAMIC(CSettingDlg)

public:
	CSettingDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CSettingDlg();

// �Ի�������
	enum { IDD = IDD_SettingDlg };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();
	// �ļ��洢·��
	CString m_FileSavePath;
	afx_msg void OnBnClickedFileopenbtn();
	// IP��ַ�ؼ�
	CIPAddressCtrl m_IPAddrCtrl;
	CString m_strServerIP;
	// �������˿�
	unsigned int m_nServerPort;
	CString AutoSetServerIPAddr();
	CString IPAddressToCString(CIPAddressCtrl& serverIPCtrl);

	void InitConfigGUI();
};
