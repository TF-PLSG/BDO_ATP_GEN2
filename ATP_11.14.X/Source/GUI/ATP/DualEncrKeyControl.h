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
* $Log:   N:\PVCS\PTE\Equitable\ATP_gui\DualEncrKeyControl.h  $
   
      Rev 1.0   Feb 02 2011 11:30:22   TF_Srikanth
   Added DualEncrKeyControl Class for Changing Encryption Key in Database from GUI  
    
*
************************************************************************************/
// DualEncrKeyControl.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDualEncrKeyControl dialog
#define DB_ENCRPTION_KEY  1

class CDualEncrKeyControl : public CPropertyPage
{
	DECLARE_DYNCREATE(CDualEncrKeyControl)

// Construction
public:
	CDualEncrKeyControl();
	~CDualEncrKeyControl();
	
	CTime c_time;
	BOOL CheckforInvalidChars(CString);
// Dialog Data
	//{{AFX_DATA(CDualEncrKeyControl)
	enum { IDD = IDD_DENCKEY };
	CEdit	m_CntlDualCoreEkey;
	CEdit	m_CntlCDualCoreEkey;
	CString	m_StrUserNameFld;
	CString	m_StrDualCoreEkey;
	CString	m_StrCDualCoreEkey;
	CString	m_StrMessageLbl;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CDualEncrKeyControl)
	public:
	virtual	BOOL CheckDialogForCompleteness();
	virtual void OnCancel();
	virtual BOOL OnApply();
	virtual void OnOK();
	virtual BOOL OnSetActive();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CDualEncrKeyControl)
	afx_msg void OnChangeCdualCoreEkey();
	afx_msg void OnChangeDualCoreEkey();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};
////////////////////////////////////////////////////////