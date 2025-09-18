/*********************************************************** MEMPUBP.H
**											  (MemSys - Memory System)
**
**	Name:	MEMPUBP.H
**	----
**
**	Title:	MemSys Public Header File - Function Prototypes
**	-----
**
**	Description:
**	-----------
**	This file contains the public function prototypes for the
**	Memory System.
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
**	241d  5-1-93	YF	 Define function MemSection for DOS to overcome the 
**						 problem of the structure that is left on the stack.
**
**
**	Copyright (C) Momentum Software Corporation, 1989
**
*********************************************************************/

#ifndef MEMPUBP_H
#define MEMPUBP_H MEMPUBP_H

/*********************************************************************
** functions
*********************************************************************/

#ifdef	C_PLUSPLUS
extern	"C" {
#endif

XINT	  GLOBAL XAPI MemAbortAsync C_PROTOARGS((XINT));
XINT	  GLOBAL XAPI MemAccess C_PROTOARGS((CHAR FAR *));
XINT	  GLOBAL XAPI MemCreate C_PROTOARGS((CHAR FAR *, XINT));
XINT	  GLOBAL XAPI MemDelete C_PROTOARGS((XINT));
XINT	  GLOBAL XAPI MemDestroy C_PROTOARGS((XINT));
XINT      GLOBAL XAPI MemFreeze C_PROTOARGS((VOID));
XINT	  GLOBAL XAPI MemInfoMem C_PROTOARGS((XINT, MEMINFOMEM FAR *));
XINT	  GLOBAL XAPI MemInfoSec C_PROTOARGS((SECTION, MEMINFOSEC FAR *));
XINT	  GLOBAL XAPI MemInfoSys C_PROTOARGS((MEMINFOSYS FAR *));
XINT	  GLOBAL XAPI MemInfoUser C_PROTOARGS((XINT, MEMINFOUSER FAR *));
XINT	  GLOBAL XAPI MemLock P7(
					XINT, LockType,
					PMIDLIST, MidList,
					SECTION FAR *, SecPtrArg,
					XINT,  Option1,
					XINT,  Option2,
					XANY FAR *, Option3,
					XIPC_CALLBACK_FUNC FAR *, Option4);
XINT	  GLOBAL XAPI MemPointer C_PROTOARGS((XINT, XANY FAR * FAR *));
XINT	  GLOBAL XAPI MemRead C_PROTOARGS((XINT, XINT, XINT, XANY FAR *, XINT, XINT, XANY FAR *, XIPC_CALLBACK_FUNC *));
XINT	  GLOBAL XAPI MemSecDef C_PROTOARGS((SECTION));
XINT	  GLOBAL XAPI MemSecOwn C_PROTOARGS((XINT, PMIDLIST, PSECTION, XINT, XINT, XANY FAR *, XIPC_CALLBACK_FUNC *));
XINT	  GLOBAL XAPI MemSecRel C_PROTOARGS((PMIDLIST, PSECTION));
XINT	  GLOBAL XAPI MemSecPriv C_PROTOARGS((SECTION, XINT, XINT));
XINT	  GLOBAL XAPI MemSecUndef C_PROTOARGS((SECTION));
SECTION   GLOBAL XAPI MemSection C_PROTOARGS((XINT, XINT, XINT));
PSECTION  GLOBAL XAPI MemSectionBuild C_PROTOARGS((PSECTION, XINT, XINT, XINT));
XINT	  GLOBAL XAPI MemTrigger C_PROTOARGS((XINT, XINT, XINT, XINT, XINT, XINT));
XINT      GLOBAL XAPI MemUnfreeze C_PROTOARGS((VOID));
XINT	  GLOBAL XAPI MemUntrigger C_PROTOARGS((XINT, XINT, XINT, XINT, XINT, XINT));
XINT	  GLOBAL XAPI MemUnlock C_PROTOARGS((PMIDLIST, PSECTION));
XINT	  GLOBAL XAPI MemWrite C_PROTOARGS((XINT, XINT, XINT, XANY FAR *, XINT, XINT, XANY FAR *, XIPC_CALLBACK_FUNC *));

PMIDLIST  FAR XCDECL  MemList C_PROTOARGS((SECTION,...));
PMIDLIST  FAR XCDECL  MemListAdd C_PROTOARGS((PMIDLIST, ...));
PMIDLIST  FAR XCDECL  MemListRemove C_PROTOARGS((PMIDLIST, ...));
PMIDLIST  FAR XCDECL  MemListBuild C_PROTOARGS((PMIDLIST, ...));
XINT	  GLOBAL XAPI MemListCount C_PROTOARGS((PMIDLIST));

#ifdef	C_PLUSPLUS
}
#endif

#endif /* MEMPUBP_H */

/*
** END ***************************************************** MEMPUBP.H
*/
