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
* $Log:   N:\PVCS\PTE\Equitable\ATP_gui\groupworkstation.h  $
   
      Rev 1.6   Dec 01 2003 11:29:24   lmitchel
   Added AMEX Online Updates - scr1200
   
      Rev 1.5   Jan 05 2000 14:55:02   gbarnes
   Major update. Not updated to PVCS recently.
   PVCS was locked.
   
      Rev 1.0   Jun 28 1999 13:59:54   iarustam
   Initial Release
   
      Rev 1.1   Apr 29 1999 14:57:22   iarustam
   Bug fixes
   
      Rev 1.0   Feb 23 1999 11:41:36   iarustam
   Initial Release
   
      Rev 1.0   Feb 17 1999 10:10:22   iarustam
   Initial Release
   
      Rev 1.1   Jan 22 1999 11:00:42   iarustam
   Bug fixes and core 1_3_0 changes
   
      Rev 1.1   Jan 21 1999 13:10:28   iarustam
   Bug fixes
   
   
      Rev 1.1   Dec 18 1998 13:45:20   iarustam
    
*
************************************************************************************/
// GroupWorkstation.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CGroupWorkstation dialog
#ifndef GROUPWORKSTATION
#define GROUPWORKSTATION

class CGroupWorkstation : public CPropertyPage
{
	DECLARE_DYNCREATE(CGroupWorkstation)

// Construction
public:
	CGroupWorkstation();
	~CGroupWorkstation();

	int nOldCount;

    
    void PopulateLoginStructure();
    int m_nDialogMode;
	void PopulateWorkstationDialog();
	void EnableGroupWorkstationDialog (int );
	void CleanGroupWorkstationDialog();

	char m_strCurrentData[151];
    char m_strFieldTag[20];

	BOOL m_bWorkstation;

    typedef struct
    {
        BYTE terminals[21];
    }
    sLogin, *psLogin;

    sLogin m_sLogin[10];
    sLogin m_sLoginNew[10];

    
    void PopulateHistoryTable();
	CTime c_time;

// Dialog Data
	//{{AFX_DATA(CGroupWorkstation)
	enum { IDD = IDD_GROUPS_WORKSTATION };
	CEdit	m_CntlAddWorkstationFld;
	CButton	m_CntlDeleteButton;
	CButton	m_CntlAddButton;
	CListBox	m_WorkstationList;
	CString	m_StrGroupIDFld;
	CString	m_StrAddWorkstationFld;
	CString	m_StrMessageLbl;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CGroupWorkstation)
	public:
	virtual BOOL OnSetActive();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CGroupWorkstation)
	virtual BOOL OnInitDialog();
	afx_msg void OnAddButton();
	afx_msg void OnDeleteButton();
	afx_msg void OnChangeAddWorkstation();
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

#endif