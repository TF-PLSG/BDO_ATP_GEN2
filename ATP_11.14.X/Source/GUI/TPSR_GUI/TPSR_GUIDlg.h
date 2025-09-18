// TPSR_GUIDlg.h : header file
//

#if !defined(AFX_TPSR_GUIDLG_H__C836DF7E_A598_40B6_92AC_800673E216D8__INCLUDED_)
#define AFX_TPSR_GUIDLG_H__C836DF7E_A598_40B6_92AC_800673E216D8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "ToolTipButton.h"
#include "StatusSplashWindow.h"
#include "ProcessDialog.h"
#include "utils.h"
/////////////////////////////////////////////////////////////////////////////
// CTPSR_GUIDlg dialog

class CTPSR_GUIDlg : public CPropertyPage
{
	DECLARE_DYNCREATE(CTPSR_GUIDlg)
// Construction
public:
	unsigned char m_isoBitmapBits[65];
	char m_isoBitmapByte[17];
public:
	CTPSR_GUIDlg(CWnd* pParent = NULL);	// standard constructor
	CProcessDialog* proc;
	CUtils utils;
// Dialog Data
	//{{AFX_DATA(CTPSR_GUIDlg)
	enum { IDD = IDD_TPSR_GUI_DIALOG };
	CToolTipButton	m_CntlTrack1DataCheck;
	CToolTipButton	m_CntlTrack2DataCheck;
	CToolTipButton	m_CntlTimeLocalCheck;
	CToolTipButton	m_CntlRetrivalRefNbrCheck;
	CToolTipButton	m_CntlRespCodeCheck;
	CToolTipButton	m_CntlPrivate3Check;
	CToolTipButton	m_CntlPrivate2Check;
	CToolTipButton	m_CntlPrivate1Check;
	CToolTipButton	m_CntlPinDataCheck;
	CToolTipButton	m_CntlEMVDataCheck;
	CToolTipButton	m_CntlDateLocalCheck;
	CToolTipButton	m_CntlBrowseDeviceFile;
	CToolTipButton	m_CntlBrowseCardFile;
	CToolTipButton	m_CntlAuthIDRespCheck;
	CToolTipButton	m_BtnReset;
	CToolTipButton	m_BtnSave;
	CComboBox	m_CntlCardType;
	CComboBox	m_CntlMsgType;
	CEdit		m_CntlTrack2Data;
	CEdit		m_CntlTrack1Data;
	CEdit		m_CntlTimeLocal;
	CEdit		m_CntlSysTrace;
	CEdit		m_CntlRetrivalRefNbr;
	CEdit		m_CntlRespCode;
	CEdit		m_CntlProfileName;
	CEdit		m_CntlProcessingCode;
	CEdit		m_CntlPrivate3;
	CEdit		m_CntlPrivate2;
	CEdit		m_CntlPrivate1;
	CEdit		m_CntlPosEntryMode;
	CEdit		m_CntlPosCondCode;
	CEdit		m_CntlPinData;
	CEdit		m_CntlNII;
	CEdit		m_CntlExpDate;
	CEdit		m_CntlEMVData;
	CEdit		m_CntlDateLocal;
	CEdit		m_CntlBitmap;
	CEdit		m_CntlAuthIDResp;
	CEdit		m_CntlAmount;
	CString		m_StrAmount;
	CString		m_StrAuthIDResp;
	CString		m_StrBitmap;
	CString		m_StrDateLocal;
	CString		m_StrEMVData;
	CString		m_StrExpDate;
	CString		m_StrNII;
	CString		m_StrPinData;
	CString		m_StrPosCondCode;
	CString		m_StrPosEntryMode;
	CString		m_StrPrivate1;
	CString		m_StrPrivate2;
	CString		m_StrPrivate3;
	CString		m_StrProcessingCode;
	CString		m_StrProfileName;
	CString		m_StrRespCode;
	CString		m_StrRetrivalRefNbr;
	CString		m_StrSysTrace;
	CString		m_StrTimeLocal;
	CString		m_StrTrack1Data;
	CString		m_StrTrack2Data;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTPSR_GUIDlg)
	public:
	virtual BOOL OnApply();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	CToolTipCtrl m_ToolTip;
	HICON m_hIcon;
	HBITMAP	m_hSave, m_hReset, m_hCancel;
	// Generated message map functions
	//{{AFX_MSG(CTPSR_GUIDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnBrowseCardFile();
	afx_msg void OnBrowseDeviceFile2();
	afx_msg void OnTimeLocalCheck();
	afx_msg void OnDateLocalCheck();
	afx_msg void OnRetrivalRefNbrCheck();
	afx_msg void OnAuthIdRespCheck();
	afx_msg void OnRespCodeCheck();
	afx_msg void OnTrack1DataCheck();
	afx_msg void OnTrack2DataCheck();
	afx_msg void OnPinDataCheck();
	afx_msg void OnEmvDataCheck();
	afx_msg void OnPrivate1Check();
	afx_msg void OnPrivate2Check();
	afx_msg void OnPrivate3Check();
	virtual void OnOK();
	afx_msg void OnSetfocusProfileName();
	afx_msg void OnReset();
	afx_msg void OnKillfocusCardNbrList();
	afx_msg void OnKillfocusDeviceNbrList();
	afx_msg void OnKillfocusAmount();
	afx_msg void OnKillfocusAuthIDResp();
	afx_msg void OnKillfocusBitmap();
	afx_msg void OnKillfocusDateLocal();
	afx_msg void OnKillfocusExpDate();
	afx_msg void OnKillfocusNII();
	afx_msg void OnKillfocusPinData();
	afx_msg void OnKillfocusPosCondCode();
	afx_msg void OnKillfocusPosEntryMode();
	afx_msg void OnKillfocusProcessingCode();
	afx_msg void OnKillfocusProfileName();
	afx_msg void OnKillfocusRespCode();
	afx_msg void OnKillfocusRetrivalRefNbr();
	afx_msg void OnKillfocusSysTrace();
	afx_msg void OnKillfocusTimeLocal();
	afx_msg void OnKillfocusTrack1Data();
	afx_msg void OnKillfocusTrack2Data();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TPSR_GUIDLG_H__C836DF7E_A598_40B6_92AC_800673E216D8__INCLUDED_)
