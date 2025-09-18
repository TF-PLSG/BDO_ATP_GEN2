/******************************************************************************

       Module: tcpssl.c

        Title: ASCENDENT Dialog Manager

  Description: Contains all SSL routines

  $Log:   N:\PVCS\PTE\CORE\ptecom_lib\TCPSSL.C  $
   
      Rev 1.7   Jan 19 2000 14:56:58   MSALEH
   add getpeername() support to ssl_read() and
   ssl_client_read()
   
      Rev 1.6   Nov 17 1999 09:28:10   MSALEH
   modify ssl_close() and ssl_shutdown()
   to properly shutdown the SSL connection
   
      Rev 1.5   Nov 10 1999 17:48:58   MSALEH
   * Modify SSL connection flow
   * Use a function to build a complete XIPC
   login string
   
      Rev 1.4   Nov 02 1999 13:39:30   jjones
   Remove while loop in ssl_read
   
      Rev 1.3   Oct 22 1999 15:28:26   MSALEH
   correct parameter for client_ssl_read()
   
      Rev 1.2   Oct 18 1999 14:47:52   MSALEH
   correct sprintf and login name for ssl connections
   
      Rev 1.1   Oct 15 1999 10:30:04   MSALEH
   Move SSL_SUPPORT definition to makefile
   
      Rev 1.0   Oct 14 1999 10:41:54   MSALEH
   Integrate SSL modifications
*****************************************************************************/

#include <process.h>
#include <time.h>
#include <sys/timeb.h>

#include "tcp.h"
#include "ntutils.h"
#include "ptellist.h"
#include "pteipc.h"

#ifdef SSL_SUPPORT

#include "e_os.h"
#include "sslc.h"
#include "ssldefs.h"
#include "rsa.h"

#define CERT_FILE	"server.pem"

PRIVATE HANDLE          lhandle;

PRIVATE  SSL_LINKED_LIST SSLConnects;
PRIVATE  INT volatile    SSLLinkedListLocked = FALSE;
LONG                     sequencer = 0;

static int verify_output_bio_index= -1;
static int server_verify_callback(int ok, X509_STORE_CTX *ctx);

int verification_setup(SSL_CTX *ctx, BIO *bio)
	{
	if (verify_output_bio_index < 0)
		verify_output_bio_index=SSL_CTX_get_ex_new_index(0,
			"BIO to print verification info",NULL,NULL,NULL);

	SSL_CTX_set_ex_data(ctx,verify_output_bio_index,(char *)bio);
	SSL_CTX_set_verify(ctx,
		SSL_VERIFY_PEER|SSL_VERIFY_CLIENT_ONCE,
		server_verify_callback);

	if (!SSL_CTX_load_verify_locations(ctx,CERT_FILE,NULL))
    {
		LogEvent("ASCENDENT: client_ssl_init, verification_setup failed", ERROR_MSG);
		return(0);
    } 
	return(1);
	}

/* For Win16 (using Visual C 1.5x) the definitions:
 * int _far _loadds server_verify_callback(....)
 * This callback is used for each 'check' performed by the default
 * verification routines.
 */

static int MS_CALLBACK server_verify_callback(int ok, X509_STORE_CTX *cert_ctx)
	{
	SSL *ssl;
	BIO *bio;
	X509 *cert;
	int reason,depth;
	char buf[512];

	cert=X509_STORE_CTX_get_current_cert(cert_ctx);

	reason=X509_STORE_CTX_get_error(cert_ctx);

	depth=X509_STORE_CTX_get_error_depth(cert_ctx);

	ssl=(SSL *)X509_STORE_CTX_get_ex_data(cert_ctx,
		SSL_get_ex_data_X509_STORE_CTX_idx());

	if (ssl == NULL)
		{ 
		/* internal error */ 
		abort(); 
		LogEvent("ASCENDENT: client_ssl_init, server_verify_callback failed", ERROR_MSG);
		abort(); 
		}

	bio=(BIO *)SSL_CTX_get_ex_data(SSL_get_SSL_CTX(ssl),
		verify_output_bio_index);

	X509_NAME_oneline(X509_get_subject_name(cert),buf,256);

/*	BIO_printf(bio,"depth=%d %s\n",depth,buf);

	BIO_printf(bio,"\tstate=%d reason=%s\n",ok,
		X509_verify_cert_error_string(reason));
*/

	ok=1;
	return(ok);
	}

/*------------------------------------------------------------------------------*/
BOOLEAN client_ssl_init(pIPC_INFO ipc_info)
{
	BIO	*bio=NULL;
	INT   done=0, sock;
	LONG	timer_return;
   CHAR  szBuff[DM_ERRBUF_SIZE];

   /* Create a BIO for stdout and stderr */

	ipc_info->bio_out=BIO_new_fp(stdout,BIO_NOCLOSE);
   ipc_info->bio_err=BIO_new_fp(stderr,BIO_NOCLOSE);

	/* load the error text strings for better error reporting */
 	SSL_load_error_strings();

	/* initialise the library */
	SSL_library_init();

	/* for now default to SSLv23 */
	ipc_info->ssl_ctx=SSL_CTX_new(SSLv23_client_method());

	/* setup the order and cipher suites desired */
	/* for now default to default!exp */

	switch(ipc_info->cipher_strength)
    {
		case (DEFAULT_CIPHER):
			 SSL_CTX_set_cipher_list (ipc_info->ssl_ctx,"DEFAULT");
			break;

		case (DEFAULT_NO_EXPORT):
			SSL_CTX_set_cipher_list (ipc_info->ssl_ctx, "DEFAULT:!EXP");
			break;

		case (EXPORT_CIPHER):
			SSL_CTX_set_cipher_list (ipc_info->ssl_ctx, "EXPORT");
			break;
			 
		default: 
           SSL_CTX_set_cipher_list (ipc_info->ssl_ctx, "DEFAULT:!EXP");  
		break;
	}

	/* turn on all vendor bug compatibility options */
	SSL_CTX_set_options(ipc_info->ssl_ctx,SSL_OP_ALL);

	/* setup the verification callbacks - see verify.c for source */
	if (!verification_setup(ipc_info->ssl_ctx,ipc_info->bio_out))
	{ 
		LogEvent("ASCENDENT: client_ssl_init, certificate verification routine failed", ERROR_MSG);
		ssl_close(ipc_info);
		return(FALSE);
	}	

	/* Create the SSL structure.   Defaults are inherited from the SSL_CTX.
	 * Options are usually set against the SSL_CTX, and only those
	 * requiring chnaging against the SSL.
	 */

	if ((ipc_info->ssl=SSL_new(ipc_info->ssl_ctx)) == NULL)
	{ 
		BIO_printf(ipc_info->bio_err,"SSL_CTX_new failed on port %s\n", ipc_info->ssl_port);
		ERR_print_errors(ipc_info->bio_err); 
		sprintf(szBuff, "ASCENDENT : client_ssl_init, SSL_CTX_new failed on port %s", ipc_info->ssl_port);
		LogEvent(szBuff, ERROR_MSG);
		ssl_close(ipc_info);
		return(FALSE);
	}
		
	/* set the session time out value in seconds */
	timer_return = 60;
	timer_return = SSL_CTX_set_timeout(ipc_info->ssl_ctx, timer_return);
	if (timer_return < 0)
	{
		sprintf(szBuff, "ASCENDENT : client_ssl_init, SSL_CTX_set_timeout failed on port %s", 
			ipc_info->ssl_port);
		LogEvent(szBuff, ERROR_MSG);
		ssl_close(ipc_info);
		return(FALSE);
	}

	/* An SSL exists. Which side of the protocol are we doing/
	 * If we call SSL_connect() or SSL_accept(), the side will be
	 * set In this case we simply call SSL_do_handshake() which
	 * requires the 'client/server side' flag to be set.
	 * SSL_set_connect_state() sets the SSL connection to do the 
	 * connect side of the protocol on a SSL_do_handshake().
	 * SSL_set_accept_state() sets it up to do accept.  
	 */

	SSL_set_connect_state(ipc_info->ssl);

	/* i = sizeof (ssl); */
	/* create a BIO to handle the connect and setup of transparent
	 * SSL handling. This encapsulates the socket setup. This may 
	 * also be done manually using the "standard" socket API calls.
	 * IN this example, only a socket setup is done.
    */
	
	if ((bio=BIO_new_connect(ipc_info->ssl_port)) == NULL)
	{ 
		BIO_printf(ipc_info->bio_err,"BIO_new_connect failed on port %s\n", ipc_info->ssl_port);
		ERR_print_errors(ipc_info->bio_err); 
		sprintf(szBuff, "ASCENDENT : client_ssl_init, BIO_new_connect failed on port %s", ipc_info->ssl_port);
		LogEvent(szBuff, ERROR_MSG);
		ssl_close(ipc_info);
		return(FALSE);
	}


	/* Use the newly created connect BIO.  At this point, the
	 * connect BIO has not checked whether the hostname is valid.
	 * This occurs the first time the SSL protocol attempts to
	 * read/write some data.  In order to do the "connect operations",
	 * BIO_do_handshake() or * BIO_do_connect() could be called. 
	 */

	SSL_set_bio(ipc_info->ssl,bio,bio);
	
	while (!done)
	{
		int i;

		/* Crank the handle on the state machine... */
		i=SSL_do_handshake(ipc_info->ssl);

		/* SSL_get_error() call categorises errors into groups. */
		switch (SSL_get_error(ipc_info->ssl,i))
		{
		  case SSL_ERROR_NONE:
			/* Handshake has finished, so proceed */
			done=1;
			/* bio=BIO_pop(ipc_info->abio); */
	
			/* store the current socket information */
			BIO_get_fd(bio,&sock);
			ipc_info->socket_id = sock;
			break;

		  case SSL_ERROR_SSL:

			/* Handshake error - report and exit */
			BIO_printf(ipc_info->bio_err,"Handshake failure on port %s \n", ipc_info->ssl_port);
			ERR_print_errors(ipc_info->bio_err);
			sprintf(szBuff, "ASCENDENT : client_ssl_init, Handshake failure on port %s", ipc_info->ssl_port);
			LogEvent(szBuff, ERROR_MSG);
			ssl_close(ipc_info);
	      return(FALSE);
			break;

		  case SSL_ERROR_SYSCALL:

			/* System call error.  This error is different from
			 * the SSL_ERROR_SSL in that errno (under unix)
			 * has the numeric error value, and it is not 
			 * converted into text.  If doing an SSL_read or
			 * SSL_write there is no recorded error in the error
			 * logging. This is becasue the error could be a 
			 * 'retry' error which the library is not aware of.
			 */

			BIO_printf(ipc_info->bio_err,"System call error on port %s \n", ipc_info->ssl_port);
			ERR_print_errors(ipc_info->bio_err);
			sprintf(szBuff, "ASCENDENT : client_ssl_init, System call error on port %s", ipc_info->ssl_port);
			LogEvent(szBuff, ERROR_MSG);
			ssl_close(ipc_info);
	      return(FALSE);
			break;

		  case SSL_ERROR_WANT_READ:
		  case SSL_ERROR_WANT_WRITE:
		  case SSL_ERROR_WANT_CONNECT:

			/* Perform the handshake again.  Errors are normally
			 * only reported when doing non-blocking IO. */

			//BIO_printf(ipc_info->bio_err,"sleep(1) on port %s \n", ipc_info->ssl_port);
			pteipc_sleep(1000);
			break;

		  case SSL_ERROR_ZERO_RETURN:

			/* A read(2)/write(2) system call returned 0 (usually
			 * because the socket was closed. If the socket is 
			 * closed, the protocol has failed. */

			BIO_printf(ipc_info->bio_err,"socket closed on port %s \n", ipc_info->ssl_port);
			sprintf(szBuff, "ASCENDENT : client_ssl_init, socket %0ld closed on port %s", ipc_info->socket_id, ipc_info->ssl_port);
			LogEvent(szBuff, ERROR_MSG);
			ssl_close(ipc_info);
			return(FALSE);
			break;

		default: 
         BIO_printf(ipc_info->bio_err,"undetermined error on port %s \n", ipc_info->ssl_port);
			sprintf(szBuff, "ASCENDENT : client_ssl_init, undetermined error on port %s", ipc_info->ssl_port);
			LogEvent(szBuff, ERROR_MSG);
			ssl_close(ipc_info);
			return(FALSE);
			break;
		}			
	}
	return(TRUE);
}

/*------------------------------------------------------------------------------*/
BOOLEAN server_ssl_init(const USER_TCP_STRUCT *ud, pIPC_INFO ipc_info, LOCAL_CM_CONFIG cm_config)
{

 
  DWORD lthread_id;
  CHAR  filename [SSL_MAX_FILESIZE];
  BIO	  *bio=NULL;
  CHAR  szBuff[DM_ERRBUF_SIZE];

  /* Idiot check all incoming pointers */
  if(!ud)
  {
    LogEvent("ASCENDENT: server_ssl_init, bad tcp structure pointer passed", ERROR_MSG);
    return(FALSE);
  }
  else if(!ud->pfNotify)
  {
    LogEvent("ASCENDENT: server_ssl_init, bad Function/Buffer pointer passed", ERROR_MSG);
    return(FALSE);
  }

  /* Assign tcp/ip functions to the library primitives */
  port_write = ssl_write;

  /* Populate callback fields */
  pfNotify  = ud->pfNotify;

  /* set the reconnection attempts */
  ipc_info->reconnect = SSL_INITIAL_CONNECTION;

  /* turn caching off to correct RSA problem with threading */
  /* don't call this until RSA correct bugs in bug fix */
  fix_rsa_method();
  
  /* Create a BIO for stdout and stderr */
  ipc_info->bio_out=BIO_new_fp(stdout,BIO_NOCLOSE);
  ipc_info->bio_err=BIO_new_fp(stderr,BIO_NOCLOSE); 

	/* load the error text strings for better error reporting */
	SSL_load_error_strings();

	/* initialise the library */
	SSL_library_init();

	/* create the context - we wish to operate in the normal 
	 * default SSLv3 in a SSLv2 header mode so as to maximise
	 * the number of servers that we can connect to
	 */
	
	/* set the version method */
	switch(cm_config.db_server_method)
	{
		case (SSLv2_method):
			ipc_info->ssl_ctx=SSL_CTX_new(SSLv2_server_method());
			break;

		case (SSLv3_method):
			ipc_info->ssl_ctx=SSL_CTX_new(SSLv3_server_method());
			break;

		case (SSLv23_method):
			ipc_info->ssl_ctx=SSL_CTX_new(SSLv23_server_method());
			break;

		case (TLSv1_method):
			ipc_info->ssl_ctx=SSL_CTX_new(TLSv1_server_method());
			break;

		default: 
            ipc_info->ssl_ctx=SSL_CTX_new(SSLv23_server_method());
			break;
	}
	
	/* setup the order and cipher suites desired */
	switch(cm_config.db_cipher_suite)
    {
		case (DEFAULT_CIPHER):
			 SSL_CTX_set_cipher_list (ipc_info->ssl_ctx,"DEFAULT");
			break;

		case (DEFAULT_NO_EXPORT):
			SSL_CTX_set_cipher_list (ipc_info->ssl_ctx, "DEFAULT:!EXP");
			break;

		case (EXPORT_CIPHER):
			SSL_CTX_set_cipher_list (ipc_info->ssl_ctx, "EXPORT");
			break;

		default: 
            SSL_CTX_set_cipher_list (ipc_info->ssl_ctx, "DEFAULT:!EXP");
			break;
	}

	/* setup the verification callbacks - see verify.c for source */
	if (!verification_setup(ipc_info->ssl_ctx,ipc_info->bio_out))
	{ 
		sprintf(szBuff, "ASCENDENT: server_ssl_init, verfication setup failed error on port %s",
			ipc_info->ssl_port);
		LogEvent(szBuff, ERROR_MSG);
		ssl_close(ipc_info);
		return(FALSE);
	}	

	/* get the pathname to the Ascendent directory */
	GetAscendentConfigDirectory( filename );
    strcat( filename, "server.pem" );
	
	/* load the server certificate from the file */
	if (!SSL_CTX_use_certificate_file(ipc_info->ssl_ctx,filename,SSL_FILETYPE_PEM))
	{ 
		sprintf(szBuff, "ASCENDENT: server_ssl_init, certificate failed to load on port %s",
			ipc_info->ssl_port);
		LogEvent(szBuff, ERROR_MSG);
		ssl_close(ipc_info);
		return(FALSE);
	}	
	
	/* load the server private key from the file */
	if (!SSL_CTX_use_PrivateKey_file(ipc_info->ssl_ctx,filename,SSL_FILETYPE_PEM))
	{ 
		sprintf(szBuff, "ASCENDENT: server_ssl_init, private key failed to load on port %s",
			ipc_info->ssl_port);
		LogEvent(szBuff, ERROR_MSG);
		ssl_close(ipc_info);
		return(FALSE);
	}

	/* check that the certificate and private key match - this is
	 * a common error that makes sense to trap before starting the
	 * server as without a certificate and matching private key we
	 * cannot complete the SSL handshake so it is pointless proceeding 
	 */
	if (!SSL_CTX_check_private_key(ipc_info->ssl_ctx))
	{ 
		sprintf(szBuff, "ASCENDENT: server_ssl_init, private key and certificate mismatch on port %s", 
			ipc_info->ssl_port);
		LogEvent(szBuff, ERROR_MSG);
		ssl_close(ipc_info);
		return(FALSE);
	}
		
	if ((ipc_info->abio=BIO_new_accept(ipc_info->ssl_port)) == NULL)
	{
		sprintf(szBuff, "ASCENDENT: server_ssl_init,BIO_new_accept failed on port %s", 
			ipc_info->ssl_port);
		LogEvent(szBuff, ERROR_MSG);
		ssl_close(ipc_info);
		return(FALSE);
	}

	/* Make things nice for restarting the server */

	BIO_set_bind_mode(ipc_info->abio,BIO_BIND_REUSEADDR);

	/* The first call to BIO_do_accept() sets up the socket for accepting
	   incoming connections */

	 if (BIO_do_accept(ipc_info->abio) <= 0)
	 {
		sprintf(szBuff, "ASCENDENT: server_ssl_init, Unable to initalise accept socket on port %s", 
			ipc_info->ssl_port);
		LogEvent(szBuff, ERROR_MSG);
		ssl_close(ipc_info);
		return(FALSE);
	 } 

	 /* wait here until the SSL connection linked list is unlocked,
	    then lock the linked list */
	 while (SSLLinkedListLocked);
      pteipc_sleep(100);

	 SSLLinkedListLocked = TRUE;

	 /* initialize the linked list, then unlock the linked list */
	 ptellist_init_ssl_list(&SSLConnects);
	 SSLLinkedListLocked = FALSE;
    
	 /* create thread to listen on the socket */
    lhandle = CreateThread(  NULL,
                             0,
                             (LPTHREAD_START_ROUTINE)ssl_accept, 
  							        ipc_info,
                             0,
                             &lthread_id );
    if( lhandle == NULL )
	{
      LogEvent( "ASCENDENT: server_ssl_init, Unable to create ssl accept thread", ERROR_MSG );
      ssl_close(ipc_info);
      return (FALSE);
	}
  
  return(TRUE);
}

/*------------------------------------------------------------------------------*/
void ssl_accept(pIPC_INFO ipc_info)
{
	DWORD	cli_tid;
   char	*port=NULL;
	BIO	*bio=NULL;
   INT	sock;
   CHAR  szBuff[DM_ERRBUF_SIZE];

  while(1)
	{
	    /* The second call to BIO_do_accept() sets accepts incoming connections */

		if (BIO_do_accept(ipc_info->abio) < 0)
		{
		  /* Something went wrong */
		  LogEvent( "ASCENDENT: ssl_accept, Second BIO_do_accept failed", ERROR_MSG );
        break;
		}
	
		/* set up the ctx structure */
    	if ((ipc_info->ssl=SSL_new(ipc_info->ssl_ctx)) == NULL)
		{ 
		  sprintf(szBuff, "ASCENDENT: ssl_accept, SSL_new failed on port %s", ipc_info->ssl_port);
		  LogEvent(szBuff, ERROR_MSG);
        break;
		}
	    
		/* Pop off the newly accepted connection */
		bio=BIO_pop(ipc_info->abio);
		
		BIO_get_fd(bio,&sock);
		ipc_info->socket_id = sock;
		
		/* The 'bio' is 'taken' by SSL_set_bio.  One does not
		 * need to free it since the ssl cleanup will do so. */
		SSL_set_bio(ipc_info->ssl,bio,bio);

		/* report connection established */
		sprintf(szBuff, "ASCENDENT: ssl_accept, connection established on port %s", ipc_info->ssl_port);
		LogEvent(szBuff, INFO_MSG);

		/* An SSL connection exists. Which side of the protocol are
		 * we performing?  If calling SSL_connect() or SSL_accept(),
		 * the side is set. In this case we only call SSL_do_handshake
		 * which requires the client/server flag to be set.
		 */
	
		SSL_set_accept_state(ipc_info->ssl);

		/* If the cached session-id is not cleared, use the protocol
		 * version of the last connection.  If the session-id is
		 * cleared, revert to the protocol specified by the SSL_CTX.
		 */
		SSL_set_session(ipc_info->ssl,NULL);
		SSL_clear(ipc_info->ssl);

		/* Create the worker thread to serve the new client */
	   if( CreateThread( NULL,
                        0,
                        (LPTHREAD_START_ROUTINE)ssl_read,
                        ipc_info,
	      		         0,
                        &cli_tid ) == NULL )
      {
         LogEvent( "ASCENDENT: ssl_accept, Unable to create ssl_read thread", ERROR_MSG );
         break;
      }

      /* We need to do this to allow the thread the initialize properly */
      pteipc_sleep(1000);
	}

   ssl_close(ipc_info);
}

/*------------------------------------------------------------------------------*/
BOOLEAN ssl_read(pIPC_INFO local_ipc_info)
{
  INT			         len, j;
  INT			         bytes_received = 0;
  INT			         msg_length     = 0;
  BOOLEAN		      length_bytes_including_themselves   = FALSE;
  BOOLEAN		      volatile error_condition            = FALSE;
  pBYTE			      p_incoming_buffer;
  pSSL_LIST_ELEMENT	list_element;
  INT                ret;
  IPC_INFO           ipc_info;
  struct             sockaddr_in  name;
  DWORD              namelength = sizeof(name);
  CHAR			      buf[MAX_INCOMING_SSL_BUFFER_SIZE];
  CHAR            	szBuff[DM_ERRBUF_SIZE];

  /* 
     We need to do this because the ipc_info is a pointer and the value changes with
     every new connection 
  */
  memcpy(&ipc_info, local_ipc_info, sizeof (IPC_INFO));

  ret = getpeername ( ipc_info.socket_id, (struct sockaddr*)&name, &namelength );
  
  if (ret == SOCKET_ERROR)
  {
      sprintf(szBuff, "ASCENDENT: ssl_read, getpeername failed, socket:%0ld, error: %d ", ipc_info.socket_id, WSAGetLastError());
      LogEvent(szBuff, ERROR_MSG);

      sprintf(szBuff, "ASCENDENT: ssl_read, client connected with socket:%0ld", ipc_info.socket_id );
  }
  else
  {
      /* allocate memory to receive incoming messages from this client */
      strcpy(ipc_info.buffer, inet_ntoa(name.sin_addr));
      sprintf(szBuff, "ASCENDENT: ssl_read, client at IP address:%s connected with socket:%0ld", ipc_info.buffer, ipc_info.socket_id );
  }

  LogEvent(szBuff, INFO_MSG);

  /* allocate memory to receive incoming messages from this client */
  p_incoming_buffer = NULL_PTR;
  p_incoming_buffer = (pCHAR)malloc(MAX_INCOMING_SSL_BUFFER_SIZE);
  
  /* make sure allocation was successful */
  if( p_incoming_buffer == NULL_PTR )
  {
    LogEvent( "ASCENDENT: ssl_read, Unable to allocate memory for incoming message", ERROR_MSG );
    return(FALSE);
  }

  /* log this thread on to XIPC */    
  if (ipc_info.connectiontype)
     thread_logon(ipc_info.instancename, ipc_info.loginname);

  while (!error_condition)
  {
      /* read incoming messge from the SSL client */
      len=SSL_read(ipc_info.ssl,buf,sizeof(buf));
      switch ((j=SSL_get_error(ipc_info.ssl,len)))
      {
          case SSL_ERROR_NONE:
      
      		/* Handshake finished, notify cm_notify to process the message */
      
      		/* wait here until the SSL connection linked list is unlocked, then lock the linked list */
      		while (SSLLinkedListLocked);
               pteipc_sleep(100);

      		SSLLinkedListLocked = TRUE;
      
      		/* allocate and add node to add to the linked list */
            list_element = ptellist_append_ssl_element(&SSLConnects, ipc_info.ssl, sizeof(SSL));
      		if (list_element == NULL_PTR)
      		{
      		   SSLLinkedListLocked = FALSE;
      			sprintf(szBuff, "ASCENDENT: ssl_accept, ptellist_allocate_element failed on port %s", ipc_info.ssl_port);
      			LogEvent(szBuff, ERROR_MSG);
               error_condition = TRUE;
               break;
      		}
      
      		/* clear the tran_id field */
      		memset(&list_element->tran_id, 0, sizeof(list_element->tran_id));
      
      		/* generate a unique transaction id and store in linked list and ipc_info */
      		generate_new_transaction_id(list_element->tran_id);
      		memcpy(ipc_info.ssl_id, list_element->tran_id, sizeof(list_element->tran_id));
      			
      		/* unlock the list */
      		SSLLinkedListLocked = FALSE;
      			
      		/* notify cm_notify to process the message */
      		memcpy (p_incoming_buffer, buf, len);
      		pfNotify(p_incoming_buffer, (WORD)len, &ipc_info);
      		break;
      
      	case SSL_ERROR_SSL:
      		/* Handshake error, report and exit */
      		sprintf(szBuff, "ASCENDENT: ssl_read, Handshake failure on port %s, socket id %0ld", 
                                       ipc_info.ssl_port, 
                                       ipc_info.socket_id );

      		LogEvent(szBuff, ERROR_MSG);
            error_condition = TRUE;
      		break;
      
      	case SSL_ERROR_ZERO_RETURN:
      		sprintf(szBuff, "ASCENDENT: ssl_read, client logged off on port %s, socket id %0ld", 
                                       ipc_info.ssl_port, 
                                       ipc_info.socket_id );

      		LogEvent(szBuff, INFO_MSG);
            error_condition = TRUE;
      		break;
      
      	case SSL_ERROR_WANT_READ:
      	case SSL_ERROR_WANT_WRITE:
      	case SSL_ERROR_WANT_CONNECT:
      	case SSL_ERROR_SYSCALL:
      	default:
      
      		/* System call error.  This error is different from
      		 * the SSL_ERROR_SSL in that errno (under unix)
      		 * has the numeric error value, and is not converted
      		 * into text.  If performing an SSL_read/SSL_write 
      		 * there is no recorded error in the error logging
      		 * because the error could be 'retryable' error which
      		 * the library is not aware of.
      		 */
      
      		/* During testing this leg is taken when the client process ends without calling
      		   ssl_shutdown, so for now use generic informational text */
      		sprintf(szBuff, "ASCENDENT: ssl_read, client not responding, read returned %d on port %s",
      			j, ipc_info.ssl_port);
      		LogEvent(szBuff, INFO_MSG);
            error_condition = TRUE;
      		break;
      }
  }

  /* if there is  a valid incoming buffer, then free it */
  if (p_incoming_buffer != NULL)
	free( p_incoming_buffer );
 
  thread_logoff(); 

  ssl_close(&ipc_info);

  if (j == SSL_ERROR_ZERO_RETURN)
     return(TRUE);
  else
     return(!error_condition);
}
/*------------------------------------------------------------------------------*/
BOOLEAN client_ssl_read(pIPC_INFO ipc_info, pCHAR buf)
{
   INT      len, j;
   INT      msg_length = 0;
   BOOLEAN  error_condition = FALSE;
   INT      ret;
   struct   sockaddr_in  name;
   DWORD    namelength = sizeof(name);
   CHAR     szBuff[DM_ERRBUF_SIZE];

   ret = getpeername ( ipc_info->socket_id, (struct sockaddr*)&name, &namelength );
  
   if (ret == SOCKET_ERROR)
   {
      sprintf(szBuff, "ASCENDENT: ssl_read, getpeername failed, socket:%0ld, error: %d ", ipc_info->socket_id, WSAGetLastError());
      LogEvent(szBuff, ERROR_MSG);

      sprintf(szBuff, "ASCENDENT: ssl_read, client connected with socket:%0ld", ipc_info->socket_id );
   }
   else
   {
      /* allocate memory to receive incoming messages from this client */
      strcpy(ipc_info->buffer, inet_ntoa(name.sin_addr));
      sprintf(szBuff, "ASCENDENT: ssl_read, client connected with socket:%0ld to IP address:%s", ipc_info->socket_id, ipc_info->buffer);
   }

   LogEvent(szBuff, INFO_MSG);

   len=SSL_read(ipc_info->ssl, buf, MAX_INCOMING_SSL_BUFFER_SIZE);
 
	switch ((j=SSL_get_error(ipc_info->ssl,len)))
	{
		case SSL_ERROR_NONE:
			/* Handshake finished, so continue processing the data. */
			break;

		case SSL_ERROR_SSL:
			/* Handshake error, report and exit */
			sprintf(szBuff, "ASCENDENT: ssl_client_read, Handshake failure on port %s", ipc_info->ssl_port);
			LogEvent(szBuff, ERROR_MSG);
         error_condition = TRUE;
			break;

		case SSL_ERROR_ZERO_RETURN:
			sprintf(szBuff, "ASCENDENT: ssl_client_read, SSL_ERROR_ZERO_RETURN on port %s", ipc_info->ssl_port);
			LogEvent(szBuff, ERROR_MSG);
			break;

		case SSL_ERROR_WANT_READ:
		case SSL_ERROR_WANT_WRITE:
		case SSL_ERROR_WANT_CONNECT:
		case SSL_ERROR_SYSCALL:
		default:

			/* System call error.  This error is different from
			 * the SSL_ERROR_SSL in that errno (under unix)
			 * has the numeric error value, and is not converted
			 * into text.  If performing an SSL_read/SSL_write 
			 * there is no recorded error in the error logging
			 * because the error could be 'retryable' error which
			 * the library is not aware of.
			 */

			sprintf(szBuff, "ASCENDENT: ssl_client_read, read returned %d on port %s", j, ipc_info->ssl_port);
			LogEvent(szBuff, INFO_MSG);
			ERR_print_errors(ipc_info->bio_out); 
         error_condition = TRUE;
			break;
	}

   ssl_close(ipc_info);
   return(!error_condition);
}
/*------------------------------------------------------------------------------*/
BOOLEAN ssl_write(const WORD len, const void * data, pIPC_INFO ipc_info)
{
  WORD				tmp;
  INT					offset, return_code, found;
  pBYTE				buf;
  BOOL				length_bytes_include_themselves = FALSE;
  pSSL_LIST_ELEMENT pcur_element;
  SSL					*ssl;
  CHAR            szBuff[DM_ERRBUF_SIZE];

  /* set found to false for compare loop */
  found = FALSE;

  /* Idiot check the send buffer */
  if(!data)
  { 
    LogEvent("ASCENDENT: ssl_write, bad buffer pointer passed", ERROR_MSG);
    return(FALSE);
  }

  tmp = len;
  buf = malloc(tmp);
   
  if(!buf)
  {
    LogEvent("ASCENDENT: ssl_write, Cannot allocate Memory", ERROR_MSG);
    return(FALSE);
  }

  memcpy(buf, data, len);  /* move data after tcp header    */

  /* find the SSL structure in the linked list */

  while (SSLLinkedListLocked);
      pteipc_sleep(100);

  SSLLinkedListLocked = TRUE;

  pcur_element = ptellist_first_ssl_element(&SSLConnects);
  
  while (!found && (pcur_element != NULL_PTR))
  {
      if (strncmp(ipc_info->ssl_id, pcur_element->tran_id, strlen(ipc_info->ssl_id)) == 0)
      {
         found = TRUE;
         break;
	  }
      pcur_element = ptellist_next_ssl_element (pcur_element);
  }

  /* if the ssl_id matched, then extract the ssl structure for the write */
  if (found && (pcur_element != NULL_PTR))
	  ssl = &pcur_element->stored_ssl;
  else
  /* no match was found, so there is no ssl session available for this write */
  {
		LogEvent( "ASCENDENT: ssl_write, Unable to identify SSL session for write", ERROR_MSG );
		SSLLinkedListLocked = FALSE;
		if (buf != NULL)
			free(buf);
		ssl_close(ipc_info);
		return(FALSE);
  }

  /* make sure there is data in the buffer */
  if (buf == NULL) 
  {
		LogEvent( "ASCENDENT: ssl_write, Write buffer is empty", ERROR_MSG );
		SSLLinkedListLocked = FALSE;
		ssl_close(ipc_info);
		return(FALSE);
  }

  /* make sure the length to write is greater than 0 */
  if (len < 1)
  {
		LogEvent( "ASCENDENT: ssl_write, Write buffer len is less than 1", ERROR_MSG );
		SSLLinkedListLocked = FALSE;
		ssl_close(ipc_info);
		return(FALSE);
  }
 
  /* Write data to the socket */
  offset = 0;
  return_code=SSL_write(ssl,&(buf[offset]),len);

  /* Whether the read is successful or not successful, pcur_element will
     not be reused. Delete pcur_element from the linked list */
  ptellist_delete_ssl_element(&SSLConnects, pcur_element);

  /* check the status of the SSL_write call */
  if (return_code <= 0)
  {
		sprintf(szBuff, "ASCENDENT: ssl_read, error writing to port %s\n", ipc_info->ssl_port);
		LogEvent(szBuff, ERROR_MSG);
		SSLLinkedListLocked = FALSE;
		/* if there is  a valid temporary buffer, then free it */
		if (buf != NULL)
			free(buf);
		ssl_close(ipc_info);
		return(FALSE);
  }
   
  SSLLinkedListLocked = FALSE;
  
  /* if there is a valid temporary buffer, then free it */
  if (buf != NULL)
     free(buf);

  return(TRUE);
}

/*------------------------------------------------------------------------------*/
BOOLEAN client_ssl_write(const WORD len, const void * data, pIPC_INFO ipc_info)
{
  WORD	  tmp;
  INT		  offset, return_code;
  pBYTE	  buf;
  BOOL	  length_bytes_include_themselves = FALSE;
  CHAR     szBuff[DM_ERRBUF_SIZE];
  
  /* Idiot check the send buffer */
  if(!data)
  { 
    LogEvent("ASCENDENT: ssl_write, bad buffer pointer passed", ERROR_MSG);
    return(FALSE);
  }

  /* Create a temporary storage area */
  tmp = len;
  buf = malloc(tmp);
   
  if(!buf)
  {
    LogEvent("ASCENDENT: ssl_write, Cannot allocate Memory", ERROR_MSG);
    return(FALSE);
  }

  memcpy(buf, data, len);  

  /* make sure there is data in the buffer */
  if (buf == NULL) 
  {
		LogEvent( "ASCENDENT: ssl_write, Write buffer is empty", ERROR_MSG );
		ssl_close(ipc_info);
		return(FALSE);
  }

  /* make sure the length to write is greater than 0 */
  if (len < 1)
  {
		LogEvent( "ASCENDENT: ssl_write, Write buffer len is less than 1", ERROR_MSG );
		ssl_close(ipc_info);
		return(FALSE);
  }
 
  /* Write data to the socket */
  offset = 0;
  return_code=SSL_write(ipc_info->ssl,&(buf[offset]),len);

  /* check the status of the SSL_write call */
  if (return_code <= 0)
  {
		sprintf(szBuff, "ASCENDENT: ssl_read, error writing to port %s\n", ipc_info->ssl_port);
		LogEvent(szBuff, ERROR_MSG);
		/* if there is  a valid temporary buffer, then free it */
		if (buf != NULL)
			free(buf);
		ssl_close(ipc_info);
		return(FALSE);
  }
  
  /* if there is a valid temporary buffer, then free it */
  if (buf != NULL)
     free(buf);

  return(TRUE);
}

/*------------------------------------------------------------------------------*/
VOID ssl_close (pIPC_INFO ipc_info)
{

   CHAR     szBuff[DM_ERRBUF_SIZE];

   /* free applicable memory */
   if(ipc_info->ssl != NULL_PTR )
   {
      SSL_shutdown(ipc_info->ssl);
      SSL_free(ipc_info->ssl);

      /* client has closed the socket*/
      sprintf(szBuff, "ASCENDENT: ssl_shutdown,client has closed socket id %0ld on port %s", 
                              ipc_info->socket_id, ipc_info->ssl_port);
      LogEvent(szBuff, INFO_MSG);	

      pteipc_sleep(5000);
   }
   else
   {
      /* client has closed the socket*/
      sprintf(szBuff, "ASCENDENT: ssl_shutdown failed, ipc_info->ssl is NULL, socket id %0ld on port %s", 
                              ipc_info->socket_id, ipc_info->ssl_port);
      LogEvent(szBuff, ERROR_MSG);	
   }
}

/*------------------------------------------------------------------------------*/
VOID ssl_shutdown (pIPC_INFO ipc_info)
{
   ssl_close(ipc_info);

   if (ipc_info->ssl_ctx != NULL) 
	   SSL_CTX_free(ipc_info->ssl_ctx);

   if (ipc_info->bio_out != NULL) 
	   BIO_free(ipc_info->bio_out);

   if (ipc_info->bio_err != NULL) 
	   BIO_free(ipc_info->bio_err);

   if (ipc_info->abio != NULL) 
	   BIO_free(ipc_info->abio);

   SSL_library_cleanup();
}

/*------------------------------------------------------------------------*/
/* corrects a multithreading problem with RSA code by turning caching off */
/*------------------------------------------------------------------------*/
void fix_rsa_method()
{
	RSA_METHOD *rm;
	static RSA_METHOD new_rm;

	rm=RSA_PKCS1_SSLeay();
	memcpy((char *)&new_rm,(char *)rm,sizeof(new_rm));
	new_rm.init=NULL;
	new_rm.finish=NULL;
	RSA_set_default_method(&new_rm);
}

/*------------------------------------------------------------------------*/
/* generates a unique transaction id */
/*------------------------------------------------------------------------*/
void generate_new_transaction_id(char * tranid)
{
     struct timeb tt;
     char buffer1[10]; /* holds seconds */
     char buffer2[4];  /* holds milli seconds */
     char buffer3[5];  /* holds a random number */

     /* plant a random number seed must be done just once outside this function */
     if (sequencer > 9999) sequencer = 0;
     sequencer++;
     ftime(&tt);
     memset(buffer1, 0, sizeof(buffer1));
     memset(buffer2, 0, sizeof(buffer2));
     memset(buffer2, 0, sizeof(buffer2));
     sprintf(buffer1,"%09d", tt.time);
     sprintf(buffer2,"%03d", tt.millitm);
     sprintf(buffer3,"%04d", sequencer);
     /* tranid = seconds(5) + milli seconds(3) + random number(4) */
     
     memcpy(tranid, buffer1 + 4, 5);   /* last 5 significant bytes of second */
     memcpy(tranid + 5, buffer2, 3);
     memcpy(tranid + 8, buffer3, 4);
}

#endif




