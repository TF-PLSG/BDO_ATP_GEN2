// CardMerchantDevDlg.cpp : implementation file
//

#include "stdafx.h"
#include "TPSR_GUI.h"
#include "CardMerchantDevDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCardMerchantDevDlg dialog

IMPLEMENT_DYNCREATE(CCardMerchantDevDlg, CPropertyPage)

CCardMerchantDevDlg::CCardMerchantDevDlg(CWnd* pParent /*=NULL*/)
	: CPropertyPage(CCardMerchantDevDlg::IDD)
{
	//{{AFX_DATA_INIT(CCardMerchantDevDlg)
	m_StrCardNbrListFile = _T("");
	m_StrDeviceNbrListFile = _T("");
	//}}AFX_DATA_INIT
}


void CCardMerchantDevDlg::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCardMerchantDevDlg)
	DDX_Control(pDX, IDC_DEVICE_NBR_FILE_LIST, m_CntlDeviceNbrListFile);
	DDX_Control(pDX, IDC_CARD_NBR_LIST_FILE, m_CntlCardNbrListFile);
	DDX_Text(pDX, IDC_CARD_NBR_LIST_FILE, m_StrCardNbrListFile);
	DDX_Text(pDX, IDC_DEVICE_NBR_FILE_LIST, m_StrDeviceNbrListFile);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CCardMerchantDevDlg, CPropertyPage)
	//{{AFX_MSG_MAP(CCardMerchantDevDlg)
	ON_BN_CLICKED(IDC_BROWSE_CARD_FILE, OnBrowseCardFile)
	ON_BN_CLICKED(IDC_BROWSE_DEVICE_FILE, OnBrowseDeviceFile)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCardMerchantDevDlg message handlers

void CCardMerchantDevDlg::OnBrowseCardFile() 
{
	char strFilter[] = { "CARD list Files (*.cdl)|*.cdl|Text (*.txt)|*.txt|All Files (*.*)|*.*||" };
	CFileDialog ofd(TRUE, ".cdl",NULL,0,strFilter);
	CString filename;

	if(ofd.DoModal() == IDOK)
	{
		m_StrCardNbrListFile = ofd.GetPathName();
		UpdateData(FALSE);
	}
	
}

void CCardMerchantDevDlg::OnBrowseDeviceFile() 
{
	char strFilter[] = { "Device Merchant list (*.dml)|*.dml|Text (*.txt)|*.txt|All Files (*.*)|*.*||" };
	CFileDialog ofd(TRUE, ".dml",NULL,0,strFilter);

	if(ofd.DoModal() == IDOK)
	{
		m_StrDeviceNbrListFile = ofd.GetPathName();
		UpdateData(FALSE);
	}	
}

BOOL CCardMerchantDevDlg::OnApply() 
{
	UpdateData(TRUE);
	if(m_StrCardNbrListFile.IsEmpty())
	{
		AfxMessageBox("Please provide a card numbers list file");
		return false;
	}
	if(m_StrDeviceNbrListFile.IsEmpty())
	{
		AfxMessageBox("Please provide a Device Merchant ids list file");
		return false;
	}

	FILE *cardFPtr = fopen(m_StrCardNbrListFile,"r");
	if(cardFPtr == NULL)
	{
		AfxMessageBox("Card numbers list file does not exist");
		return false;
	}
	FILE *devFPtr = fopen(m_StrDeviceNbrListFile,"r");
	if(devFPtr  == NULL)
	{
		AfxMessageBox("Device Merchant ids list file does not exist");
		return false;
	}

	return CPropertyPage::OnApply();
}
