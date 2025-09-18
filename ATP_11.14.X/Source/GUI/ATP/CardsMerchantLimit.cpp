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
* $Log:   N:\POS\PVCS6.6\EPICPORTZ\PTE\Equitable\ATP_gui\CardsMerchantLimit.cpp  $
   
      Rev 1.14   Apr 01 2005 10:13:34   lmitchel
   Initial Checking prior to integration testing for updates made to support country/curreny code limits (scr13929), unulimited purchase card MCC ranges, and purchase card transaction limits(scr12785)
   
      Rev 1.13   Jun 18 2004 16:38:56   dirby
   Split the Merchant Limit amount field into Peso amount and USD amount.
   SCR 1380
   
      Rev 1.12   Jun 04 2004 11:03:18   lmitchel
   Modified merchant limits table in Authorization Control to add/update/insert MCC limits including new  amount field.  Maximum limit of records changed to 500 from 50.
   
      Rev 1.11   Dec 01 2003 11:28:52   lmitchel
   Added AMEX Online Updates - scr1200
   
      Rev 1.10   Apr 30 2003 12:18:36   lmitchel
   Ver 4.2.0.2 : SCR614 - Allow support for 50 Mercant Type Limits.  Modified Merchant Type Limits dialog - using list control to list all MCC usage codes Auth Profile.  Maximimum number of MCC codes that can be entered is 50 as defined in the database.
   
      Rev 1.9   Jan 12 2000 14:14:34   mbysani
   Changes mades acc to bugs
   
      Rev 1.2   Oct 15 1999 17:06:52   iarustam
   bug fixes
   
      Rev 1.1   Sep 22 1999 15:45:28   iarustam
   Added manual batch entry GUI
   
      Rev 1.0   Jul 01 1999 14:17:06   iarustam
   Initial Release
   
      Rev 1.0   Jun 28 1999 13:58:12   iarustam
   Initial Release
   
      Rev 1.5   Jun 11 1999 14:07:24   iarustam
   Bug fixes
   
   
      Rev 1.4   Apr 29 1999 14:56:04   iarustam
   Bug fixes
   
      Rev 1.3   Mar 16 1999 16:29:38   iarustam
   Bug fixes
   
      Rev 1.2   Mar 04 1999 13:31:50   iarustam
   Bug fixes
   
      Rev 1.0   Feb 23 1999 11:40:10   iarustam
   Initial Release
   
      Rev 1.0   Feb 17 1999 10:09:14   iarustam
   Initial Release
   
      Rev 1.1   Jan 22 1999 10:58:08   iarustam
   Bug fixes and core 1_3_0 changes
   
      Rev 1.1   Jan 21 1999 13:07:14   iarustam
   Bug fixes
   
   
      Rev 1.1   Dec 18 1998 13:42:46   iarustam
    
*
************************************************************************************/
// CardsMerchantLimit.cpp : implementation file
//

#include "stdafx.h"
#include "Atp.h"
#include "CardsMerchantLimit.h"
#include "MyUtils.h"

#include "SharedMem.h"

extern CSharedMem shrmem;

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

//////////////////////////////////////////////////////////////////
// CCardsMerchantLimit property page

IMPLEMENT_DYNCREATE(CCardsMerchantLimit, CPropertyPage)

CCardsMerchantLimit::CCardsMerchantLimit() : CPropertyPage(CCardsMerchantLimit::IDD)
{
	//{{AFX_DATA_INIT(CCardsMerchantLimit)
	m_StrMessageLbl = _T("");
	m_StrProfileId_Fld = _T("");
	m_StrType_Fld = _T("");
	m_StrUses = _T("");
	m_StrMCC = _T("");
	m_StrAmount = _T("");
	m_StrUSDLimit = _T("");
	//}}AFX_DATA_INIT
}

CCardsMerchantLimit::~CCardsMerchantLimit()
{
}

void CCardsMerchantLimit::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCardsMerchantLimit)
	DDX_Control(pDX, IDC_DOLLAR_LIMIT, m_CntlUSDLimit);
	DDX_Control(pDX, IDC_AMOUNT, m_CntlAmount);
	DDX_Control(pDX, IDC_MCC, m_CntlMCC);
	DDX_Control(pDX, IDC_USES, m_CntlUses);
	DDX_Control(pDX, IDC_MCC_UPDATE, m_CntlUpdateButton);
	DDX_Control(pDX, IDC_MCC_DELETE, m_CntlDeleteButton);
	DDX_Control(pDX, IDC_MCC_ADD, m_CntlAddButton);
	DDX_Control(pDX, IDC_MCCLIST_CTRL, m_CntlMCCList);
	DDX_Control(pDX, IDC_TYPE_FLD, m_CntlType_Fld);
	DDX_Control(pDX, IDC_PROFILE_ID_FLD, m_CntlProfileId_Fld);
	DDX_Text(pDX, IDC_MESSAGE_LBL, m_StrMessageLbl);
	DDX_Text(pDX, IDC_PROFILE_ID_FLD, m_StrProfileId_Fld);
	DDV_MaxChars(pDX, m_StrProfileId_Fld, 4);
	DDX_Text(pDX, IDC_TYPE_FLD, m_StrType_Fld);
	DDX_Text(pDX, IDC_USES, m_StrUses);
	DDV_MaxChars(pDX, m_StrUses, 4);
	DDX_Text(pDX, IDC_MCC, m_StrMCC);
	DDV_MaxChars(pDX, m_StrMCC, 4);
	DDX_Text(pDX, IDC_AMOUNT, m_StrAmount);
	DDV_MaxChars(pDX, m_StrAmount, 13);
	DDX_Text(pDX, IDC_DOLLAR_LIMIT, m_StrUSDLimit);
	DDV_MaxChars(pDX, m_StrUSDLimit, 13);
	//}}AFX_DATA_MAP
    
    }


BEGIN_MESSAGE_MAP(CCardsMerchantLimit, CPropertyPage)
	//{{AFX_MSG_MAP(CCardsMerchantLimit)
	ON_BN_CLICKED(IDC_MCC_ADD, OnMccAddButton)
	ON_BN_CLICKED(IDC_MCC_DELETE, OnMccDeleteButton)
	ON_BN_CLICKED(IDC_MCC_UPDATE, OnMccUpdateButton)
	ON_NOTIFY(NM_DBLCLK, IDC_MCCLIST_CTRL, OnDblclkMcclistCtrl)
	ON_NOTIFY(NM_CLICK, IDC_MCCLIST_CTRL, OnClickMcclistCtrl)
	ON_NOTIFY(LVN_KEYDOWN, IDC_MCCLIST_CTRL, OnKeydownMcclistCtrl)
	ON_NOTIFY(NM_RETURN, IDC_MCCLIST_CTRL, OnReturnMcclistCtrl)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCardsMerchantLimit message handlers

/************************************************************/
 ////////////////////////////////////////////////////////////
/************************************************************/

BOOL CCardsMerchantLimit::OnInitDialog() 
{
   CPropertyPage::OnInitDialog();

   CFont m_font;
   LOGFONT lf;
   LV_COLUMN lvColumn;

   memset ( &lf, 0, sizeof (lf));
   lf.lfHeight = 12;
   lf.lfWeight = 700;
   strcpy(lf.lfFaceName, "Arial");
   m_font.CreateFontIndirect (&lf);
   GetDlgItem(IDC_MESSAGE_LBL)->SetFont (&m_font); 

   m_bMerchant = TRUE;
   m_StrMessageLbl = "";

   /*Set up column headings in list control: MCC and Uses*/
   char strHeaderData[4][20] = {"  MCC  ", " USES  ", "    PESO LIMIT    ", "USD LIMIT"};

   for (int nHeader = 0; nHeader < 4; nHeader++)
   {
      lvColumn.mask = LVCF_FMT | LVCF_SUBITEM | LVCF_TEXT ;
      lvColumn.fmt  = LVCFMT_RIGHT;
      lvColumn.pszText = strHeaderData[nHeader];
      lvColumn.iSubItem = nHeader;

      m_CntlMCCList.InsertColumn(nHeader, &lvColumn); // assumes return value is ok
      m_CntlMCCList.SetColumnWidth(0, LVSCW_AUTOSIZE_USEHEADER);
      m_CntlMCCList.SetColumnWidth(1, LVSCW_AUTOSIZE_USEHEADER);
      m_CntlMCCList.SetColumnWidth(2, LVSCW_AUTOSIZE_USEHEADER);
      m_CntlMCCList.SetColumnWidth(3, LVSCW_AUTOSIZE_USEHEADER);
   }

   if (m_nDialogMode != MODE_DONE )
   {
      if (m_nDialogMode != MODE_VIEW)
         EnableMerchantDialog(TRUE);
      else
         EnableMerchantDialog(FALSE);

      if(m_nDialogMode != MODE_ADD)
      {
         PopulateMerchantDialogFromDatabase (sProfileIdOld);
      }
   }    

   if(m_nDialogMode == MODE_ADD)
   {
      m_StrMCC.Empty();
      m_StrUses.Empty();
      m_StrAmount.Empty();
      m_StrUSDLimit.Empty();
      m_CntlMCC.SetFocus();
   }
   else
      m_CntlMCCList.SetFocus();

   if (!UpdateData(FALSE))
   {
      AfxMessageBox (IDS_UPDATEDATAFAILED, MB_ICONSTOP);
   }

   return TRUE;  // return TRUE unless you set the focus to a control
                 // EXCEPTION: OCX Property Pages should return FALSE
}

/************************************************************/
 ////////////////////////////////////////////////////////////
/************************************************************/

BOOL CCardsMerchantLimit::PopulateMerchantDialogFromDatabase( CAF01 sProfileId )
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
     EnableMerchantDialog(TRUE);

    ZeroMemory ( &sProfileIdOld, sizeof (CAF01));

	m_StrMessageLbl = "";
	//empty list control & edit boxes
	m_CntlMCCList.DeleteAllItems();
	m_StrMCC.Empty();
	m_StrUses.Empty();
	m_StrAmount.Empty();
	m_StrUSDLimit.Empty();
	m_CntlMCCList.EnableWindow(TRUE);

	memset( &m_mcc_limits, 0, sizeof(m_mcc_limits));
	memset( &m_mcc_limits_gui_list, 0, sizeof(m_mcc_limits_gui_list));

    m_StrProfileId_Fld          = sProfileId.primary_key.caf_profile_id;
    m_StrType_Fld               = sProfileId.primary_key.type;

	if ( strcmp (m_StrType_Fld, "C") == 0 )
		m_StrType_Fld = "CREDIT";
	else if ( strcmp (m_StrType_Fld, "F") == 0 )
		m_StrType_Fld = "FLEET";
	else 
		m_StrType_Fld = "DEBIT";

	strcpy( (pCHAR)m_mcc_limits.primary_key.caf_profile_id, m_StrProfileId_Fld);
	strcpy( (pCHAR)m_mcc_limits.primary_key.type, m_StrType_Fld.Left(1));

	/*Get MCC List for profile id and profile type*/
	rcode = txdsapi_get_list( (pBYTE) &m_mcc_limits, sizeof(MCC_LIMITS),
		                      (pBYTE) &m_mcc_limits_gui_list, sizeof(MCC_LIMITS_GUI),
							  0, MCC_LIMITS_DATA, error_msg );

	if(rcode != PTEMSG_OK)
	{
		m_StrMessageLbl = error_msg;
		UpdateData(FALSE);
	}

	num_returned = atoi( (pCHAR)m_mcc_limits_gui_list.num_returned);

	if(num_returned == 0)
	{
		m_StrMessageLbl = "No Data Found";
		UpdateData(FALSE);
		return FALSE;
	}

	if (rcode == PTEMSG_OK)
	{
		if( num_returned == SELECT_LIST_SIZE)
		{
		    strcpy( (pCHAR)m_mcc_limits.primary_key.caf_profile_id,(pCHAR)m_mcc_limits_gui_list.mcc_limits[num_returned-1].primary_key.caf_profile_id );
			strcpy( (pCHAR)m_mcc_limits.primary_key.type,          (pCHAR)m_mcc_limits_gui_list.mcc_limits[num_returned-1].primary_key.type);
			strcpy( (pCHAR)m_mcc_limits.primary_key.mcc,           (pCHAR)m_mcc_limits_gui_list.mcc_limits[num_returned-1].primary_key.mcc);
		}
	    else
		{
				bDone = TRUE;
		}

		UpdateData(FALSE);

	
		//insert items into list view control
		for ( int nItem = 0; nItem < num_returned; nItem++ )
		{
			m_CntlMCCList.EnableWindow(TRUE);

			for(int nSubItem = 0; nSubItem < 4; nSubItem++)
			{
				lvItem.mask = LVIF_TEXT;
				lvItem.iItem = (nSubItem == 0)? nRowItem : nActualItem;
				lvItem.iSubItem = nSubItem;
				temp.Empty();
				memset(strSum, 0, sizeof(strSum));

				if(nSubItem == 0)
				{	
					temp = (char *)m_mcc_limits_gui_list.mcc_limits[nItem].primary_key.mcc;
					temp.TrimLeft() ;
					temp.TrimRight();
					sprintf(strSum, "%-4s", (LPCTSTR)temp);
					lvItem.pszText = strSum;
				}
				else
				if(nSubItem == 1)
				{
					temp = (char *)m_mcc_limits_gui_list.mcc_limits[nItem].usage;
					temp.TrimLeft() ;
					temp.TrimRight();
					sprintf(strSum, "%-3s", (LPCTSTR)temp);
					lvItem.pszText = strSum;
				}
				else
				if(nSubItem == 2)
				{
					temp = (char *)m_mcc_limits_gui_list.mcc_limits[nItem].peso_amt_limit;
					memcpy(strSum, myUtilsObject.DisplayAmount(temp), sizeof (strSum));
					lvItem.pszText = strSum;
				}

				if(nSubItem == 3)
				{
					temp = (char *)m_mcc_limits_gui_list.mcc_limits[nItem].usd_amt_limit;
					memcpy(strSum, myUtilsObject.DisplayAmount(temp), sizeof (strSum));
					lvItem.pszText = strSum;
				}


				if(nSubItem == 0)
					nActualItem = m_CntlMCCList.InsertItem(&lvItem);
				else
					m_CntlMCCList.SetItem(&lvItem); 

				m_CntlMCCList.SetFocus();
			}//for insert sub items
		}//for insert row items
		
		m_CntlMCCList.SetFocus();
	}//for pte_msg_ok

	UpdateData(FALSE);

    memcpy ( &sProfileIdOld, &sProfileId, sizeof ( CAF01));

    return TRUE;

}

/************************************************************/
 ////////////////////////////////////////////////////////////
/************************************************************/

/*BOOL CCardsMerchantLimit::PopulateDatabaseFromMerchantDialog( pCAF01 psProfileId )
{

	if ( !m_bMerchant )
		return TRUE;
	int nCount = 0;
	int i = 0;

	memset (psProfileId->mcl, 0x00, sizeof(psProfileId->mcl));
	for (nCount = m_CntlMCCList.GetItemCount(),i = 0; i< nCount; i++)
	{
		memcpy ( psProfileId->mcl[i].category_code, m_CntlMCCList.GetItemText(i, 0), 4);
		memcpy ( psProfileId->mcl[i].usage, m_CntlMCCList.GetItemText(i, 1), 3);
				
	}

    

    return TRUE;
}*/

/************************************************************/
 ////////////////////////////////////////////////////////////
/************************************************************/

void CCardsMerchantLimit::CleanMerchantDialog()
{
    m_StrProfileId_Fld = "";
    m_StrType_Fld = "";
/*    m_StrMerchCode1_Fld = "";
    m_StrMerchCode2_Fld = "";
    m_StrMerchCode3_Fld = "";
    m_StrMerchCode4_Fld = "";
    m_StrMerchCode5_Fld = "";
    m_StrMerchCode6_Fld = "";
    m_StrMerchCode7_Fld = "";
    m_StrMerchCode8_Fld = "";
    m_StrMerchCode9_Fld = "";
    m_StrMerchCode10_Fld = "";
    m_StrNumberOfUses1_Fld = "";
    m_StrNumberOfUses2_Fld = "";
    m_StrNumberOfUses3_Fld = "";
    m_StrNumberOfUses4_Fld = "";
    m_StrNumberOfUses5_Fld = "";
    m_StrNumberOfUses6_Fld = "";
    m_StrNumberOfUses7_Fld = "";
    m_StrNumberOfUses8_Fld = "";
    m_StrNumberOfUses9_Fld = "";
    m_StrNumberOfUses10_Fld = "";
    m_StrMessageLbl = "";
*/
	UpdateData(FALSE);
}

/************************************************************/
 ////////////////////////////////////////////////////////////
/************************************************************/

void CCardsMerchantLimit::EnableMerchantDialog(int nStatus)
{
	m_CntlMCC.EnableWindow(nStatus);
	m_CntlUses.EnableWindow(nStatus);
	m_CntlAmount.EnableWindow(nStatus);
	m_CntlUSDLimit.EnableWindow(nStatus);

	m_CntlAddButton.EnableWindow(nStatus);
	m_CntlUpdateButton.EnableWindow(nStatus);
	m_CntlDeleteButton.EnableWindow(nStatus);

	if(m_nDialogMode == MODE_ADD)
	{
		m_CntlUpdateButton.EnableWindow(FALSE);
		m_CntlDeleteButton.EnableWindow(FALSE);
	}

	m_CntlMCCList.EnableWindow(TRUE);

}

/************************************************************/
 ////////////////////////////////////////////////////////////
/************************************************************/


BOOL CCardsMerchantLimit::OnCommand(WPARAM wParam, LPARAM lParam) 
{  
    if (( m_nDialogMode != MODE_VIEW) && ( m_nDialogMode != MODE_DONE))
		SetModified ( TRUE );
	
	
	c_time = CTime::GetCurrentTime();	
	shrmem.SetLastUpdateTime(c_time);

	return CPropertyPage::OnCommand(wParam, lParam);
}

/************************************************************/
 ////////////////////////////////////////////////////////////
/************************************************************/

BOOL CCardsMerchantLimit::CompareStructures( pCAF01 psProfileId )
{
	int nCount = 0;
	int i = 0;
/*	for (nCount = m_CntlMCCList.GetItemCount(),i = 0; i< nCount; i++)
	{
		if (strcmp ( (CHAR *)psProfileId->mcl[i].category_code, (CHAR *)sProfileIdOld.mcl[i].category_code) != 0)
		{
			memset (m_StrCurrentData, 0, sizeof (m_StrCurrentData));
			memset (m_StrPreviousData, 0, sizeof (m_StrPreviousData));
			memset (m_StrFieldTag, 0, sizeof (m_StrFieldTag));
	        memcpy(m_StrCurrentData,psProfileId->mcl[i].category_code,sizeof (m_StrCurrentData));
			memcpy(m_StrPreviousData, sProfileIdOld.mcl[i].category_code,sizeof (m_StrPreviousData));
			sprintf(m_StrFieldTag,"Merchant Code %d", i+1);

			PopulateHistoryTable();
		}	

		if (strcmp ( (CHAR *)psProfileId->mcl[i].usage, (CHAR *)sProfileIdOld.mcl[i].usage) != 0)
		{
			memset (m_StrCurrentData, 0, sizeof (m_StrCurrentData));
			memset (m_StrPreviousData, 0, sizeof (m_StrPreviousData));
			memset (m_StrFieldTag, 0, sizeof (m_StrFieldTag));
	        memcpy(m_StrCurrentData,psProfileId->mcl[i].usage,sizeof (m_StrCurrentData));
			memcpy(m_StrPreviousData, sProfileIdOld.mcl[i].usage,sizeof (m_StrPreviousData));
			sprintf(m_StrFieldTag,"Merchant Usage %d", i+1);

			PopulateHistoryTable();
		}	
				
	}
*/
   // if (strcmp ( m_StrMerchCode1_Fld,(char *)sProfileIdOld.mcl[0].category_code) != 0)
/*	if (strcmp ( (CHAR *)psProfileId->mcl[0].category_code, (CHAR *)sProfileIdOld.mcl[0].category_code) != 0)
    {
		memset (m_StrCurrentData, 0, sizeof (m_StrCurrentData));
		memset (m_StrPreviousData, 0, sizeof (m_StrPreviousData));
		memset (m_StrFieldTag, 0, sizeof (m_StrFieldTag));
//        memcpy(m_StrCurrentData,m_StrMerchCode1_Fld,sizeof (m_StrCurrentData));
        memcpy(m_StrPreviousData, sProfileIdOld.mcl[0].category_code,sizeof (m_StrPreviousData));
        memcpy(m_StrFieldTag,"Merchant Code1",sizeof(m_StrFieldTag));

        PopulateHistoryTable();
    }
/*
    if (strcmp ( m_StrMerchCode2_Fld,(char *)sProfileIdOld.mcl[1].category_code) != 0)
    {
        memcpy(m_StrCurrentData, m_StrMerchCode2_Fld,sizeof (m_StrCurrentData));
        memcpy(m_StrPreviousData,sProfileIdOld.mcl[1].category_code,sizeof (m_StrPreviousData));
        memcpy(m_StrFieldTag,"Merchant Code2",sizeof(m_StrFieldTag));

        PopulateHistoryTable();
    }
	memset (m_StrCurrentData, 0, sizeof (m_StrCurrentData));
	memset (m_StrPreviousData, 0, sizeof (m_StrPreviousData));
	memset (m_StrFieldTag, 0, sizeof (m_StrFieldTag));


    if (strcmp ( m_StrMerchCode3_Fld,(char *)sProfileIdOld.mcl[2].category_code) != 0)
    {
        memcpy(m_StrCurrentData,m_StrMerchCode3_Fld,sizeof (m_StrCurrentData));
        memcpy(m_StrPreviousData,sProfileIdOld.mcl[2].category_code,sizeof (m_StrPreviousData));
        memcpy(m_StrFieldTag,"Merchant Code3",sizeof(m_StrFieldTag));

        PopulateHistoryTable();
    }
	memset (m_StrCurrentData, 0, sizeof (m_StrCurrentData));
	memset (m_StrPreviousData, 0, sizeof (m_StrPreviousData));
	memset (m_StrFieldTag, 0, sizeof (m_StrFieldTag));

    if (strcmp ( m_StrMerchCode4_Fld,(char *)sProfileIdOld.mcl[3].category_code) != 0)
    {
        memcpy(m_StrCurrentData,m_StrMerchCode4_Fld,sizeof (m_StrCurrentData));
        memcpy(m_StrPreviousData,sProfileIdOld.mcl[3].category_code,sizeof (m_StrPreviousData));
        memcpy(m_StrFieldTag,"Merchant Code4",sizeof(m_StrFieldTag));

        PopulateHistoryTable();
    }
	memset (m_StrCurrentData, 0, sizeof (m_StrCurrentData));
	memset (m_StrPreviousData, 0, sizeof (m_StrPreviousData));
	memset (m_StrFieldTag, 0, sizeof (m_StrFieldTag));

    if (strcmp ( m_StrMerchCode5_Fld,(char *)sProfileIdOld.mcl[4].category_code) != 0)
    {
        memcpy(m_StrCurrentData,m_StrMerchCode5_Fld,sizeof (m_StrCurrentData));
        memcpy(m_StrPreviousData,sProfileIdOld.mcl[4].category_code,sizeof (m_StrPreviousData));
        memcpy(m_StrFieldTag,"Merchant Code5",sizeof(m_StrFieldTag));

        PopulateHistoryTable();
    }
	memset (m_StrCurrentData, 0, sizeof (m_StrCurrentData));
	memset (m_StrPreviousData, 0, sizeof (m_StrPreviousData));
	memset (m_StrFieldTag, 0, sizeof (m_StrFieldTag));

    if (strcmp ( m_StrMerchCode6_Fld,(char *)sProfileIdOld.mcl[5].category_code) != 0)
    {
        memcpy(m_StrCurrentData,m_StrMerchCode6_Fld,sizeof (m_StrCurrentData));
        memcpy(m_StrPreviousData,sProfileIdOld.mcl[5].category_code, sizeof (m_StrPreviousData));
        memcpy(m_StrFieldTag,"Merchant Code6",sizeof(m_StrFieldTag));

        PopulateHistoryTable();
    }
	memset (m_StrCurrentData, 0, sizeof (m_StrCurrentData));
	memset (m_StrPreviousData, 0, sizeof (m_StrPreviousData));
	memset (m_StrFieldTag, 0, sizeof (m_StrFieldTag));

    if (strcmp ( m_StrMerchCode7_Fld,(char *)sProfileIdOld.mcl[6].category_code) != 0)
    {
        memcpy(m_StrCurrentData,m_StrMerchCode7_Fld,sizeof (m_StrCurrentData));
        memcpy(m_StrPreviousData,sProfileIdOld.mcl[6].category_code,sizeof (m_StrPreviousData));
        memcpy(m_StrFieldTag,"Merchant Code7",sizeof(m_StrFieldTag));

        PopulateHistoryTable();
    }
	memset (m_StrCurrentData, 0, sizeof (m_StrCurrentData));
	memset (m_StrPreviousData, 0, sizeof (m_StrPreviousData));
	memset (m_StrFieldTag, 0, sizeof (m_StrFieldTag));

    if (strcmp ( m_StrMerchCode8_Fld,(char *)sProfileIdOld.mcl[7].category_code) != 0)
    {
        memcpy(m_StrCurrentData,m_StrMerchCode8_Fld,sizeof (m_StrCurrentData));
        memcpy(m_StrPreviousData,sProfileIdOld.mcl[7].category_code,sizeof (m_StrPreviousData));
        memcpy(m_StrFieldTag,"Merchant Code8",sizeof(m_StrFieldTag));

        PopulateHistoryTable();
    }
	memset (m_StrCurrentData, 0, sizeof (m_StrCurrentData));
	memset (m_StrPreviousData, 0, sizeof (m_StrPreviousData));
	memset (m_StrFieldTag, 0, sizeof (m_StrFieldTag));

    if (strcmp ( m_StrMerchCode9_Fld,(char *)sProfileIdOld.mcl[8].category_code) != 0)
    {
        memcpy(m_StrCurrentData, m_StrMerchCode9_Fld,sizeof (m_StrCurrentData));
        memcpy(m_StrPreviousData,sProfileIdOld.mcl[8].category_code,sizeof (m_StrPreviousData));
        memcpy(m_StrFieldTag,"Merchant Code9",sizeof(m_StrFieldTag));

        PopulateHistoryTable();
    }
	memset (m_StrCurrentData, 0, sizeof (m_StrCurrentData));
	memset (m_StrPreviousData, 0, sizeof (m_StrPreviousData));
	memset (m_StrFieldTag, 0, sizeof (m_StrFieldTag));

    if (strcmp ( m_StrMerchCode10_Fld,(char *)sProfileIdOld.mcl[9].category_code) != 0)
    {
        memcpy(m_StrCurrentData,m_StrMerchCode10_Fld,sizeof (m_StrCurrentData));
        memcpy(m_StrPreviousData,sProfileIdOld.mcl[9].category_code,sizeof (m_StrPreviousData));
        memcpy(m_StrFieldTag,"Merchant Code10",sizeof(m_StrFieldTag));

        PopulateHistoryTable();
    }
	memset (m_StrCurrentData, 0, sizeof (m_StrCurrentData));
	memset (m_StrPreviousData, 0, sizeof (m_StrPreviousData));
	memset (m_StrFieldTag, 0, sizeof (m_StrFieldTag));

    if (strcmp ( m_StrNumberOfUses1_Fld,(char *)sProfileIdOld.mcl[0].usage) != 0)
    {
        memcpy(m_StrCurrentData,m_StrNumberOfUses1_Fld,sizeof (m_StrCurrentData));
        memcpy(m_StrPreviousData,sProfileIdOld.mcl[0].usage,sizeof (m_StrPreviousData));
        memcpy(m_StrFieldTag,"Number of Uses1",sizeof(m_StrFieldTag));

        PopulateHistoryTable();
    }
	memset (m_StrCurrentData, 0, sizeof (m_StrCurrentData));
	memset (m_StrPreviousData, 0, sizeof (m_StrPreviousData));
	memset (m_StrFieldTag, 0, sizeof (m_StrFieldTag));

    if (strcmp ( m_StrNumberOfUses2_Fld,(char *)sProfileIdOld.mcl[1].usage) != 0)
    {
        memcpy(m_StrCurrentData,m_StrNumberOfUses2_Fld,sizeof (m_StrCurrentData));
        memcpy(m_StrPreviousData,sProfileIdOld.mcl[1].usage,sizeof (m_StrPreviousData));
        memcpy(m_StrFieldTag,"Number of Uses2",sizeof(m_StrFieldTag));

        PopulateHistoryTable();
    }
	memset (m_StrCurrentData, 0, sizeof (m_StrCurrentData));
	memset (m_StrPreviousData, 0, sizeof (m_StrPreviousData));
	memset (m_StrFieldTag, 0, sizeof (m_StrFieldTag));

    if (strcmp ( m_StrNumberOfUses3_Fld,(char *)sProfileIdOld.mcl[2].usage) != 0)
    {
        memcpy(m_StrCurrentData,m_StrNumberOfUses3_Fld, sizeof (m_StrCurrentData));
        memcpy(m_StrPreviousData,sProfileIdOld.mcl[2].usage,sizeof (m_StrPreviousData));
        memcpy(m_StrFieldTag,"Number of Uses3",sizeof(m_StrFieldTag));

        PopulateHistoryTable();
    }
	memset (m_StrCurrentData, 0, sizeof (m_StrCurrentData));
	memset (m_StrPreviousData, 0, sizeof (m_StrPreviousData));
	memset (m_StrFieldTag, 0, sizeof (m_StrFieldTag));

    if (strcmp ( m_StrNumberOfUses4_Fld,(char *)sProfileIdOld.mcl[3].usage) != 0)
    {
        memcpy(m_StrCurrentData,m_StrNumberOfUses4_Fld,sizeof (m_StrCurrentData));
        memcpy(m_StrPreviousData,sProfileIdOld.mcl[3].usage,sizeof (m_StrPreviousData));
        memcpy(m_StrFieldTag,"Number of Uses4", sizeof(m_StrFieldTag));

        PopulateHistoryTable();
    }
	memset (m_StrCurrentData, 0, sizeof (m_StrCurrentData));
	memset (m_StrPreviousData, 0, sizeof (m_StrPreviousData));
	memset (m_StrFieldTag, 0, sizeof (m_StrFieldTag));

    if (strcmp ( m_StrNumberOfUses5_Fld,(char *)sProfileIdOld.mcl[4].usage) != 0)
    {
        memcpy(m_StrCurrentData, m_StrNumberOfUses5_Fld,sizeof (m_StrCurrentData));
        memcpy(m_StrPreviousData,sProfileIdOld.mcl[4].usage,sizeof (m_StrPreviousData));
        memcpy(m_StrFieldTag,"Number of Uses5",sizeof(m_StrFieldTag));

        PopulateHistoryTable();
    }
	memset (m_StrCurrentData, 0, sizeof (m_StrCurrentData));
	memset (m_StrPreviousData, 0, sizeof (m_StrPreviousData));
	memset (m_StrFieldTag, 0, sizeof (m_StrFieldTag));

    if (strcmp ( m_StrNumberOfUses6_Fld,(char *)sProfileIdOld.mcl[5].usage) != 0)
    {
        memcpy(m_StrCurrentData,m_StrNumberOfUses6_Fld,sizeof (m_StrCurrentData));
        memcpy(m_StrPreviousData,sProfileIdOld.mcl[5].usage,sizeof (m_StrPreviousData));
        memcpy(m_StrFieldTag,"Number of Uses6",sizeof(m_StrFieldTag));

        PopulateHistoryTable();
    }
	memset (m_StrCurrentData, 0, sizeof (m_StrCurrentData));
	memset (m_StrPreviousData, 0, sizeof (m_StrPreviousData));
	memset (m_StrFieldTag, 0, sizeof (m_StrFieldTag));

    if (strcmp ( m_StrNumberOfUses7_Fld,(char *)sProfileIdOld.mcl[6].usage) != 0)
    {
        memcpy(m_StrCurrentData, m_StrNumberOfUses7_Fld,sizeof (m_StrCurrentData));
        memcpy(m_StrPreviousData,sProfileIdOld.mcl[6].usage,sizeof (m_StrPreviousData));
        memcpy(m_StrFieldTag,"Number of Uses7",sizeof(m_StrFieldTag));

        PopulateHistoryTable();
    }
	memset (m_StrCurrentData, 0, sizeof (m_StrCurrentData));
	memset (m_StrPreviousData, 0, sizeof (m_StrPreviousData));
	memset (m_StrFieldTag, 0, sizeof (m_StrFieldTag));

    if (strcmp ( m_StrNumberOfUses8_Fld,(char *)sProfileIdOld.mcl[7].usage) != 0)
    {
        memcpy(m_StrCurrentData,m_StrNumberOfUses8_Fld,sizeof (m_StrCurrentData));
        memcpy(m_StrPreviousData,sProfileIdOld.mcl[7].usage,sizeof (m_StrPreviousData));
        memcpy(m_StrFieldTag,"Number of Uses8",sizeof(m_StrFieldTag));

        PopulateHistoryTable();
    }
	memset (m_StrCurrentData, 0, sizeof (m_StrCurrentData));
	memset (m_StrPreviousData, 0, sizeof (m_StrPreviousData));
	memset (m_StrFieldTag, 0, sizeof (m_StrFieldTag));

    if (strcmp ( m_StrNumberOfUses9_Fld,(char *)sProfileIdOld.mcl[8].usage) != 0)
    {
        memcpy(m_StrCurrentData,m_StrNumberOfUses9_Fld,sizeof (m_StrCurrentData));
        memcpy(m_StrPreviousData,sProfileIdOld.mcl[8].usage, sizeof (m_StrPreviousData));
        memcpy(m_StrFieldTag,"Number of Uses9",sizeof(m_StrFieldTag));

        PopulateHistoryTable();
    }
	memset (m_StrCurrentData, 0, sizeof (m_StrCurrentData));
	memset (m_StrPreviousData, 0, sizeof (m_StrPreviousData));
	memset (m_StrFieldTag, 0, sizeof (m_StrFieldTag));

    if (strcmp ( m_StrNumberOfUses10_Fld,(char *)sProfileIdOld.mcl[9].usage) != 0)
    {
        memcpy(m_StrCurrentData,m_StrNumberOfUses10_Fld,sizeof (m_StrCurrentData));
        memcpy(m_StrPreviousData,sProfileIdOld.mcl[9].usage,sizeof (m_StrPreviousData));
        memcpy(m_StrFieldTag,"Number of Uses10",sizeof(m_StrFieldTag));

        PopulateHistoryTable();
    }

    
  */  
        return TRUE;
}

/************************************************************/
 ////////////////////////////////////////////////////////////
/************************************************************/

void CCardsMerchantLimit::PopulateHistoryTable()
{
    HST01 sHistoryData;
    CHAR dest_str[18];   
	CMyUtils myUtilsObject;
	char strErrorMsg [200] = "";
    CString temp;
       
    CHAR wstation[21]; 
	DWORD wsize;

    char strDate[9] = "";
    char strTime[7] = "";
    char strPrimaryKey[50] = "";

    memset ( strDate, 0, sizeof (strDate));
    memset ( strTime, 0, sizeof (strTime));
    memset ( strPrimaryKey, 0, sizeof (strPrimaryKey));

    strcpy ( strPrimaryKey, m_StrProfileId_Fld);
	strcat ( strPrimaryKey, m_StrType_Fld);//praneeth added to display mcc limit changes in changed history tab
    
    myUtilsObject.GetSystemDate(strDate); 
    myUtilsObject.GetSystemTime(strTime);

    pintime_get_gmt ( dest_str );

    ZeroMemory ( &sHistoryData, sizeof (HST01));
    memcpy ((LPTSTR) sHistoryData.file_name,
        (LPCTSTR)"CAF01",
        sizeof (sHistoryData.file_name));

    temp = strDate;
    if ( temp.GetLength() != 0 )
      temp = temp.Right(4) + temp.Left(2) + temp.Mid(2,2);


    wsize = sizeof( wstation );
    GetComputerName( wstation, &wsize );

	memcpy ((LPTSTR)sHistoryData.reserved,(LPTSTR)wstation, sizeof(sHistoryData.reserved));
    memcpy ((LPTSTR) sHistoryData.date_modified, temp, sizeof (sHistoryData.date_modified));

    memcpy ((LPTSTR) sHistoryData.file_primary_key,
        (LPCTSTR)strPrimaryKey,
        sizeof (sHistoryData.file_primary_key));

    memcpy ((LPTSTR) sHistoryData.primary_key.unique_id,
        (LPCTSTR)dest_str,
        sizeof (sHistoryData.primary_key.unique_id)); 
    
    memcpy ((LPTSTR) sHistoryData.time_modified,
        (LPCTSTR)strTime,
        sizeof (sHistoryData.time_modified)); 
             
    memcpy ((LPTSTR)sHistoryData.current_data,
        (LPCTSTR)m_StrCurrentData,
        sizeof (sHistoryData.current_data));

	memcpy ((LPTSTR)sHistoryData.previous_data,
        (LPCTSTR)m_StrPreviousData,
        sizeof (sHistoryData.previous_data));

	memcpy ((LPTSTR)sHistoryData.changed_by,
        (LPCTSTR)ucf01.primary_key.name,
        sizeof (sHistoryData.changed_by));

	memcpy ((LPTSTR)sHistoryData.field_tag,
        (LPCTSTR)m_StrFieldTag,
        sizeof (sHistoryData.field_tag));
        
    int rcode = txdsapi_insert_record  ( (pBYTE)&sHistoryData, sizeof (HST01), HST01_DATA, strErrorMsg );
	if ( rcode != PTEMSG_OK )
	{
		m_StrMessageLbl = strErrorMsg;
		UpdateData( FALSE );
		m_nDialogMode = MODE_DONE;
	}
    
}

void CCardsMerchantLimit::OnMccAddButton() 
{
	LV_ITEM lvItem;
	CMyUtils myUtilsObject;
	char  error_msg[100] = {0};
	char strTemp[100];
	CString temp;

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
	nCount = m_CntlMCCList.GetItemCount();
	if (nCount >= MAX_MCC_CNT) /*Max codes that can be defined is 500)*/
	{
		sprintf (strTemp, "ERROR: MCC Code count cannot exceed %d. Current count = %d", MAX_MCC_CNT,nCount);
		m_StrMessageLbl = strTemp;
		UpdateData(FALSE);
		return ;
	}

	/*Check for blank mcc code or uses*/
	if(m_StrMCC.IsEmpty())
	{
		m_StrMessageLbl = "MCC Code is required and cannot be empty.";
		m_CntlMCC.SetFocus();
		UpdateData(FALSE);
		return;
	}

	if(m_StrUses.IsEmpty())
	{
		m_StrMessageLbl = "Number of Uses is required and cannot be empty.";
		m_CntlUses.SetFocus();
		UpdateData(FALSE);
		return;
	}

   /* Peso Amount Limit */
	if(m_StrAmount.IsEmpty())
	{
		m_StrMessageLbl = "Peso amount is required and cannot be empty.";
		m_CntlAmount.SetFocus();
		UpdateData(FALSE);
		return;
	}

	int length = m_StrAmount.GetLength();
	int res = strspn ( m_StrAmount, "1234567890." );
	if ( res < length )
	{
		m_CntlAmount.SetFocus();
		m_StrMessageLbl = "Invalid Character.";
		UpdateData(FALSE);
		return;
	}
	
	decpos = m_StrAmount.Find(".");
	
	if (decpos < 0)				//-1 returned if not found
	{
		m_CntlAmount.SetFocus();
		m_StrMessageLbl = "Invalid Peso Amount - Decimal required.";
		UpdateData(FALSE);
		return;
	}
	
	/*Check for 2 decimal places*/
	if(length < decpos + 1 + 2)		/*decpos is zero based index, so add 1 and 2 decimal places*/
	{
		m_CntlAmount.SetFocus();
		m_StrMessageLbl = "Invalid Peso Amount - Two digits after decimal required.";
		UpdateData(FALSE);
		return;
	}

   /* US Dollar Amount Limit */
	if(m_StrUSDLimit.IsEmpty())
	{
		m_StrMessageLbl = "USD amount is required and cannot be empty.";
		m_CntlUSDLimit.SetFocus();
		UpdateData(FALSE);
		return;
	}

	length = m_StrUSDLimit.GetLength();
	res = strspn ( m_StrUSDLimit, "1234567890." );
	if ( res < length )
	{
		m_CntlUSDLimit.SetFocus();
		m_StrMessageLbl = "Invalid Character.";
		UpdateData(FALSE);
		return;
	}
	
	decpos = m_StrUSDLimit.Find(".");
	
	if (decpos < 0)				//-1 returned if not found
	{
		m_CntlUSDLimit.SetFocus();
		m_StrMessageLbl = "Invalid USD Amount - Decimal required.";
		UpdateData(FALSE);
		return;
	}
	
	/*Check for 2 decimal places*/
	if(length < decpos + 1 + 2)		/*decpos is zero based index, so add 1 and 2 decimal places*/
	{
		m_CntlUSDLimit.SetFocus();
		m_StrMessageLbl = "Invalid USD Amount - Two digits after decimal required.";
		UpdateData(FALSE);
		return;
	}


	/*verify MCC code does not already exist*/
	if(nCount > 0) 
	{
		for ( int i = 0; i < nCount; i++ )
		{
		  temp = m_CntlMCCList.GetItemText (i, 0) ;
  		  temp.TrimLeft();
		  temp.TrimRight();

		  if ( strcmp ( temp, m_StrMCC ) == 0)
		  {
					nSelectedIndex = i;
					break;
		  }
		}

		if ( nSelectedIndex != -1 )
		{
			m_StrMessageLbl = "ERROR: MCC Code for Profile already exists";		
	
			if (!UpdateData(FALSE))
			{
				AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
			}
			return ;
		}
	}

	//convert Peso amount for insertion into database and verify amount format entered
	CString dbAmount; //amount in database format
	dbAmount = myUtilsObject.GenerateTranAmount ( m_StrAmount, 12, error_msg );	
	if ( dbAmount == "Error" )
	{
		m_StrMessageLbl = error_msg;
		UpdateData(FALSE);
		return;
	}

	//convert USD amount for insertion into database and verify amount format entered
	CString dbUSAmount; //amount in database format
	dbUSAmount = myUtilsObject.GenerateTranAmount ( m_StrUSDLimit, 12, error_msg );	
	if ( dbUSAmount == "Error" )
	{
		m_StrMessageLbl = error_msg;
		UpdateData(FALSE);
		return;
	}


	UpdateData(TRUE);

	/*Add new mcc/uses to list control*/
	for (int nSubItem = 0; nSubItem < 4; nSubItem++)
	{
		lvItem.mask = LVIF_TEXT;
		lvItem.iItem = (nSubItem == 0)? nRowItem : nActualItem;
		lvItem.iSubItem = nSubItem;
		temp.Empty();


		if(nSubItem == 0)
		{	
			memcpy(strTemp, m_StrMCC, sizeof(strTemp));
			lvItem.pszText = strTemp;
		}
		else
		if(nSubItem == 1)
		{
			memset(strTemp, 0, sizeof(temp));
			memcpy (strTemp, m_StrUses, sizeof(strTemp));
			lvItem.pszText = strTemp;
		}
		else 
		if(nSubItem == 2)
		{
			memset(strTemp, 0, sizeof(temp));
			memcpy(strTemp, m_StrAmount, sizeof(strTemp));
			lvItem.pszText = strTemp;
		}
		else 
		if(nSubItem == 3)
		{
			memset(strTemp, 0, sizeof(temp));
			memcpy(strTemp, m_StrUSDLimit, sizeof(strTemp));
			lvItem.pszText = strTemp;
		}

		if(nSubItem == 0)
			nActualItem = m_CntlMCCList.InsertItem(&lvItem);
		else
			m_CntlMCCList.SetItem(&lvItem); 

		nCount++; /*increment count for new record in list control*/
		m_CntlMCCList.SetFocus();

	}//for insert subfields in row

	//Insert into database
	strcpy( (pCHAR)m_mcc_limits.primary_key.caf_profile_id, m_StrProfileId_Fld);
	strcpy( (pCHAR)m_mcc_limits.primary_key.type, m_StrType_Fld.Left(1));
	strcpy( (pCHAR)m_mcc_limits.primary_key.mcc, m_StrMCC);
	strcpy( (pCHAR)m_mcc_limits.usage, m_StrUses );
	strcpy( (pCHAR)m_mcc_limits.peso_amt_limit, dbAmount);
	strcpy( (pCHAR)m_mcc_limits.usd_amt_limit,  dbUSAmount);

	rcode = txdsapi_insert_record( (pBYTE)&m_mcc_limits, sizeof (MCC_LIMITS),
									MCC_LIMITS_DATA, error_msg );

	if (rcode != PTEMSG_OK )
	{
		 m_StrMessageLbl =  error_msg;
		 if (!UpdateData(FALSE))
			 AfxMessageBox (IDS_UPDATEDATAFAILED, MB_ICONSTOP);
		 return;
	}

	//log to history table
	memcpy ((LPSTR)m_StrCurrentData, m_StrUses , sizeof (m_StrCurrentData));
	memset  ((LPSTR)m_StrPreviousData, 0, sizeof (m_StrPreviousData));
    sprintf((LPSTR)m_StrFieldTag, "MCC Code %s Uses Added", m_StrMCC);
	PopulateHistoryTable(); 


   memcpy ((LPSTR)m_StrCurrentData, m_StrAmount , sizeof (m_StrCurrentData));
   memset  ((LPSTR)m_StrPreviousData, 0, sizeof (m_StrPreviousData));
   sprintf((LPSTR)m_StrFieldTag, "MCC Code %s Peso Amount Added", m_StrMCC);
   PopulateHistoryTable(); 

   memcpy ((LPSTR)m_StrCurrentData, m_StrUSDLimit , sizeof (m_StrCurrentData));
   memset  ((LPSTR)m_StrPreviousData, 0, sizeof (m_StrPreviousData));
   sprintf((LPSTR)m_StrFieldTag, "MCC Code %s USD Amount Added", m_StrMCC);
   PopulateHistoryTable(); 

	m_StrMessageLbl = "Record Added";
	m_CntlUpdateButton.EnableWindow(TRUE);
	m_CntlDeleteButton.EnableWindow(TRUE);

	nCount++; /*increment count for new record in list control*/
	m_CntlMCCList.SetFocus();

	UpdateData(FALSE);

}

void CCardsMerchantLimit::OnMccDeleteButton() 
{
//	LV_ITEM lvItem;
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
	m_CntlMCCList.SetFocus();

	UpdateData (FALSE);

	/*Verify delete?*/
	strMessage.Format ("Are you sure you want to delete MCC code %s ?", (LPCTSTR)m_StrMCC);
	rcode = AfxMessageBox ((LPCTSTR)strMessage, MB_OKCANCEL|MB_ICONQUESTION);
	if (rcode != IDOK)
		return ;

	/*check for blank mcc code*/
	if(m_StrMCC.IsEmpty())
	{
		m_StrMessageLbl = "MCC Code is required and cannot be empty.";
		m_CntlMCC.SetFocus();
		UpdateData(FALSE);
		return;
	}



	/* Delete MCC Code from Database */
	strcpy( (pCHAR)m_mcc_limits.primary_key.caf_profile_id, m_StrProfileId_Fld);
	strcpy( (pCHAR)m_mcc_limits.primary_key.type, m_StrType_Fld.Left(1));
	strcpy( (pCHAR)m_mcc_limits.primary_key.mcc, m_StrMCC);

	rcode = txdsapi_delete_record( (pBYTE)&m_mcc_limits, sizeof (MCC_LIMITS),
									MCC_LIMITS_DATA, error_msg );

	if (rcode != PTEMSG_OK )
	{
		 m_StrMessageLbl =  error_msg;
		 if (!UpdateData(FALSE))
			 AfxMessageBox (IDS_UPDATEDATAFAILED, MB_ICONSTOP);
		 m_CntlMCCList.SetFocus();
		 return;
	}


	//log to history table
	memcpy ((LPSTR)m_StrCurrentData, "Record Deleted", sizeof (m_StrCurrentData));
	memcpy ((LPSTR)m_StrPreviousData, m_StrMCC, sizeof (m_StrPreviousData));
	sprintf ((LPSTR)m_StrFieldTag, "MCC Code %s Deleted", m_StrMCC);
	PopulateHistoryTable();


	/*find row number for selected limit code*/
	nCount = m_CntlMCCList.GetItemCount();
	nSelectedIndex = -1;

	for ( int idx = 0; idx < nCount; idx++ )
	{
		temp = m_CntlMCCList.GetItemText (idx, 0) ;
		temp.TrimLeft();
		temp.TrimRight();
		if ( strcmp ( temp, m_StrMCC ) == 0)
		{
			nSelectedIndex = idx;
			break;
		}
	}	
	
	if (nSelectedIndex == -1)
	{
		m_StrMessageLbl = "ERROR: Invalid Limit Code";
		UpdateData(FALSE);
		return;
	}

	m_CntlMCCList.DeleteItem(nSelectedIndex);
	m_StrMessageLbl = "Record Deleted";
	UpdateData(FALSE);
	
}

void CCardsMerchantLimit::OnMccUpdateButton() 
{
	
	int		nCount = 0;
	int		rcode = 0;
	int	    length = 0;
	int	    res = 0;
	char    error_msg[100] = {0};

	CString temp;
	CMyUtils myUtilsObject;


	UpdateData (TRUE);
    m_StrMessageLbl = "";
	UpdateData (FALSE);
	SetModified(TRUE);//enable apply button

	// check for blank MCC code/ Uses
	if(m_StrMCC.IsEmpty())
	{
		m_StrMessageLbl = "MCC Code is required and cannot be empty.";
		m_CntlMCC.SetFocus();
		UpdateData(FALSE);
		return;
	}

	if(m_StrUses.IsEmpty())
	{
		m_CntlUses.SetFocus();
		m_StrMessageLbl = "Number of Uses is required and cannot be empty.";
		UpdateData(FALSE);
		return;

	}

	if(m_StrAmount.IsEmpty())
	{
		m_CntlAmount.SetFocus();
		m_StrMessageLbl = "Peso Amount is required and cannot be empty.";
		UpdateData(FALSE);
		return;

	}

	if(m_StrUSDLimit.IsEmpty())
	{
		m_CntlUSDLimit.SetFocus();
		m_StrMessageLbl = "USD Amount is required and cannot be empty.";
		UpdateData(FALSE);
		return;

	}

	/*Verify Peso amount format*/
	length = m_StrAmount.GetLength();
	res = strspn ( m_StrAmount, "1234567890." );
	if ( res < length )
	{
		m_CntlAmount.SetFocus();
		m_StrMessageLbl = "Invalid Character.";
		UpdateData(FALSE);
		return;
	}
	
	if (m_StrAmount.Find(".") < 0)		//-1 returned if not found
	{
		m_CntlAmount.SetFocus();
		m_StrMessageLbl = "Invalid Peso Amount - Decimal required.";
		UpdateData(FALSE);
		return;
	}

	/*Verify USD amount format*/
	length = m_StrUSDLimit.GetLength();
	res = strspn ( m_StrUSDLimit, "1234567890." );
	if ( res < length )
	{
		m_CntlUSDLimit.SetFocus();
		m_StrMessageLbl = "Invalid Character.";
		UpdateData(FALSE);
		return;
	}
	
	if (m_StrUSDLimit.Find(".") < 0)		//-1 returned if not found
	{
		m_CntlUSDLimit.SetFocus();
		m_StrMessageLbl = "Invalid USD Amount - Decimal required.";
		UpdateData(FALSE);
		return;
	}


	//convert Peso amount for insertion into database and verify amount format entered
	CString dbAmount; //amount in database format
	dbAmount = myUtilsObject.GenerateTranAmount ( m_StrAmount, 12, error_msg );	
	if ( dbAmount == "Error" )
	{
		m_StrMessageLbl = error_msg;
		UpdateData(FALSE);
		return;
	}

	//convert USD amount for insertion into database and verify amount format entered
	CString dbUSAmount; //amount in database format
	dbUSAmount = myUtilsObject.GenerateTranAmount ( m_StrUSDLimit, 12, error_msg );	
	if ( dbUSAmount == "Error" )
	{
		m_StrMessageLbl = error_msg;
		UpdateData(FALSE);
		return;
	}


	//Insert into database
	strcpy( (pCHAR)m_mcc_limits.primary_key.caf_profile_id, m_StrProfileId_Fld);
	strcpy( (pCHAR)m_mcc_limits.primary_key.type, m_StrType_Fld.Left(1));
	strcpy( (pCHAR)m_mcc_limits.primary_key.mcc, m_StrMCC);
	strcpy( (pCHAR)m_mcc_limits.usage, m_StrUses );
	strcpy( (pCHAR)m_mcc_limits.peso_amt_limit, dbAmount);
	strcpy( (pCHAR)m_mcc_limits.usd_amt_limit,  dbUSAmount);

	rcode = txdsapi_update_record( (pBYTE)&m_mcc_limits, sizeof (MCC_LIMITS),
									MCC_LIMITS_DATA, error_msg );

	if (rcode != PTEMSG_OK )
	{
		 m_StrMessageLbl =  error_msg;
		 if (!UpdateData(FALSE))
			 AfxMessageBox (IDS_UPDATEDATAFAILED, MB_ICONSTOP);
		 return;
	}


	nCount = m_CntlMCCList.GetItemCount();
	nSelectedIndex = -1;

	for ( int idx = 0; idx < nCount; idx++ )//find row number for selected limit code
	{
		temp = m_CntlMCCList.GetItemText (idx, 0) ;
		temp.TrimLeft();
		temp.TrimRight();
		if ( strcmp ( temp, m_StrMCC ) == 0)
		{
			nSelectedIndex = idx;
			break;
		}
	}	
	
	if (nSelectedIndex == -1)
	{
		m_StrMessageLbl = "ERROR: Invalid Limit Code";
		UpdateData(FALSE);
		return;
	}


   //log to history table
   memcpy ((LPSTR)m_StrCurrentData, m_StrUses , sizeof (m_StrCurrentData));
   memcpy ((LPSTR)m_StrPreviousData, m_CntlMCCList.GetItemText(nSelectedIndex, 1), sizeof (m_StrPreviousData));
   sprintf((LPSTR)m_StrFieldTag, "MCC Code %s Uses", m_StrMCC);
   PopulateHistoryTable(); 

   memcpy ((LPSTR)m_StrCurrentData, m_StrAmount , sizeof (m_StrCurrentData));
   memcpy ((LPSTR)m_StrPreviousData, m_CntlMCCList.GetItemText(nSelectedIndex, 2), sizeof (m_StrPreviousData));
   sprintf((LPSTR)m_StrFieldTag, "MCC Code %s Peso Amount", m_StrMCC);
   PopulateHistoryTable(); 

   memcpy ((LPSTR)m_StrCurrentData, m_StrUSDLimit , sizeof (m_StrCurrentData));
   memcpy ((LPSTR)m_StrPreviousData, m_CntlMCCList.GetItemText(nSelectedIndex, 3), sizeof (m_StrPreviousData));
   sprintf((LPSTR)m_StrFieldTag, "MCC Code %s USD Amount", m_StrMCC);
   PopulateHistoryTable(); 

   //Set new List Box Text
   m_CntlMCCList.SetItemText ( nSelectedIndex, 1, m_StrUses     );
   m_CntlMCCList.SetItemText ( nSelectedIndex, 2, m_StrAmount   );
   m_CntlMCCList.SetItemText ( nSelectedIndex, 3, m_StrUSDLimit );
   m_StrMessageLbl = "Record Updated";
   UpdateData(FALSE);
	
}

void CCardsMerchantLimit::OnDblclkMcclistCtrl(NMHDR* pNMHDR, LRESULT* pResult) 
{
	CString temp;

	if(m_nDialogMode != MODE_VIEW)
	{
		nSelectedIndex = m_CntlMCCList.GetNextItem (-1, LVNI_SELECTED) ;
		
		temp  = m_CntlMCCList.GetItemText(nSelectedIndex, 0);
  		temp.TrimLeft();
		temp.TrimRight();
		m_StrMCC = temp;

      m_StrUses     = m_CntlMCCList.GetItemText(nSelectedIndex, 1);
      m_StrAmount   = m_CntlMCCList.GetItemText(nSelectedIndex, 2); 
      m_StrUSDLimit = m_CntlMCCList.GetItemText(nSelectedIndex, 3);
      m_CntlMCC.SetFocus();

      UpdateData(FALSE);	
   }

   *pResult = 0;	
}

void CCardsMerchantLimit::OnClickMcclistCtrl(NMHDR* pNMHDR, LRESULT* pResult) 
{
	CString temp;

	if(m_nDialogMode != MODE_VIEW)
	{
		nSelectedIndex = m_CntlMCCList.GetNextItem (-1, LVNI_SELECTED) ;
		
		temp  = m_CntlMCCList.GetItemText(nSelectedIndex, 0);
  		temp.TrimLeft();
		temp.TrimRight();
		m_StrMCC = temp;

      m_StrUses     = m_CntlMCCList.GetItemText(nSelectedIndex, 1);
      m_StrAmount   = m_CntlMCCList.GetItemText(nSelectedIndex, 2);
      m_StrUSDLimit = m_CntlMCCList.GetItemText(nSelectedIndex, 3);
      m_CntlMCC.SetFocus();

      UpdateData(FALSE);

   }

   *pResult = 0;
}

void CCardsMerchantLimit::OnReturnMcclistCtrl(NMHDR* pNMHDR, LRESULT* pResult) 
{
	CString temp;
	UpdateData(TRUE);

	nSelectedIndex = m_CntlMCCList.GetNextItem (-1, LVNI_SELECTED) ;
	
	temp  = m_CntlMCCList.GetItemText(nSelectedIndex, 0);
  	temp.TrimLeft();
	temp.TrimRight();
	m_StrMCC = temp;

   m_StrUses     = m_CntlMCCList.GetItemText(nSelectedIndex, 1);
   m_StrAmount   = m_CntlMCCList.GetItemText(nSelectedIndex, 2);
   m_StrUSDLimit = m_CntlMCCList.GetItemText(nSelectedIndex, 3);
   m_CntlMCC.SetFocus();

   UpdateData(FALSE);	

   *pResult = 0;
}

/*void CCardsMerchantLimit::OnMccselect() 
{
	CString temp;

	nSelectedIndex = m_CntlMCCList.GetNextItem (-1, LVNI_SELECTED) ;
	
	temp  = m_CntlMCCList.GetItemText(nSelectedIndex, 0);
  	temp.TrimLeft();
	temp.TrimRight();
	m_StrMCC = temp;
	
	m_StrUses = m_CntlMCCList.GetItemText(nSelectedIndex, 1);
	m_CntlMCC.SetFocus();

	UpdateData(FALSE);		
}  */

void CCardsMerchantLimit::OnKeydownMcclistCtrl(NMHDR* pNMHDR, LRESULT* pResult) 
{
   LV_KEYDOWN* pLVKeyDow = (LV_KEYDOWN*)pNMHDR;

   CString temp;

   UpdateData(TRUE);
   if(pLVKeyDow->wVKey == 0x20)//space bar pressed
   {
      if(m_nDialogMode != MODE_VIEW)
      {
         nSelectedIndex = m_CntlMCCList.GetNextItem (-1, LVNI_FOCUSED) ;

         temp  = m_CntlMCCList.GetItemText(nSelectedIndex, 0);
         temp.TrimLeft();
         temp.TrimRight();
         m_StrMCC = temp;
         m_StrUses     = m_CntlMCCList.GetItemText(nSelectedIndex, 1);
         m_StrAmount   = m_CntlMCCList.GetItemText(nSelectedIndex, 2);
         m_StrUSDLimit = m_CntlMCCList.GetItemText(nSelectedIndex, 3);

         m_CntlUses.SetFocus();

         UpdateData(FALSE);
      }
   }

   *pResult = 0;


}

