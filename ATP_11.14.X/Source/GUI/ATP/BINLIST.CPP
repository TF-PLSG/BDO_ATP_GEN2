/*********************************************************************************
 Module: BinList.cpp : implementation file

 Copyright (c) 2002, Hypercom, Inc.                         
 All Rights Reserved.                                       

 Description: Logic for Bin List property page
			  This module defines and uses NegativeCardFileControl class

 Application: ATP

 Author: Irina Arustamova   

 Revision History:
  
 $Log:   N:\POS\PVCS6.6\EPICPORTZ\PTE\Equitable\ATP_gui\BINLIST.CPP  $
   
      Rev 1.15   Apr 01 2005 10:13:28   lmitchel
   Initial Checking prior to integration testing for updates made to support country/curreny code limits (scr13929), unulimited purchase card MCC ranges, and purchase card transaction limits(scr12785)
   
      Rev 1.14   Dec 01 2003 11:28:42   lmitchel
   Added AMEX Online Updates - scr1200
   
      Rev 1.13   Aug 15 2002 17:12:24   dirby
   Modified to support a new bin type called DEFERRED.
   SCR 551
   
      Rev 1.12   Mar 16 2000 10:38:12   iarustam
   Added alpha lookup screens and updated
   Merchant main, Card Record main and 
   primary account main screens
   
      Rev 1.11   Mar 02 2000 11:56:02   iarustam
   SCR # 425,21,383,407,459,300,460,461
   
      Rev 1.10   Dec 16 1999 09:34:02   mbysani
   corrected some error based on error reports
   
      Rev 1.3   Oct 15 1999 17:02:22   iarustam
   bug fixes
   
      Rev 1.2   Sep 30 1999 16:16:46   npogosya
   Bug fixes
   
      Rev 1.1   Sep 22 1999 15:39:42   iarustam
   Added manual batch entry GUI
   
      Rev 1.0   Jul 01 1999 14:16:46   iarustam
   Initial Release
   

    

************************************************************************************/

// BinList.cpp : implementation file
//

#include "stdafx.h"
#include "Atp.h"
#include "BinList.h"
#include "TransactionDetails.h"
#include "MyUtils.h"
#include "BatchDetails.h"
#include "SharedMem.h"
extern "C"{
   //#include "Txutils.h"
   #include "PteTime.h"
   //#include "ntutils.h"
}

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CString strLength;
CString strLow;
CString strBinLow;
CString strBinHigh;
CString strType1;
CString strMerchantID;
CString strOrganizationID;
CString Stronbehalfservice;
CString	Stronbehalfresult;
CString	Strresponsecode;
CString	Strdescription;
CString	StrCAVV;
CString StrRule_ID;
CString StrException_cardnum;
CString	StrCAVVresponsecode;
CString	StrCAVVdescription;
CString Str3DS_Authentiction_RC_Value;
CString Str3DS_Authentiation_Deline_Response_Code;
CString Str3DS_AUTHENTICATION_VERIFY_FLAG;
CString StrCurrency_Value;
CString StrCurrency_code;
CString StrDecimal_places;
CString StrCurrency_Name;
CString StrCard_brand;
CString StrBDK_key;



extern CSharedMem shrmem;
extern MIDBINBLK01	sMidBinBlkRecord_Global ;
extern MIDBINBLK01 sOldMidBinBlkRecord_Global;
/////////////////////////////////////////////////////////////////////////////
// CBinList dialog


CBinList::CBinList(CWnd* pParent /*=NULL*/)
	: CDialog(CBinList::IDD, pParent)
{
	//{{AFX_DATA_INIT(CBinList)
	m_StrMessageLbl = _T("");
	//}}AFX_DATA_INIT
}


void CBinList::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CBinList)
	DDX_Control(pDX, IDC_DETAILS_BUTTON, m_DetailsButton);
	DDX_Control(pDX, IDC_NEXT_BUTTON, m_CntlNextButton);
	DDX_Control(pDX, IDC_NAME_LIST, m_CntlList);
	DDX_Text(pDX, IDC_MESSAGE_LBL, m_StrMessageLbl);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CBinList, CDialog)
	//{{AFX_MSG_MAP(CBinList)
	ON_BN_CLICKED(IDC_NEXT_BUTTON, OnNextButton)
	ON_NOTIFY(NM_DBLCLK, IDC_NAME_LIST, OnDblclkNameList)
	ON_BN_CLICKED(IDC_DETAILS_BUTTON, OnDetailsButton)	
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBinList message handlers

BOOL CBinList::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	LV_COLUMN lvColumn ;
	CFont m_font;
	LOGFONT lf;
	CMyUtils myUtilsObject;

	int nHeader;
	char strHeaderData[5][50] = { 0 };
	char strHeaderData1[5][50] = { 0 };
	char strHeaderData2[15][50] = { 0 };
	char strHeaderData3[15][50] = { 0 };
	int nST2 = 0;
	
	memset ( &lf, 0, sizeof (lf));
	lf.lfHeight = 12;
	lf.lfWeight = 700;
	strcpy(lf.lfFaceName, "Arial");
	m_font.CreateFontIndirect (&lf);
	GetDlgItem(IDC_MESSAGE_LBL)->SetFont (&m_font);

	strMerchantID = "";
	strOrganizationID = "";
	m_bParam = FALSE;

/*	if ( m_nDialogMode != MODE_BATCH  )
	{
		m_DetailsButton.ShowWindow(SW_HIDE);
		m_AddButton.ShowWindow(SW_HIDE);
		m_DeleteButton.ShowWindow(SW_HIDE);
	}
   else if ( m_nViewMode == MODE_VIEW )
   {
		m_AddButton.EnableWindow(FALSE);
		m_DeleteButton.EnableWindow(FALSE);
   }
   else if ( m_nDialogMode != MODE_BATCH_HISTORY )
	{
		m_DetailsButton.ShowWindow(SW_HIDE);
		m_AddButton.ShowWindow(SW_HIDE);
		m_DeleteButton.ShowWindow(SW_HIDE);
	}
  */
	if ( m_nDialogMode == MODE_BATCH || m_nDialogMode == MODE_BATCH_HISTORY )
      m_DetailsButton.ShowWindow(SW_SHOW);

/*   if ( m_nDialogMode == MODE_BATCH )
	   {
		   m_AddButton.ShowWindow(SW_SHOW);
		   m_DeleteButton.ShowWindow(SW_SHOW);
	   }*/
     
	switch ( m_nDialogMode )
	{
	case MODE_RELATED:
	
		memcpy (strHeaderData[0],"       ACCOUNT NUMBER      ", sizeof (strHeaderData[0]));
		memcpy (strHeaderData[1],"   TYPE        ", sizeof (strHeaderData[1])) ;

		for (nHeader = 0; nHeader < 2; nHeader++) { 
			lvColumn.mask = LVCF_FMT | LVCF_SUBITEM | LVCF_TEXT  ;
			lvColumn.fmt = LVCFMT_LEFT ;
			lvColumn.pszText = strHeaderData[nHeader] ;
			lvColumn.iSubItem = nHeader ;
			
			m_CntlList.InsertColumn(nHeader, &lvColumn) ;  // assumes return value is OK.
			m_CntlList.SetColumnWidth (0, LVSCW_AUTOSIZE_USEHEADER) ;
			m_CntlList.SetColumnWidth (1, LVSCW_AUTOSIZE_USEHEADER) ;
			
		}


		SetWindowText ( "Related Account List");    

		ZeroMemory ( &sACFRecord, sizeof (ACF01));

		if ( myUtilsObject.IsEmpty ( strItem0, this ) && myUtilsObject.IsEmpty ( strItem1, this ))
		{
			strcpy((char *)sACFRecord.primary_key.account_type, " ");
			strcpy((char *)sACFRecord.primary_key.account_nbr, " ");
			nST2 = 0;
		}
		else if ( !myUtilsObject.IsEmpty ( strItem0, this ) && !myUtilsObject.IsEmpty ( strItem1, this ))
		{
			strcpy((char *)sACFRecord.primary_key.account_nbr, strItem0);
			strcpy((char *)sACFRecord.primary_key.account_type, strItem1);
			nST2 = 0;
		}
		else if ( !myUtilsObject.IsEmpty ( strItem0, this ) && myUtilsObject.IsEmpty ( strItem1, this ))
 
		{
			strcpy ((char *)sACFRecord.primary_key.account_nbr, strItem0);
			strcpy((char *)sACFRecord.primary_key.account_type, "05");
			nST2 = 0;
		}
		else if ( myUtilsObject.IsEmpty ( strItem0, this ) && !myUtilsObject.IsEmpty ( strItem1, this ))
 
		{
			strcpy((char *)sACFRecord.primary_key.account_nbr, " ");
			strcpy ((char *)sACFRecord.primary_key.account_type, strItem1);
			nST2 = ST2_DB_SELECT_LIST_BY_PARAM;
			m_bParam = TRUE;
		}
		
		PopulateACFListDialog(nST2);

	break;
	/////////////////////////////////////////////////////////////////////
/*		case MODE_PURCHASE:
	
		memcpy (strHeaderData[0],"    CARD NUMBER    ", sizeof (strHeaderData[0]));
		memcpy (strHeaderData[1],"CARD TYPE  ", sizeof (strHeaderData[1])) ;
		memcpy (strHeaderData[2],"MCC LOW    ", sizeof (strHeaderData[2]));
		memcpy (strHeaderData[3],"MCC HIGH   ", sizeof (strHeaderData[3]));

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


		SetWindowText ( "Purchase Card List");    

		ZeroMemory ( &sCCFRecord, sizeof (CCF04));

		
		if ( myUtilsObject.IsEmpty ( strItem0, this ) && myUtilsObject.IsEmpty ( strItem1, this ))
		{
			strcpy((char *)sCCFRecord.primary_key.card_nbr, " ");
			strcpy((char *)sCCFRecord.primary_key.card_type, " ");
			nST2 = 0;
		}
		else if ( !myUtilsObject.IsEmpty ( strItem0, this ) && !myUtilsObject.IsEmpty ( strItem1, this ))
		{
			strcpy((char *)sCCFRecord.primary_key.card_nbr, strItem0);
			strcpy((char *)sCCFRecord.primary_key.card_type, strItem1);
			nST2 = 0;
		}
		else if ( !myUtilsObject.IsEmpty ( strItem0, this ) && myUtilsObject.IsEmpty ( strItem1, this ))
 
		{
			strcpy ((char *)sCCFRecord.primary_key.card_nbr, strItem0);
			strcpy((char *)sCCFRecord.primary_key.card_type, "C");
			nST2 = 0;
		}
		else if ( myUtilsObject.IsEmpty ( strItem0, this ) && !myUtilsObject.IsEmpty ( strItem1, this ))
 
		{
			strcpy((char *)sCCFRecord.primary_key.card_nbr, " ");
			strcpy ((char *)sCCFRecord.primary_key.card_type, strItem1);
			nST2 = ST2_DB_SELECT_LIST_BY_PARAM;
			m_bParam = TRUE;
		}
		PopulateCCF02ListDialog( nST2 );

	break;
*/
		/////////////////////////////////////////////////////////////////////
		case MODE_DEFERRED:
	
		memcpy (strHeaderData3[0],"    MERCHANT ID   ", sizeof (strHeaderData[0]));
		memcpy (strHeaderData3[1],"ORGANIZATION ID  ", sizeof (strHeaderData[1])) ;
		memcpy (strHeaderData3[2],"TERM LENGTH  ", sizeof (strHeaderData[2])) ;

		for (nHeader = 0; nHeader < 3; nHeader++) { 
			lvColumn.mask = LVCF_FMT | LVCF_SUBITEM | LVCF_TEXT  ;
			lvColumn.fmt = LVCFMT_LEFT ;
			lvColumn.pszText = strHeaderData3[nHeader] ;
			lvColumn.iSubItem = nHeader ;
			
			m_CntlList.InsertColumn(nHeader, &lvColumn) ;  // assumes return value is OK.
			m_CntlList.SetColumnWidth (0, LVSCW_AUTOSIZE_USEHEADER) ;
			m_CntlList.SetColumnWidth (1, LVSCW_AUTOSIZE_USEHEADER) ;
			m_CntlList.SetColumnWidth (2, LVSCW_AUTOSIZE_USEHEADER) ;			
		}


		SetWindowText ( "Deferred Purchase List");    

		ZeroMemory ( &sDEFRecord, sizeof (DEF01));

		strcpy((char *)sDEFRecord.primary_key.merchant_id, " ");
		strcpy((char *)sDEFRecord.primary_key.organization_id, " ");
		strcpy((char *)sDEFRecord.primary_key.term_length, " ");
		
		PopulateDEFListDialog();

	break;

	/////////////////////////////////////////////////////////////////////
	case MODE_PRIMACC:
	
		memcpy (strHeaderData[0],"    PRIMARY ACCT NBR    ", sizeof (strHeaderData[0]));
		memcpy (strHeaderData[1],"NAME                  ", sizeof (strHeaderData[1])) ;

		for (nHeader = 0; nHeader < 2; nHeader++) { 
			lvColumn.mask = LVCF_FMT | LVCF_SUBITEM | LVCF_TEXT  ;
			lvColumn.fmt = LVCFMT_LEFT ;
			lvColumn.pszText = strHeaderData[nHeader] ;
			lvColumn.iSubItem = nHeader ;
			
			m_CntlList.InsertColumn(nHeader, &lvColumn) ;  // assumes return value is OK.
			m_CntlList.SetColumnWidth (0, LVSCW_AUTOSIZE_USEHEADER) ;
			m_CntlList.SetColumnWidth (1, LVSCW_AUTOSIZE_USEHEADER) ;
			
		}


		SetWindowText ( "PAN List");    

		ZeroMemory ( &sPAFRecord, sizeof (PAF01));

		if ( myUtilsObject.IsEmpty ( strItem0, this ) )
			strcpy((char *)sPAFRecord.primary_key.primary_acct_nbr, "  ");
		else
			strcpy((char *)sPAFRecord.primary_key.primary_acct_nbr, strItem0);

		
		PopulatePAFListDialog();

	break;
	/////////////////////////////////////////////////////////////////////
    
	case MODE_PROFID: case MODE_PROFIDBYTYPE:
		memcpy (strHeaderData[0],"VIP PROFILE    ", sizeof (strHeaderData[0]));
		memcpy (strHeaderData[1],"TYPE             ", sizeof (strHeaderData[1])) ;

	//	strHeaderData[2][50] = {"VIP PROFILE    ", "TYPE             "} ;

		for (nHeader = 0; nHeader < 2; nHeader++) { 
			lvColumn.mask = LVCF_FMT | LVCF_SUBITEM | LVCF_TEXT  ;
			lvColumn.fmt = LVCFMT_LEFT ;
			lvColumn.pszText = strHeaderData[nHeader] ;
			lvColumn.iSubItem = nHeader ;
			
			m_CntlList.InsertColumn(nHeader, &lvColumn) ;  // assumes return value is OK.
			m_CntlList.SetColumnWidth (0, LVSCW_AUTOSIZE_USEHEADER) ;
			m_CntlList.SetColumnWidth (1, LVSCW_AUTOSIZE_USEHEADER) ;
			
		}


		SetWindowText ( "VIP Profile List");    

		ZeroMemory ( &sCAFRecord, sizeof (CAF01));

		strcpy((char *)sCAFRecord.primary_key.caf_profile_id, "  ");
		if (m_nDialogMode == MODE_PROFID)
			strcpy((char *)sCAFRecord.primary_key.type, " ");
		else
			strcpy((char *)sCAFRecord.primary_key.type, m_strType);
		
		PopulateCAFListDialog();

	break;
	/////////////////////////////////////////////////////////////////////
	 
    case MODE_NEGBIN:
		memcpy (strHeaderData[0],"     LOW             ", sizeof (strHeaderData[0]));
		memcpy (strHeaderData[1],"     HIGH            ", sizeof (strHeaderData[1])) ;
	
//		strHeaderData[2][50] = {"LOW             ", "HIGH            "} ;

		for (nHeader = 0; nHeader < 2; nHeader++) { 
			lvColumn.mask = LVCF_FMT | LVCF_SUBITEM | LVCF_TEXT  ;
			lvColumn.fmt = LVCFMT_LEFT ;
			lvColumn.pszText = strHeaderData[nHeader] ;
			lvColumn.iSubItem = nHeader ;
			
			m_CntlList.InsertColumn(nHeader, &lvColumn) ;  // assumes return value is OK.
			m_CntlList.SetColumnWidth (0, LVSCW_AUTOSIZE_USEHEADER) ;
			m_CntlList.SetColumnWidth (1, LVSCW_AUTOSIZE_USEHEADER) ;
			
		}


		SetWindowText ( "Negative BIN List");    

		ZeroMemory ( &sNegBinRecord, sizeof (NBF01));

		if ( myUtilsObject.IsEmpty ( strItem0, this ) )
			strcpy((char *)sNegBinRecord.primary_key.low, " ");
		else
			strcpy((char *)sNegBinRecord.primary_key.low, strItem0);

		if ( myUtilsObject.IsEmpty ( strItem1, this ) )
			strcpy((char *)sNegBinRecord.primary_key.high, " ");
		else
			strcpy((char *)sNegBinRecord.primary_key.high, strItem1);
		
		PopulateNegBinListDialog();

	break;
	////////////////////////////////////////////////////////////////////////
	
	case MODE_STATUS:
	
//		strHeaderData[2][50] = {"STATUS    ", "DESCRIPTION           "} ;
		memcpy (strHeaderData[0],"STATUS    ", sizeof (strHeaderData[0]));
		memcpy (strHeaderData[1],"DESCRIPTION           ", sizeof (strHeaderData[1])) ;

		for (nHeader = 0; nHeader < 2; nHeader++) { 
			lvColumn.mask = LVCF_FMT | LVCF_SUBITEM | LVCF_TEXT  ;
			lvColumn.fmt = LVCFMT_LEFT ;
			lvColumn.pszText = strHeaderData[nHeader] ;
			lvColumn.iSubItem = nHeader ;
			
			m_CntlList.InsertColumn(nHeader, &lvColumn) ;  // assumes return value is OK.
			m_CntlList.SetColumnWidth (0, LVSCW_AUTOSIZE_USEHEADER) ;
			m_CntlList.SetColumnWidth (1, LVSCW_AUTOSIZE_USEHEADER) ;
			
		}


		SetWindowText ( "Status List");    

		ZeroMemory ( &sCSFRecord, sizeof (CSF01));

		strcpy((char *)sCSFRecord.primary_key.status, " ");
		
		PopulateCSFListDialog();

	break;
	/////////////////////////////////////////////////////////////////////////
    case MODE_OMC01:
	
//		strHeaderData[2][50] = {"STATUS    ", "DESCRIPTION           "} ;
		memcpy (strHeaderData2[0],"MERCHANT ID       ", sizeof (strHeaderData2[0]));
		memcpy (strHeaderData2[1],"CARD NUMBER      ", sizeof (strHeaderData2[1]));
		memcpy (strHeaderData2[2],"EXPIRES ON       ", sizeof (strHeaderData2[2]));
		memcpy (strHeaderData2[3],"NO OF TIMES      ", sizeof (strHeaderData2[3]));
		memcpy (strHeaderData2[4],"AMOUNT(PESO)     ", sizeof (strHeaderData2[4]));
		memcpy (strHeaderData2[5],"AMOUNT(USD)      ", sizeof (strHeaderData2[5]));
		memcpy (strHeaderData2[6],"BYPASS MID CAPPING       ", sizeof (strHeaderData2[6]));
		memcpy (strHeaderData2[7],"BYPASS MCC CAPPING       ", sizeof (strHeaderData2[7]));
		memcpy (strHeaderData2[8],"BYPASS GLOBAL PARAMETER CAPPING       ", sizeof (strHeaderData2[8]));

		for (nHeader = 0; nHeader < 9; nHeader++) { 
			lvColumn.mask = LVCF_FMT | LVCF_SUBITEM | LVCF_TEXT  ;
			lvColumn.fmt = LVCFMT_LEFT ;
			lvColumn.pszText = strHeaderData2[nHeader] ;
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
		}

		SetWindowText ( "Status List");    

		ZeroMemory ( &sOMC01Record, sizeof (OMC01));

		if ( myUtilsObject.IsEmpty ( strItem0, this ) )
			strcpy((char *)sOMC01Record.primary_key.merchant_id, " " );
		else
			strcpy((char *)sOMC01Record.primary_key.merchant_id, strItem0);
		if ( myUtilsObject.IsEmpty ( strItem1, this ) )
			strcpy((char *)sOMC01Record.primary_key.card_nbr, " ");
		else
			strcpy((char *)sOMC01Record.primary_key.card_nbr, strItem1 );		
		PopulateOMCListDialog();

	break;
	/////////////////////////////////////////////////////////////////////////
    
	case MODE_CCF:
	case MODE_PURCHASE:
	
		memcpy (strHeaderData[0],"         CARD NUMBER      ", sizeof (strHeaderData[0]));
		memcpy (strHeaderData[1],"CARD TYPE        ", sizeof (strHeaderData[1])) ;

		for (nHeader = 0; nHeader < 2; nHeader++) { 
			lvColumn.mask = LVCF_FMT | LVCF_SUBITEM | LVCF_TEXT  ;
			lvColumn.fmt = LVCFMT_LEFT ;
			lvColumn.pszText = strHeaderData[nHeader] ;
			lvColumn.iSubItem = nHeader ;
			
			m_CntlList.InsertColumn(nHeader, &lvColumn) ;  // assumes return value is OK.
			m_CntlList.SetColumnWidth (0, LVSCW_AUTOSIZE_USEHEADER) ;
			m_CntlList.SetColumnWidth (1, LVSCW_AUTOSIZE_USEHEADER) ;
			
		}


		SetWindowText ( "Card List");    

		ZeroMemory ( &sCCF02Record, sizeof (CCF02P));

		if ( myUtilsObject.IsEmpty ( strItem0, this ) && myUtilsObject.IsEmpty ( strItem1, this ))
		{
			strcpy((char *)sCCF02Record.primary_key.card_type, " ");
			strcpy((char *)sCCF02Record.primary_key.card_nbr, " ");
			nST2 = 0;
		}
		else if ( !myUtilsObject.IsEmpty ( strItem0, this ) && !myUtilsObject.IsEmpty ( strItem1, this ))
		{
			strcpy((char *)sCCF02Record.primary_key.card_type, strItem0);
			strcpy((char *)sCCF02Record.primary_key.card_nbr, strItem1);
			nST2 = ST2_DB_SELECT_LIST_BY_PARAM;
			m_bParam = TRUE;
		}
		else if ( !myUtilsObject.IsEmpty ( strItem0, this ) && myUtilsObject.IsEmpty ( strItem1, this ))
 
		{
			strcpy ((char *)sCCF02Record.primary_key.card_type, strItem0);
			strcpy((char *)sCCF02Record.primary_key.card_nbr, " ");
			nST2 = ST2_DB_SELECT_LIST_BY_PARAM;
			m_bParam = TRUE;
		}
		else if ( myUtilsObject.IsEmpty ( strItem0, this ) && !myUtilsObject.IsEmpty ( strItem1, this ))
 
		{
			strcpy((char *)sCCF02Record.primary_key.card_type, "C");
			strcpy ((char *)sCCF02Record.primary_key.card_nbr, strItem1);
			nST2 = 0;
		}
		
		PopulateCCF02ListDialog(nST2);

	break;
	///////////////////////////////////////////////////////////////////////

	case MODE_BIN:
	case MODE_RULE_GENERAL:
	 
//		strHeaderData1[5][50] = {"DESCRIPTION  ", "LOW          ","HIGH       ", "LENGTH", "TYPE  "} ;
		memcpy (strHeaderData1[0],"LOW          ", sizeof (strHeaderData[1])) ;
		memcpy (strHeaderData1[1],"HIGH         ", sizeof (strHeaderData[2]));
		memcpy (strHeaderData1[2],"LENGTH", sizeof (strHeaderData[3])) ;
		memcpy (strHeaderData1[3],"TYPE    ", sizeof (strHeaderData[4]));
		memcpy (strHeaderData1[4],"DESCRIPTION  ", sizeof (strHeaderData[0]));

		for (nHeader = 0; nHeader < 5; nHeader++) { 
			lvColumn.mask = LVCF_FMT | LVCF_SUBITEM | LVCF_TEXT  ;
			lvColumn.fmt = LVCFMT_LEFT ;
			lvColumn.pszText = strHeaderData1[nHeader] ;
			lvColumn.iSubItem = nHeader ;
			
			m_CntlList.InsertColumn(nHeader, &lvColumn) ;  // assumes return value is OK.
			m_CntlList.SetColumnWidth (0, LVSCW_AUTOSIZE_USEHEADER) ;
			m_CntlList.SetColumnWidth (1, LVSCW_AUTOSIZE_USEHEADER) ;
			m_CntlList.SetColumnWidth (2, LVSCW_AUTOSIZE_USEHEADER) ;
			m_CntlList.SetColumnWidth (3, LVSCW_AUTOSIZE_USEHEADER) ;
			m_CntlList.SetColumnWidth (4, LVSCW_AUTOSIZE_USEHEADER) ;
		}


		SetWindowText ( "BIN Range List");    

		ZeroMemory ( &sBinRecord, sizeof (UCF01));


		if ( myUtilsObject.IsEmpty ( strItem0, this ) )
		{
			strcpy((char *)sBinRecord.primary_key.length, " ");
		}
		else 
		{
			strcpy((char *)sBinRecord.primary_key.length, strItem0);
		}
		if ( myUtilsObject.IsEmpty ( strItem1, this ) )
		{
			strcpy((char *)sBinRecord.primary_key.low, " ");
		}
		else 
			strcpy((char *)sBinRecord.primary_key.low, strItem1);

		if ( myUtilsObject.IsEmpty ( strItem2, this ) )
		{
			strcpy((char *)sBinRecord.primary_key.type, " ");
		}
		else 
			strcpy((char *)sBinRecord.primary_key.type, strItem2);

		if ( myUtilsObject.IsEmpty ( strItem0, this ) && myUtilsObject.IsEmpty ( strItem1, this )
			      && !myUtilsObject.IsEmpty ( strItem2, this ))
 
		{
			nST2 = ST2_DB_SELECT_LIST_BY_PARAM;
			m_bParam = TRUE;
		}

		PopulateBinListDialog( nST2 );
	break;
////////////////////////////////////////////////////////////////////////
	case MODE_MERCHANT:
	
//		strHeaderData[2][50] = {"MERCHANT ID  ", "ORGANIZATION ID"} ;
		memcpy (strHeaderData[0],"ORGANIZATION ID", sizeof (strHeaderData[0])) ;
		memcpy (strHeaderData[1],"MERCHANT ID       ", sizeof (strHeaderData[1]));

		for (nHeader = 0; nHeader < 2; nHeader++) { 
			lvColumn.mask = LVCF_FMT | LVCF_SUBITEM | LVCF_TEXT  ;
			lvColumn.fmt = LVCFMT_LEFT ;
			lvColumn.pszText = strHeaderData[nHeader] ;
			lvColumn.iSubItem = nHeader ;
			
			m_CntlList.InsertColumn(nHeader, &lvColumn) ;  // assumes return value is OK.
			m_CntlList.SetColumnWidth (0, LVSCW_AUTOSIZE_USEHEADER) ;
			m_CntlList.SetColumnWidth (1, LVSCW_AUTOSIZE_USEHEADER) ;
			
		}

		SetWindowText ( "Merchant List");    

		ZeroMemory ( &sMerchantRecord, sizeof (MCF01));

		if ( myUtilsObject.IsEmpty ( strItem0, this ) )
			strcpy((char *)sMerchantRecord.primary_key.merchant_id, " " );
		else
			strcpy((char *)sMerchantRecord.primary_key.merchant_id, strItem0);
		if ( myUtilsObject.IsEmpty ( strItem1, this ) )
			strcpy((char *)sMerchantRecord.primary_key.organization_id, " ");
		else
			strcpy((char *)sMerchantRecord.primary_key.organization_id, strItem1 );

		
		PopulateMerchantListDialog();

	break;

	case MODE_OBS01:
	
//		strHeaderData[2][50] = {"MERCHANT ID  ", "ORGANIZATION ID"} ;
		memcpy (strHeaderData[0],"On Behalf Result", sizeof (strHeaderData[0])) ;
		memcpy (strHeaderData[1],"On Behalf Service", sizeof (strHeaderData[1]));
		memcpy (strHeaderData[2],"Response code", sizeof (strHeaderData[2]));
		memcpy (strHeaderData[3],"Description", sizeof (strHeaderData[3]));

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

		SetWindowText ( "OBS List");    

		ZeroMemory ( &sObs01, sizeof (OBS01));

		if ( myUtilsObject.IsEmpty ( strItem0, this ) )
			strcpy((char *)sObs01.on_behalf_service, " " );
		else
			strcpy((char *)sObs01.on_behalf_service, strItem0);

		if ( myUtilsObject.IsEmpty ( strItem1, this ) )
			strcpy((char *)sObs01.on_behalf_result, " ");
		else
			strcpy((char *)sObs01.on_behalf_result, strItem1 );

		if ( myUtilsObject.IsEmpty ( strItem2, this ) )
			strcpy((char *)sObs01.response_code, " ");
		else
			strcpy((char *)sObs01.response_code, strItem2 );

		if ( myUtilsObject.IsEmpty ( strItem3, this ) )
			strcpy((char *)sObs01.description, " ");
		else
			strcpy((char *)sObs01.description, strItem3 );

		
	PopulateObsListDialog();

	break;


	case MODE_JCB3DS2:
	
//		strHeaderData[2][50] = {"MERCHANT ID  ", "ORGANIZATION ID"} ;
		memcpy (strHeaderData[0],"Auth result code", sizeof (strHeaderData[0])) ;
		memcpy (strHeaderData[1],"Response code", sizeof (strHeaderData[1]));
		memcpy (strHeaderData[2],"Verify flag", sizeof (strHeaderData[2]));

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

		SetWindowText ( "JCB3S2 List");    

		ZeroMemory ( &sjcb3ds2, sizeof (JCB3DS2));

		if ( myUtilsObject.IsEmpty ( strItem0, this ) )
			strcpy((char *)sjcb3ds2.auth_result_code_value, " " );
		else
			strcpy((char *)sjcb3ds2.auth_result_code_value, strItem0);

		if ( myUtilsObject.IsEmpty ( strItem1, this ) )
			strcpy((char *)sjcb3ds2.response_code, " ");
		else
			strcpy((char *)sjcb3ds2.response_code, strItem1 );

		if ( myUtilsObject.IsEmpty ( strItem2, this ) )
			strcpy((char *)sjcb3ds2.validation_flag, " ");
		else
			strcpy((char *)sjcb3ds2.validation_flag, strItem2 );

		
	PopulateJcb3ds2ListDialog();

	break;

	case MODE_CURRENCY_CODE_LIST:
	
//		strHeaderData[2][50] = {"MERCHANT ID  ", "ORGANIZATION ID"} ;
		memcpy (strHeaderData[0],"Currency Value", sizeof (strHeaderData[0])) ;
		memcpy (strHeaderData[1],"Currency Code", sizeof (strHeaderData[1]));
		memcpy (strHeaderData[2],"Decimal Places", sizeof (strHeaderData[2]));
		memcpy (strHeaderData[3],"Currency Name", sizeof (strHeaderData[3]));

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

		SetWindowText ( "CCL01 List");    

		ZeroMemory ( &sccl01, sizeof (CCL01));

		if ( myUtilsObject.IsEmpty ( strItem0, this ) )
			strcpy((char *)sccl01.currency_value, " " );
		else
			strcpy((char *)sccl01.currency_value, strItem0);

		if ( myUtilsObject.IsEmpty ( strItem1, this ) )
			strcpy((char *)sccl01.currency_code, " ");
		else
			strcpy((char *)sccl01.currency_code, strItem1 );

		if ( myUtilsObject.IsEmpty ( strItem2, this ) )
			strcpy((char *)sccl01.currency_decimal_places, " ");
		else
			strcpy((char *)sccl01.currency_decimal_places, strItem2 );

		if ( myUtilsObject.IsEmpty ( strItem3, this ) )
			strcpy((char *)sccl01.currency_name, " ");
		else
			strcpy((char *)sccl01.currency_name, strItem3 );

		
	PopulateCcl01ListDialog();

	break;

		case MODE_BDK_KEY01:
	
//		strHeaderData[2][50] = {"MERCHANT ID  ", "ORGANIZATION ID"} ;
		memcpy (strHeaderData[0],"Card Barnd", sizeof (strHeaderData[0])) ;
		memcpy (strHeaderData[1],"BDK Key", sizeof (strHeaderData[1]));


		for (nHeader = 0; nHeader < 2; nHeader++) { 
			lvColumn.mask = LVCF_FMT | LVCF_SUBITEM | LVCF_TEXT  ;
			lvColumn.fmt = LVCFMT_LEFT ;
			lvColumn.pszText = strHeaderData[nHeader] ;
			lvColumn.iSubItem = nHeader ;
			
			m_CntlList.InsertColumn(nHeader, &lvColumn) ;  // assumes return value is OK.
			m_CntlList.SetColumnWidth (0, LVSCW_AUTOSIZE_USEHEADER) ;
			m_CntlList.SetColumnWidth (1, LVSCW_AUTOSIZE_USEHEADER) ;

		}

		SetWindowText ( "BDKKEY01 List");    

		ZeroMemory ( &sbdkkey01, sizeof (BDKKEY01));

		if ( myUtilsObject.IsEmpty ( strItem0, this ) )
			strcpy((char *)sbdkkey01.card_brand, " " );
		else
			strcpy((char *)sbdkkey01.card_brand, strItem0);

		if ( myUtilsObject.IsEmpty ( strItem1, this ) )
			strcpy((char *)sbdkkey01.bdk_key, " ");
		else
			strcpy((char *)sbdkkey01.bdk_key, strItem1 );
		
	PopulateBdkkey01ListDialog();

	break;

	case MODE_VISA3DS2:

	//		strHeaderData[2][50] = {"MERCHANT ID  ", "ORGANIZATION ID"} ;
			memcpy (strHeaderData[0],"CAVV Results Code", sizeof (strHeaderData[0])) ;
			memcpy (strHeaderData[1],"Response Code", sizeof (strHeaderData[1])) ;
			memcpy (strHeaderData[2],"Description", sizeof (strHeaderData[2])) ;


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

			SetWindowText ( "VISA3DS2 List");

			ZeroMemory ( &sVisa3ds2, sizeof (VISA3DS2));

			if ( myUtilsObject.IsEmpty ( strItem0, this ) )
				strcpy((char *)sVisa3ds2.cavv, " " );
			else
				strcpy((char *)sVisa3ds2.cavv, strItem0);

			if ( myUtilsObject.IsEmpty ( strItem1, this ) )
				strcpy((char *)sVisa3ds2.response_code, " ");
			else
				strcpy((char *)sVisa3ds2.response_code, strItem1 );

			if ( myUtilsObject.IsEmpty ( strItem2, this ) )
				strcpy((char *)sVisa3ds2.description, " ");
			else
				strcpy((char *)sVisa3ds2.description, strItem2 );


			PopulateVisa3ds2ListDialog();

		break;


		case MODE_RULEID:

	//		strHeaderData[2][50] = {"MERCHANT ID  ", "ORGANIZATION ID"} ;
			memcpy (strHeaderData[0],"Rule ID", sizeof (strHeaderData[0])) ;


			for (nHeader = 0; nHeader < 1; nHeader++) {
				lvColumn.mask = LVCF_FMT | LVCF_SUBITEM | LVCF_TEXT  ;
				lvColumn.fmt = LVCFMT_LEFT ;
				lvColumn.pszText = strHeaderData[nHeader] ;
				lvColumn.iSubItem = nHeader ;

				m_CntlList.InsertColumn(nHeader, &lvColumn) ;  // assumes return value is OK.
				m_CntlList.SetColumnWidth (0, LVSCW_AUTOSIZE_USEHEADER) ;

			}

			SetWindowText ( "Rule ID List");

			ZeroMemory ( &sCR01, sizeof (CR01));

			if ( myUtilsObject.IsEmpty ( strItem0, this ) )
				strcpy((char *)sCR01.rule_id, " " );
			else
				strcpy((char *)sCR01.rule_id, strItem0);

			PopulateRuleIDListDialog();

		break;

	case MODE_MGF01:
		memcpy (strHeaderData[0],"MERCHANT GROUP ID", sizeof (strHeaderData[0])) ;
		
		for (nHeader = 0; nHeader < 1; nHeader++) { 
			lvColumn.mask = LVCF_FMT | LVCF_SUBITEM | LVCF_TEXT  ;
			lvColumn.fmt = LVCFMT_LEFT ;
			lvColumn.pszText = strHeaderData[nHeader] ;
			lvColumn.iSubItem = nHeader ;
			
			m_CntlList.InsertColumn(nHeader, &lvColumn) ;  // assumes return value is OK.
			m_CntlList.SetColumnWidth (0, LVSCW_AUTOSIZE_USEHEADER) ;
			m_CntlList.SetColumnWidth (1, LVSCW_AUTOSIZE_USEHEADER) ;
			
		}

		SetWindowText ( "Merchant Group List");    

		ZeroMemory ( &sMGF01Record, sizeof (MGF01));

		if ( myUtilsObject.IsEmpty ( strItem0, this ) )
			strcpy((char *)sMGF01Record.primary_key.merchant_group_id, " " );
		else
			strcpy((char *)sMGF01Record.primary_key.merchant_group_id, strItem0);


		PopulateMGF01ListDialog();
		
		break;

	case MODE_MERCHANT_PAY_PARAM:
		memcpy (strHeaderData[0]," Product code", sizeof (strHeaderData[0])) ;
		
		for (nHeader = 0; nHeader < 1; nHeader++) { 
			lvColumn.mask = LVCF_FMT | LVCF_SUBITEM | LVCF_TEXT  ;
			lvColumn.fmt = LVCFMT_LEFT ;
			lvColumn.pszText = strHeaderData[nHeader] ;
			lvColumn.iSubItem = nHeader ;
			
			m_CntlList.InsertColumn(nHeader, &lvColumn) ;  // assumes return value is OK.
			m_CntlList.SetColumnWidth (0, LVSCW_AUTOSIZE_USEHEADER) ;
			m_CntlList.SetColumnWidth (1, LVSCW_AUTOSIZE_USEHEADER) ;
			
		}
		
		SetWindowText ( " MP Product Code List");    
		
		ZeroMemory ( &sMPPARA01Record, sizeof (MPPARA01));
		
		if ( myUtilsObject.IsEmpty ( strItem0, this ) )
			strcpy((char *)sMPPARA01Record.primary_key.product_code, " " );
		else
			strcpy((char *)sMPPARA01Record.primary_key.product_code, strItem0);
		
		
		
		PopulateMPPARA01ListDialog();
		break;

////////////////////////////////////////////////////////////////////////////
	case MODE_TERMINAL:
	
//		strHeaderData2[3][50] = {"DEVICE ID", "MERCHANT ID      ", "ORGANIZATION ID"} ;
		memcpy (strHeaderData2[0],"DEVICE ID", sizeof (strHeaderData[0]));
		memcpy (strHeaderData2[1],"ORGANIZATION ID", sizeof (strHeaderData[2]));
		memcpy (strHeaderData2[2],"MERCHANT ID      ", sizeof (strHeaderData[1])) ;

		for (nHeader = 0; nHeader < 3; nHeader++) { 
			lvColumn.mask = LVCF_FMT | LVCF_SUBITEM | LVCF_TEXT  ;
			lvColumn.fmt = LVCFMT_LEFT ;
			lvColumn.pszText = strHeaderData2[nHeader] ;
			lvColumn.iSubItem = nHeader ;
			
			m_CntlList.InsertColumn(nHeader, &lvColumn) ;  // assumes return value is OK.
			m_CntlList.SetColumnWidth (0, LVSCW_AUTOSIZE_USEHEADER) ;
			m_CntlList.SetColumnWidth (1, LVSCW_AUTOSIZE_USEHEADER) ;
			m_CntlList.SetColumnWidth (2, LVSCW_AUTOSIZE_USEHEADER) ;
			
		}


		SetWindowText ( "Device List");    

		ZeroMemory ( &sTerminalRecord, sizeof (DCF01));

		strcpy((char *)sTerminalRecord.merchant_id, " ");
		strcpy((char *)sTerminalRecord.organization_id, " ");
		if ( myUtilsObject.IsEmpty ( strItem0, this ) )
			strcpy((char *)sTerminalRecord.primary_key.device_id, " ");
		else
			strcpy((char *)sTerminalRecord.primary_key.device_id, strItem0);

		
		PopulateTerminalListDialog();

	break;
	//////////////////////////////////////////////////////////////////
    
   case MODE_BATCH: case MODE_BATCH_HISTORY:
		memcpy (strHeaderData3[0],"BATCH NUM", sizeof (strHeaderData[0]));
		memcpy (strHeaderData3[1],"REF NUM", sizeof (strHeaderData[1])) ;
		memcpy (strHeaderData3[2],"PROCESSING CODE", sizeof (strHeaderData[2]));
		memcpy (strHeaderData3[3],"TRAN AMOUNT", sizeof (strHeaderData[3]));
		memcpy (strHeaderData3[4],"AUTH NUM", sizeof (strHeaderData[5]));
		memcpy (strHeaderData3[5],"TRAN DATE", sizeof (strHeaderData[6]));
		memcpy (strHeaderData3[6],"TRAN TIME", sizeof (strHeaderData[7])) ;
		memcpy (strHeaderData3[7],"TRAN ID", sizeof (strHeaderData[8]));
		memcpy (strHeaderData3[8],"MESSAGE TYPE", sizeof (strHeaderData[9]));
		memcpy (strHeaderData3[9],"CARD NUM", sizeof (strHeaderData[10])) ;
		memcpy (strHeaderData3[10],"INVOICE NUM", sizeof (strHeaderData[11]));
		memcpy (strHeaderData3[11],"FILE PREFIX", sizeof (strHeaderData[12]));
		memcpy (strHeaderData3[12],"PRODUCT CODE", sizeof (strHeaderData[13])) ;
//		memcpy (strHeaderData3[14],"ODOMETER", sizeof (strHeaderData[14])); removed for EB
	
/*		strHeaderData3[15][50] = {"BATCH NUM", "REF NUM", "PROCESSING CODE", "TRAN AMOUNT",
									"RESPONSE CODE", "AUTH NUM", "TRAN DATE",
									"TRAN TIME", "TRAN ID", "MESSAGE TYPE",
									"CARD NUM", "INVOICE NUM", "FILE PREFIX",
									"PRODUCT CODE", "ODOMETER"} ;*/

		for (nHeader = 0; nHeader < 13; nHeader++) { 
			lvColumn.mask = LVCF_FMT | LVCF_SUBITEM | LVCF_TEXT  ;
			lvColumn.fmt = LVCFMT_JUSTIFYMASK ;
			lvColumn.pszText = strHeaderData3[nHeader] ;
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
		//	m_CntlList.SetColumnWidth (13, LVSCW_AUTOSIZE_USEHEADER) ;
		//	m_CntlList.SetColumnWidth (14, LVSCW_AUTOSIZE_USEHEADER) ;
			
		}

		SetWindowText ( "Batch List");    

		memcpy (sBatchRecord.primary_key.retrieval_ref_num ,"  ", sizeof (sBatchRecord.primary_key.retrieval_ref_num));
		
		PopulateBatchListDialog();

	break;
}
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


void CBinList::OnNextButton() 
{
	bNext = TRUE;
	int nST2;

	if (!bDone) 
	{
		switch (m_nDialogMode)
		{
		case MODE_BIN:
			if ( m_bParam )
			{
				nST2 = ST2_DB_SELECT_LIST_BY_PARAM;
				PopulateBinListDialog(nST2);
			}
			else
			{
				nST2 = 0;
				PopulateBinListDialog(nST2);
			}
		break;

		case MODE_RELATED:
			if ( m_bParam )
			{
				nST2 = ST2_DB_SELECT_LIST_BY_PARAM;
				PopulateACFListDialog(nST2);
			}
			else
			{
				nST2 = 0;
				PopulateACFListDialog(nST2);
			}
		break;

		case MODE_PURCHASE:
			if ( m_bParam )
			{
				nST2 = ST2_DB_SELECT_LIST_BY_PARAM;
				PopulateCCF02ListDialog(nST2);
			}
			else
			{
				nST2 = 0;
				PopulateCCF02ListDialog(nST2);
			}
		break;
		case MODE_MERCHANT:
			PopulateMerchantListDialog();
		break;

		case MODE_OBS01:
			PopulateObsListDialog();
		break;

		case MODE_VISA3DS2:
					PopulateVisa3ds2ListDialog();
				break;

				case MODE_RULEID:
					PopulateRuleIDListDialog();
				break;
		

		case MODE_MGF01:
		PopulateMGF01ListDialog();
		break;

		case MODE_MERCHANT_PAY_PARAM:
		PopulateMPPARA01ListDialog();
		break;

		case MODE_DEFERRED:
			PopulateDEFListDialog();
		break;


      case MODE_BATCH: case MODE_BATCH_HISTORY:
			PopulateBatchListDialog();
		break;

		case MODE_TERMINAL:
			PopulateTerminalListDialog();
		break;

		case MODE_PRIMACC:
			PopulatePAFListDialog();
		break;

		case MODE_PROFID: case MODE_PROFIDBYTYPE:
			PopulateCAFListDialog();
		break;

		case MODE_STATUS:
			PopulateCSFListDialog();
		break;

		case MODE_OMC01:
			PopulateOMCListDialog();
		break;

		case MODE_CCF:
			if ( m_bParam )
			{
				nST2 = ST2_DB_SELECT_LIST_BY_PARAM;
				PopulateCCF02ListDialog(nST2);
			}
			else
			{
				nST2 = 0;
				PopulateCCF02ListDialog(nST2);
			}


		break;

		case MODE_NEGBIN:
			PopulateNegBinListDialog();
		break;

		}
	}
	else 
	{
        m_StrMessageLbl = "End of List";
		m_CntlNextButton.EnableWindow(FALSE);
		m_CntlList.SetFocus();
		UpdateData(FALSE);
	}
	
}

void CBinList::PopulateACFListDialog( int nST2 )
{
	CFont m_font;
	LV_ITEM lvItem;
	char strErrorMessage[200];
	CString temp;
	char strType[20];
        
    int nRowItem = 0;
    int nActualItem = 0;
    int nNumberReturned;
    bDone = FALSE;
    int nSize, nCount;
    nCount = m_CntlList.GetItemCount();

	int rcode = txdsapi_get_list ((pBYTE)&sACFRecord,sizeof(ACF01), (pBYTE)&sACFList, 
									sizeof (ACF01_GUI_LIST),nST2,ACF01_DATA,strErrorMessage);
	if (rcode == PTEMSG_OK)
	{

			nNumberReturned = atoi ((char *)&sACFList.num_returned);
            if( nNumberReturned == GUI_MAX_LIST_SIZE)
            {				
			    memcpy( sACFRecord.primary_key.account_nbr, (LPCTSTR) sACFList.acf01_list[nNumberReturned-1].primary_key.account_nbr, sizeof (sACFRecord.primary_key.account_nbr)); 
				memcpy( sACFRecord.primary_key.account_type, (LPCTSTR) sACFList.acf01_list[nNumberReturned-1].primary_key.account_type, sizeof (sACFRecord.primary_key.account_type)); 
			   nSize = nNumberReturned - 1;
			}
			else
         {
				bDone = TRUE;
			   nSize = nNumberReturned;
         }
						
			for (int nItem = 0; nItem <  nSize; nItem++,nRowItem ++) { // will now insert the items and subitems into the list view.
				m_CntlList.EnableWindow(TRUE);  
			    for (int nSubItem = 0; nSubItem < 2; nSubItem++) {					
					
			    lvItem.mask = LVIF_TEXT ;
				lvItem.iItem = (nSubItem == 0)? nRowItem : nActualItem;
				lvItem.iSubItem = nSubItem;

				if (nSubItem == 0) { 
					lvItem.pszText = (char *)sACFList.acf01_list[nItem].primary_key.account_nbr;                   
				}
				else if (nSubItem == 1) { 
					temp = sACFList.acf01_list[nItem].primary_key.account_type;
					if ( strcmp ( temp, "05" ) == 0 )
						temp = "US Dollar";					
					else if ( strcmp ( temp, "06" ) == 0 )
						temp = "Phil.Peso";
					else if ( strcmp ( temp, "07" ) == 0 )
						temp = "Deferred";
					else if ( strcmp ( temp, "08" ) == 0 )
						temp = "House Card";
					memcpy ( strType, temp, sizeof (strType) );
					lvItem.pszText = (char *)strType;                   
				}
				
                
						
				if (nSubItem == 0) 
					nActualItem = m_CntlList.InsertItem(nCount+nSize,(char *)sACFList.acf01_list[nItem].primary_key.account_nbr);
				else 
					m_CntlList.SetItem(&lvItem);


						
			} // for
		} // for 
				m_CntlList.SetFocus() ;
				int nCount = m_CntlList.GetItemCount();

				m_CntlList.EnsureVisible( nCount-1, TRUE);
		}
}

void CBinList::PopulateCCF04ListDialog( int nST2 )
{
	CFont m_font;
	LV_ITEM lvItem;
	char strErrorMessage[200];
	CString temp;
	char strType[10];
        
    int nRowItem = 0;
    int nActualItem = 0;
    int nNumberReturned;
    bDone = FALSE;
    int nSize, nCount;
    nCount = m_CntlList.GetItemCount();

	int rcode = txdsapi_get_list ((pBYTE)&sCCFRecord,sizeof(CCF04), (pBYTE)&sCCFList, 
									sizeof (CCF04_GUI_LIST),nST2,CCF04_DATA,strErrorMessage);
	if (rcode == PTEMSG_OK)
	{

		nNumberReturned = atoi ((char *)&sCCFList.num_returned);
        if( nNumberReturned == GUI_MAX_LIST_SIZE)
        {				
		    memcpy( sCCFRecord.primary_key.card_nbr, (LPCTSTR) sCCFList.ccf04_list[nNumberReturned-1].primary_key.card_nbr, sizeof (sCCFRecord.primary_key.card_nbr)); 
			memcpy( sCCFRecord.primary_key.card_type, (LPCTSTR) sCCFList.ccf04_list[nNumberReturned-1].primary_key.card_type, sizeof (sCCFRecord.primary_key.card_type)); 
		   nSize = nNumberReturned - 1;
		}
		else
        {
				bDone = TRUE;
			   nSize = nNumberReturned;
        }
						
		for (int nItem = 0; nItem <  nSize; nItem++,nRowItem ++) 
		{ // will now insert the items and subitems into the list view.
			m_CntlList.EnableWindow(TRUE);  
		    for (int nSubItem = 0; nSubItem < 4; nSubItem++) {					
				
		    lvItem.mask = LVIF_TEXT ;
			lvItem.iItem = (nSubItem == 0)? nRowItem : nActualItem;
			lvItem.iSubItem = nSubItem;

			if (nSubItem == 0) 
			{ 
				lvItem.pszText = (char *)sCCFList.ccf04_list[nItem].primary_key.card_nbr;                   
			}
			else if (nSubItem == 1) 
			{ 
				temp = sCCFList.ccf04_list[nItem].primary_key.card_type;
				if ( strcmp ( temp, "C" ) == 0 )
					temp = "CREDIT";
				else if ( strcmp ( temp, "L" ) == 0 )
					temp = "LOYALTY";
				else if ( strcmp ( temp, "D" ) == 0 )
					temp = "DEBIT";
                else if ( strcmp ( temp, "E" ) == 0 )
                     temp = "DEFERRED";
				else if ( strcmp ( temp, "F" ) == 0 )
					 temp = "FLEET";
				memcpy ( strType, temp, sizeof (strType) );
				lvItem.pszText = (char *)strType;                   
			}
			else if (nSubItem == 2)
			{
				lvItem.pszText = (char *)sCCFList.ccf04_list[nItem].mcc_lo_range;
			}
			else if (nSubItem == 3)
			{
				lvItem.pszText = (char *)sCCFList.ccf04_list[nItem].mcc_hi_range;
			}
			
            
					
			if (nSubItem == 0) 
				nActualItem = m_CntlList.InsertItem(nCount+nSize,(char *)sCCFList.ccf04_list[nItem].primary_key.card_nbr);
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

void CBinList::PopulateNegBinListDialog()
{
	CFont m_font;
	LV_ITEM lvItem;
	char strErrorMessage[200];
    
    int nRowItem = 0;
    int nActualItem = 0;
    int nNumberReturned;
    bDone = FALSE;
    int nSize, nCount;
    nCount = m_CntlList.GetItemCount();

	int rcode = txdsapi_get_list ((pBYTE)&sNegBinRecord,sizeof(NBF01), (pBYTE)&sNegBinList, 
									sizeof (NBF01_GUI_LIST),0,NBF01_DATA,strErrorMessage);
	if (rcode == PTEMSG_OK)
	{

			nNumberReturned = atoi ((char *)&sNegBinList.num_returned);
            if( nNumberReturned == GUI_MAX_LIST_SIZE)
            {				
			    memcpy( sNegBinRecord.primary_key.low, (LPCTSTR) sNegBinList.nbf01_record[nNumberReturned-1].primary_key.low, sizeof (sNegBinRecord.primary_key.low)); 
				memcpy( sNegBinRecord.primary_key.high, (LPCTSTR) sNegBinList.nbf01_record[nNumberReturned-1].primary_key.high, sizeof (sNegBinRecord.primary_key.high)); 
			   nSize = nNumberReturned - 1;
			}
			else
         {
				bDone = TRUE;
			   nSize = nNumberReturned;
         }
						
			for (int nItem = 0; nItem <  nSize; nItem++,nRowItem ++) { // will now insert the items and subitems into the list view.
				m_CntlList.EnableWindow(TRUE);  
			    for (int nSubItem = 0; nSubItem < 2; nSubItem++) {					
					
			    lvItem.mask = LVIF_TEXT ;
				lvItem.iItem = (nSubItem == 0)? nRowItem : nActualItem;
				lvItem.iSubItem = nSubItem;

				if (nSubItem == 0) { 
					lvItem.pszText = (char *)sNegBinList.nbf01_record[nItem].primary_key.low;                   
				}
				else if (nSubItem == 1) { 
					lvItem.pszText = (char *)sNegBinList.nbf01_record[nItem].primary_key.high;                   
				}
				
                
						
				if (nSubItem == 0) 
					nActualItem = m_CntlList.InsertItem(nCount+nSize,(char *)sNegBinList.nbf01_record[nItem].primary_key.low);
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


void CBinList::PopulateBatchListDialog()
{
	CFont m_font;
	LV_ITEM lvItem;
	char strErrorMessage[200];
	CString  temp;
   char strSum[50];
   CMyUtils myUtilsObject;
   int nSize, nCount, rcode;
    
    int nRowItem = 0;
    int nActualItem = 0;
    int nNumberReturned;
    bDone = FALSE;

    nCount = m_CntlList.GetItemCount();

    if ( 0 == strcmp ( m_strBatchStatus,"C" ) || 0 == strcmp ( m_strBatchStatus,"P" ))
	   rcode = txdsapi_get_list ((pBYTE)&sBatchRecord,sizeof(BCH10), (pBYTE)&sBatchList, sizeof (BCH10_GUI_LIST),0,BCH10_DATA,strErrorMessage);
    else     if ( 0 == strcmp ( m_strBatchStatus,"R" ) || 0 == strcmp ( m_strBatchStatus,"W" ))
	   rcode = txdsapi_get_list ((pBYTE)&sBatchRecord,sizeof(BCH11), (pBYTE)&sBatchList, sizeof (BCH11_GUI_LIST),0,BCH11_DATA,strErrorMessage);
	else
		rcode = PTEMSG_NOT_FOUND;

	if (rcode == PTEMSG_OK)
	{

			nNumberReturned = atoi ((char *)&sBatchList.num_returned);
            if( nNumberReturned == GUI_MAX_LIST_SIZE)
            {				
			    memcpy( sBatchRecord.primary_key.device_id, (LPCTSTR) sBatchList.batch_list[nNumberReturned-1].primary_key.device_id, sizeof (sBatchRecord.primary_key.device_id)); 
				memcpy( sBatchRecord.primary_key.batch_nbr, (LPCTSTR) sBatchList.batch_list[nNumberReturned-1].primary_key.batch_nbr, sizeof (sBatchRecord.primary_key.batch_nbr)); 
				memcpy( sBatchRecord.merchant_id, (LPCTSTR) sBatchList.batch_list[nNumberReturned-1].merchant_id, sizeof (sBatchRecord.merchant_id)); 
				memcpy( sBatchRecord.primary_key.retrieval_ref_num, (LPCTSTR) sBatchList.batch_list[nNumberReturned-1].primary_key.retrieval_ref_num, sizeof (sBatchRecord.primary_key.retrieval_ref_num)); 
            nSize = nNumberReturned - 1;			
			}
			else
         {
				bDone = TRUE;
            nSize = nNumberReturned;	
         }
						
			for (int nItem = 0; nItem <  nSize; nItem++,nRowItem ++) { // will now insert the items and subitems into the list view.
				m_CntlList.EnableWindow(TRUE);  
			    for (int nSubItem = 0; nSubItem < 13; nSubItem++) {					
					
			    lvItem.mask = LVIF_TEXT ;
				lvItem.iItem = (nSubItem == 0)? nRowItem : nActualItem;
				lvItem.iSubItem = nSubItem;

				if (nSubItem == 0) { 
					lvItem.pszText = (char *)sBatchList.batch_list[nItem].primary_key.batch_nbr;                   
				}
				else if (nSubItem == 1) { 
					lvItem.pszText = (char *)sBatchList.batch_list[nItem].primary_key.retrieval_ref_num;                   
				}
				else if (nSubItem == 2) { 
					lvItem.pszText = (char *)sBatchList.batch_list[nItem].processing_code;                   
				}
				else if (nSubItem == 3) { 
					temp = sBatchList.batch_list[nItem].tran_amount ;
                memcpy ( strSum, myUtilsObject.DisplayAmount(temp), sizeof (strSum));
					lvItem.pszText = strSum ;                    
				}
				else if (nSubItem == 4) { 
					lvItem.pszText = (char *)sBatchList.batch_list[nItem].auth_number;                   
				}
				else if (nSubItem == 5) { 
					temp = (char *)sBatchList.batch_list[nItem].tran_date;
               if ( temp.GetLength() != 0 )
               {
					   temp = temp.Right(2) + "/" + temp.Mid(4,2) + "/" + temp.Left(4);
					   memcpy ( strSum, temp, sizeof (strSum));
					   lvItem.pszText = strSum;                   
               }
				}
				else if (nSubItem == 6) { 
					temp = (char *)sBatchList.batch_list[nItem].tran_time;
               if ( temp.GetLength() != 0 )
               {
					   temp = temp.Left(2) + ":" + temp.Mid(2,2) + ":" + temp.Right(2);
					   memcpy ( strSum, temp, sizeof (strSum));
					   lvItem.pszText = strSum;                   
               }
				}
				else if (nSubItem == 7) { 
					lvItem.pszText = (char *)sBatchList.batch_list[nItem].transaction_id;                   
				}
				else if (nSubItem == 8) { 
					lvItem.pszText = (char *)sBatchList.batch_list[nItem].message_type;                   
				}
				else if (nSubItem == 9) { 
					lvItem.pszText = (char *)sBatchList.batch_list[nItem].card_nbr;                   
				}
				else if (nSubItem == 10) { 
					lvItem.pszText = (char *)sBatchList.batch_list[nItem].invoice_nbr;                   
				}
				else if (nSubItem == 11) { 
					lvItem.pszText = (char *)sBatchList.batch_list[nItem].settle_file_prefix;                   
				}
				else if (nSubItem == 12) { 
					lvItem.pszText = (char *)sBatchList.batch_list[nItem].product_code;                   
				}
			/*	else if (nSubItem == 14) { 
					lvItem.pszText = (char *)sBatchList.batch_list[nItem].odometer;                   
				} removed for EB */
                
						
				if (nSubItem == 0) 
					nActualItem = m_CntlList.InsertItem(nCount+nSize,(char *)sBatchList.batch_list[nItem].primary_key.batch_nbr);
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

void CBinList::PopulateMerchantListDialog()
{
	CFont m_font;
	LV_ITEM lvItem;
	char strErrorMessage[200];
    
    int nRowItem = 0;
    int nActualItem = 0;
    int nNumberReturned;
    int nSize, nCount, rcode;
    bDone = FALSE;
       
    nCount = m_CntlList.GetItemCount();

 //   if ( strcmp ( sMerchantRecord.primary_key.organization_id, "" ) == 0 )
	      rcode = txdsapi_get_list ((pBYTE)&sMerchantRecord, sizeof (MCF01), 
									       (pBYTE)&sMerchantList, sizeof (MCF01_GUI_LIST),
									        0, MCF01_DATA, strErrorMessage);
/*    else
	      rcode = txdsapi_get_list ((pBYTE)&sMerchantRecord, sizeof (MCF01), 
									       (pBYTE)&sMerchantList, sizeof (MCF01_GUI_LIST),
									        ST2_DB_SELECT_BY_ORG_ID, MCF01_DATA, strErrorMessage);
*/
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
            nSize = nNumberReturned - 1;			
			}
			else
         {
				bDone = TRUE;
            nSize = nNumberReturned;			
         }
						
			for (int nItem = 0; nItem <  nSize; nItem++,nRowItem ++) { // will now insert the items and subitems into the list view.
				m_CntlList.EnableWindow(TRUE);  

			   for (int nSubItem = 0; nSubItem < 5; nSubItem++) {					
					
			   lvItem.mask = LVIF_TEXT ;
				lvItem.iItem = (nSubItem == 0)? nRowItem : nActualItem;
				lvItem.iSubItem = nSubItem;

				if (nSubItem == 0) { 
					lvItem.pszText = (char *)sMerchantList.merchant_info[nItem].organization_id;                   
				}
				else if (nSubItem == 1) { 
					lvItem.pszText = (char *)sMerchantList.merchant_info[nItem].merchant_id;                   
				}
				
                
						
				if (nSubItem == 0) 
                nActualItem = m_CntlList.InsertItem(nCount+nSize, (char *)sMerchantList.merchant_info[nItem].organization_id);
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

void CBinList::PopulateObsListDialog()
{
	CFont m_font;
	LV_ITEM lvItem;
	char strErrorMessage[200];
    
    int nRowItem = 0;
    int nActualItem = 0;
    int nNumberReturned;
    int nSize, nCount, rcode;
    bDone = FALSE;
       
    nCount = m_CntlList.GetItemCount();

 //   if ( strcmp ( sMerchantRecord.primary_key.organization_id, "" ) == 0 )
	      rcode = txdsapi_get_list ((pBYTE)&sObs01, sizeof (OBS01), 
									       (pBYTE)&sObsList, sizeof (OBS01_GUI_LIST),
									        0, OBS01_DATA, strErrorMessage);
/*    else
	      rcode = txdsapi_get_list ((pBYTE)&sMerchantRecord, sizeof (MCF01), 
									       (pBYTE)&sMerchantList, sizeof (MCF01_GUI_LIST),
									        ST2_DB_SELECT_BY_ORG_ID, MCF01_DATA, strErrorMessage);
*/
	if (rcode == PTEMSG_OK)
	{
			nNumberReturned = atoi ((char *)&sObsList.num_returned);
         if( nNumberReturned == GUI_MAX_LIST_SIZE)
         {				
			    memcpy( sObs01.on_behalf_service, 
                     (LPCTSTR) sObsList.obs01_record[nNumberReturned-1].on_behalf_service, 
                     sizeof (sObs01.on_behalf_service)); 
				 memcpy( sObs01.on_behalf_result, 
                     (LPCTSTR) sObsList.obs01_record[nNumberReturned-1].on_behalf_result, 
                     sizeof (sObs01.on_behalf_result)); 
				 memcpy( sObs01.response_code, 
                     (LPCTSTR) sObsList.obs01_record[nNumberReturned-1].response_code, 
                     sizeof (sObs01.response_code)); 
				  memcpy( sObs01.description, 
                     (LPCTSTR) sObsList.obs01_record[nNumberReturned-1].description, 
                     sizeof (sObs01.description)); 
            nSize = nNumberReturned - 1;			
			}
			else
         {
				bDone = TRUE;
            nSize = nNumberReturned;			
         }
						
			for (int nItem = 0; nItem <  nSize; nItem++,nRowItem ++) { // will now insert the items and subitems into the list view.
				m_CntlList.EnableWindow(TRUE);  

			   for (int nSubItem = 0; nSubItem < 5; nSubItem++) {					
					
			   lvItem.mask = LVIF_TEXT ;
				lvItem.iItem = (nSubItem == 0)? nRowItem : nActualItem;
				lvItem.iSubItem = nSubItem;

				if (nSubItem == 0) { 
					lvItem.pszText = (char *)sObsList.obs01_record[nItem].on_behalf_result ;                   
				}
				else if (nSubItem == 1) { 
					lvItem.pszText = (char *)sObsList.obs01_record[nItem].on_behalf_service;                   
				}
				else if (nSubItem == 2) { 
					lvItem.pszText = (char *)sObsList.obs01_record[nItem].response_code;                   
				}
				else if (nSubItem == 3) { 
					lvItem.pszText = (char *)sObsList.obs01_record[nItem].description;                   
				}
				
                
						
				if (nSubItem == 0) 
				{
					nActualItem = m_CntlList.InsertItem(nCount+nSize, (char *)sObsList.obs01_record[nItem].on_behalf_result);
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

void CBinList::PopulateVisa3ds2ListDialog()
{
	CFont m_font;
	LV_ITEM lvItem;
	char strErrorMessage[200];

    int nRowItem = 0;
    int nActualItem = 0;
    int nNumberReturned;
    int nSize, nCount, rcode;
    bDone = FALSE;

    nCount = m_CntlList.GetItemCount();

 //   if ( strcmp ( sMerchantRecord.primary_key.organization_id, "" ) == 0 )
	      rcode = txdsapi_get_list ((pBYTE)&sVisa3ds2, sizeof (VISA3DS2),
									       (pBYTE)&sVisa3ds2List, sizeof (VISA3DS2_GUI_LIST),
									        0, VISA3DS2_DATA, strErrorMessage);
/*    else
	      rcode = txdsapi_get_list ((pBYTE)&sMerchantRecord, sizeof (MCF01),
									       (pBYTE)&sMerchantList, sizeof (MCF01_GUI_LIST),
									        ST2_DB_SELECT_BY_ORG_ID, MCF01_DATA, strErrorMessage);
*/
	if (rcode == PTEMSG_OK)
	{
			nNumberReturned = atoi ((char *)&sVisa3ds2List.num_returned);
         if( nNumberReturned == GUI_MAX_LIST_SIZE)
         {
			    memcpy( sVisa3ds2.cavv,
                     (LPCTSTR) sVisa3ds2List.visa3ds2_record[nNumberReturned-1].cavv,
                     sizeof (sVisa3ds2.cavv));
				 memcpy( sVisa3ds2.response_code,
                     (LPCTSTR) sVisa3ds2List.visa3ds2_record[nNumberReturned-1].response_code,
                     sizeof (sVisa3ds2.response_code));
				 memcpy( sVisa3ds2.description,
                     (LPCTSTR) sVisa3ds2List.visa3ds2_record[nNumberReturned-1].description,
                     sizeof (sVisa3ds2.description));

            nSize = nNumberReturned - 1;
			}
			else
         {
				bDone = TRUE;
            nSize = nNumberReturned;
         }

			for (int nItem = 0; nItem <  nSize; nItem++,nRowItem ++) { // will now insert the items and subitems into the list view.
				m_CntlList.EnableWindow(TRUE);

			   for (int nSubItem = 0; nSubItem < 4; nSubItem++) {

			   lvItem.mask = LVIF_TEXT ;
				lvItem.iItem = (nSubItem == 0)? nRowItem : nActualItem;
				lvItem.iSubItem = nSubItem;

				if (nSubItem == 0)
				{
					lvItem.pszText = (char *)sVisa3ds2List.visa3ds2_record[nItem].cavv ;
				}
				else if(nSubItem == 1) 
				{
					lvItem.pszText = (char *)sVisa3ds2List.visa3ds2_record[nItem].response_code ;
				}
				else if(nSubItem == 2) 
				{
					lvItem.pszText = (char *)sVisa3ds2List.visa3ds2_record[nItem].description ;
				}


				if (nSubItem == 0)
                nActualItem = m_CntlList.InsertItem(nCount+nSize, (char *)sVisa3ds2List.visa3ds2_record[nItem].cavv);

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

void CBinList::PopulateJcb3ds2ListDialog()
{
	CFont m_font;
	LV_ITEM lvItem;
	char strErrorMessage[200];

    int nRowItem = 0;
    int nActualItem = 0;
    int nNumberReturned;
    int nSize, nCount, rcode;
    bDone = FALSE;

    nCount = m_CntlList.GetItemCount();

 //   if ( strcmp ( sMerchantRecord.primary_key.organization_id, "" ) == 0 )
	      rcode = txdsapi_get_list ((pBYTE)&sjcb3ds2, sizeof (JCB3DS2),
									       (pBYTE)&sjcb3ds2List, sizeof (JCB3DS2_GUI_LIST),
									        0, JCB3DS2_DATA, strErrorMessage);
/*    else
	      rcode = txdsapi_get_list ((pBYTE)&sMerchantRecord, sizeof (MCF01),
									       (pBYTE)&sMerchantList, sizeof (MCF01_GUI_LIST),
									        ST2_DB_SELECT_BY_ORG_ID, MCF01_DATA, strErrorMessage);
*/
	if (rcode == PTEMSG_OK)
	{
			nNumberReturned = atoi ((char *)&sjcb3ds2List.num_returned);
         if( nNumberReturned == GUI_MAX_LIST_SIZE)
         {
			    memcpy( sjcb3ds2.auth_result_code_value,
                     (LPCTSTR) sjcb3ds2List.jcb3ds2_record[nNumberReturned-1].auth_result_code_value,
                     sizeof (sjcb3ds2.auth_result_code_value));
				memcpy( sjcb3ds2.response_code,
                     (LPCTSTR) sjcb3ds2List.jcb3ds2_record[nNumberReturned-1].response_code,
                     sizeof (sjcb3ds2.response_code));
					memcpy( sjcb3ds2.validation_flag,
                     (LPCTSTR) sjcb3ds2List.jcb3ds2_record[nNumberReturned-1].validation_flag,
                     sizeof (sjcb3ds2.validation_flag));


            nSize = nNumberReturned - 1;
			}
			else
         {
				bDone = TRUE;
            nSize = nNumberReturned;
         }

			for (int nItem = 0; nItem <  nSize; nItem++,nRowItem ++) { // will now insert the items and subitems into the list view.
				m_CntlList.EnableWindow(TRUE);

			   for (int nSubItem = 0; nSubItem < 3; nSubItem++) {

			   lvItem.mask = LVIF_TEXT ;
				lvItem.iItem = (nSubItem == 0)? nRowItem : nActualItem;
				lvItem.iSubItem = nSubItem;

				if (nSubItem == 0)
				{
					lvItem.pszText = (char *)sjcb3ds2List.jcb3ds2_record[nItem].auth_result_code_value ;
				}
				else if(nSubItem == 1) 
				{
					lvItem.pszText = (char *)sjcb3ds2List.jcb3ds2_record[nItem].response_code ;
				}
				else if(nSubItem == 2) 
				{
					lvItem.pszText = (char *)sjcb3ds2List.jcb3ds2_record[nItem].validation_flag ;
				}



				if (nSubItem == 0)
                nActualItem = m_CntlList.InsertItem(nCount+nSize, (char *)sjcb3ds2List.jcb3ds2_record[nItem].auth_result_code_value);

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

void CBinList::PopulateCcl01ListDialog()
{
	CFont m_font;
	LV_ITEM lvItem;
	char strErrorMessage[200];

    int nRowItem = 0;
    int nActualItem = 0;
    int nNumberReturned;
    int nSize, nCount, rcode;
    bDone = FALSE;

    nCount = m_CntlList.GetItemCount();

	      rcode = txdsapi_get_list ((pBYTE)&sccl01, sizeof (CCL01),
									       (pBYTE)&sccl01List, sizeof (CCL01_GUI_LIST),
									        0, CCL01_DATA, strErrorMessage);

	if (rcode == PTEMSG_OK)
	{
			nNumberReturned = atoi ((char *)&sccl01List.num_returned);
         if( nNumberReturned == GUI_MAX_LIST_SIZE)
         {
			    memcpy( sccl01.currency_value,
                     (LPCTSTR) sccl01List.ccl01_record[nNumberReturned-1].currency_value,
                     sizeof (sccl01.currency_value));
			    memcpy( sccl01.currency_code,
                     (LPCTSTR) sccl01List.ccl01_record[nNumberReturned-1].currency_code,
                     sizeof (sccl01.currency_code));
			    memcpy( sccl01.currency_decimal_places,
                     (LPCTSTR) sccl01List.ccl01_record[nNumberReturned-1].currency_decimal_places,
                     sizeof (sccl01.currency_decimal_places));
			    memcpy( sccl01.currency_name,
                     (LPCTSTR) sccl01List.ccl01_record[nNumberReturned-1].currency_name,
                     sizeof (sccl01.currency_name));

            nSize = nNumberReturned - 1;
			}
			else
         {
				bDone = TRUE;
            nSize = nNumberReturned;
         }

			for (int nItem = 0; nItem <  nSize; nItem++,nRowItem ++) { // will now insert the items and subitems into the list view.
				m_CntlList.EnableWindow(TRUE);

			   for (int nSubItem = 0; nSubItem < 4; nSubItem++) {

			   lvItem.mask = LVIF_TEXT ;
				lvItem.iItem = (nSubItem == 0)? nRowItem : nActualItem;
				lvItem.iSubItem = nSubItem;

				if (nSubItem == 0)
				{
					lvItem.pszText = (char *)sccl01List.ccl01_record[nItem].currency_value ;
				}
				else if(nSubItem == 1) 
				{
					lvItem.pszText = (char *)sccl01List.ccl01_record[nItem].currency_code ;
				}
				else if(nSubItem == 2) 
				{
					lvItem.pszText = (char *)sccl01List.ccl01_record[nItem].currency_decimal_places ;
				}
				else if(nSubItem == 3) 
				{
					lvItem.pszText = (char *)sccl01List.ccl01_record[nItem].currency_name ;
				}



				if (nSubItem == 0)
                nActualItem = m_CntlList.InsertItem(nCount+nSize, (char *)sccl01List.ccl01_record[nItem].currency_value);

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

void CBinList::PopulateBdkkey01ListDialog()
{
	CFont m_font;
	LV_ITEM lvItem;
	char strErrorMessage[200];

    int nRowItem = 0;
    int nActualItem = 0;
    int nNumberReturned;
    int nSize, nCount, rcode;
    bDone = FALSE;

    nCount = m_CntlList.GetItemCount();

	      rcode = txdsapi_get_list ((pBYTE)&sbdkkey01, sizeof (BDKKEY01),
									       (pBYTE)&sbdkkey01List, sizeof (BDKKEY01_GUI_LIST),
									        0, BDKKEY01_DATA, strErrorMessage);

	if (rcode == PTEMSG_OK)
	{
			nNumberReturned = atoi ((char *)&sbdkkey01List.num_returned);
         if( nNumberReturned == GUI_MAX_LIST_SIZE)
         {
			    memcpy( sbdkkey01.card_brand,
                     (LPCTSTR) sbdkkey01List.bdkkey01_record[nNumberReturned-1].card_brand,
                     sizeof (sbdkkey01.card_brand));
			    memcpy( sbdkkey01.bdk_key,
                     (LPCTSTR) sbdkkey01List.bdkkey01_record[nNumberReturned-1].bdk_key,
                     sizeof (sbdkkey01.bdk_key));

            nSize = nNumberReturned - 1;
			}
			else
         {
				bDone = TRUE;
            nSize = nNumberReturned;
         }

			for (int nItem = 0; nItem <  nSize; nItem++,nRowItem ++) { // will now insert the items and subitems into the list view.
				m_CntlList.EnableWindow(TRUE);

			   for (int nSubItem = 0; nSubItem < 2; nSubItem++) {

			   lvItem.mask = LVIF_TEXT ;
				lvItem.iItem = (nSubItem == 0)? nRowItem : nActualItem;
				lvItem.iSubItem = nSubItem;

				if (nSubItem == 0)
				{
					lvItem.pszText = (char *)sbdkkey01List.bdkkey01_record[nItem].card_brand ;
				}
				else if(nSubItem == 1) 
				{
					lvItem.pszText = (char *)sbdkkey01List.bdkkey01_record[nItem].bdk_key ;
				}




				if (nSubItem == 0)
                nActualItem = m_CntlList.InsertItem(nCount+nSize, (char *)sbdkkey01List.bdkkey01_record[nItem].card_brand);

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


void CBinList::PopulateRuleIDListDialog()
{
	CFont m_font;
	LV_ITEM lvItem;
	char strErrorMessage[200];

    int nRowItem = 0;
    int nActualItem = 0;
    int nNumberReturned;
    int nSize, nCount, rcode;
    bDone = FALSE;

    nCount = m_CntlList.GetItemCount();

 //   if ( strcmp ( sMerchantRecord.primary_key.organization_id, "" ) == 0 )
	      rcode = txdsapi_get_list ((pBYTE)&sCR01, sizeof (CR01),
									       (pBYTE)&sCR01List, sizeof (CR01_GUI_LIST),
									        0, CR01_DATA, strErrorMessage);
/*    else
	      rcode = txdsapi_get_list ((pBYTE)&sMerchantRecord, sizeof (MCF01),
									       (pBYTE)&sMerchantList, sizeof (MCF01_GUI_LIST),
									        ST2_DB_SELECT_BY_ORG_ID, MCF01_DATA, strErrorMessage);
*/
	if (rcode == PTEMSG_OK)
	{
			nNumberReturned = atoi ((char *)&sCR01List.num_returned);
         if( nNumberReturned == GUI_MAX_LIST_SIZE)
         {
			    memcpy( sCR01.rule_id,
                     (LPCTSTR) sCR01List.CR01_record[nNumberReturned-1].rule_id,
                     sizeof (sCR01.rule_id));

            nSize = nNumberReturned - 1;
			}
			else
         {
				bDone = TRUE;
            nSize = nNumberReturned;
         }

			for (int nItem = 0; nItem <  nSize; nItem++,nRowItem ++) { // will now insert the items and subitems into the list view.
				m_CntlList.EnableWindow(TRUE);

			   for (int nSubItem = 0; nSubItem < 4; nSubItem++) {

			   lvItem.mask = LVIF_TEXT ;
				lvItem.iItem = (nSubItem == 0)? nRowItem : nActualItem;
				lvItem.iSubItem = nSubItem;

				if (nSubItem == 0)
				{
					lvItem.pszText = (char *)sCR01List.CR01_record[nItem].rule_id ;
				}


				if (nSubItem == 0)
                nActualItem = m_CntlList.InsertItem(nCount+nSize, (char *)sCR01List.CR01_record[nItem].rule_id);

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

void CBinList::PopulateTerminalListDialog()
{
	CFont m_font;
	LV_ITEM lvItem;
	char strErrorMessage[200];
   int nSize;
    
    int nRowItem = 0;
    int nActualItem = 0;
    int nNumberReturned;
    int nCount = m_CntlList.GetItemCount();
    bDone = FALSE;
	int rcode = txdsapi_get_list ((pBYTE)&sTerminalRecord, sizeof(DCF01), 
								(pBYTE) &sTerminalList,sizeof(DCF01_GUI_LIST),0,DCF01_DATA, strErrorMessage);
	if (rcode == PTEMSG_OK)
	{

			nNumberReturned = atoi ((char *)&sTerminalList.num_returned);
            if( nNumberReturned == GUI_MAX_LIST_SIZE)
            {				
			    memcpy( sTerminalRecord.merchant_id,				(LPCTSTR) sTerminalList.dcf01_record[nNumberReturned-1].merchant_id, sizeof (sTerminalRecord.merchant_id)); 
				memcpy( sTerminalRecord.organization_id,			(LPCTSTR) sTerminalList.dcf01_record[nNumberReturned-1].organization_id, sizeof (sTerminalRecord.organization_id)); 
				memcpy( sTerminalRecord.primary_key.device_id,(LPCTSTR) sTerminalList.dcf01_record[nNumberReturned-1].primary_key.device_id, sizeof (sTerminalRecord.primary_key.device_id)); 
            nSize = nNumberReturned - 1;			
			}
			else
         {
				bDone = TRUE;
            nSize = nNumberReturned;			
         }
						
			for (int nItem = 0; nItem <  nSize; nItem++,nRowItem ++) { // will now insert the items and subitems into the list view.
				m_CntlList.EnableWindow(TRUE);  
			    for (int nSubItem = 0; nSubItem < 3; nSubItem++) {					
					
			    lvItem.mask = LVIF_TEXT ;
				lvItem.iItem = (nSubItem == 0)? nRowItem : nActualItem;
				lvItem.iSubItem = nSubItem;

				if (nSubItem == 0) { 
					lvItem.pszText = (char *)sTerminalList.dcf01_record[nItem].primary_key.device_id;                   
				}
				else if (nSubItem == 1) { 
					lvItem.pszText = (char *)sTerminalList.dcf01_record[nItem].organization_id;                   
				}                
				else if (nSubItem == 2) { 
					lvItem.pszText = (char *)sTerminalList.dcf01_record[nItem].merchant_id;                   
				}
						
				if (nSubItem == 0) 
					nActualItem = m_CntlList.InsertItem(nCount+nSize,(char *)sTerminalList.dcf01_record[nItem].primary_key.device_id);
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

void CBinList::PopulateBinListDialog( int nST2 )
{
	CFont m_font;
	LV_ITEM lvItem;
	char strErrorMessage[200];
   int nSize;
    
    int nRowItem = 0;
    int nActualItem = 0;
    int nNumberReturned;
    int nCount = m_CntlList.GetItemCount();
    bDone = FALSE;

	int rcode = txdsapi_get_list ((pBYTE)&sBinRecord, sizeof (BIN01),
									 (pBYTE)&sBinList, sizeof(BIN01_GUI_LIST),
									 nST2, BIN01_DATA, strErrorMessage);
	if (rcode == PTEMSG_OK)
	{

			nNumberReturned = atoi ((char *)&sBinList.num_returned);
            if( nNumberReturned == GUI_MAX_LIST_SIZE)
            {				
			    memcpy( sBinRecord.primary_key.length, (LPCTSTR) sBinList.bin_range_list[nNumberReturned-1].primary_key.length, sizeof (sBinRecord.primary_key.length)); 
				memcpy( sBinRecord.primary_key.low, (LPCTSTR) sBinList.bin_range_list[nNumberReturned-1].primary_key.low, sizeof (sBinRecord.primary_key.low)); 
				memcpy( sBinRecord.primary_key.type, (LPCTSTR) sBinList.bin_range_list[nNumberReturned-1].primary_key.type, sizeof (sBinRecord.primary_key.type)); 
            nSize = nNumberReturned - 1;			
			
			}
			else
         {
				bDone = TRUE;
            nSize = nNumberReturned;			
         }
						
			for (int nItem = 0; nItem <  nSize; nItem++,nRowItem ++) { // will now insert the items and subitems into the list view.
				m_CntlList.EnableWindow(TRUE);  
			    for (int nSubItem = 0; nSubItem < 5; nSubItem++) {					
					
			    lvItem.mask = LVIF_TEXT ;
				lvItem.iItem = (nSubItem == 0)? nRowItem : nActualItem;
				lvItem.iSubItem = nSubItem;

				if (nSubItem == 0) { 
					lvItem.pszText = (char *)sBinList.bin_range_list[nItem].primary_key.low;                   
				}
				else if (nSubItem == 1) { 
					lvItem.pszText = (char *)sBinList.bin_range_list[nItem].high;                    
				}
				else if (nSubItem == 2) { 												
					lvItem.pszText = (char *)sBinList.bin_range_list[nItem].primary_key.length;			
				}
				else if (nSubItem == 3) { 
					char strTemp[20] = "";
                   // CString CstrTemp;
                    memset (strTemp, 0, sizeof(strTemp)) ;
							
					//CstrTemp.Empty() ;
					if ( strcmp((char *)sBinList.bin_range_list[nItem].primary_key.type,"C") == 0)
						strcpy (strTemp,"CREDIT");
					else if ( strcmp((char *)sBinList.bin_range_list[nItem].primary_key.type,"F") == 0)
						strcpy (strTemp,"FLEET");
					else if ( strcmp((char *)sBinList.bin_range_list[nItem].primary_key.type,"E") == 0)
						strcpy (strTemp,"DEFERRED");
					if ( strcmp((char *)sBinList.bin_range_list[nItem].primary_key.type,"D") == 0)
						strcpy (strTemp, "DEBIT");
					if ( strcmp((char *)sBinList.bin_range_list[nItem].primary_key.type,"L") == 0)
					strcpy (strTemp, "LOYALTY");
							
							
					lvItem.pszText = strTemp ;
				} 
				else if (nSubItem == 4) { 
					lvItem.pszText = (char *)sBinList.bin_range_list[nItem].description;                   
				}
                
						
				if (nSubItem == 0) 
					nActualItem = m_CntlList.InsertItem(nCount+nSize,(char *)sBinList.bin_range_list[nItem].primary_key.low);
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

void CBinList::PopulateDEFListDialog()
{
	CFont m_font;
	LV_ITEM lvItem;
	char strErrorMessage[200];
    int nSize;
    
    int nRowItem = 0;
    int nActualItem = 0;
    int nNumberReturned;
    int nCount = m_CntlList.GetItemCount();
    bDone = FALSE;

	int rcode = txdsapi_get_list ((pBYTE)&sDEFRecord, sizeof (DEF01),
								  (pBYTE)&sDEFList, sizeof(DEF01_GUI_LIST),
									 0, DEF01_DATA, strErrorMessage);
	if (rcode == PTEMSG_OK)
	{

			nNumberReturned = atoi ((char *)&sDEFList.num_returned);
            if( nNumberReturned == GUI_MAX_LIST_SIZE)
            {				
			    memcpy( sDEFRecord.primary_key.merchant_id, (LPCTSTR) sDEFList.def_list[nNumberReturned-1].primary_key.merchant_id,  sizeof (sDEFRecord.primary_key.merchant_id)); 
				memcpy( sDEFRecord.primary_key.organization_id, (LPCTSTR) sDEFList.def_list[nNumberReturned-1].primary_key.organization_id, sizeof (sDEFRecord.primary_key.organization_id)); 
				memcpy( sDEFRecord.primary_key.term_length, (LPCTSTR) sDEFList.def_list[nNumberReturned-1].primary_key.term_length,    sizeof (sDEFRecord.primary_key.term_length)); 
            nSize = nNumberReturned - 1;			
			
			}
			else
         {
				bDone = TRUE;
            nSize = nNumberReturned;			
         }
						
			for (int nItem = 0; nItem <  nSize; nItem++,nRowItem ++) { // will now insert the items and subitems into the list view.
				m_CntlList.EnableWindow(TRUE);  
			    for (int nSubItem = 0; nSubItem < 5; nSubItem++) {					
					
			    lvItem.mask = LVIF_TEXT ;
				lvItem.iItem = (nSubItem == 0)? nRowItem : nActualItem;
				lvItem.iSubItem = nSubItem;

				if (nSubItem == 0) { 
					lvItem.pszText = (char *)sDEFList.def_list[nItem].primary_key.merchant_id;                   
				}
				else if (nSubItem == 1) { 
					lvItem.pszText = (char *)sDEFList.def_list[nItem].primary_key.organization_id;             
				}
				else if (nSubItem == 2) { 												
					lvItem.pszText = (char *)sDEFList.def_list[nItem].primary_key.term_length;			
				}

                
						
				if (nSubItem == 0) 
					nActualItem = m_CntlList.InsertItem(nCount+nSize,(char *)sDEFList.def_list[nItem].primary_key.merchant_id);
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

void CBinList::OnOK() 
{
    
	int nRowSelected = m_CntlList.GetNextItem (-1, LVNI_SELECTED) ;

	if( nRowSelected == -1 && m_nDialogMode != MODE_BATCH && m_nDialogMode != MODE_BATCH_HISTORY )
	{
		m_StrMessageLbl = "Please select an item";
		UpdateData(FALSE);	
		return;
	}
	else
	{
		if ( m_nDialogMode == MODE_BIN )
		{
			strLow = m_CntlList.GetItemText (nRowSelected, 0) ;
			strLength = m_CntlList.GetItemText (nRowSelected, 2) ;
			strType1 = m_CntlList.GetItemText (nRowSelected, 3) ;
		}

		if ( m_nDialogMode == MODE_RULE_GENERAL )
		{
			strBinLow = m_CntlList.GetItemText (nRowSelected, 0) ;
			strBinHigh = m_CntlList.GetItemText (nRowSelected, 1) ;

		}

		if ( m_nDialogMode == MODE_MERCHANT )
		{
			strOrganizationID = m_CntlList.GetItemText (nRowSelected, 0) ;
			strMerchantID = m_CntlList.GetItemText (nRowSelected, 1) ;
		}
			if ( m_nDialogMode == MODE_OBS01 )
		{
			 Stronbehalfresult = m_CntlList.GetItemText (nRowSelected, 0) ;
			 Stronbehalfservice = m_CntlList.GetItemText (nRowSelected, 1) ;
			Strresponsecode = m_CntlList.GetItemText (nRowSelected, 2) ;
			Strdescription = m_CntlList.GetItemText (nRowSelected, 3) ;
		}

			if ( m_nDialogMode == MODE_VISA3DS2 )
		{
			 StrCAVV = m_CntlList.GetItemText (nRowSelected, 0) ;
			 StrCAVVresponsecode = m_CntlList.GetItemText (nRowSelected, 1) ;
			 StrCAVVdescription = m_CntlList.GetItemText (nRowSelected, 2) ;
		}

		if ( m_nDialogMode == MODE_RULEID )
		{
			 StrRule_ID = m_CntlList.GetItemText (nRowSelected, 0) ;
			
		}

		if ( m_nDialogMode == MODE_JCB3DS2 )
		{
			 Str3DS_Authentiction_RC_Value = m_CntlList.GetItemText (nRowSelected, 0) ;
			 Str3DS_Authentiation_Deline_Response_Code = m_CntlList.GetItemText (nRowSelected, 1) ;
			Str3DS_AUTHENTICATION_VERIFY_FLAG=m_CntlList.GetItemText (nRowSelected, 2) ;
		}

		if ( m_nDialogMode == MODE_BDK_KEY01 )
		{
			 StrCard_brand = m_CntlList.GetItemText (nRowSelected, 0) ;
			 StrBDK_key    = m_CntlList.GetItemText (nRowSelected, 1) ;
		}

		if ( m_nDialogMode == MODE_CURRENCY_CODE_LIST )
		{
			 StrCurrency_Value = m_CntlList.GetItemText (nRowSelected, 0) ;
			 StrCurrency_code  = m_CntlList.GetItemText (nRowSelected, 1) ;
			StrDecimal_places  = m_CntlList.GetItemText (nRowSelected, 2) ;
			StrCurrency_Name   = m_CntlList.GetItemText (nRowSelected, 3) ;
		}

		if ( m_nDialogMode == MODE_TERMINAL )
		{
			strItem0 = m_CntlList.GetItemText (nRowSelected, 0) ;
			strItem1 = m_CntlList.GetItemText (nRowSelected, 2) ;
			strItem2 = m_CntlList.GetItemText (nRowSelected, 1) ;

		}
		if ( m_nDialogMode == MODE_DEFERRED )
		{
			strItem0 = m_CntlList.GetItemText (nRowSelected, 0) ;
			strItem1 = m_CntlList.GetItemText (nRowSelected, 1) ;
			strItem2 = m_CntlList.GetItemText (nRowSelected, 2) ;

		}
		if ( m_nDialogMode == MODE_NEGBIN )
		{
			strItem0 = m_CntlList.GetItemText (nRowSelected, 0) ;
			strItem1 = m_CntlList.GetItemText (nRowSelected, 1) ;
		}
		if ( m_nDialogMode == MODE_PRIMACC )
		{
			m_strPan = m_CntlList.GetItemText (nRowSelected, 0) ;
		}

		if ( m_nDialogMode == MODE_PROFID )
		{
			m_strProfId = m_CntlList.GetItemText (nRowSelected, 0) ;
			m_strType = m_CntlList.GetItemText (nRowSelected, 1) ;
		}

		if ( m_nDialogMode == MODE_PROFIDBYTYPE )
		{
			m_strProfId = m_CntlList.GetItemText (nRowSelected, 0) ;
		}

		if ( m_nDialogMode == MODE_STATUS )
		{
			m_strStatus = m_CntlList.GetItemText (nRowSelected, 0) ;
		}
		
		if ( m_nDialogMode == MODE_CCF )
		{
			m_strCardNbr = m_CntlList.GetItemText (nRowSelected, 0) ;
			m_strCardType = m_CntlList.GetItemText (nRowSelected, 1) ;
		}

		if ( m_nDialogMode == MODE_RELATED )
		{
			strItem0 = m_CntlList.GetItemText (nRowSelected, 0) ;
			strItem1 = m_CntlList.GetItemText (nRowSelected, 1) ;	
		}
		if ( m_nDialogMode == MODE_PURCHASE )
		{
			strItem0 = m_CntlList.GetItemText (nRowSelected, 0) ;
			strItem1 = m_CntlList.GetItemText (nRowSelected, 1) ;	
		}
		if ( m_nDialogMode == MODE_OMC01 )
		{
			strItem1 = m_CntlList.GetItemText (nRowSelected, 0) ;
			strItem2 = m_CntlList.GetItemText (nRowSelected, 1) ;
		}
		if(m_nDialogMode = MODE_MGF01)
		{
			strItem0 = m_CntlList.GetItemText (nRowSelected, 0) ;
		}
		if(m_nDialogMode = MODE_MERCHANT_PAY_PARAM)
		{
			strItem0 = m_CntlList.GetItemText (nRowSelected, 0) ;
		}


	}
	
	CDialog::OnOK();
}

void CBinList::OnDblclkNameList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	OnOK();
	
	*pResult = 0;
}

void CBinList::PopulatePAFListDialog()
{	
	CFont m_font;
	LV_ITEM lvItem;
	char strErrorMessage[200];
    
    int nRowItem = 0;
    int nActualItem = 0;
    int nNumberReturned;
    bDone = FALSE;
    int nSize;
    int nCount = m_CntlList.GetItemCount();

	int rcode = txdsapi_get_list ((pBYTE)&sPAFRecord, sizeof(PAF01),
								(pBYTE)&sPAFList, sizeof(PAF01_GUI_LIST),
								 0, PAF01_DATA, strErrorMessage);
	if (rcode == PTEMSG_OK)
	{

			nNumberReturned = atoi ((char *)&sPAFList.num_returned);
            if( nNumberReturned == GUI_MAX_LIST_SIZE)
            {				
			    memcpy( sPAFRecord.primary_key.primary_acct_nbr, (LPCTSTR) sPAFList.paf_info[nNumberReturned-1].primary_acct_nbr, sizeof (sPAFRecord.primary_key.primary_acct_nbr)); 
			    memcpy( sPAFRecord.name_dba, (LPCTSTR) sPAFList.paf_info[nNumberReturned-1].name_dba, sizeof (sPAFRecord.name_dba)); 
            nSize = nNumberReturned - 1;			
			}
			else
         {
				bDone = TRUE;
            nSize = nNumberReturned;			
         }
						
			for (int nItem = 0; nItem <  nSize; nItem++,nRowItem ++) { // will now insert the items and subitems into the list view.
				m_CntlList.EnableWindow(TRUE);  
			    for (int nSubItem = 0; nSubItem < 3; nSubItem++) {					
					
			    lvItem.mask = LVIF_TEXT ;
				lvItem.iItem = (nSubItem == 0)? nRowItem : nActualItem;
				lvItem.iSubItem = nSubItem;

				if (nSubItem == 0) { 
					lvItem.pszText = (char *)sPAFList.paf_info[nItem].primary_acct_nbr;                   
				}
				else if (nSubItem == 1) { 
					lvItem.pszText = (char *)sPAFList.paf_info[nItem].name_dba;                   
				}
				
                
						
				if (nSubItem == 0) 
			//		nActualItem = m_CntlList.InsertItem(&lvItem);
				nActualItem = m_CntlList.InsertItem(nCount+nSize,(char *)sPAFList.paf_info[nItem].primary_acct_nbr);
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




void CBinList::PopulateCAFListDialog()
{
	CFont m_font;
	LV_ITEM lvItem;
	char strErrorMessage[200];
	CString temp;
	char strType[20];
	int rcode;
        
    int nRowItem = 0;
    int nActualItem = 0;
    int nNumberReturned;
    bDone = FALSE;
    int nSize, nCount;
    nCount = m_CntlList.GetItemCount();

	if (m_nDialogMode == MODE_PROFID)
		rcode = txdsapi_get_list ((pBYTE)&sCAFRecord, sizeof(CAF01),
									(pBYTE)&sCAFList, sizeof(CAF01_GUI_LIST),
									0,CAF01_DATA, strErrorMessage);
	else
		rcode = txdsapi_get_list ((pBYTE)&sCAFRecord, sizeof(CAF01),
									(pBYTE)&sCAFList, sizeof(CAF01_GUI_LIST),
									ST2_DB_GET_CAF01_LIST_BY_TYPE,
									CAF01_DATA, strErrorMessage);
	if (rcode == PTEMSG_OK)
	{

			nNumberReturned = atoi ((char *)&sCAFList.num_returned);
            if( nNumberReturned == GUI_MAX_LIST_SIZE)
            {				
			    memcpy( sCAFRecord.primary_key.caf_profile_id, (LPCTSTR) sCAFList.caf_info[nNumberReturned-1].caf_profile_id, sizeof (sCAFRecord.primary_key.caf_profile_id)); 
			    memcpy( sCAFRecord.primary_key.type, (LPCTSTR) sCAFList.caf_info[nNumberReturned-1].type, sizeof (sCAFRecord.primary_key.type)); 
            nSize = nNumberReturned - 1;			
			}
			else
         {
				bDone = TRUE;
            nSize = nNumberReturned;			
         }
						
			for (int nItem = 0; nItem <  nSize; nItem++,nRowItem ++) { // will now insert the items and subitems into the list view.
				m_CntlList.EnableWindow(TRUE);  
			    for (int nSubItem = 0; nSubItem < 2; nSubItem++) {					
					
			    lvItem.mask = LVIF_TEXT ;
				lvItem.iItem = (nSubItem == 0)? nRowItem : nActualItem;
				lvItem.iSubItem = nSubItem;

				if (nSubItem == 0) { 
					lvItem.pszText = (char *)sCAFList.caf_info[nItem].caf_profile_id;                   
				}
				else if (nSubItem == 1) { 
					temp = (char *)sCAFList.caf_info[nItem].type;
					if ( strcmp ( temp, "C" ) == 0 )
						temp = "CREDIT";
					else if ( strcmp ( temp, "D" ) == 0 )
						temp = "DEBIT";
					else if ( strcmp ( temp, "E" ) == 0 )
						temp = "DEFERRED";
					else if ( strcmp ( temp, "L" ) == 0 )
						temp = "LOYALTY";
					else if ( strcmp ( temp, "F" ) == 0 )
						temp = "FLEET";
					memcpy ( strType, temp, sizeof (strType) );
					lvItem.pszText = (char *)strType;                   
				}
				
                  
						
				if (nSubItem == 0) 
					nActualItem = m_CntlList.InsertItem(nCount+nSize,(char *)sCAFList.caf_info[nItem].caf_profile_id);
				else 
					m_CntlList.SetItem(&lvItem);


						
			} // for
		} // for 
				m_CntlList.SetFocus() ;
				int nCount = m_CntlList.GetItemCount();

				m_CntlList.EnsureVisible( nCount-1, TRUE);
		}
}


void CBinList::PopulateCSFListDialog()
{	
	CFont m_font;
	LV_ITEM lvItem;
	char strErrorMessage[200];
    int nSize;
    int nRowItem = 0;
    int nActualItem = 0;
    int nNumberReturned;
    bDone = FALSE;
	int nCount = m_CntlList.GetItemCount();

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
				m_CntlList.EnableWindow(TRUE);  
			    for (int nSubItem = 0; nSubItem < 3; nSubItem++) {					
					
			    lvItem.mask = LVIF_TEXT ;
				lvItem.iItem = (nSubItem == 0)? nRowItem : nActualItem;
				lvItem.iSubItem = nSubItem;

				if (nSubItem == 0) { 
					lvItem.pszText = (char *)sCSFList.csf01_record[nItem].primary_key.status;                   
				}
				else if (nSubItem == 1) { 
					lvItem.pszText = (char *)sCSFList.csf01_record[nItem].description;                   
				}
				
                
						
				if (nSubItem == 0) 
			//		nActualItem = m_CntlList.InsertItem(&lvItem);
					nActualItem = m_CntlList.InsertItem(nCount+nSize,(char *)sCSFList.csf01_record[nItem].primary_key.status);
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

void CBinList::PopulateOMCListDialog(  )
{	
	CFont m_font;
	LV_ITEM lvItem;
	char strErrorMessage[200];
	char tmpDateStr[20] = {0};
	CString temp;
    int nRowItem = 0;
    int nActualItem = 0;
    int nNumberReturned;
    int nSize, nCount;
    bDone = FALSE;
    nCount = m_CntlList.GetItemCount();
	int nST2 = 0;
	CMyUtils myUtilsObject;

    bDone = FALSE;
	int rcode = txdsapi_get_list ((pBYTE)&sOMC01Record, sizeof(OMC01),
									(pBYTE)&sOMC01List, sizeof(OMC01_GUI_LIST),
									nST2, OMC01_DATA, strErrorMessage);
	if (rcode == PTEMSG_OK)
	{

			nNumberReturned = atoi ((char *)&sOMC01List.num_returned);
            if( nNumberReturned == GUI_MAX_LIST_SIZE)
            {				
				memcpy(sOMC01Record.primary_key.merchant_id,(LPCTSTR) sOMC01List.omc01_pk[nNumberReturned-1].merchant_id,	sizeof (sOMC01Record.primary_key.merchant_id));
				memcpy(sOMC01Record.primary_key.card_nbr,	(LPCTSTR) sOMC01List.omc01_pk[nNumberReturned-1].card_nbr,	sizeof (sOMC01Record.primary_key.card_nbr));
				memcpy(sOMC01Record.primary_key.max_repeat,	(LPCTSTR) sOMC01List.omc01_pk[nNumberReturned-1].max_repeat,	sizeof (sOMC01Record.primary_key.max_repeat));
				memcpy(sOMC01Record.primary_key.repeated,	(LPCTSTR) sOMC01List.omc01_pk[nNumberReturned-1].repeated,	sizeof (sOMC01Record.primary_key.repeated));
				memcpy(sOMC01Record.primary_key.expiry_date,(LPCTSTR) sOMC01List.omc01_pk[nNumberReturned-1].expiry_date,	sizeof (sOMC01Record.primary_key.expiry_date));
				memcpy(sOMC01Record.primary_key.peso_amt_limit,(LPCTSTR) sOMC01List.omc01_pk[nNumberReturned-1].peso_amt_limit,	sizeof (sOMC01Record.primary_key.peso_amt_limit));
				memcpy(sOMC01Record.primary_key.usd_amt_limit,(LPCTSTR) sOMC01List.omc01_pk[nNumberReturned-1].usd_amt_limit,	sizeof (sOMC01Record.primary_key.usd_amt_limit));

				nSize = nNumberReturned - 1;			
			}
			else
         {
				bDone = TRUE;
            nSize = nNumberReturned;			
         }
						
			for (int nItem = 0; nItem <  nSize; nItem++,nRowItem ++) { // will now insert the items and subitems into the list view.
				m_CntlList.EnableWindow(TRUE);
				for(int ic = 0; ic < m_CntlList.GetItemCount(); ic++)
				{
					CString m_id = m_CntlList.GetItemText(ic,0);
					CString c_nb = m_CntlList.GetItemText(ic,1);
				}
			    for (int nSubItem = 0; nSubItem < 9; nSubItem++) {					
					
			    lvItem.mask = LVIF_TEXT ;
				lvItem.iItem = (nSubItem == 0)? nRowItem : nActualItem;
				lvItem.iSubItem = nSubItem;

				if (nSubItem == 0) {
					lvItem.pszText = (char *)sOMC01List.omc01_pk[nItem].merchant_id;                   
				}
				else if (nSubItem == 1) { 
					lvItem.pszText = (char *)sOMC01List.omc01_pk[nItem].card_nbr;
				}
				else if (nSubItem == 2) { 
					
					strcpy(tmpDateStr, myUtilsObject.FormatDate( (char *)sOMC01List.omc01_pk[nItem].expiry_date));
					if(strlen(tmpDateStr) == 0)
					{
						strcpy(tmpDateStr, "Never Expires");
					}
					lvItem.pszText = tmpDateStr;
				}
				else if (nSubItem == 3) { 
					lvItem.pszText = (char *)sOMC01List.omc01_pk[nItem].max_repeat;
				}
				else if (nSubItem == 4) { 
					lvItem.pszText = (char *)sOMC01List.omc01_pk[nItem].peso_amt_limit;
				}
				else if (nSubItem == 5) { 
					lvItem.pszText = (char *)sOMC01List.omc01_pk[nItem].usd_amt_limit;
				}
				else if (nSubItem == 6) { 
					if(sOMC01List.omc01_pk[nItem].bypass[3]=='1')
					{
						char yes[2]="Y";
						lvItem.pszText = (char *)yes;
					}
					else
					{	
						char No[2]="N";
						lvItem.pszText = (char *)No;
					}
				}
				else if (nSubItem == 7) { 
					if(sOMC01List.omc01_pk[nItem].bypass[2]=='1')
					{
						char yes[2]="Y";
						lvItem.pszText = (char *)yes;
					}
					else
					{	
						char No[2]="N";
						lvItem.pszText = (char *)No;
					}
				}
				else if (nSubItem == 8) { 
					if(sOMC01List.omc01_pk[nItem].bypass[1]=='1')
					{
						char yes[2]="Y";
						lvItem.pszText = (char *)yes;
					}
					else
					{	
						char No[2]="N";
						lvItem.pszText = (char *)No;
					}
				}
				
				if (nSubItem == 0) 
			//		nActualItem = m_CntlList.InsertItem(&lvItem);
					nActualItem = m_CntlList.InsertItem(nCount+nSize,(char *)sOMC01List.omc01_pk[nItem].merchant_id);
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

void CBinList::PopulateCCF02ListDialog( int nST2 )
{	
	CFont m_font;
	LV_ITEM lvItem;
	char strErrorMessage[200];
	CString temp;
    int nRowItem = 0;
    int nActualItem = 0;
    int nNumberReturned;
    int nSize, nCount;
	char strType[20];
    bDone = FALSE;
    nCount = m_CntlList.GetItemCount();

    bDone = FALSE;
	int rcode = txdsapi_get_list ((pBYTE)&sCCF02Record, sizeof(CCF02P),
									(pBYTE)&sCCF02List, sizeof(CCF02P_GUI_LIST),
									nST2, CCF02P_DATA, strErrorMessage);
	if (rcode == PTEMSG_OK)
	{

			nNumberReturned = atoi ((char *)&sCCF02List.num_returned);
            if( nNumberReturned == GUI_MAX_LIST_SIZE)
            {				
			    memcpy( sCCF02Record.primary_key.card_type, (LPCTSTR) sCCF02List.ccf02p_pk[nNumberReturned-1].card_type, sizeof (sCCF02Record.primary_key.card_type)); 
			    memcpy( sCCF02Record.primary_key.card_nbr, (LPCTSTR) sCCF02List.ccf02p_pk[nNumberReturned-1].card_nbr, sizeof (sCCF02Record.primary_key.card_nbr)); 
            nSize = nNumberReturned - 1;			
			}
			else
         {
				bDone = TRUE;
            nSize = nNumberReturned;			
         }
						
			for (int nItem = 0; nItem <  nSize; nItem++,nRowItem ++) { // will now insert the items and subitems into the list view.
				m_CntlList.EnableWindow(TRUE);  
			    for (int nSubItem = 0; nSubItem < 3; nSubItem++) {					
					
			    lvItem.mask = LVIF_TEXT ;
				lvItem.iItem = (nSubItem == 0)? nRowItem : nActualItem;
				lvItem.iSubItem = nSubItem;

				if (nSubItem == 0) { 
					lvItem.pszText = (char *)sCCF02List.ccf02p_pk[nItem].card_nbr;                   
				}
				else if (nSubItem == 1) { 
					temp = (char *)sCCF02List.ccf02p_pk[nItem].card_type;
					if ( strcmp ( temp, "C" ) == 0 )
						temp = "CREDIT";
					else if ( strcmp ( temp, "D" ) == 0 )
						temp = "DEBIT";
					else if ( strcmp ( temp, "E" ) == 0 )
						temp = "DEFERRED";
					else if ( strcmp ( temp, "L" ) == 0 )
						temp = "LOYALTY";
					else if ( strcmp ( temp, "F" ) == 0 )
						temp = "FLEET";
					memcpy ( strType, temp, sizeof (strType) );
					lvItem.pszText = (char *)strType;                   
				}
				
                
						
				if (nSubItem == 0) 
			//		nActualItem = m_CntlList.InsertItem(&lvItem);
					nActualItem = m_CntlList.InsertItem(nCount+nSize,(char *)sCCF02List.ccf02p_pk[nItem].card_nbr);
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

void CBinList::PopulateMGF01ListDialog()
{
	CFont m_font;
	LV_ITEM lvItem;
	char strErrorMessage[200];
    
    int nRowItem = 0;
    int nActualItem = 0;
    int nNumberReturned;
    int nSize, nCount, rcode;
    bDone = FALSE;
       
    nCount = m_CntlList.GetItemCount();

	      rcode = txdsapi_get_list ((pBYTE)&sMGF01Record, sizeof (MGF01), 
									       (pBYTE)&sMGF01List, sizeof (MGF01_GUI_LIST),
									        0, MGF01_DATA, strErrorMessage);
	if (rcode == PTEMSG_OK)
	{
			nNumberReturned = atoi ((char *)&sMGF01List.num_returned);
         if( nNumberReturned == GUI_MAX_LIST_SIZE)
         {				
			    memcpy( sMGF01Record.primary_key.merchant_group_id, 
                     (LPCTSTR) sMGF01List.merchant_group_info[nNumberReturned-1].merchant_group_id, 
                     sizeof (sMGF01Record.primary_key.merchant_group_id)); 
				 
            nSize = nNumberReturned - 1;			
			}
			else
         {
				bDone = TRUE;
            nSize = nNumberReturned;			
         }
						
			for (int nItem = 0; nItem <  nSize; nItem++,nRowItem ++) { // will now insert the items and subitems into the list view.
				m_CntlList.EnableWindow(TRUE);  

			   for (int nSubItem = 0; nSubItem < 1; nSubItem++) {					
					
			   lvItem.mask = LVIF_TEXT ;
				lvItem.iItem = (nSubItem == 0)? nRowItem : nActualItem;
				lvItem.iSubItem = nSubItem;

				if (nSubItem == 0) { 
					lvItem.pszText = (char *)sMGF01List.merchant_group_info[nItem].merchant_group_id;                   
				}
				              
						
				if (nSubItem == 0) 
                nActualItem = m_CntlList.InsertItem(nCount+nSize, (char *)sMGF01List.merchant_group_info[nItem].merchant_group_id);
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

void CBinList::PopulateMPPARA01ListDialog()
{
	CFont m_font;
	LV_ITEM lvItem;
	char strErrorMessage[200];
    
    int nRowItem = 0;
    int nActualItem = 0;
    int nNumberReturned;
    int nSize, nCount, rcode;
    bDone = FALSE;
       
    nCount = m_CntlList.GetItemCount();

	      rcode = txdsapi_get_list ((pBYTE)&sMPPARA01Record, sizeof (MPPARA01), 
									       (pBYTE)&sMPPARA01_GUI_LISTList, sizeof (MPPARA01_GUI_LIST),
									        0, MPPARA01_DATA, strErrorMessage);
	if (rcode == PTEMSG_OK)
	{
			nNumberReturned = atoi ((char *)&sMPPARA01_GUI_LISTList.num_returned);
         if( nNumberReturned == GUI_MAX_LIST_SIZE)
         {				
			    memcpy( sMPPARA01Record.primary_key.product_code, 
                     (LPCTSTR) sMPPARA01_GUI_LISTList.mppara_info[nNumberReturned-1].product_code, 
                     sizeof (sMPPARA01Record.primary_key.product_code)); 
				 
            nSize = nNumberReturned - 1;			
			}
			else
         {
				bDone = TRUE;
            nSize = nNumberReturned;			
         }
						
			for (int nItem = 0; nItem <  nSize; nItem++,nRowItem ++) { // will now insert the items and subitems into the list view.
				m_CntlList.EnableWindow(TRUE);  

			   for (int nSubItem = 0; nSubItem < 1; nSubItem++) {					
					
			   lvItem.mask = LVIF_TEXT ;
				lvItem.iItem = (nSubItem == 0)? nRowItem : nActualItem;
				lvItem.iSubItem = nSubItem;

				if (nSubItem == 0) { 
					lvItem.pszText = (char *)sMPPARA01_GUI_LISTList.mppara_info[nItem].product_code;                   
				}
				              
						
				if (nSubItem == 0) 
                nActualItem = m_CntlList.InsertItem(nCount+nSize, (char *)sMPPARA01_GUI_LISTList.mppara_info[nItem].product_code);
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

void CBinList::OnDetailsButton() 
{
	CTransactionDetails tranDlg;

	int nRowSelected = m_CntlList.GetNextItem (-1, LVNI_SELECTED) ;

	if( nRowSelected == -1)
	{
		m_StrMessageLbl = "Please select an item";
		UpdateData(FALSE);	
		return;
	}
	else
		tranDlg.m_strTranID = m_CntlList.GetItemText (nRowSelected, 7) ;

	tranDlg.m_nDialogMode = m_nDialogMode;
	tranDlg.DoModal();
}


void CBinList::OnAddButton() 
{
	CBatchDetails	batchDialog;
	int nReturnCode;
   BCH01 sBCH01Record;
   char strErrorMessage[300] = "";


	memcpy ( batchDialog.strDeviceID, sBatchRecord.primary_key.device_id, sizeof (sBatchRecord.primary_key.device_id ) );
	batchDialog.strOpenDate =  sBatchList.batch_list[0].open_date;
	batchDialog.strOpenTime =  sBatchList.batch_list[0].open_time;
	nReturnCode = batchDialog.DoModal();

   if ( m_CntlList.GetItemCount() == 0 )
   {
      memcpy ( sBCH01Record.primary_key.device_id,  sBatchRecord.primary_key.device_id, sizeof (sBCH01Record.primary_key.device_id) );
      memcpy ( sBCH01Record.primary_key.batch_nbr, sBatchRecord.primary_key.batch_nbr, sizeof (sBCH01Record.primary_key.batch_nbr) );
	  memcpy ( sBCH01Record.merchant_id, sBatchRecord.merchant_id, sizeof (sBCH01Record.merchant_id) );
      memcpy ( sBCH01Record.open_date, " ", sizeof (sBCH01Record.open_date) );
      memcpy ( sBCH01Record.open_time, " ", sizeof (sBCH01Record.open_time) );

	   int rcode = txdsapi_get_record ( (pBYTE)&sBCH01Record, sizeof (BCH01), BCH01_DATA, strErrorMessage);        
	   if ( rcode != PTEMSG_OK && rcode != PTEMSG_NOT_FOUND )
      {
			m_StrMessageLbl = strErrorMessage;
			UpdateData(FALSE);
		   return;
		}
      else
      {
	      memcpy (sBatchRecord.open_date , sBCH01Record.open_date, sizeof (sBatchRecord.open_date) );
	      memcpy (sBatchRecord.open_time , sBCH01Record.open_time, sizeof (sBatchRecord.open_time) );
      }

   }

   m_StrMessageLbl = "";
   UpdateData(FALSE);
 
	m_CntlList.DeleteAllItems();
	memcpy (sBatchRecord.primary_key.retrieval_ref_num ," ", sizeof (sBatchRecord.primary_key.retrieval_ref_num));
	PopulateBatchListDialog();

}

void CBinList::OnDeleteButton() 
{
   BCH10 sBCH10Record;
   CHAR strErrorMessage[200];
   CString strDeviceID, strOpenDate, strOpenTime, strRRN, strAmount;
   CMyUtils myUtilsObject;
   CHAR ErrorMessage[300] = "";
   CHAR unique_tran_str_id[20] = "";


   ZeroMemory ( &sBCH10Record, sizeof (BCH10) );

   UpdateData(TRUE);
	int nRowSelected = m_CntlList.GetNextItem (-1, LVNI_SELECTED) ;

	if( nRowSelected == -1)
	{
		m_StrMessageLbl = "Please select an item";
		UpdateData(FALSE);	
		return;
	}
   else
   {
		strDeviceID = sBatchList.batch_list[0].primary_key.device_id ;
        strOpenDate = sBatchList.batch_list[0].open_date ;
		strOpenTime = sBatchList.batch_list[0].open_time ;
 		strRRN = m_CntlList.GetItemText (nRowSelected, 1) ;
 		strAmount = myUtilsObject.GenerateTranAmount( m_CntlList.GetItemText (nRowSelected, 3), 12, ErrorMessage ) ;
  }

	int nReturnCode = AfxMessageBox ("Are you sure you want to delete Batch ?", MB_OKCANCEL|MB_ICONQUESTION) ;
	if (nReturnCode != IDOK) 
		return ;


	ptetime_get_strtime(unique_tran_str_id); 
	memset(sBCH10Record.transaction_id, 0, sizeof(sBCH10Record.transaction_id));
	memcpy(sBCH10Record.transaction_id, strDeviceID, sizeof(sBCH10Record.transaction_id) - 1);
	strncat((char*)&sBCH10Record.transaction_id, unique_tran_str_id+2, 12);

   memcpy ( sBCH10Record.primary_key.device_id, strDeviceID, sizeof (sBCH10Record.primary_key.device_id) );
   memcpy ( sBCH10Record.open_date, strOpenDate, sizeof (sBCH10Record.open_date) ); 
   memcpy ( sBCH10Record.open_time, strOpenTime, sizeof (sBCH10Record.open_time) ); 
   memcpy ( sBCH10Record.primary_key.retrieval_ref_num, strRRN, sizeof (sBCH10Record.primary_key.retrieval_ref_num) ); 
   memcpy ( sBCH10Record.primary_key.batch_nbr, "ACTIVE", sizeof (sBCH10Record.primary_key.batch_nbr) ); 
 //  memcpy ( sBCH10Record.tran_amount, strAmount, sizeof (sBCH10Record.tran_amount) ); 

   int rcode = txdsapi_delete_bch10_update_bch01 ( &sBCH10Record, strErrorMessage );
   if( PTEMSG_OK != rcode )
   {
	  m_StrMessageLbl = strErrorMessage;

	  if (!UpdateData(FALSE)){
		AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
	  }
   } // if
   else
   {
		m_CntlList.DeleteAllItems();
		memcpy (sBatchRecord.primary_key.retrieval_ref_num ," ", sizeof (sBatchRecord.primary_key.retrieval_ref_num));
		memcpy (sBatchRecord.open_date ,strOpenDate, sizeof (sBatchRecord.open_date));
		memcpy (sBatchRecord.open_time ,strOpenTime, sizeof (sBatchRecord.open_time));
		PopulateBatchListDialog();
   }
}

BOOL CBinList::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	// TODO: Add your specialized code here and/or call the base class
		c_time = CTime::GetCurrentTime();	
	shrmem.SetLastUpdateTime(c_time);
	return CDialog::OnCommand(wParam, lParam);
}
