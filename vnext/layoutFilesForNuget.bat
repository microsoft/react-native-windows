@echo off
setlocal
goto :parse_args

:Usage
echo %~nx0 usage:
echo %~nx0 DestinationRoot
goto :eof

:parse_args
IF [%1]==[] GOTO :Usage

set DESTROOT=%~1
set SRCROOT=%~dp0
:: Remove trailing \ from srcroot
IF %SRCROOT:~-1%==\ SET SRCROOT=%SRCROOT:~0,-1%
set RNROOT=%~dp0..\node_modules\react-native
set FOLLYROOT=%~dp0..\node_modules\.folly\folly-2019.08.12.00

echo Source root: %SRCROOT%
echo Dest root: %DESTROOT%
echo React-Native root: %RNROOT%

set COPYCMD=xcopy /s /y /i

mkdir %DESTROOT% >nul 2>&1
mkdir %DESTROOT%\inc >nul 2>&1
mkdir %DESTROOT%\inc\cxxreact >nul 2>&1
mkdir %DESTROOT%\inc\jschelpers >nul 2>&1
mkdir %DESTROOT%\inc\jsi >nul 2>&1
mkdir %DESTROOT%\inc\yoga >nul 2>&1
mkdir %DESTROOT%\inc\ReactWin32 >nul 2>&1
mkdir %DESTROOT%\inc\ReactUWP >nul 2>&1

%COPYCMD%  %RNROOT%\ReactCommon\cxxreact\cxxmodule.h                      %DESTROOT%\inc\cxxreact
%COPYCMD%  %RNROOT%\ReactCommon\cxxreact\Instance.h                       %DESTROOT%\inc\cxxreact
%COPYCMD%  %RNROOT%\ReactCommon\cxxreact\jsargumenthelpers-inl.h          %DESTROOT%\inc\cxxreact
%COPYCMD%  %RNROOT%\ReactCommon\cxxreact\jsargumenthelpers.h              %DESTROOT%\inc\cxxreact
%COPYCMD%  %RNROOT%\ReactCommon\cxxreact\JSBigString.h                    %DESTROOT%\inc\cxxreact
%COPYCMD%  %RNROOT%\ReactCommon\cxxreact\JSCExecutor.h                    %DESTROOT%\inc\cxxreact
%COPYCMD%  %RNROOT%\ReactCommon\cxxreact\JSCNativeModules.h               %DESTROOT%\inc\cxxreact
%COPYCMD%  %RNROOT%\ReactCommon\cxxreact\JSExecutor.h                     %DESTROOT%\inc\cxxreact
%COPYCMD%  %RNROOT%\ReactCommon\cxxreact\MessageQueueThread.h             %DESTROOT%\inc\cxxreact
%COPYCMD%  %RNROOT%\ReactCommon\cxxreact\ModuleRegistry.h                 %DESTROOT%\inc\cxxreact
%COPYCMD%  %RNROOT%\ReactCommon\cxxreact\NativeToJsBridge.h               %DESTROOT%\inc\cxxreact
%COPYCMD%  %RNROOT%\ReactCommon\cxxreact\NativeModule.h                   %DESTROOT%\inc\cxxreact
%COPYCMD%  %RNROOT%\ReactCommon\cxxreact\PlatformBundleInfo.h             %DESTROOT%\inc\cxxreact

%COPYCMD%  %SRCROOT%\Chakra\ChakraCoreDebugger.h                          %DESTROOT%\inc\jsi
%COPYCMD%  %SRCROOT%\Chakra\ChakraJsiRuntimeArgs.h                        %DESTROOT%\inc\jsi
%COPYCMD%  %SRCROOT%\Chakra\ChakraJsiRuntimeFactory.h                     %DESTROOT%\inc\jsi

%COPYCMD%  %RNROOT%\ReactCommon\Yoga\Yoga\*.h                             %DESTROOT%\inc\yoga

%COPYCMD%  %FOLLYROOT%                                                    %DESTROOT%\inc\Folly
%COPYCMD%  %RNROOT%\ReactCommon\jsi                                       %DESTROOT%\inc\jsi
%COPYCMD%  %SRCROOT%\stubs                                                %DESTROOT%\inc\stubs
%COPYCMD%  %SRCROOT%\Desktop\*.h                                          %DESTROOT%\inc\ReactWin32
%COPYCMD%  %SRCROOT%\ReactWindowsCore\*.h                                 %DESTROOT%\inc\ReactWindowsCore
%COPYCMD%  %SRCROOT%\ReactUWP\*.h                                         %DESTROOT%\inc\ReactUWP
%COPYCMD%  %SRCROOT%\include                                              %DESTROOT%\inc\include
%COPYCMD%  %SRCROOT%\Test\*.h                                              %DESTROOT%\inc\Test

%COPYCMD%  %SRCROOT%\Desktop.DLL\react-native-win32.*.def                 %DESTROOT%\inc

%COPYCMD%  %SRCROOT%\ReactWin32.nuspec                                    %DESTROOT%
%COPYCMD%  %SRCROOT%\ReactUwp.nuspec                                      %DESTROOT%

