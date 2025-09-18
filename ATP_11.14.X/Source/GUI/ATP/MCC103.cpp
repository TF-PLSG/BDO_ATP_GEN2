// MCC103.cpp : implementation file
//
/*
Rev        April 28.4.11          Krishna Kiran S@TF
	 Added Idle Time Calculation & calling Login screen on Idle Timeout
*/
#include "stdafx.h"
#include "atp.h"
#include "MCC103.h"
#include "MyUtils.h"
//#include "Login.h"
#include "SharedMem.h"
extern "C"
{
   #include "ntutils.h"
}

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern UCF01 ucf01;
extern CString workstation;
extern CSharedMem shrmem;
//extern CLogin login;

/////////////////////////////////////////////////////////////////////////////
// CMCC103 dialog


CMCC103::CMCC103(CWnd* pParent /*=NULL*/)
	: CDialog(CMCC103::IDD, pParent)
{
	//{{AFX_DATA_INIT(CMCC103)
	m_StrEntryReason = _T("");
	m_StrMessageLbl = _T("");
	m_StrProductCode = _T("");
	m_StrPurgeDateFld = _T("");
	m_Asia = FALSE;
	m_Canada = FALSE;
	m_Europe = FALSE;
	m_Latin = FALSE;
	m_USA = FALSE;
	m_StrPurgeDateFld2 = _T("");
	m_StrPurgeDateFld3 = _T("");
	m_StrPurgeDateFld4 = _T("");
	m_StrPurgeDateFld5 = _T("");
	m_StrAccountNumberFld = _T("");
	m_Africa = FALSE;
	m_StrPurgeDateFld6 = _T("");
	m_strRespCode = _T("");
	m_strErrorResults = _T("");
	m_CkDigit = FALSE;
	m_strAccountResults = _T("");
	//}}AFX_DATA_INIT
}


void CMCC103::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMCC103)
	DDX_Control(pDX, IDC_ACCOUNT_RESULTS, m_cntlAccountResults);
	DDX_Control(pDX, IDC_CKDIGIT, m_cntlCkDigit);
	DDX_Control(pDX, IDC_ERROR_RESULTS, m_cntlErrorResults);
	DDX_Control(pDX, IDC_RESP_CODE3, m_cntlRespCode);
	DDX_Control(pDX, IDC_INQUIRE_BUTTON, m_InquiryButton);
	DDX_Control(pDX, IDC_PURGE_DATE_FLD6, m_CntlPurgeDateFld6);
	DDX_Control(pDX, IDC_AFRICA, m_CntlAfrica);
	DDX_Control(pDX, IDC_USA, m_CntlUSA);
	DDX_Control(pDX, IDC_LATIN, m_CntlLatin);
	DDX_Control(pDX, IDC_EUROPE, m_CntlEurope);
	DDX_Control(pDX, IDC_CANADA, m_CntlCanada);
	DDX_Control(pDX, IDC_ASIA, m_CntlAsia);
	DDX_Control(pDX, IDC_DELETE_BUTTON, m_DeleteButton);
	DDX_Control(pDX, IDC_ADD_BUTTON, m_AddButton);
	DDX_Control(pDX, IDC_PRODUCT_CODE, m_CntlProductCode);
	DDX_Control(pDX, IDC_ENTRY_REASON, m_CntlEntryReason);
	DDX_Control(pDX, IDC_ACCOUNT_NUMBER_FLD, m_CntlAccountNumberFld);
	DDX_Control(pDX, IDC_PURGE_DATE_FLD5, m_CntlPurgeDateFld5);
	DDX_Control(pDX, IDC_PURGE_DATE_FLD4, m_CntlPurgeDateFld4);
	DDX_Control(pDX, IDC_PURGE_DATE_FLD3, m_CntlPurgeDateFld3);
	DDX_Control(pDX, IDC_PURGE_DATE_FLD2, m_CntlPurgeDateFld2);
	DDX_Control(pDX, IDC_PURGE_DATE_FLD, m_CntlPurgeDateFld);
	DDX_CBString(pDX, IDC_ENTRY_REASON, m_StrEntryReason);
	DDX_Text(pDX, IDC_MESSAGE_LBL, m_StrMessageLbl);
	DDX_CBString(pDX, IDC_PRODUCT_CODE, m_StrProductCode);
	DDX_Text(pDX, IDC_PURGE_DATE_FLD, m_StrPurgeDateFld);
	DDV_MaxChars(pDX, m_StrPurgeDateFld, 8);
	DDX_Check(pDX, IDC_ASIA, m_Asia);
	DDX_Check(pDX, IDC_CANADA, m_Canada);
	DDX_Check(pDX, IDC_EUROPE, m_Europe);
	DDX_Check(pDX, IDC_LATIN, m_Latin);
	DDX_Check(pDX, IDC_USA, m_USA);
	DDX_Text(pDX, IDC_PURGE_DATE_FLD2, m_StrPurgeDateFld2);
	DDV_MaxChars(pDX, m_StrPurgeDateFld2, 8);
	DDX_Text(pDX, IDC_PURGE_DATE_FLD3, m_StrPurgeDateFld3);
	DDV_MaxChars(pDX, m_StrPurgeDateFld3, 8);
	DDX_Text(pDX, IDC_PURGE_DATE_FLD4, m_StrPurgeDateFld4);
	DDV_MaxChars(pDX, m_StrPurgeDateFld4, 8);
	DDX_Text(pDX, IDC_PURGE_DATE_FLD5, m_StrPurgeDateFld5);
	DDV_MaxChars(pDX, m_StrPurgeDateFld5, 8);
	DDX_Text(pDX, IDC_ACCOUNT_NUMBER_FLD, m_StrAccountNumberFld);
	DDV_MaxChars(pDX, m_StrAccountNumberFld, 19);
	DDX_Check(pDX, IDC_AFRICA, m_Africa);
	DDX_Text(pDX, IDC_PURGE_DATE_FLD6, m_StrPurgeDateFld6);
	DDV_MaxChars(pDX, m_StrPurgeDateFld6, 8);
	DDX_Text(pDX, IDC_RESP_CODE3, m_strRespCode);
	DDX_LBString(pDX, IDC_ERROR_RESULTS, m_strErrorResults);
	DDX_Check(pDX, IDC_CKDIGIT, m_CkDigit);
	DDX_LBString(pDX, IDC_ACCOUNT_RESULTS, m_strAccountResults);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CMCC103, CDialog)
	//{{AFX_MSG_MAP(CMCC103)
	ON_BN_CLICKED(IDC_ADD_BUTTON, OnAddButton)
	ON_BN_CLICKED(IDC_DELETE_BUTTON, OnDeleteButton)
	ON_BN_CLICKED(IDC_ASIA, OnAsia)
	ON_BN_CLICKED(IDC_CANADA, OnCanada)
	ON_BN_CLICKED(IDC_EUROPE, OnEurope)
	ON_BN_CLICKED(IDC_LATIN, OnLatin)
	ON_BN_CLICKED(IDC_USA, OnUsa)
	ON_BN_CLICKED(IDC_AFRICA, OnAfrica)
	ON_BN_CLICKED(IDC_INQUIRE_BUTTON, OnInquireButton)
	ON_BN_CLICKED(IDOK, OnExit)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMCC103 message handlers

BOOL CMCC103::OnInitDialog() 
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

/*	cntr = 0;
   
    timer_handle = StartTimer(1000);	   

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
	m_CntlEntryReason.EnableWindow(TRUE);
	m_CntlProductCode.EnableWindow(TRUE);
	m_CntlAsia.EnableWindow(TRUE);
	m_CntlCanada.EnableWindow(TRUE);
	m_CntlEurope.EnableWindow(TRUE);
  	m_CntlAfrica.EnableWindow(TRUE);
	m_CntlLatin.EnableWindow(TRUE);
	m_CntlUSA.EnableWindow(TRUE);
	m_CntlAccountNumberFld.EnableWindow(TRUE);
    m_cntlCkDigit.EnableWindow(TRUE);
    m_CkDigit = TRUE;

	if ( nIndex != -1)
	{
		if ( '1' == ucf01.control[nIndex].action[0]  &&
           ( '1' == ucf01.control[nIndex].action[1]  ) &&
           ( '1' == ucf01.control[nIndex].action[2]  ) &&
           ( '1' == ucf01.control[nIndex].action[3]  ) )

		{
			m_AddButton.EnableWindow(TRUE);
			m_DeleteButton.EnableWindow(TRUE);
		}
		else if ( '0' == ucf01.control[nIndex].action[0]  &&
         ( '1' == ucf01.control[nIndex].action[1]  ) &&
         ( '0' == ucf01.control[nIndex].action[2]  ) &&
         ( '1' == ucf01.control[nIndex].action[3]  ) )

		{
			m_AddButton.EnableWindow(FALSE);
			m_DeleteButton.EnableWindow(TRUE);
		}
		else if ( '1' == ucf01.control[nIndex].action[0]  &&
         ( '0' == ucf01.control[nIndex].action[1]  ) &&
         ( '0' == ucf01.control[nIndex].action[2]  ) &&
         ( '1' == ucf01.control[nIndex].action[3]  ) )

		{
			m_AddButton.EnableWindow(TRUE);
			m_DeleteButton.EnableWindow(FALSE);
		}
		else if ( '0' == ucf01.control[nIndex].action[0]  &&  /*scr1138 - Update permission gives full access 7/1/03*/
         ( '0' == ucf01.control[nIndex].action[1]  ) &&
         ( '1' == ucf01.control[nIndex].action[2]  ) &&
         ( '1' == ucf01.control[nIndex].action[3]  ) )

		{
			m_AddButton.EnableWindow(TRUE);
			m_DeleteButton.EnableWindow(TRUE);
		}
		else if ( '1' == ucf01.control[nIndex].action[0]  &&  /*scr1138 - Update permission gives full access 7/1/03*/
         ( '0' == ucf01.control[nIndex].action[1]  ) &&
         ( '1' == ucf01.control[nIndex].action[2]  ) &&
         ( '1' == ucf01.control[nIndex].action[3]  ) )

		{
			m_AddButton.EnableWindow(TRUE);
			m_DeleteButton.EnableWindow(TRUE);
		}
		else
		{
			m_CntlEntryReason.EnableWindow(FALSE);
			m_CntlProductCode.EnableWindow(FALSE);
			m_CntlAsia.EnableWindow(FALSE);
			m_CntlCanada.EnableWindow(FALSE);
			m_CntlEurope.EnableWindow(FALSE);
            m_CntlEurope.EnableWindow(FALSE);
			m_CntlLatin.EnableWindow(FALSE);
			m_CntlUSA.EnableWindow(FALSE);
			m_CntlAccountNumberFld.EnableWindow(FALSE);
		 	m_CntlAfrica.EnableWindow(FALSE);
			m_AddButton.EnableWindow(FALSE);
			m_DeleteButton.EnableWindow(FALSE);
         m_cntlCkDigit.EnableWindow(FALSE);
         m_CkDigit = TRUE;
		}
	}	
	m_CntlPurgeDateFld.EnableWindow(FALSE);
	m_CntlPurgeDateFld2.EnableWindow(FALSE);
	m_CntlPurgeDateFld3.EnableWindow(FALSE);
	m_CntlPurgeDateFld4.EnableWindow(FALSE);
	m_CntlPurgeDateFld5.EnableWindow(FALSE);
	m_CntlPurgeDateFld6.EnableWindow(FALSE);
    m_cntlAccountResults.EnableWindow(TRUE);
    m_cntlErrorResults.EnableWindow(TRUE);
    m_cntlRespCode.EnableWindow(TRUE);

    m_cntlAccountResults.ResetContent();
    m_cntlErrorResults.ResetContent();
    m_cntlRespCode.Clear();

   UpdateData(FALSE);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CMCC103::OnAddButton() 
{
   CHAR strErrorMessage[200];
   INT  rcode;

   m_nDialogMode = MODE_ADD;

   m_cntlAccountResults.ResetContent();
   m_cntlErrorResults.ResetContent();

   if ( CheckDialogForCompleteness() )
   {
      if ( PopulateDatabaseStructure() )
      {
         m_strRespCode = "";
         m_StrMessageLbl = "Please Wait ... Processing Add/Update Message";
         UpdateData(FALSE);

         rcode = txdsapi_send_mcc_record(&Mcc_Gui,MCC103_DATA,strErrorMessage);
         if( PTEMSG_OK == rcode )
         {
            m_strRespCode = Mcc_Gui.response_code;
            if ( 0 == strcmp(Mcc_Gui.response_code, "00") )
            {
               m_StrMessageLbl = "Transaction Response Received";
               handle_response();
            }
            else 
            {
               m_StrMessageLbl = "Record not added or updated to Account Management File";
               m_strRespCode = Mcc_Gui.response_code;
               m_cntlErrorResults.AddString(Mcc_Gui.err_msg1);
               m_cntlErrorResults.AddString(Mcc_Gui.err_msg2);
               m_cntlErrorResults.AddString(Mcc_Gui.err_msg3);
            }
         }
         else 
         {
            m_strRespCode = "96";
            m_StrMessageLbl = strErrorMessage;
         }
         UpdateData(FALSE);
      }
   }
   return;
}

void CMCC103::OnDeleteButton() 
{
   CHAR strErrorMessage[200];
   INT  rcode;

   m_nDialogMode = MODE_DELETE;

   m_cntlAccountResults.ResetContent();
   m_cntlErrorResults.ResetContent();

   if ( CheckDialogForCompleteness() )
   {
      if ( PopulateDatabaseStructure() )
      {
         m_strRespCode = "";
         m_StrMessageLbl = "Please Wait ... Processing Delete Message";
         UpdateData(FALSE);

         rcode = txdsapi_send_mcc_record( &Mcc_Gui, MCC103_DATA, strErrorMessage );
         if( PTEMSG_OK == rcode )
         {
            m_strRespCode = Mcc_Gui.response_code;
            if ( 0 == strcmp(Mcc_Gui.response_code, "00") )
            {
               m_StrMessageLbl = "Transaction Response Received";
               handle_response();
            }
            else 
            {
               m_StrMessageLbl = "Record not deleted from Account Management File";
               m_strRespCode = Mcc_Gui.response_code;
               m_cntlErrorResults.AddString(Mcc_Gui.err_msg1);
               m_cntlErrorResults.AddString(Mcc_Gui.err_msg2);
               m_cntlErrorResults.AddString(Mcc_Gui.err_msg3);
            }
         }
         else 
         {
            m_strRespCode = "96";
            m_StrMessageLbl = strErrorMessage;
         }
         UpdateData(FALSE);
      }
   }
   return;
}

void CMCC103::OnInquireButton() 
{
   CHAR strErrorMessage[200];
   INT  rcode;

   m_nDialogMode = MODE_INQUIRE;

   m_cntlAccountResults.ResetContent();
   m_cntlErrorResults.ResetContent();

   if ( CheckDialogForCompleteness() )
   {
      if ( PopulateDatabaseStructure() )
      {
         m_strRespCode = "";
         m_StrMessageLbl = "Please Wait ... Processing Inquiry Message";
         UpdateData(FALSE);

         rcode = txdsapi_send_mcc_record( &Mcc_Gui, MCC103_DATA, strErrorMessage );
         if( PTEMSG_OK == rcode )
         {
            m_strRespCode = Mcc_Gui.response_code;
            if ( 0 == strcmp(Mcc_Gui.response_code, "00") )
            {
               m_StrMessageLbl = "Transaction Response Received";
               handle_response();
            }
            else 
            {
               m_StrMessageLbl = "Unable to read record from Account Management File";
               m_strRespCode = Mcc_Gui.response_code;
               m_cntlErrorResults.AddString(Mcc_Gui.err_msg1);
               m_cntlErrorResults.AddString(Mcc_Gui.err_msg2);
               m_cntlErrorResults.AddString(Mcc_Gui.err_msg3);
            }
         }
         else 
         {
            m_strRespCode = "96";
            m_StrMessageLbl = strErrorMessage;
         }
         UpdateData(FALSE);
      }
   }
   return;
}

BOOL CMCC103::PopulateDatabaseStructure()
{
   CString temp; 
   char strIndicator[10];
   int nCount = 0;
   CHAR strDeviceID[20] = "";

   UpdateData(TRUE);

   ZeroMemory( &Mcc_Gui, sizeof(MCC_ONLINE_UPDATES) );

   memcpy( Mcc_Gui.msg_type, "MCC103", sizeof(Mcc_Gui.msg_type) );

	if ( MODE_ADD == m_nDialogMode )
		temp = "A";
	else if ( MODE_DELETE == m_nDialogMode )
		temp = "D";
   else
      temp = "I";

   Mcc_Gui.choice = temp[0];
	
	memcpy ( Mcc_Gui.card_nbr, m_StrAccountNumberFld, sizeof ( Mcc_Gui.card_nbr));

	if ( m_StrProductCode == "MasterCard Card" )
		temp = "MCC";
	else if ( m_StrProductCode == "MasterCard Debit Card" )
		temp = "MCD";
	else if ( m_StrProductCode == "Gold MasterCard Card" )
		temp = "MCG";
	else if ( m_StrProductCode == "Non-standard" )
		temp = "MNS";
	else if ( m_StrProductCode == "Other" )
		temp = "OTH";
	memcpy ( Mcc_Gui.product_code, temp, sizeof ( Mcc_Gui.product_code));

	if ( m_StrEntryReason == "Credit" )
		temp = "C";
	else if ( m_StrEntryReason == "Counterfeit" )
		temp = "X";
	else if ( m_StrEntryReason == "Other" )
		temp = "O";
	else if ( m_StrEntryReason == "Fraud" )
		temp = "F";

   Mcc_Gui.reason = temp[0];


	if ( !ValidateDate ( m_StrPurgeDateFld ))
	{
		m_CntlPurgeDateFld.SetFocus();
		return FALSE;
	}
	if ( !ValidateDate ( m_StrPurgeDateFld2 ))
	{
		m_CntlPurgeDateFld2.SetFocus();
		return FALSE;
	}
	if ( !ValidateDate ( m_StrPurgeDateFld3 ))
	{
		m_CntlPurgeDateFld3.SetFocus();
		return FALSE;
	}
	if ( !ValidateDate ( m_StrPurgeDateFld4 ))
	{
		m_CntlPurgeDateFld4.SetFocus();
		return FALSE;
	}
	if ( !ValidateDate ( m_StrPurgeDateFld5 ))
	{
		m_CntlPurgeDateFld5.SetFocus();
		return FALSE;
	}
	if ( !ValidateDate ( m_StrPurgeDateFld6 ))
	{
		m_CntlPurgeDateFld6.SetFocus();
		return FALSE;
	}
  
   if ( m_USA == 1 )
   {
      nCount ++;
      memset( strIndicator, 0, sizeof (strIndicator));
      temp = m_StrPurgeDateFld.Right(2) + m_StrPurgeDateFld.Mid(2,2) + m_StrPurgeDateFld.Left(2);
      strcpy( strIndicator, temp );
      Mcc_Gui.region_us.indicator = '1';
      memcpy( Mcc_Gui.region_us.purge_date, strIndicator, sizeof(Mcc_Gui.region_us.purge_date));
   }
   if ( m_Canada == 1 )
   {
      nCount ++;
      memset( strIndicator, 0, sizeof (strIndicator));
      temp = m_StrPurgeDateFld2.Right(2) + m_StrPurgeDateFld2.Mid(2,2) + m_StrPurgeDateFld2.Left(2);
      strcpy( strIndicator, temp );
      Mcc_Gui.region_canada.indicator = 'A';
      memcpy( Mcc_Gui.region_canada.purge_date, strIndicator, sizeof(Mcc_Gui.region_canada.purge_date));
   }
   if ( m_Latin == 1 )
   {
      nCount ++;
      memset( strIndicator, 0, sizeof (strIndicator));
      temp = m_StrPurgeDateFld3.Right(2) + m_StrPurgeDateFld3.Mid(2,2) + m_StrPurgeDateFld3.Left(2);
      strcpy( strIndicator, temp );
      Mcc_Gui.region_sa.indicator = 'B';
      memcpy( Mcc_Gui.region_sa.purge_date, strIndicator, sizeof(Mcc_Gui.region_sa.purge_date));
   }
   if ( m_Asia == 1 )
   {
      nCount ++;
      memset( strIndicator, 0, sizeof (strIndicator));
      temp = m_StrPurgeDateFld4.Right(2) + m_StrPurgeDateFld4.Mid(2,2) + m_StrPurgeDateFld4.Left(2);
      strcpy( strIndicator, temp );
      Mcc_Gui.region_asia.indicator = 'C';
      memcpy( Mcc_Gui.region_asia.purge_date, strIndicator, sizeof(Mcc_Gui.region_asia.purge_date));
   }
   if ( m_Europe == 1 )
   {
      nCount ++;
      memset( strIndicator, 0, sizeof (strIndicator));
      temp = m_StrPurgeDateFld5.Right(2) + m_StrPurgeDateFld5.Mid(2,2) + m_StrPurgeDateFld5.Left(2);
      strcpy( strIndicator, temp );
      Mcc_Gui.region_europe.indicator = 'D';
      memcpy( Mcc_Gui.region_europe.purge_date, strIndicator, sizeof(Mcc_Gui.region_europe.purge_date));
   }

   if ( m_Africa == 1 )
   {
      nCount ++;
      memset( strIndicator, 0, sizeof (strIndicator));
      temp = m_StrPurgeDateFld6.Right(2) + m_StrPurgeDateFld6.Mid(2,2) + m_StrPurgeDateFld6.Left(2);
      strcpy( strIndicator, temp );
      Mcc_Gui.region_africa.indicator = 'E';
      memcpy( Mcc_Gui.region_africa.purge_date, strIndicator, sizeof(Mcc_Gui.region_africa.purge_date));
	}

   sprintf( Mcc_Gui.number_of_regions, "%02d", nCount );

   /* Get Terminal ID from TF.INI file. */
   GetPrivateField1(strDeviceID);
   memcpy( Mcc_Gui.terminal_id, strDeviceID, 8 );

   /* Get Workstation and Operator ID for logging to TLF01. */
   memcpy( &Mcc_Gui.workstation, workstation, 20 );
   memcpy( &Mcc_Gui.operator_id, ucf01.primary_key.name, 20 );

	return TRUE;

}

BOOL CMCC103::ValidateDate ( CString strDate )
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
		m_StrMessageLbl = "ERROR: Invalid Date Format";
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
		m_StrMessageLbl = "ERROR: Invalid Data";
		UpdateData(FALSE);
		return FALSE;
	}


	if ( nMonth > 12 )
	{
		m_StrMessageLbl = "ERROR: Invalid Month";
		UpdateData(FALSE);
		return FALSE;
	}

	if ( nMonth == 1 || nMonth == 3 || nMonth == 5 || 
		 nMonth == 7 || nMonth == 8 || nMonth == 10 || nMonth == 12 )
	{
		if ( nDay > 31 )
		{
			m_StrMessageLbl = "ERROR: Invalid Day";
			UpdateData(FALSE);
			return FALSE;
		}
	}

	if ( nMonth == 4 || nMonth == 6 || nMonth == 9 || 
		 nMonth == 11 )
	{
		if ( nDay > 30 )
		{
			m_StrMessageLbl = "ERROR: Invalid Day";
			UpdateData(FALSE);
			return FALSE;
		}
	}

	if ( nMonth ==2 )
	{
		int res = nYear % 4;
		if ( res != 0 && nDay >28)
		{
			m_StrMessageLbl = "ERROR:Invalid Day";
			UpdateData(FALSE);
			return FALSE;
		}
		if (res == 0 && nDay >29)
		{
			m_StrMessageLbl = "ERROR:Invalid Day";
			UpdateData(FALSE);
			return FALSE;
		}
	}
	
	return TRUE;
}

void CMCC103::OnAsia() 
{
	UpdateData(TRUE);
	if ( m_Asia == 1 )
	{
		m_CntlPurgeDateFld4.EnableWindow(TRUE);
		m_CntlPurgeDateFld4.SetFocus();
	}
	else if ( m_Asia == 0 )
   {
		m_CntlPurgeDateFld4.EnableWindow(FALSE);
      m_StrPurgeDateFld4 = "";
      UpdateData(FALSE);
   }
}

void CMCC103::OnCanada() 
{
	UpdateData(TRUE);
	if ( m_Canada == 1 )
	{
		m_CntlPurgeDateFld2.EnableWindow(TRUE);
		m_CntlPurgeDateFld2.SetFocus();
	}
	else if ( m_Canada == 0 )
   {
		m_CntlPurgeDateFld2.EnableWindow(FALSE);
      m_StrPurgeDateFld2 = "";
      UpdateData(FALSE);
   }
	
}

void CMCC103::OnEurope() 
{
	UpdateData(TRUE);
	if ( m_Europe == 1 )
	{
		m_CntlPurgeDateFld5.EnableWindow(TRUE);
		m_CntlPurgeDateFld5.SetFocus();
	}
	else if ( m_Europe == 0 )
   {
		m_CntlPurgeDateFld5.EnableWindow(FALSE);
      m_StrPurgeDateFld5 = "";
      UpdateData(FALSE);
   }
	
}

void CMCC103::OnLatin() 
{
	UpdateData(TRUE);
	if ( m_Latin == 1 )
	{
		m_CntlPurgeDateFld3.EnableWindow(TRUE);
		m_CntlPurgeDateFld3.SetFocus();
	}
	else if ( m_Latin == 0 )
   {
		m_CntlPurgeDateFld3.EnableWindow(FALSE);
      m_StrPurgeDateFld3 = "";
      UpdateData(FALSE);
   }

	
}

void CMCC103::OnUsa() 
{
	UpdateData(TRUE);
	if ( m_USA == 1 )
	{
		m_CntlPurgeDateFld.EnableWindow(TRUE);
		m_CntlPurgeDateFld.SetFocus();
	}
	else if ( m_USA == 0 )
   {
		m_CntlPurgeDateFld.EnableWindow(FALSE);
      m_StrPurgeDateFld = "";
      UpdateData(FALSE);
   }
	
}

void CMCC103::OnAfrica() 
{
	UpdateData(TRUE);
	if ( m_Africa == 1 )
	{
		m_CntlPurgeDateFld6.EnableWindow(TRUE);
		m_CntlPurgeDateFld6.SetFocus();
	}
	else if ( m_Africa == 0 )
   {
		m_CntlPurgeDateFld6.EnableWindow(FALSE);	
      m_StrPurgeDateFld6 = "";
      UpdateData(FALSE);
   }
}

BOOL CMCC103::CheckDialogForCompleteness()
{
	CMyUtils myUtilsObject;
   CHAR     CardNbr[20] = "";
   INT      verify_digit = 0;
   INT      length = 0;

	UpdateData(TRUE);

	if ( myUtilsObject.IsEmpty (m_StrEntryReason, this ) )
	{
		m_StrMessageLbl = "Please enter Entry Reason";
		UpdateData(FALSE);
		m_CntlEntryReason.SetFocus();
		return FALSE;
	}

	if ( myUtilsObject.IsEmpty (m_StrProductCode, this )) 
	{
		m_StrMessageLbl = "Please enter Product Code";
		UpdateData(FALSE);
		m_CntlProductCode.SetFocus();
		return FALSE;
	}
	if ( myUtilsObject.IsEmpty (m_StrAccountNumberFld, this ) )
	{
		m_StrMessageLbl = "Please enter Account Number";
		UpdateData(FALSE);
		m_CntlAccountNumberFld.SetFocus();
		return FALSE;
	}
   if (m_CkDigit)  //verify account number if check digit checked
   {
      length = m_StrAccountNumberFld.GetLength();
      lstrcpy ((LPTSTR) CardNbr,m_StrAccountNumberFld.Left(length - 1));	
      verify_digit = myUtilsObject.GetMod10CheckDigit( CardNbr, length - 1 );
      if (atoi(m_StrAccountNumberFld.Right(1)) != verify_digit)
      {
         m_StrMessageLbl = "Invalid Account Number - check digit failed.";
         UpdateData(FALSE);
         m_CntlAccountNumberFld.SetFocus();
         return FALSE;
      }
   }

	if ( myUtilsObject.IsEmpty (m_StrPurgeDateFld, this ) && (m_USA == 1 ) )
	{
		m_StrMessageLbl = "Please enter Purge Date";
		UpdateData(FALSE);
		m_CntlPurgeDateFld.SetFocus();
		return FALSE;
	}
	if ( myUtilsObject.IsEmpty (m_StrPurgeDateFld2, this ) && (m_Canada == 1 ) )
	{
		m_StrMessageLbl = "Please enter Purge Date";
		UpdateData(FALSE);
		m_CntlPurgeDateFld2.SetFocus();
		return FALSE;
	}
	if ( myUtilsObject.IsEmpty (m_StrPurgeDateFld3, this ) && (m_Latin == 1 ) )
	{
		m_StrMessageLbl = "Please enter Purge Date";
		UpdateData(FALSE);
		m_CntlPurgeDateFld3.SetFocus();
		return FALSE;
	}
	if ( myUtilsObject.IsEmpty (m_StrPurgeDateFld4, this ) && (m_Asia == 1 ) )
	{
		m_StrMessageLbl = "Please enter Purge Date";
		UpdateData(FALSE);
		m_CntlPurgeDateFld4.SetFocus();
		return FALSE;
	}
	if ( myUtilsObject.IsEmpty (m_StrPurgeDateFld5, this ) && (m_Europe == 1 ) )
	{
		m_StrMessageLbl = "Please enter Purge Date";
		UpdateData(FALSE);
		m_CntlPurgeDateFld5.SetFocus();
		return FALSE;
	}

	if ( myUtilsObject.IsEmpty (m_StrPurgeDateFld6, this ) && (m_Africa == 1 ) )
	{
		m_StrMessageLbl = "Please enter Purge Date";
		UpdateData(FALSE);
		m_CntlPurgeDateFld6.SetFocus();
		return FALSE;
	}
	return TRUE;
}

void CMCC103::handle_response() 
{
   typedef struct
   {
      INT   on_off;
      CHAR  line[80];
   } outbuf;

   outbuf line1;
   outbuf line2;
   outbuf line3;
   outbuf line4;
   outbuf line5;
   outbuf line6;
   outbuf line7;
   outbuf line8;
   outbuf line9;
   outbuf line10;

   /* Clear the list box. */
   m_cntlAccountResults.ResetContent();

   m_strRespCode = Mcc_Gui.response_code;

   memset( &line1, 0x00, sizeof(outbuf) );
   memset( &line2, 0x00, sizeof(outbuf) );
   memset( &line3, 0x00, sizeof(outbuf) );
   memset( &line4, 0x00, sizeof(outbuf) );
   memset( &line5, 0x00, sizeof(outbuf) );
   memset( &line6, 0x00, sizeof(outbuf) );
   memset( &line7, 0x00, sizeof(outbuf) );
   memset( &line8, 0x00, sizeof(outbuf) );
   memset( &line9, 0x00, sizeof(outbuf) );
   memset( &line10,0x00, sizeof(outbuf) );

   line1.on_off = 1;
   if ( m_nDialogMode == MODE_DELETE )
   {
      sprintf( line1.line, "Account %s successfully deleted", Mcc_Gui.card_nbr );
   }
   else
   {
      if ( m_nDialogMode == MODE_INQUIRE )
         strcpy( line1.line, "Results of Inquiry:" );
      else
         strcpy( line1.line, "Successful Add/Update:" );

      sprintf( line2.line, "   Card Number: %s", Mcc_Gui.card_nbr );
      line2.on_off = 1;

      /* Get Product code */
      if ( 0 == strcmp(Mcc_Gui.product_code,"MCC") )
      {
         strcpy( line3.line, "   Product Code: MCC = MasterCard Card" );
         line3.on_off = 1;
      }
      else if ( 0 == strcmp(Mcc_Gui.product_code,"MCD") )
      {
         strcpy( line3.line, "   Product Code: MCD = MasterCard Debit Card" );
         line3.on_off = 1;
      }
      else if ( 0 == strcmp(Mcc_Gui.product_code,"MCG") )
      {
         strcpy( line3.line, "   Product Code: MCG = GOLD MasterCard Card" );
         line3.on_off = 1;
      }
      else if ( 0 == strcmp(Mcc_Gui.product_code,"MNS") )
      {
         strcpy( line3.line, "   Product Code: MNS = Non-Standard" );
         line3.on_off = 1;
      }
      else if ( 0 == strcmp(Mcc_Gui.product_code,"OTH") )
      {
         strcpy( line3.line, "   Product Code: OTH = Other" );
         line3.on_off = 1;
      }

      /* Get the Entry Reason - spell it out for operator. */
      line4.on_off = 1;
      switch( Mcc_Gui.reason )
      {
         case 'C':  strcpy( line4.line, "   Reason: C = Credit"      ); break;
         case 'F':  strcpy( line4.line, "   Reason: F = Fraud"       ); break;
         case 'O':  strcpy( line4.line, "   Reason: O = Other"       ); break;
         case 'X':  strcpy( line4.line, "   Reason: X = Counterfeit" ); break;
         default:   line4.on_off = 0;
      }

      /* Get the Region Chargeback Effective dates and Purge dates. */
      if ( Mcc_Gui.region_us.indicator == '1' )
      {
         line5.on_off = 1;
         if ( Mcc_Gui.region_us.chargeback_date[0] != 0x00 )
         {
            change_date_yymmdd_to_ddmmyy(Mcc_Gui.region_us.chargeback_date);
            change_date_yymmdd_to_ddmmyy(Mcc_Gui.region_us.purge_date);
            sprintf( line5.line,
                    "   Region USA: Chargeback Effective Date %s,   Purge Date %s",
                     Mcc_Gui.region_us.chargeback_date,
                     Mcc_Gui.region_us.purge_date );
         }
         else
         {
            change_date_yymmdd_to_ddmmyy(Mcc_Gui.region_us.purge_date);
            sprintf( line5.line,
                    "   Region USA: Purge Date %s",
                     Mcc_Gui.region_us.purge_date );
         }
      }

      if ( Mcc_Gui.region_canada.indicator == 'A' )
      {
         line6.on_off = 1;
         if ( Mcc_Gui.region_us.chargeback_date[0] != 0x00 )
         {
            change_date_yymmdd_to_ddmmyy(Mcc_Gui.region_canada.chargeback_date);
            change_date_yymmdd_to_ddmmyy(Mcc_Gui.region_canada.purge_date);
            sprintf( line6.line,
                    "   Region Canada: Chargeback Effective Date %s,   Purge Date %s",
                     Mcc_Gui.region_canada.chargeback_date,
                     Mcc_Gui.region_canada.purge_date );
         }
         else
         {
            change_date_yymmdd_to_ddmmyy(Mcc_Gui.region_canada.purge_date);
            sprintf( line6.line,
                    "   Region Canada: Purge Date %s",
                     Mcc_Gui.region_canada.purge_date );
         }
      }

      if ( Mcc_Gui.region_sa.indicator == 'B' )
      {
         line7.on_off = 1;
         if ( Mcc_Gui.region_us.chargeback_date[0] != 0x00 )
         {
            change_date_yymmdd_to_ddmmyy(Mcc_Gui.region_sa.chargeback_date);
            change_date_yymmdd_to_ddmmyy(Mcc_Gui.region_sa.purge_date);
            sprintf( line7.line,
                    "   Region Latin: Chargeback Effective Date %s,   Purge Date %s",
                     Mcc_Gui.region_sa.chargeback_date,
                     Mcc_Gui.region_sa.purge_date );
         }
         else
         {
            change_date_yymmdd_to_ddmmyy(Mcc_Gui.region_sa.purge_date);
            sprintf( line7.line,
                    "   Region Latin: Purge Date %s",
                     Mcc_Gui.region_sa.purge_date );
         }
      }

      if ( Mcc_Gui.region_asia.indicator == 'C' )
      {
         line8.on_off = 1;
         if ( Mcc_Gui.region_us.chargeback_date[0] != 0x00 )
         {
            change_date_yymmdd_to_ddmmyy(Mcc_Gui.region_asia.chargeback_date);
            change_date_yymmdd_to_ddmmyy(Mcc_Gui.region_asia.purge_date);
            sprintf( line8.line,
                    "   Region Asia: Chargeback Effective Date %s,   Purge Date %s",
                     Mcc_Gui.region_asia.chargeback_date,
                     Mcc_Gui.region_asia.purge_date );
         }
         else
         {
            change_date_yymmdd_to_ddmmyy(Mcc_Gui.region_asia.purge_date);
            sprintf( line8.line,
                    "   Region Asia: Purge Date %s",
                     Mcc_Gui.region_asia.purge_date );
         }
      }

      if ( Mcc_Gui.region_europe.indicator == 'D' )
      {
         line9.on_off = 1;
         if ( Mcc_Gui.region_us.chargeback_date[0] != 0x00 )
         {
            change_date_yymmdd_to_ddmmyy(Mcc_Gui.region_europe.chargeback_date);
            change_date_yymmdd_to_ddmmyy(Mcc_Gui.region_europe.purge_date);
            sprintf( line9.line,
                    "   Region Europe: Chargeback Effective Date %s,   Purge Date %s",
                     Mcc_Gui.region_europe.chargeback_date,
                     Mcc_Gui.region_europe.purge_date );
         }
         else
         {
            change_date_yymmdd_to_ddmmyy(Mcc_Gui.region_europe.purge_date);
            sprintf( line9.line,
                    "   Region Europe: Purge Date %s",
                     Mcc_Gui.region_europe.purge_date );
         }
      }

      if ( Mcc_Gui.region_africa.indicator == 'E' )
      {
         line10.on_off = 1;
         if ( Mcc_Gui.region_us.chargeback_date[0] != 0x00 )
         {
            change_date_yymmdd_to_ddmmyy(Mcc_Gui.region_africa.chargeback_date);
            change_date_yymmdd_to_ddmmyy(Mcc_Gui.region_africa.purge_date);
            sprintf( line10.line,
                    "   Region Africa: Chargeback Effective Date %s,   Purge Date %s",
                     Mcc_Gui.region_africa.chargeback_date,
                     Mcc_Gui.region_africa.purge_date );
         }
         else
         {
            change_date_yymmdd_to_ddmmyy(Mcc_Gui.region_africa.purge_date);
            sprintf( line10.line,
                    "   Region Africa: Purge Date %s",
                     Mcc_Gui.region_africa.purge_date );
         }
      }
   }

   /* Paste the results into the Account Results field. */
   if ( line1.on_off == 1 )
      m_cntlAccountResults.AddString(line1.line);

   if ( line2.on_off == 1 )
      m_cntlAccountResults.AddString(line2.line);

   if ( line3.on_off == 1 )
      m_cntlAccountResults.AddString(line3.line);

   if ( line4.on_off == 1 )
      m_cntlAccountResults.AddString(line4.line);

   /* Snhow the date format - ddmmyy. */
   if ( (line5.on_off == 1) ||
        (line6.on_off == 1) ||
        (line7.on_off == 1) ||
        (line8.on_off == 1) ||
        (line9.on_off == 1) ||
        (line10.on_off== 1) )
   {
      m_cntlAccountResults.AddString("  ");
      m_cntlAccountResults.AddString("   -- Date Format: DDMMYY --");
   }

   if ( line5.on_off == 1 )
      m_cntlAccountResults.AddString(line5.line);

   if ( line6.on_off == 1 )
      m_cntlAccountResults.AddString(line6.line);

   if ( line7.on_off == 1 )
      m_cntlAccountResults.AddString(line7.line);

   if ( line8.on_off == 1 )
      m_cntlAccountResults.AddString(line8.line);

   if ( line9.on_off == 1 )
      m_cntlAccountResults.AddString(line9.line);

   if ( line10.on_off == 1 )
      m_cntlAccountResults.AddString(line10.line);

   return;
}

void CMCC103::change_date_yymmdd_to_ddmmyy( pCHAR date_buf ) 
{
   CHAR   day[3] = "";
   CHAR   year[3]  = "";

   memcpy( day, date_buf+4, 2 );
   memcpy( year,date_buf,   2 );

   memcpy( date_buf,   day,  2 );
   memcpy( date_buf+4, year, 2 );
   return;
}


void CMCC103::OnExit() 
{
	// TODO: Add your control notification handler code here
	//StopTimer();
	CDialog::OnCancel();
}

BOOL CMCC103::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	// TODO: Add your specialized code here and/or call the base class
	c_time = CTime::GetCurrentTime();	
	shrmem.SetLastUpdateTime(c_time);
	return CDialog::OnCommand(wParam, lParam);
}
