#pragma once
#include "Resource.h"

// AmexInfo dialog

class MCInfo : public CDialogEx
{
	DECLARE_DYNAMIC(MCInfo)

public:
	MCInfo(CWnd* pParent = NULL);   // standard constructor
	virtual ~MCInfo();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
};
