/******************************************************************************

        Module: ptecopy.h
  
         Title: PTE General Utility Routine
  
   Description:

  $Log:   N:\PVCS\PTE\SYSTEM\PTECOPY.H  $
   
      Rev 1.0   Jan 08 1998 09:59:18   drodden
   Changed ptecopy to include the string header for the needed prototypes.
   Also created a header and prototypes for ptecopy.
   
   
******************************************************************************/

#ifndef PTECOPY_H
#define PTECOPY_H

#include "basictyp.h"


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
                       LONG  dest_size );



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
                        LONG  dest_size );


#endif
