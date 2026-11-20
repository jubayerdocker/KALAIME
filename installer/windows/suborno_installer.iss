; Suborno IME Installer Script for Inno Setup
; This script creates a professional Windows installer for Suborno IME.

#define MyAppName "Suborno IME"
#define MyAppVersion "0.1.0"
#define MyAppPublisher "Suborno Project"
#define MyAppExeName "SubornoIME.exe"

[Setup]
AppId={4F1A2B3C-5D6E-7F8A-9B0C-1D2E3F4A5B6C}
AppName={#MyAppName}
AppVersion={#MyAppVersion}
AppPublisher={#MyAppPublisher}
DefaultDirName={pf}\{#MyAppName}
DefaultGroupName={#MyAppName}
AllowNoIcons=yes
TargetArchitecture=x64
ArchitecturesAllowed=x64
Compression=lzma
SolidCompression=yes
WizardStyle=modern

[Languages]
Name: "english"; MessagesFile: "compiler:Default.isl"

[Tasks]
Name: "desktopicon"; Description: "Create a desktop icon"; GroupDescription: "Additional icons:"; Flags: unchecked

[Files]
Source: "build\windows-msvc\Release\suborno_windows_tsf.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "build\windows-msvc\Release\suborno_core.lib"; DestDir: "{app}"; Flags: ignoreversion
Source: "build\windows-msvc\Release\suborno_unicode.lib"; DestDir: "{app}"; Flags: ignoreversion
Source: "build\windows-msvc\Release\suborno_dictionary.lib"; DestDir: "{app}"; Flags: ignoreversion
Source: "build\windows-msvc\Release\suborno_settings.lib"; DestDir: "{app}"; Flags: ignoreversion

[Icons]
Name: "{group}\{#MyAppName}"; Filename: "{app}\{#MyAppExeName}"
Name: "{group}\Uninstall {#MyAppName}"; Filename: "{uninstallexe}"
Name: "{desktop}\{#MyAppName}"; Filename: "{app}\{#MyAppExeName}"; Tasks: desktopicon

[Registry]
; Register TSF language profile
Root: HKLM; Subkey: "Software\Microsoft\CTF\TIP\{4F1A2B3C-5D6E-7F8A-9B0C-1D2E3F4A5B6C}\LanguageProfile\0x00000045\{7A1B2C3D-4E5F-6A7B-8C9D-0E1F2A3B4C5D}"; ValueType: string; ValueName: ""; ValueData: "Suborno Bengali (Bangladesh)"; Flags: uninsdeletekey
Root: HKLM; Subkey: "Software\Microsoft\CTF\TIP\{4F1A2B3C-5D6E-7F8A-9B0C-1D2E3F4A5B6C}\LanguageProfile\0x00000045\{7A1B2C3D-4E5F-6A7B-8C9D-0E1F2A3B4C5D}"; ValueType: string; ValueName: "Description"; ValueData: "Suborno Bengali Input Method Editor"; Flags: uninsdeletekey
Root: HKLM; Subkey: "Software\Microsoft\CTF\TIP\{4F1A2B3C-5D6E-7F8A-9B0C-1D2E3F4A5B6C}\LanguageProfile\0x00000045\{7A1B2C3D-4E5F-6A7B-8C9D-0E1F2A3B4C5D}"; ValueType: string; ValueName: "TIP"; ValueData: "{4F1A2B3C-5D6E-7F8A-9B0C-1D2E3F4A5B6C}"; Flags: uninsdeletekey

[Code]
function InitializeSetup(): Boolean;
begin
  Result := True;
end;

procedure CurStepChanged(CurStep: TSetupStep);
begin
  if CurStep = ssPostInstall then begin
    ; Register the TSF DLL
    if not RegServer(ExpandConstant('{app}\suborno_windows_tsf.dll'), False) then begin
      MsgBox('Failed to register Suborno TSF service.', mbError, MB_OK);
    end;
  end;
end;

function RegServer(const FileName: string; bUnregister: Boolean): Integer;
var
  ResultCode: Integer;
begin
  if bUnregister then
    ResultCode := Exec('regsvr32.exe', '/s /u "' + FileName + '"', '', SW_HIDE, ewWaitUntilTerminated, ResultCode)
  else
    ResultCode := Exec('regsvr32.exe', '/s "' + FileName + '"', '', SW_HIDE, ewWaitUntilTerminated, ResultCode);
  RegServer := ResultCode;
end;
