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
* $Log:   N:\PVCS\PTE\Equitable\ATP_gui\groupaccess.h  $
   
      Rev 1.8   Dec 01 2003 11:29:22   lmitchel
   Added AMEX Online Updates - scr1200
   
      Rev 1.7   Jan 05 2000 14:55:02   gbarnes
   Major update. Not updated to PVCS recently.
   PVCS was locked.
   
      Rev 1.0   Jun 28 1999 13:59:52   iarustam
   Initial Release
   
      Rev 1.2   Jun 11 1999 14:08:42   iarustam
   Bug fixes
   
   
      Rev 1.1   Apr 29 1999 14:57:20   iarustam
   Bug fixes
   
      Rev 1.0   Feb 23 1999 11:41:34   iarustam
   Initial Release
   
      Rev 1.0   Feb 17 1999 10:10:20   iarustam
   Initial Release
   
      Rev 1.0   Feb 12 1999 11:06:32   iarustam
   initial release
   
      Rev 1.1   Dec 18 1998 13:45:16   iarustam
    
*
************************************************************************************/
// GroupAccess.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CGroupAccess dialog
#ifndef GROUPACCESS
#define GROUPACCESS


class CGroupAccess : public CPropertyPage
{
	DECLARE_DYNCREATE(CGroupAccess)

// Construction
public:
	CGroupAccess();
	~CGroupAccess();

     typedef struct
    {
        BYTE subsystem_name  [31];
        BYTE action [6];
    }
    sControl, *psControl;

	int m_nDialogMode;

	BOOL m_bPopulate;
	BOOL m_bAccess;

	BOOL PopulateDialogFromDatabase();
	void CleanGroupAccessDialog();
	void EnableGroupAccessDialog(int );

    sControl m_sControl[MAX_SUBSYSTEMS];
    sControl m_sControlNew[MAX_SUBSYSTEMS];
    

    //BOOL GetName (CString, CString);
    
    void PopulateHistoryTable();
    BOOL PopulateControlStructure ();
	CTime c_time;
   

// Dialog Data
	//{{AFX_DATA(CGroupAccess)
	enum { IDD = IDD_GROUPS_ACCESS };
	CButton	m_CntlViewCheck19;
	CButton	m_CntlUpdateCheck19;
	CButton	m_CntlDeleteCheck19;
	CButton	m_CntlAddCheck19;
	CButton	m_CntlCheckAll;
	CButton	m_CntlViewCheck15;
	CButton	m_CntlViewCheck13;
	CButton	m_CntlUpdateCheck13;
	CButton	m_CntlDeleteCheck13;
	CButton	m_CntlAddCheck13;
	CButton	m_CntlUpdateCheck12;
	CButton	m_CntlViewCheck12;
	CButton	m_CntlDeleteCheck12;
	CButton	m_CntlAddCheck12;
	CButton	m_CntlViewCheck11;
	CButton	m_CntlUpdateCheck11;
	CButton	m_CntlDeleteCheck11;
	CButton	m_CntlAddCheck11;
	CButton	m_CntlViewCheck9;
	CButton	m_CntlViewCheck8;
	CButton	m_CntlViewCheck7;
	CButton	m_CntlViewCheck6;
	CButton	m_CntlViewCheck5;
	CButton	m_CntlViewCheck4;
	CButton	m_CntlViewCheck3;
	CButton	m_CntlViewCheck2;
	CButton	m_CntlViewCheck10;
	CButton	m_CntlViewCheck1;
	CButton	m_CntlUpdateCheck9;
	CButton	m_CntlUpdateCheck8;
	CButton	m_CntlUpdateCheck7;
	CButton	m_CntlUpdateCheck6;
	CButton	m_CntlUpdateCheck5;
	CButton	m_CntlUpdateCheck4;
	CButton	m_CntlUpdateCheck3;
	CButton	m_CntlUpdateCheck2;
	CButton	m_CntlUpdateCheck10;
	CButton	m_CntlUpdateCheck1;
	CButton	m_CntlDeleteCheck9;
	CButton	m_CntlDeleteCheck8;
	CButton	m_CntlDeleteCheck7;
	CButton	m_CntlDeleteCheck6;
	CButton	m_CntlDeleteCheck5;
	CButton	m_CntlDeleteCheck4;
	CButton	m_CntlDeleteCheck3;
	CButton	m_CntlDeleteCheck2;
	CButton	m_CntlDeleteCheck10;
	CButton	m_CntlDeleteCheck1;
	CButton	m_CntlAddCheck9;
	CButton	m_CntlAddCheck8;
	CButton	m_CntlAddCheck7;
	CButton	m_CntlAddCheck6;
	CButton	m_CntlAddCheck5;
	CButton	m_CntlAddCheck4;
	CButton	m_CntlAddCheck3;
	CButton	m_CntlAddCheck2;
	CButton	m_CntlAddCheck10;
	CButton	m_CntlAddCheck1;
	CButton m_CntlTlfExtract;
	CString	m_StrLabel1;
	CString	m_StrLabel10;
	CString	m_StrLabel2;
	CString	m_StrLabel3;
	CString	m_StrLabel4;
	CString	m_StrLabel5;
	CString	m_StrLabel6;
	CString	m_StrLabel7;
	CString	m_StrLabel8;
	CString	m_StrLabel9;
	CString	m_StrGroupIDFld;
	CString	m_StrMessageLbl;
	CString	m_StrLabel11;
	CString	m_StrLabel12;
	CString	m_StrLabel13;
	CString	m_StrLabel15;
	CString	m_StrLabel19;
	 CString	m_StrLabel20;
	 CButton m_CntlViewCheck20;
	CButton  m_CntlUpdateCheck20;
	CString m_StrLabel21;
	CButton m_CntlViewCheck21;
	CString m_StrLabel22;
	CButton m_CntlViewCheck22;
	CButton m_CntlUpdateCheck22;
	CButton m_CntlAddCheck22;
	CButton m_CntlDeleteCheck22;
		CString m_StrLabel23;
	CButton m_CntlViewCheck23;
	CButton m_CntlUpdateCheck23;
	CButton m_CntlAddCheck23;
	CButton m_CntlDeleteCheck23;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CGroupAccess)
	public:
	virtual BOOL OnSetActive();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CGroupAccess)
	virtual BOOL OnInitDialog();
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	afx_msg void OnCheckAll();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnBnClickedDeleteCheck19();
	afx_msg void OnBnClickedViewCheck19();
};

#endif
