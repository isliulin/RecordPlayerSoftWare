#pragma once
#include "afxdtctl.h"


// CQueryRecordDBDlg 对话框

class CQueryRecordDBDlg : public CDialog
{
	DECLARE_DYNAMIC(CQueryRecordDBDlg)

public:
	CQueryRecordDBDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CQueryRecordDBDlg();

// 对话框数据
	enum { IDD = IDD_QueryRecordDBDlg };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();
	// 查询起始日期
	COleDateTime m_StartDate;
	// 起始时间
	COleDateTime m_StartTime;
	// 结束日期
	COleDateTime m_EndDate;
	// 结束时间
	COleDateTime m_EndTime;
};
