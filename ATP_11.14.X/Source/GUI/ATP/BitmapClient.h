// BitmapClient.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CBitmapClient window

class CBitmapClient : public CWnd
{
// Construction
public:
	CBitmapClient();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBitmapClient)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CBitmapClient();

	// Generated message map functions
protected:
    CBitmap m_bmp;
	//{{AFX_MSG(CBitmapClient)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
