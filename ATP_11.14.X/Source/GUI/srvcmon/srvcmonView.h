/****************************************************************************************

   Module: srvcmonView.h

   Title: Ascendent Services Monitor

   $Log:   N:\pvcs\PTE\CORE\Srvcmgr2\Gui\srvcmonView.h  $
   
      Rev 1.3   Nov 18 1998 10:05:42   MSALEH
   Ran through Purify and corrected some
   calls to free()
   
      Rev 1.2   Nov 17 1998 12:12:38   MSALEH
   Added new functionality to manager
   logger/reply Primary/secondary designation,
   it works with logger usiong special message
   subtypes under MT_SYSTEM_REQUEST
   
      Rev 1.1   Sep 04 1998 13:11:54   skatuka2
   1. changed 'pinnacle' to 'ascedent'
   
      Rev 1.0   27 May 1998 16:54:28   rmalathk
   Initial Revision
   
*****************************************************************************************/
// srvcmonView.h : interface of the CSrvcmonView class
//
/////////////////////////////////////////////////////////////////////////////
#ifndef __AFXEXT_H__
#include <afxext.h>
#endif
#define  IDT_TIMER  WM_USER + 200
#ifndef SRVCMONVIEW
#define SRVCMONVIEW

#include "srvcinfo.h"
#include "Lock.h"
#include "afxwin.h"
		   class CSrvcmonView : public CFormView
{
protected: // create from serialization only
	CSrvcmonView();
	DECLARE_DYNCREATE(CSrvcmonView)

public:
	//{{AFX_DATA(CSrvcmonView)
	enum { IDD = IDD_SRVCMON_DIALOG };
	CButton	m_CntlEncOn;
	CButton	m_CntlEncOff;
	CButton	m_CntlFalconOff;
	CButton	m_CntlFalconOn;
	CButton	m_CntlMraOff;
	CButton	m_CntlMraOn;
	CButton m_CntlNullcheckOn;
	CButton m_CntlNullcheckOff;
	CButton m_CntlDbstaticsOn;
	CButton m_CntlDbstaticsOff;
	CButton m_CntlLoadRpsflag;
	CButton m_CntlLoadOBSrecords;
	CButton m_CntlLoadVISA3DS2records;
	CButton m_CntlLoadAMEX3DS2records;
	CButton m_CntlLoadDINERS3DS2records;
	CButton m_CntlLoadJCB3DS2records;
	CButton m_CntlLoadJCB3DS2recordstfini;
	CButton m_CntlLoadCUP3DS2records;
	CButton m_CntlLoadMIDCpngrecords;
	CButton m_CntlReLoadFalconflag;
	CButton m_CntlFalconRTOn;
	CButton m_CntlFalconRTOff;
	CButton	m_CntlLoadToRcValue;
	CButton m_CntlLoadFaiDeclineRcValue;
	CButton m_CntlLoadSchemeTORcValue;
	CButton m_CntlLoadReversalattepmtsLR;
	CButton m_CntlLoadReversalattepmtsTO;
	CButton m_CntlLoadSecExpiryDateValidation;
	CButton m_CntlFalconRT_EXT10On;
	CButton m_CntlFalconRT_EXT10Off;
	CButton m_CntlKeyblockOn;
	CButton m_CntlKeyblockOff;
	CListBox m_ServiceList;
	CTabCtrl m_MainTab;
	//}}AFX_DATA

// Attributes
public:
	//CSrvcmonDoc* GetDocument();
    SERVICE_INFO m_ServiceTable [MAX_SERVICES];
    SERVICE_INFO m_TempTable [MAX_SERVICES];
    INT m_NumServices;
    INT m_TabSelected;

    UINT m_RefreshTimer;
    INT  m_AutoRefreshOff;
    CHAR m_PingStatus[MAX_SERVICES][15];

    CHAR m_ThisMachine[56];
    CHAR m_OtherMachine[50];
    CHAR m_XipcInstance[12];
    CHAR m_ConnectString[256];
    INT  m_HighAvailability;
    INT  m_FirstUpdate;

    void InitLogins();
    void TerminateLogins();
    void RefreshList();
    void ModifyList( pSERVICE_INFO service_info, INT index );

// Operations
public:

    CFrameWnd* m_StatusWindow;
	CImageList* m_pImageList ;
	CWinThread* m_pDataServerThreadObject ;
	/** Store pointer to the application object **/
	CSrvcmonApp* m_pApplicationObject ;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSrvcmonView)
	public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual void OnInitialUpdate();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);	
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

	UINT timer_handle;
	int cntr;
	int idle_time;
	char idle_time_val[7];
	int status;
	CTime c_time, shm_time;
	int ret;

	bool StopTimer();
	int StartTimer(int time);
	void DisableEditControls();
	void EnableEditControls();
	short LockScreen();
	void ResetCntr();
	void UpdateLastEventUpdateTime();
	
// Implementation
public:
	virtual ~CSrvcmonView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CSrvcmonView)
	afx_msg void OnStart();
	afx_msg void OnStop();
	afx_msg void OnStartall();
	afx_msg void OnStopall();
	afx_msg void OnTraceoff();
	afx_msg void OnTraceon();
	afx_msg void OnClose();
	afx_msg void OnDisable();
	afx_msg void OnEnable();
	afx_msg void OnPing();
	afx_msg void OnPingall();
	afx_msg void OnRefresh();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnSelchangeServiceTab(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSysConfig();
	afx_msg void OnEncryptOn();
	afx_msg void OnEncryptOff();
	afx_msg void OnMraOn();
	afx_msg void OnMraOff();
	afx_msg void OnFalconOn();
	afx_msg void OnFalconOff();
	afx_msg void OnSelchangeServiceList();
	afx_msg void OnNullcheckOn();
	afx_msg void OnNullcheckOff();
	afx_msg void OnDbstaticsOn();
	afx_msg void OnDbstaticsOff();
	afx_msg void OnLoadRpsFlag();
	afx_msg void OnBnClickedReloadObs();
	afx_msg void OnBnClickedReloadVisa3ds2();
	afx_msg void OnBnClickedReloadAmex3ds2();
	afx_msg void OnBnClickedReloadDiners3ds2();
	afx_msg void OnBnClickedReloadJcb3ds2();
	afx_msg void OnBnClickedReloadJCB3DS2recordstfini();
	afx_msg void OnBnClickedReloadCup3ds2();
	afx_msg void OnBnClickedReloadMIDCpng();
	afx_msg void OnReLoadFalcon();
	afx_msg void OnFalconRTOn();
	afx_msg void OnFalconRTOff();
	afx_msg void OnLoadToRcValue_forallcardbrands();
	afx_msg void OnLoadFaiDeclineRcValue_forallcardbrands();
	afx_msg void OnLoadReversalattepmts_for_TORequest();
	afx_msg void OnLoadReversalattepmts_for_LateResponse();
	afx_msg void OnReLoadSecExpiryDateValidationFlag();
	afx_msg void OnFalconRT_EXT10On();
	afx_msg void OnFalconRT_EXT10Off();
	afx_msg void OnKeyblockOn();
	afx_msg void OnKeyblockOff();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	CDMLock * pLock;

public:
	afx_msg void OnBnClickedSecondaryexpirydatevalReload();
	afx_msg void OnClickedVertexonCardStatus();
	CButton m_VertexonCardStatus;
};

/*
#ifndef _DEBUG  // debug version in srvcmonView.cpp
inline CSrvcmonDoc* CSrvcmonView::GetDocument()
   { return (CSrvcmonDoc*)m_pDocument; }
#endif
*/


#endif
/////////////////////////////////////////////////////////////////////////////
