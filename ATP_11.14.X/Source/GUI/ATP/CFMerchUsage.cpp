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
* $Log:   N:\PVCS\PTE\Equitable\ATP_gui\CFMerchUsage.cpp  $
   
      Rev 1.5   Dec 01 2003 11:29:06   lmitchel
   Added AMEX Online Updates - scr1200
   
      Rev 1.4   Jan 05 2000 14:52:58   gbarnes
   Major update. Not updated to PVCS recently.
   PVCS was locked.
   
      Rev 1.1   Sep 22 1999 15:42:58   iarustam
   Added manual batch entry GUI
   
      Rev 1.0   Jul 01 1999 14:16:54   iarustam
   Initial Release
   
      Rev 1.0   Jun 28 1999 13:57:58   iarustam
   Initial Release
   
      Rev 1.1   Apr 29 1999 14:55:52   iarustam
   Bug fixes
   
      Rev 1.0   Feb 23 1999 11:40:00   iarustam
   Initial Release
   
      Rev 1.0   Feb 17 1999 10:09:04   iarustam
   Initial Release
   
      Rev 1.1   Jan 22 1999 10:57:46   iarustam
   Bug fixes and core 1_3_0 changes
   
      Rev 1.1   Jan 21 1999 13:06:46   iarustam
   Bug fixes
   
   
      Rev 1.1   Dec 18 1998 13:42:26   iarustam
    
*
************************************************************************************/
// CFMerchUsage.cpp : implementation file
//

#include "stdafx.h"
#include "atp.h"
#include "CFMerchUsage.h"
#include "SharedMem.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern 	CCF03	sCCF03Record;


extern CSharedMem shrmem;

/////////////////////////////////////////////////////////////////////////////
// CCFMerchUsage property page

IMPLEMENT_DYNCREATE(CCFMerchUsage, CPropertyPage)

CCFMerchUsage::CCFMerchUsage() : CPropertyPage(CCFMerchUsage::IDD)
{
	//{{AFX_DATA_INIT(CCFMerchUsage)
	m_strCardNbr = _T("");
	m_strCardType = _T("");
	//}}AFX_DATA_INIT
}

CCFMerchUsage::~CCFMerchUsage()
{
}

void CCFMerchUsage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCFMerchUsage)
	DDX_Control(pDX, IDC_LISTREPEAT, m_ctrlRepeatList);
	DDX_Control(pDX, IDC_LISTMCC, m_ctrlMCCList);
	DDX_Control(pDX, IDC_COMMENTS, m_ComEdit);
	DDX_Text(pDX, IDC_CARDNBR, m_strCardNbr);
	DDX_Text(pDX, IDC_CARDTYPE, m_strCardType);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CCFMerchUsage, CPropertyPage)
	//{{AFX_MSG_MAP(CCFMerchUsage)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCFMerchUsage message handlers

BOOL CCFMerchUsage::OnInitDialog() 
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

	char strHeaderData1[2][50] = {"Category Code            ","Number of Uses  "} ;

	for (int nHeader1 = 0; nHeader1 < 2; nHeader1++)
	{ 
		lvColumn.mask = LVCF_FMT | LVCF_SUBITEM | LVCF_TEXT  ;
		lvColumn.fmt = LVCFMT_LEFT ;
		lvColumn.pszText = strHeaderData1[nHeader1] ;
		lvColumn.iSubItem = nHeader1 ;
		
		m_ctrlMCCList.InsertColumn(nHeader1, &lvColumn) ;  // assumes return value is OK.
		m_ctrlMCCList.SetColumnWidth (0, LVSCW_AUTOSIZE_USEHEADER) ;
		m_ctrlMCCList.SetColumnWidth (1, LVSCW_AUTOSIZE_USEHEADER) ;
   	}

	char strHeaderData2[2][50] = {"Merchant Id             ","Number of Uses  "} ;

	for (int nHeader2 = 0; nHeader2 < 2; nHeader2++)
	{ 
		lvColumn.mask = LVCF_FMT | LVCF_SUBITEM | LVCF_TEXT  ;
		lvColumn.fmt = LVCFMT_LEFT ;
		lvColumn.pszText = strHeaderData2[nHeader2] ;
		lvColumn.iSubItem = nHeader2 ;
		
		m_ctrlRepeatList.InsertColumn(nHeader2, &lvColumn) ;  // assumes return value is OK.
		m_ctrlRepeatList.SetColumnWidth (0, LVSCW_AUTOSIZE_USEHEADER) ;
		m_ctrlRepeatList.SetColumnWidth (1, LVSCW_AUTOSIZE_USEHEADER) ;
   	}

    if (DialogMode != MODE_ADD && DialogMode != MODE_DONE && m_bCCF03)  
	{
		PopulateMerchUsDialog();	
	}
    
	m_bMerchUsage = TRUE;
	
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CCFMerchUsage::OnSetActive() 
{
	m_ComEdit.SetWindowText("");

	if (DialogMode == MODE_DONE)
	{
		m_ctrlMCCList.DeleteAllItems();
		m_ctrlRepeatList.DeleteAllItems();
	}
	
	return CPropertyPage::OnSetActive();
}

void CCFMerchUsage::PopulateMerchUsDialog()
{
	LV_ITEM lvItem1,lvItem2;
    
    int nRowItem1 = 0;
    int nActualItem1 = 0;

    int nRowItem2 = 0;
    int nActualItem2 = 0;

	m_ctrlMCCList.DeleteAllItems();
   
    for (int nItem1 = 0; nItem1 <  20; nItem1++,nRowItem1 ++)
	{ // will now insert the items and subitems into the list view.
		for (int nSubItem1 = 0; nSubItem1 < 2; nSubItem1++) 
		{					
			
			lvItem1.mask = LVIF_TEXT ;
			lvItem1.iItem = (nSubItem1 == 0)? nRowItem1 : nActualItem1;
			lvItem1.iSubItem = nSubItem1;
				
			if (nSubItem1 == 0)
			{ 
				lvItem1.pszText = (char *)sCCF03Record.mcc[nItem1].category_code;				
			}
			else if (nSubItem1 == 1)
			{ 
				lvItem1.pszText = (char *)sCCF03Record.mcc[nItem1].usage ;								
			} // end else if
					
			if (nSubItem1 == 0) 
				nActualItem1 = m_ctrlMCCList.InsertItem(&lvItem1);
			else 
				m_ctrlMCCList.SetItem(&lvItem1);
		}
	
	} // for 			

	m_ctrlRepeatList.DeleteAllItems();
   
    for (int nItem2 = 0; nItem2 <  20; nItem2++,nRowItem2 ++) 
	{ // will now insert the items and subitems into the list view.
		for (int nSubItem2 = 0; nSubItem2 < 2; nSubItem2++)
		{					
			lvItem2.mask = LVIF_TEXT ;
			lvItem2.iItem = (nSubItem2 == 0)? nRowItem2 : nActualItem2;
			lvItem2.iSubItem = nSubItem2;
				
			if (nSubItem2 == 0) 
			{ 
				lvItem2.pszText = (char *)sCCF03Record.repeat[nItem2].merchant_id;				
			}
			else if (nSubItem2 == 1) 
			{ 
				lvItem2.pszText = (char *)sCCF03Record.repeat[nItem2].nbr_uses ;								
			} // end else if
					
			if (nSubItem2 == 0) 
				nActualItem2 = m_ctrlRepeatList.InsertItem(&lvItem2);
			else 
				m_ctrlRepeatList.SetItem(&lvItem2);
		}
		
	} // for 			

}



BOOL CCFMerchUsage::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	// TODO: Add your specialized code here and/or call the base class
	c_time = CTime::GetCurrentTime();	
	shrmem.SetLastUpdateTime(c_time);
	return CPropertyPage::OnCommand(wParam, lParam);
}
