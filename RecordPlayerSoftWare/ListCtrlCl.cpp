// ListCtrlCl.cpp : 实现文件
//

#include "stdafx.h"
#include "ListCtrlCl.h"


/////////////////////////////////////////////////////////////////////////////
// CExArray
/**************************************
*	函数名称：Add(CELLVALUE& cell)
*	函数类型：void
*	参数说明：CELLVALUE& cell:CELLVALUE的结构体的引用
*	函数功能：添加CELLVALUE结构到CExArray类中，它的内存大小会动态的增加和减小
**************************************/
template <class T>
void CExArray<T>::Add(T& cell)
{
	T* pCell=GetAt(cell.dwRow,cell.dwCol);
	if(pCell!=NULL)
	{
		memcpy(pCell,&cell,sizeof(T));
		return;
	}
	//////////////////////////////
	if(m_dwCurCount>=m_dwSize)
	{
		T *pTmp1,*pTmp2;
		pTmp1=m_pVal;
		m_dwSize+=10;
		pTmp2=new T[m_dwSize];
		memcpy(pTmp2,m_pVal,sizeof(T)*m_dwCurCount);
		m_pVal=pTmp2;
		delete[] pTmp1;
	}
	////////////////////////////
	memcpy(&m_pVal[m_dwCurCount],&cell,sizeof(T));
	m_dwCurCount++;
}

template <class T>
void CExArray<T>::DelAll()
{
	m_dwCurCount=0;
	if(m_pVal!=NULL)
	{
		delete[] m_pVal;
		m_pVal=NULL;
	}
	m_pVal=new T[10];
	m_dwSize=10;
}

template<class T>
void CExArray<T>::DelAt(DWORD dwRow,DWORD dwCol)
{
	for(int i=0;(DWORD)i<m_dwCurCount;i++)
	{
		if(m_pVal[i].dwCol==dwCol && m_pVal[i].dwRow==dwRow)
		{
			for(int j=i;(DWORD)j<m_dwCurCount-1;j++)
			{
				m_pVal[j]=m_pVal[j+1];
			}
			m_dwCurCount--;
			break;
		}
	}
	//动态减小内存
	if(m_dwCurCount<=(m_dwSize-10))
	{
		T *pTmp1,*pTmp2;
		pTmp1=m_pVal;
		m_dwSize-=10;
		pTmp2=new T[m_dwSize];
		memcpy(pTmp2,m_pVal,sizeof(T)*m_dwCurCount);
		m_pVal=pTmp2;
		delete[] pTmp1;		
	}
}

template<class T>
BOOL CExArray<T>::GetAt(DWORD dwRow,DWORD dwCol,T& cell)
{
	for(int i=0;(DWORD)i<m_dwCurCount;i++)
	{
		if(m_pVal[i].dwCol==dwCol && m_pVal[i].dwRow==dwRow)
		{
			cell=m_pVal[i];
			return TRUE;
		}
	}
	return FALSE;
}

template<class T>
BOOL CExArray<T>::GetAt(DWORD dwIndex,T& cell)
{
	if(dwIndex>=0 && dwIndex<m_dwCurCount)
	{
		cell=m_pVal[dwIndex];
		return TRUE;
	}
	return FALSE;
}

template<class T>
T* CExArray<T>::GetAt(DWORD dwIndex)
{
	if(dwIndex>=0 && dwIndex<m_dwCurCount)
	{
		return (m_pVal+dwIndex);
	}
	return NULL;
}

template<class T>
T* CExArray<T>::GetAt(DWORD dwRow, DWORD dwCol)
{
	for(int i=0;(DWORD)i<m_dwCurCount;i++)
	{
		if(m_pVal[i].dwCol==dwCol && m_pVal[i].dwRow==dwRow)
		{
			return (m_pVal+i);
		}
	}
	return NULL;
}

template<class T>
int CExArray<T>::GetCount()
{
	return m_dwCurCount;
}


#define  COLOR_BK_FRAME    RGB(192,187,187)
#define  COLOR_FRAME       RGB(161,216,139) //进度条颜色
#define  HIGHT_COLOR       RGB(227,234,122)  //高亮颜色
#define  HIGH_TEXT_COLOR   RGB(235,80,80) //高亮文本颜色


#define SHOW_PROGRESS_PERCENT 0x00000001
#define SUPPORT_SORT	0x00000002

struct stColor
{
	int nRow;
	int nCol;
	COLORREF rgb;
};
// CListCtrlCl

IMPLEMENT_DYNAMIC(CListCtrlCl, CListCtrl)

CListCtrlCl::CListCtrlCl()
: m_nRowHeight(0)
, m_fontHeight(12)
, m_fontWith(0)
{
	m_color = RGB(0,0,0);

	m_nImageListType=LVSIL_NORMAL;
	m_dwListCtrlExStyle=0;

	m_pDC=NULL;

}

CListCtrlCl::~CListCtrlCl()
{
	if (m_pDC)
	{
		delete m_pDC;
	}

}


BEGIN_MESSAGE_MAP(CListCtrlCl, CListCtrl)
	ON_WM_MEASUREITEM()
	ON_WM_MEASUREITEM_REFLECT()
	ON_WM_LBUTTONDOWN()
	ON_NOTIFY_REFLECT(LVN_COLUMNCLICK, &CListCtrlCl::OnLvnColumnclick)
END_MESSAGE_MAP()



// CListCtrlCl 消息处理程序



void CListCtrlCl::PreSubclassWindow()
{
	// TODO: 在此添加专用代码和/或调用基类
	ModifyStyle(0,LVS_OWNERDRAWFIXED);
	CListCtrl::PreSubclassWindow();
	CHeaderCtrl *pHeader = GetHeaderCtrl();
	m_Header.SubclassWindow(pHeader->GetSafeHwnd());
}

void CListCtrlCl::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{

	// TODO:  添加您的代码以绘制指定项
	TCHAR lpBuffer[256];

	LV_ITEM lvi;

	lvi.mask = LVIF_TEXT | LVIF_PARAM|LVIF_IMAGE ;
	lvi.iItem = lpDrawItemStruct->itemID ; 
	lvi.iSubItem = 0;
	lvi.pszText = lpBuffer ;
	lvi.cchTextMax = sizeof(lpBuffer);
	VERIFY(GetItem(&lvi));

	LV_COLUMN lvc, lvcprev ;
	::ZeroMemory(&lvc, sizeof(lvc));
	::ZeroMemory(&lvcprev, sizeof(lvcprev));
	lvc.mask = LVCF_WIDTH | LVCF_FMT;
	lvcprev.mask = LVCF_WIDTH | LVCF_FMT;

	if (m_pDC==NULL)
	{
		m_pDC=new CDC;

		if (m_pDC==NULL)
		{
			AfxMessageBox("创建CDC资源失败!",MB_OK,MB_ICONERROR);
		}

	}
	m_pDC->Attach(lpDrawItemStruct->hDC);
	CDC* pDC;
	pDC =m_pDC;
	CRect rtClient;
	GetClientRect(&rtClient);
	for ( int nCol=0; GetColumn(nCol, &lvc); nCol++)
	{
		if ( nCol > 0 ) 
		{
			// Get Previous Column Width in order to move the next display item
			GetColumn(nCol-1, &lvcprev) ;
			lpDrawItemStruct->rcItem.left += lvcprev.cx ;
			lpDrawItemStruct->rcItem.right += lpDrawItemStruct->rcItem.left; 
		}

		CRect rcItem;   
		if (!GetSubItemRect(lpDrawItemStruct->itemID,nCol,LVIR_LABEL,rcItem))   
			continue;   

		::ZeroMemory(&lvi, sizeof(lvi));
		lvi.iItem = lpDrawItemStruct->itemID;
		lvi.mask = LVIF_TEXT | LVIF_PARAM|LVIF_IMAGE;
		lvi.iSubItem = nCol;
		lvi.pszText = lpBuffer;
		lvi.cchTextMax = sizeof(lpBuffer);
		VERIFY(GetItem(&lvi));
		CRect rcTemp;
		rcTemp = rcItem;

		if (nCol==0)
		{
			rcTemp.left -=2;
		}

		if ( lpDrawItemStruct->itemState & ODS_SELECTED )
		{
			//pDC->FillSolidRect(&rcTemp, GetSysColor(COLOR_HIGHLIGHT)) ;
			
			//如果列有图标
			if (nCol==0)
			{

              rcTemp.left=ICON_OFFSET;
			}

			pDC->FillSolidRect(&rcTemp, HIGHT_COLOR) ;
			//pDC->SetTextColor(GetSysColor(COLOR_HIGHLIGHTTEXT)) ;
			//修改高亮颜色
			pDC->SetTextColor(HIGH_TEXT_COLOR) ;
		}
		else
		{
			COLORREF color;
			color = GetBkColor();
			pDC->FillSolidRect(rcTemp,color);

			if (FindColColor(nCol,color))
			{
				pDC->FillSolidRect(rcTemp,color);
			}
			if (FindItemColor(nCol,lpDrawItemStruct->itemID,color))
			{
				pDC->FillSolidRect(rcTemp,color);
			}
			
			//pDC->SetTextColor(m_color);
		}

		pDC->SelectObject(GetStockObject(DEFAULT_GUI_FONT));

		UINT   uFormat    = DT_CENTER ;
		if (m_Header.m_Format[nCol]=='0')
		{
			uFormat = DT_LEFT;
		}
		else if (m_Header.m_Format[nCol]=='1')
		{
			uFormat = DT_CENTER;
		}
		else if (m_Header.m_Format[nCol]=='2')
		{
			uFormat = DT_RIGHT;
		}
		TEXTMETRIC metric;
		pDC->GetTextMetrics(&metric);
		int ofst;
		ofst = rcItem.Height() - metric.tmHeight;
		rcItem.OffsetRect(0,ofst/2);
		pDC->SetTextColor(m_color);
		COLORREF color;
		if (FindColTextColor(nCol,color))
		{
			pDC->SetTextColor(color);
		}
		if (FindItemTextColor(nCol,lpDrawItemStruct->itemID,color))
		{
			pDC->SetTextColor(color);
		}
		CFont nFont ,* nOldFont; 
		nFont.CreateFont(m_fontHeight,m_fontWith,0,0,0,FALSE,FALSE,0,0,0,0,0,0,_TEXT("宋体"));//创建字体 
		nOldFont = pDC->SelectObject(&nFont);
		DrawText(lpDrawItemStruct->hDC, lpBuffer, _tcslen(lpBuffer), &rcItem, uFormat) ;

		pDC->SelectStockObject(SYSTEM_FONT) ;
          
		DWORD dwStype = GetExtendedStyle();
		if((dwStype&LVS_EX_CHECKBOXES)==LVS_EX_CHECKBOXES)
		{
			BOOL	bSelected =lpDrawItemStruct->itemState & ODS_SELECTED;
			BOOL	bChecked  =lpDrawItemStruct->itemState & ODS_CHECKED;
			BOOL	bGrayed	  =lpDrawItemStruct->itemState & ODS_GRAYED;
			DrawCheckBox(pDC,lvi.iItem);
		}



		
		//绘制位图
		int  nRow=lpDrawItemStruct->itemID;
		CString strText = GetItemText(nRow, nCol);

		DrawBmpImage(pDC,strText,nRow,nCol);


		//绘制进度条或其他内嵌控件		
		CRect rcCell;
		if (GetCellRectEx(nRow, nCol, rcCell))
		{
			bool bSelected=GetItemState(nRow, LVIS_SELECTED) == LVIS_SELECTED;
			// get the device context.
			//CDC *pDC= CDC::FromHandle(lplvcd->nmcd.hdc);
			// draw the cell.
			DrawCell(pDC, strText, rcCell, bSelected, nRow, nCol); // *pResult will be changed here
		}	
	
	}

	m_pDC->Detach();
}

BOOL CListCtrlCl::DrawBmpImage(CDC *pDC,  CString &strText, int nRow,int nCol)
{
	CELLVALUE* pCell=m_val.GetAt(nRow,nCol);
	CBitmap bmp,*pOldBmp;
	CDC memDC;
	memDC.CreateCompatibleDC(pDC);
	if (strText==_T(""))
	{
		if(pCell)
		{
			CRect rect;
			GetCellRect(pCell->dwRow,pCell->dwCol,rect);
			if(pCell->nIDResource>0 && bmp.LoadBitmap(pCell->nIDResource))
			{
				BITMAP bm;
				bmp.GetBitmap(&bm);
				pOldBmp=memDC.SelectObject(&bmp);
				int offH=rect.Height()-bm.bmHeight;
				int offW=rect.Width()-bm.bmWidth;
				if(offH>0)
				{
					rect.top+=offH/2;
					rect.bottom-=(offH-offH/2);
				}
				if(offW>0)
				{
					rect.left+=offW/2;
					rect.right-=(offW-offW/2);
				}
				//显示
				pDC->StretchBlt(rect.left,rect.top,rect.Width(),rect.Height(),&memDC,0,0,\
					bm.bmWidth,bm.bmHeight,SRCCOPY);

				//pDC->DrawText(strText, &rect, DT_CENTER);

				//Clean
				memDC.SelectObject(pOldBmp);
				bmp.DeleteObject();


			}
		}
	}
	else
	{
		//有文字时，将图标绘制在最左边
		if(pCell)
		{
			CRect rect;
			GetCellRect(pCell->dwRow,pCell->dwCol,rect);
			if(pCell->nIDResource>0 && bmp.LoadBitmap(pCell->nIDResource))
			{

				BITMAP bm;
				bmp.GetBitmap(&bm);
				pOldBmp=memDC.SelectObject(&bmp);
				
				/*
				int offH=rect.Height()-bm.bmHeight;
				int offW=rect.Width()-bm.bmWidth;
				if(offH>0)
				{
					//rect.top+=offH/2;
					//rect.bottom-=(offH-offH/2);
				}
				if(offW>0)
				{
					//rect.left-=offW/2;
					//rect.right-=(offW-offW/2);
				}
				*/
				//pDC->FillSolidRect(&rect, HIGHT_COLOR);
				pDC->StretchBlt(rect.left+ICON_OFFSET,rect.top,bm.bmWidth,bm.bmHeight,&memDC,0,0,\
					bm.bmWidth,bm.bmHeight,SRCCOPY);

				//pDC->DrawText(strText, &rect, DT_RIGHT);

				//Clean
				memDC.SelectObject(pOldBmp);
				bmp.DeleteObject();

		}

	}
}
	
	//Clean
	memDC.DeleteDC();
	////////////////////////
	//COLORREF crOldText=pDC->SetTextColor(strText);

	
	

	return TRUE;



}


int CListCtrlCl::DrawCheckBox(CDC *pDC,int nItem)
{


	CRect rt;                                         //得到rt的范围，在此范围内创建CheckBox
	GetItemRect(nItem,&rt,LVIR_LABEL);
	rt.top+=OFFSET_TOP;
	rt.bottom-=OFFSET_BOTTOM;
	rt.left-=OFFSET_LEFT;
	rt.right = rt.left + OFFSET_RIGHT;

	CBrush  bsh(CHECK_BOX_COLOR);                   //checkBox边框颜色
	pDC->FillSolidRect(rt, CHECK_BOX_BK_COLOR);
	//CBrush  bsh(RGB(0, 255, 255));                   //checkBox边框颜色
	//pDC->FillSolidRect(rt, RGB(255, 255, 255));
	pDC->FrameRect(&rt,&bsh);
	if(m_itemVector[nItem].flag == 0) 
	{
	}else if(m_itemVector[nItem].flag == 1)
	{
		CBitmap  tickBmp;
		BITMAP   bmp;
		tickBmp.LoadBitmap(IDB_TICK);
		tickBmp.GetBitmap(&bmp);
		CDC      memDC;
		memDC.CreateCompatibleDC(pDC);
		CBitmap *pOldBmp = memDC.SelectObject(&tickBmp);
		pDC->BitBlt(rt.left + 2, rt.top + 2, bmp.bmWidth, bmp.bmHeight, &memDC, 0, 0, SRCCOPY);
		memDC.SelectObject(pOldBmp);
		memDC.DeleteDC();
	}else
	{
		pDC->DrawFrameControl(&rt,DFC_BUTTON,DFCS_INACTIVE);
	}
	return TRUE;

}

int CListCtrlCl::InsertItem(int nItem,LPCTSTR lpszItem)
{
	//插入
	CListCtrl::InsertItem(nItem,lpszItem);

	LVCOLUMN   lvColumn;   
	TCHAR strChar[256];
	lvColumn.pszText=strChar;   
	lvColumn.cchTextMax=256 ;
	lvColumn.mask   = LVCF_TEXT;
	GetColumn(0, &lvColumn);
	
	m_itemCheck.item=nItem;
	m_itemCheck.flag=0;
	m_itemVector.push_back(m_itemCheck);
	return 1;
}

int CListCtrlCl::InsertItem(int nItem,LPCTSTR lpszItem,int index)
{
  
    return CListCtrl::InsertItem(nItem,lpszItem,index);

}

BOOL CListCtrlCl::DeleteAllItems()
{
	m_itemVector.clear();
	OnDestroy();
	return CListCtrl::DeleteAllItems();

}

BOOL CListCtrlCl::DeleteSelectedItem()
{

	int CurrentCount=(int)m_itemVector.size();
    if(CurrentCount<=0)
	{
		return FALSE;
	}


	int iSelected=0;
	vector<ITEM_CHECK>::iterator iter=m_itemVector.begin();
	
     //如何判断选中项
	for(int i=0;i<CurrentCount;i++)
	{
		if(m_itemVector.at(i).flag==1)
		{
			iSelected=1;
			break;                                            //相邻选中删除，需要后退以一次 ，不然会错过下个选中的。
		}
	}
	 
    if (iSelected==0)
    {
		return FALSE;
    }
	int index=0;
	for(vector<ITEM_CHECK>::iterator it=iter;it!=m_itemVector.end();)
	{
		
		if(it->flag==1)
		{
			DeleteItem(index);
			it=m_itemVector.erase(it);  //擦除返回下一个位置
			this->Invalidate();	
		}
		else
		{
			index++;
			it++;
			
		}
	
	}


	return TRUE;

}

int CListCtrlCl::SelectOneCheckBox(int ItemID, CPoint pt)
{
	if(m_itemVector.empty())
	{
		return -1;
	}

	//当点击空白区域时；如果不增加该选项则容易出现错误
	if (ItemID==-1)
	{
       return -1;
	}

	if(-1==m_itemVector.at(ItemID).flag)
	{
		return -1;
	}

	CRect rt(0,0,0,0);
	GetItemRect(ItemID,&rt,LVIR_LABEL);
	rt.top+=OFFSET_TOP;
	rt.bottom-=OFFSET_BOTTOM;
	rt.left-=OFFSET_LEFT;
	rt.right = rt.left + OFFSET_RIGHT;

	if(rt.PtInRect(pt))
	{
		m_itemVector[ItemID].flag = m_itemVector[ItemID].flag==0?1:0;
		InvalidateRect(rt);
	}
	return TRUE;
}

int CListCtrlCl:: SetItemCheckBox(int itemID,int state)
{

	m_itemVector.at(itemID).flag=state;
	return 1;
}

void CListCtrlCl::OnMeasureItem(int nIDCtl, LPMEASUREITEMSTRUCT lpMeasureItemStruct)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	CListCtrl::OnMeasureItem(nIDCtl, lpMeasureItemStruct);
}
void CListCtrlCl::MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct)
{
	if (m_nRowHeight>0)
	{
		lpMeasureItemStruct->itemHeight = m_nRowHeight;
	}
}
int CListCtrlCl::InsertColumn(int nCol, LPCTSTR lpszColumnHeading, int nFormat /* = LVCFMT_LEFT */, int nWidth /* = -1 */, int nSubItem /* = -1 */)
{
	m_Header.m_HChar.Add(lpszColumnHeading);
	if (nFormat==LVCFMT_LEFT)
	{
		m_Header.m_Format = m_Header.m_Format + _T("0");
	}
	else if (nFormat==LVCFMT_CENTER)
	{
		m_Header.m_Format = m_Header.m_Format + _T("1");
	}
	else if (nFormat==LVCFMT_RIGHT)
	{
		m_Header.m_Format = m_Header.m_Format + _T("2");
	}
	else
	{
		m_Header.m_Format = m_Header.m_Format + _T("1");
	}
	return CListCtrl::InsertColumn(nCol,lpszColumnHeading,nFormat,nWidth,nSubItem);
}
// Gradient - 渐变系数，立体背景用,不用渐变设为0
void CListCtrlCl::SetHeaderBKColor(int R, int G, int B, int Gradient) //设置表头背景色
{
	m_Header.m_R = R;
	m_Header.m_G = G;
	m_Header.m_B = B;
	m_Header.m_Gradient = Gradient;
}

// 设置表头高度
void CListCtrlCl::SetHeaderHeight(float Height) //设置表头高度
{
	m_Header.m_Height = Height;
}
bool CListCtrlCl::FindColColor(int col,COLORREF &color) //查找列颜色
{
	int flag = 0;
	for (POSITION pos = m_ptrListCol.GetHeadPosition();pos!=NULL;)
	{
		stColor *pColor = (stColor*)m_ptrListCol.GetNext(pos);
		if (pColor->nCol==col)
		{
			flag = 1;
			color = pColor->rgb;
			break;
		}
	}
	if (1==flag)
	{
		return true;
	}
	return false;
}
bool CListCtrlCl::FindItemColor(int col,int row,COLORREF &color) //查找颜色
{
	int flag = 0;
	for (POSITION pos = m_ptrListItem.GetHeadPosition();pos!=NULL;)
	{
		stColor *pColor = (stColor*)m_ptrListItem.GetNext(pos);
		if (pColor->nCol==col&&pColor->nRow==row)
		{
			flag = 1;
			color = pColor->rgb;
			break;
		}
	}
	if (1==flag)
	{
		return true;
	}
	return false;
}
void CListCtrlCl::SetColColor(int col,COLORREF color) //设置列颜色
{
	stColor *pColor  = new stColor;
	pColor->nCol = col;
	pColor->rgb = color;
	m_ptrListCol.AddTail(pColor);
}
void CListCtrlCl::SetItemColor(int col,int row,COLORREF color) //设置格子颜色
{
	stColor *pColor  = new stColor;
	pColor->nCol = col;
	pColor->nRow = row;
	pColor->rgb = color;
	m_ptrListItem.AddTail(pColor);
}
void CListCtrlCl::SetRowHeigt(int nHeight) //高置行高
{
	m_nRowHeight = nHeight;

	CRect rcWin;
	GetWindowRect(&rcWin);
	WINDOWPOS wp;
	wp.hwnd = m_hWnd;
	wp.cx = rcWin.Width();
	wp.cy = rcWin.Height();
	wp.flags = SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOOWNERZORDER | SWP_NOZORDER;
	SendMessage(WM_WINDOWPOSCHANGED, 0, (LPARAM)&wp);
}
void CListCtrlCl::SetHeaderFontHW(int nHeight,int nWith) //设置头部字体宽和高
{
	m_Header.m_fontHeight = nHeight;
	m_Header.m_fontWith = nWith;
}
void CListCtrlCl::SetHeaderTextColor(COLORREF color) //设置头部字体颜色
{
	m_Header.m_color = color;
}
BOOL CListCtrlCl::SetTextColor(COLORREF cr)  //设置字体颜色
{
	m_color = cr;
	return TRUE;
}
void CListCtrlCl::SetFontHW(int nHeight,int nWith) //设置字体高和宽
{
	m_fontHeight = nHeight;
	m_fontWith = nWith;
}
void CListCtrlCl::SetColTextColor(int col,COLORREF color)
{
	stColor *pColor = new stColor;
	pColor->nCol = col;
	pColor->rgb = color;
	m_colTextColor.AddTail(pColor);
}
bool CListCtrlCl::FindColTextColor(int col,COLORREF &color)
{
	int flag = 0;
	for (POSITION pos = m_colTextColor.GetHeadPosition();pos!=NULL;)
	{
		stColor *pColor = (stColor*)m_colTextColor.GetNext(pos);
		if (pColor->nCol==col)
		{
			flag = 1;
			color = pColor->rgb;
			break;
		}
	}
	if (1==flag)
	{
		return true;
	}
	return false;
}
bool CListCtrlCl::FindItemTextColor(int col,int row,COLORREF &color)
{
	int flag = 0;
	for (POSITION pos = m_ItemTextColor.GetHeadPosition();pos!=NULL;)
	{
		stColor *pColor = (stColor*)m_ItemTextColor.GetNext(pos);
		if (pColor->nCol==col&&pColor->nRow==row)
		{
			flag = 1;
			color = pColor->rgb;
			break;
		}
	}
	if (1==flag)
	{
		return true;
	}
	return false;
}
void CListCtrlCl::SetItemTextColor(int col,int row,COLORREF color)
{
	stColor *pColor = new stColor;
	pColor->nCol = col;
	pColor->nRow = row;
	pColor->rgb = color;
	m_ItemTextColor.AddTail(pColor);
}
void CListCtrlCl::OnLButtonDown(UINT nFlags, CPoint point)
{
	DWORD dwStype = GetExtendedStyle();
	CRect  itemRect;
	int iItem = HitTest(point);
	CRect  rt;
	GetItemRect(iItem,&rt,LVIR_LABEL);
	rt.top+=OFFSET_TOP;
	rt.bottom-=OFFSET_BOTTOM;
	rt.left-=OFFSET_LEFT;
	rt.right = rt.left + OFFSET_RIGHT;
	if((dwStype&LVS_EX_CHECKBOXES)==LVS_EX_CHECKBOXES)
	{
		//防止当没有选中LVS_EX_CHECKBOXES风格时，点击鼠标左键运行错误。
         SelectOneCheckBox(iItem, point);
	}
	

	CListCtrl::OnLButtonDown(nFlags, point);
}

// 获取列表的某一行某一列的矩形区域
void CListCtrlCl::GetCellRect(int nItem,int nSubItem,CRect& rect)
{
	this->GetItemRect(nItem,&rect,LVIR_BOUNDS);

	LVCOLUMN column;
	column.mask=LVCF_WIDTH;
	for(int i=0;i<nSubItem;i++)
	{
		this->GetColumn(i,&column);
		rect.left+=column.cx;
	}

	this->GetColumn(nSubItem,&column);
	rect.right=rect.left+column.cx;
}

 BOOL CListCtrlCl::IsItemImage(int row,int col)
 {
	 CELLVALUE* pCell=m_val.GetAt(row,col);
	 if(pCell)
	 {
		 if(pCell->nIDResource>0)
			 return TRUE;
	 }
	 return FALSE;
 }

 //设置单元中图片
 void CListCtrlCl::SetItemImage(int row,int col,UINT nIDResource)
 {
	 CELLVALUE* pCell;
	 CELLVALUE cell;
	 memset(&cell,0,sizeof(CELLVALUE));
	 pCell=m_val.GetAt(row,col);
	 if(pCell)
	 {
		 pCell->nIDResource=nIDResource;
	 }
	 else
	 {
		 cell.dwRow=row;
		 cell.dwCol=col;
		 cell.nIDResource=nIDResource;
		 m_val.Add(cell);
	 }
	 ////////////////////////////
	 //刷新
	 CRect rect;
	 GetCellRect(row,col,rect);
	 this->InvalidateRect(&rect,FALSE);
 }

 BOOL CListCtrlCl::DrawBmpImage(CDC* pDC,int nRow,int nCol)
 {
	 CELLVALUE* pCell=m_val.GetAt(nRow,nCol);
	 CBitmap bmp,*pOldBmp;
	 CDC memDC;
	 memDC.CreateCompatibleDC(pDC);
	 if(pCell)
	 {
		 CRect rect;
		 GetCellRect(pCell->dwRow,pCell->dwCol,rect);
		 if(pCell->nIDResource>0 && bmp.LoadBitmap(pCell->nIDResource))
			{
				BITMAP bm;
				bmp.GetBitmap(&bm);
				pOldBmp=memDC.SelectObject(&bmp);
				int offH=rect.Height()-bm.bmHeight;
				int offW=rect.Width()-bm.bmWidth;
				if(offH>0)
				{
					rect.top+=offH/2;
					rect.bottom-=(offH-offH/2);
				}
				if(offW>0)
				{
					rect.left+=offW/2;
					rect.right-=(offW-offW/2);
				}
				//显示
				pDC->StretchBlt(rect.left,rect.top,rect.Width(),rect.Height(),&memDC,0,0,\
					bm.bmWidth,bm.bmHeight,SRCCOPY);
				//Clean
				memDC.SelectObject(pOldBmp);
				bmp.DeleteObject();
			}
	 }
	 //Clean
	 memDC.DeleteDC();
	 ////////////////////////

	 return TRUE;

 }

 void CListCtrlCl::DrawProgress(CDC *pDC,  CString &strText, CRect &rcCell, BOOL bSelected, const CellData &cellData)
 {
     //绘制进度条
	 COLORREF crText=bSelected?GetSysColor(COLOR_HIGHLIGHTTEXT):GetSysColor(COLOR_WINDOWTEXT);
	 COLORREF crTextBkgrnd=bSelected?GetSysColor(COLOR_HIGHLIGHT):GetSysColor(COLOR_WINDOW);
	// COLORREF crProgBack=bSelected?GetSysColor(COLOR_HOTLIGHT):GetSysColor(COLOR_BTNFACE);
	  COLORREF crProgBack=bSelected?COLOR_BK_FRAME:COLOR_BK_FRAME;
	 COLORREF crProgFill=bSelected?COLOR_FRAME:COLOR_FRAME;

	 CRect rcProg(rcCell), rcIcon(rcCell);
	 // fill background
	 pDC->FillSolidRect(&rcCell, crTextBkgrnd);
	 //-- draw icon--//
	 CImageList *pImageList=TheImageList();
	 if (cellData.m_nImage>=0 && pImageList)
	 {
		 rcIcon.right=rcIcon.left+rcIcon.Height();
		 pImageList->DrawIndirect(	pDC, cellData.m_nImage, rcIcon.TopLeft(), rcIcon.Size(), CPoint(0, 0), ILD_NORMAL, 
			 SRCCOPY,crTextBkgrnd, crTextBkgrnd );
	 }
	 else
	 {
		 rcIcon.right=rcIcon.left;
	 }
	// rcProg.left=rcIcon.right+3;
	 rcProg.left=rcIcon.right;


	 pDC->FillSolidRect(&rcProg, crProgBack);
	 pDC->DrawEdge(&rcProg, EDGE_SUNKEN, BF_RECT);
	 CRect rcFill(rcProg);
	 rcFill.DeflateRect(1,1,1,1);
	 float fPersent=0.0f;
	 int nFill=GetProgBarSize(cellData, rcProg, &fPersent);
	 rcFill.right=min(rcFill.left+nFill, rcFill.right);
	 pDC->FillSolidRect(&rcFill, crProgFill);
	 COLORREF crOldText=pDC->SetTextColor(crText);

	 if (IsShowPogressPercent())
	 {
		 strText.Format(_T("%.2f%%"), fPersent*100);
	 }
	 rcProg.top+=5;//用于居中显示
	 pDC->DrawText(strText, &rcProg, DT_CENTER);
	 pDC->SetTextColor(crOldText);
#ifdef USING_CUSTOM_DRAW	
#else
#endif	



 }

 int CListCtrlCl::GetProgBarSize(const CellData &cellData, CRect &rcProg, float *pPersent)
 {
	 float fScale=0.0f;
	 if (cellData.m_uProgresVal>=cellData.m_uProgressMax)
		 fScale=1.0f;
	 else if(cellData.m_uProgresVal<=0 || cellData.m_uProgressMax<=0)
		 fScale=0.0f;
	 else
		 fScale=(float)cellData.m_uProgresVal/(float)cellData.m_uProgressMax;

	 if (pPersent)
	 {
		 *pPersent=fScale;
	 }
	 return (int)(rcProg.Width()*fScale);


 }

 CImageList*	CListCtrlCl::TheImageList()
 {
    return GetImageList(m_nImageListType);


 }

 BOOL CListCtrlCl::IsShowPogressPercent()
 {
      
    return m_dwListCtrlExStyle & SHOW_PROGRESS_PERCENT;  

 }

 void CListCtrlCl::SetShowProgressPercent(BOOL bShowProgPercent)
 {
	 if (bShowProgPercent)
		 m_dwListCtrlExStyle|=SHOW_PROGRESS_PERCENT;
	 else
		 m_dwListCtrlExStyle&=(~SHOW_PROGRESS_PERCENT);
 }

 void CListCtrlCl::SetCellProgressMaxValue(int nRow, int nCol, UINT uMaxValue)
 {
	 ASSERT(0<=nRow && nRow<GetItemCount() && nCol>=0 && nCol< GetColumnCount());
	 FindCellData(nRow, nCol).m_uProgressMax=uMaxValue;
	 CRect rcCell;
	 if(GetCellRectEx(nRow, nCol, rcCell))
	 {
		 InvalidateRect(&rcCell); // update progress bar here immediately
		 UpdateWindow();	
	 }
 }

 void CListCtrlCl::SetCellProgressValue(int nRow, int nCol, UINT uValue)
 {
	 ASSERT(0<=nRow && nRow<GetItemCount() && nCol>=0 && nCol< GetColumnCount());
	 FindCellData(nRow, nCol).m_uProgresVal=uValue;
	 CRect rcCell;
	 if(GetCellRectEx(nRow, nCol, rcCell))
	 {
		 InvalidateRect(&rcCell); // update progress bar here immediately
		 UpdateWindow();	
	 }
 }

 UINT   CListCtrlCl::GetCellProgressValue(int nRow, int nCol)
 {

	 ASSERT(0<=nRow && nRow<GetItemCount() && nCol>=0 && nCol< GetColumnCount());
	
	 
	 return FindCellData(nRow, nCol).m_uProgresVal;
	



 }


 ColumnType CListCtrlCl::GetColumnType(int nColIndex)
 {
	 // insert new if not found
	 ColMap::iterator iter=m_mapCol2ColType.find(nColIndex);
	 if (iter==m_mapCol2ColType.end())
	 {
		 m_mapCol2ColType[nColIndex]=NormalText;
	 }
	 return m_mapCol2ColType[nColIndex];
 }

 void CListCtrlCl::SetColumnType(int nColIndex, ColumnType eColType)
 {
	 ASSERT(0<=nColIndex && nColIndex<GetColumnCount());
	 // init if empty
	 if (m_mapCol2ColType.empty())
	 {
		 for (int i=0; i<GetColumnCount(); ++i)
		 {
			 m_mapCol2ColType.insert(ColMap::value_type(i, NormalText));
		 }
	 }
	 // assign
	 m_mapCol2ColType[nColIndex]=eColType;
 }


 BOOL	CListCtrlCl::GetCellRectEx(int iRow, int iCol, CRect &rect, int nArea)
 {
	 ASSERT(0<=iRow && iRow<GetItemCount() && iCol>=0 && iCol< GetColumnCount());
	 if(iCol)
		 return GetSubItemRect(iRow, iCol, nArea, rect);

	 if(GetColumnCount()== 1)
		 return GetItemRect(iRow, rect, nArea);

	 iCol = 1;
	 CRect rCol1;
	 if(!GetSubItemRect(iRow, iCol, nArea, rCol1))
		 return FALSE;

	 if(!GetItemRect(iRow, rect, nArea))
		 return FALSE;

	 rect.right = rCol1.left;

	 return TRUE;
 }

 CellData& CListCtrlCl::FindCellData(int nRow, int nCol)
 {
	 // init if empty
	 if (m_mapCell2Data.empty())
	 {
		 for (int x=0; x<GetItemCount(); ++x)
		 {
			 for (int y=0; y< GetColumnCount(); ++y)
			 {
				 m_mapCell2Data[make_pair(x,y)]=CellData(TRUE, FALSE, y);
			 }
		 }
	 }

	 CellDataMap::iterator iter=m_mapCell2Data.find(make_pair(nRow, nCol));
	 if (iter==m_mapCell2Data.end())
	 {
		 m_mapCell2Data[make_pair(nRow, nCol)]=CellData(TRUE, FALSE, nCol);
	 }
	 return m_mapCell2Data[make_pair(nRow, nCol)];
 }

 void CListCtrlCl::DrawCell(CDC *pDC,  CString &strText, CRect &rcCell, BOOL bSelected, 
	 int nRow, int nCol)
 {
	 switch(GetColumnType(nCol))
	 {
	 case Progress:
		
		 DrawProgress(pDC, strText, rcCell, bSelected, FindCellData(nRow,nCol));
		 return;
	default:
		
		 break;
	 }
 }

 int CListCtrlCl::InsertColumn(int nCol, CString strColHead, int nWidth, ColumnType eColType, int nFormat, 
	 SortType eSortBy, int nSubItem )
 {
	 //
	 m_mapCol2ColType[nCol]=eColType;
	 m_mapCol2Sort[nCol].m_eSortType=eSortBy;
	 return CListCtrl::InsertColumn(nCol, strColHead, nFormat, nWidth, nSubItem);
 }


 int CALLBACK CListCtrlCl::CompareFunc(LPARAM lParam1, LPARAM lParam2, 
	 LPARAM lParamSort)
 {
	 _SortParam_t *pSortParam_t=reinterpret_cast<_SortParam_t*>(lParamSort);
	 if (NULL==pSortParam_t)
		 return 0;
	 CListCtrlCl *pList=pSortParam_t->m_pTheList;
	 ASSERT(pList!=NULL && ::IsWindow(pList->m_hWnd));
	 CString strCell1 = pList->GetItemText((int)lParam1, pSortParam_t->m_nSortCol);   
	 CString strCell2 = pList->GetItemText((int)lParam2, pSortParam_t->m_nSortCol);   
	 int nResult=0;
	 try
	 {
		 switch(pSortParam_t->m_ColParam.m_eSortType)
		 {
		 default:
		 case SortByString:
			 nResult=StrCompare(strCell1,strCell2);
			 break;
		 case SortByDigit:
			 nResult=IntCompare(strCell1,strCell2);
			 break;
		 }
	 }
	 catch (...)
	 {
		 nResult=StrCompare(strCell1,strCell2);
	 }

	 return (pSortParam_t->m_ColParam.m_bIsAsc?nResult:-nResult);
 }

 bool CListCtrlCl::IsDigitStr(LPCTSTR str)
 {
	 int nLen=(int)_tcslen(str);
	 int nPointCnt=0;
	 TCHAR ch=0;
	 for (int i=0; i<nLen; ++i)
	 {
		 ch=str[i];
		 // for negative
		 if (0==i && _T('-')==ch) {
			 continue;
		 }else if ( 0!=i && _T('-')==ch) {
			 return false;
		 }

		 if (_T('.')==ch) {
			 ++nPointCnt;
		 }

		 if (nPointCnt>1) {
			 return false;
		 }

		 if ((ch<48 || ch>57) && _T('.')!=ch) {
			 return false;
		 }
	 }
	 return true;
 }


#ifdef _DEBUG
#define ASSERT_VALID_STRING( str ) ASSERT( !IsBadStringPtr( str, 0xfffff ) )
#else	//	_DEBUG
#define ASSERT_VALID_STRING( str ) ( (void)0 )
#endif	//	_DEBUG

 int CListCtrlCl::IntCompare(LPCTSTR strInt1st,LPCTSTR strInt2nd)
 {
	 ASSERT_VALID_STRING( strInt1st );
	 ASSERT_VALID_STRING( strInt2nd);
	 if (!IsDigitStr(strInt1st) || !IsDigitStr(strInt2nd))
	 {
		 //AfxThrowInvalidArgException();
		 return StrCompare(strInt1st, strInt2nd);
	 }

	 const int num1st=_ttoi(strInt1st);
	 const int num2nd=_ttoi(strInt2nd);
	 return (num1st-num2nd);

 }



 int CListCtrlCl::StrCompare(LPCTSTR str1st, LPCTSTR str2nd)
 {
	 ASSERT_VALID_STRING( str1st );
	 ASSERT_VALID_STRING( str2nd);
	 return _tcscmp(str1st, str2nd);
 }

 // before sort items, backup old item data here, then set the item data with old row NO.
 // vec2StoreData must be initialized as size of "GetItemCount()"
 void CListCtrlCl::PreSortItems( vector<DWORD_PTR> &vec2StoreData )
 {
	 int nRowCnt=GetItemCount();
	 for (int i=0; i<nRowCnt; ++i)
	 {
		 vec2StoreData[i]=GetItemData(i);
		 SetItemData(i, (DWORD_PTR)i);
	 }
 }

 // after sort items, re-mapping the cell data here and restore the old item data here
 // vec2StoreData must be initialized as size of "GetItemCount()" and assigned with old item data
 void CListCtrlCl::PostSortItems( vector<DWORD_PTR> &vec2StoreData )
 {
	 // adjust the cell data map
	 CellDataMap newMap;
	 int nRowCnt=GetItemCount();
	 int nColCnt=GetColumnCount();
	 for (int i=0; i<nRowCnt; ++i)
	 {
		 int nOldRow=(int)GetItemData(i);
		 for (int j=0; j<nColCnt; ++j)
		 {
			 newMap[make_pair(i,j)]=m_mapCell2Data[make_pair(nOldRow, j)];	
		 }
		 // restore item data
		 SetItemData(i, (DWORD_PTR)vec2StoreData[nOldRow]);
	 }
	 m_mapCell2Data.swap(newMap);	
 }

 void CListCtrlCl::SetSupportSort(BOOL bSuptSort)
 {
	 if (bSuptSort)
		 m_dwListCtrlExStyle|=SUPPORT_SORT;
	 else
		 m_dwListCtrlExStyle&=(~SUPPORT_SORT);
 }
 void CListCtrlCl::OnLvnColumnclick(NMHDR *pNMHDR, LRESULT *pResult)
 {
	
	 //单击了标题栏的某个列
	 CWaitCursor waitCursor;
	 LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	 if(IsSupportSort())
	 {
		 vector<DWORD_PTR> vecItemDatas(GetItemCount());
		 //map<int, int> mapOldRow2NewRow(GetItemCount());
		 PreSortItems(vecItemDatas);
		 //SortType eSortBy=GetColumnSortBy(pNMLV->iSubItem);
		 BOOL bIsAsc=!IsColumnSortAsc(pNMLV->iSubItem);
		 SetColumnSortDirection(pNMLV->iSubItem, bIsAsc);
		 _SortParam_t aSortParam;
		 aSortParam.m_ColParam=m_mapCol2Sort[pNMLV->iSubItem];
		 aSortParam.m_nSortCol=pNMLV->iSubItem;
		 aSortParam.m_pTheList=this;
		 SortItems(CompareFunc, (DWORD_PTR)&aSortParam);
		 //m_ctlSortHead.SetSortArrow(pNMLV->iSubItem, !bIsAsc);
		 PostSortItems(vecItemDatas);
	 }

	 *pResult = 0;
	 waitCursor.Restore();

 }

 BOOL CListCtrlCl::IsSupportSort()
 {
	 return m_dwListCtrlExStyle & SUPPORT_SORT;
 }

 BOOL	CListCtrlCl::IsColumnSortAsc(int nColIndex)
 {
	 // insert new if not found
	 ColSortMap::iterator iter=m_mapCol2Sort.find(nColIndex);
	 if (m_mapCol2Sort.end()==iter)
	 {
		 m_mapCol2Sort[nColIndex]=_ColumnSort_t();
	 }
	 return m_mapCol2Sort[nColIndex].m_bIsAsc;
 }

 void CListCtrlCl::SetColumnSortDirection(int nColIndex, BOOL bIsAsc )
 {
	 // init sort map
	 if (m_mapCol2Sort.empty())
	 {
		 for (int i=0; i<GetColumnCount(); ++i)
		 {
			 m_mapCol2Sort[i]=_ColumnSort_t();
		 }
	 }
	 // assign
	 m_mapCol2Sort[nColIndex].m_bIsAsc=bIsAsc;
 }
