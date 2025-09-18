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
* $Log:   N:\PVCS\PTE\Equitable\ATP_gui\UserDefinition.h  $

      Rev 1.8   Jan 01 2011 11:30:22   TF-Srikanth
   Added PCI changes Unlocking User account through GUI

  
      Rev 1.7   Dec 01 2003 11:30:22   lmitchel
   Added AMEX Online Updates - scr1200
   
      Rev 1.6   Jan 25 2000 14:01:02   iarustam
   bug fixes
   
      Rev 1.5   Jan 05 2000 14:54:32   gbarnes
   Major update. Not updated to PVCS recently.
   PVCS was locked.
   
      Rev 1.0   Jun 28 1999 13:59:38   iarustam
   Initial Release
   
      Rev 1.1   Apr 29 1999 14:57:12   iarustam
   Bug fixes
   
      Rev 1.0   Feb 23 1999 11:41:26   iarustam
   Initial Release
   
      Rev 1.0   Feb 17 1999 10:10:14   iarustam
   Initial Release
   
      Rev 1.1   Jan 22 1999 11:00:26   iarustam
   Bug fixes and core 1_3_0 changes
   
      Rev 1.1   Jan 21 1999 13:10:12   iarustam
   Bug fixes
   
   
      Rev 1.1   Dec 18 1998 13:45:04   iarustam
    
*
************************************************************************************/
// UserDefinition.h : header file
//

#include "common.h"
/////////////////////////////////////////////////////////////////////////////
// CUserDefinition dialog
#ifndef USER_DEFINITION
#define USER_DEFINITION

#include "ChangeHistory.h"
#include "UserAccountStatus.h"
class CUserDefinition : public CPropertyPage
{
	DECLARE_DYNCREATE(CUserDefinition)

// Construction
public:
	CUserDefinition();
	~CUserDefinition();

	int m_nDialogMode;
	int m_nPrevMode;
	BOOL CheckDialogForCompleteness();
	BOOL PopulateUserDialogFromGroupDatabase ( UCF02  );
	BOOL PopulateUserDialogFromDatabase ( UCF01,UCF01  );
	BOOL PopulateUserDatabaseStructureFromDialog ( pUCF01 ) ;
	BOOL PopulatenewUserDatabaseStructureFromDialog ( pUCF01 ) ;
	void PopulateHistoryTable();
	void SetExpiryDate(char *ToDayDate);

	char m_strCurrentData[151];
	char m_strPassword[51];
	char m_strGroup[16];
	CString m_bAccStatus;

	BOOL m_bStatus;
	CChangeHistory* pChangeHistoryPage;

	/***********************/

	typedef struct
    {
        BYTE subsystem_name  [31];
        BYTE action [6];
    }
    sControl, *psControl;	

    typedef struct
    {
        BYTE terminals[21];
    }
    sLogin, *psLogin;

    sLogin m_sLogin1[10];

    sControl m_sControl1[MAX_SUBSYSTEMS];
    psControl m_psControl1;
    UCF01 sOldUCF01Record;
//	CUserAccess* pAccessPage;
	CTime c_time;
// Dialog Data
	//{{AFX_DATA(CUserDefinition)
	enum { IDD = IDD_USER_DEFINITON };
	CButton	m_CntlListGroupsBtn;
	CEdit	m_CntlUserNameFld2;
	CButton	m_AddButton;
	CButton	m_ViewButton;
	CButton	m_UpdateButton;
	CButton	m_ListButton;
	CButton	m_DeleteButton;
	CEdit	m_CntlUserPassword2Fld;
	CEdit	m_CntlUserPasswordFld;
	CEdit	m_CntlUserNameFld;
	CEdit	m_CntlGroupIDFld;
	CString	m_StrGroupIDFld;
	CString	m_StrUserNameFld;
	CString	m_StrUserPasswordFld;
	CString	m_StrMessageLbl;
	CString	m_StrUserPassword2Fld;
	CString	m_StrUserNameFld2;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CUserDefinition)
	public:
	virtual BOOL OnApply();
	virtual BOOL OnSetActive();
	virtual void OnOK();
	virtual void OnCancel();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CUserDefinition)
	afx_msg void OnUserAddButton();
	afx_msg void OnUserDeleteButton();
	afx_msg void OnUserListButton();
	afx_msg void OnUserUpdateButton();
	afx_msg void OnUserViewButton();
	virtual BOOL OnInitDialog();
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	afx_msg void OnUserAcessButton();
	afx_msg void OnGroupList();
	
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};
#endif
