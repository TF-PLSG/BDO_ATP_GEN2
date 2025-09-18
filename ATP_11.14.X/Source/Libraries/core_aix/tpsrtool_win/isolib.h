
#ifndef ISOLIB_H
#define ISOLIB_H


#include "basictyp.h"





void genutil_bcd_to_asc (BYTE bstring[], INT blen, pCHAR astr, INT astr_len);
INT genutil_bcd_to_int (pBYTE  p_bcd_data, INT bcd_len);
BOOLEAN genutil_blank_string(pCHAR astr, INT max_len);
int isoutil_CondExpd(enum bitfields bitfield, pCHAR trantype);
int isoutil_CondPan(enum bitfields bitfield, pCHAR p_trantype);
int isoutil_CondProc(enum bitfields bitfield, pCHAR trantype);
int isoutil_CondTrack1 (enum bitfields bitfield, pCHAR trantype);
struct bitfield_data *isoutil_Find_Bitfield_Table (enum bitfields bitfield);
void isoutil_GenericMove(enum bitfields fieldid);
int isoutil_Move_Always(enum bitfields bitfield, pCHAR trantype);
int isoutil_Move_IfThere(enum bitfields fieldid, pCHAR trantype);
BOOLEAN isoutil_verify_msg_type();
BOOLEAN isoutil_parse_terminal_request_or_response_message(pBYTE in_msg, int len, int field_no, pBYTE out_ptr);
BOOLEAN isoutil_RspMoveGeneric(enum bitfields fieldid);
BOOLEAN isoutil_RspMovevBCD (enum bitfields fieldid);
BOOLEAN isoutil_RspMovevData (enum bitfields fieldid);
void isoutil_vBCDMove(enum bitfields fieldid);
void isoutil_vDataMove(enum bitfields fieldid);
BOOLEAN isoutil_verify_fieldlen (enum bitfields fieldid, int movelen);


#endif
