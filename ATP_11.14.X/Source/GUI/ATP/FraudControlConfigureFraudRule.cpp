

#include "stdafx.h"
#include "Atp.h"
#include "MyUtils.h"
#include "FraudControlConfigureFraudRule.h"
#include "FraudControlConfigureFraudRuleGeneral.h"
#include "FraudControlFraudRuleSheet.h"
#include "common.h"
#include "ptepci.h"
#include "SharedMem.h"
#include "NameList.h"
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
extern CString name;
extern CSharedMem shrmem;
char frule[20]={0};

IMPLEMENT_DYNAMIC(CFraudControlConfigurationLink, CPropertyPage)  // if u use DECLARE_DYNAMIC(CATP_Monitor_Service) IN .H FILE
														// u shld defin it in particulsr .cpp
CFraudControlConfigurationLink::CFraudControlConfigurationLink()
	: CPropertyPage(CFraudControlConfigurationLink::IDD)	
{
	//{{AFX_DATA_INIT(CNetworkDefinition)
	m_StrFraudRuleFld 		= _T("");
	m_StrMessageLbl	  		= _T("");
	//}}AFX_DATA_INIT
}

CFraudControlConfigurationLink::~CFraudControlConfigurationLink()
{
}
void CFraudControlConfigurationLink::DoDataExchange(CDataExchange* pDX) // very imprtnt 
{
	CPropertyPage::DoDataExchange(pDX);
		//{{AFX_DATA_MAP(CFraudControlConfigurationLink)
	DDX_Control(pDX, IDC_CONFIGURERULE_VIEW_BUTTON, m_ViewButton);
	DDX_Control(pDX, IDC_CONFIGURERULE_UPDATE_BUTTON, m_UpdateButton);
	DDX_Control(pDX, IDC_CONFIGURERULE_LIST_BUTTON, m_ListButton);
	DDX_Control(pDX, IDC_CONFIGURERULE_DELETE_BUTTON, m_DeleteButton);
	DDX_Control(pDX, IDC_CONFIGURERULE_ADD_BUTTON, m_AddButton);
	DDX_Control(pDX, IDC_CONFIGURELINK_NAME, m_CntlFraudRuleFld);
	DDX_Text(pDX, IDC_MESSAGE_LBL, m_StrMessageLbl);
	DDX_Text(pDX, IDC_CONFIGURELINK_NAME, m_StrFraudRuleFld);
	DDV_MaxChars(pDX, m_StrFraudRuleFld, 19);
}


BEGIN_MESSAGE_MAP(CFraudControlConfigurationLink, CPropertyPage) // very imprtnt.
	ON_WM_HELPINFO()
	ON_BN_CLICKED(IDC_CONFIGURERULE_ADD_BUTTON, &CFraudControlConfigurationLink::OnBnClickedConfigureruleAddButton)
	ON_BN_CLICKED(IDC_CONFIGURERULE_DELETE_BUTTON, &CFraudControlConfigurationLink::OnBnClickedConfigureruleDeleteButton)
	ON_BN_CLICKED(IDC_CONFIGURERULE_UPDATE_BUTTON, &CFraudControlConfigurationLink::OnBnClickedConfigureruleUpdateButton)
	ON_BN_CLICKED(IDC_CONFIGURERULE_VIEW_BUTTON, &CFraudControlConfigurationLink::OnBnClickedConfigureruleViewButton)
	ON_BN_CLICKED(IDC_CONFIGURERULE_LIST_BUTTON, &CFraudControlConfigurationLink::OnBnClickedConfigureruleListButton)
END_MESSAGE_MAP()


void CFraudControlConfigurationLink::OnCancel() 
{
	// TODO: Add your specialized code here and/or call the base class

   //clear out all the user fields on the main tab
	
   //clear out all the user fields on the main tab

	if(pFraudControlConfigurationFraudRule->m_bFraudGeneral)
	{
		pFraudControlConfigurationFraudRule->CleanConfigurGeneralDialog();	          
		pFraudControlConfigurationFraudRule->EnableConfigurGeneralDialog(FALSE);       
	}
		if ( pChangeHistoryPage->m_bChangeHistory )
		pChangeHistoryPage->CleanChangeHistory();

	CPropertyPage::OnCancel();
}


BOOL CFraudControlConfigurationLink::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	// TODO: Add your specialized code here and/or call the base class
	
	c_time = CTime::GetCurrentTime();	
	shrmem.SetLastUpdateTime(c_time);
	return CPropertyPage::OnCommand(wParam, lParam);
}

BOOL CFraudControlConfigurationLink::OnInitDialog() 
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

	m_StrFraudRuleFld 		= _T("");
	m_StrMessageLbl	  		= _T("");
  
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
		  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE

	return TRUE;
}


//void CFraudControlConfigurationLink::OnUpdateButton() 
BOOL CFraudControlConfigurationLink::OnApply()
{
	char buf[100]={0};
	BYTE rcode ;
	char strErrorMessage[200]={0};
	CMyUtils myUtilsObject;
	INT length=0;
	if ( m_nDialogMode == MODE_VIEW)
	{
		((CPropertySheet*) this->GetParent())->SetActivePage(0);
		//return CPropertyPage::OnApply();
      return FALSE;
	}
	if (m_nDialogMode != MODE_DONE)
	{
		if ( myUtilsObject.IsEmpty (pFraudControlConfigurationFraudRule->m_StrCard_brand, this)) 
	    {
		    ((CPropertySheet*) this->GetParent())->SetActivePage(1);
            pFraudControlConfigurationFraudRule->m_CntlCard_brand.SetFocus();
		    pFraudControlConfigurationFraudRule->m_StrMessageLbl = "Please Select the Card Brand";
		    pFraudControlConfigurationFraudRule->UpdateData(FALSE);
		    return FALSE;
	    }
		if ( myUtilsObject.IsEmpty (pFraudControlConfigurationFraudRule->m_StrAcq_country_code, this) &&
			myUtilsObject.IsEmpty (pFraudControlConfigurationFraudRule->m_StrBilling_amt, this)&&
			myUtilsObject.IsEmpty (pFraudControlConfigurationFraudRule->m_StrPos_mode_1, this) &&
			 myUtilsObject.IsEmpty (pFraudControlConfigurationFraudRule->m_StrPos_mode_2, this) &&
			 myUtilsObject.IsEmpty (pFraudControlConfigurationFraudRule->m_StrPos_mode_3, this)&&
			 myUtilsObject.IsEmpty (pFraudControlConfigurationFraudRule->m_StrMcc_1, this) &&
			 myUtilsObject.IsEmpty (pFraudControlConfigurationFraudRule->m_StrMcc_2, this) &&
			 myUtilsObject.IsEmpty (pFraudControlConfigurationFraudRule->m_StrMcc_3, this)) 
	    {
		    ((CPropertySheet*) this->GetParent())->SetActivePage(1);
            pFraudControlConfigurationFraudRule->m_CntlAcq_country_code.SetFocus();
		    pFraudControlConfigurationFraudRule->m_StrMessageLbl = "Please Enter one other parameter to complete the rule details";
		    pFraudControlConfigurationFraudRule->UpdateData(FALSE);
		    return FALSE;
	    }
		if(myUtilsObject.IsEmpty (pFraudControlConfigurationFraudRule->m_StrAcq_country_code, this) ==0)
		{
			length = pFraudControlConfigurationFraudRule->m_StrAcq_country_code.GetLength();
			if(length != 3)
	    {
		    ((CPropertySheet*) this->GetParent())->SetActivePage(1);
				pFraudControlConfigurationFraudRule->m_CntlAcq_country_code.SetFocus();
				pFraudControlConfigurationFraudRule->m_StrMessageLbl = "Please Enter Exact 3 bytes for Acq Country code parameter";
		    pFraudControlConfigurationFraudRule->UpdateData(FALSE);
		    return FALSE;
	    }
		}

		if(myUtilsObject.IsEmpty (pFraudControlConfigurationFraudRule->m_StrBilling_amt, this) ==0)
		{
			length = pFraudControlConfigurationFraudRule->m_StrBilling_amt.GetLength();
			if(length > 13)
			{
				((CPropertySheet*) this->GetParent())->SetActivePage(1);
				pFraudControlConfigurationFraudRule->m_CntlBilling_amt.SetFocus();
					pFraudControlConfigurationFraudRule->m_StrMessageLbl = "Please Enter max 10 digits for Billing amount parameter";
				pFraudControlConfigurationFraudRule->UpdateData(FALSE);
				return FALSE;
			}
		}
		if(myUtilsObject.IsEmpty (pFraudControlConfigurationFraudRule->m_StrPos_mode_1, this) ==0)
		{
			length = pFraudControlConfigurationFraudRule->m_StrPos_mode_1.GetLength();
			if(length != 2)
	    {
		    ((CPropertySheet*) this->GetParent())->SetActivePage(1);
            pFraudControlConfigurationFraudRule->m_CntlPos_mode_1.SetFocus();
				pFraudControlConfigurationFraudRule->m_StrMessageLbl = "Please Enter Exact 2 bytes for POS entry Mode 1 parameter";
		    pFraudControlConfigurationFraudRule->UpdateData(FALSE);
		    return FALSE;
	    }
		}
		if(myUtilsObject.IsEmpty (pFraudControlConfigurationFraudRule->m_StrPos_mode_2, this) ==0)
		{
			length = pFraudControlConfigurationFraudRule->m_StrPos_mode_2.GetLength();
			if(length != 2)
			{
				((CPropertySheet*) this->GetParent())->SetActivePage(1);
				pFraudControlConfigurationFraudRule->m_CntlPos_mode_2.SetFocus();
				pFraudControlConfigurationFraudRule->m_StrMessageLbl = "Please Enter Exact 2 bytes for POS entry Mode 2 parameter";
				pFraudControlConfigurationFraudRule->UpdateData(FALSE);
				return FALSE;
			}
		}
		if(myUtilsObject.IsEmpty (pFraudControlConfigurationFraudRule->m_StrPos_mode_3, this) ==0)
		{
			length = pFraudControlConfigurationFraudRule->m_StrPos_mode_3.GetLength();
			if(length != 2)
			{
				((CPropertySheet*) this->GetParent())->SetActivePage(1);
				pFraudControlConfigurationFraudRule->m_CntlPos_mode_3.SetFocus();
				pFraudControlConfigurationFraudRule->m_StrMessageLbl = "Please Enter Exact 2 bytes for POS entry Mode 3 parameter";
				pFraudControlConfigurationFraudRule->UpdateData(FALSE);
				return FALSE;
			}
		}
		if(myUtilsObject.IsEmpty (pFraudControlConfigurationFraudRule->m_StrMcc_1, this) ==0)
		{
			length = pFraudControlConfigurationFraudRule->m_StrMcc_1.GetLength();
			if(length != 4)
	    {
		    ((CPropertySheet*) this->GetParent())->SetActivePage(1);
            pFraudControlConfigurationFraudRule->m_CntlMcc_1.SetFocus();
				pFraudControlConfigurationFraudRule->m_StrMessageLbl = "Please Enter Exact 4 bytes for MCC 1 parameter";
				pFraudControlConfigurationFraudRule->UpdateData(FALSE);
				return FALSE;
			}
		}
		if(myUtilsObject.IsEmpty (pFraudControlConfigurationFraudRule->m_StrMcc_2, this) ==0)
		{
			length = pFraudControlConfigurationFraudRule->m_StrMcc_2.GetLength();
			if(length != 4)
			{
				((CPropertySheet*) this->GetParent())->SetActivePage(1);
				pFraudControlConfigurationFraudRule->m_CntlMcc_2.SetFocus();
				pFraudControlConfigurationFraudRule->m_StrMessageLbl = "Please Enter Exact 4 bytes for MCC 2 parameter";
				pFraudControlConfigurationFraudRule->UpdateData(FALSE);
				return FALSE;
			}
		}
		if(myUtilsObject.IsEmpty (pFraudControlConfigurationFraudRule->m_StrMcc_3, this) ==0)
		{
			length = pFraudControlConfigurationFraudRule->m_StrMcc_3.GetLength();
			if(length != 4)
			{
				((CPropertySheet*) this->GetParent())->SetActivePage(1);
				pFraudControlConfigurationFraudRule->m_CntlMcc_3.SetFocus();
				pFraudControlConfigurationFraudRule->m_StrMessageLbl = "Please Enter Exact 4 bytes for MCC 3 parameter";
		    pFraudControlConfigurationFraudRule->UpdateData(FALSE);
		    return FALSE;
	    }
	}

	}

	if (m_nDialogMode == MODE_EDIT)
	{ 
		memset(&FRULE01,0,sizeof(FRULE01)) ;
		if (!(PopulateDatabaseFromConfigureRuleDialog(&FRULE01)))
        {		
			return FALSE;
		}

		rcode = txdsapi_update_record((pBYTE)&FRULE01, sizeof (FRULE01), FRULE01_DATA, strErrorMessage);  
             
        if (rcode != PTEMSG_OK)
		{   	
			m_StrMessageLbl = strErrorMessage;
            m_nDialogMode = MODE_DONE;

			 ((CPropertySheet*) this->GetParent())->SetWindowText(" Fraud rule Definition");
				//set version info in the title bar
			CString appProdVer;
			CString lpszTitle;
			appProdVer.LoadString(IDS_PRODUCT_VERSION);
			((CPropertySheet*) this->GetParent())->GetWindowText(lpszTitle);
		//	lpszTitle = lpszTitle + " - Version - ";
			lpszTitle += appProdVer;
			((CPropertySheet*) this->GetParent())->SetWindowText(lpszTitle);
		    m_CntlFraudRuleFld.EnableWindow(TRUE);

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

			strncpy(frule,m_StrFraudRuleFld,sizeof(frule));
			pFraudControlConfigurationFraudRule->CompareStructures();
			if(pFraudControlConfigurationFraudRule->m_bFraudGeneral)
			{
				pFraudControlConfigurationFraudRule->CleanConfigurGeneralDialog();	          
				pFraudControlConfigurationFraudRule->EnableConfigurGeneralDialog(FALSE);
			}
            	
            if ( pChangeHistoryPage->m_bChangeHistory )
				pChangeHistoryPage->CleanChangeHistory();
			
			((CPropertySheet*) this->GetParent())->SetActivePage(0);
            ((CPropertySheet*) this->GetParent())->SetWindowText("Fraud rule Definition");	
				//set version info in the title bar
			CString appProdVer;
			CString lpszTitle;
			appProdVer.LoadString(IDS_PRODUCT_VERSION);
			((CPropertySheet*) this->GetParent())->GetWindowText(lpszTitle);
			lpszTitle = lpszTitle + " - Version - ";
			lpszTitle += appProdVer;
			((CPropertySheet*) this->GetParent())->SetWindowText(lpszTitle);
			
			m_CntlFraudRuleFld.EnableWindow(TRUE);
           	SetModified ( FALSE );

            m_nDialogMode = MODE_DONE;
			m_CntlFraudRuleFld.EnableWindow(TRUE);
           // return CPropertyPage::OnApply();
           return FALSE;
        } 
 
	}

	if(m_nDialogMode == MODE_ADD)
	{
		
        memset(&FRULE01,0,sizeof(FRULE01)) ;

        
        if (!(PopulateDatabaseFromConfigureRuleDialog(&FRULE01)))
        {		
			return FALSE;
		}             


		rcode = txdsapi_insert_record((pBYTE)&FRULE01, sizeof (FRULE01), FRULE01_DATA, strErrorMessage);  
             
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
			
			 m_CntlFraudRuleFld.SetFocus(); 
			m_CntlFraudRuleFld.EnableWindow(TRUE);
			if (!UpdateData(FALSE))
            {
				AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
			}
			pFraudControlConfigurationFraudRule->CleanConfigurGeneralDialog();	          
            pFraudControlConfigurationFraudRule->EnableConfigurGeneralDialog(FALSE);                
            	
            if ( pChangeHistoryPage->m_bChangeHistory )
				pChangeHistoryPage->CleanChangeHistory();
			
         m_CntlFraudRuleFld.SetFocus(); 
			m_CntlFraudRuleFld.EnableWindow(TRUE);
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


void CFraudControlConfigurationLink::OnBnClickedConfigureruleAddButton()
{
	// TODO: Add your control notification handler code here
	CMyUtils myUtilsObject;
	char strErrorMessage [200] = {0};   

	m_nDialogMode = MODE_ADD;    
	if (!CheckDialogForCompleteness())
	{
		return ;	
	}
	pFraudControlConfigurationFraudRule->m_nDialogMode = MODE_ADD;
	pChangeHistoryPage->m_nDialogMode = MODE_ADD;

	memcpy ((LPSTR)(FRULE01.fraud_rule), m_StrFraudRuleFld ,sizeof (FRULE01.fraud_rule));

		//check if network with this ID is already exists in NCF01 structure 
    int rcode = txdsapi_get_record((pBYTE)&FRULE01, sizeof (FRULE01), FRULE01_DATA, strErrorMessage);
         
    if(rcode == PTEMSG_OK )
    {
	    m_StrMessageLbl = "ERROR: Rule already exists";
		m_nDialogMode = MODE_DONE;
        
        if (!UpdateData(FALSE))
        {
		    AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
	    }
		    return ;
    }

	if(pFraudControlConfigurationFraudRule->m_bFraudGeneral)
	{
		pFraudControlConfigurationFraudRule->EnableConfigurGeneralDialog(TRUE);
		pFraudControlConfigurationFraudRule->CleanConfigurGeneralDialog();	 
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

	m_CntlFraudRuleFld.EnableWindow(FALSE);
   ((CPropertySheet*) this->GetParent())->SetActivePage(1);
}

void CFraudControlConfigurationLink::OnBnClickedConfigureruleDeleteButton()
{
	// TODO: Add your control notification handler code here
	char strErrorMessage[200] = {0};   
	CString strMessage;
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
    
	strMessage.Format ("Are you sure you want to delete Network ID configuration %s?", (LPCTSTR) m_StrFraudRuleFld) ;	

	nReturnCode = AfxMessageBox ((LPCTSTR)strMessage,MB_OKCANCEL|MB_ICONQUESTION) ;

	if (nReturnCode != IDOK) 
	{
		return ;
	}

	memcpy ((LPSTR)(FRULE01.fraud_rule), m_StrFraudRuleFld ,sizeof (FRULE01.fraud_rule));
 
    int rcode = txdsapi_delete_record((pBYTE)&FRULE01, sizeof (FRULE01), FRULE01_DATA, strErrorMessage);

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
		m_StrFraudRuleFld= _T("");	
		if (!UpdateData(FALSE))
		{
			AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
		}
			if(pFraudControlConfigurationFraudRule->m_bFraudGeneral)
			{
				pFraudControlConfigurationFraudRule->CleanConfigurGeneralDialog();	          
				pFraudControlConfigurationFraudRule->EnableConfigurGeneralDialog(FALSE);
			}
			
		if ( pChangeHistoryPage->m_bChangeHistory )
			pChangeHistoryPage->CleanChangeHistory();

		((CPropertySheet*) this->GetParent())->SetActivePage(0);
		 SetModified ( FALSE );    


		m_CntlFraudRuleFld.EnableWindow(TRUE);

	} 
}


void CFraudControlConfigurationLink::OnBnClickedConfigureruleUpdateButton()
{
	// TODO: Add your control notification handler code here
	char strErrorMessage [200] = {0};   
	HST01 sHistoryRecord;

	m_nDialogMode = MODE_EDIT;    
		pFraudControlConfigurationFraudRule->m_nDialogMode = MODE_EDIT;
	pChangeHistoryPage->m_nDialogMode = MODE_EDIT;
	if (!CheckDialogForCompleteness())
	{
		return ;	
	}
	m_StrMessageLbl= _T("");
	memcpy ((LPSTR)(FRULE01_old.fraud_rule), m_StrFraudRuleFld ,sizeof (FRULE01_old.fraud_rule));

	int rcode = txdsapi_get_record((pBYTE)&FRULE01_old, sizeof (FRULE01), FRULE01_DATA, strErrorMessage);
         
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

    strcpy ( strFilePrimaryKey, m_StrFraudRuleFld);

    memcpy ((LPSTR)(sHistoryRecord.primary_key.unique_id),"                 ",sizeof (sHistoryRecord.primary_key.unique_id));
    memcpy ((LPSTR)(sHistoryRecord.file_name),"FRULE01",	sizeof (sHistoryRecord.file_name));
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

	pFraudControlConfigurationFraudRule->PopulateGeneralDialogFromDatabase (&FRULE01_old);
		m_CntlFraudRuleFld.EnableWindow(FALSE);

			if(pFraudControlConfigurationFraudRule->m_bFraudGeneral)
			{
				
				pFraudControlConfigurationFraudRule->UpdateData(FALSE);  
				pFraudControlConfigurationFraudRule->EnableConfigurGeneralDialog(TRUE);
				
			}
((CPropertySheet*) this->GetParent())->SetActivePage(1);
}


void CFraudControlConfigurationLink::OnBnClickedConfigureruleViewButton()
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

	memcpy ((LPSTR)(FRULE01.fraud_rule), m_StrFraudRuleFld ,sizeof (FRULE01.fraud_rule));

		//check if network with this ID is already exists in NCF01 structure 
    int rcode = txdsapi_get_record((pBYTE)&FRULE01, sizeof (FRULE01), FRULE01_DATA, strErrorMessage);
         
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

	if(pFraudControlConfigurationFraudRule->m_bFraudGeneral)
	{
		pFraudControlConfigurationFraudRule->CleanConfigurGeneralDialog();	          
		pFraudControlConfigurationFraudRule->EnableConfigurGeneralDialog(FALSE);
		pFraudControlConfigurationFraudRule->m_nDialogMode = MODE_VIEW;
	}

	ZeroMemory (&sHistoryRecord, sizeof (sHistoryRecord));

    memset ( strFilePrimaryKey,0,sizeof (strFilePrimaryKey));
    strcpy ( strFilePrimaryKey,m_StrFraudRuleFld);

    memcpy ((LPSTR)(sHistoryRecord.primary_key.unique_id),"                 ",sizeof (sHistoryRecord.primary_key.unique_id));
    memcpy ((LPSTR)(sHistoryRecord.file_name),"FRULE01",sizeof (sHistoryRecord.file_name));
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

		         
				pFraudControlConfigurationFraudRule->PopulateGeneralDialogFromDatabase (&FRULE01);
		pFraudControlConfigurationFraudRule->SetModified(FALSE);
}


void CFraudControlConfigurationLink::OnBnClickedConfigureruleListButton()
{
	// TODO: Add your control notification handler code here
	CNameList listDlg;    
    int nReturnCode;

	m_StrMessageLbl = "";
	UpdateData(TRUE);

    listDlg.m_nIndicator = MODE_FRULE01;
    listDlg.strPrimaryKey1 = m_StrFraudRuleFld;  

	nReturnCode = listDlg.DoModal();

	if ( nReturnCode == IDOK)
        m_StrFraudRuleFld = name;

		if(pFraudControlConfigurationFraudRule->m_bFraudGeneral)
			{
				pFraudControlConfigurationFraudRule->CleanConfigurGeneralDialog();	          
				pFraudControlConfigurationFraudRule->EnableConfigurGeneralDialog(FALSE);
			}
	if ( pChangeHistoryPage->m_bChangeHistory )
		pChangeHistoryPage->CleanChangeHistory();
     
    m_CntlFraudRuleFld.EnableWindow(TRUE);
        
    if (!UpdateData(FALSE))
    {
		AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
	}	
}

BOOL CFraudControlConfigurationLink::CheckDialogForCompleteness()
{
    CMyUtils myUtilsObject;

    if (!UpdateData(TRUE))
	{
		AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
	}

    if (myUtilsObject.IsEmpty (m_StrFraudRuleFld, this))
    {
		m_StrMessageLbl = "Please enter Fraud Rule";	
		m_CntlFraudRuleFld.SetFocus();
		if (!UpdateData(FALSE))
		{
			AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
		}
		m_nDialogMode = MODE_DONE;
		return FALSE;
	}	
   
    return TRUE;
}

BOOL CFraudControlConfigurationLink::PopulateDatabaseFromConfigureRuleDialog( pFRULE01 psFRULE01)
{
	ZeroMemory (psFRULE01, sizeof(FRULE01)) ;
	   
	memcpy ((LPTSTR) psFRULE01->fraud_rule,m_StrFraudRuleFld, sizeof (psFRULE01->fraud_rule));

	pFraudControlConfigurationFraudRule->PopulateDatabaseFromGeneralDialog( psFRULE01 ); 

	return TRUE;
  
}

void CFraudControlConfigurationLink::PopulateHistoryTable()
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

    strcpy ( strPrimaryKey, m_StrFraudRuleFld);
    
    myUtilsObject.GetSystemDate(strDate); 
    myUtilsObject.GetSystemTime(strTime);

    pintime_get_gmt ( dest_str );

    ZeroMemory ( &sHistoryData, sizeof (HST01));

    memcpy ((LPTSTR) sHistoryData.file_name,            (LPCTSTR)"FRULE01",               sizeof (sHistoryData.file_name));
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

BOOL CFraudControlConfigurationLink::PopulateConfigureRuleDialogFromDatabase( pFRULE01 sFRULE01)
{
	if (!pFraudControlConfigurationFraudRule->m_bFraudGeneral)
	{
		memcpy ( &pFraudControlConfigurationFraudRule->FRULE01_old, &sFRULE01, sizeof ( FRULE01));
	}
	else
	{
		pFraudControlConfigurationFraudRule->PopulateGeneralDialogFromDatabase(sFRULE01);
	}

	 return TRUE;
}