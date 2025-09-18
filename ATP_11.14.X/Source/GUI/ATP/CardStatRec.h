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
* $Log:   N:\PVCS\PTE\Equitable\ATP_gui\CardStatRec.h  $
   
      Rev 1.5   Dec 01 2003 11:28:56   lmitchel
   Added AMEX Online Updates - scr1200
   
      Rev 1.4   Jan 05 2000 14:53:04   gbarnes
   Major update. Not updated to PVCS recently.
   PVCS was locked.
   
      Rev 1.1   Sep 22 1999 15:44:44   iarustam
   Added manual batch entry GUI
   
      Rev 1.0   Jul 01 1999 14:17:02   iarustam
   Initial Release
   
      Rev 1.0   Jun 28 1999 13:58:08   iarustam
   Initial Release
   
      Rev 1.1   Apr 29 1999 14:56:00   iarustam
   Bug fixes
   
      Rev 1.0   Feb 23 1999 11:40:08   iarustam
   Initial Release
   
      Rev 1.0   Feb 17 1999 10:09:10   iarustam
   Initial Release
   
      Rev 1.1   Jan 22 1999 10:58:00   iarustam
   Bug fixes and core 1_3_0 changes
   
      Rev 1.1   Jan 21 1999 13:07:02   iarustam
   Bug fixes
   
   
      Rev 1.1   Dec 18 1998 13:42:38   iarustam
    
*
************************************************************************************/
// CardStatRec.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CCardStatRec dialog

class CCardStatRec : public CDialog
{
// Construction
public:
	CCardStatRec(CWnd* pParent = NULL);   // standard constructor

	int DialogMode;

	CTime c_time;
// Dialog Data
	//{{AFX_DATA(CCardStatRec)
	enum { IDD = IDD_CARDSTATREC };
	CStatic	m_ctrlComEdit;
	CEdit	m_ctrlISOResp;
	CEdit	m_ctrlMacValue;
	CEdit	m_ctrlCardStat;
	CString	m_strCardStat;
	CString	m_strDescr;
	CString	m_strISOResp;
	CString	m_strMacValue;
	CString	m_strTextMes;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCardStatRec)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CCardStatRec)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
