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
* $Log:   N:\POS\PVCS6.6\EPICPORTZ\PTE\Equitable\ATP_gui\NAMELIST.CPP  $
   
      Rev 1.10   Sep 05 2006 10:46:32   dirby
   Added code for a new Cash Bonus program (see CB_... files)
   SCR 22385
   
      Rev 1.9   Jul 07 2005 15:49:24   lmitchel
   Implementation of Fraud Guard Requirements - New Fraud Guard sub  menu under Network Definition
   
      Rev 1.8   Dec 01 2003 11:29:48   lmitchel
   Added AMEX Online Updates - scr1200
   
      Rev 1.7   Jan 21 2000 12:23:30   iarustam
   Bug fixes # 14,24,,52,63,65,151,165,91,228,202,180,205,504,152,179,251,109,182,177,183,224,225,217,229,230,231,247,200,199,203,201,206,172
   
      Rev 1.6   Jan 05 2000 14:54:06   gbarnes
   Major update. Not updated to PVCS recently.
   PVCS was locked.
   
      Rev 1.1   Sep 22 1999 15:55:26   iarustam
   Added manual batch entry GUI
   
      Rev 1.0   Jul 01 1999 14:17:54   iarustam
   Initial Release
   
      Rev 1.0   Jun 28 1999 13:59:00   iarustam
   Initial Release
   
      Rev 1.3   May 10 1999 11:27:48   dperson
   Standardized error messages
   
      Rev 1.2   Apr 29 1999 14:56:44   iarustam
   Bug fixes
   
      Rev 1.1   Mar 22 1999 11:12:42   iarustam
   Bug fixes
   
   
      Rev 1.0   Feb 23 1999 11:40:50   iarustam
   Initial Release
   
      Rev 1.0   Feb 17 1999 10:09:44   iarustam
   Initial Release
   
      Rev 1.1   Jan 22 1999 10:59:32   iarustam
   Bug fixes and core 1_3_0 changes
   
      Rev 1.1   Jan 21 1999 13:09:02   iarustam
   Bug fixes
   
   
      Rev 1.1   Dec 18 1998 13:45:26   iarustam
    
*
************************************************************************************/
// NameList.cpp : implementation file
//

#include "stdafx.h"
#include "Atp.h"
#include "NameList.h"
#include "SharedMem.h"
//#include "htmlhelp.h"


extern CSharedMem shrmem;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CString name;

/////////////////////////////////////////////////////////////////////////////
// CNameList dialog


CNameList::CNameList(CWnd* pParent /*=NULL*/)
	: CDialog(CNameList::IDD, pParent)
{
	//{{AFX_DATA_INIT(CNameList)
	m_StrMessageLbl = _T("");
	//}}AFX_DATA_INIT
}


void CNameList::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CNameList)
	DDX_Control(pDX, IDOK, m_OkButton);
	DDX_Control(pDX, IDC_NEXT_BUTTON, m_NextButton);
	DDX_Control(pDX, IDC_LIST1, m_CntlList);
	DDX_Text(pDX, IDC_MESSAGE_LBL, m_StrMessageLbl);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CNameList, CDialog)
	//{{AFX_MSG_MAP(CNameList)
	ON_LBN_DBLCLK(IDC_LIST1, OnDblclkList1)
	ON_WM_HELPINFO()
	ON_BN_CLICKED(IDC_NEXT_BUTTON, OnNextButton)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CNameList message handlers

BOOL CNameList::OnInitDialog() 
{
	CDialog::OnInitDialog();

	char strErrorMessage[200];
	CFont m_font;
	LOGFONT lf;
	int rcode;
	int i, nSize;

	memset ( &lf, 0, sizeof (lf));
	lf.lfHeight = 12;
	lf.lfWeight = 700;
	strcpy(lf.lfFaceName, "Arial");
	m_font.CreateFontIndirect (&lf);
	GetDlgItem(IDC_MESSAGE_LBL)->SetFont (&m_font);

	m_NextButton.EnableWindow (TRUE);
    
    
    SetWindowText ( "Group List");
   
    memset (&sGroupRecord, 0, sizeof (UCF02)) ;
    
	switch ( m_nIndicator ) {
	case MODE_DISPLAY_NEG_GROUP:
		SetWindowText ( "Negative Merchant Group List");    
		nNumberReturned = atoi ((char *)&sNegGroupList.num_returned);                
		if( nNumberReturned == GUI_MAX_LIST_SIZE)
		{				
			CString strTemp = sNegGroupList.ngf01_record[nNumberReturned-1].primary_key.negative_group_id;
			strTemp.TrimRight() ; strTemp.TrimLeft() ;
			memcpy( sNegGroupRecord.primary_key.negative_group_id, (LPCTSTR) strTemp, sizeof (sNegGroupRecord.primary_key.negative_group_id)); 
         nSize = nNumberReturned - 1;			
		}
      else
         nSize = nNumberReturned;			

				
		char str[50] ; memset (str, 0, sizeof(str)) ;           
		for (  i = 0; i < nSize; i++ )
		{
			memcpy ((LPTSTR)str, (LPCTSTR)(sNegGroupList.ngf01_record[i].primary_key.negative_group_id),sizeof (str)) ;
			m_CntlList.AddString(str); 
		}
		if (m_CntlList.GetCount() > 0) m_CntlList.SetCurSel(0) ;
	break;

	case MODE_DISPLAY_NEG_GROUP_BY_BIN_RANGE:
		SetWindowText ( "Negative Merchant Group List");    
		nNumberReturned = atoi ((char *)&sNGFList.num_returned);                
		if( nNumberReturned == GUI_MAX_LIST_SIZE)
		{				
			CString strTemp = sNGFList.negative_group_id[nNumberReturned-1];
			strTemp.TrimRight() ; strTemp.TrimLeft() ;
			memcpy( sNegGroupRecord.primary_key.negative_group_id, (LPCTSTR) strTemp, sizeof (sNegGroupRecord.primary_key.negative_group_id)); 
         nSize = nNumberReturned - 1;			
		}
      else
         nSize = nNumberReturned;			
				
		for (  i = 0; i < nSize; i++ )
		{
			memcpy ((LPTSTR)str, (LPCTSTR)(sNGFList.negative_group_id[i]),sizeof (str)) ;
			m_CntlList.AddString((char *)sNGFList.negative_group_id[i]); 
		}
		if (m_CntlList.GetCount() > 0) m_CntlList.SetCurSel(0) ;
	break;
	case MODE_DISPLAY_USER:
		SetWindowText ( "User List");    

		nNumberReturned = atoi ((char *)&sUserList.num_returned);
		if( nNumberReturned == GUI_MAX_LIST_SIZE)
		{				
			CString strTemp = sUserList.name[nNumberReturned-1] ;
			strTemp.TrimRight() ; strTemp.TrimLeft() ;
			memcpy( sUserRecord.primary_key.name, (LPCTSTR) strTemp, sizeof (sUserRecord.primary_key.name)); 
         nSize = nNumberReturned - 1;			
		}
      else
         nSize = nNumberReturned;			
							
		for ( i = 0; i < nSize; i++ ) {
				m_CntlList.AddString((char *)sUserList.name[i]);                 
		}
		if (m_CntlList.GetCount() > 0) m_CntlList.SetCurSel(0) ;

	break;

	case MODE_DISPLAY_GROUP:
		SetWindowText ( "Group List");    

		nNumberReturned = atoi ((char *)&sGroupList.num_returned);
                
        if( nNumberReturned == GUI_MAX_LIST_SIZE)
        {				
			CString strTemp = sGroupList.group_id[nNumberReturned-1] ;
			strTemp.TrimRight() ; strTemp.TrimLeft() ;
			memcpy( sGroupRecord.primary_key.group_id, (LPCTSTR) strTemp, sizeof (sGroupRecord.primary_key.group_id)); 
         nSize = nNumberReturned - 1;			
		}
      else
         nSize = nNumberReturned;			
			
        memset (str, 0, sizeof(str)) ;
           
		for ( i = 0; i < nSize; i++ ) {
             memcpy ((LPTSTR)str, (LPCTSTR)(&sGroupList.group_id[i]),sizeof (str)) ;
             m_CntlList.AddString(str); 
        }
		if (m_CntlList.GetCount() > 0) m_CntlList.SetCurSel(0) ;

	break;
  

	case MODE_NETWORK:
      SetWindowText ( "Network List");    

	  ZeroMemory ( &sNetworkId, sizeof (NCF01));

	  strcpy((char *)sNetworkId.primary_key.network_type, strNetworkType);
		if ( !strPrimaryKey1.IsEmpty())
			strcpy((char *)sNetworkId.primary_key.network_id, strPrimaryKey1);
		else
			strcpy((char *)sNetworkId.primary_key.network_id, " ");
	
	  rcode = txdsapi_get_list((pBYTE)&sNetworkId, sizeof (NCF01), 
									(pBYTE)&sNetworkList, sizeof (NCF01_GUI_LIST), 
									ST2_DB_GET_NCF01_LIST_BY_TYPE, NCF01_DATA, strErrorMessage) ;	
	  if (rcode == PTEMSG_OK) 
	  {	
	      nNumberReturned = atoi ((char *)&sNetworkList.num_returned);		
          if( nNumberReturned == GUI_MAX_LIST_SIZE)
          {				
		      CString strTemp = sNetworkList.ncf01_record[nNumberReturned-1].primary_key.network_id ;
			  strTemp.TrimRight();
			  strTemp.TrimLeft() ;
			  memcpy( sNetworkId.primary_key.network_id, (LPCTSTR) strTemp, sizeof (sNetworkId.primary_key.network_id)); 
			  memcpy( sNetworkId.primary_key.network_type, strNetworkType, sizeof (sNetworkId.primary_key.network_type)); 
           nSize = nNumberReturned - 1;			
		   }
         else
            nSize = nNumberReturned;			
		  
		  CString temp;
		  //CString temp1;
		 
						
		  for (  i = 0; i < nSize; i++ )
		  {			  
		      temp = sNetworkList.ncf01_record[i].primary_key.network_id;
			  //temp1 = sNetworkList.ncf01_record[i].primary_key.network_type;			 
              
			  //int res = strcmp(temp1,"A");

			  //if( res == 0)
			  m_CntlList.AddString(temp);  			
          }
		  if (nNumberReturned > 0) 
		      m_CntlList.SetCurSel(0) ;
	   }

	break;

	case MODE_NCF01_KEYBLOCK:
      SetWindowText ( "Network ID List");    

	  ZeroMemory ( &sNCF01keyblock01, sizeof (NCF01_KEYBLOCK));

		if ( !strPrimaryKey1.IsEmpty())
			strcpy((char *)sNCF01keyblock01.network_id, strPrimaryKey1);
		else
			strcpy((char *)sNCF01keyblock01.network_id, " ");
	
	  rcode = txdsapi_get_list((pBYTE)&sNCF01keyblock01, sizeof (NCF01_KEYBLOCK), 
									(pBYTE)&sNCF01keyblockList, sizeof (NCF01_KEYBLOCK_GUI_LIST), 
									ST2_DB_GET_NCF01_LIST_BY_TYPE, NCF01_KEYBLOCK01_DATA, strErrorMessage) ;	
	  if (rcode == PTEMSG_OK) 
	  {	
	      nNumberReturned = atoi ((char *)&sNCF01keyblockList.num_returned);		
          if( nNumberReturned == GUI_MAX_LIST_SIZE)
          {				
		      CString strTemp = sNCF01keyblockList.ncf01_keyblock_record[nNumberReturned-1].network_id ;
			  strTemp.TrimRight();
			  strTemp.TrimLeft() ;
			  memcpy( sNCF01keyblock01.network_id, (LPCTSTR) strTemp, sizeof (sNCF01keyblock01.network_id)); 			 
           nSize = nNumberReturned - 1;			
		   }
         else
            nSize = nNumberReturned;			
		  
		  CString temp;
		  //CString temp1;
		 
						
		  for (  i = 0; i < nSize; i++ )
		  {			  
		      temp = sNCF01keyblockList.ncf01_keyblock_record[i].network_id;
			  //temp1 = sNetworkList.ncf01_record[i].primary_key.network_type;			 
              
			  //int res = strcmp(temp1,"A");

			  //if( res == 0)
			  m_CntlList.AddString(temp);  			
          }
		  if (nNumberReturned > 0) 
		      m_CntlList.SetCurSel(0) ;
	   }

	break;
	case MODE_FGUARD:
      SetWindowText ( "Fraud Guard List");    

	  ZeroMemory ( &sFraudGuardRecord, sizeof (FG01));

		if ( !strPrimaryKey1.IsEmpty())
			strcpy((char *)sFraudGuardRecord.name, strPrimaryKey1);
		else
			strcpy((char *)sFraudGuardRecord.name, " ");

	  strcpy((char *)sFraudGuardRecord.type, strNetworkType);
	
	  rcode = txdsapi_get_list((pBYTE)&sFraudGuardRecord, sizeof (FG01), 
									(pBYTE)&sFraudGuardList, sizeof (FG01_GUI_LIST), 
									0, FG01_DATA, strErrorMessage) ;		  
	  if (rcode == PTEMSG_OK) 
	  {	
	      nNumberReturned = atoi ((char *)&sFraudGuardList.num_returned);

          if( nNumberReturned == GUI_MAX_LIST_SIZE) 
          {				
		      CString strTemp = sFraudGuardList.FG01_record[nNumberReturned-1].name ;
			  strTemp.TrimRight();
			  strTemp.TrimLeft() ;
			  memcpy( sFraudGuardRecord.name, (LPCTSTR) strTemp, sizeof (sFraudGuardRecord.name)); 
			  memcpy( sFraudGuardRecord.type, strNetworkType, sizeof (sFraudGuardRecord.type)); 
		  }		  
		  
		  CString temp;		 
						
		  for ( int i = 0; i < nNumberReturned; i++ )
		  {			  
		      temp = sFraudGuardList.FG01_record[i].name;
			  m_CntlList.AddString(temp);  			
          }
		  if (nNumberReturned > 0) 
		      m_CntlList.SetCurSel(0) ;
	   }

	break;

	case MODE_BON02:
      SetWindowText ( "Cash Bonus List");    

	  ZeroMemory ( &sBon02Record, sizeof(BON02));

		if ( !strPrimaryKey1.IsEmpty())
			strcpy((char *)sBon02Record.cash_id, strPrimaryKey1);
		else
			strcpy((char *)sBon02Record.cash_id, " ");

	  rcode = txdsapi_get_list((pBYTE)&sBon02Record, sizeof (BON02), 
									(pBYTE)&sBon02List, sizeof (BON02_GUI_LIST), 
									0, BON02_DATA, strErrorMessage) ;		  
	  if (rcode == PTEMSG_OK) 
	  {	
	      nNumberReturned = atoi ((char *)&sBon02List.num_returned);

          if( nNumberReturned == GUI_MAX_LIST_SIZE) 
          {				
		      CString strTemp = sBon02List.cash_bonus_info[nNumberReturned-1].cash_id;
			  strTemp.TrimRight();
			  strTemp.TrimLeft() ;
			  memcpy( sBon02Record.cash_id, (LPCTSTR) strTemp, sizeof (sBon02Record.cash_id)); 
		  }		  
		  
		  CString temp;		 
						
		  for ( int i = 0; i < nNumberReturned; i++ )
		  {			  
		      temp = sBon02List.cash_bonus_info[i].cash_id;
			  m_CntlList.AddString(temp);  			
          }
		  if (nNumberReturned > 0) 
		      m_CntlList.SetCurSel(0) ;
	   }

	break;

	case MODE_NEGFILE:
		SetWindowText ( "Negative Group ID List");    
		ZeroMemory ( &sNegGroupRecord, sizeof (NGF01));

		if ( !strPrimaryKey1.IsEmpty())
			strcpy((char *)sNegGroupRecord.primary_key.negative_group_id, strPrimaryKey1);
		else
			strcpy((char *)sNegGroupRecord.primary_key.negative_group_id, " ");     

		rcode = txdsapi_get_list((pBYTE)&sNegGroupRecord,sizeof(NGF01), 
									  (pBYTE)&sNegGroupList,  sizeof (NGF01_GUI_LIST),0,
									   NGF01_DATA,	strErrorMessage) ;	
		if (rcode == PTEMSG_OK)
		{
			nNumberReturned = atoi ((char *)&sNegGroupList.num_returned);                
			if( nNumberReturned == GUI_MAX_LIST_SIZE)
			{				
				CString strTemp = sNegGroupList.ngf01_record[nNumberReturned-1].primary_key.negative_group_id;
				strTemp.TrimRight() ; strTemp.TrimLeft() ;
				memcpy( sNegGroupRecord.primary_key.negative_group_id, (LPCTSTR) strTemp, sizeof (sNegGroupRecord.primary_key.negative_group_id)); 
            nSize = nNumberReturned - 1;			
    		}
         else
            nSize = nNumberReturned;			
				
			char str[50] ; memset (str, 0, sizeof(str)) ;           
			for (  i = 0; i < nSize; i++ )
			{
				memcpy ((LPTSTR)str, (LPCTSTR)(sNegGroupList.ngf01_record[i].primary_key.negative_group_id),sizeof (str)) ;
				m_CntlList.AddString(str); 
			}
			if (m_CntlList.GetCount() > 0) m_CntlList.SetCurSel(0) ;
	 }		
   break;
   //////////////////////////////////////////////////////////////
	case MODE_WORKSTATION:
      SetWindowText ( "Workstation List");    
      ZeroMemory ( &sWorkstationRecord, sizeof (ATP02));
     	
	  if ( !strPrimaryKey1.IsEmpty())
		strcpy((char *)sWorkstationRecord.primary_key.workstation_id, strPrimaryKey1);
	  else
		strcpy((char *)sWorkstationRecord.primary_key.workstation_id, " ");     

	   rcode = txdsapi_get_list((pBYTE)&sWorkstationRecord,sizeof(ATP02), 
									 (pBYTE)&sWorkstationList,  sizeof (ATP02_GUI_LIST),0,
									 ATP02_DATA,	strErrorMessage) ;	
	   if (rcode == PTEMSG_OK)
      {
		   nNumberReturned = atoi ((char *)&sWorkstationList.num_returned);                
         if( nNumberReturned == GUI_MAX_LIST_SIZE)
         {				
			   CString strTemp = sWorkstationList.atp_list[nNumberReturned-1].primary_key.workstation_id;
			   strTemp.TrimRight() ; strTemp.TrimLeft() ;
			   memcpy( sWorkstationRecord.primary_key.workstation_id, (LPCTSTR) strTemp, sizeof (sWorkstationRecord.primary_key.workstation_id)); 
            nSize = nNumberReturned - 1;			
		   }
         else
            nSize = nNumberReturned;			
			
         char str[50] ; memset (str, 0, sizeof(str)) ;           
			for (  i = 0; i < nSize; i++ )
         {
            memcpy ((LPTSTR)str, (LPCTSTR)(sWorkstationList.atp_list[i].primary_key.workstation_id),sizeof (str)) ;
            m_CntlList.AddString(str); 
         }
			if (m_CntlList.GetCount() > 0) m_CntlList.SetCurSel(0) ;
      }		
	break;
//////////////////////////////////////////////////////////////////////////////////////
	case MODE_USER:
		SetWindowText ( "User List");    

		ZeroMemory ( &sUserRecord, sizeof (UCF01));

		if ( !strPrimaryKey1.IsEmpty())
			strcpy((char *)sUserRecord.primary_key.name, strPrimaryKey1);
		else
			strcpy((char *)sUserRecord.primary_key.name, " ");    
		  
		rcode = txdsapi_get_list((pBYTE)&sUserRecord, sizeof (UCF01), 
									  (pBYTE)&sUserList,sizeof (UCF01_LIST),ST2_DB_GET_UCF01_USERLIST,
									  UCF01_DATA, strErrorMessage) ;	
		if (rcode == PTEMSG_OK) {
			nNumberReturned = atoi ((char *)&sUserList.num_returned);
			if( nNumberReturned == GUI_MAX_LIST_SIZE)
			{				
				CString strTemp = sUserList.name[nNumberReturned-1] ;
				strTemp.TrimRight() ; strTemp.TrimLeft() ;
				memcpy( sUserRecord.primary_key.name, (LPCTSTR) strTemp, sizeof (sUserRecord.primary_key.name)); 
            nSize = nNumberReturned - 1;			
		   }
         else
            nSize = nNumberReturned;			
							
			for (  i = 0; i < nSize; i++ ) {
					m_CntlList.AddString((char *)sUserList.name[i]);                 
			}
			if (m_CntlList.GetCount() > 0) m_CntlList.SetCurSel(0) ;
		}		

	break;
/////////////////////////////////////////////////////////////////////////////
	case MODE_GROUP:
    ZeroMemory ( &sGroupRecord, sizeof (UCF02));

	if ( !strPrimaryKey1.IsEmpty())
		strcpy((char *)sGroupRecord.primary_key.group_id, strPrimaryKey1);
	else
		strcpy((char *)sGroupRecord.primary_key.group_id, " ");
	  
	rcode = txdsapi_get_list((pBYTE)&sGroupRecord, sizeof (UCF02), 
								  (pBYTE)&sGroupList,sizeof (UCF02_LIST), 0,
								  UCF02_DATA, strErrorMessage) ;	
     
	if (rcode == PTEMSG_OK) {		
		nNumberReturned = atoi ((char *)&sGroupList.num_returned);
                
        if( nNumberReturned == GUI_MAX_LIST_SIZE)
        {				
			CString strTemp = sGroupList.group_id[nNumberReturned-1] ;
			strTemp.TrimRight() ; strTemp.TrimLeft() ;
			memcpy( sGroupRecord.primary_key.group_id, (LPCTSTR) strTemp, sizeof (sGroupRecord.primary_key.group_id)); 
         nSize = nNumberReturned - 1;			
		}
      else
         nSize = nNumberReturned;			
			
        char str[50] ; memset (str, 0, sizeof(str)) ;
           
		for (  i = 0; i < nSize; i++ ) {
             memcpy ((LPTSTR)str, (LPCTSTR)(&sGroupList.group_id[i]),sizeof (str)) ;
             m_CntlList.AddString(str); 
        }
		if (m_CntlList.GetCount() > 0) m_CntlList.SetCurSel(0) ;
	}
	break;
/////////////////////////////////////////////////////////////////////////////
	case MODE_AUTH:
		SetWindowText ( "Authorization Profile List");    

		ZeroMemory ( &sAuthRecord, sizeof (CAF01));
		if ( !strPrimaryKey1.IsEmpty())
			strcpy((char *)sAuthRecord.primary_key.caf_profile_id, strPrimaryKey1);
		else
			strcpy((char *)sAuthRecord.primary_key.caf_profile_id, " ");

		strcpy((char *)sAuthRecord.primary_key.type, strBinType);
		  
		rcode = txdsapi_get_list((pBYTE)&sAuthRecord, sizeof (CAF01), 
									  (pBYTE)&sAuthList,sizeof (CAF01_GUI_LIST), ST2_DB_GET_CAF01_LIST_BY_TYPE,
									  CAF01_DATA, strErrorMessage) ;	
		
		if (rcode == PTEMSG_OK) {	
	
			nNumberReturned = atoi ((char *)&sAuthList.num_returned);		
            if( nNumberReturned == GUI_MAX_LIST_SIZE)
            {				
			    CString strTemp = sAuthList.caf_info[nNumberReturned-1].caf_profile_id ;
			    strTemp.TrimRight() ; strTemp.TrimLeft() ;
			    memcpy( sAuthRecord.primary_key.caf_profile_id, (LPCTSTR) strTemp, sizeof (sAuthRecord.primary_key.caf_profile_id)); 
			    memcpy( sAuthRecord.primary_key.type, strBinType, sizeof (sAuthRecord.primary_key.type)); 
             nSize = nNumberReturned - 1;			
    		}
          else
             nSize = nNumberReturned;	
          
			CString temp;
						
			for (  i = 0; i < nSize; i++ ) {
				temp = sAuthList.caf_info[i].caf_profile_id;
				 m_CntlList.AddString(temp);  			
            }
			if (nNumberReturned > 0) 
				m_CntlList.SetCurSel(0) ;
		}
		
	break;
////////////////////////////////////////////////////////////
	case MODE_CARD:
		SetWindowText ( "Card Type List");    

		ZeroMemory ( &sCardRecord, sizeof (CTF01));

		if ( !strPrimaryKey1.IsEmpty())
			strcpy((char *)sCardRecord.primary_key.card_type, strPrimaryKey1);
		else
			strcpy((char *)sCardRecord.primary_key.card_type, " ");
		  
		rcode = txdsapi_get_list((pBYTE)&sCardRecord, sizeof (CTF01), 
									  (pBYTE)&sCardList,sizeof (CTF01_GUI_LIST), 0,
									  CTF01_DATA, strErrorMessage) ;
		
		if (rcode == PTEMSG_OK) {	
	
			nNumberReturned = atoi ((char *)&sCardList.num_returned);		
            if( nNumberReturned == GUI_MAX_LIST_SIZE)
            {				
			    CString strTemp = sCardList.card_type[nNumberReturned-1] ;
			    strTemp.TrimRight() ; strTemp.TrimLeft() ;
			    memcpy( sCardRecord.primary_key.card_type, (LPCTSTR) strTemp, sizeof (sCardRecord.primary_key.card_type)); 
             nSize = nNumberReturned - 1;			
    		}
          else
             nSize = nNumberReturned;			
						
			for (  i = 0; i < nSize; i++ ) {
				 m_CntlList.AddString((char *)sCardList.card_type[i]);  			
            }
			if (nNumberReturned > 0) 
				m_CntlList.SetCurSel(0) ;
		}
		
   break;
//////////////////////////////////////////////////////////////////////////
	case MODE_CONVERSION:
		SetWindowText ( "Conversion Rate List");    

		ZeroMemory ( &sCurrencyCode, sizeof (CRF01));
		if ( !strPrimaryKey1.IsEmpty())
			strcpy((char *)sCurrencyCode.primary_key.currency_code, strPrimaryKey1);
		else
			strcpy((char *)sCurrencyCode.primary_key.currency_code, " ");

		strcpy((char *)sAuthRecord.primary_key.type, strBinType);
		  
		rcode = txdsapi_get_list((pBYTE)&sCurrencyCode, sizeof (CRF01), 
								  (pBYTE)&sConversionList,sizeof (CRF01_GUI_LIST), 0,
								  CRF01_DATA, strErrorMessage) ;	

		if (rcode == PTEMSG_OK)  { // Dataserver returned DBERR_OK
			nNumberReturned = atoi ((char *)&sConversionList.num_returned);		
            if( nNumberReturned == GUI_MAX_LIST_SIZE)
            {				
			    CString strTemp = sConversionList.currency_code[nNumberReturned-1] ;
			    strTemp.TrimRight() ; strTemp.TrimLeft() ;
			    memcpy( sCurrencyCode.primary_key.currency_code, (LPCTSTR) strTemp, sizeof (sCurrencyCode.primary_key.currency_code)); 
             nSize = nNumberReturned - 1;			
    		}
          else
             nSize = nNumberReturned;			
						
			for (  i = 0; i < nSize; i++ ) {
				 m_CntlList.AddString((char *)sConversionList.currency_code[i]);  			
            }
			if (nNumberReturned > 0) 
				m_CntlList.SetCurSel(0) ;
		}
		
   break;

   	case MODE_FCLF01:
		SetWindowText ( "Fraud Configure List");    
		
	  ZeroMemory ( &fclf01networkid, sizeof (FCLF01));

		if ( !strPrimaryKey1.IsEmpty())
			strcpy((char *)fclf01networkid.network_id, strPrimaryKey1);
		else
			strcpy((char *)fclf01networkid.network_id, " ");
	  rcode = txdsapi_get_list((pBYTE)&fclf01networkid, sizeof (FCLF01), 
									(pBYTE)&fclf01networkidlist, sizeof (FCLF01_GUI_LIST), 
									0, FCLF01_DATA, strErrorMessage) ;	
	  if (rcode == PTEMSG_OK) 
	  {	
	      nNumberReturned = atoi ((char *)&fclf01networkidlist.num_returned);		
          if( nNumberReturned == GUI_MAX_LIST_SIZE)
          {				
		      CString strTemp = fclf01networkidlist.fclf01_list[nNumberReturned-1].network_id ;
			  strTemp.TrimRight();
			  strTemp.TrimLeft() ;
			  memcpy( fclf01networkid.network_id, (LPCTSTR) strTemp, sizeof (fclf01networkid.network_id)); 
			  nSize = nNumberReturned - 1;			
		   }
         else
            nSize = nNumberReturned;			
		  
		  CString temp;
		  //CString temp1;
		 
						
		  for (  i = 0; i < nSize; i++ )
		  {			  
		      temp = fclf01networkidlist.fclf01_list[i].network_id;
			  //temp1 = sNetworkList.ncf01_record[i].primary_key.network_type;			 
              
			  //int res = strcmp(temp1,"A");

			  //if( res == 0)
			  m_CntlList.AddString(temp);  			
          }
		  if (nNumberReturned > 0) 
		      m_CntlList.SetCurSel(0) ;
	   }

	break;

	case MODE_CAF01_KEYBLOCK:
		SetWindowText ( "CAF01 Keyblock Profile List");    
		
	  ZeroMemory ( &sCaf01Keyblock01, sizeof (CAF01_KEYBLOCK));

		if ( !strPrimaryKey1.IsEmpty())
			strcpy((char *)sCaf01Keyblock01.primary_key.caf_profile_id, strPrimaryKey1);
		else
			strcpy((char *)sCaf01Keyblock01.primary_key.caf_profile_id, " ");
		
		strcpy((char *)sCaf01Keyblock01.primary_key.type, strBinType);
		  
	  rcode = txdsapi_get_list((pBYTE)&sCaf01Keyblock01, sizeof (CAF01_KEYBLOCK), 
									(pBYTE)&sCaf01Keyblock01List, sizeof (CAF01_KEYBLOCK_GUI_LIST), 
									0, CAF01_KEYBLOCK_DATA, strErrorMessage) ;	
	  if (rcode == PTEMSG_OK) 
	  {	
	      nNumberReturned = atoi ((char *)&sCaf01Keyblock01List.num_returned);		
          if( nNumberReturned == GUI_MAX_LIST_SIZE)
          {				
		      CString strTemp = sCaf01Keyblock01List.caf01_keyblock_record[nNumberReturned-1].caf_profile_id ;
			  strTemp.TrimRight();
			  strTemp.TrimLeft() ;
			  memcpy( sCaf01Keyblock01.primary_key.caf_profile_id, (LPCTSTR) strTemp, sizeof (sCaf01Keyblock01.primary_key.caf_profile_id)); 
			  memcpy( sCaf01Keyblock01.primary_key.type, strBinType, sizeof (sCaf01Keyblock01.primary_key.type)); 
			  nSize = nNumberReturned - 1;			
		   }
         else
            nSize = nNumberReturned;			
		  
		  CString temp;
		  //CString temp1;
		 
						
		  for (  i = 0; i < nSize; i++ )
		  {			  
		      temp = sCaf01Keyblock01List.caf01_keyblock_record[i].caf_profile_id;
			  //temp1 = sNetworkList.ncf01_record[i].primary_key.network_type;			 
              
			  //int res = strcmp(temp1,"A");

			  //if( res == 0)
			  m_CntlList.AddString(temp);  			
          }
		  if (nNumberReturned > 0) 
		      m_CntlList.SetCurSel(0) ;
	   }

	break;
      	case MODE_FRULE01:
		SetWindowText ( "Fraud Rule List");    
		
	  ZeroMemory ( &FRULE01networkid, sizeof (FRULE01));

		if ( !strPrimaryKey1.IsEmpty())
			strcpy((char *)FRULE01networkid.card_brand, strPrimaryKey1);
		else
			strcpy((char *)FRULE01networkid.fraud_rule, " ");
		rcode = txdsapi_get_list((pBYTE)&FRULE01networkid, sizeof (FRULE01), 
									(pBYTE)&frulenetworkidlist, sizeof (FRULE01_GUI_LIST), 
									0, FRULE01_DATA, strErrorMessage) ;	
	  if (rcode == PTEMSG_OK) 
	  {	
	      nNumberReturned = atoi ((char *)&frulenetworkidlist.num_returned);		
          if( nNumberReturned == GUI_MAX_LIST_SIZE)
          {				
		      CString strTemp = frulenetworkidlist.frule01_list[nNumberReturned-1].card_brand ;
			  strTemp.TrimRight();
			  strTemp.TrimLeft() ;
			  memcpy( FRULE01networkid.fraud_rule, (LPCTSTR) strTemp, sizeof (FRULE01networkid.fraud_rule)); 
			 
				nSize = nNumberReturned - 1;			
		   }
         else
            nSize = nNumberReturned;			
		  
		  CString temp;
		  //CString temp1;
		 
						
		  for (  i = 0; i < nSize; i++ )
		  {			  
		      temp = frulenetworkidlist.frule01_list[i].fraud_rule;
			  //temp1 = sNetworkList.ncf01_record[i].primary_key.network_type;			 
              
			  //int res = strcmp(temp1,"A");

			  //if( res == 0)
			  m_CntlList.AddString(temp);  			
          }
		  if (nNumberReturned > 0) 
		      m_CntlList.SetCurSel(0) ;
	   }

		
   break;
		
}
			
	if ( nNumberReturned == 0 )
	{
		m_StrMessageLbl = "ERROR: No data found";
		UpdateData(FALSE);
		m_NextButton.EnableWindow (FALSE);
		m_OkButton.SetFocus();
	}

	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CNameList::OnNextButton()  
{
	char strErrorMessage[200];
	int rcode;
   int nSize;

    if( nNumberReturned == GUI_MAX_LIST_SIZE) 
    {
		switch ( m_nIndicator )
		{
			
		case MODE_DISPLAY_NEG_GROUP_BY_BIN_RANGE:
			rcode = txdsapi_get_list((pBYTE)&sNegGroupRecord,sizeof(NGF01), 
									 (pBYTE)&sNGFList,  sizeof (NGF01_LIST),
									 ST2_DB_GET_NGF01_LIST_BY_BINRANGE,
									 NGF01_DATA,	strErrorMessage) ;		
		    if (rcode == PTEMSG_OK) { // Dataserver returned DBERR_OK
			    nNumberReturned = atoi ((char *)&sNegGroupList.num_returned);
             if( nNumberReturned == GUI_MAX_LIST_SIZE)
             {				
			         CString strTemp = sNGFList.negative_group_id[nNumberReturned-1] ;
			         strTemp.TrimRight() ; strTemp.TrimLeft() ;
			         memcpy( sNegGroupRecord.primary_key.negative_group_id, (LPCTSTR) strTemp, sizeof (sNegGroupRecord.primary_key.negative_group_id)); 
                  nSize = nNumberReturned - 1;			
    		     }
              else
                nSize = nNumberReturned;			
            
						    
			    for ( int i = 0; i < nSize; i++ ) {
				     //m_CntlList.InsertString(0,(char *)sGroupList.group_id[i]);                 
				     m_CntlList.AddString( (const char *)sNGFList.negative_group_id[i] );                 
                }
		    }            

		break;
		case MODE_DISPLAY_NEG_GROUP:
			rcode = txdsapi_get_list((pBYTE)&sNegGroupRecord,sizeof(NGF01), 
									 (pBYTE)&sNegGroupList,  sizeof (NGF01_GUI_LIST),
									 ST2_DB_GET_NGF01_LIST_BY_BINRANGE,
									 NGF01_DATA,	strErrorMessage) ;		
		    if (rcode == PTEMSG_OK) { // Dataserver returned DBERR_OK
			    nNumberReturned = atoi ((char *)&sNegGroupList.num_returned);
                if( nNumberReturned == GUI_MAX_LIST_SIZE)
                {				
			        CString strTemp = sNegGroupList.ngf01_record[nNumberReturned-1].primary_key.negative_group_id ;
			        strTemp.TrimRight() ; strTemp.TrimLeft() ;
			        memcpy( sNegGroupRecord.primary_key.negative_group_id, (LPCTSTR) strTemp, sizeof (sNegGroupRecord.primary_key.negative_group_id)); 
                 nSize = nNumberReturned - 1;			
        		}
              else
                 nSize = nNumberReturned;			
            
						    
			    for ( int i = 0; i < nSize; i++ ) {
				     //m_CntlList.InsertString(0,(char *)sGroupList.group_id[i]);                 
				     m_CntlList.AddString( (const char *)sNegGroupList.ngf01_record[i].primary_key.negative_group_id );                 
                }
		    }            

		break;

		case MODE_DISPLAY_USER:
			rcode = txdsapi_get_list((pBYTE)&sUserRecord, sizeof (UCF01), 
								  (pBYTE)&sUserList,sizeof (UCF01_LIST), 
								  ST2_DB_GET_UCF01_LIST_BY_TERMINAL,
								  UCF01_DATA, strErrorMessage) ;	

		    if (rcode == PTEMSG_OK) { // Dataserver returned DBERR_OK
			    nNumberReturned = atoi ((char *)&sUserList.num_returned);
                if( nNumberReturned == GUI_MAX_LIST_SIZE)
                {				
			            CString strTemp = sUserList.name[nNumberReturned-1] ;
			            strTemp.TrimRight() ; strTemp.TrimLeft() ;
			            memcpy( &sUserRecord.primary_key.name, (LPCTSTR) strTemp, sizeof (sUserRecord.primary_key.name)); 
                     nSize = nNumberReturned - 1;			
        		    }
                  else
                     nSize = nNumberReturned;			
            
						    
			    for ( int i = 0; i < nSize; i++ ) {
				     m_CntlList.AddString((char *)sUserList.name[i]);                 
                }
			    if (m_CntlList.GetCount() > 0) 
				//	m_CntlList.SetTopIndex(m_CntlList.GetCount() - 20) ;
				m_CntlList.SetTopIndex(0) ;
				m_CntlList.SetCurSel(0) ;
		    }
		break;
		case MODE_DISPLAY_GROUP:
			rcode = txdsapi_get_list((pBYTE)&sGroupRecord, sizeof (UCF02), 
								  (pBYTE)&sGroupList,sizeof (UCF02_LIST), 
								  ST2_DB_GET_UCF02_LIST_BY_TERMINAL,
								  UCF02_DATA, strErrorMessage) ;	

		    if (rcode == PTEMSG_OK) { // Dataserver returned DBERR_OK
			    nNumberReturned = atoi ((char *)&sGroupList.num_returned);
                if( nNumberReturned == GUI_MAX_LIST_SIZE)
                {				
			            CString strTemp = sGroupList.group_id[nNumberReturned-1] ;
			            strTemp.TrimRight() ; strTemp.TrimLeft() ;
			            memcpy( &sGroupRecord.primary_key.group_id, (LPCTSTR) strTemp, sizeof (sGroupRecord.primary_key.group_id)); 
                     nSize = nNumberReturned - 1;			
        		     }
              else
                 nSize = nNumberReturned;			
            
						    
			    for ( int i = 0; i < nSize; i++ ) {
				     m_CntlList.AddString((char *)sGroupList.group_id[i]);                 
                }
		    }            
		break;


        case MODE_USER:
			rcode = txdsapi_get_list((pBYTE)&sUserRecord, sizeof (UCF01), 
								  (pBYTE)&sUserList,sizeof (UCF01_LIST), 0,
								  UCF01_DATA, strErrorMessage) ;	

		    if (rcode == PTEMSG_OK) { // Dataserver returned DBERR_OK
			    nNumberReturned = atoi ((char *)&sUserList.num_returned);
                if( nNumberReturned == GUI_MAX_LIST_SIZE)
                {				
			            CString strTemp = sUserList.name[nNumberReturned-1] ;
			            strTemp.TrimRight() ; strTemp.TrimLeft() ;
			            memcpy( &sUserRecord.primary_key.name, (LPCTSTR) strTemp, sizeof (sUserRecord.primary_key.name)); 
                     nSize = nNumberReturned - 1;			
        		    }
                  else
                     nSize = nNumberReturned;			
            
						    
			    for ( int i = 0; i < nSize; i++ ) {
				     m_CntlList.AddString((char *)sUserList.name[i]);                 
                }
		    }
		break;
       ///////////////////////////////////////////////////// 
		case MODE_NEGFILE:
			rcode = txdsapi_get_list((pBYTE)&sNegGroupRecord,sizeof(NGF01), 
									 (pBYTE)&sNegGroupList,  sizeof (NGF01_GUI_LIST),0,
									 NGF01_DATA,	strErrorMessage) ;		
		    if (rcode == PTEMSG_OK) { // Dataserver returned DBERR_OK
			    nNumberReturned = atoi ((char *)&sNegGroupList.num_returned);
                if( nNumberReturned == GUI_MAX_LIST_SIZE)
                {				
			            CString strTemp = sNegGroupList.ngf01_record[nNumberReturned-1].primary_key.negative_group_id ;
			            strTemp.TrimRight() ; strTemp.TrimLeft() ;
			            memcpy( sNegGroupRecord.primary_key.negative_group_id, (LPCTSTR) strTemp, sizeof (sNegGroupRecord.primary_key.negative_group_id)); 
                     nSize = nNumberReturned - 1;			
        		    }
                  else
                     nSize = nNumberReturned;			
            
						    
			    for ( int i = 0; i < nSize; i++ ) {
				     m_CntlList.AddString( (const char *)sNegGroupList.ngf01_record[i].primary_key.negative_group_id );                 
                }
		    }            
		break;
 ///////////////////////////////////////////////////////////////////       
        case MODE_WORKSTATION:
			rcode = txdsapi_get_list((pBYTE)&sWorkstationRecord,sizeof(ATP02), 
									 (pBYTE)&sWorkstationList,  sizeof (ATP02_GUI_LIST),0,
									 ATP02_DATA,	strErrorMessage) ;		
		    if (rcode == PTEMSG_OK) { // Dataserver returned DBERR_OK
			    nNumberReturned = atoi ((char *)&sWorkstationList.num_returned);
                if( nNumberReturned == GUI_MAX_LIST_SIZE)
                {				
			            CString strTemp = sWorkstationList.atp_list[nNumberReturned-1].primary_key.workstation_id ;
			            strTemp.TrimRight() ; strTemp.TrimLeft() ;
			            memcpy( sWorkstationRecord.primary_key.workstation_id, (LPCTSTR) strTemp, sizeof (sWorkstationRecord.primary_key.workstation_id)); 
                     nSize = nNumberReturned - 1;			
        		    }
                  else
                     nSize = nNumberReturned;			
            
						    
			    for ( int i = 0; i < nSize; i++ ) {
				     //m_CntlList.InsertString(0,(char *)sGroupList.group_id[i]);                 
				     m_CntlList.AddString( (const char *)sWorkstationList.atp_list[i].primary_key.workstation_id );                 
                }
		    }            
        break;
//////////////////////////////////////////////////////////////////////
	  case MODE_NETWORK:
	  rcode = txdsapi_get_list((pBYTE)&sNetworkId, sizeof (NCF01), 
									(pBYTE)&sNetworkList, sizeof (NCF01_GUI_LIST), 
									ST2_DB_GET_NCF01_LIST_BY_TYPE, NCF01_DATA, strErrorMessage) ;	
			if (rcode == PTEMSG_OK) { 	 	 
			    nNumberReturned = atoi ((char *)&sNetworkList.num_returned);
                if( nNumberReturned == GUI_MAX_LIST_SIZE)
                {				
			            CString strTemp = sNetworkList.ncf01_record[nNumberReturned-1].primary_key.network_id ;
			            strTemp.TrimRight() ; strTemp.TrimLeft() ;
			            memcpy( &sNetworkId.primary_key.network_id, (LPCTSTR) strTemp, sizeof (sNetworkId.primary_key.network_id)); 
                     nSize = nNumberReturned - 1;			
        		    }
                  else
                     nSize = nNumberReturned;			
            
						    
			    for ( int i = 0; i < nSize; i++ ) {
				     m_CntlList.AddString((char *)sNetworkList.ncf01_record[i].primary_key.network_id);                 
                }
		    }            

		break;

	  case MODE_NCF01_KEYBLOCK:
	  rcode = txdsapi_get_list((pBYTE)&sNCF01keyblock01, sizeof (NCF01_KEYBLOCK), 
									(pBYTE)&sNCF01keyblockList, sizeof (NCF01_KEYBLOCK_GUI_LIST), 
									ST2_DB_GET_NCF01_LIST_BY_TYPE, NCF01_DATA, strErrorMessage) ;	
			if (rcode == PTEMSG_OK) { 	 	 
			    nNumberReturned = atoi ((char *)&sNCF01keyblockList.num_returned);
                if( nNumberReturned == GUI_MAX_LIST_SIZE)
                {				
			            CString strTemp = sNCF01keyblockList.ncf01_keyblock_record[nNumberReturned-1].network_id ;
			            strTemp.TrimRight() ; strTemp.TrimLeft() ;
			            memcpy( &sNCF01keyblock01.network_id, (LPCTSTR) strTemp, sizeof (sNCF01keyblock01.network_id)); 
                     nSize = nNumberReturned - 1;			
        		    }
                  else
                     nSize = nNumberReturned;			
            
						    
			    for ( int i = 0; i < nSize; i++ ) {
				     m_CntlList.AddString((char *)sNCF01keyblockList.ncf01_keyblock_record[i].network_id);                 
                }
		    }            

		break;

//////////////////////////////////////////////////////////////////////
        case MODE_GROUP:
			rcode = txdsapi_get_list((pBYTE)&sGroupRecord, sizeof (UCF02), 
								  (pBYTE)&sGroupList,sizeof (UCF02_LIST), 0,
								  UCF02_DATA, strErrorMessage) ;	

		    if (rcode == PTEMSG_OK) { // Dataserver returned DBERR_OK
			    nNumberReturned = atoi ((char *)&sGroupList.num_returned);
                if( nNumberReturned == GUI_MAX_LIST_SIZE)
                {				
			            CString strTemp = sGroupList.group_id[nNumberReturned-1] ;
			            strTemp.TrimRight() ; strTemp.TrimLeft() ;
			            memcpy( &sGroupRecord.primary_key.group_id, (LPCTSTR) strTemp, sizeof (sGroupRecord.primary_key.group_id)); 
                     nSize = nNumberReturned - 1;			
        		    }
                  else
                     nSize = nNumberReturned;			
            
						    
			    for ( int i = 0; i < nSize; i++ ) {
				     m_CntlList.AddString((char *)sGroupList.group_id[i]);                 
                }
		    }            
        break;
//////////////////////////////////////////////////////////////////
		case MODE_AUTH:
			rcode = txdsapi_get_list((pBYTE)&sAuthRecord, sizeof (CAF01), 
								  (pBYTE)&sAuthList,sizeof (CAF01_GUI_LIST), ST2_DB_GET_CAF01_LIST_BY_TYPE,
								  CAF01_DATA, strErrorMessage) ;	
			if (rcode == PTEMSG_OK) { 	 	 
			    nNumberReturned = atoi ((char *)&sAuthList.num_returned);
                if( nNumberReturned == GUI_MAX_LIST_SIZE)
                {				
			            CString strTemp = sAuthList.caf_info[nNumberReturned-1].caf_profile_id ;
			            strTemp.TrimRight() ; strTemp.TrimLeft() ;
			            memcpy( &sAuthRecord.primary_key.caf_profile_id, (LPCTSTR) strTemp, sizeof (sAuthRecord.primary_key.caf_profile_id)); 
                     nSize = nNumberReturned - 1;			
        		    }
                  else
                     nSize = nNumberReturned;			
            
						    
			    for ( int i = 0; i < nSize; i++ ) {
				     m_CntlList.AddString((char *)sAuthList.caf_info[i].caf_profile_id);                 
                }
		    } 
		break;
/////////////////////////////////////////////////////////////////////////
		case MODE_CAF01_KEYBLOCK:
			rcode = txdsapi_get_list((pBYTE)&sCaf01Keyblock01, sizeof (CAF01_KEYBLOCK), 
								  (pBYTE)&sCaf01Keyblock01List,sizeof (CAF01_KEYBLOCK_GUI_LIST), ST2_DB_GET_CAF01_LIST_BY_TYPE,
								  CAF01_KEYBLOCK_DATA, strErrorMessage) ;	
			if (rcode == PTEMSG_OK) { 	 	 
			    nNumberReturned = atoi ((char *)&sCaf01Keyblock01List.num_returned);
                if( nNumberReturned == GUI_MAX_LIST_SIZE)
                {				
			            CString strTemp = sCaf01Keyblock01List.caf01_keyblock_record[nNumberReturned-1].caf_profile_id ;
			            strTemp.TrimRight() ; 
				    strTemp.TrimLeft() ;
			            memcpy( &sCaf01Keyblock01.primary_key.caf_profile_id, (LPCTSTR) strTemp, sizeof (sCaf01Keyblock01.primary_key.caf_profile_id)); 
                     nSize = nNumberReturned - 1;			
        		    }
                  else
                     nSize = nNumberReturned;			
            
						    
			    for ( int i = 0; i < nSize; i++ ) {
				     m_CntlList.AddString((char *)sCaf01Keyblock01List.caf01_keyblock_record[i].caf_profile_id);                 
                }
		    } 
		break;
/////////////////////////////////////////////////////////////////////////
		case MODE_CARD:
			rcode = txdsapi_get_list((pBYTE)&sCardRecord, sizeof (CTF01), 
								  (pBYTE)&sCardList,sizeof (CTF01_GUI_LIST), 0,
								  CTF01_DATA, strErrorMessage) ;	
			if (rcode == PTEMSG_OK) { 	    
			    nNumberReturned = atoi ((char *)&sCardList.num_returned);
                if( nNumberReturned == GUI_MAX_LIST_SIZE)
                {				
			            CString strTemp = sCardList.card_type[nNumberReturned-1] ;
			            strTemp.TrimRight() ; strTemp.TrimLeft() ;
			            memcpy( &sCardRecord.primary_key.card_type, (LPCTSTR) strTemp, sizeof (sCardRecord.primary_key.card_type)); 
                     nSize = nNumberReturned - 1;			
        		    }
                  else
                     nSize = nNumberReturned;			
						    
			    for ( int i = 0; i < nSize; i++ ) {
				     m_CntlList.AddString((char *)sCardList.card_type[i]);                 
                }
		    }            
        break;
////////////////////////////////////////////////////////////////////////////
		case MODE_CONVERSION:
			SetWindowText ( "Conversion Rate List");    


			rcode = txdsapi_get_list((pBYTE)&sCurrencyCode, sizeof (CRF01), 
									  (pBYTE)&sConversionList,sizeof (CRF01_GUI_LIST), 0,
									  CRF01_DATA, strErrorMessage) ;	
			if (rcode== PTEMSG_OK) {// Dataserver returned DBERR_OK
				nNumberReturned = atoi ((char *)&sConversionList.num_returned);		
				if( nNumberReturned == GUI_MAX_LIST_SIZE)
				{				
				    	CString strTemp = sConversionList.currency_code[nNumberReturned-1] ;
				    	strTemp.TrimRight() ; strTemp.TrimLeft() ;
				    	memcpy( sCurrencyCode.primary_key.currency_code, (LPCTSTR) strTemp, sizeof (sCurrencyCode.primary_key.currency_code)); 
                 nSize = nNumberReturned - 1;			
        		}
              else
                 nSize = nNumberReturned;			
							
				for ( int i = 0; i < nSize; i++ ) {
					 m_CntlList.AddString((char *)sConversionList.currency_code[i]);  			
				}
			}
		
		break;	
		}
		if (m_CntlList.GetCount() > 0) 
		m_CntlList.SetTopIndex(m_CntlList.GetCount() - 20) ;
		m_CntlList.SetCurSel(m_CntlList.GetCount() - 20) ;

	}
    else
	{
        m_StrMessageLbl = "End of List";
		UpdateData(FALSE);
		m_NextButton.EnableWindow (FALSE);
		m_OkButton.SetFocus();

	}
}

void CNameList::OnOK() 
{
	int nListIndex = m_CntlList.GetCurSel() ;    

	if (nListIndex == LB_ERR) {
		m_StrMessageLbl = "Please select an item";
		UpdateData(FALSE);	
		return;
	}
    m_CntlList.GetText (nListIndex, name);
    m_CntlList.GetText (nListIndex, m_strName);

	
	CDialog::OnOK();
}

void CNameList::OnDblclkList1() 
{
	OnOK();
	
}

BOOL CNameList::OnHelpInfo(HELPINFO* pHelpInfo) 
{
//		HtmlHelp (AfxGetMainWnd()->GetSafeHwnd(),
//		"security.chm::/SELECT.HTM",HH_DISPLAY_TOPIC,0);
	
	
	return TRUE;
}



BOOL CNameList::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	// TODO: Add your specialized code here and/or call the base class
	
	c_time = CTime::GetCurrentTime();	
	shrmem.SetLastUpdateTime(c_time);
	return CDialog::OnCommand(wParam, lParam);
}
