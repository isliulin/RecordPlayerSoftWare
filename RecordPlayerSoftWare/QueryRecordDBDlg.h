#pragma once
#include "afxdtctl.h"


// CQueryRecordDBDlg �Ի���

class CQueryRecordDBDlg : public CDialog
{
	DECLARE_DYNAMIC(CQueryRecordDBDlg)

public:
	CQueryRecordDBDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CQueryRecordDBDlg();

// �Ի�������
	enum { IDD = IDD_QueryRecordDBDlg };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();
	// ��ѯ��ʼ����
	COleDateTime m_StartDate;
	// ��ʼʱ��
	COleDateTime m_StartTime;
	// ��������
	COleDateTime m_EndDate;
	// ����ʱ��
	COleDateTime m_EndTime;
};
