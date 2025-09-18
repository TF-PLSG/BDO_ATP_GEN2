/*******************************************************************************
*  
* MODULE:      mainfrm.h
*  
* TITLE:       
*  
* DESCRIPTION:
*
* APPLICATION: Ascendent System Monitor
*
* AUTHOR:  Ram Malathkar
*
* REVISION HISTORY:
*
* $Log:   N:\POS\PVCS6.6\EPICPORTZ\PTE\Equitable\monitor\files\MAINFRM.H  $
   
      Rev 1.3   Apr 04 2005 15:58:44   lmitchel
   Rebuild 4_4_1
   
      Rev 1.2   Aug 29 2000 15:24:34   lmitchel
   New implementation of monitor including new functions for displaying device and network controller statistics and dialog status messages.  Two windows added to the existing monitor to display device and network transaction counts (sent, received, tps rate) and dialog status (connected, disconnected).  The new windows are updated with current data every nn seconds defined by the interval in the monitor.ini file.  Functions added to send and recieved stats and status updates form the controllers.
   
      Rev 1.1   Nov 24 1999 14:31:58   lmitchel
   Recompile w/new libraries ver 3_2_0/core1_3_1_3
   
      Rev 1.0   24 Feb 1999 15:59:00   rmalathk
   Initial Revision
   
      Rev 1.0   Dec 04 1998 15:33:32   MSALEH
   initial release
   
      Rev 1.0   Dec 04 1998 15:00:02   MSALEH
   initial release
   
      Rev 1.0   28 Oct 1998 16:30:58   jhapitas
   Initial checkin
*
*******************************************************************************/
// MainFrm.h : interface of the CMainFrame class
//
/////////////////////////////////////////////////////////////////////////////
#ifndef MONITORVIEW
#include "monitordcoview.h"
#endif

class CMainFrame : public CFrameWnd
{
protected: // create from serialization only
	CMainFrame();
	DECLARE_DYNCREATE(CMainFrame)

// Attributes
public:

// Operations
public:
	CMonitorView* m_pMonitorView ;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMainFrame)
	public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual void ActivateFrame(int nCmdShow = -1);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:  // control bar embedded members
	CStatusBar  m_wndStatusBar;
	//CToolBar    m_wndToolBar;

// Generated message map functions
protected:
	//{{AFX_MSG(CMainFrame)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
