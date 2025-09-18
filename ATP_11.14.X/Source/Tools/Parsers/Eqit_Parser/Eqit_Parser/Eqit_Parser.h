
// Eqit_Parser.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols


// Cex_amex_parserApp:
// See Eqit_Parser.cpp for the implementation of this class
//

class Cex_amex_parserApp : public CWinApp
{
public:
	Cex_amex_parserApp();

// Overrides
public:
	virtual BOOL InitInstance();

// Implementation

	DECLARE_MESSAGE_MAP()
};

extern Cex_amex_parserApp theApp;