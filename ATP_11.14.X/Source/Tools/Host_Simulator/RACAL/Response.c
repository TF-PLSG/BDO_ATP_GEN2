/*******************************************************************************

  Module:      response.c

  Title:    Host Response Definition File for Racal

  Description: This module defines the various Racal box Responses.
               It isolates the changes required to emulate a different
               kind of Host processing system.


  Revision History:

  $Log:   N:\PVCS\PTE\Hostsim_Racal\racalcon\RESPONSE.C  $
   
      Rev 1.4   Nov 19 2003 10:29:40   dirby
   Corrected a bug in the parsing of pin translate of BDK to interchange.
   
      Rev 1.3   Nov 18 2003 15:28:28   dirby
   Modified to use sPin_Block when parsing a "CI" command.
   This is so it can use the common code for Pin Translation when
   generating/updating responses.
   SCR - Triple DES for Petco
   
      Rev 1.2   Nov 18 2003 13:58:28   dirby
   Added code to support translating a pin from BDK encryption to
   Interchange Key Encryption.  This is the "CI" command.
   SCR - Triple DES for Petco
   
      Rev 1.1   Mar 14 2003 13:21:54   dirby
   Modified to support key tags 'X' for double and 'Y' for triple.
   This is to support X9.17 key encryption scheme.  This only
   applies to keys not encrypted under LMK, so really it only
   applies to the key change command.
   SCR 1055
   
      Rev 1.0   Feb 18 2003 15:18:50   dirby
   Checked in from initial workfile by PVCS Version Manager Project Assistant.
   
******************************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <math.h>
#include <sys/timeb.h>

#include "basictyp.h"
#include "pte.h"
#include "ptecom.h"
#include "response.h"


extern int volatile connection_up;
extern INT init_trace();
extern void write_to_comm_trace_file( pBYTE bufptr, INT len, BYTE msg_type );
char RespCodeinTxt[4] = {0};
char AuthCodeinTxt[7] = {0};
char RRNinTxt[12] = {0};


int    g_using_tpdu_prefix = false;      // Whether the TPDU is present or not


extern void genutil_asc_to_bin( pCHAR astr, pBYTE bstr, pINT bstr_len )
{
   INT  i,j;
   INT  len;

   len = strlen( astr );
   *bstr_len = 0;
   for( i=0,j=0; i<len; i+=2,j++ )
   {
      /* Upper nibble */
      if ( astr[i] <= '9' )
         bstr[j] = (astr[i] & 0x0f) * 16;
      else
         bstr[j] = ((astr[i] & 0x0f) + 9) * 16;

      /* Lower nibble */
      if ( astr[i+1] <= '9' )
         bstr[j] |= (astr[i+1] & 0x0f);
      else
         bstr[j] |= ((astr[i+1] & 0x0f) + 9);

      (*bstr_len)++;
   }
   return;
}

/****************************************************************************
 * 
 *   FUNCTION: generate_response
 * 
 *   PURPOSE:  This function creates a response message based on the
 *             command code.  Most of the pieces should already be
 *             populated.  Just need to put them together.
 * 
 * ARGUMENTS:  host_resp_buffer  = Response data buffer area. This is the 
 *                                 network message buffer for output.
 * 
 * RETURN VAL: response_len = The number of bytes generated in the buffer
 * 
 ****************************************************************************/
WORD generate_response ()
{
   WORD  response_len;        // Length of the response generated
   INT   len;
   char bstrng[20]= {0};
   int blen = 0;
   memset( host_resp_buffer, 0x00, sizeof(host_resp_buffer) );

   // Insert the TPDU if appropriate;
   if (g_using_tpdu_prefix == true)
   {
      host_resp_buffer [0] = save_tpdu [0];
      host_resp_buffer [1] = save_tpdu [3];
      host_resp_buffer [2] = save_tpdu [4];
      host_resp_buffer [3] = save_tpdu [1];
      host_resp_buffer [4] = save_tpdu [2];
      response_len   = sizeof(save_tpdu);
   }
   else 
      response_len = 0;

   if ( 0 == strcmp(CmdCode, KEY_CHANGE) )
   {
      memcpy( &host_resp_buffer[response_len], MsgHeader, 4 );
      response_len += 4;

      memcpy( &host_resp_buffer[response_len], RespCode, 2 );
      response_len += 2;

      memcpy( &host_resp_buffer[response_len], ErrCode, 2 );
      response_len += 2;

      /* Determine length of ZPK - the new key. */
      len = strlen( Zpk );
      switch( len )
      {
         case 32: host_resp_buffer[response_len++] = 'X'; break;

         default: len = 16; break;
      }
      memcpy( &host_resp_buffer[response_len], Zpk, len );
      response_len += len;
   }
   else if ( (0 == strcmp(CmdCode,PIN_VERIFY_LOCAL)  ) ||
             (0 == strcmp(CmdCode,PIN_VERIFY_NETWORK))  )
   {
      memcpy( &host_resp_buffer[response_len], MsgHeader, 4 );
      response_len += 4;

      memcpy( &host_resp_buffer[response_len], RespCode, 2 );
      response_len += 2;

      memcpy( &host_resp_buffer[response_len], ErrCode, 2 );
      response_len += 2;
   }
   else if ( (0 == strcmp(CmdCode,PIN_TRANSLATE_LOCAL)  ) ||
             (0 == strcmp(CmdCode,PIN_TRANSLATE_NETWORK)) ||
             (0 == strcmp(CmdCode,PIN_TRANSLATE_BDK_TO_INTERCHANGE)) )
   {
      memcpy( &host_resp_buffer[response_len], MsgHeader, 4 );
      response_len += 4;

      memcpy( &host_resp_buffer[response_len], RespCode, 2 );
      response_len += 2;

      memcpy( &host_resp_buffer[response_len], ErrCode, 2 );
      response_len += 2;

      memcpy( &host_resp_buffer[response_len], "16", 2 );
      response_len += 2;

      memcpy( &host_resp_buffer[response_len], dPin_Block, 16 );
     response_len += 16;

    memcpy( &host_resp_buffer[response_len], dPin_Block_Format, 2 );
    response_len += 2;

   }
   else if ( 0 == strcmp(CmdCode,MAGSTRIPE_CVV)  )
   {
      memcpy( &host_resp_buffer[response_len], MsgHeader, 4 );
      response_len += 4;

      memcpy( &host_resp_buffer[response_len], RespCode, 2 );
      response_len += 2;

      memcpy( &host_resp_buffer[response_len], ErrCode, 2 );
      response_len += 2;
   }
   else if ( 0 == strcmp(CmdCode,MAGSTRIPE_CSC) )
   {
      memcpy( &host_resp_buffer[response_len], MsgHeader, 4 );
      response_len += 4;

      memcpy( &host_resp_buffer[response_len], RespCode, 2 );
      response_len += 2;

      memcpy( &host_resp_buffer[response_len], ErrCode, 2 );
      response_len += 2;

   }
   else if ( 0 == strcmp(CmdCode,PIN_VERIFY) )
   {
      memcpy( &host_resp_buffer[response_len], MsgHeader, 4 );
      response_len += 4;

      memcpy( &host_resp_buffer[response_len], RespCode, 2 );
      response_len += 2;

      memcpy( &host_resp_buffer[response_len], ErrCode, 2 );
      response_len += 2;

   }
   else if ( 0 == strcmp(CmdCode,GENERATE_DINERS_KEY) )
   {
      memcpy( &host_resp_buffer[response_len], MsgHeader, 4 );
      response_len += 4;

      memcpy( &host_resp_buffer[response_len], RespCode, 2 );
      response_len += 2;

      memcpy( &host_resp_buffer[response_len], ErrCode, 2 );
      response_len += 2;
	        /* Determine length of ZPK - the new key. */
      len = strlen( Zpk );
      switch( len )
      {
         case 32: host_resp_buffer[response_len++] = 'X'; break;
         default: len = 16; break;
      }
      memcpy( &host_resp_buffer[response_len], Zpk, len );
      response_len += len;

	  memcpy( &host_resp_buffer[response_len], "111111", 6 );
      response_len += 6;

   }
   else if( 0 == strcmp(CmdCode,PIN_VERIFY_ARQC_DINERS))
   {
	       memcpy( &host_resp_buffer[response_len], MsgHeader, 4 );
      response_len += 4;

      memcpy( &host_resp_buffer[response_len], RespCode, 2 );
      response_len += 2;

      memcpy( &host_resp_buffer[response_len], ErrCode, 2 );
      response_len += 2;
	        /* Determine length of ZPK - the new key. */
	  genutil_asc_to_bin("e17b2716fc622096",(BYTE *)&bstrng,&blen);
	  memcpy( &host_resp_buffer[response_len], bstrng, 8 );
	  response_len +=8;
   }
   else if ( 0 == strcmp(CmdCode,PIN_VERIFY_ARQC_CUP) )
   {
      memcpy( &host_resp_buffer[response_len], MsgHeader, 4 );
      response_len += 4;

      memcpy( &host_resp_buffer[response_len], RespCode, 2 );
      response_len += 2;

      memcpy( &host_resp_buffer[response_len], ErrCode, 2 );
      response_len += 2;

   }
   else if ( 0 == strcmp(CmdCode,PIN_VERIFY_ARQC_JCB) )
   {
      memcpy( &host_resp_buffer[response_len], MsgHeader, 4 );
      response_len += 4;

      memcpy( &host_resp_buffer[response_len], RespCode, 2 );
      response_len += 2;

      memcpy( &host_resp_buffer[response_len], ErrCode, 2 );
      response_len += 2;

   }
   else if ( 0 == strcmp(CmdCode,PIN_VERIFY_ARQC_EMV) )
   {
      memcpy( &host_resp_buffer[response_len], MsgHeader, 4 );
      response_len += 4;

      memcpy( &host_resp_buffer[response_len], RespCode, 2 );
      response_len += 2;

      memcpy( &host_resp_buffer[response_len], ErrCode, 2 );
      response_len += 2;

   }
   return( response_len );

}  // end generate_response


/****************************************************************************
 *
 * FUNCTION:   update_iso_response
 * PURPOSE:    This function performs error checking
 *             according to the rules listed on the About button.
 *
 * ARGUMENTS:  NONE 
 *
 * RETURN VAL: NONE
 * 
 ****************************************************************************/
void update_iso_response()
{
   CHAR  first;
   CHAR  first_two[3] = "";
   CHAR  last;

   if ( 0 == strcmp(CmdCode, KEY_CHANGE) )
   {
      /* Set default values. */
      strcpy( RespCode, KEY_CHANGE_RESPONSE );
      strcpy( ErrCode,  "00" );

      /* Check ZPK (CWK) for error criteria. */
      first = Zpk[0];
      if ( first == 'A' )
         strcpy( MsgHeader, "KEYC" );
      else if ( first == 'B' )
         strcpy( RespCode, PIN_VERIFY_NETWORK_RESPONSE ); /* Err, wrong one */
    /*  else
      {
         memcpy( first_two, Zpk, 2 );
         if ( 0 == strcmp(first_two, "01") )
            strcpy( ErrCode, "01" );
         else if ( 0 == strcmp(first_two, "10") )
            strcpy( ErrCode, "10" );
         else if ( 0 == strcmp(first_two, "11") )
            strcpy( ErrCode, "11" );
         else if ( 0 == strcmp(first_two, "12") )
            strcpy( ErrCode, "12" );
         else if ( 0 == strcmp(first_two, "13") )
            strcpy( ErrCode, "13" );
         else if ( 0 == strcmp(first_two, "15") )
            strcpy( ErrCode, "15" );
         else if ( 0 == strcmp(first_two, "21") )
            strcpy( ErrCode, "21" );
         else
         {
            No errors. Create new key - See ABOUT button. */
            first = increment_hex_value( first );
            last  = Zpk[strlen(Zpk)-1];
            last  = increment_hex_value( last );
            Zpk[0] = first;
            Zpk[strlen(Zpk)-1] = last;
       //  }
    //  }
   }
   else if ( (0 == strcmp(CmdCode,PIN_VERIFY_LOCAL)  ) ||
             (0 == strcmp(CmdCode,PIN_VERIFY_NETWORK))  )
   {
      /* Set default values. */
      strcpy( ErrCode,  "00" );

      if ( 0 == strcmp(CmdCode,PIN_VERIFY_LOCAL) )
         strcpy( RespCode, PIN_VERIFY_LOCAL_RESPONSE );
      else
         strcpy( RespCode, PIN_VERIFY_NETWORK_RESPONSE );

      /* Check TPK (CWK) for error criteria. */
      first = Tpk[0];
      if ( first == 'A' )
         strcpy( MsgHeader, "PINV" );
      else if ( first == 'B' )
         strcpy( RespCode, KEY_CHANGE_RESPONSE ); /* Err, wrong one */
   /*   else
      {
         memcpy( first_two, Tpk, 2 );
         if ( 0 == strcmp(first_two, "01") )
            strcpy( ErrCode, "01" );
         else if ( 0 == strcmp(first_two, "10") )
            strcpy( ErrCode, "10" );
         else if ( 0 == strcmp(first_two, "11") )
            strcpy( ErrCode, "11" );
         else if ( 0 == strcmp(first_two, "12") )
            strcpy( ErrCode, "12" );
         else if ( 0 == strcmp(first_two, "13") )
            strcpy( ErrCode, "13" );
         else if ( 0 == strcmp(first_two, "15") )
            strcpy( ErrCode, "15" );
         else if ( 0 == strcmp(first_two, "20") )
            strcpy( ErrCode, "20" );
         else if ( 0 == strcmp(first_two, "21") )
            strcpy( ErrCode, "21" );
         else if ( 0 == strcmp(first_two, "23") )
            strcpy( ErrCode, "23" );
         else if ( 0 == strcmp(first_two, "27") )
            strcpy( ErrCode, "27" );
      }*/
   }
   else if ( (0 == strcmp(CmdCode,PIN_TRANSLATE_LOCAL)  ) ||
             (0 == strcmp(CmdCode,PIN_TRANSLATE_NETWORK)) ||
             (0 == strcmp(CmdCode,PIN_TRANSLATE_BDK_TO_INTERCHANGE)) )
   {
      /* Set default values. */
      strcpy( ErrCode,  "00" );

      if ( 0 == strcmp(CmdCode,PIN_TRANSLATE_LOCAL) )
         strcpy( RespCode, PIN_TRANSLATE_LOCAL_RESPONSE );
      else if ( 0 == strcmp(CmdCode,PIN_TRANSLATE_NETWORK) )
         strcpy( RespCode, PIN_TRANSLATE_NETWORK_RESPONSE );
      else
         strcpy( RespCode, PIN_TRANSLATE_BDK_TO_INTERCHANGE_RESPONSE );

      /* Check Source ZPK (CWK) for error criteria. */
/*      first = sZpk[0];
      if ( first == 'A' )
         strcpy( MsgHeader, "PINT" );
      else if ( first == 'B' )
         strcpy( RespCode, KEY_CHANGE_RESPONSE ); // Err, wrong one 
      else
      {
         memcpy( first_two, sZpk, 2 );
         if ( 0 == strcmp(first_two, "10") )
            strcpy( ErrCode, "10" );
         else if ( 0 == strcmp(first_two, "11") )
            strcpy( ErrCode, "11" );
         else if ( 0 == strcmp(first_two, "12") )
            strcpy( ErrCode, "12" );
         else if ( 0 == strcmp(first_two, "13") )
            strcpy( ErrCode, "13" );
         else if ( 0 == strcmp(first_two, "15") )
            strcpy( ErrCode, "15" );
         else if ( 0 == strcmp(first_two, "20") )
            strcpy( ErrCode, "20" );
         else if ( 0 == strcmp(first_two, "21") )
            strcpy( ErrCode, "21" );
         else if ( 0 == strcmp(first_two, "22") )
            strcpy( ErrCode, "22" );
         else if ( 0 == strcmp(first_two, "23") )
            strcpy( ErrCode, "23" );
         else if ( 0 == strcmp(first_two, "24") )
            strcpy( ErrCode, "24" );
         else
         {
            // No errors. Create new pin - See ABOUT button. 
            memcpy( dPin_Block, sPin_Block, sizeof(sPin_Block) );
            first = sPin_Block[0];
            first = increment_hex_value( first );
            last  = sPin_Block[strlen(sPin_Block)-1];
            last  = increment_hex_value( last );
            dPin_Block[0] = first;
            dPin_Block[strlen(dPin_Block)-1] = last;
         }/* test start*/
		     memcpy( dPin_Block, sPin_Block, sizeof(sPin_Block) );
            first = sPin_Block[0];
            first = increment_hex_value( first );
            last  = sPin_Block[strlen(sPin_Block)-1];
            last  = increment_hex_value( last );
            dPin_Block[0] = first;
            dPin_Block[strlen(dPin_Block)-1] = last;
			/*test ends */
      
   }
   else if ( 0 == strcmp(CmdCode,MAGSTRIPE_CVV)  )
   {
      /* Set default values. */
      strcpy( RespCode, MAGSTRIPE_CVV_RESPONSE );
      strcpy( ErrCode,  "00" );

      memcpy( first_two, Card_Number, 2 );
    /*  if ( 0 == strcmp(first_two, "01") )
         strcpy( ErrCode, "01" );
      else if ( 0 == strcmp(first_two, "10") )
         strcpy( ErrCode, "10" );
      else if ( 0 == strcmp(first_two, "11") )
         strcpy( MsgHeader, "KEYC" );
      else if ( 0 == strcmp(first_two, "12") )
         strcpy( ErrCode, "12" );
      else if ( 0 == strcmp(first_two, "13") )
         strcpy( ErrCode, "13" );
      else if ( 0 == strcmp(first_two, "15") )
         strcpy( ErrCode, "15" );
      else if ( 0 == strcmp(first_two, "21") )
         strcpy( ErrCode, "21" );
      else if ( 0 == strcmp(first_two, "22") )
         strcpy( RespCode, PIN_VERIFY_NETWORK_RESPONSE );// Err, wrong one 
      else if ( 0 == strcmp(first_two, "27") )
         strcpy( ErrCode, "27" );*/
   }
   else if ( 0 == strcmp(CmdCode,MAGSTRIPE_CSC) )
   {
      /* Set default values. */
      strcpy( RespCode, MAGSTRIPE_CSC_RESPONSE );
      strcpy( ErrCode,  "00" );

      memcpy( first_two, Card_Number, 2 );
  /*    if ( 0 == strcmp(first_two, "01") )
         strcpy( ErrCode, "01" );
      else if ( 0 == strcmp(first_two, "10") )
         strcpy( ErrCode, "10" );
      else if ( 0 == strcmp(first_two, "11") )
         strcpy( MsgHeader, "KEYC" );
      else if ( 0 == strcmp(first_two, "12") )
         strcpy( ErrCode, "12" );
      else if ( 0 == strcmp(first_two, "13") )
         strcpy( ErrCode, "13" );
      else if ( 0 == strcmp(first_two, "15") )
         strcpy( ErrCode, "15" );
      else if ( 0 == strcmp(first_two, "21") )
         strcpy( ErrCode, "21" );
      else if ( 0 == strcmp(first_two, "22") )
         strcpy( RespCode, PIN_VERIFY_NETWORK_RESPONSE ); // Err, wrong one 
      else if ( 0 == strcmp(first_two, "27") )
         strcpy( ErrCode, "27" );*/
   }
      else if ( 0 == strcmp(CmdCode,PIN_VERIFY) )
   {
      /* Set default values. */
      strcpy( RespCode, PIN_VERIFY_RESPONSE );
      strcpy( ErrCode,  "00" );

  
   }
      else if ( 0 == strcmp(CmdCode,GENERATE_DINERS_KEY) )
   {
      /* Set default values. */
      strcpy( RespCode, GENERATE_DINERS_KEY_RESPONSE );
      strcpy( ErrCode,  "00" );

      
   }
	else if ( 0 == strcmp(CmdCode,PIN_VERIFY_ARQC_CUP) )
   {
      /* Set default values. */
      strcpy( RespCode, PIN_VERIFY_ARQC_CUP_RESPONSE );
      strcpy( ErrCode,  "00" );

   }
	else if ( 0 == strcmp(CmdCode,PIN_VERIFY_ARQC_JCB) )
   {
      /* Set default values. */
      strcpy( RespCode, PIN_VERIFY_ARQC_JCB_RESPONSE );
      strcpy( ErrCode,  "00" );

   }
	else if ( 0 == strcmp(CmdCode,PIN_VERIFY_ARQC_EMV) )
   {
      /* Set default values. */
      strcpy( RespCode, PIN_VERIFY_ARQC_EMV_RESPONSE );
      strcpy( ErrCode,  "00" );

    
   }
	else if ( 0 == strcmp(CmdCode,PIN_VERIFY_ARQC_DINERS) )
	{
		strcpy( RespCode, PIN_VERIFY_ARQC_DINERS_RESPONSE );
		strcpy( ErrCode,  "00" );

	}

	strncpy(ErrCode,RespCodeinTxt,2);

   return;
}  // end update_iso_response



//****************************************************************************
//***
//*** FUNCTION:   parse_host_request
//*** PURPOSE:    To examine the incoming request. This
//***               determines the value of certain fields.
//***               It always returns.
//*** 
//*** ARGUMENTS:  p_msg    = A pointer to the complete 
//***                   Host Request message.
//*** 
//***             length_of_msg = The length of the message
//*** 
//*** RETURN VAL: 0  = Request was correctly processed
//***              >0  = Request is formatted INCORRECTLY
//*** 
//*** NOTES:      This function should be modified to get
//***          additional data from incoming requests.
//*** 
//****************************************************************************
long parse_host_request(LPBYTE p_msg, WORD length_of_msg)
{
   INT  i;
   INT  len1;
   INT  len2;
   long validity = 0;     // Validity processing flag
   CHAR len_indicator = 0x00;

   // Check arguments for validity  
   if ((p_msg == NULL_PTR) || (length_of_msg == 0))
   {
      validity = 255;
   }
   else
   {
      memset( MsgHeader, 0x00, sizeof(MsgHeader) );
      memset( CmdCode,   0x00, sizeof(CmdCode  ) );

      /* Get Message Header and Command Code */
      memcpy( MsgHeader, p_msg,   4 );
      memcpy( CmdCode,   p_msg+4, 2 );

      /* Act based on Command Code */
      if ( 0 == strcmp(CmdCode, KEY_CHANGE) )
      {
         /* Get ZMK */
         memset( Zmk, 0x00, sizeof(Zmk) );
         len_indicator = p_msg[6];
         switch( len_indicator )
         {
            case 'X': 
            case 'U': len1 = 32; i = 1; break;
            case 'Y':
            case 'T': len1 = 48; i = 1; break;
            default:  len1 = 16; i = 0; break;
         }
         memcpy( Zmk, p_msg+6+i, len1 );
         len1 += 6 + i;

         /* Get ZPK - a.k.a. CWK */
         memset( Zpk, 0x00, sizeof(Zpk) );
         len_indicator = p_msg[len1];
         switch( len_indicator )
         {
            case 'X':
            case 'U': len2 = 32; i = 1; break;
            case 'Y':
            case 'T': len2 = 48; i = 1; break;
            default:  len2 = 16; i = 0; break;
         }
         memcpy( Zpk, p_msg+len1+i, len2 );
      }
      else if ( (0 == strcmp(CmdCode,PIN_VERIFY_LOCAL)  ) ||
                (0 == strcmp(CmdCode,PIN_VERIFY_NETWORK))  )
      {
         /* Get TPK */
         memset( Tpk, 0x00, sizeof(Tpk) );
         len_indicator = p_msg[6];
         switch( len_indicator )
         {
            case 'X':
            case 'U': len1 = 32; i = 1; break;
            case 'Y':
            case 'T': len1 = 48; i = 1; break;
            default:  len1 = 16; i = 0; break;
         }
         memcpy( Tpk, p_msg+6+i, len1 );
         len1 += 6 + i;

         /* Get PVK Pair */
         memset( PVK_Pair, 0x00, sizeof(PVK_Pair) );
         memcpy( PVK_Pair, p_msg+len1, 32 );
         len1 += 32;

         /* Get Pin Block */
         memset( Pin_Block, 0x00, sizeof(Pin_Block) );
         memcpy( Pin_Block, p_msg+len1, 16 );
         len1 += 16;

         /* Get Pin Block Format */
         memset( Pin_Block_Format, 0x00, sizeof(Pin_Block_Format) );
         memcpy( Pin_Block_Format, p_msg+len1, 2 );
         len1 += 2;

         /* Get Card Number */
         memset( Card_Number, 0x00, sizeof(Card_Number) );
         memcpy( Card_Number, p_msg+len1, 12 );
         len1 += 12;

         /* Get PVKI */
         PVKI = p_msg[len1++];

         /* Get PVV */
         memset( PVV, 0x00, sizeof(PVV) );
         memcpy( PVV, p_msg+len1, 4 );
      }
      else if ( (0 == strcmp(CmdCode,PIN_TRANSLATE_LOCAL)  ) ||
                (0 == strcmp(CmdCode,PIN_TRANSLATE_NETWORK))  )
      {
         /* Get Source ZPK */
         memset( sZpk, 0x00, sizeof(sZpk) );
         len_indicator = p_msg[6];
         switch( len_indicator )
         {
            case 'X':
            case 'U': len1 = 32; i = 1; break;
            case 'Y':
            case 'T': len1 = 48; i = 1; break;
            default:  len1 = 16; i = 0; break;
         }
         memcpy( sZpk, p_msg+6+i, len1 );
         len1 += 6 + i;

         /* Get Destination ZPK */
         memset( dZpk, 0x00, sizeof(dZpk) );
         len_indicator = p_msg[len1];
         switch( len_indicator )
         {
            case 'X':
            case 'U': len2 = 32; i = 1; break;
            case 'Y':
            case 'T': len2 = 48; i = 1; break;
            default:  len2 = 16; i = 0; break;
         }
         memcpy( dZpk, p_msg+len1+i, len2 );
         len1 += len2 + i;

         /* Move past max pin length, always 12. */
         len1 += 2;

         /* Get Source Pin Block */
         memset( sPin_Block, 0x00, sizeof(sPin_Block) );
         memcpy( sPin_Block, p_msg+len1, 16 );
         len1 += 16;

         /* Get Source Pin Block Format */
         memset( sPin_Block_Format, 0x00, sizeof(sPin_Block_Format) );
         memcpy( sPin_Block_Format, p_msg+len1, 2 );
         len1 += 2;

         /* Get Destination Pin Block Format */
         memset( dPin_Block_Format, 0x00, sizeof(dPin_Block_Format) );
         memcpy( dPin_Block_Format, p_msg+len1, 2 );
         len1 += 2;

         /* Get Card Number */
         memset( Card_Number, 0x00, sizeof(Card_Number) );
         memcpy( Card_Number, p_msg+len1, 12 );
      }
      else if ( 0 == strcmp(CmdCode,MAGSTRIPE_CVV)  )
      {
         /* Get CVK A/B */
         memset( CVK, 0x00, sizeof(CVK) );
         len_indicator = p_msg[6];
         switch( len_indicator )
         {
            case 'X':
            case 'U': len1 = 32; i = 1; break;
            case 'Y':
            case 'T': len1 = 48; i = 1; break;
            default:  len1 = 32; i = 0; break;
         }
         memcpy( CVK, p_msg+6+i, len1 );
         len1 += 6 + i;

         /* Get CVV */
         memset( CVV, 0x00, sizeof(CVV) );
         memcpy( CVV, p_msg+len1, 3 );
         len1 += 3;

         /* Get Card Number */
         memset( Card_Number, 0x00, sizeof(Card_Number) );
         memcpy( Card_Number, p_msg+len1, 19 );
      }
      else if ( 0 == strcmp(CmdCode,MAGSTRIPE_CSC) )
      {
         /* Get CSCK */
         memset( CVK, 0x00, sizeof(CVK) );
         len_indicator = p_msg[8];
         switch( len_indicator )
         {
            case 'X':
            case 'U': len1 = 32; i = 1; break;
            case 'Y':
            case 'T': len1 = 48; i = 1; break;
            default:  len1 = 32; i = 0; break;
         }
         memcpy( CVK, p_msg+8+i, len1 );
         len1 += 8 + i;

         /* Get Card Number */
         memset( Card_Number, 0x00, sizeof(Card_Number) );
         memcpy( Card_Number, p_msg+len1, 19 );
      }
      else if ( 0 == strcmp(CmdCode,PIN_TRANSLATE_BDK_TO_INTERCHANGE) )
      {
         /* Get BDK */
         memset( Bdk, 0x00, sizeof(Bdk) );
         memcpy( Bdk, p_msg+6, 32 );

         /* Get Encrypted Interchange Key */
         memset( sZpk, 0x00, sizeof(sZpk) );
         len_indicator = p_msg[38];
         switch( len_indicator )
         {
            case 'X':
            case 'U': len1 = 32; i = 1; break;
            case 'Y':
            case 'T': len1 = 48; i = 1; break;
            default:  len1 = 16; i = 0; break;
         }
         memcpy( sZpk, p_msg+38+i, len1 );
         len1 += 38 + i;

         /* Get KSN Descriptor */
         memset( Ksn_Desc, 0x00, sizeof(Ksn_Desc) );
         memcpy( Ksn_Desc, p_msg+len1, 3 );
         len1 += 3;

         /* Get Key Serial Number */
         memset( Ksn, 0x00, sizeof(Ksn) );
         memcpy( Ksn, p_msg+len1, 16 );
         len1 += 16;

         /* Get Pin Block */
         memset( sPin_Block, 0x00, sizeof(sPin_Block) );
         memcpy( sPin_Block, p_msg+len1, 16 );
         len1 += 16;

         /* Get Pin Block Format */
         memset( Pin_Block_Format, 0x00, sizeof(Pin_Block_Format) );
         memcpy( Pin_Block_Format, p_msg+len1, 2 );
         len1 += 2;

         /* Get Card Number */
         memset( Card_Number, 0x00, sizeof(Card_Number) );
         memcpy( Card_Number, p_msg+len1, 12 );
         len1 += 12;
      }
	  else if ( 0 == strcmp(CmdCode,PIN_VERIFY) )
      {
         /* Get BDK */
         memset( Bdk, 0x00, sizeof(Bdk) );
         memcpy( Bdk, p_msg+6, 32 );

         /* Get Encrypted Interchange Key */
         memset( sZpk, 0x00, sizeof(sZpk) );
         len_indicator = p_msg[38];
         switch( len_indicator )
         {
            case 'X':
            case 'U': len1 = 32; i = 1; break;
            case 'Y':
            case 'T': len1 = 48; i = 1; break;
            default:  len1 = 16; i = 0; break;
         }
         memcpy( sZpk, p_msg+38+i, len1 );
         len1 += 38 + i;

         /* Get KSN Descriptor */
         memset( Ksn_Desc, 0x00, sizeof(Ksn_Desc) );
         memcpy( Ksn_Desc, p_msg+len1, 3 );
         len1 += 3;

         /* Get Key Serial Number */
         memset( Ksn, 0x00, sizeof(Ksn) );
         memcpy( Ksn, p_msg+len1, 16 );
         len1 += 16;

         /* Get Pin Block */
         memset( sPin_Block, 0x00, sizeof(sPin_Block) );
         memcpy( sPin_Block, p_msg+len1, 16 );
         len1 += 16;

         /* Get Pin Block Format */
         memset( Pin_Block_Format, 0x00, sizeof(Pin_Block_Format) );
         memcpy( Pin_Block_Format, p_msg+len1, 2 );
         len1 += 2;

         /* Get Card Number */
         memset( Card_Number, 0x00, sizeof(Card_Number) );
         memcpy( Card_Number, p_msg+len1, 12 );
         len1 += 12;
      }
	  else if ( 0 == strcmp(CmdCode,GENERATE_DINERS_KEY) )
      {
         /* Get BDK */
         memset( Bdk, 0x00, sizeof(Bdk) );
         memcpy( Bdk, p_msg+6, 32 );

         /* Get Encrypted Interchange Key */
         memset( sZpk, 0x00, sizeof(sZpk) );
         len_indicator = p_msg[38];
         switch( len_indicator )
         {
            case 'X':
            case 'U': len1 = 32; i = 1; break;
            default:  len1 = 16; i = 0; break;
         }
         memcpy( sZpk, p_msg+38+i, len1 );
         len1 += 38 + i;

         /* Get KSN Descriptor */
         memset( Ksn_Desc, 0x00, sizeof(Ksn_Desc) );
         memcpy( Ksn_Desc, p_msg+len1, 3 );
         len1 += 3;

         /* Get Key Serial Number */
         memset( Ksn, 0x00, sizeof(Ksn) );
         memcpy( Ksn, p_msg+len1, 16 );
         len1 += 16;

         /* Get Pin Block */
         memset( sPin_Block, 0x00, sizeof(sPin_Block) );
         memcpy( sPin_Block, p_msg+len1, 16 );
         len1 += 16;

         /* Get Pin Block Format */
         memset( Pin_Block_Format, 0x00, sizeof(Pin_Block_Format) );
         memcpy( Pin_Block_Format, p_msg+len1, 2 );
         len1 += 2;

         /* Get Card Number */
         memset( Card_Number, 0x00, sizeof(Card_Number) );
         memcpy( Card_Number, p_msg+len1, 12 );
         len1 += 12;
      }
	  else if(0 == strcmp(CmdCode,PIN_VERIFY_ARQC_DINERS))
	  {
		           /* Get BDK */
         memset( Bdk, 0x00, sizeof(Bdk) );
         memcpy( Bdk, p_msg+6, 32 );

         /* Get Encrypted Interchange Key */
         memset( sZpk, 0x00, sizeof(sZpk) );
         len_indicator = p_msg[38];
         switch( len_indicator )
         {
            case 'X':
            case 'U': len1 = 32; i = 1; break;
            case 'Y':
            case 'T': len1 = 48; i = 1; break;
            default:  len1 = 16; i = 0; break;
         }
         memcpy( sZpk, p_msg+38+i, len1 );
         len1 += 38 + i;

         /* Get KSN Descriptor */
         memset( Ksn_Desc, 0x00, sizeof(Ksn_Desc) );
         memcpy( Ksn_Desc, p_msg+len1, 3 );
         len1 += 3;

         /* Get Key Serial Number */
         memset( Ksn, 0x00, sizeof(Ksn) );
         memcpy( Ksn, p_msg+len1, 16 );
         len1 += 16;

         /* Get Pin Block */
         memset( sPin_Block, 0x00, sizeof(sPin_Block) );
         memcpy( sPin_Block, p_msg+len1, 16 );
         len1 += 16;

         /* Get Pin Block Format */
         memset( Pin_Block_Format, 0x00, sizeof(Pin_Block_Format) );
         memcpy( Pin_Block_Format, p_msg+len1, 2 );
         len1 += 2;

         /* Get Card Number */
         memset( Card_Number, 0x00, sizeof(Card_Number) );
         memcpy( Card_Number, p_msg+len1, 12 );
         len1 += 12;
	  }
	  else if ( 0 == strcmp(CmdCode,PIN_VERIFY_ARQC_CUP) )
      {
         /* Get BDK */
         memset( Bdk, 0x00, sizeof(Bdk) );
         memcpy( Bdk, p_msg+6, 32 );

         /* Get Encrypted Interchange Key */
         memset( sZpk, 0x00, sizeof(sZpk) );
         len_indicator = p_msg[38];
         switch( len_indicator )
         {
            case 'X':
            case 'U': len1 = 32; i = 1; break;
            case 'Y':
            case 'T': len1 = 48; i = 1; break;
            default:  len1 = 16; i = 0; break;
         }
         memcpy( sZpk, p_msg+38+i, len1 );
         len1 += 38 + i;

         /* Get KSN Descriptor */
         memset( Ksn_Desc, 0x00, sizeof(Ksn_Desc) );
         memcpy( Ksn_Desc, p_msg+len1, 3 );
         len1 += 3;

         /* Get Key Serial Number */
         memset( Ksn, 0x00, sizeof(Ksn) );
         memcpy( Ksn, p_msg+len1, 16 );
         len1 += 16;

         /* Get Pin Block */
         memset( sPin_Block, 0x00, sizeof(sPin_Block) );
         memcpy( sPin_Block, p_msg+len1, 16 );
         len1 += 16;

         /* Get Pin Block Format */
         memset( Pin_Block_Format, 0x00, sizeof(Pin_Block_Format) );
         memcpy( Pin_Block_Format, p_msg+len1, 2 );
         len1 += 2;

         /* Get Card Number */
         memset( Card_Number, 0x00, sizeof(Card_Number) );
         memcpy( Card_Number, p_msg+len1, 12 );
         len1 += 12;
      }
	  else if ( 0 == strcmp(CmdCode,PIN_VERIFY_ARQC_JCB) )
      {
         /* Get BDK */
         memset( Bdk, 0x00, sizeof(Bdk) );
         memcpy( Bdk, p_msg+6, 32 );

         /* Get Encrypted Interchange Key */
         memset( sZpk, 0x00, sizeof(sZpk) );
         len_indicator = p_msg[38];
         switch( len_indicator )
         {
            case 'X':
            case 'U': len1 = 32; i = 1; break;
            case 'Y':
            case 'T': len1 = 48; i = 1; break;
            default:  len1 = 16; i = 0; break;
         }
         memcpy( sZpk, p_msg+38+i, len1 );
         len1 += 38 + i;

         /* Get KSN Descriptor */
         memset( Ksn_Desc, 0x00, sizeof(Ksn_Desc) );
         memcpy( Ksn_Desc, p_msg+len1, 3 );
         len1 += 3;

         /* Get Key Serial Number */
         memset( Ksn, 0x00, sizeof(Ksn) );
         memcpy( Ksn, p_msg+len1, 16 );
         len1 += 16;

         /* Get Pin Block */
         memset( sPin_Block, 0x00, sizeof(sPin_Block) );
         memcpy( sPin_Block, p_msg+len1, 16 );
         len1 += 16;

         /* Get Pin Block Format */
         memset( Pin_Block_Format, 0x00, sizeof(Pin_Block_Format) );
         memcpy( Pin_Block_Format, p_msg+len1, 2 );
         len1 += 2;

         /* Get Card Number */
         memset( Card_Number, 0x00, sizeof(Card_Number) );
         memcpy( Card_Number, p_msg+len1, 12 );
         len1 += 12;
      }
	  else if ( 0 == strcmp(CmdCode,PIN_VERIFY_ARQC_EMV) )
      {
         /* Get BDK */
         memset( Bdk, 0x00, sizeof(Bdk) );
         memcpy( Bdk, p_msg+6, 32 );

         /* Get Encrypted Interchange Key */
         memset( sZpk, 0x00, sizeof(sZpk) );
         len_indicator = p_msg[38];
         switch( len_indicator )
         {
            case 'X':
            case 'U': len1 = 32; i = 1; break;
            case 'Y':
            case 'T': len1 = 48; i = 1; break;
            default:  len1 = 16; i = 0; break;
         }
         memcpy( sZpk, p_msg+38+i, len1 );
         len1 += 38 + i;

         /* Get KSN Descriptor */
         memset( Ksn_Desc, 0x00, sizeof(Ksn_Desc) );
         memcpy( Ksn_Desc, p_msg+len1, 3 );
         len1 += 3;

         /* Get Key Serial Number */
         memset( Ksn, 0x00, sizeof(Ksn) );
         memcpy( Ksn, p_msg+len1, 16 );
         len1 += 16;

         /* Get Pin Block */
         memset( sPin_Block, 0x00, sizeof(sPin_Block) );
         memcpy( sPin_Block, p_msg+len1, 16 );
         len1 += 16;

         /* Get Pin Block Format */
         memset( Pin_Block_Format, 0x00, sizeof(Pin_Block_Format) );
         memcpy( Pin_Block_Format, p_msg+len1, 2 );
         len1 += 2;

         /* Get Card Number */
         memset( Card_Number, 0x00, sizeof(Card_Number) );
         memcpy( Card_Number, p_msg+len1, 12 );
         len1 += 12;
      }
      else
      {
         validity = 254;
      }
   }
   return(validity);
}  // end parse_host_request



//****************************************************************************
//***
//*** FUNCTION:   host_notify
//*** PURPOSE:    To initiate processing of an incoming
//***               request to the Host. This function will
//***               cause a response to be generated and sent.
//*** 
//*** ARGUMENTS:  p_req = A pointer to the complete message,
//***                 i.e. the Host Request.
//*** 
//***             len    = The length of the complete 
//***                 message in bytes.
//*** 
//*** NOTES:      This function has to initiate the processing
//***          cycle for incoming requests.
//*** 
//*** 
//****************************************************************************
void host_notify( LPBYTE p_req, WORD len, pIPC_INFO p_ipc_info )
{
   long field_error = 0;     // Error field, if any
   int  write_status; 
   unsigned char temp_msg[1000];
   /*char field_message[64];*/      // Error message, if any


   if (priority_set == false)
   {
      SetThreadPriority (GetCurrentThread(), THREAD_PRIORITY_HIGHEST);
      priority_set = true;
   }

   memcpy( temp_msg, p_req, len );

   // Initialize
   if (g_using_tpdu_prefix == true)
   {
      // Save TPDU for later use
      memcpy (save_tpdu, p_req, sizeof(save_tpdu));
   }

   // Parse the incoming message
   field_error = parse_host_request (p_req, len);

   if (field_error != 0)
   {
      // Report the bad field to the user
      printf("ERROR: Bit field %d is invalid!", field_error);
      return;
   }

   // Update the message prototype to provide all information
   update_iso_response ();

   // Now generate a response message
   response_buffer_length = generate_response ();

   
   //MessageBox( NULL, "Request Processed", NULL, MB_OK );

   // Now write the response out to the network
   /*tcp_write(response_buffer_length, host_resp_buffer);*/

   if (response_buffer_length > 0) 
   {
      //MessageBox( NULL, "Sending Response", NULL, MB_OK );
      //write_to_comm_trace_file( host_resp_buffer, response_buffer_length, '1' );
       write_status = port_write(response_buffer_length, host_resp_buffer);
      if (!write_status)
         connection_up = FALSE;
   }
}  // end host_notify


/******************************************************************************
 *
 *  NAME:         INCREMENT_HEX_VALUE
 *
 *  DESCRIPTION:  This function takes a single character in the range of
 *                0-9,A-F.  It outputs this value incremented.  F rolls
 *                over to 0.
 *
 *  INPUTS:       in_char - Character to be incremented.
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   out_char - The incremented value.
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
CHAR increment_hex_value( CHAR in_char )
{
   CHAR  out_char;

   switch( in_char )
   {
      case '0':  out_char = '1'; break;
      case '1':  out_char = '2'; break;
      case '2':  out_char = '3'; break;
      case '3':  out_char = '4'; break;
      case '4':  out_char = '5'; break;
      case '5':  out_char = '6'; break;
      case '6':  out_char = '7'; break;
      case '7':  out_char = '8'; break;
      case '8':  out_char = '9'; break;
      case '9':  out_char = 'A'; break;
      case 'A':  out_char = 'B'; break;
      case 'B':  out_char = 'C'; break;
      case 'C':  out_char = 'D'; break;
      case 'D':  out_char = 'E'; break;
      case 'E':  out_char = 'F'; break;
      case 'F':  out_char = '0'; break;
      default:   out_char = in_char; break;
   }
   return( out_char );
}

