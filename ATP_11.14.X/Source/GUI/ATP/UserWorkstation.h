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
* $Log:   N:\PVCS\PTE\Equitable\ATP_gui\UserWorkstation.h  $
   
      Rev 1.6   Dec 01 2003 11:30:24   lmitchel
   Added AMEX Online Updates - scr1200
   
      Rev 1.5   Jan 05 2000 14:54:32   gbarnes
   Major update. Not updated to PVCS recently.
   PVCS was locked.
   
      Rev 1.0   Jun 28 1999 13:59:38   iarustam
   Initial Release
   
      Rev 1.1   Apr 29 1999 14:57:14   iarustam
   Bug fixes
   
      Rev 1.0   Feb 23 1999 11:41:28   iarustam
   Initial Release
   
      Rev 1.0   Feb 17 1999 10:10:16   iarustam
   Initial Release
   
      Rev 1.1   Jan 22 1999 11:00:30   iarustam
   Bug fixes and core 1_3_0 changes
   
      Rev 1.1   Jan 21 1999 13:10:16   iarustam
   Bug fixes
   
   
      Rev 1.1   Dec 18 1998 13:45:08   iarustam
    
*
************************************************************************************/
// UserWorkstation.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CUserWorkstation dialog
#ifndef USER_WORKSTATION
#define USER_WORKSTATION

class CUserWorkstation : public CPropertyPage
{
	DECLARE_DYNCREATE(CUserWorkstation)

// Construction
public:
	CUserWorkstation();
	~CUserWorkstation();
 
    void PopulateLoginStructure();
	void PopulateWorkstationDialog();
	void EnableUserWorkstationDialog (int);
	void CleanUserWorkstationDialog ();

    int m_nDialogMode;
    BOOL m_bGroup;
	BOOL m_bWorkstation;

	int nOldCount;


    typedef struct
    {
        BYTE terminals[21];
    }
    sLogin, *psLogin;

    sLogin m_sLogin[10];
    sLogin m_sLoginNew[10];


    char m_strCurrentData[151];
    char m_strFieldTag[20];




    //BOOL GetName (CString, CString);
    void GetSystemTime();
    void GetSystemDate();
    void PopulateHistoryTable();
    CTime c_time;

// Dialog Data
	//{{AFX_DATA(CUserWorkstation)
	enum { IDD = IDD_USER_WORKSTATION };
	CEdit	m_CntlAddWorkstationFld;
	CButton	m_CntlDeleteButton;
	CButton	m_CntlAddButton;
	CListBox	m_WorkstationList;
	CEdit	m_CntlUserNameFld;
	CString	m_StrUserNameFld;
	CString	m_StrGroupIDFld;
	CString	m_StrMessageLbl;
	CString	m_StrAddWorkstationFld;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CUserWorkstation)
	public:
	virtual BOOL OnSetActive();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CUserWorkstation)
	virtual BOOL OnInitDialog();
	afx_msg void OnAddButton();
	afx_msg void OnDeleteButton();
	afx_msg void OnChangeAddWorkstationFld();
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};
#endif
