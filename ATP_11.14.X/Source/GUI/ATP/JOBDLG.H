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
* $Log:   N:\PVCS\PTE\Equitable\ATP_gui\JOBDLG.H  $
   
      Rev 1.5   Dec 01 2003 11:29:28   lmitchel
   Added AMEX Online Updates - scr1200
   
      Rev 1.4   Jan 05 2000 14:53:38   gbarnes
   Major update. Not updated to PVCS recently.
   PVCS was locked.
   
      Rev 1.1   Sep 22 1999 15:50:26   iarustam
   Added manual batch entry GUI
   
      Rev 1.0   Jul 01 1999 14:17:32   iarustam
   Initial Release
   
      Rev 1.0   Jun 28 1999 13:58:36   iarustam
   Initial Release
   
      Rev 1.1   Apr 29 1999 14:56:24   iarustam
   Bug fixes
   
      Rev 1.0   Feb 23 1999 11:40:30   iarustam
   Initial Release
   
      Rev 1.0   Feb 17 1999 10:09:28   iarustam
   Initial Release
   
      Rev 1.1   Jan 22 1999 10:58:54   iarustam
   Bug fixes and core 1_3_0 changes
   
      Rev 1.1   Jan 21 1999 13:08:08   iarustam
   Bug fixes
   
   
      Rev 1.2   Jan 13 1999 14:18:02   iarustam
    
   
      Rev 1.1   Dec 18 1998 13:41:58   iarustam
    
*
************************************************************************************/
// JobDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CJobDlg dialog
#include "app_info.h"

class CJobDlg : public CDialog
{
// Construction
public:
	CJobDlg(CWnd* pParent = NULL);   // standard constructor

   INT   m_mode;
   JBF01 m_jbf01;
   CTime c_time;

// Dialog Data
	//{{AFX_DATA(CJobDlg)
	enum { IDD = IDD_JOB_DIALOG };
	CComboBox	m_Status;
	CEdit	m_JobIdCtrl;
	CComboBox	m_Minute;
	CComboBox	m_AppName;
	CComboBox	m_Hour;
	CString	m_JobIdStr;
	CString	m_LastRun;
	CString	m_Message;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CJobDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CJobDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
