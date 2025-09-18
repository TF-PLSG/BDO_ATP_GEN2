/*********************************************************************************
 Module: NegativeGroupInfo.cpp : implementation file

 Copyright (c) 1998, Hypercom, Inc.                         
 All Rights Reserved.                                       

 Description: Logic for Negative Card property page
			  This module defines and uses NegativeGroupInfo class

 Application: ATP

 Author: Irina Arustamova

 Revision History:
  
 $Log:   N:\PVCS\PTE\Equitable\ATP_gui\NegativeGroupInfo.cpp  $
   
      Rev 1.7   Dec 01 2003 11:29:52   lmitchel
   Added AMEX Online Updates - scr1200
   
      Rev 1.6   Jan 05 2000 14:54:08   gbarnes
   Major update. Not updated to PVCS recently.
   PVCS was locked.
   
      Rev 1.1   Sep 22 1999 15:56:18   iarustam
   Added manual batch entry GUI
   
      Rev 1.0   Jul 01 1999 14:17:58   iarustam
   Initial Release
   
      Rev 1.0   Jun 28 1999 13:59:06   iarustam
   Initial Release
   
      Rev 1.3   May 10 1999 11:30:36   dperson
   Standardized error messages
   
      Rev 1.2   Apr 29 1999 14:56:48   iarustam
   Bug fixes
   
      Rev 1.1   Mar 16 1999 16:30:14   iarustam
   Bug fixes
   
      Rev 1.0   Feb 23 1999 11:40:54   iarustam
   Initial Release
   
      Rev 1.0   Feb 17 1999 10:09:48   iarustam
   Initial Release
   
      Rev 1.1   Jan 22 1999 10:59:40   iarustam
   Bug fixes and core 1_3_0 changes
   
      Rev 1.1   Jan 21 1999 13:09:14   iarustam
   Bug fixes
   
   
      Rev 1.1   Dec 18 1998 13:44:16   iarustam
    

************************************************************************************/
// NegativeGroupInfo.cpp : implementation file
//

#include "stdafx.h"
#include "atp.h"
#include "NegativeGroupInfo.h"
#include "MyUtils.h"
#include "SharedMem.h"	
extern "C" {
#include "PTETIME.h"
}

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//#include "htmlhelp.h"

extern UCF01 ucf01;
extern CSharedMem shrmem;

/////////////////////////////////////////////////////////////////////////////
// CNegativeGroupInfo property page

IMPLEMENT_DYNCREATE(CNegativeGroupInfo, CPropertyPage)

CNegativeGroupInfo::CNegativeGroupInfo() : CPropertyPage(CNegativeGroupInfo::IDD)
{
	//{{AFX_DATA_INIT(CNegativeGroupInfo)
	m_StrMessageLbl = _T("");
	m_StrNegGroupIDFld = _T("");
	//}}AFX_DATA_INIT
}

CNegativeGroupInfo::~CNegativeGroupInfo()
{
}

void CNegativeGroupInfo::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CNegativeGroupInfo)
	DDX_Control(pDX, IDC_OLD_LIST, m_OldList);
	DDX_Control(pDX, IDC_NEXT_BUTTON, m_NextButton);
	DDX_Control(pDX, IDC_NEW_LIST, m_NewList);
	DDX_Control(pDX, IDC_DELETE_BUTTON, m_DeleteButton);
	DDX_Control(pDX, IDC_ADD_BUTTON, m_AddButton);
	DDX_Text(pDX, IDC_MESSAGE_LBL, m_StrMessageLbl);
	DDX_Text(pDX, IDC_NEG_GROUP_ID_FLD, m_StrNegGroupIDFld);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CNegativeGroupInfo, CPropertyPage)
	//{{AFX_MSG_MAP(CNegativeGroupInfo)
	ON_BN_CLICKED(IDC_ADD_BUTTON, OnAddButton)
	ON_BN_CLICKED(IDC_DELETE_BUTTON, OnDeleteButton)
	ON_BN_CLICKED(IDC_NEXT_BUTTON, OnNextButton)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CNegativeGroupInfo message handlers

BOOL CNegativeGroupInfo::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
	CFont m_font;
	LOGFONT lf;
	LV_COLUMN lvColumn ;


	memset ( &lf, 0, sizeof (lf));
	lf.lfHeight = 12;
	lf.lfWeight = 700;
	strcpy(lf.lfFaceName, "Arial");
	m_font.CreateFontIndirect (&lf);
	GetDlgItem(IDC_MESSAGE_LBL)->SetFont (&m_font);

    m_bGeneral = TRUE;

	char strHeaderData[2][50] = {"LOW             ", "HIGH            "} ;

    for (int nHeader = 0; nHeader < 2; nHeader++) { 
		lvColumn.mask = LVCF_FMT | LVCF_SUBITEM | LVCF_TEXT  ;
		lvColumn.fmt = LVCFMT_LEFT ;
		lvColumn.pszText = strHeaderData[nHeader] ;
		lvColumn.iSubItem = nHeader ;
		
		m_NewList.InsertColumn(nHeader, &lvColumn) ;  // assumes return value is OK.
		m_OldList.InsertColumn(nHeader, &lvColumn) ;  // assumes return value is OK.
        m_NewList.SetColumnWidth (0, LVSCW_AUTOSIZE_USEHEADER) ;
        m_OldList.SetColumnWidth (0, LVSCW_AUTOSIZE_USEHEADER) ;
        m_NewList.SetColumnWidth (1, LVSCW_AUTOSIZE_USEHEADER) ;
        m_OldList.SetColumnWidth (1, LVSCW_AUTOSIZE_USEHEADER) ;
    }


	if (m_nDialogMode != MODE_DONE)
	{
		ZeroMemory ( &sNegBinRecord, sizeof (NBF01));

		if ( m_nDialogMode != MODE_VIEW )
			EnableGeneralDialog(TRUE);
		if ( m_nDialogMode != MODE_ADD )
			PopulateGeneralDialogFromDatabase ( sNegGroupRecordOld );

		strcpy((char *)sNegBinRecord.primary_key.low, " ");
		strcpy((char *)sNegBinRecord.primary_key.high, " ");		
		PopulateNegBinListDialog();

	}  	
	
	if (!UpdateData(FALSE)){
		AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
	}
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CNegativeGroupInfo::PopulateNegBinListDialog()
{
CFont m_font;
	LV_ITEM lvItem;
	char strErrorMessage[200];
	BOOL bFound;
	CString strListItem1,strListItem2,strListItem3,strListItem4;
    
    int nRowItem = 0;
    int nActualItem = 0;
    int nNumberReturned;
    bDone = FALSE;
	int rcode = txdsapi_get_list ((pBYTE)&sNegBinRecord,sizeof(NBF01), (pBYTE)&sNegBinList, 
									sizeof (NBF01_GUI_LIST),0,NBF01_DATA,strErrorMessage);
	if (rcode == PTEMSG_OK)
	{

			nNumberReturned = atoi ((char *)&sNegBinList.num_returned);
            if( nNumberReturned == GUI_MAX_LIST_SIZE)
            {				
			    memcpy( sNegBinRecord.primary_key.low, (LPCTSTR) sNegBinList.nbf01_record[nNumberReturned-1].primary_key.low, sizeof (sNegBinRecord.primary_key.low)); 
				memcpy( sNegBinRecord.primary_key.high, (LPCTSTR) sNegBinList.nbf01_record[nNumberReturned-1].primary_key.high, sizeof (sNegBinRecord.primary_key.high)); 
			
			}
			else
				bDone = TRUE;
						
			for (int nItem = 0; nItem <  nNumberReturned; nItem++,nRowItem ++) { // will now insert the items and subitems into the list view.
				m_OldList.EnableWindow(TRUE);  
				bFound = FALSE;
			    for (int nSubItem = 0; nSubItem < 2; nSubItem++) {					
					
			    lvItem.mask = LVIF_TEXT ;
				lvItem.iItem = (nSubItem == 0)? nRowItem : nActualItem;
				lvItem.iSubItem = nSubItem;

				int nCount = m_NewList.GetItemCount();
				int nSelectedIndex = m_OldList.GetNextItem (-1, LVNI_SELECTED) ;

				strListItem3 = sNegBinList.nbf01_record[nItem].primary_key.low ;
				strListItem4 = sNegBinList.nbf01_record[nItem].primary_key.high ;

				for ( int i = 0; i < nCount; i++ )
				{
					strListItem1 = m_NewList.GetItemText (i, 0) ;
					strListItem2 = m_NewList.GetItemText (i, 1) ;
					if ( (strcmp ( strListItem1, strListItem3 ) == 0) && (strcmp ( strListItem2, strListItem4 ) == 0))
						bFound = TRUE;
				}

				if ( !bFound )
				{
					if (nSubItem == 0) { 
						lvItem.pszText = (char *)sNegBinList.nbf01_record[nItem].primary_key.low;                   
					}
					else if (nSubItem == 1) { 
						lvItem.pszText = (char *)sNegBinList.nbf01_record[nItem].primary_key.high;                   
					}                
							
					if (nSubItem == 0) 
						nActualItem = m_OldList.InsertItem(&lvItem);
					else 
						m_OldList.SetItem(&lvItem);
					

					m_OldList.SetFocus() ;
				}
						
			} // for
		} // for 
		//	if (m_CntlList.GetCount() > 0) m_CntlList.SetCurSel(0) ;
		}
}


void CNegativeGroupInfo::EnableGeneralDialog ( int nStatus )
{
	m_NewList.EnableWindow(nStatus);
	m_OldList.EnableWindow(nStatus);
	m_AddButton.EnableWindow(nStatus);
	m_DeleteButton.EnableWindow(nStatus);

	if ( m_nDialogMode != MODE_DONE )
		m_NextButton.EnableWindow(nStatus);

/*	if ( m_nDialogMode == MODE_VIEW )
	{
		m_AddButton.EnableWindow(FALSE);
		m_DeleteButton.EnableWindow(FALSE);
	}
*/

}

BOOL CNegativeGroupInfo::PopulateGeneralDialogFromDatabase ( NGF01 sNegGroupRecord )
{
	CString temp;
	CMyUtils myUtilsObject;
	int nRowItem = 0;
    int nActualItem = 0;
	CFont m_font;
	LV_ITEM lvItem;
    bDone = FALSE;

	if ( m_nDialogMode != MODE_VIEW && m_bGeneral )	
		EnableGeneralDialog(TRUE);

	m_StrNegGroupIDFld = sNegGroupRecord.primary_key.negative_group_id;  	
						
	for (int nItem = 0; nItem <  10; nItem++,nRowItem ++) { // will now insert the items and subitems into the list view.
		m_NewList.EnableWindow(TRUE);  
		for (int nSubItem = 0; nSubItem < 2; nSubItem++) {					
						
			lvItem.mask = LVIF_TEXT ;
			lvItem.iItem = (nSubItem == 0)? nRowItem : nActualItem;
			lvItem.iSubItem = nSubItem;

			CString temp = sNegGroupRecord.bin_range[nItem].low;
			if (!myUtilsObject.IsEmpty (temp, this))
			{
				if (nSubItem == 0) { 
					lvItem.pszText = (char *)sNegGroupRecord.bin_range[nItem].low;                   
				}
				else if (nSubItem == 1) { 
					lvItem.pszText = (char *)sNegGroupRecord.bin_range[nItem].high;                       
				}
							
                									
				if (nSubItem == 0) 
					nActualItem = m_NewList.InsertItem(&lvItem);
				else 
					m_NewList.SetItem(&lvItem);

					m_NewList.SetFocus() ;

			} //if

							
		} // for
	} // for 	
	
	UpdateData(FALSE);

	m_nOldCount = m_NewList.GetItemCount();
	if ( m_nOldCount == 10 )
		m_AddButton.EnableWindow(FALSE);

	memcpy ( &sNegGroupRecordOld, &sNegGroupRecord, sizeof ( NGF01));

	return TRUE;

}

void CNegativeGroupInfo::CleanGeneralDialog()
{
	m_StrMessageLbl = _T("");
	m_StrNegGroupIDFld = _T("");
	m_NewList.DeleteAllItems();
	m_OldList.DeleteAllItems();

	UpdateData (FALSE);
}

BOOL  CNegativeGroupInfo::PopulateDatabaseFromGeneralDialog ( pNGF01 psNegGroupRecord )
{ 
	int nCount;
	CString temp1, temp2;

	nCount = m_NewList.GetItemCount();

	for ( int i = 0; i <= nCount; i++ )
	{
		temp1 = m_NewList.GetItemText (i, 0) ;
		temp2 = m_NewList.GetItemText (i, 1) ;	

		UpdateData(TRUE);
	
		memcpy (psNegGroupRecord->bin_range[i].low,  temp1, sizeof (psNegGroupRecord->bin_range[i].low));
		memcpy (psNegGroupRecord->bin_range[i].high, temp2, sizeof (psNegGroupRecord->bin_range[i].high));
	
	}
	return TRUE;

}

BOOL CNegativeGroupInfo::CompareStructures()
{
	int nNewCount;
	BOOL bFound = FALSE;
	CString temp1, temp2, temp3, temp4;
	int i,j;
	CMyUtils myUtilsObject;
    char strDate[9] = "";


	nNewCount = m_NewList.GetItemCount();     
    
	for (  i = 0; i < m_nOldCount; i++ )
    { 
		bFound = FALSE;
		for (  j = 0; j < nNewCount; j++ )
		{
			temp1 = m_NewList.GetItemText (j, 0) ;
			temp2 = m_NewList.GetItemText (j, 1) ;
			
			int nResult = strcmp ((char *)sNegGroupRecordOld.bin_range[i].low,temp1);
			int nResult1 = strcmp ((char *)sNegGroupRecordOld.bin_range[i].high,temp2);
			if ( nResult == 0 && nResult1 == 0)
			{
				bFound = TRUE;
			}
		}
		if ( !bFound )
		{
			temp3 = sNegGroupRecordOld.bin_range[i].low;
			temp4 = sNegGroupRecordOld.bin_range[i].high;
			strcpy (m_strCurrentData, "BIN Range Deleted");
			strcpy (m_strPreviousData, temp3);
			strcat (m_strPreviousData, " - ");	
			strcat (m_strPreviousData, temp4);

			strcpy (m_strFieldTag, "BIN Range");

			PopulateHistoryTable();
		}
	}

	bFound = FALSE;
    
	for (  i = 0; i < nNewCount; i++ )
    { 
		bFound = FALSE;
		for ( int j = 0; j < m_nOldCount; j++ )
		{	
			temp1 = m_NewList.GetItemText (i, 0) ;
			temp2 = m_NewList.GetItemText (i, 1) ;
			
			int nResult = strcmp ((char *)sNegGroupRecordOld.bin_range[j].low,temp1);
			int nResult1 = strcmp ((char *)sNegGroupRecordOld.bin_range[j].high,temp2);
			if ( nResult == 0 && nResult1 == 0)
			{
				bFound = TRUE;
			}
		}
	
		if ( !bFound )
		{
			strcpy (m_strCurrentData, "BIN Range Added");
			strcpy (m_strPreviousData, temp1);
			strcat (m_strPreviousData, " - ");	
			strcat (m_strPreviousData, temp2);

			strcpy (m_strFieldTag, "BIN Range");

			PopulateHistoryTable();

		}
	}
	return TRUE;
}

void CNegativeGroupInfo::PopulateHistoryTable()
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

	strcpy ( strPrimaryKey, m_StrNegGroupIDFld);
    
    myUtilsObject.GetSystemDate(strDate);  

    myUtilsObject.GetSystemTime(strTime);
    pintime_get_gmt ( dest_str );
    ZeroMemory ( &sHistoryData, sizeof (HST01));
    memcpy ((LPTSTR)sHistoryData.file_name,				(LPCTSTR)"NGF01",			sizeof (sHistoryData.file_name));
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
		m_nDialogMode = MODE_DONE;
		UpdateData( FALSE );
	}
}


void CNegativeGroupInfo::OnAddButton() 
{
	char temp1[40] = "";
	char temp2[40] = "";
	int nRowItem = 0;
    int nActualItem = 0;
	LV_ITEM lvItem;
	CString strListItem1, strListItem2, strListItem3, strListItem4;
	BOOL bFound = FALSE;

	int nCount = m_NewList.GetItemCount();
	int nSelectedIndex = m_OldList.GetNextItem (-1, LVNI_SELECTED) ;

	if ( nSelectedIndex == -1 )
	{
		m_StrMessageLbl = "Please select an item to add";
		UpdateData(FALSE);
		return;
	}

	strListItem3 = m_OldList.GetItemText (nSelectedIndex, 0) ;
	strListItem4 = m_OldList.GetItemText (nSelectedIndex, 1) ;

	for ( int i = 0; i < nCount; i++ )
	{
		strListItem1 = m_NewList.GetItemText (i, 0) ;
		strListItem2 = m_NewList.GetItemText (i, 1) ;
		if ( (strcmp ( strListItem1, strListItem3 ) == 0) && (strcmp ( strListItem2, strListItem4 ) == 0))
			bFound = TRUE;
	}

	if ( bFound )
	{
		m_StrMessageLbl = "ERROR: Card Type already exists";		

		if (!UpdateData(FALSE)){
			AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
		}
		return ;
	}



	memcpy (temp1,m_OldList.GetItemText (nSelectedIndex, 0), sizeof (temp1)) ;
	memcpy (temp2,m_OldList.GetItemText (nSelectedIndex, 1), sizeof (temp2)) ;

	
	for (int nSubItem = 0; nSubItem < 2; nSubItem++) {					
		lvItem.mask = LVIF_TEXT ;
		lvItem.iItem = (nSubItem == 0)? nRowItem : nActualItem;
		lvItem.iSubItem = nSubItem;

		if (nSubItem == 0) {
			lvItem.pszText = temp1 ;
		}

		if (nSubItem == 1) { 
			lvItem.pszText = temp2;                   
		}			
							
		if (nSubItem == 0) 
			nActualItem = m_NewList.InsertItem(&lvItem);
		else 
			m_NewList.SetItem(&lvItem);

		m_NewList.SetFocus() ;	
	}
	m_StrMessageLbl = "";
	UpdateData(FALSE);

	nSelectedIndex = m_OldList.GetNextItem (-1, LVNI_SELECTED) ;
	m_OldList.DeleteItem(nSelectedIndex);

	int count = m_NewList.GetItemCount();
	if ( count == 10)
		m_AddButton.EnableWindow(FALSE);

	SetModified(TRUE);
	
}

void CNegativeGroupInfo::OnDeleteButton() 
{
	char temp1[40] = "";
	char temp2[40] = "";
	int nRowItem = 0;
    int nActualItem = 0;
	LV_ITEM lvItem;
	int nSelectedIndex = m_NewList.GetNextItem (-1, LVNI_SELECTED) ;

	if ( nSelectedIndex == -1 )
	{
		m_StrMessageLbl = "Please select an item to delete";
		UpdateData(FALSE);
		return;
	}

	memcpy (temp1,m_NewList.GetItemText (nSelectedIndex, 0), sizeof (temp1)) ;
	memcpy (temp2,m_NewList.GetItemText (nSelectedIndex, 1), sizeof (temp2)) ;

	
	for (int nSubItem = 0; nSubItem < 2; nSubItem++) {					
		lvItem.mask = LVIF_TEXT ;
		lvItem.iItem = (nSubItem == 0)? nRowItem : nActualItem;
		lvItem.iSubItem = nSubItem;

		if (nSubItem == 0) {
			lvItem.pszText = temp1 ;
		}

		if (nSubItem == 1) { 
			lvItem.pszText = temp2;                   
		}			
							
		if (nSubItem == 0) 
			nActualItem = m_OldList.InsertItem(&lvItem);
		else 
			m_OldList.SetItem(&lvItem);

		m_OldList.SetFocus() ;	
	}
	m_StrMessageLbl = "";
	UpdateData(FALSE);


	m_NewList.DeleteItem(nSelectedIndex);



	m_StrMessageLbl = "Record Deleted";

	if (!UpdateData(FALSE)){
		AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
	}

	m_AddButton.EnableWindow(TRUE);


	SetModified(TRUE);
	
}

void CNegativeGroupInfo::OnNextButton() 
{
	bNext = TRUE;

	if (!bDone) 
	{
		PopulateNegBinListDialog();
	}
	else 
	{
        m_StrMessageLbl = "ERROR: No data found";
		m_NextButton.EnableWindow(FALSE);
		UpdateData(FALSE);
	}
	
}

BOOL CNegativeGroupInfo::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	// TODO: Add your specialized code here and/or call the base class
	
	c_time = CTime::GetCurrentTime();	
	shrmem.SetLastUpdateTime(c_time);
	return CPropertyPage::OnCommand(wParam, lParam);
}
