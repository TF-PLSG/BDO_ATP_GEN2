// CntryAmtLimits.cpp : implementation file
//

#include "stdafx.h"
#include "atp.h"
#include "CntryAmtLimits.h"
#include "MyUtils.h"

extern UCF01 ucf01;

extern "C"
{
#include "PteTime.h"
}

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#include "SharedMem.h"

extern CSharedMem shrmem;
/////////////////////////////////////////////////////////////////////////////
// CCntryAmtLimits property page

IMPLEMENT_DYNCREATE(CCntryAmtLimits, CPropertyPage)

CCntryAmtLimits::CCntryAmtLimits() : CPropertyPage(CCntryAmtLimits::IDD)
{
	//{{AFX_DATA_INIT(CCntryAmtLimits)
	m_StrCntryCode = _T("");
	m_StrDollarLimit = _T("");
	m_StrMessageLbl = _T("");
	m_StrProfileIdFld = _T("");
	m_StrTypeFld = _T("");
	//}}AFX_DATA_INIT
}

CCntryAmtLimits::~CCntryAmtLimits()
{
}

void CCntryAmtLimits::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCntryAmtLimits)
	DDX_Control(pDX, IDC_CNTRY_LMT_UPDATE, m_CntlUpdateButton);
	DDX_Control(pDX, IDC_CNTRY_LMT_DELETE, m_CntlDeleteButton);
	DDX_Control(pDX, IDC_CNTRY_LMT_ADD, m_CntlAddButton);
	DDX_Control(pDX, IDC_TYPE_FLD, m_CntlTypeFld);
	DDX_Control(pDX, IDC_PROFILE_ID_FLD, m_CntlProfileIdFld);
	DDX_Control(pDX, IDC_MESSAGE_LBL, m_CntlMessageLbl);
	DDX_Control(pDX, IDC_DOLLAR_LIMIT, m_CntlDollarLimit);
	DDX_Control(pDX, IDC_CNTRY_CODE, m_CntlCntryCode);
	DDX_Control(pDX, IDC_CNTRY_LIST_CTRL, m_CntlCntryList);
	DDX_Text(pDX, IDC_CNTRY_CODE, m_StrCntryCode);
	DDV_MaxChars(pDX, m_StrCntryCode, 3);
	DDX_Text(pDX, IDC_DOLLAR_LIMIT, m_StrDollarLimit);
	DDV_MaxChars(pDX, m_StrDollarLimit, 13);
	DDX_Text(pDX, IDC_MESSAGE_LBL, m_StrMessageLbl);
	DDX_Text(pDX, IDC_PROFILE_ID_FLD, m_StrProfileIdFld);
	DDX_Text(pDX, IDC_TYPE_FLD, m_StrTypeFld);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CCntryAmtLimits, CPropertyPage)
	//{{AFX_MSG_MAP(CCntryAmtLimits)
	ON_BN_CLICKED(IDC_CNTRY_LMT_ADD, OnCntryLmtAdd)
	ON_BN_CLICKED(IDC_CNTRY_LMT_DELETE, OnCntryLmtDelete)
	ON_BN_CLICKED(IDC_CNTRY_LMT_UPDATE, OnCntryLmtUpdate)
	ON_NOTIFY(NM_CLICK, IDC_CNTRY_LIST_CTRL, OnClickCntryListCtrl)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCntryAmtLimits message handlers

BOOL CCntryAmtLimits::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();

	LV_COLUMN lvColumn;
	CFont m_font;
	LOGFONT lf;
//	TC_ITEM ItemDef;		// TC_ITEM specifies or retrieves items of a tab

//	INT		nIndex;


	memset ( &lf, 0, sizeof (lf));
	lf.lfHeight = 12;
	lf.lfWeight = 700;
	strcpy(lf.lfFaceName, "Arial");
	m_font.CreateFontIndirect (&lf);
	GetDlgItem(IDC_MESSAGE_LBL)->SetFont (&m_font);
    m_StrMessageLbl = "";

	m_bCntryAmtLmt = TRUE;

	char strHeaderData[2][25] = {"  COUNTRY CODE   ", "   AMOUNT LIMIT  "};
	
	for (int nHeader = 0; nHeader < 2; nHeader++)
	{
		lvColumn.mask = LVCF_FMT | LVCF_SUBITEM | LVCF_TEXT ;
		lvColumn.fmt  = LVCFMT_RIGHT;
		lvColumn.pszText = strHeaderData[nHeader];
		lvColumn.iSubItem = nHeader;

		m_CntlCntryList.InsertColumn(nHeader, &lvColumn); // assumes return value is ok
		m_CntlCntryList.SetColumnWidth(0, LVSCW_AUTOSIZE_USEHEADER);
		m_CntlCntryList.SetColumnWidth(1, LVSCW_AUTOSIZE_USEHEADER);

	}
	

	if(m_nDialogMode != MODE_DONE)
	{
		if(m_nDialogMode != MODE_VIEW)
			EnableCntryAmtLmtDialog(TRUE);
		else
			m_CntlCntryList.EnableWindow(FALSE);

		if(m_nDialogMode != MODE_ADD)
		{
            PopulateCntryLmtDialogFromDatabase (sProfileIdOld);
		}
	}
	else
		EnableCntryAmtLmtDialog(FALSE);//MODE_DONE

	if(!UpdateData(FALSE))
	{
		AfxMessageBox(IDS_UPDATEDATAFAILED, MB_ICONSTOP);
	}

	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CCntryAmtLimits::EnableCntryAmtLmtDialog ( int nStatus )
{
	m_CntlCntryCode.EnableWindow(nStatus);
	m_CntlDollarLimit.EnableWindow(nStatus);

	m_CntlAddButton.EnableWindow(nStatus);
	m_CntlUpdateButton.EnableWindow(nStatus);
	m_CntlDeleteButton.EnableWindow(nStatus);

	if(m_nDialogMode == MODE_ADD)
	{
		m_CntlUpdateButton.EnableWindow(FALSE);
		m_CntlDeleteButton.EnableWindow(FALSE);
	}

	m_CntlCntryList.EnableWindow(TRUE);	
}

void CCntryAmtLimits::CleanCntryLmtsDialog() 
{
   m_StrProfileIdFld  = _T("");
   m_StrTypeFld       = _T("");	
   m_StrCntryCode     = _T("");
   m_StrDollarLimit     = _T("");

   m_CntlCntryList.DeleteAllItems();
}


	void CCntryAmtLimits::PopulateHistoryTable()
{

    HST01 sHistoryData;
    CHAR dest_str[18];   
	CMyUtils myUtilsObject;
	char strErrorMsg[200] = "";
    CString temp;
    CHAR wstation[21]; 
	DWORD wsize;


    char strDate[9] = "";
    char strTime[7] = "";

	char strPrimaryKey[50];

    memset ( strDate, 0, sizeof (strDate));
    memset ( strTime, 0, sizeof (strTime));
	memset ( strPrimaryKey, 0, sizeof (strPrimaryKey));

	strcpy  ( strPrimaryKey, m_StrProfileIdFld);
	strcat ( strPrimaryKey, m_StrTypeFld);
    
    myUtilsObject.GetSystemDate(strDate);  
    myUtilsObject.GetSystemTime(strTime);

    pintime_get_gmt ( dest_str );

    ZeroMemory ( &sHistoryData, sizeof (HST01));
    memcpy ((LPTSTR) sHistoryData.file_name,     (LPCTSTR)"CAF01", sizeof (sHistoryData.file_name));
    
	temp = strDate;
	if (temp.GetLength() != 0)
		temp = temp.Right(4) + temp.Left(2) + temp.Mid(2,2);

    wsize = sizeof( wstation );
    GetComputerName( wstation, &wsize );

    
    memcpy ((LPTSTR) sHistoryData.date_modified, temp, sizeof (sHistoryData.date_modified));
    memcpy ((LPTSTR) sHistoryData.file_primary_key,  (LPCTSTR)strPrimaryKey, sizeof (sHistoryData.file_primary_key));
    memcpy ((LPTSTR) sHistoryData.primary_key.unique_id,     (LPCTSTR)dest_str, sizeof (sHistoryData.primary_key.unique_id));        
    memcpy ((LPTSTR) sHistoryData.time_modified,     (LPCTSTR)strTime, sizeof (sHistoryData.time_modified)); 
             
    memcpy ((LPTSTR)sHistoryData.current_data, (LPCTSTR)m_StrCurrentData, sizeof (sHistoryData.current_data));
	memcpy ((LPTSTR)sHistoryData.previous_data, (LPCTSTR)m_StrPreviousData, sizeof (sHistoryData.previous_data));
	memcpy ((LPTSTR)sHistoryData.field_tag, (LPCTSTR)m_StrFieldTag, sizeof (sHistoryData.field_tag));
	memcpy ((LPTSTR)sHistoryData.changed_by,(LPTSTR)ucf01.primary_key.name, sizeof(sHistoryData.changed_by));

    wsize = sizeof( wstation );
    GetComputerName( wstation, &wsize );

	memcpy ((LPTSTR)sHistoryData.reserved,(LPTSTR)wstation, sizeof(sHistoryData.reserved));

    int rcode = txdsapi_insert_record  ( (pBYTE)&sHistoryData, sizeof (HST01), HST01_DATA,strErrorMsg );
	if ( rcode != PTEMSG_OK )
	{
		m_StrMessageLbl = strErrorMsg;
		UpdateData( FALSE );
	}	

	
	return;
}

VOID CCntryAmtLimits::PopulateCntryLmtDialogFromDatabase( CAF01 sProfileId )
{
 
	LV_ITEM  lvItem;
    INT      nRowItem = 0;
    INT      nActualItem = 0;
	INT      num_returned = 0;
	CString  temp;
    CHAR     strSum[50];
    BYTE 	 rcode;
    CHAR	 error_msg[256];
	BOOL     bDone = FALSE;
	CMyUtils myUtilsObject;


	
	if ( m_nDialogMode != MODE_VIEW )
       EnableCntryAmtLmtDialog(TRUE);

    ZeroMemory ( &sProfileIdOld, sizeof (CAF01));

	m_StrMessageLbl = "";
	//empty list control & edit boxes
	m_CntlCntryList.DeleteAllItems();
	m_StrCntryCode.Empty();
	m_StrDollarLimit.Empty();
	m_CntlCntryList.EnableWindow(TRUE);

	memset( &m_cntry_amt_limits, 0, sizeof(m_cntry_amt_limits));
	memset( &m_cntry_amt_limits_gui_list, 0, sizeof(m_cntry_amt_limits_gui_list));

    m_StrProfileIdFld          = sProfileId.primary_key.caf_profile_id;
    m_StrTypeFld               = sProfileId.primary_key.type;

	if ( strcmp (m_StrTypeFld, "C") == 0 )
		m_StrTypeFld = "CREDIT";
	else if ( strcmp (m_StrTypeFld, "F") == 0 )
		m_StrTypeFld = "FLEET";
	else 
		m_StrTypeFld = "DEBIT";

	strcpy( (pCHAR)m_cntry_amt_limits.primary_key.caf_profile_id, m_StrProfileIdFld);
	strcpy( (pCHAR)m_cntry_amt_limits.primary_key.type, m_StrTypeFld.Left(1));

	/*Get Country List for profile id and profile type*/
	rcode = txdsapi_get_list( (pBYTE) &m_cntry_amt_limits, sizeof(COUNTRY_AMT_LIMITS),
		                      (pBYTE) &m_cntry_amt_limits_gui_list, sizeof(COUNTRY_AMT_LIMITS_GUI),
							  0, COUNTRY_AMT_LIMITS_DATA, error_msg );

	if(rcode != PTEMSG_OK)
	{
		m_StrMessageLbl = error_msg;
		UpdateData(FALSE);
	}

	num_returned = atoi( (pCHAR)m_cntry_amt_limits_gui_list.num_returned);

	if(num_returned == 0)
	{
		m_StrMessageLbl = "No Data Found";
		UpdateData(FALSE);
		return;
	}

	if (rcode == PTEMSG_OK)
	{
		if( num_returned == SELECT_LIST_SIZE)
		{
		    strcpy( (pCHAR)m_cntry_amt_limits.primary_key.caf_profile_id,(pCHAR)m_cntry_amt_limits_gui_list.country_amt_limits[num_returned-1].primary_key.caf_profile_id );
			strcpy( (pCHAR)m_cntry_amt_limits.primary_key.type,          (pCHAR)m_cntry_amt_limits_gui_list.country_amt_limits[num_returned-1].primary_key.type);
		}
	    else
		{
				bDone = TRUE;
		}

		UpdateData(FALSE);

	
		//insert items into list view control
		for ( int nItem = 0; nItem < num_returned; nItem++ )
		{
			m_CntlCntryList.EnableWindow(TRUE);

			for(int nSubItem = 0; nSubItem < 4; nSubItem++)
			{
				lvItem.mask = LVIF_TEXT;
				lvItem.iItem = (nSubItem == 0)? nRowItem : nActualItem;
				lvItem.iSubItem = nSubItem;
				temp.Empty();
				memset(strSum, 0, sizeof(strSum));

				if(nSubItem == 0)
				{	
					temp = (char *)m_cntry_amt_limits_gui_list.country_amt_limits[nItem].country_code;
					temp.TrimLeft() ;
					temp.TrimRight();
					sprintf(strSum, "%-3s", (LPCTSTR)temp);
					lvItem.pszText = strSum;
				}
				else
				if(nSubItem == 1)
				{
					temp = (char *)m_cntry_amt_limits_gui_list.country_amt_limits[nItem].amount_limit;
					memcpy(strSum, myUtilsObject.DisplayAmount(temp), sizeof (strSum));
					lvItem.pszText = strSum;
				}


				if(nSubItem == 0)
					nActualItem = m_CntlCntryList.InsertItem(&lvItem);
				else
					m_CntlCntryList.SetItem(&lvItem); 

				m_CntlCntryList.SetFocus();
			}//for insert sub items
		}//for insert row items
		
		m_CntlCntryList.SetFocus();
	}//for pte_msg_ok

	UpdateData(FALSE);

    memcpy ( &sCntryAmtLmtRecordOld, &sCntryAmtLmtRecord, sizeof ( COUNTRY_AMT_LIMITS));

    return;

}

void CCntryAmtLimits::OnCntryLmtAdd() 
{
	LV_ITEM  lvItem;
	CMyUtils myUtilsObject;
	char     error_msg[100] = {0};
	char     strTemp[100];
	CString  temp;

	INT		nSelectedIndex = -1;
	INT		nRowItem = 0;
	INT		nActualItem = 0;
	INT		nCount = 0;
	INT     rcode = 0;
	INT     decpos = 0;

	memset(strTemp, 0x00, sizeof(strTemp));
	UpdateData (TRUE);
    m_StrMessageLbl = "";
	UpdateData (FALSE);
	SetModified(TRUE);//enable apply button


	/*Check list is not full at 500*/
	nCount = m_CntlCntryList.GetItemCount();
	if (nCount >= MAX_CNT) /*Max codes that can be defined is 500)*/
	{
		sprintf (strTemp, "ERROR: Country Code count cannot exceed %d. Current count = %d", MAX_CNT,nCount);
		m_StrMessageLbl = strTemp;
		UpdateData(FALSE);
		return ;
	}

	/*Check for blank country code or amount field*/
	if(m_StrCntryCode.IsEmpty())
	{
		m_StrMessageLbl = "Country Code is required and cannot be empty.";
		m_CntlCntryCode.SetFocus();
		UpdateData(FALSE);
		return;
	}

   /* Dollar Amount Limit */
	if(m_StrDollarLimit.IsEmpty())
	{
		m_StrMessageLbl = "Dollar Amount limit is required and cannot be empty.";
		m_CntlDollarLimit.SetFocus();
		UpdateData(FALSE);
		return;
	}

	int length = m_StrDollarLimit.GetLength();
	int res = strspn ( m_StrDollarLimit, "1234567890." );
	if ( res < length )
	{
		m_CntlDollarLimit.SetFocus();
		m_StrMessageLbl = "Invalid Character.";
		UpdateData(FALSE);
		return;
	}
	
	decpos = m_StrDollarLimit.Find(".");
	
	if (decpos < 0)				//-1 returned if not found
	{
		m_CntlDollarLimit.SetFocus();
		m_StrMessageLbl = "Invalid Dollar Amount - Decimal required.";
		UpdateData(FALSE);
		return;
	}
	
	/*Check for 2 decimal places*/
	if(length < decpos + 1 + 2)		/*decpos is zero based index, so add 1 and 2 decimal places*/
	{
		m_CntlDollarLimit.SetFocus();
		m_StrMessageLbl = "Invalid DollarAmount - Two digits after decimal required.";
		UpdateData(FALSE);
		return;
	}


	/*verify Country code does not already exist*/
	if(nCount > 0) 
	{
		for ( int i = 0; i < nCount; i++ )
		{
		  temp = m_CntlCntryList.GetItemText (i, 0) ;
  		  temp.TrimLeft();
		  temp.TrimRight();

		  if ( strcmp ( temp, m_StrCntryCode ) == 0)
		  {
					nSelectedIndex = i;
					break;
		  }
		}

		if ( nSelectedIndex != -1 )
		{
			m_StrMessageLbl = "ERROR: Country Code for Profile already exists";		
	
			if (!UpdateData(FALSE))
			{
				AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
			}
			return ;
		}
	}

	//convert dollar amount for insertion into database and verify amount format entered
	CString dbUSAmount; //amount in database format
	dbUSAmount = myUtilsObject.GenerateTranAmount ( m_StrDollarLimit, 12, error_msg );	
	if ( dbUSAmount == "Error" )
	{
		m_StrMessageLbl = error_msg;
		UpdateData(FALSE);
		return;
	}


	UpdateData(TRUE);

	/*Add new country and amount to list control*/
	for (int nSubItem = 0; nSubItem < 4; nSubItem++)
	{
		lvItem.mask = LVIF_TEXT;
		lvItem.iItem = (nSubItem == 0)? nRowItem : nActualItem;
		lvItem.iSubItem = nSubItem;
		temp.Empty();


		if(nSubItem == 0)
		{	
			memcpy(strTemp, m_StrCntryCode, sizeof(strTemp));
			lvItem.pszText = strTemp;
		}
		else
		if(nSubItem == 1)
		{
			memset(strTemp, 0, sizeof(temp));
			memcpy (strTemp, m_StrDollarLimit, sizeof(strTemp));
			lvItem.pszText = strTemp;
		}

		if(nSubItem == 0)
			nActualItem = m_CntlCntryList.InsertItem(&lvItem);
		else
			m_CntlCntryList.SetItem(&lvItem); 

		nCount++; /*increment count for new record in list control*/
		m_CntlCntryList.SetFocus();

	}//for insert subfields in row

	//Insert into database
	strcpy( (pCHAR)m_cntry_amt_limits.primary_key.caf_profile_id, m_StrProfileIdFld);
	strcpy( (pCHAR)m_cntry_amt_limits.primary_key.type, m_StrTypeFld.Left(1));
	strcpy( (pCHAR)m_cntry_amt_limits.country_code, m_StrCntryCode);
	strcpy( (pCHAR)m_cntry_amt_limits.amount_limit,  dbUSAmount);

	rcode = txdsapi_insert_record( (pBYTE)&m_cntry_amt_limits, sizeof (COUNTRY_AMT_LIMITS),
									COUNTRY_AMT_LIMITS_DATA, error_msg );

	if (rcode != PTEMSG_OK )
	{
		 m_StrMessageLbl =  error_msg;
		 if (!UpdateData(FALSE))
			 AfxMessageBox (IDS_UPDATEDATAFAILED, MB_ICONSTOP);
		 return;
	}

	//log to history table
	memcpy ((LPSTR)m_StrCurrentData, m_StrCntryCode , sizeof (m_StrCurrentData));
	memset  ((LPSTR)m_StrPreviousData, 0, sizeof (m_StrPreviousData));
    sprintf((LPSTR)m_StrFieldTag, "Country Code %s Added", m_StrCntryCode);
	PopulateHistoryTable(); 


   memcpy ((LPSTR)m_StrCurrentData, m_StrDollarLimit , sizeof (m_StrCurrentData));
   memset  ((LPSTR)m_StrPreviousData, 0, sizeof (m_StrPreviousData));
   sprintf((LPSTR)m_StrFieldTag, "Country Code %s Dollar Limit Added", m_StrDollarLimit);
   PopulateHistoryTable(); 

	m_StrMessageLbl = "Record Added";
	m_CntlUpdateButton.EnableWindow(TRUE);
	m_CntlDeleteButton.EnableWindow(TRUE);

	nCount++; /*increment count for new record in list control*/
	m_CntlCntryList.SetFocus();

	UpdateData(FALSE);

}	


void CCntryAmtLimits::OnCntryLmtDelete() 
{
	CMyUtils myUtilsObject;

	char  error_msg[100] = {0};
	CString temp;
	CString strMessage;

	int		rcode= 0;
	int		nRowItem = 0;
	int		nActualItem = 0;
	int		nCount = 0;


    m_StrMessageLbl = "";
	SetModified(TRUE);//enable apply button
	m_CntlCntryList.SetFocus();

	UpdateData (FALSE);

	/*Verify delete?*/
	strMessage.Format ("Are you sure you want to delete Country Code %s ?", (LPCTSTR)m_StrCntryCode);
	rcode = AfxMessageBox ((LPCTSTR)strMessage, MB_OKCANCEL|MB_ICONQUESTION);
	if (rcode != IDOK)
		return ;

	/*check for blank mcc code*/
	if(m_StrCntryCode.IsEmpty())
	{
		m_StrMessageLbl = "Country Code is required and cannot be empty.";
		m_CntlCntryCode.SetFocus();
		UpdateData(FALSE);
		return;
	}



	/* Delete MCC Code from Database */
	strcpy( (pCHAR)m_cntry_amt_limits.primary_key.caf_profile_id, m_StrProfileIdFld);
	strcpy( (pCHAR)m_cntry_amt_limits.primary_key.type, m_StrTypeFld.Left(1));
	strcpy( (pCHAR)m_cntry_amt_limits.country_code, m_StrCntryCode);

	rcode = txdsapi_delete_record( (pBYTE)&m_cntry_amt_limits, sizeof (COUNTRY_AMT_LIMITS),
									COUNTRY_AMT_LIMITS_DATA, error_msg );

	if (rcode != PTEMSG_OK )
	{
		 m_StrMessageLbl =  error_msg;
		 if (!UpdateData(FALSE))
			 AfxMessageBox (IDS_UPDATEDATAFAILED, MB_ICONSTOP);
		 m_CntlCntryList.SetFocus();
		 return;
	}


	//log to history table
	memcpy ((LPSTR)m_StrCurrentData, "Record Deleted", sizeof (m_StrCurrentData));
	memcpy ((LPSTR)m_StrPreviousData, m_StrCntryCode, sizeof (m_StrPreviousData));
	sprintf ((LPSTR)m_StrFieldTag, "Country Code %s Deleted", m_StrCntryCode);
	PopulateHistoryTable();


	/*find row number for selected limit code*/
	nCount = m_CntlCntryList.GetItemCount();
	nSelectedIndex = -1;

	for ( int idx = 0; idx < nCount; idx++ )
	{
		temp = m_CntlCntryList.GetItemText (idx, 0) ;
		temp.TrimLeft();
		temp.TrimRight();
		if ( strcmp ( temp, m_StrCntryCode ) == 0)
		{
			nSelectedIndex = idx;
			break;
		}
	}	
	
	if (nSelectedIndex == -1)
	{
		m_StrMessageLbl = "ERROR: Invalid Country Code";
		UpdateData(FALSE);
		return;
	}

	m_CntlCntryList.DeleteItem(nSelectedIndex);
	m_StrMessageLbl = "Record Deleted";
	UpdateData(FALSE);	
}

void CCntryAmtLimits::OnCntryLmtUpdate() 
{
	int		rcode = 0;
	int     length = 0;
	int     res = 0;
	int     nCount = 0;
	char    error_msg[100] = {0};

	CString temp;
	CMyUtils myUtilsObject;


	UpdateData (TRUE);
    m_StrMessageLbl = "";
	UpdateData (FALSE);
	SetModified(TRUE);//enable apply button

	// check for blank input fields
	if(m_StrCntryCode.IsEmpty())
	{
		m_StrMessageLbl = "Country Code is required and cannot be empty.";
		m_CntlCntryCode.SetFocus();
		UpdateData(FALSE);
		return;
	}


	if(m_StrDollarLimit.IsEmpty())
	{
		m_CntlDollarLimit.SetFocus();
		m_StrMessageLbl = "Dollar Amount is required and cannot be empty.";
		UpdateData(FALSE);
		return;

	}

	
	if (m_StrDollarLimit.Find(".") < 0)		//-1 returned if not found
	{
		m_CntlDollarLimit.SetFocus();
		m_StrMessageLbl = "Invalid Dollar Amount - Decimal required.";
		UpdateData(FALSE);
		return;
	}

	/*Verify USD amount format*/
	length = m_StrDollarLimit.GetLength();
	res = strspn ( m_StrDollarLimit, "1234567890." );
	if ( res < length )
	{
		m_CntlDollarLimit.SetFocus();
		m_StrMessageLbl = "Invalid Character.";
		UpdateData(FALSE);
		return;
	}
	
	if (m_StrDollarLimit.Find(".") < 0)		//-1 returned if not found
	{
		m_CntlDollarLimit.SetFocus();
		m_StrMessageLbl = "Invalid Dollar Amount - Decimal required.";
		UpdateData(FALSE);
		return;
	}


	//convert USD amount for insertion into database and verify amount format entered
	CString dbUSAmount; //amount in database format
	dbUSAmount = myUtilsObject.GenerateTranAmount ( m_StrDollarLimit, 12, error_msg );	
	if ( dbUSAmount == "Error" )
	{
		m_StrMessageLbl = error_msg;
		UpdateData(FALSE);
		return;
	}


	//Insert into database
	strcpy( (pCHAR)m_cntry_amt_limits.primary_key.caf_profile_id, m_StrProfileIdFld);
	strcpy( (pCHAR)m_cntry_amt_limits.primary_key.type, m_StrTypeFld.Left(1));
	strcpy( (pCHAR)m_cntry_amt_limits.country_code, m_StrCntryCode);
	strcpy( (pCHAR)m_cntry_amt_limits.amount_limit,  dbUSAmount);

	rcode = txdsapi_update_record( (pBYTE)&m_cntry_amt_limits, sizeof (COUNTRY_AMT_LIMITS),
									COUNTRY_AMT_LIMITS_DATA, error_msg );

	if (rcode != PTEMSG_OK )
	{
		 m_StrMessageLbl =  error_msg;
		 if (!UpdateData(FALSE))
			 AfxMessageBox (IDS_UPDATEDATAFAILED, MB_ICONSTOP);
		 return;
	}


	nCount = m_CntlCntryList.GetItemCount();
	nSelectedIndex = -1;

	for ( int idx = 0; idx < nCount; idx++ )//find row number for selected limit code
	{
		temp = m_CntlCntryList.GetItemText (idx, 0) ;
		temp.TrimLeft();
		temp.TrimRight();
		if ( strcmp ( temp, m_StrCntryCode ) == 0)
		{
			nSelectedIndex = idx;
			break;
		}
	}	
	
	if (nSelectedIndex == -1)
	{
		m_StrMessageLbl = "ERROR: Invalid Country Code";
		UpdateData(FALSE);
		return;
	}


	//log to history table
/*	memcpy ((LPSTR)m_StrCurrentData, m_StrCntryCode , sizeof (m_StrCurrentData));
    memcpy ((LPSTR)m_StrPreviousData,m_CntlCntryList.GetItemText(nSelectedIndex, 1), sizeof (m_StrPreviousData));
    sprintf((LPSTR)m_StrFieldTag, "Country Code %s Updated", m_StrCntryCode);
	PopulateHistoryTable(); 
*/
    memset (m_StrCurrentData,  0, sizeof (m_StrCurrentData));
    memset (m_StrPreviousData, 0, sizeof (m_StrPreviousData));

   memcpy ((LPSTR)m_StrCurrentData, m_StrDollarLimit , sizeof (m_StrCurrentData));
   memcpy ((LPSTR)m_StrPreviousData,m_CntlCntryList.GetItemText(nSelectedIndex, 1), sizeof (m_StrPreviousData));
   sprintf((LPSTR)m_StrFieldTag, "Country Code %s Dollar Limit Updated", m_StrCntryCode);
   PopulateHistoryTable(); 

   //Set new List Box Text
   m_CntlCntryList.SetItemText ( nSelectedIndex, 0, m_StrCntryCode );
   m_CntlCntryList.SetItemText ( nSelectedIndex, 1, m_StrDollarLimit);
   m_StrMessageLbl = "Record Updated";
   UpdateData(FALSE);

}

void CCntryAmtLimits::OnClickCntryListCtrl(NMHDR* pNMHDR, LRESULT* pResult) 
{
	CString temp;

	if(m_nDialogMode != MODE_VIEW)
	{
		nSelectedIndex = m_CntlCntryList.GetNextItem (-1, LVNI_SELECTED) ;
		
		temp  = m_CntlCntryList.GetItemText(nSelectedIndex, 0);
  		temp.TrimLeft();
		temp.TrimRight();
		m_StrCntryCode = temp;

      m_StrDollarLimit  = m_CntlCntryList.GetItemText(nSelectedIndex, 1); 
      m_CntlCntryCode.SetFocus();

      UpdateData(FALSE);

	}
	  
	*pResult = 0;
}

BOOL CCntryAmtLimits::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	// TODO: Add your specialized code here and/or call the base class
	
	c_time = CTime::GetCurrentTime();	
	shrmem.SetLastUpdateTime(c_time);
	return CPropertyPage::OnCommand(wParam, lParam);
}
