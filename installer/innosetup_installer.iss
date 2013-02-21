
; See README.md for build and packaging instructions.

; Make sure this matches the version number in config.h:
#define MyAppVersion "2.2"
; Point this to wherever you gathered the distribution files:
#define DeploymentDir "C:\dev\deployments\yle-downloader-gui-windows-" + MyAppVersion

#define MyAppId "{5CDFB502-2DDD-488F-A22A-0EB27AD4C4BA}"
#define MyAppName "YLE Areena -lataaja"
#define MyAppURL "http://mpartel.github.com/yle-downloader-gui/"
#define MyAppExeName "yle-downloader-gui.exe"

[Setup]
; NOTE: The value of AppId uniquely identifies this application.
; Do not use the same AppId value in installers for other applications.
; (To generate a new GUID, click Tools | Generate GUID inside the IDE.)
AppId={{#MyAppId}
AppName={#MyAppName}
AppVersion={#MyAppVersion}
AppVerName={#MyAppName} {#MyAppVersion}
AppPublisherURL={#MyAppURL}
AppSupportURL={#MyAppURL}
AppUpdatesURL={#MyAppURL}
DefaultDirName={pf}\{#MyAppName}
DefaultGroupName={#MyAppName}
InfoBeforeFile={#DeploymentDir}\Tietoa ohjelmasta.txt
OutputBaseFilename=setup-yle-downloader-gui-{#MyAppVersion}
Compression=lzma
SolidCompression=yes
SetupIconFile=..\icons\icon.ico

[Languages]
Name: "finnish"; MessagesFile: "compiler:Languages\Finnish.isl"

[CustomMessages]
ProgramOnTheWeb=Ohjelman kotisivu

[Tasks]
Name: "desktopicon"; Description: "{cm:CreateDesktopIcon}"; GroupDescription: "{cm:AdditionalIcons}"; Flags: unchecked

[Files]
Source: "{#DeploymentDir}\*"; DestDir: "{app}"; Flags: ignoreversion recursesubdirs createallsubdirs

[Icons]
Name: "{group}\Käynnistä YLE Areena -lataaja"; Filename: "{app}\{#MyAppExeName}"
Name: "{group}\Tietoa ohjelmasta.txt"; Filename: "{app}\Tietoa ohjelmasta.txt"
Name: "{group}\{cm:ProgramOnTheWeb,{#MyAppName}}"; Filename: "{#MyAppURL}"
Name: "{group}\{cm:UninstallProgram,{#MyAppName}}"; Filename: "{uninstallexe}"
Name: "{commondesktop}\{#MyAppName}"; Filename: "{app}\{#MyAppExeName}"; Tasks: desktopicon

[code]
// OMG it's Pascal! ^_^
// Attempt to silently uninstall old version if found to avoid leaving old versions of rtmpdump-yle lying around.
function NextButtonClick(CurPageID: Integer): Boolean;
var
  Uninstaller: String;
  ErrorCode: Integer;
begin
  if (CurPageId = wpReady) then
  begin
    if not RegQueryStringValue(HKEY_LOCAL_MACHINE,
      'SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\{#MyAppId}_is1',
      'UninstallString', Uninstaller) then
    begin
      RegQueryStringValue(HKEY_CURRENT_USER,
        'SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\{#MyAppId}_is1',
        'UninstallString', Uninstaller);
    end;
    if Uninstaller <> '' then
    begin
      ShellExec('runas', uninstaller, '/SILENT', '', SW_HIDE, ewWaitUntilTerminated, ErrorCode);
      // (ignore any error in ErrorCode)
    end;
  end;
  Result := True;
end;

[Run]
Filename: "{app}\{#MyAppExeName}"; Description: "{cm:LaunchProgram,{#StringChange(MyAppName, "&", "&&")}}"; Flags: nowait postinstall skipifsilent

