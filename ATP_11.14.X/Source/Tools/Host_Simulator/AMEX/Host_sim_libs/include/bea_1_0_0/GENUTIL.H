/**************************************************************************************************
*  
* MODULE:      GENUTIL.H
*
* Copyright (c) 1998, Hypercom, Inc.
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
* $Log:   N:\PVCS\PTE\bea\headers\GENUTIL.H  $
   
      Rev 1.0   08 Sep 1999 10:53:58   epriyev
   initial BEA load
   
      Rev 1.0   03 Feb 1999 10:40:58   jhapitas
   Intial Revision
   
      Rev 1.0   Dec 04 1998 14:44:04   MSALEH
   initial release
*
**************************************************************************************************/

void		genutil_asc_to_bcd (pCHAR  afield, INT how_many, BYTE bstring[]);
void		genutil_asc_to_vbcd (pCHAR afield, int fld_len, unsigned char bcd_buf[]);
void		genutil_bcd_to_asc (unsigned char bstring[], int blen, char *astr, int astr_len);
INT		genutil_bcd_to_int (pBYTE  p_bcd_data, INT bcd_len);
void		genutil_bin_to_ascii (unsigned char bstring[], int blen, char	*astr, int astr_len);
BOOLEAN	genutil_blank_string(pCHAR astr, INT max_len);
void		genutil_format_date(pCHAR temp);
void	   genutil_get_strtime ( pCHAR);
void		genutil_hex_to_bin (pCHAR  afield, int  how_many,unsigned char bstring[]);
int		genutil_hex_to_int ( char *buf_ptr, int   buf_len);
int		genutil_minim (int a, int b);
void	   genutil_opermon_update(pCHAR msg_id, pCHAR msg, pCHAR app_name, pCHAR etype, pCHAR severity);
void	   genutil_process_error(pCHAR func_name, pCHAR err_code_str, pCHAR err_msg, pCHAR tid,
                           pCHAR disp_abort1, pCHAR disp_abort2, int log_event);
void		genutil_str_asc_to_bcd (char  *astr, int  how_many, unsigned char bstring[]);

