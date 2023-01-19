@echo off
REM SetupLocalDumps.cmd [ExecutableName] [DumpFolder]
REM Ex: .\SetupLocalDumps.cmd RNTesterApp C:\WER\UserDumps
REM
REM This script sets the registry so that, if an executable of the given name crashes, to
REM prevent any automatic debugger from attaching, and instead save a full crash dump to
REM the given folder.

setlocal

if "%1"=="" (
    @echo Must provide an executable name to set up local crash dumps
    exit /b 1
)
if "%2"=="" (
    @echo Must provide a writable folder to save local crash dumps
    exit /b 1
)

set CRASHDUMPS_FOLDER=%2
@echo Configuring registry to save "%1.exe" crash dumps to "%CRASHDUMPS_FOLDER%"...
reg add  "HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Windows\Windows Error Reporting\LocalDumps\%1.exe" /v DumpFolder /t REG_EXPAND_SZ /d %CRASHDUMPS_FOLDER%
reg add "HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Windows\Windows Error Reporting\LocalDumps\%1.exe" /v DumpType /t REG_DWORD /d 2
reg add "HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Windows\Windows Error Reporting\LocalDumps\%1.exe" /v DumpCount /t REG_DWORD /d 3
reg add "HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Windows NT\CurrentVersion\AeDebug\AutoExclusionList" /v %1.exe  /t REG_DWORD /d 1
if not exist %CRASHDUMPS_FOLDER% (
    @echo Creating %CRASHDUMPS_FOLDER%
    md %CRASHDUMPS_FOLDER%
)

@echo Registry configuration:
reg query "HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Windows\Windows Error Reporting\LocalDumps\%1.exe" /s
reg query "HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Windows NT\CurrentVersion\AeDebug\AutoExclusionList"

endlocal

exit /b %ERRORLEVEL%