/*******************************************************************************
*  
* MODULE:      PrimaryAccountList.h
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
* $Log:   N:\POS\PVCS6.6\EPICPORTZ\PTE\Equitable\monitor\files\PrimaryAccountList.h  $
   
      Rev 1.3   Apr 04 2005 15:58:50   lmitchel
   Rebuild 4_4_1
   
      Rev 1.2   Aug 29 2000 15:24:36   lmitchel
   New implementation of monitor including new functions for displaying device and network controller statistics and dialog status messages.  Two windows added to the existing monitor to display device and network transaction counts (sent, received, tps rate) and dialog status (connected, disconnected).  The new windows are updated with current data every nn seconds defined by the interval in the monitor.ini file.  Functions added to send and recieved stats and status updates form the controllers.
   
      Rev 1.1   Nov 24 1999 14:32:02   lmitchel
   Recompile w/new libraries ver 3_2_0/core1_3_1_3
   
      Rev 1.0   24 Feb 1999 15:59:08   rmalathk
   Initial Revision
   
      Rev 1.0   Dec 04 1998 15:33:36   MSALEH
   initial release
   
      Rev 1.0   Dec 04 1998 15:00:04   MSALEH
   initial release
   
      Rev 1.0   28 Oct 1998 16:31:00   jhapitas
   Initial checkin
*
*******************************************************************************/
// PrimaryAccountList.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CPrimaryAccountList dialog

class CPrimaryAccountList : public CDialog
{
// Construction
public:
	CPrimaryAccountList(CWnd* pParent = NULL);   // standard constructor

   CHAR m_pan[7];

   struct
   {
      BYTE  identifier[7];
   } m_logon_bin[10];

// Dialog Data
	//{{AFX_DATA(CPrimaryAccountList)
	enum { IDD = IDD_PRIMARYACCOUNT_SELECTION };
	CListBox	m_PrimaryAccountList;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPrimaryAccountList)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CPrimaryAccountList)
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
