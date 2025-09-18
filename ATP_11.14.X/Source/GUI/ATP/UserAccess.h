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
* $Log:   N:\PVCS\PTE\Equitable\ATP_gui\UserAccess.h  $
   
      Rev 1.7   Dec 01 2003 11:30:22   lmitchel
   Added AMEX Online Updates - scr1200
   
      Rev 1.6   Jan 05 2000 14:54:30   gbarnes
   Major update. Not updated to PVCS recently.
   PVCS was locked.
   
      Rev 1.0   Jun 28 1999 13:59:36   iarustam
   Initial Release
   
      Rev 1.2   Jun 11 1999 14:08:20   iarustam
   Bug fixes
   
   
      Rev 1.1   Apr 29 1999 14:57:12   iarustam
   Bug fixes
   
      Rev 1.0   Feb 23 1999 11:41:26   iarustam
   Initial Release
   
      Rev 1.0   Feb 17 1999 10:10:12   iarustam
   Initial Release
   
      Rev 1.0   Feb 12 1999 11:06:24   iarustam
   initial release
   
      Rev 1.1   Dec 18 1998 13:45:02   iarustam
    
*
************************************************************************************/
// UserAccess.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CUserAccess dialog
#ifndef USER_ACCESS
#define USER_ACCESS

class CUserAccess : public CPropertyPage
{
	DECLARE_DYNCREATE(CUserAccess)

// Construction
public:
	CUserAccess();
	~CUserAccess();

	BOOL PopulateDialogFromDatabase ();
	void CleanUserAccessDialog();
	UCF01 m_sUserRecord;

    typedef struct
    {
        BYTE subsystem_name  [31];
        BYTE action [6];
    }
    sControl, *psControl;

	void EnableUserAccessDialog( int );

    sControl m_sControl[MAX_SUBSYSTEMS];
    sControl m_sControlNew[MAX_SUBSYSTEMS];
	CString Old_Gid;
	CString Old_Uname;
	CString Old_pswrd;
    CString New_Gid;
	CString New_Uname;
    int m_nDialogMode;

    BOOL m_bGroup;
	BOOL m_bPopulate;
	BOOL m_bAccess;
	BOOL m_bGroupIDChanged;

	BOOL m_enc;
	BOOL m_nenc;
	CString m_strGroup;
    CString m_strNewPassword;
    UCF02 sGroupRecord;

    //BOOL GetName (CString, CString);
   
    void PopulateHistoryTable();
    BOOL PopulateControlStructure ();
	BYTE CheckCountofUsersAssignedforDualEncryption(CString m_Username, int idx);
   // BOOL CUserAccess::GetControlStructure ( psControl);
   CTime c_time;
// Dialog Data
	//{{AFX_DATA(CUserAccess)
	enum { IDD = IDD_USER_ACCESS };
	CButton	m_CntlViewCheck20;
	CButton	m_CntlAddCheck18;
	CButton	m_CntlDeleteCheck18;
	CStatic	m_CntlLabel18;
	CButton	m_CntlUpdateCheck18;
	CButton	m_CntlViewCheck18;
	CStatic	m_CntlLabel16;
	CButton	m_CntlOverrideCheck;
	CButton	m_CntlChangePasswordButton;
	CButton	m_CntlGroupListButton;
	CEdit	m_CntlUserNameFld2;
	CEdit	m_CntlGroupIDFld;
	CButton	m_CntlViewCheck15;
	CStatic	m_CntlLabel15;
	CButton	m_CntlViewCheck14;
	CButton	m_CntlUpdateCheck14;
	CButton	m_CntlDeleteCheck14;
	CButton	m_CntlAddCheck14;
	CStatic	m_CntlLabel14;
	CButton	m_CntlAll;
	CButton	m_CntlAddCheck13; //For Dual Encryption, there is only Add check box
	CButton	m_CntlViewCheck12;
	CButton	m_CntlUpdateCheck12;
	CButton	m_CntlDeleteCheck12;
	CButton	m_CntlAddCheck12;
	CButton	m_CntlAddCheck11;
	CButton	m_CntlViewCheck11;
	CButton	m_CntlUpdateCheck11;
	CButton	m_CntlDeleteCheck11;
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
	CStatic	m_CntlLabel9;
	CStatic	m_CntlLabel8;
	CStatic	m_CntlLabel7;
	CStatic	m_CntlLabel6;
	CStatic	m_CntlLabel5;
	CStatic	m_CntlLabel23;
	CStatic	m_CntlLabel4;
	CStatic	m_CntlLabel3;
	CStatic	m_CntlLabel2;
	CStatic	m_CntlLabel10;
	CStatic	m_CntlLabel1;
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
	CString	m_StrUserNameFld;
	CString	m_StrGroupIDFld;
	CString	m_StrLabel1;
	CString	m_StrLabel2;
	CString	m_StrLabel3;
	CString	m_StrLabel4;
	CString	m_StrLabel5;
	CString	m_StrLabel6;
	CString	m_StrLabel7;
	CString	m_StrLabel8;
	CString	m_StrLabel9;
	CString	m_StrLabel10;
	CString	m_StrMessageLbl;
	CString	m_StrLabel11;
	CString	m_StrLabel12;
	CString	m_StrLabel13;
	CString	m_StrLabel14;
	CString	m_StrLabel15;
	CString	m_StrUserNameFld2;
	CString	m_StrLabel16;
	CString	m_StrLabel18;
	CString m_StrLabel17; // for system global parameter
	CButton	m_CntlUpdateCheck17;
	CButton	m_CntlViewCheck17;
	CString m_StrLabel20; 
	CButton	m_CntlViewCheck21;
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
	//{{AFX_VIRTUAL(CUserAccess)
	public:
	virtual BOOL OnSetActive();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CUserAccess)
	virtual BOOL OnInitDialog();
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	afx_msg void OnCheckAll();
	afx_msg void OnCheckEncryptionControl();
	afx_msg void OnGroupListButton();
	afx_msg void OnChangePasswordButton();
	afx_msg void OnChangeUserNameFld2();
	afx_msg void OnChangeGroupIdFld();
	afx_msg void OnLabel13();
	afx_msg void OnLabel16();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnBnClickedAddCheck18();
	afx_msg void OnBnClickedViewCheck12();
};
#endif
