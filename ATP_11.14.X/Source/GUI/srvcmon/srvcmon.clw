; CLW file contains information for the MFC ClassWizard

[General Info]
Version=1
LastClass=CSrvcmonView
LastTemplate=CDialog
NewFileInclude1=#include "stdafx.h"
NewFileInclude2=#include "srvcmon.h"
LastPage=0

ClassCount=7
Class1=CSysConfig
Class2=CMainFrame
Class3=CSrvcmonApp
Class4=CAboutDlg
Class5=CSrvcmonDoc
Class6=CSrvcmonView

ResourceCount=10
Resource1=IDD_ABOUTBOX
Resource2=IDD_SRVCMON_DIALOG
Resource3=IDR_MAINFRAME (English (U.S.))
Resource4=IDD_SYS_CONFIG
Class7=CIdleLogin
Resource5=IDR_MAINFRAME
Resource6=IDD_SRVCMON_DIALOG (English (U.S.))
Resource7=IDD_ABOUTBOX (English (U.S.))
Resource8=IDD_IDLE_LOGIN
Resource9=IDD_SYS_CONFIG (English (U.S.))
Resource10=IDD_IDLE_LOGIN (English (U.S.))

[CLS:CMainFrame]
Type=0
BaseClass=CFrameWnd
HeaderFile=Mainfrm.h
ImplementationFile=Mainfrm.cpp

[CLS:CSrvcmonApp]
Type=0
BaseClass=CWinApp
HeaderFile=srvcmon.h
ImplementationFile=srvcmon.cpp

[CLS:CAboutDlg]
Type=0
BaseClass=CDialog
HeaderFile=srvcmon.cpp
ImplementationFile=srvcmon.cpp
LastObject=IDC_STATIC_APPVERSION
Filter=D
VirtualFilter=dWC

[CLS:CSrvcmonDoc]
Type=0
BaseClass=CDocument
HeaderFile=srvcmonDoc.h
ImplementationFile=srvcmonDoc.cpp

[CLS:CSrvcmonView]
Type=0
BaseClass=CFormView
HeaderFile=srvcmonView.h
ImplementationFile=srvcmonView.cpp
Filter=D
VirtualFilter=VWC
LastObject=IDC_MRA_ON

[CLS:CSysConfig]
Type=0
BaseClass=CDialog
HeaderFile=SysConfig.h
ImplementationFile=SysConfig.cpp

[DLG:IDD_ABOUTBOX]
Type=1
Class=CAboutDlg
ControlCount=4
Control1=IDC_STATIC,static,1342177283
Control2=IDC_STATIC_APPVERSION,static,1342308480
Control3=IDC_STATIC,static,1342308352
Control4=IDOK,button,1342373889

[DLG:IDD_SRVCMON_DIALOG]
Type=1
Class=CSrvcmonView
ControlCount=16
Control1=IDC_SERVICE_LIST,listbox,1352728961
Control2=IDC_STATIC,static,1342308352
Control3=IDC_SERVICE_TAB,SysTabControl32,1342177280
Control4=IDC_START,button,1342242816
Control5=IDC_STOP,button,1342242816
Control6=IDC_STARTALL,button,1476460544
Control7=IDC_STOPALL,button,1476460544
Control8=IDC_ENABLE,button,1342242816
Control9=IDC_DISABLE,button,1342242816
Control10=IDC_TRACEON,button,1342242816
Control11=IDC_TRACEOFF,button,1342242816
Control12=IDC_PING,button,1342242816
Control13=IDC_PINGALL,button,1342242816
Control14=IDC_REFRESH,button,1342242816
Control15=IDC_CLOSE,button,1342242816
Control16=IDC_SYS_CONFIG,button,1476460544

[DLG:IDD_SYS_CONFIG]
Type=1
Class=CSysConfig
ControlCount=12
Control1=IDOK,button,1342242817
Control2=IDC_HIGH_AVAILABILITY_INDICATOR,static,1342308352
Control3=IDC_THIS_MACHINE_ID,static,1342308352
Control4=IDC_OTHER_MACHINE_ID,static,1342308352
Control5=IDC_THIS_XIPC_INSTANCE,static,1342308352
Control6=IDC_OTHER_XIPC_INSTANCE,static,1342308352
Control7=IDC_CURRENT_SETUP,button,1342308359
Control8=IDC_PRIMARY_SERVER,static,1342308352
Control9=IDC_SECONDARY_SERVER,static,1342308352
Control10=IDC_MAKE_THIS_SERVER_PRIMARY,button,1342242816
Control11=IDC_MAKE_OTHER_SERVER_PRIMARY,button,1342242816
Control12=IDC_SWITCH_PRIMARY_SECONDARY,button,1342242816

[MNU:IDR_MAINFRAME]
Type=1
Class=?
Command1=ID_FILE_NEW
Command2=ID_FILE_OPEN
Command3=ID_FILE_SAVE
Command4=ID_FILE_SAVE_AS
Command5=ID_FILE_MRU_FILE1
Command6=ID_APP_EXIT
Command7=ID_EDIT_UNDO
Command8=ID_EDIT_CUT
Command9=ID_EDIT_COPY
Command10=ID_EDIT_PASTE
Command11=ID_VIEW_STATUS_BAR
Command12=ID_APP_ABOUT
CommandCount=12

[ACL:IDR_MAINFRAME]
Type=1
Class=?
Command1=ID_FILE_NEW
Command2=ID_FILE_OPEN
Command3=ID_FILE_SAVE
Command4=ID_EDIT_UNDO
Command5=ID_EDIT_CUT
Command6=ID_EDIT_COPY
Command7=ID_EDIT_PASTE
Command8=ID_EDIT_UNDO
Command9=ID_EDIT_CUT
Command10=ID_EDIT_COPY
Command11=ID_EDIT_PASTE
Command12=ID_NEXT_PANE
Command13=ID_PREV_PANE
CommandCount=13

[CLS:CIdleLogin]
Type=0
HeaderFile=IdleLogin.h
ImplementationFile=IdleLogin.cpp
BaseClass=CDialog
Filter=D
LastObject=CIdleLogin
VirtualFilter=dWC

[DLG:IDD_IDLE_LOGIN]
Type=1
Class=CIdleLogin
ControlCount=6
Control1=IDC_EDIT_UID,edit,1350631560
Control2=IDC_EDIT_PWD,edit,1350631584
Control3=IDOK,button,1342242817
Control4=IDCANCEL,button,1342242816
Control5=IDC_STATIC_UID,static,1342308352
Control6=IDC_STATIC_PWD,static,1342308352

[MNU:IDR_MAINFRAME (English (U.S.))]
Type=1
Class=?
Command1=ID_FILE_NEW
Command2=ID_FILE_OPEN
Command3=ID_FILE_SAVE
Command4=ID_FILE_SAVE_AS
Command5=ID_FILE_MRU_FILE1
Command6=ID_APP_EXIT
Command7=ID_EDIT_UNDO
Command8=ID_EDIT_CUT
Command9=ID_EDIT_COPY
Command10=ID_EDIT_PASTE
Command11=ID_VIEW_STATUS_BAR
Command12=ID_APP_ABOUT
CommandCount=12

[ACL:IDR_MAINFRAME (English (U.S.))]
Type=1
Class=?
Command1=ID_FILE_NEW
Command2=ID_FILE_OPEN
Command3=ID_FILE_SAVE
Command4=ID_EDIT_UNDO
Command5=ID_EDIT_CUT
Command6=ID_EDIT_COPY
Command7=ID_EDIT_PASTE
Command8=ID_EDIT_UNDO
Command9=ID_EDIT_CUT
Command10=ID_EDIT_COPY
Command11=ID_EDIT_PASTE
Command12=ID_NEXT_PANE
Command13=ID_PREV_PANE
CommandCount=13

[DLG:IDD_ABOUTBOX (English (U.S.))]
Type=1
Class=?
ControlCount=4
Control1=IDC_STATIC,static,1342177283
Control2=IDC_STATIC_APPVERSION,static,1342308480
Control3=IDC_STATIC,static,1342308352
Control4=IDOK,button,1342373889

[DLG:IDD_SRVCMON_DIALOG (English (U.S.))]
Type=1
Class=CSrvcmonView
ControlCount=22
Control1=IDC_SERVICE_LIST,listbox,1352728961
Control2=IDC_STATIC,static,1342308352
Control3=IDC_SERVICE_TAB,SysTabControl32,1342177280
Control4=IDC_START,button,1342242816
Control5=IDC_STOP,button,1342242816
Control6=IDC_STARTALL,button,1476460544
Control7=IDC_STOPALL,button,1476460544
Control8=IDC_ENABLE,button,1342242816
Control9=IDC_DISABLE,button,1342242816
Control10=IDC_TRACEON,button,1342242816
Control11=IDC_TRACEOFF,button,1342242816
Control12=IDC_PING,button,1342242816
Control13=IDC_PINGALL,button,1342242816
Control14=IDC_REFRESH,button,1342242816
Control15=IDC_CLOSE,button,1342242816
Control16=IDC_SYS_CONFIG,button,1476460544
Control17=IDC_ENCRYPT_ON,button,1476460544
Control18=IDC_ENCRYPT_OFF,button,1476460544
Control19=IDC_MRA_ON,button,1476460544
Control20=IDC_MRA_OFF,button,1476460544
Control21=IDC_FALCON_ON,button,1476460544
Control22=IDC_FALCON_OFF,button,1476460544

[DLG:IDD_SYS_CONFIG (English (U.S.))]
Type=1
Class=?
ControlCount=12
Control1=IDOK,button,1342242817
Control2=IDC_HIGH_AVAILABILITY_INDICATOR,static,1342308352
Control3=IDC_THIS_MACHINE_ID,static,1342308352
Control4=IDC_OTHER_MACHINE_ID,static,1342308352
Control5=IDC_THIS_XIPC_INSTANCE,static,1342308352
Control6=IDC_OTHER_XIPC_INSTANCE,static,1342308352
Control7=IDC_CURRENT_SETUP,button,1342308359
Control8=IDC_PRIMARY_SERVER,static,1342308352
Control9=IDC_SECONDARY_SERVER,static,1342308352
Control10=IDC_MAKE_THIS_SERVER_PRIMARY,button,1342242816
Control11=IDC_MAKE_OTHER_SERVER_PRIMARY,button,1342242816
Control12=IDC_SWITCH_PRIMARY_SECONDARY,button,1342242816

[DLG:IDD_IDLE_LOGIN (English (U.S.))]
Type=1
Class=?
ControlCount=6
Control1=IDC_EDIT_UID,edit,1350631560
Control2=IDC_EDIT_PWD,edit,1350631584
Control3=IDOK,button,1342242817
Control4=IDCANCEL,button,1342242816
Control5=IDC_STATIC_UID,static,1342308352
Control6=IDC_STATIC_PWD,static,1342308352

