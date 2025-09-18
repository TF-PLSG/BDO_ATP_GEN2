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
* $Log:   N:\PVCS\PTE\Equitable\ATP_gui\CurrencyCode.cpp  $
   
      Rev 1.13   Dec 01 2003 11:29:12   lmitchel
   Added AMEX Online Updates - scr1200
   
      Rev 1.12   Jul 10 2002 09:56:04   lmitchel
   Ver:4.0.3.1:  Added SendMessage command in PropertySheet files (on_init function) to set the Apply button as the default.  If the enter key is pressed, action will default to  the apply button instead of the OK button.  This allows the user to remain in the current dialog instead of exiting to the main menu.
   
      Rev 1.11   May 21 2002 13:34:50   dirby
   Modified to not exit when <enter> is pressed.  If OK is selected,
   the database gets updated still, but screen is not exited.
   SCR 587
   
      Rev 1.10   Jan 21 2000 12:22:56   iarustam
   Bug fixes # 14,24,,52,63,65,151,165,91,228,202,180,205,504,152,179,251,109,182,177,183,224,225,217,229,230,231,247,200,199,203,201,206,172
   
      Rev 1.9   Jan 14 2000 16:25:02   mbysani
   bug fixes
   
      Rev 1.4   Oct 15 1999 17:09:12   iarustam
   bug fixes
   
      Rev 1.3   Sep 30 1999 16:17:08   npogosya
   Bug fixes
   
      Rev 1.1   Sep 22 1999 15:47:46   iarustam
   Added manual batch entry GUI
   
      Rev 1.0   Jul 01 1999 14:17:16   iarustam
   Initial Release
   
      Rev 1.0   Jun 28 1999 13:58:22   iarustam
   Initial Release
       
*
************************************************************************************/
// CurrencyCode.cpp : implementation file
//

#include "stdafx.h"
#include "Atp.h"
#include "CurrencyCode.h"
#include "MyUtils.h"
#include "NameList.h"
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
extern CString name;

/////////////////////////////////////////////////////////////////////////////
// CCurrencyCode property page

IMPLEMENT_DYNCREATE(CCurrencyCode, CPropertyPage)

CCurrencyCode::CCurrencyCode() : CPropertyPage(CCurrencyCode::IDD)
{
	//{{AFX_DATA_INIT(CCurrencyCode)
	m_StrCurrencyCodeFld = _T("");
	m_StrMessageLbl = _T("");
	//}}AFX_DATA_INIT
}

CCurrencyCode::~CCurrencyCode()
{
}

void CCurrencyCode::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCurrencyCode)
	DDX_Control(pDX, IDC_VIEW_BUTTON, m_ViewButton);
	DDX_Control(pDX, IDC_UPDATE_BUTTON, m_UpdateButton);
	DDX_Control(pDX, IDC_LIST_BUTTON, m_ListButton);
	DDX_Control(pDX, IDC_DELETE_BUTTON, m_DeleteButton);
	DDX_Control(pDX, IDC_ADD_BUTTON, m_AddButton);
	DDX_Control(pDX, IDC_CURRENCY_CODE_FLD, m_CntlCurrencyCodeFld);
	DDX_Text(pDX, IDC_CURRENCY_CODE_FLD, m_StrCurrencyCodeFld);
	DDV_MaxChars(pDX, m_StrCurrencyCodeFld, 3);
	DDX_Text(pDX, IDC_MESSAGE_LBL, m_StrMessageLbl);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CCurrencyCode, CPropertyPage)
	//{{AFX_MSG_MAP(CCurrencyCode)
	ON_BN_CLICKED(IDC_ADD_BUTTON, OnAddButton)
	ON_BN_CLICKED(IDC_DELETE_BUTTON, OnDeleteButton)
	ON_BN_CLICKED(IDC_LIST_BUTTON, OnListButton)
	ON_BN_CLICKED(IDC_UPDATE_BUTTON, OnUpdateButton)
	ON_BN_CLICKED(IDC_VIEW_BUTTON, OnViewButton)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCurrencyCode message handlers

BOOL CCurrencyCode::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();	
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
		if ( strcmp ( temp, "BIN Control" ) == 0)
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
	m_StrCurrencyCodeFld = "";
	m_StrMessageLbl = "";
   m_nDialogMode = MODE_DONE;


	UpdateData(FALSE);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CCurrencyCode::OnAddButton() 
{
	CRF01 sCurrencyCode; 
	char strErrorMessage[200];   
   CMyUtils myUtilsObject;

	m_StrMessageLbl = "";

	m_nDialogMode = MODE_ADD;  

	if (!CheckDialogForCompleteness())
        return;
	 
	pConversionRatePage->m_nDialogMode = MODE_ADD;
	pChangeHistoryPage->m_nDialogMode = MODE_ADD;

	ZeroMemory(&sCurrencyCode,sizeof(CRF01)) ;

   memcpy ((LPSTR)(sCurrencyCode.primary_key.currency_code), m_StrCurrencyCodeFld , sizeof (sCurrencyCode.primary_key.currency_code));

   int rcode = txdsapi_get_record( (pBYTE)&sCurrencyCode, sizeof (CRF01), CRF01_DATA, strErrorMessage );
     if(rcode == PTEMSG_OK )    
     {
	    m_StrMessageLbl = "ERROR: Currency Code already exists";
       m_nDialogMode = MODE_DONE;

	    if (!UpdateData(FALSE))
		    AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);	
       
		 return ;  
     }
	
	((CPropertySheet*) this->GetParent())->SetWindowText("Add Currency Code");
	//set version info in the title bar
	CString appProdVer;
	CString lpszTitle;
	appProdVer.LoadString(IDS_PRODUCT_VERSION);
	((CPropertySheet*) this->GetParent())->GetWindowText(lpszTitle);
	lpszTitle = lpszTitle + " - Version - ";
	lpszTitle += appProdVer;
	((CPropertySheet*) this->GetParent())->SetWindowText(lpszTitle);

	if ( pConversionRatePage->m_bConversion )
	 {
		pConversionRatePage->CleanConvertCodeDialog();
		pConversionRatePage->UpdateData(FALSE);
		pConversionRatePage->EnableConvertCodeDialog(TRUE);
		
	 }
	pConversionRatePage->m_StrCurrencyCodeFld = m_StrCurrencyCodeFld;

   if ( pChangeHistoryPage->m_bChangeHistory)
	{  
   	pChangeHistoryPage->UpdateData(FALSE); 
		pChangeHistoryPage->EnableWindow(FALSE);
	}            
	
	((CPropertySheet*) this->GetParent())->SetActivePage(1);
	m_CntlCurrencyCodeFld.EnableWindow(FALSE);	
	
}

void CCurrencyCode::OnDeleteButton() 
{
	CString strMessage;
    int nReturnCode;
    CRF01 sCurrencyCode; 
	char strErrorMessage[200];

	m_StrMessageLbl = "";

    m_nDialogMode = MODE_EDIT;	

    if (!CheckDialogForCompleteness())
        return;   
   
    if (!UpdateData(TRUE))
     {
		AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
	 }
    
	strMessage.Format ("Are you sure you want to delete %s ?", (LPCTSTR) m_StrCurrencyCodeFld) ;

	nReturnCode = AfxMessageBox ((LPCTSTR)strMessage, MB_OKCANCEL|MB_ICONQUESTION) ;

	if (nReturnCode != IDOK)
		return ;
	memcpy ((LPTSTR)sCurrencyCode.primary_key.currency_code, m_StrCurrencyCodeFld, sizeof (sCurrencyCode.primary_key.currency_code)) ;

    int rcode = txdsapi_delete_record ((pBYTE)&sCurrencyCode, sizeof (CRF01), CRF01_DATA, strErrorMessage);
    if(rcode != PTEMSG_OK )    
     {
		m_StrMessageLbl = "ERROR: Currency Code does not exist";
      m_nDialogMode = MODE_DONE;

		if (!UpdateData(FALSE))
         {
			AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
		 }
		return ;
	 }

  else
     {
        m_StrMessageLbl = "Record Deleted";
        
		if (!UpdateData(FALSE))
         {
			AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
		 }
		memset ( m_strPreviousData, 0, sizeof (m_strPreviousData));
		memset ( m_strFieldTag, 0, sizeof (m_strFieldTag));
        memcpy ((LPSTR)m_strCurrentData, "Record Deleted", sizeof (m_strCurrentData));
        PopulateHistoryTable();
		m_StrCurrencyCodeFld = "";
		if ( pConversionRatePage->m_bConversion )
		 {
			pConversionRatePage->CleanConvertCodeDialog();
			pConversionRatePage->EnableConvertCodeDialog(FALSE);
			UpdateData(FALSE);
		 }
		
		m_CntlCurrencyCodeFld.EnableWindow(TRUE);

		if ( pChangeHistoryPage->m_bChangeHistory )
			pChangeHistoryPage->CleanChangeHistory();

		m_nDialogMode = MODE_DONE;

		((CPropertySheet*) this->GetParent())->SetActivePage(0);
		SetModified ( FALSE );
     }
	
}

void CCurrencyCode::OnListButton() 
{
	CNameList listDlg;
    int nReturnCode;

	m_StrMessageLbl = "";
	UpdateData(TRUE);

    listDlg.m_nIndicator = MODE_CONVERSION;   
    listDlg.strPrimaryKey1 = m_StrCurrencyCodeFld;
    nReturnCode = listDlg.DoModal();
    if ( nReturnCode == IDOK)
        m_StrCurrencyCodeFld = name;
        
    if (!UpdateData(FALSE))
     {
		AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
	 }	
}

void CCurrencyCode::OnUpdateButton() 
{
    HST01 sHistoryRecord;
	char strErrorMessage[200];
    
    
    m_nDialogMode = MODE_EDIT;
    pConversionRatePage->m_nDialogMode = MODE_EDIT;
    pChangeHistoryPage->m_nDialogMode = MODE_EDIT;
	pChangeHistoryPage->bNext = FALSE;

	m_StrMessageLbl = "";

    if (!CheckDialogForCompleteness())
        return;
    memcpy ((LPSTR)(sCurrencyCodeRecordOld.primary_key.currency_code), m_StrCurrencyCodeFld, sizeof (sCurrencyCodeRecordOld.primary_key.currency_code) );
    int rcode = txdsapi_get_record( (pBYTE)&sCurrencyCodeRecordOld, sizeof (CRF01), CRF01_DATA, strErrorMessage );
    if(rcode != PTEMSG_OK )   
    {
		m_StrMessageLbl = "ERROR: Currency Code does not exist";
      m_nDialogMode = MODE_DONE;

	   if (!UpdateData(FALSE))
       {
		   AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
	   }
       return;
    }

	pConversionRatePage->m_StrCurrencyCodeFld = m_StrCurrencyCodeFld;    

    ZeroMemory (&sHistoryRecord, sizeof (sHistoryRecord));

    memcpy ((LPSTR)(sHistoryRecord.primary_key.unique_id), "                 ", sizeof (sHistoryRecord.primary_key.unique_id));
    memcpy ((LPSTR)(sHistoryRecord.file_name), "CRF01", sizeof (sHistoryRecord.file_name));
    memcpy ((LPSTR)(sHistoryRecord.file_primary_key), (LPSTR)sCurrencyCodeRecordOld.primary_key.currency_code, sizeof (sHistoryRecord.file_primary_key));

    memcpy ( &(pChangeHistoryPage->sHistoryRecord), &sHistoryRecord, sizeof(sHistoryRecord));
    
	((CPropertySheet*) this->GetParent())->SetActivePage(1);  
    

	((CPropertySheet*) this->GetParent())->SetWindowText("Update Currency Code");
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
	PopulateCurrencyCodeDialogFromDatabase ( sCurrencyCodeRecordOld );

	m_CntlCurrencyCodeFld.EnableWindow(FALSE);	
	
}

void CCurrencyCode::OnViewButton() 
{
    HST01 sHistoryRecord;
	char strErrorMessage[200];
   
    m_nDialogMode = MODE_VIEW;
    pConversionRatePage->m_nDialogMode = MODE_VIEW;
    pChangeHistoryPage->m_nDialogMode = MODE_VIEW;
	pChangeHistoryPage->bNext = FALSE;

	m_StrMessageLbl = "";

    if (!CheckDialogForCompleteness())
        return;
    memcpy ((LPSTR)(sCurrencyCodeRecordOld.primary_key.currency_code), m_StrCurrencyCodeFld, sizeof (sCurrencyCodeRecordOld.primary_key.currency_code) );
    int rcode = txdsapi_get_record( (pBYTE)&sCurrencyCodeRecordOld, sizeof (CRF01), CRF01_DATA, strErrorMessage );
    if(rcode != PTEMSG_OK )   
    {
		m_StrMessageLbl = "ERROR: Currency Code does not exist";
      m_nDialogMode = MODE_DONE;

	   if (!UpdateData(FALSE))
       {
		   AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
	   }
       return;
    }

	pConversionRatePage->m_StrCurrencyCodeFld = m_StrCurrencyCodeFld;   

    ZeroMemory (&sHistoryRecord, sizeof (sHistoryRecord));

    memcpy ((LPSTR)(sHistoryRecord.primary_key.unique_id), "                 ", sizeof (sHistoryRecord.primary_key.unique_id));
    memcpy ((LPSTR)(sHistoryRecord.file_name), "CRF01", sizeof (sHistoryRecord.file_name));
    memcpy ((LPSTR)(sHistoryRecord.file_primary_key), (LPSTR)sCurrencyCodeRecordOld.primary_key.currency_code, sizeof (sHistoryRecord.file_primary_key));

    memcpy ( &(pChangeHistoryPage->sHistoryRecord), &sHistoryRecord, sizeof(sHistoryRecord));
    
	((CPropertySheet*) this->GetParent())->SetActivePage(1);  
    

	((CPropertySheet*) this->GetParent())->SetWindowText("View Currency Code");
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
	PopulateCurrencyCodeDialogFromDatabase ( sCurrencyCodeRecordOld );

	m_CntlCurrencyCodeFld.EnableWindow(TRUE);	
	
}

/**************************************************************/
 //////////////////////////////////////////////////////////////
/**************************************************************/

BOOL CCurrencyCode::PopulateCurrencyCodeDialogFromDatabase ( CRF01 pCurrencyCode)
{
	
   pConversionRatePage->m_StrCurrencyCodeFld = (LPSTR)pCurrencyCode.primary_key.currency_code;
	pConversionRatePage->m_StrConversionRateFld = (LPSTR)pCurrencyCode.conversion_rate;
	pConversionRatePage->m_StrNameFld = (LPSTR)pCurrencyCode.currency_name;

	pConversionRatePage->UpdateData(FALSE);
	pConversionRatePage->EnableConvertCodeDialog(TRUE );	

   return TRUE;
}

/**************************************************************/
 //////////////////////////////////////////////////////////////
/**************************************************************/

BOOL CCurrencyCode::PopulateCurrencyCodeFromDialog ( pCRF01 pCurrencyCode ) 
{
   ZeroMemory (pCurrencyCode, sizeof(CRF01)) ;	
       
	memcpy ((LPTSTR) pCurrencyCode->primary_key.currency_code,m_StrCurrencyCodeFld, sizeof (pCurrencyCode->primary_key.currency_code));
	memcpy ((LPTSTR) pCurrencyCode->conversion_rate, pConversionRatePage->m_StrConversionRateFld, sizeof (pCurrencyCode->conversion_rate));
	memcpy ((LPTSTR) pCurrencyCode->currency_name, pConversionRatePage->m_StrNameFld, sizeof (pCurrencyCode->currency_name));

	if ( m_nDialogMode == MODE_EDIT )
	{
		if ( strcmp ( (char *)sCurrencyCodeRecordOld.conversion_rate, pConversionRatePage->m_StrConversionRateFld)!= 0)
		{
			memcpy ((LPTSTR) m_strCurrentData,pConversionRatePage->m_StrConversionRateFld, sizeof (m_strCurrentData));
			memcpy ((LPTSTR) m_strPreviousData,sCurrencyCodeRecordOld.conversion_rate, sizeof (m_strPreviousData));
			memcpy ((LPTSTR) m_strFieldTag,"Conversion Rate", sizeof (m_strFieldTag));
			PopulateHistoryTable();		
		}

      if ( strcmp ( (char *)sCurrencyCodeRecordOld.currency_name, pConversionRatePage->m_StrNameFld)!= 0)
		{
			memcpy ((LPTSTR) m_strCurrentData,pConversionRatePage->m_StrNameFld, sizeof (m_strCurrentData));
			memcpy ((LPTSTR) m_strPreviousData,sCurrencyCodeRecordOld.currency_name, sizeof (m_strPreviousData));
			memcpy ((LPTSTR) m_strFieldTag,"Currency Name", sizeof (m_strFieldTag));
			PopulateHistoryTable();		
		}
	}
	
    	
	return TRUE;
}

/**************************************************************/
 //////////////////////////////////////////////////////////////
/**************************************************************/

void CCurrencyCode::PopulateHistoryTable()
{
    
    HST01 sHistoryData;
    CHAR dest_str[18];   
	CMyUtils myUtilsObject;
    char strErrorMsg[200] = "";
    char strDate[9] = "";
    char strTime[7] = "";
    CString temp;
    
    memset ( strDate, 0, sizeof (strDate));
    memset ( strTime, 0, sizeof (strTime));
    
    myUtilsObject.GetSystemDate(strDate);  

    myUtilsObject.GetSystemTime(strTime);
    pintime_get_gmt ( dest_str );
    ZeroMemory ( &sHistoryData, sizeof (HST01));
    memcpy ((LPTSTR) sHistoryData.file_name,     (LPCTSTR)"CRF01", sizeof (sHistoryData.file_name));
    temp = strDate;
    temp = temp.Right(4) + temp.Left(2) + temp.Mid(2,2);
    
    CHAR wstation[21]; DWORD wsize;

    wsize = sizeof( wstation );
    GetComputerName( wstation, &wsize );

	memcpy ((LPTSTR)sHistoryData.reserved,(LPTSTR)wstation, sizeof(sHistoryData.reserved));
    memcpy ((LPTSTR) sHistoryData.date_modified, temp, sizeof (sHistoryData.date_modified));
    memcpy ((LPTSTR) sHistoryData.file_primary_key,  (LPCTSTR)m_StrCurrencyCodeFld, sizeof (sHistoryData.file_primary_key));
    memcpy ((LPTSTR) sHistoryData.primary_key.unique_id,     (LPCTSTR)dest_str, sizeof (sHistoryData.primary_key.unique_id));        
    memcpy ((LPTSTR) sHistoryData.time_modified,     (LPCTSTR)strTime, sizeof (sHistoryData.time_modified)); 
             
    memcpy ((LPTSTR)sHistoryData.current_data, (LPCTSTR)m_strCurrentData, sizeof (sHistoryData.current_data));
	memcpy ((LPTSTR)sHistoryData.previous_data, (LPCTSTR)m_strPreviousData, sizeof (sHistoryData.previous_data));
	memcpy ((LPTSTR)sHistoryData.changed_by, (LPCTSTR)ucf01.primary_key.name, sizeof (sHistoryData.changed_by));
	memcpy ((LPTSTR)sHistoryData.field_tag, (LPCTSTR)m_strFieldTag, sizeof (sHistoryData.field_tag));  
    int rcode = txdsapi_insert_record  ( (pBYTE)&sHistoryData, sizeof (HST01), HST01_DATA, strErrorMsg );
	if ( rcode != PTEMSG_OK )
	{
		m_StrMessageLbl = strErrorMsg;
		UpdateData( FALSE );
	}
    
}

 
/**************************************************************/
 //////////////////////////////////////////////////////////////
/**************************************************************/

 BOOL CCurrencyCode::CheckDialogForCompleteness()
{  
    CMyUtils myUtilsObject;
    
    if (!UpdateData(TRUE))
    {
		AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
	 }

    if (myUtilsObject.IsEmpty (m_StrCurrencyCodeFld, this))
    {
		m_StrMessageLbl = "Please enter Currency Code";
		m_nDialogMode = MODE_DONE;
		m_CntlCurrencyCodeFld.SetFocus();

	   if (!UpdateData(FALSE))
		   AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
	
		return FALSE;
    }      
    return TRUE;
}

void CCurrencyCode::OnCancel() 
{
if ( pChangeHistoryPage->m_bChangeHistory )
		pChangeHistoryPage->CleanChangeHistory();

	if ( pConversionRatePage->m_bConversion)
	{
		pConversionRatePage->CleanConvertCodeDialog();
		pConversionRatePage->EnableConvertCodeDialog(FALSE);	
	}
	
	CPropertyPage::OnCancel();
}

BOOL CCurrencyCode::OnApply() 
{ 
	CRF01 sCurrencyDatabaseStructure;
	char strErrorMessage[200];
	
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
	UpdateData(TRUE); 

   if ( m_nDialogMode == MODE_EDIT || m_nDialogMode == MODE_ADD )
   {
      if ( 0 == strcmp(pConversionRatePage->m_StrConversionRateFld, "0000.0000") )
      {
         pConversionRatePage->m_StrMessageLbl = "Please enter Conversion Rate";
         pConversionRatePage->UpdateData(FALSE);
			((CPropertySheet*) this->GetParent())->SetActivePage(1);
         return FALSE;
      }
   }
	
    if (m_nDialogMode == MODE_EDIT)
	{ 
        memset(&sCurrencyDatabaseStructure, 0, sizeof(sCurrencyDatabaseStructure)) ;
		if ( !pConversionRatePage->m_bConversion )
			((CPropertySheet*) this->GetParent())->SetActivePage(1);
		PopulateCurrencyCodeFromDialog (&sCurrencyDatabaseStructure );

		int rcode = txdsapi_update_record ((pBYTE)&sCurrencyDatabaseStructure, sizeof (CRF01), CRF01_DATA, strErrorMessage);
		if (rcode != PTEMSG_OK)
		{    	
			m_StrMessageLbl = strErrorMessage;

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
			if (pConversionRatePage->m_bConversion)
			{
			pConversionRatePage->CleanConvertCodeDialog();
			pConversionRatePage->EnableConvertCodeDialog(FALSE);
			}
			
			if ( pChangeHistoryPage->m_bChangeHistory )
				pChangeHistoryPage->CleanChangeHistory();
			((CPropertySheet*) this->GetParent())->SetActivePage(0);
			SetModified ( FALSE );
			m_nDialogMode = MODE_DONE;
			pConversionRatePage->m_nDialogMode = MODE_DONE;
			m_CntlCurrencyCodeFld.EnableWindow(TRUE);
			//return CPropertyPage::OnApply();
         return FALSE;
		}	
    }

	if (m_nDialogMode == MODE_ADD)
	{
		if (!pConversionRatePage->CheckDialogForCompleteness())
		{
			return FALSE;
		}

		if (pConversionRatePage->m_StrConversionRateFld == "0000.0000")
		{
		 pConversionRatePage->m_StrMessageLbl = "ERROR: Conversion Rate cannot be 0000.0000";
		 pConversionRatePage->UpdateData(FALSE);		
		 pConversionRatePage->m_CntlConversionRateFld.SetFocus();
	     if (!UpdateData(FALSE))
         {
		    AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
	     }     
	
		return FALSE;
		}

		memset(&sCurrencyDatabaseStructure, 0, sizeof(sCurrencyDatabaseStructure)) ;
		if (!PopulateCurrencyCodeFromDialog (&sCurrencyDatabaseStructure)) {		
			return FALSE;
		}
		int rcode = txdsapi_insert_record ((pBYTE)&sCurrencyDatabaseStructure, sizeof (CRF01), CRF01_DATA, strErrorMessage);
		if (rcode != PTEMSG_OK)
		{   	
			m_StrMessageLbl = strErrorMessage;
			if (!UpdateData(FALSE)){
				AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
			}
			return FALSE;
		}
		else 
		{            
			memcpy ((LPSTR)m_strCurrentData, "Record Added", sizeof (m_strCurrentData));
			memset ( m_strPreviousData, 0, sizeof (m_strPreviousData));
		    memset ( m_strFieldTag, 0, sizeof (m_strFieldTag));
			PopulateHistoryTable();
			((CPropertySheet*) this->GetParent())->SetActivePage(0);
		
			m_StrMessageLbl = "Record Added";	
			m_CntlCurrencyCodeFld.SetFocus();
			m_CntlCurrencyCodeFld.EnableWindow(TRUE);
			if (!UpdateData(FALSE)){
				AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
			}
			pConversionRatePage->CleanConvertCodeDialog();
			pConversionRatePage->EnableConvertCodeDialog(FALSE);
			

			if ( pChangeHistoryPage->m_bChangeHistory )
				pChangeHistoryPage->CleanChangeHistory();
			SetModified ( FALSE );
			m_nDialogMode = MODE_DONE;
			pConversionRatePage->m_nDialogMode = MODE_DONE;
			//return CPropertyPage::OnApply();
         return FALSE;
		} 
	}
	
	//return CPropertyPage::OnApply();
   return FALSE;
}

BOOL CCurrencyCode::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	// TODO: Add your specialized code here and/or call the base class
	
	c_time = CTime::GetCurrentTime();	
	shrmem.SetLastUpdateTime(c_time);
	return CPropertyPage::OnCommand(wParam, lParam);
}
