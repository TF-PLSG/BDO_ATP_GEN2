#pragma once
#include "Resource.h"

// AmexInfo dialog

class AmexInfo : public CDialogEx
{
	DECLARE_DYNAMIC(AmexInfo)

public:
	AmexInfo(CWnd* pParent = NULL);   // standard constructor
	virtual ~AmexInfo();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
};
