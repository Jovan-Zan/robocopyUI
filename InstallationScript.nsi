;---Installation script

	!define APP_NAME "robocopyUI"

; Include Modern UI
	!include "MUI2.nsh"
	
; Includes KillProcess
	!include "nsProcess.nsh"
	
; Includes if statement
	!include LogicLib.nsh
	
; Includes runningX64
	!include x64.nsh
	
;------------------------------

; The name of the installer
	Name "${APP_NAME}"

; The output file
	OutFile "robocopyUIInstaller.exe"
	
; Installer and program icon
;	!define MUI_ICON "Icon.ico"

; The default installation directory
	InstallDir 	"$PROGRAMFILES\${APP_NAME}"
	
; Registry key to check for directory (so if installed again, it will 
; overwrite the old one automatically)
	InstallDirRegKey HKLM "Software\${APP_NAME}" "InstallDir"

; Administrator privileges are needed to install to ProgramFiles directory
	RequestExecutionLevel admin

;------------------------------

; Prompt a warning when user tries to cancel installation.
	!define MUI_ABORTWARNING	
	
;------------------------------

; Pages user goes through when installing
	!insertmacro MUI_PAGE_DIRECTORY
	!insertmacro MUI_PAGE_INSTFILES  
	
;------------------------------

; Uninstall pages
	!insertmacro MUI_UNPAGE_CONFIRM
	!insertmacro MUI_UNPAGE_INSTFILES

;------------------------------

; On installation initialization, set registry view to x64 if running x64
Function .onInit
  ${If} ${RunningX64}
    SetRegView 64
  ${EndIf}
FunctionEnd

;------------------------------

Section ""

; Set output path to the installation directory.
	SetOutPath $INSTDIR

; Put files
	File "CopyApp.exe"
	File "PasteApp.exe"
	File "ClipboardApp.exe"
	
; Save installation folder to registry
	WriteRegStr HKLM "Software\${APP_NAME}" "InstallDir" "$INSTDIR"  

; Add Robocopy script generation command
	CreateShortCut "$SENDTO\Robocopy Script Generator.lnk" "$INSTDIR\CopyApp.exe"
	
; Add Execute Robocopy command
	WriteRegStr HKCR "Directory\Background\shell\Execute Robocopy\command" "" '"$INSTDIR\PasteApp.exe" "%v"' 
	
; Set "hide file name extensions" to false
	WriteRegDWORD HKCU "Software\Microsoft\Windows\CurrentVersion\Explorer\Advanced" "HideFileExt" 0
		
; Write the uninstall keys for Windows
	WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APP_NAME}" "DisplayName" "${APP_NAME}"
	WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APP_NAME}" "UninstallString" '"$INSTDIR\uninstall.exe"'
	WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APP_NAME}" "NoModify" 1
	WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APP_NAME}" "NoRepair" 1
	
; Write the uninstaller
	WriteUninstaller "$INSTDIR\uninstall.exe"
SectionEnd

;------------------------------

; On UNinstallation initialization, set registry view to x64 if running x64
Function un.onInit
  ${If} ${RunningX64}
    SetRegView 64
  ${EndIf}
FunctionEnd

;------------------------------

Section "Uninstall"
; Kill ClipboardApp.exe before uninstallation and save result in registry R0
	${nsProcess::KillProcess} "ClipboardApp.exe" $R0

; Give Windows some time to actually kill the process
	Sleep 3000
	
; Delete registry keys and values
	DeleteRegKey HKLM "Software\${APP_NAME}"
	DeleteRegKey HKCR "Directory\Background\shell\Execute Robocopy"
	DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APP_NAME}"

; Delete files
	Delete "$INSTDIR\CopyApp.exe"
	Delete "$INSTDIR\CopyApp.exe.config"
	Delete "$INSTDIR\PasteApp.exe"
	Delete "$INSTDIR\PasteApp.exe.config"
	Delete "$INSTDIR\ClipboardApp.exe"
	Delete "$INSTDIR\ClipboardApp.exe.config"
	Delete "$INSTDIR\uninstall.exe"
	Delete "$SENDTO\Robocopy Script Generator.lnk"
	
; Delete directories used
	RMDir $INSTDIR
	
SectionEnd