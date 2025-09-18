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
* $Log:   N:\PVCS\PTE\Equitable\ATP_gui\DBPasswords.cpp  $
   
      Rev 1.0   Jan 31 2011 11:30:22   TF_Srikanth
   Added DB Passwords Class for Changing Database Connectivity Passwords    
    
*
************************************************************************************/
//
// DBPasswords.cpp : implementation file
//

#include "stdafx.h"
#include "atp.h"
#include "DBPasswords.h"
extern "C"
{
   #include "ntutils.h"
}
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CHAR core_user_id[20],core_password[40],app_password[40],app_user_id[20],ini_file[256],ini_path[256];
DbPasswords dbpass;
CHAR error_msg[256];
int m_baccess;
/////////////////////////////////////////////////////////////////////////////
// CDBPasswords property page

IMPLEMENT_DYNCREATE(CDBPasswords, CPropertyPage)

CDBPasswords::CDBPasswords() : CPropertyPage(CDBPasswords::IDD)
{
	//{{AFX_DATA_INIT(CDBPasswords)
	m_StrAppUserFld = _T("");
	m_StrCoreUserFld = _T("");
	m_StrAppPassword = _T("");
	m_StrConfirmAppPassword = _T("");
	m_StrCorePassword = _T("");
	m_StrConfirmCorePassword = _T("");
	m_StrMessageLbl = _T("");
	m_StrAppOldPassword = _T("");
	m_StrCoreOldPassword = _T("");
	//}}AFX_DATA_INIT
}

CDBPasswords::~CDBPasswords()
{
		
}

void CDBPasswords::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDBPasswords)
	DDX_Control(pDX, IDC_CHECK_COREENABLE, m_CntlCoreEnable);
	DDX_Control(pDX, IDC_CORE_PASSWORD2, m_CntlCorePassword2);
	DDX_Control(pDX, IDC_CORE_PASSWORD, m_CntlCorePassword);
	DDX_Control(pDX, IDC_APP_PASSWORD2, m_CntlAppPassword2);
	DDX_Control(pDX, IDC_APP_PASSWORD, m_CntlAppPassword);
	DDX_Control(pDX, IDC_APP_OLD_PASSWORD, m_CntlAppoldPassword);
	DDX_Control(pDX, IDC_CORE_OLD_PASSWORD, m_CntlCoreOldPassword);
	DDX_Control(pDX, IDC_CHECK_APPENABLE, m_CntlAppEnable);
	DDX_Control(pDX, IDC_CORE_USER_ID, m_CntlCoreUserFld);
	DDX_Control(pDX, IDC_APP_USER_ID, m_CntlAppUserFld);
	DDX_Text(pDX, IDC_APP_USER_ID, m_StrAppUserFld);
	DDX_Text(pDX, IDC_CORE_USER_ID, m_StrCoreUserFld);
	DDX_Text(pDX, IDC_APP_PASSWORD, m_StrAppPassword);
	DDV_MaxChars(pDX, m_StrAppPassword, 12);
	DDX_Text(pDX, IDC_APP_PASSWORD2, m_StrConfirmAppPassword);
	DDV_MaxChars(pDX, m_StrConfirmAppPassword, 12);
	DDX_Text(pDX, IDC_CORE_PASSWORD, m_StrCorePassword);
	DDV_MaxChars(pDX, m_StrCorePassword, 12);
	DDX_Text(pDX, IDC_CORE_PASSWORD2, m_StrConfirmCorePassword);
	DDV_MaxChars(pDX, m_StrConfirmCorePassword, 12);
	DDX_Text(pDX, IDC_MESSAGE_LBL, m_StrMessageLbl);
	DDX_Text(pDX, IDC_APP_OLD_PASSWORD, m_StrAppOldPassword);
	DDV_MaxChars(pDX, m_StrAppOldPassword, 12);
	DDX_Text(pDX, IDC_CORE_OLD_PASSWORD, m_StrCoreOldPassword);
	DDV_MaxChars(pDX, m_StrCoreOldPassword, 12);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDBPasswords, CPropertyPage)
	//{{AFX_MSG_MAP(CDBPasswords)
	ON_WM_HELPINFO()
	ON_EN_CHANGE(IDC_CORE_PASSWORD, OnChangeCorePassword)
	ON_EN_CHANGE(IDC_APP_PASSWORD, OnChangeAppPassword)
	ON_EN_CHANGE(IDC_APP_OLD_PASSWORD, OnChangeAppOldPassword)
	ON_EN_CHANGE(IDC_CORE_OLD_PASSWORD, OnChangeCoreOldPassword)
	ON_EN_CHANGE(IDC_APP_PASSWORD2, OnChangeAppPassword2)
	ON_EN_CHANGE(IDC_CORE_PASSWORD2, OnChangeCorePassword2)
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_CHECK_COREENABLE, OnCheckCoreenable)
	ON_BN_CLICKED(IDC_CHECK_APPENABLE, OnCheckAppenable)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDBPasswords message handlers

BOOL CDBPasswords::OnHelpInfo(HELPINFO* pHelpInfo) 
{
	// TODO: Add your message handler code here and/or call default
	
	return CPropertyPage::OnHelpInfo(pHelpInfo);
}
	
BOOL CDBPasswords::OnInitDialog() 
{
		m_baccess=2;
		m_StrCorePassword = "";
		m_StrAppPassword = ""; 
		m_StrCoreOldPassword = ""; 
		m_StrAppOldPassword = "";
		m_StrConfirmAppPassword = ""; 
		m_StrCorePassword = ""; 
		m_StrConfirmCorePassword = "";
		m_StrMessageLbl="";
		CFont m_font;
		LOGFONT lf;
		memset ( &lf, 0, sizeof (lf));
		lf.lfHeight = 15;
		lf.lfWeight = 700;
		strcpy(lf.lfFaceName, "Arial");
		m_font.CreateFontIndirect (&lf);

		GetDlgItem(IDC_MESSAGE_LBL)->SetFont (&m_font);//Set the message font
		GetDlgItem(IDC_APP_USER_ID)->SetFont (&m_font);//Set the message font
		GetDlgItem(IDC_CORE_USER_ID)->SetFont (&m_font);//Set the message font

		// Get User IDs from DataServer.

		//To Get the username,old Passwords from tf.ini file
		int dbrcode= txdsapi_get_record((pBYTE)&dbpass, sizeof (DbPasswords),DB_PASSWORD_DATA, error_msg );
		if( dbrcode == PTEMSG_NOT_FOUND )
		{
			m_StrMessageLbl=error_msg;			
			return TRUE;
		}
		m_StrCoreUserFld=dbpass.core_user_id;
		m_StrAppUserFld=dbpass.app_user_id;
			
		CPropertyPage::OnInitDialog();	
		
		return TRUE;  

}

BOOL CDBPasswords::CheckDialogForCompleteness()
{
			//Get Passwords From tf.ini 
			int dbrcode= txdsapi_get_record((pBYTE)&dbpass, sizeof (DbPasswords),DB_PASSWORD_DATA, error_msg );
			if( dbrcode == PTEMSG_NOT_FOUND )
			{
				m_StrMessageLbl=error_msg;			
				return TRUE;
			}
			if(m_CntlCoreEnable.GetCheck()==TRUE)
			if( m_StrCoreOldPassword.IsEmpty())
			{
					m_StrMessageLbl="Please enter Core OLD Password";
					m_CntlCoreOldPassword.SetFocus();
					if (!UpdateData(FALSE)){
						AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
					}
						return FALSE;			
			}
			if(m_CntlCoreEnable.GetCheck()==TRUE)
			if( m_StrCorePassword.IsEmpty())
			{
					m_StrMessageLbl="Please enter Core Password";
					m_CntlCorePassword.SetFocus();
					if (!UpdateData(FALSE)){
					AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
					}
					return FALSE;
			}
			if(m_CntlCoreEnable.GetCheck()==TRUE)
			if( m_StrConfirmCorePassword.IsEmpty())
			{

					m_StrMessageLbl="Please Confirm Core Password";
					m_CntlCorePassword2.SetFocus();
					if (!UpdateData(FALSE)){
					AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
					}
					return FALSE;
			}
			if(m_CntlAppEnable.GetCheck()==TRUE)
			if( m_StrAppOldPassword.IsEmpty())
			{

					m_StrMessageLbl="Please enter APP OLDPassword";
					m_CntlAppoldPassword.SetFocus();
					if (!UpdateData(FALSE)){
					AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
					}
					return FALSE;	
			}			
			if(m_CntlAppEnable.GetCheck()==TRUE)
			if( m_StrAppPassword.IsEmpty())
			{

				m_StrMessageLbl="Please enter Application Password";	
				m_CntlAppPassword.SetFocus();
				if (!UpdateData(FALSE)){
					AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
				}
					return FALSE;
			}	
			if(m_CntlAppEnable.GetCheck()==TRUE)
			if( m_StrConfirmAppPassword.IsEmpty())
			{

				m_StrMessageLbl="Please Confirm Application Password";
				m_CntlAppPassword2.SetFocus();
				if (!UpdateData(FALSE))
				{
					AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
				}
					return FALSE;
			}
			
			if(m_CntlCoreEnable.GetCheck()==TRUE)
			if( m_StrCoreOldPassword != dbpass.core_old_password  )
			{
					m_StrMessageLbl="Your Core Old Password is incorrect.Please re-enter.";
					m_CntlCoreOldPassword.SetFocus();
					if (!UpdateData(FALSE)){
						AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
					}
						return FALSE;				
			  
			}
			if(m_CntlAppEnable.GetCheck()==TRUE)
			if(m_StrAppOldPassword != dbpass.app_old_password )
			{
				m_StrMessageLbl="Your Application Old Password is incorrect.Please re-enter.";
					m_CntlAppoldPassword.SetFocus();
					if (!UpdateData(FALSE)){
					AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
					}
					return FALSE;	
			  
			}
			if(m_CntlCoreEnable.GetCheck()==TRUE)
			//Checking Password Length
			if(m_StrCorePassword.GetLength()<8)
			{
				m_StrMessageLbl="Please enter Core New Password more than 8 characters";
				m_CntlCorePassword.SetFocus();
				if (!UpdateData(FALSE)){
				AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
				}
				return FALSE;			
			}	
			if(m_CntlAppEnable.GetCheck()==TRUE)
 			if(m_StrAppPassword.GetLength()<8)
			{
				m_StrMessageLbl="Please enter App New Password more than 8 characters";
				m_CntlAppPassword.SetFocus();
				if (!UpdateData(FALSE)){
					AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
				}
					return FALSE;		
			}	
			if(m_CntlAppEnable.GetCheck()==TRUE)
			if( m_StrAppPassword != m_StrConfirmAppPassword)
			{
	
				m_StrMessageLbl="App Passwords Confirmation does not match. Please re-enter.";
				m_CntlAppPassword.SetFocus();
				if (!UpdateData(FALSE)){
					AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
				}
					return FALSE;
			  
			}
			if(m_CntlCoreEnable.GetCheck()==TRUE)
			if(m_StrCorePassword != m_StrConfirmCorePassword )
			{		
				m_StrMessageLbl="Core Passwords Confirmation does not match. Please re-enter.";
					m_CntlCorePassword.SetFocus();
					if (!UpdateData(FALSE)){
					AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
					}
					return FALSE;				  
			}
	return TRUE;
}
BOOL CDBPasswords::OnApply() 
{	
	if(m_StrCorePassword != "" || m_StrAppPassword != "" || m_StrCoreOldPassword != "" || m_StrAppOldPassword != "" ||  m_StrConfirmAppPassword != "" || m_StrConfirmCorePassword != "" )
	{
		while( 1 )
		{	
			if (!CheckDialogForCompleteness())
			return FALSE;
			
			//memset(&dbpass, 0, sizeof(dbpass)) ;
			if(m_CntlCoreEnable.GetCheck()==TRUE)
			strcpy( (char *)dbpass.core_password, m_StrCorePassword);
			if(m_CntlAppEnable.GetCheck()!=TRUE)
			strcpy( (char *)dbpass.app_password , (char *)dbpass.app_old_password);
			if(m_CntlAppEnable.GetCheck()==TRUE)
			strcpy( (char *)dbpass.app_password , m_StrAppPassword);
			if(m_CntlCoreEnable.GetCheck()!=TRUE)
			strcpy( (char *)dbpass.core_password , (char *)dbpass.core_old_password);
		
			//Update Passwords in dataserver and tf.ini file.
			int rcode = txdsapi_update_record((pBYTE)&dbpass, sizeof (DbPasswords),DB_PASSWORD_DATA, error_msg );
			if( rcode == PTEMSG_OK )
			{	//Set Message labels according to the updated passwords.
				if(m_CntlCoreEnable.GetCheck()==TRUE && m_CntlAppEnable.GetCheck()==FALSE)
				{	
					m_StrMessageLbl="Core Password Updated Successfullly...";
				}
				if(m_CntlCoreEnable.GetCheck()==FALSE && m_CntlAppEnable.GetCheck()==TRUE)
				{
					m_StrMessageLbl="Application Password Updated Successfullly...";
				}
				if(m_CntlCoreEnable.GetCheck()==TRUE && m_CntlAppEnable.GetCheck()==TRUE)				
				m_StrMessageLbl="Core And Application Passwords Updated Successfullly...";
			}
			else
			m_StrMessageLbl="Password not Updated";
			m_StrConfirmAppPassword="";
			m_StrConfirmCorePassword="";
			m_StrCorePassword="";
			m_StrAppPassword="";
			m_StrAppOldPassword="";
			m_StrCoreOldPassword="";
			SetModified(FALSE);
			break;
		}

	}
	else 
		return TRUE;
	return CPropertyPage::OnApply();
}

void CDBPasswords::OnOK() 
{		
	CPropertyPage::OnOK();
}

void CDBPasswords::OnCancel() 
{	
	CPropertyPage::OnCancel();
}

BOOL CDBPasswords::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	return CPropertyPage::OnCommand(wParam, lParam);
}

BOOL CDBPasswords::OnSetActive() 
{	
	if(m_baccess==2)	//Initally Enable both App and Core Password checkbox.
	{	
		m_CntlCoreEnable.SetCheck(1);
		m_CntlAppEnable.SetCheck(1);
		m_baccess=0;
	}

	return CPropertyPage::OnSetActive();
}

void CDBPasswords::OnChangeCorePassword() 
{
	SetModified(TRUE);
}

void CDBPasswords::OnChangeAppPassword() 
{
	SetModified(TRUE);
}

void CDBPasswords::OnChangeAppOldPassword() 
{
	SetModified(TRUE);	
}

void CDBPasswords::OnChangeCoreOldPassword() 
{
	SetModified(TRUE);		
}

void CDBPasswords::OnChangeAppPassword2() 
{
	SetModified(TRUE);	
	
}

void CDBPasswords::OnChangeCorePassword2() 
{
	SetModified(TRUE);		
}

void CDBPasswords::OnClose() 
{
	CPropertyPage::OnClose();
}

void CDBPasswords::OnCheckCoreenable() 
{	
	//Manipulate Check box operations
	if(m_CntlCoreEnable.GetCheck()!=FALSE)
	{
		m_CntlCoreOldPassword.EnableWindow(TRUE);
		m_CntlCorePassword2.EnableWindow(TRUE);
		m_CntlCorePassword.EnableWindow(TRUE);
	}
	else
	{
		m_CntlCoreOldPassword.EnableWindow(FALSE);
		m_CntlCorePassword2.EnableWindow(FALSE);
		m_CntlCorePassword.EnableWindow(FALSE);
	}
}
void CDBPasswords::OnCheckAppenable() 
{	
	//Manipulate Check box operations
	if(m_CntlAppEnable.GetCheck()!=FALSE)
	{
		m_CntlAppoldPassword.EnableWindow(TRUE);
		m_CntlAppPassword.EnableWindow(TRUE);
		m_CntlAppPassword2.EnableWindow(TRUE);
	}
	else
	{
		m_CntlAppoldPassword.EnableWindow(FALSE);
		m_CntlAppPassword.EnableWindow(FALSE);
		m_CntlAppPassword2.EnableWindow(FALSE);
	}
}
