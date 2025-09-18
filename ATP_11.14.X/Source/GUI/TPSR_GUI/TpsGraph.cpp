// TpsGraph.cpp : implementation file
//

#include "stdafx.h"
#include "TPSR_GUI.h"
#include "TpsGraph.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTpsGraph dialog


CTpsGraph::CTpsGraph(CWnd* pParent /*=NULL*/)
	: CDialog(CTpsGraph::IDD, pParent)
{
	//{{AFX_DATA_INIT(CTpsGraph)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CTpsGraph::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTpsGraph)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CTpsGraph, CDialog)
	//{{AFX_MSG_MAP(CTpsGraph)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTpsGraph message handlers

void CTpsGraph::OnOK() 
{

}

BOOL CTpsGraph::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	CRect rcClient;
	CDC *pDC = this->GetDC();
	GetClientRect(&rcClient);
	//Put a horizontal line in the middle of the dialog
	int nMiddle = (rcClient.bottom / 2);
	//Starting point of the line
	pDC->MoveTo(0, nMiddle);
	//Draw the line
	pDC->LineTo(0, nMiddle);	
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
