// Log.cpp: implementation of the CLog class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <string>
using namespace std;
#include "Log.h"
#include <Shlwapi.h>  //XP����

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CLog::CLog()
{
	//������ʼ��
	m_file.m_hFile = CFile::hFileNull;
	//����־�ļ� | CFile::shareDenyNone
	TCHAR lpPath[MAX_PATH];
	GetModuleFileName(NULL, lpPath, MAX_PATH);
	m_strFileName = lpPath;
	m_strFileName = m_strFileName.Left(m_strFileName.ReverseFind('\\') + 1);
	
	CString strfilename;
	GetFileNameCurTime(strfilename);
	m_strFileName +="Log\\";

	//�����ļ���
	if (!::PathIsDirectory(m_strFileName))
	{
		//�����ļ���
		if (CreateDirectory(m_strFileName,NULL)==FALSE)
		{
			AfxMessageBox("������־�ļ���ʧ��",MB_OK,MB_ICONERROR);
			return;
		}

	}
	m_strFileName+=strfilename;


}



CLog::~CLog()
{

	if (m_file.m_hFile != CFile::hFileNull)
	{
		m_file.Close();
		m_file.m_hFile==CFile::hFileNull;
	}
}

BOOL CLog::OpenLog()
{
	ASSERT(m_file.m_hFile == CFile::hFileNull);
	BOOL ret=m_file.Open(m_strFileName, CFile::modeCreate | CFile::modeNoTruncate \
		| CFile::modeReadWrite | CFile::shareDenyWrite);

	return ret;
}

void CLog::CloseLog()
{

	if (m_file.m_hFile != CFile::hFileNull)
	{
		m_file.Close();
		m_file.m_hFile== CFile::hFileNull;
	}
}


void CLog::GetCurTime(CString &strCurTime)
{
	SYSTEMTIME stCurTime;
	GetLocalTime(&stCurTime);
	strCurTime.Format(_T("[%04d-%02d-%02d %02d:%02d:%02d] "),stCurTime.wYear, stCurTime.wMonth, stCurTime.wDay,
				stCurTime.wHour, stCurTime.wMinute, stCurTime.wSecond);
	
}


void CLog::GetFileNameCurTime(CString &strCurTimeFileName)
{

	SYSTEMTIME stCurTime;
	GetLocalTime(&stCurTime);
	strCurTimeFileName.Format(_T("������־-%4d-%2d.log"),stCurTime.wYear, stCurTime.wMonth);
	strCurTimeFileName.Replace(' ', '0');
}


void CLog::Write(CHAR *lpBuf, BOOL bEnter)
{
	ASSERT(m_file.m_hFile != CFile::hFileNull);
	if (m_file.m_hFile == CFile::hFileNull)
		return;
	if (strlen(lpBuf) > 0)
	{
		CString strTemp;
		GetCurTime(strTemp);
		//strTemp += _T(" : ");
		m_file.SeekToEnd();
		m_file.WriteString(strTemp);
		m_file.Write(lpBuf, strlen(lpBuf));
		if (bEnter && lpBuf[strlen(lpBuf) - 1] != '\n')
			m_file.Write("\n", 1);
		m_file.Flush();
	}
}

void CLog::Write(const CString &strLog, BOOL bEnter)
{
	ASSERT(m_file.m_hFile != CFile::hFileNull);
	if (m_file.m_hFile == CFile::hFileNull)
		return;
	if (!strLog.IsEmpty())
	{
		
		CString strTemp;
		GetCurTime(strTemp);
		strTemp=strTemp;
		strTemp += strLog;
		if (bEnter && strTemp.GetAt(strTemp.GetLength() - 1) != '\n')
			strTemp += _T("\n");
		m_file.SeekToEnd();
		m_file.WriteString(strTemp);
		m_file.Flush();
	}
}
