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
* $Log:   N:\PVCS\PTE\Equitable\ATP_gui\CFSHEET.H  $
   
      Rev 1.6   Dec 01 2003 11:29:08   lmitchel
   Added AMEX Online Updates - scr1200
   
      Rev 1.5   Jul 10 2002 09:56:00   lmitchel
   Ver:4.0.3.1:  Added SendMessage command in PropertySheet files (on_init function) to set the Apply button as the default.  If the enter key is pressed, action will default to  the apply button instead of the OK button.  This allows the user to remain in the current dialog instead of exiting to the main menu.
   
      Rev 1.4   Jan 05 2000 14:53:00   gbarnes
   Major update. Not updated to PVCS recently.
   PVCS was locked.
   
      Rev 1.1   Sep 22 1999 15:43:12   iarustam
   Added manual batch entry GUI
   
      Rev 1.0   Jul 01 1999 14:16:56   iarustam
   Initial Release
   
      Rev 1.0   Jun 28 1999 13:58:00   iarustam
   Initial Release
   
      Rev 1.1   Apr 29 1999 14:55:54   iarustam
   Bug fixes
   
      Rev 1.0   Feb 23 1999 11:40:02   iarustam
   Initial Release
   
      Rev 1.0   Feb 17 1999 10:09:04   iarustam
   Initial Release
   
      Rev 1.1   Jan 22 1999 10:57:50   iarustam
   Bug fixes and core 1_3_0 changes
   
      Rev 1.1   Jan 21 1999 13:06:48   iarustam
   Bug fixes
   
   
      Rev 1.1   Dec 18 1998 13:41:52   iarustam
    
*
************************************************************************************/
// CFSheet.h : header file
//
#include "CFMain.h"
#include "CFGeneral.h"
#include "CFCardBalAcc.h"
#include "CFCapability.h"
#include "CFDemogr.h"
#include "CFActivity.h"
#include "CFMerchUsage.h"
#include "CFTxHistory.h"
#include "ChangeHistory.h"

/////////////////////////////////////////////////////////////////////////////
// CCFSheet

class CCFSheet : public CPropertySheet
{
	DECLARE_DYNAMIC(CCFSheet)

// Construction
public:
	CCFSheet(UINT nIDCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
	CCFSheet(LPCTSTR pszCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);

// Attributes
public:

	CCFMain	MainPage;
	CCFGeneral GenPage;
	CCFCardBalAcc BalPage;
	CCFCapability CapPage;
	CCFDemogr DemogrPage;
	CCFActivity ActivPage;
	CCFMerchUsage MerchUsPage;
	CCFTxHistory TxPage;
	CChangeHistory  ChHistPage;

void SetData();

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCFSheet)
	//}}AFX_VIRTUAL
	BOOL CCFSheet::OnInitDialog();

// Implementation
public:
	virtual ~CCFSheet();

	// Generated message map functions
protected:
	//{{AFX_MSG(CCFSheet)
		// NOTE - the ClassWizard will add and remove member functions here.


	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
