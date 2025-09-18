/*********************************************************************************
* Module:
*
* Title:
*
* Description:
*
* Application:
*
* Author:
*
* Revision History:
*  
* $Log:   N:\PVCS\PTE\Equitable\ATP_gui\CardTypeDescription.cpp  $
   
      Rev 1.5   Dec 01 2003 11:29:00   lmitchel
   Added AMEX Online Updates - scr1200
   
      Rev 1.4   Jan 05 2000 14:53:06   gbarnes
   Major update. Not updated to PVCS recently.
   PVCS was locked.
   
      Rev 1.1   Sep 22 1999 15:45:02   iarustam
   Added manual batch entry GUI
   
      Rev 1.0   Jul 01 1999 14:17:02   iarustam
   Initial Release
   
      Rev 1.0   Jun 28 1999 13:58:10   iarustam
   Initial Release
   
      Rev 1.1   Apr 29 1999 14:56:02   iarustam
   Bug fixes
   
      Rev 1.0   Feb 23 1999 11:40:08   iarustam
   Initial Release
   
      Rev 1.0   Feb 17 1999 10:09:12   iarustam
   Initial Release
   
      Rev 1.1   Jan 22 1999 10:58:02   iarustam
   Bug fixes and core 1_3_0 changes
   
      Rev 1.1   Jan 21 1999 13:07:06   iarustam
   Bug fixes
   
   
      Rev 1.1   Dec 18 1998 13:42:40   iarustam
    
*
************************************************************************************/
// CardTypeDescription.cpp : implementation file
//
#include "stdafx.h"
#include "Atp.h"
#include "CardTypeDescription.h"
#include "CardTypePropertySheet.h"
#include "MyUtils.h"
//#include "htmlhelp.h"
#include "SharedMem.h"
extern "C" 
{
#include "PTETIME.h"
}

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
extern CSharedMem shrmem;
/////////////////////////////////////////////////////////////////////////////
// CCardTypeDescription property page

IMPLEMENT_DYNCREATE(CCardTypeDescription, CPropertyPage)

CCardTypeDescription::CCardTypeDescription() : CPropertyPage(CCardTypeDescription::IDD)
{
	//{{AFX_DATA_INIT(CCardTypeDescription)
	m_StrCardTypeFld = _T("");
	m_StrDescriptionFld = _T("");
	m_StrMessageLbl = _T("");
	//}}AFX_DATA_INIT
}

CCardTypeDescription::~CCardTypeDescription()
{
}

void CCardTypeDescription::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCardTypeDescription)
	DDX_Control(pDX, IDC_DESCRIPTION_FLD, m_CntlDescriptionFld);
	DDX_Text(pDX, IDC_CARD_TYPE_DESCRIPTION_FLD, m_StrCardTypeFld);
	DDX_Text(pDX, IDC_DESCRIPTION_FLD, m_StrDescriptionFld);
	DDV_MaxChars(pDX, m_StrDescriptionFld, 30);
	DDX_Text(pDX, IDC_MESSAGE_LBL, m_StrMessageLbl);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CCardTypeDescription, CPropertyPage)
	//{{AFX_MSG_MAP(CCardTypeDescription)
	ON_WM_HELPINFO()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCardTypeDescription message handlers
/******************************************************************************
 *
 * MODULE:	    OnInitDialog
 *
 * DESCRIPTION: This function performs special processing when 
 *              the dialog box is initialized.
 *
 * AUTHOR:      Narina Pogosyants
 *
 * INPUTS:      None
 *
 * OUTPUTS:     None
 *
 * RTRN VALUE:  Bool(true--exited successfully,false--not exited)
 *
 * MODIFICATIONS:
 *
 ******************************************************************************/
BOOL CCardTypeDescription::OnInitDialog() 
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

	if (m_nDialogMode != MODE_DONE )
    {
		if (m_nDialogMode != MODE_VIEW)
        {
			EnableCardDescriptionDialog(TRUE );
		}
        if (m_nDialogMode != MODE_ADD)
        {
            PopulateDialogFromDatabase();
		}
    } 

	m_bDescription = TRUE;

	m_StrMessageLbl = " ";	

	UpdateData(FALSE);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CCardTypeDescription::OnCommand(WPARAM wParam, LPARAM lParam) 
{
    if ((m_nDialogMode != MODE_VIEW) && (m_nDialogMode != MODE_DONE))
		SetModified (TRUE);
	
	c_time = CTime::GetCurrentTime();	
	shrmem.SetLastUpdateTime(c_time);
	return CPropertyPage::OnCommand(wParam, lParam);
}
/******************************************************************************
 *
 * MODULE:	    PopulateDialogFromDatabase
 *
 * DESCRIPTION: This function populates dialog.
 *
 * AUTHOR:      Narina Pogosyants
 *
 * INPUTS:      None             
 *
 * OUTPUTS:     None
 *
 * RTRN VALUE:  Bool(true--exited successfully,false--not exited)
 *
 * MODIFICATIONS:
 *
 ******************************************************************************/

BOOL CCardTypeDescription::PopulateDialogFromDatabase()
{
	if ((m_nDialogMode == MODE_EDIT) || (m_nDialogMode == MODE_VIEW))
	{
		EnableCardDescriptionDialog(TRUE );
    }
        	
	if (!UpdateData(FALSE))
	{
		AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
	}
	return TRUE;
}

/******************************************************************************
 *
 * MODULE:	    EnableCardDescriptionDialog
 *
 * DESCRIPTION: This function enables dialog by enabling all the controls.
 *
 * AUTHOR:      Narina Pogosyants
 *
 * INPUTS:      Bool(true--enable dialog,false--disable dialog)             
 *
 * OUTPUTS:     None
 *
 * RTRN VALUE:  None
 *
 * MODIFICATIONS:
 *
 ******************************************************************************/
void CCardTypeDescription::EnableCardDescriptionDialog(int nStatus)
{
}

/******************************************************************************
 *
 * MODULE:	    CleanCardDescriptionDialog
 *
 * DESCRIPTION: This function cleans dialog by cleaning all the strings.
 *
 * AUTHOR:      Narina Pogosyants
 *
 * INPUTS:      None             
 *
 * OUTPUTS:     None
 *
 * RTRN VALUE:  None
 *
 * MODIFICATIONS:
 *
 ******************************************************************************/
void CCardTypeDescription::CleanCardDescriptionDialog()
{
    m_StrCardTypeFld = "";
	m_StrDescriptionFld = "";
	m_StrMessageLbl = "";
	UpdateData(FALSE);
}

BOOL CCardTypeDescription::OnSetActive() 
{
	//((CCardTypePropertySheet*) this->GetParent())->m_nActiveTab = 1;
   if ( m_nDialogMode == MODE_EDIT || m_nDialogMode == MODE_ADD )
		m_CntlDescriptionFld.EnableWindow(TRUE);
   else if ( m_nDialogMode == MODE_VIEW || m_nDialogMode == MODE_DONE )
		m_CntlDescriptionFld.EnableWindow(FALSE);

	
	return CPropertyPage::OnSetActive();
}

BOOL CCardTypeDescription::OnHelpInfo(HELPINFO* pHelpInfo) 
{
/*	HtmlHelp (AfxGetMainWnd()->GetSafeHwnd(),
	"bin.chm>Main",HH_HELP_CONTEXT,IDD_CARD_TYPE_DESCRIPTION);
*/	
	return TRUE;
}

