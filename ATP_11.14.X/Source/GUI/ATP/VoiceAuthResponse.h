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
* $Log:   N:\PVCS\PTE\Equitable\ATP_gui\VoiceAuthResponse.h  $
   
      Rev 1.8   Dec 01 2003 11:30:24   lmitchel
   Added AMEX Online Updates - scr1200
   
      Rev 1.7   Jan 05 2000 14:54:34   gbarnes
   Major update. Not updated to PVCS recently.
   PVCS was locked.
   
      Rev 1.1   Sep 22 1999 16:02:50   iarustam
   Added manual batch entry GUI
   
      Rev 1.0   Jul 01 1999 14:18:32   iarustam
   Initial Release
   
      Rev 1.0   Jun 28 1999 13:59:40   iarustam
   Initial Release
   
      Rev 1.4   May 10 1999 11:42:36   dperson
   Added PVCS header
*   
************************************************************************************/
// VoiceAuthResponse.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CVoiceAuthResponse dialog


class CVoiceAuthResponse : public CDialog
{
// Construction
public:
	CVoiceAuthResponse(CWnd* pParent = NULL);   // standard constructor


	AUTH_TX sAuthTxRecord;
	MCF01 sMCFRecord;
	DCF01 sDCFRecord;
	CCF01 sCCFRecord;


	BOOL GetMCFandDCFStructures();
	void PopulateDialog();
	void PupulateTLFStructure();
	void SendVoiceAuthTransaction();
	BYTE CalculateMonthlyAmount ( double*  );
	BYTE CalculateGrossAmount ( double* );


	int m_nDialogMode;
	BOOL m_bOverride;
	BOOL m_bTransaction;
	CTime c_time;
// Dialog Data
	//{{AFX_DATA(CVoiceAuthResponse)
	enum { IDD = IDD_VOICE_AUTH_RESPONSE };
	CString	m_StrAuthNumFld;
	CString	m_StrBankNameFld;
	CString	m_StrCardholderNameFld;
	CString	m_StrGrossAmountFld;
	CString	m_StrMerchantNameFld;
	CString	m_StrMessageLbl;
	CString	m_StrMonthlyAmountFld;
	CString	m_StrResponseCodeFld;
	CString	m_StrResponseTextFld;
	CString	m_StrTextFld;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CVoiceAuthResponse)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CVoiceAuthResponse)
	afx_msg void OnOverrideDenialButton();
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
