#if !defined(AFX_TPSGRAPH_H__4C19591A_9B49_40F9_AD73_608A7E056066__INCLUDED_)
#define AFX_TPSGRAPH_H__4C19591A_9B49_40F9_AD73_608A7E056066__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TpsGraph.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CTpsGraph dialog

class CTpsGraph : public CDialog
{
// Construction
public:
	CTpsGraph(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CTpsGraph)
	enum { IDD = IDD_TPS_GRAPH };
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTpsGraph)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CTpsGraph)
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TPSGRAPH_H__4C19591A_9B49_40F9_AD73_608A7E056066__INCLUDED_)
