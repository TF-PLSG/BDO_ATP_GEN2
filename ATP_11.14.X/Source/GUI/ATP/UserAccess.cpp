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
* $Log:   N:\PVCS\PTE\Equitable\ATP_gui\UserAccess.cpp  $

		Rev 1.10  Jan 01 2011  11:30:20   TF-Srikanth
		Resolved bug for Last login username on user access dialog   
		
		  Rev 1.9   Dec 01 2003 11:30:20   lmitchel
		  Added AMEX Online Updates - scr1200
		  
			Rev 1.8   Jul 10 2002 09:56:54   lmitchel
			Ver:4.0.3.1:  Added SendMessage command in PropertySheet files (on_init function) to set the Apply button as the default.  If the enter key is pressed, action will default to  the apply button instead of the OK button.  This allows the user to remain in the current dialog instead of exiting to the main menu.
			
			  Rev 1.7   Jan 05 2000 14:54:30   gbarnes
			  Major update. Not updated to PVCS recently.
			  PVCS was locked.
			  
				Rev 1.0   Jun 28 1999 13:59:36   iarustam
				Initial Release
				
				  Rev 1.3   Jun 11 1999 14:08:20   iarustam
				  Bug fixes
				  
					
					  Rev 1.2   Apr 29 1999 14:57:12   iarustam
					  Bug fixes
					  
						Rev 1.1   Mar 16 1999 16:30:36   iarustam
						Bug fixes
						
						  Rev 1.0   Feb 23 1999 11:41:24   iarustam
						  Initial Release
						  
							Rev 1.0   Feb 17 1999 10:10:12   iarustam
							Initial Release
							
							  Rev 1.0   Feb 12 1999 11:06:22   iarustam
							  initial release
							  
								
								  *
************************************************************************************/

// UserAccess.cpp : implementation file
//

#include "stdafx.h"
#include "Atp.h"
#include "UserAccess.h"
//#include "PTEDataAccess.h"  
#include "MyUtils.h"
#include "SharedMem.h"
#include "NameList.h"

extern "C" {
#include "PTETIME.h"
}

//#include "htmlhelp.h"
#include "UserPropertySheet.h"
#include "changepassword.h"


//CString UserNameFld;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern UCF01 ucf01;
//UCF03 sUserRecord;
UCF01 sUserRecord;
extern CSharedMem shrmem;


/////////////////////////////////////////////////////////////////////////////
// CUserAccess property page

IMPLEMENT_DYNCREATE(CUserAccess, CPropertyPage)

CUserAccess::CUserAccess() : CPropertyPage(CUserAccess::IDD)
{
	//{{AFX_DATA_INIT(CUserAccess)
	m_StrUserNameFld = _T("");
	m_StrGroupIDFld = _T("");
	m_StrLabel1 = _T("");
	m_StrLabel2 = _T("");
	m_StrLabel3 = _T("");
	m_StrLabel4 = _T("");
	m_StrLabel5 = _T("");
	m_StrLabel6 = _T("");
	m_StrLabel7 = _T("");
	m_StrLabel8 = _T("");
	m_StrLabel9 = _T("");
	m_StrLabel10 = _T("");
	m_StrMessageLbl = _T("");
	m_StrLabel11 = _T("");
	m_StrLabel12 = _T("");
	m_StrLabel13 = _T("");	
	m_StrLabel14 = _T("");
	m_StrLabel15 = _T("");
	m_StrUserNameFld2 = _T("");
	m_StrLabel16 = _T("");
	m_StrLabel18 = _T("");
	m_StrLabel17 = _T("");
	m_StrLabel20 = _T("");
	m_StrLabel22 = _T("");
	m_StrLabel23 = _T("");
	//}}AFX_DATA_INIT
	m_bGroupIDChanged = false;
}

CUserAccess::~CUserAccess()
{
}

void CUserAccess::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CUserAccess)
	DDX_Control(pDX, IDC_VIEW_CHECK20, m_CntlViewCheck20);
	DDX_Control(pDX, IDC_ADD_CHECK18, m_CntlAddCheck18);
	DDX_Control(pDX, IDC_DELETE_CHECK18, m_CntlDeleteCheck18);
	DDX_Control(pDX, IDC_LABEL18, m_CntlLabel18);
	DDX_Control(pDX, IDC_UPDATE_CHECK18, m_CntlUpdateCheck18);
	DDX_Control(pDX, IDC_VIEW_CHECK18, m_CntlViewCheck18);
	DDX_Control(pDX, IDC_LABEL16, m_CntlLabel16);
	DDX_Control(pDX, IDC_ADD_CHECK17, m_CntlOverrideCheck);
	DDX_Control(pDX, IDC_CHANGE_PASSWORD_BUTTON, m_CntlChangePasswordButton);
	DDX_Control(pDX, IDC_GROUP_LIST_BUTTON, m_CntlGroupListButton);
	DDX_Control(pDX, IDC_USER_NAME_FLD2, m_CntlUserNameFld2);
	DDX_Control(pDX, IDC_GROUP_ID_FLD, m_CntlGroupIDFld);
	DDX_Control(pDX, IDC_VIEW_CHECK15, m_CntlViewCheck15);
	DDX_Control(pDX, IDC_LABEL15, m_CntlLabel15);
	DDX_Control(pDX, IDC_VIEW_CHECK14, m_CntlViewCheck14);
	DDX_Control(pDX, IDC_UPDATE_CHECK14, m_CntlUpdateCheck14);
	DDX_Control(pDX, IDC_DELETE_CHECK14, m_CntlDeleteCheck14);
	DDX_Control(pDX, IDC_ADD_CHECK14, m_CntlAddCheck14);
	DDX_Control(pDX, IDC_LABEL14, m_CntlLabel14);
	DDX_Control(pDX, IDC_CHECK_ALL, m_CntlAll);
	DDX_Control(pDX, IDC_ADD_CHECK13, m_CntlAddCheck13);	//For Dual Encryption, there is only Add check box
	DDX_Control(pDX, IDC_VIEW_CHECK12, m_CntlViewCheck12);
	DDX_Control(pDX, IDC_UPDATE_CHECK12, m_CntlUpdateCheck12);
	DDX_Control(pDX, IDC_DELETE_CHECK12, m_CntlDeleteCheck12);
	DDX_Control(pDX, IDC_ADD_CHECK12, m_CntlAddCheck12);
	DDX_Control(pDX, IDC_ADD_CHECK11, m_CntlAddCheck11);
	DDX_Control(pDX, IDC_VIEW_CHECK11, m_CntlViewCheck11);
	DDX_Control(pDX, IDC_UPDATE_CHECK11, m_CntlUpdateCheck11);
	DDX_Control(pDX, IDC_DELETE_CHECK11, m_CntlDeleteCheck11);
	DDX_Control(pDX, IDC_VIEW_CHECK9, m_CntlViewCheck9);
	DDX_Control(pDX, IDC_VIEW_CHECK8, m_CntlViewCheck8);
	DDX_Control(pDX, IDC_VIEW_CHECK7, m_CntlViewCheck7);
	DDX_Control(pDX, IDC_VIEW_CHECK6, m_CntlViewCheck6);
	DDX_Control(pDX, IDC_VIEW_CHECK5, m_CntlViewCheck5);
	DDX_Control(pDX, IDC_VIEW_CHECK4, m_CntlViewCheck4);
	DDX_Control(pDX, IDC_VIEW_CHECK3, m_CntlViewCheck3);
	DDX_Control(pDX, IDC_VIEW_CHECK2, m_CntlViewCheck2);
	DDX_Control(pDX, IDC_VIEW_CHECK10, m_CntlViewCheck10);
	DDX_Control(pDX, IDC_VIEW_CHECK1, m_CntlViewCheck1);
	DDX_Control(pDX, IDC_UPDATE_CHECK9, m_CntlUpdateCheck9);
	DDX_Control(pDX, IDC_UPDATE_CHECK8, m_CntlUpdateCheck8);
	DDX_Control(pDX, IDC_UPDATE_CHECK7, m_CntlUpdateCheck7);
	DDX_Control(pDX, IDC_UPDATE_CHECK6, m_CntlUpdateCheck6);
	DDX_Control(pDX, IDC_UPDATE_CHECK5, m_CntlUpdateCheck5);
	DDX_Control(pDX, IDC_UPDATE_CHECK4, m_CntlUpdateCheck4);
	DDX_Control(pDX, IDC_UPDATE_CHECK3, m_CntlUpdateCheck3);
	DDX_Control(pDX, IDC_UPDATE_CHECK2, m_CntlUpdateCheck2);
	DDX_Control(pDX, IDC_UPDATE_CHECK10, m_CntlUpdateCheck10);
	DDX_Control(pDX, IDC_UPDATE_CHECK1, m_CntlUpdateCheck1);
	DDX_Control(pDX, IDC_TLF_EXTRACT, m_CntlTlfExtract);
	DDX_Control(pDX, IDC_LABEL9, m_CntlLabel9);
	DDX_Control(pDX, IDC_LABEL8, m_CntlLabel8);
	DDX_Control(pDX, IDC_LABEL7, m_CntlLabel7);
	DDX_Control(pDX, IDC_LABEL6, m_CntlLabel6);
	DDX_Control(pDX, IDC_LABEL5, m_CntlLabel5);
	DDX_Control(pDX, IDC_LABEL4, m_CntlLabel4);
	DDX_Control(pDX, IDC_LABEL3, m_CntlLabel3);
	DDX_Control(pDX, IDC_LABEL2, m_CntlLabel2);
	DDX_Control(pDX, IDC_LABEL10, m_CntlLabel10);
	DDX_Control(pDX, IDC_LABEL1, m_CntlLabel1);
	DDX_Control(pDX, IDC_DELETE_CHECK9, m_CntlDeleteCheck9);
	DDX_Control(pDX, IDC_DELETE_CHECK8, m_CntlDeleteCheck8);
	DDX_Control(pDX, IDC_DELETE_CHECK7, m_CntlDeleteCheck7);
	DDX_Control(pDX, IDC_DELETE_CHECK6, m_CntlDeleteCheck6);
	DDX_Control(pDX, IDC_DELETE_CHECK5, m_CntlDeleteCheck5);
	DDX_Control(pDX, IDC_DELETE_CHECK4, m_CntlDeleteCheck4);
	DDX_Control(pDX, IDC_DELETE_CHECK3, m_CntlDeleteCheck3);
	DDX_Control(pDX, IDC_DELETE_CHECK2, m_CntlDeleteCheck2);
	DDX_Control(pDX, IDC_DELETE_CHECK10, m_CntlDeleteCheck10);
	DDX_Control(pDX, IDC_DELETE_CHECK1, m_CntlDeleteCheck1);
	DDX_Control(pDX, IDC_ADD_CHECK9, m_CntlAddCheck9);
	DDX_Control(pDX, IDC_ADD_CHECK8, m_CntlAddCheck8);
	DDX_Control(pDX, IDC_ADD_CHECK7, m_CntlAddCheck7);
	DDX_Control(pDX, IDC_ADD_CHECK6, m_CntlAddCheck6);
	DDX_Control(pDX, IDC_ADD_CHECK5, m_CntlAddCheck5);
	DDX_Control(pDX, IDC_ADD_CHECK4, m_CntlAddCheck4);
	DDX_Control(pDX, IDC_ADD_CHECK3, m_CntlAddCheck3);
	DDX_Control(pDX, IDC_ADD_CHECK2, m_CntlAddCheck2);
	DDX_Control(pDX, IDC_ADD_CHECK10, m_CntlAddCheck10);
	DDX_Control(pDX, IDC_ADD_CHECK1, m_CntlAddCheck1);
	DDX_Text(pDX, IDC_USER_NAME_FLD, m_StrUserNameFld);
	DDV_MaxChars(pDX, m_StrUserNameFld, 20);
	DDX_Text(pDX, IDC_GROUP_ID_FLD, m_StrGroupIDFld);
	DDX_Text(pDX, IDC_LABEL1, m_StrLabel1);
	DDX_Text(pDX, IDC_LABEL2, m_StrLabel2);
	DDX_Text(pDX, IDC_LABEL3, m_StrLabel3);
	DDX_Text(pDX, IDC_LABEL4, m_StrLabel4);
	DDX_Text(pDX, IDC_LABEL5, m_StrLabel5);
	DDX_Text(pDX, IDC_LABEL6, m_StrLabel6);
	DDX_Text(pDX, IDC_LABEL7, m_StrLabel7);
	DDX_Text(pDX, IDC_LABEL8, m_StrLabel8);
	DDX_Text(pDX, IDC_LABEL9, m_StrLabel9);
	DDX_Text(pDX, IDC_LABEL10, m_StrLabel10);
	DDX_Text(pDX, IDC_MESSAGE_LBL, m_StrMessageLbl);
	DDX_Text(pDX, IDC_LABEL11, m_StrLabel11);
	DDX_Text(pDX, IDC_LABEL12, m_StrLabel12);
	DDX_Text(pDX, IDC_LABEL13, m_StrLabel13);
	DDX_Text(pDX, IDC_LABEL14, m_StrLabel14);
	DDX_Text(pDX, IDC_LABEL15, m_StrLabel15);
	DDX_Text(pDX, IDC_USER_NAME_FLD2, m_StrUserNameFld2);
	DDV_MaxChars(pDX, m_StrUserNameFld2, 20);
	DDX_Text(pDX, IDC_LABEL16, m_StrLabel16);
	DDX_Text(pDX, IDC_LABEL18, m_StrLabel18);
	DDX_Text(pDX, IDC_LABEL17, m_StrLabel17);
	DDX_Text(pDX, IDC_FRAUD_USER_LABEL21, m_StrLabel22);
	DDX_Control(pDX, ID_GLOBAL_UPDATE_CHECK, m_CntlUpdateCheck17);
	DDX_Control(pDX, ID_GLOBAL_VIEW_CHECK, m_CntlViewCheck17);
	DDX_Text(pDX, IDC_LABEL_USR_STS_20, m_StrLabel20);
	DDX_Control(pDX, IDC_USER_STATUS_VIEW_CHECK20, m_CntlViewCheck21);
	DDX_Control(pDX, IDC_FRAUD_USER_VIEW_CHECK21, m_CntlViewCheck22);
	DDX_Control(pDX, IDC_FRAUD_USER_UPDATE_CHECK21, m_CntlUpdateCheck22);
	DDX_Control(pDX, IDC_FRAUD_USER_DELETE_CHECK21, m_CntlDeleteCheck22);
	DDX_Control(pDX, IDC_FRAUD_USER_ADD_CHECK21, m_CntlAddCheck22);
	DDX_Text(pDX, IDC_CF_USER_LABEL23, m_StrLabel23);
	DDX_Control(pDX, IDC_CF_USER_LABEL23, m_CntlLabel23);
	DDX_Control(pDX, IDC_CF_USER_VIEW_CHECK23, m_CntlViewCheck23);
	DDX_Control(pDX, IDC_CF_USER_UPDATE_CHECK23, m_CntlUpdateCheck23);
	DDX_Control(pDX, IDC_CF_USER_DELETE_CHECK23, m_CntlDeleteCheck23);
	DDX_Control(pDX, IDC_CF_USER_ADD_CHECK23, m_CntlAddCheck23);

	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CUserAccess, CPropertyPage)
//{{AFX_MSG_MAP(CUserAccess)
ON_WM_HELPINFO()
ON_BN_CLICKED(IDC_CHECK_ALL, OnCheckAll)
ON_BN_CLICKED(IDC_ADD_CHECK13, OnCheckEncryptionControl)
	ON_BN_CLICKED(IDC_GROUP_LIST_BUTTON, OnGroupListButton)
	ON_BN_CLICKED(IDC_CHANGE_PASSWORD_BUTTON, OnChangePasswordButton)
	ON_EN_CHANGE(IDC_USER_NAME_FLD2, OnChangeUserNameFld2)
	ON_EN_CHANGE(IDC_GROUP_ID_FLD, OnChangeGroupIdFld)
	ON_BN_CLICKED(IDC_LABEL13, OnLabel13)
	ON_BN_CLICKED(IDC_LABEL16, OnLabel16)
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_ADD_CHECK18, &CUserAccess::OnBnClickedAddCheck18)
	ON_BN_CLICKED(IDC_VIEW_CHECK12, &CUserAccess::OnBnClickedViewCheck12)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CUserAccess message handlers


BOOL CUserAccess::OnInitDialog() 
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
	
	ATP01 sATPStructure;
	char strErrorMessage[200];
	
	int ekeyusercount=0;
	
	m_StrUserNameFld="";//Nullifying user name of last login
	m_StrUserNameFld2 ="";//Nullifying user name of last login
	ZeroMemory(&sATPStructure,sizeof(ATP01)) ;
	//  memcpy ( sATPStructure.site_name, "ATP SigCapture Concord EFS", sizeof (sATPStructure.site_name));
	
	int rcode = txdsapi_get_record  ((pBYTE)&sATPStructure,sizeof (ATP01), ATP01_DATA, strErrorMessage);
	if ( rcode != PTEMSG_OK )
	{   
		m_StrMessageLbl = strErrorMessage;
		m_nDialogMode = MODE_DONE; 
		
		if (!UpdateData(FALSE)){
			AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
		}
		return FALSE;
    }
	
	/*   m_StrLabel1 = "System Administration";	
	m_StrLabel2 = "System Configuration";
    m_StrLabel3 = "System Operations";
    m_StrLabel4 = "BIN Control";
    m_StrLabel5 = "Institution Control";
    m_StrLabel6 = "Authorization Control";
    m_StrLabel7 = "Merchant Control";
    m_StrLabel8 = "Network Control";
    m_StrLabel9 = "Card Account Control";
    m_StrLabel10 = "User Control";*/
	
	m_StrLabel1 = sATPStructure.system_components[0].subsystem_name;	//System Administration
	m_StrLabel2 = sATPStructure.system_components[1].subsystem_name;	//System Configuration 
	m_StrLabel3 = sATPStructure.system_components[2].subsystem_name;	//System Operations    
	m_StrLabel4 = sATPStructure.system_components[3].subsystem_name;	//BIN Control          
	m_StrLabel5 = sATPStructure.system_components[4].subsystem_name;	//Authorization Control
	m_StrLabel6 = sATPStructure.system_components[5].subsystem_name;	//Merchant Control     
	m_StrLabel7 = sATPStructure.system_components[6].subsystem_name;	//Device Definition    
	m_StrLabel8 = sATPStructure.system_components[7].subsystem_name;	//Network Control      
	m_StrLabel9 = sATPStructure.system_components[8].subsystem_name;	//Card Account Control 
	m_StrLabel10 = sATPStructure.system_components[9].subsystem_name;	//Voice Authorization  
	m_StrLabel11 = sATPStructure.system_components[10].subsystem_name;	//Transaction Lookup   
	//m_StrLabel12 = sATPStructure.system_components[11].subsystem_name;	//Recurring Payment    
	m_StrLabel13 = sATPStructure.system_components[12].subsystem_name;	//Dual Encryption      
	m_StrLabel15 = sATPStructure.system_components[13].subsystem_name;	//Service Manager      
	m_StrLabel14 = sATPStructure.system_components[14].subsystem_name;	//Emtpy
	m_StrLabel16 = "Override Denial";
	m_StrLabel18 = sATPStructure.system_components[15].subsystem_name;	//ATP Archiving
	m_StrLabel17 =sATPStructure.system_components[16].subsystem_name;
	m_StrLabel20 =sATPStructure.system_components[17].subsystem_name;
	m_StrLabel22 =sATPStructure.system_components[18].subsystem_name;
	m_StrLabel23 = sATPStructure.system_components[19].subsystem_name;
	if ( m_StrLabel1.IsEmpty() )
	{
		m_CntlAddCheck1.ShowWindow(FALSE);
		m_CntlDeleteCheck1.ShowWindow(FALSE);
		m_CntlViewCheck1.ShowWindow(FALSE);
		m_CntlUpdateCheck1.ShowWindow(FALSE);
		m_CntlAddCheck1.SetCheck(0);
		m_CntlDeleteCheck1.SetCheck(0);
		m_CntlViewCheck1.SetCheck(0);
		m_CntlUpdateCheck1.SetCheck(0);
	}
	
	if ( m_StrLabel2.IsEmpty() )
	{
		m_CntlAddCheck2.ShowWindow(FALSE);
		m_CntlDeleteCheck2.ShowWindow(FALSE);
		m_CntlViewCheck2.ShowWindow(FALSE);
		m_CntlUpdateCheck2.ShowWindow(FALSE);
		m_CntlAddCheck2.SetCheck(0);
		m_CntlDeleteCheck2.SetCheck(0);
		m_CntlViewCheck2.SetCheck(0);
		m_CntlUpdateCheck2.SetCheck(0);
	}
	if ( m_StrLabel3.IsEmpty() )
	{
		m_CntlAddCheck3.ShowWindow(FALSE);
		m_CntlDeleteCheck3.ShowWindow(FALSE);
		m_CntlViewCheck3.ShowWindow(FALSE);
		m_CntlUpdateCheck3.ShowWindow(FALSE);
		m_CntlAddCheck3.SetCheck(0);
		m_CntlDeleteCheck3.SetCheck(0);
		m_CntlViewCheck3.SetCheck(0);
		m_CntlUpdateCheck3.SetCheck(0);
	}
	if ( m_StrLabel4.IsEmpty() )
	{
		m_CntlAddCheck4.ShowWindow(FALSE);
		m_CntlDeleteCheck4.ShowWindow(FALSE);
		m_CntlViewCheck4.ShowWindow(FALSE);
		m_CntlUpdateCheck4.ShowWindow(FALSE);
		m_CntlAddCheck4.SetCheck(0);
		m_CntlDeleteCheck4.SetCheck(0);
		m_CntlViewCheck4.SetCheck(0);
		m_CntlUpdateCheck4.SetCheck(0);
	}
	if ( m_StrLabel5.IsEmpty() )
	{
		m_CntlAddCheck5.ShowWindow(FALSE);
		m_CntlDeleteCheck5.ShowWindow(FALSE);
		m_CntlViewCheck5.ShowWindow(FALSE);
		m_CntlUpdateCheck5.ShowWindow(FALSE);
		m_CntlAddCheck5.SetCheck(0);
		m_CntlDeleteCheck5.SetCheck(0);
		m_CntlViewCheck5.SetCheck(0);
		m_CntlUpdateCheck5.SetCheck(0);
	}
	if ( m_StrLabel6.IsEmpty() )
	{
		m_CntlAddCheck6.ShowWindow(FALSE);
		m_CntlDeleteCheck6.ShowWindow(FALSE);
		m_CntlViewCheck6.ShowWindow(FALSE);
		m_CntlUpdateCheck6.ShowWindow(FALSE);
		m_CntlAddCheck6.SetCheck(0);
		m_CntlDeleteCheck6.SetCheck(0);
		m_CntlViewCheck6.SetCheck(0);
		m_CntlUpdateCheck6.SetCheck(0);
	}
	if ( m_StrLabel7.IsEmpty() )
	{
		m_CntlAddCheck7.ShowWindow(FALSE);
		m_CntlDeleteCheck7.ShowWindow(FALSE);
		m_CntlViewCheck7.ShowWindow(FALSE);
		m_CntlUpdateCheck7.ShowWindow(FALSE);
		m_CntlAddCheck7.SetCheck(0);
		m_CntlDeleteCheck7.SetCheck(0);
		m_CntlViewCheck7.SetCheck(0);
		m_CntlUpdateCheck7.SetCheck(0);
	}
	if ( m_StrLabel8.IsEmpty() )
	{
		m_CntlAddCheck8.ShowWindow(FALSE);
		m_CntlDeleteCheck8.ShowWindow(FALSE);
		m_CntlViewCheck8.ShowWindow(FALSE);
		m_CntlUpdateCheck8.ShowWindow(FALSE);
		m_CntlAddCheck8.SetCheck(0);
		m_CntlDeleteCheck8.SetCheck(0);
		m_CntlViewCheck8.SetCheck(0);
		m_CntlUpdateCheck8.SetCheck(0);
	}
	if ( m_StrLabel9.IsEmpty() )
	{
		m_CntlAddCheck9.ShowWindow(FALSE);
		m_CntlDeleteCheck9.ShowWindow(FALSE);
		m_CntlViewCheck9.ShowWindow(FALSE);
		m_CntlUpdateCheck9.ShowWindow(FALSE);
		m_CntlAddCheck9.SetCheck(0);
		m_CntlDeleteCheck9.SetCheck(0);
		m_CntlViewCheck9.SetCheck(0);
		m_CntlUpdateCheck9.SetCheck(0);
	}
	if ( m_StrLabel10.IsEmpty() )
	{
		m_CntlAddCheck10.ShowWindow(FALSE);
		m_CntlDeleteCheck10.ShowWindow(FALSE);
		m_CntlViewCheck10.ShowWindow(FALSE);
		m_CntlUpdateCheck10.ShowWindow(FALSE);
		m_CntlAddCheck10.SetCheck(0);
		m_CntlDeleteCheck10.SetCheck(0);
		m_CntlViewCheck10.SetCheck(0);
		m_CntlUpdateCheck10.SetCheck(0);
	}
	if ( m_StrLabel11.IsEmpty() )
	{
		m_CntlAddCheck11.ShowWindow(FALSE);
		m_CntlDeleteCheck11.ShowWindow(FALSE);
		m_CntlViewCheck11.ShowWindow(FALSE);
		m_CntlUpdateCheck11.ShowWindow(FALSE);
		m_CntlTlfExtract.ShowWindow(FALSE);
		m_CntlAddCheck11.SetCheck(0);
		m_CntlDeleteCheck11.SetCheck(0);
		m_CntlViewCheck11.SetCheck(0);
		m_CntlUpdateCheck11.SetCheck(0);
		m_CntlTlfExtract.SetCheck(0);
	}
	if ( m_StrLabel12.IsEmpty() )
	{
		m_CntlAddCheck12.ShowWindow(FALSE);
		m_CntlDeleteCheck12.ShowWindow(FALSE);
		m_CntlViewCheck12.ShowWindow(FALSE);
		m_CntlUpdateCheck12.ShowWindow(FALSE);
		m_CntlAddCheck12.SetCheck(0);
		m_CntlDeleteCheck12.SetCheck(0);
		m_CntlViewCheck12.SetCheck(0);
		m_CntlUpdateCheck12.SetCheck(0);
	}
	//Krishna@TF, for Dual Encryption
	if ( m_StrLabel13.IsEmpty() )
	{
		m_CntlAddCheck13.ShowWindow(FALSE);     
		m_CntlAddCheck13.SetCheck(0);     
	}
	
	if ( m_StrLabel14.IsEmpty() )
	{
		m_CntlAddCheck14.ShowWindow(FALSE);
		m_CntlDeleteCheck14.ShowWindow(FALSE);
		m_CntlViewCheck14.ShowWindow(FALSE);
		m_CntlUpdateCheck14.ShowWindow(FALSE);
		m_CntlAddCheck14.SetCheck(0);
		m_CntlDeleteCheck14.SetCheck(0);
		m_CntlViewCheck14.SetCheck(0);
		m_CntlUpdateCheck14.SetCheck(0);
	}
	
	if ( m_StrLabel15.IsEmpty() )
	{
		m_CntlViewCheck15.ShowWindow(FALSE);
		m_CntlViewCheck15.SetCheck(0);
	}

	if(m_StrLabel16.IsEmpty())
	{
		m_CntlOverrideCheck.ShowWindow(FALSE);
		m_CntlOverrideCheck.SetCheck(0);
	}

	if ( m_StrLabel18.IsEmpty() )
	{
		m_CntlViewCheck20.ShowWindow(FALSE);
		m_CntlDeleteCheck18.ShowWindow(FALSE);
		m_CntlViewCheck18.ShowWindow(FALSE);
		m_CntlUpdateCheck18.ShowWindow(FALSE);
		m_CntlViewCheck20.SetCheck(0);
		m_CntlDeleteCheck18.SetCheck(0);
		m_CntlViewCheck18.SetCheck(0);
		m_CntlUpdateCheck18.SetCheck(0);
				
	}
	if ( m_StrLabel17.IsEmpty() )
	{
		m_CntlUpdateCheck17.ShowWindow(FALSE);
		m_CntlViewCheck17.ShowWindow(FALSE);
		m_CntlUpdateCheck17.SetCheck(0);
		m_CntlViewCheck17.SetCheck(0);
	}
	if ( m_StrLabel20.IsEmpty() )
	{
		m_CntlViewCheck21.ShowWindow(FALSE);
		m_CntlViewCheck21.SetCheck(0);
	}
   if(m_StrLabel22.IsEmpty())
   {
		m_CntlViewCheck22.ShowWindow(FALSE);
		m_CntlUpdateCheck22.ShowWindow(FALSE);
		m_CntlAddCheck22.ShowWindow(FALSE);
		m_CntlDeleteCheck22.ShowWindow(FALSE);
   }

    if(m_StrLabel23.IsEmpty())
   {
     		m_CntlAddCheck23.ShowWindow(FALSE);
     		m_CntlDeleteCheck23.ShowWindow(FALSE);
     		m_CntlViewCheck23.ShowWindow(FALSE);
     		m_CntlUpdateCheck23.ShowWindow(FALSE);
     		m_CntlAddCheck23.SetCheck(0);
     		m_CntlDeleteCheck23.SetCheck(0);
     		m_CntlViewCheck23.SetCheck(0);
     		m_CntlUpdateCheck23.SetCheck(0);
   }
   	/*memcpy ((LPSTR)(sUserRecord.primary_key.name), m_StrUserNameFld, sizeof (sUserRecord.primary_key.name));
	
	  rcode = txdsapi_Select_Ucf01_DualEncryptionKey_UserCount(&sUserRecord, strErrorMessage);
	  
		if(rcode != PTEMSG_OK)
		{
		AfxMessageBox(strErrorMessage);
		return FALSE;
		}
		
		  ekeyusercount = atoi((char *)sUserRecord.dekey);
		  if(ekeyusercount == 2)
		  {
		  m_CntlAddCheck13.SetCheck(0);
		  m_CntlAddCheck13.ShowWindow(FALSE);
		  }
	*/
	
	
	SetModified ( FALSE );
	
	if ( m_bPopulate )
		PopulateDialogFromDatabase();
	
	m_bAccess = TRUE;	
	
	//Check for dual encryption
	/* This check has been performed in PopulateDialogFromDatabase() func called above. So commenting out this line
	if(ucf01.control[11].action[0] != '1')
		CheckCountofUsersAssignedforDualEncryption(m_StrUserNameFld, 0);
	*/
	if (!UpdateData(FALSE)){
		AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
	}
	
	
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CUserAccess::PopulateControlStructure ()
{
    ZeroMemory ( m_sControlNew, sizeof (m_sControlNew));
	if (m_bAccess)
		{
		//System Administration
		if(m_CntlAddCheck1.GetCheck() == 1)
			m_sControlNew[0].action[0] = '1';
		else 
			m_sControlNew[0].action[0] = '0';
		if(m_CntlDeleteCheck1.GetCheck() == 1)
			m_sControlNew[0].action[1] = '1';
		else 
			m_sControlNew[0].action[1] = '0';
		if(m_CntlUpdateCheck1.GetCheck() == 1)
			m_sControlNew[0].action[2] = '1';
		else 
			m_sControlNew[0].action[2] = '0';
		if(m_CntlViewCheck1.GetCheck() == 1)
			m_sControlNew[0].action[3] = '1';
		else 
			m_sControlNew[0].action[3] = '0';
		memcpy ((LPTSTR)m_sControlNew[0].subsystem_name, m_StrLabel1, sizeof (m_sControlNew[0].subsystem_name));
		
		//System Configuration
		if(m_CntlAddCheck2.GetCheck() == 1)
			m_sControlNew[1].action[0] = '1';
		else 
			m_sControlNew[1].action[0] = '0';
		if(m_CntlDeleteCheck2.GetCheck() == 1)
			m_sControlNew[1].action[1] = '1';
		else 
			m_sControlNew[1].action[1] = '0';
		if(m_CntlUpdateCheck2.GetCheck() == 1)
			m_sControlNew[1].action[2] = '1';
		else 
			m_sControlNew[1].action[2] = '0';
		if(m_CntlViewCheck2.GetCheck() == 1)
			m_sControlNew[1].action[3] = '1';
		else 
			m_sControlNew[1].action[3] = '0';
		memcpy ((LPTSTR)m_sControlNew[1].subsystem_name, m_StrLabel2, sizeof (m_sControlNew[1].subsystem_name));
		
		//System Operations    
		if(m_CntlAddCheck3.GetCheck() == 1)
			m_sControlNew[2].action[0] = '1';
		else 
			m_sControlNew[2].action[0] = '0';
		if(m_CntlDeleteCheck3.GetCheck() == 1)
			m_sControlNew[2].action[1] = '1';
		else 
			m_sControlNew[2].action[1] = '0';
		if(m_CntlUpdateCheck3.GetCheck() == 1)
			m_sControlNew[2].action[2] = '1';
		else 
			m_sControlNew[2].action[2] = '0';
		if(m_CntlViewCheck3.GetCheck() == 1)
			m_sControlNew[2].action[3] = '1';
		else 
			m_sControlNew[2].action[3] = '0';
		memcpy ((LPTSTR)m_sControlNew[2].subsystem_name, m_StrLabel3, sizeof (m_sControlNew[2].subsystem_name));
		
		//BIN Control
		if(m_CntlAddCheck4.GetCheck() == 1)
			m_sControlNew[3].action[0] = '1';
		else 
			m_sControlNew[3].action[0] = '0';
		if(m_CntlDeleteCheck4.GetCheck() == 1)
			m_sControlNew[3].action[1] = '1';
		else 
			m_sControlNew[3].action[1] = '0';
		if(m_CntlUpdateCheck4.GetCheck() == 1)
			m_sControlNew[3].action[2] = '1';
		else 
			m_sControlNew[3].action[2] = '0';
		if(m_CntlViewCheck4.GetCheck() == 1)
			m_sControlNew[3].action[3] = '1';
		else 
			m_sControlNew[3].action[3] = '0';
		memcpy ((LPTSTR)m_sControlNew[3].subsystem_name, m_StrLabel4, sizeof (m_sControlNew[3].subsystem_name));
		
		//Authorization Control
		if(m_CntlAddCheck5.GetCheck() == 1)
			m_sControlNew[4].action[0] = '1';
		else 
			m_sControlNew[4].action[0] = '0';

		if(m_CntlDeleteCheck5.GetCheck() == 1)
			m_sControlNew[4].action[1] = '1';
		else 
			m_sControlNew[4].action[1] = '0';

		if(m_CntlUpdateCheck5.GetCheck() == 1)
			m_sControlNew[4].action[2] = '1';
		else 
			m_sControlNew[4].action[2] = '0';

		if(m_CntlViewCheck5.GetCheck() == 1)
			m_sControlNew[4].action[3] = '1';
		else 
			m_sControlNew[4].action[3] = '0';
		memcpy ((LPTSTR)m_sControlNew[4].subsystem_name, m_StrLabel5, sizeof (m_sControlNew[4].subsystem_name));
		
		//Merchant Control
		if(m_CntlAddCheck6.GetCheck() == 1)
			m_sControlNew[5].action[0] = '1';
		else 
			m_sControlNew[5].action[0] = '0';
		if(m_CntlDeleteCheck6.GetCheck() == 1)
			m_sControlNew[5].action[1] = '1';
		else 
			m_sControlNew[5].action[1] = '0';
		if(m_CntlUpdateCheck6.GetCheck() == 1)
			m_sControlNew[5].action[2] = '1';
		else 
			m_sControlNew[5].action[2] = '0';
		if(m_CntlViewCheck6.GetCheck() == 1)    
			m_sControlNew[5].action[3] = '1';
		else 
			m_sControlNew[5].action[3] = '0';
		memcpy ((LPTSTR)m_sControlNew[5].subsystem_name, m_StrLabel6, sizeof (m_sControlNew[5].subsystem_name));
		
		//Device definition
		if(m_CntlAddCheck7.GetCheck() == 1)
			m_sControlNew[6].action[0] = '1';
		else 
			m_sControlNew[6].action[0] = '0';
		if(m_CntlDeleteCheck7.GetCheck() == 1)
			m_sControlNew[6].action[1] = '1';
		else 
			m_sControlNew[6].action[1] = '0';
		if(m_CntlUpdateCheck7.GetCheck() == 1)
			m_sControlNew[6].action[2] = '1';
		else 
			m_sControlNew[6].action[2] = '0';
		if(m_CntlViewCheck7.GetCheck() == 1)
			m_sControlNew[6].action[3] = '1';
		else 
			m_sControlNew[6].action[3] = '0';
		memcpy ((LPTSTR)m_sControlNew[6].subsystem_name, m_StrLabel7, sizeof (m_sControlNew[6].subsystem_name));
		
		//Network Control
		if(m_CntlAddCheck8.GetCheck() == 1)
			m_sControlNew[7].action[0] = '1';
		else
			m_sControlNew[7].action[0] = '0';
		if(m_CntlDeleteCheck8.GetCheck() == 1)
			m_sControlNew[7].action[1] = '1';
		else 
			m_sControlNew[7].action[1] = '0';
		if(m_CntlUpdateCheck8.GetCheck() == 1)
			m_sControlNew[7].action[2] = '1';
		else 
			m_sControlNew[7].action[2] = '0';
		if(m_CntlViewCheck8.GetCheck() == 1)
			m_sControlNew[7].action[3] = '1';
		else 
			m_sControlNew[7].action[3] = '0';
		memcpy ((LPTSTR)m_sControlNew[7].subsystem_name, m_StrLabel8, sizeof (m_sControlNew[7].subsystem_name));
		
		//Card Account Control
		if(m_CntlAddCheck9.GetCheck() == 1)
			m_sControlNew[8].action[0] = '1';
		else 
			m_sControlNew[8].action[0] = '0';
		if(m_CntlDeleteCheck9.GetCheck() == 1)
			m_sControlNew[8].action[1] = '1';
		else 
			m_sControlNew[8].action[1] = '0';
		if(m_CntlUpdateCheck9.GetCheck() == 1)
			m_sControlNew[8].action[2] = '1';
		else 
			m_sControlNew[8].action[2] = '0';
		if(m_CntlViewCheck9.GetCheck() == 1)
			m_sControlNew[8].action[3] = '1';
		else 
			m_sControlNew[8].action[3] = '0';
		memcpy ((LPTSTR)m_sControlNew[8].subsystem_name, m_StrLabel9, sizeof (m_sControlNew[8].subsystem_name));
		
		//Voice Authorization
		if(m_CntlAddCheck10.GetCheck() == 1)
			m_sControlNew[9].action[0] = '1';
		else 
			m_sControlNew[9].action[0] = '0';
		if(m_CntlDeleteCheck10.GetCheck() == 1)
			m_sControlNew[9].action[1] = '1';
		else 
			m_sControlNew[9].action[1] = '0';
		if(m_CntlUpdateCheck10.GetCheck() == 1)
			m_sControlNew[9].action[2] = '1';
		else 
			m_sControlNew[9].action[2] = '0';
		if(m_CntlViewCheck10.GetCheck() == 1)
			m_sControlNew[9].action[3] = '1';
		else 
			m_sControlNew[9].action[3] = '0';
		memcpy ((LPTSTR)m_sControlNew[9].subsystem_name, m_StrLabel10,sizeof (m_sControlNew[9].subsystem_name));
		
		//Transaction Lookup
		if(m_CntlAddCheck11.GetCheck() == 1)
			m_sControlNew[10].action[0] = '1';
		else 
			m_sControlNew[10].action[0] = '0';
		if(m_CntlDeleteCheck11.GetCheck() == 1)
			m_sControlNew[10].action[1] = '1';
		else 
			m_sControlNew[10].action[1] = '0';
		if(m_CntlUpdateCheck11.GetCheck() == 1)
			m_sControlNew[10].action[2] = '1';
		else 
			m_sControlNew[10].action[2] = '0';
		if(m_CntlViewCheck11.GetCheck() == 1)
			m_sControlNew[10].action[3] = '1';
		else 
			m_sControlNew[10].action[3] = '0';
		if(m_CntlTlfExtract.GetCheck() == 1)
            m_sControlNew[10].action[4] = '1';
        else 
            m_sControlNew[10].action[4] = '0';
		memcpy ((LPTSTR)m_sControlNew[10].subsystem_name, m_StrLabel11,sizeof (m_sControlNew[10].subsystem_name));
		
		/**Recurring Payment/
		/*   if(m_CntlAddCheck12.GetCheck() == 1)
        m_sControlNew[11].action[0] = '1';
		else 
        m_sControlNew[11].action[0] = '0';
		if(m_CntlDeleteCheck12.GetCheck() == 1)
        m_sControlNew[11].action[1] = '1';
		else 
        m_sControlNew[11].action[1] = '0';
		if(m_CntlUpdateCheck12.GetCheck() == 1)
        m_sControlNew[11].action[2] = '1';
		else 
        m_sControlNew[11].action[2] = '0';
		if(m_CntlViewCheck12.GetCheck() == 1)
        m_sControlNew[11].action[3] = '1';
		else 
        m_sControlNew[11].action[3] = '0';
		memcpy ((LPTSTR)m_sControlNew[10].subsystem_name, m_StrLabel11,sizeof (m_sControlNew[10].subsystem_name));
		*/
		//Krishna@TF, For Dual Encryption Option
		if(m_CntlAddCheck13.GetCheck() == 1)
		{
			m_sControlNew[11].action[0] = '1';
			m_sControlNew[11].action[1] = '1';
			m_sControlNew[11].action[2] = '1';
			m_sControlNew[11].action[3] = '1';
		}
		else {
			m_sControlNew[11].action[0] = '0';
			m_sControlNew[11].action[1] = '0';
			m_sControlNew[11].action[2] = '0';
			m_sControlNew[11].action[3] = '0';
		}
		memcpy ((LPTSTR)m_sControlNew[11].subsystem_name, m_StrLabel13,sizeof (m_sControlNew[11].subsystem_name));
		
		
		
		//Service Manager
		m_sControlNew[13].action[0] = '0';
		m_sControlNew[13].action[1] = '0';
		m_sControlNew[13].action[2] = '0';
		if(m_CntlViewCheck15.GetCheck() == 1)
			m_sControlNew[13].action[3] = '1';
		else 
			m_sControlNew[13].action[3] = '0';
		memcpy ((LPTSTR)m_sControlNew[13].subsystem_name, m_StrLabel15,sizeof (m_sControlNew[13].subsystem_name));

		//Override Merchant Denial
		if(m_CntlAddCheck14.GetCheck() == 1)
			m_sControlNew[14].action[0] = '1';
		else 
			m_sControlNew[14].action[0] = '0';
		if(m_CntlDeleteCheck14.GetCheck() == 1)
			m_sControlNew[14].action[1] = '1';
		else 
			m_sControlNew[14].action[1] = '0';
		if(m_CntlUpdateCheck14.GetCheck() == 1)
			m_sControlNew[14].action[2] = '1';
		else 
			m_sControlNew[14].action[2] = '0';
		if(m_CntlViewCheck14.GetCheck() == 1)
			m_sControlNew[14].action[3] = '1';
		else 
			m_sControlNew[14].action[3] = '0';
		memcpy ((LPTSTR)m_sControlNew[14].subsystem_name, m_StrLabel14,sizeof (m_sControlNew[14].subsystem_name));

				//ATP Archiving
		if(m_CntlViewCheck20.GetCheck() == 1)
			m_sControlNew[15].action[0] = '1';
		else 
			m_sControlNew[15].action[0] = '0';
		if(m_CntlDeleteCheck18.GetCheck() == 1)
			m_sControlNew[15].action[1] = '1';
		else 
			m_sControlNew[15].action[1] = '0';
		if(m_CntlUpdateCheck18.GetCheck() == 1)
			m_sControlNew[15].action[2] = '1';
		else 
			m_sControlNew[15].action[2] = '0';
		if(m_CntlViewCheck18.GetCheck() == 1)
			m_sControlNew[15].action[3] = '1';
		else 
			m_sControlNew[15].action[3] = '0';
		memcpy ((LPTSTR)m_sControlNew[15].subsystem_name, m_StrLabel18,sizeof (m_sControlNew[15].subsystem_name));

		// System Global parameter
		m_sControlNew[16].action[0] = '0';
		m_sControlNew[16].action[1] = '0';
		if(m_CntlUpdateCheck17.GetCheck() == 1)
			m_sControlNew[16].action[2] = '1';
		else 
			m_sControlNew[16].action[2] = '0';
		if(m_CntlViewCheck17.GetCheck() == 1)
			m_sControlNew[16].action[3] = '1';
		else 
			m_sControlNew[16].action[3] = '0';
		memcpy ((LPTSTR)m_sControlNew[16].subsystem_name, m_StrLabel17,sizeof (m_sControlNew[16].subsystem_name));
		// for logging into HST01 table

		m_sControlNew[17].action[0] = '0';
		m_sControlNew[17].action[1] = '0';
		if(m_CntlViewCheck21.GetCheck() == 1)
			m_sControlNew[17].action[3] = '1';
		else 
			m_sControlNew[17].action[3] = '0';

		m_sControlNew[17].action[2] = '0';
		memcpy ((LPTSTR)m_sControlNew[17].subsystem_name, m_StrLabel20,sizeof (m_sControlNew[17].subsystem_name));

				// Fraud Control
        if(m_CntlAddCheck22.GetCheck() == 1)
            m_sControlNew[18].action[0] = '1';
        else
            m_sControlNew[18].action[0] = '0';
        if(m_CntlDeleteCheck22.GetCheck() == 1)
            m_sControlNew[18].action[1] = '1';
        else 
            m_sControlNew[18].action[1] = '0';
        if(m_CntlUpdateCheck22.GetCheck() == 1)
            m_sControlNew[18].action[2] = '1';
        else 
            m_sControlNew[18].action[2] = '0';
        if(m_CntlViewCheck22.GetCheck() == 1)
            m_sControlNew[18].action[3] = '1';
        else 
            m_sControlNew[18].action[3] = '0';
  		memcpy ((LPTSTR)m_sControlNew[18].subsystem_name, m_StrLabel22,sizeof (m_sControlNew[15].subsystem_name));

		// CF Rules Definition
        if(m_CntlAddCheck23.GetCheck() == 1)
            m_sControlNew[19].action[0] = '1';
        else 
            m_sControlNew[19].action[0] = '0';
        if(m_CntlDeleteCheck23.GetCheck() == 1)
            m_sControlNew[19].action[1] = '1';
        else 
            m_sControlNew[19].action[1] = '0';
        if(m_CntlUpdateCheck23.GetCheck() == 1)
            m_sControlNew[19].action[2] = '1';
        else 
            m_sControlNew[19].action[2] = '0';
        if(m_CntlViewCheck23.GetCheck() == 1)
            m_sControlNew[19].action[3] = '1';
        else 
            m_sControlNew[19].action[3] = '0';
  		memcpy ((LPTSTR)m_sControlNew[19].subsystem_name, m_StrLabel23,sizeof (m_sControlNew[19].subsystem_name));


		New_Gid = m_StrGroupIDFld;
		New_Uname = m_StrUserNameFld2;
		

	}

	
    return TRUE;
}


void CUserAccess::PopulateHistoryTable()
{
    HST01 sHistoryData;
    CHAR dest_str[18]; 
	char strFieldTag [50]={0};
    char temp[5];
	CMyUtils myUtilsObject;
	char strErrorMsg[200] = "";
	CString temp1;
	
    char strDate[9] = "";
    char strTime[7] = "";
	
    memset ( strDate, 0, sizeof (strDate));
    memset ( strTime, 0, sizeof (strTime));
    
    myUtilsObject.GetSystemDate(strDate);  

	myUtilsObject.GetSystemTime(strTime);
  //  pintime_get_gmt ( dest_str );
    ZeroMemory ( &sHistoryData, sizeof (HST01));
    memcpy ((LPTSTR) sHistoryData.file_name,     (LPCTSTR)"UCF01", sizeof ( sHistoryData.file_name));
    temp1 = strDate;
    temp1 = temp1.Right(4) + temp1.Left(2) + temp1.Mid(2,2);
	
	CHAR wstation[21]; DWORD wsize;
	
	wsize = sizeof( wstation );
	GetComputerName( wstation, &wsize );
    
    for (int i = 0; i < MAX_SUBSYSTEMS; i++)
    {
        int nResultCode = strcmp ((char *)m_sControlNew[i].action,(char *)m_sControl[i].action);
		//if (m_sControlNew[i].action[j] != m_sControl[i].action[j] )
        if ( nResultCode != 0 && 
			'\0' != m_sControlNew[i].subsystem_name[0] &&
			'\0' != m_sControl[i].subsystem_name[0])
			
        {
           /* myUtilsObject.GetSystemTime(strTime);
            pintime_get_gmt ( dest_str );
            ZeroMemory ( &sHistoryData, sizeof (HST01));
            memcpy ((LPTSTR) sHistoryData.file_name,     (LPCTSTR)"UCF01", sizeof ( sHistoryData.file_name));
            temp1 = strDate;
            temp1 = temp1.Right(4) + temp1.Left(2) + temp1.Mid(2,2);
			
			CHAR wstation[21]; DWORD wsize;
			
			wsize = sizeof( wstation );
			GetComputerName( wstation, &wsize );*/
			 pintime_get_gmt ( dest_str );
			memcpy ((LPTSTR)sHistoryData.reserved,(LPTSTR)wstation, sizeof(sHistoryData.reserved));
            memcpy ((LPTSTR) sHistoryData.date_modified, temp1, sizeof (sHistoryData.date_modified));
            memcpy ((LPTSTR) sHistoryData.file_primary_key,  (LPCTSTR)m_StrUserNameFld, sizeof ( sHistoryData.file_primary_key));
            memcpy ((LPTSTR) sHistoryData.primary_key.unique_id,     (LPCTSTR)dest_str, sizeof ( sHistoryData.primary_key.unique_id));        
            memcpy ((LPTSTR) sHistoryData.time_modified,     (LPCTSTR)strTime, sizeof ( sHistoryData.time_modified)); 
           /* memcpy (strFieldTag, "Action_", sizeof (strFieldTag));
            itoa ( i+1, temp, 10);
            strcat (strFieldTag, temp);
            memcpy ((LPTSTR) sHistoryData.field_tag,     (LPCTSTR)strFieldTag, sizeof ( sHistoryData.field_tag));*/
			memcpy (strFieldTag, m_sControl[i].subsystem_name, sizeof (strFieldTag));
			if(strlen(strFieldTag)>0)
			{
				strcat (strFieldTag, " Permission");
			}
			memcpy ((LPTSTR) sHistoryData.field_tag,     strFieldTag, sizeof ( sHistoryData.field_tag));
            memcpy ((LPTSTR) sHistoryData.previous_data, (LPCTSTR)m_sControl[i].action, sizeof ( sHistoryData.previous_data));  
            memcpy ((LPTSTR) sHistoryData.current_data, (LPCTSTR)m_sControlNew[i].action, sizeof ( sHistoryData.current_data));                        
			memcpy ((LPTSTR)sHistoryData.changed_by,(LPTSTR)ucf01.primary_key.name, sizeof(sHistoryData.changed_by));
			int rcode = txdsapi_insert_record  ( (pBYTE)&sHistoryData, sizeof (HST01), HST01_DATA, strErrorMsg );
			if ( rcode != PTEMSG_OK )
			{
				m_StrMessageLbl = strErrorMsg;
				UpdateData( FALSE );
			}
            
        }
    }
	if(Old_Gid != New_Gid )  // checking for group id update  
	{
			/*myUtilsObject.GetSystemTime(strTime);
            pintime_get_gmt ( dest_str );
            ZeroMemory ( &sHistoryData, sizeof (HST01));
            memcpy ((LPTSTR) sHistoryData.file_name,     (LPCTSTR)"UCF01", sizeof ( sHistoryData.file_name));
            temp1 = strDate;
            temp1 = temp1.Right(4) + temp1.Left(2) + temp1.Mid(2,2);
			
			CHAR wstation[21]; DWORD wsize;
			
			wsize = sizeof( wstation );
			GetComputerName( wstation, &wsize );*/
			 pintime_get_gmt ( dest_str );
			memcpy ((LPTSTR)sHistoryData.reserved,(LPTSTR)wstation, sizeof(sHistoryData.reserved));
            memcpy ((LPTSTR) sHistoryData.date_modified, temp1, sizeof (sHistoryData.date_modified));
            memcpy ((LPTSTR) sHistoryData.file_primary_key,  (LPCTSTR)m_StrUserNameFld, sizeof ( sHistoryData.file_primary_key));
            memcpy ((LPTSTR) sHistoryData.primary_key.unique_id,     (LPCTSTR)dest_str, sizeof ( sHistoryData.primary_key.unique_id));        
            memcpy ((LPTSTR) sHistoryData.time_modified,     (LPCTSTR)strTime, sizeof ( sHistoryData.time_modified)); 
            memcpy ((LPTSTR) sHistoryData.field_tag,     "GROUP ID", sizeof ( sHistoryData.field_tag));
            memcpy ((LPTSTR) sHistoryData.previous_data, Old_Gid, sizeof ( sHistoryData.previous_data));  
            memcpy ((LPTSTR) sHistoryData.current_data, New_Gid, sizeof ( sHistoryData.current_data));                        
			memcpy ((LPTSTR)sHistoryData.changed_by,(LPTSTR)ucf01.primary_key.name, sizeof(sHistoryData.changed_by));
			int rcode = txdsapi_insert_record  ( (pBYTE)&sHistoryData, sizeof (HST01), HST01_DATA, strErrorMsg );
			if ( rcode != PTEMSG_OK )
			{
				m_StrMessageLbl = strErrorMsg;
				UpdateData( FALSE );
			}
            
    }
	
	if(Old_Uname != New_Uname) // checking for user name update
	{
			/*myUtilsObject.GetSystemTime(strTime);
            pintime_get_gmt ( dest_str );
            ZeroMemory ( &sHistoryData, sizeof (HST01));
            memcpy ((LPTSTR) sHistoryData.file_name,     (LPCTSTR)"UCF01", sizeof ( sHistoryData.file_name));
            temp1 = strDate;
            temp1 = temp1.Right(4) + temp1.Left(2) + temp1.Mid(2,2);
			
			CHAR wstation[21]; DWORD wsize;
			
			wsize = sizeof( wstation );
			GetComputerName( wstation, &wsize );*/
			 pintime_get_gmt ( dest_str );
			memcpy ((LPTSTR)sHistoryData.reserved,(LPTSTR)wstation, sizeof(sHistoryData.reserved));
            memcpy ((LPTSTR) sHistoryData.date_modified, temp1, sizeof (sHistoryData.date_modified));
            memcpy ((LPTSTR) sHistoryData.file_primary_key,  (LPCTSTR)m_StrUserNameFld, sizeof ( sHistoryData.file_primary_key));
            memcpy ((LPTSTR) sHistoryData.primary_key.unique_id,     (LPCTSTR)dest_str, sizeof ( sHistoryData.primary_key.unique_id));        
            memcpy ((LPTSTR) sHistoryData.time_modified,     (LPCTSTR)strTime, sizeof ( sHistoryData.time_modified)); 
            memcpy ((LPTSTR) sHistoryData.field_tag, "USER NAME", sizeof ( sHistoryData.field_tag));
            memcpy ((LPTSTR) sHistoryData.previous_data,Old_Uname, sizeof ( sHistoryData.previous_data));  
            memcpy ((LPTSTR) sHistoryData.current_data,New_Uname, sizeof ( sHistoryData.current_data));                        
			memcpy ((LPTSTR)sHistoryData.changed_by,(LPTSTR)ucf01.primary_key.name, sizeof(sHistoryData.changed_by));
			int rcode = txdsapi_insert_record  ( (pBYTE)&sHistoryData, sizeof (HST01), HST01_DATA, strErrorMsg );
			if ( rcode != PTEMSG_OK )
			{
				m_StrMessageLbl = strErrorMsg;
				UpdateData( FALSE );
			}
            
    }
	if(!m_strNewPassword.IsEmpty())
	{
		char new_Encryp_pswrd[51]={0};
		strcpy(new_Encryp_pswrd,m_strNewPassword);
		int len = strlen(new_Encryp_pswrd);
		if(len <= MAX_USERPWD_LEN)
		{
				int encrypt_len = gui_password_encryption((BYTE *)new_Encryp_pswrd,len);
		}
		pintime_get_gmt ( dest_str );
		memcpy ((LPTSTR)sHistoryData.reserved,(LPTSTR)wstation, sizeof(sHistoryData.reserved));
        memcpy ((LPTSTR) sHistoryData.date_modified, temp1, sizeof (sHistoryData.date_modified));
        memcpy ((LPTSTR) sHistoryData.file_primary_key,  (LPCTSTR)m_StrUserNameFld, sizeof ( sHistoryData.file_primary_key));
        memcpy ((LPTSTR) sHistoryData.primary_key.unique_id,     (LPCTSTR)dest_str, sizeof ( sHistoryData.primary_key.unique_id));        
        memcpy ((LPTSTR) sHistoryData.time_modified,     (LPCTSTR)strTime, sizeof ( sHistoryData.time_modified)); 
        memcpy ((LPTSTR) sHistoryData.field_tag, "CHNAGE PASSWORD", sizeof ( sHistoryData.field_tag));
        memcpy ((LPTSTR) sHistoryData.previous_data,Old_pswrd, sizeof ( sHistoryData.previous_data));  
        memcpy ((LPTSTR) sHistoryData.current_data,new_Encryp_pswrd, sizeof ( sHistoryData.current_data));                        
		memcpy ((LPTSTR)sHistoryData.changed_by,(LPTSTR)ucf01.primary_key.name, sizeof(sHistoryData.changed_by));
		int rcode = txdsapi_insert_record  ( (pBYTE)&sHistoryData, sizeof (HST01), HST01_DATA, strErrorMsg );
		if ( rcode != PTEMSG_OK )
		{
			m_StrMessageLbl = strErrorMsg;
			UpdateData( FALSE );
		}
	}
	
}

BOOL CUserAccess::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	if ( m_nDialogMode != MODE_VIEW)
		SetModified ( TRUE );
	c_time = CTime::GetCurrentTime();	
	shrmem.SetLastUpdateTime(c_time);
	return CPropertyPage::OnCommand(wParam, lParam);
}


BOOL CUserAccess::PopulateDialogFromDatabase ()
{
	
	if ( m_nDialogMode != MODE_VIEW )
		EnableUserAccessDialog(TRUE);
	else
		EnableUserAccessDialog(FALSE);
	
	
	if (m_nDialogMode == MODE_ADD || m_nDialogMode == MODE_EDIT || m_nDialogMode == MODE_VIEW || m_bGroup )
    {
		
			//System Administration
		if (m_sControl[0].action[0] == '1')
			m_CntlAddCheck1.SetCheck(1);
		if (m_sControl[0].action[1] == '1')
			m_CntlDeleteCheck1.SetCheck(1);
		if (m_sControl[0].action[2] == '1')
			m_CntlUpdateCheck1.SetCheck(1);
		if (m_sControl[0].action[3] == '1')
			m_CntlViewCheck1.SetCheck(1);

		//System Configuration		
		if (m_sControl[1].action[0] == '1')
			m_CntlAddCheck2.SetCheck(1);
		if (m_sControl[1].action[1] == '1')
			m_CntlDeleteCheck2.SetCheck(1);
		if (m_sControl[1].action[2] == '1')
			m_CntlUpdateCheck2.SetCheck(1);
		if (m_sControl[1].action[3] == '1')
			m_CntlViewCheck2.SetCheck(1);
		
		//System Operations    
		if (m_sControl[2].action[0] == '1')
			m_CntlAddCheck3.SetCheck(1);
		if (m_sControl[2].action[1] == '1')
			m_CntlDeleteCheck3.SetCheck(1);
		if (m_sControl[2].action[2] == '1')
			m_CntlUpdateCheck3.SetCheck(1);
		if (m_sControl[2].action[3] == '1')
			m_CntlViewCheck3.SetCheck(1);
		
		//BIN Control
		if (m_sControl[3].action[0] == '1')
			m_CntlAddCheck4.SetCheck(1);
		if (m_sControl[3].action[1] == '1')
			m_CntlDeleteCheck4.SetCheck(1);
		if (m_sControl[3].action[2] == '1')
			m_CntlUpdateCheck4.SetCheck(1);
		if (m_sControl[3].action[3] == '1')
			m_CntlViewCheck4.SetCheck(1);
		
		//Authorization Control
		if (m_sControl[4].action[0] == '1')
			m_CntlAddCheck5.SetCheck(1);
		if (m_sControl[4].action[1] == '1')
			m_CntlDeleteCheck5.SetCheck(1);
		if (m_sControl[4].action[2] == '1')
			m_CntlUpdateCheck5.SetCheck(1);
		if (m_sControl[4].action[3] == '1')
			m_CntlViewCheck5.SetCheck(1);
		
		//Merchant Control
		if (m_sControl[5].action[0] == '1')
			m_CntlAddCheck6.SetCheck(1);
		if (m_sControl[5].action[1] == '1')
			m_CntlDeleteCheck6.SetCheck(1);
		if (m_sControl[5].action[2] == '1')
			m_CntlUpdateCheck6.SetCheck(1);
		if (m_sControl[5].action[3] == '1')
			m_CntlViewCheck6.SetCheck(1);
		
		//Device definition
		if (m_sControl[6].action[0] == '1')
			m_CntlAddCheck7.SetCheck(1);
		if (m_sControl[6].action[1] == '1')
			m_CntlDeleteCheck7.SetCheck(1);
		if (m_sControl[6].action[2] == '1')
			m_CntlUpdateCheck7.SetCheck(1);
		if (m_sControl[6].action[3] == '1')
			m_CntlViewCheck7.SetCheck(1);
		
		//Network Control
		if (m_sControl[7].action[0] == '1')
			m_CntlAddCheck8.SetCheck(1);
		if (m_sControl[7].action[1] == '1')
			m_CntlDeleteCheck8.SetCheck(1);
		if (m_sControl[7].action[2] == '1')
			m_CntlUpdateCheck8.SetCheck(1);
		if (m_sControl[7].action[3] == '1')
			m_CntlViewCheck8.SetCheck(1);
		
		//Card Account Control
		if (m_sControl[8].action[0] == '1')
			m_CntlAddCheck9.SetCheck(1);
		if (m_sControl[8].action[1] == '1')
			m_CntlDeleteCheck9.SetCheck(1);
		if (m_sControl[8].action[2] == '1')
			m_CntlUpdateCheck9.SetCheck(1);
		if (m_sControl[8].action[3] == '1')
			m_CntlViewCheck9.SetCheck(1);
		
		//Voice Authorization
		if (m_sControl[9].action[0] == '1')
			m_CntlAddCheck10.SetCheck(1);
		if (m_sControl[9].action[1] == '1')
			m_CntlDeleteCheck10.SetCheck(1);
		if (m_sControl[9].action[2] == '1')
			m_CntlUpdateCheck10.SetCheck(1);
		if (m_sControl[9].action[3] == '1')
			m_CntlViewCheck10.SetCheck(1);
		
		//Transaction Lookup
		if (m_sControl[10].action[0] == '1')
			m_CntlAddCheck11.SetCheck(1);
		if (m_sControl[10].action[1] == '1')
			m_CntlDeleteCheck11.SetCheck(1);
		if (m_sControl[10].action[2] == '1')
			m_CntlUpdateCheck11.SetCheck(1);
		if (m_sControl[10].action[3] == '1')
			m_CntlViewCheck11.SetCheck(1);
		if(m_sControl[10].action[4] == '1')
			m_CntlTlfExtract.SetCheck(1);
		
		//Recurring Payment
			/*  if (m_sControl[11].action[0] == '1')
			m_CntlAddCheck12.SetCheck(1);
			if (m_sControl[11].action[1] == '1')
			m_CntlDeleteCheck12.SetCheck(1);
			if (m_sControl[11].action[2] == '1')
			m_CntlUpdateCheck12.SetCheck(1);
			if (m_sControl[11].action[3] == '1')
			m_CntlViewCheck12.SetCheck(1);
			
		*/
		//Krishna@TF, For Dual Encryption option
		if (m_sControl[11].action[0] == '1')
			m_CntlAddCheck13.SetCheck(1);
		else 
		{
			m_CntlAddCheck13.SetCheck(0);
			CheckCountofUsersAssignedforDualEncryption(m_StrUserNameFld, 0);
		}
		
		
		//Service Manager
		if (m_sControl[13].action[3] == '1')
			m_CntlViewCheck15.SetCheck(1);

		//Override Merchant Denial
		if (m_sControl[14].action[0] == '1')
			m_CntlAddCheck14.SetCheck(1);
		if (m_sControl[14].action[1] == '1')
			m_CntlDeleteCheck14.SetCheck(1);
		if (m_sControl[14].action[2] == '1')
			m_CntlUpdateCheck14.SetCheck(1);
		if (m_sControl[14].action[3] == '1')
			m_CntlViewCheck14.SetCheck(1);

		//ATP Archiving
		if (m_sControl[15].action[0] == '1')
			m_CntlViewCheck20.SetCheck(1);
		if (m_sControl[15].action[1] == '1')
			m_CntlDeleteCheck18.SetCheck(1);
		if (m_sControl[15].action[2] == '1')
			m_CntlUpdateCheck18.SetCheck(1);
		if (m_sControl[15].action[3] == '1')
			m_CntlViewCheck18.SetCheck(1);

		// System Global Parameter
		if(m_sControl[16].action[2]=='1')
			m_CntlUpdateCheck17.SetCheck(1);
		if(m_sControl[16].action[3]=='1')
			m_CntlViewCheck17.SetCheck(1);

		if(m_sControl[17].action[3]=='1')
			m_CntlViewCheck21.SetCheck(1);

				//Fraud Control
		if (m_sControl[18].action[0] == '1')
			m_CntlAddCheck22.SetCheck(1);
		if (m_sControl[18].action[1] == '1')
			m_CntlDeleteCheck22.SetCheck(1);
		if (m_sControl[18].action[2] == '1')
			m_CntlUpdateCheck22.SetCheck(1);
		if (m_sControl[18].action[3] == '1')
			m_CntlViewCheck22.SetCheck(1);

				
		//Chipfallback
		if (m_sControl[19].action[0] == '1')
			m_CntlAddCheck23.SetCheck(1);
		if (m_sControl[19].action[1] == '1')
			m_CntlDeleteCheck23.SetCheck(1);
		if (m_sControl[19].action[2] == '1')
			m_CntlUpdateCheck23.SetCheck(1);
		if (m_sControl[19].action[3] == '1')
			m_CntlViewCheck23.SetCheck(1);

 }
 
	if (!UpdateData(FALSE)){
		AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
	}
	
	SetModified ( FALSE );
	return TRUE;
}

void CUserAccess::EnableUserAccessDialog(int nStatus)
{
	m_CntlGroupListButton.EnableWindow(nStatus);
	m_CntlChangePasswordButton.EnableWindow(nStatus);

	m_CntlGroupIDFld.EnableWindow(nStatus);
	m_CntlUserNameFld2.EnableWindow(nStatus);

	m_CntlUpdateCheck1.EnableWindow(nStatus);
	m_CntlUpdateCheck2.EnableWindow(nStatus);
	m_CntlUpdateCheck3.EnableWindow(nStatus);
	m_CntlUpdateCheck4.EnableWindow(nStatus);
	m_CntlUpdateCheck5.EnableWindow(nStatus);
	m_CntlUpdateCheck6.EnableWindow(nStatus);
	m_CntlUpdateCheck7.EnableWindow(nStatus);
	m_CntlUpdateCheck8.EnableWindow(nStatus);
	m_CntlUpdateCheck9.EnableWindow(nStatus);
	m_CntlUpdateCheck10.EnableWindow(nStatus);
	m_CntlUpdateCheck11.EnableWindow(nStatus);
	//m_CntlUpdateCheck12.EnableWindow(nStatus);
//	m_CntlUpdateCheck13.EnableWindow(nStatus);	//Dual Encryption doesnt have update
	m_CntlUpdateCheck14.EnableWindow(nStatus);
	m_CntlUpdateCheck18.EnableWindow(nStatus);	//Archiving doesnt have the update
	m_CntlAll.EnableWindow(nStatus);
	m_CntlUpdateCheck22.EnableWindow(nStatus);
	m_CntlUpdateCheck23.EnableWindow(nStatus);
	m_CntlUpdateCheck23.EnableWindow(nStatus);

	m_CntlAddCheck1.EnableWindow(nStatus);
	m_CntlAddCheck2.EnableWindow(nStatus);
	m_CntlAddCheck3.EnableWindow(nStatus);
	m_CntlAddCheck4.EnableWindow(nStatus);
	m_CntlAddCheck5.EnableWindow(nStatus);
	m_CntlAddCheck6.EnableWindow(nStatus);
	m_CntlAddCheck7.EnableWindow(nStatus);
	m_CntlAddCheck8.EnableWindow(nStatus);
	m_CntlAddCheck9.EnableWindow(nStatus);
	m_CntlAddCheck10.EnableWindow(nStatus);
	m_CntlAddCheck11.EnableWindow(nStatus);
	//m_CntlAddCheck12.EnableWindow(nStatus);
	m_CntlAddCheck13.EnableWindow(nStatus);	
	m_CntlAddCheck14.EnableWindow(nStatus);
	m_CntlAddCheck22.EnableWindow(nStatus);
	m_CntlAddCheck23.EnableWindow(nStatus);
	//KrishnaTF, For Dual Encryption
//	m_CntlAddCheck13.EnableWindow(nStatus);
	
	m_CntlDeleteCheck1.EnableWindow(nStatus);
	m_CntlDeleteCheck2.EnableWindow(nStatus);
	m_CntlDeleteCheck3.EnableWindow(nStatus);
	m_CntlDeleteCheck4.EnableWindow(nStatus);
	m_CntlDeleteCheck5.EnableWindow(nStatus);
	m_CntlDeleteCheck6.EnableWindow(nStatus);
	m_CntlDeleteCheck7.EnableWindow(nStatus);
	m_CntlDeleteCheck8.EnableWindow(nStatus);
	m_CntlDeleteCheck9.EnableWindow(nStatus);
	m_CntlDeleteCheck10.EnableWindow(nStatus);
	m_CntlDeleteCheck11.EnableWindow(nStatus);
	//m_CntlDeleteCheck12.EnableWindow(nStatus);
//	m_CntlDeleteCheck13.EnableWindow(nStatus);	//Dual Encryption doesnt have Delete
	m_CntlDeleteCheck14.EnableWindow(nStatus);
	m_CntlDeleteCheck18.EnableWindow(nStatus);
	m_CntlDeleteCheck22.EnableWindow(nStatus);
	m_CntlDeleteCheck23.EnableWindow(nStatus);

	m_CntlViewCheck1.EnableWindow(nStatus);
	m_CntlViewCheck2.EnableWindow(nStatus);
	m_CntlViewCheck3.EnableWindow(nStatus);
	m_CntlViewCheck4.EnableWindow(nStatus);
	m_CntlViewCheck5.EnableWindow(nStatus);
	m_CntlViewCheck6.EnableWindow(nStatus);
	m_CntlViewCheck7.EnableWindow(nStatus);
	m_CntlViewCheck8.EnableWindow(nStatus);
	m_CntlViewCheck9.EnableWindow(nStatus);
	m_CntlViewCheck10.EnableWindow(nStatus);
	m_CntlViewCheck11.EnableWindow(nStatus);
	//m_CntlViewCheck12.EnableWindow(nStatus);
//	m_CntlViewCheck13.EnableWindow(nStatus);	//Dual Encryption doesnt have View
	m_CntlViewCheck14.EnableWindow(nStatus);
	m_CntlViewCheck15.EnableWindow(nStatus);
	m_CntlViewCheck18.EnableWindow(nStatus);
	m_CntlViewCheck20.EnableWindow(nStatus);
	m_CntlViewCheck22.EnableWindow(nStatus);
	m_CntlViewCheck23.EnableWindow(nStatus);

	m_CntlUserNameFld2.EnableWindow(nStatus);
	m_CntlGroupIDFld.EnableWindow(nStatus);
	m_CntlGroupListButton.EnableWindow(nStatus);
	m_CntlChangePasswordButton.EnableWindow(nStatus);

	m_CntlOverrideCheck.EnableWindow(nStatus);

	//ATP Archiving has no add, update, delete
	m_CntlAddCheck18.ShowWindow(FALSE);

	m_CntlUpdateCheck17.EnableWindow(nStatus);
	m_CntlViewCheck17.EnableWindow(nStatus);

	m_CntlViewCheck21.EnableWindow(nStatus);
	//m_CntlUpdateCheck18.ShowWindow(FALSE);
	//m_CntlDeleteCheck18.ShowWindow(FALSE);

	m_CntlTlfExtract.EnableWindow(nStatus);
	
}

void CUserAccess::CleanUserAccessDialog()
{
	m_CntlUpdateCheck1.SetCheck(0);
	m_CntlUpdateCheck2.SetCheck(0);
	m_CntlUpdateCheck3.SetCheck(0);
	m_CntlUpdateCheck4.SetCheck(0);
	m_CntlUpdateCheck5.SetCheck(0);
	m_CntlUpdateCheck6.SetCheck(0);
	m_CntlUpdateCheck7.SetCheck(0);
	m_CntlUpdateCheck8.SetCheck(0);
	m_CntlUpdateCheck9.SetCheck(0);
	m_CntlUpdateCheck10.SetCheck(0);
	m_CntlUpdateCheck11.SetCheck(0);
//	m_CntlUpdateCheck12.SetCheck(0);	//Recurring payment is not used
//	m_CntlUpdateCheck13.SetCheck(0);	//Dual Encryption doesnt have update
	m_CntlUpdateCheck14.SetCheck(0);
	m_CntlUpdateCheck18.SetCheck(0);
	m_CntlUpdateCheck22.SetCheck(0);
	m_CntlUpdateCheck23.SetCheck(0);
	
	m_CntlAddCheck1.SetCheck(0);
	m_CntlAddCheck2.SetCheck(0);
	m_CntlAddCheck3.SetCheck(0);
	m_CntlAddCheck4.SetCheck(0);
	m_CntlAddCheck5.SetCheck(0);
	m_CntlAddCheck6.SetCheck(0);
	m_CntlAddCheck7.SetCheck(0);
	m_CntlAddCheck8.SetCheck(0);
	m_CntlAddCheck9.SetCheck(0);
	m_CntlAddCheck10.SetCheck(0);
	m_CntlAddCheck11.SetCheck(0);
//	m_CntlAddCheck12.SetCheck(0);	//Recurring payment is not used
	m_CntlAddCheck13.SetCheck(0);
	m_CntlAddCheck14.SetCheck(0);
	m_CntlAddCheck23.SetCheck(0);
	//KrishnaTF, For Dual Encryption
	m_CntlAddCheck13.SetCheck(0);
	m_CntlAddCheck22.SetCheck(0);
	m_CntlAddCheck23.SetCheck(0);
	
	m_CntlDeleteCheck1.SetCheck(0);
	m_CntlDeleteCheck2.SetCheck(0);
	m_CntlDeleteCheck3.SetCheck(0);
	m_CntlDeleteCheck4.SetCheck(0);
	m_CntlDeleteCheck5.SetCheck(0);
	m_CntlDeleteCheck6.SetCheck(0);
	m_CntlDeleteCheck7.SetCheck(0);
	m_CntlDeleteCheck8.SetCheck(0);
	m_CntlDeleteCheck9.SetCheck(0);
	m_CntlDeleteCheck10.SetCheck(0);
	m_CntlDeleteCheck11.SetCheck(0);
//	m_CntlDeleteCheck12.SetCheck(0);	//Recurring payment is not used
//	m_CntlDeleteCheck13.SetCheck(0);	//Dual Encryption doesnt have update
	m_CntlDeleteCheck14.SetCheck(0);
	m_CntlDeleteCheck18.SetCheck(0);
	m_CntlDeleteCheck22.SetCheck(0);
	m_CntlDeleteCheck23.SetCheck(0);

	m_CntlViewCheck1.SetCheck(0);
	m_CntlViewCheck2.SetCheck(0);
	m_CntlViewCheck3.SetCheck(0);
	m_CntlViewCheck4.SetCheck(0);
	m_CntlViewCheck5.SetCheck(0);
	m_CntlViewCheck6.SetCheck(0);
	m_CntlViewCheck7.SetCheck(0);
	m_CntlViewCheck8.SetCheck(0);
	m_CntlViewCheck9.SetCheck(0);
	m_CntlViewCheck10.SetCheck(0);
	m_CntlViewCheck11.SetCheck(0);
//	m_CntlViewCheck12.SetCheck(0);	//Recurring payment is not used
//	m_CntlViewCheck13.SetCheck(0);	//Dual Encryption doesnt have update
	m_CntlViewCheck14.SetCheck(0);
	m_CntlViewCheck15.SetCheck(0);
	m_CntlViewCheck18.SetCheck(0);
	m_CntlViewCheck20.SetCheck(0);
	m_CntlUpdateCheck17.SetCheck(0);
	m_CntlViewCheck17.SetCheck(0);
	m_CntlViewCheck21.SetCheck(0);
	m_CntlViewCheck22.SetCheck(0);
	m_CntlViewCheck23.SetCheck(0);
	m_CntlTlfExtract.SetCheck(0);
	m_StrUserNameFld = "";
	m_StrUserNameFld2 = "";
	m_StrGroupIDFld = "";
	m_StrMessageLbl = "";

	m_CntlOverrideCheck.SetCheck(0);
	
	UpdateData(FALSE);
}


BOOL CUserAccess::OnSetActive() 
{
	((CUserPropertySheet*) this->GetParent())->m_nActiveTab = 1;
	
	return CPropertyPage::OnSetActive();
}

BOOL CUserAccess::OnHelpInfo(HELPINFO* pHelpInfo) 
{
	
	
	return TRUE;
}

void CUserAccess::OnCheckAll() 
{
	
	if(m_CntlAll.GetCheck()==1)
	{
		m_CntlUpdateCheck1.SetCheck(1);
		m_CntlUpdateCheck2.SetCheck(1);
		m_CntlUpdateCheck3.SetCheck(1);
		m_CntlUpdateCheck4.SetCheck(1);
		m_CntlUpdateCheck5.SetCheck(1);
		m_CntlUpdateCheck6.SetCheck(1);
		m_CntlUpdateCheck7.SetCheck(1);
		m_CntlUpdateCheck8.SetCheck(1);
		m_CntlUpdateCheck9.SetCheck(1);
		m_CntlUpdateCheck10.SetCheck(1);
		m_CntlUpdateCheck11.SetCheck(1);
//		m_CntlUpdateCheck12.SetCheck(1);
//		m_CntlUpdateCheck13.SetCheck(1);
		m_CntlUpdateCheck14.SetCheck(1);
		m_CntlUpdateCheck18.SetCheck(1);
		m_CntlUpdateCheck22.SetCheck(1);
		m_CntlUpdateCheck23.SetCheck(1);
		
		m_CntlAddCheck1.SetCheck(1);
		m_CntlAddCheck2.SetCheck(1);
		m_CntlAddCheck3.SetCheck(1);
		m_CntlAddCheck4.SetCheck(1);
		m_CntlAddCheck5.SetCheck(1);
		m_CntlAddCheck6.SetCheck(1);
		m_CntlAddCheck7.SetCheck(1);
		m_CntlAddCheck8.SetCheck(1);
		m_CntlAddCheck9.SetCheck(1);
		m_CntlAddCheck10.SetCheck(1);
		m_CntlAddCheck11.SetCheck(1);
//		m_CntlAddCheck12.SetCheck(1);
		if(!m_StrLabel13.IsEmpty())
			m_CntlAddCheck13.SetCheck(1);	//Dual Encryption
		m_CntlAddCheck14.SetCheck(1);
		m_CntlAddCheck22.SetCheck(1);
		m_CntlAddCheck23.SetCheck(1);

		m_CntlDeleteCheck1.SetCheck(1);
		m_CntlDeleteCheck2.SetCheck(1);
		m_CntlDeleteCheck3.SetCheck(1);
		m_CntlDeleteCheck4.SetCheck(1);
		m_CntlDeleteCheck5.SetCheck(1);
		m_CntlDeleteCheck6.SetCheck(1);
		m_CntlDeleteCheck7.SetCheck(1);
		m_CntlDeleteCheck8.SetCheck(1);
		m_CntlDeleteCheck9.SetCheck(1);
		m_CntlDeleteCheck10.SetCheck(1);
		m_CntlDeleteCheck11.SetCheck(1);
//		m_CntlDeleteCheck12.SetCheck(1);
//		m_CntlDeleteCheck13.SetCheck(1);
		m_CntlDeleteCheck14.SetCheck(1);
		m_CntlDeleteCheck18.SetCheck(1);
		m_CntlDeleteCheck22.SetCheck(1);
		m_CntlDeleteCheck23.SetCheck(1);
		
		m_CntlViewCheck1.SetCheck(1);
		m_CntlViewCheck2.SetCheck(1);
		m_CntlViewCheck3.SetCheck(1);
		m_CntlViewCheck4.SetCheck(1);
		m_CntlViewCheck5.SetCheck(1);
		m_CntlViewCheck6.SetCheck(1);
		m_CntlViewCheck7.SetCheck(1);
		m_CntlViewCheck8.SetCheck(1);
		m_CntlViewCheck9.SetCheck(1);
		m_CntlViewCheck10.SetCheck(1);
		m_CntlViewCheck11.SetCheck(1);
//		m_CntlViewCheck12.SetCheck(1);
//		m_CntlViewCheck13.SetCheck(1);
		m_CntlViewCheck14.SetCheck(1);
		m_CntlViewCheck15.SetCheck(1);
		m_CntlViewCheck18.SetCheck(1);
		m_CntlViewCheck20.SetCheck(1);
		m_CntlUpdateCheck17.SetCheck(1);
		m_CntlViewCheck17.SetCheck(1);
		m_CntlOverrideCheck.SetCheck(1);
		m_CntlViewCheck21.SetCheck(1);
		m_CntlViewCheck22.SetCheck(1);
		m_CntlViewCheck23.SetCheck(1);

		m_CntlTlfExtract.SetCheck(1);
	}
	else
	{
		m_CntlUpdateCheck1.SetCheck(0);
		m_CntlUpdateCheck2.SetCheck(0);
		m_CntlUpdateCheck3.SetCheck(0);
		m_CntlUpdateCheck4.SetCheck(0);
		m_CntlUpdateCheck5.SetCheck(0);
		m_CntlUpdateCheck6.SetCheck(0);
		m_CntlUpdateCheck7.SetCheck(0);
		m_CntlUpdateCheck8.SetCheck(0);
		m_CntlUpdateCheck9.SetCheck(0);
		m_CntlUpdateCheck10.SetCheck(0);
		m_CntlUpdateCheck11.SetCheck(0);
//		m_CntlUpdateCheck12.SetCheck(0);
//		m_CntlUpdateCheck13.SetCheck(0);
		m_CntlUpdateCheck14.SetCheck(0);
		m_CntlUpdateCheck18.SetCheck(0);
		m_CntlUpdateCheck22.SetCheck(0);
		m_CntlUpdateCheck23.SetCheck(0);

		m_CntlAddCheck1.SetCheck(0);
		m_CntlAddCheck2.SetCheck(0);
		m_CntlAddCheck3.SetCheck(0);
		m_CntlAddCheck4.SetCheck(0);
		m_CntlAddCheck5.SetCheck(0);
		m_CntlAddCheck6.SetCheck(0);
		m_CntlAddCheck7.SetCheck(0);
		m_CntlAddCheck8.SetCheck(0);
		m_CntlAddCheck9.SetCheck(0);
		m_CntlAddCheck10.SetCheck(0);
		m_CntlAddCheck11.SetCheck(0);
//		m_CntlAddCheck12.SetCheck(0);
		m_CntlAddCheck13.SetCheck(0);
		m_CntlAddCheck14.SetCheck(0);
		m_CntlAddCheck22.SetCheck(0);
		m_CntlAddCheck23.SetCheck(0);

		m_CntlDeleteCheck1.SetCheck(0);
		m_CntlDeleteCheck2.SetCheck(0);
		m_CntlDeleteCheck3.SetCheck(0);
		m_CntlDeleteCheck4.SetCheck(0);
		m_CntlDeleteCheck5.SetCheck(0);
		m_CntlDeleteCheck6.SetCheck(0);
		m_CntlDeleteCheck7.SetCheck(0);
		m_CntlDeleteCheck8.SetCheck(0);
		m_CntlDeleteCheck9.SetCheck(0);
		m_CntlDeleteCheck10.SetCheck(0);
		m_CntlDeleteCheck11.SetCheck(0);
//		m_CntlDeleteCheck12.SetCheck(0);
//		m_CntlDeleteCheck13.SetCheck(0);
		m_CntlDeleteCheck14.SetCheck(0);
		m_CntlDeleteCheck18.SetCheck(0);
		m_CntlDeleteCheck22.SetCheck(0);
		m_CntlDeleteCheck23.SetCheck(0);

		m_CntlViewCheck1.SetCheck(0);
		m_CntlViewCheck2.SetCheck(0);
		m_CntlViewCheck3.SetCheck(0);
		m_CntlViewCheck4.SetCheck(0);
		m_CntlViewCheck5.SetCheck(0);
		m_CntlViewCheck6.SetCheck(0);
		m_CntlViewCheck7.SetCheck(0);
		m_CntlViewCheck8.SetCheck(0);
		m_CntlViewCheck9.SetCheck(0);
		m_CntlViewCheck10.SetCheck(0);
		m_CntlViewCheck11.SetCheck(0);
//		m_CntlViewCheck12.SetCheck(0);
//		m_CntlViewCheck13.SetCheck(0);
		m_CntlViewCheck14.SetCheck(0);
		m_CntlViewCheck15.SetCheck(0);
		m_CntlViewCheck18.SetCheck(0);
		m_CntlViewCheck20.SetCheck(0);
		m_CntlUpdateCheck17.SetCheck(0);
		m_CntlViewCheck17.SetCheck(0);
		m_CntlOverrideCheck.SetCheck(0);
		m_CntlViewCheck21.SetCheck(0);
		m_CntlViewCheck22.SetCheck(0);
		m_CntlViewCheck23.SetCheck(0);
		m_CntlTlfExtract.SetCheck(0);
	}	
}

void CUserAccess::OnCheckEncryptionControl() 
{
	memset(&sUserRecord, 0, sizeof(sUserRecord)) ;
	
	if(m_CntlAddCheck13.GetCheck() == 1)
	{			
		CheckCountofUsersAssignedforDualEncryption(m_StrUserNameFld, 1);
	}
}

BYTE CUserAccess::CheckCountofUsersAssignedforDualEncryption(CString m_Username, int idx)
{
	//Krishna@TF, 2.6.11, Retreive the Other User Dual Encryption Param from DB	
	
	char error_msg[256];
	int ekeyusercount=0;
	int rcode;
	memcpy ((LPSTR)(sUserRecord.primary_key.name), m_Username, sizeof (sUserRecord.primary_key.name));
	
	rcode = txdsapi_Select_Ucf01_DualEncryptionKey_UserCount(&sUserRecord, error_msg);
	
	if(rcode != PTEMSG_OK)
	{
		AfxMessageBox(error_msg);
		return -1;
	}
	
	ekeyusercount = atoi((char *)sUserRecord.dekey);
	if(ekeyusercount == 2)
	{
		if(idx == 1)
			AfxMessageBox("Cannot be assigned to user as Dual Encryption Key is already entered in System");
		
		m_CntlAddCheck13.SetCheck(0);
		m_CntlAddCheck13.ShowWindow(FALSE);
		m_CntlAddCheck13.EnableWindow(FALSE);	
		m_StrLabel13 = "";		
	}
	
	return ekeyusercount;
}

void CUserAccess::OnGroupListButton() 
{
	char strErrorMessage[200];
	CNameList listDlg;
    int nReturnCode;

	m_StrMessageLbl = "";
	UpdateData(TRUE);
    listDlg.m_nIndicator = MODE_GROUP;
	listDlg.strPrimaryKey1 = "";

    nReturnCode = listDlg.DoModal();
	if ( nReturnCode == IDOK)
	{
		if(m_StrGroupIDFld.Compare(listDlg.m_strName) != 0)
		{
			m_bGroupIDChanged =true;
			m_StrGroupIDFld = listDlg.m_strName;
			memcpy ((LPSTR)(sGroupRecord.primary_key.group_id), m_StrGroupIDFld, sizeof (sGroupRecord.primary_key.group_id) );
			int rcode = txdsapi_get_record  ((pBYTE)&sGroupRecord,sizeof (UCF02), UCF02_DATA, strErrorMessage);
			if ( rcode != PTEMSG_OK )
			{
				switch( rcode )
				{
				case PTEMSG_NOT_FOUND:
					sprintf( strErrorMessage, "ERROR: Group record for %s does not exist", m_StrGroupIDFld );
					break;
				}
				
				m_StrMessageLbl = strErrorMessage;
				m_nDialogMode = MODE_DONE;
				if (!UpdateData(FALSE)){
					AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
				}
				
				return;
			}
			memcpy(&m_sControl, &sGroupRecord.control, sizeof(m_sControl));
			m_CntlAll.SetCheck(0);
			OnCheckAll();
			PopulateDialogFromDatabase();
				SetModified ( TRUE );


		}
	}
        
    if (!UpdateData(FALSE)){
		AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
	}		
}

void CUserAccess::OnChangePasswordButton() 
{
	CChangePassword ChangePass;

	int ret;
		//CHAR temp_passwd[20];

	while(1)
	{				
					
			//LogEvent( "in change passwordb", INFO_MSG );
			ChangePass.m_UserLabel = (char *)m_sUserRecord.primary_key.name;
			ChangePass.bDontCheckOldPassword = true;
			ret = ChangePass.DoModal();
			if( ret == IDOK )
			{
				//strcpy( temp_passwd, ChangePass.m_OldPassword );
				// check password
			/*	if( strcmp( (char *)m_sUserRecord.password, temp_passwd ) != 0 )
				{		
					AfxMessageBox( "Invalid Password .", MB_ICONSTOP | MB_OK );
					continue;
				}*/
				if( strcmp( (char *)m_sUserRecord.password_1, ChangePass.m_StrNewPassword ) == 0 || 
					strcmp( (char *)m_sUserRecord.password_2, ChangePass.m_StrNewPassword ) == 0 || 
					strcmp( (char *)m_sUserRecord.password_3, ChangePass.m_StrNewPassword )==0 || 
					strcmp( (char *)m_sUserRecord.password_4, ChangePass.m_StrNewPassword) == 0) 
				{		
					AfxMessageBox( "Enter Different Password from your last 4 passwords", MB_ICONSTOP | MB_OK );
					continue;
				}				
				
				m_strNewPassword = ChangePass.m_StrNewPassword;

			/*	We are updating the password here. Instead it will be updated in OnApply function.

				strcpy((char *)m_sUserRecord.password_4,(char *)m_sUserRecord.password_3);
				strcpy((char *)m_sUserRecord.password_3,(char *)m_sUserRecord.password_2);
				strcpy((char *)m_sUserRecord.password_2,(char *)m_sUserRecord.password_1);		
				strcpy((char *)m_sUserRecord.password_1, ChangePass.m_StrNewPassword );
				strcpy((char *)m_sUserRecord.password, ChangePass.m_StrNewPassword );
				strcpy((char *)m_sUserRecord.password_modified_date, toDayDate);
				*/

			//	rcode = txdsapi_update_record( (pBYTE)&m_sUserRecord, sizeof (UCF01), UCF01_DATA, error_msg );
			//	if( rcode == PTEMSG_OK )
			//		AfxMessageBox( "Your password has been updated", MB_ICONINFORMATION | MB_OK );
			//	else
			//		AfxMessageBox( error_msg, MB_ICONSTOP | MB_OK );
			}	
			break;

		}
	
}

void CUserAccess::OnChangeUserNameFld2() 
{
	SetModified ( TRUE );
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CPropertyPage::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	
}

void CUserAccess::OnChangeGroupIdFld() 
{
	SetModified ( TRUE );
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CPropertyPage::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	
}

void CUserAccess::OnLabel13() 
{
	//	Click on Dual Encryption label.
	if(!m_StrLabel13.IsEmpty() && m_CntlAddCheck13.IsWindowEnabled())
	{
		int check = m_CntlAddCheck13.GetCheck();
		m_CntlAddCheck13.SetCheck(!check);
	}
}

void CUserAccess::OnLabel16() 
{
	if(!m_StrLabel16.IsEmpty() && m_CntlOverrideCheck.IsWindowEnabled())
	{
		int check = m_CntlOverrideCheck.GetCheck();
		m_CntlOverrideCheck.SetCheck(!check);
	}	
}


void CUserAccess::OnBnClickedAddCheck18()
{
	// TODO: Add your control notification handler code here
}


void CUserAccess::OnBnClickedViewCheck12()
{
	// TODO: Add your control notification handler code here
}
