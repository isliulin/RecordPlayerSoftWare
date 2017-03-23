// RecordDevDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "RecordPlayerSoftWare.h"
#include "RecordDevDlg.h"
#include "GlobalVF.h"

// CRecordDevDlg �Ի���

IMPLEMENT_DYNAMIC(CRecordDevDlg, CDialog)

CRecordDevDlg::CRecordDevDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CRecordDevDlg::IDD, pParent)
	, m_sDevName(_T("¼���豸"))
	, m_nDevID(1)
	, m_dDevIP(0)
    , m_sDevIP(_T("192.168.1.101"))
	, m_nDevPort(5081)
{
    //��IPת��ΪDWORD
	
	m_dDevIP= ntohl(inet_addr(m_sDevIP));   
	m_bAddFlag=TRUE; //Ĭ��Ϊ���
	
}

CRecordDevDlg::~CRecordDevDlg()
{
}

void CRecordDevDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_DevName, m_sDevName);
	DDX_Text(pDX, IDC_DevID, m_nDevID);
	DDX_IPAddress(pDX, IDC_DevIP, m_dDevIP);
	DDX_Text(pDX, IDC_DevPort, m_nDevPort);
}


BEGIN_MESSAGE_MAP(CRecordDevDlg, CDialog)
	ON_BN_CLICKED(IDOK, &CRecordDevDlg::OnBnClickedOk)
	ON_WM_CTLCOLOR()
END_MESSAGE_MAP()


// CRecordDevDlg ��Ϣ�������

BOOL CRecordDevDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	//��ʼ��
	CEdit* pEdit=(CEdit*)GetDlgItem(IDC_DevID);
	if (m_bAddFlag)
	{
		
		pEdit->SetReadOnly(FALSE);

	}
	else
	{
		
		pEdit->SetReadOnly(TRUE);
	}

	m_brBrush.CreateSolidBrush(RGB(255,255,255));

	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
}

void CRecordDevDlg::OnBnClickedOk()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	UpdateData(TRUE);
    
	m_gGlobalVF.m_sDevInfo.m_sDevName=m_sDevName;
	m_gGlobalVF.m_sDevInfo.m_nDevID=m_nDevID;
	m_gGlobalVF.m_sDevInfo.m_dDevIP=m_dDevIP;
	m_gGlobalVF.m_sDevInfo.m_nDevPort=m_nDevPort;

	if(m_bAddFlag)
	{
		//����豸д����
		SRecordDevInfo* pDevInfo=new SRecordDevInfo;
		if (pDevInfo==NULL)
		{
			m_gGlobalVF.WriteLog("�����豸�������!");

			return;

		}

		pDevInfo->m_sDevName=m_sDevName;
		pDevInfo->m_nDevID=m_nDevID;
		pDevInfo->m_dDevIP=m_dDevIP;
		pDevInfo->m_nDevPort=m_nDevPort;



		if(!m_gGlobalVF.IsSameDevID(m_nDevID))
		{
			m_gGlobalVF.m_lDevInfoList.AddTail(pDevInfo);
		}
		else
		{
			AfxMessageBox("�豸ID�Ѵ��ڣ�",MB_OK,MB_ICONWARNING);
			return;
		}

	}
	else{
		//�޸�����
		//�ҵ���ID���豸��Ϣ
		SRecordDevInfo* pDevInfo=m_gGlobalVF.FindDevInfo(m_nDevID);

		if (pDevInfo!=NULL)
		{

			pDevInfo->m_sDevName=m_sDevName;
			pDevInfo->m_nDevID=m_nDevID;
			pDevInfo->m_dDevIP=m_dDevIP;
			pDevInfo->m_nDevPort=m_nDevPort;

		}
		
	}
	
	
	OnOK();
}

HBRUSH CRecordDevDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);

	switch(nCtlColor)
	{
		case CTLCOLOR_DLG:
		case CTLCOLOR_STATIC:
			{

				pDC->SetBkColor(RGB(255,255,255));       //���屳��ɫ ��ע��SetBkColor�������������ı���ɫ�����ǿؼ��ı���ɫ
				pDC->SetBkMode(TRANSPARENT); 
			}
		return m_brBrush;
	}
	
	return hbr;
}
