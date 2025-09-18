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
* $Log:   N:\PVCS\PTE\Equitable\ATP_gui\PanDemogrPage.h  $
   
      Rev 1.5   Dec 01 2003 11:30:00   lmitchel
   Added AMEX Online Updates - scr1200
   
      Rev 1.4   Jan 05 2000 14:54:10   gbarnes
   Major update. Not updated to PVCS recently.
   PVCS was locked.
   
      Rev 1.1   Sep 22 1999 15:57:28   iarustam
   Added manual batch entry GUI
   
      Rev 1.0   Jul 01 1999 14:18:04   iarustam
   Initial Release
   
      Rev 1.0   Jun 28 1999 13:59:12   iarustam
   Initial Release
   
      Rev 1.1   Apr 29 1999 14:56:54   iarustam
   Bug fixes
   
      Rev 1.0   Feb 23 1999 11:41:00   iarustam
   Initial Release
   
      Rev 1.0   Feb 17 1999 10:09:52   iarustam
   Initial Release
   
      Rev 1.1   Jan 22 1999 10:59:52   iarustam
   Bug fixes and core 1_3_0 changes
   
      Rev 1.1   Jan 21 1999 13:09:28   iarustam
   Bug fixes
   
   
      Rev 1.1   Dec 18 1998 13:44:30   iarustam
    
*
************************************************************************************/
// PanDemogrPage.h : header file
//
/////////////////////////////////////////////////////////////////////////////
// CPanDemogrPage dialog

#ifndef DEMOPAGE
#define DEMOPAGE

class CPanDemogrPage : public CPropertyPage
{
	DECLARE_DYNCREATE(CPanDemogrPage)

// Construction
public:
	CPanDemogrPage();
	~CPanDemogrPage();

	int DialogMode;
	
CTime c_time;

// Dialog Data
	//{{AFX_DATA(CPanDemogrPage)
	enum { IDD = IDD_PANDEMOGR };
	CEdit	m_panedit;
	CEdit	m_Zip;
	CEdit	m_WorkPhone;
	CEdit	m_State;
	CEdit	m_Name;
	CEdit	m_HomePnone;
	CStatic	m_ComEdit;
	CEdit	m_City;
	CEdit	m_Addr2;
	CEdit	m_Addr1;
	CString	m_panstr;
	CString	m_strAddr1;
	CString	m_strAddr2;
	CString	m_strCity;
	CString	m_strHomePhone;
	CString	m_strName;
	CString	m_strState;
	CString	m_strWorkPhone;
	CString	m_strZip;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CPanDemogrPage)
	public:
	virtual BOOL OnSetActive();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CPanDemogrPage)
	virtual BOOL OnInitDialog();
	afx_msg void OnChangeZip();
	afx_msg void OnChangeAddr1();
	afx_msg void OnChangeAddr2();
	afx_msg void OnChangeCity();
	afx_msg void OnChangeHomephone();
	afx_msg void OnChangeName();
	afx_msg void OnChangeState();
	afx_msg void OnChangeWorkphone();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	
};

#endif
