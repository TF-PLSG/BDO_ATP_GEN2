/* FILE: RESPONSE.H */
/* GOAL: To document various symbols and structures for RESPONSE.C */

/********************   VARIOUS DEFINES and MACROS  **************************/
CHAR  MsgHeader[5];
CHAR  CmdCode[3];
CHAR  RespCode[3];
CHAR  ErrCode[3];
CHAR  Zmk[49];
CHAR  Zpk[49];
CHAR  Tpk[49];
CHAR  Bdk[33];
CHAR  Cwk[49];
CHAR  Ksn_Desc[4];
CHAR  Ksn[21];
CHAR  PVK_Pair[33];
CHAR  Pin_Block[17];
CHAR  Pin_Block_Format[3];
CHAR  Card_Number[20];
CHAR  PVKI;
CHAR  PVV[5];
CHAR  sZpk[49];
CHAR  dZpk[49];
CHAR  sPin_Block[17];
CHAR  dPin_Block[17];
CHAR  sPin_Block_Format[3];
CHAR  dPin_Block_Format[3];
CHAR  CVK[33];
CHAR  CVV[4];


/* Command Codes */
#define  KEY_CHANGE           "FA"
#define  KEY_CHANGE_RESPONSE  "FB"

#define  PIN_VERIFY_LOCAL             "DC"
#define  PIN_VERIFY_LOCAL_RESPONSE    "DD"
#define  PIN_VERIFY_NETWORK           "EC"
#define  PIN_VERIFY_NETWORK_RESPONSE  "ED"

#define  PIN_TRANSLATE_LOCAL              "CA"
#define  PIN_TRANSLATE_LOCAL_RESPONSE     "CB"
#define  PIN_TRANSLATE_NETWORK            "CC"
#define  PIN_TRANSLATE_NETWORK_RESPONSE   "CD"

#define  PIN_TRANSLATE_BDK_TO_INTERCHANGE           "CI"
#define  PIN_TRANSLATE_BDK_TO_INTERCHANGE_RESPONSE  "CJ"

#define  MAGSTRIPE_CVV           "CY"
#define  MAGSTRIPE_CVV_RESPONSE  "CZ"
#define  MAGSTRIPE_CSC           "RY"
#define  MAGSTRIPE_CSC_RESPONSE  "RZ"

#define PIN_VERIFY						"EA" 
#define PIN_VERIFY_RESPONSE				"EB" 
#define GENERATE_DINERS_KEY				"A0"
#define GENERATE_DINERS_KEY_RESPONSE	"A1"
#define PIN_VERIFY_ARQC_CUP				"JS"
#define PIN_VERIFY_ARQC_CUP_RESPONSE	"JT"
#define PIN_VERIFY_ARQC_JCB				"US"
#define PIN_VERIFY_ARQC_JCB_RESPONSE	"UT"
#define PIN_VERIFY_ARQC_EMV				"KQ"
#define PIN_VERIFY_ARQC_EMV_RESPONSE	"KR"
#define PIN_VERIFY_ARQC_DINERS			"KW"
#define PIN_VERIFY_ARQC_DINERS_RESPONSE	"KX"


/********************   FUNCTION PROTOTYPES   ********************************/
WORD generate_response ();
void host_notify( LPBYTE p_req, WORD len, pIPC_INFO p_ipc_info );
long parse_host_request(LPBYTE p_msg, WORD length_of_msg);
void update_iso_response();
CHAR increment_hex_value( CHAR );

 
static WORD    response_buffer_length;     // Length of the response
static BYTE    host_resp_buffer [5500];    // Response Buffer
static BYTE    save_tpdu[5];               // TPDU bytes, if present
static BOOLEAN priority_set = false;


/* End of module RESPONSE.H */

