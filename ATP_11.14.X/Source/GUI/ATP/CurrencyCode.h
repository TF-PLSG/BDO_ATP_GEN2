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
* $Log:   N:\PVCS\PTE\Equitable\ATP_gui\CurrencyCode.h  $
   
      Rev 1.5   Dec 01 2003 11:29:12   lmitchel
   Added AMEX Online Updates - scr1200
   
      Rev 1.4   Jan 05 2000 14:53:18   gbarnes
   Major update. Not updated to PVCS recently.
   PVCS was locked.
   
      Rev 1.1   Sep 22 1999 15:47:50   iarustam
   Added manual batch entry GUI
   
      Rev 1.0   Jul 01 1999 14:17:16   iarustam
   Initial Release
   
      Rev 1.0   Jun 28 1999 13:58:22   iarustam
   Initial Release
   
      Rev 1.1   Apr 29 1999 14:56:14   iarustam
   Bug fixes
   
      Rev 1.0   Feb 23 1999 11:40:18   iarustam
   Initial Release
   
      Rev 1.0   Feb 17 1999 10:09:22   iarustam
   Initial Release
   
      Rev 1.1   Jan 22 1999 10:58:32   iarustam
   Bug fixes and core 1_3_0 changes
   
      Rev 1.1   Jan 21 1999 13:07:44   iarustam
   Bug fixes
   
   
      Rev 1.1   Dec 18 1998 13:43:08   iarustam
    
*
************************************************************************************/
// CurrencyCode.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CCurrencyCode dialog
#include "Conversion.h"
#include "ChangeHistory.h"

class CCurrencyCode : public CPropertyPage
{
	DECLARE_DYNCREATE(CCurrencyCode)

// Construction
public:
	CCurrencyCode();
	~CCurrencyCode();

	BOOL PopulateCurrencyCodeDialogFromDatabase ( CRF01 );
    BOOL PopulateCurrencyCodeFromDialog ( pCRF01 );
	BOOL CheckDialogForCompleteness();   
	void PopulateHistoryTable();

	CConversion *pConversionRatePage;
	CChangeHistory *pChangeHistoryPage;

	int m_nDialogMode;
    int ReturnCode;
	char m_strCurrentData[151];
	char m_strPreviousData[151];
	char m_strFieldTag[50];


	CRF01 sCurrencyCodeRecordOld;
	
	CTime c_time;

// Dialog Data
	//{{AFX_DATA(CCurrencyCode)
	enum { IDD = IDD_CURRENCY_CODE };
	CButton	m_ViewButton;
	CButton	m_UpdateButton;
	CButton	m_ListButton;
	CButton	m_DeleteButton;
	CButton	m_AddButton;
	CEdit	m_CntlCurrencyCodeFld;
	CString	m_StrCurrencyCodeFld;
	CString	m_StrMessageLbl;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CCurrencyCode)
	public:
	virtual void OnCancel();
	virtual BOOL OnApply();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CCurrencyCode)
	afx_msg void OnAddButton();
	afx_msg void OnDeleteButton();
	afx_msg void OnListButton();
	afx_msg void OnUpdateButton();
	afx_msg void OnViewButton();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};
