// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers
//#define _WIN32_WINNT 0x0400
#define _WIN32_WINNT 0x0501
#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT




#define LS_SERVICE_NAME	0 
#define LS_STATUS		1 
#define LS_NETWORK_NAME	2 
#define LS_RECEIVED		3 
#define LS_SENT			4 
#define LS_RATE			5 
#define LS_APPROVED		6 
#define LS_DECLINED		7 
#define LS_TIMEDOUT		8 
#define LS_REVERSED		9 
#define LS_MGT_COUNTS	10
#define LS_STIP_SENT	11
#define LS_STIP_RECV	12

