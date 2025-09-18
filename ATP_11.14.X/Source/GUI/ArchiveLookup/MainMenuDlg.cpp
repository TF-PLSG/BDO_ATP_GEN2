// MainMenuDlg.cpp : implementation file
//

#include "stdafx.h"
#include "archivelookup.h"
#include "MainMenuDlg.h"
#include "ArchiveLookupDlg.h"
#include "BatchLookup.h"
#include "BCH11Lookup.h"
#include "BCH20Lookup.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMainMenuDlg dialog


CMainMenuDlg::CMainMenuDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CMainMenuDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CMainMenuDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CMainMenuDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMainMenuDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CMainMenuDlg, CDialog)
	//{{AFX_MSG_MAP(CMainMenuDlg)
	ON_BN_CLICKED(IDC_TLF01MENU, OnTlf01menu)
	ON_BN_CLICKED(IDC_BCH01MENU, OnBch01menu)
	ON_BN_CLICKED(IDC_BCH11MENU, OnBch11menu)
	ON_BN_CLICKED(IDC_BCH20MENU, OnBch20menu)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMainMenuDlg message handlers

void CMainMenuDlg::OnTlf01menu() 
{
	CArchiveLookupDlg TLF01Dlg;
	TLF01Dlg.DoModal();
}

void CMainMenuDlg::OnBch01menu() 
{
	CBatchLookup BCH01Dlg;
	BCH01Dlg.DoModal();
}

void CMainMenuDlg::OnBch11menu() 
{
	CBCH11Lookup BCH11Dlg;
	BCH11Dlg.DoModal();
}

void CMainMenuDlg::OnBch20menu() 
{
	CBCH20Lookup BCH20Dlg;
	BCH20Dlg.DoModal();
}
