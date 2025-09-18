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
* $Log:   
*
************************************************************************************/
// DebitPropertySheet.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDebitPropertySheet
#include "CardsMerchantLimit.h"
#include "CardsProcessing.h"
#include "CardsTime1.h"
#include "CardsTime2.h"
#include "CardsTime3a.h"
#include "CardsFleetDefinition.h"
#include "ChangeHistory.h"
#include "CardsProfile.h"

class CDebitPropertySheet : public CPropertySheet
{
	DECLARE_DYNAMIC(CDebitPropertySheet)

// Construction
public:
	CDebitPropertySheet(UINT nIDCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
	CDebitPropertySheet(LPCTSTR pszCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);

// Attributes
public:

	int m_nCardMode;
    CCardsMerchantLimit m_MerchantLimitPage;
	CCardsProcessing m_ProcessingPage;
	CCardsTime1 m_Time1Page;
	CCardsTime2 m_Time2Page;
	CCardsTime3a m_Time3Page;
	CCntryAmtLimits m_CntryLimitPage;
	CCardsFleetDefinition m_DefinitionPage;    
	CChangeHistory m_ChangeHistoryPage;
	CCardsProfile m_CardsProfilePage;

// Operations
public:
	void SetData();
	BOOL CDebitPropertySheet::OnInitDialog();


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDebitPropertySheet)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CDebitPropertySheet();

	// Generated message map functions
protected:
	//{{AFX_MSG(CDebitPropertySheet)

	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
