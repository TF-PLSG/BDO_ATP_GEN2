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
* $Log:   N:\PVCS\PTE\Equitable\ATP_gui\CFCardBalAcc.h  $
   
      Rev 1.6   Dec 01 2003 11:29:04   lmitchel
   Added AMEX Online Updates - scr1200
   
      Rev 1.5   Jan 25 2000 14:01:16   iarustam
   bug fixes
   
      Rev 1.4   Jan 05 2000 14:52:58   gbarnes
   Major update. Not updated to PVCS recently.
   PVCS was locked.
   
      Rev 1.1   Sep 22 1999 15:42:44   iarustam
   Added manual batch entry GUI
   
      Rev 1.0   Jul 01 1999 14:16:52   iarustam
   Initial Release
   
      Rev 1.0   Jun 28 1999 13:57:56   iarustam
   Initial Release
   
      Rev 1.1   Apr 29 1999 14:55:52   iarustam
   Bug fixes
   
      Rev 1.0   Feb 23 1999 11:40:00   iarustam
   Initial Release
   
      Rev 1.0   Feb 17 1999 10:09:02   iarustam
   Initial Release
   
      Rev 1.1   Jan 22 1999 10:57:40   iarustam
   Bug fixes and core 1_3_0 changes
   
      Rev 1.1   Jan 21 1999 13:06:32   iarustam
   Bug fixes
   
   
      Rev 1.1   Dec 18 1998 13:42:22   iarustam
    
*
************************************************************************************/
// CFCardBalAcc.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CCFCardBalAcc dialog
#ifndef BALPAGE
#define BALPAGE

class CCFCardBalAcc : public CPropertyPage
{
	DECLARE_DYNCREATE(CCFCardBalAcc)

// Construction
public:
	CCFCardBalAcc();
	~CCFCardBalAcc();

	int DialogMode;

	CTime c_time;	
// Dialog Data
	//{{AFX_DATA(CCFCardBalAcc)
	enum { IDD = IDD_CCFCARDBAL };
	CComboBox	m_ctrlType1;
	CComboBox	m_ctrlType9;
	CComboBox	m_ctrlType8;
	CComboBox	m_ctrlType7;
	CComboBox	m_ctrlType6;
	CComboBox	m_ctrlType5;
	CComboBox	m_ctrlType4;
	CComboBox	m_ctrlType3;
	CComboBox	m_ctrlType2;
	CComboBox	m_ctrlType10;
	CEdit	m_ctrlNbr9;
	CEdit	m_ctrlNbr8;
	CEdit	m_ctrlNbr7;
	CEdit	m_ctrlNbr6;
	CEdit	m_ctrlNbr5;
	CEdit	m_ctrlNbr4;
	CEdit	m_ctrlNbr3;
	CEdit	m_ctrlNbr2;
	CEdit	m_ctrlNbr10;
	CEdit	m_ctrlNbr1;
	CString	m_strNbr1;
	CString	m_strNbr10;
	CString	m_strNbr2;
	CString	m_strNbr3;
	CString	m_strNbr4;
	CString	m_strNbr5;
	CString	m_strNbr6;
	CString	m_strNbr7;
	CString	m_strNbr8;
	CString	m_strNbr9;
	CString	m_strType10;
	CString	m_strType2;
	CString	m_strType3;
	CString	m_strType4;
	CString	m_strType5;
	CString	m_strType6;
	CString	m_strType7;
	CString	m_strType8;
	CString	m_strType9;
	CString	m_strCardType;
	CString	m_strCardNbr;
	CString	m_strComEdit;
	CString	m_strType1;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CCFCardBalAcc)
	public:
	virtual BOOL OnSetActive();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CCFCardBalAcc)
	virtual BOOL OnInitDialog();
	afx_msg void OnChangeNbr1();
	afx_msg void OnChangeNbr10();
	afx_msg void OnChangeNbr2();
	afx_msg void OnChangeNbr3();
	afx_msg void OnChangeNbr4();
	afx_msg void OnChangeNbr5();
	afx_msg void OnChangeNbr6();
	afx_msg void OnChangeNbr7();
	afx_msg void OnChangeNbr8();
	afx_msg void OnChangeNbr9();
	afx_msg void OnSelchangeType1();
	afx_msg void OnSelchangeType10();
	afx_msg void OnSelchangeType2();
	afx_msg void OnSelchangeType3();
	afx_msg void OnSelchangeType4();
	afx_msg void OnSelchangeType5();
	afx_msg void OnSelchangeType6();
	afx_msg void OnSelchangeType7();
	afx_msg void OnSelchangeType8();
	afx_msg void OnSelchangeType9();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

#endif