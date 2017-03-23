#pragma once
#include "afxcmn.h"
#include "ListCtrlCl.h"

// CPlayerListShowDlg �Ի���

class CPlayerListShowDlg : public CDialog
{
	DECLARE_DYNAMIC(CPlayerListShowDlg)

public:
	CPlayerListShowDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CPlayerListShowDlg();

// �Ի�������
	enum { IDD = IDD_PlayerListShowDlg };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	
	CListCtrlCl m_PlayerListCtrl;// �����б�ؼ�
	//�ؼ��䶯��ε���
	CRect m_ClientRect; //����ܾ��δ�С
	CPoint m_oSize;  //��¼����ܵĿ�͸�

	void InitGUICtrl(); //��ʼ������ؼ�
};
