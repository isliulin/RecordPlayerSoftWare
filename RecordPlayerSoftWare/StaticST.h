#pragma once


// CStaticST

class CStaticST : public CStatic
{
	DECLARE_DYNAMIC(CStaticST)

public:
	CStaticST();
	virtual ~CStaticST();

private:  
	BOOL b_FillBackColor;  
	COLORREF m_DisBackColro;  
	COLORREF m_BackColor;  
	COLORREF m_TextColor;  
	CFont *m_Font;  
	CBrush m_BackBrush;  
	CBrush m_DisBackBrush;  

	// Construction  
public:  
	CFont *GetFont() { return m_Font; }  
	virtual void SetFont(LOGFONT *LogFont, BOOL bRedraw = TRUE);  
	virtual void SetFont(CFont* pFont, BOOL bRedraw = TRUE);  
	virtual void SetFont(int nHeight, LPCTSTR fontName, BOOL bRedraw = true);  

	void SetBackColor(COLORREF cr);   
	COLORREF GetBackColor() { return(m_BackColor); };  

	void SetTextColor(COLORREF cr)   
	{   
		m_TextColor = cr;  
		if(GetSafeHwnd())  
			Invalidate();  
	}  

	COLORREF GetTextColor()   
	{  
		return(m_TextColor);   
	}  
	 
protected:  
//public:
	//{{AFX_MSG(CColorText_Static)  
	afx_msg HBRUSH CtlColor(CDC* pDC, UINT nCtlColor);  
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);  
	//}}AFX_MSG  
	DECLARE_MESSAGE_MAP()  


};


#pragma once  
/////////////////////////////////////////////////////////////////////////////  
// CColorText_Static window  

class CColorText_Static : public CStatic  
{  
	// Attributes  
private:  
	BOOL b_FillBackColor;  
	COLORREF m_DisBackColro;  
	COLORREF m_BackColor;  
	COLORREF m_TextColor;  
	CFont *m_Font;  
	CBrush m_BackBrush;  
	CBrush m_DisBackBrush;  

	// Construction  
public:  
	CFont *GetFont() { return m_Font; }  

	CColorText_Static();  
	virtual void SetFont(LOGFONT *LogFont, BOOL bRedraw = TRUE);  
	virtual void SetFont(CFont* pFont, BOOL bRedraw = TRUE);  
	virtual void SetFont(int nHeight, LPCTSTR fontName, BOOL bRedraw = true);  

	void Set_BackColor(COLORREF cr);   
	COLORREF Get_BackColor() { return(m_BackColor); };  

	void Set_TextColor(COLORREF cr)   
	{   
		m_TextColor = cr;  
		if(GetSafeHwnd())  
			Invalidate();  
	}  

	COLORREF Get_TextColor()   
	{  
		return(m_TextColor);   
	}  
	// Attributes  
public:  

	// Operations  
public:  

	// Overrides  
	// ClassWizard generated virtual function overrides  
	//{{AFX_VIRTUAL(CColorText_Static)  
	//}}AFX_VIRTUAL  

	// Implementation  
public:  
	virtual ~CColorText_Static();  

	// Generated message map functions  
protected:  
	//{{AFX_MSG(CColorText_Static)  
	afx_msg HBRUSH CtlColor(CDC* pDC, UINT nCtlColor);  
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);  
	//}}AFX_MSG  
	DECLARE_MESSAGE_MAP()  

};  
