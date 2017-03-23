#ifndef GLOBAL_VF_H
#define GLOBAL_VF_H
#pragma once

#include "Define.h"



//ȫ�ֱ����ͺ�����
class CGlobalVF
{
public:
	CGlobalVF(void);

	~CGlobalVF(void);

public:
	void WriteLog(CString str);  //д��־
	CRITICAL_SECTION m_gcslogMutex; //ȫ����־��
public:
	//��������
	CString m_ServerIP; //������IP
	unsigned int m_nServerPort; //�������˿�
	CString m_FileSavePath;  //�����ļ��Ĵ洢·��
	CRITICAL_SECTION m_gcsMutex; //ȫ�ֱ�����

	SRecordDevInfo m_sDevInfo;  //�豸��Ϣ
	CTypedPtrList<CPtrList,SRecordDevInfo*> m_lDevInfoList; //�豸��Ϣ����

	void WriteConfigFile(void);
	void ReadConfigFile(void);

	BOOL DeleteDevInfo(DWORD id);
	BOOL DeleteDevInfo(SRecordDevInfo& sDevInfo);
	BOOL IsSameDevID(DWORD id);
    SRecordDevInfo* FindDevInfo(DWORD id);

	//���ݿ�Ĭ�ϴ��·��
	CString m_DBSavePath; 

	BOOL IsHasRecordDBFile(CString FilePath); //�жϸ�·�����Ƿ������ݿ��ļ�
	BOOL m_bConnServer; //���ӷ������Ƿ�ɹ�


};




#endif