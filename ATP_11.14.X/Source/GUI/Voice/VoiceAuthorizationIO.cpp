// VoiceAuthorizationIO.cpp : implementation file
//

#include "stdafx.h"
#include "VoiceAuthorization.h"
#include "VoiceAuthorizationIO.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CVoiceAuthorizationIO dialog


CVoiceAuthorizationIO::CVoiceAuthorizationIO(CWnd* pParent /*=NULL*/)
	: CDialog(CVoiceAuthorizationIO::IDD, pParent)
{
	//{{AFX_DATA_INIT(CVoiceAuthorizationIO)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CVoiceAuthorizationIO::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CVoiceAuthorizationIO)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CVoiceAuthorizationIO, CDialog)
	//{{AFX_MSG_MAP(CVoiceAuthorizationIO)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CVoiceAuthorizationIO message handlers

void CVoiceAuthorizationIO::OnOK() 
{
	// TODO: Add extra validation here
	
	CDialog::OnOK();
}
