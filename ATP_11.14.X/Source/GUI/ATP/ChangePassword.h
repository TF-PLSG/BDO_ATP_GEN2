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
* $Log:   N:\PVCS\PTE\Equitable\ATP_gui\ChangePassword.h  $

		Rev 1.6   Jan 25 2011  11:29:10 TF-Srikanth
   Added PCI changes for Old USer Password fields in GUI Change password window
   
      Rev 1.5   Dec 01 2003 11:29:10   lmitchel
   Added AMEX Online Updates - scr1200
   
      Rev 1.4   Jan 05 2000 14:53:14   gbarnes
   Major update. Not updated to PVCS recently.
   PVCS was locked.
   
      Rev 1.1   Sep 22 1999 15:47:14   iarustam
   Added manual batch entry GUI
   
      Rev 1.0   Jul 01 1999 14:17:12   iarustam
   Initial Release
   
      Rev 1.0   Jun 28 1999 13:58:20   iarustam
   Initial Release
   
      Rev 1.1   Apr 29 1999 14:56:10   iarustam
   Bug fixes
   
      Rev 1.0   Feb 23 1999 11:40:16   iarustam
   Initial Release
   
      Rev 1.0   Feb 17 1999 10:09:18   iarustam
   Initial Release
   
      Rev 1.1   Jan 22 1999 10:58:24   iarustam
   Bug fixes and core 1_3_0 changes
   
      Rev 1.1   Jan 21 1999 13:07:34   iarustam
   Bug fixes
   
   
      Rev 1.1   Dec 18 1998 13:43:02   iarustam
    
*
************************************************************************************/
// ChangePassword.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CChangePassword dialog

class CChangePassword : public CDialog
{
// Construction
public:
	CChangePassword(CWnd* pParent = NULL);   // standard constructor
	bool passwordExpired;
	bool bDontCheckOldPassword;
	BOOL isSpace(pCHAR string);
	BOOL isalphabet(pCHAR string);
	BOOL isnumeric(pCHAR string);

// Dialog Data
	//{{AFX_DATA(CChangePassword)
	enum { IDD = IDD_CHANGE_PASSWORD };
	CStatic	m_CntlOldPwdLbl;
	CStatic	m_CntlOldPasswordLbl;
	CEdit	m_CntlConfirmPassword;
	CEdit	m_CntlNewPassword;
	CEdit	m_CntlOldPassword;
	CString	m_ConfirmPassword;
	CString	m_UserLabel;
	CString	m_OldPassword;
	CString	m_StrNewPassword;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CChangePassword)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CChangePassword)
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	afx_msg void OnChangeOldPassword();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
