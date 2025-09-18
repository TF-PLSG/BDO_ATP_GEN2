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
* $Log:   N:\PVCS\PTE\Equitable\ATP_gui\MerchantTerminalBatchHistory.h  $
   
      Rev 1.5   Dec 01 2003 11:29:40   lmitchel
   Added AMEX Online Updates - scr1200
   
      Rev 1.4   Jan 05 2000 14:54:00   gbarnes
   Major update. Not updated to PVCS recently.
   PVCS was locked.
   
      Rev 1.2   Sep 30 1999 16:17:28   npogosya
   Bug fixes
   
      Rev 1.1   Sep 22 1999 15:53:58   iarustam
   Added manual batch entry GUI
   
      Rev 1.0   Jul 01 1999 14:17:48   iarustam
   Initial Release
   
      Rev 1.0   Jun 28 1999 13:58:54   iarustam
   Initial Release
   
      Rev 1.1   Apr 29 1999 14:56:38   iarustam
   Bug fixes
   
      Rev 1.0   Feb 23 1999 11:40:44   iarustam
   Initial Release
   
      Rev 1.0   Feb 17 1999 10:09:40   iarustam
   Initial Release
   
      Rev 1.1   Jan 22 1999 10:59:18   iarustam
   Bug fixes and core 1_3_0 changes
   
      Rev 1.1   Jan 21 1999 13:08:40   iarustam
   Bug fixes
   
   
      Rev 1.1   Dec 18 1998 13:43:56   iarustam
    
*
************************************************************************************/
// MerchantTerminalBatchHistory.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CMerchantTerminalBatchHistory dialog
#ifndef BATCHHISTORY
#define BATCHHISTORY

extern "C"
{
#include "PteTime.h"
#include "Txutils.h"
}

class CMerchantTerminalBatchHistory : public CPropertyPage
{
	DECLARE_DYNCREATE(CMerchantTerminalBatchHistory)

// Construction
public:
	CMerchantTerminalBatchHistory();
	~CMerchantTerminalBatchHistory();

	BOOL bNext;
	BOOL m_bBatchHistory;
	BOOL bDone;
	int m_nDialogMode;

	void PopulateBatchHistoryDialog(DCF01);
	void CleanBatchHistoryDialog();
	afx_msg void OnBnClickedSortButton1();
	BCH01 sBatchRecord;
	DCF01 sTerminalRecord;
	//BCH01_GUI_LIST sBatchList;
	CTime c_time;

// Dialog Data
	//{{AFX_DATA(CMerchantTerminalBatchHistory)
	enum { IDD = IDD_TERMINAL_BATCH_HISTORY };
	CButton	m_CntlNextButton;
	CListCtrl	m_CntlList;
	CButton	m_CntlViewButton;
	CButton m_CntlSortButton;
	CString	m_StrOrganizationIDFld;
	CString	m_StrMerchantIDFld;
	CString	m_StrMessageLbl;
	CString	m_StrTerminalIDFld;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CMerchantTerminalBatchHistory)
	public:
	virtual BOOL OnSetActive();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CMerchantTerminalBatchHistory)
	afx_msg void OnViewButton();
	afx_msg void OnNextButton();
	virtual BOOL OnInitDialog();
	
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	
	afx_msg int isvalid();
	afx_msg int isleap ( long int y );
	afx_msg int getnum ( char *date );
	afx_msg void OnBnClickedNextButton2();
	afx_msg void OnLvnItemchangedList(NMHDR *pNMHDR, LRESULT *pResult);
};
#endif
