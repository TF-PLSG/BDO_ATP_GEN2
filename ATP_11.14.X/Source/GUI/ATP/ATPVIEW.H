/*********************************************************************************
* Module:
*
* Title:
*
* Description:
*
* Application:
*
* Author:
*
* Revision History:
*  
* $Log:   N:\PVCS\PTE\Equitable\ATP_gui\ATPVIEW.H  $
   
      Rev 1.5   Dec 01 2003 11:28:40   lmitchel
   Added AMEX Online Updates - scr1200
   
      Rev 1.4   Jan 05 2000 14:52:38   gbarnes
   Major update. Not updated to PVCS recently.
   PVCS was locked.
   
      Rev 1.1   Sep 22 1999 15:40:38   iarustam
   Added manual batch entry GUI
   
      Rev 1.0   Jul 01 1999 14:16:46   iarustam
   Initial Release
   
      Rev 1.0   Jun 28 1999 13:57:48   iarustam
   Initial Release
   
      Rev 1.1   Apr 29 1999 14:55:38   iarustam
   Bug fixes
   
      Rev 1.0   Feb 23 1999 11:39:52   iarustam
   Initial Release
   
      Rev 1.0   Feb 17 1999 10:08:54   iarustam
   Initial Release
   
      Rev 1.1   Jan 22 1999 10:57:14   iarustam
   Bug fixes and core 1_3_0 changes
   
      Rev 1.1   Jan 21 1999 13:23:02   iarustam
   Bug fixes
   
   
      Rev 1.1   Dec 18 1998 13:41:34   iarustam
    
*
************************************************************************************/
// AtpView.h : interface of the CAtpView class
//
/////////////////////////////////////////////////////////////////////////////
#include "menutab.h"

class CAtpView : public CScrollView
{
protected: // create from serialization only
	CAtpView();
	DECLARE_DYNCREATE(CAtpView)

// Attributes
public:
	CAtpDoc* GetDocument();
    BOOLEAN AppCleanUp();
   struct menu_struct* find_menu_table( pCHAR menu_item );

// Operations
public:
    CDC*        m_pdcDisplayMemory;
    CBitmap*    m_pBitmap;
    CSize       m_sizeSource, m_sizeDest;
    CAtpApp*    m_pApplicationObject;
    
	//CAtpDoc* m_pDocumentObject ;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAtpView)
	public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual void OnInitialUpdate();
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CAtpView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CAtpView)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in AtpView.cpp
inline CAtpDoc* CAtpView::GetDocument()
   { return (CAtpDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////
