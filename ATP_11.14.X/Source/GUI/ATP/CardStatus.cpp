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
* $Log:   N:\PVCS\PTE\Equitable\ATP_gui\CardStatus.cpp  $
   
      Rev 1.10   Dec 01 2003 11:28:56   lmitchel
   Added AMEX Online Updates - scr1200
   
      Rev 1.9   Jan 13 2000 17:07:44   mbysani
   bug fixed
   
      Rev 1.2   Sep 30 1999 16:16:56   npogosya
   Bug fixes
   
      Rev 1.1   Sep 22 1999 15:44:50   iarustam
   Added manual batch entry GUI
   
      Rev 1.0   Jul 01 1999 14:17:02   iarustam
   Initial Release
   
      Rev 1.0   Jun 28 1999 13:58:08   iarustam
   Initial Release
   
      Rev 1.4   Jun 11 1999 14:07:22   iarustam
   Bug fixes
   
   
      Rev 1.3   Jun 02 1999 16:23:00   iarustam
   Bug fixes
   
      Rev 1.2   May 10 1999 11:07:18   dperson
   Standardized error messages
   
      Rev 1.1   Apr 29 1999 14:56:00   iarustam
   Bug fixes
   
      Rev 1.0   Feb 23 1999 11:40:08   iarustam
   Initial Release
   
      Rev 1.0   Feb 17 1999 10:09:10   iarustam
   Initial Release
   
      Rev 1.1   Jan 22 1999 10:58:00   iarustam
   Bug fixes and core 1_3_0 changes
   
      Rev 1.1   Jan 21 1999 13:07:04   iarustam
   Bug fixes
   
   
      Rev 1.1   Dec 18 1998 13:42:38   iarustam
    
*
************************************************************************************/

// CardStatus.cpp : implementation file
//

#include "stdafx.h"
#include "atp.h"
#include "CardStatus.h"
#include "CardStatRec.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#include "SharedMem.h"

extern CSharedMem shrmem;
extern UCF01 ucf01;

/////////////////////////////////////////////////////////////////////////////
// CCardStatus dialog


CCardStatus::CCardStatus(CWnd* pParent /*=NULL*/)
	: CDialog(CCardStatus::IDD, pParent)
{
	//{{AFX_DATA_INIT(CCardStatus)
	//}}AFX_DATA_INIT
}


void CCardStatus::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCardStatus)
	DDX_Control(pDX, IDC_UPDATE, m_UpdateButton);
	DDX_Control(pDX, IDC_DELETE, m_DeleteButton);
	DDX_Control(pDX, IDC_ADD, m_AddButton);
	DDX_Control(pDX, IDC_CSFLIST, m_ctrlList);
	DDX_Control(pDX, IDC_COMMENTS, m_ctrlComEdit);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CCardStatus, CDialog)
	//{{AFX_MSG_MAP(CCardStatus)
	ON_BN_CLICKED(IDC_UPDATE, OnUpdate)
	ON_BN_CLICKED(IDC_ADD, OnAdd)
	ON_NOTIFY(NM_DBLCLK, IDC_CSFLIST, OnDblclkCsflist)
	ON_BN_CLICKED(IDC_DELETE, OnDelete)
	ON_WM_KILLFOCUS()
	ON_BN_CLICKED(IDC_NEXT, OnNext)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCardStatus message handlers

BOOL CCardStatus::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	LV_COLUMN lvColumn ;
	CFont m_font;
	LOGFONT lf;
	
	memset ( &lf, 0, sizeof (lf));
	lf.lfHeight = 12;
	lf.lfWeight = 700;
	strcpy(lf.lfFaceName, "Arial");
	m_font.CreateFontIndirect (&lf);
	m_ctrlComEdit.SetFont (&m_font);

	CString temp;
	int nIndex = -1;

	for ( int i = 0; i <=15; i++ )
	{
		temp = ucf01.control[i].subsystem_name;
		if ( strcmp ( temp, "Card Account Control" ) == 0)
			nIndex = i;
	}

	if ( nIndex != -1)
	{
		if ( ucf01.control[nIndex].action[0] == '0' )
			m_AddButton.EnableWindow(FALSE);
		if ( ucf01.control[nIndex].action[1] == '0' )
			m_DeleteButton.EnableWindow(FALSE);
		if ( ucf01.control[nIndex].action[2] == '0' )
			m_UpdateButton.EnableWindow(FALSE);		
	}

	char strHeaderData[7][50] = {"Status ","Description                      ","ISO Response Code","Mac Value          ","Response Text Message"} ;

	for (int nHeader = 0; nHeader < 5; nHeader++)
	{ 
		lvColumn.mask = LVCF_FMT | LVCF_SUBITEM | LVCF_TEXT  ;
		lvColumn.fmt = LVCFMT_LEFT ;
		lvColumn.pszText = strHeaderData[nHeader] ;
		lvColumn.iSubItem = nHeader ;
		
		m_ctrlList.InsertColumn(nHeader, &lvColumn) ;  // assumes return value is OK.
		m_ctrlList.SetColumnWidth (0, LVSCW_AUTOSIZE_USEHEADER) ;
		m_ctrlList.SetColumnWidth (1, LVSCW_AUTOSIZE_USEHEADER) ;
		m_ctrlList.SetColumnWidth (2, LVSCW_AUTOSIZE_USEHEADER) ;
		m_ctrlList.SetColumnWidth (3, LVSCW_AUTOSIZE_USEHEADER) ;
		m_ctrlList.SetColumnWidth (4, LVSCW_AUTOSIZE_USEHEADER) ;
   	}

  	ZeroMemory ( &sCSFRecord, sizeof (CSF01));

	strcpy((char *)sCSFRecord.primary_key.status, " ");
	
	PopulateCSFListDialog();

	if ( ( ucf01.control[nIndex].action[0] == '0' ) &&	
	     ( ucf01.control[nIndex].action[1] == '0' ) &&
	     ( ucf01.control[nIndex].action[2] == '0' ) )
			m_ctrlList.EnableWindow(FALSE);
	UpdateData(FALSE);	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


void CCardStatus::PopulateCSFListDialog()
{	
	LV_ITEM lvItem;
	char strErrorMessage[200];
    int nSize;
    int nRowItem = 0;
    int nActualItem = 0;
    int nNumberReturned;
    bDone = FALSE;
	int nCount = m_ctrlList.GetItemCount();
	int rcode = txdsapi_get_list ((pBYTE)&sCSFRecord, sizeof(CSF01),
									(pBYTE)&sCSFList, sizeof(CSF01_GUI_LIST),
									0,CSF01_DATA, strErrorMessage);
	if (rcode == PTEMSG_OK)
	{

			nNumberReturned = atoi ((char *)&sCSFList.num_returned);
            if( nNumberReturned == GUI_MAX_LIST_SIZE)
            {				
			    memcpy( sCSFRecord.primary_key.status, (LPCTSTR) sCSFList.csf01_record[nNumberReturned-1].primary_key.status, sizeof (sCSFRecord.primary_key.status)); 
			    memcpy( sCSFRecord.description, (LPCTSTR) sCSFList.csf01_record[nNumberReturned-1].description, sizeof (sCSFRecord.description)); 
				nSize = nNumberReturned - 1;
			}
			else
			{
				bDone = TRUE;
				nSize = nNumberReturned;
			}
						
			for (int nItem = 0; nItem <  nSize; nItem++,nRowItem ++) { // will now insert the items and subitems into the list view.
			    for (int nSubItem = 0; nSubItem < 5; nSubItem++) {					
					
			    lvItem.mask = LVIF_TEXT ;
				lvItem.iItem = (nSubItem == 0)? nRowItem : nActualItem;
				lvItem.iSubItem = nSubItem;

				if (nSubItem == 0) 
				{ 
					lvItem.pszText = (char *)sCSFList.csf01_record[nItem].primary_key.status;                   
				}
				else if (nSubItem == 1) 
				{ 
					lvItem.pszText = (char *)sCSFList.csf01_record[nItem].description;                   
				}
				else if (nSubItem == 2) 
				{ 
					lvItem.pszText = (char *)sCSFList.csf01_record[nItem].iso_resp_code;                   
				}
				else if(nSubItem == 3) 
				{ 
					lvItem.pszText = (char *)sCSFList.csf01_record[nItem].mac_value;                   
				}
				else if (nSubItem == 4) 
				{ 
					lvItem.pszText = (char *)sCSFList.csf01_record[nItem].resp_text_msg;                   
				}
				
                
						
				if (nSubItem == 0) 
					nActualItem = m_ctrlList.InsertItem(nCount+nSize,(char *)sCSFList.csf01_record[nItem].primary_key.status);
					//nActualItem = m_ctrlList.InsertItem(&lvItem);
				else 
					m_ctrlList.SetItem(&lvItem);

	
			} // for
		} // for 
					m_ctrlList.SetFocus() ;
		nCount = m_ctrlList.GetItemCount();
		if(nCount > GUI_MAX_LIST_SIZE)
			m_ctrlList.EnsureVisible( nCount-1, TRUE);
	}
}


void CCardStatus::OnUpdate() 
{
	int result;
	CCardStatRec sStatRec;
	char ErrMsg[256];

	CSF01 sTmpCardRecord;

	int nRowSelected = m_ctrlList.GetNextItem (-1, LVNI_SELECTED) ;

	if( nRowSelected == -1)
	{
		m_ctrlComEdit.SetWindowText("Please select an item");
		return;
	}
	else
	{
		sStatRec.DialogMode = MODE_EDIT;

		m_ctrlComEdit.SetWindowText("");

		sStatRec.m_strCardStat = m_ctrlList.GetItemText (nRowSelected, 0) ;
		sStatRec.m_strDescr = m_ctrlList.GetItemText (nRowSelected, 1) ;
		sStatRec.m_strISOResp = m_ctrlList.GetItemText (nRowSelected, 2) ;
		sStatRec.m_strMacValue = m_ctrlList.GetItemText (nRowSelected, 3) ;
		sStatRec.m_strTextMes = m_ctrlList.GetItemText (nRowSelected, 4) ;

		result = sStatRec.DoModal();
		if (result == IDOK)
		{
			ZeroMemory (&sTmpCardRecord,sizeof(CSF01));
			lstrcpy ((LPTSTR) sTmpCardRecord.primary_key.status,sStatRec.m_strCardStat);
			lstrcpy ((LPTSTR) sTmpCardRecord.description,sStatRec.m_strDescr);
			lstrcpy ((LPTSTR) sTmpCardRecord.iso_resp_code,sStatRec.m_strISOResp);
			lstrcpy ((LPTSTR) sTmpCardRecord.mac_value,sStatRec.m_strMacValue);
			lstrcpy ((LPTSTR) sTmpCardRecord.resp_text_msg,sStatRec.m_strTextMes);

			if (txdsapi_update_record((pBYTE)&sTmpCardRecord,sizeof(CSF01),CSF01_DATA,ErrMsg) != PTEMSG_OK)
			{
				m_ctrlComEdit.SetWindowText(ErrMsg);
				return ;
			}

			m_ctrlList.SetItemText(nRowSelected, 1, sStatRec.m_strDescr);
			m_ctrlList.SetItemText(nRowSelected, 2, sStatRec.m_strISOResp);
			m_ctrlList.SetItemText(nRowSelected, 3, sStatRec.m_strMacValue);
			m_ctrlList.SetItemText(nRowSelected, 4, sStatRec.m_strTextMes);

			m_ctrlComEdit.SetWindowText("Record Updated");
		}

	}
}

void CCardStatus::OnAdd() 
{
	CCardStatRec sStatRec;
	int result,nCount,nSelectedIndex = -1;
	char ErrMsg[256];
	LV_ITEM lvItem;
	CString temp;
CSF01 sTmpCSFRecord;
	sStatRec.DialogMode = MODE_ADD;
	sStatRec.m_strISOResp = "51";
	m_ctrlComEdit.SetWindowText("");
	    result = sStatRec.DoModal();

	if (result != IDOK)
		return;	
	nCount = m_ctrlList.GetItemCount();

	for (int i = 0; i < nCount; i++)
	{
		temp = m_ctrlList.GetItemText(i,0);
		if (strcmp(temp,sStatRec.m_strCardStat) == 0)
			nSelectedIndex = i;
	}

	if (nSelectedIndex != -1)
	{
		m_ctrlComEdit.SetWindowText("ERROR: Card Status already exists");		
		return;
	}

	ZeroMemory (&sTmpCSFRecord,sizeof(CSF01));
	lstrcpy ((LPTSTR) sTmpCSFRecord.primary_key.status,sStatRec.m_strCardStat);
	lstrcpy ((LPTSTR) sTmpCSFRecord.description,sStatRec.m_strDescr);
	lstrcpy ((LPTSTR) sTmpCSFRecord.iso_resp_code,sStatRec.m_strISOResp);
	lstrcpy ((LPTSTR) sTmpCSFRecord.mac_value,sStatRec.m_strMacValue);
	lstrcpy ((LPTSTR) sTmpCSFRecord.resp_text_msg,sStatRec.m_strTextMes);

	if (txdsapi_get_record((pBYTE)&sTmpCSFRecord,sizeof(CSF01),CSF01_DATA,ErrMsg) == PTEMSG_OK)
	{
		m_ctrlComEdit.SetWindowText("ERROR: Card Status already exists");
		return ;
	}

	if (txdsapi_insert_record((pBYTE)&sTmpCSFRecord,sizeof(CSF01),CSF01_DATA,ErrMsg) != PTEMSG_OK)
	{
		m_ctrlComEdit.SetWindowText(ErrMsg);		
		return ;
	}

	int nRowItem = 0;
	int nActualItem = 0;

	for (int nSubItem = 0; nSubItem < 4; nSubItem++) 
	{					
		
		lvItem.mask = LVIF_TEXT ;
		lvItem.iItem = (nSubItem == 0)? nRowItem : nActualItem;
		lvItem.iSubItem = nSubItem;

		if (nSubItem == 0) 
		{ 
			lvItem.pszText = (char *)sTmpCSFRecord.primary_key.status;                   
		}
		else if (nSubItem == 1) 
		{ 
			lvItem.pszText = (char *)sTmpCSFRecord.description;                   
		}
		else if (nSubItem == 2) 
		{ 
			lvItem.pszText = (char *)sTmpCSFRecord.iso_resp_code;                   
		}
		else if (nSubItem == 3) 
		{ 
			lvItem.pszText = (char *)sTmpCSFRecord.resp_text_msg;                   
		}
		
    
				
		if (nSubItem == 0) 
			nActualItem = m_ctrlList.InsertItem(&lvItem);
		else 
			m_ctrlList.SetItem(&lvItem);

		m_ctrlList.SetFocus();
		UpdateData(FALSE);
		m_ctrlComEdit.SetWindowText("Record added");

	}
}


void CCardStatus::OnDblclkCsflist(NMHDR* pNMHDR, LRESULT* pResult) 
{
	OnUpdate();
	
	*pResult = 0;
}

void CCardStatus::OnDelete() 
{
	CString strMessage,sStat;
	int nReturnCode;
	char ErrMsg[256];

	CSF01 sTmpCardRecord;
	m_ctrlComEdit.SetWindowText("");

	int nRowSelected = m_ctrlList.GetNextItem (-1, LVNI_SELECTED) ;
	sStat = m_ctrlList.GetItemText (nRowSelected, 0);

	if( nRowSelected == -1)
	{
		m_ctrlComEdit.SetWindowText("Please select an item");
		return;
	}
	else
	{
		strMessage.Format ("Are you sure you want to delete status %s ?", (LPCTSTR) sStat) ;
		nReturnCode = AfxMessageBox ((LPCTSTR)strMessage, MB_OKCANCEL|MB_ICONQUESTION) ;
		if (nReturnCode != IDOK) return ;
	}

	ZeroMemory (&sTmpCardRecord,sizeof(CSF01));
	lstrcpy ((LPTSTR) sTmpCardRecord.primary_key.status,sStat);
	if (txdsapi_delete_record((pBYTE)&sTmpCardRecord,sizeof(CSF01),CSF01_DATA,ErrMsg) != PTEMSG_OK)
	{
		m_ctrlComEdit.SetWindowText(ErrMsg);
		return ;
	}

	
		m_ctrlList.DeleteItem(nRowSelected);
		m_ctrlComEdit.SetWindowText("Record Deleted");
}

void CCardStatus::OnKillFocus(CWnd* pNewWnd) 
{
	CDialog::OnKillFocus(pNewWnd);
	
	m_ctrlComEdit.SetWindowText("");
	
}

BOOL CCardStatus::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	// TODO: Add your specialized code here and/or call the base class
	
	c_time = CTime::GetCurrentTime();	
	shrmem.SetLastUpdateTime(c_time);
	return CDialog::OnCommand(wParam, lParam);
}

void CCardStatus::OnNext() 
{
	if(!bDone)
	{
		PopulateCSFListDialog();
	}
	else
	{
		m_ctrlComEdit.SetWindowText("End of List");
		
		m_ctrlList.SetFocus();
		UpdateData(FALSE);
	}
}
