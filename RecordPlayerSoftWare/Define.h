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

#define APP_VERSION            _T("V1.3(2016-12-08)")   //版本号
#define FILE_CONFIG			   _T("Config.ini")        //配置文件


//语言类型枚举
typedef enum  tagLANUAGETYPE {
	LANGUAGETYPE_ENGLISH = 0,
	LANGUAGETYPE_SIMP_CHS,	
	LANGUAGETYPE_TRAD_CHS,
} LANGUAGE_TYPE;


typedef enum APP_ERROR
{
	ERROR_LEVE0,       //错误等级0：低级错误，比如用户输入等操作性异常
	ERROR_LEVE1,       //错误等级1：内存分配错误
	ERROR_LEVE2,       //错误等级2：数据库不存在、文件不存在等错误
	ERROR_LEVE3,       //错误等级3：网络通信错误
	ERROR_LEVE4,       //错误等级4：录音播放错误
	ERROR_LEVE5,       //错误等级5：系统错误：如磁盘空间已满等
	ERROR_LEVE6,       //错误等级6：保留
	ERROR_LEVE7        //错误等级7：程序奔溃

};

typedef enum DownLoadState
{
	DWL_START,  //下载开始
	DWL_PAUSE,  //下载暂停
	DWL_RESUME, //下载继续
	DWL_END     //下载结束

};

//定义应用程序运行状态
typedef enum AppRunState
{
	APP_INIT_STATE,         //初始状态
	APP_CONN_STATE,        //连接状态
	APP_BATCH_DOWN_STATE, //批量下载状态
	APP_BATCH_DOWN_END_STATE
};


typedef struct tagFTPServerInfo
{
   CString  serverIP;  //服务器IP地址
   CString  username;  //用户名
   CString  pwd;       //密码
   int      port;      //端口

   CString  strRomoteFileName; //服务器端的文件名
   CString  strLocalFileName;  //本地端的文件名
   CString  strLocalFileSavePath; //本地文件存放路径



}SFTPServerInfo,*PFTPServerInfo;

typedef enum tagRECORDTYPE 
{
	EVENT_RECORDER=0,
	RECYCLE_RECORDER

}RECORDTYPE;


//定义全局变量
extern  SYSTEMTIME		m_stStartTime;    //查询起始时间
extern  SYSTEMTIME		m_stEndTime;     //查询结束时间

//录音设备信息
typedef struct tagRecordDevInfo
{
    CString         m_sDevName;  //设备名称
	unsigned int    m_nDevID;    //设备ID
	DWORD           m_dDevIP;    //设备Ip
	unsigned int    m_nDevPort;  //设备端口
	CString         m_chn1name;  //通道1名称
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
