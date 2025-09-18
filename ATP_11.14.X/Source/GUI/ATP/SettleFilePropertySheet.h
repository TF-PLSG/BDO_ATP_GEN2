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
* $Log:   N:\PVCS\PTE\Equitable\ATP_gui\SettleFilePropertySheet.h  $
   
      Rev 1.5   Dec 01 2003 11:30:14   lmitchel
   Added AMEX Online Updates - scr1200
   
      Rev 1.4   Jan 05 2000 14:54:24   gbarnes
   Major update. Not updated to PVCS recently.
   PVCS was locked.
   
      Rev 1.1   Sep 22 1999 16:00:18   iarustam
   Added manual batch entry GUI
   
      Rev 1.0   Jul 01 1999 14:18:20   iarustam
   Initial Release
   
      Rev 1.0   Jun 28 1999 13:59:28   iarustam
   Initial Release
   
      Rev 1.1   Apr 29 1999 14:57:04   iarustam
   Bug fixes
   
      Rev 1.0   Feb 23 1999 11:41:18   iarustam
   Initial Release
   
      Rev 1.0   Feb 17 1999 10:10:08   iarustam
   Initial Release
   
      Rev 1.1   Jan 22 1999 11:00:14   iarustam
   Bug fixes and core 1_3_0 changes
   
      Rev 1.1   Jan 21 1999 13:09:54   iarustam
   Bug fixes
   
   
      Rev 1.1   Dec 18 1998 13:44:50   iarustam
    
*
************************************************************************************/
// SettleFilePropertySheet.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSettleFilePropertySheet
#include "SettleFileDefinition.h"
#include "SettlePrefixTime.h"
#include "ChangeHistory.h"

class CSettleFilePropertySheet : public CPropertySheet
{
	DECLARE_DYNAMIC(CSettleFilePropertySheet)

// Construction
public:
	CSettleFilePropertySheet(UINT nIDCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
	CSettleFilePropertySheet(LPCTSTR pszCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);

// Attributes
public:
	CSettleFileDefinition m_DefinitionPage;
	CSettlePrefixTime m_PrefixTimePage;
	CChangeHistory m_ChangeHistoryPage;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSettleFilePropertySheet)
	public:
	virtual BOOL OnInitDialog();
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CSettleFilePropertySheet();

	// Generated message map functions
protected:
	//{{AFX_MSG(CSettleFilePropertySheet)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
