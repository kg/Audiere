# Microsoft Developer Studio Project File - Name="audiere" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=audiere - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "audiere.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "audiere.mak" CFG="audiere - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "audiere - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "audiere - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "audiere - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "output/Release"
# PROP Intermediate_Dir "output/Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "AUDIERE_EXPORTS" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "acoustique" /D "NDEBUG" /D "_WINDOWS" /D "_USRDLL" /D "AUDIERE_EXPORTS" /D for="if (0) ; else for" /D "WIN32" /D "_MBCS" /D "WIN32_LEAN_AND_MEAN" /D "STRICT" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386
# ADD LINK32 vorbisfile_static.lib vorbis_static.lib ogg_static.lib acoustique_s.lib dxguid.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386 /libpath:"acoustique"

!ELSEIF  "$(CFG)" == "audiere - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "output/Debug"
# PROP Intermediate_Dir "output/Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "AUDIERE_EXPORTS" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "acoustique" /D "_DEBUG" /D "_WINDOWS" /D "_USRDLL" /D "AUDIERE_EXPORTS" /D for="if (0) ; else for" /D "WIN32" /D "_MBCS" /D "WIN32_LEAN_AND_MEAN" /D "STRICT" /YX /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 vorbisfile_static_d.lib vorbis_static_d.lib ogg_static_d.lib acoustique_sd.lib dxguid.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept /libpath:"acoustique"

!ENDIF 

# Begin Target

# Name "audiere - Win32 Release"
# Name "audiere - Win32 Debug"
# Begin Group "files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\audiere.cpp
# End Source File
# Begin Source File

SOURCE=.\audiere.def
# End Source File
# Begin Source File

SOURCE=.\audiere.h
# End Source File
# Begin Source File

SOURCE=.\default_io.hpp
# End Source File
# Begin Source File

SOURCE=.\file.hpp
# End Source File
# Begin Source File

SOURCE=.\file_win32.cpp
# End Source File
# Begin Source File

SOURCE=.\output.cpp
# End Source File
# Begin Source File

SOURCE=.\output.hpp
# End Source File
# Begin Source File

SOURCE=.\output_dll.cpp
# End Source File
# Begin Source File

SOURCE=.\output_dll.hpp
# End Source File
# Begin Source File

SOURCE=.\output_ds.cpp
# End Source File
# Begin Source File

SOURCE=.\output_ds.hpp
# End Source File
# Begin Source File

SOURCE=.\output_ds3.cpp
# End Source File
# Begin Source File

SOURCE=.\output_ds3.hpp
# End Source File
# Begin Source File

SOURCE=.\output_ds8.hpp
# End Source File
# Begin Source File

SOURCE=.\output_null.cpp
# End Source File
# Begin Source File

SOURCE=.\output_null.hpp
# End Source File
# Begin Source File

SOURCE=.\threads.hpp
# End Source File
# Begin Source File

SOURCE=.\threads_win32.cpp
# End Source File
# End Group
# End Target
# End Project
