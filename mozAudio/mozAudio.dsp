# Microsoft Developer Studio Project File - Name="mozAudio" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=mozAudio - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "mozAudio.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "mozAudio.mak" CFG="mozAudio - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "mozAudio - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "mozAudio - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "mozAudio - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "MOZAUDIO_EXPORTS" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "MOZAUDIO_EXPORTS" /D "XP_WIN" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386
# ADD LINK32 acoustique_s.lib audiere_s.lib dxguid.lib nbase.lib xpcom.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386 /libpath:"../../nbase"

!ELSEIF  "$(CFG)" == "mozAudio - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "MOZAUDIO_EXPORTS" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "MOZAUDIO_EXPORTS" /D "XP_WIN" /YX /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 acoustique_sd.lib audiere_sd.lib dxguid.lib nbase.lib xpcom.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept /libpath:"../../nbase"

!ENDIF 

# Begin Target

# Name "mozAudio - Win32 Release"
# Name "mozAudio - Win32 Debug"
# Begin Group "files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\audiere.h
# End Source File
# Begin Source File

SOURCE=.\mozAudioContext.cpp
# End Source File
# Begin Source File

SOURCE=.\mozAudioContext.h
# End Source File
# Begin Source File

SOURCE=.\mozAudioModule.cpp
# End Source File
# Begin Source File

SOURCE=.\mozAudioService.cpp
# End Source File
# Begin Source File

SOURCE=.\mozAudioService.h
# End Source File
# Begin Source File

SOURCE=.\mozAudioStream.cpp
# End Source File
# Begin Source File

SOURCE=.\mozAudioStream.h
# End Source File
# Begin Source File

SOURCE=.\mozIAudioContext.h
# End Source File
# Begin Source File

SOURCE=.\mozIAudioService.h
# End Source File
# Begin Source File

SOURCE=.\mozIAudioStream.h
# End Source File
# End Group
# Begin Group "mozilla"

# PROP Default_Filter ""
# Begin Source File

SOURCE=F:\mozilla\dist\include\nsCom.h
# End Source File
# Begin Source File

SOURCE=F:\mozilla\dist\include\nsComponentManagerUtils.h
# End Source File
# Begin Source File

SOURCE=F:\mozilla\dist\include\nsCOMPtr.h
# End Source File
# Begin Source File

SOURCE=F:\mozilla\dist\include\nscore.h
# End Source File
# Begin Source File

SOURCE=F:\mozilla\dist\include\nsDebug.h
# End Source File
# Begin Source File

SOURCE=F:\mozilla\dist\include\nsError.h
# End Source File
# Begin Source File

SOURCE=F:\mozilla\dist\include\nsIClassInfo.h
# End Source File
# Begin Source File

SOURCE=F:\mozilla\dist\include\nsIComponentManager.h
# End Source File
# Begin Source File

SOURCE=F:\mozilla\dist\include\nsID.h
# End Source File
# Begin Source File

SOURCE=F:\mozilla\dist\include\nsIEnumerator.h
# End Source File
# Begin Source File

SOURCE=F:\mozilla\dist\include\nsIFactory.h
# End Source File
# Begin Source File

SOURCE=F:\mozilla\dist\include\nsIFile.h
# End Source File
# Begin Source File

SOURCE=F:\mozilla\dist\include\nsIGenericFactory.h
# End Source File
# Begin Source File

SOURCE=F:\mozilla\dist\include\nsIID.h
# End Source File
# Begin Source File

SOURCE=F:\mozilla\dist\include\nsILocalFile.h
# End Source File
# Begin Source File

SOURCE=F:\mozilla\dist\include\nsIMemory.h
# End Source File
# Begin Source File

SOURCE=F:\mozilla\dist\include\nsIModule.h
# End Source File
# Begin Source File

SOURCE=F:\mozilla\dist\include\nsIProgrammingLanguage.h
# End Source File
# Begin Source File

SOURCE=F:\mozilla\dist\include\nsIProperties.h
# End Source File
# Begin Source File

SOURCE=F:\mozilla\dist\include\nsIServiceManager.h
# End Source File
# Begin Source File

SOURCE=F:\mozilla\dist\include\nsISupports.h
# End Source File
# Begin Source File

SOURCE=F:\mozilla\dist\include\nsISupportsUtils.h
# End Source File
# Begin Source File

SOURCE=F:\mozilla\dist\include\nsMemory.h
# End Source File
# Begin Source File

SOURCE=F:\mozilla\dist\include\nsrootidl.h
# End Source File
# Begin Source File

SOURCE=F:\mozilla\dist\include\nsTraceRefcnt.h
# End Source File
# Begin Source File

SOURCE=F:\mozilla\dist\include\nspr\pratom.h
# End Source File
# Begin Source File

SOURCE=F:\mozilla\dist\include\nspr\prcpucfg.h
# End Source File
# Begin Source File

SOURCE=F:\mozilla\dist\include\nspr\prinet.h
# End Source File
# Begin Source File

SOURCE=F:\mozilla\dist\include\nspr\prinrval.h
# End Source File
# Begin Source File

SOURCE=F:\mozilla\dist\include\nspr\prio.h
# End Source File
# Begin Source File

SOURCE=F:\mozilla\dist\include\nspr\prlink.h
# End Source File
# Begin Source File

SOURCE=F:\mozilla\dist\include\nspr\prlock.h
# End Source File
# Begin Source File

SOURCE=F:\mozilla\dist\include\nspr\prlong.h
# End Source File
# Begin Source File

SOURCE=F:\mozilla\dist\include\nspr\obsolete\protypes.h
# End Source File
# Begin Source File

SOURCE=F:\mozilla\dist\include\nspr\prtime.h
# End Source File
# Begin Source File

SOURCE=F:\mozilla\dist\include\nspr\prtypes.h
# End Source File
# End Group
# End Target
# End Project
