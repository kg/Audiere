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
# ADD CPP /nologo /MD /W3 /O2 /I "libogg-1.0beta4/include" /I "libvorbis-1.0beta4/include" /I "mad-0.13.0b/libmad" /I "mad-0.13.0b" /I "mikmod/lib/mikmod/include" /I "mikmod/lib/mmio/include" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "ACOUSTIQUE_EXPORTS" /D "FPM_DEFAULT" /D "LITTLE_ENDIAN" /FD /c
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
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386

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
# ADD CPP /nologo /MDd /W3 /Gm /ZI /Od /I "libogg-1.0beta4/include" /I "libvorbis-1.0beta4/include" /I "mad-0.13.0b/libmad" /I "mad-0.13.0b" /I "mikmod/lib/mikmod/include" /I "mikmod/lib/mmio/include" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "ACOUSTIQUE_EXPORTS" /D "FPM_DEFAULT" /D "LITTLE_ENDIAN" /FR /FD /GZ /c
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
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept

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
# ADD CPP /nologo /MD /W3 /O2 /I "libogg-1.0beta4/include" /I "libvorbis-1.0beta4/include" /I "mad-0.13.0b/libmad" /I "mad-0.13.0b" /I "mikmod/lib/mikmod/include" /I "mikmod/lib/mmio/include" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "ACOUSTIQUE_EXPORTS" /D "FPM_DEFAULT" /D "LITTLE_ENDIAN" /FD /c
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
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /profile /machine:I386

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

SOURCE=.\acq_mp3.cpp
# End Source File
# Begin Source File

SOURCE=.\acq_ogg.cpp
# End Source File
# Begin Source File

SOURCE=.\acq_wav.cpp
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
# Begin Group "libogg"

# PROP Default_Filter ""
# Begin Source File

SOURCE=".\libvorbis-1.0beta4\lib\analysis.c"
# End Source File
# Begin Source File

SOURCE=".\libvorbis-1.0beta4\lib\backends.h"
# End Source File
# Begin Source File

SOURCE=".\libvorbis-1.0beta4\lib\bitbuffer.h"
# End Source File
# Begin Source File

SOURCE=".\libogg-1.0beta4\src\bitwise.c"
# End Source File
# Begin Source File

SOURCE=".\libvorbis-1.0beta4\lib\block.c"
# End Source File
# Begin Source File

SOURCE=".\libvorbis-1.0beta4\lib\codebook.c"
# End Source File
# Begin Source File

SOURCE=".\libvorbis-1.0beta4\lib\codebook.h"
# End Source File
# Begin Source File

SOURCE=".\libvorbis-1.0beta4\include\vorbis\codec.h"
# End Source File
# Begin Source File

SOURCE=".\libvorbis-1.0beta4\lib\codec_internal.h"
# End Source File
# Begin Source File

SOURCE=".\libvorbis-1.0beta4\lib\envelope.c"
# End Source File
# Begin Source File

SOURCE=".\libvorbis-1.0beta4\lib\envelope.h"
# End Source File
# Begin Source File

SOURCE=".\libvorbis-1.0beta4\lib\floor0.c"
# End Source File
# Begin Source File

SOURCE=".\libogg-1.0beta4\src\framing.c"
# End Source File
# Begin Source File

SOURCE=".\libvorbis-1.0beta4\lib\info.c"
# End Source File
# Begin Source File

SOURCE=".\libvorbis-1.0beta4\lib\lookup.h"
# End Source File
# Begin Source File

SOURCE=".\libvorbis-1.0beta4\lib\lpc.c"
# End Source File
# Begin Source File

SOURCE=".\libvorbis-1.0beta4\lib\lpc.h"
# End Source File
# Begin Source File

SOURCE=".\libvorbis-1.0beta4\lib\lsp.c"
# End Source File
# Begin Source File

SOURCE=".\libvorbis-1.0beta4\lib\lsp.h"
# End Source File
# Begin Source File

SOURCE=".\libvorbis-1.0beta4\lib\mapping0.c"
# End Source File
# Begin Source File

SOURCE=".\libvorbis-1.0beta4\lib\masking.h"
# End Source File
# Begin Source File

SOURCE=".\libvorbis-1.0beta4\lib\mdct.c"
# End Source File
# Begin Source File

SOURCE=".\libvorbis-1.0beta4\lib\mdct.h"
# End Source File
# Begin Source File

SOURCE=".\libvorbis-1.0beta4\lib\misc.h"
# End Source File
# Begin Source File

SOURCE=".\libogg-1.0beta4\include\ogg\ogg.h"
# End Source File
# Begin Source File

SOURCE=".\libvorbis-1.0beta4\lib\os.h"
# End Source File
# Begin Source File

SOURCE=".\libogg-1.0beta4\include\ogg\os_types.h"
# End Source File
# Begin Source File

SOURCE=".\libvorbis-1.0beta4\lib\psy.c"
# End Source File
# Begin Source File

SOURCE=".\libvorbis-1.0beta4\lib\psy.h"
# End Source File
# Begin Source File

SOURCE=".\libvorbis-1.0beta4\lib\registry.c"
# End Source File
# Begin Source File

SOURCE=".\libvorbis-1.0beta4\lib\registry.h"
# End Source File
# Begin Source File

SOURCE=".\libvorbis-1.0beta4\lib\res0.c"
# End Source File
# Begin Source File

SOURCE=".\libvorbis-1.0beta4\lib\scales.h"
# End Source File
# Begin Source File

SOURCE=".\libvorbis-1.0beta4\lib\sharedbook.c"
# End Source File
# Begin Source File

SOURCE=".\libvorbis-1.0beta4\lib\smallft.c"
# End Source File
# Begin Source File

SOURCE=".\libvorbis-1.0beta4\lib\smallft.h"
# End Source File
# Begin Source File

SOURCE=".\libvorbis-1.0beta4\lib\synthesis.c"
# End Source File
# Begin Source File

SOURCE=".\libvorbis-1.0beta4\lib\time0.c"
# End Source File
# Begin Source File

SOURCE=".\libvorbis-1.0beta4\lib\vorbisfile.c"
# End Source File
# Begin Source File

SOURCE=".\libvorbis-1.0beta4\include\vorbis\vorbisfile.h"
# End Source File
# Begin Source File

SOURCE=".\libvorbis-1.0beta4\lib\window.c"
# End Source File
# Begin Source File

SOURCE=".\libvorbis-1.0beta4\lib\window.h"
# End Source File
# End Group
# Begin Group "mikmod"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\mikmod\lib\mikmod\src\virtch\resfilter\16.c
# End Source File
# Begin Source File

SOURCE=.\mikmod\lib\mikmod\src\virtch\resfilter\8.c
# End Source File
# Begin Source File

SOURCE=.\mikmod\lib\mikmod\src\drivers\drv_nos.c
# End Source File
# Begin Source File

SOURCE=.\mikmod\lib\mikmod\src\loaders\itshare.h
# End Source File
# Begin Source File

SOURCE=.\mikmod\lib\mikmod\src\loaders\Load_it.c
# End Source File
# Begin Source File

SOURCE=.\mikmod\lib\mikmod\src\loaders\Load_mod.c
# End Source File
# Begin Source File

SOURCE=.\mikmod\lib\mikmod\src\loaders\Load_mtm.c
# End Source File
# Begin Source File

SOURCE=.\mikmod\lib\mikmod\src\loaders\Load_s3m.c
# End Source File
# Begin Source File

SOURCE=.\mikmod\lib\mikmod\src\loaders\Load_stm.c
# End Source File
# Begin Source File

SOURCE=.\mikmod\lib\mikmod\src\loaders\Load_xm.c
# End Source File
# Begin Source File

SOURCE=.\mikmod\lib\mmio\src\log.c
# End Source File
# Begin Source File

SOURCE=.\mikmod\lib\mmio\include\log.h
# End Source File
# Begin Source File

SOURCE=.\mikmod\lib\mikmod\src\mdreg.c
# End Source File
# Begin Source File

SOURCE=.\mikmod\lib\mikmod\src\mdriver.c
# End Source File
# Begin Source File

SOURCE=.\mikmod\lib\mikmod\include\mikmod.h
# End Source File
# Begin Source File

SOURCE=.\mikmod\lib\mikmod\src\mloader.c
# End Source File
# Begin Source File

SOURCE=.\mikmod\lib\mikmod\src\mlreg.c
# End Source File
# Begin Source File

SOURCE=.\mikmod\lib\mmio\src\mmalloc.c
# End Source File
# Begin Source File

SOURCE=.\mikmod\lib\mmio\src\mmconfig.c
# End Source File
# Begin Source File

SOURCE=.\mikmod\lib\mmio\include\mmconfig.h
# End Source File
# Begin Source File

SOURCE=.\mikmod\lib\mmio\src\mmcopy.c
# End Source File
# Begin Source File

SOURCE=.\mikmod\lib\mmio\src\mmerror.c
# End Source File
# Begin Source File

SOURCE=.\mikmod\lib\mmio\src\win32\mmforbid.c
# End Source File
# Begin Source File

SOURCE=.\mikmod\lib\mmio\include\mmforbid.h
# End Source File
# Begin Source File

SOURCE=.\mikmod\lib\mmio\include\mminline.h
# End Source File
# Begin Source File

SOURCE=.\mikmod\lib\mmio\src\mmio.c
# End Source File
# Begin Source File

SOURCE=.\mikmod\lib\mmio\include\mmio.h
# End Source File
# Begin Source File

SOURCE=.\mikmod\lib\mmio\include\mmtypes.h
# End Source File
# Begin Source File

SOURCE=.\mikmod\lib\mikmod\src\mpforbid.h
# End Source File
# Begin Source File

SOURCE=.\mikmod\lib\mikmod\src\mplayer.c
# End Source File
# Begin Source File

SOURCE=.\mikmod\lib\mikmod\include\mplayer.h
# End Source File
# Begin Source File

SOURCE=.\mikmod\lib\mikmod\src\munitrk.c
# End Source File
# Begin Source File

SOURCE=.\mikmod\lib\mikmod\src\virtch\nc16.c
# End Source File
# Begin Source File

SOURCE=.\mikmod\lib\mikmod\src\virtch\nc16ss.c
# End Source File
# Begin Source File

SOURCE=.\mikmod\lib\mikmod\src\virtch\nc8.c
# End Source File
# Begin Source File

SOURCE=.\mikmod\lib\mikmod\src\virtch\nc8ss.c
# End Source File
# Begin Source File

SOURCE=.\mikmod\lib\mikmod\src\npertab.c
# End Source File
# Begin Source File

SOURCE=.\mikmod\lib\mikmod\src\virtch\placebo.c
# End Source File
# Begin Source File

SOURCE=.\mikmod\lib\mikmod\src\virtch\resfilter\resshare.h
# End Source File
# Begin Source File

SOURCE=.\mikmod\lib\mikmod\src\loaders\S3m_it.c
# End Source File
# Begin Source File

SOURCE=.\mikmod\lib\mikmod\src\sloader.c
# End Source File
# Begin Source File

SOURCE=.\mikmod\lib\mikmod\src\virtch\ssmix.h
# End Source File
# Begin Source File

SOURCE=.\mikmod\lib\mikmod\src\virtch\stdmix.h
# End Source File
# Begin Source File

SOURCE=.\mikmod\lib\mikmod\include\uniform.h
# End Source File
# Begin Source File

SOURCE=.\mikmod\lib\mikmod\src\unimod.c
# End Source File
# Begin Source File

SOURCE=.\mikmod\lib\mikmod\src\virtch\vc16.c
# End Source File
# Begin Source File

SOURCE=.\mikmod\lib\mikmod\src\virtch\vc16ss.c
# End Source File
# Begin Source File

SOURCE=.\mikmod\lib\mikmod\src\virtch\vc8.c
# End Source File
# Begin Source File

SOURCE=.\mikmod\lib\mikmod\src\virtch\vc8ss.c
# End Source File
# Begin Source File

SOURCE=.\mikmod\lib\mikmod\src\virtch\vchcrap.h
# End Source File
# Begin Source File

SOURCE=.\mikmod\lib\mikmod\src\virtch\virtch.c
# End Source File
# Begin Source File

SOURCE=.\mikmod\lib\mikmod\include\virtch.h
# End Source File
# Begin Source File

SOURCE=.\mikmod\lib\mikmod\src\voiceset.c
# End Source File
# Begin Source File

SOURCE=.\mikmod\lib\mikmod\src\virtch\wrap16.c
# End Source File
# Begin Source File

SOURCE=.\mikmod\lib\mikmod\src\virtch\wrap16.h
# End Source File
# Begin Source File

SOURCE=.\mikmod\lib\mikmod\src\virtch\wrap8.c
# End Source File
# Begin Source File

SOURCE=.\mikmod\lib\mikmod\src\virtch\wrap8.h
# End Source File
# End Group
# End Target
# End Project
# End Project
