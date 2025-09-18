#if !defined(AFX_POSENTRYMODESLISTDLG_H__0D955E84_5A2B_4013_9D28_4DB7A9197476__INCLUDED_)
#define AFX_POSENTRYMODESLISTDLG_H__0D955E84_5A2B_4013_9D28_4DB7A9197476__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PosEntryModesListDlg.h : header file
//
#define MAX_POS_MODES 12
/////////////////////////////////////////////////////////////////////////////
// CPosEntryModesListDlg dialog

class CPosEntryModesListDlg : public CDialog
{
// Construction
public:
	CPosEntryModesListDlg(CWnd* pParent = NULL);   // standard constructor
	CButton *m_cChkbx[100];
	char pos_modes[20][256];
	int index;
	CFont NewFont;
	LOGFONT newLogFont;
	unsigned char m_PosEntryModes[2];
	void SetPosEntryModes(unsigned char MSB, unsigned char LSB);
	unsigned char GetPosEntryModesMSB();
	unsigned char GetPosEntryModesLSB();
// Dialog Data
	//{{AFX_DATA(CPosEntryModesListDlg)
	enum { IDD = IDD_CARDS_POSENTRYMODES };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPosEntryModesListDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CPosEntryModesListDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_POSENTRYMODESLISTDLG_H__0D955E84_5A2B_4013_9D28_4DB7A9197476__INCLUDED_)
