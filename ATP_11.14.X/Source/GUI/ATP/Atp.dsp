# Microsoft Developer Studio Project File - Name="Atp" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=Atp - Win32 Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "Atp.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Atp.mak" CFG="Atp - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Atp - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "Atp - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Atp - Win32 Release"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir ".\Release"
# PROP BASE Intermediate_Dir ".\Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir ".\Release"
# PROP Intermediate_Dir ".\Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /c
# ADD CPP /nologo /MD /W3 /GX /I "..\include" /I "..\..\..\Source\Libraries\include" /I "..\..\Libraries\Application_Win95\include" /I "..\..\Third_Party_Libraries\Win95\xipc" /I "..\..\..\Source\Server_AIX\include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Fr /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 version.lib ptetcp.lib Atptcp.lib pte.lib mutex.lib shrmem.lib ptepci.lib /nologo /subsystem:windows /machine:I386 /nodefaultlib:"libcmtd.lib" /nodefaultlib:"nafxcwd" /nodefaultlib:"nafxcw" /nodefaultlib:"libcmt" /nodefaultlib:"libcmtd" /nodefaultlib:"libcd" /nodefaultlib:"libc" /libpath:"..\lib" /libpath:"..\..\Build_GUI\lib\Release"
# SUBTRACT LINK32 /pdb:none
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=copy Release\Atp.exe ..\..\build_gui\bin\Release
# End Special Build Tool

!ELSEIF  "$(CFG)" == "Atp - Win32 Debug"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir ".\Debug"
# PROP BASE Intermediate_Dir ".\Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir ".\Debug"
# PROP Intermediate_Dir ".\Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /c
# ADD CPP /nologo /MDd /W3 /GX /ZI /I "..\include" /I "..\..\Libraries\include" /I "..\..\..\Source\Libraries\Application_Win95\include" /I "..\..\Third_Party_Libraries\Win95\xipc" /I "..\..\..\Source\Server_AIX\include" /I "..\..\Libraries\Application_Win95\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386
# ADD LINK32 Version.lib ptetcp.lib Atptcp.lib pte.lib mutex.lib shrmem.lib ptepci.lib version.lib /nologo /subsystem:windows /map /debug /machine:I386 /nodefaultlib:"nafxcw" /nodefaultlib:"libcmt" /nodefaultlib:"libcmtd" /nodefaultlib:"libcd" /nodefaultlib:"libc" /nodefaultlib:"nafxcwd" /libpath:"..\lib" /libpath:"..\..\Build_GUI\lib\debug" /libpath:"..\Server_aix\libequit\debug"
# SUBTRACT LINK32 /pdb:none
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=copy Debug\Atp.exe ..\..\build_gui\bin\Debug
# End Special Build Tool

!ENDIF 

# Begin Target

# Name "Atp - Win32 Release"
# Name "Atp - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat;for;f90"
# Begin Source File

SOURCE=.\AlphaLookup.cpp
# End Source File
# Begin Source File

SOURCE=.\AMEXUPDATES1.cpp
# End Source File
# Begin Source File

SOURCE=.\AppDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\ArchiveDBPasswords.cpp
# End Source File
# Begin Source File

SOURCE=.\Atp.cpp
# End Source File
# Begin Source File

SOURCE=.\hlp\Atp.hpj
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\Atp.rc
# End Source File
# Begin Source File

SOURCE=.\AtpDoc.cpp
# End Source File
# Begin Source File

SOURCE=.\ATPTimedDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\AtpView.cpp
# End Source File
# Begin Source File

SOURCE=.\BatchDetails.cpp
# End Source File
# Begin Source File

SOURCE=.\BinBlocking.cpp
# End Source File
# Begin Source File

SOURCE=.\BinDefinition.cpp
# End Source File
# Begin Source File

SOURCE=.\BinGeneral.cpp
# End Source File
# Begin Source File

SOURCE=.\BinList.cpp
# End Source File
# Begin Source File

SOURCE=.\BinMccBlocking.cpp
# End Source File
# Begin Source File

SOURCE=.\BinPropertySheet.cpp
# End Source File
# Begin Source File

SOURCE=.\CalendarButton.cpp
# End Source File
# Begin Source File

SOURCE=.\CalendarPage.cpp
# End Source File
# Begin Source File

SOURCE=.\CalendarSheet.cpp
# End Source File
# Begin Source File

SOURCE=.\CardDefinition.cpp
# End Source File
# Begin Source File

SOURCE=.\CardPlusData.cpp
# End Source File
# Begin Source File

SOURCE=.\CardPlusMain.cpp
# End Source File
# Begin Source File

SOURCE=.\CardPlusPropSheet.cpp
# End Source File
# Begin Source File

SOURCE=.\CardsFleetDefinition.cpp
# End Source File
# Begin Source File

SOURCE=.\CardsMerchantLimit.cpp
# End Source File
# Begin Source File

SOURCE=.\CardsPinVerification.cpp
# End Source File
# Begin Source File

SOURCE=.\CardsPinVerifVisa.cpp
# End Source File
# Begin Source File

SOURCE=.\CardsProcessing.cpp
# End Source File
# Begin Source File

SOURCE=.\CardsProfile.cpp
# End Source File
# Begin Source File

SOURCE=.\CardStatRec.cpp
# End Source File
# Begin Source File

SOURCE=.\CardStatus.cpp
# End Source File
# Begin Source File

SOURCE=.\CardsTime1.cpp
# End Source File
# Begin Source File

SOURCE=.\CardsTime2.cpp
# End Source File
# Begin Source File

SOURCE=.\CardsTime3a.cpp
# End Source File
# Begin Source File

SOURCE=.\CardsVelocity.cpp
# End Source File
# Begin Source File

SOURCE=.\CardTypeDescription.cpp
# End Source File
# Begin Source File

SOURCE=.\CardTypePropertySheet.cpp
# End Source File
# Begin Source File

SOURCE=.\CashBonus.cpp
# End Source File
# Begin Source File

SOURCE=.\CB_Amounts.cpp
# End Source File
# Begin Source File

SOURCE=.\CB_Definition.cpp
# End Source File
# Begin Source File

SOURCE=.\CB_General.cpp
# End Source File
# Begin Source File

SOURCE=.\CB_PropertySheet.cpp
# End Source File
# Begin Source File

SOURCE=.\CFActivity.cpp
# End Source File
# Begin Source File

SOURCE=.\CFCapability.cpp
# End Source File
# Begin Source File

SOURCE=.\CFCardBalAcc.cpp
# End Source File
# Begin Source File

SOURCE=.\CFDemogr.cpp
# End Source File
# Begin Source File

SOURCE=.\CFGeneral.cpp
# End Source File
# Begin Source File

SOURCE=.\CFMain.cpp
# End Source File
# Begin Source File

SOURCE=.\CFMerchUsage.cpp
# End Source File
# Begin Source File

SOURCE=.\CFSheet.cpp
# End Source File
# Begin Source File

SOURCE=.\CFTxHistory.cpp
# End Source File
# Begin Source File

SOURCE=.\ChangeHistory.cpp
# End Source File
# Begin Source File

SOURCE=.\ChangePassword.cpp
# End Source File
# Begin Source File

SOURCE=.\CntryAmtLimits.cpp
# End Source File
# Begin Source File

SOURCE=.\Conversion.cpp
# End Source File
# Begin Source File

SOURCE=.\ConversionPropertySheet.cpp
# End Source File
# Begin Source File

SOURCE=.\CreditPropertySheet.cpp
# End Source File
# Begin Source File

SOURCE=.\CurrencyCode.cpp
# End Source File
# Begin Source File

SOURCE=.\DAY.CPP
# End Source File
# Begin Source File

SOURCE=.\DBPasswords.cpp
# End Source File
# Begin Source File

SOURCE=.\DBPasswordsPropertySheet.cpp
# End Source File
# Begin Source File

SOURCE=.\DebitPropertySheet.cpp
# End Source File
# Begin Source File

SOURCE=.\DeferredDefinition.cpp
# End Source File
# Begin Source File

SOURCE=.\DeferredGeneral.cpp
# End Source File
# Begin Source File

SOURCE=.\DeferredPropertySheet.cpp
# End Source File
# Begin Source File

SOURCE=.\DialogCommSetup.cpp
# End Source File
# Begin Source File

SOURCE=.\DialogConfig.cpp
# End Source File
# Begin Source File

SOURCE=.\DialogMain.cpp
# End Source File
# Begin Source File

SOURCE=.\DialogPropertySheet.cpp
# End Source File
# Begin Source File

SOURCE=.\DialogRouter.cpp
# End Source File
# Begin Source File

SOURCE=.\DialogTpdu.cpp
# End Source File
# Begin Source File

SOURCE=.\DualEncrKeyControl.cpp
# End Source File
# Begin Source File

SOURCE=.\DualEnKeyCtrlPropertySheet.cpp
# End Source File
# Begin Source File

SOURCE=.\FleetPropertySheet.cpp
# End Source File
# Begin Source File

SOURCE=.\FraudGuardDefinition.cpp
# End Source File
# Begin Source File

SOURCE=.\FraudGuardGeneral.cpp
# End Source File
# Begin Source File

SOURCE=.\FraudGuardPropertySheet.cpp
# End Source File
# Begin Source File

SOURCE=.\GroupAccess.cpp
# End Source File
# Begin Source File

SOURCE=.\GroupDefinition.cpp
# End Source File
# Begin Source File

SOURCE=.\GroupPropertySheet.cpp
# End Source File
# Begin Source File

SOURCE=.\GroupWorkstation.cpp
# End Source File
# Begin Source File

SOURCE=.\IdleLoginfrm.cpp
# End Source File
# Begin Source File

SOURCE=.\JCBUPDATES.cpp
# End Source File
# Begin Source File

SOURCE=.\JobDefDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\JobDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\ListCtrlEx.cpp
# End Source File
# Begin Source File

SOURCE=.\ListExCtrl.cpp
# End Source File
# Begin Source File

SOURCE=.\Login.cpp
# End Source File
# Begin Source File

SOURCE=.\MainFrm.cpp
# End Source File
# Begin Source File

SOURCE=.\MCC102.cpp
# End Source File
# Begin Source File

SOURCE=.\MCC103.cpp
# End Source File
# Begin Source File

SOURCE=.\MCCRangeDefinition.cpp
# End Source File
# Begin Source File

SOURCE=.\MerchantBlocking.cpp
# End Source File
# Begin Source File

SOURCE=.\MerchantCapabilities.cpp
# End Source File
# Begin Source File

SOURCE=.\MerchantContact.cpp
# End Source File
# Begin Source File

SOURCE=.\MerchantDefinition.cpp
# End Source File
# Begin Source File

SOURCE=.\MerchantGeneral.cpp
# End Source File
# Begin Source File

SOURCE=.\MerchantGroupDefinition.cpp
# End Source File
# Begin Source File

SOURCE=.\MerchantGroupDefinitionSheet.cpp
# End Source File
# Begin Source File

SOURCE=.\MerchantGroupGeneral.cpp
# End Source File
# Begin Source File

SOURCE=.\MerchantLocation.cpp
# End Source File
# Begin Source File

SOURCE=.\MerchantPayParam.cpp
# End Source File
# Begin Source File

SOURCE=.\MerchantPayParamDefinition.cpp
# End Source File
# Begin Source File

SOURCE=.\MerchantPayParamSheet.cpp
# End Source File
# Begin Source File

SOURCE=.\MerchantPropertySheet.cpp
# End Source File
# Begin Source File

SOURCE=.\MerchantSettlement.cpp
# End Source File
# Begin Source File

SOURCE=.\MerchantTerminalBatchHistory.cpp
# End Source File
# Begin Source File

SOURCE=.\MerchantTerminalConfig.cpp
# End Source File
# Begin Source File

SOURCE=.\MerchantTerminalCUPCurrentBatch.cpp
# End Source File
# Begin Source File

SOURCE=.\MerchantTerminalCurrentBatch.cpp
# End Source File
# Begin Source File

SOURCE=.\MerchantTerminalGeneral.cpp
# End Source File
# Begin Source File

SOURCE=.\MerchantTerminalKeys.cpp
# End Source File
# Begin Source File

SOURCE=.\MerchantTerminalPoints.cpp
# End Source File
# Begin Source File

SOURCE=.\MerchantTerminalPropertySheet.cpp
# End Source File
# Begin Source File

SOURCE=.\MerchantVelocity.cpp
# End Source File
# Begin Source File

SOURCE=..\Monitor\monitorDoc.cpp
# End Source File
# Begin Source File

SOURCE=.\MraDBPasswords.cpp
# End Source File
# Begin Source File

SOURCE=.\MyUtils.cpp
# End Source File
# Begin Source File

SOURCE=.\NameList.cpp
# End Source File
# Begin Source File

SOURCE=.\NegativeBinRange.cpp
# End Source File
# Begin Source File

SOURCE=.\NegativeCardFileControl.cpp
# End Source File
# Begin Source File

SOURCE=.\NegativeCardFileControlPropertySheet.cpp
# End Source File
# Begin Source File

SOURCE=.\NegativeFileDefinition.cpp
# End Source File
# Begin Source File

SOURCE=.\NegativeGroupInfo.cpp
# End Source File
# Begin Source File

SOURCE=.\NegativeGroupPropertySheet.cpp
# End Source File
# Begin Source File

SOURCE=.\NetworkAcquirerPropertySheet.cpp
# End Source File
# Begin Source File

SOURCE=.\NetworkDefinition.cpp
# End Source File
# Begin Source File

SOURCE=.\NetworkGeneral.cpp
# End Source File
# Begin Source File

SOURCE=.\NetworkGeneral1.cpp
# End Source File
# Begin Source File

SOURCE=.\NetworkIssuerPropertySheet.cpp
# End Source File
# Begin Source File

SOURCE=.\NetworkLogon.cpp
# End Source File
# Begin Source File

SOURCE=.\NetworkProcessing.cpp
# End Source File
# Begin Source File

SOURCE=.\NetworkTotals.cpp
# End Source File
# Begin Source File

SOURCE=.\OverrideMerchantDenial.cpp
# End Source File
# Begin Source File

SOURCE=.\OverrideMerchantDenialPropertySheet.cpp
# End Source File
# Begin Source File

SOURCE=.\PanDemogrPage.cpp
# End Source File
# Begin Source File

SOURCE=.\PanMainPage.cpp
# End Source File
# Begin Source File

SOURCE=.\PanProcesPage.cpp
# End Source File
# Begin Source File

SOURCE=.\PanPropertySheet.cpp
# End Source File
# Begin Source File

SOURCE=.\PanTxPage.cpp
# End Source File
# Begin Source File

SOURCE=.\PosEntryModesListDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\ProfileDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\PurchaseCardDefinition.cpp
# End Source File
# Begin Source File

SOURCE=.\PurchaseCardGeneral.cpp
# End Source File
# Begin Source File

SOURCE=.\PurchaseCardPropertySheet.cpp
# End Source File
# Begin Source File

SOURCE=.\ReadMe.txt
# End Source File
# Begin Source File

SOURCE=.\RelatedDefinition.cpp
# End Source File
# Begin Source File

SOURCE=.\RelatedGeneral.cpp
# End Source File
# Begin Source File

SOURCE=.\RelatedPropertySheet.cpp
# End Source File
# Begin Source File

SOURCE=.\SchedulerConfig.cpp
# End Source File
# Begin Source File

SOURCE=.\SchedulerPropertySheet.cpp
# End Source File
# Begin Source File

SOURCE=.\SchSearch.cpp
# End Source File
# Begin Source File

SOURCE=.\SettleFileDefinition.cpp
# End Source File
# Begin Source File

SOURCE=.\SettleFilePropertySheet.cpp
# End Source File
# Begin Source File

SOURCE=.\SettlePrefixTime.cpp
# End Source File
# Begin Source File

SOURCE=.\ShedulerControl.cpp
# End Source File
# Begin Source File

SOURCE=.\SrvcMgr.cpp
# End Source File
# Begin Source File

SOURCE=.\srvcom_t.c
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\SystemGlobalParameterDefinition.cpp
# ADD CPP /Yc"SystemGlobalParameterDefinition.h"
# End Source File
# Begin Source File

SOURCE=.\SystemGlobalParameterSheet.cpp
# ADD CPP /Yc"SystemGlobalParameterSheet.h"
# End Source File
# Begin Source File

SOURCE=.\TerminalAutoInit.cpp
# End Source File
# Begin Source File

SOURCE=.\TerminalCapabilities.cpp
# End Source File
# Begin Source File

SOURCE=.\TerminalValidTransactions.cpp
# End Source File
# Begin Source File

SOURCE=.\TransactionDetails.cpp
# End Source File
# Begin Source File

SOURCE=.\UserAccess.cpp
# End Source File
# Begin Source File

SOURCE=.\UserAccountStatus.cpp
# End Source File
# Begin Source File

SOURCE=.\UserAuditLog1.cpp
# End Source File
# Begin Source File

SOURCE=.\UserDefinition.cpp
# End Source File
# Begin Source File

SOURCE=.\UserPropertySheet.cpp
# End Source File
# Begin Source File

SOURCE=.\UsersList.cpp
# End Source File
# Begin Source File

SOURCE=.\UserWorkstation.cpp
# End Source File
# Begin Source File

SOURCE=.\VoiceAuthInput.cpp
# End Source File
# Begin Source File

SOURCE=.\VoiceAuthResponse.cpp
# End Source File
# Begin Source File

SOURCE=.\WorkstationDefinition.cpp
# End Source File
# Begin Source File

SOURCE=.\WorkstationDescription.cpp
# End Source File
# Begin Source File

SOURCE=.\WorkstationPropertySheet.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl;fi;fd"
# Begin Source File

SOURCE=.\AlphaLookup.h
# End Source File
# Begin Source File

SOURCE=.\AMEXUPDATES1.h
# End Source File
# Begin Source File

SOURCE=.\APPDLG.H
# End Source File
# Begin Source File

SOURCE=.\ArchiveDBPasswords.h
# End Source File
# Begin Source File

SOURCE=.\Atp.H
# End Source File
# Begin Source File

SOURCE=.\AtpDOC.H
# End Source File
# Begin Source File

SOURCE=.\ATPTimedDialog.h
# End Source File
# Begin Source File

SOURCE=.\AtpVIEW.H
# End Source File
# Begin Source File

SOURCE=.\BatchDetails.h
# End Source File
# Begin Source File

SOURCE=.\BinBlocking.h
# End Source File
# Begin Source File

SOURCE=.\BinDefinition.h
# End Source File
# Begin Source File

SOURCE=.\BinGeneral.h
# End Source File
# Begin Source File

SOURCE=.\BinList.h
# End Source File
# Begin Source File

SOURCE=.\BinMccBlocking.h
# End Source File
# Begin Source File

SOURCE=.\BinPropertySheet.h
# End Source File
# Begin Source File

SOURCE=.\CalendarButton.h
# End Source File
# Begin Source File

SOURCE=.\CalendarPage.h
# End Source File
# Begin Source File

SOURCE=.\CalendarSheet.h
# End Source File
# Begin Source File

SOURCE=.\CardDefinition.h
# End Source File
# Begin Source File

SOURCE=.\cardplusdata.h
# End Source File
# Begin Source File

SOURCE=.\CardPlusMain.h
# End Source File
# Begin Source File

SOURCE=.\CardPlusPropSheet.h
# End Source File
# Begin Source File

SOURCE=.\CardsFleetDefinition.h
# End Source File
# Begin Source File

SOURCE=.\CardsMerchantLimit.h
# End Source File
# Begin Source File

SOURCE=.\cardspinverification.h
# End Source File
# Begin Source File

SOURCE=.\CardsPinVerifVisa.h
# End Source File
# Begin Source File

SOURCE=.\cardsprocessing.h
# End Source File
# Begin Source File

SOURCE=.\CardsProfile.h
# End Source File
# Begin Source File

SOURCE=.\CardStatRec.h
# End Source File
# Begin Source File

SOURCE=.\CardStatus.h
# End Source File
# Begin Source File

SOURCE=.\CardsTime1.h
# End Source File
# Begin Source File

SOURCE=.\CardsTime2.h
# End Source File
# Begin Source File

SOURCE=.\CardsTime3a.h
# End Source File
# Begin Source File

SOURCE=.\CardsVelocity.h
# End Source File
# Begin Source File

SOURCE=.\CardTypeDescription.h
# End Source File
# Begin Source File

SOURCE=.\CardTypePropertySheet.h
# End Source File
# Begin Source File

SOURCE=.\CashBonus.h
# End Source File
# Begin Source File

SOURCE=.\CB_Amounts.h
# End Source File
# Begin Source File

SOURCE=.\CB_Definition.h
# End Source File
# Begin Source File

SOURCE=.\CB_General.h
# End Source File
# Begin Source File

SOURCE=.\CB_PropertySheet.h
# End Source File
# Begin Source File

SOURCE=.\CFActivity.h
# End Source File
# Begin Source File

SOURCE=.\CFCapability.h
# End Source File
# Begin Source File

SOURCE=.\CFCardBalAcc.h
# End Source File
# Begin Source File

SOURCE=.\cfdemogr.h
# End Source File
# Begin Source File

SOURCE=.\cfgeneral.h
# End Source File
# Begin Source File

SOURCE=.\CFMAIN.H
# End Source File
# Begin Source File

SOURCE=.\CFMerchUsage.h
# End Source File
# Begin Source File

SOURCE=.\CFSHEET.H
# End Source File
# Begin Source File

SOURCE=.\CFTxHistory.h
# End Source File
# Begin Source File

SOURCE=.\ChangeHistory.h
# End Source File
# Begin Source File

SOURCE=.\ChangePassword.h
# End Source File
# Begin Source File

SOURCE=.\CntryAmtLimits.h
# End Source File
# Begin Source File

SOURCE=.\Conversion.h
# End Source File
# Begin Source File

SOURCE=.\ConversionPropertySheet.h
# End Source File
# Begin Source File

SOURCE=.\CreditPropertySheet.h
# End Source File
# Begin Source File

SOURCE=.\CurrencyCode.h
# End Source File
# Begin Source File

SOURCE=.\DAY.H
# End Source File
# Begin Source File

SOURCE=.\DBPasswords.h
# End Source File
# Begin Source File

SOURCE=.\DBPasswordsPropertySheet.h
# End Source File
# Begin Source File

SOURCE=.\DebitPropertySheet.h
# End Source File
# Begin Source File

SOURCE=.\DeferredDefinition.h
# End Source File
# Begin Source File

SOURCE=.\DeferredGeneral.h
# End Source File
# Begin Source File

SOURCE=.\DeferredPropertySheet.h
# End Source File
# Begin Source File

SOURCE=.\DialogCommSetup.h
# End Source File
# Begin Source File

SOURCE=.\DialogConfig.h
# End Source File
# Begin Source File

SOURCE=.\DialogMain.h
# End Source File
# Begin Source File

SOURCE=.\DialogPropertySheet.h
# End Source File
# Begin Source File

SOURCE=.\DialogRouter.h
# End Source File
# Begin Source File

SOURCE=.\DialogTpdu.h
# End Source File
# Begin Source File

SOURCE=.\DualEncrKeyControl.h
# End Source File
# Begin Source File

SOURCE=.\DualEnKeyCtrlPropertySheet.h
# End Source File
# Begin Source File

SOURCE=.\FleetPropertySheet.h
# End Source File
# Begin Source File

SOURCE=.\font.h
# End Source File
# Begin Source File

SOURCE=.\FraudGuardDefinition.h
# End Source File
# Begin Source File

SOURCE=.\FraudGuardGeneral.h
# End Source File
# Begin Source File

SOURCE=.\FraudGuardPropertySheet.h
# End Source File
# Begin Source File

SOURCE=.\groupaccess.h
# End Source File
# Begin Source File

SOURCE=.\groupdefinition.h
# End Source File
# Begin Source File

SOURCE=.\grouppropertysheet.h
# End Source File
# Begin Source File

SOURCE=.\groupworkstation.h
# End Source File
# Begin Source File

SOURCE=.\IdleLoginfrm.h
# End Source File
# Begin Source File

SOURCE=.\JCBUPDATES.h
# End Source File
# Begin Source File

SOURCE=.\JobDefDlg.h
# End Source File
# Begin Source File

SOURCE=.\JOBDLG.H
# End Source File
# Begin Source File

SOURCE=.\ListExCtrl.h
# End Source File
# Begin Source File

SOURCE=.\Lock.h
# End Source File
# Begin Source File

SOURCE=.\LOGIN.H
# End Source File
# Begin Source File

SOURCE=.\MainFrm.h
# End Source File
# Begin Source File

SOURCE=.\MCC102.h
# End Source File
# Begin Source File

SOURCE=.\MCC103.h
# End Source File
# Begin Source File

SOURCE=.\MCCRangeDefinition.h
# End Source File
# Begin Source File

SOURCE=.\MENUTAB.H
# End Source File
# Begin Source File

SOURCE=.\MerchantBlocking.h
# End Source File
# Begin Source File

SOURCE=.\MerchantCapabilities.h
# End Source File
# Begin Source File

SOURCE=.\MerchantContact.h
# End Source File
# Begin Source File

SOURCE=.\MerchantDefinition.h
# End Source File
# Begin Source File

SOURCE=.\MerchantGeneral.h
# End Source File
# Begin Source File

SOURCE=.\MerchantGroupDefinition.h
# End Source File
# Begin Source File

SOURCE=.\MerchantGroupDefinitionSheet.h
# End Source File
# Begin Source File

SOURCE=.\MerchantGroupGeneral.h
# End Source File
# Begin Source File

SOURCE=.\MerchantLocation.h
# End Source File
# Begin Source File

SOURCE=.\MerchantPayParam.h
# End Source File
# Begin Source File

SOURCE=.\MerchantPayParamDefinition.h
# End Source File
# Begin Source File

SOURCE=.\MerchantPayParamSheet.h
# End Source File
# Begin Source File

SOURCE=.\MerchantPropertySheet.h
# End Source File
# Begin Source File

SOURCE=.\MerchantSettlement.h
# End Source File
# Begin Source File

SOURCE=.\MerchantTerminalBatchHistory.h
# End Source File
# Begin Source File

SOURCE=.\MerchantTerminalConfig.h
# End Source File
# Begin Source File

SOURCE=.\MerchantTerminalCUPCurrentBatch.h
# End Source File
# Begin Source File

SOURCE=.\MerchantTerminalCurrentBatch.h
# End Source File
# Begin Source File

SOURCE=.\MerchantTerminalGeneral.h
# End Source File
# Begin Source File

SOURCE=.\MerchantTerminalKeys.h
# End Source File
# Begin Source File

SOURCE=.\MerchantTerminalPoints.h
# End Source File
# Begin Source File

SOURCE=.\MerchantTerminalPropertySheet.h
# End Source File
# Begin Source File

SOURCE=.\MerchantVelocity.h
# End Source File
# Begin Source File

SOURCE=.\MraDBPasswords.h
# End Source File
# Begin Source File

SOURCE=.\msmask.h
# End Source File
# Begin Source File

SOURCE=.\MYUTILS.H
# End Source File
# Begin Source File

SOURCE=.\NAMELIST.H
# End Source File
# Begin Source File

SOURCE=.\NegativeBinRange.h
# End Source File
# Begin Source File

SOURCE=.\NegativeCardFileControl.h
# End Source File
# Begin Source File

SOURCE=.\NegativeCardFileControlPropertySheet.h
# End Source File
# Begin Source File

SOURCE=.\NegativeFileDefinition.h
# End Source File
# Begin Source File

SOURCE=.\NegativeGroupInfo.h
# End Source File
# Begin Source File

SOURCE=.\NegativeGroupPropertySheet.h
# End Source File
# Begin Source File

SOURCE=.\NetworkAcquirerPropertySheet.h
# End Source File
# Begin Source File

SOURCE=.\networkdefinition.h
# End Source File
# Begin Source File

SOURCE=.\networkgeneral.h
# End Source File
# Begin Source File

SOURCE=.\networkgeneral1.h
# End Source File
# Begin Source File

SOURCE=.\NetworkIssuerPropertySheet.h
# End Source File
# Begin Source File

SOURCE=.\NetworkLogon.h
# End Source File
# Begin Source File

SOURCE=.\NetworkProcessing.h
# End Source File
# Begin Source File

SOURCE=.\NetworkTotals.h
# End Source File
# Begin Source File

SOURCE=.\OverrideMerchantDenial.h
# End Source File
# Begin Source File

SOURCE=.\OverrideMerchantDenialPropertySheet.h
# End Source File
# Begin Source File

SOURCE=.\PanDemogrPage.h
# End Source File
# Begin Source File

SOURCE=.\PanMainPage.h
# End Source File
# Begin Source File

SOURCE=.\PanProcesPage.h
# End Source File
# Begin Source File

SOURCE=.\PanPropertySheet.h
# End Source File
# Begin Source File

SOURCE=.\PanTxPage.h
# End Source File
# Begin Source File

SOURCE=.\picture.h
# End Source File
# Begin Source File

SOURCE=.\PosEntryModesListDlg.h
# End Source File
# Begin Source File

SOURCE=.\ProfileDlg.h
# End Source File
# Begin Source File

SOURCE=.\PurchaseCardDefinition.h
# End Source File
# Begin Source File

SOURCE=.\PurchaseCardGeneral.h
# End Source File
# Begin Source File

SOURCE=.\PurchaseCardPropertySheet.h
# End Source File
# Begin Source File

SOURCE=.\RelatedDefinition.h
# End Source File
# Begin Source File

SOURCE=.\relatedgeneral.h
# End Source File
# Begin Source File

SOURCE=.\RelatedPropertySheet.h
# End Source File
# Begin Source File

SOURCE=.\resource.h
# End Source File
# Begin Source File

SOURCE=.\resource.hm
# End Source File
# Begin Source File

SOURCE=.\SchedulerConfig.h
# End Source File
# Begin Source File

SOURCE=.\SchedulerPropertySheet.h
# End Source File
# Begin Source File

SOURCE=.\SchSearch.h
# End Source File
# Begin Source File

SOURCE=.\SCHTYPES.H
# End Source File
# Begin Source File

SOURCE=.\SettleFileDefinition.h
# End Source File
# Begin Source File

SOURCE=.\SettleFilePropertySheet.h
# End Source File
# Begin Source File

SOURCE=.\SettlePrefixTime.h
# End Source File
# Begin Source File

SOURCE=.\SharedMem.h
# End Source File
# Begin Source File

SOURCE=.\ShedulerControl.h
# End Source File
# Begin Source File

SOURCE=.\SRVCCOMM.H
# End Source File
# Begin Source File

SOURCE=.\SRVCINFO.H
# End Source File
# Begin Source File

SOURCE=.\SRVCMGR.H
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\SystemGlobalParameterDefinition.h
# End Source File
# Begin Source File

SOURCE=.\SystemGlobalParameterSheet.h
# End Source File
# Begin Source File

SOURCE=.\TerminalAutoInit.h
# End Source File
# Begin Source File

SOURCE=.\TerminalCapabilities.h
# End Source File
# Begin Source File

SOURCE=.\TerminalValidTransactions.h
# End Source File
# Begin Source File

SOURCE=.\TransactionDetails.h
# End Source File
# Begin Source File

SOURCE=.\TXUTILS.H
# End Source File
# Begin Source File

SOURCE=.\UserAccess.h
# End Source File
# Begin Source File

SOURCE=.\UserAccountStatus.h
# End Source File
# Begin Source File

SOURCE=.\UserAuditLog1.h
# End Source File
# Begin Source File

SOURCE=.\UserDefinition.h
# End Source File
# Begin Source File

SOURCE=.\UserPropertySheet.h
# End Source File
# Begin Source File

SOURCE=.\UsersList.h
# End Source File
# Begin Source File

SOURCE=.\UserWorkstation.h
# End Source File
# Begin Source File

SOURCE=.\VoiceAuthInput.h
# End Source File
# Begin Source File

SOURCE=.\VoiceAuthResponse.h
# End Source File
# Begin Source File

SOURCE=.\WorkstationDefinition.h
# End Source File
# Begin Source File

SOURCE=.\WorkstationDescription.h
# End Source File
# Begin Source File

SOURCE=.\WorkstationPropertySheet.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;cnt;rtf;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\res\Atp.rc2
# End Source File
# Begin Source File

SOURCE=.\res\AtpDoc.ico
# End Source File
# Begin Source File

SOURCE=.\res\bdo.ico
# End Source File
# Begin Source File

SOURCE=".\res\bdo2 - Copy.ico"
# End Source File
# Begin Source File

SOURCE=.\res\cursor1.cur
# End Source File
# Begin Source File

SOURCE=.\res\ico00001.ico
# End Source File
# Begin Source File

SOURCE=.\res\icon1.ico
# End Source File
# Begin Source File

SOURCE=.\res\logo2.bmp
# End Source File
# Begin Source File

SOURCE=.\res\Toolbar.bmp
# End Source File
# End Group
# Begin Source File

SOURCE=.\a.txt
# End Source File
# Begin Source File

SOURCE=.\res\manifest.xml
# End Source File
# Begin Source File

SOURCE=.\SharedMem.asp
# End Source File
# End Target
# End Project
# Section Atp : {C932BA85-4374-101B-A56C-00AA003668DC}
# 	2:21:DefaultSinkHeaderFile:msmask.h
# 	2:16:DefaultSinkClass:CMSMask
# End Section
# Section Atp : {BEF6E003-A874-101A-8BBA-00AA00300CAB}
# 	2:5:Class:COleFont
# 	2:10:HeaderFile:font.h
# 	2:8:ImplFile:font.cpp
# End Section
# Section Atp : {4D6CC9A0-DF77-11CF-8E74-00A0C90F26F8}
# 	2:5:Class:CMSMask
# 	2:10:HeaderFile:msmask.h
# 	2:8:ImplFile:msmask.cpp
# End Section
# Section Atp : {7BF80981-BF32-101A-8BBB-00AA00300CAB}
# 	2:5:Class:CPicture
# 	2:10:HeaderFile:picture.h
# 	2:8:ImplFile:picture.cpp
# End Section
