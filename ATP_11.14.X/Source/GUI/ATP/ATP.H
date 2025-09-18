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
* $Log:   N:\POS\PVCS6.6\EPICPORTZ\PTE\Equitable\ATP_gui\ATP.H  $

     Rev 1.11   Jan 01 2011 11:30:22   TF-Srikanth
   Added PCI changes calculating idletime of ATP,password validations,Unlocking User account through GUI

      Rev 1.10   Sep 05 2006 10:46:30   dirby
   Added code for a new Cash Bonus program (see CB_... files)
   SCR 22385
   
      Rev 1.9   Jul 07 2005 15:50:06   lmitchel
   Implementation of Fraud Guard Requirements - New Fraud Guard sub  menu under Network Definition
   
      Rev 1.8   Dec 01 2003 11:28:28   lmitchel
   Added AMEX Online Updates - scr1200
   
      Rev 1.7   Dec 13 1999 11:49:10   mbysani
    
   
      Rev 1.2   Oct 15 1999 17:01:34   iarustam
   bug fixes
   
      Rev 1.1   Sep 22 1999 15:40:18   iarustam
   Added manual batch entry GUI
   
      Rev 1.0   Jul 01 1999 14:16:42   iarustam
   Initial Release
   
      Rev 1.0   Jun 28 1999 13:57:46   iarustam
   Initial Release
   
      Rev 1.4   Apr 29 1999 14:55:06   iarustam
   Bug fixes
   
      Rev 1.3   Mar 08 1999 17:19:30   iarustam
   Bug fixes
   
   
      Rev 1.2   Mar 04 1999 13:30:58   iarustam
   Bug fixes
   
      Rev 1.0   Feb 23 1999 11:39:44   iarustam
   Initial Release
   
      Rev 1.0   Feb 17 1999 10:08:48   iarustam
   Initial Release
   
      Rev 1.1   Jan 22 1999 10:56:40   iarustam
   Bug fixes and core 1_3_0 changes
   
      Rev 1.1   Jan 22 1999 09:50:44   iarustam
   Changes for core 1_3_0
   
   
      Rev 1.1   Dec 18 1998 13:40:10   iarustam
    
*
************************************************************************************/
// Atp.h : main header file for the ATP application
//

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols

extern "C"
{
	#include "app_info.h"
	#include "equitdb.h"
	#include "dc_dbstruct.h"
	#include "nc_dbstruct.h"
	#include "tx_dbstruct.h"
#include "ptedes.h"
}

//#include "remotelogin.h"

// dialog modes
#define MODE_ADD	  0
#define MODE_EDIT	  1
#define MODE_VIEW	  2
#define MODE_DONE	  3
#define MODE_DELETE   4
#define MODE_OVERRIDE 5
#define MODE_INQUIRE  6
#define MODE_ACCESS   7
// list modes
#define MODE_GROUP				20
#define MODE_USER				21
#define MODE_BIN				22
#define MODE_AUTH				23
#define MODE_CARD				24
#define MODE_PREFIX				25
#define MODE_CONVERSION			26
#define MODE_MERCHANT			27
#define MODE_PROFILE			28
#define MODE_TERMINAL			29
#define MODE_BATCH				30
#define MODE_WORKSTATION		31
#define MODE_PRIMACC			32
#define MODE_PROFID				33
#define MODE_PROFIDBYTYPE		34
#define MODE_STATUS				35
#define MODE_CCF				36
#define MODE_CREDIT				37
#define MODE_FLEET				38
#define MODE_NEGBIN				39
#define MODE_NEGFILE            40
#define MODE_ACQUIRER			41
#define MODE_ISSUER				42
#define MODE_NETWORK		    43
#define MODE_DISPLAY_USER       44
#define MODE_DISPLAY_GROUP      45
#define MODE_DISPLAY_NEG_GROUP  46
#define MODE_DISPLAY_NEG_GROUP_BY_BIN_RANGE 47
#define MODE_RELATED            48
#define MODE_PURCHASE           49
#define MODE_DEBIT              50
#define MODE_DEFERRED           51
#define MODE_BATCH_HISTORY      52
#define MODE_FGUARD             53
#define MODE_BON02              54
#define MODE_OMC01				55
#define MODE_MCC01				56
#define MODE_MGF01				57
#define MODE_MERCHANT_PAY_PARAM	58
#define MODE_BLOCK_POSENTRYMODES 59 
#define MODE_OBS01				 60
#define MODE_VISA3DS2			61
#define MODE_AMEX3DS2			62
#define MODE_CUP3DS2			63
#define MODE_DINERS3DS2			64
#define MODE_JCB3DS2			65
#define MODE_FCLF01				66
#define MODE_FRULE01			67
#define MODE_RULEID				68
#define MODE_EXCEPTION_LIST     69
#define MODE_RULE_GENERAL     70
#define MODE_CURRENCY_CODE_LIST	71
#define MODE_BDK_KEY01			72
#define MODE_CAF01_KEYBLOCK		73
#define MODE_NCF01_KEYBLOCK		74
#define MAX_SUBSYSTEMS			21
extern "C" {
#include "TXDSAPI.h"

}

/////////////////////////////////////////////////////////////////////////////
// CAtpApp:
// See Atp.cpp for the implementation of this class
//

class CAtpApp : public CWinApp
{
//	virtual BOOL OnIdle(LONG lCount); // return TRUE if more idle processing

public:
	CAtpApp();   

//    CRemoteLogin m_RemoteLoginObject ;
    HANDLE m_hFileMappingObject ;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAtpApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CAtpApp)
	afx_msg void OnAppAbout();
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG

	afx_msg void OnReadDcoMonitorDetails();
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////
