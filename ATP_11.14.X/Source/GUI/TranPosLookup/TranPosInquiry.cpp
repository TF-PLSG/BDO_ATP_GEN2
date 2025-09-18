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
* $Log:   N:\PVCS\PTE\Equitable\tran_lookup\TranPosInquiry.cpp  $
   
      Rev 1.0   Jan 25 2011  16:55:18   TF-Sriaknth
   Added Idle Time of TranPosInquiry
   
     
*
************************************************************************************/

// TranPosInquiry.cpp : implementation file
//

#include "stdafx.h"
#include "TranPosSearch.h"
#include "TranPosInquiry.h"
#include "transactiondetailsPos.h"
#include "MyUtils.h"
#include "Loginfrm.h"
#include "SharedMem.h"

extern "C"
{
   #include "ntutils.h"

}

	BOOL m_bTran;
	BOOL m_bFGuard;
	
	INT ST2_TYPE = 0;

CLoginfrm loginfrm;
CString idle_time_val_str;
CSharedMem shrmem;

extern CString strUserName;
extern int idleTime;
extern char strFileRecord[56];

/////////////////////////////////////////////////////////////////////////////
// CTranPosInquiry dialog


CTranPosInquiry::CTranPosInquiry(CWnd* pParent /*=NULL*/)
	: CDialog(CTranPosInquiry::IDD, pParent)
{
	//{{AFX_DATA_INIT(CTranPosInquiry)
	m_StrCardNumberFld = _T("");
	m_StrAmountFld = _T("");
	m_StrDeviceIDFld = _T("");
	m_StrMerchantIDFld = _T("");
	m_StrMessageLbl = _T("");
	m_StrDateEndFld = _T("");
	m_StrTimeEndFld = _T("");
	m_StrTimeStartFld = _T("");
	m_StrDateStartFld = _T("");
	m_StrViewFGRawRadio = -1;
	m_StrViewAcceptedRadio = -1;
	m_StrViewFailedRadio = -1;
	m_StrEmvRawRadio = -1;
	m_StrFGuardSearchCheck = FALSE;
	m_StrPosSearchCheck = FALSE;
	//}}AFX_DATA_INIT

	//Instantiate the Object for SHM
	pLock = new CDMLock("IdleLock");
}

CTranPosInquiry::~CTranPosInquiry()
{

  	delete pLock;
   pLock = NULL;

}
void CTranPosInquiry::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTranPosInquiry)
	DDX_Control(pDX, IDC_MASK_CARD_NUMBER_CHK, m_MaskCardNumber);
	DDX_Control(pDX, IDC_POS_SEARCH_CHECK, m_CntlPosSearchCheck);
	DDX_Control(pDX, IDC_FGUARD_SEARCH_CHECK, m_CntlFGuardSearchCheck);
	DDX_Control(pDX, IDC_EMV_RAW_RADIO, m_CntlEmvRawRadio);
	DDX_Control(pDX, IDC_VIEW_FAILED_RADIO, m_CntlViewFailedRadio);
	DDX_Control(pDX, IDC_VIEW_ACCEPTED_RADIO, m_CntlViewAcceptedRadio);
	DDX_Control(pDX, IDC_VIEW_FGRAW_RADIO, m_CntlViewFGRawRadio);
	DDX_Control(pDX, IDC_VIEW_ALLRADIO, m_CntlViewAllRadio);
	DDX_Control(pDX, IDC_RESEND_BUTTON, m_CntlResendButton);
	DDX_Control(pDX, IDC_NEXT_BUTTON, m_NextButton);
	DDX_Control(pDX, IDC_INQUIRY_BUTTON, m_InquiryButton);
	DDX_Control(pDX, IDC_DETAILS_BUTTON, m_DetailsButton);
	DDX_Control(pDX, IDC_TIME_FLD, m_CntlTimeStartFld);
	DDX_Control(pDX, IDC_TIME_END_FLD, m_CntlTimeEndFld);
	DDX_Control(pDX, IDC_DATE_FLD, m_CntlDateStartFld);
	DDX_Control(pDX, IDC_DATE_END_FLD, m_CntlDateEndFld);
	DDX_Control(pDX, IDC_MERCHANT_ID_FLD, m_CntlMerchantIDFld);
	DDX_Control(pDX, IDC_LIST, m_CntlList);
	DDX_Control(pDX, IDC_DEVICE_ID_FLD, m_CntlDeviceIDFld);
	DDX_Control(pDX, IDC_ACCOUNT_NBR_FLD, m_CntlCardNumberFld);
	DDX_Text(pDX, IDC_ACCOUNT_NBR_FLD, m_StrCardNumberFld);
	DDV_MaxChars(pDX, m_StrCardNumberFld, 6);
	DDX_Text(pDX, IDC_DEVICE_ID_FLD, m_StrDeviceIDFld);
	DDV_MaxChars(pDX, m_StrDeviceIDFld, 8);
	DDX_Text(pDX, IDC_MERCHANT_ID_FLD, m_StrMerchantIDFld);
	DDV_MaxChars(pDX, m_StrMerchantIDFld, 15);
	DDX_Text(pDX, IDC_MESSAGE_LBL, m_StrMessageLbl);
	DDX_Text(pDX, IDC_DATE_END_FLD, m_StrDateEndFld);
	DDV_MaxChars(pDX, m_StrDateEndFld, 8);
	DDX_Text(pDX, IDC_TIME_END_FLD, m_StrTimeEndFld);
	DDV_MaxChars(pDX, m_StrTimeEndFld, 4);
	DDX_Text(pDX, IDC_TIME_FLD, m_StrTimeStartFld);
	DDV_MaxChars(pDX, m_StrTimeStartFld, 4);
	DDX_Text(pDX, IDC_DATE_FLD, m_StrDateStartFld);
	DDV_MaxChars(pDX, m_StrDateStartFld, 8);
	DDX_Check(pDX, IDC_FGUARD_SEARCH_CHECK, m_StrFGuardSearchCheck);
	DDX_Check(pDX, IDC_POS_SEARCH_CHECK, m_StrPosSearchCheck);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CTranPosInquiry, CDialog)
	//{{AFX_MSG_MAP(CTranPosInquiry)
	ON_BN_CLICKED(IDC_INQUIRY_BUTTON, OnInquiryButton)
	ON_BN_CLICKED(IDC_NEXT_BUTTON, OnNextButton)
	ON_BN_CLICKED(IDC_DETAILS_BUTTON, OnDetailsButton)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST, OnDblclkList)
	ON_BN_CLICKED(IDC_EMV_RAW_RADIO, OnEMVRAWRadio)
	ON_BN_CLICKED(IDC_POS_SEARCH_CHECK, OnPosSearchCheck)
	ON_BN_CLICKED(IDC_VIEW_ALLRADIO, OnViewAllradio)
	ON_BN_CLICKED(IDC_VIEW_ACCEPTED_RADIO, OnViewAcceptedRadio)
	ON_BN_CLICKED(IDC_VIEW_FAILED_RADIO, OnViewFailedRadio)
	ON_BN_CLICKED(IDC_RESEND_BUTTON, OnResendButton)
	ON_BN_CLICKED(IDC_BUTTON_EXIT, OnButtonExit)
	ON_BN_CLICKED(IDC_MASK_CARD_NUMBER_CHK, OnMaskCardNumberChk)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTranPosInquiry message handlers

BOOL CTranPosInquiry::OnInitDialog() 
{
   CDialog::OnInitDialog();
   CFont m_font;
   LOGFONT lf;
   int nHeader;
   int nIndex = -1;
   int rcode;
   char strHeaderData[14][50];
   LV_COLUMN lvColumn ;
   UCF01 sUCFRecord;
   CString temp;
   char strErrorMessage[260] = "";
   BOOL bDoneLoop = FALSE;
   BOOL retval    = FALSE;

   char ini_file[256]={'\0'};

   CString title_text, title_buffer;

   //Krishna, 10.4.11 : Below changes are made for Idle time calculation, 
   //displaying the Login Screen & blocking other screens from editing.

   	GetAscendentConfigDirectory( ini_file );
	strcat( ini_file, "tf.ini" );

	//Get the Idle TIme from tf.ini
	GetPrivateProfileString(
        "GUI", 	                   // points to section name 
        "IDLE_TIMEOUT_VALUE",	   // points to key name 
        "600",					   // points to default string 
        idle_time_val,             // points to destination buffer 
        sizeof(idle_time_val) - 1, // size of destination buffer 
        ini_file );                // points to initialization filename 
		 
	    idle_time_val[sizeof(idle_time_val)] = 0; //Make sure string is terminated
		
		idle_time = atoi(idle_time_val);

	//Start the Timer
	timer_handle = StartTimer(1000);

    if (timer_handle == 0)
    {
          AfxMessageBox("Unable to obtain timer");
    }

   shrmem.CreateMutexLock();
   shrmem.CreateSharedMemorySegment();
   shrmem.MapSharedMemorySegment();
   
   //Krishna : Get the Current time & store it in SHM
	c_time = CTime::GetCurrentTime();	
	shrmem.SetLastUpdateTime(c_time);

   memset ( &lf, 0, sizeof (lf));
   lf.lfHeight = 13;
   lf.lfWeight = 700;
   strcpy(lf.lfFaceName, "Arial");
   m_font.CreateFontIndirect (&lf);
   GetDlgItem(IDC_MESSAGE_LBL)->SetFont (&m_font);	

   memcpy (strHeaderData[0], "Card Number",    sizeof (strHeaderData[0]) );
   memcpy (strHeaderData[1], "Merchant ID",    sizeof (strHeaderData[1]) );
   memcpy (strHeaderData[2], "Device ID",      sizeof (strHeaderData[2]) );
   memcpy (strHeaderData[3], "Tran Date",      sizeof (strHeaderData[3]) );
   memcpy (strHeaderData[4], "Tran Time",      sizeof (strHeaderData[4]) );
   memcpy (strHeaderData[5], "Resp Code",      sizeof (strHeaderData[6]) );
   memcpy (strHeaderData[6], "Sys Trace Nbr",  sizeof (strHeaderData[9]) );
   memcpy (strHeaderData[7],"Ref Nbr",        sizeof (strHeaderData[10]));
   memcpy (strHeaderData[8],"Transaction ID", sizeof (strHeaderData[11]));
   memcpy (strHeaderData[9],"System Date",    sizeof (strHeaderData[12]));

   for (nHeader = 0; nHeader < 10; nHeader++)
   {
      lvColumn.mask = LVCF_FMT | LVCF_SUBITEM | LVCF_TEXT  ;
      lvColumn.fmt = LVCFMT_LEFT ;
      lvColumn.pszText = strHeaderData[nHeader] ;
      lvColumn.iSubItem = nHeader ;

      m_CntlList.InsertColumn(nHeader, &lvColumn) ;  // assumes return value is OK.
      m_CntlList.SetColumnWidth (0,  LVSCW_AUTOSIZE_USEHEADER) ;
      m_CntlList.SetColumnWidth (1,  LVSCW_AUTOSIZE_USEHEADER) ;
      m_CntlList.SetColumnWidth (2,  LVSCW_AUTOSIZE_USEHEADER) ;
      m_CntlList.SetColumnWidth (3,  LVSCW_AUTOSIZE_USEHEADER) ;
      m_CntlList.SetColumnWidth (4,  LVSCW_AUTOSIZE_USEHEADER) ;
      m_CntlList.SetColumnWidth (5,  LVSCW_AUTOSIZE_USEHEADER) ;
      m_CntlList.SetColumnWidth (6,  LVSCW_AUTOSIZE_USEHEADER) ;
      m_CntlList.SetColumnWidth (7, LVSCW_AUTOSIZE_USEHEADER) ;
      m_CntlList.SetColumnWidth (8, LVSCW_AUTOSIZE_USEHEADER) ;
      m_CntlList.SetColumnWidth (9, LVSCW_AUTOSIZE_USEHEADER) ;
   }

   /*Set title bar with site and host id*/ 
   GetWindowText(title_text);
   title_buffer.Format("%s     (%s)", title_text, strFileRecord);
   SetWindowText(title_buffer);

   /* Initialize all boxes/controls to disabled. */
   m_CntlCardNumberFld.EnableWindow(FALSE);
   m_CntlDeviceIDFld.EnableWindow(FALSE);
   m_CntlMerchantIDFld.EnableWindow(FALSE);
   m_CntlDateEndFld.EnableWindow(FALSE);
   m_CntlTimeEndFld.EnableWindow(FALSE);
   m_CntlTimeStartFld.EnableWindow(FALSE);
   m_CntlDateStartFld.EnableWindow(FALSE);
   m_InquiryButton.EnableWindow(FALSE);
   m_DetailsButton.EnableWindow(FALSE);
   m_NextButton.EnableWindow(FALSE);
   m_CntlList.EnableWindow(FALSE);	
   m_CntlPosSearchCheck.EnableWindow(FALSE);
   m_CntlPosSearchCheck.SetCheck(CHECKED);

   m_CntlEmvRawRadio.EnableWindow(FALSE);

   m_CntlFGuardSearchCheck.EnableWindow(FALSE);
   m_CntlViewAllRadio.EnableWindow(FALSE);
   m_CntlViewAcceptedRadio.EnableWindow(FALSE);
   m_CntlViewFailedRadio.EnableWindow(FALSE);
   m_CntlViewFGRawRadio.EnableWindow(FALSE);
   m_CntlResendButton.EnableWindow(FALSE);

   // Get the access rights to Transaction Lookup screen
   memset ( &sUCFRecord, 0x00, sizeof(UCF01) );
   memcpy ( sUCFRecord.primary_key.name,
            strUserName,
            sizeof(sUCFRecord.primary_key.name) );

   rcode = txdsapi_get_record( (pBYTE)&sUCFRecord,
                                sizeof(UCF01),
                                UCF01_DATA,
                                strErrorMessage );

   if( PTEMSG_NOT_FOUND == rcode )
   {
      m_StrMessageLbl = "Invalid User ID";
      UpdateData(FALSE);
   }
   else if ( PTEMSG_INVALID_DATATYPE == rcode )
   {
      m_StrMessageLbl = "Please try again";
      UpdateData(FALSE);	
   }
   else if( PTEMSG_OK != rcode )
   {
      m_StrMessageLbl = strErrorMessage;
      UpdateData(FALSE);
   }
   else
   {
      for ( int i = 0; i <=15; i++ )
      {
         temp = sUCFRecord.control[i].subsystem_name;
         if (  temp == "Transaction Lookup" )
            nIndex = i;
      }

      if ( nIndex != -1)
      {
         /* Must have VIEW privileges to do anything in TranLookup. */
         if ( '1' == sUCFRecord.control[nIndex].action[3] )
         {
            m_CntlCardNumberFld.EnableWindow(TRUE);
            m_CntlDeviceIDFld.EnableWindow(TRUE);
            m_CntlMerchantIDFld.EnableWindow(TRUE);
			
            m_CntlDateEndFld.EnableWindow(TRUE);
            m_CntlTimeEndFld.EnableWindow(TRUE);
            m_CntlTimeStartFld.EnableWindow(TRUE);
            m_CntlDateStartFld.EnableWindow(TRUE);
            m_InquiryButton.EnableWindow(TRUE);
            m_DetailsButton.EnableWindow(FALSE);
            m_NextButton.EnableWindow(FALSE);
			m_MaskCardNumber.EnableWindow(FALSE);
            m_CntlList.EnableWindow(TRUE);	
            m_StrPosSearchCheck = true;
            m_bTran = true;
			m_MaskCardNumber.SetCheck(CHECKED);

			if(('1' == sUCFRecord.control[nIndex].action[0]) ||
				('1' == sUCFRecord.control[nIndex].action[1]) ||
				('1' == sUCFRecord.control[nIndex].action[2]))
			{
				m_MaskCardNumber.EnableWindow(TRUE);
			}

            /* Must have UPDATE privileges to enable EMV/Fraudguard activities.*/
            if ( '1' == sUCFRecord.control[nIndex].action[2] )
            {
               m_CntlPosSearchCheck.EnableWindow(TRUE);
               m_CntlPosSearchCheck.SetCheck(CHECKED);
               m_CntlEmvRawRadio.EnableWindow(true);
               m_CntlFGuardSearchCheck.EnableWindow(TRUE);
               m_StrFGuardSearchCheck = false;
               m_CntlViewAllRadio.EnableWindow(FALSE);
               m_CntlViewAcceptedRadio.EnableWindow(FALSE);
               m_CntlViewFailedRadio.EnableWindow(FALSE);
               m_CntlViewFGRawRadio.EnableWindow(FALSE);
               m_CntlResendButton.EnableWindow(FALSE);
			   
            }
         }
      }

      m_bViewRawData = false;
      m_bViewEmvData = false;
      retval = TRUE;
   }
   	//set version info in the title bar
	CString appProdVer;
	CString lpszTitle;
	appProdVer.LoadString(IDS_PRODUCT_VERSION);
	this->GetWindowText(lpszTitle);
	lpszTitle = lpszTitle + " - Version - ";
	lpszTitle += appProdVer;
	this->SetWindowText(lpszTitle);
	
	
	CTime time = CTime::GetCurrentTime();
	CString timestr = time.Format("%d%m%Y");
	m_CntlDateStartFld.SetWindowText(timestr);
 	m_CntlDateEndFld.SetWindowText(timestr);
    

   return(retval);  // return TRUE unless you set the focus to a control
	                 // EXCEPTION: OCX Property Pages should return FALSE
}

void CTranPosInquiry::OnInquiryButton() 
{
   CMyUtils  myUtilsObject;

   CString strCardNum    = ""; // 19
   CString strDeviceID   = ""; //  8
   CString strMerchantID = ""; // 15
   CString strDateMin    = ""; //  8
   CString strDateMax    = ""; //  8
   CString strTimeMin    = ""; //  6
   CString strTimeMax    = ""; //  6
   CString temp;
   BYTE    all_fields_empty = true;

   m_CntlList.DeleteAllItems();

   UpdateData(TRUE);
   ZeroMemory ( &sTLFLimRecord, sizeof ( TLF01_LIMITS ) );

   /* START DATE */
   if ( !myUtilsObject.IsEmpty(m_StrDateStartFld, this) )
   {
      all_fields_empty = false;
      if ( !ValidateDate(m_StrDateStartFld) )
      {
         m_CntlDateStartFld.SetFocus();
         return;
      }

      /* Cannot have start date without an end date. */
      if ( myUtilsObject.IsEmpty(m_StrDateEndFld, this) )
      {
         m_CntlDateEndFld.SetFocus();
         m_StrMessageLbl = "Missing an End Date - required with Start Date.";
         UpdateData(FALSE);
         return;
      }
   }
   else
   {
		m_CntlDateStartFld.SetFocus();
		m_StrMessageLbl = "required Start and End Date.";
         UpdateData(FALSE);
		 return;
   }

   /* END DATE */
   if ( !myUtilsObject.IsEmpty(m_StrDateEndFld, this) )
   {
      all_fields_empty = false;
      if ( !ValidateDate(m_StrDateEndFld) )
      {
         m_CntlDateEndFld.SetFocus();
         return;
      }

      /* Cannot have end date without a start date. */
      if ( myUtilsObject.IsEmpty(m_StrDateStartFld, this) )
      {
         m_CntlDateStartFld.SetFocus();
         m_StrMessageLbl = "Missing a Start Date - required with End Date.";
         UpdateData(FALSE);
         return;
      }
   }
   else
   {
	    m_CntlDateEndFld.SetFocus();
	   m_StrMessageLbl = "required Start and End Date.";
         UpdateData(FALSE);
		 return;
   }

   /* Start Time */
   if ( !myUtilsObject.IsEmpty(m_StrTimeStartFld, this) )
   {
      all_fields_empty = false;
      if ( !ValidateTime(m_StrTimeStartFld) )
      {
         m_CntlTimeStartFld.SetFocus();
         return;
      }

      /* Cannot have a start time without an end time. */
      if ( myUtilsObject.IsEmpty(m_StrTimeEndFld, this) )
      {
         m_CntlTimeEndFld.SetFocus();
         m_StrMessageLbl = "Missing an End Time - required with a Start Time.";
         UpdateData(FALSE);
         return;
      }
   }

   /* End Time */
   if ( !myUtilsObject.IsEmpty(m_StrTimeEndFld, this) )
   {
      all_fields_empty = false;
      if ( !ValidateTime(m_StrTimeEndFld) )
      {
         m_CntlTimeEndFld.SetFocus();
         return;
      }

      /* Cannot have an end time without a start time. */
      if ( myUtilsObject.IsEmpty(m_StrTimeStartFld, this) )
      {
         m_CntlTimeStartFld.SetFocus();
         m_StrMessageLbl = "Missing a Start Time - required with an End Time.";
         UpdateData(FALSE);
         return;
      }
   }

   if ( !myUtilsObject.IsEmpty ( m_StrDateStartFld, this ) &&
        !myUtilsObject.IsEmpty ( m_StrDateEndFld,   this )  )
   {
      if ( m_StrDateStartFld.Right(4) > m_StrDateEndFld.Right(4) )
      {
         m_CntlDateStartFld.SetFocus();
         m_StrMessageLbl = "Start Date Can Not be Greater than End Date";
         UpdateData(FALSE);
         return;
      }

      if ( m_StrDateStartFld.Right(4) == m_StrDateEndFld.Right(4) && 
           m_StrDateEndFld.Mid(2,2) <  m_StrDateStartFld.Mid(2,2) )
      {
         m_CntlDateStartFld.SetFocus();
         m_StrMessageLbl = "Start Date Can Not be Greater than End Date";
         UpdateData(FALSE);
         return;
      }

      if ( m_StrDateStartFld.Right(4) == m_StrDateEndFld.Right(4) && 
           m_StrDateEndFld.Mid(2,2) ==  m_StrDateStartFld.Mid(2,2) && 
           m_StrDateEndFld.Left(2) < m_StrDateStartFld.Left(2) )
      {
         m_CntlDateStartFld.SetFocus();
         m_StrMessageLbl = "Start Date Can Not be Greater than End Date";
         UpdateData(FALSE);
         return;
      }
   }

   if ( !myUtilsObject.IsEmpty ( m_StrTimeStartFld, this ) &&
        !myUtilsObject.IsEmpty ( m_StrTimeEndFld, this ) )
   {
      if ( m_StrTimeStartFld > m_StrTimeEndFld )
      {
         m_StrMessageLbl = "Start Time Can Not be Greater than End Time";
         UpdateData(FALSE);
         return;
      }
   }

   /* ----------- */
   /* CARD NUMBER */
   /* ----------- */
   if ( !(myUtilsObject.IsEmpty(m_StrCardNumberFld, this)) ) // empty field
   {
      all_fields_empty = false;
      memcpy ( sTLFLimRecord.card_num_min,
               m_StrCardNumberFld,
               sizeof (sTLFLimRecord.card_num_min) );
   }

   int card_len=m_StrCardNumberFld.GetLength();
   if((card_len > 6) && (card_len > 0))
   {
      m_StrMessageLbl = "Please enter first 6 digit of card number like 412345";
      UpdateData(FALSE);
	  m_CntlCardNumberFld.SetFocus();
	  return;
   }
   /* ----------- */
   /* TERMINAL ID */
   /* ----------- */
   if ( !(myUtilsObject.IsEmpty(m_StrDeviceIDFld, this)) ) // empty field
   {
      all_fields_empty = false;
      memcpy ( sTLFLimRecord.terminal_id_min,
               m_StrDeviceIDFld,
               sizeof (sTLFLimRecord.terminal_id_min) );
   }

   /* ----------- */
   /* MERCHANT ID */
   /* ----------- */
   if ( !(myUtilsObject.IsEmpty(m_StrMerchantIDFld, this)) ) // empty field
   {
      all_fields_empty = false;
      memcpy ( sTLFLimRecord.merchant_id_min,
               m_StrMerchantIDFld,
               sizeof (sTLFLimRecord.merchant_id_min) );
   }

   /* ---- */
   /* DATE */
   /* ---- */
   if ( !(myUtilsObject.IsEmpty(m_StrDateStartFld, this)) ) // empty field
   {
      /* Reformat start date from DDMMYYYY into YYYYMMDD. */
      temp = m_StrDateStartFld.Right(4) +
             m_StrDateStartFld.Mid(2,2) +
             m_StrDateStartFld.Left(2);

      memcpy ( sTLFLimRecord.date_yyyymmdd_start, temp,
               sizeof (sTLFLimRecord.date_yyyymmdd_start) );
   }

   if ( !(myUtilsObject.IsEmpty(m_StrDateEndFld, this)) ) // empty field
   {
      /* Reformat end date from DDMMYYYY into YYYYMMDD. */
      temp = m_StrDateEndFld.Right(4) +
             m_StrDateEndFld.Mid(2,2) +
             m_StrDateEndFld.Left(2);

      memcpy ( sTLFLimRecord.date_yyyymmdd_end, temp,
               sizeof (sTLFLimRecord.date_yyyymmdd_end) );
   }

   /* ---- */
   /* TIME */
   /* ---- */
   if ( !(myUtilsObject.IsEmpty(m_StrTimeStartFld, this)) ) // empty field
   {
      /* Reformat the start time to include seconds set to 00 at the end. */
      memcpy ( sTLFLimRecord.time_hhmmss_start, m_StrTimeStartFld + "00",
               sizeof (sTLFLimRecord.time_hhmmss_start) );
   }

   if ( !(myUtilsObject.IsEmpty(m_StrTimeEndFld, this)) ) // empty field
   {
      /* Reformat the end time to include seconds set to 00 at the end. */
      memcpy ( sTLFLimRecord.time_hhmmss_end, m_StrTimeEndFld + "00",
               sizeof (sTLFLimRecord.time_hhmmss_end) );
   }

   /* -------------- */
   /* TRANSACTION ID */
   /* -------------- */
   memcpy ( sTLFLimRecord.transaction_id, " ",
            sizeof (sTLFLimRecord.transaction_id) );

   if ( all_fields_empty == true )
   {
      m_StrMessageLbl = "Please enter search criteria...";
      UpdateData(FALSE);
	  m_CntlCardNumberFld.SetFocus();
	  return;
   }

   if(m_StrPosSearchCheck)
   {
      PopulateTranListDialog();
   }
   
  OnMaskCardNumberChk();
   return;
}

void CTranPosInquiry::OnNextButton() 
{
   if (!bDone)
   {
	   if(m_StrPosSearchCheck)
	   {
		  PopulateTranListDialog();
	   }
	   OnMaskCardNumberChk();
   }
   else 
   {
      m_StrMessageLbl = "End of list";
      m_NextButton.EnableWindow(FALSE);
      UpdateData(FALSE);
   }
}

void CTranPosInquiry::OnDetailsButton() 
{
	CTransactionDetails detailsDlg;
	CString strTranID;
	CString strtempSystemDate;
	CString strSystemDate;
	int nRowSelected = m_CntlList.GetNextItem (-1, LVNI_SELECTED) ;

	if( nRowSelected == -1)
	{
		m_StrMessageLbl = "Please Select an item";
		UpdateData(FALSE);	
		return;
	}
	
	detailsDlg.m_bMaskCardNumbers = m_MaskCardNumber.GetCheck();
	

    strTranID = m_CntlList.GetItemText (nRowSelected, 8) ;
	strSystemDate = m_CntlList.GetItemText(nRowSelected, 9) ;
	strSystemDate.Remove('/');

	if(m_bTran)
	{
		strcpy((char *)detailsDlg.sTranRecord.tpos01.primary_key.transaction_id, strTranID);
		strcpy((char *)detailsDlg.sTranRecord.tpos01.primary_key.system_date, strSystemDate);
 	    detailsDlg.m_bViewEmvData = m_bViewEmvData;	/*view EMV raw data in details dialog*/
	}
	else
	{
		detailsDlg.m_bViewRawData = m_bViewRawData;
	}
	
	UpdateData();
	detailsDlg.DoModal();
	
}

void CTranPosInquiry::PopulateTranListDialog()
{
   CFont m_font;
   LV_ITEM lvItem;
   char strErrorMessage[200];
   CString temp;
   char strTemp[20];

   int nRowItem = 0;
   int nActualItem = 0;
   int nNumberReturned;
   bDone = FALSE;
   int nSize, nCount;
   nCount = m_CntlList.GetItemCount();
   CMyUtils myUtilsObject;

   UpdateData(TRUE);
   m_StrMessageLbl = "Please Wait";
   UpdateData(FALSE);

   int rcode = txdsapi_get_list ((pBYTE)&sTLFLimRecord,sizeof(TPOS01_LIMITS), (pBYTE)&sTLFList, 
                                 sizeof (TPOS01_GUI_LIST),0,TPOS01_DATA,strErrorMessage);
   if (rcode == PTEMSG_OK)
   {
      nNumberReturned = atoi ((char *)&sTLFList.num_returned);

      if ( 0 == nNumberReturned )
      {
         m_StrMessageLbl = "No Records Found";
         bDone = TRUE;
         nSize = 0;
			m_DetailsButton.EnableWindow(FALSE);
         m_NextButton.EnableWindow(FALSE);
         UpdateData(FALSE);
      }
      else if( nNumberReturned == GUI_MAX_LIST_SIZE1)
      {				
         memcpy( sTLFLimRecord.transaction_id, (LPCTSTR) sTLFList.tpos01_structure[nNumberReturned-1].primary_key.transaction_id, sizeof (sTLFLimRecord.transaction_id)); 
         nSize = nNumberReturned - 1;
		 m_DetailsButton.EnableWindow(TRUE);
         m_NextButton.EnableWindow(TRUE);
         m_StrMessageLbl = "";
         UpdateData(FALSE);
      }
      else
      {
         m_StrMessageLbl = "End of list";
         bDone = TRUE;
         nSize = nNumberReturned;
		 m_DetailsButton.EnableWindow(TRUE);
         m_NextButton.EnableWindow(FALSE);
         UpdateData(FALSE);
      }

      for (int nItem = 0; nItem <  nSize; nItem++,nRowItem ++)
      { // will now insert the items and subitems into the list view.
         m_CntlList.EnableWindow(TRUE);  
         for (int nSubItem = 0; nSubItem < 14; nSubItem++)
         {
            lvItem.mask = LVIF_TEXT ;
            lvItem.iItem = (nSubItem == 0)? nRowItem : nActualItem;
            lvItem.iSubItem = nSubItem;

            if (nSubItem == 0) { 
               lvItem.pszText = (char *)sTLFList.tpos01_structure[nItem].card_num;                   
            }
            else if (nSubItem == 1) { 
               lvItem.pszText = (char *)sTLFList.tpos01_structure[nItem].merchant_id;
            }
            else if (nSubItem == 2) { 
               lvItem.pszText = (char *)sTLFList.tpos01_structure[nItem].terminal_id;
            }
            else if (nSubItem == 3) { 
               if ( sTLFList.tpos01_structure[nItem].date_YYYYMMDD[0] != 0x00 )
               {
   					temp = sTLFList.tpos01_structure[nItem].date_YYYYMMDD;
                  temp = temp.Right(2) + "/" + temp.Mid(4,2) + "/" + temp.Left(4);
                  memcpy ( strTemp, temp, sizeof (strTemp));
                  lvItem.pszText = strTemp ;                    
               }
               else
                  lvItem.pszText = " ";
            }
            else if (nSubItem == 4) { 
               if ( sTLFList.tpos01_structure[nItem].time_HHMMSS[0] != 0x00 )
               {
                  temp = sTLFList.tpos01_structure[nItem].time_HHMMSS;
                  temp = temp.Left(2) + ":" + temp.Mid(2,2);
                  memcpy ( strTemp, temp, sizeof (strTemp));
                  lvItem.pszText = strTemp ;                    
               }
               else
                  lvItem.pszText = " ";
            }
            else if (nSubItem == 5) { 
               lvItem.pszText = (char *)sTLFList.tpos01_structure[nItem].response_code;
            }
            else if (nSubItem == 6) { 
               lvItem.pszText = (char *)sTLFList.tpos01_structure[nItem].sys_trace_audit_num;
            }
            else if (nSubItem == 7) { 
               lvItem.pszText = (char *)sTLFList.tpos01_structure[nItem].retrieval_ref_num;
            }
            else if (nSubItem == 8) { 
               lvItem.pszText = (char *)sTLFList.tpos01_structure[nItem].primary_key.transaction_id;
            }
            else if (nSubItem == 9) { 
               if ( sTLFList.tpos01_structure[nItem].primary_key.system_date[0] != 0x00 )
               {
   					temp = sTLFList.tpos01_structure[nItem].primary_key.system_date;
                  temp = temp.Right(2) + "/" + temp.Mid(4,2) + "/" + temp.Left(4);
                  memcpy ( strTemp, temp, sizeof (strTemp));
                  lvItem.pszText = strTemp ;   
               }
               else
                  lvItem.pszText = " ";
            }
			else if (nSubItem == 10){
				lvItem.pszText = " ";
			}

            if (nSubItem == 0) 
               nActualItem = m_CntlList.InsertItem(nCount+nSize,(char *)sTLFList.tpos01_structure[nItem].card_num);
            else 
               m_CntlList.SetItem(&lvItem);

         } // for
      } // for
      m_CntlList.SetFocus() ;
      int nCount = m_CntlList.GetItemCount();

      m_CntlList.EnsureVisible( nCount-1, TRUE);
   }		
   else if ( PTEMSG_INVALID_DATATYPE == rcode )
   {
      m_StrMessageLbl = "Please try again";
   }

   else if( PTEMSG_OK != rcode )
   {
      m_StrMessageLbl = strErrorMessage;
   }

   UpdateData(FALSE);

}


BOOL CTranPosInquiry::ValidateTime ( CString strTime )
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
		m_StrMessageLbl = "Invalid Time Format";
		UpdateData(FALSE);
		return FALSE;
	}

	strHH = strTime.Left(2);
	strMM = strTime.Right(2);

	nHH = atoi ( strHH );
	nMM = atoi ( strMM );

	if ( nHH == 0 && nMM == 0 )
	{
		m_StrMessageLbl = "ERROR:Invalid Data!";
		UpdateData(FALSE);
		return FALSE;
	}

	if ( nHH > 23 || nMM >59 )
	{
		m_StrMessageLbl = "ERROR:Invalid Data!";
		UpdateData(FALSE);
		return FALSE;
	}

	return TRUE;

}

BOOL CTranPosInquiry::ValidateDate ( CString strDate )
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
		m_StrMessageLbl = "Invalid Date Format";
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
		m_StrMessageLbl = "ERROR:Invalid Data!";
		UpdateData(FALSE);
		return FALSE;
	}


	if ( nMonth > 12 )
	{
		m_StrMessageLbl = "ERROR:Invalid Month!";
		UpdateData(FALSE);
		return FALSE;
	}

	if ( nMonth == 1 || nMonth == 3 || nMonth == 5 || 
		 nMonth == 7 || nMonth == 8 || nMonth == 10 || nMonth == 12 )
	{
		if ( nDay > 31 )
		{
			m_StrMessageLbl = "ERROR:Invalid Day!";
			UpdateData(FALSE);
			return FALSE;
		}
	}

	if ( nMonth == 4 || nMonth == 6 || nMonth == 9 || 
		 nMonth == 11 )
	{
		if ( nDay > 30 )
		{
			m_StrMessageLbl = "ERROR:Invalid Day!";
			UpdateData(FALSE);
			return FALSE;
		}
	}

	if ( nMonth ==2 )
	{
		int res = nYear % 4;
		if ( res != 0 && nDay >28)
		{
			m_StrMessageLbl = "ERROR:Invalid Day!";
			UpdateData(FALSE);
			return FALSE;
		}
		if (res == 0 && nDay >29)
		{
			m_StrMessageLbl = "ERROR:Invalid Day!";
			UpdateData(FALSE);
			return FALSE;
		}
	}
	
	return TRUE;
}

void CTranPosInquiry::OnDblclkList(NMHDR* pNMHDR, LRESULT* pResult) 
{
    OnDetailsButton(); 	

	*pResult = 0;
}

void CTranPosInquiry::OnEMVRAWRadio() 
{
	UpdateData(TRUE);

	if(m_CntlEmvRawRadio.GetCheck() == CHECKED)
	{
		m_bViewEmvData = true;
		UpdateData(FALSE);
	}
	
	return;
}

void CTranPosInquiry::OnPosSearchCheck() 
{
	UpdateData(TRUE);

	if(m_CntlPosSearchCheck.GetCheck() == CHECKED)
	{
		m_CntlViewAllRadio.EnableWindow(FALSE);
		m_CntlViewAcceptedRadio.EnableWindow(FALSE);
		m_CntlViewFailedRadio.EnableWindow(FALSE);
		m_CntlViewFGRawRadio.SetCheck(UNCHECKED);
		m_CntlViewFGRawRadio.EnableWindow(FALSE);
		m_CntlResendButton.EnableWindow(FALSE);
		m_CntlFGuardSearchCheck.SetCheck(UNCHECKED);

		m_CntlEmvRawRadio.EnableWindow();
		m_CntlEmvRawRadio.SetCheck(UNCHECKED);
		m_StrPosSearchCheck = true;
		m_StrFGuardSearchCheck = false;
	    m_bFGuard = false;
		m_bTran = true;
	}
	else
	{
		m_bTran = false;
	}

	  m_bViewEmvData = false;

  UpdateData(FALSE);
	return;
}

void CTranPosInquiry::OnViewAllradio() 
{
	ST2_TYPE = ST2_NONE;
}

void CTranPosInquiry::OnViewAcceptedRadio() 
{
	ST2_TYPE = ST2_GET_ALL_FG_SUCCESS;
}

void CTranPosInquiry::OnViewFailedRadio() 
{
	ST2_TYPE = ST2_GET_ALL_FG_FAILED;
}

void CTranPosInquiry::OnResendButton() 
{

	AUTH_TX auth_tx;
	CString strTranID;
	CString strMessage;
	CHAR    FG_tranid_list[21];

	char     strErrorMessage[260] = "";

	int rcode;
	int nRowSelected = 0;
	long len = 0;

	memset (&auth_tx, 0x00, sizeof(auth_tx));

	nRowSelected = m_CntlList.GetNextItem (-1, LVNI_SELECTED) ;

	if( nRowSelected == -1) /*no selection made - default to send all records?*/
	{
		/*Verify send all?*/
		strMessage.Format ("Resend ALL Failed Records?");
		rcode = AfxMessageBox ((LPCTSTR)strMessage, MB_OKCANCEL|MB_ICONQUESTION);
		if (rcode != IDOK)
		{
			m_StrMessageLbl = "Please Select an item";
			UpdateData(FALSE);	
			return;
		}
		else /*send all failed transactions*/
		{
			len = 0;
			memset (&FG_tranid_list, 0x00, sizeof(FG_tranid_list));
		}
	}
	else /* Send selected record*/
	{
		/*get tran_id of selected record*/
		strTranID = m_CntlList.GetItemText (nRowSelected, 11) ;
		strcpy(FG_tranid_list, strTranID);
		len = strlen(FG_tranid_list);
	}

    rcode = txdsapi_fraudguard(FG_MODE_SEND_BATCH, &auth_tx, FG_tranid_list, &len, strErrorMessage);
	if (rcode == PTEMSG_OK)
	{
		m_StrMessageLbl = "Send Completed.";
	}
	else
	{
		m_StrMessageLbl = "Send Error: ";
	}

		UpdateData(FALSE);
		return ;

}

LRESULT CTranPosInquiry::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	// TODO: Add your specialized code here and/or call the base class


	switch(message)
    {	

      case WM_TIMER:
		{
			cntr++;
			
			shm_time = shrmem.GetLastUpdateTime();

			if(shm_time > c_time)
			{
				c_time = shm_time;
				ResetCntr();
			}
			if ( cntr == idle_time/2 )
			{
				shrmem.SetIdleStatus(0);
			}
			//Check whether Max Idle Time has elapsed
			if(cntr >= idle_time)
			{
				ret = LockScreen();

				if(ret == -1 || ret == -2)
				{				
					::PostMessage(this->m_hWnd, WM_CLOSE, 0, 0);
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


	 default:break;
			
	}

	return CDialog::DefWindowProc(message, wParam, lParam);
}


int CTranPosInquiry::StartTimer(int time)
{
	int handle = SetTimer (IDT_TIMER, time, NULL);  // Start the Timer

	return handle;
}

bool CTranPosInquiry::StopTimer()
{

	if (!KillTimer (timer_handle))	//Stop Timer	
	{
				::MessageBox(NULL,"Error While Killing the Timer","ShutEvt",1);	
		return false;
	}

	return true;
}

BOOL CTranPosInquiry::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo) 
{
	// TODO: Add your specialized code here and/or call the base class
	ResetCntr();
	c_time = CTime::GetCurrentTime();	
	shrmem.SetLastUpdateTime(c_time);
	return CDialog::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}



void CTranPosInquiry::OnButtonExit() 
{
	// TODO: Add your control notification handler code here		
				
		//Delete the associated Mutex lock 
		shrmem.DeleteMutexLock();
		CDialog::OnCancel();
}



short CTranPosInquiry::LockScreen()
{
	int retcode = 0;

	CLoginfrm idleloginfrm;

	//Stop the Timer & Display Login Screen
	StopTimer();

	ResetCntr();

	if(pLock->Lock())
	{
		status = shrmem.GetIdleStatus();

		if(status == 0)
		{
			FlashWindow(true);

			//AfxMessageBox("Session Timeout. Please Login");
			::MessageBox(this->m_hWnd,"Session Timeout. Please Login","Transaction Lookup",MB_OK);
			
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

void CTranPosInquiry::ResetCntr()
{
	cntr = 0;
}

void CTranPosInquiry::OnMaskCardNumberChk() 
{
	if(m_MaskCardNumber.GetCheck() != 0)
	{
		OriginalCardNoList.RemoveAll();
		int count = m_CntlList.GetItemCount();
		for(int i=0;i<count;i++)
		{
			CString str = m_CntlList.GetItemText(i,0);
			CString str1 = str;
			OriginalCardNoList.Add(str1);
			int len = str.GetLength();
			if(len > 0)
			{
				char *pStr = str.GetBuffer(len);
				int card_num_len=str.GetLength();
				if(card_num_len>12)
				{
					if(pStr[6] != 'X')
					{
						pStr[6]  = 'X';
						pStr[7]  = 'X';
						pStr[8]  = 'X';
						pStr[9]  = 'X';
						pStr[10] = 'X';
						pStr[11] = 'X';
					}
				}
				else if(card_num_len>11)
				{
					if(pStr[6] != 'X')
					{
						pStr[6]  = 'X';
						pStr[7]  = 'X';
						pStr[8]  = 'X';
						pStr[9]  = 'X';
						pStr[10] = 'X';
					}
				}
				else if(card_num_len>10)
				{
					if(pStr[6] != 'X')
					{
						pStr[6]  = 'X';
						pStr[7]  = 'X';
						pStr[8]  = 'X';
						pStr[9]  = 'X';
					}
				}
				else if(card_num_len>9)
				{
					if(pStr[6] != 'X')
					{
						pStr[6]  = 'X';
						pStr[7]  = 'X';
						pStr[8]  = 'X';
					}
				}
				else if(card_num_len>8)
				{
					if(pStr[6] != 'X')
					{
						pStr[6]  = 'X';
						pStr[7]  = 'X';
					}
				}
				else if(card_num_len>7)
				{
					if(pStr[6] != 'X')
					{
						pStr[6]  = 'X';
					}
				}
		
			}
			m_CntlList.SetItemText(i,0,str);
		}
	}
	else
	{
		if(OriginalCardNoList.GetSize() > 0)
		{
			int count = m_CntlList.GetItemCount();
			for(int i=0;i<count;i++)
			{
				if(i < OriginalCardNoList.GetSize())
				{
					CString origCardNo = OriginalCardNoList[i];
					m_CntlList.SetItemText(i,0,origCardNo);
				}
			}
		}
	}
}
