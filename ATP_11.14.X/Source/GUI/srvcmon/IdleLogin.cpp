// Loginfrm.cpp : implementation file
//

#include "stdafx.h"
#include "srvcmon.h"
#include "IdleLoginfrm.h"
#include "common.h"
#include "ptepci.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern "C"
{  
	#include "app_info.h"
	#include "equitdb.h"
	#include "dc_dbstruct.h"
	#include "nc_dbstruct.h"
	#include "tx_dbstruct.h"
    #include "txdsapi.h"
}

/////////////////////////////////////////////////////////////////////////////
// CIdleLoginfrm dialog
 
CIdleLoginfrm::CIdleLoginfrm(CWnd* pParent /*=NULL*/)
	: CDialog(CIdleLoginfrm::IDD, pParent)
{
	//{{AFX_DATA_INIT(CIdleLoginfrm)
	m_userpwd = _T("");
	m_userid = _T("");
	//}}AFX_DATA_INIT
}


void CIdleLoginfrm::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CIdleLoginfrm)
	DDX_Text(pDX, IDC_EDIT_PWD, m_userpwd);
	DDX_Text(pDX, IDC_EDIT_UID, m_userid);
	DDV_MaxChars(pDX,m_userid, MAX_USERNAME_LEN);
	DDV_MaxChars(pDX,m_userpwd, MAX_USERPWD_LEN);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CIdleLoginfrm, CDialog)
	//{{AFX_MSG_MAP(CIdleLoginfrm)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CIdleLoginfrm message handlers


// CIdleLoginfrm message handlers
short CIdleLoginfrm::CheckLoginDetails()
{
	
	CHAR error_msg[256];
	short rcode;	
	char tmpbuff[250]={'\0'};
	char u_name[51]={'\0'};
	char u_pwd[51]={'\0'};
	int logincnt=0;
	int ret;
	CPtepci ptepci;

   while( 1 )
   {
	  m_userpwd = _T("");
      ret = DoModal();

      if( ret == IDOK )
      {	
		  strcpy( u_name, m_userid );
          strcpy( u_pwd, m_userpwd );

		  rcode = ptepci.ValidateIdleLogin(u_name, u_pwd, error_msg);
		  if(rcode == 1 )
		  {
			  return true;
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
				  rcode = ptepci.BlockUser(u_name, error_msg);
				  
				  AfxMessageBox(error_msg);

				  continue;
			  }
		  }
	  }
	  else
	  {
			return CANCEL;
			//continue;
	  }
	}
}

