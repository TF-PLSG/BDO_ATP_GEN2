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
* $Log:   N:\PVCS\PTE\Equitable\ATP_gui\MCC103.h  $
   
      Rev 1.10   Dec 01 2003 11:29:32   lmitchel
   Added AMEX Online Updates - scr1200
   
      Rev 1.9   Mar 03 2003 11:40:24   dirby
   Modified for the new fields on the GUI, such as check digit
   and Account Results and Error Results.
   SCR 891
   
      Rev 1.8   Feb 14 2000 13:18:06   iarustam
   SCR # 398
   
      Rev 1.7   Jan 05 2000 14:53:44   gbarnes
   Major update. Not updated to PVCS recently.
   PVCS was locked.
   
      Rev 1.1   Sep 22 1999 15:51:14   iarustam
   Added manual batch entry GUI
   
      Rev 1.0   Jul 01 1999 14:17:36   iarustam
   Initial Release
   
      Rev 1.0   Jun 28 1999 13:58:40   iarustam
   Initial Release
   
      Rev 1.4   Jun 11 1999 14:07:44   iarustam
   Bug fixes
   
   
      Rev 1.3   May 10 1999 13:38:50   dperson
   Updated comments
   
      Rev 1.2   May 10 1999 11:21:58   dperson
	Added PVCS header
    
*   
************************************************************************************/
// MCC103.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CMCC103 dialog

class CMCC103 : public CDialog
{
// Construction
public:
	CMCC103(CWnd* pParent = NULL);   // standard constructor
	CTime c_time;

// Dialog Data
	//{{AFX_DATA(CMCC103)
	enum { IDD = IDD_MCC103 };
	CListBox	m_cntlAccountResults;
	CButton	m_cntlCkDigit;
	CListBox	m_cntlErrorResults;
	CEdit	m_cntlRespCode;
	CButton	m_InquiryButton;
	CEdit	m_CntlPurgeDateFld6;
	CButton	m_CntlAfrica;
	CButton	m_CntlUSA;
	CButton	m_CntlLatin;
	CButton	m_CntlEurope;
	CButton	m_CntlCanada;
	CButton	m_CntlAsia;
	CButton	m_DeleteButton;
	CButton	m_AddButton;
	CComboBox	m_CntlProductCode;
	CComboBox	m_CntlEntryReason;
	CEdit	m_CntlAccountNumberFld;
	CEdit	m_CntlPurgeDateFld5;
	CEdit	m_CntlPurgeDateFld4;
	CEdit	m_CntlPurgeDateFld3;
	CEdit	m_CntlPurgeDateFld2;
	CEdit	m_CntlPurgeDateFld;
	CString	m_StrEntryReason;
	CString	m_StrMessageLbl;
	CString	m_StrProductCode;
	CString	m_StrPurgeDateFld;
	BOOL	m_Asia;
	BOOL	m_Canada;
	BOOL	m_Europe;
	BOOL	m_Latin;
	BOOL	m_USA;
	CString	m_StrPurgeDateFld2;
	CString	m_StrPurgeDateFld3;
	CString	m_StrPurgeDateFld4;
	CString	m_StrPurgeDateFld5;
	CString	m_StrAccountNumberFld;
	BOOL	m_Africa;
	CString	m_StrPurgeDateFld6;
	CString	m_strRespCode;
	CString	m_strErrorResults;
	BOOL	m_CkDigit;
	CString	m_strAccountResults;
	//}}AFX_DATA

	int m_nDialogMode;
	BOOL PopulateDatabaseStructure();
	BOOL CheckDialogForCompleteness();
	BOOL ValidateDate ( CString );
   void handle_response();
   void change_date_yymmdd_to_ddmmyy( pCHAR date_buf );

   
   MCC_ONLINE_UPDATES  Mcc_Gui;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMCC103)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CMCC103)
	virtual BOOL OnInitDialog();
	afx_msg void OnAddButton();
	afx_msg void OnDeleteButton();
	afx_msg void OnAsia();
	afx_msg void OnCanada();
	afx_msg void OnEurope();
	afx_msg void OnLatin();
	afx_msg void OnUsa();
	afx_msg void OnAfrica();
	afx_msg void OnInquireButton();
	afx_msg void OnExit();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
