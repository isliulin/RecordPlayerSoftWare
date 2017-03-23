// PlayerListShowDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "RecordPlayerSoftWare.h"
#include "PlayerListShowDlg.h"


// CPlayerListShowDlg 对话框

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


// CPlayerListShowDlg 消息处理程序

BOOL CPlayerListShowDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  在此添加额外的初始化
   InitGUICtrl();
	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void CPlayerListShowDlg::InitGUICtrl()
{
    //初始化控件

	//m_PlayerListCtrl.SetExtendedStyle(LVS_EX_GRIDLINES|LVS_EX_FULLROWSELECT|LVS_EX_CHECKBOXES);
	m_PlayerListCtrl.SetExtendedStyle(LVS_EX_GRIDLINES|LVS_EX_FULLROWSELECT);

	m_PlayerListCtrl.SetHeaderHeight(1.5);
	m_PlayerListCtrl.InsertColumn(0,"序号",LVCFMT_CENTER,50);
	m_PlayerListCtrl.InsertColumn(1,"文件名",LVCFMT_CENTER,300);
	m_PlayerListCtrl.InsertColumn(2,"通道",LVCFMT_CENTER,100);
	m_PlayerListCtrl.InsertColumn(3,"录音时间",LVCFMT_CENTER,100);
	m_PlayerListCtrl.InsertColumn(4,"录音时长",LVCFMT_CENTER,100);
	m_PlayerListCtrl.InsertColumn(5,"文件大小",LVCFMT_CENTER,100);
	m_PlayerListCtrl.SetHeaderBKColor(204,199,199,2);
	m_PlayerListCtrl.SetHeaderTextColor(RGB(16,71,9));
	m_PlayerListCtrl.SetRowHeigt(20);
	m_PlayerListCtrl.SetTextColor(RGB(0,76,136));



}

void CPlayerListShowDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);
	//处理子控件
	//尺寸变化
	if (nType==SIZE_RESTORED || nType==SIZE_MAXIMIZED)  
	{  

		float   ratio[2];  
		POINT   newDialogSize;  
		CRect   newRect;  

		//获取新的客户区的大小  
		GetClientRect(&newRect);  
		newDialogSize.x = newRect.right - newRect.left;  
		newDialogSize.y = newRect.bottom - newRect.top;  

		//得现在的对话框与以往对话框的大小比例  
		ratio[0]    = (float)newDialogSize.x / m_oSize.x;  
		ratio[1]    = (float)newDialogSize.y / m_oSize.y;  

		CRect Rect;  
		int woc;  

		//左右上角的数据  
		CPoint OldTLPoint, NewTLPint;  
		CPoint OldBRPoint, NewBRPint;  

		//列出所有控件  
		HWND  hwndChild = ::GetWindow(m_hWnd,GW_CHILD);  

		while(hwndChild)  
		{  
			//取得ID  
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
