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
* $Log:   N:\PVCS\PTE\Equitable\ATP_gui\NegativeCardFileControl.h  $
   
      Rev 1.6   Dec 01 2003 11:29:50   lmitchel
   Added AMEX Online Updates - scr1200
   
      Rev 1.5   Jan 05 2000 14:54:06   gbarnes
   Major update. Not updated to PVCS recently.
   PVCS was locked.
   
      Rev 1.1   Sep 22 1999 15:55:54   iarustam
   Added manual batch entry GUI
   
      Rev 1.0   Jul 01 1999 14:17:56   iarustam
   Initial Release
   
      Rev 1.0   Jun 28 1999 13:59:04   iarustam
   Initial Release
   
      Rev 1.2   May 10 1999 11:28:36   dperson
   Added PVCS header
*   
************************************************************************************/
// NegativeCardFileControl.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CNegativeCardFileControl dialog
#include "ChangeHistory.h"

class CNegativeCardFileControl : public CPropertyPage
{
	DECLARE_DYNCREATE(CNegativeCardFileControl)

// Construction
public:
	CNegativeCardFileControl();
	~CNegativeCardFileControl();

	int m_nDialogMode;
	BOOL m_bFirstRecord;
	NFI02 sNegFileRecordOld;

	
	CChangeHistory *pChangeHistoryPage;

	BOOL CheckDialogForCompleteness();
	BOOL ValidateDate ( CString );
	void PopulateHistoryTable();
	BOOL m_bValidDay;

	char m_strCurrentData[151];
	char m_strPreviousData[151];
	char m_strFieldTag[60];
	int nStartDay;
	int nDay;
	int nStartMonth;
	int nMonth;
	int nStartYear;
	int nYear;
	CTime c_time;


// Dialog Data
	//{{AFX_DATA(CNegativeCardFileControl)
	enum { IDD = IDD_NEG_CARD_FILE_CONTROL };
	CEdit	m_CntlFileNameFld;
	CEdit	m_CntlStartFld;
	CEdit	m_CntlEndFld;
	CButton	m_Button;
	CString	m_StrEndFld;
	CString	m_StrFileNameFld;
	CString	m_StrMessageLbl;
	CString	m_StrStartFld;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CNegativeCardFileControl)
	public:
	virtual BOOL OnApply();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CNegativeCardFileControl)
	virtual BOOL OnInitDialog();
	afx_msg void OnButton();
	afx_msg void OnUpdateFileNameFld();
	afx_msg void OnUpdateStartFld();
	afx_msg void OnUpdateEndFld();
	afx_msg void OnKillfocusStartFld();
	afx_msg void OnKillfocusEndFld();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};
