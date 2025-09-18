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
* $Log:   N:\PVCS\PTE\Equitable\ATP_gui\DeferredGeneral.h  $
   
      Rev 1.6   Dec 01 2003 11:29:16   lmitchel
   Added AMEX Online Updates - scr1200
   
      Rev 1.5   Jan 05 2000 14:53:30   gbarnes
   Major update. Not updated to PVCS recently.
   PVCS was locked.
   
      Rev 1.1   Sep 22 1999 15:48:34   iarustam
   Added manual batch entry GUI
   
      Rev 1.0   Jul 01 1999 14:17:20   iarustam
   Initial Release
   
      Rev 1.0   Jun 28 1999 13:58:26   iarustam
   Initial Release
   
      Rev 1.2   May 10 1999 11:17:04   dperson
    
*   
************************************************************************************/
// DeferredGeneral.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDeferredGeneral dialog
#ifndef DEFERREDGEN
#define DEFERREDGEN


class CDeferredGeneral : public CPropertyPage
{
	DECLARE_DYNCREATE(CDeferredGeneral)

// Construction
public:
	CDeferredGeneral();
	~CDeferredGeneral();

	int m_nDialogMode;
	BOOL m_bGeneral;
	BOOL CompareStructures();

	char m_strCurrentData[151];
	char m_strPreviousData[151];
	char m_strFieldTag[50];


	void CleanGeneralDialog();
	BOOL PopulateGeneralDialogFromDatabase ( DEF01 );
	BOOL PopulateDatabaseFromGeneralDialog ( pDEF01 );
	void PopulateHistoryTable();

	DEF01 sDEFRecordOld;
	
	CTime c_time;


// Dialog Data
	//{{AFX_DATA(CDeferredGeneral)
	enum { IDD = IDD_DEFERRED_GENERAL };
	CEdit	m_CntlMinAmountFld;
	CEdit	m_CntlMaxAmountFld;
	CEdit	m_CntlFactorFld;
	CString	m_StrFactorFld;
	CString	m_StrMaxAmountFld;
	CString	m_StrMerchantIDFld;
	CString	m_StrMessageLbl;
	CString	m_StrMinAmountFld;
	CString	m_StrOrganizationIDFld;
	CString	m_StrTermLengthFld;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CDeferredGeneral)
	public:
	virtual BOOL OnSetActive();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CDeferredGeneral)
	virtual BOOL OnInitDialog();
	afx_msg void OnChangeFactorFld();
	afx_msg void OnChangeMaxAmountFld();
	afx_msg void OnChangeMinAmountFld();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};
#endif