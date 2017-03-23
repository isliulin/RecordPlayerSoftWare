#pragma once
#include "afxcmn.h"
#include "ListCtrlCl.h"

// CShowDownLoadDlg 对话框
#include "./SQLite3/KompexSQLitePrerequisites.h"
#include "./SQLite3/KompexSQLiteDatabase.h"
#include "./SQLite3/KompexSQLiteStatement.h"
#include "./SQLite3/KompexSQLiteException.h"
#include "./SQLite3/KompexSQLiteStreamRedirection.h"
#include "./SQLite3/KompexSQLiteBlob.h"
#include "afxwin.h"
#include "afxdtctl.h"

//查询页
using namespace Kompex;

class CShowDownLoadDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CShowDownLoadDlg)

public:
	CShowDownLoadDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CShowDownLoadDlg();

// 对话框数据
	enum { IDD = IDD_DownLoadListShowDlg };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	// 下载文件的列表
	CListCtrlCl m_DownLoadListCtrl;
	virtual BOOL OnInitDialog();
	
	//控件变动如何调整
	CRect m_ClientRect; //主框架矩形大小
	CPoint m_oSize;  //记录主框架的宽和高
	afx_msg void OnSize(UINT nType, int cx, int cy);

	//界面初始化
	void InitGUICtrl();

	afx_msg void OnLoadrecorddb();


	//SQLite3数据库操作
	SQLiteDatabase*  m_pSQLiteDatabase;
	SQLiteStatement* m_pSQLiteStatement;

	BOOL HasDataBaseFile(CString filename);
	CString m_FilePath;
	

	afx_msg void OnNMRClickDownloadlist(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnQuerydownloadfile();
	afx_msg void OnDeletedownload();
	// 设备组合框控件
	CComboBox m_ComboDevCtrl;
	// 录音通道组合框
	CComboBox m_ComboChannelCtrl;

	void InitComboBox();//需要动态添加初始化
	afx_msg void OnBnClickedQueryrecordbtn();

	BOOL OpenDataBase(CString FileName); //打开数据
	BOOL ExCuteQuerySQL(CString strSQL);
	BOOL CloseDataBase(CString FileName); //关闭数据库
	void AutoAdjustColumnWidth(CListCtrl *pListCtrl);
	// 查询起始日期
	COleDateTime m_StartDate;
	// 起始时间
	COleDateTime m_StartTime;
	// 结束日期
	COleDateTime m_EndDate;
	// 结束时间
	COleDateTime m_EndTime;
};
