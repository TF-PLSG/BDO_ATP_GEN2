// OverrideMerchantDenial.cpp : implementation file
//

#include "stdafx.h"
#include "atp.h"
#include "OverrideMerchantDenialPropertySheet.h"
#include "OverrideMerchantDenial.h"
#include "BinList.h"
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

extern CString strMerchantID;
extern CString strOrganizationID;
extern CSharedMem shrmem;
extern UCF01 ucf01;


static int counter= 0;
/////////////////////////////////////////////////////////////////////////////
// COverrideMerchantDenial property page

IMPLEMENT_DYNCREATE(COverrideMerchantDenial, CPropertyPage)

COverrideMerchantDenial::COverrideMerchantDenial() : CPropertyPage(COverrideMerchantDenial::IDD)
{
	//{{AFX_DATA_INIT(COverrideMerchantDenial)
	m_StrCardNumber = _T("");
	m_StrMerchantId = _T("");
	m_StrMessageLbl = _T("");
	m_StrRepeat		= _T("0");
	m_StrExpiryDate	= _T("");
	m_StrOverrideUsed = _T("0");
	m_StrPesoAmountLimit = _T("");
	m_StrUsdAmountLimit = _T("");
	//}}AFX_DATA_INIT
}

COverrideMerchantDenial::~COverrideMerchantDenial()
{
}

void COverrideMerchantDenial::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(COverrideMerchantDenial)
	DDX_Control(pDX, IDC_NEVER_EXPIRES, m_ChkNeverExpires);
	DDX_Control(pDX, IDC_BYPASS_MID, m_ChkBypassMID);
	DDX_Control(pDX, IDC_BYPASS_MCC, m_ChkBypassMCC);
	DDX_Control(pDX, IDC_BYPASS_GLOBAL, m_ChkBypassGlobal);
	DDX_Control(pDX, IDC_AMOUNTLIMIT_USD, m_CntlUsdAmountLimit);
	DDX_Control(pDX, IDC_AMOUNTLIMIT_PESO, m_CntlPesoAmountLimit);
	DDX_Control(pDX, IDC_UPDATE, m_CntlUpdate);
	DDX_Control(pDX, IDC_NO_OF_TIMES, m_CntlRepeat);
	DDX_Control(pDX, IDC_DATETIMEPICKER1, m_CntlExpiryDate);
	DDX_Control(pDX, IDC_LISTOVERRIDE, m_CntlListOverrides);
	DDX_Control(pDX, IDLIST, m_CntlListMerchants);
	DDX_Control(pDX, IDC_DELETE, m_CntlDelete);
	DDX_Control(pDX, IDADD, m_CntlAdd);
	DDX_Control(pDX, IDC_MERCHANT_ID_FLD, m_CntlMerchantId);
	DDX_Control(pDX, IDC_CARD_NBR_FLD, m_CntlCardNumber);
	DDX_Text(pDX, IDC_CARD_NBR_FLD, m_StrCardNumber);
	DDV_MaxChars(pDX, m_StrCardNumber, 16);
	DDX_Text(pDX, IDC_MERCHANT_ID_FLD, m_StrMerchantId);
	DDV_MaxChars(pDX, m_StrMerchantId, 15);
	DDX_Text(pDX, IDC_MESSAGE_LBL, m_StrMessageLbl);
	DDX_Text(pDX, IDC_NO_OF_TIMES, m_StrRepeat);
	DDX_Text(pDX, IDC_REPEATED, m_StrOverrideUsed);
	DDX_Text(pDX, IDC_AMOUNTLIMIT_PESO, m_StrPesoAmountLimit);
	DDV_MaxChars(pDX, m_StrPesoAmountLimit, 12);
	DDX_Text(pDX, IDC_AMOUNTLIMIT_USD, m_StrUsdAmountLimit);
	DDV_MaxChars(pDX, m_StrUsdAmountLimit, 12);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(COverrideMerchantDenial, CPropertyPage)
	//{{AFX_MSG_MAP(COverrideMerchantDenial)
	ON_BN_CLICKED(IDADD, OnAdd)
	ON_BN_CLICKED(IDLIST, OnListMerchant)
	ON_BN_CLICKED(IDC_LISTOVERRIDE, OnListoverride)
	ON_BN_CLICKED(IDC_DELETE, OnDelete)
	ON_BN_CLICKED(IDC_UPDATE, OnUpdate)
	ON_BN_CLICKED(IDC_NEVER_EXPIRES, OnNeverExpires)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// COverrideMerchantDenial message handlers

BOOL COverrideMerchantDenial::OnApply() 
{
	// TODO: Add your specialized code here and/or call the base class
	
	return CPropertyPage::OnApply();
}

void COverrideMerchantDenial::OnAdd() 
{
	int rcode;
	char strErrorMessage[200] = {0};
	UpdateData(TRUE);
	m_StrMessageLbl = "";
	char strBypass[9] = {0};
	
	CleanGeneralDialog();
	GetExpiryDate(m_StrExpiryDate);
	if((0 == CheckMerchantId()))
	{
		OMC01 sOMC01Record;
		ZeroMemory(&sOMC01Record,sizeof(OMC01)) ;
		memcpy ((LPSTR)(sOMC01Record.primary_key.merchant_id),	m_StrMerchantId, sizeof (sOMC01Record.primary_key.merchant_id));
		memcpy ((LPSTR)(sOMC01Record.primary_key.card_nbr),		m_StrCardNumber, sizeof (sOMC01Record.primary_key.card_nbr) );   
		rcode = txdsapi_get_record( (pBYTE)&sOMC01Record,sizeof (OMC01), OMC01_DATA, strErrorMessage );
		if(rcode == PTEMSG_OK)
		{
			if(strcmp((LPSTR)sOMC01Record.primary_key.merchant_id, m_StrMerchantId) == 0 && strcmp((LPSTR)sOMC01Record.primary_key.card_nbr, m_StrCardNumber) == 0)
			{
				//Same record exists
				//No action here in next else the message is displayed
			}
			else
			{
				rcode = PTEMSG_NOT_FOUND;
			}
		}
		if( rcode == PTEMSG_NOT_FOUND )
		{
			if(m_ChkNeverExpires.GetCheck() == 1)
			{
				m_StrExpiryDate.Empty();
			}
			memcpy ((LPSTR)(sOMC01Record.primary_key.merchant_id),	m_StrMerchantId, sizeof (sOMC01Record.primary_key.merchant_id));
			memcpy ((LPSTR)(sOMC01Record.primary_key.card_nbr),		m_StrCardNumber, sizeof (sOMC01Record.primary_key.card_nbr) );   
			memcpy ((LPSTR)(sOMC01Record.primary_key.max_repeat),	m_StrRepeat,	 sizeof (sOMC01Record.primary_key.max_repeat));
			memcpy ((LPSTR)(sOMC01Record.primary_key.repeated),		"0", sizeof (sOMC01Record.primary_key.repeated));
			memcpy ((LPSTR)(sOMC01Record.primary_key.expiry_date),	m_StrExpiryDate, sizeof (sOMC01Record.primary_key.expiry_date));
			memcpy ((LPSTR)(sOMC01Record.primary_key.peso_amt_limit),	m_StrPesoAmountLimit, sizeof (sOMC01Record.primary_key.peso_amt_limit));
			memcpy ((LPSTR)(sOMC01Record.primary_key.usd_amt_limit),	m_StrUsdAmountLimit, sizeof (sOMC01Record.primary_key.usd_amt_limit));

			if(m_ChkNeverExpires.GetCheck() == 1)
			{
				strBypass[0] = '1';
			}
			else
			{
				strBypass[0] = '0';
			}
			if(m_ChkBypassGlobal.GetCheck() == 1)
			{
				strBypass[1] = '1';
			}
			else
			{
				strBypass[1] = '0';
			}
			if(m_ChkBypassMCC.GetCheck() == 1)
			{
				strBypass[2] = '1';
			}
			else
			{
				strBypass[2] = '0';
			}
			if(m_ChkBypassMID.GetCheck() == 1)
			{
				strBypass[3] = '1';
			}
			else
			{
				strBypass[3] = '0';
			}

			memcpy ((LPSTR)(sOMC01Record.bypass), strBypass, sizeof (sOMC01Record.bypass));

			
			
			rcode = txdsapi_insert_record( (pBYTE)&sOMC01Record,sizeof (OMC01), OMC01_DATA, strErrorMessage );
			if(rcode != PTEMSG_OK)
			{
				m_StrMessageLbl = strErrorMessage;
				if (!UpdateData(FALSE)){
					AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
				}
				return ;
			}
			else
			{
				m_StrMessageLbl = "Record Added";
				m_strFieldTag = "ADD";
				PopulateHistoryTable();
				if (!UpdateData(FALSE)){
					AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
				}
				return ;
			}
		  
		}
		else if(rcode == PTEMSG_OK)
		{
			m_StrMessageLbl = "ERROR: Record already exists";
			if (!UpdateData(FALSE)){
				AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
			}
			return ;

		}
		else
		{
			m_StrMessageLbl = strErrorMessage;
			if (!UpdateData(FALSE)){
				AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
			}
			return ;
		}

	}
	if (!UpdateData(FALSE)){
		AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
	}
	
	
}

int COverrideMerchantDenial::CheckMerchantId()
{
	MCF01 sMerchantRecord;
	int rcode;
	char strErrorMessage[200] = {0};

	ZeroMemory(&sMerchantRecord,sizeof(MCF01)) ;

	if (!UpdateData(TRUE)){
		AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
	}
    memcpy ((LPSTR)(sMerchantRecord.primary_key.merchant_id), m_StrMerchantId, sizeof (sMerchantRecord.primary_key.merchant_id));

	rcode = txdsapi_get_record( (pBYTE)&sMerchantRecord,sizeof (MCF01), MCF01_ID_DATA, strErrorMessage );
	if( rcode == PTEMSG_NOT_FOUND )
    {
      m_StrMessageLbl = "ERROR: Invalid Merchant ID";
	  if (!UpdateData(FALSE)){
			AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
		}
		return -1;
	}
	return 0;
}

int COverrideMerchantDenial::CheckCardNumber()
{
	char ErrorMsg[500];
	CCF02P	sCCF02Record;

	if(m_StrCardNumber.GetLength() > 0)	//Card number optional
	{
		ZeroMemory(&sCCF02Record,sizeof(CCF02P)) ;
		memcpy ((LPSTR)(sCCF02Record.primary_key.card_type), "C", sizeof (sCCF02Record.primary_key.card_type));
		
		memcpy ((LPSTR)(sCCF02Record.primary_key.card_nbr), m_StrCardNumber , sizeof (sCCF02Record.primary_key.card_nbr));
		
		int rcode = txdsapi_get_record((pBYTE)&sCCF02Record, sizeof(CCF02P),CCF02P_DATA, ErrorMsg);
		if( rcode == PTEMSG_NOT_FOUND )
		{
			m_StrMessageLbl = "ERROR: Card Record does not exist";
			if (!UpdateData(FALSE)){
				AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
			}
			return -1 ;
		}
	}
	return 0;
	
}

void COverrideMerchantDenial::OnListMerchant() 
{
	CBinList listDlg;
    int nReturnCode;

	UpdateData(TRUE);
	m_StrMessageLbl = "";

	// set dialog Mode for Bin List
	listDlg.strItem0 = m_StrMerchantId;
	//listDlg.strItem1 = m_StrOrganizationIDFld;
	listDlg.m_nDialogMode = MODE_MERCHANT;

    nReturnCode = listDlg.DoModal();
	if ( nReturnCode == IDOK)
	{
        m_StrMerchantId = strMerchantID;
	}
        
    if (!UpdateData(FALSE)){
		AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
	}	
	
}


BOOL COverrideMerchantDenial::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
	CFont m_font;
	LOGFONT lf;
	CString temp;
	int nIndex = -1;
	m_bGeneral = TRUE;
	m_StrBypass = "";
	// set font for the Message lable field

	memset ( &lf, 0, sizeof (lf));
	lf.lfHeight = 12;
	lf.lfWeight = 700;
	strcpy(lf.lfFaceName, "Arial");
	m_font.CreateFontIndirect (&lf);
	GetDlgItem(IDC_MESSAGE_LBL)->SetFont (&m_font);

	// set access rights according UCF01 table
	// set access rights according UCF01 table
	for ( int i = 0; i <=15; i++ )
	{
		temp = ucf01.control[i].subsystem_name;
		if ( strcmp ( temp, "Override Merchant Denial" ) == 0)
			nIndex = i;
	}

	if ( nIndex != -1)
	{
		if ( ucf01.control[nIndex].action[3] == '1' ) //View
		{
			m_CntlListOverrides.EnableWindow(TRUE);
		}
		if ( ucf01.control[nIndex].action[2] == '1' ) //Update
		{
			EnableOverrideDialog(TRUE);
			m_CntlUpdate.EnableWindow(TRUE);
		}
		if ( ucf01.control[nIndex].action[1] == '1' ) //Delete
		{
			EnableOverrideDialog(TRUE);
			m_CntlAdd.EnableWindow(FALSE);
		}
		
		if ( ucf01.control[nIndex].action[0] == '1' ) //Add
		{
			EnableOverrideDialog(TRUE);
			if ( ucf01.control[nIndex].action[1] == '0' ) //Delete
			{
				m_CntlDelete.EnableWindow(FALSE);
			}
		}
		
	}

	m_StrCardNumber = _T("");
	m_StrMerchantId = _T("");
	m_StrMessageLbl = _T("");
	m_StrExpiryDate = _T("");
	m_StrRepeat		= _T("0");
	m_StrPesoAmountLimit= _T("");
	m_StrUsdAmountLimit= _T("");
	m_CntlExpiryDate.SetFormat(_T("dd/MM/yyyy"));

	memset (&sHistoryRecord, 0, sizeof(HST01));
	memcpy ((LPSTR)(sHistoryRecord.primary_key.unique_id), "                 ", sizeof (sHistoryRecord.primary_key.unique_id));
    memcpy ((LPSTR)(sHistoryRecord.file_name), "OMC01", sizeof (sHistoryRecord.file_name));


	UpdateData(FALSE);	


	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void COverrideMerchantDenial::OnListoverride() 
{
	CBinList listDlg;
    int nReturnCode;
	int rcode;
	char strErrorMessage[200] = {0};
	char strBypass[9] = {0};

	UpdateData(TRUE);
	m_StrMessageLbl = "";
	UpdateData(FALSE);
	listDlg.m_nDialogMode = MODE_OMC01;
	listDlg.strItem0 = m_StrMerchantId;
	listDlg.strItem1 = m_StrCardNumber;

    nReturnCode = listDlg.DoModal();
    if ( nReturnCode == IDOK)
	{
		OMC01 sOMC01Record;
		ZeroMemory(&sOMC01Record,sizeof(OMC01)) ;
		memcpy ((LPSTR)(sOMC01Record.primary_key.merchant_id), listDlg.strItem1, sizeof (sOMC01Record.primary_key.merchant_id));
		memcpy ((LPSTR)(sOMC01Record.primary_key.card_nbr), listDlg.strItem2 , sizeof (sOMC01Record.primary_key.card_nbr));
		rcode = txdsapi_get_record( (pBYTE)&sOMC01Record,sizeof (OMC01), OMC01_DATA, strErrorMessage );
		if( rcode == PTEMSG_OK)
		{
			m_StrMerchantId		= sOMC01Record.primary_key.merchant_id;
			m_StrCardNumber		= sOMC01Record.primary_key.card_nbr;
			m_StrRepeat			= sOMC01Record.primary_key.max_repeat;
			m_StrExpiryDate		= sOMC01Record.primary_key.expiry_date;
			m_StrOverrideUsed	= sOMC01Record.primary_key.repeated;
			m_StrPesoAmountLimit= sOMC01Record.primary_key.peso_amt_limit;
			m_StrUsdAmountLimit = sOMC01Record.primary_key.usd_amt_limit;

			m_strCurrentData = m_StrMerchantId + m_StrCardNumber;

			COverrideMerchantDenialPropertySheet * pParentSheet = ((COverrideMerchantDenialPropertySheet *) GetParent());

			if(strlen((LPSTR)((char *) pParentSheet->m_ChangeHistoryPage.sHistoryRecord.primary_key.unique_id)) == 0)
			{
				memcpy ( &(pParentSheet->m_ChangeHistoryPage.sHistoryRecord), &sHistoryRecord, sizeof(HST01));
			}
			strcpy((char *) pParentSheet->m_ChangeHistoryPage.sHistoryRecord.file_primary_key, m_StrMerchantId);

			SetExpiryDate(m_StrExpiryDate);

			memset(strBypass, 0, sizeof(strBypass));
			memcpy(strBypass, sOMC01Record.bypass, sizeof(sOMC01Record.bypass));

			if(strBypass[0] == '1')
			{
				m_ChkNeverExpires.SetCheck(1);
				m_CntlExpiryDate.EnableWindow(FALSE);
			}
			else
			{
				m_ChkNeverExpires.SetCheck(0);
				m_CntlExpiryDate.EnableWindow(TRUE);
			}
			if(strBypass[1] == '1')
			{
				m_ChkBypassGlobal.SetCheck(1);
			}
			else
			{
				m_ChkBypassGlobal.SetCheck(0);
			}
			if(strBypass[2] == '1')
			{
				m_ChkBypassMCC.SetCheck(1);
			}
			else
			{
				m_ChkBypassMCC.SetCheck(0);
			}
			if(strBypass[3] == '1')
			{
				m_ChkBypassMID.SetCheck(1);
			}
			else
			{
				m_ChkBypassMID.SetCheck(0);
			}

		}
	}

    if (!UpdateData(FALSE))
	{
		AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
	}	
	
}

void COverrideMerchantDenial::OnDelete() 
{
	char strErrorMessage[200] = {0};
	char strBypass[9] = {0};
	UpdateData(TRUE);
	CleanGeneralDialog();
	GetExpiryDate(m_StrExpiryDate);
	
	//if(m_StrMerchantId.IsEmpty() || m_StrRepeat.IsEmpty() || m_StrCardNumber.IsEmpty() || m_StrPesoAmountLimit.IsEmpty() || m_StrUsdAmountLimit.IsEmpty())
	if(m_StrMerchantId.IsEmpty() || m_StrRepeat.IsEmpty())
	{
		m_StrMessageLbl = "Please select a Merchant Override Record";
		if (!UpdateData(FALSE)){
			AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
		}
		return;
	}


		
	int rcode = AfxMessageBox("Do you want to delete this record?",MB_OKCANCEL|MB_ICONQUESTION);
	if(rcode == IDOK)
	{
		OMC01 sOMC01Record;
		ZeroMemory(&sOMC01Record,sizeof(OMC01)) ;
		memcpy ((LPSTR)(sOMC01Record.primary_key.merchant_id),	m_StrMerchantId, sizeof (sOMC01Record.primary_key.merchant_id));
		memcpy ((LPSTR)(sOMC01Record.primary_key.card_nbr),		m_StrCardNumber, sizeof (sOMC01Record.primary_key.card_nbr) );   
		memcpy ((LPSTR)(sOMC01Record.primary_key.max_repeat),	m_StrRepeat,	 sizeof (sOMC01Record.primary_key.max_repeat));
		//memcpy ((LPSTR)(sOMC01Record.primary_key.repeated),	m_StrCardNumber, sizeof (sOMC01Record.primary_key.repeated));
		memcpy ((LPSTR)(sOMC01Record.primary_key.expiry_date),	m_StrExpiryDate, sizeof (sOMC01Record.primary_key.expiry_date));
		memcpy((LPSTR)(sOMC01Record.primary_key.peso_amt_limit),	m_StrPesoAmountLimit, sizeof (sOMC01Record.primary_key.peso_amt_limit));
		memcpy((LPSTR)(sOMC01Record.primary_key.usd_amt_limit),	m_StrUsdAmountLimit, sizeof (sOMC01Record.primary_key.usd_amt_limit));

		m_strPreviousData = m_StrMerchantId + m_StrCardNumber;

		if(m_ChkNeverExpires.GetCheck() == 1)
		{
			strBypass[0] = '1';
		}
		else
		{
			strBypass[0] = '0';
		}
		if(m_ChkBypassGlobal.GetCheck() == 1)
		{
			strBypass[1] = '1';
		}
		else
		{
			strBypass[1] = '0';
		}
		if(m_ChkBypassMCC.GetCheck() == 1)
		{
			strBypass[2] = '1';
		}
		else
		{
			strBypass[2] = '0';
		}
		if(m_ChkBypassMID.GetCheck() == 1)
		{
			strBypass[3] = '1';
		}
		else
		{
			strBypass[3] = '0';
		}
		
		memcpy ((LPSTR)(sOMC01Record.bypass), strBypass, sizeof (sOMC01Record.bypass));
		
		rcode = txdsapi_delete_record( (pBYTE)&sOMC01Record,sizeof (OMC01), OMC01_DATA, strErrorMessage );
	}
	// failed to delete
	if ( PTEMSG_OK != rcode   )
    {
		 switch( rcode )
      {
         case PTEMSG_NOT_FOUND:
			sprintf( strErrorMessage, "ERROR: Merchant Override record for %s does not exist", m_StrMerchantId );
            break;
      }

        m_StrMessageLbl = strErrorMessage;
		if (!UpdateData(FALSE)){
			AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
		}
	}
    else
    {
		m_StrMessageLbl = "Record Deleted";
		m_strFieldTag = "DELETE";
		PopulateHistoryTable();
		if (!UpdateData(FALSE)){
			AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
		}
	}
}


void COverrideMerchantDenial::PopulateHistoryTable()
{
	CMyUtils myUtilsObject;
    HST01 sHistoryData;
    CString temp;

    CHAR dest_str[18]; 	
	char strErrorMsg [200] = "";
    char strDate[9] = "";
    char strTime[7] = "";
	char s_counter_ctr[4]   = "";
    
	//clean strings that hold date & time 
    memset ( strDate, 0, sizeof(strDate));
    memset ( strTime, 0, sizeof(strTime));

	//this function gets current date & time
    myUtilsObject.GetSystemDate(strDate);
    myUtilsObject.GetSystemTime(strTime);
    
	//this function writes generated timestamp in string
    pintime_get_gmt ( dest_str );
	counter++;
	if ( counter > 999 )
		  counter = 0;
    sprintf( s_counter_ctr, "%03d", counter );
    memcpy( &dest_str[14], s_counter_ctr, 3 );

	//clean HST01
    ZeroMemory ( &sHistoryData, sizeof(HST01));
    
	//copy file name to HST01
    memcpy ((LPTSTR) sHistoryData.file_name,
		      (LPCTSTR)"OMC01",
			            sizeof(sHistoryData.file_name));
	//copy date to HST01
    temp = strDate;
    if ( temp.GetLength() != 0 )
    temp = temp.Right(4) + temp.Left(2) + temp.Mid(2,2);
    
    CHAR wstation[21]; DWORD wsize;

    wsize = sizeof( wstation );
    GetComputerName( wstation, &wsize );

	memcpy ((LPTSTR)sHistoryData.reserved,(LPTSTR)wstation, sizeof(sHistoryData.reserved));
    memcpy ((LPTSTR) sHistoryData.date_modified, temp, sizeof (sHistoryData.date_modified));
    //copy card type to HST01
    memcpy ((LPTSTR) sHistoryData.file_primary_key,
		      (LPCTSTR)m_StrMerchantId,
			            sizeof(sHistoryData.file_primary_key));
    //copy timestamp to HST01
    memcpy ((LPTSTR) sHistoryData.primary_key.unique_id,
		      (LPCTSTR)dest_str,
			            sizeof(sHistoryData.primary_key.unique_id));
	//copy time to HST01
    memcpy ((LPTSTR) sHistoryData.time_modified,
		      (LPCTSTR)strTime,
			            sizeof(sHistoryData.time_modified)); 
    //copy current value to HST01         
    memcpy ((LPTSTR)sHistoryData.current_data,
		      (LPCTSTR)m_strCurrentData,
			            sizeof(sHistoryData.current_data));
    //copy previous value to HST01
	memcpy ((LPTSTR)sHistoryData.previous_data,
		      (LPCTSTR)m_strPreviousData,
			            sizeof(sHistoryData.previous_data));
    //copy user name to HST01
	memcpy ((LPTSTR)sHistoryData.changed_by,
		      (LPCTSTR)ucf01.primary_key.name,
			            sizeof(sHistoryData.changed_by));
    //copy field tag to HST01
	memcpy ((LPTSTR)sHistoryData.field_tag,
		      (LPCTSTR)m_strFieldTag,
			            sizeof(sHistoryData.field_tag));
    //insert record into HST01    
    int rcode = txdsapi_insert_record((pBYTE)&sHistoryData,
		                sizeof(HST01), HST01_DATA, strErrorMsg);
	if ( rcode != PTEMSG_OK )
	{   
		//give an error message if insertion failed
		m_StrMessageLbl = strErrorMsg;
		UpdateData( FALSE );
	}    
}

void COverrideMerchantDenial::EnableOverrideDialog(int status)
{
	m_CntlAdd.EnableWindow(status);
	m_CntlCardNumber.EnableWindow(status);
	m_CntlDelete.EnableWindow(status);
	m_CntlMerchantId.EnableWindow(status);
	m_CntlListMerchants.EnableWindow(status);
	m_CntlExpiryDate.EnableWindow(status);
	m_CntlRepeat.EnableWindow(status);
	m_CntlPesoAmountLimit.EnableWindow(status);
	m_CntlUsdAmountLimit.EnableWindow(status);
	m_CntlUpdate.EnableWindow(status);
	
}

int COverrideMerchantDenial::GetExpiryDate(CString &strDate)
{
	UpdateData(TRUE);
	CTime exTime;
	if(GDT_VALID == m_CntlExpiryDate.GetTime(exTime))
	{
//praneeth code		m_StrExpiryDate = exTime.Format("%d%m%Y");
		m_StrExpiryDate = exTime.Format("%Y%m%d");//praneeth code

	}
	else
	{
		m_StrMessageLbl = "Invalid time format";
		AfxMessageBox(m_StrMessageLbl);
		if (!UpdateData(FALSE)){
			AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
		}
		return -1;
	}
	return 0;
}

int COverrideMerchantDenial::SetExpiryDate(CString strDate)
{
	SYSTEMTIME sysTime;
	char date[9] = {0};
	memset(&sysTime, 0, sizeof(sysTime));

	if(!strDate.IsEmpty())
	{
		strcpy(date,strDate.GetBuffer(9));
	
		sysTime.wYear	= atoi(strDate.Left(4));
		sysTime.wMonth	= atoi(strDate.Mid(4,2));
		sysTime.wDay	= atoi(strDate.Right(2));
		m_CntlExpiryDate.SetTime(&sysTime);
	}
	return 0;
}

void COverrideMerchantDenial::OnUpdate() 
{
	int rcode;
	char strErrorMessage[200] = {0};
	UpdateData(TRUE);
	m_StrMessageLbl = "";
	char strBypass[9] = {0};

	GetExpiryDate(m_StrExpiryDate);
	if((0 == CheckMerchantId()) && !m_StrPesoAmountLimit.IsEmpty() && !m_StrUsdAmountLimit.IsEmpty())
	{
		OMC01 sOMC01Record;
		ZeroMemory(&sOMC01Record,sizeof(OMC01)) ;
		memcpy ((LPSTR)(sOMC01Record.primary_key.merchant_id),	m_StrMerchantId, sizeof (sOMC01Record.primary_key.merchant_id));
		memcpy ((LPSTR)(sOMC01Record.primary_key.card_nbr),		m_StrCardNumber, sizeof (sOMC01Record.primary_key.card_nbr) );   
		
		ZeroMemory(&sOverrideMerchantDenialold,sizeof(OMC01)) ;
		memcpy ((LPSTR)(sOverrideMerchantDenialold.primary_key.merchant_id),	m_StrMerchantId, sizeof (sOverrideMerchantDenialold.primary_key.merchant_id));
		memcpy ((LPSTR)(sOverrideMerchantDenialold.primary_key.card_nbr),		m_StrCardNumber, sizeof (sOverrideMerchantDenialold.primary_key.card_nbr) );   
		rcode = txdsapi_get_record( (pBYTE)&sOverrideMerchantDenialold,sizeof (OMC01), OMC01_DATA, strErrorMessage );
		if(m_ChkNeverExpires.GetCheck() == 1)
		{
			m_StrExpiryDate.Empty();
		}
		if( rcode == PTEMSG_OK )
		{
			m_strPreviousData = m_StrMerchantId + m_StrCardNumber;
			memcpy ((LPSTR)(sOMC01Record.primary_key.merchant_id),	m_StrMerchantId, sizeof (sOMC01Record.primary_key.merchant_id));
			memcpy ((LPSTR)(sOMC01Record.primary_key.card_nbr),		m_StrCardNumber, sizeof (sOMC01Record.primary_key.card_nbr) );   
			memcpy ((LPSTR)(sOMC01Record.primary_key.max_repeat),	m_StrRepeat,	 sizeof (sOMC01Record.primary_key.max_repeat));
			memcpy ((LPSTR)(sOMC01Record.primary_key.repeated),		"0", sizeof (sOMC01Record.primary_key.repeated));
			memcpy ((LPSTR)(sOMC01Record.primary_key.expiry_date),	m_StrExpiryDate, sizeof (sOMC01Record.primary_key.expiry_date));
			memcpy ((LPSTR)(sOMC01Record.primary_key.peso_amt_limit),	m_StrPesoAmountLimit, sizeof (sOMC01Record.primary_key.peso_amt_limit));
			memcpy ((LPSTR)(sOMC01Record.primary_key.usd_amt_limit),	m_StrUsdAmountLimit, sizeof (sOMC01Record.primary_key.usd_amt_limit));
			if(m_ChkNeverExpires.GetCheck() == 1)
			{
				strBypass[0] = '1';
			}
			else
			{
				strBypass[0] = '0';
			}
			if(m_ChkBypassGlobal.GetCheck() == 1)
			{
				strBypass[1] = '1';
			}
			else
			{
				strBypass[1] = '0';
			}
			if(m_ChkBypassMCC.GetCheck() == 1)
			{
				strBypass[2] = '1';
			}
			else
			{
				strBypass[2] = '0';
			}
			if(m_ChkBypassMID.GetCheck() == 1)
			{
				strBypass[3] = '1';
			}
			else
			{
				strBypass[3] = '0';
			}
			
			memcpy ((LPSTR)(sOMC01Record.bypass), strBypass, sizeof (sOMC01Record.bypass));
			m_StrBypass=strBypass;
			rcode = txdsapi_update_record( (pBYTE)&sOMC01Record,sizeof (OMC01), OMC01_DATA, strErrorMessage );
			if(rcode != PTEMSG_OK)
			{
				m_StrMessageLbl = strErrorMessage;
				if (!UpdateData(FALSE)){
					AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
				}
				return ;
			}
			else
			{
				m_StrMessageLbl = "Record Updated";
				m_strFieldTag = "UPDATE";
				CompareStructures();
				if (!UpdateData(FALSE)){
					AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
				}
				return ;
			}
		  
		}
		else if(rcode == PTEMSG_OK)
		{
			m_StrMessageLbl = "ERROR: Record already exists";
			if (!UpdateData(FALSE)){
				AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
			}
			return ;

		}
		else
		{
			m_StrMessageLbl = strErrorMessage;
			if (!UpdateData(FALSE)){
				AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
			}
			return ;
		}

	}
	if (!UpdateData(FALSE)){
		AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
	}
		
}

void COverrideMerchantDenial::OnNeverExpires() 
{

	if(m_ChkNeverExpires.GetCheck() == 1)
	{
		m_CntlExpiryDate.EnableWindow(FALSE);
	}
	else
	{
		m_CntlExpiryDate.EnableWindow(TRUE);
	}
	
}

BOOL COverrideMerchantDenial::OnSetActive() 
{
	// TODO: Add your specialized code here and/or call the base class
	
	return CPropertyPage::OnSetActive();
}

BOOL COverrideMerchantDenial::CompareStructures()
{
	CString Deferred_start_date;
    CString Deferred_end_date;
    CMyUtils myUtilsObject;

	if ( m_bGeneral == TRUE )
	{
		
		if (strcmp ( m_StrRepeat, (char *)sOverrideMerchantDenialold.primary_key.max_repeat) != 0)
		{
			 m_strCurrentData.Empty();
			 m_strPreviousData.Empty();
			 m_strFieldTag.Empty();

			 m_strCurrentData = m_StrRepeat;
			 m_strPreviousData = sOverrideMerchantDenialold.primary_key.max_repeat;
			 m_strFieldTag ="NO OF TIMES:";
			
			 PopulateHistoryTable();
		}

		if (strcmp ( m_StrExpiryDate, (char *)sOverrideMerchantDenialold.primary_key.expiry_date) != 0)
		{
			 m_strCurrentData.Empty();
			 m_strPreviousData.Empty();
			 m_strFieldTag.Empty();

			 m_strCurrentData = m_StrExpiryDate;
			 m_strPreviousData = sOverrideMerchantDenialold.primary_key.expiry_date;
			 m_strFieldTag ="EXP DATE";
			
			 PopulateHistoryTable();
		}
		if (strcmp ( m_StrPesoAmountLimit, (char *)sOverrideMerchantDenialold.primary_key.peso_amt_limit) != 0)
		{
			 m_strCurrentData.Empty();
			 m_strPreviousData.Empty();
			 m_strFieldTag.Empty();

			 m_strCurrentData = m_StrPesoAmountLimit;
			 m_strPreviousData = sOverrideMerchantDenialold.primary_key.peso_amt_limit;
			 m_strFieldTag ="PESO AMT LMT";
			 
			 PopulateHistoryTable();
		}
		
		if (strcmp ( m_StrUsdAmountLimit, (char *)sOverrideMerchantDenialold.primary_key.usd_amt_limit) != 0)
		{
			 m_strCurrentData.Empty();
			 m_strPreviousData.Empty();
			 m_strFieldTag.Empty();

			 m_strCurrentData = m_StrUsdAmountLimit;
			 m_strPreviousData = sOverrideMerchantDenialold.primary_key.usd_amt_limit;
			 m_strFieldTag ="USD AMT LMT";
			 
			 PopulateHistoryTable();
		}

		// No Expiry
		if ( m_StrBypass[0] != sOverrideMerchantDenialold.bypass[0])
		{
			 m_strCurrentData.Empty();
			 m_strPreviousData.Empty();
			 m_strFieldTag.Empty();

			 m_strCurrentData = m_StrBypass[0];
			 //m_strPreviousData = sOverrideMerchantDenialold.bypass[0];
			 m_strPreviousData =(CString)((char *) sOverrideMerchantDenialold.bypass[0]);
			 m_strFieldTag ="NEVER EXP";
			 
			 PopulateHistoryTable();
		}

		// By pass GLOBAL check
		if (m_StrBypass[1]!= sOverrideMerchantDenialold.bypass[1])
		{
			 m_strCurrentData.Empty();
			 m_strPreviousData.Empty();
			 m_strFieldTag.Empty();

			 m_strCurrentData = m_StrBypass[1];
			 //m_strPreviousData = sOverrideMerchantDenialold.bypass[1];
			 m_strPreviousData = (CString)((char *)sOverrideMerchantDenialold.bypass[1]);
			 m_strFieldTag ="GLOBAL CHCK";
			 
			 PopulateHistoryTable();
		}

		//MCC checking
		if ( m_StrBypass[2] != sOverrideMerchantDenialold.bypass[2])
		{
			 m_strCurrentData.Empty();
			 m_strPreviousData.Empty();
			 m_strFieldTag.Empty();

			 m_strCurrentData = m_StrBypass[2];
			 //m_strPreviousData = sOverrideMerchantDenialold.bypass[2];
			 m_strPreviousData =  (CString)((char *)sOverrideMerchantDenialold.bypass[2]);
			 m_strFieldTag ="MCC CHCK";;
			 
			 PopulateHistoryTable();
		}

		// MID capping
		if ( m_StrBypass[3]!=sOverrideMerchantDenialold.bypass[3]) 
		{
			 m_strCurrentData.Empty();
			 m_strPreviousData.Empty();
			 m_strFieldTag.Empty();

			 m_strCurrentData = m_StrBypass[3];
			 //m_strPreviousData = sOverrideMerchantDenialold.bypass[3];
			  m_strPreviousData = (CString)((char *)sOverrideMerchantDenialold.bypass[3]);
			 m_strFieldTag ="MID CAP";
			 
			 PopulateHistoryTable();
		}


	}

	return TRUE;
}

BOOL COverrideMerchantDenial::CleanGeneralDialog()
{
	m_StrMessageLbl= _T("");;
	m_strCurrentData= _T("");;
	m_strPreviousData= _T("");;
	m_strFieldTag= _T("");;
	m_StrBypass= _T("");;
	return TRUE;
}

