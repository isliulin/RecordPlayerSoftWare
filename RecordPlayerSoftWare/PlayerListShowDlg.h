#pragma once
#include "afxcmn.h"
#include "ListCtrlCl.h"

// CPlayerListShowDlg 对话框

class CPlayerListShowDlg : public CDialog
{
	DECLARE_DYNAMIC(CPlayerListShowDlg)

public:
	CPlayerListShowDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CPlayerListShowDlg();

// 对话框数据
	enum { IDD = IDD_PlayerListShowDlg };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	
	CListCtrlCl m_PlayerListCtrl;// 播放列表控件
	//控件变动如何调整
	CRect m_ClientRect; //主框架矩形大小
	CPoint m_oSize;  //记录主框架的宽和高

	void InitGUICtrl(); //初始化界面控件
};
