/******************************************************************************

        Module: ptellist.c
  
         Title: Link List APIs
  
   Description: This file contains definitions and structures used by the
                routines in ptellist.c to build, access, and manipulate the
                linked lists of data required to process transactions.

   $Log:   N:\PVCS\PTE\SYSTEM\PTELLIST.C  $
   
      Rev 1.5   Dec 17 1997 16:21:54   drodden
   Removed the last of the Passport fields (scroll id, field id, etc) from 
   the link list records and removed the APIs that manipulated them.
   
   
      Rev 1.4   Dec 16 1997 12:18:28   drodden
   Changed the delete function to return the "next" element following
   the one being deleted.
   
   
      Rev 1.3   Dec 01 1997 08:52:12   drodden
   Added the string header to eliminate compile warnings.
   
   
      Rev 1.2   Oct 23 1997 11:41:56   drodden
   This is part of the continued development of the Pinnacle 
   Transaction Environment.  Many changes, refinements,
   and additions were made to the core modules.  I'm checking
   them in now to get a snapshot of this current working version.
   
   
      Rev 1.1   26 Sep 1997 11:10:34   mvandenb
   Continued development of APIs for linked list manipulation.
   
      Rev 1.0   Sep 22 1997 09:32:18   drodden
   Continued work on the linked list module: 
     - new APIs and new list fields.
   

*******************************************************************************/

#include <stdlib.h>
#include <string.h>

#include "ptellist.h"



/*****************************************************************************
*
*  This function will 
*
*  Paramaters:
*
*
******************************************************************************/
void ptellist_init_list ( pLINKED_LIST p_list )
{
   if (p_list != NULL_PTR)
   {
      p_list->num_elements = 0;
      p_list->p_first      = NULL_PTR;
      p_list->p_last       = NULL_PTR;
   }
}



/*****************************************************************************
*
*  This function will 
*
*  Paramaters:
*     
*
******************************************************************************/
void ptellist_free_list_memory ( pLINKED_LIST p_list )
{
   pLIST_ELEMENT  p_cur;
   pLIST_ELEMENT  p_next;


   p_cur = p_list->p_first;

   while (p_cur != NULL_PTR)
   {
      p_next = p_cur->p_next_element;
   
      free (p_cur);

      p_cur = p_next;
   }

   ptellist_init_list (p_list);
}



/*****************************************************************************
*
*  This function will 
*
*  Paramaters:
*
*
******************************************************************************/
pLIST_ELEMENT ptellist_first_element ( pLINKED_LIST p_list )
{
   if (p_list != NULL_PTR)
      return (p_list->p_first);

   return (NULL_PTR);
}



/*****************************************************************************
*
*  This function will 
*
*  Paramaters:
*
*
******************************************************************************/
pLIST_ELEMENT ptellist_last_element ( pLINKED_LIST p_list )
{
   if (p_list != NULL_PTR)
      return (p_list->p_last);

   return (NULL_PTR);
}



/*****************************************************************************
*
*  This function will 
*
*  Paramaters:
*     
*
******************************************************************************/
pLIST_ELEMENT ptellist_goto_element ( pLINKED_LIST p_list,
                                      LONG         element_number )
{
   pLIST_ELEMENT  p_element;
   LONG           index;


   p_element = NULL_PTR;

   if (p_list != NULL_PTR)
   {
      p_element = p_list->p_first;
      
      for (index=1; index < element_number; index++)
      {
         if (p_element == NULL_PTR)
            break;

         p_element = p_element->p_next_element;
      }
   }

   return ( p_element );
}



/*****************************************************************************
*
*  This function will 
*
*  Paramaters:
*     
*
******************************************************************************/
pLIST_ELEMENT ptellist_next_element ( pLIST_ELEMENT p_element )
{

   if ( p_element != NULL_PTR )
      return ( p_element->p_next_element );

   return ( NULL_PTR );
}



/*****************************************************************************
*
*  This function will 
*
*  Paramaters:
*     
*
******************************************************************************/
pLIST_ELEMENT ptellist_prev_element ( pLIST_ELEMENT p_element )
{

   if ( p_element != NULL_PTR )
      return ( p_element->p_prev_element );

   return ( NULL_PTR );
}



/*****************************************************************************
*
*  This function will 
*
*  Paramaters:
*     
*
******************************************************************************/
LONG ptellist_num_elements ( pLINKED_LIST p_list )
{
   if (p_list != NULL_PTR)
      return (p_list->num_elements);

   return (0);
}



/*****************************************************************************
*
*  This function will 
*
*  Paramaters:
*     
*
******************************************************************************/
LONG ptellist_key1_offset ( pLINKED_LIST p_list )
{
   if (p_list != NULL_PTR)
      return ( p_list->key1_offset );

   return (0);
}



/*****************************************************************************
*
*  This function will 
*
*  Paramaters:
*     
*
******************************************************************************/
void ptellist_set_key1_offset ( pLINKED_LIST p_list,
                                LONG         key1_offset )
{
   if (p_list != NULL_PTR)
      p_list->key1_offset = key1_offset;
}



/*****************************************************************************
*
*  This function will 
*
*  Paramaters:
*     
*
******************************************************************************/
LONG ptellist_key2_offset ( pLINKED_LIST p_list )
{
   if (p_list != NULL_PTR)
      return ( p_list->key2_offset );

   return (0);
}



/*****************************************************************************
*
*  This function will 
*
*  Paramaters:
*     
*
******************************************************************************/
void ptellist_set_key2_offset ( pLINKED_LIST p_list,
                                LONG         key2_offset )
{
   if (p_list != NULL_PTR)
      p_list->key2_offset = key2_offset;
}



/*****************************************************************************
*
*  This function will append a new element to the end of the linked list.
*
*  Paramaters:
*     p_list........pointer to the linked list to append the new element to.
*     p_data........pointer to some data (user defineable/app specific).
*     data_size.....number of bytes of data at the p_data location.
*
******************************************************************************/
pLIST_ELEMENT ptellist_allocate_element ( pBYTE p_data,
                                          LONG  data_size )
{
   pLIST_ELEMENT  p_element;


   p_element = (pLIST_ELEMENT) malloc (sizeof(struct LIST_ELEMENT) - 1 + data_size);

   if ( p_element != NULL_PTR )
   {
      /*
       * Null fill the fixed portion of the element.
       */
      memset (p_element, 0, sizeof(struct LIST_ELEMENT));

      /*
       * Copy the data for this element to the newly allocated storage.  It will
       * overlay the storage starting with the byte element_data.
       */
      p_element->data_size = data_size;
      memcpy (&p_element->element_data, p_data, data_size);
   }

   return ( p_element );
}



/*****************************************************************************
*
*  This function will append a new element to the end of the linked list.
*
*  Paramaters:
*     p_list........pointer to the linked list to append the new element to.
*     p_data........pointer to some data (user defineable/app specific).
*     data_size.....number of bytes of data at the p_data location.
*
******************************************************************************/
pLIST_ELEMENT ptellist_append_element ( pLINKED_LIST p_list,
                                        pBYTE        p_data,
                                        LONG         data_size )
{
   pLIST_ELEMENT  p_new_elem;
   pLIST_ELEMENT  p_prev_elem;


   if (p_list == NULL_PTR)
      return (NULL_PTR);

   p_new_elem = ptellist_allocate_element ( p_data, data_size );

   if (p_new_elem != NULL_PTR)
   {
      /*
       * Set the pointers for the linked list and the element itself.
       */
      if ( p_list->p_first == NULL_PTR )
      {
         p_list->p_first = p_new_elem;
         p_list->p_last  = p_new_elem;
      }
      else
      {
         p_prev_elem                 = p_list->p_last;
         p_new_elem->p_prev_element  = p_prev_elem;
         p_prev_elem->p_next_element = p_new_elem;
         p_list->p_last              = p_new_elem;
      }

      p_list->num_elements++;
   }

   return ( p_new_elem );
}



/*****************************************************************************
*
*  This function will 
*
*  Paramaters:
*     
*
******************************************************************************/
pLIST_ELEMENT ptellist_insert_element ( pLINKED_LIST  p_list,
                                        pLIST_ELEMENT p_cur_elem,
                                        pBYTE         p_data,
                                        LONG          data_size   )
 {
   pLIST_ELEMENT  p_new_elem;
   pLIST_ELEMENT  p_prev_elem;


   if (p_list == NULL_PTR)
      return (NULL_PTR);

   if (p_cur_elem == NULL_PTR)
   {
      /*
       * I don't have an element to insert in front of.  Just append it.
       */
      p_new_elem = ptellist_append_element ( p_list, p_data, data_size );
   }
   else
   {
      p_new_elem = ptellist_allocate_element ( p_data, data_size );

      if (p_new_elem != NULL_PTR)
      {
         /*
          * Set the pointers to insert the new element in front of the current element:
          *    - if there is a previous element, set the next-pointer
          *      of the previous element to this new element.
          *    - set the previous-pointer of the current element to this new element.
          *    - set the next-pointer of the new element to the current element.
          *    - set the previous-pointer the new element to the previous element.
          *    - if the current element is the first element, the new element
          *      becomes the first element.
          */
         p_prev_elem = p_cur_elem->p_prev_element;

         if (p_prev_elem != NULL_PTR)
            p_prev_elem->p_next_element = p_new_elem;

         p_cur_elem->p_prev_element  = p_new_elem;
         p_new_elem->p_next_element  = p_cur_elem;
         p_new_elem->p_prev_element  = p_prev_elem;

         if ( p_list->p_first == p_cur_elem )
            p_list->p_first = p_new_elem;

         p_list->num_elements++;
      }
   }

   return ( p_new_elem );
}



/*****************************************************************************
*
*  This function will 
*
*  Paramaters:
*     
*
******************************************************************************/
pLIST_ELEMENT ptellist_delete_element ( pLINKED_LIST  p_list,
                                        pLIST_ELEMENT p_cur_elem )
{
   pLIST_ELEMENT  p_prev_elem;
   pLIST_ELEMENT  p_next_elem;


   if ( (p_list     == NULL_PTR) ||
        (p_cur_elem == NULL_PTR)    )
      return (NULL_PTR);

   /*
    * Set the pointers to delete element in front of the current element:
    *    - if there is a previous element, set the next-pointer
    *      of the previous element to the element after the current one.
    *    - if there is a next element, set the previous-pointer of the
    *      next element to the previous element.
    *    - if the current element is the first element, the next element
    *      becomes the first element.
    *    - if the current element is the last element, the previous element
    *      becomes the last element.
    */
   p_prev_elem = p_cur_elem->p_prev_element;
   p_next_elem = p_cur_elem->p_next_element;

   if (p_prev_elem != NULL_PTR)
      p_prev_elem->p_next_element = p_next_elem;

   if (p_next_elem != NULL_PTR)
      p_next_elem->p_prev_element = p_prev_elem;

   if (p_list->p_first == p_cur_elem)
      p_list->p_first = p_next_elem;

   if (p_list->p_last == p_cur_elem)
      p_list->p_last = p_prev_elem;

   p_list->num_elements--;

   free (p_cur_elem);

   return (p_next_elem);
}



/*****************************************************************************
*
*  This function will 
*
*  Paramaters:
*     
*
******************************************************************************/
pLIST_ELEMENT ptellist_update_element ( pLIST_ELEMENT p_cur_elem,
                                        pBYTE         p_data,
                                        LONG          data_size   )
{
   pLIST_ELEMENT  p_new_elem;


   p_new_elem = p_cur_elem;

   if (p_cur_elem != NULL_PTR)
   {
      if (p_cur_elem->data_size != data_size)
         p_new_elem = (pLIST_ELEMENT) realloc (p_cur_elem, sizeof(struct LIST_ELEMENT) - 1 + data_size);

      if (p_new_elem != NULL_PTR)
      {
         /*
          * Copy the data for this list element to the newly allocated storage.
          */
         p_new_elem->data_size = data_size;
         memcpy (&p_new_elem->element_data, p_data, data_size);
      }
   }

   return (p_new_elem);
}



/*****************************************************************************
*
*  This function will 
*
*  Paramaters:
*     
*
******************************************************************************/
pBYTE ptellist_element_data ( pLIST_ELEMENT p_element )
{
   if (p_element != NULL_PTR)
      return ( (pBYTE)&p_element->element_data );

   return ( NULL_PTR );
}



/*****************************************************************************
*
*  This function will 
*
*  Paramaters:
*     
*
******************************************************************************/
LONG ptellist_element_data_size ( pLIST_ELEMENT p_element )
{

   if (p_element != NULL_PTR)
      return (p_element->data_size);

   return (0);
}



