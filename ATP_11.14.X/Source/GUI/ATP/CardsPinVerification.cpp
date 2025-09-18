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
* $Log:   N:\PVCS\PTE\Equitable\ATP_gui\CardsPinVerification.cpp  $
   
      Rev 1.8   Dec 01 2003 11:28:52   lmitchel
   Added AMEX Online Updates - scr1200
   
      Rev 1.7   Jan 05 2000 14:53:10   gbarnes
   Major update. Not updated to PVCS recently.
   PVCS was locked.
   
      Rev 1.1   Sep 22 1999 15:45:50   iarustam
   Added manual batch entry GUI
   
      Rev 1.0   Jul 01 1999 14:17:06   iarustam
   Initial Release
   
      Rev 1.0   Jun 28 1999 13:58:14   iarustam
   Initial Release
   
      Rev 1.4   Jun 04 1999 17:40:04   iarustam
   Bug fixes
   
      Rev 1.1   Apr 29 1999 14:56:04   iarustam
   Bug fixes
   
      Rev 1.0   Feb 23 1999 11:40:12   iarustam
   Initial Release
   
      Rev 1.0   Feb 17 1999 10:09:14   iarustam
   Initial Release
   
      Rev 1.1   Jan 22 1999 10:58:10   iarustam
   Bug fixes and core 1_3_0 changes
   
      Rev 1.1   Jan 21 1999 13:07:16   iarustam
   Bug fixes
   
   
      Rev 1.1   Dec 18 1998 13:42:48   iarustam
    
*
************************************************************************************/

// CardsPinVerification.cpp : implementation file
//

#include "stdafx.h"
#include "atp.h"
#include "CardsPinVerification.h"
#include "SharedMem.h"

extern CSharedMem shrmem;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCardsPinVerification dialog


CCardsPinVerification::CCardsPinVerification(CWnd* pParent /*=NULL*/)
	: CDialog(CCardsPinVerification::IDD, pParent)
{
	//{{AFX_DATA_INIT(CCardsPinVerification)
	m_StrDesimTable_Fld = _T("");
	m_StrOffsetLength_Fld = _T("");
	m_StrPinKey_Fld = _T("");
	m_StrPinOffsetLocation_Fld = _T("");
	m_StrTrackPinOffsetLoc_Fld = _T("");
	m_StrMessageLbl = _T("");
	//}}AFX_DATA_INIT
}


void CCardsPinVerification::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCardsPinVerification)
	DDX_Control(pDX, IDC_TRACK_PIN_OFFSET_LOC_FLD, m_CntlTrackPinOffsetLoc_Fld);
	DDX_Control(pDX, IDC_PIN_OFFSET_LOCATION_FLD, m_CntlPinOffsetLocation_Fld);
	DDX_Control(pDX, IDC_PIN_KEY_FLD, m_CntlPinKey_Fld);
	DDX_Control(pDX, IDC_OFFSET_LENGTH_FLD, m_CntlOffsetLength_Fld);
	DDX_Control(pDX, IDC_DECIM_TABLE_FLD, m_CntlDesimTable_Fld);
	DDX_Text(pDX, IDC_DECIM_TABLE_FLD, m_StrDesimTable_Fld);
	DDV_MaxChars(pDX, m_StrDesimTable_Fld, 16);
	DDX_Text(pDX, IDC_OFFSET_LENGTH_FLD, m_StrOffsetLength_Fld);
	DDV_MaxChars(pDX, m_StrOffsetLength_Fld, 1);
	DDX_Text(pDX, IDC_PIN_KEY_FLD, m_StrPinKey_Fld);
	DDV_MaxChars(pDX, m_StrPinKey_Fld, 16);
	DDX_Text(pDX, IDC_PIN_OFFSET_LOCATION_FLD, m_StrPinOffsetLocation_Fld);
	DDV_MaxChars(pDX, m_StrPinOffsetLocation_Fld, 2);
	DDX_Text(pDX, IDC_TRACK_PIN_OFFSET_LOC_FLD, m_StrTrackPinOffsetLoc_Fld);
	DDV_MaxChars(pDX, m_StrTrackPinOffsetLoc_Fld, 2);
	DDX_Text(pDX, IDC_MESSAGE_LBL, m_StrMessageLbl);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CCardsPinVerification, CDialog)
	//{{AFX_MSG_MAP(CCardsPinVerification)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCardsPinVerification message handlers

BOOL CCardsPinVerification::OnInitDialog() 
{
	CDialog::OnInitDialog();    	

	CFont m_font;
	LOGFONT lf;

	memset ( &lf, 0, sizeof (lf));
	lf.lfHeight = 12;
	lf.lfWeight = 700;
	strcpy(lf.lfFaceName, "Arial");
	m_font.CreateFontIndirect (&lf);
	GetDlgItem(IDC_MESSAGE_LBL)->SetFont (&m_font);
    
    m_StrMessageLbl = "";    
    
	if (m_nDialogMode != MODE_DONE) 
    {  		
         EnablePinVerificationDialog(TRUE);
        
        if (m_nDialogMode != MODE_ADD )
        PopulatePinVerificationDialogFromDatabase (sProfileIdOld);		
    }    	
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CCardsPinVerification::OnCancel() 
{

	CDialog::OnCancel();
}



/************************************************************/
 ////////////////////////////////////////////////////////////
/************************************************************/

BOOL CCardsPinVerification::PopulatePinVerificationDialogFromDatabase( CAF01 sProfileId )
{
     if ( m_nDialogMode != MODE_VIEW )
     EnablePinVerificationDialog(TRUE);

     ZeroMemory ( &sProfileIdOld, sizeof (CAF01));
 
    m_StrPinKey_Fld                = sProfileId.pin.IbmOffset.pin_key;    
    m_StrDesimTable_Fld            = sProfileId.pin.IbmOffset.decimalization_tbl;
    m_StrOffsetLength_Fld          = sProfileId.pin.IbmOffset.offset_length;
    m_StrPinOffsetLocation_Fld     = sProfileId.pin.IbmOffset.pin_offset_location;
    m_StrTrackPinOffsetLoc_Fld     = sProfileId.pin.IbmOffset.track_offset;    
    

    UpdateData(FALSE);

    memcpy ( &sProfileIdOld, &sProfileId, sizeof ( CAF01));

    return TRUE;

}

/************************************************************/
 ////////////////////////////////////////////////////////////
/************************************************************/

BOOL CCardsPinVerification::PopulateDatabaseFromPinVerificationDialog( pCAF01 psProfileId )
{   
	int res;
 
  /*  memcpy ( psProfileId->pin.IbmOffset.decimalization_tbl,  m_StrDesimTable_Fld,        sizeof ( psProfileId->pin.IbmOffset.decimalization_tbl));    
    memcpy ( psProfileId->pin.IbmOffset.offset_length,       m_StrOffsetLength_Fld,      sizeof ( psProfileId->pin.IbmOffset.offset_length));   
    memcpy ( psProfileId->pin.IbmOffset.pin_offset_location, m_StrPinOffsetLocation_Fld, sizeof ( psProfileId->pin.IbmOffset.pin_offset_location)); 
    memcpy ( psProfileId->pin.IbmOffset.track_offset,        m_StrTrackPinOffsetLoc_Fld, sizeof ( psProfileId->pin.IbmOffset.track_offset));
   */

	memcpy ( psProfileId->pin.IbmOffset.decimalization_tbl,  m_StrDesimTable_Fld,        sizeof ( psProfileId->pin.IbmOffset.decimalization_tbl));    
    memcpy ( psProfileId->pin.IbmOffset.offset_length,       m_StrOffsetLength_Fld,      sizeof ( psProfileId->pin.IbmOffset.offset_length));   
    memcpy ( psProfileId->pin.IbmOffset.pin_offset_location, m_StrPinOffsetLocation_Fld, sizeof ( psProfileId->pin.IbmOffset.pin_offset_location)); 
    memcpy ( psProfileId->pin.IbmOffset.track_offset,        m_StrTrackPinOffsetLoc_Fld, sizeof ( psProfileId->pin.IbmOffset.track_offset));
	 //if (!CheckPinKeyData(m_StrPinKey_Fld))
	
	res = strcspn(m_StrPinKey_Fld, "0123456789ABCDEF");
	if (res == 0)
	{	
	memcpy ( psProfileId->pin.IbmOffset.pin_key,             m_StrPinKey_Fld,            sizeof ( psProfileId->pin.IbmOffset.pin_key));
    }
	else
	{
        m_StrMessageLbl = "Invalid PIN Key!";
        UpdateData(FALSE);
        return FALSE;
    }
    
    
    return TRUE;

}

/************************************************************/
 ////////////////////////////////////////////////////////////
/************************************************************/

void CCardsPinVerification::EnablePinVerificationDialog(int nStatus)
{   
    	if ( m_nDialogMode != MODE_VIEW )
{
    m_CntlPinKey_Fld.EnableWindow(nStatus);    
    m_CntlDesimTable_Fld.EnableWindow(nStatus);
    m_CntlOffsetLength_Fld.EnableWindow(nStatus);
    m_CntlPinOffsetLocation_Fld.EnableWindow(nStatus);
    m_CntlTrackPinOffsetLoc_Fld.EnableWindow(nStatus);

}
}

/************************************************************/
 ////////////////////////////////////////////////////////////
/************************************************************/

void CCardsPinVerification::CleanPinVerificationDialog()
{
    m_StrPinKey_Fld = "";    
    m_StrDesimTable_Fld = "";    
    m_StrOffsetLength_Fld = "";
    m_StrPinOffsetLocation_Fld = "";
    m_StrTrackPinOffsetLoc_Fld = ""; 
    m_StrMessageLbl = "";

	UpdateData(FALSE);

}

/************************************************************/
 ////////////////////////////////////////////////////////////
/************************************************************/

BOOL CCardsPinVerification::CheckPinKeyData(CString strData)
{
    int len;
    int input;

    //UpdateData(TRUE);
	
	UpdateData(FALSE);

    len = strData.GetLength();     
   
     
    input = strspn(strData, "0123456789ABCDEF");

    if (input < len)
    {
        m_StrMessageLbl = "Invalid character!";
        UpdateData(FALSE);
        return FALSE;
    }
    return TRUE;
}


void CCardsPinVerification::OnOK() 
{
	UpdateData(TRUE);
	if (!CheckPinKeyData(m_StrPinKey_Fld))
	{
		m_StrMessageLbl = "ERROR: Invalid character in Pin Key!";
		m_CntlPinKey_Fld.SetFocus();
		UpdateData(FALSE);
		return;
	}
	
	CDialog::OnOK();
}

BOOL CCardsPinVerification::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	// TODO: Add your specialized code here and/or call the base class
	
	c_time = CTime::GetCurrentTime();	
	shrmem.SetLastUpdateTime(c_time);

	return CDialog::OnCommand(wParam, lParam);
}
