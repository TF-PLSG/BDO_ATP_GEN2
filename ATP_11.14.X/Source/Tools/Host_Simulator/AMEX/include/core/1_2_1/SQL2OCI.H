
/*
 * $Header: sql2oci.h 05-dec-96.14:42:32 lvbcheng Exp $
 */

/* Copyright (c) Oracle Corporation 1996, 1997. All Rights Reserved. */ 
 
/* NOTE:  See 'header_template.doc' in the 'doc' dve under the 'forms' 
      directory for the header file template that includes instructions. 
*/
 
/* 
   NAME 
     sql2oci.h - SQLLIB functions for V8 OCI interoperability

   DESCRIPTION 
     This file contains SQLLIB functions to obtain OCI environment handle
     and OCI service context in a Pro*c program.

   RELATED DOCUMENTS 
 
   INSPECTION STATUS 
     Inspection date: 
     Inspection status: 
     Estimated increasing cost defects per page: 
     Rule sets: 
 
   ACCEPTANCE REVIEW STATUS 
     Review date: 
     Review status: 
     Reviewers: 
 
   PUBLIC FUNCTION(S) 
     SQLEnvGet()
     SQLSvcCtxGet()

   PRIVATE FUNCTION(S)
     NONE

   EXAMPLES

   NOTES
     <other useful comments, qualifications, etc.>

   MODIFIED   (MM/DD/YY)
   lvbcheng    05/13/97 - Desupport wrappers
   lvbcheng    01/13/97 - fix non-ANSI SQLNumberFromFloat error
   lvbcheng    01/03/97 - test
   lvbcheng    01/03/97 - Revert back to OCINumber *
   lvbcheng    12/12/96 - Finalize
   lvbcheng    12/06/96 - test
   lvbcheng    12/06/96 - test
   lvbcheng    12/05/96 - Add default context code
   lvbcheng    12/05/96 - Add connection to sqlcpr.h and sql[a|k]pr.h
   lvbcheng    12/05/96 - Remove merge conflicts
   lvbcheng    12/02/96 - Change Interface of sql2oci wrappers
   lvbcheng    10/21/96 - Add OCINumber set procedures
   lvbcheng    10/21/96 - Add vstr wrappers
   jbasu       08/02/96 - Remove private include files
   jbasu       07/05/96 - [8.0] SQLSvcCtxGet() enhancements
   jbasu       06/04/96 - Support for V80 objects
   jbasu       06/04/96 - Creation

*/

#ifndef SQL2OCI_ORACLE
#  define SQL2OCI_ORACLE

#ifndef OCI_ORACLE
#  include <oci.h>
#endif /* OCI_ORACLE */

#ifndef ORL_ORACLE
#  include <orl.h>
#endif /* ORL_ORACLE */

#ifndef SQLCPR_ORACLE
#  include <sqlcpr.h>
#endif /* SQLCPR_ORACLE */

#ifndef __STDC__

/* -------------------- */
/*  Includes for K&R C  */
/* -------------------- */

#ifndef SQLKPR_ORACLE
#  include <sqlkpr.h>
#endif /* SQLKPR_ORACLE */

/* ------------------------------------------- */
/*  Get 8.0 OCI environment handle from SQLLIB */
/* ------------------------------------------- */

sword SQLEnvGet(/*_ dvoid *rctx, OCIEnv **oeh _*/);
/*
   NAME: SQLEnvGet - SQLLIB Get Environment from the Runtime Context

   PARAMETERS:
   rctx (IN): pointer to a sqllib runtime context
   oeh (IN/OUT): OCIEnv ptr
   
   DESCRIPTION:
   Sets oeh to the OCIEnv corresponding to the runtime context.

   RETURNS:
   status - SQL_SUCCESS on success, SQL_ERROR on failure.

   */

/* ---------------------------------------------- */

/* Get 8.0 OCI Service Context for a database connection in Pro*C */

/* non-threaded, no Pro*C runtime context */
sword SQLSvcCtxGet(/*_ dvoid *rctx, text *dbname, sb4 dbnamelen,
		     OCISvcCtx **svc _*/);
/*
   NAME: SQLSvcCtxGet - SQLLIB Service Context Get

   PARAMETERS:
   rctx (IN): pointer to a sqllib runtime context
   dbname (IN): buffer containing the "logical" name for this connection.
   dbnamelen (IN): length of the dbname buffer
   svc (IN/OUT): address of an OCISvcCtx ptr
   
   DESCRIPTION:
   Sets svc to the OCI Service Context corresponding to the runtime context.

   RETURNS:
   status - SQL_SUCCESS on success, SQL_ERROR on failure.

   NOTES:
   'dbname' is the buffer containing the "logical" name for this connection.
   This  would be the same identifier used in an AT clause in an embedded
   SQL statement.  
   
   'dbnamelen' is the length of buffer containing dbname.
   
   If dbname is a NULL ptr or dbnamelen is 0, then the "default" database
   connection is assumed (as in a SQL statement with no AT clause).
   
   A value of -1 for dbnamelen can be used to indicate that dbname 
   is a NULL-terminated string.
   
   */

#else

/* --------------------- */
/*  Includes for ANSI C  */
/* --------------------- */

#ifndef SQLAPR_ORACLE
#  include <sqlapr.h>
#endif /* SQLAPR_ORACLE */

/* Get Procedures for OCI Env and Svc Ctx */

extern sword SQLEnvGet(dvoid *rctx, OCIEnv **oeh);

extern sword SQLSvcCtxGet(dvoid *rctx, text *dbname, sb4 dbnamelen, 
			  OCISvcCtx **svc);

#endif /* __STDC__ */

#endif /* SQL2OCI_ORACLE */
