
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
* $Log:   N:\PVCS\PTE\Equitable\ATP_gui\ChangePassword.cpp  $
   	 
		Rev 1.9   Jan 01 2011 11:30:22  -TF-Srikanth
    Added PCI changes -Password validations,locking user account 

	       Rev 1.8   Dec 30 2011 11:29:10   TF-Srikanth
   Added PCI GUI User Related Changes

      Rev 1.7   Dec 01 2003 11:29:10   lmitchel
   Added AMEX Online Updates - scr1200
   
      Rev 1.6   Mar 02 2000 11:56:22   iarustam
   SCR # 425,21,383,407,459,300,460,461
   
      Rev 1.5   Jan 05 2000 14:53:14   gbarnes
   Major update. Not updated to PVCS recently.
   PVCS was locked.
   
      Rev 1.1   Sep 22 1999 15:47:12   iarustam
   Added manual batch entry GUI
   
      Rev 1.0   Jul 01 1999 14:17:12   iarustam
   Initial Release
   
      Rev 1.0   Jun 28 1999 13:58:20   iarustam
   Initial Release
   
      Rev 1.2   May 10 1999 11:11:40   dperson
   Standardized error messages
   
      Rev 1.1   Apr 29 1999 14:56:10   iarustam
   Bug fixes
   
      Rev 1.0   Feb 23 1999 11:40:16   iarustam
   Initial Release
   
      Rev 1.0   Feb 17 1999 10:09:18   iarustam
   Initial Release
   
      Rev 1.1   Jan 22 1999 10:58:24   iarustam
   Bug fixes and core 1_3_0 changes
   
      Rev 1.1   Jan 21 1999 13:07:32   iarustam
   Bug fixes
   
   
      Rev 1.1   Dec 18 1998 13:43:00   iarustam
    
*
************************************************************************************/
// ChangePassword.cpp : implementation file
//

#include "stdafx.h"
#include "Atp.h"
#include "ChangePassword.h"
#include "ctype.h"
#include "SharedMem.h"
#include "common.h"
#include "login.h"

extern CSharedMem shrmem;
extern UCF01 ucf01;
extern char login_name[51];
extern GPCF01 gpcf01;
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/**temp_accountstatus declared in order to use it in the userdefinition.cpp file -TF_Veena**/
CHAR temp_accountstatus[10] = {0};
CLogin *pclogin;
/******************************************************************************************/
/////////////////////////////////////////////////////////////////////////////
// CChangePassword dialog


CChangePassword::CChangePassword(CWnd* pParent /*=NULL*/)
	: CDialog(CChangePassword::IDD, pParent)
{
	//{{AFX_DATA_INIT(CChangePassword)
	m_ConfirmPassword = _T("");
	m_UserLabel = _T("");
	m_OldPassword = _T("");
	m_StrNewPassword = _T("");
	//}}AFX_DATA_INIT
	passwordExpired = false; //set passwordExpired value on init
	bDontCheckOldPassword = false;
}


void CChangePassword::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CChangePassword)
	DDX_Control(pDX, IDC_STATIC_OLD_PWD, m_CntlOldPwdLbl);
	DDX_Control(pDX, IDC_CONFIRM_PASSWORD, m_CntlConfirmPassword);
	DDX_Control(pDX, IDC_ENTER_PASSWORD, m_CntlNewPassword);
	DDX_Control(pDX, IDC_OLD_PASSWORD, m_CntlOldPassword);
	DDX_Text(pDX, IDC_CONFIRM_PASSWORD, m_ConfirmPassword);
	DDV_MaxChars(pDX, m_ConfirmPassword, MAX_USERPWD_LEN);
	DDX_Text(pDX, IDC_USER_LABEL, m_UserLabel);
	DDX_Text(pDX, IDC_OLD_PASSWORD, m_OldPassword);
	DDV_MaxChars(pDX, m_OldPassword, MAX_USERNAME_LEN);
	DDX_Text(pDX, IDC_ENTER_PASSWORD, m_StrNewPassword);
	DDV_MaxChars(pDX, m_StrNewPassword, MAX_USERPWD_LEN);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CChangePassword, CDialog)
	//{{AFX_MSG_MAP(CChangePassword)
	ON_EN_CHANGE(IDC_OLD_PASSWORD, OnChangeOldPassword)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CChangePassword message handlers

void CChangePassword::OnOK() 
{	
	// TODO: Add extra validation here
	/*****TF_Veena*********/
	CHAR temp_connect_str[50] = { 0 };
	CHAR temp_name_str[50] = { 0 };
	char error_msg[256] = {0};
    int temp1=0;
	int len_New_Passwd=0;
	
	//BYTE rcode2;
	/**********************/
	// update data into the class variables
	UpdateData( TRUE );
	//Ramya as per BR

			//strcpy( login_name, pclogin->m_LoginName );
			//rcode2 = txdsapi_get_record( (pBYTE)&gpcf01,sizeof (GPCF01), GPCF01_DATA, error_msg );

	//Change Password Dialog Validations.
	if(!bDontCheckOldPassword)
	{
		if( m_OldPassword.IsEmpty())
		{
		  AfxMessageBox( "Please enter Old Password", MB_ICONSTOP | MB_OK );
		  m_CntlOldPassword.SetFocus();
		  return;
		}
		}
	if( m_StrNewPassword.IsEmpty() )
	{
	  AfxMessageBox( "Please enter New Password", MB_ICONSTOP | MB_OK );
	  m_CntlNewPassword.SetFocus();
	  return;
	}
	if( m_ConfirmPassword.IsEmpty() )
	{
	  AfxMessageBox( "Please Confirm New Password", MB_ICONSTOP | MB_OK );
	  m_CntlConfirmPassword.SetFocus();
	  return;
	}
	char * pBuffer = (char *)(LPCTSTR)m_StrNewPassword;

	//if( strlen( m_StrNewPassword ) < MIN_USERPWD_LEN)	//checking password length 
	if( strlen( m_StrNewPassword ) <  atoi((char *)gpcf01.min_password_length))	//checking password length 
	{
		char tempbuff[10];
		sprintf(tempbuff,"Password should not be less than %s characters",gpcf01.min_password_length);
		AfxMessageBox(tempbuff, MB_ICONSTOP | MB_OK );
		m_CntlNewPassword.SetFocus();
		if (!UpdateData(FALSE)){
		AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
		}					
	  return;
	}

	if((isnumeric(pBuffer)) == 0)	    //Check for numericals 
	{
		AfxMessageBox( "Your Password is not having numericals. Please re-enter. ", MB_ICONSTOP | MB_OK );
		m_CntlNewPassword.SetFocus();
		if (!UpdateData(FALSE)){
		AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
		}	
		return;
	}

	if((isalphabet(pBuffer)) == 0)	    //Check for characters
	{
		AfxMessageBox( "Your Password is not having characters. Please re-enter.  ", MB_ICONSTOP | MB_OK );
		m_CntlNewPassword.SetFocus();
		if (!UpdateData(FALSE)){
		AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
		}	
		return;
	}

	// checking New Passwords
	if( m_StrNewPassword != m_ConfirmPassword )
	{
	  AfxMessageBox( "Confirmation does not match. Please re-enter.", MB_ICONSTOP | MB_OK );
	  m_CntlNewPassword.SetFocus();
		if (!UpdateData(FALSE)){
		AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
		}	
	  return;
	}
	// Check for OLD 4 passwrds PHani
	/***********************************User ID cannot be nominated for password. *************/
	 temp_connect_str[strlen(temp_connect_str)] = 0; //Make sure string is terminated
	 temp1 = 0;
	
	 len_New_Passwd = strlen(m_StrNewPassword);
	 if(len_New_Passwd > 0)
	 {
		  strncpy(temp_connect_str,m_StrNewPassword,strlen(m_StrNewPassword)); //copying temp password into the temp var which will later convert to upper case.
	 }
	 strcpy(temp_connect_str,m_StrNewPassword); //copying temp password into the temp var which will later convert to upper case.
	 strcpy(temp_name_str,m_UserLabel);
	 while ( 0 != temp_connect_str[temp1] )
		temp_connect_str[temp1++] = toupper(temp_connect_str[temp1]); // Convert string to upper case

	//if( strstr( (char *)ucf01.primary_key.name,temp_connect_str ) != NULL )
	 if( strspn(temp_name_str , temp_connect_str ) != NULL )
	{
		AfxMessageBox( "Password should not contain any similarity to Username", MB_ICONSTOP | MB_OK );
		return;
	}

	 /************when an user password is updated,Accountstatus(ucf01 table) is set to "new" which will***
	***enforce the the user to change the password - TF_Veena*********************************************/
	 if(strcmp((char *)ucf01.primary_key.name,m_UserLabel) != 0)
	 {
		//strcpy((char*)ucf01.accountstatus,USER_NEW);
		 memset((char *)temp_accountstatus, 0 ,sizeof(temp_accountstatus));
		 strcpy((char *)temp_accountstatus,USER_NEW);
	 }
		 
	/***************************************************************************************/

	CDialog::OnOK();
}



BOOL CChangePassword::OnInitDialog() 
{ 
	CDialog::OnInitDialog();

	// TODO: Add extra initialization here

	//display the user name
	UpdateData( FALSE );
	//Check for password expired value
	if(passwordExpired)  
	{
		   SetDlgItemText(IDC_STATIC1,"Your Password Has Been Expired");
	}
	else
	{
	   SetDlgItemText(IDC_STATIC1,"");
	}
	if(bDontCheckOldPassword)
	{
		m_CntlOldPwdLbl.ShowWindow(SW_HIDE);
		m_CntlOldPassword.ShowWindow(SW_HIDE);
	}
	else
	{
				m_CntlOldPwdLbl.ShowWindow(SW_SHOW);
		m_CntlOldPassword.ShowWindow(SW_SHOW);

	}
	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}

void CChangePassword::OnChangeOldPassword() 
{	
	UpdateData();
	m_OldPassword.Replace(" ","");
	UpdateData( FALSE );
	
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	
}

BOOL CChangePassword::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	// TODO: Add your specialized code here and/or call the base class

	return CDialog::OnCommand(wParam, lParam);
}

BOOL CChangePassword::isSpace(pCHAR string)
{
   INT i;
   INT length;

   length = (INT)strlen(string);

   for (i = 0; i < length; i++)
   {      
       if( string[i] == ' ')
           return TRUE;      
   }
   return FALSE;
}
BOOL CChangePassword::isalphabet(pCHAR string)
{
   INT i;
   INT length;

   length = (INT)strlen(string);

   for (i = 0; i < length; i++)
   {
      if (isalpha(string[i]))
        return TRUE;
   }
   return FALSE;
}
BOOL CChangePassword::isnumeric(pCHAR string)
{
   INT i;
   INT length;

   length = (INT)strlen(string);

   for (i = 0; i < length; i++)
   {
      if (isdigit(string[i]))
        return TRUE;
   }
   return FALSE;
}
