
// RecordPlayerSoftWare.cpp : ����Ӧ�ó��������Ϊ��
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


// CRecordPlayerApp ����

CRecordPlayerApp::CRecordPlayerApp()
{
	// TODO: �ڴ˴���ӹ�����룬
	// ��������Ҫ�ĳ�ʼ�������� InitInstance ��
}


// Ψһ��һ�� CRecordPlayerApp ����

CRecordPlayerApp theApp;


// CRecordPlayerApp ��ʼ��

BOOL CRecordPlayerApp::InitInstance()
{
	// ���һ�������� Windows XP �ϵ�Ӧ�ó����嵥ָ��Ҫ
	// ʹ�� ComCtl32.dll �汾 6 ����߰汾�����ÿ��ӻ���ʽ��
	//����Ҫ InitCommonControlsEx()�����򣬽��޷��������ڡ�
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// ��������Ϊ��������Ҫ��Ӧ�ó�����ʹ�õ�
	// �����ؼ��ࡣ
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinAppEx::InitInstance();

	if (!AfxSocketInit())
	{
		AfxMessageBox(IDP_SOCKETS_INIT_FAILED,MB_OK,MB_ICONERROR);
		return FALSE;
	}

	AfxEnableControlContainer();

	// ��׼��ʼ��
	// ���δʹ����Щ���ܲ�ϣ����С
	// ���տ�ִ���ļ��Ĵ�С����Ӧ�Ƴ�����
	// ����Ҫ���ض���ʼ������
	// �������ڴ洢���õ�ע�����
	// TODO: Ӧ�ʵ��޸ĸ��ַ�����
	// �����޸�Ϊ��˾����֯��
	SetRegistryKey(_T("Ӧ�ó��������ɵı���Ӧ�ó���"));


	::InitSkinMagicLib( AfxGetInstanceHandle(), "IP�������¼���������" , 
		NULL,
		NULL );

	::LoadSkinFromResource(NULL, (LPSTR)IDR_SMF, "SMF" );


	m_hMutex=CreateMutex( FALSE, FALSE, "IP�������¼���������"); 
	if ( m_hMutex!= NULL ) 
	{ 
		DWORD err = GetLastError(); 
		if ( err == ERROR_ALREADY_EXISTS )       
		{
			AfxMessageBox("�����Ѿ����У�",MB_OK,MB_ICONERROR);
			return FALSE;   
		}
	} 

	RunCrashHandler();//����dump�����Ѽ��Ӧ�ó������ʱ������dump�ļ��ѽ��д���λ

	CLoginDlg dlg;
	if (dlg.DoModal()==IDOK)
	{
		CRecordPlayerDlg dlg;
		m_pMainWnd = &dlg;
		INT_PTR nResponse = dlg.DoModal();
		if (nResponse == IDOK)
		{
			// TODO: �ڴ˷��ô����ʱ��
			//  ��ȷ�������رնԻ���Ĵ���
		}
		else if (nResponse == IDCANCEL)
		{
			// TODO: �ڴ˷��ô����ʱ��
			//  ��ȡ�������رնԻ���Ĵ���
		}
	}
	

	
	// ���ڶԻ����ѹرգ����Խ����� FALSE �Ա��˳�Ӧ�ó���
	//  ����������Ӧ�ó������Ϣ�á�
	return FALSE;
}

int CRecordPlayerApp::ExitInstance()
{
	// TODO: �ڴ����ר�ô����/����û���
    ExitSkinMagicLib();
	return CWinAppEx::ExitInstance();
}
