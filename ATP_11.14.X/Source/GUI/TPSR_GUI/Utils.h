// Utils.h: interface for the CUtils class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_UTILS_H__F1CDC332_D6A6_451A_80CB_164FE8D7266E__INCLUDED_)
#define AFX_UTILS_H__F1CDC332_D6A6_451A_80CB_164FE8D7266E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "basictyp.h"

class CUtils  
{
public:
	CUtils();
	WORD bcd2bin(const pBYTE lpBcd);
	void bin2bcd(pBYTE lpBcd, const WORD wInt);
	void bin_to_ascii (pCHAR src, pCHAR dst, INT len);
	void ascii_to_bin (pBYTE src, pBYTE dst, INT len);
	virtual ~CUtils();

};

#endif // !defined(AFX_UTILS_H__F1CDC332_D6A6_451A_80CB_164FE8D7266E__INCLUDED_)
