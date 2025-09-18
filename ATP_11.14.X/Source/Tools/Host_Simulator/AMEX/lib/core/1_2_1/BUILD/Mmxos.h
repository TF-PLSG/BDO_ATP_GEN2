/************************************************************* MMXOS.H
**
**	Name:		MMXOS.H (WINNT)
**	----
**
**	Title:		Momentum Operating-System Dependent Header File
**	-----
**
**	Description:
**	-----------
**		This file contains operating-system definition, and
**		any dependent '#include' statements for use in all
**		Momentum Software Corporation programs.
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

#ifndef MMXOS_H
#define MMXOS_H MMXOS_H

/*	Common Header File	*/

#include <windows.h>

#ifdef __cplusplus
#define C_PLUSPLUS
#endif

/* ANSI-C function prototype specification */
#define ANSI_PROTOTYPES 	1
#define C_PROTOARGS(args)	args

/*********************************************************************
** macros
*********************************************************************/

#ifndef WINNT
#define WINNT	1
#endif

#ifndef WIN32
#define WIN32	1
#endif

#define XCDECL		__cdecl
#define XAPI		__stdcall
#define GLOBAL
#define GLOBALDATA

#define GLOBALDEF
#define GLOBALREF		extern

/*
 *	XIPC_DLL is defined for all modules that are compiled as part of
 *	an XIPC DLL.
 */
#ifdef XIPC_DLL
#define DATAIMP
#else
#define DATAIMP __declspec(dllimport)
#endif

#if defined(XIPC_DLL) && !defined(XIPCNET_DLL)
#define GLOBALIMP
#else
#define GLOBALIMP __declspec(dllimport)
#endif

#ifdef	VOID
#undef	VOID
#endif

#define VOID void

#define MEDINIT

/*********************************************************************
** data types
*********************************************************************/

#ifndef ANY
#define ANY		void
#endif

typedef void	XANY;

typedef  int	XINT;

typedef  int	XTID;
typedef  int	XPID;
typedef  char	CHAR;
typedef  int	INT;
typedef unsigned int	UXINT;
typedef unsigned long	LBITS;
typedef unsigned int	BITS;
typedef unsigned int	FLAG;

typedef VOID (XIPCSIGHANDLER) C_PROTOARGS((XINT));


#endif /* MMXOS_H */

/*
** END ******************************************************* MMXOS.H
*/
