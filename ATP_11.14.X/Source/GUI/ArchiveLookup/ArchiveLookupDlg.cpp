// ArchiveLookupDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ArchiveLookup.h"
#include "ArchiveLookupDlg.h"
#include "TransactionDetails.h"

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
// CArchiveLookupDlg dialog

CArchiveLookupDlg::~CArchiveLookupDlg()
{

}

CArchiveLookupDlg::CArchiveLookupDlg(CWnd* pParent /*=NULL*/)
: CDialog(CArchiveLookupDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CArchiveLookupDlg)
	m_szAuthResponseID = _T("");
	m_szCardNbr = _T("");
	m_szEndDate = _T("");
	m_szMerchantID = _T("");
	m_szOperatorID = _T("");
	m_szStartDate = _T("");
	m_szTerminalID = _T("");
	m_szStatus = _T("");

	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	bSearching = false;
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	
	m_pGetTLF = NULL;
	CWinThread *thr = AfxBeginThread(RUNTIME_CLASS(CGetTLFData),THREAD_PRIORITY_NORMAL,0,CREATE_SUSPENDED);
	m_pGetTLF = dynamic_cast<CGetTLFData *>(thr);
	

	arrColumnNames.Add("TRANSACTION ID");
	arrColumnNames.Add("ENTRY TYPE");
	arrColumnNames.Add("TPDU ID");
	arrColumnNames.Add("ISSUER ID");
	arrColumnNames.Add("ACQUIRER ID");
	arrColumnNames.Add("TX KEY");
	arrColumnNames.Add("TERMINAL ID");
	arrColumnNames.Add("TERMINAL TYPE");
	arrColumnNames.Add("RESPONSE TEXT");
	arrColumnNames.Add("CARD NUM LEN");
	arrColumnNames.Add("CARD NUM");
	arrColumnNames.Add("EXP DATE");
	arrColumnNames.Add("TOTAL AMOUNT");
	arrColumnNames.Add("ADD AMOUNTS");
	arrColumnNames.Add("INVOICE NUMBER");
	arrColumnNames.Add("ORIG AMOUNT");
	arrColumnNames.Add("AUTH NUMBER");
	arrColumnNames.Add("PROCESSING CODE");
	arrColumnNames.Add("SYS TRACE AUDIT NUM");
	arrColumnNames.Add("ORIG SYS TRACE NUM");
	arrColumnNames.Add("POS ENTRY MODE");
	arrColumnNames.Add("NII");
	arrColumnNames.Add("POS CONDITION CODE");
	arrColumnNames.Add("TRACK1");
	arrColumnNames.Add("TRACK2");
	arrColumnNames.Add("RETRIEVAL REF NUM");
	arrColumnNames.Add("ORIG RETRIEVAL REF NUM");
	arrColumnNames.Add("MERCHANT ID");
	arrColumnNames.Add("ORIG MESSAGE");
	arrColumnNames.Add("SETTLEMENT TOTAL");
	arrColumnNames.Add("MESSAGE TYPE");
	arrColumnNames.Add("CARD HOLDER NAME");
	arrColumnNames.Add("GENERAL STATUS");
	arrColumnNames.Add("ORIGINATOR QUEUE");
	arrColumnNames.Add("ORIGINATOR INFO");
	arrColumnNames.Add("BATCH NUMBER");
	arrColumnNames.Add("RESPONSE CODE");
	arrColumnNames.Add("DATE YYYYMMDD");
	arrColumnNames.Add("TIME HHMMSS");
	arrColumnNames.Add("NUM SALES");
	arrColumnNames.Add("SALES AMOUNT");
	arrColumnNames.Add("NUM REFUNDS");
	arrColumnNames.Add("REFUND AMOUNT");
	arrColumnNames.Add("NUM DBSALES");
	arrColumnNames.Add("DBSALES AMOUNT");
	arrColumnNames.Add("NUM DBREFUNDS");
	arrColumnNames.Add("DBREFUND AMOUNT");
	arrColumnNames.Add("PIN BLOCK");
	arrColumnNames.Add("TAX AMOUNT");
	arrColumnNames.Add("TIP AMOUNT");
	arrColumnNames.Add("TRAN START TIME");
	arrColumnNames.Add("HOST START TIME");
	arrColumnNames.Add("HOST FINISH TIME");
	arrColumnNames.Add("TRAN FINISH TIME");
	arrColumnNames.Add("SETTLE FILE PREFIX");
	arrColumnNames.Add("RESP SOURCE LEN");
	arrColumnNames.Add("VISA CHAR INDICATOR");
	arrColumnNames.Add("VISA TRAN ID");
	arrColumnNames.Add("VISA VALIDATION CODE");
	arrColumnNames.Add("MCARD BANKNET LEN");
	arrColumnNames.Add("MCARD BANKNET");
	arrColumnNames.Add("AGENT ID LEN");
	arrColumnNames.Add("AGENT ID");
	arrColumnNames.Add("CVC");
	arrColumnNames.Add("SETTLEMENT DATE");
	arrColumnNames.Add("CATEGORY CODE");
	arrColumnNames.Add("CONVERSION RATE");
	arrColumnNames.Add("PRODUCT CODE");
	arrColumnNames.Add("ODOMETER");
	arrColumnNames.Add("ORGANIZATION ID");
	arrColumnNames.Add("DYNAMIC KEYCHANGE");
	arrColumnNames.Add("SOURCE KEY");
	arrColumnNames.Add("TXN CNT");
	arrColumnNames.Add("DCF01 RETIRED CWK");
	arrColumnNames.Add("DEST KEY");
	arrColumnNames.Add("NCF01 RETIRED CWK");
	arrColumnNames.Add("HANDLER QUEUE");
	arrColumnNames.Add("AUTHORIZING HOST QUEUE");
	arrColumnNames.Add("MAX NET CONSEC TMOUTS");
	arrColumnNames.Add("MAX ACTIVE TXNS");
	arrColumnNames.Add("FORWARDING INSTITUTION ID");
	arrColumnNames.Add("REMOTE NSP TYPE");
	arrColumnNames.Add("KEK");
	arrColumnNames.Add("ACQUIRING ID");
	arrColumnNames.Add("OUTSTANDING BALANCE");
	arrColumnNames.Add("CREDIT LIMIT");
	arrColumnNames.Add("NFI NAME");
	arrColumnNames.Add("NFI SEQ NBR");
	arrColumnNames.Add("NFI START DATE");
	arrColumnNames.Add("NFI END DATE");
	arrColumnNames.Add("RESP SOURCE");
	arrColumnNames.Add("DISPENSATION ORIGIN");
	arrColumnNames.Add("DISPENSATION AUTH 1");
	arrColumnNames.Add("DISPENSATION AUTH 2");
	arrColumnNames.Add("UPDATE MASK");
	arrColumnNames.Add("TYPE OF DATA");
	arrColumnNames.Add("DOWN PAYMENT");
	arrColumnNames.Add("PERIOD");
	arrColumnNames.Add("INTEREST RATE");
	arrColumnNames.Add("TOTAL INTEREST");
	arrColumnNames.Add("REDEMPTION AMOUNT");
	arrColumnNames.Add("VEHICLE NUMBER");
	arrColumnNames.Add("PROFILE");
	arrColumnNames.Add("NBR OF PROD CODES");
	arrColumnNames.Add("PRODUCT CODES CODE 1");
	arrColumnNames.Add("PRODUCT CODES QUANTITY 1");
	arrColumnNames.Add("PRODUCT CODES AMOUNT 1");
	arrColumnNames.Add("PRODUCT CODES CODE 2");
	arrColumnNames.Add("PRODUCT CODES QUANTITY 2");
	arrColumnNames.Add("PRODUCT CODES AMOUNT 2");
	arrColumnNames.Add("PRODUCT CODES CODE 3");
	arrColumnNames.Add("PRODUCT CODES QUANTITY 3");
	arrColumnNames.Add("PRODUCT CODES AMOUNT 3");
	arrColumnNames.Add("PRODUCT CODES CODE 4");
	arrColumnNames.Add("PRODUCT CODES QUANTITY 4");
	arrColumnNames.Add("PRODUCT CODES AMOUNT 4");
	arrColumnNames.Add("PRODUCT CODES CODE 5");
	arrColumnNames.Add("PRODUCT CODES QUANTITY 5");
	arrColumnNames.Add("PRODUCT CODES AMOUNT 5");
	arrColumnNames.Add("PRODUCT CODES CODE 6");
	arrColumnNames.Add("PRODUCT CODES QUANTITY 6");
	arrColumnNames.Add("PRODUCT CODES AMOUNT 6");
	arrColumnNames.Add("PRODUCT CODES CODE 7");
	arrColumnNames.Add("PRODUCT CODES QUANTITY 7");
	arrColumnNames.Add("PRODUCT CODES AMOUNT 7");
	arrColumnNames.Add("PRODUCT CODES CODE 8");
	arrColumnNames.Add("PRODUCT CODES QUANTITY 8");
	arrColumnNames.Add("PRODUCT CODES AMOUNT 8");
	arrColumnNames.Add("PRODUCT CODES CODE 9");
	arrColumnNames.Add("PRODUCT CODES QUANTITY 9");
	arrColumnNames.Add("PRODUCT CODES AMOUNT 9");
	arrColumnNames.Add("PRODUCT CODES CODE 10");
	arrColumnNames.Add("PRODUCT CODES QUANTITY 10");
	arrColumnNames.Add("PRODUCT CODES AMOUNT 10");
	arrColumnNames.Add("PRODUCT CODES CODE 11");
	arrColumnNames.Add("PRODUCT CODES QUANTITY 11");
	arrColumnNames.Add("PRODUCT CODES AMOUNT 11");
	arrColumnNames.Add("PRODUCT CODES CODE 12");
	arrColumnNames.Add("PRODUCT CODES QUANTITY 12");
	arrColumnNames.Add("PRODUCT CODES AMOUNT 12");
	arrColumnNames.Add("PRODUCT CODES CODE 13");
	arrColumnNames.Add("PRODUCT CODES QUANTITY 13");
	arrColumnNames.Add("PRODUCT CODES AMOUNT 13");
	arrColumnNames.Add("PRODUCT CODES CODE 14");
	arrColumnNames.Add("PRODUCT CODES QUANTITY 14");
	arrColumnNames.Add("PRODUCT CODES AMOUNT 14");
	arrColumnNames.Add("PRODUCT CODES CODE 15");
	arrColumnNames.Add("PRODUCT CODES QUANTITY 15");
	arrColumnNames.Add("PRODUCT CODES AMOUNT 15");
	arrColumnNames.Add("PRODUCT CODES CODE 16");
	arrColumnNames.Add("PRODUCT CODES QUANTITY 16");
	arrColumnNames.Add("PRODUCT CODES AMOUNT 16");
	arrColumnNames.Add("PRODUCT CODES CODE 17");
	arrColumnNames.Add("PRODUCT CODES QUANTITY 17");
	arrColumnNames.Add("PRODUCT CODES AMOUNT 17");
	arrColumnNames.Add("PRODUCT CODES CODE 18");
	arrColumnNames.Add("PRODUCT CODES QUANTITY 18");
	arrColumnNames.Add("PRODUCT CODES AMOUNT 18");
	arrColumnNames.Add("PRODUCT CODES CODE 19");
	arrColumnNames.Add("PRODUCT CODES QUANTITY 19");
	arrColumnNames.Add("PRODUCT CODES AMOUNT 19");
	arrColumnNames.Add("PRODUCT CODES CODE 20");
	arrColumnNames.Add("PRODUCT CODES QUANTITY 20");
	arrColumnNames.Add("PRODUCT CODES AMOUNT 20");
	arrColumnNames.Add("VISA MERCHANT ID");
	arrColumnNames.Add("CASH BONUS");
	arrColumnNames.Add("DEFERRED FACTOR");
	arrColumnNames.Add("DEFERRED TERM LENGTH");
	arrColumnNames.Add("CURRENCY CODE");
	arrColumnNames.Add("TRANSMISSION TIMESTAMP");
	arrColumnNames.Add("TRAN FEE AMOUNT");
	arrColumnNames.Add("CVC DATA");
	arrColumnNames.Add("VERIFY MAG STRIPE");
	arrColumnNames.Add("SAF");
	arrColumnNames.Add("SECURITY RESPONSE CODE");
	arrColumnNames.Add("SERVICE CODE");
	arrColumnNames.Add("SOURCE PIN FORMAT");
	arrColumnNames.Add("DEST PIN FORMAT");
	arrColumnNames.Add("VOICE AUTH");
	arrColumnNames.Add("ACTUAL AMOUNT");
	arrColumnNames.Add("REVERSAL AMOUNT");
	arrColumnNames.Add("VOICE AUTH TEXT");
	arrColumnNames.Add("OPERATOR ID");
	arrColumnNames.Add("WORKSTATION");
	arrColumnNames.Add("ORIG AUTH NUMBER");
	arrColumnNames.Add("TICKET NBR");
	arrColumnNames.Add("SYSTEM DATE");
	arrColumnNames.Add("DEF GROSS AMT");
	arrColumnNames.Add("MONTHLY AMORT AMT");
	arrColumnNames.Add("PURGE_DATE");
	
	
}

void CArchiveLookupDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CArchiveLookupDlg)
	DDX_Control(pDX, IDC_MASK, m_ChkMaskCard);
	DDX_Control(pDX, IDC_NEXT100, m_CntlNext);
	DDX_Control(pDX, IDOK, m_CntlSearch);
	DDX_Control(pDX, IDC_PAUSE, m_CntlPause);
	DDX_Control(pDX, IDC_STOP, m_CntlStopSearch);
	DDX_Control(pDX, IDC_STATUS, m_CntlStatus);
	DDX_Control(pDX, IDC_TERMINAL_ID, m_CntlTerminalID);
	DDX_Control(pDX, IDC_START_DATE, m_CntlStartDate);
	DDX_Control(pDX, IDC_OPERATOR_ID, m_CntlOperatorID);
	DDX_Control(pDX, IDC_MERCHANT_ID, m_CntlMerchantID);
	DDX_Control(pDX, IDC_END_DATE, m_CntlEndDate);
	DDX_Control(pDX, IDC_CARD_NBR, m_CntlCardNbr);
	DDX_Control(pDX, IDC_AUTH_RESPONSE_ID, m_CntlAuthResponseID);
	DDX_Control(pDX, IDC_ARCHIVE_LIST, m_ArchiveList);
	DDX_Text(pDX, IDC_AUTH_RESPONSE_ID, m_szAuthResponseID);
	DDV_MaxChars(pDX, m_szAuthResponseID, 6);
	DDX_Text(pDX, IDC_CARD_NBR, m_szCardNbr);
	DDX_Text(pDX, IDC_END_DATE, m_szEndDate);
	DDV_MaxChars(pDX, m_szEndDate, 8);
	DDX_Text(pDX, IDC_MERCHANT_ID, m_szMerchantID);
	DDV_MaxChars(pDX, m_szMerchantID, 15);
	DDX_Text(pDX, IDC_OPERATOR_ID, m_szOperatorID);
	DDX_Text(pDX, IDC_START_DATE, m_szStartDate);
	DDV_MaxChars(pDX, m_szStartDate, 8);
	DDX_Text(pDX, IDC_TERMINAL_ID, m_szTerminalID);
	DDV_MaxChars(pDX, m_szTerminalID, 8);
	DDX_Text(pDX, IDC_STATUS, m_szStatus);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CArchiveLookupDlg, CDialog)
//{{AFX_MSG_MAP(CArchiveLookupDlg)
ON_WM_SYSCOMMAND()
ON_WM_PAINT()
ON_WM_QUERYDRAGICON()
ON_BN_CLICKED(IDC_PAUSE, OnPause)
ON_BN_CLICKED(IDC_STOP, OnStop)
	ON_NOTIFY(NM_DBLCLK, IDC_ARCHIVE_LIST, OnDblclkArchiveList)
	ON_BN_CLICKED(IDC_NEXT100, OnNext100)
	ON_WM_CLOSE()
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_MASK, OnMask)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CArchiveLookupDlg message handlers

BOOL CArchiveLookupDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	
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
	
	CFont m_font;
	LOGFONT lf;

	memset ( &lf, 0, sizeof (lf));
	lf.lfHeight = 13;
	lf.lfWeight = 700;
	strcpy(lf.lfFaceName, "Arial");
	m_font.CreateFontIndirect (&lf);
	GetDlgItem(IDC_STATUS)->SetFont (&m_font);

	// TODO: Add extra initialization here
	m_CntlPause.EnableWindow(FALSE);
	m_CntlStopSearch.EnableWindow(FALSE);
	
	m_pGetTLF->ResumeThread();
	
	m_ArchiveList.InsertColumn(arrColumnNames, LVCFMT_LEFT, 90);
	for(int i=0;i<arrColumnNames.GetSize(); i++)
	{
		switch(i)
		{
		case 0:
		case 10:
		case 27:
		case 6:
		case 37:
		case 38:
		case 12:
		case 36:
		case 4:
		case 3:
		case 18:
		case 25:
		case 186:
		case 16:
			//m_ArchiveList.ShowColumn(i, TRUE);
			break;
		default:
			m_ArchiveList.ShowColumn(i, FALSE);
			break;
		}
	}
	
	UpdateTitleWithVersion(this);
	this->ShowWindow(SW_MAXIMIZE);
	m_ChkMaskCard.SetCheck(1);


	
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CArchiveLookupDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CArchiveLookupDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting
		
		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);
		
		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;
		
		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CArchiveLookupDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CArchiveLookupDlg::OnOK() 
{
	
	CString		temp;
	bool		all_fields_empty = true;
	CMyUtils	myUtilsObject;
	
	UpdateData(TRUE);
	ZeroMemory ( &sTLFLimRecord, sizeof ( ARCHIVE_TLF01_LIMITS ) );
	
	m_szStatus= "";
		UpdateData(FALSE);	  
	
	if ( !myUtilsObject.IsEmpty(m_szStartDate, this) )
	{
		all_fields_empty = false;
		if ( !ValidateDate(m_szStartDate) )
		{
			m_CntlStartDate.SetFocus();
			return;
		}
		
		/* Cannot have start date without an end date. */
		if ( myUtilsObject.IsEmpty(m_szEndDate, this) )
		{
			m_CntlEndDate.SetFocus();
			m_szStatus= "Missing an End Date - required with Start Date.";
			UpdateData(FALSE);
			return;
		}
	}
	else
	{
		m_CntlStartDate.SetFocus();
		m_szStatus= "Start Date is required";
			UpdateData(FALSE);
			return;
	}
	
	/* END DATE */
	if ( !myUtilsObject.IsEmpty(m_szEndDate, this) )
	{
		all_fields_empty = false;
		if ( !ValidateDate(m_szEndDate) )
		{
			m_CntlEndDate.SetFocus();
			return;
		}
		
		/* Cannot have end date without a start date. */
		if ( myUtilsObject.IsEmpty(m_szStartDate, this) )
		{
			m_CntlStartDate.SetFocus();
			m_szStatus= "Missing a Start Date - required with End Date.";
			UpdateData(FALSE);
			return;
		}
	}
	else
	{
		m_CntlEndDate.SetFocus();
		m_szStatus= "END Date is required";
			UpdateData(FALSE);
			return;
	}
	
	
	if ( !myUtilsObject.IsEmpty ( m_szStartDate, this ) &&
		!myUtilsObject.IsEmpty ( m_szEndDate,   this )  )
	{
		if ( m_szStartDate.Right(4) > m_szEndDate.Right(4) )
		{
			m_CntlStartDate.SetFocus();
			m_szStatus= "Start Date Can Not be Greater than End Date";
			UpdateData(FALSE);
			return;
		}
		
		if ( m_szStartDate.Right(4) == m_szEndDate.Right(4) && 
			m_szEndDate.Mid(2,2) <  m_szStartDate.Mid(2,2) )
		{
			m_CntlStartDate.SetFocus();
			m_szStatus= "Start Date Can Not be Greater than End Date";
			UpdateData(FALSE);
			return;
		}
		
		if ( m_szStartDate.Right(4) == m_szEndDate.Right(4) && 
			m_szEndDate.Mid(2,2) ==  m_szStartDate.Mid(2,2) && 
			m_szEndDate.Left(2) < m_szStartDate.Left(2) )
		{
			m_CntlStartDate.SetFocus();
			m_szStatus= "Start Date Can Not be Greater than End Date";
			UpdateData(FALSE);
			return;
		}
	}
	
	/* ----------- */
	/* CARD NUMBER */
	/* ----------- */
	
	if ( !(myUtilsObject.IsEmpty(m_szCardNbr, this)) ) // empty field
	{
		all_fields_empty = false;
		memcpy ( sTLFLimRecord.card_num_min, m_szCardNbr, sizeof (sTLFLimRecord.card_num_min) );
	}
	
	/* ----------- */
	/* TERMINAL ID */
	/* ----------- */
	
	if ( !(myUtilsObject.IsEmpty(m_szTerminalID, this)) ) // empty field
	{
		all_fields_empty = false;
		memcpy ( sTLFLimRecord.terminal_id_min, m_szTerminalID, sizeof (sTLFLimRecord.terminal_id_min) );
	}
	
	/* ----------- */
	/* MERCHANT ID */
	/* ----------- */
	
	if ( !(myUtilsObject.IsEmpty(m_szMerchantID, this)) ) // empty field
	{
		all_fields_empty = false;
		memcpy ( sTLFLimRecord.merchant_id_min, m_szMerchantID, sizeof (sTLFLimRecord.merchant_id_min) );
	}
	
	/* ---- */
	/* DATE */
	/* ---- */
	if ( !(myUtilsObject.IsEmpty(m_szStartDate, this)) ) // empty field
	{
		all_fields_empty = false;
		/* Reformat start date from DDMMYYYY into YYYYMMDD. */
		temp = m_szStartDate.Right(4) + m_szStartDate.Mid(2,2) + m_szStartDate.Left(2);		
		memcpy ( sTLFLimRecord.date_yyyymmdd_start, temp, sizeof (sTLFLimRecord.date_yyyymmdd_start) );
	}
	
	if ( !(myUtilsObject.IsEmpty(m_szEndDate, this)) ) // empty field
	{
		all_fields_empty = false;
		/* Reformat end date from DDMMYYYY into YYYYMMDD. */
		temp = m_szEndDate.Right(4) + m_szEndDate.Mid(2,2) + m_szEndDate.Left(2);		
		memcpy ( sTLFLimRecord.date_yyyymmdd_end, temp, sizeof (sTLFLimRecord.date_yyyymmdd_end) );
	}

	/* -------------- */
	/* Auth ID		  */
	/* -------------- */
	
	if ( !(myUtilsObject.IsEmpty(m_szAuthResponseID, this)) ) // empty field
	{
		all_fields_empty = false;
		memcpy ( sTLFLimRecord.auth_number_min, m_szAuthResponseID, sizeof (sTLFLimRecord.auth_number_min) );
	}

	/* -------------- */
	/* TRANSACTION ID */
	/* -------------- */
	memcpy ( sTLFLimRecord.transaction_id, " ",
		sizeof (sTLFLimRecord.transaction_id) );
	
	if ( all_fields_empty == true )
	{
		m_szStatus= "Please enter search criteria...";
		UpdateData(FALSE);	  
		m_CntlCardNbr.SetFocus();
		return;
	}
	m_CntlSearch.EnableWindow(FALSE);
	m_CntlPause.SetWindowText("Pause");
	m_CntlPause.EnableWindow(TRUE);
	m_CntlStopSearch.EnableWindow(TRUE);
	
	m_pGetTLF->PostThreadMessage(WM_GET_TLF_LIST,0,(LPARAM)this);
	
}

BOOL CArchiveLookupDlg::ValidateDate(CString strDate)
{
	CString strDay;
	CString strMonth;
	CString strYear;
	CMyUtils myUtilsObject;
	
	int nDay, nMonth, nYear;
	
	UpdateData(TRUE);
	
	if ( myUtilsObject.IsEmpty ( strDate, this ))
		return TRUE;
	
	int length = strDate.GetLength();	
	int res = strspn ( strDate,"1234567890" );
	
	if ( length < 8 )
	{
		m_szStatus = "Invalid Date Format";
		UpdateData(FALSE);
		return FALSE;
	}
	
	strDay = strDate.Left(2);
	strMonth = strDate.Mid(2,2);
	strYear = strDate.Right(4);
	
	nDay = atoi ( strDay );
	nMonth = atoi ( strMonth );
	nYear = atoi ( strYear );
	
	if ( nDay == 0 || nMonth == 0 || nYear ==0)
	{
		m_szStatus = "ERROR:Invalid Data!";
		UpdateData(FALSE);
		return FALSE;
	}
	
	
	if ( nMonth > 12 )
	{
		m_szStatus = "ERROR:Invalid Month!";
		UpdateData(FALSE);
		return FALSE;
	}
	
	if ( nMonth == 1 || nMonth == 3 || nMonth == 5 || 
		nMonth == 7 || nMonth == 8 || nMonth == 10 || nMonth == 12 )
	{
		if ( nDay > 31 )
		{
			m_szStatus = "ERROR:Invalid Day!";
			UpdateData(FALSE);
			return FALSE;
		}
	}
	
	if ( nMonth == 4 || nMonth == 6 || nMonth == 9 || 
		nMonth == 11 )
	{
		if ( nDay > 30 )
		{
			m_szStatus = "ERROR:Invalid Day!";
			UpdateData(FALSE);
			return FALSE;
		}
	}
	
	if ( nMonth ==2 )
	{
		int res = nYear % 4;
		if ( res != 0 && nDay >28)
		{
			m_szStatus = "ERROR:Invalid Day!";
			UpdateData(FALSE);
			return FALSE;
		}
		if (res == 0 && nDay >29)
		{
			m_szStatus = "ERROR:Invalid Day!";
			UpdateData(FALSE);
			return FALSE;
		}
	}
	
	return TRUE;	
}

void CArchiveLookupDlg::OnPause() 
{
	/*
	CString str;
	m_CntlPause.GetWindowText(str);
	if(str.CompareNoCase("Pause") == 0)
	{
		m_pGetTLF->bStopSearch = true;
		m_CntlPause.SetWindowText("Resume");
		m_CntlSearch.EnableWindow(TRUE);
	}
	else
	{
		m_CntlSearch.EnableWindow(FALSE);
		m_pGetTLF->bStopSearch = false;
		m_CntlPause.SetWindowText("Pause");
		m_pGetTLF->PostThreadMessage(WM_GET_TLF_LIST,0,0);		
	}
	*/
}

void CArchiveLookupDlg::OnStop()
{
	m_CntlSearch.EnableWindow(TRUE);
	m_CntlPause.EnableWindow(FALSE);
	m_CntlStopSearch.EnableWindow(FALSE);
	m_CntlPause.SetWindowText("Pause");
	
	m_pGetTLF->bStopSearch = true;
	m_pGetTLF->PostThreadMessage(WM_STOP_SEARCH,0,0);
	
}

void CArchiveLookupDlg::OnDblclkArchiveList(NMHDR* pNMHDR, LRESULT* pResult) 
{

	int nItem = m_ArchiveList.GetFirstSelectedItem();
	CString str;
	CString str_system_date;
	m_ArchiveList.GetSelectedItemText(str,0);
	CTransactionDetails detailsDlg;
	memset(&detailsDlg.sTranRecord,0, sizeof(detailsDlg.sTranRecord));
	int len = str.GetLength();
	strncpy((char *)detailsDlg.sTranRecord.primary_key.transaction_id, str.GetBuffer(len), sizeof(detailsDlg.sTranRecord.primary_key.transaction_id));
	str.ReleaseBuffer();
	m_ArchiveList.GetSelectedItemText(str_system_date,186);
	len = str_system_date.GetLength();
	strncpy((char *)detailsDlg.sTranRecord.system_date, str_system_date.GetBuffer(len), sizeof(detailsDlg.sTranRecord.system_date));
	str_system_date.ReleaseBuffer();
	
	detailsDlg.m_bMaskCard = m_ChkMaskCard.GetCheck();
	detailsDlg.DoModal();

	*pResult = 0;
}

void CArchiveLookupDlg::OnNext100() 
{
	m_pGetTLF->PostThreadMessage(WM_GET_TLF_NEXT,0,0);	
}

void CArchiveLookupDlg::OnClose() 
{
	if(bSearching)
	{
		AfxMessageBox("Please wait till database query is completed");
		return;
	}	
	CDialog::OnClose();
}

void CArchiveLookupDlg::OnCancel() 
{
	if(bSearching)
	{
		AfxMessageBox("Please wait till database query is completed");
		return;
	}	
	
	CDialog::OnCancel();
}

void CArchiveLookupDlg::OnSize(UINT nType, int cx, int cy) 
{
	CDialog::OnSize(nType, cx, cy);
	
m_ArchiveList.Resize(cx, cy);	
}

void CArchiveLookupDlg::OnMask() 
{
	if(m_ChkMaskCard.GetCheck() == 1)
	{
		int nCount = m_ArchiveList.GetItemCount();
		for(int i=0; i< nCount; i++)
		{
			CString szCardNum = m_ArchiveList.GetItemText(i,10);
			int len = szCardNum.GetLength();
			if(len >= 12)
			{
				char *ptr = szCardNum.GetBuffer(len);
				memcpy(ptr + 6, "******", 6);
				szCardNum.ReleaseBuffer();
			}
			m_ArchiveList.SetItemText(i,10,szCardNum);
		}
	}
	else
	{
		int nCount = m_ArchiveList.GetItemCount();
		for(int i=0; i< nCount; i++)
		{
			CString szTranId = m_ArchiveList.GetItemText(i,0);
			CString szCardNum;
			tlfCardMap.Lookup(szTranId, szCardNum);
			m_ArchiveList.SetItemText(i,10,szCardNum);
		}
	}
}
