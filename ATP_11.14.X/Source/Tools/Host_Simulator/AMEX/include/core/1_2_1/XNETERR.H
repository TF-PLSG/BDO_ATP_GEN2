/************************************************************ XNETERR.H
**												(XIPC - XIPC )
**
**	Name:		XNETERR.H
**	----
**
**	Title:		XIPC Network Error Codes
**	-----
**
**	Description:
**	-----------
**		This file contains the user definitions for the
**		error codes .
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

#ifndef XNETERR_H
#define XNETERR_H XNETERR_H

#define NET_ER_BASECODE 			  (XINT)(-2200)
#define XIPCNET_ER_NOTREGISTERED	  NET_ER_BASECODE
#define XIPCNET_ER_NOTFOUND 		  (NET_ER_BASECODE-(XINT)1)
#define XIPCNET_ER_NOSEARCH 		  (NET_ER_BASECODE-(XINT)2)
#define XIPCNET_ER_DUPINSTANCE		  (NET_ER_BASECODE-(XINT)3)
#define XIPCNET_ER_DUPCONFIGPATH	  (NET_ER_BASECODE-(XINT)4)
#define XIPCNET_ER_NOFILE			  (NET_ER_BASECODE-(XINT)5)
#define XIPCNET_ER_NOFORK			  (NET_ER_BASECODE-(XINT)6)
#define XIPCNET_ER_SYSERR			  (NET_ER_BASECODE-(XINT)7)
#define XIPCNET_ER_NETERR			  (NET_ER_BASECODE-(XINT)8)
#define XIPCNET_ER_SESSION			  (NET_ER_BASECODE-(XINT)9)
#define XIPCNET_ER_TOOBIG			  (NET_ER_BASECODE-(XINT)10)
#define XIPCNET_ER_CONNECTLOST		  (NET_ER_BASECODE-(XINT)11)
#define XIPCNET_ER_BADNETNAME		  (NET_ER_BASECODE-(XINT)12)
#define XIPCNET_ER_BADMESSAGE		  (NET_ER_BASECODE-(XINT)13)
#define XIPCNET_ER_WRONGVERSION 	  (NET_ER_BASECODE-(XINT)14)
#define XIPCNET_ER_SPAWNED			  (NET_ER_BASECODE-(XINT)15)
#define XIPCNET_ER_CANNOT_SPAWN 	  (NET_ER_BASECODE-(XINT)16)
#define XIPCNET_ER_NOTLOCAL 		  (NET_ER_BASECODE-(XINT)17)
#define XIPCNET_ER_HOSTUNREACHABLE	  (NET_ER_BASECODE-(XINT)18)
#define XIPCNET_ER_BADHOST			  (NET_ER_BASECODE-(XINT)19)
#define XIPCNET_ER_PORTUNAVAILABLE	  (NET_ER_BASECODE-(XINT)20)

#endif /* XNETERR_H */

/*
** END ****************************************************** XNETERR.H
*/
