/* e_os.h */
/*
 * Copyright (C) 1998 RSA Data Security Australia Pty Ltd. All rights
 * reserved. This work contains proprietary information of RSA Data Security
 * Australia Pty Ltd. Distribution is limited to authorised licensees of
 * RSA Data Security Australia Pty Ltd. Any unauthorised reproduction or
 * distribution of this work is strictly prohibited.
 */

#ifndef HEADER_E_OS_H
#define HEADER_E_OS_H

#ifdef  __cplusplus
extern "C" {
#endif

/* Used to checking reference counts, most while doing perl5 stuff :-) */
#ifdef REF_PRINT
#undef REF_PRINT
#define REF_PRINT(a,b)	fprintf(stderr,"%08X:%4d:%s\n",(int)b,b->references,a)
#endif

#ifndef DEVRANDOM
/* set this to your 'random' device if you have one.
 * My default, we will try to read this file */
#define DEVRANDOM "/dev/urandom"
#endif

#if defined(NOCONST)
#define const
#endif

/********************************************************************
 The Microsoft section
 ********************************************************************/
/* The following is used becaue of the small stack in some
 * Microsoft operating systems */
#if defined(WIN16) || defined(MSDOS)
#  define MS_STATIC	static
#else
#  define MS_STATIC
#endif

#if defined(WIN32) || defined(WIN16)
#  ifndef WINDOWS
#    define WINDOWS
#  endif
#  ifndef MSDOS
#    define MSDOS
#  endif
#endif

#ifdef WIN32
#define get_last_sys_error()	GetLastError()
#define clear_sys_error()	SetLastError(0)
#else
#define get_last_sys_error()	errno
#define clear_sys_error()	errno=0
#endif

#ifdef WINDOWS
#define get_last_socket_error()	WSAGetLastError()
#define clear_socket_error()	WSASetLastError(0)
#define readsocket(s,b,n)	recv((s),(b),(n),0)
#define writesocket(s,b,n)	send((s),(b),(n),0)
#define EADDRINUSE		WSAEADDRINUSE
#else
#define get_last_socket_error()	errno
#define clear_socket_error()	errno=0
#define ioctlsocket(a,b,c)	ioctl(a,b,c)
#define closesocket(s)		close(s)
#define readsocket(s,b,n)	read((s),(b),(n))
#define writesocket(s,b,n)	write((s),(b),(n))
#endif

#ifdef WIN16
#  define NO_FP_API
#  define MS_CALLBACK	_far _loadds
#  define MS_FAR	_far
#else
#  define MS_CALLBACK
#  define MS_FAR
#endif

#ifdef NO_STDIO
#  define NO_FP_API
#endif

#if defined(WINDOWS) || defined(MSDOS)

#ifndef S_IFDIR
#define S_IFDIR	_S_IFDIR
#endif

#ifndef S_IFMT
#define S_IFMT	_S_IFMT
#endif

#define strncasecmp(a,b,c)	strnicmp((a),(b),(c))

#  ifdef WINDOWS
#    undef WIN32_LEAN_AND_MEAN
#    define WIN32_LEAN_AND_MEAN
#    include <windows.h>
#    include <stddef.h>
#    include <errno.h>
#    include <string.h>
#    include <malloc.h>
#  endif
#  include <io.h>
#  include <fcntl.h>

#if defined(WIN16) && !defined(MONOLITH) && defined(SSLEAY) && defined(_WINEXITNOPERSIST)
#  define EXIT(n) { if (n == 0) _wsetexit(_WINEXITNOPERSIST); return(n); }
#else
#  define EXIT(n)		return(n);
#endif
#  define LIST_SEPARATOR_CHAR ';'
#ifndef X_OK
#  define X_OK	0
#endif
#ifndef W_OK
#  define W_OK	2
#endif
#ifndef R_OK
#  define R_OK	4
#endif
#if 0
#  define SSLEAY_CONF	"ssleay.cnf"
#endif
#  define SSLEAY_CONF	"sslc.cnf"
#  define NUL_DEV	"nul"
#  define RFILE		".rnd"

#else /* The non-microsoft world world */

#  ifdef VMS
#    include <unixlib.h>
#  else
#    include <unistd.h>
#  endif

#if 0
#  define SSLEAY_CONF	"ssleay.cnf"
#endif
#  define SSLEAY_CONF	"sslc.cnf"
#  define RFILE		".rnd"
#  define LIST_SEPARATOR_CHAR ':'
#  ifndef MONOLITH
#    define EXIT(n)		exit(n); return(n)
#  else
#    define EXIT(n)		return(n)
#  endif
#  define NUL_DEV		"/dev/null"

#  define SSLeay_getpid()	getpid()

#endif

/*************/

#ifdef USE_SOCKETS
#  if defined(WINDOWS) || defined(MSDOS)
      /* windows world */

#    ifdef NO_SOCK
#      define SSLeay_Write(a,b,c)	(-1)
#      define SSLeay_Read(a,b,c)	(-1)
#      define SHUTDOWN(fd)		close(fd)
#      define SHUTDOWN2(fd)		close(fd)
#    else
#      include <winsock.h>
extern HINSTANCE _hInstance;
#      define SSLeay_Write(a,b,c)	send((a),(b),(c),0)
#      define SSLeay_Read(a,b,c)	recv((a),(b),(c),0)
#      define SHUTDOWN(fd)		{ shutdown((fd),0); closesocket(fd); }
#      define SHUTDOWN2(fd)		{ shutdown((fd),2); closesocket(fd); }
#    endif


#  else

#    ifndef VMS
      /* unix world */
#      include <netdb.h>
#      include <sys/types.h>
#      include <sys/socket.h>
#      ifdef FILIO_H
#        include <sys/filio.h> /* Added for FIONBIO under unixware */
#      endif
#      include <sys/param.h>
#      include <sys/time.h> /* Needed under linux for FD_XXX */
#      include <netinet/in.h>
#    endif

#    if defined(NeXT) || defined(_NEXT_SOURCE)
#      include <sys/fcntl.h>
#      include <sys/types.h>
#    endif

#    ifdef AIX
#      include <sys/select.h>
#    endif

#    if defined(sun)
#      include <sys/filio.h>
#    else
#      include <sys/ioctl.h>
#    endif

#    ifdef VMS
#      include <unixio.h>
#    endif

#    define SSLeay_Read(a,b,c)     read((a),(b),(c))
#    define SSLeay_Write(a,b,c)    write((a),(b),(c))
#    define SHUTDOWN(fd)    { shutdown((fd),0); close((fd)); }
#    define SHUTDOWN2(fd)   { shutdown((fd),2); close((fd)); }
#    define INVALID_SOCKET	(-1)
#  endif
#endif

#if defined(THREADS) || defined(sun)
#ifndef _REENTRANT
#define _REENTRANT
#endif
#endif

/***********************************************/

#ifndef NOPROTO
#define P_CC_CC	const void *,const void *
#define P_I_I		int,int 
#define P_I_I_P		int,int,char *
#define P_I_I_P_I	int,int,char *,int
#define P_IP_I_I_P_I	int *,int,int,char *,int
#define P_V		void 
#else
#define P_CC_CC
#define P_I_I
#define P_I_I_P
#define P_IP_I_I_P_I
#define P_I_I_P_I
#define P_V
#endif

/* not used yet */
#define	CS_BEGIN
#define CS_END

/* do we need to do this for getenv.
 * Just define getenv for use under windows */

#ifdef WIN16
/* How to do this needs to be thought out a bit more.... */
/*char *GETENV(char *);
#define Getenv	GETENV*/
#define Getenv	getenv
#else
#define Getenv getenv
#endif

#define DG_GCC_BUG	/* gcc < 2.6.3 on DGUX */

#ifdef sgi
#define IRIX_CC_BUG	/* all version of IRIX I've tested (4.* 5.*) */
#endif

#ifdef NO_MD2
#define MD2_Init MD2Init
#define MD2_Update MD2Update
#define MD2_Final MD2Final
#define MD2_DIGEST_LENGTH 16
#endif
#ifdef NO_MD5
#define MD5_Init MD5Init 
#define MD5_Update MD5Update
#define MD5_Final MD5Final
#define MD5_DIGEST_LENGTH 16
#endif

#ifdef  __cplusplus
}
#endif

#endif

