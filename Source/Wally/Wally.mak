# Microsoft Developer Studio Generated NMAKE File, Format Version 4.20
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

!IF "$(CFG)" == ""
CFG=Wally - Win32 Debug
!MESSAGE No configuration specified.  Defaulting to Wally - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "Wally - Win32 Release" && "$(CFG)" != "Wally - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE on this makefile
!MESSAGE by defining the macro CFG on the command line.  For example:
!MESSAGE 
!MESSAGE NMAKE /f "Wally.mak" CFG="Wally - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Wally - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "Wally - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 
################################################################################
# Begin Project
# PROP Target_Last_Scanned "Wally - Win32 Debug"
CPP=cl.exe
RSC=rc.exe
MTL=mktyplib.exe

!IF  "$(CFG)" == "Wally - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
OUTDIR=.\Release
INTDIR=.\Release

ALL : "$(OUTDIR)\Wally.exe"

CLEAN : 
	-@erase "$(INTDIR)\24BitDibSection.obj"
	-@erase "$(INTDIR)\AddNoiseDlg.obj"
	-@erase "$(INTDIR)\BitmapArray.obj"
	-@erase "$(INTDIR)\BlendDlg.obj"
	-@erase "$(INTDIR)\BrowseChildFrm.obj"
	-@erase "$(INTDIR)\BrowseDIBList.obj"
	-@erase "$(INTDIR)\BrowseDlg.obj"
	-@erase "$(INTDIR)\BrowseDocument.obj"
	-@erase "$(INTDIR)\BrowseView.obj"
	-@erase "$(INTDIR)\BrowseWindow.obj"
	-@erase "$(INTDIR)\ChildFrm.obj"
	-@erase "$(INTDIR)\ClipboardDIB.obj"
	-@erase "$(INTDIR)\ColorPalette.obj"
	-@erase "$(INTDIR)\ColorPaletteWnd.obj"
	-@erase "$(INTDIR)\CopyTiledDlg.obj"
	-@erase "$(INTDIR)\DibSection.obj"
	-@erase "$(INTDIR)\DIBtoDocHelper.obj"
	-@erase "$(INTDIR)\EdgeDetectDlg.obj"
	-@erase "$(INTDIR)\EmbossDlg.obj"
	-@erase "$(INTDIR)\Filter.obj"
	-@erase "$(INTDIR)\FilterBCDlg.obj"
	-@erase "$(INTDIR)\GraphicsFunctions.obj"
	-@erase "$(INTDIR)\MainFrm.obj"
	-@erase "$(INTDIR)\MarbleDlg.obj"
	-@erase "$(INTDIR)\MarbleTileDlg.obj"
	-@erase "$(INTDIR)\MdiClient.obj"
	-@erase "$(INTDIR)\MiscFunctions.obj"
	-@erase "$(INTDIR)\NewWalDlg.obj"
	-@erase "$(INTDIR)\Noise.obj"
	-@erase "$(INTDIR)\PaletteDlg.obj"
	-@erase "$(INTDIR)\PaletteMismatchDialog.obj"
	-@erase "$(INTDIR)\QuakeUnrealDlg.obj"
	-@erase "$(INTDIR)\ReMip.obj"
	-@erase "$(INTDIR)\RemipDlg.obj"
	-@erase "$(INTDIR)\RotateDlg.obj"
	-@erase "$(INTDIR)\Splash.obj"
	-@erase "$(INTDIR)\Stdafx.obj"
	-@erase "$(INTDIR)\TextureInfo.obj"
	-@erase "$(INTDIR)\ToolDlg.obj"
	-@erase "$(INTDIR)\Wally.obj"
	-@erase "$(INTDIR)\Wally.res"
	-@erase "$(INTDIR)\Wallydoc.obj"
	-@erase "$(INTDIR)\WallyDocTemplate.obj"
	-@erase "$(INTDIR)\WallyPropertyPage1.obj"
	-@erase "$(INTDIR)\WallyPropertySheet.obj"
	-@erase "$(INTDIR)\WallyUndo.obj"
	-@erase "$(INTDIR)\WallyView.obj"
	-@erase "$(INTDIR)\ZoomDialog.obj"
	-@erase "$(INTDIR)\ZoomDialogBar.obj"
	-@erase "$(OUTDIR)\Wally.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MD /W3 /GX /O2 /Ob2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /YX /c
CPP_PROJ=/nologo /MD /W3 /GX /O2 /Ob2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/Wally.pch" /YX /Fo"$(INTDIR)/" /c 
CPP_OBJS=.\Release/
CPP_SBRS=.\.
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
MTL_PROJ=/nologo /D "NDEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/Wally.res" /d "NDEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/Wally.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 /nologo /subsystem:windows /machine:I386
# SUBTRACT LINK32 /debug
LINK32_FLAGS=/nologo /subsystem:windows /incremental:no\
 /pdb:"$(OUTDIR)/Wally.pdb" /machine:I386 /out:"$(OUTDIR)/Wally.exe" 
LINK32_OBJS= \
	"$(INTDIR)\24BitDibSection.obj" \
	"$(INTDIR)\AddNoiseDlg.obj" \
	"$(INTDIR)\BitmapArray.obj" \
	"$(INTDIR)\BlendDlg.obj" \
	"$(INTDIR)\BrowseChildFrm.obj" \
	"$(INTDIR)\BrowseDIBList.obj" \
	"$(INTDIR)\BrowseDlg.obj" \
	"$(INTDIR)\BrowseDocument.obj" \
	"$(INTDIR)\BrowseView.obj" \
	"$(INTDIR)\BrowseWindow.obj" \
	"$(INTDIR)\ChildFrm.obj" \
	"$(INTDIR)\ClipboardDIB.obj" \
	"$(INTDIR)\ColorPalette.obj" \
	"$(INTDIR)\ColorPaletteWnd.obj" \
	"$(INTDIR)\CopyTiledDlg.obj" \
	"$(INTDIR)\DibSection.obj" \
	"$(INTDIR)\DIBtoDocHelper.obj" \
	"$(INTDIR)\EdgeDetectDlg.obj" \
	"$(INTDIR)\EmbossDlg.obj" \
	"$(INTDIR)\Filter.obj" \
	"$(INTDIR)\FilterBCDlg.obj" \
	"$(INTDIR)\GraphicsFunctions.obj" \
	"$(INTDIR)\MainFrm.obj" \
	"$(INTDIR)\MarbleDlg.obj" \
	"$(INTDIR)\MarbleTileDlg.obj" \
	"$(INTDIR)\MdiClient.obj" \
	"$(INTDIR)\MiscFunctions.obj" \
	"$(INTDIR)\NewWalDlg.obj" \
	"$(INTDIR)\Noise.obj" \
	"$(INTDIR)\PaletteDlg.obj" \
	"$(INTDIR)\PaletteMismatchDialog.obj" \
	"$(INTDIR)\QuakeUnrealDlg.obj" \
	"$(INTDIR)\ReMip.obj" \
	"$(INTDIR)\RemipDlg.obj" \
	"$(INTDIR)\RotateDlg.obj" \
	"$(INTDIR)\Splash.obj" \
	"$(INTDIR)\Stdafx.obj" \
	"$(INTDIR)\TextureInfo.obj" \
	"$(INTDIR)\ToolDlg.obj" \
	"$(INTDIR)\Wally.obj" \
	"$(INTDIR)\Wally.res" \
	"$(INTDIR)\Wallydoc.obj" \
	"$(INTDIR)\WallyDocTemplate.obj" \
	"$(INTDIR)\WallyPropertyPage1.obj" \
	"$(INTDIR)\WallyPropertySheet.obj" \
	"$(INTDIR)\WallyUndo.obj" \
	"$(INTDIR)\WallyView.obj" \
	"$(INTDIR)\ZoomDialog.obj" \
	"$(INTDIR)\ZoomDialogBar.obj"

"$(OUTDIR)\Wally.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "Wally - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
OUTDIR=.\Debug
INTDIR=.\Debug

ALL : "$(OUTDIR)\Wally.exe" "$(OUTDIR)\Wally.bsc"

CLEAN : 
	-@erase "$(INTDIR)\24BitDibSection.obj"
	-@erase "$(INTDIR)\24BitDibSection.sbr"
	-@erase "$(INTDIR)\AddNoiseDlg.obj"
	-@erase "$(INTDIR)\AddNoiseDlg.sbr"
	-@erase "$(INTDIR)\BitmapArray.obj"
	-@erase "$(INTDIR)\BitmapArray.sbr"
	-@erase "$(INTDIR)\BlendDlg.obj"
	-@erase "$(INTDIR)\BlendDlg.sbr"
	-@erase "$(INTDIR)\BrowseChildFrm.obj"
	-@erase "$(INTDIR)\BrowseChildFrm.sbr"
	-@erase "$(INTDIR)\BrowseDIBList.obj"
	-@erase "$(INTDIR)\BrowseDIBList.sbr"
	-@erase "$(INTDIR)\BrowseDlg.obj"
	-@erase "$(INTDIR)\BrowseDlg.sbr"
	-@erase "$(INTDIR)\BrowseDocument.obj"
	-@erase "$(INTDIR)\BrowseDocument.sbr"
	-@erase "$(INTDIR)\BrowseView.obj"
	-@erase "$(INTDIR)\BrowseView.sbr"
	-@erase "$(INTDIR)\BrowseWindow.obj"
	-@erase "$(INTDIR)\BrowseWindow.sbr"
	-@erase "$(INTDIR)\ChildFrm.obj"
	-@erase "$(INTDIR)\ChildFrm.sbr"
	-@erase "$(INTDIR)\ClipboardDIB.obj"
	-@erase "$(INTDIR)\ClipboardDIB.sbr"
	-@erase "$(INTDIR)\ColorPalette.obj"
	-@erase "$(INTDIR)\ColorPalette.sbr"
	-@erase "$(INTDIR)\ColorPaletteWnd.obj"
	-@erase "$(INTDIR)\ColorPaletteWnd.sbr"
	-@erase "$(INTDIR)\CopyTiledDlg.obj"
	-@erase "$(INTDIR)\CopyTiledDlg.sbr"
	-@erase "$(INTDIR)\DibSection.obj"
	-@erase "$(INTDIR)\DibSection.sbr"
	-@erase "$(INTDIR)\DIBtoDocHelper.obj"
	-@erase "$(INTDIR)\DIBtoDocHelper.sbr"
	-@erase "$(INTDIR)\EdgeDetectDlg.obj"
	-@erase "$(INTDIR)\EdgeDetectDlg.sbr"
	-@erase "$(INTDIR)\EmbossDlg.obj"
	-@erase "$(INTDIR)\EmbossDlg.sbr"
	-@erase "$(INTDIR)\Filter.obj"
	-@erase "$(INTDIR)\Filter.sbr"
	-@erase "$(INTDIR)\FilterBCDlg.obj"
	-@erase "$(INTDIR)\FilterBCDlg.sbr"
	-@erase "$(INTDIR)\GraphicsFunctions.obj"
	-@erase "$(INTDIR)\GraphicsFunctions.sbr"
	-@erase "$(INTDIR)\MainFrm.obj"
	-@erase "$(INTDIR)\MainFrm.sbr"
	-@erase "$(INTDIR)\MarbleDlg.obj"
	-@erase "$(INTDIR)\MarbleDlg.sbr"
	-@erase "$(INTDIR)\MarbleTileDlg.obj"
	-@erase "$(INTDIR)\MarbleTileDlg.sbr"
	-@erase "$(INTDIR)\MdiClient.obj"
	-@erase "$(INTDIR)\MdiClient.sbr"
	-@erase "$(INTDIR)\MiscFunctions.obj"
	-@erase "$(INTDIR)\MiscFunctions.sbr"
	-@erase "$(INTDIR)\NewWalDlg.obj"
	-@erase "$(INTDIR)\NewWalDlg.sbr"
	-@erase "$(INTDIR)\Noise.obj"
	-@erase "$(INTDIR)\Noise.sbr"
	-@erase "$(INTDIR)\PaletteDlg.obj"
	-@erase "$(INTDIR)\PaletteDlg.sbr"
	-@erase "$(INTDIR)\PaletteMismatchDialog.obj"
	-@erase "$(INTDIR)\PaletteMismatchDialog.sbr"
	-@erase "$(INTDIR)\QuakeUnrealDlg.obj"
	-@erase "$(INTDIR)\QuakeUnrealDlg.sbr"
	-@erase "$(INTDIR)\ReMip.obj"
	-@erase "$(INTDIR)\ReMip.sbr"
	-@erase "$(INTDIR)\RemipDlg.obj"
	-@erase "$(INTDIR)\RemipDlg.sbr"
	-@erase "$(INTDIR)\RotateDlg.obj"
	-@erase "$(INTDIR)\RotateDlg.sbr"
	-@erase "$(INTDIR)\Splash.obj"
	-@erase "$(INTDIR)\Splash.sbr"
	-@erase "$(INTDIR)\Stdafx.obj"
	-@erase "$(INTDIR)\Stdafx.sbr"
	-@erase "$(INTDIR)\TextureInfo.obj"
	-@erase "$(INTDIR)\TextureInfo.sbr"
	-@erase "$(INTDIR)\ToolDlg.obj"
	-@erase "$(INTDIR)\ToolDlg.sbr"
	-@erase "$(INTDIR)\vc40.idb"
	-@erase "$(INTDIR)\vc40.pdb"
	-@erase "$(INTDIR)\Wally.obj"
	-@erase "$(INTDIR)\Wally.res"
	-@erase "$(INTDIR)\Wally.sbr"
	-@erase "$(INTDIR)\Wallydoc.obj"
	-@erase "$(INTDIR)\Wallydoc.sbr"
	-@erase "$(INTDIR)\WallyDocTemplate.obj"
	-@erase "$(INTDIR)\WallyDocTemplate.sbr"
	-@erase "$(INTDIR)\WallyPropertyPage1.obj"
	-@erase "$(INTDIR)\WallyPropertyPage1.sbr"
	-@erase "$(INTDIR)\WallyPropertySheet.obj"
	-@erase "$(INTDIR)\WallyPropertySheet.sbr"
	-@erase "$(INTDIR)\WallyUndo.obj"
	-@erase "$(INTDIR)\WallyUndo.sbr"
	-@erase "$(INTDIR)\WallyView.obj"
	-@erase "$(INTDIR)\WallyView.sbr"
	-@erase "$(INTDIR)\ZoomDialog.obj"
	-@erase "$(INTDIR)\ZoomDialog.sbr"
	-@erase "$(INTDIR)\ZoomDialogBar.obj"
	-@erase "$(INTDIR)\ZoomDialogBar.sbr"
	-@erase "$(OUTDIR)\Wally.bsc"
	-@erase "$(OUTDIR)\Wally.exe"
	-@erase "$(OUTDIR)\Wally.ilk"
	-@erase "$(OUTDIR)\Wally.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR /YX /c
CPP_PROJ=/nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /D "_AFXDLL" /D "_MBCS" /FR"$(INTDIR)/" /Fp"$(INTDIR)/Wally.pch" /YX\
 /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c 
CPP_OBJS=.\Debug/
CPP_SBRS=.\Debug/
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
MTL_PROJ=/nologo /D "_DEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/Wally.res" /d "_DEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/Wally.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\24BitDibSection.sbr" \
	"$(INTDIR)\AddNoiseDlg.sbr" \
	"$(INTDIR)\BitmapArray.sbr" \
	"$(INTDIR)\BlendDlg.sbr" \
	"$(INTDIR)\BrowseChildFrm.sbr" \
	"$(INTDIR)\BrowseDIBList.sbr" \
	"$(INTDIR)\BrowseDlg.sbr" \
	"$(INTDIR)\BrowseDocument.sbr" \
	"$(INTDIR)\BrowseView.sbr" \
	"$(INTDIR)\BrowseWindow.sbr" \
	"$(INTDIR)\ChildFrm.sbr" \
	"$(INTDIR)\ClipboardDIB.sbr" \
	"$(INTDIR)\ColorPalette.sbr" \
	"$(INTDIR)\ColorPaletteWnd.sbr" \
	"$(INTDIR)\CopyTiledDlg.sbr" \
	"$(INTDIR)\DibSection.sbr" \
	"$(INTDIR)\DIBtoDocHelper.sbr" \
	"$(INTDIR)\EdgeDetectDlg.sbr" \
	"$(INTDIR)\EmbossDlg.sbr" \
	"$(INTDIR)\Filter.sbr" \
	"$(INTDIR)\FilterBCDlg.sbr" \
	"$(INTDIR)\GraphicsFunctions.sbr" \
	"$(INTDIR)\MainFrm.sbr" \
	"$(INTDIR)\MarbleDlg.sbr" \
	"$(INTDIR)\MarbleTileDlg.sbr" \
	"$(INTDIR)\MdiClient.sbr" \
	"$(INTDIR)\MiscFunctions.sbr" \
	"$(INTDIR)\NewWalDlg.sbr" \
	"$(INTDIR)\Noise.sbr" \
	"$(INTDIR)\PaletteDlg.sbr" \
	"$(INTDIR)\PaletteMismatchDialog.sbr" \
	"$(INTDIR)\QuakeUnrealDlg.sbr" \
	"$(INTDIR)\ReMip.sbr" \
	"$(INTDIR)\RemipDlg.sbr" \
	"$(INTDIR)\RotateDlg.sbr" \
	"$(INTDIR)\Splash.sbr" \
	"$(INTDIR)\Stdafx.sbr" \
	"$(INTDIR)\TextureInfo.sbr" \
	"$(INTDIR)\ToolDlg.sbr" \
	"$(INTDIR)\Wally.sbr" \
	"$(INTDIR)\Wallydoc.sbr" \
	"$(INTDIR)\WallyDocTemplate.sbr" \
	"$(INTDIR)\WallyPropertyPage1.sbr" \
	"$(INTDIR)\WallyPropertySheet.sbr" \
	"$(INTDIR)\WallyUndo.sbr" \
	"$(INTDIR)\WallyView.sbr" \
	"$(INTDIR)\ZoomDialog.sbr" \
	"$(INTDIR)\ZoomDialogBar.sbr"

"$(OUTDIR)\Wally.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386
# ADD LINK32 /nologo /subsystem:windows /debug /machine:I386
LINK32_FLAGS=/nologo /subsystem:windows /incremental:yes\
 /pdb:"$(OUTDIR)/Wally.pdb" /debug /machine:I386 /out:"$(OUTDIR)/Wally.exe" 
LINK32_OBJS= \
	"$(INTDIR)\24BitDibSection.obj" \
	"$(INTDIR)\AddNoiseDlg.obj" \
	"$(INTDIR)\BitmapArray.obj" \
	"$(INTDIR)\BlendDlg.obj" \
	"$(INTDIR)\BrowseChildFrm.obj" \
	"$(INTDIR)\BrowseDIBList.obj" \
	"$(INTDIR)\BrowseDlg.obj" \
	"$(INTDIR)\BrowseDocument.obj" \
	"$(INTDIR)\BrowseView.obj" \
	"$(INTDIR)\BrowseWindow.obj" \
	"$(INTDIR)\ChildFrm.obj" \
	"$(INTDIR)\ClipboardDIB.obj" \
	"$(INTDIR)\ColorPalette.obj" \
	"$(INTDIR)\ColorPaletteWnd.obj" \
	"$(INTDIR)\CopyTiledDlg.obj" \
	"$(INTDIR)\DibSection.obj" \
	"$(INTDIR)\DIBtoDocHelper.obj" \
	"$(INTDIR)\EdgeDetectDlg.obj" \
	"$(INTDIR)\EmbossDlg.obj" \
	"$(INTDIR)\Filter.obj" \
	"$(INTDIR)\FilterBCDlg.obj" \
	"$(INTDIR)\GraphicsFunctions.obj" \
	"$(INTDIR)\MainFrm.obj" \
	"$(INTDIR)\MarbleDlg.obj" \
	"$(INTDIR)\MarbleTileDlg.obj" \
	"$(INTDIR)\MdiClient.obj" \
	"$(INTDIR)\MiscFunctions.obj" \
	"$(INTDIR)\NewWalDlg.obj" \
	"$(INTDIR)\Noise.obj" \
	"$(INTDIR)\PaletteDlg.obj" \
	"$(INTDIR)\PaletteMismatchDialog.obj" \
	"$(INTDIR)\QuakeUnrealDlg.obj" \
	"$(INTDIR)\ReMip.obj" \
	"$(INTDIR)\RemipDlg.obj" \
	"$(INTDIR)\RotateDlg.obj" \
	"$(INTDIR)\Splash.obj" \
	"$(INTDIR)\Stdafx.obj" \
	"$(INTDIR)\TextureInfo.obj" \
	"$(INTDIR)\ToolDlg.obj" \
	"$(INTDIR)\Wally.obj" \
	"$(INTDIR)\Wally.res" \
	"$(INTDIR)\Wallydoc.obj" \
	"$(INTDIR)\WallyDocTemplate.obj" \
	"$(INTDIR)\WallyPropertyPage1.obj" \
	"$(INTDIR)\WallyPropertySheet.obj" \
	"$(INTDIR)\WallyUndo.obj" \
	"$(INTDIR)\WallyView.obj" \
	"$(INTDIR)\ZoomDialog.obj" \
	"$(INTDIR)\ZoomDialogBar.obj"

"$(OUTDIR)\Wally.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 

.c{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.c{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

################################################################################
# Begin Target

# Name "Wally - Win32 Release"
# Name "Wally - Win32 Debug"

!IF  "$(CFG)" == "Wally - Win32 Release"

!ELSEIF  "$(CFG)" == "Wally - Win32 Debug"

!ENDIF 

################################################################################
# Begin Source File

SOURCE=.\ZoomDialogBar.cpp
DEP_CPP_ZOOMD=\
	".\BrowseDIBList.h"\
	".\BrowseDocument.h"\
	".\DibSection.h"\
	".\DIBToDocHelper.h"\
	".\GraphicFormats.h"\
	".\MiscFunctions.h"\
	".\Stdafx.h"\
	".\Wally.h"\
	".\ZoomDialogBar.h"\
	

!IF  "$(CFG)" == "Wally - Win32 Release"


"$(INTDIR)\ZoomDialogBar.obj" : $(SOURCE) $(DEP_CPP_ZOOMD) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Wally - Win32 Debug"


"$(INTDIR)\ZoomDialogBar.obj" : $(SOURCE) $(DEP_CPP_ZOOMD) "$(INTDIR)"

"$(INTDIR)\ZoomDialogBar.sbr" : $(SOURCE) $(DEP_CPP_ZOOMD) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\BrowseDocument.cpp
DEP_CPP_BROWS=\
	".\BrowseDIBList.h"\
	".\BrowseDlg.h"\
	".\BrowseDocument.h"\
	".\BrowseView.h"\
	".\DibSection.h"\
	".\DIBToDocHelper.h"\
	".\GraphicFormats.h"\
	".\MiscFunctions.h"\
	".\Stdafx.h"\
	".\Wally.h"\
	

!IF  "$(CFG)" == "Wally - Win32 Release"


"$(INTDIR)\BrowseDocument.obj" : $(SOURCE) $(DEP_CPP_BROWS) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Wally - Win32 Debug"


"$(INTDIR)\BrowseDocument.obj" : $(SOURCE) $(DEP_CPP_BROWS) "$(INTDIR)"

"$(INTDIR)\BrowseDocument.sbr" : $(SOURCE) $(DEP_CPP_BROWS) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\BrowseView.cpp
DEP_CPP_BROWSE=\
	".\BrowseDIBList.h"\
	".\BrowseDocument.h"\
	".\BrowseView.h"\
	".\CopyTiledDlg.h"\
	".\DibSection.h"\
	".\DIBToDocHelper.h"\
	".\GraphicFormats.h"\
	".\MiscFunctions.h"\
	".\Stdafx.h"\
	".\TextureInfo.h"\
	".\Wally.h"\
	

!IF  "$(CFG)" == "Wally - Win32 Release"


"$(INTDIR)\BrowseView.obj" : $(SOURCE) $(DEP_CPP_BROWSE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Wally - Win32 Debug"


"$(INTDIR)\BrowseView.obj" : $(SOURCE) $(DEP_CPP_BROWSE) "$(INTDIR)"

"$(INTDIR)\BrowseView.sbr" : $(SOURCE) $(DEP_CPP_BROWSE) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\BrowseWindow.cpp
DEP_CPP_BROWSEW=\
	".\BrowseDIBList.h"\
	".\BrowseDocument.h"\
	".\BrowseWindow.h"\
	".\DibSection.h"\
	".\DIBToDocHelper.h"\
	".\GraphicFormats.h"\
	".\MiscFunctions.h"\
	".\Stdafx.h"\
	".\Wally.h"\
	

!IF  "$(CFG)" == "Wally - Win32 Release"


"$(INTDIR)\BrowseWindow.obj" : $(SOURCE) $(DEP_CPP_BROWSEW) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Wally - Win32 Debug"


"$(INTDIR)\BrowseWindow.obj" : $(SOURCE) $(DEP_CPP_BROWSEW) "$(INTDIR)"

"$(INTDIR)\BrowseWindow.sbr" : $(SOURCE) $(DEP_CPP_BROWSEW) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\ChildFrm.cpp
DEP_CPP_CHILD=\
	".\BrowseDIBList.h"\
	".\BrowseDocument.h"\
	".\ChildFrm.h"\
	".\DibSection.h"\
	".\DIBToDocHelper.h"\
	".\GraphicFormats.h"\
	".\MiscFunctions.h"\
	".\Stdafx.h"\
	".\Wally.h"\
	

!IF  "$(CFG)" == "Wally - Win32 Release"


"$(INTDIR)\ChildFrm.obj" : $(SOURCE) $(DEP_CPP_CHILD) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Wally - Win32 Debug"


"$(INTDIR)\ChildFrm.obj" : $(SOURCE) $(DEP_CPP_CHILD) "$(INTDIR)"

"$(INTDIR)\ChildFrm.sbr" : $(SOURCE) $(DEP_CPP_CHILD) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\MainFrm.cpp
DEP_CPP_MAINF=\
	".\24BitDibSection.h"\
	".\BrowseDIBList.h"\
	".\BrowseDocument.h"\
	".\ColorPalette.h"\
	".\ColorPaletteWnd.h"\
	".\DibSection.h"\
	".\DIBToDocHelper.h"\
	".\Filter.h"\
	".\GraphicFormats.h"\
	".\MainFrm.h"\
	".\MdiClient.h"\
	".\MiscFunctions.h"\
	".\Noise.h"\
	".\ReMip.h"\
	".\RemipDlg.h"\
	".\Splash.h"\
	".\Stdafx.h"\
	".\TextureFlags.h"\
	".\Wally.h"\
	".\Wallydoc.h"\
	".\WallyUndo.h"\
	".\WallyView.h"\
	

!IF  "$(CFG)" == "Wally - Win32 Release"


"$(INTDIR)\MainFrm.obj" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Wally - Win32 Debug"


"$(INTDIR)\MainFrm.obj" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"

"$(INTDIR)\MainFrm.sbr" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\NewWalDlg.cpp
DEP_CPP_NEWWA=\
	".\BrowseDIBList.h"\
	".\BrowseDocument.h"\
	".\DibSection.h"\
	".\DIBToDocHelper.h"\
	".\GraphicFormats.h"\
	".\MiscFunctions.h"\
	".\NewWalDlg.h"\
	".\Stdafx.h"\
	".\Wally.h"\
	

!IF  "$(CFG)" == "Wally - Win32 Release"


"$(INTDIR)\NewWalDlg.obj" : $(SOURCE) $(DEP_CPP_NEWWA) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Wally - Win32 Debug"


"$(INTDIR)\NewWalDlg.obj" : $(SOURCE) $(DEP_CPP_NEWWA) "$(INTDIR)"

"$(INTDIR)\NewWalDlg.sbr" : $(SOURCE) $(DEP_CPP_NEWWA) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\PaletteMismatchDialog.cpp
DEP_CPP_PALET=\
	".\BrowseDIBList.h"\
	".\BrowseDocument.h"\
	".\DibSection.h"\
	".\DIBToDocHelper.h"\
	".\GraphicFormats.h"\
	".\MiscFunctions.h"\
	".\PaletteMismatchDialog.h"\
	".\Stdafx.h"\
	".\Wally.h"\
	

!IF  "$(CFG)" == "Wally - Win32 Release"


"$(INTDIR)\PaletteMismatchDialog.obj" : $(SOURCE) $(DEP_CPP_PALET) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Wally - Win32 Debug"


"$(INTDIR)\PaletteMismatchDialog.obj" : $(SOURCE) $(DEP_CPP_PALET) "$(INTDIR)"

"$(INTDIR)\PaletteMismatchDialog.sbr" : $(SOURCE) $(DEP_CPP_PALET) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Stdafx.cpp
DEP_CPP_STDAF=\
	".\Stdafx.h"\
	

!IF  "$(CFG)" == "Wally - Win32 Release"


"$(INTDIR)\Stdafx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Wally - Win32 Debug"


"$(INTDIR)\Stdafx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"

"$(INTDIR)\Stdafx.sbr" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\TextureInfo.cpp
DEP_CPP_TEXTU=\
	".\BrowseDIBList.h"\
	".\BrowseDocument.h"\
	".\DibSection.h"\
	".\DIBToDocHelper.h"\
	".\GraphicFormats.h"\
	".\MiscFunctions.h"\
	".\Stdafx.h"\
	".\TextureFlags.h"\
	".\TextureInfo.h"\
	".\Wally.h"\
	

!IF  "$(CFG)" == "Wally - Win32 Release"


"$(INTDIR)\TextureInfo.obj" : $(SOURCE) $(DEP_CPP_TEXTU) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Wally - Win32 Debug"


"$(INTDIR)\TextureInfo.obj" : $(SOURCE) $(DEP_CPP_TEXTU) "$(INTDIR)"

"$(INTDIR)\TextureInfo.sbr" : $(SOURCE) $(DEP_CPP_TEXTU) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Wally.cpp
DEP_CPP_WALLY=\
	".\24BitDibSection.h"\
	".\BatchDlg.h"\
	".\BrowseChildFrm.h"\
	".\BrowseDIBList.h"\
	".\BrowseDocument.h"\
	".\BrowseView.h"\
	".\ChildFrm.h"\
	".\ClipboardDIB.h"\
	".\ColorPalette.h"\
	".\ColorPaletteWnd.h"\
	".\DibSection.h"\
	".\DIBToDocHelper.h"\
	".\Filter.h"\
	".\GraphicFormats.h"\
	".\GraphicsFunctions.h"\
	".\MainFrm.h"\
	".\MdiClient.h"\
	".\MiscFunctions.h"\
	".\Noise.h"\
	".\ReMip.h"\
	".\Splash.h"\
	".\Stdafx.h"\
	".\TextureFlags.h"\
	".\Wally.h"\
	".\Wallydoc.h"\
	".\WallyDocTemplate.h"\
	".\WallyOptions.h"\
	".\WallyPropertyPage1.h"\
	".\WallyPropertySheet.h"\
	".\WallyUndo.h"\
	".\WallyView.h"\
	

!IF  "$(CFG)" == "Wally - Win32 Release"


"$(INTDIR)\Wally.obj" : $(SOURCE) $(DEP_CPP_WALLY) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Wally - Win32 Debug"


"$(INTDIR)\Wally.obj" : $(SOURCE) $(DEP_CPP_WALLY) "$(INTDIR)"

"$(INTDIR)\Wally.sbr" : $(SOURCE) $(DEP_CPP_WALLY) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Wally.rc
DEP_RSC_WALLY_=\
	".\res\amount.bmp"\
	".\res\brush_sh.bmp"\
	".\res\cursor1.cur"\
	".\res\dropper_.cur"\
	".\res\icon1.ico"\
	".\res\idr_brow.ico"\
	".\res\imageedi.bmp"\
	".\res\options.bmp"\
	".\res\paint_cu.cur"\
	".\res\ReMip.bmp"\
	".\res\Splsh16.bmp"\
	".\res\Toolbar.bmp"\
	".\res\Wally.ico"\
	".\res\Wally.rc2"\
	".\res\WallyDoc.ico"\
	".\res\width.bmp"\
	

"$(INTDIR)\Wally.res" : $(SOURCE) $(DEP_RSC_WALLY_) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\Wallydoc.cpp
DEP_CPP_WALLYD=\
	".\24BitDibSection.h"\
	".\BrowseDIBList.h"\
	".\BrowseDocument.h"\
	".\DibSection.h"\
	".\DIBToDocHelper.h"\
	".\GraphicFormats.h"\
	".\GraphicsFunctions.h"\
	".\MiscFunctions.h"\
	".\NewWalDlg.h"\
	".\ReMip.h"\
	".\Stdafx.h"\
	".\TextureFlags.h"\
	".\TextureInfo.h"\
	".\ToolDlg.h"\
	".\Wally.h"\
	".\Wallydoc.h"\
	".\WallyUndo.h"\
	".\WallyView.h"\
	

!IF  "$(CFG)" == "Wally - Win32 Release"


"$(INTDIR)\Wallydoc.obj" : $(SOURCE) $(DEP_CPP_WALLYD) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Wally - Win32 Debug"


"$(INTDIR)\Wallydoc.obj" : $(SOURCE) $(DEP_CPP_WALLYD) "$(INTDIR)"

"$(INTDIR)\Wallydoc.sbr" : $(SOURCE) $(DEP_CPP_WALLYD) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\ZoomDialog.cpp
DEP_CPP_ZOOMDI=\
	".\BrowseDIBList.h"\
	".\BrowseDocument.h"\
	".\DibSection.h"\
	".\DIBToDocHelper.h"\
	".\GraphicFormats.h"\
	".\MiscFunctions.h"\
	".\Stdafx.h"\
	".\Wally.h"\
	".\ZoomDialog.h"\
	

!IF  "$(CFG)" == "Wally - Win32 Release"


"$(INTDIR)\ZoomDialog.obj" : $(SOURCE) $(DEP_CPP_ZOOMDI) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Wally - Win32 Debug"


"$(INTDIR)\ZoomDialog.obj" : $(SOURCE) $(DEP_CPP_ZOOMDI) "$(INTDIR)"

"$(INTDIR)\ZoomDialog.sbr" : $(SOURCE) $(DEP_CPP_ZOOMDI) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\BrowseChildFrm.cpp
DEP_CPP_BROWSEC=\
	".\BrowseChildFrm.h"\
	".\BrowseDIBList.h"\
	".\BrowseDocument.h"\
	".\DibSection.h"\
	".\DIBToDocHelper.h"\
	".\GraphicFormats.h"\
	".\MiscFunctions.h"\
	".\Stdafx.h"\
	".\Wally.h"\
	

!IF  "$(CFG)" == "Wally - Win32 Release"


"$(INTDIR)\BrowseChildFrm.obj" : $(SOURCE) $(DEP_CPP_BROWSEC) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Wally - Win32 Debug"


"$(INTDIR)\BrowseChildFrm.obj" : $(SOURCE) $(DEP_CPP_BROWSEC) "$(INTDIR)"

"$(INTDIR)\BrowseChildFrm.sbr" : $(SOURCE) $(DEP_CPP_BROWSEC) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\RemipDlg.cpp
DEP_CPP_REMIP=\
	".\BrowseDIBList.h"\
	".\BrowseDocument.h"\
	".\DibSection.h"\
	".\DIBToDocHelper.h"\
	".\GraphicFormats.h"\
	".\MiscFunctions.h"\
	".\ReMip.h"\
	".\RemipDlg.h"\
	".\Stdafx.h"\
	".\Wally.h"\
	

!IF  "$(CFG)" == "Wally - Win32 Release"


"$(INTDIR)\RemipDlg.obj" : $(SOURCE) $(DEP_CPP_REMIP) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Wally - Win32 Debug"


"$(INTDIR)\RemipDlg.obj" : $(SOURCE) $(DEP_CPP_REMIP) "$(INTDIR)"

"$(INTDIR)\RemipDlg.sbr" : $(SOURCE) $(DEP_CPP_REMIP) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\WallyUndo.cpp
DEP_CPP_WALLYU=\
	".\24BitDibSection.h"\
	".\BrowseDIBList.h"\
	".\BrowseDocument.h"\
	".\DibSection.h"\
	".\DIBToDocHelper.h"\
	".\GraphicFormats.h"\
	".\MiscFunctions.h"\
	".\Stdafx.h"\
	".\TextureFlags.h"\
	".\Wally.h"\
	".\Wallydoc.h"\
	".\WallyUndo.h"\
	".\WallyView.h"\
	

!IF  "$(CFG)" == "Wally - Win32 Release"


"$(INTDIR)\WallyUndo.obj" : $(SOURCE) $(DEP_CPP_WALLYU) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Wally - Win32 Debug"


"$(INTDIR)\WallyUndo.obj" : $(SOURCE) $(DEP_CPP_WALLYU) "$(INTDIR)"

"$(INTDIR)\WallyUndo.sbr" : $(SOURCE) $(DEP_CPP_WALLYU) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\ReMip.cpp
DEP_CPP_REMIP_=\
	".\ReMip.h"\
	".\Stdafx.h"\
	"C:\Src\LiteDLX\Common\Bspfile.h"\
	"C:\Src\LiteDLX\Common\Cmdlib.h"\
	"C:\Src\LiteDLX\Common\Mathlib.h"\
	"C:\Src\LiteDLX\Common\Wadlib.h"\
	
NODEP_CPP_REMIP_=\
	".\common\bspfile.h"\
	".\common\cmdlib.h"\
	".\common\mathlib.h"\
	".\wadlib.h"\
	

!IF  "$(CFG)" == "Wally - Win32 Release"


"$(INTDIR)\ReMip.obj" : $(SOURCE) $(DEP_CPP_REMIP_) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Wally - Win32 Debug"


"$(INTDIR)\ReMip.obj" : $(SOURCE) $(DEP_CPP_REMIP_) "$(INTDIR)"

"$(INTDIR)\ReMip.sbr" : $(SOURCE) $(DEP_CPP_REMIP_) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\ClipboardDIB.cpp
DEP_CPP_CLIPB=\
	".\BrowseDIBList.h"\
	".\BrowseDocument.h"\
	".\ClipboardDIB.h"\
	".\DibSection.h"\
	".\DIBToDocHelper.h"\
	".\GraphicFormats.h"\
	".\MiscFunctions.h"\
	".\Stdafx.h"\
	".\Wally.h"\
	

!IF  "$(CFG)" == "Wally - Win32 Release"


"$(INTDIR)\ClipboardDIB.obj" : $(SOURCE) $(DEP_CPP_CLIPB) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Wally - Win32 Debug"


"$(INTDIR)\ClipboardDIB.obj" : $(SOURCE) $(DEP_CPP_CLIPB) "$(INTDIR)"

"$(INTDIR)\ClipboardDIB.sbr" : $(SOURCE) $(DEP_CPP_CLIPB) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\DibSection.cpp
DEP_CPP_DIBSE=\
	".\BrowseDIBList.h"\
	".\BrowseDocument.h"\
	".\DibSection.h"\
	".\DIBToDocHelper.h"\
	".\GraphicFormats.h"\
	".\MiscFunctions.h"\
	".\Stdafx.h"\
	".\Wally.h"\
	

!IF  "$(CFG)" == "Wally - Win32 Release"


"$(INTDIR)\DibSection.obj" : $(SOURCE) $(DEP_CPP_DIBSE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Wally - Win32 Debug"


"$(INTDIR)\DibSection.obj" : $(SOURCE) $(DEP_CPP_DIBSE) "$(INTDIR)"

"$(INTDIR)\DibSection.sbr" : $(SOURCE) $(DEP_CPP_DIBSE) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\GraphicsFunctions.cpp
DEP_CPP_GRAPH=\
	".\BrowseDIBList.h"\
	".\BrowseDocument.h"\
	".\DibSection.h"\
	".\DIBToDocHelper.h"\
	".\GraphicFormats.h"\
	".\GraphicsFunctions.h"\
	".\MiscFunctions.h"\
	".\ReMip.h"\
	".\Stdafx.h"\
	".\Wally.h"\
	

!IF  "$(CFG)" == "Wally - Win32 Release"


"$(INTDIR)\GraphicsFunctions.obj" : $(SOURCE) $(DEP_CPP_GRAPH) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Wally - Win32 Debug"


"$(INTDIR)\GraphicsFunctions.obj" : $(SOURCE) $(DEP_CPP_GRAPH) "$(INTDIR)"

"$(INTDIR)\GraphicsFunctions.sbr" : $(SOURCE) $(DEP_CPP_GRAPH) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\MiscFunctions.cpp
DEP_CPP_MISCF=\
	".\MiscFunctions.h"\
	".\Stdafx.h"\
	

!IF  "$(CFG)" == "Wally - Win32 Release"


"$(INTDIR)\MiscFunctions.obj" : $(SOURCE) $(DEP_CPP_MISCF) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Wally - Win32 Debug"


"$(INTDIR)\MiscFunctions.obj" : $(SOURCE) $(DEP_CPP_MISCF) "$(INTDIR)"

"$(INTDIR)\MiscFunctions.sbr" : $(SOURCE) $(DEP_CPP_MISCF) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\WallyDocTemplate.cpp
DEP_CPP_WALLYDO=\
	".\24BitDibSection.h"\
	".\DibSection.h"\
	".\DIBToDocHelper.h"\
	".\GraphicFormats.h"\
	".\GraphicsFunctions.h"\
	".\MiscFunctions.h"\
	".\ReMip.h"\
	".\Stdafx.h"\
	".\TextureFlags.h"\
	".\Wallydoc.h"\
	".\WallyDocTemplate.h"\
	".\WallyUndo.h"\
	".\WallyView.h"\
	

!IF  "$(CFG)" == "Wally - Win32 Release"


"$(INTDIR)\WallyDocTemplate.obj" : $(SOURCE) $(DEP_CPP_WALLYDO) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Wally - Win32 Debug"


"$(INTDIR)\WallyDocTemplate.obj" : $(SOURCE) $(DEP_CPP_WALLYDO) "$(INTDIR)"

"$(INTDIR)\WallyDocTemplate.sbr" : $(SOURCE) $(DEP_CPP_WALLYDO) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\BitmapArray.cpp
DEP_CPP_BITMA=\
	".\24BitDibSection.h"\
	".\BitmapArray.h"\
	".\DibSection.h"\
	".\DIBToDocHelper.h"\
	".\GraphicFormats.h"\
	".\MiscFunctions.h"\
	".\Stdafx.h"\
	".\TextureFlags.h"\
	".\Wallydoc.h"\
	".\WallyUndo.h"\
	".\WallyView.h"\
	

!IF  "$(CFG)" == "Wally - Win32 Release"


"$(INTDIR)\BitmapArray.obj" : $(SOURCE) $(DEP_CPP_BITMA) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Wally - Win32 Debug"


"$(INTDIR)\BitmapArray.obj" : $(SOURCE) $(DEP_CPP_BITMA) "$(INTDIR)"

"$(INTDIR)\BitmapArray.sbr" : $(SOURCE) $(DEP_CPP_BITMA) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\MdiClient.cpp
DEP_CPP_MDICL=\
	".\BrowseDIBList.h"\
	".\BrowseDocument.h"\
	".\DibSection.h"\
	".\DIBToDocHelper.h"\
	".\GraphicFormats.h"\
	".\MdiClient.h"\
	".\MiscFunctions.h"\
	".\Stdafx.h"\
	".\Wally.h"\
	

!IF  "$(CFG)" == "Wally - Win32 Release"


"$(INTDIR)\MdiClient.obj" : $(SOURCE) $(DEP_CPP_MDICL) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Wally - Win32 Debug"


"$(INTDIR)\MdiClient.obj" : $(SOURCE) $(DEP_CPP_MDICL) "$(INTDIR)"

"$(INTDIR)\MdiClient.sbr" : $(SOURCE) $(DEP_CPP_MDICL) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\BrowseDIBList.cpp
DEP_CPP_BROWSED=\
	".\BrowseDIBList.h"\
	".\BrowseDocument.h"\
	".\DibSection.h"\
	".\DIBToDocHelper.h"\
	".\GraphicFormats.h"\
	".\MiscFunctions.h"\
	".\Stdafx.h"\
	".\Wally.h"\
	

!IF  "$(CFG)" == "Wally - Win32 Release"


"$(INTDIR)\BrowseDIBList.obj" : $(SOURCE) $(DEP_CPP_BROWSED) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Wally - Win32 Debug"


"$(INTDIR)\BrowseDIBList.obj" : $(SOURCE) $(DEP_CPP_BROWSED) "$(INTDIR)"

"$(INTDIR)\BrowseDIBList.sbr" : $(SOURCE) $(DEP_CPP_BROWSED) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Splash.cpp
DEP_CPP_SPLAS=\
	".\Splash.h"\
	".\Stdafx.h"\
	

!IF  "$(CFG)" == "Wally - Win32 Release"


"$(INTDIR)\Splash.obj" : $(SOURCE) $(DEP_CPP_SPLAS) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Wally - Win32 Debug"


"$(INTDIR)\Splash.obj" : $(SOURCE) $(DEP_CPP_SPLAS) "$(INTDIR)"

"$(INTDIR)\Splash.sbr" : $(SOURCE) $(DEP_CPP_SPLAS) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\WallyPropertyPage1.cpp
DEP_CPP_WALLYP=\
	".\24BitDibSection.h"\
	".\BrowseDIBList.h"\
	".\BrowseDocument.h"\
	".\DibSection.h"\
	".\DIBToDocHelper.h"\
	".\GraphicFormats.h"\
	".\MiscFunctions.h"\
	".\ReMip.h"\
	".\Stdafx.h"\
	".\TextureFlags.h"\
	".\Wally.h"\
	".\Wallydoc.h"\
	".\WallyDocTemplate.h"\
	".\WallyPropertyPage1.h"\
	".\WallyUndo.h"\
	".\WallyView.h"\
	

!IF  "$(CFG)" == "Wally - Win32 Release"


"$(INTDIR)\WallyPropertyPage1.obj" : $(SOURCE) $(DEP_CPP_WALLYP) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Wally - Win32 Debug"


"$(INTDIR)\WallyPropertyPage1.obj" : $(SOURCE) $(DEP_CPP_WALLYP) "$(INTDIR)"

"$(INTDIR)\WallyPropertyPage1.sbr" : $(SOURCE) $(DEP_CPP_WALLYP) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\WallyPropertySheet.cpp
DEP_CPP_WALLYPR=\
	".\Stdafx.h"\
	".\WallyPropertyPage1.h"\
	".\WallyPropertySheet.h"\
	

!IF  "$(CFG)" == "Wally - Win32 Release"


"$(INTDIR)\WallyPropertySheet.obj" : $(SOURCE) $(DEP_CPP_WALLYPR) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Wally - Win32 Debug"


"$(INTDIR)\WallyPropertySheet.obj" : $(SOURCE) $(DEP_CPP_WALLYPR) "$(INTDIR)"

"$(INTDIR)\WallyPropertySheet.sbr" : $(SOURCE) $(DEP_CPP_WALLYPR) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\ColorPalette.cpp
DEP_CPP_COLOR=\
	".\BrowseDIBList.h"\
	".\BrowseDocument.h"\
	".\ColorPalette.h"\
	".\ColorPaletteWnd.h"\
	".\DibSection.h"\
	".\DIBToDocHelper.h"\
	".\GraphicFormats.h"\
	".\MiscFunctions.h"\
	".\Stdafx.h"\
	".\Wally.h"\
	

!IF  "$(CFG)" == "Wally - Win32 Release"


"$(INTDIR)\ColorPalette.obj" : $(SOURCE) $(DEP_CPP_COLOR) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Wally - Win32 Debug"


"$(INTDIR)\ColorPalette.obj" : $(SOURCE) $(DEP_CPP_COLOR) "$(INTDIR)"

"$(INTDIR)\ColorPalette.sbr" : $(SOURCE) $(DEP_CPP_COLOR) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\ColorPaletteWnd.cpp
DEP_CPP_COLORP=\
	".\BrowseDIBList.h"\
	".\BrowseDocument.h"\
	".\ColorPaletteWnd.h"\
	".\DibSection.h"\
	".\DIBToDocHelper.h"\
	".\GraphicFormats.h"\
	".\MiscFunctions.h"\
	".\Stdafx.h"\
	".\Wally.h"\
	

!IF  "$(CFG)" == "Wally - Win32 Release"


"$(INTDIR)\ColorPaletteWnd.obj" : $(SOURCE) $(DEP_CPP_COLORP) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Wally - Win32 Debug"


"$(INTDIR)\ColorPaletteWnd.obj" : $(SOURCE) $(DEP_CPP_COLORP) "$(INTDIR)"

"$(INTDIR)\ColorPaletteWnd.sbr" : $(SOURCE) $(DEP_CPP_COLORP) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\CopyTiledDlg.cpp
DEP_CPP_COPYT=\
	".\BrowseDIBList.h"\
	".\BrowseDocument.h"\
	".\CopyTiledDlg.h"\
	".\DibSection.h"\
	".\DIBToDocHelper.h"\
	".\GraphicFormats.h"\
	".\MiscFunctions.h"\
	".\Stdafx.h"\
	".\Wally.h"\
	

!IF  "$(CFG)" == "Wally - Win32 Release"


"$(INTDIR)\CopyTiledDlg.obj" : $(SOURCE) $(DEP_CPP_COPYT) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Wally - Win32 Debug"


"$(INTDIR)\CopyTiledDlg.obj" : $(SOURCE) $(DEP_CPP_COPYT) "$(INTDIR)"

"$(INTDIR)\CopyTiledDlg.sbr" : $(SOURCE) $(DEP_CPP_COPYT) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\ToolDlg.cpp
DEP_CPP_TOOLD=\
	".\BrowseDIBList.h"\
	".\BrowseDocument.h"\
	".\DibSection.h"\
	".\DIBToDocHelper.h"\
	".\GraphicFormats.h"\
	".\MiscFunctions.h"\
	".\Stdafx.h"\
	".\ToolDlg.h"\
	".\Wally.h"\
	

!IF  "$(CFG)" == "Wally - Win32 Release"


"$(INTDIR)\ToolDlg.obj" : $(SOURCE) $(DEP_CPP_TOOLD) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Wally - Win32 Debug"


"$(INTDIR)\ToolDlg.obj" : $(SOURCE) $(DEP_CPP_TOOLD) "$(INTDIR)"

"$(INTDIR)\ToolDlg.sbr" : $(SOURCE) $(DEP_CPP_TOOLD) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\WallyView.cpp

!IF  "$(CFG)" == "Wally - Win32 Release"

DEP_CPP_WALLYV=\
	".\24BitDibSection.h"\
	".\BrowseDIBList.h"\
	".\BrowseDocument.h"\
	".\ClipboardDIB.h"\
	".\ColorPalette.h"\
	".\ColorPaletteWnd.h"\
	".\CopyTiledDlg.h"\
	".\DibSection.h"\
	".\DIBToDocHelper.h"\
	".\Filter.h"\
	".\GraphicFormats.h"\
	".\GraphicsFunctions.h"\
	".\MainFrm.h"\
	".\MdiClient.h"\
	".\MiscFunctions.h"\
	".\Noise.h"\
	".\QuakeUnrealDlg.h"\
	".\ReMip.h"\
	".\RotateDlg.h"\
	".\Stdafx.h"\
	".\TextureFlags.h"\
	".\Wally.h"\
	".\Wallydoc.h"\
	".\WallyUndo.h"\
	".\WallyView.h"\
	

"$(INTDIR)\WallyView.obj" : $(SOURCE) $(DEP_CPP_WALLYV) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Wally - Win32 Debug"

DEP_CPP_WALLYV=\
	".\24BitDibSection.h"\
	".\BrowseDIBList.h"\
	".\BrowseDocument.h"\
	".\ClipboardDIB.h"\
	".\ColorPalette.h"\
	".\ColorPaletteWnd.h"\
	".\CopyTiledDlg.h"\
	".\DibSection.h"\
	".\DIBToDocHelper.h"\
	".\Filter.h"\
	".\GraphicFormats.h"\
	".\GraphicsFunctions.h"\
	".\MainFrm.h"\
	".\MdiClient.h"\
	".\MiscFunctions.h"\
	".\Noise.h"\
	".\QuakeUnrealDlg.h"\
	".\ReMip.h"\
	".\RotateDlg.h"\
	".\Stdafx.h"\
	".\TextureFlags.h"\
	".\Wally.h"\
	".\Wallydoc.h"\
	".\WallyUndo.h"\
	".\WallyView.h"\
	
NODEP_CPP_WALLYV=\
	".\FILTER_MARBLE"\
	

"$(INTDIR)\WallyView.obj" : $(SOURCE) $(DEP_CPP_WALLYV) "$(INTDIR)"

"$(INTDIR)\WallyView.sbr" : $(SOURCE) $(DEP_CPP_WALLYV) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\RotateDlg.cpp
DEP_CPP_ROTAT=\
	".\BrowseDIBList.h"\
	".\BrowseDocument.h"\
	".\DibSection.h"\
	".\DIBToDocHelper.h"\
	".\GraphicFormats.h"\
	".\MiscFunctions.h"\
	".\RotateDlg.h"\
	".\Stdafx.h"\
	".\Wally.h"\
	

!IF  "$(CFG)" == "Wally - Win32 Release"


"$(INTDIR)\RotateDlg.obj" : $(SOURCE) $(DEP_CPP_ROTAT) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Wally - Win32 Debug"


"$(INTDIR)\RotateDlg.obj" : $(SOURCE) $(DEP_CPP_ROTAT) "$(INTDIR)"

"$(INTDIR)\RotateDlg.sbr" : $(SOURCE) $(DEP_CPP_ROTAT) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\DIBtoDocHelper.cpp
DEP_CPP_DIBTO=\
	".\24BitDibSection.h"\
	".\BrowseDIBList.h"\
	".\BrowseDocument.h"\
	".\DibSection.h"\
	".\DIBToDocHelper.h"\
	".\GraphicFormats.h"\
	".\MiscFunctions.h"\
	".\Stdafx.h"\
	".\TextureFlags.h"\
	".\Wally.h"\
	".\Wallydoc.h"\
	".\WallyUndo.h"\
	".\WallyView.h"\
	

!IF  "$(CFG)" == "Wally - Win32 Release"


"$(INTDIR)\DIBtoDocHelper.obj" : $(SOURCE) $(DEP_CPP_DIBTO) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Wally - Win32 Debug"


"$(INTDIR)\DIBtoDocHelper.obj" : $(SOURCE) $(DEP_CPP_DIBTO) "$(INTDIR)"

"$(INTDIR)\DIBtoDocHelper.sbr" : $(SOURCE) $(DEP_CPP_DIBTO) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\24BitDibSection.cpp
DEP_CPP_24BIT=\
	".\24BitDibSection.h"\
	".\BrowseDIBList.h"\
	".\BrowseDocument.h"\
	".\DibSection.h"\
	".\DIBToDocHelper.h"\
	".\GraphicFormats.h"\
	".\MiscFunctions.h"\
	".\Stdafx.h"\
	".\Wally.h"\
	

!IF  "$(CFG)" == "Wally - Win32 Release"


"$(INTDIR)\24BitDibSection.obj" : $(SOURCE) $(DEP_CPP_24BIT) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Wally - Win32 Debug"


"$(INTDIR)\24BitDibSection.obj" : $(SOURCE) $(DEP_CPP_24BIT) "$(INTDIR)"

"$(INTDIR)\24BitDibSection.sbr" : $(SOURCE) $(DEP_CPP_24BIT) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\BrowseDlg.cpp
DEP_CPP_BROWSEDL=\
	".\BrowseDIBList.h"\
	".\BrowseDlg.h"\
	".\BrowseDocument.h"\
	".\DibSection.h"\
	".\DIBToDocHelper.h"\
	".\GraphicFormats.h"\
	".\MiscFunctions.h"\
	".\Stdafx.h"\
	".\Wally.h"\
	

!IF  "$(CFG)" == "Wally - Win32 Release"


"$(INTDIR)\BrowseDlg.obj" : $(SOURCE) $(DEP_CPP_BROWSEDL) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Wally - Win32 Debug"


"$(INTDIR)\BrowseDlg.obj" : $(SOURCE) $(DEP_CPP_BROWSEDL) "$(INTDIR)"

"$(INTDIR)\BrowseDlg.sbr" : $(SOURCE) $(DEP_CPP_BROWSEDL) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Filter.cpp
DEP_CPP_FILTE=\
	".\24BitDibSection.h"\
	".\AddNoiseDlg.h"\
	".\BlendDlg.h"\
	".\BrowseDIBList.h"\
	".\BrowseDocument.h"\
	".\DibSection.h"\
	".\DIBToDocHelper.h"\
	".\EdgeDetectDlg.h"\
	".\EmbossDlg.h"\
	".\Filter.h"\
	".\FilterBCDlg.h"\
	".\GraphicFormats.h"\
	".\marbledlg.h"\
	".\MarbleTileDlg.h"\
	".\MiscFunctions.h"\
	".\Noise.h"\
	".\ReMip.h"\
	".\Stdafx.h"\
	".\TextureFlags.h"\
	".\Wally.h"\
	".\Wallydoc.h"\
	".\WallyUndo.h"\
	".\WallyView.h"\
	

!IF  "$(CFG)" == "Wally - Win32 Release"


"$(INTDIR)\Filter.obj" : $(SOURCE) $(DEP_CPP_FILTE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Wally - Win32 Debug"


"$(INTDIR)\Filter.obj" : $(SOURCE) $(DEP_CPP_FILTE) "$(INTDIR)"

"$(INTDIR)\Filter.sbr" : $(SOURCE) $(DEP_CPP_FILTE) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\FilterBCDlg.cpp
DEP_CPP_FILTER=\
	".\BrowseDIBList.h"\
	".\BrowseDocument.h"\
	".\DibSection.h"\
	".\DIBToDocHelper.h"\
	".\Filter.h"\
	".\FilterBCDlg.h"\
	".\GraphicFormats.h"\
	".\MiscFunctions.h"\
	".\Noise.h"\
	".\Stdafx.h"\
	".\Wally.h"\
	

!IF  "$(CFG)" == "Wally - Win32 Release"


"$(INTDIR)\FilterBCDlg.obj" : $(SOURCE) $(DEP_CPP_FILTER) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Wally - Win32 Debug"


"$(INTDIR)\FilterBCDlg.obj" : $(SOURCE) $(DEP_CPP_FILTER) "$(INTDIR)"

"$(INTDIR)\FilterBCDlg.sbr" : $(SOURCE) $(DEP_CPP_FILTER) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\BlendDlg.cpp
DEP_CPP_BLEND=\
	".\BlendDlg.h"\
	".\BrowseDIBList.h"\
	".\BrowseDocument.h"\
	".\DibSection.h"\
	".\DIBToDocHelper.h"\
	".\Filter.h"\
	".\GraphicFormats.h"\
	".\MiscFunctions.h"\
	".\Noise.h"\
	".\Stdafx.h"\
	".\Wally.h"\
	

!IF  "$(CFG)" == "Wally - Win32 Release"


"$(INTDIR)\BlendDlg.obj" : $(SOURCE) $(DEP_CPP_BLEND) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Wally - Win32 Debug"


"$(INTDIR)\BlendDlg.obj" : $(SOURCE) $(DEP_CPP_BLEND) "$(INTDIR)"

"$(INTDIR)\BlendDlg.sbr" : $(SOURCE) $(DEP_CPP_BLEND) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\AddNoiseDlg.cpp
DEP_CPP_ADDNO=\
	".\AddNoiseDlg.h"\
	".\BrowseDIBList.h"\
	".\BrowseDocument.h"\
	".\DibSection.h"\
	".\DIBToDocHelper.h"\
	".\Filter.h"\
	".\GraphicFormats.h"\
	".\MiscFunctions.h"\
	".\Noise.h"\
	".\Stdafx.h"\
	".\Wally.h"\
	

!IF  "$(CFG)" == "Wally - Win32 Release"


"$(INTDIR)\AddNoiseDlg.obj" : $(SOURCE) $(DEP_CPP_ADDNO) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Wally - Win32 Debug"


"$(INTDIR)\AddNoiseDlg.obj" : $(SOURCE) $(DEP_CPP_ADDNO) "$(INTDIR)"

"$(INTDIR)\AddNoiseDlg.sbr" : $(SOURCE) $(DEP_CPP_ADDNO) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\EmbossDlg.cpp
DEP_CPP_EMBOS=\
	".\BrowseDIBList.h"\
	".\BrowseDocument.h"\
	".\DibSection.h"\
	".\DIBToDocHelper.h"\
	".\EmbossDlg.h"\
	".\Filter.h"\
	".\GraphicFormats.h"\
	".\MiscFunctions.h"\
	".\Noise.h"\
	".\Stdafx.h"\
	".\Wally.h"\
	

!IF  "$(CFG)" == "Wally - Win32 Release"


"$(INTDIR)\EmbossDlg.obj" : $(SOURCE) $(DEP_CPP_EMBOS) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Wally - Win32 Debug"


"$(INTDIR)\EmbossDlg.obj" : $(SOURCE) $(DEP_CPP_EMBOS) "$(INTDIR)"

"$(INTDIR)\EmbossDlg.sbr" : $(SOURCE) $(DEP_CPP_EMBOS) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\EdgeDetectDlg.cpp
DEP_CPP_EDGED=\
	".\BrowseDIBList.h"\
	".\BrowseDocument.h"\
	".\DibSection.h"\
	".\DIBToDocHelper.h"\
	".\EdgeDetectDlg.h"\
	".\Filter.h"\
	".\GraphicFormats.h"\
	".\MiscFunctions.h"\
	".\Noise.h"\
	".\ReMip.h"\
	".\Stdafx.h"\
	".\Wally.h"\
	

!IF  "$(CFG)" == "Wally - Win32 Release"


"$(INTDIR)\EdgeDetectDlg.obj" : $(SOURCE) $(DEP_CPP_EDGED) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Wally - Win32 Debug"


"$(INTDIR)\EdgeDetectDlg.obj" : $(SOURCE) $(DEP_CPP_EDGED) "$(INTDIR)"

"$(INTDIR)\EdgeDetectDlg.sbr" : $(SOURCE) $(DEP_CPP_EDGED) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Noise.cpp

!IF  "$(CFG)" == "Wally - Win32 Release"

DEP_CPP_NOISE=\
	".\BrowseDIBList.h"\
	".\BrowseDocument.h"\
	".\DibSection.h"\
	".\DIBToDocHelper.h"\
	".\GraphicFormats.h"\
	".\MiscFunctions.h"\
	".\Noise.h"\
	".\ReMip.h"\
	".\Stdafx.h"\
	".\Wally.h"\
	

"$(INTDIR)\Noise.obj" : $(SOURCE) $(DEP_CPP_NOISE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Wally - Win32 Debug"

DEP_CPP_NOISE=\
	".\BrowseDIBList.h"\
	".\BrowseDocument.h"\
	".\DibSection.h"\
	".\DIBToDocHelper.h"\
	".\GraphicFormats.h"\
	".\MiscFunctions.h"\
	".\Noise.h"\
	".\ReMip.h"\
	".\Stdafx.h"\
	".\Wally.h"\
	
NODEP_CPP_NOISE=\
	".\DNoise"\
	".\VecAdd"\
	

"$(INTDIR)\Noise.obj" : $(SOURCE) $(DEP_CPP_NOISE) "$(INTDIR)"

"$(INTDIR)\Noise.sbr" : $(SOURCE) $(DEP_CPP_NOISE) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\MarbleDlg.cpp
DEP_CPP_MARBL=\
	".\BrowseDIBList.h"\
	".\BrowseDocument.h"\
	".\ColorPaletteWnd.h"\
	".\DibSection.h"\
	".\DIBToDocHelper.h"\
	".\Filter.h"\
	".\GraphicFormats.h"\
	".\marbledlg.h"\
	".\MiscFunctions.h"\
	".\Noise.h"\
	".\PaletteDlg.h"\
	".\ReMip.h"\
	".\Stdafx.h"\
	".\Wally.h"\
	

!IF  "$(CFG)" == "Wally - Win32 Release"


"$(INTDIR)\MarbleDlg.obj" : $(SOURCE) $(DEP_CPP_MARBL) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Wally - Win32 Debug"


"$(INTDIR)\MarbleDlg.obj" : $(SOURCE) $(DEP_CPP_MARBL) "$(INTDIR)"

"$(INTDIR)\MarbleDlg.sbr" : $(SOURCE) $(DEP_CPP_MARBL) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\QuakeUnrealDlg.cpp
DEP_CPP_QUAKE=\
	".\BrowseDIBList.h"\
	".\BrowseDocument.h"\
	".\DibSection.h"\
	".\DIBToDocHelper.h"\
	".\GraphicFormats.h"\
	".\MiscFunctions.h"\
	".\QuakeUnrealDlg.h"\
	".\Stdafx.h"\
	".\Wally.h"\
	

!IF  "$(CFG)" == "Wally - Win32 Release"


"$(INTDIR)\QuakeUnrealDlg.obj" : $(SOURCE) $(DEP_CPP_QUAKE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Wally - Win32 Debug"


"$(INTDIR)\QuakeUnrealDlg.obj" : $(SOURCE) $(DEP_CPP_QUAKE) "$(INTDIR)"

"$(INTDIR)\QuakeUnrealDlg.sbr" : $(SOURCE) $(DEP_CPP_QUAKE) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\PaletteDlg.cpp
DEP_CPP_PALETT=\
	".\BrowseDIBList.h"\
	".\BrowseDocument.h"\
	".\ColorPaletteWnd.h"\
	".\DibSection.h"\
	".\DIBToDocHelper.h"\
	".\GraphicFormats.h"\
	".\MiscFunctions.h"\
	".\PaletteDlg.h"\
	".\Stdafx.h"\
	".\Wally.h"\
	

!IF  "$(CFG)" == "Wally - Win32 Release"


"$(INTDIR)\PaletteDlg.obj" : $(SOURCE) $(DEP_CPP_PALETT) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Wally - Win32 Debug"


"$(INTDIR)\PaletteDlg.obj" : $(SOURCE) $(DEP_CPP_PALETT) "$(INTDIR)"

"$(INTDIR)\PaletteDlg.sbr" : $(SOURCE) $(DEP_CPP_PALETT) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\MarbleTileDlg.cpp
DEP_CPP_MARBLE=\
	".\BrowseDIBList.h"\
	".\BrowseDocument.h"\
	".\ColorPaletteWnd.h"\
	".\DibSection.h"\
	".\DIBToDocHelper.h"\
	".\Filter.h"\
	".\GraphicFormats.h"\
	".\marbledlg.h"\
	".\MarbleTileDlg.h"\
	".\MiscFunctions.h"\
	".\Noise.h"\
	".\PaletteDlg.h"\
	".\ReMip.h"\
	".\Stdafx.h"\
	".\Wally.h"\
	

!IF  "$(CFG)" == "Wally - Win32 Release"


"$(INTDIR)\MarbleTileDlg.obj" : $(SOURCE) $(DEP_CPP_MARBLE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Wally - Win32 Debug"


"$(INTDIR)\MarbleTileDlg.obj" : $(SOURCE) $(DEP_CPP_MARBLE) "$(INTDIR)"

"$(INTDIR)\MarbleTileDlg.sbr" : $(SOURCE) $(DEP_CPP_MARBLE) "$(INTDIR)"


!ENDIF 

# End Source File
# End Target
# End Project
################################################################################
