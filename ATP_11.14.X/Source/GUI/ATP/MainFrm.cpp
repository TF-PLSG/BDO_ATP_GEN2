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
* $Log:   N:\POS\PVCS6.6\EPICPORTZ\PTE\Equitable\ATP_gui\MainFrm.cpp  $
   
	 Rev   1.23     May 11.5.11          Krishna Kiran S@TF
	 Ucf03 changes replaced with Ucf01 table contents
	 Added Idle Time Calculation & calling Login screen on Idle Timeout
	 Added Creation of SHM & Mutex for Synchronisation of dislpalying Login
	 Form from different windows

	 Rev 1.22   Jan 01 2011 11:30:22  
    Added PCI changes -Password validations,locking user account ,idletime calculation
	
      Rev 1.21   Sep 05 2006 10:46:32   dirby
   Added code for a new Cash Bonus program (see CB_... files)
   SCR 22385
   
      Rev 1.20   Jul 07 2005 15:49:32   lmitchel
   Implementation of Fraud Guard Requirements - New Fraud Guard sub  menu under Network Definition
   
      Rev 1.19   Dec 01 2003 11:29:30   lmitchel
   Added AMEX Online Updates - scr1200
   
      Rev 1.18   Nov 16 2001 13:22:22   lmitchel
   
      Rev 1.17   Apr 17 2000 09:57:24   iarustam
   added bin blocking
   
   
      Rev 1.16   Dec 07 1999 10:57:18   mbysani
   changes status code
   
      Rev 1.3   Sep 30 1999 16:17:20   npogosya
   Bug fixes
   
      Rev 1.1   Sep 22 1999 15:51:34   iarustam
   Added manual batch entry GUI
   
      Rev 1.0   Jul 01 1999 14:17:38   iarustam
   Initial Release
   
      Rev 1.0   Jun 28 1999 13:58:42   iarustam
   Initial Release
   
      Rev 1.13   Jun 11 1999 14:07:46   iarustam
   Bug fixes
   
   
      Rev 1.12   Jun 02 1999 16:25:18   iarustam
   Bug fixes
   
      Rev 1.11   May 24 1999 15:00:06   iarustam
   Bug fixes
   
      Rev 1.10   May 21 1999 10:16:54   iarustam
   Bug fixes
   
   
      Rev 1.9   May 10 1999 11:20:14   dperson
   Standardized error messages
   
      Rev 1.8   Apr 29 1999 14:56:28   iarustam
   Bug fixes
   
      Rev 1.7   Apr 16 1999 09:44:28   iarustam
   Added Tran lookup menu
   
      Rev 1.6   Mar 22 1999 11:12:32   iarustam
   Bug fixes
   
   
      Rev 1.5   Mar 16 1999 16:30:00   iarustam
   Bug fixes
   
      Rev 1.4   Mar 10 1999 12:11:46   iarustam
   Bug fixes
   
   
      Rev 1.3   Mar 08 1999 17:20:32   iarustam
   Bug fixes
   
   
      Rev 1.2   Mar 04 1999 13:30:40   iarustam
   Bug fixes
   
      Rev 1.0   Feb 23 1999 11:40:36   iarustam
   Initial Release
   

    
*
************************************************************************************/
// MainFrm.cpp : implementation of the CMainFrame class
//

#include "stdafx.h"
#include "Atp.h"

extern "C"
{
   #include "ntutils.h"

}

#include "CardStatus.h"
#include "NegativeBinRange.h"
#include "MainFrm.h"
#include "MCC103.h"
#include "MCC102.h"
#include "VoiceAuthInput.h"
#include "CashBonus.h"
#include "AMEXUPDATES1.h"
#include "JCBUPDATES.h"
#include "IdleLoginfrm.h"
#include "SharedMem.h"
#include "Login.h"
#include "MCCRangeDefinition.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern "C"
{
   #include "ntutils.h"
   #include "Txutils.h"

}

extern int IsMon2Up, IsMon3Up, IsMon4Up;
extern CHAR hostname1[50], hostname2[50], hostname3[50], hostname4[50];
extern SHORT port_num1, port_num2, port_num3, port_num4;

extern UCF01 ucf01;
CString _user_name;
extern CString idle_time_val_str;
extern CSharedMem shrmem;
extern CString workstation;

CIdleLoginfrm idlelogin;
CTime cur_time;
extern CLogin login;
extern char New_Logout_Date1[15];
extern char Prev_Logout_Date1[15];
//CLogin *plogin;

extern CHAR wstation[21];
extern DWORD wsize;

/////////////////////////////////////////////////////////////////////////////
// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	//{{AFX_MSG_MAP(CMainFrame)
	ON_WM_CREATE()
	ON_COMMAND(ID_USER_DEFINITION_MENU, OnUserDefinitionMenu)
	// raghu added 
	//ON_COMMAND(ID_SYSTEMGLOBALPARAMETER_MENU,OnSystemGlobalParameter)ID_SYSTEM_GLOBAL_PARAMETERS
		ON_COMMAND(ID_MC3DSECUREOBS_MAIN,OnMC3DSecureOBS)
		ON_COMMAND(ID_VISA3DS2Config_MAIN,OnVISA3DS2Config)
		ON_COMMAND(ID_SYSTEM_GLOBAL_PARAMETERS,OnSystemGlobalParameter)
		ON_COMMAND(ID_AMEX3DS2_PARAMETERS,OnAmex3ds2Parameter)
		ON_COMMAND(ID_CUP3DS2_PARAMETERS,OnCup3ds2Parameter)
		ON_COMMAND(ID_DINERS3DS2_PARAMETERS,OnDiners3ds2Parameter)
		ON_COMMAND(ID_JCB3DS2_PARAMETERS,OnJcb3ds2Parameter)
		ON_COMMAND(ID_CONFIGURE_BDK_KEY_PAGE,OnBDKKeyParameter)
		ON_COMMAND(ID_CAF01_KEYBLK_MAIN,OnCAF01KeyblockParameter)
	ON_COMMAND(ID_MERCHANTDEFINITION, OnMerchantdefinition)
	ON_WM_CLOSE()
	ON_COMMAND(ID_SYSTEMADMINISTRATION_GROUPDEFINITION, OnSystemadministrationGroupdefinition)
	ON_COMMAND(ID_BINCONTROL_BINRANGEDEFINITION, OnBincontrolBinrangedefinition)
	ON_WM_DESTROY()
	ON_COMMAND(ID_SERVICE_MANAGER, OnServiceManager)
	ON_COMMAND(ID_DIALOG_MANAGER, OnDialogManager)
	ON_COMMAND(ID_CARDTYPE, OnCardtype)
	ON_COMMAND(ID_SETTLE_FILE, OnSettleFile)
	ON_COMMAND(ID_MERCHANT_RISK, OnMerchantRisk)
	ON_COMMAND(ID_FLEETCARD, OnFleetcard)
	ON_COMMAND(ID_CALENDER_DEFINITION, OnCalenderDefinition)
	ON_COMMAND(ID_SCHEDULER, OnScheduler)
	ON_COMMAND(ID_TERMINALDEFINITION, OnTerminaldefinition)
	ON_COMMAND(ID_WORKSTATION_ACCESS, OnWorkstationAccess)
	ON_COMMAND(ID_PRIMARYACCOUNT, OnPrimaryaccount)
	ON_COMMAND(ID_CREDITCARD, OnCreditcard)
	ON_COMMAND(ID_CARDRECORD, OnCardrecord)
	ON_WM_HELPINFO()
	ON_COMMAND(ID_CARD_STATUS, OnCardStatus)
	ON_COMMAND(ID_MONITOR, OnMonitor)
	ON_COMMAND(ID_MONITOR2, OnMonitor_Server2)
	ON_COMMAND(ID_MONITOR3, OnMonitor_Server3)
	ON_COMMAND(ID_MONITOR4, OnMonitor_Server4)
	ON_COMMAND(ID_NETWORKACQUIRER, OnNetworkacquirer)
	ON_COMMAND(ID_NETWORKISSUER, OnNetworkissuer)
	ON_COMMAND(ID_NCF01_KEYBLOCK_MAIN, OnNetworkkeyblockparameter)
	ON_COMMAND(ID_RELATED_ACCOUNT, OnRelatedAccount)
	ON_COMMAND(IDD_CURRENCY_CODE_DEFINITION, OnCurrencyCodeList)
	ON_COMMAND(ID_PURCHASE_CARD, OnPurchaseCard)
	ON_COMMAND(ID_MCC103, OnMcc103)
	ON_COMMAND(ID_VOICE, OnVoice)
	ON_COMMAND(ID_MCC102, OnMcc102)
	ON_COMMAND(ID_DEBITCARD, OnDebitcard)
	ON_COMMAND(ID_CASH_BONUS, OnCashBonus)
	ON_COMMAND(ID_DEFERRED, OnDeferred)
	ON_COMMAND(ID_REPACT, OnRepact)
	ON_COMMAND(ID_REPSET, OnRepset)
	ON_COMMAND(ID_REPSTAT, OnRepstat)
	ON_COMMAND(ID_TRAN_LOOKUP, OnTranLookup)
	ON_COMMAND(ID_ARCHIVE_LOOKUP, OnArchiveLookup)
	ON_COMMAND(ID_TRAN_LOOKUP_POS, OnTranPOSLookup)
	ON_COMMAND(ID_CORP_CARD_REP, OnCorpCardRep)
	ON_COMMAND(ID_TRAN_REP, OnTranRep)
	ON_COMMAND(ID_SUMMARY_REP, OnSummaryRep)
	ON_COMMAND(ID_JCBUPDATES, OnJcbupdates)
	ON_COMMAND(ID_AMEXUPDATES, OnAmexupdates)
	ON_COMMAND(ID_FRAUDGUARDDEFINITION, OnFraudguarddefinition)
    ON_COMMAND(ID_CB_DEFINITION, OnCB_Definition)
	ON_COMMAND(ID_STIP_MANAGER, OnStipManager)
	ON_COMMAND(ID_DBPASSWDS_MENU, OnDbpasswdsMenu)
	ON_COMMAND(ID_DUALENCRPTION_KEY, OnDualencrptionKey)
	ON_WM_CHAR()
	ON_WM_INITMENU()
	ON_COMMAND(ID_OVERRIDE_DENIAL, OnOverrideDenial)
	ON_COMMAND(ID_CHIPFALLBACK_RULES_DEFINITION,OnRulesDefinition)
	ON_COMMAND(ID_USERS_LIST_MENU, OnUsersListMenu)
	ON_COMMAND(ID_USER_AUDIT_MENU, OnUserAuditLogMenu)
	ON_COMMAND(ID_CLEAR_MONITOR_SOCKETS, OnResetMonitorConnections)
	ON_COMMAND(ID_MCCDEFINITION, OnMCCDefinition)
	ON_COMMAND(ID_ARCH_DBPASSWDS_MENU, OnArchDbpasswdsMenu)
	ON_COMMAND(ID_MRA_DBPASSWDS_MENU, OnMraDbpasswdsMenu)
	ON_WM_MOUSEMOVE()
	ON_WM_CAPTURECHANGED()
	ON_WM_LBUTTONDOWN()
	ON_COMMAND(ID_MERCHANT_PAY_PARAM, OnMerchantPayParam)
	ON_COMMAND(ID_MERCHANT_GROUP_DEFINITION, OnMerchantGroupDefinition)

	

	// User Status Control
	ON_COMMAND(ID_USER_STATUS_CONTROL, OnUserStatusControl)
	ON_COMMAND(ID_CONFIGURE_FRAUD_RULE, OnFraudControlConfigureRule)
	ON_COMMAND(ID_CONFIGURE_LINK, OnFraudControlConfigureLink)
	



	//}}AFX_MSG_MAP
	// Global help commands
	ON_COMMAND(ID_HELP_FINDER, CFrameWnd::OnHelpFinder)
	ON_COMMAND(ID_HELP, CFrameWnd::OnHelp)
	ON_COMMAND(ID_CONTEXT_HELP, CFrameWnd::OnContextHelp)
	ON_COMMAND(ID_DEFAULT_HELP, CFrameWnd::OnHelpFinder)
	ON_COMMAND(ID_BLOCKPOSENTRIES, OnBlockingPosEntryModes)

	
	//new changes - amith
	ON_COMMAND(ID_SYSTEMOPERATIONS_RESETMULTIPLEMONITORCONNECTI, &CMainFrame::OnResetmultiplemonitorconnections)
	ON_COMMAND(ID_SYSTEMOPERATIONS_DCOMONITOR, &CMainFrame::OnDcoMonitor)

END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // status line indicator
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};

/////////////////////////////////////////////////////////////////////////////
// CMainFrame construction/destruction

CMainFrame::CMainFrame()
{
	m_pUserSheet = new CUserPropertySheet(" ");
	m_pGroupSheet = new CGroupPropertySheet(" ");
	m_pBinSheet = new CBinPropertySheet(" ");
	m_pDialogSheet = new CDialogPropertySheet("");
	m_pCardTypeSheet = new CCardTypePropertySheet ("");
//	m_pSettleFileSheet = new CSettleFilePropertySheet ("");
	m_pMerchantSheet = new CMerchantPropertySheet ("Merchant Definition");
	m_pConversionSheet = new CConversionPropertySheet ( "Currency Code Definition");
	m_pFleetSheet = new CFleetPropertySheet ("Fleet Card Definition");
	m_pCalendarSheet = new CCalendarSheet ("");
	m_pSchSheet = new CSchedulerPropertySheet("");
	m_pTerminalSheet = new CMerchantTerminalPropertySheet("");
	m_pWorkstationSheet = new CWorkstationPropertySheet ("");
	m_pPanSheet = new CPanPropertySheet("Primary Account Definition");
	m_pCreditSheet = new CCreditPropertySheet ("Credit Card Definition");
	m_pDebitSheet = new CDebitPropertySheet ("Debit Card Definition");
	m_pCCFSheet = new CCFSheet("Card Record Definition");
	m_pNegGroupSheet = new CNegativeGroupPropertySheet ("Negative Group Definition");
	m_pAcquirerSheet = new CNetworkAcquirerPropertySheet ("Acquiring Network Definition");
	m_pIssuerSheet = new CNetworkIssuerPropertySheet ("Issuing Network Definition");
	m_pNegativeCardSheet = new CNegativeCardFileControlPropertySheet ("Negative Card File Control");
	m_pCardPlusSheet = new CCardPlusPropSheet ("Card Plus Information");
	m_pRelatedSheet = new CRelatedPropertySheet ( "Related Account Definition");
	m_pCurrencyCodeSheet = new CurrencyCodePropertySheet ( "Currency Code List");
	m_pPurchaseSheet = new CPurchaseCardPropertySheet ( "Purchase Card Definition");
	m_pDeferredSheet = new CDeferredPropertySheet ( "Deferred Purchase Definition");
	m_pFraudGuardSheet = new CFraudGuardPropertySheet ( "Fraud Card Definition");
	m_pCBSheet = new CCB_PropertySheet ( "Cash Bonus Definition");
	m_pDBPasswordsSheet = new CDBPasswordsPropertySheet(" ");
	m_pDualEnKeyCtrlPropertySheet=new CDualEnKeyCtrlPropertySheet(" ");
	m_pOverrideMerchantDenialPropertySheet = new COverrideMerchantDenialPropertySheet("Override Merchant Denial");
	m_pUsersListDlg = new CUsersList();
	m_pUserAuditLog = new CUserAuditLog();
	m_pArchiveDBPasswordsDlg = new CArchiveDBPasswords();
	m_pMraDBPasswordsDlg = new CMraDBPasswords();
	//Instantiate the Object for SHM
	pLock = new CDMLock("IdleLock");
	m_pSystemGlobalParametersheet = new CSystemGlobalParameterSheet("");
	m_pMerchantGroupDefinitionSheet = new CMerchantGroupDefinitionSheet("Merchant Group Definition");
	m_pMerchantPayParamSheet = new CMerchantPayParamSheet("MerchantPay Parameters");
	m_pBlockingPosEntryModePropertySheet = new CBlockingPosEntryModePropertySheet("Block POS Entry Mode");
	m_pMC3DSecureOBSAuthenticationSheet = new CMC3DSecureOBSAuthenticationSheet("MC 3D SECURE OBS AUTHENTICATION");
	m_pVisa3DS2ConfigPropertySheet = new CVisa3DS2ConfigPropertySheet("VISA 3D Secure CAVV Authentication");
	m_pAmex3DS2PropertySheet = new CAmex3DS2PropertySheet("AMEX 3D Secure Parameters");
	m_pCup3DS2PropertySheet = new CCup3DS2PropertySheet("CUP 3DS2 Parameters");
	m_pDiners3DS2PropertySheet = new CDiners3DS2PropertySheet("DINERS 3DS2 Parameters");
	m_pJcb3DS2PropertySheet = new CJcb3DS2PropertySheet("JCB 3DS2 Parameters");
	m_pBDKKeyPropertySheet =  new COnlinePINConfigureBDKKeySheet("BDK Key Parameters");
	m_pFraudControlSheet = new CFraudControlConfigureRuleSheet ("Configure Fraud Rule");
	m_pFraudControlConfigureLinkSheet = new CFraudControlConfigureLinkSheet ("Configure Fraud Rule");
	m_pRulesDefinitionPropertySheet = new CRulesDefinitionPropertySheet("Rules Definition");
	m_pCAF01KeyblockPropertySheet = new CCardsKeyblockPropertySheet("CAF01 Keyblock Parameters");
	m_pNCF01KeyblockPropertySheet = new CNetworkKeyblockPropertySheet("NCF01 Keyblock Parameters");
	m_pCResetDcoMonitor = new CResetDcoMonitor();
}


CMainFrame::~CMainFrame()
{
	delete pLock;
	pLock = NULL;
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
  
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

    SetWindowText ("ATP");
	
	/*if (!m_wndToolBar.Create(this) ||
		!m_wndToolBar.LoadToolBar(IDR_MAINFRAME))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}*/

	if (!m_wndStatusBar.Create(this) ||
		!m_wndStatusBar.SetIndicators(indicators,
		  sizeof(indicators)/sizeof(UINT)))
	{
		TRACE0("Failed to create status bar\n");
		return -1;      // fail to create
	}

   //Krishna, 10.4.11 : Below changes are made for Idle time calculation, 
   //displaying the Login Screen & blocking other screens from editing.

   //Start the Timer on Init
    timer_handle = StartTimer(1000);
	if (timer_handle == 0)
    {
          AfxMessageBox("Unable to obtain timer");
    }

	ResetCntr();

	//Get the Idle Time retreived from tf.ini file
   idletime = atoi(idle_time_val_str);

   //Create Mutex Lock for SHM
   shrmem.CreateMutexLock();   
   
   //Krishna : Get the Current time & store it in SHM
	c_time = CTime::GetCurrentTime();	
	shrmem.SetLastUpdateTime(c_time);

	// TODO: Remove this if you don't want tool tips or a resizeable toolbar
//	m_wndToolBar.SetBarStyle(m_wndToolBar.GetBarStyle() |
//		CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC);

	// TODO: Delete these three lines if you don't want the toolbar to
	//  be dockable
//	m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	EnableDocking(CBRS_ALIGN_ANY);
//	DockControlBar(&m_wndToolBar);


	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CFrameWnd::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWnd::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CMainFrame message handlers


void CMainFrame::OnUserDefinitionMenu() 
{
	m_pUserSheet->SetData();
	m_pUserSheet->SetTitle("User Definition");
	m_pUserSheet->DoModal();
}

void CMainFrame::OnMerchantdefinition() 
{
	m_pMerchantSheet->SetData();
	m_pMerchantSheet->DoModal();
	
}

//Raghu added 
void CMainFrame::OnSystemGlobalParameter() 
{
	m_pSystemGlobalParametersheet->SetData();
	m_pSystemGlobalParametersheet->SetTitle("System Global Parameters");
	m_pSystemGlobalParametersheet->DoModal();
}
void CMainFrame::OnMC3DSecureOBS()
{
	m_pMC3DSecureOBSAuthenticationSheet->SetData();
	m_pMC3DSecureOBSAuthenticationSheet->SetTitle("MC 3DSecure OBS Authentication");
	m_pMC3DSecureOBSAuthenticationSheet->DoModal();
}

void CMainFrame::OnVISA3DS2Config()
{
	m_pVisa3DS2ConfigPropertySheet->SetData();
	m_pVisa3DS2ConfigPropertySheet->SetTitle("VISA 3DS2 Config");
	m_pVisa3DS2ConfigPropertySheet->DoModal();
}

void CMainFrame::OnAmex3ds2Parameter()
{
	m_pAmex3DS2PropertySheet->SetData();
	m_pAmex3DS2PropertySheet->SetTitle("AMEX 3DS2 PARAMETERS");
	m_pAmex3DS2PropertySheet->DoModal();
}

void CMainFrame::OnCup3ds2Parameter()
{
	m_pCup3DS2PropertySheet->SetData();
	m_pCup3DS2PropertySheet->SetTitle("CUP 3DS2 Parameters");
	m_pCup3DS2PropertySheet->DoModal();
}

void CMainFrame::OnDiners3ds2Parameter()
{
	m_pDiners3DS2PropertySheet->SetData();
	m_pDiners3DS2PropertySheet->SetTitle("DINERS 3DS2 Parameters");
	m_pDiners3DS2PropertySheet->DoModal();
}

void CMainFrame::OnJcb3ds2Parameter()
{
	m_pJcb3DS2PropertySheet->SetData();
	m_pJcb3DS2PropertySheet->SetTitle("JCB 3DS2 Parameters");
	m_pJcb3DS2PropertySheet->DoModal();
}


void CMainFrame::OnRulesDefinition()
{
	m_pRulesDefinitionPropertySheet->SetData();
	m_pRulesDefinitionPropertySheet->SetTitle("Rules Definition");
	m_pRulesDefinitionPropertySheet->DoModal();
}

void CMainFrame::OnFraudControlConfigureRule() 
{
	m_pFraudControlSheet->SetData();
	m_pFraudControlSheet->SetTitle("Configure Fraud Rule");
	m_pFraudControlSheet->DoModal();
}
void CMainFrame::OnFraudControlConfigureLink() 
{
	m_pFraudControlConfigureLinkSheet->SetData();
	m_pFraudControlConfigureLinkSheet->SetTitle("Configure Link");
	m_pFraudControlConfigureLinkSheet->DoModal();
}

void CMainFrame::OnBDKKeyParameter() 
{
	m_pBDKKeyPropertySheet->SetData();
	m_pBDKKeyPropertySheet->SetTitle("BDK Key Parameters"); 
	m_pBDKKeyPropertySheet->DoModal();
}

void CMainFrame::OnCAF01KeyblockParameter() 
{
	m_pCAF01KeyblockPropertySheet->SetData();
	m_pCAF01KeyblockPropertySheet->SetTitle("CAF01 Keyblock Parameters"); 
	m_pCAF01KeyblockPropertySheet->DoModal();
}

void CMainFrame::OnNetworkkeyblockparameter() 
{
	m_pNCF01KeyblockPropertySheet->SetData();
	m_pNCF01KeyblockPropertySheet->SetTitle("NCF01 Keyblock Parameters"); 
	m_pNCF01KeyblockPropertySheet->DoModal();
}
void CMainFrame::OnClose() 
{	INT_PTR nResult=0;	
	// TODO: Add your message handler code here and/or call default
//	m_pMainViewObject->AppCleanUp() ; 
	char error_msg[200] = {0};
	char toDayDate[20] = ""; char hhmm[10]="";
	int rcode = txdsapi_get_record( (pBYTE)&ucf01,sizeof (UCF01), UCF01_DATA, error_msg );

	strncpy(Prev_Logout_Date1,(char *)ucf01.Logout_Date,12);

	if( rcode != PTEMSG_OK )
		AfxMessageBox( error_msg, MB_ICONSTOP | MB_OK );
	CTime theTime = CTime::GetCurrentTime();//Get Todays date
	
	int	year= 	theTime.GetYear ();
	int month = theTime.GetMonth();
    int day = theTime.GetDay ();
	int hour = theTime.GetHour();
	int min = theTime.GetMinute();

		sprintf(toDayDate,"%d%02d%02d",year,month,day);
	
	ucf01.user_info_4[0] = year - 2000;
	ucf01.user_info_4[1] = theTime.GetMonth();
	ucf01.user_info_4[2] = theTime.GetDay();
	ucf01.user_info_4[3] = theTime.GetHour();
	ucf01.user_info_4[4] = theTime.GetMinute();
	ucf01.user_info_4[5] = theTime.GetSecond();
	ucf01.user_info_4[6] = '\0';

		/*******************************Ramya Added*****************************/
		// After logging inOn clicking the Close button in the main page of UI, User Must Log OUT

		//strncpy((char *)ucf01.Login_WorkStationId,"0",wsize);
		strncpy((char *)ucf01.Login_Status,"0",LOGIN_STATUS_LEN);
		ucf01.Login_Status[1] = '\0';
		//strncpy((char *)ucf01.Login_Date,toDayDate, sizeof(toDayDate));

		/*strncat(toDayDate,(char *)hour,2);
		strncat(toDayDate,(char *)min,2);*/
		sprintf(hhmm, "%02d%02d",hour,min);
		strncat(toDayDate,hhmm,4);
		strncpy((char *)ucf01.Logout_Date,toDayDate,12);
		login.New_Logout_Date = ucf01.Logout_Date;
		strncpy(New_Logout_Date1,login.New_Logout_Date,12);

		login.PopulateLoginControlStructure();
		login.PopulateHistoryTable();
		/*********************************************************************/

		
	/****set the logged status "L" = Logged; "NL" - Not Logged In ; -TF_Veena*****
	strcpy((char *)ucf01.buffer,"NL");
	/****************************************************************************/

	rcode = txdsapi_update_record( (pBYTE)&ucf01, sizeof (UCF01), UCF01_DATA, error_msg );
	if( rcode != PTEMSG_OK )
		AfxMessageBox( error_msg, MB_ICONSTOP | MB_OK );
	


	CFrameWnd::OnClose();
}



void CMainFrame::OnSystemadministrationGroupdefinition() 
{
	m_pGroupSheet->SetData();
	m_pGroupSheet->SetTitle("Group Definition");
	m_pGroupSheet->DoModal();
	
}

void CMainFrame::OnBincontrolBinrangedefinition() 
{
	m_pBinSheet->SetData();
	m_pBinSheet->SetTitle("BIN Range Definition");
	m_pBinSheet->DoModal();
	
	
}

void CMainFrame::OnDestroy() 
{
	CFrameWnd::OnDestroy();
	
	if (m_pUserSheet != NULL)
        delete m_pUserSheet;

	if (m_pGroupSheet != NULL)
        delete m_pGroupSheet;

	if (m_pBinSheet != NULL)
        delete m_pBinSheet;

	if ( m_pDialogSheet != NULL )
		delete m_pDialogSheet;

	if ( m_pCardTypeSheet != NULL )
		delete m_pCardTypeSheet;

	if ( m_pConversionSheet != NULL )
		delete m_pConversionSheet;

	if ( m_pMerchantSheet != NULL )
		delete m_pMerchantSheet;

	if ( m_pFleetSheet != NULL )
		delete m_pFleetSheet;

	if ( m_pCalendarSheet != NULL )
		delete m_pCalendarSheet;

	if ( m_pSchSheet != NULL )
		delete m_pSchSheet;

	if ( m_pTerminalSheet != NULL )
		delete m_pTerminalSheet;

	if ( m_pWorkstationSheet != NULL )
		delete m_pWorkstationSheet;

	if ( m_pPanSheet != NULL )
		delete m_pPanSheet;

	if ( m_pCreditSheet != NULL )
		delete m_pCreditSheet;

	if ( m_pDebitSheet != NULL )
		delete m_pDebitSheet;

	if ( m_pCCFSheet != NULL )
		delete m_pCCFSheet;

	if ( m_pNegGroupSheet != NULL )
		delete m_pNegGroupSheet;

	if ( m_pAcquirerSheet != NULL )
		delete m_pAcquirerSheet;
	
	if ( m_pFraudGuardSheet != NULL )
		delete m_pFraudGuardSheet;

	if ( m_pCBSheet != NULL )
		delete m_pCBSheet;

	if ( m_pIssuerSheet != NULL )
		delete m_pIssuerSheet;	

	if ( m_pNegativeCardSheet != NULL )
		delete m_pNegativeCardSheet;	

	if ( m_pCardPlusSheet != NULL )
		delete m_pCardPlusSheet;

	if ( m_pRelatedSheet != NULL )
		delete m_pRelatedSheet;

	if ( m_pPurchaseSheet != NULL )
		delete m_pPurchaseSheet;

	if ( m_pDeferredSheet != NULL )
		delete m_pDeferredSheet;

	if (m_pDBPasswordsSheet != NULL)
        delete m_pDBPasswordsSheet;
	if (m_pDualEnKeyCtrlPropertySheet != NULL)
        delete m_pDualEnKeyCtrlPropertySheet;
	
	if(m_pOverrideMerchantDenialPropertySheet != NULL)
		delete m_pOverrideMerchantDenialPropertySheet;

	if(m_pUsersListDlg != NULL)
		delete m_pUsersListDlg;
	
	if(m_pUserAuditLog != NULL)
		delete m_pUserAuditLog;

	if(m_pMerchantGroupDefinitionSheet != NULL)
		delete m_pMerchantGroupDefinitionSheet;

	if(m_pMerchantPayParamSheet != NULL)
		delete m_pMerchantPayParamSheet;

	if(m_pCResetDcoMonitor != NULL)
		delete m_pCResetDcoMonitor;
}

void CMainFrame::OnServiceManager() 
{
	
     //CSrvcMgr SrvcMgr;
   //SrvcMgr.DoModal();


   CHAR buffer[300], exe_path[256];
   INT ret;

   GetAscendentBinDirectory( exe_path );
   
   //strcat( exe_path, "srvcmon.exe" );
   //Append User Name & PWD as Command Line Params
   sprintf(buffer, "srvcmon.exe %s %s", login.m_LoginName, login.m_Password);
   strcat( exe_path, buffer );
   ret = WinExec( exe_path, SW_SHOW );
   if( ret <= 31 )
   {
	  sprintf( buffer, "ERROR: Unable to spawn %s", exe_path );
	  AfxMessageBox( buffer, MB_ICONSTOP | MB_OK );
   }


}

void CMainFrame::OnDialogManager() 
{
	
  	//m_pDialogSheet->SetTitle("Dialog Manager");
	//m_pDialogSheet->DoModal();
   CHAR buffer[300], exe_path[256];
   INT ret;


   GetAscendentBinDirectory( exe_path );
   //strcat( exe_path, "pindlg.exe" );
   //Append User Name & PWD as Command Line Params
   sprintf(buffer, "pindlg.exe %s %s", login.m_LoginName, login.m_Password);
   strcat( exe_path, buffer );
   ret = WinExec( exe_path, SW_SHOW );
   if( ret <= 31 )
   {
	  sprintf( buffer, "ERROR: Unable to spawn %s", exe_path );
	  AfxMessageBox( buffer, MB_ICONSTOP | MB_OK );
   }




}

void CMainFrame::OnCardtype() 
{
	m_pCardTypeSheet->SetData();
	m_pCardTypeSheet->SetTitle("Card Type Definition");
	m_pCardTypeSheet->DoModal();
	
}

void CMainFrame::OnSettleFile() 
{
	m_pConversionSheet->SetData();
	m_pConversionSheet->SetTitle("Currency Code Definition");
	m_pConversionSheet->DoModal();	
}

void CMainFrame::OnMerchantRisk() 
{
	
}

void CMainFrame::OnFleetcard() 
{
	m_pFleetSheet->SetData();
	m_pFleetSheet->DoModal();
}

void CMainFrame::OnCalenderDefinition() 
{
	m_pCalendarSheet->DoModal();
	
}

void CMainFrame::OnScheduler() 
{
	m_pSchSheet->SetTitle("Scheduler");
	m_pSchSheet->DoModal();
	
}

void CMainFrame::OnTerminaldefinition() 
{
	m_pTerminalSheet->SetTitle("Device Definition");
	m_pTerminalSheet->SetData();
	m_pTerminalSheet->DoModal();
	
}

void CMainFrame::OnWorkstationAccess() 
{
	m_pWorkstationSheet->SetTitle("Workstation Access");
	m_pWorkstationSheet->SetData();
	m_pWorkstationSheet->DoModal();
	
	
}

void CMainFrame::OnPrimaryaccount() 
{
	m_pPanSheet->SetData();
	m_pPanSheet->DoModal();	
}

void CMainFrame::OnCreditcard() 
{
	m_pCreditSheet->SetData();
	m_pCreditSheet->DoModal();
	
}

void CMainFrame::OnDebitcard() 
{
	m_pDebitSheet->SetData();
	m_pDebitSheet->DoModal();
	
}

void CMainFrame::OnBlockingPosEntryModes()
{
	m_pBlockingPosEntryModePropertySheet->SetData();
	m_pBlockingPosEntryModePropertySheet->DoModal();
}

void CMainFrame::OnCardrecord() 
{
	m_pCCFSheet->SetData();
	m_pCCFSheet->DoModal();
	
}

BOOL CMainFrame::OnHelpInfo(HELPINFO* pHelpInfo) 
{
      //if there is pop-up window (dialog) and it is derived from
      //COleDialog (base class for OLE common dialogs) just return.
      CWnd *pWnd = FromHandle(::GetLastActivePopup(m_hWnd));
      if (pWnd->IsKindOf(RUNTIME_CLASS( CPropertyPage )))  
		  return TRUE;
      return TRUE;
	
	
}

void CMainFrame::OnCardStatus() 
{
	CCardStatus m_CardStatus;
	m_CardStatus.DoModal();
	
}


void CMainFrame::OnMonitor() 
{
	CHAR buffer[512] = {0}, exe_path[512] = {0};
   INT ret;
   CHAR filename[50] = {0};
   CHAR tmpstr[50] = {0};
   CHAR hostname[50] = {0};
   CHAR server_selection_flag[10] = {0};
   INT server_selection_flag_int = 0;
   CHAR port_str[10] = {0};
   SHORT port_num;

    GetAscendentBinDirectory( exe_path );

	
	 GetAscendentConfigDirectory(tmpstr);
	 sprintf(filename, "%stf.ini", tmpstr);
	 
	 WritePrivateProfileString(
		  "SERVER_SELECT",	/* pointer to section name*/
		  "SERVER_NAME",	/* pointer to key name*/
		  "1" ,	/* pointer to string to add*/
		  filename			/* pointer to initialization filename*/
	 );

	 GetPrivateProfileString(
        "SERVER_SELECT", 	               // points to section name 
        "SERVER_NAME",	       // points to key name 
        "1",	                   // points to default string 
        server_selection_flag,                   // points to destination buffer 
        sizeof(server_selection_flag) - 1,       // size of destination buffer 
        filename ); 

	 server_selection_flag_int = atoi(server_selection_flag);

 if(server_selection_flag_int == 1)
		  {
				GetPrivateProfileString(
				"APPLNK", 	               // points to section name 
				"APPLNK_TCP_PORT",	       // points to key name 
				"1450",	                   // points to default string 
				port_str,                   // points to destination buffer 
				sizeof(port_str) - 1,       // size of destination buffer 
				filename );                 // points to initialization filename 

				if( !GetHostName_1( hostname ) )
				{
					AfxMessageBox( "Primary Machine ID not found in Registry; Registry may not \
						be initialized for Ascendent. Defaulting to local host", MB_ICONSTOP | MB_OK );
					strcpy( hostname, "localhost" );
				}
		  }
		  else if(server_selection_flag_int == 2)
		  {
				GetPrivateProfileString(
				"APPLNK2", 	               // points to section name 
				"APPLNK_TCP_PORT",	       // points to key name 
				"1450",	                   // points to default string 
				port_str,                   // points to destination buffer 
				sizeof(port_str) - 1,       // size of destination buffer 
				filename );                 // points to initialization filename 

				
			if( !GetHostName_2( hostname ) )
			{
				AfxMessageBox( "Primary Machine ID not found in Registry; Registry may not \
					be initialized for Ascendent. Defaulting to local host", MB_ICONSTOP | MB_OK );
				strcpy( hostname, "localhost" );
			}
		  }
		  else if(server_selection_flag_int == 3)
		  {
				GetPrivateProfileString(
				"APPLNK3", 	               // points to section name 
				"APPLNK_TCP_PORT",	       // points to key name 
				"1450",	                   // points to default string 
				port_str,                   // points to destination buffer 
				sizeof(port_str) - 1,       // size of destination buffer 
				filename );                 // points to initialization filename 

				
			if( !GetHostName_3( hostname ) )
			{
				AfxMessageBox( "Primary Machine ID not found in Registry; Registry may not \
					be initialized for Ascendent. Defaulting to local host", MB_ICONSTOP | MB_OK );
				strcpy( hostname, "localhost" );
			}
		  }
		  else if(server_selection_flag_int == 4)
		  {
				GetPrivateProfileString(
				"APPLNK4", 	               // points to section name 
				"APPLNK_TCP_PORT",	       // points to key name 
				"1450",	                   // points to default string 
				port_str,                   // points to destination buffer 
				sizeof(port_str) - 1,       // size of destination buffer 
				filename );                 // points to initialization filename 

			if( !GetHostName_4( hostname ) )
			{
				AfxMessageBox( "Primary Machine ID not found in Registry; Registry may not \
					be initialized for Ascendent. Defaulting to local host", MB_ICONSTOP | MB_OK );
				strcpy( hostname, "localhost" );
			}
		  }
			port_num = atol( port_str );



	
		sprintf( buffer, "HostName : %s and Applink Port : %d", hostname, port_num);
	AfxMessageBox(buffer,MB_ICONINFORMATION | MB_OK);
   //strcat( exe_path, "monitor.exe" );
  // sprintf(buffer, "monitor.exe %s %s", login.m_LoginName, login.m_Password);
   sprintf(buffer, "monitor.exe %s|%s%s|%s", login.m_LoginName, ucf01.user_info_1, ucf01.user_info_2, login.m_Password);
   strcat (exe_path, buffer);
   ret = WinExec( exe_path, SW_SHOW );
   if( ret <= 31 )
   {
	  sprintf( buffer, "ERROR: Unable to spawn %s", exe_path );
	  AfxMessageBox( buffer, MB_ICONSTOP | MB_OK );
   }	
}

void CMainFrame::OnMonitor_Server2() 
{
	CHAR buffer[512] = {0}, exe_path[512] = {0};
   INT ret;
     CHAR filename[50] = {0};
	  CHAR tmpstr[50] = {0};
	  CHAR hostname[50] = {0};
	  CHAR server_selection_flag[10] = {0};
	  INT server_selection_flag_int = 0;
	  CHAR port_str[10] = {0};
	  SHORT port_num;

    GetAscendentBinDirectory( exe_path );

	 GetAscendentConfigDirectory(tmpstr);
	 sprintf(filename, "%stf.ini", tmpstr);
	 
	 WritePrivateProfileString(
		  "SERVER_SELECT",	/* pointer to section name*/
		  "SERVER_NAME",	/* pointer to key name*/
		  "2" ,	/* pointer to string to add*/
		  filename			/* pointer to initialization filename*/
	 );
	 
      GetPrivateProfileString(
        "SERVER_SELECT", 	               // points to section name 
        "SERVER_NAME",	       // points to key name 
        "2",	                   // points to default string 
        server_selection_flag,                   // points to destination buffer 
        sizeof(server_selection_flag) - 1,       // size of destination buffer 
        filename ); 


	  	 server_selection_flag_int = atoi(server_selection_flag);

	 if(server_selection_flag_int == 1)
			  {
					GetPrivateProfileString(
					"APPLNK", 	               // points to section name 
					"APPLNK_TCP_PORT",	       // points to key name 
					"1450",	                   // points to default string 
					port_str,                   // points to destination buffer 
					sizeof(port_str) - 1,       // size of destination buffer 
					filename );                 // points to initialization filename 

					if( !GetHostName_1( hostname ) )
					{
						AfxMessageBox( "Primary Machine ID not found in Registry; Registry may not \
							be initialized for Ascendent. Defaulting to local host", MB_ICONSTOP | MB_OK );
						strcpy( hostname, "localhost" );
					}
			  }
			  else if(server_selection_flag_int == 2)
			  {
					GetPrivateProfileString(
					"APPLNK2", 	               // points to section name 
					"APPLNK_TCP_PORT",	       // points to key name 
					"1450",	                   // points to default string 
					port_str,                   // points to destination buffer 
					sizeof(port_str) - 1,       // size of destination buffer 
					filename );                 // points to initialization filename 

				
				if( !GetHostName_2( hostname ) )
				{
					AfxMessageBox( "Primary Machine ID not found in Registry; Registry may not \
						be initialized for Ascendent. Defaulting to local host", MB_ICONSTOP | MB_OK );
					strcpy( hostname, "localhost" );
				}
			  }
			  else if(server_selection_flag_int == 3)
			  {
					GetPrivateProfileString(
					"APPLNK3", 	               // points to section name 
					"APPLNK_TCP_PORT",	       // points to key name 
					"1450",	                   // points to default string 
					port_str,                   // points to destination buffer 
					sizeof(port_str) - 1,       // size of destination buffer 
					filename );                 // points to initialization filename 

				
				if( !GetHostName_3( hostname ) )
				{
					AfxMessageBox( "Primary Machine ID not found in Registry; Registry may not \
						be initialized for Ascendent. Defaulting to local host", MB_ICONSTOP | MB_OK );
					strcpy( hostname, "localhost" );
				}
			  }
		  else if(server_selection_flag_int == 4)
		  {
				GetPrivateProfileString(
				"APPLNK4", 	               // points to section name 
				"APPLNK_TCP_PORT",	       // points to key name 
				"1450",	                   // points to default string 
				port_str,                   // points to destination buffer 
				sizeof(port_str) - 1,       // size of destination buffer 
				filename );                 // points to initialization filename 

			if( !GetHostName_4( hostname ) )
			{
				AfxMessageBox( "Primary Machine ID not found in Registry; Registry may not \
					be initialized for Ascendent. Defaulting to local host", MB_ICONSTOP | MB_OK );
				strcpy( hostname, "localhost" );
			}
		  }
	
		  port_num = atol( port_str );

		  

	sprintf( buffer, "HostName : %s and Applink Port : %d", hostname, port_num);
	AfxMessageBox(buffer,MB_ICONINFORMATION | MB_OK);
   //strcat( exe_path, "monitor.exe" );
  // sprintf(buffer, "monitor.exe %s %s", login.m_LoginName, login.m_Password);
	memset(buffer,0x00,sizeof(buffer));
   sprintf(buffer, "monitor.exe %s|%s%s|%s", login.m_LoginName, ucf01.user_info_1, ucf01.user_info_2, login.m_Password);
   strcat (exe_path, buffer);
   ret = WinExec( exe_path, SW_SHOW );
   if( ret <= 31 )
   {
	  sprintf( buffer, "ERROR: Unable to spawn %s", exe_path );
	  AfxMessageBox( buffer, MB_ICONSTOP | MB_OK );
   }	
}


void CMainFrame::OnMonitor_Server3() 
{
	CHAR buffer[512] = {0}, exe_path[512] = {0};
   INT ret;
     CHAR filename[50] = {0};
	  CHAR tmpstr[50] = {0};
	  CHAR hostname[50] = {0};
	  CHAR server_selection_flag[10] = {0};
	  INT server_selection_flag_int = 0;
	  CHAR port_str[10] = {0};
	  SHORT port_num;

    GetAscendentBinDirectory( exe_path );

	 GetAscendentConfigDirectory(tmpstr);
	 sprintf(filename, "%stf.ini", tmpstr);
	 
	 WritePrivateProfileString(
		  "SERVER_SELECT",	/* pointer to section name*/
		  "SERVER_NAME",	/* pointer to key name*/
		  "3" ,	/* pointer to string to add*/
		  filename			/* pointer to initialization filename*/
	 );
	 
      GetPrivateProfileString(
        "SERVER_SELECT", 	               // points to section name 
        "SERVER_NAME",	       // points to key name 
        "3",	                   // points to default string 
        server_selection_flag,                   // points to destination buffer 
        sizeof(server_selection_flag) - 1,       // size of destination buffer 
        filename ); 

	
	  	 server_selection_flag_int = atoi(server_selection_flag);

		if(server_selection_flag_int == 1)
		  {
				GetPrivateProfileString(
				"APPLNK", 	               // points to section name 
				"APPLNK_TCP_PORT",	       // points to key name 
				"1450",	                   // points to default string 
				port_str,                   // points to destination buffer 
				sizeof(port_str) - 1,       // size of destination buffer 
				filename );                 // points to initialization filename 

				if( !GetHostName_1( hostname ) )
				{
					AfxMessageBox( "Primary Machine ID not found in Registry; Registry may not \
						be initialized for Ascendent. Defaulting to local host", MB_ICONSTOP | MB_OK );
					strcpy( hostname, "localhost" );
				}
		  }
		  else if(server_selection_flag_int == 2)
		  {
				GetPrivateProfileString(
				"APPLNK2", 	               // points to section name 
				"APPLNK_TCP_PORT",	       // points to key name 
				"1450",	                   // points to default string 
				port_str,                   // points to destination buffer 
				sizeof(port_str) - 1,       // size of destination buffer 
				filename );                 // points to initialization filename 

				
			if( !GetHostName_2( hostname ) )
			{
				AfxMessageBox( "Primary Machine ID not found in Registry; Registry may not \
					be initialized for Ascendent. Defaulting to local host", MB_ICONSTOP | MB_OK );
				strcpy( hostname, "localhost" );
			}
		  }
		  else if(server_selection_flag_int == 3)
		  {
				GetPrivateProfileString(
				"APPLNK3", 	               // points to section name 
				"APPLNK_TCP_PORT",	       // points to key name 
				"1450",	                   // points to default string 
				port_str,                   // points to destination buffer 
				sizeof(port_str) - 1,       // size of destination buffer 
				filename );                 // points to initialization filename 

				
			if( !GetHostName_3( hostname ) )
			{
				AfxMessageBox( "Primary Machine ID not found in Registry; Registry may not \
					be initialized for Ascendent. Defaulting to local host", MB_ICONSTOP | MB_OK );
				strcpy( hostname, "localhost" );
			}
		  }
		  else if(server_selection_flag_int == 4)
		  {
				GetPrivateProfileString(
				"APPLNK4", 	               // points to section name 
				"APPLNK_TCP_PORT",	       // points to key name 
				"1450",	                   // points to default string 
				port_str,                   // points to destination buffer 
				sizeof(port_str) - 1,       // size of destination buffer 
				filename );                 // points to initialization filename 

				
			if( !GetHostName_4( hostname ) )
			{
			AfxMessageBox( "Primary Machine ID not found in Registry; Registry may not \
				be initialized for Ascendent. Defaulting to local host", MB_ICONSTOP | MB_OK );
			strcpy( hostname, "localhost" );
			}

		  }
			port_num = atol( port_str );



	sprintf( buffer, "HostName : %s and Applink Port : %d", hostname, port_num);
	AfxMessageBox(buffer,MB_ICONINFORMATION | MB_OK);
   //strcat( exe_path, "monitor.exe" );
  // sprintf(buffer, "monitor.exe %s %s", login.m_LoginName, login.m_Password);
	memset(buffer,0x00,sizeof(buffer));
   sprintf(buffer, "monitor.exe %s|%s%s|%s", login.m_LoginName, ucf01.user_info_1, ucf01.user_info_2, login.m_Password);
   strcat (exe_path, buffer);
   ret = WinExec( exe_path, SW_SHOW );
   if( ret <= 31 )
   {
	  sprintf( buffer, "ERROR: Unable to spawn %s", exe_path );
	  AfxMessageBox( buffer, MB_ICONSTOP | MB_OK );
   }	
}

void CMainFrame::OnMonitor_Server4() 
{
	CHAR buffer[512] = {0}, exe_path[512] = {0};
   INT ret;
     CHAR filename[50] = {0};
	  CHAR tmpstr[50] = {0};
	  CHAR hostname[50] = {0};
	  CHAR server_selection_flag[10] = {0};
	  INT server_selection_flag_int = 0;
	  CHAR port_str[10] = {0};
	  SHORT port_num;

    GetAscendentBinDirectory( exe_path );

	 GetAscendentConfigDirectory(tmpstr);
	 sprintf(filename, "%stf.ini", tmpstr);
	 
	 WritePrivateProfileString(
		  "SERVER_SELECT",	/* pointer to section name*/
		  "SERVER_NAME",	/* pointer to key name*/
		  "4" ,	/* pointer to string to add*/
		  filename			/* pointer to initialization filename*/
	 );
	 
      GetPrivateProfileString(
        "SERVER_SELECT", 	               // points to section name 
        "SERVER_NAME",	       // points to key name 
        "4",	                   // points to default string 
        server_selection_flag,                   // points to destination buffer 
        sizeof(server_selection_flag) - 1,       // size of destination buffer 
        filename ); 

	  server_selection_flag_int = atoi(server_selection_flag);

	  	  if(server_selection_flag_int == 1)
		  {
				GetPrivateProfileString(
				"APPLNK", 	               // points to section name 
				"APPLNK_TCP_PORT",	       // points to key name 
				"1450",	                   // points to default string 
				port_str,                   // points to destination buffer 
				sizeof(port_str) - 1,       // size of destination buffer 
				filename );                 // points to initialization filename 

				if( !GetHostName_1( hostname ) )
				{
					AfxMessageBox( "Primary Machine ID not found in Registry; Registry may not \
						be initialized for Ascendent. Defaulting to local host", MB_ICONSTOP | MB_OK );
					strcpy( hostname, "localhost" );
				}
		  }
		  else if(server_selection_flag_int == 2)
		  {
				GetPrivateProfileString(
				"APPLNK2", 	               // points to section name 
				"APPLNK_TCP_PORT",	       // points to key name 
				"1450",	                   // points to default string 
				port_str,                   // points to destination buffer 
				sizeof(port_str) - 1,       // size of destination buffer 
				filename );                 // points to initialization filename 

				
			if( !GetHostName_2( hostname ) )
			{
				AfxMessageBox( "Primary Machine ID not found in Registry; Registry may not \
					be initialized for Ascendent. Defaulting to local host", MB_ICONSTOP | MB_OK );
				strcpy( hostname, "localhost" );
			}
		  }
		  else if(server_selection_flag_int == 3)
		  {
				GetPrivateProfileString(
				"APPLNK3", 	               // points to section name 
				"APPLNK_TCP_PORT",	       // points to key name 
				"1450",	                   // points to default string 
				port_str,                   // points to destination buffer 
				sizeof(port_str) - 1,       // size of destination buffer 
				filename );                 // points to initialization filename 

				
			if( !GetHostName_3( hostname ) )
			{
				AfxMessageBox( "Primary Machine ID not found in Registry; Registry may not \
					be initialized for Ascendent. Defaulting to local host", MB_ICONSTOP | MB_OK );
				strcpy( hostname, "localhost" );
			}
		  }
		  else if(server_selection_flag_int == 4)
		  {
				GetPrivateProfileString(
				"APPLNK4", 	               // points to section name 
				"APPLNK_TCP_PORT",	       // points to key name 
				"1450",	                   // points to default string 
				port_str,                   // points to destination buffer 
				sizeof(port_str) - 1,       // size of destination buffer 
				filename );                 // points to initialization filename 

			if( !GetHostName_4( hostname ) )
			{
				AfxMessageBox( "Primary Machine ID not found in Registry; Registry may not \
					be initialized for Ascendent. Defaulting to local host", MB_ICONSTOP | MB_OK );
				strcpy( hostname, "localhost" );
			}
		  }


			port_num = atol( port_str );


	
	sprintf( buffer, "HostName : %s and Applink Port : %d", hostname, port_num);
	AfxMessageBox(buffer,MB_ICONINFORMATION | MB_OK);
   //strcat( exe_path, "monitor.exe" );
  // sprintf(buffer, "monitor.exe %s %s", login.m_LoginName, login.m_Password);
	memset(buffer,0x00,sizeof(buffer));
   sprintf(buffer, "monitor.exe %s|%s%s|%s", login.m_LoginName, ucf01.user_info_1, ucf01.user_info_2, login.m_Password);
   strcat (exe_path, buffer);
   ret = WinExec( exe_path, SW_SHOW );
   if( ret <= 31 )
   {
	  sprintf( buffer, "ERROR: Unable to spawn %s", exe_path );
	  AfxMessageBox( buffer, MB_ICONSTOP | MB_OK );
   }	
}

void CMainFrame::OnDcoMonitor() 
{
	CHAR buffer[512] = {0}, exe_path[512] = {0};
   INT ret;

    GetAscendentBinDirectory( exe_path );

	sprintf( buffer, "HostName1 : %s and Applink Port1 : %d\nHostName2 : %s and Applink Port2 : %d\n"
					 "HostName3 : %s and Applink Port3 : %d\nHostName4 : %s and Applink Port4 : %d"
					 "\n\nARE YOU SURE TO CONTINUE?", 
					  hostname1, port_num1, hostname2, port_num2, hostname3, port_num3, hostname4, port_num4);
	//AfxMessageBox(buffer,MB_ICONINFORMATION | MB_OK);
	int result = AfxMessageBox(buffer, MB_YESNO);

	if(result == IDYES)
	{
		memset(buffer,0x00,sizeof(buffer));
		sprintf(buffer, "monitordco.exe %s|%s%s|%s", login.m_LoginName, ucf01.user_info_1, ucf01.user_info_2, login.m_Password);
		strcat (exe_path, buffer);
		ret = WinExec( exe_path, SW_SHOW );
		if( ret <= 31 )
		{
			sprintf( buffer, "ERROR: Unable to spawn %s", exe_path );
			AfxMessageBox( buffer, MB_ICONSTOP | MB_OK );
		}
	}
}


void CMainFrame::OnNetworkacquirer() 
{
	m_pAcquirerSheet->SetData();
	m_pAcquirerSheet->DoModal();
	
}

void CMainFrame::OnNetworkissuer() 
{
	m_pIssuerSheet->SetData();
	m_pIssuerSheet->DoModal();
	
}




/*void CMainFrame::OnDarPreview() 
{
	// TODO: Add your command handler code here
	CHAR ErrMsg[256];

	if( !DoDownloadActivityRprt( DEST_WINDOW, ErrMsg ) )
	{
		AfxMessageBox( ErrMsg, MB_ICONSTOP | MB_OK );
	}
	
	
}

void CMainFrame::OnDarPrint() 
{
	// TODO: Add your command handler code here
	CHAR ErrMsg[256];

	if( !DoDownloadActivityRprt( DEST_PRINTER, ErrMsg ) )
	{
		AfxMessageBox( ErrMsg, MB_ICONSTOP | MB_OK );
	}
	
	
	
}

void CMainFrame::OnDtrPreview() 
{
	CHAR ErrMsg[256];

	if( !DoDownloadTerminalRprt( DEST_WINDOW, ErrMsg ) )
	{
		AfxMessageBox( ErrMsg, MB_ICONSTOP | MB_OK );
	}
	
}

void CMainFrame::OnDtrPrint() 
{
	CHAR ErrMsg[256];

	if( !DoDownloadTerminalRprt( DEST_PRINTER, ErrMsg ) )
	{
		AfxMessageBox( ErrMsg, MB_ICONSTOP | MB_OK );
	}
	
}*/


/*void CMainFrame::OnCardplusinformation() 
{
	m_pCardPlusSheet->DoModal();
	
}*/

void CMainFrame::OnRelatedAccount() 
{
	m_pRelatedSheet->SetData();
	m_pRelatedSheet->DoModal();
	
}

void CMainFrame::OnCurrencyCodeList() 
{
	m_pCurrencyCodeSheet->SetData();
	m_pCurrencyCodeSheet->DoModal();
	
}

void CMainFrame::OnPurchaseCard() 
{
	m_pPurchaseSheet->SetData();
	m_pPurchaseSheet->DoModal();
	
}

void CMainFrame::OnMcc103() 
{
	CMCC103 mcc103Dlg;
	mcc103Dlg.DoModal();
	
}

void CMainFrame::OnVoice() 
{

   CHAR buffer[300], exe_path[256];
   INT ret;
   
   GetAscendentBinDirectory( exe_path );
   //sprintf ( param, "VoiceAuthorization.exe %s %d", ucf01.primary_key.name,idletime);
   //strcat( exe_path, param );
   //Append User Name & PWD as Command Line Params
   sprintf(buffer, "VoiceAuthorization.exe %s %s", login.m_LoginName, login.m_Password);
   strcat( exe_path, buffer );
   ret = WinExec( exe_path, SW_SHOW );
   if( ret <= 31 )
   {
	  sprintf( buffer, "ERROR: Unable to spawn %s", exe_path );
      AfxMessageBox( buffer, MB_ICONSTOP | MB_OK );
   }
   
	
}

void CMainFrame::OnMcc102() 
{
	CMCC102 mcc102Dlg;
	mcc102Dlg.DoModal();
	
}


void CMainFrame::OnCashBonus() 
{
	CCashBonus cashDlg;
	cashDlg.DoModal();	
}

void CMainFrame::OnCB_Definition() 
{
	m_pCBSheet->SetData();
	m_pCBSheet->DoModal();
}

void CMainFrame::OnDeferred() 
{
	m_pDeferredSheet->SetData();
	m_pDeferredSheet->DoModal();
	
}

void CMainFrame::OnRepact() 
{
   CHAR buffer[300], exe_path[256];
   INT ret;

   // for now, just spawn the existing GUI's.
   GetAscendentBinDirectory( exe_path );
   strcat( exe_path, "ActRepGUI.exe equitable equitable" );
   ret = WinExec( exe_path, SW_SHOW );
   if( ret <= 31 )
   {
	  sprintf( buffer, "ERROR: Unable to spawn %s", exe_path );
      AfxMessageBox( buffer, MB_ICONSTOP | MB_OK );
   }
	
}

void CMainFrame::OnRepset() 
{
   CHAR buffer[300], exe_path[256];
   INT ret;

   // for now, just spawn the existing GUI's.
   GetAscendentBinDirectory( exe_path );
   strcat( exe_path, "SetRepGUI.exe equitable equitable" );
   ret = WinExec( exe_path, SW_SHOW );
   if( ret <= 31 )
   {
	  sprintf( buffer, "ERROR: Unable to spawn %s", exe_path );
      AfxMessageBox( buffer, MB_ICONSTOP | MB_OK );
   }
	
}

void CMainFrame::OnRepstat() 
{
   CHAR buffer[300], exe_path[256];
   INT ret;

   // for now, just spawn the existing GUI's.
   GetAscendentBinDirectory( exe_path );
   strcat( exe_path, "StatRepGUI.exe equitable equitable" );
   ret = WinExec( exe_path, SW_SHOW );
   if( ret <= 31 )
   {
	  sprintf( buffer, "ERROR: Unable to spawn %s", exe_path );
      AfxMessageBox( buffer, MB_ICONSTOP | MB_OK );
   }
	
}

void CMainFrame::OnTranLookup() 
{
   CHAR buffer[300], exe_path[256];
   INT ret;

   GetAscendentBinDirectory( exe_path );
   //sprintf ( param, "TranSearch.exe %s %d", ucf01.primary_key.name,idletime );
   //strcat( exe_path, param );
   //Append User Name & PWD as Command Line Params
   sprintf(buffer, "TranSearch.exe %s %s", login.m_LoginName, login.m_Password);
   strcat( exe_path, buffer );
   ret = WinExec( exe_path, SW_SHOW );
   if( ret <= 31 )
   {
	  sprintf( buffer, "ERROR: Unable to spawn %s", exe_path );
      AfxMessageBox( buffer, MB_ICONSTOP | MB_OK );
   }  
	
}  


void CMainFrame::OnTranPOSLookup() 
{
   CHAR buffer[300], exe_path[256];
   INT ret;

   GetAscendentBinDirectory( exe_path );
   //sprintf ( param, "TranSearch.exe %s %d", ucf01.primary_key.name,idletime );
   //strcat( exe_path, param );
   //Append User Name & PWD as Command Line Params
   sprintf(buffer, "TranPosSearch.exe %s %s", login.m_LoginName, login.m_Password);
   strcat( exe_path, buffer );
   ret = WinExec( exe_path, SW_SHOW );
   if( ret <= 31 )
   {
	  sprintf( buffer, "ERROR: Unable to spawn %s", exe_path );
      AfxMessageBox( buffer, MB_ICONSTOP | MB_OK );
   }  
	
} 

void CMainFrame::OnArchiveLookup() 
{
   CHAR buffer[300], exe_path[256];
   INT ret;

   GetAscendentBinDirectory( exe_path );
   sprintf(buffer, "ArchiveLookup.exe %s %s", login.m_LoginName, login.m_Password);
   strcat( exe_path, buffer );
   ret = WinExec( exe_path, SW_SHOW );
   if( ret <= 31 )
   {
	  sprintf( buffer, "ERROR: Unable to spawn %s", exe_path );
      AfxMessageBox( buffer, MB_ICONSTOP | MB_OK );
   }  
	
}  

void CMainFrame::OnUserStatusControl()
{
	CHAR buffer[300], exe_path[256];
	INT ret;

   GetAscendentBinDirectory( exe_path );
   sprintf(buffer, "UserStatusControl.exe %s %s", login.m_LoginName, login.m_Password);
   strcat( exe_path, buffer );
   ret = WinExec( exe_path, SW_SHOW );
   if( ret <= 31 )
   {
	  sprintf( buffer, "ERROR: Unable to spawn %s", exe_path );
      AfxMessageBox( buffer, MB_ICONSTOP | MB_OK );
   }  
}

void CMainFrame::OnFraudSystemControl()
{
	CHAR buffer[300], exe_path[256];
	INT ret;

   GetAscendentBinDirectory( exe_path );
   sprintf(buffer, "FraudSystemControl.exe %s %s", login.m_LoginName, login.m_Password);
   strcat( exe_path, buffer );
   ret = WinExec( exe_path, SW_SHOW );
   if( ret <= 31 )
   {
	  sprintf( buffer, "ERROR: Unable to spawn %s", exe_path );
      AfxMessageBox( buffer, MB_ICONSTOP | MB_OK );
   }  
}

/*void CMainFrame::OnRecurringPayment() 
{
   CHAR buffer[300], exe_path[256], param[30];
   INT ret;

   // for now, just spawn the existing GUI's.
   GetAscendentBinDirectory( exe_path );

   sprintf ( param, "RPSGui.exe %s", ucf01.primary_key.name );
 //  strcpy ( param, "RPSGui.exe" );
   strcat( exe_path, param );
   ret = WinExec( exe_path, SW_SHOW );
   if( ret <= 31 )
   {
	  sprintf( buffer, "ERROR: Unable to spawn %s", exe_path );
      AfxMessageBox( buffer, MB_ICONSTOP | MB_OK );
   }	
}*/

void CMainFrame::OnCorpCardRep() 
{
  CHAR buffer[300], exe_path[256];
   INT ret;

   // for now, just spawn the existing GUI's.
   GetAscendentBinDirectory( exe_path );
   strcat( exe_path, "ComfortCardRep.exe equitable equitable" );
   ret = WinExec( exe_path, SW_SHOW );
   if( ret <= 31 )
   {
	  sprintf( buffer, "ERROR: Unable to spawn %s", exe_path );
      AfxMessageBox( buffer, MB_ICONSTOP | MB_OK );
   }	
}

void CMainFrame::OnTranRep() 
{
  CHAR buffer[300], exe_path[256];
   INT ret;

   // for now, just spawn the existing GUI's.
   GetAscendentBinDirectory( exe_path );
   strcat( exe_path, "ComfortTranRep.exe equitable equitable" );
   ret = WinExec( exe_path, SW_SHOW );
   if( ret <= 31 )
   {
	  sprintf( buffer, "ERROR: Unable to spawn %s", exe_path );
      AfxMessageBox( buffer, MB_ICONSTOP | MB_OK );
   }	
}

void CMainFrame::OnSummaryRep() 
{
  CHAR buffer[300], exe_path[256];
   INT ret;

   // for now, just spawn the existing GUI's.
   GetAscendentBinDirectory( exe_path );
   strcat( exe_path, "ComfortSumRep.exe comfort comfort" );
   ret = WinExec( exe_path, SW_SHOW );
   if( ret <= 31 )
   {
	  sprintf( buffer, "ERROR: Unable to spawn %s", exe_path );
      AfxMessageBox( buffer, MB_ICONSTOP | MB_OK );
   }	
}

void CMainFrame::OnJcbupdates() 
{
	CJCBUPDATES jcbupdatesDlg;
	jcbupdatesDlg.DoModal();

	
}

void CMainFrame::OnAmexupdates()
{
	CAMEXUPDATES amexupdatesDlg;
	amexupdatesDlg.DoModal();
}

void CMainFrame::OnFraudguarddefinition() 
{
	m_pFraudGuardSheet->SetData();
	m_pFraudGuardSheet->DoModal();

	
}

void CMainFrame::OnStipManager() 
{
	CHAR buffer[300], exe_path[256];
   INT ret;


   GetAscendentBinDirectory( exe_path );
  // sprintf ( param, "stpmgr.exe %s %d", ucf01.primary_key.name,idletime);
  // strcpy ( param, "stpmgr.exe" );
   //strcat( exe_path, param );

   //Append User Name & PWD as Command Line Params
   sprintf(buffer, "stipmgr.exe %s %s", login.m_LoginName, login.m_Password);
   strcat( exe_path, buffer );

   ret = WinExec( exe_path, SW_SHOW );
   if( ret <= 31 )
   {
	  sprintf( buffer, "ERROR: Unable to spawn %s", exe_path );
      AfxMessageBox( buffer, MB_ICONSTOP | MB_OK );
   }
   
}
void CMainFrame::OnUsersListMenu()
{
	m_pUsersListDlg->DoModal();
}
void CMainFrame::OnUserAuditLogMenu()
{
	m_pUserAuditLog->DoModal();
}

void CMainFrame::OnDbpasswdsMenu() 
{	
	m_pDBPasswordsSheet->SetTitle("Change Database Connectivity Passwords");
	m_pDBPasswordsSheet->DoModal();	

}


void CMainFrame::OnArchDbpasswdsMenu() 
{
	m_pArchiveDBPasswordsDlg->DoModal();
}


void CMainFrame::OnMraDbpasswdsMenu() 
{
	m_pMraDBPasswordsDlg->DoModal();	
}

LRESULT CMainFrame::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	// TODO: Add your specialized code here and/or call the base class
	switch(message)
	{
		case WM_TIMER:
		{
			if (wParam == IDT_TIMER)
			{
			cntr++;
			
			shm_time = shrmem.GetLastUpdateTime();

			if(shm_time > c_time)
			{
				c_time = shm_time;
				ResetCntr();
			}
			if ( cntr == idletime/2 )
			{
				shrmem.SetIdleStatus(0);
			}
			//Check whether Max Idle Time has elapsed
			if(cntr >= idletime)
			{
				ret = LockScreen();

				if(ret == -1 || ret == -2)
				{	
					if(IsWindow(this->m_pNegGroupSheet->m_hWnd))
						this->m_pNegGroupSheet->EndDialog(0);

					if(IsWindow(this->m_pUserSheet->m_hWnd))
						this->m_pUserSheet->EndDialog(0);

					if(IsWindow(this->m_pGroupSheet->m_hWnd))
						this->m_pGroupSheet->EndDialog(0);

					if(IsWindow(this->m_pBinSheet->m_hWnd))
						this->m_pBinSheet->EndDialog(0);

					if(IsWindow(this->m_pDialogSheet->m_hWnd))
						this->m_pDialogSheet->EndDialog(0);

					if(IsWindow(this->m_pCardTypeSheet->m_hWnd))
						this->m_pCardTypeSheet->EndDialog(0);

					if(IsWindow(this->m_pWorkstationSheet->m_hWnd))
						this->m_pWorkstationSheet->EndDialog(0);

					if(IsWindow(this->m_pMerchantSheet->m_hWnd))
						this->m_pMerchantSheet->EndDialog(0);

					if(IsWindow(this->m_pConversionSheet->m_hWnd))
						this->m_pConversionSheet->EndDialog(0);

					if(IsWindow(this->m_pFleetSheet->m_hWnd))
						this->m_pFleetSheet->EndDialog(0);

					if(IsWindow(this->m_pCalendarSheet->m_hWnd))
						this->m_pCalendarSheet->EndDialog(0);

					if(IsWindow(this->m_pSchSheet->m_hWnd))
						this->m_pSchSheet->EndDialog(0);

					if(IsWindow(this->m_pTerminalSheet->m_hWnd))
						this->m_pTerminalSheet->EndDialog(0);

					if(IsWindow(this->m_pPanSheet->m_hWnd))
						this->m_pPanSheet->EndDialog(0);

					if(IsWindow(this->m_pCreditSheet->m_hWnd))
						this->m_pCreditSheet->EndDialog(0);

					if(IsWindow(this->m_pDebitSheet->m_hWnd))
						this->m_pDebitSheet->EndDialog(0);

					if(IsWindow(this->m_pCCFSheet->m_hWnd))
						this->m_pCCFSheet->EndDialog(0);

					if(IsWindow(this->m_pAcquirerSheet->m_hWnd))
						this->m_pAcquirerSheet->EndDialog(0);

					if(IsWindow(this->m_pIssuerSheet->m_hWnd))
						this->m_pIssuerSheet->EndDialog(0);

					if(IsWindow(this->m_pNegativeCardSheet->m_hWnd))
						this->m_pNegativeCardSheet->EndDialog(0);

					if(IsWindow(this->m_pCardPlusSheet->m_hWnd))
						this->m_pCardPlusSheet->EndDialog(0);

					if(IsWindow(this->m_pRelatedSheet->m_hWnd))
						this->m_pRelatedSheet->EndDialog(0);

					if(IsWindow(this->m_pPurchaseSheet->m_hWnd))
						this->m_pPurchaseSheet->EndDialog(0);

					if(IsWindow(this->m_pDeferredSheet->m_hWnd))
						this->m_pDeferredSheet->EndDialog(0);

					if(IsWindow(this->m_pFraudGuardSheet->m_hWnd))
						this->m_pFraudGuardSheet->EndDialog(0);

					if(IsWindow(this->m_pCBSheet->m_hWnd))
						this->m_pCBSheet->EndDialog(0);

					if(IsWindow(this->m_pDBPasswordsSheet->m_hWnd))
						this->m_pDBPasswordsSheet->EndDialog(0);

					if(IsWindow(this->m_pDualEnKeyCtrlPropertySheet->m_hWnd))
						this->m_pDualEnKeyCtrlPropertySheet->EndDialog(0);

					if(IsWindow(this->m_pOverrideMerchantDenialPropertySheet->m_hWnd))
						this->m_pOverrideMerchantDenialPropertySheet->EndDialog(0);

					if(IsWindow(this->m_pUsersListDlg->m_hWnd))
						this->m_pUsersListDlg->EndDialog(0);

					if(IsWindow(this->m_pUserAuditLog->m_hWnd))
						this->m_pUserAuditLog->EndDialog(0);

					if(IsWindow(this->m_pMerchantGroupDefinitionSheet->m_hWnd))
						this->m_pMerchantGroupDefinitionSheet->EndDialog(0);

					if(IsWindow(this->m_pMerchantPayParamSheet->m_hWnd))
						this->m_pMerchantPayParamSheet->EndDialog(0);

					if(IsWindow(this->m_pCResetDcoMonitor->m_hWnd))
						this->m_pCResetDcoMonitor->EndDialog(0);
						
					::PostMessage(this->m_hWnd, WM_CLOSE, 0, 0);					
				}
			}
		}
		}
		break;

	
		//On X button press of Dialog
		case WM_CLOSE:				
				
				StopTimer();
				//Delete the SHM segment & Delete Mutex Lock
				shrmem.DestroySharedMemorySegment();
				shrmem.DeleteMutexLock();
				
		break;
		
		default : break;
	}

	return CFrameWnd::DefWindowProc(message, wParam, lParam);
}

void CMainFrame::OnDualencrptionKey() 
{	
	UCF01 dekey_ucf01;
	int cnt = 0, rcode;
	char strErrorMessage[256];
	memset(&dekey_ucf01, 0x00, sizeof(UCF01));
	memcpy(dekey_ucf01.primary_key.name, ucf01.primary_key.name, sizeof(ucf01.primary_key.name));
	rcode = txdsapi_Select_Ucf01_DualEncryptionKey_Update_Count(&dekey_ucf01, strErrorMessage);
	
	if(rcode != PTEMSG_OK)
	{
		AfxMessageBox(strErrorMessage);
		return;
	}

	cnt = atoi((char *)dekey_ucf01.dekey);
	if(cnt == 2)
	{
		AfxMessageBox("Access denied as 2 users have already updated Encryption keys to Database.", MB_ICONSTOP);
		return;
	}

	m_pDualEnKeyCtrlPropertySheet->m_DualEncrKeyControl.m_StrUserNameFld=ucf01.primary_key.name;
	m_pDualEnKeyCtrlPropertySheet->SetTitle("Dual Encryption Key");
	m_pDualEnKeyCtrlPropertySheet->DoModal();	
	
}

void CMainFrame::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	CFrameWnd::OnChar(nChar, nRepCnt, nFlags);
}

BOOL CMainFrame::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	cntr=0;

	c_time = CTime::GetCurrentTime();	
	shrmem.SetLastUpdateTime(c_time);

	return CFrameWnd::OnCommand(wParam, lParam);
}


void CMainFrame::OnInitMenu(CMenu* pMenu) 
{
	CFrameWnd::OnInitMenu(pMenu);

	UpdateData(FALSE);
	
}

int CMainFrame::StartTimer(int time)
{
	int handle = SetTimer (IDT_TIMER, time, NULL);  // Start the Timer

	return handle;
}

bool CMainFrame::StopTimer()
{

	if (!KillTimer (timer_handle))	//Stop Timer	
	{
				//::MessageBox(NULL,"Error While Killing the Timer","ShutEvt",1);	
		return false;
	}

	return true;
}


void CMainFrame::ResetCntr()
{
	cntr = 0;
}

short CMainFrame::LockScreen()
{
	int retcode = 0;

	CIdleLoginfrm idleloginfrm;

	//Stop the Timer & Display Login Screen
	StopTimer();

	ResetCntr();

	if(pLock->Lock())
	{
		status = shrmem.GetIdleStatus();

		if(status == 0)
		{
			FlashWindow(true);

			AfxMessageBox("Session Timeout. Please Login");
			
			/****set the session log out in ucf01 table - >buffer column as "NL"-Not Logged -TF_Veena****
			 strcpy((char *)ucf01.buffer,"NL");
			/**************************************************************************/
			
			ret = idleloginfrm.CheckLoginDetails();

			if(ret == CANCEL)
			{
				//Update SHM with IdleStatus as 2 meaning to Close all the Apps
				shrmem.SetIdleStatus(2);
				
				retcode = -1;

			}
			else
			{				
				shrmem.SetIdleStatus(1);
				retcode =  1;
			}
		}
		else if (status == 1)
		{
				retcode = 0;
			// Continue. Password check was successful.
		}
		else if (status == 2)
		{
			retcode =  -1;
			
		}
		else
		{
			retcode = -2;
			LogEvent("Unkown SHM Status",ERROR_MSG);
			// TODO:  Error case - log a message. Status is out of sync.
		}
		//After Login Validation is successful, release the Mutex Lock
		pLock->Unlock();
	}

	StartTimer(1000);

	return retcode;
	
}


void CMainFrame::OnOverrideDenial() 
{
	CIdleLoginfrm login;
	login.m_userid = ucf01.primary_key.name;
	login.m_userpwd = _T("");
	login.m_szTitle = _T("Override Merchant Denial");
	login.m_bDisableNameFld  = TRUE;
	int ret = login.CheckLoginDetails();
	if(ret == false || ret == FALSE || ret == CANCEL) 
		return;
	m_pOverrideMerchantDenialPropertySheet->DoModal();

}
void CMainFrame::OnMCCDefinition() 
{
	CMCCRangeDefinition dlg;
	dlg.DoModal();
}

void CMainFrame::OnResetMonitorConnections() 
{
	// TODO: Add your command handler code here
	char error_msg[200] = {0};
	int ret = MessageBox("Do you want to Reset the monitor connections?\n"
					"WARNING:\n"
					"If you select YES, This will close all the existing monitor connections.\n"
					"All the open monitors will be automatically closed.",
					"Are you sure",MB_ICONWARNING | MB_YESNO | MB_DEFBUTTON2);
	if(ret == IDYES)
	{
		int rcode = txdsapi_clear_monitor_sockets( error_msg );
		if( rcode != PTEMSG_OK )
			AfxMessageBox( error_msg, MB_ICONSTOP | MB_OK );
		else
		{
			AfxMessageBox("All open Monitors are closed.\n"
							"The connections are reset",MB_ICONINFORMATION);
		}
	}

}

void CMainFrame::OnMerchantPayParam() 
{
	m_pMerchantPayParamSheet->SetData();
	m_pMerchantPayParamSheet->DoModal();
	
}

void CMainFrame::OnMerchantGroupDefinition() 
{
	m_pMerchantGroupDefinitionSheet->SetData();
	m_pMerchantGroupDefinitionSheet->DoModal();
	
}

void CMainFrame::OnResetmultiplemonitorconnections()
{
	int retCode = -1;
	int retCode_Dialog = 0;
	char error_buf[100] = "";

	get_log_flag_ini_data();
	//connect to 3 secondary monitors before sending reset requests.
	retCode = process_dco_reset_connections(error_buf);
	if(retCode != PTEMSG_LOGIN_FAILED && retCode != PTEMSG_NOT_FOUND)
	{
		write_to_reset_file_c("Calling m_pCResetDcoMonitor->DoModal()");
		retCode_Dialog = m_pCResetDcoMonitor->DoModal();
		if(retCode_Dialog == -1)
			write_to_reset_file_c("Failed to open Reset DCO dialog prompt");
			}
			else
		AfxMessageBox(error_buf, MB_ICONINFORMATION);
}