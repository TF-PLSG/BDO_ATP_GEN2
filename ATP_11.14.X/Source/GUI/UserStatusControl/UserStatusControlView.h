// UserStatusControlView.h : interface of the CUserStatusControlView class
//
/////////////////////////////////////////////////////////////////////////////

class CUserStatusControlView : public CView
{
protected: // create from serialization only
	CUserStatusControlView();
	DECLARE_DYNCREATE(CUserStatusControlView)

// Attributes
public:
	CUserStatusControlDoc* GetDocument();

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CUserStatusControlView)
	public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	protected:
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CUserStatusControlView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CUserStatusControlView)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in UserStatusControlView.cpp
inline CUserStatusControlDoc* CUserStatusControlView::GetDocument()
   { return (CUserStatusControlDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////
