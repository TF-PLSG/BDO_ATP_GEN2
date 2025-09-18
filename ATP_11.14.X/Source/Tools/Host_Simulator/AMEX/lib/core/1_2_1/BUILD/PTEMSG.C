/******************************************************************************

        Module: ptemsg.c
  
         Title: APIs to construct/manipulate IPC messages
  
   Description: This file contains all definitions and structures used to
                build messages to be sent between processes for the Pinnacle
                Transaction Environment.

   $Log:   N:\PVCS\PTE\CORE\PTE_LIB\PTEMSG.C  $
   
      Rev 1.13   May 18 1998 11:17:10   drodden
   Changed build-msg to put in the originating system name.
   
   
      Rev 1.12   13 May 1998 12:50:46   MSALEH
   1) Define PTE_MSG0, PTE_MSG20 structures
   corresponding to pte msg version < 20 and 20
   2) Define PTE_MSG_DATA0, PTE_MSG_DATA20
   3) for every get/set routine, handle both
   versions using type casts
   4) Application API will not change
   
      Rev 1.11   May 01 1998 07:55:22   drodden
   Added the message version, originator system name, and log flag
   to the message header.
   
   
      Rev 1.10   Jan 08 1998 09:59:18   drodden
   Changed ptecopy to include the string header for the needed prototypes.
   Also created a header and prototypes for ptecopy.
   
   
      Rev 1.9   Jan 07 1998 10:28:04   drodden
   Added support for the current/last tran IDs.
   Added the type-of-reply-needed field and APIs.
   
   
      Rev 1.8   12 Dec 1997 11:45:00   mvandenb
   Changed pte_copy to ptecopy_strncpy.
   
      Rev 1.7   Dec 01 1997 08:54:14   drodden
   Added APIs to get/set the originator comms info.
   Added the string header to eliminate compile warnings.
   
   
      Rev 1.6   Oct 23 1997 11:42:26   drodden
   This is part of the continued development of the Pinnacle 
   Transaction Environment.  Many changes, refinements,
   and additions were made to the core modules.  I'm checking
   them in now to get a snapshot of this current working version.
   
   
      Rev 1.5   Oct 03 1997 15:35:18   drodden
   Changed to use the native ltoa rather that itoa.
   
   
      Rev 1.4   Oct 03 1997 10:48:02   drodden
   Added functions to get/set the msg transaction id.
   Changed to call atol to decode message lengths.
   
   
      Rev 1.3   26 Sep 1997 09:38:30   mvandenb
   Added routines to get and return the 'result_code'
   field from the PTE_MSG structure.
   

      Rev 1.2   25 Sep 1997 12:43:16   mvandenb
   Changed all 'INT' declarations to 'LONG'.  Also changed to use
   ascii_to_long function instead of ascii_to_int function.
   

      Rev 1.1   25 Sep 1997 11:19:52   mvandenb
   Changes to make data/message chaining more straightforward.
   

      Rev 1.0   15 Sep 1997 16:08:18   mvandenb
   Initial version of PTE inter-process message APIs.

******************************************************************************/

#include <stdlib.h>
#include <string.h>

#include "ptemsg.h"
#include "ptecopy.h"
#include "ntutils.h"


/* 
 * The following structures are defined to be used as a 
 * reference when declaring offsets for the PTE_MSG header structure.
 * each new field will have a field code, the field code
 * will be used to determine the offset into the message header to 
 * obtain the value of that field in every PTE_MSG_VERSION
 */

/*
 * Old versions of the PTE message header, MSG VERSION < 20.
 */
typedef struct
{
   BYTE  data_type;                    /* PTE_MESSAGE_DATA or APPLICATION_DATA */
   BYTE  app_data_type;                /* Application data type designator */
   CHAR  data_length [MAX_ITOA_SIZE];  /* Length of the data. */
   BYTE  data;                         /* First byte of the data. */
} PTE_MSG_DATA0, *pPTE_MSG_DATA0;


typedef struct
{
   BYTE          msg_type;                               /* Message type. */
   BYTE          msg_subtype1;                           /* Future use. */
   BYTE          msg_subtype2;                           /* Future use. */
   BYTE          result_code;                            /* Field to hold return values of requests. */
   CHAR          current_tran_id [MAX_TRAN_ID_SIZE];     /* Transaction ID from Logger/Reply. */
   CHAR          last_tran_id    [MAX_TRAN_ID_SIZE];     /* Transaction ID from Logger/Reply. */
   CHAR          dest_queue      [MAX_QUE_NAME_SIZE];  
   CHAR          orig_queue      [MAX_QUE_NAME_SIZE];
   CHAR          orig_comms_info [MAX_ORIG_INFO_SIZE];   /* communications info about the originator */
   CHAR          msg_length      [MAX_ITOA_SIZE];        /* Length of entire message. */
   BYTE          type_of_reply_needed;                           /* reply desired: only on error, etc */
   BYTE          num_chained_data;                       /* Number of data elements in chain. */
   BYTE          num_chained_messages;                   /* Number of messages in chain. */
   PTE_MSG_DATA0  msg_data;
} PTE_MSG0, *pPTE_MSG0;

#define   PTE_MSG_HDR_SIZE0       sizeof ( PTE_MSG0 ) - 1
#define   PTE_MSG_DATA_HDR_SIZE0  sizeof ( PTE_MSG_DATA0 ) - 1


/*
 * version of the PTE message header, MSG VERSION = 20.
 */
typedef struct
{
   BYTE  data_type;                    /* PTE_MESSAGE_DATA or APPLICATION_DATA */
   BYTE  app_data_type;                /* Application data type designator */
   CHAR  data_length [MAX_ITOA_SIZE];  /* Length of the data. */
   BYTE  data;                         /* First byte of the data. */
} PTE_MSG_DATA20, *pPTE_MSG_DATA20;

typedef struct
{
   BYTE          msg_type;                               /* Message type. */
   BYTE          msg_subtype1;                           /* message subtype 1 */
   BYTE          msg_subtype2;                           /* message subtype 2 */
   BYTE          msg_version;                            /* ptemsg internal version # */
   BYTE          result_code;                            /* Field to hold return values of requests. */
   BYTE          type_of_reply_needed;                   /* reply desired: only on error, etc */
   BYTE          log_flag;                               /* logger/reply flag */
   BYTE          pad1;                                   /* pad to 32bit boundary */
   CHAR          current_tran_id [MAX_TRAN_ID_SIZE];     /* Transaction ID from Logger/Reply. */
   CHAR          last_tran_id    [MAX_TRAN_ID_SIZE];     /* Transaction ID from Logger/Reply. */
   CHAR          dest_queue      [MAX_QUE_NAME_SIZE];    /* */
   CHAR          orig_queue      [MAX_QUE_NAME_SIZE];    /* */
   CHAR          orig_comms_info [MAX_ORIG_INFO_SIZE];   /* communications info about the originator */
   CHAR          orig_system     [MAX_SYSTEM_NAME_SIZE]; /* */
   CHAR          msg_length      [MAX_ITOA_SIZE];        /* Length of entire message. */
   BYTE          num_chained_data;                       /* Number of data elements in chain. */
   BYTE          num_chained_messages;                   /* Number of messages in chain. */
   BYTE          pad2 [2];                               /* pad to 32bit boundary */
   PTE_MSG_DATA  msg_data;
} PTE_MSG20, *pPTE_MSG20;

#define   PTE_MSG_HDR_SIZE20       sizeof ( PTE_MSG20 ) - 1
#define   PTE_MSG_DATA_HDR_SIZE20  sizeof ( PTE_MSG_DATA20 ) - 1




/******************************************************************************

      Name:          ptemsg_build_msg()

      Parameters:    msg_type          pte message type
                     msg_subtype1      pte message sub-type 1
                     msg_subtype2      pte message sub-type 2
                     dest_queue        name of destination queue for message
                     orig_queue        name of originator's queue
                     p_data            data included with message
                     data_len          length of the data p_data points to
                     app_data_type     designates type of data to application

      Description:   This function builds a message for transport between
                     Pinnacle processes.

      Return:        Pointer to the allocated memory that contains the
                     message or zero if the memory could not be allocated.

 ******************************************************************************/

pPTE_MSG ptemsg_build_msg ( BYTE  msg_type,
                            BYTE  msg_subtype1,
                            BYTE  msg_subtype2,
                            pCHAR dest_queue,
                            pCHAR orig_queue,
                            pBYTE p_data,
                            LONG  data_len,
                            BYTE  app_data_type )
{
   pPTE_MSG   p_pte_msg;
   LONG       alloc_size,
              msg_length;
   
   alloc_size = PTE_MSG_HDR_SIZE + data_len;

   /*
    * Allocate enough space for the message header and the data.
    */
   p_pte_msg = (pPTE_MSG)malloc (alloc_size);

   if (p_pte_msg != NULL_PTR)
   {
      memset (p_pte_msg, 0, alloc_size);
       
      p_pte_msg->msg_type     = msg_type;
      p_pte_msg->msg_subtype1 = msg_subtype1;
      p_pte_msg->msg_subtype2 = msg_subtype2;
      p_pte_msg->msg_version  = PTEMSG_MSG_VERSION;
      ptecopy_strncpy (p_pte_msg->orig_system, GetThisSystemsName(), sizeof(p_pte_msg->orig_system));

      /*
       * Copy the destination queue's name.
       */
      ptecopy_strncpy ( p_pte_msg->dest_queue, dest_queue, sizeof ( p_pte_msg->dest_queue ) );


      /*
       * Copy the originator's queue name.
       */
      ptecopy_strncpy ( p_pte_msg->orig_queue, orig_queue, sizeof ( p_pte_msg->dest_queue ) );

      /*
       * Start calculating the message length.
       */
      msg_length = PTE_MSG_HDR_SIZE;
     
      p_pte_msg->num_chained_messages = 1;
      p_pte_msg->num_chained_data     = 1;

      p_pte_msg->msg_data.data_type = APPLICATION_DATA;
      p_pte_msg->msg_data.app_data_type = app_data_type;
         
      if ( (p_data != NULL_PTR) && (data_len > 0) )
      {
         ltoa (data_len, p_pte_msg->msg_data.data_length, 10);
         
         memcpy (&p_pte_msg->msg_data.data, p_data, data_len);

         /*
          * NOTE:
          *
          * We add data_len - 1 to msg_length because we're copying the
          * data to the address of the last byte in the PTE_MSG_DATA
          * structure.  See ptemsg.h for structure definitions.
          */
         msg_length+= data_len;
      }

      /*
       * We now know the entire length of the message, set it.
       */
      ltoa (msg_length, p_pte_msg->msg_length, 10);
   }

   return (p_pte_msg);
}



/******************************************************************************/
/******************************************************************************/
pPTE_MSG ptemsg_chain_data_or_message ( pPTE_MSG p_pte_msg,
                                        pBYTE    p_data,
                                        LONG     data_len,
                                        BYTE     data_type,
                                        BYTE     app_data_type )
{
   pPTE_MSG_DATA  p_msg_data;
   pPTE_MSG       p_new_msg;
   pBYTE          p_new_mem;
   LONG           new_alloc_size,
                  cur_alloc_size;

   /*
    * If we were passed a NULL_PTR, then simply
    * return the PTE_MSG pointer passed to us and
    * do nothing.  WE MAY WANT TO ADD AN ERROR LOG
    * ENTRY FOR THIS CONDITION.
    */
   if ( p_data == NULL_PTR )
      return ( p_pte_msg );

   /*
    * Don't do anything with an invalid pointer!
    */
   if ( p_pte_msg == NULL_PTR )
      return ( p_pte_msg );

   /*
    * Allocate enough space for the existing message plus the
    * new data portion being chained to the existing message.
    */
   cur_alloc_size = atol ( p_pte_msg->msg_length );

   /*
    * Get the size of the data header and subtract one
    * because the last byte of the data header is a place
    * keeper for us to copy the new data to.
    */
   new_alloc_size = cur_alloc_size + PTE_MSG_DATA_HDR_SIZE + data_len;

   p_new_mem = (pBYTE)malloc (new_alloc_size);

   if (p_new_mem != NULL_PTR)
   {
      memset ( (pBYTE)p_new_mem, 0, new_alloc_size );
      
      /*
       * Copy the existing message to the new memory location.
       */
      memcpy ( p_new_mem, (pBYTE)p_pte_msg, cur_alloc_size );

      /*
       * Free the memory we were using.
       */
      free ( p_pte_msg );

      /*
       * Add the new data to the message.
       */
      p_msg_data = (pPTE_MSG_DATA) (p_new_mem + cur_alloc_size);
      p_new_msg  = (pPTE_MSG)p_new_mem;

      /*
       * Set the new message length.
       */
      ltoa ( new_alloc_size, p_new_msg->msg_length, 10 );
      
      p_msg_data->data_type = data_type;
      p_msg_data->app_data_type = app_data_type;

      if ( (p_data != NULL_PTR) && (data_len > 0) )
      {
         ltoa ( data_len, p_msg_data->data_length, 10 );

         memcpy ( &p_msg_data->data, p_data, data_len );
      }
      
      return ( p_new_msg );
   }
   
   return ( NULL_PTR );
}
      


/******************************************************************************

      Name:          ptemsg_chain_message()

      Description:   This function takes an existing PTE message and chains
                     another PTE message onto it.

      Return:        Pointer to the allocated memory that contains the
                     message or zero if the memory could not be allocated.

******************************************************************************/
pPTE_MSG ptemsg_chain_new_message ( pPTE_MSG p_pte_msg,
                                    BYTE     msg_type,
                                    BYTE     msg_subtype1,
                                    BYTE     msg_subtype2,
                                    pCHAR    dest_queue,
                                    pCHAR    orig_queue,
                                    pBYTE    p_data,
                                    LONG     data_len,
                                    BYTE     app_data_type )
{
   pPTE_MSG p_new_pte_msg,
            p_chained_msg;
   LONG     msg_length;
   
   p_new_pte_msg = ptemsg_build_msg (msg_type, msg_subtype1, msg_subtype2,
                                     dest_queue, orig_queue, p_data,
                                     data_len, app_data_type);

   if (p_new_pte_msg != NULL_PTR)
   {
      msg_length = atol ( p_new_pte_msg->msg_length );

      p_chained_msg = ptemsg_chain_data_or_message ( p_pte_msg,
                                                     (pBYTE)p_new_pte_msg,
                                                     msg_length,
                                                     PTE_MESSAGE_DATA,
                                                     PTE_MESSAGE_DATA );

      if ( p_chained_msg )
      {
         /*
          * Free up the memory used for the new PTE message.  This
          * message now resides within the memory allocated for the
          * newly created chained message.
          */
         free ( p_new_pte_msg );

         p_chained_msg->num_chained_messages += 1;

         return ( p_chained_msg );
      }

   }

   return ( NULL_PTR );
}
      


/******************************************************************************

      Name:          ptemsg_chain_message()

      Description:   This function takes an existing PTE message and chains
                     another PTE message onto it.

      Return:        Pointer to the allocated memory that contains the
                     message or zero if the memory could not be allocated.

******************************************************************************/
pPTE_MSG ptemsg_chain_message ( pPTE_MSG p_pte_msg1, pPTE_MSG p_pte_msg2 )
{
   pPTE_MSG p_chained_msg;
   LONG     msg_length;
   

   /*
    * Check for valid pointers.
    */
   if ( p_pte_msg1 != NULL_PTR )
   {
      if ( p_pte_msg2 != NULL_PTR )
      {
         msg_length = atol ( p_pte_msg2->msg_length );

         p_chained_msg = ptemsg_chain_data_or_message ( p_pte_msg1,
                                                        (pBYTE)p_pte_msg2,
                                                        msg_length,
                                                        PTE_MESSAGE_DATA,
                                                        PTE_MESSAGE_DATA );

         if ( p_chained_msg != NULL_PTR )
         {
            /*
             * Free up the memory that was being used for the PTE message just added
             * to the chain.  This message now resides within the memory allocated
             * for the newly created chained message.
             */
            free ( p_pte_msg2 );

            p_chained_msg->num_chained_messages += 1;

            return ( p_chained_msg );
         }

      }  /* 'if ( p_pte_msg2 )' */

   }  /* 'if ( p_pte_msg1 )' */

   return ( p_pte_msg1 );
}



/******************************************************************************

      Name:          ptemsg_chain_data()

      Return:        Pointer to the PTE message with the chained
                     data.

******************************************************************************/
pPTE_MSG ptemsg_chain_data ( pPTE_MSG p_pte_msg,
                             pBYTE    p_data,
                             LONG     data_len,
                             BYTE     app_data_type )
{
   pPTE_MSG p_new_pte_msg;

   if ( p_pte_msg != NULL_PTR )
   {
      p_new_pte_msg = ptemsg_chain_data_or_message ( p_pte_msg,
                                                     p_data,
                                                     data_len,
                                                     APPLICATION_DATA,
                                                     app_data_type );
      if ( p_new_pte_msg != NULL_PTR )
      {
         p_new_pte_msg->num_chained_data += 1;

         return ( p_new_pte_msg );
      }

   }

   return ( NULL_PTR );
}



/******************************************************************************

      Name:          ptemsg_get_msg_type()

      Return:        This function returns the value of the 'msg_type' field
                     from a PTE message.

******************************************************************************/
BYTE ptemsg_get_msg_type ( pPTE_MSG p_pte_msg )
{
   pPTE_MSG0   p_pte_msg0;
   pPTE_MSG20  p_pte_msg20;

   if ( p_pte_msg != NULL_PTR )
   {
      switch (PTEMSG_MSG_VERSION)
      {
      case 20:
         p_pte_msg20 = (pPTE_MSG20)p_pte_msg;
         return ( p_pte_msg20->msg_type );

      default:
         p_pte_msg0 = (pPTE_MSG0)p_pte_msg;
         return ( p_pte_msg0->msg_type );
      }
   }

   return ( 0 );
}



/******************************************************************************/
/******************************************************************************/
BYTE ptemsg_get_msg_subtype1 ( pPTE_MSG p_pte_msg )
{
   pPTE_MSG0   p_pte_msg0;
   pPTE_MSG20  p_pte_msg20;

   if ( p_pte_msg != NULL_PTR )
   {
      switch (PTEMSG_MSG_VERSION)
      {
      case 20:
         p_pte_msg20 = (pPTE_MSG20)p_pte_msg;
         return ( p_pte_msg20->msg_subtype1 );

      default:
         p_pte_msg0 = (pPTE_MSG0)p_pte_msg;
         return ( p_pte_msg0->msg_subtype1 );
      }
   }

   return ( 0 );
}



/******************************************************************************/
/******************************************************************************/
BYTE ptemsg_get_msg_subtype2 ( pPTE_MSG p_pte_msg )
{
   pPTE_MSG0   p_pte_msg0;
   pPTE_MSG20  p_pte_msg20;

   if ( p_pte_msg != NULL_PTR )
   {
      switch (PTEMSG_MSG_VERSION)
      {
      case 20:
         p_pte_msg20 = (pPTE_MSG20)p_pte_msg;
         return ( p_pte_msg20->msg_subtype2 );

      default:
         p_pte_msg0 = (pPTE_MSG0)p_pte_msg;
         return ( p_pte_msg0->msg_subtype2 );
      }
   }

   return ( 0 );
}



/******************************************************************************/
/******************************************************************************/
pCHAR ptemsg_get_msg_dest_queue ( pPTE_MSG p_pte_msg )
{
   pPTE_MSG0   p_pte_msg0;
   pPTE_MSG20  p_pte_msg20;

   if ( p_pte_msg != NULL_PTR )
   {
      switch (PTEMSG_MSG_VERSION)
      {
      case 20:
         p_pte_msg20 = (pPTE_MSG20)p_pte_msg;
         return ( p_pte_msg20->dest_queue );

      default:
         p_pte_msg0 = (pPTE_MSG0)p_pte_msg;
         return ( p_pte_msg0->dest_queue );
      }
   }

   return ( NULL_PTR );
}



/******************************************************************************/
/******************************************************************************/
pCHAR ptemsg_get_msg_orig_queue ( pPTE_MSG p_pte_msg )
{
   pPTE_MSG0   p_pte_msg0;
   pPTE_MSG20  p_pte_msg20;

   if ( p_pte_msg != NULL_PTR )
   {
      switch (PTEMSG_MSG_VERSION)
      {
      case 20:
         p_pte_msg20 = (pPTE_MSG20)p_pte_msg;
         return ( p_pte_msg20->orig_queue );

      default:
         p_pte_msg0 = (pPTE_MSG0)p_pte_msg;
         return ( p_pte_msg0->orig_queue );
      }
   }

   return ( NULL_PTR );
}



/******************************************************************************/
/******************************************************************************/
LONG ptemsg_get_msg_length ( pPTE_MSG p_pte_msg )
{
   pPTE_MSG0   p_pte_msg0;
   pPTE_MSG20  p_pte_msg20;

   if ( p_pte_msg != NULL_PTR )
   {
      switch (PTEMSG_MSG_VERSION)
      {
      case 20:
         p_pte_msg20 = (pPTE_MSG20)p_pte_msg;
         return ( atol ( p_pte_msg20->msg_length ) );

      default:
         p_pte_msg0 = (pPTE_MSG0)p_pte_msg;
         return ( atol ( p_pte_msg0->msg_length ) );
      }
   }

   return ( 0 );
}



/******************************************************************************/
/******************************************************************************/
LONG ptemsg_get_num_chained_messages ( pPTE_MSG p_pte_msg )
{
   pPTE_MSG0   p_pte_msg0;
   pPTE_MSG20  p_pte_msg20;

   if ( p_pte_msg != NULL_PTR )
   {
      switch (PTEMSG_MSG_VERSION)
      {
      case 20:
         p_pte_msg20 = (pPTE_MSG20)p_pte_msg;
         return ( p_pte_msg20->num_chained_messages );

      default:
         p_pte_msg0 = (pPTE_MSG0)p_pte_msg;
         return ( p_pte_msg0->num_chained_messages );
      }
   }

   return ( 0 );
}



/******************************************************************************/
/******************************************************************************/
LONG ptemsg_get_num_chained_data ( pPTE_MSG p_pte_msg )
{
   pPTE_MSG0   p_pte_msg0;
   pPTE_MSG20  p_pte_msg20;

   if ( p_pte_msg != NULL_PTR )
   {
      switch (PTEMSG_MSG_VERSION)
      {
      case 20:
         p_pte_msg20 = (pPTE_MSG20)p_pte_msg;
         return ( p_pte_msg20->num_chained_data );

      default:
         p_pte_msg0 = (pPTE_MSG0)p_pte_msg;
         return ( p_pte_msg0->num_chained_data );
      }
   }

   return ( 0 );
}



/******************************************************************************/
/******************************************************************************/
pPTE_MSG_DATA ptemsg_get_pte_msg_data ( pPTE_MSG p_pte_msg )
{
   pPTE_MSG0   p_pte_msg0;
   pPTE_MSG20  p_pte_msg20;

   if ( p_pte_msg != NULL_PTR )
   {
      switch (PTEMSG_MSG_VERSION)
      {
      case 20:
         p_pte_msg20 = (pPTE_MSG20)p_pte_msg;
         return ( (pPTE_MSG_DATA)&p_pte_msg20->msg_data );

      default:
         p_pte_msg0 = (pPTE_MSG0)p_pte_msg;
         return ( (pPTE_MSG_DATA)&p_pte_msg0->msg_data );
      }
   }

   return ( NULL_PTR );
}



/******************************************************************************/
/******************************************************************************/
pPTE_MSG_DATA ptemsg_get_chained_pte_msg_data ( pPTE_MSG p_pte_msg,
                                                LONG     desired_link )
{

   pPTE_MSG0         p_pte_msg0;
   pPTE_MSG20        p_pte_msg20;
   pPTE_MSG_DATA0    p_cur_msg_data0   = NULL_PTR;
   pPTE_MSG_DATA20   p_cur_msg_data20  = NULL_PTR;
   BOOLEAN           data_available;

   pPTE_MSG_DATA     p_cur_msg_data    = NULL_PTR;
   pBYTE             p_cur_data;
   LONG              current_link,
                     cur_data_length;
   
   if ( p_pte_msg != NULL_PTR )
   {
      switch (PTEMSG_MSG_VERSION)
      {
      case 20:
         p_pte_msg20 = (pPTE_MSG20)p_pte_msg;
         data_available = (desired_link <= p_pte_msg20->num_chained_data);
         break;

      default:
         p_pte_msg0 = (pPTE_MSG0)p_pte_msg;
         data_available = (desired_link <= p_pte_msg0->num_chained_data);
         break;
      }

      /*
       * Make sure we're not looking for data that isn't there.
       */
      if ( data_available )
      {
         switch (PTEMSG_MSG_VERSION)
         {
            case 20:
               /*
                * Get a pointer to the first PTE_MSG_DATA structure
                * in the message pointed to by 'p_pte_msg'.
                */
               p_cur_msg_data20 = (pPTE_MSG_DATA20)&p_pte_msg20->msg_data;

               current_link = 1;
               while ( current_link < desired_link )
               {
                  /*
                   * Set 'cur_data_length' to the length of the data
                   * that the first PTE_MSG_DATA structure references.
                   */
                  cur_data_length = atol ( p_cur_msg_data20->data_length );

                  /*
                   * Because we are going to do addition to get data
                   * locations, use a pointer to a BYTE to determine
                   * the next PTE_MSG_DATA structure location in the
                   * PTE message.
                   */
                  p_cur_data = (pBYTE)p_cur_msg_data20;
            
                  /*
                   * Add to the current pointer, the current data's length
                   * and the size of the PTE_MSG_DATA header (minus one because
                   * the last BYTE of the PTE_MSG_DATA structure is where the
                   * data starts.
                   */
                  p_cur_data = p_cur_data + cur_data_length + PTE_MSG_DATA_HDR_SIZE20;  

                  p_cur_msg_data20 = (pPTE_MSG_DATA20)p_cur_data;

                  if ( p_cur_msg_data20 != NULL_PTR )
                  {
                     if ( p_cur_msg_data20->data_type == APPLICATION_DATA )
                     {
                        current_link++;
                     }

                  }  /* 'if ( p_cur_msg_data )' */
               }     /* while ( current_link < desired_link ) */
               break;

            default:
               /*
                * Get a pointer to the first PTE_MSG_DATA structure
                * in the message pointed to by 'p_pte_msg'.
                */
               p_cur_msg_data0 = (pPTE_MSG_DATA0)&p_pte_msg0->msg_data;

               current_link = 1;
               while ( current_link < desired_link )
               {
                  cur_data_length = atol ( p_cur_msg_data0->data_length );

                  /*
                   * Because we are going to do addition to get data
                   * locations, use a pointer to a BYTE to determine
                   * the next PTE_MSG_DATA structure location in the
                   * PTE message.
                   */
                  p_cur_data = (pBYTE)p_cur_msg_data0;
            
                  /*
                   * Add to the current pointer, the current data's length
                   * and the size of the PTE_MSG_DATA header (minus one because
                   * the last BYTE of the PTE_MSG_DATA structure is where the
                   * data starts.
                   */
                  p_cur_data = p_cur_data + cur_data_length + PTE_MSG_DATA_HDR_SIZE0;  

                  p_cur_msg_data0 = (pPTE_MSG_DATA0)p_cur_data;

                  if ( p_cur_msg_data0 != NULL_PTR )
                  {
                     if ( p_cur_msg_data0->data_type == APPLICATION_DATA )
                     {
                        current_link++;
                     }  

                  }  /* 'if ( p_cur_msg_data )' */
               }     /* while ( current_link < desired_link ) */
               break;

         } /* switch (PTE_MSG_VERSION) */

         switch (PTEMSG_MSG_VERSION)
         {
         case 20:
            p_cur_msg_data = (pPTE_MSG_DATA)p_cur_msg_data20;
            break;

         default:
            p_cur_msg_data = (pPTE_MSG_DATA)p_cur_msg_data0;
            break;
         }

         return ( p_cur_msg_data );

      }  /* 'if ( desired_link <= p_pte_msg->num_chained_data )' */

   }  /* 'if ( p_pte_msg )' */

   /* Return NULL_PTR */
   return ( p_cur_msg_data );
}



/******************************************************************************/
/******************************************************************************/
pPTE_MSG ptemsg_get_chained_pte_msg ( pPTE_MSG p_pte_msg,
                                      LONG     desired_message_num )
{
   pPTE_MSG0         p_pte_msg0;
   pPTE_MSG20        p_pte_msg20;
   pPTE_MSG_DATA0    p_cur_msg_data0   = NULL_PTR;
   pPTE_MSG_DATA20   p_cur_msg_data20  = NULL_PTR;
   BOOLEAN           data_available;

   pPTE_MSG_DATA     p_cur_msg_data    = NULL_PTR;
   pBYTE             p_cur_data;
   LONG              current_message_num,
                     cur_data_length;
   
   if ( p_pte_msg != NULL_PTR )
   {
      switch (PTEMSG_MSG_VERSION)
      {
      case 20:
         p_pte_msg20 = (pPTE_MSG20)p_pte_msg;
         data_available = (desired_message_num <= p_pte_msg20->num_chained_messages);
         break;

      default:
         p_pte_msg0 = (pPTE_MSG0)p_pte_msg;
         data_available = (desired_message_num <= p_pte_msg0->num_chained_messages);
         break;
      }

      /*
       * Make sure we're not looking for data that isn't there.
       */
      if ( data_available )
      {
         switch (PTEMSG_MSG_VERSION)
         {
         case 20:
            /*
             * Get a pointer to the first PTE_MSG_DATA structure
             * in the message pointed to by 'p_pte_msg'.
             */
            p_cur_msg_data20 = (pPTE_MSG_DATA20)&p_pte_msg20->msg_data;
         
            current_message_num = 1;
            while ( current_message_num < desired_message_num )
            {
               /*
                * Set 'cur_data_length' to the length of the data
                * that the first PTE_MSG_DATA structure references.
                */
               cur_data_length = atol ( p_cur_msg_data20->data_length );
            
               /*
                * Because we are going to do addition to get data
                * locations, use a pointer to a BYTE to determine
                * the next PTE_MSG_DATA structure location in the
                * PTE message.
                */
               p_cur_data = (pBYTE)p_cur_msg_data20;
            
               /*
                * Add to the current pointer, the current data's length
                * and the size of the PTE_MSG_DATA header (minus one because
                * the last BYTE of the PTE_MSG_DATA structure is where the
                * data starts.
                */
               p_cur_data = p_cur_data + cur_data_length + PTE_MSG_DATA_HDR_SIZE20;  

               p_cur_msg_data20 = (pPTE_MSG_DATA20)p_cur_data;

               if ( p_cur_msg_data20 != NULL_PTR )
               {
                  if ( p_cur_msg_data20->data_type == PTE_MESSAGE_DATA )
                  {
                     current_message_num++;
                  }
               }

            }  /* while ( current_message_num < desired_message_num ) */
            break;

         default:
            /*
             * Get a pointer to the first PTE_MSG_DATA structure
             * in the message pointed to by 'p_pte_msg'.
             */
            p_cur_msg_data0 = (pPTE_MSG_DATA0)&p_pte_msg0->msg_data;
         
            current_message_num = 1;
            while ( current_message_num < desired_message_num )
            {
               /*
                * Set 'cur_data_length' to the length of the data
                * that the first PTE_MSG_DATA structure references.
                */
               cur_data_length = atol ( p_cur_msg_data0->data_length );
            
               /*
                * Because we are going to do addition to get data
                * locations, use a pointer to a BYTE to determine
                * the next PTE_MSG_DATA structure location in the
                * PTE message.
                */
               p_cur_data = (pBYTE)p_cur_msg_data0;
            
               /*
                * Add to the current pointer, the current data's length
                * and the size of the PTE_MSG_DATA header (minus one because
                * the last BYTE of the PTE_MSG_DATA structure is where the
                * data starts.
                */
               p_cur_data = p_cur_data + cur_data_length + PTE_MSG_DATA_HDR_SIZE0;  

               p_cur_msg_data0 = (pPTE_MSG_DATA0)p_cur_data;

               if ( p_cur_msg_data0 != NULL_PTR )
               {
                  if ( p_cur_msg_data0->data_type == PTE_MESSAGE_DATA )
                  {
                     current_message_num++;
                  }
               }

            }  /* while ( current_message_num < desired_message_num ) */
            break;

         }     /* switch (PTE_MSG_VERSION */

      }  /* if ( desired_message_num <= p_pte_msg->num_chained_messages ) */


   }  /* if ( p_pte_msg ) */


   switch (PTEMSG_MSG_VERSION)
   {
   case 20:
      if ( p_cur_msg_data20 != NULL_PTR )
         return ( (pPTE_MSG)&p_cur_msg_data20->data );

   default:
      if ( p_cur_msg_data0 != NULL_PTR )
         return ( (pPTE_MSG)&p_cur_msg_data0->data );
   }

   return ( NULL_PTR );
}



/******************************************************************************/
/******************************************************************************/
BYTE ptemsg_get_pte_msg_data_data_type ( pPTE_MSG_DATA p_msg_data )
{
   pPTE_MSG_DATA0   p_msg_data0;
   pPTE_MSG_DATA20  p_msg_data20;

   if ( p_msg_data != NULL_PTR )
   {
      switch (PTEMSG_MSG_VERSION)
      {
      case 20:
         p_msg_data20 = (pPTE_MSG_DATA20)p_msg_data;
         return ( p_msg_data20->data_type );

      default:
         p_msg_data0 = (pPTE_MSG_DATA0)p_msg_data;
         return ( p_msg_data0->data_type );
      }
   }

   return ( 0 );
}



/******************************************************************************/
/******************************************************************************/
BYTE ptemsg_get_pte_msg_data_app_data_type ( pPTE_MSG_DATA p_msg_data )
{
   pPTE_MSG_DATA0   p_msg_data0;
   pPTE_MSG_DATA20  p_msg_data20;

   if ( p_msg_data != NULL_PTR )
   {
      switch (PTEMSG_MSG_VERSION)
      {
      case 20:
         p_msg_data20 = (pPTE_MSG_DATA20)p_msg_data;
         return ( p_msg_data20->app_data_type );

      default:
         p_msg_data0 = (pPTE_MSG_DATA0)p_msg_data;
         return ( p_msg_data0->app_data_type );
      }
   }

   return ( 0 );
}



/******************************************************************************/
/******************************************************************************/
LONG ptemsg_get_pte_msg_data_data_length ( pPTE_MSG_DATA p_msg_data )
{
   pPTE_MSG_DATA0   p_msg_data0;
   pPTE_MSG_DATA20  p_msg_data20;

   if ( p_msg_data != NULL_PTR )
   {
      switch (PTEMSG_MSG_VERSION)
      {
      case 20:
         p_msg_data20 = (pPTE_MSG_DATA20)p_msg_data;
         return ( atol ( p_msg_data20->data_length ) );

      default:
         p_msg_data0 = (pPTE_MSG_DATA0)p_msg_data;
         return ( atol ( p_msg_data0->data_length ) );
      }
   }

   return ( 0 );
}



/******************************************************************************/
/******************************************************************************/
pBYTE ptemsg_get_pte_msg_data_data ( pPTE_MSG_DATA p_msg_data )
{
   pPTE_MSG_DATA0   p_msg_data0;
   pPTE_MSG_DATA20  p_msg_data20;

   if ( p_msg_data != NULL_PTR )
   {
      switch (PTEMSG_MSG_VERSION)
      {
      case 20:
         p_msg_data20 = (pPTE_MSG_DATA20)p_msg_data;
         return ( (pBYTE)&p_msg_data20->data );

      default:
         p_msg_data0 = (pPTE_MSG_DATA0)p_msg_data;
         return ( (pBYTE)&p_msg_data0->data );
      }
   }

   return ( NULL_PTR );
}


/******************************************************************************

      Name:          ptemsg_set_msg_type()

      Return:        This function sets the value of the 'msg_type' field
                     in a PTE message.

******************************************************************************/
BOOLEAN ptemsg_set_msg_type ( pPTE_MSG p_pte_msg, BYTE msg_type )
{
   pPTE_MSG0   p_pte_msg0;
   pPTE_MSG20  p_pte_msg20;

   if ( p_pte_msg != NULL_PTR )
   {
      switch (PTEMSG_MSG_VERSION)
      {
      case 20:
         p_pte_msg20 = (pPTE_MSG20)p_pte_msg;
         p_pte_msg20->msg_type = msg_type;
         return ( true );

      default:
         p_pte_msg0 = (pPTE_MSG0)p_pte_msg;
         p_pte_msg0->msg_type = msg_type;
         return ( true );
      }
   }

   return ( false );
}



/******************************************************************************

      Name:          ptemsg_set_msg_subtype1()

      Return:        This function sets the value of the 'msg_subtype1' field
                     in a PTE message.

******************************************************************************/
BOOLEAN ptemsg_set_msg_subtype1 ( pPTE_MSG p_pte_msg, BYTE msg_subtype1 )
{
   pPTE_MSG0   p_pte_msg0;
   pPTE_MSG20  p_pte_msg20;

   if ( p_pte_msg != NULL_PTR )
   {
      switch (PTEMSG_MSG_VERSION)
      {
      case 20:
         p_pte_msg20 = (pPTE_MSG20)p_pte_msg;
         p_pte_msg20->msg_subtype1 = msg_subtype1;
         return ( true );

      default:
         p_pte_msg0 = (pPTE_MSG0)p_pte_msg;
         p_pte_msg0->msg_subtype1 = msg_subtype1;
         return ( true );
      }
   }

   return ( false );
}


/******************************************************************************

      Name:          ptemsg_set_msg_subtype2()

      Return:        This function sets the value of the 'msg_subtype2' field
                     in a PTE message.

 ******************************************************************************/

BOOLEAN ptemsg_set_msg_subtype2 ( pPTE_MSG p_pte_msg, BYTE msg_subtype2 )
{
   pPTE_MSG0   p_pte_msg0;
   pPTE_MSG20  p_pte_msg20;

   if ( p_pte_msg != NULL_PTR )
   {
      switch (PTEMSG_MSG_VERSION)
      {
      case 20:
         p_pte_msg20 = (pPTE_MSG20)p_pte_msg;
         p_pte_msg20->msg_subtype2 = msg_subtype2;
         return ( true );

      default:
         p_pte_msg0 = (pPTE_MSG0)p_pte_msg;
         p_pte_msg0->msg_subtype2 = msg_subtype2;
         return ( true );
      }
   }

   return ( false );
}


/******************************************************************************

      Name:          ptemsg_set_dest_queue()

      Return:        This function sets the value of the 'dest_queue' field
                     in a PTE message.

******************************************************************************/
BOOLEAN ptemsg_set_dest_queue ( pPTE_MSG p_pte_msg, pCHAR queue_name )
{
   pPTE_MSG0   p_pte_msg0;
   pPTE_MSG20  p_pte_msg20;

   if ( p_pte_msg != NULL_PTR )
   {
      switch (PTEMSG_MSG_VERSION)
      {
      case 20:
         p_pte_msg20 = (pPTE_MSG20)p_pte_msg;
         ptecopy_strncpy (p_pte_msg20->dest_queue, queue_name, sizeof(p_pte_msg20->dest_queue));
         return ( true );

      default:
         p_pte_msg0 = (pPTE_MSG0)p_pte_msg;
         ptecopy_strncpy (p_pte_msg0->dest_queue, queue_name, sizeof(p_pte_msg0->dest_queue));
         return ( true );
      }
   }

   return ( false );
}



/******************************************************************************

      Name:          ptemsg_set_orig_queue()

      Return:        This function sets the value of the 'orig_queue' field
                     in a PTE message.

******************************************************************************/
BOOLEAN ptemsg_set_orig_queue ( pPTE_MSG p_pte_msg, pCHAR queue_name )
{
   pPTE_MSG0   p_pte_msg0;
   pPTE_MSG20  p_pte_msg20;

   if ( p_pte_msg != NULL_PTR )
   {
      switch (PTEMSG_MSG_VERSION)
      {
      case 20:
         p_pte_msg20 = (pPTE_MSG20)p_pte_msg;
         ptecopy_strncpy (p_pte_msg20->orig_queue, queue_name, sizeof(p_pte_msg20->orig_queue));
         return ( true );

      default:
         p_pte_msg0 = (pPTE_MSG0)p_pte_msg;
         ptecopy_strncpy (p_pte_msg0->orig_queue, queue_name, sizeof(p_pte_msg0->orig_queue));
         return ( true );
      }
   }

   return ( false );
}



/******************************************************************************

      Name:          ptemsg_set_result_code()

      Description:   This function sets the value of the
                     'result_code' field in a PTE message.

      Return:        No return.

******************************************************************************/
BOOLEAN ptemsg_set_result_code ( pPTE_MSG p_pte_msg, BYTE result_code )
{
   pPTE_MSG0   p_pte_msg0;
   pPTE_MSG20  p_pte_msg20;

   if ( p_pte_msg != NULL_PTR )
   {
      switch (PTEMSG_MSG_VERSION)
      {
      case 20:
         p_pte_msg20 = (pPTE_MSG20)p_pte_msg;
         p_pte_msg20->result_code = result_code;
         return ( true );

      default:
         p_pte_msg0 = (pPTE_MSG0)p_pte_msg;
         p_pte_msg0->result_code = result_code;
         return ( true );
      }
   }

   return ( false );
}


/******************************************************************************

      Name:          ptemsg_get_result_code ()

      Description:   This function returns the value of the
                     'result_code' field in a PTE message.

      Return:        Returns the value of the 'result_code' field
                     in the PTE_MSG pointed to by p_pte_msg.

 ******************************************************************************/

BYTE ptemsg_get_result_code ( pPTE_MSG p_pte_msg )
{
   pPTE_MSG0   p_pte_msg0;
   pPTE_MSG20  p_pte_msg20;

   if ( p_pte_msg != NULL_PTR )
   {
      switch (PTEMSG_MSG_VERSION)
      {
      case 20:
         p_pte_msg20 = (pPTE_MSG20)p_pte_msg;
         return ( p_pte_msg20->result_code );

      default:
         p_pte_msg0 = (pPTE_MSG0)p_pte_msg;
         return ( p_pte_msg0->result_code );
      }
   }

   return (0);
}



/******************************************************************************

      Name:          ptemsg_get_current_msg_tran_id ()

      Return:        This function sets the value of the 'dest_queue' field
                     in a PTE message.

******************************************************************************/
pCHAR ptemsg_get_current_msg_tran_id ( pPTE_MSG p_pte_msg )
{
   pPTE_MSG0   p_pte_msg0;
   pPTE_MSG20  p_pte_msg20;

   if ( p_pte_msg != NULL_PTR )
   {
      switch (PTEMSG_MSG_VERSION)
      {
      case 20:
         p_pte_msg20 = (pPTE_MSG20)p_pte_msg;
         return ( p_pte_msg20->current_tran_id );

      default:
         p_pte_msg0 = (pPTE_MSG0)p_pte_msg;
         return ( p_pte_msg0->current_tran_id );
      }
   }

   return (NULL_PTR);
}



/******************************************************************************

      Name:          ptemsg_set_current_msg_tran_id ()

      Return:        This function sets the value of the 'dest_queue' field
                     in a PTE message.

******************************************************************************/
BOOLEAN ptemsg_set_current_msg_tran_id (  pPTE_MSG p_pte_msg,
                                          pCHAR    tran_id    )
{
   pPTE_MSG0   p_pte_msg0;
   pPTE_MSG20  p_pte_msg20;

   if ( p_pte_msg != NULL_PTR )
   {
      switch (PTEMSG_MSG_VERSION)
      {
      case 20:
         p_pte_msg20 = (pPTE_MSG20)p_pte_msg;
         ptecopy_strncpy (p_pte_msg20->current_tran_id, tran_id, sizeof(p_pte_msg20->current_tran_id));
         return ( true );

      default:
         p_pte_msg0 = (pPTE_MSG0)p_pte_msg;
         ptecopy_strncpy (p_pte_msg0->current_tran_id, tran_id, sizeof(p_pte_msg0->current_tran_id));
         return ( true );
      }
   }

   return (false);
}



/******************************************************************************

      Name:          ptemsg_get_last_msg_tran_id ()

      Return:        This function sets the value of the 'dest_queue' field
                     in a PTE message.

******************************************************************************/
pCHAR ptemsg_get_last_msg_tran_id ( pPTE_MSG p_pte_msg )
{
   pPTE_MSG0   p_pte_msg0;
   pPTE_MSG20  p_pte_msg20;

   if ( p_pte_msg != NULL_PTR )
   {
      switch (PTEMSG_MSG_VERSION)
      {
      case 20:
         p_pte_msg20 = (pPTE_MSG20)p_pte_msg;
         return ( p_pte_msg20->last_tran_id );

      default:
         p_pte_msg0 = (pPTE_MSG0)p_pte_msg;
         return ( p_pte_msg0->last_tran_id );
      }
   }

   return (NULL_PTR);
}



/******************************************************************************

      Name:          ptemsg_set_last_msg_tran_id ()

      Return:        This function sets the value of the 'dest_queue' field
                     in a PTE message.

******************************************************************************/
BOOLEAN ptemsg_set_last_msg_tran_id ( pPTE_MSG p_pte_msg,
                                      pCHAR    tran_id    )
{
   pPTE_MSG0   p_pte_msg0;
   pPTE_MSG20  p_pte_msg20;

   if ( p_pte_msg != NULL_PTR )
   {
      switch (PTEMSG_MSG_VERSION)
      {
      case 20:
         p_pte_msg20 = (pPTE_MSG20)p_pte_msg;
         ptecopy_strncpy (p_pte_msg20->last_tran_id, tran_id, sizeof(p_pte_msg20->last_tran_id));
         return ( true );

      default:
         p_pte_msg0 = (pPTE_MSG0)p_pte_msg;
         ptecopy_strncpy (p_pte_msg0->last_tran_id, tran_id, sizeof(p_pte_msg0->last_tran_id));
         return ( true );
      }
   }

   return (false);
}



/*******************************************************************************
*
*
*
*
*
*******************************************************************************/
pCHAR ptemsg_get_msg_orig_comms_info ( pPTE_MSG p_pte_msg )
{
   pPTE_MSG0   p_pte_msg0;
   pPTE_MSG20  p_pte_msg20;

   if ( p_pte_msg != NULL_PTR )
   {
      switch (PTEMSG_MSG_VERSION)
      {
      case 20:
         p_pte_msg20 = (pPTE_MSG20)p_pte_msg;
         return ( p_pte_msg20->orig_comms_info );

      default:
         p_pte_msg0 = (pPTE_MSG0)p_pte_msg;
         return ( p_pte_msg0->orig_comms_info );
      }
   }

   return (NULL_PTR);
}


/*******************************************************************************
*
*
*
*
*
*******************************************************************************/
BOOLEAN ptemsg_set_orig_comms_info ( pPTE_MSG p_pte_msg,
                                     pCHAR    comms_info )
{
   pPTE_MSG0   p_pte_msg0;
   pPTE_MSG20  p_pte_msg20;

   if ( p_pte_msg != NULL_PTR )
   {
      switch (PTEMSG_MSG_VERSION)
      {
      case 20:
         p_pte_msg20 = (pPTE_MSG20)p_pte_msg;
         ptecopy_strncpy (p_pte_msg20->orig_comms_info, comms_info, sizeof(p_pte_msg20->orig_comms_info));
         return ( true );

      default:
         p_pte_msg0 = (pPTE_MSG0)p_pte_msg;
         ptecopy_strncpy (p_pte_msg0->orig_comms_info, comms_info, sizeof(p_pte_msg0->orig_comms_info));
         return ( true );
      }
   }

   return (false);
}



/******************************************************************************

      Name:          ptemsg_set_type_of_reply_needed()

      Description:   This function sets the value of the
                     'type_of_reply_needed' field in a PTE message.

      Return:        No return.

******************************************************************************/
BOOLEAN ptemsg_set_type_of_reply_needed ( pPTE_MSG p_pte_msg, 
                                          BYTE     type_of_reply_needed )
{
   pPTE_MSG20  p_pte_msg20;

   if ( p_pte_msg != NULL_PTR )
   {
      switch (PTEMSG_MSG_VERSION)
      {
      case 20:
         p_pte_msg20 = (pPTE_MSG20)p_pte_msg;
         p_pte_msg20->type_of_reply_needed = type_of_reply_needed;
         return ( true );

      default:
         break;
      }
   }

   return (false);
}



/******************************************************************************

      Name:          ptemsg_get_type_of_reply_needed ()

      Description:   This function returns the value of the
                     'type_of_reply_needed' field in a PTE message.

      Return:        Returns the value of the 'type_of_reply_needed' field
                     in the PTE_MSG pointed to by p_pte_msg.

******************************************************************************/
BYTE ptemsg_get_type_of_reply_needed ( pPTE_MSG p_pte_msg )
{
   pPTE_MSG20  p_pte_msg20;

   if ( p_pte_msg != NULL_PTR )
   {
      switch (PTEMSG_MSG_VERSION)
      {
      case 20:
         p_pte_msg20 = (pPTE_MSG20)p_pte_msg;
         return (p_pte_msg20->type_of_reply_needed);

      default:
         break;
      }
   }

   return (0);
}



/******************************************************************************/
/******************************************************************************/
pCHAR ptemsg_get_msg_orig_system ( pPTE_MSG p_pte_msg )
{
   pPTE_MSG20  p_pte_msg20;

   if ( p_pte_msg != NULL_PTR )
   {
      switch (PTEMSG_MSG_VERSION)
      {
      case 20:
         p_pte_msg20 = (pPTE_MSG20)p_pte_msg;
         return (p_pte_msg20->orig_system);

      default:
         break;
      }
   }

   return ( NULL_PTR );
}



/******************************************************************************

      Name:          ptemsg_set_orig_queue()

      Return:        This function sets the value of the 'orig_queue' field
                     in a PTE message.

******************************************************************************/
BOOLEAN ptemsg_set_orig_system ( pPTE_MSG p_pte_msg, 
                                 pCHAR    system_name )
{
   pPTE_MSG20  p_pte_msg20;

   if ( p_pte_msg != NULL_PTR )
   {
      switch (PTEMSG_MSG_VERSION)
      {
      case 20:
         p_pte_msg20 = (pPTE_MSG20)p_pte_msg;
         ptecopy_strncpy (p_pte_msg20->orig_system, system_name, sizeof(p_pte_msg20->orig_system));
         return ( true );

      default:
         break;
      }
   }

   return ( false );
}



/******************************************************************************

      Name:          ptemsg_get_log_flag ()

      Description:   This function returns the value of the
                     'log_flag' field in a PTE message.

      Return:        Returns the value of the 'log_flag' field
                     in the PTE_MSG pointed to by p_pte_msg.

******************************************************************************/
BYTE ptemsg_get_log_flag ( pPTE_MSG p_pte_msg )
{
   pPTE_MSG20  p_pte_msg20;

   if ( p_pte_msg != NULL_PTR )
   {
      switch (PTEMSG_MSG_VERSION)
      {
      case 20:
         p_pte_msg20 = (pPTE_MSG20)p_pte_msg;
         return (p_pte_msg20->log_flag);

      default:
         break;
      }
   }

   return (0);
}



/******************************************************************************

      Name:          ptemsg_set_log_flag()

      Description:   This function sets the value of the
                     'log_flag' field in a PTE message.

      Return:        No return.

******************************************************************************/
BOOLEAN ptemsg_set_log_flag ( pPTE_MSG p_pte_msg, 
                              BYTE     log_flag   )
{
   pPTE_MSG20  p_pte_msg20;

   if ( p_pte_msg != NULL_PTR )
   {
      switch (PTEMSG_MSG_VERSION)
      {
      case 20:
         p_pte_msg20 = (pPTE_MSG20)p_pte_msg;
         p_pte_msg20->log_flag = log_flag;
         return ( true );

      default:
         break;
      }
   }

   return (false);
}


