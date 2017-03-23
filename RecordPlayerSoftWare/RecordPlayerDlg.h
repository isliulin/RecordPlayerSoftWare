
// RecordPlayerDlg.h : 头文件
#pragma once
//
#include "afxwin.h"
#include "afxcmn.h"
#include "CDXGraph.h"
#include "SystemTray.h"
#include "BtnST.h"
#include "BitmapSlider.h"
#include "BitItem.h"
#include "BitSlider.h"
#include "ProSliderCtrl.h"
#include "StaticST.h"
#include "ShowPageDlg.h"
#include "ShowDownLoadDlg.h"
#include "PlayerListShowDlg.h"


const UINT	TRAY_MSG = RegisterWindowMessage( "RecordPlayerSoftWare Tray" );
// CRecordPlayerDlg 对话框
class CRecordPlayerDlg : public CDialogEx
{
// 构造
public:
	CRecordPlayerDlg(CWnd* pParent = NULL);	// 标准构造函数

	~CRecordPlayerDlg();
// 对话框数据
	enum { IDD = IDD_RECORDPLAYERSOFTWARE_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()


public:
	//初始化APP皮肤
	void InitDialogAppSkin();

	//系统托盘类
	//系统托盘类
	CSystemTray				m_systemTray;		// 系统托盘图标设置
	BOOL					m_bWindowShow;		// 窗口是否在显示
	afx_msg void OnMenuitemTrayOpen();
	afx_msg void OnMenuitemTrayClose();
	afx_msg void OnSize(UINT nType, int cx, int cy);



	//控件变动如何调整
	CRect m_ClientRect; //主框架矩形大小
	CPoint m_oSize;  //记录主框架的宽和高
	void InitMainGUI(); //初始化主框架

	//控件按钮
	CButtonST m_PlayBtn; //播放暂停按钮
	CButtonST m_PlayBackBtn;//上一首按钮
	CButtonST m_PlayNextBtn;// 下一首按钮
	BOOL m_bPlayflag; //播放标志
	CButtonST m_SoundCtrlBtn;// 声音控制按钮
	BOOL m_VoCtrlFlag; //音量控制标志
	CBitmapSlider m_MusicVolumeCtrl;// 音量控制
	CProSliderCtrl m_PlayProgressCtrl; //滑块进度条类
	
	HCURSOR	GetSysHandCursor();//获取系统鼠标
    void ShowPlayBtn(BOOL flag); //显示播放或者暂停按钮
	void ShowVolCtrlBtn(BOOL flag);//声音控制按钮

	afx_msg void OnBnClickedPlaybtn();
	afx_msg void OnBnClickedPlaynextbtn();
	afx_msg void OnBnClickedPlayback();
	afx_msg void OnBnClickedSoundbtn();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg LRESULT OnBitmapSliderMoved(WPARAM wParam, LPARAM lParam);

	//DirectShow编程
	BOOL InitComObject();//com对象初始化
	BOOL DestroyComObject(); //销毁Com对象

	void CreateGraph(void);        // 创建Filter Graph
	void DestroyGraph(void);       // 析构Filter Graph

	afx_msg LRESULT OnGraphNotify(WPARAM inWParam, LPARAM inLParam);

	HRESULT SaveGraphFile(IGraphBuilder*pGraph,WCHAR*wszPath);//调试接口

	CDXGraph *   m_pFilterGraph;     // Filter Graph封装
	CString      m_SourceFile;      // 源文件
	UINT         m_nSliderTimer;     // 定时器ID
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	void OnBnClickedPausebtn(); //暂停播放
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	//提取显示时间信息
	void ShowPlayTimeTips();
	// 播放总长显示文本
	CStaticST m_PlayTotalLengthTextCtrl;
	// 播放进度文本
	CStaticST m_PlayCurLengthTextCtrl;

	//播放显示界面
	CShowPageDlg* m_ShowPageDlg;
	CRect m_ShowPageRect; //显示对话框区域

	//下载列表显示界面
	CShowDownLoadDlg* m_ShowDownLoadDlg;

	

	//播放列表显示界面
	CPlayerListShowDlg* m_PlayerListShowDlg;
	

	void CreateShowSubDlg();  //创建显示子对话框

	
	//功能区
	CButtonST m_SettingBtn;// 设置按钮
	CButtonST m_LoginBtn;// 登陆按钮
	CButtonST m_DownLoadBtn;// 下载按钮
	CButtonST m_QueryBtn;// 查询按钮
	CButtonST m_MainPageBtn;// 主页面按钮
	afx_msg void OnBnClickedSettingbtn();
	afx_msg void OnBnClickedDownloadbtn();
	afx_msg void OnBnClickedLoginbtn();
	afx_msg void OnBnClickedQuerybtn();
	afx_msg void OnBnClickedShowdevbtn();

	void SwitchBtnStatus(UINT nIDC,BOOL bactive);


	//显示软件版本
	afx_msg void OnBnClickedAppIcon();

    

	virtual BOOL PreTranslateMessage(MSG* pMsg);
};
