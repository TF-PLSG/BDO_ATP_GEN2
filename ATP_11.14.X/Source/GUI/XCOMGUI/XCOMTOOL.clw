; CLW file contains information for the MFC ClassWizard

[General Info]
Version=1
LastClass=CXCOMTOOLDlg
LastTemplate=CWinThread
NewFileInclude1=#include "stdafx.h"
NewFileInclude2=#include "XCOMTOOL.h"

ClassCount=6
Class1=CXCOMTOOLApp
Class2=CXCOMTOOLDlg

ResourceCount=9
Resource2=IDD_XCOMTOOL_DIALOG
Resource3=IDD_DIALOG2
Resource1=IDR_MAINFRAME
Class3=CRefreshWnd
Class4=CCreatewnd
Resource4=IDD_XCOMTOOL_DIALOG (English (U.S.))
Class5=CDeleteWnd
Class6=CUIThread
Resource5=IDD_DIALOG1
Resource6=IDD_DIALOG3 (English (U.S.))
Resource7=IDD_DIALOG1 (English (U.S.))
Resource8=IDD_DIALOG2 (English (U.S.))
Resource9=IDD_DIALOG3

[CLS:CXCOMTOOLApp]
Type=0
HeaderFile=XCOMTOOL.h
ImplementationFile=XCOMTOOL.cpp
Filter=N

[CLS:CXCOMTOOLDlg]
Type=0
HeaderFile=XCOMTOOLDlg.h
ImplementationFile=XCOMTOOLDlg.cpp
Filter=D
BaseClass=CDialog
VirtualFilter=dWC
LastObject=CXCOMTOOLDlg



[DLG:IDD_XCOMTOOL_DIALOG]
Type=1
Class=CXCOMTOOLDlg
ControlCount=26
Control1=IDC_STATIC_APP_NAME,static,1342308865
Control2=IDC_STATIC_DATE_TIME,static,1342308352
Control3=IDC_STATIC,static,1342308865
Control4=IDC_STATIC_USER_SUMMARY,static,1342308352
Control5=IDC_STATIC_QUE_SUMMARY,static,1342308352
Control6=IDC_STATIC_MEM_SUMMARY,static,1342308352
Control7=IDC_USERINFO_HEADER,static,1342308352
Control8=IDC_QUESYS_HEADER,static,1342308352
Control9=IDC_STATIC_MEMSYS_HEADER,static,1342308352
Control10=IDC_LIST1,listbox,1352728963
Control11=IDC_LIST2,listbox,1352728963
Control12=IDC_LIST3,listbox,1352728963
Control13=IDC_BTN_CONFIG,button,1342242817
Control14=IDC_BTN_CREATE,button,1342242817
Control15=IDC_BTN_DELETE,button,1342242817
Control16=IDC_BTN_READ,button,1342242817
Control17=IDC_BTN_WRITE,button,1342242817
Control18=IDC_BTN_RETRY,button,1342242817
Control19=IDC_BTN_FLUSH,button,1342242817
Control20=IDC_BTN_STOP,button,1342242817
Control21=IDC_USERINFO,static,1342308352
Control22=IDC_QUESYS,static,1342308352
Control23=IDC_STATIC_MEMSYS,static,1342308352
Control24=IDC_EDIT_SEARCH,edit,1350631552
Control25=IDC_BUTTON_SEARCH,button,1342242817
Control26=IDC_BUTTON_SEARCH_NEXT,button,1342242817

[DLG:IDD_DIALOG1]
Type=1
Class=CRefreshWnd
ControlCount=5
Control1=IDOK,button,1342242817
Control2=IDCANCEL,button,1342242816
Control3=IDC_REFRESH_WND,static,1342308352
Control4=IDC_STATIC,static,1342308352
Control5=IDC_EDIT1,edit,1350631552

[DLG:IDD_DIALOG2]
Type=1
Class=CCreatewnd
ControlCount=9
Control1=IDOK,button,1342242817
Control2=IDCANCEL,button,1342242816
Control3=IDC_CREATE_WND,static,1342308352
Control4=IDC_STATIC_1,static,1342308352
Control5=IDC_STATIC_2,static,1342308352
Control6=IDC_STATIC_3,static,1342308352
Control7=IDC_EDIT1,edit,1350631552
Control8=IDC_EDIT2,edit,1350639744
Control9=IDC_EDIT3,edit,1350639744

[CLS:CRefreshWnd]
Type=0
HeaderFile=RefreshWnd.h
ImplementationFile=RefreshWnd.cpp
BaseClass=CDialog
Filter=D
VirtualFilter=dWC
LastObject=CRefreshWnd

[CLS:CCreatewnd]
Type=0
HeaderFile=Createwnd.h
ImplementationFile=Createwnd.cpp
BaseClass=CDialog
Filter=D
VirtualFilter=dWC
LastObject=CCreatewnd

[DLG:IDD_DIALOG3]
Type=1
Class=CDeleteWnd
ControlCount=4
Control1=IDOK,button,1342242817
Control2=IDCANCEL,button,1342242816
Control3=IDC_STATIC,static,1342308352
Control4=IDC_STATIC,static,1342308352

[CLS:CDeleteWnd]
Type=0
HeaderFile=DeleteWnd.h
ImplementationFile=DeleteWnd.cpp
BaseClass=CDialog
Filter=D
VirtualFilter=dWC
LastObject=IDOK

[CLS:CUIThread]
Type=0
HeaderFile=UIThread.h
ImplementationFile=UIThread.cpp
BaseClass=CWinThread
Filter=N
LastObject=CUIThread
VirtualFilter=TC

[DLG:IDD_XCOMTOOL_DIALOG (English (U.S.))]
Type=1
Class=?
ControlCount=26
Control1=IDC_STATIC_APP_NAME,static,1342308865
Control2=IDC_STATIC_DATE_TIME,static,1342308352
Control3=IDC_STATIC,static,1342308865
Control4=IDC_STATIC_USER_SUMMARY,static,1342308352
Control5=IDC_STATIC_QUE_SUMMARY,static,1342308352
Control6=IDC_STATIC_MEM_SUMMARY,static,1342308352
Control7=IDC_USERINFO_HEADER,static,1342308352
Control8=IDC_QUESYS_HEADER,static,1342308352
Control9=IDC_STATIC_MEMSYS_HEADER,static,1342308352
Control10=IDC_LIST1,listbox,1352728963
Control11=IDC_LIST2,listbox,1352728963
Control12=IDC_LIST3,listbox,1352728963
Control13=IDC_BTN_CONFIG,button,1342242817
Control14=IDC_BTN_CREATE,button,1342242817
Control15=IDC_BTN_DELETE,button,1342242817
Control16=IDC_BTN_READ,button,1342242817
Control17=IDC_BTN_WRITE,button,1342242817
Control18=IDC_BTN_RETRY,button,1342242817
Control19=IDC_BTN_FLUSH,button,1342242817
Control20=IDC_BTN_STOP,button,1342242817
Control21=IDC_USERINFO,static,1342308352
Control22=IDC_QUESYS,static,1342308352
Control23=IDC_STATIC_MEMSYS,static,1342308352
Control24=IDC_EDIT_SEARCH,edit,1350631552
Control25=IDC_BUTTON_SEARCH,button,1342242817
Control26=IDC_BUTTON_SEARCH_NEXT,button,1342242817

[DLG:IDD_DIALOG1 (English (U.S.))]
Type=1
Class=?
ControlCount=5
Control1=IDOK,button,1342242817
Control2=IDCANCEL,button,1342242816
Control3=IDC_REFRESH_WND,static,1342308352
Control4=IDC_STATIC,static,1342308352
Control5=IDC_EDIT1,edit,1350631552

[DLG:IDD_DIALOG2 (English (U.S.))]
Type=1
Class=?
ControlCount=9
Control1=IDOK,button,1342242817
Control2=IDCANCEL,button,1342242816
Control3=IDC_CREATE_WND,static,1342308352
Control4=IDC_STATIC_1,static,1342308352
Control5=IDC_STATIC_2,static,1342308352
Control6=IDC_STATIC_3,static,1342308352
Control7=IDC_EDIT1,edit,1350631552
Control8=IDC_EDIT2,edit,1350639744
Control9=IDC_EDIT3,edit,1350639744

[DLG:IDD_DIALOG3 (English (U.S.))]
Type=1
Class=?
ControlCount=4
Control1=IDOK,button,1342242817
Control2=IDCANCEL,button,1342242816
Control3=IDC_STATIC,static,1342308352
Control4=IDC_STATIC,static,1342308352

