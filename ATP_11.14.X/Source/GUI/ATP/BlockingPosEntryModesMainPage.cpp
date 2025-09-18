

#include "stdafx.h"
#include "Atp.h"
#include "NameList.h"
#include "MyUtils.h"
#include "SharedMem.h"
#include "BlockingPosEntryModesSheet.h"
#include "BlockingPosEntryModesMainPage.h"

extern "C"
{
#include "PteTime.h"
}
extern CSharedMem shrmem;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNCREATE(CBlockingPosEntryModesMainPage, CPropertyPage)

CBlockingPosEntryModesMainPage::CBlockingPosEntryModesMainPage() : CPropertyPage(CBlockingPosEntryModesMainPage::IDD)
{
}

CBlockingPosEntryModesMainPage::~CBlockingPosEntryModesMainPage()
{
	//delete pLock;
	//pLock = NULL;
}

void CBlockingPosEntryModesMainPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);

}

BEGIN_MESSAGE_MAP(CBlockingPosEntryModesMainPage, CPropertyPage)
	//{{AFX_MSG_MAP(CBinDefinition)
	ON_BN_CLICKED(IDC_ADD_BLOCKPOS_BUTTON, OnAddPOSButton)
	ON_BN_CLICKED(IDC_DELETE_BLOCKPOS_BUTTON, OnDeletePOSButton)
	ON_BN_CLICKED(IDC_UPDATE_BLOCKPOS_BUTTON, OnUpdatePOSButton)
	ON_BN_CLICKED(IDC_VIEW_BLOCKPOS_BUTTON, OnViewPOSButton)
	ON_BN_CLICKED(IDC_LIST_BLOCKPOS_BUTTON, OnListPOSButton)
	ON_WM_HELPINFO()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL CBlockingPosEntryModesMainPage::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
		
	CFont m_font;
	LOGFONT lf;

	memset ( &lf, 0, sizeof (lf));
	lf.lfHeight = 12;
	lf.lfWeight = 700;
	strcpy(lf.lfFaceName, "Arial");
	m_font.CreateFontIndirect (&lf);
	GetDlgItem(IDC_MESSAGE_LBL)->SetFont (&m_font);

	UpdateData(FALSE);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
void CBlockingPosEntryModesMainPage::OnAddPOSButton() 
{

}

void CBlockingPosEntryModesMainPage::OnDeletePOSButton() 
{

}

void CBlockingPosEntryModesMainPage::OnViewPOSButton() 
{

}


void CBlockingPosEntryModesMainPage::OnListPOSButton() 
{

}

void CBlockingPosEntryModesMainPage::OnUpdatePOSButton() 
{

}
BOOL CBlockingPosEntryModesMainPage::OnApply() 
{
 
	return true;
}

void CBlockingPosEntryModesMainPage::OnCancel() 
{
}

BOOL CBlockingPosEntryModesMainPage::OnHelpInfo(HELPINFO* pHelpInfo) 
{
	


	return TRUE;
}

BOOL CBlockingPosEntryModesMainPage::OnSetActive() 
{
	((CBlockingPosEntryModePropertySheet*) this->GetParent())->m_nActiveTab = 0;
	
	return CPropertyPage::OnSetActive();
}

BOOL CBlockingPosEntryModesMainPage::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	// TODO: Add your specialized code here and/or call the base class
	c_time = CTime::GetCurrentTime();	
	shrmem.SetLastUpdateTime(c_time);
	return CPropertyPage::OnCommand(wParam, lParam);
}
