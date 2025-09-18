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
* $Log:   N:\PVCS\PTE\Equitable\ATP_gui\ProfileDlg.h  $
   
      Rev 1.5   Dec 01 2003 11:30:04   lmitchel
   Added AMEX Online Updates - scr1200
   
      Rev 1.4   Jan 05 2000 14:54:16   gbarnes
   Major update. Not updated to PVCS recently.
   PVCS was locked.
   
      Rev 1.1   Sep 22 1999 15:58:24   iarustam
   Added manual batch entry GUI
   
      Rev 1.0   Jul 01 1999 14:18:10   iarustam
   Initial Release
   
      Rev 1.0   Jun 28 1999 13:59:16   iarustam
   Initial Release
   
      Rev 1.1   Apr 29 1999 14:56:56   iarustam
   Bug fixes
   
      Rev 1.0   Feb 23 1999 11:41:08   iarustam
   Initial Release
   
      Rev 1.0   Feb 17 1999 10:09:56   iarustam
   Initial Release
   
      Rev 1.1   Jan 22 1999 11:00:00   iarustam
   Bug fixes and core 1_3_0 changes
   
      Rev 1.1   Jan 21 1999 13:09:38   iarustam
   Bug fixes
   
   
      Rev 1.2   Jan 13 1999 14:21:04   iarustam
    
   
      Rev 1.1   Dec 18 1998 13:44:38   iarustam
    
*
************************************************************************************/
// ProfileDlg.h : header file
//
#include "app_info.h"

/////////////////////////////////////////////////////////////////////////////
// CProfileDlg dialog

class CProfileDlg : public CDialog
{
// Construction
public:
	CProfileDlg(CWnd* pParent = NULL);   // standard constructor

   INT   m_mode;
   BPF01_SCH_LIST m_bpf01_sch_list;
	CTime c_time;
// Dialog Data
	//{{AFX_DATA(CProfileDlg)
	enum { IDD = IDD_PROFILE_DIALOG };
	CEdit	m_ProfileIdCtrl;
	CListBox	m_JobsInProfileList;
	CListBox	m_AvailableList;
	CString	m_ProfileIdStr;
	CString	m_Message;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CProfileDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CProfileDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnProfileAdd();
	afx_msg void OnProfileRemove();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
