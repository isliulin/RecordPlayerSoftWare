#pragma once
#include "HeaderCtrlCl.h"
#include <vector>
#include <list>
using namespace std;
// CListCtrlCl
//2015��7��31����checkbox
//���ÿ��checkBox��״̬
struct ITEM_CHECK
{
	short item;                 //�ڼ���
	short flag;                 //falg 0�ǿգ�1�Ǳ�ѡ�У���-1�Ǳ��
};

#define CELL_MAXLEN 256

typedef struct _CELLVALUE
{
	DWORD dwRow;//
	DWORD dwCol;//
	UINT nIDResource;//picture
	unsigned char cCtrlType;//111-IDC_EDIT,112-IDC_COMBO
	unsigned char cDataType;//100-char,101-int,102-float,103-char*
	WORD nStrMaxLen;
	char strPrevItem[CELL_MAXLEN];//���ITEMǰһ����ֵ
	unsigned char bItemChange;
	void* pValue;	//�洢���Ƕ�Ӧֵ��ָ��!
	unsigned char nComboStr;
	CString* pComboStr;
}CELLVALUE;

/*********************************************
* CExArray��չ��:����������CArray
* �ô�������CELLVALUE�ṹָ��
* ���ܣ���ӡ�ɾ������ȡ�ṹ��CELLVALUE��ָ��
**********************************************/
template<class T>
class CExArray
{
public:
	CExArray(int nSize=10){m_dwSize=10;m_dwCurCount=0;m_pVal=NULL;m_pVal=new T[nSize];};
	virtual ~CExArray(){if(m_pVal!=NULL){delete[] m_pVal;m_pVal=NULL;}};
	//////////////////////
public:
	void Add(T& cell);
	BOOL GetAt(DWORD dwRow,DWORD dwCol,T& cell);
	BOOL GetAt(DWORD dwIndex,T& cell);
	T* GetAt(DWORD dwRow,DWORD dwCol);
	T* GetAt(DWORD dwIndex);
	void DelAt(DWORD dwRow,DWORD dwCOl);
	void DelAll(void);
	int  GetCount(void);
private:
	T *m_pVal;
	DWORD m_dwSize;
	DWORD m_dwCurCount;
};

//��Ͻ���������
typedef list<CString> CStrList;

typedef enum CellType
{
	NormalText, // can show icon
	CheckBox, // can be checked
	RadioBox,  // can be checked
	ComboBox, // can be shown in place
	EditBox,  // can be shown in place
	Progress  // can show progress here
};

enum SortType
{
	SortByString,
	SortByDigit,
	SortByDate
};

struct CellData 
		{
		private:
			DWORD		m_dwData; // enabled at the first bit and checked at the second bit (count from the back)
			/*BOOL			m_bEnabled;
			BOOL			m_bChecked;*/
		public:
			#define Enable_Mask  0x00000001
			#define Check_Mask  0x00000002
			CellData(BOOL bEnabled=TRUE, BOOL bChecked=FALSE, int nRadioGroup=0, int nImage=-1, UINT uProgMaxVal=100) 
				: m_nRadioGroup(nRadioGroup), m_strValidChars(_T("")),
				m_nImage(nImage), m_uProgresVal(0), m_uProgressMax(uProgMaxVal)
			{
				SetEnable(bEnabled);
				SetCheck(bChecked);
			};

			int				m_nImage;
			
			int				m_nRadioGroup;
			CStrList		m_lstString;
			CString		m_strValidChars;
			UINT			m_uProgresVal;
			UINT			m_uProgressMax;

			inline void SetEnable(BOOL bEnbaled=TRUE) {
				if (bEnbaled)
					m_dwData |= Enable_Mask;
				else
					m_dwData &=(~Enable_Mask);
			}

			inline BOOL GetEnable() const {
				return (m_dwData&Enable_Mask);
			}

			inline void SetCheck(BOOL bChecked=TRUE){
				if(bChecked)
					m_dwData|=Check_Mask;
				else
					m_dwData &=(~Check_Mask);
			}

			inline BOOL GetCheck() const {
				return (m_dwData&Check_Mask);
			}
			
		};

typedef CellType ColumnType;
typedef map<int, ColumnType> ColMap; // column to column type
typedef pair<int, int> CellIndex; // row, column
typedef map<CellIndex, CellData> CellDataMap; // cell index(row, column) to cell data
typedef map<CellIndex, CProgressCtrl*> ProgressMap;


#define OFFSET_LEFT    10  //checkbox��ƫ��
#define OFFSET_TOP     7  //checkbox��ƫ��
#define OFFSET_BOTTOM  7  //checkbox��ƫ��
#define OFFSET_RIGHT   12  //checkbox��ƫ��
#define  ICON_OFFSET       15
#define  ICON_WIDTH       25

#define CHECK_BOX_COLOR  RGB(0, 76, 136) //�߿���ɫ
#define CHECK_BOX_BK_COLOR  RGB(255, 255, 255) //�߿򱳾���ɫ

class CListCtrlCl : public CListCtrl
{
	DECLARE_DYNAMIC(CListCtrlCl)

public:
	CHeaderCtrlCl m_Header;
	CListCtrlCl();
	virtual ~CListCtrlCl();

protected:
	DECLARE_MESSAGE_MAP()
	virtual void PreSubclassWindow();
public:
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
public:
	afx_msg void OnMeasureItem(int nIDCtl, LPMEASUREITEMSTRUCT lpMeasureItemStruct);
	void MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct);
public:
	// �и�
	int m_nRowHeight;
	int InsertColumn(int nCol, LPCTSTR lpszColumnHeading,
		int nFormat = LVCFMT_LEFT, int nWidth = -1, int nSubItem = -1);
public:
	// Gradient - ����ϵ�������屳����,���ý�����Ϊ0
	void SetHeaderBKColor(int R, int G, int B, int Gradient);
public:
	// ���ñ�ͷ�߶�
	void SetHeaderHeight(float Height);
	CPtrList m_ptrListCol;  //��������ɫ
	CPtrList m_ptrListItem; //����Item��ɫ��
	CPtrList m_colTextColor; //������������ɫ
	CPtrList m_ItemTextColor; //���浥Ԫ��������ɫ
	bool FindColColor(int col ,COLORREF &color); //��������ɫ
	bool FindItemColor(int col,int row,COLORREF &color);
	bool FindColTextColor(int col,COLORREF &color); //������������ɫ
	bool FindItemTextColor(int col,int row,COLORREF &color);
	void SetColColor(int col,COLORREF color);  //��������ɫ
	void SetItemColor(int col,int row,COLORREF color); //����Item��ɫ
	void SetColTextColor(int col,COLORREF color);   //�������ı���ɫ
	void SetItemTextColor(int col,int row,COLORREF color);
	void SetRowHeigt(int nHeight); //�����и�
	void SetHeaderFontHW(int nHeight,int nWith); //���ñ�ͷ�����С
	void SetHeaderTextColor(COLORREF color);
	COLORREF m_color;
	BOOL SetTextColor(COLORREF cr);
	void SetFontHW(int nHeight,int nWith);  //��������ĸߺͿ�
public:
	// ����߶�
	int m_fontHeight;
public:
	// ������
	int m_fontWith;

	//���Ӻ�����ӦCheckBox����
	int SelectOneCheckBox(int ItemID, CPoint pt);
	int DrawCheckBox(CDC *pDC,int nItem);
	int SetItemCheckBox(int itemID,int state);
     ITEM_CHECK              m_itemCheck;
     vector<ITEM_CHECK>      m_itemVector;
	 afx_msg void OnLButtonDown(UINT nFlags, CPoint point);

	 int InsertItem(int nItem,LPCTSTR lpszItem);
	 int InsertItem(int nItem,LPCTSTR lpszItem,int index);
	 BOOL DeleteAllItems();
	 BOOL DeleteSelectedItem();


	 //������ͼ����
	 //��ȡ��Ԫ���Rect
	 void GetCellRect(int nItem,int nSubItem,CRect& rect);
	 //�жϵ�Ԫ�����Ƿ���ͼƬ
	 BOOL IsItemImage(int row,int col);
	 //���õ�Ԫ��ͼƬ
	 void SetItemImage(int row,int col,UINT nIDResource);
	 CExArray<CELLVALUE> m_val;
public:
	//����Խ�����
	DWORD  m_dwListCtrlExStyle; //
	int	InsertColumn(int nCol, CString strColHead, int nWidth=-1,ColumnType eColType=NormalText, 
		int nFormat= LVCFMT_CENTER , SortType eSortBy=SortByString, int nSubItem= -1 );
	BOOL   IsShowPogressPercent();
	void   SetShowProgressPercent(BOOL bShowProgPercent=TRUE);
	void   SetCellProgressMaxValue(int nRow, int nCol, UINT uMaxValue=100);
	UINT   GetCellProgressValue(int nRow, int nCol);
	void   SetCellProgressValue(int nRow, int nCol, UINT uValue);
	CellData& FindCellData(int nRow, int nCol);
	int		GetColumnCount() { return GetHeaderCtrl()->GetItemCount(); }
	BOOL	GetCellRectEx(int iRow, int iCol, CRect &rect, int nArea=LVIR_BOUNDS);
    ColumnType GetColumnType(int nColIndex);
    void   SetColumnType(int nColIndex, ColumnType eColType);
	 void  SetSupportSort(BOOL bSuptSort);//��������
	 BOOL  IsSupportSort(); //�Ƿ�֧������;
	 BOOL IsColumnSortAsc(int nColIndex);
	 void SetColumnSortDirection(int nColIndex, BOOL bIsAsc );
 protected:
	 BOOL DrawBmpImage(CDC* pDC,int nRow,int nCol);  //����λͼ����
	 void DrawProgress(CDC *pDC,  CString &strText, CRect &rcCell, BOOL bSelected, const CellData &cellData); //���ƽ�����
	 BOOL DrawBmpImage(CDC *pDC,  CString &strText, int nRow,int nCol); //����ͼ�������

	 int GetProgBarSize(const CellData &cellData, CRect &rcProg, float *pPersent=NULL); //��ȡ�������ߴ�

	 inline void		DrawCell(CDC *pDC,  CString &strText, CRect &rcCell, BOOL bSelected, 
		 int nRow, int nCol);
	 CImageList*	TheImageList();
	 int			m_nImageListType;
	 CellDataMap	m_mapCell2Data;
	 ColMap			m_mapCol2ColType;

	 //����
	 protected:
		 struct _ColumnSort_t
		 {
			 _ColumnSort_t(SortType eSortType=SortByString, BOOL bIsAsc=TRUE)
				 :m_eSortType(eSortType), m_bIsAsc(bIsAsc)
			 {};
			 SortType	m_eSortType;
			 BOOL			m_bIsAsc;
		 };

		 struct _SortParam_t 
		 {
			 _ColumnSort_t  m_ColParam;
			 int					 m_nSortCol;
			 CListCtrlCl		 *m_pTheList;
		 };
		 typedef map<int, _ColumnSort_t> ColSortMap;
		 ColSortMap				m_mapCol2Sort;	
		 CHeaderCtrlCl	m_ctlSortHead;
		 static int CALLBACK CompareFunc(LPARAM lParam1, LPARAM lParam2,  LPARAM lParamSort);
		 static bool		IsDigitStr(LPCTSTR str);
		 static int		IntCompare(LPCTSTR strInt1st, LPCTSTR strInt2nd);
		 static int		StrCompare(LPCTSTR str1st, LPCTSTR str2nd);
		 void				PreSortItems( vector<DWORD_PTR> &vec2StoreData );
		 void				PostSortItems( vector<DWORD_PTR> &vec2StoreData );

	 
public:
	afx_msg void OnLvnColumnclick(NMHDR *pNMHDR, LRESULT *pResult);
	CDC* m_pDC; //����DC
};


