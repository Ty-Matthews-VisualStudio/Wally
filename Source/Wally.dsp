# Microsoft Developer Studio Project File - Name="Wally" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=Wally - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "Wally.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Wally.mak" CFG="Wally - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Wally - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "Wally - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Wally - Win32 Release"

# PROP BASE Use_MFC 5
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 5
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "./Output"
# PROP Intermediate_Dir "./TempFiles/Release/Wally"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_WALLY" /Yu"stdafx.h" /FD /c
# SUBTRACT CPP /Fr
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 /nologo /subsystem:windows /machine:I386 /nodefaultlib:"libc.lib"
# SUBTRACT LINK32 /debug

!ELSEIF  "$(CFG)" == "Wally - Win32 Debug"

# PROP BASE Use_MFC 5
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 5
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "./Output"
# PROP Intermediate_Dir "./TempFiles/Debug/Wally"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_WALLY" /FAcs /Fr /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 /nologo /subsystem:windows /debug /machine:I386 /nodefaultlib:"libcd.lib" /out:"./Output/WallyDebug.exe" /pdbtype:sept
# SUBTRACT LINK32 /pdb:none

!ENDIF 

# Begin Target

# Name "Wally - Win32 Release"
# Name "Wally - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\AddNoiseDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\BatchAdvancedDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\BatchDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\BatchJob.cpp
# End Source File
# Begin Source File

SOURCE=.\BatchSummaryDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\BlendDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\BrowseChildFrm.cpp
# End Source File
# Begin Source File

SOURCE=.\BrowseDoc.cpp
# End Source File
# Begin Source File

SOURCE=.\BrowserCacheList.cpp
# End Source File
# Begin Source File

SOURCE=.\BrowserCacheThread.cpp
# End Source File
# Begin Source File

SOURCE=.\BrowseView.cpp
# End Source File
# Begin Source File

SOURCE=.\BrowseWorkerThread.cpp
# End Source File
# Begin Source File

SOURCE=.\BuildChildFrm.cpp
# End Source File
# Begin Source File

SOURCE=.\BuildDoc.cpp
# End Source File
# Begin Source File

SOURCE=.\BuildList.cpp
# End Source File
# Begin Source File

SOURCE=.\BuildPropertiesDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\BuildView.cpp
# End Source File
# Begin Source File

SOURCE=.\ChildFrm.cpp
# End Source File
# Begin Source File

SOURCE=.\ClipboardDIB.cpp
# End Source File
# Begin Source File

SOURCE=.\ColorDecalDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\ColorOpt.cpp
# End Source File
# Begin Source File

SOURCE=.\ColorPalette.cpp
# End Source File
# Begin Source File

SOURCE=.\ColorPaletteWnd.cpp
# End Source File
# Begin Source File

SOURCE=.\ColorTranslatorDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\CopyTiledDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\CustomResource.cpp
# End Source File
# Begin Source File

SOURCE=.\DebugChildFrm.cpp
# End Source File
# Begin Source File

SOURCE=.\DebugDibView.cpp
# End Source File
# Begin Source File

SOURCE=.\DebugDoc.cpp
# End Source File
# Begin Source File

SOURCE=.\DecalColorChooserWnd.cpp
# End Source File
# Begin Source File

SOURCE=.\DecalListBox.cpp
# End Source File
# Begin Source File

SOURCE=.\DecalToolbar.cpp
# End Source File
# Begin Source File

SOURCE=.\DelayRender.cpp
# End Source File
# Begin Source File

SOURCE=.\DibSection.cpp
# End Source File
# Begin Source File

SOURCE=.\DiffuseDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\DirectoryList.cpp
# End Source File
# Begin Source File

SOURCE=.\DuplicateNameDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\EdgeDetectDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\EmbossDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\EnlargeCanvasDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\ExplorerTreeView.cpp
# End Source File
# Begin Source File

SOURCE=.\FileList.cpp
# End Source File
# Begin Source File

SOURCE=.\Filter.cpp
# End Source File
# Begin Source File

SOURCE=.\FilterBCDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\FixUnevenLightingDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\FreeUndoDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\GamePaletteDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\GameSelectDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\Genesis3D.cpp
# End Source File
# Begin Source File

SOURCE=.\Globals.cpp
# End Source File
# Begin Source File

SOURCE=.\GraphicsFunctions.cpp
# End Source File
# Begin Source File

SOURCE=.\HalfLifePaletteDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\ImageHelper.cpp
# End Source File
# Begin Source File

SOURCE=.\ImageHelper_BMP.cpp
# End Source File
# Begin Source File

SOURCE=.\ImageHelper_JPG.cpp
# End Source File
# Begin Source File

SOURCE=.\ImageHelper_PCX.cpp
# End Source File
# Begin Source File

SOURCE=.\ImageHelper_PNG.cpp
# End Source File
# Begin Source File

SOURCE=.\ImageHelper_TEX.cpp
# End Source File
# Begin Source File

SOURCE=.\ImageHelper_TGA.cpp
# End Source File
# Begin Source File

SOURCE=.\ImageIconList.cpp
# End Source File
# Begin Source File

SOURCE=.\ImageManipulator.cpp
# End Source File
# Begin Source File

SOURCE=.\Layer.cpp
# End Source File
# Begin Source File

SOURCE=.\lzrw1.cpp
# End Source File
# Begin Source File

SOURCE=.\MainFrm.cpp
# End Source File
# Begin Source File

SOURCE=.\MarbleDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\MarbleTileDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\MdiClient.cpp
# End Source File
# Begin Source File

SOURCE=.\MemBuffer.cpp
# End Source File
# Begin Source File

SOURCE=.\MirrorDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\MiscFunctions.cpp
# End Source File
# Begin Source File

SOURCE=.\MouseWnd.cpp
# End Source File
# Begin Source File

SOURCE=.\NewImageDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\NewPackageImageDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\NewPaletteDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\NewRuleDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\Noise.cpp
# End Source File
# Begin Source File

SOURCE=.\OffsetDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\PackageAddToDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\PackageBrowseView.cpp
# End Source File
# Begin Source File

SOURCE=.\PackageChildFrm.cpp
# End Source File
# Begin Source File

SOURCE=.\PackageDoc.cpp
# End Source File
# Begin Source File

SOURCE=.\PackageExportDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\PackageListBox.cpp
# End Source File
# Begin Source File

SOURCE=.\PackagePreviewWnd.cpp
# End Source File
# Begin Source File

SOURCE=.\PackageReMipThread.cpp
# End Source File
# Begin Source File

SOURCE=.\PackageTreeControl.cpp
# End Source File
# Begin Source File

SOURCE=.\PackageView.cpp
# End Source File
# Begin Source File

SOURCE=.\PakChildFrm.cpp
# End Source File
# Begin Source File

SOURCE=.\PakDoc.cpp
# End Source File
# Begin Source File

SOURCE=.\PakExportDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\PakImportDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\PakList.cpp
# End Source File
# Begin Source File

SOURCE=.\PakListView.cpp
# End Source File
# Begin Source File

SOURCE=.\PakTreeView.cpp
# End Source File
# Begin Source File

SOURCE=.\palettedlg.cpp
# End Source File
# Begin Source File

SOURCE=.\PaletteEditorDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\PaletteNameDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\PaletteWnd.cpp
# End Source File
# Begin Source File

SOURCE=.\PalMap.cpp
# End Source File
# Begin Source File

SOURCE=.\PasteSpecialDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\PercentDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\PrefabImages.cpp
# End Source File
# Begin Source File

SOURCE=.\ProgressBar.cpp
# End Source File
# Begin Source File

SOURCE=.\QuakeUnrealDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\ReduceColorsDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\RegistryHelper.cpp
# End Source File
# Begin Source File

SOURCE=.\ReMip.cpp
# End Source File
# Begin Source File

SOURCE=.\RemipDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\RenameImageDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\ResizeDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\RotateDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\RuleList.cpp
# End Source File
# Begin Source File

SOURCE=.\RulesDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\SafeFile.cpp
# End Source File
# Begin Source File

SOURCE=.\Selection.cpp
# End Source File
# Begin Source File

SOURCE=.\SelectPakTreeDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\SharpenDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\SinFlagsColorWnd.cpp
# End Source File
# Begin Source File

SOURCE=.\SinFlagsDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\SinRuleFlagDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\SortStringArray.cpp
# End Source File
# Begin Source File

SOURCE=.\Splash.cpp
# End Source File
# Begin Source File

SOURCE=.\STabCtrl.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\TabCheckListBox.cpp
# End Source File
# Begin Source File

SOURCE=.\TextureInfo.cpp
# End Source File
# Begin Source File

SOURCE=.\ThreadJob.cpp
# End Source File
# Begin Source File

SOURCE=.\ThreadManager.cpp
# End Source File
# Begin Source File

SOURCE=.\Tool.cpp
# End Source File
# Begin Source File

SOURCE=.\ToolDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\ToolSettingsPage.cpp
# End Source File
# Begin Source File

SOURCE=.\ToolSettingsToolbar.cpp
# End Source File
# Begin Source File

SOURCE=.\WADList.cpp
# End Source File
# Begin Source File

SOURCE=.\WadMergeAddDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\WadMergeDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\WadMergeJob.cpp
# End Source File
# Begin Source File

SOURCE=.\WadMergeStatusDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\Wally.cpp
# End Source File
# Begin Source File

SOURCE=.\hlp\Wally.hpj
# End Source File
# Begin Source File

SOURCE=.\Wally.rc
# End Source File
# Begin Source File

SOURCE=.\WallyDoc.cpp
# End Source File
# Begin Source File

SOURCE=.\WallyDocTemplate.cpp
# End Source File
# Begin Source File

SOURCE=.\WallyException.cpp
# End Source File
# Begin Source File

SOURCE=.\WallyList.cpp
# End Source File
# Begin Source File

SOURCE=.\WallyPal.cpp
# End Source File
# Begin Source File

SOURCE=.\WallyPropertyPage1.cpp
# End Source File
# Begin Source File

SOURCE=.\WallyPropertySheet.cpp
# End Source File
# Begin Source File

SOURCE=.\WallyToolBar.cpp
# End Source File
# Begin Source File

SOURCE=.\WallyUndo.cpp
# End Source File
# Begin Source File

SOURCE=.\WallyView.cpp
# End Source File
# Begin Source File

SOURCE=.\WLYChildFrm.cpp
# End Source File
# Begin Source File

SOURCE=.\WLYDoc.cpp
# End Source File
# Begin Source File

SOURCE=.\WLYView.cpp
# End Source File
# Begin Source File

SOURCE=.\WndList.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\2PassScale.h
# End Source File
# Begin Source File

SOURCE=.\AddNoiseDlg.h
# End Source File
# Begin Source File

SOURCE=.\BatchAdvancedDlg.h
# End Source File
# Begin Source File

SOURCE=.\BatchDlg.h
# End Source File
# Begin Source File

SOURCE=.\BatchJob.h
# End Source File
# Begin Source File

SOURCE=.\BatchSummaryDlg.h
# End Source File
# Begin Source File

SOURCE=.\BlendDlg.h
# End Source File
# Begin Source File

SOURCE=.\BrowseChildFrm.h
# End Source File
# Begin Source File

SOURCE=.\BrowseDoc.h
# End Source File
# Begin Source File

SOURCE=.\BrowserCacheList.h
# End Source File
# Begin Source File

SOURCE=.\BrowserCacheThread.h
# End Source File
# Begin Source File

SOURCE=.\BrowseView.h
# End Source File
# Begin Source File

SOURCE=.\BrowseWorkerThread.h
# End Source File
# Begin Source File

SOURCE=.\BuildChildFrm.h
# End Source File
# Begin Source File

SOURCE=.\BuildDoc.h
# End Source File
# Begin Source File

SOURCE=.\BuildList.h
# End Source File
# Begin Source File

SOURCE=.\BuildPropertiesDlg.h
# End Source File
# Begin Source File

SOURCE=.\BuildView.h
# End Source File
# Begin Source File

SOURCE=.\ChildFrm.h
# End Source File
# Begin Source File

SOURCE=.\ClipboardDIB.h
# End Source File
# Begin Source File

SOURCE=.\ColorDecalDlg.h
# End Source File
# Begin Source File

SOURCE=.\ColorOpt.h
# End Source File
# Begin Source File

SOURCE=.\ColorPalette.h
# End Source File
# Begin Source File

SOURCE=.\ColorPaletteWnd.h
# End Source File
# Begin Source File

SOURCE=.\ColorTranslatorDlg.h
# End Source File
# Begin Source File

SOURCE=.\CopyTiledDlg.h
# End Source File
# Begin Source File

SOURCE=.\CustomResource.h
# End Source File
# Begin Source File

SOURCE=.\DebugChildFrm.h
# End Source File
# Begin Source File

SOURCE=.\DebugDibView.h
# End Source File
# Begin Source File

SOURCE=.\DebugDoc.h
# End Source File
# Begin Source File

SOURCE=.\DecalColorChooserWnd.h
# End Source File
# Begin Source File

SOURCE=.\DecalListBox.h
# End Source File
# Begin Source File

SOURCE=.\DecalToolbar.h
# End Source File
# Begin Source File

SOURCE=.\DelayRender.h
# End Source File
# Begin Source File

SOURCE=.\DibSection.h
# End Source File
# Begin Source File

SOURCE=.\DiffuseDlg.h
# End Source File
# Begin Source File

SOURCE=.\DirectoryList.h
# End Source File
# Begin Source File

SOURCE=.\DuplicateNameDlg.h
# End Source File
# Begin Source File

SOURCE=.\EdgeDetectDlg.h
# End Source File
# Begin Source File

SOURCE=.\EmbossDlg.h
# End Source File
# Begin Source File

SOURCE=.\EnlargeCanvasDlg.h
# End Source File
# Begin Source File

SOURCE=.\ExplorerTreeView.h
# End Source File
# Begin Source File

SOURCE=.\FileList.h
# End Source File
# Begin Source File

SOURCE=.\Filter.h
# End Source File
# Begin Source File

SOURCE=.\FilterBCDlg.h
# End Source File
# Begin Source File

SOURCE=.\FixUnevenLightingDlg.h
# End Source File
# Begin Source File

SOURCE=.\FreeUndoDlg.h
# End Source File
# Begin Source File

SOURCE=.\GamePaletteDlg.h
# End Source File
# Begin Source File

SOURCE=.\GameSelectDlg.h
# End Source File
# Begin Source File

SOURCE=.\Genesis3D.h
# End Source File
# Begin Source File

SOURCE=.\Globals.h
# End Source File
# Begin Source File

SOURCE=.\GraphicsFunctions.h
# End Source File
# Begin Source File

SOURCE=.\HalfLifePaletteDlg.h
# End Source File
# Begin Source File

SOURCE=.\ImageHelper.h
# End Source File
# Begin Source File

SOURCE=.\ImageIconList.h
# End Source File
# Begin Source File

SOURCE=.\ImageManipulator.h
# End Source File
# Begin Source File

SOURCE=.\Layer.h
# End Source File
# Begin Source File

SOURCE=.\MainFrm.h
# End Source File
# Begin Source File

SOURCE=.\MarbleDlg.h
# End Source File
# Begin Source File

SOURCE=.\MarbleTileDlg.h
# End Source File
# Begin Source File

SOURCE=.\MdiClient.h
# End Source File
# Begin Source File

SOURCE=.\MemBuffer.h
# End Source File
# Begin Source File

SOURCE=.\MirrorDlg.h
# End Source File
# Begin Source File

SOURCE=.\MiscFunctions.h
# End Source File
# Begin Source File

SOURCE=.\MouseWnd.h
# End Source File
# Begin Source File

SOURCE=.\NewImageDlg.h
# End Source File
# Begin Source File

SOURCE=.\NewPackageImageDlg.h
# End Source File
# Begin Source File

SOURCE=.\NewPaletteDlg.h
# End Source File
# Begin Source File

SOURCE=.\NewRuleDlg.h
# End Source File
# Begin Source File

SOURCE=.\Noise.h
# End Source File
# Begin Source File

SOURCE=.\OffsetDlg.h
# End Source File
# Begin Source File

SOURCE=.\PackageAddToDlg.h
# End Source File
# Begin Source File

SOURCE=.\PackageBrowseView.h
# End Source File
# Begin Source File

SOURCE=.\PackageChildFrm.h
# End Source File
# Begin Source File

SOURCE=.\PackageDoc.h
# End Source File
# Begin Source File

SOURCE=.\PackageExportDlg.h
# End Source File
# Begin Source File

SOURCE=.\PackageListBox.h
# End Source File
# Begin Source File

SOURCE=.\PackagePreviewWnd.h
# End Source File
# Begin Source File

SOURCE=.\PackageReMipThread.h
# End Source File
# Begin Source File

SOURCE=.\PackageTreeControl.h
# End Source File
# Begin Source File

SOURCE=.\PackageView.h
# End Source File
# Begin Source File

SOURCE=.\PakChildFrm.h
# End Source File
# Begin Source File

SOURCE=.\PakDoc.h
# End Source File
# Begin Source File

SOURCE=.\PakExportDlg.h
# End Source File
# Begin Source File

SOURCE=.\PakImportDlg.h
# End Source File
# Begin Source File

SOURCE=.\PakList.h
# End Source File
# Begin Source File

SOURCE=.\PakListView.h
# End Source File
# Begin Source File

SOURCE=.\PakTreeView.h
# End Source File
# Begin Source File

SOURCE=.\palettedlg.h
# End Source File
# Begin Source File

SOURCE=.\PaletteEditorDlg.h
# End Source File
# Begin Source File

SOURCE=.\PaletteNameDlg.h
# End Source File
# Begin Source File

SOURCE=.\PaletteWnd.h
# End Source File
# Begin Source File

SOURCE=.\PalMap.h
# End Source File
# Begin Source File

SOURCE=.\PasteSpecialDlg.h
# End Source File
# Begin Source File

SOURCE=.\PercentDlg.h
# End Source File
# Begin Source File

SOURCE=.\PrefabImages.h
# End Source File
# Begin Source File

SOURCE=.\ProgressBar.h
# End Source File
# Begin Source File

SOURCE=.\QuakeUnrealDlg.h
# End Source File
# Begin Source File

SOURCE=.\ReduceColorsDlg.h
# End Source File
# Begin Source File

SOURCE=.\RegistryHelper.h
# End Source File
# Begin Source File

SOURCE=.\ReMip.h
# End Source File
# Begin Source File

SOURCE=.\RemipDlg.h
# End Source File
# Begin Source File

SOURCE=.\RenameImageDlg.h
# End Source File
# Begin Source File

SOURCE=.\ResizeDlg.h
# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=.\RotateDlg.h
# End Source File
# Begin Source File

SOURCE=.\RuleList.h
# End Source File
# Begin Source File

SOURCE=.\RulesDlg.h
# End Source File
# Begin Source File

SOURCE=.\SafeFile.h
# End Source File
# Begin Source File

SOURCE=.\Selection.h
# End Source File
# Begin Source File

SOURCE=.\SelectPakTreeDlg.h
# End Source File
# Begin Source File

SOURCE=.\SharpenDlg.h
# End Source File
# Begin Source File

SOURCE=.\SinFlagsColorWnd.h
# End Source File
# Begin Source File

SOURCE=.\SinFlagsDlg.h
# End Source File
# Begin Source File

SOURCE=.\SinRuleFlagDlg.h
# End Source File
# Begin Source File

SOURCE=.\SortStringArray.h
# End Source File
# Begin Source File

SOURCE=.\Splash.h
# End Source File
# Begin Source File

SOURCE=.\STabCtrl.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\TabCheckListBox.h
# End Source File
# Begin Source File

SOURCE=.\TextureFlags.h
# End Source File
# Begin Source File

SOURCE=.\TextureInfo.h
# End Source File
# Begin Source File

SOURCE=.\ThreadJob.h
# End Source File
# Begin Source File

SOURCE=.\ThreadManager.h
# End Source File
# Begin Source File

SOURCE=.\Tool.h
# End Source File
# Begin Source File

SOURCE=.\ToolDlg.h
# End Source File
# Begin Source File

SOURCE=.\ToolSettingsPage.h
# End Source File
# Begin Source File

SOURCE=.\ToolSettingsToolbar.h
# End Source File
# Begin Source File

SOURCE=.\WADList.h
# End Source File
# Begin Source File

SOURCE=.\WadMergeAddDlg.h
# End Source File
# Begin Source File

SOURCE=.\WadMergeDlg.h
# End Source File
# Begin Source File

SOURCE=.\WadMergeJob.h
# End Source File
# Begin Source File

SOURCE=.\WadMergeStatusDlg.h
# End Source File
# Begin Source File

SOURCE=.\Wally.h
# End Source File
# Begin Source File

SOURCE=.\WallyDoc.h
# End Source File
# Begin Source File

SOURCE=.\WallyDocTemplate.h
# End Source File
# Begin Source File

SOURCE=.\WallyException.h
# End Source File
# Begin Source File

SOURCE=.\WallyList.h
# End Source File
# Begin Source File

SOURCE=.\WallyPal.h
# End Source File
# Begin Source File

SOURCE=.\WallyPropertyPage1.h
# End Source File
# Begin Source File

SOURCE=.\WallyPropertySheet.h
# End Source File
# Begin Source File

SOURCE=.\WallyToolBar.h
# End Source File
# Begin Source File

SOURCE=.\WallyType.h
# End Source File
# Begin Source File

SOURCE=.\WallyUndo.h
# End Source File
# Begin Source File

SOURCE=.\WallyView.h
# End Source File
# Begin Source File

SOURCE=.\WLYChildFrm.h
# End Source File
# Begin Source File

SOURCE=.\WLYDoc.h
# End Source File
# Begin Source File

SOURCE=.\WLYView.h
# End Source File
# Begin Source File

SOURCE=.\WndList.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;cnt;rtf;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\Res\amount.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\batch.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\bitmap1.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\browse.ico
# End Source File
# Begin Source File

SOURCE=.\Res\brush_sh.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\buildtype.ico
# End Source File
# Begin Source File

SOURCE=.\Res\colordec.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\cursor1.cur
# End Source File
# Begin Source File

SOURCE=.\res\dropper_.cur
# End Source File
# Begin Source File

SOURCE=.\Res\fileasoc.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\grid.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\hand.cur
# End Source File
# Begin Source File

SOURCE=.\Res\ico00001.ico
# End Source File
# Begin Source File

SOURCE=.\Res\icon1.ico
# End Source File
# Begin Source File

SOURCE=.\RES\icon2.ico
# End Source File
# Begin Source File

SOURCE=.\Res\idr_brow.ico
# End Source File
# Begin Source File

SOURCE=.\RES\idr_buil.ico
# End Source File
# Begin Source File

SOURCE=.\RES\idr_pack.ico
# End Source File
# Begin Source File

SOURCE=.\Res\imageed.bmp
# End Source File
# Begin Source File

SOURCE=.\res\imageedi.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\imgedi_hi.bmp
# End Source File
# Begin Source File

SOURCE=.\RES\mip_icon.ico
# End Source File
# Begin Source File

SOURCE=.\RES\modes_to.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\move_cur.cur
# End Source File
# Begin Source File

SOURCE=.\Res\newtexs.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\options.bmp
# End Source File
# Begin Source File

SOURCE=.\RES\package2.ico
# End Source File
# Begin Source File

SOURCE=.\Res\package_.bmp
# End Source File
# Begin Source File

SOURCE=.\RES\packaged.bmp
# End Source File
# Begin Source File

SOURCE=.\res\paint_cu.cur
# End Source File
# Begin Source File

SOURCE=.\Res\pak.ico
# End Source File
# Begin Source File

SOURCE=.\res\palet1.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\palette.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\palette1.bin
# End Source File
# Begin Source File

SOURCE=.\Res\push_ani.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\push_bro.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\push_ran.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\push_til.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\ReMip.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\remiphlp.bmp
# End Source File
# Begin Source File

SOURCE=.\RES\Splash256.bmp
# End Source File
# Begin Source File

SOURCE=.\RES\splash256on.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\splsh16.bmp
# End Source File
# Begin Source File

SOURCE=.\Splsh16.bmp
# End Source File
# Begin Source File

SOURCE=.\res\Toolbar.bmp
# End Source File
# Begin Source File

SOURCE=.\res\Wally.ico
# End Source File
# Begin Source File

SOURCE=.\res\Wally.rc2
# End Source File
# Begin Source File

SOURCE=.\RES\wallydoc.bmp
# End Source File
# Begin Source File

SOURCE=.\res\WallyDoc.ico
# End Source File
# Begin Source File

SOURCE=.\RES\width.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\Wly.ico
# End Source File
# End Group
# Begin Group "Help Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\hlp\AfxCore.rtf
# End Source File
# Begin Source File

SOURCE=.\hlp\AppExit.bmp
# End Source File
# Begin Source File

SOURCE=.\hlp\Bullet.bmp
# End Source File
# Begin Source File

SOURCE=.\hlp\CurArw2.bmp
# End Source File
# Begin Source File

SOURCE=.\hlp\CurArw4.bmp
# End Source File
# Begin Source File

SOURCE=.\hlp\CurHelp.bmp
# End Source File
# Begin Source File

SOURCE=.\hlp\EditCopy.bmp
# End Source File
# Begin Source File

SOURCE=.\hlp\EditCut.bmp
# End Source File
# Begin Source File

SOURCE=.\hlp\EditPast.bmp
# End Source File
# Begin Source File

SOURCE=.\hlp\EditUndo.bmp
# End Source File
# Begin Source File

SOURCE=.\hlp\FileNew.bmp
# End Source File
# Begin Source File

SOURCE=.\hlp\FileOpen.bmp
# End Source File
# Begin Source File

SOURCE=.\hlp\FilePrnt.bmp
# End Source File
# Begin Source File

SOURCE=.\hlp\FileSave.bmp
# End Source File
# Begin Source File

SOURCE=.\hlp\HlpSBar.bmp
# End Source File
# Begin Source File

SOURCE=.\hlp\HlpTBar.bmp
# End Source File
# Begin Source File

SOURCE=.\MakeHelp.bat
# End Source File
# Begin Source File

SOURCE=.\hlp\RecFirst.bmp
# End Source File
# Begin Source File

SOURCE=.\hlp\RecLast.bmp
# End Source File
# Begin Source File

SOURCE=.\hlp\RecNext.bmp
# End Source File
# Begin Source File

SOURCE=.\hlp\RecPrev.bmp
# End Source File
# Begin Source File

SOURCE=.\hlp\Scmax.bmp
# End Source File
# Begin Source File

SOURCE=.\hlp\ScMenu.bmp
# End Source File
# Begin Source File

SOURCE=.\hlp\Scmin.bmp
# End Source File
# Begin Source File

SOURCE=.\hlp\Wally.cnt
# End Source File
# End Group
# Begin Source File

SOURCE=.\Res\blend.lmp
# End Source File
# Begin Source File

SOURCE=.\RES\blend.pal
# End Source File
# Begin Source File

SOURCE=.\Res\BuildViewButton.pcx
# End Source File
# Begin Source File

SOURCE=.\Res\duke3d.lmp
# End Source File
# Begin Source File

SOURCE=.\Res\Mouse.pcx
# End Source File
# Begin Source File

SOURCE=.\Res\MouseL.pcx
# End Source File
# Begin Source File

SOURCE=.\Res\MouseLRX.pcx
# End Source File
# Begin Source File

SOURCE=.\Res\MouseR.pcx
# End Source File
# Begin Source File

SOURCE=.\Res\MouseRX.pcx
# End Source File
# Begin Source File

SOURCE=.\Res\MouseX.pcx
# End Source File
# Begin Source File

SOURCE=.\Res\pldecal.wad
# End Source File
# Begin Source File

SOURCE=.\Res\pldecal.wad1
# End Source File
# Begin Source File

SOURCE=.\Res\q1pal.lmp
# End Source File
# Begin Source File

SOURCE=.\Res\q2pal.lmp
# End Source File
# Begin Source File

SOURCE=.\Res\Railgf1a.wav
# End Source File
# Begin Source File

SOURCE=.\ReadMe.txt
# End Source File
# Begin Source File

SOURCE=.\Res\redneck.dat
# End Source File
# Begin Source File

SOURCE=.\Res\rg_hum.wav
# End Source File
# Begin Source File

SOURCE=.\Res\shadow.lmp
# End Source File
# Begin Source File

SOURCE=.\ToDo.txt
# End Source File
# Begin Source File

SOURCE=.\RES\wad_thumbnail.pcx
# End Source File
# Begin Source File

SOURCE=.\Wally.reg
# End Source File
# Begin Source File

SOURCE=.\Wally.txt
# End Source File
# Begin Source File

SOURCE=.\Res\wally_logo.pcx
# End Source File
# End Target
# End Project
# Section Wally : {00000000-0000-0000-0000-000000000000}
# 	1:24:CG_IDR_POPUP_BROWSE_VIEW:106
# 	1:10:IDB_SPLASH:105
# 	1:23:CG_IDR_POPUP_WALLY_VIEW:102
# 	2:21:SplashScreenInsertKey:4.0
# End Section
