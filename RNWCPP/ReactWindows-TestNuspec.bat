@echo off
setlocal
goto :parse_args

:Usage
echo %~nx0 usage:
echo %~nx0 DestinationRoot  Version
echo e.g:  o:\myTestDir 0.2017.0817-v1
goto :eof

:parse_args
IF [%1]==[] GOTO :Usage
IF [%2]==[] GOTO :Usage

set DESTROOT=%~1
set VERSION=%~2
set SRCROOT=%~dp0..

echo Source root: %SRCROOT%
echo Dest root: %DESTROOT%
echo Version: %VERSION%

call ReactWindows-CopyToStaging.bat %DESTROOT%

set COPYCMD=xcopy /s /y /i

mkdir %DESTROOT% >nul 2>&1
mkdir %DESTROOT%\target >nul 2>&1

set PLATFORMS=x86 x64 arm
FOR %%p IN (%PLATFORMS%) DO (
mkdir %DESTROOT%\target\%%p >nul 2>&1
mkdir %DESTROOT%\target\%%p\debug >nul 2>&1
mkdir %DESTROOT%\target\%%p\ship >nul 2>&1
)

%COPYCMD%  %SRCROOT%\react-native-win\target\x86\debug       %DESTROOT%\target\x86\debug
%COPYCMD%  %SRCROOT%\react-native-win\target\x86\release     %DESTROOT%\target\x86\ship
%COPYCMD%  %SRCROOT%\react-native-win\target\x64\debug       %DESTROOT%\target\x64\debug
%COPYCMD%  %SRCROOT%\react-native-win\target\x64\release     %DESTROOT%\target\x64\ship
%COPYCMD%  %SRCROOT%\react-native-win\target\arm\debug       %DESTROOT%\target\arm\debug
%COPYCMD%  %SRCROOT%\react-native-win\target\arm\release     %DESTROOT%\target\arm\ship

@echo on
sed s/__BuildBuildNumber__/%VERSION%/g %DESTROOT%\ReactWin32.nuspec > %DESTROOT%\ReactWin32.versioned.nuspec
nuget.exe pack %DESTROOT%\ReactWin32.versioned.nuspec -NonInteractive -OutputDirectory %DESTROOT% -Verbosity Detailed