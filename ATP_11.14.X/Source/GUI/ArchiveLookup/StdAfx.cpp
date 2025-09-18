// stdafx.cpp : source file that includes just the standard includes
//	ArchiveLookup.pch will be the pre-compiled header
//	stdafx.obj will contain the pre-compiled type information

#include "stdafx.h"
#include "resource.h"

void UpdateTitleWithVersion(CDialog *pDlg)
{
	//set version info in the title bar
	CString appProdVer;
	CString lpszTitle;
	appProdVer.LoadString(IDS_PRODUCT_VERSION);
	pDlg->GetWindowText(lpszTitle);
	lpszTitle = lpszTitle + " - Version - ";
	lpszTitle += appProdVer;
	pDlg->SetWindowText(lpszTitle);
}

