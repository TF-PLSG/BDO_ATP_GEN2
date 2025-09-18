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
* $Log:   N:\PVCS\PTE\Equitable\ATP_gui\groupdefinition.h  $
   
      Rev 1.7   Dec 01 2003 11:29:22   lmitchel
   Added AMEX Online Updates - scr1200
   
      Rev 1.6   Jan 25 2000 14:01:08   iarustam
   bug fixes
   
      Rev 1.5   Jan 05 2000 14:55:02   gbarnes
   Major update. Not updated to PVCS recently.
   PVCS was locked.
   
      Rev 1.0   Jun 28 1999 13:59:52   iarustam
   Initial Release
   
      Rev 1.1   Apr 29 1999 14:57:20   iarustam
   Bug fixes
   
      Rev 1.0   Feb 23 1999 11:41:36   iarustam
   Initial Release
   
      Rev 1.0   Feb 17 1999 10:10:20   iarustam
   Initial Release
   
      Rev 1.1   Jan 22 1999 11:00:38   iarustam
   Bug fixes and core 1_3_0 changes
   
      Rev 1.1   Jan 21 1999 13:10:26   iarustam
   Bug fixes
   
   
      Rev 1.2   Jan 13 1999 14:21:40   iarustam
    
   
      Rev 1.1   Dec 18 1998 13:45:18   iarustam
    
*
************************************************************************************/
// GroupDefinition.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CGroupDefinition dialog
#ifndef GROUP_DEFINITION
#define GROUP_DEFINITION

#include "ChangeHistory.h"
#include "GroupAccess.h"
#include "GroupWorkstation.h"
#include "SharedMem.h"

extern CSharedMem shrmem;
class CGroupDefinition : public CPropertyPage
{
	DECLARE_DYNCREATE(CGroupDefinition)

// Construction
public:
	CGroupDefinition();
	~CGroupDefinition();

	BOOL CheckDialogForCompleteness();
	BOOL PopulateGroupDialogFromDatabase ( UCF02 );
	BOOL PopulateGroupDatabaseStructureFromDialog( pUCF02);

	int m_nDialogMode;
	char m_strCurrentData[151];
	void PopulateHistoryTable();

	CChangeHistory* pChangeHistoryPage;
    CGroupAccess* pAccessPage;
    CGroupWorkstation* pWorkstationPage;

    UCF02 sOldUCF02Record;

	
	CTime c_time;

// Dialog Data
	//{{AFX_DATA(CGroupDefinition)
	enum { IDD = IDD_GROUP_DEFINITION };
	CButton	m_ViewButton;
	CButton	m_UpdateButton;
	CButton	m_ListButton;
	CButton	m_DeleteButton;
	CButton	m_AddButton;
	CEdit	m_CntlGroupIDFld;
	CString	m_StrGroupIDFld;
	CString	m_StrMessageLbl;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CGroupDefinition)
	public:
	virtual BOOL OnApply();
	virtual BOOL OnSetActive();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CGroupDefinition)
	afx_msg void OnUserAddButton();
	afx_msg void OnUserDeleteButton();
	afx_msg void OnUserListButton();
	afx_msg void OnUserUpdateButton();
	afx_msg void OnUserViewButton();
	virtual BOOL OnInitDialog();
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};
#endif
