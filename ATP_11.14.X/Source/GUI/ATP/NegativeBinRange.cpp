/*********************************************************************************
 Module: NegativeBinRange.cpp : implementation file

 Copyright (c) 1998, Hypercom, Inc.                         
 All Rights Reserved.                                       

 Description: Logic for Negative Bin Range property page
			  This module defines and uses NegativeBinRange class

 Application: ATP

 Author: Irina Arustamova

 Revision History:
  
 $Log:   N:\PVCS\PTE\Equitable\ATP_gui\NegativeBinRange.cpp  $
   
      Rev 1.8   Dec 01 2003 11:29:48   lmitchel
   Added AMEX Online Updates - scr1200
   
      Rev 1.7   Jan 05 2000 14:54:06   gbarnes
   Major update. Not updated to PVCS recently.
   PVCS was locked.
   
      Rev 1.1   Sep 22 1999 15:55:40   iarustam
   Added manual batch entry GUI
   
      Rev 1.0   Jul 01 1999 14:17:56   iarustam
   Initial Release
   
      Rev 1.0   Jun 28 1999 13:59:02   iarustam
   Initial Release
   
      Rev 1.4   May 10 1999 11:28:00   dperson
   Standardized error messages
   
      Rev 1.3   Apr 29 1999 14:56:46   iarustam
   Bug fixes
   
      Rev 1.2   Mar 22 1999 11:12:42   iarustam
   Bug fixes
   
   
      Rev 1.1   Mar 04 1999 13:32:18   iarustam
   Bug fixes
   
      Rev 1.0   Feb 23 1999 11:40:50   iarustam
   Initial Release
   

************************************************************************************/
// NegativeBinRange.cpp : implementation file
//

#include "stdafx.h"
#include "atp.h"
#include "NegativeBinRange.h"
#include "MyUtils.h"
#include "BinList.h"
#include "NameList.h"
#include "SharedMem.h"	

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern UCF01 ucf01;
extern CSharedMem shrmem;
/////////////////////////////////////////////////////////////////////////////
// CNegativeBinRange dialog


CNegativeBinRange::CNegativeBinRange(CWnd* pParent /*=NULL*/)
	: CDialog(CNegativeBinRange::IDD, pParent)
{
	//{{AFX_DATA_INIT(CNegativeBinRange)
	m_StrHighFld = _T("");
	m_StrLowFld = _T("");
	m_StrMessageLbl = _T("");
	//}}AFX_DATA_INIT
}


void CNegativeBinRange::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CNegativeBinRange)
	DDX_Control(pDX, IDC_LOW_FLD, m_CntlLowFld);
	DDX_Control(pDX, IDC_LIST_BUTTON, m_ListButton);
	DDX_Control(pDX, IDC_HIGH_FLD, m_CntlHighFld);
	DDX_Control(pDX, IDC_DELETE_BUTTON, m_DeleteButton);
	DDX_Control(pDX, IDC_ADD_BUTTON, m_AddButton);
	DDX_Text(pDX, IDC_HIGH_FLD, m_StrHighFld);
	DDV_MaxChars(pDX, m_StrHighFld, 16);
	DDX_Text(pDX, IDC_LOW_FLD, m_StrLowFld);
	DDV_MaxChars(pDX, m_StrLowFld, 16);
	DDX_Text(pDX, IDC_MESSAGE_LBL, m_StrMessageLbl);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CNegativeBinRange, CDialog)
	//{{AFX_MSG_MAP(CNegativeBinRange)
	ON_BN_CLICKED(IDC_ADD_BUTTON, OnAddButton)
	ON_BN_CLICKED(IDC_DELETE_BUTTON, OnDeleteButton)
	ON_BN_CLICKED(IDC_LIST_BUTTON, OnListButton)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CNegativeBinRange message handlers

BOOL CNegativeBinRange::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	CFont m_font;
	LOGFONT lf;

	memset ( &lf, 0, sizeof (lf));
	lf.lfHeight = 12;
	lf.lfWeight = 700;
	strcpy(lf.lfFaceName, "Arial");
	m_font.CreateFontIndirect (&lf);
	GetDlgItem(IDC_MESSAGE_LBL)->SetFont (&m_font);

	CString temp;
	int nIndex = -1;

	for ( int i = 0; i <=15; i++ )
	{
		temp = ucf01.control[i].subsystem_name;
		if ( strcmp ( temp, "Merchant Control" ) == 0)
			nIndex = i;
	}

	if ( nIndex != -1)
	{
		if ( ucf01.control[nIndex].action[0] == '0' )
			m_AddButton.EnableWindow(FALSE);
		if ( ucf01.control[nIndex].action[1] == '0' )
			m_DeleteButton.EnableWindow(FALSE);
	
	}



	UpdateData(FALSE);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CNegativeBinRange::OnAddButton() 
{
	NBF01 sNegBINRecord;
	char strErrorMessage[200] = "";
	double nHigh,nLow;

	if (!CheckDialogForCompleteness())
		return;

	nHigh = atof ( m_StrHighFld);
	nLow = atof ( m_StrLowFld );

	if ( nHigh == 0 )
	{
		m_StrMessageLbl = "ERROR: BIN High cannot be 0";	
		m_CntlHighFld.SetFocus();
		if (!UpdateData(FALSE)){
			AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
		}
		return ;
	}


	if ( nLow == 0 )
	{
		m_StrMessageLbl = "ERROR: BIN Low cannot be 0";	
		m_CntlHighFld.SetFocus();
		if (!UpdateData(FALSE)){
			AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
		}
		return ;
	}


	if ( m_StrHighFld < m_StrLowFld )
	{
		m_StrMessageLbl = "ERROR: BIN High cannot be less than BIN Low";	
		m_CntlHighFld.SetFocus();
		if (!UpdateData(FALSE)){
			AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
		}
		return ;
	}

	nHigh = m_StrHighFld.GetLength();
	nLow = m_StrLowFld.GetLength();

	if ( nHigh != nLow )
	{
		m_StrMessageLbl = "ERROR: Incorrect number of digits in BIN Low/High";	
		m_CntlHighFld.SetFocus();
		if (!UpdateData(FALSE)){
			AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
		}
		return ;
	}

	memset(&sNegBINRecord, 0, sizeof(NBF01)) ;

	memcpy (sNegBINRecord.primary_key.low, m_StrLowFld, sizeof (sNegBINRecord.primary_key.low));
	memcpy (sNegBINRecord.primary_key.high, m_StrHighFld, sizeof (sNegBINRecord.primary_key.high));
		
	int rcode = txdsapi_get_record ( (pBYTE)&sNegBINRecord, 
											 sizeof (NBF01), NBF01_DATA, strErrorMessage );
	if ( rcode == PTEMSG_OK )  
	{  	
		m_StrMessageLbl = "ERROR: BIN Range already exists";
		if (!UpdateData(FALSE)){
			AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
		}
		((CPropertySheet*) this->GetParent())->SetActivePage(0);
		return ;
	}
    else if ( rcode == PTEMSG_NOT_FOUND )
	{
		rcode = txdsapi_insert_record ( (pBYTE)&sNegBINRecord, 
												 sizeof (NBF01), NBF01_DATA, strErrorMessage );
		if ( rcode != PTEMSG_OK )
		{  	
			m_StrMessageLbl = strErrorMessage;
			if (!UpdateData(FALSE)){
				AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
			}
			((CPropertySheet*) this->GetParent())->SetActivePage(0);
			return ;
		}
		else 
		{            
			m_StrMessageLbl = "Record Added";	
			if (!UpdateData(FALSE)){
				AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
			}
		}
	}
	
}

void CNegativeBinRange::OnDeleteButton() 
{
	NBF01 sNegBINRecord;
	CString strMessage;
	char strErrorMessage[200] = "";
	CNameList listDlg;

	m_StrMessageLbl = "";

    if (!CheckDialogForCompleteness())
        return;   

   
    if (!UpdateData(TRUE)){
		AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
	}
    
	strMessage.Format ("Are you sure you want to delete Low %s, High %s?", (LPCTSTR) m_StrLowFld,(LPCTSTR) m_StrHighFld) ;

	int nReturnCode = AfxMessageBox ((LPCTSTR)strMessage, MB_OKCANCEL|MB_ICONQUESTION) ;
	if (nReturnCode != IDOK) 
		return ;

	memcpy (sNegBINRecord.primary_key.low, m_StrLowFld, sizeof (sNegBINRecord.primary_key.low));
	memcpy (sNegBINRecord.primary_key.high, m_StrHighFld, sizeof (sNegBINRecord.primary_key.high));

	ZeroMemory (&listDlg.sNegGroupRecord, sizeof (NGF01));
	memcpy (listDlg.sNegGroupRecord.bin_range[0].low, m_StrLowFld, sizeof (listDlg.sNegGroupRecord.bin_range[0].low));
	memcpy (listDlg.sNegGroupRecord.bin_range[0].high, m_StrHighFld, sizeof (listDlg.sNegGroupRecord.bin_range[0].high));
	memcpy (listDlg.sNegGroupRecord.primary_key.negative_group_id, " ", sizeof (listDlg.sNegGroupRecord.primary_key.negative_group_id));

	int rcode = txdsapi_get_list ((pBYTE)&listDlg.sNegGroupRecord, sizeof (NGF01), 
									(pBYTE)&listDlg.sNGFList, sizeof (NGF01_LIST),
									ST2_DB_GET_NGF01_LIST_BY_BINRANGE, NGF01_DATA, strErrorMessage);

	if ( strcmp ((char*)&listDlg.sNGFList.num_returned, "0") != 0)
	{
		strMessage = "You cannot delete this BIN range because it currently has negative merchant groups assigned to it. \nDo you want to see the list of negative merchant groups assigned to this BIN range?";

		nReturnCode = AfxMessageBox ((LPCTSTR)strMessage, MB_OKCANCEL|MB_ICONQUESTION) ;
		if (nReturnCode != IDOK) return ;

	    listDlg.m_nIndicator = MODE_DISPLAY_NEG_GROUP_BY_BIN_RANGE;
   
	    nReturnCode = listDlg.DoModal();
		
	    return;

	}


    rcode = txdsapi_delete_record ((pBYTE)&sNegBINRecord, sizeof ( NBF01), NBF01_DATA, strErrorMessage) ;
	if ( rcode != PTEMSG_OK )
    {
		 switch( rcode )
		{
         case PTEMSG_NOT_FOUND:
			 sprintf( strErrorMessage, "ERROR: Negative BIN does not exist");
            break;
		}

		m_StrMessageLbl = strErrorMessage;
		if (!UpdateData(FALSE)){
			AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
		}
		return ;
	}

    else
    {
         m_StrMessageLbl = "Record Deleted";
        	m_StrHighFld = _T("");
        	m_StrLowFld = _T("");
	
		if (!UpdateData(FALSE)){
			AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
		}
	}
        
	
}

void CNegativeBinRange::OnListButton() 
{
	CBinList listDlg;
    int nReturnCode;

	UpdateData(TRUE);
	m_StrMessageLbl = "";
	UpdateData(FALSE);

	listDlg.m_nDialogMode = MODE_NEGBIN;
	listDlg.strItem0 = m_StrLowFld;
	listDlg.strItem1 = m_StrHighFld;

    nReturnCode = listDlg.DoModal();
    if ( nReturnCode == IDOK)
	{
        m_StrLowFld = listDlg.strItem0;
		m_StrHighFld = listDlg.strItem1;	
	}
        
    if (!UpdateData(FALSE)){
		AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
	}		
	
}

BOOL CNegativeBinRange::CheckDialogForCompleteness()
{
    CMyUtils myUtilsObject;

    if (!UpdateData(TRUE)){
		AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
	}

	if (myUtilsObject.IsEmpty (m_StrLowFld, this))
    {
		m_StrMessageLbl = "Please enter BIN Low";	
		m_CntlLowFld.SetFocus();
		if (!UpdateData(FALSE)){
			AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
		}
		return FALSE;
	}

	if (myUtilsObject.IsEmpty (m_StrHighFld, this))
    {
		m_StrMessageLbl = "Please enter BIN High";	
		m_CntlHighFld.SetFocus();
		if (!UpdateData(FALSE)){
			AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
		}
		return FALSE;
	}	
	
    return TRUE;
}

BOOL CNegativeBinRange::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	// TODO: Add your specialized code here and/or call the base class
	c_time = CTime::GetCurrentTime();	
	shrmem.SetLastUpdateTime(c_time);
	return CDialog::OnCommand(wParam, lParam);
}
