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
* $Log:   N:\PVCS\PTE\Equitable\ATP_gui\NegativeFileDefinition.h  $
   
      Rev 1.5   Dec 01 2003 11:29:52   lmitchel
   Added AMEX Online Updates - scr1200
   
      Rev 1.4   Jan 05 2000 14:54:08   gbarnes
   Major update. Not updated to PVCS recently.
   PVCS was locked.
   
      Rev 1.1   Sep 22 1999 15:56:12   iarustam
   Added manual batch entry GUI
   
      Rev 1.0   Jul 01 1999 14:17:58   iarustam
   Initial Release
   
      Rev 1.0   Jun 28 1999 13:59:04   iarustam
   Initial Release
   
      Rev 1.1   Apr 29 1999 14:56:48   iarustam
   Bug fixes
   
      Rev 1.0   Feb 23 1999 11:40:54   iarustam
   Initial Release
   
      Rev 1.0   Feb 17 1999 10:09:48   iarustam
   Initial Release
   
      Rev 1.1   Jan 22 1999 10:59:40   iarustam
   Bug fixes and core 1_3_0 changes
   
      Rev 1.1   Jan 21 1999 13:09:12   iarustam
   Bug fixes
   
   
      Rev 1.1   Dec 18 1998 13:44:16   iarustam
    
*
************************************************************************************/
// NegativeFileDefinition.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CNegativeFileDefinition dialog
#include "NegativeGroupInfo.h"
#include "ChangeHistory.h"

class CNegativeFileDefinition : public CPropertyPage
{
	DECLARE_DYNCREATE(CNegativeFileDefinition)

// Construction
public:
	CNegativeFileDefinition();
	~CNegativeFileDefinition();

	void PopulateNegGroupDialogFromDatabase ( NGF01 );
	BOOL PopulateNegGroupDatabaseStructureFromDialog ( pNGF01 );
	BOOL CheckDialogForCompleteness();
	void PopulateHistoryTable();

	int m_nDialogMode;
	char m_strCurrentData[151];
	char m_strPreviousData[151];
    char m_strFieldTag[50];
	CNegativeGroupInfo* pGeneralPage;
	CChangeHistory* pChangeHistoryPage;
	CTime c_time;



// Dialog Data
	//{{AFX_DATA(CNegativeFileDefinition)
	enum { IDD = IDD_NEG_GROUP_DEFINITION };
	CButton	m_ViewButton;
	CButton	m_UpdateButton;
	CEdit	m_CntlNegGroupIDFld;
	CButton	m_ListButton;
	CButton	m_DeleteButton;
	CButton	m_AddButton;
	CString	m_StrMessageLbl;
	CString	m_StrNegGroupIDFld;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CNegativeFileDefinition)
	public:
	virtual void OnCancel();
	virtual BOOL OnApply();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CNegativeFileDefinition)
	virtual BOOL OnInitDialog();
	afx_msg void OnAddButton();
	afx_msg void OnDeleteButton();
	afx_msg void OnListButton();
	afx_msg void OnUpdateButton();
	afx_msg void OnViewButton();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};
