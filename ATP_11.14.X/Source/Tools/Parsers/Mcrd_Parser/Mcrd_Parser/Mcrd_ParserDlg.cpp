
// Mcrd_ParserDlg : implementation file
//

#include "stdafx.h"
#include "Mcrd_Parser.h"
#include "Mcrd_ParserDlg.h"
#include "afxdialogex.h"
#include "McrdParser.h"
#include "MCInfo.h"
#include "BASICTYP.H"
#include <math.h>
#ifdef _DEBUG
#define new DEBUG_NEW
#endif


void ascii_to_bin (LPBYTE src, LPBYTE dst, INT len);
pAUTH_TX p_auth_tx;
AUTH_TX AUTH_TX_V;
EMV EMV_details;

#define  THIRD_BITMAP_SIZE					24
#define  EXTENDED_BITMAP_SIZE    			16
#define  NORMAL_BITMAP_SIZE					8
CHAR astr_bitmap[64]= {0};
// CAboutDlg dialog used for App About
int BitFieldArray[128] = {0};
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
	DDX_Control(pDX, IDC_EXPORT, exportbtn);
}

BEGIN_MESSAGE_MAP(Cex_amex_parserDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_PARSE, &Cex_amex_parserDlg::OnBnClickedParse)
	ON_EN_CHANGE(IDC_PARSEDMSG, &Cex_amex_parserDlg::OnEnChangeParsedmsg)
	ON_EN_CHANGE(IDC_FLOWFILE, &Cex_amex_parserDlg::OnEnChangeFlowfile)
	ON_BN_CLICKED(IDC_CLEAR, &Cex_amex_parserDlg::OnBnClickedClear)
	ON_BN_CLICKED(IDC_COPYBUTTON, &Cex_amex_parserDlg::OnBnClickedcopyButton)
	ON_BN_CLICKED(IDC_ABOUTBUTTON, &Cex_amex_parserDlg::OnBnClickedaboutButton)
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


/*CUP header*/
#define CUP_HDR_SIZE_SUCC 				46
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
#define COMM_BUF_SZE                   5000

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
"009 SettleConvRate				: ",
"010 BillingConvRate				: ",
"011 SystemTraceNo				: ",
"012 TxnTime				: ",
"013 TxnDate				: ",
"014 ExpirationDate				: ",
"015 SettleDate				: ",
"016 ConvDate				: ",
"017 CaptureDate			: ",
"018 Sic Number				: ",
"019 AcquiringCountry			: ",
"020 PANExtCountry			: ",
"021 ForwardCountry			: ",
"022 POSEntryMode				: ",
"023 CardSequenceNo			: ",
"024 NII					: ",
"025 POSConditionCode			: ",
"026 POSPINCode				: ",
"027 AuthIdLen				: ",
"028 TxnFeeAmount			: ",
"029 SettleFeeAmount		: ",
"030 TxnProcFeeAmount		: ",
"031 SettleProcFeeAmount	: ",
"032 AcquiringIdCode				: ",
"033 ForwardIdCode				: ",
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
"044 AddRespData			: ",
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
"070 Nmi Code			: ",
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
"095 ReplacementAmounts		: ",
"096 MessageSecCode			: ",
"097 AmountNetSettle		: ",
"098 Payee					: ",
"099 SettleIdCode			: ",
"100 ReceivingIdCode			: ",
"101 Filename				: ",
"102 AccountId1				: ",
"103 AccountId2				: ",
"104 DigitalPayData			: ",
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
        BYTE bitmap1_bin   [8];
        BYTE bitmap2_bin   [8];
        BYTE pan_len       [2];
        BYTE pan           [19];
        BYTE proccode      [6];
        BYTE amount        [12];
        BYTE settle_amt    [12];
        BYTE ch_bill_amt   [12];
        BYTE gmt_time      [10];
		BYTE ch_bill_fee   [8];
        BYTE conv_rate_stl [9];
        BYTE conv_rate_chb [8];
        BYTE stan          [6];
        BYTE time          [6];
        BYTE date          [4];
        BYTE date_expd     [4];
        BYTE date_settle   [4];
        BYTE conv_date     [4];
		BYTE capture_date     [4];
        BYTE sic_number    [4];
        BYTE ctry_code     [3];
        BYTE pan_ctry_cd   [3];
		BYTE frwrdinst_ctry_cd[3];
        BYTE posentry      [3];
		BYTE card_seq_num  [3];		  
        BYTE nii           [2];
        BYTE pos_condcd    [1];
        BYTE pos_pincode   [2];
        BYTE tran_fee      [9];
		BYTE settle_fee      [9];
		BYTE transaction_processing_fee [9];
		BYTE settle_processing_fee [9];
		BYTE acq_inst_len  [2];
        BYTE acq_inst_id   [11];
        BYTE fwd_inst_len  [2];
        BYTE fwd_inst_id   [11];
        BYTE track2_len    [2];
        BYTE track2        [37];
        BYTE rrn           [12];
        BYTE authcode      [6];
        BYTE responsecd    [2];
        BYTE termid        [8];
        BYTE cardaccid     [15];
        BYTE cardaccloc    [40];
        BYTE addl_rsp_len  [2];
        BYTE addlrspdata   [25];
        BYTE track1_len    [2];
        BYTE track1        [76];
        BYTE addl_data_len [3];
        BYTE addl_data     [100];
        BYTE tran_curr_cd  [3];
        BYTE settle_cur_cd [3];
        BYTE card_curr_cd  [3];
        BYTE pinblock      [8];
		BYTE sec_cntl      [16];  // ThoughtFocus Girija Y
        BYTE addl_len      [3];
        BYTE addlamts      [120];
        BYTE iccdata_len   [3];
        BYTE iccdata       [255];
		BYTE ISOdata_len   [3];
        BYTE ISOdata       [255]; //ISOdata_len
        BYTE advice_cd_len [3];
        BYTE advice_code   [60];
        BYTE pos_data_len  [3];
        BYTE pos_data      [26];
        BYTE inf_data_len  [3];
        BYTE inf_data      [100];
        BYTE banknet_len   [3];
        BYTE banknet       [50];
        BYTE nmi_code      [3];
        BYTE action_date   [6];
        BYTE orig_data     [42];
        BYTE file_code     [1];
        BYTE service_ind   [7];
        BYTE replace_amt   [42];
        BYTE security_code [8];
        BYTE recv_len      [2];
        BYTE recvid        [11];
		BYTE filename_len  [2];
        BYTE filename      [17];
		BYTE acct1_len     [2];
        BYTE acct1         [28];
        BYTE acct2_len     [2];
        BYTE acct2         [28];
		BYTE dpdata_len		[3];
		BYTE dpdata			[999];
		BYTE tl_data_len	[3];
		BYTE tl_data		[999];
		BYTE de108_len		[3];
		BYTE de108			[999];
		BYTE addldata_nation_len [3];
		BYTE addldata_nation   [200];  
		BYTE private_use2_len 	[3];
		BYTE private_use2  		[999];
		BYTE trandesc_len  [3];
        BYTE trandesc      [100];
        BYTE orig_msg_len  [3];
        BYTE orig_msg_id   [1000];
        BYTE mc_agent_len  [3];
        BYTE mc_agent_id   [11];
        BYTE addlrec_len   [3];
        BYTE addlrec       [1000];
		BYTE memData_len   [3];
        BYTE memData       [200];
        BYTE priv127_len   [3];
        BYTE priv127       [100];
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
CHAR  iccdata[256]={0};

BYTE   ReqRsp_Buffer [COMM_BUF_SZE];

char  A2E[4] = "A2E";    /*  ASCII to EBCDIC conversion */
char  E2A[4] = "E2A";    /*  EBCDIC to ASCII conversion */





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

enum bitfields {
   bf_done           = 0,      /* end of list                          */
   bf_1xbitfield     = 1,      /* extended bit field                   */
   bf_2pan           = 2,      /* pan                                  */
   bf_3proc          = 3,      /* processing code                      */
   bf_4amount        = 4,      /* amount                               */
   bf_5settleamt     = 5,      /* settle amount                        */
   bf_6chbillgamt    = 6,      /* cardholder billing amount            */
   bf_7trntime       = 7,      /* transmission date-time               */
   bf_8chbillingfee  = 8,      /* CH fee               */
   bf_9convratesetl  = 9,      /* conversion rate, settlement          */
   bf_10convrate     = 10,     /* Conversion rate, cardholder billing  */
   bf_11stan         = 11,     /* system trace number                  */
   bf_12time         = 12,     /* time                                 */
   bf_13date         = 13,     /* date, trans                          */
   bf_14expiry       = 14,     /* expiry date                          */
   bf_15datestl      = 15,     /* settlement date                      */
   bf_16convdate     = 16,     /* conversion date                      */
   bf_17capturedate  = 17,     /* conversion date                      */
   bf_18sicnumber    = 18,     /* Merchant's type of business          */
   bf_19ctrycd       = 19,     /* Acquiring institution's country code */
   bf_20panctrycd    = 20,     /* Identifies country of card issuer    */
   bf_21frwrdinstctrycd = 21,
   bf_22pose         = 22,     /* pos entry mode                       */
   bf_23csn          = 23,     /* Card Sequence Number                 */
   bf_24nii          = 24,     /* nii                                  */
   bf_25pocc         = 25,     /* pos condition code                   */
   bf_26pincode      = 26,     /* pos pin capture code                 */
   bf_28feeamount	 = 28,	   /* Txx fee amount					   */ 
   bf_29settlefee    = 29,     /* settelment fee*/
   bf_30transactionprocessingfee = 30,
   bf_31settleprocessingfee = 31, /* settelment processing fee */
   bf_32idcode       = 32,     /* Acquiring institution's ID code      */
   bf_33fwdid        = 33,     /* Forwarding institution's ID code     */
   bf_35track2       = 35,     /* track 2 data                         */
   bf_37rrn          = 37,     /* retrieval reference number           */
   bf_38authcode     = 38,     /* auth code                            */
   bf_39respcode     = 39,     /* response code                        */
   bf_41termid       = 41,     /* terminal id                          */
   bf_42cardacc      = 42,     /* card acceptor id                     */
   bf_43cardaccloc   = 43,     /* card acceptor name / location        */
   bf_44respdata     = 44,     /* additional response data             */
   bf_45track1       = 45,     /* track 1 data                         */
   bf_48addldata     = 48,     /* additional data                      */
   bf_49trncurcd     = 49,     /* transaction currency code            */
   bf_50stlmtcurrcd  = 50,     /* settlement currency code             */
   bf_51crdcurcd     = 51,     /* card currency code                   */
   bf_52pinblock     = 52,     /* pin block                            */
   bf_53seccntl	     = 53,	   /* Security Related control information */ // ThoughtFocus Phani
   bf_54addlamts     = 54,     /* additional amounts                   */
   bf_55iccdata      = 55,     /* .Integrated Circuit Card (ICC) System-related Data*/
   bf_56ISOuse		 = 56,
   bf_60advicecode   = 60,     /* advice reason code                   */
   bf_61posdata      = 61,     /* POS data                             */
   bf_62inf          = 62,     /* Intermediate Network Facility data   */
   bf_63banknet      = 63,     /* BankNet data                         */
   bf_70nmicode      = 70,     /* network management information code  */
   bf_73actiondate   = 73,     /* Date, action                         */
   bf_90orgdata      = 90,     /* original data elements               */
   bf_91filecode     = 91,     /* file update code                     */
   bf_94servind      = 94,     /* service indicator                    */
   bf_95repl_amt     = 95,     /* replacement amount                   */
   bf_96securcode    = 96,     /* message security code                */
   bf_100recvid      = 100,    /* Receiving institution Id code        */
   bf_101filename    = 101,    /* file name                            */
   bf_102acct1       = 102,    /* account identification 1             */
   bf_103acct2       = 103,    /* account identification 2             */
   bf_104dpdata      = 104,    /* transaction link identifier		   */
   bf_105tldata      = 105,    /* transaction link identifier		   */
   bf_108moneySend   = 108,    /* Money send data 	*/ 
   bf_112addldata	 = 112,
   bf_119privateuse2 = 119,	   /* Additional data-Private use2			*/
   bf_120orgmsgid    = 120,    /* original message type id             */
   bf_121mcagent     = 121,    /* authorizing agent id code            */
   bf_122addlrec     = 122,    /* additional record data               */
   bf_124memData      = 124,    /* additional record data               */
   bf_127privdata    = 127     /* private data                         */
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

#define  CHART_SIZE  89

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
         0x6F, 0x7A, 0x7B, 0x7C, 0x7D, 0x7E, 0x7F, 0x5F, 0x9A
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
   pBYTE           p_bd_pdata;                           /* ptr to data to move */
   INT             bd_fieldlen;                          /* # of bytes in iso8583 field */
   INT             (*bd_BFCond)(enum bitfields, BYTE);  /* ptr to conditional function */
   void            (*bd_BFMove)(enum bitfields);         /* ptr to move function */
   void            (*bd_BFRsp)(enum bitfields);          /* ptr to response handler function */
   };

/**********************************
      FUNCTION PROTOTYPES
**********************************/
/* functions local */

//For MCRD
void    vEBCDICMove( enum bitfields fieldid );
INT		Move_IfThere(enum bitfields, BYTE);
INT		Move_Always(enum bitfields, BYTE);
void	GenericMove(enum bitfields fieldid);
void	RspMoveGeneric(enum bitfields fieldid);
void	RspMovevData (enum bitfields fieldid);
int		get_variable_length( unsigned char *DataField, int FieldLen );
void	vDataMove(enum bitfields fieldid);
void    parsed_mssg();
INT		mcrdutil_parse_host_response_msg();
void    EBCDICMove( enum bitfields fieldid );
void	RspEBCDICvMove (enum bitfields fieldid);
void	RspEBCDICMove (enum bitfields fieldid);
INT		CondExpd( enum bitfields bitfield, BYTE p_trantype );
void	RspIgnore(enum bitfields fieldid);
INT		CondTrack1( enum bitfields bitfield, BYTE p_trantype );
void	vRspIgnore(enum bitfields fieldid);
void	Generate_CUP_EMV_Details();
void	genutil_EMV_To_Auth_Tx( BYTE * emv_raw, INT total_len, pAUTH_TX p_auth_tx );
void	genutil_bcd_to_asc (BYTE bstring[], INT blen, pCHAR astr, INT astr_len);
//INT		Move_Never(enum bitfields, BYTE);
//INT		vMove_IfThere(enum bitfields, BYTE);

//BYTE	getFldSep( unsigned char* resp_buff, int index );
//void	find_cvc_flag( char *, char *, char *);



void    parsed_mssg();
//INT     parse_host_msg( pCHAR err_buf );
BYTE	ascii_ebcdic_conv(BYTE conv_char, char * conv_type );
BYTE	ascii_ebcdic_conv_trk1( unsigned char conv_char, char *conv_type,unsigned char fldSep);




//static  struct bitfield_data *BFData; 
// From Cup
static  struct bitfield_data BFData[] = {
   {
   bf_2pan,                      /* field */
   HOST_MESSAGE.pan_len,         /* data to move */
   2,                            /* # of bytes in pan len field */
   Move_IfThere,                 /* conditional  */
   vEBCDICMove,                  /* move routine */
   RspEBCDICvMove                /* response routine */
   },
   {
   bf_3proc,                     /* field */
   HOST_MESSAGE.proccode,        /* data to move */
   6,                            /* # of bytes in mcrd iso field */
   Move_Always,                  /* mandatory */
   EBCDICMove,                   /* move routine */
   RspEBCDICMove                 /* response routine */
   },
   {
   bf_4amount,                   /* field */
   HOST_MESSAGE.amount,          /* data to move */
   12,                           /* # of bytes in mcrd iso field */
   Move_Always,                  /* mandatory */
   EBCDICMove,                   /* move routine */
   RspEBCDICMove                 /* response routine */
   },
   {
   bf_5settleamt,                /* field */
   HOST_MESSAGE.settle_amt,      /* data to move */
   12,                           /* # of bytes in mcrd iso field */
   Move_IfThere,                 /* conditional */
   EBCDICMove,                   /* move routine */
   RspEBCDICMove                 /* response routine */
   },
   {
   bf_6chbillgamt,               /* field */
   HOST_MESSAGE.ch_bill_amt,     /* data to move */
   12,                           /* # of bytes in mcrd iso field */
   Move_IfThere,                 /* conditional */
   EBCDICMove,                   /* move routine */
   RspEBCDICMove                 /* response routine */
   },
   {
   bf_7trntime,                  /* field */
   HOST_MESSAGE.gmt_time,        /* data to move */
   10,                           /* # of bytes in mcrd iso field */
   Move_Always,                  /* conditional */
   EBCDICMove,                   /* move routine */
   RspEBCDICMove                 /* response routine */
   },
   {
   bf_8chbillingfee,                  /* field */
   HOST_MESSAGE.ch_bill_fee,        /* data to move */
   8,                           /* # of bytes in mcrd iso field */
   Move_Always,                  /* conditional */
   EBCDICMove,                   /* move routine */
   RspEBCDICMove                 /* response routine */
   },
   {
   bf_9convratesetl,             /* field */
   HOST_MESSAGE.conv_rate_stl,   /* data to move */
   8,                            /* # of bytes in mcrd iso field */
   Move_IfThere,                 /* conditional */
   EBCDICMove,                   /* move routine */
   RspEBCDICMove                 /* response routine */
   },
   {
   bf_10convrate,                /* field */
   HOST_MESSAGE.conv_rate_chb,   /* data to move */
   8,                            /* # of bytes in mcrd iso field */
   Move_IfThere,                 /* conditional */
   EBCDICMove,                   /* move routine */
   RspEBCDICMove                 /* response routine */
   },
   {
   bf_11stan,                    /* field */
   HOST_MESSAGE.stan,            /* data to move */
   6,                            /* # of bytes in mcrd iso field */
   Move_Always,                  /* conditional */
   EBCDICMove,                   /* move routine */
   RspEBCDICMove                 /* response routine */
   },
   {
   bf_12time,                    /* field */
   HOST_MESSAGE.time,            /* data to move */
   6,                            /* # of bytes in mcrd iso field */
   Move_IfThere,                 /* conditional */
   EBCDICMove,                   /* move routine */
   RspEBCDICMove                     /* response routine */
   },
   {
   bf_13date,                    /* field */
   HOST_MESSAGE.date,            /* data to move */
   4,                            /* # of bytes in mcrd iso field */
   Move_IfThere,                 /* conditional */
   EBCDICMove,                   /* move routine */
   RspEBCDICMove                     /* response routine */
   },
   {
   bf_14expiry,                  /* field */
   HOST_MESSAGE.date_expd,       /* data to move */
   4,                            /* # of bytes in mcrd iso field */
   CondExpd,                     /* conditional */
   EBCDICMove,                   /* move routine */
   RspEBCDICMove                 /* response routine */
   },
   {
   bf_15datestl,                 /* field */
   HOST_MESSAGE.date_settle,     /* data to move */
   4,                            /* # of bytes in mcrd iso field */
//   Move_Always,				 /* mandatory */					// ThoughtFocus Girija Y Commented
   Move_IfThere,			     /* conditional */					// ThoughtFocus Girija Y, Change Field 15 as Conditional not as Mandatory 
   EBCDICMove,                   /* move routine */
   RspEBCDICMove                 /* response routine */
   },
   {
   bf_16convdate,                /* field */
   HOST_MESSAGE.conv_date,       /* data to move */
   4,                            /* # of bytes in mcrd iso field */
   Move_IfThere,                 /* conditional */
   EBCDICMove,                   /* move routine */
   RspEBCDICMove                 /* response routine */
   },
   {
   bf_17capturedate,                /* field */
   HOST_MESSAGE.capture_date,       /* data to move */
   4,                            /* # of bytes in mcrd iso field */
   Move_IfThere,                 /* conditional */
   EBCDICMove,                   /* move routine */
   RspEBCDICMove                 /* response routine */
   },
   {
   bf_18sicnumber,               /* field */
   HOST_MESSAGE.sic_number,      /* data to move */
   4,                            /* # of bytes in mcrd iso field */
   Move_Always,                  /* mandatory */
   EBCDICMove,                   /* move routine */
   RspEBCDICMove                 /* response routine */
   },
   {
   bf_19ctrycd,                  /* field */
   HOST_MESSAGE.ctry_code,       /* data to move */
   3,                            /* # of bytes in mcrd iso field */
   Move_Always,                  /* mandatory    */
   EBCDICMove,                   /* move routine */
   RspEBCDICMove                     /* response routine */
   },
   {
   bf_20panctrycd,               /* field */
   HOST_MESSAGE.pan_ctry_cd,     /* data to move */
   3,                            /* # of bytes in mcrd iso field */
   Move_IfThere,                 /* conditional */
   EBCDICMove,                   /* move routine */
   RspEBCDICMove                 /* response routine */
   },
   {
   bf_21frwrdinstctrycd,               /* field */
   HOST_MESSAGE.frwrdinst_ctry_cd,     /* data to move */
   3,                            /* # of bytes in mcrd iso field */
   Move_IfThere,                 /* conditional */
   EBCDICMove,                   /* move routine */
   RspEBCDICMove                 /* response routine */
   },
   {
   bf_22pose,                    /* field */
   HOST_MESSAGE.posentry,        /* data to move */
   3,                            /* # of bytes in mcrd iso field */
   Move_Always,                  /* conditional */
   EBCDICMove,                   /* move routine */
   RspEBCDICMove                 /* response routine */
   },
   {
   bf_23csn ,                    /* field */
   HOST_MESSAGE.card_seq_num,    /* data to move */
   3,                            /* # of bytes in mcrd iso field */
   Move_IfThere,                 /* conditional */
   EBCDICMove,                   /* move routine */
   RspEBCDICMove                 /* response routine */
   },
   {
   bf_24nii,                     /* field */
   HOST_MESSAGE.nii,             /* data to move */
   3,                            /* # of bytes in mcrd iso field */
   Move_Always,                  /* conditional */
   EBCDICMove,                   /* move routine */
   RspEBCDICMove                     /* response routine */
   },
   {
   bf_25pocc,                    /* field */
   HOST_MESSAGE.pos_condcd,      /* data to move */
   2,                            /* # of bytes in mcrd iso field */
   Move_Always,                  /* conditional */
   EBCDICMove,                   /* move routine */
   RspEBCDICMove                     /* response routine */
   },
   {
   bf_26pincode,                 /* field */
   HOST_MESSAGE.pos_pincode,      /* data to move */
   2,                            /* # of bytes in mcrd iso field */
   Move_IfThere,                 /* conditional */
   EBCDICMove,                   /* move routine */
   RspEBCDICMove                     /* response routine */
   },
    {
   bf_28feeamount,                 /* field */
   HOST_MESSAGE.tran_fee,      /* data to move */
   9,                            /* # of bytes in mcrd iso field */
   Move_IfThere,                 /* conditional */
   EBCDICMove,                   /* move routine */
   RspEBCDICMove                     /* response routine */
   },
   {
   bf_29settlefee,                 /* field */
   HOST_MESSAGE.settle_fee,      /* data to move */
   9,                            /* # of bytes in mcrd iso field */
   Move_IfThere,                 /* conditional */
   EBCDICMove,                   /* move routine */
   RspEBCDICMove                     /* response routine */
   }, 
   {
   bf_30transactionprocessingfee,                 /* field */
   HOST_MESSAGE.transaction_processing_fee,      /* data to move */
   9,                            /* # of bytes in mcrd iso field */
   Move_IfThere,                 /* conditional */
   EBCDICMove,                   /* move routine */
   RspEBCDICMove                     /* response routine */
   },
   {
   bf_31settleprocessingfee,                 /* field */
   HOST_MESSAGE.settle_processing_fee,      /* data to move */
   9,                            /* # of bytes in mcrd iso field */
   Move_IfThere,                 /* conditional */
   EBCDICMove,                   /* move routine */
   RspEBCDICMove                     /* response routine */
   },
   {
   bf_32idcode,                  /* field */
   HOST_MESSAGE.acq_inst_len,    /* data to move */
   2,                            /* # of bytes in mcrd iso field */
   Move_Always,                  /* conditional */
   vEBCDICMove,                  /* move routine */
   RspEBCDICvMove                /* response routine */
   },
   {
   bf_33fwdid,                   /* field */
   HOST_MESSAGE.fwd_inst_len,    /* data to move */
   2,                            /* # of bytes in mcrd iso field */
   Move_IfThere,                 /* conditional */
   vEBCDICMove,                  /* move routine */
   RspEBCDICvMove                /* response routine */
   },
   {
   bf_35track2,                  /* field */
   HOST_MESSAGE.track2_len,      /* data to move */
   2,                            /* # of bytes in track2 len field */
   Move_IfThere,                 /* conditional */
   vEBCDICMove,                  /* move routine */
   RspEBCDICvMove                /* response routine */
   },
   {
   bf_37rrn,                     /* field */
   HOST_MESSAGE.rrn,             /* data to move */
   12,                           /* # of bytes in mcrd iso field */
   Move_IfThere,                 /* conditional */
   EBCDICMove,                   /* move routine */
   RspEBCDICMove                 /* response routine */
   },
   {
   bf_38authcode,                /* field */
   HOST_MESSAGE.authcode,        /* data to move */
   6,                            /* # of bytes in mcrd iso field */
   Move_IfThere,                 /* conditional */
   EBCDICMove,                   /* move routine */
   RspEBCDICMove                 /* response routine */
   },
   {
   bf_39respcode,                /* field */
   HOST_MESSAGE.responsecd,      /* data to move */
   2,                            /* # of bytes in mcrd iso field */
   Move_IfThere,                 /* conditional */
   EBCDICMove,                   /* move routine */
   RspEBCDICMove                 /* response routine */
   },
   {
   bf_41termid,                  /* field */
   HOST_MESSAGE.termid,          /* data to move */
   8,                            /* # of bytes in mcrd iso field */
   Move_IfThere,                 /* conditional */
   EBCDICMove,                   /* move routine */
   RspEBCDICMove                 /* response routine */
   },
   {
   bf_42cardacc,                 /* field */
   HOST_MESSAGE.cardaccid,       /* data to move */
   15,                           /* # of bytes in mcrd iso field */
   Move_Always,                  /* conditional */
   EBCDICMove,                   /* move routine */
   RspEBCDICMove                 /* response routine */
   },
   {
   bf_43cardaccloc,              /* field */
   HOST_MESSAGE.cardaccloc,      /* data to move */
   40,                           /* # of bytes in mcrd iso field */
   Move_IfThere,                 /* conditional */
   EBCDICMove,                   /* move routine */
   RspEBCDICMove                 /* response routine */
   },
   {
   bf_44respdata,                /* field */
   HOST_MESSAGE.addl_rsp_len,    /* data to move */
   2,                            /* # of bytes in resp data len field */
   Move_IfThere,                 /* conditional */
   vEBCDICMove,                  /* move routine */
   RspEBCDICvMove                /* response routine */
   },
   {
   bf_45track1,                  /* field */
   HOST_MESSAGE.track1_len,      /* data to move */
   2,                            /* # of bytes in track1 len field */
   CondTrack1,                   /* conditional */
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
   3,                            /* # of bytes in mcrd iso field */
   Move_Always,                  /* mandatory    */
   EBCDICMove,                   /* move routine */
   RspEBCDICMove                 /* response routine */
   },
   {
   bf_50stlmtcurrcd,             /* field */
   HOST_MESSAGE.settle_cur_cd,   /* data to move */
   3,                            /* # of bytes in mcrd iso field */
   Move_IfThere,                 /* conditional  */
   EBCDICMove,                   /* move routine */
   RspEBCDICMove                 /* response routine */
   },
   {
   bf_51crdcurcd,                /* field */
   HOST_MESSAGE.card_curr_cd,    /* data to move */
   3,                            /* # of bytes in mcrd iso field */
   Move_IfThere,                 /* conditional */
   EBCDICMove,                   /* move routine */
   RspEBCDICMove                 /* response routine */
   },
   {
   bf_52pinblock,                /* field */
   HOST_MESSAGE.pinblock,        /* data to move */
   8,                            /* # of bytes in mcrd iso field */
   Move_IfThere,                 /* conditional */
   GenericMove,                  /* move routine */
   RspMoveGeneric                /* response routine */
   },
/**************************** ThoughtFocus Girija Y ***************************/
   {
   bf_53seccntl,                 /* field */
   HOST_MESSAGE.sec_cntl,        /* data to move */
   16,                            /* # of bytes in MC field */
   Move_IfThere,                 /* conditional */
   GenericMove,                  /* move routine */
   RspMoveGeneric                /* response routine */
   },
/******************************************************************************/
   {
   bf_54addlamts,                /* field */
   HOST_MESSAGE.addl_len,        /* data to move */
   3,                            /* # of bytes in addl len field */
   Move_IfThere,                 /* conditional */
   vEBCDICMove,                  /* move routine */
   RspEBCDICvMove                /* response routine */
   },
   {
   bf_55iccdata,                 /* field */
   HOST_MESSAGE.iccdata_len,     /* data to move */
   3,                            /* # of bytes in addl len field */
   Move_IfThere,                 /* conditional */
   vDataMove,                    /* move routine */
   RspMovevData                  /* response routine */
   },
   {
	bf_56ISOuse,                 /* field */
    HOST_MESSAGE.ISOdata_len,     /* data to move */
    3,                            /* # of bytes in addl len field */
	Move_IfThere,                 /* conditional */
	vDataMove,                    /* move routine */
	RspEBCDICvMove                  /* response routine */
	},
   {
   bf_60advicecode,              /* field */
   HOST_MESSAGE.advice_cd_len,   /* data to move */
   3,                            /* # of bytes in addl len field */
   Move_Always,                  /* mandatory */
   vEBCDICMove,                  /* move routine */
   RspEBCDICvMove                /* response routine */
   },
   {
   bf_61posdata,                 /* field */
   HOST_MESSAGE.pos_data_len,    /* data to move */
   3,                            /* # of bytes in addl len field */
   Move_Always,                  /* mandatory */
   vEBCDICMove,                  /* move routine */
   RspEBCDICvMove                /* response routine */
   },
   {
   bf_62inf,                     /* field */
   HOST_MESSAGE.inf_data_len,    /* data to move */
   3,                            /* # of bytes in addl len field */
   Move_IfThere,                 /* conditional */
   vEBCDICMove,                  /* move routine */
   RspEBCDICvMove                /* response routine */
   },
   {
   bf_63banknet,                 /* field */
   HOST_MESSAGE.banknet_len,     /* data to move */
   3,                            /* # of bytes in addl len field */
   Move_IfThere,                 /* conditional  */
   vEBCDICMove,                  /* move routine */
   RspEBCDICvMove                /* response routine */
   },
   {
   bf_70nmicode,                 /* field */
   HOST_MESSAGE.nmi_code,        /* data to move */
   3,                            /* # of bytes in mcrd iso field */
   Move_Always,                  /* mandatory    */
   EBCDICMove,                   /* move routine */
   RspEBCDICMove                 /* response routine */
   },
   {
   bf_73actiondate,              /* field */
   HOST_MESSAGE.action_date,     /* data to move */
   6,                            /* # of bytes in mcrd iso field */
   Move_IfThere,                 /* conditional  */
   EBCDICMove,                   /* move routine */
   RspEBCDICMove                     /* response routine */
   },
   { /* NOTE: Field 90 is no longer used in any message except 0400 and 0420 msg */
   bf_90orgdata,                 /* field */
   HOST_MESSAGE.orig_data,       /* data to move */
   42,                           /* # of bytes in mcrd iso field */
   Move_Always,                  /* mandatory    */
   EBCDICMove,                   /* move routine */
   RspEBCDICMove                 /* response routine */
   },
   {
   bf_91filecode,                /* field */
   HOST_MESSAGE.file_code,       /* data to move */
   1,                            /* # of bytes in mcrd iso field */
   Move_Always,                  /* mandatory    */
   EBCDICMove,                   /* move routine */
   RspEBCDICMove                 /* response routine */
   },
   {
   bf_94servind,                 /* field */
   HOST_MESSAGE.service_ind,     /* data to move */
   7,                            /* # of bytes in mcrd iso field */
   Move_Always,                  /* mandatory    */
   EBCDICMove,                   /* move routine */
   RspEBCDICMove                 /* response routine */
   },
   {
   bf_95repl_amt,                /* field */
   HOST_MESSAGE.replace_amt,     /* data to move */
   42,                           /* # of bytes in mcrd iso field */
   Move_IfThere,                 /* conditional  */
   EBCDICMove,                   /* move routine */
   RspEBCDICMove                 /* response routine */
   },
   {
   bf_96securcode,               /* field */
   HOST_MESSAGE.security_code,   /* data to move */
   8,                            /* # of bytes in mcrd iso field */
   Move_Always,                  /* mandatory    */
   EBCDICMove,                   /* move routine */
   RspEBCDICMove                 /* response routine */
   },
   {
   bf_100recvid,                 /* field */
   HOST_MESSAGE.recv_len,        /* data to move */
   2 ,                           /* # of bytes in mcrd iso field */
   Move_IfThere,                 /* conditional  */
   vEBCDICMove,                  /* move routine */
   RspEBCDICvMove                /* response routine */
   },
   {
   bf_101filename,               /* field */
   HOST_MESSAGE.filename_len,    /* data to move */
   2 ,                           /* # of bytes in mcrd iso field */
   Move_Always,                  /* mandatory    */
   vEBCDICMove,                  /* move routine */
   RspEBCDICvMove                /* response routine */
   },
   {
   bf_102acct1,                  /* field */
   HOST_MESSAGE.acct1_len,       /* data to move */
   2 ,                           /* # of bytes in mcrd iso field */
   Move_Always,                  /* mandatory    */
   vEBCDICMove,                  /* move routine */
   RspEBCDICvMove                    /* response routine */
   },
   {
   bf_103acct2,                  /* field */
   HOST_MESSAGE.acct2_len,       /* data to move */
   2,                            /* # of bytes in mcrd iso field */
   Move_Always,                  /* mandatory    */
   vEBCDICMove,                  /* move routine */
   RspEBCDICvMove                    /* response routine */
   },
   {
   bf_104dpdata,                  /* field */
   HOST_MESSAGE.dpdata_len,       /* data to move */
   3,                            /* # of bytes in mcrd iso field */
   Move_IfThere,                 /* conditional  */
   vEBCDICMove,                  /* move routine */
   RspEBCDICvMove                /* response routine */   
   },
   {
   bf_105tldata,                  /* field */
   HOST_MESSAGE.tl_data_len,       /* data to move */
   3,                            /* # of bytes in mcrd iso field */
   Move_IfThere,                 /* conditional  */
   vEBCDICMove,                  /* move routine */
   RspEBCDICvMove                /* response routine */   
   },
   {
   bf_108moneySend,                  /* field */
   HOST_MESSAGE.de108_len,       /* data to move */
   3,                            /* # of bytes in mcrd iso field */
   Move_IfThere,                 /* conditional  */
   vEBCDICMove,                  /* move routine */
   RspEBCDICvMove                /* response routine */   
   },
   {
   bf_112addldata,                  /* field */
   HOST_MESSAGE.addldata_nation_len,       /* data to move */
   3,                            /* # of bytes in mcrd iso field */
   Move_IfThere,                 /* conditional  */
   vEBCDICMove,                  /* move routine */
   RspEBCDICvMove                /* response routine */   
   },
   {
   bf_119privateuse2,            /* field */
   HOST_MESSAGE.private_use2_len,       /* data to move */
   3,                            /* # of bytes in mcrd iso field */
   Move_IfThere,                 /* conditional  */
   vEBCDICMove,                  /* move routine */
   RspEBCDICvMove                /* response routine */
   },
   {
   bf_120orgmsgid,               /* field */
   HOST_MESSAGE.orig_msg_len,    /* data to move */
   3,                            /* # of bytes in mcrd iso field */
   Move_IfThere,                 /* conditional  */
   vEBCDICMove,                  /* move routine */
   RspEBCDICvMove                /* response routine */
   },
   {
   bf_121mcagent,                /* field */
   HOST_MESSAGE.mc_agent_len,    /* data to move */
   3,                            /* # of bytes in mcrd iso field */
   Move_IfThere,                 /* conditional  */
   vEBCDICMove,                  /* move routine */
   RspEBCDICvMove                /* response routine */
   },
   {
   bf_122addlrec,                /* field */
   HOST_MESSAGE.addlrec_len,     /* data to move */
   3,                            /* # of bytes in mcrd iso field */
   Move_IfThere,                 /* conditional  */
   vEBCDICMove,                  /* move routine */
   RspEBCDICvMove                /* response routine */
   },
   {
   bf_124memData,                /* field */
   HOST_MESSAGE.memData_len,     /* data to move */
   3,                            /* # of bytes in mcrd iso field */
   Move_IfThere,                 /* conditional  */
   vEBCDICMove,                  /* move routine */
   RspEBCDICvMove                /* response routine */
   },
   {
   bf_127privdata,               /* field */
   HOST_MESSAGE.priv127_len,     /* data to move */
   3,                            /* # of bytes in mcrd iso field */
   Move_IfThere,                 /* conditional  */
   vEBCDICMove,                  /* move routine */
   RspEBCDICvMove                /* response routine */
   },

   {
   bf_done,                      /* field */
   NULL,                         /* data to move */
   99,                           /* # of bytes in mcrd iso field */
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
void GenericMove(enum bitfields fieldid)
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
void RspMoveGeneric(enum bitfields fieldid)
{
   struct bitfield_data *pBF;


   /*---------------------------------------------------*/
   /* find pointer to bitfield info for this bitfield...*/
        /*---------------------------------------------------*/
   pBF = Find_Bitfield_Table(fieldid);
   if (pBF != NULL)
   {
      if ( (reqidx + pBF->bd_fieldlen) <= COMM_BUF_SZE )
      {
         memcpy(pBF->p_bd_pdata,         /* to data pointer      */
                &ReqRsp_Buffer[reqidx],      /* move from request buffer */
                pBF->bd_fieldlen);      /* for length of field    */

         reqidx = reqidx + pBF->bd_fieldlen;   /* increment buffer pointer */
      }
      else
         invalid_msg_format = true;
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

/******************************************************************************
 *
 *  NAME:         vDataMove
 *
 *  DESCRIPTION:  This function moves a variable length string into
 *                ReqRsp_Buffer.  The length is also copied, but converted
 *                into EBCDIC format.  The data is not converted.
 *
 *  INPUTS:       fieldid - Field Id to denote where the source data exists
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   None
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
void vDataMove( enum bitfields fieldid )
{
   struct bitfield_data *pBF;
   int                  movelen;
   int                  i;
   unsigned char        cFieldData;

   /*---------------------------------------------------*/
   /* find pointer to bitfield info for this bitfield...*/
   /*---------------------------------------------------*/
   pBF = Find_Bitfield_Table(fieldid);
   if (pBF != NULL)
   {
      movelen  = get_variable_length( pBF->p_bd_pdata, pBF->bd_fieldlen );

      /* Convert length to EBCDIC and move to ReqRsp_Buffer */
      for( i=0; i<pBF->bd_fieldlen; i++ )
      {
         cFieldData =ascii_ebcdic_conv((unsigned char)(pBF->p_bd_pdata[i]),A2E);
         memset(&ReqRsp_Buffer[reqidx], cFieldData, 1);
         reqidx++;
      }

      /* Move the rest of the data; do not convert it. */
      memcpy(&ReqRsp_Buffer[reqidx], pBF->p_bd_pdata+pBF->bd_fieldlen, movelen);
      reqidx += movelen;
   }

} /* end vDataMove */


/******************************************************************************
 *
 *  NAME:         RspMovevData
 *
 *  DESCRIPTION:  This function moves a variable length string from
 *                ReqRsp_Buffer into HOST_MESSAGE.  The length field
 *                is in EBCDIC and must be converted. But the data is
 *                in various formats and is moved without any conversion.
 *
 *  INPUTS:       fieldid - Field Id to denote the destination of the data
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   None
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
void RspMovevData (enum bitfields fieldid)
{
   struct bitfield_data *pBF;
   INT                  i;
   INT                  FieldLen;
   double               movelen;
   unsigned char        cLen;
   unsigned char        cFieldData;

   /*---------------------------------------------------*/
   /* Find pointer to bitfield info for this bitfield...*/
   /*---------------------------------------------------*/
   pBF = Find_Bitfield_Table(fieldid);
   if (pBF != NULL)
   {
      FieldLen = pBF->bd_fieldlen;

      /* Determine number of bytes to move. */
      for( i=0,movelen=0; i<FieldLen; i++ )
      {
         cLen = ascii_ebcdic_conv( ReqRsp_Buffer[reqidx+i], E2A );
         movelen += (int)(cLen - 0x30) * (pow(10,(double)(FieldLen-i-1)));
      }

      /* Convert length from EBCDIC and move to HOST_MESSAGE. */
      for( i=0; i<FieldLen; i++,reqidx++ )
      {
         cFieldData = ascii_ebcdic_conv( (unsigned char)(ReqRsp_Buffer[reqidx]), E2A );
         memset(&pBF->p_bd_pdata[i], cFieldData, 1);
      }

      if ( (reqidx + movelen) <= COMM_BUF_SZE )
      {
         /* Move the rest of the data; do not convert it. */
         memcpy( pBF->p_bd_pdata+FieldLen, ReqRsp_Buffer+reqidx, (INT)movelen );
         reqidx += (INT)movelen;
      }
      else
         invalid_msg_format = true;
   }

} /* end RspEBCDICvMove */
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


/******************************************************************************
 *
 *  NAME:         CondTrack1
 *
 *  DESCRIPTION:  This function copies the track 1 field from
 *                HOST_MESSAGE into ReqRsp_Buffer if the track 1 field
 *                has data.
 *
 *  INPUTS:       fieldid - Field Id to denote the source field of the data
 *                p_trantype - transaction type
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   True if data was moved, else false
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
INT CondTrack1( enum bitfields bitfield, BYTE p_trantype )
{
   if ( 0x00 != HOST_MESSAGE.track1_len [0] )
       return( true );

   return( false );
} /* end CondTrack1 */

/******************************************************************************
 *
 *  NAME:         vRspIgnore - Do not copy variable length response field
 *
 *  DESCRIPTION:  This function is called when a host response is being parsed
 *                and we do not care to copy the contents of the field into
 *                the HOST_MESSAGE structure from the ReqRsp_Buffer.  The only
 *                thing that happens here is the ReqRsp_Buffer is incremented
 *                to point past this data.
 *
 *  INPUTS:       fieldid - Field Id to denote the destination of the data
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   None
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
void vRspIgnore(enum bitfields fieldid)
{
   struct bitfield_data *pBF;
   int                  i;
   int                  FieldLen;
   double               movelen;
   unsigned char        cLen;

   /*---------------------------------------------------*/
   /* Find pointer to bitfield info for this bitfield...*/
   /*---------------------------------------------------*/
   pBF = Find_Bitfield_Table(fieldid);
   if (pBF != NULL)
   {
      FieldLen = pBF->bd_fieldlen;

      if ( (reqidx + FieldLen) <= COMM_BUF_SZE )
      {
         /* Determine number of bytes to move. */
         for( i=0,movelen=0; i<FieldLen; i++ )
         {
            cLen = ascii_ebcdic_conv( ReqRsp_Buffer[reqidx+i], E2A );
            movelen += (int)(cLen - 0x30) * (pow(10,(double)(FieldLen-i-1)));
         }
         movelen += pBF->bd_fieldlen;

         if ( (reqidx + movelen) <= COMM_BUF_SZE )
            reqidx = reqidx + (int)movelen;   /* Increment buffer pointer */
         else
            invalid_msg_format = true;
      }
      else
      {
         invalid_msg_format = true;
      }
   }
} /* end vRspIgnore */
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
 *  RTRN VALUE:   None
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
void vEBCDICMove( enum bitfields fieldid )
{
   struct bitfield_data *pBF;
   int                  movelen;
   int                  i;
   unsigned char        cFieldData;

   /*---------------------------------------------------*/
   /* find pointer to bitfield info for this bitfield...*/
   /*---------------------------------------------------*/
   pBF = Find_Bitfield_Table(fieldid);
   if (pBF != NULL)
   {
      movelen  = get_variable_length( pBF->p_bd_pdata, pBF->bd_fieldlen );
      movelen += pBF->bd_fieldlen;

      for( i=0; i<movelen; i++ )
      {
         cFieldData = ascii_ebcdic_conv( (unsigned char)(pBF->p_bd_pdata[i]), A2E );
         memset(&ReqRsp_Buffer[reqidx], cFieldData, 1);
         reqidx++;
      }
   }

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
 *  RTRN VALUE:   None
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
void RspEBCDICvMove (enum bitfields fieldid)
{
   struct bitfield_data *pBF;
   int                  i;
   int                  FieldLen;
   double               movelen;
   unsigned char        cLen;
   unsigned char        cFieldData;

   /*---------------------------------------------------*/
   /* Find pointer to bitfield info for this bitfield...*/
   /*---------------------------------------------------*/
   pBF = Find_Bitfield_Table(fieldid);
   if (pBF != NULL)
   {
      FieldLen = pBF->bd_fieldlen;

      /* Determine number of bytes to move. */
      for( i=0,movelen=0; i<FieldLen; i++ )
      {
         cLen = ascii_ebcdic_conv( ReqRsp_Buffer[reqidx+i], E2A );
         movelen += (int)(cLen - 0x30) * (pow(10,(double)(FieldLen-i-1)));
      }
      movelen += pBF->bd_fieldlen;

      if ( (reqidx + movelen) <= COMM_BUF_SZE )
      {
         /* Move the data, converting it from EBCDIC to ASCII. */
         for( i=0; i<movelen; i++,reqidx++ )
         {
            cFieldData = ascii_ebcdic_conv( (unsigned char)(ReqRsp_Buffer[reqidx]), E2A );
            memset(&pBF->p_bd_pdata[i], cFieldData, 1);
         }
      }
      else
         invalid_msg_format = true;
   }

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
 *  RTRN VALUE:   None
 *
 *  AUTHOR:       Dennis Irby
 *
 *  MODIFICATIONS:
 ******************************************************************************/
void RspEBCDICMove (enum bitfields fieldid)
{
   struct bitfield_data *pBF;
   int                  i;
   int                  movelen;
   unsigned char        cFieldData;

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
         for( i=0; i<movelen; i++,reqidx++ )
         {
            cFieldData = ascii_ebcdic_conv( (unsigned char)(ReqRsp_Buffer[reqidx]), E2A );
            memset(&pBF->p_bd_pdata[i], cFieldData, 1);
         }
      }
      else
         invalid_msg_format = true;
   }
} /* end RspEBCDICMove */
/******************************************************************************
 *
 *  NAME:         CondExpd
 *
 *  DESCRIPTION:  This function copies the date expiration field from
 *                HOST_MESSAGE into ReqRsp_Buffer if the date expiration field
 *                has data.
 *
 *  INPUTS:       fieldid - Field Id to denote the source field of the data
 *                p_trantype - transaction type
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   True if data was moved, else false
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
INT CondExpd( enum bitfields bitfield, BYTE p_trantype )
{
   /* if track2 data here, no need for expdate field */
   if ( 0x00 != HOST_MESSAGE.date_expd [1] )
      return( true );

   return( false );
} /* end CondExpd */

/******************************************************************************
 *
 *  NAME:         RspIgnore - Do not copy response field
 *
 *  DESCRIPTION:  This function is called when a host response is being parsed
 *                and we do not care to copy the contents of the field into
 *                the HOST_MESSAGE structure from the ReqRsp_Buffer.  The only
 *                thing that happens here is the ReqRsp_Buffer is incremented
 *                to point past this data.
 *
 *  INPUTS:       fieldid - Field Id to denote the destination of the data
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   None
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
void RspIgnore(enum bitfields fieldid)
{
   struct bitfield_data *pBF;


   /*---------------------------------------------------*/
   /* find pointer to bitfield info for this bitfield...*/
   /*---------------------------------------------------*/
   pBF = Find_Bitfield_Table(fieldid);
   if (pBF != NULL)
   {
      if ( (reqidx + pBF->bd_fieldlen) <= COMM_BUF_SZE )
         reqidx = reqidx + pBF->bd_fieldlen;   /* increment buffer pointer */
      else
         invalid_msg_format = true;
   }
} /* end RspIgnore */

/******************************************************************************/
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
 *  RTRN VALUE:   None
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
void EBCDICMove( enum bitfields fieldid )
{
   struct bitfield_data *pBF;
   int                  movelen;
   int                  i;
   unsigned char        cFieldData;

   /*---------------------------------------------------*/
   /* find pointer to bitfield info for this bitfield...*/
   /*---------------------------------------------------*/
   pBF = Find_Bitfield_Table(fieldid);
   if (pBF != NULL)
   {
      movelen = pBF->bd_fieldlen;

      for( i=0; i<movelen; i++ )
      {
         cFieldData = ascii_ebcdic_conv( (unsigned char)(pBF->p_bd_pdata[i]), A2E );
         memset(&ReqRsp_Buffer[reqidx], cFieldData, 1);
         reqidx++;
      }
   }

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

/*********ParsedMssg() to print the amex parsed message****/
void parsed_mssg()
{
	int bitCount, startCharacter, icnt, LastDisplayBit;

	memset(Line,0,sizeof(Line));
	


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
		BYTE temp_data[100]={0};
		char buffer [100]={0};
		int len=0;
		memset((char *)temp_len,0,3);
		memset((char *)temp_data,0,100);
		memset((char *)buffer,0,100);
		switch(bitCount)
		{	
		case bf_2pan :
			//if(strlen((char *)HOST_MESSAGE.pan_len) != '\0') 
			if(BitFieldArray[bf_2pan] == 1)
			{	
				
				strncpy(temp_len,(char *)HOST_MESSAGE.pan_len,2); 
				len =atoi(temp_len);
				strcat (Line[bitCount], FieldStrings [bitCount]);
				strncat (Line[bitCount], (char *)HOST_MESSAGE.pan,len);				
			}	
		break;
		
		case bf_3proc :
			//if(strlen((char *)HOST_MESSAGE.proccode) != '\0')
			if(BitFieldArray[bf_3proc] == 1)
			{
				strcat (Line[bitCount], FieldStrings [bitCount]);
				strncat (Line[bitCount], (char *)HOST_MESSAGE.proccode, 6);				
			}
		break;
		
		case bf_4amount :
			//if(strlen((char *)HOST_MESSAGE.amount) != '\0')
			if(BitFieldArray[bf_4amount] == 1)
			{
				strcat (Line[bitCount], FieldStrings [bitCount]);
				strncat (Line[bitCount], (char *)HOST_MESSAGE.amount, 12);
			}
		break;
		
		case bf_5settleamt :
			//if(strlen((char *)HOST_MESSAGE.settle_amt) != '\0')
			if(BitFieldArray[bf_5settleamt] == 1)
			{
				strcat (Line[bitCount], FieldStrings [bitCount]);
				strncat (Line[bitCount], (char *)HOST_MESSAGE.settle_amt ,12);
			}
		break;
		
		case bf_6chbillgamt :
			//if(strlen((char *)HOST_MESSAGE.ch_bill_amt) != '\0')
			if(BitFieldArray[bf_6chbillgamt] == 1)
			{
				strcat (Line[bitCount], FieldStrings [bitCount]);
				strncat (Line[bitCount], (char *)HOST_MESSAGE.ch_bill_amt ,12);
			}
			break ;

		case bf_7trntime :
			//if(strlen((char *)HOST_MESSAGE.gmt_time) != '\0')
			if(BitFieldArray[bf_7trntime] == 1)
			{
				strcat (Line[bitCount], FieldStrings [bitCount]);
				strncat (Line[bitCount], (char *)HOST_MESSAGE.gmt_time ,10);
			}
		break;
		
		case bf_8chbillingfee :
			//if(strlen((char *)HOST_MESSAGE.ch_bill_fee) != '\0')
			if(BitFieldArray[bf_8chbillingfee] == 1)
			{
				strcat (Line[bitCount], FieldStrings [bitCount]);
				strncat (Line[bitCount], (char *)HOST_MESSAGE.ch_bill_fee ,8);
			}
		break;
		
		case bf_9convratesetl :
			//if(strlen((char *)HOST_MESSAGE.conv_rate_stl) != '\0')    
			if(BitFieldArray[bf_9convratesetl] == 1)
			{
				strcat (Line[bitCount], FieldStrings [bitCount]);
				strncat (Line[bitCount], (char *)HOST_MESSAGE.conv_rate_stl ,8);	
			}
		break;
		
		case bf_10convrate :
			//if(strlen((char *)HOST_MESSAGE.conv_rate_chb) != '\0')
			if(BitFieldArray[bf_10convrate] == 1)
			{
				strcat (Line[bitCount], FieldStrings [bitCount]);
				strncat (Line[bitCount], (char *)HOST_MESSAGE.conv_rate_chb ,8);
			}
		break;

		case bf_11stan :
			//if(strlen((char *)HOST_MESSAGE.stan) != '\0')
			if(BitFieldArray[bf_11stan] == 1)
			{
				strcat (Line[bitCount], FieldStrings [bitCount]);
				strncat (Line[bitCount], (char *)HOST_MESSAGE.stan, 6);
			}
		break;
		
		case bf_12time :
			//if(strlen((char *)HOST_MESSAGE.time) != '\0')
			if(BitFieldArray[bf_12time] == 1)
			{
				strcat (Line[bitCount], FieldStrings [bitCount]);
				strncat (Line[bitCount], (char *)HOST_MESSAGE.time, 6);
			}
		break ;

		case bf_13date :
			//if(strlen((char *)HOST_MESSAGE.date) != '\0')
			if(BitFieldArray[bf_13date] == 1)
			{
				strcat (Line[bitCount], FieldStrings [bitCount]);
				strncat (Line[bitCount], (char *)HOST_MESSAGE.date, 4);
			}
		break ;

		case bf_14expiry :
			//if(strlen((char *)HOST_MESSAGE.date_expd) != '\0')
			if(BitFieldArray[bf_14expiry] == 1)
			{
				strcat (Line[bitCount], FieldStrings [bitCount]);
				strncat (Line[bitCount], (char *)HOST_MESSAGE.date_expd, 4);
			}
		break ;

		case bf_15datestl :
			//if(strlen((char *)HOST_MESSAGE.date_settle) != '\0')
			if(BitFieldArray[bf_15datestl] == 1)
			{
				strcat (Line[bitCount], FieldStrings [bitCount]);
				strncat (Line[bitCount], (char *)HOST_MESSAGE.date_settle, 4);
			}
		break ;
			
		case bf_16convdate :
			//if(strlen((char *)HOST_MESSAGE.conv_date) != '\0')
			if(BitFieldArray[bf_16convdate] == 1)
			{
				strcat (Line[bitCount], FieldStrings [bitCount]);
				strncat (Line[bitCount], (char *)HOST_MESSAGE.conv_date, 4);
			}
		break ;

		case bf_17capturedate :
			//if(strlen((char *)HOST_MESSAGE.capture_date) != '\0')
			if(BitFieldArray[bf_17capturedate] == 1)
			{
				strcat (Line[bitCount], FieldStrings [bitCount]);
				strncat (Line[bitCount], (char *)HOST_MESSAGE.capture_date, 4);
			}
		break ;
		
		case bf_18sicnumber :
			//if(strlen((char *)HOST_MESSAGE.sic_number) != '\0')
			if(BitFieldArray[bf_18sicnumber] == 1)
			{
				strcat (Line[bitCount], FieldStrings [bitCount]);
				strncat (Line[bitCount], (char *)HOST_MESSAGE.sic_number, 4);
			}
		break ;
		
		case bf_19ctrycd :
			//if(strlen((char *)HOST_MESSAGE.ctry_code) != '\0')
			if(BitFieldArray[bf_19ctrycd] == 1)
			{
				strcat (Line[bitCount], FieldStrings [bitCount]);
				strncat (Line[bitCount], (char *)HOST_MESSAGE.ctry_code, 3);
			}
		break ;
		
		case bf_20panctrycd :
			//if(strlen((char *)HOST_MESSAGE.pan_ctry_cd) != '\0')
			if(BitFieldArray[bf_20panctrycd] == 1)
			{
				strcat (Line[bitCount], FieldStrings [bitCount]);
				strncat (Line[bitCount], (char *)HOST_MESSAGE.pan_ctry_cd, 3);
			}
		break ;
		
		case bf_21frwrdinstctrycd :
			//if(strlen((char *)HOST_MESSAGE.frwrdinst_ctry_cd) != '\0')
			if(BitFieldArray[bf_21frwrdinstctrycd] == 1)
			{
				strcat (Line[bitCount], FieldStrings [bitCount]);
				strncat (Line[bitCount], (char *)HOST_MESSAGE.frwrdinst_ctry_cd, 3);
			}
		break ;

		case bf_22pose :
			//if(strlen((char *)HOST_MESSAGE.posentry) != '\0')
			if(BitFieldArray[bf_22pose] == 1)
			{
				strcat (Line[bitCount], FieldStrings [bitCount]);
				strncat (Line[bitCount], (char *)HOST_MESSAGE.posentry, 3);
			}
		break ;
		
		case bf_23csn :
			//if(strlen((char *)HOST_MESSAGE.card_seq_num) != '\0')
			if(BitFieldArray[bf_23csn] == 1)
			{
				strcat (Line[bitCount], FieldStrings [bitCount]);
				strncat (Line[bitCount], (char *)HOST_MESSAGE.card_seq_num, 3);
			}
		break ;

		case bf_24nii :
			//if(strlen((char *)HOST_MESSAGE.nii) != '\0')
			if(BitFieldArray[bf_24nii] == 1)
			{
				strcat (Line[bitCount], FieldStrings [bitCount]);
				strncat (Line[bitCount], (char *)HOST_MESSAGE.nii, 3);
			}
		break ;

		case bf_25pocc :
			//if(strlen((char *)HOST_MESSAGE.pos_condcd) != '\0')
			if(BitFieldArray[bf_25pocc] == 1)
			{
				strcat (Line[bitCount], FieldStrings [bitCount]);
				strncat (Line[bitCount], (char *)HOST_MESSAGE.pos_condcd, 2);
			}
		break ;
		

		case bf_26pincode :
			//if(strlen((char *)HOST_MESSAGE.pos_pincode) != '\0')
			if(BitFieldArray[bf_26pincode] == 1)
			{
					strcat (Line[bitCount], FieldStrings [bitCount]);
					strncat (Line[bitCount], (char *)HOST_MESSAGE.pos_pincode , 2);
			}
		break ;
		
		case bf_28feeamount :
			//if(strlen((char *)HOST_MESSAGE.tran_fee) != '\0')
			if(BitFieldArray[bf_28feeamount] == 1)
			{
					strcat (Line[bitCount], FieldStrings [bitCount]);
					strncat (Line[bitCount], (char *)HOST_MESSAGE.tran_fee , 9);
			}
		break ;
		
		case bf_29settlefee :
			//if(strlen((char *)HOST_MESSAGE.settle_fee) != '\0')
			if(BitFieldArray[bf_29settlefee] == 1)
			{
					strcat (Line[bitCount], FieldStrings [bitCount]);
					strncat (Line[bitCount], (char *)HOST_MESSAGE.settle_fee , 9);
			}
		break ;

		case bf_30transactionprocessingfee :
			//if(strlen((char *)HOST_MESSAGE.transaction_processing_fee) != '\0')
			if(BitFieldArray[bf_30transactionprocessingfee] == 1)
			{
					strcat (Line[bitCount], FieldStrings [bitCount]);
					strncat (Line[bitCount], (char *)HOST_MESSAGE.transaction_processing_fee , 9);
			}
		break ;

		case bf_31settleprocessingfee :
			//if(strlen((char *)HOST_MESSAGE.settle_processing_fee) != '\0')
			if(BitFieldArray[bf_31settleprocessingfee] == 1)
			{
					strcat (Line[bitCount], FieldStrings [bitCount]);
					strncat (Line[bitCount], (char *)HOST_MESSAGE.settle_processing_fee , 9);
			}
		break ;
		case bf_32idcode :
			//if(strlen((char *)HOST_MESSAGE.acq_inst_len) != '\0')
			if(BitFieldArray[bf_32idcode] == 1)
			{
					strncpy(temp_len,(char *)HOST_MESSAGE.acq_inst_len,2); 
					len =atoi(temp_len);
					strcat (Line[bitCount], FieldStrings [bitCount]);
					strncat (Line[bitCount], (char *)HOST_MESSAGE.acq_inst_id ,len);
			}
		break ;
		
		case bf_33fwdid :
			//if(strlen((char *)HOST_MESSAGE.fwd_inst_len) != '\0')
			if(BitFieldArray[bf_33fwdid] == 1)
			{
					strncpy(temp_len,(char *)HOST_MESSAGE.fwd_inst_len,2);
					len =atoi(temp_len);
					strcat (Line[bitCount], FieldStrings [bitCount]);
					strncat (Line[bitCount], (char *)HOST_MESSAGE.fwd_inst_id ,len);
			}
		break ;

		case bf_35track2 :
			//if(strlen((char *)HOST_MESSAGE.track2_len) != '\0')
			if(BitFieldArray[bf_35track2] == 1)
			{
					strncpy(temp_len,(char *)HOST_MESSAGE.track2_len,2);
					len =atoi(temp_len);
					strcat (Line[bitCount], FieldStrings [bitCount]);
					strncat (Line[bitCount], (char *)HOST_MESSAGE.track2 ,len);
			}
		break ;

		case bf_37rrn :
			//if(strlen((char *)HOST_MESSAGE.rrn) != '\0')
			if(BitFieldArray[bf_37rrn] == 1)
			{
					strcat (Line[bitCount], FieldStrings [bitCount]);
					strncat (Line[bitCount], (char *)HOST_MESSAGE.rrn ,12);
			}
		break ;
		
		case bf_38authcode :
			//if(strlen((char *)HOST_MESSAGE.authcode) != '\0')
			if(BitFieldArray[bf_38authcode] == 1)
			{
					strcat (Line[bitCount], FieldStrings [bitCount]);
					strncat (Line[bitCount], (char *)HOST_MESSAGE.authcode ,6);
			}
		break ;
		
		case bf_39respcode :
			//if(strlen((char *)HOST_MESSAGE.responsecd) != '\0')
			if(BitFieldArray[bf_39respcode] == 1)
			{
					strcat (Line[bitCount], FieldStrings [bitCount]);
					strncat (Line[bitCount], (char *)HOST_MESSAGE.responsecd,2);
			}
		break ;
		
		case bf_41termid :
			//if(strlen((char *)HOST_MESSAGE.termid) != '\0')
			if(BitFieldArray[bf_41termid] == 1)
			{
					strcat (Line[bitCount], FieldStrings [bitCount]);
					strncat (Line[bitCount], (char *)HOST_MESSAGE.termid,8);
			}
		break ;
		
		case bf_42cardacc :
			//if(strlen((char *)HOST_MESSAGE.cardaccid) != '\0')
			if(BitFieldArray[bf_42cardacc] == 1)
			{
					strcat (Line[bitCount], FieldStrings [bitCount]);
					strncat (Line[bitCount], (char *)HOST_MESSAGE.cardaccid,15);
			}
		break ;
		
		case bf_43cardaccloc :
			//if(strlen((char *)HOST_MESSAGE.cardaccloc) != '\0')
			if(BitFieldArray[bf_43cardaccloc] == 1)
			{
					strcat (Line[bitCount], FieldStrings [bitCount]);
					strncat (Line[bitCount], (char *)HOST_MESSAGE.cardaccloc,40);
			}
		break ;
		
		case bf_44respdata :
			//if(strlen((char *)HOST_MESSAGE.addl_rsp_len) != '\0')
			if(BitFieldArray[bf_44respdata] == 1)
			{
					strncpy(temp_len,(char *)HOST_MESSAGE.addl_rsp_len,2);
					len =atoi(temp_len);
					strcat (Line[bitCount], FieldStrings [bitCount]);
					strncat (Line[bitCount], (char *)HOST_MESSAGE.addlrspdata,len);
			}
		break ;
		
		case bf_45track1 :
			//if(strlen((char *)HOST_MESSAGE.track1_len) != '\0')		
			if(BitFieldArray[bf_45track1] == 1)
			{
					strncpy(temp_len,(char *)HOST_MESSAGE.track1_len,2);
					len =atoi(temp_len);
					strcat (Line[bitCount], FieldStrings [bitCount]);
					strncat (Line[bitCount], (char *)HOST_MESSAGE.track1,len);
			}
		break ;

		case bf_48addldata :
			//if(strlen((char *)HOST_MESSAGE.addl_data_len) != '\0')
			if(BitFieldArray[bf_48addldata] == 1)
			{
				if(0 == strncmp((char*)HOST_MESSAGE.msgtype,"0800",4))
				{
					
					strncpy(temp_len,(char *)HOST_MESSAGE.addl_data_len,3);
					len = atoi(temp_len);
					memcpy(temp_data,HOST_MESSAGE.addl_data+ 2  , (len -2));
					genutil_bin_to_ascii (temp_data, len,buffer, (len*2));
					strcat (Line[bitCount], FieldStrings [bitCount]);
					strncat(Line[bitCount],"NK",2);
					strncat (Line[bitCount], (char *)buffer,strlen(buffer));
				}
				else
				{
					strncpy(temp_len,(char *)HOST_MESSAGE.addl_data_len,3);
					len =atoi(temp_len);
					strcat (Line[bitCount], FieldStrings [bitCount]);
					strncat (Line[bitCount], (char *)HOST_MESSAGE.addl_data,len);
				}
			}
		break ;
		
		case bf_49trncurcd :
			//if(strlen((char *)HOST_MESSAGE.tran_curr_cd) != '\0')		
			if(BitFieldArray[bf_49trncurcd] == 1)
			{
					strcat (Line[bitCount], FieldStrings [bitCount]);
					strncat (Line[bitCount], (char *)HOST_MESSAGE.tran_curr_cd,3);
			}
		break ;
		
		case bf_50stlmtcurrcd :
			//if(strlen((char *)HOST_MESSAGE.settle_cur_cd) != '\0')
			if(BitFieldArray[bf_50stlmtcurrcd] == 1)
			{
					strcat (Line[bitCount], FieldStrings [bitCount]);
					strncat (Line[bitCount], (char *)HOST_MESSAGE.settle_cur_cd,3);
			}
		break ;
		
		case bf_51crdcurcd :
			//if(strlen((char *)HOST_MESSAGE.card_curr_cd) != '\0')
			if(BitFieldArray[bf_51crdcurcd] == 1)
			{
					strcat (Line[bitCount], FieldStrings [bitCount]);
					strncat (Line[bitCount], (char *)HOST_MESSAGE.card_curr_cd,3);
			}
		break ;

		case bf_52pinblock :
			//if(strlen((char *)HOST_MESSAGE.pinblock) != '\0')    
			if(BitFieldArray[bf_52pinblock] == 1)
			{
					strncpy((char *)temp_data,(char *)HOST_MESSAGE.pinblock,8);
					len = strlen((char *)temp_data);
					genutil_bin_to_ascii (temp_data, len,buffer, (len*2));
					strcat (Line[bitCount], FieldStrings [bitCount]);
					strncat (Line[bitCount], (char *)buffer,strlen(buffer));
			}
		break ;
		
		case bf_53seccntl :
			//if(strlen((char *)HOST_MESSAGE.sec_cntl) != '\0')
			if(BitFieldArray[bf_53seccntl] == 1)
			{
					strcat (Line[bitCount], FieldStrings [bitCount]);
					strncat (Line[bitCount], (char *)HOST_MESSAGE.sec_cntl,16);
			}
		break ;
		
		case bf_54addlamts :
			//if(strlen((char *)HOST_MESSAGE.addl_len) != '\0')		
			if(BitFieldArray[bf_54addlamts] == 1)
			{
					strncpy(temp_len,(char *)HOST_MESSAGE.addl_len,3);
					len= atoi(temp_len);
					strcat (Line[bitCount], FieldStrings [bitCount]);
					strncat (Line[bitCount], (char *)HOST_MESSAGE.addlamts,len);
			}
		break ;
		
		case bf_55iccdata :
			//if(strlen((char *)HOST_MESSAGE.iccdata_len) != '\0')		
			if(BitFieldArray[bf_55iccdata] == 1)
			{
					strncpy(temp_len,(char *)HOST_MESSAGE.iccdata_len,3);
					len =atoi(temp_len);
					memcpy(iccdata,HOST_MESSAGE.iccdata,len);
					genutil_EMV_To_Auth_Tx( (BYTE*)iccdata, atoi(temp_len),&AUTH_TX_V );
					memcpy(&EMV_details,&(AUTH_TX_V.EMV_details),sizeof(EMV_details));
					Generate_CUP_EMV_Details();
					strcat (Line[bitCount], FieldStrings [bitCount]);
					strncat (Line[bitCount], (char *)iccdata,strlen((char *)iccdata));
			}
		break ;

		case bf_56ISOuse :
			//if(strlen((char *)HOST_MESSAGE.ISOdata_len) != '\0')	
			if(BitFieldArray[bf_56ISOuse] == 1)
			{
					strncpy(temp_len,(char *)HOST_MESSAGE.ISOdata_len,3);
					len= atoi(temp_len);
					strcat (Line[bitCount], FieldStrings [bitCount]);
					strncat (Line[bitCount], (char *)HOST_MESSAGE.ISOdata,len);
			}
		break ;
		
		case bf_60advicecode :
			//if(strlen((char *)HOST_MESSAGE.advice_cd_len) != '\0')	
			if(BitFieldArray[bf_60advicecode] == 1)
			{
					strncpy(temp_len,(char *)HOST_MESSAGE.advice_cd_len,3);
					len =atoi(temp_len);
					strcat (Line[bitCount], FieldStrings [bitCount]);
					strncat (Line[bitCount], (char *)HOST_MESSAGE.advice_code,len);
			}
		break ;
		
		case bf_61posdata :
			//if(strlen((char *)HOST_MESSAGE.pos_data_len) != '\0')	
			if(BitFieldArray[bf_61posdata] == 1)
			{
					strncpy(temp_len,(char *)HOST_MESSAGE.pos_data_len,3);
					len =atoi(temp_len);
					strcat (Line[bitCount], FieldStrings [bitCount]);
					strncat (Line[bitCount], (char *)HOST_MESSAGE.pos_data,len);
			}
		break ;
		
		case bf_62inf :
			//if(strlen((char *)HOST_MESSAGE.inf_data_len) != '\0')			
			if(BitFieldArray[bf_62inf] == 1)
			{
					strncpy(temp_len,(char *)HOST_MESSAGE.inf_data_len,3);
					len =atoi(temp_len);
					strcat (Line[bitCount], FieldStrings [bitCount]);
					strncat (Line[bitCount], (char *)HOST_MESSAGE.inf_data,len);
			}
		break ;
		
		case bf_63banknet :
			//if(strlen((char *)HOST_MESSAGE.banknet_len) != '\0')		
			if(BitFieldArray[bf_63banknet] == 1)
			{
					strncpy(temp_len,(char *)HOST_MESSAGE.banknet_len,3);
					len =atoi(temp_len);
					strcat (Line[bitCount], FieldStrings [bitCount]);
					strncat (Line[bitCount], (char *)HOST_MESSAGE.banknet,len);
			}
		break ;

		case bf_70nmicode :
			//if(strlen((char *)HOST_MESSAGE.nmi_code) != '\0')	
			if(BitFieldArray[bf_70nmicode] == 1)
			{
					strcat (Line[bitCount], FieldStrings [bitCount]);
					strncat (Line[bitCount], (char *)HOST_MESSAGE.nmi_code,3);
			}
		break ;

		case bf_73actiondate :
			//if(strlen((char *)HOST_MESSAGE.action_date) != '\0')	
			if(BitFieldArray[bf_73actiondate] == 1)
			{
					strcat (Line[bitCount], FieldStrings [bitCount]);
					strncat (Line[bitCount], (char *)HOST_MESSAGE.action_date,6);
			}
		break ;

		case bf_90orgdata :
			//if(strlen((char *)HOST_MESSAGE.orig_data) != '\0')	
			if(BitFieldArray[bf_90orgdata] == 1)
			{
					strcat (Line[bitCount], FieldStrings [bitCount]);
					strncat (Line[bitCount], (char *)HOST_MESSAGE.orig_data,42);
			}
		break ;
		
		case bf_91filecode :
			//if(strlen((char *)HOST_MESSAGE.file_code) != '\0')	
			if(BitFieldArray[bf_91filecode] == 1)
			{
					strcat (Line[bitCount], FieldStrings [bitCount]);
					strncat (Line[bitCount], (char *)HOST_MESSAGE.file_code,1);
			}
		break ;
		
		case bf_94servind :
			//if(strlen((char *)HOST_MESSAGE.service_ind) != '\0')
			if(BitFieldArray[bf_94servind] == 1)
			{
					strcat (Line[bitCount], FieldStrings [bitCount]);
					strncat (Line[bitCount], (char *)HOST_MESSAGE.service_ind,7);
			}
		break ;
		
		case bf_95repl_amt :
			//if(strlen((char *)HOST_MESSAGE.replace_amt) != '\0')
			if(BitFieldArray[bf_95repl_amt] == 1)
			{
					strcat (Line[bitCount], FieldStrings [bitCount]);
					strncat (Line[bitCount], (char *)HOST_MESSAGE.replace_amt,42);
			}
		break ;


		case bf_96securcode :
			//if(strlen((char *)HOST_MESSAGE.security_code) != '\0')	
			if(BitFieldArray[bf_96securcode] == 1)
			{
					strncpy((char *)temp_data,(char *)HOST_MESSAGE.security_code,8);
					len = strlen((char *)temp_data);
					genutil_bin_to_ascii (temp_data, len,buffer, (len*2));
					strcat (Line[bitCount], FieldStrings [bitCount]);
					strncat (Line[bitCount], (char *)buffer,strlen(buffer));
			}
		break ;

		case bf_100recvid :
			//if(strlen((char *)HOST_MESSAGE.recv_len) != '\0')	
			if(BitFieldArray[bf_100recvid] == 1)
			{
					strncpy(temp_len,(char *)HOST_MESSAGE.recv_len,2);
					len =atoi(temp_len);
					strcat (Line[bitCount], FieldStrings [bitCount]);
					strncat (Line[bitCount], (char *)HOST_MESSAGE.recvid,len);
			}
		break ;

		case bf_101filename :
			//if(strlen((char *)HOST_MESSAGE.filename_len) != '\0')	
			if(BitFieldArray[bf_101filename] == 1)
			{
					strncpy(temp_len,(char *)HOST_MESSAGE.filename_len,2);
					len =atoi(temp_len);
					strcat (Line[bitCount], FieldStrings [bitCount]);
					strncat (Line[bitCount], (char *)HOST_MESSAGE.filename,len);
			}
		break ;
		
		case bf_102acct1 :
			//if(strlen((char *)HOST_MESSAGE.acct1_len) != '\0')
			if(BitFieldArray[bf_102acct1] == 1)
			{
					strncpy(temp_len,(char *)HOST_MESSAGE.acct1_len,2);
					len =atoi(temp_len);
					strcat (Line[bitCount], FieldStrings [bitCount]);
					strncat (Line[bitCount], (char *)HOST_MESSAGE.acct1,len);
			}
		break ;

		case bf_103acct2 :
			//if(strlen((char *)HOST_MESSAGE.acct2_len) != '\0')	
			if(BitFieldArray[bf_103acct2] == 1)
			{
					strncpy(temp_len,(char *)HOST_MESSAGE.acct2_len,2);
					len =atoi(temp_len);
					strcat (Line[bitCount], FieldStrings [bitCount]);
					strncat (Line[bitCount], (char *)HOST_MESSAGE.acct2,len);
			}
		break ;

		case bf_104dpdata :
			//if(strlen((char *)HOST_MESSAGE.dpdata_len) != '\0')
			if(BitFieldArray[bf_104dpdata] == 1)
			{
					strncpy(temp_len,(char *)HOST_MESSAGE.dpdata_len,3);
					len =atoi(temp_len);
					strcat (Line[bitCount], FieldStrings [bitCount]);
					strncat (Line[bitCount], (char *)HOST_MESSAGE.dpdata,len);
			}
		break ;

		case bf_105tldata :
			//if(strlen((char *)HOST_MESSAGE.tl_data_len) != '\0')
			if(BitFieldArray[bf_105tldata] == 1)
			{
					strncpy(temp_len,(char *)HOST_MESSAGE.tl_data_len,3);
					len =atoi(temp_len);
					strcat (Line[bitCount], FieldStrings [bitCount]);
					strncat (Line[bitCount], (char *)HOST_MESSAGE.tl_data,len);
			}
		break ;

		case bf_108moneySend :
			//if(strlen((char *)HOST_MESSAGE.de108_len) != '\0')	
			if(BitFieldArray[bf_108moneySend] == 1)
			{
					strncpy(temp_len,(char *)HOST_MESSAGE.de108_len,3);
					len =atoi(temp_len);
					strcat (Line[bitCount], FieldStrings [bitCount]);
					strncat (Line[bitCount], (char *)HOST_MESSAGE.de108,len);
			}
		break ;

		case bf_112addldata :
			//if(strlen((char *)HOST_MESSAGE.addldata_nation_len) != '\0')	
			if(BitFieldArray[bf_112addldata] == 1)
			{
					strncpy(temp_len,(char *)HOST_MESSAGE.addldata_nation_len,3);
					len =atoi(temp_len);
					strcat (Line[bitCount], FieldStrings [bitCount]);
					strncat (Line[bitCount], (char *)HOST_MESSAGE.addldata_nation,len);
			}
		break ;

		case bf_119privateuse2 :
			//if(strlen((char *)HOST_MESSAGE.addldata_nation_len) != '\0')	
			if(BitFieldArray[bf_119privateuse2] == 1)
			{
					strncpy(temp_len,(char *)HOST_MESSAGE.private_use2_len,3);
					len =atoi(temp_len);
					strcat (Line[bitCount], FieldStrings [bitCount]);
					strncat (Line[bitCount], (char *)HOST_MESSAGE.private_use2,len);
			}
		break ;

		case bf_120orgmsgid :
			//if(strlen((char *)HOST_MESSAGE.orig_msg_len) != '\0')	
			if(BitFieldArray[bf_120orgmsgid] == 1)
			{
					strncpy(temp_len,(char *)HOST_MESSAGE.orig_msg_len,3);
					len =atoi(temp_len);
					strcat (Line[bitCount], FieldStrings [bitCount]);
					strncat (Line[bitCount], (char *)HOST_MESSAGE.orig_msg_id,len);
			}
		break ;


		case bf_121mcagent :
			//if(strlen((char *)HOST_MESSAGE.mc_agent_len) != '\0')	
			if(BitFieldArray[bf_121mcagent] == 1)
			{
					strncpy(temp_len,(char *)HOST_MESSAGE.mc_agent_len,3);
					len =atoi(temp_len);
					strcat (Line[bitCount], FieldStrings [bitCount]);
					strncat (Line[bitCount], (char *)HOST_MESSAGE.mc_agent_id,len);
			}
		break ;

		case bf_122addlrec :
			//if(strlen((char *)HOST_MESSAGE.addlrec_len) != '\0')	
			if(BitFieldArray[bf_122addlrec] == 1)
			{
					strncpy(temp_len,(char *)HOST_MESSAGE.addlrec_len,3);
					len =atoi(temp_len);
					strcat (Line[bitCount], FieldStrings [bitCount]);
					strncat (Line[bitCount], (char *)HOST_MESSAGE.addlrec,len);
			}
		break ;

		case bf_124memData :
			//if(strlen((char *)HOST_MESSAGE.memData_len) != '\0')
			if(BitFieldArray[bf_124memData] == 1)
			{
					strncpy(temp_len,(char *)HOST_MESSAGE.memData_len,3);
					len =atoi(temp_len);
					strcat (Line[bitCount], FieldStrings [bitCount]);
					strncat (Line[bitCount], (char *)HOST_MESSAGE.memData,len);
			}
		break ;

		case bf_127privdata :
			//if(strlen((char *)HOST_MESSAGE.priv127_len) != '\0')	
			if(BitFieldArray[bf_127privdata] == 1)
			{
					strncpy(temp_len,(char *)HOST_MESSAGE.priv127_len,3);
					len =atoi(temp_len);
					strcat (Line[bitCount], FieldStrings [bitCount]);
					strncat (Line[bitCount], (char *)HOST_MESSAGE.priv127,len);
			}
		break ;

	}

		
		startCharacter++;
		LastDisplayBit = bitCount;
	}

		if(strlen((char *)HOST_MESSAGE.iccdata) != 0)
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
		  sprintf(Line[bitCount++],  "91	Issuer Auth Data	:	%s",(char*)EMV_details.issuer_auth_data);
		}
	

}

/******************************************************************************
 *
 *  NAME:         MCRDUTIL_PARSE_HOST_RESPONSE_MSG
 *
 *  DESCRIPTION:  This procedure copies information from a string sent by the
 *                host.  It copies it into the HOST_MESSAGE structure.  The
 *                data is converted from EBCDIC to ASCII as they are moved.
 *
 *  INPUTS:       None
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   OK if successful, NOT_OK if a problem occurred
 *
 *  AUTHOR:       Dennis Irby
 *
 *  MODIFICATIONS:
 ******************************************************************************/
INT mcrdutil_parse_host_response_msg()
{
   struct bitfield_data *pBF;
   INT                   bfidx;    /* Index to bitfields for this transaction */
   INT                   i;
   CHAR                  Buff[70];


   /* Initialize the HOST_MESSAGE structure. */
   memset(&HOST_MESSAGE, 0x00, sizeof(HOST_MESSAGE));

   /* Extract the message type from the host message. */
   memcpy( HOST_MESSAGE.msgtype, &ReqRsp_Buffer[0],
           sizeof(HOST_MESSAGE.msgtype) );

   /* Convert Message Type from EBCDIC to ASCII. */
   for( i=0; i<4; i++ )
      HOST_MESSAGE.msgtype[i] = ascii_ebcdic_conv(HOST_MESSAGE.msgtype[i], E2A);

   bitmapidx = 4;       /* point to bitmap */

   /* Point past bitmap, but 1st check for extended bitmap. */
   if ( ReqRsp_Buffer[bitmapidx]  &  BITTAB[bf_1xbitfield].bit_mask )
   {
      reqidx = 20;                         /* Extended     */
      BitFieldSize = EXTENDED_BITFIELD;
	  genutil_bin_to_ascii ((BYTE *)&ReqRsp_Buffer[4], EXTENDED_BITMAP_SIZE, astr_bitmap, sizeof(astr_bitmap));
   }
   else
   {
      reqidx = 12;                         /* Not Extended */
      BitFieldSize = NORMAL_BITFIELD;
	  genutil_bin_to_ascii ((BYTE *)&ReqRsp_Buffer[4], NORMAL_BITMAP_SIZE, astr_bitmap, sizeof(astr_bitmap));
   }

   /*------------------------------------------------------------------
      For every bitfield present in message ( except first bit ) . . .
   -------------------------------------------------------------------*/
   for( bfidx = 2; bfidx <= BitFieldSize; bfidx++ )
   {
      if ( ReqRsp_Buffer[bitmapidx+BITTAB[bfidx].byte_no] &
           BITTAB[bfidx].bit_mask )
      {
         /* Bit is on, field is there.... */
         pBF = Find_Bitfield_Table((bitfields)bfidx);

         if ( NULL == pBF )
         {
            sprintf( Buff, "Unexpected bitfield (%d) in message from host.",
                     bfidx );
            strcat( Buff, " MsgType = " );
            strncat( Buff, (char *)HOST_MESSAGE.msgtype, 4 );
           /* nc_error_handler(1, Buff, 1, ALERT_MSG,
                            "mcrdutil_parse_host_response_msg",4,
                             FATAL_ERROR, NULL_STR, NULL_STR, NULL_STR,
                            "501", 0, NULL_PTR  );*/
            return 0; /* Invalid field in message - so quit. */
		 }

		 BitFieldArray[pBF->bd_fieldid] = 1;
         /*---------------------------------------*/
         /* call the Response Handling routine... */
         /*---------------------------------------*/
	
         (pBF->bd_BFRsp)(pBF->bd_fieldid);
         if ( true == invalid_msg_format )
         {
            strcpy( Buff, "Invalid MasterCard message format." );
            strcat( Buff, " MsgType = " );
            strncat( Buff, (char *)HOST_MESSAGE.msgtype, 4 );
            /*nc_error_handler(1, Buff,
                             1, ALERT_MSG, "mcrdutil_parse_host_response_msg",4,
                             FATAL_ERROR, NULL_STR, NULL_STR, NULL_STR,
                            "597", 0, NULL_PTR  );*/
            return 0;
         }
      }
   }
   return 1;
} /* mcrdutil_parse_host_response_msg */   


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
	char errbuf[256]={0};
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

		if ( mcrdutil_parse_host_response_msg() == 0)
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
	//UpdateData(FALSE); //transfer the data from variables back to control.
	memset(&BitFieldArray, 0, sizeof(BitFieldArray));
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

/**************************************/
void genutil_EMV_To_Auth_Tx( BYTE * emv_raw, INT total_len, pAUTH_TX p_auth_tx )
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
          	if (byteTwoOfTag == 0x20) 
            {
               genutil_bin_to_ascii (&emv_raw[dataIndex], fieldLength , &tempstr[0], sizeof(tempstr));
               memcpy(p_auth_tx->EMV_details.cardholder_name, tempstr, sizeof(p_auth_tx->EMV_details.cardholder_name)-1);
               dataIndex += fieldLength;
            }
            else if (byteTwoOfTag == 0x2A) 
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
                        genutil_bcd_to_asc (&emv_raw[dataIndex], fieldLength , &tempstr[0], sizeof(tempstr));
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
			   case 0x0B: 
                        genutil_bin_to_ascii (&emv_raw[dataIndex], fieldLength , &tempstr[0], sizeof(tempstr));
                        memcpy(p_auth_tx->EMV_details.UPI_card_std_len, tempstr, sizeof(p_auth_tx->EMV_details.UPI_card_std_len)-1);
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
            	   	    genutil_bcd_to_asc (&emv_raw[dataIndex], fieldLength , &tempstr[0], sizeof(tempstr));
            	   	    memcpy(p_auth_tx->EMV_details.serial_number, tempstr, sizeof(p_auth_tx->EMV_details.serial_number)-1);
                        //memcpy(p_auth_tx->EMV_details.serial_number, &emv_raw[dataIndex], fieldLength);
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
			case 0x8A:
                     genutil_bin_to_ascii (&emv_raw[dataIndex], fieldLength , &tempstr[0], sizeof(tempstr));
                     memcpy(p_auth_tx->EMV_details.auth_response_code, tempstr, sizeof(p_auth_tx->EMV_details.auth_response_code)-1);
                     break;
            case 0x91:
                     genutil_bin_to_ascii (&emv_raw[dataIndex], fieldLength , &tempstr[0], sizeof(tempstr));
                     memcpy(p_auth_tx->EMV_details.issuer_auth_data, tempstr, sizeof(p_auth_tx->EMV_details.issuer_auth_data)-2);
                    // p_auth_tx->EMV_details.issuer_auth_data[0] = fieldLength;
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

/************************************
 *
 *  NAME:         Generate_CUP_EMV_Details
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
void Generate_CUP_EMV_Details()
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
	if(strlen((char*)EMV_details.issuer_auth_data)!=0)
	{
		strncat((char*)iccdata,"91",2);
		strncat((char*)iccdata,(char*)EMV_details.issuer_auth_data,strlen((char *)EMV_details.issuer_auth_data));
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

void Cex_amex_parserDlg::OnBnClickedcopyButton()
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

void Cex_amex_parserDlg::OnBnClickedaboutButton()
{
	MCInfo mcrd;
	mcrd.DoModal();
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
