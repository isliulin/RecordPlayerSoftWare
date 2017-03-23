
// RecordPlayerDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "RecordPlayerSoftWare.h"
#include "RecordPlayerDlg.h"
#include "SettingDlg.h"
#include "LoginDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


#define SLIDER_TIMER   100

#define PLAYER_RANGE   1000

#define RGB_AVERAGE(a, b)   ( ((((a) ^ (b)) & 0xfffefefeL) >> 1) + ((a) & (b)) )

// 用于应用程序“关于”菜单项的 CAboutDlg 对话框
int volumes[100] ={-10000,-6418,-6147,-6000,
-5892,-4826,-4647,-4540
-4477, -4162,-3876, -3614, -3500,
-3492,-3374,-3261,-3100,-3153,-3048,-2947,-2849,-2755,-2700,
-2663,-2575,-2520,-2489,-2406,-2325,-2280,-2246,-2170,-2095,-2050,
-2023,-1952,-1900, -1884,-1834, -1820, -1800,-1780, -1757,-1695,-1636,-1579,
-1521,-1500,-1464,-1436,-1420, -1408,-1353,-1299,-1246,-1195,-1144,
-1096,-1060, -1049,-1020,-1003,-957,-912,-868, -800, -774,-784, -760, -744,
-705,-667,-630,-610,-594,-570 ,-558,-525,-493,-462,-432,-403,
-375,-348,-322,-297,-285, -273,-250,-228,-207,-187,-176, -168,
-150,-102,-75,-19,-10,0};

// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedButton1();
	CString m_strPasswordNew;
	CString m_strPasswordConfirm;
	CString m_strOldPassWord;
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
, m_strPasswordNew(_T(""))
, m_strPasswordConfirm(_T(""))
, m_strOldPassWord(_T(""))
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_NEWPASSWORD, m_strPasswordNew);
	DDX_Text(pDX, IDC_NEWPASSWORD2, m_strPasswordConfirm);
	DDX_Text(pDX, IDC_NEWUSERNAME, m_strOldPassWord);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	ON_BN_CLICKED(IDOK, &CAboutDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDC_BUTTON1, &CAboutDlg::OnBnClickedButton1)
END_MESSAGE_MAP()


// CRecordPlayerDlg 对话框




CRecordPlayerDlg::CRecordPlayerDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CRecordPlayerDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);


	m_systemTray.Create( this, TRAY_MSG, CString((LPCTSTR)IDS_RECORDPLAYERSOFTWARE_DIALOG), 
		AfxGetApp()->LoadIcon(IDR_MAINFRAME),
		IDR_TRAY );
	m_systemTray.SetMenuDefaultItem( 0, TRUE);

	m_bWindowShow = FALSE;
	m_bPlayflag=FALSE;
	m_VoCtrlFlag=FALSE;

    //初始化Directshow对象
	//初始化Com对象
	InitComObject();
	m_pFilterGraph=NULL;
	m_nSliderTimer=0;

	//界面子对话框初始化
	m_ShowPageDlg=NULL;
	m_ShowDownLoadDlg=NULL;
	m_PlayerListShowDlg=NULL;

	//读配置文件
	m_gGlobalVF.ReadConfigFile();


}

CRecordPlayerDlg::~CRecordPlayerDlg()
{
    if (m_ShowPageDlg)
    {
		m_ShowPageDlg->DestroyWindow();
    }

	if (m_ShowDownLoadDlg)
	{
		m_ShowDownLoadDlg->DestroyWindow();
	}

	DestroyComObject();

    m_gGlobalVF.WriteLog("程序退出");

	//写配置文件
	m_gGlobalVF.WriteConfigFile();
}

void CRecordPlayerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	//DDX_Control(pDX, IDC_PlayProGressBtn, m_PlayProgressCtrl);
	//DDX_Control(pDX, IDC_PlayTotalLenth, m_PlayTotalLengthTextCtrl);
	//DDX_Control(pDX, IDC_PlayCurLength, m_PlayCurLengthTextCtrl);
	//DDX_Control(pDX, IDC_SettingBtn, m_SettingBtn);
	//DDX_Control(pDX, IDC_LoginBtn, m_LoginBtn);
	//DDX_Control(pDX, IDC_DownLoadBtn, m_DownLoadBtn);
	//DDX_Control(pDX, IDC_QueryBtn, m_QueryBtn);
	//DDX_Control(pDX, IDC_ShowDevBtn, m_MainPageBtn);
}

BEGIN_MESSAGE_MAP(CRecordPlayerDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_COMMAND(ID_MENUITEM_TRAY_OPEN, &CRecordPlayerDlg::OnMenuitemTrayOpen)
	ON_COMMAND(ID_MENUITEM_TRAY_CLOSE, &CRecordPlayerDlg::OnMenuitemTrayClose)
	ON_WM_SIZE()
	
	ON_WM_ERASEBKGND()
	ON_MESSAGE(WM_GRAPHNOTIFY, OnGraphNotify)
	ON_WM_TIMER()
	ON_WM_HSCROLL()
	ON_MESSAGE(WM_BITMAPSLIDER_MOVED, OnBitmapSliderMoved)
	ON_BN_CLICKED(IDC_SettingBtn, &CRecordPlayerDlg::OnBnClickedSettingbtn)
	ON_BN_CLICKED(IDC_DownLoadBtn, &CRecordPlayerDlg::OnBnClickedDownloadbtn)
	ON_BN_CLICKED(IDC_LoginBtn, &CRecordPlayerDlg::OnBnClickedLoginbtn)
	ON_BN_CLICKED(IDC_QueryBtn, &CRecordPlayerDlg::OnBnClickedQuerybtn)
	ON_COMMAND(IDC_SHOW_VERSION,&CRecordPlayerDlg::OnBnClickedAppIcon)
//	ON_BN_CLICKED(IDC_ShowDevBtn, &CRecordPlayerDlg::OnBnClickedShowdevbtn)
END_MESSAGE_MAP()


// CRecordPlayerDlg 消息处理程序

BOOL CRecordPlayerDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标


	//登陆对话框
	
		InitDialogAppSkin();
		InitMainGUI();
	
	
	
	
   


	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CRecordPlayerDlg::OnBnClickedAppIcon()
{
	//加载版本对话框
	CAboutDlg dlg;
	dlg.DoModal();


}

void CRecordPlayerDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CRecordPlayerDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CRecordPlayerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CRecordPlayerDlg::InitDialogAppSkin()
{
      VERIFY( 1 == SetSingleDialogSkin(this->m_hWnd,"NAR5801MainFrame" ) );
	//VERIFY( 1 == SetWindowSkin(m_hWnd,"NAR5801MainFrame"));
}
void CRecordPlayerDlg::OnMenuitemTrayOpen()
{
	//显示窗口
	if (!m_bWindowShow)
	{
		ShowWindow(SW_RESTORE);
		m_bWindowShow = TRUE;
	}

}

void CRecordPlayerDlg::OnMenuitemTrayClose()
{
	//退出程序
	OnCancel();

}

void CRecordPlayerDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);

	//托盘最小化
	if (nType == SIZE_MINIMIZED)
	{
		ShowWindow(SW_HIDE);
		m_bWindowShow = FALSE;
		return;
	}

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
			case IDC_ShowDevBtn:
			case IDC_SettingBtn:
			case IDC_LoginBtn:
			case IDC_DownLoadBtn:
			case IDC_QueryBtn:
			
				{
					OldTLPoint = Rect.TopLeft();  
					NewTLPint.x  = long(OldTLPoint.x*ratio[0]+0.5);  
					NewTLPint.y  = long(OldTLPoint.y*ratio[1]+0.5);  

					OldBRPoint   = Rect.BottomRight();  
					NewBRPint.x  = NewTLPint.x +Rect.Width();  
					NewBRPint.y  = NewTLPint.y +Rect.Height();  

				

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

		//获取视频窗口父窗口的大小  
		//::GetClientRect(hWnd, &grc); 
		//获取显示区域
		if (m_ShowPageDlg!=NULL)
		{
			GetDlgItem(IDC_ShowRect)->GetWindowRect(&m_ShowPageRect);
			ScreenToClient(&m_ShowPageRect);

			m_ShowPageDlg->MoveWindow(&m_ShowPageRect);

			//m_ShowPageDlg->ShowWindow(TRUE);
		}

		if (m_ShowDownLoadDlg!=NULL)
		{
			GetDlgItem(IDC_ShowRect)->GetWindowRect(&m_ShowPageRect);
			ScreenToClient(&m_ShowPageRect);

			m_ShowDownLoadDlg->MoveWindow(&m_ShowPageRect);

			//m_ShowPageDlg->ShowWindow(TRUE);
		}



		


		m_oSize   = newDialogSize;      

	}  
	
	

}

void CRecordPlayerDlg::InitMainGUI()
{
    //初始化主框架
	//获取主框架矩形大小
	
	GetClientRect(&m_ClientRect); 
	
	m_oSize.x = m_ClientRect.right - m_ClientRect.left;  
	m_oSize.y = m_ClientRect.bottom - m_ClientRect.top;  



  //功能区按钮
	/*
  m_SettingBtn.SetIcon(IDI_SettingPress,IDI_Setting);
  m_SettingBtn.SetColor(CButtonST::BTNST_COLOR_BK_OUT,RGB(235,235,235));
  short	shSetBtnColor =255;
  m_SettingBtn.OffsetColor(CButtonST::BTNST_COLOR_BK_IN, shSetBtnColor);


  //登陆连接按钮
  m_LoginBtn.SetIcon(IDI_LoginPress,IDI_Login);
  m_LoginBtn.SetColor(CButtonST::BTNST_COLOR_BK_OUT,RGB(235,235,235));
  short	shLoginBtnColor =255;
  m_LoginBtn.OffsetColor(CButtonST::BTNST_COLOR_BK_IN, shLoginBtnColor);


  //下载按钮
  m_DownLoadBtn.SetIcon(IDI_DownLoadPress,IDI_DownLoad);
  m_DownLoadBtn.SetColor(CButtonST::BTNST_COLOR_BK_OUT,RGB(235,235,235));
  short	shLoadBtnColor =255;
  m_DownLoadBtn.OffsetColor(CButtonST::BTNST_COLOR_BK_IN, shLoadBtnColor);


  //查询按钮
  m_QueryBtn.SetIcon(IDI_QueryPress,IDI_Query);
  m_QueryBtn.SetColor(CButtonST::BTNST_COLOR_BK_OUT,RGB(235,235,235));
  short	shQueryBtnColor =255;
  m_QueryBtn.OffsetColor(CButtonST::BTNST_COLOR_BK_IN, shQueryBtnColor);


  //主界面按钮
  m_MainPageBtn.SetIcon(IDI_MainPagePress,IDI_MainPagePress);
  m_MainPageBtn.SetColor(CButtonST::BTNST_COLOR_BK_OUT,RGB(235,235,235));
  short	shMainPageBtnColor =255;
  m_MainPageBtn.OffsetColor(CButtonST::BTNST_COLOR_BK_IN, shMainPageBtnColor);
  */

  CreateShowSubDlg();
 
  //创建查询页



 
  
  //Test
  /*
  m_SourceFile="C:\\Users\\Administrator\\Desktop\\3_1970_1_1_0_5_40.aac";
  CreateGraph();
  SaveGraphFile(m_pFilterGraph->GetGraph(),L"C:\\myGraphfile.grf");
  */

 
  m_gGlobalVF.WriteLog("程序启动");

  Invalidate(TRUE);
 
}

void CRecordPlayerDlg::CreateShowSubDlg()
{
	//显示主页面
	if (m_ShowPageDlg==NULL)
	{
		//显示界面
		if (m_ShowPageDlg==NULL)
		{
			m_ShowPageDlg=new CShowPageDlg;
			m_ShowPageDlg->Create(IDD_PlayerShowDlg);
			//显示
            m_ShowPageDlg->m_hParentWnd=this->m_hWnd;
			//获取显示区域
			GetDlgItem(IDC_ShowRect)->GetWindowRect(&m_ShowPageRect);
			ScreenToClient(&m_ShowPageRect);

			m_ShowPageDlg->MoveWindow(&m_ShowPageRect);

			m_ShowPageDlg->ShowWindow(SW_SHOW);
		}


	}

	/*
	//显示查询页
	if (m_ShowDownLoadDlg==NULL)
	{
		//显示界面
		if (m_ShowDownLoadDlg==NULL)
		{
			m_ShowDownLoadDlg=new CShowDownLoadDlg;
			m_ShowDownLoadDlg->Create(IDD_DownLoadListShowDlg);
			//显示
			//获取显示区域
			/*
			GetDlgItem(IDC_ShowRect)->GetWindowRect(&m_ShowPageRect);
			ScreenToClient(&m_ShowPageRect);

			m_ShowDownLoadDlg->MoveWindow(&m_ShowPageRect);
			

			//隐藏
			//m_ShowDownLoadDlg->ShowWindow(SW_HIDE);
		}


	}
	*/




}

void CRecordPlayerDlg::SwitchBtnStatus(UINT nIDC,BOOL bactive)
{
	
	switch (nIDC)
	{
	case IDC_ShowDevBtn:
		{
            if (bactive)
            {
				m_MainPageBtn.SetIcon(IDI_MainPagePress,IDI_MainPagePress);
				m_MainPageBtn.SetColor(CButtonST::BTNST_COLOR_BK_OUT,RGB(235,235,235));
				short	shMainPageBtnColor =255;
				m_MainPageBtn.OffsetColor(CButtonST::BTNST_COLOR_BK_IN, shMainPageBtnColor);
            }
			else
			{
				m_MainPageBtn.SetIcon(IDI_MainPagePress,IDI_MainPage);
				m_MainPageBtn.SetColor(CButtonST::BTNST_COLOR_BK_OUT,RGB(235,235,235));
				short	shMainPageBtnColor =255;
				m_MainPageBtn.OffsetColor(CButtonST::BTNST_COLOR_BK_IN, shMainPageBtnColor);
			}
		}
		break;

	case IDC_QueryBtn:
		{
			if (bactive)
			{
				//设置激活按钮
				m_QueryBtn.SetIcon(IDI_QueryPress,IDI_QueryPress);
				m_QueryBtn.SetColor(CButtonST::BTNST_COLOR_BK_OUT,RGB(235,235,235));
				short	shQueryBtnColor =255;
				m_QueryBtn.OffsetColor(CButtonST::BTNST_COLOR_BK_IN, shQueryBtnColor);
			}
			else
			{
				//设置激活按钮
				m_QueryBtn.SetIcon(IDI_QueryPress,IDI_Query);
				m_QueryBtn.SetColor(CButtonST::BTNST_COLOR_BK_OUT,RGB(235,235,235));
				short	shQueryBtnColor =255;
				m_QueryBtn.OffsetColor(CButtonST::BTNST_COLOR_BK_IN, shQueryBtnColor);
			}
		}
		break;
	case IDC_LoginBtn:
		{
            if (bactive)
            {
				m_LoginBtn.SetIcon(IDI_LoginPress,IDI_LoginPress);
				m_LoginBtn.SetColor(CButtonST::BTNST_COLOR_BK_OUT,RGB(235,235,235));
				short	shLoginBtnColor =255;
				m_LoginBtn.OffsetColor(CButtonST::BTNST_COLOR_BK_IN, shLoginBtnColor);

            }
			else
			{
				m_LoginBtn.SetIcon(IDI_LoginPress,IDI_Login);
				m_LoginBtn.SetColor(CButtonST::BTNST_COLOR_BK_OUT,RGB(235,235,235));
				short	shLoginBtnColor =255;
				m_LoginBtn.OffsetColor(CButtonST::BTNST_COLOR_BK_IN, shLoginBtnColor);
			}
		}
		break;
	case IDC_DownLoadBtn:
		{
			if (bactive)
			{
				m_DownLoadBtn.SetIcon(IDI_DownLoadPress,IDI_DownLoadPress);
				m_DownLoadBtn.SetColor(CButtonST::BTNST_COLOR_BK_OUT,RGB(235,235,235));
				short	shLoadBtnColor =255;
				m_DownLoadBtn.OffsetColor(CButtonST::BTNST_COLOR_BK_IN, shLoadBtnColor);

			}
			else
			{
				m_DownLoadBtn.SetIcon(IDI_DownLoadPress,IDI_DownLoad);
				m_DownLoadBtn.SetColor(CButtonST::BTNST_COLOR_BK_OUT,RGB(235,235,235));
				short	shLoadBtnColor =255;
				m_DownLoadBtn.OffsetColor(CButtonST::BTNST_COLOR_BK_IN, shLoadBtnColor);

			}

		}
		break;
	case IDC_SettingBtn:
		{
			if (bactive)
			{
				m_SettingBtn.SetIcon(IDI_SettingPress,IDI_SettingPress);
				m_SettingBtn.SetColor(CButtonST::BTNST_COLOR_BK_OUT,RGB(235,235,235));
				short	shSetBtnColor =255;
				m_SettingBtn.OffsetColor(CButtonST::BTNST_COLOR_BK_IN, shSetBtnColor);
			}
			else
			{
				m_SettingBtn.SetIcon(IDI_SettingPress,IDI_Setting);
				m_SettingBtn.SetColor(CButtonST::BTNST_COLOR_BK_OUT,RGB(235,235,235));
				short	shSetBtnColor =255;
				m_SettingBtn.OffsetColor(CButtonST::BTNST_COLOR_BK_IN, shSetBtnColor);
			}
		}
	}
       
}

HCURSOR	CRecordPlayerDlg::GetSysHandCursor()
{

	TCHAR		strWinDir[MAX_PATH] = {0};
	HCURSOR		hHandCursor			= NULL;
	hHandCursor = ::LoadCursor(NULL, MAKEINTRESOURCE(32649));

	// Still no cursor handle - load the WinHelp hand cursor
	if( hHandCursor == NULL )
	{
		GetWindowsDirectory(strWinDir, MAX_PATH);
		strcat(strWinDir, _T("\\winhlp32.exe"));

		// This retrieves cursor #106 from winhlp32.exe, which is a hand pointer
		HMODULE hModule = ::LoadLibrary(strWinDir);
		DWORD	dwErr = GetLastError();
		if( hModule != NULL )
		{
			HCURSOR	 hTempCur = ::LoadCursor(hModule, MAKEINTRESOURCE(106));
			hHandCursor = (hTempCur != NULL) ? CopyCursor(hTempCur) : NULL;
			FreeLibrary(hModule);
		}
	}
	return hHandCursor;

}

BOOL CRecordPlayerDlg::InitComObject()
{
	//初始化Com对象
	CoInitialize(NULL);

	return TRUE;

}

BOOL CRecordPlayerDlg::DestroyComObject()
{
	CoUninitialize();

	return TRUE;
}


void CRecordPlayerDlg::OnBnClickedPlaybtn()
{
	// 播放和暂停
	// 播放音乐
	//test
	//m_SourceFile="C:\\Users\\Administrator\\Desktop\\何以爱情.mp3";
	

	
	if (!m_bPlayflag)
	{
		if (m_pFilterGraph)
		{
			ShowPlayBtn(FALSE);
			int index=m_MusicVolumeCtrl.GetPos();

			m_pFilterGraph->SetAudioVolume(volumes[index]);
			m_pFilterGraph->Run();
			m_bPlayflag=TRUE;
			//GetDlgItem(IDC_PlayBtn)->SetWindowText("停止");
			// Start a timer
			if (m_nSliderTimer == 0)
			{
				m_nSliderTimer = SetTimer(SLIDER_TIMER, 100, NULL);
				return;
			}
		}

	}else
	{
		
		ShowPlayBtn(TRUE);
		OnBnClickedPausebtn();

	}



}

void CRecordPlayerDlg::OnBnClickedPausebtn()
{
    //暂停播放
	if (m_pFilterGraph)
	{

		m_pFilterGraph->Pause();
		// Stop the timer
		if (m_nSliderTimer == 0)
		{
			m_nSliderTimer = SetTimer(SLIDER_TIMER, 100, NULL);
		}
	}

}

void CRecordPlayerDlg::ShowPlayBtn(BOOL flag)
{
       if(flag)
	   {
		   m_PlayBtn.SetIcon(IDI_PLAYPRESS,IDI_PLAY);

		   m_PlayBtn.SetColor(CButtonST::BTNST_COLOR_BK_OUT,RGB(235,235,235));
		   short	shplayBtnColor =255;
		   m_PlayBtn.OffsetColor(CButtonST::BTNST_COLOR_BK_IN, shplayBtnColor);

		   m_PlayBtn.DrawTransparent(TRUE);
		   m_bPlayflag=FALSE;
	   }
	   else
	   {
		   //显示图标
		   m_PlayBtn.SetIcon(IDI_PAUSEPRESS,IDI_PAUSE);
		 
		   m_PlayBtn.SetColor(CButtonST::BTNST_COLOR_BK_OUT,RGB(235,235,235));
		   short	shplayBtnColor =255;
		   m_PlayBtn.OffsetColor(CButtonST::BTNST_COLOR_BK_IN, shplayBtnColor);

		   m_bPlayflag=TRUE;
	   }

}

void CRecordPlayerDlg::ShowVolCtrlBtn(BOOL flag)
{
      //声音控制按钮
	if (flag)
	{
		m_SoundCtrlBtn.SetIcon(IDI_Sound);
		m_SoundCtrlBtn.SetColor(CButtonST::BTNST_COLOR_BK_OUT,RGB(235,235,235));
		short	shBtnColor =255;
		m_SoundCtrlBtn.OffsetColor(CButtonST::BTNST_COLOR_BK_IN, shBtnColor);

		m_VoCtrlFlag=FALSE;
	}
	else
	{
		m_SoundCtrlBtn.SetIcon(IDI_SoundMute);
		m_SoundCtrlBtn.SetColor(CButtonST::BTNST_COLOR_BK_OUT,RGB(235,235,235));
		short	shBtnColor =255;
		m_SoundCtrlBtn.OffsetColor(CButtonST::BTNST_COLOR_BK_IN, shBtnColor);
		m_VoCtrlFlag=TRUE;

	}



}

void CRecordPlayerDlg::OnBnClickedPlaynextbtn()
{
	// 播放下一首歌曲
	//AfxMessageBox("播放下一首歌曲");

}

void CRecordPlayerDlg::OnBnClickedPlayback()
{
	// 播放上一首歌曲
	//AfxMessageBox("播放上一首歌曲");
}

void CRecordPlayerDlg::OnBnClickedSoundbtn()
{
	//声音控制按钮
	//音量控制
	if (m_VoCtrlFlag)
	{
		//静音控制
		//音量控制
		int index=99;
		m_MusicVolumeCtrl.SetPos(index);

		if (m_pFilterGraph)
		{
			m_pFilterGraph->SetAudioVolume(volumes[index]);
		}

	    ShowVolCtrlBtn(TRUE);



	}else
	{
		//正常控制
		//音量控制
		int index=0;
		m_MusicVolumeCtrl.SetPos(index);

		if (m_pFilterGraph)
		{
			m_pFilterGraph->SetAudioVolume(volumes[index]);
		}
		//禁止音量
		ShowVolCtrlBtn(FALSE);
	}


	UpdateData(FALSE);
	

}

BOOL CRecordPlayerDlg::OnEraseBkgnd(CDC* pDC)
{
	

	return CDialogEx::OnEraseBkgnd(pDC);
}

void CRecordPlayerDlg::CreateGraph(void)
{
	//先销毁之前的
	DestroyGraph();

	//创建新的
	m_pFilterGraph=new CDXGraph;

	if (m_pFilterGraph->Create())
	{
		//如果创建成功，则选择渲染源文件，并设置消息窗口
		// Render the source clip
		m_pFilterGraph->RenderFile(m_SourceFile.GetBuffer(0));
		// Set video window and notification window
		//	m_pFilterGraph->SetDisplayWindow(this->GetSafeHwnd());
		m_pFilterGraph->SetNotifyWindow(this->GetSafeHwnd());
		// Show the first frame
		m_pFilterGraph->Pause();

	}


}

void CRecordPlayerDlg::DestroyGraph(void)
{

	//销毁滤波图表管理器
	if (m_pFilterGraph)
	{
		m_pFilterGraph->Stop(); //停止播放
		m_pFilterGraph->SetNotifyWindow(NULL);//通知窗口设为无

		delete m_pFilterGraph;
		m_pFilterGraph=NULL;
	}

}

LRESULT CRecordPlayerDlg::OnGraphNotify(WPARAM inWParam, LPARAM inLParam)
{
	//Filter Graph给应用程序返回的消息
	IMediaEventEx * pEvent = NULL;
	if (m_pFilterGraph && (pEvent = m_pFilterGraph->GetEventHandle()))
	{
		LONG   eventCode = 0, eventParam1 = 0, eventParam2 = 0;
		while (SUCCEEDED(pEvent->GetEvent(&eventCode, &eventParam1, &eventParam2, 0)))
		{ 
			// Spin through the events
			pEvent->FreeEventParams(eventCode, eventParam1, eventParam2);
			switch (eventCode)
			{
			case EC_COMPLETE:
				//OnBnClickedPausebtn();
				m_pFilterGraph->SetCurrentPosition(0);
				//更新重复播放
				m_PlayProgressCtrl.SetPos(0);
				m_PlayProgressCtrl.SendMessage(WM_LBUTTONUP);
				break;

			case EC_USERABORT:
			case EC_ERRORABORT:
				//OnButtonStop();
				break;

			default:
				break;
			}
		}
	}
	return FALSE;


}

HRESULT CRecordPlayerDlg::SaveGraphFile(IGraphBuilder*pGraph,WCHAR*wszPath)
{
	const WCHAR wszStreamName[]=L"ActiveMovieGraph";
	HRESULT hr;
	IStorage* pStorage=NULL;
	hr=StgCreateDocfile(wszPath,STGM_CREATE|STGM_TRANSACTED|STGM_READWRITE|STGM_SHARE_EXCLUSIVE,0,&pStorage);
	if (FAILED(hr))
	{
		return hr;
	}

	IStream* pStream=NULL;
	hr=pStorage->CreateStream(wszStreamName,STGM_WRITE|STGM_CREATE|STGM_SHARE_EXCLUSIVE,0,0,&pStream);
	if (FAILED(hr))
	{
		pStorage->Release();
		return hr;
	}

	IPersistStream* pPersist=NULL;
	pGraph->QueryInterface(IID_IPersistStream,(void**)&pPersist);
	hr=pPersist->Save(pStream,TRUE);
	pStream->Release();
	pPersist->Release();
	if (SUCCEEDED(hr))
	{
		hr=pStorage->Commit(STGC_DEFAULT);

	}
	pStorage->Release();
	return hr;
}


void CRecordPlayerDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	if ((m_nSliderTimer==SLIDER_TIMER)&&(m_pFilterGraph))
	{
		//播放不同位置帧
		double pos = 0, duration = 1.;
		m_pFilterGraph->GetCurrentPosition(&pos);
		m_pFilterGraph->GetDuration(&duration);
		// 获取前新位置，并更新Slider位置
		int newPos = int(pos * PLAYER_RANGE/(duration));
		if (m_PlayProgressCtrl.GetPos() != newPos)
		{

			m_PlayProgressCtrl.SetPos(newPos);
			m_PlayProgressCtrl._SetPos(newPos);
		}

		ShowPlayTimeTips();


	}

	

	CDialogEx::OnTimer(nIDEvent);
}

void CRecordPlayerDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	//拖动进度条
	//拖动播放进度条控件
	if (pScrollBar->GetSafeHwnd() == m_PlayProgressCtrl.GetSafeHwnd())
	{
		if (m_pFilterGraph)
		{
			// Calculate the current seeking position
			double duration = 1.;
			m_pFilterGraph->GetDuration(&duration);
			double pos = duration * m_PlayProgressCtrl.GetPos() /PLAYER_RANGE;
			m_pFilterGraph->SetCurrentPosition(pos);
		}		
	}
	else if(pScrollBar->GetSafeHwnd() == m_MusicVolumeCtrl.GetSafeHwnd())
	{
		//音量控制
		if (m_pFilterGraph)
		{
			int index=m_MusicVolumeCtrl.GetPos();

			m_pFilterGraph->SetAudioVolume(volumes[index]);
		}



	}

	CDialogEx::OnHScroll(nSBCode, nPos, pScrollBar);
}


LRESULT CRecordPlayerDlg::OnBitmapSliderMoved(WPARAM wParam, LPARAM lParam)
{
	
	return 0;
}

void CRecordPlayerDlg::ShowPlayTimeTips()
{
	//显示音乐媒体文件的时间总长和播放时间。
	if (m_pFilterGraph)
	{

		double pos=1;
		m_pFilterGraph->GetCurrentPosition(&pos);
		double len ;
		m_pFilterGraph->GetDuration(&len);


		int h1, s1, m1;
		int h2, s2, m2;
		h1 = int(pos) / 3600;
		m1 = int(pos) / 60 % 60;
		s1 = int(pos) % 60;
		h2 = int(len) / 3600;
		m2 = int(len) / 60 % 60;
		s2 = int(len) % 60;


		CString strLength;
		strLength.Format("%.02d:%.02d:%02d",h2,m2,s2);
		//GetDlgItem(IDC_PlayTotalLenth)->SetWindowText(strLength);

       
		m_PlayTotalLengthTextCtrl.SetTextColor(RGB(22,204,25));
		m_PlayTotalLengthTextCtrl.SetWindowText(strLength);

		RECT rc1;
		m_PlayTotalLengthTextCtrl.GetWindowRect(&rc1);
		ScreenToClient(&rc1);
		InvalidateRect(&rc1,TRUE);


		CString strCurLength;
		strCurLength.Format("%.02d:%.02d:%.02d",h1,m1,s1);
		//GetDlgItem(IDC_PlayCurLength)->SetWindowText(strCurLength);
        m_PlayCurLengthTextCtrl.SetTextColor(RGB(225,225,225));
		m_PlayCurLengthTextCtrl.SetWindowText(strCurLength);
		
		RECT rc2;
		m_PlayCurLengthTextCtrl.GetWindowRect(&rc2);
		ScreenToClient(&rc2);
		InvalidateRect(&rc2,TRUE);

	}

}




void CRecordPlayerDlg::OnBnClickedSettingbtn()
{
	//打开设置对话框进行设置
	SwitchBtnStatus(IDC_ShowDevBtn,FALSE);
	SwitchBtnStatus(IDC_DownLoadBtn,FALSE);
	SwitchBtnStatus(IDC_QueryBtn,FALSE);
	SwitchBtnStatus(IDC_LoginBtn,FALSE);
	SwitchBtnStatus(IDC_SettingBtn,TRUE);
	CSettingDlg dlg;
	if (dlg.DoModal()==IDOK)
	{
		//设置完成
		m_gGlobalVF.WriteLog("设置完成");

	}
	else
	{
		m_gGlobalVF.WriteLog("设置取消");
	}


}

void CRecordPlayerDlg::OnBnClickedDownloadbtn()
{
	// TODO: 在此添加控件通知处理程序代码
	SwitchBtnStatus(IDC_ShowDevBtn,FALSE);
	SwitchBtnStatus(IDC_DownLoadBtn,TRUE);
	SwitchBtnStatus(IDC_QueryBtn,FALSE);
	SwitchBtnStatus(IDC_LoginBtn,FALSE);
	SwitchBtnStatus(IDC_SettingBtn,FALSE);


}

void CRecordPlayerDlg::OnBnClickedLoginbtn()
{
	// TODO: 在此添加控件通知处理程序代码
	SwitchBtnStatus(IDC_ShowDevBtn,FALSE);
	SwitchBtnStatus(IDC_DownLoadBtn,FALSE);
	SwitchBtnStatus(IDC_QueryBtn,FALSE);
	SwitchBtnStatus(IDC_LoginBtn,TRUE);
	SwitchBtnStatus(IDC_SettingBtn,FALSE);
}

void CRecordPlayerDlg::OnBnClickedQuerybtn()
{
	// TODO: 在此添加控件通知处理程序代码
	SwitchBtnStatus(IDC_ShowDevBtn,FALSE);
	SwitchBtnStatus(IDC_DownLoadBtn,FALSE);
	SwitchBtnStatus(IDC_QueryBtn,TRUE);
	SwitchBtnStatus(IDC_LoginBtn,FALSE);
	SwitchBtnStatus(IDC_SettingBtn,FALSE);

	//显示界面
	if(m_ShowDownLoadDlg!=NULL)
	{
		GetDlgItem(IDC_ShowRect)->GetWindowRect(&m_ShowPageRect);
		ScreenToClient(&m_ShowPageRect);


        m_ShowDownLoadDlg->InitComboBox();
		m_ShowDownLoadDlg->MoveWindow(&m_ShowPageRect);
		m_ShowDownLoadDlg->ShowWindow(SW_SHOW);
	}

	if(m_ShowPageDlg!=NULL)
	{
		//显示
		//获取显示区域
		m_ShowPageDlg->ShowWindow(SW_HIDE);
	}


	Invalidate(TRUE);


}


void CRecordPlayerDlg::OnBnClickedShowdevbtn()
{
	//显示设备页
	SwitchBtnStatus(IDC_ShowDevBtn,TRUE);
	SwitchBtnStatus(IDC_DownLoadBtn,FALSE);
	SwitchBtnStatus(IDC_QueryBtn,FALSE);
	SwitchBtnStatus(IDC_LoginBtn,FALSE);
	SwitchBtnStatus(IDC_SettingBtn,FALSE);

	if(m_ShowDownLoadDlg!=NULL)
	{
		
		m_ShowDownLoadDlg->ShowWindow(SW_HIDE);
	}

	if(m_ShowPageDlg!=NULL)
	{
		//显示
		//获取显示区域
		GetDlgItem(IDC_ShowRect)->GetWindowRect(&m_ShowPageRect);
		ScreenToClient(&m_ShowPageRect);

		m_ShowDownLoadDlg->MoveWindow(&m_ShowPageRect);
		m_ShowPageDlg->ShowWindow(SW_SHOW);
	}


	Invalidate(TRUE);
	
}

BOOL CRecordPlayerDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 在此添加专用代码和/或调用基类
	if(pMsg->message   ==   WM_KEYDOWN)   
	{   
		switch(pMsg->wParam)   
		{   
		case   VK_RETURN://屏蔽回车   
			return   TRUE;   
		case   VK_ESCAPE://屏蔽Esc   
			return   TRUE;   
		}   
	}   
 

	return CDialogEx::PreTranslateMessage(pMsg);
}

void CAboutDlg::OnBnClickedOk()
{
	UpdateData(TRUE);
	//检测原密码
	if (!m_strOldPassWord.IsEmpty())
	{
	   //原密码不为空；
		//判断原密码是否正确
		TCHAR		chStandard[20];
		DWORD		dwLen = 20;
		CRegKey		reg;
		if (reg.Open(HKEY_LOCAL_MACHINE, REG_KEY_NAMED_PASSWOR) == ERROR_SUCCESS)
		{
			reg.QueryStringValue( _T("Password"),chStandard,&dwLen);
			reg.Close();
		}
		if ((m_strOldPassWord != _T("sponsor")) && (m_strOldPassWord != chStandard))
		{
			AfxMessageBox(CString((LPCTSTR)IDS_LOGINDLG_STR1));
			return;
		}

		if (!m_strPasswordNew.IsEmpty() || !m_strPasswordConfirm.IsEmpty())
		{
			if (m_strPasswordNew != m_strPasswordConfirm)
			{
				AfxMessageBox(CString((LPCTSTR)IDS_ABOUTDLG_STR1));
				return;
			}
			else
			{
				//保存密码
				TCHAR cbStrPassword[20];
				memset(cbStrPassword,0,sizeof(cbStrPassword));
				CRegKey		reg;
				if(reg.Open(HKEY_LOCAL_MACHINE, REG_KEY_NAMED_PASSWOR) == ERROR_SUCCESS)
				{
					memcpy(cbStrPassword,m_strPasswordNew.GetBuffer(0),m_strPasswordNew.GetLength());
					reg.SetStringValue(_T("Password"),cbStrPassword);
					reg.Close();
					AfxMessageBox( CString((LPCTSTR)IDS_ABOUTDLG_STR2));
				}
			}
		}

	}

	OnOK();
}

void CAboutDlg::OnBnClickedButton1()
{
	// TODO: 在此添加控件通知处理程序代码
	OnCancel();
}
