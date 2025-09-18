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
* $Log:   N:\PVCS\PTE\Equitable\ATP_gui\DeferredPropertySheet.h  $
   
      Rev 1.7   Dec 01 2003 11:29:16   lmitchel
   Added AMEX Online Updates - scr1200
   
      Rev 1.6   Jul 10 2002 09:56:06   lmitchel
   Ver:4.0.3.1:  Added SendMessage command in PropertySheet files (on_init function) to set the Apply button as the default.  If the enter key is pressed, action will default to  the apply button instead of the OK button.  This allows the user to remain in the current dialog instead of exiting to the main menu.
   
      Rev 1.5   Jan 05 2000 14:53:30   gbarnes
   Major update. Not updated to PVCS recently.
   PVCS was locked.
   
      Rev 1.1   Sep 22 1999 15:48:40   iarustam
   Added manual batch entry GUI
   
      Rev 1.0   Jul 01 1999 14:17:20   iarustam
   Initial Release
   
      Rev 1.0   Jun 28 1999 13:58:26   iarustam
   Initial Release
   
      Rev 1.2   May 10 1999 11:17:26   dperson
    
*   
************************************************************************************/
// DeferredPropertySheet.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDeferredPropertySheet
#include "DeferredDefinition.h"
#include "DeferredGeneral.h"
#include "ChangeHistory.h"


class CDeferredPropertySheet : public CPropertySheet
{
	DECLARE_DYNAMIC(CDeferredPropertySheet)

// Construction
public:
	CDeferredPropertySheet(UINT nIDCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
	CDeferredPropertySheet(LPCTSTR pszCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);

// Attributes
public:

	CDeferredGeneral m_GeneralPage;
	CDeferredDefinition m_DefinitionPage;
    CChangeHistory m_ChangeHistoryPage;

	void SetData();
	BOOL CDeferredPropertySheet::OnInitDialog();



// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDeferredPropertySheet)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CDeferredPropertySheet();

	// Generated message map functions
protected:
	//{{AFX_MSG(CDeferredPropertySheet)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
