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
* $Log:   N:\PVCS\PTE\Equitable\ATP_gui\ChangeHistory.h  $
   
      Rev 1.5   Dec 01 2003 11:29:08   lmitchel
   Added AMEX Online Updates - scr1200
   
      Rev 1.4   Jan 05 2000 14:53:14   gbarnes
   Major update. Not updated to PVCS recently.
   PVCS was locked.
   
      Rev 1.1   Sep 22 1999 15:47:04   iarustam
   Added manual batch entry GUI
   
      Rev 1.0   Jul 01 1999 14:17:12   iarustam
   Initial Release
   
      Rev 1.0   Jun 28 1999 13:58:18   iarustam
   Initial Release
   
      Rev 1.1   Apr 29 1999 14:56:10   iarustam
   Bug fixes
   
      Rev 1.0   Feb 23 1999 11:40:16   iarustam
   Initial Release
   
      Rev 1.0   Feb 17 1999 10:09:18   iarustam
   Initial Release
   
      Rev 1.1   Jan 22 1999 10:58:22   iarustam
   Bug fixes and core 1_3_0 changes
   
      Rev 1.1   Jan 21 1999 13:07:32   iarustam
   Bug fixes
   
   
      Rev 1.1   Dec 18 1998 13:43:00   iarustam
    
*
************************************************************************************/
// ChangeHistory.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CChangeHistory dialog
#ifndef CHANGEHISTORY
#define CHANGEHISTORY

#define UNIQUE_ID_SIZE 18
#include "app_info.h"

class CChangeHistory : public CPropertyPage
{
	DECLARE_DYNCREATE(CChangeHistory)

// Construction
public:
	CChangeHistory();
	~CChangeHistory();

    char strLastUniqueID [UNIQUE_ID_SIZE];
    HST01 sHistoryRecord;
    int m_nDialogMode;
	int m_nPropertySheetMode;
    BOOL bDone;
	BOOL bNext;
	BOOL m_bChangeHistory;

	void PopulateHistoryDialog();
	void CleanChangeHistory();

	CTime c_time;	
// Dialog Data
	//{{AFX_DATA(CChangeHistory)
	enum { IDD = IDD_CHANGE_HISTORY };
	CButton	m_CntlNextButton;
	CListCtrl	m_CntlList;
	CString	m_StrMessageLbl;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CChangeHistory)
	public:
	virtual BOOL OnSetActive();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CChangeHistory)
	virtual BOOL OnInitDialog();
	afx_msg void OnNextButton();
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

#endif
