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
* $Log:   N:\PVCS\PTE\Equitable\ATP_gui\DialogPropertySheet.h  $
   
      Rev 1.5   Dec 01 2003 11:29:18   lmitchel
   Added AMEX Online Updates - scr1200
   
      Rev 1.4   Jan 05 2000 14:53:32   gbarnes
   Major update. Not updated to PVCS recently.
   PVCS was locked.
   
      Rev 1.1   Sep 22 1999 15:49:14   iarustam
   Added manual batch entry GUI
   
      Rev 1.0   Jul 01 1999 14:17:24   iarustam
   Initial Release
   
      Rev 1.0   Jun 28 1999 13:58:30   iarustam
   Initial Release
   
      Rev 1.1   Apr 29 1999 14:56:20   iarustam
   Bug fixes
   
      Rev 1.0   Feb 23 1999 11:40:22   iarustam
   Initial Release
   
      Rev 1.0   Feb 17 1999 10:09:24   iarustam
   Initial Release
   
      Rev 1.1   Jan 22 1999 10:58:40   iarustam
   Bug fixes and core 1_3_0 changes
   
      Rev 1.1   Jan 21 1999 13:07:54   iarustam
   Bug fixes
   
   
      Rev 1.1   Dec 18 1998 13:43:18   iarustam
    
*
************************************************************************************/
// DialogPropertySheet.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDialogPropertySheet
#include "DialogCommSetup.h"
#include "DialogMain.h"
#include "DialogConfig.h"

class CDialogPropertySheet : public CPropertySheet
{
	DECLARE_DYNAMIC(CDialogPropertySheet)

// Construction
public:
	CDialogPropertySheet(UINT nIDCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
	CDialogPropertySheet(LPCTSTR pszCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);

// Attributes
public:

   CDialogCommSetup m_DialogCommSetup;
   CDialogMain m_DialogMain;
   CDialogConfig m_DialogConfig;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDialogPropertySheet)
	public:
	virtual BOOL OnInitDialog();
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CDialogPropertySheet();

	// Generated message map functions
protected:
	//{{AFX_MSG(CDialogPropertySheet)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
