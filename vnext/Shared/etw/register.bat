@echo off
wevtutil um %~dp0\react_native_windows.man
wevtutil im react_native_windows.man /rf:"%~dp0\react_native_windows_etw_res.dll " /mf:"%~dp0\react_native_windows_etw_res.dll"