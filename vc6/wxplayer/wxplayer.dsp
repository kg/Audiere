# Microsoft Developer Studio Project File - Name="wxplayer" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=wxplayer - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "wxplayer.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "wxplayer.mak" CFG="wxplayer - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "wxplayer - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "wxplayer - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "wxplayer - Win32 Release"

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
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GR /GX /O1 /Ob0 /I "../../src" /D "NDEBUG" /D for="if (0) ; else for" /D "WIN32_LEAN_AND_MEAN" /D "__WXMSW__" /D "__WIN95__" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /FD /c
# SUBTRACT CPP /YX
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 wxmsw.lib rpcrt4.lib comctl32.lib ws2_32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386 /out:"../bin/Release/wxplayer.exe"
# SUBTRACT LINK32 /debug

!ELSEIF  "$(CFG)" == "wxplayer - Win32 Debug"

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
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GR /GX /ZI /Od /I "../../src" /D "_DEBUG" /D for="if (0) ; else for" /D "WIN32_LEAN_AND_MEAN" /D "__WXMSW__" /D "__WIN95__" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /FD /GZ /c
# SUBTRACT CPP /YX
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 wxmswd.lib rpcrt4.lib comctl32.lib ws2_32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /out:"../bin/Debug/wxplayer.exe" /pdbtype:sept

!ENDIF 

# Begin Target

# Name "wxplayer - Win32 Release"
# Name "wxplayer - Win32 Debug"
# Begin Group "files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\examples\wxPlayer\wxPlayer.cpp
# End Source File
# Begin Source File

SOURCE=..\..\examples\wxPlayer\wxPlayer.rc
# End Source File
# End Group
# Begin Source File

SOURCE=..\..\examples\wxPlayer\wx\msw\blank.cur
# End Source File
# Begin Source File

SOURCE=..\..\examples\wxPlayer\wx\msw\bullseye.cur
# End Source File
# Begin Source File

SOURCE=..\..\examples\wxPlayer\wx\msw\cdrom.ico
# End Source File
# Begin Source File

SOURCE=..\..\examples\wxPlayer\wx\msw\computer.ico
# End Source File
# Begin Source File

SOURCE=..\..\examples\wxPlayer\wx\msw\drive.ico
# End Source File
# Begin Source File

SOURCE=..\..\examples\wxPlayer\wx\msw\error.ico
# End Source File
# Begin Source File

SOURCE=..\..\examples\wxPlayer\wx\msw\file1.ico
# End Source File
# Begin Source File

SOURCE=..\..\examples\wxPlayer\wx\msw\floppy.ico
# End Source File
# Begin Source File

SOURCE=..\..\examples\wxPlayer\wx\msw\folder1.ico
# End Source File
# Begin Source File

SOURCE=..\..\examples\wxPlayer\wx\msw\folder2.ico
# End Source File
# Begin Source File

SOURCE=..\..\examples\wxPlayer\wx\msw\hand.cur
# End Source File
# Begin Source File

SOURCE=..\..\examples\wxPlayer\wx\msw\info.ico
# End Source File
# Begin Source File

SOURCE=..\..\examples\wxPlayer\wx\msw\magnif1.cur
# End Source File
# Begin Source File

SOURCE=..\..\examples\wxPlayer\wx\msw\noentry.cur
# End Source File
# Begin Source File

SOURCE=..\..\examples\wxPlayer\wx\msw\pbrush.cur
# End Source File
# Begin Source File

SOURCE=..\..\examples\wxPlayer\wx\msw\pencil.cur
# End Source File
# Begin Source File

SOURCE=..\..\examples\wxPlayer\wx\msw\pntleft.cur
# End Source File
# Begin Source File

SOURCE=..\..\examples\wxPlayer\wx\msw\pntright.cur
# End Source File
# Begin Source File

SOURCE=..\..\examples\wxPlayer\wx\msw\query.cur
# End Source File
# Begin Source File

SOURCE=..\..\examples\wxPlayer\wx\msw\question.ico
# End Source File
# Begin Source File

SOURCE=..\..\examples\wxPlayer\wx\msw\removble.ico
# End Source File
# Begin Source File

SOURCE=..\..\examples\wxPlayer\wx\msw\rightarr.cur
# End Source File
# Begin Source File

SOURCE=..\..\examples\wxPlayer\wx\msw\roller.cur
# End Source File
# Begin Source File

SOURCE=..\..\examples\wxPlayer\wx\msw\size.cur
# End Source File
# Begin Source File

SOURCE=..\..\examples\wxPlayer\wx\msw\tip.ico
# End Source File
# Begin Source File

SOURCE=..\..\examples\wxPlayer\wx\msw\warning.ico
# End Source File
# Begin Source File

SOURCE=..\..\examples\wxPlayer\wx\msw\watch1.cur
# End Source File
# End Target
# End Project
