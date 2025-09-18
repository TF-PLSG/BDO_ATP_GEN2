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
* $Log:   N:\PVCS\PTE\Equitable\ATP_gui\MYUTILS.CPP  $
   
      Rev 1.18   Dec 01 2003 11:29:46   lmitchel
   Added AMEX Online Updates - scr1200
   
      Rev 1.17   Feb 27 2003 10:05:16   lmitchel
   SCR955 - Modifications to expand the following key fields from 16 to 48 bytes:  IDC_CWK_FLD, IDC_ZMK_FLD, IDC_TPK_FLD.  Added validation routines to verify field entry is in Hex digits only and to check length of key entered.
   
      Rev 1.16   Nov 09 2001 16:46:48   lmitchel
   GUI support for RSB cards - Added 2 new fields, add_on_rate, add_on_term to card general page including support for add, update, view, change history
   
      Rev 1.15   Apr 23 2001 09:21:12   SYARLAGA
   Numbered the invalid format statements and invalid character statement
   
      Rev 1.14   Jan 21 2000 12:23:16   iarustam
   Bug fixes # 14,24,,52,63,65,151,165,91,228,202,180,205,504,152,179,251,109,182,177,183,224,225,217,229,230,231,247,200,199,203,201,206,172
   
      Rev 1.13   Jan 05 2000 14:53:44   gbarnes
   Major update. Not updated to PVCS recently.
   PVCS was locked.
   
      Rev 1.2   Sep 30 1999 16:17:20   npogosya
   Bug fixes
   
      Rev 1.1   Sep 22 1999 15:51:24   iarustam
   Added manual batch entry GUI
   
      Rev 1.11   Jul 08 1999 16:06:50   iarustam
   Bug fixes
   
      Rev 1.0   Jul 01 1999 14:17:38   iarustam
   Initial Release
   
      Rev 1.0   Jun 28 1999 13:58:40   iarustam
   Initial Release
   
      Rev 1.10   Jun 16 1999 10:02:46   iarustam
   Bug fixes
   
      Rev 1.9   Jun 02 1999 16:25:18   iarustam
   Bug fixes
   
      Rev 1.8   May 21 1999 10:16:54   iarustam
   Bug fixes
   
   
      Rev 1.7   May 13 1999 14:57:32   iarustam
   Bug fixes
   
      Rev 1.6   May 10 1999 11:27:32   dperson
   Standardized error messages
   
      Rev 1.5   Apr 29 1999 14:56:28   iarustam
   Bug fixes
   
      Rev 1.4   Mar 10 1999 12:11:46   iarustam
   Bug fixes
   
   
      Rev 1.3   Mar 08 1999 17:20:32   iarustam
   Bug fixes
   
   
      Rev 1.2   Mar 04 1999 13:32:06   iarustam
   Bug fixes
   
      Rev 1.0   Feb 23 1999 11:40:34   iarustam
   Initial Release
   
      Rev 1.0   Feb 17 1999 10:09:32   iarustam
   Initial Release
   
      Rev 1.1   Jan 22 1999 10:59:00   iarustam
   Bug fixes and core 1_3_0 changes
   
      Rev 1.1   Jan 21 1999 13:08:16   iarustam
   Bug fixes
   
   
      Rev 1.1   Dec 18 1998 13:42:08   iarustam
    
*
************************************************************************************/
// MyUtils.cpp : implementation file
//

#include "stdafx.h"
#include "MyUtils.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMyUtils

CMyUtils::CMyUtils()
{
}

CMyUtils::~CMyUtils()
{
}


BEGIN_MESSAGE_MAP(CMyUtils, CWnd)
	//{{AFX_MSG_MAP(CMyUtils)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CMyUtils message handlers

int CMyUtils::IsEmpty (CString& strMyString, CDialog* _this)
{
	strMyString.TrimLeft() ;
	strMyString.TrimRight();
	_this->UpdateData(FALSE);
	if (strMyString.IsEmpty())
		return 1;
	else return 0;

}

BOOLEAN CMyUtils::IfExistsPreviousInstance() 
{
	m_hFileMappingObject = CreateFileMapping ((HANDLE) 0xFFFFFFFF,
									NULL,
									PAGE_READONLY,
									0,
									32,
									"ATPGenIIGUIInstance");
	if (m_hFileMappingObject) {
		if (GetLastError() == ERROR_ALREADY_EXISTS) 
			return TRUE ;
		else return FALSE ;
	}
	else {
		AfxGetMainWnd()->MessageBox ("Error creating File Mapping") ;
		return FALSE;
	}
}

BOOLEAN CMyUtils::ActivateCurrentInstance() 
{
	
	CString strApplicationTitle;
	strApplicationTitle.LoadString(AFX_IDS_APP_TITLE) ;
		
	CWnd* pwndFirst = CWnd::FindWindow(NULL,(LPCTSTR)strApplicationTitle);
	if (pwndFirst) {
		pwndFirst->SetForegroundWindow();
		if (pwndFirst->IsIconic())
			pwndFirst->ShowWindow(SW_RESTORE);
					
	}
	return TRUE;
}

void CMyUtils::GetSystemTime ( pCHAR strTime )
{
    char strHour[3];
    char strMinute[3];
    char strSecond[3];

    int nHour, nMinute, nSecond;

  //  char strTime[7];


    memset (&SystemTime, 0, sizeof(SystemTime)) ;
    memset ( strHour, 0, sizeof (strHour));
    memset ( strMinute, 0, sizeof (strMinute));
    memset ( strSecond, 0, sizeof (strSecond));
    memset ( strTime, 0, sizeof (strTime));
    
    GetLocalTime(&SystemTime) ;

    nHour = SystemTime.wHour;
    nMinute = SystemTime.wMinute;
    nSecond = SystemTime.wSecond;

    itoa ( nHour, strHour, 10 );
    if ( nHour < 10 )
    {
        strcpy ( strTime, "0");
        strcat ( strTime, (char *)strHour);
    }
    else
        strcpy ( strTime, (char *)strHour);

    itoa ( nMinute, strMinute, 10 );
    if ( nMinute < 10 )
    {        
        strcat ( strTime, "0");
        strcat ( strTime, (char *)strMinute);
    }
    else
        strcat ( strTime, (char *)strMinute);

    itoa ( nSecond, strSecond, 10 );
    if ( nSecond < 10 )
    {        
        strcat ( strTime, "0");
        strcat ( strTime, (char *)strSecond);
    }
    else
        strcat ( strTime, (char *)strSecond);
}

void CMyUtils::GetSystemDate( pCHAR strDate)
{

    char strDay[3];
    char strMonth[3];
    char strYear[5];    
   
    int nDay, nMonth, nYear;

    memset (&SystemTime, 0, sizeof(SystemTime)) ;
    GetLocalTime(&SystemTime) ;

    nDay = SystemTime.wDay;
    nMonth = SystemTime.wMonth;
    nYear = SystemTime.wYear;

    memset ( strDay, 0, sizeof (strDay));
    memset ( strMonth, 0, sizeof (strMonth));
    memset ( strYear, 0, sizeof (strYear));

    
    itoa ( nMonth, strMonth, 10 );
    if ( nMonth < 10 )
    {        
        strcpy ( strDate, "0");
        strcat ( strDate, (char *)strMonth);
    }
    else
        strcpy ( strDate, (char *)strMonth);

    
    itoa ( nDay, strDay, 10 );
    if ( nDay < 10 )
    {
        strcat ( strDate, "0");
        strcat ( strDate, (char *)strDay);
    }
    else
        strcat ( strDate, (char *)strDay);

    itoa ( nYear, strYear, 10 );
    strcat ( strDate, strYear );

    
}

CString CMyUtils::GenerateAmount( CString strMyAmount, int nLength )
{
	int nCurLength;

	strMyAmount.TrimLeft() ;
	strMyAmount.TrimRight();
	if (strMyAmount.IsEmpty())
		return strMyAmount;

	strMyAmount = strMyAmount + "00";

	nCurLength = strMyAmount.GetLength();

	for ( int i = 0; i < nLength-nCurLength; i++ )
		strMyAmount = "0" + strMyAmount;

	return strMyAmount;

}

CString CMyUtils::GenerateTranAmount( CString strMyAmount, int nLength, pCHAR strErrorMessage )
{
	int nCurLength;

	strMyAmount.TrimLeft() ;
	strMyAmount.TrimRight();
	if (strMyAmount.IsEmpty())
		return strMyAmount;
    
	nCurLength = strMyAmount.GetLength();

	// check for invalid character
	int res = strspn ( strMyAmount, "1234567890.");	
	if ( res < nCurLength )
	{
		strcpy (strErrorMessage, "ERROR: Invalid character entered" );
		return "Error";
	}

	// Make sure that I have only 1 "." 
	int pos = strMyAmount.Find (".");
	CString strReverse = strMyAmount;
	strReverse.MakeReverse();
	int nRevpos = strReverse.Find (".");

    if ( pos == -1 && nCurLength > nLength-2 )//no decimal entered; CurLength must allow for 2 decimal digits	
    {
		strcpy (strErrorMessage, "ERROR: Length exceeded");
		return "Error";
	}

	if ( 1 == nRevpos )
		strMyAmount = strMyAmount + "0";
	else if ( 0 == nRevpos ) 
		strMyAmount = strMyAmount + "00";
	nCurLength = strMyAmount.GetLength();

    pos = strMyAmount.Find (".");
    strReverse = strMyAmount;
	strReverse.MakeReverse();
	nRevpos = strReverse.Find (".");

    if ( nCurLength-1 > nLength )//check length	
    {
		strcpy (strErrorMessage, "ERROR: Length exceeded");
		return "Error";
	}


	if ( pos + nRevpos + 1 != nCurLength && pos != -1 || nRevpos == 1)
	{
		strcpy (strErrorMessage, "ERROR: Invalid format");
		return "Error";
	}


	// if the"."entered on the wrong position
	if ( pos  != nCurLength-3 && pos != -1 )
	{
		strcpy (strErrorMessage, "ERROR: Invalid decimal format");
		return "Error";
	}
	// if they entered too many digits
	if ( pos == -1 && nCurLength > nLength )
	{
		strcpy (strErrorMessage, "ERROR: Invalid; Length exceeded");
		return "Error";
	}

	// add 00 at the end
	if ( pos == nCurLength - 1 || pos == -1 )
	{
		if ( nCurLength <= nLength - 2 )
			strMyAmount = strMyAmount + "00";
		else if  ( nCurLength == nLength - 1 )
			strMyAmount = strMyAmount + "0";
			
	}
	if ( pos == nCurLength - 2 && nCurLength < nLength - 1 && pos != -1)
		strMyAmount = strMyAmount + "0";
	else if ( pos == nCurLength - 2 && nCurLength >= nLength - 1)
	{
		strcpy (strErrorMessage, "ERROR: Invalid format 5");
		return strMyAmount;

	}

	// if "." is not in the rigth place
	if ( pos < nCurLength -3 && pos != -1 )
	{
		strcpy (strErrorMessage, "ERROR: Invalid decimal position.");
		return strMyAmount;
	}

	nCurLength = strMyAmount.GetLength();

	// remove "." from amount
	if ( pos != -1 )
		strMyAmount = strMyAmount.Left(nCurLength-3) + strMyAmount.Right(2);

	nCurLength = strMyAmount.GetLength();

	for ( int i = 0; i < nLength-nCurLength; i++ )
		strMyAmount = "0" + strMyAmount;

	return strMyAmount;

}



CString CMyUtils::GetAmount( CString strMyAmount )	
{
	int nCurLength;
	BOOL bDone;

	bDone = FALSE;

	nCurLength = strMyAmount.GetLength();

	strMyAmount = strMyAmount.Left(nCurLength-2);

	while ( !bDone )
	{
		if ( strMyAmount.Left(1) == "0" )
			strMyAmount = strMyAmount.Right(strMyAmount.GetLength()-1);
		else
			bDone = TRUE;
	}
	if ( strMyAmount.IsEmpty() && nCurLength != 0)
		strMyAmount = "0";

	return strMyAmount;

}
	
CString CMyUtils::DisplayAmount( CString strMyAmount )	
{
	int len;
	double nAmount;
	char strSum [50];
   CString neg = "";
   BOOL bNegative;

   bNegative = FALSE;

	nAmount = atof(strMyAmount);

   if ( nAmount < 0 )
   {
      neg = "-";
      bNegative = TRUE;
   }


	gcvt ( nAmount, 15, strSum );

	strMyAmount = strSum;
	// remove the  "."
	if ( strMyAmount.Right(1) == "." )
		strMyAmount =  strMyAmount.Left(strMyAmount.GetLength()-1);

 	len = strMyAmount.GetLength();
   
   if ( bNegative )
      strMyAmount = strMyAmount.Right(len-1);
 	len = strMyAmount.GetLength();

	if ( len > 2 )
		strMyAmount = neg + strMyAmount.Left(len-2) + "." + strMyAmount.Right(2);
	else if ( 2 == len )
		strMyAmount = neg + "0." + strMyAmount ;
   else if ( 1 == len )
		strMyAmount = neg + "0.0" + strMyAmount ;


	return strMyAmount;

}

/*************************************************************************************/
/*************************************************************************************/
INT CMyUtils::GetMod10CheckDigit( pCHAR input_str, INT input_len )
{
   int i, dble = 0, result = 0;
                                       
   for( i = input_len - 1; i >= 0; i-- )
   {
      dble = (input_str[i] - 48 ) * 2;
      result = (dble >= 10) ? ( result + 1 + (dble - 10) ): (result + dble);
      i--;
	  if(i>=0)
      result += (input_str[i] - 48 );
   }

  
   dble = result / 10 ;

   i = ((dble + 1) * 10);
   result = i - result;
   if( result > 9 )
       return ( 0 );
   else
	   return (result);
}


CString CMyUtils::GenerateDate( CString strMyDate )
{
	CString strDay, strMonth, strYear;
	
	if (strMyDate.IsEmpty())
		return strMyDate;

	strDay = strMyDate.Left(2) ;
	strMonth = strMyDate.Mid(2,2);
	strYear = strMyDate.Right(4);
	
	strMyDate = strYear + strMonth + strDay;	

	return strMyDate;

}

CString CMyUtils::FormatDate( CString strMyDate )
{
	CString strDay, strMonth, strYear;

	if(strMyDate.IsEmpty())
		return strMyDate;

	strDay = strMyDate.Right(2) ;
	strMonth = strMyDate.Mid(4,2);
	strYear = strMyDate.Left(4);
	
	strMyDate = strYear + "-" + strMonth + "-"  + strDay;	

	return strMyDate;

}


/*02/18/03 LM SCR955 - Added function to validate for length and Hex entry only */
BOOLEAN CMyUtils::IsHexEntry(CString strKeyString)  
{
	INT key_length = 0;
	INT i;
	
	key_length = strKeyString.GetLength();

	/*Check TPK keys are hex characters only (0-9, A-F) - return to input if they are not else continue*/
	for(i = 0; i < key_length; i++)
	{
		if ( (strKeyString[i] >= 0x30) && (strKeyString[i] <= 0x39)||
			 (strKeyString[i] >= 0x41) && (strKeyString[i] <= 0x46))
			continue;
		else
			return FALSE;
	}

	return TRUE;

} /*end BOOLEAN CMyUtils::IsHexEntry(CString strKeyString) */
CString CMyUtils::GetProductVersionInfo(HMODULE hLib, CString csEntry)
{
  CString csRet;

  if (hLib == NULL)
    hLib = AfxGetResourceHandle();
  
  HRSRC hVersion = FindResource( hLib, MAKEINTRESOURCE(VS_VERSION_INFO), RT_VERSION );
  if (hVersion != NULL)
  {
    HGLOBAL hGlobal = LoadResource( hLib, hVersion ); 
    if ( hGlobal != NULL)  
    {  
  
      LPVOID versionInfo  = LockResource(hGlobal);  
      if (versionInfo != NULL)
      {
        DWORD vLen,langD;
        BOOL retVal;    
    
        LPVOID retbuf=NULL;
    
        static char fileEntry[256];

        sprintf(fileEntry,"\\VarFileInfo\\Translation");
        retVal = VerQueryValue(versionInfo,fileEntry,&retbuf,(UINT *)&vLen);
        if (retVal && vLen==4) 
        {
          memcpy(&langD,retbuf,4);            
          sprintf(fileEntry, "\\StringFileInfo\\%02X%02X%02X%02X\\%s",
                  (langD & 0xff00)>>8,langD & 0xff,(langD & 0xff000000)>>24, 
                  (langD & 0xff0000)>>16, csEntry);            
        }
        else 
          sprintf(fileEntry, "\\StringFileInfo\\%04X04B0\\%s",  GetUserDefaultLangID(), csEntry);

        if (VerQueryValue(versionInfo,fileEntry,&retbuf,(UINT *)&vLen)) 
          csRet = (char*)retbuf;
      }
    }

    UnlockResource( hGlobal );  
    FreeResource( hGlobal );  
  }

  return csRet;
}
