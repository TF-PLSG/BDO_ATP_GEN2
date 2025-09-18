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
* $Log:   N:\POS\PVCS6.6\EPICPORTZ\PTE\Equitable\ATP_gui\cardsprocessing.h  $
   
      Rev 1.11   Apr 01 2005 10:13:36   lmitchel
   Initial Checking prior to integration testing for updates made to support country/curreny code limits (scr13929), unulimited purchase card MCC ranges, and purchase card transaction limits(scr12785)
   
      Rev 1.10   Dec 01 2003 11:28:54   lmitchel
   Added AMEX Online Updates - scr1200
   
      Rev 1.9   Dec 16 1999 09:50:02   mbysani
    
   
      Rev 1.1   Sep 22 1999 16:03:52   iarustam
   Added manual batch entry GUI
   
      Rev 1.0   Jul 01 1999 14:18:36   iarustam
   Initial Release
   
      Rev 1.0   Jun 28 1999 13:59:48   iarustam
   Initial Release
   
      Rev 1.7   May 21 1999 10:18:00   iarustam
   Bug fixes
   
   
      Rev 1.6   Apr 29 1999 14:57:18   iarustam
   Bug fixes
   
      Rev 1.5   Apr 15 1999 14:19:44   iarustam
   Bug fixes
   
   
      Rev 1.4   Mar 30 1999 17:40:06   iarustam
   Bug fixes
   
      Rev 1.3   Mar 16 1999 16:30:42   iarustam
   Bug fixes
   
      Rev 1.2   Mar 04 1999 13:32:40   iarustam
   Bug fixes
   
      Rev 1.0   Feb 23 1999 11:41:32   iarustam
   Initial Release
   
      Rev 1.0   Feb 17 1999 10:10:20   iarustam
   Initial Release
   
      Rev 1.1   Jan 22 1999 11:00:36   iarustam
   Bug fixes and core 1_3_0 changes
   
      Rev 1.1   Jan 21 1999 13:10:24   iarustam
   Bug fixes
   
   
      Rev 1.1   Dec 18 1998 13:45:16   iarustam
    
*
************************************************************************************/
// CardsProcessing.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CCardsProcessing dialog
#ifndef CCARDSPROCESSING
#define CCARDSPROCESSING
#include "PosEntryModesListDlg.h"

class CCardsProcessing : public CPropertyPage
{
	DECLARE_DYNCREATE(CCardsProcessing)

// Construction
public:
	CCardsProcessing();
	~CCardsProcessing();
    CAF01 sProfileIdOld;
    CAF01 sProfileIdPinVerif;
	KEY3DS01 sKey3ds01Old;
	

	CPosEntryModesListDlg *m_pPosDlg ;
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

    BOOL PopulateProcessingDialogFromDatabase( CAF01 sProfileId, KEY3DS01 sKey3ds01 );
    BOOL PopulateDatabaseFromProcessingDialog( pCAF01 psProfileId, pKEY3DS01 psKey3ds01 );
    BOOL GetPinOffsetLocation();
    BOOL GetPinCaptureIndicator();
    BOOL GetExpirationDateCheck();
    BOOL GetExpirationDateLocation();
    BOOL GetAuthorizationLocation();
    BOOL GetAuthorizationMethod();
    BOOL GetStatus();
    BOOL GetPinVerificationMethod();
    BOOL CompareStructures(KEY3DS01 sKey3ds01Old);


	CTime c_time;

// Dialog Data
	//{{AFX_DATA(CCardsProcessing)
	enum { IDD = IDD_CARDS_PROCESSING };
	CEdit	m_CntlMCCAK_Fld;
	CEdit	m_Cntl3DCAKFld;
	CButton	m_CntlCSCKDesc;
	CButton m_CntlSlen3Dcak;
	CButton	m_CntlPosEntryModes;
	CButton	m_CntlCntryLimitsCheckFlag;
	CButton m_Cntlmc3dpanFlag;
	CButton	m_Cntlmc3ddstxnFlag;
	CButton	m_Cntlmc3dcurrencyFlag;
	CButton	m_Cntlmc3dtxnamtFlag;
	CButton	m_Cntlmc3dmechnameFlag;
	CEdit	m_CntlFloorLimitFld;
	CEdit	m_CntlCSCKFld;
	CEdit	m_CntlKey3ds01;
	CEdit	m_CntlMKACFld;
	CEdit	m_CntlPINValidationData;
	CEdit	m_CntlDecimalisationTable;
	CComboBox	m_CntlCVVProcessing;
	CEdit	m_CntlCodeOffset1Fld;
	CEdit	m_CntlCodeOffset2Fld;
	CEdit	m_CntlCVVOffset2Fld;
	CEdit	m_CntlCVVOffset1Fld;
	CEdit	m_CntlCVVLengthFld;
	CEdit	m_CntlCVKBFld;
	CEdit	m_CntlCVKAFld;
	CButton	m_NetworkButton;
	CButton	m_PinVerifButton;
	CEdit	m_CntlProcess_Fld;
	CComboBox	m_CntlPinVerifMethod_Fld;
	CComboBox	m_CntlStatus_Fld;
	CEdit	m_CntlMerchSame_Fld;
	CEdit	m_CntlType_Fld;
	CEdit	m_CntlRemoteDest_Fld;
	CEdit	m_CntlProfileId_Fld;
	CComboBox	m_CntlPinOffsetLocation_Fld;
	CComboBox	m_CntlPinCapIndicator_Fld;
	CEdit	m_CntlPhone_Fld;
	CEdit m_Cntl3DS_Fld;
	CEdit	m_CntlNumPinRetries_Fld;
	CEdit	m_CntlName_Fld;
	CComboBox	m_CntlExpDateLocation_Fld;
	CComboBox	m_CntlExpDateCheck_Fld;
	CEdit	m_CntlContact_Fld;
	CComboBox	m_CntlAuthMethod_Fld;
	CComboBox	m_CntlAuthLocation_Fld;
	CString	m_StrAuthLocation_Fld;
	CString	m_StrAuthMethod_Fld;
	CString	m_StrContact_Fld;
	CString	m_StrExpDateCheck_Fld;
	CString	m_StrExpDateLocation_Fld;
	CString	m_StrName_Fld;
	CString	m_StrNumPinRetries_Fld;
	CString	m_StrPhone_Fld;
	CString m_Str3DS_Fld;
	CString	m_StrPinCapIndicator_Fld;
	CString	m_StrProfileId_Fld;
	CString	m_StrRemoteDest_Fld;
	CString	m_StrType_Fld;
	CString	m_StrMessageLbl;
	CString	m_StrMerchSame_Fld;
	CString	m_StrStatus_Fld;
	CString	m_StrPinVerifMethod_Fld;
	CString	m_StrProcess_Fld;
	CString	m_StrCVKAFld;
	CString	m_StrCVKBFld;
	CString	m_StrCVVLengthFld;
	CString	m_StrCVVOffset1Fld;
	CString	m_StrCVVOffset2Fld;
	CString	m_StrCodeOffset1Fld;
	CString	m_StrCodeOffset2Fld;
	CString	m_StrCVVProcessing;
	CString	m_StrCSCKFld;
	CString	m_StrKey3ds01;
	CString	m_StrPinOffsetLocation_Fld;
	CString	m_StrFloorLimitFld;
	CString	m_StrMKACFld;
	CString	m_Str3DCAKFld;
	CString	m_StrMCCAKFld;
	CString	m_StrPINValidationData;
	CString	m_StrDecimalisationTable;
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
	afx_msg void OnPinVerifButton();
	afx_msg void OnSelchangePinVerificationMethodFld();
	afx_msg void OnNetworkButton();
	afx_msg void OnPosEntryModes();
	afx_msg void OnCSCKDesc();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};
#endif
