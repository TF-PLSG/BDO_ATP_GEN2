/******************************************************************************

        Module: ptellist.h
  
         Title: Link List APIs
  
   Description: This file contains definitions and structures used by the
                routines in ptellist.c to build, access, and manipulate the
                linked lists of data required to process transactions.

   $Log:   N:\PVCS\PTE\SYSTEM\PTELLIST.H  $
   
      Rev 1.5   Dec 17 1997 16:21:54   drodden
   Removed the last of the Passport fields (scroll id, field id, etc) from 
   the link list records and removed the APIs that manipulated them.
   
   
      Rev 1.4   Dec 16 1997 12:18:28   drodden
   Changed the delete function to return the "next" element following
   the one being deleted.
   
   
      Rev 1.3   Oct 23 1997 11:42:24   drodden
   This is part of the continued development of the Pinnacle 
   Transaction Environment.  Many changes, refinements,
   and additions were made to the core modules.  I'm checking
   them in now to get a snapshot of this current working version.
   
   
      Rev 1.2   26 Sep 1997 11:08:48   mvandenb
   Continued development.
   

      Rev 1.1   Sep 22 1997 09:32:18   drodden
   Continued work on the linked list module: 
     - new APIs and new list fields.
   
   
      Rev 1.0   15 Sep 1997 16:06:54   mvandenb
   Initial versions of Data Element Linked List structures.


******************************************************************************/

#ifndef  PTELLIST
#define  PTELLIST

#include "basictyp.h"


typedef struct LIST_ELEMENT
{
   struct LIST_ELEMENT  *p_next_element;
   struct LIST_ELEMENT  *p_prev_element;
   char  tran_id [MAX_TRAN_ID_SIZE];
   LONG  data_size;
   BYTE  update_type;
   BYTE  element_data;        /* must be last field in this struct */
}*pLIST_ELEMENT;


typedef struct
{
   pLIST_ELEMENT  p_first;
   pLIST_ELEMENT  p_last;
   LONG           num_elements;
   void           (*c2db_conv)(void *C_rec);    /* ptr to function to convert C structure to DB structure */
   void           (*db2c_conv)(void *C_rec);    /* ptr to function to convert DB structure to C structure */
   LONG           key1_offset;
   LONG           key2_offset;
} LINKED_LIST, *pLINKED_LIST;




void ptellist_init_list                 ( pLINKED_LIST  p_list );


void ptellist_free_list_memory          ( pLINKED_LIST  p_list );


pLIST_ELEMENT ptellist_first_element    ( pLINKED_LIST  p_list );


pLIST_ELEMENT ptellist_last_element     ( pLINKED_LIST  p_list );


pLIST_ELEMENT ptellist_goto_element     ( pLINKED_LIST  p_list,
                                          LONG          element_number );


pLIST_ELEMENT ptellist_next_element     ( pLIST_ELEMENT p_element );


pLIST_ELEMENT ptellist_prev_element     ( pLIST_ELEMENT p_element );


LONG ptellist_num_elements              ( pLINKED_LIST  p_list );


LONG ptellist_key1_offset               ( pLINKED_LIST p_list );


void ptellist_set_key1_offset           ( pLINKED_LIST p_list,
                                          LONG         key1_offset );


LONG ptellist_key2_offset               ( pLINKED_LIST p_list );


void ptellist_set_key2_offset           ( pLINKED_LIST p_list,
                                          LONG         key2_offset );


pLIST_ELEMENT ptellist_append_element   ( pLINKED_LIST  p_list,
                                          pBYTE         p_data,
                                          LONG          data_size );


pLIST_ELEMENT ptellist_insert_element   ( pLINKED_LIST  p_list,
                                          pLIST_ELEMENT p_cur_elem,
                                          pBYTE         p_data,
                                          LONG          data_size   );


pLIST_ELEMENT ptellist_delete_element   ( pLINKED_LIST  p_list,
                                          pLIST_ELEMENT p_cur_elem );


pLIST_ELEMENT ptellist_update_element   ( pLIST_ELEMENT p_cur_elem,
                                          pBYTE         p_data,
                                          LONG          data_size   );


pBYTE ptellist_element_data             ( pLIST_ELEMENT p_element );


LONG ptellist_element_data_size         ( pLIST_ELEMENT p_element );


#endif
