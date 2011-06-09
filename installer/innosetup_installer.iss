
; Change this to wherever you put the files to be packaged.
; See README.md for build and packaging instructions.
; Also change the version below when bumping.
#define DeploymentDir "C:\dev\deployments\yle-downloader-gui-windows-0.2"

#define MyAppName "YLE Areena -lataaja"
#define MyAppVersion "0.2"
#define MyAppURL "https://github.com/mpartel/yle-downloader-gui"
#define MyAppExeName "yle-downloader-gui.exe"

[Setup]
; NOTE: The value of AppId uniquely identifies this application.
; Do not use the same AppId value in installers for other applications.
; (To generate a new GUID, click Tools | Generate GUID inside the IDE.)
AppId={{5CDFB502-2DDD-488F-A22A-0EB27AD4C4BA}
AppName={#MyAppName}
AppVersion={#MyAppVersion}
;AppVerName={#MyAppName} {#MyAppVersion}
AppPublisherURL={#MyAppURL}
AppSupportURL={#MyAppURL}
AppUpdatesURL={#MyAppURL}
DefaultDirName={pf}\{#MyAppName}
DefaultGroupName={#MyAppName}
InfoBeforeFile={#DeploymentDir}\Tietoa Ohjelmasta.txt
OutputBaseFilename=setup
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

