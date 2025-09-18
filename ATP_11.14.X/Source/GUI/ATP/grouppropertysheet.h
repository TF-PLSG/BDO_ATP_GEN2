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
* $Log:   N:\PVCS\PTE\Equitable\ATP_gui\grouppropertysheet.h  $
   
      Rev 1.7   Dec 01 2003 11:29:24   lmitchel
   Added AMEX Online Updates - scr1200
   
      Rev 1.6   Jul 10 2002 09:56:12   lmitchel
   Ver:4.0.3.1:  Added SendMessage command in PropertySheet files (on_init function) to set the Apply button as the default.  If the enter key is pressed, action will default to  the apply button instead of the OK button.  This allows the user to remain in the current dialog instead of exiting to the main menu.
   
      Rev 1.5   Jan 05 2000 14:55:02   gbarnes
   Major update. Not updated to PVCS recently.
   PVCS was locked.
   
      Rev 1.0   Jun 28 1999 13:59:54   iarustam
   Initial Release
   
      Rev 1.1   Apr 29 1999 14:57:20   iarustam
   Bug fixes
   
      Rev 1.0   Feb 23 1999 11:41:36   iarustam
   Initial Release
   
      Rev 1.0   Feb 17 1999 10:10:20   iarustam
   Initial Release
   
      Rev 1.1   Jan 22 1999 11:00:38   iarustam
   Bug fixes and core 1_3_0 changes
   
      Rev 1.1   Jan 21 1999 13:10:26   iarustam
   Bug fixes
   
   
      Rev 1.1   Dec 18 1998 13:45:18   iarustam
    
*
************************************************************************************/
// GroupPropertySheet.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CGroupPropertySheet
#include "GroupAccess.h"
#include "GroupWorkstation.h"
#include "ChangeHistory.h"



#include "GroupDefinition.h"


class CGroupPropertySheet : public CPropertySheet
{
	DECLARE_DYNAMIC(CGroupPropertySheet)

// Construction
public:
	CGroupPropertySheet(UINT nIDCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
	CGroupPropertySheet(LPCTSTR pszCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
	void SetData();

    

// Attributes
public:
	CChangeHistory	    m_ChangeHistoryPage;
	CGroupDefinition    m_DefinitionPage;
    CGroupAccess        m_AccessPage;
    CGroupWorkstation   m_WorkstationPage;

	int m_nActiveTab;

   

    
// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGroupPropertySheet)
	//}}AFX_VIRTUAL
	BOOL CGroupPropertySheet::OnInitDialog();


// Implementation
public:
	virtual ~CGroupPropertySheet();

	// Generated message map functions
protected:
	//{{AFX_MSG(CGroupPropertySheet)
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
