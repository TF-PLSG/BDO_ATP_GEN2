/******************************************************************************
  
   Module:           nega.h
  
   Title:            
  
   Description:
   
   Application:      ATP

   Author:           

   Revision History: 1.0

$Log:   N:\PVCS\PTE\Equitable\Negative\nega.h  $
   
      Rev 1.3   Jan 10 2002 17:32:52   dirby
   Modified to support JCB and Amex cards and to allow for the
   cards in the file to be in any order.  Also modified the summary
   to show number of records of each type inserted.
   SCR 567
   
      Rev 1.2   Feb 28 2000 10:49:22   ddabberu
    
   
      Rev 1.1   Jan 03 2000 09:42:32   gbarnes
   updated for AIX port
   commented out structure because of redundant
   data structure.

******************************************************************************/

#ifndef NEGA_H
#define NEGA_H

/* ------------------- */
/* Function Prototypes */
/* ------------------- */

/* .c File */
INT   delete_record_from_neg01( pCHAR ErrorMsg );
INT   delete_record_from_neg02( pCHAR ErrorMsg );
INT   copy_neg01_neg02( pCHAR ErrorMsg );
INT   DetermineCardType( pCHAR card_record );
INT   backup_cardtype_in_db( void );
NEG01 populateNeg01Table(CHAR dataRecord[]);
void  returnNumbers(CHAR input[]);
void  initialize( void );
void  prepare_for_shutdown( pCHAR ErrorMsg );
void  log_message( pCHAR msg, INT severity );

/* .pc File */
void  neg01_to_db( pNEG01 neg01_ptr );
void  neg02_to_db( pNEG02 neg02_ptr );
BYTE  db_delete_neg01( pNEG01 neg01_ptr, pCHAR ErrorMsg);
BYTE  db_delete_neg02( pNEG02 neg02_ptr, pCHAR ErrorMsg);
BYTE  db_insert_neg01( pNEG01 neg01_ptr, pCHAR ErrorMsg );
BYTE  db_copy_neg01_neg02(pNEG01 neg01_ptr, pCHAR ErrorMsg);


/* ---------------- */
/* Global Constants */
/* ---------------- */

#define RECORD_LENGTH 21

#define MYPRINT(tempMsg)  printf("%s\n",tempMsg);           

/* SQL Return Codes */
#define NULL_COLUMN_DATA -1405
#define NO_DATA_FOUND     1403
#define UNIQUE_CONSTRAINT    1

/* Supported Card Types */
#define VISA_CARD 0
#define MC_CARD   1
#define JCB_CARD  2
#define AMEX_CARD 3

#define NUM_CARD_TYPES 4
#define INVALID_CARD   NUM_CARD_TYPES+1

/* Card Processing Status */
#define INIT    0
#define WORKING 1

/* File Processing Status */
#define NO  0
#define YES 1


#define MSG_SIZE  99


#endif
