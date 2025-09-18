; CLW file contains information for the MFC ClassWizard

[General Info]
Version=1
LastClass=CVoiceAuth
LastTemplate=CDialog
NewFileInclude1=#include "stdafx.h"
NewFileInclude2=#include "voiceauthorization.h"
LastPage=0

ClassCount=10
Class1=CIdleLoginfrm
Class2=CMainFrame
Class3=CMyUtils
Class4=COverridePassword
Class5=CVoiceAuth
Class6=CVoiceAuthorizationApp
Class7=CAboutDlg
Class8=CVoiceAuthorizationDoc
Class9=CVoiceAuthorizationIO
Class10=CVoiceAuthorizationView

ResourceCount=5
Resource1=IDR_MAINFRAME (English (U.S.))
Resource2=IDD_OVERRIDE_PASSWORD (English (U.S.))
Resource3=IDD_ABOUTBOX (English (U.S.))
Resource4=IDD_VOICE_AUTH (English (U.S.))
Resource5=IDD_IDLE_LOGIN (English (U.S.))

[CLS:CIdleLoginfrm]
Type=0
BaseClass=CDialog
HeaderFile=IdleLoginFrm.h
ImplementationFile=IdleLoginFrm.cpp
Filter=D
VirtualFilter=dWC
LastObject=CIdleLoginfrm

[CLS:CMainFrame]
Type=0
BaseClass=CFrameWnd
HeaderFile=MainFrm.h
ImplementationFile=MainFrm.cpp

[CLS:CMyUtils]
Type=0
BaseClass=CWnd
HeaderFile=MYUTILS.H
ImplementationFile=MYUTILS.CPP

[CLS:COverridePassword]
Type=0
BaseClass=CDialog
HeaderFile=OverridePassword.h
ImplementationFile=OverridePassword.cpp

[CLS:CVoiceAuth]
Type=0
BaseClass=CDialog
HeaderFile=VoiceAuth.h
ImplementationFile=VoiceAuth.cpp

[CLS:CVoiceAuthorizationApp]
Type=0
BaseClass=CWinApp
HeaderFile=VoiceAuthorization.h
ImplementationFile=VoiceAuthorization.cpp

[CLS:CAboutDlg]
Type=0
BaseClass=CDialog
HeaderFile=VoiceAuthorization.cpp
ImplementationFile=VoiceAuthorization.cpp
LastObject=CAboutDlg

[CLS:CVoiceAuthorizationDoc]
Type=0
BaseClass=CDocument
HeaderFile=VoiceAuthorizationDoc.h
ImplementationFile=VoiceAuthorizationDoc.cpp

[CLS:CVoiceAuthorizationIO]
Type=0
BaseClass=CDialog
HeaderFile=VoiceAuthorizationIO.h
ImplementationFile=VoiceAuthorizationIO.cpp

[CLS:CVoiceAuthorizationView]
Type=0
BaseClass=CView
HeaderFile=VoiceAuthorizationView.h
ImplementationFile=VoiceAuthorizationView.cpp

[DLG:IDD_IDLE_LOGIN]
Type=1
Class=CIdleLoginfrm

[DLG:IDD_OVERRIDE_PASSWORD]
Type=1
Class=COverridePassword

[DLG:IDD_VOICE_AUTH]
Type=1
Class=CVoiceAuth

[DLG:IDD_VOICE_AUTH_INPUT]
Type=1
Class=CVoiceAuthInput

[DLG:IDD_ABOUTBOX]
Type=1
Class=CAboutDlg

[DLG:IDD_VOICE]
Type=1
Class=CVoiceAuthorizationIO

[TB:IDR_MAINFRAME (English (U.S.))]
Type=1
Class=?
Command1=ID_FILE_NEW
Command2=ID_FILE_OPEN
Command3=ID_FILE_SAVE
Command4=ID_EDIT_CUT
Command5=ID_EDIT_COPY
Command6=ID_EDIT_PASTE
Command7=ID_FILE_PRINT
Command8=ID_APP_ABOUT
CommandCount=8

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
Command11=ID_VIEW_TOOLBAR
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
Control2=IDC_STATIC_APPVERION,static,1342308480
Control3=IDC_STATIC,static,1342308352
Control4=IDOK,button,1342373889

[DLG:IDD_VOICE_AUTH (English (U.S.))]
Type=1
Class=CVoiceAuth
ControlCount=55
Control1=IDC_MERCHANT_ID_FLD,edit,1350631560
Control2=IDC_ORG_ID_FLD,edit,1350631560
Control3=IDC_ACCOUNT_NUMBER_FLD,edit,1350639744
Control4=IDC_EXPIRY_DATE_FLD,edit,1350639744
Control5=IDC_MOD10_CHECK_BUTTON,button,1342242819
Control6=IDC_AMOUNT,edit,1350631552
Control7=IDC_CVV2_FLD,edit,1350639744
Control8=IDC_TRANSACTION_TYPE,combobox,1344339971
Control9=IDC_TEXT_FLD,edit,1352728644
Control10=IDC_AUTH_NUMBER_FLD,edit,1484849280
Control11=IDC_DEFERRED_PURCHASE_TERM_FLD,edit,1484857472
Control12=IDC_VERIFY_BUTTON,button,1342242816
Control13=IDC_PROCESS_AUTH_BUTTON,button,1342242816
Control14=IDC_OVERRIDE_DENIAL_BUTTON,button,1342242816
Control15=IDC_CLEAR_BUTTON,button,1342242816
Control16=IDC_OK1,button,1342242816
Control17=IDC_MCC_FLD,edit,1350568064
Control18=IDC_INDUSTRY_CODE,edit,1350568064
Control19=IDC_CURRENCY_CODE,edit,1350567936
Control20=IDC_STATIC,static,1342308352
Control21=IDC_STATIC,static,1342308352
Control22=IDC_STATIC,static,1342308352
Control23=IDC_STATIC,static,1342308352
Control24=IDC_STATIC,static,1342308352
Control25=IDC_STATIC,static,1342308352
Control26=IDC_STATIC,static,1342308352
Control27=IDC_STATIC,static,1342308352
Control28=IDC_STATIC,static,1342308352
Control29=IDC_STATIC,static,1342308352
Control30=IDC_STATIC,static,1342308352
Control31=IDC_STATIC,static,1342308352
Control32=IDC_STATIC,static,1342308352
Control33=IDC_STATIC,static,1342308352
Control34=IDC_STATIC,static,1342308352
Control35=IDC_STATIC,static,1342308352
Control36=IDC_CARDHOLDER_NAME_FLD,edit,1350568064
Control37=IDC_BANK_NAME_FLD,edit,1350568064
Control38=IDC_MERCHANT_NAME_FLD,edit,1350568064
Control39=IDC_STATIC,button,1342177287
Control40=IDC_STATIC,static,1342308352
Control41=IDC_STATIC,static,1342308352
Control42=IDC_STATIC,button,1342177287
Control43=IDC_RESPONSE_CODE_FLD,edit,1350568064
Control44=IDC_AUTH_NUM_FLD,edit,1350568064
Control45=IDC_STATIC,static,1342308352
Control46=IDC_STATIC,static,1342308352
Control47=IDC_MONTHLY_AMORTIZATION_AMOUNT_FLD,edit,1350568064
Control48=IDC_STATIC,button,1342177287
Control49=IDC_RESPONSE_TEXT_FLD,edit,1350568004
Control50=IDC_GROSS_AMOUNT_FLD,edit,1350568064
Control51=IDC_STATIC_INPUT,button,1342177287
Control52=IDC_STATIC,button,1342177287
Control53=IDC_MESSAGE_LBL,static,1342312448
Control54=IDC_STATIC,static,1342308352
Control55=IDC_CARD_STATUS_FLD,edit,1350568064

[DLG:IDD_OVERRIDE_PASSWORD (English (U.S.))]
Type=1
Class=?
ControlCount=5
Control1=IDC_OVERRIDE_PASSWORD,edit,1350631592
Control2=IDOK,button,1342242817
Control3=IDCANCEL,button,1342242816
Control4=IDC_STATIC,static,1342308876
Control5=IDC_STATIC,button,1342177287

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

