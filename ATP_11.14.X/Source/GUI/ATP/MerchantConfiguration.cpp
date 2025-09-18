// MerchantConfiguration.cpp : implementation file
//

#include "stdafx.h"
#include "Atp.h"
#include "MerchantConfiguration.h"
#include "MerchantPropertySheet.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CMerchantPropertySheet* m_MerchantSheet;

/////////////////////////////////////////////////////////////////////////////
// CMerchantConfiguration dialog


CMerchantConfiguration::CMerchantConfiguration(CWnd* pParent /*=NULL*/)
	: CDialog(CMerchantConfiguration::IDD, pParent)
{
    m_MerchantSheet = new CMerchantPropertySheet (" ");
	//{{AFX_DATA_INIT(CMerchantConfiguration)
	m_StrMerchantIDFld = _T("");
	//}}AFX_DATA_INIT
}


void CMerchantConfiguration::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMerchantConfiguration)
	DDX_Control(pDX, IDC_MERCHANT_ID_FLD, m_CntlMerchantIDFld);
	DDX_Text(pDX, IDC_MERCHANT_ID_FLD, m_StrMerchantIDFld);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CMerchantConfiguration, CDialog)
	//{{AFX_MSG_MAP(CMerchantConfiguration)
	ON_BN_CLICKED(IDC_MERCHANT_ADD_BUTTON, OnMerchantAddButton)
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMerchantConfiguration message handlers

BOOL CMerchantConfiguration::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CMerchantConfiguration::OnMerchantAddButton() 
{
	m_MerchantSheet->SetTitle("Add Merchant");
    m_MerchantSheet->DoModal();
	
}

void CMerchantConfiguration::OnDestroy() 
{
	CDialog::OnDestroy();
	
	if (m_MerchantSheet != NULL)
        delete m_MerchantSheet;
	
}
