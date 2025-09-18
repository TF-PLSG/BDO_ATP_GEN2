/********************************************************** XIPCDEMO.H
**			 (XIPC - Extended Interprocess Communications Faclilities)
**
**	Name:		XIPCDEMO.H (WINNT)
**	----
**
**	Title:		
**
**	Description:
**	-----------
**	This file contains extended standard definitions used by
**  the sample programs distributed with the XIPC release. 
**
**  IMPORTANT NOTE: XIPC USERS SHOULD NOT USE THE CONTENTS OF THIS FILE .
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

#define YES			1
#define NO			0
#define TRUE		1
#define FALSE		0
#define ON			1
#define OFF			0

#define XIPC_SIG_ATTN			0
#define XIPC_SIG_QUIT			1
#define XIPC_SIG_TERM			2

#define XIPC_SIGA_KILL			0
#define XIPC_SIGA_IGNORE		1
#define XIPC_SIGA_ACCEPT		2
#define XIPC_SIGA_ERROR 		3
#define XIPC_SIGA_ACKNOWLEDGE	4

#define XIPCTRAP(Trap) \
VOID APIENTRY			\
Trap(SigNum)		   \
INT SigNum;

#define XIPCTRAPFUNC(Trap) \
VOID APIENTRY			\
Trap()

/*
** END **************************************************** XIPCDEMO.H
*/
