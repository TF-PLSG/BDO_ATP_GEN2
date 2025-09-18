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
* $Log:   N:\PVCS\PTE\Equitable\ATP_gui\CFTxHistory.cpp  $
   
      Rev 1.6   Dec 01 2003 11:29:08   lmitchel
   Added AMEX Online Updates - scr1200
   
      Rev 1.5   Jan 05 2000 14:53:00   gbarnes
   Major update. Not updated to PVCS recently.
   PVCS was locked.
   
      Rev 1.1   Sep 22 1999 15:43:16   iarustam
   Added manual batch entry GUI
   
      Rev 1.0   Jul 01 1999 14:16:56   iarustam
   Initial Release
   
      Rev 1.0   Jun 28 1999 13:58:00   iarustam
   Initial Release
   
      Rev 1.2   Apr 29 1999 14:55:54   iarustam
   Bug fixes
   
      Rev 1.1   Mar 08 1999 17:20:00   iarustam
   Bug fixes
   
   
      Rev 1.0   Feb 23 1999 11:40:02   iarustam
   Initial Release
   
   
      Rev 1.1   Dec 18 1998 13:42:28   iarustam
    
*
************************************************************************************/
// CFTxHistory.cpp : implementation file
//

#include "stdafx.h"
#include "atp.h"
#include "CFTxHistory.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#include "SharedMem.h"

extern CSharedMem shrmem;
/////////////////////////////////////////////////////////////////////////////
// CCFTxHistory property page

IMPLEMENT_DYNCREATE(CCFTxHistory, CPropertyPage)

CCFTxHistory::CCFTxHistory() : CPropertyPage(CCFTxHistory::IDD)
{
	//{{AFX_DATA_INIT(CCFTxHistory)
	m_strCardNbr = _T("");
	m_strCardType = _T("");
	//}}AFX_DATA_INIT
}

CCFTxHistory::~CCFTxHistory()
{
}

void CCFTxHistory::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCFTxHistory)
	DDX_Control(pDX, IDC_VIEW, m_View);
	DDX_Control(pDX, IDC_NEXT, m_Next);
	DDX_Control(pDX, IDC_TXLIST, m_TxList);
	DDX_Control(pDX, IDC_COMMENTS, m_ComEdit);
	DDX_Text(pDX, IDC_CARDNBR, m_strCardNbr);
	DDX_Text(pDX, IDC_CARDTYPE, m_strCardType);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CCFTxHistory, CPropertyPage)
	//{{AFX_MSG_MAP(CCFTxHistory)
	ON_BN_CLICKED(IDC_NEXT, OnNext)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCFTxHistory message handlers

BOOL CCFTxHistory::OnInitDialog() 
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
	GetDlgItem(IDC_COMMENTS)->SetFont (&m_font);

	char strHeaderData[6][50] = {"Type    ","Amount   ","Response Code", "Auth. Number","Date   ","Time   "} ;

	for (int nHeader = 0; nHeader < 6; nHeader++)
	{ 
		lvColumn.mask = LVCF_FMT | LVCF_SUBITEM | LVCF_TEXT  ;
		lvColumn.fmt = LVCFMT_LEFT ;
		lvColumn.pszText = strHeaderData[nHeader] ;
		lvColumn.iSubItem = nHeader ;
		
		m_TxList.InsertColumn(nHeader, &lvColumn) ;  // assumes return value is OK.
		m_TxList.SetColumnWidth (0, LVSCW_AUTOSIZE_USEHEADER) ;
		m_TxList.SetColumnWidth (1, LVSCW_AUTOSIZE_USEHEADER) ;
		m_TxList.SetColumnWidth (2, LVSCW_AUTOSIZE_USEHEADER) ;
		m_TxList.SetColumnWidth (3, LVSCW_AUTOSIZE_USEHEADER) ;
		m_TxList.SetColumnWidth (4, LVSCW_AUTOSIZE_USEHEADER) ;
		m_TxList.SetColumnWidth (5, LVSCW_AUTOSIZE_USEHEADER) ;
   	}

    if (DialogMode != MODE_ADD && DialogMode != MODE_DONE)    
		PopulateTxDialog();	
	else 
    
	m_bTxPage = TRUE;
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CCFTxHistory::OnNext() 
{
    int nRowItem = 0;
    int nActualItem = 0;
	bNext = TRUE;

	if (!bDone) 
		PopulateTxDialog();
	else 
	{
        m_ComEdit.SetWindowText("ERROR: No data found");
		m_Next.EnableWindow(FALSE);
		UpdateData(FALSE);
	}
	
	
}

BOOL CCFTxHistory::OnSetActive() 
{
	m_ComEdit.SetWindowText("");

	if (DialogMode == MODE_DONE)
		m_TxList.DeleteAllItems();

	//UpdateData(FALSE);

	
	return CPropertyPage::OnSetActive();
}


void CCFTxHistory::PopulateTxDialog()
{
    CCF10_GUI_LIST sTxList;
	LV_ITEM lvItem;
	char strErrorMessage[200];
    
    int nRowItem = 0;
    int nActualItem = 0;
    int nNumberReturned;
    bDone = FALSE;

	m_TxList.DeleteAllItems();
	m_Next.EnableWindow(TRUE);
	if (!bNext)
		m_TxList.DeleteAllItems();

    ZeroMemory ( &sTxRecord, sizeof (CCF10));
	memset(&(sTxRecord.primary_key.retrieval_ref_num),32,sizeof(sTxRecord.primary_key.retrieval_ref_num)-1);
    memcpy ((LPTSTR) sTxRecord.primary_key.card_nbr,(LPCTSTR)m_strCardNbr, sizeof (sTxRecord.primary_key.card_nbr));

	int rcode = txdsapi_get_list ( (pBYTE)&sTxRecord, sizeof(CCF10),
									(pBYTE)&sTxList, sizeof(CCF10_GUI_LIST),
									ST2_DB_SELECT_CCF10_BY_CARD_NUM,
									CCF10_DATA,strErrorMessage);        
    
		if (rcode == PTEMSG_OK) { // Dataserver returned DBERR_OK
			nNumberReturned = atoi ((char *)&sTxList.num_returned);
            if( nNumberReturned == GUI_MAX_LIST_SIZE)
            {				
			   // CString strTemp = sTxList.ccf10_record->primary_key.retrieval_ref_num[nNumberReturned-1] ;
				  CString strTemp = (CString)((char *)sTxList.ccf10_record->primary_key.retrieval_ref_num[nNumberReturned-1]) ; 
				
			    strTemp.TrimRight() ; strTemp.TrimLeft() ;
			    memcpy ( &sTxRecord, &sTxList.ccf10_record[nNumberReturned-1], sizeof (CCF10));

            }
            else bDone = TRUE;
        
     
            for (int nItem = 0; nItem <  nNumberReturned; nItem++,nRowItem ++) { // will now insert the items and subitems into the list view.
				m_TxList.EnableWindow(TRUE);  
			    for (int nSubItem = 0; nSubItem < 6; nSubItem++) {					
					
			    lvItem.mask = LVIF_TEXT ;
				lvItem.iItem = (nSubItem == 0)? nRowItem : nActualItem;
				lvItem.iSubItem = nSubItem;
					
		       if (nSubItem == 0) { 
					lvItem.pszText = (char *)sTxList.ccf10_record[nItem].processing_code ;								
				}
				else if (nSubItem == 1) { 
					lvItem.pszText = (char *)sTxList.ccf10_record[nItem].tran_amount;
				}
				else if (nSubItem == 2) { 
					lvItem.pszText = (char *)sTxList.ccf10_record[nItem].response_code;
				} 
                else if (nSubItem == 3) { 
					lvItem.pszText = (char *)sTxList.ccf10_record[nItem].auth_number;
				} 
                else if (nSubItem == 4) { 
					lvItem.pszText = (char *)sTxList.ccf10_record[nItem].tran_date;
				} 
                else if (nSubItem == 5) { 
					lvItem.pszText = (char *)sTxList.ccf10_record[nItem].tran_time;
				} // end else if
						
				if (nSubItem == 0) 
					nActualItem = m_TxList.InsertItem(&lvItem);
				else 
					m_TxList.SetItem(&lvItem);
						
				m_TxList.SetFocus() ;
						
		    	} // for
	    	} // for 			
		}
		else
		{
			m_ComEdit.SetWindowText(strErrorMessage);
			UpdateData(FALSE);
		}    
}



BOOL CCFTxHistory::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	// TODO: Add your specialized code here and/or call the base class
	
	c_time = CTime::GetCurrentTime();	
	shrmem.SetLastUpdateTime(c_time);

	return CPropertyPage::OnCommand(wParam, lParam);
}
