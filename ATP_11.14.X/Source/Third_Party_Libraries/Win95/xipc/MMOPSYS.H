/*********************************************************** MMOPSYS.H
**
**	Name:		MMOPSYS.H
**	----
**
**	Title:		Momentum Operating System Specifications
**	-----
**
**	Description:
**	-----------
**		This file contains the specifications of the various supported
**	   operating systems.
**
**	Last Revision:
**	-------------
**	$Revision:   1.0  $
**	$Date:   Oct 14 1999 08:17:36  $
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

#ifndef MMOPSYS_H
#define MMOPSYS_H MMOPSYS_H

/*********************************************************************
** operating system specifications
*********************************************************************/

#define XIPC_OPSYS_ALL			(0xFFFFFFFF)

#define XIPC_OPSYS_UNIX 		(0x0001)
#define XIPC_OSNAME_UNIX		"UNIX"

#define XIPC_OPSYS_VMS			(0x0002)
#define XIPC_OSNAME_VMS 		"VMS"

#define XIPC_OPSYS_OS2			(0x0004)
#define XIPC_OSNAME_OS2 		"OS/2"

#define XIPC_OPSYS_DOS			(0x0008)
#define XIPC_OSNAME_DOS 		"DOS"

#define XIPC_OPSYS_WINDOWS		(0x0010)
#define XIPC_OSNAME_WINDOWS 	"MS-Windows"

#define XIPC_OPSYS_WIN32		(0x0020)
#define XIPC_OSNAME_WIN32		"Win32"

#define XIPC_OPSYS_NEXT 		(0x0040)
#define XIPC_OSNAME_NEXT		"NeXTSTEP"

#define XIPC_OPSYS_MACINTOSH	(0x0080)
#define XIPC_OSNAME_MACINTOSH	"Macintosh"

#define XIPC_OPSYS_OS400		(0x0100)
#define XIPC_OSNAME_OS400		"OS/400"

#define XIPC_OPSYS_GUARDIAN 	(0x0200)
#define XIPC_OSNAME_GUARDIAN	"Guardian"

#endif /* MMOPSYS_H */

/*
** END ***************************************************** MMOPSYS.H
*/
