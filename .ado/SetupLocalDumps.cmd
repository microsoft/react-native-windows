reg query "HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Windows\Windows Error Reporting\LocalDumps" /s
reg add  "HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Windows\Windows Error Reporting\LocalDumps\ReactUWPTestApp.exe" /v DumpFolder /t REG_SZ /d %BUILD_ARTIFACTSTAGINGDIRECTORY%\ReactUWPTestAppTreeDump
reg add "HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Windows\Windows Error Reporting\LocalDumps\ReactUWPTestApp.exe" /v DumpType /t REG_DWORD /d 2
