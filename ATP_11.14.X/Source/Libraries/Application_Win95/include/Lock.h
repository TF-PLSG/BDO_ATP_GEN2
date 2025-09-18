// GlobalLock.h: interface for the GlobalLock class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GLOBALLOCK_H__8349853F_82E6_4EBB_BC43_97685831BEDC__INCLUDED_)
#define AFX_GLOBALLOCK_H__8349853F_82E6_4EBB_BC43_97685831BEDC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "stdafx.h"
#include <afxmt.h>

class CDMLock  
{
public:
	CDMLock(const CString & name);
	virtual ~CDMLock();
	bool Unlock();
	bool Lock(DWORD timeout = INFINITE);
   
private:
	HANDLE mutex;
};

#endif // !defined(AFX_GLOBALLOCK_H__8349853F_82E6_4EBB_BC43_97685831BEDC__INCLUDED_)
