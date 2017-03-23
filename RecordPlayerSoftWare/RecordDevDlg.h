#pragma once


// CRecordDevDlg 对话框

class CRecordDevDlg : public CDialog
{
	DECLARE_DYNAMIC(CRecordDevDlg)

public:
	CRecordDevDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CRecordDevDlg();

// 对话框数据
	enum { IDD = IDD_AddDevDlg };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	// 设备名称
	CString m_sDevName;
	// 设备ID
	unsigned int m_nDevID;
	// 设备IP
	DWORD m_dDevIP;
	// 设备端口
	unsigned int m_nDevPort;

	CString m_sDevIP; //IP地址
	afx_msg void OnBnClickedOk();

	BOOL m_bAddFlag; //修改标志


	CBrush m_brBrush;//背景画刷
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
};
