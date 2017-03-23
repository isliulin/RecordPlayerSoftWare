#pragma once
#include <afxinet.h>

#define DIS_CONNECT_EVENT           0x01  //�������
#define DOWNLOAD_FINISH_EVENT           0x02  //�������
 
#define  WM_FTP_EVENT_MSG        WM_APP+5001  //�¼���Ϣ


class CMyFtpClient
{
public:  
	CMyFtpClient(void);  
	CMyFtpClient(CString str_ip, CString str_user, CString str_pwd, UINT port);  
	virtual ~CMyFtpClient(void);  

	void setParm(CString str_ip, CString str_user, CString str_pwd, UINT port);  
	void setPassive(BOOL state){ m_passive = state;}//�趨FTP Passive  

	static bool login();//��¼  
	static bool logout();//�˳�  

	//�ϴ��ļ�  
	static DWORD WINAPI UploadFile(LPVOID lpParameter);  
	bool UploadFileThread(CString str_remoteFileName/*Զ���ļ����������ǵ�ǰ·���µ��ļ�*/,  
		CString str_LocalFileName/*�����ļ�����ȫ·��*/);  

	//�ϴ��ļ��б������ο� UploadFile  
	bool UploadFileList(CStringList str_remoteFileNameList, CStringList str_LocalFileList);//�ϴ�  

	//����Զ�̵�ǰ·��  
	bool SetRemoteDir(CString str_remoteDir);  

	//����Զ�̵�ǰ·��Ϊ��Ŀ¼  
	bool SetRemoteRoot(){ return SetRemoteDir(_T("/"));}  

	//�õ�Զ�̵�ǰ·��  
	CString GetRemoteDir(){    
		m_pConnect->GetCurrentDirectory(m_remoteCurrentDir);   
		return m_remoteCurrentDir; }  

	//����Զ��Ŀ¼������ı䵱ǰԶ��·��  
	bool CreateRemoteDir(CString str_dirName);  

	//ɾ��Զ��Ŀ¼��Ŀ¼�µ����ļ�,���ļ���  
	bool DelRemoteDir(CString str_dirName);  

	//ɾ��Զ���ļ�  
	bool DelRemoteFile(CString str_FileName);  

	//������Զ���ļ�  
	bool RenameRemoteFile(CString str_OldFileName,CString str_NewFileName);  


	//����Զ���ļ�  
	static DWORD WINAPI DownloadFile(LPVOID lpParameter);
    
	bool DownloadFileThread(CString str_remoteFileName/*Զ���ļ���*/, CString str_LocalFileName/*�����ļ���*/,__int64 nStartPos=0,__int64 nEndPos=0);
	//bool DownloadFile(CString str_remoteFileName/*Զ���ļ���*/, CString str_LocalFileName/*�����ļ���*/);  

	//����Զ���ļ��б�  
	bool DownloadFileList(CStringList str_remoteFileNameList, CStringList str_LocalFileList);  

	//�õ�������Ϣ  
	CString GetErrorMsg();  

	//�ϴ����Ȱٷֱ�  
	DWORD GetUploadFilePercent();  

	//�ϴ��ٶ� bytes/s  
	DWORD  GetUploadSpeed();  

	//���ؽ��Ȱٷֱ�  
	DWORD GetDownloadFilePercent();  

	//�����ٶ� bytes/s  
	DWORD  GetDownloadSpeed();  

	//�趨�ļ�ÿ�ζ�д��С,����Խ�죬ֵ����Խ��  
	void SetFileStepSize(DWORD dSize){ m_FileLenStep = dSize*1024; }  

	// ȡ��Ŀ¼�µ������ļ�  
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

	HANDLE m_hDownLoadThread;//�����߳̾��
	HWND m_hWnd;

	CRITICAL_SECTION m_csProject; //�ٽ���

	static ULONGLONG GetFileTotalSize(CString strFileName);//��ȡ�ļ��ܳ�

	//����CFtpFindFile�ļ���API����FindFile��recycle_ͨ�����ļ�ʱ�����ڵ���ʱ�����������
	//�����ò���һ�α�����Ŀ¼�ļ�������
	 static CStringArray m_strFileNameArray;
	 static BOOL  AddFTPRootFileNameArray();
	
};
