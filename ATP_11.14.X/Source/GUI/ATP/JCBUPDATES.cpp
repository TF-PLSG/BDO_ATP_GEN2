// JCBUPDATES.cpp : implementation file
//
/*
Rev        April 28.4.11          Krishna Kiran S@TF
	 Added Idle Time Calculation & calling Login screen on Idle Timeout
*/

#include "stdafx.h"
#include "Atp.h"
#include "JCBUPDATES.h"
#include "MyUtils.h"
#include "SharedMem.h"
//#include "Login.h"
extern "C"
{
   #include "ntutils.h"
}

CMyUtils myUtilsObject;
BOOL response_pending = FALSE;

extern UCF01 ucf01;
extern CString workstation;
//extern CLogin login;
extern CSharedMem shrmem;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CJCBUPDATES dialog


CJCBUPDATES::CJCBUPDATES(CWnd* pParent /*=NULL*/)
	: CDialog(CJCBUPDATES::IDD, pParent)
{
	//{{AFX_DATA_INIT(CJCBUPDATES)
	m_Asia = FALSE;
	m_CkDigit = FALSE;
	m_Europe = FALSE;
	m_Japan = FALSE;
	m_Local = FALSE;
	m_strMessageLbl = _T("");
	m_USA = FALSE;
	m_strRespCode = _T("");
	m_strPurgeDate = _T("");
	m_strRespText = _T("");
	m_StopRegions = FALSE;
	m_strRequestType = _T("");
	m_strActionCode = _T("");
	m_strInquiryResults = _T("");
	m_strAccountNbr = _T("");
	//}}AFX_DATA_INIT
}


void CJCBUPDATES::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CJCBUPDATES)
	DDX_Control(pDX, IDC_INQUIRY_RESULTS, m_cntlInquiryResults);
	DDX_Control(pDX, IDC_EXIT_BUTTON, m_cntlExitButton);
	DDX_Control(pDX, IDC_SUBMIT_BUTTON, m_cntlSubmitButton);
	DDX_Control(pDX, IDC_ACCOUNT_NUMBER_FLD, m_cntlAccountNbr);
	DDX_Control(pDX, IDC_ACTION_CODE, m_cntlActionCode);
	DDX_Control(pDX, IDC_REQUEST_TYPE, m_cntlRequestType);
	DDX_Control(pDX, IDC_STOP_REGIONS, m_cntlStopRegions);
	DDX_Control(pDX, IDC_RESP_TEXT, m_cntlRespText);
	DDX_Control(pDX, IDC_PURGE_DATE_FLD, m_cntlPurgeDate);
	DDX_Control(pDX, IDC_RESP_CODE, m_cntlRespCode);
	DDX_Control(pDX, IDC_USA, m_cntlUSA);
	DDX_Control(pDX, IDC_LOCAL, m_cntlLocal);
	DDX_Control(pDX, IDC_JAPAN, m_cntlJapan);
	DDX_Control(pDX, IDC_EUROPE, m_cntlEurope);
	DDX_Control(pDX, IDC_CKDIGIT, m_cntlCkDigit);
	DDX_Control(pDX, IDC_ASIA, m_cntlAsia);
	DDX_Check(pDX, IDC_ASIA, m_Asia);
	DDX_Check(pDX, IDC_CKDIGIT, m_CkDigit);
	DDX_Check(pDX, IDC_EUROPE, m_Europe);
	DDX_Check(pDX, IDC_JAPAN, m_Japan);
	DDX_Check(pDX, IDC_LOCAL, m_Local);
	DDX_Text(pDX, IDC_MESSAGE_LBL, m_strMessageLbl);
	DDX_Check(pDX, IDC_USA, m_USA);
	DDX_Text(pDX, IDC_RESP_CODE, m_strRespCode);
	DDX_Text(pDX, IDC_PURGE_DATE_FLD, m_strPurgeDate);
	DDV_MaxChars(pDX, m_strPurgeDate, 6);
	DDX_Text(pDX, IDC_RESP_TEXT, m_strRespText);
	DDX_Check(pDX, IDC_STOP_REGIONS, m_StopRegions);
	DDX_CBString(pDX, IDC_REQUEST_TYPE, m_strRequestType);
	DDX_CBString(pDX, IDC_ACTION_CODE, m_strActionCode);
	DDX_Text(pDX, IDC_INQUIRY_RESULTS, m_strInquiryResults);
	DDX_Text(pDX, IDC_ACCOUNT_NUMBER_FLD, m_strAccountNbr);
	DDV_MaxChars(pDX, m_strAccountNbr, 19);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CJCBUPDATES, CDialog)
	//{{AFX_MSG_MAP(CJCBUPDATES)
	ON_BN_CLICKED(IDC_SUBMIT_BUTTON, OnSubmitButton)
	ON_BN_CLICKED(IDC_EXIT_BUTTON, OnExitButton)
	ON_EN_KILLFOCUS(IDC_ACCOUNT_NUMBER_FLD, OnKillfocusAccountNumberFld)
	ON_CBN_KILLFOCUS(IDC_REQUEST_TYPE, OnKillfocusRequestType)
	ON_BN_CLICKED(IDC_STOP_REGIONS, OnStopRegions)
	ON_CBN_KILLFOCUS(IDC_ACTION_CODE, OnKillfocusActionCode)
	ON_EN_KILLFOCUS(IDC_PURGE_DATE_FLD, OnKillfocusPurgeDateFld)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CJCBUPDATES message handlers

BOOL CJCBUPDATES::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	CFont m_font;
	LOGFONT lf;

	memset ( &lf, 0, sizeof (lf));
	lf.lfHeight = 12;
	lf.lfWeight = 700;
	strcpy(lf.lfFaceName, "Arial");
	m_font.CreateFontIndirect (&lf);
	GetDlgItem(IDC_MESSAGE_LBL)->SetFont (&m_font);

	//set version info in the title bar
	CString appProdVer;
	CString lpszTitle;
	appProdVer.LoadString(IDS_PRODUCT_VERSION);
	this->GetWindowText(lpszTitle);
	lpszTitle = lpszTitle + " - Version - ";
	lpszTitle += appProdVer;
	this->SetWindowText(lpszTitle);

	CString temp;
	int nIndex = -1;

/*	timer_handle = StartTimer(1000);
	if (timer_handle == 0)
    {
          AfxMessageBox("Unable to obtain timer");
    }
*/
	for ( int i = 0; i <=15; i++ )
	{
		temp = ucf01.control[i].subsystem_name;
		if ( strcmp ( temp, "Card Account Control" ) == 0)
			nIndex = i;
	}

	if ( (nIndex != -1) &&
		 ( '1' == ucf01.control[nIndex].action[0] ) ||		/*add or */
         ( '1' == ucf01.control[nIndex].action[2] ) )  	    /*update */
	{
			/* Enable entry Fields*/
		m_cntlAccountNbr.EnableWindow(TRUE);
		m_cntlRequestType.EnableWindow(TRUE);
		m_cntlActionCode.EnableWindow(TRUE);
		m_cntlPurgeDate.EnableWindow(TRUE);
		m_cntlCkDigit.EnableWindow(TRUE);
		m_CkDigit = TRUE;
		m_cntlStopRegions.EnableWindow(TRUE);
		m_cntlJapan.EnableWindow(TRUE);
		m_cntlAsia.EnableWindow(TRUE);
		m_cntlUSA.EnableWindow(TRUE);
		m_cntlEurope.EnableWindow(TRUE);
		m_cntlLocal.EnableWindow(TRUE);
		m_cntlInquiryResults.EnableWindow(FALSE);
			/*Enable buttons*/
		m_cntlSubmitButton.EnableWindow(TRUE);
		m_cntlExitButton.EnableWindow(TRUE);

	}
	else
	{		/*Entry Fields Disabled*/
		m_cntlAccountNbr.EnableWindow(FALSE);
		m_cntlRequestType.EnableWindow(FALSE);
		m_cntlActionCode.EnableWindow(FALSE);
		m_cntlPurgeDate.EnableWindow(FALSE);
		m_cntlCkDigit.EnableWindow(FALSE);
		m_CkDigit = FALSE;
		m_cntlStopRegions.EnableWindow(FALSE);
		m_cntlJapan.EnableWindow(FALSE);
		m_cntlAsia.EnableWindow(FALSE);
		m_cntlUSA.EnableWindow(FALSE);
		m_cntlEurope.EnableWindow(FALSE);
		m_cntlLocal.EnableWindow(FALSE);
		m_cntlInquiryResults.EnableWindow(FALSE);
			/*Enable Buttons*/
		m_cntlSubmitButton.EnableWindow(FALSE);
		m_cntlExitButton.EnableWindow(TRUE);
	}
		
	
	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CJCBUPDATES::OnSubmitButton() 
{
	char strErrorMessage[200];
	char line1[50];
	char line2[50];
	char line3[50];
   char tempstr[20]="";
   char purge_year[5];
   char purge_month[3];
   int  action_code;
   int  prev_region;

	if (!CheckDialogForCompleteness())
		return;

	if ( !PopulateDatabaseStructure())
		return;

	m_strMessageLbl = "Please Wait ... Waiting for Response";

	m_strRespCode   = "";
	m_strRespText   = "";
    m_cntlInquiryResults.ResetContent();//clear list box

	UpdateData(FALSE);

    response_pending = TRUE;
    BeginWaitCursor();
	int rcode = txdsapi_send_jcb_record( &sJCBRecord, 0, strErrorMessage );
    EndWaitCursor();
	if( PTEMSG_OK != rcode )
	{
		m_strMessageLbl = strErrorMessage;
		UpdateData(FALSE);
		response_pending = FALSE;
		return;
	}

	m_strMessageLbl = "Transaction Response Received";
	m_strRespCode =    sJCBRecord.resp_code;
	m_strRespText =    sJCBRecord.resp_text;
    m_cntlInquiryResults.ResetContent();//clear list box

	if(m_nDialogMode == MODE_INQUIRE)
	{
		m_cntlInquiryResults.ResetContent();//clear list box

      if ( 0 == strcmp(m_strRespCode, "00") )
      {
         /* Action Code */
         action_code = atoi((const char *)sJCBRecord.action_code);
	   	sprintf(line1, "Action Code:\t%s : ", sJCBRecord.action_code);
         switch(action_code)
         {
            case  1: strcat( line1, "Refer to Issuer " ); break;
            case  4: strcat( line1, "Pick Up         " ); break;
            case  5: strcat( line1, "Decline         " ); break;
            case  7: strcat( line1, "Pick Up - Fraud " ); break;
            case 41: strcat( line1, "Pick Up - Lost  " ); break;
            case 43: strcat( line1, "Pick Up - Stolen" ); break;
            default: strcat( line1, "None            " ); break;
         }

         /* Purge Date */
         if ( sJCBRecord.purge_date[0] == 0x00 )
         {
            strcpy( line2, "Purge Date: \tNone" );
         }
         else
         {
            memset( purge_year,  0x00, sizeof(purge_year)  );
            memset( purge_month, 0x00, sizeof(purge_month) );

            memcpy( purge_year,  sJCBRecord.purge_date,   4 );
            memcpy( purge_month, sJCBRecord.purge_date+4, 2 );

            sprintf(line2, "Purge Date: \t%s/%s", purge_year, purge_month );
         }

         /* Stop Regions */
         if ( (sJCBRecord.stop_regions[0] == 0x00) ||
              (0 == strcmp( (const char *)sJCBRecord.stop_regions,"00000")) )
         {
            strcpy( line3, "Stop Regions: \tNo Stop Regions" );
         }
         else
         {
            prev_region = false;
            strcpy( line3, "Stop Regions: \t" );

            /* Japan */
            if ( sJCBRecord.stop_regions[0] == '1' )
            {
               strcat( line3, "Japan" );
               prev_region = true;
            }

            /* Asia */
            if ( sJCBRecord.stop_regions[1] == '1' )
            {
               if ( prev_region == true )
                  strcat( line3, ",Asia" );
               else
               {
                  prev_region = true;
                  strcat( line3, "Asia" ); 
               }
            }

            /* USA */
            if ( sJCBRecord.stop_regions[2] == '1' )
            {
               if ( prev_region == true )
                  strcat( line3, ",USA" );
               else
               {
                  prev_region = true;
                  strcat( line3, "USA" ); 
               }
            }

            /* Europe */
            if ( sJCBRecord.stop_regions[3] == '1' )
            {
               if ( prev_region == true )
                  strcat( line3, ",Europe" );
               else
               {
                  prev_region = true;
                  strcat( line3, "Europe" ); 
               }
            }

            /* Local */
            if ( sJCBRecord.stop_regions[4] == '1' )
            {
               if ( prev_region == true )
                  strcat( line3, ",Local" );
               else
               {
                  strcat( line3, "Local" ); 
               }
            }
         }

         /* Paste the results into the InquiryResults field. */
         m_cntlInquiryResults.AddString(line1);
         m_cntlInquiryResults.AddString(line2);
         m_cntlInquiryResults.AddString(line3);
      }
	}
	
	UpdateData(FALSE);

	response_pending = FALSE;
	
}

void CJCBUPDATES::OnExitButton() 
{
	char strErrorMessage[200];
	int ret_code = 0;

//	StopTimer();

	if(response_pending) //has pressed exit while response is pending
	{
		// confirm exit
		strcpy (strErrorMessage, "Waiting for a response. Are you sure you want exit?") ;

		ret_code = AfxMessageBox (strErrorMessage, MB_OKCANCEL|MB_ICONQUESTION) ;
		if (ret_code != IDOK) 
			return ;

	}
	
	CDialog::OnCancel();
}

BOOL CJCBUPDATES::CheckDialogForCompleteness()
{
	char CardNbr[20];
	int date_len = 0;
	int month = 0;
	int verify_digit = 0;
	int length = 0;

	UpdateData(TRUE);

	if ( myUtilsObject.IsEmpty (m_strAccountNbr, this ) )
	{
		m_strMessageLbl = "Account Number is required.";
		UpdateData(FALSE);
		m_cntlAccountNbr.SetFocus();
		return FALSE;
	}

	if (m_CkDigit)  //verify account number if check digit checked
	{
		length = m_strAccountNbr.GetLength();
		lstrcpy ((LPTSTR) CardNbr,m_strAccountNbr.Left(length - 1));	
		verify_digit = myUtilsObject.GetMod10CheckDigit( CardNbr, length - 1 );
		if (atoi(m_strAccountNbr.Right(1)) != verify_digit)
		{
			m_strMessageLbl = "Invalid Account Number - check digit failed.";
			UpdateData(FALSE);
			m_cntlAccountNbr.SetFocus();
			return FALSE;
		}

	}

	if ( myUtilsObject.IsEmpty (m_strRequestType, this )) 
	{
		m_strMessageLbl = "Request Type is required.";
		UpdateData(FALSE);
		m_cntlRequestType.SetFocus();
		return FALSE;
	}
	else
	{
		if(strcmp(m_strRequestType, "Add") == 0)
			m_nDialogMode = MODE_ADD;
		else
			if(strcmp(m_strRequestType, "Revise") == 0)
			m_nDialogMode = MODE_EDIT;
			else
				if(strcmp(m_strRequestType, "Delete") == 0)
					m_nDialogMode = MODE_DELETE;
				else
					if(strcmp(m_strRequestType, "Inquire") == 0)
					m_nDialogMode = MODE_INQUIRE;
	}

	if ( myUtilsObject.IsEmpty (m_strActionCode, this )
			&& (m_nDialogMode == MODE_ADD || m_nDialogMode == MODE_EDIT) )
	{
		m_strMessageLbl = "Action Code is required.";
		UpdateData(FALSE);
		m_cntlActionCode.SetFocus();
		return FALSE;
	}

	if ( myUtilsObject.IsEmpty (m_strPurgeDate, this ) 
				&& (m_nDialogMode == MODE_ADD || m_nDialogMode == MODE_EDIT) )
	{
		m_strMessageLbl = "Purge Date is required.(YYYYMM)";
		UpdateData(FALSE);
		m_cntlPurgeDate.SetFocus();
		return FALSE;
	}

	date_len = m_strPurgeDate.GetLength();//validate month if date exists
	if (date_len > 0)
	{
		month = atoi(m_strPurgeDate.Right(2));
		if (date_len < 6 || month < 1 || month > 12)
		{
			m_strMessageLbl = "Invalid Purge Date. (YYYYMM)";
			UpdateData(FALSE);
			m_cntlPurgeDate.SetFocus();
			return FALSE;
		}
	}


   return TRUE;

}

void CJCBUPDATES::OnKillfocusAccountNumberFld() 
{
/*
	UpdateData(TRUE);

	if (m_cntlExitButton.GetCheck())
		return;

	m_strMessageLbl = "";
	UpdateData(FALSE);

	if ( myUtilsObject.IsEmpty (m_strAccountNbr, this ) )
	{
		m_strMessageLbl = "Account Number is required.";
		UpdateData(FALSE);
		m_cntlAccountNbr.SetFocus();
		return;
	}
*/	
}

void CJCBUPDATES::OnKillfocusRequestType() 
{
/*	if (m_cntlExitButton.GetCheck())
		return;

	UpdateData(TRUE);

	m_strMessageLbl = "";
	UpdateData(FALSE);

	if ( myUtilsObject.IsEmpty (m_strRequestType, this )) 
	{
		m_strMessageLbl = "Request Type is required.";
		UpdateData(FALSE);
		m_cntlRequestType.SetFocus();
		return;
	}
	*/

   /* Disable Purge Date, Action Code, and Stop Regions for Inquires. */
	UpdateData(TRUE);

   m_Asia   = FALSE;
   m_Europe = FALSE;
   m_Japan  = FALSE;
   m_Local  = FALSE;
   m_USA    = FALSE;
   UpdateData(FALSE);

   if ( m_strRequestType == "Inquire" )
   {
      /* Empty fields prior to disabling them. */
      m_strActionCode = "";
      m_strPurgeDate  = "";
		UpdateData(FALSE);

		m_cntlActionCode.EnableWindow(FALSE);
		m_cntlPurgeDate.EnableWindow(FALSE);
		m_cntlStopRegions.EnableWindow(FALSE);
		m_cntlJapan.EnableWindow(FALSE);
		m_cntlUSA.EnableWindow(FALSE);
		m_cntlEurope.EnableWindow(FALSE);
		m_cntlLocal.EnableWindow(FALSE);
		m_cntlAsia.EnableWindow(FALSE);

		m_cntlExitButton.SetFocus();
		m_cntlSubmitButton.SetFocus();
   }
   else
   {
		m_cntlActionCode.EnableWindow(TRUE);
		m_cntlPurgeDate.EnableWindow(TRUE);
		m_cntlStopRegions.EnableWindow(TRUE);
		m_cntlJapan.EnableWindow(TRUE);
		m_cntlUSA.EnableWindow(TRUE);
		m_cntlEurope.EnableWindow(TRUE);
		m_cntlLocal.EnableWindow(TRUE);
		m_cntlAsia.EnableWindow(TRUE);
		m_cntlActionCode.SetFocus();
   }
}

void CJCBUPDATES::OnStopRegions() 
{
	UpdateData(TRUE);

	if(m_StopRegions)//OnStopRegions checked
	{
		m_cntlJapan.EnableWindow(FALSE);
		m_cntlUSA.EnableWindow(FALSE);
		m_cntlEurope.EnableWindow(FALSE);
		m_cntlLocal.EnableWindow(FALSE);
		m_cntlAsia.EnableWindow(FALSE);
	}
	else
	{					//OnStopRegions unchecked
		m_cntlJapan.EnableWindow(TRUE);
		m_cntlUSA.EnableWindow(TRUE);
		m_cntlEurope.EnableWindow(TRUE);
		m_cntlLocal.EnableWindow(TRUE);
		m_cntlAsia.EnableWindow(TRUE);
	}
	
}


BOOL CJCBUPDATES::PopulateDatabaseStructure()
{
   CHAR temp[20]; 
   INT  nCount = 0;
   INT  idx = 0;
   CHAR strDeviceID[20] = "";

	UpdateData(TRUE);

	ZeroMemory ( &sJCBRecord, sizeof (JCB_ONLINE_UPDATES));

	memcpy ( &sJCBRecord.card_nbr, m_strAccountNbr, sizeof ( sJCBRecord.card_nbr) -1);

	if ( m_nDialogMode == MODE_ADD )
		strcpy (temp, "1");
	else if (  m_nDialogMode == MODE_DELETE )
		strcpy (temp, "0");
	else if (  m_nDialogMode == MODE_EDIT )
		strcpy (temp, "2");
	else if (  m_nDialogMode == MODE_INQUIRE )
		strcpy (temp, "5");

	memcpy ( &sJCBRecord.txn_type, temp, sizeof(sJCBRecord.txn_type ) );

	memset (temp, 0x00, sizeof(temp));
	strcpy (temp, m_strActionCode.Left(2));
	memcpy ( &sJCBRecord.action_code, temp, sizeof(sJCBRecord.action_code ) -1);

	memcpy ( sJCBRecord.purge_date, m_strPurgeDate, sizeof(sJCBRecord.purge_date ) -1);

	if(!m_StopRegions)//if 'No Stop Regions' is checked, fields will be null
	{
      if ( m_strRequestType != "Inquire" )
      {
         if(m_Japan)
            memcpy(&sJCBRecord.stop_regions[0], "1", 1);
         else
            memcpy(&sJCBRecord.stop_regions[0], "0", 1);

         if(m_Asia)
            memcpy(&sJCBRecord.stop_regions[1], "1", 1);
         else
            memcpy(&sJCBRecord.stop_regions[1], "0", 1);

         if(m_USA)
            memcpy(&sJCBRecord.stop_regions[2], "1", 1);
         else
            memcpy(&sJCBRecord.stop_regions[2], "0", 1);

         if(m_Europe)
            memcpy(&sJCBRecord.stop_regions[3], "1", 1);
         else
            memcpy(&sJCBRecord.stop_regions[3], "0", 1);

         if(m_Local)
            memcpy(&sJCBRecord.stop_regions[4], "1", 1);
         else
            memcpy(&sJCBRecord.stop_regions[4], "0", 1);
      }
	}

   /* Get Terminal ID from TF.INI file. */
   GetPrivateField1(strDeviceID);
   memcpy( sJCBRecord.terminal_id, strDeviceID, 8 );

   /* Get Workstation and Operator ID for logging to TLF01. */
   memcpy( &sJCBRecord.workstation, workstation, 20 );
   memcpy( &sJCBRecord.operator_id, ucf01.primary_key.name, 20 );

	return TRUE;
}

void CJCBUPDATES::OnKillfocusActionCode() 
{
/*
	if (m_cntlExitButton.GetCheck())
		return;

	if ( myUtilsObject.IsEmpty (m_strActionCode, this )
			&& (m_nDialogMode == MODE_ADD || m_nDialogMode == MODE_EDIT) )
	{
		m_strMessageLbl = "Action Code is required.";
		UpdateData(FALSE);
		m_cntlActionCode.SetFocus();
		return;
	}
	*/
}

void CJCBUPDATES::OnKillfocusPurgeDateFld() 
{
/*	if (m_cntlExitButton.GetCheck())
		return;

	if ( myUtilsObject.IsEmpty (m_strPurgeDate, this ) 
				&& (m_nDialogMode == MODE_ADD || m_nDialogMode == MODE_EDIT) )
	{
		m_strMessageLbl = "Purge Date is required.(YYYYMM)";
		UpdateData(FALSE);
		m_cntlPurgeDate.SetFocus();
		return;
	}*/
}


BOOL CJCBUPDATES::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	// TODO: Add your specialized code here and/or call the base class
	c_time = CTime::GetCurrentTime();	
	shrmem.SetLastUpdateTime(c_time);
	return CDialog::OnCommand(wParam, lParam);
}
