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
* $Log:   N:\PVCS\PTE\Equitable\ATP_gui\MerchantTerminalBatchHistory.cpp  $
   
      Rev 1.8   Dec 01 2003 11:29:40   lmitchel
   Added AMEX Online Updates - scr1200
   
      Rev 1.7   Jan 25 2000 14:00:32   iarustam
   bug fixes
   
      Rev 1.6   Jan 10 2000 10:55:14   mbysani
   changed the debit_refund_amt field
   
      Rev 1.3   Oct 15 1999 17:14:40   iarustam
   bug fixes
   
      Rev 1.2   Sep 30 1999 16:17:26   npogosya
   Bug fixes
   
      Rev 1.1   Sep 22 1999 15:53:54   iarustam
   Added manual batch entry GUI
   
      Rev 1.0   Jul 01 1999 14:17:48   iarustam
   Initial Release
   

*
************************************************************************************/
// MerchantTerminalBatchHistory.cpp : implementation file
//

#include "stdafx.h"
#include "Atp.h"
#include "MerchantTerminalBatchHistory.h"
#include "BinList.h"
#include "MyUtils.h"
#include "SharedMem.h"
#include <string.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CSharedMem shrmem;
BCH01_GUI_LIST sBatchListglobal;
BCH01_GUI_LIST sBatchListglobal1[20];

BCH01 sBatchRecordglobal[300];

int flagb = 0;
int total_NumberReturned ;
int NR;

int d, m ;
long int y ;
 int list =-1;
 int start_index ;
int  final_index ;

/////////////////////////////////////////////////////////////////////////////
// CMerchantTerminalBatchHistory property page

IMPLEMENT_DYNCREATE(CMerchantTerminalBatchHistory, CPropertyPage)

CMerchantTerminalBatchHistory::CMerchantTerminalBatchHistory() : CPropertyPage(CMerchantTerminalBatchHistory::IDD)
{
	//{{AFX_DATA_INIT(CMerchantTerminalBatchHistory)
	m_StrOrganizationIDFld = _T("");
	m_StrMerchantIDFld = _T("");
	m_StrMessageLbl = _T("");
	m_StrTerminalIDFld = _T("");
	//}}AFX_DATA_INIT
}

CMerchantTerminalBatchHistory::~CMerchantTerminalBatchHistory()
{
}

void CMerchantTerminalBatchHistory::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMerchantTerminalBatchHistory)
	DDX_Control(pDX, IDC_NEXT_BUTTON, m_CntlNextButton);
	DDX_Control(pDX, IDC_LIST, m_CntlList);
	DDX_Control(pDX, IDC_VIEW_BUTTON, m_CntlViewButton);
	DDX_Control(pDX, IDC_SORT_BUTTON1, m_CntlSortButton);
	DDX_Text(pDX, IDC_CHAIN_ID_FLD, m_StrOrganizationIDFld);
	DDX_Text(pDX, IDC_MERCHANT_ID_FLD, m_StrMerchantIDFld);
	DDX_Text(pDX, IDC_MESSAGE_LBL, m_StrMessageLbl);
	DDX_Text(pDX, IDC_TERMINAL_ID_FLD, m_StrTerminalIDFld);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CMerchantTerminalBatchHistory, CPropertyPage)
	//{{AFX_MSG_MAP(CMerchantTerminalBatchHistory)
	ON_BN_CLICKED(IDC_VIEW_BUTTON, OnViewButton)
	ON_BN_CLICKED(IDC_NEXT_BUTTON, OnNextButton)
	ON_BN_CLICKED(IDC_SORT_BUTTON1, OnBnClickedSortButton1)
	//}}AFX_MSG_MAP

END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMerchantTerminalBatchHistory message handlers

BOOL CMerchantTerminalBatchHistory::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
	LV_COLUMN lvColumn ;
	CFont m_font;
	LOGFONT lf;

	memset ( &lf, 0, sizeof (lf));
	lf.lfHeight = 12;
	lf.lfWeight = 700;
	strcpy(lf.lfFaceName, "Arial");
	m_font.CreateFontIndirect (&lf);
	GetDlgItem(IDC_MESSAGE_LBL)->SetFont (&m_font);
	bNext = FALSE;
	
	m_bBatchHistory = TRUE;
    
    char strHeaderData[14][50] = {"BATCH NUM", "OPEN DATE", "OPEN TIME","CREDIT CNT", "CREDIT AMT", "CREDIT REF CNT", "CREDIT REF AMT",
								  "DEBIT CNT", "DEBIT AMT", "DEBIT REF CNT", "DEBIT REF AMT", 
								  "RELEASE DATE", "RELEASE TIME", "STATUS"} ;

    for (int nHeader = 0; nHeader < 14; nHeader++) { 
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
      m_CntlList.SetColumnWidth (6, LVSCW_AUTOSIZE_USEHEADER) ;
      m_CntlList.SetColumnWidth (7, LVSCW_AUTOSIZE_USEHEADER) ;
      m_CntlList.SetColumnWidth (8, LVSCW_AUTOSIZE_USEHEADER) ;
      m_CntlList.SetColumnWidth (9, LVSCW_AUTOSIZE_USEHEADER) ;
      m_CntlList.SetColumnWidth (10, LVSCW_AUTOSIZE_USEHEADER) ;
      m_CntlList.SetColumnWidth (11, LVSCW_AUTOSIZE_USEHEADER) ;
      m_CntlList.SetColumnWidth (12, LVSCW_AUTOSIZE_USEHEADER) ;
      m_CntlList.SetColumnWidth (13, LVSCW_AUTOSIZE_USEHEADER) ;
    }

    if (m_nDialogMode != MODE_ADD && m_nDialogMode != MODE_DONE)	
		PopulateBatchHistoryDialog(sTerminalRecord);
	else 
	{
		m_CntlNextButton.EnableWindow(FALSE);
		m_CntlViewButton.EnableWindow(FALSE);
		m_CntlSortButton.EnableWindow(FALSE);
		m_CntlList.EnableWindow(FALSE);  
	}
   
	UpdateData(FALSE);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CMerchantTerminalBatchHistory::OnViewButton() 
{
	CBinList binListdlg;
	CString strBatchNum, strOpenDate, strOpenTime, strStatus;
	int nRowSelected = -1;
   LVITEM sLvitem;
	nRowSelected = m_CntlList.GetNextItem (-1, LVNI_SELECTED) ;

	if( nRowSelected == -1)
	{
		m_StrMessageLbl = "Please select an item";
		UpdateData(FALSE);	
		return;
	}
	else	
   {
		  strBatchNum = m_CntlList.GetItemText (nRowSelected, 0) ;
        strOpenDate = m_CntlList.GetItemText (nRowSelected, 1) ;
        strOpenTime = m_CntlList.GetItemText (nRowSelected, 2) ;
        strOpenDate = strOpenDate.Right(4) + strOpenDate.Mid(3,2) + strOpenDate.Left(2);
        strOpenTime = strOpenTime.Left(2) + strOpenTime.Mid(3,2) + strOpenTime.Right(2);
        strStatus = m_CntlList.GetItemText (nRowSelected, 13) ;

   }

	strcpy((char *)binListdlg.sBatchRecord.primary_key.device_id, m_StrTerminalIDFld);
	strcpy((char *)binListdlg.sBatchRecord.primary_key.batch_nbr, strBatchNum);
	strcpy((char *)binListdlg.sBatchRecord.merchant_id, m_StrMerchantIDFld);
	strcpy((char *)binListdlg.sBatchRecord.open_date, strOpenDate);
	strcpy((char *)binListdlg.sBatchRecord.open_time, strOpenTime);
	strcpy((char *)binListdlg.m_strBatchStatus, strStatus);
	binListdlg.m_nDialogMode = MODE_BATCH_HISTORY;

	binListdlg.DoModal();

     //TEST added
   UpdateData(TRUE);
   m_StrMessageLbl = ""; 

   sLvitem.stateMask = (UINT)-1;

   //nRowSelected = m_CntlList.GetNextItem (1, LVIS_FOCUSED ) ;
   //m_CntlList.SetCurSel(-1);	
   UpdateData(FALSE);

   //End_TEST
	
}

void CMerchantTerminalBatchHistory::PopulateBatchHistoryDialog(DCF01 sTerminalRecord)
{
	BCH01_GUI_LIST sBatchList;
	MCF01 sMerchantRecord;
	LV_ITEM lvItem;
	char strErrorMessage[200];
	CString temp;
//	int res, len;
	char strSum[50];
    CMyUtils myUtilsObject;
    
    int nRowItem = 0;
    int nActualItem = 0;
    int nNumberReturned;
    bDone = FALSE;
    int nSize, nCount;
	int MGIDlen = 0;
    nCount = m_CntlList.GetItemCount();

	m_CntlNextButton.EnableWindow(TRUE);
	m_CntlSortButton.EnableWindow(FALSE);

	
	if (!bNext)
		CleanBatchHistoryDialog();

	m_StrMerchantIDFld = sTerminalRecord.merchant_id;
	m_StrOrganizationIDFld = sTerminalRecord.organization_id;
	m_StrTerminalIDFld = sTerminalRecord.primary_key.device_id;
	memcpy( sBatchRecord.merchant_id, m_StrMerchantIDFld, sizeof (sBatchRecord.merchant_id)); 
	
	memcpy ((LPSTR)(sMerchantRecord.primary_key.merchant_id), m_StrMerchantIDFld , sizeof (sMerchantRecord.primary_key.merchant_id));
    memcpy ((LPSTR)(sMerchantRecord.primary_key.organization_id), m_StrOrganizationIDFld , sizeof (sMerchantRecord.primary_key.organization_id));  

	int rcode = txdsapi_get_record( (pBYTE)&sMerchantRecord, sizeof (MCF01), MCF01_DATA, strErrorMessage );
	if(rcode == PTEMSG_OK)
	{
		//Check if its A MP merchant
		MGIDlen = strlen((pCHAR)sMerchantRecord.network[5].priority_routing);
		if(MGIDlen == 1)
		{
			if(0 == strncmp((pCHAR)sMerchantRecord.network[5].priority_routing , " ", 1))
			{
				MGIDlen = 0;
			}	
		}
		
		if(MGIDlen > 1)
		{
			//its MP merchant
			rcode = txdsapi_get_list ( (pBYTE)&sBatchRecord, sizeof (BCH01_MP),
									(pBYTE) &sBatchList, sizeof (BCH01_MP_GUI_LIST), 0,
									BCH01_MP_DATA, strErrorMessage);  
			m_StrMessageLbl = "Its MP Merchant";
		}
		else
		{
			
			rcode = txdsapi_get_list ( (pBYTE)&sBatchRecord, sizeof (BCH01),
									(pBYTE) &sBatchList, sizeof (BCH01_GUI_LIST), 0,
									BCH01_DATA, strErrorMessage);

			list++;

		//	memcpy((char *)&sBatchListglobal1[list], (char *)&sBatchList, sizeof(sBatchList));
			
			NR =  atoi ((char *)&sBatchList.num_returned);
			start_index = total_NumberReturned  ;
			total_NumberReturned =total_NumberReturned + NR -1;
			final_index = total_NumberReturned ;
			
			int b;

			if(flagb ==0)
			{
				for(b=0; b<=NR;b++)
				{
					memcpy((char *)&sBatchRecordglobal[b],(char *)&sBatchList.batch_record[b],sizeof(sBatchRecord));
				}
				flagb = 1;
			}
			else
			{
				int j=0;
				for(int b1=start_index,j=0; b1<=final_index; b1++,j++)
				{
					memcpy((char *)&sBatchRecordglobal[b1],(char *)&sBatchList.batch_record[j],sizeof(sBatchRecord));
				}
			}

			
		}
		if (rcode == PTEMSG_OK) { // Dataserver returned DBERR_OK
			nNumberReturned = atoi ((char *)&sBatchList.num_returned);
            if( nNumberReturned == GUI_MAX_LIST_SIZE)
            {				
			  //  CString strTemp = sBatchList.batch_record->primary_key.batch_nbr[nNumberReturned-1] ;
				 CString strTemp =(CString)((char *) sBatchList.batch_record->primary_key.batch_nbr[nNumberReturned-1] );
			    strTemp.TrimRight() ; strTemp.TrimLeft() ;
			    memcpy ( &sBatchRecord, &sBatchList.batch_record[nNumberReturned-1], sizeof (BCH01));
			    nSize = nNumberReturned - 1;
            }
            else 
            {
               bDone = TRUE;
			      nSize = nNumberReturned;
            }

			 if (nNumberReturned == 0)
			{
				// There are no MP / Retail batch details for this batch .
				 // Let us check if we have to look for private lable
		 
				 if(MGIDlen > 1)
				 {
					 // Do nothing
				 }
				 else
				 {
					// This might br a Private lable merchant terminal combination
					rcode = txdsapi_get_list ( (pBYTE)&sBatchRecord, sizeof (BCH01_MP),
								(pBYTE) &sBatchList, sizeof (BCH01_MP_GUI_LIST), 0,
								BCH01_MP_DATA, strErrorMessage);  

					if (rcode == PTEMSG_OK) 
					{ // Dataserver returned DBERR_OK
						nNumberReturned = atoi ((char *)&sBatchList.num_returned);
						if( nNumberReturned == GUI_MAX_LIST_SIZE)
						{				
							//CString strTemp = sBatchList.batch_record->primary_key.batch_nbr[nNumberReturned-1] ;
							CString strTemp = (CString)((char *)sBatchList.batch_record->primary_key.batch_nbr[nNumberReturned-1]);
							strTemp.TrimRight() ; strTemp.TrimLeft() ;
							memcpy ( &sBatchRecord, &sBatchList.batch_record[nNumberReturned-1], sizeof (BCH01));
							nSize = nNumberReturned - 1;
						}
						else 
						{
						   bDone = TRUE;
							  nSize = nNumberReturned;
						}
						if(nNumberReturned > 0)
						{
							m_StrMessageLbl = "Its PL Merchant";
						}
					}
				}
			 }
        
     
            for (int nItem = 0; nItem <  nSize; nItem++,nRowItem ++) 
			{
				// will now insert the items and subitems into the list view.
				    m_CntlList.EnableWindow(TRUE);  
			   for (int nSubItem = 0; nSubItem < 14; nSubItem++) 
			   {					
				    	
			       lvItem.mask = LVIF_TEXT ;
				    lvItem.iItem = (nSubItem == 0)? nRowItem : nActualItem;
				    lvItem.iSubItem = nSubItem;
					
				if (nSubItem == 0) { 
					lvItem.pszText = (char *)sBatchList.batch_record[nItem].primary_key.batch_nbr ;                    
				}
				if (nSubItem == 1) { 
					 temp = (char *)sBatchList.batch_record[nItem].open_date ; 
                if ( temp.GetLength() != 0 )
                  temp = temp.Right(2) + "/" + temp.Mid(4,2) + "/" + temp.Left(4);
                memcpy ( strSum, temp, sizeof (strSum));
                lvItem.pszText = strSum;
				}
				if (nSubItem == 2) { 
					 temp = (char *)sBatchList.batch_record[nItem].open_time ;
                if ( temp.GetLength() != 0 )
                  temp = temp.Left(2) + ":" + temp.Mid(2,2) + ":" + temp.Right(2);
                memcpy ( strSum, temp, sizeof (strSum));
                lvItem.pszText = strSum;
				}
				if (nSubItem == 3) { 
					lvItem.pszText = (char *)sBatchList.batch_record[nItem].total_credit_cnt ;                    
				}
				if (nSubItem == 4) { 
					temp = sBatchList.batch_record[nItem].total_credit_amt ;
               temp = myUtilsObject.DisplayAmount(temp);
					memcpy ( strSum, temp, sizeof (strSum));

					lvItem.pszText = strSum ;                    
				}
				if (nSubItem == 5) { 
					lvItem.pszText = (char *)sBatchList.batch_record[nItem].total_credit_refund_cnt ;                    
				}
				if (nSubItem == 6) { 
					temp = sBatchList.batch_record[nItem].total_credit_refund_amt ;
               temp = myUtilsObject.DisplayAmount(temp);
					memcpy ( strSum, temp, sizeof (strSum));

					lvItem.pszText = strSum ;                    
				}
				if (nSubItem == 7) { 
					lvItem.pszText = (char *)sBatchList.batch_record[nItem].total_debit_cnt ;                    
				}
				if (nSubItem == 8) { 
					temp = sBatchList.batch_record[nItem].total_debit_amt ;
               temp = myUtilsObject.DisplayAmount(temp);
					memcpy ( strSum, temp, sizeof (strSum));

					lvItem.pszText = strSum ;                    
				}
				if (nSubItem == 9) { 
					lvItem.pszText = (char *)sBatchList.batch_record[nItem].total_debit_refund_cnt ;                    
				}
				if (nSubItem == 10) { 
					temp = sBatchList.batch_record[nItem].total_debit_refund_amt ;
               temp = myUtilsObject.DisplayAmount(temp);
					memcpy ( strSum, temp, sizeof (strSum));

					lvItem.pszText = strSum ;                    
				}
				if (nSubItem == 11) { 
					 temp = (char *)sBatchList.batch_record[nItem].release_date ;   
                if ( temp.GetLength() != 0 )
                  temp = temp.Right(2) + "/" + temp.Mid(4,2) + "/" + temp.Left(4);
                memcpy ( strSum, temp, sizeof (strSum));
                lvItem.pszText = strSum;
				}
				if (nSubItem == 12) { 
					 temp = (char *)sBatchList.batch_record[nItem].release_time ;
                if ( temp.GetLength() != 0 )
                  temp = temp.Left(2) + ":" + temp.Mid(2,2) + ":" + temp.Right(2);
                memcpy ( strSum, temp, sizeof (strSum));
                lvItem.pszText = strSum;
				}
				if (nSubItem == 13) { 
					lvItem.pszText = (char *)sBatchList.batch_record[nItem].batch_status ;                    
				}
			
						
				if (nSubItem == 0) 
					nActualItem = m_CntlList.InsertItem(nCount+nSize,(char *)sBatchList.batch_record[nItem].primary_key.batch_nbr);
				else 
					m_CntlList.SetItem(&lvItem);
						
			/*	if (nSubItem == 1) // transaction id
					m_CntlList.SetColumnWidth (nSubItem, 0) ;
						
				else 
					m_CntlList.SetColumnWidth (nSubItem, LVSCW_AUTOSIZE_USEHEADER) ;				
                    */
				m_CntlList.SetFocus() ;
						
		    	} // for
	    	} // for 			
		}
	}

		else
		{
			m_StrMessageLbl = strErrorMessage;
			UpdateData(FALSE);
		}  
      
				
		nCount = m_CntlList.GetItemCount();

		m_CntlList.EnsureVisible( nCount-1, TRUE);

}

void CMerchantTerminalBatchHistory::OnNextButton() 
{
	int nRowItem = 0;
    int nActualItem = 0;
	bNext = TRUE;

	if (!bDone) 
		PopulateBatchHistoryDialog(sTerminalRecord);
	else 
	{
	    m_StrMessageLbl = "End of list";
		m_CntlNextButton.EnableWindow(FALSE);
		m_CntlSortButton.EnableWindow(TRUE);
		UpdateData(FALSE);
	}    	
	
}

void CMerchantTerminalBatchHistory::CleanBatchHistoryDialog()
{
	m_StrOrganizationIDFld = _T("");
	m_StrMerchantIDFld = _T("");
	m_StrMessageLbl = _T("");
	m_StrTerminalIDFld = _T("");
	if ( m_bBatchHistory )
	{
		m_CntlList.DeleteAllItems();	
		m_CntlList.EnableWindow(FALSE);
		UpdateData(FALSE); 
	}
}

BOOL CMerchantTerminalBatchHistory::OnSetActive() 
{
	if (m_CntlList.GetItemCount() == 0) 
		{	
           m_CntlNextButton.EnableWindow(FALSE);
		   m_CntlViewButton.EnableWindow(FALSE);
		  // m_CntlSortButton.EnableWindow(FALSE);
		   m_CntlList.EnableWindow(FALSE);
		}
	else
		{	
           m_CntlNextButton.EnableWindow(TRUE);
		   m_CntlViewButton.EnableWindow(TRUE);
		  //m_CntlSortButton.EnableWindow(TRUE);
		   m_CntlList.EnableWindow(TRUE);
		}

	
	
	return CPropertyPage::OnSetActive();
}

BOOL CMerchantTerminalBatchHistory::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	// TODO: Add your specialized code here and/or call the base class
	
	c_time = CTime::GetCurrentTime();	
	shrmem.SetLastUpdateTime(c_time);
	return CPropertyPage::OnCommand(wParam, lParam);
}


void CMerchantTerminalBatchHistory::OnBnClickedSortButton1()
{
	// TODO: Add your control notification handler code here

	int d, m ;
	long int y ;

	int nRowItem = 0;
    int nActualItem = 0;
	int nCount = 0;
	char strSum[50];
	int nNumberReturned,nSize;
	LV_ITEM lvItem;
	CString temp1;
	CMyUtils myUtilsObject;
	nCount = m_CntlList.GetItemCount();

	char strErrorMessage[200] = {0};
	int rcode = 0;

			char dates[5][12], temp[12] ;
			long int arr[25], t ;
			int i1, j, valid ;

			

		if ( m_bBatchHistory )
		{
			m_CntlList.EnsureVisible( 0, FALSE);
			m_StrMessageLbl = _T("");
			m_CntlList.DeleteAllItems();	
			m_CntlList.EnableWindow(FALSE);
			m_CntlSortButton.EnableWindow(FALSE);
			UpdateData(FALSE); 
		}

	

	
		for ( i1 = 0 ; i1 < total_NumberReturned ; i1++ )
			{
				for ( j = 0 ; j <= total_NumberReturned - 1 ; j++ )

				{
					int j_day1=0;
					int j_day2=0;
					char first_date[10] = {0}, second_date[10] = {0}; 
					strncpy(first_date,(char *)sBatchRecordglobal[j].open_date,8);
					strncpy(second_date,(char *)sBatchRecordglobal[j+1].open_date,8);
					j_day1=Txutils_Calculate_Julian_Date((unsigned char*)first_date);
					j_day2=Txutils_Calculate_Julian_Date((unsigned char*)second_date);
						
					if (j_day1 < j_day2 )
					{
						BCH01 temp_bch01={0};
						memcpy ( &temp_bch01, &sBatchRecordglobal[j] ,sizeof(BCH01)) ;
						memcpy (&sBatchRecordglobal[j], &sBatchRecordglobal[j+1],sizeof(BCH01)) ;
						memcpy (&sBatchRecordglobal[j+1], &temp_bch01 ,sizeof(BCH01)) ;
					}
				}
			}

				
			
			/********************************/
      int i2;


			for (int i = 0; i <= total_NumberReturned; i++,nRowItem ++) 
			{
				// will now insert the items and subitems into the list view.
				    m_CntlList.EnableWindow(TRUE);  
			   for (int nSubItem = 0; nSubItem < 14; nSubItem++) 
			   {					
				    	
			       lvItem.mask = LVIF_TEXT ;
				    lvItem.iItem = (nSubItem == 0)? nRowItem : nActualItem;
				    lvItem.iSubItem = nSubItem;
					
				if (nSubItem == 0) { 
					lvItem.pszText = (char *)sBatchRecordglobal[i].primary_key.batch_nbr ;                    
				}
				if (nSubItem == 1) { 
					 temp1 = (char *)sBatchRecordglobal[i].open_date ; 
                if ( temp1.GetLength() != 0 )
                  temp1 = temp1.Right(2) + "/" + temp1.Mid(4,2) + "/" + temp1.Left(4);
                memcpy ( strSum, temp1, sizeof (strSum));
                lvItem.pszText = strSum;
				}
				if (nSubItem == 2) { 
					 temp1 = (char *)sBatchRecordglobal[i].open_time ;
                if ( temp1.GetLength() != 0 )
                  temp1 = temp1.Left(2) + ":" + temp1.Mid(2,2) + ":" + temp1.Right(2);
                memcpy ( strSum, temp1, sizeof (strSum));
                lvItem.pszText = strSum;
				}
				if (nSubItem == 3) { 
					lvItem.pszText = (char *)sBatchRecordglobal[i].total_credit_cnt ;                    
				}
				if (nSubItem == 4) { 
					temp1 = sBatchRecordglobal[i].total_credit_amt ;
               temp1 = myUtilsObject.DisplayAmount(temp1);
					memcpy ( strSum, temp1, sizeof (strSum));

					lvItem.pszText = strSum ;                    
				}
				if (nSubItem == 5) { 
					lvItem.pszText = (char *)sBatchRecordglobal[i].total_credit_refund_cnt ;                    
				}
				if (nSubItem == 6) { 
					temp1 = sBatchRecordglobal[i].total_credit_refund_amt ;
               temp1 = myUtilsObject.DisplayAmount(temp1);
					memcpy ( strSum, temp1, sizeof (strSum));

					lvItem.pszText = strSum ;                    
				}
				if (nSubItem == 7) { 
					lvItem.pszText = (char *)sBatchRecordglobal[i].total_debit_cnt ;                    
				}
				if (nSubItem == 8) { 
					temp1 = sBatchRecordglobal[i].total_debit_amt ;
               temp1 = myUtilsObject.DisplayAmount(temp1);
					memcpy ( strSum, temp1, sizeof (strSum));

					lvItem.pszText = strSum ;                    
				}
				if (nSubItem == 9) { 
					lvItem.pszText = (char *)sBatchRecordglobal[i].total_debit_refund_cnt ;                    
				}
				if (nSubItem == 10) { 
					temp1 = sBatchRecordglobal[i].total_debit_refund_amt ;
               temp1 = myUtilsObject.DisplayAmount(temp1);
					memcpy ( strSum, temp1, sizeof (strSum));

					lvItem.pszText = strSum ;                    
				}
				if (nSubItem == 11) { 
					 temp1 = (char *)sBatchRecordglobal[i].release_date ;   
                if ( temp1.GetLength() != 0 )
                  temp1 = temp1.Right(2) + "/" + temp1.Mid(4,2) + "/" + temp1.Left(4);
                memcpy ( strSum, temp1, sizeof (strSum));
                lvItem.pszText = strSum;
				}
				if (nSubItem == 12) { 
					 temp1 = (char *)sBatchRecordglobal[i].release_time ;
                if ( temp1.GetLength() != 0 )
                  temp1 = temp1.Left(2) + ":" + temp1.Mid(2,2) + ":" + temp1.Right(2);
                memcpy ( strSum, temp1, sizeof (strSum));
                lvItem.pszText = strSum;
				}
				if (nSubItem == 13) { 
					lvItem.pszText = (char *)sBatchRecordglobal[i].batch_status ;                    
				}
			
						
				if (nSubItem == 0) 
					nActualItem = m_CntlList.InsertItem(nCount+nSize,(char *)sBatchRecordglobal[i].primary_key.batch_nbr);
				else 
					m_CntlList.SetItem(&lvItem);
						
			/*	if (nSubItem == 1) // transaction id
					m_CntlList.SetColumnWidth (nSubItem, 0) ;
						
				else 
					m_CntlList.SetColumnWidth (nSubItem, LVSCW_AUTOSIZE_USEHEADER) ;				
                    */
				m_CntlList.SetFocus() ;
						//m_CntlList.GetOrigin(0);
		    	} // for
	    	} // for 			

		total_NumberReturned = 0;
		nCount = m_CntlList.GetItemCount();
		m_CntlList.EnsureVisible( nCount-1, TRUE);


	
}
	
		








