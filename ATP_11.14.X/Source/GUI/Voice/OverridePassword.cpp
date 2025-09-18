// OverridePassword.cpp : implementation file
//

#include "stdafx.h"
#include "VoiceAuthorization.h"
#include "OverridePassword.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CHAR pw[33];
extern CString TranField;

/////////////////////////////////////////////////////////////////////////////
// COverridePassword dialog


COverridePassword::COverridePassword(CWnd* pParent /*=NULL*/)
	: CDialog(COverridePassword::IDD, pParent)
{
	//{{AFX_DATA_INIT(COverridePassword)
	m_strOverridePassword = _T("");
	//}}AFX_DATA_INIT
}


void COverridePassword::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(COverridePassword)
	DDX_Control(pDX, IDC_OVERRIDE_PASSWORD, m_ctrlOverridepassword);
	DDX_Text(pDX, IDC_OVERRIDE_PASSWORD, m_strOverridePassword);
	DDV_MaxChars(pDX, m_strOverridePassword, 10);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(COverridePassword, CDialog)
	//{{AFX_MSG_MAP(COverridePassword)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// COverridePassword message handlers

void COverridePassword::OnCancel() 
{
   memset( pw, 0x00, sizeof(pw) );
   CDialog::OnCancel();
}

void COverridePassword::OnOK() 
{
   /* Update data into the class variables */
   UpdateData( TRUE );

   if( m_strOverridePassword.IsEmpty())
   {
      AfxMessageBox( "Please Enter Password or Cancel", MB_ICONSTOP | MB_OK );
   }  
   else if (m_strOverridePassword == LPCSTR(pw))
   {
      memset( pw, 0x00, sizeof(pw) );
      CDialog::OnOK();
   }
   else
   {
      AfxMessageBox( "Invalid Password Entered.", MB_ICONSTOP | MB_OK );
   }
   return;
}


BOOL COverridePassword::OnInitDialog() 
{
   /*Build password and put in Password string*/
   CHAR temp_str[33];

   memset (temp_str, 0x00, sizeof(temp_str));
   memset (pw, 0x00, sizeof(pw));

   if ( (TranField == "Deferred Purchase") ||
        (TranField == "Cancel Deferred Purchase") )
   {
      strcpy (temp_str, DEF_STR1);
      strcat (temp_str, DEF_STR2);
      strcat (temp_str, DEF_STR3);
      strcat (temp_str, DEF_STR4);

      strncpy (pw, temp_str+3,  2);
      strncat (pw, temp_str+14, 2);
      strncat (pw, temp_str+21, 3);
      strncat (pw, temp_str+25, 2);
   }
   else if ( TranField == "OFFUS" )
   {
      strcpy (temp_str, NOT_ECN_STR1);
      strcat (temp_str, NOT_ECN_STR2);
      strcat (temp_str, NOT_ECN_STR3);
      strcat (temp_str, NOT_ECN_STR4);

      strncpy (pw, temp_str+3,  2);
      strncat (pw, temp_str+9,  2);
      strncat (pw, temp_str+19, 3);
      strncat (pw, temp_str+29, 1);
   }
   else
   {
      strcpy (temp_str, STR1);
      strcat (temp_str, STR2);
      strcat (temp_str, STR3);
      strcat (temp_str, STR4);

      strncpy (pw, temp_str+4,  2);
      strncat (pw, temp_str+10, 2);
      strncat (pw, temp_str+21, 2);
      strncat (pw, temp_str+27, 2);
   }

   memset (temp_str, 0x00, sizeof(temp_str));
   CDialog::OnInitDialog();

   return TRUE;  // return TRUE unless you set the focus to a control
                 // EXCEPTION: OCX Property Pages should return FALSE
}
