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
* $Log:   N:\PVCS\PTE\Equitable\ATP_gui\MerchantTerminalPropertySheet.h  $
   
      Rev 1.7   Jul 16 2004 16:07:42   lmitchel
   SCR1287 - Added new fields to support Auto Init and Reload Mechants;   A new Auto Init tab and support fields were added to the Device Definition GUI.  A new reload flag was added to the Merchant Definition General GUI.  A new credti subtype = PREPAID was added to the BIN Definition GUI.
   
      Rev 1.6   Dec 01 2003 11:29:46   lmitchel
   Added AMEX Online Updates - scr1200
   
      Rev 1.5   Jul 10 2002 09:56:28   lmitchel
   Ver:4.0.3.1:  Added SendMessage command in PropertySheet files (on_init function) to set the Apply button as the default.  If the enter key is pressed, action will default to  the apply button instead of the OK button.  This allows the user to remain in the current dialog instead of exiting to the main menu.
   
      Rev 1.4   Jan 05 2000 14:54:04   gbarnes
   Major update. Not updated to PVCS recently.
   PVCS was locked.
   
      Rev 1.1   Sep 22 1999 15:55:02   iarustam
   Added manual batch entry GUI
   
      Rev 1.0   Jul 01 1999 14:17:54   iarustam
   Initial Release
   
      Rev 1.0   Jun 28 1999 13:59:00   iarustam
   Initial Release
   
      Rev 1.1   Apr 29 1999 14:56:44   iarustam
   Bug fixes
   
      Rev 1.0   Feb 23 1999 11:40:48   iarustam
   Initial Release
   
      Rev 1.0   Feb 17 1999 10:09:44   iarustam
   Initial Release
   
      Rev 1.1   Jan 22 1999 10:59:30   iarustam
   Bug fixes and core 1_3_0 changes
   
      Rev 1.1   Jan 21 1999 13:08:58   iarustam
   Bug fixes
   
   
      Rev 1.1   Dec 18 1998 13:44:06   iarustam
    
*
************************************************************************************/
// MerchantTerminalPropertySheet.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CMerchantTerminalPropertySheet
#ifndef TERMINALSHEET
#define TERMINALSHEET


#include "MerchantTerminalCurrentBatch.h"
#include "MerchantTerminalCUPCurrentBatch.h"
#include "MerchantTerminalGeneral.h"
#include "MerchantTerminalKeys.h"
#include "TerminalValidTransactions.h"
//#include "TerminalCapabilities.h"
#include "MerchantTerminalBatchHistory.h"
#include "MerchantTerminalConfig.h"
#include "ChangeHistory.h"
#include "TerminalAutoInit.h"


class CMerchantTerminalPropertySheet : public CPropertySheet
{
	DECLARE_DYNAMIC(CMerchantTerminalPropertySheet)

// Construction
public:
	CMerchantTerminalPropertySheet(UINT nIDCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
	CMerchantTerminalPropertySheet(LPCTSTR pszCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);

// Attributes
public:

    CMerchantTerminalGeneral        m_GeneralPage;
    CMerchantTerminalCurrentBatch   m_CurrentBatchPage;  
	CMerchantTerminalCUPCurrentBatch   m_CurrentCUPBatchPage;  
    CMerchantTerminalBatchHistory   m_BatchHistoryPage;
    CTerminalValidTransactions      m_TranPage;   
    CMerchantTerminalKeys           m_KeysPage;     
   // CTerminalCapabilities           m_CapabilitiesPage;
	CMerchantTerminalConfig         m_DefinitionPage;
	CTerminalAutoInit			    m_AutoInitPage;
	CChangeHistory					m_ChangeHistoryPage;

// Operations
public:

	void SetData();
	BOOL CMerchantTerminalPropertySheet::OnInitDialog();


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMerchantTerminalPropertySheet)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CMerchantTerminalPropertySheet();

	// Generated message map functions
protected:
	//{{AFX_MSG(CMerchantTerminalPropertySheet)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
#endif