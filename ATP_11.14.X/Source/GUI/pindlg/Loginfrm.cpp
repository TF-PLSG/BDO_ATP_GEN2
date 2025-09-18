
#include "stdafx.h"
#include "Loginfrm.h"
#include "common.h"
#include "ptepci.h"

/////////////////////////////////////////////////////////////////////////////
// CLoginfrm dialog


extern "C"
{  
	#include "app_info.h"
	#include "equitdb.h"
	#include "dc_dbstruct.h"
	#include "nc_dbstruct.h"
	#include "tx_dbstruct.h"
    #include "txdsapi.h"
	
	#include "ntutils.h"	
	#include "pte.h"
	#include "ptetcp.h"
}

extern UCF01 ucf01;
extern volatile BOOL wait_f;



CLoginfrm::CLoginfrm(CWnd* pParent /*=NULL*/)
	: CDialog(CLoginfrm::IDD, pParent)
{
	//{{AFX_DATA_INIT(CLoginfrm)
	m_userid = _T("");
	m_userpwd = _T("");
	//}}AFX_DATA_INIT
}


void CLoginfrm::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CLoginfrm)
	DDX_Text(pDX, IDC_UID, m_userid);
	DDX_Text(pDX, IDC_PWD, m_userpwd);
	DDV_MaxChars(pDX,m_userid, MAX_USERNAME_LEN);
	DDV_MaxChars(pDX,m_userpwd, MAX_USERPWD_LEN);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CLoginfrm, CDialog)
	//{{AFX_MSG_MAP(CLoginfrm)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CLoginfrm message handlers

short CLoginfrm::CheckLoginDetails()
{	
	short rcode;	
	char u_name[51]={'\0'};
	char u_pwd[51]={'\0'};
	CHAR error_msg[256];
	int ret, logincnt =0;
	
	//CPtepci ptepci;

	while(1)
	{
		m_userpwd = _T("");

		ret = DoModal();
		
		if(ret == IDOK)
		{    
			 strcpy( u_name, m_userid );
			 strcpy( u_pwd, m_userpwd );

			  //rcode = ptepci.ValidateIdleLogin(u_name, u_pwd, error_msg);
			 rcode = ValidateLoginParams(u_name, u_pwd, error_msg);

			  if(rcode == 1 )
			  {
				  return 1;
			  }
			  else if(rcode == 0)
			  {
				  AfxMessageBox(error_msg);
				  continue;
			  }
			  else if(rcode == 2)
			  {
				  AfxMessageBox(error_msg);

				  logincnt++;
				  if(logincnt >= 3)
				  {
					  logincnt = 0;
					  //rcode = ptepci.BlockUser(u_name, error_msg);
					  
					  AfxMessageBox("Your User id is disabled/inactive to run ATP. Please contact your System Administrator.");
					  return CANCEL;
					  //continue;
				  }
			  }

		}
		else
		{
			return CANCEL;
			//continue;
		}
	}
	

   return TRUE;
}



BOOL CLoginfrm::OnInitDialog() 
{
	CDialog::OnInitDialog();
	m_userpwd = "";
	// TODO: Add extra initialization here
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


BYTE CLoginfrm::ValidateLoginParams(char m_userid[], char m_userpwd[], char *error)
{
	
	CPtepci ptepci;
	BYTE rcode;
	

	  if(strlen(m_userid) < MIN_USERNAME_LEN)
	  {		 	  
		  sprintf(error, "User Name length is less than %d chars", MIN_USERNAME_LEN);
		   
		  return 0;
	  }
	 if(strlen(m_userpwd) < MIN_USERPWD_LEN)
	 
	  {
		  sprintf(error, "User Password length is less than %d chars", MIN_USERPWD_LEN);
		  
		  return 0;
	  }
	  if(!ptepci.isalphabet(m_userpwd))
	  {
		  strcpy(error,"User Password does not contain any Alphabets");
		  return 0;
	  }
	  if(!ptepci.isnumeric(m_userpwd))
	  {
		  strcpy(error,"User Password does not contain any Numbers");
		  return 0;
	  }

	 memset(&ucf01, 0x00, sizeof(ucf01));

	 strcpy( (char *)ucf01.primary_key.name, m_userid );
	 
	 wait_f = true;

	 rcode = SendCmdforRequstUcf01Params((pBYTE)&ucf01,sizeof (UCF01), UCF01_DATA, error );
	  while(wait_f == true)
	  {
		  Sleep(50);
	  }

	  //Checking the User accountstatus 
	if( strcmp( (char *)ucf01.accountstatus, USER_LOCKED ) == 0 )
	{
		strcpy(error,"Your User id is disabled/inactive to run ATP. Please contact your System Administrator.");
		return 0;			
	}	

	// check password		
	if( strcmp( (char *)ucf01.password, m_userpwd ) != 0 )
	{		
		strcpy(error,"Invalid username/password.  Login denied.");
		return 2;
	}

	 return 1;
}


BYTE CLoginfrm::SendCmdforRequstUcf01Params(pBYTE psStructurePtr, int nStructureSize, 
                         BYTE  app_data_type,
                         pCHAR error_msg )
{
   pPTE_MSG       p_msg_out;
   char           buffer[sizeof(AUTH_TX) + 4000];
   CHAR           que_name[8];
   CHAR			  interactive_que_name [MAX_QUE_NAME_SIZE];
   CHAR			  temp_str[100];
   BYTE			  rcode;

   memset( buffer, 0, sizeof( buffer ) );
   memcpy ((buffer + sizeof(AUTH_TX)), psStructurePtr, nStructureSize);
    strcpy( que_name, "trandsA" );

   p_msg_out = ptemsg_build_msg( MT_DB_REQUEST,
                                 ST1_DB_SELECT,
                                 0,
                                 que_name,
                                 interactive_que_name,
                                 (pBYTE) buffer,
                                 sizeof( buffer ),
                                 app_data_type );

   if( p_msg_out == NULL_PTR )
	{
		strcpy( error_msg, "Insufficient memory to build request message" );
		return PTEMSG_INSUFFICIENT_MEMORY;
	}

   // try to send the request
	rcode = ptetcp_send( p_msg_out, temp_str );

	free( p_msg_out );

	if( !rcode )
	{
		sprintf( error_msg, "Communication Error during request to ServiceManager: %s", temp_str );
		return PTEMSG_IPC_ERROR;
	}

	return 1;

}