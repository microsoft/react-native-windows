:: Copyright (c) Microsoft Corporation. All rights reserved.
:: Licensed under the MIT License.

:: Until we get cppwinrt support properly included in ReactUWP, this script
:: will manually build the headers we need from each idl in this directory.

@echo off
setlocal enabledelayedexpansion

set TargetSDK=10.0.18362.0

set MetadataDir=%WindowsSdkDir%References\%TargetSDK%\Windows.Foundation.FoundationContract\3.0.0.0
set FoundationRef=%MetadataDir%\Windows.Foundation.FoundationContract.winmd
set UniversalApiRef=%WindowsSdkDir%References\%TargetSDK%\Windows.Foundation.UniversalApiContract\8.0.0.0\Windows.Foundation.UniversalApiContract.winmd

set TargetDir=..\..\..\target\cppwinrt

pushd %~dp0

echo Initializing target dir
if not exist "%TargetDir%" call mkdir "%TargetDir%"
if exist "%TargetDir%" call del /s /q "%TargetDir%\*"

for %%f in (*.idl) do (
  call :BuildHeaders %%f
)

popd
goto :end

:BuildHeaders 
echo Building winmd for "%~1"
call midlrt /metadata_dir "%MetadataDir%" /winrt /nomidl /ns_prefix /enum_class /h "NUL" /reference "%FoundationRef%" /reference "%UniversalApiRef%" /winmd "%TargetDir%\%~n1.winmd" %~1
echo Building headers from "%~n1.winmd"
call cppwinrt -verbose -overwrite -prefix -comp "%TargetDir%\sources" -in "%TargetDir%\%~n1.winmd" -ref %TargetSDK% -out %TargetDir%
echo Copying generated files
call xcopy /Y "%TargetDir%\*.h" .\
call xcopy /Y  "%TargetDir%\winrt\*.h" .\winrt\
call xcopy /Y "%TargetDir%\winrt\impl\*.h" .\winrt\impl\
exit /b

:end
endlocal
exit /b