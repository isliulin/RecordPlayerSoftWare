#pragma once
#include "afxcmn.h"


// CSettingDlg 对话框

class CSettingDlg : public CDialog
{
	DECLARE_DYNAMIC(CSettingDlg)

public:
	CSettingDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CSettingDlg();

// 对话框数据
	enum { IDD = IDD_SettingDlg };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();
	// 文件存储路径
	CString m_FileSavePath;
	afx_msg void OnBnClickedFileopenbtn();
	// IP地址控件
	CIPAddressCtrl m_IPAddrCtrl;
	CString m_strServerIP;
	// 服务器端口
	unsigned int m_nServerPort;
	CString AutoSetServerIPAddr();
	CString IPAddressToCString(CIPAddressCtrl& serverIPCtrl);

	void InitConfigGUI();
};
