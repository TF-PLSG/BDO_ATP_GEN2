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
* $Log:   N:\PVCS\PTE\Equitable\ATP_gui\ChangeHistory.cpp  $
   
      Rev 1.7   Dec 01 2003 11:29:08   lmitchel
   Added AMEX Online Updates - scr1200
   
      Rev 1.6   Jan 21 2000 12:22:54   iarustam
   Bug fixes # 14,24,,52,63,65,151,165,91,228,202,180,205,504,152,179,251,109,182,177,183,224,225,217,229,230,231,247,200,199,203,201,206,172
   
      Rev 1.5   Jan 05 2000 14:53:14   gbarnes
   Major update. Not updated to PVCS recently.
   PVCS was locked.
   
      Rev 1.2   Oct 15 1999 17:08:24   iarustam
   bug fixes
   
      Rev 1.1   Sep 22 1999 15:47:02   iarustam
   Added manual batch entry GUI
   
      Rev 1.0   Jul 01 1999 14:17:12   iarustam
   Initial Release
   
 
*
************************************************************************************/
// ChangeHistory.cpp : implementation file
//

#include "stdafx.h"
#include "Atp.h"
#include "ChangeHistory.h"
//#include "htmlhelp.h"

#include "GroupPropertySheet.h"
#include "UserPropertySheet.h"
#include "BinPropertySheet.h"
#include "CardTypePropertySheet.h"

#include "SharedMem.h"

extern CSharedMem shrmem;
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/*************************************/
static int btnWidth = 0;
static int btnHeight = 0;
static int var = 300;
CRect winRect1;
CRect winRect;
CRect rect;
int flag_user = 0;


static int btnWidth_group = 0;
static int btnHeight_group = 0;
static int var_group = 300;
CRect winRect1_group;
CRect winRect_group;
CRect rect_group;
int flag_group = 0;

/*************************************/
/////////////////////////////////////////////////////////////////////////////
// CChangeHistory property page

IMPLEMENT_DYNCREATE(CChangeHistory, CPropertyPage)

CChangeHistory::CChangeHistory() : CPropertyPage(CChangeHistory::IDD)
{
	//{{AFX_DATA_INIT(CChangeHistory)
	m_StrMessageLbl = _T("");
	//}}AFX_DATA_INIT
}

CChangeHistory::~CChangeHistory()
{
}

void CChangeHistory::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CChangeHistory)
	DDX_Control(pDX, IDC_NEXT_BUTTON, m_CntlNextButton);
	DDX_Control(pDX, IDC_CNTL_LIST, m_CntlList);
	DDX_Text(pDX, IDC_MESSAGE_LBL, m_StrMessageLbl);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CChangeHistory, CPropertyPage)
	//{{AFX_MSG_MAP(CChangeHistory)
	ON_BN_CLICKED(IDC_NEXT_BUTTON, OnNextButton)
	ON_WM_HELPINFO()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CChangeHistory message handlers

BOOL CChangeHistory::OnInitDialog() 
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
	m_bChangeHistory = TRUE;
    
    char strHeaderData[6][50] = {"DATE           ", "TIME            ","USER          ", "FIELD CHANGED    ", "PREVIOUS DATA     ", "CURRENT DATA     "} ;

    for (int nHeader = 0; nHeader < 6; nHeader++) { 
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
    }


    if (m_nDialogMode != MODE_ADD && m_nDialogMode != MODE_DONE)    
		PopulateHistoryDialog();	
	else 
	{
		m_CntlNextButton.EnableWindow(FALSE);  
		m_CntlList.EnableWindow(FALSE);  
	}
    
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CChangeHistory::CleanChangeHistory()
{
	m_CntlList.DeleteAllItems();
	m_StrMessageLbl = "";
	m_CntlList.EnableWindow(FALSE);  
	UpdateData(FALSE);
}

void CChangeHistory::PopulateHistoryDialog()
{
    HST01_LIST sHistoryList;
	LV_ITEM lvItem;
	char strErrorMessage[200];
    char strDate[30] = "";
	char strTime[30] = "";
    CString CstrTemp;
    int nSize;
    int nRowItem = 0;
    int nActualItem = 0;
    int nNumberReturned;
    bDone = FALSE;
    int nCount = m_CntlList.GetItemCount();
	int rcode = -1;
	m_CntlNextButton.EnableWindow(TRUE);
	if (!bNext)
		CleanChangeHistory();
	memset(&sHistoryList,0x00,sizeof(sHistoryList));
	if(m_nPropertySheetMode == MODE_OMC01)
	{
		if(strlen((char *)sHistoryRecord.file_primary_key) == 0 || 
		   strncmp((char *)sHistoryRecord.file_name,"OBS01",5) == 0)
		{
				rcode = txdsapi_get_list ((pBYTE)&sHistoryRecord, sizeof (HST01), 
				(pBYTE)&sHistoryList, sizeof (HST01_LIST),
				ST2_GET_HST01_BY_FILE_NAME, 
				HST01_DATA, strErrorMessage);  
		}
		else
		{

			rcode = txdsapi_get_list ((pBYTE)&sHistoryRecord, sizeof (HST01), 
				(pBYTE)&sHistoryList, sizeof (HST01_LIST),
				ST2_GET_HST01_BY_FILE_NAME_KEY, 
				HST01_DATA, strErrorMessage);  
		}
	}
	else
	{
	rcode = txdsapi_get_list ((pBYTE)&sHistoryRecord, sizeof (HST01), 
								  (pBYTE)&sHistoryList, sizeof (HST01_LIST),
								   ST2_GET_HST01_BY_FILE_NAME_KEY, 
								   HST01_DATA, strErrorMessage);        
    }
		if (rcode == PTEMSG_OK) { // Dataserver returned DBERR_OK
			nNumberReturned = atoi ((char *)&sHistoryList.num_returned);
            if( nNumberReturned == GUI_MAX_LIST_SIZE)
            {				
			   // CString strTemp = sHistoryList.history->primary_key.unique_id[nNumberReturned-1] ;
				 CString strTemp = (CString)((char *)sHistoryList.history->primary_key.unique_id[nNumberReturned-1] );
			    strTemp.TrimRight() ; strTemp.TrimLeft() ;
			    memcpy ( &sHistoryRecord, &sHistoryList.history[nNumberReturned-1], sizeof (HST01));
			    nSize = nNumberReturned - 1;
            }
            else 
            {
               m_StrMessageLbl = "End of list";
               bDone = TRUE;
		         m_CntlNextButton.EnableWindow(FALSE);
			      nSize = nNumberReturned;
               UpdateData(FALSE);
            }
        
     
            for (int nItem = 0; nItem <  nSize; nItem++,nRowItem ++) { // will now insert the items and subitems into the list view.
				m_CntlList.EnableWindow(TRUE);  
			    for (int nSubItem = 0; nSubItem < 6; nSubItem++) {					
					
			    lvItem.mask = LVIF_TEXT ;
				lvItem.iItem = (nSubItem == 0)? nRowItem : nActualItem;
				lvItem.iSubItem = nSubItem;
					// compare filed tag
				
				if((strncmp((char *)sHistoryList.history[nItem].field_tag,"LogIN Date",sizeof("LogIN Date"))==0)
					||(strncmp((char *)sHistoryList.history[nItem].field_tag,"LogOut Date",sizeof("LogOut Date"))==0))
				{
					continue; }
				else
				{
					if (nSubItem == 0) { 
			//			lvItem.pszText = (char *)sHistoryList.history[nItem].date_modified ;
					    memset (strDate, 0, sizeof(strDate)) ;
								
						CstrTemp.Empty() ;
						CstrTemp += (LPTSTR)memcpy (strDate, (char *)sHistoryList.history[nItem].date_modified+6,2) ;
						CstrTemp += "/" ;
						CstrTemp += (LPTSTR)memcpy (strDate, (char *)sHistoryList.history[nItem].date_modified+4,2) ;
						CstrTemp += "/" ;
						CstrTemp += (LPTSTR)memcpy (strDate, (char *)sHistoryList.history[nItem].date_modified,4) ;
						lstrcpy ((LPTSTR)strDate, (LPCTSTR)CstrTemp) ;						
								
						CstrTemp.Empty() ;
								
						lvItem.pszText = strDate ;
					}
					else if (nSubItem == 1) { 
						//lvItem.pszText = (char *)sHistoryList.history[nItem].time_modified ;
					    memset (strTime, 0, sizeof(strTime)) ;
								
						CstrTemp.Empty() ;
						CstrTemp += (LPTSTR)memcpy (strTime, (char *)sHistoryList.history[nItem].time_modified,2) ;
					    CstrTemp += ":" ;
						CstrTemp += (LPTSTR)memcpy (strTime, (char *)sHistoryList.history[nItem].time_modified+2,2) ;
					    CstrTemp += ":" ;
						CstrTemp += (LPTSTR)memcpy (strTime, (char *)sHistoryList.history[nItem].time_modified+4,2) ;
						lstrcpy ((LPTSTR)strTime, (LPCTSTR)CstrTemp) ;						
								
						CstrTemp.Empty() ;
								
						lvItem.pszText = strTime ;
					}
					else if (nSubItem == 2) { 
													
						lvItem.pszText = (char *)sHistoryList.history[nItem].changed_by ;								
					}
					else if (nSubItem == 3) 
					{ 
						//if(strcmp(sHistoryList.history[nItem].field_tag,""))
						lvItem.pszText = (char *)sHistoryList.history[nItem].field_tag ;								
					} 
					else if (nSubItem == 4) { 
						lvItem.pszText = (char *)sHistoryList.history[nItem].previous_data ;								
					} 
					else if (nSubItem == 5) { 
						lvItem.pszText = (char *)sHistoryList.history[nItem].current_data;				
					} // end else if
							
					if (nSubItem == 0) 
						//nActualItem = m_CntlList.InsertItem(&lvItem);
						nActualItem = m_CntlList.InsertItem(nCount+nSize,(char *)strDate);
					else 
						m_CntlList.SetItem(&lvItem);
							
			/*		if (nSubItem == 1) // transaction id
						m_CntlList.SetColumnWidth (nSubItem, 0) ;
							
					else 
						m_CntlList.SetColumnWidth (nSubItem, LVSCW_AUTOSIZE_USEHEADER) ;				
					    */
					m_CntlList.SetFocus() ;
					int nCount = m_CntlList.GetItemCount();

					m_CntlList.EnsureVisible( nCount-1, TRUE);
				}//else for other tags		
		    	} // for
	    	} // for 			
		}
		else
		{
			m_StrMessageLbl = strErrorMessage;
			UpdateData(FALSE);
		}    
}

void CChangeHistory::OnNextButton()
{
    int nRowItem = 0;
    int nActualItem = 0;
	bNext = TRUE;

	if (!bDone) 
		PopulateHistoryDialog();
	else 
	{
        m_StrMessageLbl = "End of list";
		m_CntlNextButton.EnableWindow(FALSE);
		UpdateData(FALSE);
	}    	
}

BOOL CChangeHistory::OnHelpInfo(HELPINFO* pHelpInfo) 
{
//	HtmlHelp (AfxGetMainWnd()->GetSafeHwnd(),
//	"security.chm>Main",HH_HELP_CONTEXT,IDD_CHANGE_HISTORY);
	
	return TRUE;
}

BOOL CChangeHistory::OnSetActive() 
{
	if ( m_nPropertySheetMode == MODE_GROUP )
	{
		if(flag_group == 0)
			{

				btnWidth_group = 0;
				btnHeight_group = 0;
			    var_group = 300;
				
				memset(winRect1_group,0,sizeof(winRect1_group));
				GetClientRect(&winRect1_group);
			    
				
				memset(winRect_group,0,sizeof(winRect_group));
				m_CntlList.GetWindowRect(&winRect_group);
				ScreenToClient(&winRect_group);

			    
				memset(rect_group,0,sizeof(rect_group));
				m_CntlNextButton.GetWindowRect(&rect_group);
				ScreenToClient(&rect_group);

				btnWidth_group = rect_group.Width();
				btnHeight_group = rect_group.Width();

				winRect1_group.right = winRect1_group.right-btnWidth_group-80;
				winRect1_group.bottom = winRect1_group.bottom-btnHeight_group-260;

				
				winRect_group.right = winRect_group.right - winRect_group.left+var_group;
				winRect_group.bottom = winRect_group.bottom - winRect_group.top;
				
				var_group = 0;
				flag_group = 1;
			}
				
			m_CntlNextButton.SetWindowPos(NULL,winRect1_group.right,winRect1_group.bottom,SWP_NOMOVE,SWP_NOMOVE,SWP_NOSIZE|SWP_NOZORDER);	
            m_CntlList.SetWindowPos(NULL,SWP_NOMOVE,SWP_NOMOVE, winRect_group.right,winRect_group.bottom,SWP_NOMOVE | SWP_NOZORDER);

	}

	/*if ( m_nPropertySheetMode == MODE_USER )
		((CUserPropertySheet*) this->GetParent())->m_nActiveTab = 20;*/

/*	if ( m_nPropertySheetMode == MODE_USER )
	{
				CRect rect;
				m_CntlList.GetWindowRect(&rect);
				ScreenToClient(&rect); 
				m_CntlList.SetWindowPos(NULL,SWP_NOMOVE,SWP_NOMOVE, rect.right-rect.left+100,rect.bottom-rect.top,SWP_NOMOVE | SWP_NOZORDER);
		
	}*/

	if ( m_nPropertySheetMode == MODE_USER )
	{

		if(flag_user == 0)
			{

		
				memset(winRect1,0,sizeof(winRect1));
				GetClientRect(&winRect1);
			    
				
				memset(winRect,0,sizeof(winRect));
				m_CntlList.GetWindowRect(&winRect);
				ScreenToClient(&winRect);

			    
				memset(rect,0,sizeof(rect));
				m_CntlNextButton.GetWindowRect(&rect);
				ScreenToClient(&rect);

				btnWidth = rect.Width();
				btnHeight = rect.Width();

				winRect1.right = winRect1.right-btnWidth-80;
				winRect1.bottom = winRect1.bottom-btnHeight-250;

				
				winRect.right = winRect.right-winRect.left+var;
				winRect.bottom = winRect.bottom-winRect.top;
				
				var = 0;
				flag_user = 1;
			}
				
			m_CntlNextButton.SetWindowPos(NULL,winRect1.right,winRect1.bottom,SWP_NOMOVE,SWP_NOMOVE,SWP_NOSIZE|SWP_NOZORDER);	
            m_CntlList.SetWindowPos(NULL,SWP_NOMOVE,SWP_NOMOVE, winRect.right,winRect.bottom,SWP_NOMOVE | SWP_NOZORDER);

	}


	if ( m_nPropertySheetMode == MODE_CARD )
		((CCardTypePropertySheet*) this->GetParent())->m_nActiveTab = 20;
	if ( m_nPropertySheetMode == MODE_BIN )
		((CBinPropertySheet*) this->GetParent())->m_nActiveTab = 20;

	if(m_nPropertySheetMode == MODE_OMC01)
	{
		PopulateHistoryDialog();
	}

	return CPropertyPage::OnSetActive();
}

BOOL CChangeHistory::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	// TODO: Add your specialized code here and/or call the base class
	
	c_time = CTime::GetCurrentTime();	
	shrmem.SetLastUpdateTime(c_time);
	return CPropertyPage::OnCommand(wParam, lParam);
}
