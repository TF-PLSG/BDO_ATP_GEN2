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
* $Log:   N:\PVCS\PTE\Equitable\ATP_gui\ATPDOC.H  $
   
      Rev 1.5   Dec 01 2003 11:28:38   lmitchel
   Added AMEX Online Updates - scr1200
   
      Rev 1.4   Jan 05 2000 14:52:38   gbarnes
   Major update. Not updated to PVCS recently.
   PVCS was locked.
   
      Rev 1.1   Sep 22 1999 15:40:28   iarustam
   Added manual batch entry GUI
   
      Rev 1.0   Jul 01 1999 14:16:44   iarustam
   Initial Release
   
      Rev 1.0   Jun 28 1999 13:57:48   iarustam
   Initial Release
   
      Rev 1.1   Apr 29 1999 14:55:38   iarustam
   Bug fixes
   
      Rev 1.0   Feb 23 1999 11:39:50   iarustam
   Initial Release
   
      Rev 1.0   Feb 17 1999 10:08:54   iarustam
   Initial Release
   
      Rev 1.1   Jan 22 1999 10:57:10   iarustam
   Bug fixes and core 1_3_0 changes
   
      Rev 1.1   Jan 22 1999 09:51:16   iarustam
   Changes for core 1_3_0
   
   
      Rev 1.1   Dec 18 1998 13:40:48   iarustam
    
*
************************************************************************************/
// AtpDoc.h : interface of the CAtpDoc class
//
/////////////////////////////////////////////////////////////////////////////

class CAtpDoc : public CDocument
{
protected: // create from serialization only
	CAtpDoc();
	DECLARE_DYNCREATE(CAtpDoc)

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAtpDoc)
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CAtpDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CAtpDoc)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
