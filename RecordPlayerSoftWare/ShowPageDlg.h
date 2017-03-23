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
#include "BtnST.h"
#include "BitmapSlider.h"
#include "ProSliderCtrl.h"
#include "MyFtpClient.h"
//#include "CJLibrary.h"
#include "TreeCtrlEx.h"
#include "ClientSocket.h"
#include "StaticST.h"
#include "ShowInfoDlg.h"
// CShowPageDlg 对话框
//作为设备主页面显示
#include <Map>

#include "libzplay.h"
using namespace libZPlay;

using namespace std;


typedef CTypedPtrList<CPtrList ,char*>CPtrFileList;//这里定义个模板类型;存放已经下载的文件
typedef CMap<CString,LPCTSTR,DownLoadState,DownLoadState>FileNameStateMap;


#define  WM_DOWNLOAD_MSG_PROCESS WM_APP+5002

#define  PLAY_BUF_SIZE 1024*10

int  __stdcall  myCallbackFunc(void* instance,
							   void *user_data,
							   TCallbackMessage message,
							   unsigned int param1,
							   unsigned int param2);


class CShowPageDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CShowPageDlg)

public:
	CShowPageDlg();
	virtual ~CShowPageDlg();

// 对话框数据
	enum { IDD = IDD_PlayerShowDlg };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	// 设备树控件
	//CTreeCtrl m_DeviceTreeCtrl;
	//CCJTreeCtrl  m_DeviceTreeCtrl;

	CTreeCtrlEx m_DeviceTreeCtrl;

	CImageList	m_ImageList; //图像列表
	// 设备列表控件
	CListCtrlCl m_DeviceListCtrl;
	CImageList	m_ImageList2; //图像列表
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//控件变动如何调整
	CRect m_ClientRect; //主框架矩形大小
	CPoint m_oSize;  //记录主框架的宽和高

	void InitGUICtrl(); //初始化控件

	void ConfigRecordChannelName(CString strFilePath,SRecordDevInfo* pDevInfo);// 配置通道名称


	BOOL IsHasConfig(CString strFilePath); //是否有配置文件


	BOOL SetRecorderTreeChannelName(SRecordDevInfo* pDevInfo);


	afx_msg void OnBnClickedAdddevbtn();
	afx_msg void OnAdddev();
	afx_msg void OnDeldev();
	afx_msg void OnModifydev();
	afx_msg void OnBnClickedDeletedevbtn();
	afx_msg void OnNMRClickDeviceTree(NMHDR *pNMHDR, LRESULT *pResult);

	
	void OnShowrecorddb();
	afx_msg void OnUpdateShowrecorddb(CCmdUI *pCmdUI);
	CString  CreateSaveDevRecordDB(CString FileDir); //创建数据库存储路径FileDir为默认的最后一个文件夹：以设备名和ID命名
    
	BOOL IsHasRecordDBFile(CString FilePath); //判断路径下是否存在数据库文件

	CString m_DBSavePath; //数据库存放路径：默认
	//SQLite3数据库操作
	SQLiteDatabase*  m_pSQLiteDatabase;
	SQLiteStatement* m_pSQLiteStatement;
	CCriticalSection m_DBLockSection;//对数据库操作使用安全锁
	DWORD m_ChannelV[8];

	//双击树事件
	afx_msg void OnNMDblclkDeviceTree(NMHDR *pNMHDR, LRESULT *pResult);

	//列表根据内容自动调节列宽
	void AutoAdjustColumnWidth(CListCtrl *pListCtrl);

	// 查询起始日期
	COleDateTime m_StartDate;
	// 起始时间
	COleDateTime m_StartTime;
	// 结束日期
	COleDateTime m_EndDate;
	// 结束时间
	COleDateTime m_EndTime;
	//数据库操作
	BOOL OpenDataBase(CString FileName); //打开数据
	BOOL ExCuteQuerySQL(CString strSQL);
	BOOL CloseDataBase(CString FileName); //关闭数据库
	

	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	CBrush m_brBrush;//背景画刷

	//响应点击事件点击列表事件
	afx_msg void OnNMClickDeviceList(NMHDR *pNMHDR, LRESULT *pResult);

	//播放功能
	CButtonST m_PlayBtn;// 播放按钮
	
	CButtonST m_PlayBackBtn;// 后退播放
	
	CButtonST m_PlayNextBtn;// 前进播放

	CButtonST m_SoundCtrlBtn; // 声控按钮
	// 声音音量控件
	CBitmapSlider m_MusicVolumeCtrl;
	CProSliderCtrl m_PlayProgressCtrl;
	//CSliderCtrl m_PlayProgressCtrl;

	afx_msg void OnBnClickedPlaybtn();
	afx_msg void OnBnClickedPlaynextbtn();
	afx_msg void OnBnClickedPlaybackbtn();

   BOOL m_bPlayflag;
   BOOL m_VoCtrlFlag; //音量控制标志
   void ShowPlayBtn(BOOL flag); //显示播放或者暂停按钮
   void ShowVolCtrlBtn(BOOL flag);//声音控制按钮
  
   afx_msg void OnBnClickedSoundctrlbtn();
 
   afx_msg BOOL OnEraseBkgnd(CDC* pDC);


   //音频播放
   ZPlay* m_Player; //播放类实例
   BOOL PlayAAcFile(CString& filename);  //播放音频文件
   void PlayAAcStream(void* buffer=NULL,unsigned int len=0); //播放音频流
   char m_PlayBuffer[PLAY_BUF_SIZE]; //播放音频的缓冲区
   FILE* m_inFile; //打开的播放输入文件
   afx_msg LRESULT OnBitmapSliderMoved(WPARAM wParam, LPARAM lParam);
   afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);

   double m_PlayTotalLength; //播放的文件总时长：单位秒

   afx_msg void OnTimer(UINT_PTR nIDEvent);
   UINT         m_nSliderTimer;     // 定时器ID
   BOOL         m_bStreamFlag; //流播放标志
  
   void ProcessShowSliderPos();
   void ShowPlayTimeTips();//显示播放时间

  //FTP通信类
   CMyFtpClient m_FtpClient; //ftp客户端

   BOOL ConnectFtpServer(CString serverIP,CString username,CString pwd,int port=21); //连接服务器

   BOOL DownLoadFileFromServer(CString strServerFileName,CString StrLocalFileName,BOOL flag=TRUE,__int64 nStartPos=0,__int64 nEndPos=0); //下载数据库则为False，下载文件则为True

   
 

   BOOL PauseDownLoad(); //暂停下载
   BOOL ResumeDownLoad();//继续下载

   

   static DWORD WINAPI ConnectFTPServerThread(LPVOID lpParameter);//由于无法连接时返回时间太长，导致程序卡顿

   void ProcessDownLoadPos();

    unsigned int GetCurDownLoadItem(); //获取当前正在下载的项
	void SetCurDownLoadItem(unsigned int index); //设置当前下载项

	unsigned int m_nCurDownLoadItem; //当前下载项

    void SetDownLoadState(DownLoadState eState); //设置下载状态
    DownLoadState GetDownLoadState();           //获取下载状态

	DownLoadState m_eDWLState; //下载状态

	afx_msg LRESULT OnProcessDownLoadMsg(WPARAM wparam,LPARAM lparam); //处理点击列表的下载按钮事件
	BOOL ProcessDownLoadMsg(); //包装点击列表的下载按钮事件
	static DWORD WINAPI ProcessDownLoadMsgThread(LPVOID lpParameter);//由于无法连接时返回时间太长，导致程序卡顿

	afx_msg LRESULT OnProcessFTPMsg(WPARAM wparam,LPARAM lparam);
	BOOL m_bShow; //控制同步文件显示对话框提醒

	//需要采用映射的方式记录每一行的状态
	FileNameStateMap m_StateMap;  //状态映射

	CRITICAL_SECTION m_csProject; //保护临界区

	SFTPServerInfo m_sFtpSeverInfo; //用到的Ftp服务器信息

	BOOL m_bDBFlag; //数据库下载标志

private:
	CString SetFtpServerIP();  //设置服务器IP地址,返回该设备的默认

	


public:
	afx_msg void OnDownloadrecorddb();  //下载数据库
	BOOL DownLoadRecordDB();//包装OnDownloadrecorddb函数
	BOOL DownLoadChannelConfig();

	afx_msg void OnDownloadfile();
	afx_msg void OnPlayrecordfile();
	afx_msg void OnNMRClickDeviceList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnUpdateDownloadfile(CCmdUI *pCmdUI);


	//下载文件遍历;
	CPtrFileList m_ptrFileList; //设备目录下已经下载的文件列表
	BOOL FindAllFileList(CString strFileDir,CString FileExt=_T(".AAC")); //找到目录下的所有文件
    CString m_strCurDevFilePath; //当前设备文件路径

	

private:
	BOOL ParseDBFileShow();
	CString GetRecordTimeFromSecond(UINT nSecond); //将秒数转换为日期时间
	CString GetRecordTimeLength(UINT nStartSecond,UINT nEndSecond);//录音时长
	CString CreateRecordFileName(CString nCh,UINT nStartSceond,UINT nEndSecond);
	UINT   GetSecondFromRecordTime(SYSTEMTIME& st);
	void  FillTreeCtrlDefaultColor();
	CString GetFileNameFromItem(UINT nRow);//从每一行中获取文件名
	BOOL GetRecycleFilePosFromItemDB(UINT nRow,__int64& nStartPos,__int64& nEndPos); //获取下载位置
	


public:
	
	CButtonST m_EditBtn;// 修改设备按钮
	afx_msg void OnBnClickedEditbtn();
	CButtonST m_AddDevBtn; //添加设备按钮
	CButtonST m_DelDevBtn; //删除设备按钮
	// 显示声音波形
	CStatic m_ShowSpectrumCtrl;

	HWND m_hParentWnd; //父窗口句柄

	//通信句柄
	//CAsyncSocket m_Sock;
    CClientSocket m_Sock;
	SRecordDevInfo* GetSelectTreeDevInfo();


   


	// 连接等待状态
	CStaticST m_strConnectInfo;
	UINT m_nWaitConnTimer; //连接等待定时器
	void ShowConnWaitInfo(); //显示连接信息
	UINT m_nWaitSecond;//等待的秒数
	BOOL m_LoginNotify; //登陆通知

	afx_msg void OnDownloadallfile(); //下载所有录音文件
	static DWORD WINAPI ProcessDownLoadAllFileThread(LPVOID lpParameter);//由于无法连接时返回时间太长，导致程序卡顿

	void DownloadAllfile();//下载所有文件

	BOOL m_bDownFlag; //下载标志
	UINT m_nCurDownCount;//当前下载个数
	// 查询按钮
	CButtonST m_QueryBtn;
	// 批量下载按钮
	CButtonST m_BatchDownBtn;
	afx_msg void OnBnClickedQuerybtn();
	afx_msg void OnBnClickedBatchdownbtn();
	// 登陆录音服务器按钮
	CButtonST m_LoginBtn;
	afx_msg void OnBnClickedLoginbtn();

	//为了解决在下载期间，开启另一个下载,采用变量进行控制
	BOOL m_bDowningFlag;//正在下载标志
	AppRunState m_eAppRunState;//运行状态
	CShowInfoDlg m_ShowInfodlg; //显示该提示对话框表示正在下载

	
	// 录音类型
	CComboBox m_RecorderType; 
	RECORDTYPE m_eRecordType;


	
};
