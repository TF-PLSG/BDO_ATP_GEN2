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
* $Log:   N:\PVCS\PTE\Equitable\ATP_gui\CardTypePropertySheet.h  $
   
      Rev 1.6   Dec 01 2003 11:29:00   lmitchel
   Added AMEX Online Updates - scr1200
   
      Rev 1.5   Jul 10 2002 09:55:54   lmitchel
   Ver:4.0.3.1:  Added SendMessage command in PropertySheet files (on_init function) to set the Apply button as the default.  If the enter key is pressed, action will default to  the apply button instead of the OK button.  This allows the user to remain in the current dialog instead of exiting to the main menu.
   
      Rev 1.4   Jan 05 2000 14:53:08   gbarnes
   Major update. Not updated to PVCS recently.
   PVCS was locked.
   
      Rev 1.1   Sep 22 1999 15:45:12   iarustam
   Added manual batch entry GUI
   
      Rev 1.0   Jul 01 1999 14:17:04   iarustam
   Initial Release
   
      Rev 1.0   Jun 28 1999 13:58:10   iarustam
   Initial Release
   
      Rev 1.1   Apr 29 1999 14:56:02   iarustam
   Bug fixes
   
      Rev 1.0   Feb 23 1999 11:40:10   iarustam
   Initial Release
   
      Rev 1.0   Feb 17 1999 10:09:12   iarustam
   Initial Release
   
      Rev 1.1   Jan 22 1999 10:58:06   iarustam
   Bug fixes and core 1_3_0 changes
   
      Rev 1.1   Jan 21 1999 13:07:08   iarustam
   Bug fixes
   
   
      Rev 1.1   Dec 18 1998 13:42:44   iarustam
    
*
************************************************************************************/
// CardTypePropertySheet.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CCardTypePropertySheet
#include "CardDefinition.h"
#include "CardTypeDescription.h"
#include "ChangeHistory.h"


class CCardTypePropertySheet : public CPropertySheet
{
	DECLARE_DYNAMIC(CCardTypePropertySheet)

// Construction
public:
	CCardTypePropertySheet(UINT nIDCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
	CCardTypePropertySheet(LPCTSTR pszCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);

// Attributes
public:

	CCardDefinition m_DefinitionPage;
	CCardTypeDescription m_DescriptionPage;
	CChangeHistory m_ChangeHistoryPage;

	int m_nActiveTab;

// Operations
public:

	void SetData();
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCardTypePropertySheet)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CCardTypePropertySheet();

	// Generated message map functions
protected:
	//{{AFX_MSG(CCardTypePropertySheet)
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	BOOL CCardTypePropertySheet::OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
