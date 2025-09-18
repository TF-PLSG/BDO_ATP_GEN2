// PosConfig.cpp : implementation file
//

#include "stdafx.h"
#include "TPSR_GUI.h"
#include "PosConfig.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPosConfig property page

IMPLEMENT_DYNCREATE(CPosConfig, CPropertyPage)

CPosConfig::CPosConfig() : CPropertyPage(CPosConfig::IDD)
{
	//{{AFX_DATA_INIT(CPosConfig)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

CPosConfig::~CPosConfig()
{
}

void CPosConfig::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPosConfig)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPosConfig, CPropertyPage)
	//{{AFX_MSG_MAP(CPosConfig)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPosConfig message handlers

BOOL CPosConfig::OnApply() 
{

	
	return CPropertyPage::OnApply();
}
