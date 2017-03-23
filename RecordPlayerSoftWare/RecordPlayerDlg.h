
// RecordPlayerDlg.h : ͷ�ļ�
#pragma once
//
#include "afxwin.h"
#include "afxcmn.h"
#include "CDXGraph.h"
#include "SystemTray.h"
#include "BtnST.h"
#include "BitmapSlider.h"
#include "BitItem.h"
#include "BitSlider.h"
#include "ProSliderCtrl.h"
#include "StaticST.h"
#include "ShowPageDlg.h"
#include "ShowDownLoadDlg.h"
#include "PlayerListShowDlg.h"


const UINT	TRAY_MSG = RegisterWindowMessage( "RecordPlayerSoftWare Tray" );
// CRecordPlayerDlg �Ի���
class CRecordPlayerDlg : public CDialogEx
{
// ����
public:
	CRecordPlayerDlg(CWnd* pParent = NULL);	// ��׼���캯��

	~CRecordPlayerDlg();
// �Ի�������
	enum { IDD = IDD_RECORDPLAYERSOFTWARE_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()


public:
	//��ʼ��APPƤ��
	void InitDialogAppSkin();

	//ϵͳ������
	//ϵͳ������
	CSystemTray				m_systemTray;		// ϵͳ����ͼ������
	BOOL					m_bWindowShow;		// �����Ƿ�����ʾ
	afx_msg void OnMenuitemTrayOpen();
	afx_msg void OnMenuitemTrayClose();
	afx_msg void OnSize(UINT nType, int cx, int cy);



	//�ؼ��䶯��ε���
	CRect m_ClientRect; //����ܾ��δ�С
	CPoint m_oSize;  //��¼����ܵĿ�͸�
	void InitMainGUI(); //��ʼ�������

	//�ؼ���ť
	CButtonST m_PlayBtn; //������ͣ��ť
	CButtonST m_PlayBackBtn;//��һ�װ�ť
	CButtonST m_PlayNextBtn;// ��һ�װ�ť
	BOOL m_bPlayflag; //���ű�־
	CButtonST m_SoundCtrlBtn;// �������ư�ť
	BOOL m_VoCtrlFlag; //�������Ʊ�־
	CBitmapSlider m_MusicVolumeCtrl;// ��������
	CProSliderCtrl m_PlayProgressCtrl; //�����������
	
	HCURSOR	GetSysHandCursor();//��ȡϵͳ���
    void ShowPlayBtn(BOOL flag); //��ʾ���Ż�����ͣ��ť
	void ShowVolCtrlBtn(BOOL flag);//�������ư�ť

	afx_msg void OnBnClickedPlaybtn();
	afx_msg void OnBnClickedPlaynextbtn();
	afx_msg void OnBnClickedPlayback();
	afx_msg void OnBnClickedSoundbtn();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg LRESULT OnBitmapSliderMoved(WPARAM wParam, LPARAM lParam);

	//DirectShow���
	BOOL InitComObject();//com�����ʼ��
	BOOL DestroyComObject(); //����Com����

	void CreateGraph(void);        // ����Filter Graph
	void DestroyGraph(void);       // ����Filter Graph

	afx_msg LRESULT OnGraphNotify(WPARAM inWParam, LPARAM inLParam);

	HRESULT SaveGraphFile(IGraphBuilder*pGraph,WCHAR*wszPath);//���Խӿ�

	CDXGraph *   m_pFilterGraph;     // Filter Graph��װ
	CString      m_SourceFile;      // Դ�ļ�
	UINT         m_nSliderTimer;     // ��ʱ��ID
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	void OnBnClickedPausebtn(); //��ͣ����
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	//��ȡ��ʾʱ����Ϣ
	void ShowPlayTimeTips();
	// �����ܳ���ʾ�ı�
	CStaticST m_PlayTotalLengthTextCtrl;
	// ���Ž����ı�
	CStaticST m_PlayCurLengthTextCtrl;

	//������ʾ����
	CShowPageDlg* m_ShowPageDlg;
	CRect m_ShowPageRect; //��ʾ�Ի�������

	//�����б���ʾ����
	CShowDownLoadDlg* m_ShowDownLoadDlg;

	

	//�����б���ʾ����
	CPlayerListShowDlg* m_PlayerListShowDlg;
	

	void CreateShowSubDlg();  //������ʾ�ӶԻ���

	
	//������
	CButtonST m_SettingBtn;// ���ð�ť
	CButtonST m_LoginBtn;// ��½��ť
	CButtonST m_DownLoadBtn;// ���ذ�ť
	CButtonST m_QueryBtn;// ��ѯ��ť
	CButtonST m_MainPageBtn;// ��ҳ�水ť
	afx_msg void OnBnClickedSettingbtn();
	afx_msg void OnBnClickedDownloadbtn();
	afx_msg void OnBnClickedLoginbtn();
	afx_msg void OnBnClickedQuerybtn();
	afx_msg void OnBnClickedShowdevbtn();

	void SwitchBtnStatus(UINT nIDC,BOOL bactive);


	//��ʾ����汾
	afx_msg void OnBnClickedAppIcon();

    

	virtual BOOL PreTranslateMessage(MSG* pMsg);
};
