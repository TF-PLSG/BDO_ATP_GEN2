// PosEntryModesListDlg.cpp : implementation file
//

#include "stdafx.h"
#include "atp.h"
#include "PosEntryModesListDlg.h"
#include "MyUtils.h"
#include <math.h>
extern "C"
{
#include "ptetcp.h"
#include "ntutils.h"
#include "Txutils.h"  
	
}

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPosEntryModesListDlg dialog


CPosEntryModesListDlg::CPosEntryModesListDlg(CWnd* pParent /*=NULL*/)
: CDialog(CPosEntryModesListDlg::IDD, pParent)
{
	
	
	//{{AFX_DATA_INIT(CPosEntryModesListDlg)
	// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	memset(pos_modes,0,20*256);
	strcpy(pos_modes[0],"0010");
	strcpy(pos_modes[1],"0011");
	strcpy(pos_modes[2],"0012");
	strcpy(pos_modes[3],"0020");
	strcpy(pos_modes[4],"0021");
	strcpy(pos_modes[5],"0022");
	strcpy(pos_modes[6],"0050");
	strcpy(pos_modes[7],"0051");
	strcpy(pos_modes[8],"0052");
	strcpy(pos_modes[9],"0800");
	strcpy(pos_modes[10],"0801");
	strcpy(pos_modes[11],"0802");
	
	memset(&newLogFont,0,sizeof(LOGFONT));
	strcpy(newLogFont.lfFaceName,"MS Sans Serif");
	newLogFont.lfWeight = 400;
	newLogFont.lfHeight = -11;
	NewFont.CreateFontIndirect(&newLogFont);
}


void CPosEntryModesListDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPosEntryModesListDlg)
	// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPosEntryModesListDlg, CDialog)
//{{AFX_MSG_MAP(CPosEntryModesListDlg)
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPosEntryModesListDlg message handlers

BOOL CPosEntryModesListDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	int i=0;
	int ret = 0;
	int k = 1;
	int base = 0;
	
	//set version info in the title bar
	CString appProdVer;
	CString lpszTitle;
	appProdVer.LoadString(IDS_PRODUCT_VERSION);
	this->GetWindowText(lpszTitle);
	lpszTitle = lpszTitle + " - Version - ";
	lpszTitle += appProdVer;
	this->SetWindowText(lpszTitle);

	
	for(i=0;i<MAX_POS_MODES;i++)
	{
		if(i>6)
		{
			k = 0;
			base = 7;
		}
		
		//int power = (int)pow(2,(i-base));
		int power = (int)pow(static_cast<double>(2),(i-base));

		CRect rect(10,20,100,20);
		m_cChkbx[i] = new CButton;
		m_cChkbx[i]->Create(pos_modes[i],WS_CHILD ,rect ,this, 1000+i);
		m_cChkbx[i]->SetButtonStyle(BS_AUTOCHECKBOX);
		
		
		m_cChkbx[i]->SetFont(&NewFont);
		m_cChkbx[i]->SetWindowPos(NULL,20,10+(i*20),200,20,SWP_SHOWWINDOW);
		m_cChkbx[i]->ShowWindow(SW_SHOW);
		if((m_PosEntryModes[k] & power) == power)
		{
			m_cChkbx[i]->SetCheck(1);
		}
		
	}
	
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

LRESULT CPosEntryModesListDlg::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	// TODO: Add your specialized code here and/or call the base class
	
	return CDialog::DefWindowProc(message, wParam, lParam);
}

void CPosEntryModesListDlg::SetPosEntryModes(unsigned char MSB, unsigned char LSB)
{
	m_PosEntryModes[0] = MSB;
	m_PosEntryModes[1] = LSB;
}

unsigned char CPosEntryModesListDlg::GetPosEntryModesMSB()
{
	return m_PosEntryModes[0];
}

unsigned char CPosEntryModesListDlg::GetPosEntryModesLSB()
{
	return m_PosEntryModes[1];
}

void CPosEntryModesListDlg::OnOK() 
{
	int i=0,k=1;
	int base = 0;
	m_PosEntryModes[0] = m_PosEntryModes[0] & 0x40;
	m_PosEntryModes[1] = m_PosEntryModes[1] & 0x00;

	for(i=0;i<MAX_POS_MODES;i++)
	{
		if(i>6)
		{
			k = 0;
			base = 7;
		}
		if(this->m_cChkbx[i]->GetCheck() == 1)
		{
			//int n = pow(2,6);
			//unsigned char power = (unsigned char)pow(2,(i-base));
			unsigned char power = (unsigned char)pow(static_cast<double>(2),(i-base));
			m_PosEntryModes[k] = m_PosEntryModes[k] | power;
		}
	}
	CDialog::OnOK();
}
