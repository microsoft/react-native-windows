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
set SRCROOT=%~dp0..

echo Source root: %SRCROOT%
echo Dest root: %DESTROOT%

set COPYCMD=xcopy /s /y /i

mkdir %DESTROOT% >nul 2>&1
mkdir %DESTROOT%\inc >nul 2>&1
mkdir %DESTROOT%\inc\cxxreact >nul 2>&1
mkdir %DESTROOT%\inc\jschelpers >nul 2>&1
mkdir %DESTROOT%\inc\jsi >nul 2>&1
mkdir %DESTROOT%\inc\yoga >nul 2>&1
mkdir %DESTROOT%\inc\ReactWin32 >nul 2>&1
mkdir %DESTROOT%\inc\ReactUWP >nul 2>&1

%COPYCMD%  %SRCROOT%\react-native\ReactCommon\cxxreact\cxxmodule.h                %DESTROOT%\inc\cxxreact
%COPYCMD%  %SRCROOT%\react-native\ReactCommon\cxxreact\Instance.h                 %DESTROOT%\inc\cxxreact
%COPYCMD%  %SRCROOT%\react-native\ReactCommon\cxxreact\jsargumenthelpers-inl.h    %DESTROOT%\inc\cxxreact
%COPYCMD%  %SRCROOT%\react-native\ReactCommon\cxxreact\jsargumenthelpers.h        %DESTROOT%\inc\cxxreact
%COPYCMD%  %SRCROOT%\react-native\ReactCommon\cxxreact\JSBigString.h              %DESTROOT%\inc\cxxreact
%COPYCMD%  %SRCROOT%\react-native\ReactCommon\cxxreact\JSCExecutor.h              %DESTROOT%\inc\cxxreact
%COPYCMD%  %SRCROOT%\react-native\ReactCommon\cxxreact\JSCNativeModules.h         %DESTROOT%\inc\cxxreact
%COPYCMD%  %SRCROOT%\react-native\ReactCommon\cxxreact\JSExecutor.h               %DESTROOT%\inc\cxxreact
%COPYCMD%  %SRCROOT%\react-native\ReactCommon\cxxreact\MessageQueueThread.h       %DESTROOT%\inc\cxxreact
%COPYCMD%  %SRCROOT%\react-native\ReactCommon\cxxreact\ModuleRegistry.h           %DESTROOT%\inc\cxxreact
%COPYCMD%  %SRCROOT%\react-native\ReactCommon\cxxreact\NativeToJsBridge.h         %DESTROOT%\inc\cxxreact
%COPYCMD%  %SRCROOT%\react-native\ReactCommon\cxxreact\NativeModule.h             %DESTROOT%\inc\cxxreact
%COPYCMD%  %SRCROOT%\react-native\ReactCommon\cxxreact\PlatformBundleInfo.h       %DESTROOT%\inc\cxxreact

%COPYCMD%  %SRCROOT%\react-native\ReactCommon\jsi\jsi.h                           %DESTROOT%\inc\jsi
%COPYCMD%  %SRCROOT%\react-native\ReactCommon\jsi\jsi-inl.h                       %DESTROOT%\inc\jsi
%COPYCMD%  %SRCROOT%\react-native\ReactCommon\jsi\V8Runtime.h                     %DESTROOT%\inc\jsi

%COPYCMD%  %SRCROOT%\react-native\ReactCommon\Yoga\Yoga\Yoga.h                    %DESTROOT%\inc\yoga
%COPYCMD%  %SRCROOT%\react-native\ReactCommon\Yoga\Yoga\YGMacros.h                %DESTROOT%\inc\yoga
%COPYCMD%  %SRCROOT%\react-native\ReactCommon\Yoga\Yoga\YGNodeList.h              %DESTROOT%\inc\yoga
%COPYCMD%  %SRCROOT%\react-native\ReactCommon\Yoga\Yoga\YGEnums.h                 %DESTROOT%\inc\yoga

%COPYCMD%  %SRCROOT%\react-native\Folly                                           %DESTROOT%\inc\Folly
%COPYCMD%  %SRCROOT%\react-native-win\stubs                                       %DESTROOT%\inc\stubs
%COPYCMD%  %SRCROOT%\react-native-win\Desktop\*.h                                 %DESTROOT%\inc\ReactWin32
%COPYCMD%  %SRCROOT%\react-native-win\ReactWindowsCore\*.h                        %DESTROOT%\inc\ReactWindowsCore
%COPYCMD%  %SRCROOT%\react-native-win\ReactUWP\*.h                                %DESTROOT%\inc\ReactUWP
%COPYCMD%  %SRCROOT%\react-native-win\include                                     %DESTROOT%\inc\include

%COPYCMD%  %SRCROOT%\react-native-win\ReactWin32.nuspec                           %DESTROOT%

