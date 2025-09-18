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
* $Log:   N:\POS\PVCS6.6\EPICPORTZ\PTE\Equitable\ATP_gui\PurchaseCardGeneral.h  $
   
      Rev 1.8   Apr 01 2005 10:14:08   lmitchel
   Initial Checking prior to integration testing for updates made to support country/curreny code limits (scr13929), unulimited purchase card MCC ranges, and purchase card transaction limits(scr12785)
   
      Rev 1.7   Dec 01 2003 11:30:06   lmitchel
   Added AMEX Online Updates - scr1200
   
      Rev 1.6   Jan 27 2000 13:06:38   iarustam
   SCR # 314,328,330,300,159
   
      Rev 1.5   Jan 05 2000 14:54:16   gbarnes
   Major update. Not updated to PVCS recently.
   PVCS was locked.
   
      Rev 1.1   Sep 22 1999 15:58:44   iarustam
   Added manual batch entry GUI
   
      Rev 1.0   Jul 01 1999 14:18:10   iarustam
   Initial Release
   
      Rev 1.0   Jun 28 1999 13:59:18   iarustam
   Initial Release
   
      Rev 1.2   May 10 1999 11:33:46   dperson
   Added PVCS header
*   
************************************************************************************/
// PurchaseCardGeneral.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CPurchaseCardGeneral dialog
#ifndef PURCHASE
#define PURCHASE

class CPurchaseCardGeneral : public CPropertyPage
{
	DECLARE_DYNCREATE(CPurchaseCardGeneral)

// Construction
public:
	CPurchaseCardGeneral();
	~CPurchaseCardGeneral();

#define MAX_CNT		500 /*max number of cntry codes supported*/

	int m_nDialogMode;
	BOOL m_bGeneral;
	BOOL CompareStructures();
	BOOL bDone;
	int m_nOldCount;

	char m_strCurrentData[151];
	char m_strPreviousData[151];
	char m_strFieldTag[50];


	void CleanGeneralDialog();
	BOOL PopulateGeneralDialogFromDatabase ( CCF04 );
	BOOL PopulateDatabaseFromGeneralDialog ( pCCF04 );
	void PopulateHistoryTable();
    BOOL CheckDialogForCompleteness();

	CCF04 sCCF04Record;
	CCF04 sCCF04RecordOld;

	CCF04_GUI_LIST	m_ccf04_gui_list;
	CCF04	        m_ccf04_limits;
	CTime c_time;


// Dialog Data
	//{{AFX_DATA(CPurchaseCardGeneral)
	enum { IDD = IDD_PURCHASE_CARD_GENERAL };
	CEdit	m_CntlLowFld;
	CListCtrl	m_List;
	CEdit	m_CntlHighFld;
	CButton	m_DeleteButton;
	CButton	m_AddButton;
	CString	m_StrCardNumberFld;
	CString	m_StrCardTypeFld;
	CString	m_StrHighFld;
	CString	m_StrLowFld;
	CString	m_StrMessageLbl;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CPurchaseCardGeneral)
	public:
	virtual BOOL OnSetActive();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CPurchaseCardGeneral)
	virtual BOOL OnInitDialog();
	afx_msg void OnAddButton();
	afx_msg void OnDeleteButton();
	afx_msg void OnClickList(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};
#endif
