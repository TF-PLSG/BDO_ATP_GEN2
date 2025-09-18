// Advices.cpp : implementation file
//

#include "stdafx.h"
#include "monitor.h"
#include "Advices.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// Advices dialog


Advices::Advices(CWnd* pParent /*=NULL*/)
	: CDialog(Advices::IDD, pParent)
{
	//{{AFX_DATA_INIT(Advices)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void Advices::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(Advices)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(Advices, CDialog)
	//{{AFX_MSG_MAP(Advices)
	ON_BN_CLICKED(IDC_EXIT, OnExit)
	ON_BN_CLICKED(IDC_SAF, OnSaf)
	ON_BN_CLICKED(IDC_START_ADVICES, OnStartAdvices)
	ON_BN_CLICKED(IDC_STOP_ADVICES, OnStopAdvices)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Advices message handlers

BOOL Advices::OnInitDialog() 
{
   CDialog::OnInitDialog();
   return TRUE;  // return TRUE unless you set the focus to a control
                 // EXCEPTION: OCX Property Pages should return FALSE
}

void Advices::OnExit() 
{
   EndDialog(IDC_EXIT);
}

void Advices::OnSaf() 
{
   EndDialog(IDC_SAF);
}

void Advices::OnStartAdvices() 
{
   EndDialog(IDC_START_ADVICES);
}

void Advices::OnStopAdvices() 
{
   EndDialog(IDC_STOP_ADVICES);
}
