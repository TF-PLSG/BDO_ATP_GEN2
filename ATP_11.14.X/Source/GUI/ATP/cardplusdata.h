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
* $Log:   N:\PVCS\PTE\Equitable\ATP_gui\cardplusdata.h  $
   
      Rev 1.5   Dec 01 2003 11:28:50   lmitchel
   Added AMEX Online Updates - scr1200
   
      Rev 1.4   Jan 05 2000 14:54:36   gbarnes
   Major update. Not updated to PVCS recently.
   PVCS was locked.
   
      Rev 1.1   Sep 22 1999 16:03:38   iarustam
   Added manual batch entry GUI
   
      Rev 1.0   Jul 01 1999 14:18:36   iarustam
   Initial Release
   
      Rev 1.0   Jun 28 1999 13:59:46   iarustam
   Initial Release
   
      Rev 1.2   May 10 1999 10:59:24   dperson
   Added PVCS header
*   
************************************************************************************/
// CardPlusData.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CCardPlusData dialog
#ifndef CARDPLUSDATAPAGE
#define CARDPLUSDATAPAGE

class CCardPlusData : public CPropertyPage
{
	DECLARE_DYNCREATE(CCardPlusData)

// Construction
public:
	CCardPlusData();
	~CCardPlusData();

	CTime c_time;

// Dialog Data
	//{{AFX_DATA(CCardPlusData)
	enum { IDD = IDD_CARDPLUSDATA };
	CButton	m_ctrlActionReturn;
	CButton	m_ctrlActionOk;
	CEdit	m_ctrlCardType;
	CEdit	m_ctrlCardNbr;
	BOOL	m_Accessories;
	BOOL	m_Accident;
	BOOL	m_AutoGas;
	BOOL	m_Battaries;
	CString	m_CardNbr;
	CString	m_CardType;
	BOOL	m_CleanGo;
	BOOL	m_CrindAuto;
	BOOL	m_Diesel;
	BOOL	m_Driver;
	BOOL	m_DriverId;
	BOOL	m_FleetId;
	BOOL	m_Gold;
	CString	m_LastOdometer;
	CString	m_LitreLimit;
	BOOL	m_Meals;
	BOOL	m_Odometer;
	BOOL	m_Oil;
	BOOL	m_OrderNbr;
	BOOL	m_Parking;
	CString	m_RegNbr;
	BOOL	m_Repairs;
	BOOL	m_Service;
	BOOL	m_Shop;
	BOOL	m_Signature;
	BOOL	m_Super;
	BOOL	m_SuperLL;
	CString	m_TimeLimit;
	BOOL	m_Tyres;
	BOOL	m_Unleaded;
	CString	m_ValueLimit;
	BOOL	m_VehicleId;
	CString	m_strComEdit;
	int		m_ActionOk;
	int		m_ActionReturn;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CCardPlusData)
	public:
	virtual BOOL OnSetActive();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CCardPlusData)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};
#endif
