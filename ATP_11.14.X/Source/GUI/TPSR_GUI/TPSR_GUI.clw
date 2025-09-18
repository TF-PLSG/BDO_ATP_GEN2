; CLW file contains information for the MFC ClassWizard

[General Info]
Version=1
LastClass=CCardAndTrackDataDlg
LastTemplate=CPropertyPage
NewFileInclude1=#include "stdafx.h"
NewFileInclude2=#include "tpsr_gui.h"
LastPage=0

ClassCount=14
Class1=CCardMerchantDevDlg
Class2=CPosConfig
Class3=CProcessDialog
Class4=CStatusSplashWindow
Class5=CTestProfile
Class6=CToolTipButton
Class7=CTpsGraph
Class8=CTPSR_GUIApp
Class9=CAboutDlg
Class10=CTPSR_GUIDlg
Class11=CTPSRMainDlg
Class12=CTPSRMAINSHEET

ResourceCount=12
Resource1=IDD_TPSR_MAIN
Resource2=IDD_CARD_DEV_MERCHANT (English (U.S.))
Resource3=IDD_TPS_GRAPH (English (U.S.))
Resource4=IDD_SPLASH (English (U.S.))
Resource5=IDD_POS_CONFIG
Resource6=IDD_STATISTIC_DIALOG
Resource7=IDD_TPSR_GUI_DIALOG (English (U.S.))
Resource8=IDD_ABOUTBOX (English (U.S.))
Resource9=IDD_DIALOG1
Resource10=IDD_TEST_PROFILE (English (U.S.))
Class13=CStatisticsDlg
Resource11=IDR_MENU1
Class14=CCardAndTrackDataDlg
Resource12=IDD_CARD_TRACK_DATA

[CLS:CCardMerchantDevDlg]
Type=0
BaseClass=CPropertyPage
HeaderFile=CardMerchantDevDlg.h
ImplementationFile=CardMerchantDevDlg.cpp
Filter=D
VirtualFilter=idWC
LastObject=IDC_CARD_NBR_LIST_FILE

[CLS:CPosConfig]
Type=0
BaseClass=CPropertyPage
HeaderFile=PosConfig.h
ImplementationFile=PosConfig.cpp
LastObject=CPosConfig
Filter=D
VirtualFilter=idWC

[CLS:CProcessDialog]
Type=0
BaseClass=CWinThread
HeaderFile=ProcessDialog.h
ImplementationFile=ProcessDialog.cpp
LastObject=CProcessDialog

[CLS:CStatusSplashWindow]
Type=0
BaseClass=CDialog
HeaderFile=StatusSplashWindow.h
ImplementationFile=StatusSplashWindow.cpp
Filter=D
VirtualFilter=dWC
LastObject=CStatusSplashWindow

[CLS:CTestProfile]
Type=0
BaseClass=CPropertyPage
HeaderFile=TestProfile.h
ImplementationFile=TestProfile.cpp
Filter=D
VirtualFilter=idWC
LastObject=CTestProfile

[CLS:CToolTipButton]
Type=0
BaseClass=baseCToolTipButton
HeaderFile=ToolTipButton.h
ImplementationFile=ToolTipButton.cpp
LastObject=CToolTipButton

[CLS:CTpsGraph]
Type=0
BaseClass=CDialog
HeaderFile=TpsGraph.h
ImplementationFile=TpsGraph.cpp
Filter=D
VirtualFilter=dWC
LastObject=CTpsGraph

[CLS:CTPSR_GUIApp]
Type=0
BaseClass=CWinApp
HeaderFile=TPSR_GUI.h
ImplementationFile=TPSR_GUI.cpp
LastObject=CTPSR_GUIApp

[CLS:CAboutDlg]
Type=0
BaseClass=CDialog
HeaderFile=TPSR_GUIDlg.cpp
ImplementationFile=TPSR_GUIDlg.cpp
LastObject=CAboutDlg

[CLS:CTPSR_GUIDlg]
Type=0
BaseClass=CPropertyPage
HeaderFile=TPSR_GUIDlg.h
ImplementationFile=TPSR_GUIDlg.cpp
Filter=D
VirtualFilter=idWC
LastObject=CTPSR_GUIDlg

[CLS:CTPSRMainDlg]
Type=0
BaseClass=CPropertyPage
HeaderFile=TPSRMainDlg.h
ImplementationFile=TPSRMainDlg.cpp
LastObject=CTPSRMainDlg
Filter=D
VirtualFilter=idWC

[CLS:CTPSRMAINSHEET]
Type=0
BaseClass=CPropertySheet
HeaderFile=TPSRMAINSheet.h
ImplementationFile=TPSRMAINSheet.cpp
LastObject=CTPSRMAINSHEET
Filter=W
VirtualFilter=hWC

[DLG:IDD_CARD_DEV_MERCHANT]
Type=1
Class=CCardMerchantDevDlg

[DLG:IDD_POS_CONFIG]
Type=1
Class=CPosConfig
ControlCount=11
Control1=IDC_EDIT2,edit,1350631552
Control2=IDC_STATIC,static,1342308352
Control3=IDC_EDIT3,edit,1350631552
Control4=IDC_STATIC,static,1342308352
Control5=IDC_EDIT4,edit,1350631552
Control6=IDC_STATIC,static,1342308352
Control7=IDC_COMBO1,combobox,1344340227
Control8=IDC_EDIT5,edit,1350631552
Control9=IDC_STATIC,static,1342308352
Control10=IDC_EDIT6,edit,1350631552
Control11=IDC_STATIC,static,1342308352

[DLG:IDD_SPLASH]
Type=1
Class=CStatusSplashWindow

[DLG:IDD_TEST_PROFILE]
Type=1
Class=CTestProfile

[DLG:IDD_TPS_GRAPH]
Type=1
Class=CTpsGraph

[DLG:IDD_ABOUTBOX]
Type=1
Class=CAboutDlg

[DLG:IDD_TPSR_GUI_DIALOG]
Type=1
Class=CTPSR_GUIDlg

[DLG:IDD_TPSR_MAIN]
Type=1
Class=CTPSRMainDlg
ControlCount=10
Control1=IDRUN,button,1342242817
Control2=IDCANCEL,button,1476460544
Control3=IDC_STATIC_SENT,static,1342308352
Control4=IDC_STATIC_RECV,static,1342308352
Control5=IDC_TXNSENT,edit,1350631552
Control6=IDC_RESRECV,edit,1350631552
Control7=IDRESETCOUNTER,button,1342242817
Control8=IDC_TOOL_STATUS,static,1342308864
Control9=IDC_EXIT,button,1073807360
Control10=IDC_SHOW_CONSOLE,button,1073807360

[DLG:IDD_ABOUTBOX (English (U.S.))]
Type=1
Class=?
ControlCount=4
Control1=IDC_STATIC,static,1342177283
Control2=IDC_STATIC,static,1342308480
Control3=IDC_STATIC,static,1342308352
Control4=IDOK,button,1342373889

[DLG:IDD_TPSR_GUI_DIALOG (English (U.S.))]
Type=1
Class=CTPSR_GUIDlg
ControlCount=41
Control1=IDC_PROFILE_NAME,edit,1350631560
Control2=IDC_MSG_TYPE,combobox,1344340995
Control3=IDC_BITMAP,edit,1350641800
Control4=IDC_PROCESSING_CODE,edit,1350639752
Control5=IDC_AMOUNT,edit,1350639752
Control6=IDC_SYS_TRACE_NBR,edit,1350641800
Control7=IDC_POS_ENTRY_MODE,edit,1350639752
Control8=IDC_NII,edit,1350639752
Control9=IDC_POS_COND_CODE,edit,1350639752
Control10=IDC_RETRIVAL_REF_NBR_CHECK,button,1342242819
Control11=IDC_RETRIVAL_REF_NBR,edit,1484857480
Control12=IDC_AUTH_ID_RESP_CHECK,button,1342242819
Control13=IDC_AUTH_ID_RESP,edit,1484857480
Control14=IDC_RESP_CODE_CHECK,button,1342242819
Control15=IDC_RESP_CODE,edit,1484857480
Control16=IDC_PIN_DATA_CHECK,button,1342242819
Control17=IDC_PIN_DATA,edit,1484857480
Control18=IDOK,button,1342242945
Control19=IDC_STATIC,static,1342308352
Control20=IDC_STATIC,static,1342308352
Control21=IDC_STATIC,static,1342308352
Control22=IDC_STATIC,static,1342308352
Control23=IDC_STATIC,static,1342308352
Control24=IDC_STATIC,static,1342308352
Control25=IDC_STATIC,static,1342308352
Control26=IDC_STATIC,static,1342308352
Control27=IDC_STATIC,static,1342308352
Control28=IDC_RETRIVAL_REF_STATIC,static,1476526080
Control29=IDC_AUTH_ID_RESP_STATIC,static,1476526080
Control30=IDC_RESP_CODE_STATIC,static,1476526080
Control31=IDC_PIN_DATA_STATIC,static,1476526080
Control32=IDRESET,button,1342242944
Control33=IDC_STATIC,static,1342308352
Control34=IDC_TIME_LOCAL_CHECK,button,1342242819
Control35=IDC_TIME_LOCAL,edit,1484857480
Control36=IDC_DATE_LOCAL_CHECK,button,1342242819
Control37=IDC_DATE_LOCAL,edit,1484857480
Control38=IDC_TIME_LOCAL_STATIC,static,1476526080
Control39=IDC_DATE_LOCAL_STATIC,static,1476526080
Control40=IDC_STATIC,static,1476526080
Control41=IDC_STATIC,static,1476526080

[DLG:IDD_TEST_PROFILE (English (U.S.))]
Type=1
Class=CTestProfile
ControlCount=7
Control1=IDADD,button,1342242817
Control2=IDC_STATIC,static,1342308352
Control3=IDC_TXNS,combobox,1344340226
Control4=IDC_STATIC,static,1342308352
Control5=IDC_NO_OF_RUNS,edit,1350631552
Control6=IDC_TESTLIST,listbox,1352729347
Control7=IDREMOVE,button,1342242816

[DLG:IDD_SPLASH (English (U.S.))]
Type=1
Class=CStatusSplashWindow
ControlCount=1
Control1=IDC_STATUS,static,1342308864

[DLG:IDD_CARD_DEV_MERCHANT (English (U.S.))]
Type=1
Class=CCardMerchantDevDlg
ControlCount=6
Control1=IDC_CARD_NBR_LIST_FILE,edit,1350631552
Control2=IDC_STATIC,static,1342308352
Control3=IDC_BROWSE_CARD_FILE,button,1342242816
Control4=IDC_DEVICE_NBR_FILE_LIST,edit,1350631552
Control5=IDC_STATIC,static,1342308354
Control6=IDC_BROWSE_DEVICE_FILE,button,1342242816

[DLG:IDD_TPS_GRAPH (English (U.S.))]
Type=1
Class=CTpsGraph
ControlCount=2
Control1=IDOK,button,1342242817
Control2=IDCANCEL,button,1342242816

[MNU:IDR_MENU1]
Type=1
Class=?
Command1=ID_FILE_CLOSE
Command2=ID_MENU_POS_CONFIG
CommandCount=2

[DLG:IDD_DIALOG1]
Type=1
Class=?
ControlCount=64
Control1=IDC_PROFILE_NAME,edit,1350631560
Control2=IDC_MSG_TYPE,combobox,1344340995
Control3=IDC_BITMAP,edit,1350641800
Control4=IDC_PROCESSING_CODE,edit,1350639752
Control5=IDC_AMOUNT,edit,1350639752
Control6=IDC_SYS_TRACE_NBR,edit,1350639752
Control7=IDC_TIME_LOCAL_CHECK,button,1342242819
Control8=IDC_TIME_LOCAL,edit,1484857480
Control9=IDC_DATE_LOCAL_CHECK,button,1342242819
Control10=IDC_DATE_LOCAL,edit,1484857480
Control11=IDC_EXP_DATE,edit,1350639752
Control12=IDC_POS_ENTRY_MODE,edit,1350639752
Control13=IDC_NII,edit,1350639752
Control14=IDC_POS_COND_CODE,edit,1350639752
Control15=IDC_RETRIVAL_REF_NBR_CHECK,button,1342242819
Control16=IDC_RETRIVAL_REF_NBR,edit,1484857480
Control17=IDC_AUTH_ID_RESP_CHECK,button,1342242819
Control18=IDC_AUTH_ID_RESP,edit,1484857480
Control19=IDC_RESP_CODE_CHECK,button,1342242819
Control20=IDC_RESP_CODE,edit,1484857480
Control21=IDC_TRACK1_DATA_CHECK,button,1342242819
Control22=IDC_TRACK1_DATA,edit,1484857480
Control23=IDC_TRACK2_DATA_CHECK,button,1342242819
Control24=IDC_TRACK2_DATA,edit,1484857480
Control25=IDC_PIN_DATA_CHECK,button,1342242819
Control26=IDC_PIN_DATA,edit,1484857480
Control27=IDC_EMV_DATA_CHECK,button,1342242819
Control28=IDC_EMV_DATA,edit,1484857480
Control29=IDC_PRIVATE1_CHECK,button,1342242819
Control30=IDC_PRIVATE1,edit,1484857480
Control31=IDC_PRIVATE2_CHECK,button,1342242819
Control32=IDC_PRIVATE2,edit,1484857480
Control33=IDC_PRIVATE3_CHECK,button,1342242819
Control34=IDC_PRIVATE3,edit,1484857480
Control35=IDOK,button,1342242945
Control36=IDC_STATIC,static,1342308352
Control37=IDC_STATIC,static,1342308352
Control38=IDC_STATIC,static,1342308352
Control39=IDC_STATIC,static,1342308352
Control40=IDC_STATIC,static,1342308352
Control41=IDC_STATIC,static,1342308352
Control42=IDC_STATIC,static,1342308352
Control43=IDC_TIME_LOCAL_STATIC,static,1476526080
Control44=IDC_DATE_LOCAL_STATIC,static,1476526080
Control45=IDC_STATIC,static,1342308352
Control46=IDC_STATIC,static,1342308352
Control47=IDC_STATIC,static,1342308352
Control48=IDC_STATIC,static,1342308352
Control49=IDC_RETRIVAL_REF_STATIC,static,1476526080
Control50=IDC_AUTH_ID_RESP_STATIC,static,1476526080
Control51=IDC_RESP_CODE_STATIC,static,1476526080
Control52=IDC_TRACK1_STATIC,static,1476526080
Control53=IDC_TRACK2_STATIC,static,1476526080
Control54=IDC_PIN_DATA_STATIC,static,1476526080
Control55=IDC_EMV_STATIC,static,1476526080
Control56=IDC_PRIVATE1_STATIC,static,1476526080
Control57=IDC_PRIVATE2_STATIC,static,1476526080
Control58=IDC_PRIVATE3_STATIC,static,1476526080
Control59=IDC_STATIC,static,1476526080
Control60=IDC_STATIC,static,1476526080
Control61=IDC_STATIC,static,1476526080
Control62=IDRESET,button,1342242944
Control63=IDC_CARD_TYPE1,combobox,1344339971
Control64=IDC_STATIC,static,1342308352

[DLG:IDD_STATISTIC_DIALOG]
Type=1
Class=CStatisticsDlg
ControlCount=5
Control1=IDOK,button,1342242817
Control2=IDC_STATIC,static,1342308352
Control3=IDC_STATIC,static,1342308352
Control4=IDC_STATIC_TOTAL_SENT,static,1342308352
Control5=IDC_STATIC_TOTAL_RECV,static,1342308352

[CLS:CStatisticsDlg]
Type=0
HeaderFile=StatisticsDlg.h
ImplementationFile=StatisticsDlg.cpp
BaseClass=CDialog
Filter=D
VirtualFilter=dWC
LastObject=CStatisticsDlg

[DLG:IDD_CARD_TRACK_DATA]
Type=1
Class=CCardAndTrackDataDlg
ControlCount=4
Control1=IDOK,button,1342242817
Control2=IDCANCEL,button,1342242816
Control3=IDC_CARD_NUMBERS,RICHEDIT,1350643908
Control4=IDC_TRACK_DATA,RICHEDIT,1350635716

[CLS:CCardAndTrackDataDlg]
Type=0
HeaderFile=CardAndTrackDataDlg.h
ImplementationFile=CardAndTrackDataDlg.cpp
BaseClass=CPropertyPage
Filter=D
LastObject=IDOK
VirtualFilter=idWC

