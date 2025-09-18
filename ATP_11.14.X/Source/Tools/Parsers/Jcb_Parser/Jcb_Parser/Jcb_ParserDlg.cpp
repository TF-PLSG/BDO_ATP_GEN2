
// Jcb_ParserDlg : implementation file
//

#include "stdafx.h"
#include "Jcb_Parser.h"
#include "Jcb_ParserDlg.h"
#include "afxdialogex.h"
#include "JcbParser.h"
#include "BASICTYP.H"
#include <math.h>
#include <time.h>
#include <sys/timeb.h>
#include "JcbInfo.h"
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

/* Currency Codes */
#define  USD      "840"
#define  PESO     "608"
#define N_BFData ( sizeof(BFData) / sizeof (struct bitfield_data) )

/* Field 48 Constants */
#define  FIELD48_MAX_LEN  100   

/* CAVV Constants for Ecommerce */
#define  CAVV_TAG_02      "02"
#define  CAVV_TAG_02_LEN  "47"

/* CAV2 Constants for Field 48 */
#define  FIELD48_TAG87      "87"
#define  CAV2_TAG_01		"01"
#define  CAV2_TAG_03		"03"
#define  CAV2_TAG_01_LEN	"03"
#define  CAVV_3D_SECURE_TAG_02 "02"
#define  CAVV_DATA_LEN		20
#define	 XID_DATA_LEN		20
#define  ECI_VALUE_LEN		2
#define  DE48_TAG30			"3008"
#define  DE48_TAG31			"3108"
#define  DE48_TAG04			"04"
#define  DE48_TAG20			"20"
#define  DE48_TAG21			"21"

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
"018 Sic Number				: ",
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
"032 AcquiringIdCode				: ",
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
"057 Additional data prvt			: ",
"058 Field58				: ",
"059 Detaild Inqry Data1			: ",
"060 STIP Data 				: ",
"061 Pos Data				: ",
"062 Inf Data				: ",
"063 Txn Code				: ",
"064 MsgAuthCode				: ",
"065 Field 65				: ",
"066 SettlementCode				: ",
"067 PaymentCodeEx			: ",
"068 RecInstCntrCode			: ",
"069 SettlInstCntrCode			: ",
"070 Nmi Code				: ",
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
"101 Filename				: ",
"102 AccountId1				: ",
"103 AccountId2				: ",
"104 TxnDescription				: ",
"105 Field105				: ",
"106 Field106				: ",
"107 Field107				: ",
"108 De-108	MoneySend		: ",
"109 Field109				: ",
"110 Field110				: ",
"111 Field111				: ",
"112 Addldata Nation			: ",
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
"123 IsurInstitutersrvd			: ",
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
        BYTE msgtype_bcd   [4];
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
        BYTE conv_date	   [2];
        BYTE sic_number    [2];
        BYTE ctry_code     [2];
        BYTE pan_ctry_cd   [2];
        BYTE posentry      [2];
        BYTE card_seq_num  [2];		  
        BYTE nii           [2];			  
        BYTE pos_condcd    [1];
        BYTE pin_captcd    [1];
       // BYTE tran_fee_len  [1];
        BYTE tran_fee_amount [9];
        BYTE acq_inst_len  [1];
		BYTE acq_inst_id   [6];
        BYTE fwd_inst_len  [1];			
        BYTE fwd_inst_id   [7];
        BYTE pan_ext_len   [1];
        BYTE pan_ext       [15];
        BYTE track2_len    [1];
		BYTE track2        [37];
        BYTE rrn           [12];
        BYTE authcode      [6];			 
        BYTE responsecd    [2];
		BYTE src 		   [3];
        BYTE termid        [8];
        BYTE cardaccid     [15];		 
		BYTE cardaccname   [40];
        BYTE addl_rsp_len  [1];
        BYTE addlrspdata   [25];
        BYTE track1_len    [1];
        BYTE track1        [76];
        BYTE addl_data_len [1];
        BYTE addl_data     [225];
        BYTE tran_curr_cd  [3];
        BYTE settle_cur_cd [3];
        BYTE card_curr_cd  [3];
		BYTE pin_block     [8];
        BYTE sec_cntl      [8];
        BYTE addl_len      [1];
        BYTE addlamts      [120];
        BYTE iccdata_len   [2];
        BYTE iccdata       [255];
        BYTE posdata_len   [1];
        BYTE posdata       [14];
        BYTE priv60_len    [1];
        BYTE priv60        [255];
        BYTE priv61_len    [1];
        BYTE priv61        [18];
        BYTE priv62_len    [1];
        BYTE priv62        [58];
        BYTE priv63_len    [1];
        BYTE priv63        [255];
        BYTE nmi_code      [2];
        BYTE action        [3];
        BYTE orig_data     [21];
        BYTE file_update   [1];
        BYTE resp_ind      [5];
        BYTE repl_amts     [42];
        BYTE msgsec        [8];
        BYTE setl_inst_len [1];
        BYTE setl_inst_id  [7];
        BYTE recv_inst_len [1];
        BYTE recv_inst_id  [7];
        BYTE fname_len     [1];
        BYTE filename      [18];
        BYTE acct_id_1_len [1];
        BYTE acct_id_1     [29];
        BYTE acct_id_2_len [1];
        BYTE acct_id_2     [29];
        BYTE trandesc_len  [1];
        BYTE trandesc      [100];
		BYTE msg_sec_cd    [16];
        BYTE intra_ctry_len[1];
        BYTE intra_ctry    [256];
        BYTE record_len    [2];
        BYTE record        [30];
        BYTE iss_inst_len  [1];
        BYTE iss_inst_id   [12];
        BYTE open_use_len  [1];
        BYTE open_use      [14];
        BYTE addr_verf_len [1];
        BYTE addr_verf     [29];
        BYTE free_text_len [1];
        BYTE free_text     [136];
        BYTE access_cd_len [2];
        BYTE access_cd     [255];
		BYTE dest_stat_id  [3];
    	
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
enum bitfields {
   bf_done           = 0,      /* end of list    */
   bf_1xbitfield     = 1,      /* extended bit field */
   bf_2pan           = 2,      /* pan            */
   bf_3proc          = 3,      /* processing code */
   bf_4amount        = 4,      /* amount   */
   bf_5amount        = 5,      /* settelment mount   */
   bf_6crdbillamt    = 6,      /* amount cardholder billing */
   bf_7trntime       = 7,      /* transmission date-time */
   bf_9convrate      = 9,      /* Conversion rate, settlement*/
   bf_10convrate     = 10,     /* Conversion rate, cardholder billing */
   bf_11stan         = 11,     /* system trace number */
   bf_12time         = 12,     /* time    */
   bf_13date         = 13,     /* date, trans  */
   bf_14expiry       = 14,     /* expiry date  */
   bf_15datestl      = 15,     /* settlement date */
   bf_16convdate	 = 16,	   /* Conversion date */
   bf_18sicnumber    = 18,     /* Merchant's type of business */
   bf_19ctrycd       = 19,     /* Acquiring institution's country code */
   bf_20panctrycd    = 20,     /* pan extended, country code */
   bf_22pose         = 22,     /* pos entry mode */
   bf_23csn          = 23,     /* Card Sequence Number                 */
   bf_24nii          = 24,     /* nii      */
   bf_25pocc         = 25,     /* pos condition code */
   bf_26pincc        = 26,     /* pin capture code */
   bf_28feeamount	 = 28,	   /* Tran fee amount */
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
   bf_50sttlcurcd    = 50,     /* settlement transaction currency code */
   bf_51crdcurcd     = 51,     /* card currency code */
   bf_52pinblock     = 52,     /* pin block    */
   bf_53seccntl      = 53,     /* security related control info */
   bf_54addlamts     = 54,     /* additional amounts */
   bf_55iccdata      = 55,     /* .Integrated Circuit Card (ICC) System-related Data*/
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
   bf_105msgseccode  = 105,    /* message security code */
   bf_118intractry   = 118,    /* intra country data       */
   bf_120record      = 120,    /* original message type id */
   bf_121issid       = 121,    /* issuing inst id code     */
   bf_122openuse     = 122,    /* remaining open-to-use    */
   bf_123addverf     = 123,    /* address verification data*/
   bf_124freeform    = 124,    /* free form text - Japan   */
   bf_127accesscode  = 127     /* free form text - Japan   */
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
INT		Move23_IfThere( enum bitfields fieldid, BYTE p_trantype );
void	GenericMove(enum bitfields fieldid);
void    bcd_to_asc (BYTE bstring[], INT blen, char *astr, INT astr_len);
void    Rj_with_lead_zeros( pCHAR str, INT FieldWidth );
void	RspMoveGeneric(enum bitfields fieldid);
void	genutil_format_date(pCHAR temp);
void	trim_spaces( pBYTE pString );
void	RspMovevData (enum bitfields fieldid);
int		get_variable_length( unsigned char *DataField, int FieldLen );
void	vDataMove(enum bitfields fieldid);
void	find_cvc_flag( pCHAR src_len, pCHAR src,  pCHAR dest );
void	find_cav2( BYTE src_len, pCHAR src,  pCHAR dest );
void    ncjcb_parse_DE48(BYTE src_len, pCHAR src, pCHAR);
INT     genutil_base64_to_binary( pBYTE in_rec, INT in_len, pBYTE out_rec );
void    genutil_hex_to_bin (pCHAR afield, INT how_many, BYTE bstring[]);
void    trim_spaces( pBYTE pString );
void	genutil_bin_to_ascii (BYTE bstring[], INT blen, pCHAR astr, INT astr_len);
INT     genutil_hex_to_int (pCHAR buf_ptr, INT buf_len  );
BOOLEAN blank_or_null_string(pCHAR astr, INT max_len);
void	genutil_asc_to_bcd (pCHAR afield, INT how_many, BYTE bstring[]);
void    parsed_mssg();
INT		hhutil_parse_host_response_msg(pAUTH_TX p_auth_tx);
INT		move_iso_response_to_auth_struct(CHAR nmi_code[5], pAUTH_TX p_auth_tx);
void    EBCDICMove( enum bitfields fieldid );
void	RspEBCDICvMove (enum bitfields fieldid);
void	RspEBCDICMove (enum bitfields fieldid);
void	vBCDMove(enum bitfields fieldid);
void	Field62Response(enum bitfields fieldid);
void	RspMovevBCD (enum bitfields fieldid);
INT		CondExpd( enum bitfields bitfield, BYTE p_trantype );
void	RspIgnore(enum bitfields fieldid);
INT		CondTrack1( enum bitfields bitfield, BYTE p_trantype );
void	vRspIgnore(enum bitfields fieldid);
void	Generate_CUP_EMV_Details();
void	genutil_EMV_To_Auth_Tx( BYTE * emv_raw, INT total_len, pAUTH_TX p_auth_tx );
void    JCB_EMV_To_Auth_Tx( pCHAR emv_raw, INT total_len, pAUTH_TX p_auth_tx );
void	genutil_bcd_to_asc (BYTE bstring[], INT blen, pCHAR astr, INT astr_len);
INT		Move_Never(enum bitfields, BYTE);
INT		vMove_IfThere(enum bitfields, BYTE);

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
   1,                            /* # of bytes in pan len field */
   Move_Always,                  /* mandatory    */
   vBCDMove,                     /* move routine */
   RspMovevBCD                   /* response routine */
   },
   {
   bf_3proc,                     /* field */
   HOST_MESSAGE.proccode,        /* data to move */
   3,                            /* # of bytes in field */
   Move_Always,                  /* conditional */
   GenericMove,                  /* move routine */
   RspMoveGeneric                /* response routine */
   },
   {
   bf_4amount,                   /* field */
   HOST_MESSAGE.amount_bin,      /* data to move */
   6,                            /* # of bytes in  field */
   Move_Always,                  /* conditional */
   GenericMove,                  /* move routine */
   RspMoveGeneric                /* response routine */
   },
   {
   bf_5amount,                   /* field */
   HOST_MESSAGE.settle_amt,      /* data to move */
   6,                            /* # of bytes in  field */
   Move_IfThere,                  /* conditional */
   GenericMove,                  /* move routine */
   RspMoveGeneric                /* response routine */
   },
   {
   bf_6crdbillamt,               /* field */
   HOST_MESSAGE.card_bill_amt,   /* data to move */
   6,                            /* # of bytes in field */
   Move_Always,                  /* conditional */
   GenericMove,                  /* move routine */
   RspMoveGeneric                /* response routine */
   },
   {
   bf_7trntime,                  /* field */
   HOST_MESSAGE.gmt_time,        /* data to move */
   5,                            /* # of bytes in field */
   Move_Always,                  /* conditional */
   GenericMove,                  /* move routine */
   RspMoveGeneric                /* response routine */
   },
   {
   bf_9convrate,                /* field */
   HOST_MESSAGE.conv_rate_stl,       /* data to move */
   4,                            /* # of bytes in field */
   Move_IfThere,                  /* conditional */
   GenericMove,                  /* move routine */
   RspMoveGeneric                /* response routine */
   },
   {
   bf_10convrate,                /* field */
   HOST_MESSAGE.conv_rate,       /* data to move */
   4,                            /* # of bytes in field */
   Move_Always,                  /* conditional */
   GenericMove,                  /* move routine */
   RspMoveGeneric                /* response routine */
   },
   {
   bf_11stan,                    /* field */
   HOST_MESSAGE.stan_bin,        /* data to move */
   3,                            /* # of bytes in field */
   Move_Always,                  /* conditional */
   GenericMove,                  /* move routine */
   RspMoveGeneric                /* response routine */
   },
   {
   bf_12time,                    /* field */
   HOST_MESSAGE.time_bin,        /* data to move */
   3,                            /* # of bytes in field */
   Move_Always,                 /* Mandatory */
   GenericMove,                  /* move routine */
   RspMoveGeneric                /* response routine */
   },
   {
   bf_13date,                    /* field */
   HOST_MESSAGE.date_bin,         /* data to move */
   2,                            /* # of bytes in field */
   Move_Always,                 /* conditional */
   GenericMove,                  /* move routine */
   RspMoveGeneric                /* response routine */
   },
   {
   bf_14expiry,                  /* field */
   HOST_MESSAGE.date_expd,       /* data to move */
   2,                            /* # of bytes in field */
   Move_IfThere,                     /* conditional */
   GenericMove,                  /* move routine */
   RspMoveGeneric                /* response routine */
   },
   {
   bf_15datestl,                 /* field */
   HOST_MESSAGE.date_settle,     /* data to move */
   2,                            /* # of bytes in field */
   Move_IfThere,                 /* conditional */
   GenericMove,                  /* move routine */
   RspMoveGeneric                /* response routine */
   },
   {
   bf_16convdate,                 /* field */
   HOST_MESSAGE.conv_date,       /* data to move */
   2,                            /* # of bytes in field */
   Move_IfThere,                 /* conditional */
   GenericMove,                  /* move routine */
   RspMoveGeneric                /* response routine */
   },
   {
   bf_18sicnumber,               /* field */
   HOST_MESSAGE.sic_number,      /* data to move */
   2,                            /* # of bytes in field */
   Move_Always,                  /* mandatory    */
   GenericMove,                  /* move routine */
   RspMoveGeneric                /* response routine */
   },
   {
   bf_19ctrycd,                  /* field */
   HOST_MESSAGE.ctry_code,       /* data to move */
   2,                            /* # of bytes in field */
   Move_Always,                  /* mandatory    */
   GenericMove,                  /* move routine */
   RspMoveGeneric                /* response routine */
   },
   {
   bf_20panctrycd,                    /* field */
   HOST_MESSAGE.pan_ctry_cd,     /* data to move */
   2,                            /* # of bytes in field */
   Move_IfThere,                 /* conditional */
   GenericMove,                  /* move routine */
   RspMoveGeneric                /* response routine */
   },
   {
   bf_22pose,                    /* field */
   HOST_MESSAGE.posentry,         /* data to move */
   2,                            /* # of bytes in field */
   Move_Always,                  /* conditional */
   GenericMove,                  /* move routine */
   RspMoveGeneric                /* response routine */
   },
   {
   bf_23csn ,                    /* field */
   HOST_MESSAGE.card_seq_num,    /* data to move */
   2,                            /* # of bytes in visa iso field */
   Move23_IfThere,               /* conditional */
   GenericMove,                  /* move routine */
   RspMoveGeneric                /* response routine */
   },
   {
   bf_25pocc,                    /* field */
   HOST_MESSAGE.pos_condcd,      /* data to move */
   1,                            /* # of bytes in field */
   Move_Always,                  /* conditional */
   GenericMove,                  /* move routine */
   RspMoveGeneric                /* response routine */
   },
   {
   bf_26pincc,                 /* field */
   HOST_MESSAGE.pin_captcd,      /* data to move */
   1,                            /* # of bytes in field */
   Move_IfThere,                 /* conditional */
   GenericMove,                  /* move routine */
   RspMoveGeneric                /* response routine */
   },
   {
   bf_28feeamount,				 /* field */
   HOST_MESSAGE.tran_fee_amount,	     /* data to move */
   9,							 /* # of bytes in visa iso field */
   Move_IfThere,                  /* conditional */
   EBCDICMove,                  /* move routine */
   RspEBCDICMove                /* response routine */
   },
   {
   bf_32idcode,                  /* field */
   HOST_MESSAGE.acq_inst_len,    /* data to move */
   1,                            /* # of bytes in field */
   Move_Always,                  /* conditional */
   vBCDMove,                     /* move routine */
   RspMovevBCD                   /* response routine */
   },
   {
   bf_33fwdidcode,                    /* field */
   HOST_MESSAGE.fwd_inst_len,    /* data to move */
   1,                            /* # of bytes in field */
   Move_IfThere,                 /* conditional */
   vBCDMove,                     /* move routine */
   RspMovevBCD                   /* response routine */
   },
   {
   bf_34panext,                    /* field */
   HOST_MESSAGE.pan_ext_len,    /* data to move */
   1,                            /* # of bytes in field */
   Move_IfThere,                 /* conditional */
   GenericMove,                  /* move routine */
   RspMoveGeneric                /* response routine */
   },
   {
   bf_35track2,                  /* field */
   HOST_MESSAGE.track2_len,      /* data to move */
   1,                            /* # of bytes in len field */
   vMove_IfThere,                 /* conditional */
   vBCDMove,                     /* move routine */
   RspMovevBCD                   /* response routine */
   },
   {
   bf_37rrn,                     /* field */
   HOST_MESSAGE.rrn,             /* data to move */
   12,                           /* # of bytes in field */
   Move_Always,                  /* conditional */
   EBCDICMove,                   /* move routine */
   RspEBCDICMove                 /* response routine */
   },
   {
   bf_38authcode,                /* field */
   HOST_MESSAGE.authcode,        /* data to move */
   6,                            /* # of bytes in field */
   Move_IfThere,                 /* conditional */
   EBCDICMove,                   /* move routine */
   RspEBCDICMove                 /* response routine */
   },
   {
   bf_39respcode,                /* field */
   HOST_MESSAGE.responsecd,      /* data to move */
   2,                            /* # of bytes in field */
   Move_IfThere,                 /* conditional */
   EBCDICMove,                   /* move routine */
   RspEBCDICMove                 /* response routine */
   },
   {
   bf_40src,                     /* field */
   HOST_MESSAGE.src,             /* data to move */
   3,                            /* # of bytes in field */
   Move_Always,                  /* conditional */
   EBCDICMove,                   /* move routine */
   RspEBCDICMove                 /* response routine */
   },
   {
   bf_41termid,                  /* field */
   HOST_MESSAGE.termid,          /* data to move */
   8,                            /* # of bytes in field */
   Move_IfThere,                 /* conditional */
   EBCDICMove,                   /* move routine */
   RspEBCDICMove                 /* response routine */
   },
   {
   bf_42cardacc,                 /* field */
   HOST_MESSAGE.cardaccid,       /* data to move */
   15,                           /* # of bytes in field */
   Move_IfThere,                 /* conditional */
   EBCDICMove,                   /* move routine */
   RspEBCDICMove                 /* response routine */
   },
   {
   bf_43cardaccname,             /* field */
   HOST_MESSAGE.cardaccname,     /* data to move */
   40,                           /* # of bytes in field */
   Move_Always,                  /* conditional */
   EBCDICMove,                   /* move routine */
   RspEBCDICMove                 /* response routine */
   },
   {
   bf_44respdata,                /* field */
   HOST_MESSAGE.addl_rsp_len,    /* data to move */
   1,                            /* # of bytes in len field */
   Move_IfThere,                 /* conditional */
   vEBCDICMove,                  /* move routine */
   RspEBCDICvMove                /* response routine */
   },
   {
   bf_45track1,                  /* field */
   HOST_MESSAGE.track1_len,      /* data to move */
   1,                            /* # of bytes in len field */
   CondTrack1,                   /* conditional */
   vEBCDICMove,                  /* move routine */
   RspEBCDICvMove                /* response routine */
   },
   {
   bf_48addl_data,               /* field */
   HOST_MESSAGE.addl_data_len,   /* data to move */
   1,                            /* # of bytes in len field */
   Move_IfThere,                 /* mandatory    */
   vDataMove,                    /* move routine */
   RspMovevData                  /* response routine */
   },
   {
   bf_49trncurcd,                /* field */
   HOST_MESSAGE.tran_curr_cd,    /* data to move */
   3,                            /* # of bytes in field */
   Move_Always,                  /* mandatory    */
   EBCDICMove,                   /* move routine */
   RspEBCDICMove                 /* response routine */
   },
   {
   bf_50sttlcurcd,                /* field */
   HOST_MESSAGE.settle_cur_cd,    /* data to move */
   3,                            /* # of bytes in field */
   Move_IfThere,                  /* conditional    */
   EBCDICMove,                   /* move routine */
   RspEBCDICMove                 /* response routine */
   },
   {
   bf_51crdcurcd,                /* field */
   HOST_MESSAGE.card_curr_cd,    /* data to move */
   3,                            /* # of bytes in field */
   Move_Always,                  /* mandatory    */
   EBCDICMove,                   /* move routine */
   RspEBCDICMove                 /* response routine */
   },
   {
   bf_52pinblock,                /* field */
   HOST_MESSAGE.pin_block,       /* data to move */
   8,                            /* # of bytes in field */
   Move_IfThere,                 /* conditional */
   GenericMove,                  /* move routine */
   RspMoveGeneric                /* response routine */
   },
   {
   bf_53seccntl,                 /* field */
   HOST_MESSAGE.sec_cntl,        /* data to move */
   8,                            /* # of bytes in field */
   Move_IfThere,                 /* conditional */
   GenericMove,                  /* move routine */
   RspMoveGeneric                /* response routine */
   },
   {
   bf_54addlamts,                /* field */
   HOST_MESSAGE.addl_len,        /* data to move */
   1,                            /* # of bytes in len field */
   Move_IfThere,                 /* conditional */
   vEBCDICMove,                  /* move routine */
   RspEBCDICvMove                /* response routine */
   },
   {
   bf_55iccdata,                 /* field */
   HOST_MESSAGE.iccdata_len,     /* data to move */
   2,                            /* # of bytes in addl len field */
   Move_IfThere,                 /* conditional */
   vDataMove,                    /* move routine */
   RspMovevData                  /* response routine */
   },
   {
   bf_59posdata,                 /* field */
   HOST_MESSAGE.posdata_len,     /* data to move */
   1,                            /* # of bytes in len field */
   Move_IfThere,                 /* conditional */
   vEBCDICMove,                  /* move routine */
   RspEBCDICvMove                /* response routine */
   },
   {
   bf_60priv60,                  /* field */
   HOST_MESSAGE.priv60_len,      /* data to move */
   1,                            /* # of bytes in len field */
   Move_IfThere,                 /* conditional */
   vEBCDICMove,                  /* move routine */
   RspEBCDICvMove                /* response routine */
   },
   {
   bf_61priv61,                  /* field */
   HOST_MESSAGE.priv61_len,      /* data to move */
   1,                            /* # of bytes in priv61 len field */
   Move_IfThere,                 /* conditional */
   vEBCDICMove,                  /* move routine */
   RspEBCDICvMove                /* response routine */
   },
   {
   bf_62priv62,                  /* field */
   HOST_MESSAGE.priv62_len,      /* data to move */
   1,                            /* # of bytes in priv62 len field */
   Move_IfThere,                 /* conditional */
   vDataMove,                    /* move routine */
   Field62Response               /* response routine */
   },
   {
   bf_63priv63,                  /* field */
   HOST_MESSAGE.priv63_len,      /* data to move */
   1,                            /* # of bytes in priv63 len field */
   Move_IfThere,                 /* conditional */
   vEBCDICMove,                  /* move routine */
   RspEBCDICvMove                /* response routine */
   },
   {
   bf_70nmicode,                 /* field */
   HOST_MESSAGE.nmi_code,        /* data to move */
   2,                            /* # of bytes in field */
   Move_Always,                  /* mandatory    */
   GenericMove,                  /* move routine */
   RspMoveGeneric                /* response routine */
   },
   {
   bf_73action,                  /* field */
   HOST_MESSAGE.action,          /* data to move */
   3,                            /* # of bytes in field */
   Move_Always,                  /* conditional */
   GenericMove,                  /* move routine */
   RspMoveGeneric                /* response routine */
   },
   {
   bf_90orgdata,                 /* field */
   HOST_MESSAGE.orig_data,       /* data to move */
   21,                           /* # of bytes in field */
   Move_Always,                  /* mandatory    */
   GenericMove,                  /* move routine */
   RspMoveGeneric                /* response routine */
   },
   {
   bf_91fileupdate,              /* field */
   HOST_MESSAGE.file_update,     /* data to move */
   1,                            /* # of bytes in field */
   Move_Always,                  /* conditional */
   EBCDICMove,                   /* move routine */
   RspEBCDICMove                 /* response routine */
   },
   {
   bf_93rspind,                  /* field */
   HOST_MESSAGE.resp_ind,        /* data to move */
   5,                            /* # of bytes in field */
   Move_IfThere,                 /* conditional  */
   EBCDICMove,                   /* move routine */
   RspEBCDICMove                 /* response routine */
   },
   {
   bf_95replamts,                /* field */
   HOST_MESSAGE.repl_amts,       /* data to move */
   42,                           /* # of bytes in field */
   Move_IfThere,                 /* conditional  */
   EBCDICMove,                   /* move routine */
   RspEBCDICMove                 /* response routine */
   },
   {
   bf_96msgsec,                  /* field */
   HOST_MESSAGE.msgsec,          /* data to move */
   8,                            /* # of bytes in field */
   Move_IfThere,                 /* conditional  */
   GenericMove,                  /* move routine */
   RspMoveGeneric                /* response routine */
   },
   {
   bf_99stlinstid,               /* field */
   HOST_MESSAGE.setl_inst_len,   /* data to move */
   1,                            /* # of bytes in field */
   Move_IfThere,                 /* mandatory    */
   GenericMove,                  /* move routine */
   RspMoveGeneric                /* response routine */
   },
   {
   bf_100rcvid,                  /* field */
   HOST_MESSAGE.recv_inst_len,   /* data to move */
   1,                            /* # of bytes in field */
   Move_IfThere,                 /* conditional */
   vBCDMove,                     /* move routine */
   RspMovevBCD                   /* response routine */
   },
   {
   bf_101fname,                  /* field */
   HOST_MESSAGE.fname_len,       /* data to move */
   1,                            /* # of bytes in field */
   Move_IfThere,                 /* conditional */
   vEBCDICMove,                  /* move routine */
   RspEBCDICvMove                /* response routine */
   },
   {
   bf_102acct1,                  /* field */
   HOST_MESSAGE.acct_id_1_len,   /* data to move */
   1,                            /* # of bytes in field */
   Move_IfThere,                 /* conditional */
   vEBCDICMove,                  /* move routine */
   RspEBCDICvMove                /* response routine */
   },
   {
   bf_103acct2,                  /* field */
   HOST_MESSAGE.acct_id_2_len,   /* data to move */
   1,                            /* # of bytes in field */
   Move_IfThere,                 /* conditional */
   vEBCDICMove,                  /* move routine */
   RspEBCDICvMove                /* response routine */
   },
   {
   bf_104trandesc,               /* field */
   HOST_MESSAGE.trandesc_len,    /* data to move */
   1,                            /* # of bytes in field */
   Move_IfThere,                 /* conditional */
   vEBCDICMove,                  /* move routine */
   RspEBCDICvMove                /* response routine */
   },
   {
   bf_105msgseccode,			 /* field */
   HOST_MESSAGE.msg_sec_cd,	     /* data to move */
   16,                           /* # of bytes in field */
   Move_IfThere,                 /* conditional */
   GenericMove,                  /* move routine */
   RspMoveGeneric				 /* response routine */
   },
   {
   bf_118intractry,              /* field */
   HOST_MESSAGE.intra_ctry_len,  /* data to move */
   1,                            /* # of bytes in field */
   Move_Never,                   /* conditional */
   GenericMove,                  /* move routine */
   RspMoveGeneric                /* response routine */
   },
   {
   bf_120record,                 /* field */
   HOST_MESSAGE.record_len,          /* data to move */
   2,                            /* 2 byte length field */
   Move_IfThere,                 /* conditional */
   vEBCDICMove,                  /* move routine */
   RspEBCDICvMove                /* response routine */
   },
   {
   bf_121issid,                  /* field */
   HOST_MESSAGE.iss_inst_len,    /* data to move */
   1,                            /* # of bytes in field */
   Move_IfThere,                   /* conditional */
   vEBCDICMove,                  /* move routine */
   RspEBCDICvMove                /* response routine */
   },
   {
   bf_122openuse,                /* field */
   HOST_MESSAGE.open_use_len,    /* data to move */
   1,                            /* # of bytes in field */
   Move_IfThere,                 /* conditional */
   GenericMove,                  /* move routine */
   RspMoveGeneric                /* response routine */
   },
   {
   bf_123addverf,                /* field */
   HOST_MESSAGE.addr_verf_len,   /* data to move */
   1,                            /* # of bytes in field */
   Move_IfThere,                 /* conditional */
   GenericMove,                  /* move routine */
   RspMoveGeneric                /* response routine */
   },

   {
   bf_124freeform,               /* field */
   HOST_MESSAGE.free_text_len,   /* data to move */
   1,                            /* # of bytes in field */
   Move_IfThere,                 /* conditional */
   GenericMove,                  /* move routine */
   RspMoveGeneric                /* response routine */
   },

   {
   bf_127accesscode,             /* field */
   HOST_MESSAGE.access_cd_len,   /* data to move */
   2,                            /* # of bytes in field */
   Move_IfThere,                 /* conditional */
   vEBCDICMove,                  /* move routine */
   RspEBCDICvMove                /* response routine */
   },

   {
   bf_done,                      /* field */
   NULL,                         /* data to move */
   99,                           /* # of bytes in field */
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
/*************************************/

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

} /* genutil_format_date */
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
   INT                  movelen;
   INT                  j = 0;

   /*---------------------------------------------------*/
   /* find pointer to bitfield info for this bitfield...*/
   /*---------------------------------------------------*/
   pBF = Find_Bitfield_Table(fieldid);

   if (pBF != NULL)
   {
      /* Field 55 is a 2-byte length field.  First byte is NULL. */
      if ( fieldid == 55 )
      {
         reqidx++;
         j = 1;
      }

      movelen = ReqRsp_Buffer [reqidx];
      pBF->p_bd_pdata[j] = ReqRsp_Buffer [reqidx];
      reqidx++;
      if ((reqidx + movelen) <= buflen)
      {
         memcpy(&pBF->p_bd_pdata[1+j], &ReqRsp_Buffer[reqidx], movelen);

         reqidx = reqidx + movelen;   /* increment buffer pointer */
      }
      else
         invalid_msg_format = true;
   }
}

void RspMovevData_Old (enum bitfields fieldid)
{
   struct bitfield_data *pBF;
   INT                  i;
   INT                  FieldLen;
   double               movelen;
   unsigned char        cLen;
   unsigned char        cFieldData;
   char temp_len[5] = {0};
   int len =0;

   /*---------------------------------------------------*/
   /* Find pointer to bitfield info for this bitfield...*/
   /*---------------------------------------------------*/
   pBF = Find_Bitfield_Table(fieldid);
   if (pBF != NULL)
   {
      FieldLen = pBF->bd_fieldlen;

	  if(fieldid == 55)
	  {
	  				genutil_bin_to_ascii (&ReqRsp_Buffer[reqidx], 2,temp_len, 2);
					len = genutil_hex_to_int(temp_len,strlen(temp_len));
					if ( (reqidx + len) <= COMM_BUF_SZE )
					  {
						 /* Move the rest of the data; do not convert it. */
						 memcpy( pBF->p_bd_pdata+FieldLen, ReqRsp_Buffer+reqidx, (INT)len );
						 reqidx += (INT)len;
					  }
					//add the field len
					reqidx += 2;
				//	memcpy(&pBF->p_bd_pdata[i], cFieldData, 2);
					sprintf((char *)&pBF->p_bd_pdata[0],"%d",len);


	  }
	  else
	  {
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
   }
   return( retval );
} /* end Move23_IfThere */

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
   INT                  i;
   INT                  j;
   INT                  movelen;

   /*---------------------------------------------------*/
   /* Find pointer to bitfield info for this bitfield...*/
   /*---------------------------------------------------*/
   pBF = Find_Bitfield_Table(fieldid);

   /* Determine number of bytes to move. */
   if (pBF != NULL)
   {
      /* Fields 120 and 127 have 2 byte lengths.
       * But neither field uses the first byte.
       */
      if ( (fieldid == 120) || (fieldid == 127) )
      {
         j = 1;
         pBF->p_bd_pdata[0] = ReqRsp_Buffer[reqidx++];
         pBF->p_bd_pdata[1] = ReqRsp_Buffer[reqidx];
      }
      else
      {
         j = 0;
         pBF->p_bd_pdata[0] = ReqRsp_Buffer[reqidx];
      }
      movelen = ReqRsp_Buffer[reqidx++];

      if ((reqidx+movelen) <= buflen)
      {
			 /* Move the data, converting it from EBCDIC to ASCII. */
			 for( i=1; i<=movelen; i++)
			 {
				pBF->p_bd_pdata [i+j] = ascii_ebcdic_conv (ReqRsp_Buffer[reqidx], E2A);
				reqidx++;
			 }
      }
      else
         invalid_msg_format = true;
   }
} /* end RspEBCDICvMove */


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
void Field62Response(enum bitfields fieldid)
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
    
      /* Only the first 3 fields are use in JCB. */
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
      movelen = ReqRsp_Buffer[reqidx];
      pBF->p_bd_pdata[0] = ReqRsp_Buffer[reqidx];
      reqidx++;

      //if ( 60 == fieldid )
      //   movelen *= 2;

      if ((movelen % 2) != 0)
         movelen++;

      movelen/= 2;
      if ((reqidx + movelen) <= COMM_BUF_SZE)
      {
         memcpy(&pBF->p_bd_pdata[1], &ReqRsp_Buffer[reqidx], movelen);

         reqidx = reqidx + movelen;   /* increment buffer pointer */
      }
      else
         invalid_msg_format = true;
   }
} /* end RspMovevBCD */

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

   /*---------------------------------------------------*/
   /* Find pointer to bitfield info for this bitfield...*/
   /*---------------------------------------------------*/
   pBF = Find_Bitfield_Table(fieldid);
   if (pBF != NULL)
   {
      movelen = pBF->bd_fieldlen;

      if ((reqidx + movelen) <= buflen)
      {
         /* Move the data, converting it from EBCDIC to ASCII. */
         for( i=0; i<movelen; i++)
         {
            pBF->p_bd_pdata [i] = ascii_ebcdic_conv (ReqRsp_Buffer[reqidx], E2A);
            reqidx++;
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
#define CRYPTO_INFO_DATA_ASCII_LEN 2
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
#define MCRD_EMV_MAX_LEN 255
#define ISSUERAPPID      32

/*****************************************************************************

  Function:    find_cvc_flag

  Description: This function will parse field 48 of a 0110 response message 
  looking for Tag 87.  This tag contains a 1 byte flag: Magnetic Stripe / 
  CVC Invalid.
  
    FIELD 48 FORMAT:
 
      -------+------+---------+---------+---------+---------+-----
       Total |Trans | First   | First   | First   | Second  |
        Len  | Cat. |SubField |SubField |SubField |SubField | . . .
             | Code |  Tag    | Length  |  Data   |  Tag    |
      -------+------+---------+---------+---------+---------+-----
  BYTES: 3       1       2         2         x         2
 
  Author: 
      unknown
  Inputs:
      src_len - the length of src
      src - field 48
  Outputs:
      dest - the cvc flag
  Return values:
      None
  Modified by:
      BGable      6/30/98
******************************************************************************/
void find_cvc_flag( pCHAR src_len, pCHAR src,  pCHAR dest )
{
   #define  FIELD48_TAG      "87"
   INT  sub_field_len;
   CHAR tag[3];
   CHAR len[3];
   INT  MainIndex;
   INT  TotalLen;
   /* Field 48 Constants */
#define  FIELD48_MAX_LEN  100  


   TotalLen = atoi( src_len );

   /* MainIndex starts at 1 to skip over the Transaction Category Code. */
   for( MainIndex=1; MainIndex<TotalLen && MainIndex<FIELD48_MAX_LEN; )
   {
      memset( tag, 0x00, sizeof( tag ) );
      memcpy( tag, &src[MainIndex], 2 );
      MainIndex += 2;
      memset( len, 0x00, sizeof(len));
      memcpy( len, &src[MainIndex], 2 );
      sub_field_len = atoi( len );
      MainIndex += 2;

      if ( 0 == strcmp( tag, FIELD48_TAG ) )
      {
         strncpy( dest, &src[MainIndex], sub_field_len );
         break;
      }
      else
      {
         MainIndex += sub_field_len;
      }
   }
}  /* End find_cvc_flag */


/******************************************************************************
 *
 *  NAME:         FIND_CAV2
 *
 *  DESCRIPTION:  This function will parse field 48 of an 0100 or 0120
 *                message from the JCB host.  It will return the CAV2
 *                value (if it is present).  CAV2 is like Visa's CVV2 and
 *                MasterCard's CVC2.
 *
 *                The tags and length bytes are in EBCDIC.  So they must
 *                be converted into ASCII.
 *
 *  FIELD 48 FORMAT:
 *            -------+---------+---------+---------+---------+-----
 *             Total | First   | First   | First   | Second  |
 *              Len  |SubField |SubField |SubField |SubField | . . .
 *                   |  Tag    | Length  |  Data   |  Tag    |
 *            -------+---------+---------+---------+---------+-----
 *      BYTES:   1        2         2         x         2
 *
 *
 *  INPUTS:       src_len - Length of src
 *                src     - Field 48
 *
 *  OUTPUTS:      dest - The CAV2 value, or null
 *
 *  RTRN VALUE:   None
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
void find_cav2( BYTE src_len, pCHAR src,  pCHAR dest )
{
   INT  sub_field_len;
   CHAR tag[3];
   CHAR len[3];
   CHAR ascii_tag[3] = "";
   CHAR ascii_len[3] = "";
   INT  MainIndex;
   INT  TotalLen;
   /* CAVV Constants for Ecommerce */
#define  CAVV_TAG_02      "02"
#define  CAVV_TAG_02_LEN  "47"

/* CAV2 Constants for Field 48 */
#define  CAV2_TAG_01      "01"
#define  CAV2_TAG_01_LEN  "03"

   TotalLen = src_len;

   dest[0] = 0x00;
   for( MainIndex=0; MainIndex<TotalLen && MainIndex<FIELD48_MAX_LEN; )
   {
      memset( tag, 0x00, sizeof( tag ) );
      memcpy( tag, &src[MainIndex], 2 );
      ascii_tag[0] = ascii_ebcdic_conv( tag[0], E2A );
      ascii_tag[1] = ascii_ebcdic_conv( tag[1], E2A );
      MainIndex += 2;

      memset( len, 0x00, sizeof(len));
      memcpy( len, &src[MainIndex], 2 );
      ascii_len[0] = ascii_ebcdic_conv( len[0], E2A );
      ascii_len[1] = ascii_ebcdic_conv( len[1], E2A );

      sub_field_len = atoi( ascii_len );
      MainIndex += 2;

      if ( 0 == strcmp( ascii_tag, CAV2_TAG_01 ) )
      {
         /* Found it.  Convert from EBCDIC to ASCII. */
         dest[0] = ascii_ebcdic_conv( src[MainIndex  ], E2A );
         dest[1] = ascii_ebcdic_conv( src[MainIndex+1], E2A );
         dest[2] = ascii_ebcdic_conv( src[MainIndex+2], E2A );
         break;
      }
      else
      {
         MainIndex += sub_field_len;
      }
   }
   return;
}

void ncjcb_parse_DE48(BYTE src_len, pCHAR src, pCHAR de48_databuffer)
{
	   INT  sub_field_len 	=  0;
	   CHAR tag[3]		  	= {0};
	   CHAR len[3]		  	= {0};
	   CHAR ascii_tag[3] 	= {0};
	   CHAR ascii_len[3] 	= {0};
	   INT  MainIndex		=  0;
	   INT  TotalLen		=  0;
	   INT  i 				=  0;
	   CHAR cvv2_buffer[256]= {0};
	   CHAR cvv_data[256]   = {0};

	   TotalLen = src_len;

	   for( MainIndex=0; MainIndex<TotalLen && MainIndex<FIELD48_MAX_LEN; )
	   {
		  memset(cvv2_buffer, 0x00, sizeof(cvv2_buffer));
		  memset(cvv_data, 0x00, sizeof(cvv_data));
	      memset( tag, 0x00, sizeof( tag ) );
	      memcpy( tag, &src[MainIndex], 2 );
	      ascii_tag[0] = ascii_ebcdic_conv( tag[0], E2A );
	      ascii_tag[1] = ascii_ebcdic_conv( tag[1], E2A );
	      MainIndex += 2;

	      memset( len, 0x00, sizeof(len));
	      memcpy( len, &src[MainIndex], 2 );
	      ascii_len[0] = ascii_ebcdic_conv( len[0], E2A );
	      ascii_len[1] = ascii_ebcdic_conv( len[1], E2A );

	      sub_field_len = atoi( ascii_len );
	      MainIndex += 2;

	      if ( ( 0 == strncmp( ascii_tag, DE48_TAG30, 2 ) )  ||
			   ( 0 == strncmp( ascii_tag, DE48_TAG31, 2 ) )  ||
			   ( 0 == strncmp( ascii_tag, CAV2_TAG_01, 2 ) ) ||
			   ( 0 == strncmp( ascii_tag, CAV2_TAG_03, 2 ) ) ||
			   ( 0 == strncmp( ascii_tag, DE48_TAG04, 2 ) )  ||
			   ( 0 == strncmp( ascii_tag, DE48_TAG20, 2 ) )  ||
			   ( 0 == strncmp( ascii_tag, DE48_TAG21, 2 ) )
			 )
	      {
			  strcat(de48_databuffer, ascii_tag);
			  strcat(de48_databuffer, ascii_len);

		 	  for( i=0; i<sub_field_len; i++ )
			  {
                  cvv2_buffer[i] = ascii_ebcdic_conv (src[MainIndex+i], E2A);
			  }
			  strcat(de48_databuffer, cvv2_buffer);
	      }
		  else if( ( 0 == strncmp( ascii_tag, CAVV_TAG_02, 2 ) ) )
		  {
			  strcat(de48_databuffer, ascii_tag);
			  strcat(de48_databuffer, ascii_len);

			  //skip the XID 20bytes filled with spaces
		      genutil_bin_to_ascii ((BYTE*)&src[MainIndex+20], 20, cvv_data, 40);
			  strcat(de48_databuffer, cvv_data);
		  }
	      MainIndex += sub_field_len;  
	   }
}

void genutil_hex_to_bin (pCHAR afield, INT how_many, BYTE bstring[])
{
   pCHAR   astr;
   INT     index;


   astr = afield;

   for (index = 0; index < how_many; index=index+2) 
      {
      if ( (astr [index] >= '0')  &&  (astr [index] <= '9') )
         bstring [index/2] = ((INT) (astr [index] - '0')) * 16;
      else
         if ( (astr [index] >= 'A')  &&  (astr [index] <= 'F') )
            bstring [index/2] = ((INT) (astr [index] - 'A') + 10) * 16;

      if ( (astr [index+1] >= '0')  &&  (astr [index+1] <= '9') )
         bstring [index/2]+= ((INT) (astr [index+1] - '0'));
      else
         if ( (astr [index+1] >= 'A')  &&  (astr [index+1] <= 'F') )
            bstring [index/2]+= ((INT) (astr [index+1] - 'A') + 10);
      } /* end of for*/
}

INT genutil_base64_to_binary( pBYTE in_rec, INT in_len, pBYTE out_rec )
{
   /*
    * Translation Table as described in RFC1113 (not used, here for reference.
    *
    * static const char cb64[]="ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    */

   /*
    * Translation Table to decode
    */
   static const char cd64[]="|$$$}rstuvwxyz{$$$$$$$>?@ABCDEFGHIJKLMNOPQRSTUVW$$$$$$XYZ[\\]^_`abcdefghijklmnopq";

   BYTE  in[4], out[3], v;
   INT   i, len;
   INT   in_ctr  = 0;
   INT   out_ctr = 0;

   while ( in_ctr < in_len )
   {
      memset( in,  0x00, sizeof(in)  );
      memset( out, 0x00, sizeof(out) );

      for( len = 0, i = 0; i < 4 && (in_ctr < in_len); i++ )
      {
         v = 0;
         while( (in_ctr < in_len) && v == 0 )
         {
            v = in_rec[in_ctr++];
            v = (unsigned char) ((v < 43 || v > 122) ? 0 : cd64[ v - 43 ]);
            if( v )
            {
               v = (unsigned char) ((v == '$') ? 0 : v - 61);
            }
         }
         if (in_ctr <= in_len)
         {
            len++;
            if( v )
            {
               in[ i ] = (unsigned char) (v - 1);
            }
         }
         else
         {
            in[i] = 0;
         }
      }

      if ( len )
      {
         /* Decode 4 '6-bit' characters into 3 '8-bit' binary bytes */
         out[ 0 ] = (unsigned char ) (in[0] << 2 | in[1] >> 4);
         out[ 1 ] = (unsigned char ) (in[1] << 4 | in[2] >> 2);
         out[ 2 ] = (unsigned char ) (((in[2] << 6) & 0xc0) | in[3]);

         /* Put the 3 binary bytes into the output buffer. */
         for( i = 0; i < len - 1; i++ )
         {
            out_rec[out_ctr+i] = out[i];
         }
         out_ctr += len-1;
      }
   }
   return( out_ctr );
}

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

  Function:   genutil_hex_to_int

  Description:
      This function will deocde the hex characters into an integer.
  Author: 
      unknown
  Inputs:
      buf_ptr  - pointer to the ascii hex digits ('0' to '9', 'A' to 'F').
                 this buffer is not assumed, nor expected, to be a null
                 terminated string.
      buf_len  - number of digits in the buffer.
  Outputs:
      None
  Return values:
      INT   -  the integer decode result.
  Modified by:
      BGable      6/26/98
******************************************************************************/
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

  Function:   genutil_asc_to_bcd

  Description: 
      convert ascii string to bcd string
  Author: 
      unknown
  Inputs:   
      afield   - pointer to the incoming ascii string to be converted
      how_many - number to convert
  Outputs: 
      bstring  - outgoing bcd string
  Return values:
      None
  Modified by:
      BGable      6/26/98
******************************************************************************/
void genutil_asc_to_bcd (pCHAR afield, INT how_many, BYTE bstring[])
{
   pCHAR astr;
   UINT  i = 0;


   astr = afield;

   for (i = (strlen(afield) - how_many); i < strlen(afield); i=i+2)
      {
      /* if I hit a null terminator, quit.  */
      if (astr[i] == '\0')
         break;

      /* if these are leading blanks, leave the bcd zero.  */
      if (astr[i] != ' ')
         bstring[i/2] = ((astr[i]-'0')*16);

      if ( ((i+1) < strlen(afield)) &&
           (astr[i+1] != ' ')          )
         bstring[i/2] += (astr[i+1]-'0');
      }
} /* genutil_asc_to_bcd */


int CheckNotNull(BYTE data[], int len)
{
	int i;

	for(i=0; i<len;i++)
	{
		if(data[i] != '\0')
			return 1;
	}
	return 0;
}


/*********ParsedMssg() to print the amex parsed message****/
void parsed_mssg()
{
	int bitCount, startCharacter, icnt, LastDisplayBit;
	CHAR local_buffer[256] = {0};

	memset(Line,0,sizeof(Line));
	


	strcpy (Line[0], "000 MsgType				: ");
	strncat(Line[0],(char *)HOST_MESSAGE.msgtype_bcd,4);

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
			//if(strlen((char *)HOST_MESSAGE.pan_len) != 0)     
			if(BitFieldArray[bf_2pan] == 1) 
			{	
				genutil_bin_to_ascii (HOST_MESSAGE.pan_len, 1,temp_len, 2);
				len = genutil_hex_to_int(temp_len,strlen(temp_len));
				strcat (Line[bitCount], FieldStrings [bitCount]);
				strncat (Line[bitCount], (char *)AUTH_TX_V.TLF01_details.card_num,len);				
			}	
		break;
		
		case bf_3proc :
			if(BitFieldArray[bf_3proc] == 1){
			len = strlen((char *)AUTH_TX_V.TLF01_details.processing_code);
			strncpy((char *)temp_data,(char *)AUTH_TX_V.TLF01_details.processing_code,len);
			if(strlen((char *)temp_data) != 0)
			{
				strcat (Line[bitCount], FieldStrings [bitCount]);
				strncat (Line[bitCount], (char *)AUTH_TX_V.TLF01_details.processing_code, 6);				
			}
			}
		break;
		
		case bf_4amount :
			if(BitFieldArray[bf_4amount] == 1){
			len=strlen((char *)AUTH_TX_V.TLF01_details.total_amount);
			strncpy((char *)temp_data,(char *)AUTH_TX_V.TLF01_details.total_amount,len);
			if(strlen((char *)temp_data) != 0)
			{
				strcat (Line[bitCount], FieldStrings [bitCount]);
				strncat (Line[bitCount], (char *)AUTH_TX_V.TLF01_details.total_amount, len);
			}
			}
		break;
		
		case bf_5amount :
			if( BitFieldArray[bf_5amount] == 1){
			if(CheckNotNull(HOST_MESSAGE.settle_amt, sizeof(HOST_MESSAGE.settle_amt)))    
			{
				bcd_to_asc (HOST_MESSAGE.settle_amt, sizeof(HOST_MESSAGE.settle_amt),
							local_buffer, 12);
				strcat (Line[bitCount], FieldStrings [bitCount]);
				strncat (Line[bitCount], local_buffer, strlen(local_buffer));
			}
			}
		break;
		
		case bf_6crdbillamt :
			if( BitFieldArray[bf_6crdbillamt] == 1){
			//if(strlen((char*)AUTH_TX_V.ch_billing_amt) != 0)
			//{
				bcd_to_asc (HOST_MESSAGE.card_bill_amt, sizeof(HOST_MESSAGE.card_bill_amt),
               (char *)temp_data,12);
				strcat (Line[bitCount], FieldStrings [bitCount]);
				strncat (Line[bitCount], (char*)AUTH_TX_V.ch_billing_amt ,strlen((char*)AUTH_TX_V.ch_billing_amt));
			}
			//}
			break ;

		case bf_7trntime :
			if(BitFieldArray[bf_7trntime] == 1){
			//if(strlen((char *)HOST_MESSAGE.gmt_time) != 0)
			//{
				strcat (Line[bitCount], FieldStrings [bitCount]);
				strncat (Line[bitCount], (char *)AUTH_TX_V.TLF01_details.transmission_timestamp ,10);
			//}
			}
		break;
		
		case bf_9convrate :
			if(BitFieldArray[bf_9convrate] == 1){
			if(CheckNotNull(HOST_MESSAGE.conv_rate_stl, sizeof(HOST_MESSAGE.conv_rate_stl)))    
			{
				bcd_to_asc (HOST_MESSAGE.conv_rate_stl, sizeof(HOST_MESSAGE.conv_rate_stl),
               (char *)temp_data,12);
				strcat (Line[bitCount], FieldStrings [bitCount]);
				strncat (Line[bitCount], (char *)temp_data,strlen((char *)temp_data));	
			}
			}
		break;
		
		case bf_10convrate :
			if( BitFieldArray[bf_10convrate] == 1){
			if(CheckNotNull(HOST_MESSAGE.conv_rate, sizeof(HOST_MESSAGE.conv_rate)))
			{
				bcd_to_asc (HOST_MESSAGE.conv_rate, sizeof(HOST_MESSAGE.conv_rate),
						(char *)temp_data,8);
				strcat (Line[bitCount], FieldStrings [bitCount]);
				strncat (Line[bitCount], (char *)temp_data ,8);
			}
			}
		break;

		case bf_11stan :
			if(BitFieldArray[bf_11stan] == 1){
			strncpy((char *)temp_data,(char *)AUTH_TX_V.TLF01_details.sys_trace_audit_num,6);
			if(strlen((char *)temp_data) != 0)
			{
				strcat (Line[bitCount], FieldStrings [bitCount]);
				strncat (Line[bitCount], (char *)temp_data, 6);
			}
			}
		break;
		
		case bf_12time :
			if(BitFieldArray[bf_12time] == 1){
			if(strlen((char *)AUTH_TX_V.TLF01_details.time_hhmmss) != 0)
			{
				strcat (Line[bitCount], FieldStrings [bitCount]);
				strncat (Line[bitCount], (char *)AUTH_TX_V.TLF01_details.time_hhmmss, 6);
			}
			}
		break ;

		case bf_13date :
			if(BitFieldArray[bf_13date] == 1){
			//if(strlen((char *)HOST_MESSAGE.date_bin) != 0)
			//{
				strcat (Line[bitCount], FieldStrings [bitCount]);
				strncat (Line[bitCount], (char *)AUTH_TX_V.TLF01_details.date_yyyymmdd+4, 4);
			//}
			}
		break ;

		case bf_14expiry :

			//if(strlen((char *)HOST_MESSAGE.date_expd) != 0)
			if(BitFieldArray[bf_14expiry] == 1)
			{
				strcat (Line[bitCount], FieldStrings [bitCount]);
				strncat (Line[bitCount], (char *)AUTH_TX_V.TLF01_details.exp_date, 4);
			}
		break ;

		case bf_15datestl :
			//if(strlen((char *)HOST_MESSAGE.date_settle) != 0)
			if(BitFieldArray[bf_15datestl] == 1)
			{
				strcat (Line[bitCount], FieldStrings [bitCount]);
				strncat (Line[bitCount], (char *)AUTH_TX_V.TLF01_details.settlement_date, 4);
			}
		break ;
		
	    case bf_16convdate :
			//if(strlen((char *)AUTH_TX_V.conv_date) != 0)
			if(BitFieldArray[bf_16convdate] == 1)
			{
				strcat (Line[bitCount], FieldStrings [bitCount]);
				strncat (Line[bitCount], (char *)AUTH_TX_V.conv_date, 4);
			}
		break ;

		case bf_18sicnumber :
			//if(strlen((char *)HOST_MESSAGE.sic_number) != 0)
			if(BitFieldArray[bf_18sicnumber] == 1)
			{
				strcat (Line[bitCount], FieldStrings [bitCount]);
				strncat (Line[bitCount], (char *)AUTH_TX_V.TLF01_details.category_code, 4);
			}
		break ;
		
		case bf_19ctrycd :
			if(BitFieldArray[bf_19ctrycd] == 1){
			if(CheckNotNull(HOST_MESSAGE.ctry_code, sizeof(HOST_MESSAGE.ctry_code)))
			{
				bcd_to_asc (HOST_MESSAGE.ctry_code, sizeof(HOST_MESSAGE.ctry_code),
               (char *)temp_data,3);
				strcat (Line[bitCount], FieldStrings [bitCount]);
				strncat (Line[bitCount], (char *)temp_data, 3);
			}
			}
		break ;
		
		case bf_20panctrycd :
			if(BitFieldArray[bf_20panctrycd] == 1){
			if(CheckNotNull(HOST_MESSAGE.pan_ctry_cd, sizeof(HOST_MESSAGE.pan_ctry_cd)))
			{
				bcd_to_asc (HOST_MESSAGE.pan_ctry_cd, sizeof(HOST_MESSAGE.pan_ctry_cd),
               (char *)temp_data,3);
				strcat (Line[bitCount], FieldStrings [bitCount]);
				strncat (Line[bitCount], (char *)temp_data, 3);
			}
			}
		break ;

		case bf_22pose :
			//if(strlen((char *)HOST_MESSAGE.posentry) != 0)
			if(BitFieldArray[bf_22pose] == 1)
			{
				strcat (Line[bitCount], FieldStrings [bitCount]);
				strncat (Line[bitCount], (char *)AUTH_TX_V.TLF01_details.pos_entry_mode, 4);
			}
		break ;
		
		case bf_23csn :
			if(BitFieldArray[bf_23csn] == 1){
			if(strlen((char *)AUTH_TX_V.host2_message_seq_nbr) > 0)
			{
				strcat (Line[bitCount], FieldStrings [bitCount]);
				strncat (Line[bitCount], (char *)AUTH_TX_V.host2_message_seq_nbr, 4);
			}
			}
		break ;

		case bf_25pocc :
			if(BitFieldArray[bf_25pocc] == 1){
			strncpy((char *)temp_data,(char *)AUTH_TX_V.TLF01_details.pos_condition_code,2);
			if(strlen((char *)temp_data) != 0)
			{
				strcat (Line[bitCount], FieldStrings [bitCount]);
				strncat (Line[bitCount], (char *)temp_data, 2);
			}
			}
		break ;
		

		case bf_26pincc :
			if(BitFieldArray[bf_26pincc] == 1){
			strncpy((char *)temp_data,(char *)AUTH_TX_V.pos_pin_capt_code,2);
			if(strlen((char *)HOST_MESSAGE.pin_captcd) != 0)
			{
					strcat (Line[bitCount], FieldStrings [bitCount]);
					strncat (Line[bitCount], (char *)temp_data , 2);
			}
			}
		break ;

		case bf_28feeamount :
			if(BitFieldArray[bf_28feeamount] == 1)
			//if(strlen((char *)HOST_MESSAGE.tran_fee_amount) != 0)
			{
					strcat (Line[bitCount], FieldStrings [bitCount]);
					strncat (Line[bitCount], (char *)HOST_MESSAGE.tran_fee_amount, 9);
			}

		break ;
		//tran_fee_amount
		
		case bf_32idcode :
			if(BitFieldArray[bf_32idcode] == 1){
			genutil_bin_to_ascii (HOST_MESSAGE.acq_inst_len, 1,temp_len, 2);
			len = genutil_hex_to_int(temp_len,strlen(temp_len));
			if(len != 0)
			{
					strcat (Line[bitCount], FieldStrings [bitCount]);
					strncat (Line[bitCount], (char *)AUTH_TX_V.TLF01_details.acquiring_id ,len);
			}
			}
		break ;
		
		case bf_33fwdidcode :
			if(BitFieldArray[bf_33fwdidcode] == 1){
			genutil_bin_to_ascii (HOST_MESSAGE.fwd_inst_len, 1,temp_len, 2);
			len = genutil_hex_to_int(temp_len,strlen(temp_len));
			if(len != 0)
			{
					strcat (Line[bitCount], FieldStrings [bitCount]);
					strncat (Line[bitCount], (char *)AUTH_TX_V.TLF01_details.forwarding_institution_id,len);
			}
			}
		break ;

		case bf_35track2 :
			if(BitFieldArray[bf_35track2] == 1){
			genutil_bin_to_ascii (HOST_MESSAGE.track2_len, 1,temp_len, 2);
			len = genutil_hex_to_int(temp_len,strlen(temp_len));
			if(len != 0)
			{
					strcat (Line[bitCount], FieldStrings [bitCount]);
					strncat (Line[bitCount], (char *)AUTH_TX_V.TLF01_details.track2 ,len);
			}
			}
		break ;

		case bf_37rrn :
			//if(strlen((char *)HOST_MESSAGE.rrn) != 0)
			if(BitFieldArray[bf_37rrn] == 1){
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
		
		case bf_43cardaccname :
			//if(strlen((char *)HOST_MESSAGE.cardaccname) != 0)
			if(BitFieldArray[bf_43cardaccname] == 1)
			{
					strcat (Line[bitCount], FieldStrings [bitCount]);
					strncat (Line[bitCount], (char *)HOST_MESSAGE.cardaccname,40);
			}
		break ;
		
		case bf_44respdata :
			//if(strlen((char *)HOST_MESSAGE.addl_rsp_len) != 0)
			if(BitFieldArray[bf_44respdata] == 1)
			{
					len = HOST_MESSAGE.addl_rsp_len[0];
					strcat (Line[bitCount], FieldStrings [bitCount]);
					strncat (Line[bitCount], (char *)HOST_MESSAGE.addlrspdata,len);
			}
		break ;
		
		case bf_45track1 :
			//if(strlen((char *)HOST_MESSAGE.track1_len) != 0)	
			if(BitFieldArray[bf_45track1] == 1)
			{
					strncpy(temp_len,(char *)HOST_MESSAGE.track1_len,2);
					len =atoi(temp_len);
					strcat (Line[bitCount], FieldStrings [bitCount]);
					strncat (Line[bitCount], (char *)HOST_MESSAGE.track1,len);
			}
		break ;

		case bf_48addl_data :
			//if(strlen((char *)HOST_MESSAGE.addl_data_len) != 0)
			if(BitFieldArray[bf_48addl_data] == 1)
			{
				/*if(0 == strncmp((char*)HOST_MESSAGE.msgtype_bcd,"0800",4))
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
				}*/
				char de48_buffer[256] = {0};
				ncjcb_parse_DE48(HOST_MESSAGE.addl_data_len[0], (char *)HOST_MESSAGE.addl_data, de48_buffer);
				strcat (Line[bitCount], FieldStrings [bitCount]);
				strncat (Line[bitCount], de48_buffer, strlen(de48_buffer));
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
		
		case bf_50sttlcurcd :
			//if(strlen((char *)HOST_MESSAGE.settle_cur_cd) != 0)
			if(BitFieldArray[bf_50sttlcurcd] == 1)
			{
					strcat (Line[bitCount], FieldStrings [bitCount]);
					strncat (Line[bitCount], (char *)HOST_MESSAGE.settle_cur_cd,3);
			}
		break ;
		
		case bf_51crdcurcd :
			//if(strlen((char *)HOST_MESSAGE.card_curr_cd) != 0)
			if(BitFieldArray[bf_51crdcurcd] == 1)
			{
					strcat (Line[bitCount], FieldStrings [bitCount]);
					strncat (Line[bitCount], (char *)HOST_MESSAGE.card_curr_cd,3);
			}
		break ;

		case bf_52pinblock :
			//if(strlen((char *)HOST_MESSAGE.pin_block) != 0)  
			if(BitFieldArray[bf_52pinblock] == 1)
			{
					strncpy((char *)temp_data,(char *)HOST_MESSAGE.pin_block,8);
					len = strlen((char *)temp_data);
					genutil_bin_to_ascii (temp_data, len,buffer, (len*2));
					strcat (Line[bitCount], FieldStrings [bitCount]);
					strncat (Line[bitCount], (char *)buffer,strlen(buffer));
			}
		break ;
		
		case bf_53seccntl :
			if(BitFieldArray[bf_53seccntl] == 1){
			if(CheckNotNull(HOST_MESSAGE.sec_cntl, sizeof(HOST_MESSAGE.sec_cntl)))	
			{
					bcd_to_asc (HOST_MESSAGE.sec_cntl, sizeof(HOST_MESSAGE.sec_cntl),
               (char *)temp_data,16);
					strcat (Line[bitCount], FieldStrings [bitCount]);
					strncat (Line[bitCount], (char *)temp_data,16);
			}
			}
		break ;
		
		case bf_54addlamts :
			//if(strlen((char *)HOST_MESSAGE.addl_len) != 0)	
			if(BitFieldArray[bf_54addlamts] == 1)
			{
				len = HOST_MESSAGE.addl_len[0];
				strcat (Line[bitCount], FieldStrings [bitCount]);
				strncat (Line[bitCount], (char*)HOST_MESSAGE.addlamts, len);
			}
		break ;
		
		case bf_55iccdata :
			//if(strlen((char *)HOST_MESSAGE.iccdata) != 0)	
			if(BitFieldArray[bf_55iccdata] == 1)
			{
					//strncpy(temp_len,(char *)HOST_MESSAGE.iccdata_len,3);
					//len =atoi(temp_len);
					int i;
					char itemp[5] = {0}, tempstr[101] = {0};
					char ICCdata[300] = {0};

				    i = HOST_MESSAGE.iccdata_len[1];
					sprintf( itemp, "%04d", i );
					genutil_asc_to_bcd( itemp, 4, (BYTE*)tempstr );

					/* Copy the BCD length into AUTH_TX's EMV Block */
					memcpy( AUTH_TX_V.EMV_details.emv_block, tempstr, 2 );

					/* Copy the rest of the data in, untouched. */
					memcpy( AUTH_TX_V.EMV_details.emv_block+2, HOST_MESSAGE.iccdata, i );

					//memcpy((char *)iccdata, (char *)HOST_MESSAGE.iccdata, i);
					//genutil_EMV_To_Auth_Tx( iccdata, len, &AUTH_TX_V );
					if ( i > 0 && i < 255 )
					{     
						memset(ICCdata, 0, sizeof(ICCdata));
						memcpy(ICCdata, HOST_MESSAGE.iccdata, i);
						JCB_EMV_To_Auth_Tx( ICCdata, i, &AUTH_TX_V);
					}

					memcpy(&EMV_details, &(AUTH_TX_V.EMV_details), sizeof(EMV_details));
					//Generate_CUP_EMV_Details();
					strcat (Line[bitCount], FieldStrings [bitCount]);
					strncat (Line[bitCount], (char*)iccdata, strlen((char*)iccdata));
			}
		break ;
		
		case bf_60priv60 :
			if(BitFieldArray[bf_60priv60] == 1){
			genutil_bin_to_ascii (HOST_MESSAGE.priv60_len, 1,temp_len, 3);
			len = genutil_hex_to_int(temp_len,strlen(temp_len));
			if(len != 0)				
			{
					strcat (Line[bitCount], FieldStrings [bitCount]);
					strncat (Line[bitCount], (char *)HOST_MESSAGE.priv60,len);
			}
			}
		break ;
		
		case bf_61priv61 :
			if(BitFieldArray[bf_61priv61] == 1){
			genutil_bin_to_ascii (HOST_MESSAGE.priv61_len, 1,temp_len, 3);
			len = genutil_hex_to_int(temp_len,strlen(temp_len));
			if(len != 0)
			{
					strcat (Line[bitCount], FieldStrings [bitCount]);
					strncat (Line[bitCount], (char *)HOST_MESSAGE.priv61,len);
			}
			}
		break ;
		
		case bf_62priv62 :
			if(BitFieldArray[bf_62priv62] == 1){
			genutil_bin_to_ascii (HOST_MESSAGE.priv62_len, 1,temp_len, 3);
			len = genutil_hex_to_int(temp_len,strlen(temp_len));
			if(len != 0)
			{
					strcat (Line[bitCount], FieldStrings [bitCount]);
					strncat (Line[bitCount], (char *)HOST_MESSAGE.priv62,len);
			}
			}
		break ;
		
		case bf_63priv63 :
			if(BitFieldArray[bf_63priv63] == 1){
			genutil_bin_to_ascii (HOST_MESSAGE.priv63_len, 1,temp_len, 3);
			len = genutil_hex_to_int(temp_len,strlen(temp_len));
			if(len != 0)			
			{
					strcat (Line[bitCount], FieldStrings [bitCount]);
					strncat (Line[bitCount], (char *)HOST_MESSAGE.priv63,len);
			}
			}
		break ;

		case bf_70nmicode :
			//if(strlen((char *)HOST_MESSAGE.nmi_code) != 0)
			if( BitFieldArray[bf_70nmicode] == 1)
			{
					strcat (Line[bitCount], FieldStrings [bitCount]);
					strncat (Line[bitCount], (char *)AUTH_TX_V.function_code+1,3);
			}
		break ;

		case bf_73action :
			if( BitFieldArray[bf_73action] == 1){
			if(CheckNotNull(HOST_MESSAGE.action, sizeof(HOST_MESSAGE.action)))				
			{
					 bcd_to_asc (HOST_MESSAGE.action,   sizeof(HOST_MESSAGE.action),
						(char *)temp_data, sizeof(temp_data));
					strcat (Line[bitCount], FieldStrings [bitCount]);
					strncat (Line[bitCount], (char *)temp_data,6);
			}
			}
		break ;

		case bf_90orgdata :
			if(BitFieldArray[bf_90orgdata] == 1){
			strncpy((char *)temp_data,(char *)AUTH_TX_V.TLF01_details.orig_message,4);
			strncat((char *)temp_data,(char *)AUTH_TX_V.TLF01_details.orig_sys_trace_num,6);
			strncat((char *)temp_data,(char *)AUTH_TX_V.orig_local_time_date,10);
			strncat((char *)temp_data,(char *)AUTH_TX_V.orig_acq_inst_id,11);
			strncat((char *)temp_data,(char *)AUTH_TX_V.amex_pos_entry_mode,11);
			if(strlen((char *)temp_data) != 0)			
			{		
					strcat (Line[bitCount], FieldStrings [bitCount]);
					strncat (Line[bitCount], (char *)temp_data,42);
			}
			}
		break ;
		
		case bf_91fileupdate :
			//if(strlen((char *)HOST_MESSAGE.file_update) != 0)
			if(BitFieldArray[bf_91fileupdate] == 1)
			{
					strcat (Line[bitCount], FieldStrings [bitCount]);
					strncat (Line[bitCount], (char *)HOST_MESSAGE.file_update,1);
			}
		break ;
		
		case bf_93rspind :
			if(strlen((char *)HOST_MESSAGE.resp_ind) != 0)			
			{
					strcat (Line[bitCount], FieldStrings [bitCount]);
					strncat (Line[bitCount], (char *)HOST_MESSAGE.resp_ind,5);
			}
		break ;

		case bf_95replamts :
			//if(strlen((char *)HOST_MESSAGE.repl_amts) != 0)
			if(BitFieldArray[bf_95replamts] == 1)
			{
					strcat (Line[bitCount], FieldStrings [bitCount]);
					strncat (Line[bitCount], (char *)HOST_MESSAGE.repl_amts,42);
			}
		break ;

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

		case bf_99stlinstid :
			if(BitFieldArray[bf_96msgsec] == 1){
			genutil_bin_to_ascii (HOST_MESSAGE.setl_inst_len, 1,temp_len, 3);
			len = genutil_hex_to_int(temp_len,strlen(temp_len));
			if(len != 0)			
			{
					bcd_to_asc (HOST_MESSAGE.setl_inst_id,   sizeof(HOST_MESSAGE.setl_inst_id),
						(char *)temp_data, sizeof(temp_data));
					strcat (Line[bitCount], FieldStrings [bitCount]);
					strncat (Line[bitCount], (char *)temp_data,len);
			}
			}
		break ;

		case bf_100rcvid :
			if(BitFieldArray[bf_100rcvid] == 1){
			genutil_bin_to_ascii (HOST_MESSAGE.recv_inst_len, 1,temp_len, 3);
			len = genutil_hex_to_int(temp_len,strlen(temp_len));
			if(len!= 0)		
			{
				bcd_to_asc (HOST_MESSAGE.recv_inst_id,   sizeof(HOST_MESSAGE.recv_inst_id),
						(char *)temp_data, sizeof(temp_data));
					strcat (Line[bitCount], FieldStrings [bitCount]);
					strncat (Line[bitCount], (char *)temp_data,len);
			}
			}
		break ;

		case bf_101fname :
			if(BitFieldArray[bf_101fname] == 1){
			genutil_bin_to_ascii (HOST_MESSAGE.fname_len, 1,temp_len, 3);
			len = genutil_hex_to_int(temp_len,strlen(temp_len));
			if(len != 0)		
			{
			 	    /*bcd_to_asc (HOST_MESSAGE.filename,   sizeof(HOST_MESSAGE.recv_inst_id),
						(char *)temp_data, sizeof(temp_data));*/
					strcat (Line[bitCount], FieldStrings [bitCount]);
					strncat (Line[bitCount], (char *)HOST_MESSAGE.filename, len);
			}
			}
		break ;
		
		case bf_102acct1 :
			if(BitFieldArray[bf_102acct1] == 1){
			genutil_bin_to_ascii (HOST_MESSAGE.acct_id_1_len, 1,temp_len, 3);
			len = genutil_hex_to_int(temp_len,strlen(temp_len));
			if(len != 0)		
			{
					strcat (Line[bitCount], FieldStrings [bitCount]);
					strncat (Line[bitCount], (char *)HOST_MESSAGE.acct_id_1,len);
			}
			}
		break ;

		case bf_103acct2 :
			if(BitFieldArray[bf_103acct2] == 1){
			genutil_bin_to_ascii (HOST_MESSAGE.acct_id_2_len, 1,temp_len, 3);
			len = genutil_hex_to_int(temp_len,strlen(temp_len));
			if(strlen((char *)HOST_MESSAGE.acct_id_2_len) != 0)		
			{
					strcat (Line[bitCount], FieldStrings [bitCount]);
					strncat (Line[bitCount], (char *)HOST_MESSAGE.acct_id_2,len);
			}
			}
		break ;

		case bf_104trandesc :
			//if(strlen((char *)HOST_MESSAGE.trandesc_len) != 0)
			if(BitFieldArray[bf_104trandesc] == 1)
			{
					strncpy(temp_len,(char *)HOST_MESSAGE.trandesc_len,3);
					len =atoi(temp_len);
					strcat (Line[bitCount], FieldStrings [bitCount]);
					strncat (Line[bitCount], (char *)HOST_MESSAGE.trandesc,len);
			}
		break ;

		case bf_105msgseccode :
			//if(strlen((char *)HOST_MESSAGE.msg_sec_cd) != 0)
			if(BitFieldArray[bf_105msgseccode] == 1)
			{
				    strncpy((char *)temp_data,(char *)HOST_MESSAGE.msg_sec_cd,16);
					len = strlen((char *)temp_data);
					genutil_bin_to_ascii (temp_data, len, buffer, (len*2));
					strcat (Line[bitCount], FieldStrings [bitCount]);
					strncat (Line[bitCount], buffer, strlen(buffer));
			}
		break ;

		case bf_118intractry :
			//if(strlen((char *)HOST_MESSAGE.intra_ctry_len) != 0)	
			if(BitFieldArray[bf_118intractry] == 1)
			{
					strncpy(temp_len,(char *)HOST_MESSAGE.intra_ctry_len,3);
					len =atoi(temp_len);
					strcat (Line[bitCount], FieldStrings [bitCount]);
					strncat (Line[bitCount], (char *)HOST_MESSAGE.intra_ctry,len);
			}
		break ;

		case bf_120record :
			if(BitFieldArray[bf_120record] == 1){
					len = HOST_MESSAGE.record_len[1];
					strcat (Line[bitCount], FieldStrings [bitCount]);
					strncat (Line[bitCount], (char *)HOST_MESSAGE.record,len);
			}
		break ;


		case bf_121issid :
			//if(strlen((char *)HOST_MESSAGE.iss_inst_len) != 0)
			if(BitFieldArray[bf_121issid] == 1)
			{
					strncpy(temp_len,(char *)HOST_MESSAGE.iss_inst_len,3);
					len =atoi(temp_len);
					strcat (Line[bitCount], FieldStrings [bitCount]);
					strncat (Line[bitCount], (char *)HOST_MESSAGE.iss_inst_id,len);
			}
		break ;

		case bf_122openuse :
			//if(strlen((char *)HOST_MESSAGE.open_use_len) != 0)
			if(BitFieldArray[bf_122openuse] == 1)
			{
					strncpy(temp_len,(char *)HOST_MESSAGE.open_use_len,3);
					len =atoi(temp_len);
					strcat (Line[bitCount], FieldStrings [bitCount]);
					strncat (Line[bitCount], (char *)HOST_MESSAGE.open_use,len);
			}
		break ;

		case bf_123addverf :
			//if(strlen((char *)HOST_MESSAGE.addr_verf_len) != 0)
			if(BitFieldArray[bf_123addverf] == 1)
			{
					strncpy(temp_len,(char *)HOST_MESSAGE.addr_verf_len,3);
					len =atoi(temp_len);
					strcat (Line[bitCount], FieldStrings [bitCount]);
					strncat (Line[bitCount], (char *)HOST_MESSAGE.addr_verf,len);
			}
		break ;

		case bf_124freeform :
			//if(strlen((char *)HOST_MESSAGE.free_text_len) != 0)
			if(BitFieldArray[bf_124freeform] == 1)
			{
					strncpy(temp_len,(char *)HOST_MESSAGE.free_text_len,3);
					len =atoi(temp_len);
					strcat (Line[bitCount], FieldStrings [bitCount]);
					strncat (Line[bitCount], (char *)HOST_MESSAGE.free_text,len);
			}
		break ;

		case bf_127accesscode :
			if(BitFieldArray[bf_127accesscode] == 1){
			if( HOST_MESSAGE.access_cd_len[0] != 0X00 ||
				HOST_MESSAGE.access_cd_len[1] != 0X00)		
			{
					len = HOST_MESSAGE.access_cd_len[1];
					strcat (Line[bitCount], FieldStrings [bitCount]);
					strncat (Line[bitCount], (char *)HOST_MESSAGE.access_cd,len);
			}
			}
		break ;
		}
	

		
		startCharacter++;
		LastDisplayBit = bitCount;
	
	}

		if(strlen((char *)HOST_MESSAGE.iccdata) != 0)
		{
			strcat (Line[bitCount++], "------------------------------------------------------------------------------------------------------------------");
			
		  sprintf (Line[bitCount++], "4	Version_Name		:	AGNS");
		  sprintf (Line[bitCount++], "2	Version_No.		:	0001");
		  if(strlen((char*)EMV_details.app_crypto) > 0)
			sprintf(Line[bitCount++],  "9F26	AppCryptogram		:	%s",(char*)EMV_details.app_crypto);
		  if(strlen((char*)EMV_details.crypto_info) > 0)
			sprintf(Line[bitCount++],  "9F27	CryptogramInfoData		:	%s",(char*)EMV_details.crypto_info);
		  if(strlen((char*)EMV_details.iad) > 0)
			sprintf(Line[bitCount++],  "9F10	IssuerAppData		:	%-033s",(char*)EMV_details.iad );  
		  if(strlen((char*)EMV_details.unpredictable_number) > 0)
			 sprintf(Line[bitCount++],  "9F37	UnpredictableNo		:	%s",(char*)EMV_details.unpredictable_number );
		  if(strlen((char*)EMV_details.atc) > 0)
			 sprintf(Line[bitCount++],  "9F36	AppTxnCounter		:	%s",(char*)EMV_details.atc );
		  if(strlen((char*)EMV_details.tvr) > 0)
			 sprintf(Line[bitCount++],  "95	TermVerifResults		:	%s",(char*)EMV_details.tvr);  
		  if(strlen((char*)EMV_details.tran_date) > 0)
			 sprintf(Line[bitCount++],  "9A	TxnDate			:	%s",(char*)EMV_details.tran_date); 
		  if(strlen((char*)EMV_details.tran_type) > 0)
			 sprintf(Line[bitCount++],  "9C	TxnType			:	%s",(char*)EMV_details.tran_type); 
		  if(strlen((char*)EMV_details.tran_amount) > 0)
			 sprintf(Line[bitCount++],  "9F02	AmountAuthNum		:	%s",(char*)EMV_details.tran_amount);   
		  if(strlen((char*)EMV_details.currency_code) > 0)
			 sprintf(Line[bitCount++],  "5F2A	TxnCurrencyCode		:	%s",(char*)EMV_details.currency_code);  
		  if(strlen((char*)EMV_details.interchange_profile) > 0)
			 sprintf(Line[bitCount++],  "82	AppInterchProfile		:	%s",(char*)EMV_details.interchange_profile);
		  if(strlen((char*)EMV_details.country_code) > 0)
			 sprintf(Line[bitCount++],  "9F1A	TerminalCountryCode	:	%s",(char*)EMV_details.country_code); 		  
		  if(strlen((char*)EMV_details.other_amount) > 0)
			 sprintf(Line[bitCount++],  "9F03	AmountOtherNum		:	%s",(char*)EMV_details.other_amount); 
		  if(strlen((char*)EMV_details.cvm_results) > 0)
			 sprintf(Line[bitCount++],  "9F34	CVM Results		:	%s",(char*)EMV_details.cvm_results);
		  if(strlen((char*)EMV_details.terminal_type) > 0)
			 sprintf(Line[bitCount++],  "9F35	Terminal Type		:	%s",(char*)EMV_details.terminal_type);
		  if(strlen((char*)EMV_details.version) > 0)
			 sprintf(Line[bitCount++],  "9F09	TAV Number		:	%s",(char*)EMV_details.version);
		  if(strlen((char*)EMV_details.terminal_capabilities) > 0)
			 sprintf(Line[bitCount++],  "9F33	Terminal Capabilities	:	%s",(char*)EMV_details.terminal_capabilities);
		  if(strlen((char*)EMV_details.serial_number) > 0)
			 sprintf(Line[bitCount++],  "9F1E	Serial Number		:	%s",(char*)EMV_details.serial_number);
		  if(strlen((char*)EMV_details.future_field1) > 0)
			 sprintf(Line[bitCount++],  "4F	ICC Application ID		:	%s",(char*)EMV_details.future_field1);
		  if(strlen((char*)EMV_details.tran_sequence_ctr) > 0)
			 sprintf(Line[bitCount++],  "9F41	Tran Seq Counter		:	%s",(char*)EMV_details.tran_sequence_ctr);
		  if(strlen((char*)EMV_details.app_id) > 0)
			 sprintf(Line[bitCount++],  "84	Dedicated File Name		:	%s",(char*)EMV_details.app_id);
		  if(strlen((char*)EMV_details.issuer_script_2) > 0)
			 sprintf(Line[bitCount++],  "9F6E	Device Information		:	%s",(char*)EMV_details.issuer_script_2);
		  if(strlen((char*)EMV_details.issuer_script_2) > 0 && EMV_details.issuer_script_2[9] != '\0')
			 sprintf(Line[bitCount++],  "9F7C	PD Data			:	%s",(char*)EMV_details.issuer_script_2+8);
		  if(strlen((char*)EMV_details.issuer_auth_data) > 0)
			 sprintf(Line[bitCount++],  "91	Iss Auth Data		:	%s",(char*)EMV_details.issuer_auth_data+1);
		  if(strlen((char*)EMV_details.issuer_script_1) > 0 && EMV_details.issuer_script_1[7] != '\0')
			 sprintf(Line[bitCount++],  "71	Iss Script Template1	:	%s",(char*)EMV_details.issuer_script_1+6);
		  if(strlen((char*)EMV_details.future_field2) > 0)
			 sprintf(Line[bitCount++],  "72	Iss Script Template2	:	%s",(char*)EMV_details.future_field2);
		  if(strlen((char*)EMV_details.pan_sequence_number) > 0)
			 sprintf(Line[bitCount++],  "5F34	PANSequenceNo		:	%s",(char*)EMV_details.pan_sequence_number);	  
		  if(strlen((char*)EMV_details.mcc) > 0)
			 sprintf(Line[bitCount++],  "9F15	MCC			:	%s",(char*)EMV_details.mcc);		    
		  if(strlen((char*)EMV_details.tcc) > 0)
			 sprintf(Line[bitCount++],  "9F53	TCC			:	%s",(char*)EMV_details.tcc);
		}
}

/*****************************************************************************

  Function:    hhutil_parse_host_response_msg

  Description: This function will parse a host response message
  Author: 
      unknown
  Inputs:
      p_auth_tx - a pointer to the auth_tx structure
      ReqRsp_Buffer(global) - the buffer that contains the ISO formatted message
  Outputs:
      HOST_MESSAGE (global) - structure that contains data to go into the 
                              ISO message
  Return values:
      true  - no error occurred
      false - an error occurred
  Modified by:
      FArad      8/13/98
      DI - 04/14/99 - Added EBCDIC message type
******************************************************************************/

INT hhutil_parse_host_response_msg(pAUTH_TX p_auth_tx)
{
   struct bitfield_data *pBF;
   INT                  bfidx;     /* index to bitfields for this transaction */
   INT                  i;

   memset(astr_bitmap,0x00,sizeof(astr_bitmap)-1);

   memset(&HOST_MESSAGE, 0x00, sizeof(HOST_MESSAGE));

   /* Extract the message type from the host message. */
   memcpy( HOST_MESSAGE.msgtype_bcd, &ReqRsp_Buffer[0],
           sizeof(HOST_MESSAGE.msgtype_bcd) );

   /* Convert Message Type from EBCDIC to ASCII. */
   for( i=0; i<4; i++ )
      HOST_MESSAGE.msgtype_bcd[i] = 
                        ascii_ebcdic_conv(HOST_MESSAGE.msgtype_bcd[i], E2A);

   bitmapidx = 4;       /* point to bitmap */

   /* Point past bitmap, but 1st check for extended bitmap. */
   if ( ReqRsp_Buffer[bitmapidx]  &  BITTAB[bf_1xbitfield].bit_mask )
   {
	   genutil_bin_to_ascii ((BYTE *)&ReqRsp_Buffer[4], 16, astr_bitmap, sizeof(astr_bitmap));
     reqidx = 20;                         /* Extended     */
      BitFieldSize = EXTENDED_BITFIELD;
   }
   else
   {
	   genutil_bin_to_ascii ((BYTE *)&ReqRsp_Buffer[4], 8, astr_bitmap, sizeof(astr_bitmap));
      reqidx = 12;                        /* Not Extended */
      BitFieldSize = NORMAL_BITFIELD;
   }

   /*------------------------------------------------------------------
      For every bitfield present in message ( except first bit ) . . .
   -------------------------------------------------------------------*/
   for (bfidx = 2; bfidx <= BitFieldSize; bfidx++)
   {
      if (ReqRsp_Buffer[bitmapidx+BITTAB[bfidx].byte_no] & BITTAB[bfidx].bit_mask)
      {
         /* Bit is on, field is there.... */
         pBF = Find_Bitfield_Table((bitfields)bfidx);       

         if (pBF != NULL)
         {
			 BitFieldArray[pBF->bd_fieldid] = 1;
            /*---------------------------------------*/
            /* call the Response Handling routine... */
            /*---------------------------------------*/
	   
            (pBF->bd_BFRsp)(pBF->bd_fieldid);
            if (true == invalid_msg_format)
            {
              // TxUtils_Send_Msg_To_Operator(1,
               //   "Invalid message format received. Partial message. Check message source",
                //  1, WARN_MSG, "hhutil_parse_host_response_msg", 2, WARNING_ERROR,
                 // NULL_PTR, NULL_PTR, NULL_PTR);
               return(false);
            }
         }
      }
   }

   return(true);
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
   CHAR   time_date[27];
   CHAR   tempstr [101];  
   pCHAR  tempptr;    
   INT    i;
   INT    length;
   CRF01  crf01;
   double conv_rate;
   INT    int_total_amount;
   double double_total_amount;
   LONG   returncode;
   CHAR   Buffer[256];
   CHAR  itemp[5];

   //memset(p_auth_tx, 0x00, Auth_Tx_Size);

   /* MESSAGE TYPE */
   memcpy( p_auth_tx->TLF01_details.message_type,
            HOST_MESSAGE.msgtype_bcd, sizeof(HOST_MESSAGE.msgtype_bcd ) );

   /* FIELD 2: PAN */
   //  This code is right after FIELD 35, don't move it back

   /* FIELD 3: PROCESSING CODE */
   if(BitFieldArray[bf_3proc] == 1){
   bcd_to_asc (HOST_MESSAGE.proccode,   sizeof(HOST_MESSAGE.proccode),
               (char *)p_auth_tx->TLF01_details.processing_code,
               sizeof(p_auth_tx->TLF01_details.processing_code));
   }

   /* FIELD 4: AMOUNT */
   if(BitFieldArray[bf_4amount] == 1){
   bcd_to_asc (HOST_MESSAGE.amount_bin, sizeof(HOST_MESSAGE.amount_bin),
               (char *)p_auth_tx->TLF01_details.total_amount,
               (sizeof(p_auth_tx->TLF01_details.total_amount)));

   /* Store amt here as original amount prior to currency conversion. SCR 565 */
   memcpy( p_auth_tx->TLF01_details.sales_amount,
           p_auth_tx->TLF01_details.total_amount, 12 );
   }

   /* FIELD 6: Cardholder billing amount J link*/
   if(BitFieldArray[bf_6crdbillamt] == 1){
   bcd_to_asc (HOST_MESSAGE.card_bill_amt, sizeof(HOST_MESSAGE.card_bill_amt),
		   	   (char *)&p_auth_tx->ch_billing_amt, (sizeof(p_auth_tx->ch_billing_amt)-1));
   }

   /* FIELD 7: GREENWICH TIME - MMDDhhmmss */
   if(BitFieldArray[bf_7trntime] == 1){
   bcd_to_asc (HOST_MESSAGE.gmt_time, sizeof(HOST_MESSAGE.gmt_time),
               (char *)&p_auth_tx->TLF01_details.transmission_timestamp,
               sizeof(p_auth_tx->TLF01_details.transmission_timestamp));
   }

   /* FIELD 11: SYSTEMS TRACE AUDIT NUMBER */
   if(BitFieldArray[bf_11stan] == 1){
   bcd_to_asc (HOST_MESSAGE.stan_bin, sizeof(HOST_MESSAGE.stan_bin),
               (char *)p_auth_tx->TLF01_details.sys_trace_audit_num,
               sizeof(p_auth_tx->TLF01_details.sys_trace_audit_num));
   }

   /* FIELD 12: LOCAL TRANSACTION TIME  */
    if(BitFieldArray[bf_12time] == 1){
   bcd_to_asc (HOST_MESSAGE.time_bin, sizeof(HOST_MESSAGE.time_bin),
               (char *)p_auth_tx->TLF01_details.time_hhmmss,
               sizeof(p_auth_tx->TLF01_details.time_hhmmss));
	}

   /*  FIELD 13: LOCAL TRANSACTION DATE  */
   if(BitFieldArray[bf_13date] == 1){
   memset(tempstr, 0, sizeof(tempstr));
   genutil_format_date (time_date);
   memcpy (p_auth_tx->TLF01_details.date_yyyymmdd, time_date, 4);  // get year
   bcd_to_asc (HOST_MESSAGE.date_bin, sizeof(HOST_MESSAGE.date_bin),
               tempstr, sizeof(p_auth_tx->TLF01_details.date_yyyymmdd));
   memcpy (p_auth_tx->TLF01_details.date_yyyymmdd+4, &tempstr, 4);
   }
   
   /* FIELD 14: DATE EXPIRED */
   // Moved to below FIELD 35
   
   /* FIELD 15: SETTLEMENT DATE (MMDD) */
   if(BitFieldArray[bf_15datestl] == 1){
   bcd_to_asc (HOST_MESSAGE.date_settle, sizeof(HOST_MESSAGE.date_settle),
               (char *)p_auth_tx->TLF01_details.settlement_date,
               sizeof(p_auth_tx->TLF01_details.settlement_date));
   }

   /* FIELD 16: Conversion date; present if Bit 6 is present. J link */
   if(BitFieldArray[bf_16convdate] == 1)
   {
	   bcd_to_asc (HOST_MESSAGE.conv_date, sizeof(HOST_MESSAGE.conv_date),
	               (char *)p_auth_tx->conv_date,
	               sizeof(p_auth_tx->conv_date)-1);
   }

   /* FIELD 18: SIC NUMBER (MERCHANT TYPE) */
   if(BitFieldArray[bf_18sicnumber] == 1){
   bcd_to_asc (HOST_MESSAGE.sic_number, sizeof(HOST_MESSAGE.sic_number),
               (char *)p_auth_tx->TLF01_details.category_code,
               sizeof(p_auth_tx->TLF01_details.category_code));
   }

   /* FIELD 22: POS ENTRY MODE  */
   if(BitFieldArray[bf_22pose] == 1){
   bcd_to_asc (HOST_MESSAGE.posentry,   sizeof(HOST_MESSAGE.posentry),
               (char *)p_auth_tx->TLF01_details.pos_entry_mode,
               sizeof(p_auth_tx->TLF01_details.pos_entry_mode));

   /*+-----------------------------------------------------------------+
     | Need to convert field 22 to the Hypercom Standard.              |
     |                                                                 |
     |   JCB matches the standard except filler is at the end for JCB. |
     |   For standard, filler should be at the beginning:              |
     |                                                                 |
     |   JCB = xyz0    convert to    Standard = 0xyz                   |
     +-----------------------------------------------------------------+*/
   for( i=2; i>=0; i-- )
      p_auth_tx->TLF01_details.pos_entry_mode[i+1] =
          p_auth_tx->TLF01_details.pos_entry_mode[i];
   p_auth_tx->TLF01_details.pos_entry_mode[0] = '0';
   }
   
   /* FIELD 23 : CARD SEQ NO HOST_MESSAGE.cardseqnum*/
   if(BitFieldArray[bf_23csn] == 1){
   if ( HOST_MESSAGE.card_seq_num[0] != 0x00 ||
   	    HOST_MESSAGE.card_seq_num[1] != 0x00)
   {
		bcd_to_asc (HOST_MESSAGE.card_seq_num, sizeof(HOST_MESSAGE.card_seq_num),
					(char *)p_auth_tx->host2_message_seq_nbr,
					sizeof(p_auth_tx->host2_message_seq_nbr));
   }
   }

   /* FIELD 25: POS CONDITION CODE */
   if(BitFieldArray[bf_25pocc] == 1){
   bcd_to_asc (HOST_MESSAGE.pos_condcd, sizeof(HOST_MESSAGE.pos_condcd),
               (char *)p_auth_tx->TLF01_details.pos_condition_code,
               sizeof(p_auth_tx->TLF01_details.pos_condition_code));
   }

   /* FIELD 26: POS PIN capture code */
   if(BitFieldArray[bf_26pincc] == 1){
   bcd_to_asc (HOST_MESSAGE.pin_captcd, sizeof(HOST_MESSAGE.pin_captcd),
               (char *)p_auth_tx->pos_pin_capt_code,
               sizeof(p_auth_tx->pos_pin_capt_code));
   }

   /* FIELD 32: ACQUIRING INSTITUTION ID CODE  */
    if(BitFieldArray[bf_32idcode] == 1){
   length = (HOST_MESSAGE.acq_inst_len[0]+1)/2;
   p_auth_tx->acq_inst_id_len[0] =  HOST_MESSAGE.acq_inst_len[0];
   if (length >= sizeof(p_auth_tx->acq_inst_id_cd)) 
      length = (sizeof(p_auth_tx->acq_inst_id_cd) - 1);
   bcd_to_asc( HOST_MESSAGE.acq_inst_id, length, (char *)p_auth_tx->acq_inst_id_cd,
               HOST_MESSAGE.acq_inst_len[0]);

   p_auth_tx->acq_inst_id_cd[HOST_MESSAGE.acq_inst_len[0]] = '\0';

   /* Store here for TLF01. */
   strcpy( (char *)p_auth_tx->TLF01_details.acquiring_id, (char *)p_auth_tx->acq_inst_id_cd );
	}

   /* FIELD 33: FORWARDING INSTITUTION ID CODE  */
   if(BitFieldArray[bf_33fwdidcode] == 1){
   length = (HOST_MESSAGE.fwd_inst_len[0]+1)/2;
   p_auth_tx->forwarding_inst_id_len[0] =  HOST_MESSAGE.fwd_inst_len[0];
   if (length >= sizeof(p_auth_tx->forwarding_inst_id_cd)) 
      length = (sizeof(p_auth_tx->forwarding_inst_id_cd) - 1);
   bcd_to_asc( HOST_MESSAGE.fwd_inst_id, length,
               (char *)p_auth_tx->forwarding_inst_id_cd,
               HOST_MESSAGE.fwd_inst_len[0]);

   p_auth_tx->forwarding_inst_id_cd[HOST_MESSAGE.fwd_inst_len[0]] = '\0';

   /* Store here for TLF01. */
   strcpy( (char *)p_auth_tx->TLF01_details.forwarding_institution_id,
           (char *)p_auth_tx->forwarding_inst_id_cd );
   }

   /* FIELD 35: TRACK 2 */
   if(BitFieldArray[bf_35track2] == 1){
   memset(tempstr, 0, sizeof(tempstr));
   bcd_to_asc (HOST_MESSAGE.track2, (HOST_MESSAGE.track2_len[0]+1)/2,   
               tempstr, sizeof(tempstr));
   if ( 1 == (HOST_MESSAGE.track2_len[0] % 2) )
   {
	   if (HOST_MESSAGE.track2_len[0] >= sizeof(p_auth_tx->TLF01_details.track2))
		  memcpy (p_auth_tx->TLF01_details.track2, tempstr+1,
                sizeof(p_auth_tx->TLF01_details.track2)-1);
	   else
		  memcpy (p_auth_tx->TLF01_details.track2, tempstr+1,
                HOST_MESSAGE.track2_len[0]);
   }
   else
   {
      if (HOST_MESSAGE.track2_len[0] >= sizeof(p_auth_tx->TLF01_details.track2))
         memcpy (p_auth_tx->TLF01_details.track2, tempstr,
                 sizeof(p_auth_tx->TLF01_details.track2)-1);
      else
         memcpy (p_auth_tx->TLF01_details.track2, tempstr,
                 HOST_MESSAGE.track2_len[0]);
   }

   if ((tempptr = strchr((char *)p_auth_tx->TLF01_details.track2, '=')) != 0x00)
   {
 	   memcpy( p_auth_tx->TLF01_details.exp_date, tempptr+1,
              sizeof(p_auth_tx->TLF01_details.exp_date)-1);
      i = (((int)tempptr) - ((int)p_auth_tx->TLF01_details.track2));
      memcpy (p_auth_tx->TLF01_details.card_num,
              p_auth_tx->TLF01_details.track2, i);
      itoa((i), (char *)p_auth_tx->TLF01_details.card_num_len,  10);
   }
   }
   /* FIELD 2: PAN */
   // If the card number came in, use it instead of track2
   //if (0 != strlen((char *)HOST_MESSAGE.pan_bin))
   if(BitFieldArray[bf_2pan] == 1)
   {
      memset (p_auth_tx->TLF01_details.card_num_len, 0,
              sizeof(p_auth_tx->TLF01_details.card_num_len));
      memset (p_auth_tx->TLF01_details.card_num, 0,
              sizeof(p_auth_tx->TLF01_details.card_num));
      memset(tempstr, 0x00, sizeof(tempstr));
      bcd_to_asc( HOST_MESSAGE.pan_bin, (HOST_MESSAGE.pan_len[0]+1)/2, tempstr,
                  sizeof(p_auth_tx->TLF01_details.card_num) );
      itoa((HOST_MESSAGE.pan_len[0]),(char *)p_auth_tx->TLF01_details.card_num_len,10);
      if ( 1 == (HOST_MESSAGE.pan_len[0] % 2) )
      {
         memcpy( p_auth_tx->TLF01_details.card_num, tempstr+1,
                 sizeof(p_auth_tx->TLF01_details.card_num) );
      }
      else
      {
         memcpy( p_auth_tx->TLF01_details.card_num, tempstr,
                 sizeof(p_auth_tx->TLF01_details.card_num) );
      }
   }
   
   /* FIELD 14 EXPIRATION DATE  */
   //if ( HOST_MESSAGE.date_expd[0] != 0x00 )
   if(BitFieldArray[bf_14expiry] ==1)
   {
      /* If the expiration date came in, use it instead of track2. */
      memset( tempstr, 0x00, sizeof(tempstr) );
      bcd_to_asc( HOST_MESSAGE.date_expd,
                  sizeof(HOST_MESSAGE.date_expd),
                  tempstr, sizeof(tempstr) );

      memcpy( p_auth_tx->TLF01_details.exp_date, tempstr,
              sizeof(p_auth_tx->TLF01_details.exp_date) );
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
   memcpy( p_auth_tx->TLF01_details.response_code, HOST_MESSAGE.responsecd,
           sizeof(HOST_MESSAGE.responsecd));
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


   /* FIELD 48: ADDITIONAL DATA - NEED THE CVC STATUS FLAG AND CAV2. */
   ////  CVC flag is in 44, what do we need out of this???
   if(BitFieldArray[bf_48addl_data] == 1){
   find_cvc_flag( (char *)HOST_MESSAGE.addl_data_len, (char *)HOST_MESSAGE.addl_data,
                  (char *)&p_auth_tx->TLF01_details.cvc);

   find_cav2( HOST_MESSAGE.addl_data_len[0], (char *)HOST_MESSAGE.addl_data,
             (char *) p_auth_tx->TLF01_details.cvc_data );

   //ncjcb_parse_DE48(HOST_MESSAGE.addl_data_len[0], (char *)HOST_MESSAGE.addl_data, p_auth_tx);
   }

   /* FIELD 49 TRANSACTION CURRENCY CODE */
   if(BitFieldArray[bf_49trncurcd] == 1){
	memcpy (&p_auth_tx->TLF01_details.currency_code, &HOST_MESSAGE.tran_curr_cd,
           sizeof(HOST_MESSAGE.tran_curr_cd));

   /* Store currency here as original, prior to currency conversion. SCR 565 */
   memcpy( p_auth_tx->TLF01_details.nfi_seq_nbr,
           p_auth_tx->TLF01_details.currency_code, 3 );
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

   /* FIELD 55: Integrated Circuit Card (ICC) System-related Data */
   if(BitFieldArray[bf_55iccdata] == 1){
   if (false == blank_or_null_string((char *)HOST_MESSAGE.iccdata,
                                     sizeof(HOST_MESSAGE.iccdata)))
   {
      /* Convert length to BCD. */
      memset( itemp,   0x00, sizeof(itemp)   );
      memset( tempstr, 0x00, sizeof(tempstr) );

      i = HOST_MESSAGE.iccdata_len[1];
      sprintf( itemp, "%04d", i );
      genutil_asc_to_bcd( itemp, 4, (BYTE *)tempstr );

      /* Copy the BCD length into AUTH_TX's EMV Block */
      memcpy( p_auth_tx->EMV_details.emv_block, tempstr, 2 );

      /* Copy the rest of the data in, untouched. */
      memcpy( p_auth_tx->EMV_details.emv_block+2,
              HOST_MESSAGE.iccdata, i );
   }
   }

   /* FIELD 63: BANKNET DATA */

   /* This field is used for 0302 File Updates.  But
    * no need to get it.  We do not store it, nor do
    * we used it.  It is echoed from the request.
    */
	
   /* FIELD 70: NETWORK MANAGEMENT INFORMATION CODE */
   if( BitFieldArray[bf_70nmicode] == 1){
   bcd_to_asc (HOST_MESSAGE.nmi_code,   sizeof(HOST_MESSAGE.nmi_code),
               nmi_code, sizeof(nmi_code));
   memcpy (p_auth_tx->function_code, nmi_code, sizeof(nmi_code));
   }

   /* FIELD 90 ORIGINAL DATA ELEMENTS */
   if(BitFieldArray[bf_90orgdata] == 1){
   memset(tempstr, 0, sizeof(tempstr));
   if(strlen((char*)HOST_MESSAGE.orig_data) > 0)
   {
	   bcd_to_asc (HOST_MESSAGE.orig_data,   sizeof(HOST_MESSAGE.orig_data),
				   tempstr, sizeof(tempstr));
	   memcpy(p_auth_tx->TLF01_details.orig_message, tempstr, 4);
	   memcpy(p_auth_tx->TLF01_details.orig_sys_trace_num, tempstr+4, 6);
	   memcpy(p_auth_tx->orig_local_time_date, tempstr+10,10);
	   memcpy(p_auth_tx->orig_acq_inst_id,tempstr+20,11);
   }
   // Actually forwarding institution id code
   memcpy(p_auth_tx->amex_pos_entry_mode,tempstr+31,11);
   }

   /* FIELD 95 REPLACEMENT AMOUNTS */
   // moved to the end of this function

   /* FIELD 96 MESSAGE SECURITY CODE */
   ///  In the future, this will contain the new encryption key.

   /* FIELD 100: RECEIVING INSTITUTION ID CODE  */
   if(BitFieldArray[bf_100rcvid] == 1){
   memset(tempstr, 0, sizeof(tempstr));
   bcd_to_asc (HOST_MESSAGE.recv_inst_id, (HOST_MESSAGE.recv_inst_len[0]+1)/2,
				  &tempstr[0], sizeof(tempstr));
	memcpy(p_auth_tx->receiving_inst_id_code, tempstr,
          sizeof(p_auth_tx->receiving_inst_id_code)-1);
   }

   /* FIELD 101: File Name for 0302 Txns */

   /* This field is used for 0302 File Updates.  But
    * no need to get it.  We do not store it, nor do
    * we used it.  It is echoed from the request.
    */

   /* FIELD 120: Record Data */
   //if(BitFieldArray[bf_120record] == 1){
   length = HOST_MESSAGE.record_len[1];
   if ( length > 0 )
   {
      /*+---------------------------------------------------+
        | Field 120 is record data for Online File Updates. |
        | Format is:                                        |
        |    Transaction Type ( 1 byte ) (Mandatory)        |
        |    Card Number      (16 bytes) (Mandatory)        |
        |    Action Code      ( 2 bytes) (Conditional)      |
        |    Purge Date       ( 6 bytes) (Conditional)      |
        |    Stop Regions     ( 5 bytes) (Conditional)      |
        |                                                   |
        | This order must be preserved.  In other words, if |
        | you have stop regions, you must have purge date   |
        | and action code.  If you have purge date, you     |
        | must have action code.                            |
        +---------------------------------------------------+*/

      memset( tempstr, 0x00, sizeof(tempstr) );
      memcpy( tempstr, HOST_MESSAGE.record, length );

      /* Transaction Type */
      p_auth_tx->TLF01_details.processing_code[0] = tempstr[0];

      /* Card Number */
      memcpy( p_auth_tx->TLF01_details.card_num, tempstr+1, 16 );

      /* Action Code */
      memcpy( p_auth_tx->TLF01_details.type_of_data, tempstr+17, 2 );

      /* Purge Date */
      memcpy( p_auth_tx->TLF01_details.product_codes[0].amount, tempstr+19, 6);

      /* Stop Regions */
      memcpy( p_auth_tx->TLF01_details.product_codes[1].amount, tempstr+25, 5);
   }


   // Save this information to output to the host (our system needs it in PESOs, not USD)
   memcpy(p_auth_tx->reconciliation_cur_code,p_auth_tx->TLF01_details.currency_code, sizeof(p_auth_tx->TLF01_details.currency_code));
   memcpy(p_auth_tx->reconciliation_amount,p_auth_tx->TLF01_details.total_amount, sizeof(p_auth_tx->TLF01_details.total_amount));

   /*+------------------------------------------------------------------+
   | Here is a requirement unique to Equitable:                      
   |
   | ------------------------------------------                      
   |
   |   Field 49 must be "840" (USD).  Then use field 4 for the amount 
   |   field to convert to PESOs, and use the PESO Balance Account for 
   |   authorization purposes.  
   |
   | This is for switched in, or Issuing, transactions only.         
   |
   +------------------------------------------------------------------+*/
   // store the original total_amount and currency code
   if ((0 == strncmp((char *)p_auth_tx->TLF01_details.message_type, "0100", 4)) ||
       (0 == strncmp((char *)p_auth_tx->TLF01_details.message_type, "0120", 4)) ||
       (0 == strncmp((char *)p_auth_tx->TLF01_details.message_type, "0121", 4)) ||
       (0 == strncmp((char *)p_auth_tx->TLF01_details.message_type, "0420", 4)) ||
       (0 == strncmp((char *)p_auth_tx->TLF01_details.message_type, "0421", 4)) ||
       (0 == strncmp((char *)p_auth_tx->TLF01_details.message_type, "0620", 4)))
   {
      if ( 0 == strcmp( USD,(char *) p_auth_tx->TLF01_details.currency_code ) ) 
      {
         strcpy( (char *)crf01.primary_key.currency_code, PESO);// convert the total_amount from USD to PESOs

         /* Get conversion value to convert from USD to PESOs */
     //    returncode = FindMemRecord( (pBYTE)&crf01,
                         //            sizeof(crf01.primary_key.currency_code),
                  //                   sizeof(crf01),
                              //       "Crf01Table");
        // if ( returncode < 0 )
       //  {
            /* There is either a problem with shared memory
             * or the record is missing.  Go to the database.
             */
          //  returncode = get_conv_rate_from_database( &crf01, p_auth_tx );
       //  }

        // if (returncode >= 0)
         {
            // currency code found in CRF01 table
            strcpy( (char *)p_auth_tx->TLF01_details.currency_code, PESO );
            strcpy( (char *)p_auth_tx->TLF01_details.conversion_rate, (char *)crf01.conversion_rate);
            conv_rate = strtod((char *)crf01.conversion_rate, 0);
            if (conv_rate <= 0)
            {
               sprintf( Buffer,
                       "Conversion rate is 0 for currency code: %s",
                        p_auth_tx->TLF01_details.conversion_rate );
               memcpy( p_auth_tx->TLF01_details.response_text, Buffer, 40 );
             //  TxUtils_Send_Msg_To_Operator (1, Buffer, 1, ERROR_MSG, 
                //                            "move_iso_response_to_auth_struct",
                   //                          4, FATAL_ERROR, NULL_PTR,
                       //                      NULL_PTR, NULL_PTR);
               return(false);
            }

            int_total_amount = atoi((char *)p_auth_tx->TLF01_details.total_amount);
            double_total_amount = ((double)int_total_amount*conv_rate);
            int_total_amount = (int)double_total_amount;
            itoa(int_total_amount, (char *)p_auth_tx->TLF01_details.total_amount, 10);
            Rj_with_lead_zeros((char *)p_auth_tx->TLF01_details.total_amount, 12);
            if ((0 == strncmp((char *)p_auth_tx->TLF01_details.message_type, "0420", 4)) ||
               (0 == strncmp((char *)p_auth_tx->TLF01_details.message_type, "0421", 4)))
            {
               strncpy ((char *)p_auth_tx->TLF01_details.actual_amount,(char *) p_auth_tx->TLF01_details.total_amount, sizeof(p_auth_tx->TLF01_details.actual_amount));

       /* FIELD 95 REPLACEMENT AMOUNTS */
               if (0 != strlen((char *)HOST_MESSAGE.repl_amts))
               {
                  //strncpy (p_auth_tx->TLF01_details.total_amount, HOST_MESSAGE.repl_amts, 12);
                  strncpy((char *)p_auth_tx->TLF01_details.reversal_amount,(char *)HOST_MESSAGE.repl_amts, 12);

                  if( atoi((char *)p_auth_tx->TLF01_details.reversal_amount) != 0)
                  {
                      //Partial Reversal

                     /* Save the partial reversal amount into a field that does
                      * not get modified.
                      */
                     memcpy( p_auth_tx->TLF01_details.refund_amount,
                             p_auth_tx->TLF01_details.reversal_amount, 12 );

                      strncpy ((char *)p_auth_tx->TLF01_details.orig_amount, (char *)p_auth_tx->TLF01_details.total_amount, sizeof(p_auth_tx->TLF01_details.total_amount));
                      int_total_amount = atoi((char *)p_auth_tx->TLF01_details.reversal_amount);
                      double_total_amount = ((double)int_total_amount*conv_rate);
                      int_total_amount = (int)double_total_amount;
                      itoa(int_total_amount, (char *)p_auth_tx->TLF01_details.reversal_amount, 10);
                      Rj_with_lead_zeros((char *)p_auth_tx->TLF01_details.reversal_amount, 12);

                  }
                  else
                  {
                      //Full Reversal
                     strncpy ((char *)p_auth_tx->TLF01_details.reversal_amount, (char *)p_auth_tx->TLF01_details.total_amount, sizeof(p_auth_tx->TLF01_details.total_amount));

                     /* Save the reversal amount into a field that does
                      * not get modified.
                      */
                     memcpy( p_auth_tx->TLF01_details.refund_amount,
                             p_auth_tx->TLF01_details.reversal_amount, 12 );
                  }
               }
            }
         }
      //   else if (-1L == returncode)
         {
            sprintf( tempstr,
                    "Currency conversion (CRF01) record not found for currency %s",
                     PESO );
            memcpy( p_auth_tx->TLF01_details.response_text, tempstr, 40 );
           // TxUtils_Send_Msg_To_Operator (1, tempstr, 1, ERROR_MSG, 
             //           "move_iso_response_to_auth_struct", 4, FATAL_ERROR,
               //          NULL_PTR, NULL_PTR, NULL_PTR);
         }
      }
      else
      {
         strncpy((char *)p_auth_tx->TLF01_details.response_code, "91", 2);  // This is used to determine if the currency code is ok
      }

   }

   if ((0 == strncmp((char *)p_auth_tx->TLF01_details.message_type, "0420", 4)) ||
      (0 == strncmp((char *)p_auth_tx->TLF01_details.message_type, "0421", 4)))
   {
     /*
      total_int = atoi(p_auth_tx->TLF01_details.total_amount);
      actual_int = atoi(p_auth_tx->TLF01_details.actual_amount);
      reversal_int = total_int - actual_int;
      itoa(reversal_int, p_auth_tx->TLF01_details.reversal_amount,10);
      Rj_with_lead_zeros(p_auth_tx->TLF01_details.reversal_amount, 12);
      */
   }

//   if (false == verify_auth_tx_data(p_auth_tx))
   //   return(false);

   return(true);

} /* move_iso_response_to_auth_struct */
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
	CHAR nmi_code[5];

	if(strlen(temp) == 0)
	{
		m_parsedmssg.Empty();
		exportbtn.EnableWindow(FALSE);
	}
	else
	{
		exportbtn.EnableWindow(TRUE);
		len = m_flowfile.GetLength();
		buflen = len;
		temp_string = (LPBYTE)m_flowfile.GetBuffer(len);
		ascii_to_bin(temp_string, ReqRsp_Buffer, len);
		m_flowfile.ReleaseBuffer(len);
	/*	memset(&AUTH_TX_V,0x00,sizeof(AUTH_TX_V));
		memset(&EMV_details,0x00,sizeof(EMV_details));*/
		if ( hhutil_parse_host_response_msg(&AUTH_TX_V) == 0)
		{
			m_parsedmssg = "Cannot Parse the message";
			UpdateData(FALSE); //transfer the data from variables back to control.
		}
		else
		{
			memset(&AUTH_TX_V, 0, sizeof(AUTH_TX_V));
			move_iso_response_to_auth_struct(nmi_code, &AUTH_TX_V);
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
void JCB_EMV_To_Auth_Tx( pCHAR emv_raw, INT total_len, pAUTH_TX p_auth_tx )
{
   INT    fieldLength;
   INT    dataIndex;
   INT    bTwoBytesTag;
   BYTE   byteOneOfTag;
   BYTE   byteTwoOfTag;
   BYTE   b_string[4]  = {0};
   CHAR   a_string[4]  = {0};
   CHAR   tempstr[500] = {0};

   memset(&iccdata, 0, sizeof(iccdata));
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
            if (byteTwoOfTag == 0x2A)
            {
               genutil_bin_to_ascii ((BYTE*)&emv_raw[dataIndex], fieldLength , &tempstr[0], sizeof(tempstr));
               memcpy(p_auth_tx->EMV_details.currency_code, tempstr, sizeof(p_auth_tx->EMV_details.currency_code)-1);
               dataIndex += fieldLength;

			   strncat((char*)iccdata,"5F2A",4);
			   strncat((char*)iccdata, (char*)p_auth_tx->EMV_details.currency_code, sizeof(p_auth_tx->EMV_details.currency_code)-1);
            }
            else if (byteTwoOfTag == 0x34)
            {
               genutil_bcd_to_asc ((BYTE*)&emv_raw[dataIndex], fieldLength , &tempstr[0], sizeof(tempstr));
               memcpy(p_auth_tx->EMV_details.pan_sequence_number, tempstr, sizeof(p_auth_tx->EMV_details.pan_sequence_number)-1);
               dataIndex += fieldLength;

			   strncat((char*)iccdata,"5F34",4);
			   strncat((char*)iccdata, (char*)p_auth_tx->EMV_details.pan_sequence_number, sizeof(p_auth_tx->EMV_details.pan_sequence_number)-1);
            }
         }
         else if (byteOneOfTag == 0x9F)
         {
            switch (byteTwoOfTag)
            {
               case 0x02:
                        genutil_bcd_to_asc ((BYTE*)&emv_raw[dataIndex], fieldLength , &tempstr[0], sizeof(tempstr));
                        memcpy(p_auth_tx->EMV_details.tran_amount, tempstr, sizeof(p_auth_tx->EMV_details.tran_amount)-1);
						strncat((char*)iccdata,"9F02",4);
						strncat((char*)iccdata, (char*)p_auth_tx->EMV_details.tran_amount, sizeof(p_auth_tx->EMV_details.tran_amount)-1);
                        break;
               case 0x03:
                        genutil_bcd_to_asc ((BYTE*)&emv_raw[dataIndex], fieldLength , &tempstr[0], sizeof(tempstr));
                        memcpy(p_auth_tx->EMV_details.other_amount, tempstr, sizeof(p_auth_tx->EMV_details.other_amount)-1);
						strncat((char*)iccdata,"9F03",4);
						strncat((char*)iccdata, (char*)p_auth_tx->EMV_details.other_amount, sizeof(p_auth_tx->EMV_details.other_amount)-1);
                        break;
               case 0x09:
                        genutil_bin_to_ascii ((BYTE*)&emv_raw[dataIndex], fieldLength , &tempstr[0], sizeof(tempstr));
                        memcpy(p_auth_tx->EMV_details.version, tempstr, sizeof(p_auth_tx->EMV_details.version)-1);
						strncat((char*)iccdata,"9F09",4);
						strncat((char*)iccdata, (char*)p_auth_tx->EMV_details.version, sizeof(p_auth_tx->EMV_details.version)-1);
                        break;
               case 0x10:
                        genutil_bin_to_ascii ((BYTE*)&emv_raw[dataIndex], fieldLength , &tempstr[0], sizeof(tempstr));
                        memcpy(p_auth_tx->EMV_details.iad, tempstr, sizeof(p_auth_tx->EMV_details.iad)-1);
						strncat((char*)iccdata,"9F10",4);
						strncat((char*)iccdata, (char*)p_auth_tx->EMV_details.iad, sizeof(p_auth_tx->EMV_details.iad)-1);
                        break;
               case 0x1A:
                        genutil_bcd_to_asc ((BYTE*)&emv_raw[dataIndex], fieldLength , &tempstr[0], sizeof(tempstr));
                        memcpy(p_auth_tx->EMV_details.country_code, tempstr+1, sizeof(p_auth_tx->EMV_details.country_code)-1);
						strncat((char*)iccdata,"9F1A",4);
						strncat((char*)iccdata, (char*)p_auth_tx->EMV_details.country_code, sizeof(p_auth_tx->EMV_details.country_code)-1);
                        break;
               case 0x15:
                        genutil_bcd_to_asc ((BYTE*)&emv_raw[dataIndex], fieldLength , &tempstr[0], sizeof(tempstr));
                        memcpy(p_auth_tx->EMV_details.mcc, tempstr, sizeof(p_auth_tx->EMV_details.mcc)-1);
						strncat((char*)iccdata,"9F15",4);
						strncat((char*)iccdata, (char*)p_auth_tx->EMV_details.mcc, sizeof(p_auth_tx->EMV_details.mcc)-1);
                        break;
               case 0x1E:
						genutil_bin_to_ascii ((BYTE*)&emv_raw[dataIndex], fieldLength , &tempstr[0], sizeof(tempstr));	
                        memcpy(p_auth_tx->EMV_details.serial_number, tempstr, sizeof(p_auth_tx->EMV_details.serial_number)-1);
						strncat((char*)iccdata,"9F1E",4);
						strncat((char*)iccdata, (char*)p_auth_tx->EMV_details.serial_number, sizeof(p_auth_tx->EMV_details.serial_number)-1);
                        break;
               case 0x26:
                        genutil_bin_to_ascii ((BYTE*)&emv_raw[dataIndex], fieldLength , &tempstr[0], sizeof(tempstr));
                        memcpy(p_auth_tx->EMV_details.app_crypto, tempstr, sizeof(p_auth_tx->EMV_details.app_crypto)-1);
						strncat((char*)iccdata,"9F26",4);
						strncat((char*)iccdata, (char*)p_auth_tx->EMV_details.app_crypto, sizeof(p_auth_tx->EMV_details.app_crypto)-1);
                        break;
               case 0x27:
                        genutil_bin_to_ascii ((BYTE*)&emv_raw[dataIndex], fieldLength , &tempstr[0], sizeof(tempstr));
                        memcpy(p_auth_tx->EMV_details.crypto_info, tempstr, sizeof(p_auth_tx->EMV_details.crypto_info)-1);
						strncat((char*)iccdata,"9F27",4);
						strncat((char*)iccdata, (char*)p_auth_tx->EMV_details.crypto_info, sizeof(p_auth_tx->EMV_details.crypto_info)-1);
                        break;
               case 0x33:
                        genutil_bin_to_ascii ((BYTE*)&emv_raw[dataIndex], fieldLength , &tempstr[0], sizeof(tempstr));
                        memcpy(p_auth_tx->EMV_details.terminal_capabilities, tempstr, sizeof(p_auth_tx->EMV_details.terminal_capabilities)-1);
						strncat((char*)iccdata,"9F33",4);
						strncat((char*)iccdata, (char*)p_auth_tx->EMV_details.terminal_capabilities, sizeof(p_auth_tx->EMV_details.terminal_capabilities)-1);
                        break;
               case 0x34:
                        genutil_bin_to_ascii ((BYTE*)&emv_raw[dataIndex], fieldLength , &tempstr[0], sizeof(tempstr));
                        memcpy(p_auth_tx->EMV_details.cvm_results, tempstr, sizeof(p_auth_tx->EMV_details.cvm_results)-1);
						strncat((char*)iccdata,"9F34",4);
						strncat((char*)iccdata, (char*)p_auth_tx->EMV_details.cvm_results, sizeof(p_auth_tx->EMV_details.cvm_results)-1);
                        break;
               case 0x35:
                        genutil_bcd_to_asc ((BYTE*)&emv_raw[dataIndex], fieldLength , &tempstr[0], sizeof(tempstr));
                        memcpy(p_auth_tx->EMV_details.terminal_type, tempstr, sizeof(p_auth_tx->EMV_details.terminal_type)-1);
						strncat((char*)iccdata,"9F35",4);
						strncat((char*)iccdata, (char*)p_auth_tx->EMV_details.terminal_type, sizeof(p_auth_tx->EMV_details.terminal_type)-1);
                        break;
               case 0x36:
                        genutil_bin_to_ascii ((BYTE*)&emv_raw[dataIndex], fieldLength , &tempstr[0], sizeof(tempstr));
                        memcpy(p_auth_tx->EMV_details.atc, tempstr, sizeof(p_auth_tx->EMV_details.atc)-1);
						strncat((char*)iccdata,"9F36",4);
						strncat((char*)iccdata, (char*)p_auth_tx->EMV_details.atc, sizeof(p_auth_tx->EMV_details.atc)-1);
                        break;
               case 0x37:
                        genutil_bin_to_ascii ((BYTE*)&emv_raw[dataIndex], fieldLength , &tempstr[0], sizeof(tempstr));
                        memcpy(p_auth_tx->EMV_details.unpredictable_number, tempstr, sizeof(p_auth_tx->EMV_details.unpredictable_number)-1);
						strncat((char*)iccdata,"9F37",4);
						strncat((char*)iccdata, (char*)p_auth_tx->EMV_details.unpredictable_number, sizeof(p_auth_tx->EMV_details.unpredictable_number)-1);
                        break;
               case 0x41:
                        genutil_bcd_to_asc ((BYTE*)&emv_raw[dataIndex], fieldLength , &tempstr[0], sizeof(tempstr));
                        memcpy(p_auth_tx->EMV_details.tran_sequence_ctr, tempstr, sizeof(p_auth_tx->EMV_details.tran_sequence_ctr)-1);
						strncat((char*)iccdata,"9F41",4);
						strncat((char*)iccdata, (char*)p_auth_tx->EMV_details.tran_sequence_ctr, sizeof(p_auth_tx->EMV_details.tran_sequence_ctr)-1);
                        break;
               case 0x53:
						genutil_bcd_to_asc ((BYTE*)&emv_raw[dataIndex], fieldLength , &tempstr[0], sizeof(tempstr));
                        memcpy(p_auth_tx->EMV_details.tcc, tempstr, sizeof(p_auth_tx->EMV_details.tcc)-1);
						strncat((char*)iccdata,"9F53",4);
						strncat((char*)iccdata, (char*)p_auth_tx->EMV_details.tcc, sizeof(p_auth_tx->EMV_details.tcc)-1);
                        break;
               case 0x6E:
            	   	    genutil_bin_to_ascii ((BYTE*)&emv_raw[dataIndex], fieldLength , &tempstr[0], sizeof(tempstr));
                        memcpy(p_auth_tx->EMV_details.issuer_script_2, tempstr, fieldLength*2);
						strncat((char*)iccdata,"9F6E",4);
						strncat((char*)iccdata, (char*)p_auth_tx->EMV_details.issuer_script_2, sizeof(p_auth_tx->EMV_details.issuer_script_2)-1);
                        break;
               case 0x7C:
            	   	    genutil_bin_to_ascii ((BYTE*)&emv_raw[dataIndex], fieldLength , &tempstr[0], sizeof(tempstr));
                        memcpy(p_auth_tx->EMV_details.issuer_script_2+8, tempstr, fieldLength*2);
						strncat((char*)iccdata,"9F7C",4);
						strncat((char*)iccdata, (char*)p_auth_tx->EMV_details.issuer_script_2+8, fieldLength*2);
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
                     genutil_bin_to_ascii ((BYTE*)&emv_raw[dataIndex], fieldLength , &tempstr[0], sizeof(tempstr));
                     memcpy(p_auth_tx->EMV_details.issuer_script_1+6, tempstr, sizeof(p_auth_tx->EMV_details.issuer_script_1)-7);

					 strncat((char*)iccdata,"71",2);
					 strncat((char*)iccdata, (char*)p_auth_tx->EMV_details.issuer_script_1+6, sizeof(p_auth_tx->EMV_details.issuer_script_1)-7);
                     break;
            case 0x82:
                     genutil_bin_to_ascii ((BYTE*)&emv_raw[dataIndex], fieldLength , &tempstr[0], sizeof(tempstr));
                     memcpy(p_auth_tx->EMV_details.interchange_profile, tempstr, sizeof(p_auth_tx->EMV_details.interchange_profile)-1);
					 strncat((char*)iccdata,"82",2);
					 strncat((char*)iccdata, (char*)p_auth_tx->EMV_details.interchange_profile, sizeof(p_auth_tx->EMV_details.interchange_profile)-1);
                     break;
            case 0x84:
                     genutil_bin_to_ascii ((BYTE*)&emv_raw[dataIndex], fieldLength , &tempstr[0], sizeof(tempstr));
                     memcpy(p_auth_tx->EMV_details.app_id, tempstr, sizeof(p_auth_tx->EMV_details.app_id)-1);
					 strncat((char*)iccdata,"84",2);
					 strncat((char*)iccdata, (char*)p_auth_tx->EMV_details.app_id, sizeof(p_auth_tx->EMV_details.app_id)-1);
                     break;
            case 0x91:
                     genutil_bin_to_ascii ((BYTE*)&emv_raw[dataIndex], fieldLength , &tempstr[0], sizeof(tempstr));
                     memcpy(p_auth_tx->EMV_details.issuer_auth_data+1, tempstr, sizeof(p_auth_tx->EMV_details.issuer_auth_data)-2);
                     p_auth_tx->EMV_details.issuer_auth_data[0] = fieldLength;
					 strncat((char*)iccdata,"91",2);
					 strncat((char*)iccdata, (char*)p_auth_tx->EMV_details.issuer_auth_data, sizeof(p_auth_tx->EMV_details.issuer_auth_data)-1);
                     break;
            case 0x95:
                     genutil_bin_to_ascii ((BYTE*)&emv_raw[dataIndex], fieldLength , &tempstr[0], sizeof(tempstr));
                     memcpy(p_auth_tx->EMV_details.tvr, tempstr, sizeof(p_auth_tx->EMV_details.tvr)-1);
					 strncat((char*)iccdata,"95",2);
					 strncat((char*)iccdata, (char*)p_auth_tx->EMV_details.tvr, sizeof(p_auth_tx->EMV_details.tvr)-1);
                     break;
            case 0x9A:
                     genutil_bcd_to_asc ((BYTE*)&emv_raw[dataIndex], fieldLength , &tempstr[0], sizeof(tempstr));
                     memcpy(p_auth_tx->EMV_details.tran_date, tempstr, sizeof(p_auth_tx->EMV_details.tran_date)-1);
					 strncat((char*)iccdata,"9A",2);
					 strncat((char*)iccdata, (char*)p_auth_tx->EMV_details.tran_date, sizeof(p_auth_tx->EMV_details.tran_date)-1);
                     break;
            case 0x9C:
                     genutil_bcd_to_asc ((BYTE*)&emv_raw[dataIndex], fieldLength , &tempstr[0], sizeof(tempstr));
                     memcpy(p_auth_tx->EMV_details.tran_type, tempstr, sizeof(p_auth_tx->EMV_details.tran_type)-1);
					 strncat((char*)iccdata,"9C",2);
					 strncat((char*)iccdata, (char*)p_auth_tx->EMV_details.tran_type, sizeof(p_auth_tx->EMV_details.tran_type)-1);
                     break;
			case 0x4F:
                     genutil_bin_to_ascii ((BYTE*)&emv_raw[dataIndex], fieldLength , &tempstr[0], sizeof(tempstr));
                     memcpy(p_auth_tx->EMV_details.future_field1, tempstr, sizeof(p_auth_tx->EMV_details.future_field1)-1);
					 strncat((char*)iccdata,"4F",2);
					 strncat((char*)iccdata, (char*)p_auth_tx->EMV_details.future_field1, sizeof(p_auth_tx->EMV_details.future_field1)-1);
                     break;
			case 0x72:
                     genutil_bin_to_ascii ((BYTE*)&emv_raw[dataIndex], fieldLength , &tempstr[0], sizeof(tempstr));
                     memcpy(p_auth_tx->EMV_details.future_field2, tempstr, sizeof(p_auth_tx->EMV_details.future_field2)-1);
					 strncat((char*)iccdata,"72",2);
					 strncat((char*)iccdata, (char*)p_auth_tx->EMV_details.future_field2, sizeof(p_auth_tx->EMV_details.future_field2)-1);
                     break;
            default:
                     break;
         }
         dataIndex += fieldLength;
      }
   }
   return;
}

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
                     genutil_bin_to_ascii ((BYTE*)&emv_raw[dataIndex], fieldLength , &tempstr[0], sizeof(tempstr));
                     memcpy(p_auth_tx->EMV_details.issuer_auth_data+1, tempstr, sizeof(p_auth_tx->EMV_details.issuer_auth_data)-2);
                     p_auth_tx->EMV_details.issuer_auth_data[0] = fieldLength;
                     break;
            case 0x95:
                     genutil_bin_to_ascii ((BYTE*)&emv_raw[dataIndex], fieldLength , &tempstr[0], sizeof(tempstr));
                     memcpy(p_auth_tx->EMV_details.tvr, tempstr, sizeof(p_auth_tx->EMV_details.tvr)-1);
                     break;
            case 0x9A:
                     genutil_bcd_to_asc ((BYTE*)&emv_raw[dataIndex], fieldLength , &tempstr[0], sizeof(tempstr));
                     memcpy(p_auth_tx->EMV_details.tran_date, tempstr, sizeof(p_auth_tx->EMV_details.tran_date)-1);
                     break;
            case 0x9C:
                     genutil_bcd_to_asc ((BYTE*)&emv_raw[dataIndex], fieldLength , &tempstr[0], sizeof(tempstr));
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
	
	// version name and number copied. 
	
	// app_crypto added 
	if(strlen((char*)EMV_details.app_crypto)!=0)
	{
		strncat((char*)iccdata,"9F26",4);
		strncat((char*)iccdata,(char*)EMV_details.app_crypto,APP_CRYT_DATA_ASCII_LEN);
	}
	//CryptogramInfoData
	if(strlen((char*)EMV_details.crypto_info)!=0)
	{
		strncat((char*)iccdata,"9F27",4);
		strncat((char*)iccdata,(char*)EMV_details.crypto_info,CRYPTO_INFO_DATA_ASCII_LEN);
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
	//AppInterchProfile
	if(strlen((char*)EMV_details.interchange_profile)!=0)
	{
		strncat((char*)iccdata,"82",2);
		strncat((char*)iccdata,(char*)EMV_details.interchange_profile,AIP_DATA_ASCII_LEN);
	}
	//TerminalCountryCode
	if(strlen((char*)EMV_details.country_code)!=0)
	{
		strncat((char*)iccdata,"9F1A",4);
		strncat((char*)iccdata,(char*)EMV_details.country_code,TER_CUR_CODE_DATA_ASCII_LEN);
	}
	//AmountOtherNum
	if(strlen((char*)EMV_details.other_amount)!=0)
	{
		strncat((char*)iccdata,"9F03",4);
		strncat((char*)iccdata,(char*)EMV_details.other_amount,AMT_OTH_DATA_ASCII_LEN);
	}
	//
	if(strlen((char*)EMV_details.cvm_results)!=0)
	{
		strncat((char*)iccdata,"9F34",4);
		strncat((char*)iccdata,(char*)EMV_details.cvm_results,AMT_OTH_DATA_ASCII_LEN);
	}
	//PANSequenceNo
	if(strlen((char*)EMV_details.pan_sequence_number)!=0)
	{
		strncat((char*)iccdata,"5F34",4);
		strncat((char*)iccdata,(char*)EMV_details.pan_sequence_number,APP_PAN_SEQ_NUM_DATA_ASCII_LEN);
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
	JcbInfo jinfo;
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
