; CLW file contains information for the MFC ClassWizard

[General Info]
Version=1
LastClass=CAboutDlg
LastTemplate=CDialog
NewFileInclude1=#include "stdafx.h"
NewFileInclude2=#include "stipmgr.h"
LastPage=0

ClassCount=9
Class1=CListCtrlEx
Class2=CMyUtils
Class3=CStipmgrApp
Class4=CAboutDlg
Class5=CStipmgrDlg
Class6=CStipSearch

ResourceCount=8
Resource1=IDD_ABOUTBOX
Class7=CHelpstipmgr
Resource2=IDD_DIALOG_HELP
Class8=CHelpSTIP
Resource3=IDD_IDLE_LOGIN
Class9=CLoginfrm
Resource4=IDD_STIPMGR_DIALOG
Resource5=IDD_DIALOG_HELP (English (U.S.))
Resource6=IDD_ABOUTBOX (English (U.S.))
Resource7=IDD_STIPMGR_DIALOG (English (U.S.))
Resource8=IDD_IDLE_LOGIN (English (U.S.))

[CLS:CListCtrlEx]
Type=0
BaseClass=CListCtrl
HeaderFile=ListCtrlEx.h
ImplementationFile=ListCtrlEx.cpp

[CLS:CMyUtils]
Type=0
BaseClass=CWnd
HeaderFile=MYUTILS.H
ImplementationFile=MYUTILS.CPP
LastObject=CMyUtils

[CLS:CStipmgrApp]
Type=0
BaseClass=CWinApp
HeaderFile=stipmgr.h
ImplementationFile=stipmgr.cpp

[CLS:CAboutDlg]
Type=0
BaseClass=CDialog
HeaderFile=stipmgrDlg.cpp
ImplementationFile=stipmgrDlg.cpp
LastObject=IDC_STATIC_APPVERSION
Filter=D
VirtualFilter=dWC

[CLS:CStipmgrDlg]
Type=0
BaseClass=CDialog
HeaderFile=stipmgrDlg.h
ImplementationFile=stipmgrDlg.cpp
Filter=D
VirtualFilter=dWC
LastObject=CStipmgrDlg

[CLS:CStipSearch]
Type=0
BaseClass=CDialog
HeaderFile=StipSearch.h
ImplementationFile=StipSearch.cpp

[DLG:IDD_ABOUTBOX]
Type=1
Class=CAboutDlg
ControlCount=4
Control1=IDC_STATIC,static,1342177283
Control2=IDC_STATIC_APPVERSION,static,1342308480
Control3=IDC_STATIC,static,1342308352
Control4=IDOK,button,1342373889

[DLG:IDD_STIPMGR_DIALOG]
Type=1
Class=CStipmgrDlg
ControlCount=20
Control1=IDCANCEL,button,1342242817
Control2=IDC_BUTTON_RESEND,button,1342242817
Control3=IDC_BUTTON_RESEND_ALL,button,1342242817
Control4=IDC_STATIC,static,1342308352
Control5=IDC_STATIC,static,1342308352
Control6=IDC_STATIC,static,1342308352
Control7=IDC_STATIC,static,1342308352
Control8=IDC_EDIT_CARD_NUM,edit,1350566016
Control9=IDC_EDIT_TRAN_DATE,edit,1350631552
Control10=IDC_EDIT3,edit,1350631552
Control11=IDC_COMBO_NETWORKCONTROLLER,combobox,1344339971
Control12=IDC_STATIC_LBL,static,1342181388
Control13=IDC_STATIC,button,1342177287
Control14=IDC_LIST_RESEND,SysListView32,1350632457
Control15=IDC_SEARCH_BUTTON,button,1342242817
Control16=IDC_CHECK,button,1342242819
Control17=IDC_EDIT_COUNTER,edit,1350631552
Control18=IDC_STATIC,static,1342308352
Control19=IDC_BUTTON_HELP,button,1342242817
Control20=IDC_BUTTON_NEXT10,button,1342242817

[DLG:_UNKNOWN_RESOURCE_ID_]
Type=1
Class=CStipSearch

[CLS:CHelpstipmgr]
Type=0
HeaderFile=Helpstipmgr.h
ImplementationFile=Helpstipmgr.cpp
BaseClass=CDialog
Filter=D

[DLG:IDD_DIALOG_HELP]
Type=1
Class=CHelpSTIP
ControlCount=10
Control1=IDOK,button,1342242817
Control2=IDCANCEL,button,1342242816
Control3=IDC_STATIC,static,1342308352
Control4=IDC_STATIC,static,1342308352
Control5=IDC_STATIC,static,1342308352
Control6=IDC_STATIC,static,1342308352
Control7=IDC_STATIC,static,1342308352
Control8=IDC_STATIC,static,1342308352
Control9=IDC_STATIC,static,1342308352
Control10=IDC_STATIC,static,1342308352

[CLS:CHelpSTIP]
Type=0
HeaderFile=HelpSTIP.h
ImplementationFile=HelpSTIP.cpp
BaseClass=CDialog
Filter=D
LastObject=CHelpSTIP

[DLG:IDD_IDLE_LOGIN]
Type=1
Class=CLoginfrm
ControlCount=6
Control1=IDC_EDIT_UID,edit,1350631560
Control2=IDC_EDIT_PWD,edit,1350631584
Control3=IDOK,button,1342242817
Control4=IDCANCEL,button,1342242816
Control5=IDC_STATIC_PWD,static,1342308352
Control6=IDC_STATIC_UID,static,1342308352

[CLS:CLoginfrm]
Type=0
HeaderFile=Loginfrm.h
ImplementationFile=Loginfrm.cpp
BaseClass=CDialog
Filter=D
VirtualFilter=dWC
LastObject=CLoginfrm

[DLG:IDD_ABOUTBOX (English (U.S.))]
Type=1
Class=?
ControlCount=4
Control1=IDC_STATIC,static,1342177283
Control2=IDC_STATIC_APPVERSION,static,1342308480
Control3=IDC_STATIC,static,1342308352
Control4=IDOK,button,1342373889

[DLG:IDD_STIPMGR_DIALOG (English (U.S.))]
Type=1
Class=?
ControlCount=20
Control1=IDCANCEL,button,1342242817
Control2=IDC_BUTTON_RESEND,button,1342242817
Control3=IDC_BUTTON_RESEND_ALL,button,1342242817
Control4=IDC_STATIC,static,1342308352
Control5=IDC_STATIC,static,1342308352
Control6=IDC_STATIC,static,1342308352
Control7=IDC_STATIC,static,1342308352
Control8=IDC_EDIT_CARD_NUM,edit,1350566016
Control9=IDC_EDIT_TRAN_DATE,edit,1350631552
Control10=IDC_EDIT3,edit,1350631552
Control11=IDC_COMBO_NETWORKCONTROLLER,combobox,1344339971
Control12=IDC_STATIC_LBL,static,1342181388
Control13=IDC_STATIC,button,1342177287
Control14=IDC_LIST_RESEND,SysListView32,1350632457
Control15=IDC_SEARCH_BUTTON,button,1342242817
Control16=IDC_CHECK,button,1342242819
Control17=IDC_EDIT_COUNTER,edit,1350631552
Control18=IDC_STATIC,static,1342308352
Control19=IDC_BUTTON_HELP,button,1342242817
Control20=IDC_BUTTON_NEXT10,button,1342242817

[DLG:IDD_DIALOG_HELP (English (U.S.))]
Type=1
Class=?
ControlCount=10
Control1=IDOK,button,1342242817
Control2=IDCANCEL,button,1342242816
Control3=IDC_STATIC,static,1342308352
Control4=IDC_STATIC,static,1342308352
Control5=IDC_STATIC,static,1342308352
Control6=IDC_STATIC,static,1342308352
Control7=IDC_STATIC,static,1342308352
Control8=IDC_STATIC,static,1342308352
Control9=IDC_STATIC,static,1342308352
Control10=IDC_STATIC,static,1342308352

[DLG:IDD_IDLE_LOGIN (English (U.S.))]
Type=1
Class=?
ControlCount=6
Control1=IDC_EDIT_UID,edit,1350631560
Control2=IDC_EDIT_PWD,edit,1350631584
Control3=IDOK,button,1342242817
Control4=IDCANCEL,button,1342242816
Control5=IDC_STATIC_PWD,static,1342308352
Control6=IDC_STATIC_UID,static,1342308352

