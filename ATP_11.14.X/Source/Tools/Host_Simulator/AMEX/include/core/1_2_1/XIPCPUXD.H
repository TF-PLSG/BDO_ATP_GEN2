/********************************************************** XIPCPUXD.H
**			 (XIPC - Extended Interprocess Communications Faclilities)
**
**	Name:		XIPCPUXD.H
**	----
**
**	Title:		System Dependent Header File used internally.
**
**	Description:
**	-----------
**	This file contains extended standard definitions(used by xipc internally)
**  some MACROS are used by the sample programs distributed with the release. 
**  This file might not be distributed with the future releases. 
**
**	Last Revision:
**	-------------
**	$Revision:	$
**	$Date:		$
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

#ifndef XIPCPUXD_H
#define XIPCPUXD_H XIPCPUXD_H

/*********************************************************************
** macros
*********************************************************************/

/* wait options */

/* signals ranges and numbers */

#define XIPC_SIG_LO 			1
#define XIPC_SIG_HI 			NSIG

#define XIPC_SIG_ATTN			0
#define XIPC_SIG_QUIT			1
#define XIPC_SIG_TERM			2
#define XIPC_SIG_USER			3
#define XIPC_SIG_PIPE			4
#define XIPC_SIG_URGENT 		5
#define XIPC_SIG_TIMER			6

/* signal handler actions */

#define XIPC_SIGA_KILL			0
#define XIPC_SIGA_IGNORE		1
#define XIPC_SIGA_ACCEPT		2
#define XIPC_SIGA_ERROR 		3
#define XIPC_SIGA_ACKNOWLEDGE	4

/* return codes */

/* file permissions */

#define XIPC_PERM_RW			0666

/*********************************************************************
** macros
*********************************************************************/

#define XIPCSIGFUNC_PROTO(Trap) \
VOID GLOBAL \
Trap(INT)

#define XIPCSIGFUNC_DEFINE(Trap) \
VOID GLOBAL \
Trap(XINT SigNum) \

#define XIPC_NET_ABORT_OPERATION	{}

/*********************************************************************
** Prototypes
*********************************************************************/

XINT		GLOBAL XAPI XipcAsyncEventObject(void);

#endif /* XIPCPUXD_H */

/*
** END **************************************************** XIPCPUXD.H
*/
