setlocal
if "%1"=="" (echo Must provide an executable name to set up local crash dumps for && exit /b 1)
if "%BUILD_ARTIFACTSTAGINGDIRECTORY%"=="" (echo BUILD_ARTIFACTSTAGINGDIRECTORY must be set to the artifact staging directory && exit /b 2)

set CRASHDUMPS_FOLDER=%BUILD_ARTIFACTSTAGINGDIRECTORY%\CrashDumps
reg query "HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Windows\Windows Error Reporting\LocalDumps" /s
reg add  "HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Windows\Windows Error Reporting\LocalDumps\%1.exe" /v DumpFolder /t REG_SZ /d %CRASHDUMPS_FOLDER%
reg add "HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Windows\Windows Error Reporting\LocalDumps\%1.exe" /v DumpType /t REG_DWORD /d 2
if not exist %CRASHDUMPS_FOLDER% ( 
    md %CRASHDUMPS_FOLDER%
)

endlocal