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
* $Log:   N:\PVCS\PTE\Equitable\ATP_gui\DeferredDefinition.h  $
   
      Rev 1.6   Dec 01 2003 11:29:14   lmitchel
   Added AMEX Online Updates - scr1200
   
      Rev 1.5   Jan 05 2000 14:53:28   gbarnes
   Major update. Not updated to PVCS recently.
   PVCS was locked.
   
      Rev 1.1   Sep 22 1999 15:48:20   iarustam
   Added manual batch entry GUI
   
      Rev 1.0   Jul 01 1999 14:17:18   iarustam
   Initial Release
   
      Rev 1.0   Jun 28 1999 13:58:26   iarustam
   Initial Release
   
      Rev 1.2   May 10 1999 11:13:26   dperson
   Added PVCS header
*   
************************************************************************************/
// DeferredDefinition.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDeferredDefinition dialog
#include "DeferredGeneral.h"
#include "ChangeHistory.h"


class CDeferredDefinition : public CPropertyPage
{
	DECLARE_DYNCREATE(CDeferredDefinition)

// Construction
public:
	CDeferredDefinition();
	~CDeferredDefinition();

	int m_nDialogMode;
	CDeferredGeneral *pGeneralPage;
	CChangeHistory *pChangeHistoryPage;

	void PopulateHistoryTable();
	BOOL CheckDialogForCompleteness();
	BOOL PopulateDeferredDatabaseStructureFromDialog ( pDEF01 );
	void PopulateDeferredDialogFromDatabase( DEF01 );

	char m_strCurrentData[151];
	char m_strPreviousData[151];
	char m_strFieldTag[50];
	char strFilePrimaryKey[50];
	CTime c_time;


// Dialog Data
	//{{AFX_DATA(CDeferredDefinition)
	enum { IDD = IDD_DEFERRED_DEFINITION };
	CButton	m_ViewButton;
	CButton	m_UpdateButton;
	CEdit	m_CntlTermLengthFld;
	CEdit	m_CntlOrganizationIDFld;
	CEdit	m_CntlMerchantIDFld;
	CButton	m_ListButton;
	CButton	m_DeleteButton;
	CButton	m_AddButton;
	CString	m_StrMerchantIDFld;
	CString	m_StrMessageLbl;
	CString	m_StrOrganizationIDFld;
	CString	m_StrTermLengthFld;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CDeferredDefinition)
	public:
	virtual BOOL OnApply();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CDeferredDefinition)
	virtual BOOL OnInitDialog();
	afx_msg void OnAddButton();
	afx_msg void OnDeleteButton();
	afx_msg void OnListButton();
	afx_msg void OnUpdateButton();
	afx_msg void OnViewButton();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};
