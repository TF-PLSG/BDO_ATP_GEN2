#include "stdafx.h"
#include "ListExCtrl.h"
#include <afxtempl.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



///////////////////////////////////////////////////////////////////////////
// The item data class, stores item data and item text array for sorting
///////////////////////////////////////////////////////////////////////////
class ItemData
{
public:
	ItemData() : dwData(0) {} 
	CStringArray arrStr; // item texts
	DWORD dwData; // the actual item data
};


////////////////////////////////////////////////////////////////////////////
// CReportHeaderCtrl implementation
////////////////////////////////////////////////////////////////////////////
CListExCtrl::CReportHeaderCtrl::CReportHeaderCtrl()
{
	m_iSortColumn = -1;
	m_bSortAscending = TRUE;
}

int CListExCtrl::CReportHeaderCtrl::GetSortedColumn() const
{
	return m_iSortColumn;
}

BOOL CListExCtrl::CReportHeaderCtrl::IsSortAscending() const
{
	return m_bSortAscending;
}

void CListExCtrl::CReportHeaderCtrl::SetSortAscending(BOOL bAscending)
{
	m_bSortAscending = bAscending;
}

void CListExCtrl::CReportHeaderCtrl::SetSortedColumn(int nCol)
{
	m_iSortColumn = nCol;
}

////////////////////////////////////////////////////////////////////////////////
// Mark Jackson's code
////////////////////////////////////////////////////////////////////////////////
void CListExCtrl::CReportHeaderCtrl::UpdateSortArrow()
{
	// change the item to owner drawn.
	HD_ITEM hditem;
	
	hditem.mask = HDI_FORMAT;
	VERIFY(GetItem(m_iSortColumn, &hditem));
	hditem.fmt |= HDF_OWNERDRAW;
	VERIFY(SetItem(m_iSortColumn, &hditem));
	
	// invalidate the header control so it gets redrawn
	Invalidate();
}

////////////////////////////////////////////////////////////////////////////////
// Mark Jackson's code
////////////////////////////////////////////////////////////////////////////////
void CListExCtrl::CReportHeaderCtrl::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	// attath to the device context.
	CDC dc;
	VERIFY(dc.Attach(lpDrawItemStruct->hDC));
	
	// save the device context.
	const int iSavedDC = dc.SaveDC();
	
	// get the column rect.
	CRect rc(lpDrawItemStruct->rcItem);
	
	// set the clipping region to limit drawing within the column.
	CRgn rgn;
	VERIFY(rgn.CreateRectRgnIndirect(&rc));
	(void)dc.SelectObject(&rgn);
	VERIFY(rgn.DeleteObject());
	
	// draw the background,
	CBrush brush(GetSysColor(COLOR_3DFACE));
	dc.FillRect(rc, &brush);
	
	// get the column text and format.
	TCHAR szText[ 256 ];
	HD_ITEM hditem;
	
	hditem.mask = HDI_TEXT | HDI_FORMAT;
	hditem.pszText = szText;
	hditem.cchTextMax = 255;
	
	VERIFY(GetItem(lpDrawItemStruct->itemID, &hditem));
	
	// determine the format for drawing the column label.
	UINT uFormat = DT_SINGLELINE | DT_NOPREFIX | DT_NOCLIP | DT_VCENTER | DT_END_ELLIPSIS ;
	
	if(hditem.fmt & HDF_CENTER)
		uFormat |= DT_CENTER;
	else if(hditem.fmt & HDF_RIGHT)
		uFormat |= DT_RIGHT;
	else
		uFormat |= DT_LEFT;
	
	// adjust the rect if the mouse button is pressed on it.
	if(lpDrawItemStruct->itemState == ODS_SELECTED)
	{
		rc.left++;
		rc.top += 2;
		rc.right++;
	}
	
	CRect rcIcon(lpDrawItemStruct->rcItem);
	const int iOffset = (rcIcon.bottom - rcIcon.top) / 4;
	
	// adjust the rect further if the sort arrow is to be displayed.
	if(lpDrawItemStruct->itemID == (UINT)m_iSortColumn)
		rc.right -= 3 * iOffset;
	
	rc.left += iOffset;
	rc.right -= iOffset;
	
	// draw the column label.
	if(rc.left < rc.right)
		(void)dc.DrawText(szText, -1, rc, uFormat);
	
	// draw the sort arrow.
	if(lpDrawItemStruct->itemID == (UINT)m_iSortColumn)
	{
		// set up the pens to use for drawing the arrow.
		CPen penLight(PS_SOLID, 1, GetSysColor(COLOR_3DHILIGHT));
		CPen penShadow(PS_SOLID, 1, GetSysColor(COLOR_3DSHADOW));
		CPen* pOldPen = dc.SelectObject(&penLight);
		
		if(m_bSortAscending)
		{
			// draw the arrow pointing upwards.
			dc.MoveTo(rcIcon.right - 2 * iOffset, iOffset);
			dc.LineTo(rcIcon.right - iOffset, rcIcon.bottom - iOffset - 1);
			dc.LineTo(rcIcon.right - 3 * iOffset - 2, rcIcon.bottom - iOffset - 1);
			(void)dc.SelectObject(&penShadow);
			dc.MoveTo(rcIcon.right - 3 * iOffset - 1, rcIcon.bottom - iOffset - 1);
			dc.LineTo(rcIcon.right - 2 * iOffset, iOffset - 1);		
		}
		else
		{
			// draw the arrow pointing downwards.
			dc.MoveTo(rcIcon.right - iOffset - 1, iOffset);
			dc.LineTo(rcIcon.right - 2 * iOffset - 1, rcIcon.bottom - iOffset);
			(void)dc.SelectObject(&penShadow);
			dc.MoveTo(rcIcon.right - 2 * iOffset - 2, rcIcon.bottom - iOffset);
			dc.LineTo(rcIcon.right - 3 * iOffset - 1, iOffset);
			dc.LineTo(rcIcon.right - iOffset - 1, iOffset);		
		}
		
		// restore the pen.
		(void)dc.SelectObject(pOldPen);
	}
	
	// restore the previous device context.
	VERIFY(dc.RestoreDC(iSavedDC));
	
	// detach the device context before returning.
	(void)dc.Detach();
}


////////////////////////////////////////////////////////////////////////////
// CListExCtrl implementation
////////////////////////////////////////////////////////////////////////////
CListExCtrl::CListExCtrl()
{
	m_bSort = true;
	m_wndHeader.SetSortedColumn(0);
	m_wndHeader.SetSortAscending(1);

	InitializeCriticalSection(&csLock);
}

CListExCtrl::~CListExCtrl()
{
}

BEGIN_MESSAGE_MAP(CListExCtrl, CListCtrl)
//{{AFX_MSG_MAP(CListExCtrl)
ON_NOTIFY_REFLECT(LVN_COLUMNCLICK, OnColumnClick)
ON_NOTIFY_REFLECT ( NM_CUSTOMDRAW, OnCustomdraw )
ON_WM_DESTROY()
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CListExCtrl message handlers

void CListExCtrl::PreSubclassWindow()
{
	// the list control must have the report style.
	ASSERT(GetStyle() & LVS_REPORT);
	
	CListCtrl::PreSubclassWindow();
	VERIFY(m_wndHeader.SubclassWindow(GetHeaderCtrl()->GetSafeHwnd()));
	
	// I can't see any reason that a report list does not use LVS_EX_FULLROWSELECT style,
	// but of course, user can remove this style by "SetFullRowSelect(FALSE)" any time.
	SetFullRowSelect(TRUE);
}

BOOL CListExCtrl::_IsValidIndex(int nIndex) const
{
	return nIndex >= 0 && nIndex < CListCtrl::GetItemCount();
}

////////////////////////////////////////////////////////////////////////////////
// Mark Jackson's code
////////////////////////////////////////////////////////////////////////////////
BOOL CListExCtrl::SetHeadings(UINT uiStringID)
{
	CString strHeadings;
	VERIFY(strHeadings.LoadString(uiStringID));
	return SetHeadings(strHeadings);
}

////////////////////////////////////////////////////////////////////////////////
// Mark Jackson's code, modified by Bin Liu
////////////////////////////////////////////////////////////////////////////////
// the heading text is in the format column 1 text,column 1 width;column 2 text,column 3 width;etc.
BOOL CListExCtrl::SetHeadings(const CString& strHeadings)
{
	CString sPara;
	int nParaStart = 0;
	int nSemiColon = -1;
	BOOL bContinue = TRUE;
	
	while (nSemiColon < strHeadings.GetLength())
	{
		// determine the paragraph ("xxx,xxx,xxx;")
		nSemiColon = strHeadings.Find(_T(';'), nParaStart);
		if(nSemiColon == -1)
		{
			// reached the end of string
			nSemiColon = strHeadings.GetLength();
			bContinue = FALSE;
		}
		
		sPara = strHeadings.Mid(nParaStart, nSemiColon - nParaStart);
		nParaStart = nSemiColon + 1; // ready to move on to next paragraph
		
		int iStart = 0;
		int iComma = -1;
		
		// find the heading name string
		iComma = sPara.Find(_T(','), iStart);
		
		if(iComma == -1)
			break;
		
		const CString sColName = sPara.Mid(iStart, iComma - iStart);
		
		iStart = iComma + 1;
		
		// find the heading format string (0=LVCFMT_LEFT, 1=LVCFMT_CENTER, 2=LVCFMT_RIGHT)
		int nFmt = LVCFMT_LEFT;
		iComma = sPara.Find(_T(','), iStart);
		if (iComma == -1)
		{
			// user does not specify a format, so use LVCFMT_LEFT as default)
		}
		else
		{
			// there is a format string
			int n = _ttoi(sPara.Mid(iStart, iComma - iStart));
			
			if (n == 1)
				nFmt = LVCFMT_CENTER;
			else if (n == 2)
				nFmt = LVCFMT_RIGHT;
			else
				nFmt = LVCFMT_LEFT;
			
			iStart = iComma + 1;
		}
		
		// remained is the column width string
		int iWidth = _ttoi(sPara.Mid(iStart, sPara.GetLength() - iStart));
		if (iWidth < 1)
			iWidth = 1; // width should be at least 1
		
		if(InsertColumn(m_wndHeader.GetItemCount(), sColName, nFmt, iWidth) == -1)
			return FALSE;
	}
	
	return TRUE;
}

int CListExCtrl::InsertItem(int nIndex, LPCTSTR pszText, ...)
{
	const int iIndex = CListCtrl::InsertItem(nIndex, pszText);
	
	if (!_IsValidIndex(iIndex))
		return iIndex;
	
	CStringArray arr;
	arr.Add(pszText);
	
	va_list list;
	va_start(list, pszText);
	
	for(int iColumn = 1; iColumn < GetColumnCount(); iColumn++)
	{
		LPCTSTR lpsz = va_arg(list, LPCTSTR);
		  if(lpsz == NULL)
		  {
			 return 0;
		  }
		CString str = (lpsz == NULL) ? _T("") : lpsz;
		arr.Add(str);
		CListCtrl::SetItemText(iIndex, iColumn, str);
	}
	
	va_end(list);
	
	_AssignNewItemData(iIndex, arr.GetData(), arr.GetSize());
	
	return iIndex;
}

void CListExCtrl::_FreeItemMemory(const int iItem)
{
	ASSERT(_IsValidIndex(iItem));
	ItemData* pid = reinterpret_cast<ItemData*>(CListCtrl::GetItemData(iItem));
	if (pid)
	{
		delete pid;
		pid = NULL;
	}
	
	VERIFY(CListCtrl::SetItemData(iItem, NULL));
}

BOOL CListExCtrl::DeleteItem(int iItem, BOOL bSelectNextItem, ITEMDATAPROC lpFunc, LPARAM lParam)
{
	if (!_IsValidIndex(iItem))
		return FALSE;
	
	if (lpFunc != NULL && !lpFunc(GetItemData(iItem), lParam))
		return FALSE;
	
	DWORD dwData = CListCtrl::GetItemData(iItem);
	if (CListCtrl::DeleteItem(iItem))
	{
		// Free memory
		ItemData* pData = reinterpret_cast<ItemData*>(dwData);
		if (pData != NULL)
			delete pData;
		
		if (bSelectNextItem)
			SelectItem(iItem, TRUE);
		
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

int CListExCtrl::DeleteAllItems(ITEMDATAPROC lpFunc, LPARAM lParam)
{
	int nCount = 0;
	while (TRUE)
	{
		if (!DeleteItem(0, FALSE, lpFunc, lParam))
			return nCount;
		nCount++;
	}
	
	return nCount;
}

int CListExCtrl::_StringCompare(const CString& s1, const CString& s2)
{
	if (s1 < s2)
		return -1;
	
	if (s1 > s2)
		return 1;
	
	return 0;
}

BOOL CListExCtrl::_IsNumber(LPCTSTR pszText)
{
	const int LEN = _tcslen(pszText);
	
	if (LEN == 0)
		return FALSE;
	
	BOOL bPointFound = FALSE;
	
	for(int i = 0; i < LEN; i++)
	{
		const TCHAR c = pszText[i];
		
		if (c == _T('.'))
			bPointFound = TRUE;
		
		if ((c >= _T('0') && c <= _T('9')) 
			|| (c == _T('.') && !bPointFound)
			|| (i == 0 && LEN > 1 && (c == _T('-')) || c == _T('+')))
		{
			// ok
		}
		else
		{
			return FALSE;
		}
	}
	
	return TRUE;
}

int CListExCtrl::_NumberCompare(LPCTSTR pszNumber1, LPCTSTR pszNumber2)
{
	const double fNumber1 = _tcstod(pszNumber1, NULL);
	const double fNumber2 = _tcstod(pszNumber2, NULL);
	
	if(fNumber1 < fNumber2)
		return -1;
	
	if(fNumber1 > fNumber2)
		return 1;
	
	return 0;
}

BOOL CListExCtrl::_IsDate(LPCTSTR pszText)
{
	COleDateTime date;
	date.ParseDateTime(pszText);
	return date.GetStatus() == COleDateTime::valid;
}

int CListExCtrl::_DateCompare(const CString& strDate1, const CString& strDate2)
{
	COleDateTime date1, date2;
	date1.ParseDateTime(strDate1);
	date2.ParseDateTime(strDate2);
	
	if (date1.GetStatus() != COleDateTime::valid || date2.GetStatus() != COleDateTime::valid)
		return _StringCompare(strDate1, strDate2); // not date
	
	if (date1 < date2)
		return -1;
	
	if (date1 > date2)
		return 1;
	
	return 0;
}

////////////////////////////////////////////////////////////////////////////////
// Mark Jackson's code, modified by Bin Liu
////////////////////////////////////////////////////////////////////////////////
int CALLBACK CListExCtrl::_CompareFunction(LPARAM lParam1, LPARAM lParam2, LPARAM lParamData)
{
	int bRet = false;
	CListExCtrl* pListCtrl = reinterpret_cast<CListExCtrl*>(lParamData);
	ASSERT(pListCtrl->IsKindOf(RUNTIME_CLASS(CListCtrl)));
	
	ItemData* pid1 = reinterpret_cast<ItemData*>(lParam1);
	ItemData* pid2 = reinterpret_cast<ItemData*>(lParam2);
	
	ASSERT(pid1);
	ASSERT(pid2);
	int col = pListCtrl->m_wndHeader.GetSortedColumn();
	CString s1 = pid1->arrStr[ col];
	CString s2 = pid2->arrStr[ col];
	CString rs1 = pid1->arrStr[0];
	CString rs2 = pid2->arrStr[0];

	bool b1 = false, b2 = false;
	
	int r1 = pListCtrl->GetRowNumber(0, rs1);
	int r2 = pListCtrl->GetRowNumber(0, rs2);

	if(LVS_EX_RED == pListCtrl->GetItemState(r1,LVS_EX_RED) )
	{
		b1 = true;
	}
	if(LVS_EX_RED == pListCtrl->GetItemState(r2,LVS_EX_RED) )
	{
		b2 = true;
	}
	s1.TrimLeft();
	s1.TrimRight();
	s2.TrimLeft();
	s2.TrimRight();
	
	if(b1 && !b2)
	{
		
			bRet = -1;
	}
	else if(!b1 && b2)
	{
		
			bRet = 1;
	}
	else
	{
	
		bRet = pListCtrl->m_wndHeader.IsSortAscending() ? _StringCompare(s1, s2) : _StringCompare(s2, s1);
	}
	return bRet;
	/*
	if(_IsNumber(s1))
	return pListCtrl->m_wndHeader.IsSortAscending() ? _NumberCompare(s1, s2) : _NumberCompare(s2, s1);
	else if(_IsDate(s1))
	return pListCtrl->m_wndHeader.IsSortAscending() ? _DateCompare(s1, s2) : _DateCompare(s2, s1);
	else
	// text.
	return pListCtrl->m_wndHeader.IsSortAscending() ? _StringCompare(s1, s2) : _StringCompare(s2, s1);
	*/
}

////////////////////////////////////////////////////////////////////////////////
// Mark Jackson's code
////////////////////////////////////////////////////////////////////////////////
void CListExCtrl::OnCustomdraw (NMHDR* pNMHDR, LRESULT* pResult)
{
	NMLVCUSTOMDRAW* pLVCD = reinterpret_cast<NMLVCUSTOMDRAW*>( pNMHDR );
	
    // Take the default processing unless we set this to something else below.
    *pResult = CDRF_DODEFAULT;
	
    // First thing - check the draw stage. If it's the control's prepaint
    // stage, then tell Windows we want messages for every item.
	
    if ( CDDS_PREPAINT == pLVCD->nmcd.dwDrawStage )
	{
        *pResult = CDRF_NOTIFYITEMDRAW;
	}
    else if ( CDDS_ITEMPREPAINT == pLVCD->nmcd.dwDrawStage )
	{
        // This is the prepaint stage for an item. Here's where we set the
        // item's text color. Our return value will tell Windows to draw the
        // item itself, but it will use the new color we set here.
		
        COLORREF crText = RGB(0,0,0);
        //pLVCD->nmcd.dwItemSpec  - Item number
        
		if(LVS_EX_RED == GetItemState(pLVCD->nmcd.dwItemSpec,LVS_EX_RED) )
		{
			crText = RGB(255,0,0);
		}
        
        // Store the color back in the NMLVCUSTOMDRAW struct.
        pLVCD->clrText = crText;
		
        // Tell Windows to paint the control itself.
        *pResult = CDRF_DODEFAULT;
	}
}

void CListExCtrl::OnColumnClick(NMHDR* pNMHDR, LRESULT* pResult)
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	const int iColumn = pNMListView->iSubItem;
	
	// if it's a second click on the same column then reverse the sort order,
	// otherwise sort the new column in ascending order.
	Sort(iColumn, iColumn == m_wndHeader.GetSortedColumn() ? !m_wndHeader.IsSortAscending() : TRUE);
	
	*pResult = 0;
}

void CListExCtrl::Sort(int iColumn, BOOL bAscending)
{
	if(!m_bSort)
		return;
	
	if (iColumn < 0 || iColumn >= GetColumnCount())
		return;
	
	m_wndHeader.SetSortedColumn(iColumn);
	m_wndHeader.SetSortAscending(bAscending);
	
	// show the appropriate arrow in the header control.
	m_wndHeader.UpdateSortArrow();
	
	VERIFY(SortItems(_CompareFunction, reinterpret_cast<DWORD>(this)));
}

void CListExCtrl::OnDestroy() 
{
	for(int iItem = 0; iItem < GetItemCount(); iItem ++)
		_FreeItemMemory(iItem);
	
	CListCtrl::OnDestroy();
}

BOOL CListExCtrl::SetItemText(int nItem, int nSubItem, LPCTSTR lpszText)
{
	if(!CListCtrl::SetItemText(nItem, nSubItem, lpszText))
		return FALSE;
	
	ItemData* pid = reinterpret_cast<ItemData*>(CListCtrl::GetItemData(nItem));
	ASSERT(pid);
	pid->arrStr[nSubItem] = lpszText;
	return TRUE;
}

////////////////////////////////////////////////////////////////////////////////
// Mark Jackson's code
////////////////////////////////////////////////////////////////////////////////
BOOL CListExCtrl::SetItemData(int nItem, DWORD dwData)
{
	if(!_IsValidIndex(nItem))
		return FALSE;
	
	ItemData* pid = reinterpret_cast<ItemData*>(CListCtrl::GetItemData(nItem));
	ASSERT(pid);
	pid->dwData = dwData;
	
	return TRUE;
}

////////////////////////////////////////////////////////////////////////////////
// Mark Jackson's code
////////////////////////////////////////////////////////////////////////////////
DWORD CListExCtrl::GetItemData(int nItem) const
{
	ASSERT(_IsValidIndex(nItem));
	
	ItemData* pid = reinterpret_cast<ItemData*>(CListCtrl::GetItemData(nItem));
	ASSERT(pid);
	return pid->dwData;
}

BOOL CListExCtrl::_AssignNewItemData(int iItem, LPCTSTR lpItem)
{
	ASSERT(_IsValidIndex(iItem));	
	ItemData* pid = new ItemData;
	pid->dwData = CListCtrl::GetItemData(iItem); // get original data
	pid->arrStr.SetSize(GetColumnCount());
	pid->arrStr[0] = lpItem;
	
	return CListCtrl::SetItemData(iItem, reinterpret_cast<DWORD>(pid));
}

BOOL CListExCtrl::_AssignNewItemData(int iItem, const CString* pTexts, int nSize)
{
	ASSERT(pTexts != NULL);
	ASSERT(_IsValidIndex(iItem));	
	ItemData* pid = new ItemData;
	pid->dwData = CListCtrl::GetItemData(iItem); // get original data
	pid->arrStr.SetSize(GetColumnCount());
	
	for (int i = 0; i < min(nSize, GetColumnCount()); i++)
		pid->arrStr[i] = pTexts[i];
	
	return CListCtrl::SetItemData(iItem, reinterpret_cast<DWORD>(pid));
}

int CListExCtrl::GetColumnCount() const
{
	return m_wndHeader.GetItemCount();
}

int CListExCtrl::GetFirstSelectedItem(int nStartAfter) const
{
	nStartAfter = max(nStartAfter, -1);
	for (int i = nStartAfter + 1; i < GetItemCount(); i++)
	{
		if (IsItemSelected(i))
			return i;
	}
	
	return -1;
}

void CListExCtrl::Sort()
{
	SendMessage(SORT,0,0);
	
}

int CListExCtrl::InsertItem(const LVITEM *pItem)
{
	const int nIndex = CListCtrl::InsertItem(pItem);
	if (!_IsValidIndex(nIndex))
		return nIndex;
	
	VERIFY(_AssignNewItemData(nIndex, pItem->pszText));
	return nIndex;
}

int CListExCtrl::InsertItem(int nItem, LPCTSTR lpszItem, int nImage)
{
	const int nIndex = CListCtrl::InsertItem(nItem, lpszItem, nImage);
	if (!_IsValidIndex(nIndex))
		return nIndex;
	
	VERIFY(_AssignNewItemData(nIndex, lpszItem));
	return nIndex;
}

int CListExCtrl::InsertItem(UINT nMask, int nItem, LPCTSTR lpszItem, UINT nState, UINT nStateMask, int nImage, LPARAM lParam)
{
	const int nIndex = CListCtrl::InsertItem(nMask, nItem, lpszItem, nState, nStateMask, nImage, lParam);
	if (!_IsValidIndex(nIndex))
		return nIndex;
	
	VERIFY(_AssignNewItemData(nIndex, lpszItem));
	return nIndex;
}

void CListExCtrl::SetFullRowSelect(BOOL bSet)
{
	DWORD dwStype = CListCtrl::GetExtendedStyle();
	
	if (bSet)
		dwStype |= LVS_EX_FULLROWSELECT;
	else
		dwStype &= ~LVS_EX_FULLROWSELECT;
	
	CListCtrl::SetExtendedStyle(dwStype);
}

void CListExCtrl::SetGridLines(BOOL bSet)
{
	DWORD dwStype = CListCtrl::GetExtendedStyle();
	if (bSet)
		dwStype |= LVS_EX_GRIDLINES;
	else
		dwStype &= ~LVS_EX_GRIDLINES;
	
	CListCtrl::SetExtendedStyle(dwStype);
	
}

int CListExCtrl::InsertColumn(int nCol, const LVCOLUMN *pColumn)
{
	const int nIndex = CListCtrl::InsertColumn(nCol, pColumn);
	if (nIndex > -1)
	{
		for (int i = 0; i < GetItemCount(); i++)
			_ResetTextArray(i);
	}
	
	return nIndex;
}

int CListExCtrl::InsertColumn(int nCol, LPCTSTR lpszColumnHeading, int nFormat, int nWidth, int nSubItem)
{
	const int nIndex = CListCtrl::InsertColumn(nCol, lpszColumnHeading, nFormat, nWidth, nSubItem);
	if (nIndex > -1)
	{
		for (int i = 0; i < GetItemCount(); i++)
			_ResetTextArray(i);
	}
	
	return nIndex;
}

BOOL CListExCtrl::_ResetTextArray(int nItem)
{
	if (!_IsValidIndex(nItem))
		return FALSE;
	
	ItemData* pid = reinterpret_cast<ItemData*>(CListCtrl::GetItemData(nItem));
	if (pid == NULL)
		return FALSE;
	
	pid->arrStr.SetSize(GetColumnCount());
	for (int i = 0; i < GetColumnCount(); i++)
		pid->arrStr[i] = CListCtrl::GetItemText(nItem, i);
	
	return TRUE;
}

BOOL CListExCtrl::DeleteColumn(int nCol)
{
	if (!CListCtrl::DeleteColumn(nCol))
		return FALSE;
	
	for (int i = 0; i < GetItemCount(); i++)
		_ResetTextArray(i);
	
	return TRUE;
}

BOOL CListExCtrl::SelectItem(int nItem, BOOL bSelectAdjacentIfNotAvailable)
{
	if (!_IsValidIndex(nItem))
	{
		if (!bSelectAdjacentIfNotAvailable)
		{
			return FALSE;
		}
		else
		{
			if (nItem < 0)
				nItem = 0;
			
			if (nItem >= GetItemCount())
				nItem = GetItemCount() - 1;
		}
	}
	
	return SetItemState(nItem, LVIS_SELECTED, LVIS_SELECTED);
}

BOOL CListExCtrl::IsItemSelected(int nItem) const
{
	POSITION pos = GetFirstSelectedItemPosition();
	if (pos != NULL)
	{
		while (pos)
		{
			if (nItem == GetNextSelectedItem(pos))
				return TRUE;
		}
	}
	
	return FALSE;
}

int CListExCtrl::DeleteAllSelectedItems(ITEMDATAPROC lpFunc, LPARAM lParam)
{
	int nCount = 0;
	while (TRUE)
	{
		const int n = GetFirstSelectedItem();
		if (!DeleteItem(n, FALSE, lpFunc, lParam))
			return nCount;
		
		nCount++;
	}
	
	return nCount;
}

void CListExCtrl::SetCheckboxes(BOOL bSet)
{
	DWORD dwStype = CListCtrl::GetExtendedStyle();
	if (bSet)
	{
		dwStype |= LVS_EX_CHECKBOXES;
	}
	else
	{
		dwStype &= ~LVS_EX_CHECKBOXES;
	}
	
	CListCtrl::SetExtendedStyle(dwStype);
}

BOOL CListExCtrl::MoveUp(int nItem, int nCount)
{	
	if (nCount < 0)
		return MoveDown(nItem, 0 - nCount);
	
	for (int i = nItem; i > nItem - nCount; i--)
	{
		if (!SwapItems(i, i - 1))
			return FALSE;
	}
	
	return TRUE;
}

BOOL CListExCtrl::MoveDown(int nItem, int nCount)
{
	if (nCount < 0)
		return MoveUp(nItem, 0 - nCount);
	
	for (int i = nItem; i < nItem + nCount; i++)
	{
		if (!SwapItems(i, i + 1))
			return FALSE;
	}
	
	return TRUE;
}

BOOL CListExCtrl::InvertSelect()
{
	BOOL bRes = TRUE;
	for (int i = 0; i < GetItemCount(); i++)
	{
		if (IsItemSelected(i))
			bRes &= UnSelectItem(i);
		else
			bRes &= SelectItem(i);
	}
	return bRes;
}

BOOL CListExCtrl::IsItemChecked(int nItem) const
{
	return GetCheck(nItem);
}

BOOL CListExCtrl::CheckItem(int nItem)
{
	return CListCtrl::SetCheck(nItem, TRUE);
}

BOOL CListExCtrl::UnSelectItem(int nItem)
{
	if (!_IsValidIndex(nItem))
		return FALSE;
	
	return SetItemState(nItem, 0, LVIS_SELECTED);
}

BOOL CListExCtrl::SelectAllItems()
{
	BOOL bRes = TRUE;
	for (int i = 0; i < GetItemCount(); i++)
	{
		if (!IsItemSelected(i))
			bRes &= SelectItem(i);
	}
	
	return bRes;
}

BOOL CListExCtrl::UnSelectAllItems()
{
	BOOL bRes = TRUE;
	for (int i = 0; i < GetItemCount(); i++)
	{
		if (IsItemSelected(i))
			bRes &= UnSelectItem(i);
	}
	
	return bRes;
}

BOOL CListExCtrl::UnCheckItem(int nItem)
{
	return CListCtrl::SetCheck(nItem, FALSE);
}

void CListExCtrl::CheckAllItems()
{
	
	for (int i = 0; i < GetItemCount(); i++)
		CListCtrl::SetCheck(i, TRUE);
}

void CListExCtrl::UnCheckAllItems()
{
	for (int i = 0; i < GetItemCount(); i++)
		CListCtrl::SetCheck(i, FALSE);
}	

void CListExCtrl::InvertCheck()
{
	for (int i = 0; i < GetItemCount(); i++)
		CListCtrl::SetCheck(i, !CListCtrl::GetCheck(i));
}

int CListExCtrl::GetSelectedItemCount() const
{
	return CListCtrl::GetSelectedCount();
}

int CListExCtrl::GetCheckedItemCount() const
{
	UINT nCount = 0;
	for (int i = 0; i < GetItemCount(); i++)
	{
		if (CListCtrl::GetCheck(i))
			nCount++;
	}
	
	return nCount;
}

BOOL CListExCtrl::SwapItems(int nItem1, int nItem2)
{
	if (!_IsValidIndex(nItem1) || !_IsValidIndex(nItem2))
		return FALSE;
	
	// record previous states first
	const BOOL bSel1 = IsItemSelected(nItem1);
	const BOOL bSel2 = IsItemSelected(nItem2);
	const BOOL bCheck1 = CListCtrl::GetCheck(nItem1);
	const BOOL bCheck2 = CListCtrl::GetCheck(nItem2);
	const DWORD dwData1 = GetItemData(nItem1);
	const DWORD dwData2 = GetItemData(nItem2);
	
	// swap item texts
	for (int i = 0; i < GetColumnCount(); i++)
	{
		CString str = GetItemText(nItem1, i);
		SetItemText(nItem1, i, GetItemText(nItem2, i));
		SetItemText(nItem2, i, str);
	}
	
	// swap item data
	SetItemData(nItem1, dwData2);
	SetItemData(nItem2, dwData1);
	
	// restore checked/unchecked states
	CListCtrl::SetCheck(nItem1, bCheck2);
	CListCtrl::SetCheck(nItem2, bCheck1);
	
	// restore selection mark(s)
	if (bSel2)
		SelectItem(nItem1);
	else
		UnSelectItem(nItem1);
	
	if (bSel1)
		SelectItem(nItem2);
	else
		UnSelectItem(nItem2);
	
	return TRUE;
}

BOOL CListExCtrl::MoveToTop(int nItem)
{
	return MoveUp(nItem, nItem);
}

BOOL CListExCtrl::MoveToBottom(int nItem)
{
	return MoveDown(nItem, GetItemCount() - nItem - 1);
}

int CListExCtrl::GetFirstCheckedItem(int nStartAfter) const
{
	nStartAfter = max(nStartAfter, -1);
	for (int i = nStartAfter + 1; i < CListCtrl::GetItemCount(); i++)
	{
		if (IsItemChecked(i))
			return i;
	}
	
	return -1;
}

BOOL CListExCtrl::SetItemText(int nItem, int nSubItem, TCHAR val)
{
	return SetItemText(nItem, nSubItem, CString(val));
}

BOOL CListExCtrl::SetItemText(int nItem, int nSubItem, BYTE val)
{
	return SetItemText(nItem, nSubItem, (ULONG)val);
}

BOOL CListExCtrl::SetItemText(int nItem, int nSubItem, SHORT val)
{
	return SetItemText(nItem, nSubItem, (LONG)val);
}

BOOL CListExCtrl::SetItemText(int nItem, int nSubItem, USHORT val)
{
	return SetItemText(nItem, nSubItem, (ULONG)val);
}

BOOL CListExCtrl::SetItemText(int nItem, int nSubItem, INT val)
{
	return SetItemText(nItem, nSubItem, (LONG)val);
}

BOOL CListExCtrl::SetItemText(int nItem, int nSubItem, UINT val)
{
	return SetItemText(nItem, nSubItem, (ULONG)val);
}

BOOL CListExCtrl::SetItemText(int nItem, int nSubItem, LONG val)
{
	CString sText;
	sText.Format(_T("%d"), val);
	return SetItemText(nItem, nSubItem, sText);
}

BOOL CListExCtrl::SetItemText(int nItem, int nSubItem, ULONG val)
{
	CString sText;
	sText.Format(_T("%d"), val);
	return SetItemText(nItem, nSubItem, sText);
}

BOOL CListExCtrl::SetItemText(int nItem, int nSubItem, FLOAT val, int nPrecision)
{
	return SetItemText(nItem, nSubItem, (double)val, nPrecision);
}

BOOL CListExCtrl::SetItemText(int nItem, int nSubItem, double val, int nPrecision)
{
	CString sText, sFmt;
	
	if (nPrecision >= 0)
		sFmt.Format(_T("%%.%df"), nPrecision);
	
	sText.Format(sFmt, val);
	return SetItemText(nItem, nSubItem, sText);
}

int CListExCtrl::DeleteAllUnselectedItems(ITEMDATAPROC lpFunc, LPARAM lParam)
{
	int nCount = 0;
	while (TRUE)
	{
		const int n = GetFirstUnselectedItem();
		if (!DeleteItem(n, FALSE, lpFunc, lParam))
			return nCount;
		
		nCount++;
	}
	
	return nCount;
}

BOOL CListExCtrl::DeleteAllCheckedItems(ITEMDATAPROC lpFunc, LPARAM lParam)
{
	int nCount = 0;
	while (TRUE)
	{
		const int n = GetFirstCheckedItem();
		if (!DeleteItem(n, FALSE, lpFunc, lParam))
			return nCount;
		
		nCount++;
	}
	
	return nCount;
}

BOOL CListExCtrl::DeleteAllUncheckedItems(ITEMDATAPROC lpFunc, LPARAM lParam)
{
	int nCount = 0;
	while (TRUE)
	{
		const int n = GetFirstUncheckedItem();
		if (!DeleteItem(n, FALSE, lpFunc, lParam))
			return nCount;
		
		nCount++;
	}
	
	return nCount;
}

BOOL CListExCtrl::DeleteAllColumns()
{
	while (GetColumnCount() > 0)
	{
		if (!DeleteColumn(0))
			return FALSE;
	}
	
	return TRUE;
}

int CListExCtrl::GetUnselectedItemCount() const
{
	return GetItemCount() - GetSelectedItemCount();
}

int CListExCtrl::GetUncheckedItemCount() const
{
	return GetItemCount() - GetCheckedItemCount();
}

int CListExCtrl::GetFirstUnselectedItem(int nStartAfter) const
{
	nStartAfter = max(nStartAfter, -1);
	for (int i = nStartAfter + 1; i < GetItemCount(); i++)
	{
		if (!IsItemSelected(i))
			return i;
	}
	
	return -1;
}

int CListExCtrl::GetFirstUncheckedItem(int nStartAfter) const
{
	nStartAfter = max(nStartAfter, -1);
	for (int i = nStartAfter + 1; i < GetItemCount(); i++)
	{
		if (!IsItemChecked(i))
			return i;
	}
	
	return -1;
}

void CListExCtrl::SetRed(int nItem, BOOL bRed)
{
	if(bRed)
	{
		if(LVS_EX_RED != GetItemState(nItem,LVS_EX_RED))
		{
			SetItemState( nItem, LVS_EX_RED, LVS_EX_RED);
			MoveToTop(nItem);
		}
	}
	else
	{
		SetItemState( nItem, 0, LVS_EX_RED);
	}
}

void CListExCtrl::EnableSorting(bool bSort)
{
	m_bSort = bSort;
}

BOOL CListExCtrl::UpdateItemText(const CString &row_name, int nSubItem, LPTSTR lpszText)
{
	LISTITEM listItem;
	memset(&listItem.lvi, 0, sizeof(LVITEM));
	listItem.row_name = row_name;
	listItem.lvi.iSubItem = nSubItem;
	listItem.lvi.pszText = lpszText;

	int ret = SendMessage(UPDATEITEMTEXT,0, (LPARAM)&listItem);
	return ret;
/*	int len = GetItemCount();
	for (int i=0; i<len; i++)
	{
		CString str = GetItemText(i,0);
		if(str.Compare(row_name) == 0)
		{
			SetItemText(i, iSubItem, lpszText);
			return i;
		}
	}
	return -1;*/
}

BOOL CListExCtrl::UpdateItemText(const CString& row_name, int iSubItem, double val, int nPrecision) // Double.   
{
	CString str;
	str.Format("%0.01lf",val);
	int len = str.GetLength();
	int ret = UpdateItemText(row_name, iSubItem, str.GetBuffer(len));
	str.ReleaseBuffer(len);
	return ret;
	
}
BOOL CListExCtrl::UpdateItemText(const CString& row_name, int iSubItem, float val, int nPrecision) // Float.      
{
	CString str;
	str.Format("%0.01f",val);
	int len = str.GetLength();
	int ret = UpdateItemText(row_name, iSubItem, str.GetBuffer(len));
	str.ReleaseBuffer(len);
	return ret;
}
BOOL CListExCtrl::UpdateItemText(const CString& row_name, int iSubItem, ULONG val) // Unsigned long.                   
{
	CString str;
	str.Format("%ld",val);
	int len = str.GetLength();
	int ret = UpdateItemText(row_name, iSubItem, str.GetBuffer(len));
	str.ReleaseBuffer(len);
	return ret;
}
BOOL CListExCtrl::UpdateItemText(const CString& row_name, int iSubItem, LONG val) // Long.                             
{
	CString str;
	str.Format("%ld",val);
	int len = str.GetLength();
	int ret = UpdateItemText(row_name, iSubItem, str.GetBuffer(len));
	str.ReleaseBuffer(len);
	return ret;
}
BOOL CListExCtrl::UpdateItemText(const CString& row_name, int iSubItem, UINT val) // Unsigned int.                     
{
	CString str;
	str.Format("%d",val);
	int len = str.GetLength();
	int ret = UpdateItemText(row_name, iSubItem, str.GetBuffer(len));
	str.ReleaseBuffer(len);
	return ret;
}
BOOL CListExCtrl::UpdateItemText(const CString& row_name, int iSubItem, INT val) // Int.                               
{
	CString str;
	str.Format("%d",val);
	int len = str.GetLength();
	int ret = UpdateItemText(row_name, iSubItem, str.GetBuffer(len));
	str.ReleaseBuffer(len);
	return ret;
}
BOOL CListExCtrl::UpdateItemText(const CString& row_name, int iSubItem, USHORT val) // Unsigned short.                 
{
	CString str;
	str.Format("%d",val);
	int len = str.GetLength();
	int ret = UpdateItemText(row_name, iSubItem, str.GetBuffer(len));
	str.ReleaseBuffer(len);
	return ret;
}
BOOL CListExCtrl::UpdateItemText(const CString& row_name, int iSubItem, SHORT val) // Short.                           
{
	CString str;
	str.Format("%d",val);
	int len = str.GetLength();
	int ret = UpdateItemText(row_name, iSubItem, str.GetBuffer(len));
	str.ReleaseBuffer(len);
	return ret;
}
BOOL CListExCtrl::UpdateItemText(const CString& row_name, int iSubItem, BYTE val) // Byte.                             
{
	CString str;
	str.Format("%d",val);
	int len = str.GetLength();
	int ret = UpdateItemText(row_name, iSubItem, str.GetBuffer(len));
	str.ReleaseBuffer(len);
	return ret;
}
BOOL CListExCtrl::UpdateItemText(const CString& row_name, int iSubItem, TCHAR val) // TCHAR.                           
{
	CString str;
	str.Format("%c",val);
	int len = str.GetLength();
	int ret = UpdateItemText(row_name, iSubItem, str.GetBuffer(len));
	str.ReleaseBuffer(len);
	return ret;
}

void CListExCtrl::SetRed(const CString &row_name, BOOL bRed)
{
		LISTITEM listItem;
	memset(&listItem.lvi, 0, sizeof(LVITEM));
	listItem.row_name = row_name;
	
	int ret = SendMessage(SETRED,(WPARAM)bRed, (LPARAM)&listItem);
	return;

	/*
	int len = GetItemCount();
	for (int i=0; i<len; i++)
	{
		CString str = GetItemText(i,0);
		if(str.Compare(row_name) == 0)
		{
			SetRed(i, bRed);
			return;
		}
	}
	*/
	
}

int CListExCtrl::GetRowNumber(int nColumn, const CString &szCell)
{
	int len = GetItemCount();
	for (int i=0; i<len; i++)
	{
		CString str = GetItemText(i,nColumn);
		if(str.Compare(szCell) == 0)
		{
			return i;
		}
	}
	return -1;
}

BOOL CListExCtrl::Lock()
{
//	EnterCriticalSection(&csLock);
	return SendMessage(LIST_LOCK);
}

BOOL CListExCtrl::UnLock()
{
	//LeaveCriticalSection(&csLock);
	return SendMessage(LIST_UNLOCK);
}

void CListExCtrl::GetSelectedItemText(CString &szSelectedStr, int nSubItem)
{
	SendMessage(GETSELECTEDITEMTEXT,(WPARAM)nSubItem,(LPARAM)&szSelectedStr);
}

LRESULT CListExCtrl::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	switch(message)
	{
	case GETFIRSTSELECTEDITEM:
		{
			int nStartAfter = (int)wParam;
			nStartAfter = max(nStartAfter, -1);
			for (int i = nStartAfter + 1; i < GetItemCount(); i++)
			{
				if (IsItemSelected(i))
					return i;
			}
			
			return -1;
		}
		break;
	case GETITEMTEXT:
		{
		}
		break;
	case UPDATEITEMTEXT:
		{
			pLISTITEM litem = (pLISTITEM)lParam;
			int len = GetItemCount();
			for (int i=0; i<len; i++)
			{
				CString str = GetItemText(i,0);
				if(str.Compare(litem->row_name ) == 0)
				{
					SetItemText(i, litem->lvi.iSubItem, litem->lvi.pszText);
					return i;
				}
			}
			int count = GetItemCount();
			InsertItem(count,litem->row_name,"Querying","","0","0","0.0","0","0","0","0","0","0","0");
			return count;

		}
		break;
	case SETRED:
		{
			BOOL bRed = (BOOL)wParam;
			
			pLISTITEM litem = (pLISTITEM)lParam;
			int len = GetItemCount();
			for (int i=0; i<len; i++)
			{
				CString str = GetItemText(i,0);
				if(str.Compare(litem->row_name ) == 0)
				{
					//SetItemText(i, litem->lvi.iSubItem, litem->lvi.pszText);
					SetRed(i,bRed);
					return i;
				}
			}
		}
		break;
	case SORT:
		{
			Sort(m_wndHeader.GetSortedColumn(), m_wndHeader.IsSortAscending());
			
		}
		break;
	case INSERTITEM:
		break;
	case GETITEMCOUNT:
		break;
	case SETITEMTEXT:
		break;
	case GETSELECTEDITEMTEXT:
		{
			CString *str = (CString *)lParam;
			int nSubItem = (int)wParam;
			if(str != NULL)
			{
				int nItem = GetFirstSelectedItem();
				*str = GetItemText(nItem,nSubItem);
			}
		}
		break;
	case LIST_LOCK:
		{
			int nCount = 0;
			while(nCount < 5)
			{
				if(TryEnterCriticalSection(&csLock) > 0)
				{
					return 1;
				}
				nCount++;
				Sleep(1000);
			}
			return 0;
		}
		break;
	case LIST_UNLOCK:
		LeaveCriticalSection(&csLock);
		return 1;
		break;
	}
	return CListCtrl::DefWindowProc(message, wParam, lParam);
}

