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
* $Log:   N:\PVCS\PTE\Equitable\ATP_gui\UserPropertySheet.h  $
   
      Rev 1.6   Dec 01 2003 11:30:22   lmitchel
   Added AMEX Online Updates - scr1200
   
      Rev 1.5   Jan 05 2000 14:54:32   gbarnes
   Major update. Not updated to PVCS recently.
   PVCS was locked.
   
      Rev 1.0   Jun 28 1999 13:59:38   iarustam
   Initial Release
   
      Rev 1.1   Apr 29 1999 14:57:14   iarustam
   Bug fixes
   
      Rev 1.0   Feb 23 1999 11:41:26   iarustam
   Initial Release
   
      Rev 1.0   Feb 17 1999 10:10:14   iarustam
   Initial Release
   
      Rev 1.1   Jan 22 1999 11:00:28   iarustam
   Bug fixes and core 1_3_0 changes
   
      Rev 1.1   Jan 21 1999 13:10:14   iarustam
   Bug fixes
   
   
      Rev 1.1   Dec 18 1998 13:45:06   iarustam
    
*
************************************************************************************/
// UserPropertySheet.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CUserPropertySheet
#include "UserAccess.h"
#include "UserWorkstation.h"
#include "ChangeHistory.h"
#include "UserDefinition.h"
#include "UserAccountStatus.h"


//#define WM_USEROK WM_USER + 5
//extern CView* g_pView;


class CUserPropertySheet : public CPropertySheet
{
	DECLARE_DYNAMIC(CUserPropertySheet)

// Construction
public:
	CUserPropertySheet(UINT nIDCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
	CUserPropertySheet(LPCTSTR pszCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);

// Attributes
public:
	CUserDefinition m_DefinitionPage;
	CUserAccountStatus m_UserAccountStatusPage;
    CUserWorkstation m_WorkstationPage;
    CChangeHistory m_ChangeHistoryPage;
	CUserAccess m_AccessPage;
    int m_nActiveTab;
	

	void SetData();

    typedef struct
    {
        BYTE subsystem_name  [31];
        BYTE action [6];
    }
    sControl, *psControl;

    typedef struct
    {
        BYTE terminals[10];
    }
    sLogin, *psLogin;

    sLogin m_sLogin1[10];

    sControl m_sControl1[MAX_SUBSYSTEMS];
    psControl m_psControl1;

    //CString m_StrPassword;

   
 //   BOOL PopulateUserDialogFromDatabase ( pUCF01, CUserPropertySheet* );
//    BOOL PopulateUserDatabaseStructureFromDialog ( pUCF01, CUserPropertySheet* );
 //   BOOL PopulateUserDialogFromGroupDatabase ( UCF02 , CUserPropertySheet*  );
    

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CUserPropertySheet)
	public:
	virtual BOOL OnInitDialog();
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CUserPropertySheet();

	// Generated message map functions
protected:
    
       
	//{{AFX_MSG(CUserPropertySheet)
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

    
};

/////////////////////////////////////////////////////////////////////////////
