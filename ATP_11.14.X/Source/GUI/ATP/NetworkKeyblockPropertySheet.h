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
* $Log:   N:\PVCS\PTE\Equitable\ATP_gui\NetworkAcquirerPropertySheet.h  $
   
      Rev 1.6   Dec 01 2003 11:29:54   lmitchel
   Added AMEX Online Updates - scr1200
   
      Rev 1.5   Jul 10 2002 09:56:34   lmitchel
   Ver:4.0.3.1:  Added SendMessage command in PropertySheet files (on_init function) to set the Apply button as the default.  If the enter key is pressed, action will default to  the apply button instead of the OK button.  This allows the user to remain in the current dialog instead of exiting to the main menu.
   
      Rev 1.4   Jan 05 2000 14:54:08   gbarnes
   Major update. Not updated to PVCS recently.
   PVCS was locked.
   
      Rev 1.1   Sep 22 1999 15:56:36   iarustam
   Added manual batch entry GUI
   
      Rev 1.0   Jul 01 1999 14:18:00   iarustam
   Initial Release
   
      Rev 1.0   Jun 28 1999 13:59:08   iarustam
   Initial Release
   
      Rev 1.1   Apr 29 1999 14:56:50   iarustam
   Bug fixes
   
      Rev 1.0   Feb 23 1999 11:40:56   iarustam
   Initial Release
   
      Rev 1.0   Feb 17 1999 10:09:50   iarustam
   Initial Release
   
      Rev 1.1   Jan 22 1999 10:59:44   iarustam
   Bug fixes and core 1_3_0 changes
   
      Rev 1.1   Jan 21 1999 13:09:18   iarustam
   Bug fixes
   
   
      Rev 1.1   Dec 18 1998 13:44:20   iarustam
    
*
************************************************************************************/
// NetworkKeyblockPropertySheet.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CNetworkKeyblockPropertySheet

#include "NetworkKeyblockDefinition.h"
#include "NetworkKeyblockGeneral.h"

#include "ChangeHistory.h"


class CNetworkKeyblockPropertySheet : public CPropertySheet
{
	DECLARE_DYNAMIC(CNetworkKeyblockPropertySheet)

// Construction
public:
	CNetworkKeyblockPropertySheet(UINT nIDCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
	CNetworkKeyblockPropertySheet(LPCTSTR pszCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);

// Attributes
public:

	int m_nNetworkMode;

	CNetworkKeyblockDefinition m_DefinitionPage;
	CNetworkKeyblockGeneral m_GeneralPage;

	CChangeHistory m_ChangeHistoryPage;

// Operations
public:
	void SetData();
	BOOL CNetworkKeyblockPropertySheet::OnInitDialog();


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CNetworkKeyblockPropertySheet)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CNetworkKeyblockPropertySheet();

	// Generated message map functions
protected:
	//{{AFX_MSG(CNetworkKeyblockPropertySheet)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
