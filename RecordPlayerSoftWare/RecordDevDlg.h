#pragma once


// CRecordDevDlg �Ի���

class CRecordDevDlg : public CDialog
{
	DECLARE_DYNAMIC(CRecordDevDlg)

public:
	CRecordDevDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CRecordDevDlg();

// �Ի�������
	enum { IDD = IDD_AddDevDlg };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	// �豸����
	CString m_sDevName;
	// �豸ID
	unsigned int m_nDevID;
	// �豸IP
	DWORD m_dDevIP;
	// �豸�˿�
	unsigned int m_nDevPort;

	CString m_sDevIP; //IP��ַ
	afx_msg void OnBnClickedOk();

	BOOL m_bAddFlag; //�޸ı�־


	CBrush m_brBrush;//������ˢ
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
};
