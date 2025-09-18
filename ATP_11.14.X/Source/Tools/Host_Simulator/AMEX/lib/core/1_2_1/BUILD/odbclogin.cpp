/******************************************************************************
  
        Module: odbclogin.cpp
  
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

   $Log:   N:\PVCS\PTE\SYSTEM\odbclogin.cpp  $
   
      Rev 1.2   Apr 09 1998 09:11:26   drodden
   Removed all headers that were not required.  
   Changed to include PTE headers with "" rather than <>.
   
   
      Rev 1.1   27 Mar 1998 19:12:42   muysal
   1)- Modified odbclogin.cpp so that it would
   be receptive to the "command line" string
   which may be passed from the "Project
   Build Settings, Debug Tab, Program Arguments",
   when debugging GUI applications without
   engaging "Pinnacle Menu".
   2)- Added comments which describe class
   behaviour.
   
   
      Rev 1.0   05 Feb 1998 18:54:24   muysal
   Initial Release.

******************************************************************************/

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif						// _AFX_NO_AFXCMN_SUPPORT
#include "odbclogin.h"


extern "C"
{
  #include "sql.h"
  static HDBC    hdbc;	    //Input	Connection handle for ODBC dsn.
  static HENV    henv;	    //Environment handle for ODBC dsn.
  static HSTMT   *hstmt;    //SQL statement handle
}  


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMyUtils


void CODBCLogin::ODBCLogon()
{
  LPTSTR		lpCmdLineStr;
  CString		szParseStr; 
  CString		szDsn; 
  CString		szCmdLineItem;
  RETCODE		retcode;                        //Return Code
  TCHAR			szCh;
  char			szDSName[MAX_DSN+1];	        //DSN string
  char			szUserId[MAX_UID+1];	        //UserID string
  char			szPassword[MAX_PWD+1];	        //Password string
  char          szTCPSwitch[1];                 //TCP or XIPC switch 
  int			nSearchCtr;
  int			nItemLength;
  int			nItemId;
  int           pipeFl;

  //clear dataset name
  ZeroMemory(szDSName,       sizeof(szDSName));
  //clear userid
  ZeroMemory(szUserId,       sizeof(szUserId));
  //clear password
  ZeroMemory(szPassword,     sizeof(szPassword));
  //clear TCP/XIPC switch
  ZeroMemory(szTCPSwitch,    sizeof(szTCPSwitch));
  //clear XIPC hostname
  ZeroMemory(szXIPCHostName, sizeof(szXIPCHostName));
  //clear IP Address
  ZeroMemory(szIPAddress,    sizeof(szIPAddress));

  nItemId = 0;
  lpCmdLineStr = GetCommandLine();

  szCmdLineItem.Empty();
  szParseStr.Empty();

  szCmdLineItem = lpCmdLineStr;

  nItemLength = szCmdLineItem.GetLength();
  pipeFl = 0;

  for (nSearchCtr = 0; nSearchCtr < nItemLength; nSearchCtr++)
  {
  	szCh = szCmdLineItem.GetAt(nSearchCtr);
	if (pipeFl == 1)
		szParseStr += szCh;
    else
		if (szCh == '|')
		{
			pipeFl = 1;
			szParseStr += szCh;
		}
  }

  nItemLength = szParseStr.GetLength();
  szCmdLineItem.Empty();

  //Only 5 items will be parsed from the "lpCmdLineStr" 

  for (nSearchCtr = 0; nSearchCtr < nItemLength; nSearchCtr++)
  {
		szCh = szParseStr.GetAt(nSearchCtr);
		if (szCh == '|')
		{
			switch (nItemId)
			{				
				case 1:
					//dataset name
				    strcpy(szDSName, szCmdLineItem); 
					szCmdLineItem.Empty();
					break;

             	case 2:
					//userid
					strcpy(szUserId, szCmdLineItem);
					szCmdLineItem.Empty();
					break;

				case 3:
					//password
					strcpy(szPassword, szCmdLineItem);
					szCmdLineItem.Empty();
					break;

                case 4:
					//TCP or XIPC connect switch
					strcpy(szTCPSwitch, szCmdLineItem);
					szCmdLineItem.Empty();
					if (strcmp(szTCPSwitch, "0") == 0)
                        TCPSwitch = 0;
					else
					if (strcmp(szTCPSwitch, "1") == 0)
                        TCPSwitch = 1;
				    else
                        TCPSwitch = 2;		//Indicates connection error from
					                        //the menu application. 
					break;

                case 5:
					//IP Address or the XIPC host name
					if (TCPSwitch == 0)
					   strcpy(szXIPCHostName, szCmdLineItem);
					else
                    if (TCPSwitch == 1)
					   strcpy(szIPAddress, szCmdLineItem);
					else
					if (TCPSwitch == 2)
					{
					   strcpy(szIPAddress,    "ERROR");
					   strcpy(szXIPCHostName, "ERROR");
					}
					szCmdLineItem.Empty();
					break;

			} // end of switch
			nItemId++; 
		}
		else
		      szCmdLineItem += szCh; 

  } // end of for loop

  retcode = SQLAllocEnv(&henv);
  if (retcode == SQL_SUCCESS)
  {
	retcode = SQLAllocConnect(henv,(HDBC FAR *)&hdbc);
	if (retcode == SQL_SUCCESS)
	{
	    retcode = SQLConnect(hdbc,
	              (UCHAR FAR *)szDSName,
	              SQL_NTS,
		(UCHAR FAR *)szUserId,
		SQL_NTS,
		(UCHAR FAR *)szPassword,
		SQL_NTS);
	}
	else
	{
	  AfxMessageBox( "Problem allocating memory for an ODBC connection handle.",
		              MB_ICONEXCLAMATION, 0);
	}
  }
  else
	 AfxMessageBox( "Problem allocating memory for an ODBC environment handle.",
	                 MB_ICONEXCLAMATION, 0);

  if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO )
      bLogonSuccess = TRUE;

  if (retcode != SQL_SUCCESS && retcode != SQL_SUCCESS_WITH_INFO)
      bLogonSuccess = FALSE;

}// end of ODBCLogon()

//Close the ODBC connection.
void CODBCLogin::FreeConnect()
{
    SQLDisconnect(hdbc);
    SQLFreeConnect(hdbc);
    SQLFreeEnv(henv);
}

