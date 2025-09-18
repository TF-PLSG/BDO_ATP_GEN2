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
* $Log:   N:\PVCS\PTE\Equitable\ATP_gui\NetworkProcessing.h  $
   
      Rev 1.6   Dec 01 2003 11:29:58   lmitchel
   Added AMEX Online Updates - scr1200
   
      Rev 1.5   Jan 05 2000 14:54:10   gbarnes
   Major update. Not updated to PVCS recently.
   PVCS was locked.
   
      Rev 1.1   Sep 22 1999 15:57:04   iarustam
   Added manual batch entry GUI
   
      Rev 1.0   Jul 01 1999 14:18:04   iarustam
   Initial Release
   
      Rev 1.0   Jun 28 1999 13:59:10   iarustam
   Initial Release
   
      Rev 1.2   Apr 29 1999 14:56:52   iarustam
   Bug fixes
   
      Rev 1.1   Mar 16 1999 16:30:18   iarustam
   Bug fixes
   
      Rev 1.0   Feb 23 1999 11:40:58   iarustam
   Initial Release
   
      Rev 1.0   Feb 17 1999 10:09:52   iarustam
   Initial Release
   
      Rev 1.1   Jan 22 1999 10:59:48   iarustam
   Bug fixes and core 1_3_0 changes
   
      Rev 1.1   Jan 21 1999 13:09:24   iarustam
   Bug fixes
   
   
      Rev 1.1   Dec 18 1998 13:44:26   iarustam
    
*
************************************************************************************/
// NetworkProcessing.h : header file
//
#ifndef NETWORKPROCESSING
#define NETWORKPROCESSING 

/////////////////////////////////////////////////////////////////////////////
// CNetworkProcessing dialog

class CNetworkProcessing : public CPropertyPage
{
	DECLARE_DYNCREATE(CNetworkProcessing)

// Construction
public:
	CNetworkProcessing();
	~CNetworkProcessing();

	NCF01 sNetworkId;
	NCF01 sNetworkIdOld;	

	int m_bProcessing;
	int m_nDialogMode;

	char m_StrCurrentData[151];
    char m_StrPreviousData[151];
    char m_StrFieldTag[50];
    char strFilePrimaryKey[50];

	void CleanProcessingDialog();
	void EnableProcessingDialog(int);
	void PopulateHistoryTable();

	BOOL PopulateDatabaseFromProcessingDialog( pNCF01 psNetwork );
	BOOL PopulateProcessingDialogFromDatabase (NCF01 sNetwork);
	BOOL CompareStructures();
	CTime c_time;
// Dialog Data
	//{{AFX_DATA(CNetworkProcessing)
	enum { IDD = IDD_NETWORK_PROCESSING };
	CEdit	m_CntlCode9Fld;
	CEdit	m_CntlCode8Fld;
	CEdit	m_CntlCode7Fld;
	CEdit	m_CntlCode6Fld;
	CEdit	m_CntlCode5Fld;
	CEdit	m_CntlCode4Fld;
	CEdit	m_CntlCode3Fld;
	CEdit	m_CntlCode20Fld;
	CEdit	m_CntlCode2Fld;
	CEdit	m_CntlCode19Fld;
	CEdit	m_CntlCode18Fld;
	CEdit	m_CntlCode17Fld;
	CEdit	m_CntlCode16Fld;
	CEdit	m_CntlCode15Fld;
	CEdit	m_CntlCode14Fld;
	CEdit	m_CntlCode13Fld;
	CEdit	m_CntlCode12Fld;
	CEdit	m_CntlCode11Fld;
	CEdit	m_CntlCode10Fld;
	CEdit	m_CntlCode1Fld;
	CEdit	m_CntlNetworkTypeFld;
	CEdit	m_CntlNetworkIdFld;
	CString	m_StrCode1Fld;
	CString	m_StrCode10Fld;
	CString	m_StrCode11Fld;
	CString	m_StrCode12Fld;
	CString	m_StrCode13Fld;
	CString	m_StrCode14Fld;
	CString	m_StrCode15Fld;
	CString	m_StrCode16Fld;
	CString	m_StrCode17Fld;
	CString	m_StrCode18Fld;
	CString	m_StrCode19Fld;
	CString	m_StrCode2Fld;
	CString	m_StrCode20Fld;
	CString	m_StrCode3Fld;
	CString	m_StrCode4Fld;
	CString	m_StrCode5Fld;
	CString	m_StrCode6Fld;
	CString	m_StrCode7Fld;
	CString	m_StrCode8Fld;
	CString	m_StrCode9Fld;
	CString	m_StrMessageLbl;
	CString	m_StrNetworkIdFld;
	CString	m_StrNetworkTypeFld;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CNetworkProcessing)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CNetworkProcessing)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};
#endif
