#pragma once
#include "afxwin.h"


// CShowInfoDlg �Ի���

class CShowInfoDlg : public CDialog
{
	DECLARE_DYNAMIC(CShowInfoDlg)

public:
	CShowInfoDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CShowInfoDlg();

// �Ի�������
	enum { IDD = IDD_ShowTipInfoDlg };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	// ��ʾ���ı���Ϣ
	//CStatic m_ShowInfoText;
	// ��ʾͼ��
	//CButton m_IconBtn;
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	CBrush m_brBrush;

	
	afx_msg void OnBnClickedOk();

	void OnCloseDlg();
	afx_msg void OnClose();
};
