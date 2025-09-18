/************************************************************** XIPC.H
**			 (XIPC - Extended Interprocess Communications Faclilities)
**
**	Name:		XIPC.H
**	----
**
**	Title:		XIPC User Header File
**	-----
**
**	Description:
**	-----------
**		This file contains the header files for XIPC.
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

#ifndef XIPC_H
#define XIPC_H XIPC_H

/*********************************************************************
** header files
*********************************************************************/

/*
 *	operating system specific include files
 */
#include "mmxos.h"
#include "mmopsys.h"

/*
 *	Xipc General Include files
 */
#include "xipcpubd.h"
#include "xipcpubp.h"
#include "xipcpuxd.h"
#include "xipcerr.h"
#include "xneterr.h"

/*
 *	SemSys Include Files
 */
#include "sempubd.h"
#include "sempubp.h"
#include "semerr.h"

/*
 *	QueSys Include Files
 */
#include "quepubd.h"
#include "quepubp.h"
#include "queerr.h"

/*
 *	MemSys Include Files
 */
#include "mempubd.h"
#include "mempubp.h"
#include "memerr.h"
#include "memdata.h"

/*
 *	MomSys Include Files
 */
#include "mompubd.h"
#include "momerr.h"

/*
 *	Xipc Async Include File
 */
#include "xipcasyn.h"


/*
 *	Xipc General Include Files
 */
#include "mmxosoff.h"

#endif /* XIPC_H */

/*
** END ******************************************************** XIPC.H
*/
