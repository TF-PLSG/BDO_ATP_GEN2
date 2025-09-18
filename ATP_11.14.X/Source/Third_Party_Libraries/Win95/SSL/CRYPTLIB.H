/* crypto/cryptlib.h */
/*
 * Copyright (C) 1998 RSA Data Security Australia Pty Ltd. All rights
 * reserved. This work contains proprietary information of RSA Data Security
 * Australia Pty Ltd. Distribution is limited to authorised licensees of
 * RSA Data Security Australia Pty Ltd. Any unauthorised reproduction or
 * distribution of this work is strictly prohibited.
 */

#ifndef HEADER_CRYPTLIB_H
#define HEADER_CRYPTLIB_H

#include <stdlib.h>
#include <string.h>

#ifdef  __cplusplus
extern "C" {
#endif

/* #ifdef FLAT_INC */

#include "e_os.h"
#include "crypto.h"
#include "buffer.h" 
#include "bio.h" 
#include "err.h"

/*
#else

#include "../e_os.h"
#include "crypto.h"
#include "buffer/buffer.h"
#include "bio/bio.h"
#include "err/err.h"
#endif
*/

#define X509_CERT_AREA		"/usr/local/ssl"
#define X509_CERT_DIR		"/usr/local/ssl/certs"
#define X509_CERT_FILE		"/usr/local/ssl/cert.pem"
#define X509_PRIVATE_DIR        "/usr/local/ssl/private"

#define X509_CERT_DIR_EVP        "SSL_CERT_DIR"
#define X509_CERT_FILE_EVP       "SSL_CERT_FILE"

#ifdef  __cplusplus
}
#endif

#endif
