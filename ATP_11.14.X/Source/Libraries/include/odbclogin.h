/******************************************************************************
  
        Module: odbclogin.h
  
         Title: 
  
   Description: 
      This is the implementation of the ODBCLogin class which is
      utilized by all Pinnacle GUI applications that are invoked from
      the "Pinnacle Menu" application.
      This class implements the following;
      1) Reads the "command line" and parses arguments passed from
         the "Pinnacle Menu" concerning ODBC and method of communication.
         ie; XIPC or TCP/IP
      2) Based on the command line info it logs into the ODBC datasource 
         and sets a switch indicating success or failure. The GUI app 
         instantiating this class will check this switch and deny or
         accept access.

      Special Note:
      To debug a GUI application without invoking it from the "pinmenu"
      go to "Project Build Settings", "Debug" tab and set the "Program Arguments"
      to the line below. Make sure to include all the | characters.

      |unimed|unimed|unimed|0|XIPC hostname|

      The meanings of the arguments are as follows;
      1) ODBC Datasource Name
      2) ODBC userid
      3) ODBC password
      4) TCP/XIPC switch  
         0 means the connection type is XIPC.
         1 means the connection type is TCP/IP. 
      5) XIPC hostname. If you are not sure what it is, go to the command line
         and enter "set xipchost".
         If the connection type is TCP/IP, this will be an IP address.

   $Log:   N:\PVCS\PTE\SYSTEM\odbclogin.h  $
   
      Rev 1.1   Apr 09 1998 09:11:26   drodden
   Removed all headers that were not required.  
   Changed to include PTE headers with "" rather than <>.
   
   
      Rev 1.0   05 Feb 1998 18:53:54   muysal
   Initial Release.

******************************************************************************/

/////////////////////////////////////////////////////////////////////////////
// CLogin window
#ifndef LOGIN
#define LOGIN

#define  MAX_DSN      30
#define  MAX_UID      30
#define  MAX_PWD      30
#define  MAX_IPADD    20
#define  MAX_XIPCHOST 30
 
class CODBCLogin 
{
// Construction
public:
	BOOL bLogonSuccess;
	void ODBCLogon();
	void FreeConnect();
	int  TCPSwitch;			             // 0 = XIPC, 1 = TCP/IP, 2 = error  
    char szIPAddress[MAX_IPADD+1];	     //IP Address string
    char szXIPCHostName[MAX_XIPCHOST+1]; //XIPC hostname string
// Attributes
public:


};

/////////////////////////////////////////////////////////////////////////////

#endif
