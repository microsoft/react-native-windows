:: Copyright (c) Microsoft Corporation. All rights reserved.
:: Licensed under the MIT License.

:: Until we get cppwinrt support properly included in ReactUWP, this script
:: will manually build the headers we need from each idl in this directory.

@echo off
setlocal enabledelayedexpansion

set TargetSDK=10.0.18362.0

set FoundationDir=%WindowsSdkDir%References\%TargetSDK%\Windows.Foundation.FoundationContract\3.0.0.0
set FoundationRef=%FoundationDir%\Windows.Foundation.FoundationContract.winmd

set UniversalApiDir=%WindowsSdkDir%References\%TargetSDK%\Windows.Foundation.UniversalApiContract\8.0.0.0
set UniversalApiRef=%UniversalApiDir%\Windows.Foundation.UniversalApiContract.winmd

pushd %~dp0

call :GetAbsolute "..\..\..\target\cppwinrt"
set TargetDir=%absolute%

echo.
echo ### Initializing target dir ###
echo.
if not exist "%TargetDir%" call mkdir "%TargetDir%"
if exist "%TargetDir%" call del /s /q "%TargetDir%\*"

echo.
echo ### Building winmds from idls ###
echo.

for %%f in (*.idl) do (
  call :BuildWinmd %%f
)

echo.
echo ### Merge winmds ###
echo.
if not exist "%TargetDir%\merged" call mkdir "%TargetDir%\merged"
call mdmerge -metadata_dir "%FoundationDir%\." -metadata_dir "%UniversalApiDir%\." -o "%TargetDir%\merged" -i "%TargetDir%" -partial

echo.
echo ### Building headers from winmds ###

for %%w in ("%TargetDir%\merged\*.winmd") do (
  call :BuildHeaders %%w
)

echo.
echo ### Copy generated files into ReactUWP ###
echo.
call xcopy /Y "%TargetDir%\*.h" .\
call xcopy /Y "%TargetDir%\winrt\*.h" .\winrt\
call xcopy /Y "%TargetDir%\winrt\impl\*.h" .\winrt\impl\
call xcopy /Y "%TargetDir%\module.g.cpp" ..\

echo.
echo ### Stubbed out implmentations (*.h and *. cpp) are in "%TargetDir%\sources" ###
echo.

popd
goto :end

:BuildWinmd
echo.
echo Building winmd for "%~1"
call midlrt /metadata_dir "%FoundationDir%" /winrt /W1 /nologo /char signed /env win32 /nomidl /ns_prefix /enum_class /h "NUL" /dlldata "NUL" /iid "NUL" /proxy "NUL" /notlb /client none /server none /target "NT60" /reference "%FoundationRef%" /reference "%UniversalApiRef%" /winmd "%TargetDir%\%~n1.winmd" %~1
exit /b

:BuildHeaders
echo.
echo Building headers for "%~1"
call cppwinrt -overwrite -prefix -comp "%TargetDir%\sources" -in "%~1" -ref %TargetSDK% -out %TargetDir%
exit /b

:GetAbsolute
set absolute=%~f1
exit /b

:end
endlocal
exit /b