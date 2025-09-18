/*********************************************************** MEMPUBD.H
**						   (MemSys - Memory System)
**
**	Name:	MEMPUBD.H
**	----
**
**	Title:	MemSys Public Header File - Declarations
**	-----
**
**	Description:
**	-----------
**	This file contains the public declarations for the
**	Memory System.
**
**	Last Revision:
**	-------------
**	$Revision:   1.0  $
**	$Date:   Oct 14 1999 08:17:28  $
**	$Author:   MSALEH  $
**
**	Revision History:
**	----------------
**
**	Rev   Date		By	 Description
**	---   --------	---  -----------
**	241d  5-1-93	YF	 Define function MemSection for DOS to overcome the
**						 problem of the structure that is left on the stack.
**	300   3-13-95	YF	 Change defintion of OwnerPriv and OtherPriv in
**						 MEMINFOSEC to XINT.
**
**
**	Copyright (C) Momentum Software Corporation, 1989
**
*********************************************************************/

#ifndef MEMPUBD_H
#define MEMPUBD_H MEMPUBD_H

/*********************************************************************
** constants
*********************************************************************/

/* return codes */

#define MEM_SUCCESS 	XIPC_SUCCESS

/* API arguments */

#ifndef WINNT
#define MEM_PRIVATE 	XIPC_PRIVATE
#endif

/* the following values must always be negative */
#define MEM_ATOMIC		((XINT)-200)
#define MEM_ALL 		((XINT)-201)
#define MEM_ANY 		((XINT)-202)

#define MEM_NOOWNER 	((XINT)-1)

#define MEM_RW			((XINT)3)
#define MEM_RO			((XINT)2)
#define MEM_WO			((XINT)1)
#define MEM_NA			((XINT)0)
#define MEM_ADD_R		(MEM_RO+4)
#define MEM_ADD_W		(MEM_WO+4)
#define MEM_RMV_R		(MEM_RO+8)
#define MEM_RMV_W		(MEM_WO+8)
#define MEM_NC			(MEM_NA+4)
#define MEM_RP			(MEM_NA+8)

/* maximums, sizes and limits */

#define MEM_LEN_PATHNAME	XIPC_LEN_PATHNAME
#define MEM_LEN_USERNAME	XIPC_LEN_USERNAME
#define MEM_LEN_XIPCNAME	XIPC_LEN_XIPCNAME
#define MEM_LEN_PARMLINE	XIPC_LEN_PARMLINE
#define MEM_MAX_MIDLIST 	((XINT)8)
#define MEM_LEN_MIDLIST 	(MEM_MAX_MIDLIST+(XINT)1)
#define MEM_LEN_INFOLIST	((XINT)10)
#define MEM_LEN_SECTION 	(sizeof(SECTION))

/* user hold list indicators */

#define MEM_LOCKED		((XINT)-801)
#define MEM_OWNED		((XINT)-802)

/* user blocking status */

#define MEM_USER_NOTWAITING XIPC_USER_NOTWAITING
#define MEM_BLOCKEDWRITE	((XINT)-901)
#define MEM_BLOCKEDREAD 	((XINT)-902)
#define MEM_BLOCKEDOWN		((XINT)-903)
#define MEM_BLOCKEDLOCK 	((XINT)-904)

/* misc. option flags */

#define MEM_FREEZE			XIPC_FREEZE
#define MEM_RETURN			XIPC_RETURN
#define MEM_NOOPTIONS		XIPC_NOOPTIONS

/* blocking options */

#define MEM_BLOCKOPT_WAIT	XIPC_BLOCKOPT_WAIT
#define MEM_BLOCKOPT_NOWAIT XIPC_BLOCKOPT_NOWAIT
#define MEM_BLOCKOPT_TIMEOUT	XIPC_BLOCKOPT_TIMEOUT
#define MEM_BLOCKOPT_SYNC		XIPC_BLOCKOPT_SYNC

#define MEM_BLOCKOPT_CALLBACK	XIPC_BLOCKOPT_CALLBACK
#define MEM_BLOCKOPT_POST		XIPC_BLOCKOPT_POST
#define MEM_BLOCKOPT_IGNORE 	XIPC_BLOCKOPT_IGNORE
#define MEM_BLOCKOPT_ASYNC		XIPC_BLOCKOPT_ASYNC

#define MEM_WAIT				XIPC_WAIT
#define MEM_NOWAIT				XIPC_NOWAIT
#define MEM_TIMEOUT(x)			XIPC_TIMEOUT(x)
#define MEM_CALLBACK(f,r)		XIPC_CALLBACK(f,r)
#define MEM_POST(s,r)			XIPC_POST(s,r)
#define MEM_IGNORE(r)			XIPC_IGNORE(r)

/* freeze options */

#define MEM_MASKTRAPS			XIPC_MASKTRAPS
#define MEM_NOMASKTRAPS 		XIPC_NOMASKTRAPS

/*********************************************************************
** trigger codes and macros
*********************************************************************/

#define MEM_T_READ_CODE 		((XINT)800)
#define MEM_T_WRITE_CODE		((XINT)801)
#define MEM_T_LOCK_CODE 		((XINT)802)
#define MEM_T_UNLOCK_CODE		((XINT)803)
#define MEM_T_USER_READ_CODE	((XINT)804)
#define MEM_T_USER_WRITE_CODE	((XINT)805)
#define MEM_T_USER_LOCK_CODE	((XINT)806)
#define MEM_T_USER_UNLOCK_CODE	((XINT)807)
#define MEM_T_POOL_HIGH_CODE	((XINT)808)
#define MEM_T_SECTION_HIGH_CODE ((XINT)809)
#define MEM_T_POOL_LOW_CODE 	((XINT)810)
#define MEM_T_SECTION_LOW_CODE	((XINT)811)

#define MEM_T_READ(Mid,Offset,Size) 			\
			Mid, Offset, Size, MEM_T_READ_CODE, ((XINT)0)
#define MEM_T_WRITE(Mid,Offset,Size)			\
			Mid, Offset, Size, MEM_T_WRITE_CODE, ((XINT)0)
#define MEM_T_LOCK(Mid,Offset,Size) 			\
			Mid, Offset, Size, MEM_T_LOCK_CODE, ((XINT)0)
#define MEM_T_UNLOCK(Mid,Offset,Size)			\
			Mid, Offset, Size, MEM_T_UNLOCK_CODE, ((XINT)0)
#define MEM_T_USER_READ(Mid,Offset,Size,Uid)	\
			Mid, Offset, Size, MEM_T_USER_READ_CODE, Uid
#define MEM_T_USER_WRITE(Mid,Offset,Size,Uid)	\
			Mid, Offset, Size, MEM_T_USER_WRITE_CODE, Uid
#define MEM_T_USER_LOCK(Mid,Offset,Size,Uid)	\
			Mid, Offset, Size, MEM_T_USER_LOCK_CODE, Uid
#define MEM_T_USER_UNLOCK(Mid,Offset,Size,Uid)	\
			Mid, Offset, Size, MEM_T_USER_UNLOCK_CODE, Uid
#define MEM_T_POOL_HIGH(N)						\
			(XINT)0, (XINT)0, (XINT)0, MEM_T_POOL_HIGH_CODE, (N)
#define MEM_T_POOL_LOW(N)						\
			(XINT)0, (XINT)0, (XINT)0, MEM_T_POOL_LOW_CODE , (N)
#define MEM_T_SECTION_HIGH(N)					\
			(XINT)0, (XINT)0, (XINT)0, MEM_T_SECTION_HIGH_CODE, (N)
#define MEM_T_SECTION_LOW(N)					\
			(XINT)0, (XINT)0, (XINT)0, MEM_T_SECTION_LOW_CODE, (N)

/* Info Options */

#define MEM_INFO_FIRST			XIPC_INFO_FIRST
#define MEM_INFO_NEXT(Id)		XIPC_INFO_NEXT(Id)

/*********************************************************************
** macros
*********************************************************************/

#define MEM_FILL(c) ((CHAR FAR *)((XINT)(c) == 0 ? -1 : (XINT)(c)))

/*********************************************************************
** data types
*********************************************************************/

/*
 * The SECTION structure is used for manipulating
 * MemSys section overlays. MemSection() can be
 * used to initialize a sction with values.
 */
typedef struct _SECTION
{
	XINT   Mid; 		/* MemSys memory seg ID   */
	XINT   Offset;		/* Offset into segment	  */
	XINT   Size;		/* Byte size of section   */
}
SECTION;

typedef SECTION MIDLIST[MEM_LEN_MIDLIST];
typedef SECTION FAR *PMIDLIST;
typedef SECTION FAR *PSECTION;

#ifndef MEM_EOL
#define MEM_EOL MemEol
#endif

#ifndef MEM_ALLHELD
#define MEM_ALLHELD MemAllHeld
#endif

/*
 * The MEMINFOSEC structure is used for retreiving status
 * information about a particular MemSys section overlay.
 * MemInfoSec() fills the structure with the data
 * about the Section it is passed.
 */
typedef struct _MEMINFOSEC
{
	XINT Mid;				/* MemSys segment ID */
	XINT Offset;			/* Offset into the segment */
	XINT Size;				/* Section size in bytes */
	XINT OwnerUid;			/* Uid of section owner */
	XINT OwnerPriv; 		/* Owner access privileges */
	XINT OtherPriv; 		/* Other access privileges */
}
MEMINFOSEC;

typedef struct _MEM_MEMSLISTITEM
{
	XINT OwnerUid;
	XINT OwnerPriv;
	XINT OtherPriv;
	XINT Mid;
	XINT Offset;
	XINT Size;
}
MEM_MEMSLISTITEM;

typedef struct _MEM_MEMWLISTITEM
{
	XINT Uid;
	XINT OpCode;			/* Own, Lock, Read or Write */
	XINT Offset;
	XINT Size;
}
MEM_MEMWLISTITEM;

/*
 * The MEMINFOMEM structure is used for retreiving status
 * information about a particular MemSys semaphore.
 * MemInfoMem() fills the structure with the data
 * about the Mid it is passed.
 */
typedef struct _MEMINFOMEM
{
	XINT Mid;
	XINT CreateTime;		/* Time segment was created */
	XINT CreateUid; 		/* The Uid who created it */
	XINT Size;				/* Size of segment (bytes)*/
	XINT NumSections;		/* Num of sections on seg */
	XINT NumSecOwned;		/* Num of owned sections  */
	XINT NumSecLocked;		/* Num of locked sections */
	XINT NumBytesOwned; 	/* Bytes owned on segment */
	XINT NumBytesLocked;	/* Bytes locked on segment */
	XINT CountWrite;		/* Num writes to segment  */
	XINT CountRead; 		/* Num reads from segment */
	XINT LastUidWrite;		/* Last Uid to write segment */
	XINT LastUidRead;		/* Last Uid to read segment */
	XINT LastUidOwned;		/* Last Uid to own on segment */
	XINT LastUidLocked; 	/* Last Uid to lock on segment */
	XINT SListTotalLength;
	XINT SListOffset;
	XINT SListLength;
	XINT WListTotalLength;
	XINT WListOffset;
	XINT WListLength;
	MEM_MEMSLISTITEM SList[MEM_LEN_INFOLIST];
	MEM_MEMWLISTITEM WList[MEM_LEN_INFOLIST];
	CHAR Name[MEM_LEN_XIPCNAME + 1];	/* Segment name */
}
MEMINFOMEM;

typedef struct _MEM_USERQLISTITEM
{
	XINT Mid;
	XINT Offset;
	XINT Size;
}
MEM_USERQLISTITEM;

typedef struct _MEM_USERHLISTITEM
{
	XINT OpCode;			/* Locked or Owned */
	XINT Mid;
	XINT Offset;
	XINT Size;
}
MEM_USERHLISTITEM;

typedef struct _MEM_USERWLISTITEM
{
	XINT Mid;
	XINT Offset;
	XINT Size;
}
MEM_USERWLISTITEM;

/*
 * The MEMINFOUSER structure is used for retreiving status
 * information about a particular MemSys user.
 * MemInfoUser() fills the structure with the data
 * about the Uid it is passed.
 */
typedef struct _MEMINFOUSER
{
	XINT Uid;
	XINT Pid;				/* Process Id of user	  */
	XTID Tid;				/* Thread ID of user	  */
	XINT LoginTime; 		/* Time of login to MemSys*/
	XINT TimeOut;			/* Remaining timeout secs */
	XINT WaitType;			/* Type of operation blkd */
	XINT NumSecOwned;		/* Num sects owned by Uid */
	XINT NumSecLocked;		/* Num sects lcked by Uid */
	XINT NumBytesOwned; 	/* Num bytes owned by Uid */
	XINT NumBytesLocked;	/* Num bytes lcked by Uid */
	XINT CountWrite;		/* Num of Uid write opers */
	XINT CountRead; 		/* Num of Uid read opers  */
	XINT HListTotalLength;
	XINT HListOffset;
	XINT HListLength;
	XINT WListTotalLength;
	XINT WListOffset;
	XINT WListLength;
	XINT QListTotalLength;
	XINT QListOffset;
	XINT QListLength;
	MEM_USERHLISTITEM HList[MEM_LEN_INFOLIST];
	MEM_USERWLISTITEM WList[MEM_LEN_INFOLIST];
	MEM_USERQLISTITEM QList[MEM_LEN_INFOLIST];
	CHAR Name[MEM_LEN_XIPCNAME + 1];	/* User login name */
	CHAR NetLoc[XIPC_LEN_NETLOC + 1];	/* Name of client Node */
}
MEMINFOUSER;

/*
 * The MEMINFOSYS structure is used for retreiving status
 * information about the MemSys instance.
 * MemInfoSys() fills the structure with the data
 * about the instance.
 */
typedef struct _MEMINFOSYS
{
	XINT MaxUsers;				/* Max configured users */
	XINT CurUsers;				/* Current num of users */
	XINT MaxSegments;			/* Max configured segments */
	XINT CurSegments;			/* Current num of segments */
	XINT MaxNodes;				/* Max configured nodes */
	XINT FreeNCnt;				/* Current available nodes */
	XINT MaxSections;			/* Max configured sections */
	XINT FreeSCnt;				/* Current available sects */
	XINT MemPoolSizeBytes;		/* Configured mem pool size */
	XINT MemTickSize;			/* Configured mem tick size */
	XINT MemPoolTotalAvail; 	/* Free text pool space */
	XINT MemPoolLargestBlk; 	/* Largest contig block */
	XINT MemPoolMaxPosBlks; 	/* Max possible tick blocks */
	XINT MemPoolTotalBlks;		/* Number allocated blocks */
	CHAR Name[MEM_LEN_PATHNAME + 1];	/* InstanceFilename */
}
MEMINFOSYS;

#endif /* MEMPUBD_H */

/*
** END ***************************************************** MEMPUBD.H
*/
