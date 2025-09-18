// TranSearchView.h : interface of the CTranSearchView class
//
/////////////////////////////////////////////////////////////////////////////

class CTranSearchView : public CView
{
protected: // create from serialization only
	CTranSearchView();
	DECLARE_DYNCREATE(CTranSearchView)

// Attributes
public:
	CTranSearchDoc* GetDocument();

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTranSearchView)
	public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	protected:
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CTranSearchView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CTranSearchView)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in TranSearchView.cpp
inline CTranSearchDoc* CTranSearchView::GetDocument()
   { return (CTranSearchDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////
