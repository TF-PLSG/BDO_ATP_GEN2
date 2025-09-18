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
* $Log:   N:\PVCS\PTE\Equitable\tran_lookup\VoiceAuth.h  $

  Rev 1.0   Jan 25 2011  16:55:18   TF-Sriaknth
  Added Idle Time of VoiceAuthorisation module
  
	
	  *
************************************************************************************/

#if !defined(AFX_VOICEAUTH_H__20C1140A_714A_11D3_9764_0008C789BA2A__INCLUDED_)
#define AFX_VOICEAUTH_H__20C1140A_714A_11D3_9764_0008C789BA2A__INCLUDED_
#define  IDT_TIMER  WM_USER + 200
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// VoiceAuth.h : header file
//
#include "Lock.h"
/////////////////////////////////////////////////////////////////////////////
// CVoiceAuth dialog

class CVoiceAuth : public CDialog
{
	// Construction
public:
	CVoiceAuth(CWnd* pParent = NULL);   // standard constructor
	virtual ~CVoiceAuth();	
	
	int m_nDialogMode;
	BOOL m_bMCFRecordNotFound;
	BOOL m_bDCFRecordNotFound;
	MCF01 sMCFRecord;
	DCF01 sDCFRecord;
	AUTH_TX sAuthTxRecord;
	AUTH_TX sAuthTxRecordTemp;
	CCF02P  sCCF02PRecord;
	CCF01 sCCFRecord;
	BCH20 sBCH20Record;
	TLF01 sTLF01Record;
	NCF30 sNCF30Record;
	BIN01 sBIN01Record;
	CAF01 sCAF01Record;
	CSF01 sCSF01Record;
	MCT01 sMCT01Record;
	
	INT m_disposition;
	BOOL m_bEnableOverrideDenial;
	
	
	BOOL CheckDialogForCompleteness();
	BOOL ValidateDate ( CString );
	BOOL PopulateTLFStructure(); 
	BOOL SendVoiceAuthTransaction();
	BOOL GetMCFandDCFStructures();
	BOOL GetDCFStructure();
	BOOL GetMCFStructure();
	BOOL ValidateCardType();
	BOOL GetDEFStructure();
	BOOL GetCCFStructure();
	BOOL GetBINStructure();
	BOOL GetCAFStructure( pCHAR );
	BOOL GetCCF02PStructure();
	BOOL GetCSF01Structure();
	BOOL GetCardStatus();
	void PopulateDialog();
	BYTE CalculateGrossAmount ( double*  );
	BYTE CalculateMonthlyAmount ( double*  );
	BOOL CheckBCH20();
	BOOL CheckNCF30();
	BOOL CheckTLF01();
	BOOL ValidateCardNumber ( CString );
	CString GetSystemDate ();
	BOOL GetOrigTxn();
	void DisplayErrorMsg( pCHAR );
	void checkTLF01_fields( pCHAR );
	void log_auth_tx( void );
	void asc_to_bin( pCHAR astr, pBYTE bstr, pINT bstr_len );
	
	/* Authorization location in CAF01 */
#define REMOTE_AUTH_LOC   'R'
#define LOCAL_AUTH_LOC    'L'
	
	/* Authorization Destination */
#define REMOTE_DEST  "RA"
	
	/* Authorization Type in BIN01 */
#define REMOTE   '0'
#define LOCAL    '1'
	
	/* Off-Us MasterCard Network Controller */
#define OFF_US_MASTERCARD   "mcrd"
#define OFF_US_MASTERCARD2   "mcr2"
	
	/* Off-Us American Express Network Controllers */
#define OFF_US_AMEX_POS   "posa"
#define OFF_US_AMEX_ATM   "atma"
#define OFF_US_AMEX       "amaq"  /* For our testing purposes */
	
	/* Visa Electron */
#define VE_BIN   "455968"
	
    UINT timer_handle;
	int cntr;
	int idle_time;
	char idle_time_val[7];
	char char_time[20];	
	CTime c_time, shm_time;
	int ret;
	HWND p_hwnd;
	int status;	
	
	bool StopTimer();
	int StartTimer(int time);
	void ResetCntr();
	short LockScreen();
	
	// Dialog Data
	//{{AFX_DATA(CVoiceAuth)
	enum { IDD = IDD_VOICE_AUTH };
	CButton	m_imput_groupbox;
	CEdit	m_CntlIndustryCodeFld;
	CEdit	m_CntlIndustryCode;
	CButton	m_CntlMod10CheckButton;
	CButton	m_VerifyButton;
	CEdit	m_CntlCurrencyCodeFld;
	CEdit	m_CntlAuthNumberFld;
	CComboBox	m_CntlTransactionTypeFld;
	CEdit	m_CntlTextFld;
	CButton	m_ProcessButton;
	CButton	m_OverrideButton;
	CEdit	m_CntlOrganizationIDFld;
	CButton	m_OKButton;
	CEdit	m_CntlMerchantIDFld;
	CEdit	m_CntlMCCFld;
	CEdit	m_CntlExpiryDateFld;
	CEdit	m_CntlDeferredTermFld;
	CEdit	m_CntlCVV2Fld;
	CButton	m_ClearButton;
	CEdit	m_CntlAmountFld;
	CEdit	m_CntlAccountNumberFld;
	CString	m_StrAccountNumberFld;
	CString	m_StrAmountFld;
	CString	m_StrAuthNumOutputFld;
	CString	m_StrAuthNumberFld;
	CString	m_StrBankNameFld;
	CString	m_StrCardholderNameFld;
	CString	m_StrCurrencyCodeFld;
	CString	m_StrCVV2Fld;
	CString	m_StrDeferredTermFld;
	CString	m_StrExpiryDateFld;
	CString	m_StrGrossAmountFld;
	CString	m_StrIndustryCode;
	CString	m_StrMCCFld;
	CString	m_StrMerchantIDFld;
	CString	m_StrMerchantNameFld;
	CString	m_StrMessageLbl;
	CString	m_StrMonthlyAmountFld;
	CString	m_StrOrganizationIDFld;
	CString	m_StrResponseCodeFld;
	CString	m_StrResponseTextFld;
	CString	m_StrTextFld;
	CString	m_StrTransactionTypeFld;
	BOOL	m_BoolMod10Check;
	CString	m_StrIndustryCodeFld;
	CString	m_StrCardStatusFld;
	//}}AFX_DATA
	
	
	
	
	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CVoiceAuth)
	protected:
		virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
		virtual LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);
		virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
		//}}AFX_VIRTUAL
		
		// Implementation
	protected:
		
		// Generated message map functions
		//{{AFX_MSG(CVoiceAuth)
		virtual BOOL OnInitDialog();
		afx_msg void OnOk1();
		afx_msg void OnOverrideDenialButton();
		afx_msg void OnProcessAuthButton();
		afx_msg void OnClearButton();
		afx_msg void OnClose();
		virtual void OnOK();
		afx_msg void OnSelchangeTransactionType();
		afx_msg void OnVerifyButton();
		afx_msg void OnMod10CheckButton();
		afx_msg void OnChangeAccountNumberFld();
		afx_msg void OnChangeMerchantIdFld();
		afx_msg void OnChangeCVV2Fld();
		afx_msg void OnCancelMode();
		/*afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
		afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
		afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
		afx_msg void OnMove(int x, int y);
		afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
		afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
		afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
		afx_msg void OnTCard(UINT idAction, DWORD dwActionData);
		afx_msg void OnSize(UINT nType, int cx, int cy);*/
		//}}AFX_MSG
		DECLARE_MESSAGE_MAP();
		
		CDMLock * pLock;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VOICEAUTH_H__20C1140A_714A_11D3_9764_0008C789BA2A__INCLUDED_)
