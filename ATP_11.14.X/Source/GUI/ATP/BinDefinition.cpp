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
* $Log:   N:\POS\PVCS6.6\EPICPORTZ\PTE\Equitable\ATP_gui\BinDefinition.cpp  $

   Rev        April 28.4.11          Krishna Kiran S@TF
	 Added Idle Time Calculation & calling Login screen on Idle Timeout
	 Added Creation of SHM & Mutex for Synchronisation of dislpalying Login
	 Form from different windows
   
      Rev 1.28   Sep 28 2006 10:27:44   dirby
   Added a check box to the Bin Range GUI to indicate if a bin range
   is eligible or not for Fraudguard.  If not checked, then txns are
   sent to Fraudguard (if they meet other qualifications).  If checked,
   then txns will not go to Fraudguard (regardless of other qualifications).
   The new check box shares the Mod_Check field with the Mod10
   Check value and the Cash Bonus Eligible value.  Each of these
   three values is either ON or OFF, so they are represented by a
   single bit within the Mod_Check field.
   SCR 21851
   
      Rev 1.27   Oct 04 2005 12:05:40   dirby
   Added ELECTRON as a credit subtype.  This is for Visa Electron
   cards.  The bin range is being widened, so EB felt it would be
   easier to have a subtype so as not to restrict the bin range.
   SCR 17694
   
      Rev 1.26   Apr 01 2005 10:13:28   lmitchel
   Initial Checking prior to integration testing for updates made to support country/curreny code limits (scr13929), unulimited purchase card MCC ranges, and purchase card transaction limits(scr12785)
   
      Rev 1.25   Jul 16 2004 16:08:04   lmitchel
   SCR1287 - Added new fields to support Auto Init and Reload Mechants;   A new Auto Init tab and support fields were added to the Device Definition GUI.  A new reload flag was added to the Merchant Definition General GUI.  A new credti subtype = PREPAID was added to the BIN Definition GUI.
   
      Rev 1.24   Dec 01 2003 11:28:42   lmitchel
   Added AMEX Online Updates - scr1200
   
      Rev 1.23   Sep 11 2002 16:52:38   dirby
   Modified to support new Cash Bonus check box field.  No new
   database field was created.  This field shares the mod_check
   field in the database with the Mod10 Check GUI field.  Since
   these two fields are flags (on/off), there are only 4 different values
   this database field will have.
   SCR 804
   
      Rev 1.22   Aug 29 2002 12:47:30   dirby
   Modified OnAdd to distinguish between Bin Type of Debit and
   Deferred.    SCR 551
   
      Rev 1.21   Aug 15 2002 17:07:58   dirby
   Modified to support a new bin type called DEFERRED.
   SCR 551
   
      Rev 1.20   Jul 10 2002 09:55:40   lmitchel
   Ver:4.0.3.1:  Added SendMessage command in PropertySheet files (on_init function) to set the Apply button as the default.  If the enter key is pressed, action will default to  the apply button instead of the OK button.  This allows the user to remain in the current dialog instead of exiting to the main menu.
   
      Rev 1.19   May 21 2002 13:30:38   dirby
   Modified to not exit when <enter> is pressed.  If OK is selected,
   the database gets updated still, but screen is not exited.
   SCR 587
   
      Rev 1.18   Nov 16 2001 13:21:46   lmitchel
   
      Rev 1.17   Nov 09 2001 16:46:12   lmitchel
   GUI support for RSB cards - Added 2 new fields, add_on_rate, add_on_term to card general page including support for add, update, view, change history
   
      Rev 1.16   Nov 06 2001 16:13:58   lmitchel
   SCR361-Corrected MCC Blocking error checking on apply; Correction for deleting acquirer ids and MCC ranges;
   
      Rev 1.15   Nov 05 2001 14:16:02   dirby
   Added credit subtype GUARANTOR.  This is for RSB Guarantor cards.      SCR 256
   
      Rev 1.14   Oct 31 2001 15:57:36   lmitchel
   correct assertion failure bin definition; set default to Mod Check = NONE; enable apply button on update for  acquirer blocking screen.
   
      Rev 1.13   Sep 28 2001 12:38:58   SYARLAGA
   Added code for MccBinBlocking and Mod10Check
   
      Rev 1.12   May 12 2000 14:30:06   iarustam
    
   
      Rev 1.11   Apr 17 2000 09:56:54   iarustam
   added bin blocking
   
   
      Rev 1.10   Jan 25 2000 14:08:16   iarustam
   fixed check problem
   
      Rev 1.9   Jan 05 2000 14:52:50   gbarnes
   Major update. Not updated to PVCS recently.
   PVCS was locked.
   
      Rev 1.4   Oct 06 1999 16:45:42   iarustam
   Bug fixes # 65,118,58,145,117,116,96,189,97,
   120,77,50,177,95,94
   
      Rev 1.3   Sep 30 1999 16:16:48   npogosya
   Bug fixes
   
      Rev 1.1   Sep 22 1999 15:40:50   iarustam
   Added manual batch entry GUI
   
      Rev 1.0   Jul 01 1999 14:16:48   iarustam
   Initial Release
   

    
*
************************************************************************************/

// BinDefinition.cpp : implementation file
//

#include "stdafx.h"
#include "Atp.h"
#include "BinDefinition.h"
#include "BinList.h"

#include "MyUtils.h"
//#include "atpdb.h"
#include "BinPropertySheet.h"
#include "NameList.h"
#include "MyUtils.h"
#include "SharedMem.h"

extern "C"
{
#include "PteTime.h"
}
//#include "htmlhelp.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CString strLength;
extern CString strLow;
extern CString strType1;
extern UCF01 ucf01;
extern CSharedMem shrmem;
extern MIDBINBLK01	sMidBinBlkRecord_Global ;
extern MIDBINBLK01 sOldMidBinBlkRecord_Global;

/////////////////////////////////////////////////////////////////////////////
// CBinDefinition property page

IMPLEMENT_DYNCREATE(CBinDefinition, CPropertyPage)

CBinDefinition::CBinDefinition() : CPropertyPage(CBinDefinition::IDD)
{
	//{{AFX_DATA_INIT(CBinDefinition)
	m_StrBinLowFld = _T("");
	m_StrMessageLbl = _T("");
	m_StrBinLengthFld = _T("");
	m_StrType = _T("");
	//}}AFX_DATA_INIT
	//pLock = new CDMLock("IdleLock");
}

CBinDefinition::~CBinDefinition()
{
	//delete pLock;
	//pLock = NULL;
}

void CBinDefinition::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CBinDefinition)
	DDX_Control(pDX, IDC_VIEW_BIN_BUTTON, m_ViewButton);
	DDX_Control(pDX, IDC_UPDATE_BIN_BUTTON, m_UpdateButton);
	DDX_Control(pDX, IDC_LIST_BIN_BUTTON, m_ListButton);
	DDX_Control(pDX, IDC_DELETE_BIN_BUTTON, m_DeleteButton);
	DDX_Control(pDX, IDC_ADD_BIN_BUTTON, m_AddButton);
	DDX_Control(pDX, IDC_TYPE, m_CntlType);
	DDX_Control(pDX, IDC_BIN_LENGTH_FLD, m_CntlBinLengthFld);
	DDX_Control(pDX, IDC_BIN_LOW_FLD, m_CntlBinLowFld);
	DDX_Text(pDX, IDC_BIN_LOW_FLD, m_StrBinLowFld);
	DDV_MaxChars(pDX, m_StrBinLowFld, 16);
	DDX_Text(pDX, IDC_MESSAGE_LBL, m_StrMessageLbl);
	DDX_Text(pDX, IDC_BIN_LENGTH_FLD, m_StrBinLengthFld);
	DDV_MaxChars(pDX, m_StrBinLengthFld, 2);
	DDX_CBString(pDX, IDC_TYPE, m_StrType);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CBinDefinition, CPropertyPage)
	//{{AFX_MSG_MAP(CBinDefinition)
	ON_BN_CLICKED(IDC_ADD_BIN_BUTTON, OnAddBinButton)
	ON_BN_CLICKED(IDC_DELETE_BIN_BUTTON, OnDeleteBinButton)
	ON_BN_CLICKED(IDC_UPDATE_BIN_BUTTON, OnUpdateBinButton)
	ON_BN_CLICKED(IDC_VIEW_BIN_BUTTON, OnViewBinButton)
	ON_BN_CLICKED(IDC_LIST_BIN_BUTTON, OnListBinButton)
	ON_WM_HELPINFO()
	ON_EN_KILLFOCUS(IDC_BIN_LENGTH_FLD, OnKillfocusBinLengthFld)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBinDefinition message handlers

BOOL CBinDefinition::OnInitDialog() 
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

	m_StrBinLowFld = "";
	m_StrBinLengthFld = "";
	m_StrMessageLbl = "";	
//	m_CntlType.SetCurSel(0);
	m_StrType = "CREDIT";
   m_nDialogMode = MODE_DONE;

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

	UpdateData(FALSE);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CBinDefinition::OnAddBinButton() 
{
	BIN01 sBinRecord;       
   CMyUtils myUtilsObject;
	char strErrorMessage[200];


	m_StrMessageLbl = "";

	m_nDialogMode = MODE_ADD;
	pGeneralPage->m_nDialogMode = MODE_ADD;  	
   pBlockingPage->m_nDialogMode = MODE_ADD;
   pMccBlockingPage->m_nDialogMode = MODE_ADD;
   pChangeHistoryPage->m_nDialogMode = MODE_ADD; 
	pMidBlockingPage->m_nDialogMode = MODE_ADD;

	if (!CheckDialogForCompleteness())
        return;

   GetBinType();
	ZeroMemory(&sBinRecord,sizeof(BIN01)) ;
   memcpy ((LPSTR)(sBinRecord.primary_key.length), m_StrBinLengthFld , sizeof (sBinRecord.primary_key.length));
   memcpy ((LPSTR)(sBinRecord.primary_key.low), m_StrBinLowFld , sizeof (sBinRecord.primary_key.low));
   memcpy ((LPSTR)(sBinRecord.primary_key.type), strType, sizeof (sBinRecord.primary_key.type));

   int rcode = txdsapi_get_record((pBYTE)&sBinRecord,sizeof(BIN01),BIN01_DATA,strErrorMessage);
	if ( rcode == PTEMSG_OK)
	{
		m_StrMessageLbl = "ERROR: BIN Range already exists";	
		m_CntlBinLengthFld.SetFocus();
		if (!UpdateData(FALSE)){
			AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
		}
		m_nDialogMode = MODE_DONE;
		return ;
	}
	else if ( rcode == PTEMSG_INVALID_DATATYPE )
	{
		AfxMessageBox("Please try again");
		return;	
	}

	if ( pGeneralPage->m_bGeneral )
	{
		pGeneralPage->CleanBinDialog();
		pGeneralPage->EnableBinDialog(TRUE);		
	}

   if ( pBlockingPage->m_bBlocking )
	{
		pBlockingPage->CleanBlockingDialog();
	}
   
   if ( pMccBlockingPage->m_bMCCBlocking )
	{
		pMccBlockingPage->CleanBlockingDialog();
	}

   if ( pMidBlockingPage->m_bBlocking )
	{
		pMidBlockingPage->CleanBlockingDialog();
	}
    GetBinType();
    pGeneralPage->m_StrBinLowFld = m_StrBinLowFld;
    pGeneralPage->m_StrBinLengthFld = m_StrBinLengthFld;
	 memcpy (pGeneralPage->strType, strType, sizeof (strType));	
	
    pBlockingPage->m_StrBinLowFld = m_StrBinLowFld;
    pBlockingPage->m_StrBinLengthFld = m_StrBinLengthFld;
	pBlockingPage->m_StrBinTypeFld = m_StrType;	

	pMccBlockingPage->m_StrBinLowFld1 = m_StrBinLowFld;
    pMccBlockingPage->m_StrBinLengthFld1 = m_StrBinLengthFld;
	pMccBlockingPage->m_StrMccBinTypeFld1 = m_StrType;	

	pMidBlockingPage->m_StrBinLowFld = m_StrBinLowFld;
    pMidBlockingPage->m_StrBinLengthFld = m_StrBinLengthFld;
	pMidBlockingPage->m_StrBinTypeFld = m_StrType;	
            
   ((CPropertySheet*) this->GetParent())->SetWindowText("Add BIN");
   //set version info in the title bar
	CString appProdVer;
	CString lpszTitle;
	appProdVer.LoadString(IDS_PRODUCT_VERSION);
	((CPropertySheet*) this->GetParent())->GetWindowText(lpszTitle);
	lpszTitle = lpszTitle + " - Version - ";
	lpszTitle += appProdVer;
	((CPropertySheet*) this->GetParent())->SetWindowText(lpszTitle);

   if ( pGeneralPage->m_bGeneral )
	{
		pGeneralPage->UpdateData(FALSE);
		pGeneralPage->SetBinType();
	   pGeneralPage->EnableBinDialog(TRUE);
		pGeneralPage->SetBinType();
	}

   if ( pBlockingPage->m_bBlocking )
	{
		pBlockingPage->UpdateData(FALSE);
	}

   if ( pMccBlockingPage->m_bMCCBlocking )
	{
		pMccBlockingPage->UpdateData(FALSE);
				
	}

    if ( pMidBlockingPage->m_bBlocking )
	{
		pMidBlockingPage->UpdateData(FALSE);
	}
	            
	    ((CPropertySheet*) this->GetParent())->SetActivePage(1);
	    m_CntlBinLowFld.EnableWindow(FALSE);
		m_CntlBinLengthFld.EnableWindow(FALSE);
		m_CntlType.EnableWindow(FALSE);
     
}

void CBinDefinition::OnDeleteBinButton() 
{
	CString strMessage;
    int nReturnCode;
    BIN01 sBinRecord;
	char strErrorMessage[200];
	int flag=0;
    
     m_nDialogMode = MODE_DONE;
   pBlockingPage->m_nDialogMode = MODE_DONE;
   pMccBlockingPage->m_nDialogMode = MODE_DONE;
   pGeneralPage->m_nDialogMode = MODE_DONE;
	pMidBlockingPage->m_nDialogMode = MODE_DONE;

    m_StrMessageLbl = "";

    if (!CheckDialogForCompleteness())
        return;   

   /* m_pUserSheet->m_AccessPage.m_psp.dwFlags &= ~PSP_HASHELP; 
    m_pUserSheet->m_WorkstationPage.m_psp.dwFlags &= ~PSP_HASHELP; 
    m_pUserSheet->m_ChangeHistoryPage.m_psp.dwFlags &= ~PSP_HASHELP;
   
    m_pUserSheet->m_psh.dwFlags &= ~PSH_HASHELP;*/

	memset(&sMidBinBlkRecord_Global,0x00,sizeof(MIDBINBLK01));

    if (!UpdateData(TRUE)){
		AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
	}
    
	strMessage.Format ("Are you sure you want to delete BIN Low %s,\n BIN Length %s, Type %s ?", (LPCTSTR) m_StrBinLowFld,(LPCTSTR) m_StrBinLengthFld, (LPCTSTR) m_StrType) ;

	nReturnCode = AfxMessageBox ((LPCTSTR)strMessage, MB_OKCANCEL|MB_ICONQUESTION) ;
	if (nReturnCode != IDOK) 
		return ;

	GetBinType();
	memcpy ((LPTSTR)sBinRecord.primary_key.type, strType, sizeof (sBinRecord.primary_key.type)) ;
	memcpy ((LPTSTR)sBinRecord.primary_key.low, m_StrBinLowFld, sizeof (sBinRecord.primary_key.low)) ;
	memcpy ((LPTSTR)sBinRecord.primary_key.length, m_StrBinLengthFld, sizeof (sBinRecord.primary_key.length)) ;

	memset(&sMidBinBlkRecord_Global,0x00,sizeof(sMidBinBlkRecord_Global));
	memcpy ((LPTSTR)sMidBinBlkRecord_Global.card_type, strType, sizeof (sBinRecord.primary_key.type)) ;
	memcpy ((LPTSTR)sMidBinBlkRecord_Global.bin_low, m_StrBinLowFld, sizeof (sBinRecord.primary_key.low)) ;
	memcpy ((LPTSTR)sMidBinBlkRecord_Global.bin_length, m_StrBinLengthFld, sizeof (sBinRecord.primary_key.length)) ;
	memcpy(sMidBinBlkRecord_Global.bin_high,sBinRecord.high,sizeof(sMidBinBlkRecord_Global.bin_high));
	memcpy(sMidBinBlkRecord_Global.pan_length,sBinRecord.pan_length,sizeof(sMidBinBlkRecord_Global.pan_length));

   int rcode = txdsapi_delete_record ((pBYTE)&sBinRecord, sizeof ( BIN01), BIN01_DATA, strErrorMessage) ;
   if ( rcode != PTEMSG_OK )
   {
      switch( rcode )
      {
         case PTEMSG_NOT_FOUND:
            sprintf( strErrorMessage, "ERROR: Specified BIN range does not exist" );
         break;
      }

      m_StrMessageLbl = strErrorMessage;
		m_nDialogMode = MODE_DONE;
		if (!UpdateData(FALSE)){
			AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
		}
      
   }
   else
   {


	   /*Bin range table is deleted now delete corresponding MidBinBlk table*/
	   int rcode = txdsapi_delete_record ((pBYTE)&sMidBinBlkRecord_Global, sizeof ( MIDBINBLK01), MIDBINBLK01_DATA, strErrorMessage) ;

	    if ( rcode != PTEMSG_OK )
		{
		  switch( rcode )
		  {
			 case PTEMSG_NOT_FOUND:
				sprintf( strErrorMessage, "INFO: For  Specified BIN range MID blocking records  does not exist" );
			 break;
		  }
		    
		}
        memcpy ((LPSTR)m_strCurrentData, "Record Deleted", sizeof (m_strCurrentData));
		memset ( m_strPreviousData, 0, sizeof (m_strPreviousData));
		memset ( m_strFieldTag, 0, sizeof (m_strPreviousData));
        PopulateHistoryTable();

        m_StrMessageLbl = "Record Deleted";

		m_StrBinLowFld = _T("");	
		m_StrBinLengthFld = _T("");
		m_StrType = _T("");

		if (!UpdateData(FALSE)){
			AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
		}
		if ( pGeneralPage->m_bGeneral )
		{
			pGeneralPage->CleanBinDialog();
			pGeneralPage->EnableBinDialog(FALSE);
		}

		if ( pBlockingPage->m_bBlocking )
		{
			pBlockingPage->CleanBlockingDialog();
		}

		if ( pMccBlockingPage->m_bMCCBlocking )
		{
			pMccBlockingPage->CleanBlockingDialog();
		}

		if ( pMidBlockingPage->m_bBlocking )
		{
			pMidBlockingPage->CleanBlockingDialog();
		}

		if ( pChangeHistoryPage->m_bChangeHistory )
			pChangeHistoryPage->CleanChangeHistory();
		((CPropertySheet*) this->GetParent())->SetActivePage(0);
		SetModified ( FALSE );
   }
	
}

void CBinDefinition::OnUpdateBinButton() 
{
   HST01 sHistoryRecord,sHistoryRecord_MID;
	char strErrorMessage[200];
	char strFilePrimaryKey[30];

   m_nDialogMode = MODE_EDIT;
   pGeneralPage->m_nDialogMode = MODE_EDIT;   
   pChangeHistoryPage->m_nDialogMode = MODE_EDIT;
   pBlockingPage->m_nDialogMode = MODE_EDIT;
   pMccBlockingPage->m_nDialogMode = MODE_EDIT;
	pMidBlockingPage->m_nDialogMode = MODE_EDIT;

   m_StrMessageLbl = "";

    if (!CheckDialogForCompleteness())
        return;

	ZeroMemory ( &sBinOldRecord, sizeof (BIN01));
    GetBinType();
	memcpy ((LPTSTR)sBinOldRecord.primary_key.type, strType, sizeof (sBinOldRecord.primary_key.type)) ;
	memcpy ((LPTSTR)sBinOldRecord.primary_key.low, m_StrBinLowFld, sizeof (sBinOldRecord.primary_key.low)) ;
	memcpy ((LPTSTR)sBinOldRecord.primary_key.length, m_StrBinLengthFld, sizeof (sBinOldRecord.primary_key.length)) ;
  
   //************* this should be get_bin_data ************************
	int rcode = txdsapi_get_record( (pBYTE)&sBinOldRecord, sizeof (BIN01), BIN01_DATA,strErrorMessage);
   if ( rcode != PTEMSG_OK )
   {
      switch( rcode )
      {
         case PTEMSG_NOT_FOUND:
            sprintf( strErrorMessage, "ERROR: Specified BIN range does not exist" );
            break;
      }

      m_StrMessageLbl = strErrorMessage;
		m_nDialogMode = MODE_DONE;
		if (!UpdateData(FALSE)){
			AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
		}
      return;      
   }
	memset(&sOldMidBinBlkRecord_Global,0x00,sizeof (sOldMidBinBlkRecord_Global));
	memcpy ((LPTSTR)sOldMidBinBlkRecord_Global.card_type, strType, sizeof (sOldMidBinBlkRecord_Global.card_type)) ;
	memcpy ((LPTSTR)sOldMidBinBlkRecord_Global.bin_low, m_StrBinLowFld, sizeof (sOldMidBinBlkRecord_Global.bin_low)) ;
	memcpy ((LPTSTR)sOldMidBinBlkRecord_Global.bin_length, m_StrBinLengthFld, sizeof (sOldMidBinBlkRecord_Global.bin_length)) ;
    memcpy((LPTSTR)sOldMidBinBlkRecord_Global.bin_high,sBinOldRecord.high,sizeof(sOldMidBinBlkRecord_Global.bin_high));
	memcpy((LPTSTR)sOldMidBinBlkRecord_Global.pan_length,sBinOldRecord.pan_length,sizeof(sOldMidBinBlkRecord_Global.pan_length));
   int rcode1 = txdsapi_get_record( (pBYTE)&sOldMidBinBlkRecord_Global, sizeof (MIDBINBLK01), MIDBINBLK01_DATA,strErrorMessage);
   if ( rcode1 != PTEMSG_OK )
   {
	   switch( rcode )
      {
         case PTEMSG_NOT_FOUND:
            sprintf( strErrorMessage, "For Specified BIN range MID's blocking table is empty" );
            break;
      }
	    m_StrMessageLbl = strErrorMessage;

   }
    GetBinType();

	pGeneralPage->m_StrBinLowFld = m_StrBinLowFld;
    pGeneralPage->m_StrBinLengthFld = m_StrBinLengthFld;
	pGeneralPage->m_StrTypeFld = m_StrType;
	
	pBlockingPage->m_StrBinLowFld = m_StrBinLowFld;
    pBlockingPage->m_StrBinLengthFld = m_StrBinLengthFld;
	pBlockingPage->m_StrBinTypeFld = m_StrType;	

	pMccBlockingPage->m_StrBinLowFld1 = m_StrBinLowFld;
    pMccBlockingPage->m_StrBinLengthFld1 = m_StrBinLengthFld;
	pMccBlockingPage->m_StrMccBinTypeFld1 = m_StrType;
	
	pMidBlockingPage->m_StrBinLowFld = m_StrBinLowFld;
    pMidBlockingPage->m_StrBinLengthFld = m_StrBinLengthFld;
	pMidBlockingPage->m_StrBinTypeFld = m_StrType;	

    ZeroMemory (&sHistoryRecord, sizeof (sHistoryRecord));

	memset ( strFilePrimaryKey, 0, sizeof (strFilePrimaryKey));
	strcpy ( strFilePrimaryKey, (char *)sBinOldRecord.primary_key.length);
	strcat ( strFilePrimaryKey, (char *)sBinOldRecord.primary_key.low);
	strcat ( strFilePrimaryKey, (char *)sBinOldRecord.primary_key.type);

    memcpy ((LPSTR)(sHistoryRecord.primary_key.unique_id), "                 ", sizeof (sHistoryRecord.primary_key.unique_id));
    memcpy ((LPSTR)(sHistoryRecord.file_name), "BIN01", sizeof (sHistoryRecord.file_name));
    memcpy ((LPSTR)(sHistoryRecord.file_primary_key), (LPSTR)strFilePrimaryKey, sizeof (sHistoryRecord.file_primary_key));

    memcpy ( &(pChangeHistoryPage->sHistoryRecord), &sHistoryRecord, sizeof(sHistoryRecord));
    
	((CPropertySheet*) this->GetParent())->SetActivePage(1);  
  
	pChangeHistoryPage->bNext = FALSE;

	((CPropertySheet*) this->GetParent())->SetWindowText("Update BIN");
	//set version info in the title bar
	CString appProdVer;
	CString lpszTitle;
	appProdVer.LoadString(IDS_PRODUCT_VERSION);
	((CPropertySheet*) this->GetParent())->GetWindowText(lpszTitle);
	lpszTitle = lpszTitle + " - Version - ";
	lpszTitle += appProdVer;
	((CPropertySheet*) this->GetParent())->SetWindowText(lpszTitle);

	if ( pGeneralPage->m_bGeneral )
		pGeneralPage->PopulateBinDialogFromDatabase ( sBinOldRecord );

	if ( pBlockingPage->m_bBlocking )
		pBlockingPage->PopulateBinDialogFromDatabase ( sBinOldRecord );
   else
      memcpy ( &pBlockingPage->sBinRecord, &sBinOldRecord, sizeof ( BIN01 ) );

   if ( pMccBlockingPage->m_bMCCBlocking )
		pMccBlockingPage->PopulateBinDialogFromDatabase( sBinOldRecord );
   else
      memcpy ( &pMccBlockingPage->sBinRecord, &sBinOldRecord, sizeof ( BIN01 ) );

	if ( pMidBlockingPage->m_bBlocking )
	{
		pMidBlockingPage->PopulateBinDialogFromDatabase ( sOldMidBinBlkRecord_Global );
	}	
   else
      memcpy ( &sMidBinBlkRecord_Global, &sOldMidBinBlkRecord_Global, sizeof ( MIDBINBLK01 ) );

	if ( pChangeHistoryPage->m_bChangeHistory )
	{
		pChangeHistoryPage->PopulateHistoryDialog();
	}

	m_CntlBinLowFld.EnableWindow(FALSE);
	m_CntlBinLengthFld.EnableWindow(FALSE);
	m_CntlType.EnableWindow(FALSE);
	
}

void CBinDefinition::OnViewBinButton() 
{
   BIN01 sBinRecord;
   HST01 sHistoryRecord,sHistoryRecord_MID;
	char strErrorMessage[200];
	char strFilePrimaryKey[30];
	int flag = 0 ;
   m_nDialogMode = MODE_VIEW;
   pGeneralPage->m_nDialogMode = MODE_VIEW;   
   pChangeHistoryPage->m_nDialogMode = MODE_VIEW;
   pBlockingPage->m_nDialogMode = MODE_VIEW;	
   pMccBlockingPage->m_nDialogMode = MODE_VIEW;
	pMidBlockingPage->m_nDialogMode = MODE_VIEW;
   m_StrMessageLbl = "";
	

   if (!CheckDialogForCompleteness())
     return;

    /*m_pUserSheet->m_AccessPage.m_psp.dwFlags &= ~PSP_HASHELP; 
    m_pUserSheet->m_WorkstationPage.m_psp.dwFlags &= ~PSP_HASHELP; 
    m_pUserSheet->m_ChangeHistoryPage.m_psp.dwFlags &= ~PSP_HASHELP;

    m_pUserSheet->m_psh.dwFlags |= PSH_NOAPPLYNOW ; 
    m_pUserSheet->m_psh.dwFlags &= ~PSH_HASHELP;*/


    GetBinType();
	memcpy ((LPTSTR)sBinRecord.primary_key.type, strType, sizeof (sBinRecord.primary_key.type)) ;
	memcpy ((LPTSTR)sBinRecord.primary_key.low, m_StrBinLowFld, sizeof (sBinRecord.primary_key.low)) ;
	memcpy ((LPTSTR)sBinRecord.primary_key.length, m_StrBinLengthFld, sizeof (sBinRecord.primary_key.length)) ;
  
	int rcode = txdsapi_get_record ( (pBYTE)&sBinRecord, sizeof (BIN01), BIN01_DATA,strErrorMessage);
    if ( rcode != PTEMSG_OK )
    {
      switch( rcode )
      {
         case PTEMSG_NOT_FOUND:
            sprintf( strErrorMessage, "ERROR: Specified BIN range does not exist" );
            break;
      }

		m_StrMessageLbl = strErrorMessage;
		m_nDialogMode = MODE_DONE;
		if (!UpdateData(FALSE)){
			AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
		}
      
      return;
    }

    GetBinType();
	memset(&sMidBinBlkRecord_Global,0x00,sizeof(MIDBINBLK01));
	memcpy ((LPTSTR)sMidBinBlkRecord_Global.card_type, strType, sizeof (sMidBinBlkRecord_Global.card_type)) ;
	memcpy ((LPTSTR)sMidBinBlkRecord_Global.bin_low, m_StrBinLowFld, sizeof (sMidBinBlkRecord_Global.bin_low)) ;
	memcpy ((LPTSTR)sMidBinBlkRecord_Global.bin_length, m_StrBinLengthFld, sizeof (sMidBinBlkRecord_Global.bin_length)) ;
	memcpy((LPTSTR)sMidBinBlkRecord_Global.bin_high,sBinRecord.high,sizeof(sMidBinBlkRecord_Global.bin_high));
	memcpy((LPTSTR)sMidBinBlkRecord_Global.pan_length,sBinRecord.pan_length,sizeof(sMidBinBlkRecord_Global.pan_length));
	int rcode1 = txdsapi_get_record ( (pBYTE)&sMidBinBlkRecord_Global, sizeof (MIDBINBLK01), MIDBINBLK01_DATA,strErrorMessage);
    if ( rcode1 != PTEMSG_OK )
    {
      switch( rcode )
      {
         case PTEMSG_NOT_FOUND:

            sprintf( strErrorMessage, "INFO: For Specified BIN range MID blocking table is empty" );
            break;

      }

	}
	pGeneralPage->m_StrBinLowFld = m_StrBinLowFld;
   pGeneralPage->m_StrBinLengthFld = m_StrBinLengthFld;
	pGeneralPage->m_StrTypeFld = m_StrType;	
	
	pBlockingPage->m_StrBinLowFld = m_StrBinLowFld;
   pBlockingPage->m_StrBinLengthFld = m_StrBinLengthFld;
	pBlockingPage->m_StrBinTypeFld = m_StrType;	
   
   pMccBlockingPage->m_StrBinLowFld1 = m_StrBinLowFld;
   pMccBlockingPage->m_StrBinLengthFld1 = m_StrBinLengthFld;
	pMccBlockingPage->m_StrMccBinTypeFld1 = m_StrType;	

	pMidBlockingPage->m_StrBinLowFld = m_StrBinLowFld;
   pMidBlockingPage->m_StrBinLengthFld = m_StrBinLengthFld;
	pMidBlockingPage->m_StrBinTypeFld = m_StrType;	

   ZeroMemory (&sHistoryRecord, sizeof (sHistoryRecord));

	memset ( strFilePrimaryKey, 0, sizeof (strFilePrimaryKey));
	strcpy ( strFilePrimaryKey, (char *)sBinRecord.primary_key.length);
	strcat ( strFilePrimaryKey, (char *)sBinRecord.primary_key.low);
	strcat ( strFilePrimaryKey, (char *)sBinRecord.primary_key.type);

    memcpy ((LPSTR)(sHistoryRecord.primary_key.unique_id), "                 ", sizeof (sHistoryRecord.primary_key.unique_id));
    memcpy ((LPSTR)(sHistoryRecord.file_name), "BIN01", sizeof (sHistoryRecord.file_name));
    memcpy ((LPSTR)(sHistoryRecord.file_primary_key), (LPSTR)strFilePrimaryKey, sizeof (sHistoryRecord.file_primary_key));

    memcpy ( &(pChangeHistoryPage->sHistoryRecord), &sHistoryRecord, sizeof(sHistoryRecord));

	((CPropertySheet*) this->GetParent())->SetActivePage(1);  
	pChangeHistoryPage->bNext = FALSE;

	((CPropertySheet*) this->GetParent())->SetWindowText("View BIN");
	//set version info in the title bar
	CString appProdVer;
	CString lpszTitle;
	appProdVer.LoadString(IDS_PRODUCT_VERSION);
	((CPropertySheet*) this->GetParent())->GetWindowText(lpszTitle);
	lpszTitle = lpszTitle + " - Version - ";
	lpszTitle += appProdVer;
	((CPropertySheet*) this->GetParent())->SetWindowText(lpszTitle);

	if ( pGeneralPage->m_bGeneral )
		pGeneralPage->PopulateBinDialogFromDatabase ( sBinRecord );

   if ( pBlockingPage->m_bBlocking )
		pBlockingPage->PopulateBinDialogFromDatabase ( sBinRecord );
    else
      memcpy ( &pBlockingPage->sBinRecord, &sBinRecord, sizeof ( BIN01 ) );

   if ( pMccBlockingPage->m_bMCCBlocking )
		pMccBlockingPage->PopulateBinDialogFromDatabase ( sBinRecord );
   else
	    memcpy ( &pMccBlockingPage->sBinRecord, &sBinRecord, sizeof ( BIN01 ) );


   if ( pMidBlockingPage->m_bBlocking )
   {
   
		pMidBlockingPage->PopulateBinDialogFromDatabase (sMidBinBlkRecord_Global);
   }

	memcpy ( &pMidBlockingPage->sMidBinBlkRecord, &sMidBinBlkRecord_Global, sizeof ( MIDBINBLK01 ) );

	if ( pChangeHistoryPage->m_bChangeHistory )
	{
		pChangeHistoryPage->PopulateHistoryDialog();

	}


	m_CntlBinLowFld.EnableWindow(TRUE);
	m_CntlBinLengthFld.EnableWindow(TRUE);
	m_CntlType.EnableWindow(TRUE);
	
}

void CBinDefinition::OnListBinButton() 
{
	CBinList listDlg;
    int nReturnCode;

	UpdateData(TRUE);
	m_StrMessageLbl = "";
	UpdateData(FALSE);
		

   // listDlg.m_nIndicator = MODE_GROUP;
 //   listDlg.SetWindowText ( "User List");
	listDlg.m_nDialogMode = MODE_BIN;
	listDlg.strItem0 = m_StrBinLengthFld;
	listDlg.strItem1 = m_StrBinLowFld;
	listDlg.strItem2 = m_StrType.Left(1);

   /* Differentiate between Debit and Deferred. */
   if ( listDlg.strItem2 == 'D' )
   {
      if ( m_StrType.Left(3) == "DEF" )
         listDlg.strItem2 = 'E';
   }


    nReturnCode = listDlg.DoModal();
    if ( nReturnCode == IDOK)
	{
        m_StrBinLengthFld = strLength;
		m_StrBinLowFld = strLow;
		m_StrType = strType1;
	}
        
    if (!UpdateData(FALSE)){
		AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
	}		
	
}

BOOL CBinDefinition::CheckDialogForCompleteness()
{
    CMyUtils myUtilsObject;
	int nBinLength;
	int nCurLength;

    if (!UpdateData(TRUE)){
		AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
	}

    if (myUtilsObject.IsEmpty (m_StrBinLengthFld, this))
    {
		m_StrMessageLbl = "Please enter BIN Length";	
		m_CntlBinLowFld.SetFocus();
		if (!UpdateData(FALSE)){
			AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
		}
		m_nDialogMode = MODE_DONE;
		return FALSE;
	}

	if (myUtilsObject.IsEmpty (m_StrBinLowFld, this))
    {
		m_StrMessageLbl = "Please enter BIN Low";	
		m_CntlBinLowFld.SetFocus();
		if (!UpdateData(FALSE)){
			AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
		}
		m_nDialogMode = MODE_DONE;
		return FALSE;
	}

	nBinLength = atoi ( m_StrBinLengthFld );
	nCurLength = m_StrBinLowFld.GetLength();

	if ( nCurLength != nBinLength )
	{
		m_StrMessageLbl = "ERROR: Incorrect number of digits in BIN Low";	
		m_CntlBinLowFld.SetFocus();
		if (!UpdateData(FALSE)){
			AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
		}
		m_nDialogMode = MODE_DONE;
		return FALSE;
	}

    

    return TRUE;
}

BOOL CBinDefinition::GetBinType()
{
	if ( strcmp (m_StrType, "CREDIT") == 0)
         memcpy ((LPTSTR) strType,	"C", sizeof(strType)); 
	if ( strcmp (m_StrType, "DEBIT")== 0)
         memcpy ((LPTSTR) strType,	"D", sizeof(strType)); 
	if ( strcmp (m_StrType, "DEFERRED") == 0)
         memcpy ((LPTSTR) strType,	"E", sizeof(strType)); 
	if ( strcmp (m_StrType, "FLEET") == 0)   
          memcpy ((LPTSTR) strType,	"F", sizeof(strType));
	if ( strcmp (m_StrType, "LOYALTY")== 0)
          memcpy ((LPTSTR) strType,	"L", sizeof(strType)); 
	
	return TRUE;
}

BOOL CBinDefinition::OnApply() 
{
   BIN01    sBinDatabaseStructure;
   CHAR     strErrorMessage[200];
   CHAR     tempstr[10];
   CMyUtils myUtilsObject;
   CString  temp;
   BYTE     mod10_cash_bonus;
   BYTE     prev_mod10_cash_bonus;
   INT      mod10;
   INT      cash_bonus;
   INT      fguard;


	if (m_nDialogMode == MODE_ADD)
	{
		((CPropertySheet*) this->GetParent())->SetActivePage(1);
		if ( !pGeneralPage->CheckDialogForCompleteness())
		{
		//	((CPropertySheet*) this->GetParent())->SetActivePage(1);
			return FALSE;
		}
		((CPropertySheet*) this->GetParent())->SetActivePage(3);
		if ( !pMccBlockingPage->CheckDialogForCompleteness())
		{
		//	((CPropertySheet*) this->GetParent())->SetActivePage(1);
			return FALSE;
		}
		memset(&sMidBinBlkRecord_Global, 0, sizeof(MIDBINBLK01)) ;
		memset(&sBinDatabaseStructure, 0, sizeof(sBinDatabaseStructure)) ;
		if (!PopulateBinDatabaseStructureFromDialog (&sBinDatabaseStructure)) 
		{		
			return FALSE;
		}
		int rcode = txdsapi_insert_record((pBYTE)&sBinDatabaseStructure, sizeof (BIN01), BIN01_DATA, strErrorMessage);
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
			/*after bin range inserted correclty now inserting the merchant blocking table*/
			memcpy(sMidBinBlkRecord_Global.bin_high,sBinDatabaseStructure.high,sizeof(sMidBinBlkRecord_Global.bin_high));
			memcpy(sMidBinBlkRecord_Global.bin_length,sBinDatabaseStructure.primary_key.length,sizeof(sMidBinBlkRecord_Global.bin_length));
			memcpy(sMidBinBlkRecord_Global.bin_low,sBinDatabaseStructure.primary_key.low,sizeof(sMidBinBlkRecord_Global.bin_low));
			memcpy(sMidBinBlkRecord_Global.card_type,sBinDatabaseStructure.primary_key.type,sizeof(sMidBinBlkRecord_Global.card_type));
			memcpy(sMidBinBlkRecord_Global.pan_length,sBinDatabaseStructure.pan_length,sizeof(sMidBinBlkRecord_Global.pan_length));
			int rcode = txdsapi_insert_record((pBYTE)&sMidBinBlkRecord_Global, sizeof (MIDBINBLK01), MIDBINBLK01_DATA, strErrorMessage);
			if (rcode != PTEMSG_OK)
			{
				m_StrMessageLbl = strErrorMessage;
				if (!UpdateData(FALSE)){
					AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
				}
				((CPropertySheet*) this->GetParent())->SetActivePage(0);
			//	return FALSE;
			}
			memcpy ((LPSTR)m_strCurrentData, "Record Added", sizeof (m_strCurrentData));
			memset ( m_strPreviousData, 0, sizeof (m_strPreviousData));
			memset ( m_strFieldTag, 0, sizeof (m_strPreviousData));
			PopulateHistoryTable();
			((CPropertySheet*) this->GetParent())->SetActivePage(0);
			m_StrMessageLbl = "Record Added";	
			m_CntlBinLengthFld.SetFocus();
			if (!UpdateData(FALSE))
			{
				AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
			}
			pGeneralPage->CleanBinDialog();
			pGeneralPage->EnableBinDialog(FALSE);
			
			 if ( pBlockingPage->m_bBlocking )
			 {
				   pBlockingPage->CleanBlockingDialog();
				pBlockingPage->m_nDialogMode = MODE_DONE;
			 }

			 if ( pMccBlockingPage->m_bMCCBlocking )
			 {
				   pMccBlockingPage->CleanBlockingDialog();
				pMccBlockingPage->m_nDialogMode = MODE_DONE;
			 }

			 if ( pMidBlockingPage->m_bBlocking )
			 {
				   pMidBlockingPage->CleanBlockingDialog();
					pMidBlockingPage->m_nDialogMode = MODE_DONE;
			 }
			 if ( pChangeHistoryPage->m_bChangeHistory )
				pChangeHistoryPage->CleanChangeHistory();

				SetModified ( FALSE );
				m_nDialogMode = MODE_DONE;
				m_CntlBinLowFld.EnableWindow(TRUE);
				m_CntlType.EnableWindow(TRUE);
				m_CntlBinLengthFld.EnableWindow(TRUE);
				//return CPropertyPage::OnApply();
			 return FALSE;
		} 
	}

	if (m_nDialogMode == MODE_EDIT)
	{ 
		((CPropertySheet*) this->GetParent())->SetActivePage(1);
		if ( !pGeneralPage->CheckDialogForCompleteness())
		{
		//	((CPropertySheet*) this->GetParent())->SetActivePage(1);
			return FALSE;
		}
		((CPropertySheet*) this->GetParent())->SetActivePage(3);
		if ( !pMccBlockingPage->CheckDialogForCompleteness())
		{
		//	((CPropertySheet*) this->GetParent())->SetActivePage(1);
			return FALSE;
		}

		
		BIN01 sBinDatabaseStructure;
        memset(&sBinDatabaseStructure, 0, sizeof(sBinDatabaseStructure)) ;
		memcpy (&sBinDatabaseStructure, &sBinOldRecord, sizeof(sBinDatabaseStructure)) ; 
		
		PopulateBinDatabaseStructureFromDialog (&sBinDatabaseStructure );

//		pAccessPage->PopulateHistoryTable();

		int rcode = txdsapi_update_record((pBYTE)&sBinDatabaseStructure, sizeof (BIN01), BIN01_DATA,strErrorMessage);
		if (rcode != PTEMSG_OK)
		{
			m_StrMessageLbl = strErrorMessage;
			((CPropertySheet*) this->GetParent())->SetActivePage(0);
			if (!UpdateData(FALSE)){
				AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
			}
			return FALSE;
		}
		else
		{		
			memcpy(sMidBinBlkRecord_Global.bin_high,sBinDatabaseStructure.high,sizeof(sMidBinBlkRecord_Global.bin_high));
			memcpy(sMidBinBlkRecord_Global.bin_length,sBinDatabaseStructure.primary_key.length,sizeof(sMidBinBlkRecord_Global.bin_length));
			memcpy(sMidBinBlkRecord_Global.bin_low,sBinDatabaseStructure.primary_key.low,sizeof(sMidBinBlkRecord_Global.bin_low));
			memcpy(sMidBinBlkRecord_Global.card_type,sBinDatabaseStructure.primary_key.type,sizeof(sMidBinBlkRecord_Global.card_type));
			memcpy(sMidBinBlkRecord_Global.pan_length,sBinDatabaseStructure.pan_length,sizeof(sMidBinBlkRecord_Global.pan_length));


   			pBlockingPage->CompareStructures( sBinOldRecord, sBinDatabaseStructure );
		
			pMidBlockingPage->CompareStructures();

			pMccBlockingPage->CompareStructures( sBinOldRecord);


			int rcode1 = txdsapi_update_record((pBYTE)&sMidBinBlkRecord_Global, sizeof (MIDBINBLK01), MIDBINBLK01_DATA,strErrorMessage);
			if (rcode1 != PTEMSG_OK)
			{
				m_StrMessageLbl = strErrorMessage;
				((CPropertySheet*) this->GetParent())->SetActivePage(0);
				if (!UpdateData(FALSE)){
					AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
				}
				return FALSE;
			}

		 if (strcmp ( pGeneralPage->m_StrBinDescriptionFld, (char *)sBinOldRecord.description))
		 {
			memcpy(m_strCurrentData, pGeneralPage->m_StrBinDescriptionFld, sizeof (m_strCurrentData));
			memcpy(m_strPreviousData, sBinOldRecord.description, sizeof (m_strPreviousData));
			memcpy(m_strFieldTag, "Description", sizeof (m_strFieldTag));
			
			PopulateHistoryTable();

		 }

		 if (strcmp ( pGeneralPage->m_StrPanLengthFld, (char *)sBinOldRecord.pan_length))
		 {
			memcpy(m_strCurrentData, pGeneralPage->m_StrPanLengthFld, sizeof (m_strCurrentData));
			memcpy(m_strPreviousData, sBinOldRecord.pan_length, sizeof (m_strPreviousData));
			memcpy(m_strFieldTag, "Pan Length", sizeof (m_strFieldTag));
			
			PopulateHistoryTable();

		 }

		 if (strcmp ( pGeneralPage->m_StrAuthorizationFld, (char *)sBinOldRecord.caf_profile_id))
		 {
			memcpy(m_strCurrentData, pGeneralPage->m_StrAuthorizationFld, sizeof (m_strCurrentData));
			memcpy(m_strPreviousData, sBinOldRecord.caf_profile_id, sizeof (m_strPreviousData));
			memcpy(m_strFieldTag, "Auth Profile", sizeof (m_strFieldTag));
			
			PopulateHistoryTable();

		 }

		 if (strcmp ( pGeneralPage->m_StrNetworkFld, (char *)sBinOldRecord.destination_name))
		 {
			memcpy(m_strCurrentData, pGeneralPage->m_StrNetworkFld, sizeof (m_strCurrentData));
			memcpy(m_strPreviousData, sBinOldRecord.destination_name, sizeof (m_strPreviousData));
			memcpy(m_strFieldTag, "Network ID", sizeof (m_strFieldTag));
			
			PopulateHistoryTable();

		 }

		 if (strcmp ( pGeneralPage->m_StrCardTypeFld, (char *)sBinOldRecord.card_type))
		 {
			memcpy(m_strCurrentData, pGeneralPage->m_StrCardTypeFld, sizeof (m_strCurrentData));
			memcpy(m_strPreviousData, sBinOldRecord.card_type, sizeof (m_strPreviousData));
			memcpy(m_strFieldTag, "Card Type", sizeof (m_strFieldTag));
			
			PopulateHistoryTable();

		 }

		 if (strcmp ( pGeneralPage->m_StrAltDestinationNameFld, (char *)sBinOldRecord.alt_destination_name))
		 {
			memcpy(m_strCurrentData, pGeneralPage->m_StrAltDestinationNameFld, sizeof (m_strCurrentData));
			memcpy(m_strPreviousData, sBinOldRecord.card_type, sizeof (m_strPreviousData));
			memcpy(m_strFieldTag, "Alternate Destination Name", sizeof (m_strFieldTag));
			
			PopulateHistoryTable();

		 }
		 temp = sBinOldRecord.card_family;

		if ( temp == CARD_AMEX )
			temp = "AMEX";
		else if ( temp == CARD_VISA )
			temp = "VISA";
		else if ( temp == CARD_MASTERCARD )
			temp = "MASTERCARD";
		else if ( temp == CARD_JCB )
			temp = "JCB";
		else if ( temp == CARD_CUP )
			temp = "CUP";
		else if ( temp == CARD_DCC )
			temp = "DCC";
		else if ( temp == CARD_DINERS )
			temp = "DINERS";
		else if ( temp == CARD_DUAL )
			temp = "DUAL";
		else if ( temp == CARD_FLEET )
			temp = "FLEET";

		 if (strcmp ( pGeneralPage->m_StrCardFamily, temp))
		 {
			memcpy(m_strCurrentData, pGeneralPage->m_StrCardFamily, sizeof (m_strCurrentData));
			memcpy(m_strPreviousData, temp, sizeof (m_strPreviousData));
			memcpy(m_strFieldTag, "Card Family", sizeof (m_strFieldTag));
			
			PopulateHistoryTable();

		 }
		 temp = sBinOldRecord.credit_subtype;

		 if ( temp == "F" )
			temp = "FLEET";
		 else if ( temp == "G" )
			temp = "GUARANTOR";
		 else if ( temp == "L" )
			temp = "LOYALTY";
		 else if (temp == "P" )
		    temp = "PREPAID";
		 else if (temp == "E" )
		    temp = "ELECTRON";

		 if (strcmp ( pGeneralPage->m_StrCreditSubtype, temp))
		 {
			memcpy(m_strCurrentData, pGeneralPage->m_StrCreditSubtype, sizeof (m_strCurrentData));
			memcpy(m_strPreviousData, temp, sizeof (m_strPreviousData));
			memcpy(m_strFieldTag, "Card Subtype", sizeof (m_strFieldTag));
			
			PopulateHistoryTable();

		 }
		 if (strcmp ( pGeneralPage->m_StrSettleFileFld, (char *)sBinOldRecord.settle_file_prefix))
		 {
			memcpy(m_strCurrentData, pGeneralPage->m_StrSettleFileFld, sizeof (m_strCurrentData));
			memcpy(m_strPreviousData, sBinOldRecord.settle_file_prefix, sizeof (m_strPreviousData));
			memcpy(m_strFieldTag, "Settle File", sizeof (m_strFieldTag));
			
			PopulateHistoryTable();

		 }	

         /* Get previous value of Mod10 and Cash Bonus flags. */
         prev_mod10_cash_bonus = sBinOldRecord.mod_check[0];

         /* Get current value of Mod10, Cash Bonus, and Fraudguard flags. */
         if ( pGeneralPage->m_StrModCheck == "MOD10" )
            mod10 = MOD10_ON;
         else
            mod10 = 0x00;

         if ( pGeneralPage->m_BinCashBonusFld.GetCheck() )
            cash_bonus = CASH_ON;
         else
            cash_bonus = 0x00;

         if ( pGeneralPage->m_BinFraudguardFld.GetCheck() )
            fguard = FGUARD_ON;
         else
            fguard = 0x00;

         mod10_cash_bonus = fguard | cash_bonus | mod10;

         if ( mod10_cash_bonus != prev_mod10_cash_bonus )
         {
            /* One or both of these has changed.  Update history table. */
            memset( tempstr, 0x00, sizeof(tempstr) );
            tempstr[0] = prev_mod10_cash_bonus;
            memcpy( m_strPreviousData, tempstr, sizeof(tempstr) );

            tempstr[0] = mod10_cash_bonus;
            memcpy( m_strCurrentData, tempstr, sizeof(tempstr) );

            memcpy( m_strFieldTag, "Mod Check", sizeof(m_strFieldTag) );
            PopulateHistoryTable();
         }

         m_StrMessageLbl = "Record Updated";
			if (!UpdateData(FALSE)){
            AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);
         }
			pGeneralPage->CleanBinDialog();
			pGeneralPage->EnableBinDialog(FALSE);			
			
		    if ( pBlockingPage->m_bBlocking )
			{
				pBlockingPage->CleanBlockingDialog();
			}
		    if ( pMccBlockingPage->m_bMCCBlocking )
			{
				pMccBlockingPage->CleanBlockingDialog();
						
			}
			if ( pMidBlockingPage->m_bBlocking )
			{
				pMidBlockingPage->CleanBlockingDialog();
			}

			if ( pChangeHistoryPage->m_bChangeHistory )
				pChangeHistoryPage->CleanChangeHistory();

			((CPropertySheet*) this->GetParent())->SetActivePage(0);
			m_CntlBinLowFld.EnableWindow(TRUE);
			m_CntlType.EnableWindow(TRUE);
			m_CntlBinLengthFld.EnableWindow(TRUE);
			SetModified ( FALSE );
			m_nDialogMode = MODE_DONE;
         pBlockingPage->m_nDialogMode = MODE_DONE;
		 pMccBlockingPage->m_nDialogMode = MODE_DONE;
			//return CPropertyPage::OnApply();
         return FALSE;
		}
	
    }
	
	//return CPropertyPage::OnApply();
   return FALSE;
}

BOOL CBinDefinition::PopulateBinDatabaseStructureFromDialog (pBIN01 psBinDatabaseStructure)
{
   INT  mod10;
   INT  cash_bonus;
   INT  fguard;

   memcpy ((LPTSTR)psBinDatabaseStructure->description, pGeneralPage->m_StrBinDescriptionFld, sizeof(psBinDatabaseStructure->description)); 
   memcpy ((LPTSTR)psBinDatabaseStructure->primary_key.low, m_StrBinLowFld, sizeof(psBinDatabaseStructure->primary_key.low)); 
   memcpy ((LPTSTR)psBinDatabaseStructure->primary_key.length, m_StrBinLengthFld, sizeof(psBinDatabaseStructure->primary_key.length)); 	 
   memcpy ((LPTSTR)psBinDatabaseStructure->high, pGeneralPage->m_StrBinHighFld, sizeof(psBinDatabaseStructure->high)); 
   memcpy ((LPTSTR)psBinDatabaseStructure->pan_length, pGeneralPage->m_StrPanLengthFld, sizeof( psBinDatabaseStructure->pan_length)); 
   memcpy ((LPTSTR)psBinDatabaseStructure->caf_profile_id, pGeneralPage->m_StrAuthorizationFld, sizeof(psBinDatabaseStructure->caf_profile_id)); 
   memcpy ((LPTSTR)psBinDatabaseStructure->settle_file_prefix, pGeneralPage->m_StrSettleFileFld, sizeof(psBinDatabaseStructure->settle_file_prefix)); 
   memcpy ((LPTSTR)psBinDatabaseStructure->card_type, pGeneralPage->m_StrCardTypeFld, sizeof (psBinDatabaseStructure->card_type)); 	
   memcpy ((LPTSTR)psBinDatabaseStructure->primary_key.type, strType, sizeof (psBinDatabaseStructure->primary_key.type)); 
   memcpy ((LPTSTR)psBinDatabaseStructure->destination_name, pGeneralPage->m_StrNetworkFld, sizeof (psBinDatabaseStructure->destination_name)); 

   pBlockingPage->PopulateDatabaseStructure( psBinDatabaseStructure );
   pMccBlockingPage->PopulateDatabaseStructure( psBinDatabaseStructure );
	pMidBlockingPage->PopulateDatabaseStructure(  );

   if ( pGeneralPage->m_StrCreditSubtype == "FLEET" )
      memcpy ((LPTSTR)psBinDatabaseStructure->credit_subtype , "F", sizeof (psBinDatabaseStructure->credit_subtype));

   else if ( pGeneralPage->m_StrCreditSubtype == "GUARANTOR" )
      memcpy ((LPTSTR)psBinDatabaseStructure->credit_subtype , "G", sizeof (psBinDatabaseStructure->credit_subtype));

   else if ( pGeneralPage->m_StrCreditSubtype == "LOYALTY" )
      memcpy ((LPTSTR)psBinDatabaseStructure->credit_subtype , "L", sizeof (psBinDatabaseStructure->credit_subtype));

   else if ( pGeneralPage->m_StrCreditSubtype == "PREPAID" )
      memcpy ((LPTSTR)psBinDatabaseStructure->credit_subtype , "P", sizeof (psBinDatabaseStructure->credit_subtype));

   else if ( pGeneralPage->m_StrCreditSubtype == "ELECTRON" )
      memcpy ((LPTSTR)psBinDatabaseStructure->credit_subtype , "E", sizeof (psBinDatabaseStructure->credit_subtype));

   else
      memcpy ((LPTSTR)psBinDatabaseStructure->credit_subtype,  "",  sizeof (psBinDatabaseStructure->credit_subtype));

   if ( pGeneralPage->m_StrCardFamily == "AMEX" )
      memcpy ((LPTSTR)psBinDatabaseStructure->card_family , CARD_AMEX, sizeof (psBinDatabaseStructure->card_family)); 
   else if ( pGeneralPage->m_StrCardFamily == "VISA" )
      memcpy ((LPTSTR)psBinDatabaseStructure->card_family , CARD_VISA, sizeof (psBinDatabaseStructure->card_family)); 
   else if ( pGeneralPage->m_StrCardFamily == "MASTERCARD" )
      memcpy ((LPTSTR)psBinDatabaseStructure->card_family , CARD_MASTERCARD, sizeof (psBinDatabaseStructure->card_family)); 
   else if ( pGeneralPage->m_StrCardFamily == "JCB" )
      memcpy ((LPTSTR)psBinDatabaseStructure->card_family , CARD_JCB, sizeof (psBinDatabaseStructure->card_family)); 
   else if ( pGeneralPage->m_StrCardFamily == "CUP" )
	  memcpy ((LPTSTR)psBinDatabaseStructure->card_family , CARD_CUP, sizeof (psBinDatabaseStructure->card_family)); 
    else if ( pGeneralPage->m_StrCardFamily == "DCC" )
	  memcpy ((LPTSTR)psBinDatabaseStructure->card_family , CARD_DCC, sizeof (psBinDatabaseStructure->card_family)); 
	else if ( pGeneralPage->m_StrCardFamily == "DINERS" )
	  memcpy ((LPTSTR)psBinDatabaseStructure->card_family , CARD_DINERS, sizeof (psBinDatabaseStructure->card_family)); 
    else if ( pGeneralPage->m_StrCardFamily == "DUAL" )
	  memcpy ((LPTSTR)psBinDatabaseStructure->card_family , CARD_DUAL, sizeof (psBinDatabaseStructure->card_family)); 
    else if ( pGeneralPage->m_StrCardFamily == "FLEET" )
	  memcpy ((LPTSTR)psBinDatabaseStructure->card_family , CARD_FLEET, sizeof (psBinDatabaseStructure->card_family)); 


   memcpy ((LPTSTR)psBinDatabaseStructure->alt_destination_name , pGeneralPage->m_StrAltDestinationNameFld, sizeof (psBinDatabaseStructure->alt_destination_name)); 


   /* Combine Mod10, Cash Bonus, and Fraudguard values to form one
    * value that goes into the mod10_check field in the database.
    */
   if ( pGeneralPage->m_StrModCheck == "MOD10" )
      mod10 = MOD10_ON;
   else
      mod10 = 0x00;

   if ( pGeneralPage->m_BinCashBonusFld.GetCheck() )
      cash_bonus = CASH_ON;
   else
      cash_bonus = 0x00;

   if ( pGeneralPage->m_BinFraudguardFld.GetCheck() )
      fguard = FGUARD_ON;
   else
      fguard = 0x00;

   psBinDatabaseStructure->mod_check[0] = fguard | cash_bonus | mod10;


   if ( pGeneralPage->m_LocalCheck.GetCheck() == 1 )
      memcpy ((LPTSTR) psBinDatabaseStructure->auth_ind, "1", sizeof(psBinDatabaseStructure->auth_ind)); 
   else
      memcpy ((LPTSTR) psBinDatabaseStructure->auth_ind, "0", sizeof(psBinDatabaseStructure->auth_ind)); 

   return TRUE;
}

void CBinDefinition::PopulateHistoryTable()
{

    HST01 sHistoryData;
    CHAR dest_str[18];   
	CMyUtils myUtilsObject;
	char strErrorMsg[200] = "";
   CString temp;

    char strDate[9] = "";
    char strTime[7] = "";

	char strPrimaryKey[50];

    memset ( strDate, 0, sizeof (strDate));
    memset ( strTime, 0, sizeof (strTime));
	memset ( strPrimaryKey, 0, sizeof (strPrimaryKey));

	strcpy ( strPrimaryKey, m_StrBinLengthFld);
	strcat ( strPrimaryKey, m_StrBinLowFld );
	strncat ( strPrimaryKey, m_StrType,1 );
    
    myUtilsObject.GetSystemDate(strDate);  

    myUtilsObject.GetSystemTime(strTime);
    pintime_get_gmt ( dest_str );
    ZeroMemory ( &sHistoryData, sizeof (HST01));
    memcpy ((LPTSTR) sHistoryData.file_name,     (LPCTSTR)"BIN01", sizeof (sHistoryData.file_name));
    temp = strDate;
    temp = temp.Right(4) + temp.Left(2) + temp.Mid(2,2);
    
    memcpy ((LPTSTR) sHistoryData.date_modified, temp, sizeof (sHistoryData.date_modified));
    memcpy ((LPTSTR) sHistoryData.file_primary_key,  (LPCTSTR)strPrimaryKey, sizeof (sHistoryData.file_primary_key));
    memcpy ((LPTSTR) sHistoryData.primary_key.unique_id,     (LPCTSTR)dest_str, sizeof (sHistoryData.primary_key.unique_id));        
    memcpy ((LPTSTR) sHistoryData.time_modified,     (LPCTSTR)strTime, sizeof (sHistoryData.time_modified)); 
             
    memcpy ((LPTSTR)sHistoryData.current_data, (LPCTSTR)m_strCurrentData, sizeof (sHistoryData.current_data));
	memcpy ((LPTSTR)sHistoryData.previous_data, (LPCTSTR)m_strPreviousData, sizeof (sHistoryData.previous_data));
	memcpy ((LPTSTR)sHistoryData.field_tag, (LPCTSTR)m_strFieldTag, sizeof (sHistoryData.field_tag));
	memcpy ((LPTSTR)sHistoryData.changed_by,(LPTSTR)ucf01.primary_key.name, sizeof(sHistoryData.changed_by));
    CHAR wstation[21]; DWORD wsize;

    wsize = sizeof( wstation );
    GetComputerName( wstation, &wsize );

	memcpy ((LPTSTR)sHistoryData.reserved,(LPTSTR)wstation, sizeof(sHistoryData.reserved));

    int rcode = txdsapi_insert_record  ( (pBYTE)&sHistoryData, sizeof (HST01), HST01_DATA,strErrorMsg );
	if ( rcode != PTEMSG_OK )
	{
		m_StrMessageLbl = strErrorMsg;
		UpdateData( FALSE );
	}
    
}


void CBinDefinition::OnCancel() 
{
	if ( pGeneralPage->m_bGeneral )
	{
		pGeneralPage->CleanBinDialog();
		pGeneralPage->EnableBinDialog(FALSE);		
	}

	if ( pBlockingPage->m_bBlocking )
	{
		pBlockingPage->CleanBlockingDialog();
	}	
   
	if ( pMccBlockingPage->m_bMCCBlocking )
	{
		pMccBlockingPage->CleanBlockingDialog();
	}
	if ( pMidBlockingPage->m_bBlocking )
	{
		pMidBlockingPage->CleanBlockingDialog();
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

BOOL CBinDefinition::OnHelpInfo(HELPINFO* pHelpInfo) 
{
	
/*	HtmlHelp (AfxGetMainWnd()->GetSafeHwnd(),
	"bin.chm>bin",HH_HELP_CONTEXT,IDD_BIN_DEFINITION);
*/
	return TRUE;
}

BOOL CBinDefinition::OnSetActive() 
{
	((CBinPropertySheet*) this->GetParent())->m_nActiveTab = 0;
	
	return CPropertyPage::OnSetActive();
}

void CBinDefinition::OnKillfocusBinLengthFld() 
{
	UpdateData(TRUE);

	int len = m_StrBinLengthFld.GetLength();

	if ( len == 1 )
		m_StrBinLengthFld = "0" + m_StrBinLengthFld;

	UpdateData(FALSE);	
}


BOOL CBinDefinition::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	// TODO: Add your specialized code here and/or call the base class
	c_time = CTime::GetCurrentTime();	
	shrmem.SetLastUpdateTime(c_time);
	return CPropertyPage::OnCommand(wParam, lParam);
}
