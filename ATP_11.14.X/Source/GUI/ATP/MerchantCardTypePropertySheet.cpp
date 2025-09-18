// MerchantCardTypePropertySheet.cpp : implementation file
//

#include "stdafx.h"
#include "atp.h"
#include "MerchantCardTypePropertySheet.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMerchantCardTypePropertySheet

IMPLEMENT_DYNAMIC(CMerchantCardTypePropertySheet, CPropertySheet)

CMerchantCardTypePropertySheet::CMerchantCardTypePropertySheet(UINT nIDCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(nIDCaption, pParentWnd, iSelectPage)
{
}

CMerchantCardTypePropertySheet::CMerchantCardTypePropertySheet(LPCTSTR pszCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(pszCaption, pParentWnd, iSelectPage)
{
}

CMerchantCardTypePropertySheet::~CMerchantCardTypePropertySheet()
{
}


BEGIN_MESSAGE_MAP(CMerchantCardTypePropertySheet, CPropertySheet)
	//{{AFX_MSG_MAP(CMerchantCardTypePropertySheet)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMerchantCardTypePropertySheet message handlers
