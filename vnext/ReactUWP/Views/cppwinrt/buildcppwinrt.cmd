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

set TargetDir=..\..\..\target\cppwinrt

pushd %~dp0

echo Initializing target dir
if not exist "%TargetDir%" call mkdir "%TargetDir%"
if exist "%TargetDir%" call del /s /q "%TargetDir%\*"

for %%f in (*.idl) do (
  call :BuildWinmd %%f
)

echo Merge winmds
if not exist "%TargetDir%\merged" call mkdir "%TargetDir%\merged"
call mdmerge -v -metadata_dir "%FoundationDir%\." -metadata_dir "%UniversalApiDir%\." -o "%TargetDir%\merged" -i "%TargetDir%" -partial

for %%w in (%TargetDir%\merged\*.winmd) do (
  call :BuildHeaders %%w
)

echo Copy generated files
call xcopy /Y "%TargetDir%\*.h" .\
call xcopy /Y "%TargetDir%\winrt\*.h" .\winrt\
call xcopy /Y "%TargetDir%\winrt\impl\*.h" .\winrt\impl\

popd
goto :end

:BuildWinmd
echo Building winmd for "%~1"
call midlrt /metadata_dir "%FoundationDir%" /winrt /nomidl /ns_prefix /enum_class /h "NUL" /reference "%FoundationRef%" /reference "%UniversalApiRef%" /winmd "%TargetDir%\%~n1.winmd" %~1
exit /b

:BuildHeaders
echo Building headers for "%~1"
call cppwinrt -verbose -overwrite -prefix -comp "%TargetDir%\sources" -in "%TargetDir%\merged\%~1" -ref %TargetSDK% -out %TargetDir%
exit /b

:end
endlocal
exit /b