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
* $Log:   N:\PVCS\PTE\Equitable\ATP_gui\TransactionDetails.h  $
   
      Rev 1.8   Dec 01 2003 11:30:20   lmitchel
   Added AMEX Online Updates - scr1200
   
      Rev 1.7   Jan 05 2000 14:54:30   gbarnes
   Major update. Not updated to PVCS recently.
   PVCS was locked.
   
      Rev 1.1   Sep 22 1999 16:01:38   iarustam
   Added manual batch entry GUI
   
      Rev 1.0   Jul 01 1999 14:18:26   iarustam
   Initial Release
   
      Rev 1.0   Jun 28 1999 13:59:34   iarustam
   Initial Release
   
      Rev 1.4   Jun 02 1999 16:25:56   iarustam
   Bug fixes
   
      Rev 1.3   May 13 1999 14:58:08   iarustam
   Bug fixes
   
      Rev 1.2   Apr 29 1999 14:57:10   iarustam
   Bug fixes
   
      Rev 1.1   Apr 15 1999 14:19:38   iarustam
   Bug fixes
   
   
      Rev 1.0   Feb 23 1999 11:41:24   iarustam
   Initial Release
   
      Rev 1.0   Feb 17 1999 10:10:12   iarustam
   Initial Release
   
      Rev 1.1   Dec 18 1998 13:45:00   iarustam
    
*
************************************************************************************/
// TransactionDetails.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CTransactionDetails dialog

class CTransactionDetails : public CDialog
{
// Construction
public:
	CTransactionDetails(CWnd* pParent = NULL);   // standard constructor

	int m_nDialogMode;

	TLF01 sTranRecord;
	BOOL m_bTran;
	BOOL m_bTran_stip;
	BOOL m_bMaskCard;

	BOOL PopulateTranDialogFromDatabase ( TLF01 );
	void CleanTranDialog();

	CString m_strTranID;
	CTime c_time;
// Dialog Data
	//{{AFX_DATA(CTransactionDetails)
	enum { IDD = IDD_TRANDETAILS };
	CString	m_StrHostTimeFld;
	CString	m_StrAcquiringInstIDFld;
	CString	m_StrAuthIDResponseFld;
	CString	m_StrCardNumberFld;
	CString	m_StrForwardingInstIDFld;
	CString	m_StrInvoiceNumberFld;
	CString	m_StrMerchantIDFld;
	CString	m_StrMessageLbl;
	CString	m_StrPOSConditionCodeFld;
	CString	m_StrPOSEntryModeFld;
	CString	m_StrProcessingCodeFld;
	CString	m_StrResponseTextFld;
	CString	m_StrRetrievalRefNumFld;
	CString	m_StrSystemTraceNumberFld;
	CString	m_StrTerminalIDFld;
	CString	m_StrTranAmountFld;
	CString	m_StrTranDateFld;
	CString	m_StrTranTimeFld;
	CString	m_StrTranTypeFld;
	CString	m_StrResponseCodeFld;
	CString	m_StrTextFld;
	CString	m_StrExpirationDateFld;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTransactionDetails)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CTransactionDetails)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
