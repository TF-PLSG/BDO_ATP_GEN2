

#include "stdafx.h"
#include "atp.h"
#include "MyUtils.h"
#include "FraudControlConfigureLinkMainPage.h"
#include "FraudControlConfigureLinkGeneralPage.h"
#include "FraudControlConfigureLinkSheet.h"
#include "common.h"
#include "ptepci.h"
#include "SharedMem.h"
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

extern UCF01 ucf01;
extern CString name;
extern CSharedMem shrmem;
char networkid1[11]={0};
/////////////////////////////////////////////////////////////////////////////
// CFraudControlConfigurationLinkpage property page
IMPLEMENT_DYNAMIC(CFraudControlConfigurationLinkpage, CPropertyPage)  // if u use DECLARE_DYNAMIC(CATP_Monitor_Service) IN .H FILE
														// u shld defin it in particulsr .cpp
CFraudControlConfigurationLinkpage::CFraudControlConfigurationLinkpage()
	: CPropertyPage(CFraudControlConfigurationLinkpage::IDD)	
{
	//{{AFX_DATA_INIT(CNetworkDefinition)
	m_StrNetowrkFld		= _T("");
	m_StrMessageLbl		= _T("");
	//}}AFX_DATA_INIT
}

CFraudControlConfigurationLinkpage::~CFraudControlConfigurationLinkpage()
{
}
void CFraudControlConfigurationLinkpage::DoDataExchange(CDataExchange* pDX) // very imprtnt 
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFraudControlConfigurationLinkpage)
	DDX_Control(pDX, IDC_CONFIGURE_LINK_VIEW_BUTTON, m_ViewButton);
	DDX_Control(pDX, IDC_CONFIGURE_LINK_UPDATE_BUTTON, m_UpdateButton);
	DDX_Control(pDX, IDC_CONFIGURE_LINK_LIST_BUTTON, m_ListButton);
	DDX_Control(pDX, IDC_CONFIGURE_LINK_DELETE_BUTTON, m_DeleteButton);
	DDX_Control(pDX, IDC_CONFIGURE_LINK_ADD_BUTTON, m_AddButton);
	DDX_Control(pDX, IDC_CONFIGURE_LINK_ID_FLD, m_CntlNetowrkFld);
	DDX_Text(pDX, IDC_MESSAGE_LBL, m_StrMessageLbl);
	DDX_Text(pDX, IDC_CONFIGURE_LINK_ID_FLD, m_StrNetowrkFld);
	//DDV_MaxChars(pDX, m_StrNetowrkFld, 10);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CFraudControlConfigurationLinkpage, CPropertyPage) // very imprtnt.
	//{{AFX_MSG_MAP(CFraudControlConfigurationLinkpage)
	ON_WM_HELPINFO()
	ON_BN_CLICKED(IDC_CONFIGURE_LINK_ADD_BUTTON, &CFraudControlConfigurationLinkpage::OnBnClickedConfigureLinkAddButton)
	ON_BN_CLICKED(IDC_CONFIGURE_LINK_DELETE_BUTTON, &CFraudControlConfigurationLinkpage::OnBnClickedConfigureLinkDeleteButton)
	ON_BN_CLICKED(IDC_CONFIGURE_LINK_UPDATE_BUTTON, &CFraudControlConfigurationLinkpage::OnBnClickedConfigureLinkUpdateButton)
	ON_BN_CLICKED(IDC_CONFIGURE_LINK_VIEW_BUTTON, &CFraudControlConfigurationLinkpage::OnBnClickedConfigureLinkViewButton)
	ON_BN_CLICKED(IDC_CONFIGURE_LINK_LIST_BUTTON, &CFraudControlConfigurationLinkpage::OnBnClickedConfigureLinkListButton)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CFraudControlConfigurationLinkpage::OnCancel() 
{
	// TODO: Add your specialized code here and/or call the base class
		if(pFraudControlConfigurationLinkGeneralpage->m_bFraudGeneral)
	{
		pFraudControlConfigurationLinkGeneralpage->CleanConfigurGeneralDialog();	          
        pFraudControlConfigurationLinkGeneralpage->EnableConfigurGeneralDialog(FALSE);       
	}
	if ( pChangeHistoryPage->m_bChangeHistory )
		pChangeHistoryPage->CleanChangeHistory();
   //clear out all the user fields on the main tab
	
	CPropertyPage::OnCancel();
}

BOOL CFraudControlConfigurationLinkpage::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	// TODO: Add your specialized code here and/or call the base class
	
	c_time = CTime::GetCurrentTime();	
	shrmem.SetLastUpdateTime(c_time);
	return CPropertyPage::OnCommand(wParam, lParam);
}

BOOL CFraudControlConfigurationLinkpage::OnInitDialog() 
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

	m_StrNetowrkFld		= _T("");
	m_StrMessageLbl		= _T("");

			CString temp;
	int nIndex = -1;
	for ( int i = 0; i <=20; i++ )
	{
		temp = ucf01.control[i].subsystem_name;
		if ( strcmp ( temp, "Fraud Control" ) == 0)
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
	return TRUE; // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}



BOOL CFraudControlConfigurationLinkpage::OnApply()
{
	char buf[100]={0};
	BYTE rcode ;
	char strErrorMessage[200]={0};
	CMyUtils myUtilsObject;

		if ( m_nDialogMode == MODE_VIEW)
	{
		((CPropertySheet*) this->GetParent())->SetActivePage(0);
		//return CPropertyPage::OnApply();
      return FALSE;
	}

	if (m_nDialogMode == MODE_EDIT)
	{ 
		memset(&fclf01,0,sizeof(fclf01)) ;
	
		if (!(PopulateDatabaseFromConfigureDialog(&fclf01)))
        {		
			return FALSE;
		}

		rcode = txdsapi_update_record((pBYTE)&fclf01, sizeof (FCLF01), FCLF01_DATA, strErrorMessage);  
             
        if (rcode != PTEMSG_OK)
		{   	
			m_StrMessageLbl = strErrorMessage;
            m_nDialogMode = MODE_DONE;
			 ((CPropertySheet*) this->GetParent())->SetWindowText(" Network ID Definition");
				//set version info in the title bar
			CString appProdVer;
			CString lpszTitle;
			appProdVer.LoadString(IDS_PRODUCT_VERSION);
			((CPropertySheet*) this->GetParent())->GetWindowText(lpszTitle);
			lpszTitle = lpszTitle + " - Version - ";
			lpszTitle += appProdVer;
			((CPropertySheet*) this->GetParent())->SetWindowText(lpszTitle);
		    //m_CntlNetowrkFld.EnableWindow(TRUE);

			if (!UpdateData(FALSE))
            {
				AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);                
			}
			return FALSE;                       
        }
        else 
		{            
			memcpy ((LPSTR)m_strCurrentData,"Record updated",sizeof (m_strCurrentData));
		
			m_StrMessageLbl = "Record Updated";	
			
			if (!UpdateData(FALSE))
            {
				AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
			}

			strncpy(networkid1,m_StrNetowrkFld,sizeof(networkid1));
			pFraudControlConfigurationLinkGeneralpage->CompareStructures();
			if(pFraudControlConfigurationLinkGeneralpage->m_bFraudGeneral)
			{
				pFraudControlConfigurationLinkGeneralpage->CleanConfigurGeneralDialog();	          
				pFraudControlConfigurationLinkGeneralpage->EnableConfigurGeneralDialog(FALSE);
			}  
            	
            if ( pChangeHistoryPage->m_bChangeHistory )
				pChangeHistoryPage->CleanChangeHistory();
			
	         pChangeHistoryPage->m_nDialogMode = MODE_DONE;
			 pFraudControlConfigurationLinkGeneralpage->m_nDialogMode = MODE_DONE;

			 			((CPropertySheet*) this->GetParent())->SetActivePage(0);
            ((CPropertySheet*) this->GetParent())->SetWindowText("Network ID Definition");	
				//set version info in the title bar
			CString appProdVer;
			CString lpszTitle;
			appProdVer.LoadString(IDS_PRODUCT_VERSION);
			((CPropertySheet*) this->GetParent())->GetWindowText(lpszTitle);
			lpszTitle = lpszTitle + " - Version - ";
			lpszTitle += appProdVer;
			((CPropertySheet*) this->GetParent())->SetWindowText(lpszTitle);
			
			//m_CntlNetowrkFld.EnableWindow(TRUE);
           	SetModified ( FALSE );

            m_nDialogMode = MODE_DONE;
			//m_CntlNetowrkFld.EnableWindow(TRUE);
           // return CPropertyPage::OnApply();
           return FALSE;
        } 
 
	}

	if(m_nDialogMode == MODE_ADD)
	{
		
        memset(&fclf01,0,sizeof(fclf01)) ;

        
        if (!(PopulateDatabaseFromConfigureDialog(&fclf01)))
        {		
			return FALSE;
		}             


		rcode = txdsapi_insert_record((pBYTE)&fclf01, sizeof (FCLF01), FCLF01_DATA, strErrorMessage);  
             
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
			
			 m_CntlNetowrkFld.SetFocus(); 
			//m_CntlNetowrkFld.EnableWindow(TRUE);
			if (!UpdateData(FALSE))
            {
				AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
			}
			pFraudControlConfigurationLinkGeneralpage->CleanConfigurGeneralDialog();	          
            pFraudControlConfigurationLinkGeneralpage->EnableConfigurGeneralDialog(FALSE);                
            	
            if ( pChangeHistoryPage->m_bChangeHistory )
				pChangeHistoryPage->CleanChangeHistory();
			
			 pChangeHistoryPage->m_nDialogMode = MODE_DONE;
			 pFraudControlConfigurationLinkGeneralpage->m_nDialogMode = MODE_DONE;
           m_CntlNetowrkFld.SetFocus(); 
			//m_CntlNetowrkFld.EnableWindow(TRUE);
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





void CFraudControlConfigurationLinkpage::OnBnClickedConfigureLinkAddButton()
{
	// TODO: Add your control notification handler code here
    CMyUtils myUtilsObject;
	char strErrorMessage [200] = {0};   
	FCLF01 fclf01;

	 m_StrMessageLbl = ""; 

	m_nDialogMode = MODE_ADD;    
	if (!CheckDialogForCompleteness())
	{
		return ;	
	}

	pFraudControlConfigurationLinkGeneralpage->m_nDialogMode = MODE_ADD;
	pChangeHistoryPage->m_nDialogMode = MODE_ADD;

	memcpy ((LPSTR)(fclf01.network_id), m_StrNetowrkFld ,sizeof (fclf01.network_id));

		//check if network with this ID is already exists in NCF01 structure 
    int rcode = txdsapi_get_record((pBYTE)&fclf01, sizeof (FCLF01), FCLF01_DATA, strErrorMessage);
         
    if(rcode == PTEMSG_OK )
    {
	    m_StrMessageLbl = "ERROR: Network ID already exists";
		m_nDialogMode = MODE_DONE;
        
        if (!UpdateData(FALSE))
        {
		    AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
	    }
		    return ;
    }

	if(pFraudControlConfigurationLinkGeneralpage->m_bFraudGeneral)
	{
		pFraudControlConfigurationLinkGeneralpage->EnableConfigurGeneralDialog(TRUE);
		pFraudControlConfigurationLinkGeneralpage->CleanConfigurGeneralDialog();
	}

	 pChangeHistoryPage->bNext = FALSE;   

	 ((CPropertySheet*) this->GetParent())->SetWindowText("Add Network ID");
	//set version info in the title bar
	CString appProdVer;
	CString lpszTitle;
	appProdVer.LoadString(IDS_PRODUCT_VERSION);
	((CPropertySheet*) this->GetParent())->GetWindowText(lpszTitle);
	lpszTitle = lpszTitle + " - Version - ";
	lpszTitle += appProdVer;
	((CPropertySheet*) this->GetParent())->SetWindowText(lpszTitle);

	//m_CntlNetowrkFld.EnableWindow(FALSE);
   ((CPropertySheet*) this->GetParent())->SetActivePage(1);
}


void CFraudControlConfigurationLinkpage::OnBnClickedConfigureLinkDeleteButton()
{
	// TODO: Add your control notification handler code here
	char strErrorMessage[200] = {0};   
	CString strMessage;
	FCLF01 fclf01;
	int nReturnCode;

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
    
	strMessage.Format ("Are you sure you want to delete Network ID configuration %s?", (LPCTSTR) m_StrNetowrkFld) ;	

	nReturnCode = AfxMessageBox ((LPCTSTR)strMessage,MB_OKCANCEL|MB_ICONQUESTION) ;

	if (nReturnCode != IDOK) 
	{
		return ;
	}

	memcpy ((LPSTR)(fclf01.network_id), m_StrNetowrkFld ,sizeof (fclf01.network_id));
 
    int rcode = txdsapi_delete_record((pBYTE)&fclf01, sizeof (FCLF01), FCLF01_DATA, strErrorMessage);

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
		m_StrNetowrkFld	= _T(""); 

		if (!UpdateData(FALSE))
		{
			AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
		}

			
		if(pFraudControlConfigurationLinkGeneralpage->m_bFraudGeneral)
		{
			pFraudControlConfigurationLinkGeneralpage->CleanConfigurGeneralDialog();
			pFraudControlConfigurationLinkGeneralpage->EnableConfigurGeneralDialog(FALSE);
		}
		if ( pChangeHistoryPage->m_bChangeHistory )
			pChangeHistoryPage->CleanChangeHistory();

		((CPropertySheet*) this->GetParent())->SetActivePage(0);
		 SetModified ( FALSE );    

		//m_CntlNetowrkFld.EnableWindow(TRUE);

	} 
}


void CFraudControlConfigurationLinkpage::OnBnClickedConfigureLinkUpdateButton()
{
	// TODO: Add your control notification handler code here
	char strErrorMessage [200] = {0};   
	HST01 sHistoryRecord;

	m_nDialogMode = MODE_EDIT;    
	pFraudControlConfigurationLinkGeneralpage->m_nDialogMode = MODE_EDIT;
	pChangeHistoryPage->m_nDialogMode = MODE_EDIT;

	if (!CheckDialogForCompleteness())
	{
		return ;	
	}

	m_StrMessageLbl= _T("");
	memcpy ((LPSTR)(fclf01_old.network_id), m_StrNetowrkFld ,sizeof (fclf01_old.network_id));

	int rcode = txdsapi_get_record((pBYTE)&fclf01_old, sizeof (FCLF01), FCLF01_DATA, strErrorMessage);
         
    if(rcode != PTEMSG_OK )
    {
	   m_StrMessageLbl = strErrorMessage;
       m_nDialogMode = MODE_DONE;

	   if (!UpdateData(FALSE))
       {
		   AfxMessageBox (IDS_UPDATEDATAFAILED, MB_ICONSTOP);		
	   }
       return;
    }

	ZeroMemory (&sHistoryRecord, sizeof (sHistoryRecord));

    memset ( strFilePrimaryKey,0, sizeof (strFilePrimaryKey));

    strcpy ( strFilePrimaryKey, m_StrNetowrkFld);

    memcpy ((LPSTR)(sHistoryRecord.primary_key.unique_id),"                 ",sizeof (sHistoryRecord.primary_key.unique_id));
    memcpy ((LPSTR)(sHistoryRecord.file_name),"FCFL01",	sizeof (sHistoryRecord.file_name));
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

	pFraudControlConfigurationLinkGeneralpage->PopulateGeneralDialogFromDatabase (&fclf01_old);
	//m_CntlNetowrkFld.EnableWindow(FALSE);

	pFraudControlConfigurationLinkGeneralpage->SetModified(FALSE);

	if ( pFraudControlConfigurationLinkGeneralpage->m_bFraudGeneral)
    {  
    	pFraudControlConfigurationLinkGeneralpage->UpdateData(FALSE); 
		pFraudControlConfigurationLinkGeneralpage->EnableConfigurGeneralDialog(TRUE);        
	}      
            
	((CPropertySheet*) this->GetParent())->SetActivePage(1);
  
}



void CFraudControlConfigurationLinkpage::OnBnClickedConfigureLinkViewButton()
{
	// TODO: Add your control notification handler code here
	char strErrorMessage [200] = {0};   
	HST01 sHistoryRecord;

    m_nDialogMode = MODE_VIEW;
	pChangeHistoryPage->m_nDialogMode = MODE_VIEW;
	pChangeHistoryPage->bNext = FALSE;	
	m_StrMessageLbl = "";
	if (!CheckDialogForCompleteness())
	{
		return ;	
	}

	memcpy ((LPSTR)(fclf01.network_id), m_StrNetowrkFld ,sizeof (fclf01.network_id));

		//check if network with this ID is already exists in NCF01 structure 
    int rcode = txdsapi_get_record((pBYTE)&fclf01, sizeof (FCLF01), FCLF01_DATA, strErrorMessage);
         
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
  
	if(pFraudControlConfigurationLinkGeneralpage->m_bFraudGeneral)
	{
		pFraudControlConfigurationLinkGeneralpage->CleanConfigurGeneralDialog();
		pFraudControlConfigurationLinkGeneralpage->EnableConfigurGeneralDialog(FALSE);
		pFraudControlConfigurationLinkGeneralpage->m_nDialogMode = MODE_VIEW;
	}

	ZeroMemory (&sHistoryRecord, sizeof (sHistoryRecord));

    memset ( strFilePrimaryKey,0,sizeof (strFilePrimaryKey));
    strcpy ( strFilePrimaryKey,m_StrNetowrkFld);

    memcpy ((LPSTR)(sHistoryRecord.primary_key.unique_id),"                 ",sizeof (sHistoryRecord.primary_key.unique_id));
    memcpy ((LPSTR)(sHistoryRecord.file_name),"FCFL01",sizeof (sHistoryRecord.file_name));
    memcpy ((LPSTR)(sHistoryRecord.file_primary_key),strFilePrimaryKey,sizeof (sHistoryRecord.file_primary_key));
    memcpy ( &(pChangeHistoryPage->sHistoryRecord),&sHistoryRecord,sizeof(sHistoryRecord));
    
	((CPropertySheet*) this->GetParent())->SetActivePage(1);

	
	//set version info in the title bar
	CString appProdVer;
	CString lpszTitle;
	appProdVer.LoadString(IDS_PRODUCT_VERSION);
	((CPropertySheet*) this->GetParent())->GetWindowText(lpszTitle);
	//lpszTitle = lpszTitle + " - Version - ";
	//lpszTitle += appProdVer;
	((CPropertySheet*) this->GetParent())->SetWindowText(lpszTitle);

	if ( pChangeHistoryPage->m_bChangeHistory )
		pChangeHistoryPage->PopulateHistoryDialog();

	 
	pFraudControlConfigurationLinkGeneralpage->PopulateGeneralDialogFromDatabase ( &fclf01);
   
	pFraudControlConfigurationLinkGeneralpage->SetModified(FALSE);
}


void CFraudControlConfigurationLinkpage::OnBnClickedConfigureLinkListButton()
{
	// TODO: Add your control notification handler code here
	CNameList listDlg;    
    int nReturnCode;

	m_StrMessageLbl = "";
	UpdateData(TRUE);

    listDlg.m_nIndicator = MODE_FCLF01;
    listDlg.strPrimaryKey1 = m_StrNetowrkFld;  

	nReturnCode = listDlg.DoModal();

	if ( nReturnCode == IDOK)
        m_StrNetowrkFld = name;

	if(pFraudControlConfigurationLinkGeneralpage->m_bFraudGeneral)
	{
		pFraudControlConfigurationLinkGeneralpage->CleanConfigurGeneralDialog();
		pFraudControlConfigurationLinkGeneralpage->EnableConfigurGeneralDialog(FALSE);
	}
	if ( pChangeHistoryPage->m_bChangeHistory )
		pChangeHistoryPage->CleanChangeHistory();
     
    //m_CntlNetowrkFld.EnableWindow(TRUE);
        
    if (!UpdateData(FALSE))
    {
		AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
	}	
}

BOOL CFraudControlConfigurationLinkpage::CheckDialogForCompleteness()
{
    CMyUtils myUtilsObject;

    if (!UpdateData(TRUE))
	{
		AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
	}

    if (myUtilsObject.IsEmpty (m_StrNetowrkFld, this))
    {
		m_StrMessageLbl = "Please Select Network ID";	
		//m_CntlNetowrkFld.SetFocus();
		if (!UpdateData(FALSE))
		{
			AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
		}
		m_nDialogMode = MODE_DONE;
		return FALSE;
	}	
   
    return TRUE;
}

BOOL CFraudControlConfigurationLinkpage::PopulateDatabaseFromConfigureDialog( pFCLF01 psFclf01)
{
	ZeroMemory (psFclf01, sizeof(FCLF01)) ;
	   
	memcpy ((LPTSTR) psFclf01->network_id,m_StrNetowrkFld, sizeof (psFclf01->network_id));

	pFraudControlConfigurationLinkGeneralpage->PopulateDatabaseFromGeneralDialog( psFclf01 ); 

	return TRUE;
  
}

void CFraudControlConfigurationLinkpage::PopulateHistoryTable()
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

    strcpy ( strPrimaryKey, m_StrNetowrkFld);
    
    myUtilsObject.GetSystemDate(strDate); 
    myUtilsObject.GetSystemTime(strTime);

    pintime_get_gmt ( dest_str );

    ZeroMemory ( &sHistoryData, sizeof (HST01));

    memcpy ((LPTSTR) sHistoryData.file_name,            (LPCTSTR)"FCFL01",               sizeof (sHistoryData.file_name));
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

BOOL CFraudControlConfigurationLinkpage::PopulateConfigureLinkDialogFromDatabase( pFCLF01 sfclf01)
{
	if (!pFraudControlConfigurationLinkGeneralpage->m_bFraudGeneral)
	{
		memcpy ( &pFraudControlConfigurationLinkGeneralpage->fclf01_old, &sfclf01, sizeof ( FCLF01));
	}
	else
	{
		pFraudControlConfigurationLinkGeneralpage->PopulateGeneralDialogFromDatabase(sfclf01);
	}

	 return TRUE;
}



