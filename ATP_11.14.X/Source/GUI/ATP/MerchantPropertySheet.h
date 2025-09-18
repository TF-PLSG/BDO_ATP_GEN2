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
* $Log:   N:\PVCS\PTE\Equitable\ATP_gui\MerchantPropertySheet.h  $
   
      Rev 1.8   Dec 01 2003 11:29:40   lmitchel
   Added AMEX Online Updates - scr1200
   
      Rev 1.7   Jul 10 2002 09:56:24   lmitchel
   Ver:4.0.3.1:  Added SendMessage command in PropertySheet files (on_init function) to set the Apply button as the default.  If the enter key is pressed, action will default to  the apply button instead of the OK button.  This allows the user to remain in the current dialog instead of exiting to the main menu.
   
      Rev 1.6   Jan 05 2000 14:54:00   gbarnes
   Major update. Not updated to PVCS recently.
   PVCS was locked.
   
      Rev 1.1   Sep 22 1999 15:53:32   iarustam
   Added manual batch entry GUI
   
      Rev 1.0   Jul 01 1999 14:17:46   iarustam
   Initial Release
   
      Rev 1.0   Jun 28 1999 13:58:52   iarustam
   Initial Release
   
      Rev 1.3   Apr 29 1999 14:56:36   iarustam
   Bug fixes
   
      Rev 1.2   Mar 04 1999 13:32:12   iarustam
   Bug fixes
   
      Rev 1.0   Feb 23 1999 11:40:42   iarustam
   Initial Release
   
      Rev 1.0   Feb 17 1999 10:09:38   iarustam
   Initial Release
   
      Rev 1.1   Jan 22 1999 10:59:16   iarustam
   Bug fixes and core 1_3_0 changes
   
      Rev 1.1   Jan 21 1999 13:08:36   iarustam
   Bug fixes
   
   
      Rev 1.1   Dec 18 1998 13:43:46   iarustam
    
*
************************************************************************************/
// MerchantPropertySheet.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CMerchantPropertySheet
#include "MerchantBlocking.h"
#include "MerchantCapabilities.h"
#include "MerchantContact.h"
#include "MerchantLocation.h"
#include "MerchantSettlement.h"
#include "MerchantVelocity.h"
#include "MerchantGeneral.h"
#include "MerchantDefinition.h"
#include "ChangeHistory.h"


class CMerchantPropertySheet : public CPropertySheet
{
	DECLARE_DYNAMIC(CMerchantPropertySheet)

// Construction
public:
	CMerchantPropertySheet(UINT nIDCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
	CMerchantPropertySheet(LPCTSTR pszCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);

// Attributes
public:   
    CMerchantLocation           m_LocationPage;
    CMerchantCapabilities       m_CapabilitiesPage;
    CMerchantSettlement         m_SettlementPage;
    CMerchantVelocity           m_VelocityPage;    
    CMerchantContact            m_ContactPage;
    CMerchantBlocking           m_BlockingPage;
    CMerchantGeneral			m_GeneralPage;
	CMerchantDefinition			m_DefinitionPage;
	CChangeHistory				m_ChangeHistoryPage;

// Operations
public:

	void SetData();	
	BOOL CMerchantPropertySheet::OnInitDialog();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMerchantPropertySheet)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CMerchantPropertySheet();

	// Generated message map functions
protected:
	//{{AFX_MSG(CMerchantPropertySheet)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
