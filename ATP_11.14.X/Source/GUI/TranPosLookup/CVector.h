// SimpleArray.h: interface for the CVector class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SIMPLEARRAY_H__CA8E70E2_FF4E_49DB_9F68_BB691D034813__INCLUDED_)
#define AFX_SIMPLEARRAY_H__CA8E70E2_FF4E_49DB_9F68_BB691D034813__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include <vector>
using namespace std;

template < class TArr >
class CVector  
{
	

public:
	vector< TArr > array;
	CVector()
	{
	}
	virtual ~CVector()
	{
	}

	CVector(const CVector< TArr >& src) 
	{
		array = src.array;
	}
	CVector< TArr >& operator=(const CVector< TArr >& src)
	{
		array = src.array;
		return *this;
	}

	int GetSize()
	{
		return array.size();
	}

	BOOL Add(const TArr& t)
	{
		array.push_back(t);
		return TRUE;
	}
	TArr& operator[] (int nIndex)
	{
		return array[nIndex];
	}

	BOOL RemoveAt( int nPos)
	{
		if(nPos < array.size())
		{
			array.erase( array.begin() + nPos );
			return TRUE;
		}
		return FALSE;
	}
	void RemoveAll()
	{
		array.clear();
	}

};

#endif // !defined(AFX_SIMPLEARRAY_H__CA8E70E2_FF4E_49DB_9F68_BB691D034813__INCLUDED_)
