# Microsoft Developer Studio Project File - Name="wxPlayer" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=wxPlayer - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "wxPlayer.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "wxPlayer.mak" CFG="wxPlayer - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "wxPlayer - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "wxPlayer - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "wxPlayer - Win32 Release"

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
# ADD CPP /nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "WIN32_LEAN_AND_MEAN" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "__WXMSW__" /D "__WIN95__" /D "__WIN32__" /D WINVER=0x0400 /D "STRICT" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 wx.lib audiere_s.lib acoustique_s.lib nbase.lib dxguid.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib comctl32.lib ws2_32.lib rpcrt4.lib /nologo /subsystem:windows /machine:I386 /libpath:"../../nbase"

!ELSEIF  "$(CFG)" == "wxPlayer - Win32 Debug"

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
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "_DEBUG" /D DEBUG=1 /D "__WXDEBUG__" /D "WIN32_LEAN_AND_MEAN" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "__WXMSW__" /D "__WIN95__" /D "__WIN32__" /D WINVER=0x0400 /D "STRICT" /YX /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 wxd.lib audiere_sd.lib acoustique_sd.lib nbase.lib dxguid.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib comctl32.lib ws2_32.lib rpcrt4.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept /libpath:"../../nbase"

!ENDIF 

# Begin Target

# Name "wxPlayer - Win32 Release"
# Name "wxPlayer - Win32 Debug"
# Begin Group "files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\wxPlayer.cpp
# End Source File
# End Group
# Begin Group "wx"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\..\wx230\include\wx\accel.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\wx230\include\wx\msw\accel.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\wx230\include\wx\app.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\wx230\include\wx\msw\app.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\wx230\include\wx\bitmap.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\wx230\include\wx\msw\bitmap.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\wx230\include\wx\bmpbuttn.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\wx230\include\wx\msw\bmpbuttn.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\wx230\include\wx\brush.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\wx230\include\wx\msw\brush.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\wx230\include\wx\button.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\wx230\include\wx\msw\button.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\wx230\include\wx\checkbox.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\wx230\include\wx\msw\checkbox.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\wx230\include\wx\generic\choicdgg.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\wx230\include\wx\choicdlg.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\wx230\include\wx\choice.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\wx230\include\wx\msw\choice.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\wx230\include\wx\cmndata.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\wx230\include\wx\colour.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\wx230\include\wx\msw\colour.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\wx230\include\wx\combobox.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\wx230\include\wx\msw\combobox.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\wx230\include\wx\ctrlsub.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\wx230\include\wx\cursor.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\wx230\include\wx\msw\cursor.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\wx230\include\wx\dc.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\wx230\include\wx\msw\dc.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\wx230\include\wx\dcclient.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\wx230\include\wx\msw\dcclient.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\wx230\include\wx\dcmemory.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\wx230\include\wx\msw\dcmemory.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\wx230\include\wx\dcprint.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\wx230\include\wx\msw\dcprint.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\wx230\include\wx\dcscreen.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\wx230\include\wx\msw\dcscreen.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\wx230\include\wx\debug.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\wx230\include\wx\defs.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\wx230\include\wx\dialog.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\wx230\include\wx\msw\dialog.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\wx230\include\wx\dirdlg.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\wx230\include\wx\msw\dirdlg.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\wx230\include\wx\dynarray.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\wx230\include\wx\event.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\wx230\include\wx\filedlg.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\wx230\include\wx\msw\filedlg.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\wx230\include\wx\filefn.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\wx230\include\wx\font.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\wx230\include\wx\msw\font.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\wx230\include\wx\fontenc.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\wx230\include\wx\gauge.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\wx230\include\wx\msw\gauge.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\wx230\include\wx\msw\gauge95.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\wx230\include\wx\gdicmn.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\wx230\include\wx\msw\gdiimage.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\wx230\include\wx\gdiobj.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\wx230\include\wx\msw\gdiobj.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\wx230\include\wx\hash.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\wx230\include\wx\icon.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\wx230\include\wx\msw\icon.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\wx230\include\wx\intl.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\wx230\include\wx\ioswrap.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\wx230\include\wx\layout.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\wx230\include\wx\listbox.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\wx230\include\wx\msw\listbox.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\wx230\include\wx\log.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\wx230\include\wx\longlong.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\wx230\include\wx\mdi.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\wx230\include\wx\msw\mdi.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\wx230\include\wx\menu.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\wx230\include\wx\msw\menu.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\wx230\include\wx\menuitem.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\wx230\include\wx\msw\menuitem.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\wx230\include\wx\module.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\wx230\include\wx\msgdlg.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\wx230\include\wx\msw\msgdlg.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\wx230\include\wx\object.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\wx230\include\wx\ownerdrw.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\wx230\include\wx\msw\palette.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\wx230\include\wx\palette.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\wx230\include\wx\panel.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\wx230\include\wx\generic\panelg.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\wx230\include\wx\msw\pen.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\wx230\include\wx\pen.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\wx230\include\wx\msw\radiobox.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\wx230\include\wx\radiobox.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\wx230\include\wx\msw\radiobut.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\wx230\include\wx\radiobut.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\wx230\include\wx\msw\region.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\wx230\include\wx\region.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\wx230\include\wx\msw\scrolbar.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\wx230\include\wx\scrolbar.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\wx230\include\wx\generic\scrolwin.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\wx230\include\wx\scrolwin.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\wx230\include\wx\msw\settings.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\wx230\include\wx\settings.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\wx230\include\wx\msw\setup.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\wx230\include\wx\setup.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\wx230\include\wx\sizer.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\wx230\include\wx\msw\slider.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\wx230\include\wx\slider.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\wx230\include\wx\msw\slider95.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\wx230\include\wx\msw\statbmp.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\wx230\include\wx\statbmp.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\wx230\include\wx\msw\statbox.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\wx230\include\wx\statbox.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\wx230\include\wx\msw\statbr95.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\wx230\include\wx\msw\stattext.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\wx230\include\wx\stattext.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\wx230\include\wx\statusbr.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\wx230\include\wx\strconv.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\wx230\include\wx\msw\tbar95.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\wx230\include\wx\tbarbase.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\wx230\include\wx\msw\textctrl.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\wx230\include\wx\textctrl.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\wx230\include\wx\textdlg.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\wx230\include\wx\generic\textdlgg.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\wx230\include\wx\thread.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\wx230\include\wx\msw\timer.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\wx230\include\wx\timer.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\wx230\include\wx\toolbar.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\wx230\include\wx\utils.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\wx230\include\wx\validate.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\wx230\include\wx\valtext.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\wx230\include\wx\msw\window.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\wx230\include\wx\window.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\wx230\include\wx\wx.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\wx230\include\wx\wxchar.h
# End Source File
# End Group
# Begin Group "audiere"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\audiere.h
# End Source File
# End Group
# End Target
# End Project
