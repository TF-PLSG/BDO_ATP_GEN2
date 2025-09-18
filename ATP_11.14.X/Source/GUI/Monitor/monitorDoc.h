/*******************************************************************************
*  
* MODULE:      monitordoc.h
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
* $Log:   N:\POS\PVCS6.6\EPICPORTZ\PTE\Equitable\monitor\files\monitorDoc.h  $
   
      Rev 1.3   Apr 04 2005 15:58:48   lmitchel
   Rebuild 4_4_1
   
      Rev 1.2   Aug 29 2000 15:24:38   lmitchel
   New implementation of monitor including new functions for displaying device and network controller statistics and dialog status messages.  Two windows added to the existing monitor to display device and network transaction counts (sent, received, tps rate) and dialog status (connected, disconnected).  The new windows are updated with current data every nn seconds defined by the interval in the monitor.ini file.  Functions added to send and recieved stats and status updates form the controllers.
   
      Rev 1.1   Nov 24 1999 14:32:04   lmitchel
   Recompile w/new libraries ver 3_2_0/core1_3_1_3
   
      Rev 1.0   24 Feb 1999 15:59:18   rmalathk
   Initial Revision
   
      Rev 1.0   Dec 04 1998 15:33:38   MSALEH
   initial release
   
      Rev 1.0   Dec 04 1998 15:00:12   MSALEH
   initial release
   
      Rev 1.0   28 Oct 1998 16:31:16   jhapitas
   Initial checkin
*
*******************************************************************************/
// monitorDoc.h : interface of the CMonitorDoc class
//
/////////////////////////////////////////////////////////////////////////////

class CMonitorDoc : public CDocument
{
protected: // create from serialization only
	CMonitorDoc();
	DECLARE_DYNCREATE(CMonitorDoc)

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMonitorDoc)
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CMonitorDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CMonitorDoc)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
