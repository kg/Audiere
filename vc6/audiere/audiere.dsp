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
# ADD CPP /nologo /MT /W3 /GX /O2 /I "..\..\src\mikmod\mmio" /I "..\..\src\mikmod\mikmod" /I "..\..\src\libogg-1.0rc3\include" /I "..\..\src\libvorbis-1.0rc3\include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "AUDIERE_EXPORTS" /FD /c
# SUBTRACT CPP /YX
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386
# ADD LINK32 dxguid.lib winmm.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386 /out:"../bin/Release/audiere.dll"

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
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I "..\..\src\mikmod\mmio" /I "..\..\src\mikmod\mikmod" /I "..\..\src\libogg-1.0rc3\include" /I "..\..\src\libvorbis-1.0rc3\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "AUDIERE_EXPORTS" /FD /GZ /c
# SUBTRACT CPP /YX
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 dxguid.lib winmm.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /out:"../bin/Debug/audiere.dll" /pdbtype:sept

!ENDIF 

# Begin Target

# Name "audiere - Win32 Release"
# Name "audiere - Win32 Debug"
# Begin Group "files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\src\audiere.def
# End Source File
# Begin Source File

SOURCE=..\..\src\audiere.h
# End Source File
# Begin Source File

SOURCE=..\..\src\config.h
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

SOURCE=..\..\src\device_ds3.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\device_ds3.h
# End Source File
# Begin Source File

SOURCE=..\..\src\device_ds8.h
# End Source File
# Begin Source File

SOURCE=..\..\src\device_null.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\device_null.h
# End Source File
# Begin Source File

SOURCE=..\..\src\endian.h
# End Source File
# Begin Source File

SOURCE=..\..\src\file_ansi.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\input.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\input_mod.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\input_mod.h
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

SOURCE=..\..\src\repeatable.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\repeatable.h
# End Source File
# Begin Source File

SOURCE=..\..\src\sound.cpp
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
# Begin Group "mmio"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\src\mikmod\mmio\log.c
# End Source File
# Begin Source File

SOURCE=..\..\src\mikmod\mmio\log.h
# End Source File
# Begin Source File

SOURCE=..\..\src\mikmod\mmio\mmalloc.c
# End Source File
# Begin Source File

SOURCE=..\..\src\mikmod\mmio\mmconfig.c
# End Source File
# Begin Source File

SOURCE=..\..\src\mikmod\mmio\mmconfig.h
# End Source File
# Begin Source File

SOURCE=..\..\src\mikmod\mmio\mmcopy.c
# End Source File
# Begin Source File

SOURCE=..\..\src\mikmod\mmio\mmerror.c
# End Source File
# Begin Source File

SOURCE=..\..\src\mikmod\mmio\mmforbid.h
# End Source File
# Begin Source File

SOURCE=..\..\src\mikmod\mmio\mmgetopt.h
# End Source File
# Begin Source File

SOURCE=..\..\src\mikmod\mmio\mminline.h
# End Source File
# Begin Source File

SOURCE=..\..\src\mikmod\mmio\mmio.c
# End Source File
# Begin Source File

SOURCE=..\..\src\mikmod\mmio\mmio.h
# End Source File
# Begin Source File

SOURCE=..\..\src\mikmod\mmio\mmtypes.h
# End Source File
# Begin Source File

SOURCE=..\..\src\mikmod\mmio\win32_mmforbid.c
# End Source File
# End Group
# Begin Group "mikmod"

# PROP Default_Filter ""
# Begin Group "loaders"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\src\mikmod\mikmod\loaders\itshare.h
# End Source File
# Begin Source File

SOURCE=..\..\src\mikmod\mikmod\loaders\Load_it.c
# End Source File
# Begin Source File

SOURCE=..\..\src\mikmod\mikmod\loaders\Load_m15.c
# End Source File
# Begin Source File

SOURCE=..\..\src\mikmod\mikmod\loaders\Load_mod.c
# End Source File
# Begin Source File

SOURCE=..\..\src\mikmod\mikmod\loaders\Load_mtm.c
# End Source File
# Begin Source File

SOURCE=..\..\src\mikmod\mikmod\loaders\Load_s3m.c
# End Source File
# Begin Source File

SOURCE=..\..\src\mikmod\mikmod\loaders\Load_stm.c
# End Source File
# Begin Source File

SOURCE=..\..\src\mikmod\mikmod\loaders\Load_ult.c
# End Source File
# Begin Source File

SOURCE=..\..\src\mikmod\mikmod\loaders\Load_xm.c
# End Source File
# Begin Source File

SOURCE=..\..\src\mikmod\mikmod\loaders\S3m_it.c
# End Source File
# End Group
# Begin Group "virtch"

# PROP Default_Filter ""
# Begin Group "resfilter"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\src\mikmod\mikmod\virtch\resfilter\16.c
# End Source File
# Begin Source File

SOURCE=..\..\src\mikmod\mikmod\virtch\resfilter\8.c
# End Source File
# Begin Source File

SOURCE=..\..\src\mikmod\mikmod\virtch\resfilter\resshare.h
# End Source File
# End Group
# Begin Source File

SOURCE=..\..\src\mikmod\mikmod\virtch\nc16.c
# End Source File
# Begin Source File

SOURCE=..\..\src\mikmod\mikmod\virtch\nc16ss.c
# End Source File
# Begin Source File

SOURCE=..\..\src\mikmod\mikmod\virtch\nc8.c
# End Source File
# Begin Source File

SOURCE=..\..\src\mikmod\mikmod\virtch\nc8ss.c
# End Source File
# Begin Source File

SOURCE=..\..\src\mikmod\mikmod\virtch\placebo.c
# End Source File
# Begin Source File

SOURCE=..\..\src\mikmod\mikmod\virtch\regmix_ss.c
# End Source File
# Begin Source File

SOURCE=..\..\src\mikmod\mikmod\virtch\ssmix.h
# End Source File
# Begin Source File

SOURCE=..\..\src\mikmod\mikmod\virtch\stdmix.h
# End Source File
# Begin Source File

SOURCE=..\..\src\mikmod\mikmod\virtch\vc16.c
# End Source File
# Begin Source File

SOURCE=..\..\src\mikmod\mikmod\virtch\vc16ss.c
# End Source File
# Begin Source File

SOURCE=..\..\src\mikmod\mikmod\virtch\vc8.c
# End Source File
# Begin Source File

SOURCE=..\..\src\mikmod\mikmod\virtch\vc8ss.c
# End Source File
# Begin Source File

SOURCE=..\..\src\mikmod\mikmod\virtch\vchcrap.h
# End Source File
# Begin Source File

SOURCE=..\..\src\mikmod\mikmod\virtch\virtch.c
# End Source File
# Begin Source File

SOURCE=..\..\src\mikmod\mikmod\virtch\wrap16.c
# End Source File
# Begin Source File

SOURCE=..\..\src\mikmod\mikmod\virtch\wrap16.h
# End Source File
# Begin Source File

SOURCE=..\..\src\mikmod\mikmod\virtch\wrap8.c
# End Source File
# Begin Source File

SOURCE=..\..\src\mikmod\mikmod\virtch\wrap8.h
# End Source File
# End Group
# Begin Source File

SOURCE=..\..\src\mikmod\mikmod\drv_nos.c
# End Source File
# Begin Source File

SOURCE=..\..\src\mikmod\mikmod\mdriver.c
# End Source File
# Begin Source File

SOURCE=..\..\src\mikmod\mikmod\mdsfx.c
# End Source File
# Begin Source File

SOURCE=..\..\src\mikmod\mikmod\mdsfx.h
# End Source File
# Begin Source File

SOURCE=..\..\src\mikmod\mikmod\mikmod.h
# End Source File
# Begin Source File

SOURCE=..\..\src\mikmod\mikmod\mloader.c
# End Source File
# Begin Source File

SOURCE=..\..\src\mikmod\mikmod\mlreg.c
# End Source File
# Begin Source File

SOURCE=..\..\src\mikmod\mikmod\mpforbid.h
# End Source File
# Begin Source File

SOURCE=..\..\src\mikmod\mikmod\mplayer.c
# End Source File
# Begin Source File

SOURCE=..\..\src\mikmod\mikmod\mplayer.h
# End Source File
# Begin Source File

SOURCE=..\..\src\mikmod\mikmod\munitrk.c
# End Source File
# Begin Source File

SOURCE=..\..\src\mikmod\mikmod\mwav.c
# End Source File
# Begin Source File

SOURCE=..\..\src\mikmod\mikmod\mwav.h
# End Source File
# Begin Source File

SOURCE=..\..\src\mikmod\mikmod\npertab.c
# End Source File
# Begin Source File

SOURCE=..\..\src\mikmod\mikmod\sloader.c
# End Source File
# Begin Source File

SOURCE=..\..\src\mikmod\mikmod\snglen.c
# End Source File
# Begin Source File

SOURCE=..\..\src\mikmod\mikmod\uniform.h
# End Source File
# Begin Source File

SOURCE=..\..\src\mikmod\mikmod\unimod.c
# End Source File
# Begin Source File

SOURCE=..\..\src\mikmod\mikmod\virtch.h
# End Source File
# Begin Source File

SOURCE=..\..\src\mikmod\mikmod\voiceset.c
# End Source File
# End Group
# Begin Group "ogg"

# PROP Default_Filter ""
# Begin Source File

SOURCE="..\..\src\libogg-1.0rc3\src\bitwise.c"
# End Source File
# Begin Source File

SOURCE="..\..\src\libogg-1.0rc3\include\ogg\config_types.h"
# End Source File
# Begin Source File

SOURCE="..\..\src\libogg-1.0rc3\src\framing.c"
# End Source File
# Begin Source File

SOURCE="..\..\src\libogg-1.0rc3\include\ogg\ogg.h"
# End Source File
# Begin Source File

SOURCE="..\..\src\libogg-1.0rc3\include\ogg\os_types.h"
# End Source File
# End Group
# Begin Group "vorbis"

# PROP Default_Filter ""
# Begin Source File

SOURCE="..\..\src\libvorbis-1.0rc3\lib\analysis.c"
# End Source File
# Begin Source File

SOURCE="..\..\src\libvorbis-1.0rc3\lib\backends.h"
# End Source File
# Begin Source File

SOURCE="..\..\src\libvorbis-1.0rc3\lib\bitrate.c"
# End Source File
# Begin Source File

SOURCE="..\..\src\libvorbis-1.0rc3\lib\bitrate.h"
# End Source File
# Begin Source File

SOURCE="..\..\src\libvorbis-1.0rc3\lib\block.c"
# End Source File
# Begin Source File

SOURCE="..\..\src\libvorbis-1.0rc3\lib\codebook.c"
# End Source File
# Begin Source File

SOURCE="..\..\src\libvorbis-1.0rc3\lib\codebook.h"
# End Source File
# Begin Source File

SOURCE="..\..\src\libvorbis-1.0rc3\include\vorbis\codec.h"
# End Source File
# Begin Source File

SOURCE="..\..\src\libvorbis-1.0rc3\lib\codec_internal.h"
# End Source File
# Begin Source File

SOURCE="..\..\src\libvorbis-1.0rc3\lib\envelope.c"
# End Source File
# Begin Source File

SOURCE="..\..\src\libvorbis-1.0rc3\lib\envelope.h"
# End Source File
# Begin Source File

SOURCE="..\..\src\libvorbis-1.0rc3\lib\floor0.c"
# End Source File
# Begin Source File

SOURCE="..\..\src\libvorbis-1.0rc3\lib\floor1.c"
# End Source File
# Begin Source File

SOURCE="..\..\src\libvorbis-1.0rc3\lib\iir.h"
# End Source File
# Begin Source File

SOURCE="..\..\src\libvorbis-1.0rc3\lib\info.c"
# End Source File
# Begin Source File

SOURCE="..\..\src\libvorbis-1.0rc3\lib\lookup.c"
# End Source File
# Begin Source File

SOURCE="..\..\src\libvorbis-1.0rc3\lib\lookup.h"
# End Source File
# Begin Source File

SOURCE="..\..\src\libvorbis-1.0rc3\lib\lookup_data.h"
# End Source File
# Begin Source File

SOURCE="..\..\src\libvorbis-1.0rc3\lib\lpc.c"
# End Source File
# Begin Source File

SOURCE="..\..\src\libvorbis-1.0rc3\lib\lpc.h"
# End Source File
# Begin Source File

SOURCE="..\..\src\libvorbis-1.0rc3\lib\lsp.c"
# End Source File
# Begin Source File

SOURCE="..\..\src\libvorbis-1.0rc3\lib\lsp.h"
# End Source File
# Begin Source File

SOURCE="..\..\src\libvorbis-1.0rc3\lib\mapping0.c"
# End Source File
# Begin Source File

SOURCE="..\..\src\libvorbis-1.0rc3\lib\masking.h"
# End Source File
# Begin Source File

SOURCE="..\..\src\libvorbis-1.0rc3\lib\mdct.c"
# End Source File
# Begin Source File

SOURCE="..\..\src\libvorbis-1.0rc3\lib\mdct.h"
# End Source File
# Begin Source File

SOURCE="..\..\src\libvorbis-1.0rc3\lib\misc.h"
# End Source File
# Begin Source File

SOURCE="..\..\src\libvorbis-1.0rc3\lib\os.h"
# End Source File
# Begin Source File

SOURCE="..\..\src\libvorbis-1.0rc3\lib\psy.c"
# End Source File
# Begin Source File

SOURCE="..\..\src\libvorbis-1.0rc3\lib\psy.h"
# End Source File
# Begin Source File

SOURCE="..\..\src\libvorbis-1.0rc3\lib\registry-api.h"
# End Source File
# Begin Source File

SOURCE="..\..\src\libvorbis-1.0rc3\lib\registry.c"
# End Source File
# Begin Source File

SOURCE="..\..\src\libvorbis-1.0rc3\lib\registry.h"
# End Source File
# Begin Source File

SOURCE="..\..\src\libvorbis-1.0rc3\lib\res0.c"
# End Source File
# Begin Source File

SOURCE="..\..\src\libvorbis-1.0rc3\lib\scales.h"
# End Source File
# Begin Source File

SOURCE="..\..\src\libvorbis-1.0rc3\lib\sharedbook.c"
# End Source File
# Begin Source File

SOURCE="..\..\src\libvorbis-1.0rc3\lib\smallft.c"
# End Source File
# Begin Source File

SOURCE="..\..\src\libvorbis-1.0rc3\lib\smallft.h"
# End Source File
# Begin Source File

SOURCE="..\..\src\libvorbis-1.0rc3\lib\synthesis.c"
# End Source File
# Begin Source File

SOURCE="..\..\src\libvorbis-1.0rc3\lib\time0.c"
# End Source File
# Begin Source File

SOURCE="..\..\src\libvorbis-1.0rc3\lib\vorbisfile.c"
# End Source File
# Begin Source File

SOURCE="..\..\src\libvorbis-1.0rc3\include\vorbis\vorbisfile.h"
# End Source File
# Begin Source File

SOURCE="..\..\src\libvorbis-1.0rc3\lib\window.c"
# End Source File
# Begin Source File

SOURCE="..\..\src\libvorbis-1.0rc3\lib\window.h"
# End Source File
# End Group
# End Target
# End Project
