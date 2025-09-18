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
* $Log:   N:\PVCS\PTE\Equitable\ATP_gui\MCC102.cpp  $
   
	Rev        April 28.4.11          Krishna Kiran S@TF
   Added Idle Time Calculation & calling Login screen on Idle Timeout

      Rev 1.20   Dec 01 2003 11:29:30   lmitchel
   Added AMEX Online Updates - scr1200
   
      Rev 1.19   Oct 09 2003 14:49:28   dirby
   Modified to get terminal Id and pass it to the network controller.
   SCR 1224
   
      Rev 1.18   Jul 01 2003 10:36:56   lmitchel
   SCR1138:  Ver: 4.2.0.3: Enabled ADD, DELETE, UPDATE, INQUIRY access for Account File, Account Management File, ad JCB Online Updates when Update and View security permissions are enabled
   
      Rev 1.17   Mar 04 2003 17:39:16   dirby
   Modified to make the check digit field default to ON.
   SCR 891
   
      Rev 1.16   Mar 03 2003 11:45:48   dirby
   Made many changes to support a more user friendly GUI.
   Removed the Inquiry results field, added Account Results,
   Error Results, and Check Digit.  Modified code to handle
   response info better.
   SCR 891
   
      Rev 1.15   Jul 29 2002 17:44:02   dirby
   Modified to copy workstation and operator Id into the MCC structure.
   This is so they get stored into TLF01.
   SCR 575
   
      Rev 1.14   Jun 21 2000 14:25:40   lmitchel
   Modifed AlphaLookup.cpp and repds to select and display chain name(mcf01.name02) in merchant lookup list.
   Added 'Update' button to account file screen(mcc102.cpp)
   
      Rev 1.13   Mar 02 2000 11:56:36   iarustam
   SCR # 425,21,383,407,459,300,460,461
   
      Rev 1.12   Jan 21 2000 11:51:58   mbysani
   BUG FIXES
   
      Rev 1.11   Jan 13 2000 17:07:44   mbysani
   bug fixed
   
      Rev 1.10   Dec 14 1999 10:54:38   mbysani
    
   
      Rev 1.1   Sep 22 1999 15:50:56   iarustam
   Added manual batch entry GUI
   
      Rev 1.0   Jul 01 1999 14:17:34   iarustam
   Initial Release
   
      Rev 1.0   Jun 28 1999 13:58:38   iarustam
   Initial Release
   
      Rev 1.6   Jun 17 1999 17:04:18   iarustam
   Bug fixes
   
      Rev 1.5   Jun 11 1999 14:07:44   iarustam
   Bug fixes
   
   
      Rev 1.4   Jun 04 1999 17:40:26   iarustam
   Bug fixes
   
      Rev 1.3   May 10 1999 11:21:02   dperson
   Added PVCS header and
   standardized error messages
*   
************************************************************************************/
// MCC102.cpp : implementation file
//

#include "stdafx.h"
#include "atp.h"
#include "MCC102.h"
#include "MyUtils.h"
//#include "Login.h"

#include "SharedMem.h"


extern CSharedMem shrmem;
extern "C"
{
   #include "ntutils.h"
}

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define ERR_LINE_LEN  60

extern UCF01 ucf01;
extern CString workstation;
//extern CLogin login;
/////////////////////////////////////////////////////////////////////////////
// CMCC102 dialog


CMCC102::CMCC102(CWnd* pParent /*=NULL*/)
	: CDialog(CMCC102::IDD, pParent)
{
	//{{AFX_DATA_INIT(CMCC102)
	m_StrAccountNumberFld = _T("");
	m_StrDateFld = _T("");
	m_StrEntryReason = _T("");
	m_StrTimeFld = _T("");
	m_StrVIPAmountFld = _T("");
	m_StrVIPCurrencyFld = _T("");
	m_StrMessageLbl = _T("");
	m_strRespCode = _T("");
	m_CkDigit = FALSE;
	m_strAccountResults = _T("");
	m_strErrorResults = _T("");
	//}}AFX_DATA_INIT
}


void CMCC102::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMCC102)
	DDX_Control(pDX, IDC_UPDATE_BUTTON, m_UpdateButton);
	DDX_Control(pDX, IDC_ERROR_RESULTS, m_cntlErrorResults);
	DDX_Control(pDX, IDC_ACCOUNT_RESULTS, m_cntlAccountResults);
	DDX_Control(pDX, IDC_CKDIGIT, m_cntlCkDigit);
	DDX_Control(pDX, IDC_RESP_CODE2, m_cntlRespCode);
	DDX_Control(pDX, IDC_INQUIRE_BUTTON, m_InquiryButton);
	DDX_Control(pDX, IDC_ADD_BUTTON, m_AddButton);
	DDX_Control(pDX, IDC_DELETE_BUTTON, m_DeleteButton);
	DDX_Control(pDX, IDC_VIP_CURRENCY, m_CntlVIPCurrency);
	DDX_Control(pDX, IDC_VIP_AMOUNT_FLD, m_CntlVIPAmountFld);
	DDX_Control(pDX, IDC_ENTRY_REASON, m_CntlEntryReason);
	DDX_Control(pDX, IDC_TIME_FLD, m_CntlTimeFld);
	DDX_Control(pDX, IDC_DATE_FLD, m_CntlDateFld);
	DDX_Control(pDX, IDC_ACCOUNT_NUMBER_FLD, m_CntlAccountNumberFld);
	DDX_Text(pDX, IDC_ACCOUNT_NUMBER_FLD, m_StrAccountNumberFld);
	DDV_MaxChars(pDX, m_StrAccountNumberFld, 19);
	DDX_Text(pDX, IDC_DATE_FLD, m_StrDateFld);
	DDV_MaxChars(pDX, m_StrDateFld, 8);
	DDX_CBString(pDX, IDC_ENTRY_REASON, m_StrEntryReason);
	DDX_Text(pDX, IDC_TIME_FLD, m_StrTimeFld);
	DDV_MaxChars(pDX, m_StrTimeFld, 4);
	DDX_Text(pDX, IDC_VIP_AMOUNT_FLD, m_StrVIPAmountFld);
	DDV_MaxChars(pDX, m_StrVIPAmountFld, 10);
	DDX_CBString(pDX, IDC_VIP_CURRENCY, m_StrVIPCurrencyFld);
	DDV_MaxChars(pDX, m_StrVIPCurrencyFld, 3);
	DDX_Text(pDX, IDC_MESSAGE_LBL, m_StrMessageLbl);
	DDX_Text(pDX, IDC_RESP_CODE2, m_strRespCode);
	DDX_Check(pDX, IDC_CKDIGIT, m_CkDigit);
	DDX_LBString(pDX, IDC_ACCOUNT_RESULTS, m_strAccountResults);
	DDX_LBString(pDX, IDC_ERROR_RESULTS, m_strErrorResults);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CMCC102, CDialog)
	//{{AFX_MSG_MAP(CMCC102)
	ON_BN_CLICKED(IDC_ADD_BUTTON, OnAddButton)
	ON_BN_CLICKED(IDC_DELETE_BUTTON, OnDeleteButton)
	ON_BN_CLICKED(IDC_INQUIRE_BUTTON, OnInquireButton)
	ON_CBN_SELCHANGE(IDC_ENTRY_REASON, OnSelchangeEntryReason)
	ON_BN_CLICKED(IDC_UPDATE_BUTTON, OnUpdateButton)
	ON_BN_CLICKED(IDOK, OnExit)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMCC102 message handlers

BOOL CMCC102::OnInitDialog() 
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

	m_CntlVIPAmountFld.EnableWindow(FALSE);
	m_CntlVIPCurrency.EnableWindow(FALSE);

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

	if ( nIndex != -1)
	{
		if ( '1' == ucf01.control[nIndex].action[0]  &&
           ( '1' == ucf01.control[nIndex].action[1]  ) &&
           ( '1' == ucf01.control[nIndex].action[2]  ) &&
           ( '1' == ucf01.control[nIndex].action[3]  ) )

		{
			m_CntlAccountNumberFld.EnableWindow(TRUE);
            m_cntlCkDigit.EnableWindow(TRUE);
            m_CkDigit = TRUE;
			m_CntlDateFld.EnableWindow(TRUE);
			m_CntlEntryReason.EnableWindow(TRUE);
			m_CntlTimeFld.EnableWindow(TRUE);
			m_CntlVIPAmountFld.EnableWindow(TRUE);
			m_CntlVIPCurrency.EnableWindow(TRUE);
			m_AddButton.EnableWindow(TRUE);
			m_UpdateButton.EnableWindow(TRUE);
			m_DeleteButton.EnableWindow(TRUE);
			m_InquiryButton.EnableWindow(TRUE);
		}
		else if ( '0' == ucf01.control[nIndex].action[0]  &&
         ( '1' == ucf01.control[nIndex].action[1]  ) &&
         ( '0' == ucf01.control[nIndex].action[2]  ) &&
         ( '1' == ucf01.control[nIndex].action[3]  ) )

		{
			m_CntlAccountNumberFld.EnableWindow(TRUE);
            m_cntlCkDigit.EnableWindow(TRUE);
            m_CkDigit = TRUE;
			m_CntlDateFld.EnableWindow(TRUE);
			m_CntlEntryReason.EnableWindow(TRUE);
			m_CntlTimeFld.EnableWindow(TRUE);
			m_CntlVIPAmountFld.EnableWindow(TRUE);
			m_CntlVIPCurrency.EnableWindow(TRUE);
			m_AddButton.EnableWindow(FALSE);
//			m_UpdateButton.EnableWindow(TRUE);
			m_DeleteButton.EnableWindow(TRUE);
			m_InquiryButton.EnableWindow(FALSE);
		}
		else if ( '1' == ucf01.control[nIndex].action[0]  &&
         ( '0' == ucf01.control[nIndex].action[1]  ) &&
         ( '0' == ucf01.control[nIndex].action[2]  ) &&
         ( '1' == ucf01.control[nIndex].action[3]  ) )

		{
			m_CntlAccountNumberFld.EnableWindow(TRUE);
			m_cntlCkDigit.EnableWindow(TRUE);
			m_CkDigit = TRUE;
			m_CntlDateFld.EnableWindow(TRUE);
			m_CntlEntryReason.EnableWindow(TRUE);
			m_CntlTimeFld.EnableWindow(TRUE);
			m_CntlVIPAmountFld.EnableWindow(TRUE);
			m_CntlVIPCurrency.EnableWindow(TRUE);
			m_AddButton.EnableWindow(TRUE);
			m_DeleteButton.EnableWindow(FALSE);
			m_InquiryButton.EnableWindow(TRUE);
		}
		else if ( '1' == ucf01.control[nIndex].action[2] &&  /*scr1138 - Update permission gives full access 7/1/03*/
                  '1' == ucf01.control[nIndex].action[3]  ) 
		{
			m_CntlAccountNumberFld.EnableWindow(TRUE);
			m_cntlCkDigit.EnableWindow(TRUE);
			m_CkDigit = TRUE;
			m_CntlDateFld.EnableWindow(TRUE);
			m_CntlEntryReason.EnableWindow(TRUE);
			m_CntlTimeFld.EnableWindow(TRUE);
			m_CntlVIPAmountFld.EnableWindow(TRUE);
			m_CntlVIPCurrency.EnableWindow(TRUE);
			m_AddButton.EnableWindow(TRUE);
			m_DeleteButton.EnableWindow(TRUE);
			m_InquiryButton.EnableWindow(TRUE);
			m_UpdateButton.EnableWindow(TRUE);
		}
		else
		{
			m_CntlAccountNumberFld.EnableWindow(FALSE);
            m_cntlCkDigit.EnableWindow(FALSE);
            m_CkDigit = TRUE;
			m_CntlDateFld.EnableWindow(FALSE);
			m_CntlEntryReason.EnableWindow(FALSE);
			m_CntlTimeFld.EnableWindow(FALSE);
			m_CntlVIPAmountFld.EnableWindow(FALSE);
			m_CntlVIPCurrency.EnableWindow(FALSE);
			m_AddButton.EnableWindow(FALSE);
			m_UpdateButton.EnableWindow(FALSE);
			m_DeleteButton.EnableWindow(FALSE);
			m_InquiryButton.EnableWindow(FALSE);
		}

	}

   m_cntlAccountResults.ResetContent();
   m_cntlErrorResults.ResetContent();
   m_strRespCode = "";

   UpdateData(FALSE);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CMCC102::OnAddButton() 
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
         m_StrMessageLbl = "Please Wait ... Processing Add Message";
         UpdateData(FALSE);

         rcode = txdsapi_send_mcc_record(&Mcc_Gui,MCC102_DATA,strErrorMessage);
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
               m_StrMessageLbl = "Record not added to Account File";
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

void CMCC102::OnDeleteButton() 
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

         rcode = txdsapi_send_mcc_record( &Mcc_Gui, MCC102_DATA, strErrorMessage );
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
               m_StrMessageLbl = "Record not deleted from Account File";
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

void CMCC102::OnInquireButton() 
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
         rcode = txdsapi_send_mcc_record( &Mcc_Gui, MCC102_DATA, strErrorMessage );
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
               m_StrMessageLbl = "Unable to read record from Account File";
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

void CMCC102::OnUpdateButton() 
{
   CHAR strErrorMessage[200];
   INT  rcode;

   m_nDialogMode = MODE_EDIT;

   m_cntlAccountResults.ResetContent();
   m_cntlErrorResults.ResetContent();

   if ( CheckDialogForCompleteness() )
   {
      if ( PopulateDatabaseStructure() )
      {
         m_strRespCode = "";
         m_StrMessageLbl = "Please Wait ... Processing Update Message";
         UpdateData(FALSE);

         rcode = txdsapi_send_mcc_record( &Mcc_Gui, MCC102_DATA, strErrorMessage );
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
               m_StrMessageLbl = "Account File record not updated.";
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


BOOL CMCC102::PopulateDatabaseStructure()
{
	CString temp; 
	int nCount = 0;
	CMyUtils myUtilsObject;
   CHAR strDeviceID[20] = "";

	UpdateData(TRUE);

   ZeroMemory ( &Mcc_Gui, sizeof(MCC_ONLINE_UPDATES) );

   memcpy( Mcc_Gui.msg_type, "MCC102", sizeof(Mcc_Gui.msg_type) );

	if ( MODE_ADD == m_nDialogMode )
		temp = "A";
	else if ( MODE_DELETE == m_nDialogMode )
		temp = "D";
	else if ( MODE_EDIT == m_nDialogMode )
		temp = "U";
	else if ( MODE_INQUIRE == m_nDialogMode )
		temp = "I";
	

   memcpy( &Mcc_Gui.choice, temp, sizeof( Mcc_Gui.choice) );

   memcpy( &Mcc_Gui.card_nbr, m_StrAccountNumberFld, sizeof(Mcc_Gui.card_nbr) );


	if ( m_StrEntryReason == "Credit" )
		temp = "C";
	else if ( m_StrEntryReason == "Counterfeit" )
		temp = "X";
	else if ( m_StrEntryReason == "Other" )
		temp = "O";
	else if ( m_StrEntryReason == "Fraud" )
		temp = "F";
	else if ( m_StrEntryReason == "Lost" )
		temp = "L";
	else if ( m_StrEntryReason == "Stolen" )
		temp = "S";
	else if ( m_StrEntryReason == "Capture Card" )
		temp = "P";
	else if ( m_StrEntryReason == "Unauthorized Use" )
		temp = "U";
	else if ( m_StrEntryReason == "VIP" )
		temp = "V";
	else if ( m_StrEntryReason == "Gold MasterCard Card" )
		temp = "G";

   memcpy( &Mcc_Gui.reason, temp, sizeof(Mcc_Gui.reason) );

	if ( !ValidateDate ( m_StrDateFld ))
	{
		m_CntlDateFld.SetFocus();
		return FALSE;
	}

   m_StrDateFld = m_StrDateFld.Left(4) + m_StrDateFld.Right(2);
   memcpy( &Mcc_Gui.activity_date, m_StrDateFld, sizeof(Mcc_Gui.activity_date));

	if ( !ValidateTime ( m_StrTimeFld ))
	{
		m_CntlTimeFld.SetFocus();
		return FALSE;
	}

   memcpy( &Mcc_Gui.activity_time, m_StrTimeFld, sizeof(Mcc_Gui.activity_time));

	if ( !myUtilsObject.IsEmpty( m_StrVIPAmountFld, this ) )
		temp = myUtilsObject.GenerateAmount (m_StrVIPAmountFld, 12);
	else
		temp = m_StrVIPAmountFld;

   memcpy( &Mcc_Gui.vip_limit_amt, temp, sizeof(Mcc_Gui.vip_limit_amt) );

   memcpy( &Mcc_Gui.vip_currency_code, m_StrVIPCurrencyFld, sizeof(Mcc_Gui.vip_currency_code));

   /* Get Terminal ID from TF.INI file. */
   GetPrivateField1(strDeviceID);
   memcpy( Mcc_Gui.terminal_id, strDeviceID, 8 );

   /* Get Workstation and Operator ID for logging to TLF01. */
   memcpy( &Mcc_Gui.workstation, workstation, 20 );
   memcpy( &Mcc_Gui.operator_id, ucf01.primary_key.name, 20 );

	return TRUE;
}

BOOL CMCC102::ValidateTime ( CString strTime )
{
	CString strHH;
	CString strMM;
	int nHH, nMM;
	UpdateData(TRUE);
	CMyUtils myUtilsObject;

	if ( myUtilsObject.IsEmpty ( strTime, this ))
		return TRUE;

	int length = strTime.GetLength();	

	if ( length < 4 )
	{
		m_StrMessageLbl = "ERROR: Invalid time format";
		UpdateData(FALSE);
		return FALSE;
	}

	strHH = strTime.Left(2);
	strMM = strTime.Right(2);

	nHH = atoi ( strHH );
	nMM = atoi ( strMM );

	if ( nHH == 0 && nMM == 0 )
	{
		m_StrMessageLbl = "ERROR: Invalid data";
		UpdateData(FALSE);
		return FALSE;
	}

	if ( nHH > 23 || nMM >59 )
	{
		m_StrMessageLbl = "ERROR: Invalid data";
		UpdateData(FALSE);
		return FALSE;
	}

	return TRUE;

}

BOOL CMCC102::ValidateDate ( CString strDate )
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

void CMCC102::OnSelchangeEntryReason() 
{
	UpdateData(TRUE);
	if ( m_StrEntryReason == "VIP" )
	{
		m_CntlVIPAmountFld.EnableWindow(TRUE);
		m_CntlVIPCurrency.EnableWindow(TRUE);
	}
	else
	{
		m_CntlVIPAmountFld.EnableWindow(FALSE);
		m_CntlVIPCurrency.EnableWindow(FALSE);
		m_StrVIPAmountFld = "";
      m_StrVIPCurrencyFld = "";
		m_CntlVIPCurrency.SetCurSel(-1);
      UpdateData(FALSE);

	}
}

BOOL CMCC102::CheckDialogForCompleteness()
{
   CMyUtils myUtilsObject;
   CHAR     CardNbr[20] = "";
   INT      verify_digit = 0;
   INT      length = 0;

   UpdateData(TRUE);

   if ( myUtilsObject.IsEmpty (m_StrAccountNumberFld, this )) 
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

   if ( (m_nDialogMode != MODE_INQUIRE) && (m_nDialogMode != MODE_DELETE) )
   {
      if ( myUtilsObject.IsEmpty (m_StrEntryReason, this ) )
      {
         m_StrMessageLbl = "Please enter Entry Reason";
         UpdateData(FALSE);
         m_CntlEntryReason.SetFocus();
         return FALSE;
      }

      if ( myUtilsObject.IsEmpty (m_StrDateFld, this ) )
      {
         m_StrMessageLbl = "Please enter date";
         UpdateData(FALSE);
         m_CntlDateFld.SetFocus();
         return FALSE;
      }

      if ( myUtilsObject.IsEmpty (m_StrTimeFld, this )) 
      {
         m_StrMessageLbl = "Please enter time";
         UpdateData(FALSE);
         m_CntlTimeFld.SetFocus();
         return FALSE;
      }
   }

   return TRUE;

}

void CMCC102::handle_response() 
{
   typedef struct
   {
      INT   on_off;
      CHAR  line[50];
   } outbuf;

   INT  cc;
   outbuf line1;
   outbuf line2;
   outbuf line3;
   outbuf line4;
   outbuf line5;
   outbuf line6;
   outbuf line7;


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

   line1.on_off = 1;
   if ( m_nDialogMode == MODE_DELETE )
   {
      sprintf( line1.line, "Account %s successfully deleted", Mcc_Gui.card_nbr );
   }
   else
   {
      if ( m_nDialogMode == MODE_INQUIRE )
         strcpy( line1.line, "Results of Inquiry:" );
      else if ( m_nDialogMode == MODE_ADD )
         strcpy( line1.line, "Successful Add:" );
      else
         strcpy( line1.line, "Successful Update:" );

      sprintf( line2.line, "   Card Number: %s", Mcc_Gui.card_nbr );
      line2.on_off = 1;

      /* Get the Entry Reason - spell it out for operator. */
      if ( Mcc_Gui.reason != 0x00 )
      {
         switch( Mcc_Gui.reason )
         {
            case 'C':  strcpy( line3.line, "   Reason: C = Credit"               ); break;
            case 'F':  strcpy( line3.line, "   Reason: F = Fraud"                ); break;
            case 'G':  strcpy( line3.line, "   Reason: G = Gold MasterCard Card" ); break;
            case 'L':  strcpy( line3.line, "   Reason: L = Lost"                 ); break;
            case 'O':  strcpy( line3.line, "   Reason: O = Other"                ); break;
            case 'P':  strcpy( line3.line, "   Reason: P = Capture Card"         ); break;
            case 'S':  strcpy( line3.line, "   Reason: S = Stolen"               ); break;
            case 'U':  strcpy( line3.line, "   Reason: U = Unauthorized Use"     ); break;
            case 'V':  strcpy( line3.line, "   Reason: V = VIP"                  ); break;
            case 'X':  strcpy( line3.line, "   Reason: X = Counterfeit"          ); break;

            default:   if ( 0 != strcmp(Mcc_Gui.response_code,"00") )
                          sprintf( line3.line,
                                   "   Unknown entry reason: %c",
                                   Mcc_Gui.reason);
                       break;
         }
         line3.on_off = 1;
      }

      if ( Mcc_Gui.activity_date[0] != 0x00 )
      {
         sprintf(line4.line, "   Date of last update activity: %s", Mcc_Gui.activity_date);
         line4.on_off = 1;
      }

      if ( Mcc_Gui.activity_time[0] != 0x00 )
      {
         sprintf(line5.line, "   Time of last update activity: %s", Mcc_Gui.activity_time);
         line5.on_off = 1;
      }

      /* Is there VIP information? */
      cc = atoi( Mcc_Gui.vip_currency_code );
      if ( cc > 0 )
      {
         /* Yes */
         sprintf( line6.line, "   VIP Accumulative Limit  = %s", Mcc_Gui.vip_limit_amt );
         sprintf( line7.line, "   VIP Limit Currency Code = %d", cc );
         line6.on_off = 1;
         line7.on_off = 1;
      }
   }

   /* Paste the results into the InquiryResults field. */
   if ( line1.on_off == 1 )
      m_cntlAccountResults.AddString(line1.line);

   if ( line2.on_off == 1 )
      m_cntlAccountResults.AddString(line2.line);

   if ( line3.on_off == 1 )
      m_cntlAccountResults.AddString(line3.line);

   if ( line4.on_off == 1 )
      m_cntlAccountResults.AddString(line4.line);

   if ( line5.on_off == 1 )
      m_cntlAccountResults.AddString(line5.line);

   if ( line6.on_off == 1 )
      m_cntlAccountResults.AddString(line6.line);

   if ( line7.on_off == 1 )
      m_cntlAccountResults.AddString(line7.line);

   return;
}


void CMCC102::OnExit() 
{
	// TODO: Add your control notification handler code here
	//StopTimer();
	CDialog::OnCancel();
	
}

BOOL CMCC102::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	// TODO: Add your specialized code here and/or call the base class
	
	c_time = CTime::GetCurrentTime();	
	shrmem.SetLastUpdateTime(c_time);
	return CDialog::OnCommand(wParam, lParam);
}
