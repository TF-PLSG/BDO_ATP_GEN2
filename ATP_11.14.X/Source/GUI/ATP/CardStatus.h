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
* $Log:   N:\PVCS\PTE\Equitable\ATP_gui\CardStatus.h  $
   
      Rev 1.7   Dec 01 2003 11:28:56   lmitchel
   Added AMEX Online Updates - scr1200
   
      Rev 1.6   Jan 05 2000 14:53:06   gbarnes
   Major update. Not updated to PVCS recently.
   PVCS was locked.
   
      Rev 1.1   Sep 22 1999 15:44:54   iarustam
   Added manual batch entry GUI
   
      Rev 1.0   Jul 01 1999 14:17:02   iarustam
   Initial Release
   
      Rev 1.0   Jun 28 1999 13:58:08   iarustam
   Initial Release
   
      Rev 1.2   Jun 11 1999 14:08:46   iarustam
   Bug fixes
   
   
      Rev 1.1   Apr 29 1999 14:56:00   iarustam
   Bug fixes
   
      Rev 1.0   Feb 23 1999 11:40:08   iarustam
   Initial Release
   
      Rev 1.0   Feb 17 1999 10:09:10   iarustam
   Initial Release
   
      Rev 1.1   Jan 22 1999 10:58:02   iarustam
   Bug fixes and core 1_3_0 changes
   
      Rev 1.1   Jan 21 1999 13:07:04   iarustam
   Bug fixes
   
   
      Rev 1.1   Dec 18 1998 13:42:40   iarustam
    
*
************************************************************************************/
// CardStatus.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CCardStatus dialog

class CCardStatus : public CDialog
{
// Construction
public:
	CCardStatus(CWnd* pParent = NULL);   // standard constructor


	CSF01 sCSFRecord;
	CSF01_GUI_LIST sCSFList;
	BOOL bDone;

	void PopulateCSFListDialog();

	CTime c_time;

// Dialog Data
	//{{AFX_DATA(CCardStatus)
	enum { IDD = IDD_CARDSTATUS };
	CButton	m_UpdateButton;
	CButton	m_DeleteButton;
	CButton	m_AddButton;
	CListCtrl	m_ctrlList;
	CStatic	m_ctrlComEdit;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCardStatus)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CCardStatus)
	virtual BOOL OnInitDialog();
	afx_msg void OnUpdate();
	afx_msg void OnAdd();
	afx_msg void OnDblclkCsflist(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDelete();
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnNext();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
