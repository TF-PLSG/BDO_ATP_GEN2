// TPSR_GUIDlg.cpp : implementation file
//

#include "stdafx.h"
#include "TPSR_GUI.h"
#include "TPSR_GUIDlg.h"
#include "tpsgraph.h"
#include "math.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTPSR_GUIDlg dialog

IMPLEMENT_DYNCREATE(CTPSR_GUIDlg, CPropertyPage)


CTPSR_GUIDlg::CTPSR_GUIDlg(CWnd* pParent /*=NULL*/)
	: CPropertyPage(CTPSR_GUIDlg::IDD)
{
	//{{AFX_DATA_INIT(CTPSR_GUIDlg)
	m_StrAmount = _T("");
	m_StrAuthIDResp = _T("");
	m_StrBitmap = _T("");
	m_StrDateLocal = _T("");
	m_StrEMVData = _T("");
	m_StrExpDate = _T("");
	m_StrNII = _T("");
	m_StrPinData = _T("");
	m_StrPosCondCode = _T("");
	m_StrPosEntryMode = _T("");
	m_StrPrivate1 = _T("");
	m_StrPrivate2 = _T("");
	m_StrPrivate3 = _T("");
	m_StrProcessingCode = _T("");
	m_StrProfileName = _T("");
	m_StrRespCode = _T("");
	m_StrRetrivalRefNbr = _T("");
	m_StrSysTrace = _T("Automatically Generated");
	m_StrTimeLocal = _T("");
	m_StrTrack1Data = _T("");
	m_StrTrack2Data = _T("");
	proc = NULL;
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDI_TPSRICON);

	memset(m_isoBitmapByte,0,17);
	memset(m_isoBitmapBits,0,65);
	m_isoBitmapBits[0]  = 0;
	m_isoBitmapBits[1]  = 1;	//Card No
	m_isoBitmapBits[2]  = 1;	//Processing Code
	m_isoBitmapBits[3]  = 1;	//Txn Amount
	m_isoBitmapBits[4]  = 0;	//
	m_isoBitmapBits[5]  = 0;
	m_isoBitmapBits[6]  = 0;
	m_isoBitmapBits[7]  = 0;
	m_isoBitmapBits[8]  = 0;
	m_isoBitmapBits[9]  = 0;
	m_isoBitmapBits[10] = 1;	//System Trace No - STAN
	m_isoBitmapBits[11] = 1;	//Txn Time
	m_isoBitmapBits[12] = 1;	//Txn Date
	m_isoBitmapBits[13] = 0;	
	m_isoBitmapBits[14] = 0;
	m_isoBitmapBits[15] = 0;
	m_isoBitmapBits[16] = 0;
	m_isoBitmapBits[17] = 0;
	m_isoBitmapBits[18] = 0;
	m_isoBitmapBits[19] = 0;
	m_isoBitmapBits[20] = 0;
	m_isoBitmapBits[21] = 1;	//Pos Entry Mode
	m_isoBitmapBits[22] = 0;	
	m_isoBitmapBits[23] = 1;	//NII
	m_isoBitmapBits[24] = 1;	//Pos Condition Code
	m_isoBitmapBits[25] = 0;
	m_isoBitmapBits[26] = 0;
	m_isoBitmapBits[27] = 0;
	m_isoBitmapBits[28] = 0;
	m_isoBitmapBits[29] = 0;
	m_isoBitmapBits[30] = 0;
	m_isoBitmapBits[31] = 0;
	m_isoBitmapBits[32] = 0;
	m_isoBitmapBits[33] = 0;
	m_isoBitmapBits[34] = 0;
	m_isoBitmapBits[35] = 0;
	m_isoBitmapBits[36] = 0;
	m_isoBitmapBits[37] = 1;	//Auth ID
	m_isoBitmapBits[38] = 0;
	m_isoBitmapBits[39] = 0;
	m_isoBitmapBits[40] = 1;	//Terminal ID
	m_isoBitmapBits[41] = 1;	//merchant Id
	m_isoBitmapBits[42] = 0;
	m_isoBitmapBits[43] = 0;
	m_isoBitmapBits[44] = 0;
	m_isoBitmapBits[45] = 0;
	m_isoBitmapBits[46] = 0;
	m_isoBitmapBits[47] = 0;
	m_isoBitmapBits[48] = 0;
	m_isoBitmapBits[49] = 0;
	m_isoBitmapBits[50] = 0;
	m_isoBitmapBits[51] = 0;
	m_isoBitmapBits[52] = 0;
	m_isoBitmapBits[53] = 0;
	m_isoBitmapBits[54] = 0;
	m_isoBitmapBits[55] = 0;
	m_isoBitmapBits[56] = 0;
	m_isoBitmapBits[57] = 0;
	m_isoBitmapBits[58] = 0;
	m_isoBitmapBits[59] = 0;
	m_isoBitmapBits[60] = 0;	
	m_isoBitmapBits[61] = 1;	//Field 62
	m_isoBitmapBits[62] = 1;	//Field 63
	m_isoBitmapBits[63] = 0;
	m_isoBitmapBits[64] = 0;
	m_isoBitmapBits[65] = 0;
}

void CTPSR_GUIDlg::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTPSR_GUIDlg)
	DDX_Control(pDX, IDC_TIME_LOCAL_CHECK, m_CntlTimeLocalCheck);
	DDX_Control(pDX, IDC_RETRIVAL_REF_NBR_CHECK, m_CntlRetrivalRefNbrCheck);
	DDX_Control(pDX, IDC_RESP_CODE_CHECK, m_CntlRespCodeCheck);
	DDX_Control(pDX, IDC_PIN_DATA_CHECK, m_CntlPinDataCheck);
	DDX_Control(pDX, IDC_DATE_LOCAL_CHECK, m_CntlDateLocalCheck);
	DDX_Control(pDX, IDC_AUTH_ID_RESP_CHECK, m_CntlAuthIDRespCheck);
	DDX_Control(pDX, IDRESET, m_BtnReset);
	DDX_Control(pDX, IDOK, m_BtnSave);
	DDX_Control(pDX, IDC_MSG_TYPE, m_CntlMsgType);
	DDX_Control(pDX, IDC_TRACK2_DATA, m_CntlTrack2Data);
	DDX_Control(pDX, IDC_TRACK1_DATA, m_CntlTrack1Data);
	DDX_Control(pDX, IDC_TIME_LOCAL, m_CntlTimeLocal);
	DDX_Control(pDX, IDC_SYS_TRACE_NBR, m_CntlSysTrace);
	DDX_Control(pDX, IDC_RETRIVAL_REF_NBR, m_CntlRetrivalRefNbr);
	DDX_Control(pDX, IDC_RESP_CODE, m_CntlRespCode);
	DDX_Control(pDX, IDC_PROFILE_NAME, m_CntlProfileName);
	DDX_Control(pDX, IDC_PROCESSING_CODE, m_CntlProcessingCode);
	DDX_Control(pDX, IDC_PRIVATE3, m_CntlPrivate3);
	DDX_Control(pDX, IDC_PRIVATE2, m_CntlPrivate2);
	DDX_Control(pDX, IDC_PRIVATE1, m_CntlPrivate1);
	DDX_Control(pDX, IDC_POS_ENTRY_MODE, m_CntlPosEntryMode);
	DDX_Control(pDX, IDC_POS_COND_CODE, m_CntlPosCondCode);
	DDX_Control(pDX, IDC_PIN_DATA, m_CntlPinData);
	DDX_Control(pDX, IDC_NII, m_CntlNII);
	DDX_Control(pDX, IDC_EXP_DATE, m_CntlExpDate);
	DDX_Control(pDX, IDC_EMV_DATA, m_CntlEMVData);
	DDX_Control(pDX, IDC_DATE_LOCAL, m_CntlDateLocal);
	DDX_Control(pDX, IDC_BITMAP, m_CntlBitmap);
	DDX_Control(pDX, IDC_AUTH_ID_RESP, m_CntlAuthIDResp);
	DDX_Control(pDX, IDC_AMOUNT, m_CntlAmount);
	DDX_Text(pDX, IDC_AMOUNT, m_StrAmount);
	DDV_MaxChars(pDX, m_StrAmount, 12);
	DDX_Text(pDX, IDC_AUTH_ID_RESP, m_StrAuthIDResp);
	DDV_MaxChars(pDX, m_StrAuthIDResp, 6);
	DDX_Text(pDX, IDC_BITMAP, m_StrBitmap);
	DDX_Text(pDX, IDC_DATE_LOCAL, m_StrDateLocal);
	DDV_MaxChars(pDX, m_StrDateLocal, 4);
	DDX_Text(pDX, IDC_EMV_DATA, m_StrEMVData);
	DDX_Text(pDX, IDC_EXP_DATE, m_StrExpDate);
	DDV_MaxChars(pDX, m_StrExpDate, 4);
	DDX_Text(pDX, IDC_NII, m_StrNII);
	DDV_MaxChars(pDX, m_StrNII, 4);
	DDX_Text(pDX, IDC_PIN_DATA, m_StrPinData);
	DDV_MaxChars(pDX, m_StrPinData, 16);
	DDX_Text(pDX, IDC_POS_COND_CODE, m_StrPosCondCode);
	DDV_MaxChars(pDX, m_StrPosCondCode, 2);
	DDX_Text(pDX, IDC_POS_ENTRY_MODE, m_StrPosEntryMode);
	DDV_MaxChars(pDX, m_StrPosEntryMode, 4);
	DDX_Text(pDX, IDC_PRIVATE1, m_StrPrivate1);
	DDX_Text(pDX, IDC_PRIVATE2, m_StrPrivate2);
	DDX_Text(pDX, IDC_PRIVATE3, m_StrPrivate3);
	DDX_Text(pDX, IDC_PROCESSING_CODE, m_StrProcessingCode);
	DDV_MaxChars(pDX, m_StrProcessingCode, 6);
	DDX_Text(pDX, IDC_PROFILE_NAME, m_StrProfileName);
	DDV_MaxChars(pDX, m_StrProfileName, 32);
	DDX_Text(pDX, IDC_RESP_CODE, m_StrRespCode);
	DDV_MaxChars(pDX, m_StrRespCode, 2);
	DDX_Text(pDX, IDC_RETRIVAL_REF_NBR, m_StrRetrivalRefNbr);
	DDV_MaxChars(pDX, m_StrRetrivalRefNbr, 12);
	DDX_Text(pDX, IDC_SYS_TRACE_NBR, m_StrSysTrace);
	DDV_MaxChars(pDX, m_StrSysTrace, 6);
	DDX_Text(pDX, IDC_TIME_LOCAL, m_StrTimeLocal);
	DDV_MaxChars(pDX, m_StrTimeLocal, 6);
	DDX_Text(pDX, IDC_TRACK1_DATA, m_StrTrack1Data);
	DDV_MaxChars(pDX, m_StrTrack1Data, 25);
	DDX_Text(pDX, IDC_TRACK2_DATA, m_StrTrack2Data);
	DDV_MaxChars(pDX, m_StrTrack2Data, 25);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CTPSR_GUIDlg, CPropertyPage)
	//{{AFX_MSG_MAP(CTPSR_GUIDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_TIME_LOCAL_CHECK, OnTimeLocalCheck)
	ON_BN_CLICKED(IDC_DATE_LOCAL_CHECK, OnDateLocalCheck)
	ON_BN_CLICKED(IDC_RETRIVAL_REF_NBR_CHECK, OnRetrivalRefNbrCheck)
	ON_BN_CLICKED(IDC_AUTH_ID_RESP_CHECK, OnAuthIdRespCheck)
	ON_BN_CLICKED(IDC_RESP_CODE_CHECK, OnRespCodeCheck)
	ON_BN_CLICKED(IDC_TRACK1_DATA_CHECK, OnTrack1DataCheck)
	ON_BN_CLICKED(IDC_TRACK2_DATA_CHECK, OnTrack2DataCheck)
	ON_BN_CLICKED(IDC_PIN_DATA_CHECK, OnPinDataCheck)
	ON_BN_CLICKED(IDC_EMV_DATA_CHECK, OnEmvDataCheck)
	ON_BN_CLICKED(IDC_PRIVATE1_CHECK, OnPrivate1Check)
	ON_BN_CLICKED(IDC_PRIVATE2_CHECK, OnPrivate2Check)
	ON_BN_CLICKED(IDC_PRIVATE3_CHECK, OnPrivate3Check)
	ON_EN_SETFOCUS(IDC_PROFILE_NAME, OnSetfocusProfileName)
	ON_BN_CLICKED(IDRESET, OnReset)
	ON_EN_KILLFOCUS(IDC_CARD_NBR_LIST_FILE,		OnKillfocusCardNbrList)
	ON_EN_KILLFOCUS(IDC_DEVICE_NBR_FILE_LIST,	OnKillfocusDeviceNbrList)
	ON_EN_KILLFOCUS(IDC_AMOUNT,					OnKillfocusAmount)
	ON_EN_KILLFOCUS(IDC_AUTH_ID_RESP,			OnKillfocusAuthIDResp)
	ON_EN_KILLFOCUS(IDC_BITMAP,					OnKillfocusBitmap)
	ON_EN_KILLFOCUS(IDC_DATE_LOCAL,				OnKillfocusDateLocal)
	ON_EN_KILLFOCUS(IDC_EXP_DATE,				OnKillfocusExpDate)
	ON_EN_KILLFOCUS(IDC_NII,					OnKillfocusNII)
	ON_EN_KILLFOCUS(IDC_PIN_DATA,				OnKillfocusPinData)
	ON_EN_KILLFOCUS(IDC_POS_COND_CODE,			OnKillfocusPosCondCode)
	ON_EN_KILLFOCUS(IDC_POS_ENTRY_MODE,			OnKillfocusPosEntryMode)
	ON_EN_KILLFOCUS(IDC_PROCESSING_CODE,		OnKillfocusProcessingCode)
	ON_EN_KILLFOCUS(IDC_PROFILE_NAME,			OnKillfocusProfileName)
	ON_EN_KILLFOCUS(IDC_RESP_CODE,				OnKillfocusRespCode)
	ON_EN_KILLFOCUS(IDC_RETRIVAL_REF_NBR,		OnKillfocusRetrivalRefNbr)
	ON_EN_KILLFOCUS(IDC_SYS_TRACE_NBR,			OnKillfocusSysTrace)
	ON_EN_KILLFOCUS(IDC_TIME_LOCAL,				OnKillfocusTimeLocal)
	ON_EN_KILLFOCUS(IDC_TRACK1_DATA,			OnKillfocusTrack1Data)
	ON_EN_KILLFOCUS(IDC_TRACK2_DATA,			OnKillfocusTrack2Data)

	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTPSR_GUIDlg message handlers

BOOL CTPSR_GUIDlg::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

/*	m_CntlCardType.Clear();
	m_CntlCardType.AddString("VISA");
	m_CntlCardType.AddString("MCARD");
	m_CntlCardType.AddString("AMEX");
	m_CntlCardType.AddString("JCB");
	m_CntlCardType.SetCurSel(0);
*/
	m_CntlMsgType.Clear();
	m_CntlMsgType.AddString("0100 [Authorization]");
	m_CntlMsgType.AddString("0200 [Sale]");
	m_CntlMsgType.AddString("0400 [Reversal]");
	m_CntlMsgType.AddString("0500 [Settlement Request]");
	m_CntlMsgType.SetCurSel(0);

	m_CntlProfileName.SetWindowText("<Enter a profile name>");

	m_hSave		= LoadBitmap(AfxGetApp()->m_hInstance,MAKEINTRESOURCE(IDB_SAVE));
	m_hReset	= LoadBitmap(AfxGetApp()->m_hInstance,MAKEINTRESOURCE(IDB_RESET));
	m_hCancel	= LoadBitmap(AfxGetApp()->m_hInstance,MAKEINTRESOURCE(IDB_REMOVE));

	m_BtnSave.SetBitmap(m_hSave);
	m_BtnReset.SetBitmap(m_hReset);

	m_BtnSave.SetToolTipText("Save Profile",TRUE);
	m_BtnReset.SetToolTipText("Restore Defaults",TRUE);

	// TODO: Add extra initialization here
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.

void CTPSR_GUIDlg::OnBrowseCardFile() 
{
}

void CTPSR_GUIDlg::OnBrowseDeviceFile2() 
{
}

void CTPSR_GUIDlg::OnTimeLocalCheck() 
{
	int check = m_CntlTimeLocalCheck.GetCheck();
	m_CntlTimeLocal.EnableWindow(check);
	::EnableWindow(GetDlgItem(IDC_TIME_LOCAL_STATIC)->m_hWnd,check);
	m_isoBitmapBits[12] = check;
}

void CTPSR_GUIDlg::OnDateLocalCheck() 
{
	int check = m_CntlDateLocalCheck.GetCheck();
	m_CntlDateLocal.EnableWindow(check);
	::EnableWindow(GetDlgItem(IDC_DATE_LOCAL_STATIC)->m_hWnd,check);
	m_isoBitmapBits[13] = check;
}

void CTPSR_GUIDlg::OnRetrivalRefNbrCheck() 
{
	int check = m_CntlRetrivalRefNbrCheck.GetCheck();
	m_CntlRetrivalRefNbr.EnableWindow(check);
	::EnableWindow(GetDlgItem(IDC_RETRIVAL_REF_STATIC)->m_hWnd,check);
	m_isoBitmapBits[37] = check;
}

void CTPSR_GUIDlg::OnAuthIdRespCheck() 
{
	int check = m_CntlAuthIDRespCheck.GetCheck();
	m_CntlAuthIDResp.EnableWindow(check);
	::EnableWindow(GetDlgItem(IDC_AUTH_ID_RESP_STATIC)->m_hWnd,check);
	m_isoBitmapBits[38] = check;
}

void CTPSR_GUIDlg::OnRespCodeCheck() 
{
	int check = m_CntlRespCodeCheck.GetCheck();
	m_CntlRespCode.EnableWindow(check);
	::EnableWindow(GetDlgItem(IDC_RESP_CODE_STATIC)->m_hWnd,check);
	m_isoBitmapBits[39] = check;
}

void CTPSR_GUIDlg::OnTrack1DataCheck() 
{
	int check = m_CntlTrack1DataCheck.GetCheck();
	m_CntlTrack1Data.EnableWindow(check);
	::EnableWindow(GetDlgItem(IDC_TRACK1_STATIC)->m_hWnd,check);
	m_isoBitmapBits[45] = check;	
}

void CTPSR_GUIDlg::OnTrack2DataCheck() 
{
	int check = m_CntlTrack2DataCheck.GetCheck();
	m_CntlTrack2Data.EnableWindow(check);
	::EnableWindow(GetDlgItem(IDC_TRACK2_STATIC)->m_hWnd,check);
	m_isoBitmapBits[35] = check;
}

void CTPSR_GUIDlg::OnPinDataCheck() 
{
	int check = m_CntlPinDataCheck.GetCheck();
	m_CntlPinData.EnableWindow(check);
	::EnableWindow(GetDlgItem(IDC_PIN_DATA_STATIC)->m_hWnd,check);
	m_isoBitmapBits[52] = check;
}

void CTPSR_GUIDlg::OnEmvDataCheck() 
{
	int check = m_CntlEMVDataCheck.GetCheck();
	m_CntlEMVData.EnableWindow(check);
	::EnableWindow(GetDlgItem(IDC_EMV_STATIC)->m_hWnd,check);
	m_isoBitmapBits[55] = check;
}

void CTPSR_GUIDlg::OnPrivate1Check() 
{
	int check = m_CntlPrivate1Check.GetCheck();
	m_CntlPrivate1.EnableWindow(check);
	::EnableWindow(GetDlgItem(IDC_PRIVATE1_STATIC)->m_hWnd,check);
	m_isoBitmapBits[60] = check;
}

void CTPSR_GUIDlg::OnPrivate2Check() 
{
	int check = m_CntlPrivate2Check.GetCheck();
	m_CntlPrivate2.EnableWindow(check);
	::EnableWindow(GetDlgItem(IDC_PRIVATE2_STATIC)->m_hWnd,check);
	m_isoBitmapBits[62] = check;
}

void CTPSR_GUIDlg::OnPrivate3Check() 
{
	int check = m_CntlPrivate3Check.GetCheck();
	m_CntlPrivate3.EnableWindow(check);
	::EnableWindow(GetDlgItem(IDC_PRIVATE3_STATIC)->m_hWnd,check);
	m_isoBitmapBits[63] = check;
}

void CTPSR_GUIDlg::OnOK() 
{
	int stanInt = 123456;
	char tempStr[3] = {0};
	CStatusSplashWindow splash;
	UpdateData(TRUE);
	m_StrBitmap.Empty();
	char txnContent[1024] = {0};
	
	UpdateData(FALSE);

	
	//Validate Transaction name

	CFile transactionFile;
	CFileStatus fileStatus;
	CString lpszFileName = m_StrProfileName+".flw";
	if(CFile::GetStatus(m_StrProfileName,fileStatus))
	{
		AfxMessageBox("The Transaction already exists");
		return;
	}
	else
	{
	//	if(!transactionFile.Open(m_StrProfileName,CFile::modeCreate|CFile::modeWrite))
		{
	//		AfxMessageBox("Cannot create the transaction");
	//		return;
		}
	}

	//Put Message Type
	int msgType = m_CntlMsgType.GetCurSel();
	CString selText;
	switch(msgType)
	{
	case 0:
		selText = "0100";
		break;
	case 1:
		selText = "0200";
		break;
	case 2:
		selText = "0400";
		break;
	case 3:
		selText = "0500";
		break;
	default:
		AfxMessageBox("Internal Error");
		return;
	}
	strcpy(txnContent,selText);
	
	
	//Validate Processing Code

	if(m_StrProcessingCode.IsEmpty())
	{
		AfxMessageBox("Provide a value for Processing Code");
		return;
	}
	if(m_StrAmount.IsEmpty())
	{
		AfxMessageBox("Provide a value for Amount");
		return;
	}
	if(m_StrPosEntryMode.IsEmpty())
	{
		AfxMessageBox("Provide a value for Pos Entry Mode");
		return;
	}
	if(m_StrNII.IsEmpty())
	{
		AfxMessageBox("Provide a value for NII");
		return;
	}

	if(m_StrPosCondCode.IsEmpty())
	{
		AfxMessageBox("Provide a value for Pos Cond Code");
		return;
	}

	// Validate Time Local
	if(m_CntlTimeLocalCheck.GetCheck())
	{
		int value = 0;
		char *str = m_StrTimeLocal.GetBuffer(6);
		char data[3]={0};
		sprintf(data,"%c%c",str[0],str[1]);
		value = atoi(data);
		if(value > 23)
		{
			AfxMessageBox("Enter correct hours value");
			return;
		}
		sprintf(data,"%c%c",str[2],str[3]);
		value = atoi(data);
		if(value > 59)
		{
			AfxMessageBox("Enter correct minutes value");
			return;
		}
		sprintf(data,"%c%c",str[4],str[5]);
		value = atoi(data);
		if(value > 59)
		{
			AfxMessageBox("Enter correct seconds value");
			return;
		}
		m_isoBitmapBits[11] = 1;
	}
	else
	{
		m_isoBitmapBits[11] = 0;
	}

	if(m_CntlDateLocalCheck.GetCheck())
	{
		int value = 0;
		char *str = m_StrDateLocal.GetBuffer(4);
		char data[3]={0};
		sprintf(data,"%c%c",str[0],str[1]);
		value = atoi(data);
		if(value > 12)
		{
			AfxMessageBox("Enter correct month value");
			return;
		}
		sprintf(data,"%c%c",str[2],str[3]);
		value = atoi(data);
		if(value > 31)
		{
			AfxMessageBox("Enter correct date value");
			return;
		}
		m_isoBitmapBits[12] = 1;
	}
	else
	{
		m_isoBitmapBits[12] = 0;
	}

	if(m_CntlRetrivalRefNbrCheck.GetCheck())
	{
		m_isoBitmapBits[36] = 1;
	}
	else
	{
		m_isoBitmapBits[36] = 0;
	}

	if(m_CntlAuthIDRespCheck.GetCheck())
	{
		m_isoBitmapBits[37] = 1;
	}
	else
	{
		m_isoBitmapBits[37] = 0;
	}

	if(m_CntlRespCodeCheck.GetCheck())
	{
		m_isoBitmapBits[38] = 1;
	}
	else
	{
		m_isoBitmapBits[38] = 0;
	}

	if(m_CntlPinDataCheck.GetCheck())
	{
		m_isoBitmapBits[51] = 1;
	}
	else
	{
		m_isoBitmapBits[51] = 0;
	}
	
	//Calculate BitMap
	



	for(int k=0;k<16;k++)
	{
		int temp=0;
		for(int i=0;i<4;i++)
		{
			int index = ((4*(k+1))-1) - i;
			temp += ((int)pow(2,i)) * m_isoBitmapBits[index];
		}
		m_isoBitmapByte[k] = (char) temp;
		sprintf(tempStr,"%X",temp);
		m_StrBitmap += tempStr;
	}
	

}

void CTPSR_GUIDlg::OnSetfocusProfileName() 
{

}

void CTPSR_GUIDlg::OnReset() 
{
	if(proc != NULL)
	{
		proc->PostThreadMessage(WM_MSG1, 0,0);
	}

	m_StrAmount			= "";
	m_StrAuthIDResp		= "";
	m_StrBitmap			= "";
	m_StrDateLocal		= "";
	m_StrEMVData		= "";
	m_StrExpDate		= "";
	m_StrNII			= "";
	m_StrPinData		= "";
	m_StrPosCondCode	= "";
	m_StrPosEntryMode	= "";
	m_StrPrivate1		= "";
	m_StrPrivate2		= "";
	m_StrPrivate3		= "";
	m_StrProcessingCode	= "";
	m_StrProfileName	= "<ENTER A PROFILE NAME>";
	m_StrRespCode		= "";
	m_StrRetrivalRefNbr	= "";
	m_StrSysTrace		= "";
	m_StrTimeLocal		= "";
	m_StrTrack1Data		= "";
	m_StrTrack2Data		= "";
	
//	m_CntlCardType.SetCurSel(0);
	m_CntlMsgType.SetCurSel(0); 

	UpdateData(FALSE);
}

void CTPSR_GUIDlg::OnKillfocusCardNbrList()
{
	UpdateData(TRUE);
}
void CTPSR_GUIDlg::OnKillfocusDeviceNbrList()
{
	UpdateData(TRUE);
}
void CTPSR_GUIDlg::OnKillfocusAmount()
{
	UpdateData(TRUE);
	char str[13] = {0};
	if(m_StrAmount.GetLength() < 12)
	{
		for(int i= 0;i<=11 - m_StrAmount.GetLength(); i++)
		{
			str[i] = '0';
		}
		str[i] = '\0';
		strcat(str,m_StrAmount);
		m_StrAmount = str;
		UpdateData(FALSE);
	}
	

}
void CTPSR_GUIDlg::OnKillfocusAuthIDResp()
{
	UpdateData(TRUE);
	char str[7] = {0};
	if(m_StrAuthIDResp.GetLength() < 6)
	{
		for(int i= 0;i<=5 - m_StrAuthIDResp.GetLength(); i++)
		{
			str[i] = '0';
		}
		str[i] = '\0';
		strcat(str,m_StrAuthIDResp);
		m_StrAuthIDResp = str;
		UpdateData(FALSE);
	}

}
void CTPSR_GUIDlg::OnKillfocusBitmap()
{
	UpdateData(TRUE);
}
void CTPSR_GUIDlg::OnKillfocusDateLocal()
{
	UpdateData(TRUE);
	char str[5] = {0};
	if(m_StrDateLocal.GetLength() < 4)
	{
		for(int i= 0;i<=3 - m_StrDateLocal.GetLength(); i++)
		{
			str[i] = '0';
		}
		str[i] = '\0';
		strcat(str,m_StrDateLocal);
		m_StrDateLocal = str;
		UpdateData(FALSE);
	}
}
void CTPSR_GUIDlg::OnKillfocusExpDate()
{
	UpdateData(TRUE);
}
void CTPSR_GUIDlg::OnKillfocusNII()
{
	UpdateData(TRUE);
	char str[5] = {0};
	if(m_StrNII.GetLength() < 4)
	{
		for(int i= 0;i<=3 - m_StrNII.GetLength(); i++)
		{
			str[i] = '0';
		}
		str[i] = '\0';
		strcat(str,m_StrNII);
		m_StrNII = str;
		UpdateData(FALSE);
	}
}
void CTPSR_GUIDlg::OnKillfocusPinData()
{
	UpdateData(TRUE);
	char str[17] = {0};
	if(m_StrPinData.GetLength() < 16)
	{
		for(int i= 0;i<=15 - m_StrPinData.GetLength(); i++)
		{
			str[i] = '0';
		}
		str[i] = '\0';
		strcat(str,m_StrPinData);
		m_StrPinData = str;
		UpdateData(FALSE);
	}
}
void CTPSR_GUIDlg::OnKillfocusPosCondCode()
{
	UpdateData(TRUE);
	char str[3] = {0};
	if(m_StrPosCondCode.GetLength() < 2)
	{
		for(int i= 0;i<=1 - m_StrPosCondCode.GetLength(); i++)
		{
			str[i] = '0';
		}
		str[i] = '\0';
		strcat(str,m_StrPosCondCode);
		m_StrPosCondCode = str;
		UpdateData(FALSE);
	}
}
void CTPSR_GUIDlg::OnKillfocusPosEntryMode()
{
	UpdateData(TRUE);
	char str[5] = {0};
	if(m_StrPosEntryMode.GetLength() < 4)
	{
		for(int i= 0;i<=3 - m_StrPosEntryMode.GetLength(); i++)
		{
			str[i] = '0';
		}
		str[i] = '\0';
		strcat(str,m_StrPosEntryMode);
		m_StrPosEntryMode = str;
		UpdateData(FALSE);
	}
}
void CTPSR_GUIDlg::OnKillfocusProcessingCode()
{
	UpdateData(TRUE);
	char str[7] = {0};
	if(m_StrProcessingCode.GetLength() < 6)
	{
		for(int i= 0;i<=5 - m_StrProcessingCode.GetLength(); i++)
		{
			str[i] = '0';
		}
		str[i] = '\0';
		strcat(str,m_StrProcessingCode);
		m_StrProcessingCode = str;
		UpdateData(FALSE);
	}
}
void CTPSR_GUIDlg::OnKillfocusProfileName()
{
	UpdateData(TRUE);
	if(m_StrProfileName.IsEmpty())
	{
		m_StrProfileName = "<Enter a profile name>";
		UpdateData(FALSE);
	}
}
void CTPSR_GUIDlg::OnKillfocusRespCode()
{
	UpdateData(TRUE);
	char str[3] = {0};
	if(m_StrRespCode.GetLength() < 2)
	{
		for(int i= 0;i<=1 - m_StrRespCode.GetLength(); i++)
		{
			str[i] = '0';
		}
		str[i] = '\0';
		strcat(str,m_StrRespCode);
		m_StrRespCode = str;
		UpdateData(FALSE);
	}
}
void CTPSR_GUIDlg::OnKillfocusRetrivalRefNbr()
{
	UpdateData(TRUE);
	char str[13] = {0};
	if(m_StrRetrivalRefNbr.GetLength() < 12)
	{
		for(int i= 0;i<=11 - m_StrRetrivalRefNbr.GetLength(); i++)
		{
			str[i] = '0';
		}
		str[i] = '\0';
		strcat(str,m_StrRetrivalRefNbr);
		m_StrRetrivalRefNbr = str;
		UpdateData(FALSE);
	}
}
void CTPSR_GUIDlg::OnKillfocusSysTrace()
{
	UpdateData(TRUE);
	char str[7] = {0};
	if(m_StrSysTrace.GetLength() < 6)
	{
		for(int i= 0;i<=5 - m_StrSysTrace.GetLength(); i++)
		{
			str[i] = '0';
		}
		str[i] = '\0';
		strcat(str,m_StrSysTrace);
		m_StrSysTrace = str;
		UpdateData(FALSE);
	}

}
void CTPSR_GUIDlg::OnKillfocusTimeLocal()
{
	UpdateData(TRUE);

	
}
void CTPSR_GUIDlg::OnKillfocusTrack1Data()
{
	UpdateData(TRUE);
	char str[26] = {0};
	if(m_StrTrack1Data.GetLength() < 25)
	{
		for(int i= 0;i<=24 - m_StrTrack1Data.GetLength(); i++)
		{
			str[i] = '0';
		}
		str[i] = '\0';
		strcat(str,m_StrTrack1Data);
		m_StrTrack1Data = str;
		UpdateData(FALSE);
	}
}
void CTPSR_GUIDlg::OnKillfocusTrack2Data()
{
	UpdateData(TRUE);
	char str[26] = {0};
	if(m_StrTrack2Data.GetLength() < 25)
	{
		for(int i= 0;i<=24 - m_StrTrack2Data.GetLength(); i++)
		{
			str[i] = '0';
		}
		str[i] = '\0';
		strcat(str,m_StrTrack2Data);
		m_StrTrack2Data = str;
		UpdateData(FALSE);
	}
}


BOOL CTPSR_GUIDlg::OnApply() 
{
	// TODO: Add your specialized code here and/or call the base class
	
	return CPropertyPage::OnApply();
}
