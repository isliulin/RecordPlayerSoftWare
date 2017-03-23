#pragma once
#include "afxcmn.h"
#include "ListCtrlCl.h"

// CShowDownLoadDlg �Ի���
#include "./SQLite3/KompexSQLitePrerequisites.h"
#include "./SQLite3/KompexSQLiteDatabase.h"
#include "./SQLite3/KompexSQLiteStatement.h"
#include "./SQLite3/KompexSQLiteException.h"
#include "./SQLite3/KompexSQLiteStreamRedirection.h"
#include "./SQLite3/KompexSQLiteBlob.h"
#include "afxwin.h"
#include "afxdtctl.h"

//��ѯҳ
using namespace Kompex;

class CShowDownLoadDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CShowDownLoadDlg)

public:
	CShowDownLoadDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CShowDownLoadDlg();

// �Ի�������
	enum { IDD = IDD_DownLoadListShowDlg };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	// �����ļ����б�
	CListCtrlCl m_DownLoadListCtrl;
	virtual BOOL OnInitDialog();
	
	//�ؼ��䶯��ε���
	CRect m_ClientRect; //����ܾ��δ�С
	CPoint m_oSize;  //��¼����ܵĿ�͸�
	afx_msg void OnSize(UINT nType, int cx, int cy);

	//�����ʼ��
	void InitGUICtrl();

	afx_msg void OnLoadrecorddb();


	//SQLite3���ݿ����
	SQLiteDatabase*  m_pSQLiteDatabase;
	SQLiteStatement* m_pSQLiteStatement;

	BOOL HasDataBaseFile(CString filename);
	CString m_FilePath;
	

	afx_msg void OnNMRClickDownloadlist(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnQuerydownloadfile();
	afx_msg void OnDeletedownload();
	// �豸��Ͽ�ؼ�
	CComboBox m_ComboDevCtrl;
	// ¼��ͨ����Ͽ�
	CComboBox m_ComboChannelCtrl;

	void InitComboBox();//��Ҫ��̬��ӳ�ʼ��
	afx_msg void OnBnClickedQueryrecordbtn();

	BOOL OpenDataBase(CString FileName); //������
	BOOL ExCuteQuerySQL(CString strSQL);
	BOOL CloseDataBase(CString FileName); //�ر����ݿ�
	void AutoAdjustColumnWidth(CListCtrl *pListCtrl);
	// ��ѯ��ʼ����
	COleDateTime m_StartDate;
	// ��ʼʱ��
	COleDateTime m_StartTime;
	// ��������
	COleDateTime m_EndDate;
	// ����ʱ��
	COleDateTime m_EndTime;
};
