
// RecordPlayerSoftWare.cpp : 定义应用程序的类行为。
//

#include "stdafx.h"
#include "RecordPlayerSoftWare.h"
#include "RecordPlayerDlg.h"
#include "LoginDlg.h"
#include "minidump.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CRecordPlayerApp

BEGIN_MESSAGE_MAP(CRecordPlayerApp, CWinAppEx)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CRecordPlayerApp 构造

CRecordPlayerApp::CRecordPlayerApp()
{
	// TODO: 在此处添加构造代码，
	// 将所有重要的初始化放置在 InitInstance 中
}


// 唯一的一个 CRecordPlayerApp 对象

CRecordPlayerApp theApp;


// CRecordPlayerApp 初始化

BOOL CRecordPlayerApp::InitInstance()
{
	// 如果一个运行在 Windows XP 上的应用程序清单指定要
	// 使用 ComCtl32.dll 版本 6 或更高版本来启用可视化方式，
	//则需要 InitCommonControlsEx()。否则，将无法创建窗口。
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// 将它设置为包括所有要在应用程序中使用的
	// 公共控件类。
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinAppEx::InitInstance();

	if (!AfxSocketInit())
	{
		AfxMessageBox(IDP_SOCKETS_INIT_FAILED,MB_OK,MB_ICONERROR);
		return FALSE;
	}

	AfxEnableControlContainer();

	// 标准初始化
	// 如果未使用这些功能并希望减小
	// 最终可执行文件的大小，则应移除下列
	// 不需要的特定初始化例程
	// 更改用于存储设置的注册表项
	// TODO: 应适当修改该字符串，
	// 例如修改为公司或组织名
	SetRegistryKey(_T("应用程序向导生成的本地应用程序"));


	::InitSkinMagicLib( AfxGetInstanceHandle(), "IP网络高清录音播放软件" , 
		NULL,
		NULL );

	::LoadSkinFromResource(NULL, (LPSTR)IDR_SMF, "SMF" );


	m_hMutex=CreateMutex( FALSE, FALSE, "IP网络高清录音播放软件"); 
	if ( m_hMutex!= NULL ) 
	{ 
		DWORD err = GetLastError(); 
		if ( err == ERROR_ALREADY_EXISTS )       
		{
			AfxMessageBox("程序已经运行！",MB_OK,MB_ICONERROR);
			return FALSE;   
		}
	} 

	RunCrashHandler();//运行dump程序，已检测应用程序出错时，产生dump文件已进行错误定位

	CLoginDlg dlg;
	if (dlg.DoModal()==IDOK)
	{
		CRecordPlayerDlg dlg;
		m_pMainWnd = &dlg;
		INT_PTR nResponse = dlg.DoModal();
		if (nResponse == IDOK)
		{
			// TODO: 在此放置处理何时用
			//  “确定”来关闭对话框的代码
		}
		else if (nResponse == IDCANCEL)
		{
			// TODO: 在此放置处理何时用
			//  “取消”来关闭对话框的代码
		}
	}
	

	
	// 由于对话框已关闭，所以将返回 FALSE 以便退出应用程序，
	//  而不是启动应用程序的消息泵。
	return FALSE;
}

int CRecordPlayerApp::ExitInstance()
{
	// TODO: 在此添加专用代码和/或调用基类
    ExitSkinMagicLib();
	return CWinAppEx::ExitInstance();
}
