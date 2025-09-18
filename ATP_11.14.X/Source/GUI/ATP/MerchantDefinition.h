/*********************************************************************************
 Module: MerchantDefinition.h : header file

 Copyright (c) 1998, Hypercom, Inc.                          
 All Rights Reserved.                                       

 Description: Logic for Merchant Main property page

 Application: ATP

 Author: Irina Arustamova

 Revision History:
  
 $Log:   N:\PVCS\PTE\Equitable\ATP_gui\MerchantDefinition.h  $
   
      Rev 1.8   Dec 01 2003 11:29:36   lmitchel
   Added AMEX Online Updates - scr1200
   
      Rev 1.7   Mar 16 2000 10:38:56   iarustam
   Added alpha lookup screens and updated
   Merchant main, Card Record main and 
   primary account main screens
   
      Rev 1.6   Jan 05 2000 14:53:54   gbarnes
   Major update. Not updated to PVCS recently.
   PVCS was locked.
   
      Rev 1.1   Sep 22 1999 15:52:58   iarustam
   Added manual batch entry GUI
   
      Rev 1.0   Jul 01 1999 14:17:44   iarustam
   Initial Release
   
      Rev 1.0   Jun 28 1999 13:58:50   iarustam
   Initial Release
   
      Rev 1.3   Apr 29 1999 14:56:34   iarustam
   Bug fixes
   
      Rev 1.2   Mar 04 1999 13:32:12   iarustam
   Bug fixes
   
      Rev 1.0   Feb 23 1999 11:40:42   iarustam
   Initial Release
   
      Rev 1.0   Feb 17 1999 10:09:36   iarustam
   Initial Release
   
      Rev 1.1   Jan 22 1999 10:59:12   iarustam
   Bug fixes and core 1_3_0 changes
   
      Rev 1.1   Jan 21 1999 13:08:32   iarustam
   Bug fixes
   
   
      Rev 1.1   Dec 18 1998 13:43:42   iarustam
    

************************************************************************************/
// MerchantDefinition.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CMerchantDefinition dialog
#include "MerchantBlocking.h"
#include "MerchantCapabilities.h"
#include "MerchantContact.h"
#include "MerchantLocation.h"
#include "MerchantSettlement.h"
#include "MerchantVelocity.h"
#include "MerchantGeneral.h"
#include "ChangeHistory.h"


class CMerchantDefinition : public CPropertyPage
{
	DECLARE_DYNCREATE(CMerchantDefinition)

// Construction
public:
	int m_nDialogMode;
	char m_strCurrentData[151];
	char m_strPreviousData[151];
	char m_strFieldTag[50];
	char strFilePrimaryKey[50];
	MCF01 sMerchantRecordOld;

	CMerchantLocation*           pLocationPage;
	CMerchantGeneral*            pGeneralPage;
    CMerchantCapabilities*       pCapabilitiesPage;
    CMerchantSettlement *        pSettlementPage;
    CMerchantVelocity*           pVelocityPage;    
    CMerchantContact*            pContactPage;
    CMerchantBlocking*           pBlockingPage;
	CChangeHistory*              pChangeHistoryPage;

	CMerchantDefinition();
	~CMerchantDefinition();
	BOOL CheckDialogForCompleteness();
	void PopulateHistoryTable();
	BOOL PopulateMerchantDatabaseStructureFromDialog (pMCF01, pMCT01);
	void PopulateMerchantDialogFromDatabase (MCF01);
	CTime c_time;
// Dialog Data
	//{{AFX_DATA(CMerchantDefinition)
	enum { IDD = IDD_MERCHANT_DEFINITION };
	CButton	m_SearchButton;
	CButton	m_ViewButton;
	CButton	m_UpdateButton;
	CButton	m_ListButton;
	CButton	m_DeleteButton;
	CButton	m_AddButton;
	CEdit	m_CntlOrganizationIDFld;
	CEdit	m_CntlMerchantIDFld;
	CString	m_StrMerchantIDFld;
	CString	m_StrOrganizationIDFld;
	CString	m_StrMessageLbl;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CMerchantDefinition)
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
	//{{AFX_MSG(CMerchantDefinition)
	afx_msg void OnAddButton();
	virtual BOOL OnInitDialog();
	afx_msg void OnDeleteButton();
	afx_msg void OnListButton();
	afx_msg void OnUpdateButton();
	afx_msg void OnViewButton();
	afx_msg void OnSearch();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};
