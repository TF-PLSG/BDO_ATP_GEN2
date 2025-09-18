/********************************************************** XIPCASYN.H
**			 (XIPC - Extended Interprocess Communications Faclilities)
**
**	Name:		XIPCASYN.H
**	----
**
**	Title:		Async Declarations
**	-----
**
**	Description:
**	-----------
**	This file contains the structure declartions needed
**	for executing Async operations.
**
**	Last Revision:
**	-------------
**	$Revision:   1.0  $
**	$Date:   Oct 14 1999 08:17:48  $
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

#ifndef XIPCASYN_H
#define XIPCASYN_H XIPCASYN_H

/*********************************************************************
**	 Macros
*********************************************************************/

#define XIPC_ASYNC_INPROGRESS	(1)
#define XIPC_ASYNC_COMPLETED	(2)
#define XIPC_ASYNC_DETACHED 	(3)

#define ACB_FIELD(AcbPtr, Function, Field)	((AcbPtr)->Api.Function.Field)

/*********************************************************************
**	'ACB' - ASYNCRESULT Control Block ---
*********************************************************************/

struct _ASYNCRESULT /* Result of Async API call */
{
	XINT  AUid; 			/* Async Uid  "receipt" */
	XINT  AsyncStatus;		/* XIPC_ASYNC_INPROGRESS or XIPC_ASYNC_COMPLETED */
	XINT  UserData1;		/* ------ user defined usage ---- */
	XINT  UserData2;		/* ------ user defined usage ---- */
	XINT  UserData3;		/* ------ user defined usage ---- */

	XINT	  OpCode;		/* Async operation, key to union */

	union
	{
		struct
		{
			XINT	RetSid;
			XINT	RetCode;	/* ... of completed async operation */
		}
		SemWait;

		struct
		{
			XINT	RetSid;
			XINT	RetCode;	/* ... of completed async operation */
		}
		SemAcquire;

		struct
		{
			MSGHDR	MsgHdr; 	/* The resultant MsgHdr 		*/
			CHAR FAR *MsgBuf;
			XINT	RetCode;	/* ... of completed async operation */
		}
		QueWrite;

		struct
		{
			MSGHDR	MsgHdr; 	/* The resultant MsgHdr */
			XINT	RetQid;
			XINT	RetCode;
		}
		QuePut;

		struct
		{
			MSGHDR	MsgHdr; 		/* The resultant MsgHdr */
			XINT	Priority;
			XINT	RetQid;
			XINT	RetCode;
		}
		QueGet;

		struct
		{
			CHAR FAR *MsgBuf;
			XINT	RetQid;
			XINT	RetCode;
		}
		QueSend;

		struct
		{
			CHAR FAR *MsgBuf;
			XINT	MsgLen;
			XINT	Priority;
			XINT	RetQid;
			XINT	RetCode;
		}
		QueReceive;

		struct
		{
			/*
			 * Only used for passing error info in case
			 * of a failed QueBurstSend() operation.
			 */
			XINT	SeqNo;	/* of burst-send msg that failed */
			XINT	TargetQid;
			XINT	Priority;
			XINT	RetQid;
			XINT	RetCode;
		}
		QueBurstSend;

		struct
		{
			/*
			 * Only used for handling an asynchronous
			 * QueBurstSendSync() operation.
			 */
			XINT	SeqNo;	/* of last burst-send msg enqueued */
			XINT	RetCode;
		}
		QueBurstSendSync;

		struct
		{
			XINT	Mid;		/* of target */
			XINT	Offset; 	/* of target */
			XINT	Length; 	/* of target */
			CHAR FAR *Buffer;
			XINT	RetCode;
		}
		MemWrite;

		struct
		{
			XINT	Mid;		/* of target */
			XINT	Offset; 	/* of target */
			XINT	Length; 	/* of target */
			CHAR FAR *Buffer;
			XINT	RetCode;
		}
		MemRead;

		struct
		{
			SECTION RetSec;
			XINT	RetCode;
		}
		MemSecOwn;

		struct
		{
			SECTION RetSec;
			XINT	RetCode;
		}
		MemLock;

		struct
		{
			MOM_MSGID MsgId;
			XINT	  RetCode;
		}
		MomSend;

		struct
		{
			CHAR FAR *MsgBuf;
			XINT	MsgLen;
			MOM_MSGID MsgId;
			XINT	ReplyAppQueue;
			XINT	RetCode;
			XINT	TrackingLevel;
		}
		MomReceive;

		struct
		{
			XINT	RetCode;	/* ... of completed async operation */
		}
		MomEvent;

	}
	Api;

};

#endif /* XIPCASYN_H */

/*
** END **************************************************** XIPCASYN.H
*/
