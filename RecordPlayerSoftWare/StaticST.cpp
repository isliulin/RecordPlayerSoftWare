// StaticST.cpp : 实现文件
//

#include "stdafx.h"
#include "StaticST.h"


// CStaticST
IMPLEMENT_DYNAMIC(CStaticST, CStatic)


CStaticST::CStaticST()  
{  
	b_FillBackColor = 0;  
	m_Font = 0;  
	m_DisBackColro = RGB(200, 200, 200);  
	SetBackColor(GetSysColor(COLOR_3DFACE));  
	SetTextColor(0);  
}  

CStaticST::~CStaticST()  
{  
	if(m_Font)  
	{  
		m_Font->DeleteObject();  
		delete m_Font;  
	}     

	m_BackBrush.DeleteObject();  
	m_DisBackBrush.DeleteObject();  
}  

void CStaticST::SetFont(LOGFONT *LogFont, BOOL bRedraw)  
{  
	if(m_Font)  
		m_Font->DeleteObject();  

	if(m_Font == NULL)  
		m_Font = new CFont();  

	if(m_Font)  
	{  
		if(!m_Font->CreatePointFontIndirect( LogFont ))  
		{  
			delete m_Font;  
			m_Font = NULL;  
		}  
	}  
}  

void CStaticST::SetFont(CFont* pFont, BOOL bRedraw)  
{  
	LOGFONT LogFont;  
	pFont->GetLogFont(&LogFont);  
	SetFont(&LogFont, bRedraw);  
}  

void CStaticST::SetFont(int nHeight, LPCTSTR fontName, BOOL bRedraw)  
{  
	if(m_Font)  
		m_Font->DeleteObject();  

	if(m_Font == NULL)  
		m_Font = new CFont();  

	if(m_Font)  
	{  
		if(!m_Font->CreatePointFont(nHeight, fontName))  
		{  
			delete m_Font;   
			m_Font = NULL;  
		}  
	}  
	if(bRedraw && GetSafeHwnd())  
		Invalidate();  
}  

void CStaticST::SetBackColor(COLORREF cr)   
{   
	m_BackColor = cr;   
	b_FillBackColor = true;   

	m_BackBrush.DeleteObject();  
	m_BackBrush.CreateSolidBrush(m_BackColor);  

	m_DisBackBrush.DeleteObject();  
	m_DisBackBrush.CreateSolidBrush(m_DisBackColro);  

	if(GetSafeHwnd())  
		Invalidate();   
}  

BEGIN_MESSAGE_MAP(CStaticST, CStatic)  
	//{{AFX_MSG_MAP(CStaticST)  
	ON_WM_CTLCOLOR_REFLECT()  
	ON_WM_ERASEBKGND()  
	//}}AFX_MSG_MAP  
END_MESSAGE_MAP()  

/////////////////////////////////////////////////////////////////////////////  
// CStaticST message handlers  

HBRUSH CStaticST::CtlColor(CDC* pDC, UINT nCtlColor)   
{  
	// TODO: Change any attributes of the DC here  
	pDC->SetBkMode(TRANSPARENT);  
	pDC->SetTextColor(m_TextColor);  

	if(m_Font)  
		pDC->SelectObject(m_Font);  

	HBRUSH hBrush = (HBRUSH) m_BackBrush;  
	if(!IsWindowEnabled())  
		hBrush = (HBRUSH) m_DisBackBrush;  
	return ( hBrush );      
	// TODO: Return a non-NULL brush if the parent's handler should not be called   return NULL;  
}  

BOOL CStaticST::OnEraseBkgnd(CDC* pDC)  
{  
	 
	return TRUE;  
}  


