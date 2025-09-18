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
* $Log:   N:\PVCS\PTE\Equitable\ATP_gui\MerchantContact.h  $
   
      Rev 1.5   Dec 01 2003 11:29:36   lmitchel
   Added AMEX Online Updates - scr1200
   
      Rev 1.4   Jan 05 2000 14:53:54   gbarnes
   Major update. Not updated to PVCS recently.
   PVCS was locked.
   
      Rev 1.1   Sep 22 1999 15:52:44   iarustam
   Added manual batch entry GUI
   
      Rev 1.0   Jul 01 1999 14:17:44   iarustam
   Initial Release
   
      Rev 1.0   Jun 28 1999 13:58:48   iarustam
   Initial Release
   
      Rev 1.1   Apr 29 1999 14:56:34   iarustam
   Bug fixes
   
      Rev 1.0   Feb 23 1999 11:40:40   iarustam
   Initial Release
   
      Rev 1.0   Feb 17 1999 10:09:36   iarustam
   Initial Release
   
      Rev 1.1   Jan 22 1999 10:59:10   iarustam
   Bug fixes and core 1_3_0 changes
   
      Rev 1.1   Jan 21 1999 13:08:30   iarustam
   Bug fixes
   
   
      Rev 1.1   Dec 18 1998 13:43:40   iarustam
    
*
************************************************************************************/
// MerchantContact.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CMerchantContact dialog
#ifndef CONTACT
#define CONTACT

class CMerchantContact : public CPropertyPage
{
	DECLARE_DYNCREATE(CMerchantContact)

// Construction
public:
	CMerchantContact();
	~CMerchantContact();

	void EnableContactDialog ( int );
	BOOL PopulateContactDialogFromDatabase ( MCF01 );
	BOOL PopulateDatabaseFromContactDialog ( pMCF01);
	void CleanContactDialog();
	void PopulateHistoryTable();
	BOOL CompareStructures();

	BOOL m_bContact;
	int m_nDialogMode;
	MCF01 sMerchantRecordOld;
//	MCF01 sMerchantRecordNew;

	char m_strCurrentData[151];
	char m_strPreviousData[151];
	char m_strFieldTag[50];
	CTime c_time;
// Dialog Data
	//{{AFX_DATA(CMerchantContact)
	enum { IDD = IDD_MERCHANT_CONTACT };
	CEdit	m_CntlSecondaryPhoneNumberFld;
	CEdit	m_CntlPrimaryPhoneNumberFld;
	CEdit	m_CntlMerchantIDFld;
	CEdit	m_CntlContactNameFld;
	CString	m_StrContactNameFld;
	CString	m_StrMerchantIDFld;
	CString	m_StrPrimaryPhoneNumberFld;
	CString	m_StrSecondaryPhoneNumberFld;
	CString	m_StrMessageLbl;
	CString	m_StrOrganizationIDFld;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CMerchantContact)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CMerchantContact)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};
#endif