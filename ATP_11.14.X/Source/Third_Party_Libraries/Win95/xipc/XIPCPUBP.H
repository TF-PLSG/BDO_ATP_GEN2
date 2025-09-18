/********************************************************** XIPCPUBP.H
**			 (XIPC - Extended Interprocess Communications Faclilities)
**
**	Name:	XIPCPUBP.H
**	----
**
**	Title:	XIPC Public Header File - Function Prototypes
**	-----
**
**	Description:
**	-----------
**	This file contains public function prototypes common
**	to all XIPC systems.
**
**	Last Revision:
**	-------------
**	$Revision:   1.0  $
**	$Date:   Oct 14 1999 08:17:50  $
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

#ifndef XIPCPUBP_H
#define XIPCPUBP_H XIPCPUBP_H

/*********************************************************************
** functions
*********************************************************************/

#ifdef	C_PLUSPLUS
extern	"C" {
#endif

XINT		GLOBAL XAPI XipcAbort C_PROTOARGS((XINT));
XIPC_BUF_SEGMENT FAR * GLOBAL XAPI XipcArgNonContig P1(XIPC_BUF_SEGMENT *, Buffer);
XINT		GLOBAL XAPI XipcAsyncIoDescriptor P0();
XINT 		GLOBAL XAPI XipcAsyncEventHandler P0();
XINT		GLOBAL XAPI XipcConnect C_PROTOARGS((CHAR FAR *, XINT));
XINT		GLOBAL XAPI XipcDisconnect P0();
CHAR FAR *	GLOBAL XAPI XipcError C_PROTOARGS((XINT));
XINT		GLOBAL XAPI XipcFreeze P0();
XINT        GLOBAL XAPI XipcGetOpt C_PROTOARGS((XINT, XINT, XINT, XANY FAR *, XANY FAR
					*, XANY FAR *, XANY FAR *, XANY FAR *, XINT (FAR *)()));
XINT		GLOBAL XAPI XipcIdleWatch C_PROTOARGS((XINT));
XINT		GLOBAL XAPI XipcInfoLogin C_PROTOARGS((XIPCINFOLOGIN FAR *, XINT, XINT FAR *));
XINT        GLOBAL XAPI XipcInfoSystemError C_PROTOARGS((XIPC_SYSERR FAR *));
XINT		GLOBAL XAPI XipcInfoUser P2(
					XINT, Uid,
					XIPCINFOUSER *, InfoUser);
XINT		GLOBAL XAPI XipcInfoVersion P1(XIPCINFOVERSION *, RetInfoVersion);
XINT		GLOBAL XAPI XipcInit P0();
XINT		GLOBAL XAPI XipcIsThreaded P0();
XINT		GLOBAL XAPI XipcListStart 
			C_PROTOARGS((XANY FAR * FAR *HandlePtr, CHAR FAR *HostName));
XINT		GLOBAL XAPI XipcListNext C_PROTOARGS((XANY FAR *Handle,
							XIPCINFOINSTANCE FAR *InstInfoPtr));
XINT		GLOBAL XAPI XipcListEnd C_PROTOARGS((XANY FAR *Handle));
XINT		GLOBAL XAPI XipcLogin C_PROTOARGS((CHAR FAR *, CHAR FAR *));
XINT		GLOBAL XAPI XipcLoginEx C_PROTOARGS((CHAR FAR *, CHAR FAR *, XINT));
XINT		GLOBAL XAPI XipcLogout P0();
XINT		GLOBAL XAPI XipcMaskTraps P0();
XINT        GLOBAL XAPI XipcPing C_PROTOARGS((CHAR FAR *, XINT, XINT));
XINT        GLOBAL XAPI XipcSetOpt C_PROTOARGS((XINT, XINT, XINT, XINT, XINT, XANY FAR
					 *, XANY FAR *, XINT (FAR *)()));
XINT		GLOBAL XAPI XipcTerm P0();
XINT		GLOBAL XAPI XipcTrap C_PROTOARGS((INT, XIPCSIGHANDLER *));
VOID		GLOBAL XAPI XipcSignal C_PROTOARGS((XINT, XIPCSIGHANDLER *, XINT));
VOID		GLOBAL XAPI XipcSleep C_PROTOARGS((XINT));
XINT		GLOBAL XAPI XipcStart C_PROTOARGS((CHAR FAR *, CHAR FAR *, XINT));
XINT		GLOBAL XAPI XipcStop C_PROTOARGS((CHAR FAR *, XINT));
XINT        GLOBAL XAPI XipcSystemErrorReport C_PROTOARGS((CHAR FAR *, XINT));
XINT		GLOBAL XAPI XipcUnfreeze P0();
XINT		GLOBAL XAPI XipcUnmaskTraps P0();

#ifdef	C_PLUSPLUS
}
#endif

#define XipcLogin(Instance, User) XipcLoginEx(Instance, User, XIPC_BUILD_NUMBER)

#endif /* XIPCPUBP_H */

/*
** END **************************************************** XIPCPUBP.H
*/
