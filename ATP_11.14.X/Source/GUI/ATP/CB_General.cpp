// CB_General.cpp : implementation file
//

#include "stdafx.h"
#include "atp.h"
#include "CB_General.h"

extern "C"
{
#include "PteTime.h"
}

extern UCF01 ucf01;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#include "SharedMem.h"

extern CSharedMem shrmem;
/////////////////////////////////////////////////////////////////////////////
// CCB_General property page

IMPLEMENT_DYNCREATE(CCB_General, CPropertyPage)

CCB_General::CCB_General() : CPropertyPage(CCB_General::IDD)
{
	//{{AFX_DATA_INIT(CCB_General)
	m_StrCashBonusId = _T("");
	m_StrMaxPeriodWins = _T("");
	m_StrMessageLbl = _T("");
	m_StrPeriodWinCtr = _T("");
	m_StrEndDate = _T("");
	m_StrCashDesc = _T("");
	m_StrProgramWinCtr = _T("");
	m_StrStartDate = _T("");
	m_StrThreshold = _T("");
	m_StrTimePeriod = _T("");
	m_StrTxnCtr = _T("");
	m_StrMaxProgramWinners = _T("");
	m_StrMultiWinnerRespText = _T("");
	m_StrTagId = _T("");
	m_BoolMultWinsAllowed = FALSE;
	//}}AFX_DATA_INIT
}

CCB_General::~CCB_General()
{
}

void CCB_General::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCB_General)
	DDX_Control(pDX, IDC_TXN_CTR, m_CntlTxnCtr);
	DDX_Control(pDX, IDC_TIME_PERIOD, m_CntlTimePeriod);
	DDX_Control(pDX, IDC_THRESHOLD, m_CntlThreshold);
	DDX_Control(pDX, IDC_START_DATE, m_CntlStartDate);
	DDX_Control(pDX, IDC_PROGRAM_WIN_CTR, m_CntlProgramWinCtr);
	DDX_Control(pDX, IDC_PROGRAM_DESC, m_CntlCashDesc);
	DDX_Control(pDX, IDC_PERIOD_WIN_CTR, m_CntlPeriodWinCtr);
	DDX_Control(pDX, IDC_MULTI_WINNER_RESP_TEXT, m_CntlMultiWinnerRespText);
	DDX_Control(pDX, IDC_MAX_PROGRAM_WINNERS, m_CntlMaxProgramWinners);
	DDX_Control(pDX, IDC_MAX_PERIOD_WINS, m_CntlMaxPeriodWins);
	DDX_Control(pDX, IDC_END_DATE, m_CntlEndDate);
	DDX_Control(pDX, IDC_CB_ID_FLD, m_CntlCashBonusId);
	DDX_Control(pDX, IDC_MULT_WINS_CHECK, m_MultWinsAllowed);
	DDX_Text(pDX, IDC_CB_ID_FLD, m_StrCashBonusId);
	DDX_Text(pDX, IDC_MAX_PERIOD_WINS, m_StrMaxPeriodWins);
	DDV_MaxChars(pDX, m_StrMaxPeriodWins, 6);
	DDX_Text(pDX, IDC_MESSAGE_LBL, m_StrMessageLbl);
	DDX_Text(pDX, IDC_PERIOD_WIN_CTR, m_StrPeriodWinCtr);
	DDV_MaxChars(pDX, m_StrPeriodWinCtr, 6);
	DDX_Text(pDX, IDC_END_DATE, m_StrEndDate);
	DDV_MaxChars(pDX, m_StrEndDate, 8);
	DDX_Text(pDX, IDC_PROGRAM_DESC, m_StrCashDesc);
	DDV_MaxChars(pDX, m_StrCashDesc, 40);
	DDX_Text(pDX, IDC_PROGRAM_WIN_CTR, m_StrProgramWinCtr);
	DDV_MaxChars(pDX, m_StrProgramWinCtr, 6);
	DDX_Text(pDX, IDC_START_DATE, m_StrStartDate);
	DDV_MaxChars(pDX, m_StrStartDate, 8);
	DDX_Text(pDX, IDC_THRESHOLD, m_StrThreshold);
	DDV_MaxChars(pDX, m_StrThreshold, 9);
	DDX_Text(pDX, IDC_TIME_PERIOD, m_StrTimePeriod);
	DDV_MaxChars(pDX, m_StrTimePeriod, 4);
	DDX_Text(pDX, IDC_TXN_CTR, m_StrTxnCtr);
	DDV_MaxChars(pDX, m_StrTxnCtr, 9);
	DDX_Text(pDX, IDC_MAX_PROGRAM_WINNERS, m_StrMaxProgramWinners);
	DDV_MaxChars(pDX, m_StrMaxProgramWinners, 6);
	DDX_Text(pDX, IDC_MULTI_WINNER_RESP_TEXT, m_StrMultiWinnerRespText);
	DDV_MaxChars(pDX, m_StrMultiWinnerRespText, 40);
	DDX_Text(pDX, IDC_TAG_ID, m_StrTagId);
	DDV_MaxChars(pDX, m_StrTagId, 1);
	DDX_Check(pDX, IDC_MULT_WINS_CHECK, m_BoolMultWinsAllowed);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CCB_General, CPropertyPage)
	//{{AFX_MSG_MAP(CCB_General)
	ON_EN_KILLFOCUS(IDC_END_DATE, OnKillfocusEndDate)
	ON_EN_KILLFOCUS(IDC_MAX_PERIOD_WINS, OnKillfocusMaxPeriodWins)
	ON_EN_KILLFOCUS(IDC_MAX_PROGRAM_WINNERS, OnKillfocusMaxProgramWinners)
	ON_EN_KILLFOCUS(IDC_PERIOD_WIN_CTR, OnKillfocusPeriodWinCtr)
	ON_EN_KILLFOCUS(IDC_PROGRAM_WIN_CTR, OnKillfocusProgramWinCtr)
	ON_EN_KILLFOCUS(IDC_START_DATE, OnKillfocusStartDate)
	ON_EN_KILLFOCUS(IDC_THRESHOLD, OnKillfocusThreshold)
	ON_EN_KILLFOCUS(IDC_TIME_PERIOD, OnKillfocusTimePeriod)
	ON_EN_KILLFOCUS(IDC_TXN_CTR, OnKillfocusTxnCtr)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCB_General message handlers


/******************************************************************************************/

void CCB_General::CleanGeneralDialog()
{
   m_StrCashDesc            = _T("");
   m_StrStartDate           = _T("");
   m_StrEndDate             = _T("");
   m_StrTimePeriod          = _T("");
   m_StrMaxPeriodWins       = _T("");
   m_StrPeriodWinCtr        = _T("");
   m_StrMaxProgramWinners   = _T("");
   m_StrProgramWinCtr       = _T("");
   m_StrThreshold           = _T("");
   m_StrTxnCtr              = _T("");
   m_StrMultiWinnerRespText = _T("");
   m_StrMessageLbl          = _T("");
	m_MultWinsAllowed.SetCheck(0);

   UpdateData(FALSE);
}

/******************************************************************************************/
/******************************************************************************************/

void CCB_General::EnableGeneralDialog(int nStatus)
{
   m_CntlCashBonusId.EnableWindow(FALSE);
   m_CntlCashDesc.EnableWindow(nStatus);
   m_CntlStartDate.EnableWindow(nStatus);
   m_CntlEndDate.EnableWindow(nStatus);
   m_CntlTimePeriod.EnableWindow(nStatus);
   m_CntlMaxPeriodWins.EnableWindow(nStatus);
   m_CntlPeriodWinCtr.EnableWindow(nStatus);
   m_CntlMaxProgramWinners.EnableWindow(nStatus);
   m_CntlProgramWinCtr.EnableWindow(nStatus);
   m_CntlThreshold.EnableWindow(nStatus);
   m_CntlTxnCtr.EnableWindow(nStatus);
   m_CntlMultiWinnerRespText.EnableWindow(nStatus);
   m_MultWinsAllowed.EnableWindow(nStatus);
}


/******************************************************************************/
/******************************************************************************/

BOOL CCB_General::PopulateBon02DialogFromDatabase( BON02 sBon02 )
{
   ZeroMemory( &sBon02Old, sizeof(BON02) );  

   m_StrCashBonusId         = sBon02.cash_id;
   m_StrTagId               = sBon02.tag_id;
   m_StrCashDesc            = sBon02.cash_desc;
   m_StrStartDate           = sBon02.program_start_date;
   m_StrEndDate             = sBon02.program_end_date;
   m_StrTimePeriod          = sBon02.time_period;
   m_StrMaxPeriodWins       = sBon02.max_period_winners;
   m_StrPeriodWinCtr        = sBon02.period_win_ctr;
   m_StrMaxProgramWinners   = sBon02.max_program_winners;
   m_StrProgramWinCtr       = sBon02.program_win_ctr;
   m_StrThreshold           = sBon02.tran_threshold;
   m_StrTxnCtr              = sBon02.tran_ctr;
   m_StrMultiWinnerRespText = sBon02.multi_winner_text;

   if ( sBon02.multi_winners_allowed[0] == 'Y' )
   {
      m_MultWinsAllowed.SetCheck(1);
      m_BoolMultWinsAllowed = TRUE;
   }
   else
      m_BoolMultWinsAllowed = FALSE;

   memcpy( &sBon02Old, &sBon02, sizeof(BON02) );
   return TRUE;
}
/******************************************************************************/
/******************************************************************************/

BOOL CCB_General::PopulateDatabaseFromBon02Dialog( pBON02 psBon02 )
{
   memcpy( psBon02->cash_id,             m_StrCashBonusId,         sizeof(psBon02->cash_id)            );
   memcpy( psBon02->tag_id,              m_StrTagId,               sizeof(psBon02->tag_id)             );
   memcpy( psBon02->cash_desc,           m_StrCashDesc,            sizeof(psBon02->cash_desc)          );
   memcpy( psBon02->program_start_date,  m_StrStartDate,           sizeof(psBon02->program_start_date) );
   memcpy( psBon02->program_end_date,    m_StrEndDate,             sizeof(psBon02->program_end_date)   );
   memcpy( psBon02->time_period,         m_StrTimePeriod,          sizeof(psBon02->time_period)        );
   memcpy( psBon02->max_period_winners,  m_StrMaxPeriodWins,       sizeof(psBon02->max_period_winners) );
   memcpy( psBon02->period_win_ctr,      m_StrPeriodWinCtr,        sizeof(psBon02->period_win_ctr)     );
   memcpy( psBon02->max_program_winners, m_StrMaxProgramWinners,   sizeof(psBon02->max_program_winners));
   memcpy( psBon02->program_win_ctr,     m_StrProgramWinCtr,       sizeof(psBon02->program_win_ctr)    );
   memcpy( psBon02->tran_threshold,      m_StrThreshold,           sizeof(psBon02->tran_threshold)     );
   memcpy( psBon02->tran_ctr,            m_StrTxnCtr,              sizeof(psBon02->tran_ctr)           );
   memcpy( psBon02->multi_winner_text,   m_StrMultiWinnerRespText, sizeof(psBon02->multi_winner_text)  );

   if ( m_MultWinsAllowed.GetCheck() == 1 )
      memcpy ( psBon02->multi_winners_allowed, "Y", 1 );
   else
      memcpy ( psBon02->multi_winners_allowed, "N", 1 );

   /* This line is for Change History. */
	m_StrMultWinsAllowed = psBon02->multi_winners_allowed;
   return TRUE;
}

/******************************************************************************/
/******************************************************************************/

BOOL CCB_General::OnInitDialog() 
{
   CPropertyPage::OnInitDialog();
   CFont m_font;
   LOGFONT lf;

   memset ( &lf, 0, sizeof (lf));
   lf.lfHeight = 12;
   lf.lfWeight = 700;
   strcpy(lf.lfFaceName, "Arial");
   m_font.CreateFontIndirect (&lf);
   GetDlgItem(IDC_MESSAGE_LBL)->SetFont (&m_font);    

   m_bGeneral = TRUE;

   if (m_nDialogMode != MODE_DONE )
   {
      if ( m_nDialogMode == MODE_VIEW )
         EnableGeneralDialog(FALSE);
      else
         EnableGeneralDialog(TRUE);

      if ( m_nDialogMode != MODE_ADD )
         PopulateBon02DialogFromDatabase( sBon02Old );
      else
         CleanGeneralDialog();
   }

   if (!UpdateData(FALSE))
   {
      AfxMessageBox (IDS_UPDATEDATAFAILED, MB_ICONSTOP);
   }

   return TRUE;  // return TRUE unless you set the focus to a control
                 // EXCEPTION: OCX Property Pages should return FALSE
}


BOOL CCB_General::CompareStructures()
{
   if ( 0 != strcmp(m_StrCashBonusId, (char *)sBon02Old.cash_id) )
   {
      memcpy( m_StrCurrentData,  m_StrCashBonusId,  sizeof(m_StrCurrentData));
      memcpy( m_StrPreviousData, sBon02Old.cash_id, sizeof(m_StrPreviousData));
      memcpy( m_StrFieldTag,    "Cash Bonus ID",    sizeof(m_StrFieldTag));
      PopulateHistoryTable();
   }

   if ( 0 != strcmp(m_StrCashDesc, (char *)sBon02Old.cash_desc) )
   {
      memcpy( m_StrCurrentData,  m_StrCashDesc,          sizeof(m_StrCurrentData));
      memcpy( m_StrPreviousData, sBon02Old.cash_desc,    sizeof(m_StrPreviousData));
      memcpy( m_StrFieldTag,    "Cash Bonus Description",sizeof(m_StrFieldTag));
      PopulateHistoryTable();
   }

   if ( 0 != strcmp(m_StrStartDate, (char *)sBon02Old.program_start_date) )
   {
      memcpy( m_StrCurrentData,  m_StrStartDate,  sizeof(m_StrCurrentData));
      memcpy( m_StrPreviousData, sBon02Old.program_start_date, sizeof(m_StrPreviousData));
      memcpy( m_StrFieldTag,    "Program Start Date",    sizeof(m_StrFieldTag));
      PopulateHistoryTable();
   }

   if ( 0 != strcmp(m_StrEndDate, (char *)sBon02Old.program_end_date) )
   {
      memcpy( m_StrCurrentData,  m_StrEndDate,  sizeof(m_StrCurrentData));
      memcpy( m_StrPreviousData, sBon02Old.program_end_date, sizeof(m_StrPreviousData));
      memcpy( m_StrFieldTag,    "Program End Date",    sizeof(m_StrFieldTag));
      PopulateHistoryTable();
   }

   if ( 0 != strcmp(m_StrTimePeriod, (char *)sBon02Old.time_period) )
   {
      memcpy( m_StrCurrentData,  m_StrTimePeriod,  sizeof(m_StrCurrentData));
      memcpy( m_StrPreviousData, sBon02Old.time_period, sizeof(m_StrPreviousData));
      memcpy( m_StrFieldTag,    "Time Period",    sizeof(m_StrFieldTag));
      PopulateHistoryTable();
   }

   if ( 0 != strcmp(m_StrMaxPeriodWins, (char *)sBon02Old.max_period_winners) )
   {
      memcpy( m_StrCurrentData,  m_StrMaxPeriodWins,  sizeof(m_StrCurrentData));
      memcpy( m_StrPreviousData, sBon02Old.max_period_winners, sizeof(m_StrPreviousData));
      memcpy( m_StrFieldTag,    "Max Period Winners",    sizeof(m_StrFieldTag));
      PopulateHistoryTable();
   }

   if ( 0 != strcmp(m_StrMaxProgramWinners, (char *)sBon02Old.max_program_winners) )
   {
      memcpy( m_StrCurrentData,  m_StrMaxProgramWinners,  sizeof(m_StrCurrentData));
      memcpy( m_StrPreviousData, sBon02Old.max_program_winners, sizeof(m_StrPreviousData));
      memcpy( m_StrFieldTag,    "Max Program Winners",    sizeof(m_StrFieldTag));
      PopulateHistoryTable();
   }

   if ( 0 != strcmp(m_StrThreshold, (char *)sBon02Old.tran_threshold) )
   {
      memcpy( m_StrCurrentData,  m_StrThreshold,  sizeof(m_StrCurrentData));
      memcpy( m_StrPreviousData, sBon02Old.tran_threshold, sizeof(m_StrPreviousData));
      memcpy( m_StrFieldTag,    "Transaction Threshold",    sizeof(m_StrFieldTag));
      PopulateHistoryTable();
   }

   if ( 0 != strcmp(m_StrMultiWinnerRespText, (char *)sBon02Old.multi_winner_text) )
   {
      memcpy( m_StrCurrentData,  m_StrMultiWinnerRespText,  sizeof(m_StrCurrentData));
      memcpy( m_StrPreviousData, sBon02Old.multi_winner_text, sizeof(m_StrPreviousData));
      memcpy( m_StrFieldTag,    "Multi Winner Text",    sizeof(m_StrFieldTag));
      PopulateHistoryTable();
   }

   if ( 0 != strcmp(m_StrMultWinsAllowed, (char *)sBon02Old.multi_winners_allowed) )
   {
      memcpy( m_StrCurrentData,  m_StrMultWinsAllowed,            sizeof(m_StrCurrentData));
      memcpy( m_StrPreviousData, sBon02Old.multi_winners_allowed, sizeof(m_StrPreviousData));
      memcpy( m_StrFieldTag,    "Mult Wins Allowed",              sizeof(m_StrFieldTag));
      PopulateHistoryTable();
   }

   return TRUE;
}


/******************************************************************************/
/******************************************************************************/

void CCB_General::PopulateHistoryTable()
{
   HST01    sHistoryData;
   CHAR     dest_str[18];   
   CMyUtils myUtilsObject;
   CHAR     strErrorMsg [200] = "";
   CString  temp;

   CHAR strDate[9] = "";
   CHAR strTime[7] = "";
   CHAR strPrimaryKey[50] = "";
   CHAR wstation[21]; DWORD wsize;

   memset( strDate,       0x00, sizeof (strDate) );
   memset( strTime,       0x00, sizeof (strTime) );
   memset( strPrimaryKey, 0x00, sizeof (strPrimaryKey));

   strcpy( strPrimaryKey, m_StrCashBonusId );

   myUtilsObject.GetSystemDate(strDate); 
   myUtilsObject.GetSystemTime(strTime);

   pintime_get_gmt ( dest_str );

   ZeroMemory ( &sHistoryData, sizeof (HST01));
   memcpy ((LPTSTR) sHistoryData.file_name, (LPCTSTR)"BON02", sizeof (sHistoryData.file_name));
   temp = strDate;
   temp = temp.Right(4) + temp.Left(2) + temp.Mid(2,2);

   wsize = sizeof( wstation );
   GetComputerName( wstation, &wsize );

   memcpy ((LPTSTR) sHistoryData.reserved,(LPTSTR)wstation, sizeof(sHistoryData.reserved));
   memcpy ((LPTSTR) sHistoryData.date_modified, temp, sizeof (sHistoryData.date_modified));
   memcpy ((LPTSTR) sHistoryData.file_primary_key,  (LPCTSTR)strPrimaryKey,         sizeof (sHistoryData.file_primary_key));
   memcpy ((LPTSTR) sHistoryData.primary_key.unique_id,     (LPCTSTR)dest_str,      sizeof (sHistoryData.primary_key.unique_id));        
   memcpy ((LPTSTR) sHistoryData.time_modified,     (LPCTSTR)strTime,               sizeof (sHistoryData.time_modified)); 
   memcpy ((LPTSTR) sHistoryData.field_tag,         (LPCTSTR)m_StrFieldTag,         sizeof (sHistoryData.field_tag));
   memcpy ((LPTSTR) sHistoryData.previous_data,     (LPCTSTR)m_StrPreviousData,     sizeof (sHistoryData.previous_data));
   memcpy ((LPTSTR) sHistoryData.current_data,      (LPCTSTR)m_StrCurrentData,      sizeof (sHistoryData.current_data));
   memcpy ((LPTSTR) sHistoryData.changed_by,        (LPCTSTR)ucf01.primary_key.name,sizeof (sHistoryData.changed_by));

   int rcode = txdsapi_insert_record  ( (pBYTE)&sHistoryData, sizeof (HST01), HST01_DATA, strErrorMsg );
   if ( rcode != PTEMSG_OK )
   {
      m_StrMessageLbl = strErrorMsg;
      m_nDialogMode = MODE_DONE;
      UpdateData( FALSE );
   }
}

void CCB_General::OnKillfocusStartDate() 
{
   CHAR  errbuf[80] = "";

	UpdateData(TRUE);
   m_StrMessageLbl = "";
   UpdateData(FALSE);

   if ( !m_StrStartDate.IsEmpty() )
   {
      if ( m_StrStartDate.GetLength() != 8 )
      {
         m_CntlStartDate.SetFocus();
         m_StrMessageLbl = "Start Date: Invalid length - Must be 8 digits.";
      }
      else if ( !ValidateDate(m_StrStartDate, errbuf) )
      {
         m_CntlStartDate.SetFocus();
         m_StrMessageLbl = errbuf;
      }

      UpdateData(FALSE);
   }
   return;
}

void CCB_General::OnKillfocusEndDate() 
{
   CHAR  errbuf[80] = "";

	UpdateData(TRUE);
   m_StrMessageLbl = "";
   UpdateData(FALSE);

   if ( !m_StrEndDate.IsEmpty() )
   {
      if ( m_StrEndDate.GetLength() != 8 )
      {
         m_CntlEndDate.SetFocus();
         m_StrMessageLbl = "End Date: Invalid length - Must be 8 digits.";
      }
      else if ( !ValidateDate(m_StrEndDate, errbuf) )
      {
         m_CntlEndDate.SetFocus();
         m_StrMessageLbl = errbuf;
      }

      UpdateData(FALSE);
   }
   return;
}

void CCB_General::OnKillfocusMaxPeriodWins() 
{
   INT  res;
   INT  length;

	UpdateData(TRUE);
   m_StrMessageLbl = "";
   UpdateData(FALSE);

   length = m_StrMaxPeriodWins.GetLength();

   res = strspn ( m_StrMaxPeriodWins, "1234567890" );
   if ( res < length )
   {
      m_CntlMaxPeriodWins.SetFocus();
      m_StrMessageLbl = "Max Period Wins: Invalid Character - Must be numeric.";
      UpdateData(FALSE);
   }

   return;
}

void CCB_General::OnKillfocusMaxProgramWinners() 
{
   INT  res;
   INT  length;

	UpdateData(TRUE);
   m_StrMessageLbl = "";
   UpdateData(FALSE);

   length = m_StrMaxProgramWinners.GetLength();

   res = strspn ( m_StrMaxProgramWinners, "1234567890" );
   if ( res < length )
   {
      m_CntlMaxProgramWinners.SetFocus();
      m_StrMessageLbl = "Max Program Winners: Invalid Character - Must be numeric.";
      UpdateData(FALSE);
   }

   return;
}

void CCB_General::OnKillfocusPeriodWinCtr() 
{
   INT  res;
   INT  length;

	UpdateData(TRUE);
   m_StrMessageLbl = "";
   UpdateData(FALSE);

   length = m_StrPeriodWinCtr.GetLength();

   res = strspn ( m_StrPeriodWinCtr, "1234567890" );
   if ( res < length )
   {
      m_CntlPeriodWinCtr.SetFocus();
      m_StrMessageLbl = "Period Win Counter: Invalid Character - Must be numeric.";
      UpdateData(FALSE);
   }

   return;
}

void CCB_General::OnKillfocusProgramWinCtr() 
{
   INT  res;
   INT  length;

	UpdateData(TRUE);
   m_StrMessageLbl = "";
   UpdateData(FALSE);

   length = m_StrProgramWinCtr.GetLength();

   res = strspn ( m_StrProgramWinCtr, "1234567890" );
   if ( res < length )
   {
      m_CntlProgramWinCtr.SetFocus();
      m_StrMessageLbl = "Program Win Counter: Invalid Character - Must be numeric.";
      UpdateData(FALSE);
   }

   return;
}

void CCB_General::OnKillfocusThreshold() 
{
   INT  res;
   INT  length;

	UpdateData(TRUE);
   m_StrMessageLbl = "";
   UpdateData(FALSE);

   length = m_StrThreshold.GetLength();

   res = strspn ( m_StrThreshold, "1234567890" );
   if ( res < length )
   {
      m_CntlThreshold.SetFocus();
      m_StrMessageLbl = "Transaction Threshold: Invalid Character - Must be numeric.";
      UpdateData(FALSE);
   }

   return;
}

void CCB_General::OnKillfocusTimePeriod() 
{
   INT  res;
   INT  length;

	UpdateData(TRUE);
   m_StrMessageLbl = "";
   UpdateData(FALSE);

   length = m_StrTimePeriod.GetLength();

   res = strspn ( m_StrTimePeriod, "1234567890" );
   if ( res < length )
   {
      m_CntlTimePeriod.SetFocus();
      m_StrMessageLbl = "Time Period: Invalid Character - Must be numeric.";
      UpdateData(FALSE);
   }

   return;
}

void CCB_General::OnKillfocusTxnCtr() 
{
   INT  res;
   INT  length;

	UpdateData(TRUE);
   m_StrMessageLbl = "";
   UpdateData(FALSE);

   length = m_StrTxnCtr.GetLength();

   res = strspn ( m_StrTxnCtr, "1234567890" );
   if ( res < length )
   {
      m_CntlTxnCtr.SetFocus();
      m_StrMessageLbl = "Transaction Counter: Invalid Character - Must be numeric.";
      UpdateData(FALSE);
   }

   return;
}


BOOL CCB_General::ValidateDate( CString strDate, pCHAR errbuf )
{
	CString strDay;
	CString strMonth;
	CString strYear;
	CMyUtils myUtilsObject;

	int nDay, nMonth, nYear;

	UpdateData(TRUE);

	if ( myUtilsObject.IsEmpty ( strDate, this ))
		return TRUE;

	int length = strDate.GetLength();	
	int res = strspn ( strDate,"1234567890" );

	if ( length < 8 )
	{
		strcpy( errbuf, "ERROR: Invalid date format" );
		return FALSE;
	}
	
	strDay = strDate.Left(2);
	strMonth = strDate.Mid(2,2);
	strYear = strDate.Right(4);

	nDay = atoi ( strDay );
	nMonth = atoi ( strMonth );
	nYear = atoi ( strYear );

	if ( nDay == 0 || nMonth == 0 || nYear ==0)
	{
		strcpy( errbuf, "ERROR: Invalid data" );
		return FALSE;
	}


	if ( nMonth > 12 )
	{
		strcpy( errbuf, "ERROR: Invalid month" );
		return FALSE;
	}

	if ( nMonth == 1 || nMonth == 3 || nMonth == 5 || 
		 nMonth == 7 || nMonth == 8 || nMonth == 10 || nMonth == 12 )
	{
		if ( nDay > 31 )
		{
			strcpy( errbuf, "ERROR: Invalid day" );
			return FALSE;
		}
	}

	if ( nMonth == 4 || nMonth == 6 || nMonth == 9 || 
		 nMonth == 11 )
	{
		if ( nDay > 30 )
		{
			strcpy( errbuf, "ERROR: Invalid day" );
			return FALSE;
		}
	}

	if ( nMonth ==2 )
	{
		int res = nYear % 4;
		if ( res != 0 && nDay >28)
		{
			strcpy( errbuf, "ERROR: Invalid day" );
			return FALSE;
		}
		if (res == 0 && nDay >29)
		{
			strcpy( errbuf, "ERROR: Invalid day" );
			return FALSE;
		}
	}
	
	return TRUE;
}


BOOL CCB_General::OnKillActive() 
{
   CHAR  errbuf[100];

   if ( m_nDialogMode == MODE_DONE )
      return( TRUE );

   UpdateData(TRUE);
   m_StrMessageLbl = "";
   UpdateData(FALSE);

   if ( !ValidateProgramDuration(errbuf) )
   {
      m_CntlStartDate.SetFocus();
      m_StrMessageLbl = errbuf;
      UpdateData(FALSE);
      return( FALSE );
   }

   return CPropertyPage::OnKillActive();
}

BOOL CCB_General::ValidateProgramDuration( pCHAR errbuf )
{
   /* Make sure end date is not before start date.
    * Remember, the format has already been checked.
    */
   BOOL      retval = TRUE;
   INT       s_year,  e_year;
   INT       s_month, e_month;
   INT       s_day,   e_day;
   CString   tempstr;
	CMyUtils  myUtilsObject;

   /* Check YEAR */
	UpdateData(TRUE);

	if ( !myUtilsObject.IsEmpty ( m_StrStartDate, this ))
   {
      tempstr = m_StrStartDate.Right(4);
      s_year  = atoi( tempstr );

      tempstr = m_StrEndDate.Right(4);
      e_year  = atoi( tempstr );

      if ( e_year < s_year )
      {
         strcpy( errbuf, "Invalid Program Date Range: check the Year" );
         retval = FALSE;
      }
      else if ( e_year == s_year )
      {
         /* Check MONTH */
         tempstr = m_StrStartDate.Mid(2,2);
         s_month = atoi( tempstr );

         tempstr = m_StrEndDate.Mid(2,2);
         e_month = atoi( tempstr );

         if ( e_month < s_month )
         {
            strcpy( errbuf, "Invalid Program Date Range: check the Month" );
            retval = FALSE;
         }
         else if ( e_month == s_month )
         {
            /* Check DAY */
            tempstr = m_StrStartDate.Left(2);
            s_day   = atoi( tempstr );

            tempstr = m_StrEndDate.Left(2);
            e_day   = atoi( tempstr );

            if ( e_day < s_day )
            {
               strcpy( errbuf, "Invalid Program Date Range: check the Day" );
               retval = FALSE;
            }
         }
      }
   }
   return( retval );
}

BOOL CCB_General::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	// TODO: Add your specialized code here and/or call the base class
	
	c_time = CTime::GetCurrentTime();	
	shrmem.SetLastUpdateTime(c_time);
	return CPropertyPage::OnCommand(wParam, lParam);
}
