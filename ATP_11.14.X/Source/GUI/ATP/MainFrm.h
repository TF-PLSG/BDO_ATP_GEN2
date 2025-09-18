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
* $Log:   N:\POS\PVCS6.6\EPICPORTZ\PTE\Equitable\ATP_gui\MainFrm.h  $
 
		Rev 1.17   Jan 01 2011  11:30:20   TF-SRIKANTH
   Added PCI User Password validations,application idletime calculation,unlock user account from GUI
   
   
      Rev 1.16   Sep 05 2006 10:46:32   dirby
   Added code for a new Cash Bonus program (see CB_... files)
   SCR 22385
   
      Rev 1.15   Jul 07 2005 15:49:32   lmitchel
   Implementation of Fraud Guard Requirements - New Fraud Guard sub  menu under Network Definition
   
      Rev 1.14   Dec 01 2003 11:29:30   lmitchel
   Added AMEX Online Updates - scr1200
   
      Rev 1.13   Nov 16 2001 13:22:24   lmitchel
   
      Rev 1.12   Apr 17 2000 09:57:26   iarustam
   added bin blocking
   
   
      Rev 1.11   Jan 05 2000 14:53:46   gbarnes
   Major update. Not updated to PVCS recently.
   PVCS was locked.
   
      Rev 1.1   Sep 22 1999 15:51:40   iarustam
   Added manual batch entry GUI
   
      Rev 1.0   Jul 01 1999 14:17:38   iarustam
   Initial Release
   
      Rev 1.0   Jun 28 1999 13:58:42   iarustam
   Initial Release
   
      Rev 1.8   May 24 1999 15:00:06   iarustam
   Bug fixes
   
      Rev 1.7   May 21 1999 10:16:56   iarustam
   Bug fixes
   
   
      Rev 1.6   Apr 29 1999 14:56:30   iarustam
   Bug fixes
   
      Rev 1.5   Apr 16 1999 09:44:30   iarustam
   Added Tran lookup menu
   
      Rev 1.4   Mar 10 1999 12:11:46   iarustam
   Bug fixes
   
   
      Rev 1.3   Mar 08 1999 17:20:34   iarustam
   Bug fixes
   
   
      Rev 1.2   Mar 04 1999 13:32:08   iarustam
   Bug fixes
   
      Rev 1.0   Feb 23 1999 11:40:36   iarustam
   Initial Release
   
      Rev 1.0   Feb 17 1999 10:09:32   iarustam
   Initial Release
   
    
   
      Rev 1.1   Dec 18 1998 13:43:30   iarustam
    
*
************************************************************************************/
// MainFrm.h : interface of the CMainFrame class
//
/////////////////////////////////////////////////////////////////////////////
#include "userauditlog1.h"
#include "userslist.h"
#include "GroupPropertySheet.h"
#include "UserPropertySheet.h"
#include "BinPropertySheet.h"
#include "SrvcMgr.h"
#include "DialogPropertySheet.h"
#include "CardTypePropertySheet.h"
#include "WorkstationPropertySheet.h"
#include "MerchantPropertySheet.h"
#include "ConversionPropertySheet.h"
#include "FleetPropertySheet.h"
#include "CalendarSheet.h"
#include "SchedulerPropertySheet.h"
#include "MerchantTerminalPropertySheet.h"
#include "PanPropertySheet.h"
#include "CreditPropertySheet.h"
#include "DebitPropertySheet.h"
#include "CFSheet.h"
#include "NegativeGroupPropertySheet.h"
#include "NetworkAcquirerPropertySheet.h"
#include "NetworkIssuerPropertySheet.h"
#include "NegativeCardFileControlPropertySheet.h"
#include "CardPlusPropSheet.h"
#include "RelatedPropertySheet.h"
#include "CurrencyCodePropertySheet.h"
#include "PurchaseCardPropertySheet.h"
#include "DeferredPropertySheet.h"
#include "FraudGuardPropertySheet.h"
#include "CB_PropertySheet.h"
#include "DBPasswordsPropertySheet.h"
#include "DualEnKeyCtrlPropertySheet.h"
#include "Lock.h"
#include "OverrideMerchantDenialPropertySheet.h"
#include "ArchiveDBPasswords.h"
#include "MraDBPasswords.h"
#include "MerchantGroupDefinitionSheet.h"
#include "MerchantPayParamSheet.h"
// raghu added
#include "SystemGlobalParameterSheet.h"
#include "BlockingPosEntryModesSheet.h"
#include "MC3DSecureOBSAuthenticationSheet.h"
#include "Visa3DS2ConfigPropertySheet.h"
#include "Amex3DS2PropertySheet.h"
#include "Cup3DS2PropertySheet.h"
#include "Diners3DS2PropertySheet.h"
#include "Jcb3DS2PropertySheet.h"
#include "FraudControlFraudRuleSheet.h"
#include "FraudControlConfigureLinkSheet.h"
#include "RulesDefinitionPropertySheet.h"
#include "ResetDcoMonitor.h"
#include "OnlinePINControlBDKKeySheet.h"
#include "CardsKeyblockPropertySheet.h"
#include "NetworkKeyblockPropertySheet.h"

class CMainFrame : public CFrameWnd
{
public: // create from serialization only
	CMainFrame();
	DECLARE_DYNCREATE(CMainFrame)
	
// Attributes
public:
	CUserAuditLog *m_pUserAuditLog;
	CUsersList *m_pUsersListDlg;
	CNegativeGroupPropertySheet* m_pNegGroupSheet;
	CUserPropertySheet* m_pUserSheet;
	CGroupPropertySheet* m_pGroupSheet;
	CBinPropertySheet* m_pBinSheet;
	CDialogPropertySheet* m_pDialogSheet;
	CCardTypePropertySheet* m_pCardTypeSheet;
	CWorkstationPropertySheet* m_pWorkstationSheet;
	CMerchantPropertySheet* m_pMerchantSheet;
	CConversionPropertySheet* m_pConversionSheet;
	CFleetPropertySheet* m_pFleetSheet;
	CCalendarSheet* m_pCalendarSheet;
	CSchedulerPropertySheet* m_pSchSheet;
	CMerchantTerminalPropertySheet* m_pTerminalSheet;
	CPanPropertySheet* m_pPanSheet;
	CCreditPropertySheet* m_pCreditSheet;
	CDebitPropertySheet* m_pDebitSheet;
	CCFSheet* m_pCCFSheet;
	CNetworkAcquirerPropertySheet* m_pAcquirerSheet;
	CNetworkIssuerPropertySheet* m_pIssuerSheet;
	CNegativeCardFileControlPropertySheet* m_pNegativeCardSheet;
	CCardPlusPropSheet* m_pCardPlusSheet;
	CRelatedPropertySheet* m_pRelatedSheet;
	CurrencyCodePropertySheet* m_pCurrencyCodeSheet;
	CPurchaseCardPropertySheet* m_pPurchaseSheet;
	CDeferredPropertySheet* m_pDeferredSheet;
	CFraudGuardPropertySheet* m_pFraudGuardSheet;
	CCB_PropertySheet* m_pCBSheet;
	CDBPasswordsPropertySheet* m_pDBPasswordsSheet;
	CDualEnKeyCtrlPropertySheet* m_pDualEnKeyCtrlPropertySheet;
	COverrideMerchantDenialPropertySheet* m_pOverrideMerchantDenialPropertySheet;
	CArchiveDBPasswords *m_pArchiveDBPasswordsDlg;
	CMraDBPasswords *m_pMraDBPasswordsDlg;
	CMerchantGroupDefinitionSheet *m_pMerchantGroupDefinitionSheet;
	CMerchantPayParamSheet *m_pMerchantPayParamSheet;
	CSystemGlobalParameterSheet *m_pSystemGlobalParametersheet;
	CBlockingPosEntryModePropertySheet *m_pBlockingPosEntryModePropertySheet;
	CMC3DSecureOBSAuthenticationSheet *m_pMC3DSecureOBSAuthenticationSheet;
	CVisa3DS2ConfigPropertySheet *m_pVisa3DS2ConfigPropertySheet;
	CAmex3DS2PropertySheet *m_pAmex3DS2PropertySheet;
	CCup3DS2PropertySheet *m_pCup3DS2PropertySheet;
	CDiners3DS2PropertySheet *m_pDiners3DS2PropertySheet;
	CJcb3DS2PropertySheet *m_pJcb3DS2PropertySheet;
	CFraudControlConfigureRuleSheet 		*m_pFraudControlSheet;
	CFraudControlConfigureLinkSheet          *m_pFraudControlConfigureLinkSheet;
	CRulesDefinitionPropertySheet  *m_pRulesDefinitionPropertySheet;
	CResetDcoMonitor  *m_pCResetDcoMonitor;
	COnlinePINConfigureBDKKeySheet *m_pBDKKeyPropertySheet;
	CCardsKeyblockPropertySheet *m_pCAF01KeyblockPropertySheet;
	CNetworkKeyblockPropertySheet *m_pNCF01KeyblockPropertySheet;

	//For Logout History Table
	//CLogin *pclogin;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMainFrame)
	public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	protected:
	virtual LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	UINT timer_handle;
	int cntr;	
	int idletime;
	int status, ret;

	char char_time[20];
	CTime c_time, shm_time;

	bool StopTimer();
	int StartTimer(int time);	
	void ResetCntr();
	short LockScreen();	
	HWND child_dlgID;
	
	afx_msg void OnMonitor_Server2();
	afx_msg void OnMonitor_Server3();
	afx_msg void OnMonitor_Server4();
	
protected:  // control bar embedded members
	CStatusBar  m_wndStatusBar;
//	CToolBar    m_wndToolBar;

// Generated message map functions
protected:
	//{{AFX_MSG(CMainFrame)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnUserDefinitionMenu();
	afx_msg void OnSystemGlobalParameter(); // Raghu added
	afx_msg void OnFraudControlConfigureRule();
	afx_msg void OnFraudControlConfigureLink();
	afx_msg void OnMerchantdefinition();
	afx_msg void OnClose();
	afx_msg void OnSystemadministrationGroupdefinition();
	afx_msg void OnBincontrolBinrangedefinition();
	afx_msg void OnDestroy();
	afx_msg void OnServiceManager();
	afx_msg void OnDialogManager();
	afx_msg void OnCardtype();
	afx_msg void OnSettleFile();
	afx_msg void OnMerchantRisk();
	afx_msg void OnFleetcard();
	afx_msg void OnCalenderDefinition();
	afx_msg void OnScheduler();
	afx_msg void OnTerminaldefinition();
	afx_msg void OnWorkstationAccess();
	afx_msg void OnPrimaryaccount();
	afx_msg void OnCreditcard();
	afx_msg void OnCardrecord();
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	afx_msg void OnCardStatus();
	afx_msg void OnMonitor();
	afx_msg void OnNetworkacquirer();
	afx_msg void OnNetworkissuer();
	afx_msg void OnRelatedAccount();
	afx_msg void OnCurrencyCodeList();
	afx_msg void OnPurchaseCard();
	afx_msg void OnMcc103();
	afx_msg void OnVoice();
	afx_msg void OnMcc102();
	afx_msg void OnDebitcard();
	afx_msg void OnCashBonus();
	afx_msg void OnDeferred();
	afx_msg void OnRepact();
	afx_msg void OnRepset();
	afx_msg void OnRepstat();
	afx_msg void OnTranLookup();
	afx_msg void OnUserStatusControl();
	afx_msg void OnFraudSystemControl();
	afx_msg void OnArchiveLookup();
	afx_msg void OnTranPOSLookup();
	afx_msg void OnCorpCardRep();
	afx_msg void OnTranRep();
	afx_msg void OnSummaryRep();
	afx_msg void OnJcbupdates();
	afx_msg void OnAmexupdates();
	afx_msg void OnFraudguarddefinition();
	afx_msg void OnCB_Definition();
	afx_msg void OnStipManager();
	afx_msg void OnDbpasswdsMenu();
	afx_msg void OnDualencrptionKey();
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnInitMenu(CMenu* pMenu);
	afx_msg void OnOverrideDenial();
	afx_msg void OnUsersListMenu();
	afx_msg void OnUserAuditLogMenu();
	afx_msg void OnResetMonitorConnections();
	afx_msg void OnMCCDefinition();
	afx_msg void OnArchDbpasswdsMenu();
	afx_msg void OnMraDbpasswdsMenu();
	afx_msg void OnMerchantPayParam();
	afx_msg void OnMerchantGroupDefinition();
	afx_msg void OnBlockingPosEntryModes();
	afx_msg void OnMC3DSecureOBS();
	afx_msg void OnVISA3DS2Config();
	afx_msg void OnAmex3ds2Parameter();
	afx_msg void OnCup3ds2Parameter();
	afx_msg void OnDiners3ds2Parameter();
	afx_msg void OnJcb3ds2Parameter();
	afx_msg void OnRulesDefinition();
	afx_msg void OnBDKKeyParameter();
	afx_msg void OnCAF01KeyblockParameter();
	afx_msg void OnNetworkkeyblockparameter();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	CDMLock * pLock;
public:
	//new chnages - amith
	afx_msg void OnResetmultiplemonitorconnections();
	afx_msg void OnDcoMonitor();
};

/////////////////////////////////////////////////////////////////////////////
