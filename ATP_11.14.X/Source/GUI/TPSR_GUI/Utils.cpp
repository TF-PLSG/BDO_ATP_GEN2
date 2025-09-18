// Utils.cpp: implementation of the CUtils class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "TPSR_GUI.h"
#include "Utils.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CUtils::CUtils()
{

}

CUtils::~CUtils()
{

}


/* Convert a two byte BCD value to an integer value.
   bp - pointer to first byte (MSB) of 2 byte area. */
WORD CUtils::bcd2bin(const pBYTE lpBcd)
{      
   WORD wTot;

   wTot = (lpBcd[0] / 0x10) * 1000  +
         (lpBcd[0] % 0x10) *  100  +
         (lpBcd[1] / 0x10) *   10  +
         (lpBcd[1] % 0x10);

   return(wTot);
}


/* Convert an unsigned number to a two byte bcd value
   bp  - pointer to first byte (MSB) of 2 byte area
   INT - unsigned integer value to be translated */
void CUtils::bin2bcd(pBYTE lpBcd, const WORD wInt)
{
  WORD wJ;

  wJ = wInt / 100;
  lpBcd[0] = (BYTE)((wJ/10)*0x10 + (wJ%10));
  wJ = wInt % 100;
  lpBcd[1] = (BYTE)((wJ/10)*0x10 + (wJ%10));
}


/***********************************************************
 Save the binary TPDU as ASCII data
 ***********************************************************/
void CUtils::bin_to_ascii (pCHAR src, pCHAR dst, INT len)
{
   INT  i, j;

   for (i = 0; i < (len*2); i+=2)
   {
       j = (src[i/2] < 0) ? (src[i/2] + 256) : src[i/2];

      if ((j / 16) <= 9) 
        dst[i] = (j / 16) + '0';
      else
        dst[i] = (j/16 - 0x0a)+'A';

      if ((j & 0x0f) <= 9)
        dst[i+1] = (j & 0x0F) + '0'; 
      else
        dst[i+1] = (j & 0x0f) - 0x0a + 'A';    
   }
} 

/************************************************************
 Retrieve the ASCII hex TPDU and convert it to binary data
 ************************************************************/
void CUtils::ascii_to_bin (pBYTE src, pBYTE dst, INT len)
{
   INT  i, j, k;

   for (i = 0; i < (len*2); i+=2)
   {
      /* if I hit a null terminator, quit.  */
      if (src[i] == '\0')
         break;

      /* if these are leading blanks, leave the bcd zero.  */
      if (src[i] != ' ')
      {
         if (src[i] <= '9')
           dst[i/2] = ((src[i]-'0')*16); 
         else
           dst[i/2] = ((src[i]-'A')+0x0A)*16;   /* handle 0a-0f */
      }

      if (((i+1) < (len*2)) && (src[i+1] != ' '))
      {
         if (src[i+1] <= '9')
           dst[i/2] += (src[i+1]-'0'); 
         else
         {
           j         = (toupper(src[i+1]) - 'A');
           k         = j + 0x0A;
           dst[i/2] += k;

           /* dst[i/2] += ((src[i+1]-'A')+0x0A);   */
         }
      }
   }
} /* ascii_to_bin */
