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
* $Log:   N:\PVCS\PTE\Equitable\ATP_gui\LOGIN.H  $
   
      Rev 1.5   Dec 01 2003 11:29:28   lmitchel
   Added AMEX Online Updates - scr1200
   
      Rev 1.4   Jan 05 2000 14:53:40   gbarnes
   Major update. Not updated to PVCS recently.
   PVCS was locked.
   
      Rev 1.1   Sep 22 1999 15:50:46   iarustam
   Added manual batch entry GUI
   
      Rev 1.0   Jul 01 1999 14:17:34   iarustam
   Initial Release
   
      Rev 1.0   Jun 28 1999 13:58:38   iarustam
   Initial Release
   
      Rev 1.1   Apr 29 1999 14:56:26   iarustam
   Bug fixes
   
      Rev 1.0   Feb 23 1999 11:40:32   iarustam
   Initial Release
   
      Rev 1.0   Feb 17 1999 10:09:30   iarustam
   Initial Release
   
      Rev 1.1   Jan 22 1999 10:58:56   iarustam
   Bug fixes and core 1_3_0 changes
   
      Rev 1.1   Jan 21 1999 13:08:14   iarustam
   Bug fixes
   
   
      Rev 1.1   Dec 18 1998 13:42:00   iarustam
    
*
************************************************************************************/
// Login.h : header file
//
#define CANCEL 2
#define IDT_TIMER  WM_USER + 200
#define MAX_TIMEOUT 9
#define MAX_RETRY 3

/////////////////////////////////////////////////////////////////////////////
// CLogin dialog

class CLogin : public CDialog
{
// Construction
public:
	CLogin(CWnd* pParent = NULL);   // standard constructor
	CTime c_time;

// Dialog Data
	//{{AFX_DATA(CLogin)
	enum { IDD = IDD_LOGIN };
	CEdit	m_CntlLoginName;

	CString	m_StrMessageLbl;
	CString	m_LoginName;
	CString	m_Password;
	BOOL	m_ChangePassword;
	BOOL	m_Resetlogout;
	BOOL	m_bDisableNameFld;

	BOOL change_login;
	BOOL change_logout;

	//ramya
	CString Prev_Login_Date;
	CString Prev_Logout_Date;

	CString New_Login_Date;
	CString New_Logout_Date;

	
	//}}AFX_DATA

	short CheckLoginDetails();
	BOOL InactiveDetails();
	BOOL ExpiryDetails();
	BOOL PopulateLoginControlStructure();
	BOOL PopulateHistoryTable();
	BOOL Copy_previous_passwords(pUCF01 ucf01);
	short password_check();

	
	char idle_time_val[7];
	int idle_time;
	LONG result;
	HKEY hKey;
	unsigned long dwDisp;
	unsigned char idle_buffer[_MAX_PATH];
	unsigned long datatype;
	unsigned long idle_bufferlength;
	unsigned char State[2];
	int ret_val;
	

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CLogin)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CLogin)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnEnChangePassword();
	afx_msg void OnEnChangeLoginName();
	afx_msg void OnBnClickedChpasswordCheck();
	afx_msg void OnBnClickedResetLogoutCheckbox();
};

