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
* $Log:   N:\PVCS\PTE\Equitable\ATP_gui\NegativeBinRange.h  $
   
      Rev 1.5   Dec 01 2003 11:29:48   lmitchel
   Added AMEX Online Updates - scr1200
   
      Rev 1.4   Jan 05 2000 14:54:06   gbarnes
   Major update. Not updated to PVCS recently.
   PVCS was locked.
   
      Rev 1.1   Sep 22 1999 15:55:44   iarustam
   Added manual batch entry GUI
   
      Rev 1.0   Jul 01 1999 14:17:56   iarustam
   Initial Release
   
      Rev 1.0   Jun 28 1999 13:59:02   iarustam
   Initial Release
   
      Rev 1.1   Apr 29 1999 14:56:46   iarustam
   Bug fixes
   
      Rev 1.0   Feb 23 1999 11:40:50   iarustam
   Initial Release
   
      Rev 1.0   Feb 17 1999 10:09:46   iarustam
   Initial Release
   
      Rev 1.1   Jan 22 1999 10:59:34   iarustam
   Bug fixes and core 1_3_0 changes
   
      Rev 1.1   Jan 21 1999 13:09:06   iarustam
   Bug fixes
   
   
      Rev 1.1   Dec 18 1998 13:44:10   iarustam
    
*
************************************************************************************/
// NegativeBinRange.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CNegativeBinRange dialog

class CNegativeBinRange : public CDialog
{
// Construction
public:
	CNegativeBinRange(CWnd* pParent = NULL);   // standard constructor

	BOOL CheckDialogForCompleteness();
	CTime c_time;

// Dialog Data
	//{{AFX_DATA(CNegativeBinRange)
	enum { IDD = IDD_NEG_BIN_RANGE };
	CEdit	m_CntlLowFld;
	CButton	m_ListButton;
	CEdit	m_CntlHighFld;
	CButton	m_DeleteButton;
	CButton	m_AddButton;
	CString	m_StrHighFld;
	CString	m_StrLowFld;
	CString	m_StrMessageLbl;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CNegativeBinRange)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CNegativeBinRange)
	virtual BOOL OnInitDialog();
	afx_msg void OnAddButton();
	afx_msg void OnDeleteButton();
	afx_msg void OnListButton();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
