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
* $Log:   N:\PVCS\PTE\Equitable\ATP_gui\MerchantTerminalPoints.h  $
   
      Rev 1.5   Dec 01 2003 11:29:44   lmitchel
   Added AMEX Online Updates - scr1200
   
      Rev 1.4   Jan 05 2000 14:54:04   gbarnes
   Major update. Not updated to PVCS recently.
   PVCS was locked.
   
      Rev 1.1   Sep 22 1999 15:54:50   iarustam
   Added manual batch entry GUI
   
      Rev 1.0   Jul 01 1999 14:17:52   iarustam
   Initial Release
   
      Rev 1.0   Jun 28 1999 13:58:58   iarustam
   Initial Release
   
      Rev 1.1   Apr 29 1999 14:56:42   iarustam
   Bug fixes
   
      Rev 1.0   Feb 23 1999 11:40:48   iarustam
   Initial Release
   
      Rev 1.0   Feb 17 1999 10:09:42   iarustam
   Initial Release
   
      Rev 1.1   Jan 22 1999 10:59:28   iarustam
   Bug fixes and core 1_3_0 changes
   
      Rev 1.1   Jan 21 1999 13:08:56   iarustam
   Bug fixes
   
   
      Rev 1.1   Dec 18 1998 13:44:04   iarustam
    
*
************************************************************************************/
// MerchantTerminalPoints.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CMerchantTerminalPoints dialog
#ifndef TERMINALPOINTS
#define TERMINALPOINTS


class CMerchantTerminalPoints : public CPropertyPage
{
	DECLARE_DYNCREATE(CMerchantTerminalPoints)

// Construction
public:
	CMerchantTerminalPoints();
	~CMerchantTerminalPoints();
	CTime c_time;
// Dialog Data
	//{{AFX_DATA(CMerchantTerminalPoints)
	enum { IDD = IDD_TERMINAL_POINTS };
	CEdit	m_CntlSpecialPointsStartFld;
	CEdit	m_CntlSpecialPointsPurseFld;
	CEdit	m_CntlSpecialPointsEndFld;
	CEdit	m_CntlPointsPurseFld;
	CString	m_StrChainIDFld;
	CString	m_StrMerchantIDFld;
	CString	m_StrPointsPurseFld;
	CString	m_StrSpecialPointsEndFld;
	CString	m_StrSpecialPointsPurseFld;
	CString	m_StrSpecialPointsStartFld;
	CString	m_StrTerminalIDFld;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CMerchantTerminalPoints)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CMerchantTerminalPoints)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};
#endif
