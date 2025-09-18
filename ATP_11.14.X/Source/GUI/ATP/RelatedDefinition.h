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
* $Log:   N:\PVCS\PTE\Equitable\ATP_gui\RelatedDefinition.h  $
   
      Rev 1.7   Dec 01 2003 11:30:08   lmitchel
   Added AMEX Online Updates - scr1200
   
      Rev 1.6   Jan 05 2000 14:54:18   gbarnes
   Major update. Not updated to PVCS recently.
   PVCS was locked.
   
      Rev 1.1   Sep 22 1999 15:59:08   iarustam
   Added manual batch entry GUI
   
      Rev 1.0   Jul 01 1999 14:18:12   iarustam
   Initial Release
   
      Rev 1.0   Jun 28 1999 13:59:20   iarustam
   Initial Release
   
      Rev 1.3   May 10 1999 11:35:16   dperson
   Added PVCS header
*   
************************************************************************************/
// RelatedDefinition.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CRelatedDefinition dialog
#include "RelatedGeneral.h"
#include "ChangeHistory.h"
#define  IDT_TIMER  WM_USER + 200
class CRelatedDefinition : public CPropertyPage
{
	DECLARE_DYNCREATE(CRelatedDefinition)

// Construction
public:
	CRelatedDefinition();
	~CRelatedDefinition();

	int m_nDialogMode;
	CRelatedGeneral *pGeneralPage;
	CChangeHistory *pChangeHistoryPage;

	void PopulateHistoryTable();
	BOOL CheckDialogForCompleteness();
	BOOL PopulateRelatedDatabaseStructureFromDialog ( pACF01 );
	void PopulateRelatedDialogFromDatabase( ACF01 );

	char m_strCurrentData[151];
	char m_strPreviousData[151];
	char m_strFieldTag[50];
	char strFilePrimaryKey[50];


	CTime c_time;
// Dialog Data
	//{{AFX_DATA(CRelatedDefinition)
	enum { IDD = IDD_RELATED_DEFINITION };
	CButton	m_ListButton;
	CButton	m_ViewButton;
	CButton	m_UpdateButton;
	CButton	m_DeleteButton;
	CButton	m_AddButton;
	CComboBox	m_CntlAccountTypeFld;
	CEdit	m_CntlAccountNumberFld;
	CString	m_StrAccountNumberFld;
	CString	m_StrAccountTypeFld;
	CString	m_StrMessageLbl;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CRelatedDefinition)
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
	//{{AFX_MSG(CRelatedDefinition)
	virtual BOOL OnInitDialog();
	afx_msg void OnAddButton();
	afx_msg void OnDeleteButton();
	afx_msg void OnUpdateButton();
	afx_msg void OnViewButton();
	afx_msg void OnListButton();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};
