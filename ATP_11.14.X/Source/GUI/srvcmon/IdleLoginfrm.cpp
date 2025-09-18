// Loginfrm.cpp : implementation file
//

#include "stdafx.h"
#include "srvcmon.h"
#include "IdleLoginfrm.h"
#include "ptepci.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CString workstation;
 UCF01 ucf01;
 UCF03 ucf03;
extern ATP02 atp02;
extern CCF03 sCCF03Record;
/////////////////////////////////////////////////////////////////////////////
// CIdleLoginfrm dialog

extern "C"
{  
    #include "TXDSAPI.h"
}
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
short rcode;	
	char u_name[51]={'\0'};
	char u_pwd[51]={'\0'};
	CHAR error_msg[256];
	int ret;

	int logincnt = 0;

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
		  if(rcode == 1)
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
				  rcode = ptepci.BlockUser(u_name, error_msg);
				  
				  AfxMessageBox(error_msg);

				  return CANCEL;
			  }
		  }
	  }
	  else 
	  {
		  return CANCEL;		  
	  }
	}
}


/////////////////////////////////////////////////////////////////////////////
// CIdleLoginfrm dialog


CIdleLoginfrm::CIdleLoginfrm(CWnd* pParent /*=NULL*/)
	: CDialog(CIdleLoginfrm::IDD, pParent)
{
	//{{AFX_DATA_INIT(CIdleLoginfrm)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CIdleLoginfrm::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CIdleLoginfrm)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CIdleLoginfrm, CDialog)
	//{{AFX_MSG_MAP(CIdleLoginfrm)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CIdleLoginfrm message handlers
/////////////////////////////////////////////////////////////////////////////
// CIdleLoginfrm dialog


CIdleLoginfrm::CIdleLoginfrm(CWnd* pParent /*=NULL*/)
	: CDialog(CIdleLoginfrm::IDD, pParent)
{
	//{{AFX_DATA_INIT(CIdleLoginfrm)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CIdleLoginfrm::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CIdleLoginfrm)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CIdleLoginfrm, CDialog)
	//{{AFX_MSG_MAP(CIdleLoginfrm)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CIdleLoginfrm message handlers
/////////////////////////////////////////////////////////////////////////////
// CIdleLoginfrm dialog


CIdleLoginfrm::CIdleLoginfrm(CWnd* pParent /*=NULL*/)
	: CDialog(CIdleLoginfrm::IDD, pParent)
{
	//{{AFX_DATA_INIT(CIdleLoginfrm)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CIdleLoginfrm::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CIdleLoginfrm)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CIdleLoginfrm, CDialog)
	//{{AFX_MSG_MAP(CIdleLoginfrm)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CIdleLoginfrm message handlers
