// ShowPageDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "RecordPlayerSoftWare.h"
#include "ShowPageDlg.h"
#include "Define.h"
#include "RecordDevDlg.h"

// CShowPageDlg �Ի���
#define DLG_BK_COLOR  RGB(255,255,255)

#define PLAYER_RANGE   1000

#define SLIDER_TIMER   100

#define DOWN_LOAD_TIMER 101

#define CONN_WAIT_TIMER 102



#define DOWN_LOAD_ICON_INDEX 0

#define TREE_ITEM_ACTIVE_COLOR     RGB(48,163,62)
#define TREE_ITEM_NOACTIVE_COLOR   RGB(0,0,0)

#define DOWN_LOAD_SUBITEM_INDEX  4

int  __stdcall  myCallbackFunc(void* instance, void *user_data, TCallbackMessage message, unsigned int param1, unsigned int param2)
{
	ZPlay *myplayer = (ZPlay*) instance;

	switch(message)
	{
	case MsgStreamNeedMoreData: // stream needs more data
		{

			CShowPageDlg* pdlg=(CShowPageDlg*)user_data;

			FILE *in = (FILE*) pdlg->m_inFile; // this parameter is set by SetCallbackFunc

			// read next chunk of data from file into memory buffer
			//char buffer[10000];
			unsigned int read = fread(pdlg->m_PlayBuffer, 1, PLAY_BUF_SIZE,in);

			// push this memory buffer into stream
			myplayer->PushDataToStream(pdlg->m_PlayBuffer, read);
		//	AfxMessageBox("��������ݸ���������");

		}
	case MsgStopAsync:
		{
			CShowPageDlg* pdlg=(CShowPageDlg*)user_data;

           

			
			//myplayer->Stop();


			pdlg->GetDlgItem(IDC_ConnectText)->SetWindowText(_T(""));



			pdlg->ShowPlayBtn(TRUE);
			



		}
		return 0;	
	}

	return 0;
}

IMPLEMENT_DYNAMIC(CShowPageDlg, CDialogEx)

CShowPageDlg::CShowPageDlg()
	: CDialogEx(CShowPageDlg::IDD)
	, m_StartDate(COleDateTime::GetCurrentTime())
	, m_StartTime(COleDateTime::GetCurrentTime())
	, m_EndDate(COleDateTime::GetCurrentTime())
	,m_EndTime(COleDateTime::GetCurrentTime())
{

	m_StartDate.SetDate(m_StartDate.GetYear(),m_StartDate.GetMonth(),1);
	m_StartTime.SetTime(0,0,0);
	m_EndTime.SetTime(23,59,59);
	m_pSQLiteDatabase=NULL;
	m_pSQLiteStatement=NULL;
	memset(m_ChannelV,0,sizeof(m_ChannelV));
	m_bPlayflag=FALSE;
	m_VoCtrlFlag=FALSE;
	

	//��ʼ����������
	m_Player=NULL;
	memset(m_PlayBuffer,0,sizeof(m_PlayBuffer));
	

	//��������������ʱID
	m_nSliderTimer=0;
	m_nCurDownLoadItem=0;

	


	//��ʼ���ٽ���
	InitializeCriticalSection(&m_csProject);

	//��ʼ��FTP��������Ϣ
	m_sFtpSeverInfo.port=21;
	m_sFtpSeverInfo.serverIP=_T("192.168.1.101");
	m_sFtpSeverInfo.pwd=_T("sponrdc");
	m_sFtpSeverInfo.username=_T("recorder");
	m_sFtpSeverInfo.strRomoteFileName=_T("");
	m_sFtpSeverInfo.strLocalFileName=_T("");



	m_bDBFlag=FALSE;


    m_hParentWnd=NULL;

	//���ӵȴ���ʱ��
	m_nWaitConnTimer=0;

	//��ǰ�б�¼���ļ����ظ���
	m_nCurDownCount=0;

	m_bDowningFlag=FALSE;

	m_eAppRunState=APP_INIT_STATE;

	m_eRecordType=EVENT_RECORDER;

	m_bShow=FALSE; 

	
}

CShowPageDlg::~CShowPageDlg()
{

	if (m_Player)
	{
		m_Player->Release(); // destroy class
	}

	if (m_nSliderTimer)
	{
		KillTimer(m_nSliderTimer);
		m_nSliderTimer = 0;
	}
}

void CShowPageDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_Device_Tree, m_DeviceTreeCtrl);
	DDX_Control(pDX, IDC_Device_List, m_DeviceListCtrl);
	DDX_DateTimeCtrl(pDX, IDC_DATE_START, m_StartDate);
	DDX_DateTimeCtrl(pDX, IDC_TIME_START, m_StartTime);
	DDX_DateTimeCtrl(pDX, IDC_DATE_END, m_EndDate);
	DDX_DateTimeCtrl(pDX, IDC_TIME_END, m_EndTime);
	DDX_Control(pDX, IDC_PlayBtn, m_PlayBtn);
	DDX_Control(pDX, IDC_PlayBackBtn, m_PlayBackBtn);
	DDX_Control(pDX, IDC_PlayNextBtn, m_PlayNextBtn);
	DDX_Control(pDX, IDC_SoundCtrlBtn, m_SoundCtrlBtn);
	DDX_Control(pDX, IDC_SoundVolume, m_MusicVolumeCtrl);
	DDX_Control(pDX, IDC_PlayProGressBtn, m_PlayProgressCtrl);
	DDX_Control(pDX, IDC_EditBtn, m_EditBtn);
	DDX_Control(pDX, IDC_AddDevBtn, m_AddDevBtn);
	DDX_Control(pDX, IDC_DeleteDevBtn, m_DelDevBtn);
	DDX_Control(pDX, IDC_ShowVolSpectrum, m_ShowSpectrumCtrl);
	DDX_Control(pDX, IDC_ConnectText, m_strConnectInfo);
	DDX_Control(pDX, IDC_QueryBtn, m_QueryBtn);
	DDX_Control(pDX, IDC_BatchDownBtn, m_BatchDownBtn);
	DDX_Control(pDX, IDC_LoginBtn, m_LoginBtn);
	DDX_Control(pDX, IDC_ComBo_RECORDTYPE, m_RecorderType);
}


BEGIN_MESSAGE_MAP(CShowPageDlg, CDialogEx)
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_AddDevBtn, &CShowPageDlg::OnBnClickedAdddevbtn)
	ON_COMMAND(ID_AddDev, &CShowPageDlg::OnAdddev)
	ON_COMMAND(ID_DelDev, &CShowPageDlg::OnDeldev)
	ON_COMMAND(ID_ModifyDev, &CShowPageDlg::OnModifydev)
	ON_BN_CLICKED(IDC_DeleteDevBtn, &CShowPageDlg::OnBnClickedDeletedevbtn)
	ON_NOTIFY(NM_RCLICK, IDC_Device_Tree, &CShowPageDlg::OnNMRClickDeviceTree)
	ON_COMMAND(ID_DownLoadRecordDB, &CShowPageDlg::OnDownloadrecorddb)
	ON_UPDATE_COMMAND_UI(ID_DownLoadRecordDB, &CShowPageDlg::OnUpdateShowrecorddb)
	ON_NOTIFY(NM_DBLCLK, IDC_Device_Tree, &CShowPageDlg::OnNMDblclkDeviceTree)
	ON_WM_CTLCOLOR()
	ON_NOTIFY(NM_CLICK, IDC_Device_List, &CShowPageDlg::OnNMClickDeviceList)
	ON_BN_CLICKED(IDC_PlayBtn, &CShowPageDlg::OnBnClickedPlaybtn)
	ON_BN_CLICKED(IDC_PlayNextBtn, &CShowPageDlg::OnBnClickedPlaynextbtn)
	ON_BN_CLICKED(IDC_PlayBackBtn, &CShowPageDlg::OnBnClickedPlaybackbtn)
	ON_BN_CLICKED(IDC_SoundCtrlBtn, &CShowPageDlg::OnBnClickedSoundctrlbtn)
	ON_WM_ERASEBKGND()
	ON_WM_HSCROLL()
	ON_MESSAGE(WM_BITMAPSLIDER_MOVED, &CShowPageDlg::OnBitmapSliderMoved)
	ON_WM_TIMER()
	ON_MESSAGE(WM_DOWNLOAD_MSG_PROCESS, &CShowPageDlg::OnProcessDownLoadMsg)
	ON_COMMAND(ID_DownLoadRecordDB, &CShowPageDlg::OnDownloadrecorddb)
	ON_MESSAGE(WM_FTP_EVENT_MSG,&CShowPageDlg::OnProcessFTPMsg)
	ON_COMMAND(ID_DownLoadFile, &CShowPageDlg::OnDownloadfile)
	ON_COMMAND(ID_PlayRecordFile, &CShowPageDlg::OnPlayrecordfile)
	ON_NOTIFY(NM_RCLICK, IDC_Device_List, &CShowPageDlg::OnNMRClickDeviceList)
	ON_UPDATE_COMMAND_UI(ID_DownLoadFile, &CShowPageDlg::OnUpdateDownloadfile)
	ON_BN_CLICKED(IDC_EditBtn, &CShowPageDlg::OnBnClickedEditbtn)
	ON_COMMAND(ID_DownLoadAllFile, &CShowPageDlg::OnDownloadallfile)
	ON_BN_CLICKED(IDC_QueryBtn, &CShowPageDlg::OnBnClickedQuerybtn)
	ON_BN_CLICKED(IDC_BatchDownBtn, &CShowPageDlg::OnBnClickedBatchdownbtn)
	ON_BN_CLICKED(IDC_LoginBtn, &CShowPageDlg::OnBnClickedLoginbtn)
	
END_MESSAGE_MAP()


// CShowPageDlg ��Ϣ�������

BOOL CShowPageDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	

	//��ʼ��ʱ������Ĭ�ϵ����ݿ��ܴ��·��
	CString appPath=GetApplicationPath();

	appPath+="RecordDB\\";

	//�����ļ���
	if (!::PathIsDirectory(appPath))
	{
		//�����ļ���
		if (CreateDirectory(appPath,NULL)==FALSE)
		{
			AfxMessageBox("����¼����¼�ļ���ʧ��",MB_OK,MB_ICONERROR);
			return FALSE;
		}

	}


	m_DBSavePath=appPath;

	m_gGlobalVF.m_DBSavePath=m_DBSavePath; //���ڲ�ѯ�����������ݿ������




	InitGUICtrl();



	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
}

void CShowPageDlg::InitGUICtrl()
{

	//¼�����ͳ�ʼ��
	m_RecorderType.AddString("�¼�");
	m_RecorderType.AddString("ѭ��");

	m_RecorderType.SetCurSel(m_eRecordType);


	//��ѯ��ť
	m_QueryBtn.SetFlat(FALSE);
	m_QueryBtn.SetColor(CButtonST::BTNST_COLOR_BK_OUT,RGB(255,255,255));
	m_QueryBtn.SetColor(CButtonST::BTNST_COLOR_BK_IN,RGB(48,168,63));
	m_QueryBtn.SetColor(CButtonST::BTNST_COLOR_BK_FOCUS,RGB(48,168,63));

	m_BatchDownBtn.SetFlat(FALSE);
	m_BatchDownBtn.SetColor(CButtonST::BTNST_COLOR_BK_OUT,RGB(255,255,255));
	m_BatchDownBtn.SetColor(CButtonST::BTNST_COLOR_BK_IN,RGB(48,168,63));
	m_BatchDownBtn.SetColor(CButtonST::BTNST_COLOR_BK_FOCUS,RGB(48,168,63));




     //��ʼ�����οؼ�
	//m_DeviceTreeCtrl.EnableMultiSelect();
	HICON Icon[4];

	Icon[0]=AfxGetApp()->LoadIcon(IDI_DevIcon);
	Icon[1]=AfxGetApp()->LoadIcon(IDI_DevIcon);

	Icon[2]=AfxGetApp()->LoadIcon(IDI_TunnelIcon);
	Icon[3]=AfxGetApp()->LoadIcon(IDI_TunnelIcon);

	m_ImageList.Create (16, 16, 0, 4,4);

	m_ImageList.Add(Icon[0]); //����ͼ��
	m_ImageList.Add(Icon[1]); //����ͼ��
	m_ImageList.Add(Icon[2]); //����ͼ��
	m_ImageList.Add(Icon[3]); //����ͼ��
	
	m_DeviceTreeCtrl.SetImageList(&m_ImageList, TVSIL_NORMAL);

	m_DeviceTreeCtrl.SetItemHeight(15);

	//��ʾ�豸�б�
	POSITION pos=m_gGlobalVF.m_lDevInfoList.GetHeadPosition();
	CString  strIP;  
	CString strDevName;
	while (pos!=NULL)
	{
	
		SRecordDevInfo* pDevInfo=m_gGlobalVF.m_lDevInfoList.GetNext(pos);

		//ˢ�����οؼ�
		strDevName=pDevInfo->m_sDevName;

		//DWORD IP ת��ΪCString 
		unsigned  char  *pIP  =  (unsigned  char*)&pDevInfo->m_dDevIP;  
		strIP.Format("%u.%u.%u.%u",*(pIP+3),  *(pIP+2),  *(pIP+1),  *pIP);

		strDevName.Format("%s%d(%s)",pDevInfo->m_sDevName,pDevInfo->m_nDevID,strIP);

		//������
		HTREEITEM hti = m_DeviceTreeCtrl.InsertItem(strDevName, 0, 1); //0-��ʾ��һ��ͼ�ꣻ1��ʾ�ڶ���ͼ��
		
		if (pDevInfo!=NULL)
		{
			m_DeviceTreeCtrl.SetItemData(hti,(DWORD_PTR)pDevInfo);
		}

		for( int i = 0; i < 8; ++i )
		{

			//ͨ��������Ҫ���������ļ�����������
			CString str;

			CString strFileDir=strDevName;
			CString FilePath=m_DBSavePath+strFileDir;
			FilePath+="\\";
			FilePath+=RECORD_CHANNEL_CONFIG_FILENAME;
			


			if (IsHasConfig(FilePath))
			{

               //���������ļ�����ȡ�����ļ�
				ConfigRecordChannelName(FilePath,pDevInfo);

                if (i+1==1)
                {
					str.Format(_T("%d-%s"),i+1,pDevInfo->m_chn1name);
                }

				if (i+1==2)
				{
					str.Format(_T("%d-%s"),i+1,pDevInfo->m_chn2name);
				}

				if (i+1==3)
				{
					str.Format(_T("%d-%s"),i+1,pDevInfo->m_chn3name);
				}

				if (i+1==4)
				{
					str.Format(_T("%d-%s"),i+1,pDevInfo->m_chn4name);
				}

				if (i+1==5)
				{
					str.Format(_T("%d-%s"),i+1,pDevInfo->m_chn5name);
				}

				if (i+1==6)
				{
					str.Format(_T("%d-%s"),i+1,pDevInfo->m_chn6name);
				}

				if (i+1==7)
				{
					str.Format(_T("%d-%s"),i+1,pDevInfo->m_chn7name);
				}

				if (i+1==8)
				{
					str.Format(_T("%d-%s"),i+1,pDevInfo->m_chn8name);
				}
				
				


			}
			else
			{
				
				str.Format(_T("%d-ͨ��"),i+1);
			}
			

			


		    HTREEITEM	subhti=m_DeviceTreeCtrl.InsertItem(str, 2, 3, hti);
			m_ChannelV[i]=i+1;
			m_DeviceTreeCtrl.SetItemData(subhti,(DWORD_PTR)&m_ChannelV[i]);
		}
		m_DeviceTreeCtrl.Expand(hti, TVE_COLLAPSE);

	}

    
	/*
	HTREEITEM hti = m_DeviceTreeCtrl.InsertItem(_T("¼���豸"), 0, 1); //0-��ʾ��һ��ͼ�ꣻ1��ʾ�ڶ���ͼ��
	for( int i = 0; i < 8; ++i ) {
		CString str;
		str.Format(_T("ͨ��%d"),i+1);
		

		m_DeviceTreeCtrl.InsertItem(str, 2, 3, hti);
	}
	m_DeviceTreeCtrl.Expand(hti, TVE_EXPAND);
	*/
	

	//��ʼ���б�ؼ�
	//m_DeviceListCtrl.SetExtendedStyle(LVS_EX_GRIDLINES|LVS_EX_FULLROWSELECT|LVS_AUTOARRANGE|LVS_EX_SUBITEMIMAGES|LVS_EX_CHECKBOXES);
	m_DeviceListCtrl.SetExtendedStyle(LVS_EX_GRIDLINES|LVS_EX_FULLROWSELECT|LVS_AUTOARRANGE|LVS_EX_SUBITEMIMAGES);

	m_DeviceListCtrl.SetHeaderHeight(1.5);
	m_DeviceListCtrl.InsertColumn(0,"���",LVCFMT_CENTER,100);
	//m_DeviceListCtrl.InsertColumn(1,"�ļ���",LVCFMT_CENTER,300);
	m_DeviceListCtrl.InsertColumn(1,"ͨ��",LVCFMT_CENTER,100);
	m_DeviceListCtrl.InsertColumn(2,"¼��ʱ��",LVCFMT_CENTER,150);
	m_DeviceListCtrl.InsertColumn(3,"¼��ʱ��",LVCFMT_CENTER,150);
	//m_DeviceListCtrl.InsertColumn(5,"����",LVCFMT_CENTER,100);
	m_DeviceListCtrl.InsertColumn(DOWN_LOAD_SUBITEM_INDEX,"���ؽ���", LVCFMT_CENTER,150);


	m_DeviceListCtrl.SetColumnType(DOWN_LOAD_SUBITEM_INDEX,Progress);
	m_DeviceListCtrl.SetShowProgressPercent(TRUE);
    m_DeviceListCtrl.SetSupportSort(TRUE);


	m_DeviceListCtrl.SetHeaderBKColor(204,199,199,2);
	m_DeviceListCtrl.SetHeaderTextColor(RGB(16,71,9));

	m_DeviceListCtrl.SetRowHeigt(25);

	m_DeviceListCtrl.SetTextColor(RGB(48,163,62));

	

	//��ʼ����������ť
	//m_PlayBtn.SetIcon(IDI_PLAYPRESS,IDI_PLAY);
	m_PlayBtn.SetIcon(IDI_PLAY,IDI_PLAY);
	m_PlayBtn.SetColor(CButtonST::BTNST_COLOR_BK_OUT,RGB(255,255,255));
	m_PlayBtn.SetColor(CButtonST::BTNST_COLOR_BK_IN,RGB(255,255,255));
	m_PlayBtn.SetColor(CButtonST::BTNST_COLOR_BK_FOCUS,RGB(255,255,255));

	short	shplayBtnColor =255;
	m_PlayBtn.OffsetColor(CButtonST::BTNST_COLOR_BK_IN, shplayBtnColor);
	m_PlayBtn.DrawTransparent(TRUE);

	//ǰ����ť
	//m_PlayNextBtn.SetIcon(IDI_PlayNextPress,IDI_PlayNext);
	m_PlayNextBtn.SetIcon(IDI_PlayNext,IDI_PlayNext);
	m_PlayNextBtn.SetColor(CButtonST::BTNST_COLOR_BK_OUT,RGB(255,255,255));
	m_PlayNextBtn.SetColor(CButtonST::BTNST_COLOR_BK_IN,RGB(255,255,255));
	m_PlayNextBtn.SetColor(CButtonST::BTNST_COLOR_BK_FOCUS,RGB(255,255,255));

	m_PlayNextBtn.OffsetColor(CButtonST::BTNST_COLOR_BK_IN, shplayBtnColor);
	m_PlayNextBtn.DrawTransparent(TRUE);

   //���˰�ť
	//m_PlayBackBtn.SetIcon(IDI_PlayBackPress,IDI_PlayBack);
	m_PlayBackBtn.SetIcon(IDI_PlayBack,IDI_PlayBack);
	m_PlayBackBtn.SetColor(CButtonST::BTNST_COLOR_BK_OUT,RGB(255,255,255));
	m_PlayBackBtn.SetColor(CButtonST::BTNST_COLOR_BK_IN,RGB(255,255,255));
	m_PlayBackBtn.SetColor(CButtonST::BTNST_COLOR_BK_FOCUS,RGB(255,255,255));
	m_PlayBackBtn.OffsetColor(CButtonST::BTNST_COLOR_BK_IN, shplayBtnColor);
	m_PlayBackBtn.DrawTransparent(TRUE);


    //���ذ�ť
	m_SoundCtrlBtn.SetIcon(IDI_Sound);
	m_SoundCtrlBtn.SetColor(CButtonST::BTNST_COLOR_BK_OUT,RGB(255,255,255));
	m_SoundCtrlBtn.SetColor(CButtonST::BTNST_COLOR_BK_IN,RGB(255,255,255));
	m_SoundCtrlBtn.SetColor(CButtonST::BTNST_COLOR_BK_FOCUS,RGB(255,255,255));
	short	shBtnColor =255;
	m_SoundCtrlBtn.OffsetColor(CButtonST::BTNST_COLOR_BK_IN, shBtnColor);


	//�����ؼ�
	m_MusicVolumeCtrl.SetBitmapThumb(IDB_BITMAP_THUMB,IDB_BITMAP_THUMB,FALSE);
	m_MusicVolumeCtrl.SetBitmapChannel( IDB_BITMAP_VOL_OFF, IDB_BITMAP_VOL_ON, FALSE );
	m_MusicVolumeCtrl.SetRange( 0, 100 );
	m_MusicVolumeCtrl.SetPos( 100 );
	m_MusicVolumeCtrl.SetMarginLeft( 0 );
	m_MusicVolumeCtrl.SetMarginRight( 0);
	m_MusicVolumeCtrl.SetPageSize( 25 );
	m_MusicVolumeCtrl.DrawFocusRect(FALSE,TRUE);

	//��ʼ������
	//m_PlayProgressCtrl.SubclassDlgItem(IDC_PlayProGressBtn,this);
	m_PlayProgressCtrl.SetRange(0,PLAYER_RANGE);		// Horizantal slider
	m_PlayProgressCtrl._SetRange(0,PLAYER_RANGE);			// slider ranges

	
	//����豸��ť
	
	m_AddDevBtn.SetIcon(IDI_AddDev);
	m_AddDevBtn.SetColor(CButtonST::BTNST_COLOR_BK_OUT,RGB(255,255,255));
	m_AddDevBtn.SetColor(CButtonST::BTNST_COLOR_BK_IN,RGB(255,255,255));
	m_AddDevBtn.SetColor(CButtonST::BTNST_COLOR_BK_FOCUS,RGB(255,255,255));
	
	//ɾ���豸��ť
	m_DelDevBtn.SetIcon(IDI_DelDev);
	m_DelDevBtn.SetColor(CButtonST::BTNST_COLOR_BK_OUT,RGB(255,255,255));
	m_DelDevBtn.SetColor(CButtonST::BTNST_COLOR_BK_IN,RGB(255,255,255));
	m_DelDevBtn.SetColor(CButtonST::BTNST_COLOR_BK_FOCUS,RGB(255,255,255));
	//�༭�豸��ť

//	HICON btnIcon=theApp.LoadIcon(IDI_Edit);
	//m_EditBtn.SetIcon(btnIcon);
	m_EditBtn.SetIcon(IDI_Edit);
	m_EditBtn.SetColor(CButtonST::BTNST_COLOR_BK_OUT,RGB(255,255,255));
	m_EditBtn.SetColor(CButtonST::BTNST_COLOR_BK_IN,RGB(255,255,255));
	m_EditBtn.SetColor(CButtonST::BTNST_COLOR_BK_FOCUS,RGB(255,255,255));
	short	shEditBtnColor =255;
	m_EditBtn.OffsetColor(CButtonST::BTNST_COLOR_BK_IN, shEditBtnColor);


	//��½��ť
	m_LoginBtn.SetIcon(IDI_Login);
	m_LoginBtn.SetColor(CButtonST::BTNST_COLOR_BK_OUT,RGB(255,255,255));
	m_LoginBtn.SetColor(CButtonST::BTNST_COLOR_BK_IN,RGB(255,255,255));
	m_LoginBtn.SetColor(CButtonST::BTNST_COLOR_BK_FOCUS,RGB(255,255,255));
	short	shLoginBtnColor =255;
	m_LoginBtn.OffsetColor(CButtonST::BTNST_COLOR_BK_IN, shLoginBtnColor);


	//������ˢ
	m_brBrush.CreateSolidBrush(DLG_BK_COLOR);

	Invalidate(TRUE);  

}

BOOL CShowPageDlg::IsHasConfig(CString strFilePath)
{

   BOOL bRet=TRUE;


   if (!PathFileExists(strFilePath))
   {
      bRet=FALSE;
   }


   return bRet;




}

void CShowPageDlg::ConfigRecordChannelName(CString strFilePath,SRecordDevInfo*pDevInfo)
{


	if (pDevInfo!=NULL)
	{
		CString		strINIFilePath =strFilePath;
		TCHAR		pBuf[MAX_PATH];
		memset(pBuf,0,sizeof(pBuf));

		GetPrivateProfileString(_T("recorder"), _T("chn1_name"), _T(""), pBuf, MAX_PATH, strINIFilePath);
		pDevInfo->m_chn1name =Utf8ToAnsi(pBuf);

		GetPrivateProfileString(_T("recorder"), _T("chn2_name"), _T(""), pBuf, MAX_PATH, strINIFilePath);
		pDevInfo->m_chn2name =Utf8ToAnsi(pBuf);

		GetPrivateProfileString(_T("recorder"), _T("chn3_name"), _T(""), pBuf, MAX_PATH, strINIFilePath);
		pDevInfo->m_chn3name =Utf8ToAnsi(pBuf);

		GetPrivateProfileString(_T("recorder"), _T("chn4_name"), _T(""), pBuf, MAX_PATH, strINIFilePath);
		pDevInfo->m_chn4name =Utf8ToAnsi(pBuf);

		GetPrivateProfileString(_T("recorder"), _T("chn5_name"), _T(""), pBuf, MAX_PATH, strINIFilePath);
		pDevInfo->m_chn5name = Utf8ToAnsi(pBuf);

		GetPrivateProfileString(_T("recorder"), _T("chn6_name"), _T(""), pBuf, MAX_PATH, strINIFilePath);
		pDevInfo->m_chn6name = Utf8ToAnsi(pBuf);

		GetPrivateProfileString(_T("recorder"), _T("chn7_name"), _T(""), pBuf, MAX_PATH, strINIFilePath);
		pDevInfo->m_chn7name = Utf8ToAnsi(pBuf);

		GetPrivateProfileString(_T("recorder"), _T("chn8_name"), _T(""), pBuf, MAX_PATH, strINIFilePath);
		pDevInfo->m_chn8name =Utf8ToAnsi(pBuf);
	}
	
	

	




}

BOOL CShowPageDlg::SetRecorderTreeChannelName(SRecordDevInfo* pDevInfo)
{

    if (pDevInfo==NULL)
    {
		return FALSE;
    }
	//ˢ�����οؼ�
	CString strDevName=pDevInfo->m_sDevName;


	//DWORD IP ת��ΪCString 
	unsigned  char  *pIP  =  (unsigned  char*)&pDevInfo->m_dDevIP;  
	CString strIP="";
	strIP.Format("%u.%u.%u.%u",*(pIP+3),  *(pIP+2),  *(pIP+1),  *pIP);

	strDevName.Format("%s%d(%s)",pDevInfo->m_sDevName,pDevInfo->m_nDevID,strIP);

	CString strFileDir=strDevName;
	CString FilePath=m_DBSavePath+strFileDir;
	FilePath+="\\";
	FilePath+=RECORD_CHANNEL_CONFIG_FILENAME;


	if (IsHasConfig(FilePath))
	{
		ConfigRecordChannelName(FilePath,pDevInfo);


		//��ѡ������οؼ�������޸�
		HTREEITEM hItem=m_DeviceTreeCtrl.GetSelectedItem();

        HTREEITEM htitemp;

		SRecordDevInfo*pDenInfo=NULL;
		if (hItem!=NULL)
		{
			if (m_DeviceTreeCtrl.ItemHasChildren(hItem))
			{
				htitemp=hItem;
			
				pDenInfo=(SRecordDevInfo*)m_DeviceTreeCtrl.GetItemData(htitemp);
   	
			}
			else
			{

				hItem=m_DeviceTreeCtrl.GetParentItem(hItem);
				htitemp=hItem;
				pDenInfo=(SRecordDevInfo*)m_DeviceTreeCtrl.GetItemData(htitemp);

			}
		}

		if (pDenInfo!=NULL&&pDenInfo->m_dDevIP==pDevInfo->m_dDevIP)
		{

			//�ҵ�Ҫ���õ�¼��ͨ����

			CString str="";
			HTREEITEM hSubItem=m_DeviceTreeCtrl.GetChildItem(htitemp);

			str.Format(_T("%d-%s"),1,pDevInfo->m_chn1name);


            m_DeviceTreeCtrl.SetItemText(hSubItem,str);

			for(int i=1;i<8;i++)
			{

                hSubItem=m_DeviceTreeCtrl.GetNextSiblingItem(hSubItem);

				if (hSubItem!=NULL)
				{

					if (i+1==2)
					{
						str.Format(_T("%d-%s"),i+1,pDevInfo->m_chn2name);


					}

					if (i+1==3)
					{
						str.Format(_T("%d-%s"),i+1,pDevInfo->m_chn3name);
					}

					if (i+1==4)
					{
						str.Format(_T("%d-%s"),i+1,pDevInfo->m_chn4name);
					}

					if (i+1==5)
					{
						str.Format(_T("%d-%s"),i+1,pDevInfo->m_chn5name);
					}

					if (i+1==6)
					{
						str.Format(_T("%d-%s"),i+1,pDevInfo->m_chn6name);
					}

					if (i+1==7)
					{
						str.Format(_T("%d-%s"),i+1,pDevInfo->m_chn7name);
					}

					if (i+1==8)
					{
						str.Format(_T("%d-%s"),i+1,pDevInfo->m_chn8name);
					}


					m_DeviceTreeCtrl.SetItemText(hSubItem,str);

				}

				


                



			}

		}





	}
    




}

void CShowPageDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);

	//�����ӿؼ�
	//�ߴ�仯
	if (nType==SIZE_RESTORED || nType==SIZE_MAXIMIZED)  
	{  

		float   ratio[2];  
		POINT   newDialogSize;  
		CRect   newRect;  

		//��ȡ�µĿͻ����Ĵ�С  
		GetClientRect(&newRect);  
		newDialogSize.x = newRect.right - newRect.left;  
		newDialogSize.y = newRect.bottom - newRect.top;  

		//�����ڵĶԻ����������Ի���Ĵ�С����  
		ratio[0]    = (float)newDialogSize.x / m_oSize.x;  
		ratio[1]    = (float)newDialogSize.y / m_oSize.y;  

		CRect Rect;  
		int woc;  

		//�����Ͻǵ�����  
		CPoint OldTLPoint, NewTLPint;  
		CPoint OldBRPoint, NewBRPint;  

		//�г����пؼ�  
		HWND  hwndChild = ::GetWindow(m_hWnd,GW_CHILD);  

		while(hwndChild)  
		{  
			//ȡ��ID  
			woc  = ::GetDlgCtrlID(hwndChild);  

			GetDlgItem(woc)->GetWindowRect(Rect);  
			ScreenToClient(Rect);  

			switch (woc)
			{
			case IDC_ComBo_RECORDTYPE:
				{
					OldTLPoint = Rect.TopLeft();  
					NewTLPint.x  = long(OldTLPoint.x*ratio[0]+0.5);  
					NewTLPint.y  = long(OldTLPoint.y*ratio[1]+0.5);  

					OldBRPoint   = Rect.BottomRight();  
					NewBRPint.x  = long(OldBRPoint.x*ratio[0]+0.5);  
					//��Ҫ����������ĸ߶�
					NewBRPint.y  = long(OldBRPoint.y*ratio[1]+0.5)+100;  
					Invalidate();
				}
				break;	

			case IDC_PlayBtn:
			case IDC_PlayBackBtn:
			case IDC_PlayNextBtn:
			case IDC_SoundCtrlBtn:
				{
			
					OldTLPoint = Rect.TopLeft();  
					NewTLPint.x  = long(OldTLPoint.x*ratio[0]+0.5);  
					NewTLPint.y  = long(OldTLPoint.y*ratio[1]+0.5);  

					OldBRPoint   = Rect.BottomRight();  
					NewBRPint.x  = NewTLPint.x +Rect.Width();  
					NewBRPint.y  = NewTLPint.y +Rect.Height();  

					if (IDC_SoundVolume==woc)
					{
						m_MusicVolumeCtrl.DrawFocusRect(FALSE,TRUE);
					}

					Invalidate();

				}
				break;

			case IDC_Device_Tree:
			case IDC_Device_List:
			default:
				{
					OldTLPoint = Rect.TopLeft();  
					NewTLPint.x  = long(OldTLPoint.x*ratio[0]+0.5);  
					NewTLPint.y  = long(OldTLPoint.y*ratio[1]+0.5);  

					OldBRPoint   = Rect.BottomRight();  
					NewBRPint.x  = long(OldBRPoint.x*ratio[0]+0.5);  
					NewBRPint.y  = long(OldBRPoint.y*ratio[1]+0.5);  
					Invalidate(FALSE);

				}
				break;

			}

			Rect.SetRect(NewTLPint,NewBRPint);  
			GetDlgItem(woc)->MoveWindow(Rect,TRUE);  


			hwndChild=::GetWindow(hwndChild, GW_HWNDNEXT); 
			Invalidate();

		}  




		m_oSize   = newDialogSize;      

	}  

}

void CShowPageDlg::OnBnClickedAdddevbtn()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CRecordDevDlg dlg;
	dlg.m_bAddFlag=TRUE;
	if (dlg.DoModal()==IDOK)
	{
		//ˢ�����οؼ�
		CString strDevName;
		strDevName=m_gGlobalVF.m_sDevInfo.m_sDevName;
		//DWORD IP ת��ΪCString 
		CString  strIP;  
		unsigned  char  *pIP  =  (unsigned  char*)&m_gGlobalVF.m_sDevInfo.m_dDevIP;  
		strIP.Format("%u.%u.%u.%u",*(pIP+3),  *(pIP+2),  *(pIP+1),  *pIP);

		strDevName.Format("%s%d(%s)",m_gGlobalVF.m_sDevInfo.m_sDevName,m_gGlobalVF.m_sDevInfo.m_nDevID,strIP);

		//������
		HTREEITEM hti = m_DeviceTreeCtrl.InsertItem(strDevName, 0, 1); //0-��ʾ��һ��ͼ�ꣻ1��ʾ�ڶ���ͼ��
		//��������
		SRecordDevInfo* pDevInfo=m_gGlobalVF.m_lDevInfoList.GetTail();

        if (pDevInfo!=NULL)
        {
           m_DeviceTreeCtrl.SetItemData(hti,(DWORD_PTR)pDevInfo);
        }

		for( int i = 0; i < 8; ++i ) {
			CString str;
			str.Format(_T("ͨ��%d"),i+1);
			HTREEITEM subhti=m_DeviceTreeCtrl.InsertItem(str, 2, 3, hti);

			//��������
			m_ChannelV[i]=i+1;
			m_DeviceTreeCtrl.SetItemData(subhti,(DWORD_PTR)&m_ChannelV[i]);

		}
		m_DeviceTreeCtrl.Expand(hti, TVE_EXPAND);


		//�������ݿ�
		CreateSaveDevRecordDB(strDevName);



	}
	UpdateData(FALSE);
}

void CShowPageDlg::OnAdddev()
{
	// ����豸
	OnBnClickedAdddevbtn();
}

void CShowPageDlg::OnDeldev()
{
	// ɾ���豸
	OnBnClickedDeletedevbtn();

}

SRecordDevInfo* CShowPageDlg::GetSelectTreeDevInfo()
{

	// �޸��豸:���Ȼ�ȡѡ���������
	HTREEITEM hti=m_DeviceTreeCtrl.GetSelectedItem();

	HTREEITEM htitemp;
	SRecordDevInfo* pDenInfo=NULL;
	if(hti!=NULL)
	{
		//	HTREEITEM htitemp=m_DeviceTreeCtrl.GetNextItem(hti,TVGN_NEXT);
		if (m_DeviceTreeCtrl.ItemHasChildren(hti))
		{
			htitemp=hti;
			//hti=htitemp;
			//��������ɾ������
			pDenInfo=(SRecordDevInfo*)m_DeviceTreeCtrl.GetItemData(htitemp);



		}
		else
		{

			hti=m_DeviceTreeCtrl.GetParentItem(hti);
			htitemp=hti;
			//hti=htitemp;
			//��������ɾ������
			pDenInfo=(SRecordDevInfo*)m_DeviceTreeCtrl.GetItemData(htitemp);


		}

	}

	return pDenInfo;

}

void CShowPageDlg::OnModifydev()
{
	// �޸��豸:���Ȼ�ȡѡ���������
	HTREEITEM hti=m_DeviceTreeCtrl.GetSelectedItem();

	HTREEITEM htitemp;
	SRecordDevInfo* pDenInfo=NULL;
	if(hti!=NULL)
	{
		//	HTREEITEM htitemp=m_DeviceTreeCtrl.GetNextItem(hti,TVGN_NEXT);
		if (m_DeviceTreeCtrl.ItemHasChildren(hti))
		{
			htitemp=hti;
			//hti=htitemp;
			//��������ɾ������
			pDenInfo=(SRecordDevInfo*)m_DeviceTreeCtrl.GetItemData(htitemp);

			
		
		}
		else
		{

			hti=m_DeviceTreeCtrl.GetParentItem(hti);
			htitemp=hti;
			//hti=htitemp;
			//��������ɾ������
			pDenInfo=(SRecordDevInfo*)m_DeviceTreeCtrl.GetItemData(htitemp);

		
		}


		if(pDenInfo!=NULL)
		{
			//���öԻ���
			CRecordDevDlg dlg;
			dlg.m_sDevName=pDenInfo->m_sDevName;
            dlg.m_nDevID=pDenInfo->m_nDevID;
			dlg.m_dDevIP=pDenInfo->m_dDevIP;
            dlg.m_nDevPort=pDenInfo->m_nDevPort;
			dlg.m_bAddFlag=FALSE;

			

			if (dlg.DoModal()==IDOK)
			{
				//���и��ı���
				pDenInfo->m_sDevName=dlg.m_sDevName;
				pDenInfo->m_nDevID=dlg.m_nDevID;
				pDenInfo->m_dDevIP=dlg.m_dDevIP;
				pDenInfo->m_nDevPort=dlg.m_nDevPort;

				m_DeviceTreeCtrl.SetItemData(htitemp,(DWORD_PTR)pDenInfo);

				//����ItemText
				CString strDevName;
				strDevName=pDenInfo->m_sDevName;
				//DWORD IP ת��ΪCString 
				CString  strIP;  
				unsigned  char  *pIP  =  (unsigned  char*)&pDenInfo->m_dDevIP;  
				strIP.Format("%u.%u.%u.%u",*(pIP+3),  *(pIP+2),  *(pIP+1),  *pIP);

				strDevName.Format("%s%d(%s)",pDenInfo->m_sDevName,pDenInfo->m_nDevID,strIP);

				m_DeviceTreeCtrl.SetItemText(htitemp,strDevName);

				//�������ݿ���Ŀ¼
				CreateSaveDevRecordDB(strDevName);
			}

		}

	}






	//

	

	
}

void CShowPageDlg::OnBnClickedDeletedevbtn()
{
	//ɾ���豸
	//ɾ���ڵ�

	UpdateData(TRUE);
	//m_DeviceTreeCtrl.EnableMultiSelect(TRUE);
	HTREEITEM hti=m_DeviceTreeCtrl.GetSelectedItem();

	HTREEITEM htitemp;
	if(hti!=NULL)
	{
	//	HTREEITEM htitemp=m_DeviceTreeCtrl.GetNextItem(hti,TVGN_NEXT);
		if (m_DeviceTreeCtrl.ItemHasChildren(hti))
		{
			htitemp=hti;
			//hti=htitemp;
			//��������ɾ������
			SRecordDevInfo* pDenInfo=(SRecordDevInfo*)m_DeviceTreeCtrl.GetItemData(htitemp);

			if (pDenInfo!=NULL)
			{
				m_gGlobalVF.DeleteDevInfo(pDenInfo->m_nDevID);
			}
			m_DeviceTreeCtrl.DeleteItem(hti);
		}
		else
		{
			
			 hti=m_DeviceTreeCtrl.GetParentItem(hti);
			 htitemp=hti;
			 //hti=htitemp;
			 //��������ɾ������
			 SRecordDevInfo* pDenInfo=(SRecordDevInfo*)m_DeviceTreeCtrl.GetItemData(htitemp);

			 if (pDenInfo!=NULL)
			 {
				 m_gGlobalVF.DeleteDevInfo(pDenInfo->m_nDevID);
			 }
			 m_DeviceTreeCtrl.DeleteItem(hti);
		}
	}
	else
	{
		AfxMessageBox("��ѡ��Ҫɾ�����豸",MB_ICONINFORMATION);
	}

	//д�����ļ�
	m_gGlobalVF.WriteConfigFile();
		//m_DeviceTreeCtrl.EnableMultiSelect(FALSE);

}

void CShowPageDlg::OnNMRClickDeviceTree(NMHDR *pNMHDR, LRESULT *pResult)
{
	/*
	CPoint point;

	GetCursorPos(&point);

	CPoint pointInTree = point;

	m_DeviceTreeCtrl.ScreenToClient(&pointInTree);

	HTREEITEM item;

	UINT flag = TVHT_ONITEM;

	item = m_DeviceTreeCtrl.HitTest(pointInTree, &flag);

	if(item != NULL)

	{

		m_DeviceTreeCtrl.SelectItem(item);

		CMenu menu;

		menu.LoadMenu(IDR_MENU_DEV_TREE);

		menu.GetSubMenu(0)->TrackPopupMenu(TPM_LEFTALIGN | 

			TPM_RIGHTBUTTON, point.x, point.y, this, NULL);

	}
	*/
	CMenu   menu;
	POINT pt={0};
	GetCursorPos(&pt);

	menu.LoadMenu(IDR_MENU_DEV_TREE);

	CMenu* pMenu=menu.GetSubMenu(0);

	//���ز˵���
	pMenu->RemoveMenu(ID_ModifyDev, MF_BYCOMMAND);
    //���ò˵��ĳ�ʼ״̬
	//ĳ��˵���Ч�����
	HTREEITEM hti=m_DeviceTreeCtrl.GetSelectedItem();
	if (hti==NULL)
	{
	  pMenu->EnableMenuItem(1, MF_BYPOSITION | MF_DISABLED | MF_GRAYED);
	  pMenu->EnableMenuItem(2, MF_BYPOSITION | MF_DISABLED | MF_GRAYED);
	}
	

	pMenu->TrackPopupMenu(TPM_LEFTALIGN|TPM_RIGHTBUTTON,pt.x,pt.y,this);
	*pResult = 0;
}


UINT   CShowPageDlg::GetSecondFromRecordTime(SYSTEMTIME& st)
{
	//��SYSTEMTIMEת��Ϊ��1970��1��1�ռ��������
	CTime tStart(st);

	UINT nSecond=tStart.GetTime();


	return nSecond;




}


void CShowPageDlg::OnShowrecorddb()
{
	//�������ݿ�
	//1����Ҫ�����豸���ƺ�ͨ�����ƶ�ȡ���ݿ⣺������Ҫ���׵�����ô�������豸�µ����ݿ���·��
    //��ȡѡ����豸��
	UpdateData(TRUE);
	HTREEITEM hti=m_DeviceTreeCtrl.GetSelectedItem();

	BOOL bflag=TRUE; //�豸���־��1Ϊ�豸�0Ϊͨ����
	HTREEITEM htitemp;
	CString strFileDir;
	DWORD nChannel=0;
	if (hti!=NULL)
	{
		//��ȡ��hti���ı�
		if(m_DeviceTreeCtrl.ItemHasChildren(hti))
		{
			//�豸��
			bflag=TRUE;
			strFileDir=m_DeviceTreeCtrl.GetItemText(hti);

            m_DeviceTreeCtrl.SetItemColor(hti,TREE_ITEM_ACTIVE_COLOR);

			m_DeviceTreeCtrl.Expand(hti, TVE_EXPAND);

			Invalidate(FALSE);



			
		}
		else
		{
           //ͨ����
		   htitemp=m_DeviceTreeCtrl.GetParentItem(hti);
		   //��ȡѡ����ǵڼ�ͨ����
           nChannel=*((DWORD*)m_DeviceTreeCtrl.GetItemData(hti));

		   bflag=FALSE;

		   strFileDir=m_DeviceTreeCtrl.GetItemText(htitemp);

		   m_DeviceTreeCtrl.SetItemColor(hti,TREE_ITEM_ACTIVE_COLOR);

		   m_DeviceTreeCtrl.SetItemColor(htitemp,TREE_ITEM_ACTIVE_COLOR);

		   m_DeviceTreeCtrl.Expand(htitemp, TVE_EXPAND);

		   Invalidate(FALSE);

 
		}

		
		//��ȡIP��ַ
		//��ȡ���豸�µ��ļ��б�


		//��ѯ���ݿ�
		//��ȡ���豸�µ����ݿ��ļ�·��
         CString FilePath=m_DBSavePath+strFileDir;

         FilePath+="\\";

		 m_strCurDevFilePath=FilePath; //��ǰ�豸���·��

		 m_sFtpSeverInfo.strLocalFileSavePath=m_strCurDevFilePath; //�����ص�ǰ�豸·�������豸���·��

		 if (m_eRecordType==EVENT_RECORDER)
		 {
			  FilePath+=RECORD_EVENT_DB_FILENAME;
		 }
		 else if (m_eRecordType==RECYCLE_RECORDER)
		 {
			  FilePath+=RECORD_RECYCLE_DB_FILENAME;
		 }

		 //FilePath+=RECORD_DB_FILENAME;

		 if (!IsHasRecordDBFile(FilePath))
		 {
			 m_gGlobalVF.WriteLog("¼����¼�ļ������ڣ�");
			 AfxMessageBox("¼����¼�ļ�������,����ͬ��¼���ļ���",MB_ICONINFORMATION);
			 return;
		 }

		 //��ȡʱ��
		 m_stStartTime.wYear=m_StartDate.GetYear();
		 m_stStartTime.wMonth=m_StartDate.GetMonth();
		 m_stStartTime.wDay=m_StartDate.GetDay();

		 m_stStartTime.wHour=m_StartTime.GetHour();
		 m_stStartTime.wMinute=m_StartTime.GetMinute();
		 m_stStartTime.wSecond=m_StartTime.GetSecond();

		 m_stEndTime.wYear=m_EndDate.GetYear();
		 m_stEndTime.wMonth=m_EndDate.GetMonth();
		 m_stEndTime.wDay=m_EndDate.GetDay();

		 m_stEndTime.wHour=m_EndTime.GetHour();
		 m_stEndTime.wMinute=m_EndTime.GetMinute();
		 m_stEndTime.wSecond=m_EndTime.GetSecond();

		 //���ʱ��
		 /*
		 if ((m_stStartTime.wYear>m_stEndTime.wYear)||(m_stStartTime.wMonth>m_stEndTime.wMonth)\
			 ||(m_stStartTime.wDay>m_stEndTime.wDay)||(m_stStartTime.wHour>m_stEndTime.wHour)\
			 ||(m_stStartTime.wMinute>m_stEndTime.wMinute)||(m_stStartTime.wSecond>m_stEndTime.wSecond))
		 {
			 AfxMessageBox("¼������ʱ�䲻��С��¼����ʼʱ�䣡",MB_OK,MB_ICONWARNING);
			 return;
		 }
		 */

		 //������ѯ���
		 /*
		 CString strSQL; 
		 //������ѯ���
		 CString strDateTimeStart;
		 strDateTimeStart.Format("%d-%02d-%02d %02d:%02d:%02d",m_stStartTime.wYear,m_stStartTime.wMonth,m_stStartTime.wDay,\
			 m_stStartTime.wHour,m_stStartTime.wMinute,m_stStartTime.wSecond);
		 CString strDateTimeEnd;
		 strDateTimeEnd.Format("%d-%02d-%02d %02d:%02d:%02d",m_stEndTime.wYear,m_stEndTime.wMonth,m_stEndTime.wDay,\
			 m_stEndTime.wHour,m_stEndTime.wMinute,m_stEndTime.wSecond);

		 */
		 /*
		 if (bflag)
		 {
			 strSQL.Format("SELECT * FROM %s",RECORD_DB_TABLE_FILENAME);
		 }
		 else
		 {
			 strSQL.Format("SELECT * FROM %s where Channel=%d",RECORD_DB_TABLE_FILENAME,nChannel);
		 }
		 */
		 //������ѯ���
		 CString strSQL;


		 UINT nStartSecond=GetSecondFromRecordTime(m_stStartTime);
		 UINT nEndSecond=GetSecondFromRecordTime(m_stEndTime);

		 if (nStartSecond>nEndSecond)
		 {
			 
				 AfxMessageBox("¼������ʱ�䲻��С��¼����ʼʱ�䣡",MB_OK,MB_ICONWARNING);
				 return;
			 
		 }



		 if (nChannel==0)
		 {
			 if (m_eRecordType==EVENT_RECORDER)
			 {
				 strSQL.Format("SELECT * FROM %s where start_time>=%d and stop_time<=%d",RECORD_EVENT_DB_TABLE_FILENAME,nStartSecond,nEndSecond);
			 }
			 else if (m_eRecordType==RECYCLE_RECORDER)
			 {
				 strSQL.Format("SELECT * FROM %s where start_time>=%d and stop_time<=%d",RECORD_RECYCLE_DB_TABLE_FILENAME,nStartSecond,nEndSecond);
			 }
			 
		 }
		 else
		 {
			 if (m_eRecordType==EVENT_RECORDER)
			 {
				 strSQL.Format("SELECT * FROM %s where chn=%d and start_time>=%d and stop_time<=%d",RECORD_EVENT_DB_TABLE_FILENAME,nChannel,nStartSecond,nEndSecond);
			 }
			 else if (m_eRecordType==RECYCLE_RECORDER)
			 {
				 strSQL.Format("SELECT * FROM %s where chn=%d and start_time>=%d and stop_time<=%d",RECORD_RECYCLE_DB_TABLE_FILENAME,nChannel,nStartSecond,nEndSecond);
			 }
 
		 }



		 //
		
		

	

		 //�����ݿ�


        m_DBLockSection.Lock();
		 
		if (OpenDataBase(FilePath))
		{
           ExCuteQuerySQL(strSQL);
		   
		}

		CloseDataBase(FilePath);

		m_DBLockSection.Unlock();
		

	//	AutoAdjustColumnWidth(&m_DeviceListCtrl);
	}
		

}

BOOL CShowPageDlg::FindAllFileList(CString strFileDir,CString FileExt)
{
	 //���е��ļ���չ��
	CFileFind Finder ;
	CString strDir = strFileDir ;    //strFileDirΪҪ�������ļ�����Ŀ¼
	if( strDir.Right(1) != "\\" )
		strDir += "\\" ;
	strDir += "*.*" ;
	bool bWorking = Finder.FindFile( strDir ) ;
	while( bWorking ) 
	{
		bWorking = Finder.FindNextFile() ;
		CString tFileName = Finder.GetFileName() ; //GetFilePath���õ�ȫ·����
	   
		char*  pFileName=new char[tFileName.GetLength()+1];
		if (pFileName==NULL)
		{
			m_gGlobalVF.WriteLog("�����ļ��б��ڴ�ʧ��!");
			break;
		}
		memset(pFileName,0,tFileName.GetLength()+1);
		memcpy(pFileName,tFileName.GetBuffer(0),tFileName.GetLength());

		m_ptrFileList.AddTail(pFileName);

	}

      return TRUE;
}

BOOL CShowPageDlg::OpenDataBase(CString FileName)
{
	//�����ݿ�
	try{

		WCHAR* pFileName=mbcsToUnicode(FileName.GetBuffer(0));
		if (m_pSQLiteDatabase==NULL)
		{


			m_pSQLiteDatabase = new Kompex::SQLiteDatabase(unicodeToUtf8(pFileName), SQLITE_OPEN_READWRITE, 0);
		}

		m_pSQLiteDatabase->Open(unicodeToUtf8(pFileName), SQLITE_OPEN_READONLY, 0);


	//	m_pSQLiteDatabase->MoveDatabaseToMemory();:�ú����ڴ�ĳЩ���ݿ�ʱ����ִ���

		if(m_pSQLiteStatement==NULL)
		{
			m_pSQLiteStatement = new SQLiteStatement(m_pSQLiteDatabase);
		}
		else
		{
			delete m_pSQLiteStatement;
			m_pSQLiteStatement=NULL;
			m_pSQLiteStatement = new SQLiteStatement(m_pSQLiteDatabase);

		}




	}
	catch(CException* e)
	{
		m_gGlobalVF.WriteLog("�����ݿ�ʧ�ܣ�");
		return FALSE;
	}

	return TRUE;


}

BOOL CShowPageDlg::ExCuteQuerySQL(CString strSQL)
{
	//����SQL����ѯ���ݿ�
	try
	{
		m_pSQLiteStatement->Sql(strSQL);

		//�б�ɾ����������
		m_DeviceListCtrl.DeleteAllItems();

		//���ݲ�ѯ�������ݽ�����ʾ
		ParseDBFileShow();

		
	}
	catch (CMemoryException* e)
	{
		AfxMessageBox("�ڴ�New����",MB_OK,MB_ICONERROR);
		return FALSE;
	}
	catch (CFileException* e)
	{
		AfxMessageBox("�ļ��򿪴���",MB_OK,MB_ICONERROR);
		return FALSE;
	}
	catch (CException* e)
	{
		AfxMessageBox("δ֪����",MB_OK,MB_ICONERROR);
		return FALSE;
	}

	return TRUE;

}



BOOL CShowPageDlg::ParseDBFileShow()
{
	//�������ݿ��ļ�����ʾ
	    m_DeviceListCtrl.SetRedraw(FALSE);
		
		int nColCount=m_pSQLiteStatement->GetColumnCount();

		int row=0;
		int col=0;
		int nlCount=0;
		CString szText;
		CString strChannel; //¼��ͨ��
		CString strFilename; //¼���ļ���
		CString strRecordTime; //¼��ʱ��
		CString strRecordLength; //¼��ʱ��
		UINT nStartSecond=0;
		UINT nEndSecond=0;
		
		
		while(m_pSQLiteStatement->FetchRow())
		{


			//���Ȼ�ȡ��һ�е����һ�У����б�ʶΪ�Ƿ���ʾ��ʶ
			 BOOL  bShow=m_pSQLiteStatement->GetColumnBool(nColCount-1);

			 if (!bShow)
			 {
				 //���Ϊ����ȡ��һ��
				 continue;
			 }

			//��Ҫ�������
			nlCount++;
			szText.Format("%d",nlCount);
			m_DeviceListCtrl.InsertItem(row, szText);
			m_DeviceListCtrl.SetItemImage(row,0,IDB_SmallDownLoad);
             CString strColName;
			 

			for(int col = 0; col <nColCount-1; col++)
			{

				 const char* pname=m_pSQLiteStatement->GetColumnOriginName(col);

				 strColName.Format(_T("%s"),pname);

				 if (strColName==_T("chn"))
				 {
					 //��ȡͨ��
					 strChannel=m_pSQLiteStatement->GetColumnString16(col);
				 }
				 else if (strColName==_T("start_time"))
				 {
					 //��ȡ��ʼ¼��ʱ��
					 nStartSecond=m_pSQLiteStatement->GetColumnInt(col);
                     strRecordTime=GetRecordTimeFromSecond(nStartSecond);


				 }
				 else if (strColName==_T("stop_time"))
				 {
					 //��ȡֹͣʱ��ͼ���¼��ʱ��
					 nEndSecond=m_pSQLiteStatement->GetColumnInt(col);
					 strRecordLength=GetRecordTimeLength(nStartSecond,nEndSecond);

				 }
				 else
				 {
					 continue;
				 }
			}


			//��ʼ�����б���
			int nListCount=m_DeviceListCtrl.GetColumnCount();

			//�����ļ���;
			strFilename=CreateRecordFileName(strChannel,nStartSecond,nEndSecond);

			
			for (int col=1;col<nListCount;)
			{

			
				//m_DeviceListCtrl.SetItem(row, col, LVIF_TEXT|LVIF_IMAGE, strFilename, 0, 0, 0, 0);

				//col++;

                m_DeviceListCtrl.SetItem(row, col, LVIF_TEXT|LVIF_IMAGE, strChannel, 0, 0, 0, 0);

				

                col++;

				m_DeviceListCtrl.SetItem(row, col, LVIF_TEXT|LVIF_IMAGE, strRecordTime, 0, 0, 0, 0);


				col++;

				m_DeviceListCtrl.SetItem(row, col, LVIF_TEXT|LVIF_IMAGE, strRecordLength, 0, 0, 0, 0);


				//col++;

				//m_DeviceListCtrl.SetItemImage(row,col,IDB_SmallDownLoad);


				col++;
				//�������ؽ���;�����ļ��Ĵ���;�ж��Ƿ��Ѿ�����
				CString filename=m_strCurDevFilePath;

				if (m_eRecordType==EVENT_RECORDER)
				{

					filename+=RECORD_EVENT_FILE_DIR;
					filename+=strFilename;
				}
				else if (m_eRecordType==RECYCLE_RECORDER)
				{
					filename+=RECORD_RECYCLE_FILE_DIR;
					filename+=strFilename;
				}

				
				//ȡ����
				//�ļ�����ΪMP3
				//filename.Replace(FILE_SHOW_SUFFIX_FORMAT,FILE_SUFFIX_REAL_FORMAT);

				if (PathFileExists(filename))
				{
					//�ļ����ڻ���Ҫ�ж��ļ���С�Ƿ��0
					//��ȡ�ļ���С
					m_DeviceListCtrl.SetCellProgressValue(row, col, 100);
				}
				else
				{
					m_DeviceListCtrl.SetCellProgressValue(row, col, 0);
				}

				col++;

   
			}



			++row;
			
		}
		
		m_pSQLiteStatement->FreeQuery();

		m_DeviceListCtrl.SetRedraw(TRUE);


		return TRUE;
		

}

CString CShowPageDlg::CreateRecordFileName(CString nCh,UINT nStartSceond,UINT nEndSecond)
{
	//�����ļ���
	CTime tStart(nStartSceond);
	CTime tEnd(nEndSecond);


	CString strFileName;


	strFileName.Format(_T("%s-%04d%02d%02d_%02d%02d%02d-%04d%02d%02d_%02d%02d%02d.%s"),
		nCh,tStart.GetYear(),tStart.GetMonth(),tStart.GetDay()
		,tStart.GetHour(),tStart.GetMinute(),tStart.GetSecond()
		,tEnd.GetYear(),tEnd.GetMonth(),tEnd.GetDay()
		,tEnd.GetHour(),tEnd.GetMinute(),tEnd.GetSecond()
		,FILE_SHOW_SUFFIX_FORMAT);


	return strFileName;




}


BOOL CShowPageDlg::CloseDataBase(CString FileName)
{
	//�ر����ݿ�
	try{
		m_pSQLiteDatabase->Close();
		if (m_pSQLiteStatement!=NULL)
		{
			delete m_pSQLiteStatement;
			m_pSQLiteStatement=NULL;
		}
	}catch(CException* e)
	{
		m_gGlobalVF.WriteLog("�ر����ݿ�ʧ�ܣ�");
		AfxMessageBox("�ر����ݿ�ʧ�ܣ�",MB_OK,MB_ICONERROR);
		return FALSE;
	}


	return TRUE;


}




CString  CShowPageDlg::CreateSaveDevRecordDB(CString FileDir)
{
	//��ȡӦ�ó���·��
	CString appPath=m_DBSavePath;

	appPath+=FileDir;

	appPath+="\\";

	//�����ļ���
	if (!::PathIsDirectory(appPath))
	{
		//�����ļ���
		if (CreateDirectory(appPath,NULL)==FALSE)
		{
			AfxMessageBox("�������ݿ��ļ���ʧ��",MB_OK,MB_ICONERROR);
			return "";
		}

	}

	//���������¼�¼����ѭ��¼�����ļ���
    CString strEventPath=appPath;
	strEventPath+=RECORD_EVENT_FILE_DIR;

	//�����ļ���
	if (!::PathIsDirectory(strEventPath))
	{
		//�����ļ���
		if (CreateDirectory(strEventPath,NULL)==FALSE)
		{
			AfxMessageBox("�����¼�¼���ļ���ʧ��",MB_OK,MB_ICONERROR);
			return "";
		}

	}

	CString strRecyclePath=appPath;
	strRecyclePath+=RECORD_RECYCLE_FILE_DIR;

	//�����ļ���
	if (!::PathIsDirectory(strRecyclePath))
	{
		//�����ļ���
		if (CreateDirectory(strRecyclePath,NULL)==FALSE)
		{
			AfxMessageBox("�����¼�¼���ļ���ʧ��",MB_OK,MB_ICONERROR);
			return "";
		}

	}







	return appPath;
	
}

BOOL CShowPageDlg::IsHasRecordDBFile(CString FilePath)
{
	

	if (PathFileExists(FilePath))
	{

		//�������Ŀ¼�µ����

		return TRUE;
	}
	else
	{
		return FALSE;
	}



}

void CShowPageDlg::OnUpdateShowrecorddb(CCmdUI *pCmdUI)
{
	// ͨ��������ֹ�˵���
	HTREEITEM hti=m_DeviceTreeCtrl.GetSelectedItem();
	if (hti!=NULL)
	{
        if (pCmdUI->m_nID==ID_DownLoadRecordDB||pCmdUI->m_nID==ID_DelDev)
        {
           pCmdUI->Enable(TRUE);
        }
        
	}
	else
	{
		if (pCmdUI->m_nID==ID_DownLoadRecordDB||pCmdUI->m_nID==ID_DelDev)
		{
			pCmdUI->Enable(FALSE);
		}

	}
}


CString  CShowPageDlg::SetFtpServerIP()
{
	
	HTREEITEM hti=m_DeviceTreeCtrl.GetSelectedItem();

	BOOL bflag=TRUE; //�豸���־��1Ϊ�豸�0Ϊͨ����
	HTREEITEM htitemp;
	CString strFileDir;
	DWORD nChannel=0;
	if (hti!=NULL)
	{
		//��ȡ��hti���ı�
		if(m_DeviceTreeCtrl.ItemHasChildren(hti))
		{
			//�豸��
			bflag=TRUE;
			strFileDir=m_DeviceTreeCtrl.GetItemText(hti);



		}
		else
		{
			//ͨ����
			htitemp=m_DeviceTreeCtrl.GetParentItem(hti);
			//��ȡѡ����ǵڼ�ͨ����
			nChannel=*((DWORD*)m_DeviceTreeCtrl.GetItemData(hti));

			bflag=FALSE;

			strFileDir=m_DeviceTreeCtrl.GetItemText(htitemp);


		}

        int left=strFileDir.Find('(');
		int right=strFileDir.Find(')');

        CString strIP=strFileDir.Mid(left+1,right-left-1);

		m_sFtpSeverInfo.serverIP=strIP;

	}

	return strFileDir;

}


void CShowPageDlg::OnNMDblclkDeviceTree(NMHDR *pNMHDR, LRESULT *pResult)
{
	//˫��������
	//���ݿ���ʾ
	UpdateData(TRUE);
	m_DeviceListCtrl.DeleteAllItems();

    m_ptrFileList.RemoveAll();
		//��Ҫ�ȴ��������
	  //  OnDownloadrecorddb();

	//���Ĭ��ɫ
	FillTreeCtrlDefaultColor();

	CString  strFileDir=SetFtpServerIP();

	//��ȡ¼������
	if (m_RecorderType.GetCurSel()==0)
	{
		m_eRecordType=EVENT_RECORDER;
	}
	else if (m_RecorderType.GetCurSel()==1)
	{
        m_eRecordType=RECYCLE_RECORDER;
	}
	

	CWaitCursor waitCursor;


	OnShowrecorddb();
	
	if (m_Sock.m_hSocket!=INVALID_SOCKET)
	{
		//m_Sock.Close();
		//m_Sock.m_hSocket=INVALID_SOCKET;
	}
	
	//��������
	m_bDowningFlag=FALSE;

	*pResult = 1;  //˫������չ����Ҫ�ѷ���0����Ϊ1;

	waitCursor.Restore();

}

void  CShowPageDlg::FillTreeCtrlDefaultColor()
{
	//���Ĭ����ɫ
	    HTREEITEM   hRoot   =   m_DeviceTreeCtrl.GetRootItem();   
	  
		 HTREEITEM   hGrandNextItem   ;  
		 HTREEITEM   hNextItem;   
		 HTREEITEM   hChildItem;

		m_DeviceTreeCtrl.SetItemColor(hRoot,TREE_ITEM_NOACTIVE_COLOR);
		while(hRoot   !=   NULL)   
		{   
			hChildItem=   m_DeviceTreeCtrl.GetChildItem(hRoot);
			m_DeviceTreeCtrl.SetItemColor(hChildItem,TREE_ITEM_NOACTIVE_COLOR);

			Invalidate(TRUE);
			while(hChildItem!=NULL)
			{
			
				hGrandNextItem = m_DeviceTreeCtrl.GetNextItem(hChildItem,TVGN_NEXT);   
				hChildItem   =   hGrandNextItem;   
				m_DeviceTreeCtrl.SetItemColor(hChildItem,TREE_ITEM_NOACTIVE_COLOR);
                     Invalidate(TRUE);
			}
			
			hNextItem   =   m_DeviceTreeCtrl.GetNextItem(hRoot,   TVGN_NEXT);   
			hRoot   =   hNextItem;   

			m_DeviceTreeCtrl.SetItemColor(hNextItem,TREE_ITEM_NOACTIVE_COLOR);

			Invalidate(TRUE);
		}   
	  
	
	Invalidate(FALSE);



}

void CShowPageDlg::AutoAdjustColumnWidth(CListCtrl *pListCtrl)
{

	pListCtrl->SetRedraw(FALSE);  
	CHeaderCtrl *pHeader = pListCtrl->GetHeaderCtrl();  
	int nColumnCount = pHeader->GetItemCount();  

	for(int i = 0; i < nColumnCount-1; i++)  
	{  
		pListCtrl->SetColumnWidth(i, LVSCW_AUTOSIZE);  
		int nColumnWidth = pListCtrl->GetColumnWidth(i);  
		pListCtrl->SetColumnWidth(i, LVSCW_AUTOSIZE_USEHEADER);  
		int nHeaderWidth = pListCtrl->GetColumnWidth(i);  

		pListCtrl->SetColumnWidth(i, max(nColumnWidth, nHeaderWidth)+20);  
	}  
	pListCtrl->SetRedraw(TRUE);  
	pListCtrl->Invalidate(TRUE);
}
HBRUSH CShowPageDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);

	//���ı���
	switch(nCtlColor)
	{
	case CTLCOLOR_DLG:
	case CTLCOLOR_STATIC:
		{
			
			pDC-> SetBkColor(DLG_BK_COLOR);       //���屳��ɫ ��ע��SetBkColor�������������ı���ɫ�����ǿؼ��ı���ɫ
			pDC->SetBkMode(TRANSPARENT); 
		}

		return m_brBrush;
	}

	return hbr;
}

void CShowPageDlg::OnNMClickDeviceList(NMHDR *pNMHDR, LRESULT *pResult)
{
	static BOOL flag=TRUE;
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);


	if (m_bDowningFlag)
	{
		
		
		UINT pos=m_DeviceListCtrl.GetCellProgressValue(m_nCurDownLoadItem,DOWN_LOAD_SUBITEM_INDEX);

		if (pos<=99)
		{
			AfxMessageBox("��������,�����ĵȴ���");
			return;
		}
		else
		{
			m_DeviceListCtrl.SetItemImage(m_nCurDownLoadItem,DOWN_LOAD_ICON_INDEX,IDB_SmallDownLoad);
			m_bDowningFlag=FALSE;
		}
            	
			
	}

	CPoint pt;
	::GetCursorPos(&pt);
	::ScreenToClient(m_DeviceListCtrl.m_hWnd,&pt);	

    int nCount=m_DeviceListCtrl.GetItemCount();
  

   for(int i=0;i<nCount;i++)
   {
	    
	    CRect rect;
 
		//Skip//ͼ��������
	    m_DeviceListCtrl.GetSubItemRect(i,DOWN_LOAD_ICON_INDEX,LVIR_BOUNDS,rect);

		rect.left+=ICON_OFFSET;
        rect.right=rect.left+ICON_WIDTH;
		if(rect.PtInRect(pt))
		{

			EnterCriticalSection(&m_csProject);
			//��Ҫ�ж��Ƿ��������أ�������������أ����ܿ�����һ����������
			//���������ͼ�꣺����ͼ���Ƿ���ͬһ��
			if (flag)
			{
				 flag=FALSE;
               //���û������������
				  m_bDowningFlag=TRUE; //��ʾ��������
				  m_DeviceListCtrl.SetItemImage(i,DOWN_LOAD_ICON_INDEX,IDB_PauseDownLoad);
				  m_DeviceListCtrl.InvalidateRect(&rect);
				 
				//����
				//���õ�ǰ���ص���
                 SetCurDownLoadItem(i);

				//��ȡitem������ID
			//	CString strFileName=m_DeviceListCtrl.GetItemText(i,1);
				CString strFileName=GetFileNameFromItem(i);
				DownLoadState eState;
				BOOL bflag=m_StateMap.Lookup(strFileName,eState);
				//��Ҫ��ʼ��m_StateMap
				if (!bflag)
				{

					m_StateMap.SetAt(strFileName,DWL_START);
				}
                else
				{
					m_StateMap.SetAt(strFileName,DWL_RESUME);
				}


				 PostMessage(WM_DOWNLOAD_MSG_PROCESS);

				 break;
				
			}
			else
			{
				flag=TRUE;
				//SetCurDownLoadItem(i);
				m_DeviceListCtrl.SetItemImage(i,DOWN_LOAD_ICON_INDEX,IDB_SmallDownLoad);
				m_DeviceListCtrl.InvalidateRect(&rect);
				
				//��ͣ����
				//CString strFileName=m_DeviceListCtrl.GetItemText(i,1);
			    CString strFileName=GetFileNameFromItem(i);
				DownLoadState eState;
				BOOL bflag=m_StateMap.Lookup(strFileName,eState);
				//��Ҫ��ʼ��m_StateMap
				if (bflag)
				{
					m_StateMap.SetAt(strFileName,DWL_PAUSE);
				}

				break;
				
			}

			LeaveCriticalSection(&m_csProject);
		}

		
		
	}
	

   
	*pResult = 0;
	
}

CString CShowPageDlg::GetFileNameFromItem(UINT nRow)
{

	    CString strFileName;

        CString nChannel=m_DeviceListCtrl.GetItemText(nRow,1);

		CString nStartTime=m_DeviceListCtrl.GetItemText(nRow,2);

		CString nLengthTime=m_DeviceListCtrl.GetItemText(nRow,3);


		CString sStartYear=nStartTime.Mid(0,4); //ȡ��
		CString sStartMonth=nStartTime.Mid(5,2); //ȡ��
		CString sStartDay=nStartTime.Mid(8,2); //ȡ��
		CString sStartHour=nStartTime.Mid(11,2); //ȡʱ
		CString sStartMintue=nStartTime.Mid(14,2); //ȡ��
		CString sStartSecond=nStartTime.Mid(17,2); //ȡ��


		//��ʱ�����ڸ���ʱ
		int nLen=nLengthTime.GetLength();
		int posL=nLengthTime.Find(":");
		int posR=nLengthTime.ReverseFind(':');

		int nHour=atoi(nLengthTime.Left(posL));
	
		int nMinute=atoi(nLengthTime.Mid(posL+1,posR-posL-1));
		
		int nSecond=atoi(nLengthTime.Right(nLen-posR-1));

		SYSTEMTIME st;
		st.wYear=atoi(sStartYear);
		st.wMonth=atoi(sStartMonth);
		st.wDay=atoi(sStartDay);
		st.wHour=atoi(sStartHour);
		st.wMinute=atoi(sStartMintue);
		st.wSecond=atoi(sStartSecond);
		
		CTime tStart(st);

		UINT nStartSecond=tStart.GetTime();

		int nRecordLength=nHour*3600+nMinute*60+nSecond;

		
		

        UINT nEndSecond=nStartSecond+nRecordLength;

		CTime tEnd(nEndSecond);



		strFileName.Format(_T("%s-%04d%02d%02d_%02d%02d%02d-%04d%02d%02d_%02d%02d%02d.%s"),
			nChannel,tStart.GetYear(),tStart.GetMonth(),tStart.GetDay()
			,tStart.GetHour(),tStart.GetMinute(),tStart.GetSecond()
			,tEnd.GetYear(),tEnd.GetMonth(),tEnd.GetDay()
			,tEnd.GetHour(),tEnd.GetMinute(),tEnd.GetSecond()
			,FILE_SHOW_SUFFIX_FORMAT);


		


		return strFileName;

}

BOOL CShowPageDlg::GetRecycleFilePosFromItemDB(UINT nRow,__int64& nStartPos,__int64& nEndPos)
{
	//���Ȼ�ȡItem���еĲ�ѯ������¼��ͨ��+¼����ʼʱ��ͽ���ʱ��
	CString strFileName= m_strCurDevFilePath;

	CString nstrChannel=m_DeviceListCtrl.GetItemText(nRow,1);

	int nChannel=atoi(nstrChannel.GetBuffer(0));

	CString nStartTime=m_DeviceListCtrl.GetItemText(nRow,2);

	CString nLengthTime=m_DeviceListCtrl.GetItemText(nRow,3);


	CString sStartYear=nStartTime.Mid(0,4); //ȡ��
	CString sStartMonth=nStartTime.Mid(5,2); //ȡ��
	CString sStartDay=nStartTime.Mid(8,2); //ȡ��
	CString sStartHour=nStartTime.Mid(11,2); //ȡʱ
	CString sStartMintue=nStartTime.Mid(14,2); //ȡ��
	CString sStartSecond=nStartTime.Mid(17,2); //ȡ��


	//��Ҫ���Ǹ������
	/*
	int nHour=atoi(nLengthTime.Mid(0,2));
	int nMinute=atoi(nLengthTime.Mid(3,2));
	int nSecond=atoi(nLengthTime.Mid(6,2));
	*/
	int nLen=nLengthTime.GetLength();
	int posL=nLengthTime.Find(":");
	int posR=nLengthTime.ReverseFind(':');
	int nHour=atoi(nLengthTime.Left(posL));
	int nMinute=atoi(nLengthTime.Mid(posL+1,posR-posL-1));
	int nSecond=atoi(nLengthTime.Right(nLen-posR-1));

	SYSTEMTIME st;
	st.wYear=atoi(sStartYear);
	st.wMonth=atoi(sStartMonth);
	st.wDay=atoi(sStartDay);
	st.wHour=atoi(sStartHour);
	st.wMinute=atoi(sStartMintue);
	st.wSecond=atoi(sStartSecond);

	CTime tStart(st);

	UINT nStartSecond=tStart.GetTime();

	int nRecordLength=nHour*3600+nMinute*60+nSecond;

	UINT nEndSecond=nStartSecond+nRecordLength;

	CTime tEnd(nEndSecond);

	
	//������ѯ���
	   CString strSQL; 

		//ֻ��ѯѭ��¼���ļ����ݿ�
		strFileName+=RECORD_RECYCLE_DB_FILENAME;
		strSQL.Format("SELECT * FROM %s where chn=%d and start_time=%d and stop_time=%d",RECORD_RECYCLE_DB_TABLE_FILENAME,nChannel,nStartSecond,nEndSecond);
	
		//�����ݿ�
		OpenDataBase(strFileName);

		//��ѯ���ݿ��ȡ�ü�¼���ļ���ʼλ�úͽ���λ��
	try
	{
		m_pSQLiteStatement->Sql(strSQL);

		int nColCount=m_pSQLiteStatement->GetColumnCount();

		int row=0;
		int col=0;
		int nlCount=0;
		CString strColName;
		
		
		while(m_pSQLiteStatement->FetchRow())
		{
			BOOL  bShow=m_pSQLiteStatement->GetColumnBool(nColCount-1);

			if (!bShow)
			{
				//���Ϊ����ȡ��һ��
				continue;
			}

			for(int col = 0; col <nColCount-1; col++)
			{
				const char* pname=m_pSQLiteStatement->GetColumnOriginName(col);

				strColName.Format(_T("%s"),pname);

				if (strColName==_T("start_pos"))
				{
					//��ȡ��ʼλ��
					nStartPos=m_pSQLiteStatement->GetColumnInt64(col);
				}
				else if (strColName==_T("stop_pos"))
				{
					//��ȡ����λ��
					nEndPos=m_pSQLiteStatement->GetColumnInt64(col);
					
				}
				else
				{
					continue;
				}
				
			}

			
		}
		
		m_pSQLiteStatement->FreeQuery();
		
	}
	catch (CMemoryException* e)
	{
		AfxMessageBox("�ڴ�New����",MB_OK,MB_ICONERROR);
		return FALSE;
	}
	catch (CFileException* e)
	{
		AfxMessageBox("�ļ��򿪴���",MB_OK,MB_ICONERROR);
		return FALSE;
	}
	catch (CException* e)
	{
		AfxMessageBox("δ֪����",MB_OK,MB_ICONERROR);
		return FALSE;
	}

	

	CloseDataBase(strFileName);
	
	

	return TRUE;

	


}

void CShowPageDlg::OnBnClickedPlaybtn()
{

	//1��ѡ�񲥷��ļ�
	CString strFilename;
	CString strPlayFileName;
	POSITION pos=m_DeviceListCtrl.GetFirstSelectedItemPosition();

    static CString strCurFileName=_T(""); //��ǰ�����ļ�

	if(pos!=NULL)
	{
        int indexItem=m_DeviceListCtrl.GetNextSelectedItem(pos);

       // strFilename=m_DeviceListCtrl.GetItemText(indexItem,1);

		strFilename=GetFileNameFromItem(indexItem);


		//����·��
		strPlayFileName=m_strCurDevFilePath;

		if (m_eRecordType==EVENT_RECORDER)
		{

			strPlayFileName+=RECORD_EVENT_FILE_DIR;
			strPlayFileName+=strFilename;
		}
		else if (m_eRecordType==RECYCLE_RECORDER)
		{
            strPlayFileName+=RECORD_RECYCLE_FILE_DIR;
			strPlayFileName+=strFilename;
		}
		

		


		//���Ҹ��ļ��Ƿ����
		if (!PathFileExists(strPlayFileName))
		{
            CString str;
			str.Format("%s�ļ�û������",strPlayFileName);
			AfxMessageBox(str);

			return;

		}


        //���õ�ǰ�����ļ�
		if (strCurFileName!=strPlayFileName)
		{
           strCurFileName=strPlayFileName;

		   //����ʼ������ʵ��
		   if(m_Player!=NULL)
		   {
			   m_Player->Stop();
			   m_Player->Release();
			   m_Player=NULL;
		   }
		}

		m_strConnectInfo.SetTextColor(RGB(27,40,252));

		CString strInfo;

		strInfo.Format("���ڲ���%s",strFilename);

		//strInfo.Format("�����ĵȴ�ͬ��¼���������");

		GetDlgItem(IDC_ConnectText)->SetWindowText(strInfo);

		m_strConnectInfo.ShowWindow(SW_SHOW);


        #ifdef _DEBUG
      //  strFilename="C:\\Users\\Administrator\\Desktop\\3_1970_1_1_0_5_40.aac";
        #endif

        #ifdef _DEBUG
		//strFilename="C:\\Users\\Administrator\\Desktop\\���԰���.mp3";
        #endif


	}
	else
	{
		 AfxMessageBox("��ѡ��һ�������ļ�",MB_ICONINFORMATION);
		 return;
	}

	//����¼��
	if (m_bPlayflag)
	{
		//��ͣ����
		if (m_Player)
		{
			m_Player->Pause();

			CString strInfo;

			strInfo.Format("ֹͣ����%s",strFilename);

			//strInfo.Format("�����ĵȴ�ͬ��¼���������");

			GetDlgItem(IDC_ConnectText)->SetWindowText(strInfo);
		}
		
		ShowPlayBtn(TRUE);
	}
	else
	{
	   //��ʼ���������ļ�
		//�����ļ�����Ҫ������չ��
      //  strCurFileName.Replace(FILE_SHOW_SUFFIX_FORMAT,FILE_SUFFIX_REAL_FORMAT);
		if (PlayAAcFile(strCurFileName))
		{
			 ShowPlayBtn(FALSE);
		}
		

		//PlayAAcStream(m_PlayBuffer,PLAY_BUF_SIZE);
		
       
	}
	
}

BOOL CShowPageDlg::PlayAAcFile(CString& filename)
{
	if (m_Player==NULL)
	{
		m_bStreamFlag=FALSE; //�ļ�����
		m_Player = CreateZPlay();


		m_Player->SetCallbackFunc(myCallbackFunc, (TCallbackMessage)MsgStopAsync, this); 


        TStreamFormat nFormat=m_Player->GetFileFormat(filename);
		if (nFormat==sfUnknown)
		{
			// display error message
			CString strError;
			strError.Format("Error: %s\n", m_Player->GetError());
			m_gGlobalVF.WriteLog(strError);
			AfxMessageBox("�����ļ���ʽ��֧��!",MB_OK,MB_ICONWARNING);
			m_Player->Release();
			m_Player=NULL;
			return FALSE;

		}

		//���ڿͻ�������MP3ʵ������AAC��ʽ�ġ�

		//int result = m_Player->OpenFile(filename, sfAacADTS);
		int result = m_Player->OpenFile(filename, sfAacADTS);
		if(result == 0)
		{
			// display error message
			//����������MP3�����
			int result = m_Player->OpenFile(filename, sfAutodetect);
			if(result == 0)
			{
				CString strError;
				strError.Format("Error: %s\n", m_Player->GetError());
				m_gGlobalVF.WriteLog(strError);
				AfxMessageBox("�򿪲����ļ�����!",MB_OK,MB_ICONERROR);
				m_Player->Release();
				m_Player=NULL;
				return FALSE;
			}
		}


		//���ڲ���16Kʱ����ִ������������ֽ�����ת��ΪAAC 32K��ʽ
		// get song length
		TStreamInfo info;
		m_Player->GetStreamInfo(&info);

		CString strSongInfo;

		strSongInfo.Format("Length: %02u:%02u:%02u:%03u\n\n", info.Length.hms.hour, 
			info.Length.hms.minute,
			info.Length.hms.second,
			info.Length.hms.millisecond);

		m_PlayTotalLength=info.Length.hms.hour*3600+info.Length.hms.minute*60+info.Length.hms.second+info.Length.hms.millisecond*1.0/1000;
		//AfxMessageBox(strSongInfo);
	}

	if (m_Player==NULL)
	{
		m_gGlobalVF.WriteLog("��ʼ��������ʵ��ʧ��!");
		AfxMessageBox("��ʼ��������ʵ��ʧ��!",MB_OK,MB_ICONERROR);
		return FALSE;
	}

	//����Ƶ��
	// set graph type to AREA, left channel on top
	m_Player->SetFFTGraphParam(gpGraphType, gtAreaRightOnTop);
	// set linear scale
	m_Player->SetFFTGraphParam(gpWindow, fwRectangular);
	m_Player->SetFFTGraphParam(gpSubgrid, 0);
	m_Player->SetFFTGraphParam(gpFrequencyScaleVisible,0); 
	m_Player->SetFFTGraphParam(gpDecibelScaleVisible,0); 
	m_Player->SetFFTGraphParam(gpFrequencyGridVisible,0);
	m_Player->SetFFTGraphParam(gpDecibelGridVisible,0);
	m_Player->SetFFTGraphParam(gpColor2 ,RGB(255,255,255));
	m_Player->SetFFTGraphParam(gpColor14 ,RGB(146,216,12)); 
	m_Player->SetFFTGraphParam(gpColor15 ,RGB(146,216,12)); 

	

	// start playing
	m_Player->Play();

	if (m_nSliderTimer == 0)
	{
		m_nSliderTimer = SetTimer(SLIDER_TIMER, 100, NULL);
		
	}

	return TRUE;


}

void CShowPageDlg::PlayAAcStream(void* buffer,unsigned int len)
{
	//������Ƶ��
	if (m_Player==NULL)
	{
		m_bStreamFlag=TRUE; //������
		m_Player = CreateZPlay();

//#ifdef _DEBUG

		// detect stream format
		CString strFilename="C:\\Users\\Administrator\\Desktop\\���԰���.mp3";
		TStreamFormat format = m_Player->GetFileFormat(strFilename);
		if(format == sfUnknown)
		{
			printf("Unsupported format\n");
			m_Player->Release();
			return;
		}

		m_inFile = fopen(strFilename.GetBuffer(0), "rb"); // open input file
		if(m_inFile == NULL)
		{
			printf("Can't open mp3.");
			m_Player->Release();
			return;
		}
//#endif
		// set callback mechanism to intercept MsgStreamNeedMoreData message
		// send file handle as user parameter to callback function
		 // buffer for stream data
		// load some starting data into memory buffer,
		// this data must contain at least, one valid mp3 frame
		unsigned int read = fread(buffer, 1, PLAY_BUF_SIZE, m_inFile);



	//	m_Player->SetCallbackFunc(myCallbackFunc, (TCallbackMessage) MsgStreamNeedMoreData, this); 

		m_Player->SetCallbackFunc(myCallbackFunc, (TCallbackMessage) MsgStopAsync, this); 
		int result = m_Player->OpenStream(1,1,buffer,read, format);
		if(result == 0)
		{
			// display error message
			CString errorStr;
			errorStr.Format("Error: %s\n", m_Player->GetError());
			AfxMessageBox(errorStr,MB_OK,MB_ICONERROR);
			m_Player->Release();
			return;
		}

		// get song length
		TStreamInfo info;
		m_Player->GetStreamInfo(&info);
		CString strSongInfo;
		strSongInfo.Format("Length: %02u:%02u:%02u:%03u\n\n", info.Length.hms.hour, 
			info.Length.hms.minute,
			info.Length.hms.second,
			info.Length.hms.millisecond);

		//m_PlayTotalLength=info.Length.hms.hour*3600+info.Length.hms.minute*60+info.Length.hms.second+info.Length.hms.millisecond*1.0/1000;
		//AfxMessageBox(strSongInfo);
	}

	if (m_Player==NULL)
	{
		m_gGlobalVF.WriteLog("��ʼ��������ʵ��ʧ��!");
		return;
	}


	// start playing
	m_Player->Play();

	if (m_nSliderTimer == 0)
	{
		m_nSliderTimer = SetTimer(SLIDER_TIMER, 100, NULL);
		return;
	}




}

void CShowPageDlg::OnBnClickedPlaynextbtn()
{
	// ǰ������
	//��λ����󲥷�
	if (m_Player)
	{
		//�ļ�����
		double pospercent = 0;
		//����99
		//TStreamTime tstreamTime;
		//m_Player->GetPosition(&tstreamTime);
		//double Curlength=tstreamTime.hms.hour*3600+tstreamTime.hms.minute*60+tstreamTime.hms.second+tstreamTime.hms.millisecond*1.0/1000;
		//pos =Curlength/m_PlayTotalLength;

		pospercent =0.999;

		TStreamTime tstreamTime;
		//m_Player->GetPosition(&tstreamTime);

		//double Curlength=tstreamTime.hms.hour*3600+tstreamTime.hms.minute*60+tstreamTime.hms.second+tstreamTime.hms.millisecond*1.0/1000;
		double pos =m_PlayTotalLength*pospercent;

		tstreamTime.hms.hour=pos/3600;
		tstreamTime.hms.minute=(pos-tstreamTime.hms.hour*3600)/60;
		tstreamTime.hms.second=(pos-tstreamTime.hms.hour*3600-tstreamTime.hms.minute*60);
		tstreamTime.hms.millisecond=(pos-tstreamTime.hms.hour*3600-tstreamTime.hms.minute*60-tstreamTime.hms.second)*1000;

		if (!m_bStreamFlag)
		{
			int nResult=m_Player->Seek(tfHMS,&tstreamTime,smFromBeginning);
			if (nResult==0)
			{
				CString strError;
				strError.Format("Error=%s",m_Player->GetError());
				AfxMessageBox(strError,MB_OK,MB_ICONERROR);
			}
		}



	}
}

void CShowPageDlg::OnBnClickedPlaybackbtn()
{
	// ���˲���
	if (m_Player)
	{
		//�ļ�����
		double pospercent = 0;
		//����99
		//TStreamTime tstreamTime;
		//m_Player->GetPosition(&tstreamTime);
		//double Curlength=tstreamTime.hms.hour*3600+tstreamTime.hms.minute*60+tstreamTime.hms.second+tstreamTime.hms.millisecond*1.0/1000;
		//pos =Curlength/m_PlayTotalLength;

		pospercent =0;

		TStreamTime tstreamTime;
		//m_Player->GetPosition(&tstreamTime);

		//double Curlength=tstreamTime.hms.hour*3600+tstreamTime.hms.minute*60+tstreamTime.hms.second+tstreamTime.hms.millisecond*1.0/1000;
		double pos =m_PlayTotalLength*pospercent;

		tstreamTime.hms.hour=pos/3600;
		tstreamTime.hms.minute=(pos-tstreamTime.hms.hour*3600)/60;
		tstreamTime.hms.second=(pos-tstreamTime.hms.hour*3600-tstreamTime.hms.minute*60);
		tstreamTime.hms.millisecond=(pos-tstreamTime.hms.hour*3600-tstreamTime.hms.minute*60-tstreamTime.hms.second)*1000;

		if (!m_bStreamFlag)
		{
			int nResult=m_Player->Seek(tfHMS,&tstreamTime,smFromBeginning);
			if (nResult==0)
			{
				CString strError;
				strError.Format("Error=%s",m_Player->GetError());
				AfxMessageBox(strError,MB_OK,MB_ICONERROR);
			}
		}



	}
	

}

void CShowPageDlg::ShowPlayBtn(BOOL flag)
{

	if(flag)
	{
		//m_PlayBtn.SetIcon(IDI_PLAYPRESS,IDI_PLAY);

		m_PlayBtn.SetIcon(IDI_PLAY,IDI_PLAY);
		m_PlayBtn.SetColor(CButtonST::BTNST_COLOR_BK_OUT,RGB(255,255,255));
		m_PlayBtn.SetColor(CButtonST::BTNST_COLOR_BK_IN,RGB(255,255,255));
		m_PlayBtn.SetColor(CButtonST::BTNST_COLOR_BK_FOCUS,RGB(255,255,255));
		short	shplayBtnColor =255;
		m_PlayBtn.OffsetColor(CButtonST::BTNST_COLOR_BK_IN, shplayBtnColor);

		m_PlayBtn.DrawTransparent(TRUE);
		m_bPlayflag=FALSE;
	}
	else
	{
		//��ʾͼ��
		//m_PlayBtn.SetIcon(IDI_PAUSEPRESS,IDI_PAUSE);
		m_PlayBtn.SetIcon(IDI_PAUSE,IDI_PAUSE);

		m_PlayBtn.SetColor(CButtonST::BTNST_COLOR_BK_OUT,RGB(255,255,255));
		m_PlayBtn.SetColor(CButtonST::BTNST_COLOR_BK_IN,RGB(255,255,255));
		m_PlayBtn.SetColor(CButtonST::BTNST_COLOR_BK_FOCUS,RGB(255,255,255));
	
		short	shplayBtnColor =255;
		m_PlayBtn.OffsetColor(CButtonST::BTNST_COLOR_BK_IN, shplayBtnColor);

		m_bPlayflag=TRUE;
	}

}

void CShowPageDlg::ShowVolCtrlBtn(BOOL flag)
{
	//�������ư�ť
	if (flag)
	{
		m_SoundCtrlBtn.SetIcon(IDI_Sound);
		m_SoundCtrlBtn.SetColor(CButtonST::BTNST_COLOR_BK_OUT,RGB(255,255,255));
		m_SoundCtrlBtn.SetColor(CButtonST::BTNST_COLOR_BK_IN,RGB(255,255,255));
		m_SoundCtrlBtn.SetColor(CButtonST::BTNST_COLOR_BK_FOCUS,RGB(255,255,255));
		
		short	shBtnColor =255;
		m_SoundCtrlBtn.OffsetColor(CButtonST::BTNST_COLOR_BK_IN, shBtnColor);

		m_VoCtrlFlag=FALSE;
	}
	else
	{
		m_SoundCtrlBtn.SetIcon(IDI_SoundMute);
		m_SoundCtrlBtn.SetColor(CButtonST::BTNST_COLOR_BK_OUT,RGB(255,255,255));
		m_SoundCtrlBtn.SetColor(CButtonST::BTNST_COLOR_BK_IN,RGB(255,255,255));
		m_SoundCtrlBtn.SetColor(CButtonST::BTNST_COLOR_BK_FOCUS,RGB(255,255,255));

		short	shBtnColor =255;
		m_SoundCtrlBtn.OffsetColor(CButtonST::BTNST_COLOR_BK_IN, shBtnColor);
		m_VoCtrlFlag=TRUE;

	}

}
void CShowPageDlg::OnBnClickedSoundctrlbtn()
{
	//��������
	if (m_VoCtrlFlag)
	{
		//������
		int index=100;
		m_MusicVolumeCtrl.Enable(TRUE);
		m_MusicVolumeCtrl.SetPos(index);

       if(m_Player)
	   {
          m_Player->SetMasterVolume(index,index);
	   }

		ShowVolCtrlBtn(TRUE);
		
	}
	else
	{

		int index=0;
		//m_MusicVolumeCtrl.SetPos(index);

		m_MusicVolumeCtrl.Enable(FALSE);

		if(m_Player)
		{
			m_Player->SetMasterVolume(index,index);
		}
        //����
        ShowVolCtrlBtn(FALSE);
		
	}

	CRect rect;  
	m_MusicVolumeCtrl.GetClientRect(&rect);  
	ScreenToClient(&rect);
	InvalidateRect(&rect);   //���ԭ�б�  
	//m_MusicVolumeCtrl.MoveWindow(&rect);
	/*
	if (m_hParentWnd!=NULL)
	{
		CWnd* pwnd=CWnd::FromHandle(m_hParentWnd);
		//pwnd->ShowWindow(SW_MINIMIZE);
		//pwnd->ShowWindow(SW_RESTORE);
	}
	*/
	

}

BOOL CShowPageDlg::OnEraseBkgnd(CDC* pDC)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ

	return TRUE;
	//return CDialogEx::OnEraseBkgnd(pDC);
}

void CShowPageDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	
	//�϶��ؼ�
		if (pScrollBar->GetSafeHwnd() == m_PlayProgressCtrl.GetSafeHwnd())
		{  
			//���Ž���
			if(m_Player)
			{

				TStreamTime tstreamTime;
				//m_Player->GetPosition(&tstreamTime);

				//double Curlength=tstreamTime.hms.hour*3600+tstreamTime.hms.minute*60+tstreamTime.hms.second+tstreamTime.hms.millisecond*1.0/1000;
				double pos =m_PlayTotalLength*m_PlayProgressCtrl.GetPos()/PLAYER_RANGE;

				tstreamTime.hms.hour=pos/3600;
				tstreamTime.hms.minute=(pos-tstreamTime.hms.hour*3600)/60;
				tstreamTime.hms.second=(pos-tstreamTime.hms.hour*3600-tstreamTime.hms.minute*60);
				tstreamTime.hms.millisecond=(pos-tstreamTime.hms.hour*3600-tstreamTime.hms.minute*60-tstreamTime.hms.second)*1000;

				if (!m_bStreamFlag)
				{
					int nResult=m_Player->Seek(tfHMS,&tstreamTime,smFromBeginning);
					if (nResult==0)
					{
						CString strError;
						strError.Format("Error=%s",m_Player->GetError());
						AfxMessageBox(strError,MB_OK,MB_ICONERROR);
					}
				}



			}

		}
		else if(pScrollBar->GetSafeHwnd() == m_MusicVolumeCtrl.GetSafeHwnd())
		{
			//��������
			if (m_Player)
			{
				int index=m_MusicVolumeCtrl.GetPos();

				m_Player->SetMasterVolume(index,index);
			}

		}


		CDialogEx::OnHScroll(nSBCode, nPos, pScrollBar);

	
	
	
}


LRESULT CShowPageDlg::OnBitmapSliderMoved(WPARAM wParam, LPARAM lParam)
{
	CString sMsg;


	switch( wParam ) {

	case IDC_SoundVolume :
		{
			//��������
			if (m_Player)
			{
				int index=m_MusicVolumeCtrl.GetPos();

				m_Player->SetMasterVolume(index,index);
			}
		}
		break;
	default:
		break;	
	}
	return 0;
}
void CShowPageDlg::OnTimer(UINT_PTR nIDEvent)
{
	// ��ʱ��

    switch(nIDEvent)
	{
	case SLIDER_TIMER:
		{
             ProcessShowSliderPos();
		}
		break;
	case DOWN_LOAD_TIMER:
		{
             ProcessDownLoadPos();
		}
		break;

	case CONN_WAIT_TIMER:
		{
            ShowConnWaitInfo();
		}
		break;

	}
	

	CDialogEx::OnTimer(nIDEvent);
}

void CShowPageDlg::ShowConnWaitInfo()
{

   
   m_strConnectInfo.SetTextColor(RGB(27,40,252));

   CString strInfo;

   strInfo.Format("�����ĵȴ�ͬ��¼���������,����ʱ%d��",m_nWaitSecond);

   //strInfo.Format("�����ĵȴ�ͬ��¼���������");

   GetDlgItem(IDC_ConnectText)->SetWindowText(strInfo);

   m_strConnectInfo.ShowWindow(SW_SHOW);

   m_nWaitSecond++;

   if (m_nWaitSecond==30)
   {
	   m_strConnectInfo.SetWindowText(_T(""));
	   m_strConnectInfo.ShowWindow(SW_HIDE);
	   KillTimer(m_nWaitConnTimer);
	   UpdateWindow();


	   //�������ȴ�
	   EndWaitCursor();
	   ReleaseCapture();
	   
	   AfxMessageBox("ͬ��¼�����ݳ�ʱ��");


   }


}

void CShowPageDlg::ProcessDownLoadPos()
{
	//������ʾ���ؽ���
	 DWORD pos=m_FtpClient.GetDownloadFilePercent();
	  //����
	 unsigned int nRow=GetCurDownLoadItem();

	 m_DeviceListCtrl.SetCellProgressValue(nRow,DOWN_LOAD_SUBITEM_INDEX,pos);

	 if (pos>=99)
	 {
		 m_bDowningFlag=FALSE;
		 KillTimer(DOWN_LOAD_TIMER);
	 }
 


}

unsigned int  CShowPageDlg::GetCurDownLoadItem()
{
     
	   return m_nCurDownLoadItem;

}


void CShowPageDlg::ProcessShowSliderPos()
{
	if ((m_nSliderTimer==SLIDER_TIMER)&&(m_Player))
	{
		//���Ų�ͬλ��֡
		if (!m_bStreamFlag)
		{
			//�ļ�����
			double pos = 0;
			TStreamTime tstreamTime;
			m_Player->GetPosition(&tstreamTime);

			double Curlength=tstreamTime.hms.hour*3600+tstreamTime.hms.minute*60+tstreamTime.hms.second+tstreamTime.hms.millisecond*1.0/1000;
			pos =Curlength/m_PlayTotalLength;

			// ��ȡǰ��λ�ã�������Sliderλ��
			int newPos = int(pos * PLAYER_RANGE);
			if (m_PlayProgressCtrl.GetPos() != newPos)
			{

				m_PlayProgressCtrl.SetPos(newPos);
				m_PlayProgressCtrl._SetPos(newPos);
			}

			//����Ƶ��
			
			CRect rect;
			GetDlgItem(IDC_ShowVolSpectrum)->GetWindowRect(&rect);
			ScreenToClient(&rect);
			CWnd *hwnd;
			HWND hWnd;  
			hWnd    = GetDlgItem(IDC_ShowVolSpectrum)->GetSafeHwnd(); 
			hwnd=GetDlgItem(IDC_ShowVolSpectrum);
			//hwnd->ShowWindow(SW_SHOW);

			

			//m_Player->DrawFFTGraphOnHWND(&this->m_hWnd, rect.left, rect.top, rect.Width(), rect.Height());
		//m_Player->DrawFFTGraphOnHWND(NULL, rect.left, rect.top, 100, 100);
			if (rect.Width()<100||rect.Height()<100)
			{
				int width=rect.Width();
				int height=rect.Height();
				if (width<100)
				{
					width=100;
				}
				if (height<100)
				{
					//height=100;
				}
			//	m_Player->DrawFFTGraphOnHWND(NULL, rect.left, rect.top, width, height);

				m_Player->DrawFFTGraphOnHWND(this->m_hWnd, rect.left, rect.top, width, height);
                
			}
			else
			{
				
             //   m_Player->DrawFFTGraphOnHWND(NULL, rect.left, rect.top, rect.Width(), rect.Height());
				m_Player->DrawFFTGraphOnHWND(this->m_hWnd, rect.left, rect.top, rect.Width(), rect.Height());
			}

			/*
			if (pos>=100)
			{

				m_nSliderTimer=0;
				m_PlayProgressCtrl._SetPos(0);
				GetDlgItem(IDC_ConnectText)->SetWindowText(_T(""));
				KillTimer(m_nSliderTimer);
				m_Player->Stop();

			}
			*/
			
			ShowPlayTimeTips();

			
		}
		else
		{
			//������
			if(m_Player->IsStreamDataFree(m_PlayBuffer))
			{
				unsigned int nread=fread(m_PlayBuffer,1,PLAY_BUF_SIZE,m_inFile);
				m_Player->PushDataToStream(m_PlayBuffer,nread);

			}
		}
			
	}

}



void CShowPageDlg::ShowPlayTimeTips()
{

   if(m_Player)
   {
	   //���Ȼ�ȡ��ǰ���ڲ��ŵ�ʱ��
	   TStreamTime tstreamTime;
	   m_Player->GetPosition(&tstreamTime);

	   TStreamInfo tstreamInfo;
	   m_Player->GetStreamInfo(&tstreamInfo);
	 
	   CString strSongInfo;
	   strSongInfo.Format("%02u:%02u:%02u/%02u:%02u:%02u\n\n", tstreamTime.hms.hour,
		   tstreamTime.hms.minute,
		   tstreamTime.hms.second,
		   tstreamInfo.Length.hms.hour, 
		   tstreamInfo.Length.hms.minute,
		   tstreamInfo.Length.hms.second);

	   if (tstreamTime.hms.hour>tstreamInfo.Length.hms.hour)
	   {
            //�����쳣,��λ
		   strSongInfo.Format("%02u:%02u:%02u/%02u:%02u:%02u\n\n", 0,
			   0,
			   0,
			   tstreamInfo.Length.hms.hour, 
			   tstreamInfo.Length.hms.minute,
			   tstreamInfo.Length.hms.second);
		   m_nSliderTimer=0;
		   m_PlayProgressCtrl._SetPos(0);
		   m_PlayProgressCtrl.SetPos(0);
		   GetDlgItem(IDC_ConnectText)->SetWindowText(_T(""));
		   KillTimer(m_nSliderTimer);
		   m_Player->Stop();
	   }
	   else if ((tstreamTime.hms.hour==tstreamInfo.Length.hms.hour)&&(tstreamTime.hms.minute>tstreamInfo.Length.hms.minute))
	   {
		   //�����쳣,��λ
		   strSongInfo.Format("%02u:%02u:%02u/%02u:%02u:%02u\n\n", 0,
			   0,
			   0,
			   tstreamInfo.Length.hms.hour, 
			   tstreamInfo.Length.hms.minute,
			   tstreamInfo.Length.hms.second);
		   m_nSliderTimer=0;
		   m_PlayProgressCtrl._SetPos(0);
		   m_PlayProgressCtrl.SetPos(0);
		   GetDlgItem(IDC_ConnectText)->SetWindowText(_T(""));
		   KillTimer(m_nSliderTimer);
		   m_Player->Stop();
	   }
	   else if ((tstreamTime.hms.hour==tstreamInfo.Length.hms.hour)&&(tstreamTime.hms.minute==tstreamInfo.Length.hms.minute)&&(tstreamTime.hms.second>tstreamInfo.Length.hms.second))
	   {
		   //�����쳣,��λ
		   strSongInfo.Format("%02u:%02u:%02u/%02u:%02u:%02u\n\n", 0,
			   0,
			   0,
			   tstreamInfo.Length.hms.hour, 
			   tstreamInfo.Length.hms.minute,
			   tstreamInfo.Length.hms.second);
		   m_nSliderTimer=0;
		   m_PlayProgressCtrl._SetPos(0);
		   m_PlayProgressCtrl.SetPos(0);
		   GetDlgItem(IDC_ConnectText)->SetWindowText(_T(""));
		   KillTimer(m_nSliderTimer);
		   m_Player->Stop();
	   }
	   else if ((tstreamTime.hms.hour==tstreamInfo.Length.hms.hour)&&(tstreamTime.hms.minute==tstreamInfo.Length.hms.minute)&&(tstreamTime.hms.second==tstreamInfo.Length.hms.second))
	   {
           if ((tstreamInfo.Length.hms.hour!=0)||(tstreamInfo.Length.hms.minute!=0)||(tstreamInfo.Length.hms.second!=0))
           {
			   //���Ž���
			   m_nSliderTimer=0;
			   m_PlayProgressCtrl._SetPos(0);
			   m_PlayProgressCtrl.SetPos(0);
			   GetDlgItem(IDC_ConnectText)->SetWindowText(_T(""));
			  
			   m_Player->Stop();
			   KillTimer(m_nSliderTimer);
			   

           }
	   }

	  


	   GetDlgItem(IDC_PlayTimeShow)->SetWindowText(strSongInfo);

       GetDlgItem(IDC_PlayTimeShow)->ShowWindow(SW_SHOW);

   }
}

BOOL CShowPageDlg::ConnectFtpServer(CString serverIP,CString username,CString pwd,int port)
{
	 //���ӷ�����
	// ���ӷ�����
	m_FtpClient.SetMsgWnd(this->m_hWnd);
	m_FtpClient.setParm(serverIP,username,pwd,port);
	
	return m_FtpClient.login();
}

BOOL CShowPageDlg::DownLoadFileFromServer(CString strServerFileName,CString StrLocalFileName,BOOL flag,__int64 nStartPos,__int64 nEndPos)
{
	if (flag)
	{
		SetTimer(DOWN_LOAD_TIMER,100,NULL);
		m_bDBFlag=FALSE;
		//����������¼�¼���ļ�
		if (m_eRecordType==EVENT_RECORDER)
		{
			strServerFileName.Replace(FILE_SHOW_SUFFIX_FORMAT,FILE_SUFFIX_REAL_FORMAT);

			//��Ҫ�ڸ�Ŀ¼�µ��ļ���
			CString strFilePath=RECORD_EVENT_FILE_PATH;

			strServerFileName=strFilePath+strServerFileName;


		}
		else if(m_eRecordType==RECYCLE_RECORDER)
		{
			//ѭ��¼��
			return m_FtpClient.DownloadFileThread(strServerFileName,StrLocalFileName,nStartPos,nEndPos);

		}
		
        
		



	}
	else{
           m_bDBFlag=TRUE;
	}

	
	return m_FtpClient.DownloadFileThread(strServerFileName,StrLocalFileName);
}

DWORD CShowPageDlg::ConnectFTPServerThread(LPVOID lpParameter)
{
      CShowPageDlg* pdlg=(CShowPageDlg*)lpParameter;

  
	   
       
	   pdlg->DownLoadRecordDB();
	 






	   return 0;
}

BOOL  CShowPageDlg::PauseDownLoad()
{
     //��ͣ����
	m_FtpClient.PauseDownLoad();

	return TRUE;

}

BOOL CShowPageDlg::ResumeDownLoad()
{
	//��������
	m_FtpClient.ResumeDownLoad();

	return TRUE;
}

void CShowPageDlg::SetCurDownLoadItem(unsigned int index)
{

     m_nCurDownLoadItem=index;

}


DownLoadState CShowPageDlg::GetDownLoadState()
{

      return m_eDWLState;

}

void CShowPageDlg::SetDownLoadState(DownLoadState eState)
{
     m_eDWLState=eState;
}

BOOL CShowPageDlg::ProcessDownLoadMsg()
{

	//����״̬
	
	int nCount=m_DeviceListCtrl.GetItemCount();

	BOOL bDownLoad=FALSE;

	for (int i=0;i<nCount;i++)
	{
		
		// CString filename=m_DeviceListCtrl.GetItemText(i,1); //��ȡ�ļ���
		CString filename=GetFileNameFromItem(i);
		DownLoadState eDWLState;
		
		UINT curItem=i;

		if(m_StateMap.Lookup(filename,eDWLState))
		{
			switch(eDWLState)
			{

			case DWL_START:
				{
					
					
					 
					if (ConnectFtpServer(m_sFtpSeverInfo.serverIP,m_sFtpSeverInfo.username,m_sFtpSeverInfo.pwd,m_sFtpSeverInfo.port))
					{
						// CString strLocalFilename=m_sFtpSeverInfo.strLocalFileSavePath;
						

						CString strLocalFilename=m_sFtpSeverInfo.strLocalFileSavePath;
						
						
						m_sFtpSeverInfo.strRomoteFileName=filename;

						//����ѭ��¼���ļ�����ֻ����8���ļ��������Ҫ�����б�
						if(m_eRecordType==RECYCLE_RECORDER)
						{
                            strLocalFilename+=RECORD_RECYCLE_FILE_DIR;
							strLocalFilename+=filename;
							//��ȡѭ��¼���ļ�
							CString sChannel=m_DeviceListCtrl.GetItemText(i,1);
                            CString recyclefilename;
							recyclefilename.Format("recycle_%s",sChannel.GetBuffer(0));
							//m_sFtpSeverInfo.strRomoteFileName=recyclefilename;
							//��ȡ�����ļ�����ʼλ�úͽ���λ��

					        __int64 nStartPos=0;
							__int64 nEndPos=0;
							/*
							LARGE_INTEGER m_nFreq;
							LARGE_INTEGER m_nBeginTime;
							LARGE_INTEGER nEndTime;
							QueryPerformanceFrequency(&m_nFreq); // ��ȡʱ������
							QueryPerformanceCounter(&m_nBeginTime); // ��ʼ��ʱ
							GetRecycleFilePosFromItemDB(i,nStartPos,nEndPos);
							QueryPerformanceCounter(&nEndTime);// ������ʱ
							

							LONGLONG nMsTimer=(nEndTime.QuadPart-m_nBeginTime.QuadPart)*1000/m_nFreq.QuadPart; //ms
							TRACE1("ms=%ld",nMsTimer);
							*/
							
							GetRecycleFilePosFromItemDB(i,nStartPos,nEndPos);

							
							//ѭ��¼������
							BOOL flag=DownLoadFileFromServer(recyclefilename,strLocalFilename,TRUE,nStartPos,nEndPos);
							if (!flag)
							{
								m_bDownFlag=FALSE;
								m_bDowningFlag=FALSE;

								m_StateMap.RemoveKey(filename);
								m_DeviceListCtrl.SetItemImage(curItem,DOWN_LOAD_ICON_INDEX,IDB_SmallDownLoad);
								AfxMessageBox("����¼���ļ�ʧ�ܣ�",MB_OK);
							}
							else
							{
								bDownLoad=TRUE;
							}

						

						}
						else
						{

							strLocalFilename+=RECORD_EVENT_FILE_DIR;
							strLocalFilename+=filename;
							//�¼�¼������
							BOOL flag=DownLoadFileFromServer(filename,strLocalFilename);
							if (!flag)
							{
								m_bDownFlag=FALSE;
								m_bDowningFlag=FALSE;

								m_StateMap.RemoveKey(filename);
								m_DeviceListCtrl.SetItemImage(curItem,DOWN_LOAD_ICON_INDEX,IDB_SmallDownLoad);
								AfxMessageBox("����¼���ļ�ʧ�ܣ�",MB_OK);
							}
							else
							{
								bDownLoad=TRUE;
					        }


						}

						
					}
					else
					{
						//����ʧ��
						m_bDownFlag=FALSE;
						m_bDowningFlag=FALSE;
						
						m_StateMap.RemoveKey(filename);
						m_DeviceListCtrl.SetItemImage(curItem,DOWN_LOAD_ICON_INDEX,IDB_SmallDownLoad);
						AfxMessageBox("¼���豸δ����,�������磡",MB_OK);

					}
					
				}
				break;
			case DWL_PAUSE:
				{

					//�ҵ�����ͣ����
					PauseDownLoad();
					//bDownLoad=TRUE; //��ֹ�����غ���ļ���ε������֣������������������ļ�


				}
				break;

			case DWL_RESUME:
				{
					//��������
					ResumeDownLoad();
					bDownLoad=TRUE;


				}
				break;
			case DWL_END:
				{

                 

				}
				break;

			default:
				break;

			}




		}

		if (bDownLoad)
		{
			break;
		}


	}


	


	return  TRUE;

}

LRESULT CShowPageDlg::OnProcessDownLoadMsg(WPARAM wparam,LPARAM lparam)
{

	//�ĳɲ����̴߳���ʽ
	//����������Ϣ
	//EnterCriticalSection(&m_csProject);
	if (CreateThread(NULL,0,ProcessDownLoadMsgThread,this,0,NULL)==INVALID_HANDLE_VALUE)
	{
        m_gGlobalVF.WriteLog("���������ļ������߳�ʧ��");
		m_bDowningFlag=FALSE;
		
		return FALSE;
	}
	//LeaveCriticalSection(&m_csProject);
	
	return TRUE;
	//return OnProcessDownLoadMsg();
}


 DWORD CShowPageDlg::ProcessDownLoadMsgThread(LPVOID lpParameter)
 {
       CShowPageDlg* pDlg=(CShowPageDlg*)lpParameter;

	    
	   pDlg->ProcessDownLoadMsg();
	  

	   return 0;


 }

LRESULT CShowPageDlg::OnProcessFTPMsg(WPARAM wparam,LPARAM lparam)
{
	int nEventType=(int)wparam;

	switch(nEventType)
	{
	case  DIS_CONNECT_EVENT:
		{
			m_FtpClient.SetLoginFlag(FALSE);
			//��������
			m_bDownFlag=FALSE;
			m_bDowningFlag=FALSE;

			KillTimer(DOWN_LOAD_TIMER);
			
			AfxMessageBox("����Ͽ�,����������",MB_ICONINFORMATION);



		}
		break;
	case DOWNLOAD_FINISH_EVENT:
		{
			
			if (!m_bDBFlag)
			{
				

				m_bDowningFlag=FALSE;
				
				
				
				CString str;
				str.Format("%s¼���ļ��������",m_sFtpSeverInfo.strRomoteFileName);
				m_gGlobalVF.WriteLog(str);

				
				//��������ͼ�긴ԭ
				//ͨ���ļ����ҵ��������ص�Item��
				m_DeviceListCtrl.SetItemImage(m_nCurDownLoadItem,DOWN_LOAD_ICON_INDEX,IDB_SmallDownLoad);

				m_StateMap.SetAt(m_sFtpSeverInfo.strRomoteFileName,DWL_END);

				m_DeviceListCtrl.SetCellProgressValue(m_nCurDownLoadItem,DOWN_LOAD_SUBITEM_INDEX,100);
				
				KillTimer(DOWN_LOAD_TIMER);

				m_nCurDownCount++;

				m_FtpClient.logout();

				//KillTimer(DOWN_LOAD_TIMER);
			}
			else
			{

				if (m_bShow)
				{

					m_bShow=FALSE;
					DownLoadChannelConfig();
					


				}
				else
				{
					m_FtpClient.logout();


					//m_Sock.Close();
					if (m_nWaitConnTimer>0)
					{
						m_strConnectInfo.SetWindowText(_T(""));
						m_strConnectInfo.ShowWindow(SW_HIDE);
						//�ͷ����
						EndWaitCursor();
						ReleaseCapture();
						KillTimer(m_nWaitConnTimer);
						UpdateWindow();

					}

					

					AfxMessageBox("ͬ��¼����¼���!",MB_ICONINFORMATION);


					//���������б������
					SetRecorderTreeChannelName(&m_gGlobalVF.m_sDevInfo);

					

				}




				   

					//m_Sock.Close();
					//m_Sock.m_hSocket=INVALID_SOCKET;
					
					
			}
			
		}
		break;
	default:
		break;

	}


	return TRUE;
}


void CShowPageDlg::OnDownloadrecorddb()
{

	UpdateData(TRUE);
	//������Ҫ��TCP���ӷ�����
   
	////��ʼ���ȴ�
	BeginWaitCursor();
	SetCapture();



	BOOL flag=TRUE;
	if (m_Sock.m_hSocket==INVALID_SOCKET)
	{

		//flag=m_Sock.Create();


	}

	if (flag)
	{
		//�׽��ִ����ɹ�
		SRecordDevInfo*  pDevInfo=GetSelectTreeDevInfo();

		//DWORD IP ת��ΪCString 
		CString  strIP;  
		unsigned  char  *pIP  =  (unsigned  char*)&pDevInfo->m_dDevIP;  
		strIP.Format("%u.%u.%u.%u",*(pIP+3),  *(pIP+2),  *(pIP+1),  *pIP);

		m_gGlobalVF.m_ServerIP=strIP;
		m_gGlobalVF.m_nServerPort=pDevInfo->m_nDevPort;


		//m_Sock.Connect(m_gGlobalVF.m_ServerIP,m_gGlobalVF.m_nServerPort);
		

		
		 
		
	}

	//�����߳�
    if (CreateThread(NULL,0,ConnectFTPServerThread,this,0,NULL)==INVALID_HANDLE_VALUE)
    {
		m_gGlobalVF.WriteLog("��������FTP�������߳�ʧ�ܣ�");
    }
    
	m_nWaitSecond=0;
    m_nWaitConnTimer=SetTimer(CONN_WAIT_TIMER,1000,NULL);

	//DownLoadRecordDB();

}

BOOL CShowPageDlg::DownLoadRecordDB()
{
   

	// �������ݿ�������ļ�
	CString  strFileDir=SetFtpServerIP();

	if (ConnectFtpServer(m_sFtpSeverInfo.serverIP,m_sFtpSeverInfo.username,m_sFtpSeverInfo.pwd,m_sFtpSeverInfo.port))
	{
		//����֮�󣬵ȴ�������copy����
		//Sleep(5000); //��������
		//�������ݿ�
		CString FilePath=m_DBSavePath+strFileDir;
		FilePath+="\\";
		//�����ļ����·��
		m_sFtpSeverInfo.strLocalFileSavePath=FilePath;
		

		//����¼������������ѡ��¼�����ݿ��ļ�
		CString strDBFileName;
		if (m_eRecordType==EVENT_RECORDER)
		{
			 FilePath+=RECORD_EVENT_DB_FILENAME;
             strDBFileName=RECORD_EVENT_DB_FILENAME;
		}
		else if (m_eRecordType==RECYCLE_RECORDER)
		{
			 FilePath+=RECORD_RECYCLE_DB_FILENAME;
             strDBFileName=RECORD_RECYCLE_DB_FILENAME;
		}

		

		
		if (!DownLoadFileFromServer(strDBFileName,FilePath,FALSE))
		{
			CString str;
			str.Format("ͬ���豸%s��¼����¼�ļ�ʧ��!",m_sFtpSeverInfo.serverIP);
			m_gGlobalVF.WriteLog(str);
			//�Ͽ���������
			m_FtpClient.logout();//�˳�
			AfxMessageBox(str,MB_OK,MB_ICONINFORMATION);
		}
		


		
		
		


		return TRUE;

	}
	else
	{

         if (m_nWaitSecond<=30)
         {
			 m_strConnectInfo.SetWindowText(_T(""));
			 m_strConnectInfo.ShowWindow(SW_HIDE);
			 KillTimer(m_nWaitConnTimer);
			 UpdateWindow();
			 
         }

		 AfxMessageBox("ͬ��¼���ļ�ʧ��!");
	}


	


	return FALSE;



}

BOOL CShowPageDlg::DownLoadChannelConfig()
{

	// �������ݿ�������ļ�
	CString  strFileDir=SetFtpServerIP();

	if (ConnectFtpServer(m_sFtpSeverInfo.serverIP,m_sFtpSeverInfo.username,m_sFtpSeverInfo.pwd,m_sFtpSeverInfo.port))
	{
		//����֮�󣬵ȴ�������copy����
		//Sleep(5000); //��������
		//�������ݿ�
		CString FilePath=m_DBSavePath+strFileDir;
		FilePath+="\\";
		//�����ļ����·��
		m_sFtpSeverInfo.strLocalFileSavePath=FilePath;


		//����¼������������ѡ��¼�����ݿ��ļ�
		CString striniFileName;
		
		FilePath+=RECORD_CHANNEL_CONFIG_FILENAME;
		striniFileName=RECORD_CHANNEL_CONFIG_FILENAME;
		

		if (!DownLoadFileFromServer(striniFileName,FilePath,FALSE))
		{
			CString str;
			str.Format("ͬ���豸%s��ͨ�����������ļ�ʧ��!",m_sFtpSeverInfo.serverIP);
			m_gGlobalVF.WriteLog(str);
			//�Ͽ���������
			m_FtpClient.logout();//�˳�
			AfxMessageBox(str,MB_OK,MB_ICONINFORMATION);
		}

		return TRUE;

	}
	else
	{

		if (m_nWaitSecond<=30)
		{
			m_strConnectInfo.SetWindowText(_T(""));
			m_strConnectInfo.ShowWindow(SW_HIDE);
			KillTimer(m_nWaitConnTimer);
			UpdateWindow();

		}

		AfxMessageBox("ͬ��ͨ�����������ļ�ʧ��!");
	}





	return FALSE;
   

}

void CShowPageDlg::OnDownloadfile()
{
	// �����ļ�
	//��Ҫģ����Ӧ�����ĳһ�е����ذ�ť
	POSITION pos=m_DeviceListCtrl.GetFirstSelectedItemPosition();

	CRect rect;
	static BOOL flag=TRUE;
	if (pos!=NULL)
	{
		int index=m_DeviceListCtrl.GetNextSelectedItem(pos);
		EnterCriticalSection(&m_csProject);

		//���������ͼ��
		m_DeviceListCtrl.GetSubItemRect(index,DOWN_LOAD_ICON_INDEX,LVIR_BOUNDS,rect);
		if (flag)
		{

			m_DeviceListCtrl.SetItemImage(index,DOWN_LOAD_ICON_INDEX,IDB_PauseDownLoad);
			m_DeviceListCtrl.InvalidateRect(&rect);
			flag=FALSE;

			//����
			//���õ�ǰ���ص���

			SetCurDownLoadItem(index);

			//��ȡitem������ID
			//CString strFileName=m_DeviceListCtrl.GetItemText(index,1);
			CString strFileName=GetFileNameFromItem(index);
			DownLoadState eState;
			BOOL flag=m_StateMap.Lookup(strFileName,eState);
			//��Ҫ��ʼ��m_StateMap
			if (!flag)
			{

				m_StateMap.SetAt(strFileName,DWL_START);
			}
			else
			{
				m_StateMap.SetAt(strFileName,DWL_RESUME);
			}


			PostMessage(WM_DOWNLOAD_MSG_PROCESS);

		}
		else
		{
			flag=TRUE;
			SetCurDownLoadItem(index);
			m_DeviceListCtrl.SetItemImage(index,DOWN_LOAD_ICON_INDEX,IDB_SmallDownLoad);
			m_DeviceListCtrl.InvalidateRect(&rect);

			//��ͣ����
			//CString strFileName=m_DeviceListCtrl.GetItemText(index,1);
			CString strFileName=GetFileNameFromItem(index);
			DownLoadState eState;
			BOOL flag=m_StateMap.Lookup(strFileName,eState);
			//��Ҫ��ʼ��m_StateMap
			if (flag)
			{
				m_StateMap.SetAt(strFileName,DWL_PAUSE);
			}

			PostMessage(WM_DOWNLOAD_MSG_PROCESS);

		}

		LeaveCriticalSection(&m_csProject);



	}


	
	



}

void CShowPageDlg::OnPlayrecordfile()
{
	// �����ļ�
	OnBnClickedPlaybtn();

}

void CShowPageDlg::OnNMRClickDeviceList(NMHDR *pNMHDR, LRESULT *pResult)
{
	CMenu   menu;
	POINT pt={0};
	GetCursorPos(&pt);

	menu.LoadMenu(IDR_MENU_DEV_LIST);

	//���ò˵��ĳ�ʼ״̬
	//ĳ��˵���Ч�����
	


	CMenu* pMenu=menu.GetSubMenu(0);

	POSITION pos=m_DeviceListCtrl.GetFirstSelectedItemPosition();
	if (pos==NULL)
	{
		pMenu->EnableMenuItem(0, MF_BYPOSITION | MF_DISABLED | MF_GRAYED);
		pMenu->EnableMenuItem(1, MF_BYPOSITION | MF_DISABLED | MF_GRAYED);	
	}

	if(m_DeviceListCtrl.GetItemCount()<=1)
	{
        pMenu->EnableMenuItem(2, MF_BYPOSITION | MF_DISABLED | MF_GRAYED);
	}

	pMenu->TrackPopupMenu(TPM_LEFTALIGN|TPM_RIGHTBUTTON,pt.x,pt.y,this);

	*pResult = 0;
}

void CShowPageDlg::OnUpdateDownloadfile(CCmdUI *pCmdUI)
{
	
	POSITION pos=m_DeviceListCtrl.GetFirstSelectedItemPosition();
	if (pos!=NULL)
	{
		if (pCmdUI->m_nID==ID_DownLoadFile||pCmdUI->m_nID==ID_PlayRecordFile)
		{
			pCmdUI->Enable(TRUE);
		}

	}
	else
	{
		if (pCmdUI->m_nID==ID_DownLoadFile||pCmdUI->m_nID==ID_PlayRecordFile)
		{
			pCmdUI->Enable(FALSE);
		}

	}
}

CString CShowPageDlg::GetRecordTimeFromSecond(UINT nSecond)
{
    CTime tRTime(nSecond);

	CString strTime;

	strTime.Format("%04d-%02d-%02d %02d:%02d:%02d",tRTime.GetYear(),tRTime.GetMonth(),tRTime.GetDay()
		                                           ,tRTime.GetHour(),tRTime.GetMinute(),tRTime.GetSecond());






	return strTime;


}

CString CShowPageDlg::GetRecordTimeLength(UINT nStartSecond,UINT nEndSecond)
{

     //¼��ʱ����ϵͳ���ڽ�����ʱ��С����ʼʱ��
	int tRLength=nEndSecond-nStartSecond;

	int Hour=tRLength/3600;

	int Minitue=(tRLength-Hour*3600)/60;

	int Second=(tRLength-Hour*3600-Minitue*60);





	CString strTime;

	strTime.Format("%02d:%02d:%02d",Hour,Minitue,Second);


	return strTime;


}
void CShowPageDlg::OnBnClickedEditbtn()
{
	// �޸��豸��ť
	OnModifydev();

}

DWORD CShowPageDlg::ProcessDownLoadAllFileThread(LPVOID lpParameter)
{


	CShowPageDlg* pDlg=(CShowPageDlg*)lpParameter;


	//EnterCriticalSection(&pDlg->m_csProject);
    pDlg->DownloadAllfile();

	//LeaveCriticalSection(&pDlg->m_csProject);


	return 0;
}

void CShowPageDlg::DownloadAllfile()
{

	// ȫ������
	//�ж�ѡ��
	//�ж��Ƿ��Ѿ���������
	// �����ļ�
	//��Ҫģ����Ӧ�����ĳһ�е����ذ�ť
	m_bDownFlag=TRUE;
	UINT nCount=m_DeviceListCtrl.GetItemCount();

	CRect rect;
	
	UINT nRow=0xFFFFFFFF;


	for (m_nCurDownCount=0;m_nCurDownCount<nCount;)
	{
		//�жϸ����Ƿ��Ѿ��������
		
		if (!m_bDownFlag)
		{
			break;
		}

		if (nRow!=m_nCurDownCount&&m_nCurDownCount<nCount)
		{
			nRow=m_nCurDownCount;
		}
		else
		{
			if (m_nCurDownCount==nCount)
			{
				break;
			}
			continue;
		}

		//ʵ����������:ֻ����ѡ����
		
		if (!(m_DeviceListCtrl.GetItemState(nRow,LVIS_SELECTED)==LVIS_SELECTED))
		{

			m_nCurDownCount++;
			if (m_nCurDownCount==nCount)
			{
				break;
			}
			continue;
		}
	       

		UINT nPos=m_DeviceListCtrl.GetCellProgressValue(nRow,DOWN_LOAD_SUBITEM_INDEX);
		if (nPos<100)
		{
			//������
			//���������ͼ��
			//�ж��Ƿ����ӳɹ�
			   m_DeviceListCtrl.GetSubItemRect(nRow,DOWN_LOAD_ICON_INDEX,LVIR_BOUNDS,rect);
			

				m_DeviceListCtrl.SetItemImage(nRow,DOWN_LOAD_ICON_INDEX,IDB_PauseDownLoad);
				m_DeviceListCtrl.InvalidateRect(&rect);
				

				//����
				//���õ�ǰ���ص���
				SetCurDownLoadItem(nRow);

				//��ȡitem������ID
				//CString strFileName=m_DeviceListCtrl.GetItemText(index,1);
				CString strFileName=GetFileNameFromItem(nRow);
				DownLoadState eState;
				BOOL flag=m_StateMap.Lookup(strFileName,eState);
				//��Ҫ��ʼ��m_StateMap
			
			    m_StateMap.SetAt(strFileName,DWL_START);
				
			//��ʼ����
			CString filename=GetFileNameFromItem(nRow);
			DownLoadState eDWLState;

			UINT curItem=nRow;

			if(m_StateMap.Lookup(filename,eDWLState))
			{
				switch(eDWLState)
				{

				case DWL_START:
					{
						if (ConnectFtpServer(m_sFtpSeverInfo.serverIP,m_sFtpSeverInfo.username,m_sFtpSeverInfo.pwd,m_sFtpSeverInfo.port))
						{
							// CString strLocalFilename=m_sFtpSeverInfo.strLocalFileSavePath;
							CString strLocalFilename=m_sFtpSeverInfo.strLocalFileSavePath;
							
							m_sFtpSeverInfo.strRomoteFileName=filename;
							//��Ҫ����¼�������������ļ�ѡ��
							if(m_eRecordType==RECYCLE_RECORDER)
							{

								strLocalFilename+=RECORD_RECYCLE_FILE_DIR;
								strLocalFilename+=filename;
								//��ȡѭ��¼���ļ�
								CString sChannel=m_DeviceListCtrl.GetItemText(nRow,1);
								filename.Format("recycle_%s",sChannel.GetBuffer(0));
								//m_sFtpSeverInfo.strRomoteFileName=filename;
								//��ȡ�����ļ�����ʼλ�úͽ���λ��
								__int64 nStartPos=0;
								__int64 nEndPos=0;
								GetRecycleFilePosFromItemDB(nRow,nStartPos,nEndPos);
								//ѭ��¼������
								BOOL flag=DownLoadFileFromServer(filename,strLocalFilename,TRUE,nStartPos,nEndPos);
								if (!flag)
								{
									m_bDownFlag=FALSE;
									m_bDowningFlag=FALSE;

									m_StateMap.RemoveKey(filename);
									m_DeviceListCtrl.SetItemImage(curItem,DOWN_LOAD_ICON_INDEX,IDB_SmallDownLoad);
									AfxMessageBox("����¼���ļ�ʧ�ܣ�",MB_OK);
								}

							}
							else
							{
								strLocalFilename+=RECORD_EVENT_FILE_DIR;
								strLocalFilename+=filename;

								BOOL flag=DownLoadFileFromServer(filename,strLocalFilename);
								if (!flag)
								{
									m_bDownFlag=FALSE;
									m_StateMap.RemoveKey(filename);
									m_DeviceListCtrl.SetItemImage(curItem,DOWN_LOAD_ICON_INDEX,IDB_SmallDownLoad);
									AfxMessageBox("����¼���ļ�ʧ�ܣ�",MB_OK);
								}

							}



							

							
						}
						else
						{
							//����ʧ��
							m_bDownFlag=FALSE;
							m_StateMap.RemoveKey(filename);
							m_DeviceListCtrl.SetItemImage(curItem,DOWN_LOAD_ICON_INDEX,IDB_SmallDownLoad);
							AfxMessageBox("����¼���ļ�ʧ�ܣ�",MB_OK);

						}
					}
					break;
				case DWL_PAUSE:
					{

						//�ҵ�����ͣ����
						PauseDownLoad();


					}
					break;

				case DWL_RESUME:
					{
						//��������
						ResumeDownLoad();


					}
					break;
				case DWL_END:
					{



					}
					break;

				default:
					break;

				}


			}

		}
		else
		{
			m_nCurDownCount++;

		}	
		//���б������һ����ʾ
		m_DeviceListCtrl.SendMessage(WM_VSCROLL,SB_LINERIGHT,0);
	}

	m_eAppRunState=APP_BATCH_DOWN_END_STATE;
	
	::SendMessage(m_ShowInfodlg.m_hWnd,WM_CLOSE,NULL,NULL);
	
}

void CShowPageDlg::OnDownloadallfile()
{
	//�����߳�
	EnterCriticalSection(&m_csProject);
	if (CreateThread(NULL,0,ProcessDownLoadAllFileThread,this,0,NULL)==INVALID_HANDLE_VALUE)
	{
		m_gGlobalVF.WriteLog("�������������ļ������߳�ʧ��");
		return;
	}
	LeaveCriticalSection(&m_csProject);

}

void CShowPageDlg::OnBnClickedQuerybtn()
{
	// ��ѯ���ݿ�
	UpdateData(TRUE);
	CWaitCursor wait;//Open wait cursor
	m_DeviceListCtrl.DeleteAllItems();

	m_ptrFileList.RemoveAll();
	//��Ҫ�ȴ��������
	//  OnDownloadrecorddb();

	//���Ĭ��ɫ
	FillTreeCtrlDefaultColor();

	CString  strFileDir=SetFtpServerIP();


	//��ȡ��ѯ����
	if (m_RecorderType.GetCurSel()==0)
	{
		m_eRecordType=EVENT_RECORDER;
	}
	else if (m_RecorderType.GetCurSel()==1)
	{
        m_eRecordType=RECYCLE_RECORDER;
	}
	
	OnShowrecorddb();

	if (m_Sock.m_hSocket!=INVALID_SOCKET)
	{
		//m_Sock.Close();
		//m_Sock.m_hSocket=INVALID_SOCKET;
	}


	wait.Restore();

	
}

void CShowPageDlg::OnBnClickedBatchdownbtn()
{
	// ���������ļ�
	//�����߳�
	//EnterCriticalSection(&m_csProject);
	UpdateData(TRUE);
	if (m_DeviceListCtrl.GetSelectedCount()<=0)
	{
		AfxMessageBox("����ѡ��Ҫ���ص�¼���ļ�",MB_ICONINFORMATION);
		return;
	}


	//��ȡ¼������
    if(m_RecorderType.GetCurSel()==0)
	{
		m_eRecordType=EVENT_RECORDER;
	}
	else if(m_RecorderType.GetCurSel()==1)
    {
       m_eRecordType=RECYCLE_RECORDER; 
	}
	

	if (CreateThread(NULL,0,ProcessDownLoadAllFileThread,this,0,NULL)==INVALID_HANDLE_VALUE)
	{
		m_gGlobalVF.WriteLog("�������������ļ������߳�ʧ��");
		return;
	}
	//LeaveCriticalSection(&m_csProject);
	m_eAppRunState=APP_BATCH_DOWN_STATE;

	if (m_ShowInfodlg.DoModal()==IDOK)
	{
		m_bDownFlag=FALSE;
		m_bDowningFlag=FALSE;
		m_eAppRunState=APP_BATCH_DOWN_END_STATE;
	}

	
	


}

void CShowPageDlg::OnBnClickedLoginbtn()
{
	// ��¼��ť
	UpdateData(TRUE);

	//��ʼ���ȴ�
	BeginWaitCursor();
    SetCapture();
    

	//��ȡ¼������
	if (m_RecorderType.GetCurSel()==0)
	{
		m_eRecordType=EVENT_RECORDER;
	}
	else if (m_RecorderType.GetCurSel()==1)
	{
		m_eRecordType=RECYCLE_RECORDER;
	}

	//������Ҫ��TCP���ӷ�����
	if (m_eAppRunState==APP_BATCH_DOWN_STATE)
	{
		AfxMessageBox("�����������أ������ĵȴ�!");
		return;
	}

	BOOL flag=TRUE;
	if (m_Sock.m_hSocket==INVALID_SOCKET)
	{

		//flag=m_Sock.Create();
		

	}

	if (flag)
	{
		//�׽��ִ����ɹ�
		SRecordDevInfo*  pDevInfo=GetSelectTreeDevInfo();

		
		if(pDevInfo==NULL)
		{
			//��ֹΪ��ʱ�������

			EndWaitCursor();
			ReleaseCapture();

			return;

		}
		

		//DWORD IP ת��ΪCString 
		CString  strIP;  
		unsigned  char  *pIP  =  (unsigned  char*)&pDevInfo->m_dDevIP;  
		strIP.Format("%u.%u.%u.%u",*(pIP+3),  *(pIP+2),  *(pIP+1),  *pIP);

		m_gGlobalVF.m_ServerIP=strIP;
		m_gGlobalVF.m_nServerPort=pDevInfo->m_nDevPort;


		m_gGlobalVF.m_sDevInfo=(*pDevInfo); //����ôν������ӵ��豸��Ϣ

	  // m_Sock.Connect(m_gGlobalVF.m_ServerIP,m_gGlobalVF.m_nServerPort);
		
		



	}

	//�����߳�
	EnterCriticalSection(&m_csProject);
	m_FtpClient.SetLoginFlag(FALSE);
	//��ʼ���������ص��߳�
	m_bDowningFlag=FALSE;
	m_bDownFlag=FALSE;
	m_StateMap.RemoveAll();

	 m_bShow=TRUE;
	
	if (CreateThread(NULL,0,ConnectFTPServerThread,this,0,NULL)==INVALID_HANDLE_VALUE)
	{
		m_gGlobalVF.WriteLog("��������FTP�������߳�ʧ�ܣ�");
	}

	m_nWaitSecond=0;
	m_nWaitConnTimer=SetTimer(CONN_WAIT_TIMER,1000,NULL);
	LeaveCriticalSection(&m_csProject);
}




