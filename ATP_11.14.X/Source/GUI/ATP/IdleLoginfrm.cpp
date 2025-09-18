// IdleLoginfrm.cpp : implementation file
//

#include "stdafx.h"
#include "atp.h"

#include "IdleLoginfrm.h"
#include "common.h"
#include "changepassword.h"
#include "ptepci.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CString workstation;

extern UCF01 ucf01;

extern CChangePassword ChangePass;

extern "C"
{     
   #include "ntutils.h"
   #include "Txutils.h"
}

/////////////////////////////////////////////////////////////////////////////
// CIdleLoginfrm dialog


CIdleLoginfrm::CIdleLoginfrm(CWnd* pParent /*=NULL*/)
	: CDialog(CIdleLoginfrm::IDD, pParent)
{
	//{{AFX_DATA_INIT(CIdleLoginfrm)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_userpwd = _T("");
	m_userid = _T("");
	m_bDisableNameFld = FALSE;
}


void CIdleLoginfrm::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CIdleLoginfrm)
	DDX_Control(pDX, IDC_EDIT_PWD, m_upwd);
	DDX_Control(pDX, IDC_EDIT_UID, m_uid);
	DDX_Text(pDX, IDC_EDIT_PWD, m_userpwd);
	DDV_MaxChars(pDX,m_userpwd, MAX_USERPWD_LEN);
	DDX_Text(pDX, IDC_EDIT_UID, m_userid);
	DDV_MaxChars(pDX,m_userid, MAX_USERNAME_LEN);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CIdleLoginfrm, CDialog)
	//{{AFX_MSG_MAP(CIdleLoginfrm)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CIdleLoginfrm message handlers

BOOL CIdleLoginfrm::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	if(!m_bDisableNameFld)
	{
		m_uid.EnableWindow(TRUE);
	}
	else
	{
		m_uid.EnableWindow(FALSE);
		this->SetWindowText(m_szTitle);
	}
	// TODO: Add extra initialization here
	m_userpwd = _T("");
	
	m_uid.SetLimitText(50);
	m_upwd.SetLimitText(50);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


short CIdleLoginfrm::CheckLoginDetails()
{	short rcode;	
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

