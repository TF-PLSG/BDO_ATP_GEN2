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
* $Log:   N:\PVCS\PTE\Equitable\ATP_gui\ShedulerControl.h  $
   
      Rev 1.5   Dec 01 2003 11:30:14   lmitchel
   Added AMEX Online Updates - scr1200
   
      Rev 1.4   Jan 05 2000 14:54:26   gbarnes
   Major update. Not updated to PVCS recently.
   PVCS was locked.
   
      Rev 1.1   Sep 22 1999 16:00:38   iarustam
   Added manual batch entry GUI
   
      Rev 1.0   Jul 01 1999 14:18:22   iarustam
   Initial Release
   
      Rev 1.0   Jun 28 1999 13:59:28   iarustam
   Initial Release
   
      Rev 1.1   Apr 29 1999 14:57:06   iarustam
   Bug fixes
   
      Rev 1.0   Feb 23 1999 11:41:20   iarustam
   Initial Release
   
      Rev 1.0   Feb 17 1999 10:10:08   iarustam
   Initial Release
   
      Rev 1.1   Jan 22 1999 11:00:16   iarustam
   Bug fixes and core 1_3_0 changes
   
      Rev 1.1   Jan 21 1999 13:09:58   iarustam
   Bug fixes
   
   
      Rev 1.2   Jan 13 1999 14:21:24   iarustam
    
   
      Rev 1.1   Dec 18 1998 13:44:54   iarustam
    
*
************************************************************************************/
// ShedulerControl.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CShedulerControl dialog
#include "schduler.h"

class CShedulerControl : public CPropertyPage
{
	DECLARE_DYNCREATE(CShedulerControl)

// Construction
public:
	CShedulerControl();
	~CShedulerControl();

   INT m_num_jobs;
   LOCAL_JOB_TABLE m_job_table[SCH_MAX_LIST_SIZE];
	
	CTime c_time;
// Dialog Data
	//{{AFX_DATA(CShedulerControl)
	enum { IDD = IDD_SCHEDULER_CONTROL };
	CEdit	m_SchControlStatus;
	CListCtrl	m_JobList;
	CString	m_SchControlTitle;
	CString	m_Message;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CShedulerControl)
	public:
	virtual BOOL OnSetActive();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CShedulerControl)
	afx_msg void OnAddjob();
	afx_msg void OnCanceljob();
	afx_msg void OnRefresh();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};
