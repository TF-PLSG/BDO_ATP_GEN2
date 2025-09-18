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
* $Log:   N:\PVCS\PTE\Equitable\ATP_gui\SchSearch.h  $
   
      Rev 1.5   Dec 01 2003 11:30:12   lmitchel
   Added AMEX Online Updates - scr1200
   
      Rev 1.4   Jan 05 2000 14:54:22   gbarnes
   Major update. Not updated to PVCS recently.
   PVCS was locked.
   
      Rev 1.1   Sep 22 1999 15:59:46   iarustam
   Added manual batch entry GUI
   
      Rev 1.0   Jul 01 1999 14:18:18   iarustam
   Initial Release
   
      Rev 1.0   Jun 28 1999 13:59:24   iarustam
   Initial Release
   
      Rev 1.1   Apr 29 1999 14:57:02   iarustam
   Bug fixes
   
      Rev 1.0   Feb 23 1999 11:41:16   iarustam
   Initial Release
   
      Rev 1.0   Feb 17 1999 10:10:04   iarustam
   Initial Release
   
      Rev 1.1   Jan 22 1999 11:00:08   iarustam
   Bug fixes and core 1_3_0 changes
   
      Rev 1.1   Jan 21 1999 13:09:44   iarustam
   Bug fixes
   
   
      Rev 1.1   Dec 18 1998 13:44:44   iarustam
    
*
************************************************************************************/
// SchSearch.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSchSearch dialog

class CSchSearch : public CDialog
{
// Construction
public:
	CSchSearch(CWnd* pParent = NULL);   // standard constructor
	CTime c_time;
// Dialog Data
	//{{AFX_DATA(CSchSearch)
	enum { IDD = IDD_SCHSEARCH };
	CEdit	m_SearchCtrl;
	CString	m_SearchItem;
	CString	m_SearchLbl;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSchSearch)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSchSearch)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
