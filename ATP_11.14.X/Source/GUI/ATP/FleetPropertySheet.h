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
* $Log:   N:\POS\PVCS6.6\EPICPORTZ\PTE\Equitable\ATP_gui\FleetPropertySheet.h  $
   
      Rev 1.7   Apr 01 2005 10:13:48   lmitchel
   Initial Checking prior to integration testing for updates made to support country/curreny code limits (scr13929), unulimited purchase card MCC ranges, and purchase card transaction limits(scr12785)
   
      Rev 1.6   Dec 01 2003 11:29:20   lmitchel
   Added AMEX Online Updates - scr1200
   
      Rev 1.5   Jul 10 2002 09:56:10   lmitchel
   Ver:4.0.3.1:  Added SendMessage command in PropertySheet files (on_init function) to set the Apply button as the default.  If the enter key is pressed, action will default to  the apply button instead of the OK button.  This allows the user to remain in the current dialog instead of exiting to the main menu.
   
      Rev 1.4   Jan 05 2000 14:53:32   gbarnes
   Major update. Not updated to PVCS recently.
   PVCS was locked.
   
      Rev 1.1   Sep 22 1999 15:49:42   iarustam
   Added manual batch entry GUI
   
      Rev 1.0   Jul 01 1999 14:17:28   iarustam
   Initial Release
   
      Rev 1.0   Jun 28 1999 13:58:32   iarustam
   Initial Release
   
      Rev 1.1   Apr 29 1999 14:56:22   iarustam
   Bug fixes
   
      Rev 1.0   Feb 23 1999 11:40:26   iarustam
   Initial Release
   
      Rev 1.0   Feb 17 1999 10:09:26   iarustam
   Initial Release
   
      Rev 1.1   Jan 22 1999 10:58:44   iarustam
   Bug fixes and core 1_3_0 changes
   
      Rev 1.1   Jan 21 1999 13:08:00   iarustam
   Bug fixes
   
   
      Rev 1.1   Dec 18 1998 13:43:22   iarustam
    
*
************************************************************************************/
// FleetPropertySheet.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CFleetPropertySheet
#include "CardsMerchantLimit.h"
#include "CardsProcessing.h"
#include "CardsTime1.h"
#include "CardsTime2.h"
#include "CardsTime3a.h"
#include "CardsFleetDefinition.h"
#include "CardsVelocity.h"
#include "ChangeHistory.h"
#include "CardsProfile.h"

class CFleetPropertySheet : public CPropertySheet
{
	DECLARE_DYNAMIC(CFleetPropertySheet)

// Construction
public:
	CFleetPropertySheet(UINT nIDCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
	CFleetPropertySheet(LPCTSTR pszCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);

// Attributes
public:
	CCardsMerchantLimit m_MerchantLimitPage;
	CCardsProcessing m_ProcessingPage;
	CCardsTime1 m_Time1Page;
	CCardsTime2 m_Time2Page;
	CCardsTime3a m_Time3Page;
	CCntryAmtLimits m_CntryLimitPage;
	CCardsFleetDefinition m_DefinitionPage;
    CCardsVelocity m_VelocityPage;
	CChangeHistory m_ChangeHistoryPage;
	CCardsProfile m_CardsProfilePage;
public:


// Operations       
public:
    void SetData();
	BOOL CFleetPropertySheet::OnInitDialog();


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFleetPropertySheet)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CFleetPropertySheet();

	// Generated message map functions
protected:
	//{{AFX_MSG(CFleetPropertySheet)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
