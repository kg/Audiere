# Microsoft Developer Studio Project File - Name="acoustique" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=acoustique - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "acoustique.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "acoustique.mak" CFG="acoustique - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "acoustique - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "acoustique - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "acoustique - Win32 Release Profile" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "acoustique - Win32 Release"

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
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "ACOUSTIQUE_EXPORTS" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "mad-0.14.1b/libmad" /I "resample" /I "mikmod/mikmod" /I "mikmod/mmio" /D "NDEBUG" /D "_WINDOWS" /D "_USRDLL" /D "ACOUSTIQUE_EXPORTS" /D "WIN32" /D "_MBCS" /D "HAVE_CONFIG_H" /D "FPM_DEFAULT" /D "LITTLE_ENDIAN" /D "WITH_MP3" /FD /c
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
# ADD LINK32 vorbisfile_static.lib vorbis_static.lib ogg_static.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386

!ELSEIF  "$(CFG)" == "acoustique - Win32 Debug"

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
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "ACOUSTIQUE_EXPORTS" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "mad-0.14.1b/libmad" /I "resample" /I "mikmod/mikmod" /I "mikmod/mmio" /D "_DEBUG" /D "_WINDOWS" /D "_USRDLL" /D "ACOUSTIQUE_EXPORTS" /D "WIN32" /D "_MBCS" /D "HAVE_CONFIG_H" /D "FPM_DEFAULT" /D "LITTLE_ENDIAN" /D "WITH_MP3" /FR /FD /GZ /c
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
# ADD LINK32 vorbisfile_static_d.lib vorbis_static_d.lib ogg_static_d.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept

!ELSEIF  "$(CFG)" == "acoustique - Win32 Release Profile"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "acoustique___Win32_Release_Profile"
# PROP BASE Intermediate_Dir "acoustique___Win32_Release_Profile"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "output/Release_Profile"
# PROP Intermediate_Dir "output/Release_Profile"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /O2 /I "libogg-1.0beta4/include" /I "mad-0.13.0b/libmad" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "ACOUSTIQUE_EXPORTS" /D "FPM_64BIT" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "mad-0.14.1b/libmad" /I "resample" /I "mikmod/mikmod" /I "mikmod/mmio" /D "NDEBUG" /D "_WINDOWS" /D "_USRDLL" /D "ACOUSTIQUE_EXPORTS" /D "WIN32" /D "_MBCS" /D "HAVE_CONFIG_H" /D "FPM_DEFAULT" /D "LITTLE_ENDIAN" /D "WITH_MP3" /FD /c
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
# ADD LINK32 vorbisfile_static.lib vorbis_static.lib ogg_static.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /profile /machine:I386

!ENDIF 

# Begin Target

# Name "acoustique - Win32 Release"
# Name "acoustique - Win32 Debug"
# Name "acoustique - Win32 Release Profile"
# Begin Group "files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\acoustique.cpp
# End Source File
# Begin Source File

SOURCE=.\acoustique.def
# End Source File
# Begin Source File

SOURCE=.\acoustique.h
# End Source File
# Begin Source File

SOURCE=.\acq_internal.hpp
# End Source File
# Begin Source File

SOURCE=.\acq_mod.cpp
# End Source File
# Begin Source File

SOURCE=.\acq_ogg.cpp
# End Source File
# Begin Source File

SOURCE=.\acq_wav.cpp
# End Source File
# Begin Source File

SOURCE=.\debug.cpp
# End Source File
# Begin Source File

SOURCE=.\debug.hpp
# End Source File
# Begin Source File

SOURCE=.\endian.hpp
# End Source File
# End Group
# Begin Group "mikmod"

# PROP Default_Filter ""
# Begin Group "loaders"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\mikmod\mikmod\loaders\itshare.h
# End Source File
# Begin Source File

SOURCE=.\mikmod\mikmod\loaders\Load_it.c
# End Source File
# Begin Source File

SOURCE=.\mikmod\mikmod\loaders\Load_mod.c
# End Source File
# Begin Source File

SOURCE=.\mikmod\mikmod\loaders\Load_s3m.c
# End Source File
# Begin Source File

SOURCE=.\mikmod\mikmod\loaders\Load_stm.c
# End Source File
# Begin Source File

SOURCE=.\mikmod\mikmod\loaders\Load_xm.c
# End Source File
# Begin Source File

SOURCE=.\mikmod\mikmod\loaders\S3m_it.c
# End Source File
# End Group
# Begin Group "virtch"

# PROP Default_Filter ""
# Begin Group "resfilter"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\mikmod\mikmod\virtch\resfilter\16.c
# End Source File
# Begin Source File

SOURCE=.\mikmod\mikmod\virtch\resfilter\8.c
# End Source File
# Begin Source File

SOURCE=.\mikmod\mikmod\virtch\resfilter\resshare.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\mikmod\mikmod\virtch\nc16.c
# End Source File
# Begin Source File

SOURCE=.\mikmod\mikmod\virtch\nc8.c
# End Source File
# Begin Source File

SOURCE=.\mikmod\mikmod\virtch\placebo.c
# End Source File
# Begin Source File

SOURCE=.\mikmod\mikmod\virtch\stdmix.h
# End Source File
# Begin Source File

SOURCE=.\mikmod\mikmod\virtch\vc16.c
# End Source File
# Begin Source File

SOURCE=.\mikmod\mikmod\virtch\vc8.c
# End Source File
# Begin Source File

SOURCE=.\mikmod\mikmod\virtch\virtch.c
# End Source File
# Begin Source File

SOURCE=.\mikmod\mikmod\virtch\wrap16.c
# End Source File
# Begin Source File

SOURCE=.\mikmod\mikmod\virtch\wrap16.h
# End Source File
# Begin Source File

SOURCE=.\mikmod\mikmod\virtch\wrap8.c
# End Source File
# Begin Source File

SOURCE=.\mikmod\mikmod\virtch\wrap8.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\mikmod\mikmod\drv_nos.c
# End Source File
# Begin Source File

SOURCE=.\mikmod\mikmod\mdriver.c
# End Source File
# Begin Source File

SOURCE=.\mikmod\mikmod\mikmod.h
# End Source File
# Begin Source File

SOURCE=.\mikmod\mikmod\mloader.c
# End Source File
# Begin Source File

SOURCE=.\mikmod\mikmod\mpforbid.h
# End Source File
# Begin Source File

SOURCE=.\mikmod\mikmod\mplayer.c
# End Source File
# Begin Source File

SOURCE=.\mikmod\mikmod\mplayer.h
# End Source File
# Begin Source File

SOURCE=.\mikmod\mikmod\munitrk.c
# End Source File
# Begin Source File

SOURCE=.\mikmod\mikmod\npertab.c
# End Source File
# Begin Source File

SOURCE=.\mikmod\mikmod\sloader.c
# End Source File
# Begin Source File

SOURCE=.\mikmod\mikmod\uniform.h
# End Source File
# Begin Source File

SOURCE=.\mikmod\mikmod\unimod.c
# End Source File
# Begin Source File

SOURCE=.\mikmod\mikmod\virtch\vchcrap.h
# End Source File
# Begin Source File

SOURCE=.\mikmod\mikmod\virtch.h
# End Source File
# Begin Source File

SOURCE=.\mikmod\mikmod\voiceset.c
# End Source File
# End Group
# Begin Group "mmio"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\mikmod\mmio\log.c
# End Source File
# Begin Source File

SOURCE=.\mikmod\mmio\log.h
# End Source File
# Begin Source File

SOURCE=.\mikmod\mmio\mmalloc.c
# End Source File
# Begin Source File

SOURCE=.\mikmod\mmio\mmconfig.c
# End Source File
# Begin Source File

SOURCE=.\mikmod\mmio\mmconfig.h
# End Source File
# Begin Source File

SOURCE=.\mikmod\mmio\mmcopy.c
# End Source File
# Begin Source File

SOURCE=.\mikmod\mmio\mmerror.c
# End Source File
# Begin Source File

SOURCE=.\mikmod\mmio\mmforbid.h
# End Source File
# Begin Source File

SOURCE=.\mikmod\mmio\mminline.h
# End Source File
# Begin Source File

SOURCE=.\mikmod\mmio\mmio.c
# End Source File
# Begin Source File

SOURCE=.\mikmod\mmio\mmio.h
# End Source File
# Begin Source File

SOURCE=.\mikmod\mmio\mmtypes.h
# End Source File
# Begin Source File

SOURCE=.\mikmod\mmio\win32_mmforbid.c
# End Source File
# End Group
# End Target
# End Project
