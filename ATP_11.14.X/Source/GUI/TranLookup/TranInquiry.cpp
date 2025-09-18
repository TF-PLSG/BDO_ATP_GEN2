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
* $Log:   N:\PVCS\PTE\Equitable\tran_lookup\TranInquiry.cpp  $
   
      Rev 1.0   Jan 25 2011  16:55:18   TF-Sriaknth
   Added Idle Time of TranInquiry
   
     
*
************************************************************************************/

// TranInquiry.cpp : implementation file
//

#include "stdafx.h"
#include "TranSearch.h"
#include "TranInquiry.h"
#include "transactiondetails.h"
#include "TransactionDetailsPropertySheet.h"
#include "MyUtils.h"
#include "Loginfrm.h"
#include "SharedMem.h"

extern "C"
{
   #include "ntutils.h"
	#include "PteTime.h"

}

	BOOL m_bTran;
	BOOL m_bTran_stip;
	BOOL m_bFGuard;
	
	INT ST2_TYPE = 0;

CLoginfrm loginfrm;
CString idle_time_val_str;
CSharedMem shrmem;

extern CString strUserName;
extern int idleTime;
extern char strFileRecord[56];
int m_INTSTIPSearchCheck=0;

/////////////////////////////////////////////////////////////////////////////
// CTranInquiry dialog


CTranInquiry::CTranInquiry(CWnd* pParent /*=NULL*/)
	: CDialog(CTranInquiry::IDD, pParent)
{
	//{{AFX_DATA_INIT(CTranInquiry)
	m_StrCardNumberFld = _T("");
	m_StrAuthNumberFld = _T("");
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

	//hostName
	m_StrHostNameFld = _T("");

	//ramya
	m_StrSTIPSearchCheck = FALSE;
	//}}AFX_DATA_INIT

	//Instantiate the Object for SHM
	pLock = new CDMLock("IdleLock");
}

CTranInquiry::~CTranInquiry()
{

  	delete pLock;
   pLock = NULL;

}
void CTranInquiry::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTranInquiry)
	DDX_Control(pDX, IDC_MASK_CARD_NUMBER_CHK, m_MaskCardNumber);
	DDX_Control(pDX, IDC_POS_SEARCH_CHECK, m_CntlPosSearchCheck);
	//ramya
	DDX_Control(pDX, IDC_TLF01_STIP_SEARCH, m_CntlSTIPSearchCheck);

	DDX_Control(pDX, IDC_FGUARD_SEARCH_CHECK, m_CntlFGuardSearchCheck);
	DDX_Control(pDX, IDC_EMV_RAW_RADIO, m_CntlEmvRawRadio);
	DDX_Control(pDX, IDC_VIEW_FAILED_RADIO, m_CntlViewFailedRadio);
	DDX_Control(pDX, IDC_VIEW_ACCEPTED_RADIO, m_CntlViewAcceptedRadio);
	DDX_Control(pDX, IDC_VIEW_FGRAW_RADIO, m_CntlViewFGRawRadio);
	DDX_Control(pDX, IDC_VIEW_ALLRADIO, m_CntlViewAllRadio);
	DDX_Control(pDX, IDC_RESEND_BUTTON, m_CntlResendButton);
	DDX_Control(pDX, IDC_NEXT_BUTTON, m_NextButton);
	DDX_Control(pDX, IDC_INQUIRY_BUTTON, m_InquiryButton);
	DDX_Control(pDX, IDC_TLF_EXTRACT, m_TlfExtractButton);
	DDX_Control(pDX, IDC_DETAILS_BUTTON, m_DetailsButton);
	DDX_Control(pDX, IDC_TIME_FLD, m_CntlTimeStartFld);
	DDX_Control(pDX, IDC_TIME_END_FLD, m_CntlTimeEndFld);
	DDX_Control(pDX, IDC_DATE_FLD, m_CntlDateStartFld);
	DDX_Control(pDX, IDC_DATE_END_FLD, m_CntlDateEndFld);
	DDX_Control(pDX, IDC_MERCHANT_ID_FLD, m_CntlMerchantIDFld);
	DDX_Control(pDX, IDC_LIST, m_CntlList);
	DDX_Control(pDX, IDC_DEVICE_ID_FLD, m_CntlDeviceIDFld);
	DDX_Control(pDX, IDC_AUTH_NUMBER, m_CntlAuthNumberFld);
	DDX_Text(pDX, IDC_AUTH_NUMBER, m_StrAuthNumberFld);
	DDV_MaxChars(pDX, m_StrAuthNumberFld, 6);
	DDX_Control(pDX, IDC_ACCOUNT_NBR_FLD, m_CntlCardNumberFld);
	DDX_Text(pDX, IDC_ACCOUNT_NBR_FLD, m_StrCardNumberFld);
	DDV_MaxChars(pDX, m_StrCardNumberFld, 19);
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
	//ramya
	DDX_Check(pDX, IDC_TLF01_STIP_SEARCH, m_StrSTIPSearchCheck);
	//}}AFX_DATA_MAP

	DDX_Control(pDX, IDC_HOSTNAME, m_CntlAuthNumberFld);
	DDX_Text(pDX, IDC_HOSTNAME, m_StrHostNameFld);
	DDV_MaxChars(pDX, m_StrHostNameFld, 12);
}


BEGIN_MESSAGE_MAP(CTranInquiry, CDialog)
	//{{AFX_MSG_MAP(CTranInquiry)
	ON_BN_CLICKED(IDC_INQUIRY_BUTTON, OnInquiryButton)
	ON_BN_CLICKED(IDC_NEXT_BUTTON, OnNextButton)
	ON_BN_CLICKED(IDC_DETAILS_BUTTON, OnDetailsButton)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST, OnDblclkList)
	ON_BN_CLICKED(IDC_EMV_RAW_RADIO, OnEMVRAWRadio)
	ON_BN_CLICKED(IDC_POS_SEARCH_CHECK, OnPosSearchCheck)
	ON_BN_CLICKED(IDC_FGUARD_SEARCH_CHECK, OnFguardSearchCheck)
	ON_BN_CLICKED(IDC_VIEW_FGRAW_RADIO, OnViewFgrawRadio)
	ON_BN_CLICKED(IDC_VIEW_ALLRADIO, OnViewAllradio)
	ON_BN_CLICKED(IDC_VIEW_ACCEPTED_RADIO, OnViewAcceptedRadio)
	ON_BN_CLICKED(IDC_VIEW_FAILED_RADIO, OnViewFailedRadio)
	ON_BN_CLICKED(IDC_RESEND_BUTTON, OnResendButton)
	ON_BN_CLICKED(IDC_BUTTON_EXIT, OnButtonExit)
	ON_BN_CLICKED(IDC_MASK_CARD_NUMBER_CHK, OnMaskCardNumberChk)

	//ramya
	ON_BN_CLICKED(IDC_TLF01_STIP_SEARCH, OnTLF01STIPSearchCheck)
	//}}AFX_MSG_MAP

	ON_BN_CLICKED(IDC_TLF_EXTRACT, &CTranInquiry::OnBnClickedTlfExtract)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTranInquiry message handlers

BOOL CTranInquiry::OnInitDialog() 
{
   CDialog::OnInitDialog();
   CFont m_font;
   LOGFONT lf;
   int nHeader;
   int nIndex = -1;
   int rcode;
   char strHeaderData[20][50];
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
   memcpy (strHeaderData[5], "Tran Amount",    sizeof (strHeaderData[5]) );
   memcpy (strHeaderData[6], "Resp Code",      sizeof (strHeaderData[6]) );
   memcpy (strHeaderData[7], "Auth ID Response",sizeof (strHeaderData[7]) );
   memcpy (strHeaderData[8], "Acquirer ID",    sizeof (strHeaderData[8]) );
   memcpy (strHeaderData[9], "Issuer ID",      sizeof (strHeaderData[9]) );
   memcpy (strHeaderData[10], "Sys Trace Nbr",  sizeof (strHeaderData[10]) );
   memcpy (strHeaderData[11],"Ref Nbr",        sizeof (strHeaderData[11]));
   memcpy (strHeaderData[12],"Transaction ID", sizeof (strHeaderData[12]));
   memcpy (strHeaderData[13],"System Date",    sizeof (strHeaderData[13]));
   //memcpy (strHeaderData[13],"Result Code",    sizeof (strHeaderData[13]));
   memcpy (strHeaderData[14],"System Date/Time",    sizeof (strHeaderData[14]));

   for (nHeader = 0; nHeader < 15; nHeader++)
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
      m_CntlList.SetColumnWidth (7,  LVSCW_AUTOSIZE_USEHEADER) ;
      m_CntlList.SetColumnWidth (8,  LVSCW_AUTOSIZE_USEHEADER) ;
      m_CntlList.SetColumnWidth (9,  LVSCW_AUTOSIZE_USEHEADER) ;
      m_CntlList.SetColumnWidth (10, LVSCW_AUTOSIZE_USEHEADER) ;
      m_CntlList.SetColumnWidth (11, LVSCW_AUTOSIZE_USEHEADER) ;
      m_CntlList.SetColumnWidth (12, LVSCW_AUTOSIZE_USEHEADER) ;
      m_CntlList.SetColumnWidth (13, LVSCW_AUTOSIZE_USEHEADER) ;
      m_CntlList.SetColumnWidth (14, LVSCW_AUTOSIZE_USEHEADER) ;
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
   m_TlfExtractButton.EnableWindow(FALSE);
   m_DetailsButton.EnableWindow(FALSE);
   m_NextButton.EnableWindow(FALSE);
   m_CntlList.EnableWindow(FALSE);	
   m_CntlPosSearchCheck.EnableWindow(FALSE);
   m_CntlPosSearchCheck.SetCheck(CHECKED);

   //ramya
   m_CntlSTIPSearchCheck.EnableWindow(FALSE);
   m_CntlSTIPSearchCheck.SetCheck(CHECKED);

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
			//ramya
			m_StrSTIPSearchCheck = false;

            m_bTran = true;
			m_bTran_stip=false;
			m_MaskCardNumber.SetCheck(CHECKED);

			if(('1' == sUCFRecord.control[nIndex].action[0]) ||
				('1' == sUCFRecord.control[nIndex].action[1]) ||
				('1' == sUCFRecord.control[nIndex].action[2]))
			{
				m_MaskCardNumber.EnableWindow(TRUE);
			}

			if(('1' == sUCFRecord.control[nIndex].action[4]))
			{
				m_TlfExtractButton.EnableWindow(TRUE);
			}
			else 
			{
				m_TlfExtractButton.EnableWindow(FALSE);
			}

            /* Must have UPDATE privileges to enable EMV/Fraudguard activities.*/
            if ( '1' == sUCFRecord.control[nIndex].action[2] )
            {
               m_CntlPosSearchCheck.EnableWindow(TRUE);
               m_CntlPosSearchCheck.SetCheck(CHECKED);
               
			   //ramya
			   m_CntlSTIPSearchCheck.EnableWindow(TRUE);
			   m_CntlSTIPSearchCheck.SetCheck(UNCHECKED);

			   m_CntlEmvRawRadio.EnableWindow(FALSE);
               m_CntlFGuardSearchCheck.EnableWindow(FALSE);
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

void CTranInquiry::OnInquiryButton() 
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

      /* ----------- */
   /* AUTH NUMBER */
   /* ----------- */
   if ( !(myUtilsObject.IsEmpty(m_StrAuthNumberFld, this)) ) // empty field
   {
      all_fields_empty = false;
      memcpy ( sTLFLimRecord.auth_number_min,
               m_StrAuthNumberFld,
               sizeof (sTLFLimRecord.auth_number_min) );
  
   
		int len = m_StrAuthNumberFld.GetLength();
		char *str = m_StrAuthNumberFld.GetBuffer(len);
		if(len < 6)
		{
			m_StrMessageLbl = "The length of Auth ID should not be less than 6 digits";	
			m_CntlAuthNumberFld.SetFocus();
			(UpdateData(FALSE));
			return;
		}
		for(int i=0;i<len;i++)
		{
			if((str[i] > 64 && str[i] < 91) || (str[i] > 47 && str[i] < 58) )
			{
				//The character is a ALPHABHET or NUMBER
			}
			else
			{
				m_StrMessageLbl = "No Special characters allowed in Auth ID";	
				m_CntlAuthNumberFld.SetFocus();
				
				 (UpdateData(FALSE));
						
				
				m_StrAuthNumberFld.ReleaseBuffer(len);

				return;
			}
   
   
		}

		if((myUtilsObject.IsEmpty(m_StrDeviceIDFld, this)) && 
			(myUtilsObject.IsEmpty(m_StrMerchantIDFld, this)) && 
			(myUtilsObject.IsEmpty(m_StrCardNumberFld, this)))
		{
			m_StrMessageLbl = "Enter at least one other parameter with Auth Id: Card Number, Merchant ID or Device ID";
			UpdateData(FALSE);
			return;
		}
   
   }
   /**********************************************************************************/

    /* ----------- */
   /* HOSTNAME */
   /* ----------- */
   if ( !(myUtilsObject.IsEmpty(m_StrHostNameFld, this)) ) // empty field
   {
      all_fields_empty = false;
	  memcpy ( sTLFLimRecord.redemption_amount,
               m_StrHostNameFld,
               sizeof (sTLFLimRecord.redemption_amount) );
   
   }


  /***********************************************************************************/

   /* ----------- */
   /* TERMINAL ID */
   /* ----------- */
   if ( !(myUtilsObject.IsEmpty(m_StrDeviceIDFld, this)) ) // empty field
   {
      all_fields_empty = false;
      memcpy ( sTLFLimRecord.terminal_id_min,
               m_StrDeviceIDFld,
               sizeof (sTLFLimRecord.terminal_id_min));
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
   else
   if(m_StrFGuardSearchCheck)
   {
	   PopulateFGuardListDialog();
   }
   if(m_StrSTIPSearchCheck)
   {
	  PopulateTLF01_STIPListDialog();
	  m_INTSTIPSearchCheck = 1;
   }

  OnMaskCardNumberChk();
   return;
}

void CTranInquiry::OnNextButton() 
{
   if (!bDone)
   {
	   if(m_StrPosSearchCheck)
	   {
		  PopulateTranListDialog();
	   }
	   else
	   if(m_StrFGuardSearchCheck)
	   {
		   PopulateFGuardListDialog();
	   }

	   if(m_StrSTIPSearchCheck)
	   {
		  PopulateTLF01_STIPListDialog();
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

void CTranInquiry::OnDetailsButton() 
{
	CTransactionDetails detailsDlg;
	CFraudTransactionDetails fraudDlg;
	CAddlFldsTransactionDetails addlDlg;
	CVisaEDQPTransactionDetails vedqpDlg;
	CTransactionDetailsPropertySheet *propSheet = new CTransactionDetailsPropertySheet(_T("Trandetails Property Sheet"));
	CString strTranID;
	int nRowSelected = m_CntlList.GetNextItem (-1, LVNI_SELECTED) ;

	if( nRowSelected == -1)
	{
		m_StrMessageLbl = "Please Select an item";
		UpdateData(FALSE);	
		return;
	}
	
	detailsDlg.m_bMaskCardNumbers = m_MaskCardNumber.GetCheck();
	

    strTranID = m_CntlList.GetItemText (nRowSelected, 12) ;

	if(m_bTran)
	{
		strcpy((char *)detailsDlg.sTranRecord.tlf01.primary_key.transaction_id, strTranID);
 	    detailsDlg.m_bViewEmvData = m_bViewEmvData;	/*view EMV raw data in details dialog*/
		strcpy((char *)fraudDlg.sFraudRecord.ftlf01.tran_id, strTranID);
		strcpy((char *)addlDlg.sTranRecord.tlf01.primary_key.transaction_id, strTranID);
		strcpy((char *)vedqpDlg.TranRecord.tlf01.primary_key.transaction_id, strTranID);

	}
	else if(m_bTran_stip)
	{
		strcpy((char *)detailsDlg.sTranRecordS.tlf01.primary_key.transaction_id, strTranID);
		strcpy((char *)fraudDlg.sFraudRecordS.ftlf01.tran_id, strTranID);
		strcpy((char *)addlDlg.sTranRecordS.tlf01.primary_key.transaction_id, strTranID);
		strcpy((char *)vedqpDlg.TranRecordS.tlf01.primary_key.transaction_id, strTranID);
	}
	
	else
	{
		strcpy((char *)detailsDlg.sFGuardRecord.fguard.transaction_id, strTranID);
	    strcpy((char *)detailsDlg.sFGuardRecord.fgraw.transaction_id, strTranID);
	    detailsDlg.m_bViewRawData = m_bViewRawData;
	}
			
	UpdateData(TRUE);
	//detailsDlg.DoModal();
	//fraudDlg.DoModal();
	propSheet->AddPage(&detailsDlg);
	propSheet->AddPage(&fraudDlg);
	propSheet->AddPage(&addlDlg);
	propSheet->AddPage(&vedqpDlg);
	propSheet->DoModal();	

	

}

void CTranInquiry::PopulateTranListDialog()
{
   CFont m_font;
   LV_ITEM lvItem;
   char strErrorMessage[200];
   CString temp;
   char strTemp[20];
   CString tempT;

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

   int rcode = txdsapi_get_list ((pBYTE)&sTLFLimRecord,sizeof(TLF01_LIMITS), (pBYTE)&sTLFList, 
                                 sizeof (TLF01_GUI_LIST),0,TLF01_DATA,strErrorMessage);
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
         memcpy( sTLFLimRecord.transaction_id, (LPCTSTR) sTLFList.tlf01_structure[nNumberReturned-1].primary_key.transaction_id, sizeof (sTLFLimRecord.transaction_id)); 
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
         for (int nSubItem = 0; nSubItem < 15; nSubItem++)
         {
            lvItem.mask = LVIF_TEXT ;
            lvItem.iItem = (nSubItem == 0)? nRowItem : nActualItem;
            lvItem.iSubItem = nSubItem;

            if (nSubItem == 0) { 
               lvItem.pszText = (char *)sTLFList.tlf01_structure[nItem].card_num;                   
            }
            else if (nSubItem == 1) { 
               lvItem.pszText = (char *)sTLFList.tlf01_structure[nItem].merchant_id;
            }
            else if (nSubItem == 2) { 
               lvItem.pszText = (char *)sTLFList.tlf01_structure[nItem].terminal_id;
            }
            else if (nSubItem == 3) { 
               if ( sTLFList.tlf01_structure[nItem].date_yyyymmdd[0] != 0x00 )
               {
   					temp = sTLFList.tlf01_structure[nItem].date_yyyymmdd;
                  temp = temp.Right(2) + "/" + temp.Mid(4,2) + "/" + temp.Left(4);
                  memcpy ( strTemp, temp, sizeof (strTemp));
                  lvItem.pszText = strTemp ;                    
               }
               else
                  lvItem.pszText = " ";
            }
            else if (nSubItem == 4) { 
               if ( sTLFList.tlf01_structure[nItem].time_hhmmss[0] != 0x00 )
               {
                  temp = sTLFList.tlf01_structure[nItem].time_hhmmss;
                  temp = temp.Left(2) + ":" + temp.Mid(2,2);
                  memcpy ( strTemp, temp, sizeof (strTemp));
                  lvItem.pszText = strTemp ;                    
               }
               else
                  lvItem.pszText = " ";
            }
            else if (nSubItem == 5) { 
               temp = sTLFList.tlf01_structure[nItem].total_amount;
               memcpy ( strTemp, myUtilsObject.DisplayAmount(temp), sizeof (strTemp));
               lvItem.pszText = strTemp ;
            }
            else if (nSubItem == 6) { 
               lvItem.pszText = (char *)sTLFList.tlf01_structure[nItem].response_code;
            }
			else if (nSubItem == 7) { 
				lvItem.pszText = (char *)sTLFList.tlf01_structure[nItem].auth_number;
            }
            else if (nSubItem == 8) { 
               lvItem.pszText = (char *)sTLFList.tlf01_structure[nItem].acquirer_id;
            }
            else if (nSubItem == 9) { 
               lvItem.pszText = (char *)sTLFList.tlf01_structure[nItem].issuer_id;
            }
            else if (nSubItem == 10) { 
               lvItem.pszText = (char *)sTLFList.tlf01_structure[nItem].sys_trace_audit_num;
            }
            else if (nSubItem == 11) { 
               lvItem.pszText = (char *)sTLFList.tlf01_structure[nItem].retrieval_ref_num;
            }
            else if (nSubItem == 12) { 
               lvItem.pszText = (char *)sTLFList.tlf01_structure[nItem].primary_key.transaction_id;
            }
            else if (nSubItem == 13) { 
               if ( sTLFList.tlf01_structure[nItem].system_date[0] != 0x00 )
               {
   					temp = sTLFList.tlf01_structure[nItem].system_date;
                  temp = temp.Right(2) + "/" + temp.Mid(4,2) + "/" + temp.Left(4);
                  memcpy ( strTemp, temp, sizeof (strTemp));
                  lvItem.pszText = strTemp ;   
               }
               else
                  lvItem.pszText = " ";
            }
			else if (nSubItem == 14) {
				if ( sTLFList.tlf01_structure[nItem].date_yyyymmdd[0] != 0x00 && sTLFList.tlf01_structure[nItem].time_hhmmss[0] != 0x00)
				{
				  temp = sTLFList.tlf01_structure[nItem].system_date;
                  temp =  temp.Left(4)+ "-" +  temp.Mid(4,2) + "-" + temp.Right(2);

				  tempT = sTLFList.tlf01_structure[nItem].time_hhmmss;
                  tempT = temp + " " + tempT.Left(2) + ":" + tempT.Mid(2,2) + ":" + tempT.Right(2);
                  memcpy ( strTemp, tempT, sizeof (strTemp));
                  lvItem.pszText =   strTemp;              

				}
				else
				{
					lvItem.pszText = " ";
				}
			}
			/*else if (nSubItem == 13){
				lvItem.pszText = " ";
			}*/

            if (nSubItem == 0) 
               nActualItem = m_CntlList.InsertItem(nCount+nSize,(char *)sTLFList.tlf01_structure[nItem].card_num);
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

//ramya

void CTranInquiry::PopulateTLF01_STIPListDialog()
{
   CFont m_font;
   LV_ITEM lvItem;
   char strErrorMessage[200];
   CString temp;
   char strTemp[20];
   CString tempT;

   int nRowItem = 0;
   int nActualItem = 0;
   int nNumberReturned;

   int nNumberSTIPReturned = 0;
   bDone = FALSE;
   int nSize, nCount;
   nCount = m_CntlList.GetItemCount();
   CMyUtils myUtilsObject;

   UpdateData(TRUE);
   m_StrMessageLbl = "Please Wait";
   UpdateData(FALSE);


  
   int rcode = txdsapi_get_list ((pBYTE)&sTLFLimRecord,sizeof(TLF01_LIMITS), (pBYTE)&sTLFList, 
                                 sizeof (TLF01_GUI_LIST),ST2_DB_SELECT_STANDIN,TLF01_STIP_DATA,strErrorMessage);
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
         memcpy( sTLFLimRecord.transaction_id, (LPCTSTR) sTLFList.tlf01_structure[nNumberReturned-1].primary_key.transaction_id, sizeof (sTLFLimRecord.transaction_id)); 
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
         for (int nSubItem = 0; nSubItem < 15; nSubItem++)
         {
            lvItem.mask = LVIF_TEXT ;
            lvItem.iItem = (nSubItem == 0)? nRowItem : nActualItem;
            lvItem.iSubItem = nSubItem;

            if (nSubItem == 0) { 
               lvItem.pszText = (char *)sTLFList.tlf01_structure[nItem].card_num;                   
            }
            else if (nSubItem == 1) { 
               lvItem.pszText = (char *)sTLFList.tlf01_structure[nItem].merchant_id;
            }
            else if (nSubItem == 2) { 
               lvItem.pszText = (char *)sTLFList.tlf01_structure[nItem].terminal_id;
            }
            else if (nSubItem == 3) { 
               if ( sTLFList.tlf01_structure[nItem].date_yyyymmdd[0] != 0x00 )
               {
   					temp = sTLFList.tlf01_structure[nItem].date_yyyymmdd;
                  temp = temp.Right(2) + "/" + temp.Mid(4,2) + "/" + temp.Left(4);
                  memcpy ( strTemp, temp, sizeof (strTemp));
                  lvItem.pszText = strTemp ;                    
               }
               else
                  lvItem.pszText = " ";
            }
            else if (nSubItem == 4) { 
               if ( sTLFList.tlf01_structure[nItem].time_hhmmss[0] != 0x00 )
               {
                  temp = sTLFList.tlf01_structure[nItem].time_hhmmss;
                  temp = temp.Left(2) + ":" + temp.Mid(2,2);
                  memcpy ( strTemp, temp, sizeof (strTemp));
                  lvItem.pszText = strTemp ;                    
               }
               else
                  lvItem.pszText = " ";
            }
            else if (nSubItem == 5) { 
               temp = sTLFList.tlf01_structure[nItem].total_amount;
               memcpy ( strTemp, myUtilsObject.DisplayAmount(temp), sizeof (strTemp));
               lvItem.pszText = strTemp ;
            }
            else if (nSubItem == 6) { 
               lvItem.pszText = (char *)sTLFList.tlf01_structure[nItem].response_code;
            }
 /*           else if (nSubItem == 7) { 
               lvItem.pszText = (char *)sTLFList.tlf01_structure[nItem].acquirer_id;
            }
            else if (nSubItem == 8) { 
               lvItem.pszText = (char *)sTLFList.tlf01_structure[nItem].issuer_id;
            }
            else if (nSubItem == 9) { 
               lvItem.pszText = (char *)sTLFList.tlf01_structure[nItem].sys_trace_audit_num;
            }
            else if (nSubItem == 10) { 
               lvItem.pszText = (char *)sTLFList.tlf01_structure[nItem].retrieval_ref_num;
            }
            else if (nSubItem == 11) { 
               lvItem.pszText = (char *)sTLFList.tlf01_structure[nItem].primary_key.transaction_id;
            }
            else if (nSubItem == 12) { 
               if ( sTLFList.tlf01_structure[nItem].system_date[0] != 0x00 )
               {
   					temp = sTLFList.tlf01_structure[nItem].system_date;
                  temp = temp.Right(2) + "/" + temp.Mid(4,2) + "/" + temp.Left(4);
                  memcpy ( strTemp, temp, sizeof (strTemp));
                  lvItem.pszText = strTemp ;   
               }
               else
                  lvItem.pszText = " ";
            }
			else if (nSubItem == 13){
				lvItem.pszText = " ";
			}*/else if (nSubItem == 7) { 
				lvItem.pszText = (char *)sTLFList.tlf01_structure[nItem].auth_number;
            }
            else if (nSubItem == 8) { 
               lvItem.pszText = (char *)sTLFList.tlf01_structure[nItem].acquirer_id;
            }
            else if (nSubItem == 9) { 
               lvItem.pszText = (char *)sTLFList.tlf01_structure[nItem].issuer_id;
            }
            else if (nSubItem == 10) { 
               lvItem.pszText = (char *)sTLFList.tlf01_structure[nItem].sys_trace_audit_num;
            }
            else if (nSubItem == 11) { 
               lvItem.pszText = (char *)sTLFList.tlf01_structure[nItem].retrieval_ref_num;
            }
            else if (nSubItem == 12) { 
               lvItem.pszText = (char *)sTLFList.tlf01_structure[nItem].primary_key.transaction_id;
            }
            else if (nSubItem == 13) { 
               if ( sTLFList.tlf01_structure[nItem].system_date[0] != 0x00 )
               {
   					temp = sTLFList.tlf01_structure[nItem].system_date;
                  temp = temp.Right(2) + "/" + temp.Mid(4,2) + "/" + temp.Left(4);
                  memcpy ( strTemp, temp, sizeof (strTemp));
                  lvItem.pszText = strTemp ;   
               }
               else
                  lvItem.pszText = " ";
            }
			else if (nSubItem == 14) {
				if ( sTLFList.tlf01_structure[nItem].date_yyyymmdd[0] != 0x00 && sTLFList.tlf01_structure[nItem].time_hhmmss[0] != 0x00)
				{
				  temp = sTLFList.tlf01_structure[nItem].system_date;
                  temp =  temp.Left(4)+ "-" +  temp.Mid(4,2) + "-" + temp.Right(2);

				  tempT = sTLFList.tlf01_structure[nItem].time_hhmmss;
                  tempT = temp + " " + tempT.Left(2) + ":" + tempT.Mid(2,2) + ":" + tempT.Right(2);
                  memcpy ( strTemp, tempT, sizeof (strTemp));
                  lvItem.pszText =   strTemp;              
     

				}
				else
				{
					lvItem.pszText = " ";
				}
			}
			/*else if (nSubItem == 13){
				lvItem.pszText = " ";
			}*/

            if (nSubItem == 0) 
               nActualItem = m_CntlList.InsertItem(nCount+nSize,(char *)sTLFList.tlf01_structure[nItem].card_num);
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

void CTranInquiry::PopulateFGuardListDialog()
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
   int nSize, nCount, nTotalCnt;
   nCount = m_CntlList.GetItemCount();
   CMyUtils myUtilsObject;

   UpdateData(TRUE);
   m_StrMessageLbl = "Please Wait";
   UpdateData(FALSE);

   int rcode = txdsapi_get_list ((pBYTE)&sTLFLimRecord,sizeof(TLF01_LIMITS), (pBYTE)&sFGuardList, 
                                 sizeof (FGUARD_GUI_LIST),ST2_TYPE,FGUARD_DATA,strErrorMessage);
   if (rcode == PTEMSG_OK)
   {
      nNumberReturned = atoi ((char *)&sFGuardList.num_returned);

      if ( 0 == nNumberReturned )
      {
         m_StrMessageLbl = "No Records Found";
         bDone = TRUE;
         nSize = 0;
		 nTotalCnt = 0;
		 m_DetailsButton.EnableWindow(FALSE);
         m_NextButton.EnableWindow(FALSE);
         UpdateData(FALSE);
      }
      else if( nNumberReturned == GUI_MAX_LIST_SIZE1)/*10 records*/
      {				
         memcpy( sTLFLimRecord.transaction_id, (LPCTSTR) sFGuardList.FGUARD_record[nNumberReturned-1].transaction_id, sizeof (sTLFLimRecord.transaction_id)); 
         nSize = nNumberReturned - 1;
		 nTotalCnt = nNumberReturned;
		 m_DetailsButton.EnableWindow(TRUE);
         m_NextButton.EnableWindow(TRUE);
         m_StrMessageLbl = "";
         UpdateData(FALSE);
      }
      else
      {
         m_StrMessageLbl = "End of List";
         bDone = TRUE;
         nSize = nNumberReturned;
		 nTotalCnt = nNumberReturned;
		 m_DetailsButton.EnableWindow(TRUE);
         m_NextButton.EnableWindow(FALSE);
       //  UpdateData(FALSE);
      }

      for (int nItem = 0; nItem <  nSize; nItem++,nRowItem ++)
      { // will now insert the fraud guard items and subitems into the list view.
         m_CntlList.EnableWindow(TRUE);  
         for (int nSubItem = 0; nSubItem < 14; nSubItem++)
         {
            lvItem.mask = LVIF_TEXT ;
            lvItem.iItem = (nSubItem == 0)? nRowItem : nActualItem;
            lvItem.iSubItem = nSubItem;

            if (nSubItem == 0) { 
               lvItem.pszText = (char *)sFGuardList.FGUARD_record[nItem].card_num;                   
            }
            else if (nSubItem == 1) { 
               lvItem.pszText = (char *)sFGuardList.FGUARD_record[nItem].merchant_id;
            }
            else if (nSubItem == 2) { 
               lvItem.pszText = (char *)sFGuardList.FGUARD_record[nItem].terminal_id;
            }
            else if (nSubItem == 3) { 
                if( sFGuardList.FGUARD_record[nItem].timestamp[0] != 0x00 )
               {
   				  temp = sFGuardList.FGUARD_record[nItem].timestamp;
                  temp = temp.Mid(8,2)   + "/" + temp.Mid(5,2)   + "/" + temp.Left(4);
                  memcpy ( strTemp, temp, sizeof (strTemp));
                  lvItem.pszText =  strTemp ;                    
               }
               else
                  lvItem.pszText = " ";
            }
            else if (nSubItem == 4) { 
                if( sFGuardList.FGUARD_record[nItem].timestamp[0] != 0x00 )
               {
                  temp = sFGuardList.FGUARD_record[nItem].timestamp;
                  temp = temp.Right(8);
                  memcpy ( strTemp, temp, sizeof (strTemp));
                  lvItem.pszText = strTemp ;                    
               }
               else
                  lvItem.pszText = " ";
            }
            else if (nSubItem == 5) { 
               temp = sFGuardList.FGUARD_record[nItem].amount_peso;
               memcpy ( strTemp, myUtilsObject.DisplayAmount(temp), sizeof (strTemp));
               lvItem.pszText = strTemp ;
            }
            else if (nSubItem == 6) { 
               lvItem.pszText = (char *)sFGuardList.FGUARD_record[nItem].response_code;
            }
            else if (nSubItem == 7) { 
               lvItem.pszText = (char *)sFGuardList.FGUARD_record[nItem].acquiring_id;
            }
            else if (nSubItem == 8) { 
               lvItem.pszText = " ";
            }
            else if (nSubItem == 9) { 
               lvItem.pszText = (char *)sFGuardList.FGUARD_record[nItem].sys_trace_audit_num;
            }
            else if (nSubItem == 10) { 
               lvItem.pszText = " ";
            }
            else if (nSubItem == 11) { 
               lvItem.pszText = (char *)sFGuardList.FGUARD_record[nItem].transaction_id;
            }
            else if (nSubItem == 12) { 
               if ( sFGuardList.FGUARD_record[nItem].system_date[0] != 0x00 )
               {
   					temp = sFGuardList.FGUARD_record[nItem].system_date;
                  temp = temp.Right(2) + "/" + temp.Mid(4,2) + "/" + temp.Left(4);
                  memcpy ( strTemp, temp, sizeof (strTemp));
                  lvItem.pszText = strTemp ;   
               }
               else
                  lvItem.pszText = " ";
            }
			else if (nSubItem == 13){
				lvItem.pszText = (char *)sFGuardList.FGUARD_record[nItem].result_code;
			}

            if (nSubItem == 0) 
               nActualItem = m_CntlList.InsertItem(nCount+nSize,(char *)sFGuardList.FGUARD_record[nItem].card_num);
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

   /*Enable Resend Button*/
   if(m_CntlViewFailedRadio.GetCheck() == CHECKED)
		m_CntlResendButton.EnableWindow(TRUE);
   else
	    m_CntlResendButton.EnableWindow(FALSE);

   UpdateData(FALSE);

}/*populate_fguard_list_dialog*/

BOOL CTranInquiry::ValidateTime ( CString strTime )
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

BOOL CTranInquiry::ValidateDate ( CString strDate )
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

void CTranInquiry::OnDblclkList(NMHDR* pNMHDR, LRESULT* pResult) 
{
    OnDetailsButton(); 	

	*pResult = 0;
}

void CTranInquiry::OnEMVRAWRadio() 
{
	// This is check box feature is not used.
	/*UpdateData(TRUE);

	if(m_CntlEmvRawRadio.GetCheck() == CHECKED)
	{
		m_bViewEmvData = true;
		UpdateData(FALSE);
	}*/
	
	return;
}

void CTranInquiry::OnPosSearchCheck() 
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
		m_CntlFGuardSearchCheck.SetCheck(FALSE);

		m_CntlEmvRawRadio.EnableWindow(FALSE);
		m_CntlEmvRawRadio.SetCheck(FALSE);
		m_StrPosSearchCheck = true;
		m_StrSTIPSearchCheck= false;
		m_StrFGuardSearchCheck = false;
	    m_bFGuard = false;
		m_bTran = true;
		m_bTran_stip = false;
	}
	
	else if(m_CntlPosSearchCheck.GetCheck() == UNCHECKED && m_CntlSTIPSearchCheck.GetCheck() == UNCHECKED )
	{
		m_bTran = false;
		m_bTran_stip = false;
	}

	  m_bViewEmvData = false;

  UpdateData(FALSE);
	return;
}

void CTranInquiry::OnTLF01STIPSearchCheck()
{
	UpdateData(TRUE);
	//ramya
	 if(m_CntlSTIPSearchCheck.GetCheck() == CHECKED)
	{
		m_CntlViewAllRadio.EnableWindow(FALSE);
		m_CntlViewAcceptedRadio.EnableWindow(FALSE);
		m_CntlViewFailedRadio.EnableWindow(FALSE);
		m_CntlViewFGRawRadio.SetCheck(UNCHECKED);
		m_CntlViewFGRawRadio.EnableWindow(FALSE);
		m_CntlResendButton.EnableWindow(FALSE);
		m_CntlFGuardSearchCheck.SetCheck(FALSE);

		m_CntlEmvRawRadio.EnableWindow(FALSE);
		m_CntlEmvRawRadio.SetCheck(FALSE);
		m_StrSTIPSearchCheck = true;
		m_StrPosSearchCheck = false;
		m_StrFGuardSearchCheck = false;
	    m_bFGuard = false;
		m_bTran = false;
		m_bTran_stip= true;
	}
	 	else if(m_CntlSTIPSearchCheck.GetCheck() == UNCHECKED && m_CntlPosSearchCheck.GetCheck() == UNCHECKED)
		{
			m_bTran = false;
			m_bTran_stip = false;
		}
	 m_bViewEmvData = false;

  UpdateData(FALSE);
	return;
}

void CTranInquiry::OnFguardSearchCheck() 
{
	UpdateData(TRUE);

	/*if(m_CntlFGuardSearchCheck.GetCheck() == CHECKED)
	{
		m_CntlViewAllRadio.EnableWindow(TRUE);

		m_CntlViewAcceptedRadio.EnableWindow(TRUE);
		m_CntlViewAcceptedRadio.SetCheck(UNCHECKED);
		m_CntlViewFailedRadio.EnableWindow(TRUE);
		m_CntlViewFailedRadio.SetCheck(UNCHECKED);
		m_CntlResendButton.EnableWindow(FALSE);
		m_CntlViewAllRadio.SetCheck(CHECKED);
		m_CntlViewFGRawRadio.EnableWindow(TRUE);
		m_CntlViewFGRawRadio.SetCheck(UNCHECKED);
		m_CntlPosSearchCheck.SetCheck(UNCHECKED);
		//ramya
		m_CntlSTIPSearchCheck.SetCheck(UNCHECKED);
		m_CntlEmvRawRadio.EnableWindow(FALSE);
		m_CntlEmvRawRadio.SetCheck(UNCHECKED);
		m_StrPosSearchCheck = false;
		m_StrSTIPSearchCheck = false;

		m_StrFGuardSearchCheck = true;
	    m_bFGuard = true;
		m_bTran = false;
	}
	else
	{
		m_CntlViewAllRadio.EnableWindow(FALSE);
		m_CntlViewAcceptedRadio.EnableWindow(FALSE);
		m_CntlViewFailedRadio.EnableWindow(FALSE);
		m_CntlResendButton.EnableWindow(FALSE);
		m_bFGuard = false;

	}*/

  m_bViewRawData = false;


  UpdateData(FALSE);
	return;
}



void CTranInquiry::OnViewFgrawRadio() 
{
	m_bViewRawData = true;
	UpdateData(FALSE);
	return;
}

void CTranInquiry::OnViewAllradio() 
{
	ST2_TYPE = ST2_NONE;
}

void CTranInquiry::OnViewAcceptedRadio() 
{
	ST2_TYPE = ST2_GET_ALL_FG_SUCCESS;
}

void CTranInquiry::OnViewFailedRadio() 
{
	ST2_TYPE = ST2_GET_ALL_FG_FAILED;
}

void CTranInquiry::OnResendButton() 
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

LRESULT CTranInquiry::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
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


int CTranInquiry::StartTimer(int time)
{
	int handle = SetTimer (IDT_TIMER, time, NULL);  // Start the Timer

	return handle;
}

bool CTranInquiry::StopTimer()
{

	if (!KillTimer (timer_handle))	//Stop Timer	
	{
				::MessageBox(NULL,"Error While Killing the Timer","ShutEvt",1);	
		return false;
	}

	return true;
}

BOOL CTranInquiry::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo) 
{
	// TODO: Add your specialized code here and/or call the base class
	ResetCntr();
	c_time = CTime::GetCurrentTime();	
	shrmem.SetLastUpdateTime(c_time);
	return CDialog::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}



void CTranInquiry::OnButtonExit() 
{
	// TODO: Add your control notification handler code here		
				
		//Delete the associated Mutex lock 
		shrmem.DeleteMutexLock();
		CDialog::OnCancel();
}



short CTranInquiry::LockScreen()
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

void CTranInquiry::ResetCntr()
{
	cntr = 0;
}

void CTranInquiry::OnMaskCardNumberChk() 
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

 void CTranInquiry::get_time( pCHAR time_date, pCHAR time_str )
 {
    memset ( time_str,  0x00,          7 );
    strncpy( time_str, &time_date[11], 2 );
    strncat( time_str, &time_date[14], 2 );
    strncat( time_str, &time_date[17], 2 );
 }
void CTranInquiry::OnBnClickedTlfExtract()
{
	// TODO: Add your control notification handler code here

	   CMyUtils  myUtilsObject;
	   CTransactionDetails Class_transactiondetails;
  char decimal_tran_amount[50]={0};
  char str1[526] = ""; 
   CString temp=_T("");
   BYTE    all_fields_empty = true;
   char Tlf_extractfilename[100] = {0};
   char timestamp[40] = {0};
    FILE			*m_pFile;
	char szCurDir[200] = {0};
	unsigned long	m_lRecordsWritten=0;
	CTime theTime = CTime::GetCurrentTime();
	int nNumberReturned = 0;
	int rcode=0;
	char strErrorMessage[256]={0};
	INT nSize=0;
		CHAR     strFactor[50] = {0};
	double   nFactor   = 0;
		CHAR     strMPFlag[2] ="1";

	char processing_flag[20]={0};
	char card_num[20]={0};
		CHAR inifilename[100] = {0};
   CHAR configfilepath[100] = {0};
   CHAR tmpstr[256]={0};
   char temp_file[256]={0};


	CString transaction_date=_T("");
	CString transaction_time=_T("");
	CString scheme_tarnsaction_id=_T("");
	CString term=_T("");
	CString factor=_T("");
	CString monthly_amortization=_T("");
	CString eci=_T("");
	CString orig_currency_code=_T("");
	CString cup_indicator=_T("");
	CString auth_profile=_T("");
	CString hostname=_T("");
	CString msg_type=_T("");
	CString invoice_number=_T("");
	CString transaction_amount=_T("");
	CString currency_code =_T("");
	CString response_code=_T("");
	CString auth_number=_T("");
	CString retrieval_ref_num=_T("");
	CString settlement_date=_T("");
	CString pos_entry_mode=_T("");
	CString pos_data =_T("");
	CString service_code=_T("");
	CString	orig_amount=_T("");
	CString acquiring_id=_T("");
	CString forwarding_institution_id =_T("");
	CString system_date=_T("");
	CString merchnat_id=_T("");
	CString terminal_id=_T("");
	CString exp_date=_T("");
	CString m_StrFactorFld=_T("");
	CString m_StrMonthlyAmortFld=_T("");
	CString sys_trace_audit_num=_T("");
	CString m_StrNRID=_T("");
	TRANS_RECORD sTranRecord ;
   m_CntlList.DeleteAllItems();
   unsigned long	m_lTotalRecords=0;
   char time_date[25]={0};
   char time_str[15]={0};
   UpdateData(TRUE);
   ZeroMemory ( &sTLFLimRecord, sizeof ( TLF01_LIMITS ) );

   /* Get system timestamp "YYYY-MM-DD-hh.mm.ss.jjj" */
    ptetime_get_timestamp( time_date );
	 get_time( time_date, time_str );
   /* START DATE */
   if ( !myUtilsObject.IsEmpty(m_StrDateStartFld, this) )
   {
      all_fields_empty = false;
	  strcpy(str1, "Start date:");
	  strcat(str1,m_StrDateStartFld);
	  strcat(str1,"\n");
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
	  strcat(str1, "End date:");
	  strcat(str1,m_StrDateEndFld);
	  strcat(str1,"\n");
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
	    strcat(str1, "Start Time:");
	  strcat(str1,m_StrTimeStartFld);
	  strcat(str1,"\n");
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
	  	    strcat(str1, "End Time:");
	  strcat(str1,m_StrTimeEndFld);
	  strcat(str1,"\n");
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
	  	  	    strcat(str1, "Card Num:");
	  strcat(str1,m_StrCardNumberFld);
	  strcat(str1,"\n");
      memcpy ( sTLFLimRecord.card_num_min,
               m_StrCardNumberFld,
               sizeof (sTLFLimRecord.card_num_min) );
   }

      /* ----------- */
   /* AUTH NUMBER */
   /* ----------- */
   if ( !(myUtilsObject.IsEmpty(m_StrAuthNumberFld, this)) ) // empty field
   {
      all_fields_empty = false;
	  	  	  	    strcat(str1, "Auth Num:");
	  strcat(str1,m_StrAuthNumberFld);
	  strcat(str1,"\n");
      memcpy ( sTLFLimRecord.auth_number_min,
               m_StrAuthNumberFld,
               sizeof (sTLFLimRecord.auth_number_min) );
  
   
		int len = m_StrAuthNumberFld.GetLength();
		char *str = m_StrAuthNumberFld.GetBuffer(len);
		if(len < 6)
		{
			m_StrMessageLbl = "The length of Auth ID should not be less than 6 digits";	
			m_CntlAuthNumberFld.SetFocus();
			(UpdateData(FALSE));
			return;
		}
		for(int i=0;i<len;i++)
		{
			if((str[i] > 64 && str[i] < 91) || (str[i] > 47 && str[i] < 58) )
			{
				//The character is a ALPHABHET or NUMBER
			}
			else
			{
				m_StrMessageLbl = "No Special characters allowed in Auth ID";	
				m_CntlAuthNumberFld.SetFocus();
				
				 (UpdateData(FALSE));
						
				
				m_StrAuthNumberFld.ReleaseBuffer(len);

				return;
			}
   
   
		}

		if((myUtilsObject.IsEmpty(m_StrDeviceIDFld, this)) && 
			(myUtilsObject.IsEmpty(m_StrMerchantIDFld, this)) && 
			(myUtilsObject.IsEmpty(m_StrCardNumberFld, this)))
		{
			m_StrMessageLbl = "Enter at least one other parameter with Auth Id: Card Number, Merchant ID or Device ID";
			UpdateData(FALSE);
			return;
		}
   
   }
   /**********************************************************************************/

    /* ----------- */
   /* HOSTNAME */
   /* ----------- */
   if ( !(myUtilsObject.IsEmpty(m_StrHostNameFld, this)) ) // empty field
   {
      all_fields_empty = false;
	  memcpy ( sTLFLimRecord.redemption_amount,
               m_StrHostNameFld,
               sizeof (sTLFLimRecord.redemption_amount) );
   
   }


  /***********************************************************************************/

   /* ----------- */
   /* TERMINAL ID */
   /* ----------- */
   if ( !(myUtilsObject.IsEmpty(m_StrDeviceIDFld, this)) ) // empty field
   {
      all_fields_empty = false;
	  strcat(str1, "Device ID:");
	  strcat(str1,m_StrDeviceIDFld);
	  strcat(str1,"\n");
      memcpy ( sTLFLimRecord.terminal_id_min,
               m_StrDeviceIDFld,
               sizeof (sTLFLimRecord.terminal_id_min));
   }

   /* ----------- */
   /* MERCHANT ID */
   /* ----------- */
   if ( !(myUtilsObject.IsEmpty(m_StrMerchantIDFld, this)) ) // empty field
   {
      all_fields_empty = false;
	  strcat(str1, "Merchant ID:");
	  strcat(str1,m_StrMerchantIDFld);
	  strcat(str1,"\n");
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


   CString str;

	str.Format(" Following detials are used to Extract the TLF details:\n\n"
				 "%s"
		         "Are you sure to Extract TLF details",str1);
	INT ret = AfxMessageBox( str , MB_YESNO | MB_ICONQUESTION );
		if( ret == IDNO )
			return ;

	/*CFileDialog fd(FALSE, "csv",NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_ENABLESIZING , "Comma Seperated files (*.csv) | *.csv",this);
	if(fd.DoModal() == IDOK)
	{
		SetCurrentDirectory(szCurDir);
		CString filename = fd.GetPathName();
		m_pFile = fopen(filename, "w");
		if(m_pFile == NULL)
		{
				m_CntlStatus.SetWindowText("ERROR in opening the file");
			return ;
		}
	

	fprintf(m_pFile,"REPORT NAME:, TLF Extract\n");
	fprintf(m_pFile,"DATE:,%02d-%02d-%04d\n",theTime.GetMonth(), theTime.GetDay(), theTime.GetYear());
	
	fprintf(m_pFile,"Merchant ID, Terminal ID, Message Type, Card Number, Expiration Date, Invoice Number,Transaction Date, Transaction Time, Transaction Amount ,"
		            "Transaction Currency, Response Code    , Approval Code, Retrival Reference Number, STAN/Trace ,"
		            "Transcation id, Settle Date, Term     , RateFactor(%)       ,"
					"Monthly Amortization    , POS Entry Mode, POS data, ECI, Service Code, Original Transcation Amount ,"
					"Original Transcation Currency, CUP Indicator, Acquiring Institution ID, Forwarding Institution ID ,"
					"Auth Profile, System Date, Hostname, Host Processed/ATP STIP Processed             \n");
	
	}
	else 
	{
		SetCurrentDirectory(szCurDir);
		return;
	}
	*/

   	 GetAscendentConfigDirectory(tmpstr);
	 sprintf(inifilename, "%stf.ini", tmpstr);
   GetPrivateProfileString(
        "TRANLOOKUP_REP_FILE_PATH", 	               // points to section name 
        "REPORT_PATH",	       // points to key name 
        "1",	                   // points to default string 
        configfilepath,                   // points to destination buffer 
        sizeof(configfilepath) - 1,       // size of destination buffer 
        inifilename ); 

	
	sprintf(temp_file,"%s\\tranlookuprep_%02d%02d%04d%s.csv",configfilepath,theTime.GetMonth(),theTime.GetDay(),theTime.GetYear(),time_str);
	CString filename=temp_file; 
	m_pFile = fopen(filename, "w");
	if(m_pFile == NULL)
	{
	    m_StrMessageLbl = "ERROR in opening the file, please verify the configured path";
	    UpdateData(FALSE);
		return ;
	}
	

	fprintf(m_pFile,"REPORT NAME:, TLF Extract\n");
	fprintf(m_pFile,"DATE:,%02d-%02d-%04d\n",theTime.GetMonth(), theTime.GetDay(), theTime.GetYear());
	
	fprintf(m_pFile,"Merchant ID, Terminal ID, Message Type, Card Number, Expiration Date, Invoice Number,Transaction Date, Transaction Time, Transaction Amount ,"
		            "Transaction Currency, Response Code    , Approval Code, Retrieval Reference Number, STAN/Trace ,"
		            "Transaction id, Settle Date, Term     , RateFactor(%)       ,"
					"Monthly Amortization    , POS Entry Mode, POS data, ECI, Service Code, Original Transaction Amount ,"
					"Original Transaction Currency, CUP Indicator, Acquiring Institution ID, Forwarding Institution ID ,"
					"Auth Profile, System Date, Hostname, Host Processed/ATP STIP Processed             \n");
	do
	{
		BeginWaitCursor();
		nNumberReturned = 0;

	   if(m_StrPosSearchCheck)
	   {
		  
		    rcode = txdsapi_get_list ((pBYTE)&sTLFLimRecord,sizeof(TLF01_LIMITS), (pBYTE)&sTLFList, 
                                 sizeof (TLF01_GUI_LIST),0,TLF01_DATA,strErrorMessage);
	   }
	   else if(m_StrSTIPSearchCheck)
	   {
		  
		   rcode = txdsapi_get_list ((pBYTE)&sTLFLimRecord,sizeof(TLF01_LIMITS), (pBYTE)&sTLFList, 
                                 sizeof (TLF01_GUI_LIST),ST2_DB_SELECT_STANDIN,TLF01_STIP_DATA,strErrorMessage);
	   }

	   if (rcode == PTEMSG_OK) 
		{	 
		   nNumberReturned = atoi ((char *)&sTLFList.num_returned);

			if( nNumberReturned == GUI_MAX_LIST_SIZE1)
			{				
				//CString strTemp = sTLFList.tlf01_structure[nNumberReturned-1].primary_key.unique_id ;
				//strTemp.TrimRight() ; strTemp.TrimLeft() ;
				memcpy ( &sTLFLimRecord.transaction_id, &sTLFList.tlf01_structure[nNumberReturned-1].primary_key.transaction_id, sizeof (sTLFLimRecord.transaction_id));

				nSize = nNumberReturned - 1;
			}
			else if(nNumberReturned == 0 )
			{
				break;
			}
			else 
			{
				nSize = nNumberReturned;
			}
			for (  int i = 0; i < nSize; i++ ) 
			{
					m_lTotalRecords++;
					memset(&sTranRecord,0x00,sizeof(sTranRecord));
			
				merchnat_id=_T("");
				terminal_id=_T("");
				msg_type=_T("");
				exp_date=_T("");
				invoice_number=_T("");
				transaction_date=_T("");
				transaction_time=_T("");
				transaction_amount=_T("");
				currency_code=_T("");
				response_code=_T("");
				auth_number=_T("");
				retrieval_ref_num=_T("");
				sys_trace_audit_num=_T("");
				m_StrNRID=_T("");
				settlement_date=_T("");
				term=_T("");
				m_StrFactorFld=_T("");
				monthly_amortization=_T("");
				pos_entry_mode=_T("");
				pos_data=_T("");
				eci=_T("");
				service_code=_T("");
				orig_amount=_T("");
				orig_currency_code=_T("");
				cup_indicator=_T("");
				acquiring_id=_T("");
				forwarding_institution_id=_T("");
				auth_profile=_T("");
				system_date=_T("");
				hostname=_T("");
				memset(processing_flag,0x00,sizeof(processing_flag));
				memset(card_num,0x00,sizeof(card_num));

					strncpy((char *)sTranRecord.tlf01.primary_key.transaction_id,(char *)sTLFList.tlf01_structure[i].primary_key.transaction_id,
						sizeof(sTranRecord.tlf01.primary_key.transaction_id)-1);
					rcode = txdsapi_get_record ( (pBYTE)&sTranRecord, sizeof (sTranRecord), TLF01_DATA, strErrorMessage);
					/* card number */
					CString str1_card_num = sTranRecord.tlf01.card_num;
					int len = str1_card_num.GetLength();
					if(m_MaskCardNumber.GetCheck() != 0)
					{
						if(len > 0)
						{
							char *pStr = str1_card_num.GetBuffer(len);
							int card_num_len=str1_card_num.GetLength();
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
						}
						
					}
					str1_card_num ="\t"+str1_card_num;

					/* scheme transaction id*/
					
					if(0 == strncmp((char *)sTranRecord.tlf01.handler_queue,"dcpimp",6))
					{
						m_StrNRID = sTranRecord.tlf01.product_codes[17].code;
						m_StrNRID += "/";
						m_StrNRID += (CString)(char *)sTranRecord.tlf01.product_codes[17].amount;  
						m_StrNRID += (CString)(char *)sTranRecord.tlf01.product_codes[18].amount;
						m_StrNRID += (CString)(char *)sTranRecord.tlf01.product_codes[18].code;
						if(m_StrNRID == "/")
						{
							m_StrNRID=_T("");
						}
					}
					else if  (true ==  Class_transactiondetails.Check_If_AMEX_Transaction((char *)sTranRecord.tlf01.card_num)) 
					{
						if(sTranRecord.tlf01.visa_tran_id[0] != '\0')
						{
							m_StrNRID = sTranRecord.tlf01.visa_tran_id;	
						}	
						else
						{ 
							m_StrNRID = sTranRecord.tlf01.mcard_banknet;	
						}
					}
					else if(true ==  Class_transactiondetails.Check_If_Diners_Transaction((char *)sTranRecord.tlf01.card_num) )
					{
						m_StrNRID = sTranRecord.tlf01.resp_source;
					}
					else if (true == Class_transactiondetails.Check_If_VISA_Transaction((char *)sTranRecord.tlf01.card_num))
					{
						m_StrNRID = sTranRecord.tlf01.visa_tran_id;	
					}
					else if(true == Class_transactiondetails.Check_If_MC_Transaction((char *)sTranRecord.tlf01.card_num))
					{
						m_StrNRID = sTranRecord.tlf01.mcard_banknet;	
					}
					/* copy the details as text into xl sheet*/
					m_StrNRID="\t"+m_StrNRID;
						/*Term, factor and monthly amortization*/
					
				if ( 0x00 != sTranRecord.tlf01.deferred_term_length[0] )
				{
					temp = sTranRecord.tlf01.monthly_amort_amt;
					m_StrMonthlyAmortFld =  myUtilsObject.DisplayAmount(temp);
		
					term =sTranRecord.tlf01.deferred_term_length;
					/* Convert Def Factor from nnnnnnn to nnn.nnnn (0050000 = 5%). */
					m_StrFactorFld = sTranRecord.tlf01.deferred_factor;
					if((0 == strncmp((char *)sTranRecord.tlf01.product_codes[2].quantity,strMPFlag,1)))
					{
						nFactor = atof( m_StrFactorFld ) / 1000000.0;
						sprintf( strFactor, "%8.6f", nFactor );
					}
					else
					{
						nFactor = atof( m_StrFactorFld ) / 10000.0;
						sprintf( strFactor, "%8.4f", nFactor );
					}
					m_StrFactorFld = strFactor;
				}

				temp ="\t"+term;
				m_StrFactorFld= "\t"+m_StrFactorFld;
				m_StrMonthlyAmortFld="\t"+m_StrMonthlyAmortFld;
				/*eci*/		

				if(Class_transactiondetails.Check_If_MC_Transaction((char *)sTranRecord.tlf01.card_num))
				{
					eci='2';
				}

				if(sTranRecord.tlf01.profile[0]!='\0')
				{
					eci=eci+sTranRecord.tlf01.profile;
					eci="\t"+eci;
				}
				else
				{
					eci=_T("");
				}

				
				/*orig currency*/

				if(strcmp((char *)sTranRecord.tlf01.handler_queue,"dcpiso") != 0)
				{
					if(strcmp((char *)sTranRecord.tlf01.product_codes[4].code,"ISSUE") == 0)
					{
						if(sTranRecord.tlf01.nfi_seq_nbr[0] != 0x00)
						{
							orig_currency_code =sTranRecord.tlf01.nfi_seq_nbr;
						}
						else
						{
							orig_currency_code =sTranRecord.tlf01.currency_code;
						}
					}
					else
					{
						orig_currency_code =sTranRecord.tlf01.currency_code;
					}
				}
				else
				{
					orig_currency_code =sTranRecord.tlf01.currency_code;
				}

				orig_currency_code= "\t"+orig_currency_code;

				/*CUP indicator*/
				cup_indicator = sTranRecord.tlf01.product_code;
				cup_indicator ="\t"+cup_indicator;
				/* auth profile*/
				auth_profile=sTranRecord.tlf01.product_codes[3].code;
			    auth_profile ="\t" +auth_profile;
				/*hostname*/
				hostname=sTranRecord.tlf01.redemption_amount;
				hostname= "\t"+hostname;
			    /* processing flag*/
				if(	strcmp((char *)sTranRecord.tlf01.product_codes[1].code, STANDIN) == 0)
				{
					strncpy(processing_flag,"STIP Processed",14);
				}
				else
				{
					strncpy(processing_flag,"HOST processed",14);
				}
				merchnat_id= sTranRecord.tlf01.merchant_id;
				terminal_id = sTranRecord.tlf01.terminal_id;
				 msg_type = sTranRecord.tlf01.message_type;
				 invoice_number = sTranRecord.tlf01.invoice_number;
				exp_date=	 sTranRecord.tlf01.exp_date;
				transaction_date = sTranRecord.tlf01.date_yyyymmdd;
				transaction_time = sTranRecord.tlf01.time_hhmmss;
				/* sprintf(decimal_tran_amount, "%.10s.%.2s", (char*)sTranRecord.tlf01.total_amount, (char*)sTranRecord.tlf01.total_amount+10);
				 transaction_amount = decimal_tran_amount; */
				 temp = _T("");
				 temp = sTranRecord.tlf01.total_amount;
				 transaction_amount =  myUtilsObject.DisplayAmount(temp);
				 currency_code = sTranRecord.tlf01.currency_code;
				 response_code = sTranRecord.tlf01.response_code;
				 auth_number = sTranRecord.tlf01.auth_number;
				 sys_trace_audit_num= sTranRecord.tlf01.sys_trace_audit_num;
				 retrieval_ref_num= sTranRecord.tlf01.retrieval_ref_num;
				 settlement_date =sTranRecord.tlf01.settlement_date;
				 pos_entry_mode = sTranRecord.tlf01.pos_entry_mode;
				 pos_data = sTranRecord.tlf01.product_codes[16].amount;
				 service_code =sTranRecord.tlf01.service_code;
				 orig_amount = sTranRecord.tlf01.orig_amount;
				 acquiring_id = sTranRecord.tlf01.acquiring_id;
				 forwarding_institution_id = sTranRecord.tlf01.forwarding_institution_id;
				 system_date = sTranRecord.tlf01.system_date;

				 /*copies the data as text into XL*/
				merchnat_id="\t"+ merchnat_id;
				terminal_id="\t"+terminal_id;
				msg_type="\t"+msg_type;
				invoice_number="\t"+invoice_number;
				exp_date="\t"+exp_date;
				transaction_date="\t"+transaction_date;
				transaction_time="\t"+transaction_time;
				transaction_amount="\t"+transaction_amount;
				currency_code="\t"+currency_code;
				response_code="\t"+response_code;
				auth_number="\t"+auth_number;
				sys_trace_audit_num="\t"+sys_trace_audit_num;
				retrieval_ref_num="\t"+retrieval_ref_num;
				settlement_date="\t"+settlement_date;
				pos_entry_mode="\t"+pos_entry_mode;
				pos_data="\t"+pos_data;
				service_code="\t"+service_code;
				orig_amount="\t"+orig_amount;
				acquiring_id="\t"+acquiring_id;
				forwarding_institution_id="\t"+forwarding_institution_id;
				system_date="\t"+system_date;
				
				fprintf(m_pFile,"%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s",
				merchnat_id,
				terminal_id,
				msg_type,
				str1_card_num,
				exp_date,
				invoice_number,
				transaction_date,
				transaction_time,
				transaction_amount,
				currency_code,
				response_code,
				auth_number,
				retrieval_ref_num,
				sys_trace_audit_num,
				m_StrNRID,
				settlement_date,
				term,
				m_StrFactorFld,
				monthly_amortization,
				pos_entry_mode,
				pos_data,
				eci,
				service_code,
				orig_amount,
				orig_currency_code,
				cup_indicator,
				acquiring_id,
				forwarding_institution_id,
				auth_profile,
				system_date,
				hostname,
				processing_flag);
				fprintf(m_pFile,"\n");
				//Sleep(300);
			}
	   }
	   else
	   {
			nNumberReturned = 0;
	   }


	}
	while(nNumberReturned == GUI_MAX_LIST_SIZE1);
	fprintf(m_pFile,"\n\nTOTAL RECORDS:,%lu,\n",m_lTotalRecords);



	EndWaitCursor();



	fclose(m_pFile);
	
		ret = AfxMessageBox( "Report Genaration complete" , MB_OK | MB_ICONQUESTION );
		if( ret == IDNO )
			return ;
	
   // return true;
}
