
// RecordPlayerSoftWare.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CRecordPlayerApp:
// �йش����ʵ�֣������ RecordPlayerSoftWare.cpp
//

class CRecordPlayerApp : public CWinAppEx
{
public:
	CRecordPlayerApp();

// ��д
	public:
	virtual BOOL InitInstance();

// ʵ��
	HANDLE m_hMutex;

	DECLARE_MESSAGE_MAP()
	virtual int ExitInstance();
};

extern CRecordPlayerApp theApp;