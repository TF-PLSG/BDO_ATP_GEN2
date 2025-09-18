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
* $Log:   N:\PVCS\PTE\Equitable\ATP_gui\SchedulerConfig.h  $
   
      Rev 1.5   Dec 01 2003 11:30:10   lmitchel
   Added AMEX Online Updates - scr1200
   
      Rev 1.4   Jan 05 2000 14:54:22   gbarnes
   Major update. Not updated to PVCS recently.
   PVCS was locked.
   
      Rev 1.3   Oct 15 1999 17:21:02   iarustam
   bug fixes
   
      Rev 1.2   Sep 30 1999 16:17:48   npogosya
   Bug fixes
   
      Rev 1.1   Sep 22 1999 15:59:56   iarustam
   Added manual batch entry GUI
   
      Rev 1.0   Jul 01 1999 14:18:18   iarustam
   Initial Release
   
      Rev 1.0   Jun 28 1999 13:59:24   iarustam
   Initial Release
   
      Rev 1.1   Apr 29 1999 14:57:04   iarustam
   Bug fixes
   
      Rev 1.0   Feb 23 1999 11:41:16   iarustam
   Initial Release
   
      Rev 1.0   Feb 17 1999 10:10:06   iarustam
   Initial Release
   
      Rev 1.1   Jan 22 1999 11:00:10   iarustam
   Bug fixes and core 1_3_0 changes
   
      Rev 1.1   Jan 21 1999 13:09:46   iarustam
   Bug fixes
   
   
      Rev 1.2   Jan 13 1999 14:21:16   iarustam
    
   
      Rev 1.1   Dec 18 1998 13:44:46   iarustam
    
*
************************************************************************************/
// SchedulerConfig.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSchedulerConfig dialog
#include "app_info.h"

class CSchedulerConfig : public CPropertyPage
{
	DECLARE_DYNCREATE(CSchedulerConfig)

// Construction
public:
	CSchedulerConfig();
	~CSchedulerConfig();

   BPF01_GUI_LIST m_bpf01_list;
   BPF01_SCH_LIST m_bpf01_sch_list;
   BPF01 m_bpf01;

   APF01_GUI_LIST m_apf01_list;
   APF01 m_apf01;

   JBF01_GUI_LIST m_jbf01_list;
   JBF01 m_jbf01;

   INT   m_TabSelected;

   CHAR m_LastAppName[16];
   CHAR m_LastJobId[16];
   CHAR m_LastProfileId[16];
	CTime c_time;
// Dialog Data
	//{{AFX_DATA(CSchedulerConfig)
	enum { IDD = IDD_SCHEDULER_CONFIG };
	CButton	m_NextButton;
	CEdit	m_SchStatus;
	CListBox	m_SchList;
	CTabCtrl	m_SchTab;
	CString	m_Message;
	CString	m_StrStatus;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CSchedulerConfig)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CSchedulerConfig)
	virtual BOOL OnInitDialog();
	afx_msg void OnSchAdd();
	afx_msg void OnSchEdit();
	afx_msg void OnSchDelete();
	afx_msg void OnSchFind();
	afx_msg void OnSchNext20();
	afx_msg void OnDblclkSchList();
	afx_msg void OnSelchangeSchTab(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};
