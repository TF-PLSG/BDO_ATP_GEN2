/******************************************************************************

        Module: ptehash.h
  
         Title: 
  
   Description: 

   $Log:   N:\PVCS\PTE\CORE\PTE_LIB\ptehash.h  $
   
      Rev 1.0   Dec 03 1998 10:57:42   drodden
   Data caching changes.
   

******************************************************************************/

#ifndef PTEHASH_H
#define PTEHASH_H

#include "basictyp.h"


#define PTEHASH_MAX_PER_CLUSTER  10


/*******************************************************************************
*  function ptehash_calc 
*
*  This function generates a hash index from the characters of the string 
*  passed in.  It stops (does not include) when it reaches the null 
*  terminator.  This is a general-purpose algorithm used in some UNIX operating 
*  systems.
*
*  Parameters:
*     p_data_str.....pointer to a null terminated string to compute the
*                    hash index for.
*
*******************************************************************************/
DWORD ptehash_calc ( pBYTE p_data_str );



/*******************************************************************************
*  function ptehash_lcalc 
*
*  This function generates a hash index the same as the previous routine, but
*  it doesn't look for a null terminator.  It just computes for the number of
*  characters specified.
*
*  Parameters:
*     p_buffer.......pointer to the buffer to compute the hash index for.
*     buffer_len.....number of characters to compute on.
*
*******************************************************************************/
DWORD ptehash_lcalc ( pBYTE p_buffer,
                      BYTE  buffer_len );



/*******************************************************************************
*  function ptehash_cluster_number
*
*  This function returns the cluster number (0..(total_clusters - 1)) where 
*  the hash index belongs.  
*
*  Parameters:
*     hash_index.........hash index returned by one of the hash-calc routines.
*     total_clusters.....number of clusters in this hash table (computed by
*                        the total-clusters function).
*
*******************************************************************************/
LONG ptehash_cluster_number ( DWORD hash_index,
                              LONG  total_clusters );



/*******************************************************************************
*  function ptehash_total_clusters
*
*  This function returns the number of clusters needed, based on the total
*  number of records and number of entries per cluster values passed in.  
*
*  NOTE: The number returned will be a prime number and should not be modified.
*        The prime is crucial to the distribution in the hash table.
*
*  Parameters:
*     total_records.......total number of records that this table will have.
*     num_per_cluster.....number of entries per hash cluster.
*
*
*******************************************************************************/
LONG ptehash_total_clusters ( LONG total_records,
                              LONG num_per_cluster );



/*******************************************************************************
*  function ptehash_optimum_clusters
*
*  This function returns the number of clusters and number of buckets (entries)
*  per cluster needed, based on the total number of records value passed in.  
*
*  NOTE: The number of clusters returned will be a prime number and should not
*        be modified.  The prime is crucial to the distribution in the hash table.
*
*  Parameters:
*     total_records.......total number of records that this table will have.
*     p_num_clusters......number of clusters (returned)
*     num_per_cluster.....number of entries per cluster (returned).
*
*******************************************************************************/
BOOLEAN ptehash_optimum_clusters ( LONG  total_records,
                                   pLONG p_num_clusters,
                                   pLONG p_num_per_cluster );


#endif

