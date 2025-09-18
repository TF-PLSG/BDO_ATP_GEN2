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
* $Log:   N:\PVCS\PTE\Equitable\ATP_gui\MerchantSettlement.cpp  $
   
      Rev 1.10   Jul 08 2004 13:00:38   dirby
   Updated History File logging to make sure Merchant ID is in the
   File_Primary_Key and to make sure fields are not accidently logged
   when no changes have been made.
   SCR 1421
   
      Rev 1.9   Dec 01 2003 11:29:40   lmitchel
   Added AMEX Online Updates - scr1200
   
      Rev 1.8   Jan 12 2000 14:14:34   mbysani
   Changes mades acc to bugs
   
      Rev 1.1   Sep 22 1999 15:53:40   iarustam
   Added manual batch entry GUI
   
      Rev 1.0   Jul 01 1999 14:17:48   iarustam
   Initial Release
   
      Rev 1.0   Jun 28 1999 13:58:54   iarustam
   Initial Release
   
      Rev 1.4   Jun 04 1999 17:40:34   iarustam
   Bug fixes
   
      Rev 1.3   May 10 1999 11:25:12   dperson
   Standardized error messages
   
      Rev 1.2   Apr 29 1999 14:56:38   iarustam
   Bug fixes
   
      Rev 1.1   Mar 16 1999 16:30:06   iarustam
   Bug fixes
   
      Rev 1.0   Feb 23 1999 11:40:44   iarustam
   Initial Release
   
      Rev 1.0   Feb 17 1999 10:09:38   iarustam
   Initial Release
   
      Rev 1.1   Jan 22 1999 10:59:16   iarustam
   Bug fixes and core 1_3_0 changes
   
      Rev 1.1   Jan 21 1999 13:08:38   iarustam
   Bug fixes
   
   
      Rev 1.2   Jan 13 1999 14:20:22   iarustam
    
   
      Rev 1.1   Dec 18 1998 13:43:46   iarustam
    
*
************************************************************************************/
// MerchantSettlement.cpp : implementation file
//

#include "stdafx.h"
#include "Atp.h"
#include "MerchantSettlement.h"
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

extern UCF01 ucf01;

extern CSharedMem shrmem;

/////////////////////////////////////////////////////////////////////////////
// CMerchantSettlement property page

IMPLEMENT_DYNCREATE(CMerchantSettlement, CPropertyPage)

CMerchantSettlement::CMerchantSettlement() : CPropertyPage(CMerchantSettlement::IDD)
{
	//{{AFX_DATA_INIT(CMerchantSettlement)
	m_StrAquiringIDFld = _T("");
	m_StrCorporationIDFld = _T("");
	m_StrFloorLimitFld = _T("");
	m_StrMerchantIDFld = _T("");
	m_StrMessageLbl = _T("");
	m_StrOrganizationIDFld = _T("");
	m_StrSettleMerchantIDFld = _T("");
	m_StrCardTypeFld = _T("");
	//}}AFX_DATA_INIT
}

CMerchantSettlement::~CMerchantSettlement()
{
}

void CMerchantSettlement::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMerchantSettlement)
	DDX_Control(pDX, IDC_CARD_TYPE, m_CntlCardTypeFld);
	DDX_Control(pDX, IDC_LIST, m_CntlList);
	DDX_Control(pDX, IDC_UPDATE_BUTTON, m_UpdateButton);
	DDX_Control(pDX, IDC_NEXT_BUTTON, m_NextButton);
	DDX_Control(pDX, IDC_DELETE_BUTTON, m_DeleteButton);
	DDX_Control(pDX, IDC_ADD_BUTTON, m_AddButton);
	DDX_Control(pDX, IDC_SETTLE_MERCHANT_ID_FLD, m_CntlSettleMerchantIDFld);
	DDX_Control(pDX, IDC_FLOOR_LIMIT_FLD, m_CntlFloorLimitFld);
	DDX_Control(pDX, IDC_CORPORATION_ID_FLD, m_CntlCorporationIDFld);
	DDX_Control(pDX, IDC_AQUIRING_ID_FLD, m_CntlAquiringIDFld);
	DDX_Text(pDX, IDC_AQUIRING_ID_FLD, m_StrAquiringIDFld);
	DDV_MaxChars(pDX, m_StrAquiringIDFld, 6);
	DDX_Text(pDX, IDC_CORPORATION_ID_FLD, m_StrCorporationIDFld);
	DDV_MaxChars(pDX, m_StrCorporationIDFld, 6);
	DDX_Text(pDX, IDC_FLOOR_LIMIT_FLD, m_StrFloorLimitFld);
	DDV_MaxChars(pDX, m_StrFloorLimitFld, 12);
	DDX_Text(pDX, IDC_MERCHANT_ID_FLD, m_StrMerchantIDFld);
	DDX_Text(pDX, IDC_MESSAGE_LBL, m_StrMessageLbl);
	DDX_Text(pDX, IDC_ORGANIZATION_ID_FLD, m_StrOrganizationIDFld);
	DDX_Text(pDX, IDC_SETTLE_MERCHANT_ID_FLD, m_StrSettleMerchantIDFld);
	DDV_MaxChars(pDX, m_StrSettleMerchantIDFld, 16);
	DDX_CBString(pDX, IDC_CARD_TYPE, m_StrCardTypeFld);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CMerchantSettlement, CPropertyPage)
	//{{AFX_MSG_MAP(CMerchantSettlement)
	ON_BN_CLICKED(IDC_ADD_BUTTON, OnAddButton)
	ON_BN_CLICKED(IDC_DELETE_BUTTON, OnDeleteButton)
	ON_BN_CLICKED(IDC_NEXT_BUTTON, OnNextButton)
	ON_BN_CLICKED(IDC_UPDATE_BUTTON, OnUpdateButton)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST, OnDblclkList)
	ON_NOTIFY(NM_CLICK, IDC_LIST, OnClickList)
	ON_CBN_SETFOCUS(IDC_CARD_TYPE, OnSetfocusCardType)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMerchantSettlement message handlers

BOOL CMerchantSettlement::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();

	LV_COLUMN lvColumn ;
	CFont m_font;
	LOGFONT lf;

	char strErrorMessage[200];

	CTF01 sCardTypeRecord;
	CTF01_LIST sCardTypeList;

	memset ( &lf, 0, sizeof (lf));
	lf.lfHeight = 12;
	lf.lfWeight = 700;
	strcpy(lf.lfFaceName, "Arial");
	m_font.CreateFontIndirect (&lf);
	GetDlgItem(IDC_MESSAGE_LBL)->SetFont (&m_font);

	m_bSettlement = TRUE;

	char strHeaderData[5][50] = {"CARD TYPE       ", "SETTLEMENT MERCHANT ID ","CORPORATION ID          ", "ACQUIRING ID    ", "FLOOR LIMIT    "} ;

    for (int nHeader = 0; nHeader < 5; nHeader++) { 
		lvColumn.mask = LVCF_FMT | LVCF_SUBITEM | LVCF_TEXT  ;
		lvColumn.fmt = LVCFMT_LEFT ;
		lvColumn.pszText = strHeaderData[nHeader] ;
		lvColumn.iSubItem = nHeader ;
		
		m_CntlList.InsertColumn(nHeader, &lvColumn) ;  // assumes return value is OK.
        m_CntlList.SetColumnWidth (0, LVSCW_AUTOSIZE_USEHEADER) ;
        m_CntlList.SetColumnWidth (1, LVSCW_AUTOSIZE_USEHEADER) ;
        m_CntlList.SetColumnWidth (2, LVSCW_AUTOSIZE_USEHEADER) ;
        m_CntlList.SetColumnWidth (3, LVSCW_AUTOSIZE_USEHEADER) ;
        m_CntlList.SetColumnWidth (4, LVSCW_AUTOSIZE_USEHEADER) ;
        m_CntlList.SetColumnWidth (5, LVSCW_AUTOSIZE_USEHEADER) ;
        
    }

	memcpy(sCardTypeRecord.primary_key.card_type, "    ", sizeof (sCardTypeRecord.primary_key.card_type));

	int rcode = txdsapi_get_service_list ((pBYTE)&sCardTypeRecord, sizeof (CTF01),
										  (pBYTE)&sCardTypeList, sizeof (CTF01_LIST),
										  0, CTF01_DATA,  strErrorMessage);
	if (rcode == PTEMSG_OK)
	{
		int nNumberReturned = atoi ((char *)&sCardTypeList.num_returned);
		for ( int i = 0; i < nNumberReturned; i++ ) {
			m_CntlCardTypeFld.AddString((char *)sCardTypeList.card_type[i]);  			
        }	
	}

	else 
	{
		m_StrMessageLbl = strErrorMessage;
		return FALSE;
	}

/*	if (m_nDialogMode != MODE_ADD && m_nDialogMode != MODE_DONE)   
	{
		memcpy(sMerchantCardRecord.primary_key.card_type, "    ", sizeof (sMerchantCardRecord.primary_key.card_type));
		memcpy (sMerchantCardRecord.primary_key.organization_id, m_StrOrganizationIDFld, sizeof (sMerchantCardRecord.primary_key.organization_id));
		memcpy (sMerchantCardRecord.primary_key.merchant_id,     m_StrMerchantIDFld, sizeof (sMerchantCardRecord.primary_key.merchant_id));
		PopulateSettlementList();	
	}
	else 
	{
		m_UpdateButton.EnableWindow(FALSE);
		m_DeleteButton.EnableWindow(FALSE);
	}*/

	if (m_nDialogMode != MODE_DONE)
	{
		if ( m_nDialogMode != MODE_VIEW )
			EnableSettlementDialog(TRUE);
		else
			m_AddButton.EnableWindow(FALSE);
		if ( m_nDialogMode != MODE_ADD )
		{
			memcpy(sMerchantCardRecord.primary_key.card_type, "    ", sizeof (sMerchantCardRecord.primary_key.card_type));
			memcpy (sMerchantCardRecord.primary_key.organization_id, m_StrOrganizationIDFld, sizeof (sMerchantCardRecord.primary_key.organization_id));
			memcpy (sMerchantCardRecord.primary_key.merchant_id,     m_StrMerchantIDFld, sizeof (sMerchantCardRecord.primary_key.merchant_id));
			PopulateSettlementList();
		}
	} 

	else
	{
		m_AddButton.EnableWindow(FALSE);
		
		m_NextButton.EnableWindow(FALSE);
	}
	
	m_UpdateButton.EnableWindow(FALSE);
	m_DeleteButton.EnableWindow(FALSE);
	
	if (!UpdateData(FALSE)){
		AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
	}

	m_CntlCardTypeFld.SetCurSel(0);

	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CMerchantSettlement::PopulateSettlementList()
{
	CFont m_font;
	LV_ITEM lvItem;
	char strErrorMessage[200];
    
    int nRowItem = 0;
    int nActualItem = 0;
    int nNumberReturned;
    bDone = FALSE;

	UpdateData (TRUE);
	m_StrMessageLbl = "";
	UpdateData (FALSE);


	if ( m_nDialogMode != MODE_VIEW )	
		EnableSettlementDialog(TRUE);

	memcpy (sMerchantCardRecord.primary_key.organization_id, m_StrOrganizationIDFld, sizeof (sMerchantCardRecord.primary_key.organization_id));
	memcpy (sMerchantCardRecord.primary_key.merchant_id,     m_StrMerchantIDFld, sizeof (sMerchantCardRecord.primary_key.merchant_id));
	
	if ( m_nDialogMode != MODE_ADD)
	{
	int rcode = txdsapi_get_list ((pBYTE)&sMerchantCardRecord, sizeof (MCT01),
								  (pBYTE)&sMerchantCardList, sizeof (MCT01_GUI_LIST),
								  0, MCT01_DATA, strErrorMessage);
		if (rcode == PTEMSG_OK)
		{
			nNumberReturned = atoi ((char *)&sMerchantCardList.num_returned);
            if( nNumberReturned == GUI_MAX_LIST_SIZE)
            {				
				memcpy (sMerchantCardRecord.primary_key.card_type,	     sMerchantCardList.merch_card[nNumberReturned-1].primary_key.card_type, sizeof (sMerchantCardRecord.primary_key.card_type));
	//			memcpy (sMerchantCardRecord.primary_key.organization_id, sMerchantCardList.merch_card[nNumberReturned-1].primary_key.organization_id, sizeof (sMerchantCardRecord.primary_key.organization_id));
	//			memcpy (sMerchantCardRecord.primary_key.merchant_id,     sMerchantCardList.merch_card[nNumberReturned-1].primary_key.merchant_id, sizeof (sMerchantCardRecord.primary_key.merchant_id));					
			}
			else
				bDone = TRUE;
						
			for (int nItem = 0; nItem <  nNumberReturned; nItem++,nRowItem ++) { // will now insert the items and subitems into the list view.
				m_CntlList.EnableWindow(TRUE);  
			    for (int nSubItem = 0; nSubItem < 5; nSubItem++) {					
					
			    lvItem.mask = LVIF_TEXT ;
				lvItem.iItem = (nSubItem == 0)? nRowItem : nActualItem;
				lvItem.iSubItem = nSubItem;

				if (nSubItem == 0) { 
					lvItem.pszText = (char *)sMerchantCardList.merch_card[nItem].primary_key.card_type;                   
				}

				if (nSubItem == 1) { 
					lvItem.pszText = (char *)sMerchantCardList.merch_card[nItem].settlement_id;                   
				}	
				
				if (nSubItem == 2) { 
					lvItem.pszText = (char *)sMerchantCardList.merch_card[nItem].corporate_id;                   
				}		

				if (nSubItem == 3) { 
					lvItem.pszText = (char *)sMerchantCardList.merch_card[nItem].acquirer_id;                   
				}		

				if (nSubItem == 4) { 
					lvItem.pszText = (char *)sMerchantCardList.merch_card[nItem].floor_limit;                   
				}		
		
						
				if (nSubItem == 0) 
					nActualItem = m_CntlList.InsertItem(&lvItem);
				else 
					m_CntlList.SetItem(&lvItem);

				m_CntlList.SetFocus() ;						
			} // for
		} // for 
//			if (m_CntlList.GetCount() > 0) m_CntlList.SetCurSel(0) ;
		}
	}
}

void CMerchantSettlement::OnNextButton() 
{
	if ( !bDone )
		PopulateSettlementList();
	else 
	{
		m_StrMessageLbl = "ERROR: No data found";
		UpdateData(FALSE);
		m_NextButton.EnableWindow(FALSE);
	}
	
}

void CMerchantSettlement::EnableSettlementDialog ( int nStatus )
{
	m_CntlCardTypeFld.EnableWindow(nStatus);
	m_NextButton.EnableWindow(nStatus);
	m_UpdateButton.EnableWindow(nStatus);
	m_CntlSettleMerchantIDFld.EnableWindow(nStatus);
	m_CntlFloorLimitFld.EnableWindow(nStatus);
	m_DeleteButton.EnableWindow(nStatus);
	m_CntlCorporationIDFld.EnableWindow(nStatus);
	m_CntlAquiringIDFld.EnableWindow(nStatus);
	m_AddButton.EnableWindow(nStatus);


	
}

void CMerchantSettlement::CleanSettlementDialog()
{
	m_StrAquiringIDFld = _T("");
	m_StrCorporationIDFld = _T("");
	m_StrFloorLimitFld = _T("");
	m_StrMerchantIDFld = _T("");
	m_StrMessageLbl = _T("");
	m_StrOrganizationIDFld = _T("");
	m_StrSettleMerchantIDFld = _T("");
	m_StrCardTypeFld = _T("");

	m_CntlCardTypeFld.SetCurSel(0);
	m_CntlList.DeleteAllItems();

	UpdateData (FALSE);
}

void CMerchantSettlement::PopulateHistoryTable()
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

	strcpy ( strPrimaryKey, m_StrMerchantIDFld);
	strcat ( strPrimaryKey, m_StrOrganizationIDFld );
    
    myUtilsObject.GetSystemDate(strDate);  

    myUtilsObject.GetSystemTime(strTime);
    pintime_get_gmt ( dest_str );
    ZeroMemory ( &sHistoryData, sizeof (HST01));
    memcpy ((LPTSTR)sHistoryData.file_name,				(LPCTSTR)"MCF01",			sizeof (sHistoryData.file_name));
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

void CMerchantSettlement::OnAddButton() 
{
	LV_ITEM lvItem;

	char strTemp[30];
	CString strItem;
	MCT01 sMerchantCardRecord;
	char strErrorMessage[200];
	int nSelectedIndex = -1;

	UpdateData (TRUE);
	m_StrMessageLbl = "";
	UpdateData (FALSE);

	m_UpdateButton.EnableWindow(FALSE);
	m_DeleteButton.EnableWindow(FALSE);
	
    int nRowItem = 0;
    int nActualItem = 0;

	
	UpdateData(TRUE);	

	int nCount = m_CntlList.GetItemCount();
	CString temp;

	for ( int i = 0; i < nCount; i++ )
	{
		temp = m_CntlList.GetItemText (i, 0) ;
		if ( strcmp ( temp, m_StrCardTypeFld ) == 0)
			nSelectedIndex = i;
	}

	if ( nSelectedIndex != -1 )
	{
		m_StrMessageLbl = "ERROR: Card Type already exists";		

		if (!UpdateData(FALSE)){
			AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
		}
		return ;
	}

	memcpy ((LPSTR)(sMerchantCardRecord.primary_key.merchant_id), m_StrMerchantIDFld , sizeof (sMerchantCardRecord.primary_key.merchant_id));
	memcpy ((LPSTR)(sMerchantCardRecord.primary_key.organization_id), m_StrOrganizationIDFld , sizeof (sMerchantCardRecord.primary_key.organization_id));  
	memcpy ((LPSTR)(sMerchantCardRecord.primary_key.card_type), m_StrCardTypeFld , sizeof (sMerchantCardRecord.primary_key.card_type));

	int rcode = txdsapi_get_record( (pBYTE)&sMerchantCardRecord, sizeof (MCT01), MCT01_DATA, strErrorMessage );
	if( rcode == PTEMSG_OK )
    {
		m_StrMessageLbl = "ERROR: Card Type already exists";

		if (!UpdateData(FALSE)){
			AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
		}
		return ;
    }
	
	UpdateData(TRUE);

	for (int nSubItem = 0; nSubItem < 5; nSubItem++) {					
					
		lvItem.mask = LVIF_TEXT ;
		lvItem.iItem = (nSubItem == 0)? nRowItem : nActualItem;
		lvItem.iSubItem = nSubItem;

		if (nSubItem == 0) {
			memcpy ( strTemp, m_StrCardTypeFld, sizeof ( strTemp));
			lvItem.pszText = strTemp;
		}

		if (nSubItem == 1) { 
			memcpy ( strTemp, m_StrSettleMerchantIDFld, sizeof ( strTemp));
			lvItem.pszText = strTemp;                   
		}		
		
		if (nSubItem == 2) { 
			memcpy ( strTemp, m_StrCorporationIDFld, sizeof ( strTemp));
			lvItem.pszText = strTemp;                   
		}		

		if (nSubItem == 3) { 
			memcpy ( strTemp, m_StrAquiringIDFld, sizeof ( strTemp));
			lvItem.pszText = strTemp;                   
		}		

		if (nSubItem == 4) { 
			memcpy ( strTemp, m_StrFloorLimitFld, sizeof ( strTemp));
			lvItem.pszText = strTemp;                   
		}		
		
						
		if (nSubItem == 0) 
			nActualItem = m_CntlList.InsertItem(&lvItem);
		else 
			m_CntlList.SetItem(&lvItem);

		m_CntlList.SetFocus() ;	
	}

		if ( m_nDialogMode == MODE_EDIT )
		{
			memcpy ((LPSTR)(sMerchantCardRecord.settlement_id), m_StrSettleMerchantIDFld , sizeof (sMerchantCardRecord.settlement_id));
			memcpy ((LPSTR)(sMerchantCardRecord.corporate_id), m_StrCorporationIDFld , sizeof (sMerchantCardRecord.corporate_id));
			memcpy ((LPSTR)(sMerchantCardRecord.acquirer_id), m_StrAquiringIDFld , sizeof (sMerchantCardRecord.acquirer_id));
			memcpy ((LPSTR)(sMerchantCardRecord.floor_limit), m_StrFloorLimitFld , sizeof (sMerchantCardRecord.floor_limit));

			BYTE retcode = txdsapi_insert_record ( (pBYTE)&sMerchantCardRecord, sizeof (MCT01), MCT01_DATA, strErrorMessage );
			if ( retcode != PTEMSG_OK )
			{
				m_StrMessageLbl = strErrorMessage;
				UpdateData(FALSE);
				return ;
			}

			memcpy ((LPSTR)m_strCurrentData, "Merchant Card Type Added", sizeof (m_strCurrentData));
			memset ((LPSTR)m_strPreviousData, 0, sizeof (m_strPreviousData));
			memcpy ((LPSTR)m_strFieldTag, m_StrCardTypeFld, sizeof (m_strFieldTag));

			PopulateHistoryTable();

			m_StrMessageLbl = "Record Added";
			UpdateData(FALSE);

		}	
}

void CMerchantSettlement::OnDeleteButton() 
{
   char strErrorMessage[200] = "";

   UpdateData (TRUE);
   m_StrMessageLbl = "";
   UpdateData(FALSE);


   nSelectedIndex = m_CntlList.GetNextItem (-1, LVNI_SELECTED) ;

   m_StrCardTypeFld = m_CntlList.GetItemText (nSelectedIndex, 0) ;	

   memcpy (sMerchantCardRecord.primary_key.card_type,       m_StrCardTypeFld, sizeof (sMerchantCardRecord.primary_key.card_type));
   memcpy (sMerchantCardRecord.primary_key.organization_id, m_StrOrganizationIDFld, sizeof (sMerchantCardRecord.primary_key.organization_id));
   memcpy (sMerchantCardRecord.primary_key.merchant_id,     m_StrMerchantIDFld, sizeof (sMerchantCardRecord.primary_key.merchant_id));

   int rcode = txdsapi_delete_record ((pBYTE)&sMerchantCardRecord, sizeof ( MCT01), MCT01_DATA, strErrorMessage) ;
   if ( rcode != PTEMSG_OK )
   {
      switch( rcode )
      {
         case PTEMSG_NOT_FOUND:
            sprintf( strErrorMessage, "ERROR: Card type record for %s does not exist", m_StrCardTypeFld );
         break;
      }

      m_StrMessageLbl = strErrorMessage;
      if (!UpdateData(FALSE))
      {
         AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);
      }
   }
   else
   {
      memcpy ((LPSTR)m_strCurrentData, "Merchant Card Type  Deleted", sizeof (m_strCurrentData));
      memset ((LPSTR)m_strPreviousData, 0, sizeof (m_strPreviousData));
      memcpy ((LPSTR)m_strFieldTag, m_StrCardTypeFld, sizeof (m_strFieldTag));

      PopulateHistoryTable();
      m_CntlList.DeleteItem(nSelectedIndex);

      m_StrMessageLbl = "Record Deleted";
      UpdateData(FALSE);
   }
   return;
}



void CMerchantSettlement::OnUpdateButton() 
{
	MCT01 sMerchantCardRecord;
	char strErrorMessage[200] = "";

	UpdateData(TRUE);
	m_StrMessageLbl = "";
	UpdateData(FALSE);

	if ( m_nDialogMode != MODE_ADD)
	{
	ZeroMemory ( &sMerchantCardRecord, sizeof ( MCT01));

	memcpy (sMerchantCardRecord.primary_key.card_type,	     m_StrCardTypeFld, sizeof (sMerchantCardRecord.primary_key.card_type));
	memcpy (sMerchantCardRecord.primary_key.organization_id, m_StrOrganizationIDFld, sizeof (sMerchantCardRecord.primary_key.organization_id));
	memcpy (sMerchantCardRecord.primary_key.merchant_id,     m_StrMerchantIDFld, sizeof (sMerchantCardRecord.primary_key.merchant_id));

	memcpy (sMerchantCardRecord.acquirer_id,	     m_StrAquiringIDFld,       sizeof (sMerchantCardRecord.acquirer_id));
	memcpy (sMerchantCardRecord.corporate_id,	     m_StrCorporationIDFld,    sizeof (sMerchantCardRecord.corporate_id));
	memcpy (sMerchantCardRecord.settlement_id,	     m_StrSettleMerchantIDFld, sizeof (sMerchantCardRecord.settlement_id));
	memcpy (sMerchantCardRecord.floor_limit,	     m_StrFloorLimitFld,       sizeof (sMerchantCardRecord.floor_limit));

	
	BYTE rcode = txdsapi_update_record((pBYTE)&sMerchantCardRecord, sizeof (MCT01), MCT01_DATA, strErrorMessage );
	if ( rcode != PTEMSG_OK )
	{
		m_StrMessageLbl = strErrorMessage;
		UpdateData(FALSE);
		return;
	}

	if ( strcmp (m_StrAquiringIDFld,m_CntlList.GetItemText (nSelectedIndex, 3)) != 0)
	{
		memcpy(m_strCurrentData, m_StrAquiringIDFld, sizeof (m_strCurrentData));
		memcpy(m_strPreviousData, m_CntlList.GetItemText (nSelectedIndex, 3), sizeof (m_strPreviousData));
		memcpy(m_strFieldTag, "Aquiring ID", sizeof (m_strFieldTag));
			
		PopulateHistoryTable();
	}

	if ( strcmp (m_StrCorporationIDFld,m_CntlList.GetItemText (nSelectedIndex, 2)) != 0)
	{
		memcpy(m_strCurrentData, m_StrCorporationIDFld, sizeof (m_strCurrentData));
		memcpy(m_strPreviousData, m_CntlList.GetItemText (nSelectedIndex, 3), sizeof (m_strPreviousData));
		memcpy(m_strFieldTag, "Corporation ID", sizeof (m_strFieldTag));
			
		PopulateHistoryTable();
	}

	if ( strcmp (m_StrSettleMerchantIDFld,m_CntlList.GetItemText (nSelectedIndex, 1)) != 0)
	{
		memcpy(m_strCurrentData, m_StrSettleMerchantIDFld, sizeof (m_strCurrentData));
		memcpy(m_strPreviousData, m_CntlList.GetItemText (nSelectedIndex, 3), sizeof (m_strPreviousData));
		memcpy(m_strFieldTag, "Settle Merchant ID", sizeof (m_strFieldTag));
			
		PopulateHistoryTable();
	}

	if ( strcmp (m_StrFloorLimitFld,m_CntlList.GetItemText (nSelectedIndex, 4)) != 0)
	{
		memcpy(m_strCurrentData, m_StrFloorLimitFld, sizeof (m_strCurrentData));
		memcpy(m_strPreviousData, m_CntlList.GetItemText (nSelectedIndex, 3), sizeof (m_strPreviousData));
		memcpy(m_strFieldTag, "Floor Limit", sizeof (m_strFieldTag));
			
		PopulateHistoryTable();
	}
	}
//	m_StrCorporationIDFld = m_CntlList.GetItemText (nSelectedIndex, 2) ;
//	m_StrFloorLimitFld = m_CntlList.GetItemText (nSelectedIndex, 4) ;
//	m_StrSettleMerchantIDFld = m_CntlList.GetItemText (nSelectedIndex, 1) ;
//	m_StrCardTypeFld = m_CntlList.GetItemText (nSelectedIndex, 0) ;
	int nCount = m_CntlList.GetItemCount();
	CString temp;

	for ( int i = 0; i < nCount; i++ )
	{
		temp = m_CntlList.GetItemText (i, 0) ;
		if ( strcmp ( temp, m_StrCardTypeFld ) == 0)
			nSelectedIndex = i;
	}

	m_CntlList.SetItemText ( nSelectedIndex, 1, m_StrSettleMerchantIDFld);
	m_CntlList.SetItemText ( nSelectedIndex, 3, m_StrAquiringIDFld);
	m_CntlList.SetItemText ( nSelectedIndex, 2, m_StrCorporationIDFld);
	m_CntlList.SetItemText ( nSelectedIndex, 4, m_StrFloorLimitFld);

	m_StrMessageLbl = "Record Updated";
	UpdateData(FALSE);

	
}

void CMerchantSettlement::OnDblclkList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	nSelectedIndex = m_CntlList.GetNextItem (-1, LVNI_SELECTED) ;

	m_StrAquiringIDFld = m_CntlList.GetItemText (nSelectedIndex, 3) ;
	m_StrCorporationIDFld = m_CntlList.GetItemText (nSelectedIndex, 2) ;
	m_StrFloorLimitFld = m_CntlList.GetItemText (nSelectedIndex, 4) ;
	m_StrSettleMerchantIDFld = m_CntlList.GetItemText (nSelectedIndex, 1) ;
	m_StrCardTypeFld = m_CntlList.GetItemText (nSelectedIndex, 0) ;

	UpdateData(FALSE);	
	
	*pResult = 0;
}

void CMerchantSettlement::OnClickList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	if ( m_nDialogMode != MODE_VIEW )
	{
		m_UpdateButton.EnableWindow(TRUE);
		m_DeleteButton.EnableWindow(TRUE);
	}
	*pResult = 0;
}

BOOL CMerchantSettlement::PopulateDatabaseFromSettlementDialog ()
{
    int nCount;
	char strErrorMessage[200];
	MCT01 sMerchantCardRecord;
	CString temp1, temp2, temp3, temp4, temp5;

	nCount = m_CntlList.GetItemCount();

	for ( int i = 0; i < nCount; i++ )
	{
		temp1 = m_CntlList.GetItemText (i, 3) ;
		temp2 = m_CntlList.GetItemText (i, 2) ;
		temp3 = m_CntlList.GetItemText (i, 4) ;
		temp4 = m_CntlList.GetItemText (i, 1) ;
		temp5 = m_CntlList.GetItemText (i, 0) ;

		UpdateData(TRUE);

		ZeroMemory ( &sMerchantCardRecord, sizeof ( MCT01));

		memcpy (sMerchantCardRecord.primary_key.card_type,	     temp5, sizeof (sMerchantCardRecord.primary_key.card_type));
		memcpy (sMerchantCardRecord.primary_key.organization_id, m_StrOrganizationIDFld, sizeof (sMerchantCardRecord.primary_key.organization_id));
		memcpy (sMerchantCardRecord.primary_key.merchant_id,     m_StrMerchantIDFld, sizeof (sMerchantCardRecord.primary_key.merchant_id));

		BYTE rcode = txdsapi_get_record ( (pBYTE)&sMerchantCardRecord, sizeof (MCT01), MCT01_DATA, strErrorMessage );
		if ( rcode == PTEMSG_OK )
		{
			m_StrMessageLbl = "ERROR: Card Type already exists";
			UpdateData(FALSE);
			return FALSE;
		}

		memcpy (sMerchantCardRecord.acquirer_id,	     temp1,     sizeof (sMerchantCardRecord.acquirer_id));
		memcpy (sMerchantCardRecord.corporate_id,	     temp2,		sizeof (sMerchantCardRecord.corporate_id));
		memcpy (sMerchantCardRecord.settlement_id,	     temp4,		sizeof (sMerchantCardRecord.settlement_id));
		memcpy (sMerchantCardRecord.floor_limit,	     temp3,     sizeof (sMerchantCardRecord.floor_limit));

		BYTE retcode = txdsapi_insert_record ( (pBYTE)&sMerchantCardRecord, sizeof (MCT01), MCT01_DATA, strErrorMessage );
		if ( retcode != PTEMSG_OK )
		{
			m_StrMessageLbl = strErrorMessage;
			UpdateData(FALSE);
			return FALSE;
		}


	}
	return TRUE;

}


void CMerchantSettlement::OnSetfocusCardType() 
{
	m_UpdateButton.EnableWindow(TRUE);
	
}

BOOL CMerchantSettlement::OnSetActive() 
{
	if ( m_nDialogMode == MODE_VIEW )
		m_NextButton.EnableWindow(TRUE);
	
	return CPropertyPage::OnSetActive();
}

BOOL CMerchantSettlement::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	// TODO: Add your specialized code here and/or call the base class
	
	c_time = CTime::GetCurrentTime();	
	shrmem.SetLastUpdateTime(c_time);
	return CPropertyPage::OnCommand(wParam, lParam);
}
