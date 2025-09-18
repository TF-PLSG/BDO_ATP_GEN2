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
* $Log:   N:\PVCS\PTE\Equitable\ATP_gui\NetworkLogon.h  $
   
      Rev 1.6   Dec 01 2003 11:29:58   lmitchel
   Added AMEX Online Updates - scr1200
   
      Rev 1.5   Jan 05 2000 14:54:10   gbarnes
   Major update. Not updated to PVCS recently.
   PVCS was locked.
   
      Rev 1.1   Sep 22 1999 15:56:54   iarustam
   Added manual batch entry GUI
   
      Rev 1.0   Jul 01 1999 14:18:02   iarustam
   Initial Release
   
      Rev 1.0   Jun 28 1999 13:59:10   iarustam
   Initial Release
   
      Rev 1.2   Apr 29 1999 14:56:52   iarustam
   Bug fixes
   
      Rev 1.1   Mar 16 1999 16:30:18   iarustam
   Bug fixes
   
      Rev 1.0   Feb 23 1999 11:40:58   iarustam
   Initial Release
   
      Rev 1.0   Feb 17 1999 10:09:50   iarustam
   Initial Release
   
      Rev 1.1   Jan 22 1999 10:59:46   iarustam
   Bug fixes and core 1_3_0 changes
   
      Rev 1.1   Jan 21 1999 13:09:24   iarustam
   Bug fixes
   
   
      Rev 1.1   Dec 18 1998 13:44:24   iarustam
    
*
************************************************************************************/
// NetworkLogon.h : header file
//
#ifndef NETWORKLOGON
#define NETWORKLOGON
/////////////////////////////////////////////////////////////////////////////
// CNetworkLogon dialog

class CNetworkLogon : public CPropertyPage
{
	DECLARE_DYNCREATE(CNetworkLogon)

// Construction
public:
	CNetworkLogon();
	~CNetworkLogon();

	NCF01 sNetworkId;
	NCF01 sNetworkIdOld;	

	int m_bLogon;
	int m_nDialogMode;

	char m_StrCurrentData[151];
    char m_StrPreviousData[151];
    char m_StrFieldTag[50];
    char strFilePrimaryKey[50];

	void CleanLogonDialog();
	void EnableLogonDialog(int);
	void PopulateHistoryTable();

	BOOL PopulateDatabaseFromLogonDialog( pNCF01 psNetwork );
	BOOL PopulateLogonDialogFromDatabase (NCF01 sNetwork);
	BOOL CompareStructures();
	CTime c_time;
// Dialog Data
	//{{AFX_DATA(CNetworkLogon)
	enum { IDD = IDD_NETWORK_LOGON };
	CEdit	m_CntlLogon9Fld;
	CEdit	m_CntlLogon8Fld;
	CEdit	m_CntlLogon7Fld;
	CEdit	m_CntlLogon6Fld;
	CEdit	m_CntlLogon5Fld;
	CEdit	m_CntlLogon4Fld;
	CEdit	m_CntlLogon3Fld;
	CEdit	m_CntlLogon2Fld;
	CEdit	m_CntlLogon10Fld;
	CEdit	m_CntlLogon1Fld;
	CEdit	m_CntlNetworkTypeFld;
	CEdit	m_CntlNetworkIdFld;
	CString	m_StrLogon1Fld;
	CString	m_StrLogon10Fld;
	CString	m_StrLogon2Fld;
	CString	m_StrLogon3Fld;
	CString	m_StrLogon4Fld;
	CString	m_StrLogon5Fld;
	CString	m_StrLogon6Fld;
	CString	m_StrLogon7Fld;
	CString	m_StrLogon8Fld;
	CString	m_StrLogon9Fld;
	CString	m_StrMessageLbl;
	CString	m_StrNetworkIdFld;
	CString	m_StrNetworkTypeFld;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CNetworkLogon)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CNetworkLogon)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};
#endif
