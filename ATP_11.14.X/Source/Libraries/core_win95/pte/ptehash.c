/******************************************************************************

        Module: ptehash.c
  
         Title: 
  
   Description: 

   $Log:   N:\PVCS\PTE\CORE\PTE_LIB\ptehash.c  $
   
      Rev 1.0   Dec 03 1998 10:57:40   drodden
   Data caching changes.
   

******************************************************************************/

#include <stdlib.h>
#include <stdio.h>

#include "ptehash.h"


#define NUM_PRIMES   150

PRIVATE WORD  primes [NUM_PRIMES] = {
   101,   211,   307,   401,   503,   601,   701,   809,   907,   1009,
   1259,  1511,  1753,  2003,  2251,  2503,  2753,  3001,  3251,  3511,
   3761,  4001,  4253,  4507,  4751,  5003,  5261,  5501,  5779,  6007,
   6257,  6521,  6761,  7001,  7253,  7507,  7753,  8009,  8263,  8501,
   8753,  9001,  9257,  9511,  9767,  10007, 10253, 10501, 10753, 11003,
   11251, 11503, 11777, 12007, 12251, 12503, 12757, 13001, 13259, 13513,
   13751, 14009, 14251, 14503, 14753, 15013, 15259, 15511, 15761, 16001,
   16253, 16519, 16759, 17011, 17257, 17509, 17761, 18013, 18251, 18503,
   18757, 19001, 19259, 19501, 19751, 20011, 20261, 20507, 20753, 21001,
   21269, 21503, 21751, 22003, 22259, 22501, 22751, 23003, 23251, 23509,
   23753, 24001, 24251, 24509, 24763, 25013, 25253, 25523, 25759, 26003, 
   26251, 26501, 26759, 27011, 27253, 27509, 27751, 28001, 28277, 28513, 
   28751, 29009, 29251, 29501, 29753, 30011, 30253, 30509, 30757, 31013, 
   31253, 31511, 31751, 32003, 32251, 32503, 32771, 33013, 33287, 33503, 
   33751, 34019, 34253, 34501, 34757, 35023, 35251, 35507, 35753, 36007
};



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
DWORD ptehash_calc ( pBYTE p_data_str )
{
   DWORD    g;
   DWORD    h;


   h = 0;

   if (p_data_str != NULL_PTR)
   {
      while (*p_data_str != '\0')
      {
         h = (h << 4) + *p_data_str++;

         if (g = h &0xF0000000)
            h ^= (g >> 24);

         h &= ~g;
      }
   }

   return (h);
}



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
                      BYTE  buffer_len )
{
   DWORD    g;
   DWORD    h;


   h = 0;

   if (p_buffer != NULL_PTR)
   {
      while (buffer_len > 0)
      {
         h = (h << 4) + *p_buffer++;

         if (g = h &0xF0000000)
            h ^= (g >> 24);

         h &= ~g;
         buffer_len--;
      }
   }

   return (h);
}



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
                              LONG  total_clusters )
{
   return (hash_index % total_clusters);
}



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
                              LONG num_per_cluster )
{
   int   i;
   LONG  num_clusters;


   if ( (total_records % num_per_cluster) != 0)
      total_records+= num_per_cluster;

   num_clusters = (total_records / num_per_cluster);

   for (i = 0; i < NUM_PRIMES; i++)
   {
      if (primes [i] >= num_clusters)
         return (primes [i]);
   }

   return (0);
}



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
                                   pLONG p_num_per_cluster )
{
   int   i;
   LONG  bucket_to_record_ratio;
   LONG  num_per_clust;
   LONG  num_clusters;
   LONG  num_buckets;


   /*
    * This outer loop cycles through the desireable hash-index bucket to record
    * ration.  What I want is N times as many hash index buckets (entries) as I
    * have records to store in the table.  This gives a better distribution of
    * keys (entries) in the hash index.  The higher N is, the faster the hash 
    * lookup to find a record in the table.  However, this is a diminishing-
    * return value as higher and higher values use more and more memory, but 
    * give smaller and smaller improvements in performance.  Start with four 
    * and work down into the lower (less desireable) ratios.
    */
   for (bucket_to_record_ratio = 4; bucket_to_record_ratio >= 2; bucket_to_record_ratio--)
   {
      /*
       * The hash index buckets will be divided into clusters.  Each cluster 
       * is an array of X number of hash index buckets (entries).  A cluster
       * is the smallest block of the hash index that will be read/written by 
       * applications.  The idea is to not have too many buckets in a cluster 
       * (or too few).  Start with five and work up to ten.
       */
      for (num_per_clust = 5; num_per_clust <= PTEHASH_MAX_PER_CLUSTER; num_per_clust++)
      {
         /*
          * Using the # records, current ratio and number of buckets per 
          * cluster, calculate the number of clusters that will be needed (be
          * sure to round up).  Then look through the list of prime numbers
          * for a prime >= the number of clusters.  If one is found, return
          * it and the number per cluster.  Note:  it is important that the 
          * number of clusters be a prime number.
          */
         num_buckets = total_records * bucket_to_record_ratio;
         
         if ((num_buckets % num_per_clust) != 0)
            num_buckets+= num_per_clust;

         num_clusters = (num_buckets / num_per_clust);

         for (i = 0; i < NUM_PRIMES; i++)
         {
            if (primes [i] >= num_clusters)
            {
               *p_num_clusters    = primes [i];
               *p_num_per_cluster = num_per_clust;
               return (true);
            }
         }

      }
   }

   return (false);
}


