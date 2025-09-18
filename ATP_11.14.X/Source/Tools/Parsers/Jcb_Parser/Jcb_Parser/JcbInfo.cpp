// JcbInfo.cpp : implementation file
//

#include "stdafx.h"
#include "Jcb_Parser.h"
#include "JcbInfo.h"
#include "afxdialogex.h"


// JcbInfo dialog

IMPLEMENT_DYNAMIC(JcbInfo, CDialogEx)

JcbInfo::JcbInfo(CWnd* pParent /*=NULL*/)
	: CDialogEx(JcbInfo::IDD, pParent)
{

}

JcbInfo::~JcbInfo()
{
}

void JcbInfo::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(JcbInfo, CDialogEx)
END_MESSAGE_MAP()


// JcbInfo message handlers
