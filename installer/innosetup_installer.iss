
; See README.md for build and packaging instructions.

; Make sure this matches the version number in config.h:
#define MyAppVersion "1.0"
; Point this to wherever you gathered the distribution files:
#define DeploymentDir "C:\dev\deployments\yle-downloader-gui-windows-" + MyAppVersion

#define MyAppName "YLE Areena -lataaja"
#define MyAppURL "https://github.com/mpartel/yle-downloader-gui"
#define MyAppExeName "yle-downloader-gui.exe"

[Setup]
; NOTE: The value of AppId uniquely identifies this application.
; Do not use the same AppId value in installers for other applications.
; (To generate a new GUID, click Tools | Generate GUID inside the IDE.)
AppId={{5CDFB502-2DDD-488F-A22A-0EB27AD4C4BA}
AppName={#MyAppName}
AppVersion={#MyAppVersion}
AppVerName={#MyAppName} {#MyAppVersion}
AppPublisherURL={#MyAppURL}
AppSupportURL={#MyAppURL}
AppUpdatesURL={#MyAppURL}
DefaultDirName={pf}\{#MyAppName}
DefaultGroupName={#MyAppName}
InfoBeforeFile={#DeploymentDir}\Tietoa Ohjelmasta.txt
OutputBaseFilename=setup-yle-downloader-gui-{#MyAppVersion}
Compression=lzma
SolidCompression=yes
SetupIconFile=..\icon.ico

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
Name: "{group}\Tietoa Ohjelmasta.txt"; Filename: "{app}\Tietoa Ohjelmasta.txt"
Name: "{group}\{cm:ProgramOnTheWeb,{#MyAppName}}"; Filename: "{#MyAppURL}"
Name: "{group}\{cm:UninstallProgram,{#MyAppName}}"; Filename: "{uninstallexe}"
Name: "{commondesktop}\{#MyAppName}"; Filename: "{app}\{#MyAppExeName}"; Tasks: desktopicon

[Run]
Filename: "{app}\{#MyAppExeName}"; Description: "{cm:LaunchProgram,{#StringChange(MyAppName, "&", "&&")}}"; Flags: nowait postinstall skipifsilent

