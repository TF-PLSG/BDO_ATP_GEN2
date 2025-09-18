// Loginfrm.cpp : implementation file
//

#include "stdafx.h"
#include "ArchiveLookup.h"
#include "Loginfrm.h"
#include "common.h"
#include "ptepci.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
 
extern "C"
{  
    #include "TXDSAPI.h"
}
/////////////////////////////////////////////////////////////////////////////
// CLoginfrm dialog


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
		// NOTE: the ClassWizard will add message map macros here
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
	int ret;
	int logincnt = 0;
	
	CPtepci ptepci;

	while(1)
	{	
		m_userpwd = _T("");
		ret = DoModal();
					
		if(ret == IDOK)
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
	

   return TRUE;
}


