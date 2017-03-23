// PlayerListShowDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "RecordPlayerSoftWare.h"
#include "PlayerListShowDlg.h"


// CPlayerListShowDlg �Ի���

IMPLEMENT_DYNAMIC(CPlayerListShowDlg, CDialog)

CPlayerListShowDlg::CPlayerListShowDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CPlayerListShowDlg::IDD, pParent)
{

}

CPlayerListShowDlg::~CPlayerListShowDlg()
{
}

void CPlayerListShowDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PlayerList, m_PlayerListCtrl);
}


BEGIN_MESSAGE_MAP(CPlayerListShowDlg, CDialog)
	ON_WM_SIZE()
END_MESSAGE_MAP()


// CPlayerListShowDlg ��Ϣ�������

BOOL CPlayerListShowDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  �ڴ���Ӷ���ĳ�ʼ��
   InitGUICtrl();
	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
}

void CPlayerListShowDlg::InitGUICtrl()
{
    //��ʼ���ؼ�

	//m_PlayerListCtrl.SetExtendedStyle(LVS_EX_GRIDLINES|LVS_EX_FULLROWSELECT|LVS_EX_CHECKBOXES);
	m_PlayerListCtrl.SetExtendedStyle(LVS_EX_GRIDLINES|LVS_EX_FULLROWSELECT);

	m_PlayerListCtrl.SetHeaderHeight(1.5);
	m_PlayerListCtrl.InsertColumn(0,"���",LVCFMT_CENTER,50);
	m_PlayerListCtrl.InsertColumn(1,"�ļ���",LVCFMT_CENTER,300);
	m_PlayerListCtrl.InsertColumn(2,"ͨ��",LVCFMT_CENTER,100);
	m_PlayerListCtrl.InsertColumn(3,"¼��ʱ��",LVCFMT_CENTER,100);
	m_PlayerListCtrl.InsertColumn(4,"¼��ʱ��",LVCFMT_CENTER,100);
	m_PlayerListCtrl.InsertColumn(5,"�ļ���С",LVCFMT_CENTER,100);
	m_PlayerListCtrl.SetHeaderBKColor(204,199,199,2);
	m_PlayerListCtrl.SetHeaderTextColor(RGB(16,71,9));
	m_PlayerListCtrl.SetRowHeigt(20);
	m_PlayerListCtrl.SetTextColor(RGB(0,76,136));



}

void CPlayerListShowDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);
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

			case IDC_PlayerList:
			default:
				{
					OldTLPoint = Rect.TopLeft();  
					NewTLPint.x  = long(OldTLPoint.x*ratio[0]+0.5);  
					NewTLPint.y  = long(OldTLPoint.y*ratio[1]+0.5);  

					OldBRPoint   = Rect.BottomRight();  
					NewBRPint.x  = long(OldBRPoint.x*ratio[0]+0.5);  
					NewBRPint.y  = long(OldBRPoint.y*ratio[1]+0.5);  
					Invalidate();

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
