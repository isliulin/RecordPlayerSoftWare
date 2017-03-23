#ifndef GLOBAL_VF_H
#define GLOBAL_VF_H
#pragma once

#include "Define.h"



//全局变量和函数类
class CGlobalVF
{
public:
	CGlobalVF(void);

	~CGlobalVF(void);

public:
	void WriteLog(CString str);  //写日志
	CRITICAL_SECTION m_gcslogMutex; //全局日志锁
public:
	//网络设置
	CString m_ServerIP; //服务器IP
	unsigned int m_nServerPort; //服务器端口
	CString m_FileSavePath;  //下载文件的存储路径
	CRITICAL_SECTION m_gcsMutex; //全局变量锁

	SRecordDevInfo m_sDevInfo;  //设备信息
	CTypedPtrList<CPtrList,SRecordDevInfo*> m_lDevInfoList; //设备信息链表

	void WriteConfigFile(void);
	void ReadConfigFile(void);

	BOOL DeleteDevInfo(DWORD id);
	BOOL DeleteDevInfo(SRecordDevInfo& sDevInfo);
	BOOL IsSameDevID(DWORD id);
    SRecordDevInfo* FindDevInfo(DWORD id);

	//数据库默认存放路径
	CString m_DBSavePath; 

	BOOL IsHasRecordDBFile(CString FilePath); //判断该路径下是否有数据库文件
	BOOL m_bConnServer; //连接服务器是否成功


};




#endif