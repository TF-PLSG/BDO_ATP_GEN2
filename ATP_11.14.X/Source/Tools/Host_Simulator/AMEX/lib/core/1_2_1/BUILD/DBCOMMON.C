
/* Result Sets Interface */
#ifndef SQL_CRSR
#  define SQL_CRSR
  struct sql_cursor
  {
    unsigned int curocn;
    void *ptr1;
    void *ptr2;
    unsigned int magic;
  };
  typedef struct sql_cursor sql_cursor;
  typedef struct sql_cursor SQL_CURSOR;
#endif /* SQL_CRSR */

/* Thread Safety */
typedef void * sql_context;
typedef void * SQL_CONTEXT;

/* Object support */
struct sqltvn
{
  unsigned char *tvnvsn; 
  unsigned short tvnvsnl; 
  unsigned char *tvnnm;
  unsigned short tvnnml; 
  unsigned char *tvnsnm;
  unsigned short tvnsnml;
};
typedef struct sqltvn sqltvn;

struct sqladts
{
  unsigned int adtvsn; 
  unsigned short adtmode; 
  unsigned short adtnum;  
  sqltvn adttvn[1];       
};
typedef struct sqladts sqladts;

static struct sqladts sqladt = {
  1,1,0,
};

/* Binding to PL/SQL records */
struct sqltdss
{
  unsigned int tdsvsn; 
  unsigned short tdsnum; 
  unsigned char *tdsval[1]; 
};
typedef struct sqltdss sqltdss;
static struct sqltdss sqltds =
{
  1,
  0,
};

/* File name & Package Name */
struct sqlcxp
{
  unsigned short fillen;
           char  filnam[14];
};
static const struct sqlcxp sqlfpn =
{
    13,
    ".\\dbcommon.pc"
};


static const unsigned int sqlctx = 909094490;


static struct sqlexd {
   unsigned long   sqlvsn;
   unsigned long   arrsiz;
   unsigned long   iters;
   unsigned long   offset;
   unsigned short  selerr;
   unsigned short  sqlety;
   unsigned long   unused;
   const    short  *cud;
   unsigned char  *sqlest;
   const    char  *stmt;
	    sqladts  *sqladtp;
	    sqltdss  *sqltdsp;
   void  * *sqphsv;
   unsigned long  *sqphsl;
	    long  *sqphss;
	    void * *sqpind;
	    long  *sqpins;
   unsigned long  *sqparm;
   unsigned long  **sqparc;
   unsigned short  *sqpadto;
   unsigned short  *sqptdso;
	    void  *sqhstv[8];
   unsigned long  sqhstl[8];
	    long  sqhsts[8];
	    void  *sqindv[8];
	    long  sqinds[8];
   unsigned long  sqharm[8];
   unsigned long   *sqharc[8];
   unsigned short sqadto[8];
   unsigned short sqtdso[8];
} sqlstm = {10,8};

/* Prototypes */
extern void sqlcxt (void **, const unsigned int *,
		    struct sqlexd *, const struct sqlcxp *);
extern void sqlcx2t(void **, const unsigned int ,
		    struct sqlexd *, const struct sqlcxp *);
extern void sqlbuft(void **, char *);
extern void sqlgs2t(void **, char *);
extern void sqlorat(void **, const unsigned int *, void *);

/* Forms Interface */
static const int IAPSUCC = 0;
static const int IAPFAIL = 1403;
static const int IAPFTL  = 535;
extern void sqliem(char *, int *);

 static const char *sq0011 = 
"select timer_id ,app_data1 ,app_data2 ,reply_que ,orig_system ,timeout ,inte\
rval  from timer where ds_name=:b0 order by timeout            ";
typedef struct { unsigned short len; unsigned char arr[1]; } VARCHAR;
typedef struct { unsigned short len; unsigned char arr[1]; } varchar;

/* cud (compilation unit data) array */
static const short sqlcud0[] =
{10,4130,0,0,0,
5,0,0,1,0,0,283,147,0,0,4,4,0,1,0,1,97,0,0,1,97,0,0,1,97,0,0,1,10,0,0,
36,0,0,2,0,0,286,163,0,0,0,0,0,1,0,
51,0,0,3,0,0,285,177,0,0,0,0,0,1,0,
66,0,0,4,0,0,287,186,0,0,0,0,0,1,0,
81,0,0,5,72,0,260,222,0,0,2,1,0,1,0,2,9,0,0,1,9,0,0,
104,0,0,6,70,0,259,234,0,0,2,2,0,1,0,1,9,0,0,1,9,0,0,
127,0,0,7,66,0,261,265,0,0,2,2,0,1,0,1,9,0,0,1,9,0,0,
150,0,0,8,48,0,258,285,0,0,1,1,0,1,0,1,9,0,0,
169,0,0,9,135,0,259,341,0,0,8,8,0,1,0,1,9,0,0,1,9,0,0,1,9,0,0,1,9,0,0,1,9,0,0,
1,9,0,0,1,3,0,0,1,3,0,0,
216,0,0,10,56,0,258,381,0,0,2,2,0,1,0,1,9,0,0,1,9,0,0,
239,0,0,11,139,0,265,422,0,0,1,1,0,1,0,1,9,0,0,
258,0,0,11,0,0,269,430,0,0,7,0,0,1,0,2,9,0,0,2,9,0,0,2,9,0,0,2,9,0,0,2,9,0,0,2,
3,0,0,2,3,0,0,
301,0,0,11,0,0,271,450,0,0,0,0,0,1,0,
};


/******************************************************************************
  
        Module: dbcommon.pc
  
         Title: Common Dataserver DB Routines
  
   Description: 

   $Log:   N:\PVCS\PTE\CORE\ptedb_lib\dbcommon.pc  $
   
      Rev 1.3   Aug 07 1998 10:23:18   drodden
   Added the capability to store user data with the timer.  This data
   is then returned in the read/clear timer replies.
   
   
      Rev 1.2   Jun 01 1998 07:59:50   drodden
   Changed the timer table back to the old column format.  Oracle 7.3
   is too slow with raw (longraw) columns.
   
   
      Rev 1.1   May 19 1998 09:28:46   drodden
   Changed the dataserver timer routines to NOT send a timeout if the 
   timer request did originate on this system.  This is for high availability
   systems. 
   
   
      Rev 1.0   Apr 23 1998 10:57:58   drodden
   Initial code for the Core Dataserver.
   

******************************************************************************/

/* Do not remove the following lines. These are the Custom build settings for the ProC pre-compiler */
/*proc $(ProjDir)\$(InputName).pc dbms=native sqlcheck=semantics userid=core/pinnacle@pinnacle*/
/*$(ProjDir)\$(InputName).c*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sqlproto.h>

#include "basictyp.h"
#include "pte.h"
#include "ptemsg.h"
#include "ptetimer.h"
#include "ntutils.h"
#include "dbcommon.h"


PRIVATE  CHAR  this_ds_name [MAX_APP_NAME_SIZE] = {'\0'};

/* Oracle error codes */
#define NULL_COLUMN_DATA -1405
#define NO_DATA_FOUND    1403


/* EXEC SQL INCLUDE sqlca;
 */ 
/*
 * $Header: sqlca.h,v 1.3 1994/12/12 19:27:27 jbasu Exp $ sqlca.h 
 */

/* Copyright (c) 1985,1986 by Oracle Corporation. */
 
/*
NAME
  SQLCA : SQL Communications Area.
FUNCTION
  Contains no code. Oracle fills in the SQLCA with status info
  during the execution of a SQL stmt.
NOTES
  **************************************************************
  ***                                                        ***
  *** This file is SOSD.  Porters must change the data types ***
  *** appropriately on their platform.  See notes/pcport.doc ***
  *** for more information.                                  ***
  ***                                                        ***
  **************************************************************

  If the symbol SQLCA_STORAGE_CLASS is defined, then the SQLCA
  will be defined to have this storage class. For example:
 
    #define SQLCA_STORAGE_CLASS extern
 
  will define the SQLCA as an extern.
 
  If the symbol SQLCA_INIT is defined, then the SQLCA will be
  statically initialized. Although this is not necessary in order
  to use the SQLCA, it is a good pgming practice not to have
  unitialized variables. However, some C compilers/OS's don't
  allow automatic variables to be init'd in this manner. Therefore,
  if you are INCLUDE'ing the SQLCA in a place where it would be
  an automatic AND your C compiler/OS doesn't allow this style
  of initialization, then SQLCA_INIT should be left undefined --
  all others can define SQLCA_INIT if they wish.

  If the symbol SQLCA_NONE is defined, then the SQLCA variable will
  not be defined at all.  The symbol SQLCA_NONE should not be defined
  in source modules that have embedded SQL.  However, source modules
  that have no embedded SQL, but need to manipulate a sqlca struct
  passed in as a parameter, can set the SQLCA_NONE symbol to avoid
  creation of an extraneous sqlca variable.
 
MODIFIED
    jbasu      12/12/94 -  Bug 217878: note this is an SOSD file
    losborne   08/11/92 -  No sqlca var if SQLCA_NONE macro set 
  Clare      12/06/84 - Ch SQLCA to not be an extern.
  Clare      10/21/85 - Add initialization.
  Bradbury   01/05/86 - Only initialize when SQLCA_INIT set
  Clare      06/12/86 - Add SQLCA_STORAGE_CLASS option.
*/
 
#ifndef SQLCA
#define SQLCA 1
 
struct   sqlca
         {
         /* ub1 */ char    sqlcaid[8];
         /* b4  */ long    sqlabc;
         /* b4  */ long    sqlcode;
         struct
           {
           /* ub2 */ unsigned short sqlerrml;
           /* ub1 */ char           sqlerrmc[70];
           } sqlerrm;
         /* ub1 */ char    sqlerrp[8];
         /* b4  */ long    sqlerrd[6];
         /* ub1 */ char    sqlwarn[8];
         /* ub1 */ char    sqlext[8];
         };

#ifndef SQLCA_NONE 
#ifdef   SQLCA_STORAGE_CLASS
SQLCA_STORAGE_CLASS struct sqlca sqlca
#else
         struct sqlca sqlca
#endif
 
#ifdef  SQLCA_INIT
         = {
         {'S', 'Q', 'L', 'C', 'A', ' ', ' ', ' '},
         sizeof(struct sqlca),
         0,
         { 0, {0}},
         {'N', 'O', 'T', ' ', 'S', 'E', 'T', ' '},
         {0, 0, 0, 0, 0, 0},
         {0, 0, 0, 0, 0, 0, 0, 0},
         {0, 0, 0, 0, 0, 0, 0, 0}
         }
#endif
         ;
#endif
 
#endif
 
/* end SQLCA */



/* EXEC ORACLE OPTION (MAXOPENCURSORS=100); */ 

/* EXEC ORACLE OPTION (HOLD_CURSOR=YES); */ 

/* EXEC ORACLE OPTION (RELEASE_CURSOR=NO); */ 



/* EXEC SQL BEGIN DECLARE SECTION; */ 


   /* error code variable */
   long    SQLCODE;


   /* EXEC SQL TYPE RAW256  is VARRAW (256); */ 

   /* EXEC SQL TYPE RAW512  is VARRAW (512); */ 

   /* EXEC SQL TYPE RAW768  is VARRAW (768); */ 

   /* EXEC SQL TYPE RAW1024 is VARRAW (1024); */ 

   /* EXEC SQL TYPE RAW1280 is VARRAW (1280); */ 

   /* EXEC SQL TYPE RAW1536 is VARRAW (1536); */ 



   /* checkpoint table variables */
   struct
   {
      /* varchar  checkpoint_id   [24]; */ 
struct { unsigned short len; unsigned char arr[24]; } checkpoint_id;

      /* varchar  checkpoint_trid [24]; */ 
struct { unsigned short len; unsigned char arr[24]; } checkpoint_trid;

   } db_checkpoint;


   /* timer variables */
   struct 
   {
      /* varchar  timer_id    [24]; */ 
struct { unsigned short len; unsigned char arr[24]; } timer_id;

      /* varchar  app_data1   [32]; */ 
struct { unsigned short len; unsigned char arr[32]; } app_data1;

      /* varchar  app_data2   [32]; */ 
struct { unsigned short len; unsigned char arr[32]; } app_data2;

      /* varchar  reply_que   [16]; */ 
struct { unsigned short len; unsigned char arr[16]; } reply_que;

      /* varchar  orig_system [16]; */ 
struct { unsigned short len; unsigned char arr[16]; } orig_system;

      /* varchar  ds_name     [16]; */ 
struct { unsigned short len; unsigned char arr[16]; } ds_name;

      int      timeout;
      int      interval;
   } db_timer;

/* EXEC SQL END DECLARE SECTION; */ 




/*************************************************************************************/
/* convert oracle return codes to  pte result codes                                  */
/*************************************************************************************/
BYTE dbcommon_oracle_to_pte_rcode ( LONG oracle_rcode, pCHAR ErrorMsg )
{
   switch (oracle_rcode)
   {
      case NO_DATA_FOUND:
         strcpy (ErrorMsg, "The Specified row was not found");
         return (PTEMSG_NOT_FOUND);

      default:
         sprintf(ErrorMsg, "% .70s", sqlca.sqlerrm.sqlerrmc);
         return (PTEMSG_DATABASE_ERROR);
   }
}



/*************************************************************************************/
/*************************************************************************************/
BYTE dbcommon_connect ( pCHAR username, 
                        pCHAR password,
                        pCHAR alias,    
                        pCHAR database,
                        pCHAR ErrorMsg )
{
   /* EXEC SQL BEGIN DECLARE SECTION; */ 

      /* login values */
      char  db_username[21];
      char  db_password[21];
      char  db_alias[21];
      char  db_database[21];
   /* EXEC SQL END DECLARE SECTION; */ 



   /* Connect to ORACLE. */
   strcpy( db_username, username);
   strcpy( db_password, password);
   strcpy( db_alias,    alias   );
   strcpy( db_database, database);
   
   /* EXEC SQL CONNECT :db_username IDENTIFIED BY :db_password  AT :db_alias using :db_database; */
   /* EXEC SQL CONNECT :db_username  IDENTIFIED BY :db_password  USING :db_alias; */ 

{
   struct sqlexd sqlstm;

   sqlstm.sqlvsn = 10;
   sqlstm.arrsiz = 4;
   sqlstm.sqladtp = &sqladt;
   sqlstm.sqltdsp = &sqltds;
   sqlstm.iters = (unsigned long )100;
   sqlstm.offset = (unsigned long )5;
   sqlstm.cud = sqlcud0;
   sqlstm.sqlest = (unsigned char  *)&sqlca;
   sqlstm.sqlety = (unsigned short)0;
   sqlstm.sqhstv[0] = (         void  *)db_username;
   sqlstm.sqhstl[0] = (unsigned long )21;
   sqlstm.sqhsts[0] = (         long )21;
   sqlstm.sqindv[0] = (         void  *)0;
   sqlstm.sqinds[0] = (         long )0;
   sqlstm.sqharm[0] = (unsigned long )0;
   sqlstm.sqadto[0] = (unsigned short )0;
   sqlstm.sqtdso[0] = (unsigned short )0;
   sqlstm.sqhstv[1] = (         void  *)db_password;
   sqlstm.sqhstl[1] = (unsigned long )21;
   sqlstm.sqhsts[1] = (         long )21;
   sqlstm.sqindv[1] = (         void  *)0;
   sqlstm.sqinds[1] = (         long )0;
   sqlstm.sqharm[1] = (unsigned long )0;
   sqlstm.sqadto[1] = (unsigned short )0;
   sqlstm.sqtdso[1] = (unsigned short )0;
   sqlstm.sqhstv[2] = (         void  *)db_alias;
   sqlstm.sqhstl[2] = (unsigned long )21;
   sqlstm.sqhsts[2] = (         long )21;
   sqlstm.sqindv[2] = (         void  *)0;
   sqlstm.sqinds[2] = (         long )0;
   sqlstm.sqharm[2] = (unsigned long )0;
   sqlstm.sqadto[2] = (unsigned short )0;
   sqlstm.sqtdso[2] = (unsigned short )0;
   sqlstm.sqphsv = sqlstm.sqhstv;
   sqlstm.sqphsl = sqlstm.sqhstl;
   sqlstm.sqphss = sqlstm.sqhsts;
   sqlstm.sqpind = sqlstm.sqindv;
   sqlstm.sqpins = sqlstm.sqinds;
   sqlstm.sqparm = sqlstm.sqharm;
   sqlstm.sqparc = sqlstm.sqharc;
   sqlstm.sqpadto = sqlstm.sqadto;
   sqlstm.sqptdso = sqlstm.sqtdso;
   sqlcxt((void **)0, &sqlctx, &sqlstm, &sqlfpn);
   SQLCODE = sqlca.sqlcode;
}


   
   if (SQLCODE != 0)
      return (dbcommon_oracle_to_pte_rcode (SQLCODE, ErrorMsg));

   return (PTEMSG_OK);
}



/*************************************************************************************/
/*************************************************************************************/
BYTE dbcommon_disconnect( pCHAR ErrorMsg )
{
   /* Disconnect from ORACLE. */
   
   /* EXEC SQL COMMIT WORK RELEASE; */ 

{
   struct sqlexd sqlstm;

   sqlstm.sqlvsn = 10;
   sqlstm.arrsiz = 4;
   sqlstm.sqladtp = &sqladt;
   sqlstm.sqltdsp = &sqltds;
   sqlstm.iters = (unsigned long )1;
   sqlstm.offset = (unsigned long )36;
   sqlstm.cud = sqlcud0;
   sqlstm.sqlest = (unsigned char  *)&sqlca;
   sqlstm.sqlety = (unsigned short)0;
   sqlcxt((void **)0, &sqlctx, &sqlstm, &sqlfpn);
   SQLCODE = sqlca.sqlcode;
}


   
   if (SQLCODE < 0)
      return (dbcommon_oracle_to_pte_rcode (SQLCODE, ErrorMsg));

   return PTEMSG_OK;
}



/*************************************************************************************/
/*************************************************************************************/
void dbcommon_commit ()
{
   /* EXEC SQL COMMIT WORK; */ 

{
   struct sqlexd sqlstm;

   sqlstm.sqlvsn = 10;
   sqlstm.arrsiz = 4;
   sqlstm.sqladtp = &sqladt;
   sqlstm.sqltdsp = &sqltds;
   sqlstm.iters = (unsigned long )1;
   sqlstm.offset = (unsigned long )51;
   sqlstm.cud = sqlcud0;
   sqlstm.sqlest = (unsigned char  *)&sqlca;
   sqlstm.sqlety = (unsigned short)0;
   sqlcxt((void **)0, &sqlctx, &sqlstm, &sqlfpn);
   SQLCODE = sqlca.sqlcode;
}


}



/*************************************************************************************/
/*************************************************************************************/
void dbcommon_rollback ()
{
   /* EXEC SQL ROLLBACK WORK; */ 

{
   struct sqlexd sqlstm;

   sqlstm.sqlvsn = 10;
   sqlstm.arrsiz = 4;
   sqlstm.sqladtp = &sqladt;
   sqlstm.sqltdsp = &sqltds;
   sqlstm.iters = (unsigned long )1;
   sqlstm.offset = (unsigned long )66;
   sqlstm.cud = sqlcud0;
   sqlstm.sqlest = (unsigned char  *)&sqlca;
   sqlstm.sqlety = (unsigned short)0;
   sqlcxt((void **)0, &sqlctx, &sqlstm, &sqlfpn);
   SQLCODE = sqlca.sqlcode;
}


}



/*************************************************************************************/
/*************************************************************************************/
void db_to_checkpoint ( pCHECKPOINT p_checkpoint )
{
   memset (p_checkpoint, 0, sizeof(CHECKPOINT));
   VARCHAR_TO_C (p_checkpoint->checkpoint_id,   db_checkpoint.checkpoint_id);
   VARCHAR_TO_C (p_checkpoint->checkpoint_trid, db_checkpoint.checkpoint_trid);
}



/*************************************************************************************/
/*************************************************************************************/
void checkpoint_to_db ( pCHECKPOINT p_checkpoint )
{
   memset (&db_checkpoint, 0, sizeof(db_checkpoint));
   C_TO_VARCHAR (db_checkpoint.checkpoint_id,   p_checkpoint->checkpoint_id);
   C_TO_VARCHAR (db_checkpoint.checkpoint_trid, p_checkpoint->checkpoint_trid);
}



/*************************************************************************************/
/*************************************************************************************/
BYTE dbcommon_select_checkpoint ( pCHAR       checkpoint_id, 
                                  pCHECKPOINT p_checkpoint, 
                                  pCHAR       ErrorMsg )
{
   memset  (&db_checkpoint, 0, sizeof(db_checkpoint));
   C_TO_VARCHAR(db_checkpoint.checkpoint_id, checkpoint_id);

   /* EXEC SQL SELECT 
      checkpoint_trid
   INTO 
      :db_checkpoint.checkpoint_trid
   FROM checkpoint
   WHERE checkpoint_id = :db_checkpoint.checkpoint_id; */ 

{
   struct sqlexd sqlstm;

   sqlstm.sqlvsn = 10;
   sqlstm.arrsiz = 4;
   sqlstm.sqladtp = &sqladt;
   sqlstm.sqltdsp = &sqltds;
   sqlstm.stmt = "select checkpoint_trid into :b0  from checkpoint where che\
ckpoint_id=:b1";
   sqlstm.iters = (unsigned long )1;
   sqlstm.offset = (unsigned long )81;
   sqlstm.selerr = (unsigned short)1;
   sqlstm.cud = sqlcud0;
   sqlstm.sqlest = (unsigned char  *)&sqlca;
   sqlstm.sqlety = (unsigned short)0;
   sqlstm.sqhstv[0] = (         void  *)&(db_checkpoint.checkpoint_trid);
   sqlstm.sqhstl[0] = (unsigned long )26;
   sqlstm.sqhsts[0] = (         long )0;
   sqlstm.sqindv[0] = (         void  *)0;
   sqlstm.sqinds[0] = (         long )0;
   sqlstm.sqharm[0] = (unsigned long )0;
   sqlstm.sqadto[0] = (unsigned short )0;
   sqlstm.sqtdso[0] = (unsigned short )0;
   sqlstm.sqhstv[1] = (         void  *)&(db_checkpoint.checkpoint_id);
   sqlstm.sqhstl[1] = (unsigned long )26;
   sqlstm.sqhsts[1] = (         long )0;
   sqlstm.sqindv[1] = (         void  *)0;
   sqlstm.sqinds[1] = (         long )0;
   sqlstm.sqharm[1] = (unsigned long )0;
   sqlstm.sqadto[1] = (unsigned short )0;
   sqlstm.sqtdso[1] = (unsigned short )0;
   sqlstm.sqphsv = sqlstm.sqhstv;
   sqlstm.sqphsl = sqlstm.sqhstl;
   sqlstm.sqphss = sqlstm.sqhsts;
   sqlstm.sqpind = sqlstm.sqindv;
   sqlstm.sqpins = sqlstm.sqinds;
   sqlstm.sqparm = sqlstm.sqharm;
   sqlstm.sqparc = sqlstm.sqharc;
   sqlstm.sqpadto = sqlstm.sqadto;
   sqlstm.sqptdso = sqlstm.sqtdso;
   sqlcxt((void **)0, &sqlctx, &sqlstm, &sqlfpn);
   SQLCODE = sqlca.sqlcode;
}



   if (SQLCODE == NO_DATA_FOUND)
   {
      memset  (p_checkpoint, 0, sizeof(CHECKPOINT));
      strncpy (p_checkpoint->checkpoint_id, checkpoint_id, sizeof(p_checkpoint->checkpoint_id));

      /* EXEC SQL INSERT INTO checkpoint
        (checkpoint_id, 
         checkpoint_trid)
      VALUES 
        (:db_checkpoint.checkpoint_id,
         :db_checkpoint.checkpoint_trid); */ 

{
      struct sqlexd sqlstm;

      sqlstm.sqlvsn = 10;
      sqlstm.arrsiz = 4;
      sqlstm.sqladtp = &sqladt;
      sqlstm.sqltdsp = &sqltds;
      sqlstm.stmt = "insert into checkpoint(checkpoint_id,checkpoint_trid) v\
alues (:b0,:b1)";
      sqlstm.iters = (unsigned long )1;
      sqlstm.offset = (unsigned long )104;
      sqlstm.cud = sqlcud0;
      sqlstm.sqlest = (unsigned char  *)&sqlca;
      sqlstm.sqlety = (unsigned short)0;
      sqlstm.sqhstv[0] = (         void  *)&(db_checkpoint.checkpoint_id);
      sqlstm.sqhstl[0] = (unsigned long )26;
      sqlstm.sqhsts[0] = (         long )0;
      sqlstm.sqindv[0] = (         void  *)0;
      sqlstm.sqinds[0] = (         long )0;
      sqlstm.sqharm[0] = (unsigned long )0;
      sqlstm.sqadto[0] = (unsigned short )0;
      sqlstm.sqtdso[0] = (unsigned short )0;
      sqlstm.sqhstv[1] = (         void  *)&(db_checkpoint.checkpoint_trid);
      sqlstm.sqhstl[1] = (unsigned long )26;
      sqlstm.sqhsts[1] = (         long )0;
      sqlstm.sqindv[1] = (         void  *)0;
      sqlstm.sqinds[1] = (         long )0;
      sqlstm.sqharm[1] = (unsigned long )0;
      sqlstm.sqadto[1] = (unsigned short )0;
      sqlstm.sqtdso[1] = (unsigned short )0;
      sqlstm.sqphsv = sqlstm.sqhstv;
      sqlstm.sqphsl = sqlstm.sqhstl;
      sqlstm.sqphss = sqlstm.sqhsts;
      sqlstm.sqpind = sqlstm.sqindv;
      sqlstm.sqpins = sqlstm.sqinds;
      sqlstm.sqparm = sqlstm.sqharm;
      sqlstm.sqparc = sqlstm.sqharc;
      sqlstm.sqpadto = sqlstm.sqadto;
      sqlstm.sqptdso = sqlstm.sqtdso;
      sqlcxt((void **)0, &sqlctx, &sqlstm, &sqlfpn);
      SQLCODE = sqlca.sqlcode;
}



      if (SQLCODE != 0)
         return (dbcommon_oracle_to_pte_rcode (SQLCODE, ErrorMsg));

      dbcommon_commit();
      return (PTEMSG_OK);
   }

   if (SQLCODE != 0 && SQLCODE != NULL_COLUMN_DATA)
      return (dbcommon_oracle_to_pte_rcode (SQLCODE, ErrorMsg));

   db_to_checkpoint (p_checkpoint);

   return (PTEMSG_OK);
}



/*************************************************************************************/
/*************************************************************************************/
BYTE dbcommon_update_checkpoint ( pCHECKPOINT p_checkpoint, 
                                  pCHAR       ErrorMsg )
{
   checkpoint_to_db (p_checkpoint);

   /* EXEC SQL UPDATE checkpoint
   SET
      checkpoint_trid = :db_checkpoint.checkpoint_trid
   WHERE checkpoint_id = :db_checkpoint.checkpoint_id; */ 

{
   struct sqlexd sqlstm;

   sqlstm.sqlvsn = 10;
   sqlstm.arrsiz = 4;
   sqlstm.sqladtp = &sqladt;
   sqlstm.sqltdsp = &sqltds;
   sqlstm.stmt = "update checkpoint  set checkpoint_trid=:b0 where checkpoin\
t_id=:b1";
   sqlstm.iters = (unsigned long )1;
   sqlstm.offset = (unsigned long )127;
   sqlstm.cud = sqlcud0;
   sqlstm.sqlest = (unsigned char  *)&sqlca;
   sqlstm.sqlety = (unsigned short)0;
   sqlstm.sqhstv[0] = (         void  *)&(db_checkpoint.checkpoint_trid);
   sqlstm.sqhstl[0] = (unsigned long )26;
   sqlstm.sqhsts[0] = (         long )0;
   sqlstm.sqindv[0] = (         void  *)0;
   sqlstm.sqinds[0] = (         long )0;
   sqlstm.sqharm[0] = (unsigned long )0;
   sqlstm.sqadto[0] = (unsigned short )0;
   sqlstm.sqtdso[0] = (unsigned short )0;
   sqlstm.sqhstv[1] = (         void  *)&(db_checkpoint.checkpoint_id);
   sqlstm.sqhstl[1] = (unsigned long )26;
   sqlstm.sqhsts[1] = (         long )0;
   sqlstm.sqindv[1] = (         void  *)0;
   sqlstm.sqinds[1] = (         long )0;
   sqlstm.sqharm[1] = (unsigned long )0;
   sqlstm.sqadto[1] = (unsigned short )0;
   sqlstm.sqtdso[1] = (unsigned short )0;
   sqlstm.sqphsv = sqlstm.sqhstv;
   sqlstm.sqphsl = sqlstm.sqhstl;
   sqlstm.sqphss = sqlstm.sqhsts;
   sqlstm.sqpind = sqlstm.sqindv;
   sqlstm.sqpins = sqlstm.sqinds;
   sqlstm.sqparm = sqlstm.sqharm;
   sqlstm.sqparc = sqlstm.sqharc;
   sqlstm.sqpadto = sqlstm.sqadto;
   sqlstm.sqptdso = sqlstm.sqtdso;
   sqlcxt((void **)0, &sqlctx, &sqlstm, &sqlfpn);
   SQLCODE = sqlca.sqlcode;
}


       
   if (SQLCODE != 0)
      return (dbcommon_oracle_to_pte_rcode (SQLCODE, ErrorMsg));

   return (PTEMSG_OK);
}



/*************************************************************************************/
/*************************************************************************************/
BYTE dbcommon_delete_checkpoint ( pCHAR checkpoint_id, 
                                  pCHAR ErrorMsg )
{
   C_TO_VARCHAR(db_checkpoint.checkpoint_id, checkpoint_id);

   /* EXEC SQL DELETE FROM checkpoint
   WHERE checkpoint_id = :db_checkpoint.checkpoint_id; */ 

{
   struct sqlexd sqlstm;

   sqlstm.sqlvsn = 10;
   sqlstm.arrsiz = 4;
   sqlstm.sqladtp = &sqladt;
   sqlstm.sqltdsp = &sqltds;
   sqlstm.stmt = "delete  from checkpoint  where checkpoint_id=:b0";
   sqlstm.iters = (unsigned long )1;
   sqlstm.offset = (unsigned long )150;
   sqlstm.cud = sqlcud0;
   sqlstm.sqlest = (unsigned char  *)&sqlca;
   sqlstm.sqlety = (unsigned short)0;
   sqlstm.sqhstv[0] = (         void  *)&(db_checkpoint.checkpoint_id);
   sqlstm.sqhstl[0] = (unsigned long )26;
   sqlstm.sqhsts[0] = (         long )0;
   sqlstm.sqindv[0] = (         void  *)0;
   sqlstm.sqinds[0] = (         long )0;
   sqlstm.sqharm[0] = (unsigned long )0;
   sqlstm.sqadto[0] = (unsigned short )0;
   sqlstm.sqtdso[0] = (unsigned short )0;
   sqlstm.sqphsv = sqlstm.sqhstv;
   sqlstm.sqphsl = sqlstm.sqhstl;
   sqlstm.sqphss = sqlstm.sqhsts;
   sqlstm.sqpind = sqlstm.sqindv;
   sqlstm.sqpins = sqlstm.sqinds;
   sqlstm.sqparm = sqlstm.sqharm;
   sqlstm.sqparc = sqlstm.sqharc;
   sqlstm.sqpadto = sqlstm.sqadto;
   sqlstm.sqptdso = sqlstm.sqtdso;
   sqlcxt((void **)0, &sqlctx, &sqlstm, &sqlfpn);
   SQLCODE = sqlca.sqlcode;
}



   if (SQLCODE != 0)
      return (dbcommon_oracle_to_pte_rcode (SQLCODE, ErrorMsg));

   return (PTEMSG_OK);
}



/*************************************************************************************/
/*************************************************************************************/
void timer_to_db ( pTIMER_DATA p_timer )
{
   memset (&db_timer, 0, sizeof(db_timer));
   C_TO_VARCHAR (db_timer.timer_id ,   p_timer->timer_id);
   C_TO_VARCHAR (db_timer.app_data1,   p_timer->app_data1);
   C_TO_VARCHAR (db_timer.app_data2,   p_timer->app_data2);
   C_TO_VARCHAR (db_timer.reply_que,   p_timer->reply_que);
   C_TO_VARCHAR (db_timer.orig_system, p_timer->orig_system);
   C_TO_VARCHAR (db_timer.ds_name,     this_ds_name);
   db_timer.timeout  = p_timer->timeout;
   db_timer.interval = p_timer->interval;
}



/*************************************************************************************/
/*************************************************************************************/
void db_to_timer ( pTIMER_DATA p_timer )
{
   memset (p_timer, 0, sizeof(TIMER_DATA));
   VARCHAR_TO_C (p_timer->timer_id,    db_timer.timer_id);
   VARCHAR_TO_C (p_timer->app_data1,   db_timer.app_data1);
   VARCHAR_TO_C (p_timer->app_data2,   db_timer.app_data2);
   VARCHAR_TO_C (p_timer->reply_que,   db_timer.reply_que);
   VARCHAR_TO_C (p_timer->orig_system, db_timer.orig_system);
   p_timer->timeout    = db_timer.timeout;
   p_timer->interval   = db_timer.interval;
   p_timer->timer_size = sizeof(TIMER_DATA);
;
}



/*************************************************************************************/
/*************************************************************************************/
BYTE dbcommon_insert_timer ( pTIMER_DATA p_timer, 
                             pCHAR       ErrorMsg )
{
   if (this_ds_name [0] == '\0')
      GetAppName (this_ds_name);

   timer_to_db (p_timer);

   /* EXEC SQL INSERT INTO timer
     (timer_id,
      app_data1,
      app_data2,
      reply_que,
      orig_system,
      ds_name,
      timeout,
      interval 
     )
   VALUES
     (:db_timer.timer_id,
      :db_timer.app_data1,
      :db_timer.app_data2,
      :db_timer.reply_que,
      :db_timer.orig_system,
      :db_timer.ds_name,
      :db_timer.timeout,
      :db_timer.interval 
     ); */ 

{
   struct sqlexd sqlstm;

   sqlstm.sqlvsn = 10;
   sqlstm.arrsiz = 8;
   sqlstm.sqladtp = &sqladt;
   sqlstm.sqltdsp = &sqltds;
   sqlstm.stmt = "insert into timer(timer_id,app_data1,app_data2,reply_que,o\
rig_system,ds_name,timeout,interval) values (:b0,:b1,:b2,:b3,:b4,:b5,:b6,:b7)";
   sqlstm.iters = (unsigned long )1;
   sqlstm.offset = (unsigned long )169;
   sqlstm.cud = sqlcud0;
   sqlstm.sqlest = (unsigned char  *)&sqlca;
   sqlstm.sqlety = (unsigned short)0;
   sqlstm.sqhstv[0] = (         void  *)&(db_timer.timer_id);
   sqlstm.sqhstl[0] = (unsigned long )26;
   sqlstm.sqhsts[0] = (         long )0;
   sqlstm.sqindv[0] = (         void  *)0;
   sqlstm.sqinds[0] = (         long )0;
   sqlstm.sqharm[0] = (unsigned long )0;
   sqlstm.sqadto[0] = (unsigned short )0;
   sqlstm.sqtdso[0] = (unsigned short )0;
   sqlstm.sqhstv[1] = (         void  *)&(db_timer.app_data1);
   sqlstm.sqhstl[1] = (unsigned long )34;
   sqlstm.sqhsts[1] = (         long )0;
   sqlstm.sqindv[1] = (         void  *)0;
   sqlstm.sqinds[1] = (         long )0;
   sqlstm.sqharm[1] = (unsigned long )0;
   sqlstm.sqadto[1] = (unsigned short )0;
   sqlstm.sqtdso[1] = (unsigned short )0;
   sqlstm.sqhstv[2] = (         void  *)&(db_timer.app_data2);
   sqlstm.sqhstl[2] = (unsigned long )34;
   sqlstm.sqhsts[2] = (         long )0;
   sqlstm.sqindv[2] = (         void  *)0;
   sqlstm.sqinds[2] = (         long )0;
   sqlstm.sqharm[2] = (unsigned long )0;
   sqlstm.sqadto[2] = (unsigned short )0;
   sqlstm.sqtdso[2] = (unsigned short )0;
   sqlstm.sqhstv[3] = (         void  *)&(db_timer.reply_que);
   sqlstm.sqhstl[3] = (unsigned long )18;
   sqlstm.sqhsts[3] = (         long )0;
   sqlstm.sqindv[3] = (         void  *)0;
   sqlstm.sqinds[3] = (         long )0;
   sqlstm.sqharm[3] = (unsigned long )0;
   sqlstm.sqadto[3] = (unsigned short )0;
   sqlstm.sqtdso[3] = (unsigned short )0;
   sqlstm.sqhstv[4] = (         void  *)&(db_timer.orig_system);
   sqlstm.sqhstl[4] = (unsigned long )18;
   sqlstm.sqhsts[4] = (         long )0;
   sqlstm.sqindv[4] = (         void  *)0;
   sqlstm.sqinds[4] = (         long )0;
   sqlstm.sqharm[4] = (unsigned long )0;
   sqlstm.sqadto[4] = (unsigned short )0;
   sqlstm.sqtdso[4] = (unsigned short )0;
   sqlstm.sqhstv[5] = (         void  *)&(db_timer.ds_name);
   sqlstm.sqhstl[5] = (unsigned long )18;
   sqlstm.sqhsts[5] = (         long )0;
   sqlstm.sqindv[5] = (         void  *)0;
   sqlstm.sqinds[5] = (         long )0;
   sqlstm.sqharm[5] = (unsigned long )0;
   sqlstm.sqadto[5] = (unsigned short )0;
   sqlstm.sqtdso[5] = (unsigned short )0;
   sqlstm.sqhstv[6] = (         void  *)&(db_timer.timeout);
   sqlstm.sqhstl[6] = (unsigned long )4;
   sqlstm.sqhsts[6] = (         long )0;
   sqlstm.sqindv[6] = (         void  *)0;
   sqlstm.sqinds[6] = (         long )0;
   sqlstm.sqharm[6] = (unsigned long )0;
   sqlstm.sqadto[6] = (unsigned short )0;
   sqlstm.sqtdso[6] = (unsigned short )0;
   sqlstm.sqhstv[7] = (         void  *)&(db_timer.interval);
   sqlstm.sqhstl[7] = (unsigned long )4;
   sqlstm.sqhsts[7] = (         long )0;
   sqlstm.sqindv[7] = (         void  *)0;
   sqlstm.sqinds[7] = (         long )0;
   sqlstm.sqharm[7] = (unsigned long )0;
   sqlstm.sqadto[7] = (unsigned short )0;
   sqlstm.sqtdso[7] = (unsigned short )0;
   sqlstm.sqphsv = sqlstm.sqhstv;
   sqlstm.sqphsl = sqlstm.sqhstl;
   sqlstm.sqphss = sqlstm.sqhsts;
   sqlstm.sqpind = sqlstm.sqindv;
   sqlstm.sqpins = sqlstm.sqinds;
   sqlstm.sqparm = sqlstm.sqharm;
   sqlstm.sqparc = sqlstm.sqharc;
   sqlstm.sqpadto = sqlstm.sqadto;
   sqlstm.sqptdso = sqlstm.sqtdso;
   sqlcxt((void **)0, &sqlctx, &sqlstm, &sqlfpn);
   SQLCODE = sqlca.sqlcode;
}



   if (SQLCODE != 0)
      return (dbcommon_oracle_to_pte_rcode (SQLCODE, ErrorMsg));

   return (PTEMSG_OK);
}



/*************************************************************************************/
/*************************************************************************************/
BYTE dbcommon_delete_timer ( pTIMER_DATA p_timer,
                             pCHAR       ErrorMsg )
{
   if (this_ds_name [0] == '\0')
      GetAppName (this_ds_name);

   C_TO_VARCHAR (db_timer.timer_id, p_timer->timer_id);
   C_TO_VARCHAR (db_timer.ds_name,  this_ds_name);

   /* EXEC SQL DELETE FROM timer
   WHERE timer_id = :db_timer.timer_id  AND   
         ds_name  = :db_timer.ds_name; */ 

{
   struct sqlexd sqlstm;

   sqlstm.sqlvsn = 10;
   sqlstm.arrsiz = 8;
   sqlstm.sqladtp = &sqladt;
   sqlstm.sqltdsp = &sqltds;
   sqlstm.stmt = "delete  from timer  where (timer_id=:b0 and ds_name=:b1)";
   sqlstm.iters = (unsigned long )1;
   sqlstm.offset = (unsigned long )216;
   sqlstm.cud = sqlcud0;
   sqlstm.sqlest = (unsigned char  *)&sqlca;
   sqlstm.sqlety = (unsigned short)0;
   sqlstm.sqhstv[0] = (         void  *)&(db_timer.timer_id);
   sqlstm.sqhstl[0] = (unsigned long )26;
   sqlstm.sqhsts[0] = (         long )0;
   sqlstm.sqindv[0] = (         void  *)0;
   sqlstm.sqinds[0] = (         long )0;
   sqlstm.sqharm[0] = (unsigned long )0;
   sqlstm.sqadto[0] = (unsigned short )0;
   sqlstm.sqtdso[0] = (unsigned short )0;
   sqlstm.sqhstv[1] = (         void  *)&(db_timer.ds_name);
   sqlstm.sqhstl[1] = (unsigned long )18;
   sqlstm.sqhsts[1] = (         long )0;
   sqlstm.sqindv[1] = (         void  *)0;
   sqlstm.sqinds[1] = (         long )0;
   sqlstm.sqharm[1] = (unsigned long )0;
   sqlstm.sqadto[1] = (unsigned short )0;
   sqlstm.sqtdso[1] = (unsigned short )0;
   sqlstm.sqphsv = sqlstm.sqhstv;
   sqlstm.sqphsl = sqlstm.sqhstl;
   sqlstm.sqphss = sqlstm.sqhsts;
   sqlstm.sqpind = sqlstm.sqindv;
   sqlstm.sqpins = sqlstm.sqinds;
   sqlstm.sqparm = sqlstm.sqharm;
   sqlstm.sqparc = sqlstm.sqharc;
   sqlstm.sqpadto = sqlstm.sqadto;
   sqlstm.sqptdso = sqlstm.sqtdso;
   sqlcxt((void **)0, &sqlctx, &sqlstm, &sqlfpn);
   SQLCODE = sqlca.sqlcode;
}



   if (SQLCODE != 0)
      return( dbcommon_oracle_to_pte_rcode( SQLCODE, ErrorMsg ) );

   return PTEMSG_OK;
}



/*************************************************************************************/
/*************************************************************************************/
BYTE dbcommon_get_timer_table ( pTIMER_DB_LIST p_db_list,
                                pCHAR          ErrorMsg )
{
   int temp ;


   if (this_ds_name [0] == '\0')
      GetAppName (this_ds_name);

   C_TO_VARCHAR (db_timer.ds_name, this_ds_name);

   memset (p_db_list, 0, sizeof(TIMER_DB_LIST));

   /* EXEC SQL DECLARE timertable_cursor CURSOR FOR
      SELECT 
         timer_id,
         app_data1,
         app_data2,
         reply_que,
         orig_system,
         timeout,
         interval 
      FROM timer
      WHERE ds_name = :db_timer.ds_name
      ORDER BY timeout; */ 

    
   /* EXEC SQL WHENEVER SQLERROR goto sql_error; */ 

   /* EXEC SQL OPEN timertable_cursor; */ 

{
   struct sqlexd sqlstm;

   sqlstm.sqlvsn = 10;
   sqlstm.arrsiz = 8;
   sqlstm.sqladtp = &sqladt;
   sqlstm.sqltdsp = &sqltds;
   sqlstm.stmt = sq0011;
   sqlstm.iters = (unsigned long )1;
   sqlstm.offset = (unsigned long )239;
   sqlstm.cud = sqlcud0;
   sqlstm.sqlest = (unsigned char  *)&sqlca;
   sqlstm.sqlety = (unsigned short)0;
   sqlstm.sqhstv[0] = (         void  *)&(db_timer.ds_name);
   sqlstm.sqhstl[0] = (unsigned long )18;
   sqlstm.sqhsts[0] = (         long )0;
   sqlstm.sqindv[0] = (         void  *)0;
   sqlstm.sqinds[0] = (         long )0;
   sqlstm.sqharm[0] = (unsigned long )0;
   sqlstm.sqadto[0] = (unsigned short )0;
   sqlstm.sqtdso[0] = (unsigned short )0;
   sqlstm.sqphsv = sqlstm.sqhstv;
   sqlstm.sqphsl = sqlstm.sqhstl;
   sqlstm.sqphss = sqlstm.sqhsts;
   sqlstm.sqpind = sqlstm.sqindv;
   sqlstm.sqpins = sqlstm.sqinds;
   sqlstm.sqparm = sqlstm.sqharm;
   sqlstm.sqparc = sqlstm.sqharc;
   sqlstm.sqpadto = sqlstm.sqadto;
   sqlstm.sqptdso = sqlstm.sqtdso;
   sqlcxt((void **)0, &sqlctx, &sqlstm, &sqlfpn);
   SQLCODE = sqlca.sqlcode;
   if (sqlca.sqlcode < 0) goto sql_error;
}



   /* Fetch each row from the timer table into the data struct. */
   /* EXEC SQL WHENEVER SQLERROR goto sql_error2; */ 

   /* EXEC SQL WHENEVER NOTFOUND DO break; */ 

   
   for (temp = 0; temp < TIMER_DB_LIST_SIZE; temp++)
   {
      /* EXEC SQL FETCH timertable_cursor
      INTO 
         :db_timer.timer_id,
         :db_timer.app_data1,
         :db_timer.app_data2,
         :db_timer.reply_que,
         :db_timer.orig_system,
         :db_timer.timeout,
         :db_timer.interval; */ 

{
      struct sqlexd sqlstm;

      sqlstm.sqlvsn = 10;
      sqlstm.arrsiz = 8;
      sqlstm.sqladtp = &sqladt;
      sqlstm.sqltdsp = &sqltds;
      sqlstm.iters = (unsigned long )1;
      sqlstm.offset = (unsigned long )258;
      sqlstm.cud = sqlcud0;
      sqlstm.sqlest = (unsigned char  *)&sqlca;
      sqlstm.sqlety = (unsigned short)0;
      sqlstm.sqhstv[0] = (         void  *)&(db_timer.timer_id);
      sqlstm.sqhstl[0] = (unsigned long )26;
      sqlstm.sqhsts[0] = (         long )0;
      sqlstm.sqindv[0] = (         void  *)0;
      sqlstm.sqinds[0] = (         long )0;
      sqlstm.sqharm[0] = (unsigned long )0;
      sqlstm.sqadto[0] = (unsigned short )0;
      sqlstm.sqtdso[0] = (unsigned short )0;
      sqlstm.sqhstv[1] = (         void  *)&(db_timer.app_data1);
      sqlstm.sqhstl[1] = (unsigned long )34;
      sqlstm.sqhsts[1] = (         long )0;
      sqlstm.sqindv[1] = (         void  *)0;
      sqlstm.sqinds[1] = (         long )0;
      sqlstm.sqharm[1] = (unsigned long )0;
      sqlstm.sqadto[1] = (unsigned short )0;
      sqlstm.sqtdso[1] = (unsigned short )0;
      sqlstm.sqhstv[2] = (         void  *)&(db_timer.app_data2);
      sqlstm.sqhstl[2] = (unsigned long )34;
      sqlstm.sqhsts[2] = (         long )0;
      sqlstm.sqindv[2] = (         void  *)0;
      sqlstm.sqinds[2] = (         long )0;
      sqlstm.sqharm[2] = (unsigned long )0;
      sqlstm.sqadto[2] = (unsigned short )0;
      sqlstm.sqtdso[2] = (unsigned short )0;
      sqlstm.sqhstv[3] = (         void  *)&(db_timer.reply_que);
      sqlstm.sqhstl[3] = (unsigned long )18;
      sqlstm.sqhsts[3] = (         long )0;
      sqlstm.sqindv[3] = (         void  *)0;
      sqlstm.sqinds[3] = (         long )0;
      sqlstm.sqharm[3] = (unsigned long )0;
      sqlstm.sqadto[3] = (unsigned short )0;
      sqlstm.sqtdso[3] = (unsigned short )0;
      sqlstm.sqhstv[4] = (         void  *)&(db_timer.orig_system);
      sqlstm.sqhstl[4] = (unsigned long )18;
      sqlstm.sqhsts[4] = (         long )0;
      sqlstm.sqindv[4] = (         void  *)0;
      sqlstm.sqinds[4] = (         long )0;
      sqlstm.sqharm[4] = (unsigned long )0;
      sqlstm.sqadto[4] = (unsigned short )0;
      sqlstm.sqtdso[4] = (unsigned short )0;
      sqlstm.sqhstv[5] = (         void  *)&(db_timer.timeout);
      sqlstm.sqhstl[5] = (unsigned long )4;
      sqlstm.sqhsts[5] = (         long )0;
      sqlstm.sqindv[5] = (         void  *)0;
      sqlstm.sqinds[5] = (         long )0;
      sqlstm.sqharm[5] = (unsigned long )0;
      sqlstm.sqadto[5] = (unsigned short )0;
      sqlstm.sqtdso[5] = (unsigned short )0;
      sqlstm.sqhstv[6] = (         void  *)&(db_timer.interval);
      sqlstm.sqhstl[6] = (unsigned long )4;
      sqlstm.sqhsts[6] = (         long )0;
      sqlstm.sqindv[6] = (         void  *)0;
      sqlstm.sqinds[6] = (         long )0;
      sqlstm.sqharm[6] = (unsigned long )0;
      sqlstm.sqadto[6] = (unsigned short )0;
      sqlstm.sqtdso[6] = (unsigned short )0;
      sqlstm.sqphsv = sqlstm.sqhstv;
      sqlstm.sqphsl = sqlstm.sqhstl;
      sqlstm.sqphss = sqlstm.sqhsts;
      sqlstm.sqpind = sqlstm.sqindv;
      sqlstm.sqpins = sqlstm.sqinds;
      sqlstm.sqparm = sqlstm.sqharm;
      sqlstm.sqparc = sqlstm.sqharc;
      sqlstm.sqpadto = sqlstm.sqadto;
      sqlstm.sqptdso = sqlstm.sqtdso;
      sqlcxt((void **)0, &sqlctx, &sqlstm, &sqlfpn);
      SQLCODE = sqlca.sqlcode;
      if (sqlca.sqlcode == 1403) break;
      if (sqlca.sqlcode < 0) goto sql_error2;
}



      sql_error2:
         if ( (SQLCODE != 0) && (SQLCODE != NULL_COLUMN_DATA) )
            goto sql_error;

      db_to_timer (&p_db_list->db_rec [temp]);

      p_db_list->num_returned++;
   }

   /* Close the cursor. */
   /* EXEC SQL CLOSE timertable_cursor; */ 

{
   struct sqlexd sqlstm;

   sqlstm.sqlvsn = 10;
   sqlstm.arrsiz = 8;
   sqlstm.sqladtp = &sqladt;
   sqlstm.sqltdsp = &sqltds;
   sqlstm.iters = (unsigned long )1;
   sqlstm.offset = (unsigned long )301;
   sqlstm.cud = sqlcud0;
   sqlstm.sqlest = (unsigned char  *)&sqlca;
   sqlstm.sqlety = (unsigned short)0;
   sqlcxt((void **)0, &sqlctx, &sqlstm, &sqlfpn);
   SQLCODE = sqlca.sqlcode;
   if (sqlca.sqlcode < 0) goto sql_error2;
}



   return (PTEMSG_OK);

sql_error:
   /* EXEC SQL WHENEVER SQLERROR CONTINUE; */ 

   /* EXEC SQL WHENEVER NOTFOUND CONTINUE; */ 


   return (dbcommon_oracle_to_pte_rcode (SQLCODE, ErrorMsg));
}


