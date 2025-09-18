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
* $Log:   N:\POS\PVCS6.6\EPICPORTZ\PTE\Equitable\ATP_gui\PurchaseCardGeneral.cpp  $
   
      Rev 1.13   Apr 08 2005 08:47:46   lmitchel
   Add delete function to Purchase Card MCC ranges and correcting correct errors from testing
   
      Rev 1.12   Apr 01 2005 10:14:08   lmitchel
   Initial Checking prior to integration testing for updates made to support country/curreny code limits (scr13929), unulimited purchase card MCC ranges, and purchase card transaction limits(scr12785)
   
      Rev 1.11   Dec 01 2003 11:30:06   lmitchel
   Added AMEX Online Updates - scr1200
   
      Rev 1.10   Apr 17 2000 09:57:48   iarustam
   added bin blocking
   
   
      Rev 1.9   Jan 27 2000 13:06:38   iarustam
   SCR # 314,328,330,300,159
   
      Rev 1.8   Jan 13 2000 17:07:48   mbysani
   bug fixed
   
      Rev 1.1   Sep 22 1999 15:58:40   iarustam
   Added manual batch entry GUI
   
      Rev 1.0   Jul 01 1999 14:18:10   iarustam
   Initial Release
   
      Rev 1.0   Jun 28 1999 13:59:18   iarustam
   Initial Release
   
      Rev 1.4   May 10 1999 11:33:14   dperson
   Added PVCS header and
   standardized error messages
*   
************************************************************************************/
// PurchaseCardGeneral.cpp : implementation file
//

#include "stdafx.h"
#include "atp.h"
#include "PurchaseCardGeneral.h"
#include "MyUtils.h"
#include "BinList.h"

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

extern UCF01 ucf01;
extern CSharedMem shrmem;
/////////////////////////////////////////////////////////////////////////////
// CPurchaseCardGeneral property page

IMPLEMENT_DYNCREATE(CPurchaseCardGeneral, CPropertyPage)

CPurchaseCardGeneral::CPurchaseCardGeneral() : CPropertyPage(CPurchaseCardGeneral::IDD)
{
	//{{AFX_DATA_INIT(CPurchaseCardGeneral)
	m_StrCardNumberFld = _T("");
	m_StrCardTypeFld = _T("");
	m_StrHighFld = _T("");
	m_StrLowFld = _T("");
	m_StrMessageLbl = _T("");
	//}}AFX_DATA_INIT
}

CPurchaseCardGeneral::~CPurchaseCardGeneral()
{
}

void CPurchaseCardGeneral::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPurchaseCardGeneral)
	DDX_Control(pDX, IDC_LOW_FLD, m_CntlLowFld);
	DDX_Control(pDX, IDC_LIST, m_List);
	DDX_Control(pDX, IDC_HIGH_FLD, m_CntlHighFld);
	DDX_Control(pDX, IDC_DELETE_BUTTON, m_DeleteButton);
	DDX_Control(pDX, IDC_ADD_BUTTON, m_AddButton);
	DDX_Text(pDX, IDC_CARD_NUMBER_FLD, m_StrCardNumberFld);
	DDX_Text(pDX, IDC_CARD_TYPE_FLD, m_StrCardTypeFld);
	DDX_Text(pDX, IDC_HIGH_FLD, m_StrHighFld);
	DDV_MaxChars(pDX, m_StrHighFld, 4);
	DDX_Text(pDX, IDC_LOW_FLD, m_StrLowFld);
	DDV_MaxChars(pDX, m_StrLowFld, 4);
	DDX_Text(pDX, IDC_MESSAGE_LBL, m_StrMessageLbl);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPurchaseCardGeneral, CPropertyPage)
	//{{AFX_MSG_MAP(CPurchaseCardGeneral)
	ON_BN_CLICKED(IDC_ADD_BUTTON, OnAddButton)
	ON_BN_CLICKED(IDC_DELETE_BUTTON, OnDeleteButton)
	ON_NOTIFY(NM_CLICK, IDC_LIST, OnClickList)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPurchaseCardGeneral message handlers

BOOL CPurchaseCardGeneral::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
	CMyUtils myUtilsObject;
	CFont m_font;
	LOGFONT lf;
	LV_COLUMN lvColumn ;
	char strDate[9] = "";

	memset ( &lf, 0, sizeof (lf));
	lf.lfHeight = 12;
	lf.lfWeight = 700;
	strcpy(lf.lfFaceName, "Arial");
	m_font.CreateFontIndirect (&lf);
	GetDlgItem(IDC_MESSAGE_LBL)->SetFont (&m_font);

    m_bGeneral = TRUE;
//	m_nDialogMode = MODE_DELETE;

	char strHeaderData[5][50] = {"     LOW     ", "   HIGH    "} ;

    for (int nHeader = 0; nHeader < 2; nHeader++) { 
		lvColumn.mask = LVCF_FMT | LVCF_SUBITEM | LVCF_TEXT  ;
		lvColumn.fmt = LVCFMT_LEFT ;
		lvColumn.pszText = strHeaderData[nHeader] ;
		lvColumn.iSubItem = nHeader ;
		
		m_List.InsertColumn(nHeader, &lvColumn) ;  // assumes return value is OK.
        m_List.SetColumnWidth (0, LVSCW_AUTOSIZE_USEHEADER) ;
        m_List.SetColumnWidth (1, LVSCW_AUTOSIZE_USEHEADER) ;
        
    }
	

	
	if (m_nDialogMode != MODE_DONE )
	{
		PopulateGeneralDialogFromDatabase ( sCCF04RecordOld );
	}   
	
	if (!UpdateData(FALSE)){
		AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
	}	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CPurchaseCardGeneral::CleanGeneralDialog()
{
	m_StrCardNumberFld = _T("");
	m_StrCardTypeFld = _T("");
	m_StrHighFld = _T("");
	m_StrLowFld = _T("");
	m_StrMessageLbl = _T("");
	m_List.DeleteAllItems();

	UpdateData (FALSE);
}

void CPurchaseCardGeneral::PopulateHistoryTable()
{
    HST01 sHistoryData;
    CHAR dest_str[18];   
	CMyUtils myUtilsObject;
	char strErrorMsg[200] = "";
   CString temp;

    char strDate[9] = "";
    char strTime[7] = "";

	char strPrimaryKey[50];

    memset ( strDate, 0, sizeof (strDate));
    memset ( strTime, 0, sizeof (strTime));
	memset ( strPrimaryKey, 0, sizeof (strPrimaryKey));

	if ( strcmp (m_StrCardTypeFld, "CREDIT" ) == 0 )
		temp = "C";
	else
	if ( strcmp (m_StrCardTypeFld, "DEBIT" ) == 0 )
		temp = "D";
	else
	if ( strcmp (m_StrCardTypeFld, "LOYALTY" ) == 0 )
		temp = "L";
	else
	if ( strcmp (m_StrCardTypeFld, "FLEET" ) == 0 )
		temp = "F";

	strcpy ( strPrimaryKey, m_StrCardNumberFld);
	strcat ( strPrimaryKey, temp );
    
    myUtilsObject.GetSystemDate(strDate);  

    myUtilsObject.GetSystemTime(strTime);
    pintime_get_gmt ( dest_str );
    ZeroMemory ( &sHistoryData, sizeof (HST01));
    memcpy ((LPTSTR)sHistoryData.file_name,				(LPCTSTR)"CCF04",			sizeof (sHistoryData.file_name));
    temp = strDate;
    temp = temp.Right(4) + temp.Left(2) + temp.Mid(2,2);
    
    CHAR wstation[21]; DWORD wsize;

    wsize = sizeof( wstation );
    GetComputerName( wstation, &wsize );

	memcpy ((LPTSTR)sHistoryData.reserved,(LPTSTR)wstation, sizeof(sHistoryData.reserved));
    memcpy ((LPTSTR) sHistoryData.date_modified, temp, sizeof (sHistoryData.date_modified));
    memcpy ((LPTSTR)sHistoryData.file_primary_key,		(LPCTSTR)strPrimaryKey,		sizeof (sHistoryData.file_primary_key));
    memcpy ((LPTSTR)sHistoryData.primary_key.unique_id,	(LPCTSTR)dest_str,			sizeof (sHistoryData.primary_key.unique_id));        
    memcpy ((LPTSTR)sHistoryData.time_modified,			(LPCTSTR)strTime,			sizeof (sHistoryData.time_modified)); 
             
    memcpy ((LPTSTR)sHistoryData.current_data,			(LPCTSTR)m_strCurrentData,	sizeof (sHistoryData.current_data));
	memcpy ((LPTSTR)sHistoryData.previous_data,			(LPCTSTR)m_strPreviousData, sizeof (sHistoryData.previous_data));
	memcpy ((LPTSTR)sHistoryData.field_tag,				(LPCTSTR)m_strFieldTag,		sizeof (sHistoryData.field_tag));
	memcpy ((LPTSTR)sHistoryData.changed_by,			(LPTSTR)ucf01.primary_key.name, sizeof(sHistoryData.changed_by));

    int rcode = txdsapi_insert_record  ( (pBYTE)&sHistoryData, sizeof (HST01), HST01_DATA, strErrorMsg );
	if ( rcode != PTEMSG_OK )
	{
		m_StrMessageLbl = strErrorMsg;
		UpdateData( FALSE );
	}
}

BOOL CPurchaseCardGeneral::PopulateGeneralDialogFromDatabase ( CCF04 sCCFRecord )
{
	CString temp;
	CMyUtils myUtilsObject;
	int nRowItem = 0;
    int nActualItem = 0;
	int rcode = 0;
	int num_returned = 0;

    CHAR error_msg[256];
	CHAR strSum[50];
	CFont m_font;
	LV_ITEM lvItem;
    bDone = FALSE;

//	if ( m_nDialogMode != MODE_VIEW )
//       EnableCntryAmtLmtDialog(TRUE);

    ZeroMemory ( &sCCF04RecordOld, sizeof (CCF04));

	m_StrMessageLbl = "";
	//empty list control & edit boxes
	m_List.DeleteAllItems();
	m_StrLowFld.Empty();
	m_StrHighFld.Empty();
	m_List.EnableWindow(TRUE);

//	m_StrCardNumberFld = sCCFRecord.primary_key.card_nbr;
//	m_StrCardTypeFld = sCCFRecord.primary_key.card_type;

	if ( strcmp ( m_StrCardTypeFld.Left(1), "C" ) == 0 )
		m_StrCardTypeFld = "CREDIT";
	else
	if ( strcmp ( m_StrCardTypeFld.Left(1), "D" ) == 0 )
		m_StrCardTypeFld = "DEBIT";
	else
	if ( strcmp ( m_StrCardTypeFld.Left(1), "F" ) == 0 )
		m_StrCardTypeFld = "FLEET";
	else
	if ( strcmp ( m_StrCardTypeFld.Left(1), "L" ) == 0 )
		m_StrCardTypeFld = "LOYALTY";


	strcpy( (pCHAR)m_ccf04_limits.primary_key.card_nbr, m_StrCardNumberFld);
	strcpy( (pCHAR)m_ccf04_limits.primary_key.card_type, m_StrCardTypeFld.Left(1));

	/*Get Purchase Card MCC List for card number and card type*/
	rcode = txdsapi_get_list( (pBYTE) &m_ccf04_limits, sizeof(CCF04),
		                      (pBYTE) &m_ccf04_gui_list, sizeof(CCF04_GUI_LIST),
							  0, CCF04_DATA, error_msg );

	if(rcode != PTEMSG_OK)
	{
		m_StrMessageLbl = error_msg;
		UpdateData(FALSE);
	}

	num_returned = atoi( (pCHAR)m_ccf04_gui_list.num_returned);

	if(num_returned == 0)
	{
		m_StrMessageLbl = "No Data Found";
		UpdateData(FALSE);
		return(FALSE);
	}

	if (rcode == PTEMSG_OK)
	{
		if( num_returned == SELECT_LIST_SIZE)
		{
		    strcpy( (pCHAR)m_ccf04_limits.primary_key.card_nbr,  (pCHAR)m_ccf04_gui_list.ccf04_list[num_returned-1].primary_key.card_nbr );
			strcpy( (pCHAR)m_ccf04_limits.primary_key.card_type, (pCHAR)m_ccf04_gui_list.ccf04_list[num_returned-1].primary_key.card_type);
		}
	    else
		{
				bDone = TRUE;
		}

		UpdateData(FALSE);

	
		//insert items into list view control
		for ( int nItem = 0; nItem < num_returned; nItem++ )
		{
			m_List.EnableWindow(TRUE);

			for(int nSubItem = 0; nSubItem < 2; nSubItem++)
			{
				lvItem.mask = LVIF_TEXT;
				lvItem.iItem = (nSubItem == 0)? nRowItem : nActualItem;
				lvItem.iSubItem = nSubItem;
				temp.Empty();
				memset(strSum, 0, sizeof(strSum));

				if(nSubItem == 0)
				{	
					temp = (char *)m_ccf04_gui_list.ccf04_list[nItem].mcc_lo_range;
					temp.TrimLeft() ;
					temp.TrimRight();
					sprintf(strSum, "%-4s", (LPCTSTR)temp);
					lvItem.pszText = strSum;
				}
				else
				if(nSubItem == 1)
				{
					temp = (char *)m_ccf04_gui_list.ccf04_list[nItem].mcc_hi_range;
					temp.TrimLeft() ;
					temp.TrimRight();
					sprintf(strSum, "%-4s", (LPCTSTR)temp);
					lvItem.pszText = strSum;
				}


				if(nSubItem == 0)
					nActualItem = m_List.InsertItem(&lvItem);
				else
					m_List.SetItem(&lvItem); 

				m_List.SetFocus();
			}//for insert sub items
		}//for insert row items
		
		m_List.SetFocus();
	}//for pte_msg_ok

	UpdateData(FALSE);

    memcpy ( &sCCF04RecordOld, &sCCF04Record, sizeof ( CCF04));


	return TRUE;


}

BOOL CPurchaseCardGeneral::CompareStructures()
{
	int nNewCount;
	BOOL bFound = FALSE;
	CString temp1, temp2, temp3, temp4;
	int i,j;
	CMyUtils myUtilsObject;
    char strDate[9] = "";


	nNewCount = m_List.GetItemCount();     
    
	for (  i = 0; i < m_nOldCount; i++ )
    { 
		bFound = FALSE;
		for (  j = 0; j < nNewCount; j++ )
		{
			temp1 = m_List.GetItemText (j, 0) ;
			temp2 = m_List.GetItemText (j, 1) ;
			
			int nResult = strcmp ((char *)sCCF04RecordOld.mcc_lo_range,temp1);
			int nResult1 = strcmp ((char *)sCCF04RecordOld.mcc_hi_range,temp2);
			if ( nResult == 0 && nResult1 == 0)
			{
				bFound = TRUE;
			}
		}
		if ( !bFound )
		{
			temp3 = sCCF04RecordOld.mcc_lo_range;
			temp4 = sCCF04RecordOld.mcc_hi_range;
			strcpy (m_strCurrentData, "Range Deleted");
			strcpy (m_strPreviousData, temp3);
			strcat (m_strPreviousData, " - ");	
			strcat (m_strPreviousData, temp4);

			strcpy (m_strFieldTag, "Range");

			PopulateHistoryTable();
		}
	}

	bFound = FALSE;
    
	for (  i = 0; i < nNewCount; i++ )
    { 
		bFound = FALSE;
		for ( int j = 0; j < m_nOldCount; j++ )
		{	
			temp1 = m_List.GetItemText (i, 0) ;
			temp2 = m_List.GetItemText (i, 1) ;
			
			int nResult = strcmp ((char *)sCCF04RecordOld.mcc_lo_range,temp1);
			int nResult1 = strcmp ((char *)sCCF04RecordOld.mcc_hi_range,temp2);
			if ( nResult == 0 && nResult1 == 0)
			{
				bFound = TRUE;
			}
		}
	
		if ( !bFound )
		{
			strcpy (m_strCurrentData, "Range Added");
			strcpy (m_strPreviousData, temp1);
			strcat (m_strPreviousData, " - ");	
			strcat (m_strPreviousData, temp2);

			strcpy (m_strFieldTag, "Range");

			PopulateHistoryTable();

		}
	}
	return TRUE;
}

BOOL CPurchaseCardGeneral::OnSetActive() 
{
	if 	( m_nDialogMode == MODE_DONE || m_nDialogMode == MODE_VIEW )
	{
		m_CntlHighFld.EnableWindow(FALSE);
		m_CntlLowFld.EnableWindow(FALSE);
		m_AddButton.EnableWindow(FALSE);
		m_DeleteButton.EnableWindow(FALSE);
	}
	else if ( m_nDialogMode == MODE_ADD || m_nDialogMode == MODE_EDIT || m_nDialogMode == MODE_DELETE )
	{
		m_CntlHighFld.EnableWindow(TRUE);
		m_CntlLowFld.EnableWindow(TRUE);
		m_AddButton.EnableWindow(TRUE);
		m_DeleteButton.EnableWindow(TRUE);
	}
	
	PopulateGeneralDialogFromDatabase ( sCCF04RecordOld );

	
	return CPropertyPage::OnSetActive();
}

BOOL CPurchaseCardGeneral::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	if ( m_nDialogMode != MODE_VIEW &&  m_nDialogMode != MODE_DONE)
		SetModified ( TRUE );

	
	c_time = CTime::GetCurrentTime();	
	shrmem.SetLastUpdateTime(c_time);
	return CPropertyPage::OnCommand(wParam, lParam);
}

BOOL  CPurchaseCardGeneral::PopulateDatabaseFromGeneralDialog ( pCCF04 psCCFRecord )
{
//	int nCount;
	CString temp1, temp2;
	CString strType;
	CMyUtils myUtilsObject;

	ZeroMemory (psCCFRecord, sizeof(CCF04)) ;	
    
	if ( strcmp (m_StrCardTypeFld, "CREDIT" ) == 0 )
		strType = "C";
	else
	if ( strcmp (m_StrCardTypeFld, "DEBIT" ) == 0 )
		strType = "D";
	else
	if ( strcmp (m_StrCardTypeFld, "LOYALTY" ) == 0 )
		strType = "L";
	else
	if ( strcmp (m_StrCardTypeFld, "FLEET" ) == 0 )
		strType = "F";

	memcpy ((LPTSTR) psCCFRecord->primary_key.card_type, strType, sizeof (psCCFRecord->primary_key.card_type));
	memcpy ((LPTSTR) psCCFRecord->primary_key.card_nbr,m_StrCardNumberFld, sizeof (psCCFRecord->primary_key.card_nbr));
	memcpy ((LPTSTR) psCCFRecord->mcc_lo_range, m_StrLowFld, sizeof(psCCFRecord->mcc_lo_range));
	memcpy ((LPTSTR) psCCFRecord->mcc_hi_range, m_StrHighFld, sizeof(psCCFRecord->mcc_hi_range));


/*	nCount = m_List.GetItemCount();

	for ( int i = 0; i < nCount; i++ )
	{
		temp1 = m_List.GetItemText (i, 0) ;
		temp2 = m_List.GetItemText (i, 1) ;	

		UpdateData(TRUE);
	
		memcpy (psCCFRecord->mcc_lo_range,  temp1, sizeof (psCCFRecord->mcc_lo_range));
		memcpy (psCCFRecord->mcc_hi_range,  temp2, sizeof (psCCFRecord->mcc_hi_range));
	
	}
*/

	return TRUE;

}


void CPurchaseCardGeneral::OnAddButton() 
{
	LV_ITEM lvItem;
	CCF04 sCCFDatabaseStructure;

	char strTemp[30];
	CString strItem;
	CString templo, temphi;
	CMyUtils myUtilsObject;
	UpdateData (TRUE);
	m_StrMessageLbl = "";
	UpdateData (FALSE);
	
    int nRowItem = 0;
	int nCount = 0;
    int nActualItem = 0;
	int nLow, nHigh;
	int nSelectedIndex = -1;
	int rcode = 0;

	char  error_msg[100] = {0};

	
	UpdateData(TRUE);

	/*Check list is not full at 500*/
	nCount = m_List.GetItemCount();
	if (nCount >= MAX_CNT) /*Max codes that can be defined is 500)*/
	{
		sprintf (strTemp, "ERROR: MCC Range count cannot exceed %d. Current count = %d", MAX_CNT,nCount);
		m_StrMessageLbl = strTemp;
		UpdateData(FALSE);
		return ;
	}
	
	if (myUtilsObject.IsEmpty (m_StrLowFld, this))
	{
		m_CntlLowFld.SetFocus();
		m_StrMessageLbl = "Please enter Low Range";
		UpdateData(FALSE);
		return;
	}

	if (myUtilsObject.IsEmpty (m_StrHighFld, this))
	{
		m_CntlHighFld.SetFocus();
		m_StrMessageLbl = "Please enter High Range";
		UpdateData(FALSE);
		return;
	}

	nLow = atoi ( m_StrLowFld );
	nHigh = atoi ( m_StrHighFld );

	if ( nLow > nHigh )
	{
		m_CntlLowFld.SetFocus();
		m_StrMessageLbl = "ERROR: Low Range cannot be greater than High Range";
		UpdateData(FALSE);
		return;
	}

	/*verify MCC Lo Range does not already exist*/
/*	if(nCount > 0) 
	{
		for ( int i = 0; i < nCount; i++ )
		{
		  temp = m_List.GetItemText (i, 0) ;
  		  temp.TrimLeft();
		  temp.TrimRight();

		  if ( strcmp ( temp, m_StrLowFld ) == 0)
		  {
					nSelectedIndex = i;
					break;
		  }
		}

		if ( nSelectedIndex != -1 )
		{
			m_StrMessageLbl = "ERROR: MCC Low Range for Purchase Card Already Exists";		
	
			if (!UpdateData(FALSE))
			{
				AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
			}
			return ;
		}
	}*/

		/*verify MCC Lo and Hi values do not already exist within a previously defined range*/
	if(nCount > 0) 
	{
		for ( int i = 0; i < nCount; i++ )
		{
		  templo = m_List.GetItemText (i, 0) ; /*low range*/
		  temphi = m_List.GetItemText (i, 1) ; /*high range*/
  		  templo.TrimLeft();
		  templo.TrimRight();
  		  temphi.TrimLeft();
		  temphi.TrimRight();

		  if(m_StrLowFld >= templo && m_StrLowFld <= temphi)
		  {
			  m_StrMessageLbl = "ERROR: MCC Low Range Already Exists Within a Range";
			  nSelectedIndex = i;
			  break;
		  }
		
		  if (m_StrHighFld >= templo && m_StrHighFld <= temphi)
		  {
  			  m_StrMessageLbl = "ERROR: MCC High Range Already Exists Within a Range";
			  nSelectedIndex = i;
			  break;
		  }
		}

		if ( nSelectedIndex != -1 )
		{
			m_CntlLowFld.SetFocus();

			if (!UpdateData(FALSE))
			{
				AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
			}
			return ;
		}
	}

	UpdateData(TRUE);

	/* Insert new range into MCC Range List */
	for (int nSubItem = 0; nSubItem < 2; nSubItem++) {					
					
		lvItem.mask = LVIF_TEXT ;
		lvItem.iItem = (nSubItem == 0)? nRowItem : nActualItem;
		lvItem.iSubItem = nSubItem;

		if (nSubItem == 0) {
			memcpy ( strTemp, m_StrLowFld, sizeof ( strTemp));
			lvItem.pszText = strTemp;
		}

		if (nSubItem == 1) { 
			memcpy ( strTemp, m_StrHighFld, sizeof ( strTemp));
			lvItem.pszText = strTemp;                   
		}		
		
					
		if (nSubItem == 0) 
			nActualItem = m_List.InsertItem(&lvItem);
		else 
			m_List.SetItem(&lvItem);

		int nCount = m_List.GetItemCount();


		m_List.SetFocus() ;	
	}

	/*Insert mcc range into Database */
	PopulateDatabaseFromGeneralDialog(&sCCFDatabaseStructure );

	rcode = txdsapi_insert_record( (pBYTE)&sCCFDatabaseStructure, sizeof (CCF04),
									CCF04_DATA, error_msg );

	if (rcode != PTEMSG_OK )
	{
		 m_StrMessageLbl =  error_msg;
		 if (!UpdateData(FALSE))
			 AfxMessageBox (IDS_UPDATEDATAFAILED, MB_ICONSTOP);
		 return;
	}

	//log to history table
	memcpy ((LPSTR)m_strCurrentData, "Record Added" , sizeof (m_strCurrentData));
	memset  ((LPSTR)m_strPreviousData, 0, sizeof (m_strPreviousData));
    sprintf((LPSTR)m_strFieldTag, "MCC Range %s - %s Added", m_StrLowFld, m_StrHighFld);
	PopulateHistoryTable(); 


	m_StrMessageLbl = "Record Added";

	m_CntlLowFld.SetFocus();
	nCount = m_List.GetItemCount();
	UpdateData(FALSE);


}

void CPurchaseCardGeneral::OnDeleteButton() 
{
	CCF04  sCCFDatabaseStructure;
	
	CString strMessage;
	char strErrorMessage[200] = "";
	char  error_msg[100] = {0};

	int nSelectedIndex;
	int nReturnCode = 0;
	int rcode = 0;

	UpdateData (TRUE);
	m_StrMessageLbl = "";
	UpdateData(FALSE);

	nSelectedIndex = m_List.GetNextItem (-1, LVNI_SELECTED) ;

	if ( nSelectedIndex == -1 )
	{
		m_StrMessageLbl = "Please select an item to delete";
		UpdateData(FALSE);
		return;
	}

	/*Confirm the delete ? */
	strMessage.Format ("Are you sure you want to delete \nMCC Range %s - %s for card %s?", 
							(LPCTSTR) m_StrLowFld, (LPCTSTR) m_StrHighFld, (LPCTSTR) m_StrCardNumberFld) ;

	nReturnCode = AfxMessageBox ((LPCTSTR)strMessage, MB_OKCANCEL|MB_ICONQUESTION) ;
	if (nReturnCode != IDOK) 
		return ;

	/*Delete mcc range from Database */
	PopulateDatabaseFromGeneralDialog(&sCCFDatabaseStructure );

	rcode = txdsapi_delete_record( (pBYTE)&sCCFDatabaseStructure, sizeof (CCF04), CCF04_DATA, error_msg );
	
	// failed to delete
	if ( PTEMSG_OK != rcode   )
    {
		 switch( rcode )
      {
         case PTEMSG_NOT_FOUND:
			sprintf( strErrorMessage, "ERROR: Card Number %s does not exist", m_StrCardNumberFld );
            break;
      }

        m_StrMessageLbl = strErrorMessage;
		m_nDialogMode = MODE_DONE;
		if (!UpdateData(FALSE)){
			AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
		}
		return ;
	}

    else
    {
		//log to history table
		memcpy ((LPSTR)m_strCurrentData, "Record Deleted", sizeof (m_strCurrentData));
		memcpy  ((LPSTR)m_strPreviousData, m_StrLowFld, sizeof (m_strPreviousData));
		sprintf((LPSTR)m_strFieldTag, "MCC Range %s - %s Deleted", m_StrLowFld, m_StrHighFld);
		PopulateHistoryTable(); 

/*		m_StrCardNumberFld = _T("");	
		m_StrCardTypeFld = _T(""); */
        m_StrMessageLbl = "Record Deleted";

		if (!UpdateData(FALSE)){
			AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
		}

	//	CleanGeneralDialog();
    }


	/*Remove record from list control */
	m_List.DeleteItem(nSelectedIndex);

	m_AddButton.EnableWindow(TRUE);
	m_StrMessageLbl = "Record Deleted";
	m_CntlLowFld.SetFocus();
	UpdateData(FALSE);	
}

BOOL CPurchaseCardGeneral::CheckDialogForCompleteness() 
{

	CMyUtils myUtilsObject;

    if (!UpdateData(TRUE)){
		AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
	}

    if (myUtilsObject.IsEmpty (m_StrLowFld, this))
    {
		m_StrMessageLbl = "Please enter MCC Low Range";	
		m_CntlLowFld.SetFocus();
		if (!UpdateData(FALSE)){
			AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
		}
		m_nDialogMode = MODE_DONE;
		return FALSE;
	}

	if (myUtilsObject.IsEmpty (m_StrHighFld, this))
    {
		m_StrMessageLbl = "Please enter Card Type";	
		m_CntlHighFld.SetFocus();
		if (!UpdateData(FALSE)){
			AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
		}
		m_nDialogMode = MODE_DONE;
		return FALSE;
	}
	
    return TRUE;
}

void CPurchaseCardGeneral::OnClickList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	CString temp;

	INT nSelectedIndex = -1;

	if(m_nDialogMode != MODE_VIEW)
	{
		nSelectedIndex = m_List.GetNextItem (-1, LVNI_SELECTED) ;
		
		temp  = m_List.GetItemText(nSelectedIndex, 0);
  		temp.TrimLeft();
		temp.TrimRight();
		m_StrLowFld = temp;

      m_StrHighFld  = m_List.GetItemText(nSelectedIndex, 1); 
      m_List.SetFocus();

      UpdateData(FALSE);
	}

}
