/*********************************************************************************
 Module: MerchantDefinition.cpp : implementation file

 Copyright (c) 2002, Hypercom, Inc.                         
 All Rights Reserved.                                       

 Description: Logic for Merchant Main property page
			  This module defines and uses MerchantDefinition class

 Application: ATP

 Author: Irina Arustamova

 Revision History:
  
 $Log:   N:\PVCS\PTE\Equitable\ATP_gui\MerchantDefinition.cpp  $
   
      Rev 1.16   Dec 01 2003 11:29:36   lmitchel
   Added AMEX Online Updates - scr1200
   
      Rev 1.15   Jul 10 2002 09:56:22   lmitchel
   Ver:4.0.3.1:  Added SendMessage command in PropertySheet files (on_init function) to set the Apply button as the default.  If the enter key is pressed, action will default to  the apply button instead of the OK button.  This allows the user to remain in the current dialog instead of exiting to the main menu.
   
      Rev 1.14   May 21 2002 13:36:58   dirby
   Modified to not exit when <enter> is pressed.  If OK is selected,
   the database gets updated still, but screen is not exited.
   SCR 587
   
      Rev 1.13   Feb 10 2002 00:34:42   dirby
   Modified PopulateHistoryTable function so the unique_id field
   does not give a constraint violation.  It was frequently giving a
   unique constraint violation when updating the general and
   blocking tabs because it could do more than 1 within the same
   millisecond.  Also, if the data server returned with an error,
   there was an UpdateData(FALSE) command that would crash
   the program because it was trying to update a window that did
   not exist.  I replaced the command with an informational box.
   
      Rev 1.12   Mar 21 2000 12:27:52   iarustam
   scr 507
   
      Rev 1.11   Mar 16 2000 10:38:56   iarustam
   Added alpha lookup screens and updated
   Merchant main, Card Record main and 
   primary account main screens
   
      Rev 1.10   Jan 05 2000 14:53:54   gbarnes
   Major update. Not updated to PVCS recently.
   PVCS was locked.
   
      Rev 1.4   Oct 15 1999 17:13:36   iarustam
   bug fixes
   
      Rev 1.3   Sep 30 1999 16:17:24   npogosya
   Bug fixes
   
      Rev 1.1   Sep 22 1999 15:52:50   iarustam
   Added manual batch entry GUI
   
      Rev 1.0   Jul 01 1999 14:17:44   iarustam
   Initial Release
   
 
    

************************************************************************************/
// MerchantDefinition.cpp : implementation file
//

#include "stdafx.h"
#include "Atp.h"
#include "MerchantDefinition.h"
#include "BinList.h"

#include "MyUtils.h"
#include "AlphaLookup.h"
#include "SharedMem.h"	
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
extern CString strMerchantID;
extern CString strOrganizationID;
extern CSharedMem shrmem;
INT    merch_ctr = 0;


/////////////////////////////////////////////////////////////////////////////
// CMerchantDefinition property page

IMPLEMENT_DYNCREATE(CMerchantDefinition, CPropertyPage)

CMerchantDefinition::CMerchantDefinition() : CPropertyPage(CMerchantDefinition::IDD)
{
	//{{AFX_DATA_INIT(CMerchantDefinition)
	m_StrMerchantIDFld = _T("");
	m_StrOrganizationIDFld = _T("");
	m_StrMessageLbl = _T("");
	//}}AFX_DATA_INIT
}

CMerchantDefinition::~CMerchantDefinition()
{
}

void CMerchantDefinition::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMerchantDefinition)
	DDX_Control(pDX, IDC_SEARCH, m_SearchButton);
	DDX_Control(pDX, IDC_VIEW_BUTTON, m_ViewButton);
	DDX_Control(pDX, IDC_UPDATE_BUTTON, m_UpdateButton);
	DDX_Control(pDX, IDC_LIST_BUTTON, m_ListButton);
	DDX_Control(pDX, IDC_DELETE_BUTTON, m_DeleteButton);
	DDX_Control(pDX, IDC_ADD_BUTTON, m_AddButton);
	DDX_Control(pDX, IDC_ORGANIZATION_ID_FLD, m_CntlOrganizationIDFld);
	DDX_Control(pDX, IDC_MERCHANT_ID_FLD, m_CntlMerchantIDFld);
	DDX_Text(pDX, IDC_MERCHANT_ID_FLD, m_StrMerchantIDFld);
	DDV_MaxChars(pDX, m_StrMerchantIDFld, 15);
	DDX_Text(pDX, IDC_ORGANIZATION_ID_FLD, m_StrOrganizationIDFld);
	DDV_MaxChars(pDX, m_StrOrganizationIDFld, 4);
	DDX_Text(pDX, IDC_MESSAGE_LBL, m_StrMessageLbl);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CMerchantDefinition, CPropertyPage)
	//{{AFX_MSG_MAP(CMerchantDefinition)
	ON_BN_CLICKED(IDC_ADD_BUTTON, OnAddButton)
	ON_BN_CLICKED(IDC_DELETE_BUTTON, OnDeleteButton)
	ON_BN_CLICKED(IDC_LIST_BUTTON, OnListButton)
	ON_BN_CLICKED(IDC_UPDATE_BUTTON, OnUpdateButton)
	ON_BN_CLICKED(IDC_VIEW_BUTTON, OnViewButton)
	ON_BN_CLICKED(IDC_SEARCH, OnSearch)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMerchantDefinition message handlers

/******************************************************************************
 *
 * MODULE:	    OnInitDialog
 *
 * DESCRIPTION: This function performs special processing when 
 *              the dialog box is initialized.
 * AUTHOR:      Irina Arustamova
 *
 * INPUTS:      ucf01 - user structure             
 *
 * OUTPUTS:     None
 *
 * RTRN VALUE:  return TRUE unless you set the focus to a control
 *
 * MODIFICATIONS:
 *
 ******************************************************************************/
BOOL CMerchantDefinition::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
	CFont m_font;
	LOGFONT lf;
	CString temp;
	int nIndex = -1;

	// set font for the Message lable field

	memset ( &lf, 0, sizeof (lf));
	lf.lfHeight = 12;
	lf.lfWeight = 700;
	strcpy(lf.lfFaceName, "Arial");
	m_font.CreateFontIndirect (&lf);
	GetDlgItem(IDC_MESSAGE_LBL)->SetFont (&m_font);

	m_StrMerchantIDFld = _T("");
	m_StrOrganizationIDFld = _T("");
	m_StrMessageLbl = _T("");

	// set access rights according UCF01 table
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
		if ( ucf01.control[nIndex].action[2] == '0' )
			m_UpdateButton.EnableWindow(FALSE);
		if ( ucf01.control[nIndex].action[3] == '0' )
			m_ViewButton.EnableWindow(FALSE);
	}


	UpdateData(FALSE);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
	
}   /* function OnInitDialog */

/******************************************************************************
 *
 * MODULE:	    OnAddButton
 *
 * DESCRIPTION: This function sets all dialog modes to MODE_ADD, cleans all 
 *              screens, checkes if Merchant id already exists.
 *
 * AUTHOR:      Irina Arustamova
 *
 * INPUTS:      None             
 *
 * OUTPUTS:     None
 *
 * RTRN VALUE:  None
 *
 * MODIFICATIONS:
 *
 ******************************************************************************/
void CMerchantDefinition::OnAddButton() 
{
	MCF01 sMerchantRecord;
	char strDate[9] = "";
    
	char strErrorMessage[200];
    
    CMyUtils myUtilsObject;

	m_StrMessageLbl = "";

	m_nDialogMode = MODE_ADD;
	
	 if (!CheckDialogForCompleteness())
        return;

	// set m_nDialogMode for all pages to MODE_ADD
	 
	pLocationPage->m_nDialogMode = MODE_ADD;  
	pBlockingPage->m_nDialogMode = MODE_ADD;
   pChangeHistoryPage->m_nDialogMode = MODE_ADD; 
	pCapabilitiesPage->m_nDialogMode = MODE_ADD;
	pGeneralPage->m_nDialogMode = MODE_ADD;
	pContactPage->m_nDialogMode = MODE_ADD;
	pVelocityPage->m_nDialogMode = MODE_ADD;
	pSettlementPage->m_nDialogMode = MODE_ADD;

	// check if Merchant ID already exists

	ZeroMemory(&sMerchantRecord,sizeof(MCF01)) ;

    memcpy ((LPSTR)(sMerchantRecord.primary_key.merchant_id), m_StrMerchantIDFld , sizeof (sMerchantRecord.primary_key.merchant_id));
    memcpy ((LPSTR)(sMerchantRecord.primary_key.organization_id), m_StrOrganizationIDFld , sizeof (sMerchantRecord.primary_key.organization_id));  

	int rcode = txdsapi_get_record( (pBYTE)&sMerchantRecord, sizeof (MCF01), MCF01_DATA, strErrorMessage );
	if( PTEMSG_OK == rcode )
    {
      m_StrMessageLbl = "ERROR: Merchant ID already exists";
      m_nDialogMode = MODE_DONE;
	  m_CntlMerchantIDFld.SetFocus();

		if (!UpdateData(FALSE)){
			AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
		}
		return ;
    }
	
	// clean and enable all pages
	if ( pLocationPage->m_bLocation )
	{
		pLocationPage->UpdateData(FALSE);
		pLocationPage->CleanLocationDialog();
		pLocationPage->EnableLocationDialog( TRUE );
	}
	if ( pBlockingPage->m_bBlocking )
	{
		pBlockingPage->CleanBlockingDialog();
		pBlockingPage->UpdateData(FALSE);
		pBlockingPage->EnableBlockingDialog( TRUE );
	}
	if ( pCapabilitiesPage->m_bCapabilities )
	{
		pCapabilitiesPage->CleanCapabilitiesDialog();
		pCapabilitiesPage->UpdateData(FALSE);
		pCapabilitiesPage->EnableCapabilitiesDialog( TRUE );
	}
	if ( pContactPage->m_bContact)
	{
		pContactPage->CleanContactDialog();
		pContactPage->UpdateData(FALSE);
		pContactPage->EnableContactDialog( TRUE );
	}
	if ( pGeneralPage->m_bGeneral)
	{
		pGeneralPage->CleanGeneralDialog();
		pGeneralPage->UpdateData(FALSE);
	}
	if ( pVelocityPage->m_bVelocity )
	{
		pVelocityPage->CleanVelocityDialog();
		pVelocityPage->UpdateData(FALSE);
		pVelocityPage->EnableVelocityDialog( TRUE );
	}
	if ( pSettlementPage->m_bSettlement )
	{
		pSettlementPage->CleanSettlementDialog();
		pSettlementPage->UpdateData(FALSE);
		pSettlementPage->EnableSettlementDialog( TRUE );
	}

   if ( pChangeHistoryPage->m_bChangeHistory )
   {
		pChangeHistoryPage->CleanChangeHistory();
   }


	// set Merchant ID and  Organization ID for all pages
	pLocationPage->m_StrMerchantIDFld = m_StrMerchantIDFld;
   pLocationPage->m_StrOrganizationIDFld = m_StrOrganizationIDFld;
	myUtilsObject.GetSystemDate(strDate);	
	pLocationPage->m_StrDateAddedFld = strDate;
	pLocationPage->m_StrDateAddedFld =	  pLocationPage->m_StrDateAddedFld.Mid(2,2) + "/" 
										+ pLocationPage->m_StrDateAddedFld.Left(2) + "/" 
										+ pLocationPage->m_StrDateAddedFld.Right(4);

	pBlockingPage->m_StrMerchantIDFld = m_StrMerchantIDFld;
   pBlockingPage->m_StrOrganizationIDFld = m_StrOrganizationIDFld;

	pCapabilitiesPage->m_StrMerchantIDFld = m_StrMerchantIDFld;
   pCapabilitiesPage->m_StrOrganizationIDFld = m_StrOrganizationIDFld;

	pContactPage->m_StrMerchantIDFld = m_StrMerchantIDFld;
   pContactPage->m_StrOrganizationIDFld = m_StrOrganizationIDFld;

	pVelocityPage->m_StrMerchantIDFld = m_StrMerchantIDFld;
   pVelocityPage->m_StrOrganizationIDFld = m_StrOrganizationIDFld;

	pGeneralPage->m_StrMerchantIDFld = m_StrMerchantIDFld;
   pGeneralPage->m_StrOrganizationIDFld = m_StrOrganizationIDFld;

	pSettlementPage->m_StrMerchantIDFld = m_StrMerchantIDFld;
   pSettlementPage->m_StrOrganizationIDFld = m_StrOrganizationIDFld;

	pChangeHistoryPage->bNext = FALSE;
	((CPropertySheet*) this->GetParent())->SetWindowText("Add Merchant");
	//set version info in the title bar
	CString appProdVer;
	CString lpszTitle;
	appProdVer.LoadString(IDS_PRODUCT_VERSION);
	((CPropertySheet*) this->GetParent())->GetWindowText(lpszTitle);
	lpszTitle = lpszTitle + " - Version - ";
	lpszTitle += appProdVer;
	((CPropertySheet*) this->GetParent())->SetWindowText(lpszTitle);

	m_CntlMerchantIDFld.EnableWindow(FALSE);
	m_CntlOrganizationIDFld.EnableWindow(FALSE);

	// go to first page
	((CPropertySheet*) this->GetParent())->SetActivePage(1);
	SetModified(FALSE);
//	pWorkstationPage->UpdateData(FALSE);
	
}  /* function OnAddButton*/

/******************************************************************************
 *
 * MODULE:	    PopulateHistoryTable
 *
 * DESCRIPTION: This function populates history table if user updated some fields.
 *
 * AUTHOR:      Irina Arustamova
 *
 * INPUTS:      None             
 *
 * OUTPUTS:     None
 *
 * RTRN VALUE:  None
 *
 * MODIFICATIONS:
 *
 ******************************************************************************/
void CMerchantDefinition::PopulateHistoryTable()
{
    HST01 sHistoryData;
    CHAR dest_str[18];   
	CMyUtils myUtilsObject;
	char strErrorMsg[200] = "";
    char strDate[9] = "";
    char strTime[7] = "";
	char strPrimaryKey[50];
   CString temp;

    memset ( strDate, 0, sizeof (strDate));
    memset ( strTime, 0, sizeof (strTime));
	memset ( strPrimaryKey, 0, sizeof (strPrimaryKey));

	strcpy ( strPrimaryKey, m_StrMerchantIDFld);
	strcat ( strPrimaryKey, m_StrOrganizationIDFld );
    
    myUtilsObject.GetSystemDate(strDate);
    myUtilsObject.GetSystemTime(strTime);

	//  generate a timestamp for the current Greenwich Mean Time
	// in format YYYYMMDDHHIISSJJJ .

    pintime_get_gmt ( dest_str );

    ZeroMemory ( &sHistoryData, sizeof (HST01));
    CHAR wstation[21]; DWORD wsize;

    wsize = sizeof( wstation );
    GetComputerName( wstation, &wsize );

	memcpy ((LPTSTR)sHistoryData.reserved,(LPTSTR)wstation, sizeof(sHistoryData.reserved));
    memcpy ((LPTSTR) sHistoryData.file_name,     (LPCTSTR)"MCF01", sizeof (sHistoryData.file_name));
    memcpy ((LPTSTR) sHistoryData.file_primary_key,  (LPCTSTR)strPrimaryKey, sizeof (sHistoryData.file_primary_key));
    memcpy ((LPTSTR) sHistoryData.primary_key.unique_id,     (LPCTSTR)dest_str, sizeof (sHistoryData.primary_key.unique_id));        
    memcpy ((LPTSTR) sHistoryData.time_modified,     (LPCTSTR)strTime, sizeof (sHistoryData.time_modified)); 
    memcpy ((LPTSTR)sHistoryData.current_data, (LPCTSTR)m_strCurrentData, sizeof (sHistoryData.current_data));
	 memcpy ((LPTSTR)sHistoryData.changed_by,(LPTSTR)ucf01.primary_key.name, sizeof(sHistoryData.changed_by));
    temp = strDate; 
    temp = temp.Right(4) + temp.Left(2) + temp.Mid(2,2);
    
    memcpy ((LPTSTR) sHistoryData.date_modified, temp, sizeof (sHistoryData.date_modified));

    int rcode = txdsapi_insert_record  ( (pBYTE)&sHistoryData, sizeof (HST01), HST01_DATA, strErrorMsg );
	if ( PTEMSG_OK != rcode   )
	{
		m_StrMessageLbl = strErrorMsg;
		m_nDialogMode = MODE_DONE;
		UpdateData( FALSE );
	}

	/* function PopulateHistoryTable */
    
}

/******************************************************************************
 *
 * MODULE:	    CheckDialogForCompleteness
 *
 * DESCRIPTION: This function checks if all required fields was populated.
 *
 * INPUTS:      None             
 *
 * OUTPUTS:     None
 *
 * RTRN VALUE:  TRUE if everything is OK
				    FALSE if field is empty
 * AUTHOR:      Irina Arustamova
 *
 *
 * MODIFICATIONS:
 *
 ******************************************************************************/
BOOL CMerchantDefinition::CheckDialogForCompleteness()
{
    CMyUtils myUtilsObject;


    if (!UpdateData(TRUE)){
		AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
	}

    if (myUtilsObject.IsEmpty (m_StrMerchantIDFld, this))
    {
		m_StrMessageLbl = "Please enter Merchant ID";	
		m_CntlMerchantIDFld.SetFocus();
		if (!UpdateData(FALSE)){
			AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
		}
		m_nDialogMode = MODE_DONE;
		return FALSE;
	}

	if (myUtilsObject.IsEmpty (m_StrOrganizationIDFld, this))
    {
		m_StrMessageLbl = "Please enter Organization ID";	
		m_CntlOrganizationIDFld.SetFocus();
		if (!UpdateData(FALSE)){
			AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
		}
		m_nDialogMode = MODE_DONE;
		return FALSE;
	}

    return TRUE;
}   /* function CheckDialogForCompleteness */



/******************************************************************************
 *
 * MODULE:	    PopulateMerchantDatabaseStructureFromDialog
 *
 * DESCRIPTION: This function populates MCF01 structure.
 *
 * AUTHOR:      Irina Arustamova
 *
 * INPUTS:      pointer to MCF01 function             
 *
 * OUTPUTS:     None
 *
 * RTRN VALUE:  None
 *
 * MODIFICATIONS:
 *
 ******************************************************************************/
BOOL CMerchantDefinition::PopulateMerchantDatabaseStructureFromDialog ( pMCF01 psMerchantRecord, pMCT01 psMerchantCardRecord )
{
	ZeroMemory (psMerchantRecord, sizeof(MCF01)) ;
	
	// save all previous data
	if ( m_nDialogMode == MODE_EDIT )
		memcpy ( psMerchantRecord, &sMerchantRecordOld, sizeof (MCF01));
    
	memcpy ((LPTSTR) psMerchantRecord->primary_key.merchant_id, m_StrMerchantIDFld, sizeof (psMerchantRecord->primary_key.merchant_id));
	memcpy ((LPTSTR) psMerchantRecord->primary_key.organization_id,m_StrOrganizationIDFld, sizeof (psMerchantRecord->primary_key.organization_id));

	if (!pLocationPage->PopulateDatabaseFromLocationDialog ( psMerchantRecord ))
		return FALSE;
	if (!pBlockingPage->PopulateDatabaseFromBlockingDialog ( psMerchantRecord ))
		return FALSE;
	if (!pCapabilitiesPage->PopulateDatabaseFromCapabilitiesDialog ( psMerchantRecord ))
		return FALSE;
	if (!pContactPage->PopulateDatabaseFromContactDialog ( psMerchantRecord ))
		return FALSE;
	if (!pGeneralPage->PopulateDatabaseFromGeneralDialog ( psMerchantRecord ))
		return FALSE;
	if (!pVelocityPage->PopulateDatabaseFromVelocityDialog ( psMerchantRecord ))
		return FALSE;
	if ( m_nDialogMode == MODE_ADD && pSettlementPage->m_bSettlement)
	{
		if (!pSettlementPage->PopulateDatabaseFromSettlementDialog ())
		return FALSE;
	}

	return TRUE;
}  /* function PopulateMerchantDatabaseStructureFromDialog */

/******************************************************************************
 *
 * MODULE:	    OnDeleteButton
 *
 * DESCRIPTION: This function cleans dialog by cleaning all the strings.
 *
 * AUTHOR:      Irina Arustamova
 *
 * INPUTS:      None             
 *
 * OUTPUTS:     None
 *
 * RTRN VALUE:  None
 *
 * MODIFICATIONS:
 *
 ******************************************************************************/
void CMerchantDefinition::OnDeleteButton() 
{
	CString strMessage;
    int nReturnCode;
    MCF01 sMerchantRecord;
	char strErrorMessage[200];
    
     m_nDialogMode = MODE_EDIT;
	 m_StrMessageLbl = "";

    if (!CheckDialogForCompleteness())
        return; 

    if (!UpdateData(TRUE)){
		AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
	}
    
	// confirm the action
	strMessage.Format ("Are you sure you want to delete Merchant ID %s,\n Organization ID %s?", (LPCTSTR) m_StrMerchantIDFld,(LPCTSTR) m_StrOrganizationIDFld) ;

	nReturnCode = AfxMessageBox ((LPCTSTR)strMessage, MB_OKCANCEL|MB_ICONQUESTION) ;
	if (nReturnCode != IDOK) 
		return ;

	// try to delete
	memcpy ((LPTSTR)sMerchantRecord.primary_key.merchant_id,	m_StrMerchantIDFld,		sizeof (sMerchantRecord.primary_key.merchant_id)) ;
	memcpy ((LPTSTR)sMerchantRecord.primary_key.organization_id,m_StrOrganizationIDFld, sizeof (sMerchantRecord.primary_key.organization_id)) ;

    int rcode = txdsapi_delete_record ((pBYTE)&sMerchantRecord, sizeof ( MCF01), MCF01_DATA, strErrorMessage) ;
	// failed to delete
	if ( PTEMSG_OK != rcode   )
    {
		 switch( rcode )
      {
         case PTEMSG_NOT_FOUND:
			sprintf( strErrorMessage, "ERROR: Merchant record for %s does not exist", m_StrMerchantIDFld );
            break;
      }

        m_StrMessageLbl = strErrorMessage;
		m_nDialogMode = MODE_DONE;
		if (!UpdateData(FALSE)){
			AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
		}
		return ;
	}

    else
    {
        memcpy ((LPSTR)m_strCurrentData, "Record Deleted", sizeof (m_strCurrentData));

        PopulateHistoryTable();

		m_StrMerchantIDFld = _T("");	
		m_StrOrganizationIDFld = _T("");
        m_StrMessageLbl = "Record Deleted";

		if (!UpdateData(FALSE)){
			AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
		}

		// clean and disable all pages
		if ( pLocationPage->m_bLocation )
		{
			pLocationPage->CleanLocationDialog();
			pLocationPage->EnableLocationDialog(FALSE);
		}
		if ( pBlockingPage->m_bBlocking )
		{
			pBlockingPage->CleanBlockingDialog();
			pBlockingPage->EnableBlockingDialog( FALSE );
		}
		if ( pCapabilitiesPage->m_bCapabilities )
		{
			pCapabilitiesPage->CleanCapabilitiesDialog();
			pCapabilitiesPage->EnableCapabilitiesDialog( FALSE );
		}
		if ( pContactPage->m_bContact)
		{
			pContactPage->CleanContactDialog();
			pContactPage->EnableContactDialog( FALSE );
		}
		if ( pGeneralPage->m_bGeneral)
		{
			pGeneralPage->CleanGeneralDialog();
		}
		if ( pVelocityPage->m_bVelocity )
		{
			pVelocityPage->CleanVelocityDialog();
			pVelocityPage->EnableVelocityDialog( FALSE );
		}

		if ( pSettlementPage->m_bSettlement )
		{
			pSettlementPage->CleanSettlementDialog();
			pSettlementPage->EnableSettlementDialog( TRUE );
		}
		if ( pChangeHistoryPage->m_bChangeHistory )
			pChangeHistoryPage->CleanChangeHistory();

		m_nDialogMode = MODE_DONE;

		((CPropertySheet*) this->GetParent())->SetActivePage(0);
		SetModified ( FALSE );
    }
		
	
		m_CntlMerchantIDFld.EnableWindow(TRUE);
		m_CntlOrganizationIDFld.EnableWindow(TRUE);
	
}     /* function OnDeleteButton */

/******************************************************************************
 *
 * MODULE:	    OnListButton
 *
 * DESCRIPTION: This function calles BinList dialog window and displays list 
 *               of Merchant IDs and Organization IDs.
 *
 * AUTHOR:      Irina Arustamova
 *
 * INPUTS:      None             
 *
 * OUTPUTS:     None
 *
 * RTRN VALUE:  None
 *
 * MODIFICATIONS:
 *
 ******************************************************************************/
void CMerchantDefinition::OnListButton() 
{
	CBinList listDlg;
    int nReturnCode;

	UpdateData(TRUE);
	m_StrMessageLbl = "";
	UpdateData(FALSE);

	// set dialog Mode for Bin List
	listDlg.strItem0 = m_StrMerchantIDFld;
	listDlg.strItem1 = m_StrOrganizationIDFld;
	listDlg.m_nDialogMode = MODE_MERCHANT;

    nReturnCode = listDlg.DoModal();
    if ( nReturnCode == IDOK)
	{
        m_StrMerchantIDFld = strMerchantID;
		m_StrOrganizationIDFld = strOrganizationID;

		if ( pLocationPage->m_bLocation )
		{
			pLocationPage->CleanLocationDialog();
			pLocationPage->EnableLocationDialog(FALSE);
		}
		if ( pBlockingPage->m_bBlocking )
		{
			pBlockingPage->CleanBlockingDialog();
			pBlockingPage->EnableBlockingDialog( FALSE );
		}
		if ( pCapabilitiesPage->m_bCapabilities )
		{
			pCapabilitiesPage->CleanCapabilitiesDialog();
			pCapabilitiesPage->EnableCapabilitiesDialog( FALSE );
		}
		if ( pContactPage->m_bContact)
		{
			pContactPage->CleanContactDialog();
			pContactPage->EnableContactDialog( FALSE );
		}
		if ( pGeneralPage->m_bGeneral)
		{
			pGeneralPage->CleanGeneralDialog();
		}
		if ( pVelocityPage->m_bVelocity )
		{
			pVelocityPage->CleanVelocityDialog();
			pVelocityPage->EnableVelocityDialog( FALSE );
		}

		if ( pSettlementPage->m_bSettlement )
		{
			pSettlementPage->CleanSettlementDialog();
			pSettlementPage->EnableSettlementDialog( FALSE );
		}

		m_CntlMerchantIDFld.EnableWindow(TRUE);
		m_CntlOrganizationIDFld.EnableWindow(TRUE);
	

	}
        
    if (!UpdateData(FALSE)){
		AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
	}		
	
}   /* function OnListButton */

/******************************************************************************
 *
 * MODULE:	    OnUpdateButton
 *
 * DESCRIPTION: This function sets all pages to MODE_EDIT, cleans and enables pages, 
 *				gets Merchant record from DB.
 *
 * AUTHOR:      Irina Arustamova
 *
 * INPUTS:      None             
 *
 * OUTPUTS:     None
 *
 * RTRN VALUE:  None
 *
 * MODIFICATIONS:
 *
 ******************************************************************************/
void CMerchantDefinition::OnUpdateButton() 
{
    HST01 sHistoryRecord;
	char strErrorMessage[200];

   m_nDialogMode = MODE_EDIT;
   pLocationPage->m_nDialogMode = MODE_EDIT;  
   pBlockingPage->m_nDialogMode = MODE_EDIT;
   pChangeHistoryPage->m_nDialogMode = MODE_EDIT; 
	pCapabilitiesPage->m_nDialogMode = MODE_EDIT;
	pContactPage->m_nDialogMode = MODE_EDIT;
	pGeneralPage->m_nDialogMode = MODE_EDIT;
	pVelocityPage->m_nDialogMode = MODE_EDIT;  
	pSettlementPage->m_nDialogMode = MODE_EDIT;

	m_StrMessageLbl = "";

   if (!CheckDialogForCompleteness())
      return;

	// get data from DB
   memcpy ((LPSTR)(sMerchantRecordOld.primary_key.merchant_id), m_StrMerchantIDFld, sizeof (sMerchantRecordOld.primary_key.merchant_id) );
   memcpy ((LPSTR)(sMerchantRecordOld.primary_key.organization_id), m_StrOrganizationIDFld, sizeof (sMerchantRecordOld.primary_key.organization_id) );   

	int rcode = txdsapi_get_record( (pBYTE)&sMerchantRecordOld, sizeof (MCF01), MCF01_DATA, strErrorMessage );
	// failed to get the data
	if ( PTEMSG_OK != rcode )
	{       
       	 switch( rcode )
         {
         case PTEMSG_NOT_FOUND:
			 sprintf( strErrorMessage, "ERROR: Merchant record for %s does not exist", m_StrMerchantIDFld );
            break;
         }

       // all other cases
		m_StrMessageLbl = strErrorMessage;
		m_nDialogMode = MODE_DONE;
		if (!UpdateData(FALSE)){
			AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
		}

		return;
    }

	// clean and enable all pages before populating them
	if ( pLocationPage->m_bLocation )
	    {
			pLocationPage->CleanLocationDialog();
			pLocationPage->EnableLocationDialog(FALSE);
		}
		if ( pBlockingPage->m_bBlocking )
		{
			pBlockingPage->CleanBlockingDialog();
			pBlockingPage->EnableBlockingDialog( FALSE );
		}
		if ( pCapabilitiesPage->m_bCapabilities )
		{
			pCapabilitiesPage->CleanCapabilitiesDialog();
			pCapabilitiesPage->EnableCapabilitiesDialog( FALSE );
		}
		if ( pContactPage->m_bContact)
		{
			pContactPage->CleanContactDialog();
			pContactPage->EnableContactDialog( FALSE );
		}
		if ( pGeneralPage->m_bGeneral)
		{
			pGeneralPage->CleanGeneralDialog();
		}
		if ( pVelocityPage->m_bVelocity )
		{
			pVelocityPage->CleanVelocityDialog();
			pVelocityPage->EnableVelocityDialog( FALSE );
		}
		if ( pSettlementPage->m_bSettlement )
		{
			pSettlementPage->CleanSettlementDialog();
			pSettlementPage->EnableSettlementDialog( FALSE );
		}

	// get history data
    ZeroMemory (&sHistoryRecord, sizeof (sHistoryRecord));

	memset ( strFilePrimaryKey, 0, sizeof (strFilePrimaryKey));
	strcpy ( strFilePrimaryKey, (char *)sMerchantRecordOld.primary_key.merchant_id);
	strcat ( strFilePrimaryKey, (char *)sMerchantRecordOld.primary_key.organization_id);

    memcpy ((LPSTR)(sHistoryRecord.primary_key.unique_id), "                 ", sizeof (sHistoryRecord.primary_key.unique_id));
    memcpy ((LPSTR)(sHistoryRecord.file_name), "MCF01", sizeof (sHistoryRecord.file_name));
    memcpy ((LPSTR)(sHistoryRecord.file_primary_key), strFilePrimaryKey, sizeof (sHistoryRecord.file_primary_key));

    memcpy ( &(pChangeHistoryPage->sHistoryRecord), &sHistoryRecord, sizeof(sHistoryRecord));
    
	((CPropertySheet*) this->GetParent())->SetActivePage(1);    

	// set dialog mode for all pages

	pChangeHistoryPage->bNext = FALSE;

	((CPropertySheet*) this->GetParent())->SetWindowText("Update Merchant");
	//set version info in the title bar
	CString appProdVer;
	CString lpszTitle;
	appProdVer.LoadString(IDS_PRODUCT_VERSION);
	((CPropertySheet*) this->GetParent())->GetWindowText(lpszTitle);
	lpszTitle = lpszTitle + " - Version - ";
	lpszTitle += appProdVer;
	((CPropertySheet*) this->GetParent())->SetWindowText(lpszTitle);

	if ( pChangeHistoryPage->m_bChangeHistory )
		pChangeHistoryPage->PopulateHistoryDialog();

	PopulateMerchantDialogFromDatabase ( sMerchantRecordOld );

	pLocationPage->SetModified ( FALSE );	

	m_CntlMerchantIDFld.EnableWindow(FALSE);
	m_CntlOrganizationIDFld.EnableWindow(FALSE);

	
}  /* function OnUpdateButton */

/******************************************************************************
 *
 * MODULE:	    OnViewButton
 *
 * DESCRIPTION: This function sets all pages to MODE_VIEW, cleans and enables pages, 
 *				gets Merchant record from DB.
 *
 * AUTHOR:      Irina Arustamova
 *
 * INPUTS:      None             
 *
 * OUTPUTS:     None
 *
 * RTRN VALUE:  None
 *
 * MODIFICATIONS:
 *
 ******************************************************************************/
void CMerchantDefinition::OnViewButton() 
{
     HST01 sHistoryRecord;
	char strErrorMessage[200];

   m_nDialogMode = MODE_VIEW;
   pLocationPage->m_nDialogMode = MODE_VIEW;  
   pBlockingPage->m_nDialogMode = MODE_VIEW;
   pChangeHistoryPage->m_nDialogMode = MODE_VIEW; 
   pCapabilitiesPage->m_nDialogMode = MODE_VIEW;
   pContactPage->m_nDialogMode = MODE_VIEW;
	pGeneralPage->m_nDialogMode = MODE_VIEW;
	pVelocityPage->m_nDialogMode = MODE_VIEW;  
	pSettlementPage->m_nDialogMode = MODE_VIEW;

	m_StrMessageLbl = "";

   if (!CheckDialogForCompleteness())
      return;

	// get data from DB
   memcpy ((LPSTR)(sMerchantRecordOld.primary_key.merchant_id), m_StrMerchantIDFld, sizeof (sMerchantRecordOld.primary_key.merchant_id) );
   memcpy ((LPSTR)(sMerchantRecordOld.primary_key.organization_id), m_StrOrganizationIDFld, sizeof (sMerchantRecordOld.primary_key.organization_id) );   

	int rcode = txdsapi_get_record( (pBYTE)&sMerchantRecordOld, sizeof (MCF01), MCF01_DATA, strErrorMessage );
	// failed to get the data
	if ( PTEMSG_OK != rcode )
	{       
       	 switch( rcode )
         {
         case PTEMSG_NOT_FOUND:
			 sprintf( strErrorMessage, "ERROR: Merchant record for %s does not exist", m_StrMerchantIDFld );
            break;
         }

       // all other cases
		m_StrMessageLbl = strErrorMessage;
		m_nDialogMode = MODE_DONE;
		if (!UpdateData(FALSE)){
			AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
		}

		return;
    }

	// clean and enable all pages before populating them
	if ( pLocationPage->m_bLocation )
	    {
			pLocationPage->CleanLocationDialog();
			pLocationPage->EnableLocationDialog(FALSE);
		}
		if ( pBlockingPage->m_bBlocking )
		{
			pBlockingPage->CleanBlockingDialog();
			pBlockingPage->EnableBlockingDialog( FALSE );
		}
		if ( pCapabilitiesPage->m_bCapabilities )
		{
			pCapabilitiesPage->CleanCapabilitiesDialog();
			pCapabilitiesPage->EnableCapabilitiesDialog( FALSE );
		}
		if ( pContactPage->m_bContact)
		{
			pContactPage->CleanContactDialog();
			pContactPage->EnableContactDialog( FALSE );
		}
		if ( pGeneralPage->m_bGeneral)
		{
			pGeneralPage->CleanGeneralDialog();
		}
		if ( pVelocityPage->m_bVelocity )
		{
			pVelocityPage->CleanVelocityDialog();
			pVelocityPage->EnableVelocityDialog( FALSE );
		}
		if ( pSettlementPage->m_bSettlement )
		{
			pSettlementPage->CleanSettlementDialog();
			pSettlementPage->EnableSettlementDialog( FALSE );
		}

	// get history data
    ZeroMemory (&sHistoryRecord, sizeof (sHistoryRecord));

	memset ( strFilePrimaryKey, 0, sizeof (strFilePrimaryKey));
	strcpy ( strFilePrimaryKey, (char *)sMerchantRecordOld.primary_key.merchant_id);
	strcat ( strFilePrimaryKey, (char *)sMerchantRecordOld.primary_key.organization_id);

    memcpy ((LPSTR)(sHistoryRecord.primary_key.unique_id), "                 ", sizeof (sHistoryRecord.primary_key.unique_id));
    memcpy ((LPSTR)(sHistoryRecord.file_name), "MCF01", sizeof (sHistoryRecord.file_name));
    memcpy ((LPSTR)(sHistoryRecord.file_primary_key), strFilePrimaryKey, sizeof (sHistoryRecord.file_primary_key));

    memcpy ( &(pChangeHistoryPage->sHistoryRecord), &sHistoryRecord, sizeof(sHistoryRecord));
    
	((CPropertySheet*) this->GetParent())->SetActivePage(1);    

	// set dialog mode for all pages

	pChangeHistoryPage->bNext = FALSE;

	((CPropertySheet*) this->GetParent())->SetWindowText("View Merchant");
	//set version info in the title bar
	CString appProdVer;
	CString lpszTitle;
	appProdVer.LoadString(IDS_PRODUCT_VERSION);
	((CPropertySheet*) this->GetParent())->GetWindowText(lpszTitle);
	lpszTitle = lpszTitle + " - Version - ";
	lpszTitle += appProdVer;
	((CPropertySheet*) this->GetParent())->SetWindowText(lpszTitle);

	if ( pChangeHistoryPage->m_bChangeHistory )
		pChangeHistoryPage->PopulateHistoryDialog();

	PopulateMerchantDialogFromDatabase ( sMerchantRecordOld );

	pLocationPage->SetModified ( FALSE );	

	m_CntlMerchantIDFld.EnableWindow(TRUE);
	m_CntlOrganizationIDFld.EnableWindow(TRUE);

	
} /* function OnViewButton */

/******************************************************************************
 *
 * MODULE:	    PopulateMerchantDialogFromDatabase
 *
 * DESCRIPTION: This function populates MCF01 table for populating dialogs.
 *
 * AUTHOR:      Irina Arustamova
 *
 * INPUTS:      MCF01 structure             
 *
 * OUTPUTS:     None
 *
 * RTRN VALUE:  None
 *
 * MODIFICATIONS:
 *
 ******************************************************************************/

void CMerchantDefinition::PopulateMerchantDialogFromDatabase( MCF01 sMerchantRecord )
{
	// if page was open - populate fields on the page
	// if page was not open - populate MCF01 structure for that page and
	//                        it going to be populated in OnInitDialog for this page
	if ( !pLocationPage->m_bLocation )
		memcpy ( &pLocationPage->sMerchantRecordOld, &sMerchantRecord, sizeof ( MCF01));
	else
		pLocationPage->PopulateLocationDialogFromDatabase(sMerchantRecord);

	if ( !pBlockingPage->m_bBlocking )
		memcpy ( &pBlockingPage->sMerchantRecordOld, &sMerchantRecord, sizeof ( MCF01));
	else
		pBlockingPage->PopulateBlockingDialogFromDatabase(sMerchantRecord);

	if ( !pCapabilitiesPage->m_bCapabilities )
		memcpy ( &pCapabilitiesPage->sMerchantRecordOld, &sMerchantRecord, sizeof ( MCF01));
	else
		pCapabilitiesPage->PopulateCapabilitiesDialogFromDatabase(sMerchantRecord);

	if ( !pContactPage->m_bContact )
		memcpy ( &pContactPage->sMerchantRecordOld, &sMerchantRecord, sizeof ( MCF01));
	else
		pContactPage->PopulateContactDialogFromDatabase(sMerchantRecord);

	if ( !pGeneralPage->m_bGeneral )
		memcpy ( &pGeneralPage->sMerchantRecordOld, &sMerchantRecord, sizeof ( MCF01));
	else
		pGeneralPage->PopulateGeneralDialogFromDatabase(sMerchantRecord);

	if ( !pVelocityPage->m_bVelocity )
		memcpy ( &pVelocityPage->sMerchantRecordOld, &sMerchantRecord, sizeof ( MCF01));
	else
		pVelocityPage->PopulateVelocityDialogFromDatabase(sMerchantRecord);

	pSettlementPage->m_StrMerchantIDFld = m_StrMerchantIDFld;
    pSettlementPage->m_StrOrganizationIDFld = m_StrOrganizationIDFld;
	if ( pSettlementPage->m_bSettlement )
	{
		pSettlementPage->UpdateData(FALSE);
		memcpy(pSettlementPage->sMerchantCardRecord.primary_key.card_type, "    ", sizeof (pSettlementPage->sMerchantCardRecord.primary_key.card_type));
		pSettlementPage->PopulateSettlementList();
	}
}  /* function PopulateMerchantDialogFromDatabase  */

/******************************************************************************
 *
 * MODULE:      OnApply
 *
 * DESCRIPTION: This function checks for all required fields, saves the data,
 *              cleans pages.
 *
 * AUTHOR:      Irina Arustamova
 *
 * INPUTS:      m_nDialogMode             
 *
 * OUTPUTS:     None
 *
 * RTRN VALUE:  Always return FALSE to avoid exiting the window on OK
 *
 ******************************************************************************/
BOOL CMerchantDefinition::OnApply() 
{
	MCF01 sMerchantDatabaseStructure;
	MCT01 sMerchantCardDatabaseStructure;
//	NGF01 sNegGroupRecord;
	char strErrorMessage[200];

	CMyUtils myUtilsObject;
	CRF01 sCurrencyCode;

//   CString buttonlabel;
//   CWnd*   activebutton;

   /* This snippet of code prevents updating the database
    * and exiting the window when <enter> is pressed. The
    * database will only get updated when APPLY is selected
    * or if focus is on the OK button when <enter> is pressed
    * or OK is selected.
    */
/*   activebutton = GetFocus();
   activebutton->GetWindowText(buttonlabel);
   if ( buttonlabel != "&Apply"  && buttonlabel != "OK")
   {
      return FALSE;
   }
*/
	int len = pGeneralPage->m_StrVisaMVV.GetLength();
		char *str = pGeneralPage->m_StrVisaMVV.GetBuffer(len);
		for(int i=0;i<len;i++)
		{
			if(str[0] == ' ' )
            {
                ((CPropertySheet*) this->GetParent())->SetActivePage(3);
                pGeneralPage->m_CntlVisaMVV.SetFocus();
                pGeneralPage->m_StrMessageLbl = "Visa MVV should not start with space";
                pGeneralPage->UpdateData(FALSE);
                return FALSE;
            }
            if((str[i] > 64 && str[i] < 91) || (str[i] > 47 && str[i] < 58) || (str[i] > 96 && str[i] < 123) || str[i] == ' ')
            {
                //The character is a ALPHABHET or NUMBER
            }
            else
            {
                ((CPropertySheet*) this->GetParent())->SetActivePage(3);
                pGeneralPage->m_CntlVisaMVV.SetFocus();
                pGeneralPage->m_StrMessageLbl = "No Special characters allowed in Visa MVV";
                pGeneralPage->UpdateData(FALSE);
                return FALSE;
            }
		}

	// check for required fields
	if ( MODE_VIEW != m_nDialogMode && MODE_DONE != m_nDialogMode )
	{
	
       if ( myUtilsObject.IsEmpty ( pLocationPage->m_StrCurrencyCodeFld, this))
		{
			((CPropertySheet*) this->GetParent())->SetActivePage(1);
			pLocationPage->m_CntlCurrencyCodeFld.SetFocus();
			pLocationPage->m_StrMessageLbl = "Please enter Currency Code";
			pLocationPage->UpdateData(FALSE);
			return FALSE;
		}
	   	int len_Merchant_cnty_origin = pGeneralPage->m_StrMCMerchantcntryOrigin.GetLength();

		if(len_Merchant_cnty_origin >3)
		{
			AfxMessageBox( "Merchant Country of Origin Value must be 3 Numeric Characters", MB_ICONSTOP | MB_OK );
			return FALSE;	
		}


		memcpy ((LPSTR)(sCurrencyCode.primary_key.currency_code), pLocationPage->m_StrCurrencyCodeFld , sizeof (sCurrencyCode.primary_key.currency_code));

	/*	int rcode = txdsapi_get_record( (pBYTE)&sCurrencyCode, sizeof (CRF01), CRF01_DATA, strErrorMessage );
        if(rcode != PTEMSG_OK )    
		{
			((CPropertySheet*) this->GetParent())->SetActivePage(1);
			pLocationPage->m_CntlCurrencyCodeFld.SetFocus();
			pLocationPage->m_StrMessageLbl = "ERROR: Invalid Currency Code";
			pLocationPage->UpdateData(FALSE);       
			return FALSE;

		}*/

	/*	if ( !myUtilsObject.IsEmpty ( pLocationPage->m_StrNegativeGroupIDFld, this))
		{
			memcpy ((LPSTR)(sNegGroupRecord.primary_key.negative_group_id), pLocationPage->m_StrNegativeGroupIDFld , sizeof (sNegGroupRecord.primary_key.negative_group_id));

			int rcode = txdsapi_get_record( (pBYTE)&sNegGroupRecord,sizeof (NGF01), NGF01_DATA, strErrorMessage );
			if(rcode != PTEMSG_OK )    
			{
				((CPropertySheet*) this->GetParent())->SetActivePage(1);
				pLocationPage->m_CntlNegativeGroupIDFld.SetFocus();
				pLocationPage->m_StrMessageLbl = "ERROR: Invalid Negative Group ID";
				pLocationPage->UpdateData(FALSE);       
				return FALSE;

			}	
		}*/

		if ( pBlockingPage->m_bBlocking )
		{

			if ( !pBlockingPage->ValidateDates())
			{
				((CPropertySheet*) this->GetParent())->SetActivePage(6);
				return FALSE;

			}
	/*		if ( !pBlockingPage->m_bValidStartDay || !pBlockingPage->m_bValidEndDay)
			{
				((CPropertySheet*) this->GetParent())->SetActivePage(6);
				return FALSE;

			}

			if ( !pBlockingPage->m_bValidOrder)
			{
				((CPropertySheet*) this->GetParent())->SetActivePage(6);
			   pBlockingPage->m_StrMessageLbl = "ERROR: Blocking Start Day cannot be greater than End Day";
			 pBlockingPage->UpdateData(FALSE);
				return FALSE;

			}

			if ( myUtilsObject.IsEmpty ( pBlockingPage->m_StrBlockingStartFld, this) && !myUtilsObject.IsEmpty ( pBlockingPage->m_StrBlockingEndFld, this))
			{
				((CPropertySheet*) this->GetParent())->SetActivePage(6);
				pBlockingPage->m_CntlBlockingEndFld.SetFocus();
				pBlockingPage->m_StrMessageLbl = "Please enter Blocking End Day";
				pBlockingPage->UpdateData(FALSE);
				return FALSE;
			}

			if ( !myUtilsObject.IsEmpty ( pBlockingPage->m_StrBlockingStartFld, this) && myUtilsObject.IsEmpty ( pBlockingPage->m_StrBlockingEndFld, this))
			{
				((CPropertySheet*) this->GetParent())->SetActivePage(6);
				pBlockingPage->m_CntlBlockingStartFld.SetFocus();
				pBlockingPage->m_StrMessageLbl = "Please enter Blocking Start Day";
				pBlockingPage->UpdateData(FALSE);
				return FALSE;
			}*/
		}
	}
	
    // ****************************************************************************************
	if (m_nDialogMode == MODE_ADD)
	{			
		memset(&sMerchantDatabaseStructure, 0, sizeof(sMerchantDatabaseStructure)) ;
		if (!PopulateMerchantDatabaseStructureFromDialog (&sMerchantDatabaseStructure, &sMerchantCardDatabaseStructure)) {		
			return FALSE;
		}

		int rcode = txdsapi_insert_record ( (pBYTE)&sMerchantDatabaseStructure, sizeof (MCF01), MCF01_DATA, strErrorMessage );
		if ( rcode != PTEMSG_OK )
		{  	
			m_StrMessageLbl = strErrorMessage;
			m_nDialogMode = MODE_DONE;
			if (!UpdateData(FALSE)){
				AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
			}
			((CPropertySheet*) this->GetParent())->SetActivePage(0);
  			((CPropertySheet*) this->GetParent())->SetWindowText("Merchant Definition");
			//set version info in the title bar
			CString appProdVer;
			CString lpszTitle;
			appProdVer.LoadString(IDS_PRODUCT_VERSION);
			((CPropertySheet*) this->GetParent())->GetWindowText(lpszTitle);
			lpszTitle = lpszTitle + " - Version - ";
			lpszTitle += appProdVer;
			((CPropertySheet*) this->GetParent())->SetWindowText(lpszTitle);

			return FALSE;
		}
		else 
		{            
			memcpy ((LPSTR)m_strCurrentData, "Record Added", sizeof (m_strCurrentData));		
			PopulateHistoryTable();
			((CPropertySheet*) this->GetParent())->SetActivePage(0);
			m_StrMessageLbl = "Record Added";	
			if (!UpdateData(FALSE)){
				AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
			}
			pLocationPage->CleanLocationDialog();
			pLocationPage->EnableLocationDialog(FALSE);
			if ( pBlockingPage->m_bBlocking )
			{
			pBlockingPage->CleanBlockingDialog();
			pBlockingPage->EnableBlockingDialog( FALSE );
			}
			if ( pCapabilitiesPage->m_bCapabilities )
			{
			pCapabilitiesPage->CleanCapabilitiesDialog();
			pCapabilitiesPage->EnableCapabilitiesDialog( FALSE );
			}
			if ( pContactPage->m_bContact)
			{
			pContactPage->CleanContactDialog();
			pContactPage->EnableContactDialog( FALSE );
			}
			if ( pGeneralPage->m_bGeneral)
			{
			pGeneralPage->CleanGeneralDialog();
			}
			if ( pVelocityPage->m_bVelocity )
			{
			pVelocityPage->CleanVelocityDialog();
			pVelocityPage->EnableVelocityDialog( FALSE );
			}
			if ( pSettlementPage->m_bSettlement )
			{
			pSettlementPage->CleanSettlementDialog();
			pSettlementPage->EnableSettlementDialog( FALSE );
			}

			if ( pChangeHistoryPage->m_bChangeHistory )
				pChangeHistoryPage->CleanChangeHistory();

			SetModified ( FALSE );
			m_nDialogMode = MODE_DONE;
			pLocationPage->m_nDialogMode = MODE_DONE;  
			pBlockingPage->m_nDialogMode = MODE_DONE;
			pChangeHistoryPage->m_nDialogMode = MODE_DONE; 
			pCapabilitiesPage->m_nDialogMode = MODE_DONE;
			pContactPage->m_nDialogMode = MODE_DONE;
			pGeneralPage->m_nDialogMode = MODE_DONE;
			pVelocityPage->m_nDialogMode = MODE_DONE;
			pSettlementPage->m_nDialogMode = MODE_DONE;
			pSettlementPage->m_StrMerchantIDFld = "";
			pSettlementPage->m_StrOrganizationIDFld = "";



			m_CntlMerchantIDFld.SetFocus();
			m_CntlMerchantIDFld.EnableWindow(TRUE);
			m_CntlOrganizationIDFld.EnableWindow(TRUE);

			//return CPropertyPage::OnApply();
			return FALSE;
		} 
	}
//////////////////////////////////////////////////////////////////////////////////////////
	if (m_nDialogMode == MODE_EDIT)
	{ 
		MCF01 sMerchantDatabaseStructure;
        memset(&sMerchantDatabaseStructure, 0, sizeof(MCF01)) ;

		PopulateMerchantDatabaseStructureFromDialog (&sMerchantDatabaseStructure, &sMerchantCardDatabaseStructure );

		int rcode = txdsapi_update_record ( (pBYTE)&sMerchantDatabaseStructure, sizeof (MCF01), MCF01_DATA, strErrorMessage );
		if ( rcode != PTEMSG_OK )
		{ 		
			m_StrMessageLbl = strErrorMessage;
			m_nDialogMode = MODE_DONE;
			((CPropertySheet*) this->GetParent())->SetActivePage(0);
			((CPropertySheet*) this->GetParent())->SetWindowText("Merchant Definition");
			//set version info in the title bar
			CString appProdVer;
			CString lpszTitle;
			appProdVer.LoadString(IDS_PRODUCT_VERSION);
			((CPropertySheet*) this->GetParent())->GetWindowText(lpszTitle);
			lpszTitle = lpszTitle + " - Version - ";
			lpszTitle += appProdVer;
			((CPropertySheet*) this->GetParent())->SetWindowText(lpszTitle);

			if (!UpdateData(FALSE)){
				AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
			}
			return FALSE;
		}
		else
		{
			m_StrMessageLbl = "Record Updated";
			if (!UpdateData(FALSE)){
				AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
			}
			pLocationPage->CompareStructures();
			pBlockingPage->CompareStructures();
			pCapabilitiesPage->CompareStructures();
			pContactPage->CompareStructures();
			pGeneralPage->CompareStructures();
			pVelocityPage->CompareStructures();

			pLocationPage->CleanLocationDialog();
			pLocationPage->EnableLocationDialog(FALSE);
			if ( pBlockingPage->m_bBlocking )
			{
			pBlockingPage->CleanBlockingDialog();
			pBlockingPage->EnableBlockingDialog( FALSE );
			}
			if ( pCapabilitiesPage->m_bCapabilities )
			{
			pCapabilitiesPage->CleanCapabilitiesDialog();
			pCapabilitiesPage->EnableCapabilitiesDialog( FALSE );
			}
			if ( pContactPage->m_bContact)
			{
			pContactPage->CleanContactDialog();
			pContactPage->EnableContactDialog( FALSE );
			}
			if ( pGeneralPage->m_bGeneral)
			{
			pGeneralPage->CleanGeneralDialog();
			}
			if ( pVelocityPage->m_bVelocity )
			{
			pVelocityPage->CleanVelocityDialog();
			pVelocityPage->EnableVelocityDialog( FALSE );
			}
			if ( pSettlementPage->m_bSettlement )
			{
			pSettlementPage->CleanSettlementDialog();
			pSettlementPage->EnableSettlementDialog( FALSE );
			}
			
			if ( pChangeHistoryPage->m_bChangeHistory )
				pChangeHistoryPage->CleanChangeHistory();
			((CPropertySheet*) this->GetParent())->SetActivePage(0);
			((CPropertySheet*) this->GetParent())->SetWindowText("Merchant Definition");
			//set version info in the title bar
			CString appProdVer;
			CString lpszTitle;
			appProdVer.LoadString(IDS_PRODUCT_VERSION);
			((CPropertySheet*) this->GetParent())->GetWindowText(lpszTitle);
			lpszTitle = lpszTitle + " - Version - ";
			lpszTitle += appProdVer;
			((CPropertySheet*) this->GetParent())->SetWindowText(lpszTitle);

			m_CntlMerchantIDFld.EnableWindow(TRUE);
			m_CntlOrganizationIDFld.EnableWindow(TRUE);

			SetModified ( FALSE );

			m_nDialogMode = MODE_DONE;
			pLocationPage->m_nDialogMode = MODE_DONE;  
			pBlockingPage->m_nDialogMode = MODE_DONE;
			pChangeHistoryPage->m_nDialogMode = MODE_DONE; 
			pCapabilitiesPage->m_nDialogMode = MODE_DONE;
			pContactPage->m_nDialogMode = MODE_DONE;
			pGeneralPage->m_nDialogMode = MODE_DONE;
			pVelocityPage->m_nDialogMode = MODE_DONE;
			pSettlementPage->m_nDialogMode = MODE_DONE;
			pSettlementPage->m_StrMerchantIDFld = "";
			pSettlementPage->m_StrOrganizationIDFld = "";


			//return CPropertyPage::OnApply();
			return FALSE;
		}
	
    }
	m_nDialogMode = MODE_DONE;
	
   return FALSE;
	//return CPropertyPage::OnApply();
}   /* function OnApply  */
/******************************************************************************
 *
 * MODULE:	    OnCancel
 *
 * DESCRIPTION: This function cleans and disables pages then user pressed Cansel
 *				button.
 *
 * AUTHOR:      Irina Arustamova
 *
 * INPUTS:      None             
 *
 * OUTPUTS:     None
 *
 * RTRN VALUE:  None
 *
 * MODIFICATIONS:
 *
 ******************************************************************************/

void CMerchantDefinition::OnCancel() 
{
	if ( pLocationPage->m_bLocation )
	{
		pLocationPage->CleanLocationDialog();
		pLocationPage->EnableLocationDialog(FALSE);
	}
	if ( pBlockingPage->m_bBlocking )
	{
		pBlockingPage->CleanBlockingDialog();
		pBlockingPage->EnableBlockingDialog( FALSE );
	}
	if ( pCapabilitiesPage->m_bCapabilities )
	{
		pCapabilitiesPage->CleanCapabilitiesDialog();
		pCapabilitiesPage->EnableCapabilitiesDialog( FALSE );
	}
	if ( pContactPage->m_bContact)
	{
		pContactPage->CleanContactDialog();
		pContactPage->EnableContactDialog( FALSE );
	}
	if ( pGeneralPage->m_bGeneral)
	{
		pGeneralPage->CleanGeneralDialog();
	}
	if ( pVelocityPage->m_bVelocity )
	{
		pVelocityPage->CleanVelocityDialog();
		pVelocityPage->EnableVelocityDialog( FALSE );
	}
	
	CPropertyPage::OnCancel();
} /* function OnCancel */

void CMerchantDefinition::OnSearch() 
{
	CAlphaLookup alphaDlg;

   UpdateData(TRUE);
    
   alphaDlg.m_nDialogMode = MODE_MERCHANT;
   int nReturnCode = alphaDlg.DoModal();

   if ( nReturnCode == IDOK)
	{
      m_StrMerchantIDFld = alphaDlg.strItem0;
		m_StrOrganizationIDFld = alphaDlg.strItem1;
   }

   m_CntlMerchantIDFld.EnableWindow(TRUE);
   m_CntlOrganizationIDFld.EnableWindow(TRUE);

   UpdateData(FALSE);

	
}


BOOL CMerchantDefinition::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	// TODO: Add your specialized code here and/or call the base class
	
	c_time = CTime::GetCurrentTime();	
	shrmem.SetLastUpdateTime(c_time);
	return CPropertyPage::OnCommand(wParam, lParam);
}
