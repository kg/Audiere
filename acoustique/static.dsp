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
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "output/Static_Release"
# PROP Intermediate_Dir "output/Static_Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "mad-0.13.0b/libmad" /I "mad-0.13.0b" /I "mikmod/mikmod" /I "mikmod/mmio" /D "NDEBUG" /D "_LIB" /D "WIN32" /D "_MBCS" /D "FPM_DEFAULT" /D "LITTLE_ENDIAN" /D "MIKMOD_STATIC" /YX /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"acoustique_s.lib"

!ELSEIF  "$(CFG)" == "static - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "output/Static_Debug"
# PROP Intermediate_Dir "output/Static_Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "mad-0.13.0b/libmad" /I "mad-0.13.0b" /I "mikmod/mikmod" /I "mikmod/mmio" /D "_DEBUG" /D "_LIB" /D "WIN32" /D "_MBCS" /D "FPM_DEFAULT" /D "LITTLE_ENDIAN" /D "MIKMOD_STATIC" /YX /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"acoustique_sd.lib"

!ENDIF 

# Begin Target

# Name "static - Win32 Release"
# Name "static - Win32 Debug"
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

SOURCE=.\acq_mod.hpp
# End Source File
# Begin Source File

SOURCE=.\acq_mp3.cpp
# End Source File
# Begin Source File

SOURCE=.\acq_mp3.hpp
# End Source File
# Begin Source File

SOURCE=.\acq_ogg.cpp
# End Source File
# Begin Source File

SOURCE=.\acq_ogg.hpp
# End Source File
# Begin Source File

SOURCE=.\acq_wav.cpp
# End Source File
# Begin Source File

SOURCE=.\acq_wav.hpp
# End Source File
# Begin Source File

SOURCE=.\endian.hpp
# End Source File
# End Group
# Begin Group "libmad"

# PROP Default_Filter ""
# Begin Source File

SOURCE=".\mad-0.13.0b\libmad\bit.c"
# End Source File
# Begin Source File

SOURCE=".\mad-0.13.0b\libmad\bit.h"
# End Source File
# Begin Source File

SOURCE=".\mad-0.13.0b\libmad\D.dat"
# End Source File
# Begin Source File

SOURCE=".\mad-0.13.0b\libmad\extern.h"
# End Source File
# Begin Source File

SOURCE=".\mad-0.13.0b\libmad\fixed.c"
# End Source File
# Begin Source File

SOURCE=".\mad-0.13.0b\libmad\fixed.h"
# End Source File
# Begin Source File

SOURCE=".\mad-0.13.0b\libmad\frame.c"
# End Source File
# Begin Source File

SOURCE=".\mad-0.13.0b\libmad\frame.h"
# End Source File
# Begin Source File

SOURCE=".\mad-0.13.0b\global.h"
# End Source File
# Begin Source File

SOURCE=".\mad-0.13.0b\libmad\global.h"
# End Source File
# Begin Source File

SOURCE=".\mad-0.13.0b\libmad\huffman.c"
# End Source File
# Begin Source File

SOURCE=".\mad-0.13.0b\libmad\huffman.h"
# End Source File
# Begin Source File

SOURCE=".\mad-0.13.0b\libmad\imdct_s.dat"
# End Source File
# Begin Source File

SOURCE=".\mad-0.13.0b\libmad\layer12.c"
# End Source File
# Begin Source File

SOURCE=".\mad-0.13.0b\libmad\layer12.h"
# End Source File
# Begin Source File

SOURCE=".\mad-0.13.0b\libmad\layer3.c"
# End Source File
# Begin Source File

SOURCE=".\mad-0.13.0b\libmad\layer3.h"
# End Source File
# Begin Source File

SOURCE=".\mad-0.13.0b\libmad\mad.h"
# End Source File
# Begin Source File

SOURCE=".\mad-0.13.0b\libmad\qc_table.dat"
# End Source File
# Begin Source File

SOURCE=".\mad-0.13.0b\resample.c"
# End Source File
# Begin Source File

SOURCE=".\mad-0.13.0b\resample.h"
# End Source File
# Begin Source File

SOURCE=".\mad-0.13.0b\libmad\rq_table.dat"
# End Source File
# Begin Source File

SOURCE=".\mad-0.13.0b\libmad\sf_table.dat"
# End Source File
# Begin Source File

SOURCE=".\mad-0.13.0b\libmad\stream.c"
# End Source File
# Begin Source File

SOURCE=".\mad-0.13.0b\libmad\stream.h"
# End Source File
# Begin Source File

SOURCE=".\mad-0.13.0b\libmad\synth.c"
# End Source File
# Begin Source File

SOURCE=".\mad-0.13.0b\libmad\synth.h"
# End Source File
# Begin Source File

SOURCE=".\mad-0.13.0b\libmad\timer.c"
# End Source File
# Begin Source File

SOURCE=".\mad-0.13.0b\libmad\timer.h"
# End Source File
# Begin Source File

SOURCE=".\mad-0.13.0b\libmad\version.c"
# End Source File
# Begin Source File

SOURCE=".\mad-0.13.0b\libmad\version.h"
# End Source File
# End Group
# Begin Group "mikmod"

# PROP Default_Filter ""
# Begin Group "loaders"

# PROP Default_Filter ""
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

SOURCE=.\mikmod\mikmod\virtch\wrap8.c
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
