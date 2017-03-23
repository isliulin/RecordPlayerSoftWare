// ShowDownLoadDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "RecordPlayerSoftWare.h"
#include "ShowDownLoadDlg.h"
#include "Define.h"
#include "QueryRecordDBDlg.h"



// CShowDownLoadDlg 对话框
#ifdef _DEBUG
#define new DEBUG_NEW
#endif




IMPLEMENT_DYNAMIC(CShowDownLoadDlg, CDialogEx)

CShowDownLoadDlg::CShowDownLoadDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CShowDownLoadDlg::IDD, pParent)
	, m_StartDate(COleDateTime::GetCurrentTime())
	, m_StartTime(COleDateTime::GetCurrentTime())
	, m_EndDate(COleDateTime::GetCurrentTime())
	,m_EndTime(COleDateTime::GetCurrentTime())
{
       m_pSQLiteDatabase=NULL;
	   m_pSQLiteStatement=NULL;
	   m_StartDate.SetDate(m_StartDate.GetYear(),m_StartDate.GetMonth()-1,1);
}

CShowDownLoadDlg::~CShowDownLoadDlg()
{
	if(m_pSQLiteDatabase)
	{
         delete m_pSQLiteDatabase;
		 m_pSQLiteDatabase=NULL;


	}

	if(m_pSQLiteStatement)
	{
		delete m_pSQLiteStatement;
		m_pSQLiteStatement=NULL;
	}
}

void CShowDownLoadDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_DownLoadList, m_DownLoadListCtrl);
	DDX_Control(pDX, IDC_DevCombox, m_ComboDevCtrl);
	DDX_Control(pDX, IDC_ChannelCombox, m_ComboChannelCtrl);
	DDX_DateTimeCtrl(pDX, IDC_DATE_START, m_StartDate);
	DDX_DateTimeCtrl(pDX, IDC_TIME_START, m_StartTime);
	DDX_DateTimeCtrl(pDX, IDC_DATE_END, m_EndDate);
	DDX_DateTimeCtrl(pDX, IDC_TIME_END, m_EndTime);
}


BEGIN_MESSAGE_MAP(CShowDownLoadDlg, CDialogEx)
	ON_WM_SIZE()
	ON_COMMAND(ID_LoadRecordDB, &CShowDownLoadDlg::OnLoadrecorddb)
	ON_NOTIFY(NM_RCLICK, IDC_DownLoadList, &CShowDownLoadDlg::OnNMRClickDownloadlist)
	ON_COMMAND(ID_QueryDownLoadFile, &CShowDownLoadDlg::OnQuerydownloadfile)
	ON_COMMAND(ID_DeleteDownLoad, &CShowDownLoadDlg::OnDeletedownload)
	ON_BN_CLICKED(IDC_QueryRecordBtn, &CShowDownLoadDlg::OnBnClickedQueryrecordbtn)
END_MESSAGE_MAP()


// CShowDownLoadDlg 消息处理程序

BOOL CShowDownLoadDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	InitGUICtrl();

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void CShowDownLoadDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);

	//处理子控件
	//尺寸变化
	
	if (nType==SIZE_RESTORED || nType==SIZE_MAXIMIZED)  
	{  

		float   ratio[2];  
		POINT   newDialogSize;  
		CRect   newRect;  

		//获取新的客户区的大小  
		GetClientRect(&newRect);  
		newDialogSize.x = newRect.right - newRect.left;  
		newDialogSize.y = newRect.bottom - newRect.top;  

		//得现在的对话框与以往对话框的大小比例  
		ratio[0]    = (float)newDialogSize.x / m_oSize.x;  
		ratio[1]    = (float)newDialogSize.y / m_oSize.y;  

		CRect Rect;  
		int woc;  

		//左右上角的数据  
		CPoint OldTLPoint, NewTLPint;  
		CPoint OldBRPoint, NewBRPint;  

		//列出所有控件  
		HWND  hwndChild = ::GetWindow(m_hWnd,GW_CHILD);  

		while(hwndChild)  
		{  
			//取得ID  
			woc  = ::GetDlgCtrlID(hwndChild);  

			GetDlgItem(woc)->GetWindowRect(Rect);  
			ScreenToClient(Rect);  

			switch (woc)
			{
			case IDC_DevCombox:
			case IDC_ChannelCombox:
				{
					OldTLPoint = Rect.TopLeft();  
					NewTLPint.x  = long(OldTLPoint.x*ratio[0]+0.5);  
					NewTLPint.y  = long(OldTLPoint.y*ratio[1]+0.5);  

					OldBRPoint   = Rect.BottomRight();  
					NewBRPint.x  = long(OldBRPoint.x*ratio[0]+0.5);  
					//需要设置下拉框的高度
					NewBRPint.y  = long(OldBRPoint.y*ratio[1]+0.5)+100;  
					Invalidate();
				}
				break;	
			case IDC_DownLoadList:
			case IDC_QueryStatic:
			case IDC_QueryRecordBtn:
				{
					OldTLPoint = Rect.TopLeft();  
					NewTLPint.x  = long(OldTLPoint.x*ratio[0]+0.5);  
					NewTLPint.y  = long(OldTLPoint.y*ratio[1]+0.5);  

					OldBRPoint   = Rect.BottomRight();  
					NewBRPint.x  = long(OldBRPoint.x*ratio[0]+0.5);  
					NewBRPint.y  = long(OldBRPoint.y*ratio[1]+0.5);  
					Invalidate();
				}
				break;
			default:
				{
					

					OldTLPoint = Rect.TopLeft();  
					NewTLPint.x  = long(OldTLPoint.x*ratio[0]+0.5);  
					NewTLPint.y  = long(OldTLPoint.y*ratio[1]+0.5);  

					OldBRPoint   = Rect.BottomRight();  
					NewBRPint.x  = long(OldBRPoint.x*ratio[0]+0.5);  
					NewBRPint.y  = long(OldBRPoint.y*ratio[1]+0.5);  
					Invalidate();

				}
				break;

			}

			Rect.SetRect(NewTLPint,NewBRPint);  
			GetDlgItem(woc)->MoveWindow(Rect,TRUE);  


			hwndChild=::GetWindow(hwndChild, GW_HWNDNEXT); 
			Invalidate();

		}  

		


		m_oSize   = newDialogSize;      

	}  
	
	



}


void CShowDownLoadDlg::InitGUICtrl()
{
	//初始化列表
	//初始化列表控件
	//添加列
	
	m_DownLoadListCtrl.SetExtendedStyle(LVS_EX_GRIDLINES|LVS_EX_FULLROWSELECT|LVS_EX_CHECKBOXES|LVS_EX_SUBITEMIMAGES);
	//m_DownLoadListCtrl.SetExtendedStyle(LVS_EX_GRIDLINES|LVS_EX_FULLROWSELECT);

	m_DownLoadListCtrl.SetHeaderHeight(1.5);
    m_DownLoadListCtrl.InsertColumn(0,"序号",LVCFMT_CENTER,50);
	m_DownLoadListCtrl.InsertColumn(1,"文件名",LVCFMT_CENTER,150);
	m_DownLoadListCtrl.InsertColumn(2,"通道",LVCFMT_CENTER,70);
	m_DownLoadListCtrl.InsertColumn(3,"录音时间",LVCFMT_CENTER,70);
	m_DownLoadListCtrl.InsertColumn(4,"录音时长",LVCFMT_CENTER,70);
	m_DownLoadListCtrl.InsertColumn(5,"文件大小",LVCFMT_CENTER,70);
	m_DownLoadListCtrl.InsertColumn(6,"状态",LVCFMT_CENTER,70);


	m_DownLoadListCtrl.SetHeaderBKColor(204,199,199,2);
	m_DownLoadListCtrl.SetHeaderTextColor(RGB(16,71,9));

	m_DownLoadListCtrl.SetRowHeigt(20);

	m_DownLoadListCtrl.SetTextColor(RGB(0,76,136));

   


	InitComboBox();



}

void CShowDownLoadDlg::InitComboBox()
{
	//初始化
	//初始化查询条件中的Commbox
	if (m_gGlobalVF.m_lDevInfoList.IsEmpty())
	{
		//如果为空则返回
		return;
	}

	if (m_ComboDevCtrl.GetCount()>0)
	{
		 m_ComboDevCtrl.ResetContent();
		 m_ComboChannelCtrl.ResetContent();
	}

	POSITION pos=m_gGlobalVF.m_lDevInfoList.GetHeadPosition();
	CString strDevName;
	while(pos!=NULL)
	{
		SRecordDevInfo* pDevInfo=m_gGlobalVF.m_lDevInfoList.GetNext(pos);

		//刷新树形控件

		if (pDevInfo!=NULL)
		{
			strDevName=pDevInfo->m_sDevName;
			//DWORD IP 转换为CString 
			CString  strIP;  
			unsigned  char  *pIP  =  (unsigned  char*)&pDevInfo->m_dDevIP;  
			strIP.Format("%u.%u.%u.%u",*(pIP+3),  *(pIP+2),  *(pIP+1),  *pIP);

			strDevName.Format("%s%d(%s)",pDevInfo->m_sDevName,pDevInfo->m_nDevID,strIP);

			m_ComboDevCtrl.AddString(strDevName);
		}
		

	}

	m_ComboDevCtrl.SetCurSel(0);

	CString strChannel;
	m_ComboChannelCtrl.AddString(_T("All"));
	for (int i=1;i<=8;i++)
	{
		strChannel.Format(_T("通道%d"),i);
		m_ComboChannelCtrl.AddString(strChannel);
	}

	m_ComboChannelCtrl.SetCurSel(0);

	UpdateData(FALSE);

}
void CShowDownLoadDlg::OnLoadrecorddb()
{
	// 读取数据库
	if (!HasDataBaseFile(RECORD_EVENT_DB_FILENAME))
	{
        AfxMessageBox("录音记录不存在！",MB_OK,MB_ICONWARNING);
		return;
	}

	try
	{
		if (m_pSQLiteDatabase==NULL)
		{
			WCHAR* pFileName=mbcsToUnicode(m_FilePath.GetBuffer(0));

			m_pSQLiteDatabase = new Kompex::SQLiteDatabase(unicodeToUtf8(pFileName), SQLITE_OPEN_READWRITE, 0);
		}

		m_pSQLiteDatabase->MoveDatabaseToMemory();

		if(m_pSQLiteStatement==NULL)
		{
			m_pSQLiteStatement = new SQLiteStatement(m_pSQLiteDatabase);
		}

		CString tFilename; 
		tFilename.Format("SELECT * FROM %s",RECORD_EVENT_DB_FILENAME);

		m_pSQLiteStatement->Sql(tFilename);

		//列表删除所有数据
		m_DownLoadListCtrl.DeleteAllItems();

		//根据查询到的数据进行显示
		int nColCount=m_pSQLiteStatement->GetColumnCount();

		int row=0;
		int col=0;
		int nlCount=0;
		CString szText;
		while(m_pSQLiteStatement->FetchRow())
		{
			//需要插入序号
			nlCount++;
			szText.Format("%d",nlCount);
			m_DownLoadListCtrl.InsertItem(row, szText);
			
			for(int col = 0; col <nColCount; col++)
			{
				szText = m_pSQLiteStatement->GetColumnString16(col);
				m_DownLoadListCtrl.SetItem(row, col+1, LVIF_TEXT, szText, 0, 0, 0, 0);
			}
			++row;
		}

		m_pSQLiteStatement->FreeQuery();
	}
	catch (CMemoryException* e)
	{
		AfxMessageBox("内存New错误",MB_OK,MB_ICONERROR);
	}
	catch (CFileException* e)
	{
       AfxMessageBox("文件打开错误",MB_OK,MB_ICONERROR);
	}
	catch (CException* e)
	{
       AfxMessageBox("未知错误",MB_OK,MB_ICONERROR);
	}
	

}

BOOL CShowDownLoadDlg::ExCuteQuerySQL(CString strSQL)
{
	//根据SQL语句查询数据库
	try
	{
		

		m_pSQLiteStatement->Sql(strSQL);

		//列表删除所有数据
		m_DownLoadListCtrl.DeleteAllItems();

		//根据查询到的数据进行显示
		int nColCount=m_pSQLiteStatement->GetColumnCount();

		int row=0;
		int col=0;
		int nlCount=0;
		CString szText;
		while(m_pSQLiteStatement->FetchRow())
		{
			//需要插入序号
			nlCount++;
			szText.Format("%d",nlCount);
			m_DownLoadListCtrl.InsertItem(row, szText);

			for(int col = 0; col <nColCount; col++)
			{
				szText = m_pSQLiteStatement->GetColumnString16(col);
				m_DownLoadListCtrl.SetItem(row, col+1, LVIF_TEXT, szText, 0, 0, 0, 0);
			}
			++row;
		}

		m_pSQLiteStatement->FreeQuery();
	}
	catch (CMemoryException* e)
	{
		AfxMessageBox("内存New错误",MB_OK,MB_ICONERROR);
		return FALSE;
	}
	catch (CFileException* e)
	{
		AfxMessageBox("文件打开错误",MB_OK,MB_ICONERROR);
		return FALSE;
	}
	catch (CException* e)
	{
		AfxMessageBox("未知错误",MB_OK,MB_ICONERROR);
		return FALSE;
	}

   return TRUE;

}


BOOL  CShowDownLoadDlg::HasDataBaseFile(CString filename)
{
	//判断固定的目录文件夹下是否有数据库文件
	//首先获取存取路径
	CString appPath=GetApplicationPath();

	m_FilePath=appPath+"RecordDB\\";

	//生成文件夹
	if (!::PathIsDirectory(m_FilePath))
	{
		//创建文件夹
		if (CreateDirectory(m_FilePath,NULL)==FALSE)
		{
			AfxMessageBox("创建录音记录文件夹失败",MB_OK,MB_ICONERROR);
			return FALSE;
		}

	}
	m_FilePath+=filename;

	if (PathFileExists(m_FilePath))
	{

		//解决中文目录下的情况
	
		return TRUE;
	}
	else
	{
		return FALSE;
	}

}
void CShowDownLoadDlg::OnNMRClickDownloadlist(NMHDR *pNMHDR, LRESULT *pResult)
{
	
	
	
		CMenu   menu;
		POINT pt={0};
		GetCursorPos(&pt);

		menu.LoadMenu(IDR_MENU_SHOW_DOWNLOAD_FILE);

		CMenu* pMenu=menu.GetSubMenu(0);

		pMenu->TrackPopupMenu(TPM_LEFTALIGN|TPM_RIGHTBUTTON,pt.x,pt.y,this);

	    *pResult = 0;

}

void CShowDownLoadDlg::OnQuerydownloadfile()
{
	// 查询对话框
    CQueryRecordDBDlg dlg;
	if (dlg.DoModal()==IDOK)
	{
		if (!HasDataBaseFile(RECORD_EVENT_DB_FILENAME))
		{
			AfxMessageBox("录音记录文件不存在！",MB_OK,MB_ICONWARNING);
			return;
		}

		//查询数据库
		try
		{
			if (m_pSQLiteDatabase==NULL)
			{
				WCHAR* pFileName=mbcsToUnicode(m_FilePath.GetBuffer(0));
				m_pSQLiteDatabase = new Kompex::SQLiteDatabase(unicodeToUtf8(pFileName), SQLITE_OPEN_READWRITE, 0);
			}

			m_pSQLiteDatabase->MoveDatabaseToMemory();

			if(m_pSQLiteStatement==NULL)
			{
				m_pSQLiteStatement = new SQLiteStatement(m_pSQLiteDatabase);
			}

			//构建查询语句
			CString strDateTimeStart;
			strDateTimeStart.Format("%d-%02d-%02d %02d:%02d:%02d",m_stStartTime.wYear,m_stStartTime.wMonth,m_stStartTime.wDay,\
				                                                  m_stStartTime.wHour,m_stStartTime.wMinute,m_stStartTime.wSecond);


			CString strDateTimeEnd;

			
			strDateTimeEnd.Format("%d-%02d-%02d %02d:%02d:%02d",m_stEndTime.wYear,m_stEndTime.wMonth,m_stEndTime.wDay,\
				m_stEndTime.wHour,m_stEndTime.wMinute,m_stEndTime.wSecond);


			CString tFilename; 
			//tFilename.Format("SELECT * FROM %s where FileName>=%s and FileName<=%s,RecordTime>=%s and RecordTime<=%s",RECORD_DB_TABLE_FILENAME,startFileName.GetBuffer(0),endFileName.GetBuffer(0),startRecordTime.GetBuffer(0),endRecordTime.GetBuffer(0));
			tFilename.Format("SELECT * FROM %s where RecordTime>='%s' and RecordTime<='%s'",RECORD_EVENT_DB_FILENAME,strDateTimeStart.GetBuffer(0),strDateTimeEnd.GetBuffer(0));


			m_pSQLiteStatement->Sql(tFilename);

			//列表删除所有数据
			m_DownLoadListCtrl.DeleteAllItems();

			//根据查询到的数据进行显示
			int nColCount=m_pSQLiteStatement->GetColumnCount();

			int row=0;
			int col=0;
			int nlCount=0;
			CString szText;
			while(m_pSQLiteStatement->FetchRow())
			{
				//需要插入序号
				nlCount++;
				szText.Format("%d",nlCount);
				m_DownLoadListCtrl.InsertItem(row, szText);

				for(int col = 0; col <nColCount; col++)
				{
					szText = m_pSQLiteStatement->GetColumnString16(col);
					m_DownLoadListCtrl.SetItem(row, col+1, LVIF_TEXT, szText, 0, 0, 0, 0);
				}
				++row;
			}

			m_pSQLiteStatement->FreeQuery();
		}
		catch (CMemoryException* e)
		{
			AfxMessageBox("内存New错误",MB_OK,MB_ICONERROR);
		}
		catch (CFileException* e)
		{
			AfxMessageBox("文件打开错误",MB_OK,MB_ICONERROR);
		}
		catch (CException* e)
		{
			AfxMessageBox("未知错误",MB_OK,MB_ICONERROR);
		}

	}

}

BOOL CShowDownLoadDlg::OpenDataBase(CString FileName)
{
   //打开数据库
	try{
		   
		    WCHAR* pFileName=mbcsToUnicode(FileName.GetBuffer(0));
			if (m_pSQLiteDatabase==NULL)
			{
				

				m_pSQLiteDatabase = new Kompex::SQLiteDatabase(unicodeToUtf8(pFileName), SQLITE_OPEN_READWRITE, 0);
			}
			
			m_pSQLiteDatabase->Open(unicodeToUtf8(pFileName), SQLITE_OPEN_READWRITE, 0);


			m_pSQLiteDatabase->MoveDatabaseToMemory();

			if(m_pSQLiteStatement==NULL)
			{
				m_pSQLiteStatement = new SQLiteStatement(m_pSQLiteDatabase);
			}
			else
			{
               delete m_pSQLiteStatement;
			   m_pSQLiteStatement=NULL;
               m_pSQLiteStatement = new SQLiteStatement(m_pSQLiteDatabase);

			}
			 
			


	}
	catch(CException* e)
	{
			m_gGlobalVF.WriteLog("打开录音记录失败！");
			return FALSE;
	}
	
	return TRUE;


}

BOOL CShowDownLoadDlg::CloseDataBase(CString FileName)
{
	//关闭数据库
	try{
		m_pSQLiteDatabase->Close();
		if (m_pSQLiteStatement!=NULL)
		{
			delete m_pSQLiteStatement;
			m_pSQLiteStatement=NULL;
		}
	}catch(CException* e)
	{
		m_gGlobalVF.WriteLog("关闭录音记录失败！");
		AfxMessageBox("关闭录音记录失败！",MB_OK,MB_ICONERROR);
		return FALSE;
	}
	

	return TRUE;

	
}

void CShowDownLoadDlg::OnDeletedownload()
{
	//判断是否选中

	//删除数据库中数据

	if (!m_DownLoadListCtrl.DeleteSelectedItem())
	{
        AfxMessageBox("没有选中删除文件",MB_OK,MB_ICONWARNING); 
	}



	
	
	
}

void CShowDownLoadDlg::OnBnClickedQueryrecordbtn()
{
	// 查询:获取查询条件
	UpdateData(TRUE);
	CString devFileDir;
	m_ComboDevCtrl.GetWindowText(devFileDir);


	int nChannel=0;
	nChannel=m_ComboChannelCtrl.GetCurSel(); //从基数0开始：0表示所有通道，1表示1通道，依次类推





	//查询数据库路径
	CString FilePath=m_gGlobalVF.m_DBSavePath;
	FilePath+=devFileDir;

    FilePath+=_T("\\");


	


	CString strFilename=FilePath;
	strFilename+=RECORD_EVENT_DB_FILENAME;
    
	if (!m_gGlobalVF.IsHasRecordDBFile(strFilename))
	{
		m_gGlobalVF.WriteLog("查询数据库文件不存在！");
		AfxMessageBox("查询数据库文件不存在！",MB_OK,MB_ICONWARNING);
		return; 
	}


	//读取时间
	//获取查询时间
	m_stStartTime.wYear=m_StartDate.GetYear();
	m_stStartTime.wMonth=m_StartDate.GetMonth();
	m_stStartTime.wDay=m_StartDate.GetDay();

	m_stStartTime.wHour=m_StartTime.GetHour();
	m_stStartTime.wMinute=m_StartTime.GetMinute();
	m_stStartTime.wSecond=m_StartTime.GetSecond();

	m_stEndTime.wYear=m_EndDate.GetYear();
	m_stEndTime.wMonth=m_EndDate.GetMonth();
	m_stEndTime.wDay=m_EndDate.GetDay();

	m_stEndTime.wHour=m_EndTime.GetHour();
	m_stEndTime.wMinute=m_EndTime.GetMinute();
	m_stEndTime.wSecond=m_EndTime.GetSecond();

	//检查时间
	if ((m_stStartTime.wYear>m_stEndTime.wYear)||(m_stStartTime.wMonth>m_stEndTime.wMonth)\
		||(m_stStartTime.wDay>m_stEndTime.wDay)||(m_stStartTime.wHour>m_stEndTime.wHour)\
		||(m_stStartTime.wMinute>m_stEndTime.wMinute)||(m_stStartTime.wSecond>m_stEndTime.wSecond))
	{
		AfxMessageBox("录音结束时间不能小于录音起始时间！",MB_OK,MB_ICONWARNING);
		return;
	}


	//构建查询语句
	CString strSQL; 
	//构建查询语句
	CString strDateTimeStart;
	strDateTimeStart.Format("%d-%02d-%02d %02d:%02d:%02d",m_stStartTime.wYear,m_stStartTime.wMonth,m_stStartTime.wDay,\
		m_stStartTime.wHour,m_stStartTime.wMinute,m_stStartTime.wSecond);


	CString strDateTimeEnd;


	strDateTimeEnd.Format("%d-%02d-%02d %02d:%02d:%02d",m_stEndTime.wYear,m_stEndTime.wMonth,m_stEndTime.wDay,\
		m_stEndTime.wHour,m_stEndTime.wMinute,m_stEndTime.wSecond);

	if (nChannel==0)
	{
		strSQL.Format("SELECT * FROM %s where RecordTime>='%s' and RecordTime<='%s'",RECORD_EVENT_DB_FILENAME,strDateTimeStart.GetBuffer(0),strDateTimeEnd.GetBuffer(0));
	}
	else
	{
        strSQL.Format("SELECT * FROM %s where Channel=%d and RecordTime>='%s' and RecordTime<='%s'",RECORD_EVENT_DB_FILENAME,nChannel,strDateTimeStart.GetBuffer(0),strDateTimeEnd.GetBuffer(0));
	}
	



	



	OpenDataBase(strFilename);

	ExCuteQuerySQL(strSQL);

	CloseDataBase(strFilename);

    AutoAdjustColumnWidth(&m_DownLoadListCtrl);

	return;

}


void CShowDownLoadDlg::AutoAdjustColumnWidth(CListCtrl *pListCtrl)
{
	pListCtrl->SetRedraw(FALSE);  
	CHeaderCtrl *pHeader = pListCtrl->GetHeaderCtrl();  
	int nColumnCount = pHeader->GetItemCount();  

	for(int i = 0; i < nColumnCount-1; i++)  
	{  
		pListCtrl->SetColumnWidth(i, LVSCW_AUTOSIZE);  
		int nColumnWidth = pListCtrl->GetColumnWidth(i);  
		pListCtrl->SetColumnWidth(i, LVSCW_AUTOSIZE_USEHEADER);  
		int nHeaderWidth = pListCtrl->GetColumnWidth(i);  

		pListCtrl->SetColumnWidth(i, max(nColumnWidth, nHeaderWidth)+20);  
	}  
	pListCtrl->SetRedraw(TRUE);  
	pListCtrl->Invalidate(TRUE);
}