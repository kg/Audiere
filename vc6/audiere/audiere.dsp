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
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "AUDIERE_EXPORTS" /YX /FD /c
# ADD CPP /nologo /MT /W3 /GR /GX /O2 /I "..\..\third-party-vc6\include" /I "..\..\src\flac-1.0.4\include" /I "..\..\src\flac-1.0.4\src\libFLAC\include" /I "..\..\src\flac-1.0.4\src\libFLAC++\include" /I "..\..\src\mpegsound" /D "NDEBUG" /D for="if (0) ; else for" /D "WIN32_LEAN_AND_MEAN" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "AUDIERE_EXPORTS" /D "FLAC__NO_DLL" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386
# ADD LINK32 libFLAC++_static.lib libFLAC_static.lib dumb_static.lib vorbisfile_static.lib vorbis_static.lib ogg_static.lib dxguid.lib rpcrt4.lib winmm.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386 /out:"../bin/Release/audiere.dll" /libpath:"../../third-party-vc6/lib"

!ELSEIF  "$(CFG)" == "audiere - Win32 Debug"

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
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "AUDIERE_EXPORTS" /YX /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GR /GX /ZI /Od /I "..\..\third-party-vc6\include" /I "..\..\src\flac-1.0.4\include" /I "..\..\src\flac-1.0.4\src\libFLAC\include" /I "..\..\src\flac-1.0.4\src\libFLAC++\include" /I "..\..\src\mpegsound" /D "_DEBUG" /D for="if (0) ; else for" /D "WIN32_LEAN_AND_MEAN" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "AUDIERE_EXPORTS" /D "FLAC__NO_DLL" /FD /D /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 libFLAC++_static_d.lib libFLAC_static_d.lib dumbd_static.lib vorbisfile_static_d.lib vorbis_static_d.lib ogg_static_d.lib dxguid.lib rpcrt4.lib winmm.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /out:"../bin/Debug/audiere.dll" /pdbtype:sept /libpath:"../../third-party-vc6/lib"

!ENDIF 

# Begin Target

# Name "audiere - Win32 Release"
# Name "audiere - Win32 Debug"
# Begin Group "files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\src\audiere.h
# End Source File
# Begin Source File

SOURCE=..\..\src\audiere.rc
# End Source File
# Begin Source File

SOURCE=..\..\src\basic_source.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\basic_source.h
# End Source File
# Begin Source File

SOURCE=..\..\src\debug.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\debug.h
# End Source File
# Begin Source File

SOURCE=..\..\src\default_file.h
# End Source File
# Begin Source File

SOURCE=..\..\src\device.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\device_ds.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\device_ds.h
# End Source File
# Begin Source File

SOURCE=..\..\src\device_ds_buffer.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\device_ds_buffer.h
# End Source File
# Begin Source File

SOURCE=..\..\src\device_ds_stream.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\device_ds_stream.h
# End Source File
# Begin Source File

SOURCE=..\..\src\device_mixer.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\device_mixer.h
# End Source File
# Begin Source File

SOURCE=..\..\src\device_mm.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\device_mm.h
# End Source File
# Begin Source File

SOURCE=..\..\src\device_null.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\device_null.h
# End Source File
# Begin Source File

SOURCE=..\..\src\file_ansi.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\input.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\input_aiff.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\input_aiff.h
# End Source File
# Begin Source File

SOURCE=..\..\src\input_flac.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\input_flac.h
# End Source File
# Begin Source File

SOURCE=..\..\src\input_mod.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\input_mod.h
# End Source File
# Begin Source File

SOURCE=..\..\src\input_mp3.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\input_mp3.h
# End Source File
# Begin Source File

SOURCE=..\..\src\input_ogg.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\input_ogg.h
# End Source File
# Begin Source File

SOURCE=..\..\src\input_wav.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\input_wav.h
# End Source File
# Begin Source File

SOURCE=..\..\src\internal.h
# End Source File
# Begin Source File

SOURCE=..\..\src\loop_point_source.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\memory_file.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\memory_file.h
# End Source File
# Begin Source File

SOURCE=..\..\src\noise.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\resampler.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\resampler.h
# End Source File
# Begin Source File

SOURCE=..\..\src\sample_buffer.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\sound.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\sound_effect.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\square_wave.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\threads.h
# End Source File
# Begin Source File

SOURCE=..\..\src\threads_win32.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\timer.h
# End Source File
# Begin Source File

SOURCE=..\..\src\timer_win32.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\tone.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\types.h
# End Source File
# Begin Source File

SOURCE=..\..\src\utility.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\utility.h
# End Source File
# Begin Source File

SOURCE=..\..\src\version.cpp
# End Source File
# End Group
# Begin Group "mpegsound"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\src\mpegsound\bitwindow.cpp

!IF  "$(CFG)" == "audiere - Win32 Release"

# ADD CPP /Os /Oy /Ob1

!ELSEIF  "$(CFG)" == "audiere - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\src\mpegsound\filter.cpp

!IF  "$(CFG)" == "audiere - Win32 Release"

# ADD CPP /Os /Oy /Ob1

!ELSEIF  "$(CFG)" == "audiere - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\src\mpegsound\filter_2.cpp

!IF  "$(CFG)" == "audiere - Win32 Release"

# ADD CPP /Os /Oy /Ob1

!ELSEIF  "$(CFG)" == "audiere - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\src\mpegsound\huffmantable.cpp

!IF  "$(CFG)" == "audiere - Win32 Release"

# ADD CPP /Os /Oy /Ob1

!ELSEIF  "$(CFG)" == "audiere - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\src\mpegsound\mpeglayer1.cpp

!IF  "$(CFG)" == "audiere - Win32 Release"

# ADD CPP /Os /Oy /Ob1

!ELSEIF  "$(CFG)" == "audiere - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\src\mpegsound\mpeglayer2.cpp

!IF  "$(CFG)" == "audiere - Win32 Release"

# ADD CPP /Os /Oy /Ob1

!ELSEIF  "$(CFG)" == "audiere - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\src\mpegsound\mpeglayer3.cpp

!IF  "$(CFG)" == "audiere - Win32 Release"

# ADD CPP /Os /Oy /Ob1

!ELSEIF  "$(CFG)" == "audiere - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\src\mpegsound\mpegsound.h
# End Source File
# Begin Source File

SOURCE=..\..\src\mpegsound\mpegsound_locals.h
# End Source File
# Begin Source File

SOURCE=..\..\src\mpegsound\mpegtable.cpp

!IF  "$(CFG)" == "audiere - Win32 Release"

# ADD CPP /Os /Oy /Ob1

!ELSEIF  "$(CFG)" == "audiere - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\src\mpegsound\mpegtoraw.cpp

!IF  "$(CFG)" == "audiere - Win32 Release"

# ADD CPP /Os /Oy /Ob1

!ELSEIF  "$(CFG)" == "audiere - Win32 Debug"

!ENDIF 

# End Source File
# End Group
# End Target
# End Project
