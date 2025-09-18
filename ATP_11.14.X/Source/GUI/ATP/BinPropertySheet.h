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
* $Log:   N:\POS\PVCS6.6\EPICPORTZ\PTE\Equitable\ATP_gui\BinPropertySheet.h  $
   
      Rev 1.9   Apr 01 2005 10:13:30   lmitchel
   Initial Checking prior to integration testing for updates made to support country/curreny code limits (scr13929), unulimited purchase card MCC ranges, and purchase card transaction limits(scr12785)
   
      Rev 1.8   Dec 01 2003 11:28:44   lmitchel
   Added AMEX Online Updates - scr1200
   
      Rev 1.7   Jul 10 2002 09:55:44   lmitchel
   Ver:4.0.3.1:  Added SendMessage command in PropertySheet files (on_init function) to set the Apply button as the default.  If the enter key is pressed, action will default to  the apply button instead of the OK button.  This allows the user to remain in the current dialog instead of exiting to the main menu.
   
      Rev 1.6   Oct 16 2001 15:20:26   dirby
   Added blocking of a bin by Merchant Category Code (MCC)     SCR 361
   
      Rev 1.5   Apr 17 2000 09:56:54   iarustam
   added bin blocking
   
   
      Rev 1.4   Jan 05 2000 14:52:54   gbarnes
   Major update. Not updated to PVCS recently.
   PVCS was locked.
   
      Rev 1.1   Sep 22 1999 15:41:16   iarustam
   Added manual batch entry GUI
   
      Rev 1.0   Jul 01 1999 14:16:50   iarustam
   Initial Release
   
      Rev 1.0   Jun 28 1999 13:57:54   iarustam
   Initial Release
   
      Rev 1.1   Apr 29 1999 14:55:48   iarustam
   Bug fixes
   
      Rev 1.0   Feb 23 1999 11:39:56   iarustam
   Initial Release
   
      Rev 1.0   Feb 17 1999 10:08:58   iarustam
   Initial Release
   
      Rev 1.1   Jan 22 1999 10:57:32   iarustam
   Bug fixes and core 1_3_0 changes
   
      Rev 1.1   Jan 21 1999 13:06:16   iarustam
   Bug fixes
   
   
      Rev 1.1   Dec 18 1998 13:42:14   iarustam
    
*
************************************************************************************/
// BinPropertySheet.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CBinPropertySheet
#include "BinGeneral.h"
#include "ChangeHistory.h"
#include "BinDefinition.h"
#include "BinBlocking.h"
#include "BinMCCBlocking.h"
#include "BinMidBlocking.h"


class CBinPropertySheet : public CPropertySheet
{
	DECLARE_DYNAMIC(CBinPropertySheet)

// Construction
public:
	CBinPropertySheet(UINT nIDCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
	CBinPropertySheet(LPCTSTR pszCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);

    

// Attributes
public:
	CBinDefinition		m_DefinitionPage;
    CBinGeneral			m_GeneralPage;
    CBinBlocking		m_BlockingPage;  
	CBinMCCBlocking		m_MCCBlockingPage;
    CChangeHistory		m_ChangeHistoryPage;
	CBinMidBlocking     m_MidBlockingPage; 
	int m_nActiveTab;

// Operations
public:

    void SetData();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBinPropertySheet)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CBinPropertySheet();

	// Generated message map functions
protected:
	//{{AFX_MSG(CBinPropertySheet)
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	BOOL CBinPropertySheet::OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
