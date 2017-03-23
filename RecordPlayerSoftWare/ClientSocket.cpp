// ClientSocket.cpp : 实现文件
//

#include "stdafx.h"
#include "RecordPlayerSoftWare.h"
#include "ClientSocket.h"


// CClientSocket

CClientSocket::CClientSocket()
{
}

CClientSocket::~CClientSocket()
{
}


// CClientSocket 成员函数

void CClientSocket::OnClose(int nErrorCode)
{
	// TODO: 在此添加专用代码和/或调用基类

	CAsyncSocket::OnClose(nErrorCode);
}

void CClientSocket::OnConnect(int nErrorCode)
{
	// TODO: 在此添加专用代码和/或调用基类
	if(nErrorCode   ==   0)   
	{
		//连接成功，通知主窗口 
		//AfxMessageBox("连接成功");
		//m_bSuccessConnect=true;
		m_gGlobalVF.m_bConnServer=TRUE;
		m_gGlobalVF.WriteLog("同步连接成功");

	}
	else   
	{
		//m_bSuccessConnect=	false;
		//AfxMessageBox("连接失败");
		m_gGlobalVF.m_bConnServer=FALSE;


	}

	CAsyncSocket::OnConnect(nErrorCode);
}

void CClientSocket::OnSend(int nErrorCode)
{
	// TODO: 在此添加专用代码和/或调用基类

	CAsyncSocket::OnSend(nErrorCode);
}

void CClientSocket::OnReceive(int nErrorCode)
{
	// TODO: 在此添加专用代码和/或调用基类

	CAsyncSocket::OnReceive(nErrorCode);
}
