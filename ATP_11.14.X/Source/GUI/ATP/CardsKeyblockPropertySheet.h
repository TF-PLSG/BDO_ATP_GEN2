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
************************************************************************************/
// CCardsKeyblockPropertySheet.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CCardsKeyblockPropertySheet

#include "CardsKeyblockProcessing.h"
#include "CardsKeyblockDefinition.h"
#include "ChangeHistory.h"
/*#include "CardsProfile.h"*/

class CCardsKeyblockPropertySheet : public CPropertySheet
{
	DECLARE_DYNAMIC(CCardsKeyblockPropertySheet)

// Construction
public:
	CCardsKeyblockPropertySheet(UINT nIDCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
	CCardsKeyblockPropertySheet(LPCTSTR pszCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);

// Attributes
public:
	CCardsKeyblockProcessing m_ProcessingPage;
	CCardsKeyblockDefinition m_DefinitionPage; 
	CChangeHistory m_ChangeHistoryPage;
	int m_nCardMode;

// Operations
public:
    void SetData();
	BOOL CCardsKeyblockPropertySheet::OnInitDialog();


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCardsKeyblockPropertySheet)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CCardsKeyblockPropertySheet();

	// Generated message map functions
protected:
	//{{AFX_MSG(CCardsKeyblockPropertySheet)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

