/*********************************************************************************
* Module:
*
* Title:
*
* Description:
*
* Application:
*
* Author:
*
* Revision History:
*  
* $Log:   N:\PVCS\PTE\Equitable\ATP_gui\MENUTAB.H  $
   
      Rev 1.9   Dec 01 2003 11:29:32   lmitchel
   Added AMEX Online Updates - scr1200
   
      Rev 1.8   Jan 05 2000 14:53:44   gbarnes
   Major update. Not updated to PVCS recently.
   PVCS was locked.
   
      Rev 1.1   Sep 22 1999 15:51:16   iarustam
   Added manual batch entry GUI
   
      Rev 1.0   Jul 01 1999 14:17:36   iarustam
   Initial Release
   
      Rev 1.0   Jun 28 1999 13:58:40   iarustam
   Initial Release
   
      Rev 1.5   Jun 04 1999 17:40:26   iarustam
   Bug fixes
   
      Rev 1.4   May 10 1999 13:39:08   dperson
   Updated comments
   
      Rev 1.3   May 10 1999 11:21:58   dperson
    Added PVCS header
    
*   
************************************************************************************/

#ifndef __MENUTABLE__
#define __MENUTABLE__

#define VIEW_BYTE 3


struct menu_struct 
{
	CHAR menu_str[40];
	INT  position;
}; 

// table mapping command strings to command types
static struct menu_struct menu_table[] = 
{
	{ "System Administration",			 0 },
	{ "System Configuration",			 1 },
	{ "System Operations",				 2 },
	{ "BIN Control",					 3 },
	{ "Authorization Control",			 4 },
	{ "Merchant Control",				 5 },
	{ "Device Definition",				 6 },
	{ "Network Control",				 7 },
	{ "Card Account Control",			 8 },
    { "Voice Authorization",			 9 },
    { "Transaction Lookup",				 10 },
    { "Dual Encryption",				 11 },	
	{ "Service Manager",				 12 },
 	{ "Override Merchant Denial",		 13 },
	{ "System Global Parameter",         14 },
	{ "User Status Control",             15 },
	{ "Fraud Control",					 16	},
	{ "ChipFallBack Rules Definition",	 17 },
	{ "HELP",	 18 }
/*	{ "Recurring Payment",		13 },*/
 
};


#define N_MENUENTRY ( sizeof( menu_table ) / sizeof( struct menu_struct ) )


#endif

