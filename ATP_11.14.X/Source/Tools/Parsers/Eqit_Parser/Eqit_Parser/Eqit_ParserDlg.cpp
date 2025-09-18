
// Mcrd_ParserDlg : implementation file
//

#include "stdafx.h"
#include "Eqit_Parser.h"
#include "Eqit_ParserDlg.h"
#include "afxdialogex.h"
#include "EqitParser.h"
#include "BASICTYP.H"
#include <math.h>
#include "EqitInfo.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#endif


void ascii_to_bin (LPBYTE src, LPBYTE dst, INT len);
AUTH_TX auth_tx;
AUTH_TX AUTH_TX_V;
EMV EMV_details;
BYTE      a2e[257];
BYTE      e2a[257];
BOOLEAN maps_initialised=false;
// CAboutDlg dialog used for App About

#define  THIRD_BITMAP_SIZE					24
#define  EXTENDED_BITMAP_SIZE    			32
#define  NORMAL_BITMAP_SIZE					16
CHAR astr_bitmap[64]= {0};

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
	DDX_Control(pDX, IDC_COPY, parsedCopy);
	DDX_Control(pDX, IDC_EXPORT, exportbtn);
}

BEGIN_MESSAGE_MAP(Cex_amex_parserDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_PARSE, &Cex_amex_parserDlg::OnBnClickedParse)
	ON_BN_CLICKED(IDC_CLEAR, &Cex_amex_parserDlg::OnBnClickedClear)
	ON_EN_CHANGE(IDC_PARSEDMSG, &Cex_amex_parserDlg::OnEnChangeParsedmsg)
	ON_EN_CHANGE(IDC_FLOWFILE, &Cex_amex_parserDlg::OnEnChangeFlowfile)
	ON_BN_CLICKED(IDC_COPY, &Cex_amex_parserDlg::OnBnClickedCopy)
	ON_BN_CLICKED(IDC_BUTTON1, &Cex_amex_parserDlg::OnBnClickedButton1)
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


static int temp_55_counter;
static char   Line[256][256];

/* Response Codes */
#define  APPROVED         "00"
#define  CALL_ISSUER      "01"
#define  DO_NOT_HONOR     "05"
#define  INVALID_TXN      "12"
#define  ACCT_NOT_FOUND   "14"
#define  TIMED_OUT        "19"
#define  NO_ACTION_TAKEN  "21"
#define  FORMAT_ERROR     "30"
#define  EXPIRED_CARD     "54"
#define  NOT_PERMITTED    "57"
#define  EXCEEDS_VELOCITY "61"
#define  HOST_UNAVAILABLE "91"
#define  SYSTEM_ERROR     "96"


/* Miscellaneous */
#define  CVV_TEST_FLAG ' '  /* Space */
#define  CAVV_RESULT_LOC 14
#define  CARC_RESULT_LOC  8
#define  CVV2_RESULT_LOC 10

#define  VISA_HDR_SIZE   22
#define  REJECT_VISA_HDR_SIZE 26
#define THIRD_BITFIELD 192
#define MSG_HEADER_LEN 				 	1
#define MSG_HEADER_SUCC_DATA            0x2E
#define MSG_HEADER_FLAG_VER_LEN		 	0x01
#define MSG_HEADER_FLAG_TEST_VER		0x00
#define MSG_HEADER_FLAG_PROD_VER		0x01
#define MSG_HEADER_FLAG_CAD             0x02
#define MSG_HEADER_FLAG_KEY_SEND        0x81
#define TOL_MEG_LEN                  	4
#define DEST_ID_LEN						11
#define CUPS_ID                         "    00010344"
#define SOURCE_ID_LEN					11
#define RESV_FOR_USE_LEN				3
#define BATCH_NO_LEN					1
#define BATCH_NUM                       0x00
#define TRAN_INFO_LEN					8
#define TRAN_INFO_DATA                  "00000000"
#define USER_INFO_LEN					1
#define USER_INFO_DATA                  0x00
#define REJECT_CODE_LEN					5
#define REJECT_CODE_DATA                "00000"
#define CUP_HDR_SIZE_REJECTED           46

/* Other #define of CUP */
#define EXTENDED_BITFIELD              128
#define NORMAL_BITFIELD                64
#define COMM_BUF_SZE                   1500

/*EVM data Ascii Length */
#define APP_CRYT_DATA_ASCII_LEN			16
#define UNPRE_NUM_DATA_ASCII_LEN		8
#define ATC_DATA_ASCII_LEN				4
#define TVR_DATA_ASCII_LEN				10
#define TAR_DATE_DATA_ASCII_LEN			6
#define TAR_TYPE_DATA_ASCII_LEN			2
#define AMT_AUTH_DATA_ASCII_LEN			12
#define TRA_CUR_CODE_DATA_ASCII_LEN		4
#define TER_CUR_CODE_DATA_ASCII_LEN		4
#define AIP_DATA_ASCII_LEN				4
#define AMT_OTH_DATA_ASCII_LEN			12
#define APP_PAN_SEQ_NUM_DATA_ASCII_LEN	2
#define CRYPTO_INFO_DATA_ASCII_LEN		2

#define N_BFData ( sizeof(BFData) / sizeof (struct bitfield_data) )

/* LENGTHs FOR EACH BIT FIELD */

#define bf2_len       2
#define bf2_max_len  19
#define bf3_len       6
#define bf4_len      12
#define bf5_len      12
#define bf6_len      12
#define bf7_len      10
#define bf9_len       8
#define bf10_len      8
#define bf11_len      6
#define bf14_len      4
#define bf15_len      4
#define bf16_len      4
#define bf18_len      4
#define bf20_len      3
#define bf22_len      3
#define bf32_len      2
#define bf32data_len 11
#define bf33_len      2
#define bf35_len      2
#define bf37_len     12
#define bf38_len      6
#define bf39_len      2
#define bf41_len      8
#define bf42_len     15
#define bf44_len      2
#define bf45_len      2
#define bf48_len      3
#define bf49_len      3
#define bf50_len      3
#define bf51_len      3
#define bf54_len      3
#define bf60_len      3
#define bf61_len      3
#define bf62_len      3
#define bf63_len      3
#define bf70_len      3
#define bf90_len     42
#define bf94_len      7
#define bf102_len     2
#define bf103_len     2
#define bf120_len     3
#define bf121_len     3

#define bf127_len     3

#define  PESO     "608"
#define  USD      "840"
       
#define THIRD_BITFIELD    192
#define EXTENDED_BITFIELD 128
#define NORMAL_BITFIELD    64



char FieldStrings [][256] =
{
"000 MsgType				: ",
"001 BitMap				    : ",
"002 PAN					: ",
"003 ProcessingCode				: ",
"004 TxnAmount				: ",
"005 SettleAmount		                     	: ",
"006 BillingAmount				: ",
"007 TransmitDateTime			: ",
"008 BillingFeeAmount		: ",
"009 SettleConvRate			: ",
"010 BillingConvRate				: ",
"011 SystemTraceNo				: ",
"012 TxnTime				: ",
"013 TxnDate				: ",
"014 ExpirationDate				: ",
"015 SettleDate				: ",
"016 ConvDate				: ",
"017 CaptureDate			: ",
"018 Merchant type				: ",
"019 AcquiringCountry			: ",
"020 PANExtCountry			: ",
"021 ForwardCountry			: ",
"022 POSEntryMode				: ",
"023 CardSequenceNo			: ",
"024 NII					: ",
"025 POSConditionCode			: ",
"026 POSPINCode				: ",
"027 AuthIdLen				: ",
"028 TxnFeeAmount				: ",
"029 SettleFeeAmount		: ",
"030 TxnProcFeeAmount		: ",
//"031 SettleProcFeeAmount	: ",
"031 TransactionID			: ",
"032 AcquiringIdCode	                		: ",
"033 ForwardIdCode			: ",
"034 PANExt					: ",
"035 Track2				: ",
"036 Track3					: ",
"037 RetRefNo				: ", 
"038 AuthID				: ",
"039 ResponseCode				: ",
"040 ServiceRestrictCode	: ",
"041 TerminalID				: ",
"042 AcquirerID				: ",
"043 AcquirerName				: ",
"044 AddRespData				: ",
"045 Track1					: ",
"046 ISOAddData				: ",
"047 NationalAddData		: ",
"048 PrivateAddData				: ",
"049 TxnCurrency				: ",
"050 SettleCurrency				: ",
"051 BillingCurrency				: ",
"052 PINData				: ",
"053 SecurityInfo				: ",
"054 AddAmounts				: ",
"055 IccData				: ",
"056 Field56				: ",
"057 Additional data prvt				: ",
"058 Field58				: ",
"059 Detaild Inqry Data1				: ",
"060 Advice code 				: ",
"061 Pos Data				: ",
"062 Inf Data				: ",
"063 Bank Net				: ",
"064 MsgAuthCode			: ",
"065 Field 65               : ",
"066 SettlementCode			: ",
"067 PaymentCodeEx			: ",
"068 RecInstCntrCode		: ",
"069 SettlInstCntrCode		: ",
"070 Nmi Code				: ",
"071 MessageNo				: ",
"072 MessageNoLast			: ",
"073 DateAction				: ",
"074 CreditsNo				: ",
"075 CreditsRevNo			: ",
"076 DebitsNo				: ",
"077 DebitsRevNo			: ",
"078 TransferNo				: ",
"079 TransferRevNo			: ",
"080 InquiriesNo			: ",
"081 AuthNo					: ",
"082 CProcFeeAmount			: ",
"083 CTxnFeeAmount			: ",
"084 DProcFeeAmount			: ",
"085 DTxnFeeAmount			: ",
"086 CAmount				: ",
"087 CRevAmount				: ",
"088 DAmount				: ",
"089 DRevAmount				: ",
"090 OriginDataElement			: ",
"091 FileUpdateCode			: ",
"092 FileSecurityCode		: ",
"093 ResponseIndicator		: ",
"094 ServiceIndicator		: ",
"095 ReplacementAmounts			: ",
"096 MessageSecCode			: ",
"097 AmountNetSettle		: ",
"098 Payee					: ",
"099 SettleIdCode			: ",
"100 ReceivingIdCode			: ",
"101 Filename				: ",
"102 AccountId1				: ",
"103 AccountId2				: ",
"104 TxnDescription				: ",
"105 Field105				: ",
"106 Field106				: ",
"107 Field107				: ",
"108 De-108	MoneySend			: ",
"109 Field109				: ",
"110 Field110				: ",
"111 Field111				: ",
"112 Addldata Nation				: ",
"113 Field113				: ",
"114 Field114				: ",
"115 Field115				: ",
"116 Field116				: ",
"117 Field117				: ",
"118 Field118				: ",
"119 Field119				: ",
"120 Org Msg Id				: ",
"121 Mc_agent				: ",
"122 Addlrec				: ",
"123 IsurInstitutersrvd				: ",
"124 MemData				: ",
"125 Field125				: ",
"126 Field126				: ",
"127 Priv Dta				: ",
"128 MsgAuthCode				: ",
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
	BYTE time          [6];
	BYTE date          [4];
	BYTE exp_date		[4];
	BYTE date_settle   [4];
	BYTE date_conv     [4];
	BYTE merch_type	   [4];
	BYTE coun_code     [3];
	BYTE fwd_inst_cnt_code [3];
	BYTE posentry      [3];
	BYTE acq_inst_cnt_code [3];
	BYTE cardseqnum		[3];
	BYTE pos_condcd    [2];
	BYTE tran_fee_amt  [9];
	BYTE acq_inst_len  [2];
	BYTE acq_inst_id   [11];
	BYTE fwd_inst_len  [2];
	BYTE fwd_inst_id   [11];
	BYTE track2_len    [2];
	BYTE track2        [37];
	BYTE track3_len    [3];
	BYTE track3        [104];
	BYTE rrn           [12];
	BYTE authcode      [6];
	BYTE responsecd    [2];
	BYTE termid        [8];
	BYTE cardaccid     [15];
	BYTE cardaccname   [40];
	BYTE addl_rsp_len  [2];
	BYTE addlrspdata   [25];
	BYTE addl_data_len [3];
	BYTE addl_data     [70];
	BYTE tran_curr_cd  [3];
	BYTE pinblock      [16];
	BYTE ICCdata_len	[1];
	BYTE ICCdata	   [256];
	BYTE priv60_len    [1];
	BYTE priv60        [12];
	BYTE priv61_len    [1];
	BYTE priv61        [18];
	BYTE priv62_len    [3];
	BYTE priv62        [99];
	BYTE priv63_len    [2];  /* Changed the length from 3 to 2, Girija Y, TF */
	BYTE priv63        [99];
	BYTE nmi_code      [3];
	BYTE orig_data     [42];
	BYTE replace_amt   [42];
	BYTE rcvid_len     [2];
	BYTE rcvid         [11];
	BYTE acct1_len     [2];
	BYTE acct1         [28];
	BYTE acct2_len     [2];
	BYTE acct2         [28];
	BYTE trandesc_len  [3];
	BYTE trandesc      [512];/* DE104 */
	BYTE addl_trace_len[2];
	BYTE addl_trace      [50];/* DE115 */
	BYTE de126_len     [3];
	BYTE de126         [128];
	BYTE msg_authc     [16];
};
struct FieldNumberMappingToIsoMsgRec
{
	void *fldPtr[1000];
};


/*****************************************************************************
 *********************** Define all globals required here ********************
 *****************************************************************************/

INT     Field63_present;
INT		Field62_present;
INT     BitFieldSize    = NORMAL_BITFIELD;
INT		bitmapidx;              /* offset of bitmap... */
INT		reqidx;                  /* next open position in request buffer */
INT		buflen;  
INT		Txn_Is_A_Reject;
INT		invalid_msg_format=0;
struct	iso_msg_rec HOST_MESSAGE;
BYTE  iccdata[256]={0};
char ServiceCode[4]={0};
BYTE   ReqRsp_Buffer [COMM_BUF_SZE];

char  A2E[4] = "A2E";    /*  ASCII to EBCDIC conversion */
char  E2A[4] = "E2A";    /*  EBCDIC to ASCII conversion */

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

// From MCRD

enum bitfields 
{
   bf_done           = 0,      /* end of list                          */
   bf_1xbitfield     = 1,      /* extended bit field                   */
   bf_2pan           = 2,      /* pan                                  */
   bf_3proc          = 3,      /* processing code                      */
   bf_4amount        = 4,      /* amount                               */
   bf_5settamount    = 5,
   bf_7trntime       = 7,      /* transmission date-time               */
   bf_9settconvrate  = 9,
   bf_11stan         = 11,     /* system trace number                  */
   bf_12time         = 12,     /* time                                 */
   bf_13date         = 13,     /* date, trans                          */
   bf_14ExpDate		  =14,
   bf_15datestl      = 15,     /* settlement date                      */
   bf_16dateconv     = 16,
   bf_18mertype      = 18,     /* merchant type                        */
   bf_19acqinstcode  = 19,	    /* Acquiring istitution ID Code         */
   bf_20countrycode  = 20,
   bf_21fwdinstcntcode = 21,   /* Forwarding institution country code  */
   bf_22posentry     = 22,     /* pos entry mode                       */
   bf_23cardseqnum	 = 23,	   /* card sequence number */
   bf_25poscc        = 25,     /* pos condition code                   */
   bf_28tranfeeamt   = 28,     /* surcharge fee */
   bf_32idcode       = 32,     /* Acquiring institution's ID code      */
   bf_33fwdinstidcode= 33,     /* Forwarding institution ID code */
   bf_35track2       = 35,     /* track 2 data                         */
   bf_36track3       = 36,     /* track 3 data                         */
   bf_37rrn	         = 37,	    /* retrieval ref number */
   bf_38authcode     = 38,     /* auth code                            */
   bf_39respcode     = 39,     /* response code                        */
   bf_41termid       = 41,     /* terminal id                          */
   bf_42cardacc      = 42,     /* card acceptor id                     */
   bf_43cardacceptloc= 43,     /* card acceptor name/location */
   bf_44respdata     = 44,
   bf_48addldata     = 48,     /* additional data                      */
   bf_49trncurcd     = 49,     /* transaction currency code            */
   bf_52pinblock     = 52,     /* pin block                            */
   bf_55ICCdata		 = 55,
   bf_60priv60       = 60,     /* private field 60 */
   bf_61priv61       = 61,     /* private field 61 */
   bf_62priv62       = 62,     /* private field 62 */
   bf_63priv63       = 63,     /* private field 63 */
   bf_70nmicode      = 70,     /* network management information code  */
   bf_90orgdata      = 90,     /* original data elements               */
   bf_95repl_amt     = 95,     /* replacement amount                   */
   bf_100rcvid       = 100,    /* Receiving institution identification code */
   bf_102acct1       = 102,    /* account identification 1             */
   bf_103acct2       = 103,    /* account identification 2             */
   bf_104trandesc    = 104,    /* transaction description         */
   bf_115addl_trace	 = 115,		/* Additional trace data */
   bf_126priv126     = 126,    /* DE126 Pvt 126          */
   bf_128msg_authc   = 128,    /* message authentication code          */                        
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

#define  CHART_SIZE  90

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
         0x6F, 0x7A, 0x7B, 0x7C, 0x7D, 0x7E, 0x7F, 0x5F, 0x9A, 0x00
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
         0x3F, 0x3A, 0x23, 0x40, 0x27, 0x3D, 0x22, 0x5E, 0x5E, 0x30
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
   pBYTE           p_bd_pdata;                           /* ptr to data to move */
   INT             bd_fieldlen;                          /* # of bytes in iso8583 field */
   INT             (*bd_BFCond)(enum bitfields, BYTE);   /* ptr to conditional function */
   INT             (*bd_BFMove)(enum bitfields);         /* ptr to move function */
   INT             (*bd_BFRsp)(enum bitfields);          /* ptr to response handler function */
   };

/**********************************
      FUNCTION PROTOTYPES
**********************************/
/* functions local */

//For visa
void	vBCDMove(enum bitfields fieldid);
void	RspMovevBCD (enum bitfields fieldid);
INT		genutil_hex_to_int (pCHAR buf_ptr, INT buf_len  );
INT		Move_Always(enum bitfields, BYTE);
int    vEBCDICMove( enum bitfields fieldid );
INT		Move_IfThere(enum bitfields, BYTE);
INT		vMove_IfThere(enum bitfields fieldid, BYTE p_trantype);
INT		Move_BCD_Amount_IfThere(enum bitfields fieldid, BYTE p_trantype);
int	GenericMove(enum bitfields fieldid);
INT		MoveEMV_IfThere(enum bitfields fieldid, BYTE p_trantype);
INT		Move23_IfThere( enum bitfields fieldid, BYTE p_trantype );
int	RspMoveGeneric(enum bitfields fieldid);
int RspMovevData (enum bitfields fieldid);
int		get_variable_length( unsigned char *DataField, int FieldLen );
void convert_to_ebcdic( pBYTE source, pBYTE dest, INT len );
int		vDataMove(enum bitfields fieldid);
void    parsed_mssg();
int    EBCDICMove( enum bitfields fieldid );
int		RspEBCDICvMove (enum bitfields fieldid);
int		RspEBCDICMove (enum bitfields fieldid);
INT		CondExpd( enum bitfields bitfield, BYTE p_trantype );
INT		CondTrack1( enum bitfields bitfield, BYTE p_trantype );
void	Generate_EMV_Details();
INT		parse_host_msg( pCHAR err_buf );
INT		parse_track2( pAUTH_TX p_auth_tx, pCHAR error_msg );
void	trim_spaces( pBYTE pString );
void convert_to_ascii( pBYTE source, pBYTE dest, INT len );
void init_character_code_maps();
void ebcdic_to_hex( pBYTE instr, pBYTE outstr, INT len );
void	find_cavv_flag( BYTE len, pBYTE de44, pBYTE cavv_result );
void	find_carc_flag( BYTE len, pBYTE de44, pBYTE carc_result );
void	find_cvv2_result_code( BYTE len, pBYTE de44, pBYTE cvv2_result );
INT		isnum(pCHAR string);
BOOLEAN blank_or_null_string(pCHAR astr, INT max_len);
void	get_balances( pAddl_Amts p_acct_bal, pAUTH_TX p_auth_tx );
void	genutil_VISA_EMV_To_Auth_Tx( BYTE * emv_raw, INT total_len, pAUTH_TX p_auth_tx );
void	Field62Response( pAUTH_TX p_auth_tx );
INT		ParseField63( pCHAR f63, pAUTH_TX p_auth_tx );
void	LogStipReasonCode( pAUTH_TX p_auth_tx );
INT		Field116_Handler( pCHAR f116, pAUTH_TX p_auth_tx );
void	Field126Parser(  pAUTH_TX p_auth_tx, pBYTE cvv2_data  );
void	Rj_with_lead_zeros( pCHAR str, INT FieldWidth );
void	genutil_bcd_to_asc (BYTE bstring[], INT blen, pCHAR astr, INT astr_len);
BYTE	ascii_ebcdic_conv(BYTE conv_char, char * conv_type );
BYTE	ascii_ebcdic_conv_trk1( unsigned char conv_char, char *conv_type,unsigned char fldSep);
void	genutil_bin_to_ascii (BYTE bstring[], INT blen, pCHAR astr, INT astr_len);
INT		Move_Never(enum bitfields bitfield, BYTE p_trantype);
void genutil_VISA_EMV_To_Auth_Tx( pCHAR emv_raw, INT total_len, pAUTH_TX p_auth_tx );
INT RspEBCDICvMove_104 (enum bitfields fieldid);
INT RspEBCDICvMove_44 (enum bitfields fieldid);
INT vEBCDICMove_F126( enum bitfields fieldid );
INT Move_IfThere_44( enum bitfields fieldid, BYTE p_trantype );

/*****************************/

//static  struct bitfield_data *BFData; 
// From eqit

static  struct bitfield_data BFData[] = {
   {
   bf_2pan,                      /* field */
   HOST_MESSAGE.pan_len,         /* data to move */
   2,                            /* # of bytes in pan len field */
   Move_IfThere,                 /* conditional */
   vEBCDICMove,                  /* move routine */
   RspEBCDICvMove                /* response routine */
   },
   {
   bf_3proc,                     /* field */
   HOST_MESSAGE.proccode,        /* data to move */
   6,                            /* # of bytes in  iso field */
   Move_Always,                  /* mandatory */
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
   /* new */	   
   bf_5settamount,               /* field */
   HOST_MESSAGE.setl_amount,     /* data to move */
   12,                           /* # of bytes in  iso field */
   Move_IfThere,                  /* mandatory */
   EBCDICMove,                   /* move routine */
   RspEBCDICMove                 /* response routine */
   },
   {
   bf_7trntime,                  /* field */
   HOST_MESSAGE.gmt_time,        /* data to move */
   10,                           /* # of bytes in  iso field */
   Move_Always,                  /* conditional */
   EBCDICMove,                   /* move routine */
   RspEBCDICMove                 /* response routine */
   },
   {
   /* new */
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
   bf_12time,                    /* field */
   HOST_MESSAGE.time,            /* data to move */
   6,                            /* # of bytes in  iso field */
   Move_Always,                  /* conditional */
   EBCDICMove,                   /* move routine */
   RspEBCDICMove                 /* response routine */
   },
   {
   bf_13date,                    /* field */
   HOST_MESSAGE.date,            /* data to move */
   4,                            /* # of bytes in  iso field */
   Move_Always,                  /* conditional */
   EBCDICMove,                   /* move routine */
   RspEBCDICMove                 /* response routine */
   },
   {
   bf_14ExpDate,                    /* field */
   HOST_MESSAGE.exp_date,            /* data to move */
   4,                            /* # of bytes in  iso field */
   Move_IfThere,                  /* conditional */
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
   /* new */
   bf_16dateconv,                /* field */
   HOST_MESSAGE.date_conv,       /* data to move */
   4,                            /* # of bytes in  iso field */
   Move_IfThere,                  /* conditional */
   EBCDICMove,                   /* move routine */
   RspEBCDICMove                 /* response routine */
   },
   {
   /* new */
   bf_18mertype,                 /* field */
   HOST_MESSAGE.merch_type,            /* data to move */
   4,                            /* # of bytes in  iso field */
   Move_IfThere,                  /* conditional */
   EBCDICMove,                   /* move routine */
   RspEBCDICMove                 /* response routine */
   },
   {
   /* new */
   bf_19acqinstcode,             /* field */
   HOST_MESSAGE.acq_inst_cnt_code,/* data to move */
   3,                            /* # of bytes in  iso field */
   Move_IfThere,                  /* conditional */
   EBCDICMove,                   /* move routine */
   RspEBCDICMove                 /* response routine */
   },
   {
   /* new */
   bf_20countrycode,                    /* field */
   HOST_MESSAGE.coun_code,            /* data to move */
   3,                            /* # of bytes in  iso field */
   Move_IfThere,                  /* conditional */
   EBCDICMove,                   /* move routine */
   RspEBCDICMove                 /* response routine */
   },
   {
   /* new */
   bf_21fwdinstcntcode,                    /* field */
   HOST_MESSAGE.fwd_inst_cnt_code,            /* data to move */
   3,                            /* # of bytes in  iso field */
   Move_IfThere,                  /* conditional */
   EBCDICMove,                   /* move routine */
   RspEBCDICMove                 /* response routine */
   },
   {
   bf_22posentry,                /* field */
   HOST_MESSAGE.posentry,        /* data to move */
   3,                            /* # of bytes in  iso field */
   Move_Always,                  /* conditional */
   EBCDICMove,                   /* move routine */
   RspEBCDICMove                 /* response routine */
   },
   {
   bf_23cardseqnum,              /* field */
   HOST_MESSAGE.cardseqnum,      /* data to move */
   3,                            /* # of bytes in visa iso field */
   Move_IfThere,               /* conditional */
   EBCDICMove,                  /* move routine */
   RspEBCDICMove                /* response routine */
   },
   {
   bf_25poscc,                   /* field */
   HOST_MESSAGE.pos_condcd,      /* data to move */
   2,                            /* # of bytes in  iso field */
   Move_Always,                  /* conditional */
   EBCDICMove,                   /* move routine */
   RspEBCDICMove                 /* response routine */
   },
   {
   /* new */
   bf_28tranfeeamt,              /* field */
   HOST_MESSAGE.tran_fee_amt,    /* data to move */
   9,                            /* # of bytes in  iso field */
   Move_Always,                  /* conditional */
   EBCDICMove,                   /* move routine */
   RspEBCDICMove                 /* response routine */
   },
   {
   bf_32idcode,                  /* field */
   HOST_MESSAGE.acq_inst_len,    /* data to move */
   2,                            /* # of bytes in  iso field */
   Move_IfThere,                  /* conditional */
   vEBCDICMove,                  /* move routine */
   RspEBCDICvMove                /* response routine */
   },
   {
   bf_33fwdinstidcode,           /* field */
   HOST_MESSAGE.fwd_inst_len,    /* data to move */
   2,                            /* # of bytes in  iso field */
   Move_IfThere,                  /* conditional */  
   vEBCDICMove,                  /* move routine */
   RspEBCDICvMove                /* response routine */
   },
   {
   bf_35track2,                  /* field */
   HOST_MESSAGE.track2_len,      /* data to move */
   2,                            /* # of bytes in track2 len field */
   Move_IfThere,                  /* conditional - Changed to Cond - TF Phani */
   vEBCDICMove,                  /* move routine */
   RspEBCDICvMove                /* response routine */
   },
   {
   bf_36track3,                  /* field */
   HOST_MESSAGE.track3_len,      /* data to move */
   3,                            /* # of bytes in track3 len field */
   Move_IfThere,                 /* conditional */
   vEBCDICMove,                  /* move routine */
   RspEBCDICvMove                /* response routine */
   },
   {
   bf_37rrn,                    /* field */
   HOST_MESSAGE.rrn,            /* data to move */
   12,                          /* # of bytes in track2 len field */
   Move_Always,                 /* conditional */
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
   2,                            /* # of bytes in  iso field */
   Move_Always,                 /* conditional */
   EBCDICMove,                   /* move routine */
   RspEBCDICMove                 /* response routine */
   },
   {
   bf_41termid,                  /* field */
   HOST_MESSAGE.termid,          /* data to move */
   8,                            /* # of bytes in  iso field */
   Move_Always,                  /* mandatory */
   EBCDICMove,                   /* move routine */
   RspEBCDICMove                 /* response routine */
   },
   {
   bf_42cardacc,                 /* field */
   HOST_MESSAGE.cardaccid,       /* data to move */
   15,                           /* # of bytes in  iso field */
   Move_IfThere,                 /* conditional */
   EBCDICMove,                   /* move routine */
   RspEBCDICMove                 /* response routine */
   },
   {
   bf_43cardacceptloc,           /* field */
   HOST_MESSAGE.cardaccname,     /* data to move */
   40,                           /* # of bytes in  iso field */
   Move_Always,                  /* conditional */
   EBCDICMove,                   /* move routine */
   RspEBCDICMove                 /* response routine */
   },
   {
   bf_44respdata,                /* field */
   HOST_MESSAGE.addl_rsp_len,        /* data to move */
   2,                            /* # of bytes in  iso field */
   Move_IfThere_44,                 /* conditional */
   vEBCDICMove,                   /* move routine */
   RspEBCDICvMove_44                 /* response routine */
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
   Move_IfThere,                 /* mandatory    */
   EBCDICMove,                   /* move routine */
   RspEBCDICMove                 /* response routine */
   },
   {
   bf_52pinblock,                /* field */
   HOST_MESSAGE.pinblock,        /* data to move */
   16,                           /* # of bytes in  iso field */
   Move_IfThere,                 /* conditional    */
   GenericMove,                  /* move routine */
   RspMoveGeneric                /* response routine */
   },
   {
   bf_55ICCdata,                /* field */
   HOST_MESSAGE.ICCdata_len,      /* data to move */
   1,                            /* # of bytes in addl len field */
   Move_IfThere,                 /* conditional */
   vDataMove,                    /* move routine */
   RspMovevData                  /* response routine */
   },
   {
   bf_60priv60,                  /* field */
   HOST_MESSAGE.priv60,      /* data to move */
   12,                            /* # of bytes in priv60 len field */
   Move_IfThere,                 /* conditional */
   EBCDICMove,                     /* move routine */
   RspEBCDICMove                   /* response routine */
   },
   {
   bf_61priv61,                  /* field */
   HOST_MESSAGE.priv61_len,      /* data to move */
   3,                            /* # of bytes in priv61 len field */
   Move_IfThere,                 /* conditional */
   vEBCDICMove,                     /* move routine */
   RspEBCDICvMove                   /* response routine */
   },
   {
   bf_62priv62,                  /* field */
   HOST_MESSAGE.priv62_len,      /* data to move */
   3,                            /* # of bytes in  iso field */
   Move_IfThere,                  /* conditional */
   vEBCDICMove,                  /* move routine */
   RspEBCDICvMove                /* response routine */
   },
   {
   bf_63priv63,                  /* field */
   HOST_MESSAGE.priv63_len,      /* data to move */
   2,                             /* Changed the length from 3 to 2, Girija Y, TF */
   Move_IfThere,                  /* conditional */
   vEBCDICMove,                  /* move routine */
   RspEBCDICvMove                /* response routine */
   },
   {
   bf_70nmicode,                 /* field */
   HOST_MESSAGE.nmi_code,        /* data to move */
   3,                            /* # of bytes in  iso field */
   Move_IfThere,                  /* mandatory    */
   EBCDICMove,                   /* move routine */
   RspEBCDICMove                 /* response routine */
   },
   {  
   bf_90orgdata,                 /* field */
   HOST_MESSAGE.orig_data,       /* data to move */
   42,                           /* # of bytes in  iso field */
   Move_Always,                  /* mandatory    */
   EBCDICMove,                   /* move routine */
   RspEBCDICMove                 /* response routine */
   },
   {
   bf_95repl_amt,                /* field */
   HOST_MESSAGE.replace_amt,     /* data to move */
   42,                           /* # of bytes in  iso field */
   Move_IfThere,                  /* conditional  */
   EBCDICMove,                   /* move routine */
   RspEBCDICMove                 /* response routine */
   },  
   {
   bf_100rcvid,                  /* field */
   HOST_MESSAGE.rcvid_len,       /* data to move */
   2 ,                           /* # of bytes in  iso field */
   Move_IfThere,                 /* conditional */
   vEBCDICMove,                  /* move routine */
   RspEBCDICvMove                /* response routine */
   },
   {
   bf_102acct1,                  /* field */
   HOST_MESSAGE.acct1_len,       /* data to move */
   2 ,                           /* # of bytes in  iso field */
   Move_IfThere,                 /* conditional */
   vEBCDICMove,                  /* move routine */
   RspEBCDICvMove                /* response routine */
   },
   {
   bf_103acct2,                  /* field */
   HOST_MESSAGE.acct2_len,       /* data to move */
   2,                            /* # of bytes in  iso field */
   Move_IfThere,                 /* conditional */
   vEBCDICMove,                  /* move routine */
   RspEBCDICvMove                /* response routine */
   },
   {
   bf_104trandesc,               /* field */
   HOST_MESSAGE.trandesc_len,    /* data to move */
   3,                            /* # of bytes in track1 len field */
   Move_IfThere,                 /* conditional    */
   vDataMove,                    /* move routine */
   RspEBCDICvMove_104                  /* response routine */   
   },
   {  
   bf_115addl_trace,               /* field */
   HOST_MESSAGE.addl_trace_len,    /* data to move */
   2,                            /* # of bytes in track1 len field */
   Move_IfThere,                 /* conditional */
   vEBCDICMove,                  /* move routine */
   RspEBCDICvMove                /* response routine */
   },
   {
   bf_126priv126,				 /* field */
   HOST_MESSAGE.de126_len,       /* data to move */
   3,                            /* # of bytes in  iso field */
   Move_IfThere,                 /* conditional */
   vEBCDICMove_F126,             /* move routine */
   RspEBCDICvMove                 /* response routine */
   },
   {
   bf_128msg_authc,				 /* field */
   HOST_MESSAGE.msg_authc,       /* data to move */
   16,                            /* # of bytes in  iso field */
   Move_IfThere,                 /* conditional */
   EBCDICMove,                   /* move routine */
   RspEBCDICMove                 /* response routine */
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
   CHAR     temp_str[50] ={0};

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

/***********************************************
   GenericMove
************************************************/
INT GenericMove(enum bitfields fieldid)
{
   struct bitfield_data *pBF;
   INT                  ret_val = true;

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
   else
      ret_val = false;

   return( ret_val );

} /* end GenericMove */

/*****************************************************************************

  Function:    MoveEMV_IfThere

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
INT MoveEMV_IfThere(enum bitfields fieldid, BYTE p_trantype)
{
   struct bitfield_data *pBF;
   INT  i;

   /*---------------------------------------------------*/
   /* find pointer to bitfield info for this bitfield...*/
   /*---------------------------------------------------*/
   pBF = Find_Bitfield_Table(fieldid);

   /*---------------------------------------------------*/
   /* in any byte non-zero, it's there, return true     */
   /*---------------------------------------------------*/
   if (pBF != NULL)
   {
	  // only for parser
      // if ( (Third_Bitmap == true) && (fieldid > 128) )
         // return( true );

      for (i=0; i < pBF->bd_fieldlen; i++)
      {
         if ( (pBF->p_bd_pdata [i] != 0) && (pBF->p_bd_pdata [i] != ' ') ) 
            return(true);
      }
   }
   return(false);
} /* end Move_IfThere */

/*****************************************************************************

  Function:    Move23_IfThere

  Description: This function will determine if field 23 is present
  Author: 
      unknown
  Inputs:
      fieldid    - the field id of the data to be moved
      p_trantype - a pointer to tran type
  Outputs:
      None
  Return values:
      true  - field is  there 
      false - field not there
******************************************************************************/
INT Move23_IfThere( enum bitfields fieldid, BYTE p_trantype )
{
   struct bitfield_data *pBF;
   INT    retval = false;
  
   /*---------------------------------------------------*/
   /* find pointer to bitfield info for this bitfield...*/
   /*---------------------------------------------------*/
   pBF = Find_Bitfield_Table(fieldid);

   /*---------------------------------------------------*/
   /* in any byte non-zero, it's there, return true     */
   /*---------------------------------------------------*/
   if (pBF != NULL)
   {
      retval = true;
    //  de23_present = false; for only visa parser 
   }
   return( retval );
} /* end Move23_IfThere */


/*****************************************************************************

  Function:    Move_BCD_Amount_IfThere

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
  Modified by: TF PHANI
******************************************************************************/
 INT Move_BCD_Amount_IfThere(enum bitfields fieldid, BYTE p_trantype)
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
         if ( (pBF->p_bd_pdata [i] > 0) && (pBF->p_bd_pdata [i] < (0x99+1))  ) 
            return(true);
      }
   }
   return(false);
} /* end Move_BCD_Amount_IfThere */


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

/***********************************************
   RspMoveGeneric
************************************************/
INT RspMoveGeneric(enum bitfields fieldid)
{
   struct bitfield_data *pBF;
   INT                  ret_val = true;


   /*---------------------------------------------------*/
   /* find pointer to bitfield info for this bitfield...*/
   /*---------------------------------------------------*/
   pBF = Find_Bitfield_Table(fieldid);
   if (pBF != NULL)
   {
      if ( (reqidx + pBF->bd_fieldlen) <= COMM_BUF_SZE )
      {
         memcpy(pBF->p_bd_pdata,               /* to data pointer          */
                &ReqRsp_Buffer[reqidx],        /* move from request buffer */
                pBF->bd_fieldlen);             /* for length of field      */

         reqidx = reqidx + pBF->bd_fieldlen;   /* increment buffer pointer */
      }
      else
      {
         ret_val = false;
         invalid_msg_format = true;
      }
   }
   else
      ret_val = false;

   return( ret_val );

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
 /*****************************************************************************/
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
  
/*------------------------ get_variable_length -------------------------------------------------- */
int get_variable_length( unsigned char *DataField, int FieldLen )
{
   int    i;
   double len=0;

   if ( 0 < FieldLen )
   {
      for( i=0; i<FieldLen; i++ )
      {
         len += (int)( DataField[i] - 0x30 ) * (pow(10,(double)(FieldLen-i-1)));
      }
   }
   return( (int)len );
}
/* Ends get_variable_length */



/*************************************************************************************
       NAME:               convert_to_ebcdic

       DESCRIPTION:        converts a stream of bytes from ASCII to EBCDIC

       INPUTS:
           source          pointer to a stream of bytes to be converted
           len             length of the byte stream
       OUTPUTS:
           dest            pointer to stream of converted bytes

       RETURNS:            
           None.
           
       AUTHOR:             Ram Malathkar
*************************************************************************************/
void convert_to_ebcdic( pBYTE source, pBYTE dest, INT len )
{
   INT temp;
   BYTE temp_byte;

   if( !maps_initialised )
      init_character_code_maps();

   for( temp = 0; temp < len; temp++ )
   {
      temp_byte = *(source + temp);
      *(dest + temp) = a2e[temp_byte];
   }

   return;
} /* convert_to_ebcdic */
 
INT vDataMove(enum bitfields fieldid)
{
   struct bitfield_data *pBF;
   INT                  movelen;
      INT                  ret_val = true;
	char temp_len[3];
   /*---------------------------------------------------*/
   /* find pointer to bitfield info for this bitfield...*/
   /*---------------------------------------------------*/
   pBF = Find_Bitfield_Table(fieldid);

   if (pBF != NULL)
   {
      if(fieldid == 104)
	  {memcpy(temp_len,&pBF->p_bd_pdata [0],3);
		movelen = atoi(temp_len);
		//sprintf(temp_str,"%2x",movelen);
      ReqRsp_Buffer [reqidx] = ascii_ebcdic_conv(temp_len [0],A2E);
      reqidx ++;
	  ReqRsp_Buffer [reqidx] = ascii_ebcdic_conv(temp_len [1],A2E);
	   //ReqRsp_Buffer [reqidx] = temp_str[1];
	  reqidx ++;
      ReqRsp_Buffer [reqidx] = ascii_ebcdic_conv(temp_len [2],A2E);
      reqidx ++;

	  memcpy(&ReqRsp_Buffer[reqidx], &pBF->p_bd_pdata[3], movelen);
	  }	
	  else
	  {movelen = (pBF->p_bd_pdata [0]);
	   ReqRsp_Buffer [reqidx] = pBF->p_bd_pdata [0];
      reqidx++;
	    memcpy(&ReqRsp_Buffer[reqidx], &pBF->p_bd_pdata[1], movelen);
	  }
     // memcpy(&ReqRsp_Buffer[reqidx], &pBF->p_bd_pdata[1], movelen);

      reqidx = reqidx + movelen;   /* increment buffer pointer */
   }
   return ret_val;
} /* end vDataMove */

INT RspMovevData (enum bitfields fieldid)
{
   struct bitfield_data *pBF;
   INT                  movelen;
      INT                  ret_val = true;

   /*---------------------------------------------------*/
   /* find pointer to bitfield info for this bitfield...*/
   /*---------------------------------------------------*/
   pBF = Find_Bitfield_Table(fieldid);

   if (pBF != NULL)
   {
      movelen = ReqRsp_Buffer[reqidx];

      pBF->p_bd_pdata[0] = ReqRsp_Buffer[reqidx];
      reqidx++;

      memcpy(&pBF->p_bd_pdata[1], &ReqRsp_Buffer[reqidx], movelen);

      reqidx = reqidx + movelen;   /* increment buffer pointer */
   }
   return ret_val;
} /* end RspMovevData */
/******************************************************************************
 *
 *  NAME:         Move_IfThere
 *
 *  DESCRIPTION:  This function returns a value based on whether or not there
 *                is data in the specified HOST_MESSAGE field.  If there is
 *                data in it, TRUE is returned, else FALSE is returned.
 *
 *  INPUTS:       fieldid - Field Id to denote the source field of the data
 *                p_trantype - transaction type
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   True if data exists to be moved, else false
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
INT Move_IfThere( enum bitfields fieldid, BYTE p_trantype )
{
   struct bitfield_data *pBF;
   INT i;

   /*---------------------------------------------------*/
   /* find pointer to bitfield info for this bitfield...*/
   /*---------------------------------------------------*/
   pBF = Find_Bitfield_Table( fieldid );
   if (pBF != NULL)
   {
      /*---------------------------------------------------*/
      /* in any byte non-zero, it's there, return true     */
      /*---------------------------------------------------*/
      for ( i=0; i < pBF->bd_fieldlen; i++ )
      {
         if ( (pBF->p_bd_pdata [i] != 0) && (pBF->p_bd_pdata [i] != ' ') ) 
            return( true );
      }
   }
   return( false );
} /* end Move_IfThere */

INT Move_IfThere_44( enum bitfields fieldid, BYTE p_trantype )
{
   struct bitfield_data *pBF;
   INT i;

   /*---------------------------------------------------*/
   /* find pointer to bitfield info for this bitfield...*/
   /*---------------------------------------------------*/
   pBF = Find_Bitfield_Table( fieldid );
   if (pBF != NULL)
   {
      /*---------------------------------------------------*/
      /* in any byte non-zero, it's there, return true     */
      /*---------------------------------------------------*/
      for ( i=0; i < pBF->bd_fieldlen; i++ )
      {
         if ( (pBF->p_bd_pdata [i] != 0))
            return( true );
      }
   }
   return( false );
} /* end Move_IfThere */

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
INT vMove_IfThere(enum bitfields fieldid, BYTE p_trantype)
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


/******************************************************************************
 *
 *  NAME:         Move_Always
 *
 *  DESCRIPTION:  This function returns true.  It is used to indicate that data
 *                SHOULD be moved from HOST_MESSAGE into ReqRsp_Buffer.
 *
 *  INPUTS:       fieldid - Field Id to denote the source field of the data
 *                p_trantype - transaction type
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   True
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
INT Move_Always( enum bitfields bitfield, BYTE p_trantype )
{
   return( true );
} /* end Move_Always */
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
void vBCDMove(enum bitfields fieldid)
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

      if (( 60 == fieldid ) || ( 61 == fieldid ) || ( 120 == fieldid ))
         movelen *= 2;

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
void RspMovevBCD (enum bitfields fieldid)
{
   struct bitfield_data *pBF;
   INT                  movelen;

   /*---------------------------------------------------*/
   /* find pointer to bitfield info for this bitfield...*/
   /*---------------------------------------------------*/
   pBF = Find_Bitfield_Table(fieldid);

   if (pBF != NULL)
   {
      if ( (ReqRsp_Buffer[0] == 0x03) && (ReqRsp_Buffer[1] == 0x02) )
      {
         /* Convert length from BCD to binary. */
         movelen  = ((ReqRsp_Buffer[reqidx] & 0xf0) >> 4) * 10;
         movelen +=   ReqRsp_Buffer[reqidx] & 0x0f;
      }
      else
         movelen = ReqRsp_Buffer[reqidx];

      pBF->p_bd_pdata[0] = movelen;
      reqidx++;

      if (( 60 == fieldid ) || ( 61 == fieldid ) || ( 120 == fieldid ))
         movelen *= 2;

      if ((movelen % 2) != 0)
         movelen++;

      movelen/= 2;

      if (( reqidx + movelen) <= COMM_BUF_SZE)
      {
         memcpy(&pBF->p_bd_pdata[1], &ReqRsp_Buffer[reqidx], movelen);

         reqidx = reqidx + movelen;   /* increment buffer pointer */
      }
      else
         invalid_msg_format = true;
   }
} /* end RspMovevBCD */

INT genutil_hex_to_int (pCHAR buf_ptr, INT buf_len  )
{
   INT     index;
   INT     n;
  
   n=0;

   for (index = 0; index < buf_len; index++, buf_ptr++) 
      {
      if (*buf_ptr >= '0'  &&  *buf_ptr <= '9')
         n = (n * 16) + ((INT) (*buf_ptr - '0'));
      else
         if (*buf_ptr >= 'A'  &&  *buf_ptr <= 'F')
            n = (n * 16) + ((INT) (*buf_ptr - 'A') + 10);
         else
            return (-1);  /* error*/
      } /* end of for*/

  return (n);
} /* genutil_hex_to_int */
/******************************************************************************
 *
 *  NAME:         vEBCDICMove
 *
 *  DESCRIPTION:  This function converts a variable length string into EBCDIC,
 *                then copies it into ReqRsp_Buffer.  The length is also copied
 *                into the buffer in EBCDIC format.
 *
 *  INPUTS:       fieldid - Field Id to denote where the source data exists
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   False if bit field Id is not defined, else true
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
INT vEBCDICMove( enum bitfields fieldid )
{
   struct bitfield_data *pBF;
   INT                  movelen;
   INT                  ret_val = true;

   /*---------------------------------------------------*/
   /* find pointer to bitfield info for this bitfield...*/
   /*---------------------------------------------------*/
   pBF = Find_Bitfield_Table(fieldid);
   if (pBF != NULL)
   {
      movelen  = get_variable_length( pBF->p_bd_pdata, pBF->bd_fieldlen );
      movelen += pBF->bd_fieldlen;

      convert_to_ebcdic( pBF->p_bd_pdata, &ReqRsp_Buffer[reqidx], movelen );
      reqidx += movelen;
   }
   else
      ret_val = false;

   return( ret_val );

} /* end vEBCDICMove */

INT vEBCDICMove_F126( enum bitfields fieldid )
{
   struct bitfield_data *pBF;
   INT                  movelen;
   INT                  ret_val = true;

   /*---------------------------------------------------*/
   /* find pointer to bitfield info for this bitfield...*/
   /*---------------------------------------------------*/
   pBF = Find_Bitfield_Table(fieldid);
   if (pBF != NULL)
   {
      movelen  = get_variable_length( pBF->p_bd_pdata, pBF->bd_fieldlen );
      movelen += pBF->bd_fieldlen;

	  convert_to_ebcdic( pBF->p_bd_pdata, &ReqRsp_Buffer[reqidx], movelen );
	  if (ReqRsp_Buffer[reqidx + pBF->bd_fieldlen] == 0)
	  {
		  ReqRsp_Buffer[reqidx + pBF->bd_fieldlen] = 0xF0;
	  }
	  reqidx += movelen;
   }
   else
      ret_val = false;

   return( ret_val );

} /* end vEBCDICMove */
/******************************************************************************
 *
 *  NAME:         RspEBCDICvMove
 *
 *  DESCRIPTION:  This function converts a variable length string from EBCDIC,
 *                into ASCII.  It copies it from ReqRsp_Buffer into where ever
 *                the parameter 'fieldid' is pointing, usually into specific
 *                fields in the HOST_MESSAGE data structure.  The length is
 *                also copied.
 *
 *  INPUTS:       fieldid - Field Id to denote the destination of the data
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   False if bit field Id is not defined, else true
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
INT RspEBCDICvMove (enum bitfields fieldid)
{
   struct bitfield_data *pBF;
   INT                  FieldLen;
   INT                  movelen;
   INT                  ret_val = true;
   BYTE                 temp_string_len[4]="";

   /*---------------------------------------------------*/
   /* Find pointer to bitfield info for this bitfield...*/
   /*---------------------------------------------------*/
   pBF = Find_Bitfield_Table(fieldid);
   if (pBF != NULL)
   {
      FieldLen = pBF->bd_fieldlen;

      /* Determine number of bytes to move. */
      convert_to_ascii( &ReqRsp_Buffer[reqidx], temp_string_len, FieldLen );
      movelen = atoi((char *)temp_string_len) + FieldLen;
	/*Temp fix for DE44 length, IST has to revise repsone format, Now it is coming as FIXED 5 byte length,
		Expected is LLVAR- Phani*/
		if(fieldid == bf_44respdata)
			//movelen= 5;
			movelen =26;
      if ( (reqidx + movelen) <= COMM_BUF_SZE )
      {
         /* Move the data, converting it from EBCDIC to ASCII. */
         convert_to_ascii( &ReqRsp_Buffer[reqidx], pBF->p_bd_pdata, movelen );
         reqidx += movelen;
      }
      else
      {
         ret_val = false;
         invalid_msg_format = true;
      }
   }
   else
      ret_val = false;

   return( ret_val );

} /* end RspEBCDICvMove */


INT RspEBCDICvMove_104 (enum bitfields fieldid)
{
   struct bitfield_data *pBF;
   INT                  FieldLen;
   INT                  movelen;
   INT                  ret_val = true;
   BYTE                 temp_string_len[4]="";

   /*---------------------------------------------------*/
   /* Find pointer to bitfield info for this bitfield...*/
   /*---------------------------------------------------*/
   pBF = Find_Bitfield_Table(fieldid);
   if (pBF != NULL)
   {
      FieldLen = pBF->bd_fieldlen;

      /* Determine number of bytes to move. */
      convert_to_ascii( &ReqRsp_Buffer[reqidx], temp_string_len, FieldLen );
      movelen = atoi((char *)temp_string_len);

     // memcpy(pBF->p_bd_pdata, temp_string_len, FieldLen);
      sprintf((char *)pBF->p_bd_pdata,"%03d",movelen);
      reqidx +=FieldLen;
      if ( (reqidx + movelen) <= COMM_BUF_SZE )
      {
         /* Move the data, converting it from EBCDIC to ASCII. */
        // convert_to_ascii( &ReqRsp_Buffer[reqidx], pBF->p_bd_pdata, movelen );

         memcpy(&pBF->p_bd_pdata[3], &ReqRsp_Buffer[reqidx], movelen);
         reqidx += movelen;
      }
      else
      {
         ret_val = false;
         invalid_msg_format = true;
      }
   }
   else
      ret_val = false;

   return( ret_val );

} /* end RspEBCDICvMove */

INT RspEBCDICvMove_44 (enum bitfields fieldid)
{
   struct bitfield_data *pBF;
   INT                  FieldLen;
   INT                  movelen;
   INT                  ret_val = true;
   BYTE                 temp_string_len[4]="";

   /*---------------------------------------------------*/
   /* Find pointer to bitfield info for this bitfield...*/
   /*---------------------------------------------------*/
   pBF = Find_Bitfield_Table(fieldid);
   if (pBF != NULL)
   {
      FieldLen = pBF->bd_fieldlen;

      /* Determine number of bytes to move. */
      convert_to_ascii( &ReqRsp_Buffer[reqidx], temp_string_len, FieldLen );
      movelen = atoi((char *)temp_string_len) + FieldLen;

      if ( (reqidx + movelen) <= COMM_BUF_SZE )
      {
         /* Move the data, converting it from EBCDIC to ASCII. */
    	  /* As length not available store parsed value in data part. hence +2 for pBF->p_bd_pdata */
         convert_to_ascii( &ReqRsp_Buffer[reqidx], pBF->p_bd_pdata, movelen );
         reqidx += movelen;
      }
      else
      {
         ret_val = false;
         invalid_msg_format = true;
      }
   }
   else
      ret_val = false;

   return( ret_val );

} /* end RspEBCDICvMove */
/******************************************************************************
 *
 *  NAME:         RspEBCDICMove
 *
 *  DESCRIPTION:  This function converts a fixed length string from EBCDIC,
 *                into ASCII.  It copies it from ReqRsp_Buffer into where ever
 *                the parameter 'fieldid' is pointing, usually into specific
 *                fields in the HOST_MESSAGE data structure.
 *
 *  INPUTS:       fieldid - Field Id to denote the destination of the data
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   False if bit field Id is not defined, else true
 *
 *  AUTHOR:       Dennis Irby
 *
 *  MODIFICATIONS:
 ******************************************************************************/
INT RspEBCDICMove (enum bitfields fieldid)
{
   struct bitfield_data *pBF;
   INT                  movelen;
   INT                  ret_val = true;
   BYTE asci_data;
   int i=0;
   /*---------------------------------------------------*/
   /* Find pointer to bitfield info for this bitfield...*/
   /*---------------------------------------------------*/
   pBF = Find_Bitfield_Table(fieldid);
   if (pBF != NULL)
   {
      movelen = pBF->bd_fieldlen;

      if ( (reqidx + movelen) <= COMM_BUF_SZE )
      {
         /* Move the data, converting it from EBCDIC to ASCII. */
      //   convert_to_ascii( &ReqRsp_Buffer[reqidx], pBF->p_bd_pdata, movelen );//ascii_ebcdic_conv
		  for(i=0;i<movelen;i++)
		  {
			pBF->p_bd_pdata[i] = ascii_ebcdic_conv(ReqRsp_Buffer[reqidx],E2A);
			reqidx++;
		  }
         //reqidx += movelen;
      }
      else
      {
         ret_val = false;
         invalid_msg_format = true;
      }
   }
   else
      ret_val = false;

   return( ret_val );

} /* end RspEBCDICMove */

/******************************************************************************
 *
 *  NAME:         EBCDICMove
 *
 *  DESCRIPTION:  This function converts a fixed length string into EBCDIC,
 *                then copies it into ReqRsp_Buffer.
 *
 *  INPUTS:       fieldid - Field Id to denote where the source data exists
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   False if bit field Id is not defined, else true
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
INT EBCDICMove( enum bitfields fieldid )
{
   struct bitfield_data *pBF;
   INT                  movelen;
   INT                  ret_val = true;

   /*---------------------------------------------------*/
   /* find pointer to bitfield info for this bitfield...*/
   /*---------------------------------------------------*/
   pBF = Find_Bitfield_Table(fieldid);
   if (pBF != NULL)
   {
      movelen = pBF->bd_fieldlen;

      convert_to_ebcdic( pBF->p_bd_pdata, &ReqRsp_Buffer[reqidx], movelen );
      reqidx += movelen;
   }
   else
      ret_val = false;

   return( ret_val );

} /* end EBCDICMove */

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
INT Move_Never(enum bitfields bitfield, BYTE p_trantype)
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
#define MCRD_EMV_MAX_LEN 255
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
void genutil_bin_to_ascii (BYTE bstring[], INT blen, pCHAR astr, INT astr_len)
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

/*********ParsedMssg() to print the amex parsed message****/
void parsed_mssg()
{
	int bitCount, startCharacter, icnt, LastDisplayBit;
	BYTE temp_data[512]={0};
		char asci_cnvrtd_data[100]={0};
		char buffer [256]={0};
		int len=0;
	memset(Line,0,sizeof(Line));
	memset(&AUTH_TX_V,0x00,sizeof(AUTH_TX_V));
	memset(&EMV_details,0x00,sizeof(EMV_details));
	

	strcpy (Line[0], "000 MsgType				: ");
	strncat(Line[0],(char *)HOST_MESSAGE.msgtype,4);

	if(strlen((char *)HOST_MESSAGE.bitmap1_bin) != 0 && strlen((char *)HOST_MESSAGE.bitmap2_bin) != 0 )
	{
		strcpy (Line[1], "001 BitMap							: ");
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
	LastDisplayBit = 1;						// This is the bit that was displayed last.
	char temp_len[4]={0};						
		strcat (Line[1], "BIT MAP					: ");
	strncat (Line[1],astr_bitmap,strlen(astr_bitmap));
	for (bitCount = 2; bitCount <= 192; bitCount++)
	{
		len=0;
		memset((char *)temp_len,0,3);
		memset((char *)temp_data,0,100);
		memset((char *)buffer,0,100);
		memset((char *)asci_cnvrtd_data,0,100);
		switch(bitCount)
		{	
		case bf_2pan :
			//if(strlen((char *)HOST_MESSAGE.pan_len) != 0)    
			if(BitFieldArray[bf_2pan] == 1)
			{	
				strncpy(temp_len,(char *)HOST_MESSAGE.pan_len,2);
				len=atoi(temp_len);
				strcat (Line[bitCount], FieldStrings [bitCount]);
				strncat (Line[bitCount],(char *)HOST_MESSAGE.pan,len);				
			}	
		break;
		
		case bf_3proc :
			//if(strlen((char *)HOST_MESSAGE.proccode) != 0)
			if(BitFieldArray[bf_3proc] == 1)
			{
				
				strcat (Line[bitCount], FieldStrings [bitCount]);
				strncat (Line[bitCount],(char *)HOST_MESSAGE.proccode, 6);				
			}
		break;
		
		case bf_4amount :
			//if(strlen((char *)HOST_MESSAGE.amount) != 0)
			if(BitFieldArray[bf_4amount] == 1)
			{
				strcat (Line[bitCount], FieldStrings [bitCount]);
				strncat (Line[bitCount],(char *)HOST_MESSAGE.amount, 12);
			}
		break;

		case bf_5settamount :
			//if(strlen((char *)HOST_MESSAGE.setl_amount) != 0)
			if(BitFieldArray[bf_5settamount] == 1)
			{
				strcat (Line[bitCount], FieldStrings [bitCount]);
				strncat (Line[bitCount],(char *)HOST_MESSAGE.setl_amount, 12);
			}
		break;
	
		case bf_7trntime :
			//if(strlen((char *)HOST_MESSAGE.gmt_time) != 0)
			if(BitFieldArray[bf_7trntime] == 1)
			{
				strcat (Line[bitCount], FieldStrings [bitCount]);
				strncat (Line[bitCount], (char *)HOST_MESSAGE.gmt_time,10);
			}
		break;

		case bf_9settconvrate :
			//if(strlen((char *)HOST_MESSAGE.setl_conv_rate) != 0)
			if(BitFieldArray[bf_9settconvrate] == 1)
			{
				strcat (Line[bitCount], FieldStrings [bitCount]);
				strncat (Line[bitCount], (char *)HOST_MESSAGE.setl_conv_rate,8);
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
		
		case bf_12time :
			//if(strlen((char *)HOST_MESSAGE.time) != 0)
			if(BitFieldArray[bf_12time] == 1)
			{
				strcat (Line[bitCount], FieldStrings [bitCount]);
				strncat (Line[bitCount],(char *)HOST_MESSAGE.time, 6);
			}
		break ;

		case bf_13date :
			//if(strlen((char *)HOST_MESSAGE.date) != 0)
			if(BitFieldArray[bf_13date] == 1)
			{
				strcat (Line[bitCount], FieldStrings [bitCount]);
				strncat (Line[bitCount],(char *) HOST_MESSAGE.date, 4);
			}
		break ;

		case bf_14ExpDate :
			//if(strlen((char *)HOST_MESSAGE.exp_date) != 0)
			if(BitFieldArray[bf_14ExpDate] == 1)
			{
				strcat (Line[bitCount], FieldStrings [bitCount]);
				strncat (Line[bitCount], (char *)HOST_MESSAGE.exp_date, 4);
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
		
		case bf_18mertype :
			//if(strlen((char *)HOST_MESSAGE.merch_type) != 0)
			if(BitFieldArray[bf_18mertype] == 1)
			{
				strcat (Line[bitCount], FieldStrings [bitCount]);
				strncat (Line[bitCount], (char *)HOST_MESSAGE.merch_type, 4);
			}
		break ;
		
		case bf_19acqinstcode :
			//if(strlen((char *)HOST_MESSAGE.acq_inst_cnt_code) != 0)
			if(BitFieldArray[bf_19acqinstcode] == 1)
			{
				strcat (Line[bitCount], FieldStrings [bitCount]);
				strncat (Line[bitCount], (char *)HOST_MESSAGE.acq_inst_cnt_code, 4);
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
				strncat (Line[bitCount], (char *)HOST_MESSAGE.posentry, 4);
			}
		break ;
		
		case bf_23cardseqnum :
			//if(strlen((char *)HOST_MESSAGE.cardseqnum) != 0)
			if(BitFieldArray[bf_23cardseqnum] == 1)
			{
				strcat (Line[bitCount], FieldStrings [bitCount]);
				strncat (Line[bitCount],(char *)HOST_MESSAGE.cardseqnum, 3);
			}
		break ;

		case bf_25poscc :
			//if(strlen((char *)HOST_MESSAGE.pos_condcd) != 0)
			if(BitFieldArray[bf_25poscc] == 1)
			{
				strcat (Line[bitCount], FieldStrings [bitCount]);
				strncat (Line[bitCount], (char *)HOST_MESSAGE.pos_condcd, 2);
			}
		break ;
		
		case bf_28tranfeeamt :
			//if(strlen((char *)HOST_MESSAGE.tran_fee_amt) != 0)
			if(BitFieldArray[bf_28tranfeeamt] == 1)
			{
					strcat (Line[bitCount], FieldStrings [bitCount]);
					strncat (Line[bitCount], (char *)HOST_MESSAGE.tran_fee_amt , 9);
			}
		break ;
		
		case bf_32idcode :
			//if(strlen((char *)HOST_MESSAGE.acq_inst_len) != 0)
			if(BitFieldArray[bf_32idcode] == 1)
			{
				strncpy(temp_len,(char *)HOST_MESSAGE.acq_inst_len,2);
				len=atoi(temp_len);
					strcat (Line[bitCount], FieldStrings [bitCount]);
					strncat (Line[bitCount], (char *)HOST_MESSAGE.acq_inst_id,len);
			}
		break ;
		
		case bf_33fwdinstidcode :
			//if(strlen((char *)HOST_MESSAGE.fwd_inst_len) != 0)
			if(BitFieldArray[bf_33fwdinstidcode] == 1)
			{
				strncpy(temp_len,(char *)HOST_MESSAGE.fwd_inst_len,2);
				len=atoi(temp_len);
					strcat (Line[bitCount], FieldStrings [bitCount]);
					strncat (Line[bitCount], (char *)HOST_MESSAGE.fwd_inst_id ,len);
			}
		break ;

		case bf_35track2 :
			//if(strlen((char *)HOST_MESSAGE.track2_len) != 0)
			if(BitFieldArray[bf_35track2] == 1)
			{
				strncpy(temp_len,(char *)HOST_MESSAGE.track2_len,2);
				len=atoi(temp_len);
					strcat (Line[bitCount], FieldStrings [bitCount]);
					strncat (Line[bitCount], (char *)HOST_MESSAGE.track2,len);
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
					strncat (Line[bitCount], (char *)HOST_MESSAGE.responsecd,2);
			}
		break ;
		
		case bf_41termid :
			if(BitFieldArray[bf_41termid] == 1){
			memcpy(asci_cnvrtd_data,(char *)HOST_MESSAGE.termid,8);
			if(strlen(asci_cnvrtd_data) != 0)
			{
					strcat (Line[bitCount], FieldStrings [bitCount]);
					strncat (Line[bitCount], (char *)asci_cnvrtd_data,8);
			}
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
			//if(strlen((char *)HOST_MESSAGE.cardaccname) != 0)
			if(BitFieldArray[bf_43cardacceptloc] == 1)
			{
					strcat (Line[bitCount], FieldStrings [bitCount]);
					strncat (Line[bitCount], (char *)HOST_MESSAGE.cardaccname,40);
			}
		break ;
		// here i am printing 5 bytes bcz while cnvrtng they have fixed this for 5 byte..
		// even though it have length part as 1 byte ...
		case bf_44respdata :
			//if(strlen((char *)HOST_MESSAGE.addl_rsp_len) != 0)	
			if(BitFieldArray[bf_44respdata] == 1)
			{
				strncpy((char *)temp_data,(char *)HOST_MESSAGE.addl_rsp_len,2);
				strncat((char *)temp_data,(char *)HOST_MESSAGE.addlrspdata,3);
				//len=atoi(temp_len);
				strcat (Line[bitCount], FieldStrings [bitCount]);
				strncat (Line[bitCount], (char *)temp_data,5);
			}
		break ;
		
		case bf_48addldata :
			//if(strlen((char *)HOST_MESSAGE.addl_data_len) != 0)
			if(BitFieldArray[bf_48addldata] == 1)
			{
				strncpy(temp_len,(char *)HOST_MESSAGE.addl_data_len,3);
				len=atoi(temp_len);
				if(0 == strncmp((char*)HOST_MESSAGE.msgtype,"0800",4))
				{
					memcpy(temp_data,HOST_MESSAGE.addl_data+ 2  , (len -2));
					genutil_bin_to_ascii (temp_data, len,buffer, (len*2));
					strcat (Line[bitCount], FieldStrings [bitCount]);
					strncat(Line[bitCount],"NK",2);
					strncat (Line[bitCount], (char *)buffer,strlen(buffer));
				}
				else
				{
					strcat (Line[bitCount], FieldStrings [bitCount]);
					strncat (Line[bitCount], (char *)HOST_MESSAGE.addl_data,len);
				}
			}
		break ;
		
		case bf_49trncurcd :
			//if(strlen((char *)HOST_MESSAGE.tran_curr_cd) != 0)		
			if(BitFieldArray[bf_49trncurcd] == 1)
			{
					strcat (Line[bitCount], FieldStrings [bitCount]);
					strncat (Line[bitCount], (char *)HOST_MESSAGE.tran_curr_cd,3);
			}
		break ;

		case bf_52pinblock :
			//if(strlen((char *)HOST_MESSAGE.pinblock) != 0) 
			if(BitFieldArray[bf_52pinblock] == 1)
			{	
					genutil_bin_to_ascii (HOST_MESSAGE.pinblock, 16,buffer, (len*2));
					strcat (Line[bitCount], FieldStrings [bitCount]);
					strncat (Line[bitCount], (char *)buffer,strlen(buffer));
			}
		break ;

		case bf_55ICCdata :
			//if(strlen((char *)HOST_MESSAGE.ICCdata_len) != 0)		
			if(BitFieldArray[bf_55ICCdata] == 1)
			{
					genutil_bin_to_ascii (HOST_MESSAGE.ICCdata_len, 1,temp_len, (len*2));
					len = genutil_hex_to_int(temp_len,strlen(temp_len));
					memset(iccdata,0x00,sizeof(iccdata));
					memcpy((char *)iccdata,(char *)HOST_MESSAGE.ICCdata,len);
					genutil_VISA_EMV_To_Auth_Tx( iccdata, len,&AUTH_TX_V );
					memcpy(&EMV_details,&(AUTH_TX_V.EMV_details),sizeof(EMV_details));
					Generate_EMV_Details();
					strcat (Line[bitCount], FieldStrings [bitCount]);
					strncat (Line[bitCount], (char *)iccdata,strlen((char *)iccdata));
			}
		break ;
		
		case bf_60priv60 :
			//if(strlen((char *)HOST_MESSAGE.priv60) != 0)		
			if(BitFieldArray[bf_60priv60] == 1)
			{
					strcat (Line[bitCount], FieldStrings [bitCount]);
					strncat (Line[bitCount], (char *)HOST_MESSAGE.priv60,12);
			}
		break ;
		
		case bf_61priv61 :
			if(BitFieldArray[bf_61priv61] == 1){
			convert_to_ascii(HOST_MESSAGE.priv61_len, (BYTE *)temp_len, 3 );
					len=atoi(temp_len);	
			if(len != 0)			
			{
					strcat (Line[bitCount], FieldStrings [bitCount]);
					strncat (Line[bitCount], (char *)HOST_MESSAGE.priv61,len);
			}
			}
		break ;
		
		case bf_62priv62 :
			
			//if(strlen((char *)HOST_MESSAGE.priv62_len) != 0)
			if(BitFieldArray[bf_62priv62] == 1)
			{	
					strncpy(temp_len,(char *)HOST_MESSAGE.priv62_len,3);
					len=atoi(temp_len);
					strcat (Line[bitCount], FieldStrings [bitCount]);
					strncat (Line[bitCount],(char *) HOST_MESSAGE.priv62,len);
			}
		break ;
		
		case bf_63priv63 :
			//if(strlen((char *)HOST_MESSAGE.priv63_len) != 0)		
			if(BitFieldArray[bf_63priv63] == 1)
			{
					strncpy(temp_len,(char *)HOST_MESSAGE.priv63_len,2);
					len=atoi(temp_len);
					strcat (Line[bitCount], FieldStrings [bitCount]);
					strncat (Line[bitCount], (char *)HOST_MESSAGE.priv63,len);
			}
		break ;			
	
		case bf_70nmicode :
			//if(strlen((char *)HOST_MESSAGE.nmi_code) != 0)	
			if(BitFieldArray[bf_70nmicode] == 1)
			{
					strcat (Line[bitCount], FieldStrings [bitCount]);
					strncat (Line[bitCount], (char *)HOST_MESSAGE.nmi_code,3);
			}
		break ;

		case bf_90orgdata :
			//if(strlen((char *)HOST_MESSAGE.orig_data) != 0)	
			if(BitFieldArray[bf_90orgdata] == 1)
			{
					memset((char *)temp_data, 0x00, sizeof(temp_data));
					/*strncat((char *)temp_data,(char *)auth_tx.TLF01_details.orig_message,4);
					strncat((char *)temp_data,(char *)auth_tx.TLF01_details.orig_sys_trace_num,6);
					strncat((char *)temp_data,(char *)auth_tx.orig_local_time_date,10);
					strncat((char *)temp_data,(char *)auth_tx.orig_acq_inst_id,11);
					strncat((char *)temp_data,(char *)auth_tx.amex_pos_entry_mode,11);*/
					strncat((char *)temp_data, (char *)HOST_MESSAGE.orig_data, 42);
					strcat (Line[bitCount], FieldStrings [bitCount]);
					strncat (Line[bitCount], (char *)temp_data,42);
			}
		break ;

		case bf_95repl_amt :
			//if(strlen((char *)HOST_MESSAGE.replace_amt) != 0)	
			if(BitFieldArray[bf_95repl_amt] == 1)
			{
					strcat (Line[bitCount], FieldStrings [bitCount]);
					strncat (Line[bitCount], (char *)HOST_MESSAGE.replace_amt,42);
			}
		break ;

		case bf_100rcvid :
			//if(strlen((char *)HOST_MESSAGE.rcvid_len) != 0)	
			if(BitFieldArray[bf_100rcvid] == 1)
			{
					genutil_bin_to_ascii (HOST_MESSAGE.rcvid_len, 1,temp_len, 3);
					len = genutil_hex_to_int(temp_len,strlen(temp_len));
					bcd_to_asc( HOST_MESSAGE.rcvid, len,(char *)temp_data,strlen((char *)temp_data));
					strcat (Line[bitCount], FieldStrings [bitCount]);
					strncat (Line[bitCount], (char *)temp_data,len);
			}
		break ;

		case bf_102acct1 :
			//if(strlen((char *)HOST_MESSAGE.acct1_len) != 0)	
			if(BitFieldArray[bf_102acct1] == 1)
			{
					strncpy(temp_len,(char *)HOST_MESSAGE.acct1_len,2);
					len=atoi(temp_len);
					strcat (Line[bitCount], FieldStrings [bitCount]);
					strncat (Line[bitCount], (char *)HOST_MESSAGE.acct1,len);
			}
		break ;

		case bf_103acct2 :
			//if(strlen((char *)HOST_MESSAGE.acct2_len) != 0)		
			if(BitFieldArray[bf_103acct2] == 1)
			{
					strncpy(temp_len,(char *)HOST_MESSAGE.acct2_len,2);
					len=atoi(temp_len);
					strcat (Line[bitCount], FieldStrings [bitCount]);
					strncat (Line[bitCount], (char *)HOST_MESSAGE.acct2,len);
			}
		break ;

		case bf_104trandesc :
			//if(strlen((char *)HOST_MESSAGE.trandesc_len) != 0)	
			if(BitFieldArray[bf_104trandesc] == 1)
			{
					memset((char *)temp_data, 0x00, sizeof(temp_data));
					strncpy(temp_len,(char *)HOST_MESSAGE.trandesc_len,3);
					len = atoi((char *)temp_len);
					genutil_bin_to_ascii (HOST_MESSAGE.trandesc, len,(char *)temp_data, sizeof(temp_data));
					strcat (Line[bitCount], FieldStrings [bitCount]);
					strncat (Line[bitCount], (char *)temp_data,len*2);
			}
		break ;

		case bf_115addl_trace :
			//if(strlen((char *)HOST_MESSAGE.addl_trace_len) != 0)		
			if(BitFieldArray[bf_115addl_trace] == 1)
			{
					genutil_bin_to_ascii (HOST_MESSAGE.addl_trace_len, 1,temp_len, 3);
					len =atoi(temp_len);
					strcat (Line[bitCount], FieldStrings [bitCount]);
					strncat (Line[bitCount], (char *)HOST_MESSAGE.addl_trace,len);
			}
		break ;

		case bf_126priv126 :
			//if(strlen((char *)HOST_MESSAGE.de126_len) != 0)	
			if(BitFieldArray[bf_126priv126] == 1)
			{
					strncpy(temp_len,(char *)HOST_MESSAGE.de126_len,3);
					len = atoi((char *)temp_len);
					memcpy((char *)auth_tx.TLF01_details.voice_auth_text, HOST_MESSAGE.de126, len);
					strcat (Line[bitCount], FieldStrings [bitCount]);
					strncat (Line[bitCount], (char *)auth_tx.TLF01_details.voice_auth_text,strlen((char *)auth_tx.TLF01_details.voice_auth_text));
			}
		break ;

		case bf_128msg_authc :
			//if(strlen((char *)HOST_MESSAGE.msg_authc) != 0)
			if(BitFieldArray[bf_128msg_authc] == 1)
			{
					strcat (Line[bitCount], FieldStrings [bitCount]);
					strncat (Line[bitCount], (char *)HOST_MESSAGE.msg_authc,len);
			}
		break ;

	}

		
		startCharacter++;
		LastDisplayBit = bitCount;
	}

		if(strlen((char *)HOST_MESSAGE.ICCdata_len) != 0)
		{
			strcat (Line[bitCount++], "------------------------------------------------------------------------------------------------------------------");
			
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
		  sprintf(Line[bitCount++],  "91 	Issuer_auth_data	:	%s",(char*)EMV_details.issuer_auth_data); 
		  
		}
	

}


/***********************************************************************************************************/

void Cex_amex_parserDlg::OnBnClickedParse()
{
	// TODO: Add your control notification handler code here
	CString sz = "";
	CString temp;
//	BYTE buf[10048];
	LPBYTE temp_string;
//	LPBYTE temp_string2;
	int j = 0;
	int len = 0;
	UpdateData(); //transafers the data from controls to variables.
	char errbuf[256]={0},nmi_code[5]={0};
	temp = m_flowfile;
	if(strlen(temp) == 0)
	{
		m_parsedmssg.Empty();
		exportbtn.EnableWindow(FALSE);
	}
	else
	{
		exportbtn.EnableWindow(TRUE);
		//memset(temp_string,0x00,sizeof(temp_string));
		len = m_flowfile.GetLength();
		temp_string = (LPBYTE)m_flowfile.GetBuffer(len);
		ascii_to_bin(temp_string, ReqRsp_Buffer, len);
		m_flowfile.ReleaseBuffer(len);
		
		if ( parse_host_msg(errbuf) == 0)
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
// Raghu
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

/**************************************/
void genutil_VISA_EMV_To_Auth_Tx( pCHAR emv_raw, INT total_len, pAUTH_TX p_auth_tx )
{
   INT    fieldLength;
   INT    dataIndex;
   INT    bTwoBytesTag;
   BYTE   byteOneOfTag;
   BYTE   byteTwoOfTag;
   BYTE   b_string[4]  = {0};
   CHAR   a_string[4]  = {0};
   CHAR   tempstr[500] = {0};


   dataIndex   = 0;
   if(emv_raw[dataIndex] == 0x01)
   {
		dataIndex=dataIndex+3;
   }
   else if(emv_raw[dataIndex] == 0x00)
   {
	   dataIndex=dataIndex+3;
   }

   while (dataIndex <= total_len)
   {
      bTwoBytesTag = 0;
      byteOneOfTag = emv_raw[dataIndex];
      dataIndex++;
      if ((byteOneOfTag & 0x1F) == 0x1F)
      {
         bTwoBytesTag = 1;
         byteTwoOfTag = emv_raw[dataIndex];
         dataIndex++;
      }
      fieldLength = emv_raw[dataIndex];
      dataIndex++;

      if (bTwoBytesTag)
      {
         if (byteOneOfTag == 0x5F)
         {
            if (byteTwoOfTag == 0x2A) 
            {
               genutil_bin_to_ascii ((BYTE *)&emv_raw[dataIndex], fieldLength , &tempstr[0], sizeof(tempstr));
               memcpy(p_auth_tx->EMV_details.currency_code, tempstr, sizeof(p_auth_tx->EMV_details.currency_code)-1);
               dataIndex += fieldLength;
            }
            else if (byteTwoOfTag == 0x34) 
            {
               genutil_bcd_to_asc ((BYTE *)&emv_raw[dataIndex], fieldLength , &tempstr[0], sizeof(tempstr));	
               memcpy(p_auth_tx->EMV_details.pan_sequence_number, tempstr, sizeof(p_auth_tx->EMV_details.pan_sequence_number)-1);
               dataIndex += fieldLength;
            }
         }
         else if (byteOneOfTag == 0x9F)
         {
            switch (byteTwoOfTag)
            {
               case 0x02: 
                        genutil_bcd_to_asc ((BYTE *)&emv_raw[dataIndex], fieldLength , &tempstr[0], sizeof(tempstr));
                        memcpy(p_auth_tx->EMV_details.tran_amount, tempstr, sizeof(p_auth_tx->EMV_details.tran_amount)-1);
                        break;
               case 0x03:
                        genutil_bcd_to_asc ((BYTE *)&emv_raw[dataIndex], fieldLength , &tempstr[0], sizeof(tempstr));
                        memcpy(p_auth_tx->EMV_details.other_amount, tempstr, sizeof(p_auth_tx->EMV_details.other_amount)-1);
                        break;
               case 0x09: 
                        genutil_bin_to_ascii ((BYTE *)&emv_raw[dataIndex], fieldLength , &tempstr[0], sizeof(tempstr));
                        memcpy(p_auth_tx->EMV_details.version, tempstr, sizeof(p_auth_tx->EMV_details.version)-1);
                        break;
               case 0x10: 
                        genutil_bin_to_ascii ((BYTE *)&emv_raw[dataIndex], fieldLength , &tempstr[0], sizeof(tempstr));
                        memcpy(p_auth_tx->EMV_details.iad, tempstr, sizeof(p_auth_tx->EMV_details.iad)-1);
                        break;
               case 0x1A: 
                        genutil_bcd_to_asc ((BYTE *)&emv_raw[dataIndex], fieldLength , &tempstr[0], sizeof(tempstr));
                        memcpy(p_auth_tx->EMV_details.country_code, tempstr+1, sizeof(p_auth_tx->EMV_details.country_code)-1);
                        break;
               case 0x15:
                        genutil_bcd_to_asc ((BYTE *)&emv_raw[dataIndex], fieldLength , &tempstr[0], sizeof(tempstr));
                        memcpy(p_auth_tx->EMV_details.mcc, tempstr, sizeof(p_auth_tx->EMV_details.mcc)-1);
                        break;
               case 0x1E:
                        memcpy(p_auth_tx->EMV_details.serial_number, &emv_raw[dataIndex], fieldLength);
                        break;
               case 0x26: 
                        genutil_bin_to_ascii ((BYTE *)&emv_raw[dataIndex], fieldLength , &tempstr[0], sizeof(tempstr));
                        memcpy(p_auth_tx->EMV_details.app_crypto, tempstr, sizeof(p_auth_tx->EMV_details.app_crypto)-1);
                        break;
               case 0x27: 
                        genutil_bin_to_ascii ((BYTE *)&emv_raw[dataIndex], fieldLength , &tempstr[0], sizeof(tempstr));
                        memcpy(p_auth_tx->EMV_details.crypto_info, tempstr, sizeof(p_auth_tx->EMV_details.crypto_info)-1);
                        break;
               case 0x33:
                        genutil_bin_to_ascii ((BYTE *)&emv_raw[dataIndex], fieldLength , &tempstr[0], sizeof(tempstr));
                        memcpy(p_auth_tx->EMV_details.terminal_capabilities, tempstr, sizeof(p_auth_tx->EMV_details.terminal_capabilities)-1);
                        break;
               case 0x34:
                        genutil_bin_to_ascii ((BYTE *)&emv_raw[dataIndex], fieldLength , &tempstr[0], sizeof(tempstr));
                        memcpy(p_auth_tx->EMV_details.cvm_results, tempstr, sizeof(p_auth_tx->EMV_details.cvm_results)-1);
                        break;
               case 0x35:
                        genutil_bcd_to_asc ((BYTE *)&emv_raw[dataIndex], fieldLength , &tempstr[0], sizeof(tempstr));
                        memcpy(p_auth_tx->EMV_details.terminal_type, tempstr, sizeof(p_auth_tx->EMV_details.terminal_type)-1);
                        break;
               case 0x36:
                        genutil_bin_to_ascii ((BYTE *)&emv_raw[dataIndex], fieldLength , &tempstr[0], sizeof(tempstr));
                        memcpy(p_auth_tx->EMV_details.atc, tempstr, sizeof(p_auth_tx->EMV_details.atc)-1);
                        break;
               case 0x37:
                        genutil_bin_to_ascii ((BYTE *)&emv_raw[dataIndex], fieldLength , &tempstr[0], sizeof(tempstr));
                        memcpy(p_auth_tx->EMV_details.unpredictable_number, tempstr, sizeof(p_auth_tx->EMV_details.unpredictable_number)-1);
                        break;
               case 0x41:
                        genutil_bcd_to_asc ((BYTE *)&emv_raw[dataIndex], fieldLength , &tempstr[0], sizeof(tempstr));
                        memcpy(p_auth_tx->EMV_details.tran_sequence_ctr, tempstr, sizeof(p_auth_tx->EMV_details.tran_sequence_ctr)-1);
                        break;
               case 0x53:
                        memcpy(p_auth_tx->EMV_details.tcc, &emv_raw[dataIndex], fieldLength);
                        break;
               default:
                        break;
            }
            dataIndex += fieldLength;
         }
      }
      else
      {
         switch (byteOneOfTag)
         {
            case 0x71:
                     /* Get overall length of this field - including tag. */
                     memset( a_string, 0x00, sizeof(a_string) );
                     memset( b_string, 0x00, sizeof(b_string) );
                     b_string[0] = fieldLength+2;
                     genutil_bin_to_ascii( b_string, 1, a_string, sizeof(a_string) );
                     memcpy( p_auth_tx->EMV_details.issuer_script_1, a_string, 2 );

                     /* Get Tag ID */
                     memcpy(p_auth_tx->EMV_details.issuer_script_1+2, "71", 2 ); /* Tag ID */

                     /* Get length of data for this tag. */
                     memset( a_string, 0x00, sizeof(a_string) );
                     memset( b_string, 0x00, sizeof(b_string) );
                     b_string[0] = fieldLength;
                     genutil_bin_to_ascii( b_string, 1, a_string, sizeof(a_string) );
                     memcpy( p_auth_tx->EMV_details.issuer_script_1+4, a_string, 2 );

                     /* Finally put in the data. */
                     genutil_bin_to_ascii ((BYTE *)&emv_raw[dataIndex], fieldLength , &tempstr[0], sizeof(tempstr));
                     memcpy(p_auth_tx->EMV_details.issuer_script_1+6, tempstr, sizeof(p_auth_tx->EMV_details.issuer_script_1)-7);

                     break;
            case 0x82:
                     genutil_bin_to_ascii ((BYTE *)&emv_raw[dataIndex], fieldLength , &tempstr[0], sizeof(tempstr));
                     memcpy(p_auth_tx->EMV_details.interchange_profile, tempstr, sizeof(p_auth_tx->EMV_details.interchange_profile)-1);
                     break;
            case 0x84:
                     genutil_bin_to_ascii ((BYTE *)&emv_raw[dataIndex], fieldLength , &tempstr[0], sizeof(tempstr));
                     memcpy(p_auth_tx->EMV_details.app_id, tempstr, sizeof(p_auth_tx->EMV_details.app_id)-1);
                     break;
            case 0x91:
                     genutil_bin_to_ascii ((BYTE *)&emv_raw[dataIndex], fieldLength , &tempstr[0], sizeof(tempstr));
                     memcpy(p_auth_tx->EMV_details.issuer_auth_data+1, tempstr, sizeof(p_auth_tx->EMV_details.issuer_auth_data)-2);
                     p_auth_tx->EMV_details.issuer_auth_data[0] = fieldLength;
                     break;
            case 0x95:
                     genutil_bin_to_ascii ((BYTE *)&emv_raw[dataIndex], fieldLength , &tempstr[0], sizeof(tempstr));
                     memcpy(p_auth_tx->EMV_details.tvr, tempstr, sizeof(p_auth_tx->EMV_details.tvr)-1);
                     break;
            case 0x9A:
                     genutil_bcd_to_asc ((BYTE *)&emv_raw[dataIndex], fieldLength , &tempstr[0], sizeof(tempstr));
                     memcpy(p_auth_tx->EMV_details.tran_date, tempstr, sizeof(p_auth_tx->EMV_details.tran_date)-1);
                     break;
            case 0x9C:
                     genutil_bcd_to_asc ((BYTE *)&emv_raw[dataIndex], fieldLength , &tempstr[0], sizeof(tempstr));
                     memcpy(p_auth_tx->EMV_details.tran_type, tempstr, sizeof(p_auth_tx->EMV_details.tran_type)-1);
                     break;
            default:
                     break;
         }
         dataIndex += fieldLength;
      }
   }
   return;
}
/*****************************************************************************

  Function:   genutil_bcd_to_asc

  Description:  
      convert bcd/binary string to ascii string
  Author: 
      unknown
  Inputs:
      bstring  - incoming bcd string
      blen     - length of bstring
      astr_len - length of astr
  Outputs:
      astr     - outgoing ascii character string
  Return values:
      None
  Modified by:
      BGable      6/26/98
******************************************************************************/
void genutil_bcd_to_asc (BYTE bstring[], INT blen, pCHAR astr, INT astr_len)
{
   INT  i = 0;

   memset (astr, 0x00, astr_len);

   for (i = 0; i < (blen * 2); i=i+2)
      {
      astr[i] = (bstring[i/2] / 16) + '0';
      astr[i+1] = (bstring[i/2] & 0x0F) + '0';
      }
} /* genutil_bcd_to_asc */

/*****************************************************************************

  Function:    blank_or_null_string

  Description: This function will determine if a string is blank or contains
               only nulls
  Author: 
      E. Gable  3/7/99
  Inputs:
      astr - the string to test
      max_len - the length of the string to test.
  Outputs:
      None
  Return values:
      true - the string is blank
      false - the string is not blank
  Modified by:
******************************************************************************/
BOOLEAN blank_or_null_string(pCHAR astr, INT max_len)
{
   int i;
   for(i = 0;i < max_len; i++)
   {
      if ((astr[i] != ' ') && (astr[i] != '\0'))
         return(false);
   }
   return(true);    
}

/*****************************************************************************
 *
 *  Function:    GET_BALANCES
 *
 *  Description: This function copies balance information from a balance
 *               structure into auth_tx for current and available balances.
 *
 * Author:       D. Irby
 *
 * Inputs:       p_acct_bal - Structure containing account balances.
 *
 * Outputs:      p_auth_tx  - Transaction structure
 *
 * Return values: None
 *
 ******************************************************************************/
void get_balances( pAddl_Amts p_acct_bal, pAUTH_TX p_auth_tx )
{
   /* Currency Code */
   memcpy( p_auth_tx->TLF01_details.currency_code, p_acct_bal->cur_code, 3 );

   if ( p_acct_bal->amt_type[1] == '1' )
   {
      /* --------------- */
      /* CURRENT BALANCE */
      /* --------------- */
      memcpy( p_auth_tx->ACF01_details.cash_available,
              p_acct_bal->amount+3, 9 );

      /* Sign */
      if ( p_acct_bal->sign == 'C' )
         p_auth_tx->current_balance_sign = '+';
      else
         p_auth_tx->current_balance_sign = '-';
   }
   else
   {
      /* ----------------- */
      /* AVAILABLE BALANCE */
      /* ----------------- */
      memcpy( p_auth_tx->ACF01_details.cash_limit,
              p_acct_bal->amount+3, 9 );

      /* Sign */
      if ( p_acct_bal->sign == 'C' )
         p_auth_tx->available_balance_sign = '+';
      else
         p_auth_tx->available_balance_sign = '-';
   }
   return;
}
void genutil_VISA_EMV_To_Auth_Tx( BYTE * emv_raw, INT total_len, pAUTH_TX p_auth_tx )
{
   INT    fieldLength;
   INT    dataIndex;
   INT    bTwoBytesTag;
   BYTE   byteOneOfTag;
   BYTE   byteTwoOfTag;
   BYTE   b_string[4]  = {0};
   CHAR   a_string[4]  = {0};
   CHAR   tempstr[500] = {0};


   dataIndex   = 0;
   if(emv_raw[dataIndex] == 0x01)
   {
		dataIndex=dataIndex+3;
   }
   else if(emv_raw[dataIndex] == 0x00)
   {
	   dataIndex=dataIndex+3;
   }

   while (dataIndex <= total_len)
   {
      bTwoBytesTag = 0;
      byteOneOfTag = emv_raw[dataIndex];
      dataIndex++;
      if ((byteOneOfTag & 0x1F) == 0x1F)
      {
         bTwoBytesTag = 1;
         byteTwoOfTag = emv_raw[dataIndex];
         dataIndex++;
      }
      fieldLength = emv_raw[dataIndex];
      dataIndex++;

      if (bTwoBytesTag)
      {
         if (byteOneOfTag == 0x5F)
         {
            if (byteTwoOfTag == 0x2A) 
            {
               genutil_bin_to_ascii (&emv_raw[dataIndex], fieldLength , &tempstr[0], sizeof(tempstr));
               memcpy(p_auth_tx->EMV_details.currency_code, tempstr, sizeof(p_auth_tx->EMV_details.currency_code)-1);
               dataIndex += fieldLength;
            }
            else if (byteTwoOfTag == 0x34) 
            {
               genutil_bcd_to_asc (&emv_raw[dataIndex], fieldLength , &tempstr[0], sizeof(tempstr));	
               memcpy(p_auth_tx->EMV_details.pan_sequence_number, tempstr, sizeof(p_auth_tx->EMV_details.pan_sequence_number)-1);
               dataIndex += fieldLength;
            }
         }
         else if (byteOneOfTag == 0x9F)
         {
            switch (byteTwoOfTag)
            {
               case 0x02: 
                        genutil_bcd_to_asc (&emv_raw[dataIndex], fieldLength , &tempstr[0], sizeof(tempstr));
                        memcpy(p_auth_tx->EMV_details.tran_amount, tempstr, sizeof(p_auth_tx->EMV_details.tran_amount)-1);
                        break;
               case 0x03:
                        genutil_bcd_to_asc ((BYTE *)&emv_raw[dataIndex], fieldLength , &tempstr[0], sizeof(tempstr));
                        memcpy(p_auth_tx->EMV_details.other_amount, tempstr, sizeof(p_auth_tx->EMV_details.other_amount)-1);
                        break;
               case 0x09: 
                        genutil_bin_to_ascii (&emv_raw[dataIndex], fieldLength , &tempstr[0], sizeof(tempstr));
                        memcpy(p_auth_tx->EMV_details.version, tempstr, sizeof(p_auth_tx->EMV_details.version)-1);
                        break;
               case 0x10: 
                        genutil_bin_to_ascii (&emv_raw[dataIndex], fieldLength , &tempstr[0], sizeof(tempstr));
                        memcpy(p_auth_tx->EMV_details.iad, tempstr, sizeof(p_auth_tx->EMV_details.iad)-1);
                        break;
               case 0x1A: 
                        genutil_bcd_to_asc (&emv_raw[dataIndex], fieldLength , &tempstr[0], sizeof(tempstr));
                        memcpy(p_auth_tx->EMV_details.country_code, tempstr+1, sizeof(p_auth_tx->EMV_details.country_code)-1);
                        break;
               case 0x15:
                        genutil_bcd_to_asc (&emv_raw[dataIndex], fieldLength , &tempstr[0], sizeof(tempstr));
                        memcpy(p_auth_tx->EMV_details.mcc, tempstr, sizeof(p_auth_tx->EMV_details.mcc)-1);
                        break;
               case 0x1E:
                        memcpy(p_auth_tx->EMV_details.serial_number, &emv_raw[dataIndex], fieldLength);
                        break;
               case 0x26: 
                        genutil_bin_to_ascii (&emv_raw[dataIndex], fieldLength , &tempstr[0], sizeof(tempstr));
                        memcpy(p_auth_tx->EMV_details.app_crypto, tempstr, sizeof(p_auth_tx->EMV_details.app_crypto)-1);
                        break;
               case 0x27: 
                        genutil_bin_to_ascii (&emv_raw[dataIndex], fieldLength , &tempstr[0], sizeof(tempstr));
                        memcpy(p_auth_tx->EMV_details.crypto_info, tempstr, sizeof(p_auth_tx->EMV_details.crypto_info)-1);
                        break;
               case 0x33:
                        genutil_bin_to_ascii (&emv_raw[dataIndex], fieldLength , &tempstr[0], sizeof(tempstr));
                        memcpy(p_auth_tx->EMV_details.terminal_capabilities, tempstr, sizeof(p_auth_tx->EMV_details.terminal_capabilities)-1);
                        break;
               case 0x34:
                        genutil_bin_to_ascii (&emv_raw[dataIndex], fieldLength , &tempstr[0], sizeof(tempstr));
                        memcpy(p_auth_tx->EMV_details.cvm_results, tempstr, sizeof(p_auth_tx->EMV_details.cvm_results)-1);
                        break;
               case 0x35:
                        genutil_bcd_to_asc (&emv_raw[dataIndex], fieldLength , &tempstr[0], sizeof(tempstr));
                        memcpy(p_auth_tx->EMV_details.terminal_type, tempstr, sizeof(p_auth_tx->EMV_details.terminal_type)-1);
                        break;
               case 0x36:
                        genutil_bin_to_ascii (&emv_raw[dataIndex], fieldLength , &tempstr[0], sizeof(tempstr));
                        memcpy(p_auth_tx->EMV_details.atc, tempstr, sizeof(p_auth_tx->EMV_details.atc)-1);
                        break;
               case 0x37:
                        genutil_bin_to_ascii (&emv_raw[dataIndex], fieldLength , &tempstr[0], sizeof(tempstr));
                        memcpy(p_auth_tx->EMV_details.unpredictable_number, tempstr, sizeof(p_auth_tx->EMV_details.unpredictable_number)-1);
                        break;
               case 0x41:
                        genutil_bcd_to_asc (&emv_raw[dataIndex], fieldLength , &tempstr[0], sizeof(tempstr));
                        memcpy(p_auth_tx->EMV_details.tran_sequence_ctr, tempstr, sizeof(p_auth_tx->EMV_details.tran_sequence_ctr)-1);
                        break;
               case 0x53:
                        memcpy(p_auth_tx->EMV_details.tcc, &emv_raw[dataIndex], fieldLength);
                        break;
               default:
                        break;
            }
            dataIndex += fieldLength;
         }
      }
      else
      {
         switch (byteOneOfTag)
         {
            case 0x71:
                     /* Get overall length of this field - including tag. */
                     memset( a_string, 0x00, sizeof(a_string) );
                     memset( b_string, 0x00, sizeof(b_string) );
                     b_string[0] = fieldLength+2;
                     genutil_bin_to_ascii( b_string, 1, a_string, sizeof(a_string) );
                     memcpy( p_auth_tx->EMV_details.issuer_script_1, a_string, 2 );

                     /* Get Tag ID */
                     memcpy(p_auth_tx->EMV_details.issuer_script_1+2, "71", 2 ); /* Tag ID */

                     /* Get length of data for this tag. */
                     memset( a_string, 0x00, sizeof(a_string) );
                     memset( b_string, 0x00, sizeof(b_string) );
                     b_string[0] = fieldLength;
                     genutil_bin_to_ascii( b_string, 1, a_string, sizeof(a_string) );
                     memcpy( p_auth_tx->EMV_details.issuer_script_1+4, a_string, 2 );

                     /* Finally put in the data. */
                     genutil_bin_to_ascii (&emv_raw[dataIndex], fieldLength , &tempstr[0], sizeof(tempstr));
                     memcpy(p_auth_tx->EMV_details.issuer_script_1+6, tempstr, sizeof(p_auth_tx->EMV_details.issuer_script_1)-7);

                     break;
            case 0x82:
                     genutil_bin_to_ascii (&emv_raw[dataIndex], fieldLength , &tempstr[0], sizeof(tempstr));
                     memcpy(p_auth_tx->EMV_details.interchange_profile, tempstr, sizeof(p_auth_tx->EMV_details.interchange_profile)-1);
                     break;
            case 0x84:
                     genutil_bin_to_ascii (&emv_raw[dataIndex], fieldLength , &tempstr[0], sizeof(tempstr));
                     memcpy(p_auth_tx->EMV_details.app_id, tempstr, sizeof(p_auth_tx->EMV_details.app_id)-1);
                     break;
            case 0x91:
                     genutil_bin_to_ascii (&emv_raw[dataIndex], fieldLength , &tempstr[0], sizeof(tempstr));
                     memcpy(p_auth_tx->EMV_details.issuer_auth_data+1, tempstr, sizeof(p_auth_tx->EMV_details.issuer_auth_data)-2);
                     p_auth_tx->EMV_details.issuer_auth_data[0] = fieldLength;
                     break;
            case 0x95:
                     genutil_bin_to_ascii (&emv_raw[dataIndex], fieldLength , &tempstr[0], sizeof(tempstr));
                     memcpy(p_auth_tx->EMV_details.tvr, tempstr, sizeof(p_auth_tx->EMV_details.tvr)-1);
                     break;
            case 0x9A:
                     genutil_bcd_to_asc (&emv_raw[dataIndex], fieldLength , &tempstr[0], sizeof(tempstr));
                     memcpy(p_auth_tx->EMV_details.tran_date, tempstr, sizeof(p_auth_tx->EMV_details.tran_date)-1);
                     break;
            case 0x9C:
                     genutil_bcd_to_asc (&emv_raw[dataIndex], fieldLength , &tempstr[0], sizeof(tempstr));
                     memcpy(p_auth_tx->EMV_details.tran_type, tempstr, sizeof(p_auth_tx->EMV_details.tran_type)-1);
                     break;
            default:
                     break;
         }
         dataIndex += fieldLength;
      }
   }
   return;
}
/*****************************************************************************

  Function:    Field62Response

  Description: This function will decode field 62 data from HOST_MESSAGE
               and stores pertinent data into the auth_tx structure.
  Author: 
      unknown
  Inputs:
      HOST_MESSAGE (global)  - structure that contains field 62 data 
  Outputs:
      p_auth_tx - Transaction record - parsed data goes into it
  Return values:
      None
  Modified by:
******************************************************************************/
void Field62Response( pAUTH_TX p_auth_tx )
{
   INT     i;
   INT     cps_bit;
   INT     cps_byte;
   INT     Len;
   INT     TotalLen;
   INT     BitMap_Len = 8;
   BYTE    BitMap[8];
   BYTE    TempStr[51];
   CHAR    temp_txn_id[17];


   /* Get length of field. */
   TotalLen = (INT)HOST_MESSAGE.priv62_len;

   /* Copy bitmap into local variable. */
   if ( TotalLen > 0 )
   {
      memset( BitMap, 0x00, sizeof(BitMap) );
      for( i=0; i<BitMap_Len; i++ )
         BitMap[i] = HOST_MESSAGE.priv62[i];

      memset( TempStr, 0x00, sizeof(TempStr) );
      memcpy( TempStr, &HOST_MESSAGE.priv62[8],
              sizeof(HOST_MESSAGE.priv62) - BitMap_Len );
   }

   /* Parse the bitmap for 1st 3 bytes, extracting the fields we want. */
   Len = 0;
   for( cps_byte=1; cps_byte<=3; cps_byte++ )
   {
      for( cps_bit=1; cps_bit<=8; cps_bit++ )
      {
         if ( BitMap[cps_byte-1] & BITTAB[cps_bit].bit_mask )
         {
            /* The bit is set. */
            switch( cps_byte )
            {
               case 1: /* Byte 1 */
                      switch( cps_bit )
                      {
                         case 1:  
                                /* CPS Authorization */
                                p_auth_tx->TLF01_details.visa_char_indicator =
                                   ascii_ebcdic_conv( TempStr[Len], E2A );

                                Len++;
                                break;
                         case 2:
                                /* CPS Transaction ID */
                                memset( temp_txn_id, 0x00, sizeof(temp_txn_id));
                                bcd_to_asc( &TempStr[Len], 8, temp_txn_id, 16 );
                                strcpy( (char *)p_auth_tx->TLF01_details.visa_tran_id,
                                       &temp_txn_id[1] );
                                Len += 8;
                                break;
                         case 3:
                                /* CPS Validation Code */
                                for( i=0; i<4 && ((Len+i)<sizeof(TempStr)); i++)
                                   p_auth_tx->TLF01_details.visa_validation_code[i]
                                   = ascii_ebcdic_conv( TempStr[Len+i], E2A );

                                /* Trim any trailing spaces. */
                                trim_spaces( p_auth_tx->TLF01_details.visa_validation_code );
                                Len += 4;
                                break;
                         case 7:
                                Len += 26;
                                break;
                         case 8:
                                Len += 3;
                                break;
                         default:
                                Len++;
                                break;
                      }
                      break;
               case 2: /* Byte 2 */
                      switch( cps_bit+8 )
                      {
                         case 10: Len += 3;
                                  break;
                         case 14: Len += 6;
                                  break;
                         case 16: Len += 2;
                                  break;
                         default: Len++;
                                  break;
                      }
                      break;
               case 3: /* Byte 3 */
                      switch( cps_bit+16 )
                      {
                         case 17:
//                                  for( i=0; (i<13) && ((Len+i)<sizeof(TempStr)); i++ )
//                                     p_auth_tx->TLF01_details.cps_master_card_info[i]
//                                     = ascii_ebcdic_conv( TempStr[Len+i], E2A );
                         default: break;
                      }
               default: break;
            }  /* End SWITCH based on BYTE */
         }  /* End IF bit is set */
      }  /* End LOOP on BIT */
   }  /* End LOOP on BYTE */
}


/******************************************************************************
 *
 *  NAME:         ParseField63
 *
 *  DESCRIPTION:  This function parses field 63, which is a bitmapped field.
 *                The values parsed are stored in the TLF01 structure:
 *
 *                   63.1 - Network ID              => product_codes11.code
 *                   63.3 - Reversal Reason Code    => product_codes12.code
 *                   63.4 - STIP/Switch Reason Code => product_codes13.code
 *
 *  INPUTS:       f63 - Field 63 data from Visa (first byte is length)
 *
 *  OUTPUTS:      p_auth_tx - Auth Tx structure containing parsed data
 *
 *  RTRN VALUE:   True means parsing was successful, else false
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
INT ParseField63( pCHAR f63, pAUTH_TX p_auth_tx )
{
   #define MAX_F63_SIZE   20
   #define F63_BITMAP_LEN  3

   INT     retval = true;
   INT     f63_bit;
   INT     Len;
   INT     TotalLen;
   BYTE    BitMap[F63_BITMAP_LEN];
   BYTE    TempStr[MAX_F63_SIZE+1];
   BYTE    buffer[6];
   int negativ_len_check=0;

   /* Get length of field. */
   TotalLen = f63[0];

   /* Copy bitmap into local variable. */
   if ( (TotalLen < 0) || (TotalLen > MAX_F63_SIZE) )
   {
      retval = false;
   }
   else
   {
      memcpy( BitMap, f63+1, sizeof(BitMap) );

      memset( TempStr, 0x00, sizeof(TempStr) );
	   negativ_len_check= TotalLen-F63_BITMAP_LEN;
	   if(negativ_len_check<0)
		   negativ_len_check= negativ_len_check*(-1);
      memcpy( TempStr, f63+F63_BITMAP_LEN+1,negativ_len_check);

      /* Parse the bitmap for 1st byte only, extracting the fields we want. */
      Len = 0;
      for( f63_bit=1; f63_bit<=8; f63_bit++ )
      {
         if ( BitMap[0] & BITTAB[f63_bit].bit_mask )
         {
            switch( f63_bit )
            {
               case 1:  
                      /* Network ID */
                      memset( buffer, 0x00, sizeof(buffer));
                      bcd_to_asc( &TempStr[Len], 2,(char*) buffer, 4 );
                      strcpy( (char*)p_auth_tx->TLF01_details.product_codes[10].code,
                             (char*) buffer );
                      Len+= 2;
                      break;
               case 3:
                      /* Reversal Message Reason Code */
                      memset( buffer, 0x00, sizeof(buffer));
                      bcd_to_asc( &TempStr[Len], 2, (char*)buffer, 4 );
                      strcpy((char*)p_auth_tx->TLF01_details.product_codes[11].code,
                             (char*)buffer );
                      Len += 2;
                      break;
               case 4:
                      /* STIP/Switch Reason Code */
                      memset( buffer, 0x00, sizeof(buffer));
                      bcd_to_asc( &TempStr[Len], 2,(char*) buffer, 4 );
                      strcpy((char*)p_auth_tx->TLF01_details.product_codes[12].code,
                             (char*)buffer );
                      Len += 2;
                      break;
               default:
                      Len++;
                      break;
            }
         }  /* End IF bit is set */
      }  /* End LOOP on BIT */
   }
   return( retval );
}
/******************************************************************************
 *
 *  NAME:         LogStipReasonCode
 *
 *  DESCRIPTION:  This function translates a STIP/Switch message code into
 *                a text message, which is logged to the system log.
 *
 *  INPUTS:       p_auth_tx - Structure with STIP/Switch message code (4 digits)
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   None
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
void LogStipReasonCode( pAUTH_TX p_auth_tx )
{
   INT  icode;
   BYTE scode[5]    = "";
   CHAR buffer[200] = "";

   strcpy((char *) scode, (char *)p_auth_tx->TLF01_details.product_codes[12].code );
   if ( isnum((char *)scode) )
   {
      icode = atoi( (char *)scode );
      sprintf( buffer, "STIP Reason Code %s: ", scode );

      switch( icode )
      {
         case 9001:
                  strcat( buffer, "The issuer was signed off." );
                  break;
         case 9002:
                  strcat( buffer, "The issuer was signed off by the switch." );
                  break;
         case 9011:
                  strcat( buffer, "The line to issuer is down." );
                  break;
         case 9020:
                  strcat( buffer, "Response to issuer timed out." );
                  break;
         case 9022:
                  strcat( buffer, "PACM diverted." );
                  break;
         case 9029:
                  strcat( buffer, "IARS Authorization was approved by referral center (member-certified)" );
                  break;
         case 9030:
                  strcat( buffer, "This transaction is auto-CDB; there is a pickup response from issuer." );
                  break;
         case 9031:
                  strcat( buffer, "Original processed in stand-in." );
                  break;
         case 9041:
                  strcat( buffer, "There was a pin verification error." );
                  break;
         case 9045:
                  strcat( buffer, "Switch was unable to translate the pin." );
                  break;
         case 9047:
                  strcat( buffer, "Real-time decision (RTD) decline." );
                  break;
         case 9048:
                  strcat( buffer, "There is an invalid CVV with the All Respond Option." );
                  break;
         case 9050:
                  strcat( buffer, "Switch generated this 0620 Alert message." );
                  break;
         case 9061:
                  strcat( buffer, "Internal system error or other switch-detected error condition" );
                  break;
         case 9102:
                  strcat( buffer, "Switch generated this 0420; an approval cannot be delivered to acquirer" );
                  break;
         case 9103:
                  strcat( buffer, "An approval cannot be delivered to acquirer; issuer timed out." );
                  break;

              	/*New STIP reason codes
              		3900 Incremental authorization
              		3901 Resubmission
              		3902 Delayed charges
              		3903 Reauthorization
              		3904 No show
              		3905 Account Top-up
              		*/
                       case 3900:
                                         strcat( buffer, "-Incremental authorization..." );
                                         break;
                       case 3901:
                                         strcat( buffer, "-Resubmission..." );
                                         break;
                       case 3902:
                                         strcat( buffer, "Delayed charges." );
                                         break;
                       case 3903:
                                         strcat( buffer, "Reauthorization." );
                                         break;
                       case 3904:
                                         strcat( buffer, "NO Show." );
                                         break;
                       case 3905:
                                         strcat( buffer, "Account Top-up..." );
                                          break;

         default  :
                  strcat( buffer, "Unrecognized STIP Reason Code." );
                  break;
      }
   }
   else
   {
      sprintf( buffer,
              "Invalid STIP Advice Code in field 63.4 from host: %s, "
              "must be numeric",
               scode );
   }

   
   return;
}


/******************************************************************************
 *
 *  NAME:         Field116_Handler
 *
 *  DESCRIPTION:  Field 116 contains various data from other hosts, such as
 *                MasterCard and Amex.  This field comes in responses from
 *                Visa when we process MC or Amex transactions thru the VAP.
 *
 *                This function will parse the field, reformat the varying
 *                data types into ASCII, then store into TLF01.
 *
 *                The format of field 116 is thus:
 *                   There are 1 or more data sets that define the network
 *                   such as MasterCard or American Express. Within each
 *                   data set there can be 1 or more tag sets. Each tag set
 *                   will contain an identifier, length, and data. Below are
 *                   the data types of the fields, shown for MC and Amex.
 *
 *                MasterCard Format
 *                   1 byte  (binary) = Overall length of field 116
 *                   1 byte  (BCD   ) = Data Set Id (0x67 = MasterCard)
 *                   2 bytes (binary) = Length of Data Set (..0x00,0x25)
 *                   1 byte  (BCD   ) = Tag ID (0x01 = DE22 from MC)
 *                   1 byte  (binary) = Tag Len (0x03 = 3)
 *                   3 bytes (UN    ) = Example: '902' --> 0x09,0x00,0x02
 *                   1 byte  (BCD   ) = Tag ID (0x02 = DE26 from MC)
 *                   1 byte  (binary) = Tag Len (0x02 = 2)
 *                   3 bytes (UN    ) = Example: '31' --> 0x03,0x01
 *                   1 byte  (BCD   ) = Tag ID (0x03 = DE61 from MC)
 *                   1 byte  (binary) = Tag Len (var ..0x1A = ..up to 26)
 *                  26 bytes (AN    ) = Example: '1B2' --> 0x31,0x42,0x32
 *                   
 *                American Express Format
 *                   1 byte  (binary) = Overall length of field 116
 *                   1 byte  (BCD   ) = Data Set Id (0x66 = Amex)
 *                   2 bytes (binary) = Length of Data Set (..0x00,0x15)
 *                   1 byte  (BCD   ) = Tag ID (0x01 = DE22 from Amex)
 *                   1 byte  (binary) = Tag Len (0x0C = 12)
 *                  12 bytes (AN    ) = Example: '12A12B12C12D' --> 0x31,..0x44
 *
 *                Note: UN = Unpacked Numeric (lower 4 bits only)
 *
 *
 *  INPUTS:       f116 - Field 116 data from Visa (first byte is length)
 *
 *  OUTPUTS:      p_auth_tx - Auth Tx structure containing parsed data
 *
 *  RTRN VALUE:   True means parsing was successful, else false
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
INT Field116_Handler( pCHAR f116, pAUTH_TX p_auth_tx )
{
   INT  retval = true;
   INT  f116_len_total;
   INT  f116_len_temp;
   INT  tlf01_buffer_len;
   INT  dataset_len;
   INT  i;
   INT  taglen;
   INT  taglen_temp;
   INT  tempNum;
   INT  moreTags       = true;
   INT  moreDataSets   = true;
   INT  errorFlag      = false;
   BYTE errbuf   [100] = "";
   BYTE tlf01_buf[256] = ""; /* This field contains the data for TLF01. */
   BYTE tempstr  [256] = "";
   BYTE dataset    [5] = "";
   BYTE dataset_Id [3] = "";
   BYTE tagId      [2] = "";
   BYTE tagData  [256] = "";

   f116_len_total = f116[0];
   if ( f116_len_total > 0 )
   {
      f116_len_temp    = 1; /* Current data pointer within location within f116. */
      tlf01_buffer_len = 0; /* Current data pointer within location within buffer*/

      /* Loop through the data sets. */
      while( moreDataSets && (f116_len_temp < f116_len_total) )
      {
         /* Get the Data Set. */
         bcd_to_asc((BYTE *) &f116[f116_len_temp++], 1,(char *) dataset_Id, 2 );
         if ( 0 == strcmp((char *)dataset_Id,"67") )
         {
            /* MasterCard */
            /* ---------- */
            dataset_len = f116[++f116_len_temp]; /* Pt to 2nd len byte. */
            f116_len_temp++;

            if ( dataset_len > 256 )
            {
               //sprintf( errbuf,
                   //    "Field 116 err: Unexpected Data Set length (%d) for MC",
//dataset_len );
               moreDataSets = false;
            }
            else
            {
               /* Copy Data Set ID and Data Set length into output buffer. */
               memset( dataset, 0x00, sizeof(dataset) );
               sprintf( (char *)dataset, "%02d", dataset_len );
               strcat(  (char *)tlf01_buf,  (char *)dataset_Id );
               strcat(  (char *)tlf01_buf,  (char *)dataset    );

               tlf01_buffer_len += strlen( (char *)dataset) + 2;

               /* Loop through the Tags for this Data Set. */
               taglen = 0;
               while( moreTags && (taglen < dataset_len) )
               {
                  /* Get Tag ID: Expecting 0x01, 0x02, 0x03 only. */
                  memset( tagData, 0x00, sizeof(tagData) );
                  if ( 0x01 == f116[f116_len_temp] )
                  {
                     /* TAG 01 - DE22 */
                     strcat( (char *) tlf01_buf, "01" );                /* Tag ID  */
                     taglen_temp = f116[++f116_len_temp];
                     sprintf(  (char *)tempstr, "%02d", taglen_temp );
                     strcat(  (char *)tlf01_buf, (char *) tempstr );             /* Tag Len */
                     f116_len_temp++;
                     tlf01_buffer_len += 4;

                     /* Get the tag data; convert from UN to ASCII. */
                     for( i=0; i<3; i++ )
                     {
                        memset( tagData, 0x00, sizeof(tagData) );
                        memset( tempstr, 0x00, sizeof(tempstr) );
                        tempstr[0] = f116[f116_len_temp++];
                        tempstr[0] &= 0x0f;  /* Get only the 4 LSB's (UN). */
                        tempNum = tempstr[0];
                        sprintf( (char *) tagData, "%1d", tempNum );
                        strcat(  (char *)tlf01_buf,  (char *)tagData );
                     }
                     taglen += 5;
                     tlf01_buffer_len += 3;
                  }
                  else if ( 0x02 == f116[f116_len_temp] )
                  {
                     /* TAG 02 - DE26 */
                     strcat(  (char *)tlf01_buf, "02" );                /* Tag ID  */
                     taglen_temp = f116[++f116_len_temp];
                     sprintf( (char *) tempstr, "%02d", taglen_temp );
                     strcat(  (char *)tlf01_buf, (char *) tempstr );             /* Tag Len */
                     f116_len_temp++;
                     tlf01_buffer_len += 4;

                     /* Get the tag data; convert from UN to ASCII. */
                     for( i=0; i<2; i++ )
                     {
                        memset( tagData, 0x00, sizeof(tagData) );
                        memset( tempstr, 0x00, sizeof(tempstr) );
                        tempstr[0] = f116[f116_len_temp++];
                        tempstr[0] &= 0x0f;  /* Get only the 4 LSB's (UN). */
                        tempNum = tempstr[0];
                        sprintf( (char *) tagData, "%1d", tempNum );
                        strcat( (char *) tlf01_buf,  (char *)tagData );
                     }
                     taglen += 4;
                     tlf01_buffer_len += 2;
                  }
                  else if ( 0x03 == f116[f116_len_temp] )
                  {
                     /* TAG 03 - DE61 */
                     strcat( (char *) tlf01_buf, "03" );                /* Tag ID  */
                     taglen_temp = f116[++f116_len_temp];
                     sprintf(  (char *)tempstr, "%02d", taglen_temp );
                     strcat(  (char *)tlf01_buf, (char *) tempstr );             /* Tag Len */
                     f116_len_temp++;
                     tlf01_buffer_len += 4;

                     /* Get the tag data. It is already in ASCII. */
                     memset( tagData, 0x00, sizeof(tagData) );
                     memcpy( tagData, &f116[f116_len_temp], taglen_temp );
                     strcat(  (char *)tlf01_buf, (char *) tagData );
                     f116_len_temp    += taglen_temp;
                     tlf01_buffer_len += taglen_temp;
                     taglen           += taglen_temp + 2;
                  }
                  else
                  {
                     taglen_temp    = f116[++f116_len_temp];
                     f116_len_temp += taglen_temp + 1;
                     taglen        += taglen_temp + 2;
                  }
               }
            }
         }
         else if ( 0 == strcmp( (char *)dataset_Id,"66") )
         {
            /* American Express */
            /* ---------------- */
            dataset_len = f116[++f116_len_temp]; /* Pt to 2nd len byte. */
            f116_len_temp++;

            if ( dataset_len > 256 )
            {
             //  sprintf( errbuf,
                 //      "Field 116 err: Unexpected Data Set length (%d) for Amex",
               //         dataset_len );
               moreDataSets = false;
            }
            else
            {
               /* Copy Data Set ID and Data Set length into output buffer. */
               memset( dataset, 0x00, sizeof(dataset) );
               sprintf(  (char *)dataset, "%02d", dataset_len );
               strcat( (char *) tlf01_buf,  (char *)dataset_Id );
               strcat(  (char *)tlf01_buf,  (char *)dataset    );

               tlf01_buffer_len += strlen( (char *)dataset) + 2;

               /* Loop through the Tags for this Data Set. */
               taglen = 0;
               while( moreTags && (taglen < dataset_len) )
               {
                  /* Get Tag ID: Expecting 0x01 only. */
                  memset( tagData, 0x00, sizeof(tagData) );
                  if ( 0x01 == f116[f116_len_temp] )
                  {
                     /* TAG 01 - DE22 */
                     strcat(  (char *)tlf01_buf, "01" );                /* Tag ID  */
                     taglen_temp = f116[++f116_len_temp];
                     sprintf(  (char *)tempstr, "%02d", taglen_temp );
                     strcat(  (char *)tlf01_buf, (char *) tempstr );             /* Tag Len */
                     f116_len_temp++;
                     tlf01_buffer_len += 4;

                     /* Get the tag data. It is already in ASCII. */
                     memset( tagData, 0x00, sizeof(tagData) );
                     memcpy( tagData, &f116[f116_len_temp], taglen_temp );
                     strcat( (char *) tlf01_buf,  (char *)tagData );
                     f116_len_temp    += taglen_temp;
                     tlf01_buffer_len += taglen_temp;
                     taglen           += taglen_temp + 2;
                  }
                  else
                  {
                     taglen_temp    = f116[++f116_len_temp];
                     f116_len_temp += taglen_temp + 1;
                     taglen        += taglen_temp + 2;
                  }
               }
            }
         }
         else
         {
            /* This data set is not supported. Get its length and skip it. */
            dataset_len = f116[f116_len_temp];
            f116_len_temp += dataset_len + 1;

            if ( f116_len_temp < 0 )
               moreDataSets = false;
         }
      }

      if ( errbuf[0] != 0x00 )
      {
        return 0;
      }
      else
      {
         /* Log the parsed data into TLF01. */
         strcpy( (char*)p_auth_tx->TLF01_details.dest_key, (char *)tlf01_buf );
      }
   }
   return( retval );
}


/*****************************************************************************
 *
 *  Function:    FIELD126PARSER
 *
 *  Description: This function parses field 126 from Visa and puts it into
 *               'outstring' in ASCII format, including the bitmap.  The
 *               bitmap in 'outstring' will be 16 ASCII bytes.
 *
 * Author:       D. Irby
 *
 * Inputs:       HOST_MESSAGE.de126 (Global)
 *
 * Outputs:      outstring - DE126 in ASCII format.
 *               cvv2_data - Data from field 126.10
 *
 * Return values: None
 *
 ******************************************************************************/
void Field126Parser(  pAUTH_TX p_auth_tx, pBYTE cvv2_data  )
{
   #define BCD_BITMAP_LEN   8
   #define ASC_BITMAP_LEN  16

   INT     i;
   INT     cps_bit;
   INT     cps_byte;
   INT     TotalLen;
   INT     inptr  = BCD_BITMAP_LEN;   /* Point to inbuf, past bitmap. */
   INT     outptr = ASC_BITMAP_LEN;   /* Point to outstring, past bitmap */
   BYTE    bcd_BitMap[8] ="";
   BYTE    asc_BitMap[17]="";
   BYTE    inbuf[255]    ="";
   BYTE    tempstr[58];
   BYTE	   outstring[241]="";	


   /* Get length of field. */
   TotalLen = (INT)HOST_MESSAGE.de126_len[0];
   memcpy( inbuf, HOST_MESSAGE.de126, TotalLen );

   /* Copy bitmap into local variable. */
   memcpy( bcd_BitMap, HOST_MESSAGE.de126, BCD_BITMAP_LEN );

   /* Convert bitmap to ASCII. */
   bcd_to_asc( bcd_BitMap, BCD_BITMAP_LEN, (char *) asc_BitMap, ASC_BITMAP_LEN );
   memcpy( outstring, asc_BitMap, ASC_BITMAP_LEN );

   /* Parse the bitmap for the first 2 bytes only - others are unused. */
   for( cps_byte=1; cps_byte<=2; cps_byte++ )
   {
      for( cps_bit=1; cps_bit<=8; cps_bit++ )
      {
         if ( bcd_BitMap[cps_byte-1] & BITTAB[cps_bit].bit_mask )
         {
            /* The bit is set. */
            switch( cps_byte )
            {
               case 1: /* Byte 1 */
                      switch( cps_bit )
                      {
                         case 1:  
                                /* Customer Name */
                                memset(tempstr, 0x00, sizeof(tempstr) );
                                for( i=0; i<25; i++ )
                                {
                                   tempstr[i] =
                                      ascii_ebcdic_conv( inbuf[inptr+i], E2A );
                                }
                                strcat( (char *) outstring+outptr, (char *) tempstr );
                                inptr  += 25;
                                outptr += 25;
                                break;
                         case 2:
                                /* Customer Address */
                                memset(tempstr, 0x00, sizeof(tempstr) );
                                for( i=0; i<57; i++ )
                                {
                                   tempstr[i] =
                                      ascii_ebcdic_conv( inbuf[inptr+i], E2A );
                                }
                                strcat(  (char *)outstring+outptr,  (char *)tempstr );
                                inptr  += 57;
                                outptr += 57;
                                break;
                         case 3:
                                /* Biller Address */
                                memset(tempstr, 0x00, sizeof(tempstr) );
                                for( i=0; i<57; i++ )
                                {
                                   tempstr[i] =
                                      ascii_ebcdic_conv( inbuf[inptr+i], E2A );
                                }
                                strcat( (char *) outstring+outptr,  (char *)tempstr );
                                inptr  += 57;
                                outptr += 57;
                                break;
                         case 4:
                                /* Biller Telephone Number */
                                memset(tempstr, 0x00, sizeof(tempstr) );
                                for( i=0; i<18; i++ )
                                {
                                   tempstr[i] =
                                      ascii_ebcdic_conv( inbuf[inptr+i], E2A );
                                }
                                strcat(  (char *)outstring+outptr,  (char *)tempstr );
                                inptr  += 18;
                                outptr += 18;
                                break;
                         case 5:
                                /* Process by Date */
                                memset(tempstr, 0x00, sizeof(tempstr) );
                                for( i=0; i<6; i++ )
                                {
                                   tempstr[i] =
                                      ascii_ebcdic_conv( inbuf[inptr+i], E2A );
                                }
                                strcat( (char *) outstring+outptr, (char *) tempstr );
                                inptr  += 6;
                                outptr += 6;
                                break;
                         case 6:
                                /* Cardholder Certificate Serial Number */
                                memset(tempstr, 0x00, sizeof(tempstr) );
                                genutil_bin_to_ascii( &inbuf[inptr], 1,
                                                       (char *)tempstr, sizeof(tempstr));
                                strncat(  (char *)outstring+outptr, (char *) tempstr, 2 );
                                inptr++;
                                outptr += 2;
                                memset(tempstr, 0x00, sizeof(tempstr) );
                                bcd_to_asc( &inbuf[inptr], 16,  (char *)tempstr, 32 );
                                strcat(  (char *)outstring+outptr,  (char *)tempstr );
                                inptr  += 16;
                                outptr += 32;
                                break;
                         case 7:
                                /* Merchant Certificate Serial Number */
                                memset(tempstr, 0x00, sizeof(tempstr) );
                                genutil_bin_to_ascii( &inbuf[inptr], 1,
                                                       (char *)tempstr, sizeof(tempstr));
                                strncat(  (char *)outstring+outptr,  (char *)tempstr, 2 );
                                inptr++;
                                outptr += 2;
                                memset(tempstr, 0x00, sizeof(tempstr) );
                                bcd_to_asc( &inbuf[inptr], 16,  (char *)tempstr, 32 );
                                strcat(  (char *)outstring+outptr,  (char *)tempstr );
                                inptr  += 16;
                                outptr += 32;
                                break;
                         case 8:
                                /* Transaction ID (VSEC) */
                                memset(tempstr, 0x00, sizeof(tempstr) );
                                genutil_bin_to_ascii( &inbuf[inptr], 20,
                                                      (char *) tempstr, sizeof(tempstr));
                                strcat( (char *) outstring+outptr, (char *) tempstr );
                                inptr  += 20;
                                outptr += 20;
                                break;
                      }
                      break;
               case 2: /* Byte 2 */
                      switch( cps_bit+8 )
                      {
                         case  9:
                                /* TranStain (VSEC) */
                                memset(tempstr, 0x00, sizeof(tempstr) );
                                genutil_bin_to_ascii( &inbuf[inptr], 20,
                                                      (char *) tempstr, sizeof(tempstr));
                                strcat(  (char *)outstring+outptr, (char *) tempstr );
                                inptr  += 20;
                                outptr += 20;
                                break;
                         case 10:
                                /* CVV2 Request Data */
                                memset(tempstr, 0x00, sizeof(tempstr) );
                                for( i=0; i<6; i++ )
                                {
                                   tempstr[i] =
                                      ascii_ebcdic_conv( inbuf[inptr+i], E2A );
                                }
                                inptr  += 6;

                                strcpy(  (char *)cvv2_data, (char *) tempstr );
                                break;
                         case 11:
                                /* IGOTS Transaction Description */
                                memset(tempstr, 0x00, sizeof(tempstr) );
                                for( i=0; i<2; i++ )
                                {
                                   tempstr[i] =
                                      ascii_ebcdic_conv( inbuf[inptr+i], E2A );
                                }
                                strcat( (char *) outstring+outptr,  (char *)tempstr );
                                inptr  += 2;
                                outptr += 2;
                                break;
                         case 12:
                                /* Service Development Indicator */
                                strncat(  (char *)outstring+outptr,  (char *)&inbuf[inptr], 24 );
                                break;
                      }
                      break;
               default: break;
            }  /* End SWITCH based on BYTE */
         }  /* End IF bit is set */
      }  /* End LOOP on BIT */
   }  /* End LOOP on BYTE */
    memcpy( p_auth_tx->TLF01_details.voice_auth_text,outstring ,sizeof(outstring));

}
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
void Rj_with_lead_zeros( pCHAR str, INT FieldWidth )
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
	//issuer_auth_data
	if(strlen((char*)EMV_details.issuer_auth_data)!=0)
	{
		strncat((char*)iccdata,"91",2);
		strncat((char*)iccdata,(char*)EMV_details.issuer_auth_data,strlen((char *)EMV_details.issuer_auth_data));
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
	if(strlen((char*)EMV_details.pan_sequence_number)!=0 ||
		strlen((char *)(AUTH_TX_V.host2_message_seq_nbr)))
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

void Cex_amex_parserDlg::OnEnChangeParsedmsg()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialogEx::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
}


void Cex_amex_parserDlg::OnEnChangeFlowfile()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialogEx::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
}

/******************************************************************************
 *
 *  NAME:         PARSE_HOST_MSG
 *
 *  DESCRIPTION:  This procedure copies information from a string sent by the
 *                host.  It copies it into the HOST_MESSAGE structure.  The
 *                data is converted from EBCDIC to ASCII as they are moved.
 *
 *  INPUTS:       None
 *
 *  OUTPUTS:      err_buf - Textual message in case of a problem
 *
 *  RTRN VALUE:   True if successful, false if a problem occurred
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
INT parse_host_msg( pCHAR err_buf )
{
   struct bitfield_data *pBF;
   INT                   bfidx;    /* Index to bitfields for this transaction */
   INT                   ret_val = true;
   INT                   BitFieldSize;
   BYTE                  ReqRsp_Buffer_hex[17]="";
   BYTE                  bitmap;

   /* Initialize the HOST_MESSAGE structure. */
   memset(&HOST_MESSAGE, 0x00, sizeof(HOST_MESSAGE));

   /* Extract the message type from the host message. */
   convert_to_ascii( ReqRsp_Buffer, HOST_MESSAGE.msgtype, 4 );
   ebcdic_to_hex(&ReqRsp_Buffer[4],&bitmap, 1 );
     
   bitmapidx = 4;       /* point to bitmap */

   /* Point past bitmap, but 1st check for extended bitmap. */
   
   if ( bitmap &  BITTAB[bf_1xbitfield].bit_mask )
   {
      reqidx = 36;                         /* Extended     */
	  genutil_bin_to_ascii ((BYTE *)&ReqRsp_Buffer[4], EXTENDED_BITMAP_SIZE, astr_bitmap, sizeof(astr_bitmap));
	  ebcdic_to_hex(&ReqRsp_Buffer[bitmapidx],ReqRsp_Buffer_hex, 32 );
      BitFieldSize = EXTENDED_BITFIELD;
   }
   else
   {
      reqidx = 20;                         /* Not Extended */
	 genutil_bin_to_ascii ((BYTE *)&ReqRsp_Buffer[4], NORMAL_BITFIELD, astr_bitmap, sizeof(astr_bitmap));
	  ebcdic_to_hex(&ReqRsp_Buffer[bitmapidx],ReqRsp_Buffer_hex, 16 );
      BitFieldSize = NORMAL_BITFIELD;
   }

  
   /*------------------------------------------------------------------
      For every bitfield present in message ( except first bit ) . . .
   -------------------------------------------------------------------*/
   for( bfidx = 2; bfidx <= BitFieldSize; bfidx++ )
   {
      if ( ReqRsp_Buffer_hex[BITTAB[bfidx].byte_no] &
           BITTAB[bfidx].bit_mask )
      {
         /* Bit is on, field is there.... */
         pBF = Find_Bitfield_Table((bitfields)bfidx);

         if ( NULL == pBF )
         {
            sprintf( err_buf, "Unexpected bitfield (%d) in message from host.",
                     bfidx );
            strcat( err_buf, " MsgType = " );
           // strncat( err_buf, HOST_MESSAGE.msgtype, 4 );
            ret_val = false; /* Invalid field in message - so quit. */
            break;
         }
		 else
		 {
			 BitFieldArray[pBF->bd_fieldid] = 1;
		 }
         /*---------------------------------------*/
         /* call the Response Handling routine... */
         /*---------------------------------------*/
	
         if ( false == (ret_val = (pBF->bd_BFRsp)(pBF->bd_fieldid)) )
         {
            if ( true == invalid_msg_format )
            {
               strcpy( err_buf, "Invalid  message format." );
               strcat( err_buf, " MsgType = " );
              // strncat( err_buf, HOST_MESSAGE.msgtype, 4 );
               break;
            }
            else
            {
               sprintf( err_buf,
                    "Unable to parse field %d in msg from host, MsgType: %4.4s",
                     bfidx, HOST_MESSAGE.msgtype );
            }
         }
      }
   }
   return( ret_val );
} /* parse_host_msg */  




/*************************************************************************************
       NAME:               convert_to_ascii

       DESCRIPTION:        converts a stream of bytes from EBCDIC to ASCII

       INPUTS:
           source          pointer to a stream of bytes to be converted
           len             length of the byte stream
       OUTPUTS:
           dest            pointer to stream of converted bytes

       RETURNS:            
           None.
           
       AUTHOR:             Ram Malathkar
*************************************************************************************/
void convert_to_ascii( pBYTE source, pBYTE dest, INT len )
{
   INT  temp;
   BYTE temp_byte;
   
   if( !maps_initialised )
      init_character_code_maps();

   for( temp = 0; temp < len; temp++ )
   {
      temp_byte = *(source + temp);
      *(dest + temp) = e2a[temp_byte];
   }

   return;
} /* convert_to_ascii */


/*****************************************************************************

Function:      init_character_code_maps

Description:   This function will initialise the EBCDIC and ASCII code 
               maps needed for the conversion routines.
               
Author: 
               Ram Malathkar
Inputs:
               None
Outputs:
               None
Return values:
               None
******************************************************************************/
void init_character_code_maps()
{
   // ascii to ebcdic map
   memset( a2e, 0, sizeof( a2e ) );

   a2e[0x30]=0xF0; a2e[0x31]=0xF1; a2e[0x32]=0xF2; a2e[0x33]=0xF3; a2e[0x34]=0xF4; 
   a2e[0x35]=0xF5; a2e[0x36]=0xF6; a2e[0x37]=0xF7; a2e[0x38]=0xF8; a2e[0x39]=0xF9;  
   a2e[0x61]=0x81; a2e[0x62]=0x82; a2e[0x63]=0x83; a2e[0x64]=0x84; a2e[0x65]=0x85; 
   a2e[0x66]=0x86; a2e[0x67]=0x87; a2e[0x68]=0x88; a2e[0x69]=0x89; a2e[0x6A]=0x91; 
   a2e[0x6B]=0x92; a2e[0x6C]=0x93; a2e[0x6D]=0x94; a2e[0x6E]=0x95; a2e[0x6F]=0x96; 
   a2e[0x70]=0x97; a2e[0x71]=0x98; a2e[0x72]=0x99; a2e[0x73]=0xA2; a2e[0x74]=0xA3; 
   a2e[0x75]=0xA4; a2e[0x76]=0xA5; a2e[0x77]=0xA6; a2e[0x78]=0xA7; a2e[0x79]=0xA8; 
   a2e[0x7A]=0xA9; a2e[0x41]=0xC1; a2e[0x42]=0xC2; a2e[0x43]=0xC3; a2e[0x44]=0xC4; 
   a2e[0x45]=0xC5; a2e[0x46]=0xC6; a2e[0x47]=0xC7; a2e[0x48]=0xC8; a2e[0x49]=0xC9; 
   a2e[0x4A]=0xD1; a2e[0x4B]=0xD2; a2e[0x4C]=0xD3; a2e[0x4D]=0xD4; a2e[0x4E]=0xD5; 
   a2e[0x4F]=0xD6; a2e[0x50]=0xD7; a2e[0x51]=0xD8; a2e[0x52]=0xD9; a2e[0x53]=0xE2; 
   a2e[0x54]=0xE3; a2e[0x55]=0xE4; a2e[0x56]=0xE5; a2e[0x57]=0xE6; a2e[0x58]=0xE7; 
   a2e[0x59]=0xE8; a2e[0x5A]=0xE9; a2e[0x20]=0x40; a2e[0x2E]=0x4B; a2e[0x3C]=0x4C; 
   a2e[0x28]=0x4D; a2e[0x2B]=0x4E; a2e[0x7C]=0x4F; a2e[0x26]=0x50; a2e[0x21]=0x5A; 
   a2e[0x24]=0x5B; a2e[0x2A]=0x5C; a2e[0x29]=0x5D; a2e[0x3B]=0x5E; a2e[0x2D]=0x60; 
   a2e[0x2F]=0x61; a2e[0x2C]=0x6B; a2e[0x25]=0x6C; a2e[0x5F]=0x6D; a2e[0x3E]=0x6E; 
   a2e[0x3F]=0x6F; a2e[0x3A]=0x7A; a2e[0x23]=0x7B; a2e[0x40]=0x7C; a2e[0x27]=0x7D; 
   a2e[0x3D]=0x7E; a2e[0x22]=0x7F; 
   
   // ebcdic to ascii map
   memset( e2a, 0, sizeof( e2a ) );

   e2a[0xF0]=0x30; e2a[0xF1]=0x31; e2a[0xF2]=0x32; e2a[0xF3]=0x33; e2a[0xF4]=0x34; 
   e2a[0xF5]=0x35; e2a[0xF6]=0x36; e2a[0xF7]=0x37; e2a[0xF8]=0x38; e2a[0xF9]=0x39;  
   e2a[0x81]=0x61; e2a[0x82]=0x62; e2a[0x83]=0x63; e2a[0x84]=0x64; e2a[0x85]=0x65; 
   e2a[0x86]=0x66; e2a[0x87]=0x67; e2a[0x88]=0x68; e2a[0x89]=0x69; e2a[0x91]=0x6A; 
   e2a[0x92]=0x6B; e2a[0x93]=0x6C; e2a[0x94]=0x6D; e2a[0x95]=0x6E; e2a[0x96]=0x6F; 
   e2a[0x97]=0x70; e2a[0x98]=0x71; e2a[0x99]=0x72; e2a[0xA2]=0x73; e2a[0xA3]=0x74; 
   e2a[0xA4]=0x75; e2a[0xA5]=0x76; e2a[0xA6]=0x77; e2a[0xA7]=0x78; e2a[0xA8]=0x79; 
   e2a[0xA9]=0x7A; e2a[0xC1]=0x41; e2a[0xC2]=0x42; e2a[0xC3]=0x43; e2a[0xC4]=0x44; 
   e2a[0xC5]=0x45; e2a[0xC6]=0x46; e2a[0xC7]=0x47; e2a[0xC8]=0x48; e2a[0xC9]=0x49; 
   e2a[0xD1]=0x4A; e2a[0xD2]=0x4B; e2a[0xD3]=0x4C; e2a[0xD4]=0x4D; e2a[0xD5]=0x4E; 
   e2a[0xD6]=0x4F; e2a[0xD7]=0x50; e2a[0xD8]=0x51; e2a[0xD9]=0x52; e2a[0xE2]=0x53; 
   e2a[0xE3]=0x54; e2a[0xE4]=0x55; e2a[0xE5]=0x56; e2a[0xE6]=0x57; e2a[0xE7]=0x58; 
   e2a[0xE8]=0x59; e2a[0xE9]=0x5A; e2a[0x40]=0x20; e2a[0x4B]=0x2E; e2a[0x4C]=0x3C; 
   e2a[0x4D]=0x28; e2a[0x4E]=0x2B; e2a[0x4F]=0x7C; e2a[0x50]=0x26; e2a[0x5A]=0x21; 
   e2a[0x5B]=0x24; e2a[0x5C]=0x2A; e2a[0x5D]=0x29; e2a[0x5E]=0x3B; e2a[0x60]=0x2D;
   e2a[0x61]=0x2F; e2a[0x6B]=0x2C; e2a[0x6C]=0x25; e2a[0x6D]=0x5F; e2a[0x6E]=0x3E; 
   e2a[0x6F]=0x3F; e2a[0x7A]=0x3A; e2a[0x7B]=0x23; e2a[0x7C]=0x40; e2a[0x7D]=0x27; 
   e2a[0x7E]=0x3D; e2a[0x7F]=0x22;
   
   maps_initialised = true;

} /* init_character_code_maps */
/******************************************************************************
 *
 *  NAME:         EBCDIC_TO_HEX
 *
 *  DESCRIPTION:  This function converts a byte string from EBCDIC format
 *                into HEX format.  It does this by first converting
 *                from ebcdic to ascii by calling an existing function.
 *                Then it converts from ascii to hex.
 *
 *  INPUTS:       instr - EBCDIC string to be converted to HEX
 *                len   - Number of bytes in 'instr'. Maximum len is 100.
 *
 *  OUTPUTS:      outstr - The converted string in HEX format
 *
 *  RTRN VALUE:   None
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
void ebcdic_to_hex( pBYTE instr, pBYTE outstr, INT len )
{
   #define  MAX_LEN 100

   INT   i, j, outstr_idx;
   BYTE  temp_byte;
   BYTE  temp_str[MAX_LEN] = "";

   BYTE  hex_chars[22] =   { 0x00, 0x01, 0x02, 0x03,
                             0x04, 0x05, 0x06, 0x07,
                             0x08, 0x09, 0x0a, 0x0b,
                             0x0c, 0x0d, 0x0e, 0x0f,
                             0x0A, 0x0B, 0x0C, 0x0D,
                             0x0E, 0x0F };

   BYTE  ascii_chars[22] = {  '0',  '1',  '2',  '3',
                              '4',  '5',  '6',  '7',
                              '8',  '9',  'a',  'b',
                              'c',  'd',  'e',  'f',
                              'A',  'B',  'C',  'D',
                              'E',  'F' };

   /* Convert from EBCDIC into ASCII. */
   if ( len > MAX_LEN )
      len = MAX_LEN;

   convert_to_ascii( instr, temp_str, len );

   /* Loop through the input string for each byte. */
   for ( i=0,outstr_idx=0; i<len; i+=2,outstr_idx++ )
   {
      /* Get upper half of hex byte. */
      temp_byte = temp_str[i];
      for ( j=0; j<22; j++ )
      {
         if ( ascii_chars[j] == temp_byte )
         {
            outstr[outstr_idx] = hex_chars[j];

            /* Move to upper nibble. */
            outstr[outstr_idx] <<= 4;
            break;
         }
      }

      /* Get lower half of hex byte. */
      temp_byte = temp_str[i+1];
      for ( j=0; j<22; j++ )
      {
         if ( ascii_chars[j] == temp_byte )
         {
            outstr[outstr_idx] |= hex_chars[j];
            break;
         }
      }
   } /* End outer loop */

   return;
}
/******************************************************************************
 *
 *  NAME:         TRIM_SPACES
 *
 *  DESCRIPTION:  This function converts spaces at the end of a string into
 *                nulls.
 *
 *  INPUTS:       pString - String that does not want spaces at the end
 *
 *  OUTPUTS:      pString - Same string, but without trailing spaces
 *
 *  RTRN VALUE:   None
 *
 *  AUTHOR:       D. Irby
 *
 *  MODIFICATIONS:
 ******************************************************************************/
void trim_spaces( pBYTE pString )
{

   INT len, position;

   len = strlen((char *)pString) - 1;
   for(position = len; position >= 0; position--)
   {
      if(pString[position] == ' ')
         pString[position] = 0;
      else
         break;
   }
}


/*****************************************************************************

  Function:    FIND_CAVV_FLAG

  Description: This function will dig the CAVV result value out of field
               44 response data.  This value is in byte 15.

  Author:      D. Irby

  Inputs:      len  - Length of DE44
               de44 - Field 44 data

  Outputs:     cavv_result - Value defining result of Cardholder Authentication
                             Verification Value (CAVV)

  Return values: None

******************************************************************************/
void find_cavv_flag( BYTE len, pBYTE de44, pBYTE cavv_result )
{
   if ( len > CAVV_RESULT_LOC )
   {
      *cavv_result = de44[CAVV_RESULT_LOC];
   }
   return;
}


/*****************************************************************************

  Function:    FIND_CARC_FLAG

  Description: This function will dig the Card Authentication Result code
               out of field 44 response data.  This value is in byte 9.

  Author:      D. Irby

  Inputs:      len  - Length of DE44
               de44 - Field 44 data

  Outputs:     carc_result - Value defining result of Cardholder Authentication
                             Result Code

  Return values: None

******************************************************************************/
void find_carc_flag( BYTE len, pBYTE de44, pBYTE carc_result )
{
   if ( len > CARC_RESULT_LOC )
   {
      *carc_result = de44[CARC_RESULT_LOC];
   }
   return;
}

/*****************************************************************************

  Function:    FIND_CVV2_RESULT_CODE

  Description: This function will dig the Card Authentication Result code
               out of field 44 response data.  This value is in byte 9.

  Author:      D. Irby

  Inputs:      len  - Length of DE44
               de44 - Field 44 data

  Outputs:     cvv2_result - Value defining result of CVV2 Verification

  Return values: None

******************************************************************************/
void find_cvv2_result_code( BYTE len, pBYTE de44, pBYTE cvv2_result )
{
   if ( len > CVV2_RESULT_LOC )
   {
      *cvv2_result = de44[CVV2_RESULT_LOC];
   }
   return;
}

INT isnum(pCHAR string)
{
   INT i;
   INT length;

   length = (INT)strlen(string);

   for (i = 0; i < length; i++)
   {
      if (!isdigit(string[i]))
        return(false);
   }
   return(true);
}

void Cex_amex_parserDlg::OnBnClickedCopy()
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


void Cex_amex_parserDlg::OnBnClickedButton1()
{
	EqitInfo Einfo;
	Einfo.DoModal();
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

	for(int i=0; i<105; i++)
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
