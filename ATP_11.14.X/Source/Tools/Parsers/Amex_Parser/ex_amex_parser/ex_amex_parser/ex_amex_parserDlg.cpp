
// ex_amex_parserDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ex_amex_parser.h"
#include "ex_amex_parserDlg.h"
#include "afxdialogex.h"
#include "AmexParser.h"
#include "AmexInfo.h"
#include "BASICTYP.H"
#ifdef _DEBUG
#define new DEBUG_NEW
#endif

BYTE      a2e[257];
BYTE      e2a[257];
BOOLEAN   maps_initialised = false;

void ascii_to_bin (LPBYTE src, LPBYTE dst, INT len);
void get_field_61();
pAUTH_TX p_auth_tx;
EMV EMV_details;
BYTE  iccdata[256]={0};
char field_61[256]={0};

#define  THIRD_BITMAP_SIZE					24
#define  EXTENDED_BITMAP_SIZE    			16
#define  NORMAL_BITMAP_SIZE					8

CHAR astr_bitmap[64]= {0};

// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// Cex_amex_parserDlg dialog




Cex_amex_parserDlg::Cex_amex_parserDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(Cex_amex_parserDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_flowfile = _T("");
	m_parsedmssg = _T("");
}

void Cex_amex_parserDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_FLOWFILE, m_flowfile);
	DDX_Text(pDX, IDC_PARSEDMSG, m_parsedmssg);
	DDX_Control(pDX, IDC_PARSEDMSG, m_CntlParsedMsg);
	DDX_Control(pDX, IDC_BUTTON1, copyParsedData);
	DDX_Control(pDX, IDC_EXPORT, exportbtn);
}

BEGIN_MESSAGE_MAP(Cex_amex_parserDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_PARSE, &Cex_amex_parserDlg::OnBnClickedParse)
	ON_BN_CLICKED(IDC_CLEAR, &Cex_amex_parserDlg::OnBnClickedClear)
	ON_BN_CLICKED(IDC_BUTTON1, &Cex_amex_parserDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &Cex_amex_parserDlg::OnBnClickedButton2)
	ON_BN_CLICKED(IDC_EXPORT, &Cex_amex_parserDlg::OnBnClickedExport)
	ON_BN_CLICKED(IDC_EXPORT, &Cex_amex_parserDlg::OnBnClickedExport)
END_MESSAGE_MAP()


// Cex_amex_parserDlg message handlers

BOOL Cex_amex_parserDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
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

	// TODO: Add extra initialization here
	m_CntlParsedMsg.SetWindowTextA("Hi");
	exportbtn.EnableWindow(FALSE);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void Cex_amex_parserDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void Cex_amex_parserDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

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
		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR Cex_amex_parserDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

/***********************************************************************************************************/
#define MAX_LOADSTRING 100
#define MAX_TRANSACTION 1000
static int temp_55_counter;
static char   Line[256][256];

char FieldStrings [][256] =
{
"MsgType					: ",
"001 BitMap				    : ",
"002 PAN					: ",
"003 ProcessingCode				: ",
"004 TxnAmount				: ",
"005 SettleAmount		                     	: ",
"006 BillingAmount				: ",
"007 TransmitDateTime			: ",
"008 BillingFeeAmount			: ",
"009 SettleConvRate				: ",
"010 BillingConvRate				: ",
"011 SystemTraceNo				: ",
"012 TxnTime				: ",
"013 TxnDate				: ",
"014 ExpirationDate				: ",
"015 SettleDate				: ",
"016 ConvDate				: ",
"017 CaptureDate				: ",
"018 MerchantType				: ",
"019 AcquiringCountry			: ",
"020 PANExtCountry				: ",
"021 ForwardCountry			: ",
"022 POSEntryMode				: ",
"023 CardSequenceNo			: ",
"024 NII					: ",
"025 POSConditionCode			: ",
"026 POSPINCode				: ",
"027 AuthIdLen				: ",
"028 TxnFeeAmount				: ",
"029 SettleFeeAmount			: ",
"030 TxnProcFeeAmount			: ",
//"031 SettleProcFeeAmount	: ",
"031 TransactionID				: ",
"032 AcquiringIdCode	                	: ",
"033 ForwardIdCode				: ",
"034 PANExt				: ",
"035 Track2				: ",
"036 Track3				: ",
"037 RetRefNo				: ", 
"038 AuthID				: ",
"039 ResponseCode				: ",
"040 ServiceRestrictCode			: ",
"041 TerminalID				: ",
"042 AcquirerID				: ",
"043 AcquirerName				: ",
"044 AddRespData				: ",
"045 Track1				: ",
"046 ISOAddData				: ",
"047 NationalAddData			: ",
"048 PrivateAddData				: ",
"049 TxnCurrency				: ",
"050 SettleCurrency				: ",
"051 BillingCurrency				: ",
"052 PINData				: ",
"053 SecurityInfo				: ",
"054 AddAmounts				: ",
"055 IccData				: ",
"056 Field56				: ",
"057 Field57				: ",
"058 Field58				: ",
"059 Field59				: ",
"060 Field60				: ",
"061 Field61				: ",
"062 Field62				: ",
"063 Field63				: ",
"064 MsgAuthCode				: ",
"066 SettlementCode				: ",
"067 PaymentCodeEx			: ",
"068 RecInstCntrCode			: ",
"069 SettlInstCntrCode			: ",
"070 NetworkInfCode			: ",
"071 MessageNo				: ",
"072 MessageNoLast				: ",
"073 DateAction				: ",
"074 CreditsNo				: ",
"075 CreditsRevNo				: ",
"076 DebitsNo				: ",
"077 DebitsRevNo				: ",
"078 TransferNo				: ",
"079 TransferRevNo				: ",
"080 InquiriesNo				: ",
"081 AuthNo				: ",
"082 CProcFeeAmount			: ",
"083 CTxnFeeAmount			: ",
"084 DProcFeeAmount			: ",
"085 DTxnFeeAmount			: ",
"086 CAmount				: ",
"087 CRevAmount				: ",
"088 DAmount				: ",
"089 DRevAmount				: ",
"090 OriginDataElement			: ",
"091 FileUpdateCode				: ",
"092 FileSecurityCode			: ",
"093 ResponseIndicator			: ",
"094 ServiceIndicator			: ",
"095 ReplacementAmounts			: ",
"096 MessageSecCode			: ",
"097 AmountNetSettle			: ",
"098 Payee				: ",
"099 SettleIdCode				: ",
"100 ReceivingIdCode			: ",
"101 FileName				: ",
"102 AccountId1				: ",
"103 AccountId2				: ",
"104 TxnDescription				: ",
"105 Field105				: ",
"106 Field106				: ",
"107 Field107				: ",
"108 Field108				: ",
"109 Field109				: ",
"110 Field110				: ",
"111 Field111				: ",
"112 Field112				: ",
"113 Field113				: ",
"114 Field114				: ",
"115 Field115				: ",
"116 Field116				: ",
"117 Field117				: ",
"118 Field118				: ",
"119 Field119				: ",
"120 Field120				: ",
"121 Field121				: ",
"122 Field122				: ",
"123 Field123				: ",
"124 Field124				: ",
"125 Field125				: ",
"126 Field126				: ",
"127 Field127				: ",
"128 Field128				: ",
"129 Field129				: ",
"130 Field130				: ",
"131 Field131				: ",
"132 Field132				: ",
"133 Field133				: ",
"134 Field134				: ",
"135 Field135				: ",
"136 Field136				: ",
"137 Field137				: ",
"138 Field138				: ",
"139 Field139				: ",
"140 Field140				: ",
"141 Field141				: ",
"142 Field142				: ",
"143 Field143				: ",
"144 Field144				: ",
"145 Field145				: ",
"146 Field146				: ",
"147 Field147				: ",
"148 Field148				: ",
"149 Field149				: ",
"150 Field150				: ",
"151 Field151				: ",
"152 Field152				: ",
"153 Field153				: ",
"154 Field154				: ",
"155 Field155				: ",
"156 Field156				: ",
"157 Field157				: ",
"158 Field158				: ",
"159 Field159				: ",
"160 Field160				: ",
"161 Field161				: ",
"162 Field162				: ",
"163 Field163				: ",
"164 Field164				: ",
"165 Field165				: ",
"166 Field166				: ",
"167 Field167				: ",
"168 Field168				: ",
"169 Field169				: ",
"170 Field170				: ",
"171 Field171				: ",
"172 Field172				: ",
"173 Field173				: ",
"174 Field174				: ",
"175 Field175				: ",
"176 Field176				: ",
"177 Field177				: ",
"178 Field178				: ",
"179 Field179				: ",
"180 Field180				: ",
"181 Field181				: ",
"182 Field182				: ",
"183 Field183				: ",
"184 Field184				: ",
"185 Field185				: ",
"186 Field186				: ",
"187 Field187				: ",
"188 Field188				: ",
"189 Field189				: ",
"190 Field190				: ",
"191 Field191				: ",
"192 Field192				: "
};



struct iso_msg_rec
{
   BYTE msgtype       [4];
   BYTE bitmap1_bin   [16];
   BYTE bitmap2_bin   [16];
   BYTE pan_len       [2];
   BYTE pan           [19];
   BYTE proccode      [6];
   BYTE amount        [12];
   BYTE setl_amount   [12];
   BYTE gmt_time      [10];
   BYTE setl_conv_rate[8];
   BYTE stan          [6];
   BYTE datetime      [12];
   BYTE eff_date	  [4];
   BYTE date_expd	  [4];
   BYTE date_settle   [4];
   BYTE date_conv     [4];
   BYTE merch_type	  [4];
   BYTE ctry_code	  [3];
   BYTE coun_code     [3];
   BYTE fwd_inst_cnt_code [3];
   BYTE posentry      [12];
   BYTE card_seq_num  [3];
   BYTE func_cd       [3];
   BYTE pos_condcd    [4];
   BYTE accpt_bus_cd  [4];
   BYTE orig_amt      [24];	
   BYTE tran_fee_amt  [8];
   BYTE tranid_len	  [2];
   BYTE tranid		  [40];
   BYTE acq_inst_len  [2];
   BYTE acq_inst_id   [11];
   BYTE fwd_inst_len  [2];
   BYTE fwd_inst_id   [11];
   BYTE pan_extd_len  [2];
   BYTE pan_extd      [30];
   BYTE track2_len    [2];
   BYTE track2        [37];
   BYTE rrn			  [12];
   BYTE authcode      [6];
   BYTE responsecd    [3];
   BYTE termid        [8];
   BYTE cardaccid     [15];
   BYTE cardaccnamelen[2];
   BYTE cardaccname	  [101];
   BYTE addl_rsp_len  [2];
   BYTE addlrspdata   [25];
   BYTE track1_len    [2];
   BYTE track1        [76];
   BYTE addl_data_len [3];
   BYTE addl_data     [70];
   BYTE tran_curr_cd  [3];
   BYTE recn_curr_cd  [3];
   BYTE pinblock      [16];
   BYTE sec_cntl_len  [2];
   BYTE sec_cntl      [8];
   BYTE AddlAmts_len   [3];
   BYTE AddlAmts     [246];
   BYTE IccData_len   [3];
   BYTE IccData	    [264];
   BYTE orig_data_len [2];
   BYTE orig_data     [35];
   BYTE priv60_len	  [3];
   BYTE priv60 	      [212];
   BYTE priv61_len	  [3];
   BYTE priv61		  [99];
   BYTE priv62_len	  [3];
   BYTE priv62		  [99];
   BYTE priv63_len	  [3];
   BYTE priv63		  [99];
};

struct FieldNumberMappingToIsoMsgRec
{
	void *fldPtr[1000];
};


/*****************************************************************************
 *********************** Define all globals required here ********************
 *****************************************************************************/

INT          Field63_present;
INT			 Field62_present;
struct iso_msg_rec HOST_MESSAGE;
#define  COMM_BUF_SZE  2000
BYTE   ReqRsp_Buffer [COMM_BUF_SZE];
char  A2E[4] = "A2E";    /*  ASCII to EBCDIC conversion */
char  E2A[4] = "E2A";    /*  EBCDIC to ASCII conversion */
#define EXTENDED_BITFIELD 128
#define NORMAL_BITFIELD    64
int     BitFieldSize = NORMAL_BITFIELD;
INT    bitmapidx;              /* offset of bitmap... */
INT    reqidx;                  /* next open position in request buffer */
#define N_BFData ( sizeof(BFData) / sizeof (struct bitfield_data) )

int BitFieldArray[128] = {0};

struct isobit 
   {
   INT byte_no;         /* byte number for this bit field */
   INT bit_mask;        /* bit to set in this byte        */
   };

const struct isobit BITTAB[128+1] =
   {
	   { 0, 0 }, /*zero justify */
	   { 0, 0x80 },   /* bit 1  */   { 0, 0x40 },   /* bit 2  */   { 0, 0x20 },   /* bit 3  */   { 0, 0x10 },   /* bit 4  */
	   { 0, 0x08 },   /* bit 5  */   { 0, 0x04 },   /* bit 6  */   { 0, 0x02 },   /* bit 7  */   { 0, 0x01 },   /* bit 8  */
	   { 1, 0x80 },   /* bit 9  */   { 1, 0x40 },   /* bit 10 */   { 1, 0x20 },   /* bit 11 */   { 1, 0x10 },   /* bit 12 */
	   { 1, 0x08 },   /* bit 13 */   { 1, 0x04 },   /* bit 14 */   { 1, 0x02 },   /* bit 15 */   { 1, 0x01 },   /* bit 16 */
	   { 2, 0x80 },   /* bit 17 */   { 2, 0x40 },   /* bit 18 */   { 2, 0x20 },   /* bit 19 */   { 2, 0x10 },   /* bit 20 */
	   { 2, 0x08 },   /* bit 21 */   { 2, 0x04 },   /* bit 22 */   { 2, 0x02 },   /* bit 23 */   { 2, 0x01 },   /* bit 24 */
	   { 3, 0x80 },   /* bit 25 */   { 3, 0x40 },   /* bit 26 */   { 3, 0x20 },   /* bit 27 */   { 3, 0x10 },   /* bit 28 */
	   { 3, 0x08 },   /* bit 29 */   { 3, 0x04 },   /* bit 30 */   { 3, 0x02 },   /* bit 31 */   { 3, 0x01 },   /* bit 32 */
	   { 4, 0x80 },   /* bit 33 */   { 4, 0x40 },   /* bit 34 */   { 4, 0x20 },   /* bit 35 */   { 4, 0x10 },   /* bit 36 */
	   { 4, 0x08 },   /* bit 37 */   { 4, 0x04 },   /* bit 38 */   { 4, 0x02 },   /* bit 39 */   { 4, 0x01 },   /* bit 40 */
	   { 5, 0x80 },   /* bit 41 */   { 5, 0x40 },   /* bit 42 */   { 5, 0x20 },   /* bit 43 */   { 5, 0x10 },   /* bit 44 */
	   { 5, 0x08 },   /* bit 45 */   { 5, 0x04 },   /* bit 46 */   { 5, 0x02 },   /* bit 47 */   { 5, 0x01 },   /* bit 48 */
	   { 6, 0x80 },   /* bit 49 */   { 6, 0x40 },   /* bit 50 */   { 6, 0x20 },   /* bit 51 */   { 6, 0x10 },   /* bit 52 */
	   { 6, 0x08 },   /* bit 53 */   { 6, 0x04 },   /* bit 54 */   { 6, 0x02 },   /* bit 55 */   { 6, 0x01 },   /* bit 56 */
	   { 7, 0x80 },   /* bit 57 */   { 7, 0x40 },   /* bit 58 */   { 7, 0x20 },   /* bit 59 */   { 7, 0x10 },   /* bit 60 */
	   { 7, 0x08 },   /* bit 61 */   { 7, 0x04 },   /* bit 62 */   { 7, 0x02 },   /* bit 63 */   { 7, 0x01 },    /* bit 64 */

	   { 8, 0x80 },    /* bit 1  */   { 8, 0x40 },    /* bit 2  */   { 8, 0x20 },    /* bit 3  */   { 8, 0x10 },    /* bit 4  */
	   { 8, 0x08 },    /* bit 5  */   { 8, 0x04 },    /* bit 6  */   { 8, 0x02 },    /* bit 7  */   { 8, 0x01 },    /* bit 8  */
	   { 9, 0x80 },    /* bit 9  */   { 9, 0x40 },    /* bit 10 */   { 9, 0x20 },    /* bit 11 */   { 9, 0x10 },    /* bit 12 */
	   { 9, 0x08 },    /* bit 13 */   { 9, 0x04 },    /* bit 14 */   { 9, 0x02 },    /* bit 15 */   { 9, 0x01 },    /* bit 16 */
	   { 10, 0x80 },   /* bit 17 */   { 10, 0x40 },   /* bit 18 */   { 10, 0x20 },   /* bit 19 */   { 10, 0x10 },   /* bit 20 */
	   { 10, 0x08 },   /* bit 21 */   { 10, 0x04 },   /* bit 22 */   { 10, 0x02 },   /* bit 23 */   { 10, 0x01 },   /* bit 24 */
	   { 11, 0x80 },   /* bit 25 */   { 11, 0x40 },   /* bit 26 */   { 11, 0x20 },   /* bit 27 */   { 11, 0x10 },   /* bit 28 */
	   { 11, 0x08 },   /* bit 29 */   { 11, 0x04 },   /* bit 30 */   { 11, 0x02 },   /* bit 31 */   { 11, 0x01 },   /* bit 32 */
	   { 12, 0x80 },   /* bit 33 */   { 12, 0x40 },   /* bit 34 */   { 12, 0x20 },   /* bit 35 */   { 12, 0x10 },   /* bit 36 */
	   { 12, 0x08 },   /* bit 37 */   { 12, 0x04 },   /* bit 38 */   { 12, 0x02 },   /* bit 39 */   { 12, 0x01 },   /* bit 40 */
	   { 13, 0x80 },   /* bit 41 */   { 13, 0x40 },   /* bit 42 */   { 13, 0x20 },   /* bit 43 */   { 13, 0x10 },   /* bit 44 */
	   { 13, 0x08 },   /* bit 45 */   { 13, 0x04 },   /* bit 46 */   { 13, 0x02 },   /* bit 47 */   { 13, 0x01 },   /* bit 48 */
	   { 14, 0x80 },   /* bit 49 */   { 14, 0x40 },   /* bit 50 */   { 14, 0x20 },   /* bit 51 */   { 14, 0x10 },   /* bit 52 */
	   { 14, 0x08 },   /* bit 53 */   { 14, 0x04 },   /* bit 54 */   { 14, 0x02 },   /* bit 55 */   { 14, 0x01 },   /* bit 56 */
	   { 15, 0x80 },   /* bit 57 */   { 15, 0x40 },   /* bit 58 */   { 15, 0x20 },   /* bit 59 */   { 15, 0x10 },   /* bit 60 */
	   { 15, 0x08 },   /* bit 61 */   { 15, 0x04 },   /* bit 62 */   { 15, 0x02 },   /* bit 63 */   { 15, 0x01 }   /* bit 64 */
   };


enum bitfields {
   bf_done           = 0,      /* end of list    */
   bf_1xbitfield     = 1,      /* extended bit field */
   bf_2pan           = 2,      /* pan            */
   bf_3proc          = 3,      /* processing code */
   bf_4amount        = 4,      /* amount   */
   bf_5recon_amt     = 5,      /* reconcilliation amount */
   bf_6crdbillamt    = 6,      /* amount cardholder billing */
   bf_7trntime       = 7,      /* transmission date-time */
   bf_9settconvrate   =  9,
   bf_10convrate     = 10,     /* Conversion rate, cardholder billing */
   bf_11stan         = 11,     /* system trace number */
   bf_12datetime         = 12,     /* time    */
   bf_13date         = 13,     /* date, trans  */
   bf_14expiry       = 14,     /* expiry date  */
   bf_15datestl      = 15,     /* settlement date */
   bf_16dateconv      =16,
   bf_18mertype		 = 18,     /* Merchant's type of business */
   bf_19ctrycd       = 19,     /* Acquiring institution's country code */
   bf_20countrycode  = 20,     /* pan extended, country code */
   bf_21fwdinstcntcode = 21,  
   bf_22posentry     = 22,     /* pos entry mode */
   bf_23csn          = 23,     /* card sequence number*/
   bf_24funccd       = 24,     /* function code */
   bf_25poscc        = 25,       /* reason code */
   bf_26accptcd      = 26,     /* acceptor code */
  bf_28tranfeeamt   = 28,
   bf_30origamt      = 30,     /* original amount   */
   bf_31tranid		 = 31,
   bf_32idcode       = 32,     /* Acquiring institution's ID code */
   bf_33fwdinstidcode= 33,     /* Forwarding institution's ID code */
   bf_34panext       = 34,     /* pan extended */
   bf_35track2       = 35,     /* track 2 data */
   bf_37rrn          = 37,     /* retrieval reference number */
   bf_38authcode     = 38,     /* auth code   */
   bf_39respcode     = 39,     /* response code*/
   bf_40src          = 40,     /* service restriction code */
   bf_41termid       = 41,     /* terminal id  */
   bf_42cardacc      = 42,     /* card acceptor id */
   bf_43cardacceptloc= 43,     /* card acceptor name/location */
   bf_44respdata	 = 44,    /* additional response data */
   bf_45track1       = 45,     /* track 1 data   */
   bf_48addldata    = 48,     /* Additional Data - Private */
   bf_49trncurcd     = 49,     /* transaction currency code */
   bf_50recncurcd    = 50,     /* reconcilliation currency code */
   bf_51crdcurcd     = 51,     /* card currency code */
   bf_52pinblock     = 52,     /* pin block    */
   bf_53seccntl      = 53,     /* security related control info */
   bf_54AddlAmount   = 54,     /* additional amounts */
   bf_55IccData		 = 55,	   /* emv data*/
   bf_56orgdata      = 56,     /* original data elements */
   bf_59posdata      = 59,     /* national point-of-service geographic data */
   bf_60priv60       = 60,     /* private field 60 */
   bf_61priv61       = 61,     /* private field 61 */
   bf_62priv62       = 62,     /* private field 62 */
   bf_63priv63       = 63,     /* private field 63 */
   bf_70nmicode      = 70,     /* network management information code */
   bf_73action       = 73,     /* Date, action */
   bf_90orgdata      = 90,     /* original data elements   */
   bf_91fileupdate   = 91,     /* file update code         */
   bf_93rspind       = 93,     /* response indicator       */
   bf_95replamts     = 95,     /* replacement amounts      */
   bf_96msgsec       = 96,     /* message security code    */
   bf_99stlinstid    = 99,     /* settlement institution id code */
   bf_100rcvid       = 100,    /* receiving inst id code   */
   bf_101fname       = 101,    /* file name                */
   bf_102acct1       = 102,    /* acct id 1                */
   bf_103acct2       = 103,    /* acct id 2                */
   bf_104trandesc    = 104,    /* transaction description  */
   bf_118intractry   = 118,    /* intra country data       */
   bf_120orgmsgid    = 120,    /* original message type id */
   bf_121issid       = 121,    /* issuing inst id code     */
   bf_122openuse     = 122,    /* remaining open-to-use    */
   bf_123addverf     = 123,    /* address verification data*/
   bf_124freeform    = 124,     /* free form text - Japan   */
   bf_127filerecs    = 127     /* free form text - Japan   */
   }; 



/******************************************************************************
 *
 *  NAME:         ASCII_EBCDIC_CONV
 *
 *  DESCRIPTION:  This function converts an ASCII character into and an EBCDIC
 *                character, or visa versa.
 *
 *                    ascii_ebcdic_conv - IBM EBCDIC
 *                    ------------------------------
 *                    Convert ASCII to EBCDIC -or- EBCDIC to ASCII.
 *                    Use input parameter 2, either "A2E" or "E2A".
 *
 *  INPUTS:       conv_char - Character to be converted
 *                conv_type - A2E or E2A: indicates which way to convert
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   Converted character
 *
 *  AUTHOR:       Dennis Irby
 *
 *  MODIFICATIONS:
 *     DI - 03/15/99 - Added '^' to the conversion chart.
 *     DI - 05/19/99 - Changed '^' from 0xB0 to 0x9A as the EBCDIC value.
 ******************************************************************************/
BYTE ascii_ebcdic_conv( unsigned char conv_char, char *conv_type )
{

#define  CHART_SIZE  92

   INT   i;
   INT   found = 0;
   BYTE  ReturnChar;

      /*
        +------------------------------------------------------------+
        |   0     1     2     3     4     5     6     7     8     9  |
        |   a     b     c     d     e     f     g     h     i     j  |
        |   k     l     m     n     o     p     q     r     s     t  |
        |   u     v     w     x     y     z     A     B     C     D  |
        |   E     F     G     H     I     J     K     L     M     N  |
        |   O     P     Q     R     S     T     U     V     W     X  |
        |   Y     Z           .     <     (     +     |     &     !  |
        |   $     *     )     ;     -     /     ,     %     _     >  |
        |   ?     :     #     @     '     =     "     ^              |
        +------------------------------------------------------------+
      */

   static unsigned char ebcdic_chart[CHART_SIZE] = 
      {
         0xF0, 0xF1, 0xF2, 0xF3, 0xF4, 0xF5, 0xF6, 0xF7, 0xF8, 0xF9,
         0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x91,
         0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0xA2, 0xA3,
         0xA4, 0xA5, 0xA6, 0xA7, 0xA8, 0xA9, 0xC1, 0xC2, 0xC3, 0xC4,
         0xC5, 0xC6, 0xC7, 0xC8, 0xC9, 0xD1, 0xD2, 0xD3, 0xD4, 0xD5,
         0xD6, 0xD7, 0xD8, 0xD9, 0xE2, 0xE3, 0xE4, 0xE5, 0xE6, 0xE7,
         0xE8, 0xE9, 0x40, 0x4B, 0x4C, 0x4D, 0x4E, 0x4F, 0x50, 0x5A,
         0x5B, 0x5C, 0x5D, 0x5E, 0x60, 0x61, 0x6B, 0x6C, 0x6D, 0x6E,
         0x6F, 0x7A, 0x7B, 0x7C, 0x7D, 0x7E, 0x7F, 0x5F, 0x9A, 0xB0,
		 0x8A, 0xE0
      };

   static unsigned char ascii_chart[CHART_SIZE] = 
      {
         0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39,
         0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6A,
         0x6B, 0x6C, 0x6D, 0x6E, 0x6F, 0x70, 0x71, 0x72, 0x73, 0x74,
         0x75, 0x76, 0x77, 0x78, 0x79, 0x7A, 0x41, 0x42, 0x43, 0x44,
         0x45, 0x46, 0x47, 0x48, 0x49, 0x4A, 0x4B, 0x4C, 0x4D, 0x4E,
         0x4F, 0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58,
         0x59, 0x5A, 0x20, 0x2E, 0x3C, 0x28, 0x2B, 0x7C, 0x26, 0x21,
         0x24, 0x2A, 0x29, 0x3B, 0x2D, 0x2F, 0x2C, 0x25, 0x5F, 0x3E,
         0x3F, 0x3A, 0x23, 0x40, 0x27, 0x3D, 0x22, 0x5E, 0x5E
      };


   if ( 0 == strcmp( conv_type, A2E ) )
   {
      /* CONVERT ASCII TO EBCDIC */
      for( i=0; i<CHART_SIZE  &&  !found; i++ )
      {
         if ( conv_char == ascii_chart[i] )
         {
            ReturnChar = ebcdic_chart[i];
            found = 1;
         }
      }
   }
   else if ( 0 == strcmp( conv_type, E2A ) )
   {
      /* CONVERT EBCDIC TO ASCII */
      for( i=0; i<CHART_SIZE  &&  !found; i++ )
      {
         if ( conv_char == ebcdic_chart[i] )
         {
            ReturnChar = ascii_chart[i];
            found = 1;
         }
      }
   }

   if ( 0 == found )
      ReturnChar = 0x00;

   return( ReturnChar );
}

/************************************************
  Bit Field Data Record
*************************************************/
struct bitfield_data 
   {
   enum bitfields  bd_fieldid;                           /* id for this field */
   unsigned char * p_bd_pdata;                           /* ptr to data to move */
   INT             bd_fieldlen;                          /* # of bytes in iso8583 field */
   INT             (*bd_BFCond)(enum bitfields, BYTE);  /* ptr to conditional function */
   void            (*bd_BFMove)(enum bitfields);         /* ptr to move function */
   void            (*bd_BFRsp)(enum bitfields);          /* ptr to response handler function */
   };

/**********************************
      FUNCTION PROTOTYPES
**********************************/
/* functions local */
BYTE ascii_ebcdic_conv_trk1( unsigned char conv_char, char *conv_type,unsigned char fldSep);
BYTE getFldSep( unsigned char* resp_buff, int index );
INT CondProc(enum bitfields, BYTE);
INT CondExpd(enum bitfields, BYTE);
INT CondPan(enum bitfields, BYTE);
INT CondTrack1(enum bitfields, BYTE);
INT Move_Always(enum bitfields, BYTE);
INT Move_IfThere(enum bitfields, BYTE);
INT vMove_IfThere(enum bitfields, BYTE);
INT Move_Never(enum bitfields, BYTE);
void genutil_bin_to_ascii (BYTE bstring[], INT blen, pCHAR astr, INT astr_len);
void EBCDICMove(enum bitfields);
void vEBCDICMove(enum bitfields);
void RspEBCDICvMove(enum bitfields);
void RspEBCDICMove(enum bitfields);
void RspIgnore(enum bitfields);
void vRspIgnore(enum bitfields);
void Rj_with_lead_zeros( char * str, INT FieldLen );
void generate_AMEX_EMV_data();
void Generate_EMV_Details();



void asc_to_vbcd (char *, INT, BYTE [] );
void asc_to_bcd (char *, INT, BYTE [] );
void str_asc_to_bcd(char *, INT, BYTE [] ); 
void vBCDMove(enum bitfields fieldid);
void RspMovevBCD (enum bitfields fieldid);
void GenericMove(enum bitfields fieldid);
void RspMoveGeneric(enum bitfields fieldid);
void vDataMove(enum bitfields fieldid);
void RspMovevData (enum bitfields fieldid);

void find_cvc_flag( char *, char *, char *);

void Field62Response(enum bitfields);
void parsed_mssg();
void    Rj_with_lead_zeros( char * str, INT FieldWidth );
INT     hhutil_send_net_mngmnt_response(pAUTH_TX p_auth_tx, CHAR amex_response_code[4]);
INT     hhutil_send_net_mngmnt_request(pAUTH_TX p_auth_tx, CHAR amex_response_code[4]);
void    convert_from_julian_day( char * julian_str, char * date_time );
BOOLEAN Build_Host_Request ( pAUTH_TX p_auth_tx);
INT     hhutil_parse_host_response_msg();
INT     move_iso_response_to_auth_struct(CHAR nmi_code[4], pAUTH_TX p_auth_tx, INT);
void    build_host_structure( pAUTH_TX p_auth_tx, CHAR amex_response_code[4]);
void    copy_to_req_resp_buffer (char * indata, INT  msglen   );
BYTE    get_tran_type_2(BYTE);
BYTE    get_tran_type( pAUTH_TX p_auth_tx ) ;
void    TestResponses(BYTE ReqRsp_Buffer[COMM_BUF_SZE]);
BOOLEAN blank_string(char * astr, INT max_len);
void    hhutil_get_strtime ( char * deststr );
BYTE    hhutil_process_host_request(pAUTH_TX p_auth_tx);
INT     incoming_other_host_response(pAUTH_TX p_auth_tx);
BOOLEAN hhutil_verify_pin_block(pAUTH_TX p_auth_tx);
BOOLEAN hhutil_process_debit_transaction(pAUTH_TX p_auth_tx);
BOOLEAN hhutil_send_host_response(pAUTH_TX p_auth_tx);
void    hhutil_send_retry_response_to_host(char * code, pAUTH_TX p_auth_tx);
//void    hhutil_parse_db_ok(pPTE_MSG p_msg_in);
//void    hhutil_parse_db_error(pPTE_MSG p_msg_in);
//BYTE    hhutil_get_ncf01_cfg( pNCF01 pncf01, char * error_msg );
BOOLEAN blank_or_null_string(char * astr, INT max_len);
BOOLEAN hhutil_transfer_ctl_to_auth_app ( pAUTH_TX p_auth_tx);
INT     incoming_other_host_response_db_reply( pAUTH_TX p_auth_tx);
INT     incoming_other_host_response_on_error( pAUTH_TX p_auth_tx,CHAR amex_response_code[4]);
INT     insert_tlf01(pAUTH_TX p_auth_tx);
void    get_current_date_time( char * system_date, char * system_time );

//LONG    get_crf01( pCRF01 pcrf01 );

//moved from amexutil.c to amexutil.h
BYTE ascii_ebcdic_conv(BYTE conv_char, char * conv_type );
INT isnum(char * string);
INT isalphanum(char * string);
INT isalphanumSpace(char * string);
INT sendBkToHost(pAUTH_TX p_auth_tx,CHAR amex_response_code[4]);
INT verify_auth_tx_data(pAUTH_TX p_auth_tx);

void convert_to_ebcdic(pBYTE source, pBYTE dest, INT len);
void init_character_code_maps();

//static  struct bitfield_data *BFData;
static  struct bitfield_data BFData[] = {
   {
   bf_2pan,                      /* field */
   HOST_MESSAGE.pan_len,         /* data to move */
   2,                            /* # of bytes in pan len field */
   Move_Always,                  /* mandatory */
   vEBCDICMove,                  /* move routine */
   RspEBCDICvMove                /* response routine */
   },
   {
   bf_3proc,                     /* field */
   HOST_MESSAGE.proccode,        /* data to move */
   6,                            /* # of bytes in  iso field */
   Move_IfThere,                  /* mandatory */
   EBCDICMove,                   /* move routine */
   RspEBCDICMove                 /* response routine */
   },
   {
   bf_4amount,                   /* field */
   HOST_MESSAGE.amount,          /* data to move */
   12,                           /* # of bytes in  iso field */
   Move_Always,                  /* mandatory */
   EBCDICMove,                   /* move routine */
   RspEBCDICMove                 /* response routine */
   },
   {
   bf_5recon_amt,                /* field */
   HOST_MESSAGE.setl_amount,     /* data to move */
   12,                           /* # of bytes in  iso field */
   Move_IfThere,                  /* mandatory */
   EBCDICMove,                   /* move routine */
   RspEBCDICMove                 /* response routine */
   },
   {
   bf_7trntime,                  /* field */
   HOST_MESSAGE.gmt_time,        /* data to move : MMDDhhmmss*/
   10,                           /* # of bytes in  iso field */
   Move_IfThere,                  /* conditional */
   EBCDICMove,                   /* move routine */
   RspEBCDICMove                 /* response routine */
   },
   {
   bf_9settconvrate,             /* field */
   HOST_MESSAGE.setl_conv_rate,  /* data to move */
   8,                            /* # of bytes in  iso field */
   Move_IfThere,                  /* conditional */
   EBCDICMove,                   /* move routine */
   RspEBCDICMove                 /* response routine */
   },
   {
   bf_11stan,                    /* field */
   HOST_MESSAGE.stan,            /* data to move */
   6,                            /* # of bytes in  iso field */
   Move_Always,                  /* conditional */
   EBCDICMove,                   /* move routine */
   RspEBCDICMove                 /* response routine */
   },
   {
   bf_12datetime,                    /* field */
   HOST_MESSAGE.datetime,            /* data to move */
   12,                            /* # of bytes in  iso field */
   Move_Always,                  /* conditional */
   EBCDICMove,                   /* move routine */
   RspEBCDICMove                 /* response routine */
   },
   {
   bf_13date,                    /* field */
   HOST_MESSAGE.eff_date,            /* effective data to move */
   4,                            /* # of bytes in  iso field */
   Move_IfThere,                  /* conditional */
   EBCDICMove,                   /* move routine */
   RspEBCDICMove                 /* response routine */
   },
   {
   bf_14expiry,                  /* field */
   HOST_MESSAGE.date_expd,       /* data to move */
   4,                            /* # of bytes in field */
   Move_IfThere,                     /* conditional */
   EBCDICMove,                   /* move routine */
   RspEBCDICMove                 /* response routine */
   },
   {
   bf_15datestl,                 /* field */
   HOST_MESSAGE.date_settle,     /* data to move */
   4,                            /* # of bytes in  iso field */
   Move_IfThere,                 /* conditional */
   EBCDICMove,                   /* move routine */
   RspEBCDICMove                 /* response routine */
   },
   {
   bf_16dateconv,                /* field */
   HOST_MESSAGE.date_conv,       /* data to move */
   4,                            /* # of bytes in  iso field */
   Move_Always,                  /* conditional */
   EBCDICMove,                   /* move routine */
   RspEBCDICMove                 /* response routine */
   },
   {
   bf_18mertype,                 /* field */
   HOST_MESSAGE.merch_type,            /* data to move */
   4,                            /* # of bytes in  iso field */
   Move_Always,                  /* conditional */
   EBCDICMove,                   /* move routine */
   RspEBCDICMove                 /* response routine */
   },
   {
   bf_19ctrycd,                  /* field */
   HOST_MESSAGE.ctry_code,       /* data to move */
   3,                            /* # of bytes in field */
   Move_Always,                  /* mandatory    */
   EBCDICMove,                   /* move routine */
   RspEBCDICMove                 /* response routine */
   },
   {
   bf_20countrycode,                    /* field */
   HOST_MESSAGE.coun_code,            /* data to move */
   3,                            /* # of bytes in  iso field */
   Move_Always,                  /* conditional */
   EBCDICMove,                   /* move routine */
   RspEBCDICMove                 /* response routine */
   },
   {
   bf_21fwdinstcntcode,                    /* field */
   HOST_MESSAGE.fwd_inst_cnt_code,            /* data to move */
   3,                            /* # of bytes in  iso field */
   Move_Always,                  /* conditional */
   EBCDICMove,                   /* move routine */
   RspEBCDICMove                 /* response routine */
   },
   {
   bf_22posentry,                /* field */
   HOST_MESSAGE.posentry,        /* data to move */
   12,                            /* # of bytes in  iso field */
   Move_Always,                  /* conditional */
   EBCDICMove,                   /* move routine */
   RspEBCDICMove                 /* response routine */
   },
   {
   bf_23csn,					/* field */
   HOST_MESSAGE.card_seq_num,        /* data to move */
   3,                            /* # of bytes in  iso field */
   Move_Always,                  /* conditional */
   EBCDICMove,                   /* move routine */
   RspEBCDICMove                 /* response routine */
   },
   {
   bf_24funccd,                  /* field */
   HOST_MESSAGE.func_cd,         /* data to move */
   3,                            /* # of bytes in field */
   Move_Always,                 /* conditional */
   EBCDICMove,                   /* move routine */
   RspEBCDICMove                 /* response routine */
   },
   {
   bf_25poscc,                   /* field */
   HOST_MESSAGE.pos_condcd,      /* data to move */
   4,                            /* # of bytes in  iso field */
   Move_Always,                  /* conditional */
   EBCDICMove,                   /* move routine */
   RspEBCDICMove                 /* response routine */
   },
    {
   bf_26accptcd,                 /* field */
   HOST_MESSAGE.accpt_bus_cd,    /* data to move */
   4,                            /* # of bytes in field */
   Move_Always,                  /* conditional */
   EBCDICMove,                   /* move routine */
   RspEBCDICMove                 /* response routine */
   },
   {
   bf_28tranfeeamt,              /* field */
   HOST_MESSAGE.tran_fee_amt,      /* data to move */
   8,                            /* # of bytes in  iso field */
   Move_Always,                  /* conditional */
   EBCDICMove,                   /* move routine */
   RspEBCDICMove                 /* response routine */
   },
   {
   bf_30origamt,                 /* field */
   HOST_MESSAGE.orig_amt,        /* data to move */
   24,                           /* # of bytes in field */
   Move_Always,                  /* conditional */
   EBCDICMove,                   /* move routine */
   RspEBCDICMove                 /* response routine */
   },
   {
   bf_31tranid,                  /* field */
   HOST_MESSAGE.tranid_len,    /* data to move */
   2,                            /* # of bytes in  iso field */
   Move_IfThere,                  /* conditional */
   vEBCDICMove,                  /* move routine */
   RspEBCDICvMove                /* response routine */
   },
   {
   bf_32idcode,                  /* field */
   HOST_MESSAGE.acq_inst_len,    /* data to move */
   2,                            /* # of bytes in  iso field */
   Move_Always,                  /* conditional */
   vEBCDICMove,                  /* move routine */
   RspEBCDICvMove                /* response routine */
   },
   {
   bf_33fwdinstidcode,                  /* field */
   HOST_MESSAGE.fwd_inst_len,    /* data to move */
   2,                            /* # of bytes in  iso field */
   Move_IfThere,                  /* conditional */
   vEBCDICMove,                  /* move routine */
   RspEBCDICvMove                /* response routine */
   },
   {
   bf_34panext,                  /* field */
   HOST_MESSAGE.pan_extd_len,    /* data to move */
   2,                            /* # of bytes in  iso field */
   Move_IfThere,                  /* conditional */
   vEBCDICMove,                  /* move routine */
   RspEBCDICvMove                /* response routine */
   },
   {
   bf_35track2,                  /* field */
   HOST_MESSAGE.track2_len,      /* data to move */
   2,                            /* # of bytes in track2 len field */
   Move_IfThere,                  /* conditional */
   vEBCDICMove,                  /* move routine */
   RspEBCDICvMove                /* response routine */
   },
   {
   bf_37rrn,                  /* field */
   HOST_MESSAGE.rrn,      /* data to move */
   12,                            /* # of bytes in track2 len field */
   Move_IfThere,                  /* conditional */
   EBCDICMove,                  /* move routine */
   RspEBCDICMove                /* response routine */
   },
   {
   bf_38authcode,                /* field */
   HOST_MESSAGE.authcode,        /* data to move */
   6,                            /* # of bytes in  iso field */
   Move_IfThere,                 /* conditional */
   EBCDICMove,                   /* move routine */
   RspEBCDICMove                 /* response routine */
   },
   {
   bf_39respcode,                /* field */
   HOST_MESSAGE.responsecd,      /* data to move */
   3,                            /* # of bytes in  iso field */
   Move_Always,                 /* conditional */
   EBCDICMove,                   /* move routine */
   RspEBCDICMove                 /* response routine */
   },
   {
   bf_41termid,                  /* field */
   HOST_MESSAGE.termid,          /* data to move */
   8,                            /* # of bytes in  iso field */
   Move_IfThere,                 /* conditional */
   EBCDICMove,                   /* move routine */
   RspEBCDICMove                 /* response routine */
   },
   {
   bf_42cardacc,                 /* field */
   HOST_MESSAGE.cardaccid,       /* data to move */
   15,                           /* # of bytes in  iso field */
   Move_IfThere,                  /* conditional */
   EBCDICMove,                   /* move routine */
   RspEBCDICMove                 /* response routine */
   },
   {
   bf_43cardacceptloc,                 /* field */
   HOST_MESSAGE.cardaccnamelen,       /* data to move */
   2,                           /* # of bytes in  iso field */
   Move_IfThere,                  /* conditional */
   vEBCDICMove,                   /* move routine */
   RspEBCDICMove                 /* response routine */
   },
   {
   bf_44respdata,                 /* field */
   HOST_MESSAGE.addl_rsp_len,       /* data to move */
   2,                           /* # of bytes in  iso field */
   Move_Always,                  /* conditional */
   vEBCDICMove,                   /* move routine */
   RspEBCDICvMove                 /* response routine */
   },
   {
   bf_45track1,                  /* field */
   HOST_MESSAGE.track1_len,      /* data to move */
   2,                            /* # of bytes in len field */
   Move_IfThere,                   /* conditional */
   vEBCDICMove,                  /* move routine */
   RspEBCDICvMove                /* response routine */
   },
   {
   bf_48addldata,                /* field */
   HOST_MESSAGE.addl_data_len,   /* data to move */
   3,                            /* # of bytes in track1 len field */
   Move_IfThere,                 /* conditional */
   vEBCDICMove,                  /* move routine */
   RspEBCDICvMove                /* response routine */
   },
   {
   bf_49trncurcd,                /* field */
   HOST_MESSAGE.tran_curr_cd,    /* data to move */
   3,                            /* # of bytes in  iso field */
   Move_Always,                  /* mandatory    */
   EBCDICMove,                   /* move routine */
   RspEBCDICMove                 /* response routine */
   },
   {
   bf_50recncurcd,               /* field */
   HOST_MESSAGE.recn_curr_cd,    /* data to move */
   3,                            /* # of bytes in field */
   Move_IfThere,                 /* mandatory    */
   EBCDICMove,                   /* move routine */
   RspEBCDICMove                 /* response routine */
   },
   {
   bf_52pinblock,                /* field */
   HOST_MESSAGE.pinblock,        /* data to move */
   8,                            /* # of bytes in  iso field */
   Move_Always,                  /* mandatory */
   GenericMove,                  /* move routine */
   RspMoveGeneric                /* response routine */
   },
   {
   bf_53seccntl,                 /* field */
   HOST_MESSAGE.sec_cntl_len,    /* data to move */
   2,                            /* # of bytes in field */
   Move_IfThere,                   /* mandatory    */
   vEBCDICMove,                   /* move routine */
   RspEBCDICMove                 /* response routine */
   },
   {
   bf_54AddlAmount,                 /* field */
   HOST_MESSAGE.AddlAmts_len,   /* data to move */
   3,                            /* # of bytes in field */
   Move_IfThere,                  /* mandatory    */
   vEBCDICMove,                  /* move routine */
   RspEBCDICvMove                /* response routine */
   },
   {
   bf_55IccData,                 /* field */
   HOST_MESSAGE.IccData_len,   /* data to move */
   3,                            /* # of bytes in field */
   Move_IfThere,                  /* mandatory    */
   vEBCDICMove,                  /* move routine */
   RspEBCDICvMove                /* response routine */
   },
   {
   bf_56orgdata,                 /* field */
   HOST_MESSAGE.orig_data_len,   /* data to move */
   2,                            /* # of bytes in field */
   Move_Always,                  /* mandatory    */
   vEBCDICMove,                  /* move routine */
   RspEBCDICvMove                /* response routine */
   },
   {   
   bf_60priv60,                 /* field */   
   HOST_MESSAGE.priv60_len,     /* data to move */  
   3,                           /* # of bytes in field */   
   Move_IfThere,                /* mandatory    */   
   vEBCDICMove,                  /* move routine */   
   RspEBCDICvMove                /* response routine */   
   },
   {   
   bf_61priv61,                 /* field */   
   HOST_MESSAGE.priv61_len,     /* data to move */  
   3,                           /* # of bytes in field */   
   Move_IfThere,                /* mandatory    */   
   vEBCDICMove,                  /* move routine */   
   RspEBCDICvMove    
   },
   {   
   bf_62priv62,                 /* field */   
   HOST_MESSAGE.priv62_len,     /* data to move */  
   3,                           /* # of bytes in field */   
   Move_IfThere,                /* mandatory    */   
   vEBCDICMove,                  /* move routine */   
   RspEBCDICvMove                /* response routine */   
   },
   {
   bf_63priv63,                 /* field */
   HOST_MESSAGE.priv63_len,     /* data to move */
   3,                           /* # of bytes in  iso field */
   Move_IfThere,                 /* mandatory    */
   vEBCDICMove,                  /* move routine */
   RspEBCDICvMove                /* response routine */
   },
   {
   bf_done,                      /* field */
   NULL,                         /* data to move */
   99,                           /* # of bytes in  iso field */
   NULL,                         /* conditional */
   NULL,                         /* move routine */
   NULL                          /* response routine */
   }
}; /* end BFData */


/*****************************************************************************

  Function:    Find_Bitfield_Table

  Description: This function will determine whether the bitfield is in the
               bitfield table.
  Author: 
      unknown
  Inputs:
      bitfield - the bitfield to search for
  Outputs:
      None
  Return values:
      pointer to the bitfield data for the bitfield entered.
  Modified by:
******************************************************************************/
static  struct bitfield_data *Find_Bitfield_Table (enum bitfields bitfield)
{
   struct bitfield_data *pBF = NULL;      /* point to return record       */
   INT      i;            /* loop index         */
   CHAR     temp_str[50] = "";

   for (i=0; i <= N_BFData; i++)
      if ( BFData[i].bd_fieldid == bitfield)
      {
         pBF = &BFData[i];
         return (pBF);
         break;
      }
 
   if (NULL == pBF)
   {
      printf(temp_str, "Bitfield: %d not found in BFData", bitfield);
      //TxUtils_Send_Msg_To_Operator (1, temp_str, 1, WARN_MSG, 
		//							           "Find_Bitfield_Table", 3, WARNING_ERROR, NULL_PTR,
         //                              NULL_PTR, NULL_PTR);
      return(NULL);
   };
   return(NULL);

} /* Find_Bitfield_Table */


/*****************************************************************************

  Function:    GenericMove

  Description: This function is the generic move function that will move
               data from the HOST_MESSAGE structure to the the raw ISO message.
  Author: 
      unknown
  Inputs:
      fieldid                - the bit field of the data to be moved
      HOST_MESSAGE (global)  - structure that contains data to go into the 
                               outgoing ISO message
  Outputs:
      ReqRsp_Buffer (global) - the buffer that contains the ISO formatted message
  Return values:
      None
  Modified by:
******************************************************************************/
static  void GenericMove(enum bitfields fieldid)
{
   struct bitfield_data *pBF;


   /*---------------------------------------------------*/
   /* find pointer to bitfield info for this bitfield...*/
        /*---------------------------------------------------*/
   pBF = Find_Bitfield_Table(fieldid); 

   if (pBF != NULL)
   {
      memcpy(&ReqRsp_Buffer[reqidx],      /* move to request buffer */
                 pBF->p_bd_pdata,         /* from data pointer      */
                  pBF->bd_fieldlen);      /* for length of field    */
      reqidx = reqidx + pBF->bd_fieldlen;   /* increment buffer pointer */
   }
} /* end GenericMove */

/*****************************************************************************

  Function:    bcd_to_asc

  Description: This function will convert bcd string to an ascii string 
  Author: 
      Farad       8/13/98
  Inputs:
      bstring  - the bcd string to conver to ascii
      blen     - the length of bstring
  Outputs:
      astr     - the new ascii string
      astr_len - the length of astr
  Return values:
      None
  Modified by:
******************************************************************************/
static void bcd_to_asc (BYTE bstring[], INT blen, char *astr, INT astr_len)
{
   int  i = 0;

   for (i = 0; i < (blen * 2); i=i+2)
      {
      astr[i] = (bstring[i/2] / 16) + '0';
      astr[i+1] = (bstring[i/2] & 0x0F) + '0';
      }

   astr [(blen * 2)] = '\0';
} /* bcd_to_asc */

/*****************************************************************************

  Function:    asc_to_bcd
  Description: This function will convert an ascii string to bcd.
  Author:
	  unknown
  Inputs:
	  afield - a pointer to the ascii string to convert
	  how_many - number of characters in p_astr to convert
  Outputs:
	  bstring - the bcd string
  Return values:
	  None
  Modified by:
	  FArad      8/14/98
******************************************************************************/
static  void asc_to_bcd(char *afield, int how_many, BYTE bstring[])
{
	pCHAR  astr;
	UINT   i = 0;

	astr = afield;

	for (i = (strlen(afield) - how_many); i < strlen(afield); i = i + 2)
	{
		if ('\0' == astr[i])
			break;

		if (astr[i] != ' ')
			bstring[i / 2] = ((astr[i] - '0') * 16);

		if (((i + 1) < strlen(afield)) && (astr[i + 1] != ' '))
			bstring[i / 2] += (astr[i + 1] - '0');

	}

} /* asc_to_bcd */

void convert_to_ascii(pBYTE source, pBYTE dest, INT len)
{
	INT  temp;
	BYTE temp_byte;

	if (!maps_initialised)
		init_character_code_maps();

	for (temp = 0; temp < len; temp++)
	{
		temp_byte = *(source + temp);
		*(dest + temp) = e2a[temp_byte];
	}

	return;
}

void genutil_asc_to_bin(pCHAR astr, pBYTE bstr, pINT bstr_len)
{
	INT  i, j;
	INT  len;

	len = strlen(astr);
	*bstr_len = 0;
	for (i = 0, j = 0; i < len; i += 2, j++)
	{
		/* Upper nibble */
		if (astr[i] <= '9')
			bstr[j] = (astr[i] & 0x0f) * 16;
		else
			bstr[j] = ((astr[i] & 0x0f) + 9) * 16;

		/* Lower nibble */
		if (astr[i + 1] <= '9')
			bstr[j] |= (astr[i + 1] & 0x0f);
		else
			bstr[j] |= ((astr[i + 1] & 0x0f) + 9);

		(*bstr_len)++;
	}
	return;
}

void hex_to_ascii(pBYTE instr, pBYTE outstr, INT len)
{
	INT   i, j, outstr_idx;
	BYTE  temp_byte;

	BYTE  hex_chars[16] = { 0x00, 0x01, 0x02, 0x03,
							  0x04, 0x05, 0x06, 0x07,
							  0x08, 0x09, 0x0a, 0x0b,
							  0x0c, 0x0d, 0x0e, 0x0f };

	BYTE  ascii_chars[16] = { '0',  '1',  '2',  '3',
							   '4',  '5',  '6',  '7',
							   '8',  '9',  'A',  'B',
							   'C',  'D',  'E',  'F' };

	/* Loop through the input string for each byte. */
	for (i = 0, outstr_idx = 0; i < len; i++, outstr_idx += 2)
	{
		/* Convert upper half of byte to ASCII. */
		temp_byte = instr[i] & 0xf0;

		/* Shift the bits 4 positions to the right. */
		temp_byte >>= 4;

		/* Find the ASCII equivalent. */
		for (j = 0; j < 16; j++)
		{
			if (hex_chars[j] == temp_byte)
			{
				outstr[outstr_idx] = ascii_chars[j];
				break;
			}
		}

		/* Convert the lower half of byte to ASCII. */
		temp_byte = instr[i] & 0x0f;

		/* Find the ASCII equivalent. */
		for (j = 0; j < 16; j++)
		{
			if (hex_chars[j] == temp_byte)
			{
				outstr[outstr_idx + 1] = ascii_chars[j];
				break;
			}
		}
	} /* End outer loop */


	return;
}

void genutil_hex_to_bin(pCHAR afield, INT how_many, BYTE bstring[])
{
	pCHAR   astr;
	INT     index;


	astr = afield;

	for (index = 0; index < how_many; index = index + 2)
	{
		if ((astr[index] >= '0') && (astr[index] <= '9'))
			bstring[index / 2] = ((INT)(astr[index] - '0')) * 16;
		else
			if ((astr[index] >= 'A') && (astr[index] <= 'F'))
				bstring[index / 2] = ((INT)(astr[index] - 'A') + 10) * 16;

		if ((astr[index + 1] >= '0') && (astr[index + 1] <= '9'))
			bstring[index / 2] += ((INT)(astr[index + 1] - '0'));
		else
			if ((astr[index + 1] >= 'A') && (astr[index + 1] <= 'F'))
				bstring[index / 2] += ((INT)(astr[index + 1] - 'A') + 10);
	} /* end of for*/
} /* genutil_hex_to_bin */

void hex_to_ebcdic(pBYTE instr, pBYTE outstr, INT len)
{
	INT   i, j, outstr_idx;
	BYTE  temp_byte;

	BYTE  hex_chars[16] = { 0x00, 0x01, 0x02, 0x03,
							  0x04, 0x05, 0x06, 0x07,
							  0x08, 0x09, 0x0a, 0x0b,
							  0x0c, 0x0d, 0x0e, 0x0f };

	BYTE  ascii_chars[16] = { '0',  '1',  '2',  '3',
							   '4',  '5',  '6',  '7',
							   '8',  '9',  'A',  'B',
							   'C',  'D',  'E',  'F' };

	/* Loop through the input string for each byte. */
	for (i = 0, outstr_idx = 0; i < len; i++, outstr_idx += 2)
	{
		/* Convert upper half of byte to ASCII. */
		temp_byte = instr[i] & 0xf0;

		/* Shift the bits 4 positions to the right. */
		temp_byte >>= 4;

		/* Find the ASCII equivalent. */
		for (j = 0; j < 16; j++)
		{
			if (hex_chars[j] == temp_byte)
			{
				outstr[outstr_idx] = ascii_chars[j];
				break;
			}
		}

		/* Convert the lower half of byte to ASCII. */
		temp_byte = instr[i] & 0x0f;

		/* Find the ASCII equivalent. */
		for (j = 0; j < 16; j++)
		{
			if (hex_chars[j] == temp_byte)
			{
				outstr[outstr_idx + 1] = ascii_chars[j];
				break;
			}
		}
	} /* End outer loop */

	/* Outstr is now in ASCII.  Now convert it to EBCDIC.
	 * Len has doubled in size because it went from hex to ASCII.
	 */
	convert_to_ebcdic(outstr, outstr, len * 2);

	return;
}

void convert_to_ebcdic(pBYTE source, pBYTE dest, INT len)
{
	INT temp;
	BYTE temp_byte;

	if (!maps_initialised)
		init_character_code_maps();

	for (temp = 0; temp < len; temp++)
	{
		temp_byte = *(source + temp);
		*(dest + temp) = a2e[temp_byte];
	}

	return;
} /* convert_to_ebcdic */

void init_character_code_maps()
{
	// ascii to ebcdic map
	memset(a2e, 0, sizeof(a2e));

	a2e[0x30] = 0xF0; a2e[0x31] = 0xF1; a2e[0x32] = 0xF2; a2e[0x33] = 0xF3; a2e[0x34] = 0xF4;
	a2e[0x35] = 0xF5; a2e[0x36] = 0xF6; a2e[0x37] = 0xF7; a2e[0x38] = 0xF8; a2e[0x39] = 0xF9;
	a2e[0x61] = 0x81; a2e[0x62] = 0x82; a2e[0x63] = 0x83; a2e[0x64] = 0x84; a2e[0x65] = 0x85;
	a2e[0x66] = 0x86; a2e[0x67] = 0x87; a2e[0x68] = 0x88; a2e[0x69] = 0x89; a2e[0x6A] = 0x91;
	a2e[0x6B] = 0x92; a2e[0x6C] = 0x93; a2e[0x6D] = 0x94; a2e[0x6E] = 0x95; a2e[0x6F] = 0x96;
	a2e[0x70] = 0x97; a2e[0x71] = 0x98; a2e[0x72] = 0x99; a2e[0x73] = 0xA2; a2e[0x74] = 0xA3;
	a2e[0x75] = 0xA4; a2e[0x76] = 0xA5; a2e[0x77] = 0xA6; a2e[0x78] = 0xA7; a2e[0x79] = 0xA8;
	a2e[0x7A] = 0xA9; a2e[0x41] = 0xC1; a2e[0x42] = 0xC2; a2e[0x43] = 0xC3; a2e[0x44] = 0xC4;
	a2e[0x45] = 0xC5; a2e[0x46] = 0xC6; a2e[0x47] = 0xC7; a2e[0x48] = 0xC8; a2e[0x49] = 0xC9;
	a2e[0x4A] = 0xD1; a2e[0x4B] = 0xD2; a2e[0x4C] = 0xD3; a2e[0x4D] = 0xD4; a2e[0x4E] = 0xD5;
	a2e[0x4F] = 0xD6; a2e[0x50] = 0xD7; a2e[0x51] = 0xD8; a2e[0x52] = 0xD9; a2e[0x53] = 0xE2;
	a2e[0x54] = 0xE3; a2e[0x55] = 0xE4; a2e[0x56] = 0xE5; a2e[0x57] = 0xE6; a2e[0x58] = 0xE7;
	a2e[0x59] = 0xE8; a2e[0x5A] = 0xE9; a2e[0x20] = 0x40; a2e[0x2E] = 0x4B; a2e[0x3C] = 0x4C;
	a2e[0x28] = 0x4D; a2e[0x2B] = 0x4E; a2e[0x7C] = 0x4F; a2e[0x26] = 0x50; a2e[0x21] = 0x5A;
	a2e[0x24] = 0x5B; a2e[0x2A] = 0x5C; a2e[0x29] = 0x5D; a2e[0x3B] = 0x5E; a2e[0x2D] = 0x60;
	a2e[0x2F] = 0x61; a2e[0x2C] = 0x6B; a2e[0x25] = 0x6C; a2e[0x5F] = 0x6D; a2e[0x3E] = 0x6E;
	a2e[0x3F] = 0x6F; a2e[0x3A] = 0x7A; a2e[0x23] = 0x7B; a2e[0x40] = 0x7C; a2e[0x27] = 0x7D;
	a2e[0x3D] = 0x7E; a2e[0x22] = 0x7F; a2e[0x5E] = 0x5F;
	// TF Phani - added for Amex
	a2e[0x7E] = 0xA1; a2e[0x5C] = 0xE0;

	// ebcdic to ascii map
	memset(e2a, 0x00, sizeof(e2a));

	e2a[0xF0] = 0x30; e2a[0xF1] = 0x31; e2a[0xF2] = 0x32; e2a[0xF3] = 0x33; e2a[0xF4] = 0x34;
	e2a[0xF5] = 0x35; e2a[0xF6] = 0x36; e2a[0xF7] = 0x37; e2a[0xF8] = 0x38; e2a[0xF9] = 0x39;
	e2a[0x81] = 0x61; e2a[0x82] = 0x62; e2a[0x83] = 0x63; e2a[0x84] = 0x64; e2a[0x85] = 0x65;
	e2a[0x86] = 0x66; e2a[0x87] = 0x67; e2a[0x88] = 0x68; e2a[0x89] = 0x69; e2a[0x91] = 0x6A;
	e2a[0x92] = 0x6B; e2a[0x93] = 0x6C; e2a[0x94] = 0x6D; e2a[0x95] = 0x6E; e2a[0x96] = 0x6F;
	e2a[0x97] = 0x70; e2a[0x98] = 0x71; e2a[0x99] = 0x72; e2a[0xA2] = 0x73; e2a[0xA3] = 0x74;
	e2a[0xA4] = 0x75; e2a[0xA5] = 0x76; e2a[0xA6] = 0x77; e2a[0xA7] = 0x78; e2a[0xA8] = 0x79;
	e2a[0xA9] = 0x7A; e2a[0xC1] = 0x41; e2a[0xC2] = 0x42; e2a[0xC3] = 0x43; e2a[0xC4] = 0x44;
	e2a[0xC5] = 0x45; e2a[0xC6] = 0x46; e2a[0xC7] = 0x47; e2a[0xC8] = 0x48; e2a[0xC9] = 0x49;
	e2a[0xD1] = 0x4A; e2a[0xD2] = 0x4B; e2a[0xD3] = 0x4C; e2a[0xD4] = 0x4D; e2a[0xD5] = 0x4E;
	e2a[0xD6] = 0x4F; e2a[0xD7] = 0x50; e2a[0xD8] = 0x51; e2a[0xD9] = 0x52; e2a[0xE2] = 0x53;
	e2a[0xE3] = 0x54; e2a[0xE4] = 0x55; e2a[0xE5] = 0x56; e2a[0xE6] = 0x57; e2a[0xE7] = 0x58;
	e2a[0xE8] = 0x59; e2a[0xE9] = 0x5A; e2a[0x40] = 0x20; e2a[0x4B] = 0x2E; e2a[0x4C] = 0x3C;
	e2a[0x4D] = 0x28; e2a[0x4E] = 0x2B; e2a[0x4F] = 0x7C; e2a[0x50] = 0x26; e2a[0x5A] = 0x21;
	e2a[0x5B] = 0x24; e2a[0x5C] = 0x2A; e2a[0x5D] = 0x29; e2a[0x5E] = 0x3B; e2a[0x60] = 0x2D;
	e2a[0x61] = 0x2F; e2a[0x6B] = 0x2C; e2a[0x6C] = 0x25; e2a[0x6D] = 0x5F; e2a[0x6E] = 0x3E;
	e2a[0x6F] = 0x3F; e2a[0x7A] = 0x3A; e2a[0x7B] = 0x23; e2a[0x7C] = 0x40; e2a[0x7D] = 0x27;
	e2a[0x7E] = 0x3D; e2a[0x7F] = 0x22; e2a[0x5F] = 0x5E;

	// TF Phani -
	e2a[0xA1] = 0x7E; e2a[0xE0] = 0x5C;

	maps_initialised = true;

} /* init_character_code_maps */

/*****************************************************************************

  Function:    RspMoveGeneric

  Description: This function is the generic move function that will move
               data from the raw ISO message to the HOST_MESSAGE structure.
  Author: 
      Farad       8/13/98
  Inputs:
      ReqRsp_Buffer (global) - the buffer that contains the ISO formatted message
      fieldid                - the bit field of the data to be moved
  Outputs:
      HOST_MESSAGE (global)  - structure that contains data from the incominging ISO message
  Return values:
      None
  Modified by:
******************************************************************************/
static  void RspMoveGeneric(enum bitfields fieldid)
{
   struct bitfield_data *pBF;


   /*---------------------------------------------------*/
   /* find pointer to bitfield info for this bitfield...*/
        /*---------------------------------------------------*/
   pBF = Find_Bitfield_Table(fieldid);

   

   if (pBF != NULL)
   {
	   if(fieldid == 52)
	   {
			genutil_bin_to_ascii(&ReqRsp_Buffer[reqidx],8,(char *)pBF->p_bd_pdata,16);
	   }
	   else
	   {
		 memcpy(pBF->p_bd_pdata,         /* to data pointer      */
             &ReqRsp_Buffer[reqidx],      /* move from request buffer */
             pBF->bd_fieldlen);    
	   }/* for length of field    */
   
      reqidx = reqidx + pBF->bd_fieldlen;   /* increment buffer pointer */
   }
} /* end RspMoveGeneric */


/*****************************************************************************

  Function:   ascii_to_bin

  Description:
      Convert an ascii string to a binary string
  Author: 
      SVelaga   6/21/99
  Inputs:
      astr      - pointer to the incoming ascii string
      ascii_len - length of astr
      blen      - length of bstring
  Outputs:
      binary_string  - pointer to the outgoing binary string
  Return values:
      NONE
  Modified by:

******************************************************************************/
void ascii_to_bin_field55( CHAR *astr, INT ascii_len, BYTE *binary_string, INT blen)
{

	BYTE temp_char;
	INT i , j = 0;

	
	memset(binary_string, 0x00, blen);
	

	for(i=0, j=0; ((i < blen ) && (j < ascii_len)); i++, j+=2)
	{
		 	
		if( astr[j] <= '9' )
		{
			if(astr[j+1] <= '9')
				temp_char = (( (unsigned char )(astr[j] & 0x0f ) << 4 ) |  (unsigned char )(astr[j+1] & 0x0f ));
			else
				temp_char = (( (unsigned char )(astr[j] & 0x0f ) << 4 ) |  (unsigned char )((astr[j+1] + '9') & 0x0f ));

		}
		else 
		{
			if(astr[j+1] <= '9')
				temp_char = (( (unsigned char )((astr[j] + '9')  & 0x0f ) << 4 ) |  (unsigned char )(astr[j+1]  & 0x0f ));
			else
				temp_char = (( (unsigned char )((astr[j] + '9')  & 0x0f ) << 4 ) |  (unsigned char )((astr[j+1] + '9') & 0x0f ));
		}
		 
			binary_string[i] = temp_char;
	}
}




/*****************************************************************************

  Function:    vEBCDICMove

  Description: This function is the  move function that will move data
               from the HOST_MESSAGE structure (after converting it to 
               EBCDIC) to the the raw ISO message.  Note: it moves the 
               number indicated by the length variable pointed to by 
               pBF->p_db_data[0].
  Author: 
      unknown
  Inputs:
      fieldid                - the bit field of the data to be moved
      HOST_MESSAGE (global)  - structure that contains data to go into the 
                               outgoing ISO message
  Outputs:
      ReqRsp_Buffer (global) - the buffer that contains the ISO formatted message
  Return values:
      None
  Modified by:
******************************************************************************/
static  void vEBCDICMove( enum bitfields fieldid )
{
   struct bitfield_data *pBF;
   INT                  movelen;
   INT                  i;
   CHAR                 tempstr[4];

      /***************************/
		  char val_1[5] = "AGNS";
		  int j = 0;
			char header_version[5]="0001";
			BYTE val_2[3]={0};
		  char val_3[3] = "91";
		  char val_4[7] = "ABCDEF";
		  char val_6[3]		=	"00";
		  char rsp_55_msg[18] ={0}; //"AGNS000191ABCDEFXX00";
		  int temp_val = 0;
		 
		  


   /***************************/
   /*---------------------------------------------------*/
   /* find pointer to bitfield info for this bitfield...*/
   /*---------------------------------------------------*/
   pBF = Find_Bitfield_Table(fieldid);

   /* Determine the value of the variable length field.  Note: for
      AMEX, even the length field is in EBCDIC.  For Visa, the
      length comes in as binary. */

   if (pBF != NULL)
   {
      memset (tempstr, 0, sizeof(tempstr));
      strncpy((char*)tempstr, (char*)pBF->p_bd_pdata,pBF->bd_fieldlen);  // needed because no nulls in HOST_MESSAGE
      movelen = atoi(tempstr);
      for( i=0; i<pBF->bd_fieldlen; i++ )
      {
         ReqRsp_Buffer [reqidx] = ascii_ebcdic_conv (pBF->p_bd_pdata[i], A2E);
         reqidx++;
      }

		if(fieldid == 55)
	  {
		  // Copy the data based on above len. Do the memcpy
			/*sprintf(rsp_55_msg,"%s",val_1,sizeof(val_1));
			ascii_to_bin_field55(header_version,strlen(header_version),&val_2[0],4);
			sprintf(rsp_55_msg,"%s%s",rsp_55_msg,val_2,sizeof(val_2));
			sprintf(rsp_55_msg,"%s%d",rsp_55_msg,val_3,sizeof(val_3));
			sprintf(rsp_55_msg,"%s%s",rsp_55_msg,val_4,sizeof(val_4));
		  /*for(i = 0;i<21;i++)
		  {
			sprintf(&rsp_55_msg[16],"%d",temp_55_counter,2);
			temp_55_counter++;
		  }----
			//sprintf(&rsp_55_msg[16],"%d",temp_55_counter,2);
		  sprintf(rsp_55_msg,"%s%02d",rsp_55_msg,temp_55_counter,2);
			temp_55_counter++;
		  sprintf(rsp_55_msg,"%s%s",rsp_55_msg,val_6,sizeof(val_6));
		  
		  sprintf(&pBF->p_bd_pdata[3],"%s",rsp_55_msg,sizeof(rsp_55_msg));*/

		    memcpy(rsp_55_msg,val_1,sizeof(val_1));
			ascii_to_bin_field55(header_version,strlen(header_version),&val_2[0],4);
			memcpy(&rsp_55_msg[4],val_2,sizeof(val_2));
			memcpy(&rsp_55_msg[6],val_3,sizeof(val_3));
			memcpy(&rsp_55_msg[8],val_4,sizeof(val_4));
			sprintf(&rsp_55_msg[13],"02%d",temp_55_counter);
			temp_55_counter++;
			memcpy(&rsp_55_msg[15],val_6,sizeof(val_6));
			memcpy(&pBF->p_bd_pdata[3],rsp_55_msg,sizeof(rsp_55_msg));
		 /* temp_val = (movelen+pBF->bd_fieldlen)-18;
		  
		  for(j = 18;j<temp_val;j++)
		  {
			 reqidx++;
		  }*/
		  for( i=pBF->bd_fieldlen; i<movelen+pBF->bd_fieldlen; i++ )
		{
			 ReqRsp_Buffer[reqidx] = ascii_ebcdic_conv (pBF->p_bd_pdata[i], A2E);
			reqidx++;
		}
	  }
	  else
	  {
		 for( i=pBF->bd_fieldlen; i<movelen+pBF->bd_fieldlen; i++ )
		{
			 ReqRsp_Buffer[reqidx] = ascii_ebcdic_conv (pBF->p_bd_pdata[i], A2E);
			reqidx++;
		}
	  }


   }
} /* end vEBCDICMove */

/*****************************************************************************

  Function:    EBCDICMove

  Description: This function is the  move function that will move data
               from the HOST_MESSAGE structure (after converting it to 
               EBCDIC) to the the raw ISO message.  Note: it moves the 
               number indicated by the length variable pointed to by 
               pBF->bd_fieldlen.
  Author: 
      unknown
  Inputs:
      fieldid                - the bit field of the data to be moved
      HOST_MESSAGE (global)  - structure that contains data to go into the 
                               outgoing ISO message
  Outputs:
      ReqRsp_Buffer (global) - the buffer that contains the ISO formatted message
  Return values:
      None
  Modified by:
******************************************************************************/
static  void EBCDICMove( enum bitfields fieldid )
{
   struct bitfield_data *pBF;
   INT                  movelen;
   INT                  i;

   /*---------------------------------------------------*/
   /* find pointer to bitfield info for this bitfield...*/
   /*---------------------------------------------------*/
   pBF = Find_Bitfield_Table(fieldid);

   if (pBF != NULL)
   {
      movelen = pBF->bd_fieldlen;

      for( i=0; i<movelen; i++ )
      {
         ReqRsp_Buffer[reqidx] = ascii_ebcdic_conv (pBF->p_bd_pdata[i], A2E);
         reqidx++;
      }
   }
} /* end EBCDICMove */

  
BYTE getFldSep( unsigned char* resp_buff, int index )
{
   INT   flg=1,cnt=0;
   BYTE  returnChar=0x00;

      cnt=index;
      while( flg == 1)
      {
        switch( resp_buff[cnt])
        {
        case 0xF0:
        case 0xF1:
        case 0xF2:
        case 0xF3:
        case 0xF4:
        case 0xF5:
        case 0xF6:
        case 0xF7:
        case 0xF8:
        case 0xF9:
        case 0xC2: cnt++;break; //"B" Format Specifier
        default:flg=0; returnChar=resp_buff[cnt];break;
        }
      }
      return( returnChar );
}   //end of getFldSep


  
BYTE ascii_ebcdic_conv_trk1( unsigned char conv_char, char *conv_type,unsigned char fldSep)
{
   INT            i;
   INT    found = 0;
   BYTE  ReturnChar;

      /*
        +------------------------------------------------------------+
        |   0     1     2     3     4     5     6     7     8     9  |
        |   a     b     c     d     e     f     g     h     i     j  |
        |   k     l     m     n     o     p     q     r     s     t  |
        |   u     v     w     x     y     z     A     B     C     D  |
        |   E     F     G     H     I     J     K     L     M     N  |
        |   O     P     Q     R     S     T     U     V     W     X  |
        |   Y     Z           .     <     (     +     |     &     !  |
        |   $     *     )     ;     -     /     ,     %     _     >  |
        |   ?     :     #     @     '     =     "     ^              |
        +------------------------------------------------------------+
      */

   static unsigned char ebcdic_chart[89] = 
      {
         0xF0, 0xF1, 0xF2, 0xF3, 0xF4, 0xF5, 0xF6, 0xF7, 0xF8, 0xF9,
         0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x91,
         0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0xA2, 0xA3,
         0xA4, 0xA5, 0xA6, 0xA7, 0xA8, 0xA9, 0xC1, 0xC2, 0xC3, 0xC4,
         0xC5, 0xC6, 0xC7, 0xC8, 0xC9, 0xD1, 0xD2, 0xD3, 0xD4, 0xD5,
         0xD6, 0xD7, 0xD8, 0xD9, 0xE2, 0xE3, 0xE4, 0xE5, 0xE6, 0xE7,
         0xE8, 0xE9, 0x40, 0x4B, 0x4C, 0x4D, 0x4E, 0x4F, 0x50, 0x5A,
         0x5B, 0x5C, 0x5D, 0x5E, 0x60, 0x61, 0x6B, 0x6C, 0x6D, 0x6E,
         0x6F, 0x7A, 0x7B, 0x7C, 0x7D, 0x7E, 0x7F, 0xB0, 0x8A
      };

   static unsigned char ascii_chart[89] = 
      {
         0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39,
         0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6A,
         0x6B, 0x6C, 0x6D, 0x6E, 0x6F, 0x70, 0x71, 0x72, 0x73, 0x74,
         0x75, 0x76, 0x77, 0x78, 0x79, 0x7A, 0x41, 0x42, 0x43, 0x44,
         0x45, 0x46, 0x47, 0x48, 0x49, 0x4A, 0x4B, 0x4C, 0x4D, 0x4E,
         0x4F, 0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58,
         0x59, 0x5A, 0x20, 0x2E, 0x3C, 0x28, 0x2B, 0x7C, 0x26, 0x21,
         0x24, 0x2A, 0x29, 0x3B, 0x2D, 0x2F, 0x2C, 0x25, 0x5F, 0x3E,
         0x3F, 0x3A, 0x23, 0x40, 0x27, 0x3D, 0x22, 0x5E, 0x5E
      };


   if ( 0 == strcmp( conv_type, A2E ) )
   {
      /* CONVERT ASCII TO EBCDIC */
	  for( i=0; i<88 && !found; i++ )
      {
         if ( conv_char == ascii_chart[i] )
         {
            ReturnChar = ebcdic_chart[i];
            found = 1;
         }
      }
   }
   else if ( 0 == strcmp( conv_type, E2A ) )
   {
      /* CONVERT EBCDIC TO ASCII */
      for( i=0; i<88 && !found; i++ )
      {
         if ( conv_char == ebcdic_chart[i] )
         {
            ReturnChar = ascii_chart[i];
            found = 1;
         }

      }
      if(fldSep==conv_char)
      {
        ReturnChar = 0x5E;
        found = 1;
      }

   }

   if ( 0 == found )
      ReturnChar = 0x00;

   return( ReturnChar );
}   //end of the ascii_ebcdic_conv_trk1
  


/*****************************************************************************

  Function:    RspEBCDICvMove

  Description: This function will move data from the the raw ISO message 
               (after converting it from EBCDIC) to the HOST_MESSAGE structure.  
               Note: it moves the number indicated by the length variable pointed 
               to by pBF->p_db_data[0].
  Author: 
      unknown
  Inputs:
      fieldid                - the bit field of the data to be moved
      ReqRsp_Buffer (global) - the buffer that contains the ISO formatted message
  Outputs:
      HOST_MESSAGE (global)  - structure that contains data to go into the 
                               outgoing ISO message
  Return values:
      None
  Modified by:
******************************************************************************/
static  void RspEBCDICvMove (enum bitfields fieldid)
{
   struct bitfield_data *pBF;
   int                  i;
   int                  movelen;
   char                 templen[5];
   BYTE    fld_sep;

   /*---------------------------------------------------*/
   /* Find pointer to bitfield info for this bitfield...*/
   /*---------------------------------------------------*/
   pBF = Find_Bitfield_Table(fieldid);

   /* Determine the value of the variable length field.  Note: for
      AMEX, even the length field is in EBCDIC.  For Visa, the
      length comes in as binary. */

   if (pBF != NULL)
   {
      memset( templen, 0x00, sizeof(templen) );
      for( i=0; i<pBF->bd_fieldlen; i++ )
      {
         templen[i] = ascii_ebcdic_conv (ReqRsp_Buffer[reqidx], E2A);
         reqidx++;
      }
      movelen = atoi(templen);
      strcpy((char*)pBF->p_bd_pdata, templen);
     
      if(fieldid == 45) //TRACK1
      {
         //call the function that returns the first non-numeric character
         fld_sep= getFldSep( ReqRsp_Buffer, reqidx); 
          /* Move the data, converting it from EBCDIC to ASCII. */
          for( i=pBF->bd_fieldlen; i<movelen+pBF->bd_fieldlen; i++)
          {
             pBF->p_bd_pdata [i] = ascii_ebcdic_conv_trk1  (ReqRsp_Buffer[reqidx], E2A,fld_sep);
             reqidx++;
          }

      }
	  else if(fieldid == 55) //EMV data
      {
		 
			memcpy(HOST_MESSAGE.IccData,(char *)&ReqRsp_Buffer[reqidx],movelen);
		  /* incremnting the reqidx index*/
		  for( i=pBF->bd_fieldlen; i<movelen+pBF->bd_fieldlen; i++)
          {
			  reqidx++;
		  }

		  char temp_str_emv[1000] = {0};
		  BYTE val_2[5]={0};
		  char header_version[5]="0001";

		  generate_AMEX_EMV_data();
		  Generate_EMV_Details();
		  
		  sprintf(temp_str_emv,"%s",(char*)templen);
		  sprintf(&temp_str_emv[3],"AGNS");
		  sprintf(&temp_str_emv[7],"0001");   
		  sprintf(&temp_str_emv[11],"%s",(char*)EMV_details.app_crypto);   
		  sprintf(&temp_str_emv[19],"%-033s",(char*)EMV_details.iad );  
		  sprintf(&temp_str_emv[52],"%s",(char*)EMV_details.unpredictable_number );
		  sprintf(&temp_str_emv[56],"%s",(char*)EMV_details.atc );
		  sprintf(&temp_str_emv[58],"%s",(char*)EMV_details.tvr);  
		  sprintf(&temp_str_emv[63],"%s",(char*)EMV_details.tran_date); 
		  sprintf(&temp_str_emv[66],"%s",(char*)EMV_details.tran_type); 
		  sprintf(&temp_str_emv[67],"%s",(char*)EMV_details.tran_amount);   
		  sprintf(&temp_str_emv[73],"%s",(char*)EMV_details.currency_code);      
		  sprintf(&temp_str_emv[75],"%s",(char*)EMV_details.country_code); 
		  sprintf(&temp_str_emv[77],"%s",(char*)EMV_details.interchange_profile);
		  sprintf(&temp_str_emv[79],"%s",(char*)EMV_details.other_amount); 
		  sprintf(&temp_str_emv[85],"%s",(char*)EMV_details.pan_sequence_number);
		  sprintf(&temp_str_emv[86],"%s",(char*)EMV_details.crypto_info);  

		  sprintf((char *)pBF->p_bd_pdata,"%s",temp_str_emv);
		  
      }
	  /*raghu */
	  else if(fieldid == 61)
	  {
		  // HOST_MESSAGE.priv61_len
		  memcpy(HOST_MESSAGE.priv61,(char *)&ReqRsp_Buffer[reqidx],movelen);
		  reqidx += movelen;

		  get_field_61();
		  sprintf((char *)pBF->p_bd_pdata, "%s", field_61);
	  }
	  else if (fieldid == 62)
	  {
		  int index = 0, bytes_read = 0;

		  for( i=pBF->bd_fieldlen; i<8; i++) //10bytes after length
		  {
			  pBF->p_bd_pdata [i] = ascii_ebcdic_conv (ReqRsp_Buffer[reqidx], E2A);
			  reqidx++;
		  }
		  index = i;
		  bytes_read = 10;

		  bcd_to_asc(&ReqRsp_Buffer[reqidx], 4, (char*)&pBF->p_bd_pdata[index], 8);
		  reqidx += 4;
		  index += 8;
		  bytes_read += 8;

		  for (int k = 0; k < ((movelen * 2) - bytes_read) / 2; k++)
		  {
			  pBF->p_bd_pdata[index] = ascii_ebcdic_conv(ReqRsp_Buffer[reqidx], E2A);
			  reqidx++;
			  index++;
		  }
	  }

      else
      {
          /* Move the data, converting it from EBCDIC to ASCII. */
          for( i=pBF->bd_fieldlen; i<movelen+pBF->bd_fieldlen; i++)
          {
             pBF->p_bd_pdata [i] = ascii_ebcdic_conv (ReqRsp_Buffer[reqidx], E2A);
             reqidx++;
          }
      }
   }
} /* end RspEBCDICvMove */


  
  /*****************************************************************************

  Function:    RspEBCDICMove

  Description: This function will move data from the the raw ISO message 
               (after converting it from EBCDIC) to the HOST_MESSAGE structure.  
               Note: it moves the number indicated by the length variable pointed
               to by pBF->bd_fieldlen.
  Author: 
      unknown
  Inputs:
      fieldid                - the bit field of the data to be moved
      HOST_MESSAGE (global)  - structure that contains data to go into the 
                               outgoing ISO message
  Outputs:
      ReqRsp_Buffer (global) - the buffer that contains the ISO formatted message
  Return values:
      None
  Modified by:
      BGable      6/30/98
******************************************************************************/
static  void RspEBCDICMove (enum bitfields fieldid)
{
   struct bitfield_data *pBF;
   int                  i;
   int                  movelen;
   char asci_len[4]={0};
   

   /*---------------------------------------------------*/
   /* Find pointer to bitfield info for this bitfield...*/
   /*---------------------------------------------------*/
   pBF = Find_Bitfield_Table(fieldid);
   if (pBF != NULL)
   {
	   if(fieldid==43 || fieldid == 53)
	   {
		   for( i=0; i<pBF->bd_fieldlen; i++)
			 {
				asci_len[i] =ascii_ebcdic_conv(ReqRsp_Buffer[reqidx],E2A);
				pBF->p_bd_pdata [i]= asci_len[i];
				reqidx++;
			}
		   movelen=atoi(asci_len);
		   for( i=0; i<movelen; i++)
           {
				pBF->p_bd_pdata [i+pBF->bd_fieldlen] = ascii_ebcdic_conv (ReqRsp_Buffer[reqidx], E2A);
				reqidx++;
			}
	   }
	   else
	   {
			movelen = pBF->bd_fieldlen;
			for( i=0; i<movelen; i++)
			{
				pBF->p_bd_pdata [i] = ascii_ebcdic_conv (ReqRsp_Buffer[reqidx], E2A);
				reqidx++;
			}
	   }

      /* Move the data, converting it from EBCDIC to ASCII. */
      



   }
} /* end RspEBCDICMove */

/*****************************************************************************

  Function:    Rj_with_lead_zeros

  Description: This function will right justify a string and add leading zeros
  Author: 
      unknown
  Inputs:
      str        - the ascii string to be right justified 
      FieldWidth
  Outputs:
      str        - the right justified ascii string
  Return values:
      None
  Modified by:
******************************************************************************/
void Rj_with_lead_zeros( char * str, INT FieldWidth )
{
   INT i;
   INT dif;
   INT strlength;

   strlength = strlen(str);

   /* Convert any leading blanks to zero. */
   for (i = 0; i < strlength; i++)
   {
      if ( 0x20 == str[i] )
         str [i] = 0x30;
      else
         break;
   }

   /* Remove any trailing blanks. */
   for (i = strlength-1; i >= 0; i--)
   {
      if ( 0x20 == str[i] )
         str [i] = 0x00;
      else
         break;
   }

   strlength = strlen(str);
   dif       = FieldWidth - strlength;

   if (dif > 0)
   {
      /* Right justify, then prepend leading zeros. */
      memmove (&str [dif], &str[0], strlength+1);
      memset  (&str[0], 0x30, dif);
   }
}

/*****************************************************************************

  Function:    RspIgnore  

  Description: This function will not copy the response field, but will increment
               the index by the length of the response field.
  Author: 
      unknown
  Inputs:
      fieldid   - the bit field of the data not to be moved
  Outputs:
      None
  Return values:
      None
  Modified by:
******************************************************************************/
static  void RspIgnore(enum bitfields fieldid)
{
   struct bitfield_data *pBF;


   /*---------------------------------------------------*/
   /* find pointer to bitfield info for this bitfield...*/
   /*---------------------------------------------------*/
   pBF = Find_Bitfield_Table(fieldid);

   if (pBF != NULL)
   {
      reqidx = reqidx + pBF->bd_fieldlen;   /* increment buffer pointer */
   }
} /* end RspIgnore */

/*****************************************************************************

  Function:    RspIgnore  

  Description: This function will not copy the response field, but will increment
               the index by the variable length of the response field.
  Author: 
      unknown
  Inputs:
      fieldid   - the bit field of the data not to be moved
  Outputs:
      None
  Return values:
      None
  Modified by:
      Farad      8/13/98
******************************************************************************/
static  void vRspIgnore(enum bitfields fieldid)
{
   struct bitfield_data *pBF;
   int                  movelen;

   /*---------------------------------------------------*/
   /* Find pointer to bitfield info for this bitfield...*/
   /*---------------------------------------------------*/
   pBF = Find_Bitfield_Table(fieldid);

   /* Determine number of bytes to move. */
   if (pBF != NULL)
   {
      movelen = ReqRsp_Buffer[reqidx] + pBF->bd_fieldlen;

      reqidx = reqidx + movelen;   /* increment buffer pointer */
   }
} /* end vRspIgnore */

/*****************************************************************************

  Function:    bcd_to_int   

  Description: This function will convert a bcd string into an integer. 
  Author: 
      unknown
  Inputs:
      bcd_data  - a pointer to the bcd string to be converted
      bcd_len   - the length of the string pointed to by bcd_data
  Outputs:
      None
  Return values:
      INT - the integer created by converting bcd_data to an integer.
  Modified by:
******************************************************************************/
static  INT bcd_to_int ( BYTE *bcd_data,
                 int           bcd_len    )
{
   INT   temp_int;
   INT   multiplier;
   INT   i;


   multiplier = 1;
   temp_int   = 0;
   
   for (i = (bcd_len-1); i >= 0; i--)
      {
      temp_int+= (bcd_data [i] & 0x0f) * multiplier;
      multiplier = multiplier * 10;
      temp_int+= (bcd_data [i] / 16) * multiplier;
      multiplier = multiplier * 10;
      }

   return (temp_int);
}

/*****************************************************************************

  Function:    vBCDMove 

  Description: This function will move data from the HOST_MESSAGE structure to 
               the the raw ISO message.  Note: it moves the number indicated 
               by the length variable pointed to by pBF->p_db_data[0].
  Author: 
      unknown
  Inputs:
      fieldid                - the bit field of the data to be moved
      HOST_MESSAGE (global)  - structure that contains data to go into the 
                               outgoing ISO message
  Outputs:
      ReqRsp_Buffer (global) - the buffer that contains the ISO formatted message
  Return values:
      None
  Modified by:
      BGable      6/30/98
******************************************************************************/
static  void vBCDMove(enum bitfields fieldid)
{
   struct bitfield_data *pBF;
   INT                  movelen;

   /*---------------------------------------------------*/
   /* find pointer to bitfield info for this bitfield...*/
   /*---------------------------------------------------*/
   pBF = Find_Bitfield_Table(fieldid);

   /* Determine number of bytes to move. */
   if (pBF != NULL)
   {
      movelen = pBF->p_bd_pdata[0];
      ReqRsp_Buffer[reqidx] = pBF->p_bd_pdata[0];
      reqidx++;

      //if ( 60 == fieldid )
      //   movelen *= 2;

      if ((movelen % 2) != 0)
         movelen++;

      movelen = (movelen / 2);

      memcpy(&ReqRsp_Buffer[reqidx], &pBF->p_bd_pdata[1], movelen);

      reqidx = reqidx + movelen;   /* increment buffer pointer */
   }
} /* end vBCDMove */

/*****************************************************************************

  Function:    

  Description: This function will move data from the raw ISO message to 
               the HOST_MESSAGE structure.  
  Author: 
      unknown
  Inputs:
      fieldid                - the bit field of the data to be moved
      ReqRsp_Buffer (global) - the buffer that contains the ISO formatted message
  Outputs:
      HOST_MESSAGE (global)  - structure for the outgoing ISO message
  Return values:
      None
  Modified by:
      BGable      6/30/98
******************************************************************************/
static  void RspMovevBCD (enum bitfields fieldid)
{
   struct bitfield_data *pBF;
   INT                  movelen;

   /*---------------------------------------------------*/
   /* find pointer to bitfield info for this bitfield...*/
   /*---------------------------------------------------*/
   pBF = Find_Bitfield_Table(fieldid);

   if (pBF != NULL)
   {
      movelen = ReqRsp_Buffer[reqidx];
      pBF->p_bd_pdata[0] = ReqRsp_Buffer[reqidx];
      reqidx++;

      //if ( 60 == fieldid )
      //   movelen *= 2;

      if ((movelen % 2) != 0)
         movelen++;

      movelen/= 2;

      memcpy(&pBF->p_bd_pdata[1], &ReqRsp_Buffer[reqidx], movelen);

      reqidx = reqidx + movelen;   /* increment buffer pointer */
   }
} /* end RspMovevBCD */

/*****************************************************************************

  Function:    vDataMove

  Description: This function will move data from the HOST_MESSAGE structure to 
               the the raw ISO message.  Note: it moves the number indicated 
               by the length variable pointed to by pBF->p_db_data[0].
  Author: 
      unknown
  Inputs:
      fieldid                - the bit field of the data to be moved
      HOST_MESSAGE (global)  - structure that contains data to go into the 
                               outgoing ISO message
  Outputs:
      ReqRsp_Buffer (global) - the buffer that contains the ISO formatted message
  Return values:
      None
  Modified by:
******************************************************************************/
static  void vDataMove(enum bitfields fieldid)
{
   struct bitfield_data *pBF;
   INT                  movelen;

   /*---------------------------------------------------*/
   /* find pointer to bitfield info for this bitfield...*/
   /*---------------------------------------------------*/
   pBF = Find_Bitfield_Table(fieldid);

   if (pBF != NULL)
   {
      movelen = pBF->p_bd_pdata [0];
      ReqRsp_Buffer [reqidx] = pBF->p_bd_pdata [0];
      reqidx++;

      memcpy(&ReqRsp_Buffer[reqidx], &pBF->p_bd_pdata[1], movelen);

      reqidx = reqidx + movelen;   /* increment buffer pointer */
   }
} /* end vDataMove */

/*****************************************************************************

  Function:    RspMovevData  

  Description: This function will move data from the the raw ISO message 
               to the HOST_MESSAGE structure.  Note: it moves the number 
               indicated by the length variable pointed to by pBF->p_db_data[0].
  Author: 
      unknown
  Inputs:
      fieldid                - the bit field of the data to be moved
      ReqRsp_Buffer (global) - the buffer that contains the ISO formatted message
  Outputs:
      HOST_MESSAGE (global)  - structure that contains data to go into the 
                               outgoing ISO message
  Return values:
      None
  Modified by:
******************************************************************************/
static  void RspMovevData (enum bitfields fieldid)
{
   struct bitfield_data *pBF;
   INT                  movelen;

   /*---------------------------------------------------*/
   /* find pointer to bitfield info for this bitfield...*/
   /*---------------------------------------------------*/
   pBF = Find_Bitfield_Table(fieldid);

   if (pBF != NULL)
   {
      movelen = ReqRsp_Buffer [reqidx];
      pBF->p_bd_pdata[0] = ReqRsp_Buffer [reqidx];
      reqidx++;

      memcpy(&pBF->p_bd_pdata[1], &ReqRsp_Buffer[reqidx], movelen);

      reqidx = reqidx + movelen;   /* increment buffer pointer */
   }
} /* end RspMovevData */

/*****************************************************************************

  Function:    Move_IfThere

  Description: This function will determine if the data for fieldid is there.
  Author: 
      unknown
  Inputs:
      fieldid    - the field id of the data to be moved
      p_trantype - a pointer to tran type
  Outputs:
      None
  Return values:
      true  - field is there 
      false - field not there
  Modified by:
******************************************************************************/
static  INT Move_IfThere(enum bitfields fieldid, BYTE p_trantype)
{
   struct bitfield_data *pBF;
        INT i;

   /*---------------------------------------------------*/
   /* find pointer to bitfield info for this bitfield...*/
   /*---------------------------------------------------*/
   pBF = Find_Bitfield_Table(fieldid);

   /*---------------------------------------------------*/
   /* in any byte non-zero, it's there, return true     */
   /*---------------------------------------------------*/
   if (pBF != NULL)
   {
      for (i=0; i < pBF->bd_fieldlen; i++)
         {
         if ( (pBF->p_bd_pdata [i] != 0) && (pBF->p_bd_pdata [i] != ' ') ) 
            return(true);
         }
   }
   return(false);
} /* end Move_IfThere */


/*****************************************************************************

  Function:    CondPan   

  Description: This function will move the account number if it is there.
  Author: 
      unknown
  Inputs:
      bitfield   - the bit field of the account number 
      p_trantype - pointer to the tran type
  Outputs:
      None
  Return values:
      true  - account number moved
      false - account number not moved
  Modified by:
      BGable      6/30/98
******************************************************************************/
static  INT CondPan(enum bitfields bitfield, BYTE p_trantype)
{
/* JH Apr 29  changed procedure to only send the pan if it is there */
   if (HOST_MESSAGE.pan_len [0] != 0x00)   /* JH Apr 29  Fixed bug */
       return(Move_IfThere(bitfield, p_trantype));
  
return(false);
} /* end CondPan */

/*****************************************************************************

  Function:    CondExpd

  Description: This function will move the expiration dat if it is there.
  Author: 
      unknown
  Inputs:
      bitfield   - the bit field of the account number 
      p_trantype - pointer to the tran type
  Outputs:
      None
  Return values:
      true  - expiration date moved
      false - expiration date not moved
  Modified by:
      BGable      6/30/98
******************************************************************************/
static  INT CondExpd(enum bitfields bitfield, BYTE p_trantype)
{
        /* if track2 data here, no need for expdate field */
   if (HOST_MESSAGE.date_expd [1] != 0x00)    /* JH Apr 29  Fixed bug */
       return(true);
  
return(false);
} /* end CondExpd */

/*****************************************************************************

  Function:    CondProc

  Description: This function will determine whether the message type is not 
               a reversal. 
  Author: 
      unknown
  Inputs:
      bitfield - the bit field of the account number 
      trantype - tran type
  Outputs:
      None
  Return values:
      true  - not reversal
      false - reversal
  Modified by:
      BGable      6/30/98
******************************************************************************/
static  INT CondProc(enum bitfields bitfield, BYTE trantype)
{
   if (HOST_MESSAGE.msgtype [0] != 0x04)
      return (true);

   return(false);
} /* end CondProc */

/*****************************************************************************

  Function:    CondTrack1

  Description: This function will determine whether or not there is a track1. 
  Author: 
      unknown
  Inputs:
      bitfield   - the bit field of track1
      p_trantype - a pointer to tran type
  Outputs:
      None
  Return values:
      true  - track1 
      false - no track1
  Return values:
      None
  Modified by:
******************************************************************************/
static  INT CondTrack1 (enum bitfields bitfield, BYTE p_trantype)
{
   if (HOST_MESSAGE.track1_len [0] != 0x00)   /* JH Apr 29  Fixed bug */
       return(true);

return(false);
} /* end CondTrack1 */


/*****************************************************************************

  Function:    vMove_IfThere 
  Description: This function will determine if the data for fieldid is there for
               fields with variable lengths.
  Author: 
      unknown
  Inputs:
      fieldid    - the field id of the data to be moved
      p_trantype - a pointer to tran type
  Outputs:
      None
  Return values:
      true  - field is there 
      false - field not there
  Modified by:
******************************************************************************/
static  INT vMove_IfThere(enum bitfields fieldid, BYTE p_trantype)
{
   struct bitfield_data *pBF;

   /*---------------------------------------------------*/
   /* find pointer to bitfield info for this bitfield...*/
   /*---------------------------------------------------*/
   pBF = Find_Bitfield_Table(fieldid);

   /* if length byte is non-zero */
   if (pBF != NULL)
   {
      if (pBF->p_bd_pdata [0] != 0)
         return(true);
   }
   return(false);
} /* end Move_IfThere */

/*****************************************************************************

  Function:    Move_Always

  Description: This function will cause data to be move always by always 
               returning true.
  Author: 
      unknown
  Inputs:
      fieldid    - the field id of the data to be moved
      p_trantype - a pointer to tran type
  Outputs:
      None
  Return values:
      true  - field should be moved
  Modified by:
******************************************************************************/
static  INT Move_Always(enum bitfields bitfield, BYTE p_trantype)
{
   return(true);
} /* end Move_Always */



/*****************************************************************************

  Function:    Move_Never

  Description: This function will cause data to never be moved by always 
               returning false.
  Author: 
      unknown
  Inputs:
      fieldid    - the field id of the data to be moved
      p_trantype - a pointer to tran type
  Outputs:
      None
  Return values:
      false  - field should not be moved
  Modified by:
******************************************************************************/
static  INT Move_Never(enum bitfields bitfield, BYTE p_trantype)
{
   return(false);
} /* end Move_Never */


typedef char * pCHAR;
/*EVM data Ascii Length */
#define APP_CRYT_DATA_ASCII_LEN 16
#define UNPRE_NUM_DATA_ASCII_LEN 8
#define ATC_DATA_ASCII_LEN  4
#define TVR_DATA_ASCII_LEN 10
#define TAR_DATE_DATA_ASCII_LEN 6
#define TAR_TYPE_DATA_ASCII_LEN 2
#define AMT_AUTH_DATA_ASCII_LEN 12
#define TRA_CUR_CODE_DATA_ASCII_LEN 4
#define TER_CUR_CODE_DATA_ASCII_LEN 4
#define AIP_DATA_ASCII_LEN 4
#define AMT_OTH_DATA_ASCII_LEN 12
#define APP_PAN_SEQ_NUM_DATA_ASCII_LEN 2
#define CRYPTO_INFO_DATA_ASCII_LEN 2
#define AMEX_EMV_MAX_LEN 255
#define ISSUERAPPID      32


/*****************************************************************************

  Function:   genutil_bin_to_ascii

  Description:
      Convert binary string to ascii string
  Author: 
      JHapitas   5/6/98
  Inputs:
      bstring  - incoming binary string
      blen     - length of bstring
      astr_len - length of astr
  Outputs:
      astr     - pointer to the outgoing ascii string
  Return values:
  Modified by:
      BGable      6/26/98
******************************************************************************/
/*****************************************
   convert binary string to ascii string     JH May 06  New Procedure
*****************************************/
extern void genutil_bin_to_ascii (BYTE bstring[], INT blen, pCHAR astr, INT astr_len)
{
   INT  i = 0;

   memset (astr, 0x00, astr_len);

   for (i = 0; i < (blen * 2); i+=2)
   {
      if ((bstring[i/2] / 16) <= 9) 
        astr[i] = (bstring[i/2] / 16) + '0';
      else
        astr[i] = (bstring[i/2] / 16 - 0x0a)+'A';

      if ((bstring[i/2] & 0x0f) <= 9)
        astr[i+1] = (bstring[i/2] & 0x0F) + '0'; 
      else
        astr[i+1] = (bstring[i/2] & 0x0F) - 0x0a + 'A';    
   }
} /* genutil_bin_to_ascii */


/*****************************************************************************

  Function:    generate_AMEX_EMV_data

  Description: This function will Store the evn raw data in EMV_details.emv_block
  			and ICC System Related Data in to related field in p_auth_tx->EMV_details in 
  			ASCII formate.
  Author:
     	TF - Abhishek
  Inputs:
     pAUTH_TX p_auth_tx : Pointer to AUTH_TX structure.
  Outputs:
      None
  Return values:
	None
  Modified by:
******************************************************************************/

void generate_AMEX_EMV_data()
{
	
	CHAR	IccData_Length[3]=""; 
	CHAR	IccRawData[256]=""; 
	INT		A2E_count =0;
	CHAR 	IccData_len_ebcdoc_formate[4]="";
	INT 	IccRawDataLen=0;
	CHAR 	IccAsciiformatedata[512]="";
	CHAR 	iadlen[3]="";
	INT 	iadlen_data_len = 0;
	INT 	iadlen_data_ascii_len = 0;
	INT 	ICCfeilddataIndex = 0;
	memset(&EMV_details,0x00,sizeof(EMV_details));
	
	/******************************TF Abhishek*************************************/
		/*FEILD 55 EMV data*/
		/*A:Acquirer
			C: Chip Card
			T: Terminal
			F: Fixed
			V:Variable
		 Maximum Length Fixed/variable		Source		Feild Name
			4				F			A			Header Version Name "AGNS"
			2				F			A			Header Version Number  "0001"
			8				F			C			Application Cryptogram
			33				V			C			Issuer Application Data (IAD)
			4				F			T			Unpredictable Number
			2				F			C			Application Transaction Counter (ATC)
			5				F			T			Terminal Verification Results (TVR)
			3				F			T			Transaction Date
			1				F			T			Transaction Type
			6				F			T			Amount, Authorized
			2				F			T			Transaction Currency Code
			2				F			T			Terminal Country Code
			2				F			C			Application Interchange Profile (AIP)
			6				F			T			Amount, Other
			1				F			C			Application PAN Sequence Number
			1				F			C			Cryptogram Information Data
			
	***********************************************************************************/
	//memset(&p_auth_tx,0,sizeof(AUTH_TX));

	/* Copy IccData_len of 3 byte into local buffer */
	memcpy (IccData_Length,HOST_MESSAGE.IccData_len,3);

	/* Conver IccData_len into EBCDIC formate */
	memcpy(IccData_len_ebcdoc_formate,IccData_Length,3);

	for ( A2E_count = 0; A2E_count <3; A2E_count++)
		{
			IccData_len_ebcdoc_formate[A2E_count]=ascii_ebcdic_conv(IccData_len_ebcdoc_formate[A2E_count],A2E);
		}

	/*1st 3 bytes of emv_block in EVM_detalis structure is used to hold IccData_len_ebcdoc_formate 
	Store IccData_len to in to EVM_detalis.emv_block*/
	memcpy(EMV_details.emv_block,IccData_len_ebcdoc_formate,3);

	/* Copy Raw ICC data in to local buffer*/
	IccRawDataLen=atoi(IccData_Length);

	memcpy(IccRawData,HOST_MESSAGE.IccData,IccRawDataLen);

	/* Copy ICC Header Version Name & ICC Header Version Name in to EVM_detalis.future_field1*/
	memcpy(EMV_details.future_field1,IccRawData,6);

	/* Store Raw ICC data in to EVM_detalis.emv_block field*/
	memcpy(EMV_details.emv_block+3,IccRawData,IccRawDataLen);

	/* Convert Bin data to Ascii data*/
	genutil_bin_to_ascii((unsigned char*)IccRawData+6,IccRawDataLen -6,IccAsciiformatedata,(2*IccRawDataLen));
	

	/*Store Application Cryptogram in to EVM_detalis.app_crypto field*/
	memcpy(EMV_details.app_crypto,IccAsciiformatedata + ICCfeilddataIndex,APP_CRYT_DATA_ASCII_LEN);

	/* Move pointer to point Issuer Application Data (IAD)  in buffer */
	memcpy(iadlen,IccAsciiformatedata+ APP_CRYT_DATA_ASCII_LEN,2);/*Copy length of IAD*/
	iadlen_data_len = atoi(iadlen);
	iadlen_data_ascii_len = 2 *iadlen_data_len;
	ICCfeilddataIndex = APP_CRYT_DATA_ASCII_LEN +2;
	
	/*Store Issuer Application Data (IAD) in to EVM_detalis.iad field*/
	memcpy(EMV_details.iad,IccAsciiformatedata + ICCfeilddataIndex,iadlen_data_ascii_len);	

	/* Move pointer to point Unpredictable Number  in buffer */
	ICCfeilddataIndex = ICCfeilddataIndex + iadlen_data_ascii_len ;
	
	/*Store Unpredictable Number in to EVM_detalis.unpredictable_number field*/
	memcpy(EMV_details.unpredictable_number,IccAsciiformatedata + ICCfeilddataIndex,UNPRE_NUM_DATA_ASCII_LEN);

	/* Move pointer to point  Application Transaction Counter (ATC) in buffer */
	ICCfeilddataIndex = ICCfeilddataIndex + UNPRE_NUM_DATA_ASCII_LEN;
	
	/*Store  Application Transaction Counter (ATC) in to EVM_detalis.atc field*/
	memcpy(EMV_details.atc,IccAsciiformatedata + ICCfeilddataIndex,ATC_DATA_ASCII_LEN);

	/* Move pointer to point Terminal Verification Results (TVR) in buffer */
	ICCfeilddataIndex = ICCfeilddataIndex + ATC_DATA_ASCII_LEN;
	
	/*Store Terminal Verification Results (TVR)  in to EVM_detalis.tvr field*/
	memcpy(EMV_details.tvr,IccAsciiformatedata + ICCfeilddataIndex,TVR_DATA_ASCII_LEN);

	/* Move pointer to point Transaction Date in buffer */
	ICCfeilddataIndex = ICCfeilddataIndex  + TVR_DATA_ASCII_LEN;
	
	/*Store Transaction Date in to EVM_detalis.tran_date field*/
	memcpy(EMV_details.tran_date,IccAsciiformatedata + ICCfeilddataIndex,TAR_DATE_DATA_ASCII_LEN);

	/* Move pointer to point Transaction Type in buffer */
	ICCfeilddataIndex = ICCfeilddataIndex + TAR_DATE_DATA_ASCII_LEN;
	
	/*Store Transaction Type in to EVM_detalis.tran_type field*/
	memcpy(EMV_details.tran_type,IccAsciiformatedata + ICCfeilddataIndex,TAR_TYPE_DATA_ASCII_LEN);

	/* Move pointer to point Amount, Authorized in buffer */
	ICCfeilddataIndex = ICCfeilddataIndex + TAR_TYPE_DATA_ASCII_LEN;
	
	/*Store Amount, Authorized in to EVM_detalis.tran_amount field*/
	memcpy(EMV_details.tran_amount,IccAsciiformatedata + ICCfeilddataIndex,AMT_AUTH_DATA_ASCII_LEN);

	/* Move pointer to point Transaction Currency Code in buffer */
	ICCfeilddataIndex = ICCfeilddataIndex + AMT_AUTH_DATA_ASCII_LEN;
	
	/*Store Transaction Currency Code in to EVM_detalis.currency_code field*/
	memcpy(EMV_details.currency_code,IccAsciiformatedata + ICCfeilddataIndex,TRA_CUR_CODE_DATA_ASCII_LEN);

	/* Move pointer to point Terminal Country Code in buffer */
	ICCfeilddataIndex = ICCfeilddataIndex + TRA_CUR_CODE_DATA_ASCII_LEN;
	
	/*Store Transaction Currency Code in to EVM_detalis.country_code field*/
	memcpy(EMV_details.country_code,IccAsciiformatedata + ICCfeilddataIndex,TER_CUR_CODE_DATA_ASCII_LEN);

	/* Move pointer to point Application Interchange Profile (AIP) in buffer */
	ICCfeilddataIndex = ICCfeilddataIndex + TER_CUR_CODE_DATA_ASCII_LEN;
	
	/*Store Application Interchange Profile (AIP) in to EVM_detalis.interchange_profile field*/
	memcpy(EMV_details.interchange_profile,IccAsciiformatedata + ICCfeilddataIndex,AIP_DATA_ASCII_LEN);

	/* Move pointer to point Amount, Other in buffer */
	ICCfeilddataIndex = ICCfeilddataIndex + AIP_DATA_ASCII_LEN;
	
	/*Store Amount, Other in to EVM_detalis.other_amount field*/
	memcpy(EMV_details.other_amount,IccAsciiformatedata + ICCfeilddataIndex,AMT_OTH_DATA_ASCII_LEN);

	/* Move pointer to point Application PAN Sequence Number in buffer */
	ICCfeilddataIndex = ICCfeilddataIndex + AMT_OTH_DATA_ASCII_LEN;
	
	/*Store Application PAN Sequence Number in to EVM_detalis.pan_sequence_number field*/
	memcpy(EMV_details.pan_sequence_number,IccAsciiformatedata + ICCfeilddataIndex,APP_PAN_SEQ_NUM_DATA_ASCII_LEN);

	/* Move pointer to point Cryptogram Information Data in buffer */
	ICCfeilddataIndex = ICCfeilddataIndex + APP_PAN_SEQ_NUM_DATA_ASCII_LEN;
	
	/*Store Cryptogram Information Data in to EVM_detalis.crypto_info field*/
	memcpy(EMV_details.crypto_info,IccAsciiformatedata + ICCfeilddataIndex,CRYPTO_INFO_DATA_ASCII_LEN);

/*****************************************************************************/

}



/*********ParsedMssg() to print the amex parsed message****/
void parsed_mssg()
{

	int bitCount, startCharacter, icnt, LastDisplayBit,len=0;
	char cardAccBuffer[150] = {0};

	memset(Line,0,sizeof(Line));
	


	strcpy (Line[0], "000 MsgType				: ");
	strncat(Line[0],(char *)HOST_MESSAGE.msgtype,4);

	if(strlen((char *)HOST_MESSAGE.bitmap1_bin) != 0 && strlen((char *)HOST_MESSAGE.bitmap2_bin) != 0 )
	{
		strcpy (Line[0], "000 MsgType				: ");
		for (icnt = 0; icnt < 8; icnt++)
		{
			sprintf(&Line[1][31 + 3*icnt],"%x%x ", HOST_MESSAGE.bitmap1_bin[icnt]>>4,
						((BYTE)(HOST_MESSAGE.bitmap1_bin[icnt]<<4))>>4);
		}

		for (icnt = 0; icnt < 8; icnt++)
		{
			sprintf(&Line[1][55 + 3*icnt],"%x%x ", HOST_MESSAGE.bitmap2_bin[icnt]>>4,
							((BYTE)(HOST_MESSAGE.bitmap2_bin[icnt]<<4))>>4);
		}
	}

	startCharacter = 1;
	LastDisplayBit = 1;
	char temp_len[4]={0};// This is the bit that was displayed last.

	strcat (Line[1], "BIT MAP					: ");
	strncat (Line[1],astr_bitmap,strlen(astr_bitmap));

	for (bitCount = 2; bitCount <= 192; bitCount++)
	{
		memset((char *)temp_len,0,3);
		switch(bitCount)
		{	
		case bf_2pan :
			//if(strlen((char *)HOST_MESSAGE.pan_len) != 0)                     // card number  len
			if(BitFieldArray[bf_2pan] == 1)
			{	
				//char temp_len[2]={0};
				strncpy(temp_len,(char *)HOST_MESSAGE.pan_len,2); 
				len =atoi(temp_len);
				strcat (Line[bitCount], FieldStrings [bitCount]);
				strncat (Line[bitCount], (char *)HOST_MESSAGE.pan,len);				
			}	
		break;
		
		case bf_3proc :
			//if(strlen((char *)HOST_MESSAGE.proccode) != 0)
			if(BitFieldArray[bf_3proc] == 1)
			{
				strcat (Line[bitCount], FieldStrings [bitCount]);
				strncat (Line[bitCount], (char *)HOST_MESSAGE.proccode, 6);				
			}
		break;
		
		case bf_4amount :
			//if(strlen((char *)HOST_MESSAGE.amount) != 0)
			if(BitFieldArray[bf_4amount] == 1)
			{
				strcat (Line[bitCount], FieldStrings [bitCount]);
				strncat (Line[bitCount], (char *)HOST_MESSAGE.amount, 12);
			}
		break;
		
		case bf_5recon_amt :
			//if(strlen((char *)HOST_MESSAGE.setl_amount) != 0)
			if( BitFieldArray[bf_5recon_amt] == 1)
			{
				strcat (Line[bitCount], FieldStrings [bitCount]);
				strncat (Line[bitCount], (char *)HOST_MESSAGE.setl_amount ,12);
			}
		break;
		
		case bf_7trntime :
			//if(strlen((char *)HOST_MESSAGE.gmt_time) != 0)
			if(BitFieldArray[bf_7trntime] == 1)
			{
				strcat (Line[bitCount], FieldStrings [bitCount]);
				strncat (Line[bitCount], (char *)HOST_MESSAGE.gmt_time ,10);
			}
		break;
		
		case bf_9settconvrate :
			//if(strlen((char *)HOST_MESSAGE.setl_conv_rate) != 0)
			if(BitFieldArray[bf_9settconvrate] == 1)
			{
				strcat (Line[bitCount], FieldStrings [bitCount]);
				strncat (Line[bitCount], (char *)HOST_MESSAGE.setl_conv_rate ,8);
			}
		break;
		
		case bf_11stan :
			//if(strlen((char *)HOST_MESSAGE.stan) != 0)
			if(BitFieldArray[bf_11stan] == 1)
			{
				strcat (Line[bitCount], FieldStrings [bitCount]);
				strncat (Line[bitCount], (char *)HOST_MESSAGE.stan, 6);
			}
		break;
		
		case bf_12datetime :
			//if(strlen((char *)HOST_MESSAGE.datetime) != 0)
			if(BitFieldArray[bf_12datetime] == 1)
			{
				strcat (Line[bitCount], FieldStrings [bitCount]);
				strncat (Line[bitCount], (char *)HOST_MESSAGE.datetime, 12);
			}
		break ;
		case bf_13date :
			//if(strlen((char *)HOST_MESSAGE.eff_date) != 0)
			if(BitFieldArray[bf_13date] == 1)
			{
				strcat (Line[bitCount], FieldStrings [bitCount]);
				strncat (Line[bitCount], (char *)HOST_MESSAGE.eff_date, 4);
			}
		break ;
		
		case bf_14expiry :
			//if(strlen((char *)HOST_MESSAGE.date_expd) != 0)
			if(BitFieldArray[bf_14expiry] == 1)
			{
				strcat (Line[bitCount], FieldStrings [bitCount]);
				strncat (Line[bitCount], (char *)HOST_MESSAGE.date_expd, 4);
			}
		break ;
		
		case bf_15datestl :
			//if(strlen((char *)HOST_MESSAGE.date_settle) != 0)
			if(BitFieldArray[bf_15datestl] == 1)
			{
				strcat (Line[bitCount], FieldStrings [bitCount]);
				strncat (Line[bitCount], (char *)HOST_MESSAGE.date_settle, 4);
			}
		break ;
		
		case bf_16dateconv :
			//if(strlen((char *)HOST_MESSAGE.date_conv) != 0)
			if(BitFieldArray[bf_16dateconv] == 1)
			{
				strcat (Line[bitCount], FieldStrings [bitCount]);
				strncat (Line[bitCount], (char *)HOST_MESSAGE.date_conv, 4);
			}
		break ;
		
		case bf_18mertype :
			//if(strlen((char *)HOST_MESSAGE.merch_type) != 0)
			if(BitFieldArray[bf_18mertype] == 1)
			{
				strcat (Line[bitCount], FieldStrings [bitCount]);
				strncat (Line[bitCount], (char *)HOST_MESSAGE.merch_type, 4);
			}
		break ;
		
		case bf_19ctrycd :
			//if(strlen((char *)HOST_MESSAGE.ctry_code) != 0)
			if(BitFieldArray[bf_19ctrycd] == 1)
			{
				strcat (Line[bitCount], FieldStrings [bitCount]);
				strncat (Line[bitCount], (char *)HOST_MESSAGE.ctry_code, 3);
			}
		break ;
		
		case bf_20countrycode :
			//if(strlen((char *)HOST_MESSAGE.coun_code) != 0)
			if(BitFieldArray[bf_20countrycode] == 1)
			{
				strcat (Line[bitCount], FieldStrings [bitCount]);
				strncat (Line[bitCount], (char *)HOST_MESSAGE.coun_code, 3);
			}
		break ;
		
		case bf_21fwdinstcntcode :
			//if(strlen((char *)HOST_MESSAGE.fwd_inst_cnt_code) != 0)
			if(BitFieldArray[bf_21fwdinstcntcode] == 1)
			{
				strcat (Line[bitCount], FieldStrings [bitCount]);
				strncat (Line[bitCount], (char *)HOST_MESSAGE.fwd_inst_cnt_code, 3);
			}
		break ;
		
		case bf_22posentry :
			//if(strlen((char *)HOST_MESSAGE.posentry) != 0)
			if(BitFieldArray[bf_22posentry] == 1)
			{
				strcat (Line[bitCount], FieldStrings [bitCount]);
				strncat (Line[bitCount], (char *)HOST_MESSAGE.posentry, 12);
			}
		break ;

		case bf_23csn :
			//if(strlen((char *)HOST_MESSAGE.card_seq_num) != 0)
			if(BitFieldArray[bf_23csn] == 1)
			{
				strcat (Line[bitCount], FieldStrings [bitCount]);
				strncat (Line[bitCount], (char *)HOST_MESSAGE.card_seq_num, 3);
			}
		break ;
		
		case bf_24funccd :
			//if(strlen((char *)HOST_MESSAGE.func_cd) != 0)
			if(BitFieldArray[bf_24funccd] == 1)
			{
				strcat (Line[bitCount], FieldStrings [bitCount]);
				strncat (Line[bitCount], (char *)HOST_MESSAGE.func_cd, 3);
			}
		break ;
		
		case bf_25poscc :
			//if(strlen((char *)HOST_MESSAGE.pos_condcd) != 0)
			if(BitFieldArray[bf_25poscc] == 1)
			{
				strcat (Line[bitCount], FieldStrings [bitCount]);
				strncat (Line[bitCount], (char *)HOST_MESSAGE.pos_condcd, 4);
			}
		break ;
		
		case bf_26accptcd :
			//if(strlen((char *)HOST_MESSAGE.accpt_bus_cd) != 0)
			if(BitFieldArray[bf_26accptcd] == 1)
			{
					strcat (Line[bitCount], FieldStrings [bitCount]);
					strncat (Line[bitCount], (char *)HOST_MESSAGE.accpt_bus_cd , 4);
			}
		break ;
		
		case bf_28tranfeeamt :
			//if(strlen((char *)HOST_MESSAGE.tran_fee_amt) != 0)
			if(BitFieldArray[bf_28tranfeeamt] == 1)
			{
					strcat (Line[bitCount], FieldStrings [bitCount]);
					strncat (Line[bitCount], (char *)HOST_MESSAGE.tran_fee_amt , 8);
			}
		break ;
		
		case bf_30origamt :
			//if(strlen((char *)HOST_MESSAGE.orig_amt) != 0)
			if(BitFieldArray[bf_30origamt] == 1)
			{
					strcat (Line[bitCount], FieldStrings [bitCount]);
					strncat (Line[bitCount], (char *)HOST_MESSAGE.orig_amt , 24);
			}
		break ;
		
		case bf_31tranid :
			//if(strlen((char *)HOST_MESSAGE.tranid_len) != 0)
			if(BitFieldArray[bf_31tranid] == 1)
			{
					strncpy(temp_len,(char *)HOST_MESSAGE.tranid_len,2); 
					len =atoi(temp_len);
					strcat (Line[bitCount], FieldStrings [bitCount]);
					strncat (Line[bitCount], (char *)HOST_MESSAGE.tranid ,len);
			}
		break ;
		
		case bf_32idcode :
			//if(strlen((char *)HOST_MESSAGE.acq_inst_len) != 0)
			if(BitFieldArray[bf_32idcode] == 1)
			{
					strncpy(temp_len,(char *)HOST_MESSAGE.acq_inst_len,2); 
					len = atoi(temp_len);
					strcat (Line[bitCount], FieldStrings [bitCount]);
					strncat (Line[bitCount], (char *)HOST_MESSAGE.acq_inst_id ,len);
			}
		break ;
		
		case bf_33fwdinstidcode :
			//if(strlen((char *)HOST_MESSAGE.fwd_inst_len) != 0)
			if(BitFieldArray[bf_33fwdinstidcode] == 1)
			{
					strncpy(temp_len,(char *)HOST_MESSAGE.fwd_inst_len,2);
					len = atoi(temp_len); 
					strcat (Line[bitCount], FieldStrings [bitCount]);
					strncat (Line[bitCount], (char *)HOST_MESSAGE.fwd_inst_id ,len);
			}
		break ;

		case bf_34panext :
			//if(strlen((char *)HOST_MESSAGE.pan_extd) != 0)
			if(BitFieldArray[bf_34panext] == 1)
			{
					strncpy(temp_len,(char *)HOST_MESSAGE.pan_extd_len,2);
					len = atoi(temp_len); 
					strcat (Line[bitCount], FieldStrings [bitCount]);
					strncat (Line[bitCount], (char *)HOST_MESSAGE.pan_extd ,len);
			}
		break ;

		case bf_35track2 :
			//if(strlen((char *)HOST_MESSAGE.track2_len) != 0)
			if(BitFieldArray[bf_35track2] == 1)
			{
					strncpy(temp_len,(char *)HOST_MESSAGE.track2_len,2);
					len = atoi(temp_len);
					strcat (Line[bitCount], FieldStrings [bitCount]);
					strncat (Line[bitCount], (char *)HOST_MESSAGE.track2 ,len);
			}
		break ;
		
		case bf_37rrn :
			//if(strlen((char *)HOST_MESSAGE.rrn) != 0)
			if(BitFieldArray[bf_37rrn] == 1)
			{
					strcat (Line[bitCount], FieldStrings [bitCount]);
					strncat (Line[bitCount], (char *)HOST_MESSAGE.rrn ,12);
			}
		break ;
		
		case bf_38authcode :
			//if(strlen((char *)HOST_MESSAGE.authcode) != 0)
			if(BitFieldArray[bf_38authcode] == 1)
			{
					strcat (Line[bitCount], FieldStrings [bitCount]);
					strncat (Line[bitCount], (char *)HOST_MESSAGE.authcode ,6);
			}
		break ;
		
		case bf_39respcode :
			//if(strlen((char *)HOST_MESSAGE.responsecd) != 0)
			if(BitFieldArray[bf_39respcode] == 1)
			{
					strcat (Line[bitCount], FieldStrings [bitCount]);
					strncat (Line[bitCount], (char *)HOST_MESSAGE.responsecd,3);
			}
		break ;
		
		case bf_41termid :
			//if(strlen((char *)HOST_MESSAGE.termid) != 0)
			if(BitFieldArray[bf_41termid] == 1)
			{
					strcat (Line[bitCount], FieldStrings [bitCount]);
					strncat (Line[bitCount], (char *)HOST_MESSAGE.termid,8);
			}
		break ;
		
		case bf_42cardacc :
			//if(strlen((char *)HOST_MESSAGE.cardaccid) != 0)
			if(BitFieldArray[bf_42cardacc] == 1)
			{
					strcat (Line[bitCount], FieldStrings [bitCount]);
					strncat (Line[bitCount], (char *)HOST_MESSAGE.cardaccid,15);
			}
		break ;
		
		case bf_43cardacceptloc :
			//if(strlen((char *)HOST_MESSAGE.cardaccnamelen) != 0)
			if(BitFieldArray[bf_43cardacceptloc] == 1)
			{
					strncpy(temp_len,(char *)HOST_MESSAGE.cardaccnamelen,2);
					len = atoi(temp_len);
					strcat (Line[bitCount], FieldStrings [bitCount]);
					for(int i=0; i<len; i++)
					{
						if((char)HOST_MESSAGE.cardaccname[i] != 0x00)
							cardAccBuffer[i] = (char)HOST_MESSAGE.cardaccname[i];
						else
							cardAccBuffer[i] = ' ';
					}
					strncat(Line[bitCount], cardAccBuffer, len);
			}
		break ;
		
		case bf_44respdata :
			//if(strlen((char *)HOST_MESSAGE.addl_rsp_len) != 0)			 //addl_rsp_len	
			if(BitFieldArray[bf_44respdata] == 1)
			{
					strncpy(temp_len,(char *)HOST_MESSAGE.addl_rsp_len,2);
					len = atoi(temp_len);
					strcat (Line[bitCount], FieldStrings [bitCount]);
					strncat (Line[bitCount], (char *)HOST_MESSAGE.addlrspdata,len);
			}
		break ;
		
		case bf_45track1 :
			//if(strlen((char *)HOST_MESSAGE.track1_len) != 0)
			if(BitFieldArray[bf_45track1] == 1)
			{
					strncpy(temp_len,(char *)HOST_MESSAGE.track1_len,2);
					len = atoi(temp_len);
					strcat (Line[bitCount], FieldStrings [bitCount]);
					strncat (Line[bitCount], (char *)HOST_MESSAGE.track1 ,len);
			}
		break ;

		case bf_48addldata :
			//if(strlen((char *)HOST_MESSAGE.addl_data_len) != 0)
			if(BitFieldArray[bf_48addldata] == 1)
			{
					strncpy(temp_len,(char *)HOST_MESSAGE.addl_data_len,3);
					len = atoi(temp_len);
					strcat (Line[bitCount], FieldStrings [bitCount]);
					strncat (Line[bitCount], (char *)HOST_MESSAGE.addl_data,len);
			}
		break ;
		
		case bf_49trncurcd :
			//if(strlen((char *)HOST_MESSAGE.tran_curr_cd) != 0)			// tran_curr_cd
			if(BitFieldArray[bf_49trncurcd] == 1)
			{
					strcat (Line[bitCount], FieldStrings [bitCount]);
					strncat (Line[bitCount], (char *)HOST_MESSAGE.tran_curr_cd,3);
			}
		break ;
		
		case bf_50recncurcd :
			//if(strlen((char *)HOST_MESSAGE.recn_curr_cd) != 0)
			if(BitFieldArray[bf_50recncurcd] == 1)
			{
					strcat (Line[bitCount], FieldStrings [bitCount]);
					strncat (Line[bitCount], (char *)HOST_MESSAGE.recn_curr_cd,3);
			}
		break ;
		
		case bf_52pinblock :
			//if(strlen((char *)HOST_MESSAGE.pinblock) != 0)           // pinblock
			if(BitFieldArray[bf_52pinblock] == 1)
			{
					strcat (Line[bitCount], FieldStrings [bitCount]);
					strncat (Line[bitCount], (char *)HOST_MESSAGE.pinblock,16);
			}
		break ;
		
		case bf_53seccntl :
			//if(strlen((char *)HOST_MESSAGE.sec_cntl_len) != 0)			// sec_cntl_len
			if(BitFieldArray[bf_53seccntl] == 1)
			{
					strncpy(temp_len,(char *)HOST_MESSAGE.sec_cntl_len,2);
					len = atoi(temp_len);
					strcat (Line[bitCount], FieldStrings [bitCount]);
					strncat (Line[bitCount], (char *)HOST_MESSAGE.sec_cntl,len);
			}
		break ;
		
		case bf_54AddlAmount :
			//if(strlen((char *)HOST_MESSAGE.AddlAmts_len) != 0)			// AddlAmts_len
			if(BitFieldArray[bf_54AddlAmount] == 1)
			{
					strncpy(temp_len,(char *)HOST_MESSAGE.AddlAmts_len,3);
					len = atoi(temp_len);
					strcat (Line[bitCount], FieldStrings [bitCount]);
					strncat (Line[bitCount], (char *)HOST_MESSAGE.AddlAmts,len);
			}
		break ;
		
		case bf_55IccData :
			//if(strlen((char *)HOST_MESSAGE.IccData_len) != 0)			// IccData_len
			if(BitFieldArray[bf_55IccData] == 1)
			{
					strncpy(temp_len,(char *)HOST_MESSAGE.IccData_len,3);
					len = strlen((char *)iccdata);
					strcat (Line[bitCount], FieldStrings [bitCount]);
					strncat (Line[bitCount], (char *)iccdata,len);
			}
		break ;
		
		case bf_56orgdata :
			//if(strlen((char *)HOST_MESSAGE.orig_data_len) != 0)				// orig_data_len
			if(BitFieldArray[bf_56orgdata] == 1)
			{
					strncpy(temp_len,(char *)HOST_MESSAGE.orig_data_len,2);
					len = atoi(temp_len);
					strcat (Line[bitCount], FieldStrings [bitCount]);
					strncat (Line[bitCount], (char *)HOST_MESSAGE.orig_data,len);
			}
		break ;
		
		case bf_60priv60 :
			int i,k;
			char BitMap[10], a[100];
			//if(strlen((char *)HOST_MESSAGE.priv60_len) != 0)				// priv60_len MsgType
			if(BitFieldArray[bf_60priv60] == 1)
			{
					strncpy(temp_len,(char *)HOST_MESSAGE.priv60_len,3);
					len = atoi(temp_len);
					strcat (Line[bitCount], FieldStrings [bitCount]);
					memset( BitMap, 0x00, sizeof(BitMap) );
					memset( a, 0x00, sizeof(a) );
					for( k=0,i=5; k<8; i++ ,k++)
		    			BitMap[k] = HOST_MESSAGE.priv60[i];
 
					strncpy (a, (char *)HOST_MESSAGE.priv60,5);
					strncat (a, BitMap, 8);
					strcat (a, (char *)&HOST_MESSAGE.priv60[9]);
					strncat (Line[bitCount], a, len);
			}
		break ;
		
		case bf_61priv61 :
			//if(strlen((char *)HOST_MESSAGE.priv61_len) != 0)			// priv61_len
			if(BitFieldArray[bf_61priv61] == 1)
			{
					strncpy(temp_len,(char *)HOST_MESSAGE.priv61_len,3);
					len = atoi(temp_len);
					strcat (Line[bitCount], FieldStrings [bitCount]);
					strncat (Line[bitCount], (char *)HOST_MESSAGE.priv61, strlen((char*)HOST_MESSAGE.priv61));
			}
		break ;
		
		case bf_62priv62 :
			//if(strlen((char *)HOST_MESSAGE.priv62_len) != 0)			//priv62_len
			if(BitFieldArray[bf_62priv62] == 1)
			{
					strncpy(temp_len,(char *)HOST_MESSAGE.priv62_len,3);
					len = atoi(temp_len);
					strcat (Line[bitCount], FieldStrings [bitCount]);
					strncat (Line[bitCount], (char *)HOST_MESSAGE.priv62, strlen((char*)HOST_MESSAGE.priv62));
			}
		break ;
		
		case bf_63priv63 :
			//if(strlen((char *)HOST_MESSAGE.priv63_len) != 0)			//priv63_len
			if(BitFieldArray[bf_63priv63] == 1)
			{
					strncpy(temp_len,(char *)HOST_MESSAGE.priv63_len,3);
					len = atoi(temp_len);
					strcat (Line[bitCount], FieldStrings [bitCount]);
					strncat (Line[bitCount], (char *)HOST_MESSAGE.priv63,len);
			}
		break ;
		}
	
		startCharacter++;
		LastDisplayBit = bitCount;
	}
		if(strlen((char *)HOST_MESSAGE.IccData) != 0)
		{
			strcat (Line[bitCount++], "------------------------------------------------------------------------------------------------------------------");
			
		  sprintf (Line[bitCount++], "4	Version_Name		:	AGNS");
		  sprintf (Line[bitCount++], "2	Version_No.		:	0001");   
		  sprintf(Line[bitCount++],  "9F26	AppCryptogram		:	%s",(char*)EMV_details.app_crypto);   
		  sprintf(Line[bitCount++],  "9F10	IssuerAppData		:	%-033s",(char*)EMV_details.iad );  
		  sprintf(Line[bitCount++],  "9F37	UnpredictableNo		:	%s",(char*)EMV_details.unpredictable_number );
		  sprintf(Line[bitCount++],  "9F36	AppTxnCounter		:	%s",(char*)EMV_details.atc );
		  sprintf(Line[bitCount++],  "95	TermVerifResults		:	%s",(char*)EMV_details.tvr);  
		  sprintf(Line[bitCount++],  "9A	TxnDate			:	%s",(char*)EMV_details.tran_date); 
		  sprintf(Line[bitCount++],  "9C	TxnType			:	%s",(char*)EMV_details.tran_type); 
		  sprintf(Line[bitCount++],  "9F02	AmountAuthNum		:	%s",(char*)EMV_details.tran_amount);   
		  sprintf(Line[bitCount++],  "5F2A	TxnCurrencyCode		:	%s",(char*)EMV_details.currency_code);      
		  sprintf(Line[bitCount++],  "9F1A	TerminalCountryCode	:	%s",(char*)EMV_details.country_code); 
		  sprintf(Line[bitCount++],  "82	AppInterchProfile		:	%s",(char*)EMV_details.interchange_profile);
		  sprintf(Line[bitCount++],  "9F03	AmountOtherNum		:	%s",(char*)EMV_details.other_amount); 
		  sprintf(Line[bitCount++],  "5F34	PANSequenceNo		:	%s",(char*)EMV_details.pan_sequence_number);
		  sprintf(Line[bitCount++],  "9F27	CryptogramInfoData	:	%s",(char*)EMV_details.crypto_info);  
		}
}

/*****************************************************************************

  Function:    Field62Response

  Description: This function will decode field 62 data and put it in HOST_MESSAGE
  Author: 
      unknown
  Inputs:
      fieldid - the index into the bitfield data table
      ReqRsp_Buffer (global) - the buffer that contains the ISO formatted message
  Outputs:
      HOST_MESSAGE (global)  - structure that contains data to go into the 
                               ISO message
  Return values:
      None
  Modified by:
******************************************************************************/
static  void Field62Response(enum bitfields fieldid)
{
   struct bitfield_data *pBF;
   INT             i;
   INT             Len;
   INT             TotalLen;
   BYTE   BitMap[8];
   BYTE   TempStr[20];

   /*+-------------------------------+
     | Determine length of field 62: |
     |    Length:    1 byte          |
     |    Bit map:   8 bytes         |
     |    Field 1:   1 byte   EBCDIC |
     |    Field 2:   8 bytes  BCD    |
     |    Field 3:   4 bytes  EBCDIC |
     +-------------------------------+*/

   pBF      = Find_Bitfield_Table(fieldid);
   if (pBF != NULL)
   {
      TotalLen = ReqRsp_Buffer[reqidx] + pBF->bd_fieldlen;
      if ( TotalLen > 0 )
      {
         for( i=1; i<9; i++ )
            BitMap[i-1] = ReqRsp_Buffer[reqidx+i];

         memcpy( &pBF->p_bd_pdata[0], &ReqRsp_Buffer[reqidx], 9 );
      }

      /* Only the first 3 fields are use in AMEX. */
      Len = 9;
      memset( TempStr, 0x00, sizeof( TempStr ) );
      if ( BitMap[0] & 0x80 )   /* Field 1 */
      {
         pBF->p_bd_pdata[Len] = ascii_ebcdic_conv( ReqRsp_Buffer[reqidx+Len], E2A );
         Len++;
      }

      if ( BitMap[0] & 0x40 )   /* Field 2 */
      {
         bcd_to_asc(&ReqRsp_Buffer[reqidx+Len], 8, (char *)&pBF->p_bd_pdata[Len], 16 );
         Len += 8;
      }

      if ( BitMap[0] & 0x20 )   /* Field 3 */
      {
         memcpy( TempStr, &ReqRsp_Buffer[reqidx+Len], 4 );
         for( i=0; i<4; i++ )
            TempStr[i] = ascii_ebcdic_conv( TempStr[i], E2A );
         Len += 8;
         memcpy( &pBF->p_bd_pdata[Len], TempStr, 4 );
      }
      reqidx += TotalLen;
   }
}

/*****************************************************************************

  Function:    hhutil_parse_host_response_msg

  Description: This function will parse a host response message
  Author: 
      unknown
  Inputs:
      p_auth_tx - a pointer to the auth_tx structure
      ReqRsp_Buffer (global) - the buffer that contains the ISO formatted message
  Outputs:
      HOST_MESSAGE (global)  - structure that contains data to go into the 
                               ISO message
  Return values:
      true  - no error occurred
      false - an error occurred
  Modified by:
      FArad      8/13/98
******************************************************************************/

INT hhutil_parse_host_response_msg()
{
   struct bitfield_data *pBF;
   INT                  bfidx;    /* index to bitfields for this transaction */
   INT                  i;

   Field63_present = 0;
   Field62_present = 0;
   memset(&HOST_MESSAGE, 0x00, sizeof(HOST_MESSAGE));

   /* Extract the message type from the host message. */
   for( i=0; i<4; i++ )
   {
      HOST_MESSAGE.msgtype[i] = ascii_ebcdic_conv (ReqRsp_Buffer[i],E2A);
   }

   /* Ignore Responses to Reversals. */
   //if ( 0 == strncmp( (char*)HOST_MESSAGE.msgtype, "04", 2 ) )
     // return( true );

   bitmapidx = 4;       /* point to bitmap */

   /* Point past bitmap, but 1st check for extended bitmap. */
   if ( ReqRsp_Buffer[bitmapidx]  &  BITTAB[bf_1xbitfield].bit_mask )
   {
      reqidx = 20;                         /* Extended     */
      BitFieldSize = EXTENDED_BITFIELD;
      memcpy(HOST_MESSAGE.bitmap1_bin, &ReqRsp_Buffer[bitmapidx], 8);
      memcpy(HOST_MESSAGE.bitmap2_bin, &ReqRsp_Buffer[bitmapidx + 8], 8);
	  genutil_bin_to_ascii ((BYTE *)&ReqRsp_Buffer[4], EXTENDED_BITMAP_SIZE, astr_bitmap, sizeof(astr_bitmap));
   }
   else
   {
      reqidx = 12;                        /* Not Extended */
      BitFieldSize = NORMAL_BITFIELD;
      memcpy(HOST_MESSAGE.bitmap1_bin, &ReqRsp_Buffer[bitmapidx], 8);
	  genutil_bin_to_ascii ((BYTE *)&ReqRsp_Buffer[4], NORMAL_BITMAP_SIZE, astr_bitmap, sizeof(astr_bitmap));
   }


   /*------------------------------------------------------------------
      For every bitfield present in message ( except first bit ) . . .
   -------------------------------------------------------------------*/
   for (bfidx = 2; bfidx <= BitFieldSize; bfidx++)
   {
       if (ReqRsp_Buffer[bitmapidx+BITTAB[bfidx].byte_no] & BITTAB[bfidx].bit_mask)
      {
         /* Bit is on, field is there.... */

         if (bfidx == 63) Field63_present=1;
         pBF = Find_Bitfield_Table((enum bitfields)bfidx); 
		  if (pBF != NULL)       
         {
			 BitFieldArray[pBF->bd_fieldid] = 1;
            /*---------------------------------------*/
            /* call the Response Handling routine... */
            /*---------------------------------------*/
	   
            (pBF->bd_BFRsp)(pBF->bd_fieldid);
         }
         else
         {
            return(false);
         }
      }
   }
   
   return(true);
} /* hhutil_parse_host_response_msg */


/***********************************************************************************************************/

void Cex_amex_parserDlg::OnBnClickedParse()
{
	// TODO: Add your control notification handler code here
	CString sz = "";
	CString temp;
	BYTE buf[10048];
	LPBYTE temp_string;
	LPBYTE temp_string2;
	int i,j = 0;
	int len = 0;
	UpdateData(); //transafers the data from controls to variables.

	temp = m_flowfile;
	if(strlen(temp) == 0)
	{
		m_parsedmssg.Empty();
		exportbtn.EnableWindow(FALSE);
	}
	else
	{
		exportbtn.EnableWindow(TRUE);
		len = m_flowfile.GetLength();
		temp_string = (LPBYTE)m_flowfile.GetBuffer(len);
		ascii_to_bin(temp_string, ReqRsp_Buffer, len);
		m_flowfile.ReleaseBuffer(len);

		if ( hhutil_parse_host_response_msg() == 0)
		{
			m_parsedmssg = "Cannot Parse the message";
			UpdateData(FALSE); //transfer the data from variables back to control.
		}
		else
		{
			
			parsed_mssg();
			m_parsedmssg = "";
			for(int j=0;j<256; j++)
			{
				if(strlen(Line[j]) != 0)
				{
					sz.AppendFormat("%s\n", Line[j]); 
				}
			}
		}
	}
	m_CntlParsedMsg.SetWindowTextA(sz);
	memset(BitFieldArray,0x00,sizeof(BitFieldArray));
	//UpdateData(FALSE); //transfer the data from variables back to control.
}

/*  Raghu on click clear both window should be cleared.*/
void Cex_amex_parserDlg::OnBnClickedClear()
{
	m_CntlParsedMsg.SetWindowTextA("");
	m_flowfile.SetString("");
	exportbtn.EnableWindow(FALSE);
	UpdateData(FALSE);
}

void ascii_to_bin (LPBYTE src, LPBYTE dst, INT len)
{
   INT  i, j, k;

   for (i = 0; i < (len*2); i+=2)
   {
      /* if I hit a null terminator, quit.  */
      if (src[i] == '\0')
         break;

      /* if these are leading blanks, leave the bcd zero.  */
      if (src[i] != ' ')
      {
         if (src[i] <= '9')
           dst[i/2] = ((src[i]-'0')*16); 
         else
           dst[i/2] = ((src[i]-'A')+0x0A)*16;   /* handle 0a-0f */
      }

      if (((i+1) < (len*2)) && (src[i+1] != ' '))
      {
         if (src[i+1] <= '9')
           dst[i/2] += (src[i+1]-'0'); 
         else
         {
           j         = (toupper(src[i+1]) - 'A');
           k         = j + 0x0A;
           dst[i/2] += k;

           /* dst[i/2] += ((src[i+1]-'A')+0x0A);   */
         }
      }
   }
} /* ascii_to_bin */


/* Field 61 - get_field_61*/

	/*
		(3) (numeric EBCDIC)             Variable Length Indicator (VLI)

		(2) (alphanumeric)      Subfield 2 Primary ID

		(3) (alphanumeric)      Subfield 3 Secondary ID

		(2) (alphanumeric)      Subfield 4 Electronic Commerce Indicator (ECI)

		(4) (alphanumeric)      Subfield 5 AEVV ID

		(20) (binary)            Subfield 6 American Express Verification Value (AEVV)

		(3) (alphanumeric)       Subfield 7 SafeKey Transaction ID

		(20) (binary)			Subfield 8 SafeKey Transaction ID Value
	*/
void get_field_61()
{
	int  priv61_rawdatalen   = 0;
	CHAR VLIA[4]             = {0};
	CHAR priv61_rawdata[256] = {0};
	CHAR Parsd_priv61[256]   = {0};
	char bin_asci[100]       = {0};

	/*Subfield 1 Variable Length Indicator (VLI) 3 bytes*/
	memcpy (VLIA, HOST_MESSAGE.priv61_len, 3);
	priv61_rawdatalen = atoi(VLIA);
	memcpy(priv61_rawdata, HOST_MESSAGE.priv61, priv61_rawdatalen);

	for (int i = 0; i < 11; i++)
	{
		Parsd_priv61[i] = ascii_ebcdic_conv(priv61_rawdata[i], E2A);
	}

	/* Subfield 6 American Express Verification Value (AEVV) 20 bytes */
	genutil_bin_to_ascii((BYTE*)&priv61_rawdata[11], 20, bin_asci, 40);
	strncat(Parsd_priv61, bin_asci, 40);

	/*	Subfield 7 SafeKey Transaction ID 3 bytes */
	for (int i = 31; i < 34; i++)
	{
		Parsd_priv61[20+i] = ascii_ebcdic_conv(priv61_rawdata[i], E2A);
	}
	
    /*	Subfield 8 SafeKey Transaction ID Value 20 bytes*/
	memset(bin_asci, 0, sizeof(bin_asci));
	genutil_bin_to_ascii((BYTE*)&priv61_rawdata[34], 20, bin_asci, 40);
	strncat(Parsd_priv61, bin_asci, 40);

	memset(field_61, 0, sizeof(field_61));
	memcpy(field_61, VLIA, 3);
	if(priv61_rawdatalen < 54)
		memcpy(field_61+3, Parsd_priv61, priv61_rawdatalen);
	else
		memcpy(field_61+3, Parsd_priv61, strlen(Parsd_priv61));
}

/************************************
 *
 *  NAME:         Generate_EMV_Details
 *
 *  DESCRIPTION:  This procedure copies information from a string sent by the
 *                host by parsing it to print as ascii.  
 *
 *  INPUTS:       None
 *
 *  OUTPUTS:      Parsed data will be filled in Host structure
 *
 *  AUTHOR:       Raghavendra H R
/**************************************/
void Generate_EMV_Details()
{
	memset(iccdata,0,strlen((char*)iccdata));
	
	// version name and number copied. 
		
	
	// app_crypto added 
	if(strlen((char*)EMV_details.app_crypto)!=0)
	{
		strncat((char*)iccdata,"9F26",4);
		strncat((char*)iccdata,(char*)EMV_details.app_crypto,APP_CRYT_DATA_ASCII_LEN);
	}
	// IssuerAppData 
	if(strlen((char*)EMV_details.iad)!=0)
	{
		strncat((char*)iccdata,"9F10",4);
		strncat((char*)iccdata,(char*)EMV_details.iad,ISSUERAPPID);
	}
	//UnpredictableNo
	if(strlen((char*)EMV_details.unpredictable_number)!=0)
	{
		strncat((char*)iccdata,"9F37",4);
		strncat((char*)iccdata,(char*)EMV_details.unpredictable_number,UNPRE_NUM_DATA_ASCII_LEN);
	}
	//AppTxnCounter
	if(strlen((char*)EMV_details.atc)!=0)
	{
		strncat((char*)iccdata,"9F36",4);
		strncat((char*)iccdata,(char*)EMV_details.atc,ATC_DATA_ASCII_LEN);
	}
	//TermVerifResults
	if(strlen((char*)EMV_details.tvr)!=0)
	{
		strncat((char*)iccdata,"95",2);
		strncat((char*)iccdata,(char*)EMV_details.tvr,TVR_DATA_ASCII_LEN);
	}
	//TxnDate
	if(strlen((char*)EMV_details.tran_date)!=0)
	{
		strncat((char*)iccdata,"9A",2);
		strncat((char*)iccdata,(char*)EMV_details.tran_date,TAR_DATE_DATA_ASCII_LEN);
	}
	//TxnType
	if(strlen((char*)EMV_details.tran_type)!=0)
	{
		strncat((char*)iccdata,"9C",2);
		strncat((char*)iccdata,(char*)EMV_details.tran_type,TAR_TYPE_DATA_ASCII_LEN);
	}
	//AmountAuthNum
	if(strlen((char*)EMV_details.tran_amount)!=0)
	{
		strncat((char*)iccdata,"9F02",4);
		strncat((char*)iccdata,(char*)EMV_details.tran_amount,AMT_AUTH_DATA_ASCII_LEN);
	}
	//TxnCurrencyCode
	if(strlen((char*)EMV_details.currency_code)!=0)
	{
		strncat((char*)iccdata,"5F2A",4);
		strncat((char*)iccdata,(char*)EMV_details.currency_code,TRA_CUR_CODE_DATA_ASCII_LEN);
	}
	//TerminalCountryCode
	if(strlen((char*)EMV_details.country_code)!=0)
	{
		strncat((char*)iccdata,"9F1A",4);
		strncat((char*)iccdata,(char*)EMV_details.country_code,TER_CUR_CODE_DATA_ASCII_LEN);
	}
	//AppInterchProfile
	if(strlen((char*)EMV_details.interchange_profile)!=0)
	{
		strncat((char*)iccdata,"82",2);
		strncat((char*)iccdata,(char*)EMV_details.interchange_profile,AIP_DATA_ASCII_LEN);
	}
	//AmountOtherNum
	if(strlen((char*)EMV_details.other_amount)!=0)
	{
		strncat((char*)iccdata,"9F03",4);
		strncat((char*)iccdata,(char*)EMV_details.other_amount,AMT_OTH_DATA_ASCII_LEN);
	}
	//PANSequenceNo
	if(strlen((char*)EMV_details.pan_sequence_number)!=0)
	{
		strncat((char*)iccdata,"5F34",4);
		strncat((char*)iccdata,(char*)EMV_details.pan_sequence_number,APP_PAN_SEQ_NUM_DATA_ASCII_LEN);
	}
	//CryptogramInfoData
	if(strlen((char*)EMV_details.crypto_info)!=0)
	{
		strncat((char*)iccdata,"9F27",4);
		strncat((char*)iccdata,(char*)EMV_details.crypto_info,CRYPTO_INFO_DATA_ASCII_LEN);
	}
}


void Cex_amex_parserDlg::OnBnClickedButton1()
{
	char *parsedData = 0;
	CString buffer;

	// TODO: Add your control notification handler code here
	if (!OpenClipboard())
	{
		AfxMessageBox(_T("Cannot open the Clipboard"));
		return;
	}
	// Remove the current Clipboard contents
	if (!EmptyClipboard())
	{
		AfxMessageBox(_T("Cannot empty the Clipboard"));
		return;
	}

	// Get the parsed data
	m_CntlParsedMsg.GetWindowText(buffer);
	int parseSize = strlen(buffer);

	HGLOBAL hGlob = GlobalAlloc(GMEM_FIXED, parseSize);
	memcpy((char*)hGlob, buffer, parseSize);

	// For the appropriate data formats...
	if (SetClipboardData(CF_TEXT, hGlob) == NULL)
	{
		CString msg;
		msg.Format(_T("Unable to set Clipboard data, error: %d"), GetLastError());
		AfxMessageBox(msg);
		CloseClipboard();
		//GlobalFree(hGlob);
		return;
	}
	CloseClipboard();
}


void Cex_amex_parserDlg::OnBnClickedButton2()
{
	AmexInfo amx;
	amx.DoModal();
	// TODO: Add your control notification handler code here
}



void Cex_amex_parserDlg::OnBnClickedExport()
{
	// TODO: Add your control notification handler code here
	FILE *m_pFile;
	char szCurDir[200] = {0};
	char key[100], *value;
	CString data1, data2;
	int key_len = 0;

	// TODO: Add your control notification handler code here
	CFileDialog fd(FALSE, "csv",NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_ENABLESIZING , "Comma Seperated files (*.csv) | *.csv",this);
	if(fd.DoModal() == IDOK)
	{
		SetCurrentDirectory(szCurDir);
		CString filename = fd.GetPathName();
		m_pFile = fopen(filename, "w");
		if(m_pFile == NULL)
		{
			MessageBox("ERROR in opening the file");
			return ;
		}

		fprintf(m_pFile,"Bit Map, Data\n");
	}
	else 
	{
		SetCurrentDirectory(szCurDir);
		return;
	}

	//fprintf(m_pFile,"RAW PACKET, %s\n\n", m_flowfile);

	for(int i=0; i<256; i++)
	{
		if(strlen(Line[i]) != 0)
		{
			if(strstr(Line[i], "-----") != NULL)
			{
				fprintf(m_pFile, "%.40s\n", Line[i]);
				continue;
			}
			value = strstr(Line[i], ":");
			key_len = strlen(Line[i])-strlen(value);
			if(value != NULL)
				value = value + 2;
			memset(key, 0, sizeof(key));
			strncpy(key, (char*)Line[i], key_len);

			data1 = key;
			data2 = value;

			data1 = "\t" + data1;
			data2 = "\t" + data2;

			fprintf(m_pFile,"%s, %s", data1, data2);
			fprintf(m_pFile, "\n");
		}
	}
	fclose(m_pFile);
}
