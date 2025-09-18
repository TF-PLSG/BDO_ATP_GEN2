/********************************************************** XIPCPUBD.H
**		 (XIPC - Extended Interprocess Communications Faclilities)
**
**	Name:	XIPCPUBD.H
**	----
**
**	Title:	XIPC Public Header File - Declarations
**	-----
**
**	Description:
**	-----------
**	This file contains public declarations common
**	to all XIPC systems.
**
**	Last Revision:
**	-------------
**	$Revision:	$
**	$Date:	$
**	$Author:	$
**
**	Revision History:
**	----------------
**
**	Rev   Date		By	 Description
**	---   --------	---  -----------
**	250b   2-15-95	YF	 Change XIPC_LEN_PARMLINE to 120
**
**
**	Copyright (C) Momentum Software Corporation, 1989
**
*********************************************************************/

#ifndef XIPCPUBD_H
#define XIPCPUBD_H XIPCPUBD_H

/*********************************************************************
** XIPC Current Version
*********************************************************************/

#define XIPC_VERSION		"3.0.2"

#define XIPC_VERSION_MAJOR	3
#define XIPC_VERSION_MINOR	0
#define XIPC_VERSION_LEVEL	2
#define XIPC_BUILD_COMMON	'a'

#define XIPC_BUILD_NUMBER	302

/*********************************************************************
** constants
*********************************************************************/

/* XIPC system ids */		/* *** DO NOT CHANGE VALUES *** */

#define XIPC_ID_NETWORK 	((XINT)0)
#define XIPC_ID_SEMSYS		((XINT)1)
#define XIPC_ID_QUESYS		((XINT)2)
#define XIPC_ID_MEMSYS		((XINT)3)

/* added 1/8/97; note that XIPC_ID_MAXSYS remains = 4 */
#define XIPC_ID_MOMSYS		((XINT)4)

#define XIPC_ID_MAXSYS		((XINT)4)	/* maximum number of systems */

/* freeze options */

#define XIPC_MASKTRAPS		((XINT)1)
#define XIPC_NOMASKTRAPS	((XINT)2)


/* start/stop options */

#define XIPC_START_NOOPT		((XINT)0)
#define XIPC_START_REPORT		((XINT)1)
#define XIPC_START_TEST 		((XINT)2)
#define XIPC_START_LOCAL		((XINT)4)
#define XIPC_START_NETWORK		((XINT)32)
#define XIPC_START_INITIALIZE	((XINT)64)

#define XIPC_STOP_NOOPT 		((XINT)0)
#define XIPC_STOP_REPORT		((XINT)1)
#define XIPC_STOP_FORCE 		((XINT)2)

/* Error Code options */

#define XIPC_ERROR_NET			((XINT)1)
#define XIPC_ERROR_OS			((XINT)2)
#define XIPC_ERROR_MRI			((XINT)3)
#define XIPC_ERROR_MRO			((XINT)4)

#define XIPC_ERROR_LOCAL		((XINT)11)
#define XIPC_ERROR_REMOTE		((XINT)12)

/* maximum sizes and limits */

#define XIPC_LEN_NETNAME	((XINT)255)
#define XIPC_LEN_PATHNAME	((XINT)255)
#define XIPC_LEN_USERNAME	((XINT)31)
#define XIPC_LEN_XIPCNAME	((XINT)31)
#define XIPC_LEN_PARMLINE	((XINT)120)
#define XIPC_LEN_FUNCNAME	((XINT)64)
#define XIPC_LEN_ERRORMSG	((XINT)128)
#define XIPC_LEN_TIMELIMIT	((XINT)31)
#define XIPC_LEN_SYSNAME	((XINT)8)
#define XIPC_LEN_NETLOC 	((XINT)7)
#define XIPC_LEN_DTSDATATYPE	XIPC_LEN_XIPCNAME
#define XIPC_LEN_VERSION	((XINT)63)
#define XIPC_MAX_SIGNALS	XIPC_SIG_HI

/* miscellaneous option flags */

#define XIPC_FREEZE 		((XINT)0x00010000)
#define XIPC_CLEAR			((XINT)0x00020000)
#define XIPC_NOCLEAR		((XINT)0x00040000)
#define XIPC_NOREMOVE		((XINT)0x00080000)
#define XIPC_FINISHWRITE	((XINT)0x00100000) /* XIPC internal */
#define XIPC_RETURN 		((XINT)0x00200000)
#define XIPC_ALLOC			((XINT)0x00400000) /* XIPC internal */
#define XIPC_DETACH 		((XINT)0x00800000)
#define XIPC_FASTPATH		((XINT)0x00F00000)

#define XIPC_RETSEQ 		((XINT)0x01000000)
#define XIPC_RETPRIO		((XINT)0x02000000)

#define XIPC_USER_NULL		((XINT)-1)

/* Login Flags */

#define XIPC_LOGIN_FLAG_SUPERUSER ((XINT)1)
#define XIPC_LOGIN_SUPERUSER	"SuperUser"

/* blocking options */

#define XIPC_BLOCKOPT_WAIT		((XINT)0x00000001)
#define XIPC_BLOCKOPT_NOWAIT	((XINT)0x00000002)
#define XIPC_BLOCKOPT_TIMEOUT	((XINT)0x00000004)
#define XIPC_BLOCKOPT_SYNC		((XINT)0x00000007)

#define XIPC_BLOCKOPT_CALLBACK	((XINT)0x00000010)
#define XIPC_BLOCKOPT_POST		((XINT)0x00000020)
#define XIPC_BLOCKOPT_IGNORE	((XINT)0x00000040)
#define XIPC_BLOCKOPT_SPAWN 	((XINT)0x00000080)
#define XIPC_BLOCKOPT_ASYNC 	((XINT)0x000000F0)

#define XIPC_BLOCKOPT_REPLICATE ((XINT)0x00000100)
#define XIPC_BLOCKOPT_REPLACE	((XINT)0x00000200)

#define XIPC_NULL_CALLBACK	(XIPC_CALLBACK_FUNC *)0

#define XIPC_WAIT			XIPC_BLOCKOPT_WAIT,0, NULL, XIPC_NULL_CALLBACK
#define XIPC_NOWAIT 		XIPC_BLOCKOPT_NOWAIT, 0, NULL, XIPC_NULL_CALLBACK
#define XIPC_TIMEOUT(x) 	\
						XIPC_BLOCKOPT_TIMEOUT, (x), NULL, XIPC_NULL_CALLBACK
#define XIPC_CALLBACK(f,r)	XIPC_BLOCKOPT_CALLBACK, 0, (r), (f)
#define XIPC_POST(s,r)		XIPC_BLOCKOPT_POST, (s), (r), XIPC_NULL_CALLBACK
#define XIPC_IGNORE(r)		XIPC_BLOCKOPT_IGNORE, 0, (r), XIPC_NULL_CALLBACK
#define XIPC_SPAWN(c, r)	XIPC_BLOCKOPT_SPAWN, 0, (r), (c)
#define XIPC_REPLICATE		XIPC_BLOCKOPT_REPLICATE, 0, NULL, XIPC_NULL_CALLBACK
#define XIPC_REPLACE		XIPC_BLOCKOPT_REPLACE, 0, NULL, XIPC_NULL_CALLBACK

/*	XipcLoginInfo Options	*/

#define XIPC_LOGIN_INIT_ENUMERATION 0
#define XIPC_LOGIN_CURRENT			((XINT)-1), NULL
#define XIPC_LOGIN_COUNT			NULL, ((XINT)0), NULL
#define XIPC_LOGIN_NEXT(c)			((XINT)1), &(c)

/* XIPC Function OpCodes - Async Operations */

#define XIPC_OPCODE_SEMWAIT 			((XINT)1)
#define XIPC_OPCODE_SEMACQUIRE			((XINT)2)

#define XIPC_OPCODE_QUEWRITE			((XINT)3)
#define XIPC_OPCODE_QUEPUT				((XINT)4)
#define XIPC_OPCODE_QUEGET				((XINT)5)
#define XIPC_OPCODE_QUESEND 			((XINT)6)
#define XIPC_OPCODE_QUESENDWRITE		((XINT)7)
#define XIPC_OPCODE_QUESENDPUT			((XINT)8)
#define XIPC_OPCODE_QUERECEIVE			((XINT)9)
#define XIPC_OPCODE_QUERECEIVEGET		((XINT)10)
#define XIPC_OPCODE_QUEBURSTSEND		((XINT)11)
#define XIPC_OPCODE_QUEBURSTSENDSYNC	((XINT)12)

#define XIPC_OPCODE_MEMWRITE			((XINT)13)
#define XIPC_OPCODE_MEMREAD 			((XINT)14)
#define XIPC_OPCODE_MEMSECOWN			((XINT)15)
#define XIPC_OPCODE_MEMLOCK 			((XINT)16)

#define XIPC_OPCODE_MOMSEND 			((XINT)17)
#define XIPC_OPCODE_MOMRECEIVE			((XINT)18)
#define XIPC_OPCODE_MOMEVENT			((XINT)19)

/* internal error codes */

#define XIPC_ER_INTR	EINTR
#define XIPC_ER_IDRM	EIDRM

/* semaphore initialization flags */

#define XIPC_SEM_FREE	((XINT)0)
#define XIPC_SEM_HELD	((XINT)1)

/* private resource name */

#define XIPC_PRIVATE	"@PRIVATE"

/* XIPC environment variable */

#define XIPC_ENVNAME	"XIPC"

/* Wakeup Code Values */

#define XIPC_WAKEDATA_LOCAL 	((XINT)1)
#define XIPC_WAKEDATA_REMOTE	((XINT)2)
#define XIPC_WAKEDATA_EOD		((XINT)3)

/* user status */

#define XIPC_USER_NOTWAITING	(-999L)

/* XIPC network location defines */

#define XIPC_NET_LOCAL		"*LOCAL*"
#define XIPC_LEN_HOSTNAME	((XINT)255)

/* Xipc IdleWatch Flags */
#define XIPC_IDLEWATCH_START		(0x01)
#define XIPC_IDLEWATCH_STOP 		(0x02)
#define XIPC_IDLEWATCH_MARK 		(0x04)

/* Xipc SetOpt OptCodes */

#define XIPC_OPTCODE_PINGTIMEOUT		((XINT)1)
#define XIPC_OPTCODE_PINGRETRIES		((XINT)2)
#define XIPC_OPTCODE_RECVTIMEOUT		((XINT)3)
#define XIPC_OPTCODE_ASYNCFD			((XINT)4)
#define XIPC_OPTCODE_MAXTHREADS 		((XINT)5)
#define XIPC_OPTCODE_MAXTEXTSIZE		((XINT)6)
#define XIPC_OPTCODE_READAHEAD			((XINT)7)
#define XIPC_OPTCODE_NETXMTSIZE 		((XINT)8)
#define XIPC_OPTCODE_PRIVATEQUEUE		((XINT)9)
#define XIPC_OPTCODE_CONNECTTIMEOUT 	((XINT)10)

#define XIPC_OPTCODE_PINGFUNCTION		((XINT)21)
#define XIPC_OPTCODE_CRITSECEXIT		((XINT)22)

#define XIPC_OPTCODE_CAT				((XINT)31)
#define XIPC_OPTCODE_CATLIST			((XINT)32)
#define XIPC_OPTCODE_HOST				((XINT)33)
#define XIPC_OPTCODE_HOSTLIST			((XINT)34)

/* Info Options */

#define XIPC_INFO_FIRST 		((XINT)0x7FFFFFFF)
#define XIPC_INFO_NEXT(Id)		(-((Id)+1))

/* Non-Contiguous Buffer Option */

#define XIPC_NONCONTIG(ListAddr)	XipcArgNonContig(ListAddr)

/*********************************************************************
** macros -
*********************************************************************/

#define XIPC_TRAP_FUNCTION_TEST(FUNC, SIG) \
	if (XipcTrap((INT)(SIG), (FUNC))) \
		return;

/* Macros For the XipcSetOpt Function */

#define XIPC_SETOPT_PINGTIMEOUT(t)	\
			XIPC_OPTCODE_PINGTIMEOUT, (XINT) (t), 0, 0, 0, NULL, NULL, NULL

#define XIPC_GETOPT_PINGTIMEOUT(t)	\
			XIPC_OPTCODE_PINGTIMEOUT, 0, 0, (t), NULL, NULL, NULL, NULL, NULL

#define XIPC_SETOPT_PINGRETRIES(t)	\
			XIPC_OPTCODE_PINGRETRIES, (XINT) (t), 0, 0, 0, NULL, NULL, NULL

#define XIPC_GETOPT_PINGRETRIES(t)	\
			XIPC_OPTCODE_PINGRETRIES, 0, 0, (t), NULL, NULL, NULL, NULL, NULL

#define XIPC_SETOPT_RECVTIMEOUT(t)	\
			XIPC_OPTCODE_RECVTIMEOUT, (XINT) (t), 0, 0, 0, NULL, NULL, NULL

#define XIPC_GETOPT_RECVTIMEOUT(t)	\
			XIPC_OPTCODE_RECVTIMEOUT, 0, 0, (t), NULL, NULL, NULL, NULL, NULL

#define XIPC_SETOPT_PINGFUNCTION(f) \
			XIPC_OPTCODE_PINGFUNCTION, 0, 0, 0, 0, NULL, NULL, (f)

#define XIPC_GETOPT_PINGFUNCTION(f) \
			XIPC_OPTCODE_PINGFUNCTION, 0, 0, NULL, NULL, NULL, NULL, NULL, (f)

#define XIPC_SETOPT_CRITSECEXIT(f)	\
			XIPC_OPTCODE_CRITSECEXIT, 0, 0, 0, 0, NULL, NULL, (f)

#define XIPC_GETOPT_CRITSECEXIT(f)	\
			XIPC_OPTCODE_CRITSECEXIT, 0, 0, NULL, NULL, NULL, NULL, NULL, (f)

#define XIPC_SETOPT_ASYNCFD 		\
			XIPC_OPTCODE_ASYNCFD, 0, 0, 0, 0, NULL, NULL, NULL

#define XIPC_GETOPT_ASYNCFD(t)		\
			XIPC_OPTCODE_ASYNCFD, 0, 0, (t), NULL, NULL, NULL, NULL, NULL

#define XIPC_SETOPT_MAXTEXTSIZE(t)	\
			XIPC_OPTCODE_MAXTEXTSIZE, (XINT) (t), 0, 0, 0, NULL, NULL, NULL

#define XIPC_GETOPT_MAXTEXTSIZE(t)	\
			XIPC_OPTCODE_MAXTEXTSIZE, 0, 0, (t), NULL, NULL, NULL, NULL, NULL

#define XIPC_SETOPT_PRIVATEQUEUE \
			XIPC_OPTCODE_PRIVATEQUEUE, 0, 0, 0, 0, NULL, NULL, NULL

#define XIPC_GETOPT_PRIVATEQUEUE(t) \
			XIPC_OPTCODE_PRIVATEQUEUE, 0, 0, (t), NULL, NULL, NULL, NULL, NULL

#define XIPC_SETOPT_MAXTHREADS(t)	\
			XIPC_OPTCODE_MAXTHREADS, (XINT) (t), 0, 0, 0, NULL, NULL, NULL

#define XIPC_GETOPT_MAXTHREADS(t)	\
			XIPC_OPTCODE_MAXTHREADS, 0, 0, (t), NULL, NULL, NULL, NULL, NULL

#define XIPC_SETOPT_READAHEAD(t)	\
			XIPC_OPTCODE_READAHEAD, (XINT) (t), 0, 0, 0, NULL, NULL, NULL

#define XIPC_GETOPT_READAHEAD(t)	\
			XIPC_OPTCODE_READAHEAD, 0, 0, (t), NULL, NULL, NULL, NULL, NULL

#define XIPC_SETOPT_NETXMTSIZE(t)	\
			XIPC_OPTCODE_NETXMTSIZE, (XINT) (t), 0, 0, 0, NULL, NULL, NULL

#define XIPC_GETOPT_NETXMTSIZE(t)	\
			XIPC_OPTCODE_NETXMTSIZE, 0, 0, (t), NULL, NULL, NULL, NULL, NULL

#define XIPC_SETOPT_CAT(x)			\
			XIPC_OPTCODE_CAT, 0, 0, 0, 0, (x), NULL, NULL

#define XIPC_GETOPT_CAT(x)			\
			XIPC_OPTCODE_CAT, 0, 0, (x), NULL, NULL, NULL, NULL, NULL

#define XIPC_SETOPT_HOST(x) 		\
			XIPC_OPTCODE_HOST, 0, 0, 0, 0, (x), NULL, NULL

#define XIPC_GETOPT_HOST(x) 		\
			XIPC_OPTCODE_HOST, 0, 0, (x), NULL, NULL, NULL, NULL, NULL

#define XIPC_SETOPT_CATLIST(x)		\
			XIPC_OPTCODE_CATLIST, 0, 0, 0, 0, (x), NULL, NULL

#define XIPC_GETOPT_CATLIST(x)		\
			XIPC_OPTCODE_CATLIST, 0, 0, (x), NULL, NULL, NULL, NULL, NULL

#define XIPC_SETOPT_HOSTLIST(x) 	\
			XIPC_OPTCODE_HOSTLIST, 0, 0, 0, 0, (x), NULL, NULL

#define XIPC_GETOPT_HOSTLIST(x) 	\
			XIPC_OPTCODE_HOSTLIST, 0, 0, (x), NULL, NULL, NULL, NULL, NULL

#define XIPC_SETOPT_CONNECTTIMEOUT(t)	\
			XIPC_OPTCODE_CONNECTTIMEOUT, (XINT) (t), 0, 0, 0, NULL, NULL, NULL

#define XIPC_GETOPT_CONNECTTIMEOUT(t)	\
			XIPC_OPTCODE_CONNECTTIMEOUT, 0, 0, (t), NULL, NULL, NULL, NULL, NULL

/*********************************************************************
** Prototype macros -
*********************************************************************/
#ifdef ANSI_PROTOTYPES

#define A0() ()

#define A1(t1, n1) \
					(t1 n1)
#define A2(t1, n1, t2, n2) \
					(t1 n1, t2 n2)
#define A3(t1, n1, t2, n2, t3, n3) \
					(t1 n1, t2 n2, t3 n3)
#define A4(t1, n1, t2, n2, t3, n3, t4, n4) \
					(t1 n1, t2 n2, t3 n3, t4 n4)
#define A5(t1, n1, t2, n2, t3, n3, t4, n4, t5, n5) \
					(t1 n1, t2 n2, t3 n3, t4 n4, t5 n5)
#define A6(t1, n1, t2, n2, t3, n3, t4, n4, t5, n5, t6, n6) \
					(t1 n1, t2 n2, t3 n3, t4 n4, t5 n5, t6 n6)
#define A7(t1, n1, t2, n2, t3, n3, t4, n4, t5, n5, t6, n6, t7, n7) \
					(t1 n1, t2 n2, t3 n3, t4 n4, t5 n5, t6 n6, t7 n7)
#define A8(t1, n1, t2, n2, t3, n3, t4, n4, t5, n5, t6, n6, t7, n7, t8, n8) \
					(t1 n1, t2 n2, t3 n3, t4 n4, t5 n5, t6 n6, t7 n7, t8 n8)
#define A9(t1, n1, t2, n2, t3, n3, t4, n4, t5, n5, t6, n6, t7, n7, t8, n8, t9, n9) \
					(t1 n1, t2 n2, t3 n3, t4 n4, t5 n5, t6 n6, t7 n7, t8 n8, t9 n9)
#define A10(t1, n1, t2, n2, t3, n3, t4, n4, t5, n5, t6, n6, t7, n7, t8, n8, t9, n9, t10, n10) \
					(t1 n1, t2 n2, t3 n3, t4 n4, t5 n5, t6 n6, t7 n7, t8 n8, t9 n9, t10 n10)
#define A11(t1, n1, t2, n2, t3, n3, t4, n4, t5, n5, t6, n6, t7, n7, t8, n8, t9, n9, t10, n10, t11, n11) \
					(t1 n1, t2 n2, t3 n3, t4 n4, t5 n5, t6 n6, t7 n7, t8 n8, t9 n9, t10 n10, t11 n11)
#define A12(t1, n1, t2, n2, t3, n3, t4, n4, t5, n5, t6, n6, t7, n7, t8, n8, t9, n9, t10, n10, t11, n11, t12, n12) \
					(t1 n1, t2 n2, t3 n3, t4 n4, t5 n5, t6 n6, t7 n7, t8 n8, t9 n9, t10 n10, t11 n11, t12 n12)
#define A13(t1, n1, t2, n2, t3, n3, t4, n4, t5, n5, t6, n6, t7, n7, t8, n8, t9, n9, t10, n10, t11, n11, t12, n12, t13, n13) \
					(t1 n1, t2 n2, t3 n3, t4 n4, t5 n5, t6 n6, t7 n7, t8 n8, t9 n9, t10 n10, t11 n11, t12 n12, t13 n13)
#define A14(t1, n1, t2, n2, t3, n3, t4, n4, t5, n5, t6, n6, t7, n7, t8, n8, t9, n9, t10, n10, t11, n11, t12, n12, t13, n13, t14, n14) \
					(t1 n1, t2 n2, t3 n3, t4 n4, t5 n5, t6 n6, t7 n7, t8 n8, t9 n9, t10 n10, t11 n11, t12 n12, t13 n13, t14 n14)
#define A15(t1, n1, t2, n2, t3, n3, t4, n4, t5, n5, t6, n6, t7, n7, t8, n8, t9, n9, t10, n10, t11, n11, t12, n12, t13, n13, t14, n14, t15, n15) \
					(t1 n1, t2 n2, t3 n3, t4 n4, t5 n5, t6 n6, t7 n7, t8 n8, t9 n9, t10 n10, t11 n11, t12 n12, t13 n13, t14 n14, t15 n15)
#define A16(t1, n1, t2, n2, t3, n3, t4, n4, t5, n5, t6, n6, t7, n7, t8, n8, t9, n9, t10, n10, t11, n11, t12, n12, t13, n13, t14, n14, t15, n15, t16, n16) \
					(t1 n1, t2 n2, t3 n3, t4 n4, t5 n5, t6 n6, t7 n7, t8 n8, t9 n9, t10 n10, t11 n11, t12 n12, t13 n13, t14 n14, t15 n15, t16 n16)
#define A17(t1, n1, t2, n2, t3, n3, t4, n4, t5, n5, t6, n6, t7, n7, t8, n8, t9, n9, t10, n10, t11, n11, t12, n12, t13, n13, t14, n14, t15, n15, t16, n16, t17, n17) \
					(t1 n1, t2 n2, t3 n3, t4 n4, t5 n5, t6 n6, t7 n7, t8 n8, t9 n9, t10 n10, t11 n11, t12 n12, t13 n13, t14 n14, t15 n15, t16 n16, t17 n17)
#define A18(t1, n1, t2, n2, t3, n3, t4, n4, t5, n5, t6, n6, t7, n7, t8, n8, t9, n9, t10, n10, t11, n11, t12, n12, t13, n13, t14, n14, t15, n15, t16, n16, t17, n17, t18, n18) \
					(t1 n1, t2 n2, t3 n3, t4 n4, t5 n5, t6 n6, t7 n7, t8 n8, t9 n9, t10 n10, t11 n11, t12 n12, t13 n13, t14 n14, t15 n15, t16 n16, t17 n17, t18 n18)

#define P0() (void)

#define P1(t1, n1) \
					(t1 n1)
#define P2(t1, n1, t2, n2) \
					(t1 n1, t2 n2)
#define P3(t1, n1, t2, n2, t3, n3) \
					(t1 n1, t2 n2, t3 n3)
#define P4(t1, n1, t2, n2, t3, n3, t4, n4) \
					(t1 n1, t2 n2, t3 n3, t4 n4)
#define P5(t1, n1, t2, n2, t3, n3, t4, n4, t5, n5) \
					(t1 n1, t2 n2, t3 n3, t4 n4, t5 n5)
#define P6(t1, n1, t2, n2, t3, n3, t4, n4, t5, n5, t6, n6) \
					(t1 n1, t2 n2, t3 n3, t4 n4, t5 n5, t6 n6)
#define P7(t1, n1, t2, n2, t3, n3, t4, n4, t5, n5, t6, n6, t7, n7) \
					(t1 n1, t2 n2, t3 n3, t4 n4, t5 n5, t6 n6, t7 n7)
#define P8(t1, n1, t2, n2, t3, n3, t4, n4, t5, n5, t6, n6, t7, n7, t8, n8) \
					(t1 n1, t2 n2, t3 n3, t4 n4, t5 n5, t6 n6, t7 n7, t8 n8)
#define P9(t1, n1, t2, n2, t3, n3, t4, n4, t5, n5, t6, n6, t7, n7, t8, n8, t9, n9) \
					(t1 n1, t2 n2, t3 n3, t4 n4, t5 n5, t6 n6, t7 n7, t8 n8, t9 n9)
#define P10(t1, n1, t2, n2, t3, n3, t4, n4, t5, n5, t6, n6, t7, n7, t8, n8, t9, n9, t10, n10) \
					(t1 n1, t2 n2, t3 n3, t4 n4, t5 n5, t6 n6, t7 n7, t8 n8, t9 n9, t10 n10)
#define P11(t1, n1, t2, n2, t3, n3, t4, n4, t5, n5, t6, n6, t7, n7, t8, n8, t9, n9, t10, n10, t11, n11) \
					(t1 n1, t2 n2, t3 n3, t4 n4, t5 n5, t6 n6, t7 n7, t8 n8, t9 n9, t10 n10, t11 n11)
#define P12(t1, n1, t2, n2, t3, n3, t4, n4, t5, n5, t6, n6, t7, n7, t8, n8, t9, n9, t10, n10, t11, n11, t12, n12) \
					(t1 n1, t2 n2, t3 n3, t4 n4, t5 n5, t6 n6, t7 n7, t8 n8, t9 n9, t10 n10, t11 n11, t12 n12)
#define P13(t1, n1, t2, n2, t3, n3, t4, n4, t5, n5, t6, n6, t7, n7, t8, n8, t9, n9, t10, n10, t11, n11, t12, n12, t13, n13) \
					(t1 n1, t2 n2, t3 n3, t4 n4, t5 n5, t6 n6, t7 n7, t8 n8, t9 n9, t10 n10, t11 n11, t12 n12, t13 n13)
#define P14(t1, n1, t2, n2, t3, n3, t4, n4, t5, n5, t6, n6, t7, n7, t8, n8, t9, n9, t10, n10, t11, n11, t12, n12, t13, n13, t14, n14) \
					(t1 n1, t2 n2, t3 n3, t4 n4, t5 n5, t6 n6, t7 n7, t8 n8, t9 n9, t10 n10, t11 n11, t12 n12, t13 n13, t14 n14)
#define P15(t1, n1, t2, n2, t3, n3, t4, n4, t5, n5, t6, n6, t7, n7, t8, n8, t9, n9, t10, n10, t11, n11, t12, n12, t13, n13, t14, n14, t15, n15) \
					(t1 n1, t2 n2, t3 n3, t4 n4, t5 n5, t6 n6, t7 n7, t8 n8, t9 n9, t10 n10, t11 n11, t12 n12, t13 n13, t14 n14, t15 n15)
#define P16(t1, n1, t2, n2, t3, n3, t4, n4, t5, n5, t6, n6, t7, n7, t8, n8, t9, n9, t10, n10, t11, n11, t12, n12, t13, n13, t14, n14, t15, n15, t16, n16) \
					(t1 n1, t2 n2, t3 n3, t4 n4, t5 n5, t6 n6, t7 n7, t8 n8, t9 n9, t10 n10, t11 n11, t12 n12, t13 n13, t14 n14, t15 n15, t16 n16)
#define P17(t1, n1, t2, n2, t3, n3, t4, n4, t5, n5, t6, n6, t7, n7, t8, n8, t9, n9, t10, n10, t11, n11, t12, n12, t13, n13, t14, n14, t15, n15, t16, n16, t17, n17) \
					(t1 n1, t2 n2, t3 n3, t4 n4, t5 n5, t6 n6, t7 n7, t8 n8, t9 n9, t10 n10, t11 n11, t12 n12, t13 n13, t14 n14, t15 n15, t16 n16, t17 n17)
#define P18(t1, n1, t2, n2, t3, n3, t4, n4, t5, n5, t6, n6, t7, n7, t8, n8, t9, n9, t10, n10, t11, n11, t12, n12, t13, n13, t14, n14, t15, n15, t16, n16, t17, n17, t18, n18) \
					(t1 n1, t2 n2, t3 n3, t4 n4, t5 n5, t6 n6, t7 n7, t8 n8, t9 n9, t10 n10, t11 n11, t12 n12, t13 n13, t14 n14, t15 n15, t16 n16, t17 n17, t18 n18)
#else

#define A0() ()

#define A1(t1, n1) \
					(n1) \
					t1 n1;
#define A2(t1, n1, t2, n2) \
					(n1, n2) \
					t1 n1; t2 n2;
#define A3(t1, n1, t2, n2, t3, n3) \
					(n1, n2, n3) \
					t1 n1; t2 n2; t3 n3;
#define A4(t1, n1, t2, n2, t3, n3, t4, n4) \
					(n1, n2, n3, n4) \
					t1 n1; t2 n2; t3 n3; t4 n4;
#define A5(t1, n1, t2, n2, t3, n3, t4, n4, t5, n5) \
					(n1, n2, n3, n4, n5) \
					t1 n1; t2 n2; t3 n3; t4 n4; t5 n5;
#define A6(t1, n1, t2, n2, t3, n3, t4, n4, t5, n5, t6, n6) \
					(n1, n2, n3, n4, n5, n6) \
					t1 n1; t2 n2; t3 n3; t4 n4; t5 n5; t6 n6;
#define A7(t1, n1, t2, n2, t3, n3, t4, n4, t5, n5, t6, n6, t7, n7) \
					(n1, n2, n3, n4, n5, n6, n7) \
					t1 n1; t2 n2; t3 n3; t4 n4; t5 n5; t6 n6; t7 n7;
#define A8(t1, n1, t2, n2, t3, n3, t4, n4, t5, n5, t6, n6, t7, n7, t8, n8) \
					(n1, n2, n3, n4, n5, n6, n7, n8) \
					t1 n1; t2 n2; t3 n3; t4 n4; t5 n5; t6 n6; t7 n7; t8 n8;
#define A9(t1, n1, t2, n2, t3, n3, t4, n4, t5, n5, t6, n6, t7, n7, t8, n8, t9, n9) \
					(n1, n2, n3, n4, n5, n6, n7, n8, n9) \
					t1 n1; t2 n2; t3 n3; t4 n4; t5 n5; t6 n6; t7 n7; t8 n8; t9 n9;
#define A10(t1, n1, t2, n2, t3, n3, t4, n4, t5, n5, t6, n6, t7, n7, t8, n8, t9, n9, t10, n10) \
					(n1, n2, n3, n4, n5, n6, n7, n8, n9, n10) \
					t1 n1; t2 n2; t3 n3; t4 n4; t5 n5; t6 n6; t7 n7; t8 n8; t9 n9; t10 n10;
#define A11(t1, n1, t2, n2, t3, n3, t4, n4, t5, n5, t6, n6, t7, n7, t8, n8, t9, n9, t10, n10, t11, n11) \
					(n1, n2, n3, n4, n5, n6, n7, n8, n9, n10, n11) \
					t1 n1; t2 n2; t3 n3; t4 n4; t5 n5; t6 n6; t7 n7; t8 n8; t9 n9; t10 n10; t11 n11;
#define A12(t1, n1, t2, n2, t3, n3, t4, n4, t5, n5, t6, n6, t7, n7, t8, n8, t9, n9, t10, n10, t11, n11, t12, n12) \
					(n1, n2, n3, n4, n5, n6, n7, n8, n9, n10, n11, n12) \
					t1 n1; t2 n2; t3 n3; t4 n4; t5 n5; t6 n6; t7 n7; t8 n8; t9 n9; t10 n10; t11 n11; t12 n12;
#define A13(t1, n1, t2, n2, t3, n3, t4, n4, t5, n5, t6, n6, t7, n7, t8, n8, t9, n9, t10, n10, t11, n11, t12, n12, t13, n13) \
					(n1, n2, n3, n4, n5, n6, n7, n8, n9, n10, n11, n12, n13) \
					t1 n1; t2 n2; t3 n3; t4 n4; t5 n5; t6 n6; t7 n7; t8 n8; t9 n9; t10 n10; t11 n11; t12 n12; t13 n13;
#define A14(t1, n1, t2, n2, t3, n3, t4, n4, t5, n5, t6, n6, t7, n7, t8, n8, t9, n9, t10, n10, t11, n11, t12, n12, t13, n13, t14, n14) \
					(n1, n2, n3, n4, n5, n6, n7, n8, n9, n10, n11, n12, n13, n14) \
					t1 n1; t2 n2; t3 n3; t4 n4; t5 n5; t6 n6; t7 n7; t8 n8; t9 n9; t10 n10; t11 n11; t12 n12; t13 n13; t14 n14;
#define A15(t1, n1, t2, n2, t3, n3, t4, n4, t5, n5, t6, n6, t7, n7, t8, n8, t9, n9, t10, n10, t11, n11, t12, n12, t13, n13, t14, n14, t15, n15) \
					(n1, n2, n3, n4, n5, n6, n7, n8, n9, n10, n11, n12, n13, n14, n15) \
					t1 n1; t2 n2; t3 n3; t4 n4; t5 n5; t6 n6; t7 n7; t8 n8; t9 n9; t10 n10; t11 n11; t12 n12; t13 n13; t14 n14; t15 n15;

#ifndef SUN41_COMPILER
#define A16(t1, n1, t2, n2, t3, n3, t4, n4, t5, n5, t6, n6, t7, n7, t8, n8, t9, n9, t10, n10, t11, n11, t12, n12, t13, n13, t14, n14, t15, n15, t16, n16) \
					(n1, n2, n3, n4, n5, n6, n7, n8, n9, n10, n11, n12, n13, n14, n15, n16) \
					t1 n1; t2 n2; t3 n3; t4 n4; t5 n5; t6 n6; t7 n7; t8 n8; t9 n9; t10 n10; t11 n11; t12 n12; t13 n13; t14 n14; t15 n15; t16 n16;
#define A17(t1, n1, t2, n2, t3, n3, t4, n4, t5, n5, t6, n6, t7, n7, t8, n8, t9, n9, t10, n10, t11, n11, t12, n12, t13, n13, t14, n14, t15, n15, t16, n16, t17, n17) \
					(n1, n2, n3, n4, n5, n6, n7, n8, n9, n10, n11, n12, n13, n14, n15, n16, n17) \
					t1 n1; t2 n2; t3 n3; t4 n4; t5 n5; t6 n6; t7 n7; t8 n8; t9 n9; t10 n10; t11 n11; t12 n12; t13 n13; t14 n14; t15 n15; t16 n16; t17 n17;
#define A18(t1, n1, t2, n2, t3, n3, t4, n4, t5, n5, t6, n6, t7, n7, t8, n8, t9, n9, t10, n10, t11, n11, t12, n12, t13, n13, t14, n14, t15, n15, t16, n16, t17, n17, t18, n18) \
					(n1, n2, n3, n4, n5, n6, n7, n8, n9, n10, n11, n12, n13, n14, n15, n16, n17, n18) \
					t1 n1; t2 n2; t3 n3; t4 n4; t5 n5; t6 n6; t7 n7; t8 n8; t9 n9; t10 n10; t11 n11; t12 n12; t13 n13; t14 n14; t15 n15; t16 n16; t17 n17; t18 n18;

#endif /* SUN41_COMPILER */

#define P0() ()
#define P1(t1, n1) ()
#define P2(t1, n1, t2, n2) ()
#define P3(t1, n1, t2, n2, t3, n3) ()
#define P4(t1, n1, t2, n2, t3, n3, t4, n4) ()
#define P5(t1, n1, t2, n2, t3, n3, t4, n4, t5, n5) ()
#define P6(t1, n1, t2, n2, t3, n3, t4, n4, t5, n5, t6, n6) ()
#define P7(t1, n1, t2, n2, t3, n3, t4, n4, t5, n5, t6, n6, t7, n7) ()
#define P8(t1, n1, t2, n2, t3, n3, t4, n4, t5, n5, t6, n6, t7, n7, t8, n8) ()
#define P9(t1, n1, t2, n2, t3, n3, t4, n4, t5, n5, t6, n6, t7, n7, t8, n8, t9, n9) ()
#define P10(t1, n1, t2, n2, t3, n3, t4, n4, t5, n5, t6, n6, t7, n7, t8, n8, t9, n9, t10, n10) ()
#define P11(t1, n1, t2, n2, t3, n3, t4, n4, t5, n5, t6, n6, t7, n7, t8, n8, t9, n9, t10, n10, t11, n11) ()
#define P12(t1, n1, t2, n2, t3, n3, t4, n4, t5, n5, t6, n6, t7, n7, t8, n8, t9, n9, t10, n10, t11, n11, t12, n12) ()
#define P13(t1, n1, t2, n2, t3, n3, t4, n4, t5, n5, t6, n6, t7, n7, t8, n8, t9, n9, t10, n10, t11, n11, t12, n12, t13, n13) ()
#define P14(t1, n1, t2, n2, t3, n3, t4, n4, t5, n5, t6, n6, t7, n7, t8, n8, t9, n9, t10, n10, t11, n11, t12, n12, t13, n13, t14, n14) ()

#ifndef SUN41_COMPILER
#define P15(t1, n1, t2, n2, t3, n3, t4, n4, t5, n5, t6, n6, t7, n7, t8, n8, t9, n9, t10, n10, t11, n11, t12, n12, t13, n13, t14, n14, t15, n15) ()
#define P16(t1, n1, t2, n2, t3, n3, t4, n4, t5, n5, t6, n6, t7, n7, t8, n8, t9, n9, t10, n10, t11, n11, t12, n12, t13, n13, t14, n14, t15, n15, t16, n16) ()
#define P17(t1, n1, t2, n2, t3, n3, t4, n4, t5, n5, t6, n6, t7, n7, t8, n8, t9, n9, t10, n10, t11, n11, t12, n12, t13, n13, t14, n14, t15, n15, t16, n16, t17, n17) ()
#define P18(t1, n1, t2, n2, t3, n3, t4, n4, t5, n5, t6, n6, t7, n7, t8, n8, t9, n9, t10, n10, t11, n11, t12, n12, t13, n13, t14, n14, t15, n15, t16, n16, t17, n17, t18, n18) ()

#endif /* SUN41_COMPILER */

#endif /* ANSI_PROTOTYPES */

/*********************************************************************
** datatypes, tables and control structures
*********************************************************************/

typedef struct _ASYNCRESULT ASYNCRESULT;
typedef struct _ASYNCRESULT ACB;

typedef VOID (GLOBAL XIPC_CALLBACK_FUNC) C_PROTOARGS((ACB FAR *));

typedef struct _XIPCINFOUSER
{
	XINT Uid;				/* User Id */
	XINT Pid;				/* Process ID of user */
	XTID Tid;				/* Thread ID of user */
	XINT LoginTime; 		/* Time of login to Instance */
	CHAR Name[XIPC_LEN_XIPCNAME + 1];	/* User login name	*/
	CHAR NetLoc[XIPC_LEN_NETLOC + 1];	/* Name of client Node */
}
XIPCINFOUSER;

typedef struct _XIPCINFOLOGIN	/* Login Information	*/
{
	XINT Lid;				/* Login Id 				*/
	CHAR FAR *InstanceName; /* Pointer to instance name */
	XINT UserId;			/* User Id					*/
}
XIPCINFOLOGIN;

typedef struct _XIPCINFOINSTANCE
{

	XINT	BufSize;
	CHAR	ConfigName[XIPC_LEN_NETNAME+1];
	CHAR	Name[XIPC_LEN_NETNAME+1];
	CHAR	HostName[XIPC_LEN_NETNAME+1];
}
XIPCINFOINSTANCE;

typedef struct _XIPC_SYSERR
{
	XINT	Status1;			/* First Error Status Code */
	XINT	Status2;			/* Second Error Status Code .. in some O.S. */
	XINT	Source; 			/* O.S. or Network Error */
	XINT	Class;				/* Xipc's best guess on type of Error */
	CHAR	OperatingSystem[XIPC_LEN_FUNCNAME];  /* Name of operating system */
	XINT	Location;			/* Local or Remote Error */
	CHAR	XipcVerb[XIPC_LEN_FUNCNAME];	/* XIPC Verb reporting the Error */
	CHAR	SystemCall[XIPC_LEN_FUNCNAME];	/* Name of failing System Call */
	CHAR	ErrorMessage[XIPC_LEN_ERRORMSG];  /* Formatted Error Message */
}
XIPC_SYSERR;

typedef struct _XIPC_BUF_SEGMENT
{
	XANY *Segment;			/* Buffer segment address */
	XINT Length;			/* Buffer segment length */
}
XIPC_BUF_SEGMENT;

typedef struct _XIPCINFOVERSION
{
	XINT	Major;				/* The 3 in version 3.1.0ba */
	XINT	Minor;				/* The 1 in version 3.1.0ba */
	XINT	Level;				/* The 0 in version 3.1.0ba */
	CHAR	Modifier[XIPC_LEN_XIPCNAME+1];	/* Optional string such as "(EA)"*/
	CHAR	BuildCommon;		/* The 'b' in version 3.1.0ba   */
	CHAR	BuildSpecific;		/* The 'a' in version 3.1.0ba   */
	CHAR	OperatingSystem[XIPC_LEN_XIPCNAME+1]; /* Operating system info */
	CHAR	StringSummary[XIPC_LEN_VERSION+1];	  /* Version in string format */
}
XIPCINFOVERSION;

#endif /* XIPCPUBD_H */

/*
** END **************************************************** XIPCPUBD.H
*/
