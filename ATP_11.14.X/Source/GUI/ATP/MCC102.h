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
* $Log:   N:\PVCS\PTE\Equitable\ATP_gui\MCC102.h  $
   
      Rev 1.13   Dec 01 2003 11:29:30   lmitchel
   Added AMEX Online Updates - scr1200
   
      Rev 1.12   Jul 01 2003 10:36:58   lmitchel
   SCR1138:  Ver: 4.2.0.3: Enabled ADD, DELETE, UPDATE, INQUIRY access for Account File, Account Management File, ad JCB Online Updates when Update and View security permissions are enabled
   
      Rev 1.11   Mar 03 2003 11:41:38   dirby
   Modified for the new fields on the GUI, such as check digit
   and Account Results and Error Results.
   SCR 891
   
      Rev 1.10   Jun 21 2000 14:25:40   lmitchel
   Modifed AlphaLookup.cpp and repds to select and display chain name(mcf01.name02) in merchant lookup list.
   Added 'Update' button to account file screen(mcc102.cpp)
   
      Rev 1.9   Jan 05 2000 14:53:42   gbarnes
   Major update. Not updated to PVCS recently.
   PVCS was locked.
   
      Rev 1.1   Sep 22 1999 15:51:04   iarustam
   Added manual batch entry GUI
   
      Rev 1.0   Jul 01 1999 14:17:36   iarustam
   Initial Release
   
      Rev 1.0   Jun 28 1999 13:58:38   iarustam
   Initial Release
   
      Rev 1.5   Jun 17 1999 17:04:18   iarustam
   Bug fixes
   
      Rev 1.4   Jun 11 1999 14:07:44   iarustam
   Bug fixes
   
   
      Rev 1.3   May 10 1999 11:21:56   dperson
   Added PVCS header
*   
************************************************************************************/
// MCC102.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CMCC102 dialog

class CMCC102 : public CDialog
{
// Construction
public:
	CMCC102(CWnd* pParent = NULL);   // standard constructor

	BOOL PopulateDatabaseStructure();
	BOOL ValidateDate ( CString );
	BOOL ValidateTime ( CString );

	int m_nDialogMode;
	BOOL CheckDialogForCompleteness();
   void handle_response();

   MCC_ONLINE_UPDATES  Mcc_Gui;
   CTime c_time;

   
// Dialog Data
	//{{AFX_DATA(CMCC102)
	enum { IDD = IDD_MCC102 };
	CButton	m_UpdateButton;
	CListBox	m_cntlErrorResults;
	CListBox	m_cntlAccountResults;
	CButton	m_cntlCkDigit;
	CEdit	m_cntlRespCode;
	CButton	m_InquiryButton;
	CButton	m_AddButton;
	CButton	m_DeleteButton;
	CComboBox	m_CntlVIPCurrency;
	CEdit	m_CntlVIPAmountFld;
	CComboBox	m_CntlEntryReason;
	CEdit	m_CntlTimeFld;
	CEdit	m_CntlDateFld;
	CEdit	m_CntlAccountNumberFld;
	CString	m_StrAccountNumberFld;
	CString	m_StrDateFld;
	CString	m_StrEntryReason;
	CString	m_StrTimeFld;
	CString	m_StrVIPAmountFld;
	CString	m_StrVIPCurrencyFld;
	CString	m_StrMessageLbl;
	CString	m_strRespCode;
	BOOL	m_CkDigit;
	CString	m_strAccountResults;
	CString	m_strErrorResults;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMCC102)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CMCC102)
	virtual BOOL OnInitDialog();
	afx_msg void OnAddButton();
	afx_msg void OnDeleteButton();
	afx_msg void OnInquireButton();
	afx_msg void OnSelchangeEntryReason();
	afx_msg void OnUpdateButton();
	afx_msg void OnExit();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
