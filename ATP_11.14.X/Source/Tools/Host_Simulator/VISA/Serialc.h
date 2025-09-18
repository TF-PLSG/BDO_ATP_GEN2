// Serialc.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// Serial dialog

class Serialc : public CDialog
{
// Construction
public:
	Serialc(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(Serialc)
	enum { IDD = IDD_SERIAL };
	CButton	m_port;
	CButton	m_com1;
	CButton	m_com2;
	CButton	m_baudrate;
	CButton	m_9600;
	CButton	m_19200;
	CButton	m_38400;
	CButton	m_bytesize;
	CButton	m_size8;
	CButton	m_size7;
	CButton	m_mparity;
	CButton	m_none;
	CButton	m_odd;
	CButton	m_even;
	CButton	m_stopbits;
	CButton	m_stop1;
	CButton	m_stop12;
	CButton	m_stop2;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(Serial)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(Serialc)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
