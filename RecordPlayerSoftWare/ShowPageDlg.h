#pragma once
#include "afxcmn.h"
#include "ListCtrlCl.h"

// CShowDownLoadDlg �Ի���
#include "./SQLite3/KompexSQLitePrerequisites.h"
#include "./SQLite3/KompexSQLiteDatabase.h"
#include "./SQLite3/KompexSQLiteStatement.h"
#include "./SQLite3/KompexSQLiteException.h"
#include "./SQLite3/KompexSQLiteStreamRedirection.h"
#include "./SQLite3/KompexSQLiteBlob.h"
#include "afxwin.h"
#include "BtnST.h"
#include "BitmapSlider.h"
#include "ProSliderCtrl.h"
#include "MyFtpClient.h"
//#include "CJLibrary.h"
#include "TreeCtrlEx.h"
#include "ClientSocket.h"
#include "StaticST.h"
#include "ShowInfoDlg.h"
// CShowPageDlg �Ի���
//��Ϊ�豸��ҳ����ʾ
#include <Map>

#include "libzplay.h"
using namespace libZPlay;

using namespace std;


typedef CTypedPtrList<CPtrList ,char*>CPtrFileList;//���ﶨ���ģ������;����Ѿ����ص��ļ�
typedef CMap<CString,LPCTSTR,DownLoadState,DownLoadState>FileNameStateMap;


#define  WM_DOWNLOAD_MSG_PROCESS WM_APP+5002

#define  PLAY_BUF_SIZE 1024*10

int  __stdcall  myCallbackFunc(void* instance,
							   void *user_data,
							   TCallbackMessage message,
							   unsigned int param1,
							   unsigned int param2);


class CShowPageDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CShowPageDlg)

public:
	CShowPageDlg();
	virtual ~CShowPageDlg();

// �Ի�������
	enum { IDD = IDD_PlayerShowDlg };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	// �豸���ؼ�
	//CTreeCtrl m_DeviceTreeCtrl;
	//CCJTreeCtrl  m_DeviceTreeCtrl;

	CTreeCtrlEx m_DeviceTreeCtrl;

	CImageList	m_ImageList; //ͼ���б�
	// �豸�б�ؼ�
	CListCtrlCl m_DeviceListCtrl;
	CImageList	m_ImageList2; //ͼ���б�
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//�ؼ��䶯��ε���
	CRect m_ClientRect; //����ܾ��δ�С
	CPoint m_oSize;  //��¼����ܵĿ�͸�

	void InitGUICtrl(); //��ʼ���ؼ�

	void ConfigRecordChannelName(CString strFilePath,SRecordDevInfo* pDevInfo);// ����ͨ������


	BOOL IsHasConfig(CString strFilePath); //�Ƿ��������ļ�


	BOOL SetRecorderTreeChannelName(SRecordDevInfo* pDevInfo);


	afx_msg void OnBnClickedAdddevbtn();
	afx_msg void OnAdddev();
	afx_msg void OnDeldev();
	afx_msg void OnModifydev();
	afx_msg void OnBnClickedDeletedevbtn();
	afx_msg void OnNMRClickDeviceTree(NMHDR *pNMHDR, LRESULT *pResult);

	
	void OnShowrecorddb();
	afx_msg void OnUpdateShowrecorddb(CCmdUI *pCmdUI);
	CString  CreateSaveDevRecordDB(CString FileDir); //�������ݿ�洢·��FileDirΪĬ�ϵ����һ���ļ��У����豸����ID����
    
	BOOL IsHasRecordDBFile(CString FilePath); //�ж�·�����Ƿ�������ݿ��ļ�

	CString m_DBSavePath; //���ݿ���·����Ĭ��
	//SQLite3���ݿ����
	SQLiteDatabase*  m_pSQLiteDatabase;
	SQLiteStatement* m_pSQLiteStatement;
	CCriticalSection m_DBLockSection;//�����ݿ����ʹ�ð�ȫ��
	DWORD m_ChannelV[8];

	//˫�����¼�
	afx_msg void OnNMDblclkDeviceTree(NMHDR *pNMHDR, LRESULT *pResult);

	//�б���������Զ������п�
	void AutoAdjustColumnWidth(CListCtrl *pListCtrl);

	// ��ѯ��ʼ����
	COleDateTime m_StartDate;
	// ��ʼʱ��
	COleDateTime m_StartTime;
	// ��������
	COleDateTime m_EndDate;
	// ����ʱ��
	COleDateTime m_EndTime;
	//���ݿ����
	BOOL OpenDataBase(CString FileName); //������
	BOOL ExCuteQuerySQL(CString strSQL);
	BOOL CloseDataBase(CString FileName); //�ر����ݿ�
	

	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	CBrush m_brBrush;//������ˢ

	//��Ӧ����¼�����б��¼�
	afx_msg void OnNMClickDeviceList(NMHDR *pNMHDR, LRESULT *pResult);

	//���Ź���
	CButtonST m_PlayBtn;// ���Ű�ť
	
	CButtonST m_PlayBackBtn;// ���˲���
	
	CButtonST m_PlayNextBtn;// ǰ������

	CButtonST m_SoundCtrlBtn; // ���ذ�ť
	// ���������ؼ�
	CBitmapSlider m_MusicVolumeCtrl;
	CProSliderCtrl m_PlayProgressCtrl;
	//CSliderCtrl m_PlayProgressCtrl;

	afx_msg void OnBnClickedPlaybtn();
	afx_msg void OnBnClickedPlaynextbtn();
	afx_msg void OnBnClickedPlaybackbtn();

   BOOL m_bPlayflag;
   BOOL m_VoCtrlFlag; //�������Ʊ�־
   void ShowPlayBtn(BOOL flag); //��ʾ���Ż�����ͣ��ť
   void ShowVolCtrlBtn(BOOL flag);//�������ư�ť
  
   afx_msg void OnBnClickedSoundctrlbtn();
 
   afx_msg BOOL OnEraseBkgnd(CDC* pDC);


   //��Ƶ����
   ZPlay* m_Player; //������ʵ��
   BOOL PlayAAcFile(CString& filename);  //������Ƶ�ļ�
   void PlayAAcStream(void* buffer=NULL,unsigned int len=0); //������Ƶ��
   char m_PlayBuffer[PLAY_BUF_SIZE]; //������Ƶ�Ļ�����
   FILE* m_inFile; //�򿪵Ĳ��������ļ�
   afx_msg LRESULT OnBitmapSliderMoved(WPARAM wParam, LPARAM lParam);
   afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);

   double m_PlayTotalLength; //���ŵ��ļ���ʱ������λ��

   afx_msg void OnTimer(UINT_PTR nIDEvent);
   UINT         m_nSliderTimer;     // ��ʱ��ID
   BOOL         m_bStreamFlag; //�����ű�־
  
   void ProcessShowSliderPos();
   void ShowPlayTimeTips();//��ʾ����ʱ��

  //FTPͨ����
   CMyFtpClient m_FtpClient; //ftp�ͻ���

   BOOL ConnectFtpServer(CString serverIP,CString username,CString pwd,int port=21); //���ӷ�����

   BOOL DownLoadFileFromServer(CString strServerFileName,CString StrLocalFileName,BOOL flag=TRUE,__int64 nStartPos=0,__int64 nEndPos=0); //�������ݿ���ΪFalse�������ļ���ΪTrue

   
 

   BOOL PauseDownLoad(); //��ͣ����
   BOOL ResumeDownLoad();//��������

   

   static DWORD WINAPI ConnectFTPServerThread(LPVOID lpParameter);//�����޷�����ʱ����ʱ��̫�������³��򿨶�

   void ProcessDownLoadPos();

    unsigned int GetCurDownLoadItem(); //��ȡ��ǰ�������ص���
	void SetCurDownLoadItem(unsigned int index); //���õ�ǰ������

	unsigned int m_nCurDownLoadItem; //��ǰ������

    void SetDownLoadState(DownLoadState eState); //��������״̬
    DownLoadState GetDownLoadState();           //��ȡ����״̬

	DownLoadState m_eDWLState; //����״̬

	afx_msg LRESULT OnProcessDownLoadMsg(WPARAM wparam,LPARAM lparam); //�������б�����ذ�ť�¼�
	BOOL ProcessDownLoadMsg(); //��װ����б�����ذ�ť�¼�
	static DWORD WINAPI ProcessDownLoadMsgThread(LPVOID lpParameter);//�����޷�����ʱ����ʱ��̫�������³��򿨶�

	afx_msg LRESULT OnProcessFTPMsg(WPARAM wparam,LPARAM lparam);
	BOOL m_bShow; //����ͬ���ļ���ʾ�Ի�������

	//��Ҫ����ӳ��ķ�ʽ��¼ÿһ�е�״̬
	FileNameStateMap m_StateMap;  //״̬ӳ��

	CRITICAL_SECTION m_csProject; //�����ٽ���

	SFTPServerInfo m_sFtpSeverInfo; //�õ���Ftp��������Ϣ

	BOOL m_bDBFlag; //���ݿ����ر�־

private:
	CString SetFtpServerIP();  //���÷�����IP��ַ,���ظ��豸��Ĭ��

	


public:
	afx_msg void OnDownloadrecorddb();  //�������ݿ�
	BOOL DownLoadRecordDB();//��װOnDownloadrecorddb����
	BOOL DownLoadChannelConfig();

	afx_msg void OnDownloadfile();
	afx_msg void OnPlayrecordfile();
	afx_msg void OnNMRClickDeviceList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnUpdateDownloadfile(CCmdUI *pCmdUI);


	//�����ļ�����;
	CPtrFileList m_ptrFileList; //�豸Ŀ¼���Ѿ����ص��ļ��б�
	BOOL FindAllFileList(CString strFileDir,CString FileExt=_T(".AAC")); //�ҵ�Ŀ¼�µ������ļ�
    CString m_strCurDevFilePath; //��ǰ�豸�ļ�·��

	

private:
	BOOL ParseDBFileShow();
	CString GetRecordTimeFromSecond(UINT nSecond); //������ת��Ϊ����ʱ��
	CString GetRecordTimeLength(UINT nStartSecond,UINT nEndSecond);//¼��ʱ��
	CString CreateRecordFileName(CString nCh,UINT nStartSceond,UINT nEndSecond);
	UINT   GetSecondFromRecordTime(SYSTEMTIME& st);
	void  FillTreeCtrlDefaultColor();
	CString GetFileNameFromItem(UINT nRow);//��ÿһ���л�ȡ�ļ���
	BOOL GetRecycleFilePosFromItemDB(UINT nRow,__int64& nStartPos,__int64& nEndPos); //��ȡ����λ��
	


public:
	
	CButtonST m_EditBtn;// �޸��豸��ť
	afx_msg void OnBnClickedEditbtn();
	CButtonST m_AddDevBtn; //����豸��ť
	CButtonST m_DelDevBtn; //ɾ���豸��ť
	// ��ʾ��������
	CStatic m_ShowSpectrumCtrl;

	HWND m_hParentWnd; //�����ھ��

	//ͨ�ž��
	//CAsyncSocket m_Sock;
    CClientSocket m_Sock;
	SRecordDevInfo* GetSelectTreeDevInfo();


   


	// ���ӵȴ�״̬
	CStaticST m_strConnectInfo;
	UINT m_nWaitConnTimer; //���ӵȴ���ʱ��
	void ShowConnWaitInfo(); //��ʾ������Ϣ
	UINT m_nWaitSecond;//�ȴ�������
	BOOL m_LoginNotify; //��½֪ͨ

	afx_msg void OnDownloadallfile(); //��������¼���ļ�
	static DWORD WINAPI ProcessDownLoadAllFileThread(LPVOID lpParameter);//�����޷�����ʱ����ʱ��̫�������³��򿨶�

	void DownloadAllfile();//���������ļ�

	BOOL m_bDownFlag; //���ر�־
	UINT m_nCurDownCount;//��ǰ���ظ���
	// ��ѯ��ť
	CButtonST m_QueryBtn;
	// �������ذ�ť
	CButtonST m_BatchDownBtn;
	afx_msg void OnBnClickedQuerybtn();
	afx_msg void OnBnClickedBatchdownbtn();
	// ��½¼����������ť
	CButtonST m_LoginBtn;
	afx_msg void OnBnClickedLoginbtn();

	//Ϊ�˽���������ڼ䣬������һ������,���ñ������п���
	BOOL m_bDowningFlag;//�������ر�־
	AppRunState m_eAppRunState;//����״̬
	CShowInfoDlg m_ShowInfodlg; //��ʾ����ʾ�Ի����ʾ��������

	
	// ¼������
	CComboBox m_RecorderType; 
	RECORDTYPE m_eRecordType;


	
};
