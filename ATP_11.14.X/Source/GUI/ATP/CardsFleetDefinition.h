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
* $Log:   N:\POS\PVCS6.6\EPICPORTZ\PTE\Equitable\ATP_gui\CardsFleetDefinition.h  $
   
      Rev 1.7   Apr 01 2005 10:13:34   lmitchel
   Initial Checking prior to integration testing for updates made to support country/curreny code limits (scr13929), unulimited purchase card MCC ranges, and purchase card transaction limits(scr12785)
   
      Rev 1.6   Dec 01 2003 11:28:52   lmitchel
   Added AMEX Online Updates - scr1200
   
      Rev 1.5   Jan 05 2000 14:53:10   gbarnes
   Major update. Not updated to PVCS recently.
   PVCS was locked.
   
      Rev 1.1   Sep 22 1999 15:45:22   iarustam
   Added manual batch entry GUI
   
      Rev 1.0   Jul 01 1999 14:17:04   iarustam
   Initial Release
   
      Rev 1.0   Jun 28 1999 13:58:12   iarustam
   Initial Release
   
      Rev 1.2   Apr 29 1999 14:56:02   iarustam
   Bug fixes
   
      Rev 1.1   Mar 16 1999 16:29:38   iarustam
   Bug fixes
   
      Rev 1.0   Feb 23 1999 11:40:10   iarustam
   Initial Release
   
      Rev 1.0   Feb 17 1999 10:09:14   iarustam
   Initial Release
   
      Rev 1.1   Jan 22 1999 10:58:08   iarustam
   Bug fixes and core 1_3_0 changes
   
      Rev 1.1   Jan 21 1999 13:07:12   iarustam
   Bug fixes
   
   
      Rev 1.1   Dec 18 1998 13:42:46   iarustam
    
*
************************************************************************************/
// CardsFleetDefinition.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CCardsFleetDefinition dialog
#ifndef CCARDSFLEETDEFINITION
#define CCARDSFLEETDEFINITION

#include "CardsMerchantLimit.h"
#include "CardsProcessing.h"
#include "CardsTime1.h"
#include "CardsTime2.h"
#include "CardsTime3a.h"
#include "CardsVelocity.h" 
#include "ChangeHistory.h"
#include "CardsPinVerification.h"
#include "CntryAmtLimits.h" 
#include "CardsProfile.h"
class CCardsFleetDefinition : public CPropertyPage
{
	DECLARE_DYNCREATE(CCardsFleetDefinition)

// Construction
public:
	CCardsFleetDefinition();
	~CCardsFleetDefinition();
    
    char m_strCurrentData[151];
	char m_strPreviousData[151];
	char m_strFieldTag[50];
    char strFilePrimaryKey[50];

	CAF01 sProfileIdOld;
	FAF01 sFleetCardOld;
	CAF01_EMV sEMVProfileOld;
	KEY3DS01 sKey3ds01Old;

    int m_nDialogMode;
    int m_nCardMode;

    void PopulateHistoryTable();    

    BOOL PopulateProfileIdDialogFromDatabase ( CAF01, FAF01, CAF01_EMV ,KEY3DS01);    
    BOOL PopulateProfileIdDatabaseStructureFromDialog ( pCAF01 , pFAF01, pCAF01_EMV, pKEY3DS01 ); 
    BOOL CheckDialogForCompleteness();
    BOOL CheckTime();

    CCardsMerchantLimit *pMerchantLimitPage;
    CCardsProcessing *pProcessingPage;
    CCardsTime1 *pTime1Page;
    CCardsTime2 *pTime2Page;
    CCardsTime3a *pTime3Page;
    CCardsVelocity *pVelocityPage;
	CChangeHistory *pChangeHistoryPage;
    CCardsPinVerification *ppinDlg;
	CCntryAmtLimits *pCntryAmtLimitsPage;
	CCardsProfile *pCardsProfilePage;


	
	CTime c_time;

   // CCardsPinVerification  *ppinDlg;

// Dialog Data
	//{{AFX_DATA(CCardsFleetDefinition)
	enum { IDD = IDD_CARDS_FLEET_DEFINITION };
	CButton	m_ViewButton;
	CButton	m_UpdateButton;
	CButton	m_ListButton;
	CButton	m_DeleteButton;
	CButton	m_AddButton;
	CEdit	m_CntlType_Fld;
	CEdit	m_CntlProfileId_Fld;
	CString	m_StrMessageLbl;
	CString	m_StrProfileId_Fld;
	CString	m_StrType_Fld;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CCardsFleetDefinition)
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
	//{{AFX_MSG(CCardsFleetDefinition)
	virtual BOOL OnInitDialog();
	afx_msg void OnCardsAddButton();
	afx_msg void OnCardsDeleteButton();
	afx_msg void OnCardsListButton();
	afx_msg void OnCardsUpdateButton();
	afx_msg void OnCardsViewButton();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};
#endif
