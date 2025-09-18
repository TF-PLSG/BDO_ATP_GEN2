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
* $Log:   N:\PVCS\PTE\Equitable\ATP_gui\MerchantBlocking.h  $
   
      Rev 1.8   Dec 01 2003 11:29:32   lmitchel
   Added AMEX Online Updates - scr1200
   
      Rev 1.7   Jan 13 2000 17:07:46   mbysani
   bug fixed
   
      Rev 1.1   Sep 22 1999 15:51:54   iarustam
   Added manual batch entry GUI
   
      Rev 1.0   Jul 01 1999 14:17:40   iarustam
   Initial Release
   
      Rev 1.0   Jun 28 1999 13:58:42   iarustam
   Initial Release
   
      Rev 1.3   Jun 04 1999 17:40:28   iarustam
   Bug fixes
   
      Rev 1.2   Apr 29 1999 14:56:30   iarustam
   Bug fixes
   
      Rev 1.1   Apr 22 1999 15:34:12   iarustam
   Bug fixes
   
      Rev 1.0   Feb 23 1999 11:40:36   iarustam
   Initial Release
   
      Rev 1.0   Feb 17 1999 10:09:32   iarustam
   Initial Release
   
      Rev 1.1   Jan 22 1999 10:59:02   iarustam
   Bug fixes and core 1_3_0 changes
   
      Rev 1.1   Jan 21 1999 13:08:22   iarustam
   Bug fixes
   
   
      Rev 1.1   Dec 18 1998 13:43:32   iarustam
    
*
************************************************************************************/
// MerchantBlocking.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CMerchantBlocking dialog
#ifndef BLOCKING
#define BLOCKING

class CMerchantBlocking : public CPropertyPage
{
	DECLARE_DYNCREATE(CMerchantBlocking)

// Construction
public:
	CMerchantBlocking();
	~CMerchantBlocking();

	void EnableBlockingDialog ( int );
	BOOL PopulateBlockingDialogFromDatabase ( MCF01 );
	BOOL PopulateDatabaseFromBlockingDialog ( pMCF01 );
	void CleanBlockingDialog();
	void PopulateHistoryTable();
	BOOL CompareStructures();
	BOOL ValidateDate ( CString );
	BOOL ValidateDates();

	BOOL m_bBlocking;
	int m_nDialogMode;
	MCF01 sMerchantRecordOld;
//	MCF01 sMerchantRecordNew;

	char m_strCurrentData[151];
	char m_strPreviousData[151];
	char m_strFieldTag[50];

	int nStartDay;
	int nStartMonth;
	int nStartYear;

		int nDay;
	int nMonth;
	int nYear;
	BOOL m_bValidStartDay;
	BOOL m_bValidEndDay;
   BOOL m_bValidOrder;
	CTime c_time;

// Dialog Data
	//{{AFX_DATA(CMerchantBlocking)
	enum { IDD = IDD_MERCHANT_BLOCKING };
	CEdit	m_CntlBlockingStartFld;
	CEdit	m_CntlBlockingEndFld;
	CString	m_StrBlockingEndFld;
	CString	m_StrBlockingStartFld;
	CString	m_StrMerchantIDFld;
	CString	m_StrMessageLbl;
	CString	m_StrOrganizationIDFld;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CMerchantBlocking)
	public:
	virtual BOOL OnSetActive();
	virtual BOOL OnApply();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CMerchantBlocking)
	virtual BOOL OnInitDialog();
	afx_msg void OnKillfocusBlockingStartFld();
	afx_msg void OnKillfocusBlockingEndFld();
	afx_msg void OnPaint();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};
#endif