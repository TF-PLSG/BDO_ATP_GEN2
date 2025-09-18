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
* $Log:   N:\PVCS\PTE\Equitable\ATP_gui\CardsPinVerifVisa.cpp  $
   
      Rev 1.8   Dec 01 2003 11:28:54   lmitchel
   Added AMEX Online Updates - scr1200
   
      Rev 1.7   Jan 05 2000 14:53:10   gbarnes
   Major update. Not updated to PVCS recently.
   PVCS was locked.
   
      Rev 1.1   Sep 22 1999 15:45:40   iarustam
   Added manual batch entry GUI
   
      Rev 1.0   Jul 01 1999 14:17:06   iarustam
   Initial Release
   
      Rev 1.0   Jun 28 1999 13:58:12   iarustam
   Initial Release
   
      Rev 1.4   May 10 1999 11:02:24   dperson
   Added PVCS header and
*   
************************************************************************************/
// CardsPinVerifVisa.cpp : implementation file
//

#include "stdafx.h"
#include "atp.h"
#include "CardsPinVerifVisa.h"
#include "MyUtils.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#include "SharedMem.h"

extern CSharedMem shrmem;
/////////////////////////////////////////////////////////////////////////////
// CCardsPinVerifVisa dialog


CCardsPinVerifVisa::CCardsPinVerifVisa(CWnd* pParent /*=NULL*/)
	: CDialog(CCardsPinVerifVisa::IDD, pParent)
{
	//{{AFX_DATA_INIT(CCardsPinVerifVisa)
	m_StrKeyIndFld = _T("");
	m_StrKeyLeftFld = _T("");
	m_StrKeyRightFld = _T("");
	m_StrLocation1Fld = _T("");
	m_StrLocation2Fld = _T("");
	m_StrMessageLbl = _T("");
	m_StrOffset1Fld = _T("");
	m_StrOffset2Fld = _T("");
	m_StrPVKLenTr1Fld = _T("");
	m_StrPVKLenTr2Fld = _T("");
	m_StrPVKLocTr1Fld = _T("");
	m_StrPVKLocTr2Fld = _T("");
	//}}AFX_DATA_INIT
}


void CCardsPinVerifVisa::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCardsPinVerifVisa)
	DDX_Control(pDX, IDC_PVKI_LOC_TR2_FLD, m_CntlPVKLocTr2Fld);
	DDX_Control(pDX, IDC_PVKI_LOC_TR1_FLD, m_CntlPVKLocTr1Fld);
	DDX_Control(pDX, IDC_PVKI_LEN_TR2_FLD, m_CntlPVKLenTr2Fld);
	DDX_Control(pDX, IDC_PVKI_LEN_TR1_FLD, m_CntlPVKLenTr1Fld);
	DDX_Control(pDX, IDC_OFFSET2_FLD, m_CntlOffset2Fld);
	DDX_Control(pDX, IDC_OFFSET1_FLD, m_CntlOffset1Fld);
	DDX_Control(pDX, IDC_LOCATION2_FLD, m_CntlLocation2Fld);
	DDX_Control(pDX, IDC_LOCATION1_FLD, m_CntlLocation1Fld);
	DDX_Control(pDX, IDC_KEY_RIGHT_FLD, m_CntlKeyRightFld);
	DDX_Control(pDX, IDC_KEY_LEFT_FLD, m_CntlKeyLeftFld);
	DDX_Control(pDX, IDC_KEY_IND_FLD, m_CntlKeyIndFld);
	DDX_Text(pDX, IDC_KEY_IND_FLD, m_StrKeyIndFld);
	DDV_MaxChars(pDX, m_StrKeyIndFld, 1);
	DDX_Text(pDX, IDC_KEY_LEFT_FLD, m_StrKeyLeftFld);
	DDV_MaxChars(pDX, m_StrKeyLeftFld, 16);
	DDX_Text(pDX, IDC_KEY_RIGHT_FLD, m_StrKeyRightFld);
	DDV_MaxChars(pDX, m_StrKeyRightFld, 16);
	DDX_Text(pDX, IDC_LOCATION1_FLD, m_StrLocation1Fld);
	DDV_MaxChars(pDX, m_StrLocation1Fld, 1);
	DDX_Text(pDX, IDC_LOCATION2_FLD, m_StrLocation2Fld);
	DDV_MaxChars(pDX, m_StrLocation2Fld, 1);
	DDX_Text(pDX, IDC_MESSAGE_LBL, m_StrMessageLbl);
	DDX_Text(pDX, IDC_OFFSET1_FLD, m_StrOffset1Fld);
	DDV_MaxChars(pDX, m_StrOffset1Fld, 1);
	DDX_Text(pDX, IDC_OFFSET2_FLD, m_StrOffset2Fld);
	DDV_MaxChars(pDX, m_StrOffset2Fld, 1);
	DDX_Text(pDX, IDC_PVKI_LEN_TR1_FLD, m_StrPVKLenTr1Fld);
	DDV_MaxChars(pDX, m_StrPVKLenTr1Fld, 1);
	DDX_Text(pDX, IDC_PVKI_LEN_TR2_FLD, m_StrPVKLenTr2Fld);
	DDV_MaxChars(pDX, m_StrPVKLenTr2Fld, 1);
	DDX_Text(pDX, IDC_PVKI_LOC_TR1_FLD, m_StrPVKLocTr1Fld);
	DDV_MaxChars(pDX, m_StrPVKLocTr1Fld, 1);
	DDX_Text(pDX, IDC_PVKI_LOC_TR2_FLD, m_StrPVKLocTr2Fld);
	DDV_MaxChars(pDX, m_StrPVKLocTr2Fld, 1);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CCardsPinVerifVisa, CDialog)
	//{{AFX_MSG_MAP(CCardsPinVerifVisa)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCardsPinVerifVisa message handlers

BOOL CCardsPinVerifVisa::OnInitDialog() 
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
         EnablePinVerifVisaDialog(TRUE);
        
        if (m_nDialogMode != MODE_ADD )
        PopulatePinVerifVisaDialogFromDatabase (sProfileIdOld);		
    }    	
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CCardsPinVerifVisa::OnCancel() 
{

	CDialog::OnCancel();
}



/************************************************************/
 ////////////////////////////////////////////////////////////
/************************************************************/

BOOL CCardsPinVerifVisa::PopulatePinVerifVisaDialogFromDatabase( CAF01 sProfileId )
{
     if ( m_nDialogMode != MODE_VIEW )
     EnablePinVerifVisaDialog(TRUE);

     ZeroMemory ( &sProfileIdOld, sizeof (CAF01));

 
    m_StrKeyIndFld     = sProfileId.pin.Visa.pvkey_ind;    
    m_StrKeyLeftFld    = sProfileId.pin.Visa.key_left;
    m_StrKeyRightFld   = sProfileId.pin.Visa.key_right;
	m_StrLocation1Fld  = sProfileId.pin.Visa.pvv_location_tr1;
    m_StrLocation2Fld  = sProfileId.pin.Visa.pvv_location_tr2;
	m_StrOffset1Fld    = sProfileId.pin.Visa.pvv_offset_tr1;
    m_StrOffset2Fld    = sProfileId.pin.Visa.pvv_offset_tr2;
	m_StrPVKLenTr1Fld  = sProfileId.pin.Visa.pvki_length_tr1;
	m_StrPVKLenTr2Fld  = sProfileId.pin.Visa.pvki_length_tr2;
	m_StrPVKLocTr1Fld  = sProfileId.pin.Visa.pvki_location_tr1;
	m_StrPVKLocTr2Fld  = sProfileId.pin.Visa.pvki_location_tr2;
    

    UpdateData(FALSE);

    memcpy ( &sProfileIdOld, &sProfileId, sizeof ( CAF01));

    return TRUE;

}

/************************************************************/
 ////////////////////////////////////////////////////////////
/************************************************************/

BOOL CCardsPinVerifVisa::PopulateDatabaseFromPinVerifVisaDialog( pCAF01 psProfileId )
{   


	memcpy ( psProfileId->pin.Visa.pvkey_ind,                 m_StrKeyIndFld,      sizeof ( psProfileId->pin.Visa.pvkey_ind));
    memcpy ( psProfileId->pin.Visa.key_left,                  m_StrKeyLeftFld,     sizeof ( psProfileId->pin.Visa.key_left));    
    memcpy ( psProfileId->pin.Visa.key_right,                 m_StrKeyRightFld,    sizeof ( psProfileId->pin.Visa.key_right));
	memcpy ( psProfileId->pin.Visa.pvv_location_tr1,          m_StrLocation1Fld,   sizeof ( psProfileId->pin.Visa.pvv_location_tr2));   
    memcpy ( psProfileId->pin.Visa.pvv_location_tr2,          m_StrLocation2Fld,   sizeof ( psProfileId->pin.Visa.pvv_location_tr1));
	memcpy ( psProfileId->pin.Visa.pvv_offset_tr1,            m_StrOffset1Fld,     sizeof ( psProfileId->pin.Visa.pvv_offset_tr1)); 
    memcpy ( psProfileId->pin.Visa.pvv_offset_tr2,            m_StrOffset2Fld,     sizeof ( psProfileId->pin.Visa.pvv_offset_tr2));
    memcpy ( psProfileId->pin.Visa.pvki_length_tr1,            m_StrPVKLenTr1Fld,     sizeof ( psProfileId->pin.Visa.pvki_length_tr1));
    memcpy ( psProfileId->pin.Visa.pvki_length_tr2,            m_StrPVKLenTr2Fld,     sizeof ( psProfileId->pin.Visa.pvki_length_tr2));
    memcpy ( psProfileId->pin.Visa.pvki_location_tr1,            m_StrPVKLocTr1Fld,     sizeof ( psProfileId->pin.Visa.pvki_location_tr1));
    memcpy ( psProfileId->pin.Visa.pvki_location_tr2,            m_StrPVKLocTr2Fld,     sizeof ( psProfileId->pin.Visa.pvki_location_tr2));
    
    return TRUE;

}

/************************************************************/
 ////////////////////////////////////////////////////////////
/************************************************************/

void CCardsPinVerifVisa::EnablePinVerifVisaDialog(int nStatus)
{   
    	if ( m_nDialogMode != MODE_VIEW )
{    
    m_CntlKeyIndFld.EnableWindow(nStatus);
    m_CntlKeyLeftFld.EnableWindow(nStatus);
	m_CntlKeyRightFld.EnableWindow(nStatus);
    m_CntlLocation1Fld.EnableWindow(nStatus);
	m_CntlLocation2Fld.EnableWindow(nStatus);
    m_CntlOffset1Fld.EnableWindow(nStatus);    
    m_CntlOffset2Fld.EnableWindow(nStatus);
	m_CntlPVKLenTr1Fld.EnableWindow(nStatus);
	m_CntlPVKLenTr2Fld.EnableWindow(nStatus);
	m_CntlPVKLocTr1Fld.EnableWindow(nStatus);
	m_CntlPVKLocTr2Fld.EnableWindow(nStatus);

	
}
}

/************************************************************/
 ////////////////////////////////////////////////////////////
/************************************************************/

void CCardsPinVerifVisa::CleanPinVerifVisaDialog()
{
    m_StrMessageLbl = _T("");
	m_StrKeyIndFld = _T("");
	m_StrKeyLeftFld = _T("");
	m_StrKeyRightFld = _T("");
	m_StrLocation1Fld = _T("");
	m_StrLocation2Fld = _T("");
	m_StrOffset1Fld = _T("");
	m_StrOffset2Fld = _T("");
	m_StrPVKLenTr1Fld = _T("");
	m_StrPVKLenTr2Fld = _T("");
	m_StrPVKLocTr1Fld = _T("");
	m_StrPVKLocTr2Fld = _T("");


	UpdateData(FALSE);

}

void CCardsPinVerifVisa::OnOK() 
{
	CMyUtils myUtilsObject;

	UpdateData(TRUE);
	
	if ( myUtilsObject.IsEmpty( m_StrKeyLeftFld, this ) && !myUtilsObject.IsEmpty( m_StrKeyRightFld, this ) )
	{
		m_StrMessageLbl = "Please enter Key Left";
		UpdateData(FALSE);
		m_CntlKeyLeftFld.SetFocus();
		return;
	}
	if ( !myUtilsObject.IsEmpty( m_StrKeyLeftFld, this ) && myUtilsObject.IsEmpty( m_StrKeyRightFld, this ) )
	{
		m_StrMessageLbl = "Please enter Key Right";
		UpdateData(FALSE);
		m_CntlKeyRightFld.SetFocus();
		return;
	}
	
	CDialog::OnOK();
}


BOOL CCardsPinVerifVisa::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	// TODO: Add your specialized code here and/or call the base class
	


	c_time = CTime::GetCurrentTime();	
	shrmem.SetLastUpdateTime(c_time);
	return CDialog::OnCommand(wParam, lParam);
}
