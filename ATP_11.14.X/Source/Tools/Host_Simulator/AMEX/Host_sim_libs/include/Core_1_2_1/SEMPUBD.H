/*********************************************************** SEMPUBD.H
**					   (SemSys - Semaphore System)
**
**	Name:	SEMPUBD.H
**	----
**
**	Title:	SemSys Public Header File - Declarations
**	-----
**
**	Description:
**	-----------
**	This file contains the public declarations for the
**	Semaphore System.
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

#ifndef SEMPUBD_H
#define SEMPUBD_H SEMPUBD_H

/*********************************************************************
** constants
*********************************************************************/

/* API arguments */

#define SEM_PRIVATE 	XIPC_PRIVATE
#define SEM_EVENT		((XINT)-1)

/* the following values must
always be negative */

#define SEM_EOL 		((XINT)-1)
#define SEM_ATOMIC		((XINT)-200)
#define SEM_ALL 		((XINT)-201)
#define SEM_ANY 		((XINT)-202)

/* user blocking status */

#define SEM_USER_NOTWAITING	XIPC_USER_NOTWAITING
#define SEM_BLOCKEDATOMIC	SEM_ATOMIC
#define SEM_BLOCKEDALL		SEM_ALL
#define SEM_BLOCKEDANY		SEM_ANY

/* maximums, sizes and limits */

#define SEM_LEN_PATHNAME	XIPC_LEN_PATHNAME
#define SEM_LEN_USERNAME	XIPC_LEN_USERNAME
#define SEM_LEN_XIPCNAME	XIPC_LEN_XIPCNAME
#define SEM_LEN_PARMLINE	XIPC_LEN_PARMLINE
#define SEM_MAX_SIDLIST 	((XINT)32)
#define SEM_LEN_SIDLIST 	(SEM_MAX_SIDLIST+((XINT)1))
#define SEM_LEN_INFOLIST	((XINT)10)

/* semaphore types */

#define SEM_TYPE_RESOURCE	((XINT)0)
#define SEM_TYPE_EVENT		((XINT)1)

/* misc. option flags */

#define SEM_FREEZE		XIPC_FREEZE
#define SEM_CLEAR		XIPC_CLEAR
#define SEM_NOCLEAR 	XIPC_NOCLEAR
#define SEM_SET 		SEM_NOCLEAR
#define SEM_RETURN		XIPC_RETURN

#define SEM_NOOPTIONS		XIPC_NOOPTIONS

/* blocking options */

#define SEM_BLOCKOPT_WAIT		XIPC_BLOCKOPT_WAIT
#define SEM_BLOCKOPT_NOWAIT 	XIPC_BLOCKOPT_NOWAIT
#define SEM_BLOCKOPT_TIMEOUT	XIPC_BLOCKOPT_TIMEOUT
#define SEM_BLOCKOPT_SYNC		XIPC_BLOCKOPT_SYNC

#define SEM_BLOCKOPT_CALLBACK	XIPC_BLOCKOPT_CALLBACK
#define SEM_BLOCKOPT_POST		XIPC_BLOCKOPT_POST
#define SEM_BLOCKOPT_IGNORE 	XIPC_BLOCKOPT_IGNORE
#define SEM_BLOCKOPT_ASYNC		XIPC_BLOCKOPT_ASYNC

#define SEM_WAIT				XIPC_WAIT
#define SEM_NOWAIT				XIPC_NOWAIT
#define SEM_TIMEOUT(x)			XIPC_TIMEOUT(x)
#define SEM_CALLBACK(f,r)		XIPC_CALLBACK(f,r)
#define SEM_POST(s,r)			XIPC_POST(s,r)
#define SEM_IGNORE(r)			XIPC_IGNORE(r)

/* Freeze options */

#define SEM_MASKTRAPS			XIPC_MASKTRAPS
#define SEM_NOMASKTRAPS 		XIPC_NOMASKTRAPS

/* Info Options */

#define SEM_INFO_FIRST			XIPC_INFO_FIRST
#define SEM_INFO_NEXT(Id)		XIPC_INFO_NEXT(Id)		

/*********************************************************************
** data types
*********************************************************************/

typedef XINT SIDLIST[SEM_LEN_SIDLIST];
typedef XINT FAR *PSIDLIST;
typedef LBITS SEMVAL;

typedef struct _SEM_SEMWLISTITEM
{
	XINT Uid;
}
SEM_SEMWLISTITEM;

typedef struct _SEM_SEMHLISTITEM
{
	XINT Uid;
}
SEM_SEMHLISTITEM;

/*
 * The SEMINFOSEM structure is used for retreiving status
 * information about a particular SemSys semaphore.
 * SemInfoSem() fills the structure with the data
 * about the Sid it is passed.
 */
typedef struct _SEMINFOSEM
{
	XINT Sid;
	XINT CreateTime;		/* Time semaphore created */
	XINT CreateUid; 		/* The Uid who created it */
	XINT LastUid;			/* Last Uid to use it */
	XINT MaxValue;			/* Initial value */
	XINT CurValue;			/* Current value */
	LBITS SemType;			/* Resource or Event */
	XINT HListTotalLength;
	XINT HListOffset;
	XINT HListLength;
	XINT WListTotalLength;
	XINT WListOffset;
	XINT WListLength;
	SEM_SEMHLISTITEM HList[SEM_LEN_INFOLIST];
	SEM_SEMWLISTITEM WList[SEM_LEN_INFOLIST];
	CHAR Name[SEM_LEN_XIPCNAME + 1];	/* Semaphore name */
}
SEMINFOSEM;

typedef struct _SEM_USERQLISTITEM
{
	XINT Sid;
}
SEM_USERQLISTITEM;

typedef struct _SEM_USERWLISTITEM
{
	XINT Sid;
}
SEM_USERWLISTITEM;

typedef struct _SEM_USERHLISTITEM
{
	XINT Sid;
}
SEM_USERHLISTITEM;

/*
 * The SEMINFOUSER structure is used for retreiving status
 * information about a particular SemSys user.
 * SemInfoUser() fills the structure with the data
 * about the Uid it is passed.
 */
typedef struct _SEMINFOUSER
{
	XINT Uid;
	XINT Pid;				/* Process ID of user */
	XTID Tid;				/* Thread ID of user */
	XINT LoginTime; 		/* Time of login to SemSys */
	XINT TimeOut;			/* Remaining timeout secs */
	XINT WaitType;			/* ALL, ANY or ATOMIC */
	XINT HListTotalLength;
	XINT HListOffset;
	XINT HListLength;
	XINT WListTotalLength;
	XINT WListOffset;
	XINT WListLength;
	XINT QListTotalLength;
	XINT QListOffset;
	XINT QListLength;
	SEM_USERHLISTITEM HList[SEM_LEN_INFOLIST];
	SEM_USERWLISTITEM WList[SEM_LEN_INFOLIST];
	SEM_USERQLISTITEM QList[SEM_LEN_INFOLIST];
	CHAR Name[SEM_LEN_XIPCNAME + 1];	/* User login name*/
	CHAR NetLoc[XIPC_LEN_NETLOC + 1]; 	/* Name of client Node */
}
SEMINFOUSER;

/*
 * The SEMINFOSYS structure is used for retreiving status
 * information about the SemSys instance.
 * SemInfoSys() fills the structure with the data
 * about the instance.
 */
typedef struct _SEMINFOSYS
{
	XINT MaxUsers;			/* Maximum allowed users */
	XINT CurUsers;			/* Current number of users */
	XINT MaxSems;			/* Maximum allowed sems */
	XINT CurSems;			/* Current number of sems */
	XINT MaxNodes;			/* Max configured nodes */
	XINT FreeCnt;			/* Current available nodes */
	CHAR Name[SEM_LEN_PATHNAME + 1];	/* InstanceFileName */
}
SEMINFOSYS;

#endif /* SEMPUBD_H */

/*
** END ***************************************************** SEMPUBD.H
*/
