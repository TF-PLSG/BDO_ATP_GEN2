


/*

 Rev        April 19.5.11          Krishna Kiran S@TF
	 Added Reseting Idle time, whenever any event is fired in GUI
*/

// AMEXUPDATES1.cpp : implementation file
//

#include "stdafx.h"
#include "Atp.h"
#include "AMEXUPDATES1.h"
#include "MyUtils.h"
#include "Login.h"
#include "MainFrm.h"
#include "SharedMem.h"

extern "C"
{
   #include "ntutils.h"
}


extern UCF01 ucf01;

extern CString workstation;


BOOL responsePending = FALSE;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


extern CSharedMem shrmem;

/////////////////////////////////////////////////////////////////////////////
// CAMEXUPDATES dialog


CAMEXUPDATES::CAMEXUPDATES(CWnd* pParent /*=NULL*/)
	: CDialog(CAMEXUPDATES::IDD, pParent)
{
	//{{AFX_DATA_INIT(CAMEXUPDATES)
	m_strDestQue = _T("");
	//}}AFX_DATA_INIT
}

CAMEXUPDATES ::~CAMEXUPDATES()
{

}

void CAMEXUPDATES::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAMEXUPDATES)
	DDX_Control(pDX, IDC_DEST_QUE, m_cntlDestQue);
	DDX_Control(pDX, IDC_ACCOUNT_NUMBER_FLD, m_cntlAcctNbr);
	DDX_Control(pDX, IDC_TRAN_INFO, m_cntlTranInfo);
	DDX_Control(pDX, IDC_RESP_TEXT, m_cntlResponseText);
	DDX_Control(pDX, IDC_RESP_CODE, m_cntlResponseCode);
	DDX_Control(pDX, IDC_MESSAGE_LBL, m_cntlMessageLbl);
	DDX_Control(pDX, IDC_EXIT_BUTTON, m_cntlExitButton);
	DDX_Control(pDX, IDC_SUBMIT_BUTTON, m_cntlSubmitButton);
	DDX_Control(pDX, IDC_EXPIRE_DATE_FLD, m_cntlExpireDate);
	DDX_Control(pDX, IDC_REQUEST_TYPE, m_cntlRequestType);
	DDX_Control(pDX, IDC_ACTION_CODE, m_cntlActionCode);
	DDX_Control(pDX, IDC_CKDIGIT, m_cntlCkDigit);
	DDX_Text(pDX, IDC_ACCOUNT_NUMBER_FLD, m_strAcctNbr);
	DDV_MaxChars(pDX, m_strAcctNbr, 19);
	DDX_Check(pDX, IDC_CKDIGIT, m_CkDigit);
	DDX_CBString(pDX, IDC_ACTION_CODE, m_strActionCode);
	DDX_CBString(pDX, IDC_REQUEST_TYPE, m_strRequestType);
	DDX_Text(pDX, IDC_EXPIRE_DATE_FLD, m_strExpireDate);
	DDV_MaxChars(pDX, m_strExpireDate, 4);
	DDX_Text(pDX, IDC_MESSAGE_LBL, m_strMessageLbl);
	DDX_Text(pDX, IDC_RESP_CODE, m_strResponseCode);
	DDX_Text(pDX, IDC_RESP_TEXT, m_strResponseText);
	DDX_LBString(pDX, IDC_TRAN_INFO, m_strTranInfo);	
	DDX_Text(pDX, IDC_DEST_QUE, m_strDestQue);
	DDV_MaxChars(pDX, m_strDestQue, 6);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CAMEXUPDATES, CDialog)
	//{{AFX_MSG_MAP(CAMEXUPDATES)
	ON_BN_CLICKED(IDC_EXIT_BUTTON, OnExitButton)
	ON_EN_KILLFOCUS(IDC_ACCOUNT_NUMBER_FLD, OnKillfocusAccountNumberFld)
	ON_CBN_KILLFOCUS(IDC_ACTION_CODE, OnKillfocusActionCode)
	ON_EN_KILLFOCUS(IDC_EXPIRE_DATE_FLD, OnKillfocusExpireDateFld)
	ON_CBN_KILLFOCUS(IDC_REQUEST_TYPE, OnKillfocusRequestType)
	ON_BN_CLICKED(IDC_SUBMIT_BUTTON, OnSubmitButton)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAMEXUPDATES message handlers

void CAMEXUPDATES::OnExitButton() 
{
	char strErrorMessage[200];
	int ret_code = 0;
	

	if(responsePending) //has pressed exit while response is pending
	{
		// confirm exit
		strcpy (strErrorMessage, "Waiting for a response. Are you sure you want exit?") ;

		ret_code = AfxMessageBox (strErrorMessage, MB_OKCANCEL|MB_ICONQUESTION) ;
		if (ret_code != IDOK) 
			return ;

	}

	
	CDialog::OnCancel();	
}

void CAMEXUPDATES::OnKillfocusAccountNumberFld() 
{
	// TODO: Add your control notification handler code here
	
}

void CAMEXUPDATES::OnKillfocusActionCode() 
{
	// TODO: Add your control notification handler code here
	
}

void CAMEXUPDATES::OnKillfocusExpireDateFld() 
{
	// TODO: Add your control notification handler code here
	
}

void CAMEXUPDATES::OnKillfocusRequestType() 
{
	// TODO: Add your control notification handler code here
	
}

void CAMEXUPDATES::OnSubmitButton() 
{
   char strErrorMessage[200];
   char que_name[10] = "";
   char line1[50];
   char line2[50];
   char line3[50];
   char line4[50];
   char year[5], month[3], day[3], hour[3], minutes[3], seconds[3];
   int  rcode;

   m_strMessageLbl.Empty();

   if ( CheckDialogForCompleteness() )
   {
      if ( PopulateDatabaseStructure() )
      {
         m_strMessageLbl = "Please Wait ... Waiting for Response";

         m_strResponseCode   = "";
         m_strResponseText   = "";
         m_cntlTranInfo.ResetContent();//clear list box

         UpdateData(FALSE);

         memcpy( que_name, m_strDestQue, 6 );
         strcat( que_name, "A" );

         responsePending = TRUE;
         BeginWaitCursor();
         rcode = txdsapi_send_amex_record( &sAMEXRecord,
                                           0,
                                           que_name,
                                           strErrorMessage );
         EndWaitCursor();
         if( PTEMSG_OK != rcode )
         {
            m_strMessageLbl = strErrorMessage;
            UpdateData(FALSE);
            responsePending = FALSE;
         }
         else
         {
            m_strMessageLbl   = "Transaction Response Received";
            m_strResponseCode =  sAMEXRecord.resp_code;
            m_strResponseText =  sAMEXRecord.resp_text;
            m_cntlTranInfo.ResetContent();//clear list box

            memset (&year,    0x00, sizeof(year)   );
            memset (&month,   0x00, sizeof(month)  );
            memset (&day,     0x00, sizeof(day)    );
            memset (&hour,    0x00, sizeof(hour)   );
            memset (&minutes, 0x00, sizeof(minutes));
            memset (&seconds, 0x00, sizeof(seconds));

            memcpy (year,    sAMEXRecord.date_time,      4);
            memcpy (month,   sAMEXRecord.date_time + 4,  2);
            memcpy (day,     sAMEXRecord.date_time + 6,  2);
            memcpy (hour,    sAMEXRecord.date_time + 8,  2);
            memcpy (minutes, sAMEXRecord.date_time + 10, 2);
            memcpy (seconds, sAMEXRecord.date_time + 12, 2);

            sprintf(line1, "Date:\t\t%s/%s/%s  (dd/mm/yyyy)", day, month, year);
            sprintf(line2, "Time:\t\t%s:%s:%s ", hour, minutes, seconds);
            sprintf(line3, "System Trace Nbr:\t%s ", sAMEXRecord.stan);
            sprintf(line4, "Fowarding Inst ID:\t%s", sAMEXRecord.fwd_inst_code);

            /* Populate Transaction Info Box */
            m_cntlTranInfo.AddString(line1);
            m_cntlTranInfo.AddString(line2);
            m_cntlTranInfo.AddString(line3);
            m_cntlTranInfo.AddString(line4);

            UpdateData(FALSE);

            responsePending = FALSE;
         }
      }
   }
   return;
}



BOOL CAMEXUPDATES::CheckDialogForCompleteness()
{
   CMyUtils myUtilsObject;
   char CardNbr[20];
   int date_len = 0;
   int month = 0;
   int verify_digit = 0;
   int length = 0;

   UpdateData(TRUE);

   /*Verify account number is present*/
   if ( myUtilsObject.IsEmpty (m_strAcctNbr, this ) )
   {
      m_strMessageLbl = "Account Number is required.";
      UpdateData(FALSE);
      m_cntlAcctNbr.SetFocus();
      return FALSE;
   }

   /*Verify account number check digit if check digit is checked*/
   if (m_CkDigit)  
   {
      length = m_strAcctNbr.GetLength();
      lstrcpy ((LPTSTR) CardNbr,m_strAcctNbr.Left(length - 1));	
      verify_digit = myUtilsObject.GetMod10CheckDigit( CardNbr, length - 1 );
      if (atoi(m_strAcctNbr.Right(1)) != verify_digit)
      {
         m_strMessageLbl = "Invalid Account Number - check digit failed.";
         UpdateData(FALSE);
         m_cntlAcctNbr.SetFocus();
         return FALSE;
      }
   }

   /*Verify expiration date is present and valid*/
   if ( myUtilsObject.IsEmpty (m_strExpireDate, this ) 
        && (m_nDialogMode == MODE_ADD || m_nDialogMode == MODE_EDIT) )
   {
      m_strMessageLbl = "Expiration Date is required.(YYMM)";
      UpdateData(FALSE);
      m_cntlExpireDate.SetFocus();
      return FALSE;
   }

   date_len = m_strExpireDate.GetLength();  /*validate month if date exists*/
   if (date_len > 0)
   {
      month = atoi(m_strExpireDate.Right(2));
      if (date_len < 4 || month < 1 || month > 12)
      {
         m_strMessageLbl = "Invalid Expiration Date. (YYMM)";
         UpdateData(FALSE);
         m_cntlExpireDate.SetFocus();
         return FALSE;
      }
   }

   /*Verify request type is present*/
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
      else if(strcmp(m_strRequestType, "Update") == 0)
         m_nDialogMode = MODE_EDIT;
      else if(strcmp(m_strRequestType, "Delete") == 0)
         m_nDialogMode = MODE_DELETE;
   }

   /*Verify action code is present*/
   if ( myUtilsObject.IsEmpty (m_strActionCode, this )
        && (m_nDialogMode == MODE_ADD || m_nDialogMode == MODE_EDIT) )
   {
      m_strMessageLbl = "Action Code is required.";
      UpdateData(FALSE);
      m_cntlActionCode.SetFocus();
      return FALSE;
   }

   /* Verify destination service is present */
   if ( myUtilsObject.IsEmpty (m_strDestQue, this ) )
   {
      m_strMessageLbl = "Destination Service is required = Network Controller.";
      UpdateData(FALSE);
      m_cntlDestQue.SetFocus();
      return FALSE;
   }

   return TRUE;
}


BOOL CAMEXUPDATES::PopulateDatabaseStructure()
{
   CHAR temp[20]; 
   INT  nCount = 0;
   INT  idx = 0;
   CHAR strDeviceID[20] = "";

	UpdateData(TRUE);

	ZeroMemory ( &sAMEXRecord, sizeof (AMEX_ONLINE_UPDATES));

	/*Account Number*/
	memcpy ( &sAMEXRecord.card_nbr, m_strAcctNbr, sizeof ( sAMEXRecord.card_nbr) -1);

	/*Function Code = 301 for add/update and 303 for delete*/
	if ( m_nDialogMode == MODE_ADD || m_nDialogMode == MODE_EDIT )
		memcpy (&sAMEXRecord.function_code, RECORD_ADD_UPDATE, sizeof(sAMEXRecord.function_code) -1);
	else if (  m_nDialogMode == MODE_DELETE )
		memcpy (&sAMEXRecord.function_code, RECORD_DELETE, sizeof(sAMEXRecord.function_code) -1);

	/*Expiration Date*/
	memcpy ( sAMEXRecord.exp_date, m_strExpireDate, sizeof(sAMEXRecord.exp_date ) -1);

	/*Reason Code = GUI Action Code*/
	memset (temp, 0x00, sizeof(temp));
	strcpy (temp, m_strActionCode.Left(4));
	memcpy ( &sAMEXRecord.reason_code, temp, sizeof(sAMEXRecord.reason_code ) -1);


   /* Get Terminal ID from TF.INI file. */
   GetPrivateField1(strDeviceID);
   memcpy( sAMEXRecord.terminal_id, strDeviceID, 8 );

   /* Get Workstation and Operator ID for logging to TLF01. */
   memcpy( &sAMEXRecord.workstation, workstation, 20 );
   memcpy( &sAMEXRecord.operator_id, ucf01.primary_key.name, 20 );

	return TRUE;
}

BOOL CAMEXUPDATES::OnInitDialog() 
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

   CString temp;
   int nIndex = -1;

	//set version info in the title bar
	CString appProdVer;
	CString lpszTitle;
	appProdVer.LoadString(IDS_PRODUCT_VERSION);
	this->GetWindowText(lpszTitle);
	lpszTitle = lpszTitle + " - Version - ";
	lpszTitle += appProdVer;
	this->SetWindowText(lpszTitle);

   m_strMessageLbl.Empty();

   for ( int i = 0; i <=15; i++ )
   {
      temp = ucf01.control[i].subsystem_name;
      if ( strcmp ( temp, "Card Account Control" ) == 0)
         nIndex = i;
   }

   if ( (nIndex != -1) &&
        ( '1' == ucf01.control[nIndex].action[0] ) ||  /* add or */
        ( '1' == ucf01.control[nIndex].action[2] )  )  /* update */
   {
      m_cntlAcctNbr.EnableWindow(TRUE);     /* Initialize entry Fields*/
      m_cntlExpireDate.EnableWindow(TRUE);
      m_cntlRequestType.EnableWindow(TRUE);
      m_cntlActionCode.EnableWindow(TRUE);
      m_CkDigit = TRUE;
      m_cntlResponseText.EnableWindow(FALSE);
      m_cntlResponseCode.EnableWindow(FALSE);
      m_cntlTranInfo.EnableWindow(FALSE);
        /*Enable buttons*/
      m_cntlSubmitButton.EnableWindow(TRUE);
      m_cntlExitButton.EnableWindow(TRUE);
   }
   else
   {
      m_cntlAcctNbr.EnableWindow(FALSE);      /* Initialize entry Fields*/
      m_cntlExpireDate.EnableWindow(FALSE);
      m_cntlRequestType.EnableWindow(FALSE);
      m_cntlActionCode.EnableWindow(FALSE);
      m_CkDigit = FALSE;
      m_cntlResponseText.EnableWindow(FALSE);
      m_cntlResponseCode.EnableWindow(FALSE);
      m_cntlTranInfo.EnableWindow(FALSE);
         /*Enable buttons*/
      m_cntlSubmitButton.EnableWindow(FALSE);
      m_cntlExitButton.EnableWindow(TRUE);
   }

   /* Set default value for destination service = name of network controller */
   m_strDestQue = "ncpos";

   UpdateData(FALSE);   
   
   return TRUE;  // return TRUE unless you set the focus to a control
                 // EXCEPTION: OCX Property Pages should return FALSE
}


BOOL CAMEXUPDATES::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	// TODO: Add your specialized code here and/or call the base class
	c_time = CTime::GetCurrentTime();	
	shrmem.SetLastUpdateTime(c_time);
	return CDialog::OnCommand(wParam, lParam);
}