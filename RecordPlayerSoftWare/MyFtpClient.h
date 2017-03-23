#pragma once
#include <afxinet.h>

#define DIS_CONNECT_EVENT           0x01  //网络掉线
#define DOWNLOAD_FINISH_EVENT           0x02  //下载完成
 
#define  WM_FTP_EVENT_MSG        WM_APP+5001  //事件消息


class CMyFtpClient
{
public:  
	CMyFtpClient(void);  
	CMyFtpClient(CString str_ip, CString str_user, CString str_pwd, UINT port);  
	virtual ~CMyFtpClient(void);  

	void setParm(CString str_ip, CString str_user, CString str_pwd, UINT port);  
	void setPassive(BOOL state){ m_passive = state;}//设定FTP Passive  

	static bool login();//登录  
	static bool logout();//退出  

	//上传文件  
	static DWORD WINAPI UploadFile(LPVOID lpParameter);  
	bool UploadFileThread(CString str_remoteFileName/*远程文件名，必须是当前路径下的文件*/,  
		CString str_LocalFileName/*本地文件名，全路径*/);  

	//上传文件列表，参数参考 UploadFile  
	bool UploadFileList(CStringList str_remoteFileNameList, CStringList str_LocalFileList);//上传  

	//设置远程当前路径  
	bool SetRemoteDir(CString str_remoteDir);  

	//设置远程当前路径为根目录  
	bool SetRemoteRoot(){ return SetRemoteDir(_T("/"));}  

	//得到远程当前路径  
	CString GetRemoteDir(){    
		m_pConnect->GetCurrentDirectory(m_remoteCurrentDir);   
		return m_remoteCurrentDir; }  

	//创建远程目录，不会改变当前远程路径  
	bool CreateRemoteDir(CString str_dirName);  

	//删除远程目录及目录下的子文件,子文件夹  
	bool DelRemoteDir(CString str_dirName);  

	//删除远程文件  
	bool DelRemoteFile(CString str_FileName);  

	//重命名远程文件  
	bool RenameRemoteFile(CString str_OldFileName,CString str_NewFileName);  


	//下载远程文件  
	static DWORD WINAPI DownloadFile(LPVOID lpParameter);
    
	bool DownloadFileThread(CString str_remoteFileName/*远程文件名*/, CString str_LocalFileName/*本地文件名*/,__int64 nStartPos=0,__int64 nEndPos=0);
	//bool DownloadFile(CString str_remoteFileName/*远程文件名*/, CString str_LocalFileName/*本地文件名*/);  

	//下载远程文件列表  
	bool DownloadFileList(CStringList str_remoteFileNameList, CStringList str_LocalFileList);  

	//得到错误信息  
	CString GetErrorMsg();  

	//上传进度百分比  
	DWORD GetUploadFilePercent();  

	//上传速度 bytes/s  
	DWORD  GetUploadSpeed();  

	//下载进度百分比  
	DWORD GetDownloadFilePercent();  

	//下载速度 bytes/s  
	DWORD  GetDownloadSpeed();  

	//设定文件每次读写大小,网速越快，值设置越大  
	void SetFileStepSize(DWORD dSize){ m_FileLenStep = dSize*1024; }  

	// 取得目录下的所有文件  
	void GetFileInPath(LPCTSTR lpszPath, CStringArray &arrstrFile,   
		BOOL bFindSubDir/* = FALSE*/,  
		LPCTSTR lpszFileExt);  

	void PauseDownLoad();
	void ResumeDownLoad();

	void SetMsgWnd(HWND& wnd);

	void SetLoginFlag(BOOL flag);

	


private:  
	static BOOL m_passive;  
	static CString m_errorMsg;  
	static BOOL m_bLogin;  
	CString m_remoteCurrentDir;  

	static CFile m_CFile;  


	static CInternetSession* m_session;  
	
	static CFtpConnection * m_pConnect;  
	static CInternetFile * m_pRemoteFile;  

	static CString m_Ip,m_user,m_pwd;  
	static UINT m_port;  
	static DWORD m_FileLenStep;  

	static ULONGLONG m_TotalUploadSize, m_TotalDownloadSize;  
	static ULONGLONG m_CurUploadSize, m_CurDownloadSize;  
	static ULONGLONG m_nStartPos,m_nEndPos;

	ULONGLONG m_UploadSpeed, m_DownloadSpeed;  

	void Init();  
	static void CloseRemoteFile();  
	static void SetErrorMsg();  

	static CString m_str_remoteFileName, m_str_LocalFileName;  

	HANDLE m_hDownLoadThread;//下载线程句柄
	HWND m_hWnd;

	CRITICAL_SECTION m_csProject; //临界区

	static ULONGLONG GetFileTotalSize(CString strFileName);//获取文件总长

	//由于CFtpFindFile文件的API函数FindFile在recycle_通道号文件时，存在调用时间过长的问题
	//因此最好采用一次遍历根目录文件的问题
	 static CStringArray m_strFileNameArray;
	 static BOOL  AddFTPRootFileNameArray();
	
};
