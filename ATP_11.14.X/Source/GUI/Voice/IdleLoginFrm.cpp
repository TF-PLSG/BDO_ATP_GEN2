// IdleLoginfrm1.cpp : implementation file
//

#include "stdafx.h"
#include "voiceauthorization.h"
#include "IdleLoginfrm.h"
#include "common.h"
#include "ptepci.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


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
	DDX_Control(pDX, IDC_EDIT_UID, m_CntlUID);
	DDX_Text(pDX, IDC_EDIT_PWD, m_userpwd);
	DDV_MaxChars(pDX,m_userpwd, MAX_USERPWD_LEN);
	DDX_Text(pDX, IDC_EDIT_UID, m_userid);
	DDV_MaxChars(pDX,m_userid, MAX_USERPWD_LEN);
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
	int rcode;
	char u_name[51]={'\0'};
	char u_pwd[51]={'\0'};
	CHAR error_msg[256];
	CPtepci ptepci;
	int logincnt = 0;
	int ret;

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
			
		}
	}
	

   return TRUE;
}



BOOL CIdleLoginfrm::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	
	if(m_userid.IsEmpty())
	{
		m_CntlUID.EnableWindow(TRUE);
	}
	else
	{
		m_CntlUID.EnableWindow(TRUE);
		this->SetWindowText(m_szTitle);
	}
	// TODO: Add extra initialization here
	m_userpwd = _T("");

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
