
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
* $Log:   N:\PVCS\PTE\Equitable\tran_lookup\TranInquiry.h  $
   
      Rev 1.0   Jan 25 2011  16:55:18   TF-Sriaknth
   Added Idle Time of TranInquiry
   
     
*
************************************************************************************/


// TranInquiry.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CTranInquiry dialog
#include "ListCtrlEx.h"
#include "Lock.h"
#include "CVector.h"

class CTranInquiry : public CDialog
{
// Construction
public:
	CTranInquiry(CWnd* pParent = NULL);   // standard constructor

	~CTranInquiry();

#define IDT_TIMER WM_USER + 200
#define CHECKED   1
#define UNCHECKED 0

	CVector<CString> OriginalCardNoList;

	TLF01_LIMITS sTLFLimRecord;
	TLF01 sTLFRecord;
	TLF01_GUI_LIST sTLFList;

	FGUARD sFGuardRecord;
	FGUARD_GUI_LIST sFGuardList;

	BOOL bDone;
	BOOL ValidateTime ( CString );
	BOOL ValidateDate ( CString );

// Dialog Data
	//{{AFX_DATA(CTranInquiry)
	enum { IDD = IDD_TRAN_INQUIRY };
	CButton	m_MaskCardNumber;
	CButton	m_CntlPosSearchCheck;
	//ramya
	CButton	m_CntlSTIPSearchCheck;
	CButton	m_CntlFGuardSearchCheck;
	CButton	m_CntlEmvRawRadio;
	CButton	m_CntlViewFailedRadio;
	CButton	m_CntlViewAcceptedRadio;
	CButton	m_CntlViewFGRawRadio;
	CButton	m_CntlViewAllRadio;
	CButton	m_CntlFGuardGroup;
	CButton	m_CntlResendButton;
	CButton	m_NextButton;
	CButton	m_InquiryButton;
	CButton	m_TlfExtractButton;
	CButton	m_DetailsButton;
	CEdit	m_CntlReversalAmountFld;
	CComboBox	m_CntlIssuerIDFld;
	CComboBox	m_CntlAcquirerIDFld;
	CEdit	m_CntlSTANFld;
	CEdit	m_CntlRRNFld;
	CEdit	m_CntlTimeStartFld;
	CEdit	m_CntlTimeEndFld;
	CEdit	m_CntlDateStartFld;
	CEdit	m_CntlDateEndFld;
	CEdit	m_CntlMerchantIDFld;
	CListCtrlEx	m_CntlList;
	CEdit	m_CntlDeviceIDFld;
	CEdit	m_CntlResponseCodeFld;
	CEdit	m_CntlAmountFld;
	CEdit	m_CntlCardNumberFld;
	CEdit	m_CntlAuthNumberFld;
	CEdit   m_CntlHostNameFld;
	CStatic			m_CntlStatus;
	CString			m_StrStatus;
	CString m_StrHostNameFld;
	CString m_StrAuthNumberFld;
	CString	m_StrCardNumberFld;
	CString	m_StrAmountFld;
	CString	m_StrResponseCodeFld;
	CString	m_StrResponseCodeFldbyScheme;
	CString	m_StrDeviceIDFld;
	CString	m_StrMerchantIDFld;
	CString	m_StrMessageLbl;
	CString	m_StrDateEndFld;
	CString	m_StrTimeEndFld;
	CString	m_StrTimeStartFld;
	CString	m_StrRRNFld;
	CString	m_StrSTANFld;
	CString	m_StrAcquirerIDFld;
	CString	m_StrIssuerIDFld;
	CString	m_StrDateStartFld;
	CString	m_StrReversalAmountFld;
	BOOL	m_bViewEmvData;
	BOOL    m_bViewRawData;
	int		m_StrViewAllRadio;
	int		m_StrViewFGRawRadio;
	int		m_StrViewAcceptedRadio;
	int		m_StrViewFailedRadio;
	int		m_StrEmvRawRadio;
	BOOL	m_StrFGuardSearchCheck;
	BOOL	m_StrPosSearchCheck;
	//ramya
	BOOL	m_StrSTIPSearchCheck;
	//}}AFX_DATA

	void PopulateTranListDialog();
	void PopulateTLF01_STIPListDialog();
	void PopulateFGuardListDialog();

	UINT timer_handle;
	int cntr;
	int idle_time;
	char idle_time_val[7];
	bool StopTimer();
	int StartTimer(int time);
	void DisableEditControls();
	void EnableEditControls();
	CTime c_time, shm_time;
	int ret;

	short LockScreen();
	void ResetCntr();
	int status;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTranInquiry)
	public:
	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CTranInquiry)
	afx_msg void OnInquiryButton();
	afx_msg void OnNextButton();
	afx_msg void OnDetailsButton();
	virtual BOOL OnInitDialog();
	afx_msg void OnDblclkList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnEMVRAWRadio();
	afx_msg void OnPosSearchCheck();
	
	//ramya
	afx_msg void OnTLF01STIPSearchCheck();

	afx_msg void OnFguardSearchCheck();
	afx_msg void OnViewFgrawRadio();
	afx_msg void OnViewAllradio();
	afx_msg void OnViewAcceptedRadio();
	afx_msg void OnViewFailedRadio();
	afx_msg void OnResendButton();
	afx_msg void OnExit();
	afx_msg void OnButtonExit();
	afx_msg void OnMaskCardNumberChk();
	afx_msg void OnBnClickedTlfExtract();
	 void get_time( pCHAR time_date, pCHAR time_str );
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP();

	CDMLock * pLock;

	



	
};
