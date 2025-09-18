// CardsFleetDefinition.cpp : implementation file
//

#include "stdafx.h"
#include "Atp.H"
#include "RulesDefinitionMain.h"
#include "RulesDefinitionProcessing.h"
#include "RulesExceptionList.h"
#include "RulesDefinitionpropertySheet.h"
#include "MyUtils.h"
#include "BinList.h"
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
extern CString name;  
extern CString StrRule_ID;

//////////////////////////////////////////////////////////
// CRuleDefinitionMain property page

IMPLEMENT_DYNCREATE(CRuleDefinitionMain, CPropertyPage)

CRuleDefinitionMain::CRuleDefinitionMain() : CPropertyPage(CRuleDefinitionMain::IDD)
{
	//{{AFX_DATA_INIT(CRuleDefinitionMain)
	m_StrMessageLbl = _T("");
	m_StrRuleID_Fld = _T("");

	//}}AFX_DATA_INIT
}

CRuleDefinitionMain::~CRuleDefinitionMain()
{
}


void CRuleDefinitionMain::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CRuleDefinitionMain)
	DDX_Control(pDX, IDC_VIEW_CR01_BUTTON, m_ViewButton);
	DDX_Control(pDX, IDC_UPDATE_CR01_BUTTON, m_UpdateButton);
	DDX_Control(pDX, IDC_LIST_CR01_BUTTON, m_ListButton);
	DDX_Control(pDX, IDC_DELETE_CR01_BUTTON, m_DeleteButton);
	DDX_Control(pDX, IDC_ADD_CR01_BUTTON, m_AddButton);
	DDX_Control(pDX, IDC_RULE_ID_FLD, m_CntlRuleID);
	DDX_Text(pDX, IDC_MESSAGE_LBL, m_StrMessageLbl);
	DDX_Text(pDX, IDC_RULE_ID_FLD, m_StrRuleID_Fld);
	DDV_MaxChars(pDX, m_StrRuleID_Fld, 10);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CRuleDefinitionMain, CPropertyPage)
//{{AFX_MSG_MAP(CRuleDefinitionMain)
ON_BN_CLICKED(IDC_ADD_CR01_BUTTON, OnRulesAddButton)
ON_BN_CLICKED(IDC_DELETE_CR01_BUTTON, OnRulesDeleteButton)
ON_BN_CLICKED(IDC_LIST_CR01_BUTTON, OnRulesListButton)
ON_BN_CLICKED(IDC_UPDATE_CR01_BUTTON, OnRulesUpdateButton)
ON_BN_CLICKED(IDC_VIEW_CR01_BUTTON, OnRulesViewButton)
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////
// CRuleDefinitionMain message handlers

/************************************************************/
////////////////////////////////////////////////////////////
/************************************************************/

BOOL CRuleDefinitionMain::OnInitDialog() 
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

	m_StrRuleID_Fld = _T("");
    m_StrMessageLbl = _T("");
	UpdateData(FALSE);
	m_nDialogMode = MODE_DONE;

	CString temp;
	int nIndex = -1;

	for ( int i = 0; i <=20; i++ )
	{
		temp = ucf01.control[i].subsystem_name;
		if ( strcmp ( temp, "ChipFallBack Rules Definition" ) == 0)
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

	return TRUE; 
	
}

/************************************************************/
////////////////////////////////////////////////////////////
/************************************************************/

void CRuleDefinitionMain::OnRulesAddButton() 
{
		CR01 sRuleId;    
   CMyUtils myUtilsObject;
	char strErrorMessage[200];


	m_StrMessageLbl = "";

	m_nDialogMode = MODE_ADD;  
	pRulesProcessingPage->m_nDialogMode = MODE_ADD;
	pCardNumBlockingPage->m_nDialogMode = MODE_ADD;
    pChangeHistoryPage->m_nDialogMode = MODE_ADD;

	if (!CheckDialogForCompleteness())
        return;


	ZeroMemory(&sRuleId,sizeof(CR01));

	
    memcpy ((LPSTR)(sRuleId.rule_id),m_StrRuleID_Fld ,sizeof (sRuleId.rule_id));


	
    int rcode = txdsapi_get_record ( (pBYTE)&sRuleId, sizeof (CR01), CR01_DATA, strErrorMessage );
    if(rcode == PTEMSG_OK )
    {
		m_StrMessageLbl = "ERROR: Rule ID already exists";
        if (!UpdateData(FALSE))
        {
			AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
		}
		 m_nDialogMode = MODE_DONE;
		return ;
	}
	rcode = txdsapi_get_record_bin_range ( (pBYTE)&sRuleId, sizeof (CR01), CR01_DATA, strErrorMessage );
	    if(rcode == PTEMSG_OK )
    {
		m_StrMessageLbl = "ERROR: Rule ID with the Same Bin Range exists. Please Try with other bin range";
        if (!UpdateData(FALSE))
        {
			AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
		}
		 m_nDialogMode = MODE_DONE;
		return ;
	}

		if ( pRulesProcessingPage->m_bProcessing)
    {
		pRulesProcessingPage->CleanProcessingDialog(); 
		pRulesProcessingPage->EnableProcessingDialog(TRUE);
    }

		if ( pCardNumBlockingPage->m_bExceptionCardNum)
    {
		pCardNumBlockingPage->CleanCardBlockingDialog(); 

    }
			
    pRulesProcessingPage->m_StrRuleID_Fld = m_StrRuleID_Fld;
	pCardNumBlockingPage->m_StrRuleID_Fld = m_StrRuleID_Fld;
	   
	char buffer[20];
	int accumulated_txn_counter;
	int accumulated_amt_counter;

	accumulated_txn_counter=0;
	accumulated_amt_counter=0;

	itoa(accumulated_txn_counter,buffer,10);
	pRulesProcessingPage->m_StrAccTransactionLimitCounter_Fld.Format((LPCTSTR)buffer);
	itoa(accumulated_amt_counter,buffer,10);
	pRulesProcessingPage->m_StrAccAmtLimitCounter_Fld.Format((LPCTSTR)buffer);
	/*gcvt(accumulated_amt_counter/100,10,buffer);
	pRulesProcessingPage->m_StrAccAmtLimitCounter_Fld.Format((LPCTSTR)buffer);*/
	           
   ((CPropertySheet*) this->GetParent())->SetWindowText("Add RULE");
   //set version info in the title bar
	CString appProdVer;
	CString lpszTitle;
	appProdVer.LoadString(IDS_PRODUCT_VERSION);
	((CPropertySheet*) this->GetParent())->GetWindowText(lpszTitle);
	lpszTitle = lpszTitle + " - Version - ";
	lpszTitle += appProdVer;
	((CPropertySheet*) this->GetParent())->SetWindowText(lpszTitle);

	
	if ( pRulesProcessingPage->m_bProcessing)
    {
		pRulesProcessingPage->UpdateData(FALSE);
		pRulesProcessingPage->EnableProcessingDialog(TRUE);
    }

	if ( pCardNumBlockingPage->m_bExceptionCardNum)
    {
		pCardNumBlockingPage->UpdateData(FALSE); 

    }	
	
	((CPropertySheet*) this->GetParent())->SetActivePage(1);
	    m_CntlRuleID.EnableWindow(FALSE);
	
}
/************************************************************/
////////////////////////////////////////////////////////////
/************************************************************/

void CRuleDefinitionMain::OnRulesDeleteButton() 
{
	CString strMessage;
    int nReturnCode;
    CR01 sRuleId;

	
	char strErrorMessage[200];
	
    m_nDialogMode = MODE_EDIT;
	
	m_StrMessageLbl = "";
	
	if (!CheckDialogForCompleteness())
        return;   
	
    if (!UpdateData(TRUE))
	{
		AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);	
	}
    
	strMessage.Format ("Are you sure you want to delete Rule ID %s?", (LPCTSTR) m_StrRuleID_Fld) ;	
	
	nReturnCode = AfxMessageBox ((LPCTSTR)strMessage,MB_OKCANCEL|MB_ICONQUESTION) ;
	
	if (nReturnCode != IDOK) 
        return ;
	
	memcpy ((LPTSTR)sRuleId.rule_id,m_StrRuleID_Fld,sizeof (sRuleId.rule_id)) ;
	   
    
    int rcode = txdsapi_delete_record ((pBYTE)&sRuleId,
        sizeof (CR01), CR01_DATA, strErrorMessage);
	
    if(rcode != PTEMSG_OK )    
	{
		m_StrMessageLbl = strErrorMessage;
        m_nDialogMode = MODE_DONE;
		
		if (!UpdateData(FALSE))
		{
			AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
		}
		return ;
	}
	else
	{		
        memcpy ((LPSTR)m_strCurrentData,"Record Deleted", sizeof (m_strCurrentData));
		
        PopulateHistoryTable();
		
		m_StrRuleID_Fld = _T(""); 
		m_StrMessageLbl = "Record Deleted";
		
		if (!UpdateData(FALSE))
		{
			AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
		}
		
		if ( pRulesProcessingPage->m_bProcessing )
		{
			pRulesProcessingPage->CleanProcessingDialog();
			pRulesProcessingPage->EnableProcessingDialog(FALSE);
		}

		if ( pCardNumBlockingPage->m_bExceptionCardNum)
        {
		   pCardNumBlockingPage->CleanCardBlockingDialog(); 
		   pCardNumBlockingPage->UpdateData(FALSE); 
		   //pCardNumBlockingPage->EnableProcessingDialog(TRUE);
        }
		
		if ( pChangeHistoryPage->m_bChangeHistory )
			pChangeHistoryPage->CleanChangeHistory();
		
		m_nDialogMode = MODE_DONE;
		
		((CPropertySheet*) this->GetParent())->SetActivePage(0);
		SetModified ( FALSE );    
	} 
	
	m_CntlRuleID.EnableWindow(TRUE);
	
	
}


/************************************************************/
////////////////////////////////////////////////////////////
/************************************************************/

void CRuleDefinitionMain::OnRulesListButton() 
{
	CBinList listDlg;
    int nReturnCode;


	m_StrMessageLbl = "";

		UpdateData(TRUE);
	// set dialog Mode for Bin List
	listDlg.strItem0 = m_StrRuleID_Fld;
	listDlg.m_nDialogMode = MODE_RULEID;
	
    nReturnCode = listDlg.DoModal();
	 if ( nReturnCode == IDOK)
	{
		m_StrRuleID_Fld = StrRule_ID;
	}

    if (pRulesProcessingPage->m_bProcessing)
	{
		pRulesProcessingPage->CleanProcessingDialog();
		pRulesProcessingPage->EnableProcessingDialog(FALSE);	
	}

	if ( pCardNumBlockingPage->m_bExceptionCardNum)
     {
		   pCardNumBlockingPage->CleanCardBlockingDialog(); 
		   pCardNumBlockingPage->UpdateData(FALSE); 
		   //pCardNumBlockingPage->EnableProcessingDialog(TRUE);
     }

	m_CntlRuleID.EnableWindow(TRUE);
    if (!UpdateData(FALSE))
    {
		AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
	}		
	
}

/************************************************************/
////////////////////////////////////////////////////////////
/************************************************************/

void CRuleDefinitionMain::OnRulesUpdateButton() 
{    
    HST01 sHistoryRecord;
	char strErrorMessage[200]={0};
    
   m_nDialogMode = MODE_EDIT; 
	pRulesProcessingPage->m_nDialogMode = MODE_EDIT;
	pCardNumBlockingPage->m_nDialogMode = MODE_EDIT;
	 pChangeHistoryPage->m_nDialogMode = MODE_EDIT;

	m_StrMessageLbl = "";
	
	if (!CheckDialogForCompleteness())
        return;

	memset(&sRuleIdOld,0,sizeof(sRuleIdOld));
	
    memcpy ((LPSTR)(sRuleIdOld.rule_id),
		      m_StrRuleID_Fld,sizeof (sRuleIdOld.rule_id) );
			
        int  rcode = txdsapi_get_record  ( (pBYTE)&sRuleIdOld,
			sizeof (CR01), CR01_DATA, strErrorMessage );
		if (rcode != PTEMSG_OK)
		{        
			m_StrMessageLbl = "ERROR: Rule ID does not exist";
			m_nDialogMode = MODE_DONE;
			if (!UpdateData(FALSE))
			{
				AfxMessageBox (IDS_UPDATEDATAFAILED,
					MB_ICONSTOP);		
			}
			return;
		}


    pRulesProcessingPage->m_StrRuleID_Fld = m_StrRuleID_Fld;
	pCardNumBlockingPage->m_StrRuleID_Fld = m_StrRuleID_Fld;

    ZeroMemory (&sHistoryRecord, sizeof (sHistoryRecord));
	
    memset ( strFilePrimaryKey, 0,sizeof (strFilePrimaryKey));
    strcpy ( strFilePrimaryKey, (char *)sRuleIdOld.rule_id);	

    memcpy ((LPSTR)(sHistoryRecord.primary_key.unique_id),"                 ",sizeof (sHistoryRecord.primary_key.unique_id));
    memcpy ((LPSTR)(sHistoryRecord.file_name),"CR01",sizeof (sHistoryRecord.file_name));
    memcpy ((LPSTR)(sHistoryRecord.file_primary_key),strFilePrimaryKey,sizeof (sHistoryRecord.file_primary_key));

    memcpy ( &(pChangeHistoryPage->sHistoryRecord),&sHistoryRecord,sizeof(sHistoryRecord));

	((CPropertySheet*) this->GetParent())->SetActivePage(1);        
    
	pChangeHistoryPage->bNext = FALSE;
	
	((CPropertySheet*) this->GetParent())->SetWindowText("Update Rule ID");	
	//set version info in the title bar
	CString appProdVer;
	CString lpszTitle;
	appProdVer.LoadString(IDS_PRODUCT_VERSION);
	((CPropertySheet*) this->GetParent())->GetWindowText(lpszTitle);
	lpszTitle = lpszTitle + " - Version - ";
	lpszTitle += appProdVer;
	((CPropertySheet*) this->GetParent())->SetWindowText(lpszTitle);
    
   
	pRulesProcessingPage->PopulateRulesProcessingDialogFromDatabase(sRuleIdOld);   


	if ( pCardNumBlockingPage->m_bExceptionCardNum)
     {
		   pCardNumBlockingPage->PopulateCardBlockingDialogFromDatabase(sRuleIdOld);

     }
	else
	{
		 memcpy ( &pCardNumBlockingPage->sCR01Record, &sRuleIdOld, sizeof ( CR01));
	}

    if ( pChangeHistoryPage->m_bChangeHistory)
        pChangeHistoryPage->PopulateHistoryDialog();    
	
	
	m_CntlRuleID.EnableWindow(FALSE);
    
}

/************************************************************/
////////////////////////////////////////////////////////////
/************************************************************/

void CRuleDefinitionMain::OnRulesViewButton() 
{

	CR01 sCR01Record;
	HST01 sHistoryRecord;

	char strErrorMessage[200];
	char strFilePrimaryKey[30];

	m_nDialogMode = MODE_VIEW;
	pRulesProcessingPage->m_nDialogMode = MODE_VIEW;
	pCardNumBlockingPage->m_nDialogMode= MODE_VIEW;
    pChangeHistoryPage->m_nDialogMode = MODE_VIEW;
	m_StrMessageLbl = "";

	if (!CheckDialogForCompleteness())
	return;


	memset(&sCR01Record,0,sizeof(sCR01Record));
	
    memcpy ((LPSTR)(sCR01Record.rule_id),
		      m_StrRuleID_Fld,sizeof (sCR01Record.rule_id) );
		
		int  rcode = txdsapi_get_record  ( (pBYTE)&sCR01Record,
			sizeof (CR01), CR01_DATA, strErrorMessage );
		if (rcode != PTEMSG_OK)
		{        
			m_StrMessageLbl = "ERROR: Rule ID does not exist";
			m_nDialogMode = MODE_DONE;
			
			if (!UpdateData(FALSE))
			{
				AfxMessageBox (IDS_UPDATEDATAFAILED,
					MB_ICONSTOP);		
			}
			return;
		}


	pRulesProcessingPage->m_StrRuleID_Fld = m_StrRuleID_Fld;
	pCardNumBlockingPage->m_StrRuleID_Fld = m_StrRuleID_Fld;

	ZeroMemory (&sHistoryRecord, sizeof (sHistoryRecord));

	memset ( strFilePrimaryKey, 0,sizeof (strFilePrimaryKey));
	strcpy ( strFilePrimaryKey, (char *)sCR01Record.rule_id);

	memcpy ((LPSTR)(sHistoryRecord.primary_key.unique_id),"                 ",sizeof (sHistoryRecord.primary_key.unique_id));
	memcpy ((LPSTR)(sHistoryRecord.file_name),"CR01",sizeof (sHistoryRecord.file_name));
	memcpy ((LPSTR)(sHistoryRecord.file_primary_key),strFilePrimaryKey,sizeof (sHistoryRecord.file_primary_key));
	memcpy ( &(pChangeHistoryPage->sHistoryRecord),&sHistoryRecord,sizeof(sHistoryRecord));

	((CPropertySheet*) this->GetParent())->SetActivePage(1);            

	pChangeHistoryPage->bNext = FALSE;

	((CPropertySheet*) this->GetParent())->SetWindowText("View RULE ID");	
	//set version info in the title bar
	CString appProdVer;
	CString lpszTitle;
	appProdVer.LoadString(IDS_PRODUCT_VERSION);
	((CPropertySheet*) this->GetParent())->GetWindowText(lpszTitle);
	lpszTitle = lpszTitle + " - Version - ";
	lpszTitle += appProdVer;
	((CPropertySheet*) this->GetParent())->SetWindowText(lpszTitle);

   
		pRulesProcessingPage->PopulateRulesProcessingDialogFromDatabase(sCR01Record);   
 

	if ( pCardNumBlockingPage->m_bExceptionCardNum)
     {
		   pCardNumBlockingPage->PopulateCardBlockingDialogFromDatabase(sRuleIdOld);

     }
	else
	{
		 memcpy ( &pCardNumBlockingPage->sCR01Record, &sCR01Record, sizeof ( CR01));
	}

    if ( pChangeHistoryPage->m_bChangeHistory)
        pChangeHistoryPage->PopulateHistoryDialog();    
	
	
	m_CntlRuleID.EnableWindow(FALSE);  	


}

/************************************************************/
////////////////////////////////////////////////////////////
/************************************************************/

BOOL CRuleDefinitionMain::OnApply() 
{

	CMyUtils myUtilsObject;
    CR01 sCR01DatabaseStructure;
	char strErrorMessage[200]={0};	
	CHAR     tempstr[10]={0};



	if (m_nDialogMode == MODE_ADD)
    {

        ((CPropertySheet*) this->GetParent())->SetActivePage(1);
		if ( !pRulesProcessingPage->CheckDialogForCompleteness())
		{
		//	((CPropertySheet*) this->GetParent())->SetActivePage(1);
			return FALSE;
		}   

		memset(&sCR01DatabaseStructure,0,sizeof(sCR01DatabaseStructure));
		
        if (!(PopulateDatabaseStructureFromRuleIdDialog(&sCR01DatabaseStructure)))
        {		
			return FALSE;
		}

		int rcode = txdsapi_insert_record((pBYTE)&sCR01DatabaseStructure,
			sizeof (CR01), CR01_DATA, strErrorMessage);
		if (rcode != PTEMSG_OK)
		{
			m_StrMessageLbl = strErrorMessage;
			if (!UpdateData(FALSE)){
				AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
			}
			((CPropertySheet*) this->GetParent())->SetActivePage(0);
			return FALSE;
		}
		else
		{
			memcpy ((LPSTR)m_strCurrentData, "Record Added", sizeof (m_strCurrentData));
			memset ( m_strPreviousData, 0, sizeof (m_strPreviousData));
			memset ( m_strFieldTag, 0, sizeof (m_strPreviousData));
			PopulateHistoryTable();
			((CPropertySheet*) this->GetParent())->SetActivePage(0);
			m_StrMessageLbl = "Record Added";	
			m_CntlRuleID.SetFocus();
			if (!UpdateData(FALSE))
			{
				AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
			}

			pRulesProcessingPage->CleanProcessingDialog();
			pRulesProcessingPage->EnableProcessingDialog(FALSE);
			pRulesProcessingPage->CheckDialogForCompleteness();

			if ( pCardNumBlockingPage->m_bExceptionCardNum)
			{
				pCardNumBlockingPage->CleanCardBlockingDialog(); 
				pCardNumBlockingPage->m_nDialogMode= MODE_DONE;

			}

						 if ( pChangeHistoryPage->m_bChangeHistory )
				pChangeHistoryPage->CleanChangeHistory();

               	SetModified ( FALSE );
				m_nDialogMode = MODE_DONE;
				m_CntlRuleID.EnableWindow(TRUE);

				//return CPropertyPage::OnApply();
			 return FALSE;
		}
    }

	if (m_nDialogMode == MODE_EDIT)
	{ 

        ((CPropertySheet*) this->GetParent())->SetActivePage(1);
		if ( !pRulesProcessingPage->CheckDialogForCompleteness())
		{
		//	((CPropertySheet*) this->GetParent())->SetActivePage(1);
			return FALSE;
		} 

		CR01 sCR01DatabaseStructure;
		memset(&sCR01DatabaseStructure, 0, sizeof(sCR01DatabaseStructure));
		memcpy(&sCR01DatabaseStructure,&sRuleIdOld, sizeof(sCR01DatabaseStructure));
		
        PopulateDatabaseStructureFromRuleIdDialog(&sCR01DatabaseStructure);
        

        int rcode = txdsapi_update_record  ((pBYTE)&sCR01DatabaseStructure,
			sizeof (CR01), CR01_DATA, strErrorMessage);
		if (rcode != PTEMSG_OK)
		{    	
			m_StrMessageLbl = strErrorMessage;
            ((CPropertySheet*) this->GetParent())->SetActivePage(0);
			m_nDialogMode = MODE_DONE;
			if (!UpdateData(FALSE)){
				AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
			}
			return FALSE;
			

		}        
        else
		{     
			pRulesProcessingPage->CompareStructures(sRuleIdOld);
			pRulesProcessingPage->CheckDialogForCompleteness();
			pCardNumBlockingPage->CompareStructures(sRuleIdOld,sCR01DatabaseStructure);

			m_StrMessageLbl = "Record Updated";
			if (!UpdateData(FALSE))
            {
				AfxMessageBox (IDS_UPDATEDATAFAILED, MB_ICONSTOP);	
			}
			
            pRulesProcessingPage->CleanProcessingDialog();				
			pRulesProcessingPage->EnableProcessingDialog(FALSE);				


			if ( pCardNumBlockingPage->m_bExceptionCardNum)
			{
				pCardNumBlockingPage->CleanCardBlockingDialog(); 

			}

			if (pChangeHistoryPage->m_bChangeHistory)
				pChangeHistoryPage->CleanChangeHistory();

						
			((CPropertySheet*) this->GetParent())->SetActivePage(0);		
			m_CntlRuleID.EnableWindow(TRUE);

			SetModified ( FALSE );
			m_nDialogMode = MODE_DONE;


			pCardNumBlockingPage->m_nDialogMode =MODE_DONE;
		
			//return CPropertyPage::OnApply();
			return FALSE;
		}	
     }

		if (m_nDialogMode == MODE_VIEW)
    {
		m_nDialogMode = MODE_DONE;
		pCardNumBlockingPage->m_nDialogMode =MODE_DONE;

		pRulesProcessingPage->CleanProcessingDialog();				
		pRulesProcessingPage->EnableProcessingDialog(FALSE);				


			if ( pCardNumBlockingPage->m_bExceptionCardNum)
			{
				pCardNumBlockingPage->CleanCardBlockingDialog(); 

			}
		((CPropertySheet*) this->GetParent())->SetActivePage(0);
		m_CntlRuleID.EnableWindow(TRUE); 

		return FALSE;
	}

 

	
			m_CntlRuleID.EnableWindow(TRUE);
		
    //return CPropertyPage::OnApply();
    return FALSE;
} 

/************************************************************/
////////////////////////////////////////////////////////////
/************************************************************/

BOOL CRuleDefinitionMain::PopulateDatabaseStructureFromRuleIdDialog ( pCR01 psRuleId) 
{
    ZeroMemory (psRuleId, sizeof(CR01)) ;

	if ( m_nDialogMode == MODE_EDIT )
	{
		memcpy (psRuleId,&sRuleIdOld, sizeof(CR01));

	}
	
    pRulesProcessingPage->PopulateDatabaseFromRulesProcessingDialog( psRuleId );      
	pCardNumBlockingPage->PopulateDatabaseStructurefromCardBlockingDialog( psRuleId );
	
    
	
	
	return TRUE;
}

/************************************************************/
////////////////////////////////////////////////////////////
/************************************************************/

void CRuleDefinitionMain::PopulateHistoryTable()
{
    HST01 sHistoryData;
    CHAR dest_str[18];   
	CMyUtils myUtilsObject;
	char strErrorMsg [200] = "";
	CString temp;
	
    char strDate[9] = "";
    char strTime[7] = "";
    char strPrimaryKey[50] = "";
	
    memset ( strDate, 0, sizeof (strDate));
    memset ( strTime, 0, sizeof (strTime));
    memset ( strPrimaryKey, 0, sizeof (strPrimaryKey));
	
    strcpy ( strPrimaryKey, m_StrRuleID_Fld);

    
    myUtilsObject.GetSystemDate(strDate); 
    myUtilsObject.GetSystemTime(strTime);
	
    pintime_get_gmt ( dest_str );
	
    ZeroMemory ( &sHistoryData, sizeof (HST01));
    memcpy ((LPTSTR) sHistoryData.file_name,     (LPCTSTR)"CR01", sizeof (sHistoryData.file_name));
    temp = strDate;
    if ( temp.GetLength() != 0 )
		temp = temp.Right(4) + temp.Left(2) + temp.Mid(2,2);
    
    CHAR wstation[21]; DWORD wsize;
	
    wsize = sizeof( wstation );
    GetComputerName( wstation, &wsize );
	
	memcpy ((LPTSTR)sHistoryData.reserved,(LPTSTR)wstation, sizeof(sHistoryData.reserved));
    memcpy ((LPTSTR) sHistoryData.date_modified, temp, sizeof (sHistoryData.date_modified));
    memcpy ((LPTSTR) sHistoryData.file_primary_key,  (LPCTSTR)strPrimaryKey, sizeof (sHistoryData.file_primary_key));
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
		m_nDialogMode = MODE_DONE;
		UpdateData( FALSE );
	}
    
}

/************************************************************/
////////////////////////////////////////////////////////////
/************************************************************/

BOOL CRuleDefinitionMain::CheckDialogForCompleteness()
{
    CMyUtils myUtilsObject;
	
    if (!UpdateData(TRUE)){
		AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
	}
	
    if (myUtilsObject.IsEmpty (m_StrRuleID_Fld, this))
    {
		m_StrMessageLbl = "Please enter Rule ID";	
		m_CntlRuleID.SetFocus();
		if (!UpdateData(FALSE)){
			AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
		}
		m_nDialogMode = MODE_DONE;
		return FALSE;
	}	
	
	return TRUE;
}

/************************************************************/
////////////////////////////////////////////////////////////
/************************************************************/

void CRuleDefinitionMain::OnCancel() 
{      

	if ( pRulesProcessingPage->m_bProcessing)
    {
		pRulesProcessingPage->CleanProcessingDialog();
		pRulesProcessingPage->EnableProcessingDialog(FALSE);
    }

	if ( pCardNumBlockingPage->m_bExceptionCardNum)
	{
		pCardNumBlockingPage->CleanCardBlockingDialog(); 

	}

    if ( pChangeHistoryPage->m_bChangeHistory )
	{
		pChangeHistoryPage->CleanChangeHistory();
		pChangeHistoryPage->m_nDialogMode = MODE_DONE;
	}
	


	SetModified ( FALSE );
	m_nDialogMode = MODE_DONE;
	CPropertyPage::OnCancel();
}

BOOL CRuleDefinitionMain::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	// TODO: Add your specialized code here and/or call the base class
	
	c_time = CTime::GetCurrentTime();	
	shrmem.SetLastUpdateTime(c_time);
	return CPropertyPage::OnCommand(wParam, lParam);
}
