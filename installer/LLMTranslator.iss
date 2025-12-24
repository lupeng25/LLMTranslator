; LLMTranslator Inno Setup 安装脚本
; 为 Release 版本生成安装包

#define MyAppName "LLM Translator"
#define MyAppVersion "1.0"
#define MyAppPublisher "Your Company"
#define MyAppURL "https://example.com/"
#define MyAppExeName "LLMTranslator.exe"

[Setup]
; 应用程序基本信息
AppId={{A1B2C3D4-E5F6-7890-ABCD-EF1234567890}
AppName={#MyAppName}
AppVersion={#MyAppVersion}
AppPublisher={#MyAppPublisher}
AppPublisherURL={#MyAppURL}
AppSupportURL={#MyAppURL}
AppUpdatesURL={#MyAppURL}

; 安装目录
DefaultDirName={autopf}\{#MyAppName}
DefaultGroupName={#MyAppName}
AllowNoIcons=yes

; 输出文件
OutputDir=.
OutputBaseFilename=LLMTranslator_Setup

; 压缩设置
Compression=lzma
SolidCompression=yes
WizardStyle=modern

; 权限设置
PrivilegesRequired=lowest

; 安装程序图标和信息（可选）
; SetupIconFile=icon.ico
; InfoBeforeFile=readme.txt
; InfoAfterFile=

[Languages]
Name: "chinesesimp"; MessagesFile: "compiler:Languages\ChineseSimplified.isl"
Name: "english"; MessagesFile: "compiler:Default.isl"

[Tasks]
Name: "desktopicon"; Description: "{cm:CreateDesktopIcon}"; GroupDescription: "{cm:AdditionalIcons}"; Flags: unchecked
Name: "quicklaunchicon"; Description: "{cm:CreateQuickLaunchIcon}"; GroupDescription: "{cm:AdditionalIcons}"; Flags: unchecked; OnlyBelowVersion: 0,6.1

[Files]
; 主程序
Source: "..\build\Release\LLMTranslator.exe"; DestDir: "{app}"; Flags: ignoreversion

; Qt 核心 DLL
Source: "..\build\Release\Qt5Core.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "..\build\Release\Qt5Gui.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "..\build\Release\Qt5Widgets.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "..\build\Release\Qt5Network.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "..\build\Release\Qt5Xml.dll"; DestDir: "{app}"; Flags: ignoreversion

; OpenSSL DLL
Source: "..\build\Release\libcrypto-3-x64.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "..\build\Release\libssl-3-x64.dll"; DestDir: "{app}"; Flags: ignoreversion

; 其他系统 DLL
Source: "..\build\Release\libpng16.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "..\build\Release\zlib.dll"; DestDir: "{app}"; Flags: ignoreversion

; Qt 平台插件
Source: "..\build\Release\platforms\qwindows.dll"; DestDir: "{app}\platforms"; Flags: ignoreversion

; Qt 图像格式插件
Source: "..\build\Release\imageformats\qgif.dll"; DestDir: "{app}\imageformats"; Flags: ignoreversion
Source: "..\build\Release\imageformats\qico.dll"; DestDir: "{app}\imageformats"; Flags: ignoreversion
Source: "..\build\Release\imageformats\qjpeg.dll"; DestDir: "{app}\imageformats"; Flags: ignoreversion

; Qt 样式插件
Source: "..\build\Release\styles\qwindowsvistastyle.dll"; DestDir: "{app}\styles"; Flags: ignoreversion

; 注意：不需要 Visual C++ 运行时库，因为用户系统应该已经安装
; 如果需要，可以在 [Run] 部分添加运行时库安装程序

[Icons]
Name: "{group}\{#MyAppName}"; Filename: "{app}\{#MyAppExeName}"
Name: "{group}\{cm:UninstallProgram,{#MyAppName}}"; Filename: "{uninstallexe}"
Name: "{autodesktop}\{#MyAppName}"; Filename: "{app}\{#MyAppExeName}"; Tasks: desktopicon
Name: "{userappdata}\Microsoft\Internet Explorer\Quick Launch\{#MyAppName}"; Filename: "{app}\{#MyAppExeName}"; Tasks: quicklaunchicon

[Run]
Filename: "{app}\{#MyAppExeName}"; Description: "{cm:LaunchProgram,{#StringChange(MyAppName, '&', '&&')}}"; Flags: nowait postinstall skipifsilent

[Code]
// 安装后处理函数
procedure CurStepChanged(CurStep: TSetupStep);
begin
  if CurStep = ssPostInstall then
  begin
    // 安装完成后的操作
    // 可以在这里添加注册表设置、配置文件创建等
  end;
end;

