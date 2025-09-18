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
* $Log:   N:\PVCS\PTE\Equitable\ATP_gui\CashBonus.h  $
   
      Rev 1.7   Dec 01 2003 11:29:02   lmitchel
   Added AMEX Online Updates - scr1200
   
      Rev 1.6   Jan 05 2000 14:53:14   gbarnes
   Major update. Not updated to PVCS recently.
   PVCS was locked.
   
      Rev 1.1   Sep 22 1999 15:46:54   iarustam
   Added manual batch entry GUI
   
      Rev 1.0   Jul 01 1999 14:17:12   iarustam
   Initial Release
   
      Rev 1.0   Jun 28 1999 13:58:18   iarustam
   Initial Release
   
      Rev 1.3   Jun 11 1999 14:07:30   iarustam
   Bug fixes
   
   
      Rev 1.2   May 10 1999 11:10:44   dperson
   Added PVCS header
*   
************************************************************************************/
// CashBonus.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CCashBonus dialog

class CCashBonus : public CDialog

//class CCashBonus : public CPropertyPage
{
//	DECLARE_DYNCREATE(CCashBonus)
public:
	CCashBonus(CWnd* pParent = NULL);   // standard constructor

// Construction

	//CCashBonus();
	//~CCashBonus();

	int m_nDialogMode;
	BOOL m_bFirstRecord;
	BON01 sBONRecordOld;	

	BOOL CheckDialogForCompleteness();
	BOOL ValidateDate ( CString );
	BOOL m_bValidDay;
	CTime c_time;

// Dialog Data
	//{{AFX_DATA(CCashBonus)
	enum { IDD = IDD_BONUS_DEFINITION };
	CButton	m_AddButton;
	CEdit	m_CntlThresholdFld;
	CEdit	m_CntlStartDateFld;
	CEdit	m_CntlRewardFld;
	CEdit	m_CntlResponseFld;
	CEdit	m_CntlMinAmountFld;
	CEdit	m_CntlEndDateFld;
	CEdit	m_CntlCounterFld;
	CString	m_StrCounterFld;
	CString	m_StrEndDateFld;
	CString	m_StrMinAmountFld;
	CString	m_StrResponseFld;
	CString	m_StrRewardFld;
	CString	m_StrStartDateFld;
	CString	m_StrThresholdFld;
	CString	m_StrMessageLbl;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CCashBonus)
	public:
	virtual void OnCancel();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CCashBonus)
	virtual BOOL OnInitDialog();
	afx_msg void OnAddButton();
	afx_msg void OnCanButton();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};
