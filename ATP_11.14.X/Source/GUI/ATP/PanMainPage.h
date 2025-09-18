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
* $Log:   N:\PVCS\PTE\Equitable\ATP_gui\PanMainPage.h  $
   
      Rev 1.6   Dec 01 2003 11:30:00   lmitchel
   Added AMEX Online Updates - scr1200
   
      Rev 1.5   Mar 16 2000 10:39:14   iarustam
   Added alpha lookup screens and updated
   Merchant main, Card Record main and 
   primary account main screens
   
      Rev 1.4   Jan 05 2000 14:54:12   gbarnes
   Major update. Not updated to PVCS recently.
   PVCS was locked.
   
      Rev 1.1   Sep 22 1999 15:57:46   iarustam
   Added manual batch entry GUI
   
      Rev 1.0   Jul 01 1999 14:18:06   iarustam
   Initial Release
   
      Rev 1.0   Jun 28 1999 13:59:12   iarustam
   Initial Release
   
      Rev 1.1   Apr 29 1999 14:56:54   iarustam
   Bug fixes
   
      Rev 1.0   Feb 23 1999 11:41:02   iarustam
   Initial Release
   
      Rev 1.0   Feb 17 1999 10:09:54   iarustam
   Initial Release
   
      Rev 1.1   Jan 22 1999 10:59:54   iarustam
   Bug fixes and core 1_3_0 changes
   
      Rev 1.1   Jan 21 1999 13:09:30   iarustam
   Bug fixes
   
   
      Rev 1.1   Dec 18 1998 13:44:32   iarustam
    
*
************************************************************************************/
// PanMainPage.h : header file
//
/////////////////////////////////////////////////////////////////////////////
// CPanMainPage dialog

#include "PanProcesPage.h"
#include "PanDemogrPage.h"
#include "PanTxPage.h"
#include "ChangeHistory.h"

class CPanMainPage : public CPropertyPage
{
	DECLARE_DYNCREATE(CPanMainPage)

// Construction
public:
	CPanMainPage();
	~CPanMainPage();


	int DialogMode;
	CPropertySheet *m_pSheet;

	CPanProcesPage*		pProcPage;
	CPanDemogrPage*		pDemoPage;
	CPanTxPage*			pTxPage;
	CChangeHistory*		pChHistPage;

	char m_strCurrentData[151];
	char m_strPreviousData[151];
	char m_strFieldTag[50];
	PAF01	sOldPAFRecord;

	BOOL PanEntered();
	void SetPanEdits();
	BOOL GetPAFData(int DlgMode);
	void PopulatePAF01(pPAF01 psPAFRecord);
	void SetModes(int mode);
	void PopulateHistoryTable();
	void PopulateHistPage();
	void CleanPages();


	CTime c_time;
	// Dialog Data
	//{{AFX_DATA(CPanMainPage)
	enum { IDD = IDD_PANMAIN };
	CButton	m_Search;
	CButton	m_View;
	CButton	m_Update;
	CButton	m_List;
	CButton	m_Delete;
	CButton	m_Add;
	CEdit	m_pan;
	CStatic	m_ComEdit;
	CString	m_panselected;
	CString	m_StrMessageLbl;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CPanMainPage)
	public:
	virtual BOOL OnSetActive();
	virtual BOOL OnApply();
	virtual void OnCancel();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CPanMainPage)
	afx_msg void OnAdd();
	virtual BOOL OnInitDialog();
	afx_msg void OnView();
	afx_msg void OnUpdate();
	afx_msg void OnDelete();
	afx_msg void OnList();
	afx_msg void OnChangePan();
	afx_msg void OnSearch();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};
