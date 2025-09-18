#if !defined(AFX_CARDANDTRACKDATADLG_H__463688C7_A863_431D_A3B6_4BCD6B2A6DF2__INCLUDED_)
#define AFX_CARDANDTRACKDATADLG_H__463688C7_A863_431D_A3B6_4BCD6B2A6DF2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CardAndTrackDataDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CCardAndTrackDataDlg dialog

class CCardAndTrackDataDlg : public CPropertyPage
{
	DECLARE_DYNCREATE(CCardAndTrackDataDlg)

// Construction
public:
	CCardAndTrackDataDlg();
	~CCardAndTrackDataDlg();
	CString m_CardNumbers[512];
	CString m_TrackData[512];
// Dialog Data
	//{{AFX_DATA(CCardAndTrackDataDlg)
	enum { IDD = IDD_CARD_TRACK_DATA };
	CRichEditCtrl	m_CntlTrackData;
	CRichEditCtrl	m_CntlCardNumbers;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CCardAndTrackDataDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CCardAndTrackDataDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CARDANDTRACKDATADLG_H__463688C7_A863_431D_A3B6_4BCD6B2A6DF2__INCLUDED_)
