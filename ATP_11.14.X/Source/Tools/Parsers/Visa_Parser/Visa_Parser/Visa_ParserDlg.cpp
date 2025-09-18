
// Mcrd_ParserDlg : implementation file
//

#include "stdafx.h"
#include "Visa_Parser.h"
#include "Visa_ParserDlg.h"
#include "afxdialogex.h"
#include "VisaParser.h"
#include "BASICTYP.H"
#include <math.h>
#include <time.h>
#include <sys/timeb.h>
#include "VisaInfo.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#endif


void ascii_to_bin (LPBYTE src, LPBYTE dst, INT len);
AUTH_TX auth_tx;
AUTH_TX AUTH_TX_V;
EMV EMV_details;
CHAR astr_bitmap[64]={0};

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
	DDX_Control(pDX, IDC_COPY, parsedCopy);
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
static char   Line[256][512];

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


#define  THIRD_BITMAP_SIZE					24
#define  EXTENDED_BITMAP_SIZE    			16
#define  NORMAL_BITMAP_SIZE					8

bool field_25pocc_present = false;

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
"028 TxnFeeAmount				: ",
"029 SettleFeeAmount		: ",
"030 TxnProcFeeAmount		: ",
//"031 SettleProcFeeAmount	: ",
"031 TransactionID			: ",
"032 AcquiringIdCode	                		: ",
"033 ForwardIdCode				: ",
"034 PANExt				: ",
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
"045 Track1				: ",
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
"059 Detaild Inqry Data1			: ",
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
        BYTE msgtype_bcd   [2];
        BYTE bitmap1_bin   [8];
        BYTE bitmap2_bin   [8];
        BYTE pan_len       [1];
        BYTE pan_bin       [10];
        BYTE proccode      [3];
        BYTE amount_bin    [6];
		BYTE settle_amt    [6];
        BYTE card_bill_amt [6];
        BYTE gmt_time      [5];
		BYTE conv_rate_stl [4];
        BYTE conv_rate	   [4];
        BYTE stan_bin      [3];
        BYTE time_bin      [3];
        BYTE date_bin      [2];
        BYTE date_expd     [2];
        BYTE date_settle   [2];
		BYTE conv_date     [2];
        BYTE sic_number    [2];
        BYTE ctry_code     [2];
        BYTE pan_ctry_cd   [2];
        BYTE posentry      [2];
        BYTE cardseqnum    [2];   /* DE23 */
        BYTE nii           [2];			  
        BYTE pos_condcd    [1];			   
        BYTE pin_captcd    [1];	
		BYTE fee_amount    [9];
        BYTE acq_inst_len  [1];
        BYTE acq_inst_id   [6];
        BYTE fwd_inst_len  [1];			
        BYTE fwd_inst_id   [7];
        BYTE pan_ext_len   [2];
        BYTE pan_ext       [256];
        BYTE track2_len    [1];
        BYTE track2        [37];
        BYTE rrn           [12];
        BYTE authcode      [6];			 
        BYTE responsecd    [2];
        BYTE src           [3];
        BYTE termid        [8];
        BYTE cardaccid     [15];		 
        BYTE cardaccname   [40];
        BYTE addl_rsp_len  [1];
        BYTE addlrspdata   [25];
        BYTE track1_len    [1];
        BYTE track1        [76];
        BYTE addl_data_len [1];
        BYTE addl_data     [256]; /* DE48 */
        BYTE tran_curr_cd  [2];
		BYTE settle_cur_cd [2];
        BYTE card_curr_cd  [2];
        BYTE pin_block     [8];
        BYTE sec_cntl      [8];
        BYTE addl_len      [1];
        BYTE addlamts      [120];
		BYTE ICCdata_len   [1];
        BYTE ICCdata       [255];
		BYTE PAD_len	   [2];
		BYTE PAD_data      [255];
        BYTE posdata_len   [1];
        BYTE posdata       [14];
        BYTE priv60_len    [1];
        BYTE priv60        [6];
        BYTE priv61_len    [1];
        BYTE priv61        [18];
        BYTE priv62_len    [1];
        BYTE priv62        [58];
        BYTE priv63_len    [1];
        BYTE priv63        [255];
        BYTE nmi_code      [2];
        BYTE action        [3];  /* DE73 */
        BYTE orig_data     [21];
        BYTE file_update   [1];  /* DE91 */
        BYTE resp_ind      [5];
        BYTE repl_amts     [42];
        BYTE msgsec        [8];
        BYTE recv_inst_len [1];
        BYTE recv_inst_id  [7];
        BYTE fname_len     [1];
        BYTE filename      [18]; /* DE101 */
        BYTE acct_id_1_len [1];
        BYTE acct_id_1     [29];
        BYTE acct_id_2_len [1];
        BYTE acct_id_2     [29];
        BYTE trandesc_len  [1];
        BYTE trandesc      [256]; /* DE104 */
        BYTE addl_tran_len [2];
        BYTE addl_trandata [256]; /* DE111 */
		BYTE addl_trace    [50];
		BYTE addl_trace_len[1];
        BYTE issref_len    [1];  /* DE116 */
        BYTE issref        [256];
        BYTE intra_ctry_len[1];  /* DE118 */
        BYTE intra_ctry    [256];
        BYTE org_msg_id    [3];
        BYTE iss_inst_len  [1];
        BYTE iss_inst_id   [12];
        BYTE open_use_len  [1];
        BYTE open_use      [14];
        BYTE addr_verf_len [1];
        BYTE addr_verf     [29];
        BYTE free_text_len [1];
        BYTE free_text     [136];
        BYTE sup_info_len  [1];
        BYTE sup_info      [255];
        BYTE de126_len     [1];
        BYTE de126         [255];
        BYTE file_rec_len  [1];  /* DE127 */
        BYTE file_rec      [255];
        BYTE dest_stat_id  [3];
        BYTE src_stat_id   [3];
        BYTE term_cap_prof [3];  /* DE130 */
        BYTE tvr           [5];  /* DE131 */
        BYTE unpredictable [4];  /* DE132 */
        BYTE tid_serial    [8];  /* DE133 */
        BYTE visa_data_len [1];  /* DE134 */
        BYTE visa_data     [32]; /*   "   */
        BYTE iss_data_len  [1];  /* DE135 */
        BYTE iss_data      [15]; /*   "   */
        BYTE cryptogram    [8];  /* DE136 */
        BYTE app_tran_ctr  [2];  /* DE137 */
        BYTE app_profile   [2];  /* DE138 */
        BYTE arpc_resp     [10]; /* DE139 */
        BYTE iss_auth_len  [1];  /* DE140 */
        BYTE iss_auth_data [255];/*   "   */
        BYTE iss_script_len[1];  /* DE142 */
        BYTE iss_script    [255];/*   "   */
        BYTE iss_result_len[1];  /* DE143 */
        BYTE iss_result    [20]; /*   "   */
        BYTE crypto_type   [1];  /* DE144 */
        BYTE tid_country   [2];  /* DE145 */
        BYTE tid_tran_date [3];  /* DE146 */
        BYTE crypto_amt    [6];  /* DE147 */
        BYTE crypto_curr   [2];  /* DE148 */
        BYTE crypto_cashbk [6];  /* DE149 */
        BYTE header        [30];
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
   bf_done           = 0,      /* end of list    */
   bf_1xbitfield     = 1,      /* extended bit field */
   bf_2pan           = 2,      /* pan            */
   bf_3proc          = 3,      /* processing code */
   bf_4amount        = 4,      /* amount   */
   bf_5settleamt     = 5,      /* Settlement Amount */
   bf_6crdbillamt    = 6,      /* amount cardholder billing */
   bf_7trntime       = 7,      /* transmission date-time */
   bf_9setconvrate   = 9,      /* Settlement Conversion rate */
   bf_10convrate     = 10,     /* Conversion rate, cardholder billing */
   bf_11stan         = 11,     /* system trace number */
   bf_12time         = 12,     /* time    */
   bf_13date         = 13,     /* date, trans  */
   bf_14expiry       = 14,     /* expiry date  */
   bf_15datestl      = 15,     /* settlement date */
   bf_16convdate     = 16,     /* Conversion date */
   bf_18sicnumber    = 18,     /* Merchant's type of business */
   bf_19ctrycd       = 19,     /* Acquiring institution's country code */
   bf_20panctrycd    = 20,     /* pan extended, country code */
   bf_22pose         = 22,     /* pos entry mode */
   bf_23cardseqnum   = 23,     /* card sequence number */
   bf_24nii          = 24,     /* nii      */
   bf_25pocc         = 25,     /* pos condition code */
   bf_26pincc        = 26,     /* pin capture code */
   bf_28feeamount    = 28,     /* Transaction Fee Amount */ /* Girija Y TF Apr 06 2009 */
   bf_32idcode       = 32,     /* Acquiring institution's ID code */
   bf_33fwdidcode    = 33,     /* Forwarding institution's ID code */
   bf_34panext       = 34,     /* pan extended */
   bf_35track2       = 35,     /* track 2 data */
   bf_37rrn          = 37,     /* retrieval reference number */
   bf_38authcode     = 38,     /* auth code   */
   bf_39respcode     = 39,     /* response code*/
   bf_40src          = 40,     /* service restriction code */
   bf_41termid       = 41,     /* terminal id  */
   bf_42cardacc      = 42,     /* card acceptor id */
   bf_43cardaccname  = 43,     /* card acceptor name/location */
   bf_44respdata     = 44,     /* additional response data */
   bf_45track1       = 45,     /* track 1 data   */
   bf_48addl_data    = 48,     /* Additional Data - Private */
   bf_49trncurcd     = 49,     /* transaction currency code */
   bf_50stlmtcurrcd  = 50,     /* settlement currency code             */
   bf_51crdcurcd     = 51,     /* card currency code */
   bf_52pinblock     = 52,     /* pin block    */
   bf_53seccntl      = 53,     /* security related control info */
   bf_54addlamts     = 54,     /* additional amounts */
   bf_55ICCdata		 = 55,
   bf_56pad	 		 = 56,     /* PAN based Transaction*/
   bf_59posdata      = 59,     /* national point-of-service geographic data */
   bf_60priv60       = 60,     /* private field 60 */
   bf_61priv61       = 61,     /* private field 61 */
   bf_62priv62       = 62,     /* private field 62 */
   bf_63priv63       = 63,     /* private field 63 */
   bf_2xbitfield     = 65,     /* third bit field */
   bf_70nmicode      = 70,     /* network management information code */
   bf_73action       = 73,     /* Date, action */
   bf_90orgdata      = 90,     /* original data elements   */
   bf_91fileupdate   = 91,     /* file update code         */
   bf_93rspind       = 93,     /* response indicator       */
   bf_95replamts     = 95,     /* replacement amounts      */
   bf_96msgsec       = 96,     /* message security code    */
   bf_100rcvid       = 100,    /* receiving inst id code   */
   bf_101fname       = 101,    /* file name                */
   bf_102acct1       = 102,    /* acct id 1                */
   bf_103acct2       = 103,    /* acct id 2                */
   bf_104trandesc    = 104,    /* transaction description  */
   bf_111addl_tran   = 111,    /* additional tran data	   */
   bf_115addl_trace  = 115,
   bf_116issref      = 116,    /* card issuer reference data      */
   bf_118intractry   = 118,    /* intra country data       */
   bf_120orgmsgid    = 120,    /* original message type id */
   bf_121issid       = 121,    /* issuing inst id code     */
   bf_122openuse     = 122,    /* remaining open-to-use    */
   bf_123addverf     = 123,    /* address verification data*/
   bf_124freeform    = 124,    /* free form text - Japan   */
   bf_125supinfo     = 125,    /* cris alert               */
   bf_126priv126     = 126,    /* private fields VSEC,CVV2 */
   bf_127filerecs    = 127,    /* free form text - Japan   */
   bf_130termcap     = 130,    /* terminal capability profile     */
   bf_131tvr         = 131,    /* terminal verifictation results  */
   bf_132unpredict   = 132,    /* unpredictable number            */
   bf_133termserial  = 133,    /* terminal serial number          */
   bf_134visadata    = 134,    /* Visa discretionary data         */
   bf_135issdata     = 135,    /* issuer discretionary data       */
   bf_136crytogram   = 136,    /* cryptogram                      */
   bf_137apptranctr  = 137,    /* application transaction counter */
   bf_138app_profile = 138,    /* application interchange profile */
   bf_139arpcresp    = 139,    /* ARPC Response crytpo and code   */
   bf_140issauth     = 140,    /* Issuer Authentication Data      */
   bf_142iss_script  = 142,    /* Issuer script                   */
   bf_143iss_result  = 143,    /* Issuer script result            */
   bf_144crytpotype  = 144,    /* cryptogram transaction type     */
   bf_145tcountry    = 145,    /* terminal country code           */
   bf_146termtxndate = 146,    /* terminal transaction date       */
   bf_147cryptamt    = 147,    /* cryptogram amount               */
   bf_148cryptcurcd  = 148,    /* cryptogram currency code        */
   bf_149cryptcash   = 149     /* cryptogram cash back amount     */
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

//For visa
void	vBCDMove(enum bitfields fieldid);
void	RspMovevBCD (enum bitfields fieldid);
void	vBCDMove_Field34(enum bitfields fieldid);
void	RspMovevBCD_Field34(enum bitfields fieldid);
INT genutil_hex_to_int (pCHAR buf_ptr, INT buf_len  );
INT		Move_Always(enum bitfields, BYTE);
void    vEBCDICMove( enum bitfields fieldid );
INT		Move_IfThere(enum bitfields, BYTE);
INT		vMove_IfThere(enum bitfields fieldid, BYTE p_trantype);
INT		Move_BCD_Amount_IfThere(enum bitfields fieldid, BYTE p_trantype);
void	GenericMove(enum bitfields fieldid);
INT		MoveEMV_IfThere(enum bitfields fieldid, BYTE p_trantype);
INT		Move23_IfThere( enum bitfields fieldid, BYTE p_trantype );
void	RspMoveGeneric(enum bitfields fieldid);
void	RspMovevData (enum bitfields fieldid);
int		get_variable_length( unsigned char *DataField, int FieldLen );
void	vDataMove(enum bitfields fieldid);
void    parsed_mssg();
void    EBCDICMove( enum bitfields fieldid );
void	RspEBCDICvMove (enum bitfields fieldid);
void	RspEBCDICMove (enum bitfields fieldid);
INT		CondExpd( enum bitfields bitfield, BYTE p_trantype );
INT		CondTrack1( enum bitfields bitfield, BYTE p_trantype );
void	Generate_CUP_EMV_Details();
void genutil_VISA_EMV_To_Auth_Tx( pCHAR emv_raw, INT total_len, pAUTH_TX p_auth_tx );
INT		hhutil_parse_host_response_msg(pAUTH_TX p_auth_tx);
INT		move_iso_response_to_auth_struct(CHAR nmi_code[5], pAUTH_TX p_auth_tx);
INT		parse_track2( pAUTH_TX p_auth_tx, pCHAR error_msg );
void	trim_spaces( pBYTE pString );
void	find_cavv_flag( BYTE len, pBYTE de44, pBYTE cavv_result );
void	find_carc_flag( BYTE len, pBYTE de44, pBYTE carc_result );
void	find_cvv2_result_code( BYTE len, pBYTE de44, pBYTE cvv2_result );
INT isnum(pCHAR string);
BOOLEAN blank_or_null_string(pCHAR astr, INT max_len);
void	get_balances( pAddl_Amts p_acct_bal, pAUTH_TX p_auth_tx );
void	genutil_VISA_EMV_To_Auth_Tx( BYTE * emv_raw, INT total_len, pAUTH_TX p_auth_tx );
void	Field62Response( pAUTH_TX p_auth_tx );
INT ParseField63( pCHAR f63, pAUTH_TX p_auth_tx );
void	LogStipReasonCode( pAUTH_TX p_auth_tx );
INT		Field116_Handler( pCHAR f116, pAUTH_TX p_auth_tx );
void	Field126Parser(  pAUTH_TX p_auth_tx, pBYTE cvv2_data  );
void	Rj_with_lead_zeros( pCHAR str, INT FieldWidth );
void	genutil_EMV_To_Auth_Tx( BYTE * emv_raw, INT total_len, pAUTH_TX p_auth_tx );
void	genutil_bcd_to_asc (BYTE bstring[], INT blen, pCHAR astr, INT astr_len);
BYTE	ascii_ebcdic_conv(BYTE conv_char, char * conv_type );
BYTE	ascii_ebcdic_conv_trk1( unsigned char conv_char, char *conv_type,unsigned char fldSep);
void	genutil_bin_to_ascii (BYTE bstring[], INT blen, pCHAR astr, INT astr_len);
INT		Move_Never(enum bitfields bitfield, BYTE p_trantype);

/*****************************/

//static  struct bitfield_data *BFData; 
// From Visa2
static  struct bitfield_data BFData[] = {
   {
   bf_2pan,                      /* field */
   HOST_MESSAGE.pan_len,         /* data to move */
   1,                            /* # of bytes in pan len field */
   Move_Always,                  /* mandatory    */
   vBCDMove,                     /* move routine */
   RspMovevBCD                   /* response routine */
   },
   {
   bf_3proc,                     /* field */
   HOST_MESSAGE.proccode,        /* data to move */
   3,                            /* # of bytes in visa iso field */
   Move_Always,                  /* conditional */
   GenericMove,                  /* move routine */
   RspMoveGeneric                /* response routine */
   },
   {
   bf_4amount,                   /* field */
   HOST_MESSAGE.amount_bin,      /* data to move */
   6,                            /* # of bytes in visa iso field */
   Move_Always,                  /* conditional */
   GenericMove,                  /* move routine */
   RspMoveGeneric                /* response routine */
   },
   {
   bf_5settleamt,                /* field */
   HOST_MESSAGE.settle_amt,      /* data to move */
   6,                            /* # of bytes in visa iso field */
   Move_IfThere,                 /* conditional */
   GenericMove,                  /* move routine */
   RspMoveGeneric                /* response routine */
   },
   {
   bf_6crdbillamt,               /* field */
   HOST_MESSAGE.card_bill_amt,   /* data to move */
   6,                            /* # of bytes in visa iso field */
   Move_BCD_Amount_IfThere,      /* conditional */ // TF PHANI ..changed from Move_IfThere
   GenericMove,                  /* move routine */
   RspMoveGeneric                /* response routine */
   },
   {
   bf_7trntime,                  /* field */
   HOST_MESSAGE.gmt_time,        /* data to move */
   5,                            /* # of bytes in visa iso field */
   Move_Always,                  /* conditional */
   GenericMove,                  /* move routine */
   RspMoveGeneric                /* response routine */
   },
   {
   bf_9setconvrate,              /* field */
   HOST_MESSAGE.conv_rate_stl,   /* data to move */
   4,                            /* # of bytes in visa iso field */
   Move_IfThere,                 /* conditional */
   GenericMove,                  /* move routine */
   RspMoveGeneric                /* response routine */
   },
   {
   bf_10convrate,                /* field */
   HOST_MESSAGE.conv_rate,       /* data to move */
   4,                            /* # of bytes in visa iso field */
   Move_IfThere,                 /* conditional */
   GenericMove,                  /* move routine */
   RspMoveGeneric                /* response routine */
   },
   {
   bf_11stan,                    /* field */
   HOST_MESSAGE.stan_bin,        /* data to move */
   3,                            /* # of bytes in visa iso field */
   Move_Always,                  /* conditional */
   GenericMove,                  /* move routine */
   RspMoveGeneric                /* response routine */
   },
   {
   bf_12time,                    /* field */
   HOST_MESSAGE.time_bin,        /* data to move */
   3,                            /* # of bytes in visa iso field */
   Move_IfThere,                 /* conditional */
   GenericMove,                  /* move routine */
   RspMoveGeneric                /* response routine */
   },
   {
   bf_13date,                    /* field */
   HOST_MESSAGE.date_bin,         /* data to move */
   2,                            /* # of bytes in visa iso field */
   Move_IfThere,                 /* conditional */
   GenericMove,                  /* move routine */
   RspMoveGeneric                /* response routine */
   },
   {
   bf_14expiry,                  /* field */
   HOST_MESSAGE.date_expd,       /* data to move */
   2,                            /* # of bytes in visa iso field */
   CondExpd,                     /* conditional */
   GenericMove,                  /* move routine */
   RspMoveGeneric                /* response routine */
   },
   {
   bf_15datestl,                 /* field */
   HOST_MESSAGE.date_settle,     /* data to move */
   2,                            /* # of bytes in visa iso field */
   Move_IfThere,                 /* conditional */
   GenericMove,                  /* move routine */
   RspMoveGeneric                /* response routine */
   },
   {
   bf_16convdate,                /* field */
   HOST_MESSAGE.conv_date,       /* data to move */
   2,                            /* # of bytes in visa iso field */
   Move_IfThere,                 /* conditional */
   GenericMove,                  /* move routine */
   RspMoveGeneric                /* response routine */
   },
   {
   bf_18sicnumber,               /* field */
   HOST_MESSAGE.sic_number,      /* data to move */
   2,                            /* # of bytes in visa iso field */
   Move_Always,                  /* mandatory    */
   GenericMove,                  /* move routine */
   RspMoveGeneric                /* response routine */
   },
   {
   bf_19ctrycd,                  /* field */
   HOST_MESSAGE.ctry_code,       /* data to move */
   2,                            /* # of bytes in visa iso field */
   Move_Always,                  /* mandatory    */
   GenericMove,                  /* move routine */
   RspMoveGeneric                /* response routine */
   },
   {
   bf_20panctrycd,                    /* field */
   HOST_MESSAGE.pan_ctry_cd,     /* data to move */
   2,                            /* # of bytes in visa iso field */
   Move_IfThere,                 /* conditional */
   GenericMove,                  /* move routine */
   RspMoveGeneric                /* response routine */
   },
   {
   bf_22pose,                    /* field */
   HOST_MESSAGE.posentry,         /* data to move */
   2,                            /* # of bytes in visa iso field */
   Move_Always,                  /* conditional */
   GenericMove,                  /* move routine */
   RspMoveGeneric                /* response routine */
   },
   {
   bf_23cardseqnum,              /* field */
   HOST_MESSAGE.cardseqnum,      /* data to move */
   2,                            /* # of bytes in visa iso field */
   Move23_IfThere,               /* conditional */
   GenericMove,                  /* move routine */
   RspMoveGeneric                /* response routine */
   },
   {
   bf_25pocc,                    /* field */
   HOST_MESSAGE.pos_condcd,      /* data to move */
   1,                            /* # of bytes in visa iso field */
   Move_Always,                  /* conditional */
   GenericMove,                  /* move routine */
   RspMoveGeneric                /* response routine */
   },
   {
   bf_26pincc,                 /* field */
   HOST_MESSAGE.pin_captcd,      /* data to move */
   1,                            /* # of bytes in visa iso field */
   Move_Always,                  /* conditional */
   GenericMove,                  /* move routine */
   RspMoveGeneric                /* response routine */
   },
/* Girija Y TF, Apr 2009 */
   {
   bf_28feeamount,				/* field */
   HOST_MESSAGE.fee_amount,      /* data to move */
   9,                            /* # of bytes in visa iso field */
   Move_IfThere,                  /* conditional */
   EBCDICMove,                  /* move routine */
   RspEBCDICMove                /* response routine */
   },         
   {
   bf_32idcode,                  /* field */
   HOST_MESSAGE.acq_inst_len,    /* data to move */
   1,                            /* # of bytes in visa iso field */
   Move_Always,                  /* conditional */
   vBCDMove,                     /* move routine */
   RspMovevBCD                   /* response routine */
   },
   {
   bf_33fwdidcode,                    /* field */
   HOST_MESSAGE.fwd_inst_len,    /* data to move */
   1,                            /* # of bytes in visa iso field */
   Move_IfThere,                 /* conditional */
   vBCDMove,                     /* move routine */
   RspMovevBCD                   /* response routine */
   },
   {
   bf_34panext,                    /* field */
   HOST_MESSAGE.pan_ext_len,    /* data to move */
   2,                            /* # of bytes in visa iso field */		/* DE 34 */
   Move_IfThere,                 /* conditional */
   vBCDMove_Field34,                     /* move routine */
   RspMovevBCD_Field34                   /* response routine */
   },
   {
   bf_35track2,                  /* field */
   HOST_MESSAGE.track2_len,      /* data to move */
   1,                            /* # of bytes in track2 len field */
   vMove_IfThere,                 /* conditional */
   vBCDMove,                     /* move routine */
   RspMovevBCD                   /* response routine */
   },
   {
   bf_37rrn,                     /* field */
   HOST_MESSAGE.rrn,             /* data to move */
   12,                           /* # of bytes in visa iso field */
   Move_Always,                  /* conditional */
   EBCDICMove,                   /* move routine */
   RspEBCDICMove                 /* response routine */
   },
   {
   bf_38authcode,                /* field */
   HOST_MESSAGE.authcode,        /* data to move */
   6,                            /* # of bytes in visa iso field */
   Move_IfThere,                 /* conditional */
   EBCDICMove,                   /* move routine */
   RspEBCDICMove                 /* response routine */
   },
   {
   bf_39respcode,                /* field */
   HOST_MESSAGE.responsecd,      /* data to move */
   2,                            /* # of bytes in visa iso field */
   Move_IfThere,                 /* conditional */
   EBCDICMove,                   /* move routine */
   RspEBCDICMove                 /* response routine */
   },
   {
   bf_40src,                     /* field */
   HOST_MESSAGE.src,             /* data to move */
   3,                            /* # of bytes in visa iso field */
   Move_Always,                  /* conditional */
   EBCDICMove,                   /* move routine */
   RspEBCDICMove                 /* response routine */
   },
   {
   bf_41termid,                  /* field */
   HOST_MESSAGE.termid,          /* data to move */
   8,                            /* # of bytes in visa iso field */
   Move_IfThere,                 /* conditional */
   EBCDICMove,                   /* move routine */
   RspEBCDICMove                 /* response routine */
   },
   {
   bf_42cardacc,                 /* field */
   HOST_MESSAGE.cardaccid,       /* data to move */
   15,                           /* # of bytes in visa iso field */
   Move_IfThere,                 /* conditional */
   EBCDICMove,                   /* move routine */
   RspEBCDICMove                 /* response routine */
   },
   {
   bf_43cardaccname,             /* field */
   HOST_MESSAGE.cardaccname,     /* data to move */
   40,                           /* # of bytes in visa iso field */
   Move_IfThere,                  /* conditional */
   EBCDICMove,                   /* move routine */
   RspEBCDICMove                 /* response routine */
   },
   {
   bf_44respdata,                /* field */
   HOST_MESSAGE.addl_rsp_len,    /* data to move */
   1,                            /* # of bytes in resp data len field */
   Move_IfThere,                 /* conditional */
   vEBCDICMove,                  /* move routine */
   RspEBCDICvMove                /* response routine */
   },
   {
   bf_45track1,                  /* field */
   HOST_MESSAGE.track1_len,      /* data to move */
   1,                            /* # of bytes in track1 len field */
   CondTrack1,                   /* conditional */
   vEBCDICMove,                  /* move routine */
   RspEBCDICvMove                /* response routine */
   },
   {
   bf_48addl_data,               /* field */
   HOST_MESSAGE.addl_data_len,   /* data to move */
   1,                            /* # of bytes in addl data len field */
   Move_IfThere,                 /* conditional    */
   vDataMove,                    /* move routine */
   RspMovevData                  /* response routine */
   },
   {
   bf_49trncurcd,                /* field */
   HOST_MESSAGE.tran_curr_cd,    /* data to move */
   2,                            /* # of bytes in visa iso field */
   Move_Always,                  /* mandatory    */
   GenericMove,                  /* move routine */
   RspMoveGeneric                /* response routine */
   },
   {
   bf_50stlmtcurrcd,             /* field */
   HOST_MESSAGE.settle_cur_cd,   /* data to move */
   2,                            /* # of bytes in visa sms iso field */
   Move_Always,                 /* conditional  */
   GenericMove,                   /* move routine */
   RspMoveGeneric                 /* response routine */
   },
   {
   bf_51crdcurcd,                /* field */
   HOST_MESSAGE.card_curr_cd,    /* data to move */
   2,                            /* # of bytes in visa iso field */
   Move_IfThere,                 /* conditional */
   GenericMove,                  /* move routine */
   RspMoveGeneric                /* response routine */
   },
   {
   bf_52pinblock,                /* field */
   HOST_MESSAGE.pin_block,       /* data to move */
   8,                            /* # of bytes in visa iso field */
   Move_IfThere,                 /* conditional */
   GenericMove,                  /* move routine */
   RspMoveGeneric                /* response routine */
   },
   {
   bf_53seccntl,                 /* field */
   HOST_MESSAGE.sec_cntl,        /* data to move */
   8,                            /* # of bytes in visa iso field */
   Move_IfThere,                 /* conditional */
   GenericMove,                  /* move routine */
   RspMoveGeneric                /* response routine */
   },
   {
   bf_54addlamts,                /* field */
   HOST_MESSAGE.addl_len,        /* data to move */
   1,                            /* # of bytes in addl len field */
   Move_IfThere,                 /* conditional */
   vEBCDICMove,                  /* move routine */
   RspEBCDICvMove                /* response routine */
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
   bf_56pad,                	 /* field */
   HOST_MESSAGE.PAD_len,         /* data to move */
   2,                            /* # of bytes in addl len field */
   Move_IfThere,                 /* conditional */
   vDataMove,                    /* move routine */
   RspMovevData                  /* response routine */
   },
   {
   bf_59posdata,                 /* field */
   HOST_MESSAGE.posdata_len,     /* data to move */
   1,                            /* # of bytes in addl len field */
   Move_IfThere,                 /* conditional */
   vEBCDICMove,                  /* move routine */
   RspEBCDICvMove                /* response routine */
   },
   {
   bf_60priv60,                  /* field */
   HOST_MESSAGE.priv60_len,      /* data to move */
   1,                            /* # of bytes in priv60 len field */
   Move_IfThere,                 /* conditional */
   vBCDMove,                     /* move routine */
   RspMovevBCD                   /* response routine */
   },
   {
   bf_61priv61,                  /* field */
   HOST_MESSAGE.priv61_len,      /* data to move */
   1,                            /* # of bytes in priv61 len field */
   Move_IfThere,                 /* conditional */
   vBCDMove,                     /* move routine */
   RspMovevBCD                   /* response routine */
   },
   {
   bf_62priv62,                  /* field */
   HOST_MESSAGE.priv62_len,      /* data to move */
   1,                            /* # of bytes in priv62 len field */
   Move_IfThere,                 /* conditional */
   vDataMove,                    /* move routine */
   RspMovevData                  /* response routine */
   },
   {
   bf_63priv63,                  /* field */
   HOST_MESSAGE.priv63_len,      /* data to move */
   1,                            /* # of bytes in priv63 len field */
   Move_IfThere,                 /* conditional */
   vDataMove,                    /* move routine */
   RspMovevData                  /* response routine */
   },
   {
   bf_70nmicode,                 /* field */
   HOST_MESSAGE.nmi_code,        /* data to move */
   2,                            /* # of bytes in visa iso field */
   Move_Always,                  /* mandatory    */
   GenericMove,                  /* move routine */
   RspMoveGeneric                /* response routine */
   },
   {
   bf_73action,                  /* field */
   HOST_MESSAGE.action,          /* data to move */
   3,                            /* # of bytes in visa iso field */
   Move_IfThere,                 /* conditional */
   GenericMove,                  /* move routine */
   RspMoveGeneric                /* response routine */
   },
   {
   bf_90orgdata,                 /* field */
   HOST_MESSAGE.orig_data,       /* data to move */
   21,                           /* # of bytes in visa iso field */
   Move_Always,                  /* mandatory    */
   GenericMove,                  /* move routine */
   RspMoveGeneric                /* response routine */
   },
   {
   bf_91fileupdate,              /* field */
   HOST_MESSAGE.file_update,     /* data to move */
   1,                            /* # of bytes in visa iso field */
   Move_Always,                  /* conditional */
   EBCDICMove,                   /* move routine */
   RspEBCDICMove                 /* response routine */
   },
   {
   bf_93rspind,                  /* field */
   HOST_MESSAGE.resp_ind,        /* data to move */
   5,                            /* # of bytes in visa iso field */
   Move_IfThere,                 /* conditional  */
   EBCDICMove,                   /* move routine */
   RspEBCDICMove                 /* response routine */
   },
   {
   bf_95replamts,                /* field */
   HOST_MESSAGE.repl_amts,       /* data to move */
   42,                           /* # of bytes in visa iso field */
   Move_IfThere,                 /* conditional  */
   EBCDICMove,                   /* move routine */
   RspEBCDICMove                 /* response routine */
   },
   {
   bf_96msgsec,                  /* field */
   HOST_MESSAGE.msgsec,          /* data to move */
   8,                            /* # of bytes in visa iso field */
   Move_Always,                  /* mandatory    */
   GenericMove,                  /* move routine */
   RspMoveGeneric                /* response routine */
   },
   {
   bf_100rcvid,                  /* field */
   HOST_MESSAGE.recv_inst_len,   /* data to move */
   1,                            /* # of bytes in visa iso field */
   Move_IfThere,                 /* conditional */
   vBCDMove,                     /* move routine */
   RspMovevBCD                   /* response routine */
   },
   {
   bf_101fname,                  /* field */
   HOST_MESSAGE.fname_len,       /* data to move */
   1,                            /* # of bytes in track1 len field */
   Move_IfThere,                 /* conditional */
   vEBCDICMove,                  /* move routine */
   RspEBCDICvMove                /* response routine */
   },
   {
   bf_102acct1,                  /* field */
   HOST_MESSAGE.acct_id_1_len,   /* data to move */
   1,                            /* # of bytes in visa iso field */
   Move_IfThere,                 /* conditional */
   vEBCDICMove,                  /* move routine */
   RspEBCDICvMove                /* response routine */
   },
   {
   bf_103acct2,                  /* field */
   HOST_MESSAGE.acct_id_2_len,   /* data to move */
   1,                            /* # of bytes in visa iso field */
   Move_IfThere,                 /* conditional */
   vEBCDICMove,                  /* move routine */
   RspEBCDICvMove                /* response routine */
   },
   {
   bf_104trandesc,               /* field */
   HOST_MESSAGE.trandesc_len,    /* data to move */
   1,                            /* # of bytes in track1 len field */
   Move_IfThere,                 /* conditional */
   vDataMove,                  /* move routine */
   RspMovevData                /* response routine */
   },
   {
   bf_111addl_tran,               /* field */
   HOST_MESSAGE.addl_tran_len,    /* data to move */
   2,                            /* # of bytes in track1 len field */
   Move_IfThere,                 /* conditional */
   vBCDMove_Field34,             /* move routine */
   RspMovevBCD_Field34           /* response routine */
   },
   {
   bf_116issref,                 /* field */
   HOST_MESSAGE.issref_len,      /* data to move */
   1,                            /* # of bytes in visa iso field */
   Move_IfThere,                 /* conditional */
   vDataMove,                    /* move routine */
   RspMovevData                  /* response routine */
   },
   {
   bf_118intractry,              /* field */
   HOST_MESSAGE.intra_ctry_len,  /* data to move */
   1,                            /* # of bytes in visa iso field */
   Move_Never,                   /* conditional */
   vDataMove,                    /* move routine */
   RspMovevData                  /* response routine */
   },
   {
   bf_120orgmsgid,               /* field */
   HOST_MESSAGE.org_msg_id,      /* data to move */
   3,                            /* # of bytes in visa iso field */
   Move_IfThere,                 /* conditional */
   vBCDMove,                     /* move routine */
   RspMovevBCD                   /* response routine */
   },
   {
   bf_121issid,                  /* field */
   HOST_MESSAGE.iss_inst_len,    /* data to move */
   1,                            /* # of bytes in visa iso field */
   Move_IfThere,                   /* conditional */
   vEBCDICMove,                  /* move routine */
   RspEBCDICvMove                /* response routine */
   },
   {
   bf_122openuse,                /* field */
   HOST_MESSAGE.open_use_len,    /* data to move */
   1,                            /* # of bytes in visa iso field */
   Move_IfThere,                 /* conditional */
   EBCDICMove,                   /* move routine */
   RspEBCDICMove                 /* response routine */
   },
   {
   bf_123addverf,                /* field */
   HOST_MESSAGE.addr_verf_len,    /* data to move */
   1,                            /* # of bytes in visa iso field */
   Move_IfThere,                 /* conditional */
   vEBCDICMove,                  /* move routine */
   RspEBCDICvMove                /* response routine */
   },
   {
   bf_124freeform,               /* field */
   HOST_MESSAGE.free_text_len,   /* data to move */
   1,                            /* # of bytes in visa iso field */
   Move_IfThere,                 /* conditional */
   vEBCDICMove,                  /* move routine */
   RspEBCDICvMove                /* response routine */
   },
   {
    bf_125supinfo,               /* field */
    HOST_MESSAGE.sup_info_len,   /*data to move */
    1,                           /* # of bytes in visa iso field */ 
    Move_IfThere,                /* conditional */
    vDataMove,                 /* move routine */
    RspMovevData               /* response routine */
   },

   {
   bf_126priv126,                /* field */
   HOST_MESSAGE.de126_len,       /* data to move */
   1,                            /* # of bytes in priv60 len field */
   vMove_IfThere,                /* conditional */
   vDataMove,                    /* move routine */
   RspMovevData                  /* response routine */
   },
   {
   bf_127filerecs,               /* field */
   HOST_MESSAGE.file_rec_len,    /* data to move */
   1,                            /* # of bytes in visa iso field */
   Move_IfThere,                 /* conditional */
   vDataMove,                    /* move routine */
   RspMovevData                  /* response routine */
   },

   {
   bf_130termcap,                /* field */
   HOST_MESSAGE.term_cap_prof,   /* data to move */
   3,                            /* # of bytes in visa iso field */
   MoveEMV_IfThere,              /* conditional */
   GenericMove,                  /* move routine */
   RspMoveGeneric                /* response routine */
   },

   {
   bf_131tvr,                    /* field */
   HOST_MESSAGE.tvr,             /* data to move */
   5,                            /* # of bytes in visa iso field */
   Move_IfThere,                 /* conditional */
   GenericMove,                  /* move routine */
   RspMoveGeneric                /* response routine */
   },

   {
   bf_132unpredict,              /* field */
   HOST_MESSAGE.unpredictable,   /* data to move */
   4,                            /* # of bytes in visa iso field */
   MoveEMV_IfThere,              /* conditional */
   GenericMove,                  /* move routine */
   RspMoveGeneric                /* response routine */
   },

   {
   bf_133termserial,             /* field */
   HOST_MESSAGE.tid_serial,      /* data to move */
   8,                            /* # of bytes in visa iso field */
   MoveEMV_IfThere,              /* conditional */
   EBCDICMove,                   /* move routine */
   RspEBCDICMove                 /* response routine */
   },

   {
   bf_134visadata,               /* field */
   HOST_MESSAGE.visa_data_len,   /* data to move */
   1,                            /* # of bytes in visa iso field */
   vMove_IfThere,                /* conditional */
   vDataMove,                    /* move routine */
   RspMovevData                  /* response routine */
   },

   {
   bf_135issdata,                /* field */
   HOST_MESSAGE.iss_data_len,    /* data to move */
   1,                            /* # of bytes in visa iso field */
   Move_IfThere,                 /* conditional */
   vDataMove,                    /* move routine */
   RspMovevData                  /* response routine */
   },

   {
   bf_136crytogram,              /* field */
   HOST_MESSAGE.cryptogram,      /* data to move */
   8,                            /* # of bytes in visa iso field */
   MoveEMV_IfThere,              /* conditional */
   GenericMove,                  /* move routine */
   RspMoveGeneric                /* response routine */
   },

   {
   bf_137apptranctr,             /* field */
   HOST_MESSAGE.app_tran_ctr,    /* data to move */
   2,                            /* # of bytes in visa iso field */
   MoveEMV_IfThere,              /* conditional */
   GenericMove,                  /* move routine */
   RspMoveGeneric                /* response routine */
   },

   {
   bf_138app_profile,            /* field */
   HOST_MESSAGE.app_profile,     /* data to move */
   2,                            /* # of bytes in visa iso field */
   MoveEMV_IfThere,              /* conditional */
   GenericMove,                  /* move routine */
   RspMoveGeneric                /* response routine */
   },

   {
   bf_139arpcresp,               /* field */
   HOST_MESSAGE.arpc_resp,       /* data to move */
   10,                           /* # of bytes in visa iso field */
   MoveEMV_IfThere,              /* conditional */
   GenericMove,                  /* move routine */
   RspMoveGeneric                /* response routine */
   },

   {
   bf_140issauth,                /* field */
   HOST_MESSAGE.iss_auth_len,    /* data to move */
   1,                            /* # of bytes in visa iso field */
   Move_IfThere,                 /* conditional */
   vDataMove,                    /* move routine */
   RspMovevData                  /* response routine */
   },

   {
   bf_142iss_script,             /* field */
   HOST_MESSAGE.iss_script_len,  /* data to move */
   1,                            /* # of bytes in visa iso field */
   Move_IfThere,                 /* conditional */
   vDataMove,                    /* move routine */
   RspMovevData                  /* response routine */
   },

   {
   bf_143iss_result,             /* field */
   HOST_MESSAGE.iss_result_len,  /* data to move */
   1,                            /* # of bytes in visa iso field */
   Move_IfThere,                 /* conditional */
   vDataMove,                    /* move routine */
   RspMovevData                  /* response routine */
   },

   {
   bf_144crytpotype,             /* field */
   HOST_MESSAGE.crypto_type,     /* data to move */
   1,                            /* # of bytes in visa iso field */
   MoveEMV_IfThere,              /* conditional */
   GenericMove,                  /* move routine */
   RspMoveGeneric                /* response routine */
   },

   {
   bf_145tcountry,               /* field */
   HOST_MESSAGE.tid_country,     /* data to move */
   2,                            /* # of bytes in visa iso field */
   MoveEMV_IfThere,              /* conditional */
   GenericMove,                  /* move routine */
   RspMoveGeneric                /* response routine */
   },

   {
   bf_146termtxndate,            /* field */
   HOST_MESSAGE.tid_tran_date,   /* data to move */
   3,                            /* # of bytes in visa iso field */
   MoveEMV_IfThere,              /* conditional */
   GenericMove,                  /* move routine */
   RspMoveGeneric                /* response routine */
   },

   {
   bf_147cryptamt,               /* field */
   HOST_MESSAGE.crypto_amt,      /* data to move */
   6,                            /* # of bytes in visa iso field */
   MoveEMV_IfThere,              /* conditional */
   GenericMove,                  /* move routine */
   RspMoveGeneric                /* response routine */
   },

   {
   bf_148cryptcurcd,             /* field */
   HOST_MESSAGE.crypto_curr,     /* data to move */
   2,                            /* # of bytes in visa iso field */
   MoveEMV_IfThere,              /* conditional */
   GenericMove,                  /* move routine */
   RspMoveGeneric                /* response routine */
   },

   {
   bf_149cryptcash,              /* field */
   HOST_MESSAGE.crypto_cashbk,   /* data to move */
   6,                            /* # of bytes in visa iso field */
   MoveEMV_IfThere,              /* conditional */
   GenericMove,                  /* move routine */
   RspMoveGeneric                /* response routine */
   },

   {
   bf_done,                      /* field */
   NULL,                         /* data to move */
   99,                           /* # of bytes in visa iso field */
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
   char asc_len[3]={0};

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
        // cLen = ascii_ebcdic_conv( ReqRsp_Buffer[reqidx+i], E2A );
        // movelen += (int)(cLen - 0x30) * (pow(10,(double)(FieldLen-i-1)));
		  if (fieldid == 56)
			genutil_bin_to_ascii(&ReqRsp_Buffer[reqidx+i],1,asc_len,2);
		  else
		  genutil_bin_to_ascii(&ReqRsp_Buffer[reqidx+i],FieldLen,asc_len,FieldLen*2);
		  if(fieldid==48 || fieldid == 62 || fieldid == 63)
		  {
			movelen=genutil_hex_to_int(asc_len,strlen(asc_len));
		  }
		  else
		  {
			 movelen = atoi(asc_len);
		  }
		  movelen=genutil_hex_to_int(asc_len,strlen(asc_len));
		  if(fieldid == 56)
			 memset(&pBF->p_bd_pdata[i], ReqRsp_Buffer[reqidx+i], 1);
		  else
		  memset(&pBF->p_bd_pdata[i], ReqRsp_Buffer[reqidx], 1);// raghu for copieng the length correctly and converting in parse_mssg fun.
      }
	  
      /* Convert length from EBCDIC and move to HOST_MESSAGE. */
      for( i=0; i<FieldLen; i++,reqidx++ )
      {

        // cFieldData = ascii_ebcdic_conv( (unsigned char)(ReqRsp_Buffer[reqidx]), E2A );
        // memset(&pBF->p_bd_pdata[i], ReqRsp_Buffer[reqidx], 1);
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

static  void vBCDMove_Field34(enum bitfields fieldid)
{
   struct bitfield_data *pBF;
   INT                  movelen;
   INT  dec_f34	   = 0;
   CHAR hex_f34[5] = {};

   /*---------------------------------------------------*/
   /* find pointer to bitfield info for this bitfield...*/
   /*---------------------------------------------------*/
   pBF = Find_Bitfield_Table(fieldid);

   /* Determine number of bytes to move. */
   if (pBF != NULL)
   {
		   /*movelen = genutil_bcd_to_int(&pBF->p_bd_pdata[0], pBF->bd_fieldlen);
		   sprintf(hex_f34, "%ld",movelen);
		   dec_f34 = Hex_to_Dec(hex_f34); */

		   dec_f34 = pBF->p_bd_pdata[1];

		   memcpy(&ReqRsp_Buffer[reqidx], &pBF->p_bd_pdata[0], pBF->bd_fieldlen);
		   reqidx = reqidx + 2;

          memcpy(&ReqRsp_Buffer[reqidx], &pBF->p_bd_pdata[2], dec_f34);

      reqidx = reqidx + dec_f34;   /* increment buffer pointer */
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

static  void RspMovevBCD_Field34 (enum bitfields fieldid)
{
   struct bitfield_data *pBF;
   INT                  movelen;
   INT  dec_f34 = 0;
   CHAR hex_f34[5]= {};
   CHAR bf_34[3] = {};

   /*---------------------------------------------------*/
   /* find pointer to bitfield info for this bitfield...*/
   /*---------------------------------------------------*/
   pBF = Find_Bitfield_Table(fieldid);

   if (pBF != NULL)
   {
    	  	  	  /* DE 34 */
		/*movelen = genutil_bcd_to_int(&ReqRsp_Buffer[reqidx], pBF->bd_fieldlen);
		sprintf(hex_f34, "%ld",movelen);
		dec_f34 = Hex_to_Dec(hex_f34);*/
				/* Copy length */

		dec_f34 = ReqRsp_Buffer[reqidx+1];

		memcpy(&pBF->p_bd_pdata[0], &ReqRsp_Buffer[reqidx], pBF->bd_fieldlen );
		reqidx = reqidx +2;

      if (( reqidx + dec_f34) <= COMM_BUF_SZE)
      {			/* Copy data */
			memcpy(&pBF->p_bd_pdata[2], &ReqRsp_Buffer[reqidx], dec_f34);
    	 reqidx = reqidx + dec_f34;
      }
      else
         invalid_msg_format = true;
   }
} /* end RspMovevBCD_Field34 */

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
   char temp_len[4];
   /*---------------------------------------------------*/
   /* Find pointer to bitfield info for this bitfield...*/
   /*---------------------------------------------------*/
   pBF = Find_Bitfield_Table(fieldid);
   if (pBF != NULL)
   {
      FieldLen = pBF->bd_fieldlen;

      /* Determine number of bytes to move. */
	  if (fieldid == 44 || fieldid == 101)
	  {
			genutil_bin_to_ascii(&ReqRsp_Buffer[reqidx],1,temp_len,sizeof(temp_len));
			movelen = genutil_hex_to_int(temp_len,strlen(temp_len));
	  }
	  else if( fieldid == 54 || fieldid == 59 )
	  {
		  movelen = ReqRsp_Buffer[reqidx];
	  }
	  else
	  {
		 for( i=0,movelen=0; i<FieldLen; i++ )
		{
			 cLen = ascii_ebcdic_conv( ReqRsp_Buffer[reqidx+i], E2A );
			movelen += (int)(cLen - 0x30) * (pow(10,(double)(FieldLen-i-1)));
		}
	  }
      movelen += pBF->bd_fieldlen;

      if ( (reqidx + movelen) <= COMM_BUF_SZE )
      {
         /* Move the data, converting it from EBCDIC to ASCII. */
         for( i=0; i<movelen; i++,reqidx++ )
         {
			if (i== 0 && (fieldid == 44 || fieldid == 54 || fieldid == 59 || fieldid == 101 ))
			{
				pBF->p_bd_pdata[0] = ReqRsp_Buffer[reqidx];
			}
			else
			{
				cFieldData = ascii_ebcdic_conv( (unsigned char)(ReqRsp_Buffer[reqidx]), E2A );
				memset(&pBF->p_bd_pdata[i], cFieldData, 1);
			}
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
	BYTE temp_data[500]={0};
		char asci_cnvrtd_data[100]={0};
		char buffer [256]={0};
		int len=0;
	memset(Line,0,sizeof(Line));
	
	/* For visa instead of storing the parsed msg in HOSTMESSAGE only, We used a
	move_iso_response_to_auth_struct function and kept all the parsed things in the auth structure and used Auth structure */
	
	strcpy (Line[0], "000 MsgType				: ");
	strncat(Line[0],(char *)auth_tx.TLF01_details.message_type,4);

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
			//if(strlen((char *)auth_tx.TLF01_details.card_num) != 0)
			if(BitFieldArray[bf_2pan] == 1)                    
			{	
 				strcpy(temp_len,(char *)auth_tx.TLF01_details.card_num_len);
				len=atoi(temp_len);
				strcat (Line[bitCount], FieldStrings [bitCount]);
				strncat (Line[bitCount],(char *)auth_tx.TLF01_details.card_num,len);				
			}	
		break;
		
		case bf_3proc :
			//if(strlen((char *)auth_tx.TLF01_details.processing_code) != 0)
			if(BitFieldArray[bf_3proc] == 1)
			{
				
				strcat (Line[bitCount], FieldStrings [bitCount]);
				strncat (Line[bitCount],(char *)auth_tx.TLF01_details.processing_code, 6);				
			}
		break;
		//bf_4amount stored in  auth_tx->total_amount but it vl b cnvrtd ltr so copied into 
		// auth_tx.reconciliation_amount for printg
		case bf_4amount :
			  // if(strlen((char *)auth_tx.TLF01_details.total_amount) != 0 &&
			   //strncmp((char *)auth_tx.TLF01_details.message_type, "0800", 4) != 0)
			   if(BitFieldArray[bf_4amount] == 1)
			{

				strcat (Line[bitCount], FieldStrings [bitCount]);
				strncat (Line[bitCount],(char *)auth_tx.TLF01_details.total_amount, 12);
			}
		break;
		
		case bf_6crdbillamt :
			//if(strlen((char *)auth_tx.ch_billing_amt) != 0 &&
			  // strncmp((char *)auth_tx.TLF01_details.message_type, "0800", 4) != 0)
			   if( BitFieldArray[bf_6crdbillamt] == 1)
			{
				strcat (Line[bitCount], FieldStrings [bitCount]);
				strncat (Line[bitCount], (char *)auth_tx.ch_billing_amt ,12);
			}
			break ;

		case bf_7trntime :
			//if(strlen((char *)auth_tx.TLF01_details.transmission_timestamp) != 0)
			 if(BitFieldArray[bf_7trntime] == 1)
			{
				strcat (Line[bitCount], FieldStrings [bitCount]);
				strncat (Line[bitCount], (char *)auth_tx.TLF01_details.transmission_timestamp,10);
			}
		break;

		case bf_10convrate :
			//if(strlen((char *)auth_tx.ch_billing_conv_rate) != 0 &&
			  // strncmp((char *)auth_tx.TLF01_details.message_type, "0800", 4) != 0)
			if( BitFieldArray[bf_10convrate] == 1 &&
			   strncmp((char *)auth_tx.TLF01_details.message_type, "0800", 4) != 0)
			{
				strcat (Line[bitCount], FieldStrings [bitCount]);
				strncat (Line[bitCount], (char *)auth_tx.ch_billing_conv_rate ,8);
			}
		break;

		case bf_11stan :
			//if(strlen((char *)auth_tx.TLF01_details.sys_trace_audit_num) != 0)
			if(BitFieldArray[bf_11stan] == 1)
			{
				strcat (Line[bitCount], FieldStrings [bitCount]);
				strncat (Line[bitCount], (char *)auth_tx.TLF01_details.sys_trace_audit_num, 6);
			}
		break;
		// For field 12 and 13 move_iso_response_to_auth_struct function not parsed hence using HOST_MSG only
		case bf_12time :
			//if(strlen((char *)HOST_MESSAGE.time_bin) != 0)
			if(BitFieldArray[bf_12time] == 1)
			{
				 bcd_to_asc (HOST_MESSAGE.time_bin, sizeof(HOST_MESSAGE.time_bin),
							asci_cnvrtd_data,6);
				strcat (Line[bitCount], FieldStrings [bitCount]);
				strncat (Line[bitCount],asci_cnvrtd_data, 6);
			}
		break ;

		case bf_13date :
			//if(strlen((char *)HOST_MESSAGE.date_bin) != 0)
			if(BitFieldArray[bf_13date] == 1)
			{
				bcd_to_asc (HOST_MESSAGE.date_bin, sizeof(HOST_MESSAGE.date_expd),
							asci_cnvrtd_data,4);
				strcat (Line[bitCount], FieldStrings [bitCount]);
				strncat (Line[bitCount], asci_cnvrtd_data, 4);
			}
		break ;

		case bf_14expiry :
			/*if(strlen((char *)auth_tx.TLF01_details.exp_date) != 0 &&
			   strncmp((char *)auth_tx.TLF01_details.message_type, "0800", 4) != 0)*/
			 if(BitFieldArray[bf_14expiry] == 1 &&
			   strncmp((char *)auth_tx.TLF01_details.message_type, "0800", 4) != 0)
			{
				strcat (Line[bitCount], FieldStrings [bitCount]);
				strncat (Line[bitCount], (char *)auth_tx.TLF01_details.exp_date, 4);
			}
		break ;
		// For field 15 move_iso_response_to_auth_struct function not parsed hence using HOST_MSG only
		case bf_15datestl :
			//if(strlen((char *)HOST_MESSAGE.date_settle) != 0)
			if(BitFieldArray[bf_15datestl] == 1)
			{
				bcd_to_asc (HOST_MESSAGE.date_settle, sizeof(HOST_MESSAGE.date_expd),
							asci_cnvrtd_data,4);
				strcat (Line[bitCount], FieldStrings [bitCount]);
				strncat (Line[bitCount], asci_cnvrtd_data, 4);
			}
		break ;
		
		case bf_18sicnumber :
			/*if(strlen((char *)auth_tx.TLF01_details.category_code) != 0 &&
			   strncmp((char *)auth_tx.TLF01_details.message_type, "0800", 4) != 0)*/
			   if(BitFieldArray[bf_18sicnumber] == 1 &&
			   strncmp((char *)auth_tx.TLF01_details.message_type, "0800", 4) != 0)
			{
				strcat (Line[bitCount], FieldStrings [bitCount]);
				strncat (Line[bitCount], (char *)auth_tx.TLF01_details.category_code, 4);
			}
		break ;
		
		case bf_19ctrycd :
			/*if(strlen((char *)auth_tx.country_code) != 0 &&
			   strncmp((char *)auth_tx.TLF01_details.message_type, "0800", 4) != 0)*/
			 if(BitFieldArray[bf_19ctrycd] == 1 &&
			   strncmp((char *)auth_tx.TLF01_details.message_type, "0800", 4) != 0)
			{
				strcat (Line[bitCount], FieldStrings [bitCount]);
				strncat (Line[bitCount], (char *)auth_tx.country_code, 4);
			}
		break ;
		// even thourh data not prsnt in host msg the auth_tx.pan_ext_ctry_code hvng 0000 so using host msg only 
		// for the rfrnce
		case bf_20panctrycd :
			//if(strlen((char *)HOST_MESSAGE.pan_ctry_cd) != 0)
			if(BitFieldArray[bf_20panctrycd] == 1)
			{
				strcat (Line[bitCount], FieldStrings [bitCount]);
				strncat (Line[bitCount], (char *)auth_tx.pan_ext_ctry_code, 4);
			}
		break ;

		case bf_22pose :
			/*if(strlen((char *)auth_tx.TLF01_details.pos_entry_mode) != 0 &&
			   strncmp((char *)auth_tx.TLF01_details.message_type, "0800", 4) != 0)*/
			if(BitFieldArray[bf_22pose] == 1 &&
			   strncmp((char *)auth_tx.TLF01_details.message_type, "0800", 4) != 0)
			{
				strcat (Line[bitCount], FieldStrings [bitCount]);
				strncat (Line[bitCount], (char *)auth_tx.TLF01_details.pos_entry_mode, 4);
			}
		break ;
		// For field 23 move_iso_response_to_auth_struct function not parsed hence using HOST_MSG only
		
		case bf_23cardseqnum :
			//for(int i=0;i<sizeof(HOST_MESSAGE.cardseqnum);i++){
			//if(HOST_MESSAGE.cardseqnum[i] != 0x00){
			//bcd_to_asc (HOST_MESSAGE.cardseqnum, sizeof(HOST_MESSAGE.cardseqnum),
							//asci_cnvrtd_data,3);
			
			//if(strlen((char *)auth_tx.host2_message_seq_nbr) != 0 &&
			  // strncmp((char *)auth_tx.TLF01_details.message_type, "0800", 4) != 0)
		    if(BitFieldArray[bf_23cardseqnum] == 1 &&
			   strncmp((char *)auth_tx.TLF01_details.message_type, "0800", 4) != 0)
			{
				strcat (Line[bitCount], FieldStrings [bitCount]);
				strncat (Line[bitCount],(char *)auth_tx.host2_message_seq_nbr+1, 3);
			}			
		break ;
			//}
			//}

		case bf_25pocc :
			//if(strlen((char *)auth_tx.TLF01_details.pos_condition_code) != 0 &&
				//strncmp((char *)auth_tx.TLF01_details.message_type, "0800", 4) != 0)
			if(BitFieldArray[bf_25pocc] == 1 &&
				strncmp((char *)auth_tx.TLF01_details.message_type, "0800", 4) != 0)
			{
				strcat (Line[bitCount], FieldStrings [bitCount]);
				strncat (Line[bitCount], (char *)auth_tx.TLF01_details.pos_condition_code, 2);
			}
		break ;
		
// For field 26 move_iso_response_to_auth_struct function not parsed hence using HOST_MSG only
		case bf_26pincc :
			//if(strlen((char *)HOST_MESSAGE.pin_captcd) != 0)
			if(BitFieldArray[bf_26pincc] == 1)
			{
					bcd_to_asc (HOST_MESSAGE.pin_captcd, sizeof(HOST_MESSAGE.pin_captcd),
							asci_cnvrtd_data,2);
					strcat (Line[bitCount], FieldStrings [bitCount]);
					strncat (Line[bitCount],asci_cnvrtd_data , 2);
			}
		break ;
		// It is parsed and kept in HOST_MESSAGE structure. And move_iso_response_to_auth_struct fun 
		// did not cnvrtd this field
		case bf_28feeamount :
			//if(strlen((char *)HOST_MESSAGE.fee_amount) != 0)
			if(BitFieldArray[bf_28feeamount] == 1)
			{
					strcat (Line[bitCount], FieldStrings [bitCount]);
					strncat (Line[bitCount], (char *)HOST_MESSAGE.fee_amount , 9);
			}
		break ;
		
		case bf_32idcode :
			//if(strlen((char *)auth_tx.acq_inst_id_cd) != 0)
			if(BitFieldArray[bf_32idcode] == 1)
			{
					genutil_bin_to_ascii(HOST_MESSAGE.acq_inst_len,1,temp_len,2); 
					len = genutil_hex_to_int(temp_len,strlen(temp_len));
					strcat (Line[bitCount], FieldStrings [bitCount]);
					strncat (Line[bitCount], (char *)auth_tx.acq_inst_id_cd ,len);
			}
		break ;
		
		case bf_33fwdidcode :
			//if(strlen((char *)auth_tx.forwarding_inst_id_cd ) != 0)
			if(BitFieldArray[bf_33fwdidcode] == 1)
			{
					strcat (Line[bitCount], FieldStrings [bitCount]);
					strncat (Line[bitCount], (char *)auth_tx.forwarding_inst_id_cd ,strlen((char *)auth_tx.forwarding_inst_id_cd ));
			}
		break ;

		case bf_34panext :
			//if (strlen((char *)auth_tx.pan_extended ) > 0)
			if(BitFieldArray[bf_34panext] == 1)
			{
				strcat(Line[bitCount], FieldStrings [bitCount]);
				strncat(Line[bitCount], (char *)auth_tx.pan_extended ,strlen((char *)auth_tx.pan_extended ));
			}
			break;

		case bf_35track2 :
			//if(strlen((char *)auth_tx.TLF01_details.track2) != 0)
			if(BitFieldArray[bf_35track2] == 1)
			{
					strcat (Line[bitCount], FieldStrings [bitCount]);
					strncat (Line[bitCount], (char *)auth_tx.TLF01_details.track2 ,strlen((char *)auth_tx.TLF01_details.track2));
			}
		break ;

		case bf_37rrn :
			//if(strlen((char *)auth_tx.TLF01_details.retrieval_ref_num) != 0)
			if(BitFieldArray[bf_37rrn] == 1)
			{
					strcat (Line[bitCount], FieldStrings [bitCount]);
					strncat (Line[bitCount], (char *)auth_tx.TLF01_details.retrieval_ref_num ,12);
			}
		break ;
		
		case bf_38authcode :
			//if(strlen((char *)auth_tx.TLF01_details.auth_number) != 0)
			if(BitFieldArray[bf_38authcode] == 1)
			{
					strcat (Line[bitCount], FieldStrings [bitCount]);
					strncat (Line[bitCount], (char *)auth_tx.TLF01_details.auth_number ,6);
			}
		break ;
	
		case bf_39respcode :
			//if(strlen((char *)auth_tx.TLF01_details.response_code) != 0) HOST_MESSAGE.responsecd
			//if(strlen((char *)HOST_MESSAGE.responsecd) != 0)
			if(BitFieldArray[bf_39respcode] == 1)

			{
					strcat (Line[bitCount], FieldStrings [bitCount]);
					strncat (Line[bitCount], (char *)auth_tx.TLF01_details.response_code,2);
			}
		break ;
		 // It is parsed and kept in HOST_MESSAGE structure. And move_iso_response_to_auth_struct fun 
		// did not cnvrtd this field
		case bf_40src :
			//if(strlen((char *)HOST_MESSAGE.src) != 0)
			if(BitFieldArray[bf_40src] == 1)
			{
					strcat (Line[bitCount], FieldStrings [bitCount]);
					strncat (Line[bitCount], (char *)HOST_MESSAGE.src,3);
			}
		break ;
		
		case bf_41termid :
		//	if(strlen((char *)auth_tx.TLF01_details.terminal_id) != 0)
			//if(strlen((char *)HOST_MESSAGE.termid) != 0)// spaces were not recongnized by in auth, so used host structure.
			if(BitFieldArray[bf_41termid] == 1)
			{
					strcat (Line[bitCount], FieldStrings [bitCount]);
					strncat (Line[bitCount], (char *)HOST_MESSAGE.termid,8);
					//memcpy(Line[bitCount]+30,(char *)HOST_MESSAGE.termid,8);
			}
		break ;
		
		case bf_42cardacc :
			//if(strlen((char *)auth_tx.TLF01_details.merchant_id) != 0)
			if(BitFieldArray[bf_42cardacc] == 1)
			{
					strcat (Line[bitCount], FieldStrings [bitCount]);
					strncat (Line[bitCount], (char *)auth_tx.TLF01_details.merchant_id,15);
			}
		break ;
		// 43 field stored in 3 diffrnt auth struct.
		case bf_43cardaccname :
			strncpy((char *)temp_data,(char *)auth_tx.acceptor_term_name,25);
			strncat((char *)temp_data,(char *)auth_tx.acceptor_city_name,13);
			strncat((char *)temp_data,(char *)auth_tx.acceptor_country,2);
			//if(strlen((char *)temp_data) != 0)
			if(BitFieldArray[bf_43cardaccname] == 1)
			{
					strcat (Line[bitCount], FieldStrings [bitCount]);
					strncat (Line[bitCount], (char *)temp_data,strlen((char *)temp_data));
			}
		break ;
		// It is  moved as cvv data , need to chk
		case bf_44respdata :
			//if(strlen((char *)HOST_MESSAGE.addl_rsp_len) != 0)
			if(BitFieldArray[bf_44respdata] == 1)
			{
					genutil_bin_to_ascii(HOST_MESSAGE.addl_rsp_len,1,temp_len,3);
					len = genutil_hex_to_int(temp_len,strlen(temp_len));
					strcat (Line[bitCount], FieldStrings [bitCount]);
					strncat (Line[bitCount], (char *)HOST_MESSAGE.addlrspdata,len);
			}
		break ;
		
		case bf_45track1 :
			//if(strlen((char *)HOST_MESSAGE.track1_len) != 0)
			if(BitFieldArray[bf_45track1] == 1)
			{
					genutil_bin_to_ascii(HOST_MESSAGE.track1_len,1,temp_len,3);
					len = genutil_hex_to_int(temp_len,strlen(temp_len));
					strcat (Line[bitCount], FieldStrings [bitCount]);
					strncat (Line[bitCount], (char *)HOST_MESSAGE.track1,len);
			}
		break ;

		case bf_48addl_data :
			//if(strlen((char *)auth_tx.TLF01_details.response_text) != 0)
			if(BitFieldArray[bf_48addl_data] == 1)
			{
				len = strlen((char *)auth_tx.TLF01_details.response_text);
				strcat (Line[bitCount], FieldStrings [bitCount]);
				strncat (Line[bitCount], (char *)auth_tx.TLF01_details.response_text,len);
			}
		break ;
		
			//the crncy code moved to this member auth_tx.TLF01_details.currency_code but for othr things it is cnvrtd to USD
			// bfr cnvrtng old crncy strd in auth_tx.reconciliation_cur_code
		case bf_49trncurcd :
			//if(strlen((char *)HOST_MESSAGE.tran_curr_cd) != 0)	
			if(BitFieldArray[bf_49trncurcd] == 1)
			{
					strcat (Line[bitCount], FieldStrings [bitCount]);
					strncat (Line[bitCount], (char *)auth_tx.TLF01_details.currency_code,3);
			}
		break ;
		
		case bf_51crdcurcd :
			//if(strlen((char *)HOST_MESSAGE.card_curr_cd) != 0)
			if(BitFieldArray[bf_51crdcurcd] == 1)
			{
					strcat (Line[bitCount], FieldStrings [bitCount]);
					strncat (Line[bitCount], (char *)auth_tx.ch_billing_curr_code,3);
			}
		break ;

		case bf_52pinblock :
			//if(strlen((char *)auth_tx.TLF01_details.pin_block) != 0) 
			if(BitFieldArray[bf_52pinblock] == 1)
			{
					strncpy((char *)temp_data,(char *)HOST_MESSAGE.pin_block,8);
					len = strlen((char *)temp_data);
					genutil_bin_to_ascii (temp_data, len,buffer, (len*2));
					strcat (Line[bitCount], FieldStrings [bitCount]);
					strncat (Line[bitCount], (char *)auth_tx.TLF01_details.pin_block,(strlen((char *)auth_tx.TLF01_details.pin_block)));
			}
		break ;
		// it is not moved to auth so cnvrtd here
		case bf_53seccntl :
			//if(strlen((char *)HOST_MESSAGE.sec_cntl) != 0)	
			if(BitFieldArray[bf_53seccntl] == 1)
			{
				bcd_to_asc (HOST_MESSAGE.tran_curr_cd,sizeof(HOST_MESSAGE.tran_curr_cd),
							asci_cnvrtd_data, 16);   
					strcat (Line[bitCount], FieldStrings [bitCount]);
					strncat (Line[bitCount], asci_cnvrtd_data,16);
			}
		break ;
		
		case bf_54addlamts :
			//if(strlen((char *)HOST_MESSAGE.addl_len) != 0)
			if(BitFieldArray[bf_54addlamts] == 1)
			{
					strncpy(temp_len,(char *)HOST_MESSAGE.addl_len,1);
					len=HOST_MESSAGE.addl_len[0];
					strcat (Line[bitCount], FieldStrings [bitCount]);
					strncat (Line[bitCount], (char *)HOST_MESSAGE.addlamts,len);
			}
		break ;
		// from auth we moved filed 55(EMV data) to iccdata and used it as reference
		case bf_55ICCdata :
			memcpy(&EMV_details,&(auth_tx.EMV_details),sizeof(EMV_details));
			Generate_CUP_EMV_Details();
			//if(strlen((char *)iccdata) != 0)
			if(BitFieldArray[bf_55ICCdata] == 1)
			{
					strcat (Line[bitCount], FieldStrings [bitCount]);
					strncat (Line[bitCount], (char *)iccdata,strlen((char *)iccdata));
			}
		break ;
		
		case bf_56pad :
			//if(strlen((char *)HOST_MESSAGE.trandesc_len) > 0)	
			if(BitFieldArray[bf_56pad] == 1)
			{
					/*genutil_bin_to_ascii (HOST_MESSAGE.trandesc_len, 1,temp_len, 3);
					len = genutil_hex_to_int(temp_len,strlen(temp_len));*/
					len = HOST_MESSAGE.PAD_len[0];
					strcat (Line[bitCount], FieldStrings [bitCount]);
					strncat (Line[bitCount], (char *)auth_tx.future_use_3, strlen((char *)auth_tx.future_use_3));
			}
		break ;
		
		case bf_59posdata :
			//if(strlen((char *)HOST_MESSAGE.posdata_len) != 0)	
            if(BitFieldArray[bf_59posdata] == 1)
			{
					genutil_bin_to_ascii(HOST_MESSAGE.posdata_len,1,temp_len,3);
					len = genutil_hex_to_int(temp_len,strlen(temp_len));
					strcat (Line[bitCount], FieldStrings [bitCount]);
					strncat (Line[bitCount], (char *)HOST_MESSAGE.posdata,len);
			}
		break ;

		
		case bf_60priv60 :
			//if(strlen((char *)auth_tx.TLF01_details.nfi_name) != 0)
			if(BitFieldArray[bf_60priv60] == 1)
			{
					strcat (Line[bitCount], FieldStrings [bitCount]);
					strncat (Line[bitCount], (char *)auth_tx.TLF01_details.nfi_name,strlen((char *)auth_tx.TLF01_details.nfi_name));
			}
		break ;
		// fld 61 , 62 and 63 is copied to ddfrnt members of auth so concatnte it into bufer and use it for prntng
		case bf_61priv61 :
			//if(strlen((char *)auth_tx.TLF01_details.add_amounts) != 0)
			if(BitFieldArray[bf_61priv61] == 1)
			{
					strcat (Line[bitCount], FieldStrings [bitCount]);
					strncat (Line[bitCount], (char *)auth_tx.TLF01_details.add_amounts,strlen((char *)auth_tx.TLF01_details.add_amounts));
			}
		break ;
		
		case bf_62priv62 :
			 //if (HOST_MESSAGE.priv62_len[0] > 0) 
			 if(BitFieldArray[bf_62priv62] == 1)
			 {
				buffer[0]=auth_tx.TLF01_details.visa_char_indicator;	
				strncat(buffer,(char*)auth_tx.TLF01_details.visa_tran_id,16);
				strncat(buffer,(char*)auth_tx.TLF01_details.visa_validation_code,5);
				strncat(buffer,(char*)auth_tx.TLF01_details.product_codes[15].quantity,1);
				strncat(buffer,(char*)auth_tx.mac_key,2);
				strncat(buffer, (char *)auth_tx.TLF01_details.product_codes[8].amount, 10);

				if(strlen(buffer) != 0)			
				{	
						strcat (Line[bitCount], FieldStrings [bitCount]);
						strncat (Line[bitCount], buffer,strlen(buffer));
				}
					 //strcat (Line[bitCount], FieldStrings [bitCount]);
						//strncat (Line[bitCount], (char*)auth_tx.int_net_facility_data,strlen((char*)auth_tx.int_net_facility_data));

			 }
		break ;
		
		case bf_63priv63 :
			strncpy(buffer,(char *)auth_tx.TLF01_details.product_codes[10].code,4);
			strncat (buffer, (char *)auth_tx.TLF01_details.product_codes[11].code,4);
			strncat (buffer, (char *)auth_tx.TLF01_details.product_codes[12].code,4);
			//if(strlen(buffer) != 0)	
			if(BitFieldArray[bf_63priv63] == 1)
			{
					strcat (Line[bitCount], FieldStrings [bitCount]);
					strncat (Line[bitCount], buffer,strlen(buffer));
			}
		break ;			
	// eventhoug field is not prsnt the value was prntng so using hos struct only HOST_MESSAGE.nmi_code
		case bf_70nmicode :
			//if(strlen((char *)auth_tx.function_code) != 0 &&
				//strncmp((char *)auth_tx.TLF01_details.message_type, "0800", 4) == 0)
			if( BitFieldArray[bf_70nmicode] == 1 &&
				strncmp((char *)auth_tx.TLF01_details.message_type, "0800", 4) == 0)
			{
					strcat (Line[bitCount], FieldStrings [bitCount]);
					strncat (Line[bitCount], (char *)auth_tx.function_code,4);
			}
		break ;

		case bf_73action :
			//if(strlen((char *)auth_tx.TLF01_details.product_codes[0].amount) != 0)
			if(BitFieldArray[bf_73action] == 1)
			{
					strcat (Line[bitCount], FieldStrings [bitCount]);
					strncat (Line[bitCount], (char *)auth_tx.TLF01_details.product_codes[0].amount,6);
			}
		break ;
		// same as 61 ,62 & 63
		case bf_90orgdata :
			strncat((char *)temp_data,(char *)auth_tx.TLF01_details.orig_message,4);
			strncat((char *)temp_data,(char *)auth_tx.TLF01_details.orig_sys_trace_num,6);
			strncat((char *)temp_data,(char *)auth_tx.orig_local_time_date,10);
			strncat((char *)temp_data,(char *)auth_tx.orig_acq_inst_id,11);
			strncat((char *)temp_data,(char *)auth_tx.amex_pos_entry_mode,11);
			//if(strlen((char *)temp_data) != 0)	
			if(BitFieldArray[bf_90orgdata] == 1)
			{
					strcat (Line[bitCount], FieldStrings [bitCount]);
					strncat (Line[bitCount], (char *)temp_data,42);
			}
		break ;
		//HOST_MESSAGE.file_update
		case bf_91fileupdate  :
			//if(strlen((char *)HOST_MESSAGE.file_update)!= 0)	
			if(BitFieldArray[bf_91fileupdate] == 1)
			{
					
					strcat (Line[bitCount], FieldStrings [bitCount]);
					strncat (Line[bitCount],(char *)HOST_MESSAGE.file_update ,1);
			}
		break ;
		// Not moved to auth so used teh HOST_MESSAGE struct.
		case bf_93rspind :
			//if(strlen((char *)HOST_MESSAGE.resp_ind) != 0)
			if(BitFieldArray[bf_93rspind] == 1)
			{
					strcat (Line[bitCount], FieldStrings [bitCount]);
					strncat (Line[bitCount], (char *)HOST_MESSAGE.resp_ind,5);
			}
		break ;
		//  stored in  auth_tx->actual_amount but it vl b cnvrtd ltr so copied into 
		// auth_tx.orig_amount for printg HOST_MESSAGE.repl_amts
		case bf_95replamts :
			//if(strlen((char *)auth_tx.TLF01_details.actual_amount) != 0)		
			memcpy(temp_data,(char *)HOST_MESSAGE.repl_amts,42);
			//if(strlen((char *)temp_data) != 0)	
			if(BitFieldArray[bf_95replamts] == 1)
			{
					strcat (Line[bitCount], FieldStrings [bitCount]);
				//	strncat (Line[bitCount], (char *)auth_tx.TLF01_details.actual_amount,strlen((char *)auth_tx.TLF01_details.actual_amount));
					//strncat (Line[bitCount], (char *)temp_data,strlen((char *)temp_data));
					strncat(Line[bitCount], (char *)temp_data,12);
			}
		break ;
		// not moved to auth so used HOST_MESSAGE
		case bf_96msgsec :
			//if(strlen((char *)HOST_MESSAGE.msgsec) != 0)
			if(BitFieldArray[bf_96msgsec] == 1)
			{
					strncpy((char *)temp_data,(char *)HOST_MESSAGE.msgsec,8);
					len = strlen((char *)temp_data);
					genutil_bin_to_ascii (temp_data, len,buffer, (len*2));
					strcat (Line[bitCount], FieldStrings [bitCount]);
					strncat (Line[bitCount], (char *)buffer,strlen(buffer));
			}
		break ;
		// not moved to auth so used HOST_MESSAGE
		case bf_100rcvid :
			//if(strlen((char *)HOST_MESSAGE.recv_inst_len) != 0)
			if(BitFieldArray[bf_100rcvid] == 1)
			{
					genutil_bin_to_ascii (HOST_MESSAGE.recv_inst_len, 1,temp_len, 3);
					len = genutil_hex_to_int(temp_len,strlen(temp_len));
					bcd_to_asc( HOST_MESSAGE.recv_inst_id, len,(char *)temp_data,strlen((char *)temp_data));
					strcat (Line[bitCount], FieldStrings [bitCount]);
					strncat (Line[bitCount], (char *)temp_data,len);
			}
		break ;

		case bf_101fname :
			//if(strlen((char *)auth_tx.TLF01_details.kek) != 0)	
			if(BitFieldArray[bf_101fname] == 1)
			{
					strcat (Line[bitCount], FieldStrings [bitCount]);
					strncat (Line[bitCount], (char *)auth_tx.TLF01_details.kek,strlen((char *)auth_tx.TLF01_details.kek));
			}
		break ;
		
		case bf_102acct1 :
			//if(strlen((char *)auth_tx.acct_id_1) != 0)	
			if(BitFieldArray[bf_102acct1] == 1)
			{
					strcat (Line[bitCount], FieldStrings [bitCount]);
					strncat (Line[bitCount], (char *)auth_tx.acct_id_1,strlen((char *)auth_tx.acct_id_1));
			}
		break ;

		case bf_103acct2 :
			//if(strlen((char *)auth_tx.acct_id_2) != 0)	
			if(BitFieldArray[bf_103acct2] == 1)
			{
					strcat (Line[bitCount], FieldStrings [bitCount]);
					strncat (Line[bitCount], (char *)auth_tx.acct_id_2,strlen((char *)auth_tx.acct_id_2));
			}
		break ;
		// not moved to auth so using HOST_MESSAGE
		case bf_104trandesc :
			//if(strlen((char *)HOST_MESSAGE.trandesc_len) > 0)	
			if(BitFieldArray[bf_104trandesc] == 1)
			{
					/*genutil_bin_to_ascii (HOST_MESSAGE.trandesc_len, 1,temp_len, 3);
					len = genutil_hex_to_int(temp_len,strlen(temp_len));*/
					len = HOST_MESSAGE.trandesc_len[0];
					strcat (Line[bitCount], FieldStrings [bitCount]);
					strncat (Line[bitCount], (char *)auth_tx.future_use_1,strlen((char *)auth_tx.future_use_1));
			}
		break ;

		/* F111 */
		case bf_111addl_tran :
			//if (strlen((char *)auth_tx.future_use_2) > 0)
			if(BitFieldArray[bf_111addl_tran] == 1)
			{
				strcat (Line[bitCount], FieldStrings [bitCount]);
				strncat(Line[bitCount], (char *)auth_tx.future_use_2, strlen((char *)auth_tx.future_use_2));
			}

		case bf_116issref :
			//if(strlen((char *)auth_tx.TLF01_details.dest_key) != 0)	
			if(BitFieldArray[bf_116issref] == 1)
			{
					genutil_bin_to_ascii (HOST_MESSAGE.issref_len, 1,temp_len, 3);
					len =atoi(temp_len);
					strcat (Line[bitCount], FieldStrings [bitCount]);
					strncat (Line[bitCount], (char *)auth_tx.TLF01_details.dest_key,strlen((char *)auth_tx.TLF01_details.dest_key));
			}
		break ;
		// not moved to auth hence usnig host struct
		case bf_118intractry :
			//if(strlen((char *)HOST_MESSAGE.intra_ctry_len) != 0)	
			if(BitFieldArray[bf_118intractry] == 1)
			{
					genutil_bin_to_ascii (HOST_MESSAGE.intra_ctry_len, 1,temp_len, 3);
					len = genutil_hex_to_int(temp_len,strlen(temp_len));
					strcat (Line[bitCount], FieldStrings [bitCount]);
					strncat (Line[bitCount], (char *)HOST_MESSAGE.intra_ctry,len);
			}
		break ;
		// not moved to auth hence usnig host struct
		case bf_120orgmsgid :
			//if(strlen((char *)HOST_MESSAGE.org_msg_id) != 0)	
			if(BitFieldArray[bf_120orgmsgid] == 1)
			{
					strcat (Line[bitCount], FieldStrings [bitCount]);
					strncat (Line[bitCount], (char *)HOST_MESSAGE.org_msg_id,3);
			}
		break ;
		// not moved to auth hence usnig host struct
		case bf_121issid :
			//if(strlen((char *)auth_tx.iss_inst_id_cd) != 0)	
			if(BitFieldArray[bf_121issid] == 1)
			{
					strcat (Line[bitCount], FieldStrings [bitCount]);
					strncat (Line[bitCount], (char *)auth_tx.iss_inst_id_cd,strlen((char *)auth_tx.iss_inst_id_cd));
			}
		break ;
		// not moved to auth hence usnig host struct
		case bf_122openuse :
			//if(strlen((char *)HOST_MESSAGE.open_use_len) != 0)	
			if(BitFieldArray[bf_122openuse] == 1)
			{
					genutil_bin_to_ascii (HOST_MESSAGE.open_use_len, 1,temp_len, 3);
					len = genutil_hex_to_int(temp_len,strlen(temp_len));
					strcat (Line[bitCount], FieldStrings [bitCount]);
					strncat (Line[bitCount], (char *)HOST_MESSAGE.open_use,len);
			}
		break ;
		// not moved to auth hence usnig host struct
		case bf_123addverf :
			//if(strlen((char *)HOST_MESSAGE.addr_verf_len) != 0)	
			if(BitFieldArray[bf_123addverf] == 1)
			{
					genutil_bin_to_ascii (HOST_MESSAGE.addr_verf_len, 1,temp_len, 3);
					len = genutil_hex_to_int(temp_len,strlen(temp_len));
					strcat (Line[bitCount], FieldStrings [bitCount]);
					strncat (Line[bitCount], (char *)HOST_MESSAGE.addr_verf,len);
			}
		break ;
		// not moved to auth hence usnig host struct
		case bf_124freeform :
			//if(strlen((char *)HOST_MESSAGE.free_text_len) != 0)	
			if(BitFieldArray[bf_124freeform] == 1)
			{
					genutil_bin_to_ascii (HOST_MESSAGE.free_text_len, 1,temp_len, 3);
					len = genutil_hex_to_int(temp_len,strlen(temp_len));
					strcat (Line[bitCount], FieldStrings [bitCount]);
					strncat (Line[bitCount], (char *)HOST_MESSAGE.free_text,len);
			}
		break ;
		// for both 125 and 126 they r using the auth_tx.TLF01_details.voice_auth_text member so for 125 use Host msg
		case bf_125supinfo :
			//if(strlen((char *)HOST_MESSAGE.sup_info_len) != 0)	
			if(BitFieldArray[bf_125supinfo] == 1)
			{
					genutil_bin_to_ascii (HOST_MESSAGE.sup_info_len, 1,temp_len, 3);
					len = genutil_hex_to_int(temp_len,strlen(temp_len));
					strcat (Line[bitCount], FieldStrings [bitCount]);
					bcd_to_asc( HOST_MESSAGE.sup_info, len,(char *)temp_data,strlen((char *)temp_data));
					strncat (Line[bitCount], (char*)temp_data,len);
			}
		break ;

		case bf_126priv126 :
			//if(strlen((char *)auth_tx.TLF01_details.voice_auth_text) != 0)	
			if(BitFieldArray[bf_126priv126] == 1)
			{
					strcat (Line[bitCount], FieldStrings [bitCount]);
					strncat (Line[bitCount], (char *)auth_tx.TLF01_details.voice_auth_text,strlen((char *)auth_tx.TLF01_details.voice_auth_text));
			}
		break ;

		case bf_127filerecs :
			//if(strlen((char *)auth_tx.TLF01_details.product_codes[1].amount) != 0)
			if(BitFieldArray[bf_127filerecs] == 1)
			{
					/*genutil_bin_to_ascii (HOST_MESSAGE.file_rec_len, 1,temp_len, 3);
					len = genutil_hex_to_int(temp_len,strlen(temp_len));
					for(int i=0; i<len; i++ )
					{
						 
						buffer[i] = ascii_ebcdic_conv( HOST_MESSAGE.file_rec[i], E2A );
					}*/
					strcat (Line[bitCount], FieldStrings [bitCount]);
					strncat (Line[bitCount], (char *)auth_tx.TLF01_details.type_of_data, 2);
					strncat (Line[bitCount], (char *)auth_tx.TLF01_details.product_codes[1].amount,strlen((char *)auth_tx.TLF01_details.product_codes[1].amount));
			}
		break ;

	}

		
		startCharacter++;
		LastDisplayBit = bitCount;
	}

		if(strlen((char *)HOST_MESSAGE.ICCdata) != 0)
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
	UpdateData(); //transfers the data from controls to variables.
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
		len = m_flowfile.GetLength();
		temp_string = (LPBYTE)m_flowfile.GetBuffer(len);
		ascii_to_bin(temp_string, ReqRsp_Buffer, len);
		m_flowfile.ReleaseBuffer(len);

		if ( hhutil_parse_host_response_msg(&auth_tx) == 0)
		{
			m_parsedmssg = "Cannot Parse the message";
			UpdateData(FALSE); //transfer the data from variables back to control.
		}
		else
		{
			memset(&auth_tx,0x00,sizeof(auth_tx));
			move_iso_response_to_auth_struct(nmi_code, &auth_tx);
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
  // memcpy( p_auth_tx->TLF01_details.currency_code, p_acct_bal->cur_code, 3 );

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
   INT     i   = 0;
   INT     cps_bit = 0;
   INT     cps_byte = 0 ;
   INT     Len = 0;
   INT     TotalLen = 0 ;
   INT     BitMap_Len = 8;
   BYTE    BitMap[8] = {0};
   BYTE    TempStr[51] ={0};
   CHAR    temp_txn_id[17] = {0};
   CHAR	   temp_mvv[11] = {0};


   /* Get length of field. */
   TotalLen = (INT)HOST_MESSAGE.priv62_len[0];

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
						 case 4:  
                                /* CPS Authorization */
                                p_auth_tx->TLF01_details.product_codes[15].quantity[0] =
                                   ascii_ebcdic_conv( TempStr[Len], E2A );

                                Len++;
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
						 case 12: Len += 1;
							    memset( temp_txn_id, 0x00, sizeof(temp_txn_id));
                                bcd_to_asc( &TempStr[Len], 1, temp_txn_id, 2 );
                                strcpy( (char *)p_auth_tx->mac_key,
                                       &temp_txn_id[0] );
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
									break;
						 case 20:
									memset(temp_mvv, 0x00, sizeof(temp_mvv));
									genutil_bin_to_ascii( &TempStr[Len], 5, temp_mvv, 10 );
									strcpy((char *)p_auth_tx->TLF01_details.product_codes[8].amount,temp_mvv );
									Len += 5;
									break;

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
   INT     TotalLen=0;
   BYTE    BitMap[F63_BITMAP_LEN];
   BYTE    TempStr[100]={0};
   BYTE    buffer[6] = {0};
   int negativ_len_check=0;

   /* Get length of field. */
   TotalLen = f63[0];

   printf("%d\n",TotalLen);

   /* Copy bitmap into local variable. */
 
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
                      strncpy( (char*)p_auth_tx->TLF01_details.product_codes[10].code,
                             (char*) buffer,strlen((char*)buffer) );
                      Len+= 2;
                      break;
               case 3:
                      /* Reversal Message Reason Code */
                      memset( buffer, 0x00, sizeof(buffer));
                      bcd_to_asc( &TempStr[Len], 2, (char*)buffer, 4 );
                      strncpy((char*)p_auth_tx->TLF01_details.product_codes[11].code,
                             (char*)buffer ,strlen((char*)buffer));
                      Len += 2;
                      break;
               case 4:
                      /* STIP/Switch Reason Code */
                      memset( buffer, 0x00, sizeof(buffer));
                      bcd_to_asc( &TempStr[Len], 2,(char*) buffer, 4 );
                      strncpy((char*)p_auth_tx->TLF01_details.product_codes[12].code,
                             (char*)buffer,strlen((char*)buffer) );
                      Len += 2;
                      break;
               default:
                      Len++;
                      break;
            }
         }  /* End IF bit is set */
      }  /* End LOOP on BIT */
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
   BYTE	   outstring[255]="";	


   /* Get length of field. */
   TotalLen = (INT)HOST_MESSAGE.de126_len[0];
   memcpy( inbuf, HOST_MESSAGE.de126, TotalLen );

   /* Copy bitmap into local variable. */
   memcpy( bcd_BitMap, HOST_MESSAGE.de126, BCD_BITMAP_LEN );

   /* Convert bitmap to ASCII. */
   bcd_to_asc( bcd_BitMap, BCD_BITMAP_LEN, (char *) asc_BitMap, ASC_BITMAP_LEN );
   memcpy( outstring, asc_BitMap, ASC_BITMAP_LEN );

   /* Parse the bitmap for the first 2 bytes only - others are unused. */
   for( cps_byte=1; cps_byte<=3; cps_byte++ )
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

                                strcpy(  (char *)cvv2_data, (char *) tempstr );
								strncat(  (char *)outstring+outptr,  (char *)tempstr, 6 );
								inptr  += 6;
                                outptr += 6;
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
                                strncat(  (char *)outstring+outptr,  (char *)&inbuf[inptr], 3 );
								inptr  += 3; 
                                outptr += 3;
                                break;

						 case 13:
                                /* Service Development Indicator */
								outstring[outptr] = ascii_ebcdic_conv( inbuf[inptr], E2A );
                                break;
								inptr  += 1; 
                                outptr += 1;
								break;
                      }
					  break;
				case 3: /* Byte 3 */
					switch( cps_bit+16 )
					{
						 case  18:
						 	 memset(tempstr, 0x00, sizeof(tempstr) );
							 strncat( (char *)(outstring+outptr), (char *)&inbuf[inptr],12 );
                                // Increment pointers as we are parsing for other fields now.
                                inptr  += 12; 
                                outptr += 12; 
						 	break; 
                         case  17:
                         case  19:
                        	 break;
                         case  20:
                        	 memset(tempstr, 0x00, sizeof(tempstr) );
                        	 tempstr[0] = ascii_ebcdic_conv( inbuf[inptr], E2A );
                        	 strncat( (char *)(outstring+outptr), ( char *)tempstr,1);
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
		strncat((char*)iccdata,(char*)EMV_details.issuer_auth_data,31);
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
void genutil_format_date(pCHAR temp)
{
   struct timeb   time_structure;   /* Time for ftime */
   time_t long_time_since_1970;        /* Number of seconds since 01/01/1970 A.D. 12:00 A.M.*/
   struct tm *current_time;            /* Current time structure*/
   CHAR   temp1[4] = "0";

   ftime(&time_structure);
   long_time_since_1970 = time_structure.time;
   current_time = localtime(&long_time_since_1970);
   current_time->tm_mon++;
   if (current_time->tm_year >= 70)
      current_time->tm_year += 1900;
   else
      current_time->tm_year += 2000;
   itoa(current_time->tm_year, temp, 10);
   itoa(current_time->tm_mon, temp1 + 1, 10);
   if(strlen(temp1) == 2)
      memcpy(temp + 4, temp1, 2);
   else
      memcpy(temp + 4, temp1 + 1, 2);
   memset(temp1, 0, sizeof(temp1));
   temp1[0] = '0';
   itoa(current_time->tm_mday, temp1 + 1, 10);
   if(strlen(temp1) == 2)
      memcpy(temp + 6, temp1, 2);
   else
      memcpy(temp + 6, temp1 + 1, 2);
   memset(temp1, 0, sizeof(temp1));
   temp1[0] = '0';
   itoa(current_time->tm_hour, temp1 + 1, 10);
   if(strlen(temp1) == 2)
      memcpy(temp + 8, temp1, 2);
   else
      memcpy(temp + 8, temp1 + 1, 2);
   memset(temp1, 0, sizeof(temp1));
   temp1[0] = '0';
   itoa(current_time->tm_min, temp1 + 1, 10);
   if(strlen(temp1) == 2)
      memcpy(temp + 10, temp1, 2);
   else
      memcpy(temp + 10, temp1 + 1, 2);
   memset(temp1, 0, sizeof(temp1));
   temp1[0] = '0';
   itoa(current_time->tm_sec, temp1 + 1, 10);
   if(strlen(temp1) == 2)
      memcpy(temp + 12, temp1, 2);
   else
      memcpy(temp + 12, temp1 + 1, 2);

}

/*****************************************************************************

  Function:    hhutil_parse_host_response_msg

  Description: This function will parse a host response message
  Author: 
      unknown
  Inputs:
      p_auth_tx - a pointer to the auth_tx structure
      ReqRsp_Buffer (global) - the buffer that contains the ISO formatted msg
  Outputs:
      HOST_MESSAGE (global)  - structure that contains data to go into the 
                               ISO message
  Return values:
      true  - no error occurred
      false - an error occurred
******************************************************************************/
INT hhutil_parse_host_response_msg(pAUTH_TX p_auth_tx)
{
   struct bitfield_data *pBF;
   INT                  hdr_size;
   INT                  i;
   INT                  ret_val = true;
   INT                  bfidx;    /* index to bitfields for this transaction */
   CHAR				      ErrorMsg[100];
   CHAR                 tid[9];
   CHAR                 mid[16];
   CHAR                 card_num[20];
   CHAR                 tempstr[50];
   CHAR					Dest_Stat_Id[7]={0};


   memset( &HOST_MESSAGE, 0x00, sizeof(HOST_MESSAGE) );
   memset( &Dest_Stat_Id, 0x00, sizeof(Dest_Stat_Id) );

   if (0x1A == ReqRsp_Buffer[0])
   {
      Txn_Is_A_Reject = true;
      hdr_size = VISA_HDR_SIZE + REJECT_VISA_HDR_SIZE;

      /* Get the Reject Code */
      memcpy( HOST_MESSAGE.msgtype_bcd,
             &ReqRsp_Buffer[REJECT_VISA_HDR_SIZE - 2],
             sizeof(HOST_MESSAGE.msgtype_bcd));

      bcd_to_asc( HOST_MESSAGE.msgtype_bcd,
                  2,
                  (char *)p_auth_tx->TLF01_details.message_type,
                  4);

      strcpy( ErrorMsg, "Received header reject code: " );
      strcat( ErrorMsg, (char *) p_auth_tx->TLF01_details.message_type );

      ret_val = true;
   }
   else if (0x16 == ReqRsp_Buffer[0])
   {
      Txn_Is_A_Reject = false;
      hdr_size = VISA_HDR_SIZE;

      /* Parse the Destination Station Id */
      memcpy( HOST_MESSAGE.dest_stat_id,
             &ReqRsp_Buffer[5],
              sizeof(HOST_MESSAGE.dest_stat_id) );

      /* Store the destination station id */
      bcd_to_asc( HOST_MESSAGE.dest_stat_id,
                  sizeof(HOST_MESSAGE.dest_stat_id),
                   (char *)p_auth_tx->incoming_dest_station_id,
                  sizeof(p_auth_tx->incoming_dest_station_id) );

      /* Parse the Source Station Id */
      memcpy( HOST_MESSAGE.src_stat_id,
             &ReqRsp_Buffer[8],
              sizeof(HOST_MESSAGE.src_stat_id) );

      /* Store the source station id */
      bcd_to_asc( HOST_MESSAGE.src_stat_id,
                  sizeof(HOST_MESSAGE.src_stat_id),
                   (char *)p_auth_tx->incoming_src_station_id,
                  sizeof(p_auth_tx->incoming_src_station_id) );

      /* Store the Header so we can echo any fields we need to.
       * This is for switched in txns.  Set the first byte to
       * 'I' to indicate a switched in txn.  If this txn is a
       * response to a switched out txn, this field does not
       * get used.
       */
      HOST_MESSAGE.header[0] = 0x02;
      memcpy( HOST_MESSAGE.header+1, ReqRsp_Buffer, 22 );
   }
   else if ((ReqRsp_Buffer[0] == 0x03) && (ReqRsp_Buffer[1] == 0x02) )
   {
      /* 0302 Request from a device controller. No Visa header. */
      Txn_Is_A_Reject = false;
      hdr_size = 0;
   }
   else if ((ReqRsp_Buffer[0] == 0x03) && (ReqRsp_Buffer[1] == 0x12) )	// ThoughtFocus nirbhay: Cadencie link changes
   {
	/* Response for 0302 (0302 + 10 = 0312) msg from VISA. Forward without parsing to Cadencie */
	Txn_Is_A_Reject = false;
	hdr_size = 0;
   }
   
   else
   {
      Txn_Is_A_Reject = false;
      //TxUtils_Send_Msg_To_Operator(1,
        //       "Invalid message format received. Unexpected Visa Header length",
          //      1, WARN_MSG, "hhutil_parse_host_response_msg", 2,
            //    WARNING_ERROR, NULL_PTR, NULL_PTR, NULL_PTR);
      ret_val = false;
   }

   if ( ret_val == true )
   {
      /* Get the Message Type */
      memcpy( HOST_MESSAGE.msgtype_bcd,
             &ReqRsp_Buffer[hdr_size],
              sizeof(HOST_MESSAGE.msgtype_bcd) );


// route 302 msg directly to ncvisa : ThoughtFocus nirbhay: cadencie changes.
	if(HOST_MESSAGE.msgtype_bcd[0] == 0x03 && HOST_MESSAGE.msgtype_bcd[1] == 0x02)
	{
		//return( false );
	}

			
	   
			
      /* Point past bitmap, but 1st check for extended bitmap. */
      bitmapidx = 2 + hdr_size;
      if ( ReqRsp_Buffer[bitmapidx]  &  BITTAB[bf_1xbitfield].bit_mask )
      {
         if ( ReqRsp_Buffer[bitmapidx+8]  &  BITTAB[bf_2xbitfield].bit_mask )
         {
            /* 3 bitmaps */
			 
            reqidx = 26 + hdr_size;
            BitFieldSize = THIRD_BITFIELD;
			genutil_bin_to_ascii ((BYTE *)&ReqRsp_Buffer[hdr_size+2], THIRD_BITMAP_SIZE, astr_bitmap, sizeof(astr_bitmap));
         }
         else
         {
            /* 2 bitmaps */
			 
            reqidx = 18 + hdr_size;
            BitFieldSize = EXTENDED_BITFIELD;
			genutil_bin_to_ascii ((BYTE *)&ReqRsp_Buffer[hdr_size+2], EXTENDED_BITMAP_SIZE, astr_bitmap, sizeof(astr_bitmap));
         }
      }
      else
      {
         /* 1 bitmaps */
         reqidx = 10 + hdr_size;
		 
         BitFieldSize = NORMAL_BITFIELD;
		 genutil_bin_to_ascii ((BYTE *)&ReqRsp_Buffer[hdr_size+2], NORMAL_BITMAP_SIZE, astr_bitmap, sizeof(astr_bitmap));
      }

      /*------------------------------------------------------------------
         For every bitfield present in message ( except first bit ) . . .
      -------------------------------------------------------------------*/
      for (bfidx = 2; bfidx <= BitFieldSize; bfidx++)
      {
         if (ReqRsp_Buffer[bitmapidx+BITTAB[bfidx].byte_no] & BITTAB[bfidx].bit_mask)
         {
            /* Don't get bitfield for 3rd Bitmap Indicator. */
            if ( bfidx != 65 )
            {
               /* Bit is on, field is there.... */
               pBF = Find_Bitfield_Table((bitfields)bfidx);       

               /*---------------------------------------*/
               /* call the Response Handling routine... */
               /*---------------------------------------*/
	
               if (pBF != NULL)
               {
				   BitFieldArray[pBF->bd_fieldid] = 1;

                  (pBF->bd_BFRsp)(pBF->bd_fieldid);
                  if (true == invalid_msg_format)
                  {
                    // TxUtils_Send_Msg_To_Operator (1, 
                             //  "Unable to parse transaction from Visa Network.",
                              //  1, WARN_MSG, "hhutil_parse_host_response_msg",
                              //  2, WARNING_ERROR, NULL_PTR, NULL_PTR, NULL_PTR);
                     ret_val = false;
                     break;
                  }
               }
               else
               {
                  ret_val = false;
                  break;
               }
            }
         }
      }

      if ( Txn_Is_A_Reject == true )
      {
         /* Terminal ID, Merchant ID, and Card Number and log reject message. */
         memset( tid,      0x00, sizeof(tid)      );
         memset( mid,      0x00, sizeof(mid)      );
         memset( card_num, 0x00, sizeof(card_num) );

         memcpy( tid, HOST_MESSAGE.termid,    sizeof(tid)-1 );
         memcpy( mid, HOST_MESSAGE.cardaccid, sizeof(mid)-1 );

         /* Card Number is a little more tricky to get.
          * It is variable length, and might have a leading zero.
          */
         memset( tempstr, 0x00, sizeof(tempstr) );
         bcd_to_asc( HOST_MESSAGE.pan_bin,
                    (HOST_MESSAGE.pan_len[0]+1)/2,
                     tempstr,
                     sizeof(p_auth_tx->TLF01_details.card_num) );

         i = HOST_MESSAGE.pan_len[0] % 2;
         memcpy( card_num, tempstr+i, sizeof(card_num)-1 );

         /* All this work to log a useful message.  Here it is. 
         TxUtils_Send_Msg_To_Operator (1, ErrorMsg, 1, WARN_MSG, 
                                      "hhutil_parse_host_response_msg", 2,
                                       WARNING_ERROR, tid, card_num, mid );*/
      }
   }
   return( ret_val );
} /* hhutil_parse_host_response_msg */


/*****************************************************************************

  Function:    move_iso_response_to_auth_struct

  Description: This function will convert ISO8583 Fields from their binary fields
               to their ascii fields... 
  Author:
      unknown
  Inputs:
      nmi_code  - 
      p_auth_tx - a pointer to the auth_tx structure
  Outputs:
      None
  Return values:
      true  - no error occurred
      false - an error occurred
  Modified by:
******************************************************************************/
INT move_iso_response_to_auth_struct(CHAR nmi_code[5], pAUTH_TX p_auth_tx)
{
   #define  CVV2_PRESENCE  0
   #define  CVV2_RESPONSE  1
   #define  CVV2_VALUE     2

   INT   retval = true;
   CHAR  tempstr [512];  
   CHAR  temp_str [512];
   pCHAR tempptr;    
   CHAR  templen [6];	 
   INT   i;
   INT   actual_int;
   INT   total_int;
   INT   reversal_int;
   CHAR  error_msg[100]="";
   CHAR  temp[100]; 
   CHAR  Buffer[200];
   BYTE  cavv_result[2];
   BYTE  carc_result[2];
   BYTE  cvv2_result[2];
   BYTE  cvv2_data[7];
   Addl_Amts  acct_bal[4];
   INT        bal_set;
   INT        len;
   INT        emv_len;
   INT        addl_amts_size;
   INT        emv_txn = false;
   INT		  len_f34 = 0;
   INT		  f34	  = 0;
   INT		  len_f56  = 0;
   INT		  len_f104 = 0;
   INT		  f56	   = 0;
   INT		  f104	   = 0;
   INT		  len_f111 = 0;
   INT		  f111	   = 0;
   char vLogon_NMI_Code[4];
      CHAR 	  time_date[25];
   /* Visa Electron Service Code from Track2 */
#define  VISA_ELECTRON_SERVICE_CODE   "126"
#define  SERVICE_CODE_SIZE_PLUS_1        4
   

   memset(vLogon_NMI_Code, 0x00, 4);

   /* VISA HEADER */

   /* Store in Merchant Contact field for switched in txns.
    * If txn is switched out, this field will not get copied
    * to the auth_tx from the timer, so it has no effect.
    */
   memcpy( p_auth_tx->MCF01_details.contact, HOST_MESSAGE.header, 23 );

   /* MESSAGE TYPE */
   bcd_to_asc (HOST_MESSAGE.msgtype_bcd, sizeof(HOST_MESSAGE.msgtype_bcd), 
               (char*)p_auth_tx->TLF01_details.message_type,
               sizeof(p_auth_tx->TLF01_details.message_type));

   /* FIELD 2: PAN */
   if(BitFieldArray[bf_2pan] == 1){
	   memset(&tempstr, 0x00, sizeof(tempstr));
	   memcpy(p_auth_tx->TLF01_details.card_num_len,HOST_MESSAGE.pan_len,bf2_len);
	   bcd_to_asc( HOST_MESSAGE.pan_bin, (HOST_MESSAGE.pan_len[0]+1)/2, tempstr,
				   sizeof(p_auth_tx->TLF01_details.card_num) );
	   if ( 1 == (HOST_MESSAGE.pan_len[0] % 2) )
	   {
		  itoa((HOST_MESSAGE.pan_len[0]),(char*)p_auth_tx->TLF01_details.card_num_len,10);
		  memcpy( p_auth_tx->TLF01_details.card_num, tempstr+1,
				  sizeof(p_auth_tx->TLF01_details.card_num) );
	   }
	   else
	   {
		  itoa(HOST_MESSAGE.pan_len[0],(char*)p_auth_tx->TLF01_details.card_num_len,10);
		  memcpy( p_auth_tx->TLF01_details.card_num, tempstr,
				  sizeof(p_auth_tx->TLF01_details.card_num) );
	   }
   }

   /* FIELD 3: PROCESSING CODE */
   if(BitFieldArray[bf_3proc] == 1){
   bcd_to_asc (HOST_MESSAGE.proccode,   sizeof(HOST_MESSAGE.proccode),
              (char*) p_auth_tx->TLF01_details.processing_code,
               sizeof(p_auth_tx->TLF01_details.processing_code));
   }
 
   /* FIELD 4: AMOUNT */
   if(BitFieldArray[bf_4amount] == 1){
   bcd_to_asc (HOST_MESSAGE.amount_bin, sizeof(HOST_MESSAGE.amount_bin),
              (char*) p_auth_tx->TLF01_details.total_amount,
               sizeof(p_auth_tx->TLF01_details.total_amount));
   }

   /* FIELD 6: CARDHOLDER BILLING AMOUNT */
   if(BitFieldArray[bf_6crdbillamt] == 1){
   bcd_to_asc (HOST_MESSAGE.card_bill_amt,   sizeof(HOST_MESSAGE.card_bill_amt),
              (char*) p_auth_tx->ch_billing_amt, sizeof(p_auth_tx->ch_billing_amt));
   }

   /* FIELD 7: GREENWICH TIME - MMDDhhmmss */
   if(BitFieldArray[bf_7trntime] == 1){
   bcd_to_asc (HOST_MESSAGE.gmt_time,   sizeof(HOST_MESSAGE.gmt_time),
              (char*) &p_auth_tx->TLF01_details.transmission_timestamp,
               sizeof(p_auth_tx->TLF01_details.transmission_timestamp));
   }

   /* FIELD 10: CARDHOLDER BILLING CONVERSION RATE */
   if( BitFieldArray[bf_10convrate] == 1){
   bcd_to_asc (HOST_MESSAGE.conv_rate,   sizeof(HOST_MESSAGE.conv_rate),
              (char*) p_auth_tx->ch_billing_conv_rate,
               sizeof(p_auth_tx->ch_billing_conv_rate));
   }

   /* FIELD 11: SYSTEMS TRACE AUDIT NUMBER */
   if(BitFieldArray[bf_11stan] == 1){
   bcd_to_asc (HOST_MESSAGE.stan_bin, sizeof(HOST_MESSAGE.stan_bin),
              (char*) p_auth_tx->TLF01_details.sys_trace_audit_num,
               sizeof(p_auth_tx->TLF01_details.sys_trace_audit_num));
   }

   /* FIELD 12: TRAN TIME HHMMSS */
       if(BitFieldArray[bf_12time] == 1){
	   bcd_to_asc (HOST_MESSAGE.time_bin, sizeof(HOST_MESSAGE.time_bin),
	             (char*) p_auth_tx->TLF01_details.time_hhmmss,
	               sizeof(p_auth_tx->TLF01_details.time_hhmmss));
	   }

   /* FIELD 13: TRAN DATE MMDD */
       if(BitFieldArray[bf_13date] == 1){
	   memset(tempstr, 0, sizeof(tempstr));
	   genutil_format_date (time_date);
	   memcpy (p_auth_tx->TLF01_details.date_yyyymmdd, time_date, 4);  // get year
	   bcd_to_asc (HOST_MESSAGE.date_bin, sizeof(HOST_MESSAGE.date_bin),
	               tempstr, sizeof(p_auth_tx->TLF01_details.date_yyyymmdd));
	   memcpy (p_auth_tx->TLF01_details.date_yyyymmdd+4, &tempstr, 4);
	   }

   /* FIELD 14: DATE EXPIRED */
   if(BitFieldArray[bf_14expiry] ==1){
   bcd_to_asc (HOST_MESSAGE.date_expd, sizeof(HOST_MESSAGE.date_expd),
               (char*)p_auth_tx->TLF01_details.exp_date,
               sizeof(p_auth_tx->TLF01_details.exp_date));
   }

   /* FIELD 18: SIC NUMBER (MERCHANT TYPE) */
   if(BitFieldArray[bf_18sicnumber] == 1){
   bcd_to_asc (HOST_MESSAGE.sic_number, sizeof(HOST_MESSAGE.sic_number),
               (char*)p_auth_tx->TLF01_details.category_code,
               sizeof(p_auth_tx->TLF01_details.category_code));
   }

   /* FIELD 19: COUNTRY CODE */
   if(BitFieldArray[bf_19ctrycd] == 1){
   memset( &tempstr, 0x00, sizeof(tempstr) );
   bcd_to_asc( HOST_MESSAGE.ctry_code,
               sizeof(HOST_MESSAGE.ctry_code),
               &tempstr[0],  sizeof(tempstr));
   memcpy(p_auth_tx->country_code, tempstr+1,sizeof(p_auth_tx->country_code)-1);

   /* Also store here for logging to TLF01. */
   memcpy( p_auth_tx->TLF01_details.num_dbsales, p_auth_tx->country_code, 3 );
   }

   /* FIELD 20: PRIMARY ACCOUNT NUMBER EXTENDED COUNTRY CODE */
   if(BitFieldArray[bf_20panctrycd] == 1){
   memset(&tempstr, 0x00, sizeof(tempstr));
   bcd_to_asc (HOST_MESSAGE.pan_ctry_cd, sizeof(HOST_MESSAGE.pan_ctry_cd),
               &tempstr[0],  sizeof(tempstr));
   memcpy(p_auth_tx->pan_ext_ctry_code, tempstr+1,
          sizeof(p_auth_tx->pan_ext_ctry_code)-1);
   }

   /* FIELD 22: POS ENTRY MODE  */
   if(BitFieldArray[bf_22pose] == 1){
   bcd_to_asc (HOST_MESSAGE.posentry, sizeof(HOST_MESSAGE.posentry),
               (char*)p_auth_tx->TLF01_details.pos_entry_mode,
               sizeof(p_auth_tx->TLF01_details.pos_entry_mode));
   

   if ( (HOST_MESSAGE.posentry[0] == 0x05) ||
        (HOST_MESSAGE.posentry[0] == 0x95)  )
      emv_txn = true;

   /*+-----------------------------------------------------------------+
     | Need to convert field 22 to the Hypercom Standard.              |
     |                                                                 |
     |   Visa is close to the standard, except Visa swipe and filler.  |
     |       Visa swipe = 90   convert to   Standard = 02              |
     |                                                                 |
     |   For standard, filler should be at the beginning:              |
     |       Visa = xyz0    convert to    Standard = 0xyz              |
     +-----------------------------------------------------------------+*/
   if ( 0==strncmp("90", (char*)p_auth_tx->TLF01_details.pos_entry_mode, 2) )
      memcpy( p_auth_tx->TLF01_details.pos_entry_mode, "02", 2 );

   for( i=2; i>=0; i-- )
      p_auth_tx->TLF01_details.pos_entry_mode[i+1] =
          p_auth_tx->TLF01_details.pos_entry_mode[i];
   p_auth_tx->TLF01_details.pos_entry_mode[0] = '0';
   }
   
   /* FIELD 23 : CARD SEQ NO HOST_MESSAGE.cardseqnum*/

	   if(BitFieldArray[bf_23cardseqnum] == 1){
	   bcd_to_asc (HOST_MESSAGE.cardseqnum, sizeof(HOST_MESSAGE.cardseqnum),
               (char*)p_auth_tx->host2_message_seq_nbr,
               sizeof(p_auth_tx->host2_message_seq_nbr));
	   }

   /* FIELD 24: NII (Round Trip Indicator, RTI, from Visa's header */

   /* First byte is the RTI.
    * Second byte, if 'E' indicates to echo this in response.
    */
	   
   if(BitFieldArray[bf_24nii] == 1){
   memcpy( p_auth_tx->TLF01_details.nii, HOST_MESSAGE.nii, 2 );
   }

   /* FIELD 25: POS CONDITION CODE */
   if(BitFieldArray[bf_25pocc] == 1){
   bcd_to_asc (HOST_MESSAGE.pos_condcd, sizeof(HOST_MESSAGE.pos_condcd),
              (char*) p_auth_tx->TLF01_details.pos_condition_code,
               sizeof(p_auth_tx->TLF01_details.pos_condition_code));
   field_25pocc_present = false;
   }

    /* FIELD 28: TRANSACTION FEE AMOUNT */  
   /* Tran fee amount defined in TLF01 with 8 byte. But we will get 9 byte of value from the host
      So first byte we do copy to product[2].code[0]*/
     if(BitFieldArray[bf_28feeamount] == 1){
	 p_auth_tx->TLF01_details.product_codes[2].code[0]=HOST_MESSAGE.fee_amount[0];
   	 memcpy( &p_auth_tx->TLF01_details.tran_fee_amount, &HOST_MESSAGE.fee_amount[1],
           sizeof(HOST_MESSAGE.fee_amount)-1);
	 }

   /* FIELD 32: ACQUIRING INSTITUTION ID CODE  */
   if(BitFieldArray[bf_32idcode] == 1){
   memset(&tempstr, 0x00, sizeof(tempstr));
   bcd_to_asc (HOST_MESSAGE.acq_inst_id, (HOST_MESSAGE.acq_inst_len[0]+1)/2,
				  &tempstr[0], sizeof(tempstr));
   if (1 == HOST_MESSAGE.acq_inst_len[0] % 2)
   {
      memcpy(p_auth_tx->acq_inst_id_cd, tempstr+1,
             sizeof(p_auth_tx->acq_inst_id_cd)-1);
   }
   else
   {
      memcpy(p_auth_tx->acq_inst_id_cd, tempstr,
             sizeof(p_auth_tx->acq_inst_id_cd)-1);
   }
	

   /* 
    This statement is required : because to insert into TLF01, we need to copy
	acquirer institution code into TLF01 structure variable.
	*/
   strcpy((char*)p_auth_tx->TLF01_details.acquiring_id,(char*) p_auth_tx->acq_inst_id_cd);

   p_auth_tx->acq_inst_id_len[0] =  HOST_MESSAGE.acq_inst_len[0];
   }
   
   /* FIELD 33: FORWARDING INSTITUTION ID CODE  */
   if(BitFieldArray[bf_33fwdidcode] == 1){
   memset(&tempstr, 0x00, sizeof(tempstr));
   bcd_to_asc (HOST_MESSAGE.fwd_inst_id, (HOST_MESSAGE.fwd_inst_len[0]+1)/2,
				  &tempstr[0], sizeof(tempstr));
   if (1 == HOST_MESSAGE.fwd_inst_len[0] % 2)
   {
      memcpy(p_auth_tx->forwarding_inst_id_cd, tempstr+1,
             sizeof(p_auth_tx->forwarding_inst_id_cd)-1);
   }
   else
   {
      memcpy(p_auth_tx->forwarding_inst_id_cd, tempstr,
             sizeof(p_auth_tx->forwarding_inst_id_cd)-1);
   }
   p_auth_tx->forwarding_inst_id_len[0] =  HOST_MESSAGE.fwd_inst_len[0];

   /* Store here for TLF01. */
   strcpy((char*) p_auth_tx->TLF01_details.forwarding_institution_id,
          (char*) p_auth_tx->forwarding_inst_id_cd );
   }

   /* FIELD 34: PRIMARY ACCOUNT NUMBER EXTENDED */
	//if(HOST_MESSAGE.pan_ext_len[i] != 0x00){
   if(BitFieldArray[bf_34panext] == 1){
   memset(&tempstr, 0x00, sizeof(tempstr));
   /*bcd_to_asc (HOST_MESSAGE.pan_ext, (HOST_MESSAGE.pan_ext_len[0]+1)/2,
   		     &tempstr[0], sizeof(tempstr));
   if (1 == (HOST_MESSAGE.pan_ext_len[0] % 2) )
   {
      memcpy(p_auth_tx->pan_extended, tempstr+1,
             sizeof(p_auth_tx->pan_extended)-2);
   }
   else
   {
      memcpy(p_auth_tx->pan_extended, tempstr,
             sizeof(p_auth_tx->pan_extended)-1);
   }*/
    len_f34 = HOST_MESSAGE.pan_ext_len[1];
	genutil_bin_to_ascii (HOST_MESSAGE.pan_ext, len_f34, &tempstr[0], sizeof(tempstr));
	f34 = strlen(tempstr);
	if ( f34 > 0 && f34 < 320 )
	{
      memcpy(p_auth_tx->pan_extended, tempstr,
             sizeof(p_auth_tx->pan_extended)-1);
	}
	}

   /* FIELD 35: TRACK 2 */
      if(BitFieldArray[bf_35track2] == 1){
      bcd_to_asc (HOST_MESSAGE.track2, (HOST_MESSAGE.track2_len[0]+1)/2,temp,
                  sizeof(p_auth_tx->TLF01_details.track2));

      if(temp[0]== '0')
      {
         strcpy((char*)p_auth_tx->TLF01_details.track2,&temp[1]);
      }
      else
      {
         strcpy((char*)p_auth_tx->TLF01_details.track2,temp);
      }

      memset(p_auth_tx->TLF01_details.exp_date, 0 ,
             sizeof(p_auth_tx->TLF01_details.exp_date));

      memset(p_auth_tx->TLF01_details.card_num, 0 ,
             sizeof(p_auth_tx->TLF01_details.card_num));

      if ( false == parse_track2( p_auth_tx, error_msg ) )
      {
         memcpy( p_auth_tx->TLF01_details.terminal_id, HOST_MESSAGE.termid,
                 sizeof(HOST_MESSAGE.termid));
       //  TxUtils_Send_Msg_To_Operator (1, error_msg, 1, WARN_MSG, 
        //                              "move_iso_response_to_auth_struct",
        //                               3, WARNING_ERROR,
         //                              p_auth_tx->TLF01_details.terminal_id,
         //                              p_auth_tx->TLF01_details.card_num,
           //                            p_auth_tx->TLF01_details.merchant_id );
         return(false);
      }
	  }

   /* FIELD 37: RETRIEVAL REFERENCE NUMBER */
   if(BitFieldArray[bf_37rrn] == 1){
   memcpy( p_auth_tx->TLF01_details.retrieval_ref_num, HOST_MESSAGE.rrn,
           sizeof(HOST_MESSAGE.rrn));
   }

   /* FIELD 38: AUTH NUMBER */
   if(BitFieldArray[bf_38authcode] == 1){
   memcpy( p_auth_tx->TLF01_details.auth_number, HOST_MESSAGE.authcode,
           sizeof(HOST_MESSAGE.authcode));
   }

   /* FIELD 39: RESPONSE CODE */
   if(BitFieldArray[bf_39respcode] == 1){
   memcpy( p_auth_tx->TLF01_details.response_code,
           HOST_MESSAGE.responsecd,
           sizeof(HOST_MESSAGE.responsecd) );

   if ( 0 == strcmp((char*)p_auth_tx->TLF01_details.response_code,"Q1") )
      strcpy( (char*)p_auth_tx->TLF01_details.response_code, CALL_ISSUER );
   }

   /* FIELD 41: TERMINAL ID */
   if(BitFieldArray[bf_41termid] == 1){
   memcpy( p_auth_tx->TLF01_details.terminal_id, HOST_MESSAGE.termid,
           sizeof(HOST_MESSAGE.termid));

   /* Do this in case terminal Id is not 8 characters.
    * For example, DBS has 7 character terminal Ids.
    */
   trim_spaces( p_auth_tx->TLF01_details.terminal_id );
   }

   /* FIELD 42: CARD ACCEPTOR ID CODE */
   if(BitFieldArray[bf_42cardacc] == 1){
   memcpy( p_auth_tx->TLF01_details.merchant_id, HOST_MESSAGE.cardaccid,
           sizeof(HOST_MESSAGE.cardaccid));
   }

   /* FIELD 43: CARD ACCEPTOR NAME/LOCATION */
   if(BitFieldArray[bf_43cardaccname] == 1){
   memcpy( p_auth_tx->acceptor_term_name, HOST_MESSAGE.cardaccname,    25);
   memcpy( p_auth_tx->acceptor_city_name, HOST_MESSAGE.cardaccname+25, 13);
   memcpy( p_auth_tx->acceptor_country,   HOST_MESSAGE.cardaccname+38, 2);

   /* Store DE43 here so it gets logged to TLF01:
    *   Merchant Name         --> TLF01.card_holder_name
    *   City and Country Code --> TLF01.ncf01_retired_cwk
    * SCR 775
    */
   memcpy( p_auth_tx->TLF01_details.card_holder_name,
           HOST_MESSAGE.cardaccname,
           25 );
   memcpy( p_auth_tx->TLF01_details.ncf01_retired_cwk,
           HOST_MESSAGE.cardaccname+25,
           15 );
   }

   /* FIELD 44: ADDITIONAL RESPONSE DATA */
      if(BitFieldArray[bf_44respdata] == 1){
      if ( HOST_MESSAGE.addl_rsp_len[0] > 25)
         HOST_MESSAGE.addl_rsp_len[0] = 25;

     // memcpy( &p_auth_tx->TLF01_details.resp_source,
     //         &HOST_MESSAGE.addlrspdata,
     //          HOST_MESSAGE.addl_rsp_len[0] );
	  strncpy((char*)&p_auth_tx->TLF01_details.resp_source[0],(char*)&HOST_MESSAGE.addl_rsp_len[0],1)	;	
	  memcpy( &p_auth_tx->TLF01_details.resp_source[1],
             &HOST_MESSAGE.addlrspdata,
               HOST_MESSAGE.addl_rsp_len[0] );
		

      /* Get the CAVV Result Code */
      memset( cavv_result,0x00, sizeof(cavv_result) );
      find_cavv_flag( HOST_MESSAGE.addl_rsp_len[0],
                      HOST_MESSAGE.addlrspdata,
                      cavv_result );

      if ( cavv_result[0] != 0x00 )
      {
         p_auth_tx->TLF01_details.cvc = cavv_result[0];

         if ( cavv_result[0] != '2' )
         {
            /* Log invalid result flag, then continue on. */
           // get_cavv_result_error( cavv_result[0], tempstr );
         //   TxUtils_Send_Msg_To_Operator(0, tempstr, 1, WARN_MSG, 
                                        "move_iso_response_to_auth_struct", 1,
                                      //   WARNING_ERROR,
                                       //  p_auth_tx->TLF01_details.terminal_id,
                                       //  p_auth_tx->TLF01_details.card_num,
                                       //  p_auth_tx->TLF01_details.merchant_id );
									   printf("WARNING_ERROR\n");
									
									   
         }
      }

      /* Get the Card Authentication Result Code */
      memset( carc_result, 0x00, sizeof(carc_result) );
      find_carc_flag( HOST_MESSAGE.addl_rsp_len[0],
                      HOST_MESSAGE.addlrspdata,
                      carc_result );

      if ( (carc_result[0] == '1') || (carc_result[0] == '2') )
      {
         emv_txn = true;
         if ( carc_result[0] == '1' )
         {
            strcpy( error_msg, "ARQC checked, but failed" );
          //  TxUtils_Send_Msg_To_Operator(1, error_msg, 1, WARN_MSG, 
          //                              "move_iso_response_to_auth_struct", 2,
           //                              WARNING_ERROR,
            //                             p_auth_tx->TLF01_details.terminal_id,
              //                           p_auth_tx->TLF01_details.card_num,
                //                         p_auth_tx->TLF01_details.merchant_id );
         }
      }

      /* Get the CVV2 Result Code */
      memset( cvv2_result, 0x00, sizeof(cvv2_result) );
      find_cvv2_result_code( HOST_MESSAGE.addl_rsp_len[0],
                             HOST_MESSAGE.addlrspdata,
                             cvv2_result );

      p_auth_tx->TLF01_details.product_codes[17].quantity[0] = cvv2_result[0];
      if ( cvv2_result[0] == 'N' )
      {
         strcpy( error_msg, "CVV2 No Match" );
         strcpy((char *) p_auth_tx->TLF01_details.response_text, error_msg );
        // TxUtils_Send_Msg_To_Operator(1, error_msg, 1, WARN_MSG, 
        //                             "move_iso_response_to_auth_struct", 2,
          //                            WARNING_ERROR,
            //                          p_auth_tx->TLF01_details.terminal_id,
              //                        p_auth_tx->TLF01_details.card_num,
                //                      p_auth_tx->TLF01_details.merchant_id );
	}
	  }

   /* FIELD 45: TRACK1 */
	  if(BitFieldArray[bf_45track1] == 1){
      memset( tempstr, 0x00, sizeof(tempstr) );
      memcpy( p_auth_tx->TLF01_details.track1, &HOST_MESSAGE.track1,
              HOST_MESSAGE.track1_len[0]);
      strcpy( tempstr,(char*) p_auth_tx->TLF01_details.track1 );

      /* Get card number */
      if (0x00 != (tempptr = strchr(tempstr, '^'))) 
      {
         /* Get length of card number.  Need to account for the first character,
          * a Format Code (B), which is not part of the card number.
          */
         i = tempptr-tempstr-1;
         memcpy(p_auth_tx->TLF01_details.card_num, tempstr+1, i );
         sprintf((char*) p_auth_tx->TLF01_details.card_num_len, "%02d", i);
      }

      /* Get exp date */
      /* Search for ^ from the END of track1 because track1 contains two ^s. */
      if ((tempptr = strrchr(tempstr, '^')) != 0x00) 
      {
         memcpy( p_auth_tx->TLF01_details.exp_date, tempptr+1,
                 sizeof(p_auth_tx->TLF01_details.exp_date)-1);

         /* Get the service code - used for Visa Electron. */
         memset( ServiceCode, 0x00, SERVICE_CODE_SIZE_PLUS_1 );
         memcpy( ServiceCode, tempptr+5, 3 );

	  }
	  }


   /* FIELD 48: ADDITIONAL DATA */
	  if(BitFieldArray[bf_48addl_data] == 1){
      len = HOST_MESSAGE.addl_data_len[0];
      if ( 0 == strcmp("0312",(char *)p_auth_tx->TLF01_details.message_type) )
      {
         /* Currently this field is only used for 0312 declines. */
         bcd_to_asc( HOST_MESSAGE.addl_data, len,
                    (char *) p_auth_tx->TLF01_details.resp_source,
                     len*2 );
      }
      else if ( p_auth_tx->TLF01_details.message_type[2] == '1' )
      {
         /* This is a response. */
         if ( 0 != strcmp((char *)p_auth_tx->TLF01_details.response_code,APPROVED) )
         {
            /* DE48 gives reason for declines. */
            memset( tempstr, 0x00, sizeof(tempstr) );
            for( i=0; i<len; i++ )
            {
               tempstr[i] = ascii_ebcdic_conv( HOST_MESSAGE.addl_data[i], E2A );
            }
            memcpy( p_auth_tx->TLF01_details.response_text,
                    tempstr,
                    sizeof(p_auth_tx->TLF01_details.response_text)-1 );

         //   sprintf( Buffer, "Declined (Visa) - DE48: %s", tempstr );
           // TxUtils_Send_Msg_To_Operator (1, Buffer, 1, WARN_MSG, 
             //                            "move_iso_response_to_auth_struct",
               //                           3,WARNING_ERROR,
                 //                         p_auth_tx->TLF01_details.terminal_id,
                   //                       p_auth_tx->TLF01_details.card_num,
                     //                     p_auth_tx->TLF01_details.merchant_id);
         }
      }
	  else 
	  { memset( tempstr, 0x00, sizeof(tempstr) );
            for( i=0; i<len; i++ )
            {
               tempstr[i] = ascii_ebcdic_conv( HOST_MESSAGE.addl_data[i], E2A );
            }
            memcpy( p_auth_tx->TLF01_details.response_text,
                    tempstr,
                    sizeof(p_auth_tx->TLF01_details.response_text)-1 );
   }
	  }
   /* FIELD 49 TRANSACTION CURRENCY CODE */
	  if(BitFieldArray[bf_49trncurcd] == 1){
   memset(&tempstr, 0x00, sizeof(tempstr));
   bcd_to_asc (HOST_MESSAGE.tran_curr_cd,   sizeof(HOST_MESSAGE.tran_curr_cd),
               tempstr, sizeof(p_auth_tx->TLF01_details.currency_code));   
   memcpy( p_auth_tx->TLF01_details.currency_code, tempstr+1,
           sizeof(p_auth_tx->TLF01_details.currency_code)-1 );
	  }

   /* FIELD 51 CARDHOLDER BILLING CURRENCY CODE */
   if(BitFieldArray[bf_51crdcurcd] == 1){
   memset(&tempstr, 0x00, sizeof(tempstr));
   bcd_to_asc (HOST_MESSAGE.card_curr_cd,   sizeof(HOST_MESSAGE.card_curr_cd),
               tempstr, sizeof(p_auth_tx->ch_billing_curr_code));
   memcpy( p_auth_tx->ch_billing_curr_code, tempstr+1,
           sizeof(p_auth_tx->ch_billing_curr_code)-1 );
   }

   /* FIELD 52 PIN DATA */
   if(BitFieldArray[bf_52pinblock] == 1){
   if (false == blank_or_null_string((char *)HOST_MESSAGE.pin_block,
                                     sizeof(HOST_MESSAGE.pin_block)))
   {
      memset(tempstr, 0, sizeof(tempstr));
      genutil_bin_to_ascii (HOST_MESSAGE.pin_block,
                            sizeof(HOST_MESSAGE.pin_block), 
                            &tempstr[0], sizeof(tempstr));   
      memcpy (p_auth_tx->TLF01_details.pin_block, tempstr,
              sizeof(p_auth_tx->TLF01_details.pin_block)-1);
   }
   }
   /* FIELD 54 ADDITIONAL AMOUNTS *
    * ---------------------------
    * This field contains current and/or available balance in one or
    * two currencies.  The two currencies are the issuer's currency
    * and the acquirer's currency.  There can be up to 4 sets of balances
    * here:  current and available for Issuer and Acquirer.  Acquirer
    * currency, which is what we want, is always the 3rd or the 3rd and
    * 4th set.
    *
    * If there is 1 set, it is in the Issuer's currency.  In this case,
    * just copy it into the auth_tx values, cause it is also our currency.
    *
    * If there are 2 sets, the Issuer has sent current and available in
    * the Issuer's currency.  Again, just copy these into auth_tx, because
    * if the Issuer sends only one currency, Issuer and Acquirer currency
    * must be the same.
    *
    * If there are 3 sets, copy the 3rd set (Acquirer) into auth_tx.
    *
    * If there are 4 sets, copy the 3rd and 4th (both Acquirer currencies)
    * into auth_tx.  Need to distinguish between current and available
    * balance.
    *
    * Current Balance:   For credit accounts, this is the current account
    *                    ledger balance and refers to the amount of credit
    *                    remaining to the customer.
    *
    * Available Balance: This is typically ledger balance less outstanding
    *                    authorizations.
    */

   /* Only get this data for responses. */
   if(BitFieldArray[bf_54addlamts] == 1){
   len = HOST_MESSAGE.addl_len[0];
   if ( (len > 0) && (p_auth_tx->TLF01_details.message_type[2] == '1') )
   {
      memset( tempstr, 0x00, sizeof(tempstr) );
      memcpy( tempstr, HOST_MESSAGE.addlamts, len );

      addl_amts_size = sizeof(Addl_Amts);
      bal_set = 0;

      /* Copy balance sets into structure that temporarily holds balances. */
      i = 0;
      while( i < len )
      {
         /* Initialize the structure that will hold all four sets. */
         memset( &acct_bal[bal_set], 0x00, addl_amts_size );

         memcpy( acct_bal[bal_set].acct_type, tempstr+i,   2 );
         memcpy( acct_bal[bal_set].amt_type,  tempstr+i+2, 2 );
         memcpy( acct_bal[bal_set].cur_code,  tempstr+i+4, 3 );
         acct_bal[bal_set].sign = tempstr[i+7];
         memcpy( acct_bal[bal_set].amount,    tempstr+i+8, 12);

         bal_set++;
         i += 20;   /* Increase ptr in tempstr by length of 1 set. */
      }

      /* Find the correct balance sets and copy them into auth_tx. */
      switch( bal_set )
      {
         case 1:  (void)get_balances( &acct_bal[0], p_auth_tx );
                  break;

         case 2:  (void)get_balances( &acct_bal[0], p_auth_tx );
                  (void)get_balances( &acct_bal[1], p_auth_tx );
                  break;

         case 3:  (void)get_balances( &acct_bal[2], p_auth_tx );
                  break;

         case 4:  (void)get_balances( &acct_bal[2], p_auth_tx );
                  (void)get_balances( &acct_bal[3], p_auth_tx );
                  break;
      }
   }
   else
   {
      memset(&tempstr, 0x00, sizeof(tempstr));
      memset(&templen, 0x00, sizeof(templen));
      memcpy(templen,HOST_MESSAGE.addl_len,bf54_len);
      memcpy(tempstr,HOST_MESSAGE.addlamts,sizeof(HOST_MESSAGE.addlamts));
      i = 0;
      /* the loop is controlled by the ifs at the end of the loop */
      while (i < 21 && templen[0] > 19)
      {
         if (0 == strncmp(tempstr+i+2,"01",2))   /* indicates current balance */
         {
            memcpy(p_auth_tx->ACF01_details.cash_available, tempstr+i+11, 9);
            if ('D' == tempstr[i+7])
            {
               p_auth_tx->current_balance_sign = '-';
            }
            else if ('C' == tempstr[i+7])
            {
               p_auth_tx->current_balance_sign = '+';
            }
            strncpy((char *)p_auth_tx->TLF01_details.currency_code, tempstr+i+4, 3);    
         }
         else if (0 == strncmp(tempstr+i+2,"02",2)) /*indicates available balance*/
         {
            memcpy(p_auth_tx->ACF01_details.cash_limit, tempstr+i+11,9);
            if ('D' == tempstr[i+7])
            {
               p_auth_tx->available_balance_sign = '-';
            }
            else if ('C' == tempstr[i+7])
            {
               p_auth_tx->available_balance_sign = '+';
            }
            strncpy((char *)p_auth_tx->TLF01_details.currency_code, tempstr+i+4, 3);    
         }
         if (templen[0] > 20)
            i = i + 20;  /* This will allow us to get the second set of data */
         else
            i = 40;      /* This will cause the loop to end */
	  }
   }
   }

   	/*  FIELD 55 EMV/CHIP/ data */
	if(BitFieldArray[bf_55ICCdata] == 1){
	{	CHAR  templen [6] = {0} ;INT temp_len=0;	 
	   BYTE  ICCdata [256] = {0};
	  temp_len = HOST_MESSAGE.ICCdata_len[0];
	  memcpy(p_auth_tx->EMV_details.emv_block,HOST_MESSAGE.ICCdata_len,1);
	  if ( temp_len > 0) 
	  {     memset(ICCdata, 0, sizeof(ICCdata)); 
	  		memcpy(ICCdata, HOST_MESSAGE.ICCdata, temp_len);    
			memcpy( p_auth_tx->EMV_details.emv_block+1,HOST_MESSAGE.ICCdata,temp_len);
			genutil_VISA_EMV_To_Auth_Tx( ICCdata, temp_len, p_auth_tx); 
	  }
	  }
	}

	/* FIELD 56 Customer related data */
	if(BitFieldArray[bf_56pad] == 1){
	   /* copy in parsed_msg directly */
	memset(&tempstr, 0x00, sizeof(tempstr));
	len_f56 = HOST_MESSAGE.PAD_len[1];
	genutil_bin_to_ascii (HOST_MESSAGE.PAD_data, len_f56, &tempstr[0], sizeof(tempstr));
	f56 = strlen(tempstr);
	if ( f56 > 0 && f104 < 256 )
	{
      memcpy(p_auth_tx->future_use_3, tempstr,
             f56);
	}
	}

   /* FIELD 60 RESERVED */
	  if(BitFieldArray[bf_60priv60] == 1){
      memset(&tempstr, 0x00, sizeof(tempstr));
      memset(&templen, 0x00, sizeof(templen));
      bcd_to_asc( HOST_MESSAGE.priv60_len, sizeof(HOST_MESSAGE.priv60_len),
                 &templen[0], sizeof(templen));

      bcd_to_asc( HOST_MESSAGE.priv60, (atoi(templen)),
                  (&tempstr[0]), sizeof(tempstr));

      memcpy( p_auth_tx->additional_pos_information, tempstr,
              sizeof(p_auth_tx->additional_pos_information)-1);

      /* Copy into here so this information is logged into TLF01 (SCR 743). */
      memcpy( p_auth_tx->TLF01_details.nfi_name, tempstr,
              sizeof(p_auth_tx->TLF01_details.nfi_name)-1 );
	  }
   

   /* FIELD 61: OTHER AMOUNTS */
	  if(BitFieldArray[bf_61priv61] == 1){
   memset(&tempstr, 0x00, sizeof(tempstr));
   memset(&templen, 0x00, sizeof(templen));
	bcd_to_asc (HOST_MESSAGE.priv61_len, sizeof(HOST_MESSAGE.priv61_len),
				  &templen[0], sizeof(templen));
   bcd_to_asc (HOST_MESSAGE.priv61, (atoi(templen)),
				  (&tempstr[0]), sizeof(tempstr));
   memcpy(p_auth_tx->TLF01_details.add_amounts, tempstr,
          sizeof(p_auth_tx->TLF01_details.add_amounts)-1);

   if ( 0 == strcmp((char *)p_auth_tx->TLF01_details.message_type, "0102") )
   {
      /* This is an ATM Confirmation message.
       * DE61, subfield 1 (positions 1-12) is the dispensed amount.
       */
      memcpy( p_auth_tx->TLF01_details.actual_amount, tempstr, 12 );
   }
	  }

   /* FIELD 62 CUSTOM PAYMENT SERVICE FIELDS - BITMAP */
   /* Store this for echoing. */
	if(BitFieldArray[bf_62priv62] == 1){
    memset(&tempstr, 0x00, sizeof(tempstr));
	memset(&templen, 0x00, sizeof(templen));
  // memcpy(p_auth_tx->int_net_facility_data, HOST_MESSAGE.priv62, 
         // sizeof(HOST_MESSAGE.priv62 ));
   memcpy(p_auth_tx->int_net_facility_len, HOST_MESSAGE.priv62_len, 
          sizeof(HOST_MESSAGE.priv62_len ));

	bcd_to_asc (HOST_MESSAGE.priv62_len, sizeof(HOST_MESSAGE.priv62_len),
				  &templen[0], sizeof(templen));
   bcd_to_asc (HOST_MESSAGE.priv62, atoi(templen),
				  &tempstr[0], sizeof(tempstr));
   memcpy(p_auth_tx->int_net_facility_data, tempstr, 
          atoi(templen));
   
   if (HOST_MESSAGE.priv62_len[0] > 0)  
   {
      Field62Response( p_auth_tx );
      if ( false == isalpha(p_auth_tx->TLF01_details.visa_char_indicator) )
         p_auth_tx->TLF01_details.visa_char_indicator = ' ';

      if ( p_auth_tx->TLF01_details.message_type[2] == '0' )
      {
         /* This is a request, so store 62.1 and 62.2 into the following fields
          * so they get stored into NCF30 for reversals.
          * This code was added late.  Since there are no fields in NCF30 large
          * enough to hold DE62, use a combination of network data and invoice
          * number.  Network data will hold 62.1 (1 byte) and the first 11 bytes
          * of 62.2 (15 bytes). The last 4 bytes of 62.2 go into invoice number.
          */
         p_auth_tx->TLF01_details.mcard_banknet[0] =
                                   p_auth_tx->TLF01_details.visa_char_indicator;

         memcpy( p_auth_tx->TLF01_details.mcard_banknet+1,
                 p_auth_tx->TLF01_details.visa_tran_id,
                 11);

         memcpy( p_auth_tx->TLF01_details.invoice_number,
                 p_auth_tx->TLF01_details.visa_tran_id+11,
                 4 );
      }
   }
   else
      p_auth_tx->TLF01_details.visa_char_indicator = ' '; 
	}
   /* FIELD 63: V.I.P. PRIVATE USE FIELDS */
   if(BitFieldArray[bf_63priv63] == 1){
   {
      /* Do this for switched in to echo and log the values.
       * Do for switched out to log the values.
       */
      ParseField63( (char *)HOST_MESSAGE.priv63_len, p_auth_tx );

      /* Set this bit to indicate on responses that field 63 does exist. */
      p_auth_tx->TLF01_details.product_codes[10].quantity[0] = 0x01;

      if ( p_auth_tx->TLF01_details.product_codes[12].code[0] != 0x00 )
      {
         /* STIP sent us an advice code. Log this for customer. */
         LogStipReasonCode( p_auth_tx );
      }
   }
   }

   /* FIELD 70: NETWORK MANAGEMENT INFORMATION CODE */
   if( BitFieldArray[bf_70nmicode] == 1){
   bcd_to_asc (HOST_MESSAGE.nmi_code,   sizeof(HOST_MESSAGE.nmi_code),
               nmi_code, sizeof(nmi_code));
   memcpy (p_auth_tx->function_code, nmi_code, sizeof(nmi_code));
   }

   /* FIELD 73: DATE, ACTION */
      if(BitFieldArray[bf_73action] == 1){
      bcd_to_asc (HOST_MESSAGE.action, sizeof(HOST_MESSAGE.action),
                (char *)  p_auth_tx->TLF01_details.product_codes[0].amount,
                  sizeof(p_auth_tx->TLF01_details.product_codes[0].amount));
	  }

   /* FIELD 90 ORIGINAL DATA ELEMENTS */
	  if(BitFieldArray[bf_90orgdata] == 1){
   if ( !blank_or_null_string((char *)HOST_MESSAGE.orig_data,5) )
   {
      /* Field 90 is formatted in BCD as:
       *    Orig Msg:  2 bytes
       *    Orig Stan: 3 bytes
       *    Orig DE7:  5 bytes
       *    DE32/33:  11 bytes
       * for a total of 21 BCD bytes.  Above, we checked the first 5 bytes,
       * Msg and Stan, to see if they exist. If yes, then DE90 is present.
       * So now convert to 42 bytes ASCII and check to make sure Orig Msg
       * contains a non-zero value. To be valid, the 2nd char must be non-zero.
       */
      memset( tempstr, 0x00, sizeof(tempstr) );
      bcd_to_asc (HOST_MESSAGE.orig_data,   sizeof(HOST_MESSAGE.orig_data),
                  tempstr, sizeof(tempstr));
      if((tempstr[1] == '0') && (p_auth_tx->TLF01_details.message_type[1]=='4'))
      {
         sprintf(Buffer, "Invalid data for original data elements");
      //   TxUtils_Send_Msg_To_Operator (1, Buffer, 1, WARN_MSG, 
        //                               "move_iso_response_to_auth_struct",
          //                             3,WARNING_ERROR,
            //                           p_auth_tx->TLF01_details.terminal_id,
              //                         p_auth_tx->TLF01_details.card_num,
                //                       p_auth_tx->TLF01_details.merchant_id);
         return false;
      }
      else
      {
         memcpy(p_auth_tx->TLF01_details.orig_message, tempstr, 4);
         memcpy(p_auth_tx->TLF01_details.orig_sys_trace_num, tempstr+4, 6);
         memcpy(p_auth_tx->orig_local_time_date, tempstr+10,10);
         memcpy(p_auth_tx->orig_acq_inst_id,tempstr+20,11);

         /* Actually forwarding institution id code */
         memcpy(p_auth_tx->amex_pos_entry_mode,tempstr+31,11);

         /* Since all values after message type in DE90 can be zero,
          * check the stan.  If zero, will need to use DE11 for reversals
          * to look up original transaction.
          */
         if ( 0 == strcmp((char *)p_auth_tx->TLF01_details.orig_sys_trace_num,"000000"))
         {
            memcpy( p_auth_tx->TLF01_details.orig_sys_trace_num,
                    p_auth_tx->TLF01_details.sys_trace_audit_num, 6 );
		 }
       }
   }
	  }

   /* FIELD 91: FILE UPDATE CODE */
	   if(BitFieldArray[bf_91fileupdate] == 1){
      p_auth_tx->TLF01_details.processing_code[0] = HOST_MESSAGE.file_update[0];
	   
	   }
   /* FIELD 95 REPLACEMENT AMOUNTS */
	   if(BitFieldArray[bf_95replamts] == 1){
      memcpy( p_auth_tx->TLF01_details.dcf01_retired_cwk, HOST_MESSAGE.repl_amts,
              sizeof(p_auth_tx->TLF01_details.dcf01_retired_cwk)-1);

      /* Set flag here so we know to either echo or not echo this field. */
	  for(int i=0;i<sizeof(HOST_MESSAGE.repl_amts);i++){
      if ( HOST_MESSAGE.repl_amts[i] != 0x00 )
      {
         p_auth_tx->MCF01_details.totals_ind[0] = 'E';
         memcpy( p_auth_tx->MCF01_details.address01,
                 HOST_MESSAGE.repl_amts,   30 );
         memcpy( p_auth_tx->MCF01_details.address02,
                 HOST_MESSAGE.repl_amts+30,12 );
		 break;
	  }
	  }
	   }


   /* FIELD 96: Not Used by Visa (Hdr Fld 9, Msg Status Flags, from Visa hdr. */

   /* First 3 bytes are the Message Status Flags.
    * Fourth byte, if 'E' indicates to echo this in response.
    * Use the MCF01 field because for switched in txns, MCF01 is not used.
    */
   if(BitFieldArray[bf_96msgsec] == 1){
   memcpy( p_auth_tx->MCF01_details.class_code, HOST_MESSAGE.msgsec, 4 );
   }

   /* FIELD 101: FILE NAME */
      if(BitFieldArray[bf_101fname] == 1){
      len = HOST_MESSAGE.fname_len[0];
      memcpy( p_auth_tx->TLF01_details.kek,
              HOST_MESSAGE.filename, len );
	  }

   /* FIELD 102 ACCOUNT ID #1 */
	  if(BitFieldArray[bf_102acct1] == 1){
      memcpy( p_auth_tx->acct_id_1, HOST_MESSAGE.acct_id_1,
              sizeof(HOST_MESSAGE.acct_id_1));

      /* Set flag indicating this field came in a request.
       * At this point, we don't know if this is a request
       * or response, but that is ok.  Use FAF01 because
       * EB is not using the Fleet functionality at all.
       */
      p_auth_tx->FAF01_details.velocity[0].amount[0] = '1';
	  }

   /* FIELD 103 ACCOUNT ID #2 */
	  if(BitFieldArray[bf_103acct2] == 1){
      memcpy( p_auth_tx->acct_id_2, HOST_MESSAGE.acct_id_2,
              sizeof(HOST_MESSAGE.acct_id_2));

      /* Set flag indicating this field came in a request.
       * At this point, we don't know if this is a request
       * or response, but that is ok.  Use FAF01 because
       * EB is not using the Fleet functionality at all.
       */
      p_auth_tx->FAF01_details.velocity[0].amount[1] = '1';
	  }

   /*	FIELD 104 TRAN SPECF DATA	*/
	  if(BitFieldArray[bf_104trandesc] == 1){
	   /* copy in parsed_msg directly */
	memset(&tempstr, 0x00, sizeof(tempstr));
	len_f104 = HOST_MESSAGE.trandesc_len[0];
	genutil_bin_to_ascii (HOST_MESSAGE.trandesc, len_f104, &tempstr[0], sizeof(tempstr));
	f104 = strlen(tempstr);
	if ( f104 > 0 && f104 < 512 )
	{
      memcpy(p_auth_tx->future_use_1, tempstr,
             f104);
	}
	  }

   /* FIELD 111 ADDL TRAN DATA  */
	    if(BitFieldArray[bf_111addl_tran] == 1){
		memset(&temp_str, 0x00, sizeof(temp_str));
		len_f111 = HOST_MESSAGE.addl_tran_len[1];
		genutil_bin_to_ascii (HOST_MESSAGE.addl_trandata, len_f111, &temp_str[0], sizeof(temp_str));
		f111 = strlen(temp_str);
		if ( f111 > 0 && f111 < 256 )
		{
		  memcpy(p_auth_tx->future_use_2, temp_str,
				 sizeof(p_auth_tx->future_use_2)-1);

		}
		}

   /* FIELD 115 */
		if(BitFieldArray[bf_115addl_trace] == 1){
	    memcpy( p_auth_tx->TLF01_details.dcf01_retired_cwk, HOST_MESSAGE.addl_trace,
              sizeof(HOST_MESSAGE.addl_trace));
		}

   /* FIELD 116: CARD ISSUER REFERENCE DATA */
		if(BitFieldArray[bf_116issref] == 1){
      /* Data is present. Format it into ASCII and log into TLF01. */
      Field116_Handler( (char *)HOST_MESSAGE.issref_len, p_auth_tx );
		}

   /* FIELD 121 ISSUING INSTITUTION ID CODE */
		if(BitFieldArray[bf_121issid] == 1){
   memcpy( p_auth_tx->iss_inst_id_cd, HOST_MESSAGE.iss_inst_id,
           sizeof(HOST_MESSAGE.iss_inst_id));
		}

   /* FIELD 125 SUPPORT INFORMATION */
   //for(int i=0;i<sizeof(HOST_MESSAGE.sup_info);i++){
  // if(HOST_MESSAGE.sup_info[i] != 0x00){
   //memcpy(p_auth_tx->TLF01_details.voice_auth_text,HOST_MESSAGE.sup_info,
	      //sizeof(HOST_MESSAGE.sup_info));
	  // break;
   //}
   //}

   /* FIELD 126 PRIVATE USE FIELDS */

      /* Store this information in voice_auth_text.  It is the only field
       * large enough to hold it.  No need to add any fields to TLF01.
       * EB just wants this information logged for now - no processing on it
       * except for CVV2.
       */
		if(BitFieldArray[bf_126priv126] == 1){
      memset( cvv2_data, 0x00, sizeof(cvv2_data) );
      Field126Parser( p_auth_tx, cvv2_data );

      if ( cvv2_data[0] != 0x00 )
      {
         /* CVV2 Data is made up of 3 parts:
          * 1. Presence Indicator: 0 = CVV2 bypassed or not provided
          *                        1 = CVV2 is present
          *                        2 = CVV2 on card but illegible
          *                        9 = CH states no CVV2 on card
          *
          * 2. Response Type:      0 = Send only normal RC in field 39
          *                        1 = Send RC and CVV2 Result
          *
          * 3. CVV2 Value:         Right justified, blank filled
          */
         if ( true == blank_or_null_string((char *)&cvv2_data[CVV2_VALUE], 4) )
         {
            /* CVV2 value is not present. */
            if ( (cvv2_data[CVV2_PRESENCE] == '2') ||
                 (cvv2_data[CVV2_PRESENCE] == '9')  )
            {
               /* Decline the transaction. */
               if ( cvv2_data[CVV2_PRESENCE] == '2' )
               {
                  strcpy( Buffer, "CVV2 Value is illegible on the card.");
                  p_auth_tx->TLF01_details.product_codes[17].quantity[0] = 'P';
               }
               else
               {
                  strcpy( Buffer, "Cardholder states no CVV2 on card." );
                  p_auth_tx->TLF01_details.product_codes[17].quantity[0] = 'S';
               }

               if ( cvv2_data[CVV2_RESPONSE] == '1' )
               {
                  /* Do these steps to make sure CVV2 Result Code is in resp. */
                  p_auth_tx->TLF01_details.product_codes[17].quantity[1] = '.';
                  p_auth_tx->TLF01_details.verify_mag_stripe[0] = 'Y';
                  p_auth_tx->TLF01_details.cvc = 'N';
                  strcpy( (char *)p_auth_tx->TLF01_details.security_response_code, "02" );
               }

             //  TxUtils_Send_Msg_To_Operator (1, Buffer, 1, WARN_MSG, 
               //                             "move_iso_response_to_auth_struct",
                 //                            3,WARNING_ERROR,
                   //                          p_auth_tx->TLF01_details.terminal_id,
                     //                        p_auth_tx->TLF01_details.card_num,
                       //                      p_auth_tx->TLF01_details.merchant_id);
               retval = false;
            }
            else if ( cvv2_data[CVV2_PRESENCE] == '0' )
            {
               if ( cvv2_data[CVV2_RESPONSE] == '1' )
               {
                  /* Do these steps to make sure CVV2 Result Code is in resp. */
                  p_auth_tx->TLF01_details.product_codes[17].quantity[1] = '.';
                  p_auth_tx->TLF01_details.verify_mag_stripe[0] = 'Y';
                  strcpy((char *) p_auth_tx->TLF01_details.security_response_code, "01" );
               }
            }
         }
         else
         {
            /* Copy CVV2 into Auth Tx */
            if ( cvv2_data[CVV2_VALUE] == ' ' )
               strcpy((char *)p_auth_tx->TLF01_details.cvc_data,(char *)&cvv2_data[CVV2_VALUE+1]);
            else
               strcpy((char *)p_auth_tx->TLF01_details.interest_rate,(char *)&cvv2_data[CVV2_VALUE]);

            if ( cvv2_data[CVV2_RESPONSE] == '1' )
            {
               /* Set indicator for DE44.10 in response (CVV2 Result). */
               p_auth_tx->TLF01_details.product_codes[17].quantity[0] = ' ';
               p_auth_tx->TLF01_details.product_codes[17].quantity[1] = '.';
			}
           }
         }
		}


   /* FIELD 127: FILE MAINTENANCE (FORMAT 2) */
      /* Subfield E.1 = Action Code */
		if(BitFieldArray[bf_127filerecs] == 1){
      p_auth_tx->TLF01_details.type_of_data[0]
                      = ascii_ebcdic_conv( HOST_MESSAGE.file_rec[0], E2A );

      p_auth_tx->TLF01_details.type_of_data[1]
                      = ascii_ebcdic_conv( HOST_MESSAGE.file_rec[1], E2A );

      /* Subfield E.2 = Region Coding */
      for( i=0; i<9; i++ )
      {
         p_auth_tx->TLF01_details.product_codes[1].amount[i]
                      = ascii_ebcdic_conv( HOST_MESSAGE.file_rec[i+2], E2A );
					  //break;
	  

   // untill here only is enough because we want only the moving part below is some logic and it will change the auth struct based
   // on the transaction
   

   /* Store the original currency code and amount
    * before converting them (if required)
    */
  // memcpy(p_auth_tx->reconciliation_cur_code,
   //       p_auth_tx->TLF01_details.currency_code,
    //      sizeof(p_auth_tx->TLF01_details.currency_code));
  // memcpy( p_auth_tx->reconciliation_amount,
   //       p_auth_tx->TLF01_details.total_amount,
   //       sizeof(p_auth_tx->TLF01_details.total_amount));
 //  memcpy(p_auth_tx->TLF01_details.orig_amount,
    //      p_auth_tx->TLF01_details.actual_amount,
      //    sizeof(p_auth_tx->TLF01_details.actual_amount));

   /* Save the partial reversal amount into a field that does
    * not get modified.
    */
  // memcpy( p_auth_tx->TLF01_details.refund_amount,
    //       p_auth_tx->TLF01_details.actual_amount, 12 );

   /*+------------------------------------------------------------------+
     | Here is a requirement unique to Equitable:                       |
     | ------------------------------------------                       |
     |                                                                  |
     |   If field 49 is "608" (PESO), use field 4 for the amount field, |
     |   and use the PESO Balance Account for authorization purposes.   |
     |                                                                  |
     |   If field 49 is not "608", use field 6 for the amount field, it |
     |   will be in US Dollars.                                         |
     |                                                                  |
     | This is for switched in, or Issuing, transactions only.          |
     |                                                                  |
     |                                                                  |
     | Requirements for Logging to TLF01:                               |
     | ----------------------------------                               |
     | 1.  If Currency Code (cc) is Peso                                |
     |     1.1  DE4  --> total_amount                                   |
     |     1.2  DE6  --> sales_amount  (not used, just logged)          |
     |     1.3  DE49 --> currency_code                                  |
     |     1.4  DE51 --> nfi_seq_nbr (not used, just logged)            |
     |                                                                  |
     | 2.  Else if cc is USD                                            |
     |     2.1  DE4  --> total_amount                                   |
     |     2.2  DE4  --> sales_amount (same value as DE6)               |
     |     2.3  DE49 --> currency_code                                  |
     |     2.4  DE49 --> nfi_seq_nbr (not used, just logged)            |
     |                                                                  |
     | 3.  Else (cc is other)                                           |
     |     3.1  DE4  --> sales_amount  (not used, just logged)          |
     |     3.2  DE6  --> total_amount                                   |
     |     3.3  DE49 --> nfi_seq_nbr (not used, just logged)            |
     |     3.4  DE51 --> currency_code                                  |
     |                                                                  |
     +------------------------------------------------------------------+*/
 //  if ((0 == strncmp((char *)p_auth_tx->TLF01_details.message_type, "0100", 4)) ||
  // (0 == strncmp((char *)p_auth_tx->TLF01_details.message_type, "0101", 4)) ||
  // (0 == strncmp((char *)p_auth_tx->TLF01_details.message_type, "0120", 4)) ||
   //(0 == strncmp((char *)p_auth_tx->TLF01_details.message_type, "0400", 4)) ||
   //(0 == strncmp((char *)p_auth_tx->TLF01_details.message_type, "0401", 4)) ||
   //(0 == strncmp((char *)p_auth_tx->TLF01_details.message_type, "0420", 4)))
  // {
      /* A currency code of Pesos */
    //  if ( 0 != strcmp( PESO,(char *) p_auth_tx->TLF01_details.currency_code ) )
     // {
         /* or dollars is allowed. */
       //  if ( 0 != strcmp( USD, (char *)p_auth_tx->TLF01_details.currency_code ) )
        // { 
            /* else use ch billing's currency code */
          //  if ( 0 == strcmp( USD, (char *)p_auth_tx->ch_billing_curr_code ) )
///{
               /* Currency Code is neither Peso nor USD.
                * Store according to comment box above.
                */

               /* Copy DE4 to sales_amount.  */
          //     memcpy( p_auth_tx->TLF01_details.sales_amount,
              //         p_auth_tx->TLF01_details.total_amount,
                //       12 );

               /* Copy DE6 to total_amount. */
              // memcpy( p_auth_tx->TLF01_details.total_amount,
               //        p_auth_tx->ch_billing_amt,
                //       12 );

               /* Copy DE49 to nfi_seq_nbr. */
              // memcpy( p_auth_tx->TLF01_details.nfi_seq_nbr,
                   //    p_auth_tx->TLF01_details.currency_code,
                   //    3 );

               /* Copy DE51 to currency_code. */
              // memcpy( p_auth_tx->TLF01_details.currency_code,
                  //     p_auth_tx->ch_billing_curr_code,
                  //     3 );

             //  memcpy (p_auth_tx->TLF01_details.actual_amount,
               //        p_auth_tx->TLF01_details.add_amounts+24,
                    //   sizeof(p_auth_tx->TLF01_details.actual_amount));
           // }
           // else
          //  {
           //    if ( 0==strncmp((char *)p_auth_tx->TLF01_details.processing_code,"30",2))
          //     {
                  /* Balance Inquiry can have a 3rd currency code.  But must
                   * use USD account.  Need to save original currency code
                   * to echo back out.
                   */
              //    memcpy( p_auth_tx->MCF01_details.region_nbr,
               //           p_auth_tx->TLF01_details.currency_code,
               //           3 );
              //    memcpy( p_auth_tx->TLF01_details.currency_code, USD, 3 );
             //  }
             //  else
             //  {
                  /* This is used to determine if the currency code is ok. */
              //    memcpy(p_auth_tx->TLF01_details.response_code, "91", 2);
              // }
           // }
        // }
       //  else
       //  {
            /* Currency Code is USD.  Store according to comment box above. */

            /* DE4  is already in total_amount.  */
            /* DE49 is already in currency_code. */

            /* Copy DE4 to sales_amount. */
       //     memcpy( p_auth_tx->TLF01_details.sales_amount,
       //             p_auth_tx->TLF01_details.total_amount,
    //                12 );

            /* Copy DE49 to nfi_seq_nbr. */
    //        memcpy( p_auth_tx->TLF01_details.nfi_seq_nbr,
    //                p_auth_tx->TLF01_details.currency_code,
    //                3 );
     //    }
     // }
   //   else
   //   {
         /* Currency Code is PESO.  Store according to comment box above. */

         /* DE4  is already in total_amount.  */
         /* DE49 is already in currency_code. */

         /* Copy DE6 to sales_amount. */
      //   memcpy( p_auth_tx->TLF01_details.sales_amount,
      //           p_auth_tx->ch_billing_amt,
        //         12 );

         /* Copy DE51 to nfi_seq_nbr. */
        // memcpy( p_auth_tx->TLF01_details.nfi_seq_nbr,
         //        p_auth_tx->ch_billing_curr_code,
           //      3 );
   //   }
  // }

 //  if ((0 == strncmp((char *)p_auth_tx->TLF01_details.message_type, "0400", 4)) ||
   //    (0 == strncmp((char *)p_auth_tx->TLF01_details.message_type, "0401", 4)) ||
    //   (0 == strncmp((char *)p_auth_tx->TLF01_details.message_type, "0102", 4)) ||
    //   (0 == strncmp((char *)p_auth_tx->TLF01_details.message_type, "0420", 4)))
  // {
  //    total_int    = atoi((char *)p_auth_tx->TLF01_details.total_amount);
   //   actual_int   = atoi((char *)p_auth_tx->TLF01_details.actual_amount);
   //   reversal_int = total_int - actual_int;
    //  itoa(reversal_int, (char *)p_auth_tx->TLF01_details.reversal_amount,10);
  //    Rj_with_lead_zeros((char *)p_auth_tx->TLF01_details.reversal_amount, 12);

      /* Do this check only for reversals, not ATM confirms.
       * The reason is, DE61 (actual amt for ATM Confirm) is in USD,
       * whereas DE4 will be in the original currency.
       */
    //  if ( p_auth_tx->TLF01_details.message_type[1] == '4' )
  //    {
   //      if(actual_int > total_int)
    //     {
      //      memset( tempstr, 0x00, sizeof(tempstr) );
       //     sprintf( tempstr,
        //            "Dispensed amount (%d) is greater than total amount (%d).",
         //            actual_int, total_int );
        //    TxUtils_Send_Msg_To_Operator(1, tempstr, 1, WARN_MSG,
          //                              "move_iso_response_to_auth_struct",
            //                             2,WARNING_ERROR,
              //                           p_auth_tx->TLF01_details.terminal_id,
                //                         p_auth_tx->TLF01_details.card_num,
                  //                       p_auth_tx->TLF01_details.merchant_id );
      //   }
   //   }
  // }
#if 0
   /*+-----------------------------+
     | EMV FIELDS and THIRD BITMAP |
     +-----------------------------+*/
   if ( (emv_txn == true) ||
        (HOST_MESSAGE.arpc_resp[8]    != 0x00) ||   /* If Field 139 present */
        (HOST_MESSAGE.iss_auth_len[0] != 0x00)  )   /* If Field 140 present */
   {
      emv_len = 2;  /* Allow 2 bytes for length. */

      /* FIELD 139: ARPC RESPONSE CRYPTOGRAM AND CODE */
      if ( !blank_or_null_string( HOST_MESSAGE.arpc_resp,
           sizeof(HOST_MESSAGE.arpc_resp)-1 ))
      {
         /* Convert the last 2-bytes (Resp Code) from EBCDIC to ASCII */
         HOST_MESSAGE.arpc_resp[8] = ascii_ebcdic_conv( HOST_MESSAGE.arpc_resp[8], E2A );
         HOST_MESSAGE.arpc_resp[9] = ascii_ebcdic_conv( HOST_MESSAGE.arpc_resp[9], E2A );

         memset( tempstr, 0x00, sizeof(tempstr) );
         genutil_bin_to_ascii( HOST_MESSAGE.arpc_resp,
                               sizeof(HOST_MESSAGE.arpc_resp), 
                               &tempstr[0], sizeof(tempstr) );

         /* Supply the Fixed Length value for the Device Controller
          * Binary = 10,  ASCII = 20
          */
         p_auth_tx->EMV_details.issuer_auth_data[0] = 0x0a;
         memcpy( p_auth_tx->EMV_details.issuer_auth_data+1, tempstr, 20 );

         /* Store the raw data. */
         p_auth_tx->EMV_details.emv_block[emv_len] = 0x91;  /* Tag 91 */
         emv_len++;
         p_auth_tx->EMV_details.emv_block[emv_len] = 0x0a;
         emv_len++;

         memcpy( &p_auth_tx->EMV_details.emv_block[emv_len],
                  HOST_MESSAGE.arpc_resp,
                  10 );
         emv_len += 10;
      }

      /* FIELD 140: ISSUER AUTHENTICATION DATA */
      len = HOST_MESSAGE.iss_auth_len[0];
      if ( len > 0 )
      {
         /* This field replaces field 139 for expanded format acquirers.
          * It contains information to be used by the card to perform
          * issuer authentication.
          */
         memset( tempstr, 0x00, sizeof(tempstr) );
         genutil_bin_to_ascii( HOST_MESSAGE.iss_auth_data,
                               sizeof(HOST_MESSAGE.iss_auth_data), 
                               &tempstr[0], sizeof(tempstr) );

         /* Supply the length of the Binary string for the Device Controller */
         p_auth_tx->EMV_details.issuer_auth_data[0] = len;
         memcpy( p_auth_tx->EMV_details.issuer_auth_data+1, tempstr, len*2 );

         /* Store the raw data. */
         p_auth_tx->EMV_details.emv_block[emv_len] = 0x91;  /* Tag 91 */
         emv_len++;
         p_auth_tx->EMV_details.emv_block[emv_len] = len;
         emv_len++;

         memcpy( &p_auth_tx->EMV_details.emv_block[emv_len],
                  HOST_MESSAGE.iss_auth_data,
                  len );
         emv_len += len;
      }

      /* FIELD 142: ISSUER SCRIPT */
      memset( tempstr, 0x00, sizeof(tempstr) );
      len = HOST_MESSAGE.iss_script_len[0];
      if ( len > 0 )
      {
         genutil_bin_to_ascii( HOST_MESSAGE.iss_script_len,
                               len+1, 
                               &tempstr[0], sizeof(tempstr) );

         memcpy( p_auth_tx->EMV_details.issuer_script_1, tempstr,
                 sizeof(p_auth_tx->EMV_details.issuer_script_1)-1 );

         /* Store the raw data - tag and len are embedded in data. */
         memcpy( &p_auth_tx->EMV_details.emv_block[emv_len],
                  HOST_MESSAGE.iss_script,
                  len );
         emv_len += len;
      }

      /* FIELD 143: ISSUER SCRIPT RESULTS */
      memset( tempstr, 0x00, sizeof(tempstr) );
      len = HOST_MESSAGE.iss_result_len[0];
      if ( len > 0 )
      {
         genutil_bin_to_ascii( HOST_MESSAGE.iss_result_len,
                               len+1, 
                               &tempstr[0], sizeof(tempstr) );

         memcpy( p_auth_tx->EMV_details.issuer_script_2, tempstr,
                 sizeof(p_auth_tx->EMV_details.issuer_script_2)-1 );

         /* Store the raw data - tag and len are embedded in data. */
         memcpy( &p_auth_tx->EMV_details.emv_block[emv_len],
                  HOST_MESSAGE.iss_result,
                  len );
         emv_len += len;
      }

      memset( tempstr, 0x00, sizeof(tempstr) );
      sprintf( tempstr, "%04d", emv_len-2 );
      genutil_asc_to_bcd( tempstr, 4, p_auth_tx->EMV_details.emv_block );
   }
#endif
//   if ( false == verify_auth_tx_data(p_auth_tx) )
  //    retval = false;
   
   return( retval );
   }
}
}

/******************************************************************************
 *
 *  NAME:         PARSE_TRACK2
 *
 *  DESCRIPTION:  This function parses track2 data.  It looks for the first
 *                non-numeric character to determine the field separator.
 *                It compares to anything in the card number field in auth_tx.
 *                It compares the expiration date to anything in the exp date
 *                field in auth_tx.  If empty, it copies them into those fields.
 *                It also copies the service code into a global variable in
 *                order to check for a Visa Electron transaction.
 *
 *  INPUTS:       p_auth_tx - Structure containing transaction information
 *
 *  OUTPUTS:      error_msg - Textual message in case of failure
 *
 *  RTRN VALUE:   True if track2 is successfully parsed, else false
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
INT parse_track2( pAUTH_TX p_auth_tx, pCHAR error_msg )
{
   INT    ret_val = true;
   INT    card_num_len;
   INT    track2_idx;
   pBYTE  field_separator   = NULL_PTR;

   if ( p_auth_tx->TLF01_details.track2[0] != 0x00 )
   {
      /* Locate the field separator. */
      field_separator = p_auth_tx->TLF01_details.track2;
      for(track2_idx=0;track2_idx <= bf2_max_len;track2_idx++,field_separator++)
      {
         if ( !isdigit( *field_separator ) )
            break;
      }

      if ( track2_idx > bf2_max_len )
      {
         ret_val = false;
         sprintf( error_msg,
                 "Unable to parse track2. No field separator. Track2: %s",
                  p_auth_tx->TLF01_details.track2 );
      }
      else
      {
         /* Put Card Number into auth_tx. */
         if ( p_auth_tx->TLF01_details.card_num[0] == 0x00 )
         {
            card_num_len = field_separator - p_auth_tx->TLF01_details.track2;

           /* If track2 length is odd, ignore leading zero on card number. */
               memcpy( p_auth_tx->TLF01_details.card_num,
                       p_auth_tx->TLF01_details.track2, card_num_len);
            
            sprintf( (char *)p_auth_tx->TLF01_details.card_num_len,"%02d",card_num_len);
         }

         /* Get the expiration date. */
         if ( p_auth_tx->TLF01_details.exp_date[0] == 0x00 )
         {
            memcpy( p_auth_tx->TLF01_details.exp_date, field_separator+1, 4 );
         }

         /* Get the service code. */
         memset( ServiceCode, 0x00, SERVICE_CODE_SIZE_PLUS_1 );
         memcpy( ServiceCode, field_separator+5, 3 );
      }
   }
   return( ret_val );
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
	VisaInfo jinfo;
	jinfo.DoModal();
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
