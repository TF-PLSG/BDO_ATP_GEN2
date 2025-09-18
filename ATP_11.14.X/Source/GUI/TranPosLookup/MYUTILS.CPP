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
* $Log:   N:\PVCS\PTE\Equitable\tran_lookup\MYUTILS.CPP  $
   
      Rev 1.3   Jul 28 1999 16:55:18   iarustam
   Bug fixes
   
      Rev 1.2   Jun 24 1999 16:14:06   iarustam
   Bug fixes
   
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
									"TranPosSearchGenIIInstance");
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
		strcpy (strErrorMessage, "ERROR: Invalid character" );
		return "Error";
	}

	// Make sure that I have only 1 "." 
	int pos = strMyAmount.Find (".");
	CString strReverse = strMyAmount;
	strReverse.MakeReverse();
	int nRevpos = strReverse.Find (".");

	if ( pos + nRevpos + 1 != nCurLength && pos != -1 )
	{
		strcpy (strErrorMessage, "ERROR: Invalid format");
		return "Error";
	}
   
	// if the"."entered on the wrong position
	if ( pos  != nCurLength-3 && pos != -1 )
	{
		strcpy (strErrorMessage, "ERROR: Invalid format");
		return "Error";
	}
	// if they entered to many digits
	if ( pos == -1 && nCurLength > nLength )
	{
		strcpy (strErrorMessage, "ERROR: Invalid format");
		return "Error";
	}

	// add 00 at the end
	if ( pos == nCurLength - 1 || pos == -1 )
	{
		if ( nCurLength < nLength - 2 )
			strMyAmount = strMyAmount + "00";
		else if  ( nCurLength == nLength - 1 )
			strMyAmount = strMyAmount + "0";
			
	}
	if ( pos == nCurLength - 2 && nCurLength < nLength - 1 && pos != -1)
		strMyAmount = strMyAmount + "0";
	else if ( pos == nCurLength - 2 && nCurLength >= nLength - 1)
	{
		strcpy (strErrorMessage, "ERROR: Invalid format");
		return strMyAmount;

	}

	// if "." is not in the rigth place
	if ( pos < nCurLength -3 && pos != -1 )
	{
		strcpy (strErrorMessage, "ERROR: Invalid format");
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
//	if ( strMyAmount.IsEmpty() )
//		strMyAmount = "0";

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

