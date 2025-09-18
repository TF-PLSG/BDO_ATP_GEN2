/*********************************************************** SEMPUBP.H
**										   (SemSys - Semaphore System)
**
**	Name:	SEMPUBP.H
**	----
**
**	Title:	SemSys Public Header File - Function Prototypes
**	-----
**
**	Description:
**	-----------
**	This file contains the public function prototypes for the
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

#ifndef SEMPUBP_H
#define SEMPUBP_H SEMPUBP_H

/*********************************************************************
** functions
*********************************************************************/

#ifdef	C_PLUSPLUS
extern	"C" {
#endif

XINT		GLOBAL XAPI	SemAbortAsync C_PROTOARGS((XINT));
XINT		GLOBAL XAPI	SemAccess C_PROTOARGS((CHAR FAR *));
XINT		GLOBAL XAPI	SemAcquire P7(
						XINT,  	WaitType,
						PSIDLIST, SidList,
						XINT FAR *, SidPtr,
						XINT,  	Option1,
						XINT,  	Option2,
						XANY FAR *,	Option3,
						XIPC_CALLBACK_FUNC FAR *, Option4);
XINT		GLOBAL XAPI	SemCancel C_PROTOARGS((PSIDLIST, XINT  FAR *));
XINT		GLOBAL XAPI	SemClear C_PROTOARGS((PSIDLIST, XINT FAR *));
XINT		GLOBAL XAPI	SemCreate C_PROTOARGS((CHAR FAR *, SEMVAL));
XINT		GLOBAL XAPI	SemDelete C_PROTOARGS((XINT));
XINT		GLOBAL XAPI	SemDestroy C_PROTOARGS((XINT));
XINT		GLOBAL XAPI	SemFreeze C_PROTOARGS((VOID));
XINT		GLOBAL XAPI	SemInfoSem C_PROTOARGS((XINT, SEMINFOSEM FAR *));
XINT		GLOBAL XAPI	SemInfoSys C_PROTOARGS((SEMINFOSYS FAR *));
XINT		GLOBAL XAPI	SemInfoUser C_PROTOARGS((XINT, SEMINFOUSER FAR *));
XINT		GLOBAL XAPI	SemRelease C_PROTOARGS((PSIDLIST, XINT  FAR *));
XINT		GLOBAL XAPI	SemSet C_PROTOARGS((PSIDLIST, XINT	   FAR *));
XINT		GLOBAL XAPI	SemUnfreeze C_PROTOARGS((VOID));
XINT		GLOBAL XAPI	SemWait P7(
						XINT,  WaitType,
						PSIDLIST, SidList,
						XINT FAR *, SidPtr,
						XINT,  Option1,
						XINT,  Option2,
						XANY FAR *, Option3,
						XIPC_CALLBACK_FUNC FAR *, Option4);
PSIDLIST	FAR XCDECL	SemList C_PROTOARGS((XINT,...));
PSIDLIST	FAR XCDECL	SemListAdd C_PROTOARGS((PSIDLIST, ...));
PSIDLIST	FAR XCDECL	SemListRemove C_PROTOARGS((PSIDLIST, ...));
PSIDLIST	FAR XCDECL	SemListBuild C_PROTOARGS((PSIDLIST, ...));
XINT		GLOBAL XAPI	SemListCount C_PROTOARGS((PSIDLIST));

#ifdef	C_PLUSPLUS
}
#endif

#endif /* SEMPUBP_H */

/*
** END ***************************************************** SEMPUBP.H
*/
