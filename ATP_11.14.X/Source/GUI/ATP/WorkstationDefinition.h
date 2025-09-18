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
* $Log:   N:\PVCS\PTE\Equitable\ATP_gui\WorkstationDefinition.h  $
   
      Rev 1.5   Dec 01 2003 11:30:26   lmitchel
   Added AMEX Online Updates - scr1200
   
      Rev 1.4   Jan 05 2000 14:54:34   gbarnes
   Major update. Not updated to PVCS recently.
   PVCS was locked.
   
      Rev 1.1   Sep 22 1999 16:03:00   iarustam
   Added manual batch entry GUI
   
      Rev 1.0   Jul 01 1999 14:18:32   iarustam
   Initial Release
   
      Rev 1.0   Jun 28 1999 13:59:42   iarustam
   Initial Release
   
      Rev 1.1   Apr 29 1999 14:57:16   iarustam
   Bug fixes
   
      Rev 1.0   Feb 23 1999 11:41:30   iarustam
   Initial Release
   
      Rev 1.0   Feb 17 1999 10:10:16   iarustam
   Initial Release
   
      Rev 1.1   Jan 22 1999 11:00:32   iarustam
   Bug fixes and core 1_3_0 changes
   
      Rev 1.1   Jan 21 1999 13:10:18   iarustam
   Bug fixes
   
   
      Rev 1.1   Dec 18 1998 13:45:10   iarustam
    
*
************************************************************************************/
// WorkstationDefinition.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CWorkstationDefinition dialog
#include "WorkstationDescription.h"
#include "ChangeHistory.h"

class CWorkstationDefinition : public CPropertyPage
{
	DECLARE_DYNCREATE(CWorkstationDefinition)

// Construction
public:
	CWorkstationDefinition();
	~CWorkstationDefinition();

	int m_nDialogMode;
	char m_strCurrentData[151];
   char m_strPreviousData[151];
	char m_strPassword[51];
	char m_strGroup[16];
   char m_strFieldTag[60];


   CString strStatus;
	
	CChangeHistory* pChangeHistoryPage;
	CWorkstationDescription *pDescriptionPage;

   BOOL CheckDialogForCompleteness();
   BOOL PopulateWorkstationDialogFromDatabase ( ATP02 Workstation);
   void PopulateHistoryTable();	
	CTime c_time;
   // Dialog Data
	//{{AFX_DATA(CWorkstationDefinition)
	enum { IDD = IDD_WORKSTATION_DEFINITION };
	CButton	m_ViewButton;
	CButton	m_UpdateButton;
	CButton	m_ListButton;
	CButton	m_DeleteButton;
	CButton	m_AddButton;
	CEdit	m_WorkstationIdCtrl;
	CStatic	m_MessageCtrl;
	CString	m_Message;
	CString	m_WorkstationIdStr;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CWorkstationDefinition)
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
	//{{AFX_MSG(CWorkstationDefinition)
	afx_msg void OnWorkstationAddButton();
	afx_msg void OnWorkstationDeleteButton();
	afx_msg void OnWorkstationListButton();
	afx_msg void OnWorkstationUpdateButton();
	afx_msg void OnWorkstationViewButton();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};
