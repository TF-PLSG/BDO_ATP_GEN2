/*********************************************************** MOMPUBD.H
**						 (MomSys - Message Oriented Middleware System)
**
**	Name:	MOMPUBD.H
**	----
**
**	Title:	MomSys Public Header File
**	-----
**
**	Description:
**	-----------
**	This file contains the public declarations for MomSys
**
**	Last Revision:
**	-------------
**	$Revision:   1.0  $
**	$Date:   Oct 14 1999 08:17:40  $
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

#ifndef MOMPUBD_H
#define MOMPUBD_H MOMPUBD_H

/*********************************************************************
** determine if actual declarations or 'extern' defintions
*********************************************************************/

#ifdef	EXTERN
#undef	EXTERN
#endif

#ifdef	PIF_DATA_ALLOC
#define EXTERN	GLOBALDEF
#else
#define EXTERN	GLOBALREF DATAIMP
#endif

/**********************************************************************
**	Lengths
***********************************************************************/

#define MOM_LEN_FILTER		(255)
#define MOM_LEN_FILTER_PAT	(16)
#define MOM_LEN_KEY_STR 	(16)
#define MOM_LEN_MESSAGETAG	XIPC_LEN_XIPCNAME
#define MOM_LEN_SCHEDULE	(127)
#define MOM_LEN_SCANKEYS	(127)

/*
 * Application Queue Types
 * =======================
 */
#define MOM_APPQUEUE_LOCAL	(1)
#define MOM_APPQUEUE_REMOTE (2)

/*
 *	Message Status and TrackingLevels
 *	=================================
 */
#define MOM_STATUS_HELD 		(1)
#define MOM_STATUS_SHIPPED		(2)
#define MOM_STATUS_DELIVERED	(3)

#define MOM_OPCODE_SEND 		(1)
#define MOM_OPCODE_RECEIVE		(2)
#define MOM_OPCODE_EVENT		(3)

#define MOM_LINKSTATUS_UP		(1)
#define MOM_LINKSTATUS_DOWN 	(2)

#define MOM_APPQUEUE_VERIFIED	 (1)
#define MOM_APPQUEUE_NOTVERIFIED (0)

#define MOM_TRACK_HELD			MOM_STATUS_HELD
#define MOM_TRACK_SHIPPED		MOM_STATUS_SHIPPED
#define MOM_TRACK_DELIVERED 	MOM_STATUS_DELIVERED

#define MOM_PRIVATE 			XIPC_PRIVATE
#define MOM_TRUNCATE(n) 		(((n) > 0) ? (-(n)) : (0x7FFFFFFF))
#define MOM_NOLIMIT 			(((XINT)0))

#define MOM_MSG_NOSYNC			((XINT)1)

#define MOM_VSOPT_OPTARGS		((XINT)0x80000000)
#define MOM_VSOPT_REPLYTO		((XINT)0x80000001)
#define MOM_VSOPT_TAG			((XINT)0x80000002)
#define MOM_VSOPT_EXPIRE		((XINT)0x80000004)
#define MOM_VSOPT_TRANSLATE 	((XINT)0x80000008)

#define MOM_REPLYTO(MsgId)		MOM_VSOPT_REPLYTO, (MsgId)
#define MOM_TAG(MsgTag) 		MOM_VSOPT_TAG, (MsgTag)
#define MOM_EXPIRE(TimeLimit)	MOM_VSOPT_EXPIRE, (TimeLimit)
#define MOM_TRANSLATE(DataType) MOM_VSOPT_TRANSLATE, (DataType)

/*
 *	Event Codes
 *	===========
 */

#define MOM_EV_TAGGEDARGS_CODE			((XINT)0x80000000)

#define MOM_EV_MSG_STATUS_CODE			((XINT)0x80000001)
#define MOM_EV_APPQUE_MSGS_HI_CODE		((XINT)0x80000002)
#define MOM_EV_APPQUE_MSGS_LOW_CODE 	((XINT)0x80000004)
#define MOM_EV_APPQUE_BYTES_HI_CODE 	((XINT)0x80000008)
#define MOM_EV_APPQUE_BYTES_LOW_CODE	((XINT)0x80000010)
#define MOM_EV_MRI_SIZE_HI_CODE 		((XINT)0x80000020)
#define MOM_EV_MRO_SIZE_HI_CODE 		((XINT)0x80000040)
#define MOM_EV_MR_SIZE_HI_CODE			((XINT)0x80000080)
#define MOM_EV_LOGFILE_SIZE_HI_CODE 	((XINT)0x80000100)

#define MOM_EV_MSG_STATUS(MsgId, Status) \
								MOM_EV_MSG_STATUS_CODE, (MsgId), (Status)
#define MOM_EV_APPQUE_MSGS_HI(AQid, NumMsgs) \
								MOM_EV_MSGS_HI_CODE, (AQid), (NumMsgs)
#define MOM_EV_APPQUE_MSGS_LOW(AQid, NumMsgs) \
								MOM_EV_MSGS_LOW_CODE, (AQid), (NumMsgs)
#define MOM_EV_APPQUE_BYTES_HI(AQid, NumBytes) \
								MOM_EV_BYTES_HI_CODE, (AQid), (NumBytes)
#define MOM_EV_APPQUE_BYTES_LOW(AQid, NumBytes) \
								MOM_EV_BYTES_LOW_CODE, (AQid), (NumBytes)
#define MOM_EV_MRI_SIZE_HI(NumBytes) \
								MOM_EV_MRI_HI_CODE, (NumBytes)
#define MOM_EV_MRO_SIZE_HI(NumBytes) \
								MOM_EV_MRO_HI_CODE, (NumBytes)
#define MOM_EV_MR_SIZE_HI(NumBytes) \
								MOM_EV_MR_HI_CODE, (NumBytes)
#define MOM_EV_LOGFILE_SIZE_HI(NumBytes) \
								MOM_EV_LOGFILE_HI_CODE, (NumBytes)

#define MOM_NOREMOVE			XIPC_NOREMOVE
#define MOM_RETURN				XIPC_RETURN
#define MOM_NOOPTIONS			XIPC_NOOPTIONS
#define MOM_FASTPATH			XIPC_FASTPATH

#define MOM_EV_DETACHED 		XIPC_DETACH

/*
 *	Blocking Options
 *	================
 */
#define MOM_BLOCKOPT_WAIT		XIPC_BLOCKOPT_WAIT
#define MOM_BLOCKOPT_NOWAIT 	XIPC_BLOCKOPT_NOWAIT
#define MOM_BLOCKOPT_TIMEOUT	XIPC_BLOCKOPT_TIMEOUT
#define MOM_BLOCKOPT_SYNC		XIPC_BLOCKOPT_SYNC

#define MOM_BLOCKOPT_CALLBACK	XIPC_BLOCKOPT_CALLBACK
#define MOM_BLOCKOPT_POST		XIPC_BLOCKOPT_POST
#define MOM_BLOCKOPT_IGNORE 	XIPC_BLOCKOPT_IGNORE
#define MOM_BLOCKOPT_ASYNC		XIPC_BLOCKOPT_ASYNC
#define MOM_BLOCKOPT_SPAWN		XIPC_BLOCKOPT_SPAWN

#define MOM_WAIT				XIPC_WAIT
#define MOM_NOWAIT				XIPC_NOWAIT
#define MOM_TIMEOUT(x)			XIPC_TIMEOUT(x)
#define MOM_CALLBACK(f, r)		XIPC_CALLBACK(f, r)
#define MOM_POST(s,r)			XIPC_POST(s,r)
#define MOM_IGNORE(r)			XIPC_IGNORE(r)
#define MOM_SPAWN(c, r) 		XIPC_SPAWN(c, r)

#define MOM_INFO_FIRST			XIPC_INFO_FIRST
#define MOM_INFO_NEXT(Id)		XIPC_INFO_NEXT(Id)

#define MOM_REPLY_NONE			(((XINT)-1))

#define XDISK					1
#define XDISK_REGISTER			2
#define XDISK_REGISTER_UPDATE	3
#define XDISK_PRIORITY			4
#define XDISK_PRIORITY_REGISTER	5
#define XMEMORY					6
#define XMEMORY_REGISTER		7

/*
 *	Application Queue Attribute Macros
 *	==================================
 */
#define MOM_APPQUEUE_DISK	(MOM_ATTRBLOCK_APPQUEUE *) \
						(MomAttrFill(XDISK))
#define MOM_APPQUEUE_DISK_REGISTER (MOM_ATTRBLOCK_APPQUEUE *) \
						(MomAttrFill(XDISK_REGISTER))
#define MOM_APPQUEUE_DISK_REGISTER_UPDATE (MOM_ATTRBLOCK_APPQUEUE *) \
						(MomAttrFill(XDISK_REGISTER_UPDATE))
#define MOM_APPQUEUE_DISK_PRIORITY (MOM_ATTRBLOCK_APPQUEUE *) \
						(MomAttrFill(XDISK_PRIORITY))
#define MOM_APPQUEUE_DISK_PRIORITY_REGISTER (MOM_ATTRBLOCK_APPQUEUE *) \
						(MomAttrFill(XDISK_PRIORITY_REGISTER))
#define MOM_APPQUEUE_MEMORY (MOM_ATTRBLOCK_APPQUEUE *) \
						(MomAttrFill(XMEMORY))
#define MOM_APPQUEUE_MEMORY_REGISTER (MOM_ATTRBLOCK_APPQUEUE *) \
						(MomAttrFill(XMEMORY_REGISTER))

#define MOM_NOVERIFY(AppQueueName)	PifSetNoVerify(AppQueueName)

/*
 *	Message Specifier Macros
 *	========================
 */
#define MOM_MESSAGE_FIRST \
					MOM_SEQUENCE_NATURAL, MOM_SELECT_FIRST

#define MOM_MESSAGE_LAST \
					MOM_SEQUENCE_NATURAL, MOM_SELECT_LAST

#define MOM_MESSAGE_NEXT(MsgId) \
					MOM_SEQUENCE_NATURAL, MOM_SELECT_NEXT(MsgId)

#define MOM_MESSAGE_PREV(MsgId) \
					MOM_SEQUENCE_NATURAL, MOM_SELECT_PREV(MsgId)

#define MOM_MESSAGE_DIRECT(MsgId) \
					MOM_SEQUENCE_ANY, MOM_SELECT_DIRECT(MsgId)

#define MOM_MESSAGE_DIRECT_RMT(RmtNode, RmtInstance, MsgId) \
					MOM_SEQUENCE_ANY, \
					MOM_SELECT_DIRECT_RMT((RmtNode), (RmtInstance), (MsgId))

#define MOM_MESSAGE_REPLYTO(MsgId)	\
					MOM_SEQUENCE_ANY, MOM_SELECT_REPLYTO(MsgId)

#define MOM_MESSAGE_KEYHIGH \
					MOM_SEQUENCE_KEY, MOM_SELECT_FIRST

#define MOM_MESSAGE_KEYLOW	\
					MOM_SEQUENCE_KEY, MOM_SELECT_LAST

#define MOM_MESSAGE_KEYEQINT(IntValue)	\
					MOM_SEQUENCE_KEY, MOM_SELECT_KEY_INT_EQ(IntValue)

#define MOM_MESSAGE_KEYEQSTR(StrValue)	\
					MOM_SEQUENCE_KEY, MOM_SELECT_KEY_STR_EQ(StrValue)

#define MOM_MESSAGE_KEYNEXT(MsgId)	\
					MOM_SEQUENCE_KEY, MOM_SELECT_NEXT(MsgId)

#define MOM_MESSAGE_KEYPREV(MsgId)	\
					MOM_SEQUENCE_KEY, MOM_SELECT_PREV(MsgId)

/*
 *	Message Specifier Sequence
 *	==========================
 */
#define MOM_SEQUENCE_NATURAL	(11)
#define MOM_SEQUENCE_KEY		(12)
#define MOM_SEQUENCE_ANY		(13)

/*
 *	Message Specifier Selection Type
 *	================================
 */
#define MOM_SELECT_FIRST \
			MOM_SELECT_FIRST_CODE, (MomNullMsgIdFill()), 0, NULL, NULL

#define MOM_SELECT_LAST \
			MOM_SELECT_LAST_CODE, (MomNullMsgIdFill()), 0, NULL, NULL

#define MOM_SELECT_NEXT(MsgId) \
			MOM_SELECT_NEXT_CODE, (MsgId), 0, NULL, NULL

#define MOM_SELECT_PREV(MsgId) \
			MOM_SELECT_PREV_CODE, (MsgId), 0, NULL, NULL

#define MOM_SELECT_DIRECT(MsgId) \
			MOM_SELECT_DIRECT_CODE, (MsgId), 0, NULL, NULL

#define MOM_SELECT_DIRECT_RMT(RmtNode, RmtInstance, MsgId) \
			MOM_SELECT_DIRECT_RMT_CODE, (MsgId), 0, (RmtNode), (RmtInstance)

#define MOM_SELECT_REPLYTO(MsgId) \
			MOM_SELECT_REPLY_CODE, (MsgId), 0, NULL, NULL

#define MOM_SELECT_KEY_STR_EQ(StrValue) \
			MOM_SELECT_KEYSTREQ_CODE, (MomNullMsgIdFill()), 0, (StrValue), NULL

#define MOM_SELECT_KEY_STR_NE(StrValue) \
			MOM_SELECT_KEYSTRNE_CODE, (MomNullMsgIdFill()), 0, (StrValue), NULL

#define MOM_SELECT_KEY_STR_LT(StrValue) \
			MOM_SELECT_KEYSTRLT_CODE, (MomNullMsgIdFill()), 0, (StrValue), NULL

#define MOM_SELECT_KEY_STR_LE(StrValue) \
			MOM_SELECT_KEYSTRLE_CODE, (MomNullMsgIdFill()), 0, (StrValue), NULL

#define MOM_SELECT_KEY_STR_GT(StrValue) \
			MOM_SELECT_KEYSTRGT_CODE, (MomNullMsgIdFill()), 0, (StrValue), NULL

#define MOM_SELECT_KEY_STR_GE(StrValue) \
			MOM_SELECT_KEYSTRGE_CODE, (MomNullMsgIdFill()), 0, (StrValue), NULL

#define MOM_SELECT_KEY_INT_EQ(IntValue) \
			MOM_SELECT_KEYINTEQ_CODE, (MomNullMsgIdFill()), \
				(IntValue), NULL, NULL

#define MOM_SELECT_KEY_INT_NE(IntValue) \
			MOM_SELECT_KEYINTNE_CODE, (MomNullMsgIdFill()), \
				(IntValue), NULL, NULL

#define MOM_SELECT_KEY_INT_LT(IntValue) \
			MOM_SELECT_KEYINTLT_CODE, (MomNullMsgIdFill()), (IntValue), NULL, NULL

#define MOM_SELECT_KEY_INT_LE(IntValue) \
			MOM_SELECT_KEYINTLE_CODE, (MomNullMsgIdFill()), (IntValue), NULL, NULL

#define MOM_SELECT_KEY_INT_GT(IntValue) \
			MOM_SELECT_KEYINTGT_CODE, (MomNullMsgIdFill()), (IntValue), NULL, NULL

#define MOM_SELECT_KEY_INT_GE(IntValue) \
			MOM_SELECT_KEYINTGE_CODE, (MomNullMsgIdFill()), (IntValue), NULL, NULL

/*
 *	Message Specifier Start Codes
 *	=============================
 */
#define MOM_SELECT_FIRST_CODE		(256)
#define MOM_SELECT_LAST_CODE		(257)
#define MOM_SELECT_NEXT_CODE		(258)
#define MOM_SELECT_PREV_CODE		(259)
#define MOM_SELECT_REPLY_CODE		(260)

#define MOM_SELECT_DIRECT_CODE		(512)
#define MOM_SELECT_DIRECT_RMT_CODE	(513)

#define MOM_SELECT_KEYSTREQ_CODE	(1024)
#define MOM_SELECT_KEYSTRNE_CODE	(1025)
#define MOM_SELECT_KEYSTRLT_CODE	(1026)
#define MOM_SELECT_KEYSTRLE_CODE	(1027)
#define MOM_SELECT_KEYSTRGT_CODE	(1028)
#define MOM_SELECT_KEYSTRGE_CODE	(1029)

#define MOM_SELECT_KEYINTEQ_CODE	(2048)
#define MOM_SELECT_KEYINTNE_CODE	(2049)
#define MOM_SELECT_KEYINTLT_CODE	(2050)
#define MOM_SELECT_KEYINTLE_CODE	(2051)
#define MOM_SELECT_KEYINTGT_CODE	(2052)
#define MOM_SELECT_KEYINTGE_CODE	(2053)

/*
 *	Message Chain Flags
 *	===================
 */
#define MOM_CHAIN_NONE			(0)
#define MOM_CHAIN				TBD
#define MOM_CHAIN_LAST			TBD

/*
 *	Application Queue Key Types
 *	===========================
 */
#define MOM_APPQKEY_NONE		(0)
#define MOM_APPQKEY_STR 		(1)
#define MOM_APPQKEY_INT 		(2)

/*
 *	Application Queue Filter Types
 *	==============================
 */
#define MOM_APPQFILTER_NOCHANGE (-1)
#define MOM_APPQFILTER_CLEAR	(-2)
#define MOM_APPQFILTER_NONE 	(0)
#define MOM_APPQFILTER_STR		(1)
#define MOM_APPQFILTER_INT		(2)

/*
 *	Message Priorities
 *	==================
 */
#define MOM_PRIORITY_LOWEST 	(1)
#define MOM_PRIORITY_NORMAL 	(32767) /* 2^15 - 1 */
#define MOM_PRIORITY_HIGHEST	(65535) /* 2^16-1: fits in 16 unsigned bits */

#define MOM_PRIORITY(TripPrio, QueuePrio)	\
							(((TripPrio) * 65536) + (QueuePrio))

/*
 *	MomAttrSet() / MomAttrGet() Definitions
 *	=======================================
 */
#define MOM_ATTRBLOCK_INIT				(0x01030705)
#define MOM_ATTRBLOCK_UPDATE			(0x01030905)

#define MOM_ATTR_TYPE_INITIALIZE		(10)
#define MOM_ATTR_TYPE_UPDATE			(11)
#define MOM_ATTR_TYPE_DISK				(20)
#define MOM_ATTR_TYPE_MEMORY			(21)
#define MOM_ATTR_TYPE_EXPIRE			(22)
#define MOM_ATTR_TYPE_RETIRE			(23)
#define MOM_ATTR_TYPE_TIME				(25)
#define MOM_ATTR_TYPE_PRIORITY			(26)
#define MOM_ATTR_TYPE_KEY_STR			(30)
#define MOM_ATTR_TYPE_KEY_INT			(31)
#define MOM_ATTR_TYPE_T0				(35)
#define MOM_ATTR_TYPE_T1				(36)
#define MOM_ATTR_TYPE_AUTO_REGISTER 	(40)
#define MOM_ATTR_TYPE_AUTO_REGISTER_UPDATE		(41)
#define MOM_ATTR_TYPE_USER_ATTACHED 	(42)
#define MOM_ATTR_TYPE_INSTANCE_ATTACHED (44)
#define MOM_ATTR_TYPE_GROUP_JOIN		(50)
#define MOM_ATTR_TYPE_FILTER_STR		(55)
#define MOM_ATTR_TYPE_FILTER_INT		(56)
#define MOM_ATTR_TYPE_FILTER_CLEAR		(57)

#define MOM_ATTR_SET_INITIALIZE 	MOM_ATTR_TYPE_INITIALIZE, 0, 0, NULL, NULL
#define MOM_ATTR_SET_UPDATE 		MOM_ATTR_TYPE_UPDATE, 0, 0, NULL, NULL
#define MOM_ATTR_SET_DISK			MOM_ATTR_TYPE_DISK, 0, 0, NULL, NULL
#define MOM_ATTR_SET_MEMORY(MaxMsgs, MaxBytes) \
				MOM_ATTR_TYPE_MEMORY, (MaxMsgs), (MaxBytes), NULL, NULL
#define MOM_ATTR_SET_EXPIRE(Time) \
				MOM_ATTR_TYPE_EXPIRE, 0, 0, (Time), NULL
#define MOM_ATTR_SET_RETIRE(Time) \
				MOM_ATTR_TYPE_RETIRE, 0, 0, (Time), NULL
#define MOM_ATTR_SET_TIME			MOM_ATTR_TYPE_TIME, 0, 0, NULL, NULL
#define MOM_ATTR_SET_PRIORITY		MOM_ATTR_TYPE_PRIORITY, 0, 0, NULL, NULL
#define MOM_ATTR_SET_KEY_STR(Offset, Size) \
				MOM_ATTR_TYPE_KEY_STR, (Offset), (Size), NULL, NULL
#define MOM_ATTR_SET_KEY_INT(Offset, Size) \
				MOM_ATTR_TYPE_KEY_INT, (Offset), (Size), NULL, NULL
#define MOM_ATTR_SET_T0(Value)		MOM_ATTR_TYPE_T0, (Value), 0, NULL, NULL
#define MOM_ATTR_SET_T1(Value)		MOM_ATTR_TYPE_T1, (Value), 0, NULL, NULL
#define MOM_ATTR_SET_AUTO_REGISTER	\
				MOM_ATTR_TYPE_AUTO_REGISTER, 0, 0, NULL, NULL
#define MOM_ATTR_SET_AUTO_REGISTER_UPDATE	\
				MOM_ATTR_TYPE_AUTO_REGISTER_UPDATE, 0, 0, NULL, NULL
#define MOM_ATTR_SET_USER_ATTACHED	\
				MOM_ATTR_TYPE_USER_ATTACHED, 0, 0, NULL, NULL
#define MOM_ATTR_SET_GROUP_JOIN(GroupName) \
				MOM_ATTR_TYPE_GROUP_JOIN, 0, 0, (GroupName), NULL
#define MOM_ATTR_SET_FILTER_STR(Offset, Pattern) \
				MOM_ATTR_TYPE_FILTER_STR, (Offset), 0, (Pattern), NULL
#define MOM_ATTR_SET_FILTER_INT(Offset, Pattern) \
				MOM_ATTR_TYPE_FILTER_INT, (Offset), 0, (Pattern), NULL
#define MOM_ATTR_SET_FILTER_CLEAR \
				MOM_ATTR_TYPE_FILTER_CLEAR, 0, 0, NULL, NULL

	/*
	 *	MomSys Dynamic Debugging
	 *	========================
	 */
#define MOM_DEBUG_STOP	(0)
#define MOM_DEBUG_START (1)

	/*
	 *	Typedefs
	 *	========
	 */
typedef UXINT MOM_MSG_HANDLE[2];

typedef struct _MOM_MSGID
{
	MOM_MSG_HANDLE Local;
	MOM_MSG_HANDLE Remote;
}
MOM_MSGID;

typedef struct _MOM_ATTRBLOCK_APPQUEUE
{
	XINT AttrBlockType; 				/* MOM_ATTR_TYPE_INITIALIZE or,
											MOM_ATTR_TYPE_UPDATE */
	XINT MemoryQueue;					/* TRUE if memory App Queue */
	XINT MemoryQueueByteLimit;			/* integer > 0, or MOM_NOLIMIT */
	XINT MemoryQueueMsgsLimit;			/* integer > 0, or MOM_NOLIMIT */
	XINT PriorityQueue; 				/* TRUE if natural seq is Priority */
	XINT AutoRegister;					/* 0 - Not registered
										   1 - auto-registered
										   2 - auto-registered (replace) */
	XINT ExpireTime;					/* Time limit to hold incomplete msgs */
	XINT RetireTime;					/* Time limit to hold completed msgs */
	XINT KeyType;						/* MOM_APPQKEY_STR, MOM_APPQKEY_INT or,
											MOM_APPQKEY_NONE */
	XINT KeyOffset; 					/* Integer >= 0 */
	XINT KeySize;						/* Integer > 0 */
	XINT T0;							/* Integer > 0 */
	XINT T1;							/* Integer > 0 */
	XINT UserAttached;					/* TRUE if App Queue is user-attached */
	XINT CatFilterType; 				/* MOM_APPQFILTER_STR,
											MOM_APPQFILTER_INT,
											MOM_APPQFILTER_NONE,
											MOM_APPQFILTER_NOCHANGE, or
											MOM_APPQFILTER_CLEAR */
	CHAR CatGroupName[XIPC_LEN_XIPCNAME+1];
	CHAR CatFilter[MOM_LEN_FILTER+1];
}
MOM_ATTRBLOCK_APPQUEUE;

/*
 *	MomInfoSys() Return Structures
 *	==============================
 */
typedef struct _MOMINFOSYS
{
	XINT MaxUsers;						/* Max configured users */
	XINT MaxDiskAppQueues;				/* Max configured disk App Queueus */
	XINT MaxMemAppQueues;				/* Max configured memory App Queues */
	XINT MaxRemoteAppQueues;			/* Max configured remote App Queues */
	XINT MaxMemText;					/* Max configured mem-queue text */
	XINT MaxMemMessages;				/* Max configured mem-queue messages */
	XINT MaxMsgLength;					/* Max configured message size */
	XINT CurrUsers; 					/* Current number of MomSys users */
	XINT CurrDiskAppQueues; 			/* Current number of disk App Queues */
	XINT CurrMemAppQueues;				/* Current number of mem App Queues */
	XINT CurrRemoteAppQueues;			/* Current number of remote App Que */
	XINT CurrMemText;					/* Size of current mem-queue text */
	XINT CurrMemMessages;				/* Current mem-queue messages */
	XINT EpisodeNumber; 				/* Num of current episode(0 is first) */
	UXINT CreateTime;					/* Instance create time (UTC format) */
	UXINT StartTime;					/* Instance start time (UTC format) */
	CHAR InstName[XIPC_LEN_NETNAME + 1];		/* Instance name */
	CHAR CfgFileName[XIPC_LEN_PATHNAME + 1];	/* Configuration file name */
	CHAR LogFileName[XIPC_LEN_PATHNAME + 1];	/* Instance LogFile name */
	CHAR NameSpace[XIPC_LEN_XIPCNAME + 1];		/* Namespace affiliation */
}
MOMINFOSYS;

typedef struct _MOMINFOMRX
{
	XINT CountReads;					/* Total reads from MR */
	XINT CountInserts;					/* Total inserts into MR */
	XINT CountDeletes;					/* Total deletes from MR */
	XINT CountUpdates;					/* Total updates within MR */
	XINT CurrSize;						/* Current size of MR */
	XINT CurrFree;						/* Free space in MR */
	XINT RetiredMsgs;					/* Not implemented */
	XINT ExpiredMsgs;					/* Not implemented */
	XINT CleanupInProgress; 			/* 1 - Cleanup in progress; 0 - Not */
	XINT CleanupPercentComplete;		/* Percent cleanup complete */
	CHAR FileName[XIPC_LEN_PATHNAME + 1]; /* Msg repository */
	CHAR JournalExpiredMsgs[XIPC_LEN_PATHNAME + 1]; /* For expired messages */
	CHAR JournalRetiredMsgs[XIPC_LEN_PATHNAME + 1]; /* For purged messages */
}
MOMINFOMRX;

typedef struct _MOMINFOMR
{
	MOMINFOMRX MRI; 					/* MRI Information */
	MOMINFOMRX MRO; 					/* MRO Information */
	UXINT NextCleanupTime;				/* Next scheduled MR Cleanup */
	CHAR ScheduleCleanup[MOM_LEN_SCHEDULE + 1]; /* MR Cleanup schedule */
	CHAR ScheduleCompact[MOM_LEN_SCHEDULE + 1]; /* MR Compaction schedule */
}
MOMINFOMR;

typedef struct _MOMINFOCM
{
	XINT MaxLinks;						/* Max configured number of links */
	XINT CurrLinksUp;					/* Current number of links */
	XINT CurrLinksDown; 				/* Current number of links */
	XINT ActiveCSIs;					/* Number of CSI's running */
	XINT ActiveCSOs;					/* Number of CSO's running */
#if 0
	XINT CountMsgsIn;					/* Total inbound messages recd */
	XINT CountMsgsOut;					/* Total outbound messages sent */
#endif
}
MOMINFOCM;

/*
 *	MomInfoMessage() Return Structure
 *	=================================
 */
typedef struct _MOMINFOMESSAGE
{
	MOM_MSGID MsgId;					/* Message Id */
	XINT MsgLength; 					/* Length of message */
	XINT AQid;							/* AQid of app-queue */
	XINT ReplyAQid; 					/* AQid of response app-queue */
	UXINT SendTime; 					/* Time message was sent (UTC) */
	UXINT ExpirationTime;				/* Time of message expiration (UTC) */
	XINT TripPriority;					/* Trip Priority msg was sent with */
	XINT QueuePriority; 				/* Queue Priority msg was sent with */
	XINT TrackingLevel; 				/* Tracking level message was sent with */
	XINT LatestStatus;					/* Latest known message status */
	XINT SenderUid; 					/* Uid of sending user */
	CHAR SenderInstance[XIPC_LEN_NETNAME+1]; /* Instance of sending user */
	CHAR SenderNode[XIPC_LEN_HOSTNAME+1]; /* Network Node of sending user */
	CHAR MessageTag[MOM_LEN_MESSAGETAG+1]; /* Message Tag */
	CHAR DataTranslationType[XIPC_LEN_DTSDATATYPE+1]; /* DTS Type of message */
}
MOMINFOMESSAGE;

/*
 *	MomInfoAppQueue() Return Structures
 *	===================================
 */
typedef struct _MOM_APPQUEUEWLISITEM
{
	XINT OpCode;						/* MOM_OPCODE_SEND,MOM_OPCODE_RECEIVE */
#ifdef XIPC_WLISTTIMEOUT
	UXINT TimeOut;						/* Time Operation will Time Out */
#endif
	union
	{
		struct
		{
			XINT Uid;					/* User blocked */
			XINT MsgSize;				/* Sending Msg */
			XINT MsgPrio;				/* Msg Priority */
			XINT TrackingLevel;
			UXINT Time;
		}
		Send;

		struct
		{
			XINT Uid;					/* User blocked */
			XINT MsgSequence;
			XINT MsgSelector;
			UXINT Time;
		}
		Receive;
	}
	u;
}
MOM_APPQUEUEWLISTITEM;

typedef struct _MOM_CURSOR
{
	XINT	AQid;
	XINT	Cursor;
}
MOM_CURSOR;

typedef struct _MOMINFOAPPQUEUE
{
	MOM_ATTRBLOCK_APPQUEUE	AttrBlock;	/* Attribute block for App-Queue */
	XINT AQid;							/* Application Queue Id */
	UXINT CreateTime;					/* Time App-Queue created */
	XINT CreateUid; 					/* Uid of creating user */
	XINT LocalRemote;					/* 0 - MOM_APPQUEUE_LOCAL  or,
										   1 - MOM_APPQUEUE_REMOTE App-Queue */
	XINT RemoteState;					/* State info of a remote queue
										   MOM_APPQUEUE_VERIFIED or
										   MOM_APPQUEUE_NOTVERIFIED */
	XINT CountMsgsIn;					/* Total number of msgs enqueued
											on queue */
	XINT CountMsgsOut;					/* Total number of msgs dequeued
											from queue */
	XINT CountMsgsSentTo;				/* Num messages sent to remote queue */
	XINT NumMsgs;						/* Current number of msgs on queue */
	XINT NumBytes;						/* Current number of bytes on queue */
	XINT NumMsgsHWM;					/* Number of msgs - High Water Mark */
	XINT NumBytesHWM;					/* Number of bytes - High Water Mark */
	XINT AttachedUid;					/* Uid of Attached user (if any) */
	XINT WListTotalLength;				/* Num of items in App-Queue's WList */
	XINT WListInitialCursor;			/* Initial value for scanning WList */
	MOM_APPQUEUEWLISTITEM WListFirstItem; /* Wait List on AppQueue */
	CHAR Name[XIPC_LEN_XIPCNAME+1]; 	/* Name of App-Queue */
	CHAR InstName[XIPC_LEN_NETNAME+1];	/* Instance name where App-Queue is */
	CHAR NodeName[XIPC_LEN_HOSTNAME+1]; /* Node name where App-Queue is */
}
MOMINFOAPPQUEUE;

/*
 *	MomInfoLink() Return Structure
 *	==============================
 */
typedef struct _MOMINFOLINK
{
	XINT LinkId;						/* Link Id */
	XINT Attributes;					/* Attributes of Link - not in Ver3.0 */
	XINT Protocol;						/* Protocol of Link */
	XINT LinkStatus;					/* Current status of Link */
	XINT InCommServerPID;				/* Process ID of CSI */
	XINT OutCommServerPID;				/* Process ID of CSO */
	XINT CountMsgsSent; 				/* Total messages sent out over Link */
	XINT CountMsgsReceived; 			/* Total messages recd. over Link */
	XINT NumBacklogMsgs;				/* Number of messages waiting to exit
											over Link */
	UXINT StartupTime;					/* Time Link was started (UTC format) */
	CHAR Timeout[XIPC_LEN_TIMELIMIT+1]; /* When does Link time out -
											not in Ver3.0 */
	CHAR RetryInterval[XIPC_LEN_TIMELIMIT+1]; /* How often retries attempted -
											not in Ver3.0 */
	CHAR RemoteInstance[XIPC_LEN_NETNAME+1]; /* Name of remote instance */
	CHAR RemoteNode[XIPC_LEN_HOSTNAME+1]; /* Name of remote host */
}
MOMINFOLINK;

/*
 *	MomInfoUser() Return Structures
 *	===============================
 */
typedef struct _MOM_USERALISTITEM
{
	XINT AUid;							/* AUID of Async Operation */
}
MOM_USERALISTITEM;

typedef struct _MOM_USERWLISTITEM
{
	XINT OpCode;						/* MOM_OPCODE_SEND, MOM_OPCODE_RECEIVE
											or MOM_OPCODE_EVENT */
	UXINT TimeOut;						/* Time Operation will Time Out*/

	union
	{
		struct
		{
			XINT AQid;					/* App Queue blocked */
			XINT MsgSize;				/* Size of sending message */
			XINT MsgPrio;				/* Message priority */
			XINT TrackingLevel;
			UXINT Time;
		}
		Send;

		struct
		{
			XINT AQid;					/* App Queue blocked */
			XINT MsgSequence;		/* Natural or Key */
			XINT MsgSelector;
			UXINT Time;
		}
		Receive;

		struct
		{
			XINT EventCode;
			UXINT Time;
		}
		Event;
	}
	u;
}
MOM_USERWLISTITEM;

typedef struct _MOMINFOUSER
{
	XINT Uid;							/* User id */
	XINT Pid;							/* Process id of user */
	XINT Tid;							/* Thread id of user */
	UXINT LoginTime;						/* Time of Login (UTC format) */
	XINT CountMsgsSent; 				/* Number of messages sent by user */
	XINT CountMsgsReceived; 			/* Number of messages recd by user */
	XINT WListTotalLength;				/* Should never be > 1 in Version 3 */
	XINT WListInitialCursor;			/* Initial value for scanning WList */
	MOM_USERWLISTITEM WListFirstItem;	/* Wait List info. */
	XINT AListTotalLength;				/* Number of Async Ops for user */
	XINT AListInitialCursor;			/* Initial value for scanning AList */
	MOM_USERALISTITEM AListFirstItem;	/* AList Info */
	CHAR Name[XIPC_LEN_XIPCNAME + 1];	/* Name of user */
}
MOMINFOUSER;

typedef struct _MOMSEND_OPTARGS
{
	MOM_MSG_HANDLE	RequestMsgHandle;
	UXINT			 ExpirationTime;
	CHAR			MessageTag[MOM_LEN_MESSAGETAG+1];
	CHAR			DataTranslation[XIPC_LEN_DTSDATATYPE+1];
}
MOMSEND_OPTARGS;

typedef MOMSEND_OPTARGS * MOMSEND_OPTARGS_PTR;

/*********************************************************************
** API Prototypes
*********************************************************************/

#ifdef	C_PLUSPLUS
extern	"C" {
#endif

XINT GLOBAL XAPI
MomAbortAsync P1(
	XINT, AUid);

XINT GLOBAL XAPI
MomAccess P1(
	CHAR *, AppQueueNameArg);

XINT GLOBAL XAPI
MomDeaccess P1(
	XINT, AQidArg);

XINT GLOBAL XAPI
MomCreate P2(
	CHAR *, AppQueueNameArg,
	MOM_ATTRBLOCK_APPQUEUE *, AttrBlockPtrArg);

XINT GLOBAL XAPI
MomDelete P1(
	XINT, AQidArg);

XINT GLOBAL XAPI
MomDestroy P1(
	XINT, AQidArg);

XINT XCDECL
MomEvent C_PROTOARGS((XINT EventCode, ...));

#ifdef SUN41_COMPILER
XINT XAPI
MomReceive C_PROTOARGS((
	XINT AppQueueArg,
	XANY *BufferArg,
	XINT LengthArg,
	XINT SequenceArg,
	XINT SelectionType,
	MOM_MSGID MsgIdArg,
	XINT KeyIntValueArg,
	CHAR *KeyCharValueArg,
	XANY *SelectPtrArg,
	XINT *ReplyAppQueArg,
	MOM_MSGID *RetMsgIdArg,
	MOMINFOMESSAGE *RetMomInfoMsgArg,
	XINT Option1Arg,
	XINT Option2Arg,
	XANY *Option3Arg,
	XIPC_CALLBACK_FUNC *Option4,
	));
#else
XINT GLOBAL XAPI
MomReceive P16(
	XINT, AppQueueArg,				/* Receive AppQueue Id */
	XANY *, BufferArg,				/* Buffer pointer */
	XINT, LengthArg,				/* Buffer length */

/*	Message Specifier
	================= */
	XINT, SequenceArg,				/* Message specifier view code */
	XINT, SelectionType,			/* Message specifier start code */
	MOM_MSGID , MsgIdArg,			/* Message specifier start message id */
	XINT, KeyIntValueArg,			/* Message specifier key value (int) */
	CHAR *, KeyCharValueArg,		/* Message specifier key value (string) */
	XANY *, SelectPtrArg,			/* Message specifier additional pointer */

	XINT *, ReplyAppQueArg, 		/* Sender Reply App Queue Id */
	MOM_MSGID *, RetMsgIdArg,		/* Message Id */
	MOMINFOMESSAGE *, RetMomInfoMsgArg, /* Message */

/*	Blocking Options
	================ */
	XINT, Option1Arg,
	XINT, Option2Arg,
	XANY *,Option3Arg,
	XIPC_CALLBACK_FUNC *, Option4);

#endif /* SUN41_COMPILER */

XINT GLOBAL XCDECL
MomSend C_PROTOARGS((
	XINT TargetAQidArg, 			/* Send AppQueue Id */
	XANY * MsgBufArg,				/* Buffer pointer */
	XINT MsgLenArg, 				/* Buffer length */
	UXINT PriorityArg,				/* Message priority */
	XINT TrackingLevelArg,			/* Message Tracking Level */
	XINT ReplyAQidArg,				/* Message reply AppQueue Id */
	MOM_MSGID * RetMsgIdArg,		/* Message Id */
	...));

XINT GLOBAL XAPI
MomStatus P2(
	MOM_MSGID , MsgIdArg,
	XINT *, RetStatusArg);

XINT GLOBAL XAPI
MomStatusWait P6(
	MOM_MSGID , MsgIdArg,
	XINT , StatusArg,
/*	Blocking Options
	================ */
	XINT, Option1Arg,
	XINT, Option2Arg,
	XANY *,Option3Arg,
	XIPC_CALLBACK_FUNC *, Option4);

XINT GLOBAL XAPI
MomAttrSet P6(
	MOM_ATTRBLOCK_APPQUEUE *, AttrBlockPtrArg,
	XINT, AttrTypeArg,
	XINT, AttrValue1Arg,
	XINT, AttrValue2Arg,
	XANY *, AttrValue3PtrArg,
	XANY *, AttrValue4PtrArg);

XINT GLOBAL XAPI
MomAttrGet P6(
	MOM_ATTRBLOCK_APPQUEUE *, AttrBlockPtrArg,
	XINT, AttrTypeArg,
	XINT *, AttrValue1PtrArg,
	XINT *, AttrValue2PtrArg,
	XANY *, AttrValue3PtrArg,
	XANY *, AttrValue4PtrArg);

MOM_ATTRBLOCK_APPQUEUE * GLOBAL XAPI
MomAttrFill P1(
	XINT, AttrType);

MOM_MSGID GLOBAL XAPI
MomNullMsgIdFill P0();

XINT GLOBAL XAPI
MomInfoAppQueue P2(
	XINT , AQidArg,
	MOMINFOAPPQUEUE *, InfoAppQueuePtr);

XINT GLOBAL XAPI
MomInfoAppQueueWList P3(
	XINT , AQidArg,
	XINT *, CursorArg,
	MOM_APPQUEUEWLISTITEM *, WListItemPtr);

XINT GLOBAL XAPI
MomInfoLink P2(
	XINT , LinkIdArg,
	MOMINFOLINK *, InfoLinkPtr);

XINT GLOBAL XAPI
MomInfoMessage P2(
	MOM_MSGID , MsgIdArg,
	MOMINFOMESSAGE *, InfoMessagePtr);

XINT GLOBAL XAPI
MomInfoSys P3(
	MOMINFOSYS *, InfoSysPtr,
	MOMINFOMR *, InfoMRPtr,
	MOMINFOCM *, InfoCMPtr);

XINT GLOBAL XAPI
MomInfoUser P2(
	XINT , UidArg,
	MOMINFOUSER *, InfoUserPtr);

XINT GLOBAL XAPI
MomInfoUserAList P3(
	XINT , UidArg,
	XINT *, CursorArg,
	MOM_USERALISTITEM *, AListItemPtr);

XINT GLOBAL XAPI
MomLastMsg P3(
	XINT , AQidArg,
	XANY *, MsgBufArg,				/* Buffer pointer */
	XINT , MsgLenArg);				/* Buffer length */

CHAR FAR * GLOBAL XAPI
PifSetNoVerify P1(
	CHAR *, AppQueueNameArg);

#ifdef	C_PLUSPLUS
}
#endif

#endif /* MOMPUBD_H */
