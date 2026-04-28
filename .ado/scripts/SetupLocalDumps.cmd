@echo off
REM SetupLocalDumps.cmd [ExecutableName] [DumpFolder] [DumpType] [DumpCount]
REM Ex: .\SetupLocalDumps.cmd RNTesterApp-Fabric C:\WER\UserDumps
REM Ex: .\SetupLocalDumps.cmd RNTesterApp-Fabric C:\WER\UserDumps 2 5
REM
REM Configures Windows Error Reporting (WER) to save crash dumps for the named
REM executable to the given folder. This is the supported mechanism for
REM packaged/UWP apps where AeDebug-based JIT debuggers (e.g. ProcDump) are
REM not reliably invoked.
REM
REM DumpType:
REM   1 = Custom dump (uses CustomDumpFlags)
REM   2 = Full dump (default)
REM   3 = Mini dump
REM
REM DumpCount: max number of dumps to keep per exe (default 10)

setlocal enableextensions

if "%~1"=="" (
    @echo Must provide an executable name to set up local crash dumps
    exit /b 1
)
if "%~2"=="" (
    @echo Must provide a writable folder to save local crash dumps
    exit /b 1
)

set EXE_NAME=%~1
set CRASHDUMPS_FOLDER=%~2
set DUMP_TYPE=%~3
set DUMP_COUNT=%~4
if "%DUMP_TYPE%"=="" set DUMP_TYPE=2
if "%DUMP_COUNT%"=="" set DUMP_COUNT=10

if not exist "%CRASHDUMPS_FOLDER%" (
    @echo Creating %CRASHDUMPS_FOLDER%
    md "%CRASHDUMPS_FOLDER%"
)

set REG_KEY=HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Windows\Windows Error Reporting\LocalDumps\%EXE_NAME%.exe
@echo Configuring WER to save "%EXE_NAME%.exe" crash dumps (DumpType=%DUMP_TYPE%, DumpCount=%DUMP_COUNT%) to "%CRASHDUMPS_FOLDER%"...
reg add "%REG_KEY%" /v DumpFolder /t REG_EXPAND_SZ /d "%CRASHDUMPS_FOLDER%" /f
reg add "%REG_KEY%" /v DumpType   /t REG_DWORD     /d %DUMP_TYPE% /f
reg add "%REG_KEY%" /v DumpCount  /t REG_DWORD     /d %DUMP_COUNT% /f

REM Prevent the AeDebug post-mortem debugger from being invoked for this
REM executable so that WER LocalDumps gets first crack and writes to our folder.
reg add "HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Windows NT\CurrentVersion\AeDebug\AutoExclusionList" /v %EXE_NAME%.exe /t REG_DWORD /d 1 /f

@echo Registry configuration:
reg query "%REG_KEY%" /s
reg query "HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Windows NT\CurrentVersion\AeDebug\AutoExclusionList" /v %EXE_NAME%.exe

endlocal

exit /b %ERRORLEVEL%
