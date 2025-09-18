

#include "stdafx.h"
#include "Atp.h"
#include "OnlinePINConfigureBDKKey.h"
#include "OnlinePINConfigureBDKKeyGeneral.h"
#include "NameList.h"
#include "MyUtils.h"

//#include "common.h"
//#include "ptepci.h"
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
extern CSharedMem shrmem;
char card_type[20]={0};
extern CString StrCard_brand;
extern CString StrBDK_key;

IMPLEMENT_DYNAMIC(CBDKKeyConfigurationLink, CPropertyPage)  // if u use DECLARE_DYNAMIC(CATP_Monitor_Service) IN .H FILE
														// u shld defin it in particulsr .cpp
CBDKKeyConfigurationLink::CBDKKeyConfigurationLink(): CPropertyPage(CBDKKeyConfigurationLink::IDD)	
{
	//{{AFX_DATA_INIT(CNetworkDefinition)
	m_StrCardBrandFld 		= _T("");
	m_StrMessageLbl	  		= _T("");
	//}}AFX_DATA_INIT
}

CBDKKeyConfigurationLink::~CBDKKeyConfigurationLink()
{
}
void CBDKKeyConfigurationLink::DoDataExchange(CDataExchange* pDX) // very imprtnt 
{
	CPropertyPage::DoDataExchange(pDX);
		//{{AFX_DATA_MAP(CFraudControlConfigurationLink)
	DDX_Control(pDX, IDC_CONFIGUREKEY_VIEW_BUTTON, m_ViewButton);
	DDX_Control(pDX, IDC_CONFIGUREKEY_UPDATE_BUTTON, m_UpdateButton);
	//DDX_Control(pDX, IDC_CONFIGUREKEY_LIST_BUTTON, m_ListButton);
	DDX_Control(pDX, IDC_CONFIGUREKEY_DELETE_BUTTON, m_DeleteButton);
	DDX_Control(pDX, IDC_CONFIGUREKEY_ADD_BUTTON, m_AddButton);
	DDX_Control(pDX, IDC_CONFIGURE_KEY_ID_FLD, m_CntlCardBrandFld);
	DDX_Text(pDX, IDC_MESSAGE_LBL, m_StrMessageLbl);
	DDX_Text(pDX, IDC_CONFIGURE_KEY_ID_FLD, m_StrCardBrandFld);
	DDV_MaxChars(pDX, m_StrCardBrandFld, 11);
		//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CBDKKeyConfigurationLink, CPropertyPage) // very imprtnt.
//{{AFX_MSG_MAP(CCardsKeyblockDefinition
	ON_BN_CLICKED(IDC_CONFIGUREKEY_ADD_BUTTON, OnBnClickedConfigurekeyAddButton)
	ON_BN_CLICKED(IDC_CONFIGUREKEY_DELETE_BUTTON, OnBnClickedConfigurekeyDeleteButton)
	ON_BN_CLICKED(IDC_CONFIGUREKEY_UPDATE_BUTTON, OnBnClickedConfigurekeyUpdateButton)
	ON_BN_CLICKED(IDC_CONFIGUREKEY_VIEW_BUTTON, OnBnClickedConfigurekeyViewButton)
	//ON_BN_CLICKED(IDC_CONFIGUREKEY_LIST_BUTTON, &CBDKKeyConfigurationLink::OnBnClickedConfigurekeyListButton)
//}}AFX_MSG_MAP
END_MESSAGE_MAP()


void CBDKKeyConfigurationLink::OnCancel() 
{
	// TODO: Add your specialized code here and/or call the base class

   //clear out all the user fields on the main tab
	
   //clear out all the user fields on the main tab
		if ( pChangeHistoryPage->m_bChangeHistory )
		pChangeHistoryPage->CleanChangeHistory();

	if(pCBDKKeyConfigurationGeneral->m_bBDKKEYGeneral)
	{
		pCBDKKeyConfigurationGeneral->CleanConfigurGeneralDialog();	          
		pCBDKKeyConfigurationGeneral->EnableConfigurGeneralDialog(FALSE);       
	}


	CPropertyPage::OnCancel();
}


BOOL CBDKKeyConfigurationLink::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	// TODO: Add your specialized code here and/or call the base class
	
	c_time = CTime::GetCurrentTime();	
	shrmem.SetLastUpdateTime(c_time);
	return CPropertyPage::OnCommand(wParam, lParam);
}

BOOL CBDKKeyConfigurationLink::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	CFont m_font;
	LOGFONT lf;
	BYTE rcode;
	char strErrorMessage[200]={0};
	memset ( &lf, 0, sizeof (lf));
	lf.lfHeight = 12;
	lf.lfWeight = 700;
	strcpy(lf.lfFaceName, "Arial");
	m_font.CreateFontIndirect (&lf);
	GetDlgItem(IDC_MESSAGE_LBL)->SetFont (&m_font);

	m_StrCardBrandFld 		= _T("");
	m_StrMessageLbl	  		= _T("");
  
	CString temp;
	int nIndex = -1;
	for ( int i = 0; i <=15; i++ )
	{
		temp = ucf01.control[i].subsystem_name;
		if ( strcmp ( temp, "Authorization Control" ) == 0)
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
	  m_nDialogMode = MODE_DONE;
		  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE

	return TRUE;
}


//void CFraudControlConfigurationLink::OnUpdateButton() 
BOOL CBDKKeyConfigurationLink::OnApply()
{
	char buf[100]={0};
	BYTE rcode ;
	char strErrorMessage[200]={0};
	CMyUtils myUtilsObject;
	BDKKEY01 sBDKKeyDatabasestructure;
	INT length=0;

	/*if ( m_nDialogMode == MODE_VIEW)
	{
		((CPropertySheet*) this->GetParent())->SetActivePage(0);
		//return CPropertyPage::OnApply();
      return FALSE;
	}*/
	if (m_nDialogMode != MODE_DONE)
	{
		/*if ( myUtilsObject.IsEmpty (pCBDKKeyConfigurationGeneral->m_StrCard_brand, this)) 
	    {
		    ((CPropertySheet*) this->GetParent())->SetActivePage(1);
            pCBDKKeyConfigurationGeneral->m_CntlCard_brand.SetFocus();
		    pCBDKKeyConfigurationGeneral->m_StrMessageLbl = "Please Select the Card Brand";
		    pCBDKKeyConfigurationGeneral->UpdateData(FALSE);
		    return FALSE;
	    }*/
		if ( myUtilsObject.IsEmpty (pCBDKKeyConfigurationGeneral->m_StrBdk_key, this) ) 
	    {
		    ((CPropertySheet*) this->GetParent())->SetActivePage(1);
            pCBDKKeyConfigurationGeneral->m_CntlBdk_key.SetFocus();
		    pCBDKKeyConfigurationGeneral->m_StrMessageLbl = "Please Enter the BDK Key details";
		    pCBDKKeyConfigurationGeneral->UpdateData(FALSE);
		    return FALSE;
	    }
		
	}

	

	if (m_nDialogMode == MODE_EDIT)
	{ 
		memset(&sBDKKeyDatabasestructure,0,sizeof(BDKKEY01)) ;
		if (!(PopulateDatabaseFromConfigureBDKKeyDialog(&sBDKKeyDatabasestructure)))
        {		
			return FALSE;
		}

		rcode = txdsapi_update_record((pBYTE)&sBDKKeyDatabasestructure, sizeof (BDKKEY01), BDKKEY01_DATA, strErrorMessage);  
             
        if (rcode != PTEMSG_OK)
		{   	
			m_StrMessageLbl = strErrorMessage;
            m_nDialogMode = MODE_DONE;

			 ((CPropertySheet*) this->GetParent())->SetWindowText(" BDK Key Configuration");
				//set version info in the title bar
			CString appProdVer;
			CString lpszTitle;
			appProdVer.LoadString(IDS_PRODUCT_VERSION);
			((CPropertySheet*) this->GetParent())->GetWindowText(lpszTitle);
		//	lpszTitle = lpszTitle + " - Version - ";
			lpszTitle += appProdVer;
			((CPropertySheet*) this->GetParent())->SetWindowText(lpszTitle);
		    m_CntlCardBrandFld.EnableWindow(TRUE);

			if (!UpdateData(FALSE))
            {
				AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);                
			}
			return FALSE;                         
        }
        else 
		{            
			memcpy ((LPSTR)m_strCurrentData,"Record updated",sizeof (m_strCurrentData));
		
			PopulateHistoryTable();            
		
			m_StrMessageLbl = "Record Updated";	
			
			if (!UpdateData(FALSE))
            {
				AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
			}

			strncpy(card_type,m_StrCardBrandFld,sizeof(card_type));
			pCBDKKeyConfigurationGeneral->CompareStructures();
			if(pCBDKKeyConfigurationGeneral->m_bBDKKEYGeneral)
			{
				pCBDKKeyConfigurationGeneral->CleanConfigurGeneralDialog();	          
				pCBDKKeyConfigurationGeneral->EnableConfigurGeneralDialog(FALSE);
			}
            	
            if ( pChangeHistoryPage->m_bChangeHistory )
				pChangeHistoryPage->CleanChangeHistory();
			
			((CPropertySheet*) this->GetParent())->SetActivePage(0);
            ((CPropertySheet*) this->GetParent())->SetWindowText("BDKKey Definition");	
				//set version info in the title bar
			CString appProdVer;
			CString lpszTitle;
			appProdVer.LoadString(IDS_PRODUCT_VERSION);
			((CPropertySheet*) this->GetParent())->GetWindowText(lpszTitle);
			lpszTitle = lpszTitle + " - Version - ";
			lpszTitle += appProdVer;
			((CPropertySheet*) this->GetParent())->SetWindowText(lpszTitle);
			
			m_CntlCardBrandFld.EnableWindow(TRUE);
           	SetModified ( FALSE );

            m_nDialogMode = MODE_DONE;

           // return CPropertyPage::OnApply();
           return FALSE;
        } 
 
	}

	if(m_nDialogMode == MODE_ADD)
	{
		
        memset(&sBDKKeyDatabasestructure,0,sizeof(BDKKEY01)) ;

        
        if (!(PopulateDatabaseFromConfigureBDKKeyDialog(&sBDKKeyDatabasestructure)))
        {		
			return FALSE;
		}             


		rcode = txdsapi_insert_record((pBYTE)&sBDKKeyDatabasestructure, sizeof (BDKKEY01), BDKKEY01_DATA, strErrorMessage);  
             
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
			
			 m_CntlCardBrandFld.SetFocus(); 
			m_CntlCardBrandFld.EnableWindow(TRUE);
			if (!UpdateData(FALSE))
            {
				AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
			}
			if (pCBDKKeyConfigurationGeneral->m_bBDKKEYGeneral)
			{
				pCBDKKeyConfigurationGeneral->CleanConfigurGeneralDialog();	          
				pCBDKKeyConfigurationGeneral->EnableConfigurGeneralDialog(FALSE);  
			}
              
            	
            if ( pChangeHistoryPage->m_bChangeHistory )
				pChangeHistoryPage->CleanChangeHistory();
			
         m_CntlCardBrandFld.SetFocus(); 
			m_CntlCardBrandFld.EnableWindow(TRUE);
			 SetModified ( FALSE );
            m_nDialogMode = MODE_DONE;
            //return CPropertyPage::OnApply();
           return FALSE;
        } 
	
	}
	m_nDialogMode = MODE_DONE;		
     //return CPropertyPage::OnApply();
    return FALSE;
}


void CBDKKeyConfigurationLink::OnBnClickedConfigurekeyAddButton()
{
	// TODO: Add your control notification handler code here
	CMyUtils myUtilsObject;
	char strErrorMessage [200] = {0};   
	BDKKEY01 sBDKKEY01;

	m_nDialogMode = MODE_ADD;    
	if (!CheckDialogForCompleteness())
	{
		return ;	
	}
	pCBDKKeyConfigurationGeneral->m_nDialogMode = MODE_ADD;
	pChangeHistoryPage->m_nDialogMode = MODE_ADD;
	ZeroMemory(&sBDKKEY01,sizeof(BDKKEY01));

	memcpy ((LPSTR)(sBDKKEY01.card_brand), m_StrCardBrandFld ,sizeof (sBDKKEY01.card_brand));

		//check if network with this ID is already exists in NCF01 structure 
    int rcode = txdsapi_get_record((pBYTE)&sBDKKEY01, sizeof (BDKKEY01), BDKKEY01_DATA, strErrorMessage);
         
    if(rcode == PTEMSG_OK )
    {
	    m_StrMessageLbl = "ERROR: Key already exists";
		m_nDialogMode = MODE_DONE;
        
        if (!UpdateData(FALSE))
        {
		    AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
	    }
		    return ;
    }

	pCBDKKeyConfigurationGeneral->m_StrCard_brand = m_StrCardBrandFld;
	if((pCBDKKeyConfigurationGeneral->m_bBDKKEYGeneral))
	{
		pCBDKKeyConfigurationGeneral->CleanConfigurGeneralDialog();
		pCBDKKeyConfigurationGeneral->UpdateData(FALSE); 
		pCBDKKeyConfigurationGeneral->EnableConfigurGeneralDialog(TRUE);
		/*memcpy((LPSTR)BDKKEY01.card_brand, m_StrCardBrandFld, sizeof(BDKKEY01.card_brand));
		memcpy((LPSTR)BDKKEY01.bdk_key, m_StrCardBrandFld, sizeof(BDKKEY01.bdk_key));*/

	}
            	

	 pChangeHistoryPage->bNext = FALSE;   

	 ((CPropertySheet*) this->GetParent())->SetWindowText("Add Card Brand details");
	//set version info in the title bar
	CString appProdVer;
	CString lpszTitle;
	appProdVer.LoadString(IDS_PRODUCT_VERSION);
	((CPropertySheet*) this->GetParent())->GetWindowText(lpszTitle);
	lpszTitle = lpszTitle + " - Version - ";
	lpszTitle += appProdVer;
	((CPropertySheet*) this->GetParent())->SetWindowText(lpszTitle);

	m_CntlCardBrandFld.EnableWindow(FALSE);
   ((CPropertySheet*) this->GetParent())->SetActivePage(1);
}

void CBDKKeyConfigurationLink::OnBnClickedConfigurekeyDeleteButton()
{
	// TODO: Add your control notification handler code here
	char strErrorMessage[200] = {0};   
	CString strMessage;
	int nReturnCode;
	BDKKEY01 sBDKKEY01;

	m_nDialogMode = MODE_DONE;    
		m_StrMessageLbl = "";

	if (!CheckDialogForCompleteness())
	{
		return ;	
	}

	if (!UpdateData(TRUE))
    {
		AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);	
	}
    
	strMessage.Format ("Are you sure you want to delete BDK Key configuration %s?", (LPCTSTR) m_StrCardBrandFld) ;	

	nReturnCode = AfxMessageBox ((LPCTSTR)strMessage,MB_OKCANCEL|MB_ICONQUESTION) ;

	if (nReturnCode != IDOK) 
	{
		return ;
	}

	memcpy ((LPSTR)(sBDKKEY01.card_brand), m_StrCardBrandFld ,sizeof (sBDKKEY01.card_brand));
 
    int rcode = txdsapi_delete_record((pBYTE)&sBDKKEY01, sizeof (BDKKEY01), BDKKEY01_DATA, strErrorMessage);

	if (rcode != PTEMSG_OK)
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
		memcpy ((LPSTR)m_strCurrentData,"Record deleted",sizeof (m_strCurrentData));
		
		PopulateHistoryTable();
		m_StrMessageLbl = "Record deleted";	
		m_StrCardBrandFld= _T("");	
		if (!UpdateData(FALSE))
		{
			AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
		}
			if(pCBDKKeyConfigurationGeneral->m_bBDKKEYGeneral)
			{
				pCBDKKeyConfigurationGeneral->CleanConfigurGeneralDialog();	          
				pCBDKKeyConfigurationGeneral->EnableConfigurGeneralDialog(FALSE);
			}
			
		if ( pChangeHistoryPage->m_bChangeHistory )
			pChangeHistoryPage->CleanChangeHistory();

		((CPropertySheet*) this->GetParent())->SetActivePage(0);
		 SetModified ( FALSE );    


		m_CntlCardBrandFld.EnableWindow(TRUE);

	} 
}


void CBDKKeyConfigurationLink::OnBnClickedConfigurekeyUpdateButton()
{
	// TODO: Add your control notification handler code here
	char strErrorMessage [200] = {0};   
	HST01 sHistoryRecord;

	m_nDialogMode = MODE_EDIT;    
		pCBDKKeyConfigurationGeneral->m_nDialogMode = MODE_EDIT;
	pChangeHistoryPage->m_nDialogMode = MODE_EDIT;
	if (!CheckDialogForCompleteness())
	{
		return ;	
	}
	m_StrMessageLbl= _T("");
	memcpy ((LPSTR)(BDKKEY01_old.card_brand), m_StrCardBrandFld ,sizeof (BDKKEY01_old.card_brand));

	int rcode = txdsapi_get_record((pBYTE)&BDKKEY01_old, sizeof (BDKKEY01), BDKKEY01_DATA, strErrorMessage);
         
    if(rcode != PTEMSG_OK )
    {
	   m_StrMessageLbl = "ERROR: Record does not exist";
       m_nDialogMode = MODE_DONE;

	   if (!UpdateData(FALSE))
       {
		   AfxMessageBox (IDS_UPDATEDATAFAILED, MB_ICONSTOP);		
	   }
       return;
    }

	ZeroMemory (&sHistoryRecord, sizeof (sHistoryRecord));

    memset ( strFilePrimaryKey,0, sizeof (strFilePrimaryKey));

    strcpy ( strFilePrimaryKey, m_StrCardBrandFld);

    memcpy ((LPSTR)(sHistoryRecord.primary_key.unique_id),"                 ",sizeof (sHistoryRecord.primary_key.unique_id));
    memcpy ((LPSTR)(sHistoryRecord.file_name),"BDKKEY01",	sizeof (sHistoryRecord.file_name));
    memcpy ((LPSTR)(sHistoryRecord.file_primary_key), strFilePrimaryKey,	sizeof (sHistoryRecord.file_primary_key));
    memcpy ( &(pChangeHistoryPage->sHistoryRecord),&sHistoryRecord,sizeof(sHistoryRecord));

	((CPropertySheet*) this->GetParent())->SetActivePage(1);  

	 ((CPropertySheet*) this->GetParent())->SetWindowText("Update Network ID");
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

	pCBDKKeyConfigurationGeneral->PopulateGeneralDialogFromDatabase (BDKKEY01_old);
		m_CntlCardBrandFld.EnableWindow(FALSE);

			if(pCBDKKeyConfigurationGeneral->m_bBDKKEYGeneral)
			{
				
				pCBDKKeyConfigurationGeneral->UpdateData(FALSE);  
				pCBDKKeyConfigurationGeneral->EnableConfigurGeneralDialog(TRUE);
				
			}
((CPropertySheet*) this->GetParent())->SetActivePage(1);
}


void CBDKKeyConfigurationLink::OnBnClickedConfigurekeyViewButton()
{
	// TODO: Add your control notification handler code here
	char strErrorMessage [200] = {0};   
	HST01 sHistoryRecord;
	BDKKEY01 sBDKKEY01;
    m_nDialogMode = MODE_VIEW;
	pChangeHistoryPage->m_nDialogMode = MODE_VIEW;
	pChangeHistoryPage->bNext = FALSE;	
	m_StrMessageLbl = "";	

	if (!CheckDialogForCompleteness())
	{
		return ;	
	}

	memcpy ((LPSTR)(sBDKKEY01.card_brand), m_StrCardBrandFld ,sizeof (sBDKKEY01.card_brand));

		//check if network with this ID is already exists in NCF01 structure 
    int rcode = txdsapi_get_record((pBYTE)&sBDKKEY01, sizeof (BDKKEY01), BDKKEY01_DATA, strErrorMessage);
         
	if (rcode != PTEMSG_OK)
	{        
       m_StrMessageLbl = strErrorMessage;

	   if (!UpdateData(FALSE))
       {
		   AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
	   }
       return;
    }
	((CPropertySheet*) this->GetParent())->SetActivePage(1);

	if(pCBDKKeyConfigurationGeneral->m_bBDKKEYGeneral)
	{
		pCBDKKeyConfigurationGeneral->CleanConfigurGeneralDialog();	          
		pCBDKKeyConfigurationGeneral->EnableConfigurGeneralDialog(FALSE);
		pCBDKKeyConfigurationGeneral->m_nDialogMode = MODE_VIEW;
	}

	ZeroMemory (&sHistoryRecord, sizeof (sHistoryRecord));

    memset ( strFilePrimaryKey,0,sizeof (strFilePrimaryKey));
    strcpy ( strFilePrimaryKey,m_StrCardBrandFld);

    memcpy ((LPSTR)(sHistoryRecord.primary_key.unique_id),"                 ",sizeof (sHistoryRecord.primary_key.unique_id));
    memcpy ((LPSTR)(sHistoryRecord.file_name),"BDKKEY01",sizeof (sHistoryRecord.file_name));
    memcpy ((LPSTR)(sHistoryRecord.file_primary_key),strFilePrimaryKey,sizeof (sHistoryRecord.file_primary_key));
    memcpy ( &(pChangeHistoryPage->sHistoryRecord),&sHistoryRecord,sizeof(sHistoryRecord));
    
	((CPropertySheet*) this->GetParent())->SetActivePage(1);
		
	//set version info in the title bar
	CString appProdVer;
	CString lpszTitle;
	appProdVer.LoadString(IDS_PRODUCT_VERSION);
	((CPropertySheet*) this->GetParent())->GetWindowText(lpszTitle);
//	lpszTitle = lpszTitle + " - Version - ";
	//lpszTitle += appProdVer;
	((CPropertySheet*) this->GetParent())->SetWindowText(lpszTitle);

	if ( pChangeHistoryPage->m_bChangeHistory )
		pChangeHistoryPage->PopulateHistoryDialog();

		         
		pCBDKKeyConfigurationGeneral->PopulateGeneralDialogFromDatabase (sBDKKEY01);
		pCBDKKeyConfigurationGeneral->SetModified(FALSE);
}

/*
void CBDKKeyConfigurationLink::OnBnClickedConfigurekeyListButton()
{
	// TODO: Add your control notification handler code here
	CNameList listDlg;    
    int nReturnCode;

	m_StrMessageLbl = "";
	UpdateData(TRUE);

    listDlg.m_nIndicator = MODE_BDK_KEY01;
    listDlg.strPrimaryKey1 = m_StrCardBrandFld;  

	nReturnCode = listDlg.DoModal();

	if ( nReturnCode == IDOK)
	{
        m_StrCardBrandFld = StrCard_brand;
		pCBDKKeyConfigurationGeneral->m_StrBdk_key = StrBDK_key;
	}

		if(pCBDKKeyConfigurationGeneral->m_bBDKKEYGeneral)
			{
				pCBDKKeyConfigurationGeneral->CleanConfigurGeneralDialog();	          
				pCBDKKeyConfigurationGeneral->EnableConfigurGeneralDialog(FALSE);
			}
	if ( pChangeHistoryPage->m_bChangeHistory )
		pChangeHistoryPage->CleanChangeHistory();
     
    m_CntlCardBrandFld.EnableWindow(TRUE);
        
    if (!UpdateData(FALSE))
    {
		AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
	}	
}*/

BOOL CBDKKeyConfigurationLink::CheckDialogForCompleteness()
{
    CMyUtils myUtilsObject;

    if (!UpdateData(TRUE))
	{
		AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
	}

    if (myUtilsObject.IsEmpty (m_StrCardBrandFld, this))
    {
		m_StrMessageLbl = "Please enter card brand";	
		m_CntlCardBrandFld.SetFocus();
		if (!UpdateData(FALSE))
		{
			AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
		}
		m_nDialogMode = MODE_DONE;
		return FALSE;
	}	
   
    return TRUE;
}

BOOL CBDKKeyConfigurationLink::PopulateDatabaseFromConfigureBDKKeyDialog( pBDKKEY01 psBDKKEY01)
{
	ZeroMemory (psBDKKEY01, sizeof(BDKKEY01)) ;
	   
	memcpy ((LPTSTR) psBDKKEY01->card_brand,m_StrCardBrandFld, sizeof (psBDKKEY01->card_brand));

	pCBDKKeyConfigurationGeneral->PopulateDatabaseFromGeneralDialog( psBDKKEY01 ); 

	return TRUE;
  
}

void CBDKKeyConfigurationLink::PopulateHistoryTable()
{
	HST01 sHistoryData;
    CHAR dest_str[18];   
	CMyUtils myUtilsObject;
	char strErrorMsg [200] = "";
	CString temp;
    CHAR wstation[21]; DWORD wsize;
    char strDate[9] = "";
    char strTime[7] = "";
    char strPrimaryKey[50] = "";

    memset ( strDate, 0, sizeof (strDate));
    memset ( strTime, 0, sizeof (strTime));
    memset ( strPrimaryKey, 0, sizeof (strPrimaryKey));

    strcpy ( strPrimaryKey, m_StrCardBrandFld);
    
    myUtilsObject.GetSystemDate(strDate); 
    myUtilsObject.GetSystemTime(strTime);

    pintime_get_gmt ( dest_str );

    ZeroMemory ( &sHistoryData, sizeof (HST01));

    memcpy ((LPTSTR) sHistoryData.file_name,            (LPCTSTR)"BDKKEY01",               sizeof (sHistoryData.file_name));
    temp = strDate;
    temp = temp.Right(4) + temp.Left(2) + temp.Mid(2,2);
    

    wsize = sizeof( wstation );
    GetComputerName( wstation, &wsize );

	memcpy ((LPTSTR)sHistoryData.reserved,(LPTSTR)wstation, sizeof(sHistoryData.reserved));
    memcpy ((LPTSTR) sHistoryData.date_modified, temp, sizeof (sHistoryData.date_modified));
    memcpy ((LPTSTR) sHistoryData.file_primary_key,     (LPCTSTR)strPrimaryKey,         sizeof (sHistoryData.file_primary_key));
    memcpy ((LPTSTR) sHistoryData.primary_key.unique_id,(LPCTSTR)dest_str,              sizeof (sHistoryData.primary_key.unique_id));        
    memcpy ((LPTSTR) sHistoryData.time_modified,        (LPCTSTR)strTime,               sizeof (sHistoryData.time_modified));               
    memcpy ((LPTSTR)sHistoryData.current_data,          (LPCTSTR)m_strCurrentData,      sizeof (sHistoryData.current_data));
	//memcpy ((LPTSTR)sHistoryData.previous_data,         (LPCTSTR)m_StrPreviousData,     sizeof (sHistoryData.previous_data));
	memcpy ((LPTSTR)sHistoryData.changed_by,            (LPCTSTR)ucf01.primary_key.name,sizeof (sHistoryData.changed_by));
	//memcpy ((LPTSTR)sHistoryData.field_tag,             (LPCTSTR)m_StrFieldTag,         sizeof (sHistoryData.field_tag));
        
    int rcode = txdsapi_insert_record  ( (pBYTE)&sHistoryData, sizeof (HST01), HST01_DATA, strErrorMsg );
	if ( rcode != PTEMSG_OK )
	{
		m_StrMessageLbl = strErrorMsg;
		m_nDialogMode = MODE_DONE;
		UpdateData( FALSE );
	}	
}

BOOL CBDKKeyConfigurationLink::PopulateConfigureBDKKeyDialogFromDatabase( BDKKEY01 sBDKKEY01)
{
	if (!pCBDKKeyConfigurationGeneral->m_bBDKKEYGeneral)
	{
		memcpy ( &pCBDKKeyConfigurationGeneral->BDKKEY01_old, &sBDKKEY01, sizeof ( BDKKEY01));
	}
	else
	{
		pCBDKKeyConfigurationGeneral->PopulateGeneralDialogFromDatabase(sBDKKEY01);
	}

	 return TRUE;
}