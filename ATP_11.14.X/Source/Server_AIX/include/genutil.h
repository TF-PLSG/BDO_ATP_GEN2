/**************************************************************************************************
*  
* MODULE:      GENUTIL.H
*
* Copyright (c) 2005, Hypercom, Inc.
* All Rights Reserved
*  
* TITLE:       ATP Standard - Generic Terminal/Host Handler Utilities.
*  
* DESCRIPTION: Contains function prototypes and definitions for the terminal/host handler utilities.  
*
* APPLICATION: multiple
*
* AUTHOR:  BGABLE
*
* REVISION HISTORY:
*
* $Log:   N:\POS\PVCS6.6\EPICPORTZ\PTE\Equitable\App_lib\GENUTIL.H  $
   
      Rev 1.7   Jun 10 2005 16:13:46   dirby
   Added a function to copy raw EMV data into the Auth Tx structure.
   SCR 15725
   
      Rev 1.6   Jun 01 2005 17:02:44   dirby
   Added function prototype for function to validate a state code.
   SCR 15607
   
      Rev 1.5   Mar 10 2005 10:55:04   dirby
   Added prototype to convert ASCII to Binary.
   SCR 14238
   
      Rev 1.4   Oct 07 2004 14:47:18   dirby
   Added function prototype and definitions to support converting
   of country codes from alpha to numeric and back.
   SCR 12462
   
      Rev 1.3   Jul 21 2004 16:55:42   dirby
   Added function to find a specific subfield within the raw data of
   an EMV block - finds an EMV tag and returns its value and length.
   SCR 1388
   
      Rev 1.2   Jan 13 2004 13:56:58   dirby
   Added function prototypes to convert Base64 into binary and to
   convert binary into BCD.
   SCR 1117
   
      Rev 1.1   Jan 18 2000 16:35:48   ddabberu
   removed c++ style comments
   
      Rev 1.0   03 Feb 1999 10:40:58   jhapitas
   Intial Revision
   
      Rev 1.0   Dec 04 1998 14:44:04   MSALEH
   initial release
*
**************************************************************************************************/

void     genutil_asc_to_bcd (pCHAR  afield, INT how_many, BYTE bstring[]);
void     genutil_asc_to_vbcd (pCHAR afield, int fld_len, unsigned char bcd_buf[]);
void     genutil_asc_to_bin( pCHAR astr, pBYTE bstr, pINT bstr_len );
void     genutil_bcd_to_asc (unsigned char bstring[], int blen, char *astr, int astr_len);
INT      genutil_bcd_to_int (pBYTE  p_bcd_data, INT bcd_len);
void     genutil_bin_to_ascii (unsigned char bstring[], int blen, char *astr, int astr_len);
BOOLEAN  genutil_blank_string(pCHAR astr, INT max_len);
void     genutil_format_date(pCHAR temp);
void     genutil_get_strtime ( pCHAR);
void     genutil_hex_to_bin (pCHAR  afield, int  how_many,unsigned char bstring[]);
int      genutil_hex_to_int ( char *buf_ptr, int   buf_len);
int      genutil_minim (int a, int b);
void     genutil_opermon_update(pCHAR msg_id, pCHAR msg, pCHAR app_name, pCHAR etype, pCHAR severity);
void     genutil_process_error(pCHAR func_name, pCHAR err_code_str, pCHAR err_msg, pCHAR tid,
                               pCHAR disp_abort1, pCHAR disp_abort2, int log_event);
void     genutil_str_asc_to_bcd (char  *astr, int  how_many, unsigned char bstring[]);
INT      genutil_base64_to_binary( pBYTE in_rec, INT in_len, pBYTE out_rec );
INT      genutil_binary_to_bcd(long long_val, unsigned char bcd[]);
INT      getLengthOfLong( long n );


void ebcdic_to_hex( pBYTE instr, pBYTE outstr, INT len );
void init_character_code_maps();
void convert_to_ascii( pBYTE source, pBYTE dest, INT len );
void hex_to_ebcdic( pBYTE instr, pBYTE outstr, INT len );
void convert_to_ebcdic( pBYTE source, pBYTE dest, INT len );
/* Definitions for converting country codes */
#define  A2N  0   /* Alpha to numeric */
#define  N2A  1   /* Numeric to Alpha */

#define MC06_OCT2023 1

/******************************************************************************
 *
 *  NAME:         GENUTIL_GET_EMV_TAG
 *
 *  DESCRIPTION:  This function finds an EMV tag in the raw EMV data from
 *                a terminal.  The tag can be any length.  The length of the
 *                tag's data field, and the tag's data are returned.
 *
 *                This function uses EMV 98 specifications for determining
 *                the length of a tag as it searches through the data field.
 *                If the last 5 bits of the first byte are set, then another
 *                byte follows.  For example, xxx11111 indicates another byte
 *                follows for the tag. Typical is 0x5F and 0x9F.  For subsequent
 *                bytes, if the most significant bit is set (1xxxxxxx), then
 *                another byte follows.
 *
 *                Here is the format of the EMV field:
 *
 *                   TAG1 LEN1 VALUE1 TAG2 LEN2 VALUE2 . . . TAGn LENn VALUEn
 *
 *                   Each TAG can be multiple bytes in length.
 *                   Each LEN can be 1, 2, or 3 bytes
 *                   Each VALUE can be up to 255 bytes.
 *
 *                The coding of the LEN field is as follows:
 *                When the most significant bit of the length field is set to
 *                0, the length field consists of only one byte.  The other
 *                bits code the number of bytes of the value field.  The
 *                length field is within the range 1 to 127.
 *
 *                When the most significant bit of first byte of the length
 *                field is set to 1, the subsequent bits indicate the number
 *                of subsequent bytes in the length field.  The subsequent
 *                bytes code an integer representing the number of bytes in
 *                the value field.  Two bytes are necessary to express up to
 *                255 bytes in the value field.
 *
 *  INPUTS:       tag_in    - Tag to search for
 *                emv_field - Raw EMV data - all tags
 *                emv_len   - Length of emv_field
 *
 *  OUTPUTS:      tag_value     - String containing value of 'tag_in' if found
 *                tag_value_len - Length of tag_value 
 *
 *  RTRN VALUE:   True if tag is found; false if tag not found.
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
INT genutil_get_emv_tag( pBYTE tag_in, pBYTE emv_field,  INT emv_len,
                                       pBYTE tag_value, pINT tag_value_len );


/******************************************************************************
 *
 *  NAME:         GENUTIL_GET_COUNTRY_CODE
 *
 *  DESCRIPTION:  This function will return convert an alpha country code
 *                into a numeric country code.  Conversely, it can convert a
 *                numeric country code an alpha country code.
 *
 *                Right now, this function only works with 2-char country
 *                codes and 3-digit country codes.  For example, for the
 *                United States, the values:  "US" and "840".
 *
 *                In the future, this function can be expanded to work with
 *                3-char country codes as well.
 *
 *                If a conversion cannot be done due to value not found,
 *                the result will be null for alpha, or zero for numeric.
 *
 *  INPUTS:       method  - Flag to indicate which way to convert:
 *                             A2N = alpha->numeric  or  
 *                             N2A = numeric->alpha
 *                char_cc - 2-char country code or null
 *                num_cc  - 3-digit country code or zero
 *
 *  OUTPUTS:      char_cc - 2-char alpha country code or null
 *                num_cc  - 3-digit numeric country code or null
 *
 *  RTRN VALUE:   None
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
void genutil_get_country_code( INT method, pCHAR char_cc, pCHAR num_cc );


/******************************************************************************
 *
 *  NAME:         GENUTIL_IS_US_STATE_CODE_VALID
 *
 *  DESCRIPTION:  This function will determines if the input string is a
 *                valid State code of the United States.
 *
 *  INPUTS:       state_code - Two-character US State Code
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   True is state_code is valid, else false
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
INT genutil_is_US_state_code_valid( pCHAR state_code );



/******************************************************************************
 *
 *  NAME:         GENUTIL_EMV_TO_AUTH_TX
 *
 *  DESCRIPTION:  Convert ISO8583 Field 55 (EMV Data) from their binary/bcd
 *                fields to their ASCII fields in Auth_Tx.
 *
 *  INPUTS:       emv_raw   - The EMV data in its raw format
 *                total_len - Length of the EMV Raw data
 *
 *  OUTPUTS:      p_auth_tx - Auth Tx structure with EMV in ASCII format
 *
 *  RTRN VALUE:   None
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
void genutil_EMV_To_Auth_Tx( pCHAR emv_raw, INT total_len, pAUTH_TX p_auth_tx );



