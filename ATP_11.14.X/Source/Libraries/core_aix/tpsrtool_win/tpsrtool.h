/******************************************************************************

  Module: tpsrtool.h
  
	Title: Routines for tpsrtool
	
	  Description: 
						  
******************************************************************************/

#ifndef TPSRTOOL_H
#define TPSRTOOL_H

#include <windows.h>

#define WM_REPONSERECEIVED		WM_USER+200
#define WM_TRANSACTIONSENT		WM_USER+201
#define WM_DEVICEMERCHANTID		WM_USER+202
#define WM_STAN					WM_USER+203
#define WM_CONSOLE_HANDLE		WM_USER+204

int tpsr_establish_connection(int);
void set_connection_status(int status);
int PostMessageToParent(UINT message, WPARAM wParam, LPARAM lParam);

#endif

