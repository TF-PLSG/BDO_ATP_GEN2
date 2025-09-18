/******************************************************************************

        Module: ptecopy.c
  
         Title: PTE General Utility Routine
  
   Description:

  $Log:   N:\PVCS\PTE\SYSTEM\PTECOPY.C  $
   
      Rev 1.3   Jan 08 1998 09:59:12   drodden
   Changed ptecopy to include the string header for the needed prototypes.
   Also created a header and prototypes for ptecopy.
   
   
      Rev 1.2   12 Dec 1997 11:22:52   mvandenb
   Changed function names to ptecopy_memcpy and
   added a function, ptecopy_strncpy().

  
      Rev 1.1   25 Sep 1997 12:42:18   mvandenb
   Changed return from void to void *.  The pointer returned is
   a pointer to the destination string.

  
      Rev 1.0   19 Sep 1997 15:55:38   mvandenb
   Initial version.
   
******************************************************************************/

#include <stdlib.h>
#include <string.h>

#include "ptecopy.h"



/******************************************************************************
 *
 *  FUNCTION:       ptecopy_memcpy ();
 *
 *  DESCRIPTION:    This function copies bytes from the source location to the
 *                  destination location while limiting the number of bytes
 *                  copied to the maximum length of the destination location.
 *
 *  PARAMETERS:     pBYTE   p_dest      -   pointer to destination location.
 *                  pBYTE   p_src       -   pointer to source location.
 *                  INT     dest_size   -   size of destination location.
 *
 *  RETURN:         pBYTE - Pointer to the destination location or NULL_PTR.
 *
 ******************************************************************************/
pBYTE ptecopy_memcpy ( pBYTE p_dest, 
                       pBYTE p_src, 
                       LONG  dest_size )
{

    /*
     *  Make sure that the source and destination
     *  pointers actually contain pointer values.
     */
    if ( p_dest && p_src )
    {
        /*
         *  Initialize the destination location to 0's.
         */
        memset ( p_dest, 0, dest_size );
   
        /*
         *  Don't copy any more than destination size bytes.
         */
        memcpy ( p_dest, p_src, dest_size );

        return ( p_dest );
    }
   
    /*
     *  Return a NULL_PTR if one of the pointers
     *  passed in as parameters was invalid.
     */
    return ( NULL_PTR );
}



/******************************************************************************
 *
 *  FUNCTION:       ptecopy_strncpy ();
 *
 *  DESCRIPTION:    This function copies a source string to a destination
 *                  string while limiting the number of characters copied
 *                  to the maximum length of the destination string.  It also
 *                  makes sure that the destination string is NULL terminated.
 *
 *  PARAMETERS:     pBYTE   p_dest      -   pointer to destination string.
 *                  pBYTE   p_src       -   pointer to source string.
 *                  INT     dest_size   -   size of destination array.
 *
 *  RETURN:         pCHAR - Pointer to NULL terminated string or pointer to
 *                          NULL string.
 *
 ******************************************************************************/
pCHAR ptecopy_strncpy ( pCHAR p_dest, 
                        pCHAR p_src, 
                        LONG  dest_size )
{

    /*
     *  Make sure that the source and destination
     *  pointers actually contain pointer values.
     */
    if ( p_dest && p_src )
    {
        /*
         *  Don't copy any more than (destination size - 1) bytes.  This
         *  insures that the return string is NULL terminated.
         *  
         *  NOTE:
         *      strncpy() will pad the destination string with
         *      zeros if it is longer that the source string.
         */
        strncpy (p_dest, p_src, dest_size - 1);
        p_dest [dest_size - 1] = '\0'; 
        return ( p_dest );
    }
   
    /*
     *  Return a NULL_STR if one of the pointers
     *  passed in as parameters was invalid.
     */
    return ( NULL_PTR );
}


