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
* $Log:   N:\PVCS\PTE\Equitable\ATP_gui\DualEncrKeyControl.cpp  $
   
      Rev 1.0   Feb 02 2011 11:30:22   TF_Srikanth
   Added DualEncrKeyControl Class for Changing Encryption Key in Database from GUI  
    
*
************************************************************************************/
// DualEncrKeyControl.cpp : implementation file
//

#include "stdafx.h"
#include "atp.h"
#include "DualEncrKeyControl.h"
#include "SharedMem.h"
extern "C"
{
   #include "myutils.h"
}
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
extern CSharedMem shrmem;

//UCF03 firstUserDatabaseStructure,scndUserDatabaseStructure;
UCF01 firstUserDatabaseStructure,scndUserDatabaseStructure;
char strErrorMessage[200];
/////////////////////////////////////////////////////////////////////////////
// CDualEncrKeyControl property page

IMPLEMENT_DYNCREATE(CDualEncrKeyControl, CPropertyPage)

CDualEncrKeyControl::CDualEncrKeyControl() : CPropertyPage(CDualEncrKeyControl::IDD)
{
	//{{AFX_DATA_INIT(CDualEncrKeyControl)
	m_StrUserNameFld = _T("");
	m_StrDualCoreEkey = _T("");
	m_StrCDualCoreEkey = _T("");
	m_StrMessageLbl = _T("");
	//}}AFX_DATA_INIT
}

CDualEncrKeyControl::~CDualEncrKeyControl()
{
}
void CDualEncrKeyControl::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDualEncrKeyControl)
	DDX_Control(pDX, IDC_DUAL_CORE_EKEY, m_CntlDualCoreEkey);
	DDX_Control(pDX, IDC_CDUAL_CORE_EKEY, m_CntlCDualCoreEkey);
	DDX_Text(pDX, IDC_USER_NAME_FLD, m_StrUserNameFld);
	DDX_Text(pDX, IDC_DUAL_CORE_EKEY, m_StrDualCoreEkey);
	DDV_MaxChars(pDX, m_StrDualCoreEkey, 12);
	DDX_Text(pDX, IDC_CDUAL_CORE_EKEY, m_StrCDualCoreEkey);
	DDV_MaxChars(pDX, m_StrCDualCoreEkey, 12);
	DDX_Text(pDX, IDC_MESSAGE_LBL, m_StrMessageLbl);
	//}}AFX_DATA_MAP
}
BEGIN_MESSAGE_MAP(CDualEncrKeyControl, CPropertyPage)
	//{{AFX_MSG_MAP(CDualEncrKeyControl)
	ON_EN_CHANGE(IDC_CDUAL_CORE_EKEY, OnChangeCdualCoreEkey)
	ON_EN_CHANGE(IDC_DUAL_CORE_EKEY, OnChangeDualCoreEkey)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDualEncrKeyControl message handlers
BOOL CDualEncrKeyControl::OnInitDialog() 
{	
	m_StrMessageLbl="";
	m_StrDualCoreEkey="";
	m_StrCDualCoreEkey="";
	//To Set the text Font  
	CFont m_font;
	LOGFONT lf;
	memset ( &lf, 0, sizeof (lf));
	lf.lfHeight = 12;
	lf.lfWeight = 700;
	strcpy(lf.lfFaceName, "Arial");
	m_font.CreateFontIndirect (&lf);
	GetDlgItem(IDC_MESSAGE_LBL)->SetFont (&m_font);

	//To Get the UserRecord .
	memset(&firstUserDatabaseStructure, 0, sizeof(firstUserDatabaseStructure)) ;
	//memcpy ((LPSTR)(firstUserDatabaseStructure.foreign_key.name), m_StrUserNameFld , sizeof (firstUserDatabaseStructure.foreign_key.name));
	memcpy ((LPSTR)(firstUserDatabaseStructure.primary_key.name), m_StrUserNameFld , sizeof (firstUserDatabaseStructure.primary_key.name));
	int rcode = txdsapi_get_record  ((pBYTE)&firstUserDatabaseStructure, sizeof (UCF01), UCF01_DATA, strErrorMessage);
	if ( rcode != PTEMSG_OK )
	{	    
        switch( rcode )
		{
		 case PTEMSG_NOT_FOUND:
				m_StrMessageLbl = strErrorMessage;
				sprintf( strErrorMessage, "ERROR: User record for %s does not exist", m_StrUserNameFld );
				break;
		}		
		if (!UpdateData(FALSE))
		{
			AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
		}
	}
	//Checking whether User already added Encryption key
	CString addedEkey=(char *)firstUserDatabaseStructure.user_info_4;
	if(addedEkey!="")
	{
		AfxMessageBox ("You already added Encryption Key...", MB_ICONINFORMATION | MB_OK );
		EndDialog(IDD_DENCKEY);
	}		
	CPropertyPage::OnInitDialog();
			
					
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDualEncrKeyControl::OnCancel() 
{
	// TODO: Add your specialized code here and/or call the base class
	
	CPropertyPage::OnCancel();
}
BOOL CDualEncrKeyControl::CheckDialogForCompleteness()
{
    if (!UpdateData(TRUE)){
		AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
	}

    return TRUE;
}
BOOL CDualEncrKeyControl::OnApply() 
{
	int rcode;
	char dual_key[ENC_DEC_KEY_SIZE];

	if(m_StrDualCoreEkey != "")
	{
		while(1)
		{
			//Check for Dialog Completeness
			if(m_StrMessageLbl !="")
			{
				SetModified ( TRUE );
			}			
			if( m_StrDualCoreEkey.IsEmpty() )
			{
					m_StrMessageLbl="Please enter Encyption Key";
					m_CntlDualCoreEkey.SetFocus();
					if (!UpdateData(FALSE)){
						AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);
					}
						return FALSE;	
		
			}
			if( m_StrCDualCoreEkey.IsEmpty() )
			{
					m_StrMessageLbl="Please Confirm Encyption Key";
					m_CntlCDualCoreEkey.SetFocus();
					if (!UpdateData(FALSE)){
						AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
					}
						return FALSE;
			}	
			if(m_StrDualCoreEkey.GetLength()<12)
			{
					m_StrMessageLbl="Please enter Encyption Key more than 11 characters";
					m_CntlDualCoreEkey.SetFocus();
					if (!UpdateData(FALSE)){
						AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);		
					}
						return FALSE;			
			}	
			if(m_StrDualCoreEkey != m_StrCDualCoreEkey)
			{
					m_StrMessageLbl="Encrption Key Confirmation Does not Match.Please re-enter!";
					m_CntlCDualCoreEkey.SetFocus();
					if (!UpdateData(FALSE)){
						AfxMessageBox (IDS_UPDATEDATAFAILED,MB_ICONSTOP);
					}
						return FALSE;
			}

			//Krishna@TF, 2.6.11, Validate the Dual Encryption Key for nly Hex Numbers being jeyed in
			if(!CheckforInvalidChars(m_StrDualCoreEkey))
			{
				AfxMessageBox("Key should contain only numbers & alphabets from A to F");
				return FALSE;
			}
			if(!CheckforInvalidChars(m_StrCDualCoreEkey))
			{
				AfxMessageBox("Key should contain only numbers & alphabets from A to F");
				return FALSE;
			}
			
			
			//Krishna@TF, 2.6.11, Retreive the Other User Dual Encryption Param from DB	
			
			memcpy ((LPSTR)(scndUserDatabaseStructure.primary_key.name), m_StrUserNameFld, sizeof (scndUserDatabaseStructure.primary_key.name));
			
			rcode = txdsapi_Select_Ucf01_DualEncryptionKey_Other_User(&scndUserDatabaseStructure, strErrorMessage);
			
			if(rcode != PTEMSG_OK)
			{
				AfxMessageBox(strErrorMessage);
				return FALSE;
			}

			//If key len is 0, meaning, no user has entered the Key. So update the First User Key.
			if(strlen((char *)scndUserDatabaseStructure.ekey) == 0)
			{	
				memcpy ((LPSTR)(firstUserDatabaseStructure.primary_key.name), m_StrUserNameFld, sizeof (firstUserDatabaseStructure.primary_key.name));
				rcode = txdsapi_get_record( (pBYTE)&firstUserDatabaseStructure, sizeof (UCF01), UCF01_DATA,strErrorMessage );				
				if(rcode != PTEMSG_OK)
				{
					AfxMessageBox(strErrorMessage);
					return FALSE;
				}

				memcpy((LPSTR)firstUserDatabaseStructure.ekey, m_StrDualCoreEkey, 12);

				rcode = txdsapi_update_record  ((pBYTE)&firstUserDatabaseStructure, sizeof (UCF01), UCF01_DATA,strErrorMessage);
				if(rcode != PTEMSG_OK)
				{
					AfxMessageBox(strErrorMessage);
					return FALSE;
				}

				m_StrMessageLbl = "Encryption Key Updated.";

				m_StrDualCoreEkey = "";
				m_StrCDualCoreEkey = "";

				return TRUE;
			}
			//Second User key updation
			else if(strlen((char *)scndUserDatabaseStructure.ekey)> 0)
			{	
				memset(dual_key, 0, sizeof(dual_key));

				strncpy(dual_key, (char *)scndUserDatabaseStructure.ekey, strlen((char *)scndUserDatabaseStructure.ekey));

				strcat(dual_key, m_StrDualCoreEkey);

				//Imp!!.. To be Called Update DB function
				rcode = txdsapi_Update_Ucf01_DualEncryptionKey(dual_key, strErrorMessage);
				if(rcode != PTEMSG_OK)
				{
					AfxMessageBox(strErrorMessage);
					return FALSE;
				}

				//Update the dkey field as Updated, meaning both users have updated their keys
				memset(scndUserDatabaseStructure.dekey, 0, sizeof(scndUserDatabaseStructure.dekey));
				memcpy(scndUserDatabaseStructure.dekey, UPDATED, strlen(UPDATED));
				memset(scndUserDatabaseStructure.ekey, 0, sizeof(scndUserDatabaseStructure.ekey));
				rcode = txdsapi_update_record  ((pBYTE)&scndUserDatabaseStructure, sizeof (UCF01), UCF01_DATA,strErrorMessage);
				
				if(rcode != PTEMSG_OK)
				{
					AfxMessageBox(strErrorMessage);
					return FALSE;
				}

				memcpy ((LPSTR)(firstUserDatabaseStructure.primary_key.name), m_StrUserNameFld, sizeof (firstUserDatabaseStructure.primary_key.name));
				rcode = txdsapi_get_record( (pBYTE)&firstUserDatabaseStructure, sizeof (UCF01), UCF01_DATA,strErrorMessage );				
				if(rcode != PTEMSG_OK)
				{
					AfxMessageBox(strErrorMessage);
					return FALSE;
				}
				memset(firstUserDatabaseStructure.dekey, 0, sizeof(firstUserDatabaseStructure.dekey));
				memcpy(firstUserDatabaseStructure.dekey, UPDATED, strlen(UPDATED));
				memset(firstUserDatabaseStructure.ekey, 0, sizeof(firstUserDatabaseStructure.ekey));

				rcode = txdsapi_update_record  ((pBYTE)&firstUserDatabaseStructure, sizeof (UCF01), UCF01_DATA,strErrorMessage);
				if(rcode != PTEMSG_OK)
				{
					AfxMessageBox(strErrorMessage);
					return FALSE;
				}

				m_StrMessageLbl = "Encryption Key Updated.";

				m_StrDualCoreEkey = "";
				m_StrCDualCoreEkey = "";

				return true;
			}		
		}
	}
	else
		return TRUE;

	
	return CPropertyPage::OnApply();
}

BOOL CDualEncrKeyControl::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	c_time = CTime::GetCurrentTime();	
	shrmem.SetLastUpdateTime(c_time);
	return CPropertyPage::OnCommand(wParam, lParam);
}

void CDualEncrKeyControl::OnOK() 
{
	// TODO: Add your specialized code here and/or call the base class
	
	CPropertyPage::OnOK();
}

BOOL CDualEncrKeyControl::OnSetActive() 
{			
	return CPropertyPage::OnSetActive();
}

void CDualEncrKeyControl::OnChangeCdualCoreEkey() 
{
	SetModified ( TRUE );	
}

void CDualEncrKeyControl::OnChangeDualCoreEkey() 
{
	SetModified ( TRUE );	
}

//Krishna@TF, 2.6.11: Check whether any chars apart from Hexa Decimal No's are entered
BOOL CDualEncrKeyControl::CheckforInvalidChars(CString str)
{
	int i=0;	
	char text[12];
	strcpy(text, str);
	int len = strlen(text);
	if(len > 0)
	{
		for(i = 0;i<len;i++)
		{
			//			  '0'			   '9'				  'A'			   'F'
			if((text[i] >= 48 && text[i]<= 57) || (text[i] >= 65  && text[i] <= 70))
			{
				continue;
			}
			else
			{				
				return FALSE;
			}
		}	
	}
	return TRUE;
}