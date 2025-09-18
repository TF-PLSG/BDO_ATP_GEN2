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
* $Log:   N:\PVCS\PTE\Equitable\ATP_gui\CFActivity.h  $
   
      Rev 1.8   Dec 01 2003 11:29:02   lmitchel
   Added AMEX Online Updates - scr1200
   
      Rev 1.7   Jan 05 2000 14:52:56   gbarnes
   Major update. Not updated to PVCS recently.
   PVCS was locked.
   
      Rev 1.1   Sep 22 1999 15:42:18   iarustam
   Added manual batch entry GUI
   
      Rev 1.0   Jul 01 1999 14:16:52   iarustam
   Initial Release
   
      Rev 1.0   Jun 28 1999 13:57:56   iarustam
   Initial Release
   
      Rev 1.4   Jun 04 1999 17:41:26   iarustam
   Bug fixes
   
      Rev 1.3   Apr 29 1999 14:55:50   iarustam
   Bug fixes
   
      Rev 1.2   Mar 04 1999 13:32:46   iarustam
   Bug fixes
   
      Rev 1.0   Feb 23 1999 11:39:58   iarustam
   Initial Release
   
      Rev 1.0   Feb 17 1999 10:09:00   iarustam
   Initial Release
   
      Rev 1.1   Jan 22 1999 10:57:36   iarustam
   Bug fixes and core 1_3_0 changes
   
      Rev 1.1   Jan 21 1999 13:06:28   iarustam
   Bug fixes
   
   
      Rev 1.1   Dec 18 1998 13:42:18   iarustam
    
*
************************************************************************************/
// CFActivity.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CCFActivity dialog
#ifndef ACTIVPAGE
#define ACTIVPAGE

class CCFActivity : public CPropertyPage
{
	DECLARE_DYNCREATE(CCFActivity)

// Construction
public:
	CCFActivity();
	~CCFActivity();

	int DialogMode;
	CTime c_time;
	// Dialog Data
	//{{AFX_DATA(CCFActivity)
	enum { IDD = IDD_CARDACTIVITY };
	CStatic	m_ComEdit;
	CString	m_strCardType;
	CString	m_strDate;
	CString	m_strP1Amt;
	CString	m_strP1Nbr;
	CString	m_strP1Lbl;
	CString	m_strP2Amt;
	CString	m_strP2Nbr;
	CString	m_strP2Lbl;
	CString	m_strP3Amt;
	CString	m_strP3Nbr;
	CString	m_strP3Lbl;
	CString	m_strP1AmtDol;
	CString	m_strP1NbrDol;
	CString	m_strP2AmtDol;
	CString	m_strP2NbrDol;
	CString	m_strP3AmtDol;
	CString	m_strP3NbrDol;
	CString	m_strCardNbr;
	CString	m_strDate2;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CCFActivity)
	public:
	virtual BOOL OnSetActive();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CCFActivity)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};
#endif
