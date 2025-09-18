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
*     
*
************************************************************************************/
// TransactionDetailsPropertySheet.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CTransactionDetailsPropertySheet



#include "Fraud_Txn_Details.h"
#include "transactiondetails.h"
#include "Addnl_Txn_Details.h"
#include "Visa_EDQP_Txn_Details.h"

/*#include "Visa_EDQP_Txn_Details.h"
#include "Addnl_Txn_Details.h"*/


class CTransactionDetailsPropertySheet : public CPropertySheet
{
	DECLARE_DYNAMIC(CTransactionDetailsPropertySheet)

// Construction
public:
	CTransactionDetailsPropertySheet(UINT nIDCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
	CTransactionDetailsPropertySheet(LPCTSTR pszCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);

// Attributes
public:

	//CTransactionDetails			m_TranDetailsPage;
    //CFraudTransactionDetails    m_FraudDetailsPage;
	/*CAddlFldsTransactionDetails m_AddlTranDetailsPage;
	CVisaEDQPTransactionDetails m_VEDQPTranDetailsPage;*/


// Operations
public:

	void SetData();
	BOOL CTransactionDetailsPropertySheet::OnInitDialog();


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMerchantTerminalPropertySheet)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CTransactionDetailsPropertySheet();

	// Generated message map functions
protected:
	//{{AFX_MSG(CMerchantTerminalPropertySheet)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
