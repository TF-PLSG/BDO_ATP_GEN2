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
* $Log:   N:\PVCS\PTE\Equitable\ATP_gui\SettleFileDefinition.h  $
   
      Rev 1.5   Dec 01 2003 11:30:12   lmitchel
   Added AMEX Online Updates - scr1200
   
      Rev 1.4   Jan 05 2000 14:54:24   gbarnes
   Major update. Not updated to PVCS recently.
   PVCS was locked.
   
      Rev 1.1   Sep 22 1999 16:00:12   iarustam
   Added manual batch entry GUI
   
      Rev 1.0   Jul 01 1999 14:18:20   iarustam
   Initial Release
   
      Rev 1.0   Jun 28 1999 13:59:26   iarustam
   Initial Release
   
      Rev 1.1   Apr 29 1999 14:57:04   iarustam
   Bug fixes
   
      Rev 1.0   Feb 23 1999 11:41:18   iarustam
   Initial Release
   
      Rev 1.0   Feb 17 1999 10:10:06   iarustam
   Initial Release
   
      Rev 1.1   Jan 22 1999 11:00:12   iarustam
   Bug fixes and core 1_3_0 changes
   
      Rev 1.1   Jan 21 1999 13:09:52   iarustam
   Bug fixes
   
   
      Rev 1.1   Dec 18 1998 13:44:48   iarustam
    
*
************************************************************************************/
// SettleFileDefinition.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSettleFileDefinition dialog

class CSettleFileDefinition : public CPropertyPage
{
	DECLARE_DYNCREATE(CSettleFileDefinition)

// Construction
public:
	CSettleFileDefinition();
	~CSettleFileDefinition();
	CTime c_time;
// Dialog Data
	//{{AFX_DATA(CSettleFileDefinition)
	enum { IDD = IDD_SETTLE_FILE_DEFINITION };
	CEdit	m_CntlSettleFileFld;
	CString	m_StrSettleFileFld;
	CString	m_StrMessageLbl;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CSettleFileDefinition)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CSettleFileDefinition)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};
