// ClientSocket.cpp : ʵ���ļ�
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


// CClientSocket ��Ա����

void CClientSocket::OnClose(int nErrorCode)
{
	// TODO: �ڴ����ר�ô����/����û���

	CAsyncSocket::OnClose(nErrorCode);
}

void CClientSocket::OnConnect(int nErrorCode)
{
	// TODO: �ڴ����ר�ô����/����û���
	if(nErrorCode   ==   0)   
	{
		//���ӳɹ���֪ͨ������ 
		//AfxMessageBox("���ӳɹ�");
		//m_bSuccessConnect=true;
		m_gGlobalVF.m_bConnServer=TRUE;
		m_gGlobalVF.WriteLog("ͬ�����ӳɹ�");

	}
	else   
	{
		//m_bSuccessConnect=	false;
		//AfxMessageBox("����ʧ��");
		m_gGlobalVF.m_bConnServer=FALSE;


	}

	CAsyncSocket::OnConnect(nErrorCode);
}

void CClientSocket::OnSend(int nErrorCode)
{
	// TODO: �ڴ����ר�ô����/����û���

	CAsyncSocket::OnSend(nErrorCode);
}

void CClientSocket::OnReceive(int nErrorCode)
{
	// TODO: �ڴ����ר�ô����/����û���

	CAsyncSocket::OnReceive(nErrorCode);
}
