// ArchiveDBPasswords.cpp : implementation file
//

#include "stdafx.h"
#include "atp.h"
#include "ArchiveDBPasswords.h"
extern "C"
{
   #include "ntutils.h"
#include "tx_dbstruct.h"
}

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

ArchDbPasswords dbpass;
/////////////////////////////////////////////////////////////////////////////
// CArchiveDBPasswords dialog


CArchiveDBPasswords::CArchiveDBPasswords(CWnd* pParent /*=NULL*/)
	: CDialog(CArchiveDBPasswords::IDD, pParent)
{
	//{{AFX_DATA_INIT(CArchiveDBPasswords)
	m_StrOldPassword = _T("");
	m_StrNewPassword = _T("");
	m_StrUserId = _T("");
	m_StrConfirmPassword = _T("");
	m_StrMessageLbl = _T("");
	//}}AFX_DATA_INIT
}


void CArchiveDBPasswords::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CArchiveDBPasswords)
	DDX_Control(pDX, IDC_NEW_ARCH_PASSWORD, m_CntlConfirmPassword);
	DDX_Control(pDX, IDC_ARCH_USER_ID, m_CntlUserId);
	DDX_Control(pDX, IDC_ARCH_PASSWORD, m_CntlNewPassword);
	DDX_Control(pDX, IDC_ARCH_OLD_PASSWORD, m_CntlOldPassword);
	DDX_Text(pDX, IDC_ARCH_OLD_PASSWORD, m_StrOldPassword);
	DDX_Text(pDX, IDC_ARCH_PASSWORD, m_StrNewPassword);
	DDX_Text(pDX, IDC_ARCH_USER_ID, m_StrUserId);
	DDX_Text(pDX, IDC_NEW_ARCH_PASSWORD, m_StrConfirmPassword);
	DDX_Text(pDX, IDC_STATUS, m_StrMessageLbl);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CArchiveDBPasswords, CDialog)
	//{{AFX_MSG_MAP(CArchiveDBPasswords)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CArchiveDBPasswords message handlers

BOOL CArchiveDBPasswords::OnInitDialog() 
{
	CDialog::OnInitDialog();
	CHAR error_msg[256] = {0};
	CFont m_font;
		LOGFONT lf;
		memset ( &lf, 0, sizeof (lf));
		lf.lfHeight = 15;
		lf.lfWeight = 700;
		strcpy(lf.lfFaceName, "Arial");
		m_font.CreateFontIndirect (&lf);

		GetDlgItem(IDC_STATUS)->SetFont (&m_font);//Set the message font

		m_StrConfirmPassword = "";
		m_StrMessageLbl = "";
		m_StrNewPassword = "";
		m_StrOldPassword = "";
		m_StrUserId = "";

		int dbrcode= txdsapi_get_record((pBYTE)&dbpass, sizeof (ArchDbPasswords),DB_ARCH_PASSWORD_DATA, error_msg );
		if( dbrcode == PTEMSG_NOT_FOUND )
		{
			m_StrMessageLbl=error_msg;
			UpdateData(FALSE);
			return TRUE;
		}
		m_StrUserId = dbpass.arch_user_id;
UpdateData(FALSE);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CArchiveDBPasswords::OnOK() 
{
	char error_msg[256] = {0};
	UpdateData(TRUE);

	if(m_StrOldPassword.IsEmpty())
	{
		m_StrMessageLbl = "Please enter old password";
		m_CntlOldPassword.SetFocus();
		UpdateData(FALSE);
		return;
	}
	if(m_StrNewPassword.IsEmpty())
	{
		m_StrMessageLbl = "Please enter new password";
		m_CntlNewPassword.SetFocus();
		UpdateData(FALSE);
		return;
	}

	if(m_StrNewPassword.Compare(m_StrConfirmPassword) != 0)
	{
		m_StrMessageLbl="Password Confirmation does not match. Please re-enter.";
		m_CntlNewPassword.SetFocus();
		UpdateData(FALSE);
		return;
	}
	if(m_StrOldPassword.Compare((char *)dbpass.arch_old_password) != 0)
	{
		m_StrMessageLbl="Old password is incorrect. Please re-enter.";
		m_CntlNewPassword.SetFocus();
		UpdateData(FALSE);
		return;
	}

	strcpy( (char *) dbpass.arch_password, m_StrNewPassword);
	int rcode = txdsapi_update_record((pBYTE)&dbpass, sizeof (ArchDbPasswords),DB_ARCH_PASSWORD_DATA, error_msg );
	if( rcode == PTEMSG_OK )
	{
		m_StrMessageLbl = "Password Updated Successfully";
		m_StrConfirmPassword = "";
		m_StrNewPassword = "";
		m_StrOldPassword = "";
	}
	else
	{
		m_StrMessageLbl = "Password not updated";
	}
	UpdateData(FALSE);

	
}
