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
* $Log:   N:\PVCS\PTE\Equitable\ATP_gui\PurchaseCardDefinition.h  $
   
      Rev 1.6   Dec 01 2003 11:30:04   lmitchel
   Added AMEX Online Updates - scr1200
   
      Rev 1.5   Jan 05 2000 14:54:16   gbarnes
   Major update. Not updated to PVCS recently.
   PVCS was locked.
   
      Rev 1.1   Sep 22 1999 15:58:34   iarustam
   Added manual batch entry GUI
   
      Rev 1.0   Jul 01 1999 14:18:10   iarustam
   Initial Release
   
      Rev 1.0   Jun 28 1999 13:59:16   iarustam
   Initial Release
   
      Rev 1.2   May 10 1999 11:33:30   dperson
   Added PVCS header
*   
************************************************************************************/
// PurchaseCardDefinition.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CPurchaseCardDefinition dialog
#include "PurchaseCardGeneral.h"
#include "ChangeHistory.h"

class CPurchaseCardDefinition : public CPropertyPage
{
	DECLARE_DYNCREATE(CPurchaseCardDefinition)

// Construction
public:
	CPurchaseCardDefinition();
	~CPurchaseCardDefinition();

	int m_nDialogMode;
	CPurchaseCardGeneral *pGeneralPage;
	CChangeHistory *pChangeHistoryPage;

	void PopulateHistoryTable();
	BOOL CheckDialogForCompleteness();
	BOOL PopulatePurchaseDatabaseStructureFromDialog ( pCCF04 );
	void PopulatePurchaseDialogFromDatabase( CCF04 );

	char m_strCurrentData[151];
	char m_strPreviousData[151];
	char m_strFieldTag[50];
	char strFilePrimaryKey[50];

	CTime c_time;
// Dialog Data
	//{{AFX_DATA(CPurchaseCardDefinition)
	enum { IDD = IDD_PURCHASE_CARD_DEFINITION };
	CEdit	m_CntlCardNumberFld;
	CComboBox	m_CntlCardTypeFld;
	CButton	m_UpdateButton;
	CButton	m_ViewButton;
	CButton	m_ListButton;
	CButton	m_DeleteButton;
	CButton	m_AddButton;
	CString	m_StrCardNumberFld;
	CString	m_StrMessageLbl;
	CString	m_StrCardTypeFld;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CPurchaseCardDefinition)
	public:
	virtual BOOL OnApply();
	virtual void OnCancel();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CPurchaseCardDefinition)
	virtual BOOL OnInitDialog();
	afx_msg void OnAddButton();
	afx_msg void OnDeleteButton();
	afx_msg void OnListButton();
	afx_msg void OnUpdateButton();
	afx_msg void OnViewButton();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};
