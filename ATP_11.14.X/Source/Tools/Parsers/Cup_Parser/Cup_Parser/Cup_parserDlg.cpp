
// Cup_parserDlg : implementation file
//

#include "stdafx.h"
#include "Cup_Parser.h"
#include "Cup_parserDlg.h"
#include "afxdialogex.h"
#include "CupParser.h"
#include "CUPInfo.h"
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
#define AMEX_EMV_MAX_LEN				255

#define N_BFData ( sizeof(BFData) / sizeof (struct bitfield_data) )

char FieldStrings [][256] =
{
"000 MsgType				: ",
"001 BitMap				    : ",
"002 PAN					: ",
"003 ProcessingCode			: ",
"004 TxnAmount				: ",
"005 SettleAmount		                     	: ",
"006 BillingAmount				: ",
"007 TransmitDateTime			: ",
"008 BillingFeeAmount		: ",
"009 SettleConvRate		    : ",
"010 BillingConvRate				: ",
"011 SystemTraceNo			: ",
"012 TxnTime				: ",
"013 TxnDate				: ",
"014 ExpirationDate				: ",
"015 SettleDate				: ",
"016 ConvDate				: ",
"017 CaptureDate			: ",
"018 MerchantType				: ",
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
//"031 SettleProcFeeAmount	: ",
"031 TransactionID			: ",
"032 AcquiringIdCode	                	: ",
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
"044 AddRespData			: ",
"045 Track1					: ",
"046 ISOAddData				: ",
"047 NationalAddData		: ",
"048 PrivateAddData			: ",
"049 TxnCurrency				: ",
"050 SettleCurrency				: ",
"051 BillingCurrency				: ",
"052 PINData				: ",
"053 SecurityInfo				: ",
"054 AddAmounts				: ",
"055 IccData				: ",
"056 Token PAR				: ",
"057 Additional data prvt				: ",
"058 Field58				: ",
"059 Detaild Inqry Data1				: ",
"060 Self-Defined 				: ",
"061 Crd Authntcn Info			: ",
"062 Field62				: ",
"063 Financl Ntwrk Data				: ",
"064 MsgAuthCode			: ",
"065 Field 65               : ",
"066 SettlementCode			: ",
"067 PaymentCodeEx			: ",
"068 RecInstCntrCode		: ",
"069 SettlInstCntrCode		: ",
"070 NetworkInfCode			: ",
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
"090 OriginDataElement		: ",
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
"101 Field 101				: ",
"102 AccountId1				: ",
"103 AccountId2				: ",
"104 TxnDescription			: ",
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
"121 Cups Resrvd				: ",
"122 AcqrerInstitutersrvd				: ",
"123 IsurInstitutersrvd				: ",
"124 Field124				: ",
"125 Field125				: ",
"126 Field126				: ",
"127 Field127				: ",
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
   BYTE hdr_len       [1];
   BYTE hdr_flag_ver  [1];
   BYTE tot_msg_len   [4];
   BYTE dest_stat_id  [11];
   BYTE source_id     [11];
   BYTE res_for_use   [3];
   BYTE batch_no      [1];
   BYTE tran_info     [8];
   BYTE user_info     [1];
   BYTE rej_code      [5];
   BYTE msgtype       [4];
   BYTE bitmap1_bin   [16];
   BYTE bitmap2_bin   [16];
   BYTE pan_len       [2];
   BYTE pan           [19];
   BYTE proccode      [6];
   BYTE amount        [12];
   BYTE setl_amount   [12];
   BYTE ch_bill_amt   [12];
   BYTE gmt_time      [10];
   BYTE conv_rate_stl [8];
   BYTE setl_conv_rate[8];
   BYTE conv_rate_chb [8];
   BYTE stan          [6];
   BYTE time          [6];
   BYTE date		  [4];
   BYTE date_expd	  [4];
   BYTE date_settle   [4];
   BYTE conv_date     [4];
   BYTE merch_type	  [4];
   BYTE ctry_code	  [3];
   BYTE posentry      [3];
   BYTE card_seq_num  [3];
   BYTE pos_condcd    [4];
   BYTE pos_pincode   [2];
   BYTE tran_fee_amt  [9];
   BYTE acq_inst_len  [2];
   BYTE acq_inst_id   [11];
   BYTE fwd_inst_len  [2];
   BYTE fwd_inst_id   [11];
   BYTE track2_len    [2];
   BYTE track2        [37];
   BYTE track3_len    [3];
   BYTE track3        [104];
   BYTE rrn			  [12];
   BYTE authcode      [6];
   BYTE responsecd    [2];
   BYTE termid        [8];
   BYTE cardaccid     [15];
   BYTE cardaccname [40];
   BYTE addl_rsp_len  [2];
   BYTE addlrspdata   [25];
   BYTE track1_len    [2];
   BYTE track1        [77];
   BYTE addl_data_len [3];
   BYTE addl_data     [70];
   BYTE tran_curr_cd  [3];
   BYTE settle_cur_cd  [3];
   BYTE card_curr_cd  [3];
   BYTE pinblock      [8];
   BYTE sec_cntl      [16];
   BYTE AddlAmts_len   [3];
   BYTE AddlAmts     [40];
   BYTE IccData_len   [3];
   BYTE IccData	    [264];
   BYTE add_data_pvt_len [3];
   BYTE add_data_pvt [100];	
   BYTE det_inq_data1_len [3];
   BYTE det_inq_data1 [600];
   BYTE self_def_feild_len [3];
   BYTE self_def_feild [100];
   BYTE card_holder_auth_info_len [3];
   BYTE card_holder_auth_info [200];
   BYTE switching_data_len[3];
   BYTE switching_data[200];
   BYTE net_manag_info_code [3];
   BYTE orig_data [42];
   BYTE mess_sec_code[8];
   BYTE recv_ins_id_code_len[2];
   BYTE recv_ins_id_code[11];
   BYTE acc_id_len[2];
   BYTE acc_id[28];
   BYTE cup_resv_len[3];
   BYTE cup_resv[100];
   BYTE accq_inst_resv_len[3];
   BYTE accq_inst_resv[100];
   BYTE issu_inst_resv_len [3];
   BYTE issu_inst_resv [100];
   BYTE security_rai_len [3];
   BYTE security_rai[256];
   BYTE mess_auth_code [8];
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
INT		invalid_msg_format;
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

// From Cup
enum bitfields 
{
   bf_done                 = 0,      /* end of list */
   bf_1xbitfield           = 1,      /* extended bit field */
   bf_2pan                 = 2,      /* Pan */
   bf_3proc                = 3,      /* Processing code */
   bf_4amount              = 4,      /* Transaction Amount */
   bf_5settleamt	       = 5,	     /* Settlement Amount */
   bf_6cardbillamt         = 6,      /* Cardholder Billing Amount */
   bf_7trntime             = 7,      /* Transmission date-time */
   bf_9settconvrate        = 9,      /* Settlement Conversion Rate */
   bf_10cardbillconvrate   = 10,     /* Cardholder Billing Conversion Rate */
   bf_11stan               = 11,     /* System trace number */
   bf_12time               = 12,     /* Local Transaction time */
   bf_13date               = 13,     /* Local Transaction date */
   bf_14expiry		       = 14,	 /* Expiry date */
   bf_15datestl            = 15,     /* Settlement date */
   bf_16dateconv           = 16,     /* Conversion date */
   bf_18mertype		       = 18,	 /* Merchant type	*/
   bf_19ctrycd             = 19,     /* Acquiring institution's country code(MCC) */
   bf_22posentry           = 22,     /* Pos entry mode  */
   bf_23csn                = 23,	 /* Card Sequence Number */
   bf_25poscc              = 25,     /* Pos condition code  */
   bf_26pincode		       = 26,	 /* Point of Service PIN Capture Code */
   bf_28tranfeeamt         = 28,     /* Transaction fee amount */
   bf_32idcode             = 32,     /* Acquiring institution's ID code  */
   bf_33fwdinstidcode      = 33,     /* Forwarding institution ID code */
   bf_35track2             = 35,     /* Track 2 data */
   bf_36track3             = 36,     /* Track 3 data */
   bf_37rrn	               = 37,	 /* Retrieval ref number */
   bf_38authcode           = 38,     /* Auth identification response/code */
   bf_39respcode           = 39,     /* Response code */
   bf_41termid             = 41,     /* Terminal id  */
   bf_42cardacc            = 42,     /* Card acceptor id  */
   bf_43cardacceptloc      = 43,     /* Card acceptor name/location */
   bf_44respdata	       = 44,     /* Additional response data */
   bf_45track1             = 45,     /* Track 1 data */
   bf_48addldata           = 48,     /* Additional private data */
   bf_49trncurcd           = 49,     /* Transaction currency code */
   bf_50stlmtcurrcd        = 50,     /* Settlement currency code */
   bf_51crdcurcd           = 51,     /* Cardholder Billing Currency Code */
   bf_52pinblock           = 52,     /* Pin block */
   bf_53seccntl            = 53,     /* Recurity related control info  */
   bf_54AddlAmount         = 54,     /* Additional amount */
   bf_55IccData		       = 55,     /* IC Card data*/
   bf_57adddatapvt         = 57,	 /* Additional Data Private */
   bf_59detinqdata1        = 59,     /* Detailed Inquiry Data1 */
   bf_60selfdeffeild 	   = 60,     /* Self-Defined Field */
   bf_61cardholderauthinfo = 61,     /* Cardholder Authentication Information */
   bf_62SwitchingData      = 62,     /* Switching Data*/
   bf_70netmanginfocode    = 70,     /* Network Management Information Code */
   bf_90origdata           = 90,     /* Original Data */
   bf_96messeccode         = 96,     /* Message Security Code */
   bf_100revinsidcode      = 100,    /* Receiving Institution Identification Code */
   bf_102actid1            = 102,    /* Account Identification 1 */
   bf_121CUPSres           = 121,    /* CUPS Reserved */
   bf_122accqinstres       = 122,    /* Acquirer Institution Reserved */
   bf_123issrinstres       = 123,    /* Issuer Institution Reserved */
   bf_125security_RAI      = 125,    /* Security and Risk Assessment Information*/
   bf_128messuthcode       = 128     /* Message Authentication Code */
   
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
		INT             (*bd_BFCond)(enum bitfields, BYTE);   /* ptr to conditional function */
		INT             (*bd_BFMove)(enum bitfields);         /* ptr to move function */
		INT             (*bd_BFRsp)(enum bitfields);          /* ptr to response handler function */
   };
/**********************************
      FUNCTION PROTOTYPES
**********************************/
/* functions local */

//For Cup
INT		Move_Never(enum bitfields, BYTE);
INT		vMove_IfThere(enum bitfields, BYTE);
INT		Move_IfThere(enum bitfields, BYTE);
INT		Move_Always(enum bitfields, BYTE);
BYTE	getFldSep( unsigned char* resp_buff, int index );
INT		GenericMove(enum bitfields fieldid);
INT		RspMoveGeneric(enum bitfields fieldid);
void	find_cvc_flag( char *, char *, char *);
INT		RspMovevData (enum bitfields fieldid);
int		get_variable_length( unsigned char *DataField, int FieldLen );
INT		vDataMove(enum bitfields fieldid);
void    parsed_mssg();
INT     parse_host_msg( pCHAR err_buf );
BYTE	ascii_ebcdic_conv(BYTE conv_char, char * conv_type );
BYTE	ascii_ebcdic_conv_trk1( unsigned char conv_char, char *conv_type,unsigned char fldSep);

void Generate_CUP_EMV_Details();
void genutil_EMV_To_Auth_Tx( BYTE * emv_raw, INT total_len, pAUTH_TX p_auth_tx );
void genutil_bcd_to_asc (BYTE bstring[], INT blen, pCHAR astr, INT astr_len);

INT hexchar2byte (pBYTE dst, pBYTE src);

INT hexchar2byte (pBYTE dst, pBYTE src)
{
  INT    i;
  BYTE   n_src [16];

  for (i=0; i<2; i++)
    {
    if (src[i] <= '9')
      n_src[i] = (BYTE)(src[i] - '0');
    else
      n_src[i] = (BYTE)(src[i] - 'A' + 10);

    /* let's test key information - just for in case... */
    if (n_src[i] > 0xf)
      return 0;
    }

  dst[0] = (BYTE)((0xf0&(n_src[0]<<4)) | (0x0f&n_src[1]));
  return 1;
}
//static  struct bitfield_data *BFData; 
// From Cup
static  struct bitfield_data BFData[] = {
   {
   bf_2pan,                      /* Primary Account Number  */
   HOST_MESSAGE.pan_len,         /* data to move */
   2,                            /* # of bytes in pan len field */
   Move_IfThere,                  /* mandatory */
   vDataMove,                     /* move routine */
   RspMovevData                   /* response routine */
   },
   {
   bf_3proc,                     /* field */
   HOST_MESSAGE.proccode,        /* data to move */
   6,                            /* # of bytes in  iso field */
   Move_Always,                  /* mandatory */
   GenericMove,                   /* move routine */
   RspMoveGeneric                 /* response routine */
   },
   {
   bf_4amount,                   /* field */
   HOST_MESSAGE.amount,          /* data to move */
   12,                           /* # of bytes in  iso field */
   Move_Always,                  /* mandatory */
   GenericMove,                   /* move routine */
   RspMoveGeneric                 /* response routine */
   },
   {
   bf_5settleamt,                /* field */
   HOST_MESSAGE.setl_amount,     /* data to move */
   12,                           /* # of bytes in  iso field */
   Move_IfThere,                  /* mandatory */
   GenericMove,                   /* move routine */
   RspMoveGeneric                 /* response routine */
   },
   {
   bf_6cardbillamt,                /* field */
   HOST_MESSAGE.ch_bill_amt,     /* data to move */
   12,                           /* # of bytes in  iso field */
   Move_IfThere,                  /* mandatory */
   GenericMove,                   /* move routine */
   RspMoveGeneric                 /* response routine */
   },
   {
   bf_7trntime,                  /* field */
   HOST_MESSAGE.gmt_time,        /* data to move : MMDDhhmmss*/
   10,                           /* # of bytes in  iso field */
   Move_IfThere,                  /* conditional */
   GenericMove,                   /* move routine */
   RspMoveGeneric                 /* response routine */
   },
   {
   bf_9settconvrate,             /* field */
   HOST_MESSAGE.setl_conv_rate,  /* data to move */
   8,                            /* # of bytes in  iso field */
   Move_IfThere,                  /* conditional */
   GenericMove,                   /* move routine */
   RspMoveGeneric                 /* response routine */
   },
   {
   bf_10cardbillconvrate,             /* field */
   HOST_MESSAGE.conv_rate_chb,  /* data to move */
   8,                            /* # of bytes in  iso field */
   Move_IfThere,                  /* conditional */
   GenericMove,                   /* move routine */
   RspMoveGeneric                 /* response routine */
   },
   {
   bf_11stan,                    /* field */
   HOST_MESSAGE.stan,            /* data to move */
   6,                            /* # of bytes in  iso field */
   Move_Always,                  /* conditional */
   GenericMove,                   /* move routine */
   RspMoveGeneric                 /* response routine */
   },
   {
   bf_12time,                    /* field */
   HOST_MESSAGE.time,            /* data to move */
   6,                            /* # of bytes in  iso field */
   Move_IfThere,                  /* conditional */
   GenericMove,                   /* move routine */
   RspMoveGeneric                 /* response routine */
   },
   {
   bf_13date,                    /* field */
   HOST_MESSAGE.date,            /* effective data to move */
   4,                            /* # of bytes in  iso field */
   Move_IfThere,                  /* conditional */
   GenericMove,                   /* move routine */
   RspMoveGeneric                 /* response routine */
   },
   {
   bf_14expiry,                  /* field */
   HOST_MESSAGE.date_expd,       /* data to move */
   4,                            /* # of bytes in field */
   Move_IfThere,                     /* conditional */
   GenericMove,                   /* move routine */
   RspMoveGeneric                 /* response routine */
   },
   {
   bf_15datestl,                 /* field */
   HOST_MESSAGE.date_settle,     /* data to move */
   4,                            /* # of bytes in  iso field */
   Move_IfThere,                 /* conditional */
   GenericMove,                   /* move routine */
   RspMoveGeneric                 /* response routine */
   },
   {
   bf_16dateconv,                /* field */
   HOST_MESSAGE.conv_date,       /* data to move */
   4,                            /* # of bytes in  iso field */
   Move_IfThere,                  /* conditional */
   GenericMove,                   /* move routine */
   RspMoveGeneric                 /* response routine */
   },
   {
   bf_18mertype,                 /* field */
   HOST_MESSAGE.merch_type,            /* data to move */
   4,                            /* # of bytes in  iso field */
   Move_IfThere,                  /* conditional */
   GenericMove,                   /* move routine */
   RspMoveGeneric                 /* response routine */
   },
   {
   bf_19ctrycd,                  /* field */
   HOST_MESSAGE.ctry_code,       /* data to move */
   3,                            /* # of bytes in field */
   Move_IfThere,                  /* mandatory    */
   GenericMove,                   /* move routine */
   RspMoveGeneric                 /* response routine */
   },
   {
   bf_22posentry,                /* field */
   HOST_MESSAGE.posentry,        /* data to move */
   3,                            /* # of bytes in  iso field */
   Move_IfThere,                  /* conditional */
   GenericMove,                   /* move routine */
   RspMoveGeneric                 /* response routine */
   },
   {
   bf_23csn,                  /* field */
   HOST_MESSAGE.card_seq_num,         /* data to move */
   3,                            /* # of bytes in field */
   Move_IfThere,                 /* conditional */
   GenericMove,                   /* move routine */
   RspMoveGeneric                 /* response routine */
   },
   {
   bf_25poscc,                   /* field */
   HOST_MESSAGE.pos_condcd,      /* data to move */
   2,                            /* # of bytes in  iso field */
   Move_IfThere,                  /* conditional */
   GenericMove,                   /* move routine */
   RspMoveGeneric                 /* response routine */
   },
    {
   bf_26pincode,                 /* field */
   HOST_MESSAGE.pos_pincode,    /* data to move */
   2,                            /* # of bytes in field */
   Move_IfThere,                  /* conditional */
   GenericMove,                   /* move routine */
   RspMoveGeneric                 /* response routine */
   },
   {
   bf_28tranfeeamt,              /* field */
   HOST_MESSAGE.tran_fee_amt,      /* data to move */
   9,                            /* # of bytes in  iso field */
   Move_IfThere,                  /* conditional */
   GenericMove,                   /* move routine */
   RspMoveGeneric                 /* response routine */
   },
   {
   bf_32idcode,                  /* field */
   HOST_MESSAGE.acq_inst_len,    /* data to move */
   2,                            /* # of bytes in  iso field */
   Move_IfThere,                  /* conditional */
   vDataMove,                  /* move routine */
   RspMovevData                /* response routine */
   },
   {
   bf_33fwdinstidcode,                  /* field */
   HOST_MESSAGE.fwd_inst_len,    /* data to move */
   2,                            /* # of bytes in  iso field */
   Move_IfThere,                  /* conditional */
   vDataMove,                  /* move routine */
   RspMovevData                /* response routine */
   },
   {
   bf_35track2,                  /* field */
   HOST_MESSAGE.track2_len,      /* data to move */
   2,                            /* # of bytes in track2 len field */
   Move_IfThere,                  /* conditional */
   vDataMove,                  /* move routine */
   RspMovevData                /* response routine */
   },
   {
   bf_36track3,                  /* field */
   HOST_MESSAGE.track3_len,      /* data to move */
   3,                            /* # of bytes in track2 len field */
   Move_IfThere,                  /* conditional */
   vDataMove,                  /* move routine */
   RspMovevData                /* response routine */
   },
   {
   bf_37rrn,                  /* field */
   HOST_MESSAGE.rrn,      /* data to move */
   12,                            /* # of bytes in track2 len field */
   Move_IfThere,                  /* conditional */
   GenericMove,                  /* move routine */
   RspMoveGeneric                /* response routine */
   },
   {
   bf_38authcode,                /* field */
   HOST_MESSAGE.authcode,        /* data to move */
   6,                            /* # of bytes in  iso field */
   Move_IfThere,                 /* conditional */
   GenericMove,                   /* move routine */
   RspMoveGeneric                 /* response routine */
   },
   {
   bf_39respcode,                /* field */
   HOST_MESSAGE.responsecd,      /* data to move */
   2,                            /* # of bytes in  iso field */
   Move_IfThere,                 /* conditional */
   GenericMove,                   /* move routine */
   RspMoveGeneric                 /* response routine */
   },
   {
   bf_41termid,                  /* field */
   HOST_MESSAGE.termid,          /* data to move */
   8,                            /* # of bytes in  iso field */
   Move_IfThere,                 /* conditional */
   GenericMove,                   /* move routine */
   RspMoveGeneric                 /* response routine */
   },
   {
   bf_42cardacc,                 /* field */
   HOST_MESSAGE.cardaccid,       /* data to move */
   15,                           /* # of bytes in  iso field */
   Move_IfThere,                  /* conditional */
   GenericMove,                   /* move routine */
   RspMoveGeneric                 /* response routine */
   },
   {
   bf_43cardacceptloc,                 /* field */
   HOST_MESSAGE.cardaccname,       /* data to move */
   40,                           /* # of bytes in  iso field */
   Move_IfThere,                  /* conditional */
   GenericMove,                   /* move routine */
   RspMoveGeneric                 /* response routine */
   },
   {
   bf_44respdata,                 /* field */
   HOST_MESSAGE.addl_rsp_len,       /* data to move */
   2,                           /* # of bytes in  iso field */
   Move_IfThere,                  /* conditional */
   vDataMove,                   /* move routine */
   RspMovevData					/* response routine */
   },
   {
   bf_45track1,                  /* field */
   HOST_MESSAGE.track1_len,      /* data to move */
   2,                            /* # of bytes in len field */
   Move_IfThere,                   /* conditional */
   vDataMove,                  /* move routine */
   RspMovevData                /* response routine */
   },
   {
   /*Need to write function to read binary data*/
   bf_48addldata,                /* field */
   HOST_MESSAGE.addl_data_len,   /* data to move */
   3,                            /* # of bytes in track1 len field */
   Move_IfThere,                 /* conditional */
   vDataMove,                  /* move routine */
   RspMovevData                /* response routine */
   },
   {
   bf_49trncurcd,               /* field */
   HOST_MESSAGE.tran_curr_cd,   /* data to move */
   3,                           /* # of bytes in  iso field */
   Move_IfThere,                /* mandatory    */
   GenericMove,                 /* move routine */
   RspMoveGeneric				/* response routine */
   },
   {
   bf_50stlmtcurrcd,               /* field */
   HOST_MESSAGE.settle_cur_cd,    /* data to move */
   3,                            /* # of bytes in field */
   Move_IfThere,                 /* mandatory    */
   GenericMove,                   /* move routine */
   RspMoveGeneric                 /* response routine */
   },
   {
   bf_51crdcurcd,               /* field */
   HOST_MESSAGE.card_curr_cd,    /* data to move */
   3,                            /* # of bytes in field */
   Move_IfThere,                 /* mandatory    */
   GenericMove,                   /* move routine */
   RspMoveGeneric                 /* response routine */
   },
   {
   bf_52pinblock,                /* field */
   HOST_MESSAGE.pinblock,        /* data to move */
   8,                            /* # of bytes in  iso field */
   Move_IfThere,                  /* mandatory */
   GenericMove,                  /* move routine */
   RspMoveGeneric                /* response routine */
   },
   {
   bf_53seccntl,                 /* field */
   HOST_MESSAGE.sec_cntl,    /* data to move */
   16,                            /* # of bytes in field */
   Move_IfThere,                   /* mandatory    */
   GenericMove,                   /* move routine */
   RspMoveGeneric                 /* response routine */
   },
   {
   bf_54AddlAmount,                 /* field */
   HOST_MESSAGE.AddlAmts_len,   /* data to move */
   3,                            /* # of bytes in field */
   Move_IfThere,                  /* mandatory    */
   vDataMove,                  /* move routine */
   RspMovevData                /* response routine */
   },
   {
   bf_55IccData,                 /* field */
   HOST_MESSAGE.IccData_len,   /* data to move */
   3,                            /* # of bytes in field */
   Move_IfThere,                  /* mandatory    */
   vDataMove,                  /* move routine */
   RspMovevData                /* response routine */
   },
   {
   bf_57adddatapvt,                 /* field */
   HOST_MESSAGE.add_data_pvt_len,   /* data to move */
   3,                            /* # of bytes in field */
   Move_IfThere,                  /* mandatory    */
   vDataMove,                  /* move routine */
   RspMovevData                /* response routine */
   },
   {
   bf_59detinqdata1,                 /* field */
   HOST_MESSAGE.det_inq_data1_len,   /* data to move */
   3,                            /* # of bytes in field */
   Move_IfThere,                  /* mandatory    */
   vDataMove,                  /* move routine */
   RspMovevData                /* response routine */
   },
   {   
   bf_60selfdeffeild,                 /* field */   
   HOST_MESSAGE.self_def_feild_len,     /* data to move */  
   3,                           /* # of bytes in field */   
   Move_IfThere,                /* mandatory    */   
   vDataMove,                  /* move routine */   
   RspMovevData                /* response routine */   
   },
   {   
   bf_61cardholderauthinfo,                 /* field */   
   HOST_MESSAGE.card_holder_auth_info_len,     /* data to move */  
   3,                           /* # of bytes in field */   
   Move_IfThere,                /* mandatory    */   
   vDataMove,                  /* move routine */   
   RspMovevData                /* response routine */   
   },
   {
   bf_62SwitchingData,                 /* field */   
   HOST_MESSAGE.switching_data_len,     /* data to move */  
   3,                           /* # of bytes in field */   
   Move_IfThere,                /* mandatory    */   
   vDataMove,                  /* move routine */   
   RspMovevData                /* response routine */   
   },
   {
   bf_70netmanginfocode,                /* field */
   HOST_MESSAGE.net_manag_info_code,    /* data to move */
   3,                            		/* # of bytes in  iso field */
   Move_IfThere,                 		/* mandatory    */
   GenericMove,               		/* move routine */
   RspMoveGeneric               		/* response routine */
   },
   {
   bf_90origdata,                /* field */
   HOST_MESSAGE.orig_data,    /* data to move */
   42,                            		/* # of bytes in  iso field */
   Move_IfThere,                 		/* mandatory    */
   GenericMove,               		/* move routine */
   RspMoveGeneric                /* response routine */
   },
   {
   bf_96messeccode,                /* field */
   HOST_MESSAGE.mess_sec_code,    /* data to move */
   8,                            		/* # of bytes in  iso field */
   Move_Always,                 		/* mandatory    */
   GenericMove,               		/* move routine */
   RspMoveGeneric                /* response routine */
   },
   {
   bf_100revinsidcode,                /* field */
   HOST_MESSAGE.recv_ins_id_code_len,    /* data to move */
   2,                            		/* # of bytes in  iso field */
   Move_IfThere,                 		/* mandatory    */
   vDataMove,               		/* move routine */
   RspMovevData                /* response routine */
   },
   {
   bf_102actid1,                /* field */
   HOST_MESSAGE.acc_id_len,    /* data to move */
   2,                            		/* # of bytes in  iso field */
   Move_IfThere,                 		/* mandatory    */
   vDataMove,               		/* move routine */
   RspMovevData                /* response routine */
   },
   {
   bf_121CUPSres,                /* field */
   HOST_MESSAGE.cup_resv_len,    /* data to move */
   3,                            		/* # of bytes in  iso field */
   Move_IfThere,                 		/* mandatory    */
   vDataMove,               		/* move routine */
   RspMovevData                /* response routine */
   },
   {
   bf_122accqinstres,                /* field */
   HOST_MESSAGE.accq_inst_resv_len,    /* data to move */
   3,                            		/* # of bytes in  iso field */
   Move_IfThere,                 		/* mandatory    */
   vDataMove,               		/* move routine */
   RspMovevData                /* response routine */
   },
   {
   bf_123issrinstres,                /* field */
   HOST_MESSAGE.issu_inst_resv_len,    /* data to move */
   3,                            		/* # of bytes in  iso field */
   Move_IfThere,                 		/* mandatory    */
   vDataMove,               		/* move routine */
   RspMovevData                /* response routine */
   },
   {
   bf_125security_RAI,                /* field */
   HOST_MESSAGE.security_rai_len,    /* data to move */
   3,                            		/* # of bytes in  iso field */
   Move_IfThere,                 		/* mandatory    */
   vDataMove,               		/* move routine */
   RspMovevData                /* response routine */
   },
   {
   bf_128messuthcode,                /* field */
   HOST_MESSAGE.mess_auth_code ,    /* data to move */
   8,                            		/* # of bytes in  iso field */
   Move_IfThere,                 		/* mandatory    */
   GenericMove,               		/* move routine */
   RspMoveGeneric                /* response routine */
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
 INT vDataMove(enum bitfields fieldid)
{
   struct bitfield_data *pBF;
   INT                  movelen;

   /*---------------------------------------------------*/
   /* find pointer to bitfield info for this bitfield...*/
   /*---------------------------------------------------*/
   pBF = Find_Bitfield_Table(fieldid);

   if (pBF != NULL)
   {
  
     movelen  = get_variable_length( pBF->p_bd_pdata, pBF->bd_fieldlen );
	 memcpy(&ReqRsp_Buffer[reqidx],&pBF->p_bd_pdata[0], pBF->bd_fieldlen);
     reqidx =reqidx + pBF->bd_fieldlen;

     memcpy(&ReqRsp_Buffer[reqidx], &pBF->p_bd_pdata[pBF->bd_fieldlen], movelen);

     reqidx = reqidx + movelen;   /* increment buffer pointer */

	 return 1;
   }
   return 0;
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
INT RspMovevData (enum bitfields fieldid)
{
   struct bitfield_data *pBF;
   INT                  movelen;
   char temp_len[4]={0};
   /*---------------------------------------------------*/
   /* find pointer to bitfield info for this bitfield...*/
   /*---------------------------------------------------*/
   pBF = Find_Bitfield_Table(fieldid);

   if (pBF != NULL)
   {
     
	 memcpy(&pBF->p_bd_pdata[0],&ReqRsp_Buffer[reqidx],pBF->bd_fieldlen);
	 reqidx =reqidx + pBF->bd_fieldlen;
	
	 strcpy(temp_len,(char *)pBF->p_bd_pdata);
  	 movelen = atoi(temp_len);
     memcpy(&pBF->p_bd_pdata[pBF->bd_fieldlen], &ReqRsp_Buffer[reqidx], movelen);

      reqidx = reqidx + movelen;   /* increment buffer pointer */
	  return 1;
   }
   return 0;
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

/*********ParsedMssg() to print the amex parsed message****/
void parsed_mssg()
{
	int bitCount, startCharacter, icnt, LastDisplayBit;
	BYTE security_raitag01[3]={0};
	CHAR ascii_value[5]={0}, security_rai[257] = {0};
		BYTE stagidlen[3]={0};
	char hexa[3]={0};
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
	LastDisplayBit = 1;
	char temp_len[4]={0};										// This is the bit that was displayed last.
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
			//if(strlen((char *)HOST_MESSAGE.pan_len) != 0)                     // card number  len
			if(BitFieldArray[bf_2pan] == 1)
			{	
				
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
		
		case bf_5settleamt :
			//if(strlen((char *)HOST_MESSAGE.setl_amount) != 0)
			if(BitFieldArray[bf_5settleamt] == 1)
			{
				strcat (Line[bitCount], FieldStrings [bitCount]);
				strncat (Line[bitCount], (char *)HOST_MESSAGE.setl_amount ,12);
			}
		break;
		
		case bf_6cardbillamt :
			//if(strlen((char *)HOST_MESSAGE.ch_bill_amt) != 0)
			if(BitFieldArray[bf_6cardbillamt] == 1)
			{
				strcat (Line[bitCount], FieldStrings [bitCount]);
				strncat (Line[bitCount], (char *)HOST_MESSAGE.ch_bill_amt ,12);
			}
			break ;

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
		
		case bf_10cardbillconvrate :
			//if(strlen((char *)HOST_MESSAGE.conv_rate_chb) != 0)
			if(BitFieldArray[bf_10cardbillconvrate] == 1)
			{
				strcat (Line[bitCount], FieldStrings [bitCount]);
				strncat (Line[bitCount], (char *)HOST_MESSAGE.conv_rate_chb ,8);
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
				strncat (Line[bitCount], (char *)HOST_MESSAGE.time, 6);
			}
		break ;

		case bf_13date :
			//if(strlen((char *)HOST_MESSAGE.date) != 0)
			if(BitFieldArray[bf_13date] == 1)
			{
				strcat (Line[bitCount], FieldStrings [bitCount]);
				strncat (Line[bitCount], (char *)HOST_MESSAGE.date, 4);
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
			//if(strlen((char *)HOST_MESSAGE.conv_date) != 0)
			if(BitFieldArray[bf_16dateconv] == 1)
			{
				strcat (Line[bitCount], FieldStrings [bitCount]);
				strncat (Line[bitCount], (char *)HOST_MESSAGE.conv_date, 4);
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
		
		case bf_22posentry :
			//if(strlen((char *)HOST_MESSAGE.posentry) != 0)
			if(BitFieldArray[bf_22posentry] == 1)
			{
				strcat (Line[bitCount], FieldStrings [bitCount]);
				strncat (Line[bitCount], (char *)HOST_MESSAGE.posentry, 3);
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

		case bf_25poscc :
			//if(strlen((char *)HOST_MESSAGE.pos_condcd) != 0)
			if(BitFieldArray[bf_25poscc] == 1)
			{
				strcat (Line[bitCount], FieldStrings [bitCount]);
				strncat (Line[bitCount], (char *)HOST_MESSAGE.pos_condcd, 2);
			}
		break ;
		

		case bf_26pincode :
			//if(strlen((char *)HOST_MESSAGE.pos_pincode) != 0)
			if(BitFieldArray[bf_26pincode] == 1)
			{
					strcat (Line[bitCount], FieldStrings [bitCount]);
					strncat (Line[bitCount], (char *)HOST_MESSAGE.pos_pincode , 2);
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
					len =atoi(temp_len);
					strcat (Line[bitCount], FieldStrings [bitCount]);
					strncat (Line[bitCount], (char *)HOST_MESSAGE.acq_inst_id ,len);
			}
		break ;
		
		case bf_33fwdinstidcode :
			//if(strlen((char *)HOST_MESSAGE.fwd_inst_len) != 0)
			if(BitFieldArray[bf_33fwdinstidcode] == 1)
			{
					strncpy(temp_len,(char *)HOST_MESSAGE.fwd_inst_len,2);
					len =atoi(temp_len);
					strcat (Line[bitCount], FieldStrings [bitCount]);
					strncat (Line[bitCount], (char *)HOST_MESSAGE.fwd_inst_id ,len);
			}
		break ;

		case bf_35track2 :
			//if(strlen((char *)HOST_MESSAGE.track2_len) != 0)
			if(BitFieldArray[bf_35track2] == 1)
			{
					strncpy(temp_len,(char *)HOST_MESSAGE.track2_len,2);
					len =atoi(temp_len);
					strcat (Line[bitCount], FieldStrings [bitCount]);
					strncat (Line[bitCount], (char *)HOST_MESSAGE.track2 ,len);
			}
		break ;

		case bf_36track3 :
			//if(strlen((char *)HOST_MESSAGE.track3_len) != 0)
			if(BitFieldArray[bf_36track3] == 1)
			{
					strncpy(temp_len,(char *)HOST_MESSAGE.track3_len,2);
					len =atoi(temp_len);
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
		
		case bf_43cardacceptloc :
			//if(strlen((char *)HOST_MESSAGE.cardaccname) != 0)
			if(BitFieldArray[bf_43cardacceptloc] == 1)
			{
					strcat (Line[bitCount], FieldStrings [bitCount]);
					strncat (Line[bitCount], (char *)HOST_MESSAGE.cardaccname,40);
			}
		break ;
		
		case bf_44respdata :
			//if(strlen((char *)HOST_MESSAGE.addl_rsp_len) != 0)			 //addl_rsp_len	
			if(BitFieldArray[bf_44respdata] == 1)
			{
					strncpy(temp_len,(char *)HOST_MESSAGE.addl_rsp_len,2);
					len =atoi(temp_len);
					strcat (Line[bitCount], FieldStrings [bitCount]);
					strncat (Line[bitCount], (char *)HOST_MESSAGE.addlrspdata,len);
			}
		break ;
		
		case bf_45track1 :
			//if(strlen((char *)HOST_MESSAGE.track1_len) != 0)			 //addl_rsp_len	
			if(BitFieldArray[bf_45track1] == 1)
			{
					strncpy(temp_len,(char *)HOST_MESSAGE.track1_len,2);
					len =atoi(temp_len);
					strcat (Line[bitCount], FieldStrings [bitCount]);
					strncat (Line[bitCount], (char *)HOST_MESSAGE.track1,len);
			}
		break ;

		case bf_48addldata :
			//if(strlen((char *)HOST_MESSAGE.addl_data_len) != 0)
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
			//if(strlen((char *)HOST_MESSAGE.tran_curr_cd) != 0)			// tran_curr_cd
			if(BitFieldArray[bf_49trncurcd] == 1)
			{
					strcat (Line[bitCount], FieldStrings [bitCount]);
					strncat (Line[bitCount], (char *)HOST_MESSAGE.tran_curr_cd,3);
			}
		break ;
		
		case bf_50stlmtcurrcd :
			//if(strlen((char *)HOST_MESSAGE.settle_cur_cd) != 0)
			if(BitFieldArray[bf_50stlmtcurrcd] == 1)
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
			//if(strlen((char *)HOST_MESSAGE.pinblock) != 0)           // pinblock
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
			//if(strlen((char *)HOST_MESSAGE.sec_cntl) != 0)	
			if(BitFieldArray[bf_53seccntl] == 1)
			{
					strcat (Line[bitCount], FieldStrings [bitCount]);
					strncat (Line[bitCount], (char *)HOST_MESSAGE.sec_cntl,16);
			}
		break ;
		
		case bf_54AddlAmount :
			//if(strlen((char *)HOST_MESSAGE.AddlAmts_len) != 0)			// AddlAmts_len
			if(BitFieldArray[bf_54AddlAmount] == 1)
			{
					strncpy(temp_len,(char *)HOST_MESSAGE.AddlAmts_len,3);
					len= atoi(temp_len);
					strcat (Line[bitCount], FieldStrings [bitCount]);
					strncat (Line[bitCount], (char *)HOST_MESSAGE.AddlAmts,len);
			}
		break ;
		
		case bf_55IccData :
			//if(strlen((char *)HOST_MESSAGE.IccData_len) != 0)		
			if(BitFieldArray[bf_55IccData] == 1)
			{
					strncpy(temp_len,(char *)HOST_MESSAGE.IccData_len,3);
					len =atoi(temp_len);
					strncpy((char *)iccdata,(char *)HOST_MESSAGE.IccData,atoi(temp_len));
					genutil_EMV_To_Auth_Tx( iccdata, atoi(temp_len),&AUTH_TX_V );
					memcpy(&EMV_details,&(AUTH_TX_V.EMV_details),sizeof(EMV_details));
					Generate_CUP_EMV_Details();
					strcat (Line[bitCount], FieldStrings [bitCount]);
					strncat (Line[bitCount], (char *)iccdata,strlen((char *)iccdata));
			}
		break ;
		
		case bf_57adddatapvt :
			//if(strlen((char *)HOST_MESSAGE.add_data_pvt_len) != 0)	
			if(BitFieldArray[bf_57adddatapvt] == 1)
			{
					strncpy(temp_len,(char *)HOST_MESSAGE.add_data_pvt_len,3);
					len =atoi(temp_len);
					strcat (Line[bitCount], FieldStrings [bitCount]);
					strncat (Line[bitCount], (char *)HOST_MESSAGE.add_data_pvt,len);
			}
		break ;

		case bf_59detinqdata1 :
			//if(strlen((char *)HOST_MESSAGE.add_data_pvt_len) != 0)
			if(BitFieldArray[bf_59detinqdata1] == 1)
			{
					strncpy(temp_len,(char *)HOST_MESSAGE.det_inq_data1_len,3);
					len =atoi(temp_len);
					strcat (Line[bitCount], FieldStrings [bitCount]);
					strncat (Line[bitCount], (char *)HOST_MESSAGE.det_inq_data1,len);
			}
		break ;

		case bf_60selfdeffeild :
			//if(strlen((char *)HOST_MESSAGE.self_def_feild_len) != 0)	
			if(BitFieldArray[bf_60selfdeffeild] == 1)
			{
					strncpy(temp_len,(char *)HOST_MESSAGE.self_def_feild_len,3);
					len =atoi(temp_len);
					strcat (Line[bitCount], FieldStrings [bitCount]);
					strncat (Line[bitCount], (char *)HOST_MESSAGE.self_def_feild,len);
			}
		break ;
		
		case bf_61cardholderauthinfo :
			//if(strlen((char *)HOST_MESSAGE.card_holder_auth_info_len) != 0)	
			if(BitFieldArray[bf_61cardholderauthinfo] == 1)
			{
					strncpy(temp_len,(char *)HOST_MESSAGE.card_holder_auth_info_len,3);
					len =atoi(temp_len);
					strcat (Line[bitCount], FieldStrings [bitCount]);
					strncat (Line[bitCount], (char *)HOST_MESSAGE.card_holder_auth_info,len);
			}
		break ;
		
		case bf_62SwitchingData :
			//if(strlen((char *)HOST_MESSAGE.switching_data_len) != 0)	
			if(BitFieldArray[bf_62SwitchingData] == 1)
			{
					strncpy(temp_len,(char *)HOST_MESSAGE.switching_data_len,3);
					len =atoi(temp_len);
					strcat (Line[bitCount], FieldStrings [bitCount]);
					strncat (Line[bitCount], (char *)HOST_MESSAGE.switching_data,len);
			}
		break ;
		
		case bf_70netmanginfocode :
			//if(strlen((char *)HOST_MESSAGE.net_manag_info_code) != 0)	
			if(BitFieldArray[bf_70netmanginfocode] == 1)
			{
					strcat (Line[bitCount], FieldStrings [bitCount]);
					strncat (Line[bitCount], (char *)HOST_MESSAGE.net_manag_info_code,3);
			}
		break ;

		case bf_90origdata :
			//if(strlen((char *)HOST_MESSAGE.orig_data) != 0)	
			if(BitFieldArray[bf_90origdata] == 1)
			{
					strcat (Line[bitCount], FieldStrings [bitCount]);
					strncat (Line[bitCount], (char *)HOST_MESSAGE.orig_data,42);
			}
		break ;

		case bf_96messeccode :
			//if(strlen((char *)HOST_MESSAGE.mess_sec_code) != 0)	
			if(BitFieldArray[bf_96messeccode] == 1)
			{
					strncpy((char *)temp_data,(char *)HOST_MESSAGE.mess_sec_code,8);
					len = strlen((char *)temp_data);
					genutil_bin_to_ascii (temp_data, len,buffer, (len*2));
					strcat (Line[bitCount], FieldStrings [bitCount]);
					strncat (Line[bitCount], (char *)buffer,strlen(buffer));
			}
		break ;

		case bf_100revinsidcode :
			//if(strlen((char *)HOST_MESSAGE.recv_ins_id_code_len) != 0)	
			if(BitFieldArray[bf_100revinsidcode] == 1)
			{
					strncpy(temp_len,(char *)HOST_MESSAGE.recv_ins_id_code_len,2);
					len =atoi(temp_len);
					strcat (Line[bitCount], FieldStrings [bitCount]);
					strncat (Line[bitCount], (char *)HOST_MESSAGE.recv_ins_id_code,len);
			}
		break ;

		
		case bf_102actid1 :
			//if(strlen((char *)HOST_MESSAGE.acc_id_len) != 0)	
			if(BitFieldArray[bf_102actid1] == 1)
			{
					strncpy(temp_len,(char *)HOST_MESSAGE.acc_id_len,2);
					len =atoi(temp_len);
					strcat (Line[bitCount], FieldStrings [bitCount]);
					strncat (Line[bitCount], (char *)HOST_MESSAGE.acc_id,len);
			}
		break ;

		case bf_121CUPSres :
			//if(strlen((char *)HOST_MESSAGE.cup_resv_len) != 0)
			if(BitFieldArray[bf_121CUPSres] == 1)
			{
					strncpy(temp_len,(char *)HOST_MESSAGE.cup_resv_len,3);
					len =atoi(temp_len);
					strcat (Line[bitCount], FieldStrings [bitCount]);
					strncat (Line[bitCount], (char *)HOST_MESSAGE.cup_resv,len);
			}
		break ;

		case bf_122accqinstres :
			//if(strlen((char *)HOST_MESSAGE.accq_inst_resv_len) != 0)	
			if(BitFieldArray[bf_122accqinstres] == 1)
			{
					strncpy(temp_len,(char *)HOST_MESSAGE.accq_inst_resv_len,3);
					len =atoi(temp_len);
					strcat (Line[bitCount], FieldStrings [bitCount]);
					strncat (Line[bitCount], (char *)HOST_MESSAGE.accq_inst_resv,len);
			}
		break ;

		case bf_123issrinstres :
			//if(strlen((char *)HOST_MESSAGE.issu_inst_resv_len) != 0)	
			if(BitFieldArray[bf_123issrinstres] == 1)
			{
					strncpy(temp_len,(char *)HOST_MESSAGE.issu_inst_resv_len,3);
					len =atoi(temp_len);
					strcat (Line[bitCount], FieldStrings [bitCount]);
					strncat (Line[bitCount], (char *)HOST_MESSAGE.issu_inst_resv,len);
			}
		break ;

		case bf_125security_RAI :
			//if(strlen((char *)HOST_MESSAGE.security_rai_len) != 0)	
			if(BitFieldArray[bf_125security_RAI] == 1)
			{
					strncpy(temp_len,(char *)HOST_MESSAGE.security_rai_len,3);
					len =atoi(temp_len);
					strcat (Line[bitCount], FieldStrings [bitCount]);
					if(0 == strncmp((char *)HOST_MESSAGE.security_rai,"TF",2))
					{
						memcpy(security_raitag01,&HOST_MESSAGE.security_rai[5],2);
					    genutil_bin_to_ascii(security_raitag01,2, ascii_value,4);

						strncpy(security_rai,(char *)HOST_MESSAGE.security_rai,5);
						int taglenth = HOST_MESSAGE.security_rai[6];
						sprintf((char *)stagidlen,"%02d",taglenth);
					    strncat(security_rai,ascii_value,2);
						strncat(security_rai,(char *)stagidlen,2);
						strcat(security_rai,(char *)&HOST_MESSAGE.security_rai[7]);
						strncat (Line[bitCount], security_rai,strlen(security_rai));
					}
					else
					{
						strncat (Line[bitCount], (char *)HOST_MESSAGE.security_rai,len);
					}
					
					
			}
		break ;

		case bf_128messuthcode :
			//if(strlen((char *)HOST_MESSAGE.mess_auth_code) != 0)		
			if(BitFieldArray[bf_128messuthcode] == 1)
			{
					strncpy((char *)temp_data,(char *)HOST_MESSAGE.mess_auth_code,8);
					len = strlen((char *)temp_data);
					genutil_bin_to_ascii (temp_data, len,buffer, (len*2));
					strcat (Line[bitCount], FieldStrings [bitCount]);
					strncat (Line[bitCount], (char *)buffer,strlen(buffer));
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
   BYTE                  ReqRsp_Buffer_hex[17]={0};
   BYTE                  bitmap;
   INT                   i= 0;
   BYTE                  ErrorMsg[100]={0};

   /* Initialize the HOST_MESSAGE structure. */
   memset(&HOST_MESSAGE, 0x00, sizeof(HOST_MESSAGE));

   // Header len is 46 for success header
   if (0x2E == ReqRsp_Buffer[4])
   {
      //Txn_Is_A_Reject = false;
      //hdr_size = CUP_HDR_SIZE_SUCC;

	  /* Header length */
      memcpy( HOST_MESSAGE.hdr_len,&ReqRsp_Buffer[4],
      		  MSG_HEADER_LEN );

	  /* Header Flag and Version */
	  memcpy( HOST_MESSAGE.hdr_flag_ver, &ReqRsp_Buffer[5],
	  		   MSG_HEADER_FLAG_VER_LEN);
	  
	  /* Total messsage length */
	  memcpy( HOST_MESSAGE.tot_msg_len,&ReqRsp_Buffer[6], 
	  		 TOL_MEG_LEN);
	  
	  /* Destination ID*/
	  memcpy( HOST_MESSAGE.dest_stat_id, &ReqRsp_Buffer[10],
	  		  DEST_ID_LEN);

	  /*Source ID */
	  memcpy( HOST_MESSAGE.source_id, &ReqRsp_Buffer[21],
			  SOURCE_ID_LEN);
	  
	  /* Reserve for use*/
	  memcpy( HOST_MESSAGE.res_for_use,&ReqRsp_Buffer[32],
	  		  RESV_FOR_USE_LEN);

	  /*Batch Number*/
	  memcpy( HOST_MESSAGE.batch_no,&ReqRsp_Buffer[35],
	  		   BATCH_NO_LEN);

	  /* Transaction Information */
	  memcpy( HOST_MESSAGE.tran_info, &ReqRsp_Buffer[36],
	  		  TRAN_INFO_LEN);
	  
	  /* User Info  */
	  memcpy( HOST_MESSAGE.user_info,&ReqRsp_Buffer[44],
	  		  USER_INFO_LEN);
	  /*Reject code*/
	  memcpy( HOST_MESSAGE.rej_code, &ReqRsp_Buffer[45],
	  			REJECT_CODE_LEN);

	  Txn_Is_A_Reject = false;

	  if(0 == strncmp((char  *)HOST_MESSAGE.rej_code,"00000",REJECT_CODE_LEN))
	  {
	  	  reqidx =  CUP_HDR_SIZE_SUCC + 4;       /* point to message type */
	  }
	  else
	  {
	  	// Error rejected code is their
	  	reqidx =  CUP_HDR_SIZE_SUCC + CUP_HDR_SIZE_SUCC + 4;
		/*Find message Type*/
	    memcpy(HOST_MESSAGE.msgtype, &ReqRsp_Buffer[reqidx], 4);
       	strcpy( err_buf, "Received header with message type:  " );
      	strcat( err_buf,(char  *) HOST_MESSAGE.msgtype );
	  	strcat( err_buf, " reject code: " );
	  	strncat( err_buf, (char *)&ReqRsp_Buffer[45],REJECT_CODE_LEN);
		strcat( err_buf, " parse_host_msg ");
	  	return false;
	  }

	  /*Find message Type*/
	  memcpy(HOST_MESSAGE.msgtype, &ReqRsp_Buffer[reqidx], 4);
      reqidx = reqidx +4;

  
   	}
    else if (0 == strncmp( (char *)ReqRsp_Buffer, "0000", 4))
	{
		return false;
	}
    else
   	{
	   strcpy( (char *)ErrorMsg, "Unknown header type :" );
	 //  log_message( 1, 3, ErrorMsg, "parse_host_msg" );	
	   Txn_Is_A_Reject = true;
   	}


   bitmap = ReqRsp_Buffer[reqidx];
   bitmapidx = reqidx;

   /* Point past bitmap, but 1st check for extended bitmap. */
   
   if ( bitmap &  BITTAB[bf_1xbitfield].bit_mask )
   {
	   genutil_bin_to_ascii ((BYTE *)&ReqRsp_Buffer[reqidx], EXTENDED_BITMAP_SIZE, astr_bitmap, sizeof(astr_bitmap));
      reqidx = reqidx + 16;                         /* Extended     */
	 
	  /*ebcdic_to_hex(&ReqRsp_Buffer[bitmapidx],ReqRsp_Buffer_hex, 32 );*/
	  
      BitFieldSize = EXTENDED_BITFIELD;
   }
   else
   {
	   genutil_bin_to_ascii ((BYTE *)&ReqRsp_Buffer[reqidx], NORMAL_BITFIELD, astr_bitmap, sizeof(astr_bitmap));
     reqidx = reqidx + 8;                         /* Not Extended */
	 
	  /*ebcdic_to_hex(&ReqRsp_Buffer[bitmapidx],ReqRsp_Buffer_hex, 16 );*/
      BitFieldSize = NORMAL_BITFIELD;
   }

  
   /*------------------------------------------------------------------
      For every bitfield present in message ( except first bit ) . . .
   -------------------------------------------------------------------*/
   for( bfidx = 2; bfidx <= BitFieldSize; bfidx++ )
   {
      //if ( ReqRsp_Buffer_hex[BITTAB[bfidx].byte_no] &
	   if ( ReqRsp_Buffer[bitmapidx + BITTAB[bfidx].byte_no] &
           BITTAB[bfidx].bit_mask )
      {
         /* Bit is on, field is there.... */
         pBF = Find_Bitfield_Table((enum bitfields)bfidx);

         if ( NULL == pBF )
         {
            sprintf( err_buf, "Unexpected bitfield (%d) in message from host.",
                     bfidx );
            strcat( err_buf, " MsgType = " );
            strncat( err_buf, (char *)HOST_MESSAGE.msgtype, 4 );
            ret_val = false; /* Invalid field in message - so quit. */
            break;
         }
		 else{
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
               strncat(err_buf,(char *)HOST_MESSAGE.msgtype, 4 );
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
		strncat((char*)iccdata,"AGNS",4);
		strncat((char*)iccdata,"0001",4);
	
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
	CUPInfo CUP;
	CUP.DoModal();
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
