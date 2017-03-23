#include "StdAfx.h"
#include "GlobalVF.h"
#include "Log.h"
#include "Define.h"

CGlobalVF::CGlobalVF(void)
{

	m_ServerIP=_T("192.168.1.13");
	m_nServerPort=6788;
	InitializeCriticalSection(&m_gcslogMutex);
	InitializeCriticalSection(&m_gcsMutex);
	m_bConnServer=FALSE;
}

CGlobalVF::~CGlobalVF(void)
{

}

void CGlobalVF::WriteLog(CString str)
{
	EnterCriticalSection(&m_gcslogMutex);
	CLog log;
	log.OpenLog();
	log.Write(str);
	log.CloseLog();
	LeaveCriticalSection(&m_gcslogMutex);

}

void CGlobalVF::WriteConfigFile(void)
{
       //写配置
	EnterCriticalSection(&m_gcsMutex);
	CString		strINIFilePath = GetApplicationPath() + FILE_CONFIG;
	CString		strTemp;
	WritePrivateProfileString(_T("ServerInfo"), _T("m_ServerIP"), m_ServerIP, strINIFilePath);
	strTemp.Format("%d",m_nServerPort);
	WritePrivateProfileString(_T("ServerInfo"), _T("m_nServerPort"), strTemp, strINIFilePath);
	

	//写存储路径
    WritePrivateProfileString(_T("Path"), _T("m_FileSavePath"), m_FileSavePath, strINIFilePath);


	//写设备信息
	unsigned int nCount=m_gGlobalVF.m_lDevInfoList.GetSize();
	//写设备总数
	strTemp.Format("%d",nCount);
	WritePrivateProfileString(_T("DevInfo"), _T("DevNum"), strTemp, strINIFilePath);

	POSITION pos=m_gGlobalVF.m_lDevInfoList.GetHeadPosition();
	int num=0;
	CString strLeft;
	CString strRight;
	while (pos!=NULL)
	{
		  num++;
          SRecordDevInfo* pDevInfo=m_gGlobalVF.m_lDevInfoList.GetNext(pos);
		 
		  //写名称
		  strLeft.Format(_T("DevName%d"),num);
		  WritePrivateProfileString(_T("DevInfo"), strLeft, pDevInfo->m_sDevName, strINIFilePath);
		  //写ID
          strLeft.Format(_T("DevID%d"),num);
          strRight.Format("%d",pDevInfo->m_nDevID);
		  WritePrivateProfileString(_T("DevInfo"), strLeft, strRight, strINIFilePath);
		  //写IP地址
		  strLeft.Format(_T("DevIP%d"),num);
		  strRight.Format("%ul",pDevInfo->m_dDevIP);
		  WritePrivateProfileString(_T("DevInfo"), strLeft,strRight, strINIFilePath);
		  //写端口
          strLeft.Format(_T("DevPort%d"),num);
		  strRight.Format("%d",pDevInfo->m_nDevPort);
		  WritePrivateProfileString(_T("DevInfo"), strLeft,strRight, strINIFilePath);

	}

	LeaveCriticalSection(&m_gcsMutex);


}

void CGlobalVF::ReadConfigFile(void)
{
	//读取配置文件
	//读取配置信息
	CString		strINIFilePath = GetApplicationPath() + FILE_CONFIG;
	TCHAR		pBuf[MAX_PATH];
	memset(pBuf,0,sizeof(pBuf));

	GetPrivateProfileString(_T("ServerInfo"), _T("m_serverIP"), _T(""), pBuf, MAX_PATH, strINIFilePath);
	m_ServerIP = pBuf;
	m_nServerPort = GetPrivateProfileInt(_T("ServerInfo"), _T("m_nServerPort"), m_nServerPort, strINIFilePath);
	
	//读取文件存储路径
    GetPrivateProfileString(_T("Path"), _T("m_FileSavePath"), _T(""), pBuf, MAX_PATH, strINIFilePath);
    m_FileSavePath=pBuf;

	//读设备总数
	//写设备信息
	unsigned int nCount=0;
	nCount=GetPrivateProfileInt(_T("DevInfo"), _T("DevNum"), nCount, strINIFilePath);

	for (int i=0;i<nCount;i++)
	{
         SRecordDevInfo* pDevInfo=new SRecordDevInfo;
		 if (pDevInfo==NULL)
		 {
			 m_gGlobalVF.WriteLog("初始化分配录音设备信息失败！");
			 break;
		 }

		 CString strLeft;
		 strLeft.Format("DevName%d",i+1);
		 memset(pBuf,0,sizeof(pBuf));
		 //读名称
		 GetPrivateProfileString(_T("DevInfo"), strLeft,_T(""), pBuf, MAX_PATH,strINIFilePath);
		 pDevInfo->m_sDevName=pBuf;
		 //读ID
		 strLeft.Format("DevID%d",i+1);
		 pDevInfo->m_nDevID=GetPrivateProfileInt(_T("DevInfo"), strLeft, pDevInfo->m_nDevID, strINIFilePath);
		 //读IP地址
         strLeft.Format("DevIP%d",i+1);
		 pDevInfo->m_dDevIP=GetPrivateProfileInt(_T("DevInfo"),strLeft,pDevInfo->m_dDevIP, strINIFilePath);
		 //读端口
          strLeft.Format("DevPort%d",i+1);
		 pDevInfo->m_nDevPort=GetPrivateProfileInt(_T("DevInfo"), strLeft, pDevInfo->m_nDevPort, strINIFilePath);

		 m_gGlobalVF.m_lDevInfoList.AddTail(pDevInfo);


	}
		
}

BOOL CGlobalVF::DeleteDevInfo(DWORD id)
{
	//删除ID
	if (!m_lDevInfoList.IsEmpty())
	{
         POSITION pos=m_lDevInfoList.GetHeadPosition();


		 while (pos!=NULL)
		 {
			 POSITION postemp=pos;
			 SRecordDevInfo* pDevInfo=m_lDevInfoList.GetNext(pos);

			 if ((pDevInfo!=NULL)&&(pDevInfo->m_nDevID==id))
			 {
                 m_lDevInfoList.RemoveAt(postemp);
				 break;
			 }

		 }
	}

	return TRUE;
}

BOOL  CGlobalVF::DeleteDevInfo(SRecordDevInfo& sDevInfo)
{

	if (!m_lDevInfoList.IsEmpty())
	{
		POSITION pos=m_lDevInfoList.Find(&sDevInfo);


		if (pos!=NULL)
		{

			SRecordDevInfo* pDevInfo=m_lDevInfoList.GetNext(pos);

			if ((pDevInfo!=NULL)&&(pDevInfo->m_nDevID==sDevInfo.m_nDevID))
			{
				m_lDevInfoList.RemoveAt(pos);
			}

		}
	}

	return TRUE;



}

BOOL CGlobalVF::IsSameDevID(DWORD id)
{
	//是否有同名ID
	if (!m_lDevInfoList.IsEmpty())
	{
		POSITION pos=m_lDevInfoList.GetHeadPosition();;


		while (pos!=NULL)
		{

			SRecordDevInfo* pDevInfo=m_lDevInfoList.GetNext(pos);

			if ((pDevInfo!=NULL)&&(pDevInfo->m_nDevID==id))
			{
				return TRUE;
			}

		}
	}

	return FALSE;

}

SRecordDevInfo* CGlobalVF::FindDevInfo(DWORD id)
{
	//按照ID搜索设备信息
	SRecordDevInfo* pDevInfo=NULL;
	if (!m_lDevInfoList.IsEmpty())
	{
		POSITION pos=m_lDevInfoList.GetHeadPosition();;


		while (pos!=NULL)
		{

			pDevInfo=m_lDevInfoList.GetNext(pos);

			if ((pDevInfo!=NULL)&&(pDevInfo->m_nDevID==id))
			{
				return pDevInfo;
			}

		}
	}

   return pDevInfo;


}

BOOL CGlobalVF::IsHasRecordDBFile(CString FilePath)
{
      //
	if (PathFileExists(FilePath))
	{

		//解决中文目录下的情况

		return TRUE;
	}
	else
	{
		return FALSE;
	}
     

}