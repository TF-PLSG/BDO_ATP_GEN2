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
* $Log:   N:\PVCS\PTE\Equitable\ATP_gui\SRVCMGR.H  $
   
      Rev 1.5   Dec 01 2003 11:30:16   lmitchel
   Added AMEX Online Updates - scr1200
   
      Rev 1.4   Jan 05 2000 14:54:20   gbarnes
   Major update. Not updated to PVCS recently.
   PVCS was locked.
   
      Rev 1.1   Sep 22 1999 15:59:32   iarustam
   Added manual batch entry GUI
   
      Rev 1.0   Jul 01 1999 14:18:16   iarustam
   Initial Release
   
      Rev 1.0   Jun 28 1999 13:59:22   iarustam
   Initial Release
   
      Rev 1.1   Apr 29 1999 14:57:02   iarustam
   Bug fixes
   
      Rev 1.0   Feb 23 1999 11:41:14   iarustam
   Initial Release
   
      Rev 1.0   Feb 17 1999 10:10:04   iarustam
   Initial Release
   
      Rev 1.1   Jan 22 1999 11:00:04   iarustam
   Bug fixes and core 1_3_0 changes
   
      Rev 1.1   Jan 21 1999 13:09:42   iarustam
   Bug fixes
   
   
      Rev 1.1   Dec 18 1998 13:48:14   iarustam
    
*
************************************************************************************/

// SrvcMgr.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSrvcMgr dialog
#include "srvcinfo.h"


class CSrvcMgr : public CDialog
{
// Construction
public:
	CSrvcMgr(CWnd* pParent = NULL);   // standard constructor

   SERVICE_INFO m_ServiceTable [MAX_SERVICES];
   SERVICE_INFO m_TempTable [MAX_SERVICES];
   INT m_NumServices;
   INT m_TabSelected;

   UINT m_RefreshTimer;
   INT  m_AutoRefreshOff;
   CHAR m_PingStatus[MAX_SERVICES][15];

   CHAR m_PrimaryMachine[50];
   CHAR m_SecondaryMachine[50];
   CHAR m_ConnectString[256];
   INT  m_HighAvailability;
   INT  m_FirstUpdate;
   CHAR m_XipcInstance[12];

   CFrameWnd* m_StatusWindow;
	//CImageList* m_pImageList ;

   INT GetHighAvailableIndicator( pINT Indicator );
   void InitLogins();
   void TerminateLogins();
   void RefreshList();	
	CTime c_time;
// Dialog Data
	//{{AFX_DATA(CSrvcMgr)
	enum { IDD = IDD_SRVCMON_DIALOG };
	CTabCtrl	m_MainTab;
	CListBox	m_ServiceList;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSrvcMgr)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSrvcMgr)
	afx_msg void OnClose();
	afx_msg void OnDisable();
	afx_msg void OnEnable();
	afx_msg void OnPing();
	afx_msg void OnPingall();
	afx_msg void OnRefresh();
	afx_msg void OnStart();
	afx_msg void OnStartall();
	afx_msg void OnStop();
	afx_msg void OnStopall();
	afx_msg void OnTraceoff();
	afx_msg void OnTraceon();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnSelchangeServiceTab(NMHDR* pNMHDR, LRESULT* pResult);
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
