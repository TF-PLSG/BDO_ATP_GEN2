// CB_Definition.cpp : implementation file
//

#include "stdafx.h"
#include "atp.h"
#include "CB_Definition.h"
#include "CB_General.h"
#include "ChangeHistory.h"
#include "NameList.h"

extern "C"
{
#include "PteTime.h"
}

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#include "SharedMem.h"

extern CSharedMem shrmem;
extern UCF01 ucf01;
extern CString name;

/////////////////////////////////////////////////////////////////////////////
// CCB_Definition property page

IMPLEMENT_DYNCREATE(CCB_Definition, CPropertyPage)

CCB_Definition::CCB_Definition() : CPropertyPage(CCB_Definition::IDD)
{
	//{{AFX_DATA_INIT(CCB_Definition)
	m_StrCashBonusId = _T("");
	m_StrMessageLbl = _T("");
	//}}AFX_DATA_INIT
}

CCB_Definition::~CCB_Definition()
{
}

void CCB_Definition::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCB_Definition)
	DDX_Control(pDX, IDC_CB_ID_FLD, m_CntlCashBonusId);
	DDX_Control(pDX, IDC_CB_VIEW_BUTTON, m_ViewButton);
	DDX_Control(pDX, IDC_CB_UPD_BUTTON, m_UpdateButton);
	DDX_Control(pDX, IDC_CB_DEL_BUTTON, m_DeleteButton);
	DDX_Control(pDX, IDC_CB_LIST_BUTTON, m_ListButton);
	DDX_Control(pDX, IDC_CB_ADD_BUTTON, m_AddButton);
	DDX_Text(pDX, IDC_CB_ID_FLD, m_StrCashBonusId);
	DDX_Text(pDX, IDC_MESSAGE_LBL, m_StrMessageLbl);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CCB_Definition, CPropertyPage)
	//{{AFX_MSG_MAP(CCB_Definition)
	ON_BN_CLICKED(IDC_CB_ADD_BUTTON, OnCbAddButton)
	ON_BN_CLICKED(IDC_CB_DEL_BUTTON, OnCbDelButton)
	ON_BN_CLICKED(IDC_CB_LIST_BUTTON, OnCbListButton)
	ON_BN_CLICKED(IDC_CB_UPD_BUTTON, OnCbUpdButton)
	ON_BN_CLICKED(IDC_CB_VIEW_BUTTON, OnCbViewButton)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCB_Definition message handlers

void CCB_Definition::OnCbAddButton() 
{
   BON02    sBon02;
   CMyUtils myUtilsObject;
   char     strErrorMessage [200]="";

   m_StrMessageLbl = "";

   m_nDialogMode = MODE_ADD;

   // Verify Cash Bonus ID field is not blank
   if (!CheckDialogForCompleteness())
      return ;

   // set the mode on each page to mode ADD
   pGeneralPage->m_nDialogMode       = MODE_ADD;
   pAmountsPage->m_nDialogMode       = MODE_ADD;
   pChangeHistoryPage->m_nDialogMode = MODE_ADD;

   // Copy the Cash Bonus ID data from control to BON02 table structure
   ZeroMemory (&sBon02, sizeof(BON02));
   memcpy ((LPSTR)(sBon02.cash_id), m_StrCashBonusId ,sizeof(sBon02.cash_id));

   // Verify Cash Bonus ID does not already exists in BON02 database 
   int rcode = txdsapi_get_record((pBYTE)&sBon02, sizeof(BON02), BON02_DATA, strErrorMessage);

   if(rcode == PTEMSG_OK )
   {
      m_StrMessageLbl = "ERROR: Cash Bonus ID already exists";
      m_nDialogMode = MODE_DONE;

      if (!UpdateData(FALSE))
      {
         AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);
      }
      return ;
   }

   pChangeHistoryPage->bNext = FALSE;      

   ((CPropertySheet*) this->GetParent())->SetWindowText("Add Cash Bonus Definition");
   //set version info in the title bar
	CString appProdVer;
	CString lpszTitle;
	appProdVer.LoadString(IDS_PRODUCT_VERSION);
	((CPropertySheet*) this->GetParent())->GetWindowText(lpszTitle);
	lpszTitle = lpszTitle + " - Version - ";
	lpszTitle += appProdVer;
	((CPropertySheet*) this->GetParent())->SetWindowText(lpszTitle);

   m_CntlCashBonusId.EnableWindow(FALSE);

   if ( pChangeHistoryPage->m_bChangeHistory)
   {  
      pChangeHistoryPage->UpdateData(FALSE); 
      pChangeHistoryPage->EnableWindow(TRUE);
      pChangeHistoryPage->CleanChangeHistory();
   }   

   // Enable controls on the General page
   if ( pGeneralPage->m_bGeneral )
   {
      pGeneralPage->CleanGeneralDialog();
      pGeneralPage->EnableGeneralDialog(TRUE);
   }

   // Enable controls on the Amounts page
   if ( pAmountsPage->m_bAmounts )
   {
      pAmountsPage->CleanAmountsDialog();
      pAmountsPage->EnableAmountsDialog(TRUE);
   }

   pGeneralPage->m_StrCashBonusId = m_StrCashBonusId;
   pAmountsPage->m_StrCashBonusId = m_StrCashBonusId;

   ((CPropertySheet*) this->GetParent())->SetActivePage(1);

   pGeneralPage->UpdateData(FALSE);
}

void CCB_Definition::OnCbDelButton() 
{
   BON02    sBon02;
   CMyUtils myUtilsObject;
   char     strErrorMessage [200]="";
   int      nReturnCode = 0;
   CString  strMessage; 

   m_nDialogMode = MODE_DONE;

	m_StrMessageLbl = "";

	if (!CheckDialogForCompleteness())
        return;   
   
    if (!UpdateData(TRUE))
    {
		AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);	
	}
    
	strMessage.Format ("Are you sure you want to delete Cash Bonus record %s?", (LPCTSTR) m_StrCashBonusId) ;	

	nReturnCode = AfxMessageBox ((LPCTSTR)strMessage,MB_OKCANCEL|MB_ICONQUESTION) ;

	if (nReturnCode != IDOK) 
        return ;

   // Copy the Cash Bonus ID data from control to Bon02 table structure
   memcpy ((LPSTR)(sBon02.cash_id), m_StrCashBonusId ,sizeof(sBon02.cash_id));

   int rcode = txdsapi_delete_record ((pBYTE)&sBon02, sizeof(BON02), BON02_DATA, strErrorMessage);

   if(rcode != PTEMSG_OK )    
   {
      m_StrMessageLbl = strErrorMessage;
      m_nDialogMode = MODE_DONE;
     
      if (!UpdateData(FALSE))
      {
         AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);
      }
      return;
   }
   else
   {
      memcpy ((LPSTR)m_strCurrentData, "Record Deleted", sizeof (m_strCurrentData));

      PopulateHistoryTable();
      m_StrMessageLbl  = "Record Deleted";
      m_StrCashBonusId = _T("");        

      if (!UpdateData(FALSE))
      {
         AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);
      }

      if ( pGeneralPage->m_bGeneral )
      {
         pGeneralPage->CleanGeneralDialog();
         pGeneralPage->EnableGeneralDialog(FALSE);
      }

      if ( pAmountsPage->m_bAmounts )
      {
         pAmountsPage->CleanAmountsDialog();
         pAmountsPage->EnableAmountsDialog(FALSE);
      }

      if ( pChangeHistoryPage->m_bChangeHistory )
         pChangeHistoryPage->CleanChangeHistory();

      ((CPropertySheet*) this->GetParent())->SetActivePage(0);
      SetModified ( FALSE );    

      m_CntlCashBonusId.EnableWindow(TRUE);
   } 
}

void CCB_Definition::OnCbListButton() 
{
   CNameList listDlg;   
   int nReturnCode;

   m_StrMessageLbl = "";
   UpdateData(TRUE);

   listDlg.m_nIndicator   = MODE_BON02;
   listDlg.strPrimaryKey1 = m_StrCashBonusId;

   nReturnCode = listDlg.DoModal();

   if ( nReturnCode == IDOK)
      m_StrCashBonusId = name;

   if ( pGeneralPage->m_bGeneral )	
   {
      pGeneralPage->CleanGeneralDialog();
      pGeneralPage->EnableGeneralDialog(FALSE);
   }

   if ( pAmountsPage->m_bAmounts )	
   {
      pAmountsPage->CleanAmountsDialog();
      pAmountsPage->EnableAmountsDialog(FALSE);
   }

   if ( pChangeHistoryPage->m_bChangeHistory )
      pChangeHistoryPage->CleanChangeHistory();

   m_CntlCashBonusId.EnableWindow(TRUE);

   if (!UpdateData(FALSE))
   {
      AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);
   }	
}

void CCB_Definition::OnCbUpdButton() 
{
   HST01	sHistoryRecord;
   BON02 sBon02;
   char  strErrorMessage[200];

   // set the mode on each page to mode ADD
   m_nDialogMode = MODE_EDIT;
   pGeneralPage->m_nDialogMode       = MODE_EDIT;
   pAmountsPage->m_nDialogMode       = MODE_EDIT;
   pChangeHistoryPage->m_nDialogMode = MODE_EDIT;
   m_StrMessageLbl = "";

   if (!CheckDialogForCompleteness())
      return;

   //copy the Name ID data from control to FG01 table structure
   memcpy( (LPSTR)(sBon02.cash_id), m_StrCashBonusId ,sizeof(sBon02.cash_id) );

   int rcode = txdsapi_get_record((pBYTE)&sBon02, sizeof(BON02), BON02_DATA, strErrorMessage);

   if (rcode != PTEMSG_OK)
   {
      m_StrMessageLbl = strErrorMessage;
      m_nDialogMode = MODE_DONE;

      if (!UpdateData(FALSE))
      {
         AfxMessageBox (IDS_UPDATEDATAFAILED, MB_ICONSTOP);
      }
      return;
   }

   ZeroMemory( &sHistoryRecord, sizeof(sHistoryRecord) );

   memset ( strFilePrimaryKey, 0, sizeof (strFilePrimaryKey));

   strcpy ( strFilePrimaryKey, m_StrCashBonusId);

   memcpy ((LPSTR)(sHistoryRecord.primary_key.unique_id),"                 ",sizeof(sHistoryRecord.primary_key.unique_id));
   memcpy ((LPSTR)(sHistoryRecord.file_name),"BON02", sizeof(sHistoryRecord.file_name));
   memcpy ((LPSTR)(sHistoryRecord.file_primary_key), strFilePrimaryKey,	sizeof (sHistoryRecord.file_primary_key));
   memcpy ( &(pChangeHistoryPage->sHistoryRecord),&sHistoryRecord,sizeof(sHistoryRecord));

   ((CPropertySheet*) this->GetParent())->SetActivePage(1);       

   pChangeHistoryPage->bNext = FALSE;

   ((CPropertySheet*) this->GetParent())->SetWindowText("Update Cash Bonus Record");
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

   // Enable controls on the General page
   if ( pGeneralPage->m_bGeneral )
   {
      pGeneralPage->CleanGeneralDialog();
      pGeneralPage->EnableGeneralDialog(TRUE);
   }

   // Enable controls on the Amounts page
   if ( pAmountsPage->m_bAmounts )
   {
      pAmountsPage->CleanAmountsDialog();
      pAmountsPage->EnableAmountsDialog(TRUE);
   }

   pGeneralPage->PopulateBon02DialogFromDatabase( sBon02 );
   pAmountsPage->PopulateBon02DialogFromDatabase( sBon02, 0 );

   m_CntlCashBonusId.EnableWindow(FALSE);

   pGeneralPage->UpdateData(FALSE); 
   pGeneralPage->SetModified(TRUE);

   ((CPropertySheet*) this->GetParent())->SetActivePage(1); 
}

void CCB_Definition::OnCbViewButton() 
{
   BON02  sBon02;
   HST01  sHistoryRecord;
   char   strErrorMessage[200];

   m_nDialogMode = MODE_VIEW;
   pGeneralPage->m_nDialogMode       = MODE_VIEW;
   pAmountsPage->m_nDialogMode       = MODE_VIEW;
   pChangeHistoryPage->m_nDialogMode = MODE_VIEW;
   m_StrMessageLbl = "";

   if (!CheckDialogForCompleteness())
      return;

   //copy the Name ID data from control to FG01 table structure
   memcpy( (LPSTR)(sBon02.cash_id), m_StrCashBonusId ,sizeof(sBon02.cash_id) );

   int rcode = txdsapi_get_record((pBYTE)&sBon02, sizeof(BON02), BON02_DATA, strErrorMessage);

   if (rcode != PTEMSG_OK)
   {
      m_StrMessageLbl = strErrorMessage;
      m_nDialogMode = MODE_DONE;

      if (!UpdateData(FALSE))
      {
         AfxMessageBox (IDS_UPDATEDATAFAILED, MB_ICONSTOP);
      }
      return;
   }

   ZeroMemory( &sHistoryRecord, sizeof(sHistoryRecord) );

   memset ( strFilePrimaryKey, 0, sizeof (strFilePrimaryKey));

   strcpy ( strFilePrimaryKey, m_StrCashBonusId);

   memcpy ((LPSTR)(sHistoryRecord.primary_key.unique_id),"                 ",sizeof(sHistoryRecord.primary_key.unique_id));
   memcpy ((LPSTR)(sHistoryRecord.file_name),"BON02", sizeof(sHistoryRecord.file_name));
   memcpy ((LPSTR)(sHistoryRecord.file_primary_key), strFilePrimaryKey,	sizeof (sHistoryRecord.file_primary_key));
   memcpy ( &(pChangeHistoryPage->sHistoryRecord),&sHistoryRecord,sizeof(sHistoryRecord));

   ((CPropertySheet*) this->GetParent())->SetActivePage(1);       

   pChangeHistoryPage->bNext = FALSE;

	((CPropertySheet*) this->GetParent())->SetWindowText("View Cash Bonus Definition");
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

   // Enable controls on the General page
   if ( pGeneralPage->m_bGeneral )
   {
      pGeneralPage->CleanGeneralDialog();
      pGeneralPage->EnableGeneralDialog(FALSE);
   }

   // Enable controls on the Amounts page
   if ( pAmountsPage->m_bAmounts )
   {
      pAmountsPage->CleanAmountsDialog();
      pAmountsPage->EnableAmountsDialog(FALSE);
   }

   pGeneralPage->PopulateBon02DialogFromDatabase( sBon02 );
   pAmountsPage->PopulateBon02DialogFromDatabase( sBon02, 0 );

   m_CntlCashBonusId.EnableWindow(FALSE);

   pGeneralPage->UpdateData(FALSE); 
   pGeneralPage->SetModified(FALSE);

   ((CPropertySheet*) this->GetParent())->SetActivePage(1); 
}

BOOL CCB_Definition::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
	CFont m_font;
	LOGFONT lf;
	CString temp;
	char strErrorMessage[200] = "";


	memset ( &lf, 0, sizeof (lf));
	lf.lfHeight = 12;
	lf.lfWeight = 700;
	strcpy(lf.lfFaceName, "Arial");
	m_font.CreateFontIndirect (&lf);
	GetDlgItem(IDC_MESSAGE_LBL)->SetFont (&m_font);

	// Clean all strings
   m_StrCashBonusId = _T("");
   m_StrMessageLbl = _T("");


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
      if ( ucf01.control[nIndex].action[2] == '0' )
         m_UpdateButton.EnableWindow(FALSE);
      if ( ucf01.control[nIndex].action[3] == '0' )
         m_ViewButton.EnableWindow(FALSE);
   }

   m_CntlCashBonusId.SetFocus();
   UpdateData(FALSE);  

   m_nDialogMode = MODE_DONE;	

	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CCB_Definition::CheckDialogForCompleteness()
{
   BOOL     retval = TRUE;
   CMyUtils myUtilsObject;

   if (!UpdateData(TRUE))
   {
      AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);
   }

   if (myUtilsObject.IsEmpty (m_StrCashBonusId, this))
   {
      m_StrMessageLbl = "Please enter Cash Bonus Program ID";	
      m_CntlCashBonusId.SetFocus();

      if (!UpdateData(FALSE))
      {
         AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);
      }
      m_nDialogMode = MODE_DONE;
      retval = FALSE;
   }
   return( retval );
}



/******************************************************************************
 *
 * MODULE:      OnApply
 *
 * DESCRIPTION: This function checks for all required fields, saves the data,
 *              cleans pages.
 *
 * AUTHOR:      D. Irby
 *
 * INPUTS:      m_nDialogMode             
 *
 * OUTPUTS:     None
 *
 * RTRN VALUE:  Always return FALSE to avoid exiting the window on OK
 *
 ******************************************************************************/
BOOL CCB_Definition::OnApply() 
{
   BON02    sBon02Struture;
   char     strErrorMessage[200]="";
   CMyUtils myUtilsObject;
   CString  buttonlabel;
   CWnd*    activebutton;

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

   if (m_nDialogMode != MODE_DONE)
   {
      if ( myUtilsObject.IsEmpty (pGeneralPage->m_StrCashBonusId, this)) 
      {
         ((CPropertySheet*) this->GetParent())->SetActivePage(1);
         pGeneralPage->m_CntlCashBonusId.SetFocus();
         pGeneralPage->m_StrMessageLbl = "Please enter Cash Bonus ID";
         pGeneralPage->UpdateData(FALSE);
         return FALSE;
      }
	}

   if (m_nDialogMode == MODE_EDIT)
   {
      memset( &sBon02Struture, 0x00, sizeof(sBon02Struture) );

      pGeneralPage->PopulateDatabaseFromBon02Dialog( &sBon02Struture );
      pAmountsPage->PopulateDatabaseFromBon02Dialog( &sBon02Struture );

      int rcode = txdsapi_update_record((pBYTE)&sBon02Struture, sizeof(BON02), BON02_DATA, strErrorMessage);
      if (rcode != PTEMSG_OK)
      {
         m_StrMessageLbl = strErrorMessage;
         m_nDialogMode   = MODE_DONE;

         ((CPropertySheet*) this->GetParent())->SetActivePage(0);
         ((CPropertySheet*) this->GetParent())->SetWindowText(" Cash Bonus Definition");
		 //set version info in the title bar
		 CString appProdVer;
		 CString lpszTitle;
		 appProdVer.LoadString(IDS_PRODUCT_VERSION);
		 ((CPropertySheet*) this->GetParent())->GetWindowText(lpszTitle);
		 lpszTitle = lpszTitle + " - Version - ";
		 lpszTitle += appProdVer;
		 ((CPropertySheet*) this->GetParent())->SetWindowText(lpszTitle);

         m_CntlCashBonusId.EnableWindow(TRUE);

         if (!UpdateData(FALSE))
         {
            AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);
         }
         return FALSE;
      }
      else
      {
         m_StrMessageLbl = "Record Updated";
         if (!UpdateData(FALSE))
         {
            AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);
         }

         pGeneralPage->CompareStructures();
         if ( pGeneralPage->m_bGeneral )
         {
            pGeneralPage->CleanGeneralDialog();
            pGeneralPage->EnableGeneralDialog(FALSE);
         }

         pAmountsPage->CompareStructures();
         if ( pAmountsPage->m_bAmounts )
         {
            pAmountsPage->CleanAmountsDialog();
            pAmountsPage->EnableAmountsDialog(FALSE);
         }

         if ( pChangeHistoryPage->m_bChangeHistory )
            pChangeHistoryPage->CleanChangeHistory();

         pChangeHistoryPage->m_nDialogMode = MODE_DONE;
         pGeneralPage->m_nDialogMode       = MODE_DONE;
         pAmountsPage->m_nDialogMode       = MODE_DONE;
 
         ((CPropertySheet*) this->GetParent())->SetActivePage(0);
         ((CPropertySheet*) this->GetParent())->SetWindowText("Cash Bonus Definition");	
		 //set version info in the title bar
		 CString appProdVer;
		 CString lpszTitle;
		 appProdVer.LoadString(IDS_PRODUCT_VERSION);
		 ((CPropertySheet*) this->GetParent())->GetWindowText(lpszTitle);
		 lpszTitle = lpszTitle + " - Version - ";
		 lpszTitle += appProdVer;
		 ((CPropertySheet*) this->GetParent())->SetWindowText(lpszTitle);

         m_CntlCashBonusId.EnableWindow(TRUE);
         SetModified ( FALSE );
         m_nDialogMode = MODE_DONE;
         return TRUE;
      }
   }

   if (m_nDialogMode == MODE_ADD)
   {
      memset(&sBon02Struture,0,sizeof(sBon02Struture) );

      pGeneralPage->PopulateDatabaseFromBon02Dialog( &sBon02Struture );
      pAmountsPage->PopulateDatabaseFromBon02Dialog( &sBon02Struture );

      int rcode = txdsapi_insert_record((pBYTE)&sBon02Struture, sizeof(BON02), BON02_DATA, strErrorMessage);

      if (rcode != PTEMSG_OK)
      {
         m_StrMessageLbl = strErrorMessage;
         m_nDialogMode = MODE_DONE;

         if (!UpdateData(FALSE))
         {
            AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);
         }

         ((CPropertySheet*) this->GetParent())->SetActivePage(0);
         return FALSE;
      }
      else 
      {
         memcpy ((LPSTR)m_strCurrentData,"Record Added",sizeof (m_strCurrentData));

         PopulateHistoryTable();

         ((CPropertySheet*) this->GetParent())->SetActivePage(0);

         m_StrMessageLbl = "Record Added";	

         if (!UpdateData(FALSE))
         {
            AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);
         }

         if ( pGeneralPage->m_bGeneral )
         {
            pGeneralPage->CleanGeneralDialog();
            pGeneralPage->EnableGeneralDialog(FALSE);
         }

         if ( pChangeHistoryPage->m_bChangeHistory )
            pChangeHistoryPage->CleanChangeHistory();

         pChangeHistoryPage->m_nDialogMode = MODE_DONE;
         pGeneralPage->m_nDialogMode       = MODE_DONE;
         pAmountsPage->m_nDialogMode       = MODE_DONE;

         m_CntlCashBonusId.SetFocus();
			m_CntlCashBonusId.EnableWindow(TRUE);

         SetModified ( FALSE );
         m_nDialogMode = MODE_DONE;
         return FALSE;
      }
   }
   m_nDialogMode = MODE_DONE;		

   return CPropertyPage::OnApply();
}


void CCB_Definition::PopulateHistoryTable()
{
   HST01    sHistoryData;
   CHAR     dest_str[18];   
   CMyUtils myUtilsObject;
   CHAR     strErrorMsg [200] = "";
   CString  temp;

   CHAR strDate[9] = "";
   CHAR strTime[7] = "";
   CHAR strPrimaryKey[50] = "";
   CHAR wstation[21]; DWORD wsize;

   memset ( strDate, 0, sizeof (strDate));
   memset ( strTime, 0, sizeof (strTime));
   memset ( strPrimaryKey, 0, sizeof (strPrimaryKey));

   strcpy ( strPrimaryKey, m_StrCashBonusId);
    
   myUtilsObject.GetSystemDate(strDate); 
   myUtilsObject.GetSystemTime(strTime);

   pintime_get_gmt ( dest_str );

   ZeroMemory ( &sHistoryData, sizeof (HST01));
   memcpy ((LPTSTR) sHistoryData.file_name, (LPCTSTR)"BON02", sizeof(sHistoryData.file_name));
   temp = strDate;
   temp = temp.Right(4) + temp.Left(2) + temp.Mid(2,2);

   wsize = sizeof( wstation );
   GetComputerName( wstation, &wsize );

   memcpy ((LPTSTR) sHistoryData.reserved,(LPTSTR)wstation, sizeof(sHistoryData.reserved));
   memcpy ((LPTSTR) sHistoryData.date_modified, temp, sizeof (sHistoryData.date_modified));
   memcpy ((LPTSTR) sHistoryData.file_primary_key,  (LPCTSTR)strPrimaryKey, sizeof (sHistoryData.file_primary_key));
   memcpy ((LPTSTR) sHistoryData.primary_key.unique_id,     (LPCTSTR)dest_str, sizeof (sHistoryData.primary_key.unique_id));        
   memcpy ((LPTSTR) sHistoryData.time_modified,     (LPCTSTR)strTime, sizeof (sHistoryData.time_modified)); 

   memcpy ((LPTSTR)sHistoryData.current_data, (LPCTSTR)m_strCurrentData, sizeof (sHistoryData.current_data));
   memcpy ((LPTSTR)sHistoryData.changed_by, (LPCTSTR)ucf01.primary_key.name, sizeof (sHistoryData.changed_by));

   int rcode = txdsapi_insert_record( (pBYTE)&sHistoryData, sizeof (HST01), HST01_DATA, strErrorMsg );
   if ( rcode != PTEMSG_OK )
   {
      m_StrMessageLbl = strErrorMsg;
      m_nDialogMode = MODE_DONE;
      UpdateData( FALSE );
   }
}

BOOL CCB_Definition::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	// TODO: Add your specialized code here and/or call the base class
	
	c_time = CTime::GetCurrentTime();	
	shrmem.SetLastUpdateTime(c_time);
	return CPropertyPage::OnCommand(wParam, lParam);
}
