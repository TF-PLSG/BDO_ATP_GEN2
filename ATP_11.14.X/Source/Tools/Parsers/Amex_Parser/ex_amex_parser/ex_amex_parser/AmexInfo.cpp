// AmexInfo.cpp : implementation file
//

#include "stdafx.h"
#include "AmexInfo.h"
#include "afxdialogex.h"


// AmexInfo dialog

IMPLEMENT_DYNAMIC(AmexInfo, CDialogEx)

AmexInfo::AmexInfo(CWnd* pParent /*=NULL*/)
	: CDialogEx(AmexInfo::IDD, pParent)
{

}

AmexInfo::~AmexInfo()
{
}

void AmexInfo::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(AmexInfo, CDialogEx)
END_MESSAGE_MAP()


// AmexInfo message handlers
