/********************************************************************
	created:	2015/04/03

	author:		尹新华
	
	purpose:	程序运行日志
*********************************************************************/
// Log.h: interface for the CLog class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_LOG_H__E4E4D1E6_0D93_4193_8E5A_0B5AD255B21A__INCLUDED_)
#define AFX_LOG_H__E4E4D1E6_0D93_4193_8E5A_0B5AD255B21A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CLog  
{
public:
	void Write(const CString &strLog, BOOL bEnter = TRUE);
	void Write(CHAR *lpBuf, BOOL bEnter = TRUE);
	BOOL OpenLog();
	void CloseLog();

	
	CLog();
	virtual ~CLog();

private:
	CStdioFile		m_file;			//日志文件
	CString			m_strFileName;	//日志文件路径及文件名
protected:
	void GetCurTime(CString &strCurTime);
	void GetFileNameCurTime(CString &strCurTimeFileName); //生成文件名
};

#endif // !defined(AFX_LOG_H__E4E4D1E6_0D93_4193_8E5A_0B5AD255B21A__INCLUDED_)
