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
* $Log:   N:\PVCS\PTE\Equitable\ATP_gui\networkdefinition.h  $
   
      Rev 1.5   Dec 01 2003 11:29:54   lmitchel
   Added AMEX Online Updates - scr1200
   
      Rev 1.4   Jan 05 2000 14:55:02   gbarnes
   Major update. Not updated to PVCS recently.
   PVCS was locked.
   
      Rev 1.1   Sep 22 1999 16:04:56   iarustam
   Added manual batch entry GUI
   
      Rev 1.0   Jul 01 1999 14:18:42   iarustam
   Initial Release
   
      Rev 1.0   Jun 28 1999 13:59:56   iarustam
   Initial Release
   
      Rev 1.1   Apr 29 1999 14:57:22   iarustam
   Bug fixes
   
      Rev 1.0   Feb 23 1999 11:41:38   iarustam
   Initial Release
   
      Rev 1.0   Feb 17 1999 10:10:22   iarustam
   Initial Release
   
      Rev 1.1   Jan 22 1999 11:00:42   iarustam
   Bug fixes and core 1_3_0 changes
   
      Rev 1.1   Jan 21 1999 13:10:32   iarustam
   Bug fixes
   
   
      Rev 1.1   Dec 18 1998 13:45:22   iarustam
    
*
************************************************************************************/
// NetworkDefinition.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CNetworkDefinition dialog
#ifndef CNETWORKKEYBLOCKDEFINITION
#define CNETWORKKEYBLOCKDEFINITION

#include "NetworkKeyblockGeneral.h"

#include "ChangeHistory.h"


class CNetworkKeyblockDefinition : public CPropertyPage
{
	DECLARE_DYNCREATE(CNetworkKeyblockDefinition)

// Construction
public:
	CNetworkKeyblockDefinition();
	~CNetworkKeyblockDefinition();
	
	int m_nNetworkMode;
	int m_nDialogMode;

	char m_strCurrentData[151];
	char strFilePrimaryKey[30];

	NCF01 sNetworkIdOld;
	NCF02 sNetworkOld;
	NCF01_KEYBLOCK sNetworkkeyblockOld;

	BOOL PopulateDatabaseFromNetworkKeyblockDialog( pNCF01_KEYBLOCK  );
	BOOL PopulateNetworkKeyblockDialogFromDatabase( NCF01_KEYBLOCK  );
	BOOL CheckDialogForCompleteness();
	void PopulateHistoryTable();


	CNetworkKeyblockGeneral *pGeneralPage;

    CChangeHistory *pChangeHistoryPage;
	CTime c_time;
// Dialog Data
	//{{AFX_DATA(CNetworkDefinition)
	enum { IDD = ID_NCF01_KEYBLOCK_MAIN };
	CButton	m_ViewButton;
	CButton	m_UpdateButton;
	CButton	m_ListButton;
	CButton	m_DeleteButton;
	CButton	m_AddButton;
	CEdit	m_CntlNetworkIdFld;
	/*CEdit	m_CntlNetworkTypeFld;
	CString	m_StrNetworkTypeFld;*/
	CString	m_StrMessageLbl;
	CString	m_StrNetworkIdFld;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CNetworkDefinition)
	public:
	virtual BOOL OnApply();
	virtual void OnCancel();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CNetworkDefinition)
	virtual BOOL OnInitDialog();
	afx_msg void OnNetworkAddButton();
	afx_msg void OnNetworkDeleteButton();
	afx_msg void OnNetworkListButton();
	afx_msg void OnNetworkUpdateButton();
	afx_msg void OnNetworkViewButton();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};
#endif
