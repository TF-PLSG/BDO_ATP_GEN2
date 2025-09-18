// MerchantCardTypeDefinition.cpp : implementation file
//

#include "stdafx.h"
#include "atp.h"
#include "MerchantCardTypeDefinition.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMerchantCardTypeDefinition property page

IMPLEMENT_DYNCREATE(CMerchantCardTypeDefinition, CPropertyPage)

CMerchantCardTypeDefinition::CMerchantCardTypeDefinition() : CPropertyPage(CMerchantCardTypeDefinition::IDD)
{
	//{{AFX_DATA_INIT(CMerchantCardTypeDefinition)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

CMerchantCardTypeDefinition::~CMerchantCardTypeDefinition()
{
}

void CMerchantCardTypeDefinition::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMerchantCardTypeDefinition)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CMerchantCardTypeDefinition, CPropertyPage)
	//{{AFX_MSG_MAP(CMerchantCardTypeDefinition)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMerchantCardTypeDefinition message handlers
