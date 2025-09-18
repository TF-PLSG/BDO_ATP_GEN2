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
    
*
************************************************************************************/
// CardsKeyblockProcessing.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CCardsProcessing dialog

//#include "PosEntryModesListDlg.h"
#ifndef CCARDSKEYBLOCKPROCESSING
#define CCARDSKEYBLOCKPROCESSING

class CCardsKeyblockProcessing : public CPropertyPage
{
	DECLARE_DYNCREATE(CCardsKeyblockProcessing)

// Construction
public:
	CCardsKeyblockProcessing();
	~CCardsKeyblockProcessing();
    CAF01 sProfileIdOld;
    CAF01 sProfileIdPinVerif;
	KEY3DS01 sKey3ds01Old;
	CAF01_KEYBLOCK sCaf01_keyblkOld;		
    int m_nDialogMode;
    int m_nCardMode;

    char strStatus[2];
    char m_StrCurrentData[151];
    char m_StrPreviousData[151];
    char m_StrFieldTag[50];  
	unsigned char m_PosEntryModes[2];

    BOOL m_bProcessing;
    
    void CleanProcessingDialog();
    void EnableProcessingDialog(int);
    void PopulateHistoryTable();

    BOOL PopulateProcessingDialogFromDatabase( CAF01_KEYBLOCK sProfileId );
    BOOL PopulateDatabaseFromProcessingDialog( pCAF01_KEYBLOCK psProfileId );
    BOOL GetPinOffsetLocation();
    BOOL GetPinCaptureIndicator();
    BOOL GetExpirationDateCheck();
    BOOL GetExpirationDateLocation();
    BOOL GetAuthorizationLocation();
    BOOL GetAuthorizationMethod();
    BOOL GetStatus();
    BOOL GetPinVerificationMethod();
	BOOL CompareStructures();


	CTime c_time;

// Dialog Data
	//{{AFX_DATA(CCardsProcessing)
	enum { IDD = ID_CAF01_KEYBLK_GENERAL };
	CEdit	m_CntlCvk_Mag_KeyblockFld;
	CEdit	m_CntlCvk_Acq_3DS_KeyblockFld;
	CEdit	m_CntlCvk_Pin_VerificationFld;
	CEdit	m_CntlCvk_Mkac_KeyblockFld;
	CEdit	m_CntlCsck_Iss_3ds_KeyblockFld;
	CEdit	m_CntlCsck_Mag_KeyblockFld;
	CEdit	m_CntlDci_3ds_Keyblock_Fld;
	CEdit	m_CntlType_Fld;
	CEdit	m_CntlProfileId_Fld;
	CEdit	m_CntlCvk_3ds_Cak_KeyblockFld;	

	CString	m_StrCvk_Mag_KeyblockFld;
	CString	m_StrCvk_Acq_3DS_KeyblockFld;
	CString	m_StrCvk_Pin_VerificationFld;
	CString	m_StrCvk_Mkac_KeyblockFld;
	CString	m_StrCsck_Iss_3ds_KeyblockFld;
	CString	m_StrCsck_Mag_KeyblockFld;
	CString	m_StrDci_3ds_Keyblock_Fld;
	CString	m_StrCvk_3ds_Cak_KeyblockFld;

	CString	m_StrProfileId_Fld;
	CString	m_StrType_Fld;
	CString	m_StrMessageLbl;


	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CCardsProcessing)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CCardsProcessing)
	virtual BOOL OnInitDialog();
	/*afx_msg void OnPinVerifButton();
	afx_msg void OnSelchangePinVerificationMethodFld();
	afx_msg void OnNetworkButton();
	afx_msg void OnPosEntryModes();
	afx_msg void OnCSCKDesc();*/
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};
#endif
