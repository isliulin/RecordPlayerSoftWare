#pragma once
#include "afxwin.h"


// CShowInfoDlg 对话框

class CShowInfoDlg : public CDialog
{
	DECLARE_DYNAMIC(CShowInfoDlg)

public:
	CShowInfoDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CShowInfoDlg();

// 对话框数据
	enum { IDD = IDD_ShowTipInfoDlg };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	// 显示的文本信息
	//CStatic m_ShowInfoText;
	// 显示图标
	//CButton m_IconBtn;
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	CBrush m_brBrush;

	
	afx_msg void OnBnClickedOk();

	void OnCloseDlg();
	afx_msg void OnClose();
};
