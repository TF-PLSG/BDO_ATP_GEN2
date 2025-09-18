#include "stdafx.h"
#include "atp.h"
#include "BlockingPosEntryModesGeneralPage.h"
#include "BlockingPosEntryModesSheet.h"
#include "SharedMem.h"
extern "C" {
#include "PTETIME.h"
}

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


extern CSharedMem shrmem;
/////////////////////////////////////////////////////////////////////////////
// CBinGeneral property page

IMPLEMENT_DYNCREATE(CBlockingPosEntryModesGeneralPage, CPropertyPage)

CBlockingPosEntryModesGeneralPage::CBlockingPosEntryModesGeneralPage() : CPropertyPage(CBlockingPosEntryModesGeneralPage::IDD)
{
}

CBlockingPosEntryModesGeneralPage::~CBlockingPosEntryModesGeneralPage()
{
	
}
void CBlockingPosEntryModesGeneralPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CBlockingPosEntryModesGeneralPage, CPropertyPage)
	//{{AFX_MSG_MAP(CBinMCCBlocking)

	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL CBlockingPosEntryModesGeneralPage::OnInitDialog() 
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

   // m_bMCCBlocking = TRUE;	
	
    if ( m_nDialogMode == MODE_VIEW || m_nDialogMode == MODE_EDIT )
      // PopulateBinDialogFromDatabase ( sBinRecord );	   

	if (!UpdateData(FALSE)){
		AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
	}	
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


BOOL CBlockingPosEntryModesGeneralPage::OnSetActive() 
{
	return CPropertyPage::OnSetActive();
}