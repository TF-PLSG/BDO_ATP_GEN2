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
* $Log:   N:\POS\PVCS6.6\EPICPORTZ\PTE\Equitable\ATP_gui\PurchaseCardDefinition.cpp  $
   
      Rev 1.16   Apr 08 2005 08:47:46   lmitchel
   Add delete function to Purchase Card MCC ranges and correcting correct errors from testing
   
      Rev 1.15   Apr 01 2005 10:14:08   lmitchel
   Initial Checking prior to integration testing for updates made to support country/curreny code limits (scr13929), unulimited purchase card MCC ranges, and purchase card transaction limits(scr12785)
   
      Rev 1.14   Dec 01 2003 11:30:04   lmitchel
   Added AMEX Online Updates - scr1200
   
      Rev 1.13   Jul 10 2002 09:56:42   lmitchel
   Ver:4.0.3.1:  Added SendMessage command in PropertySheet files (on_init function) to set the Apply button as the default.  If the enter key is pressed, action will default to  the apply button instead of the OK button.  This allows the user to remain in the current dialog instead of exiting to the main menu.
   
      Rev 1.12   May 21 2002 13:44:02   dirby
   Modified to not exit when <enter> is pressed.  If OK is selected,
   the database gets updated still, but screen is not exited.
   SCR 587
   
      Rev 1.11   Jan 27 2000 13:06:38   iarustam
   SCR # 314,328,330,300,159
   
      Rev 1.10   Jan 13 2000 17:07:48   mbysani
   bug fixed
   
      Rev 1.1   Sep 22 1999 15:58:30   iarustam
   Added manual batch entry GUI
   
      Rev 1.0   Jul 01 1999 14:18:10   iarustam
   Initial Release
   
      Rev 1.0   Jun 28 1999 13:59:16   iarustam
   Initial Release
   
      Rev 1.6   Jun 04 1999 17:40:52   iarustam
   Bug fixes
   
      Rev 1.5   May 10 1999 13:39:52   dperson
   Updated comments
   
      Rev 1.4   May 10 1999 11:32:46   dperson
   Added PVCS header and
   standardized error messages
*   
************************************************************************************/
// PurchaseCardDefinition.cpp : implementation file
//

#include "stdafx.h"
#include "atp.h"
#include "PurchaseCardDefinition.h"
#include "BinList.h"

#include "MyUtils.h"
#include "SharedMem.h"	
//#include "Login.h"
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
extern CSharedMem shrmem;
//extern CLogin login;
/////////////////////////////////////////////////////////////////////////////
// CPurchaseCardDefinition property page

IMPLEMENT_DYNCREATE(CPurchaseCardDefinition, CPropertyPage)

CPurchaseCardDefinition::CPurchaseCardDefinition() : CPropertyPage(CPurchaseCardDefinition::IDD)
{
	//{{AFX_DATA_INIT(CPurchaseCardDefinition)
	m_StrCardNumberFld = _T("");
	m_StrMessageLbl = _T("");
	m_StrCardTypeFld = _T("");
	//}}AFX_DATA_INIT
}

CPurchaseCardDefinition::~CPurchaseCardDefinition()
{
}

void CPurchaseCardDefinition::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPurchaseCardDefinition)
	DDX_Control(pDX, IDC_CARD_NUMBER_FLD, m_CntlCardNumberFld);
	DDX_Control(pDX, IDC_CARD_TYPE, m_CntlCardTypeFld);
	DDX_Control(pDX, IDC_UPDATE_BUTTON, m_UpdateButton);
	DDX_Control(pDX, IDC_VIEW_BUTTON, m_ViewButton);
	DDX_Control(pDX, IDC_LIST_BUTTON, m_ListButton);
	DDX_Control(pDX, IDC_DELETE_BUTTON, m_DeleteButton);
	DDX_Control(pDX, IDC_ADD_BUTTON, m_AddButton);
	DDX_Text(pDX, IDC_CARD_NUMBER_FLD, m_StrCardNumberFld);
	DDV_MaxChars(pDX, m_StrCardNumberFld, 19);
	DDX_Text(pDX, IDC_MESSAGE_LBL, m_StrMessageLbl);
	DDX_CBString(pDX, IDC_CARD_TYPE, m_StrCardTypeFld);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPurchaseCardDefinition, CPropertyPage)
	//{{AFX_MSG_MAP(CPurchaseCardDefinition)
	ON_BN_CLICKED(IDC_ADD_BUTTON, OnAddButton)
	ON_BN_CLICKED(IDC_DELETE_BUTTON, OnDeleteButton)
	ON_BN_CLICKED(IDC_LIST_BUTTON, OnListButton)
	ON_BN_CLICKED(IDC_UPDATE_BUTTON, OnUpdateButton)
	ON_BN_CLICKED(IDC_VIEW_BUTTON, OnViewButton)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPurchaseCardDefinition message handlers

BOOL CPurchaseCardDefinition::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
	CFont m_font;
	LOGFONT lf;
	CString temp;
	int nIndex= -1;

	// set font for the Message lable field

	memset ( &lf, 0, sizeof (lf));
	lf.lfHeight = 12;
	lf.lfWeight = 700;
	strcpy(lf.lfFaceName, "Arial");
	m_font.CreateFontIndirect (&lf);
	GetDlgItem(IDC_MESSAGE_LBL)->SetFont (&m_font);

	m_StrCardNumberFld = _T("");
	m_StrCardTypeFld = _T("");
	m_StrMessageLbl = _T("");

	// set access rights according UCF01 table
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
		if ( ucf01.control[nIndex].action[3] == '0' )
			m_ViewButton.EnableWindow(FALSE);
	}
	else 
	{
		m_AddButton.EnableWindow(FALSE);
		m_DeleteButton.EnableWindow(FALSE);
		m_ViewButton.EnableWindow(FALSE);
		m_UpdateButton.EnableWindow(FALSE);
		m_ListButton.EnableWindow(FALSE);
	}

/*	cntr = 0;
   
   timer_handle = StartTimer(1000);	   

    if (timer_handle == 0)
    {
          AfxMessageBox("Unable to obtain timer");
    }
*/
	UpdateData(FALSE);	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CPurchaseCardDefinition::OnAddButton() 
{
	CCF04 sCCFRecord;
	CCF02P sCCF02Record;
	char strDate[9] = "";
	int rcode = 0;
	CString temp;    
	char strErrorMessage[200];
    
    CMyUtils myUtilsObject;

	UpdateData(TRUE);

	m_StrMessageLbl = "";
	UpdateData(FALSE);

	m_nDialogMode = MODE_ADD;
	
	if (!CheckDialogForCompleteness())
        return;

	// set m_nDialogMode for all pages to MODE_ADD
	 
	pGeneralPage->m_nDialogMode = MODE_ADD;  

	// check if Merchant ID already exists

	ZeroMemory(&sCCFRecord,sizeof(CCF04)) ;

	if ( m_StrCardTypeFld == "CREDIT" )
		temp = "C";
	else if ( m_StrCardTypeFld == "DEBIT" )
		temp = "D";
	else if ( m_StrCardTypeFld == "LOYALTY" )
		temp = "L";
	else if ( m_StrCardTypeFld == "FLEET" )
		temp = "F";

    memcpy ((LPSTR)(sCCFRecord.primary_key.card_type), temp, sizeof (sCCFRecord.primary_key.card_type));
    memcpy ((LPSTR)(sCCFRecord.primary_key.card_nbr), m_StrCardNumberFld, sizeof (sCCFRecord.primary_key.card_nbr));  

	// check if this is valid card number, existing in ccf02p

	memcpy ((LPSTR)(sCCF02Record.primary_key.card_type), temp, sizeof (sCCF02Record.primary_key.card_type));
    memcpy ((LPSTR)(sCCF02Record.primary_key.card_nbr), m_StrCardNumberFld, sizeof (sCCF02Record.primary_key.card_nbr));  

	rcode = txdsapi_get_record( (pBYTE)&sCCF02Record, sizeof (CCF02P), CCF02P_DATA, strErrorMessage );
	if( PTEMSG_OK != rcode )
    {
      m_StrMessageLbl = "ERROR: Invalid Card Number";
      m_nDialogMode = MODE_DONE;
	  m_CntlCardNumberFld.SetFocus();

		if (!UpdateData(FALSE)){
			AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
		}
		return ;
    }
	
	// clean and enable all pages
	if ( pGeneralPage->m_bGeneral )
	{
		pGeneralPage->UpdateData(FALSE);
		pGeneralPage->CleanGeneralDialog();
	}


	// set Card Number and Card Type
	pGeneralPage->m_StrCardTypeFld = m_StrCardTypeFld;
    pGeneralPage->m_StrCardNumberFld = m_StrCardNumberFld;

	pChangeHistoryPage->bNext = FALSE;
	((CPropertySheet*) this->GetParent())->SetWindowText("Purchase Card Account");
	//set version info in the title bar
	CString appProdVer;
	CString lpszTitle;
	appProdVer.LoadString(IDS_PRODUCT_VERSION);
	((CPropertySheet*) this->GetParent())->GetWindowText(lpszTitle);
	lpszTitle = lpszTitle + " - Version - ";
	lpszTitle += appProdVer;
	((CPropertySheet*) this->GetParent())->SetWindowText(lpszTitle);

	m_CntlCardTypeFld.EnableWindow(FALSE);
	m_CntlCardNumberFld.EnableWindow(FALSE);

	// go to first page
	((CPropertySheet*) this->GetParent())->SetActivePage(1);
	SetModified(FALSE);		
}

void CPurchaseCardDefinition::OnDeleteButton() 
{
	CString strMessage;
    int nReturnCode;
	int rcode = 0;
	CCF04 sCCFRecord;
	char strErrorMessage[200];
	UpdateData(TRUE);
    
    m_nDialogMode = MODE_DELETE;

	m_StrMessageLbl = "";
	UpdateData(FALSE);
    if (!CheckDialogForCompleteness())
        return; 

    if (!UpdateData(TRUE)){
		AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
	}
    
	// confirm the action
	strMessage.Format ("Are you sure you want to delete Card Number %s,\n Card Type %s and all %d related MCC Ranges?", 
								(LPCTSTR) m_StrCardNumberFld,(LPCTSTR) m_StrCardTypeFld, pGeneralPage->m_List.GetItemCount()) ;

	nReturnCode = AfxMessageBox ((LPCTSTR)strMessage, MB_OKCANCEL|MB_ICONQUESTION) ;
	if (nReturnCode != IDOK) 
		return ;


    memcpy ((LPTSTR)sCCFRecord.primary_key.card_type,   m_StrCardTypeFld.Left(1),   sizeof (sCCFRecord.primary_key.card_type)) ;
	memcpy ((LPTSTR)sCCFRecord.primary_key.card_nbr,    m_StrCardNumberFld,         sizeof (sCCFRecord.primary_key.card_nbr)) ;

	// try to delete
    rcode = txdsapi_delete_record_by_subtype2( (pBYTE)&sCCFRecord, sizeof( CCF04 ), CCF04_DATA,
		                                        ST2_DB_DELETE_ALL_CCF04_MCC_RECORDS, strErrorMessage );
			  
	// failed to delete
	if ( PTEMSG_OK != rcode   )
    {
		 switch( rcode )
      {
         case PTEMSG_NOT_FOUND:
			sprintf( strErrorMessage, "ERROR: Card Number %s does not exist", m_StrCardNumberFld );
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
        memcpy ((LPSTR)m_strCurrentData, "MCC Records Deleted", sizeof (m_strCurrentData));

        PopulateHistoryTable();

		m_StrCardNumberFld = _T("");	
		m_StrCardTypeFld = _T("");
        m_StrMessageLbl = "MCC Records Deleted";

		if (!UpdateData(FALSE)){
			AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
		}

		// clean and disable all pages
		if ( pGeneralPage->m_bGeneral )
		{
			pGeneralPage->CleanGeneralDialog();
			//pGeneralPage->EnableLocationDialog(FALSE);
		}

    }
	
		// set card number and card type
//		pGeneralPage->m_StrCardTypeFld = m_StrCardTypeFld;
//		pGeneralPage->m_StrCardNumberFld = m_StrCardNumberFld;

	
	    m_CntlCardTypeFld.EnableWindow(TRUE);
    	m_CntlCardNumberFld.EnableWindow(TRUE);

		((CPropertySheet*) this->GetParent())->SetActivePage(0);
		SetModified ( FALSE );
		UpdateData(FALSE);

		
}

void CPurchaseCardDefinition::OnListButton() 
{
	CBinList listDlg;
    int nReturnCode;
	UpdateData(TRUE);

	m_StrMessageLbl = "";
	UpdateData(FALSE);

	listDlg.m_nDialogMode = MODE_PURCHASE;
	listDlg.strItem0 = m_StrCardTypeFld.Left(1);
	listDlg.strItem1 = m_StrCardNumberFld;
 

    nReturnCode = listDlg.DoModal();
    if ( nReturnCode == IDOK)
        m_StrCardNumberFld = listDlg.strItem0;
		m_StrCardTypeFld = listDlg.strItem1;
        
    if (!UpdateData(FALSE)){
		AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
	}		
	
}

void CPurchaseCardDefinition::OnUpdateButton() 
{
	char strErrorMessage[200];
	CCF02P sCCF02Record;
	CString strCardType;
	CString temp;    

	int rcode = 0;


    m_nDialogMode = MODE_EDIT;
	pGeneralPage->m_nDialogMode = MODE_EDIT;  
    pChangeHistoryPage->m_nDialogMode = MODE_EDIT; 
	UpdateData(TRUE);

	m_StrMessageLbl = "";
	UpdateData(FALSE);

    if (!CheckDialogForCompleteness())
        return;

	if ( strcmp (m_StrCardTypeFld, "CREDIT" ) == 0 )
		strCardType = "C";
	else
	if ( strcmp (m_StrCardTypeFld, "DEBIT" ) == 0 )
		strCardType = "D";
	else
	if ( strcmp (m_StrCardTypeFld, "LOYALTY" ) == 0 )
		strCardType = "L";
	else
	if ( strcmp (m_StrCardTypeFld, "FLEET" ) == 0 )
		strCardType = "F";

	
	// check if this is valid card number, existing in ccf02p
	memcpy ((LPSTR)(sCCF02Record.primary_key.card_type), strCardType, sizeof (sCCF02Record.primary_key.card_type));
    memcpy ((LPSTR)(sCCF02Record.primary_key.card_nbr), m_StrCardNumberFld, sizeof (sCCF02Record.primary_key.card_nbr));  

	rcode = txdsapi_get_record( (pBYTE)&sCCF02Record, sizeof (CCF02P), CCF02P_DATA, strErrorMessage );
	if( PTEMSG_OK != rcode )
    {
      m_StrMessageLbl = "ERROR: Invalid Card Number";
      m_nDialogMode = MODE_DONE;
	  m_CntlCardNumberFld.SetFocus();

		if (!UpdateData(FALSE)){
			AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
		}
		return ;
    }


	// clean and enable all pages
	if ( pGeneralPage->m_bGeneral )
	{
		pGeneralPage->UpdateData(FALSE);
		pGeneralPage->CleanGeneralDialog();
	}

	// set Card Number and Card Type
	pGeneralPage->m_StrCardTypeFld = m_StrCardTypeFld;
    pGeneralPage->m_StrCardNumberFld = m_StrCardNumberFld;

	pChangeHistoryPage->bNext = FALSE;
	((CPropertySheet*) this->GetParent())->SetWindowText("Purchase Card Account");
	//set version info in the title bar
	CString appProdVer;
	CString lpszTitle;
	appProdVer.LoadString(IDS_PRODUCT_VERSION);
	((CPropertySheet*) this->GetParent())->GetWindowText(lpszTitle);
	lpszTitle = lpszTitle + " - Version - ";
	lpszTitle += appProdVer;
	((CPropertySheet*) this->GetParent())->SetWindowText(lpszTitle);

	m_CntlCardTypeFld.EnableWindow(FALSE);
	m_CntlCardNumberFld.EnableWindow(FALSE);

	// go to first page
	((CPropertySheet*) this->GetParent())->SetActivePage(1);
	SetModified(FALSE);		

}

void CPurchaseCardDefinition::OnViewButton() 
{
    HST01 sHistoryRecord;
//	CCF02P sCCF02Record;
	CCF04 sCCFRecord;
	CString strCardType;

	memset( &sCCFRecord, 0, sizeof( CCF04 ) );
    m_nDialogMode = MODE_VIEW;
	pGeneralPage->m_nDialogMode = MODE_VIEW;  
    pChangeHistoryPage->m_nDialogMode = MODE_VIEW; 
//	UpdateData(TRUE);

	m_StrMessageLbl = "";
//	UpdateData(FALSE);

    if (!CheckDialogForCompleteness())
        return;

	// get data from DB
	if ( strcmp (m_StrCardTypeFld, "CREDIT" ) == 0 )
		strCardType = "C";
	else
	if ( strcmp (m_StrCardTypeFld, "DEBIT" ) == 0 )
		strCardType = "D";
	else
	if ( strcmp (m_StrCardTypeFld, "LOYALTY" ) == 0 )
		strCardType = "L";
	else
	if ( strcmp (m_StrCardTypeFld, "FLEET" ) == 0 )
		strCardType = "F";

    memcpy ((LPSTR)(sCCFRecord.primary_key.card_type), strCardType, sizeof (sCCFRecord.primary_key.card_type) );
    memcpy ((LPSTR)(sCCFRecord.primary_key.card_nbr),  m_StrCardNumberFld,  sizeof (sCCFRecord.primary_key.card_nbr) );   


	// clean and enable all pages before populating them
	if ( pGeneralPage->m_bGeneral )
	    {
    		pGeneralPage->CleanGeneralDialog();
		}

	// get history data
    ZeroMemory (&sHistoryRecord, sizeof (HST01));

	memset ( strFilePrimaryKey, 0, sizeof (strFilePrimaryKey));
	strcpy ( strFilePrimaryKey, (char *)sCCFRecord.primary_key.card_nbr);
	strcat ( strFilePrimaryKey, (char *)sCCFRecord.primary_key.card_type);

    memcpy ((LPSTR)(sHistoryRecord.primary_key.unique_id), " ", sizeof (sHistoryRecord.primary_key.unique_id));
    memcpy ((LPSTR)(sHistoryRecord.file_name), "CCF04", sizeof (sHistoryRecord.file_name));
    memcpy ((LPSTR)(sHistoryRecord.file_primary_key), strFilePrimaryKey, sizeof (sHistoryRecord.file_primary_key));
    memcpy ( &(pChangeHistoryPage->sHistoryRecord), &sHistoryRecord, sizeof(sHistoryRecord));

	// set card number and card type
	pGeneralPage->m_StrCardTypeFld = m_StrCardTypeFld;
	pGeneralPage->m_StrCardNumberFld = m_StrCardNumberFld;
    

	// set dialog mode for all pages
	pChangeHistoryPage->bNext = FALSE;

	((CPropertySheet*) this->GetParent())->SetWindowText("View Purchase Card");
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

	pGeneralPage->SetModified ( FALSE );	

	m_CntlCardNumberFld.EnableWindow(TRUE);
	m_CntlCardTypeFld.EnableWindow(TRUE);

	
	//go to first page	
	((CPropertySheet*) this->GetParent())->SetActivePage(1);    
	SetModified (FALSE);
}

void CPurchaseCardDefinition::PopulateHistoryTable()
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

	if ( strcmp (m_StrCardTypeFld, "CREDIT" ) == 0 )
		temp = "C";
	else
	if ( strcmp (m_StrCardTypeFld, "DEBIT" ) == 0 )
		temp = "D";
	else
	if ( strcmp (m_StrCardTypeFld, "LOYALTY" ) == 0 )
		temp = "L";
	else
	if ( strcmp (m_StrCardTypeFld, "FLEET" ) == 0 )
		temp = "F";

	strcpy ( strPrimaryKey, m_StrCardNumberFld);
	strcat ( strPrimaryKey, m_StrCardTypeFld );
    
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
    memcpy ((LPTSTR) sHistoryData.file_name,     (LPCTSTR)"CCF04", sizeof (sHistoryData.file_name));
    memcpy ((LPTSTR) sHistoryData.file_primary_key,  (LPCTSTR)strPrimaryKey, sizeof (sHistoryData.file_primary_key));
    memcpy ((LPTSTR) sHistoryData.primary_key.unique_id,     (LPCTSTR)dest_str, sizeof (sHistoryData.primary_key.unique_id));        
    memcpy ((LPTSTR) sHistoryData.time_modified,     (LPCTSTR)strTime, sizeof (sHistoryData.time_modified)); 
    memcpy ((LPTSTR) sHistoryData.current_data, (LPCTSTR)m_strCurrentData, sizeof (sHistoryData.current_data));
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

BOOL CPurchaseCardDefinition::CheckDialogForCompleteness()
{
    CMyUtils myUtilsObject;


    if (!UpdateData(TRUE)){
		AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
	}

    if (myUtilsObject.IsEmpty (m_StrCardNumberFld, this))
    {
		m_StrMessageLbl = "Please enter Card Number";	
		m_CntlCardNumberFld.SetFocus();
		if (!UpdateData(FALSE)){
			AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
		}
		m_nDialogMode = MODE_DONE;
		return FALSE;
	}

	if (myUtilsObject.IsEmpty (m_StrCardTypeFld, this))
    {
		m_StrMessageLbl = "Please enter Card Type";	
		m_CntlCardTypeFld.SetFocus();
		if (!UpdateData(FALSE)){
			AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
		}
		m_nDialogMode = MODE_DONE;
		return FALSE;
	}
	
    return TRUE;

}   /* function CheckDialogForCompleteness */

BOOL CPurchaseCardDefinition::PopulatePurchaseDatabaseStructureFromDialog ( pCCF04 psCCFRecord )
{
/*	CString strType;
	ZeroMemory (psCCFRecord, sizeof(CCF04)) ;	
    
	if ( strcmp (m_StrCardTypeFld, "CREDIT" ) == 0 )
		strType = "C";
	else
	if ( strcmp (m_StrCardTypeFld, "DEBIT" ) == 0 )
		strType = "D";
	else
	if ( strcmp (m_StrCardTypeFld, "LOYALTY" ) == 0 )
		strType = "L";
	else
	if ( strcmp (m_StrCardTypeFld, "FLEET" ) == 0 )
		strType = "F";

	memcpy ((LPTSTR) psCCFRecord->primary_key.card_type, strType, sizeof (psCCFRecord->primary_key.card_type));
	memcpy ((LPTSTR) psCCFRecord->primary_key.card_nbr,m_StrCardNumberFld, sizeof (psCCFRecord->primary_key.card_nbr));
	memcpy ((LPTSTR) psCCFRecord->mcc_lo_range, m_StrLoFld, sizeof(psCCFRecord->mcc_lo_range));
	memcpy ((LPTSTR) psCCFRecord->mcc_hi_range, m_StrHiFld, sizeof(psCCFRecord->mcc_hi_range));
*/

//	if (!pGeneralPage->PopulateDatabaseFromGeneralDialog ( psCCFRecord ))
//		return FALSE;

	return TRUE;
}  /* function PopulatePurchaseDatabaseStructureFromDialog */

void CPurchaseCardDefinition::PopulatePurchaseDialogFromDatabase( CCF04 sCCFRecord )
{
	// if page was open - populate fields on the page
	// if page was not open - populate MCF01 structure for that page and
	//                        it going to be populated in OnInitDialog for this page
	if ( !pGeneralPage->m_bGeneral )
		memcpy( &pGeneralPage->sCCF04RecordOld, &sCCFRecord, sizeof (CCF04) );
	else
		pGeneralPage->PopulateGeneralDialogFromDatabase(sCCFRecord);

}  /* function PopulatePurchaseDialogFromDatabase  */

BOOL CPurchaseCardDefinition::OnApply() 
{
/*	CCF04 sCCFDatabaseStructure;
	char strErrorMessage[200];

	CMyUtils myUtilsObject;
*/
   CString buttonlabel;
   CWnd*   activebutton;

   /* This snippet of code prevents updating the database
    * and exiting the window when <enter> is pressed. The
    * database will only get updated when APPLY is selected
    * or if focus is on the OK button when <enter> is pressed
    * or OK is selected.
    */
   activebutton = GetFocus();
   activebutton->GetWindowText(buttonlabel);
   if ( buttonlabel != "&Apply"  && buttonlabel != "OK")
   {
      return FALSE;
   }

	// check for required fields
/*   if ( !pGeneralPage->CheckDialogForCompleteness() )
      return FALSE;
*/	
    // ****************************************************************************************
/*	if (m_nDialogMode == MODE_ADD)
	{			
		memset(&sCCFDatabaseStructure, 0, sizeof(sCCFDatabaseStructure)) ;*/
/*		if (!PopulatePurchaseDatabaseStructureFromDialog (&sCCFDatabaseStructure)) {		
			return FALSE;
		}
*/

/*		if (!pGeneralPage->PopulateDatabaseFromGeneralDialog ( &sCCFDatabaseStructure ))
		return FALSE;

		int rcode = txdsapi_insert_record ( (pBYTE)&sCCFDatabaseStructure, sizeof (CCF04), CCF04_DATA, strErrorMessage );
		if ( rcode != PTEMSG_OK )
		{  	
			m_StrMessageLbl = strErrorMessage;
			m_nDialogMode = MODE_DONE;
			if (!UpdateData(FALSE)){
				AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
			}
			((CPropertySheet*) this->GetParent())->SetActivePage(0);
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
			pGeneralPage->CleanGeneralDialog();


			if ( pChangeHistoryPage->m_bChangeHistory )
				pChangeHistoryPage->CleanChangeHistory();

			SetModified ( FALSE );
			m_nDialogMode = MODE_DONE;
			pGeneralPage->m_nDialogMode = MODE_DONE;  
			pChangeHistoryPage->m_nDialogMode = MODE_DONE; 

			m_CntlCardNumberFld.SetFocus();
			m_CntlCardNumberFld.EnableWindow(TRUE);
			m_CntlCardTypeFld.EnableWindow(TRUE);

			//return CPropertyPage::OnApply();
         return FALSE;
		} 
	}*/
//////////////////////////////////////////////////////////////////////////////////////////
/*	if (m_nDialogMode == MODE_EDIT)
	{ 
		CCF04 sCCFDatabaseStructure;
        memset(&sCCFDatabaseStructure, 0, sizeof(CCF04)) ;

		PopulatePurchaseDatabaseStructureFromDialog (&sCCFDatabaseStructure);

		int rcode = txdsapi_update_record ( (pBYTE)&sCCFDatabaseStructure, sizeof (CCF04), CCF04_DATA, strErrorMessage );
		if ( rcode != PTEMSG_OK )
		{ 		
			m_StrMessageLbl = strErrorMessage;
			m_nDialogMode = MODE_DONE;
			((CPropertySheet*) this->GetParent())->SetActivePage(0);
			((CPropertySheet*) this->GetParent())->SetWindowText("Purchase Card Definition");

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
			pGeneralPage->CompareStructures();

			pGeneralPage->CleanGeneralDialog();
			if ( pChangeHistoryPage->m_bChangeHistory )
				pChangeHistoryPage->CleanChangeHistory();
			((CPropertySheet*) this->GetParent())->SetActivePage(0);
			((CPropertySheet*) this->GetParent())->SetWindowText("Purchase Card Definition");

			m_CntlCardNumberFld.EnableWindow(TRUE);
			m_CntlCardTypeFld.EnableWindow(TRUE);

			SetModified ( FALSE );

			m_nDialogMode = MODE_DONE;
			pGeneralPage->m_nDialogMode = MODE_DONE;  
			pChangeHistoryPage->m_nDialogMode = MODE_DONE; 
			//return CPropertyPage::OnApply();
         return FALSE;
		}
	
    }*/
	m_nDialogMode = MODE_DONE;	
	return CPropertyPage::OnApply();	
   //return FALSE;
}

void CPurchaseCardDefinition::OnCancel() 
{
	// TODO: Add your specialized code here and/or call the base class
	//StopTimer();
	CPropertyPage::OnCancel();
}

BOOL CPurchaseCardDefinition::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	// TODO: Add your specialized code here and/or call the base class
	c_time = CTime::GetCurrentTime();	
	shrmem.SetLastUpdateTime(c_time);
	return CPropertyPage::OnCommand(wParam, lParam);
}
