@echo off
mc -um react_native_windows.man 
rc.exe react_native_windows.rc 
link.exe  /dll /noentry /machine:x64 react_native_windows.res /OUT:react_native_windows_etw_res.dll 