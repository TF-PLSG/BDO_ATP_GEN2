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
* $Log:   N:\PVCS\PTE\Equitable\ATP_gui\CardPlusMain.h  $
   
      Rev 1.6   Dec 01 2003 11:28:50   lmitchel
   Added AMEX Online Updates - scr1200
   
      Rev 1.5   Jan 05 2000 14:53:04   gbarnes
   Major update. Not updated to PVCS recently.
   PVCS was locked.
   
      Rev 1.1   Sep 22 1999 15:44:24   iarustam
   Added manual batch entry GUI
   
      Rev 1.0   Jul 01 1999 14:17:00   iarustam
   Initial Release
   
      Rev 1.0   Jun 28 1999 13:58:06   iarustam
   Initial Release
   
      Rev 1.2   May 10 1999 11:00:20   dperson
   Added PVCS header
*   
************************************************************************************/
// CardPlusMain.h : header file
//
#include "CardPlusData.h"
/////////////////////////////////////////////////////////////////////////////
// CCardPlusMain dialog

class CCardPlusMain : public CPropertyPage
{
	DECLARE_DYNCREATE(CCardPlusMain)

// Construction
public:
	CCardPlusMain();
	~CCardPlusMain();

	CCardPlusData* pDataPage;
	CPropertySheet *m_pCPSheet;

	void SetKeys();
	BOOL GetFCFData();
	BOOL KeyEntered();
	void CleanPages();
	
	CTime c_time;

// Dialog Data
	//{{AFX_DATA(CCardPlusMain)
	enum { IDD = IDD_CARDPLUSMAIN };
	CStatic	m_ComEdit;
	CButton	m_View;
	CButton	m_List;
	CComboBox	m_ctrlCardType;
	CEdit	m_ctrlCardNbr;
	CString	m_strCardNbr;
	CString	m_strCardType;
	CString	m_strComEdit;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CCardPlusMain)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CCardPlusMain)
	afx_msg void OnList();
	virtual BOOL OnInitDialog();
	afx_msg void OnView();
	afx_msg void OnChangeCardnbr();
	afx_msg void OnSelchangeCardtype();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};
