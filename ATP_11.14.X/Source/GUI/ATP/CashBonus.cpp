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
* $Log:   N:\PVCS\PTE\Equitable\ATP_gui\CashBonus.cpp  $
   
      Rev 1.11   Dec 01 2003 11:29:00   lmitchel
   Added AMEX Online Updates - scr1200
   
      Rev 1.10   Jan 05 2000 14:53:14   gbarnes
   Major update. Not updated to PVCS recently.
   PVCS was locked.
   
      Rev 1.1   Sep 22 1999 15:46:50   iarustam
   Added manual batch entry GUI
   
      Rev 1.0   Jul 01 1999 14:17:10   iarustam
   Initial Release
   
*   
************************************************************************************/
// CashBonus.cpp : implementation file
//

#include "stdafx.h"
#include "atp.h"
#include "CashBonus.h"
#include "MyUtils.h"
#include "SharedMem.h"
extern "C"{
#include "Txutils.h"
}

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern UCF01 ucf01;
extern CSharedMem shrmem;
/////////////////////////////////////////////////////////////////////////////
// CCashBonus property page

//IMPLEMENT_DYNCREATE(CCashBonus, CPropertyPage)

//CCashBonus::CCashBonus() : CPropertyPage(CCashBonus::IDD)
CCashBonus::CCashBonus(CWnd* pParent /*=NULL*/)
	: CDialog(CCashBonus::IDD, pParent)
{
	//{{AFX_DATA_INIT(CCashBonus)
	m_StrCounterFld = _T("");
	m_StrEndDateFld = _T("");
	m_StrMinAmountFld = _T("");
	m_StrResponseFld = _T("");
	m_StrRewardFld = _T("");
	m_StrStartDateFld = _T("");
	m_StrThresholdFld = _T("");
	m_StrMessageLbl = _T("");
	//}}AFX_DATA_INIT
}

/*CCashBonus::~CCashBonus()
{
}*/

void CCashBonus::DoDataExchange(CDataExchange* pDX)
{
//	CPropertyPage::DoDataExchange(pDX);
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCashBonus)
	DDX_Control(pDX, IDC_ADD_BUTTON, m_AddButton);
	DDX_Control(pDX, IDC_THRESHOLD_FLD, m_CntlThresholdFld);
	DDX_Control(pDX, IDC_START_DATE_FLD, m_CntlStartDateFld);
	DDX_Control(pDX, IDC_REWARD_FLD, m_CntlRewardFld);
	DDX_Control(pDX, IDC_RESPONSE_FLD, m_CntlResponseFld);
	DDX_Control(pDX, IDC_MINIMUM_AMOUNT_FLD, m_CntlMinAmountFld);
	DDX_Control(pDX, IDC_END_DATE_FLD, m_CntlEndDateFld);
	DDX_Control(pDX, IDC_COUNTER_FLD, m_CntlCounterFld);
	DDX_Text(pDX, IDC_COUNTER_FLD, m_StrCounterFld);
	DDV_MaxChars(pDX, m_StrCounterFld, 7);
	DDX_Text(pDX, IDC_END_DATE_FLD, m_StrEndDateFld);
	DDV_MaxChars(pDX, m_StrEndDateFld, 8);
	DDX_Text(pDX, IDC_MINIMUM_AMOUNT_FLD, m_StrMinAmountFld);
	DDV_MaxChars(pDX, m_StrMinAmountFld, 10);
	DDX_Text(pDX, IDC_RESPONSE_FLD, m_StrResponseFld);
	DDV_MaxChars(pDX, m_StrResponseFld, 40);
	DDX_Text(pDX, IDC_REWARD_FLD, m_StrRewardFld);
	DDV_MaxChars(pDX, m_StrRewardFld, 10);
	DDX_Text(pDX, IDC_START_DATE_FLD, m_StrStartDateFld);
	DDV_MaxChars(pDX, m_StrStartDateFld, 8);
	DDX_Text(pDX, IDC_THRESHOLD_FLD, m_StrThresholdFld);
	DDV_MaxChars(pDX, m_StrThresholdFld, 7);
	DDX_Text(pDX, IDC_MESSAGE_LBL, m_StrMessageLbl);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CCashBonus, CDialog)


//BEGIN_MESSAGE_MAP(CCashBonus, CPropertyPage)
	//{{AFX_MSG_MAP(CCashBonus)
	ON_BN_CLICKED(IDC_ADD_BUTTON, OnAddButton)
	ON_BN_CLICKED(IDC_CAN_BUTTON, OnCanButton)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCashBonus message handlers

BOOL CCashBonus::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	CFont m_font;
	LOGFONT lf;
	CString temp;
	CMyUtils myUtilsObject;
	char strErrorMessage[200] = "";


	memset ( &lf, 0, sizeof (lf));
	lf.lfHeight = 12;
	lf.lfWeight = 700;
	strcpy(lf.lfFaceName, "Arial");
	m_font.CreateFontIndirect (&lf);
	GetDlgItem(IDC_MESSAGE_LBL)->SetFont (&m_font);

	// Clean all strings
	m_StrCounterFld = _T("");
	m_StrEndDateFld = _T("");
	m_StrMinAmountFld = _T("");
	m_StrResponseFld = _T("");
	m_StrRewardFld = _T("");
	m_StrStartDateFld = _T("");
	m_StrThresholdFld = _T("");
	m_bFirstRecord = FALSE;

	int nIndex = -1;

	for ( int i = 0; i <=15; i++ )
	{
		temp = ucf01.control[i].subsystem_name;
		if ( strcmp ( temp, "Merchant Control" ) == 0)
			nIndex = i;
	}

	if ( nIndex != -1)
	{
		if ( '1' == ucf01.control[nIndex].action[0]  &&
         ( '1' == ucf01.control[nIndex].action[1]  ) &&
         ( '1' == ucf01.control[nIndex].action[2]  ) &&
         ( '1' == ucf01.control[nIndex].action[3]  ) )

		{
			m_CntlCounterFld.EnableWindow(TRUE);
			m_CntlEndDateFld.EnableWindow(TRUE);
			m_CntlMinAmountFld.EnableWindow(TRUE);
			m_CntlResponseFld.EnableWindow(TRUE);
			m_CntlRewardFld.EnableWindow(TRUE);
			m_CntlStartDateFld.EnableWindow(TRUE);
			m_CntlThresholdFld.EnableWindow(TRUE);
			m_AddButton.EnableWindow(TRUE);

		}
		else
		{
			m_CntlCounterFld.EnableWindow(FALSE);
			m_CntlEndDateFld.EnableWindow(FALSE);
			m_CntlMinAmountFld.EnableWindow(FALSE);
			m_CntlResponseFld.EnableWindow(FALSE);
			m_CntlRewardFld.EnableWindow(FALSE);
			m_CntlStartDateFld.EnableWindow(FALSE);
			m_CntlThresholdFld.EnableWindow(FALSE);
			m_AddButton.EnableWindow(FALSE);

		}

	}

	// Populate screen from database
	ZeroMemory ( &sBONRecordOld, sizeof (BON01));
	memcpy( sBONRecordOld.minimum_amount, " ", sizeof (sBONRecordOld.minimum_amount));
	memcpy( sBONRecordOld.program_start_date, " ", sizeof (sBONRecordOld.program_start_date));
	memcpy( sBONRecordOld.program_end_date, " ", sizeof (sBONRecordOld.program_end_date));
	memcpy( sBONRecordOld.transaction_threshold, " ", sizeof (sBONRecordOld.transaction_threshold));
	memcpy( sBONRecordOld.transaction_counter, " ", sizeof (sBONRecordOld.transaction_counter));
	memcpy( sBONRecordOld.maximum_reward, " ", sizeof (sBONRecordOld.maximum_reward));
	memcpy( sBONRecordOld.response_message, " ", sizeof (sBONRecordOld.response_message));

	int rcode = txdsapi_get_record ( (pBYTE)&sBONRecordOld, 
											 sizeof (BON01), BON01_DATA, strErrorMessage );
	if ( PTEMSG_OK != rcode && PTEMSG_NOT_FOUND != rcode)
	{  	
		m_StrMessageLbl = strErrorMessage;
		if (!UpdateData(FALSE)){
			AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
		}
		return FALSE;
	}
	else if ( PTEMSG_OK == rcode )
	{
		char yyyymmdd[9];
		CString strAmount;
		LONG julian = atol((LPCTSTR)sBONRecordOld.program_start_date);
		Txutils_Calculate_Gregorian_Date(julian, yyyymmdd);
		m_StrStartDateFld = yyyymmdd;
		m_StrStartDateFld = m_StrStartDateFld.Right(2) + m_StrStartDateFld.Mid(4,2) + m_StrStartDateFld.Left(4);


		julian = atol((LPCTSTR)sBONRecordOld.program_end_date);
		Txutils_Calculate_Gregorian_Date(julian, yyyymmdd);
		m_StrEndDateFld = yyyymmdd;
		m_StrEndDateFld = m_StrEndDateFld.Right(2) + m_StrEndDateFld.Mid(4,2) + m_StrEndDateFld.Left(4);

		strAmount = sBONRecordOld.minimum_amount;
		m_StrMinAmountFld = myUtilsObject.GetAmount ( strAmount );
		strAmount = sBONRecordOld.maximum_reward;
		m_StrRewardFld = myUtilsObject.GetAmount ( strAmount );
		m_StrThresholdFld = sBONRecordOld.transaction_threshold;
		m_StrCounterFld = sBONRecordOld.transaction_counter;
		m_StrResponseFld = sBONRecordOld.response_message;
	}


	if (myUtilsObject.IsEmpty (m_StrMinAmountFld, this))
	{
		m_bFirstRecord = TRUE;
	}


	UpdateData(FALSE);	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CCashBonus::CheckDialogForCompleteness()
{
    CMyUtils myUtilsObject;

    if (!UpdateData(TRUE)){
		AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
	}

	if (myUtilsObject.IsEmpty (m_StrMinAmountFld, this))
    {
		m_StrMessageLbl = "Please enter Minimum Amount";	
		m_CntlMinAmountFld.SetFocus();
		if (!UpdateData(FALSE)){
			AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
		}
		m_nDialogMode = MODE_DONE;
		return FALSE;
	}

	if (myUtilsObject.IsEmpty (m_StrStartDateFld, this))
    {
		m_StrMessageLbl = "Please enter Start Date";	
		m_CntlStartDateFld.SetFocus();
		if (!UpdateData(FALSE)){
			AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
		}
		m_nDialogMode = MODE_DONE;
		return FALSE;
	}

	if (myUtilsObject.IsEmpty (m_StrEndDateFld, this))
    {
		m_StrMessageLbl = "Please enter End Date";	
		m_CntlEndDateFld.SetFocus();
		if (!UpdateData(FALSE)){
			AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
		}
		m_nDialogMode = MODE_DONE;
		return FALSE;
	}

	if (myUtilsObject.IsEmpty (m_StrThresholdFld, this))
    {
		m_StrMessageLbl = "Please enter Transaction Threshold";	
		m_CntlThresholdFld.SetFocus();
		if (!UpdateData(FALSE)){
			AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
		}
		m_nDialogMode = MODE_DONE;
		return FALSE;
	}

	if (myUtilsObject.IsEmpty (m_StrCounterFld, this))
    {
		m_StrMessageLbl = "Please enter Transaction Counter";	
		m_CntlCounterFld.SetFocus();
		if (!UpdateData(FALSE)){
			AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
		}
		m_nDialogMode = MODE_DONE;
		return FALSE;
	}

	if (myUtilsObject.IsEmpty (m_StrRewardFld, this))
    {
		m_StrMessageLbl = "Please enter Maximum Reward";	
		m_CntlRewardFld.SetFocus();
		if (!UpdateData(FALSE)){
			AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
		}
		m_nDialogMode = MODE_DONE;
		return FALSE;
	}

	if (myUtilsObject.IsEmpty (m_StrResponseFld, this))
    {
		m_StrMessageLbl = "Please enter Response Message";	
		m_CntlResponseFld.SetFocus();
		if (!UpdateData(FALSE)){
			AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
		}
		m_nDialogMode = MODE_DONE;
		return FALSE;
	}

	
    return TRUE;
}



/******************************************************************************
 
  MODULE:	   ValidateDate
 
  DESCRIPTION: Validates format, number of days, 
                valid day, month, year, 
                leap year.

  INPUTS:      None             
 
  OUTPUTS:     None
 
  RTRN VALUE:  None
 
  AUTHOR:      Irina Arustamova
 
  MODIFICATIONS:
 
 ******************************************************************************/
BOOL CCashBonus::ValidateDate ( CString strDate )
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
		m_StrMessageLbl = "ERROR: Invalid date format";
		UpdateData(FALSE);
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
		m_StrMessageLbl = "ERROR: Invalid data";
		UpdateData(FALSE);
		return FALSE;
	}


	if ( nMonth > 12 )
	{
		m_StrMessageLbl = "ERROR: Invalid month";
		UpdateData(FALSE);
		return FALSE;
	}

	if ( nMonth == 1 || nMonth == 3 || nMonth == 5 || 
		 nMonth == 7 || nMonth == 8 || nMonth == 10 || nMonth == 12 )
	{
		if ( nDay > 31 )
		{
			m_StrMessageLbl = "ERROR: Invalid day";
			UpdateData(FALSE);
			return FALSE;
		}
	}

	if ( nMonth == 4 || nMonth == 6 || nMonth == 9 || 
		 nMonth == 11 )
	{
		if ( nDay > 30 )
		{
			m_StrMessageLbl = "ERROR: Invalid day";
			UpdateData(FALSE);
			return FALSE;
		}
	}

	if ( nMonth ==2 )
	{
		int res = nYear % 4;
		if ( res != 0 && nDay >28)
		{
			m_StrMessageLbl = "ERROR: Invalid day";
			UpdateData(FALSE);
			return FALSE;
		}
		if (res == 0 && nDay >29)
		{
			m_StrMessageLbl = "ERROR: Invalid day";
			UpdateData(FALSE);
			return FALSE;
		}
	}
	
	return TRUE;
}



void CCashBonus::OnAddButton() 
{
	BON01 sBONRecord;
	char strErrorMessage[200] = "";
	CString temp;

	//char strStartFld[10], strEndFld[10];

	if (!CheckDialogForCompleteness())
		return ;

	memset(&sBONRecord, 0, sizeof(BON01)) ;


/*	memcpy( sBONRecord.minimum_amount,			sBONRecordOld.minimum_amount, sizeof (sBONRecord.minimum_amount));
	memcpy( sBONRecord.program_start_date,		sBONRecordOld.program_start_date, sizeof (sBONRecord.program_start_date));
	memcpy( sBONRecord.program_end_date,		sBONRecordOld.program_end_date,   sizeof (sBONRecord.program_end_date));
	memcpy( sBONRecord.transaction_threshold,	sBONRecordOld.transaction_threshold, sizeof (sBONRecord.transaction_threshold));
	memcpy( sBONRecord.transaction_counter,		sBONRecordOld.transaction_counter,   sizeof (sBONRecord.transaction_counter));
	memcpy( sBONRecord.maximum_reward,			sBONRecordOld.maximum_reward,    sizeof (sBONRecord.maximum_reward));
	memcpy( sBONRecord.response_message,		sBONRecordOld.response_message,  sizeof (sBONRecord.response_message));
*/
	
	if ( !ValidateDate ( m_StrStartDateFld ) )
	{
		m_CntlStartDateFld.SetFocus();
		return;
	}

	if ( !ValidateDate ( m_StrEndDateFld ) )
	{
		m_CntlEndDateFld.SetFocus();
		return;
	}

	if ( m_StrStartDateFld.Right(4) > m_StrEndDateFld.Right(4) )
	{
		m_CntlStartDateFld.SetFocus();
		m_StrMessageLbl = "ERROR: Program Start Day cannot be greater than End Day";
		UpdateData(FALSE);
		return;
	}

	if ( m_StrStartDateFld.Right(4) == m_StrEndDateFld.Right(4) && 
		m_StrEndDateFld.Mid(2,2) <  m_StrStartDateFld.Mid(2,2) )
	{
		m_CntlStartDateFld.SetFocus();
		m_StrMessageLbl = "ERROR: Program Start Day cannot be greater than End Day";
		UpdateData(FALSE);
		return;
	}

	if ( m_StrStartDateFld.Right(4) == m_StrEndDateFld.Right(4) && 
		 m_StrEndDateFld.Mid(2,2) ==  m_StrStartDateFld.Mid(2,2) && 
		 m_StrEndDateFld.Left(2) < m_StrStartDateFld.Left(2) )
	{
		m_CntlStartDateFld.SetFocus();
		m_StrMessageLbl = "ERROR: Program Start Day cannot be greater than End Day";
		UpdateData(FALSE);
		return;
	}
	if ( !m_bFirstRecord )
	{
	int rcode = txdsapi_delete_record ( (pBYTE)&sBONRecordOld, 
											 sizeof (BON01), BON01_DATA, strErrorMessage );
		if (PTEMSG_OK != rcode )
		{  	
			m_StrMessageLbl = strErrorMessage;
			if (!UpdateData(FALSE)){
				AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
			}
			return ;
		}
	}
// Inserts new record
	char strDate[10];
	CString strYYYYMMDD;
	LONG nJulianDate;
	CString strAmount;
	CMyUtils myUtilsObject;


	strYYYYMMDD = m_StrStartDateFld.Right(4) + m_StrStartDateFld.Mid(2,2) + m_StrStartDateFld.Left(2);
	memcpy ( strDate, strYYYYMMDD, sizeof (strDate));
	nJulianDate = Txutils_Calculate_Julian_Date ((BYTE *)strDate) ;
	ltoa ( nJulianDate, strDate,  10 );

	memcpy( sBONRecord.program_start_date, strDate, sizeof (sBONRecord.program_start_date));

	strYYYYMMDD = m_StrEndDateFld.Right(4) + m_StrEndDateFld.Mid(2,2) + m_StrEndDateFld.Left(2);
	memcpy ( strDate, strYYYYMMDD, sizeof (strDate));
	nJulianDate = Txutils_Calculate_Julian_Date ((BYTE *)strDate) ;
	ltoa ( nJulianDate, strDate,  10 );

	strAmount = myUtilsObject.GenerateAmount( m_StrMinAmountFld, 12 );

	memcpy( sBONRecord.minimum_amount,			strAmount, sizeof (sBONRecord.minimum_amount));

	strAmount = myUtilsObject.GenerateAmount( m_StrRewardFld, 12 );

	memcpy( sBONRecord.maximum_reward,			strAmount,    sizeof (sBONRecord.maximum_reward));

	memcpy( sBONRecord.program_end_date,		strDate,   sizeof (sBONRecord.program_end_date));
	memcpy( sBONRecord.transaction_threshold,	m_StrThresholdFld, sizeof (sBONRecord.transaction_threshold));
	memcpy( sBONRecord.transaction_counter,		m_StrCounterFld,   sizeof (sBONRecord.transaction_counter));
	memcpy( sBONRecord.response_message,		m_StrResponseFld,  sizeof (sBONRecord.response_message));

	int rcode = txdsapi_insert_record ( (pBYTE)&sBONRecord, 
											 sizeof (BON01), BON01_DATA, strErrorMessage );
	if ( PTEMSG_OK != rcode )
	{  	
		m_StrMessageLbl = strErrorMessage;
		if (!UpdateData(FALSE)){
			AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
		}
		return ;
	}
	else 
	{    
        if ( m_bFirstRecord )
			m_StrMessageLbl = "Record Added";
		else
			m_StrMessageLbl = "Record Updated";	
		if (!UpdateData(FALSE)){
			AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
		}
		m_bFirstRecord = FALSE;
	}

}


void CCashBonus::OnCancel() 
{	
	CDialog::OnCancel();
}

void CCashBonus::OnCanButton() 
{
    OnCancel();	
}


BOOL CCashBonus::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	// TODO: Add your specialized code here and/or call the base class
		c_time = CTime::GetCurrentTime();	
	shrmem.SetLastUpdateTime(c_time);
	return CDialog::OnCommand(wParam, lParam);
}
