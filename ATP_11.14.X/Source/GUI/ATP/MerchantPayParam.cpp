// MerchantPayParam.cpp : implementation file
//

#include "stdafx.h"
#include "atp.h"
#include "MerchantPayParam.h"
#include "MyUtils.h"

#include "SharedMem.h"	

extern "C"
{
#include "PteTime.h"
}

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


extern UCF01 ucf01;
//extern merch_ctr;
extern int merch_ctr;
CString strBrokenPeriodInterest;
extern CSharedMem shrmem;

/////////////////////////////////////////////////////////////////////////////
// CMerchantPayParam property page

IMPLEMENT_DYNCREATE(CMerchantPayParam, CPropertyPage)

CMerchantPayParam::CMerchantPayParam() : CPropertyPage(CMerchantPayParam::IDD)
{
	//{{AFX_DATA_INIT(CMerchantPayParam)
	m_StrEffectiveDate = _T("");
	m_StrBPIMinimumAmount = _T("");
	m_StrEarlyRepaymentFeePercentage = _T("");
	m_StrAuthPercentOverlimit = _T("");
	m_StrProductCode = _T("");
	//}}AFX_DATA_INIT
}

CMerchantPayParam::~CMerchantPayParam()
{
}

CString CMerchantPayParam::convert_ddmmyyyy_to_yyyymmdd( CString input_date )
{
   char output_date[11]="";
   char instring[11]="";
   CString outstring;

   /* Input  date is format DD/MM/YYYY.
    * Output date is format YYYYMMDD.
    */
   memcpy( instring, input_date, sizeof(instring) );
   memcpy( output_date,   instring+6, 4 );  /* Year  */
   memcpy( output_date+4, instring+3, 2 );  /* Month */
   memcpy( output_date+6, instring,   2 );  /* Day   */

   outstring = output_date;
   return( outstring );
}

CString CMerchantPayParam::convert_yyyymmdd_to_ddmmyyyy( CString input_date )
{
   char output_date[11]="";
   char instring[11]="";
   CString outstring;

   /* Input  date is format YYYYMMDD.
    * Output date is format DD/MM/YYYY.
    */
   memcpy( instring, input_date, sizeof(instring) );
   memcpy( output_date,   instring+6, 2 );  /* Day   */
   strcat( output_date, "/" );
   memcpy( output_date+3, instring+4, 2 );  /* Month */
   strcat( output_date, "/" );
   memcpy( output_date+6, instring,   4 );  /* Year  */

   outstring = output_date;
   return( outstring );
}

void CMerchantPayParam::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMerchantPayParam)
	DDX_Control(pDX, IDC_PRODUCT_CODE, m_CntlProductCode);
	DDX_Control(pDX, IDC_BPI_CHECK, m_CntlBPICheck);
	DDX_Text(pDX, IDC_EFFECTIVE_DATE_FLD, m_StrEffectiveDate);
	DDV_MaxChars(pDX, m_StrEffectiveDate			, 8);
	DDX_Control(pDX, IDC_EFFECTIVE_DATE_FLD, m_CntlEffectiveDate);
	DDX_Text(pDX, IDC_MIN_MAJOR_PUR_AMT, m_StrMinMajorPurchaseAmount);
	DDV_MaxChars(pDX, m_StrMinMajorPurchaseAmount		, 12);
	DDX_Control(pDX, IDC_MIN_MAJOR_PUR_AMT, m_CntlMinMajorPurchaseAmount);
	DDX_Text(pDX, IDC_MAX_MAJOR_PUR_AMT, m_StrMaxMajorPurchaseAmount);
	DDV_MaxChars(pDX, m_StrMaxMajorPurchaseAmount		, 12);
	DDX_Control(pDX, IDC_MAX_MAJOR_PUR_AMT, m_CntlMaxMajorPurchaseAmount);
	DDX_Text(pDX, IDC_MIN_INSTALL_AMT, m_StrMinInstalmentAmount);
	DDV_MaxChars(pDX, m_StrMinInstalmentAmount		, 12);
	DDX_Control(pDX, IDC_MIN_INSTALL_AMT, m_CntlMinInstalmentAmount);
	DDX_Text(pDX, IDC_MAX_INSTALL_AMT, m_StrMaxInstalmentAmount);
	DDV_MaxChars(pDX, m_StrMaxInstalmentAmount		, 12);
	DDX_Control(pDX, IDC_MAX_INSTALL_AMT, m_CntlMaxInstalmentAmount);
	DDX_Text(pDX, IDC_MIN_MAJOR_PUR_LIMIT, m_StrMinMajorPurchaseLimit);
	DDV_MaxChars(pDX, m_StrMinMajorPurchaseLimit		, 10);
	DDX_Control(pDX, IDC_MIN_MAJOR_PUR_LIMIT, m_CntlMinMajorPurchaseLimit);
	DDX_Text(pDX, IDC_BPI_MIN_AMOUNT, m_StrBPIMinimumAmount);
	DDV_MaxChars(pDX, m_StrBPIMinimumAmount			, 12);
	DDX_Control(pDX, IDC_BPI_MIN_AMOUNT, m_CntlBPIMinimumAmount);
	DDX_Text(pDX, IDC_MIN_PUR_AMT_FEE, m_StrMinPurchaseAmtForFee);
	DDV_MaxChars(pDX, m_StrMinPurchaseAmtForFee		, 12);
	DDX_Control(pDX, IDC_MIN_PUR_AMT_FEE, m_CntlMinPurchaseAmtForFee);
	DDX_Text(pDX, IDC_FIXED_MP_FEE, m_StrFixedMPFee);
	DDV_MaxChars(pDX, m_StrFixedMPFee			, 15);
	DDX_Control(pDX, IDC_FIXED_MP_FEE, m_CntlFixedMPFee);
	DDX_Text(pDX, IDC_MP_FEE_PERCENT, m_StrMPFeePercentage);
	DDV_MaxChars(pDX, m_StrMPFeePercentage			, 6);
	DDX_Control(pDX, IDC_MP_FEE_PERCENT, m_CntlMPFeePercentage);
	DDX_Text(pDX, IDC_EARLY_REPAY_FEE_PERCENT, m_StrEarlyRepaymentFeePercentage);
	DDV_MaxChars(pDX, m_StrEarlyRepaymentFeePercentage	, 8);
	DDX_Control(pDX, IDC_EARLY_REPAY_FEE_PERCENT, m_CntlEarlyRepaymentFeePercentage);
	DDX_Text(pDX, IDC_MIN_PENALTY_AMT, m_StrMinPenaltyAmount);
	DDV_MaxChars(pDX, m_StrMinPenaltyAmount			, 15);
	DDX_Control(pDX, IDC_MIN_PENALTY_AMT, m_CntlMinPenaltyAmount);
	DDX_Text(pDX, IDC_MAX_PENALTY_AMT, m_StrMaxPenaltyAmount);
	DDV_MaxChars(pDX, m_StrMaxPenaltyAmount			, 15);
	DDX_Control(pDX, IDC_MAX_PENALTY_AMT, m_CntlMaxPenaltyAmount);
	DDX_Text(pDX, IDC_PENALTY_PERCENT, m_StrPenaltyPercentage);
	DDV_MaxChars(pDX, m_StrPenaltyPercentage		, 7);
	DDX_Control(pDX, IDC_PENALTY_PERCENT, m_CntlPenaltyPercentage);
	DDX_Text(pDX, IDC_PENALTY_DAYS, m_StrPenaltyDays);
	DDV_MaxChars(pDX, m_StrPenaltyDays			, 3);
	DDX_Control(pDX, IDC_PENALTY_DAYS, m_CntlPenaltyDays);
	DDX_Text(pDX, IDC_MIN_AMT_AGING, m_StrMinAmtForAging);
	DDV_MaxChars(pDX, m_StrMinAmtForAging			, 18);
	DDX_Control(pDX, IDC_MIN_AMT_AGING, m_CntlMinAmtForAging);
	DDX_Text(pDX, IDC_MONTHS_WRITEOFF, m_StrMonthsToWriteOff);
	DDV_MaxChars(pDX, m_StrMonthsToWriteOff			, 2);
	DDX_Control(pDX, IDC_MONTHS_WRITEOFF, m_CntlMonthsToWriteOff);
	DDX_Text(pDX, IDC_ERR_UNAUTH_AMT, m_StrErrorUnauthorizedAmount);
	DDV_MaxChars(pDX, m_StrErrorUnauthorizedAmount		, 15);
	DDX_Control(pDX, IDC_ERR_UNAUTH_AMT, m_CntlErrorUnauthorizedAmount);
	DDX_Text(pDX, IDC_MAX_TRANS_DAY, m_StrMaxNoOfTransPerDay);
	DDV_MaxChars(pDX, m_StrMaxNoOfTransPerDay		, 2);
	DDX_Control(pDX, IDC_MAX_TRANS_DAY, m_CntlMaxNoOfTransPerDay);
	DDX_Text(pDX, IDC_MAX_AMT_DAY, m_StrMaxAmountPerDay);
	DDV_MaxChars(pDX, m_StrMaxAmountPerDay			, 15);
	DDX_Control(pDX, IDC_MAX_AMT_DAY, m_CntlMaxAmountPerDay);
	DDX_Text(pDX, IDC_AUTH_OVERLIMIT, m_StrAuthPercentOverlimit);
	DDV_MaxChars(pDX, m_StrAuthPercentOverlimit		, 7);
	DDX_Control(pDX, IDC_AUTH_OVERLIMIT, m_CntlAuthPercentOverlimit);
	DDX_Text(pDX, IDC_RESP_CODE_1, m_StrRespCode1);
	DDV_MaxChars(pDX, m_StrRespCode1			, 2);
	DDX_Control(pDX, IDC_RESP_CODE_1, m_CntlRespCode1);
	DDX_Text(pDX, IDC_RESP_CODE_2, m_StrRespCode2);
	DDV_MaxChars(pDX, m_StrRespCode2				, 2);
	DDX_Control(pDX, IDC_RESP_CODE_2, m_CntlRespCode2);
	DDX_Text(pDX, IDC_RESP_CODE_3, m_StrRespCode3);
	DDV_MaxChars(pDX, m_StrRespCode3			, 2);
	DDX_Control(pDX, IDC_RESP_CODE_3, m_CntlRespCode3);
	DDX_Text(pDX, IDC_MSG_CODE_1, m_StrMessage1);
	DDV_MaxChars(pDX, m_StrMessage1				, 2);
	DDX_Control(pDX, IDC_MSG_CODE_1, m_CntlMessage1);
	DDX_Text(pDX, IDC_MSG_CODE_2, m_StrMessage2);
	DDV_MaxChars(pDX, m_StrMessage2			, 2);
	DDX_Control(pDX, IDC_MSG_CODE_2, m_CntlMessage2);
	DDX_Text(pDX, IDC_MSG_CODE_3, m_StrMessage3);
	DDV_MaxChars(pDX, m_StrMessage3				, 2);
	DDX_Control(pDX, IDC_MSG_CODE_3, m_CntlMessage3);
	DDX_Text(pDX, IDC_PRODUCT_CODE, m_StrProductCode);
	DDV_MaxChars(pDX, m_StrProductCode			, 4);

	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CMerchantPayParam, CPropertyPage)
	//{{AFX_MSG_MAP(CMerchantPayParam)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMerchantPayParam message handlers


void CMerchantPayParam::OnCancel() 
{
	// TODO: Add your specialized code here and/or call the base class
	
	CPropertyPage::OnCancel();
}

BOOL CMerchantPayParam::PopulateGeneralDialogFromDatabase ( MPPARA01 sMerchantPayParameter)
{

		ZeroMemory ( &sMerchantPayParameterRecordOld, sizeof ( MPPARA01) );
		
		m_StrProductCode = sMerchantPayParameter.primary_key.product_code;
		m_StrEffectiveDate = sMerchantPayParameter.effective_date;
		m_StrMinMajorPurchaseAmount = sMerchantPayParameter.minmajpurchamt;
		m_StrMaxMajorPurchaseAmount= sMerchantPayParameter.maxmajpurchamt;
		m_StrMinInstalmentAmount= sMerchantPayParameter.mininstamt;
		m_StrMaxInstalmentAmount= sMerchantPayParameter.maxinstamt;
		m_StrMinMajorPurchaseLimit= sMerchantPayParameter.minmajpurchlmt;
		m_StrBPIMinimumAmount= sMerchantPayParameter.BPIminamt;
		m_StrBrokenPeriodInterest= sMerchantPayParameter.brokenperdinst;
		m_StrMinPurchaseAmtForFee= sMerchantPayParameter.minfreepurchamt;
		m_StrFixedMPFee= sMerchantPayParameter.fixedmpfee;
		m_StrMPFeePercentage= sMerchantPayParameter.mpfeepercnt;
		m_StrEarlyRepaymentFeePercentage= sMerchantPayParameter.earlyrepayfeeper;
		m_StrMinPenaltyAmount= sMerchantPayParameter.minpenaltyamt;
		m_StrMaxPenaltyAmount= sMerchantPayParameter.maxpenaltyamt;
		m_StrPenaltyPercentage= sMerchantPayParameter.penaltyper;
		m_StrPenaltyDays= sMerchantPayParameter.penaltyday;
		m_StrMinAmtForAging= sMerchantPayParameter.minamtofaging;
		m_StrMonthsToWriteOff= sMerchantPayParameter.mntowrtof;
		m_StrErrorUnauthorizedAmount= sMerchantPayParameter.errorunauthamt;
		m_StrMaxNoOfTransPerDay= sMerchantPayParameter.maxtrnperday;
		m_StrMaxAmountPerDay= sMerchantPayParameter.maxamtperday;
		m_StrAuthPercentOverlimit= sMerchantPayParameter.authoverlimit;
		m_StrRespCode1= sMerchantPayParameter.authpara[0].response_code;
		m_StrMessage1 = sMerchantPayParameter.authpara[0].message;
		m_StrRespCode2= sMerchantPayParameter.authpara[1].response_code;
		m_StrMessage2 = sMerchantPayParameter.authpara[1].message;
		m_StrRespCode3= sMerchantPayParameter.authpara[2].response_code;
		m_StrMessage3 = sMerchantPayParameter.authpara[2].message;

		/*if ( m_StrBrokenPeriodInterest == "1" )
			m_CntlBPICheck.SetCheck(1);
		else if ( m_StrBrokenPeriodInterest == "0")
			m_CntlBPICheck.SetCheck(0);*/

	
	
	//UpdateData(FALSE);
	
	memcpy ( &sMerchantPayParameterRecordOld, &sMerchantPayParameter, sizeof ( MPPARA01));
	
	
	return TRUE;
}

void CMerchantPayParam::CleanGeneralDialog()
{
	m_StrEffectiveDate = _T("");
	m_StrMinMajorPurchaseAmount = _T("");
	m_StrMaxMajorPurchaseAmount = _T("");
	m_StrMinInstalmentAmount = _T("");
	m_StrMaxInstalmentAmount = _T("");
	m_StrMinMajorPurchaseLimit = _T("");
	m_StrBPIMinimumAmount = _T("");
	m_StrBrokenPeriodInterest = _T("");
	m_StrMinPurchaseAmtForFee = _T("");
	m_StrFixedMPFee = _T("");
	m_StrMPFeePercentage = _T("");
	m_StrEarlyRepaymentFeePercentage = _T("");
	m_StrMinPenaltyAmount = _T("");
	m_StrMaxPenaltyAmount = _T("");
	m_StrPenaltyPercentage = _T("");
	m_StrPenaltyDays = _T("");
	m_StrMinAmtForAging = _T("");
	m_StrMonthsToWriteOff = _T("");
	m_StrErrorUnauthorizedAmount = _T("");
	m_StrMaxNoOfTransPerDay = _T("");
	m_StrMaxAmountPerDay = _T("");
	m_StrAuthPercentOverlimit = _T("");
	m_StrRespCode1 = _T("");
	m_StrMessage1 = _T("");
	m_StrRespCode2 = _T("");
	m_StrMessage2 = _T("");
	m_StrRespCode3 = _T("");
	m_StrMessage3 = _T("");
	m_StrProductCode = _T("");
}

BOOL CMerchantPayParam::PopulateDatabaseFromGeneralDialog ( pMPPARA01 pMerchantPayParamRecord)
{
	UpdateData(TRUE);
	
	memcpy( pMerchantPayParamRecord->primary_key.product_code,     m_StrProductCode  ,	sizeof(pMerchantPayParamRecord->primary_key.product_code ));
	memcpy( pMerchantPayParamRecord->effective_date,               m_StrEffectiveDate,	sizeof(pMerchantPayParamRecord->effective_date  ));
	memcpy( pMerchantPayParamRecord->minmajpurchamt,               m_StrMinMajorPurchaseAmount,	sizeof(pMerchantPayParamRecord->minmajpurchamt  ));
	memcpy( pMerchantPayParamRecord->maxmajpurchamt,               m_StrMaxMajorPurchaseAmount,	sizeof(pMerchantPayParamRecord->maxmajpurchamt  ));
	memcpy( pMerchantPayParamRecord->mininstamt,                   m_StrMinInstalmentAmount,	sizeof(pMerchantPayParamRecord->mininstamt  ));
	memcpy( pMerchantPayParamRecord->maxinstamt,                   m_StrMaxInstalmentAmount,	sizeof(pMerchantPayParamRecord->maxinstamt  ));
	memcpy( pMerchantPayParamRecord->minmajpurchlmt,               m_StrMinMajorPurchaseLimit,	sizeof(pMerchantPayParamRecord->minmajpurchlmt  ));
	memcpy( pMerchantPayParamRecord->BPIminamt,					   m_StrBPIMinimumAmount,	sizeof(pMerchantPayParamRecord->BPIminamt  ));
	memcpy( pMerchantPayParamRecord->minfreepurchamt,              m_StrMinPurchaseAmtForFee,	sizeof(pMerchantPayParamRecord->minfreepurchamt  ));
	memcpy( pMerchantPayParamRecord->fixedmpfee,				   m_StrFixedMPFee,	sizeof(pMerchantPayParamRecord->fixedmpfee  ));
	memcpy( pMerchantPayParamRecord->mpfeepercnt,                  m_StrMPFeePercentage,	sizeof(pMerchantPayParamRecord->mpfeepercnt  ));
	memcpy( pMerchantPayParamRecord->earlyrepayfeeper,             m_StrEarlyRepaymentFeePercentage,	sizeof(pMerchantPayParamRecord->earlyrepayfeeper ));
	memcpy( pMerchantPayParamRecord->minpenaltyamt,                m_StrMinPenaltyAmount,	sizeof(pMerchantPayParamRecord->minpenaltyamt  ));
	memcpy( pMerchantPayParamRecord->maxpenaltyamt,                m_StrMaxPenaltyAmount,	sizeof(pMerchantPayParamRecord->maxpenaltyamt  ));
	memcpy( pMerchantPayParamRecord->penaltyper,                   m_StrPenaltyPercentage,	sizeof(pMerchantPayParamRecord->penaltyper  ));
	memcpy( pMerchantPayParamRecord->penaltyday,                   m_StrPenaltyDays,	sizeof(pMerchantPayParamRecord->penaltyday  ));
	memcpy( pMerchantPayParamRecord->minamtofaging,                m_StrMinAmtForAging,	sizeof(pMerchantPayParamRecord->minamtofaging  ));
	memcpy( pMerchantPayParamRecord->mntowrtof,                    m_StrMonthsToWriteOff,	sizeof(pMerchantPayParamRecord->mntowrtof  ));
	memcpy( pMerchantPayParamRecord->errorunauthamt,               m_StrErrorUnauthorizedAmount,	sizeof(pMerchantPayParamRecord->errorunauthamt  ));
	memcpy( pMerchantPayParamRecord->maxtrnperday,                 m_StrMaxNoOfTransPerDay,	sizeof(pMerchantPayParamRecord->maxtrnperday  ));
	memcpy( pMerchantPayParamRecord->maxamtperday,                 m_StrMaxAmountPerDay,	sizeof(pMerchantPayParamRecord->maxamtperday  ));
	memcpy( pMerchantPayParamRecord->authoverlimit,                m_StrAuthPercentOverlimit,	sizeof(pMerchantPayParamRecord->authoverlimit  ));
	memcpy( pMerchantPayParamRecord->authpara[0].response_code,    m_StrRespCode1,	sizeof(pMerchantPayParamRecord->authpara[0].response_code  ));
	memcpy( pMerchantPayParamRecord->authpara[0].message,          m_StrMessage1,	sizeof(pMerchantPayParamRecord->authpara[0].message));
	memcpy( pMerchantPayParamRecord->authpara[1].response_code,    m_StrRespCode2,	sizeof(pMerchantPayParamRecord->authpara[1].response_code  ));
	memcpy( pMerchantPayParamRecord->authpara[1].message,          m_StrMessage2,	sizeof(pMerchantPayParamRecord->authpara[1].message));
	memcpy( pMerchantPayParamRecord->authpara[2].response_code,    m_StrRespCode3,	sizeof(pMerchantPayParamRecord->authpara[2].response_code  ));
	memcpy( pMerchantPayParamRecord->authpara[2].message,          m_StrMessage3,	sizeof(pMerchantPayParamRecord->authpara[2].message));
	if ( m_CntlBPICheck.GetCheck() == 1)
			memcpy( pMerchantPayParamRecord->brokenperdinst,  "1",	sizeof(pMerchantPayParamRecord->brokenperdinst  ));
		else
			memcpy( pMerchantPayParamRecord->brokenperdinst,  "0",	sizeof(pMerchantPayParamRecord->brokenperdinst  ));
	strBrokenPeriodInterest = pMerchantPayParamRecord->brokenperdinst;
	return TRUE;
}

void CMerchantPayParam::EnableMerchantGroupDialog(BOOL nEnable)
{
	m_CntlProductCode.EnableWindow(nEnable);
	m_CntlEffectiveDate.EnableWindow(nEnable);
	m_CntlMinMajorPurchaseAmount.EnableWindow(nEnable);
	m_CntlMaxMajorPurchaseAmount.EnableWindow(nEnable);
	m_CntlMinInstalmentAmount.EnableWindow(nEnable);
	m_CntlMaxInstalmentAmount.EnableWindow(nEnable);
	m_CntlMinMajorPurchaseLimit.EnableWindow(nEnable);
	m_CntlBPIMinimumAmount.EnableWindow(nEnable);
	m_CntlBPICheck.EnableWindow(nEnable);
	m_CntlMinPurchaseAmtForFee.EnableWindow(nEnable);
	m_CntlFixedMPFee.EnableWindow(nEnable);
	m_CntlMPFeePercentage.EnableWindow(nEnable);
	m_CntlEarlyRepaymentFeePercentage.EnableWindow(nEnable);
	m_CntlMinPenaltyAmount.EnableWindow(nEnable);
	m_CntlMaxPenaltyAmount.EnableWindow(nEnable);
	m_CntlPenaltyPercentage.EnableWindow(nEnable);
	m_CntlPenaltyDays.EnableWindow(nEnable);
	m_CntlMinAmtForAging.EnableWindow(nEnable);
	m_CntlMonthsToWriteOff.EnableWindow(nEnable);
	m_CntlErrorUnauthorizedAmount.EnableWindow(nEnable);
	m_CntlMaxNoOfTransPerDay.EnableWindow(nEnable);
	m_CntlMaxAmountPerDay.EnableWindow(nEnable);
	m_CntlAuthPercentOverlimit.EnableWindow(nEnable);
	m_CntlRespCode1.EnableWindow(nEnable);
	m_CntlMessage1.EnableWindow(nEnable);
	m_CntlRespCode2.EnableWindow(nEnable);
	m_CntlMessage2.EnableWindow(nEnable);
	m_CntlRespCode3.EnableWindow(nEnable);
	m_CntlMessage3.EnableWindow(nEnable);
}


BOOL CMerchantPayParam::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
	m_bGeneral = TRUE;

	if (m_nDialogMode != MODE_DONE)
	{
		if ( m_nDialogMode != MODE_ADD )
		{
			PopulateGeneralDialogFromDatabase ( sMerchantPayParameterRecordOld);
		}
		if(m_nDialogMode == MODE_ADD || m_nDialogMode == MODE_EDIT)
		{
			EnableMerchantGroupDialog(TRUE);
		}
		else
		{
			EnableMerchantGroupDialog(FALSE);
		}
	}

	if (!UpdateData(FALSE)){
		AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
	}
		
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CMerchantPayParam::OnCommand(WPARAM wParam, LPARAM lParam) 
{

	if ( m_nDialogMode != MODE_VIEW &&  m_nDialogMode != MODE_DONE)
		SetModified (TRUE);
	c_time = CTime::GetCurrentTime();	
	shrmem.SetLastUpdateTime(c_time);

	return CPropertyPage::OnCommand(wParam, lParam);
}

void CMerchantPayParam::PopulateHistoryTable()
{
	HST01 sHistoryData;
    CHAR dest_str[18]="";   
	CMyUtils myUtilsObject;
	char strErrorMsg[200] = "";
    char strDate[9] = "";
    char strTime[7] = "";
	char strPrimaryKey[50];
	CHAR strDisplayErrMsg[250];
    CString temp;
	CHAR s_merch_ctr[4]   = "";
	
	memset ( strDate, 0, sizeof (strDate));
    memset ( strTime, 0, sizeof (strTime));
	memset ( strPrimaryKey, 0, sizeof (strPrimaryKey));

	strcpy ( strPrimaryKey, m_StrProductCode);
	    
    myUtilsObject.GetSystemDate(strDate);
    myUtilsObject.GetSystemTime(strTime);

	//  generate a timestamp for the current Greenwich Mean Time
	// in format YYYYMMDDHHIISSJJJ .

    pintime_get_gmt (dest_str);
	merch_ctr++;
   if ( merch_ctr > 999 )
      merch_ctr = 0;
   sprintf( s_merch_ctr, "%03d", merch_ctr );
   memcpy( &dest_str[14], s_merch_ctr, 3 );

    ZeroMemory ( &sHistoryData, sizeof (HST01));
    CHAR wstation[21]; DWORD wsize;

    wsize = sizeof( wstation );
    GetComputerName( wstation, &wsize );

	memcpy ((LPTSTR) sHistoryData.reserved,(LPTSTR)wstation, sizeof(sHistoryData.reserved));
    memcpy ((LPTSTR) sHistoryData.file_name,     (LPCTSTR)"MPPARA01", sizeof (sHistoryData.file_name));
	memcpy ((LPTSTR) sHistoryData.field_tag,     (LPCTSTR)m_strFieldTag, sizeof (sHistoryData.file_name));
    memcpy ((LPTSTR) sHistoryData.file_primary_key,  (LPCTSTR)strPrimaryKey, sizeof (sHistoryData.file_primary_key));
    memcpy ((LPTSTR) sHistoryData.primary_key.unique_id,     (LPCTSTR)dest_str, sizeof (sHistoryData.primary_key.unique_id));        
    memcpy ((LPTSTR) sHistoryData.time_modified,     (LPCTSTR)strTime, sizeof (sHistoryData.time_modified)); 
    memcpy ((LPTSTR) sHistoryData.current_data, (LPCTSTR)m_strCurrentData, sizeof (sHistoryData.current_data));
	memcpy ((LPTSTR) sHistoryData.previous_data, (LPCTSTR)m_strPreviousData, sizeof (sHistoryData.previous_data));
	memcpy ((LPTSTR) sHistoryData.changed_by,(LPTSTR)ucf01.primary_key.name, sizeof(sHistoryData.changed_by));
    temp = strDate; 
    temp = temp.Right(4) + temp.Left(2) + temp.Mid(2,2);
    
    memcpy ((LPTSTR) sHistoryData.date_modified, temp, sizeof (sHistoryData.date_modified));

    int rcode = txdsapi_insert_record  ( (pBYTE)&sHistoryData, sizeof (HST01), HST01_DATA, strErrorMsg );
    if ( rcode != PTEMSG_OK )
    {
      m_nDialogMode = MODE_DONE;
      memset( strDisplayErrMsg, 0x00, sizeof(strDisplayErrMsg) );
      sprintf( strDisplayErrMsg,
              "Unable to update History Table HST01 - %s",
               strErrorMsg );
      AfxMessageBox( strDisplayErrMsg, MB_OK );
   }
   return;
	/* function PopulateHistoryTable */
}

BOOL CMerchantPayParam::CompareStructures()
{
	CString Deferred_start_date;
    CString Deferred_end_date;
    CMyUtils myUtilsObject;

	if ( m_bGeneral == TRUE )
	{
		if (strcmp ( m_StrMinMajorPurchaseAmount, (char *)sMerchantPayParameterRecordOld.minmajpurchamt) != 0)
		{
			 memset(m_strCurrentData,  0x00, sizeof(m_strCurrentData) );
			 memset(m_strPreviousData, 0x00, sizeof(m_strPreviousData));
			 memset(m_strFieldTag,     0x00, sizeof(m_strFieldTag)    );

			 memcpy(m_strCurrentData, m_StrMinMajorPurchaseAmount, sizeof (m_strCurrentData));
			 memcpy(m_strPreviousData, sMerchantPayParameterRecordOld.minmajpurchamt, sizeof (m_strPreviousData));
			 memcpy(m_strFieldTag, "Min Major Purchase Amount ", sizeof (m_strFieldTag));

			 PopulateHistoryTable();
		}

		if (strcmp ( m_StrMaxMajorPurchaseAmount, (char *)sMerchantPayParameterRecordOld.maxmajpurchamt) != 0)
		{
			 memset(m_strCurrentData,  0x00, sizeof(m_strCurrentData) );
			 memset(m_strPreviousData, 0x00, sizeof(m_strPreviousData));
			 memset(m_strFieldTag,     0x00, sizeof(m_strFieldTag)    );

			 memcpy(m_strCurrentData, m_StrMaxMajorPurchaseAmount, sizeof (m_strCurrentData));
			 memcpy(m_strPreviousData, sMerchantPayParameterRecordOld.maxmajpurchamt, sizeof (m_strPreviousData));
			 memcpy(m_strFieldTag, "Max Major Purchase Amount ", sizeof (m_strFieldTag));

			 PopulateHistoryTable();
		}


		if (strcmp ( m_StrMinInstalmentAmount, (char *)sMerchantPayParameterRecordOld.mininstamt) != 0)
		{
			 memset(m_strCurrentData,  0x00, sizeof(m_strCurrentData) );
			 memset(m_strPreviousData, 0x00, sizeof(m_strPreviousData));
			 memset(m_strFieldTag,     0x00, sizeof(m_strFieldTag)    );

			 memcpy(m_strCurrentData, m_StrMinInstalmentAmount, sizeof (m_strCurrentData));
			 memcpy(m_strPreviousData, sMerchantPayParameterRecordOld.mininstamt, sizeof (m_strPreviousData));
			 memcpy(m_strFieldTag, "Min Installment Amount ", sizeof (m_strFieldTag));

			 PopulateHistoryTable();
		}

		if (strcmp ( m_StrMaxInstalmentAmount, (char *)sMerchantPayParameterRecordOld.maxinstamt) != 0)
		{
			 memset(m_strCurrentData,  0x00, sizeof(m_strCurrentData) );
			 memset(m_strPreviousData, 0x00, sizeof(m_strPreviousData));
			 memset(m_strFieldTag,     0x00, sizeof(m_strFieldTag)    );

			 memcpy(m_strCurrentData, m_StrMaxInstalmentAmount, sizeof (m_strCurrentData));
			 memcpy(m_strPreviousData, sMerchantPayParameterRecordOld.maxinstamt, sizeof (m_strPreviousData));
			 memcpy(m_strFieldTag, "Max Installment Amount ", sizeof (m_strFieldTag));

			 PopulateHistoryTable();
		}

		if (strcmp ( m_StrMinMajorPurchaseLimit, (char *)sMerchantPayParameterRecordOld.minmajpurchlmt) != 0)
		{
			 memset(m_strCurrentData,  0x00, sizeof(m_strCurrentData) );
			 memset(m_strPreviousData, 0x00, sizeof(m_strPreviousData));
			 memset(m_strFieldTag,     0x00, sizeof(m_strFieldTag)    );

			 memcpy(m_strCurrentData, m_StrMinMajorPurchaseLimit, sizeof (m_strCurrentData));
			 memcpy(m_strPreviousData, sMerchantPayParameterRecordOld.minmajpurchlmt, sizeof (m_strPreviousData));
			 memcpy(m_strFieldTag, "Min Major Purchase Amount limitt ", sizeof (m_strFieldTag));

			 PopulateHistoryTable();
		}

		if (strcmp ( m_StrMinPenaltyAmount, (char *)sMerchantPayParameterRecordOld.minpenaltyamt) != 0)
		{
			 memset(m_strCurrentData,  0x00, sizeof(m_strCurrentData) );
			 memset(m_strPreviousData, 0x00, sizeof(m_strPreviousData));
			 memset(m_strFieldTag,     0x00, sizeof(m_strFieldTag)    );

			 memcpy(m_strCurrentData, m_StrMinPenaltyAmount, sizeof (m_strCurrentData));
			 memcpy(m_strPreviousData, sMerchantPayParameterRecordOld.minpenaltyamt, sizeof (m_strPreviousData));
			 memcpy(m_strFieldTag, "Min Penalty Amount ", sizeof (m_strFieldTag));

			 PopulateHistoryTable();

		}if (strcmp ( m_StrMaxPenaltyAmount, (char *)sMerchantPayParameterRecordOld.maxpenaltyamt) != 0)
		{
			 memset(m_strCurrentData,  0x00, sizeof(m_strCurrentData) );
			 memset(m_strPreviousData, 0x00, sizeof(m_strPreviousData));
			 memset(m_strFieldTag,     0x00, sizeof(m_strFieldTag)    );

			 memcpy(m_strCurrentData, m_StrMaxPenaltyAmount, sizeof (m_strCurrentData));
			 memcpy(m_strPreviousData, sMerchantPayParameterRecordOld.maxpenaltyamt, sizeof (m_strPreviousData));
			 memcpy(m_strFieldTag, "Max Penalty Amount ", sizeof (m_strFieldTag));

			 PopulateHistoryTable();
		}

		if (strcmp ( m_StrPenaltyPercentage, (char *)sMerchantPayParameterRecordOld.penaltyper) != 0)
		{
			 memset(m_strCurrentData,  0x00, sizeof(m_strCurrentData) );
			 memset(m_strPreviousData, 0x00, sizeof(m_strPreviousData));
			 memset(m_strFieldTag,     0x00, sizeof(m_strFieldTag)    );

			 memcpy(m_strCurrentData, m_StrPenaltyPercentage, sizeof (m_strCurrentData));
			 memcpy(m_strPreviousData, sMerchantPayParameterRecordOld.penaltyper, sizeof (m_strPreviousData));
			 memcpy(m_strFieldTag, "Penalty Percentage ", sizeof (m_strFieldTag));

			 PopulateHistoryTable();
		}

		if (strcmp ( m_StrPenaltyDays, (char *)sMerchantPayParameterRecordOld.penaltyday) != 0)
		{
			 memset(m_strCurrentData,  0x00, sizeof(m_strCurrentData) );
			 memset(m_strPreviousData, 0x00, sizeof(m_strPreviousData));
			 memset(m_strFieldTag,     0x00, sizeof(m_strFieldTag)    );

			 memcpy(m_strCurrentData, m_StrPenaltyDays, sizeof (m_strCurrentData));
			 memcpy(m_strPreviousData, sMerchantPayParameterRecordOld.penaltyday, sizeof (m_strPreviousData));
			 memcpy(m_strFieldTag, "Penalty days ", sizeof (m_strFieldTag));

			 PopulateHistoryTable();
		}

		if (strcmp ( m_StrMinAmtForAging, (char *)sMerchantPayParameterRecordOld.minamtofaging) != 0)
		{
			 memset(m_strCurrentData,  0x00, sizeof(m_strCurrentData) );
			 memset(m_strPreviousData, 0x00, sizeof(m_strPreviousData));
			 memset(m_strFieldTag,     0x00, sizeof(m_strFieldTag)    );

			 memcpy(m_strCurrentData, m_StrMinAmtForAging, sizeof (m_strCurrentData));
			 memcpy(m_strPreviousData, sMerchantPayParameterRecordOld.minamtofaging, sizeof (m_strPreviousData));
			 memcpy(m_strFieldTag, "Min Amt of Aging ", sizeof (m_strFieldTag));

			 PopulateHistoryTable();
		}

		if (strcmp ( m_StrMonthsToWriteOff, (char *)sMerchantPayParameterRecordOld.mntowrtof) != 0)
		{
			 memset(m_strCurrentData,  0x00, sizeof(m_strCurrentData) );
			 memset(m_strPreviousData, 0x00, sizeof(m_strPreviousData));
			 memset(m_strFieldTag,     0x00, sizeof(m_strFieldTag)    );

			 memcpy(m_strCurrentData, m_StrMonthsToWriteOff, sizeof (m_strCurrentData));
			 memcpy(m_strPreviousData, sMerchantPayParameterRecordOld.mntowrtof, sizeof (m_strPreviousData));
			 memcpy(m_strFieldTag, "Months to write off ", sizeof (m_strFieldTag));

			 PopulateHistoryTable();
		}

		if (strcmp ( m_StrBPIMinimumAmount, (char *)sMerchantPayParameterRecordOld.BPIminamt) != 0)
		{
			 memset(m_strCurrentData,  0x00, sizeof(m_strCurrentData) );
			 memset(m_strPreviousData, 0x00, sizeof(m_strPreviousData));
			 memset(m_strFieldTag,     0x00, sizeof(m_strFieldTag)    );

			 memcpy(m_strCurrentData, m_StrBPIMinimumAmount, sizeof (m_strCurrentData));
			 memcpy(m_strPreviousData, sMerchantPayParameterRecordOld.BPIminamt, sizeof (m_strPreviousData));
			 memcpy(m_strFieldTag, "BPI Minimum Amount ", sizeof (m_strFieldTag));

			 PopulateHistoryTable();
		}

		if (strcmp ( strBrokenPeriodInterest, (char *)sMerchantPayParameterRecordOld.brokenperdinst) != 0)
		{
			 memset(m_strCurrentData,  0x00, sizeof(m_strCurrentData) );
			 memset(m_strPreviousData, 0x00, sizeof(m_strPreviousData));
			 memset(m_strFieldTag,     0x00, sizeof(m_strFieldTag)    );

			 memcpy(m_strCurrentData, strBrokenPeriodInterest, sizeof (m_strCurrentData));
			 memcpy(m_strPreviousData, sMerchantPayParameterRecordOld.brokenperdinst, sizeof (m_strPreviousData));
			 memcpy(m_strFieldTag, "Broken Period Interest ", sizeof (m_strFieldTag));

			 PopulateHistoryTable();
		}

		if (strcmp ( m_StrMinPurchaseAmtForFee, (char *)sMerchantPayParameterRecordOld.minfreepurchamt) != 0)
		{
			 memset(m_strCurrentData,  0x00, sizeof(m_strCurrentData) );
			 memset(m_strPreviousData, 0x00, sizeof(m_strPreviousData));
			 memset(m_strFieldTag,     0x00, sizeof(m_strFieldTag)    );

			 memcpy(m_strCurrentData, m_StrMinPurchaseAmtForFee, sizeof (m_strCurrentData));
			 memcpy(m_strPreviousData, sMerchantPayParameterRecordOld.minfreepurchamt, sizeof (m_strPreviousData));
			 memcpy(m_strFieldTag, "Min Purchase Amt For Fee ", sizeof (m_strFieldTag));

			 PopulateHistoryTable();
		}

		if (strcmp ( m_StrErrorUnauthorizedAmount, (char *)sMerchantPayParameterRecordOld.errorunauthamt) != 0)
		{
			 memset(m_strCurrentData,  0x00, sizeof(m_strCurrentData) );
			 memset(m_strPreviousData, 0x00, sizeof(m_strPreviousData));
			 memset(m_strFieldTag,     0x00, sizeof(m_strFieldTag)    );

			 memcpy(m_strCurrentData, m_StrErrorUnauthorizedAmount, sizeof (m_strCurrentData));
			 memcpy(m_strPreviousData, sMerchantPayParameterRecordOld.errorunauthamt, sizeof (m_strPreviousData));
			 memcpy(m_strFieldTag, " Error Unauthorized Amount ", sizeof (m_strFieldTag));

			 PopulateHistoryTable();
		}

		if (strcmp ( m_StrFixedMPFee, (char *)sMerchantPayParameterRecordOld.fixedmpfee) != 0)
		{
			 memset(m_strCurrentData,  0x00, sizeof(m_strCurrentData) );
			 memset(m_strPreviousData, 0x00, sizeof(m_strPreviousData));
			 memset(m_strFieldTag,     0x00, sizeof(m_strFieldTag)    );

			 memcpy(m_strCurrentData, m_StrFixedMPFee, sizeof (m_strCurrentData));
			 memcpy(m_strPreviousData, sMerchantPayParameterRecordOld.fixedmpfee, sizeof (m_strPreviousData));
			 memcpy(m_strFieldTag, "Fixed MP Fee ", sizeof (m_strFieldTag));

			 PopulateHistoryTable();
		}
		
		if (strcmp ( m_StrMPFeePercentage, (char *)sMerchantPayParameterRecordOld.mpfeepercnt) != 0)
		{
			 memset(m_strCurrentData,  0x00, sizeof(m_strCurrentData) );
			 memset(m_strPreviousData, 0x00, sizeof(m_strPreviousData));
			 memset(m_strFieldTag,     0x00, sizeof(m_strFieldTag)    );

			 memcpy(m_strCurrentData, m_StrMPFeePercentage, sizeof (m_strCurrentData));
			 memcpy(m_strPreviousData, sMerchantPayParameterRecordOld.mpfeepercnt, sizeof (m_strPreviousData));
			 memcpy(m_strFieldTag, " MP Fee Percentage ", sizeof (m_strFieldTag));

			 PopulateHistoryTable();
		}

		if (strcmp ( m_StrEarlyRepaymentFeePercentage, (char *)sMerchantPayParameterRecordOld.earlyrepayfeeper) != 0)
		{
			 memset(m_strCurrentData,  0x00, sizeof(m_strCurrentData) );
			 memset(m_strPreviousData, 0x00, sizeof(m_strPreviousData));
			 memset(m_strFieldTag,     0x00, sizeof(m_strFieldTag)    );

			 memcpy(m_strCurrentData, m_StrEarlyRepaymentFeePercentage, sizeof (m_strCurrentData));
			 memcpy(m_strPreviousData, sMerchantPayParameterRecordOld.earlyrepayfeeper, sizeof (m_strPreviousData));
			 memcpy(m_strFieldTag, " Early Repayment Fee Percentage ", sizeof (m_strFieldTag));

			 PopulateHistoryTable();
		}

		if (strcmp ( m_StrEffectiveDate, (char *)sMerchantPayParameterRecordOld.effective_date) != 0)
		{
			 memset(m_strCurrentData,  0x00, sizeof(m_strCurrentData) );
			 memset(m_strPreviousData, 0x00, sizeof(m_strPreviousData));
			 memset(m_strFieldTag,     0x00, sizeof(m_strFieldTag)    );

			 memcpy(m_strCurrentData, m_StrEffectiveDate, sizeof (m_strCurrentData));
			 memcpy(m_strPreviousData, sMerchantPayParameterRecordOld.effective_date, sizeof (m_strPreviousData));
			 memcpy(m_strFieldTag, "Effective Date ", sizeof (m_strFieldTag));

			 PopulateHistoryTable();
		}
		if (strcmp ( m_StrMaxNoOfTransPerDay, (char *)sMerchantPayParameterRecordOld.maxtrnperday) != 0)
		{
			 memset(m_strCurrentData,  0x00, sizeof(m_strCurrentData) );
			 memset(m_strPreviousData, 0x00, sizeof(m_strPreviousData));
			 memset(m_strFieldTag,     0x00, sizeof(m_strFieldTag)    );

			 memcpy(m_strCurrentData, m_StrMaxNoOfTransPerDay, sizeof (m_strCurrentData));
			 memcpy(m_strPreviousData, sMerchantPayParameterRecordOld.maxtrnperday, sizeof (m_strPreviousData));
			 memcpy(m_strFieldTag, "Max No Of Trans Per Day ", sizeof (m_strFieldTag));

			 PopulateHistoryTable();
		}

		if (strcmp ( m_StrMaxAmountPerDay, (char *)sMerchantPayParameterRecordOld.maxamtperday) != 0)
		{
			 memset(m_strCurrentData,  0x00, sizeof(m_strCurrentData) );
			 memset(m_strPreviousData, 0x00, sizeof(m_strPreviousData));
			 memset(m_strFieldTag,     0x00, sizeof(m_strFieldTag)    );

			 memcpy(m_strCurrentData, m_StrMaxAmountPerDay, sizeof (m_strCurrentData));
			 memcpy(m_strPreviousData, sMerchantPayParameterRecordOld.maxamtperday, sizeof (m_strPreviousData));
			 memcpy(m_strFieldTag, "Max Amount Per Day ", sizeof (m_strFieldTag));

			 PopulateHistoryTable();
		}

		if (strcmp ( m_StrAuthPercentOverlimit, (char *)sMerchantPayParameterRecordOld.authoverlimit) != 0)
		{
			 memset(m_strCurrentData,  0x00, sizeof(m_strCurrentData) );
			 memset(m_strPreviousData, 0x00, sizeof(m_strPreviousData));
			 memset(m_strFieldTag,     0x00, sizeof(m_strFieldTag)    );

			 memcpy(m_strCurrentData, m_StrAuthPercentOverlimit, sizeof (m_strCurrentData));
			 memcpy(m_strPreviousData, sMerchantPayParameterRecordOld.authoverlimit, sizeof (m_strPreviousData));
			 memcpy(m_strFieldTag, "Auth Percent Over limit ", sizeof (m_strFieldTag));

			 PopulateHistoryTable();
		}

		if (strcmp ( m_StrRespCode1, (char *)sMerchantPayParameterRecordOld.authpara[0].response_code) != 0)
		{
			 memset(m_strCurrentData,  0x00, sizeof(m_strCurrentData) );
			 memset(m_strPreviousData, 0x00, sizeof(m_strPreviousData));
			 memset(m_strFieldTag,     0x00, sizeof(m_strFieldTag)    );

			 memcpy(m_strCurrentData, m_StrRespCode1, sizeof (m_strCurrentData));
			 memcpy(m_strPreviousData, sMerchantPayParameterRecordOld.authpara[0].response_code, sizeof (m_strPreviousData));
			 memcpy(m_strFieldTag, "Resp Code ", sizeof (m_strFieldTag));

			 PopulateHistoryTable();
		}

		if (strcmp ( m_StrMessage1, (char *)sMerchantPayParameterRecordOld.authpara[0].message) != 0)
		{
			 memset(m_strCurrentData,  0x00, sizeof(m_strCurrentData) );
			 memset(m_strPreviousData, 0x00, sizeof(m_strPreviousData));
			 memset(m_strFieldTag,     0x00, sizeof(m_strFieldTag)    );

			 memcpy(m_strCurrentData, m_StrMessage1, sizeof (m_strCurrentData));
			 memcpy(m_strPreviousData, sMerchantPayParameterRecordOld.authpara[0].message, sizeof (m_strPreviousData));
			 memcpy(m_strFieldTag, "Message ", sizeof (m_strFieldTag));

			 PopulateHistoryTable();
		}

		if (strcmp ( m_StrRespCode2, (char *)sMerchantPayParameterRecordOld.authpara[1].response_code) != 0)
		{
			 memset(m_strCurrentData,  0x00, sizeof(m_strCurrentData) );
			 memset(m_strPreviousData, 0x00, sizeof(m_strPreviousData));
			 memset(m_strFieldTag,     0x00, sizeof(m_strFieldTag)    );

			 memcpy(m_strCurrentData, m_StrRespCode2, sizeof (m_strCurrentData));
			 memcpy(m_strPreviousData, sMerchantPayParameterRecordOld.authpara[1].response_code, sizeof (m_strPreviousData));
			 memcpy(m_strFieldTag, "Resp Code ", sizeof (m_strFieldTag));

			 PopulateHistoryTable();
		}

		if (strcmp ( m_StrMessage2, (char *)sMerchantPayParameterRecordOld.authpara[1].message) != 0)
		{
			 memset(m_strCurrentData,  0x00, sizeof(m_strCurrentData) );
			 memset(m_strPreviousData, 0x00, sizeof(m_strPreviousData));
			 memset(m_strFieldTag,     0x00, sizeof(m_strFieldTag)    );

			 memcpy(m_strCurrentData, m_StrMessage2, sizeof (m_strCurrentData));
			 memcpy(m_strPreviousData, sMerchantPayParameterRecordOld.authpara[1].message, sizeof (m_strPreviousData));
			 memcpy(m_strFieldTag, "Message ", sizeof (m_strFieldTag));

			 PopulateHistoryTable();
		}

		if (strcmp ( m_StrRespCode3, (char *)sMerchantPayParameterRecordOld.authpara[2].response_code) != 0)
		{
			 memset(m_strCurrentData,  0x00, sizeof(m_strCurrentData) );
			 memset(m_strPreviousData, 0x00, sizeof(m_strPreviousData));
			 memset(m_strFieldTag,     0x00, sizeof(m_strFieldTag)    );

			 memcpy(m_strCurrentData, m_StrRespCode3, sizeof (m_strCurrentData));
			 memcpy(m_strPreviousData, sMerchantPayParameterRecordOld.authpara[2].response_code, sizeof (m_strPreviousData));
			 memcpy(m_strFieldTag, "Resp Code ", sizeof (m_strFieldTag));

			 PopulateHistoryTable();
		}

		if (strcmp ( m_StrMessage3, (char *)sMerchantPayParameterRecordOld.authpara[2].message) != 0)
		{
			 memset(m_strCurrentData,  0x00, sizeof(m_strCurrentData) );
			 memset(m_strPreviousData, 0x00, sizeof(m_strPreviousData));
			 memset(m_strFieldTag,     0x00, sizeof(m_strFieldTag)    );

			 memcpy(m_strCurrentData, m_StrMessage3, sizeof (m_strCurrentData));
			 memcpy(m_strPreviousData, sMerchantPayParameterRecordOld.authpara[2].message, sizeof (m_strPreviousData));
			 memcpy(m_strFieldTag, "Message ", sizeof (m_strFieldTag));

			 PopulateHistoryTable();
		}
	}

	return TRUE;
}
