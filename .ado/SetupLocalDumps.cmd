setlocal
set CRASHDUMPS_FOLDER=%BUILD_ARTIFACTSTAGINGDIRECTORY%\CrashDumps
reg query "HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Windows\Windows Error Reporting\LocalDumps" /s
reg add  "HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Windows\Windows Error Reporting\LocalDumps\%1.exe" /v DumpFolder /t REG_SZ /d %CRASHDUMPS_FOLDER% /f
reg add "HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Windows\Windows Error Reporting\LocalDumps\%1.exe" /v DumpType /t REG_DWORD /d 2 /f
if not exist %CRASHDUMPS_FOLDER% ( 
    md %CRASHDUMPS_FOLDER%
)
echo LocalDumps settings:
reg query "HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Windows\Windows Error Reporting\LocalDumps" /s

rem AppVerifier flags
reg add "HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Windows NT\CurrentVersion\Image File Execution Options\%1.exe" /f /v "DelayFreeSizeMB" /t REG_DWORD /d 0x00000040
reg add "HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Windows NT\CurrentVersion\Image File Execution Options\%1.exe" /f /v "VerifierDlls" /t REG_SZ /d "vrfcore.dll vfbasics.dll"
reg add "HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Windows NT\CurrentVersion\Image File Execution Options\%1.exe" /f /v "GlobalFlag" /t REG_DWORD /d 0x100
reg add "HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Windows NT\CurrentVersion\Image File Execution Options\%1.exe" /f /v "VerifierFlags" /t REG_DWORD /d 0x80203001
reg add "HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Windows NT\CurrentVersion\Image File Execution Options\%1.exe" /f /v "PageHeapFlags" /t REG_DWORD /d 0x3

reg add "HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Windows NT\CurrentVersion\Image File Execution Options\%1.exe\Core-{E5DCDFAA-3B35-46A8-B370-9D3575A68E53}" /f /v "Propagate" /t REG_DWORD /d 0
reg add "HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Windows NT\CurrentVersion\Image File Execution Options\%1.exe\Core-{E5DCDFAA-3B35-46A8-B370-9D3575A68E53}" /f /v "AutoClr" /t REG_DWORD /d 0
reg add "HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Windows NT\CurrentVersion\Image File Execution Options\%1.exe\Core-{E5DCDFAA-3B35-46A8-B370-9D3575A68E53}" /f /v "AutoDisableStop" /t REG_DWORD /d 1
reg add "HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Windows NT\CurrentVersion\Image File Execution Options\%1.exe\Core-{E5DCDFAA-3B35-46A8-B370-9D3575A68E53}" /f /v "LoggingWithLocksHeld" /t REG_DWORD /d 1
reg add "HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Windows NT\CurrentVersion\Image File Execution Options\%1.exe\Core-{E5DCDFAA-3B35-46A8-B370-9D3575A68E53}" /f /v "ExceptionOnStop" /t REG_DWORD /d 0
reg add "HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Windows NT\CurrentVersion\Image File Execution Options\%1.exe\Core-{E5DCDFAA-3B35-46A8-B370-9D3575A68E53}" /f /v "MinimumMemoryOverhead" /t REG_DWORD /d 0

reg add "HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Windows NT\CurrentVersion\Image File Execution Options\%1.exe\Exceptions-{F86B022F-E589-4e8f-B0DD-6B7AF9D71A59}" /f
reg add "HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Windows NT\CurrentVersion\Image File Execution Options\%1.exe\Heaps-{4D056CEB-D8E3-4b85-B148-B543D56D9BDE}" /f

reg add "HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Windows NT\CurrentVersion\Image File Execution Options\%1.exe\Leak-{81EEC8DA-0E61-4942-8037-9A6C4A86510D}" /f /v "ExcludeDlls" /t REG_SZ /d ""
reg add "HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Windows NT\CurrentVersion\Image File Execution Options\%1.exe\Leak-{81EEC8DA-0E61-4942-8037-9A6C4A86510D}" /f /v "ExcludeUCRT" /t REG_DWORD /d 0

reg add "HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Windows NT\CurrentVersion\Image File Execution Options\%1.exe\Memory-{8A70B8A4-4FA6-41c3-85EE-595FCB3E1051}" /f /v "ExecWritePage" /t REG_DWORD /d 0


endlocal