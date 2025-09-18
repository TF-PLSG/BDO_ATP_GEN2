/* $Log:   N:\POS\PVCS6.6\EPICPORTZ\PTE\Equitable\App_lib\Source\TXUTILS.C  $
   
      Rev 1.12   Mar 12 2007 14:35:34   DIrby
   Increased max number of lines to read when looking for a section
   in an ini file.
   SCR 24081
   
      Rev 1.11   Jul 18 2002 14:56:46   dirby
   Added functionality to support timing of queries and reporting
   on any that take too long.  This is configurable in TF.INI.
   SCR 818
   
      Rev 1.10   Jan 15 2002 15:24:08   dirby
   In function read_id, changed MAX_LEN from 80 to queue size.
   This is so we do not read more data than what calling functions
   might allocate.
   SCR 546
   
      Rev 1.9   Jan 14 2002 18:00:40   dirby
   Added functions for transaction statistics.
   SCR 484
   
      Rev 1.8   Aug 22 2000 16:42:54   dirby
   Changed the name of function 'blank_or_null_string' to
   'blank_or_null_str' to avoid duplicate definition for some
   applications.
   
      Rev 1.7   Aug 22 2000 15:25:06   dirby
   Changed some comments and input parameters in the INI file
   functions to reference size of a single element instead of 
   the size of an array of elements.
   
      Rev 1.6   Aug 22 2000 11:31:46   dirby
   Changed MAX_APP_NAME_SIZE to MAX_QUE_NAME_SIZE in the INI files.  This is
   because the app name size is huge, and we are only dealing with queue names.
   
      Rev 1.5   Aug 20 2000 10:31:52   dirby
   Added function GetPrivateProfileList and necessary subfunctions.  These
   functions allow you to read a list from an INI file.
   
      Rev 1.4   Nov 16 1999 14:13:22   mbysani
    
   
      Rev 1.3   Nov 16 1999 11:19:14   mbysani
   Added 3 functions
   
      Rev 1.3   Nov 16 1999 09:50:40   mbysani
    
   
      Rev 1.2   Feb 16 1999 15:23:26   MSALEH
   new GUI code
   
      Rev 1.1   Feb 02 1999 15:39:20   IGOLDFIE
   Changed include files
   
      Rev 1.0   01 Feb 1999 16:24:54   jhapitas
   Initial Revision
   
      Rev 1.1   Dec 15 1998 09:11:04   skatuka2
   Commented 'Initial Revision .... '

      Rev 1.0   Dec 04 1998 15:58:18   MSALEH
   Initial revision
*/
#include <stdlib.h>
#include <stdio.h>
#include <memory.h>
#include <math.h>
#include <string.h>
#include "basictyp.h"
#include "txutils.h"
#include "app_info.h"
#include "equitdb.h"
#include "dc_dbstruct.h"
#include "tx_dbstruct.h"
#include "nc_dbstruct.h"
#include "mmcos.h"
#include "memmnger.h"
#include "ptemsg.h"
#include "ntutils.h"
#include "diskio.h"

PRIVATE CHAR   applnk_que_name[MAX_QUE_NAME_SIZE+1] = "applnkA" ;
PRIVATE CHAR   ErrorMsg  [100];

/* Globals for Transaction Statistics */
TXN_COUNTS     txn_counts;
MONITOR_STATS  monitor_stats;

#ifdef XIPC
#include "pteipc.h"
#else
#include "ptetcp.h"
#endif

#ifdef  _DEBUG
   #define PRINT(x) printf(x) 
#else   
   #define PRINT
#endif 

/***************************************************************************************/
LONG Txutils_Calculate_Julian_Date (BYTE* yyyymmdd)
{
   LONG jul, IGREG;
   int year, month, day, ja, jy, jm;
   BYTE yyyy[5] = "";
   BYTE mm[3] = "";
   BYTE dd[3] = "";

   memcpy(yyyy, yyyymmdd, 4);
   memcpy(mm, yyyymmdd + 4, 2);
   memcpy(dd, yyyymmdd + 6, 2);
   year = atoi(yyyy);
   month = atoi(mm);
   day = atoi(dd);
   jy = year;

   IGREG = 15 + 31L * (10 + 12L * 1582);

   if(jy == 0) 
      return(0);
   if(jy < 0)
      jy++;

   if (month > 2)
      jm = month + 1;
   else
   {
      jy--;
      jm = month + 13;
   }
   jul = (long)(floor(365.25 * jy) + floor(30.6001 * jm) + day + 1720995);
   if(day + 31L * (month + 12L * year) >= IGREG)
   {
      ja = (int)(0.01 * jy);
      jul += 2 - ja + (int)(0.25 * ja);
   }
   return(jul);
}

void Txutils_Calculate_Gregorian_Date(LONG julian, pCHAR yyyymmdd)
{
   CHAR yyyy[5] = "";
   CHAR mm[3] = "";
   CHAR dd[3] = "";
   int year, month, day;
   long IGREG;
   long ja, jalpha, jb, jc, jd, je;

   IGREG = 15 + 31L * (10 + 12L * 1582);

   if(julian >= IGREG)
   {
      jalpha = (long)(((float)(julian - 1867216) - 0.25) / 36524.25);
      ja = julian + 1 + jalpha - (long)(0.25 * jalpha);
   }
   else
      ja = julian;
   jb = ja + 1524;
   jc = (long)(6680.0 + ((float)(jb - 2439870) - 122.1) / 365.25);
   jd = (long)(365 * jc + (0.25 * jc));
   je = (long)((jb - jd) / 30.6001);
   day = jb - jd - (long)(30.6001 * je);
   month = je - 1;
   if(month > 12)
      month = month - 12;
   year = jc - 4715;
   if(month > 2)
      year = year - 1;
   if(year <= 0)
      year--;

   memset(yyyymmdd, 0, 9);

   itoa(year, yyyy, 10);
   itoa(month, mm, 10);
   itoa(day, dd, 10);
   memset(yyyymmdd, 48, 8);
   memcpy(yyyymmdd + 4 - strlen(yyyy), yyyy, strlen(yyyy));
   memcpy(yyyymmdd + 6 - strlen(mm), mm, strlen(mm));
   memcpy(yyyymmdd + 8 - strlen(dd), dd, strlen(dd));
}

/*************************************************************************************
NAME:			TxUtils_Send_Msg_To_Operator
DESCRIPTION:	This function gets called by each of the services whenever a message is to
				be sent to the Systems Monitor (or the Operator Monitor)

INPUTS:			int nWriteToSystemMonitor:  1 (write to Systems Monitor), 0 (do not write to Systems Monitor)
				BYTE* strMessage: the text of the message (do not exceed 100 bytes)
				int nWriteToEventLog:		1 (write to Event Log). 0 (do not write to Event Log)
				int nEventLogMsgType:		ERROR_MSG, INFO_MSG, WARN_MSG, ALERT_MSG
				BYTE* strFunction:			Name of the function where the error is being generated
				BYTE nSeverity:				0,1,2,3,4 (0 being the lowest severity)
				BYTE* strMessageType:		INFO_ERROR, WARNING_ERROR, FATAL_ERROR 
				BYTE* strTerminal:			Terminal (NULL_PTR or a nulled string if not relevant)  
				BYTE* strCardNumber:		Card Number (NULL_PTR or a nulled string if not relevant)  
				BYTE* strMerchant:			Merchant (NULL_PTR or a nulled string if not relevant) 

OUTPUTS:		None
RETURNS:        false - if error
				true  - if no error
AUTHOR:         Sanjoy Dasgupta
MODIFIED BY:	
*************************************************************************************/
#ifdef XIPC

BOOLEAN TxUtils_Send_Msg_To_Operator (int nWriteToSystemMonitor, BYTE* strMessage, 
									  int nWriteToEventLog, int nEventLogMsgType, 
									  BYTE* strFunction, BYTE nSeverity, 
									  BYTE* strMessageType, BYTE* strTerminal, 
									  BYTE* strCardNumber, BYTE* strMerchant)
{
	BYTE strTempMessage[256] = {0} ;
	
	CHAR ConnectionId[MAX_ORIG_INFO_SIZE+1] = {0} ;
	CHAR strAppName[MAX_APP_NAME_SIZE+1] = {0} ;
	pPTE_MSG p_msg_out = 0 ;
	LONG lReturnCode = 0L ;
	XINT ReturnCode = 0 ;
	OPR_MESSAGE sOpr_Message ;
	INT nIndex = 0;char *cardIndex=NULL ;
	INT cardlen = 0;
	CHAR tempcard[21] = "";

	GetAppName (strAppName) ;

	memset (&sOpr_Message, 0, sizeof (sOpr_Message)) ;

	if (strCardNumber != NULL_PTR)
	{
		if (strCardNumber[0] != NULL_PTR)		
		{
		/* Masking card number first 5 and last 4 in clear form remaining all masked. */
		cardlen = strlen( strCardNumber );
		strcpy( tempcard, strCardNumber );
		for ( nIndex = cardlen-5; nIndex >= cardlen-11; nIndex-- )
		{
			tempcard[nIndex] = '*';
		}
			cardIndex = strstr(strMessage,strCardNumber);

		}
	
	}
/* Check the string message if card number exists or not. If Card Number inside the messages
	Mask the card number and print	*/

	if(cardIndex != NULL)
	{
		strncpy(strMessage+4,tempcard,sizeof(tempcard));	
	}
	strcpy (strTempMessage, strMessage) ;
	if (strTerminal != NULL_PTR)
		if (strTempMessage[0] != NULL_PTR)
		{
			strcat (strTempMessage, " TERM: ") ;
			strcat (strTempMessage, strTerminal) ;
		}

	if (strCardNumber != NULL_PTR)
		if (strCardNumber[0] != NULL_PTR)
		{
			strcat (strTempMessage, " CARD: ") ;
			strcat (strTempMessage, tempcard) ;
		}

	if (strMerchant != NULL_PTR)
		if (strMerchant[0] != NULL_PTR)
		{
			strcat (strTempMessage, " MERCHANT: ") ;
			strcat (strTempMessage, strMerchant) ;
		}
	
	PRINT (strTempMessage) ;
	PRINT ("\n") ;
		
	if (nWriteToEventLog == 1)
		LogEvent (strTempMessage, nEventLogMsgType) ;

	if (nWriteToSystemMonitor == 1)
	{
		// Retrieve the connection id from shared memory
		// If connection_id does not exist => systemMonitor is down, return
		// ptemsg_build (MT_SYS_MONITOR, ST1_OPRMSG, set originator info = connection_id
		// ptemsg_send (msg, "applnkA") - ignore response

		// Try to read the MemMonitorTable twice if the first try fails
		for (nIndex = 1 ; nIndex <= 2 ; nIndex ++)
		{
			memset (ConnectionId, 0, sizeof (ConnectionId)) ;
			ReturnCode = ReadMemMonitorTable (ConnectionId) ;
			/*if (MEMACCESSERROR == ReturnCode )
			{
				if (nIndex == 2)
					return false ;
			}
			*/
			if (ConnectionId[0] == 0)
				continue ;
			else 
				break ;
		} // for 

		if (ConnectionId[0] != 0)
		{
			// Connection Id exists in shared memory
			// Populate the OPR_MESSAGE structure
			strcpy (sOpr_Message.message, strTempMessage) ;
			strcpy (sOpr_Message.module, strAppName) ;
			strcpy (sOpr_Message.function, strFunction) ;
			sOpr_Message.severity = nSeverity ;
			strcpy (sOpr_Message.message_type, strMessageType) ;

			p_msg_out = ptemsg_build_msg (MT_SYS_MONITOR, ST1_OPRMSG, 0, applnk_que_name, 0, (pBYTE) &sOpr_Message, sizeof (sOpr_Message), 0) ;
			ptemsg_set_orig_comms_info   (p_msg_out, ConnectionId) ;
          
         lReturnCode = pteipc_send (p_msg_out, applnk_que_name) ;
            
			free (p_msg_out) ;

			if (lReturnCode < 0)
				return false ;
		}
		else
			return false ;
	}
	return true ;
} /* TxUtils_Send_Msg_To_Operator */

#endif


void mask_card_number(char *tempcard, char *cardNum, int cardlen)
{
	char strCardNumber[30] = {0};
	int nIndex;

	memcpy(strCardNumber, cardNum, cardlen);

	if (strCardNumber != NULL_PTR)
	{
		if (strCardNumber[0] != NULL_PTR)
		{
			/* Masking card number first 5 and last 4 in clear form remaining all masked. */
			strcpy( tempcard, strCardNumber );
			for ( nIndex = cardlen-5; nIndex >= cardlen-11; nIndex-- )
			{			 //cardlen-6???
				tempcard[nIndex] = '*';
			}
		}
	}
}

/******************************************************************************
 *
 *  NAME:         GETPRIVATEPROFILELIST
 *
 *  DESCRIPTION:  This function reads a list of identifiers from an .INI file.
 *                It opens the given filename, searches for the section title,
 *                then reads the list for that section.  It reads the list
 *                until it finds one of the following four:
 *
 *                   A blank line
 *                   End of File
 *                   The phrase "END OF LIST"
 *                   Another section title
 *
 *                The list of identifiers is stored into a character array
 *                whose elements should be defined as MAX_QUE_NAME_SIZE.  The
 *                array can hold as many as MAX_INI_ID_LIST_SIZE identifiers.
 *
 *  INPUTS:       filename        = Path and filename of INI file
 *                section_name    = Name of list to read
 *                size_of_element = Size of a single structure in 'list';
 *                                  Note that 'list' is an array of structures
 *
 *  OUTPUTS:      list = Pointer to start of memory allocated for ID's
 *
 *  RTRN VALUE:   If positive, indicates the number of ID's being returned
 *                If negative, indicates an error as follows:
 *
 *                   -1 = Unable to open the file
 *                   -2 = Cannot find the section name
 *                   -3 = Error occurred while copying an ID into the output
 *                        list.  Could indicate 'size_of_list' is too small
 *                        of a value to hold all the ID's being read.
 *
 *  AUTHOR:       Dennis Irby
 *
 *  MODIFICATIONS:
 ******************************************************************************/
INT GetPrivateProfileList( pCHAR filename, pCHAR section_name,
                           pCHAR list,     INT   size_of_element )
{
   INT      rc  = 0;
   BOOLEAN  end_of_list;
   CHAR     id_string[MAX_QUE_NAME_SIZE+1];
   FILE     *fp    =  NULL;
   FILE     **p_fp =  &(fp);


   if ( false == open_file_for_reading(p_fp, filename) )
   {
      /* Unable to open the file. */
      rc = -1;
   }
   else 
   {
      if ( false == find_section_name(p_fp, section_name) )
      {
         /* Cannot find section name in file. */
         rc = -2;
      }
      else
      {
         /* Read through the list of ID's for the given section. */
         end_of_list = false;
         while( !end_of_list )
         {
            memset( id_string, 0x00, sizeof(id_string) );
            if ( false == read_id(p_fp, id_string) )
               end_of_list = true;
            else
            {
               /* Successful read - result is in id_string.
                * Make sure we have room to store it.
                */
               if ( rc >= MAX_INI_ID_LIST_SIZE )
               {
                  /* Out of storage space.
                   * Do not read anymore.
                   * Do not store the ID we just read.
                   */
                  end_of_list = true;
               }
               else if ( !copy_id_to_list(id_string,list,size_of_element,rc) )
               {
                  /* 'list' is too small to hold this ID. */
                  rc = -3;
                  end_of_list = true;
               }
               else
               {
                  ++rc;
                  list += size_of_element;
               }
            }
         } /* End of WHILE Loop */
      }
      (void)close_file( p_fp );
   }
   return( rc );
}

/******************************************************************************
 *
 * MODULE:	    FIND_SECTION_NAME
 *
 * DESCRIPTION: This function reads a file looking for a name defined within
 *              square brackets, i.e. [section_name].
 *
 * AUTHOR:      D. Irby
 *
 * INPUTS:      fp           = File pointer
 *              section_name = Name we are looking for within brackets
 *
 * OUTPUTS:     fp = On success, fp will point to the line after section name
 *
 * RTRN VALUE:  True  if section name is found
 *              False if section name is not found
 *
 * MODIFICATIONS:
 ******************************************************************************/
BOOLEAN find_section_name( FILE **fp, pCHAR section_name )
{
#define  MAXLEN     80
#define  MAXLINES  400

   BOOLEAN  rc = false;
   CHAR     buffer[MAXLEN];
   CHAR     section_name_with_brackets[MAXLEN];
   INT      lines_read = 0;
   INT      len;

   /* Check length of section name */
   len = strlen(section_name)+2;
   if ( MAXLEN > len )
   {
      /* Before reading the file, put brackets around
       * section name for comparison.
       */
      memset( section_name_with_brackets, 0x00, MAXLEN );
      section_name_with_brackets[0] = '[';
      strcat( section_name_with_brackets, section_name );
      strcat( section_name_with_brackets, "]" );

      /* Read through the file looking for the section. */
      while( buffer[0] != EOF )
      {
         memset( buffer, 0x00, MAXLEN );
         if ( NULL == fgets(buffer, MAXLEN, *fp) )
            break;
         else
         {
            ++lines_read;
            if ( 0 == strncmp(section_name_with_brackets, buffer, len) )
            {
               rc = true;
               break;
            }
         }
      }
   }
   return( rc );
}

/******************************************************************************
 *
 * MODULE:	    READ_ID
 *
 * DESCRIPTION: This function reads application name identifiers from an ini
 *              file and stores it into an output buffer.
 *
 * AUTHOR:      D. Irby
 *
 * INPUTS:      fp = File pointer
 *
 * OUTPUTS:     id_string = String where ID is stored, must be at least
 *                          MAX_QUE_NAME_SIZE+1 in size.
 *
 * RTRN VALUE:   False = End of list - see note
 *               True  = Successful read
 *
 *       NOTE:   End of list is defined as any of the following:
 *                  The string "END OF LIST"
 *                  A blank line
 *                  A new section
 *                  End of file
 *                  File read error
 *
 * MODIFICATIONS:
 ******************************************************************************/
BOOLEAN read_id( FILE **fp, pCHAR id_string )
{
#define  MAX_LEN  MAX_QUE_NAME_SIZE

   BOOLEAN   rc = true;
   CHAR      temp_id_string[MAX_QUE_NAME_SIZE+1];
   INT       len;
   INT       i;

   if ( NULL == fgets(id_string, MAX_LEN, *fp) )
   {
      /* Either an error occurred, or we reached the end of file. */
      rc = false;
   }
   else
   {
      /* The read was successful.
       * Remove '\n' character at end of string.
       * Check for end of list, blank line, or a section name.
       */
      len = strlen(id_string);
      if ( len > 0 )
         id_string[len-1] = '\0';

      if ( id_string[0] == '[' )
      {
         /* New section - end of current list. */
         rc = false;
      }
      else if ( blank_or_null_str(id_string, len) )
      {
         /* Blank line */
         rc = false;
      }
      else
      {
         /* Convert to uppercase to test for string of END OF LIST. */
         memset(temp_id_string, 0x00, sizeof(temp_id_string) );
         strcpy( temp_id_string, id_string );
         for( i=0; i<len; i++ )
            temp_id_string[i] = toupper( id_string[i] );

         if ( 0 == strcmp(temp_id_string, END_OF_LIST) )
            rc = false;
      }
   }
   return( rc );
}

/******************************************************************************
 *
 * MODULE:	    COPY_ID_TO_LIST
 *
 * DESCRIPTION: This function copies an application ID into a list of IDs.
 *              It will first make sure there is enough memory allocated
 *              in the list.  It does this based on a list defined as a CHAR
 *              array[MAX_INI_ID_LIST_SIZE][MAX_QUE_NAME_SIZE+1].  If there
 *              is not enough memory, an error is returned.
 *
 * AUTHOR:      D. Irby
 *
 * INPUTS:      size_of_element = Number of bytes allocted for the output
 *              num_items       = Number of identifiers already stored in list
 *
 * OUTPUTS:     list = Array of application identifiers
 *
 * RTRN VALUE:  True  if storage of identifier into list is successful
 *              False if there is not enough storage in list
 *
 * MODIFICATIONS:
 ******************************************************************************/
BOOLEAN copy_id_to_list( pCHAR id_string,       pCHAR list, 
                         INT   size_of_element, INT   num_items )
{
   BOOLEAN  rc;
   INT      mem_used;
   INT      len;

   /* Calculate amount of memory used. */
   mem_used = num_items * size_of_element;

   /* Is there enough memory available? */
   if ( num_items >= MAX_INI_ID_LIST_SIZE )
   {
      /* NO, not enough memory */
      rc = false;
   }
   else
   {
      /* YES, plenty of room */
      len = strlen( id_string );
      strncpy( list, id_string, len );
      list[len] = '\0';
      rc = true;
   }
   return( rc );
}



/*****************************************************************************

  Function:    blank_or_null_str

  Description: This function will determine if a string is blank or contains
               only nulls.
  Author: 
      E. Gable  3/7/99
  Inputs:
      astr    - the string to test
      max_len - the length of the string to test.
  Outputs:
      None
  Return values:
      true - the string is blank
      false - the string is not blank
  Modified by:
******************************************************************************/
BOOLEAN blank_or_null_str(pCHAR astr, INT max_len)
{
   INT i;
   for(i = 0;i < max_len; i++)
   {
      if ((astr[i] != ' ') && (astr[i] != '\0'))
         return(false);
   }
   return(true);    
}

/******************************************************************************
 *
 *  NAME:         RESET_STATS
 *
 *  DESCRIPTION:  This function will set the transaction statistics to zero.
 *                This is the number of transactions received from the host,
 *                and the number of transactions this network controller has
 *                sent to the host.  There are also stats for number of txns
 *                approved, declined, reversed, and timed out.
 *
 *  INPUTS:       p_txn_cnts - Structure containing the counts
 *
 *  OUTPUTS:      p_txn_cnts - Structure counts are updated to zero
 *
 *  RTRN VALUE:   None
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
void reset_stats( pTXN_COUNTS p_txn_cnts )
{
   p_txn_cnts->sent_count     = 0UL;
   p_txn_cnts->received_count = 0UL;
   p_txn_cnts->approved_count = 0UL;
   p_txn_cnts->declined_count = 0UL;
   p_txn_cnts->reversed_count = 0UL;
   p_txn_cnts->time_out_count = 0UL;
   p_txn_cnts->mgtrequest_count = 0UL;//praneeth
   p_txn_cnts->stip_sent_count = 0UL;	//ANNA
   p_txn_cnts->stip_recv_count = 0UL;	//ANNA
   return;
}

/******************************************************************************
 *
 *  NAME:         UPDATE_TXN_STATS
 *
 *  DESCRIPTION:  This function updates the transaction statistics.  The input
 *                parameter determines which statistic gets incremented.  This
 *                function also will reset the statistics if the count gets
 *                too high.
 *
 *  INPUTS:       p_txn_cnt - Structure containing the counts
 *                p_auth_tx - Transaction data structure
 *                stat_type - SENT, RECEIVED, or NA
 *
 *  OUTPUTS:      p_txn_cnts - Structure counts are updated
 *
 *  RTRN VALUE:   None
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
void update_txn_stats( pTXN_COUNTS p_txn_cnt, pAUTH_TX p_auth_tx,INT stat_type )
{
   /*+-----------------------------+
     | Update sent/received counts |
     +-----------------------------+*/
   if ( stat_type == RECEIVED )
   {
	   if( p_auth_tx != NULL_PTR )
	   {
	   		if(strcmp(p_auth_tx->TLF01_details.product_codes[1].code,STANDIN) == 0)
			{
				p_txn_cnt->stip_recv_count++;
				if ( p_txn_cnt->stip_recv_count >= MAX_STAT_COUNT )
				{
				   p_txn_cnt->stip_recv_count = 0UL;
				}
				return;
			}
	   	}
   
      p_txn_cnt->received_count++;
      if ( p_txn_cnt->received_count >= MAX_STAT_COUNT )
         p_txn_cnt->received_count = 0UL;
   }
   else if ( stat_type == SENT )
   {
	   if( p_auth_tx != NULL_PTR )
	   {
	   		if(strcmp(p_auth_tx->TLF01_details.product_codes[1].code,STANDIN) == 0)
			{
				p_txn_cnt->stip_sent_count++;
				if ( p_txn_cnt->stip_sent_count >= MAX_STAT_COUNT )
				{
				   p_txn_cnt->stip_sent_count = 0UL;
				}
				return;
			}
	   	}
	   
      p_txn_cnt->sent_count++;
      if ( p_txn_cnt->sent_count >= MAX_STAT_COUNT )
         p_txn_cnt->sent_count = 0UL;
   }
   //else added by praneeth for MGT
   else if ( stat_type == MGT)
   {
	   p_txn_cnt->mgtrequest_count++;
	   if ( p_txn_cnt->mgtrequest_count >= MAX_STAT_COUNT )
		   p_txn_cnt->mgtrequest_count = 0UL;	  
	   return;
   }

   /*+-----------------------------------------+
     | Update counts dealing with txn results. |
     +-----------------------------------------+*/
   if ( p_auth_tx != NULL_PTR )
   {
      if ( p_auth_tx->TLF01_details.tx_key > 99 )
      {
         /* This is a response. */

         /* Was it approved? */
         if ( 0 == strcmp(p_auth_tx->TLF01_details.response_code,"00") )
         {
            /* Yes */
            p_txn_cnt->approved_count++;
            if ( p_txn_cnt->approved_count >= MAX_STAT_COUNT )
               p_txn_cnt->approved_count = 0UL;
         }

         /* No it was not approved.  Did it time out? */
         else if ( 0 == strcmp(p_auth_tx->TLF01_details.response_code,"19") )
         {
            /* Yes */
            p_txn_cnt->time_out_count++;
            if ( p_txn_cnt->time_out_count >= MAX_STAT_COUNT )
               p_txn_cnt->time_out_count = 0UL;
         }

         /* No it did not time out.  Then it was declined. */
         else
         {
            p_txn_cnt->declined_count++;
            if ( p_txn_cnt->declined_count >= MAX_STAT_COUNT )
               p_txn_cnt->declined_count = 0UL;
         }
      }
      else
      {
         /* This is a request.  Is it a reversal? */
         if ( p_auth_tx->TLF01_details.message_type[1] == '4' )
         {
            /* Yes */
            p_txn_cnt->reversed_count++;
            if ( p_txn_cnt->reversed_count >= MAX_STAT_COUNT )
               p_txn_cnt->reversed_count = 0UL;
         }

         /* Did it time out? */
         if ( 0 == strcmp(p_auth_tx->TLF01_details.response_code,"19") )
         {
            /* Yes */
            p_txn_cnt->time_out_count++;
            if ( p_txn_cnt->time_out_count >= MAX_STAT_COUNT )
               p_txn_cnt->time_out_count = 0UL;
         }

      }
   }
   return;
}


/******************************************************************************
 *
 *  NAME:         GET_TXN_STATS
 *
 *  DESCRIPTION:  This function gets the transaction statistics from the
 *                txn_cnt structure and copies them into the MONITOR_STATS
 *                structure, converting them as needed.  The MONITOR_STATS
 *                structure is used to pass the stats from a service to
 *                the monitor.
 *
 *  INPUTS:       p_txn_cnt - Structure containing the counts
 *
 *  OUTPUTS:      p_monitor_stats - Structure to be passed to Monitor
 *
 *  RTRN VALUE:   None
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
void get_txn_stats( pTXN_COUNTS p_txn_cnt, pMONITOR_STATS p_monitor_stats )
{
   /* Convert statistics from numbers to strings. */
   sprintf(p_monitor_stats->cnt_to_network,  "%09ld",p_txn_cnt->sent_count    );
   sprintf(p_monitor_stats->cnt_from_network,"%09ld",p_txn_cnt->received_count);
   sprintf(p_monitor_stats->num_approved,    "%09ld",p_txn_cnt->approved_count);
   sprintf(p_monitor_stats->num_declined,    "%09ld",p_txn_cnt->declined_count);
   sprintf(p_monitor_stats->num_reversed,    "%09ld",p_txn_cnt->reversed_count);
   sprintf(p_monitor_stats->num_time_out,    "%09ld",p_txn_cnt->time_out_count);
   sprintf(p_monitor_stats->num_mgt_request, "%09ld",p_txn_cnt->mgtrequest_count);//praneeth
   sprintf(p_monitor_stats->num_stip_recv,	 "%09ld",p_txn_cnt->stip_recv_count);//TF Anna
   sprintf(p_monitor_stats->num_stip_sent,   "%09ld",p_txn_cnt->stip_sent_count);//TF Anna

   return;
}

#ifdef XIPC

/******************************************************************************
 *
 *  NAME:         GET_DATASERVER_INI_TIMER_VALUE
 *
 *  DESCRIPTION:  This function finds the DATASERVERS section in the tf.ini
 *                file.  It then reads the timer flag to determine if timing
 *                of queries is turned on.  If it is, then the value is read.
 *                This timer value is the criteria for logging messages to the
 *                system log (not Monitor).  This value is in seconds, e.g.
 *                a value of 60 is the same as 1 minute.  If a query takes
 *                longer than this value, then a message is logged, but
 *                processing continues.
 *
 *  INPUTS:       appname - Name of calling service, for error messages
 *
 *  OUTPUTS:      ds_timer_flag  - 0 = timing is turned off, 1 = on
 *                ds_timer_value - Number of seconds to compare a query against.
 *                errbuf         - Text message in case of problems.
 *
 *  RTRN VALUE:   True if no problems reading tf.ini file, else false.
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
INT get_dataserver_ini_timer_value(pINT  ds_timer_flag,
                                   pINT  ds_timer_value,
                                   pCHAR appname, pCHAR errbuf )
{
   #define sON  "1"
   #define sOFF "0"
   #define sDEFAULT_DS_TIMER_VALUE  "5"

   DWORD rc;
   CHAR  filename   [80];
   CHAR  tmpstr     [80];
   CHAR  timer_flag [5] = "";
   CHAR  timer_value[5] = "";
   INT   ret_val = true;

   /* Get path to the tf.ini file. */
   memset( tmpstr, 0x00, sizeof(tmpstr) );
   GetPinnacleConfigDirectory(tmpstr);
   sprintf(filename, "%s%s", tmpstr, "tf.ini" );

   /*----------------------------------------*/
   /* Get Data Server Timer Flag - On or Off */
   /*----------------------------------------*/
   memcpy( timer_flag, sOFF, sizeof(sOFF) );
   rc = GetPrivateProfileString(
            "DATASERVERS",        /* points to section name         */
            "QUERY_TIMING",       /* points to key name             */
             sOFF,                /* Default string (Off)           */
             timer_flag,          /* points to destination buffer   */
             sizeof(timer_flag)-1,/* size of destination buffer     */
             filename             /* points to ini filename         */
   );

   if ( 0 == strcmp(timer_flag, sOFF) )
   {
      /* Timer is Off; no need to get the value. */
      *ds_timer_flag = DS_TIMER_OFF;
   }
   else if ( 0 == strcmp(timer_flag, sON) )
   {
      /* Timer is On; get the number of seconds for query criteria. */
      *ds_timer_flag = DS_TIMER_ON;
      memcpy( timer_value,
              sDEFAULT_DS_TIMER_VALUE,
              sizeof(sDEFAULT_DS_TIMER_VALUE) );

      rc = GetPrivateProfileString(
               "DATASERVERS",             /* points to section name         */
               "QUERY_TIMER_VALUE",       /* points to key name             */
                sDEFAULT_DS_TIMER_VALUE,  /* Default string                 */
                timer_value,              /* points to destination buffer   */
                sizeof(timer_value)-1,    /* size of destination buffer     */
                filename                  /* points to ini filename         */
      );

      *ds_timer_value = atoi(timer_value);
      if ( (*ds_timer_value <= 0) || (*ds_timer_value > 999) )
      {
         sprintf( errbuf,
                 "%s:Invalid value DS Timer Value (%s) in tf.ini. Use 0 to 999",
                  appname, timer_value );
         *ds_timer_flag = DS_TIMER_OFF;
         ret_val = false;
      }
   }
   else
   {
      /* The tf.ini file contains an invalid value for the timer flag.
       * Log this fact, set timer off, and exit.
       */
      sprintf( errbuf,
              "%s:Invalid value (%s) in tf.ini file for Data Server QUERY_TIMING flag.",
               appname, timer_flag );
      *ds_timer_flag = DS_TIMER_OFF;
      ret_val = false;
   }
   return( ret_val );
}


/******************************************************************************
 *
 *  NAME:         COMPARE_QUERY_TIME_AGAINST_CRITERIA
 *
 *  DESCRIPTION:  This function compares the time that a query spent with the
 *                database against a configured time.  If the query time is
 *                greater than the configured time, a message is returned,
 *                giving information about the query.  This is so the query
 *                can be identified, with the hopes of improving it.
 *
 *  INPUTS:       start_time    - Time before query (format = ssssssssssss.jjj)
 *                end_time      - Time after  query ( <same> )
 *                criteria_time - Number of seconds that the query should not
 *                                exceed
 *                app_name      - Name of data server service (for err msg)
 *                p_msg         - PTE Msg containing identifying info about
 *                                the query.
 *
 *  OUTPUTS:      errbuf - Text message identifying a query that exceeds
 *                         the criteria time.
 *
 *  RTRN VALUE:   True if query meets the time, else false
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
INT compare_query_time_against_criteria( pCHAR start_time,    pCHAR end_time,
                                          INT  criteria_time, pPTE_MSG p_msg,
                                         pCHAR app_name,      pCHAR errbuf )
{
   pBYTE          p_data;
   pPTE_MSG_DATA  p_msg_data;
   double         i_start_time;
   double         i_end_time;
   double         query_time;
   BYTE           msg_type;
   BYTE           subtype1;
   BYTE           subtype2;
   BYTE           app_data_type;
   CHAR           orig_comm_inf[MAX_ORIG_INFO_SIZE];
   INT            ret_val,cardlen=0,nIndex;
   AUTH_TX        auth_tx;BYTE mask_card_num[20];

   /* Get the amount of time the query spent. */
   i_start_time = atof( start_time );
   i_end_time   = atof( end_time   );
   query_time   = i_end_time - i_start_time;

   /* Compare time of query against the criteria. */
   if ( query_time > (double)criteria_time )
   {
      /* Query took too long.  Gather the identifying information about it. */
      msg_type = ptemsg_get_msg_type    ( p_msg );
      subtype1 = ptemsg_get_msg_subtype1( p_msg );
      subtype2 = ptemsg_get_msg_subtype2( p_msg );

      p_msg_data    = ptemsg_get_pte_msg_data( p_msg );
      app_data_type = ptemsg_get_pte_msg_data_app_data_type( p_msg_data );

      /* Get information to identify the transaction. */
      p_data = ptemsg_get_pte_msg_data_data( p_msg_data );
      memcpy( &auth_tx, p_data, sizeof(AUTH_TX) );
	  /* TF-Phani - Mask the card num*/
	  memcpy(mask_card_num,auth_tx.TLF01_details.card_num,sizeof(auth_tx.TLF01_details.card_num));
	  cardlen = strlen(mask_card_num);
	  for ( nIndex = cardlen-5; nIndex >= cardlen-11; nIndex-- )
		{
			mask_card_num[nIndex] = '*';
		}
      if ( auth_tx.TLF01_details.handler_queue[0] == 0x00 )
      {
         /* This is from the GUI. */
         strcpy( orig_comm_inf, ptemsg_get_msg_orig_comms_info(p_msg) );
         sprintf( errbuf,
                 "%s:Exceeded Query Time: MsgType=%d, ST1=%d, ST2=%d, AppData=%d, Time=%.3f, from %s",
                  app_name, msg_type, subtype1, subtype2,
                  app_data_type, query_time, orig_comm_inf );
      }
      else
      {
         /* This is a transaction. */
         sprintf( errbuf,
                 "%s:Exceeded Query Time: MsgType=%d, ST1=%d, ST2=%d, AppData=%d, Time=%.3f, TID=%s, Card=%s, Handler=%s",
                  app_name,msg_type,subtype1,subtype2,app_data_type,query_time,
                  auth_tx.TLF01_details.terminal_id,
                  mask_card_num,
                  auth_tx.TLF01_details.handler_queue );
      }
      ret_val = false;
   }
   else
   {
      ret_val = true;
   }
   return( ret_val );
}

#endif
