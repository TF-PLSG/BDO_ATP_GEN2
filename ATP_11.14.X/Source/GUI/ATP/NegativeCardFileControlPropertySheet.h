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
* $Log:   N:\PVCS\PTE\Equitable\ATP_gui\NegativeCardFileControlPropertySheet.h  $
   
      Rev 1.6   Dec 01 2003 11:29:50   lmitchel
   Added AMEX Online Updates - scr1200
   
      Rev 1.5   Jan 05 2000 14:54:06   gbarnes
   Major update. Not updated to PVCS recently.
   PVCS was locked.
   
      Rev 1.1   Sep 22 1999 15:56:02   iarustam
   Added manual batch entry GUI
   
      Rev 1.0   Jul 01 1999 14:17:58   iarustam
   Initial Release
   
      Rev 1.0   Jun 28 1999 13:59:04   iarustam
   Initial Release
   
      Rev 1.2   May 10 1999 11:30:04   dperson
   Added PVCS header
*   
************************************************************************************/
// NegativeCardFileControlPropertySheet.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CNegativeCardFileControlPropertySheet
#include "NegativeCardFileControl.h"
#include "ChangeHistory.h"

class CNegativeCardFileControlPropertySheet : public CPropertySheet
{
	DECLARE_DYNAMIC(CNegativeCardFileControlPropertySheet)

// Construction
public:
	CNegativeCardFileControlPropertySheet(UINT nIDCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
	CNegativeCardFileControlPropertySheet(LPCTSTR pszCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);

// Attributes
public:

	CNegativeCardFileControl m_DefinitionPage;
	CChangeHistory m_ChangeHistoryPage;

// Operations
public:

	void SetData();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CNegativeCardFileControlPropertySheet)
	public:
	virtual BOOL OnInitDialog();
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CNegativeCardFileControlPropertySheet();

	// Generated message map functions
protected:
	//{{AFX_MSG(CNegativeCardFileControlPropertySheet)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
