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
* $Log:   N:\PVCS\PTE\Equitable\ATP_gui\JobDefDlg.h  $
   
      Rev 1.5   Dec 01 2003 11:29:26   lmitchel
   Added AMEX Online Updates - scr1200
   
      Rev 1.4   Jan 05 2000 14:53:40   gbarnes
   Major update. Not updated to PVCS recently.
   PVCS was locked.
   
      Rev 1.1   Sep 22 1999 15:50:38   iarustam
   Added manual batch entry GUI
   
      Rev 1.0   Jul 01 1999 14:17:34   iarustam
   Initial Release
   
      Rev 1.0   Jun 28 1999 13:58:36   iarustam
   Initial Release
   
      Rev 1.1   Apr 29 1999 14:56:26   iarustam
   Bug fixes
   
      Rev 1.0   Feb 23 1999 11:40:32   iarustam
   Initial Release
   
      Rev 1.0   Feb 17 1999 10:09:30   iarustam
   Initial Release
   
      Rev 1.1   Jan 22 1999 10:58:54   iarustam
   Bug fixes and core 1_3_0 changes
   
      Rev 1.1   Jan 21 1999 13:08:10   iarustam
   Bug fixes
   
   
      Rev 1.1   Dec 18 1998 13:43:28   iarustam
    
*
************************************************************************************/
// JobDefDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CJobDefDlg dialog

class CJobDefDlg : public CDialog
{
// Construction
public:
	CJobDefDlg(CWnd* pParent = NULL);   // standard constructor
	CTime c_time, shm_time;
// Dialog Data
	//{{AFX_DATA(CJobDefDlg)
	enum { IDD = ID_JOBDEFDLG };
	CString	m_ProfileName;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CJobDefDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CJobDefDlg)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
