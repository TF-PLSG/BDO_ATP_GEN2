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
* $Log:   N:\POS\PVCS6.6\EPICPORTZ\PTE\Equitable\ATP_gui\CardsFleetDefinition.cpp  $

  Rev 1.19   Apr 08 2005 08:47:14   lmitchel
  Add delete function to Purchase Card MCC ranges and correcting correct errors from testing
  
	Rev 1.18   Apr 01 2005 10:13:34   lmitchel
	Initial Checking prior to integration testing for updates made to support country/curreny code limits (scr13929), unulimited purchase card MCC ranges, and purchase card transaction limits(scr12785)
	
      Rev 1.17   Jun 04 2004 11:03:18   lmitchel
	  Modified merchant limits table in Authorization Control to add/update/insert MCC limits including new  amount field.  Maximum limit of records changed to 500 from 50.
	  
		Rev 1.16   Dec 01 2003 11:28:50   lmitchel
		Added AMEX Online Updates - scr1200
		
		  Rev 1.15   Apr 30 2003 12:18:36   lmitchel
		  Ver 4.2.0.2 : SCR614 - Allow support for 50 Mercant Type Limits.  Modified Merchant Type Limits dialog - using list control to list all MCC usage codes Auth Profile.  Maximimum number of MCC codes that can be entered is 50 as defined in the database.
		  
			Rev 1.14   Jul 10 2002 09:55:48   lmitchel
			Ver:4.0.3.1:  Added SendMessage command in PropertySheet files (on_init function) to set the Apply button as the default.  If the enter key is pressed, action will default to  the apply button instead of the OK button.  This allows the user to remain in the current dialog instead of exiting to the main menu.
			
			  Rev 1.13   May 21 2002 13:33:04   dirby
			  Modified to not exit when <enter> is pressed.  If OK is selected,
			  the database gets updated still, but screen is not exited.
			  SCR 587
			  
				Rev 1.12   Jan 21 2000 12:22:46   iarustam
				Bug fixes # 14,24,,52,63,65,151,165,91,228,202,180,205,504,152,179,251,109,182,177,183,224,225,217,229,230,231,247,200,199,203,201,206,172
				
				  Rev 1.11   Jan 05 2000 14:53:08   gbarnes
				  Major update. Not updated to PVCS recently.
				  PVCS was locked.
				  
					Rev 1.4   Oct 15 1999 17:06:38   iarustam
					bug fixes
					
					  Rev 1.3   Sep 30 1999 16:16:58   npogosya
					  Bug fixes
					  
						Rev 1.1   Sep 22 1999 15:45:18   iarustam
						Added manual batch entry GUI
						
						  Rev 1.0   Jul 01 1999 14:17:04   iarustam
						  Initial Release
						  
							*
************************************************************************************/
// CardsFleetDefinition.cpp : implementation file
//

#include "stdafx.h"
#include "Atp.h"
#include "CardsFleetDefinition.h"
#include "CardsMerchantLimit.h"
#include "CardsProcessing.h"
#include "CardsTime1.h"
#include "CardsTime2.h"
#include "CardsTime3a.h"
#include "CardsVelocity.h"
#include "CntryAmtLimits.h"
#include "MyUtils.h"
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
int Record_presentIn_key3ds01Flag=1;
//////////////////////////////////////////////////////////
// CCardsFleetDefinition property page

IMPLEMENT_DYNCREATE(CCardsFleetDefinition, CPropertyPage)

CCardsFleetDefinition::CCardsFleetDefinition() : CPropertyPage(CCardsFleetDefinition::IDD)
{
	//{{AFX_DATA_INIT(CCardsFleetDefinition)
	m_StrMessageLbl = _T("");
	m_StrProfileId_Fld = _T("");
	m_StrType_Fld = _T("");
	//}}AFX_DATA_INIT
}

CCardsFleetDefinition::~CCardsFleetDefinition()
{
}

void CCardsFleetDefinition::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCardsFleetDefinition)
	DDX_Control(pDX, IDC_CARDS_VIEW_BUTTON, m_ViewButton);
	DDX_Control(pDX, IDC_CARDS_UPDATE_BUTTON, m_UpdateButton);
	DDX_Control(pDX, IDC_CARDS_LIST_BUTTON, m_ListButton);
	DDX_Control(pDX, IDC_CARDS_DELETE_BUTTON, m_DeleteButton);
	DDX_Control(pDX, IDC_CARDS_ADD_BUTTON, m_AddButton);
	DDX_Control(pDX, IDC_TYPE_FLD, m_CntlType_Fld);
	DDX_Control(pDX, IDC_PROFILE_ID_FLD, m_CntlProfileId_Fld);
	DDX_Text(pDX, IDC_MESSAGE_LBL, m_StrMessageLbl);
	DDX_Text(pDX, IDC_PROFILE_ID_FLD, m_StrProfileId_Fld);
	DDV_MaxChars(pDX, m_StrProfileId_Fld, 4);
	DDX_Text(pDX, IDC_TYPE_FLD, m_StrType_Fld);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CCardsFleetDefinition, CPropertyPage)
//{{AFX_MSG_MAP(CCardsFleetDefinition)
ON_BN_CLICKED(IDC_CARDS_ADD_BUTTON, OnCardsAddButton)
ON_BN_CLICKED(IDC_CARDS_DELETE_BUTTON, OnCardsDeleteButton)
ON_BN_CLICKED(IDC_CARDS_LIST_BUTTON, OnCardsListButton)
ON_BN_CLICKED(IDC_CARDS_UPDATE_BUTTON, OnCardsUpdateButton)
ON_BN_CLICKED(IDC_CARDS_VIEW_BUTTON, OnCardsViewButton)
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////
// CCardsFleetDefinition message handlers

/************************************************************/
////////////////////////////////////////////////////////////
/************************************************************/

BOOL CCardsFleetDefinition::OnInitDialog() 
{  
	CPropertyPage::OnInitDialog();
    CFont m_font;
	CString temp;
	LOGFONT lf;
	int nIndex = -1;
	
	memset ( &lf, 0, sizeof (lf));
	lf.lfHeight = 12;
	lf.lfWeight = 700;
	strcpy(lf.lfFaceName, "Arial");
	m_font.CreateFontIndirect (&lf);
	GetDlgItem(IDC_MESSAGE_LBL)->SetFont (&m_font);
    
    m_StrProfileId_Fld = _T("");
    m_StrMessageLbl = _T("");
    if (m_nCardMode == MODE_FLEET)
    { 	
		m_StrType_Fld = _T("FLEET");	
    }
    if (m_nCardMode == MODE_CREDIT)
    { 	
		m_StrType_Fld = _T("CREDIT");
    }
	if (m_nCardMode == MODE_DEBIT)
    { 	
		m_StrType_Fld = _T("DEBIT");
    }
	
	for (int i = 0; i <=15; i++)
	{
		temp = ucf01.control[i].subsystem_name;
		if ( strcmp(temp, "Authorization Control") == 0)
			nIndex = i;
	}
	if ( nIndex != -1)
	{
		if (ucf01.control[nIndex].action[0] == '0')
			m_AddButton.EnableWindow(FALSE);
		if (ucf01.control[nIndex].action[1] == '0')
			m_DeleteButton.EnableWindow(FALSE);
		if (ucf01.control[nIndex].action[2] == '0')
			m_UpdateButton.EnableWindow(FALSE);
		if (ucf01.control[nIndex].action[3] == '0')
			m_ViewButton.EnableWindow(FALSE);
	}
	
    UpdateData(FALSE);    	
	m_nDialogMode = MODE_DONE;
	
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

/************************************************************/
////////////////////////////////////////////////////////////
/************************************************************/

void CCardsFleetDefinition::OnCardsAddButton() 
{
    CCardsPinVerification pinDlg;
	CMyUtils myUtilsObject;
    CAF01 sProfileId;
    FAF01 sFleetCard;
	CAF01_EMV sEMVProfile;
    KEY3DS01 sKey3ds01;

    char strErrorMessage [200]="";    
	
    m_StrMessageLbl = "";
	
	m_nDialogMode = MODE_ADD;  
	
	if (!CheckDialogForCompleteness())
        return;
	pCardsProfilePage->m_nDialogMode = MODE_ADD;
	pProcessingPage->m_nDialogMode = MODE_ADD;
    pMerchantLimitPage->m_nDialogMode = MODE_ADD;
    pTime1Page->m_nDialogMode = MODE_ADD;
    pTime2Page->m_nDialogMode = MODE_ADD;
    pTime3Page->m_nDialogMode = MODE_ADD;
	pCntryAmtLimitsPage->m_nDialogMode = MODE_ADD;
    pChangeHistoryPage->m_nDialogMode = MODE_ADD;
	
	if ( pProcessingPage->m_bProcessing)
    {
		pProcessingPage->CleanProcessingDialog(); 
		pProcessingPage->GetPinOffsetLocation();
		pProcessingPage->GetPinCaptureIndicator();
		pProcessingPage->GetExpirationDateCheck();
		pProcessingPage->GetExpirationDateLocation();
		pProcessingPage->GetAuthorizationLocation();
		pProcessingPage->GetAuthorizationMethod();
		pProcessingPage->GetStatus();
		pProcessingPage->UpdateData(FALSE); 
		pProcessingPage->EnableProcessingDialog(TRUE);
    }
	
	if( pCardsProfilePage->m_bProcessing)
	{
		pCardsProfilePage->CleanCardsProfileDialog();
		pCardsProfilePage->UpdateData(FALSE);
		pCardsProfilePage->EnableCardsProfileDialog(TRUE);
	}
    if ( pMerchantLimitPage->m_bMerchant)
    {  
		pMerchantLimitPage->CleanMerchantDialog();
		pMerchantLimitPage->UpdateData(FALSE); 
		pMerchantLimitPage->EnableMerchantDialog(TRUE);
    }
	
    if ( pTime1Page->m_bTime1)
    {  
		pTime1Page->CleanTime1Dialog();
   	    pTime1Page->UpdateData(FALSE); 
        pTime1Page->EnableTime1Dialog(TRUE);
    }
	
    if ( pTime2Page->m_bTime2)
    {  
		pTime2Page->CleanTime2Dialog();
		pTime2Page->UpdateData(FALSE); 
		pTime2Page->EnableTime2Dialog(TRUE);
    }
	
	if ( pTime3Page->m_bTime3)
	{  
		pTime3Page->CleanTime3Dialog();
		pTime3Page->UpdateData(FALSE); 
        pTime3Page->EnableTime3Dialog(TRUE);
	}
	
    if ( pCntryAmtLimitsPage->m_bCntryAmtLmt)
	{
		pCntryAmtLimitsPage->CleanCntryLmtsDialog();
		pCntryAmtLimitsPage->UpdateData(FALSE);
		pCntryAmtLimitsPage->EnableCntryAmtLmtDialog(TRUE);
	}
	
	
    if ( pChangeHistoryPage->m_bChangeHistory)
	{  
   	    pChangeHistoryPage->UpdateData(FALSE); 
        pChangeHistoryPage->EnableWindow(FALSE);
	}            
	
	
	ZeroMemory(&sProfileId,sizeof(CAF01));
    ZeroMemory(&sFleetCard,sizeof(FAF01));
	ZeroMemory(&sEMVProfile, sizeof(CAF01_EMV));
	ZeroMemory(&sKey3ds01,sizeof(KEY3DS01));
	
    memcpy ((LPSTR)(sProfileId.primary_key.caf_profile_id),m_StrProfileId_Fld ,sizeof (sProfileId.primary_key.caf_profile_id));
    memcpy ((LPSTR)(sEMVProfile.primary_key.caf_profile_id),m_StrProfileId_Fld ,sizeof (sEMVProfile.primary_key.caf_profile_id));
    memcpy ((LPSTR)(sKey3ds01.caf_profile_id),m_StrProfileId_Fld ,sizeof (sKey3ds01.caf_profile_id));

    if (m_nCardMode == MODE_FLEET)
    { 
        pVelocityPage->m_nDialogMode = MODE_ADD;
        memcpy ((LPSTR)(sProfileId.primary_key.type),"F" ,sizeof (sProfileId.primary_key.type));
        memcpy ((LPSTR)(sEMVProfile.primary_key.type),"F" ,sizeof (sEMVProfile.primary_key.type));
		memcpy ((LPSTR)(sFleetCard.primary_key.caf_profile_id),m_StrProfileId_Fld ,sizeof (sFleetCard.primary_key.caf_profile_id));
    }
    if (m_nCardMode == MODE_CREDIT)
    { 		   
        memcpy ((LPSTR)(sProfileId.primary_key.type),"C" ,sizeof (sProfileId.primary_key.type));  
		memcpy ((LPSTR)(sEMVProfile.primary_key.type),"C" ,sizeof (sEMVProfile.primary_key.type));
		
    }
	if (m_nCardMode == MODE_DEBIT)
    { 		   
        memcpy ((LPSTR)(sProfileId.primary_key.type),"D" ,sizeof (sProfileId.primary_key.type));  
		memcpy ((LPSTR)(sEMVProfile.primary_key.type),"D" ,sizeof (sEMVProfile.primary_key.type));
		
    }
	
    int rcode = txdsapi_get_record ( (pBYTE)&sProfileId, sizeof (CAF01), CAF01_DATA, strErrorMessage );
    if(rcode == PTEMSG_OK )
    {
		m_StrMessageLbl = "ERROR: Profile ID already exists";
        m_nDialogMode = MODE_DONE;
        if (!UpdateData(FALSE))
        {
			AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
		}
		return ;
    }
	
    rcode = txdsapi_get_record  ( (pBYTE)&sFleetCard, sizeof (FAF01), FAF01_DATA, strErrorMessage );
    if(rcode == PTEMSG_OK )
    {
		m_StrMessageLbl = "ERROR: Profile ID already exists";
        //nDialogMode = MODE_DONE;
        if (!UpdateData(FALSE))
        {
			AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
		}
		return ;
    }
	 
	rcode = txdsapi_get_record  ( (pBYTE)&sEMVProfile, sizeof (CAF01_EMV), CAF01_EMV_DATA, strErrorMessage );
    if(rcode == PTEMSG_OK )
    {
		m_StrMessageLbl = "ERROR: Profile ID already exists";
        //nDialogMode = MODE_DONE;
        if (!UpdateData(FALSE))
        {
			AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
		}
		return ;
    }

	rcode = txdsapi_get_record  ( (pBYTE)&sKey3ds01, sizeof (KEY3DS01), KEY3DS01_DATA, strErrorMessage );
    if(rcode == PTEMSG_OK )
    {
		m_StrMessageLbl = "ERROR: Profile ID already exists";
        //nDialogMode = MODE_DONE;
        if (!UpdateData(FALSE))
        {
			AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
		}
		return ;
    }
	
	pCardsProfilePage->m_StrProfileId_Fld = m_StrProfileId_Fld;
    pMerchantLimitPage->m_StrProfileId_Fld = m_StrProfileId_Fld;
    pProcessingPage->m_StrProfileId_Fld = m_StrProfileId_Fld;
    pTime1Page->m_StrProfileId_Fld = m_StrProfileId_Fld;
    pTime2Page->m_StrProfileId_Fld = m_StrProfileId_Fld;
    pTime3Page->m_StrProfileId_Fld = m_StrProfileId_Fld;
	pCntryAmtLimitsPage->m_StrProfileIdFld = m_StrProfileId_Fld;
	
    if (m_nCardMode == MODE_FLEET)
	{         
		pCardsProfilePage->m_StrCardType_Fld = "FLEET";
		pMerchantLimitPage->m_StrType_Fld = "FLEET";         
		pProcessingPage->m_StrType_Fld = "FLEET";        
		pTime1Page->m_StrType_Fld = "FLEET";        
		pTime2Page->m_StrType_Fld = "FLEET";        
		pTime3Page->m_StrType_Fld = "FLEET"; 
		pCntryAmtLimitsPage->m_StrTypeFld = m_StrProfileId_Fld;
		pVelocityPage->m_StrProfileId_Fld = m_StrProfileId_Fld;
		pVelocityPage->m_StrType_Fld = "FLEET";
		
		if ( pVelocityPage->m_bVelocity)
		{  
			pVelocityPage->CleanVelocityDialog();
			pVelocityPage->UpdateData(FALSE); 
			pVelocityPage->EnableVelocityDialog(TRUE);
		}
		
	}
	
    if (m_nCardMode == MODE_CREDIT)
	{      
		pCardsProfilePage->m_StrCardType_Fld = "CREDIT";
		pMerchantLimitPage->m_StrType_Fld = "CREDIT";      
		pProcessingPage->m_StrType_Fld = "CREDIT";    
		pTime1Page->m_StrType_Fld = "CREDIT";    
		pTime2Page->m_StrType_Fld = "CREDIT";    
		pTime3Page->m_StrType_Fld = "CREDIT";
		pCntryAmtLimitsPage->m_StrTypeFld = m_StrProfileId_Fld;
		
    }
	
    if (m_nCardMode == MODE_DEBIT)
	{      
		pCardsProfilePage->m_StrCardType_Fld = "DEBIT";
        pMerchantLimitPage->m_StrType_Fld = "DEBIT";      
        pProcessingPage->m_StrType_Fld = "DEBIT";    
        pTime1Page->m_StrType_Fld = "DEBIT";    
        pTime2Page->m_StrType_Fld = "DEBIT";    
        pTime3Page->m_StrType_Fld = "DEBIT";
		pCntryAmtLimitsPage->m_StrTypeFld = m_StrProfileId_Fld;
		
    }
	
	
    pChangeHistoryPage->bNext = FALSE;      
    
    ((CPropertySheet*) this->GetParent())->SetWindowText("Add Profile ID");
	//set version info in the title bar
	CString appProdVer;
	CString lpszTitle;
	appProdVer.LoadString(IDS_PRODUCT_VERSION);
	((CPropertySheet*) this->GetParent())->GetWindowText(lpszTitle);
	lpszTitle = lpszTitle + " - Version - ";
	lpszTitle += appProdVer;
	((CPropertySheet*) this->GetParent())->SetWindowText(lpszTitle);
	
    m_CntlProfileId_Fld.EnableWindow(FALSE);
    m_CntlType_Fld.EnableWindow(FALSE);
	
	
	((CPropertySheet*) this->GetParent())->SetActivePage(1);
	
}
/************************************************************/
////////////////////////////////////////////////////////////
/************************************************************/

void CCardsFleetDefinition::OnCardsDeleteButton() 
{
	CString strMessage;
    int nReturnCode;
    CAF01 sProfileId;
    FAF01 sFleetCard;
	CAF01_EMV sEMVProfile;
	KEY3DS01 sKey3ds01;
	
	char strErrorMessage[200];
	
    m_nDialogMode = MODE_EDIT;
	
	m_StrMessageLbl = "";
	
	if (!CheckDialogForCompleteness())
        return;   
	
    if (!UpdateData(TRUE))
	{
		AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);	
	}
    
	strMessage.Format ("Are you sure you want to delete Profile ID %s?", (LPCTSTR) m_StrProfileId_Fld) ;	
	
	nReturnCode = AfxMessageBox ((LPCTSTR)strMessage,MB_OKCANCEL|MB_ICONQUESTION) ;
	
	if (nReturnCode != IDOK) 
        return ;
	
	memset (&sEMVProfile, 0, sizeof(sEMVProfile));
	memcpy ((LPTSTR)sEMVProfile.primary_key.caf_profile_id,m_StrProfileId_Fld,sizeof (sEMVProfile.primary_key.caf_profile_id)) ;
	memcpy ((LPTSTR)sProfileId.primary_key.caf_profile_id,m_StrProfileId_Fld,sizeof (sProfileId.primary_key.caf_profile_id)) ;
	memcpy ((LPTSTR)sKey3ds01.caf_profile_id,m_StrProfileId_Fld,sizeof (sKey3ds01.caf_profile_id)) ;


  
	if (m_nCardMode == MODE_CREDIT)
	{
		memcpy ((LPTSTR)sProfileId.primary_key.type,"C",sizeof (sProfileId.primary_key.type)) ;
		memcpy ((LPTSTR)sEMVProfile.primary_key.type,"C",sizeof (sEMVProfile.primary_key.type)) ; 			        
	}
	if (m_nCardMode == MODE_DEBIT)
	{   	
		memcpy ((LPTSTR)sProfileId.primary_key.type,"D",sizeof (sProfileId.primary_key.type)) ;
		memcpy ((LPTSTR)sEMVProfile.primary_key.type,"D",sizeof (sEMVProfile.primary_key.type)) ;
	}
	
	if (m_nCardMode == MODE_FLEET)
	{    
		memcpy ((LPTSTR)sProfileId.primary_key.type,"F",sizeof (sProfileId.primary_key.type)) ;
		memcpy ((LPTSTR)sFleetCard.primary_key.caf_profile_id,m_StrProfileId_Fld,sizeof (sFleetCard.primary_key.caf_profile_id)) ;
		
		memcpy ((LPTSTR)sEMVProfile.primary_key.type,"F",sizeof (sEMVProfile.primary_key.type)) ;
		
		int rcode = txdsapi_delete_record ((pBYTE)&sFleetCard,
			sizeof (FAF01), FAF01_DATA, strErrorMessage);
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
	}
    
    int rcode = txdsapi_delete_record ((pBYTE)&sProfileId,
        sizeof (CAF01), CAF01_DATA, strErrorMessage);
	
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
		rcode = txdsapi_delete_record ((pBYTE)&sEMVProfile,
			sizeof (CAF01_EMV), CAF01_EMV_DATA, strErrorMessage);
		
		if(rcode != PTEMSG_OK )    
		{
			
		}  

		rcode = txdsapi_delete_record ((pBYTE)&sKey3ds01,
			sizeof (KEY3DS01), KEY3DS01_DATA, strErrorMessage);
		
		if(rcode != PTEMSG_OK )    
		{
			
		}   
        memcpy ((LPSTR)m_strCurrentData,"Record Deleted", sizeof (m_strCurrentData));
		
        PopulateHistoryTable();
		
		m_StrProfileId_Fld = _T(""); 
		m_StrMessageLbl = "Record Deleted";
		
		if (!UpdateData(FALSE))
		{
			AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
		}
		
		if ( pProcessingPage->m_bProcessing )
		{
			pProcessingPage->CleanProcessingDialog();
			pProcessingPage->EnableProcessingDialog(FALSE);
		}
		
        if ( pMerchantLimitPage->m_bMerchant )
		{
			pMerchantLimitPage->CleanMerchantDialog();
			pMerchantLimitPage->EnableMerchantDialog(FALSE);
		}
		if(pCardsProfilePage->m_bProcessing)
		{
			pCardsProfilePage->CleanCardsProfileDialog();
			pCardsProfilePage->EnableCardsProfileDialog(FALSE);
		}
		
		if (pCntryAmtLimitsPage->m_bCntryAmtLmt )
		{
			pCntryAmtLimitsPage->CleanCntryLmtsDialog();
			pCntryAmtLimitsPage->EnableCntryAmtLmtDialog(FALSE);
		}
		
        if ( pTime1Page->m_bTime1 )
		{
			pTime1Page->CleanTime1Dialog();
			pTime1Page->EnableTime1Dialog(FALSE);
		}			
		
        if ( pTime2Page->m_bTime2 )
		{
			pTime2Page->CleanTime2Dialog();
			pTime2Page->EnableTime2Dialog(FALSE);
		}
		
        if ( pTime3Page->m_bTime3 )
		{
			pTime3Page->CleanTime3Dialog();
			pTime3Page->EnableTime3Dialog(FALSE);
		}
		if (m_nCardMode == MODE_FLEET)
		{	
			if ( pVelocityPage->m_bVelocity )
			{
				pVelocityPage->CleanVelocityDialog();
				pVelocityPage->EnableVelocityDialog(FALSE);
			}
		}	
		
		if ( pChangeHistoryPage->m_bChangeHistory )
			pChangeHistoryPage->CleanChangeHistory();
		
		m_nDialogMode = MODE_DONE;
		
		((CPropertySheet*) this->GetParent())->SetActivePage(0);
		SetModified ( FALSE );    
	} 
	
	m_CntlProfileId_Fld.EnableWindow(TRUE);
	
	
}


/************************************************************/
////////////////////////////////////////////////////////////
/************************************************************/

void CCardsFleetDefinition::OnCardsListButton() 
{
    CNameList listDlg;    
    int nReturnCode;
	
	m_StrMessageLbl = "";
	
	UpdateData(TRUE);
	
    listDlg.m_nIndicator = MODE_AUTH;
    listDlg.strPrimaryKey1 = m_StrProfileId_Fld;
	
	if (m_nCardMode == MODE_FLEET)
		listDlg.strBinType = "F";
    else if (m_nCardMode == MODE_CREDIT)
		listDlg.strBinType = "C";
	else listDlg.strBinType = "D";
	
    nReturnCode = listDlg.DoModal();
	
    if ( nReturnCode == IDOK)
        m_StrProfileId_Fld = name;
	
    if (pProcessingPage->m_bProcessing)
	{
		pProcessingPage->CleanProcessingDialog();
		pProcessingPage->EnableProcessingDialog(FALSE);
		
	}			
    if (pMerchantLimitPage->m_bMerchant)
    {
		pMerchantLimitPage->CleanMerchantDialog();	    
        pMerchantLimitPage->EnableMerchantDialog(FALSE);
    }
	if (pCardsProfilePage->m_bProcessing)
	{
		pCardsProfilePage->CleanCardsProfileDialog();
		pCardsProfilePage->EnableCardsProfileDialog(FALSE);
	}
    if ( pCntryAmtLimitsPage->m_bCntryAmtLmt)
	{
		pCntryAmtLimitsPage->CleanCntryLmtsDialog();
		pCntryAmtLimitsPage->EnableCntryAmtLmtDialog(FALSE);
	}
    if (pTime1Page->m_bTime1)
    {
		pTime1Page->CleanTime1Dialog();	     
        pTime1Page->EnableTime1Dialog(FALSE);
    }
    if (pTime2Page->m_bTime2)
    {
		pTime2Page->CleanTime2Dialog();              
		pTime2Page->EnableTime2Dialog(FALSE);
    }
    if (pTime3Page->m_bTime3)
    {	    
		pTime3Page->CleanTime3Dialog();
        pTime3Page->EnableTime3Dialog(FALSE);
    }
	if (m_nCardMode == MODE_FLEET)
	{
		if (pVelocityPage->m_bVelocity)
		{
			pVelocityPage->CleanVelocityDialog();
			pVelocityPage->EnableVelocityDialog(FALSE);
		}
	}
	
    m_CntlProfileId_Fld.EnableWindow(TRUE);
	
    if (!UpdateData(FALSE))
    {
		AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
	}		
	
}

/************************************************************/
////////////////////////////////////////////////////////////
/************************************************************/

void CCardsFleetDefinition::OnCardsUpdateButton() 
{    
    HST01 sHistoryRecord;
	char strErrorMessage[200];
    
    m_nDialogMode = MODE_EDIT;
	pCardsProfilePage->m_nDialogMode = MODE_EDIT;
	pProcessingPage->m_nDialogMode = MODE_EDIT;
    pMerchantLimitPage->m_nDialogMode = MODE_EDIT;
    pTime1Page->m_nDialogMode = MODE_EDIT;
    pTime2Page->m_nDialogMode = MODE_EDIT;
    pTime3Page->m_nDialogMode = MODE_EDIT;
    pCntryAmtLimitsPage->m_nDialogMode = MODE_EDIT;
	pChangeHistoryPage->m_nDialogMode = MODE_EDIT;
	
	m_StrMessageLbl = "";
	
	if (!CheckDialogForCompleteness())
        return;
	memset(&sEMVProfileOld,0,sizeof(sEMVProfileOld));
		memset(&sKey3ds01Old,0,sizeof(sKey3ds01Old));
	
    memcpy ((LPSTR)(sProfileIdOld.primary_key.caf_profile_id),
		      m_StrProfileId_Fld,sizeof (sProfileIdOld.primary_key.caf_profile_id) );
	
    memcpy ((LPSTR)(sEMVProfileOld.primary_key.caf_profile_id),
		      m_StrProfileId_Fld,sizeof (sEMVProfileOld.primary_key.caf_profile_id) );

		    memcpy ((LPSTR)(sKey3ds01Old.caf_profile_id),
		      m_StrProfileId_Fld,sizeof (sKey3ds01Old.caf_profile_id) );
	
    if (m_nCardMode == MODE_FLEET)
    {     
		pVelocityPage->m_nDialogMode = MODE_EDIT; 
		
		memcpy((LPSTR)(sEMVProfileOld.primary_key.type),
			"F",sizeof (sEMVProfileOld.primary_key.type));
        memcpy ((LPSTR)(sProfileIdOld.primary_key.type),
			"F",sizeof (sProfileIdOld.primary_key.type));
		memcpy ((LPSTR)(sFleetCardOld.primary_key.caf_profile_id),
			m_StrProfileId_Fld,sizeof (sFleetCardOld.primary_key.caf_profile_id));
		
        int  rcode = txdsapi_get_record  ( (pBYTE)&sFleetCardOld,
			sizeof (FAF01), FAF01_DATA, strErrorMessage );
		if (rcode != PTEMSG_OK)
		{        
			m_StrMessageLbl = "ERROR: Profile ID does not exist";
			m_nDialogMode = MODE_DONE;
			if (!UpdateData(FALSE))
			{
				AfxMessageBox (IDS_UPDATEDATAFAILED,
					MB_ICONSTOP);		
			}
			return;
		}
    }
    if (m_nCardMode == MODE_CREDIT)
    {         
		memcpy((LPSTR)(sProfileIdOld.primary_key.type),
			"C",sizeof (sProfileIdOld.primary_key.type));	      
	
		memcpy((LPSTR)(sEMVProfileOld.primary_key.type),
			"C",sizeof (sEMVProfileOld.primary_key.type));	      
    }
	if (m_nCardMode == MODE_DEBIT)
    {         
		memcpy((LPSTR)(sProfileIdOld.primary_key.type),
			"D",sizeof (sProfileIdOld.primary_key.type));
		
		memcpy((LPSTR)(sEMVProfileOld.primary_key.type),
			"D",sizeof (sEMVProfileOld.primary_key.type));
    }
    int rcode = txdsapi_get_record((pBYTE)&sProfileIdOld,
		sizeof (CAF01),CAF01_DATA, strErrorMessage);
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
	rcode = txdsapi_get_record((pBYTE)&sEMVProfileOld,
		sizeof (CAF01_EMV),CAF01_EMV_DATA, strErrorMessage);
	if (rcode != PTEMSG_OK)
	{        
	
    }

		rcode = txdsapi_get_record((pBYTE)&sKey3ds01Old,
		sizeof (KEY3DS01),KEY3DS01_DATA, strErrorMessage);

 		if (rcode != PTEMSG_OK)
	{   
		Record_presentIn_key3ds01Flag=0;
	
    }
    ZeroMemory (&sHistoryRecord, sizeof (sHistoryRecord));
	
    memset ( strFilePrimaryKey, 0,sizeof (strFilePrimaryKey));
	
    strcpy ( strFilePrimaryKey, (char *)sProfileIdOld.primary_key.caf_profile_id);
    strcat ( strFilePrimaryKey, m_StrType_Fld); 
	
    memcpy ((LPSTR)(sHistoryRecord.primary_key.unique_id),"                 ",sizeof (sHistoryRecord.primary_key.unique_id));
    memcpy ((LPSTR)(sHistoryRecord.file_name),"CAF01",sizeof (sHistoryRecord.file_name));
    memcpy ((LPSTR)(sHistoryRecord.file_primary_key),strFilePrimaryKey,sizeof (sHistoryRecord.file_primary_key));
    memcpy ( &(pChangeHistoryPage->sHistoryRecord),&sHistoryRecord,sizeof(sHistoryRecord));
    
	if (pProcessingPage->m_bProcessing)
    {    
		pProcessingPage->UpdateData(FALSE); 
		pProcessingPage->EnableProcessingDialog(TRUE);       
    }
    if (pMerchantLimitPage->m_bMerchant)
    {  
		pMerchantLimitPage->UpdateData(FALSE); 
        pMerchantLimitPage->EnableMerchantDialog(TRUE);        
    }
	if (pCardsProfilePage->m_bProcessing)
	{
		pCardsProfilePage->UpdateData(FALSE);
		pCardsProfilePage->EnableCardsProfileDialog(TRUE);
	}
    if (pTime1Page->m_bTime1)
    {  
		pTime1Page->UpdateData(FALSE); 
        pTime1Page->EnableTime1Dialog(TRUE);        
    }
    if (pTime2Page->m_bTime2)
    {  
   	    pTime2Page->UpdateData(FALSE); 
        pTime2Page->EnableTime2Dialog(TRUE);        
    }
    if (pTime3Page->m_bTime3)
    {  
   	    pTime3Page->UpdateData(FALSE); 
        pTime3Page->EnableTime3Dialog(TRUE); 
    }
	if (pCntryAmtLimitsPage->m_bCntryAmtLmt)
	{
		pCntryAmtLimitsPage->UpdateData(FALSE);
		pCntryAmtLimitsPage->EnableCntryAmtLmtDialog(TRUE);
	}
	
    if (m_nCardMode == MODE_FLEET)
    {
		if (pVelocityPage->m_bVelocity)
		{  
			pVelocityPage->UpdateData(FALSE); 
			pVelocityPage->EnableVelocityDialog(TRUE);
        }
    }
    if ( pChangeHistoryPage->m_bChangeHistory)
        pChangeHistoryPage->PopulateHistoryDialog();    
	
	((CPropertySheet*) this->GetParent())->SetActivePage(1);        
    
	pChangeHistoryPage->bNext = FALSE;
	
	((CPropertySheet*) this->GetParent())->SetWindowText("Update Profile ID");	
	//set version info in the title bar
	CString appProdVer;
	CString lpszTitle;
	appProdVer.LoadString(IDS_PRODUCT_VERSION);
	((CPropertySheet*) this->GetParent())->GetWindowText(lpszTitle);
	lpszTitle = lpszTitle + " - Version - ";
	lpszTitle += appProdVer;
	((CPropertySheet*) this->GetParent())->SetWindowText(lpszTitle);
	
	PopulateProfileIdDialogFromDatabase (sProfileIdOld, sFleetCardOld, sEMVProfileOld,sKey3ds01Old);
	
    pProcessingPage->SetModified(FALSE);
	
	m_CntlProfileId_Fld.EnableWindow(FALSE);
    m_CntlType_Fld.EnableWindow(FALSE);        	
}

/************************************************************/
////////////////////////////////////////////////////////////
/************************************************************/

void CCardsFleetDefinition::OnCardsViewButton() 
{
/*CAF01 sProfileId;
FAF01 sFleetCard;
HST01 sHistoryRecord;
char strErrorMessage[200];

  m_nDialogMode = MODE_VIEW;
  pProcessingPage->m_nDialogMode = MODE_VIEW;
  pMerchantLimitPage->m_nDialogMode = MODE_VIEW;      
  pTime1Page->m_nDialogMode = MODE_VIEW;
  pTime2Page->m_nDialogMode = MODE_VIEW;
  pTime3Page->m_nDialogMode = MODE_VIEW;	
  pChangeHistoryPage->m_nDialogMode = MODE_VIEW;	
  
	m_StrMessageLbl = "";
	
	  if (!CheckDialogForCompleteness())
	  return;
	  
		memcpy ((LPSTR)(sProfileId.primary_key.caf_profile_id),
		m_StrProfileId_Fld, sizeof(sProfileId.primary_key.caf_profile_id));
		if (m_nCardMode == MODE_FLEET)
		{         
		memcpy ((LPSTR)(sProfileId.primary_key.type),
		"F", sizeof(sProfileId.primary_key.type));
		memcpy ((LPSTR)(sFleetCard.primary_key.caf_profile_id),
		m_StrProfileId_Fld, sizeof(sFleetCard.primary_key.caf_profile_id));
        int  rcode = txdsapi_get_record((pBYTE)&sFleetCard,
		sizeof(FAF01), FAF01_DATA, strErrorMessage );
		if(PTEMSG_OK != rcode)
		{        
		m_StrMessageLbl = "ERROR: Profile ID does not exist";
		if (!UpdateData(FALSE))
		{
		AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
		}
		return;
		} 
		}
		if (m_nCardMode == MODE_CREDIT)
		{        
		memcpy ((LPSTR)(sProfileId.primary_key.type),
		"C",sizeof (sProfileId.primary_key.type));	 	
		}
		if (m_nCardMode == MODE_DEBIT)
		{        
		memcpy ((LPSTR)(sProfileId.primary_key.type),
		"D",sizeof (sProfileId.primary_key.type));	 	
		}
		int rcode = txdsapi_get_record((pBYTE)&sProfileId,
		sizeof (CAF01), CAF01_DATA, strErrorMessage );
		if (rcode != PTEMSG_OK)
		{        
		m_StrMessageLbl = "ERROR: Profile ID does not exist";
		if (!UpdateData(FALSE))
        {
		AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
		}
        return;
		}       
		
		  if (pProcessingPage->m_bProcessing)
		  {
		  pProcessingPage->CleanProcessingDialog();
		  pProcessingPage->EnableProcessingDialog(FALSE);		   
		  }			
		  if (pMerchantLimitPage->m_bMerchant)
		  {
		  pMerchantLimitPage->CleanMerchantDialog();
		  pMerchantLimitPage->EnableMerchantDialog(FALSE);		
		  }			
		  if (pTime1Page->m_bTime1)
		  {
		  pTime1Page->CleanTime1Dialog();
		  pTime1Page->EnableTime1Dialog(FALSE);		
		  }
		  if (pTime2Page->m_bTime2)
		  {
		  pTime2Page->CleanTime2Dialog();
		  pTime2Page->EnableTime2Dialog(FALSE);		
		  }		
		  if (pTime3Page->m_bTime3)
		  {
		  pTime3Page->CleanTime3Dialog();
		  pTime3Page->EnableTime3Dialog(FALSE);		
		  }
		  if (m_nCardMode == MODE_FLEET)
		  {	
		  if (pVelocityPage->m_bVelocity)
		  {
		  pVelocityPage->CleanVelocityDialog();
		  pVelocityPage->EnableVelocityDialog(FALSE);
		  pVelocityPage->m_nDialogMode = MODE_VIEW;
		  }
		  }
		  
			if ( pChangeHistoryPage->m_bChangeHistory)
			pChangeHistoryPage->PopulateHistoryDialog();    
			ZeroMemory (&sHistoryRecord, sizeof (sHistoryRecord));
			
			  memset ( strFilePrimaryKey,0,sizeof (strFilePrimaryKey));
			  
				strcpy ( strFilePrimaryKey,(char *)sProfileId.primary_key.caf_profile_id);
				strcat ( strFilePrimaryKey, m_StrType_Fld); // fix for bug 44
				
				  memcpy ((LPSTR)(sHistoryRecord.primary_key.unique_id),"                 ",sizeof (sHistoryRecord.primary_key.unique_id));
				  memcpy ((LPSTR)(sHistoryRecord.file_name),"CAF01",sizeof (sHistoryRecord.file_name));
				  memcpy ((LPSTR)(sHistoryRecord.file_primary_key),strFilePrimaryKey,sizeof (sHistoryRecord.file_primary_key));
				  memcpy ( &(pChangeHistoryPage->sHistoryRecord),&sHistoryRecord,sizeof(sHistoryRecord));
				  
					((CPropertySheet*) this->GetParent())->SetActivePage(1);
					pChangeHistoryPage->bNext = FALSE;
					
					  
						((CPropertySheet*) this->GetParent())->SetWindowText("View Profile ID");	
						
						  PopulateProfileIdDialogFromDatabase (sProfileId, sFleetCard);
						  
							m_CntlProfileId_Fld.EnableWindow(TRUE);
							m_CntlType_Fld.EnableWindow(TRUE);        	
							
							  pProcessingPage->SetModified(FALSE);  
*/
	HST01 sHistoryRecord;
	char strErrorMessage[200];

	m_nDialogMode = MODE_VIEW;
	pCardsProfilePage->m_nDialogMode = MODE_VIEW;
	pProcessingPage->m_nDialogMode = MODE_VIEW;
	pMerchantLimitPage->m_nDialogMode = MODE_VIEW;
	pTime1Page->m_nDialogMode = MODE_VIEW;
	pTime2Page->m_nDialogMode = MODE_VIEW;
	pTime3Page->m_nDialogMode = MODE_VIEW;
	pCntryAmtLimitsPage->m_nDialogMode = MODE_VIEW;
	pChangeHistoryPage->m_nDialogMode = MODE_VIEW;

	m_StrMessageLbl = "";

	if (!CheckDialogForCompleteness())
	return;

	memset(&sEMVProfileOld, 0, sizeof(sEMVProfileOld));
	memset(&sKey3ds01Old, 0, sizeof(sKey3ds01Old));

	memcpy ((LPSTR)(sProfileIdOld.primary_key.caf_profile_id),
				  m_StrProfileId_Fld,sizeof (sProfileIdOld.primary_key.caf_profile_id) );

	memcpy((LPSTR) (sEMVProfileOld.primary_key.caf_profile_id)
		   , m_StrProfileId_Fld, sizeof (sEMVProfileOld.primary_key.caf_profile_id));
	memcpy((LPSTR) (sKey3ds01Old.caf_profile_id)
		   , m_StrProfileId_Fld, sizeof (sKey3ds01Old.caf_profile_id));

	if (m_nCardMode == MODE_FLEET)
	{     
		pVelocityPage->m_nDialogMode = MODE_VIEW; 
		
		memcpy((LPSTR)(sEMVProfileOld.primary_key.type),
			"F",sizeof (sEMVProfileOld.primary_key.type));
		memcpy ((LPSTR)(sProfileIdOld.primary_key.type),
			"F",sizeof (sProfileIdOld.primary_key.type));
		memcpy ((LPSTR)(sFleetCardOld.primary_key.caf_profile_id),
			m_StrProfileId_Fld,sizeof (sFleetCardOld.primary_key.caf_profile_id));
		
		int  rcode = txdsapi_get_record  ( (pBYTE)&sFleetCardOld,
			sizeof (FAF01), FAF01_DATA, strErrorMessage );
		if (rcode != PTEMSG_OK)
		{        
			m_StrMessageLbl = "ERROR: Profile ID does not exist";
			m_nDialogMode = MODE_DONE;
			
			if (!UpdateData(FALSE))
			{
				AfxMessageBox (IDS_UPDATEDATAFAILED,
					MB_ICONSTOP);		
			}
			return;
		}
	}
	if (m_nCardMode == MODE_CREDIT)
	{         
		memcpy((LPSTR)(sProfileIdOld.primary_key.type),
			"C",sizeof (sProfileIdOld.primary_key.type));
		memcpy((LPSTR)(sEMVProfileOld.primary_key.type),
			"C",sizeof (sEMVProfileOld.primary_key.type));   
	}
	if (m_nCardMode == MODE_DEBIT)
	{         
		memcpy((LPSTR)(sProfileIdOld.primary_key.type),
			"D",sizeof (sProfileIdOld.primary_key.type));
		   memcpy((LPSTR)(sEMVProfileOld.primary_key.type),
					 "D",sizeof (sEMVProfileOld.primary_key.type));
	}
	int rcode = txdsapi_get_record((pBYTE)&sProfileIdOld,
								   sizeof (CAF01),CAF01_DATA, strErrorMessage);
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

	rcode = txdsapi_get_record((pBYTE)&sEMVProfileOld,
							   sizeof (CAF01_EMV),CAF01_EMV_DATA, strErrorMessage);
	if (rcode != PTEMSG_OK)
	{        
	
	}

	rcode = txdsapi_get_record((pBYTE)&sKey3ds01Old,
							   sizeof (KEY3DS01),KEY3DS01_DATA, strErrorMessage);
	if (rcode != PTEMSG_OK)
	{        
		rcode = txdsapi_insert_record  ((pBYTE)&sKey3ds01Old,
			sizeof (KEY3DS01), KEY3DS01_DATA, strErrorMessage); 
			if (rcode != PTEMSG_OK)
			{   	
			                 
			}
	}

	ZeroMemory (&sHistoryRecord, sizeof (sHistoryRecord));

	memset ( strFilePrimaryKey, 0,sizeof (strFilePrimaryKey));

	strcpy ( strFilePrimaryKey, (char *)sProfileIdOld.primary_key.caf_profile_id);
	strcat ( strFilePrimaryKey, m_StrType_Fld); 

	memcpy ((LPSTR)(sHistoryRecord.primary_key.unique_id),"                 ",sizeof (sHistoryRecord.primary_key.unique_id));
	memcpy ((LPSTR)(sHistoryRecord.file_name),"CAF01",sizeof (sHistoryRecord.file_name));
	memcpy ((LPSTR)(sHistoryRecord.file_primary_key),strFilePrimaryKey,sizeof (sHistoryRecord.file_primary_key));
	memcpy ( &(pChangeHistoryPage->sHistoryRecord),&sHistoryRecord,sizeof(sHistoryRecord));

	if (pProcessingPage->m_bProcessing)
	{    
		pProcessingPage->UpdateData(FALSE); 
		pProcessingPage->EnableProcessingDialog(FALSE);       
	}
	if (pCardsProfilePage->m_bProcessing)
	{
		pCardsProfilePage->UpdateData(FALSE);
		pCardsProfilePage->EnableCardsProfileDialog(FALSE);
	}
	if (pMerchantLimitPage->m_bMerchant)
	{  
		pMerchantLimitPage->UpdateData(FALSE); 
		pMerchantLimitPage->EnableMerchantDialog(FALSE);        
	}
	if (pTime1Page->m_bTime1)
	{  
		pTime1Page->UpdateData(FALSE); 
		pTime1Page->EnableTime1Dialog(FALSE);        
	}
	if (pTime2Page->m_bTime2)
	{  
		pTime2Page->UpdateData(FALSE); 
		pTime2Page->EnableTime2Dialog(FALSE);        
	}
	if (pTime3Page->m_bTime3)
	{  
		pTime3Page->UpdateData(FALSE); 
		pTime3Page->EnableTime3Dialog(FALSE);        
	}
	if (pCntryAmtLimitsPage->m_bCntryAmtLmt)
	{
		pCntryAmtLimitsPage->UpdateData(FALSE);
		pCntryAmtLimitsPage->EnableCntryAmtLmtDialog(FALSE);
	}
	if (m_nCardMode == MODE_FLEET)
	{
		if (pVelocityPage->m_bVelocity)
		{  
			pVelocityPage->UpdateData(FALSE); 
			pVelocityPage->EnableVelocityDialog(FALSE);
		}
	}
	if ( pChangeHistoryPage->m_bChangeHistory)
	pChangeHistoryPage->PopulateHistoryDialog();    

	((CPropertySheet*) this->GetParent())->SetActivePage(1);        

	pChangeHistoryPage->bNext = FALSE;

	((CPropertySheet*) this->GetParent())->SetWindowText("View Profile ID");	
	//set version info in the title bar
	CString appProdVer;
	CString lpszTitle;
	appProdVer.LoadString(IDS_PRODUCT_VERSION);
	((CPropertySheet*) this->GetParent())->GetWindowText(lpszTitle);
	lpszTitle = lpszTitle + " - Version - ";
	lpszTitle += appProdVer;
	((CPropertySheet*) this->GetParent())->SetWindowText(lpszTitle);

	PopulateProfileIdDialogFromDatabase (sProfileIdOld, sFleetCardOld, sEMVProfileOld,sKey3ds01Old);

	pProcessingPage->SetModified(FALSE);

	m_CntlProfileId_Fld.EnableWindow(TRUE);
	m_CntlType_Fld.EnableWindow(TRUE);        	
}

/************************************************************/
////////////////////////////////////////////////////////////
/************************************************************/


BOOL CCardsFleetDefinition::OnApply() 
{

	CMyUtils myUtilsObject;
    CAF01 sProfileIdDatabaseStructure;
    FAF01 sFleetCardDatabaseStructure;
	CAF01_EMV sEMVProfileDatabaseStructure;
	KEY3DS01 sKey3ds01DatabaseStructure;
	NCF01 sNetworkRecord;
	char strErrorMessage[200]="";	
	
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
		if (( myUtilsObject.IsEmpty (pProcessingPage->m_StrRemoteDest_Fld,this)) &&
			((pProcessingPage->m_StrAuthLocation_Fld == "REMOTE") ||
			(pProcessingPage->m_StrAuthLocation_Fld == "LOCAL/REMOTE OVERRIDE APPROVALS") ||
			(pProcessingPage->m_StrAuthLocation_Fld == "LOCAL/REMOTE OVERRIDE DECLINES") ||
			(pProcessingPage->m_StrAuthLocation_Fld == "REMOTE/LOCAL STAND-IN")))
		{
			((CPropertySheet*) this->GetParent())->SetActivePage(1);
            pProcessingPage->m_CntlRemoteDest_Fld.SetFocus();
			pProcessingPage->m_StrMessageLbl = "Please enter Remote Destination";
			pProcessingPage->UpdateData(FALSE);
			return FALSE;
		}
		if (!myUtilsObject.IsEmpty (pProcessingPage->m_StrRemoteDest_Fld, this))
        {
			memcpy ( &sNetworkRecord.primary_key.network_id,
				pProcessingPage->m_StrRemoteDest_Fld,
				sizeof (sNetworkRecord.primary_key.network_id));
			memcpy ( &sNetworkRecord.primary_key.network_type,
				"I",
				sizeof (sNetworkRecord.primary_key.network_type));
			int rcode  = txdsapi_get_record ((pBYTE)&sNetworkRecord,
				sizeof (NCF01), NCF01_DATA, strErrorMessage);
			if (rcode != PTEMSG_OK)
			{
				pProcessingPage->m_StrMessageLbl = "ERROR: Invalid Network ID";			
				pProcessingPage->UpdateData(FALSE);
				((CPropertySheet*) this->GetParent())->SetActivePage(1);
				pProcessingPage->m_CntlRemoteDest_Fld.SetFocus();
				return FALSE;
			}
		}
		if ( myUtilsObject.IsEmpty(pProcessingPage->m_StrNumPinRetries_Fld, this)) 
		{
			((CPropertySheet*) this->GetParent())->SetActivePage(1);
            pProcessingPage->m_CntlNumPinRetries_Fld.SetFocus();
			pProcessingPage->m_StrMessageLbl = "Please enter Number of PIN Retries";
			pProcessingPage->UpdateData(FALSE);
			return FALSE;
		}
		/*	    if (( myUtilsObject.IsEmpty (pProcessingPage->m_StrTrackPinOffsetLoc_Fld, this)) &&
		(pProcessingPage->m_StrPinOffsetLocation_Fld == "TRACK"))
		{
		((CPropertySheet*) this->GetParent())->SetActivePage(1);
		pProcessingPage->m_CntlTrackPinOffsetLoc_Fld.SetFocus();
		pProcessingPage->m_StrMessageLbl = "Please enter Track PIN Offset Location";
		pProcessingPage->UpdateData(FALSE);
		return FALSE;
		}	  
		bug #556  */    
		if ( pTime1Page->nDays1 == pTime2Page->nDays2 && !myUtilsObject.IsEmpty(pTime1Page->m_StrDays1_Fld, this)
			&& !myUtilsObject.IsEmpty(pTime2Page->m_StrDays2_Fld, this))
		{
			((CPropertySheet*) this->GetParent())->SetActivePage(3);
			pTime1Page->m_StrMessageLbl = "ERROR: Number of Days for Time Period 1 Cannot be the Same as for Time Period 2";
			pTime1Page->UpdateData(FALSE);
			return FALSE;
		}
		if ( pTime1Page->nDays1 == pTime3Page->nDays3 && !myUtilsObject.IsEmpty(pTime1Page->m_StrDays1_Fld, this)
			&& !myUtilsObject.IsEmpty(pTime3Page->m_StrDays3_Fld, this))
		{
			((CPropertySheet*) this->GetParent())->SetActivePage(3);
			pTime1Page->m_StrMessageLbl = "ERROR: Number of Days for Time Period 1 Cannot be the Same as for Time Period 3";
			pTime1Page->UpdateData(FALSE);
			return FALSE;
		}
		if ( pTime3Page->nDays3 == pTime2Page->nDays2 && !myUtilsObject.IsEmpty(pTime3Page->m_StrDays3_Fld, this)
			&& !myUtilsObject.IsEmpty(pTime2Page->m_StrDays2_Fld, this))
		{
			((CPropertySheet*) this->GetParent())->SetActivePage(4);
			pTime2Page->m_StrMessageLbl = "ERROR: Number of Days for Time Period 2 Cannot be the Same as for Time Period 3";
			pTime2Page->UpdateData(FALSE);
			return FALSE;
		}
		
		if ( pTime1Page->nDays1 > pTime2Page->nDays2 && !myUtilsObject.IsEmpty(pTime1Page->m_StrDays1_Fld, this)
			&& !myUtilsObject.IsEmpty(pTime2Page->m_StrDays2_Fld, this))
		{
			((CPropertySheet*) this->GetParent())->SetActivePage(3);
			pTime1Page->m_StrMessageLbl = "ERROR: Number of Days for Time Period 2 Cannot be Less than for Time Period 1";
			pTime1Page->UpdateData(FALSE);
			return FALSE;
		}
		if ( pTime1Page->nDays1 > pTime3Page->nDays3 && !myUtilsObject.IsEmpty(pTime1Page->m_StrDays1_Fld, this)
			&& !myUtilsObject.IsEmpty(pTime3Page->m_StrDays3_Fld, this))
		{
			((CPropertySheet*) this->GetParent())->SetActivePage(3);
			pTime1Page->m_StrMessageLbl = "ERROR: Number of Days for Time Period 3 Cannot be Less than for Time Period 1";
			pTime1Page->UpdateData(FALSE);
			return FALSE;
		}
		if ( pTime3Page->nDays3 < pTime2Page->nDays2 && !myUtilsObject.IsEmpty(pTime3Page->m_StrDays3_Fld, this)
			&& !myUtilsObject.IsEmpty(pTime2Page->m_StrDays2_Fld, this))
		{
			((CPropertySheet*) this->GetParent())->SetActivePage(4);
			pTime2Page->m_StrMessageLbl = "ERROR: Number of Days for Time Period 3 Cannot be Less than for Time Period 2";
			pTime2Page->UpdateData(FALSE);
			return FALSE;
		}
	}
	
    if (m_nDialogMode == MODE_EDIT)
	{ 
		memset(&sProfileIdDatabaseStructure, 0, sizeof(sProfileIdDatabaseStructure));
        memset(&sFleetCardDatabaseStructure, 0, sizeof(sFleetCardDatabaseStructure));
		memset(&sEMVProfileDatabaseStructure, 0, sizeof(sEMVProfileDatabaseStructure));
		memset(&sKey3ds01DatabaseStructure, 0, sizeof(sKey3ds01DatabaseStructure));

		
        PopulateProfileIdDatabaseStructureFromDialog(&sProfileIdDatabaseStructure,
			&sFleetCardDatabaseStructure, &sEMVProfileDatabaseStructure,&sKey3ds01DatabaseStructure );
        
		if (m_nCardMode == MODE_FLEET)
        {       
			int rcode = txdsapi_update_record ((pBYTE)&sFleetCardDatabaseStructure, sizeof (FAF01), FAF01_DATA, strErrorMessage);
			if (rcode != PTEMSG_OK)
			{    	
				m_StrMessageLbl = strErrorMessage;
				m_nDialogMode = MODE_DONE;
				
				if (!UpdateData(FALSE))
				{
					AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
				}
				return FALSE;
			}
        }
        int rcode = txdsapi_update_record  ((pBYTE)&sProfileIdDatabaseStructure,
			sizeof (CAF01), CAF01_DATA, strErrorMessage);
		if (rcode != PTEMSG_OK)
		{    	
			m_StrMessageLbl = strErrorMessage;
            m_nDialogMode = MODE_DONE;
			
            ((CPropertySheet*) this->GetParent())->SetActivePage(0);
            if (m_nCardMode == MODE_FLEET)
            {           	
				((CPropertySheet*) this->GetParent())->SetWindowText("Fleet Card Definition");		
			}
			
			if (m_nCardMode == MODE_CREDIT)
			{           	
				((CPropertySheet*) this->GetParent())->SetWindowText("Credit Card Definition");		
			}
			if (m_nCardMode == MODE_DEBIT)
			{           	
				((CPropertySheet*) this->GetParent())->SetWindowText("Debit Card Definition");		
			}
			//set version info in the title bar
			CString appProdVer;
			CString lpszTitle;
			appProdVer.LoadString(IDS_PRODUCT_VERSION);
			((CPropertySheet*) this->GetParent())->GetWindowText(lpszTitle);
			lpszTitle = lpszTitle + " - Version - ";
			lpszTitle += appProdVer;
			((CPropertySheet*) this->GetParent())->SetWindowText(lpszTitle);
			m_CntlProfileId_Fld.EnableWindow(TRUE);
			
			if (!UpdateData(FALSE))
            {
				AfxMessageBox (IDS_UPDATEDATAFAILED, MB_ICONSTOP);		
			}
			return FALSE;
		}        
        else
		{
			rcode = txdsapi_update_record  ((pBYTE)&sEMVProfileDatabaseStructure,
				sizeof (CAF01_EMV), CAF01_EMV_DATA, strErrorMessage);
			if (rcode != PTEMSG_OK)
			{    	
			
			}  

			if(Record_presentIn_key3ds01Flag==0)
			{
				rcode = txdsapi_insert_record  ((pBYTE)&sKey3ds01DatabaseStructure,
				sizeof (KEY3DS01), KEY3DS01_DATA, strErrorMessage); 
				if (rcode != PTEMSG_OK)
				{   	
			                 
				}
				Record_presentIn_key3ds01Flag=1;
			}
			else
			{

				rcode = txdsapi_update_record  ((pBYTE)&sKey3ds01DatabaseStructure,
				sizeof (KEY3DS01), KEY3DS01_DATA, strErrorMessage);
				if (rcode != PTEMSG_OK)
				{    	
			
				}   
			}
		
			m_StrMessageLbl = "Record Updated";
			if (!UpdateData(FALSE))
            {
				AfxMessageBox (IDS_UPDATEDATAFAILED, MB_ICONSTOP);	
			}
			
            pProcessingPage->CompareStructures(sKey3ds01Old);
			//	pMerchantLimitPage->CompareStructures();
			//	pTime1Page->CompareStructures();
			//	pTime2Page->CompareStructures();
			//	pTime3Page->CompareStructures();
			
			if (pProcessingPage->m_bProcessing)
			{				
				pProcessingPage->CleanProcessingDialog();				
				pProcessingPage->EnableProcessingDialog(FALSE);				
			}			
            if (pMerchantLimitPage->m_bMerchant )
            {	
				//	pMerchantLimitPage->CompareStructures(&sProfileIdDatabaseStructure);
				pMerchantLimitPage->CleanMerchantDialog();				
                pMerchantLimitPage->EnableMerchantDialog(FALSE);			
            }
			if (pCardsProfilePage->m_bProcessing)
			{
				pCardsProfilePage->CleanCardsProfileDialog();
				pCardsProfilePage->EnableCardsProfileDialog(FALSE);
			}
			if ( pCntryAmtLimitsPage->m_bCntryAmtLmt)
			{
				pCntryAmtLimitsPage->CleanCntryLmtsDialog();
				pCntryAmtLimitsPage->EnableCntryAmtLmtDialog(TRUE);
			}
			
            if ( pTime1Page->m_bTime1 )
            {	
				pTime1Page->CompareStructures();
				pTime1Page->CleanTime1Dialog();				
                pTime1Page->EnableTime1Dialog(FALSE);			    
            }
            if ( pTime2Page->m_bTime2 )
            {	
				pTime2Page->CompareStructures();
				pTime2Page->CleanTime2Dialog();				
				pTime2Page->EnableTime2Dialog(FALSE);				
            }
            if ( pTime3Page->m_bTime3 )
            {				
				pTime3Page->CompareStructures();
				pTime3Page->CleanTime3Dialog();			
                pTime3Page->EnableTime3Dialog(FALSE);				
            }            
			if (m_nCardMode == MODE_FLEET)
			{
				pVelocityPage->CompareStructures();
				if ( pVelocityPage->m_bVelocity )
				{					
					pVelocityPage->CleanVelocityDialog();					
					pVelocityPage->EnableVelocityDialog(FALSE);				    
				}
				pVelocityPage->m_nDialogMode = MODE_DONE;		           
			}
			if (pChangeHistoryPage->m_bChangeHistory)
				pChangeHistoryPage->CleanChangeHistory();
			
			pProcessingPage->m_nDialogMode = MODE_DONE;
			pTime1Page->m_nDialogMode = MODE_DONE;
			pTime3Page->m_nDialogMode = MODE_DONE;
			pTime2Page->m_nDialogMode = MODE_DONE;
			pCntryAmtLimitsPage->m_nDialogMode = MODE_DONE;
			pMerchantLimitPage->m_nDialogMode = MODE_DONE;
			pCardsProfilePage->m_nDialogMode = MODE_DONE;
			pChangeHistoryPage->m_nDialogMode = MODE_DONE;  
			
			
			((CPropertySheet*) this->GetParent())->SetActivePage(0);
			if (m_nCardMode == MODE_FLEET)
			{           	
				((CPropertySheet*) this->GetParent())->SetWindowText("Fleet Card Definition");		
			}
			if (m_nCardMode == MODE_CREDIT)
			{           	
				((CPropertySheet*) this->GetParent())->SetWindowText("Credit Card Definition");		
			}
			if (m_nCardMode == MODE_DEBIT)
			{           	
				((CPropertySheet*) this->GetParent())->SetWindowText("Debit Card Definition");		
			}
			//set version info in the title bar
			CString appProdVer;
			CString lpszTitle;
			appProdVer.LoadString(IDS_PRODUCT_VERSION);
			((CPropertySheet*) this->GetParent())->GetWindowText(lpszTitle);
			lpszTitle = lpszTitle + " - Version - ";
			lpszTitle += appProdVer;
			((CPropertySheet*) this->GetParent())->SetWindowText(lpszTitle);
			
			m_CntlProfileId_Fld.EnableWindow(TRUE);
			SetModified ( FALSE );
			m_nDialogMode = MODE_DONE;			
			//return CPropertyPage::OnApply();
			return FALSE;
        }      
    }
	if (m_nDialogMode == MODE_ADD)
    {
        memset(&sProfileIdDatabaseStructure,0,sizeof(sProfileIdDatabaseStructure));
        memset(&sFleetCardDatabaseStructure,0,sizeof(sFleetCardDatabaseStructure));
        memset(&sEMVProfileDatabaseStructure,0,sizeof(sEMVProfileDatabaseStructure));
		memset(&sKey3ds01DatabaseStructure, 0, sizeof(sKey3ds01DatabaseStructure));

		
        if (!(PopulateProfileIdDatabaseStructureFromDialog(&sProfileIdDatabaseStructure,
			&sFleetCardDatabaseStructure, &sEMVProfileDatabaseStructure,&sKey3ds01DatabaseStructure)))
        {		
			return FALSE;
		}
        if (m_nCardMode == MODE_FLEET)
        {        
			int rcode = txdsapi_insert_record((pBYTE)&sFleetCardDatabaseStructure,
				sizeof (FAF01), FAF01_DATA, strErrorMessage);        
			if (rcode != PTEMSG_OK)
			{   	
				m_StrMessageLbl = strErrorMessage;
				m_nDialogMode = MODE_DONE;
				if (!UpdateData(FALSE))
				{
					AfxMessageBox (IDS_UPDATEDATAFAILED, MB_ICONSTOP);                
				}
                ((CPropertySheet*) this->GetParent())->SetActivePage(0);            
				return FALSE;
			}        
        }
        
        int rcode = txdsapi_insert_record  ((pBYTE)&sProfileIdDatabaseStructure,
			sizeof (CAF01), CAF01_DATA, strErrorMessage); 
        if (rcode != PTEMSG_OK)
		{   	
			m_StrMessageLbl = strErrorMessage;
            m_nDialogMode = MODE_DONE;
			if (!UpdateData(FALSE))
            {
				AfxMessageBox (IDS_UPDATEDATAFAILED, MB_ICONSTOP);                
			}
            ((CPropertySheet*) this->GetParent())->SetActivePage(0);
			return FALSE;                  
        }
        else 
		{   
			rcode = txdsapi_insert_record  ((pBYTE)&sEMVProfileDatabaseStructure,
			sizeof (CAF01_EMV), CAF01_EMV_DATA, strErrorMessage); 
			if (rcode != PTEMSG_OK)
			{   	
			                 
			}

			rcode = txdsapi_insert_record  ((pBYTE)&sKey3ds01DatabaseStructure,
			sizeof (KEY3DS01), KEY3DS01_DATA, strErrorMessage); 
			if (rcode != PTEMSG_OK)
			{   	
			                 
			}
			memcpy ((LPSTR)m_strCurrentData,"Record Added",sizeof (m_strCurrentData));			
			PopulateHistoryTable();
			((CPropertySheet*) this->GetParent())->SetActivePage(0);                
			
			m_StrMessageLbl = "Record Added";	
			
			if (!UpdateData(FALSE))
            {
				AfxMessageBox (IDS_UPDATEDATAFAILED, MB_ICONSTOP);		
			}
			if (pProcessingPage->m_bProcessing)
			{          
				pProcessingPage->CleanProcessingDialog();
				pProcessingPage->EnableProcessingDialog(FALSE);
			}                  
            if (pMerchantLimitPage->m_bMerchant)
            {
				pMerchantLimitPage->CleanMerchantDialog();
                pMerchantLimitPage->EnableMerchantDialog(FALSE);                
            }
			if (pCardsProfilePage->m_bProcessing)
			{
				pCardsProfilePage->CleanCardsProfileDialog();
				pCardsProfilePage->EnableCardsProfileDialog(FALSE);
			}
            if (pTime1Page->m_bTime1)
            {
				pTime1Page->CleanTime1Dialog();
                pTime1Page->EnableTime1Dialog(FALSE);                
            }
            if (pTime2Page->m_bTime2)
            {
				pTime2Page->CleanTime2Dialog();
                pTime2Page->EnableTime2Dialog(FALSE);               
            }
            if (pTime3Page->m_bTime3)
            {
				pTime3Page->CleanTime3Dialog();
                pTime3Page->EnableTime3Dialog(FALSE);                
            }
			pProcessingPage->m_nDialogMode = MODE_DONE;
			pMerchantLimitPage->m_nDialogMode = MODE_DONE;
			pCardsProfilePage->m_nDialogMode = MODE_DONE;
			pTime1Page->m_nDialogMode = MODE_DONE;
			pTime2Page->m_nDialogMode = MODE_DONE;
			pTime3Page->m_nDialogMode = MODE_DONE;
			pCntryAmtLimitsPage->m_nDialogMode = MODE_DONE;
            if (m_nCardMode == MODE_FLEET)
            {
				if (pVelocityPage->m_bVelocity)
				{
					pVelocityPage->CleanVelocityDialog();
					pVelocityPage->EnableVelocityDialog(FALSE);
				}  
				pVelocityPage->m_nDialogMode = MODE_DONE;
            }            	
            if (pChangeHistoryPage->m_bChangeHistory)
				pChangeHistoryPage->CleanChangeHistory();			
			
            m_CntlProfileId_Fld.SetFocus(); 
			m_CntlProfileId_Fld.EnableWindow(TRUE);
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

/************************************************************/
////////////////////////////////////////////////////////////
/************************************************************/

BOOL CCardsFleetDefinition::PopulateProfileIdDialogFromDatabase ( CAF01 sProfileId, FAF01 sFleetCard, CAF01_EMV sEMVProfile, KEY3DS01 sKey3ds01)
{   
	
	if (!pProcessingPage->m_bProcessing)
		memcpy ( &pProcessingPage->sProfileIdOld, &sProfileId, sizeof ( CAF01));
	else
		pProcessingPage->PopulateProcessingDialogFromDatabase(sProfileId,sKey3ds01);
	
	
	if(!pCardsProfilePage->m_bProcessing)
		memcpy( &pCardsProfilePage->sProfileIdOld, &sEMVProfile, sizeof(CAF01_EMV));
	else
		pCardsProfilePage->PopulateCardsProfileDialogFromDatabase(sEMVProfile);
	
	
    if (!pMerchantLimitPage->m_bMerchant)
		memcpy ( &pMerchantLimitPage->sProfileIdOld, &sProfileId, sizeof ( CAF01));
	else
		pMerchantLimitPage->PopulateMerchantDialogFromDatabase(sProfileId);
	
    if (!pTime1Page->m_bTime1)
		memcpy ( &pTime1Page->sProfileIdOld, &sProfileId, sizeof ( CAF01));
	else
		pTime1Page->PopulateTime1DialogFromDatabase(sProfileId);
    
	
    if (!pTime2Page->m_bTime2)
		memcpy ( &pTime2Page->sProfileIdOld, &sProfileId, sizeof ( CAF01));
	else
		pTime2Page->PopulateTime2DialogFromDatabase(sProfileId);
	
	
    if (!pTime3Page->m_bTime3)
		memcpy ( &pTime3Page->sProfileIdOld, &sProfileId, sizeof ( CAF01));
	else
		pTime3Page->PopulateTime3DialogFromDatabase(sProfileId);  
	
	if (!pCntryAmtLimitsPage->m_bCntryAmtLmt)
	{
		memcpy ( &pCntryAmtLimitsPage->sProfileIdOld, &sProfileId, sizeof(CAF01));
	}
	else
		pCntryAmtLimitsPage->PopulateCntryLmtDialogFromDatabase(sProfileId);
	
    
	if (m_nCardMode == MODE_FLEET)
    {
		if (!pVelocityPage->m_bVelocity)
			memcpy ( &pVelocityPage->sFleetCardOld, &sFleetCard, sizeof ( FAF01));
		else
			pVelocityPage->PopulateVelocityDialogFromDatabase(sFleetCard);     
    }
    return TRUE;
}


/************************************************************/
////////////////////////////////////////////////////////////
/************************************************************/

BOOL CCardsFleetDefinition::PopulateProfileIdDatabaseStructureFromDialog ( pCAF01 psProfileId, pFAF01 psFleetCard, pCAF01_EMV psEMVProfile ,pKEY3DS01 psKey3ds01) 
{
    ZeroMemory (psProfileId, sizeof(CAF01)) ;
    ZeroMemory (psFleetCard, sizeof(FAF01)) ;
	ZeroMemory (psEMVProfile, sizeof(CAF01_EMV));
			ZeroMemory (psKey3ds01, sizeof(KEY3DS01));
	
	if ( m_nDialogMode == MODE_EDIT )
	{
		memcpy (psProfileId,&sProfileIdOld, sizeof(CAF01));
		memcpy (psFleetCard,&sFleetCardOld, sizeof(FAF01));
		memcpy (psEMVProfile, &sEMVProfileOld, sizeof(CAF01_EMV));	
		memcpy (psKey3ds01, &sKey3ds01Old, sizeof(KEY3DS01));


	}
	
    pProcessingPage->PopulateDatabaseFromProcessingDialog( psProfileId, psKey3ds01 );      
	pCardsProfilePage->PopulateDatabaseFromCardsProfile(psEMVProfile);
	
    memcpy ((LPTSTR) psProfileId->primary_key.caf_profile_id,m_StrProfileId_Fld, sizeof (psProfileId->primary_key.caf_profile_id));
		memcpy ((LPTSTR) psKey3ds01->caf_profile_id,m_StrProfileId_Fld, sizeof (psKey3ds01->caf_profile_id));

	//    pMerchantLimitPage->PopulateDatabaseFromMerchantDialog( psProfileId );//handled inside MerchantLimit dialog
    pTime1Page->PopulateDatabaseFromTime1Dialog( psProfileId );
    pTime2Page->PopulateDatabaseFromTime2Dialog( psProfileId );
    pTime3Page->PopulateDatabaseFromTime3Dialog( psProfileId ); 
    if (m_nCardMode == MODE_FLEET)
    {              
        memcpy ((LPTSTR) psProfileId->primary_key.type,"F", sizeof (psProfileId->primary_key.type));
        memcpy ((LPTSTR) psFleetCard->primary_key.caf_profile_id,m_StrProfileId_Fld,
            sizeof(psFleetCard->primary_key.caf_profile_id)); 
        pVelocityPage->PopulateDatabaseFromVelocityDialog( psFleetCard );
    }
    if (m_nCardMode == MODE_CREDIT)
    {
		memcpy ((LPTSTR) psProfileId->primary_key.type,"C", sizeof (psProfileId->primary_key.type));
		
    }
	if (m_nCardMode == MODE_DEBIT)
    {
		memcpy ((LPTSTR) psProfileId->primary_key.type,"D", sizeof (psProfileId->primary_key.type));
		
    }
	
	return TRUE;
}


/************************************************************/
////////////////////////////////////////////////////////////
/************************************************************/

void CCardsFleetDefinition::PopulateHistoryTable()
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
	
    strcpy ( strPrimaryKey, m_StrProfileId_Fld);
    strcat ( strPrimaryKey, m_StrType_Fld);
    
    myUtilsObject.GetSystemDate(strDate); 
    myUtilsObject.GetSystemTime(strTime);
	
    pintime_get_gmt ( dest_str );
	
    ZeroMemory ( &sHistoryData, sizeof (HST01));
    memcpy ((LPTSTR) sHistoryData.file_name,     (LPCTSTR)"CAF01", sizeof (sHistoryData.file_name));
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
	//	memcpy ((LPTSTR)sHistoryData.previous_data, (LPCTSTR)m_strPreviousData, sizeof (sHistoryData.previous_data));
	memcpy ((LPTSTR)sHistoryData.changed_by, (LPCTSTR)ucf01.primary_key.name, sizeof (sHistoryData.changed_by));
	//	memcpy ((LPTSTR)sHistoryData.field_tag, (LPCTSTR)m_strFieldTag, sizeof (sHistoryData.field_tag));
	
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

BOOL CCardsFleetDefinition::CheckDialogForCompleteness()
{
    CMyUtils myUtilsObject;
	
    if (!UpdateData(TRUE)){
		AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
	}
	
    if (myUtilsObject.IsEmpty (m_StrProfileId_Fld, this))
    {
		m_StrMessageLbl = "Please enter Profile ID";	
		m_CntlProfileId_Fld.SetFocus();
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

void CCardsFleetDefinition::OnCancel() 
{      
    if ( pChangeHistoryPage->m_bChangeHistory )
		pChangeHistoryPage->CleanChangeHistory();
	
	if ( pProcessingPage->m_bProcessing)
    {
		pProcessingPage->CleanProcessingDialog();
		pProcessingPage->EnableProcessingDialog(FALSE);
    }

	if(pCardsProfilePage->m_bProcessing)
	{
		pCardsProfilePage->CleanCardsProfileDialog();
		pCardsProfilePage->EnableCardsProfileDialog(FALSE);
	}
	
    if ( pMerchantLimitPage->m_bMerchant)
    {
		pMerchantLimitPage->CleanMerchantDialog();
		pMerchantLimitPage->EnableMerchantDialog(FALSE);
    }
	
    if ( pTime1Page->m_bTime1)
    {
		pTime1Page->CleanTime1Dialog();
		pTime1Page->EnableTime1Dialog(FALSE);
    }
	
    if ( pTime2Page->m_bTime2)
    {
		pTime2Page->CleanTime2Dialog();
		pTime2Page->EnableTime2Dialog(FALSE);
    }
	
	if ( pTime3Page->m_bTime3)
    {
		pTime3Page->CleanTime3Dialog();
		pTime3Page->EnableTime3Dialog(FALSE);
    }
	
    if (m_nCardMode ==MODE_FLEET)
    {
		if ( pVelocityPage->m_bVelocity)
		{
			pVelocityPage->CleanVelocityDialog();
			pVelocityPage->EnableVelocityDialog(FALSE);
		}
    }
	
	
	CPropertyPage::OnCancel();
}

BOOL CCardsFleetDefinition::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	// TODO: Add your specialized code here and/or call the base class
	
	c_time = CTime::GetCurrentTime();	
	shrmem.SetLastUpdateTime(c_time);
	return CPropertyPage::OnCommand(wParam, lParam);
}
