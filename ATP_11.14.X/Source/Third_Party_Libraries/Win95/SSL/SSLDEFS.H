/******************************************************************************

       Module: ssldefs.h

        Title: ASCENDENT Dialog Manager

  Description: This is a file containing definitions specific to SSL

  $Log:   N:\PVCS\PTE\CORE\ssl\include\ssldefs.h  $
   
      Rev 1.2   Nov 05 1999 16:51:06   MSALEH
   Correct function prototype
   
      Rev 1.1   Oct 22 1999 15:20:44   MSALEH
   correct parameter to client_ssl_read()
   
      Rev 1.0   Oct 22 1999 15:18:48   MSALEH
   initial release
   
      
******************************************************************************/


/* cipher_list definitions */
#define DEFAULT_CIPHER				1
#define DEFAULT_NO_EXPORT        2
#define EXPORT_CIPHER		      3

/* SSL version */
#define SSLv2_method				1
#define SSLv3_method				2
#define SSLv23_method			3
#define TLSv1_method				4

/* SSL API definitions */
#define SSL_MAX_FILESIZE			256
#define SSL_MAX_PORT_SIZE			64
#define SSL_MAX_HOST_SIZE			32

/* SSL reconnection attempts */
#define SSL_INITIAL_CONNECTION      0
#define SSL_MAX_RECONNECTIONS	    3

/* structure used for SSL Messages */ 
typedef struct
{
   BYTE      tpdu;
   BYTE		 st1;
   BYTE		 st2;
   BYTE      app_data_type;
   BYTE		 reserved1;
   CHAR      sslbuf[MAX_INCOMING_SSL_BUFFER_SIZE];
} SSL_DATA,  *pSSL_DATA;

typedef struct
{
   BYTE		tpdu[5];
   CHAR      sslbuf[MAX_INCOMING_SSL_BUFFER_SIZE - 5];
}  SSL_BUF, *pSSL_BUF;

typedef struct
{
	SSL     tmp_ssl;
	BYTE    tmp_tpdu[5];
} SSL_INFO, *pSSL_INFO;



/* SSL routines */

int verification_setup (SSL_CTX *ctx,BIO *bio);

void ssl_accept (pIPC_INFO ipc_info);

void fix_rsa_method ();

void ssl_close (pIPC_INFO ipc_info);

void ssl_shutdown (pIPC_INFO ipc_info);
               
BOOLEAN ssl_write (const WORD len, const void * data, pIPC_INFO ipc_info);
BOOLEAN client_ssl_write(const WORD len, const void * data, pIPC_INFO ipc_info);

BOOLEAN ssl_read (pIPC_INFO ipc_info);
BOOLEAN client_ssl_read(pIPC_INFO ipc_info, pCHAR buf);

BOOLEAN server_ssl_init (const USER_TCP_STRUCT *ud, pIPC_INFO ipc_info, LOCAL_CM_CONFIG cm_config);
BOOLEAN client_ssl_init(pIPC_INFO ipc_info);

void generate_new_transaction_id(pCHAR tranid);