// MraDBPasswords.cpp : implementation file
//

#include "stdafx.h"
#include "atp.h"
#include "MraDBPasswords.h"
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

/////////////////////////////////////////////////////////////////////////////
// CMraDBPasswords dialog

MraDbPasswords dbpass;

CMraDBPasswords::CMraDBPasswords(CWnd* pParent /*=NULL*/)
	: CDialog(CMraDBPasswords::IDD, pParent)
{
	//{{AFX_DATA_INIT(CMraDBPasswords)
	m_StrOldPassword = _T("");
	m_StrNewPassword = _T("");
	m_StrUserId = _T("");
	m_StrConfirmPassword = _T("");
	m_StrMessageLbl = _T("");
	//}}AFX_DATA_INIT
}


void CMraDBPasswords::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMraDBPasswords)
	DDX_Control(pDX, IDC_NEW_MRA_PASSWORD, m_CntlConfirmPassword);
	DDX_Control(pDX, IDC_MRA_USER_ID, m_CntlUserId);
	DDX_Control(pDX, IDC_MRA_PASSWORD, m_CntlNewPassword);
	DDX_Control(pDX, IDC_MRA_OLD_PASSWORD, m_CntlOldPassword);
	DDX_Text(pDX, IDC_MRA_OLD_PASSWORD, m_StrOldPassword);
	DDX_Text(pDX, IDC_MRA_PASSWORD, m_StrNewPassword);
	DDX_Text(pDX, IDC_MRA_USER_ID, m_StrUserId);
	DDX_Text(pDX, IDC_NEW_MRA_PASSWORD, m_StrConfirmPassword);
	DDX_Text(pDX, IDC_STATUS, m_StrMessageLbl);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CMraDBPasswords, CDialog)
	//{{AFX_MSG_MAP(CMraDBPasswords)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMraDBPasswords message handlers

BOOL CMraDBPasswords::OnInitDialog() 
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

		int dbrcode= txdsapi_get_record((pBYTE)&dbpass, sizeof (MraDbPasswords),DB_MRA_PASSWORD_DATA, error_msg );
		if( dbrcode == PTEMSG_NOT_FOUND )
		{
			m_StrMessageLbl=error_msg;
			UpdateData(FALSE);
			return TRUE;
		}
		m_StrUserId = dbpass.mra_user_id;
UpdateData(FALSE);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CMraDBPasswords::OnOK() 
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
	if(m_StrOldPassword.Compare((char *)dbpass.mra_old_password) != 0)
	{
		m_StrMessageLbl="Old password is incorrect. Please re-enter.";
		m_CntlNewPassword.SetFocus();
		UpdateData(FALSE);
		return;
	}

	strcpy( (char *) dbpass.mra_password, m_StrNewPassword);
	int rcode = txdsapi_update_record((pBYTE)&dbpass, sizeof (MraDbPasswords),DB_MRA_PASSWORD_DATA, error_msg );
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

