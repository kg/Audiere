# Microsoft Developer Studio Project File - Name="static" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=static - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "static.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "static.mak" CFG="static - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "static - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "static - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "static - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "static___Win32_Release"
# PROP BASE Intermediate_Dir "static___Win32_Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "output/Static_Release"
# PROP Intermediate_Dir "output/Static_Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "acoustique" /D "NDEBUG" /D "_LIB" /D "USE_DIRECTX8" /D for="if (0) ; else for" /D "WIN32" /D "_MBCS" /D "WIN32_LEAN_AND_MEAN" /D "STRICT" /YX /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"audiere_s.lib"

!ELSEIF  "$(CFG)" == "static - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "static___Win32_Debug"
# PROP BASE Intermediate_Dir "static___Win32_Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "output/Static_Debug"
# PROP Intermediate_Dir "output/Static_Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "acoustique" /D "_DEBUG" /D "_LIB" /D "USE_DIRECTX8" /D for="if (0) ; else for" /D "WIN32" /D "_MBCS" /D "WIN32_LEAN_AND_MEAN" /D "STRICT" /YX /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"audiere_sd.lib"

!ENDIF 

# Begin Target

# Name "static - Win32 Release"
# Name "static - Win32 Debug"
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

SOURCE=.\context.cpp
# End Source File
# Begin Source File

SOURCE=.\debug.cpp
# End Source File
# Begin Source File

SOURCE=.\debug.hpp
# End Source File
# Begin Source File

SOURCE=.\file.hpp
# End Source File
# Begin Source File

SOURCE=.\file_ansi.cpp
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

SOURCE=.\stream.cpp
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
