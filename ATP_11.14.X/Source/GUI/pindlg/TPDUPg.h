// TPDUPg.h : header file
//
#define TLIST	0
#define RLIST	1

/////////////////////////////////////////////////////////////////////////////
// CTPDUPg dialog

class CTPDUPg : public CPropertyPage
{
	DECLARE_DYNCREATE(CTPDUPg)

// Construction
public:
	CTPDUPg();
	~CTPDUPg();
	void BuildTPDUStr();
	void BuildRouterStr();
	void SetNewKey();
	
// Dialog Data
	//{{AFX_DATA(CTPDUPg)
	enum { IDD = IDD_TPDU_ROUTER };
	CButton	m_Apply2_btn;
	CButton	m_Add_btn;
	CStatic	m_TranLen_lbl;
	CStatic	m_TPDUStr_lbl;
	CStatic	m_TPDUId_lbl;
	CButton	m_TPDU_tbl_grb;
	CStatic	m_SrcLen;
	CButton	m_Router_tbl_grb;
	CStatic	m_Offset_lbl;
	CStatic	m_LoRange_lbl;
	CStatic	m_Length_lbl;
	CStatic	m_HiRange_lbl;
	CButton	m_Edit_btn;
	CStatic	m_DataType_lbl;
	CStatic	m_DestQue_lbl;
	CStatic	m_DestLen_lbl;
	CButton	m_Delete_btn;
	CStatic	m_AppDesc_lbl;
	CListBox	m_Router_tbl;
	CListBox	m_TPDU_tbl;
	//}}AFX_DATA

char BldString[150];
int ActiveList;
char ErrMsg[100];
CString m_MesString;
CPropertySheet *m_PSheet;


int RouterChangeNumber;
int TPDUChangeNumber;

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CTPDUPg)
	public:
	virtual BOOL OnSetActive();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CTPDUPg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSetfocusRouterTblLst();
	afx_msg void OnSetfocusTpduTblLst();
	afx_msg void OnAdd();
	afx_msg void OnEdit();
	afx_msg void OnDelete();
	afx_msg void OnDblclkRouterTblLst();
	afx_msg void OnDblclkTpduTblLst();
	afx_msg void OnClose2();
	afx_msg void OnApply2();
	afx_msg void OnHelp2();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};
