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
* $Log:   N:\POS\PVCS6.6\EPICPORTZ\PTE\Equitable\New_voice_auth\MYUTILS.H  $
   
      Rev 1.2   Apr 04 2005 16:19:42   lmitchel
   Rebuild 4_4_1
   
      Rev 1.1   Aug 17 2000 14:06:20   lmitchel
   Issue #290 - Add Verify button and logic to validate fields prior to sending for authorizaton 
   Also populating: Cardholder name, Merchant Chain, currency code, industry code, merchant category code when verify button selected
   Issue #54 - Added modulus 10 checking on the entered card number.  added Mod10 check box 
   Issue #58 - Commit Authorization button disabled until authorization complete
   
      Rev 1.0   Nov 30 1999 15:16:52   mbysani
   Initial Release
   
      Rev 1.1   Nov 22 1999 16:32:58   mbysani
    
   
      Rev 1.0   Nov 04 1999 16:05:44   iarustam
   Initial Release
   
      Rev 1.0   Mar 30 1999 17:41:08   iarustam
   Initial Release
   
      Rev 1.1   Mar 08 1999 17:20:32   iarustam
   Bug fixes
   
   
      Rev 1.0   Feb 23 1999 11:40:36   iarustam
   Initial Release
   
      Rev 1.0   Feb 17 1999 10:09:32   iarustam
   Initial Release
   
      Rev 1.1   Jan 22 1999 10:59:00   iarustam
   Bug fixes and core 1_3_0 changes
   
      Rev 1.1   Jan 21 1999 13:08:18   iarustam
   Bug fixes
   
   
      Rev 1.1   Dec 18 1998 13:42:08   iarustam
    
*
************************************************************************************/
#ifndef MYUTILS
#define MYUTILS
#include "basictyp.h"

class CMyUtils : public CWnd
{
// Construction
public:
	HANDLE m_hFileMappingObject ;
	int IsEmpty (CString& strStringToCheck, CDialog* _this) ;
	BOOLEAN IfExistsPreviousInstance() ;
	BOOLEAN ActivateCurrentInstance() ;
	CMyUtils() ;

	void GetSystemDate( pCHAR strDate);
    void GetSystemTime( pCHAR strDate);
	CString GenerateAmount ( CString, int );
	CString GetAmount( CString );
    CString DisplayAmount ( CString );
    CString GenerateTranAmount( CString , int , pCHAR  );
	INT GetMod10CheckDigit( pCHAR, INT );


    SYSTEMTIME SystemTime;
// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMyUtils)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CMyUtils();

	// Generated message map functions
protected:
	//{{AFX_MSG(CMyUtils)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

#endif
