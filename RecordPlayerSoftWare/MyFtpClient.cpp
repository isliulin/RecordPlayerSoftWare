#include "StdAfx.h"
#include "MyFtpClient.h"
#include <io.h>
#include <fcntl.h>
//////////////////////////////////////////////////////////////////////  
// Construction/Destruction  
//////////////////////////////////////////////////////////////////////  
ULONGLONG CMyFtpClient::m_TotalUploadSize = 0, CMyFtpClient::m_TotalDownloadSize = 0;  
ULONGLONG CMyFtpClient::m_CurUploadSize = 0, CMyFtpClient::m_CurDownloadSize = 0;  
DWORD CMyFtpClient::m_FileLenStep = 1024*1024;  
ULONGLONG CMyFtpClient::m_nStartPos=0,CMyFtpClient::m_nEndPos=0;  
  
CFtpConnection * CMyFtpClient::m_pConnect = NULL;  
CInternetFile * CMyFtpClient::m_pRemoteFile = NULL;  
CInternetSession* CMyFtpClient::m_session=NULL;  
  
CFile CMyFtpClient::m_CFile;  
  
CString CMyFtpClient::m_str_remoteFileName = "", CMyFtpClient::m_str_LocalFileName = "";  
CString CMyFtpClient::m_errorMsg = "";  
CString CMyFtpClient::m_Ip = "", CMyFtpClient::m_user = "", CMyFtpClient::m_pwd = "";  
BOOL CMyFtpClient::m_passive = FALSE; //主动登陆：False为被动登陆
BOOL CMyFtpClient::m_bLogin = FALSE;  
UINT CMyFtpClient::m_port = 21;   //主动: 被动登陆端口为20 
  
CStringArray CMyFtpClient::m_strFileNameArray;
  
CMyFtpClient::CMyFtpClient(void)  
{  
    Init();  
}  
  
CMyFtpClient::~CMyFtpClient(void)  
{  
	m_hWnd=NULL;
    logout();  
	if (m_session)
	{
		m_session->Close();
		delete m_session;
		m_session=NULL;
	}
}  
  
CMyFtpClient::CMyFtpClient(CString str_ip, CString str_user, CString str_pwd, UINT port)  
{  
    m_Ip = str_ip;  
    m_user = str_user;  
    m_pwd = str_pwd;  
    m_port = port;  
  
    Init();  
}  
  
void CMyFtpClient::Init()  
{  
    m_pConnect = NULL;  
    m_pRemoteFile = NULL;  
	m_session=new CInternetSession;
    m_FileLenStep = 1024*1024;  
  
  
    m_CurUploadSize = m_TotalUploadSize = m_CurDownloadSize = m_TotalDownloadSize = 0;  
  
    setPassive(FALSE);  
	//setPassive(TRUE);  

	m_hDownLoadThread=INVALID_HANDLE_VALUE;
	m_hWnd=NULL;

	InitializeCriticalSection(&m_csProject);
}  
  
void CMyFtpClient::setParm(CString str_ip, CString str_user, CString str_pwd, UINT port)  
{  
  
    m_Ip = str_ip;  
    m_user = str_user;  
    m_pwd = str_pwd;  
    m_port = port;  
  
}  


  
bool CMyFtpClient::login()  
{  
  
  
    try  
    {  
  
		//对于服务器断开的情况怎么处理:由于连接上之后，FTP的文件传送不需要多次连接
        if(m_bLogin)  
		{
			//return true; //循环录音存在如果连接时间过长时，导致下载很慢，因此对于每一文件下载进行重连 
			logout();
		}
        //m_pConnect = m_session.GetFtpConnection( m_Ip, m_user,  
            //m_pwd, m_port, m_passive);  
        m_pConnect = m_session->GetFtpConnection( m_Ip, m_user,  
            m_pwd, 21, m_passive);  
        if(m_pConnect == NULL)  
        {  
            SetErrorMsg();  
            return false;  
        }  
    }  
    catch(CInternetException *pEx)  
    {  

		char ErrorMsg[1024];
        pEx->GetErrorMessage(ErrorMsg, 1024 );
       // pEx->Delete(); 
		//AfxMessageBox(ErrorMsg,MB_OK,MB_ICONERROR);
		if (m_pConnect)
		{
			delete m_pConnect;  
			m_pConnect = NULL;  
		}
       //当服务器没启动时，不会导致出现m_session句柄无效情况
      //  m_session->Close();  
        return false;  
    }  
    catch (...)  
    {  
        SetErrorMsg();  
		if (m_pConnect)
		{
			delete m_pConnect;  
			m_pConnect = NULL;  
		}

       // m_session->Close();  
        return false;  
    }  
  
    m_bLogin = TRUE;  

	//登陆成功后，对根目录下的文件进行遍历

   // AddFTPRootFileNameArray();

    return true;  
  
}  

BOOL CMyFtpClient::AddFTPRootFileNameArray()
{
	//文件遍历
	CFtpFileFind ftpFind(m_pConnect);
	
	BOOL flag=ftpFind.FindFile("*");
	if (!flag)
	{
		AfxMessageBox("没有录音文件",MB_OK,MB_ICONWARNING);
		ftpFind.Close();
		return FALSE;
	}

	

	while(flag)
	{

		flag=ftpFind.FindNextFile();

		CString FileName=ftpFind.GetFileName();

	    char* pNewFileName=new char[255];
		memset(pNewFileName,0,255);

		strcpy(pNewFileName,FileName.GetBuffer(0));

		m_strFileNameArray.Add(pNewFileName);

	}

	ftpFind.Close();

	

}
bool CMyFtpClient::logout()  
{  
    try  
    {  

		//写保护:对于频繁的登陆和注销操作，不能涉及文件关闭操作，否则容易导致文件崩溃
		/*
        if(m_pRemoteFile)  
        {  
            m_pRemoteFile->Close();  
            delete m_pRemoteFile;  
            m_pRemoteFile = NULL;  
        }  
		*/
		
  
        if(m_pConnect)  
        {  
            m_pConnect->Close();  
            delete m_pConnect;  
            m_pConnect = NULL;  
        }  

		/*
		if (m_session)
		{
			m_session->Close();
			delete m_session;
			m_session=NULL;
		}
		*/
  
        m_bLogin = FALSE;  
  
        //m_session.Close();  
  
    }  
    catch(CInternetException *pEx)  
    {  
        pEx->GetErrorMessage( m_errorMsg.GetBuffer(0), 1024 );  
        pEx->Delete();  
        delete m_pRemoteFile;  
        m_pRemoteFile = NULL;  
        delete m_pConnect;  
        m_pConnect = NULL;  
        m_bLogin = FALSE;  
        return false;  
    }  
    catch (...)  
    {  
        SetErrorMsg();  
        return false;  
    }  
  
  
    

    m_bLogin = FALSE;  
    return true;  
}  

void CMyFtpClient::SetLoginFlag(BOOL flag)
{
     m_bLogin =flag;

}
  
void CMyFtpClient::CloseRemoteFile()  
{  
    try  
    {  

        if(m_pRemoteFile)  
        {  
            m_pRemoteFile->Close();  
            delete m_pRemoteFile;  
            m_pRemoteFile = NULL;  
        }  
    }  
    catch(CInternetException *pEx)  
    {  
        pEx->GetErrorMessage( m_errorMsg.GetBuffer(0), 1024 );  
        pEx->Delete();  
    }  
    catch (...)  
    {  
        SetErrorMsg();  
    }  
  
  
}  
  
bool CMyFtpClient::UploadFileThread(CString str_remoteFileName, CString str_LocalFileName)  
{  
  
    DWORD      _i;  
  
    m_str_remoteFileName = str_remoteFileName;  
    m_str_LocalFileName = str_LocalFileName;  
  
    m_CurUploadSize = m_TotalUploadSize = m_CurDownloadSize = m_TotalDownloadSize = 0;  
  
    ::CreateThread (NULL, 0, UploadFile, this, 0,&_i);  
  
    return 1;  
}  
  
DWORD CMyFtpClient::UploadFile(LPVOID lpParameter)  
{  
  
  
    BOOL bOpen = FALSE;  
  
    char *pBuf = NULL;  
  
  
    try  
    {  
        if(m_pConnect == NULL)  
        {  
            if ( !login() )  
            {  
                return false;  
            }  
        }  
  
        m_pRemoteFile =  m_pConnect->OpenFile(m_str_remoteFileName, GENERIC_WRITE);  
        if(m_pRemoteFile == NULL)  
        {  
            AfxMessageBox(m_str_remoteFileName + "  :远程文件创建失败",MB_OK,MB_ICONERROR);  
            return false;  
        }  
  
  
        bOpen = m_CFile.Open( m_str_LocalFileName , CFile::modeRead );  
        if(!bOpen)  
        {  
            AfxMessageBox(m_str_LocalFileName + "  :本地文件打开失败",MB_OK,MB_ICONERROR);  
            return false;  
        }  
  
        m_TotalUploadSize =  m_CFile.GetLength(); 
		m_CurUploadSize=0;
  
  
       
        m_FileLenStep = m_FileLenStep > m_TotalUploadSize ? m_TotalUploadSize : m_FileLenStep;  
  
        DWORD dwRead = m_FileLenStep;  
  
        pBuf = new char[m_FileLenStep+1];  
        memset(pBuf, 0x00, m_FileLenStep+1);  
  
        while( (dwRead = m_CFile.Read(pBuf, m_FileLenStep)) > 0 )  
        {  
            m_pRemoteFile->Write( pBuf, dwRead );  
  
            m_CurUploadSize += dwRead;  
        }  
  
        delete pBuf;          
        m_CFile.Close();  
  
    }  
    catch(CInternetException *pEx)  
    {  
        pEx->GetErrorMessage( m_errorMsg.GetBuffer(0), 1024 );  
        pEx->Delete();  
  
        delete pBuf;  
        if(bOpen)  
            m_CFile.Close();  
        return false;  
    }  
    catch (...)  
    {  
        SetErrorMsg();  
        delete pBuf;  
        if(bOpen)  
            m_CFile.Close();  
  
        return false;  
    }  
  
    CloseRemoteFile();  
  
    return true;  
  
  
}  
bool CMyFtpClient::UploadFileList(CStringList str_remoteFileNameList, CStringList str_LocalFileList)  
{  
    for(int i = 0; i< str_remoteFileNameList.GetCount(); i++)  
    {  
        CString remoteFile = str_remoteFileNameList.GetAt(str_remoteFileNameList.FindIndex(i));  
        CString localFile = str_LocalFileList.GetAt(str_LocalFileList.FindIndex(i));  
  
        UploadFileThread( remoteFile, localFile );  
    }  
  
    return true;  
}  
bool CMyFtpClient::CreateRemoteDir(CString str_dirName)  
{  
    try  
    {  
        if(m_pConnect == NULL)  
            return false;  
  
        if ( m_pConnect->CreateDirectory(str_dirName) <= 0 )  
            return false;  
  
        return SetRemoteDir(str_dirName);  
  
  
    }  
    catch(CInternetException *pEx)  
    {  
        pEx->GetErrorMessage( m_errorMsg.GetBuffer(0), 1024 );  
        pEx->Delete();  
        return false;  
    }  
    catch (...)  
    {  
        SetErrorMsg();  
        return false;  
    }  
  
  
  
    return true;  
}  
  
bool CMyFtpClient::SetRemoteDir(CString str_remoteDir)  
{   
    try  
    {  
        if(m_pConnect == NULL)  
            return false;  
  
  
        if( m_pConnect->SetCurrentDirectory( str_remoteDir) <= 0 )  
                return false;  
  
        m_pConnect->GetCurrentDirectory(m_remoteCurrentDir);  
  
  
    }  
    catch(CInternetException *pEx)  
    {  
        pEx->GetErrorMessage( m_errorMsg.GetBuffer(0), 1024 );  
        pEx->Delete();  
        return false;  
    }  
    catch (...)  
    {  
        SetErrorMsg();  
        return false;  
    }  
  
    return true;  
  
}  
  
bool CMyFtpClient::DelRemoteDir(CString str_remoteDir/*全路径或当前路径下的文件夹*/)  
{   
  
_RECALL:  
    CFtpFileFind ftpFind(m_pConnect);  
  
  
    try  
    {  
        if(m_pConnect == NULL)  
            return false;  
  
        if(str_remoteDir.Left(1) != "/")//不是全路径  
        {  
            CString strPath;  
            m_pConnect->GetCurrentDirectory(strPath);  
            str_remoteDir = strPath + _T("/") + str_remoteDir + _T("/*");  
        }  
  
        SetRemoteDir(str_remoteDir);  
  
  
  
  
        BOOL bContinue = ftpFind.FindFile(str_remoteDir);  
  
  
        if (!bContinue)//是个空目录，删除  
        {  
            SetRemoteDir("..");//返回上层  
            m_pConnect->RemoveDirectory(str_remoteDir);//删除  
            goto _QUIT;  
        }  
  
  
  
        bContinue = ftpFind.FindFile();  
        if (!bContinue)//文件未找到  
        {  
            goto _QUIT;  
        }  
  
  
        do   
        {  
            bContinue =  ftpFind.FindNextFile();  
  
            if (ftpFind.IsDirectory())  
            {  
                CString strPath = ftpFind.GetFilePath();  
                ftpFind.Close();  
  
                DelRemoteDir( strPath );//递归调用  
                SetRemoteDir("..");//返回上层  
                goto _RECALL;  
            }  
            else  
            {                 
                DelRemoteFile(ftpFind.GetFileName());  
            }  
        }  
        while (bContinue);  
  
  
    }  
    catch(CInternetException *pEx)  
    {  
        pEx->GetErrorMessage( m_errorMsg.GetBuffer(0), 1024 );  
        pEx->Delete();  
        goto _FAIL;  
    }  
    catch (...)  
    {  
        SetErrorMsg();  
        goto _FAIL;  
    }  
  
  
_QUIT:  
    ftpFind.Close();  
    return true;  
  
_FAIL:  
  
    ftpFind.Close();  
    return false;  
}  
  
bool CMyFtpClient::DelRemoteFile(CString str_FileName)  
{   
    try  
    {  
        if(m_pConnect == NULL)  
            return false;  
  
        if( m_pConnect->Remove( str_FileName ) <= 0 )  
            return false;  
  
  
    }  
    catch(CInternetException *pEx)  
    {  
        pEx->GetErrorMessage( m_errorMsg.GetBuffer(0), 1024 );  
        pEx->Delete();  
        return false;  
    }  
    catch (...)  
    {  
        SetErrorMsg();  
        return false;  
    }  
  
    return true;  
  
}  
  
bool CMyFtpClient::RenameRemoteFile(CString str_OldFileName,CString str_NewFileName)  
{   
    try  
    {  
        if(m_pConnect == NULL)  
            return false;  
  
        if( m_pConnect->Rename( str_OldFileName,  str_NewFileName ) <= 0 )  
            return false;  
  
  
    }  
    catch(CInternetException *pEx)  
    {  
        pEx->GetErrorMessage( m_errorMsg.GetBuffer(0), 1024 );  
        pEx->Delete();  
        return false;  
    }  
    catch (...)  
    {  
        SetErrorMsg();  
        return false;  
    }  
  
    return true;  
  
}  

bool CMyFtpClient::DownloadFileThread(CString str_remoteFileName/*远程文件名*/, CString str_LocalFileName/*本地文件名*/,__int64 nStartPos,__int64 nEndPos)
{
	DWORD      _i;  

	m_str_remoteFileName = str_remoteFileName;  
	m_str_LocalFileName = str_LocalFileName;  

	 m_CurDownloadSize = m_TotalDownloadSize = 0;  


	 m_nStartPos=nStartPos;
	 m_nEndPos=nEndPos;


	 //对于

	 if (str_remoteFileName.Find("SPON")>=0)
	 {
		 //则为子目录
         this->m_pConnect->SetCurrentDirectory("SPON");
		 CFtpFileFind ftpFind(this->m_pConnect);
		 BOOL flag=ftpFind.FindFile("*");

		 if (!flag)
		 {
			 AfxMessageBox("没有录音文件",MB_OK,MB_ICONWARNING);
			 ftpFind.Close();
			 return FALSE;
		 }

		 BOOL bFileFindFlag=FALSE; //文件查找标志

		 int nPos=str_remoteFileName.ReverseFind('\\');

		 int nLength=str_remoteFileName.GetLength();
		 CString strremoteFileName=str_remoteFileName.Right(nLength-nPos-1);


		 while(flag)
		 {

			 flag=ftpFind.FindNextFile();

			 CString FileName=ftpFind.GetFileName();

			 
			 if (ftpFind.GetFileName()==strremoteFileName)
			 {
				 if ((nEndPos==0)&&(nStartPos==0))
				 {
					 m_TotalDownloadSize	= ftpFind.GetLength();
				 }
				 else
				 {
					 //由于是循环录音，因此存在起始位置大于结束位置
					 if (nStartPos>nEndPos)
					 {
						 //计算公式：nLength-nStartPos+nEndPos-1024
						 ULONGLONG nLen= ftpFind.GetLength();
						 m_TotalDownloadSize=(nLen-nStartPos)+(nEndPos-1024);

					 }
					 else
					 {
						 m_TotalDownloadSize   =nEndPos-nStartPos;
					 }

				 }

				 bFileFindFlag=TRUE;
				 break;
			 }

		 }

		 ftpFind.Close();

		 //回归根目录
		 this->m_pConnect->SetCurrentDirectory("..");

		

		 if (!bFileFindFlag)
		 { 
			 AfxMessageBox("下载文件不存在！",MB_OK,MB_ICONWARNING);

			 return FALSE;

		 }

	 }
	 else if(str_remoteFileName.Find(".db")>=0)
	 {

		 //则为根目录
		 //数据库文件
		 //解析状态
		 CFtpFileFind ftpFind(this->m_pConnect);
	
		 BOOL flag=ftpFind.FindFile("*");
		 if (!flag)
		 {
			 AfxMessageBox("没有录音文件",MB_OK,MB_ICONWARNING);
			 ftpFind.Close();
			 return FALSE;
		 }

		 BOOL bFileFindFlag=FALSE; //文件查找标志

		 while(flag)
		 {

			 flag=ftpFind.FindNextFile();


			 if (ftpFind.GetFileName()==str_remoteFileName)
			 {
				 //由于数据库文件，直接获取文件长度
				
			     m_TotalDownloadSize	= ftpFind.GetLength();

				 bFileFindFlag=TRUE;
				 break;
			 }

		 }

		 ftpFind.Close();

		 if (!bFileFindFlag)
		 { 
			 AfxMessageBox("下载文件不存在！",MB_OK,MB_ICONWARNING);

			 return FALSE;

		 }


	 }
	 else if (str_remoteFileName.Find(".ini")>=0)
	 {

		 //则为根目录
		 //数据库文件
		 //解析状态
		 CFtpFileFind ftpFind(this->m_pConnect);

		 BOOL flag=ftpFind.FindFile("*");
		 if (!flag)
		 {
			 AfxMessageBox("没有配置文件",MB_OK,MB_ICONWARNING);
			 ftpFind.Close();
			 return FALSE;
		 }

		 BOOL bFileFindFlag=FALSE; //文件查找标志

		 while(flag)
		 {

			 flag=ftpFind.FindNextFile();


			 if (ftpFind.GetFileName()==str_remoteFileName)
			 {
				 //由于数据库文件，直接获取文件长度

				 m_TotalDownloadSize	= ftpFind.GetLength();

				 bFileFindFlag=TRUE;
				 break;
			 }

		 }

		 ftpFind.Close();

		 if (!bFileFindFlag)
		 { 
			 AfxMessageBox("下载文件不存在！",MB_OK,MB_ICONWARNING);

			 return FALSE;

		 }
           
	 }
	 else if (str_remoteFileName.Find("recycle_")>=0)
	 {
		    //循环录音文件
		 
			 CFtpFileFind ftpFind(this->m_pConnect);
			 TRACE0("start Find File\n");
			 LARGE_INTEGER m_nFreq;
			 LARGE_INTEGER m_nBeginTime;
			 LARGE_INTEGER nEndTime;
			 QueryPerformanceFrequency(&m_nFreq); // 获取时钟周期
			 QueryPerformanceCounter(&m_nBeginTime); // 开始计时


			 BOOL flag=ftpFind.FindFile("*");
			 if (!flag)
			 {
				 AfxMessageBox("没有录音文件",MB_OK,MB_ICONWARNING);
				 ftpFind.Close();
				 return FALSE;
			 }

			 QueryPerformanceCounter(&nEndTime);// 结束计时
			 LONGLONG nMsTimer1=(nEndTime.QuadPart-m_nBeginTime.QuadPart)*1000/m_nFreq.QuadPart; //ms
			 TRACE1("msFindFile=%ld\n",nMsTimer1);

			 BOOL bFileFindFlag=FALSE; //文件查找标志

			 while(flag)
			 {

				 flag=ftpFind.FindNextFile();

				 CString FileName=ftpFind.GetFileName();

				 TRACE2("%s=%s\n",FileName,str_remoteFileName);

				 if (ftpFind.GetFileName()==str_remoteFileName)
				 {
					 //由于是循环录音，因此存在起始位置大于结束位置
					
						 //计算公式：nLength-nStartPos+nEndPos-1024
					 if(nStartPos<nEndPos) 
					 {
						 //正常文件大小
						 m_TotalDownloadSize   =nEndPos-nStartPos;
					 }
					 else if (nStartPos>nEndPos)
					 {
						 //文件循环
						 ULONGLONG nLen= ftpFind.GetLength();
						 m_TotalDownloadSize=(nLen-nStartPos)+(nEndPos-1024);
					 }
						
					 bFileFindFlag=TRUE;
					 break;
				 }

			 }

			 ftpFind.Close();

			 if (!bFileFindFlag)
			 { 
				 AfxMessageBox("下载文件不存在！",MB_OK,MB_ICONWARNING);

				 return FALSE;

			 }

	 }

    
	m_hDownLoadThread=::CreateThread (NULL, 0, DownloadFile, this, 0,&_i);  

	if (m_hDownLoadThread==INVALID_HANDLE_VALUE)
	{
		return FALSE;
	}
	return TRUE;  

}

void CMyFtpClient::PauseDownLoad()
{
       if (m_hDownLoadThread!=INVALID_HANDLE_VALUE)
       {
		   SuspendThread(m_hDownLoadThread);
       }

}

void CMyFtpClient::ResumeDownLoad()
{

	if (m_hDownLoadThread!=INVALID_HANDLE_VALUE)
	{
		ResumeThread(m_hDownLoadThread);
	}

}
  
DWORD CMyFtpClient::DownloadFile(LPVOID lpParameter)  
{  

	CMyFtpClient* pFtpClient=(CMyFtpClient*)lpParameter;

	EnterCriticalSection(&pFtpClient->m_csProject);
	CString str_remoteFileName=m_str_remoteFileName;
	CString str_LocalFileName=m_str_LocalFileName;
	LeaveCriticalSection(&pFtpClient->m_csProject);

    LPBYTE pBuffer = new BYTE[m_FileLenStep+1];  
    memset(pBuffer, 0, m_FileLenStep+1);  
  
    CFile wrireFile;  
  
    try  
    {  
        if(m_pConnect == NULL)  
        {  
            if ( !login() )  
            {  
                return false;  
            }  
        }  
  
        if( !wrireFile.Open(str_LocalFileName, CFile::modeCreate|CFile::modeWrite|CFile::typeBinary))  
        {  
            AfxMessageBox(str_LocalFileName + "  :文件创建失败",MB_OK,MB_ICONERROR);  
  
            CloseRemoteFile(); 

            return false;  
        }  
 
		//这样获取的文件总长度是不正确的
       // m_TotalDownloadSize = m_pRemoteFile->GetLength();//远程文件总长度  
  
	   
       UINT m_FileReadLen = m_FileLenStep > m_TotalDownloadSize ? m_TotalDownloadSize : m_FileLenStep;  
  
	   m_CurDownloadSize=0;


	   BOOL bSubDir=FALSE;

        DWORD dRead = 0;  

        if ((m_nEndPos==0)&&(m_nStartPos==0))
        {
			//事件录音文件和数据库文件读取
			if (str_remoteFileName.Find("SPON")>=0)
			{
				//事件录音
				//则为子目录
			   m_pConnect->SetCurrentDirectory("SPON");

			   bSubDir=TRUE;
			   //获取文件名
			   int nPos=str_remoteFileName.ReverseFind('\\');

			   int nLength=str_remoteFileName.GetLength();
			   CString strremoteFileName=str_remoteFileName.Right(nLength-nPos-1);

			   m_pRemoteFile = m_pConnect->OpenFile(strremoteFileName, GENERIC_READ);  
			   if(m_pRemoteFile == NULL)  
			   {  
				   AfxMessageBox(str_remoteFileName + "  :远程文件打开失败",MB_OK,MB_ICONERROR);  
				   pFtpClient->logout();//退出

				   return false;  
			   } 

			}
			else
			{
				m_pRemoteFile = m_pConnect->OpenFile(str_remoteFileName, GENERIC_READ);  
				if(m_pRemoteFile == NULL)  
				{  
					AfxMessageBox(str_remoteFileName + "  :远程文件打开失败",MB_OK,MB_ICONERROR);  
					pFtpClient->logout();//退出

					return false;  
				} 
			}
			
			EnterCriticalSection(&pFtpClient->m_csProject);

			while( (dRead = m_pRemoteFile->Read( pBuffer, m_FileReadLen)) > 0  )   
			{  
				wrireFile.Write( pBuffer, dRead );  
				m_CurDownloadSize += dRead;  

			}  
			LeaveCriticalSection(&pFtpClient->m_csProject);

			wrireFile.Close();  


			if (bSubDir)
			{
				m_pConnect->SetCurrentDirectory("..");
			}


			if ((m_CurDownloadSize!=m_TotalDownloadSize)&&(pFtpClient->m_hWnd!=NULL))
			{
				//TODO:
				
				PostMessage(pFtpClient->m_hWnd,WM_FTP_EVENT_MSG,DIS_CONNECT_EVENT,NULL);
			}
			else if ((m_CurDownloadSize==m_TotalDownloadSize)&&(pFtpClient->m_hWnd!=NULL))
			{
				PostMessage(pFtpClient->m_hWnd,WM_FTP_EVENT_MSG,DOWNLOAD_FINISH_EVENT,NULL);
			}

			



        }
		else
		{

			//循环录音文件读取
			//由于CInternetFile不支持FTP Seek,采用CinternetFile转File
			if (m_nStartPos<=m_nEndPos)
			{
				//循环录音正常读取
				CString strCmd;
				strCmd.Format("REST %I64u\r\n",m_nStartPos); //从文件的指定位置开始读
				
							
				CInternetFile* pCommadFile=m_pConnect->Command(strCmd,CFtpConnection::CmdRespNone);


				m_pRemoteFile = m_pConnect->OpenFile(str_remoteFileName, GENERIC_READ);  
				
			

				if(m_pRemoteFile == NULL)  
				{  
					AfxMessageBox(str_remoteFileName + "  :远程文件打开失败",MB_OK,MB_ICONERROR);  
					pFtpClient->logout();//退出
					
					return false;  
				} 

				

				EnterCriticalSection(&pFtpClient->m_csProject);

				while( (dRead =m_pRemoteFile->Read( pBuffer,m_FileReadLen)) > 0  )   
				{  
					wrireFile.Write( pBuffer, dRead );  
					m_CurDownloadSize += dRead; 

					if (m_CurDownloadSize>=m_TotalDownloadSize)
					{
						break;
					}

				}  
				LeaveCriticalSection(&pFtpClient->m_csProject);

				wrireFile.Close();  

				if ((m_CurDownloadSize<m_TotalDownloadSize)&&(pFtpClient->m_hWnd!=NULL))
				{
					//TODO:
					
					PostMessage(pFtpClient->m_hWnd,WM_FTP_EVENT_MSG,DIS_CONNECT_EVENT,NULL);
				}
				else if ((m_CurDownloadSize>=m_TotalDownloadSize)&&(pFtpClient->m_hWnd!=NULL))
				{
					PostMessage(pFtpClient->m_hWnd,WM_FTP_EVENT_MSG,DOWNLOAD_FINISH_EVENT,NULL);
				}
			}
			else
			{
				//循环录音产生回绕
				CString strCmd;
				strCmd.Format("REST %I64u\r\n",m_nStartPos); //从文件的指定位置开始读
				CInternetFile* pCommadFile=m_pConnect->Command(strCmd,CFtpConnection::CmdRespNone);

				m_pRemoteFile = m_pConnect->OpenFile(str_remoteFileName, GENERIC_READ);  
				if(m_pRemoteFile == NULL)  
				{  
					AfxMessageBox(str_remoteFileName + "  :远程文件打开失败",MB_OK,MB_ICONERROR);  
					pFtpClient->logout();//退出
					
					return false;  
				} 

				

				EnterCriticalSection(&pFtpClient->m_csProject);

				while( (dRead =m_pRemoteFile->Read( pBuffer,m_FileReadLen)) > 0  )   
				{  
					wrireFile.Write( pBuffer, dRead );  
					m_CurDownloadSize += dRead; 

					if (m_CurDownloadSize>=(m_TotalDownloadSize-m_nStartPos))
					{
						break;
					}

				}  
				LeaveCriticalSection(&pFtpClient->m_csProject);

               
				

				//下载第二段
				if (m_nEndPos>1024)
				{
					CloseRemoteFile();

					strCmd.Format("REST %ld\r\n",1024); //从文件的头开始读取

					CInternetFile* pCommadFile=m_pConnect->Command(strCmd,CFtpConnection::CmdRespNone);

					m_pRemoteFile = m_pConnect->OpenFile(str_remoteFileName, GENERIC_READ);  
					if(m_pRemoteFile == NULL)  
					{  
						AfxMessageBox(str_remoteFileName + "  :远程文件打开失败",MB_OK,MB_ICONERROR);  
						pFtpClient->logout();//退出
						
						return false;  
					} 

					EnterCriticalSection(&pFtpClient->m_csProject);

					while( (dRead =m_pRemoteFile->Read( pBuffer,m_FileReadLen)) > 0  )   
					{  
						wrireFile.Write( pBuffer, dRead );  
						m_CurDownloadSize += dRead; 

						if (m_CurDownloadSize>=m_TotalDownloadSize)
						{
							break;
						}

					}  
					LeaveCriticalSection(&pFtpClient->m_csProject);

				}

				wrireFile.Close();  
				

				if ((m_CurDownloadSize<m_TotalDownloadSize)&&(pFtpClient->m_hWnd!=NULL))
				{
					//TODO:
					
					PostMessage(pFtpClient->m_hWnd,WM_FTP_EVENT_MSG,DIS_CONNECT_EVENT,NULL);
				}
				else if ((m_CurDownloadSize>=m_TotalDownloadSize)&&(pFtpClient->m_hWnd!=NULL))
				{
					PostMessage(pFtpClient->m_hWnd,WM_FTP_EVENT_MSG,DOWNLOAD_FINISH_EVENT,NULL);
				}



			}
			


		}
		
    }  
    catch(CInternetException *pEx)  
    {  
        pEx->GetErrorMessage(m_errorMsg.GetBuffer(0), 1024 );  
		AfxMessageBox(m_errorMsg.GetBuffer(0));
       // pEx->Delete();  
  

        wrireFile.Close();  

		
        return false;  
    }  
    catch (...)  
    {  
        SetErrorMsg();  
  
        wrireFile.Close();  
		
        return false;  
    }  
  
  
  
  
    delete [] pBuffer;  
    CloseRemoteFile();  
  
  
    return true;  
  
  
  
  
  
}  

void CMyFtpClient::SetMsgWnd(HWND& wnd)
{
	m_hWnd=wnd;
}
bool CMyFtpClient::DownloadFileList(CStringList str_remoteFileNameList, CStringList str_LocalFileList)  
{  
    for(int i = 0; i< str_remoteFileNameList.GetCount(); i++)  
    {  
        CString remoteFile = str_remoteFileNameList.GetAt(str_remoteFileNameList.FindIndex(i));  
        CString localFile = str_LocalFileList.GetAt(str_LocalFileList.FindIndex(i));  
  
        DownloadFileThread( remoteFile, localFile );  
    }  
  
    return true;  
}  
  
DWORD CMyFtpClient::GetUploadFilePercent()  
{  
    if(m_TotalUploadSize <=0)  
        return 0;  
  
  
    double d1,d2,d3;  
    d2 = m_CurUploadSize;  
    d3 = m_TotalUploadSize;  
    d1 = d2  / d3;  
  
    return d1 * 100 ;     
}  
  
DWORD CMyFtpClient::GetDownloadFilePercent()  
{  
	if (m_TotalDownloadSize!=0)
	{
		 return m_CurDownloadSize * 100 / m_TotalDownloadSize;
	}
	else
	{
		//当总文件大小为0时
        return 0;
	}
     
}  
  
CString CMyFtpClient::GetErrorMsg()  
{  
    return "";  
    return m_errorMsg;  
}  
  
void CMyFtpClient::SetErrorMsg()  
{  
    return;  
  
    LPVOID lpMsgBuf;   
    FormatMessage(   
        FORMAT_MESSAGE_ALLOCATE_BUFFER |   
        FORMAT_MESSAGE_FROM_SYSTEM |   
        FORMAT_MESSAGE_IGNORE_INSERTS,   
        NULL,   
        GetLastError(),   
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language   
        (LPTSTR) &lpMsgBuf,   
        0,   
        NULL   
        );   
  
    m_errorMsg.Format("%s",lpMsgBuf);  
  
    LocalFree(   lpMsgBuf   );  
}  
  
  
// 判断路径是否存在  
BOOL IsPathExist(LPCTSTR lpszPathName)  
{  
    CString strTemp;  
    if(GetCurrentDirectory(MAX_PATH,strTemp.GetBuffer(MAX_PATH)) == 0 )  
    {  
        strTemp.ReleaseBuffer();  
        return FALSE;  
    }  
    strTemp.ReleaseBuffer(MAX_PATH);  
    BOOL bRes = SetCurrentDirectory(lpszPathName);  
    SetCurrentDirectory(strTemp.GetBuffer(MAX_PATH));  
    strTemp.ReleaseBuffer(MAX_PATH);  
    return bRes;  
}  
  
// 判断是否是路径  
BOOL IsDirPath(LPCTSTR lpszPathName)  
{  
    HANDLE hFindFile;  
    WIN32_FIND_DATA stuFindData;  
  
    CString strName(lpszPathName);  
    strName  += _T("\\*");  
    hFindFile = FindFirstFile(strName, &stuFindData);  
    if (hFindFile == INVALID_HANDLE_VALUE)  return FALSE;  
    else  
    {  
        FindClose(hFindFile);  
        return TRUE;  
    }  
}  
  
// 取得目录下的所有文件  
void CMyFtpClient::GetFileInPath(LPCTSTR lpszPath, CStringArray &arrstrFile,   
                                 BOOL bFindSubDir/* = FALSE*/, LPCTSTR lpszFileExt)  
{  
    if (!IsPathExist(lpszPath)) return ;  
  
    arrstrFile.RemoveAll();  
  
    WIN32_FIND_DATA     stuFindData;  
    HANDLE              hFindFile;  
    CString strName(lpszPath);  
  
    //modify by paul strName  += _T("\\*");  
    //strName  += _T(lpszFileExt);  
    strName  += _T("\\*");  
  
    hFindFile = FindFirstFile(strName, &stuFindData);  
  
    if (hFindFile == INVALID_HANDLE_VALUE)  return ;  
    else  
    {  
        strName = stuFindData.cFileName;  
        if (strName.CompareNoCase(_T(".")) != 0 && strName.CompareNoCase(_T("..")) != 0)  
        {  
            strName  = lpszPath;  
            strName += _T("\\");  
            strName += stuFindData.cFileName;  
            if (!IsDirPath(strName) && strName.CompareNoCase(_T(lpszFileExt)) != 0 )  
                arrstrFile.Add(strName);  
            else if (bFindSubDir)  
            {  
                CStringArray arrstrTemp;  
                GetFileInPath(strName, arrstrTemp, bFindSubDir,lpszFileExt);  
                for (int i=0; i<arrstrTemp.GetSize() && strName.CompareNoCase(_T(lpszFileExt)) != 0; i++)  
                    arrstrFile.Add(arrstrTemp[i]);  
                arrstrTemp.RemoveAll();  
            }  
        }  
        while (FindNextFile(hFindFile, &stuFindData))   
        {  
            strName = stuFindData.cFileName;  
            if (strName.CompareNoCase(_T(".")) != 0 && strName.CompareNoCase(_T("..")) != 0)  
            {  
                strName  = lpszPath;  
                strName += _T("\\");  
                strName += stuFindData.cFileName;  
                if (!IsDirPath(strName) && strName.CompareNoCase(_T(lpszFileExt)) != 0 )  
                    arrstrFile.Add(strName);  
                else if (bFindSubDir)  
                {  
                    CStringArray arrstrTemp;  
                    GetFileInPath(strName, arrstrTemp, bFindSubDir,lpszFileExt);  
                    for (int i=0; i<arrstrTemp.GetSize() && strName.CompareNoCase(_T(lpszFileExt)) != 0; i++)  
                        arrstrFile.Add(arrstrTemp[i]);  
                    arrstrTemp.RemoveAll();  
                }  
            }  
        }  
        FindClose(hFindFile);  
    }  
  
  
}  

 ULONGLONG CMyFtpClient::GetFileTotalSize(CString strFileName)
{
    if (m_pConnect)
    {
		m_pRemoteFile = m_pConnect->OpenFile(strFileName, GENERIC_READ);  
		if(m_pRemoteFile == NULL)  
		{  
			AfxMessageBox(strFileName + "  :远程文件打开失败",MB_OK,MB_ICONERROR);  
			return 0;  
		}  


	      
		 
				           
				           
				     
		      



    }

     
}
  
//上传速度 bytes/s  
DWORD  CMyFtpClient::GetUploadSpeed()  
{  
    /* 
    m_UploadSpeed = m_CurUploadSize; 
    Sleep(1000); 
    m_UploadSpeed = m_CurUploadSize - m_UploadSpeed; 
    return m_UploadSpeed; 
    */  
  
    return m_CurUploadSize;  
}  
  
//下载速度 bytes/s  
DWORD  CMyFtpClient::GetDownloadSpeed()  
{  
    /* 
    m_DownloadSpeed = m_CurDownloadSize; 
    Sleep(1000); 
    m_DownloadSpeed = m_CurDownloadSize - m_DownloadSpeed; 
    return m_DownloadSpeed; 
    */  
  
    return m_CurDownloadSize;  
}  