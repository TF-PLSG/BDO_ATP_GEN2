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
* $Log:   N:\PVCS\PTE\Equitable\ATP_gui\PurchaseCardPropertySheet.h  $
   
      Rev 1.7   Dec 01 2003 11:30:06   lmitchel
   Added AMEX Online Updates - scr1200
   
      Rev 1.6   Jul 10 2002 09:56:44   lmitchel
   Ver:4.0.3.1:  Added SendMessage command in PropertySheet files (on_init function) to set the Apply button as the default.  If the enter key is pressed, action will default to  the apply button instead of the OK button.  This allows the user to remain in the current dialog instead of exiting to the main menu.
   
      Rev 1.5   Jan 05 2000 14:54:16   gbarnes
   Major update. Not updated to PVCS recently.
   PVCS was locked.
   
      Rev 1.1   Sep 22 1999 15:58:52   iarustam
   Added manual batch entry GUI
   
      Rev 1.0   Jul 01 1999 14:18:12   iarustam
   Initial Release
   
      Rev 1.0   Jun 28 1999 13:59:18   iarustam
   Initial Release
   
      Rev 1.2   May 10 1999 11:34:08   dperson
    
*   
************************************************************************************/
// PurchaseCardPropertySheet.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CPurchaseCardPropertySheet
#include "PurchaseCardDefinition.h"
#include "PurchaseCardGeneral.h"
#include "ChangeHistory.h"


class CPurchaseCardPropertySheet : public CPropertySheet
{
	DECLARE_DYNAMIC(CPurchaseCardPropertySheet)

// Construction
public:
	CPurchaseCardPropertySheet(UINT nIDCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
	CPurchaseCardPropertySheet(LPCTSTR pszCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);

// Attributes
public:
	CPurchaseCardGeneral m_GeneralPage;
	CPurchaseCardDefinition m_DefinitionPage;
    CChangeHistory m_ChangeHistoryPage;

// Operations
public:

		void SetData();
		BOOL CPurchaseCardPropertySheet::OnInitDialog();


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPurchaseCardPropertySheet)
	protected:
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CPurchaseCardPropertySheet();

	// Generated message map functions
protected:
	//{{AFX_MSG(CPurchaseCardPropertySheet)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
