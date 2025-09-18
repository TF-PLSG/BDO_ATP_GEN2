
// Diners_ParserDlg : implementation file
//

#include "stdafx.h"
#include "Pulse_Parser.h"
#include "Pulse_ParserDlg.h"
#include "afxdialogex.h"
#include "PulseParser.h"
#include "BASICTYP.H"
#include <math.h>
#ifdef _DEBUG
#define new DEBUG_NEW
#endif




void	ascii_to_bin (LPBYTE src, LPBYTE dst, INT len);
INT		ncpul_Move_IfThere( enum bitfields fieldid, BYTE p_trantype );
struct	bitfield_data *ncpul_Find_Bitfield_Table( enum bitfields bitfield );
INT		ncpul_RspMovevData (enum bitfields fieldid);
INT		ncpul_vDataMove(enum bitfields fieldid);
INT		ncpul_GenericMove(enum bitfields fieldid);
INT		ncpul_Move_Always( enum bitfields bitfield, BYTE p_trantype );
INT		ncpul_Move_IfThere( enum bitfields fieldid, BYTE p_trantype );
INT		ncpul_RspMoveGeneric(enum bitfields fieldid); 
INT		ncpul_RspMovevData (enum bitfields fieldid);
void    parsed_mssg();
INT     parse_host_msg( pCHAR err_buf );
BYTE	ascii_ebcdic_conv(BYTE conv_char, char * conv_type );
BYTE	ascii_ebcdic_conv_trk1( unsigned char conv_char, char *conv_type,unsigned char fldSep);
void	genutil_EMV_To_Auth_Tx( BYTE * emv_raw, INT total_len, pAUTH_TX p_auth_tx );
void	genutil_bcd_to_asc (BYTE bstring[], INT blen, pCHAR astr, INT astr_len);
INT		ncpul_get_variable_length( unsigned char *DataField, int FieldLen );
INT		ncpul_parse_host_msg( pCHAR err_buf );
void genutil_hex_to_bin (pCHAR afield, INT how_many, BYTE bstring[]);



pAUTH_TX p_auth_tx;
AUTH_TX AUTH_TX_V;
EMV EMV_details;

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




void Generate_EMV_Details();
INT ncdci_Move_IfThere( enum bitfields fieldid, BYTE p_trantype );
struct bitfield_data *ncdci_Find_Bitfield_Table( enum bitfields bitfield );
 int ncdci_vDataMove(enum bitfields fieldid);
 int ncdci_get_variable_length( unsigned char *DataField, int FieldLen );
 int ncdci_RspMovevData (enum bitfields fieldid);
 INT ncdci_Move_Always( enum bitfields bitfield, BYTE p_trantype );
 INT ncdci_GenericMove(enum bitfields fieldid);
 INT ncdci_RspMoveGeneric(enum bitfields fieldid);
 INT ncdci_parse_host_msg( pCHAR err_buf );

 BYTE BitMap[64] = {0};
 char astr[64] = {0};

 /* Miscellaneous */
#define  PULSE_COMM_BUF_SZE          				1500
#define  PULSE_EXTENDED_BITFIELD_SIZE  				128
#define  PULSE_EXTENDED_BITMAP_SIZE					16
#define  PULSE_NORMAL_BITFIELD_SIZE    				64
#define  PULSE_NORMAL_BITMAP_SIZE					8
#define  PULSE_LOOKUP_KEY_SIZE         				22
#define  PULSE_TOTALL_MEG_LEN						4
#define  PULSE_ETX_TRN_TEXT							"0x03"
#define  PULSE_KEY_SIZE								48

 /*Message Type*/
#define PULSE_MESSAGE_TYPE_SIZE               			4
#define PULSE_NETWORK_REQ_MESSAGE						"0800"
#define PULSE_NETWORK_RESP_MESSAGE						"0810"
#define PULSE_SALE_REQ_MESSAGE							"0100"
#define PULSE_SALE_RESP_MESSAGE						"0110"
#define PULSE_ADVICE_REQ_MESSAGE						"0120"
#define PULSE_ADVICE_RESP_MESSAGE						"0130"
#define PULSE_REVERSAL_REQ_MESSAGE						"0420"
#define PULSE_REVERSAL_RESP_MESSAGE					"0430"
#define PULSE_ISO_SALE_REQ_MESSAGE_TYPE				 "0200"
#define PULSE_ISO_SALE_RESP_MESSAGE_TYPE				 "0210"
#define PULSE_ISO_AUTH_REQ_MESSAGE_TYPE				 "0100"
#define PULSE_ISO_AUTH_RESP_MESSAGE_TYPE				 "0110"
#define PULSE_ISO_REVESAL_REQ_MESSAGE_TYPE				 "0400"
#define PULSE_ISO_REVESAL_RESP_MESSAGE_TYPE			 "0410"
#define PULSE_ISO_SALE_COMPLETION_MESSAGE_TYPE			 "0220"
#define PULSE_FILE_UPDATE_REQ_MESSAGE_TYPE				"0304"
#define PULSE_FILE_UPDATE_RESP_MESSAGE_TYPE			 "0314"
#define PULSE_NEGATIVE_FILE_UPDATE_REQ_MESSAGE_TYPE	"0324"
#define PULSE_NEGATIVE_FILE_UPDATE_RESP_MESSAGE_TYPE	"0334"

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
"009 convRateSettlement		    : ",
"010 convRateCardHolder				: ",
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
"024 Functioncode				: ",
"025 POSConditionCode			: ",
"026 POSPINCode				: ",
"027 AuthIdLen				: ",
"028 TxnFeeAmount			: ",
"029 SettleFeeAmount		: ",
"030 OriginalAmount		: ",
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
"044 AddRespData				: ",
"045 Track1					: ",
"046 Additional_Fees				: ",
"047 AdditionalData			: ",
"048 Merch_name			: ",
"049 TxnCurrency				: ",
"050 SettleCurrency				: ",
"051 Card_curr_cd				: ",
"052 PINData				: ",
"053 SecurityInfo				: ",
"054 AddAmounts				: ",
"055 IccData				: ",
"056 Rplcment_additional_fee				: ",
"057 Auth_life_cycle				: ",
"058 National_pos_cc				: ",
"059 Nationalposgeogrphic_data				: ",
"060 Advice_reason_code 				: ",
"061 Acq_transport_data			: ",
"062 Iss_transport_data				: ",
"063 Pulse_data				: ",
"064 MsgAuthCode			: ",
"065 Field 65               : ",
"066 SettlementCode			: ",
"067 PaymentCodeEx			: ",
"068 RecInstCntrCode		: ",
"069 SettlInstCntrCode		: ",
"070 NetworkInfCode			: ",
"071 MessageNo				: ",
"072 Datarecord			: ",
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
"092 TxorigIntcntrycode			: ",
"093 TxnDestination_ICC			: ",
"094 TxnOriginator_ICC			: ",
"095 ReplacementAmounts		: ",
"096 Msg_security_code			: ",
"097 AmountNetSettle		: ",
"098 Payee					: ",
"099 SettleIdCode			: ",
"100 ReceivingIdCode			: ",
"101 Filename				: ",
"102 AccountId1				: ",
"103 AccountId2				: ",
"104 TxnDescription			: ",
"105 Large_pvt_data1				: ",
"106 Field106				: ",
"107 Field107				: ",
"108 Field108				: ",
"109 Sender_data				: ",
"110 Receiver_data				: ",
"111 Additional_data_acq				: ",
"112 Field112				: ",
"113 Auth_agent_inst_id_code				: ",
"114 Country_code				: ",
"115 Field115				: ",
"116 Field116				: ",
"117 Field117				: ",
"118 Field118				: ",
"119 Field119				: ",
"120 Account_qualifiers				: ",
"121 Additional_data_iss				: ",
"122 Sponsor_bank_id				: ",
"123 Avs_check_auth_data			: ",
"124 Info				: ",
"125 Network_Mngment_info				: ",
"126 Iss_trace_data				: ",
"127 Acq_trace_data				: ",
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
   BYTE msgtype       				[4];
   BYTE bitmap1_bin   				[16];
   BYTE bitmap2_bin  				[16];
   BYTE pan_len       				[2];
   BYTE pan           				[19];
   BYTE proccode      				[6];
   BYTE amount        				[12];
   BYTE Settlement_amount			[12];//5
   BYTE ch_bill_amt   				[12];
   BYTE gmt_time      				[10];
   BYTE convRateSettlement			[8];//9
   BYTE convRateCardHolder			[8];//10
   BYTE stan          				[6];
   BYTE time          				[6];// hhmmss
   BYTE date		  				[4];
   BYTE date_expd	  				[4];
   BYTE SettleDate					[4];//15
   BYTE conversionDate				[4];//16
   BYTE captureDate					[4];//17
   BYTE Merch_type					[4];//18
   BYTE Acq_country_code			[3];//19
   BYTE posentry      				[3];//22
   BYTE card_seq_num  				[3];//23
   BYTE Pos_pin_capture_code   		[2];//26
   BYTE Txn_Fee_Amount				[9];//28
   BYTE Settlement_Fee_Amount		[9];//29
   BYTE acq_inst_len  				[2];//32
   BYTE acq_inst_id   				[11];
   BYTE fwd_inst_len  				[2];//33
   BYTE fwd_inst_id   				[11];
   BYTE track2_len    				[2];//35
   BYTE track2        				[37];
   BYTE rrn			  				[12];//37
   BYTE authcode      				[6];//38
   BYTE responsecd    				[2];//39
   BYTE service_code   				[3];//40
   BYTE termid        				[8];//41
   BYTE cardaccid     				[15];//42
   BYTE cardaccname 				[40];//43
   BYTE addl_rsp_len  				[2];
   BYTE addlrspdata   				[25];//44
   BYTE Additional_Fees_len			[3];//46
   BYTE Additional_Fees 			[88];
   BYTE addl_data_len				[3];//47
   BYTE addl_data     				[100];
   BYTE Merch_name_len				[3];//48
   BYTE Merch_name					[25];
   BYTE tran_curr_cd  				[3];//49
   BYTE settlement_curr_cd			[3];//50
   BYTE card_curr_cd				[3];//51
   BYTE pinblock      				[16];//52
   BYTE AddlAmts_len   				[3];//54
   BYTE AddlAmts     				[120];
   BYTE IccData_len   				[3];//55
   BYTE IccData	    				[255];
   BYTE Rplcment_additional_fee_len [3];//56
   BYTE Rplcment_additional_fee		[88];
   BYTE Auth_life_cycle_len			[3];//57
   BYTE Auth_life_cycle				[3];
   BYTE National_pos_cc_len			[3];//58
   BYTE National_pos_cc				[11];
   BYTE Nationalposgeogrphic_datalen[3];//59
   BYTE Nationalposgeographic_data	[17];
   BYTE Advice_reason_code_len		[3];//60
   BYTE	Advice_reason_code			[6];
   BYTE Acq_transport_data_len		[3];//61
   BYTE Acq_transport_data			[100];
   BYTE Iss_transport_data_len		[3];//62
   BYTE Iss_transport_data			[100];
   BYTE Pulse_data_len				[3];//63
   BYTE Pulse_data					[50];
   BYTE Network_Management_info_code[3];//70
   BYTE Original_data				[42];//90
   BYTE File_update_code			[1];//91
   BYTE Replace_amounts				[42];//95
   BYTE Msg_security_code			[8];//96
   BYTE Payee						[25];//98
   BYTE file_name_len				[2];//101
   BYTE file_name					[17];
   BYTE Account_identificaton1_len	[2];//102
   BYTE Account_identificaton1		[28];
   BYTE Account_identificaton2_len	[2];//103
   BYTE Account_identificaton2		[28];
   BYTE Txn_description_len			[3];//104
   BYTE Txn_description				[100];
   BYTE Large_pvt_data1_len			[3];//105
   BYTE Large_pvt_data1				[999];
   BYTE Sender_data_len				[3];//109
   BYTE Sender_data					[255];
   BYTE Receiver_data_len			[3];//110
   BYTE Receiver_data				[255];
   BYTE Additional_data_acq_len		[3];//111
   BYTE Additional_data_acq			[255];
   BYTE Auth_agent_inst_id_code_len [3];//113
   BYTE Auth_agent_inst_id_code	 	[11];
   BYTE Country_code_len			[3];//114
   BYTE Country_code				[3];
   BYTE Account_qualifiers_len		[3];//120
   BYTE Account_qualifiers			[6];
   BYTE Additional_data_iss_len		[3];//121
   BYTE Additional_data_iss			[255];
   BYTE Sponsor_bank_id_len			[3];//122
   BYTE Sponsor_bank_id				[11];
   BYTE Avs_check_auth_data_len		[3];//123
   BYTE Avs_check_auth_data			[255];
   BYTE Info_len					[3];//124
   BYTE Info						[255];
   BYTE Network_Mngment_info_len	[3];//125
   BYTE Network_Mngment_info		[125];
   BYTE Iss_trace_data_len			[3];//126
   BYTE	Iss_trace_data				[100];
   BYTE Acq_trace_data_len			[3];//127
   BYTE	Acq_trace_data				[100];
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
struct	iso_msg_rec ncpul_HOST_MESSAGE;
BYTE  iccdata[256]={0};

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

enum bitfields 
{
   bf_done                 = 0,      /* end of list */
   bf_1xbitfield           = 1,      /* extended bit field */
   bf_2pan                 = 2,      /* Pan */
   bf_3proc                = 3,      /* Processing code */
   bf_4amount              = 4,      /* Transaction Amount */
   bf_5settlmntamt		   = 5,		/*  Settlement_amount*/
   bf_6cardbillamt         = 6,      /* Card holder Billing Amount */
   bf_7trntime             = 7,      /* Transmission date-time */
   bf_9convratesettlement  = 9,      /* Conversion Rate, settlement */
   bf_10convratecardhldr   = 10,      /* Conversion Rate, settlement */
   bf_11stan               = 11,     /* System trace number */
   bf_12time               = 12,     /* Local Transaction time */
   bf_13date               = 13,     /* Local Transaction date */
   bf_14expiry		       = 14,	 /* Expiry date */
   bf_15SettleDate		   = 15,     /* settlement date */
   bf_16conversiondate	   = 16,     /* Conversion date */
   bf_17capturedate		   = 17,	 /* Capture date   */
   bf_18merchtype		   = 18,     /* Merchant type */
   bf_19acqcntrycode       = 19,	/*  Acquirer country code */
   bf_22posentry           = 22,     /* Pos entry mode  */
   bf_23csn                = 23,	 /* Card Sequence Number */
   bf_26pospincapturecode  = 26,	 /* Point of service pin capture Code (Abc) */
   bf_28txnfeeamount       = 28,	 /* Transaction fee amount */
   bf_29settlementfeeamt   = 29,     /* Settlement fee amount */
   bf_32acqidcode          = 32,     /* Acquiring institution's ID code  */
   bf_33fwdinstidcode      = 33,     /* Forwarding institution ID code */
   bf_35track2             = 35,     /* Track 2 data */
   bf_37rrn	               = 37,	 /* Retrieval ref number */
   bf_38authcode           = 38,     /* Auth identification response/code */
   bf_39respcode           = 39,     /* Action code */
   bf_40service_code	   = 40,     /* Service Code (CVV2)*/
   bf_41termid             = 41,     /* Terminal id  */
   bf_42cardacc            = 42,     /* Card acceptor id  */
   bf_43cardacceptloc      = 43,     /* Card acceptor name/location */
   bf_44respdata	       = 44,     /* Additional response data */
   bf_46additionalfees     = 46,     /* Additional Fees*/
   bf_47addldata           = 47,     /* Additional private data */
   bf_48merchname          = 48,     /* Merchant name */
   bf_49trncurcd           = 49,     /* Transaction currency code */
   bf_50settlmntcurcd	   = 50,     /* settlement currency code */
   bf_51crdcurcd           = 51,     /* Cardholder Billing Currency Code */
   bf_52pinblock           = 52,     /* Pin block */
   bf_54AddlAmount         = 54,     /* Additional amount */
   bf_55IccData		       = 55,     /* IC Card data*/
   bf_56rplcmntaddlfee     = 56,     /* Replacement Additional Fees */
   bf_57authlfecycle	   = 57,     /* Authorization life cycle */
   bf_58nationalposcndtncd = 58,	 /* National Point-Of-Service Condition Code*/
   bf_59nationalposgeodata = 59,     /* National Point-Of-Service geogrphic data  */
   bf_60advicersncode	   = 60,     /* Advice reason code */
   bf_61acqtransprtdata    = 61,     /* Acquirer transport data */
   bf_62isstransprtdata    = 62,     /* Issuer transport data */
   bf_63pulsedata		   = 63,     /* Pulse data */
   bf_70ntwrkmngmntinfocd  = 70,     /* Network managment info code*/
   bf_90originaldata	   = 90,	 /* Original data */
   bf_91fileupdatecode     = 91,     /* File update code */
   bf_95rplcmntamounts     = 95,     /* Replacement amounts */
   bf_96msgsecuritycode    = 96,     /* Message security code */
   bf_98payee			   = 98,     /* payee */
   bf_101filename          = 101,    /* File Name */
   bf_102accidentificaton1 = 102,    /* Account identification 1*/
   bf_103accidentificaton2 = 103,    /*Account identification 2 */
   bf_104txndescription    = 104,    /* Transaction description */
   bf_105largepvtdata1	   = 105,    /* Large private data 1*/
   bf_109senderdata		   = 109,    /* Senders data */
   bf_110receiverdata	   = 110,    /* Receiver data */
   bf_111additionaldataacq = 111,    /* Additional data */
   bf_113authagntinstidcd  = 113,  	 /* Authorizing agent institution id code */
   bf_114contrycode		   = 114,	 /* country code */
   bf_120accqualifiers     = 120, 	 /* account qualifiers */
   bf_121additionaldataiss = 121,    /* additional data issuer */
   bf_122sponsorbankid     = 122,    /* Sponsor bank id */
   bf_123avsauthdata       = 123,    /* Avs/check auth data*/
   bf_124info              = 124,    /* info , text */
   bf_125ntwrkmngmntinfo   = 125,     /* Network Management info */
   bf_126isstracedata      = 126,    /* issuer trace data */
   bf_127acqtracedata      = 127,    /* acquirer trace data */

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

//static  struct bitfield_data *BFData; 
// From PULSE
static  struct bitfield_data BFData[] = {
	{
		bf_2pan,                      		 /* Primary Account Number  */
		ncpul_HOST_MESSAGE.pan_len,          /* data to move */
		2,                           		 /* # of bytes in pan len field */
		ncpul_Move_IfThere,                  /* mandatory */
		ncpul_vDataMove,                   	 /* move routine */
		ncpul_RspMovevData                   /* response routine */
	},
	{
		bf_3proc,                     		 /* field */
		ncpul_HOST_MESSAGE.proccode,         /* data to move */
		6,                            		 /* # of bytes in  iso field */
		ncpul_Move_Always,                 	 /* mandatory */
		ncpul_GenericMove,                   /* move routine */
		ncpul_RspMoveGeneric                 /* response routine */
	},
	{
		bf_4amount,                   		 /* field */
		ncpul_HOST_MESSAGE.amount,           /* data to move */
		12,                           		 /* # of bytes in  iso field */
		ncpul_Move_Always,                   /* mandatory */
		ncpul_GenericMove,                   /* move routine */
		ncpul_RspMoveGeneric                 /* response routine */
	},
	{
		bf_5settlmntamt,                   	  /* field */
		ncpul_HOST_MESSAGE.Settlement_amount, /* data to move */
		12,                           		  /* # of bytes in  iso field */
		ncpul_Move_IfThere,                   /* conditional */
		ncpul_GenericMove,                    /* move routine */
		ncpul_RspMoveGeneric                  /* response routine */
	},
	{
		bf_6cardbillamt,                	 /* field */
		ncpul_HOST_MESSAGE.ch_bill_amt,      /* data to move */
		12,                           		 /* # of bytes in  iso field */
		ncpul_Move_IfThere,                  /* mandatory */
		ncpul_GenericMove,                   /* move routine */
		ncpul_RspMoveGeneric                 /* response routine */
	},
	{
		bf_7trntime,                		 /* field */
		ncpul_HOST_MESSAGE.gmt_time,         /* data to move : MMDDhhmmss*/
		10,                           		 /* # of bytes in  iso field */
		ncpul_Move_Always,                   /* mandatory */
		ncpul_GenericMove,                   /* move routine */
		ncpul_RspMoveGeneric                 /* response routine */
	},
	{
    	bf_9convratesettlement,               /* field */
		ncpul_HOST_MESSAGE.convRateSettlement,/* data to move : MMDDhhmmss*/
		8,                           		  /* # of bytes in  iso field */
		ncpul_Move_IfThere,                   /* conditional */
		ncpul_GenericMove,                    /* move routine */
		ncpul_RspMoveGeneric                  /* response routine */
	},
	{
		bf_10convratecardhldr,                /* field */
		ncpul_HOST_MESSAGE.convRateCardHolder,/* data to move : MMDDhhmmss*/
		8,                           		  /* # of bytes in  iso field */
		ncpul_Move_IfThere,                   /* conditional */
		ncpul_GenericMove,                    /* move routine */
		ncpul_RspMoveGeneric                  /* response routine */
	},
	{
		bf_11stan,                    		/* field */
		ncpul_HOST_MESSAGE.stan,            /* data to move */
		6,                            		/* # of bytes in  iso field */
		ncpul_Move_Always,                  /* Mandatory */
		ncpul_GenericMove,                  /* move routine */
		ncpul_RspMoveGeneric                /* response routine */
	},
	{
		bf_12time,                      	/* field */
		ncpul_HOST_MESSAGE.time,            /* data to move */
		6,                             		/* # of bytes in  iso field */
		ncpul_Move_Always,                  /* Mandatory */
		ncpul_GenericMove,                  /* move routine */
		ncpul_RspMoveGeneric                /* response routine */
	},
	{
		bf_13date,                    		/* field */
		ncpul_HOST_MESSAGE.date,            /* effective data to move */
		4,                            		/* # of bytes in  iso field */
		ncpul_Move_Always,                  /* Mandatory */
		ncpul_GenericMove,                  /* move routine */
		ncpul_RspMoveGeneric                /* response routine */
	},
	{
		bf_14expiry,                  		/* field */
		ncpul_HOST_MESSAGE.date_expd,       /* data to move */
		4,                            		/* # of bytes in field */
		ncpul_Move_IfThere,                 /* conditional */
		ncpul_GenericMove,                  /* move routine */
		ncpul_RspMoveGeneric                /* response routine */
	},
	{
		bf_15SettleDate,                  	/* field */
		ncpul_HOST_MESSAGE.SettleDate,      /* data to move */
		4,                            		/* # of bytes in field */
		ncpul_Move_Always,                  /* Mandatory */
		ncpul_GenericMove,                  /* move routine */
		ncpul_RspMoveGeneric                /* response routine */
	},
	{
	    bf_16conversiondate,                /* field */
		ncpul_HOST_MESSAGE.conversionDate,  /* data to move */
		4,                                  /* # of bytes in field */
		ncpul_Move_IfThere,                 /* conditional */
		ncpul_GenericMove,                  /* move routine */
		ncpul_RspMoveGeneric                /* response routine */
	},
	{
		bf_17capturedate,                   /* field */
		ncpul_HOST_MESSAGE.captureDate,     /* data to move */
		4,                                  /* # of bytes in field */
		ncpul_Move_IfThere,                 /* Optional */
		ncpul_GenericMove,                  /* move routine */
		ncpul_RspMoveGeneric                /* response routine */
	},
	{
		bf_18merchtype,                     /* field */
		ncpul_HOST_MESSAGE.Merch_type,      /* data to move */
		4,                                  /* # of bytes in field */
		ncpul_Move_IfThere,                 /* conditional */
		ncpul_GenericMove,                  /* move routine */
		ncpul_RspMoveGeneric                /* response routine */
	},
	{
		bf_19acqcntrycode,                   /* field */
		ncpul_HOST_MESSAGE.Acq_country_code, /* data to move */
		3,                                   /* # of bytes in field */
		ncpul_Move_IfThere,                  /* conditional */
		ncpul_GenericMove,                   /* move routine */
		ncpul_RspMoveGeneric                 /* response routine */
	},
	{
		bf_22posentry,                		/* field */
		ncpul_HOST_MESSAGE.posentry,        /* data to move */
		3,                           		/* # of bytes in  iso field */
		ncpul_Move_IfThere,                 /* conditional */
		ncpul_GenericMove,                  /* move routine */
		ncpul_RspMoveGeneric                /* response routine */
	},
	{
		bf_23csn,                  		    /* field */
		ncpul_HOST_MESSAGE.card_seq_num,    /* data to move */
		3,                            	    /* # of bytes in field */
		ncpul_Move_IfThere,                 /* conditional */
		ncpul_GenericMove,                  /* move routine */
		ncpul_RspMoveGeneric                /* response routine */
	},
	{
	    bf_26pospincapturecode,                     /* field */
		ncpul_HOST_MESSAGE.Pos_pin_capture_code,    /* data to move */
		2,                             				/* # of bytes in field */
		ncpul_Move_IfThere,                 	    /* conditional */
		ncpul_GenericMove,                  	    /* move routine */
		ncpul_RspMoveGeneric                	    /* response routine */
	},
	{
		bf_28txnfeeamount,                  	/* field */
		ncpul_HOST_MESSAGE.Txn_Fee_Amount,    	/* data to move */
		9,                             			/* # of bytes in field */
		ncpul_Move_IfThere,                  	/* conditional */
		ncpul_GenericMove,                      /* move routine */
		ncpul_RspMoveGeneric                    /* response routine */
	},
	{
		bf_29settlementfeeamt,                    /* field */
		ncpul_HOST_MESSAGE.Settlement_Fee_Amount, /* data to move */
		9,                             			  /* # of bytes in field */
		ncpul_Move_IfThere,                       /* conditional */
		ncpul_GenericMove,                        /* move routine */
		ncpul_RspMoveGeneric                      /* response routine */
	},
	{
		bf_32acqidcode,                           /* field */
		ncpul_HOST_MESSAGE.acq_inst_len,          /* data to move */
		2,                            		      /* # of bytes in  iso field */
		ncpul_Move_Always,                        /* Mandatory */
		ncpul_vDataMove,                          /* move routine */
		ncpul_RspMovevData                        /* response routine */
	},
	{
		bf_33fwdinstidcode,                       /* field */
		ncpul_HOST_MESSAGE.fwd_inst_len,    	  /* data to move */
		2,                            		      /* # of bytes in  iso field */
		ncpul_Move_IfThere,                       /* conditional */
		ncpul_vDataMove,                          /* move routine */
		ncpul_RspMovevData                		  /* response routine */
	},
	{
		bf_35track2,                 		 	  /* field */
		ncpul_HOST_MESSAGE.track2_len,            /* data to move */
		2,                           		      /* # of bytes in track2 len field */
		ncpul_Move_IfThere,                       /* conditional */
		ncpul_vDataMove,                          /* move routine */
		ncpul_RspMovevData                        /* response routine */
	},
	{
		bf_37rrn,                  			      /* field */
		ncpul_HOST_MESSAGE.rrn,      		      /* data to move */
		12,                           	 		  /* # of bytes in track2 len field */
		ncpul_Move_Always,                  	  /* Mandatory */
		ncpul_GenericMove,                  	  /* move routine */
		ncpul_RspMoveGeneric                	  /* response routine */
	},
	{
		bf_38authcode,                			  /* field */
		ncpul_HOST_MESSAGE.authcode,        	  /* data to move */
		6,                            			  /* # of bytes in  iso field */
		ncpul_Move_IfThere,                 	  /* conditional */
		ncpul_GenericMove,                   	  /* move routine */
		ncpul_RspMoveGeneric                 	  /* response routine */
	},
	{
		bf_39respcode,                			   /* field */
		ncpul_HOST_MESSAGE.responsecd,      	   /* data to move */
		2,                            		       /* # of bytes in  iso field */
		ncpul_Move_IfThere,                 	   /* conditional */
		ncpul_GenericMove,                         /* move routine */
		ncpul_RspMoveGeneric                 	   /* response routine */
	},
	{
		bf_40service_code,                			/* field */
		ncpul_HOST_MESSAGE.service_code,      		/* data to move */
		3,                            				/* # of bytes in  iso field */
		ncpul_Move_IfThere,                 		/* conditional */
		ncpul_GenericMove,                   		/* move routine */
		ncpul_RspMoveGeneric                 		/* response routine */
	},
	{
		bf_41termid,                  				/* field */
		ncpul_HOST_MESSAGE.termid,          		/* data to move */
		8,                            				/* # of bytes in  iso field */
		ncpul_Move_Always,                 			/* Mandatory */
		ncpul_GenericMove,                  	 	/* move routine */
		ncpul_RspMoveGeneric                 		/* response routine */
	},
	{
		bf_42cardacc,                 				/* field */
		ncpul_HOST_MESSAGE.cardaccid,       		/* data to move */
		15,                          		 		/* # of bytes in  iso field */
		ncpul_Move_IfThere,                  		/* conditional */
		ncpul_GenericMove,                   		/* move routine */
		ncpul_RspMoveGeneric                 		/* response routine */
	},
	{
		bf_43cardacceptloc,                 		/* field */
		ncpul_HOST_MESSAGE.cardaccname,       		/* data to move */
		40,                          		 		/* # of bytes in  iso field */
		ncpul_Move_IfThere,                  		/* conditional */
		ncpul_GenericMove,                   		/* move routine */
		ncpul_RspMoveGeneric						/* response routine */
	},
	{
		bf_44respdata,                 				/* field */
		ncpul_HOST_MESSAGE.addl_rsp_len,       		/* data to move */
		2,                          		 		/* # of bytes in  iso field */
		ncpul_Move_IfThere,                  		/* conditional */
		ncpul_vDataMove,                   			/* move routine */
		ncpul_RspMovevData							/* response routine */
	},
	{
		bf_46additionalfees,                 		 /* field */
		ncpul_HOST_MESSAGE.Additional_Fees_len,      /* data to move */
		3,                            				 /* # of bytes in len field */
		ncpul_Move_IfThere,                   		 /* conditional */
		ncpul_vDataMove,                  			 /* move routine */
		ncpul_RspMovevData                		 	 /* response routine */
	},
	{
		bf_47addldata,                				/* field */
		ncpul_HOST_MESSAGE.addl_data_len,  	 		/* data to move */
		3,                            				/* # of bytes in track1 len field */
		ncpul_Move_IfThere,                	 		/* conditional */
		ncpul_vDataMove,                  			/* move routine */
		ncpul_RspMovevData                			/* response routine */
	},
	{
		bf_48merchname,               			 	/* field */
		ncpul_HOST_MESSAGE.Merch_name_len,   		/* data to move */
		3,                            				/* # of bytes in track1 len field */
		ncpul_Move_IfThere,                 		/* conditional */
		ncpul_vDataMove,                  			/* move routine */
		ncpul_RspMovevData                			/* response routine */
	},
	{
		bf_49trncurcd,               				/* field */
		ncpul_HOST_MESSAGE.tran_curr_cd,   			/* data to move */
		3,                           				/* # of bytes in  iso field */
		ncpul_Move_Always,                			/* mandatory */
		ncpul_GenericMove,                 			/* move routine */
		ncpul_RspMoveGeneric						/* response routine */
	},
	{
		bf_50settlmntcurcd,               			/* field */
		ncpul_HOST_MESSAGE.settlement_curr_cd,   	/* data to move */
		3,                           				/* # of bytes in  iso field */
		ncpul_Move_IfThere,               	 		/* mandatory    */
		ncpul_GenericMove,                 			/* move routine */
		ncpul_RspMoveGeneric						/* response routine */
	},
	{
		bf_51crdcurcd,              			 	/* field */
		ncpul_HOST_MESSAGE.card_curr_cd,    		/* data to move */
		3,                           				/* # of bytes in field */
		ncpul_Move_IfThere,                 		/* conditional    */
		ncpul_GenericMove,                   		/* move routine */
		ncpul_RspMoveGeneric                 		/* response routine */
	},
	{
		bf_52pinblock,                				/* field */
		ncpul_HOST_MESSAGE.pinblock,        		/* data to move */
		16,                            				/* # of bytes in  iso field */
		ncpul_Move_IfThere,                  		/* conditional */
		ncpul_GenericMove,                  		/* move routine */
		ncpul_RspMoveGeneric                		/* response routine */
	},
	{
		bf_54AddlAmount,                 			/* field */
		ncpul_HOST_MESSAGE.AddlAmts_len,   			/* data to move */
		3,                            		/* # of bytes in field */
		ncpul_Move_IfThere,                  /* conditional    */
		ncpul_vDataMove,                  /* move routine */
		ncpul_RspMovevData                /* response routine */
	},
	{
		bf_55IccData,                 		/* field */
		ncpul_HOST_MESSAGE.IccData_len,   /* data to move */
		3,                           		 /* # of bytes in field */
		ncpul_Move_IfThere,                  /* conditional    */
		ncpul_vDataMove,                  /* move routine */
		ncpul_RspMovevData                /* response routine */
	},
	{
		bf_56rplcmntaddlfee,                			 /* field */
		ncpul_HOST_MESSAGE.Rplcment_additional_fee_len,   /* data to move */
		3,                            						/* # of bytes in field */
		ncpul_Move_IfThere,                 			 /* conditional    */
		ncpul_vDataMove,                  				/* move routine */
		ncpul_RspMovevData                				/* response routine */
	},
	{
		bf_57authlfecycle,                			 /* field */
		ncpul_HOST_MESSAGE.Auth_life_cycle_len,   /* data to move */
		3,                            			/* # of bytes in field */
		ncpul_Move_IfThere,                  		/* conditional    */
		ncpul_vDataMove,                  		/* move routine */
		ncpul_RspMovevData                		/* response routine */
	},
	{
		bf_58nationalposcndtncd,                 /* field */
		ncpul_HOST_MESSAGE.National_pos_cc_len,   /* data to move */
		3,                            			/* # of bytes in field */
		ncpul_Move_Always,                  /* mandatory    */
		ncpul_vDataMove,                   /* move routine */
		ncpul_RspMovevData                 	/* response routine */
	},
	{
		bf_59nationalposgeodata,                 				/* field */
		ncpul_HOST_MESSAGE.Nationalposgeogrphic_datalen,   /* data to move */
		3,                            						/* # of bytes in field */
		ncpul_Move_IfThere,                 			 /* conditional    */
		ncpul_vDataMove,                  				/* move routine */
		ncpul_RspMovevData                				/* response routine */
	},
	{
		bf_60advicersncode,                 		/* field */
		ncpul_HOST_MESSAGE.Advice_reason_code_len,   /* data to move */
		3,                           				 /* # of bytes in field */
		ncpul_Move_IfThere,                 		 /* conditional    */
		ncpul_vDataMove,                 			 /* move routine */
		ncpul_RspMovevData                			/* response routine */
	},
	{
		bf_61acqtransprtdata,                 			/* field */
		ncpul_HOST_MESSAGE.Acq_transport_data_len,   /* data to move */
		3,                            					/* # of bytes in field */
		ncpul_Move_IfThere,                 		 /* conditional    */
		ncpul_vDataMove,                 		 	/* move routine */
		ncpul_RspMovevData               			 /* response routine */
	},
	{
		bf_62isstransprtdata,                	 /* field */
		ncpul_HOST_MESSAGE.Iss_transport_data_len,   /* data to move */
		3,                            				/* # of bytes in field */
		ncpul_Move_IfThere,                  		/* conditional    */
		ncpul_vDataMove,                  			/* move routine */
		ncpul_RspMovevData                			/* response routine */
	},
	{
		bf_63pulsedata,                 		/* field */
		ncpul_HOST_MESSAGE.Pulse_data_len,   /* data to move */
		3,                            		/* # of bytes in field */
		ncpul_Move_Always,                  /* mandatory    */
		ncpul_vDataMove,                   /* move routine */
		ncpul_RspMovevData                 /* response routine */
	},
	{
		bf_70ntwrkmngmntinfocd,                 			/* field */
		ncpul_HOST_MESSAGE.Network_Management_info_code,   /* data to move */
		3,                            					/* # of bytes in field */
		ncpul_Move_IfThere,                  			/* mandatory    */
		ncpul_GenericMove,                  		/* move routine */
		ncpul_RspMoveGeneric                		/* response routine */
	},
	{
		bf_90originaldata,               		 /* field */
		ncpul_HOST_MESSAGE.Original_data,    /* data to move */
		42,                            			/* # of bytes in field */
		ncpul_Move_IfThere,                  /* mandatory    */
		ncpul_GenericMove,                   /* move routine */
		ncpul_RspMoveGeneric                 /* response routine */              		/* response routine */
	},
	{
		bf_91fileupdatecode,                	/* field */
		ncpul_HOST_MESSAGE.File_update_code,    /* data to move */
		1,                            			/* # of bytes in field */
		ncpul_Move_IfThere,                  /* mandatory    */
		ncpul_GenericMove,                   /* move routine */
		ncpul_RspMoveGeneric                 /* response routine */              		/* response routine */
	},
	{
		bf_95rplcmntamounts,                	/* field */
		ncpul_HOST_MESSAGE.Replace_amounts,    /* data to move */
		42,                            			/* # of bytes in  iso field */
		ncpul_Move_IfThere,                 		/* conditional    */
		ncpul_GenericMove,               			/* move routine */
		ncpul_RspMoveGeneric                	/* response routine */
	},
	{
		bf_96msgsecuritycode,               	 /* field */
		ncpul_HOST_MESSAGE.Msg_security_code,    /* data to move */
		8,                            			/* # of bytes in  iso field */
		ncpul_Move_IfThere,                 		/* mandatory    */
		ncpul_GenericMove,               				/* move routine */
		ncpul_RspMoveGeneric                		/* response routine */
	},
	{
		bf_98payee,                				/* field */
		ncpul_HOST_MESSAGE.Payee,    			/* data to move */
		25,                            		/* # of bytes in  iso field */
		ncpul_Move_IfThere,                 		/* conditional    */
		ncpul_GenericMove,               		/* move routine */
		ncpul_RspMoveGeneric                /* response routine */
	},
	{
		bf_101filename,                			/* field */
		ncpul_HOST_MESSAGE.file_name_len,    /* data to move */
		2,                            		/* # of bytes in  iso field */
		ncpul_Move_IfThere,                 		/* mandatory    */
		ncpul_vDataMove,               		/* move routine */
		ncpul_RspMovevData                	/* response routine */
	},
	{
		bf_102accidentificaton1,                			/* field */
		ncpul_HOST_MESSAGE.Account_identificaton1_len,   	 /* data to move */
		2,                            						/* # of bytes in  iso field */
		ncpul_Move_IfThere,                 				/* conditional    */
		ncpul_vDataMove,               						/* move routine */
		ncpul_RspMovevData                					/* response routine */
	},
	{
		bf_103accidentificaton2,                			/* field */
		ncpul_HOST_MESSAGE.Account_identificaton2_len,    /* data to move */
		2,                            					/* # of bytes in  iso field */
		ncpul_Move_IfThere,                 				/* conditional    */
		ncpul_vDataMove,               					/* move routine */
		ncpul_RspMovevData                			/* response routine */
	},
	{
		bf_104txndescription,                		/* field */
		ncpul_HOST_MESSAGE.Txn_description_len,    /* data to move */
		3,                            				/* # of bytes in  iso field */
		ncpul_Move_IfThere,                 		/* conditional    */
		ncpul_vDataMove,               				/* move routine */
		ncpul_RspMovevData                			/* response routine */
	},
	{
		bf_105largepvtdata1,                			/* field */
		ncpul_HOST_MESSAGE.Large_pvt_data1_len,    /* data to move */
		3,                            				/* # of bytes in  iso field */
		ncpul_Move_IfThere,                 		/* conditional    */
		ncpul_vDataMove,               				/* move routine */
		ncpul_RspMovevData                			/* response routine */
	},
	{
		bf_109senderdata,               			 /* field */
		ncpul_HOST_MESSAGE.Sender_data_len, 	   /* data to move */
		3,                            				/* # of bytes in  iso field */
		ncpul_Move_IfThere,                 		/* conditional    */
		ncpul_vDataMove,               				/* move routine */
		ncpul_RspMovevData                			/* response routine */
	},
	{
		bf_110receiverdata,                			/* field */
		ncpul_HOST_MESSAGE.Receiver_data_len,    /* data to move */
		3,                            				/* # of bytes in  iso field */
		ncpul_Move_IfThere,                 		/* conditional    */
		ncpul_vDataMove,               				/* move routine */
		ncpul_RspMovevData               			 /* response routine */
	},
	{
		bf_111additionaldataacq,               			 /* field */
		ncpul_HOST_MESSAGE.Additional_data_acq_len,    /* data to move */
		3,                            					/* # of bytes in  iso field */
		ncpul_Move_IfThere,                 		/* conditional    */
		ncpul_vDataMove,               				/* move routine */
		ncpul_RspMovevData                			/* response routine */
	},
	{
		bf_113authagntinstidcd,               			 /* field */
		ncpul_HOST_MESSAGE.Auth_agent_inst_id_code_len,    /* data to move */
		3,                            					/* # of bytes in  iso field */
		ncpul_Move_IfThere,                 		/* conditional    */
		ncpul_vDataMove,               				/* move routine */
		ncpul_RspMovevData                				/* response routine */
	},
	{
		bf_114contrycode,                		/* field */
		ncpul_HOST_MESSAGE.Country_code_len,    /* data to move */
		3,                            			/* # of bytes in  iso field */
		ncpul_Move_IfThere,                 		/* conditional    */
		ncpul_vDataMove,               			/* move routine */
		ncpul_RspMovevData                	/* response routine */
	},
	{
		bf_120accqualifiers,                		/* field */
		ncpul_HOST_MESSAGE.Account_qualifiers_len,    /* data to move */
		3,                            				/* # of bytes in  iso field */
		ncpul_Move_IfThere,                 		/* conditional    */
		ncpul_vDataMove,               				/* move routine */
		ncpul_RspMovevData                			/* response routine */
	},
	{
		bf_121additionaldataiss,                		/* field */
		ncpul_HOST_MESSAGE.Additional_data_iss_len,    /* data to move */
		3,                            					/* # of bytes in  iso field */
		ncpul_Move_IfThere,                 			/* optional    */
		ncpul_vDataMove,               					/* move routine */
		ncpul_RspMovevData                				/* response routine */
	},
	{
		bf_122sponsorbankid,                		/* field */
		ncpul_HOST_MESSAGE.Sponsor_bank_id_len,    /* data to move */
		3,                            				/* # of bytes in  iso field */
		ncpul_Move_IfThere,                 		/* conditional    */
		ncpul_vDataMove,               				/* move routine */
		ncpul_RspMovevData               		 /* response routine */
	},
	{
		bf_123avsauthdata,                				/* field */
		ncpul_HOST_MESSAGE.Avs_check_auth_data_len,    /* data to move */
		3,                            					/* # of bytes in  iso field */
		ncpul_Move_IfThere,                 		/* conditional    */
		ncpul_vDataMove,               				/* move routine */
		ncpul_RspMovevData                			/* response routine */
	},
	{
		bf_124info,               				 /* field */
		ncpul_HOST_MESSAGE.Info_len,    		/* data to move */
		3,                            			/* # of bytes in  iso field */
		ncpul_Move_IfThere,                 		/* conditional    */
		ncpul_vDataMove,               			/* move routine */
		ncpul_RspMovevData                		/* response routine */
	},
	{
		bf_125ntwrkmngmntinfo,                			/* field */
		ncpul_HOST_MESSAGE.Network_Mngment_info_len,    /* data to move */
		3,                            					/* # of bytes in  iso field */
		ncpul_Move_IfThere,                 			/* conditional    */
		ncpul_vDataMove,               					/* move routine */
		ncpul_RspMovevData                				/* response routine */
	},
	{
		bf_126isstracedata,               			 /* field */
		ncpul_HOST_MESSAGE.Iss_trace_data_len,    	/* data to move */
		3,                            				/* # of bytes in  iso field */
		ncpul_Move_IfThere,                 		/* conditional    */
		ncpul_vDataMove,               				/* move routine */
		ncpul_RspMovevData                			/* response routine */
	},
	{
		bf_127acqtracedata,                			/* field */
		ncpul_HOST_MESSAGE.Acq_trace_data_len,   	 /* data to move */
		3,                            				/* # of bytes in  iso field */
		ncpul_Move_IfThere,                 		/* conditional    */
		ncpul_vDataMove,               				/* move routine */
		ncpul_RspMovevData                			/* response routine */
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

	memset(Line,0,sizeof(Line));
	
	//sprintf(Line[bitCount++],  "BITMAP			:  %s",astr); 
	memset(&AUTH_TX_V,0x00,sizeof(AUTH_TX_V));
	memset(&EMV_details,0x00,sizeof(EMV_details));

	strcpy (Line[0], "000 MsgType				: ");
	strncat(Line[0],(char *)ncpul_HOST_MESSAGE.msgtype,4);

	if(strlen((char *)ncpul_HOST_MESSAGE.bitmap1_bin) != 0 && strlen((char *)ncpul_HOST_MESSAGE.bitmap2_bin) != 0 )
	{
		strcpy (Line[1], "001 BitMap							: ");
		for (icnt = 0; icnt < 8; icnt++)
		{
			sprintf(&Line[1][31 + 3*icnt],"%x%x ", ncpul_HOST_MESSAGE.bitmap1_bin[icnt]>>4,
						((BYTE)(ncpul_HOST_MESSAGE.bitmap1_bin[icnt]<<4))>>4);
		}

		for (icnt = 0; icnt < 8; icnt++)
		{
			sprintf(&Line[1][55 + 3*icnt],"%x%x ", ncpul_HOST_MESSAGE.bitmap2_bin[icnt]>>4,
							((BYTE)(ncpul_HOST_MESSAGE.bitmap2_bin[icnt]<<4))>>4);
		}
	}

	startCharacter = 1;
	LastDisplayBit = 1;
	char temp_len[4]={0};										// This is the bit that was displayed last.
	sprintf(Line[1],  "Bitmap					:  %s",astr); 
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
			if(strlen((char *)ncpul_HOST_MESSAGE.pan_len) != 0)                     // card number  len
			{	
				memset(temp_len,0x00,sizeof(temp_len));
				strncpy(temp_len,(char *)ncpul_HOST_MESSAGE.pan_len,2); 
				len =atoi(temp_len);
				strcat (Line[bitCount], FieldStrings [bitCount]);
				strncat (Line[bitCount], (char *)ncpul_HOST_MESSAGE.pan,len);				
			}	
		break;
		
		case bf_3proc :
			if(strlen((char *)ncpul_HOST_MESSAGE.proccode) != 0)
			{
				strcat (Line[bitCount], FieldStrings [bitCount]);
				strncat (Line[bitCount], (char *)ncpul_HOST_MESSAGE.proccode, 6);				
			}
		break;
		
		case bf_4amount :
			if(strlen((char *)ncpul_HOST_MESSAGE.amount) != 0)
			{
				strcat (Line[bitCount], FieldStrings [bitCount]);
				strncat (Line[bitCount], (char *)ncpul_HOST_MESSAGE.amount, 12);
			}
		break;
			
		case bf_5settlmntamt :
			if(strlen((char *)ncpul_HOST_MESSAGE.Settlement_amount) != 0)
			{
				strcat (Line[bitCount], FieldStrings [bitCount]);
				strncat (Line[bitCount], (char *)ncpul_HOST_MESSAGE.Settlement_amount, 12);
			}
		break;

		case bf_6cardbillamt :
			if(strlen((char *)ncpul_HOST_MESSAGE.ch_bill_amt) != 0)
			{
				strcat (Line[bitCount], FieldStrings [bitCount]);
				strncat (Line[bitCount], (char *)ncpul_HOST_MESSAGE.ch_bill_amt ,12);
			}
			break ;

		case bf_7trntime :
			if(strlen((char *)ncpul_HOST_MESSAGE.gmt_time) != 0)
			{
				strcat (Line[bitCount], FieldStrings [bitCount]);
				strncat (Line[bitCount], (char *)ncpul_HOST_MESSAGE.gmt_time ,10);
			}
		break;
	
		case bf_9convratesettlement :
			if(strlen((char *)ncpul_HOST_MESSAGE.convRateSettlement) != 0)
			{
				strcat (Line[bitCount], FieldStrings [bitCount]);
				strncat (Line[bitCount], (char *)ncpul_HOST_MESSAGE.convRateSettlement ,8);
			}
		break;

		case bf_10convratecardhldr :
			if(strlen((char *)ncpul_HOST_MESSAGE.convRateCardHolder) != 0)
			{
				strcat (Line[bitCount], FieldStrings [bitCount]);
				strncat (Line[bitCount], (char *)ncpul_HOST_MESSAGE.convRateCardHolder ,8);
			}
		break;

		case bf_11stan :
			if(strlen((char *)ncpul_HOST_MESSAGE.stan) != 0)
			{
				strcat (Line[bitCount], FieldStrings [bitCount]);
				strncat (Line[bitCount], (char *)ncpul_HOST_MESSAGE.stan, 6);
			}
		break;
		
		case bf_12time :
			if(strlen((char *)ncpul_HOST_MESSAGE.time) != 0)
			{
				strcat (Line[bitCount], FieldStrings [bitCount]);
				strncat (Line[bitCount], (char *)ncpul_HOST_MESSAGE.time, 6);
			}
		break ;

		case bf_13date :
			if(strlen((char *)ncpul_HOST_MESSAGE.date) != 0)
			{
				strcat (Line[bitCount], FieldStrings [bitCount]);
				strncat (Line[bitCount], (char *)ncpul_HOST_MESSAGE.date, 4);
			}
		break ;

		case bf_14expiry :
			if(strlen((char *)ncpul_HOST_MESSAGE.date_expd) != 0)
			{
				strcat (Line[bitCount], FieldStrings [bitCount]);
				strncat (Line[bitCount], (char *)ncpul_HOST_MESSAGE.date_expd, 4);
			}
		break ;

		case bf_15SettleDate :
			if(strlen((char *)ncpul_HOST_MESSAGE.SettleDate) != 0)
			{
				strcat (Line[bitCount], FieldStrings [bitCount]);
				strncat (Line[bitCount], (char *)ncpul_HOST_MESSAGE.SettleDate, 4);
			}
		break ;

		case bf_16conversiondate :
			if(strlen((char *)ncpul_HOST_MESSAGE.conversionDate) != 0)
			{
				strcat (Line[bitCount], FieldStrings [bitCount]);
				strncat (Line[bitCount], (char *)ncpul_HOST_MESSAGE.conversionDate, 4);
			}
		break ;

		case bf_17capturedate :
			if(strlen((char *)ncpul_HOST_MESSAGE.captureDate) != 0)
			{
				strcat (Line[bitCount], FieldStrings [bitCount]);
				strncat (Line[bitCount], (char *)ncpul_HOST_MESSAGE.captureDate, 4);
			}
		break ;

		case bf_18merchtype :
			if(strlen((char *)ncpul_HOST_MESSAGE.Merch_type) != 0)
			{
				strcat (Line[bitCount], FieldStrings [bitCount]);
				strncat (Line[bitCount], (char *)ncpul_HOST_MESSAGE.Merch_type, 4);
			}
		break ;

		case bf_19acqcntrycode :
			if(strlen((char *)ncpul_HOST_MESSAGE.Acq_country_code) != 0)
			{
				strcat (Line[bitCount], FieldStrings [bitCount]);
				strncat (Line[bitCount], (char *)ncpul_HOST_MESSAGE.Acq_country_code, 3);
			}
		break ;


		case bf_22posentry :
			if(strlen((char *)ncpul_HOST_MESSAGE.posentry) != 0)
			{
				strcat (Line[bitCount], FieldStrings [bitCount]);
				strncat (Line[bitCount], (char *)ncpul_HOST_MESSAGE.posentry, 3);
			}
		break ;
		
		case bf_23csn :
			if(strlen((char *)ncpul_HOST_MESSAGE.card_seq_num) != 0)
			{
				strcat (Line[bitCount], FieldStrings [bitCount]);
				strncat (Line[bitCount], (char *)ncpul_HOST_MESSAGE.card_seq_num, 3);
			}
		break ;

		case bf_26pospincapturecode :
			if(strlen((char *)ncpul_HOST_MESSAGE.Pos_pin_capture_code) != 0)
			{
					strcat (Line[bitCount], FieldStrings [bitCount]);
					strncat (Line[bitCount], (char *)ncpul_HOST_MESSAGE.Pos_pin_capture_code ,2);
			}
		break ;

		case bf_28txnfeeamount :
			if(strlen((char *)ncpul_HOST_MESSAGE.Txn_Fee_Amount) != 0)
			{
					strcat (Line[bitCount], FieldStrings [bitCount]);
					strncat (Line[bitCount], (char *)ncpul_HOST_MESSAGE.Txn_Fee_Amount ,9);
			}
		break ;

		case bf_29settlementfeeamt :
			if(strlen((char *)ncpul_HOST_MESSAGE.Settlement_Fee_Amount) != 0)
			{
					strcat (Line[bitCount], FieldStrings [bitCount]);
					strncat (Line[bitCount], (char *)ncpul_HOST_MESSAGE.Settlement_Fee_Amount ,9);
			}
		break ;

		case bf_32acqidcode :
			if(strlen((char *)ncpul_HOST_MESSAGE.acq_inst_len) != 0)
			{
				memset(temp_len,0x00,sizeof(temp_len));
				strncpy(temp_len,(char *)ncpul_HOST_MESSAGE.acq_inst_len,2); 
				len =atoi(temp_len);
				strcat (Line[bitCount], FieldStrings [bitCount]);
				strncat (Line[bitCount], (char *)ncpul_HOST_MESSAGE.acq_inst_id ,len);
			}
		break ;
		
		case bf_33fwdinstidcode :
			if(strlen((char *)ncpul_HOST_MESSAGE.fwd_inst_len) != 0)
			{
				memset(temp_len,0x00,sizeof(temp_len));
				strncpy(temp_len,(char *)ncpul_HOST_MESSAGE.fwd_inst_len,2);
				len =atoi(temp_len);
				strcat (Line[bitCount], FieldStrings [bitCount]);
				strncat (Line[bitCount], (char *)ncpul_HOST_MESSAGE.fwd_inst_id ,len);
			}
		break ;

		case bf_35track2 :
			if(strlen((char *)ncpul_HOST_MESSAGE.track2_len) != 0)
			{
				memset(temp_len,0x00,sizeof(temp_len));
					strncpy(temp_len,(char *)ncpul_HOST_MESSAGE.track2_len,2);
					len =atoi(temp_len);
					strcat (Line[bitCount], FieldStrings [bitCount]);
					strncat (Line[bitCount], (char *)ncpul_HOST_MESSAGE.track2 ,len);
			}
		break ;

		case bf_37rrn :
			if(strlen((char *)ncpul_HOST_MESSAGE.rrn) != 0)
			{
					strcat (Line[bitCount], FieldStrings [bitCount]);
					strncat (Line[bitCount], (char *)ncpul_HOST_MESSAGE.rrn ,12);
			}
		break ;
		
		case bf_38authcode :
			if(strlen((char *)ncpul_HOST_MESSAGE.authcode) != 0)
			{
					strcat (Line[bitCount], FieldStrings [bitCount]);
					strncat (Line[bitCount], (char *)ncpul_HOST_MESSAGE.authcode ,6);
			}
		break ;
		
		case bf_39respcode :
			if(strlen((char *)ncpul_HOST_MESSAGE.responsecd) != 0)
			{
					strcat (Line[bitCount], FieldStrings [bitCount]);
					strncat (Line[bitCount], (char *)ncpul_HOST_MESSAGE.responsecd,2);
			}
		break ;

		case bf_40service_code :
			if(strlen((char *)ncpul_HOST_MESSAGE.service_code) != 0)
			{
					strcat (Line[bitCount], FieldStrings [bitCount]);
					strncat (Line[bitCount], (char *)ncpul_HOST_MESSAGE.service_code,3);
			}
		break ;
		
		case bf_41termid :
			if(strlen((char *)ncpul_HOST_MESSAGE.termid) != 0)
			{
					strcat (Line[bitCount], FieldStrings [bitCount]);
					strncat (Line[bitCount], (char *)ncpul_HOST_MESSAGE.termid,8);
			}
		break ;
		
		case bf_42cardacc :
			if(strlen((char *)ncpul_HOST_MESSAGE.cardaccid) != 0)
			{
					strcat (Line[bitCount], FieldStrings [bitCount]);
					strncat (Line[bitCount], (char *)ncpul_HOST_MESSAGE.cardaccid,15);
			}
		break ;
		
		case bf_43cardacceptloc :
			if(strlen((char *)ncpul_HOST_MESSAGE.cardaccname) != 0)
			{
					strcat (Line[bitCount], FieldStrings [bitCount]);
					strncat (Line[bitCount], (char *)ncpul_HOST_MESSAGE.cardaccname,40);
			}
		break ;
		
		case bf_44respdata :
			if(strlen((char *)ncpul_HOST_MESSAGE.addl_rsp_len) != 0)			 //addl_rsp_len	
			{
				memset(temp_len,0x00,sizeof(temp_len));
					strncpy(temp_len,(char *)ncpul_HOST_MESSAGE.addl_rsp_len,2);
					len =atoi(temp_len);
					strcat (Line[bitCount], FieldStrings [bitCount]);
					strncat (Line[bitCount], (char *)ncpul_HOST_MESSAGE.addlrspdata,len);
			}
		break ;
		

		case bf_46additionalfees :
			if(strlen((char *)ncpul_HOST_MESSAGE.Additional_Fees_len) != 0)			 //addl_rsp_len	
			{
				memset(temp_len,0x00,sizeof(temp_len));
					strncpy(temp_len,(char *)ncpul_HOST_MESSAGE.Additional_Fees_len,3);
					len =atoi(temp_len);
					strcat (Line[bitCount], FieldStrings [bitCount]);
					strncat (Line[bitCount], (char *)ncpul_HOST_MESSAGE.Additional_Fees,len);
			}
		break ;

		case bf_47addldata :
			if(strlen((char *)ncpul_HOST_MESSAGE.addl_data_len) != 0)			 //addl_rsp_len	
			{
				memset(temp_len,0x00,sizeof(temp_len));
					strncpy(temp_len,(char *)ncpul_HOST_MESSAGE.addl_data_len,3);
					len =atoi(temp_len);
					strcat (Line[bitCount], FieldStrings [bitCount]);
					strncat (Line[bitCount], (char *)ncpul_HOST_MESSAGE.addl_data,len);
			}
		break ;

		case bf_48merchname :
			if(strlen((char *)ncpul_HOST_MESSAGE.Merch_name_len) != 0)
			{
				if(0 == strncmp((char*)ncpul_HOST_MESSAGE.msgtype,"0800",4))
				{
					memset(temp_len,0x00,sizeof(temp_len));
					strncpy(temp_len,(char *)ncpul_HOST_MESSAGE.Merch_name_len,3);
					len = atoi(temp_len);
					memcpy(temp_data,ncpul_HOST_MESSAGE.Merch_name+ 2  , (len -2));
					genutil_bin_to_ascii (temp_data, len,buffer, (len*2));
					strcat (Line[bitCount], FieldStrings [bitCount]);
					strncat(Line[bitCount],"NK",2);
					strncat (Line[bitCount], (char *)buffer,strlen(buffer));
				}
				else
				{
					memset(temp_len,0x00,sizeof(temp_len));
					strncpy(temp_len,(char *)ncpul_HOST_MESSAGE.Merch_name_len,3);
					len =atoi(temp_len);
					strcat (Line[bitCount], FieldStrings [bitCount]);
					strncat (Line[bitCount], (char *)ncpul_HOST_MESSAGE.Merch_name,len);
				}
			}
		break ;
		
		case bf_49trncurcd :
			if(strlen((char *)ncpul_HOST_MESSAGE.tran_curr_cd) != 0)			// tran_curr_cd
			{
					strcat (Line[bitCount], FieldStrings [bitCount]);
					strncat (Line[bitCount], (char *)ncpul_HOST_MESSAGE.tran_curr_cd,3);
			}
		break ;

		case bf_50settlmntcurcd :
			if(strlen((char *)ncpul_HOST_MESSAGE.settlement_curr_cd) != 0)			// tran_curr_cd
			{
					strcat (Line[bitCount], FieldStrings [bitCount]);
					strncat (Line[bitCount], (char *)ncpul_HOST_MESSAGE.settlement_curr_cd,3);
			}
		break ;


		case bf_51crdcurcd :
			if(strlen((char *)ncpul_HOST_MESSAGE.card_curr_cd) != 0)
			{
					strcat (Line[bitCount], FieldStrings [bitCount]);
					strncat (Line[bitCount], (char *)ncpul_HOST_MESSAGE.card_curr_cd,3);
			}
		break ;

		case bf_52pinblock :
			if(strlen((char *)ncpul_HOST_MESSAGE.pinblock) != 0)           // pinblock
			{
					strcat (Line[bitCount], FieldStrings [bitCount]);
					strncat (Line[bitCount], (char *)ncpul_HOST_MESSAGE.pinblock,16);
			}
		break ;
	
		case bf_54AddlAmount :
			if(strlen((char *)ncpul_HOST_MESSAGE.AddlAmts_len) != 0)			// AddlAmts_len
			{
				memset(temp_len,0x00,sizeof(temp_len));
					strncpy(temp_len,(char *)ncpul_HOST_MESSAGE.AddlAmts_len,3);
					len= atoi(temp_len);
					strcat (Line[bitCount], FieldStrings [bitCount]);
					strncat (Line[bitCount], (char *)ncpul_HOST_MESSAGE.AddlAmts,len);
			}
		break ;
		
		case bf_55IccData :
			if(strlen((char *)ncpul_HOST_MESSAGE.IccData_len) != 0)			
			{
					//strncpy(temp_len,(char *)ncpul_HOST_MESSAGE.IccData_len,2);
					memset(temp_len,0x00,sizeof(temp_len));
					//genutil_bcd_to_asc (ncpul_HOST_MESSAGE.IccData_len, 3, temp_len, sizeof(temp_len));
				//	len =atoi(temp_len);
					len = atoi((char *)ncpul_HOST_MESSAGE.IccData_len);
					memcpy((char *)iccdata,(char *)ncpul_HOST_MESSAGE.IccData,len);
					genutil_EMV_To_Auth_Tx( iccdata, atoi(temp_len),&AUTH_TX_V );
					memcpy(&EMV_details,&(AUTH_TX_V.EMV_details),sizeof(EMV_details));
					Generate_EMV_Details();
					strcat (Line[bitCount], FieldStrings [bitCount]);
					strncat (Line[bitCount], (char *)iccdata,strlen((char *)iccdata));
			}
		break ;

		case bf_56rplcmntaddlfee :
			if(strlen((char *)ncpul_HOST_MESSAGE.Rplcment_additional_fee_len) != 0)			
			{
				memset(temp_len,0x00,sizeof(temp_len));
					strncpy(temp_len,(char *)ncpul_HOST_MESSAGE.Rplcment_additional_fee_len,3);
					len =atoi(temp_len);
					strcat (Line[bitCount], FieldStrings [bitCount]);
					strncat (Line[bitCount], (char *)ncpul_HOST_MESSAGE.Rplcment_additional_fee,len);
			}
		break ;
		
		case bf_57authlfecycle :
			if(strlen((char *)ncpul_HOST_MESSAGE.Auth_life_cycle_len) != 0)			
			{
				memset(temp_len,0x00,sizeof(temp_len));
					strncpy(temp_len,(char *)ncpul_HOST_MESSAGE.Auth_life_cycle_len,3);
					len =atoi(temp_len);
					strcat (Line[bitCount], FieldStrings [bitCount]);
					strncat (Line[bitCount], (char *)ncpul_HOST_MESSAGE.Auth_life_cycle,len);
			}
		break ;

		case bf_58nationalposcndtncd :
			if(strlen((char *)ncpul_HOST_MESSAGE.National_pos_cc_len) != 0)			
			{
					memset(temp_len,0x00,sizeof(temp_len));
					strncpy(temp_len,(char *)ncpul_HOST_MESSAGE.National_pos_cc_len,3);
					len =atoi(temp_len);
					strcat (Line[bitCount], FieldStrings [bitCount]);
					strncat (Line[bitCount], (char *)ncpul_HOST_MESSAGE.National_pos_cc,len);
			}
		break ;

		case bf_59nationalposgeodata :
			if(strlen((char *)ncpul_HOST_MESSAGE.Nationalposgeogrphic_datalen) != 0)			
			{
				memset(temp_len,0x00,sizeof(temp_len));
					strncpy(temp_len,(char *)ncpul_HOST_MESSAGE.Nationalposgeogrphic_datalen,3);
					len =atoi(temp_len);
					strcat (Line[bitCount], FieldStrings [bitCount]);
					strncat (Line[bitCount], (char *)ncpul_HOST_MESSAGE.Nationalposgeographic_data,len);
			}
		break ;
		
		case bf_60advicersncode :
			if(strlen((char *)ncpul_HOST_MESSAGE.Advice_reason_code_len) != 0)			
			{
				memset(temp_len,0x00,sizeof(temp_len));
					strncpy(temp_len,(char *)ncpul_HOST_MESSAGE.Advice_reason_code_len,3);
					len =atoi(temp_len);
					strcat (Line[bitCount], FieldStrings [bitCount]);
					strncat (Line[bitCount], (char *)ncpul_HOST_MESSAGE.Advice_reason_code,len);
			}
		break ;

		case bf_61acqtransprtdata :
			if(strlen((char *)ncpul_HOST_MESSAGE.Acq_transport_data_len) != 0)			
			{
				memset(temp_len,0x00,sizeof(temp_len));
					strncpy(temp_len,(char *)ncpul_HOST_MESSAGE.Acq_transport_data_len,3);
					len =atoi(temp_len);
					strcat (Line[bitCount], FieldStrings [bitCount]);
					strncat (Line[bitCount], (char *)ncpul_HOST_MESSAGE.Acq_transport_data,len);
			}
		break ;
		case bf_62isstransprtdata :
			if(strlen((char *)ncpul_HOST_MESSAGE.Iss_transport_data_len) != 0)			
			{
				memset(temp_len,0x00,sizeof(temp_len));
					strncpy(temp_len,(char *)ncpul_HOST_MESSAGE.Iss_transport_data_len,3);
					len =atoi(temp_len);
					strcat (Line[bitCount], FieldStrings [bitCount]);
					strncat (Line[bitCount], (char *)ncpul_HOST_MESSAGE.Iss_transport_data,len);
			}
		break ;
		
		case bf_63pulsedata :
			if(strlen((char *)ncpul_HOST_MESSAGE.Pulse_data_len) != 0)			
			{
				memset(temp_len,0x00,sizeof(temp_len));
					strncpy(temp_len,(char *)ncpul_HOST_MESSAGE.Pulse_data_len,3);
					len =atoi(temp_len);
					strcat (Line[bitCount], FieldStrings [bitCount]);
					strncat (Line[bitCount], (char *)ncpul_HOST_MESSAGE.Pulse_data,len);
			}
		break ;

		case bf_70ntwrkmngmntinfocd :
			if(strlen((char *)ncpul_HOST_MESSAGE.Network_Management_info_code) != 0)			
			{
					strcat (Line[bitCount], FieldStrings [bitCount]);
					strncat (Line[bitCount], (char *)ncpul_HOST_MESSAGE.Network_Management_info_code,3);
			}
		break ;

		case bf_90originaldata :
			if(strlen((char *)ncpul_HOST_MESSAGE.Original_data) != 0)			
			{
					strcat (Line[bitCount], FieldStrings [bitCount]);
					strncat (Line[bitCount], (char *)ncpul_HOST_MESSAGE.Original_data,42);
			}
		break ;

		case bf_91fileupdatecode :
			if(strlen((char *)ncpul_HOST_MESSAGE.File_update_code) != 0)			
			{
					strcat (Line[bitCount], FieldStrings [bitCount]);
					strncat (Line[bitCount], (char *)ncpul_HOST_MESSAGE.File_update_code,1);
			}
		break ;

		case bf_95rplcmntamounts :
			if(strlen((char *)ncpul_HOST_MESSAGE.Replace_amounts) != 0)			
			{
					strcat (Line[bitCount], FieldStrings [bitCount]);
					strncat (Line[bitCount], (char *)ncpul_HOST_MESSAGE.Replace_amounts,42);
			}
		break ;

		case bf_96msgsecuritycode :
			if(strlen((char *)ncpul_HOST_MESSAGE.Msg_security_code) != 0)			
			{
					strcat (Line[bitCount], FieldStrings [bitCount]);
					strncat (Line[bitCount], (char *)ncpul_HOST_MESSAGE.Msg_security_code,8);
			}
		break ;

		case bf_98payee :
			if(strlen((char *)ncpul_HOST_MESSAGE.Payee) != 0)			
			{
					strcat (Line[bitCount], FieldStrings [bitCount]);
					strncat (Line[bitCount], (char *)ncpul_HOST_MESSAGE.Payee,25);
			}
		break ;
		
		case bf_101filename :
			if(strlen((char *)ncpul_HOST_MESSAGE.file_name_len) != 0)		
			{
				memset(temp_len,0x00,sizeof(temp_len));
					strncpy(temp_len,(char *)ncpul_HOST_MESSAGE.file_name_len,2);
					len =atoi(temp_len);
					strcat (Line[bitCount], FieldStrings [bitCount]);
					strncat (Line[bitCount], (char *)ncpul_HOST_MESSAGE.file_name,len);
			}
		break ;

		case bf_102accidentificaton1 :
			if(strlen((char *)ncpul_HOST_MESSAGE.Account_identificaton1_len) != 0)		
			{
				memset(temp_len,0x00,sizeof(temp_len));
					strncpy(temp_len,(char *)ncpul_HOST_MESSAGE.Account_identificaton1_len,2);
					len =atoi(temp_len);
					strcat (Line[bitCount], FieldStrings [bitCount]);
					strncat (Line[bitCount], (char *)ncpul_HOST_MESSAGE.Account_identificaton1,len);
			}
		break ;

		case bf_103accidentificaton2 :
			if(strlen((char *)ncpul_HOST_MESSAGE.Account_identificaton2_len) != 0)		
			{
				memset(temp_len,0x00,sizeof(temp_len));
					strncpy(temp_len,(char *)ncpul_HOST_MESSAGE.Account_identificaton2_len,2);
					len =atoi(temp_len);
					strcat (Line[bitCount], FieldStrings [bitCount]);
					strncat (Line[bitCount], (char *)ncpul_HOST_MESSAGE.Account_identificaton2,len);
			}
		break ;

		case bf_104txndescription :
			if(strlen((char *)ncpul_HOST_MESSAGE.Txn_description_len) != 0)		
			{
				memset(temp_len,0x00,sizeof(temp_len));
					strncpy(temp_len,(char *)ncpul_HOST_MESSAGE.Txn_description_len,3);
					len =atoi(temp_len);
					strcat (Line[bitCount], FieldStrings [bitCount]);
					strncat (Line[bitCount], (char *)ncpul_HOST_MESSAGE.Txn_description,len);
			}
		break ;

		case bf_105largepvtdata1 :
			if(strlen((char *)ncpul_HOST_MESSAGE.Large_pvt_data1_len) != 0)		
			{
				memset(temp_len,0x00,sizeof(temp_len));
					strncpy(temp_len,(char *)ncpul_HOST_MESSAGE.Large_pvt_data1_len,3);
					len =atoi(temp_len);
					strcat (Line[bitCount], FieldStrings [bitCount]);
					strncat (Line[bitCount], (char *)ncpul_HOST_MESSAGE.Large_pvt_data1,len);
			}
		break ;

		case bf_109senderdata :
			if(strlen((char *)ncpul_HOST_MESSAGE.Sender_data_len) != 0)		
			{
				memset(temp_len,0x00,sizeof(temp_len));
					strncpy(temp_len,(char *)ncpul_HOST_MESSAGE.Sender_data_len,3);
					len =atoi(temp_len);
					strcat (Line[bitCount], FieldStrings [bitCount]);
					strncat (Line[bitCount], (char *)ncpul_HOST_MESSAGE.Sender_data,len);
			}
		break ;

		case bf_110receiverdata :
			if(strlen((char *)ncpul_HOST_MESSAGE.Receiver_data_len) != 0)		
			{
				memset(temp_len,0x00,sizeof(temp_len));
					strncpy(temp_len,(char *)ncpul_HOST_MESSAGE.Receiver_data_len,3);
					len =atoi(temp_len);
					strcat (Line[bitCount], FieldStrings [bitCount]);
					strncat (Line[bitCount], (char *)ncpul_HOST_MESSAGE.Receiver_data,len);
			}
		break ;

		case bf_111additionaldataacq :
			if(strlen((char *)ncpul_HOST_MESSAGE.Additional_data_acq_len) != 0)		
			{
				memset(temp_len,0x00,sizeof(temp_len));
					strncpy(temp_len,(char *)ncpul_HOST_MESSAGE.Additional_data_acq_len,3);
					len =atoi(temp_len);
					strcat (Line[bitCount], FieldStrings [bitCount]);
					strncat (Line[bitCount], (char *)ncpul_HOST_MESSAGE.Additional_data_acq,len);
			}
		break ;

		case bf_113authagntinstidcd :
			if(strlen((char *)ncpul_HOST_MESSAGE.Auth_agent_inst_id_code_len) != 0)		
			{
				memset(temp_len,0x00,sizeof(temp_len));
					strncpy(temp_len,(char *)ncpul_HOST_MESSAGE.Auth_agent_inst_id_code_len,3);
					len =atoi(temp_len);
					strcat (Line[bitCount], FieldStrings [bitCount]);
					strncat (Line[bitCount], (char *)ncpul_HOST_MESSAGE.Auth_agent_inst_id_code,len);
			}
		break ;

		case bf_114contrycode :
			if(strlen((char *)ncpul_HOST_MESSAGE.Country_code_len) != 0)		
			{
				memset(temp_len,0x00,sizeof(temp_len));
					strncpy(temp_len,(char *)ncpul_HOST_MESSAGE.Country_code_len,3);
					len =atoi(temp_len);
					strcat (Line[bitCount], FieldStrings [bitCount]);
					strncat (Line[bitCount], (char *)ncpul_HOST_MESSAGE.Country_code,len);
			}
		break ;

		case bf_120accqualifiers :
			if(strlen((char *)ncpul_HOST_MESSAGE.Account_qualifiers_len) != 0)		
			{
				memset(temp_len,0x00,sizeof(temp_len));
					strncpy(temp_len,(char *)ncpul_HOST_MESSAGE.Account_qualifiers_len,3);
					len =atoi(temp_len);
					strcat (Line[bitCount], FieldStrings [bitCount]);
					strncat (Line[bitCount], (char *)ncpul_HOST_MESSAGE.Account_qualifiers,len);
			}
		break ;

		case bf_121additionaldataiss :
			if(strlen((char *)ncpul_HOST_MESSAGE.Additional_data_iss_len) != 0)		
			{
				memset(temp_len,0x00,sizeof(temp_len));
					strncpy(temp_len,(char *)ncpul_HOST_MESSAGE.Additional_data_iss_len,3);
					len =atoi(temp_len);
					strcat (Line[bitCount], FieldStrings [bitCount]);
					strncat (Line[bitCount], (char *)ncpul_HOST_MESSAGE.Additional_data_iss,len);
			}
		break ;

		case bf_122sponsorbankid :
			if(strlen((char *)ncpul_HOST_MESSAGE.Sponsor_bank_id_len) != 0)		
			{
				memset(temp_len,0x00,sizeof(temp_len));
					strncpy(temp_len,(char *)ncpul_HOST_MESSAGE.Sponsor_bank_id_len,3);
					len =atoi(temp_len);
					strcat (Line[bitCount], FieldStrings [bitCount]);
					strncat (Line[bitCount], (char *)ncpul_HOST_MESSAGE.Sponsor_bank_id,len);
			}
		break ;

		case bf_123avsauthdata :
			if(strlen((char *)ncpul_HOST_MESSAGE.Avs_check_auth_data_len) != 0)		
			{
				memset(temp_len,0x00,sizeof(temp_len));
					strncpy(temp_len,(char *)ncpul_HOST_MESSAGE.Avs_check_auth_data_len,3);
					len =atoi(temp_len);
					strcat (Line[bitCount], FieldStrings [bitCount]);
					strncat (Line[bitCount], (char *)ncpul_HOST_MESSAGE.Avs_check_auth_data,len);
			}
		break ;

		case bf_124info :
			if(strlen((char *)ncpul_HOST_MESSAGE.Info_len) != 0)			
			{
				memset(temp_len,0x00,sizeof(temp_len));
					strncpy(temp_len,(char *)ncpul_HOST_MESSAGE.Info_len,3);
					len =atoi(temp_len);
					strcat (Line[bitCount], FieldStrings [bitCount]);
					strncat (Line[bitCount], (char *)ncpul_HOST_MESSAGE.Info,len);
			}
		break ;

		case bf_125ntwrkmngmntinfo :
			if(strlen((char *)ncpul_HOST_MESSAGE.Network_Mngment_info_len) != 0)			
			{
				memset(temp_len,0x00,sizeof(temp_len));
					strncpy((char *)temp_len,(char *)ncpul_HOST_MESSAGE.Network_Mngment_info_len,3);
					len =atoi(temp_len);
					strcat (Line[bitCount], FieldStrings [bitCount]);
					strncat (Line[bitCount], (char *)ncpul_HOST_MESSAGE.Network_Mngment_info,len);
			}
		break ;

		case bf_126isstracedata :
			if(strlen((char *)ncpul_HOST_MESSAGE.Iss_trace_data_len) != 0)			
			{
				memset(temp_len,0x00,sizeof(temp_len));
					strncpy((char *)temp_data,(char *)ncpul_HOST_MESSAGE.Iss_trace_data_len,3);
					len =atoi(temp_len);
					strcat (Line[bitCount], FieldStrings [bitCount]);
					strncat (Line[bitCount], (char *)ncpul_HOST_MESSAGE.Iss_trace_data,len);
			}
		break ;

		case bf_127acqtracedata :
			if(strlen((char *)ncpul_HOST_MESSAGE.Acq_trace_data_len) != 0)			
			{
				memset(temp_len,0x00,sizeof(temp_len));
					strncpy((char *)temp_data,(char *)ncpul_HOST_MESSAGE.Acq_trace_data_len,3);
					len =atoi(temp_len);
					strcat (Line[bitCount], FieldStrings [bitCount]);
					strncat (Line[bitCount], (char *)ncpul_HOST_MESSAGE.Acq_trace_data,len);
			}
		break ;

	}

		
		startCharacter++;
		LastDisplayBit = bitCount;
	}

		if(strlen((char *)ncpul_HOST_MESSAGE.IccData) != 0)
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

		if ( ncpul_parse_host_msg(errbuf) == 0)
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


/***********************************************
   ncpul_RspMoveGeneric
************************************************/
INT ncpul_RspMoveGeneric(enum bitfields fieldid)
{
   struct bitfield_data *pBF;
   INT                  ret_val = true;


   /*---------------------------------------------------*/
   /* find pointer to bitfield info for this bitfield...*/
   /*---------------------------------------------------*/
   pBF = ncpul_Find_Bitfield_Table(fieldid);
   if (pBF != NULL)
   {
      if ( (reqidx + pBF->bd_fieldlen) <= PULSE_COMM_BUF_SZE )
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

} /* end ncpul_RspMoveGeneric */




/******************************************************************************
 *
 *  NAME:         ncpul_Move_IfThere
 *
 *  DESCRIPTION:  This function returns a value based on whether or not there
 *                is data in the specifiedncpul_HOST_MESSAGE field.  If there is
 *                data in it, TRUE is returned, else FALSE is returned.
 *
 *  INPUTS:       fieldid - Field Id to denote the source field of the data
 *                p_trantype - transaction type
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   True if data exists to be moved, else false
 *
 *  AUTHOR:       Abhishek Verma
 *
 ******************************************************************************/
INT ncpul_Move_IfThere( enum bitfields fieldid, BYTE p_trantype )
{
   struct bitfield_data *pBF;
   INT i = 0;

   /*---------------------------------------------------*/
   /* find pointer to bitfield info for this bitfield...*/
   /*---------------------------------------------------*/
   pBF = ncpul_Find_Bitfield_Table( fieldid );
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
} /* end ncpul_Move_IfThere */


/******************************************************************************
 *
 *  NAME:         ncpul_Move_Always
 *
 *  DESCRIPTION:  This function returns true.  It is used to indicate that data
 *                SHOULD be moved fromncpul_HOST_MESSAGE into ReqRsp_Buffer.
 *
 *  INPUTS:       fieldid - Field Id to denote the source field of the data
 *                p_trantype - transaction type
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   True
 *
 *  AUTHOR:       Abhishek Verma
 *
 *  MODIFICATIONS:
 ******************************************************************************/
INT ncpul_Move_Always( enum bitfields bitfield, BYTE p_trantype )
{
   return( true );
} /* end ncpul_Move_Always */

/***********************************************
   ncpul_GenericMove
************************************************/
INT ncpul_GenericMove(enum bitfields fieldid)
{
   struct bitfield_data *pBF;
   INT                  ret_val = true;

   /*---------------------------------------------------*/
   /* find pointer to bitfield info for this bitfield...*/
   /*---------------------------------------------------*/
   pBF = ncpul_Find_Bitfield_Table(fieldid);
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

} /* end ncpul_GenericMove */


/*****************************************************************************

  Function:    ncpul_vDataMove

  Description: This function will move data from thencpul_HOST_MESSAGE structure to
               the the raw ISO message.  Note: it moves the number indicated 
               by the length variable pointed to by pBF->p_db_data[0].
  Author: 
      Abhishek Verma
  Inputs:
      fieldid                - the bit field of the data to be moved
     ncpul_HOST_MESSAGE (global)  - structure that contains data to go into the
                               outgoing ISO message
  Outputs:
      ReqRsp_Buffer (global) - the buffer that contains the ISO formatted message
  Return values:
      None
  Modified by:
******************************************************************************/
 INT ncpul_vDataMove(enum bitfields fieldid)
{
   struct bitfield_data *pBF;
   INT                  movelen    = 0;
   CHAR					asc_len[5] = {0};

   /*---------------------------------------------------*/
   /* find pointer to bitfield info for this bitfield...*/
   /*---------------------------------------------------*/
   pBF = ncpul_Find_Bitfield_Table(fieldid);

   if (pBF != NULL)
   {
	// if(fieldid == 55)
	 {
	//	 genutil_bcd_to_asc (pBF->p_bd_pdata, 2, asc_len, sizeof(asc_len));
//movelen = atoi(asc_len);
	 }
	// else
	 {
		 movelen  = ncpul_get_variable_length( pBF->p_bd_pdata, pBF->bd_fieldlen );
	 }
	 memcpy(&ReqRsp_Buffer[reqidx],&pBF->p_bd_pdata[0], pBF->bd_fieldlen);
     reqidx =reqidx + pBF->bd_fieldlen;

     memcpy(&ReqRsp_Buffer[reqidx], &pBF->p_bd_pdata[pBF->bd_fieldlen], movelen);

     reqidx = reqidx + movelen;   /* increment buffer pointer */
	 return 1;
   }
   return 0;
} /* end ncpul_vDataMove */



/*****************************************************************************

  Function:    ncpul_RspMovevData

  Description: This function will move data from the the raw ISO message 
               to thencpul_HOST_MESSAGE structure.  Note: it moves the number
               indicated by the length variable pointed to by pBF->p_db_data[0].
  Author: 
      Abhishek Verma
  Inputs:
      fieldid                - the bit field of the data to be moved
      ReqRsp_Buffer (global) - the buffer that contains the ISO formatted message
  Outputs:
     ncpul_HOST_MESSAGE (global)  - structure that contains data to go into the
                               outgoing ISO message
  Return values:
      None
  Modified by:
******************************************************************************/
 INT ncpul_RspMovevData (enum bitfields fieldid)
{
   struct bitfield_data *pBF;
   INT                  movelen = 0;
   CHAR					asc_len[5] = {0};
   /*---------------------------------------------------*/
   /* find pointer to bitfield info for this bitfield...*/
   /*---------------------------------------------------*/
   pBF = ncpul_Find_Bitfield_Table(fieldid);

   if (pBF != NULL)
   {
     

	 memcpy(&pBF->p_bd_pdata[0],&ReqRsp_Buffer[reqidx],pBF->bd_fieldlen);
	// if(fieldid == 55)
	 {
		// genutil_bcd_to_asc (pBF->p_bd_pdata, 2, asc_len, sizeof(asc_len));
		// movelen = atoi(asc_len);
	 }
	// else
	 {
		movelen = atoi((char *)pBF->p_bd_pdata);
	 }
	 reqidx =reqidx + pBF->bd_fieldlen;
     memcpy(&pBF->p_bd_pdata[pBF->bd_fieldlen], &ReqRsp_Buffer[reqidx], movelen);

      reqidx = reqidx + movelen;   /* increment buffer pointer */
	  return 1;
   }
   return 1;
} /* end ncpul_RspMovevData */

 /******************************************************************************
 *
 *  NAME:         ncpul_Find_Bitfield_Table
 *
 *  DESCRIPTION:  This procedure finds a bitfield definition in the bitfield
 *                table where the bitfields are defined.  A pointer to it
 *                is returned.
 *
 *  INPUTS:       bitfield - ID of bitfield definition we are looking for
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   Pointer to the a specific bitfield definition in the
 *                bitfield table.  If one is not found, NULL is returned.
 *
 *  AUTHOR:       Abhishek Verma
 *
 ******************************************************************************/
struct bitfield_data *ncpul_Find_Bitfield_Table( enum bitfields bitfield )
{
   INT      i = 0;                    /* loop index */
   CHAR     temp_str[60] = {0};

   for ( i=0; i <= N_BFData; i++ )
      if ( BFData[i].bd_fieldid == bitfield )
         return &BFData[i];  /* Found the field - return address */

   /* Field not found in table - log error and return NULL */
   sprintf(temp_str, "This bitfield not found in BFData, bitfield: %d",
                      bitfield);
  // ncpul_log_message( 2, 2, temp_str, "ncpul_Find_Bitfield_Table",0 );
   return NULL;

} /* end ncpul_Find_Bitfield_Table */

/******************************************************************************
 *
 *  NAME:         ncpul_get_variable_length
 *
 *  DESCRIPTION:  This function converts a variable length character string
 *                into an integer.
 *
 *  INPUTS:       DataField - Pointer to ASCII character string
 *                FieldLen  - Length of 'DataField'
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   Integer value of 'DataField'
 *
 *  AUTHOR:      Abhishek Verma
 *
 *  MODIFICATIONS:
 ******************************************************************************/
int ncpul_get_variable_length( unsigned char *DataField, int FieldLen )
{
   int    i = 0;
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
/******************************************************************************
 *
 *  NAME:         ncpul_parse_host_msg
 *
 *  DESCRIPTION:  This procedure copies information from a string sent by the
 *                host.  It copies it into thencpul_HOST_MESSAGE structure.  The
 *                data is converted from EBCDIC to ASCII as they are moved.
 *
 *  INPUTS:       None
 *
 *  OUTPUTS:      err_buf - Textual message in case of a problem
 *
 *  RTRN VALUE:   True if successful, false if a problem occurred
 *
 *  AUTHOR:       Abhishek Verma
 *
 ******************************************************************************/
INT ncpul_parse_host_msg( pCHAR err_buf )
{
	struct bitfield_data *pBF;
	INT                   bfidx = 0;    /* Index to bitfields for this transaction */
	INT                   ret_val = true;
	INT                   BitFieldSize = 0;
	BYTE                  ReqRsp_Buffer_hex[17] = {0};
	BYTE                  bitmap = 0;
	INT                   i= 0;
	BYTE                  ErrorMsg[100] = {0};


	/* Initialize thencpul_HOST_MESSAGE structure. */
	memset(&ncpul_HOST_MESSAGE, 0x00, sizeof (ncpul_HOST_MESSAGE));

	reqidx = 0; 

	/*Reterive message Type*/
	memcpy(ncpul_HOST_MESSAGE.msgtype, &ReqRsp_Buffer[reqidx], PULSE_MESSAGE_TYPE_SIZE);

    // Point to Bitmap postion
	reqidx = reqidx + PULSE_MESSAGE_TYPE_SIZE;
	bitmap = ReqRsp_Buffer[reqidx];
	bitmapidx = reqidx;

	/* Point past bitmap, but 1st check for extended bitmap. */
	
	if ( bitmap &  BITTAB[bf_1xbitfield].bit_mask )
	{
		/* Extended     */
	    genutil_bin_to_ascii ((BYTE *)&ReqRsp_Buffer[reqidx], PULSE_EXTENDED_BITMAP_SIZE, astr, sizeof(astr));
		reqidx = reqidx + PULSE_EXTENDED_BITMAP_SIZE;
		BitFieldSize = PULSE_EXTENDED_BITFIELD_SIZE;
	}
	else
	{
		 genutil_bin_to_ascii ((BYTE *)&ReqRsp_Buffer[reqidx], PULSE_NORMAL_BITMAP_SIZE, astr, sizeof(astr));
		reqidx = reqidx + PULSE_NORMAL_BITMAP_SIZE;
		BitFieldSize = PULSE_NORMAL_BITFIELD_SIZE;
	}

	/*------------------------------------------------------------------
	For every bitfield present in message ( except first bit ) . . .
	-------------------------------------------------------------------*/
	for( bfidx = 2; bfidx <= BitFieldSize; bfidx++ )
	{
		if ( ReqRsp_Buffer[bitmapidx + BITTAB[bfidx].byte_no] &
			 BITTAB[bfidx].bit_mask )
		{
			/* Bit is on, field is there.... */
			pBF = ncpul_Find_Bitfield_Table((bitfields)bfidx);

			if ( NULL == pBF )
			{
				sprintf( err_buf, "Unexpected bitfield (%d) in message from host.",
				bfidx );
				strcat( err_buf, " MsgType = " );
				strncat( err_buf,(char *)ncpul_HOST_MESSAGE.msgtype, 4 );
				ret_val = false; /* Invalid field in message - so quit. */
				break;
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
				//	strncat( err_buf,ncpul_HOST_MESSAGE.msgtype, 4 );
					//ncpul_log_message( 3, 2, err_buf, "parse_host_msg",0 );
					break;
				}
				else
				{
					sprintf( err_buf,
							"Unable to parse field %d in msg from host, MsgType: %4.4s",
							bfidx,ncpul_HOST_MESSAGE.msgtype );
					//ncpul_log_message( 3, 2, err_buf, "parse_host_msg",0 );
				}
			}
		}
	}
	return( ret_val );
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
