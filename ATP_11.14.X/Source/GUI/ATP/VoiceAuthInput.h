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
* $Log:   N:\PVCS\PTE\Equitable\ATP_gui\VoiceAuthInput.h  $
   
      Rev 1.7   Dec 01 2003 11:30:24   lmitchel
   Added AMEX Online Updates - scr1200
   
      Rev 1.6   Jan 05 2000 14:54:32   gbarnes
   Major update. Not updated to PVCS recently.
   PVCS was locked.
   
      Rev 1.1   Sep 22 1999 16:02:40   iarustam
   Added manual batch entry GUI
   
      Rev 1.0   Jul 01 1999 14:18:30   iarustam
   Initial Release
   
      Rev 1.0   Jun 28 1999 13:59:40   iarustam
   Initial Release
   
      Rev 1.3   May 10 1999 11:42:24   dperson
   Added PVCS header
*   
************************************************************************************/
// VoiceAuthInput.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CVoiceAuthInput dialog

class CVoiceAuthInput : public CDialog
{
// Construction
public:
	CVoiceAuthInput(CWnd* pParent = NULL);   // standard constructor
	CTime c_time;
// Dialog Data
	//{{AFX_DATA(CVoiceAuthInput)
	enum { IDD = IDD_VOICE_AUTH_INPUT };
	CEdit	m_CntlCVV2Fld;
	CEdit	m_CntlResponseCodeFld;
	CEdit	m_CntlAuthNumberFld;
	CEdit	m_CntlDeferredPurchaseTermFld;
	CEdit	m_CntlMerchantIDFld;
	CEdit	m_CntlExpiryDateFld;
	CEdit	m_CntlAmountFld;
	CEdit	m_CntlAccountNumberFld;
	CString	m_StrAccountNumberFld;
	CString	m_StrAmountFld;
	CString	m_StrAuthNumberFld;
	CString	m_StrDeferredPurchaseTermFld;
	CString	m_StrExpiryDateFld;
	CString	m_StrMerchantIDFld;
	CString	m_StrMessageLbl;
	CString	m_StrResponseCodeFld;
	CString	m_StrTransactionType;
	CString	m_StrCVV2Fld;
	//}}AFX_DATA

	BOOL CheckDialogForCompleteness();
	BOOL ValidateDate( CString );
	int m_nDialogMode;


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CVoiceAuthInput)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CVoiceAuthInput)
	virtual BOOL OnInitDialog();
	afx_msg void OnProcessAuthButton();
	afx_msg void OnSelchangeTransactionType();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
