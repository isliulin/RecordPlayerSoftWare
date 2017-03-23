#ifndef DEFINE_H
#define DEFINE_H



#define RECORD_EVENT_DB_FILENAME  _T("event.db")
#define RECORD_EVENT_DB_TABLE_FILENAME _T("recorder")

#define RECORD_RECYCLE_DB_FILENAME _T("recycle.db")
#define RECORD_RECYCLE_DB_TABLE_FILENAME _T("recorder")


#define RECORD_CHANNEL_CONFIG_FILENAME  _T("chn_name.ini")


#define RECORD_EVENT_FILE_DIR     _T("EventRecorder\\")
#define RECORD_RECYCLE_FILE_DIR   _T("RecycleRecorder\\")


#define RECORD_EVENT_FILE_PATH    _T(".\\SPON\\")
#define REG_KEY_NAMED_PASSWOR	_T("SOFTWARE\\SPON\\RecordPlayerSoftWare\\Password")
#define REG_KEY_NAMED_USERNAME  _T("SOFTWARE\\SPON\\RecordPlayerSoftWare\\Username")

#define FILE_SUFFIX_REAL_FORMAT  _T("aac")
#define FILE_RECORD_FORMAT       _T("aac") 
#define FILE_SHOW_SUFFIX_FORMAT  _T("mp3")

#define APP_VERSION            _T("V1.3(2016-12-08)")   //�汾��
#define FILE_CONFIG			   _T("Config.ini")        //�����ļ�


//��������ö��
typedef enum  tagLANUAGETYPE {
	LANGUAGETYPE_ENGLISH = 0,
	LANGUAGETYPE_SIMP_CHS,	
	LANGUAGETYPE_TRAD_CHS,
} LANGUAGE_TYPE;


typedef enum APP_ERROR
{
	ERROR_LEVE0,       //����ȼ�0���ͼ����󣬱����û�����Ȳ������쳣
	ERROR_LEVE1,       //����ȼ�1���ڴ�������
	ERROR_LEVE2,       //����ȼ�2�����ݿⲻ���ڡ��ļ������ڵȴ���
	ERROR_LEVE3,       //����ȼ�3������ͨ�Ŵ���
	ERROR_LEVE4,       //����ȼ�4��¼�����Ŵ���
	ERROR_LEVE5,       //����ȼ�5��ϵͳ��������̿ռ�������
	ERROR_LEVE6,       //����ȼ�6������
	ERROR_LEVE7        //����ȼ�7��������

};

typedef enum DownLoadState
{
	DWL_START,  //���ؿ�ʼ
	DWL_PAUSE,  //������ͣ
	DWL_RESUME, //���ؼ���
	DWL_END     //���ؽ���

};

//����Ӧ�ó�������״̬
typedef enum AppRunState
{
	APP_INIT_STATE,         //��ʼ״̬
	APP_CONN_STATE,        //����״̬
	APP_BATCH_DOWN_STATE, //��������״̬
	APP_BATCH_DOWN_END_STATE
};


typedef struct tagFTPServerInfo
{
   CString  serverIP;  //������IP��ַ
   CString  username;  //�û���
   CString  pwd;       //����
   int      port;      //�˿�

   CString  strRomoteFileName; //�������˵��ļ���
   CString  strLocalFileName;  //���ض˵��ļ���
   CString  strLocalFileSavePath; //�����ļ����·��



}SFTPServerInfo,*PFTPServerInfo;

typedef enum tagRECORDTYPE 
{
	EVENT_RECORDER=0,
	RECYCLE_RECORDER

}RECORDTYPE;


//����ȫ�ֱ���
extern  SYSTEMTIME		m_stStartTime;    //��ѯ��ʼʱ��
extern  SYSTEMTIME		m_stEndTime;     //��ѯ����ʱ��

//¼���豸��Ϣ
typedef struct tagRecordDevInfo
{
    CString         m_sDevName;  //�豸����
	unsigned int    m_nDevID;    //�豸ID
	DWORD           m_dDevIP;    //�豸Ip
	unsigned int    m_nDevPort;  //�豸�˿�
	CString         m_chn1name;  //ͨ��1����
	CString         m_chn2name;
	CString         m_chn3name;
	CString         m_chn4name;
	CString         m_chn5name;
	CString         m_chn6name;
	CString         m_chn7name;
	CString         m_chn8name;


}SRecordDevInfo,*PSRecordDevInfo;

extern CString			GetApplicationPath();
extern char *unicodeToUtf8(const WCHAR *zWideFilename);
extern WCHAR * mbcsToUnicode(const char *zFilename);
extern wchar_t * UTF8ToUnicode( const char* str );
extern char * UnicodeToANSI( const wchar_t* str );

extern  CString Utf8ToAnsi(const char* srcCode);  


#endif
