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
* $Log:   N:\PVCS\PTE\Equitable\ATP_gui\networkdefinition.cpp  $
   
      Rev 1.13   Dec 01 2003 11:29:54   lmitchel
   Added AMEX Online Updates - scr1200
   
      Rev 1.12   Jul 10 2002 09:56:34   lmitchel
   Ver:4.0.3.1:  Added SendMessage command in PropertySheet files (on_init function) to set the Apply button as the default.  If the enter key is pressed, action will default to  the apply button instead of the OK button.  This allows the user to remain in the current dialog instead of exiting to the main menu.
   
      Rev 1.11   May 21 2002 13:41:02   dirby
   Modified to not exit when <enter> is pressed.  If OK is selected,
   the database gets updated still, but screen is not exited.
   SCR 587
   
      Rev 1.10   Jan 21 2000 12:24:10   iarustam
   Bug fixes # 14,24,,52,63,65,151,165,91,228,202,180,205,504,152,179,251,109,182,177,183,224,225,217,229,230,231,247,200,199,203,201,206,172
   
      Rev 1.9   Jan 13 2000 17:07:48   mbysani
   bug fixed
   
      Rev 1.2   Oct 15 1999 17:25:32   iarustam
   bug fixes
   
      Rev 1.1   Sep 22 1999 16:04:52   iarustam
   Added manual batch entry GUI
   
      Rev 1.0   Jul 01 1999 14:18:42   iarustam
   Initial Release
   
      Rev 1.0   Jun 28 1999 13:59:54   iarustam
   Initial Release
   
      Rev 1.5   May 10 1999 11:31:22   dperson
   Standardized error messages
   
      Rev 1.4   Apr 29 1999 14:57:22   iarustam
   Bug fixes
   
      Rev 1.3   Mar 22 1999 11:13:12   iarustam
   Bug fixes
   
   
      Rev 1.2   Mar 16 1999 16:30:44   iarustam
   Bug fixes
   
      Rev 1.1   Mar 04 1999 13:32:42   iarustam
   Bug fixes
   
      Rev 1.0   Feb 23 1999 11:41:36   iarustam
   Initial Release
   
      Rev 1.0   Feb 17 1999 10:10:22   iarustam
   Initial Release
   
      Rev 1.1   Jan 22 1999 11:00:42   iarustam
   Bug fixes and core 1_3_0 changes
   
      Rev 1.1   Jan 21 1999 13:10:30   iarustam
   Bug fixes
   
   
      Rev 1.1   Dec 18 1998 13:45:20   iarustam
    
*
************************************************************************************/
// NetworkKeyblockDefinition.cpp : implementation file
//

#include "stdafx.h"
#include "atp.h"
#include "NetworkKeyblockDefinition.h"
#include "NetworkKeyblockGeneral.h"


#include "NameList.h"
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
extern CSharedMem shrmem;


/////////////////////////////////////////////////////////////////////////////
// CNetworkKeyblockDefinition property page

IMPLEMENT_DYNCREATE(CNetworkKeyblockDefinition, CPropertyPage)

CNetworkKeyblockDefinition::CNetworkKeyblockDefinition() : CPropertyPage(CNetworkKeyblockDefinition::IDD)
{
	//{{AFX_DATA_INIT(CNetworkDefinition)
	/*m_StrNetworkTypeFld = _T("");*/
	m_StrMessageLbl = _T("");
	m_StrNetworkIdFld = _T("");
	//}}AFX_DATA_INIT
}

CNetworkKeyblockDefinition::~CNetworkKeyblockDefinition()
{
}

void CNetworkKeyblockDefinition::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CNetworkDefinition)
	DDX_Control(pDX, IDC_NETWORK_VIEW_BUTTON, m_ViewButton);
	DDX_Control(pDX, IDC_NETWORK_UPDATE_BUTTON, m_UpdateButton);
	DDX_Control(pDX, IDC_NETWORK_LIST_BUTTON, m_ListButton);
	DDX_Control(pDX, IDC_NETWORK_DELETE_BUTTON, m_DeleteButton);
	DDX_Control(pDX, IDC_NETWORK_ADD_BUTTON, m_AddButton);
	DDX_Control(pDX, IDC_NCF01_KEYBLOCK_NETWORK, m_CntlNetworkIdFld);
	/*DDX_Control(pDX, IDC_NETWORK_TYPE_FLD, m_CntlNetworkTypeFld);
	DDX_Text(pDX, IDC_NETWORK_TYPE_FLD, m_StrNetworkTypeFld);*/
	DDX_Text(pDX, IDC_MESSAGE_LBL, m_StrMessageLbl);
	DDX_Text(pDX, IDC_NCF01_KEYBLOCK_NETWORK, m_StrNetworkIdFld);
	DDV_MaxChars(pDX, m_StrNetworkIdFld, 10);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CNetworkKeyblockDefinition, CPropertyPage)
	//{{AFX_MSG_MAP(CNetworkDefinition)
	ON_BN_CLICKED(IDC_NETWORK_ADD_BUTTON, OnNetworkAddButton)
	ON_BN_CLICKED(IDC_NETWORK_DELETE_BUTTON, OnNetworkDeleteButton)
	ON_BN_CLICKED(IDC_NETWORK_LIST_BUTTON, OnNetworkListButton)
	ON_BN_CLICKED(IDC_NETWORK_UPDATE_BUTTON, OnNetworkUpdateButton)
	ON_BN_CLICKED(IDC_NETWORK_VIEW_BUTTON, OnNetworkViewButton)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CNetworkDefinition message handlers
/******************************************************************************************/
////////////////////////////////////////////////////////////////////////////////////////////
/******************************************************************************************/

BOOL CNetworkKeyblockDefinition::OnInitDialog() 
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
    
    m_StrNetworkIdFld = _T("");
    //m_StrMessageLbl = _T("");

    // set the network type to ACQUIRER, if Acquiring Network is selected
    /*if (m_nNetworkMode == MODE_ACQUIRER)
    { 	
	    m_StrNetworkTypeFld = _T("ACQUIRER");	
    }

    // set the network type to ISSUER, if Issuing Network is selected
    if (m_nNetworkMode == MODE_ISSUER)
    { 	
	    m_StrNetworkTypeFld = _T("ISSUER");		
    }*/

		CString temp;
	int nIndex = -1;

	for ( int i = 0; i <=15; i++ )
	{
		temp = ucf01.control[i].subsystem_name;
		if ( strcmp ( temp, "Network Control" ) == 0)
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
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

/******************************************************************************************/
////////////////////////////////////////////////////////////////////////////////////////////
/******************************************************************************************/

void CNetworkKeyblockDefinition::OnNetworkAddButton() 
{   	 

	NCF01_KEYBLOCK sNetworkkeyblock;
    CMyUtils myUtilsObject;
    char strErrorMessage [200]="";   

    m_StrMessageLbl = ""; 

	m_nDialogMode = MODE_ADD;                                                                                       
    // check if network ID field is not blank
	if (!CheckDialogForCompleteness())
        return ;	
    // set the mode on each page to mode ADD
	

    pChangeHistoryPage->m_nDialogMode = MODE_ADD;

	// set NCF01_KEYBLOCK structure to 0 

	ZeroMemory(&sNetworkkeyblock,sizeof(NCF01_KEYBLOCK));	
    
	//copy the network ID data from control to NCF01 & NCF02 structures
	
	memcpy ((LPSTR)(sNetworkkeyblock.network_id), m_StrNetworkIdFld ,sizeof (sNetworkkeyblock.network_id));
    
         
		pGeneralPage->m_nDialogMode = MODE_ADD;
   
	//check if network with this ID is already exists in NCF01_KEYBLCOK structure 
    int rcode = txdsapi_get_record((pBYTE)&sNetworkkeyblock, sizeof (NCF01_KEYBLOCK), NCF01_KEYBLOCK01_DATA, strErrorMessage);
         
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
          
    
	
	


		
		
	    if ( pGeneralPage->m_bGeneral)
        {

	        //enable controls on the General page
            pGeneralPage->EnableGeneralDialog(TRUE);
            pGeneralPage->CleanGeneralDialog();
        }

       		pGeneralPage->m_StrNetworkIdFld = m_StrNetworkIdFld;



	

    
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

    m_CntlNetworkIdFld.EnableWindow(FALSE);



    if ( pChangeHistoryPage->m_bChangeHistory)
    {  
   	    pChangeHistoryPage->UpdateData(FALSE); 
        pChangeHistoryPage->EnableWindow(FALSE);
        pChangeHistoryPage->CleanChangeHistory();
    }            
	((CPropertySheet*) this->GetParent())->SetActivePage(1);

}

/******************************************************************************************/
////////////////////////////////////////////////////////////////////////////////////////////
/******************************************************************************************/

void CNetworkKeyblockDefinition::OnNetworkDeleteButton() 
{
    CString strMessage;    
    NCF01 sNetworkId;
	NCF02 sNetwork;
	NCF01_KEYBLOCK sNetworkkeyblock;
		
	int nReturnCode;
	char strErrorMessage[200];

    m_nDialogMode = MODE_DONE;

	m_StrMessageLbl = "";

	if (!CheckDialogForCompleteness())
        return;   
   
    if (!UpdateData(TRUE))
    {
		AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);	
	}
    
	strMessage.Format ("Are you sure you want to delete Network ID %s?", (LPCTSTR) m_StrNetworkIdFld) ;	

	nReturnCode = AfxMessageBox ((LPCTSTR)strMessage,MB_OKCANCEL|MB_ICONQUESTION) ;

	if (nReturnCode != IDOK) 
        return ;

/*	memcpy ((LPTSTR)sNetworkId.primary_key.network_id, m_StrNetworkIdFld, sizeof (sNetworkId.primary_key.network_id)) ;
	memcpy ((LPTSTR)sNetwork.primary_key.network_id, m_StrNetworkIdFld, sizeof (sNetwork.primary_key.network_id)) ;*/
	memcpy ((LPTSTR)sNetworkkeyblock.network_id, m_StrNetworkIdFld, sizeof (sNetworkkeyblock.network_id)) ;
	   
 
    int rcode = txdsapi_delete_record ((pBYTE)&sNetworkkeyblock, sizeof (NCF01_KEYBLOCK), NCF01_KEYBLOCK01_DATA, strErrorMessage);

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


         memcpy ((LPSTR)m_strCurrentData, "Record Deleted", sizeof (m_strCurrentData));

         PopulateHistoryTable();
         m_StrMessageLbl = "Record Deleted";
         m_StrNetworkIdFld = _T("");        

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

		 ((CPropertySheet*) this->GetParent())->SetActivePage(0);
		 SetModified ( FALSE );    

		m_CntlNetworkIdFld.EnableWindow(TRUE);
 

		
}

/******************************************************************************************/
////////////////////////////////////////////////////////////////////////////////////////////
/******************************************************************************************/

void CNetworkKeyblockDefinition::OnNetworkListButton() 
{
    CNameList listDlg;    
    int nReturnCode;

	m_StrMessageLbl = "";
	UpdateData(TRUE);

    listDlg.m_nIndicator = MODE_NCF01_KEYBLOCK;
    listDlg.strPrimaryKey1 = m_StrNetworkIdFld;   


   
    nReturnCode = listDlg.DoModal();

    if ( nReturnCode == IDOK)
        m_StrNetworkIdFld = name;

 
	    if ( pGeneralPage->m_bGeneral )
		{
		    pGeneralPage->CleanGeneralDialog();
		    pGeneralPage->EnableGeneralDialog(FALSE);
		} 
       	


    
	if ( pChangeHistoryPage->m_bChangeHistory )
		pChangeHistoryPage->CleanChangeHistory();
     
    m_CntlNetworkIdFld.EnableWindow(TRUE);
        
    if (!UpdateData(FALSE))
    {
		AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
	}	
}

/******************************************************************************************/
////////////////////////////////////////////////////////////////////////////////////////////
/******************************************************************************************/

void CNetworkKeyblockDefinition::OnNetworkUpdateButton() 
{		
    HST01 sHistoryRecord;
	char strErrorMessage[200];
    
    m_nDialogMode = MODE_EDIT;
    /*pTotalsPage->m_nDialogMode = MODE_EDIT;
	pProcessingPage->m_nDialogMode = MODE_EDIT;
	pLogonPage->m_nDialogMode = MODE_EDIT;*/
	pChangeHistoryPage->m_nDialogMode = MODE_EDIT;

	m_StrMessageLbl = "";

    if (!CheckDialogForCompleteness())
        return;
    
    
/*    memcpy ((LPSTR)(sNetworkIdOld.primary_key.network_id),	m_StrNetworkIdFld,sizeof (sNetworkIdOld.primary_key.network_id) );
	memcpy ((LPSTR)(sNetworkOld.primary_key.network_id),	m_StrNetworkIdFld,sizeof (sNetworkOld.primary_key.network_id) );	*/
memcpy ((LPSTR)(sNetworkkeyblockOld.network_id),	m_StrNetworkIdFld,sizeof (sNetworkkeyblockOld.network_id) );



	pGeneralPage->m_nDialogMode = MODE_EDIT;	
	int rcode = txdsapi_get_record((pBYTE)&sNetworkkeyblockOld, sizeof (NCF01_KEYBLOCK), NCF01_KEYBLOCK01_DATA, strErrorMessage);

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



    ZeroMemory (&sHistoryRecord, sizeof (sHistoryRecord));

    memset ( strFilePrimaryKey,0, sizeof (strFilePrimaryKey));

    strcpy ( strFilePrimaryKey, m_StrNetworkIdFld);
    //strcat ( strFilePrimaryKey, m_StrNetworkTypeFld);  
	
	
    memcpy ((LPSTR)(sHistoryRecord.primary_key.unique_id),"                 ",sizeof (sHistoryRecord.primary_key.unique_id));
    memcpy ((LPSTR)(sHistoryRecord.file_name),"NCF01",	sizeof (sHistoryRecord.file_name));
    memcpy ((LPSTR)(sHistoryRecord.file_primary_key), strFilePrimaryKey,	sizeof (sHistoryRecord.file_primary_key));
    memcpy ( &(pChangeHistoryPage->sHistoryRecord),&sHistoryRecord,sizeof(sHistoryRecord));
    
	((CPropertySheet*) this->GetParent())->SetActivePage(1);       
    
	pChangeHistoryPage->bNext = FALSE;

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

	PopulateNetworkKeyblockDialogFromDatabase ( sNetworkkeyblockOld );
    
	m_CntlNetworkIdFld.EnableWindow(FALSE);
    //m_CntlNetworkTypeFld.EnableWindow(FALSE);





		pGeneralPage->SetModified(FALSE);

		if ( pGeneralPage->m_bGeneral)
        {   
			//pGeneralPage->GetEncrType();
            //pGeneralPage->GetStatus();
	        //pGeneralPage->GetPin();	        
 	        pGeneralPage->UpdateData(FALSE); 
            pGeneralPage->EnableGeneralDialog(TRUE);       
        }	  

 


   // if ( pChangeHistoryPage->m_bChangeHistory)
   //     pChangeHistoryPage->PopulateHistoryDialog();	     
            
	((CPropertySheet*) this->GetParent())->SetActivePage(1); 
}

/******************************************************************************************/
////////////////////////////////////////////////////////////////////////////////////////////
/******************************************************************************************/

void CNetworkKeyblockDefinition::OnNetworkViewButton() 
{	
	NCF01 sNetworkId;
	NCF02 sNetwork;
	NCF01_KEYBLOCK	sNetworkkeyblock;
    HST01 sHistoryRecord;
	char strErrorMessage[200];
    
    m_nDialogMode = MODE_VIEW;

	pChangeHistoryPage->m_nDialogMode = MODE_VIEW;
    pChangeHistoryPage->bNext = FALSE;

	m_StrMessageLbl = "";

    if (!CheckDialogForCompleteness())
       return;
    /*memcpy ((LPSTR)(	sNetworkId.primary_key.network_id),	m_StrNetworkIdFld,sizeof (sNetworkId.primary_key.network_id) );
	memcpy ((LPSTR)(	sNetwork.primary_key.network_id),	m_StrNetworkIdFld,sizeof (sNetwork.primary_key.network_id) );*/
		memcpy ((LPSTR)(	sNetworkkeyblock.network_id),	m_StrNetworkIdFld,sizeof (sNetworkkeyblock.network_id) );

    int  rcode = txdsapi_get_record((pBYTE)&sNetworkkeyblock, sizeof (NCF01_KEYBLOCK), NCF01_KEYBLOCK01_DATA, strErrorMessage); 
	if (rcode != PTEMSG_OK)
	{        
       m_StrMessageLbl = strErrorMessage;

	   if (!UpdateData(FALSE))
       {
		   AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
	   }
           return;
    }
	  	       
    ZeroMemory (&sHistoryRecord, sizeof (sHistoryRecord));

    memset ( strFilePrimaryKey,0,sizeof (strFilePrimaryKey));
    strcpy ( strFilePrimaryKey,m_StrNetworkIdFld);
    //strcat ( strFilePrimaryKey,m_StrNetworkTypeFld); 

    memcpy ((LPSTR)(sHistoryRecord.primary_key.unique_id),"                 ",sizeof (sHistoryRecord.primary_key.unique_id));
    memcpy ((LPSTR)(sHistoryRecord.file_name),"NCF01",sizeof (sHistoryRecord.file_name));
    memcpy ((LPSTR)(sHistoryRecord.file_primary_key),strFilePrimaryKey,sizeof (sHistoryRecord.file_primary_key));
    memcpy ( &(pChangeHistoryPage->sHistoryRecord),&sHistoryRecord,sizeof(sHistoryRecord));
    
	((CPropertySheet*) this->GetParent())->SetActivePage(1);
    
		if ( pGeneralPage->m_bGeneral )
		 {
			pGeneralPage->CleanGeneralDialog();
			pGeneralPage->EnableGeneralDialog(FALSE);
			pGeneralPage->m_nDialogMode = MODE_VIEW;
		 }
	
	((CPropertySheet*) this->GetParent())->SetWindowText("View Network ID");
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

	PopulateNetworkKeyblockDialogFromDatabase ( sNetworkkeyblock );
	 pGeneralPage->SetModified(FALSE);    
    
}

/******************************************************************************************/
////////////////////////////////////////////////////////////////////////////////////////////
/******************************************************************************************/

BOOL CNetworkKeyblockDefinition::OnApply() 
{
	NCF01 sNetworkIdDatabaseStructure;
    NCF02 sNetworkDatabaseStructure; 
	NCF01_KEYBLOCK sNetworkKeyblockDatabaseStructure;
	char strErrorMessage[200]="";
	CMyUtils myUtilsObject;

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



	
	if (m_nDialogMode != MODE_DONE)
	{
	
			if ( myUtilsObject.IsEmpty (pGeneralPage->m_Striss_cwk_keyblockFld, this)) 
			{
				((CPropertySheet*) this->GetParent())->SetActivePage(1);
				pGeneralPage->m_Cntliss_cwk_keyblockFld.SetFocus();
				pGeneralPage->m_StrMessageLbl = "Please enter ISS CWK Keyblock";
				pGeneralPage->UpdateData(FALSE);
				return FALSE;
			}

			if ( myUtilsObject.IsEmpty (pGeneralPage->m_Striss_zmk_keyblockFld, this)) 
			{
				((CPropertySheet*) this->GetParent())->SetActivePage(1);
				pGeneralPage->m_Cntliss_zmk_keyblockFld.SetFocus();
				pGeneralPage->m_StrMessageLbl = "Please enter ISS ZMK Keyblock";
				pGeneralPage->UpdateData(FALSE);
				return FALSE;
			}
			if ( myUtilsObject.IsEmpty (pGeneralPage->m_Striss_rtdcwk_keyblockFld, this)) 
			{
				((CPropertySheet*) this->GetParent())->SetActivePage(1);
				pGeneralPage->m_Cntliss_rtdcwk_keyblockFld.SetFocus();
				pGeneralPage->m_StrMessageLbl = "Please enter ISS RTD CWK Keyblock";
				pGeneralPage->UpdateData(FALSE);
				return FALSE;
			}	
			
			if ( myUtilsObject.IsEmpty (pGeneralPage->m_Striss_kek_keyblockFld, this)) 
			{
				((CPropertySheet*) this->GetParent())->SetActivePage(1);
				pGeneralPage->m_Cntliss_kek_keyblockFld.SetFocus();
				pGeneralPage->m_StrMessageLbl = "Please enter ISS KEK Keyblock";
				pGeneralPage->UpdateData(FALSE);
				return FALSE;
			}
			if ( myUtilsObject.IsEmpty (pGeneralPage->m_Stracq_cwk_keyblockFld, this)) 
			{
				((CPropertySheet*) this->GetParent())->SetActivePage(1);
				pGeneralPage->m_Cntlacq_cwk_keyblockFld.SetFocus();
				pGeneralPage->m_StrMessageLbl = "Please enter ACQ CWK Keyblock";
				pGeneralPage->UpdateData(FALSE);
				return FALSE;
			}
			if ( myUtilsObject.IsEmpty (pGeneralPage->m_Stracq_zmk_keyblockFld, this)) 
			{
				((CPropertySheet*) this->GetParent())->SetActivePage(1);
				pGeneralPage->m_Cntlacq_zmk_keyblockFld.SetFocus();
				pGeneralPage->m_StrMessageLbl = "Please enter ACQ ZMK Keyblock";
				pGeneralPage->UpdateData(FALSE);
				return FALSE;
			}
			if ( myUtilsObject.IsEmpty (pGeneralPage->m_Stracq_kek_keyblockFld, this)) 
			{
				((CPropertySheet*) this->GetParent())->SetActivePage(1);
				pGeneralPage->m_Cntlacq_kek_keyblockFld.SetFocus();
				pGeneralPage->m_StrMessageLbl = "Please enter ACQ KEK Keyblock";
				pGeneralPage->UpdateData(FALSE);
				return FALSE;
			}	
	}

    if (m_nDialogMode == MODE_EDIT)
	{ 

		memset(&sNetworkKeyblockDatabaseStructure,0,sizeof(sNetworkKeyblockDatabaseStructure)) ;      
        PopulateDatabaseFromNetworkKeyblockDialog( &sNetworkKeyblockDatabaseStructure );        
	     		
	    int rcode = txdsapi_update_record((pBYTE)&sNetworkKeyblockDatabaseStructure, sizeof (NCF01_KEYBLOCK), NCF01_KEYBLOCK01_DATA, strErrorMessage); 
		if (rcode != PTEMSG_OK)
		{    	
			m_StrMessageLbl = strErrorMessage;
            m_nDialogMode = MODE_DONE;

            ((CPropertySheet*) this->GetParent())->SetActivePage(0);
            ((CPropertySheet*) this->GetParent())->SetWindowText(" Network ID Definition");
				//set version info in the title bar
			CString appProdVer;
			CString lpszTitle;
			appProdVer.LoadString(IDS_PRODUCT_VERSION);
			((CPropertySheet*) this->GetParent())->GetWindowText(lpszTitle);
			lpszTitle = lpszTitle + " - Version - ";
			lpszTitle += appProdVer;
			((CPropertySheet*) this->GetParent())->SetWindowText(lpszTitle);
		    m_CntlNetworkIdFld.EnableWindow(TRUE);

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
				
        } 	  
		
            			
                

            if ( pChangeHistoryPage->m_bChangeHistory )
				pChangeHistoryPage->CleanChangeHistory();
            pChangeHistoryPage->m_nDialogMode = MODE_DONE;

			
 			pGeneralPage->m_nDialogMode = MODE_DONE;
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
			
			m_CntlNetworkIdFld.EnableWindow(TRUE);
           	SetModified ( FALSE );
            m_nDialogMode = MODE_DONE;			
			//return CPropertyPage::OnApply();
            return FALSE;              
    }

	if (m_nDialogMode == MODE_ADD)
    {
        memset(&sNetworkKeyblockDatabaseStructure,0,sizeof(sNetworkKeyblockDatabaseStructure)) ;

        
        if (!(PopulateDatabaseFromNetworkKeyblockDialog( &sNetworkKeyblockDatabaseStructure )))
        {		
			return FALSE;
		}             

		int rcode = txdsapi_insert_record((pBYTE)&sNetworkKeyblockDatabaseStructure, sizeof (NCF01_KEYBLOCK), NCF01_KEYBLOCK01_DATA, strErrorMessage);  
             
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

			
			pGeneralPage->m_nDialogMode = MODE_DONE;
            m_CntlNetworkIdFld.SetFocus(); 
			m_CntlNetworkIdFld.EnableWindow(TRUE);
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

/******************************************************************************************/
////////////////////////////////////////////////////////////////////////////////////////////
/******************************************************************************************/

void CNetworkKeyblockDefinition::OnCancel() 
{
   
	
		if ( pGeneralPage->m_bGeneral )
	    { 
            pGeneralPage->CleanGeneralDialog();
		    pGeneralPage->EnableGeneralDialog(FALSE);           	    			
        }  

	if ( pChangeHistoryPage->m_bChangeHistory )
	    pChangeHistoryPage->CleanChangeHistory();
	
	CPropertyPage::OnCancel();
}

/******************************************************************************************/
////////////////////////////////////////////////////////////////////////////////////////////
/******************************************************************************************/
    
BOOL CNetworkKeyblockDefinition::PopulateDatabaseFromNetworkKeyblockDialog( pNCF01_KEYBLOCK psNetworkKeyblock)
{

	ZeroMemory (psNetworkKeyblock, sizeof(NCF01_KEYBLOCK)) ;
    
	if (m_nDialogMode == MODE_EDIT)
    {
			memcpy (psNetworkKeyblock,&sNetworkkeyblockOld, sizeof(NCF01_KEYBLOCK));

	}
	   
	memcpy ((LPTSTR) psNetworkKeyblock->network_id,m_StrNetworkIdFld, sizeof (psNetworkKeyblock->network_id));

	
	       
   /* if (m_nNetworkMode == MODE_ACQUIRER)
    {              
        memcpy ((LPTSTR) psNetworkId->primary_key.network_type,"A", sizeof (psNetworkId->primary_key.network_type));
		memcpy ((LPTSTR) psNetwork->primary_key.network_type,"A", sizeof (psNetwork->primary_key.network_type));
		pGeneralPage->PopulateDatabaseFromGeneralDialog( psNetworkId ); 
    }

	if (m_nNetworkMode == MODE_ISSUER)
    {              
        memcpy ((LPTSTR) psNetworkId->primary_key.network_type,"I", sizeof (psNetworkId->primary_key.network_type));
		memcpy ((LPTSTR) psNetwork->primary_key.network_type,"I", sizeof (psNetwork->primary_key.network_type));
		pGeneral1Page->PopulateDatabaseFromGeneral1Dialog( psNetworkId ); 
    }*/
		pGeneralPage->PopulateDatabaseFromGeneralDialog( psNetworkKeyblock ); 	    
    return TRUE;
}

/******************************************************************************************/
////////////////////////////////////////////////////////////////////////////////////////////
/******************************************************************************************/

BOOL CNetworkKeyblockDefinition::PopulateNetworkKeyblockDialogFromDatabase( NCF01_KEYBLOCK sNetworKKeyblock )
{



		if (!pGeneralPage->m_bGeneral)
		    memcpy ( &pGeneralPage->sNetworkkeyblockOld, &sNetworKKeyblock, sizeof ( NCF01_KEYBLOCK));
	    else
		    pGeneralPage->PopulateGeneralDialogFromDatabase(sNetworKKeyblock);
	
    return TRUE;
}

/******************************************************************************************/
////////////////////////////////////////////////////////////////////////////////////////////
/******************************************************************************************/

BOOL CNetworkKeyblockDefinition::CheckDialogForCompleteness()
{
    CMyUtils myUtilsObject;

    if (!UpdateData(TRUE))
	{
		AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
	}

    if (myUtilsObject.IsEmpty (m_StrNetworkIdFld, this))
    {
		m_StrMessageLbl = "Please enter Network ID";	
		m_CntlNetworkIdFld.SetFocus();
		if (!UpdateData(FALSE))
		{
			AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
		}
		m_nDialogMode = MODE_DONE;
		return FALSE;
	}	
   
    return TRUE;
}

/******************************************************************************************/
////////////////////////////////////////////////////////////////////////////////////////////
/******************************************************************************************/

void CNetworkKeyblockDefinition::PopulateHistoryTable()
{
    HST01 sHistoryData;
    CHAR dest_str[18];   
	CMyUtils myUtilsObject;
	char strErrorMsg [200] = "";
   CString temp;

    char strDate[9] = "";
    char strTime[7] = "";
    char strPrimaryKey[50] = "";
    CHAR wstation[21]; DWORD wsize;

    memset ( strDate, 0, sizeof (strDate));
    memset ( strTime, 0, sizeof (strTime));
    memset ( strPrimaryKey, 0, sizeof (strPrimaryKey));

    strcpy ( strPrimaryKey, m_StrNetworkIdFld);
    //strcat ( strPrimaryKey, m_StrNetworkTypeFld);
    
    myUtilsObject.GetSystemDate(strDate); 
    myUtilsObject.GetSystemTime(strTime);

    pintime_get_gmt ( dest_str );

    ZeroMemory ( &sHistoryData, sizeof (HST01));
    memcpy ((LPTSTR) sHistoryData.file_name,         (LPCTSTR)"NCF01", sizeof (sHistoryData.file_name));
    temp = strDate;
    temp = temp.Right(4) + temp.Left(2) + temp.Mid(2,2);
    

    wsize = sizeof( wstation );
    GetComputerName( wstation, &wsize );

	memcpy ((LPTSTR)sHistoryData.reserved,(LPTSTR)wstation, sizeof(sHistoryData.reserved));
    memcpy ((LPTSTR) sHistoryData.date_modified, temp, sizeof (sHistoryData.date_modified));
    memcpy ((LPTSTR) sHistoryData.file_primary_key,  (LPCTSTR)strPrimaryKey, sizeof (sHistoryData.file_primary_key));
    memcpy ((LPTSTR) sHistoryData.primary_key.unique_id,     (LPCTSTR)dest_str, sizeof (sHistoryData.primary_key.unique_id));        
    memcpy ((LPTSTR) sHistoryData.time_modified,     (LPCTSTR)strTime, sizeof (sHistoryData.time_modified)); 
             
    memcpy ((LPTSTR)sHistoryData.current_data, (LPCTSTR)m_strCurrentData, sizeof (sHistoryData.current_data));
  //memcpy ((LPTSTR)sHistoryData.previous_data, (LPCTSTR)m_strPreviousData, sizeof (sHistoryData.previous_data));
	memcpy ((LPTSTR)sHistoryData.changed_by, (LPCTSTR)ucf01.primary_key.name, sizeof (sHistoryData.changed_by));
  //memcpy ((LPTSTR)sHistoryData.field_tag, (LPCTSTR)m_strFieldTag, sizeof (sHistoryData.field_tag));
        
    int rcode = txdsapi_insert_record  ( (pBYTE)&sHistoryData, sizeof (HST01), HST01_DATA, strErrorMsg );
	if ( rcode != PTEMSG_OK )
	{
		m_StrMessageLbl = strErrorMsg;
		m_nDialogMode = MODE_DONE;
		UpdateData( FALSE );
	}
    
}




BOOL CNetworkKeyblockDefinition::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	// TODO: Add your specialized code here and/or call the base class
	c_time = CTime::GetCurrentTime();	
	shrmem.SetLastUpdateTime(c_time);
	return CPropertyPage::OnCommand(wParam, lParam);
}
