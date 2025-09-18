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
* $Log:   N:\PVCS\PTE\Equitable\ATP_gui\RelatedPropertySheet.h  $
   
      Rev 1.8   Dec 01 2003 11:30:08   lmitchel
   Added AMEX Online Updates - scr1200
   
      Rev 1.7   Jul 10 2002 09:56:46   lmitchel
   Ver:4.0.3.1:  Added SendMessage command in PropertySheet files (on_init function) to set the Apply button as the default.  If the enter key is pressed, action will default to  the apply button instead of the OK button.  This allows the user to remain in the current dialog instead of exiting to the main menu.
   
      Rev 1.6   Jan 05 2000 14:54:18   gbarnes
   Major update. Not updated to PVCS recently.
   PVCS was locked.
   
      Rev 1.1   Sep 22 1999 15:59:16   iarustam
   Added manual batch entry GUI
   
      Rev 1.0   Jul 01 1999 14:18:14   iarustam
   Initial Release
   
      Rev 1.0   Jun 28 1999 13:59:20   iarustam
   Initial Release
   
      Rev 1.3   May 10 1999 13:40:38   dperson
   Updated comments
   
      Rev 1.2   May 10 1999 11:37:14   dperson
	Added PVCS header
    
*   
************************************************************************************/
// RelatedPropertySheet.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CRelatedPropertySheet
#include "RelatedGeneral.h"
#include "RelatedDefinition.h"
#include "ChangeHistory.h"


class CRelatedPropertySheet : public CPropertySheet
{
	DECLARE_DYNAMIC(CRelatedPropertySheet)

// Construction
public:
	CRelatedPropertySheet(UINT nIDCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
	CRelatedPropertySheet(LPCTSTR pszCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);

// Attributes
public:

	CRelatedGeneral m_GeneralPage;
	CRelatedDefinition m_DefinitionPage;
    CChangeHistory m_ChangeHistoryPage;

	void SetData();
	BOOL CRelatedPropertySheet::OnInitDialog();


// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRelatedPropertySheet)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CRelatedPropertySheet();

	// Generated message map functions
protected:
	//{{AFX_MSG(CRelatedPropertySheet)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
