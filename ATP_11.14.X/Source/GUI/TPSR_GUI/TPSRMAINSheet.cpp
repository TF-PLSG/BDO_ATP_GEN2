// TPSRMAINSheet.cpp : implementation file
//

#include "stdafx.h"
#include "TPSR_GUI.h"
#include "TPSRMAINSheet.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTPSRMAINSHEET

IMPLEMENT_DYNAMIC(CTPSRMAINSHEET, CPropertySheet)

CTPSRMAINSHEET::CTPSRMAINSHEET(UINT nIDCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(nIDCaption, pParentWnd, iSelectPage)
{
}

CTPSRMAINSHEET::CTPSRMAINSHEET(LPCTSTR pszCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(pszCaption, pParentWnd, iSelectPage)
{
	AddPage(&m_TpsrMainDlg);
	AddPage(&m_CardAndTrackDataDlg);
//	AddPage(&m_TpsrConfigDlg);
//	AddPage(&m_CardMerDevDlg);
//	AddPage(&m_PosConfigDlg);
//	AddPage(&m_TestProfileDlg);
	m_pMainMenu = new CMenu();
	m_pMainMenu->LoadMenu(MAKEINTRESOURCE(IDR_MENU1));

}

CTPSRMAINSHEET::~CTPSRMAINSHEET()
{
	delete m_pMainMenu;
}


BEGIN_MESSAGE_MAP(CTPSRMAINSHEET, CPropertySheet)
	//{{AFX_MSG_MAP(CTPSRMAINSHEET)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTPSRMAINSHEET message handlers
BOOL CTPSRMAINSHEET::OnInitDialog()
{
	BOOL bResult = CPropertySheet::OnInitDialog();
	int headerHeight = 0; //Height of the menu
	if(IsWindow(this->m_hWnd))
	{
//		this->SetMenu(m_pMainMenu);
//		DrawMenuBar();
		CRect rectWnd;
		GetWindowRect (rectWnd);
		ScreenToClient (rectWnd);
		SetWindowPos (NULL, 0, 0, rectWnd.Width(), 
			rectWnd.Height() + headerHeight,
			SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE);
	}
	HWND hWnd = ::GetDlgItem(m_hWnd, IDHELP);
    if (hWnd != NULL)
    {
        ::ShowWindow (hWnd, SW_HIDE);
        ::EnableWindow (hWnd, TRUE);
    }
	hWnd = ::GetDlgItem(m_hWnd, ID_APPLY_NOW);
    if (hWnd != NULL)
    {
        ::ShowWindow (hWnd, SW_HIDE);
        ::EnableWindow (hWnd, TRUE);
    }
		hWnd = ::GetDlgItem(m_hWnd, IDOK);
    if (hWnd != NULL)
    {
		::ShowWindow (hWnd, SW_HIDE);
        ::EnableWindow (hWnd, TRUE);
    }
		hWnd = ::GetDlgItem(m_hWnd, IDCANCEL);
    if (hWnd != NULL)
    {
		CString title = _T("Exit");
		::SetWindowText(hWnd, title);
		::ShowWindow (hWnd, SW_SHOW);
        ::EnableWindow (hWnd, TRUE);
    }

	return bResult;
}

LRESULT CTPSRMAINSHEET::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	int received=0;
	int sent=0;
	char strReceived[20] = {0};
	
	switch(message)
	{
	case WM_REPONSERECEIVED:
		
		received = GetDlgItemInt(IDC_RESRECV);
		received++;
		SetDlgItemInt(IDC_RESRECV,received);
		break;
	case WM_TRANSACTIONSENT:

		sent = GetDlgItemInt(IDC_TXNSENT);
		sent++;
		SetDlgItemInt(IDC_TXNSENT,sent);
		break;
	default:
		break;
	}		
	return CPropertySheet::DefWindowProc(message, wParam, lParam);
}
