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
# ADD CPP /nologo /MD /W3 /GX /O2 /I "mikmod/mikmod" /I "mikmod/mmio" /D "NDEBUG" /D "_LIB" /D for="if (0) ; else for" /D "WIN32" /D "_MBCS" /D "WIN32_LEAN_AND_MEAN" /D "STRICT" /FD /c
# SUBTRACT CPP /YX
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
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "mikmod/mikmod" /I "mikmod/mmio" /D "_DEBUG" /D "_LIB" /D for="if (0) ; else for" /D "WIN32" /D "_MBCS" /D "WIN32_LEAN_AND_MEAN" /D "STRICT" /FD /GZ /c
# SUBTRACT CPP /YX
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

SOURCE=.\mikmod\mikmod\virtch\vchcrap.h
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

SOURCE=.\mikmod\mikmod\virtch.h
# End Source File
# Begin Source File

SOURCE=.\mikmod\mikmod\voiceset.c
# End Source File
# End Group
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

SOURCE=.\config.h
# End Source File
# Begin Source File

SOURCE=.\context.cpp
# End Source File
# Begin Source File

SOURCE=.\context.hpp
# End Source File
# Begin Source File

SOURCE=.\debug.cpp
# End Source File
# Begin Source File

SOURCE=.\debug.hpp
# End Source File
# Begin Source File

SOURCE=.\default_file.hpp
# End Source File
# Begin Source File

SOURCE=.\endian.hpp
# End Source File
# Begin Source File

SOURCE=.\file.cpp
# End Source File
# Begin Source File

SOURCE=.\file.hpp
# End Source File
# Begin Source File

SOURCE=.\file_ansi.cpp
# End Source File
# Begin Source File

SOURCE=.\input.cpp
# End Source File
# Begin Source File

SOURCE=.\input.hpp
# End Source File
# Begin Source File

SOURCE=.\input_mod.cpp
# End Source File
# Begin Source File

SOURCE=.\input_mod.hpp
# End Source File
# Begin Source File

SOURCE=.\input_ogg.cpp
# End Source File
# Begin Source File

SOURCE=.\input_ogg.hpp
# End Source File
# Begin Source File

SOURCE=.\input_wav.cpp
# End Source File
# Begin Source File

SOURCE=.\input_wav.hpp
# End Source File
# Begin Source File

SOURCE=.\output.cpp
# End Source File
# Begin Source File

SOURCE=.\output.hpp
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

SOURCE=.\repeatable.cpp
# End Source File
# Begin Source File

SOURCE=.\repeatable.hpp
# End Source File
# Begin Source File

SOURCE=.\stream.cpp
# End Source File
# Begin Source File

SOURCE=.\stream.hpp
# End Source File
# Begin Source File

SOURCE=.\threads.hpp
# End Source File
# Begin Source File

SOURCE=.\threads_win32.cpp
# End Source File
# Begin Source File

SOURCE=.\utility.cpp
# End Source File
# Begin Source File

SOURCE=.\utility.hpp
# End Source File
# End Group
# End Target
# End Project
