setlocal
set CRASHDUMPS_FOLDER=%BUILD_ARTIFACTSTAGINGDIRECTORY%\ReactUWPTestAppTreeDump\CrashDumps
reg query "HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Windows\Windows Error Reporting\LocalDumps" /s
reg add  "HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Windows\Windows Error Reporting\LocalDumps\ReactUWPTestApp.exe" /v DumpFolder /t REG_SZ /d %CRASHDUMPS_FOLDER%
reg add "HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Windows\Windows Error Reporting\LocalDumps\ReactUWPTestApp.exe" /v DumpType /t REG_DWORD /d 2
if not exist %CRASHDUMPS_FOLDER% ( 
    md %CRASHDUMPS_FOLDER%
) else (
    del %CRASHDUMPS_FOLDER%\* /s /q
)
endlocal