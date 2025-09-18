/*********************************************************** QUEPUBP.H
**											   (QueSys - Queue System)
**
**	Name:	QUEPUBP.H
**	----
**
**	Title:	QueSys Public Header File - Function Prototypes
**	-----
**
**	Description:
**	-----------
**	This file contains the public function prototypes for the
**	Queue System.
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
**
**
**	Copyright (C) Momentum Software Corporation, 1989
**
*********************************************************************/

#ifndef QUEPUBP_H
#define QUEPUBP_H QUEPUBP_H

/*********************************************************************
** functions
*********************************************************************/

#ifdef	C_PLUSPLUS
extern	"C" {
#endif

XINT		GLOBAL XAPI	QueAbortAsync C_PROTOARGS((XINT));
XINT		GLOBAL XAPI	QueAccess C_PROTOARGS((CHAR FAR *));
XINT		GLOBAL XAPI	QueBrowse C_PROTOARGS((MSGHDR FAR *, XINT));
#ifdef QUE_BURST_RECEIVE
XINT		GLOBAL XAPI  QueBurstReceive
					C_PROTOARGS((XANY *MsgBuf, XINT *MsgLength, XINT *Priority,
					XINT *RetQid));
XINT		GLOBAL XAPI  QueBurstReceiveCreditSet
					C_PROTOARGS((XINT IncrementCreditMessages,
					XINT IncrementCreditBytes));
XINT		GLOBAL XAPI  QueBurstReceiveNotify C_PROTOARGS((XINT Uid, XINT Notice));
XINT		GLOBAL XAPI  QueBurstReceiveStart
					C_PROTOARGS((XINT QueSelectCode, QIDLIST QidList,
					XINT MaxMsgLength,
					XINT ReadAheadBufSize, XINT FlowControlThresh,
					XINT BlockingOption1, XINT BlockingOption2,
					XINT BlockingOption3, XINT CBOption1,
					XINT CBOption2, XINT CBOption3));
XINT		GLOBAL XAPI  QueBurstReceiveStop C_PROTOARGS((VOID));
XINT		GLOBAL XAPI  QueBurstReceiveSync C_PROTOARGS((XINT));
#endif
XINT		GLOBAL XAPI  QueBurstSend
					C_PROTOARGS((XINT TargetQid, XANY *MsgBuf,
					XINT MsgLength, XINT Priority));
XINT		GLOBAL XAPI  QueBurstSendStart C_PROTOARGS((XINT QueSelectCode, 
					QIDLIST QidList,
					XINT MaxMsgLength,
					XINT ReadAheadBufSize,
					XINT BlockingOption1, 
					XINT BlockingOption2,
					XANY *BlockingOption3, 
					XIPC_CALLBACK_FUNC *BlockingOption4,
					XINT ErrorOption1,
					XINT ErrorOption2, 
					XANY *ErrorOption3, 
					XIPC_CALLBACK_FUNC *ErrorOption4));

XINT		GLOBAL XAPI  QueBurstSendStop C_PROTOARGS((VOID));
XINT		GLOBAL XAPI  QueBurstSendSync C_PROTOARGS((XINT, XINT, XANY *, XIPC_CALLBACK_FUNC *));
XINT		GLOBAL XAPI	QueCopy C_PROTOARGS((MSGHDR FAR *, XINT, XINT, XANY FAR *));
XINT		GLOBAL XAPI	QueCreate C_PROTOARGS((CHAR FAR *, XINT, XINT));
XINT		GLOBAL XAPI	QueDelete C_PROTOARGS((XINT));
XINT		GLOBAL XAPI	QueDestroy C_PROTOARGS((XINT));
XINT		GLOBAL XAPI	QueFreeze C_PROTOARGS((VOID));
XINT		GLOBAL XAPI	QueGet P9(
						MSGHDR FAR *, MsgHdr,
						XINT, QueSelectCode,
						PQIDLIST, QidList,
						XINT FAR *, PriorityPtrArg,
						XINT FAR *, QidPtrArg,
						XINT,  Option1,
						XINT,  Option2,
						XANY  FAR *, Option3,
						XIPC_CALLBACK_FUNC FAR *, Option4);
XINT		GLOBAL XAPI	QueInfoQue C_PROTOARGS((XINT, QUEINFOQUE FAR *));
XINT		GLOBAL XAPI	QueInfoQueEx C_PROTOARGS((XINT, QUEINFOQUEX FAR *));
XINT		GLOBAL XAPI	QueInfoSys C_PROTOARGS((QUEINFOSYS FAR *));
XINT		GLOBAL XAPI	QueInfoSysEx C_PROTOARGS((QUEINFOSYSX FAR *));
XINT		GLOBAL XAPI	QueInfoUser C_PROTOARGS((XINT, QUEINFOUSER FAR *));
XINT		GLOBAL XAPI	QueMsgHdrDup P2(
						MSGHDR *, MsgHdrSrc,
						MSGHDR *, MsgHdrDst);
XINT		GLOBAL XAPI	QuePurge C_PROTOARGS((XINT));
XINT		GLOBAL XAPI	QuePointer C_PROTOARGS((MSGHDR FAR *, XANY FAR *FAR*));
XINT		GLOBAL XAPI	QuePut P9(
						MSGHDR FAR *, MsgHdr,
						XINT, QueSelectCode,
						PQIDLIST, QidList,
						XINT, Priority,
						XINT FAR *, QidPtrArg,
						XINT,	 Option1,
						XINT,  Option2,
						XANY  FAR *, Option3,
						XIPC_CALLBACK_FUNC FAR *, Option4);
XINT		GLOBAL XAPI	QueRead C_PROTOARGS((MSGHDR FAR *, XANY FAR *, XINT));
XINT		GLOBAL XAPI	QueReceive P10(
						XINT, QueSelectCode,
						PQIDLIST, QidList,
						XANY *, MsgBuf,
						XINT, MsgLen,
						XINT  *, PriorityPtrArg,
						XINT *, QidPtrArg,
						XINT,  Option1,
						XINT,  Option2,
						XANY FAR *, Option3,
						XIPC_CALLBACK_FUNC FAR *, Option4);
XINT		GLOBAL XAPI	QueRemove C_PROTOARGS((MSGHDR FAR *));
XINT		GLOBAL XAPI	QueSendReceive P10(
						QUE_SEND_ARGS FAR *, SendArgs,
						XINT,  SendOption1,
						XINT,  SendOption2,
						XANY   FAR *, SendOption3,
						XIPC_CALLBACK_FUNC FAR *, SendOption4,
						QUE_RECEIVE_ARGS FAR *, ReceiveArgs,
						XINT,  ReceiveOption1,
						XINT,  ReceiveOption2,
						XANY FAR *, ReceiveOption3,
						XIPC_CALLBACK_FUNC FAR *, ReceiveOption4);
XINT		GLOBAL XAPI	QueSend
C_PROTOARGS((XINT, PQIDLIST, XANY FAR *, XINT, XINT, XINT FAR *, XINT, XINT, XANY FAR *, XIPC_CALLBACK_FUNC FAR *));
XINT		GLOBAL XAPI	QueSpool C_PROTOARGS((XINT, CHAR FAR *));
XINT		GLOBAL XAPI	QueTrigger C_PROTOARGS((XINT, XINT, XINT, XINT));
XINT		GLOBAL XAPI	QueUnfreeze C_PROTOARGS((VOID));
XINT		GLOBAL XAPI	QueUntrigger C_PROTOARGS((XINT, XINT, XINT, XINT));
XINT		GLOBAL XAPI	QueUnget C_PROTOARGS((MSGHDR FAR *));
XINT		GLOBAL XAPI	QueWrite C_PROTOARGS((MSGHDR *, XANY FAR *, XINT, XINT, XINT, XANY FAR *, XIPC_CALLBACK_FUNC FAR *));

PQIDLIST	FAR XCDECL	QueList C_PROTOARGS((XINT,...));
PQIDLIST	FAR XCDECL	QueListAdd C_PROTOARGS((PQIDLIST, ...));
PQIDLIST	FAR XCDECL	QueListRemove C_PROTOARGS((PQIDLIST, ...));
PQIDLIST	FAR XCDECL	QueListBuild C_PROTOARGS((PQIDLIST, ...));
XINT		GLOBAL XAPI QueListCount C_PROTOARGS((PQIDLIST));

#ifdef	C_PLUSPLUS
}
#endif

#endif /* QUEPUBP_H */

/*
** END ***************************************************** QUEPUBP.H
*/
