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
* $Log:   N:\PVCS\PTE\Equitable\ATP_gui\WorkstationPropertySheet.h  $
   
      Rev 1.5   Dec 01 2003 11:30:28   lmitchel
   Added AMEX Online Updates - scr1200
   
      Rev 1.4   Jan 05 2000 14:54:36   gbarnes
   Major update. Not updated to PVCS recently.
   PVCS was locked.
   
      Rev 1.1   Sep 22 1999 16:03:20   iarustam
   Added manual batch entry GUI
   
      Rev 1.0   Jul 01 1999 14:18:34   iarustam
   Initial Release
   
      Rev 1.0   Jun 28 1999 13:59:44   iarustam
   Initial Release
   
      Rev 1.1   Apr 29 1999 14:57:18   iarustam
   Bug fixes
   
      Rev 1.0   Feb 23 1999 11:41:32   iarustam
   Initial Release
   
      Rev 1.0   Feb 17 1999 10:10:18   iarustam
   Initial Release
   
      Rev 1.1   Jan 22 1999 11:00:36   iarustam
   Bug fixes and core 1_3_0 changes
   
      Rev 1.1   Jan 21 1999 13:10:22   iarustam
   Bug fixes
   
   
      Rev 1.1   Dec 18 1998 13:45:14   iarustam
    
*
************************************************************************************/
// WorkstationPropertySheet.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CWorkstationPropertySheet
#include "WorkstationDefinition.h"
#include "WorkstationDescription.h"
#include "ChangeHistory.h"

class CWorkstationPropertySheet : public CPropertySheet
{
	DECLARE_DYNAMIC(CWorkstationPropertySheet)

// Construction
public:
	CWorkstationPropertySheet(UINT nIDCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
	CWorkstationPropertySheet(LPCTSTR pszCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);

// Attributes
public:

   CWorkstationDefinition m_WorkstationDefinition;
   CWorkstationDescription m_WorkstationDescription;
   CChangeHistory m_ChangeHistoryPage;

// Operations
public:
   void SetData();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWorkstationPropertySheet)
	public:
	virtual BOOL OnInitDialog();
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CWorkstationPropertySheet();

	// Generated message map functions
protected:
	//{{AFX_MSG(CWorkstationPropertySheet)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
