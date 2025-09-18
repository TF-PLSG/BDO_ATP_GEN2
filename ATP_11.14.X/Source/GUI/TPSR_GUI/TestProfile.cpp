// TestProfile.cpp : implementation file
//

#include "stdafx.h"
#include "TPSR_GUI.h"
#include "TestProfile.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTestProfile dialog

IMPLEMENT_DYNCREATE(CTestProfile, CPropertyPage)

CTestProfile::CTestProfile(CWnd* pParent /*=NULL*/)
	: CPropertyPage(CTestProfile::IDD)
{
	//{{AFX_DATA_INIT(CTestProfile)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CTestProfile::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTestProfile)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CTestProfile, CPropertyPage)
	//{{AFX_MSG_MAP(CTestProfile)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTestProfile message handlers

BOOL CTestProfile::OnApply() 
{

	
	return CPropertyPage::OnApply();
}
