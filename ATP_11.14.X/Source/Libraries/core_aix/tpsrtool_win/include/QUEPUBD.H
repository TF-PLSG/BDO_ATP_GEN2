/*********************************************************** QUEPUBD.H
**						   (QueSys - Queue System)
**
**	Name:	QUEPUBD.H
**	----
**
**	Title:	QueSys Public Header File - Declarations
**	-----
**
**	Description:
**	-----------
**	This file contains the public declarations for the
**	Queue System.
**
**	Last Revision:
**	-------------
**	$Revision:   1.0  $
**	$Date:   Oct 14 1999 08:17:42  $
**	$Author:   MSALEH  $
**
**	Revision History:
**	----------------
**
**	Rev   Date		By	 Description
**	---   --------	---  -----------
**
**
**	Copyright (C) Momentum Software Corporation, 1989
**
*********************************************************************/

#ifndef QUEPUBD_H
#define QUEPUBD_H QUEPUBD_H

/*********************************************************************
** constants
*********************************************************************/

/* return codes */

#define QUE_SUCCESS 	XIPC_SUCCESS

/* API arguments */

#define QUE_PRIVATE 	XIPC_PRIVATE
#define QUE_SPOOL_OFF	((CHAR FAR *)0)
#define QUE_TRUNCATE(n) (-(n))
#define QUE_NOLIMIT 	(((XINT)0))

/* the following values must always be negative */

#define QUE_EOL 		((XINT)-1)
#define QUE_ALL 		((XINT)-200)
#define QUE_ANY 		((XINT)-201)

/* user blocking status */

#define QUE_USER_NOTWAITING XIPC_USER_NOTWAITING
#define QUE_BLOCKEDWRITE	((XINT)-901)
#define QUE_BLOCKEDPUT		((XINT)-902)
#define QUE_BLOCKEDGET		((XINT)-903)

/* maximums, sizes and limits */

#define QUE_LEN_PATHNAME	XIPC_LEN_PATHNAME
#define QUE_LEN_USERNAME	XIPC_LEN_USERNAME
#define QUE_LEN_XIPCNAME	XIPC_LEN_XIPCNAME
#define QUE_LEN_PARMLINE	XIPC_LEN_PARMLINE
#define QUE_LEN_QIDITEM 	((XINT)4)
#define QUE_MAX_QIDLIST 	((XINT)32)
#define QUE_LEN_QIDLIST 	((QUE_MAX_QIDLIST+(XINT)1)*QUE_LEN_QIDITEM)
#define QUE_LEN_QIDLIST_S	(((XINT)9)*QUE_LEN_QIDITEM)
#define QUE_LEN_INFOLIST	((XINT)10)

#define MSGHDR_SIZE 	sizeof(MSGHDR)
#define MSGHDR_DATASIZE 	(16)

/*********************************************************************
** queue and message selection codes and macros
*********************************************************************/

#define   QUE_Q_HIGHVALUE	((XINT)-1000)
#define   QUE_Q_HP		((XINT)-1000)
#define   QUE_Q_LP		((XINT)-1001)
#define   QUE_Q_EA		((XINT)-1002)
#define   QUE_Q_LA		((XINT)-1003)
#define   QUE_Q_HPQ 	((XINT)-1004)
#define   QUE_Q_LPQ 	((XINT)-1005)
#define   QUE_Q_EAQ 	((XINT)-1006)
#define   QUE_Q_LAQ 	((XINT)-1007)
#define   QUE_Q_SHQ 	((XINT)-1008)
#define   QUE_Q_LNQ 	((XINT)-1009)
#define   QUE_Q_ANY 	((XINT)-1010)
#define   QUE_Q_HAVESOURCEHDR	((XINT)-5000)		/* Internal usage */
#define   QUE_Q_LOWVALUE	((XINT)-1999)

#define   QUE_M_HIGHVALUE	((XINT)-2000)
#define   QUE_M_HP_CODE 	(QUE_Q_HP-(XINT)1000)
#define   QUE_M_LP_CODE 	(QUE_Q_LP-(XINT)1000)
#define   QUE_M_EA_CODE 	(QUE_Q_EA-(XINT)1000)
#define   QUE_M_LA_CODE 	(QUE_Q_LA-(XINT)1000)
#define   QUE_M_PREQ_CODE	((XINT)-2004)
#define   QUE_M_PRNE_CODE	((XINT)-2005)
#define   QUE_M_PRGT_CODE	((XINT)-2006)
#define   QUE_M_PRGE_CODE	((XINT)-2007)
#define   QUE_M_PRLT_CODE	((XINT)-2008)
#define   QUE_M_PRLE_CODE	((XINT)-2009)
#define   QUE_M_PRRNG_CODE	((XINT)-2010)
#define   QUE_M_SEQEQ_CODE	((XINT)-2015)
#define   QUE_M_SEQGT_CODE	((XINT)-2016)
#define   QUE_M_SEQGE_CODE	((XINT)-2017)
#define   QUE_M_SEQLT_CODE	((XINT)-2018)
#define   QUE_M_SEQLE_CODE	((XINT)-2019)
#define   QUE_M_LOWVALUE	((XINT)-2999)

#define   QUE_M_HP(q)		(XINT)(q), QUE_M_HP_CODE, ((XINT)0), ((XINT)0)
#define   QUE_M_LP(q)		(XINT)(q), QUE_M_LP_CODE, ((XINT)0), ((XINT)0)
#define   QUE_M_EA(q)		(XINT)(q), QUE_M_EA_CODE, ((XINT)0), ((XINT)0)
#define   QUE_M_LA(q)		(XINT)(q), QUE_M_LA_CODE, ((XINT)0), ((XINT)0)
#define   QUE_M_PREQ(q,n)	(XINT)(q), QUE_M_PREQ_CODE, (XINT)(n), ((XINT)0)
#define   QUE_M_PRNE(q,n)	(XINT)(q), QUE_M_PRNE_CODE, (XINT)(n), ((XINT)0)
#define   QUE_M_PRGT(q,n)	(XINT)(q), QUE_M_PRGT_CODE, (XINT)(n), ((XINT)0)
#define   QUE_M_PRGE(q,n)	(XINT)(q), QUE_M_PRGE_CODE, (XINT)(n), ((XINT)0)
#define   QUE_M_PRLT(q,n)	(XINT)(q), QUE_M_PRLT_CODE, (XINT)(n), ((XINT)0)
#define   QUE_M_PRLE(q,n)	(XINT)(q), QUE_M_PRLE_CODE, (XINT)(n), ((XINT)0)
#define   QUE_M_PRRNG(q,n,m)	(XINT)(q), QUE_M_PRRNG_CODE, (XINT)(n), (XINT)(m)

#define   QUE_M_SEQEQ(q,n)	(XINT)(q), QUE_M_SEQEQ_CODE, (XINT)(n), ((XINT)0)
#define   QUE_M_SEQGT(q,n)	(XINT)(q), QUE_M_SEQGT_CODE, (XINT)(n), ((XINT)0)
#define   QUE_M_SEQGE(q,n)	(XINT)(q), QUE_M_SEQGE_CODE, (XINT)(n), ((XINT)0)
#define   QUE_M_SEQLT(q,n)	(XINT)(q), QUE_M_SEQLT_CODE, (XINT)(n), ((XINT)0)
#define   QUE_M_SEQLE(q,n)	(XINT)(q), QUE_M_SEQLE_CODE, (XINT)(n), ((XINT)0)


/*********************************************************************
** trigger codes and macros
*********************************************************************/

#define QUE_T_BYTES_HIGH_CODE	((XINT)700)
#define QUE_T_MSGS_HIGH_CODE	((XINT)701)
#define QUE_T_PUT_CODE			((XINT)702)
#define QUE_T_PUT_PREQ_CODE 	((XINT)703)
#define QUE_T_PUT_PRLT_CODE 	((XINT)704)
#define QUE_T_PUT_PRGT_CODE 	((XINT)705)
#define QUE_T_USER_PUT_CODE 	((XINT)706)
#define QUE_T_POOL_HIGH_CODE	((XINT)707)
#define QUE_T_HEADER_HIGH_CODE	((XINT)708)
#define QUE_T_BYTES_LOW_CODE	((XINT)709)
#define QUE_T_MSGS_LOW_CODE 	((XINT)710)
#define QUE_T_GET_CODE			((XINT)711)
#define QUE_T_GET_PREQ_CODE 	((XINT)712)
#define QUE_T_GET_PRLT_CODE 	((XINT)713)
#define QUE_T_GET_PRGT_CODE 	((XINT)714)
#define QUE_T_USER_GET_CODE 	((XINT)715)
#define QUE_T_POOL_LOW_CODE 	((XINT)716)
#define QUE_T_HEADER_LOW_CODE	((XINT)717)

#define QUE_T_BYTES_HIGH(Qid,N) 	(Qid), QUE_T_BYTES_HIGH_CODE, (N)
#define QUE_T_BYTES_LOW(Qid,N)		(Qid), QUE_T_BYTES_LOW_CODE, (N)
#define QUE_T_MSGS_HIGH(Qid,N)		(Qid), QUE_T_MSGS_HIGH_CODE, (N)
#define QUE_T_MSGS_LOW(Qid,N)		(Qid), QUE_T_MSGS_LOW_CODE, (N)
#define QUE_T_PUT(Qid)				(Qid), QUE_T_PUT_CODE, ((XINT)0)
#define QUE_T_GET(Qid)				(Qid), QUE_T_GET_CODE, ((XINT)0)
#define QUE_T_PUT_PREQ(Qid,Prio)	(Qid), QUE_T_PUT_PREQ_CODE, (Prio)
#define QUE_T_GET_PREQ(Qid,Prio)	(Qid), QUE_T_GET_PREQ_CODE, (Prio)
#define QUE_T_PUT_PRGT(Qid,Prio)	(Qid), QUE_T_PUT_PRGT_CODE, (Prio)
#define QUE_T_GET_PRGT(Qid,Prio)	(Qid), QUE_T_GET_PRGT_CODE, (Prio)
#define QUE_T_PUT_PRLT(Qid,Prio)	(Qid), QUE_T_PUT_PRLT_CODE, (Prio)
#define QUE_T_GET_PRLT(Qid,Prio)	(Qid), QUE_T_GET_PRLT_CODE, (Prio)
#define QUE_T_USER_PUT(Qid,Uid) 	(Qid), QUE_T_USER_PUT_CODE, (Uid)
#define QUE_T_USER_GET(Qid,Uid) 	(Qid), QUE_T_USER_GET_CODE, (Uid)
#define QUE_T_POOL_HIGH(N)			((XINT)0), QUE_T_POOL_HIGH_CODE, (N)
#define QUE_T_POOL_LOW(N)			((XINT)0), QUE_T_POOL_LOW_CODE, (N)
#define QUE_T_HEADER_HIGH(N)		((XINT)0), QUE_T_HEADER_HIGH_CODE, (N)
#define QUE_T_HEADER_LOW(N) 		((XINT)0), QUE_T_HEADER_LOW_CODE, (N)

/*********************************************************************
** miscellaneous option flags
*********************************************************************/

#define   QUE_FREEZE			XIPC_FREEZE
#define   QUE_NOREMOVE			XIPC_NOREMOVE
#define   QUE_FINISHWRITE		XIPC_FINISHWRITE
#define   QUE_RETURN			XIPC_RETURN
#define   QUE_ALLOC 			XIPC_ALLOC
#define   QUE_NOOPTIONS 		XIPC_NOOPTIONS

#define   QUE_RETSEQ			XIPC_RETSEQ
#define   QUE_RETPRIO			XIPC_RETPRIO

/* blocking options */

#define   QUE_BLOCKOPT_WAIT 	XIPC_BLOCKOPT_WAIT
#define   QUE_BLOCKOPT_NOWAIT	XIPC_BLOCKOPT_NOWAIT
#define   QUE_BLOCKOPT_TIMEOUT	XIPC_BLOCKOPT_TIMEOUT
#define   QUE_BLOCKOPT_SYNC 	XIPC_BLOCKOPT_SYNC

#define   QUE_BLOCKOPT_CALLBACK XIPC_BLOCKOPT_CALLBACK
#define   QUE_BLOCKOPT_POST 	XIPC_BLOCKOPT_POST
#define   QUE_BLOCKOPT_IGNORE	XIPC_BLOCKOPT_IGNORE
#define   QUE_BLOCKOPT_ASYNC	XIPC_BLOCKOPT_ASYNC

#define   QUE_BLOCKOPT_REPLACE	((XINT)0x00000200)

#define   QUE_WAIT				XIPC_WAIT
#define   QUE_NOWAIT			XIPC_NOWAIT
#define   QUE_TIMEOUT(x)		XIPC_TIMEOUT(x)
#define   QUE_CALLBACK(f,r) 	XIPC_CALLBACK(f,r)
#define   QUE_POST(s,r) 		XIPC_POST(s,r)
#define   QUE_IGNORE(r) 		XIPC_IGNORE(r)

#define   QUE_REPLACE			QUE_BLOCKOPT_REPLACE, QUE_M_EA_CODE, NULL, XIPC_NULL_CALLBACK
#define   QUE_REPLACE_EA		QUE_BLOCKOPT_REPLACE, QUE_M_EA_CODE, NULL, XIPC_NULL_CALLBACK
#define   QUE_REPLACE_LA		QUE_BLOCKOPT_REPLACE, QUE_M_LA_CODE, NULL, XIPC_NULL_CALLBACK
#define   QUE_REPLACE_HP		QUE_BLOCKOPT_REPLACE, QUE_M_HP_CODE, NULL, XIPC_NULL_CALLBACK
#define   QUE_REPLACE_LP		QUE_BLOCKOPT_REPLACE, QUE_M_LP_CODE, NULL, XIPC_NULL_CALLBACK

#define   QUE_REPLICATE 		XIPC_REPLICATE

/* MsgHdr.HdrStatus values */

#define   QUE_REMOVED		((XINT)1)	/* Header was taken off the queue */
#define   QUE_NOTREMOVED	((XINT)2)	/* Header is a dupliacte of a header 
											that is on the queue */
#define   QUE_REPLICATED	((XINT)4)
#define	  QUE_DUPLICATED	((XINT)8)
#define   QUE_HDRINVALID	((XINT)16)	/* Header text pointer invalid */

/* QueBrowse Direction macros */

#define   QUE_PRIO_NEXT 	((XINT)1)
#define   QUE_PRIO_PREV 	((XINT)2)
#define   QUE_TIME_NEXT 	((XINT)3)
#define   QUE_TIME_PREV 	((XINT)4)

/* Freeze options */

#define   QUE_MASKTRAPS 		XIPC_MASKTRAPS
#define   QUE_NOMASKTRAPS		XIPC_NOMASKTRAPS

/* Burst options */

#define   QUE_BURST_NOLIMIT 				0L
#define   QUE_BURST_RESTART 				1L	/* must NOT be 0 */
#define   QUE_BURST_NOFLOWCONTROL			0L	/* must be 0 */
#define   QUE_BURST_FLOWCONTROLWAIT(n)		(n)
#define   QUE_BURST_FLOWCONTROLNOWAIT(n)	(-(n))
#define   QUE_BURST_DEFAULT_READAHEADSIZE 0L

/* Null Values */

#define   QUE_NULL_CALLBACK 		0L, 0L, 0L
#define   QUE_NULL_QID				-1L
#define   QUE_NULL_QUESELECTCODE	0L
#define   QUE_NULL_QIDLIST			((PQIDLIST)NULL)


/* Possible "Notice" values  to QueBurstReceiveNotify() */

#define QUE_BURST_RECEIVE_NOTICE_SYNC	1L
#define QUE_BURST_RECEIVE_NOTICE_STOP	2L

/* Info Options */

#define QUE_INFO_FIRST			XIPC_INFO_FIRST
#define QUE_INFO_NEXT(Id)		XIPC_INFO_NEXT(Id)

/* Non-Contiguous Buffer Option */

#define QUE_NONCONTIG(ListAddr) XIPC_NONCONTIG(ListAddr)

/*********************************************************************
** data types
*********************************************************************/

typedef XINT QIDLIST[QUE_LEN_QIDLIST];
typedef XINT QIDLIST_S[QUE_LEN_QIDLIST_S];
typedef XINT FAR *PQIDLIST;

/*
 * The MSGHDR structure is used for manipulating
 * QueSys message headers. Each active message
 * in an instance has a message header associated
 * with it.
 */
typedef struct _MSGHDR
{
	XINT GetQid;		/* Last Qid msg was on */
	XINT HdrStatus; 	/* Rmvd or Not Rmvd, etc */
	XINT Priority;		/* Message's priority */
	XINT SeqNum;		/* Message sequence number (per-queue) */
	XINT TimeVal;		/* Global time stamp */
	XINT Size;			/* Numb. of bytes in msg */
	XINT TextOffset;	/* Offset to message text */
	XINT Uid;			/* User Id of message originator */
	CHAR Data[MSGHDR_DATASIZE];
}
MSGHDR;

/*
 *	Version 2.8 Message Header
 */
typedef struct _MSGHDR28
{
	XINT GetQid;		/* Last Qid msg was on */
	XINT HdrStatus; 	/* Rmvd or Not Rmvd, etc */
	XINT Priority;		/* Message's priority */
	XINT TimeVal;		/* Global time stamp */
	XINT Size;			/* Numb. of bytes in msg */
	XINT TextOffset;	/* Offset to message text */
}
MSGHDR28;

/*
 *	QueSend Arguments for QueSendReceive API
 */
typedef struct _QUE_SEND_ARGS
{
	XINT QueSelectCode;
	PQIDLIST QidList;
	XANY *MsgBuf;
	XINT MsgLength;
	XINT Priority;
	XINT *QidPtr;
}
QUE_SEND_ARGS;

#define QUE_SEND_ARGS_SET(ArgsPtr, Qsc, Ql, Mb, Ml, Pr, Qp) \
	{	\
		(ArgsPtr)->QueSelectCode = (Qsc);	\
		(ArgsPtr)->QidList = (Ql);	\
		(ArgsPtr)->MsgBuf = (Mb);		\
		(ArgsPtr)->MsgLength = (Ml);	\
		(ArgsPtr)->Priority = (Pr); \
		(ArgsPtr)->QidPtr = (Qp);	  \
	}

/*
 *	QueReceive Arguments for QueSendReceive API
 */
typedef struct _QUE_RECEIVE_ARGS
{
	XINT QueSelectCode;
	PQIDLIST QidList;
	XANY *MsgBuf;
	XINT MsgLength;
	XINT *PrioPtr;
	XINT *QidPtr;
}
QUE_RECEIVE_ARGS;

#define QUE_RECEIVE_ARGS_SET(ArgsPtr, Qsc, Ql, Mb, Ml, Pr, Qp) \
	{	\
		(ArgsPtr)->QueSelectCode = (Qsc);	\
		(ArgsPtr)->QidList = (Ql);	\
		(ArgsPtr)->MsgBuf = (Mb);		\
		(ArgsPtr)->MsgLength = (Ml);	\
		(ArgsPtr)->PrioPtr = (Pr);	\
		(ArgsPtr)->QidPtr = (Qp);	  \
	}

typedef struct _QUE_QUEWLISTITEM
{
	XINT  OpCode;			/* PUT or GET */
	union
	{
		struct
		{
			XINT Uid;		/* User blocked */
			XINT MsgSize;	/* Putting Msg */
			XINT MsgPrio;	/* Msg Priority */
		}
		Put;
		struct
		{
			XINT Uid;		/* User blocked */
			XINT MsgSelCode;/* Getting Msg */
			XINT Parm1;
			XINT Parm2;
		}
		Get;
	}
	u;
}
QUE_QUEWLISTITEM;

/*
 * The QUEINFOQUE structure is used for retreiving status
 * information about a particular QueSys message queue.
 * QueInfoQue() fills the structure with the data
 * about the Qid it is passed.
 */
typedef struct _QUEINFOQUE
{
	XINT Qid;
	XINT CreateTime;		/* Time queue was created */
	XINT CreateUid; 		/* The Uid who created it */
	XINT LastUid;			/* Last Uid to use queue */
	LBITS QueType;			/*	- Not Used - */
	XINT LimitMessages; 	/* Max message capacity */
	XINT LimitBytes;		/* Max byte capacity */
	XINT CountMessages; 	/* Current number of msgs */
	XINT CountBytes;		/* Current number of bytes*/
	XINT CountIn;			/* Number msgs entered que*/
	XINT CountOut;			/* Number msgs exited que */
	XINT LastUidGet;		/* Last Uid to put a msg */
	XINT LastUidPut;		/* Last Uid to get a msg */
	XINT SpoolFlag; 		/* Spooling: ON or OFF */
	XINT SpoolMessages; 	/* Number msgs spooled */
	XINT SpoolBytes;		/* Number bytes spooled */
	XINT WListTotalLength;
	XINT WListOffset;
	XINT WListLength;
	QUE_QUEWLISTITEM WList[QUE_LEN_INFOLIST];
	CHAR SpoolFileName[QUE_LEN_PATHNAME+1];
	CHAR Name[QUE_LEN_XIPCNAME + 1];	/* Queue name */
}
QUEINFOQUE;

typedef struct _QUEINFOQUEX
{
	QUEINFOQUE	QInfoQue;
	XINT CountMessagesHWM;
	XINT CountBytesHWM;
}
QUEINFOQUEX;

typedef struct _QUE_USERWLISTITEM
{
	XINT  OpCode;				/* PUT, GET or WRITE */
	union
	{
		struct
		{
			XINT Qid;			/* Que Blocked */
			XINT MsgSize;		/* Putting Msg */
			XINT MsgPrio;		/* Msg Priority */
		}
		Put;
		struct
		{
			XINT Qid;			/* Que Blocked */
			XINT MsgSelCode;	/* Getting Msg */
			XINT Parm1;
			XINT Parm2;
		}
		Get;
		struct
		{
			XINT MsgSize;		/* Write Blked */

		}
		Write;
	}
	u;
}
QUE_USERWLISTITEM;

/*
 * The QUEINFOUSER structure is used for retreiving status
 * information about a particular QueSys user.
 * QueInfoUser() fills the structure with the data
 * about the Uid it is passed.
 */
typedef struct _QUEINFOUSER
{
	XINT Uid;
	XINT Pid;			/* Process ID of user */
	XTID Tid;			/* Thread ID of user */
	XINT LoginTime; 	/* Time of login to QueSys */
	XINT TimeOut;		/* Remaining timeout secs */
	XINT WaitType;		/* Type of operation blkd */
	XINT CountPut;		/* Number of msgs put */
	XINT CountGet;		/* Number of msgs gotten */
	XINT LastQidPut;	/* Last Qid msg was put on */
	XINT LastQidGet;	/* Last Qid msg taken from */
	XINT WListTotalLength;
	XINT WListOffset;
	XINT WListLength;
	QUE_USERWLISTITEM WList[QUE_LEN_INFOLIST];
	CHAR Name[QUE_LEN_XIPCNAME + 1];	/* User login name */
	CHAR NetLoc[XIPC_LEN_NETLOC + 1];	/* Name of client Node */
}
QUEINFOUSER;

typedef struct _QUE_SYSWLISTITEM
{

	XINT Uid;				/* User Blked */
	XINT MsgSize;			/* Write size */
}
QUE_SYSWLISTITEM;

/*
 * The QUEINFOSYS structure is used for retreiving status
 * information about the QueSys instance.
 * QueInfoSys() fills the structure with the data
 * about the instance.
 */
typedef struct _QUEINFOSYS	/* system information */
{
	XINT MaxUsers;			/* Max configured users */
	XINT CurUsers;			/* Number of current users */
	XINT MaxQueues; 		/* Max configured queues */
	XINT CurQueues; 		/* Number of current que's */
	XINT MaxNodes;			/* Max configured nodes */
	XINT FreeNCnt;			/* Current available nodes*/
	XINT MaxHeaders;		/* Max configured headers */
	XINT FreeHCnt;			/* Current available hdrs */
	XINT SplTickSizeBytes;	/* Configured spool tick value */
	XINT MsgPoolSizeBytes;	/* Configured text pool size */
	XINT MsgTickSize;		/* Configured text tick size */
	XINT MsgPoolTotalAvail; /* Free text pool space */
	XINT MsgPoolLargestBlk; /* Largest contig block */
	XINT MsgPoolMaxPosBlks; /* Max possible tick blocks */
	XINT MsgPoolTotalBlks;	/* Number allocated blocks */
	XINT WListTotalLength;
	XINT WListOffset;
	XINT WListLength;
	QUE_SYSWLISTITEM WList[QUE_LEN_INFOLIST];
	CHAR Name[QUE_LEN_PATHNAME + 1];	/* InstanceFileName */
}
QUEINFOSYS;

typedef struct _QUEINFOSYSX
{
	QUEINFOSYS InfoSys;
	XINT HeaderHWM;
	XINT NodeHWM;
	XINT QueueHWM;
	XINT UserHWM;
	XINT AsyncUserHWM;
	XINT MsgPoolHWM;
}
QUEINFOSYSX;

typedef XIPC_BUF_SEGMENT QUE_BUF_SEGMENT;

#endif /* QUEPUBD_H */

/*
** END ***************************************************** QUEPUBD.H
*/
