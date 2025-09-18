

/*
Rev        April 19.5.11          Krishna Kiran S@TF
	 Added Reseting Idle time, whenever any event is fired in GUI
*/

// AlphaLookup.cpp : implementation file
//

#include "stdafx.h"
#include "atp.h"
#include "AlphaLookup.h"
#include "SharedMem.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
extern CSharedMem shrmem;
/////////////////////////////////////////////////////////////////////////////
// CAlphaLookup dialog


CAlphaLookup::CAlphaLookup(CWnd* pParent /*=NULL*/)
	: CDialog(CAlphaLookup::IDD, pParent)
{
	//{{AFX_DATA_INIT(CAlphaLookup)
	m_StrNameFld = _T("");
	m_StrMessageLbl = _T("");
	//}}AFX_DATA_INIT
}


void CAlphaLookup::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAlphaLookup)
	DDX_Control(pDX, IDC_NEXT, m_Next);
	DDX_Control(pDX, IDC_LIST, m_CntlList);
	DDX_Text(pDX, IDC_NAME_FLD, m_StrNameFld);
	DDV_MaxChars(pDX, m_StrNameFld, 30);
	DDX_Text(pDX, IDC_COMMENTS, m_StrMessageLbl);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CAlphaLookup, CDialog)
	//{{AFX_MSG_MAP(CAlphaLookup)
	ON_BN_CLICKED(IDC_INQUIRY, OnInquiry)
	ON_BN_CLICKED(IDC_NEXT, OnNext)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST, OnDblclkList)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAlphaLookup message handlers

BOOL CAlphaLookup::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	CFont m_font;
	LOGFONT lf;
   char strHeaderData[5][70];
	int nHeader;
	LV_COLUMN lvColumn ;

	
	UpdateData(TRUE);
	memset ( &lf, 0, sizeof (lf));
	lf.lfHeight = 12;
	lf.lfWeight = 700;
	strcpy(lf.lfFaceName, "Arial");
	m_font.CreateFontIndirect (&lf);
	GetDlgItem(IDC_COMMENTS)->SetFont (&m_font);

   m_StrNameFld.Empty();
   m_StrMessageLbl.Empty();

   ZeroMemory ( &sMerchantRecord, sizeof (MCF01));
   ZeroMemory ( &sPAFRecord, sizeof (PAF01));
   ZeroMemory ( &sCCFRecord, sizeof (CCF01));

   m_Next.EnableWindow(FALSE);

   switch ( m_nDialogMode )
   {

      case MODE_CCF :
   	   memcpy (strHeaderData[0],"               NAME               ", sizeof (strHeaderData[0]));
		   memcpy (strHeaderData[1],"               ADDRESS               ", sizeof (strHeaderData[1])) ;
		   memcpy (strHeaderData[2],"        ACCOUNT NUMBER       ", sizeof (strHeaderData[1])) ;
		   memcpy (strHeaderData[3]," TYPE ", sizeof (strHeaderData[1])) ;

		   for (nHeader = 0; nHeader < 4; nHeader++) { 
			   lvColumn.mask = LVCF_FMT | LVCF_SUBITEM | LVCF_TEXT  ;
			   lvColumn.fmt = LVCFMT_LEFT ;
			   lvColumn.pszText = strHeaderData[nHeader] ;
			   lvColumn.iSubItem = nHeader ;
			   
			   m_CntlList.InsertColumn(nHeader, &lvColumn) ;  // assumes return value is OK.
			   m_CntlList.SetColumnWidth (0, LVSCW_AUTOSIZE_USEHEADER) ;
			   m_CntlList.SetColumnWidth (1, LVSCW_AUTOSIZE_USEHEADER) ;
			   m_CntlList.SetColumnWidth (2, LVSCW_AUTOSIZE_USEHEADER) ;
			   m_CntlList.SetColumnWidth (3, LVSCW_AUTOSIZE_USEHEADER) ;
			   
		   }


		   

      break;

      case MODE_PRIMACC:
   
		   memcpy (strHeaderData[0],"                      NAME                    ", sizeof (strHeaderData[0]));
		   memcpy (strHeaderData[1],"                     ADDRESS                      ", sizeof (strHeaderData[1])) ;
		   memcpy (strHeaderData[2],"      ACCOUNT NUMBER       ", sizeof (strHeaderData[1])) ;

		   for (nHeader = 0; nHeader < 3; nHeader++) { 
			   lvColumn.mask = LVCF_FMT | LVCF_SUBITEM | LVCF_TEXT  ;
			   lvColumn.fmt = LVCFMT_LEFT ;
			   lvColumn.pszText = strHeaderData[nHeader] ;
			   lvColumn.iSubItem = nHeader ;
			   
			   m_CntlList.InsertColumn(nHeader, &lvColumn) ;  // assumes return value is OK.
			   m_CntlList.SetColumnWidth (0, LVSCW_AUTOSIZE_USEHEADER) ;
			   m_CntlList.SetColumnWidth (1, LVSCW_AUTOSIZE_USEHEADER) ;
			   m_CntlList.SetColumnWidth (2, LVSCW_AUTOSIZE_USEHEADER) ;
			   
		   }

		   
      break;

      case MODE_MERCHANT:
     
		   memcpy (strHeaderData[0],"                      NAME                      ", sizeof (strHeaderData[0]));
		   memcpy (strHeaderData[1],"                        ADDRESS                   ", sizeof (strHeaderData[1])) ;
		   memcpy (strHeaderData[2],"    MERCHANT ID    ", sizeof (strHeaderData[2])) ;
		   memcpy (strHeaderData[3],"ORG ID", sizeof (strHeaderData[3])) ;

		   for (nHeader = 0; nHeader < 4; nHeader++) { 
			   lvColumn.mask = LVCF_FMT | LVCF_SUBITEM | LVCF_TEXT  ;
			   lvColumn.fmt = LVCFMT_LEFT ;
			   lvColumn.pszText = strHeaderData[nHeader] ;
			   lvColumn.iSubItem = nHeader ;
			   
			   m_CntlList.InsertColumn(nHeader, &lvColumn) ;  // assumes return value is OK.
			   m_CntlList.SetColumnWidth (0, LVSCW_AUTOSIZE_USEHEADER) ;
			   m_CntlList.SetColumnWidth (1, LVSCW_AUTOSIZE_USEHEADER) ;
			   m_CntlList.SetColumnWidth (2, LVSCW_AUTOSIZE_USEHEADER) ;
			   m_CntlList.SetColumnWidth (3, LVSCW_AUTOSIZE_USEHEADER) ;
			   
		   }


      break;
   
   
   }
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CAlphaLookup::PopulateMCFListDialog()
{
	CFont m_font;
	LV_ITEM lvItem;
	char strErrorMessage[200];
   MCF01_REPORT_LIST sMerchantList;
    
   int nRowItem = 0;
   int nActualItem = 0;
   int nNumberReturned;
   int nSize, nCount, rcode;
   bDone = FALSE;
   char temp[250] = "";
       
   nCount = m_CntlList.GetItemCount();
   UpdateData(TRUE);
	m_StrMessageLbl = "Please Wait Processing The Request.........";
	UpdateData(FALSE);

   rcode = txdsapi_get_list ((pBYTE)&sMerchantRecord, sizeof (MCF01), 
									  (pBYTE)&sMerchantList, sizeof (MCF01_REPORT_LIST),
									  ST2_REPORT, MCF01_DATA, strErrorMessage);
   if(strErrorMessage != NULL)
	{
				m_StrMessageLbl = "Timed Out";
				UpdateData(FALSE); 
	}

    
	if (rcode == PTEMSG_OK)
	{
			nNumberReturned = atoi ((char *)&sMerchantList.num_returned);

         if( nNumberReturned == GUI_MAX_LIST_SIZE)
         {				
			    memcpy( sMerchantRecord.primary_key.merchant_id, 
                     (LPCTSTR) sMerchantList.merchant_info[nNumberReturned-1].merchant_id, 
                     sizeof (sMerchantRecord.primary_key.merchant_id)); 
				 memcpy( sMerchantRecord.primary_key.organization_id, 
                     (LPCTSTR) sMerchantList.merchant_info[nNumberReturned-1].organization_id, 
                     sizeof (sMerchantRecord.primary_key.organization_id)); 
				 memcpy( sMerchantRecord.name02, 
                     (LPCTSTR) sMerchantList.merchant_info[nNumberReturned-1].name02, 
                     sizeof (sMerchantRecord.name02)); 
            nSize = nNumberReturned - 1;			
            m_Next.EnableWindow(TRUE);
			m_StrMessageLbl = "Process Completed For The Request";
			UpdateData(FALSE);
			}
			else if ( 0 == nNumberReturned )
			{
				// Next 2 lines added by JMG on 5/2/02 to fix no records path
				bDone = TRUE;
				nSize = 0;
				m_StrMessageLbl = "No Records Found";
				UpdateData(FALSE);
			}
			else
         {
				
			bDone = TRUE;
            nSize = nNumberReturned;
			m_StrMessageLbl = "End Of List";
			UpdateData(FALSE);
            m_Next.EnableWindow(FALSE);
		}
						
			for (int nItem = 0; nItem <  nSize; nItem++,nRowItem ++) { // will now insert the items and subitems into the list view.
				m_CntlList.EnableWindow(TRUE);  

			   for (int nSubItem = 0; nSubItem < 4; nSubItem++) {					
					
			    lvItem.mask = LVIF_TEXT ;
				lvItem.iItem = (nSubItem == 0)? nRowItem : nActualItem;
				lvItem.iSubItem = nSubItem;

				if (nSubItem == 0) { 
               CString name1, name2, city;
               name1 = (char *)sMerchantList.merchant_info[nItem].name01;
               name2 = (char *)sMerchantList.merchant_info[nItem].name02;
               city = (char *)sMerchantList.merchant_info[nItem].city;
               name1.TrimRight();
               name2.TrimRight();
               memcpy ( temp, name2 + " " + name1 + " " + city, sizeof ( temp ) -1 );

               lvItem.pszText = (char *)temp;                   
				}
				else if (nSubItem == 1) { 
               CString address1, address2;
               address1 = (char *)sMerchantList.merchant_info[nItem].address01;  
               address2 = (char *)sMerchantList.merchant_info[nItem].address02;  
               address1.TrimRight();
               memcpy ( temp, address1 + " " + address2, sizeof ( temp ) -1 );
					lvItem.pszText = (char *)temp;                   
				}
				else if (nSubItem == 2) { 
					lvItem.pszText = (char *)sMerchantList.merchant_info[nItem].merchant_id;                   
				}
				else if (nSubItem == 3) { 
					lvItem.pszText = (char *)sMerchantList.merchant_info[nItem].organization_id;                   
				}                
						
				if (nSubItem == 0) 
            {
               CString name1, name2;
               name1 = (char *)sMerchantList.merchant_info[nItem].name01;
               name2 = (char *)sMerchantList.merchant_info[nItem].name02;
               name1.TrimRight();
               name1 = name2 + " " + name1;
               memcpy ( temp, name1, sizeof ( temp ) -1 );
              nActualItem = m_CntlList.InsertItem(nCount+nSize, (char *)temp);
            }
				//	nActualItem = m_CntlList.InsertItem(nCount+nSize, &lvItem);
				else 
					m_CntlList.SetItem(&lvItem);

				m_CntlList.SetFocus() ;

						
			} // for
		} // for 
		m_CntlList.SetFocus() ;
		int nCount = m_CntlList.GetItemCount();

		m_CntlList.EnsureVisible( nCount-1, TRUE);
		}


}

void CAlphaLookup::PopulatePAFListDialog()
{
	CFont m_font;
	LV_ITEM lvItem;
	char strErrorMessage[200];
   PAF01_REPORT_LIST sPAFList;
    
   int nRowItem = 0;
   int nActualItem = 0;
   int nNumberReturned;
   int nSize, nCount, rcode;
   bDone = FALSE;
   char temp[250] = "";
   
            
   nCount = m_CntlList.GetItemCount();
   UpdateData(TRUE);
   
	m_StrMessageLbl = "Please Wait Processing The Request.........";
	UpdateData(FALSE);
	
	

   rcode = txdsapi_get_list ((pBYTE)&sPAFRecord, sizeof (PAF01), 
									  (pBYTE)&sPAFList, sizeof (PAF01_REPORT_LIST),
									  ST2_REPORT, PAF01_DATA, strErrorMessage);
   if(strErrorMessage != NULL)
	{
		m_StrMessageLbl = "Timed Out";
		UpdateData(FALSE); 
	}
 
  
	if (rcode == PTEMSG_OK)
	{
			nNumberReturned = atoi ((char *)&sPAFList.num_returned);
			
			
         if( nNumberReturned == GUI_MAX_LIST_SIZE)
         {				
				 memcpy( sPAFRecord.primary_key.primary_acct_nbr, 
                     (LPCTSTR) sPAFList.paf_info[nNumberReturned-1].primary_acct_nbr, 
                     sizeof (sPAFRecord.primary_key.primary_acct_nbr)); 
				 memcpy( sPAFRecord.name_dba, 
                     (LPCTSTR) sPAFList.paf_info[nNumberReturned-1].name_dba, 
                     sizeof (sPAFRecord.name_dba)); 
            nSize = nNumberReturned - 1;
		    m_Next.EnableWindow(TRUE);
			m_StrMessageLbl = "Process Completed For The Request";
			UpdateData(FALSE);
			}
			else if ( 0 == nNumberReturned )
		{
			// Next 2 lines added by JMG on 5/2/02 to fix no records path
			bDone = TRUE;
			nSize = 0;
			m_StrMessageLbl = "No Records Found";
			UpdateData(FALSE);
		}
			else
         {				
			
				bDone = TRUE;
            nSize = nNumberReturned;
			m_StrMessageLbl = "End Of List";
			UpdateData(FALSE);
            m_Next.EnableWindow(FALSE);
						
         }
						
			for (int nItem = 0; nItem <  nSize; nItem++,nRowItem ++) { // will now insert the items and subitems into the list view.
				m_CntlList.EnableWindow(TRUE);  

			   for (int nSubItem = 0; nSubItem < 3; nSubItem++) {					
					
			   lvItem.mask = LVIF_TEXT ;
				lvItem.iItem = (nSubItem == 0)? nRowItem : nActualItem;
				lvItem.iSubItem = nSubItem;

				if (nSubItem == 0) { 
               lvItem.pszText = (char *)sPAFList.paf_info[nItem].name_dba;
				}
				else if (nSubItem == 1) { 
               CString address1, address2, city;
               address1 = (char *)sPAFList.paf_info[nItem].address1;  
               address2 = (char *)sPAFList.paf_info[nItem].address2;  
               city = (char *)sPAFList.paf_info[nItem].city;  
               address1.TrimRight();
               memcpy ( temp, address1 + " " + address2 + " " + city, sizeof ( temp ) -1 );
					lvItem.pszText = (char *)temp;                   
				}
				else if (nSubItem == 2) { 
					lvItem.pszText = (char *)sPAFList.paf_info[nItem].primary_acct_nbr;                   
				}
						
				if (nSubItem == 0) 
            {
              nActualItem = m_CntlList.InsertItem(nCount+nSize, (char *)sPAFList.paf_info[nItem].name_dba);
            }
				//	nActualItem = m_CntlList.InsertItem(nCount+nSize, &lvItem);
				else 
					m_CntlList.SetItem(&lvItem);

				m_CntlList.SetFocus() ;

						
			} // for
		} // for 
		m_CntlList.SetFocus() ;
		int nCount = m_CntlList.GetItemCount();

		m_CntlList.EnsureVisible( nCount-1, TRUE);
		}

}

void CAlphaLookup::PopulateCCFListDialog()
{
	CFont m_font;
	LV_ITEM lvItem;
	char strErrorMessage[200];
   CCF01_REPORT_LIST sCCFList;
    
   int nRowItem = 0;
   int nActualItem = 0;
   int nNumberReturned;
   int nSize, nCount, rcode;
   bDone = FALSE;
   char temp[250] = "";
   
  
       
   nCount = m_CntlList.GetItemCount();
   UpdateData(TRUE);
	m_StrMessageLbl = "Please Wait Processing The Request.........";
	UpdateData(FALSE);

   rcode = txdsapi_get_list ((pBYTE)&sCCFRecord, sizeof (CCF01), 
									  (pBYTE)&sCCFList, sizeof (CCF01_REPORT_LIST),
									  ST2_REPORT, CCF01_DATA, strErrorMessage);
   if(strErrorMessage != NULL)
	{
				m_StrMessageLbl = "Timed Out";
				UpdateData(FALSE); 
	}

    
	if (rcode == PTEMSG_OK)
	{
		    
			nNumberReturned = atoi ((char *)&sCCFList.num_returned);

		    					
			if( nNumberReturned == GUI_MAX_LIST_SIZE)
			{				
			    memcpy( sCCFRecord.primary_key.card_type, 
                     (LPCTSTR) sCCFList.ccf_info[nNumberReturned-1].card_type, 
                     sizeof (sCCFRecord.primary_key.card_type)); 
				 memcpy( sCCFRecord.primary_key.card_nbr, 
                     (LPCTSTR) sCCFList.ccf_info[nNumberReturned-1].card_nbr, 
                     sizeof (sCCFRecord.primary_key.card_nbr)); 
				 memcpy( sCCFRecord.name_dba, 
                     (LPCTSTR) sCCFList.ccf_info[nNumberReturned-1].name_dba, 
                     sizeof (sCCFRecord.name_dba)); 
				 nSize = nNumberReturned - 1;			
				 m_Next.EnableWindow(TRUE);
				 m_StrMessageLbl = "Process Completed For The Request";
				 UpdateData(FALSE);			
			}
			else if ( 0 == nNumberReturned )
			{
				// Next 2 lines added by JMG on 5/2/02 to fix no records path
				bDone = TRUE;
				nSize = 0;
				m_StrMessageLbl = "No Records Found";
				UpdateData(FALSE);
			}
			else
         {
		    bDone = TRUE;
            nSize = nNumberReturned;
			m_StrMessageLbl = "End Of List";
			UpdateData(FALSE);
            m_Next.EnableWindow(FALSE);		
					
         }
						
			for (int nItem = 0; nItem <  nSize; nItem++,nRowItem ++) { // will now insert the items and subitems into the list view.
				m_CntlList.EnableWindow(TRUE);  

			   for (int nSubItem = 0; nSubItem < 4; nSubItem++) {					
					
			   lvItem.mask = LVIF_TEXT ;
				lvItem.iItem = (nSubItem == 0)? nRowItem : nActualItem;
				lvItem.iSubItem = nSubItem;

				if (nSubItem == 0) { 
               lvItem.pszText = (char *)sCCFList.ccf_info[nItem].name_dba;
				}
				else if (nSubItem == 1) { 
               CString address1, address2, city;
               address1 = (char *)sCCFList.ccf_info[nItem].address1;  
               address2 = (char *)sCCFList.ccf_info[nItem].address2;  
               city = (char *)sCCFList.ccf_info[nItem].city;  
               address1.TrimRight();
               memcpy ( temp, address1 + " " + address2 + " " + city, sizeof ( temp ) -1 );
					lvItem.pszText = (char *)temp;                   
				}
				else if (nSubItem == 2) { 
					lvItem.pszText = (char *)sCCFList.ccf_info[nItem].card_nbr;                   
				}
				else if (nSubItem == 3) { 
					lvItem.pszText = (char *)sCCFList.ccf_info[nItem].card_type;                   
				}                
						
				if (nSubItem == 0) 
            {
              nActualItem = m_CntlList.InsertItem(nCount+nSize, (char *)sCCFList.ccf_info[nItem].name_dba);
            }
				//	nActualItem = m_CntlList.InsertItem(nCount+nSize, &lvItem);
				else 
					m_CntlList.SetItem(&lvItem);

				m_CntlList.SetFocus() ;

						
			} // for
		} // for 
		m_CntlList.SetFocus() ;
		int nCount = m_CntlList.GetItemCount();

		m_CntlList.EnsureVisible( nCount-1, TRUE);
		}
}

void CAlphaLookup::OnInquiry() 
{
   	 
   UpdateData(TRUE);
   

   m_CntlList.DeleteAllItems();

   if ( m_nDialogMode == MODE_MERCHANT )
   {
      memcpy ( sMerchantRecord.address01, m_StrNameFld, sizeof ( sMerchantRecord.address01 ) -1 );
      memcpy ( sMerchantRecord.primary_key.merchant_id, " ", sizeof ( sMerchantRecord.primary_key.merchant_id ) -1 );
      memcpy ( sMerchantRecord.primary_key.organization_id, " ", sizeof ( sMerchantRecord.primary_key.organization_id ) -1 );
      memcpy ( sMerchantRecord.name02,   " ", sizeof ( sMerchantRecord.name02) -1 );
	   PopulateMCFListDialog();
   }
   else if ( m_nDialogMode == MODE_CCF )
   {
	   
      memcpy ( sCCFRecord.address1, m_StrNameFld, sizeof ( sCCFRecord.address1 ) -1 );
      memcpy ( sCCFRecord.primary_key.card_nbr,  " ", sizeof ( sCCFRecord.primary_key.card_nbr )  -1 );
      memcpy ( sCCFRecord.primary_key.card_type, " ", sizeof ( sCCFRecord.primary_key.card_type ) -1 );
      memcpy ( sCCFRecord.name_dba,              " ", sizeof ( sCCFRecord.name_dba )-1 );

    
	   PopulateCCFListDialog();
       
   }
   else if ( m_nDialogMode == MODE_PRIMACC )
   {
	   
      memcpy ( sPAFRecord.address1, m_StrNameFld, sizeof ( sCCFRecord.address1 ) -1 );
      memcpy ( sPAFRecord.primary_key.primary_acct_nbr, " ", sizeof ( sPAFRecord.primary_key.primary_acct_nbr ) -1 );
      memcpy ( sPAFRecord.name_dba, " ", sizeof ( sPAFRecord.name_dba ) -1 );
	   PopulatePAFListDialog();

   }

	
}

void CAlphaLookup::OnNext() 
{
  
   UpdateData(TRUE);

	if (!bDone) 
   {
      if ( m_nDialogMode == MODE_MERCHANT )
      {
         memcpy ( sMerchantRecord.address01, m_StrNameFld, sizeof ( sMerchantRecord.address01 ) -1 );
         PopulateMCFListDialog();
      }
      else if ( m_nDialogMode == MODE_CCF )
      {
         memcpy ( sCCFRecord.address1, m_StrNameFld, sizeof ( sCCFRecord.address1 ) -1 );
         PopulateCCFListDialog();
      }
      else if ( m_nDialogMode == MODE_PRIMACC )
      {
         memcpy ( sPAFRecord.address1, m_StrNameFld, sizeof ( sCCFRecord.address1 ) -1 );
         PopulatePAFListDialog();
      }
   }		
	else 
	{
      m_StrMessageLbl = "End of list";
		m_Next.EnableWindow(FALSE);
		UpdateData(FALSE);
	}    		
}

void CAlphaLookup::OnOK() 
{
	int nRowSelected = m_CntlList.GetNextItem (-1, LVNI_SELECTED) ;
   strItem0.Empty();
   strItem1.Empty();

   UpdateData(TRUE);

	if( nRowSelected == -1 && m_nDialogMode != MODE_BATCH && m_nDialogMode != MODE_BATCH_HISTORY )
	{
		m_StrMessageLbl = "Please select an item";
		UpdateData(FALSE);	
		return;
	}
	else
	{
		if ( m_nDialogMode == MODE_MERCHANT )
		{
			strItem0 = m_CntlList.GetItemText (nRowSelected, 2) ;
			strItem1 = m_CntlList.GetItemText (nRowSelected, 3) ;
		}
		else if ( m_nDialogMode == MODE_CCF )
		{
			strItem0 = m_CntlList.GetItemText (nRowSelected, 2) ;
			strItem1 = m_CntlList.GetItemText (nRowSelected, 3) ;
		}
  		else if ( m_nDialogMode == MODE_PRIMACC )
		{
			strItem0 = m_CntlList.GetItemText (nRowSelected, 2) ;
		}

	
   }
	CDialog::OnOK();
}

void CAlphaLookup::OnDblclkList(NMHDR* pNMHDR, LRESULT* pResult) 
{
   OnOK() ;	

	*pResult = 0;
}

BOOL CAlphaLookup::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	// TODO: Add your specialized code here and/or call the base class
	c_time = CTime::GetCurrentTime();	
	shrmem.SetLastUpdateTime(c_time);
	return CDialog::OnCommand(wParam, lParam);
}
