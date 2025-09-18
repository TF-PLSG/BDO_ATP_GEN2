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
* $Log:   N:\PVCS\PTE\Equitable\ATP_gui\APPDLG.H  $
   
      Rev 1.5   Dec 01 2003 11:28:22   lmitchel
   Added AMEX Online Updates - scr1200
   
      Rev 1.4   Jan 05 2000 14:52:28   gbarnes
   Major update. Not updated to PVCS recently.
   PVCS was locked.
   
      Rev 1.1   Sep 22 1999 15:40:00   iarustam
   Added manual batch entry GUI
   
      Rev 1.0   Jul 01 1999 14:16:42   iarustam
   Initial Release
   
      Rev 1.0   Jun 28 1999 13:57:44   iarustam
   Initial Release
   
      Rev 1.1   Apr 29 1999 14:55:04   iarustam
   Bug fixes
   
      Rev 1.0   Feb 23 1999 11:39:42   iarustam
   Initial Release
   
      Rev 1.0   Feb 17 1999 10:08:46   iarustam
   Initial Release
   
      Rev 1.1   Jan 22 1999 10:56:30   iarustam
   Bug fixes and core 1_3_0 changes
   
      Rev 1.1   Jan 21 1999 13:04:42   iarustam
   Bug fixes
   
   
      Rev 1.2   Jan 13 1999 14:17:20   iarustam
    
   
      Rev 1.1   Dec 18 1998 13:39:58   iarustam
    
*
************************************************************************************/
// AppDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CAppDlg dialog
#include "app_info.h"

class CAppDlg : public CDialog
{
// Construction
public:
	CAppDlg(CWnd* pParent = NULL);   // standard constructor

   INT m_mode;
   APF01 m_apf01;
	CTime c_time;
// Dialog Data
	//{{AFX_DATA(CAppDlg)
	enum { IDD = IDD_APP_DIALOG };
	CEdit	m_AppNameCtrl;
	CString	m_AppNameStr;
	CString	m_ExeName;
	CString	m_Parameters;
	CString	m_Path;
	CString	m_Message;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAppDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CAppDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
