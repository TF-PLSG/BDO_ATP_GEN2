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
* $Log:   N:\PVCS\PTE\Equitable\ATP_gui\LOGIN.CPP  $

  Rev 1.6   Dec 01 2003 11:29:28   lmitchel
  Added AMEX Online Updates - scr1200
  
	Rev 1.5   Mar 02 2000 11:55:20   iarustam
	SCR # 425,21,383,407,459,300,460,461
	
      Rev 1.4   Jan 05 2000 14:53:40   gbarnes
	  Major update. Not updated to PVCS recently.
	  PVCS was locked.
	  
		Rev 1.1   Sep 22 1999 15:50:42   iarustam
		Added manual batch entry GUI
		
		  Rev 1.0   Jul 01 1999 14:17:34   iarustam
		  Initial Release
		  
			Rev 1.0   Jun 28 1999 13:58:38   iarustam
			Initial Release
			
			  Rev 1.1   Apr 29 1999 14:56:26   iarustam
			  Bug fixes
			  
				Rev 1.0   Feb 23 1999 11:40:32   iarustam
				Initial Release
				
				  Rev 1.0   Feb 17 1999 10:09:30   iarustam
				  Initial Release
				  
					Rev 1.1   Jan 22 1999 10:58:56   iarustam
					Bug fixes and core 1_3_0 changes
					
					  Rev 1.1   Jan 21 1999 13:08:12   iarustam
					  Bug fixes
					  
						
						  Rev 1.1   Dec 18 1998 13:41:58   iarustam
						  
							*
************************************************************************************/
// Login.cpp : implementation file
//

#include "stdafx.h"
#include "myutils.h"
#include "Atp.h"
#include "Login.h"
#include "changepassword.h"
#include "SharedMem.h"
#include "common.h"
#include "ptepci.h"
#include "UserAccountStatus.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CString workstation;
extern UCF01 ucf01;
extern ATP02 atp02;
extern "C"
{
#include "ptetcp.h"
#include "ntutils.h"
#include "Txutils.h"
#include "PteTime.h"
	
	
}
extern WORD key[];
CHAR wstation[21];
DWORD wsize = sizeof(wstation);
extern CUserAccountStatus* pAccountStatusPage; 
GPCF01 gpcf01;
#define ADMIN_NO_VIEW "1110"
#define ADMIN_USER "1111"
#define DAYS_BEFORE_EXPIRY "45"
#define LEN_of_ADMINISTRATOR 13
#define LEN_YYYYMMDD 8
#define LEN_YYYYMMDDHHMM 12
#define LEN_HHMM 4
/////////////////////////////////////////////////////////////////////////////
// CLogin dialog

CLogin login;
CString idle_time_val_str;
CChangePassword ChangePass;
char login_name[51];
char New_Logout_Date1[15];
char Prev_Logout_Date1[15];
char TODAYDate[20];

CHAR temp_passwd[51] = {0};
CHAR temp_passwd1[51] = {0};
int loginCount=0;
int maxpassordcount=0;

CLogin::CLogin(CWnd* pParent /*=NULL*/)
: CDialog(CLogin::IDD, pParent)
{
	//{{AFX_DATA_INIT(CLogin)
	m_LoginName = _T("");
	m_Password = _T("");
	m_ChangePassword = FALSE;
	m_bDisableNameFld = FALSE;
	//}}AFX_DATA_INIT

#ifdef _DEBUG
	m_LoginName = _T("ADMINISTRATOR");
	m_Password = _T("ADMINISTRATOR1234");
		//00000000000000000000000199770852
	
#endif
}


void CLogin::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CLogin)
	DDX_Control(pDX, IDC_LOGIN_NAME, m_CntlLoginName);
	DDX_Text(pDX, IDC_LOGIN_NAME, m_LoginName);
	//ramya
	//DDX_Control(pDX, IDC_DAYS_INACTIVE, m_CntlDaysInactive);
	//DDX_Text(pDX, IDC_DAYS_INACTIVE, m_DaysInactive);

	DDV_MaxChars(pDX, m_LoginName, MAX_USERNAME_LEN);
	DDX_Text(pDX, IDC_PASSWORD, m_Password);
	DDV_MaxChars(pDX, m_Password, MAX_USERPWD_LEN);
	DDX_Check(pDX, IDC_CHPASSWORD_CHECK, m_ChangePassword);
	DDX_Check(pDX, IDC_RESET_LOGOUT_CHECKBOX, m_Resetlogout);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CLogin, CDialog)
//{{AFX_MSG_MAP(CLogin)
//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDOK, &CLogin::OnBnClickedOk)
	ON_EN_CHANGE(IDC_PASSWORD, &CLogin::OnEnChangePassword)
	ON_EN_CHANGE(IDC_LOGIN_NAME, &CLogin::OnEnChangeLoginName)
	ON_BN_CLICKED(IDC_RESET_LOGOUT_CHECKBOX, &CLogin::OnBnClickedResetLogoutCheckbox)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CLogin message handlers
//Krishna, 27.4.11: Made Login Verification as a function, to reuse it in different clases
short CLogin::CheckLoginDetails()
{
	
	BOOL bFound;
	int loginCount=0;
	BYTE rcode_UCF01,rcode_GPCF01;
	CString accountstatus="";
	CHAR ini_file[256];
	char tmpbuff[256]={'\0'};
	CHAR error_msg[256];
	char password_retry_count[2] = {0};
	int int_password_retry_count = 0;
	char min_password_len[2]={0};
	int int_min_password_len = 0;
	int flag = 1;

	int password_attempts_left = 0;
	
	CString password_buf = m_Password;

	/*****TF_Veena*********/

    int temp1=0;
	
	int pw_len = 0;
	/**********************/
	
	GetAscendentConfigDirectory( ini_file );
	strcat( ini_file, "tf.ini" );
	
	(void) GetPrivateProfileString(
        "GUI", 	                   // points to section name 
        "IDLE_TIMEOUT_VALUE",	   // points to key name 
        "600",					   // points to default string 
        idle_time_val,             // points to destination buffer 
        sizeof(idle_time_val) - 1, // size of destination buffer 
        ini_file );                // points to initialization filename 
	
	idle_time_val[sizeof(idle_time_val)] = 0; //Make sure string is terminated
	
	idle_time_val_str = idle_time_val;		
	
	while( 1 )
	{
			CTime theTime;		
			int year,month,day,hour,min;	
			char yyyy[10] ={0}, mm[5] = {0}, dd[5] = {0};
			long jday1,jday2, jday3, jday4,jlogin,jtodayforDBE;
			char modifiedDate[10]={0},toDayDate[20]={0}, prevLoginDate[10]={0}, ExpiryDate[10]={0}, hhmm[10]={0}, ExpiryDateforDBE[20] = {0};
			char toDayLDate1[20] = {0};
			char toDayDateforDBE[20] = {0};
			int days_left;
			char tb1[256] = {0};
			int len_m_Password, len_temp_passwd, len_m_LoginName = 0;
			int old_pw_length = 0;

			
			
			

			//char Prev_Login_Date[20]=""; char Prev_Logout_Date[20]="";

			theTime = CTime::GetCurrentTime();//Get Todays date
			year= 	theTime.GetYear ();
			month = theTime.GetMonth();
			day = theTime.GetDay ();
			hour =theTime.GetHour();
			min = theTime.GetMinute();

		

			/**** Encrypt the LOGIN password  - Start ****/
			{
				m_Password = password_buf;
			}
			/**** Encrypt the password  - End ****/

		memset( &ucf01, 0, sizeof( UCF01 ) );
		int ret = DoModal();
		password_buf = m_Password;
		
		if( ret == IDOK )
		{
			len_m_LoginName=strlen(m_LoginName);
			// next authenticate the user
			if(len_m_LoginName > 0)
			{
				strncpy( (char *)ucf01.primary_key.name, m_LoginName, len_m_LoginName); 
				strncpy( login_name, m_LoginName, len_m_LoginName );
			}

			rcode_UCF01 = txdsapi_get_record( (pBYTE)&ucf01,sizeof (UCF01), UCF01_DATA, error_msg );
			if(strcmp((LPTSTR)(ucf01.User_Status),"DELETED")==0)
			{
				AfxMessageBox( "Deleted user can not Change the Password. Login Denied.", MB_ICONSTOP | MB_OK );
				return FALSE;
			}

			if(rcode_UCF01 == PTEMSG_NOT_FOUND )
			{
				AfxMessageBox( "User not found or DataBase of User was not read.  Login denied.", MB_ICONSTOP | MB_OK );
				return FALSE;
			}

			else if(rcode_UCF01 == PTEMSG_IPC_ERROR)
			{
				AfxMessageBox( "Failed to retrieve Data", MB_ICONSTOP | MB_OK );
				m_StrMessageLbl = error_msg;
				AfxMessageBox (m_StrMessageLbl, MB_ICONSTOP);
				return FALSE;
			}

			else if( rcode_UCF01 != PTEMSG_OK )
			{
				m_StrMessageLbl = error_msg;
				AfxMessageBox (m_StrMessageLbl, MB_ICONSTOP);		
				return FALSE;
			}	

			//ramya
			rcode_GPCF01 = txdsapi_get_record( (pBYTE)&gpcf01,sizeof (GPCF01), GPCF01_DATA, error_msg );
			
			if(rcode_GPCF01 == PTEMSG_NOT_FOUND )
			{
				AfxMessageBox( "DataBase of SystemGlobal Parameters was not read.", MB_ICONSTOP | MB_OK );
				return FALSE;
			}

			else if(rcode_UCF01 == PTEMSG_IPC_ERROR)
			{
				AfxMessageBox( "Failed to retrieve Data", MB_ICONSTOP | MB_OK );
				m_StrMessageLbl = error_msg;
				AfxMessageBox (m_StrMessageLbl, MB_ICONSTOP);
     			return FALSE;
			}

			else if ( rcode_GPCF01 != PTEMSG_OK )
			{   
				m_StrMessageLbl = error_msg;
				AfxMessageBox (m_StrMessageLbl, MB_ICONSTOP);		
				return FALSE;
			}

			strncpy((char *)ucf01.Days_Inactive,(char *)gpcf01.days_inactive,sizeof(gpcf01.days_inactive));
			
			Prev_Login_Date = ucf01.Login_Date;
			Prev_Logout_Date = ucf01.Logout_Date;
			strncpy(Prev_Logout_Date1,Prev_Logout_Date,LEN_YYYYMMDDHHMM);
	
			strncpy(prevLoginDate,(char *)ucf01.Login_Date, LEN_YYYYMMDD);
			strncpy(modifiedDate,(char*)ucf01.password_modified_date,LEN_YYYYMMDD);//Store passwordmodified date
			strncpy(ExpiryDate,(char*)ucf01.User_Expiry_Date,LEN_YYYYMMDD);
			CString CSExpiryDate = ExpiryDate;
			//strcpy(ExpiryDate,pAccountStatusPage->m_StrUserExpiryDate);

			sprintf(toDayDate,"%d%02d%02d",year,month,day);//converting toDayDate to yyyymmdd format
			
			strncpy(TODAYDate,toDayDate,8);

			//int DI = atol((char *)ucf01.Days_Inactive);
			jday1=Txutils_Calculate_Julian_Date((unsigned char*)modifiedDate);//calculate juliandate for modified date
			jday2=Txutils_Calculate_Julian_Date((unsigned char*)toDayDate);//calculate juliandate for TodaysDate
			jday3=Txutils_Calculate_Julian_Date((unsigned char*)prevLoginDate);
			jday4=Txutils_Calculate_Julian_Date((unsigned char*)ExpiryDate);
			
			login.m_LoginName = m_LoginName;
			login.m_Password = m_Password;
			len_m_Password = strlen(m_Password);

			if(len_m_Password>0)
			{
				memset(temp_passwd,0x00,sizeof(temp_passwd));
				strncpy(temp_passwd, m_Password,len_m_Password);
				len_temp_passwd=strlen(temp_passwd);

				if(len_temp_passwd>0)
				{
					strncpy(temp_passwd1, temp_passwd,len_temp_passwd);
				}
				else
					return FALSE;
			}
			else
			{
				AfxMessageBox( "Password length is zero/Password not fectched from DB", MB_ICONSTOP | MB_OK );	
				return FALSE;
			}

			int ret1 = password_check ();

			if(ret1 == 0)
			{
				strncpy(password_retry_count,(char *)gpcf01.max_password_retries,strlen((char *)gpcf01.max_password_retries));
				int_password_retry_count = atoi(password_retry_count);
				if(maxpassordcount >= int_password_retry_count)
				{
					if(strncmp(login_name,"ADMINISTRATOR",LEN_of_ADMINISTRATOR) == 0 )
					{
						AfxMessageBox( "Exceeded Max Password retries. Please Enter a Valid Password", MB_ICONSTOP | MB_OK );	
						continue;
					}
					else
					{
						accountstatus=USER_LOCKED;
						strcpy( (char *)ucf01.accountstatus , accountstatus);
						rcode_UCF01 = txdsapi_update_record( (pBYTE)&ucf01, sizeof (UCF01), UCF01_DATA, error_msg );
						if( rcode_UCF01 == PTEMSG_OK )
						{
							AfxMessageBox( "Exceeded Max Password retries. Your User id is disabled/inactive to run ATP. Please contact your System Administrator.", MB_ICONSTOP | MB_OK );
							continue;
						}
						else
							return FALSE;
					}

				}

				if(m_LoginName.GetLength() < MIN_USERNAME_LEN)
				{
					loginCount++;  /** To disable 3 unsuccesfull attempts - TF_Veena **/
					memset(tmpbuff,0x00,sizeof(tmpbuff));
					sprintf(tmpbuff, "User Name length is less than %d chars", MIN_USERNAME_LEN);
					AfxMessageBox(tmpbuff);
					continue;
					//return FALSE;
				}
			//if(m_Password.GetLength() < MIN_USERPWD_LEN)
			//change as per BR, ramya

				strncpy(min_password_len,(char *)gpcf01.min_password_length,strlen((char *)gpcf01.min_password_length));
				int_min_password_len = atoi(min_password_len);
				if(m_Password.GetLength() < int_min_password_len)
				{
					maxpassordcount++;
					loginCount++; /** To disable 3 unsuccesfull attempts - TF_Veena **/
					//sprintf(tmpbuff, "User Password length is less than %d chars", MIN_USERPWD_LEN);
					memset(tmpbuff,0x00,sizeof(tmpbuff));
					sprintf(tmpbuff, "User Password length is less than %s chars",gpcf01.min_password_length);
					AfxMessageBox(tmpbuff);
					continue;
					//return FALSE;
				}
				if(!ChangePass.isalphabet(temp_passwd1))
				{
					maxpassordcount++;
					loginCount++; /** To disable 3 unsuccesfull attempts - TF_Veena **/
					AfxMessageBox("User Password does not contain any Alphabets");
					continue;
					//return FALSE;
				}
				if(!ChangePass.isnumeric(temp_passwd1))
				{
					maxpassordcount++;
					loginCount++; /** To disable 3 unsuccesfull attempts - TF_Veena **/
					AfxMessageBox("User Password does not contain any Numbers");
					continue;
					//return FALSE;
				}
				maxpassordcount++;
				password_attempts_left = int_password_retry_count - maxpassordcount;
				memset(tmpbuff,0x00,sizeof(tmpbuff));
				if(password_attempts_left <= 1)
				{
					sprintf(tmpbuff, "Invalid Username/Password. Login denied. %d Retry left",password_attempts_left);
					flag = 0;
				}
				if(flag == 1 )
				{
					sprintf(tmpbuff, "Invalid Username/Password. Login denied. %d Retries left",password_attempts_left);
				}
				AfxMessageBox( tmpbuff, MB_ICONSTOP | MB_OK );
				continue;
				
				
			}


			/*********************************Ramya Added**********************************/
			//To Check If the User has already Logged in and to Log Out/Reset if he has to Log IN again from a different WorkStation
			//Added two fields, WorkStation ID through which User has logged in & LoginStatus being Set as 1 to indicate he has logged in
			//On enabling the Reset/Logout CheckBOx, User logs out from prev WorkStation

			if((ucf01.Login_Status[0]=='1') && m_Resetlogout!=1)
			{
				sprintf(tmpbuff,"User has already logged in at workstation %s. Please Reset/Logout", ucf01.Login_WorkStationId);
				AfxMessageBox(tmpbuff);
				continue;
			}



			//if(jday2 - jday3 > atoi((char *)ucf01.Days_Inactive) && (atoi(prevLoginDate)) != NULL && atoi((char *)ucf01.Days_Inactive)!=NULL)
			if (!(CSExpiryDate.IsEmpty()))
			{
				if(jday4 - jday2 < 0)
				{
					
						int re = ExpiryDetails();
							//if(!(strncmp((char *)ucf01.control[0].action,ADMIN_NO_VIEW,4))||!(strncmp((char *)ucf01.control[0].action,ADMIN_USER,4))) login_name
						if(strncmp(login_name,"ADMINISTRATOR",LEN_of_ADMINISTRATOR) == 0 )
						{
							if (re == 1)
							{
								//return TRUE;
							}
							//else
							//{
							//	return false;
							//}
						}
						else
						{
							return false;
						}
					
				}
			}

			
			//else if (!(CSExpiryDate.IsEmpty()))
			if (jday2 - jday3 > atol((char *)(ucf01.Days_Inactive)) && (atol(prevLoginDate)) != NULL && atol((char *)ucf01.Days_Inactive)!=NULL)
			{
					int ri = InactiveDetails();
					
					//if(!(strncmp((char *)ucf01.control[0].action,ADMIN_NO_VIEW,4))||!(strncmp((char *)ucf01.control[0].action,ADMIN_USER,4)))
					if(strncmp(login_name,"ADMINISTRATOR",LEN_of_ADMINISTRATOR) == 0 )
					{
						if (ri == 1)
						{
							//return TRUE;
						}
						//else
						//{
						//	return false;
						//}
					}
					else
					{
						return false;
					}
			}

		
			if( strcmp( (char *)ucf01.User_Status, STATUS_INACTIVE ) == 0 || strcmp( (char *)ucf01.User_Status, STATUS_EXPIRED ) == 0)
			{
				if(strncmp(login_name,"ADMINISTRATOR",LEN_of_ADMINISTRATOR) == 0 )
				{
					//return TRUE;
				}
				else
				{
					AfxMessageBox( "Your User id is Deactivated. Please contact your System Administrator.", MB_ICONSTOP | MB_OK );
					return FALSE;
				}
			}

					strncpy((char *)ucf01.Login_Date,toDayDate, LEN_YYYYMMDD);
					sprintf(hhmm, "%02d%02d",hour,min);
					strncat(toDayDate,hhmm,LEN_HHMM);
					strncpy((char *)ucf01.Login_Date,toDayDate,LEN_YYYYMMDDHHMM);

			if(m_Resetlogout==1)
			{
				if ((jday2 - jday3 < atol((char *)(ucf01.Days_Inactive)) || atoi((char *)ucf01.Days_Inactive)==NULL  || ((atol(prevLoginDate)) == NULL) )&&(jday4 - jday2 > 0))
				{
					strncpy((char *)ucf01.Login_WorkStationId,"0",wsize);
					strncpy((char *)ucf01.Login_Status,"0",LOGIN_STATUS_LEN);
					ucf01.Login_Status[1] = '\0';
					strncpy((char *)ucf01.Logout_Date,toDayDate,LEN_YYYYMMDDHHMM);
					New_Logout_Date = ucf01.Logout_Date;
					strncpy(New_Logout_Date1,New_Logout_Date,LEN_YYYYMMDDHHMM);
				}
				if (jday2 - jday3 >= atol((char *)(ucf01.Days_Inactive)) && (atoi(prevLoginDate)) != NULL && atoi((char *)ucf01.Days_Inactive)!=NULL)
				{
					int ri = InactiveDetails();
					
					//if(!(strncmp((char *)ucf01.control[0].action,ADMIN_NO_VIEW,4))||!(strncmp((char *)ucf01.control[0].action,ADMIN_USER,4)))
					if(strncmp(login_name,"ADMINISTRATOR",LEN_of_ADMINISTRATOR) == 0 )
					{
						if (ri == 1)
						{
							strncpy((char *)ucf01.Logout_Date,toDayDate,LEN_YYYYMMDDHHMM);
							New_Logout_Date = ucf01.Logout_Date;
							strncpy(New_Logout_Date1,New_Logout_Date,LEN_YYYYMMDDHHMM);
							//return TRUE;
						}
						//else
						//{
						//	return false;
						//}
					}
					else
					{
						return false;
					}
					
				}
				if (!(CSExpiryDate.IsEmpty()))
				{
					 if (jday4 - jday2 <= 0)
					 {
						 int re = ExpiryDetails();

						//if(!(strncmp((char *)ucf01.control[0].action,ADMIN_NO_VIEW,4))||!(strncmp((char *)ucf01.control[0].action,ADMIN_USER,4)))
						 if(strncmp(login_name,"ADMINISTRATOR",LEN_of_ADMINISTRATOR) == 0 )
						{
							if (re == 1)
							{
								strncpy((char *)ucf01.Logout_Date,toDayDate,LEN_YYYYMMDDHHMM);
								New_Logout_Date = ucf01.Logout_Date;
								strncpy(New_Logout_Date1,New_Logout_Date,LEN_YYYYMMDDHHMM);
								//return TRUE;
							}
							/*else
							{
								return false;
							}*/
						}
						else
						{
							return false;
						}
						 
					}

				}
			}

			/************************************************************************************************************/



			/******************Days b4 Expiration Ramya*******************/

			//strncpy(toDayLDate1,(char *)ucf01.Login_Date,8);
			strncpy(toDayDateforDBE,toDayDate,LEN_YYYYMMDD);
			//jlogin = Txutils_Calculate_Julian_Date((unsigned char*)toDayLDate1);
			jtodayforDBE =  Txutils_Calculate_Julian_Date((unsigned char*)toDayDateforDBE);

			
	if (!(CSExpiryDate.IsEmpty()))
	{
			if(jday4 - jtodayforDBE > 0)
			{
				days_left = jday4 - jtodayforDBE;
				strncpy(yyyy,ExpiryDate,4);
				strncpy(mm,&(ExpiryDate[4]),2);
				strncpy(dd,&(ExpiryDate[6]),2);

				strncpy((char *)ExpiryDateforDBE,dd,2);
				strncat((char *)ExpiryDateforDBE,"/",1);
				strncpy(&(ExpiryDateforDBE[3]),mm,2);
				strncpy(&(ExpiryDateforDBE[5]),"/",1);
				strncpy(&(ExpiryDateforDBE[6]),yyyy,4);
				

				if((strcmp((char *)gpcf01.days_before_expiration,""))==0)
				{
					if(jday4 - jtodayforDBE <= atol(DAYS_BEFORE_EXPIRY) )	
					{
						memset(tb1,0x00,sizeof(tb1));
						sprintf(tb1,"Your account will expire in %d days. Please request for extension before the expiry date %s",days_left,ExpiryDateforDBE);
						AfxMessageBox( tb1, MB_ICONSTOP | MB_OK );
					}
				}
				else
				{
					if(jday4 - jtodayforDBE <= atol((char *)gpcf01.days_before_expiration))	
					{
						memset(tb1,0x00,sizeof(tb1));
						sprintf(tb1,"Your account will expire in %d days. Please request for extension before the expiry date %s",days_left,ExpiryDateforDBE);
						AfxMessageBox( tb1, MB_ICONSTOP | MB_OK );
					}
				}

			}
		}

			/************************************************************/

			//Checking the User accountstatus -TF-Srikanth


			if( strcmp( (char *)ucf01.accountstatus, USER_LOCKED ) == 0 )
			{
				if(strncmp(login_name,"ADMINISTRATOR",strlen("ADMINISTRATOR")) == 0 )
				{
					// do nothing

	
				}
				else
				{
					AfxMessageBox( "Your User id is disabled/inactive to run ATP. Please contact your System Administrator.", MB_ICONSTOP | MB_OK );
					return FALSE;
				}
			}

			LogEvent( "Password check success", INFO_MSG );
			bFound = FALSE;
			
			for ( int i = 0; i < 10; i++ )
			{ 
				if ( strcmp ((char *)ucf01.login[i].terminals, workstation) == 0)
					bFound = TRUE;
			}
			
			if ( !bFound && (strcmp ((char *)ucf01.login[0].terminals, "") != 0) )
			{
				AfxMessageBox( "This user is not authorized to login from this workstation. \nPlease contact your System Administrator.", MB_ICONSTOP | MB_OK );
				return FALSE;		
			}
			
			//Check for Password expiry
			
			
			if (jday2 - jday3 < atoi((char *)(ucf01.Days_Inactive))|| atoi((char *)ucf01.Days_Inactive)==NULL || ((atoi(prevLoginDate)) == NULL) )
			{
			sprintf(hhmm, "%02d%02d",hour,min);
			strncat(toDayDate,hhmm,LEN_HHMM);
			strncpy((char *)ucf01.Login_Date,toDayDate,LEN_YYYYMMDDHHMM);
			strncpy(modifiedDate,(char*)ucf01.password_modified_date,LEN_YYYYMMDD);//Store passwordmodified date
			New_Login_Date = ucf01.Login_Date;

			
			PopulateLoginControlStructure();
			PopulateHistoryTable();
			PopulateHistoryTable();
		
			
			//checking password changed difference
			int pass_exp_days;
			char pass_days[10];
			(void) GetPrivateProfileString(
				"GUI", 	                   // points to section name 
				"USER_PASSWORDEXPIRY_DAYS",	           // points to key name 
				"90",	               // points to default string 
				pass_days,               // points to destination buffer 
				sizeof(pass_days) - 1,   // size of destination buffer 
				ini_file );                // points to initialization filename 
			
			pass_days[strlen(pass_days)] = 0; //Make sure string is terminated
			
			pass_exp_days = atoi(pass_days);
			
			if(pass_exp_days > 90)
			{
				sprintf(tmpbuff, "Illegal Password Expiry period was set to %d Days. Using 90 as Password Expiry period", pass_exp_days);
				LogEvent(tmpbuff, INFO_MSG);
				
				pass_exp_days = 90;
			}		
			
			idle_time = atoi(idle_time_val);
			
			if((jday2-jday1)>=pass_exp_days)
			{
				int len, old_pw_length, new_pw_length =0;
				LogEvent( "in change passwordb", INFO_MSG );
				ChangePass.m_UserLabel = (char *)ucf01.primary_key.name;
				ChangePass.passwordExpired = true;//enable passwordExpired label value 
				ret = ChangePass.DoModal();
				if( ret == IDOK )
				{
					BYTE rcode1; char tmpbuff[256] = {0};
					//strcpy( login_name, m_LoginName );
					//rcode1 = txdsapi_get_record( (pBYTE)&gpcf01,sizeof (GPCF01), GPCF01_DATA, error_msg );
					
					memset(temp_passwd,0x00,sizeof(temp_passwd));
					old_pw_length = strlen(ChangePass.m_OldPassword);

					if(old_pw_length > 0)
					{
						strncpy( temp_passwd, ChangePass.m_OldPassword ,old_pw_length );
						/**** Encrypt the EXPIRED password  - Start ****/
						{
							 len = strlen(temp_passwd);
							if(len <= MAX_USERPWD_LEN)
									gui_password_encryption((BYTE *)temp_passwd,len);
						}
					}
					/**** Encrypt the password  - End ****/
					
					len = strlen(temp_passwd);
					// check password
					if( strncmp( (char *)ucf01.password, temp_passwd,len) != 0 )
					{		
						AfxMessageBox( "Invalid Password .", MB_ICONSTOP | MB_OK );
						continue;
					}
					new_pw_length = strlen(ChangePass.m_StrNewPassword);
					
					if(new_pw_length>0)
					{
						memset(temp_passwd,0x00,sizeof(temp_passwd));
						strncpy( temp_passwd, ChangePass.m_StrNewPassword,len);
						/**** Encrypt the EXPIRED NEW password  - Start ****/
						{
							 len = strlen(temp_passwd);
							if(len <= MAX_USERPWD_LEN)
									gui_password_encryption((BYTE *)temp_passwd,len);
						}
					}
					/**** Encrypt the password  - End ****/

					temp1 = Copy_previous_passwords(&ucf01);

					if(temp1 == 1)
					{
						continue;
					}


					strncpy((char *)ucf01.password_modified_date, toDayDate, LEN_YYYYMMDD);
					
					rcode_UCF01 = txdsapi_update_record( (pBYTE)&ucf01, sizeof (UCF01), UCF01_DATA, error_msg );
					if( rcode_UCF01 == PTEMSG_OK)
						AfxMessageBox( "Your password has been updated", MB_ICONINFORMATION | MB_OK );
					else
						AfxMessageBox( error_msg, MB_ICONSTOP | MB_OK );
				}			
				else
				{
					return FALSE; // password expired. , 
				}
				//break;
			}
			else
			{
				LogEvent( "Terminal check success", INFO_MSG );

				/*******************************If the User is entering the application for the time.
				**We are setting the accountstatus (ucf01) as "new". If the user is logged in for first
				time then set the m_ChangePassword as "1"**********************************************/
				if(strcmp((char *)ucf01.accountstatus,USER_NEW) == 0)
				{
					m_ChangePassword = 1;
				}
				/*************************************************************************************/
				// password ok, check if user wants to change password
				if( m_ChangePassword )
				{				
					int len = 0;
					LogEvent( "in change passwordb", INFO_MSG );
					ChangePass.m_UserLabel = (char *)ucf01.primary_key.name;
					ret = ChangePass.DoModal();

					if( ret == IDOK )
					{
						//rcode1 = txdsapi_get_record( (pBYTE)&gpcf01,sizeof (GPCF01), GPCF01_DATA, error_msg );
						
						old_pw_length = strlen(ChangePass.m_OldPassword);
						len = strlen(temp_passwd);
						memset(temp_passwd,0x00,sizeof(temp_passwd));
						if(old_pw_length>0)
						{
							strncpy( temp_passwd, ChangePass.m_OldPassword ,old_pw_length);
						}
						/**** Encrypt the CHANGE OLD password  - Start ****/
						{
							len = strlen(temp_passwd);
							if(len <= MAX_USERPWD_LEN)
						
								gui_password_encryption((BYTE *)temp_passwd,len);
						}
						/**** Encrypt the password  - End ****/
						len = strlen(temp_passwd);

						// check password
						if( strncmp( (char *)ucf01.password, temp_passwd,len) != 0 )
						{		
							AfxMessageBox( "Invalid Password .", MB_ICONSTOP | MB_OK );
							continue;
						}
						memset(temp_passwd,0x00,sizeof(temp_passwd));
						len = strlen(ChangePass.m_StrNewPassword);
						if(len > 0)
						{
							
							strncpy( temp_passwd, ChangePass.m_StrNewPassword,len );
							/**** Encrypt the CHANGE NEW password  - Start ****/
							{
								len = strlen(temp_passwd);
								if(len <= MAX_USERPWD_LEN)
							
									gui_password_encryption((BYTE *)temp_passwd,len);
							}
						}					/**** Encrypt the password  - End ****/

						
					temp1 = Copy_previous_passwords(&ucf01);

					if(temp1 == 1)
					{
						continue;
					}
	

					strncpy((char *)ucf01.password_modified_date, toDayDate,LEN_YYYYMMDD);
					strcpy((char *)ucf01.accountstatus,USER_UNLOCKED);
						
						rcode_UCF01 = txdsapi_update_record( (pBYTE)&ucf01, sizeof (UCF01), UCF01_DATA, error_msg );
						//int rcode2 = txdsapi_update_record( (pBYTE)&ucf01, sizeof (UCF01), UCF01_DATA, error_msg );
						//if( rcode == PTEMSG_OK && rcode2 == PTEMSG_OK)
						if( rcode_UCF01 == PTEMSG_OK )
							AfxMessageBox( "Your password has been updated", MB_ICONINFORMATION | MB_OK );
						else
							AfxMessageBox( error_msg, MB_ICONSTOP | MB_OK );
						//	AfxMessageBox( "Invalid Password .", MB_ICONSTOP | MB_OK );
						//continue;			
					}		
				}
				//break;
			}
			
		if( ret == IDCANCEL )
		{
			AfxMessageBox( "Please Change Your Password before trying to log in", MB_ICONSTOP | MB_OK );
			continue;
			//return CANCEL;
		}
			theTime = CTime::GetCurrentTime();//Get Todays date
			year= 	theTime.GetYear ();
			
			    //ramya
			if( strcmp( (char *)ucf01.User_Status, "" ) == 0 )
			{
				strncpy((char *)ucf01.User_Status,STATUS_ACTIVE,STATUS_ACTIVE_LEN);

			}

			GetComputerName( wstation, &wsize );
		
			strncpy((char *)ucf01.Login_WorkStationId, wstation,wsize);
			strncpy((char *)ucf01.Login_Status,"1",LOGIN_STATUS_LEN);
			ucf01.Login_Status[1] = '\0';

			ucf01.user_info_3[0] = year - 2000;
			ucf01.user_info_3[1] = theTime.GetMonth();
			ucf01.user_info_3[2] = theTime.GetDay();
			ucf01.user_info_3[3] = theTime.GetHour();
			ucf01.user_info_3[4] = theTime.GetMinute();
			ucf01.user_info_3[5] = theTime.GetSecond();
			ucf01.user_info_3[6] = '\0';
			rcode_UCF01 = txdsapi_update_record( (pBYTE)&ucf01, sizeof (UCF01), UCF01_DATA, error_msg );
			if( rcode_UCF01 != PTEMSG_OK )
				AfxMessageBox( error_msg, MB_ICONSTOP | MB_OK );
			break;
		}//if current * prev Login within days Inactive 


		else if(jday2 - jday3 >= atoi((char *)(ucf01.Days_Inactive)) && atoi((char *)ucf01.Days_Inactive)!=NULL )
		{
			int ri;
			
			ri = InactiveDetails();
			
			
			//if(!(strncmp((char *)ucf01.control[0].action,ADMIN_NO_VIEW,4))||!(strncmp((char *)ucf01.control[0].action,ADMIN_USER,4)))
			if(strncmp(login_name,"ADMINISTRATOR",strlen("ADMINISTRATOR")) == 0 )
			{
				if (ri == 1)
				{
				return TRUE;
				}
				//else
				//return false;
			}
			
			else
			{
				return false;
			}

		}
		
			
     } //after DoModal, if its return value is correct.

	
	 else if(ret == IDCANCEL)
	 {
		 return CANCEL;
	 }
     else 
		 return FALSE; 
	
	
  }

 
   
   return TRUE;
}

 BOOL CLogin::PopulateLoginControlStructure()
  {
	  if(Prev_Login_Date!=New_Login_Date)
	  {
			change_login = true;
	  }

	  if(strcmp(Prev_Logout_Date1, New_Logout_Date1)!=0)
	  {
			change_logout = true;
	  }
	  return TRUE;
  }

  BOOL CLogin::PopulateHistoryTable()
  {
		HST01 sHistoryData;
		CHAR dest_str[18];   
		CMyUtils myUtilsObject;
		char strErrorMsg[200] = "";
		CString temp;
		CHAR wstation[21]; 
		DWORD wsize;
		static int i=0;

		char strDate[9] = "";
		char strTime[7] = "";

		memset ( strDate, 0, sizeof (strDate));
		memset ( strTime, 0, sizeof (strTime));
		
		myUtilsObject.GetSystemDate(strDate);
		myUtilsObject.GetSystemTime(strTime);

		pintime_get_gmt ( dest_str );
		ZeroMemory ( &sHistoryData, sizeof (HST01));
		memcpy ((LPTSTR) sHistoryData.file_name, (LPCTSTR)"UCF01", sizeof (sHistoryData.file_name));
		temp = strDate;
		temp = temp.Right(4) + temp.Left(2) + temp.Mid(2,2);
		strcpy(strDate, temp);
		wsize = sizeof( wstation );
		GetComputerName( wstation, &wsize );

		if(Prev_Login_Date!=New_Login_Date && change_login == true)
		{
			change_login = false;
			ZeroMemory ( &sHistoryData, sizeof (HST01));
			memcpy ((LPTSTR) sHistoryData.file_name, (LPCTSTR)"UCF01", sizeof (sHistoryData.file_name));
			memcpy ((LPTSTR)sHistoryData.changed_by,(LPTSTR)ucf01.primary_key.name, sizeof(ucf01.primary_key.name)); 	
			memcpy ( sHistoryData.reserved,	wstation, sizeof(sHistoryData.reserved));
			memcpy ( sHistoryData.date_modified, strDate, sizeof(sHistoryData.date_modified));
			memcpy ( sHistoryData.time_modified, strTime, sizeof(sHistoryData.time_modified));
			memcpy ( sHistoryData.file_primary_key,  m_LoginName, sizeof (sHistoryData.file_primary_key));
			memcpy ( sHistoryData.primary_key.unique_id, dest_str, sizeof (sHistoryData.primary_key.unique_id));
			memcpy ( sHistoryData.field_tag, "LogIN Date", sizeof (sHistoryData.field_tag));
			memcpy ( sHistoryData.current_data, New_Login_Date, sizeof (sHistoryData.current_data));
			memcpy ( sHistoryData.previous_data, Prev_Login_Date, sizeof (sHistoryData.previous_data));
				
			int rcode = txdsapi_insert_record  ( (pBYTE)&sHistoryData, sizeof (HST01), HST01_DATA, strErrorMsg );
			if ( rcode != PTEMSG_OK )
			{
				//AfxMessageBox( "Insertion into History Table Failed", MB_ICONSTOP | MB_OK );
				return FALSE;
			}
		}

		else if(change_logout == true && (strcmp(Prev_Logout_Date1, (char *)New_Logout_Date1)!=0))
		{
			change_logout = false;
			ZeroMemory ( &sHistoryData, sizeof (HST01));
			memcpy ((LPTSTR) sHistoryData.file_name, (LPCTSTR)"UCF01", sizeof (sHistoryData.file_name));
			memcpy ((LPTSTR)sHistoryData.changed_by,(LPTSTR)ucf01.primary_key.name, sizeof(ucf01.primary_key.name)); 	
			memcpy ( sHistoryData.reserved,	wstation, sizeof(sHistoryData.reserved));
			memcpy ( sHistoryData.date_modified, strDate, sizeof(sHistoryData.date_modified));
			memcpy ( sHistoryData.time_modified, strTime, sizeof(sHistoryData.time_modified));
			memcpy ( sHistoryData.file_primary_key,  m_LoginName, sizeof (sHistoryData.file_primary_key));
			memcpy ( sHistoryData.primary_key.unique_id, dest_str, sizeof (sHistoryData.primary_key.unique_id));
			memcpy ( sHistoryData.field_tag, "LogOut Date", sizeof (sHistoryData.field_tag));
			memcpy ( sHistoryData.current_data, New_Logout_Date1, sizeof (sHistoryData.current_data));
			memcpy ( sHistoryData.previous_data, Prev_Logout_Date1, sizeof (sHistoryData.previous_data));
				
			int rcode = txdsapi_insert_record  ( (pBYTE)&sHistoryData, sizeof (HST01), HST01_DATA, strErrorMsg );
			if ( rcode != PTEMSG_OK )
			{
				//AfxMessageBox( "Insertion into History Table Failed", MB_ICONSTOP | MB_OK );
				return FALSE;
			}
		}
		return TRUE;
  }

BOOL CLogin::InactiveDetails()
{
		char error_msg[256] = {0};
		BYTE rcode; char tmpbuff[256] = {0};
		memcpy(ucf01.User_Status,STATUS_INACTIVE,STATUS_INACTIVE_LEN);
		memset(ucf01.accountstatus,0x00,sizeof(ucf01.accountstatus));
		   
		strncpy( (char *)ucf01.accountstatus , "locked",strlen("locked"));
		strncpy((char *)ucf01.Date_Of_Deactivation, TODAYDate, 8);
		rcode = txdsapi_update_record( (pBYTE)&ucf01, sizeof (UCF01), UCF01_DATA, error_msg );
		if( rcode == PTEMSG_OK)
		{
			
			//if(!(strncmp((char *)ucf01.control[0].action,ADMIN_NO_VIEW,4))||!(strncmp((char *)ucf01.control[0].action,ADMIN_USER,4)))
			if(strncmp(login_name,"ADMINISTRATOR",strlen("ADMINISTRATOR")) == 0 )
			{
				return TRUE;
			}
			else
			{
				memset(tmpbuff,0x00,sizeof(tmpbuff));
				sprintf(tmpbuff,"User ID Inactive. Cannot Login. User status is : %s",ucf01.User_Status);
				AfxMessageBox( tmpbuff );
				return FALSE;
			}
		}
		else
		{
			AfxMessageBox( "Failed to Update User Status of an Inactive User", MB_ICONSTOP | MB_OK );
			return FALSE;
		}
}

BOOL CLogin::ExpiryDetails()
{
		char error_msg[256] = {0};	
		BYTE rcode; char tmpbuff[256] = {0};
		memcpy(ucf01.User_Status,STATUS_EXPIRED,STATUS_EXPIRED_LEN);
		memset(ucf01.accountstatus,0x00,sizeof(ucf01.accountstatus));
		
		strncpy( (char *)ucf01.accountstatus , "locked",strlen("locked"));
		rcode = txdsapi_update_record( (pBYTE)&ucf01, sizeof (UCF01), UCF01_DATA, error_msg );
		if( rcode == PTEMSG_OK)
		{
			//if(!(strncmp((char *)ucf01.control[0].action,ADMIN_NO_VIEW,4))||!(strncmp((char *)ucf01.control[0].action,ADMIN_USER,4)))
			if(strncmp(login_name,"ADMINISTRATOR",strlen("ADMINISTRATOR")) == 0 )
			{
				return TRUE;
			}
			else
			{
				memset(tmpbuff,0x00,sizeof(tmpbuff));
				sprintf(tmpbuff,"User ID Expired. Cannot Perform Reset. User status is : %s",ucf01.User_Status);
				AfxMessageBox( tmpbuff );
				return FALSE;
			}
		}
		 else
		{
			AfxMessageBox( "Failed to Update the Expired User Status while resetting User ID", MB_ICONSTOP | MB_OK );
			return FALSE;
		}
}

short CLogin :: password_check()
{
				int len = strlen(temp_passwd);
				DWORD hash = 0;
				if(len <= MAX_USERPWD_LEN)
					hash = gui_password_encryption((BYTE *)temp_passwd,len);
				m_Password = temp_passwd;
				login.m_Password = temp_passwd;
			
			/**** Encrypt the password  - End ****/



			if( strcmp( (char *)ucf01.password, temp_passwd ) != 0 )
			{
				//maxpassordcount++;
				loginCount++; /** To disable 3 unsuccesfull attempts - TF_Veena **/
				return 0;
			}
			else
			{
				return 1;
			}
}

BOOL CLogin::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	// TODO: Add your specialized code here and/or call the base class
	return CDialog::OnCommand(wParam, lParam);
}

BOOL CLogin::OnInitDialog() 
{
	CDialog::OnInitDialog();

	 m_CntlLoginName.SetLimitText(19);

	
	if(m_bDisableNameFld)
	{
		m_CntlLoginName.EnableWindow(FALSE);
		//m_CntlDaysInactive.EnableWindow(FALSE);
	}
	else
	{
		m_CntlLoginName.EnableWindow(TRUE);
		//m_CntlDaysInactive.EnableWindow(TRUE);
	}
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}


void CLogin::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	CDialog::OnOK();
}


int CLogin::Copy_previous_passwords(pUCF01 ucf01)
{
					unsigned char Pswrd_Tmp_Cpy[12][51];
					int pw_len = 0;
					CHAR temp_connect_str[50] = { 0 };
					CHAR temp_name_str[50] = { 0 };
					int temp1=0;

					pw_len = strlen((char *)ucf01->password);
					if(pw_len>0)
					{
						strncpy((char *)Pswrd_Tmp_Cpy[0],(char *)ucf01->password,pw_len);
					}

					pw_len = 0;
					pw_len = strlen((char *)ucf01->password_1);
					if(pw_len>0)
					{
						strncpy((char *)Pswrd_Tmp_Cpy[1],(char *)ucf01->password_1,pw_len);
					}

					pw_len = 0;
					pw_len = strlen((char *)ucf01->password_2);
					if(pw_len>0)
					{
						strncpy((char *)Pswrd_Tmp_Cpy[2],(char *)ucf01->password_2,pw_len);
					}

					pw_len = 0;
					pw_len = strlen((char *)ucf01->password_3);
					if(pw_len>0)
					{
						strncpy((char *)Pswrd_Tmp_Cpy[3],(char *)ucf01->password_3,pw_len);
					}

					pw_len = 0;
					pw_len = strlen((char *)ucf01->password_4);
					if(pw_len>0)
					{
						strncpy((char *)Pswrd_Tmp_Cpy[4],(char *)ucf01->password_4,pw_len);
					}

					pw_len = 0;
					pw_len = strlen((char *)ucf01->password_5);
					if(pw_len>0)
					{
						strncpy((char *)Pswrd_Tmp_Cpy[5],(char *)ucf01->password_5,pw_len);
					}

					pw_len = 0;
					pw_len = strlen((char *)ucf01->password_6);
					if(pw_len>0)
					{
						strncpy((char *)Pswrd_Tmp_Cpy[6],(char *)ucf01->password_6,pw_len);
					}

					pw_len = 0;
					pw_len = strlen((char *)ucf01->password_7);
					if(pw_len>0)
					{
						strncpy((char *)Pswrd_Tmp_Cpy[7],(char *)ucf01->password_7,pw_len);
					}

					pw_len = 0;
					pw_len = strlen((char *)ucf01->password_8);
					if(pw_len>0)
					{
						strncpy((char *)Pswrd_Tmp_Cpy[8],(char *)ucf01->password_8,pw_len);
					}

					pw_len = 0;
					pw_len = strlen((char *)ucf01->password_9);
					if(pw_len>0)
					{
						strncpy((char *)Pswrd_Tmp_Cpy[9],(char *)ucf01->password_9,pw_len);
					}

					pw_len = 0;
					pw_len = strlen((char *)ucf01->password_10);
					if(pw_len>0)
					{
						strncpy((char *)Pswrd_Tmp_Cpy[10],(char *)ucf01->password_10,pw_len);
					}
					int pwd_chk_flag = 0;
					for(int i=0; i<atoi((char *)gpcf01.max_prev_passwords_not_allowed) ; i++)
					{
						
						   if(strncmp( (char *)Pswrd_Tmp_Cpy[i], temp_passwd,strlen(temp_passwd)) ==0)
						   {	
								char tempbuff[256] = {0};
								sprintf(tempbuff,"Enter Different Password from your last %s passwords",gpcf01.max_prev_passwords_not_allowed);
								AfxMessageBox(tempbuff , MB_ICONSTOP | MB_OK );
								/*ChangePass.m_CntlNewPassword.SetFocus();*/										
								//continue;
								pwd_chk_flag = 1;
								break;
						   }	
					}
					if(pwd_chk_flag == 1)
					{
						return 1;	
					}
					////ramya Added for DAys_Inactive
					//m_DaysInactive = _T("60");
					//strncpy((char *)ucf01.Days_Inctive,m_DaysInactive,sizeof(m_DaysInactive));
					//
					
					//updating dataserver with modified user structure,Storing Previous password TF-Srikanth

				/***********************************User ID cannot be nominated for password. *************/
				 temp_connect_str[strlen(temp_connect_str)] = 0; //Make sure string is terminated
				 temp1 = 0;

				 strcpy(temp_connect_str,ChangePass.m_StrNewPassword); //copying temp password into the temp var which will later convert to upper case.
				 strcpy(temp_name_str,(char *)ucf01->primary_key.name);
				 while ( 0 != temp_connect_str[temp1] )
					temp_connect_str[temp1++] = toupper(temp_connect_str[temp1]); // Convert string to upper case

				//if( strstr( (char *)ucf01.primary_key.name,temp_connect_str ) != NULL )
				 if( strspn(temp_name_str , temp_connect_str ) != NULL )
				{
					AfxMessageBox( "Password should not contain any similarity to Username", MB_ICONSTOP | MB_OK );
					return 1;
				}

				/***************************************************************************************/

					int pswrd_len=0;
					pswrd_len=strlen((char *)ucf01->password_9);
					if(pswrd_len>0)
					{
						strncpy((char *)ucf01->password_10,(char *)ucf01->password_9,pswrd_len);
					}
					pswrd_len=0;
					pswrd_len=strlen((char *)ucf01->password_8);
					if(pswrd_len>0)
					{
						strncpy((char *)ucf01->password_9,(char *)ucf01->password_8,pswrd_len);
					}
					pswrd_len=0;
					pswrd_len=strlen((char *)ucf01->password_7);
					if(pswrd_len>0)
					{
						strncpy((char *)ucf01->password_8,(char *)ucf01->password_7,pswrd_len);
					}
					pswrd_len=0;
					pswrd_len=strlen((char *)ucf01->password_6);
					if(pswrd_len>0)
					{
						strncpy((char *)ucf01->password_7,(char *)ucf01->password_6,pswrd_len);
					}
					pswrd_len=0;
					pswrd_len=strlen((char *)ucf01->password_5);
					if(pswrd_len>0)
					{
						strncpy((char *)ucf01->password_6,(char *)ucf01->password_5,pswrd_len);
					}
					pswrd_len=0;
					pswrd_len=strlen((char *)ucf01->password_4);
					if(pswrd_len>0)
					{
						strncpy((char *)ucf01->password_5,(char *)ucf01->password_4,pswrd_len);	
					}
					pswrd_len=0;
					pswrd_len=strlen((char *)ucf01->password_3);
					if(pswrd_len>0)
					{
						strncpy((char *)ucf01->password_4,(char *)ucf01->password_3,pswrd_len);
					}
					pswrd_len=0;
					pswrd_len=strlen((char *)ucf01->password_2);
					if(pswrd_len>0)
					{
						strncpy((char *)ucf01->password_3,(char *)ucf01->password_2,pswrd_len);
					}
					pswrd_len=0;
					pswrd_len=strlen((char *)ucf01->password_1);
					if(pswrd_len>0)
					{
						strncpy((char *)ucf01->password_2,(char *)ucf01->password_1,pswrd_len);
					}
					pswrd_len=0;
					pswrd_len=strlen((char *)ucf01->password);
					if(pswrd_len>0)
					{
						strncpy((char *)ucf01->password_1,(char *)ucf01->password,pswrd_len );
					}
					pswrd_len=0;
					pswrd_len=strlen(temp_passwd);
					if(pswrd_len>0)
					{
						strncpy((char *)ucf01->password, temp_passwd ,pswrd_len);
					}
					return 0;

}

void CLogin::OnEnChangePassword()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
}


void CLogin::OnEnChangeLoginName()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
}


void CLogin::OnBnClickedResetLogoutCheckbox()
{
	// TODO: Add your control notification handler code here
}
