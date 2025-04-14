@echo off
REM creaternwlib.cmd [name]
REM Creates a RNW lib using the new arch template
REM
REM Options:
REM
REM name            The name of the app to create (default: testlib)
REM /r [version]    Use react@version (default: latest)
REM /rn [version]   Use react-native@version (default: latest)
REM /rnw [version]  Use react-native-windows@version (default: latest)
REM /lt [template]  Use create-react-native-library template (default: turbo-module)
REM /linkrnw        Use your local RNW repo at RNW_ROOT
REM /verdaccio      Configure new project to use verdaccio (used in CI)
REM
REM Requirements:
REM - You've set the RNW_ROOT environment variable with the path to your clone

setlocal enableextensions enabledelayedexpansion

if "%RNW_ROOT%"=="" (
  @echo creaternwlib.cmd: RNW_ROOT environment variable set to %~dp0..\..
  set RNW_ROOT=%~dp0..\..
)

set LIB_NAME=testlib
set RN_TEMPLATE_TYPE=turbo-module
set RNW_TEMPLATE_TYPE=cpp-lib

set R_VERSION=
set RN_VERSION=
set RNCLI_VERSION=
set RNW_VERSION=

set LINK_RNW=0
set USE_VERDACCIO=0

:loop
set part=%1
set param=%2
if not "%part%"=="" (
  if "%part%"=="/linkrnw" (
      set LINK_RNW=1
  ) else if "%part%"=="/verdaccio" (
      set USE_VERDACCIO=1
  ) else if "%part%"=="/r" (
      set R_VERSION=%param%
      shift
  ) else if "%part%"=="/rn" (
      set RN_VERSION=%param%
      shift
  ) else if "%part%"=="/rnw" (
      set RNW_VERSION=%param%
      shift
  ) else if "%part%"=="/lt" (
      set RN_TEMPLATE_TYPE=%param%
      shift
  ) else if "%part:~0,1%"=="/" (
      @echo creaternwlib.cmd: Unknown flag "%part%"
      exit /b 1
  ) else (
      set LIB_NAME=%part%
  )
  shift
  goto :loop
)
:loopend

if %USE_VERDACCIO% equ 1 (
  @echo creaternwlib.cmd: Setting npm to use verdaccio at http://localhost:4873
  call npm config set registry http://localhost:4873
)

if %LINK_RNW% equ 1 (
  @echo creaternwlib.cmd Determining versions from local RNW repo at %RNW_ROOT%
  for /f "delims=" %%a in ('npm show "%RNW_ROOT%\vnext" devDependencies.react') do @set R_VERSION=%%a
  for /f "delims=" %%a in ('npm show "%RNW_ROOT%\vnext" devDependencies.react-native') do @set RN_VERSION=%%a
  for /f "delims=" %%a in ('npm show "%RNW_ROOT%\vnext" version') do @set RNW_VERSION=%%a
  for /f "delims=" %%a in ('npm show "%RNW_ROOT%\vnext" dependencies.@react-native-community/cli') do @set RNCLI_VERSION=%%a
)

if "%RNW_VERSION%"=="" (
  @echo creaternwlib.cmd Defaulting react-native-windows version to latest
  set RNW_VERSION=latest
)

if "%RN_VERSION%"=="" (
  @echo creaternwlib.cmd Determining react-native version from react-native-windows dependency
  for /f "delims=" %%a in ('npm show react-native-windows@%RNW_VERSION% devDependencies.react-native') do @set RN_VERSION=%%a
)

if "%RNCLI_VERSION%"=="" (
  @echo creaternwlib.cmd Determining @react-native-community/cli version from react-native-windows dependency
  for /f "delims=" %%a in ('npm show react-native-windows@%RNW_VERSION% dependencies.@react-native-community/cli') do @set RNCLI_VERSION=%%a
)

if "%R_VERSION%"=="" (
  @echo creaternwlib.cmd Determining react version from react-native-windows dependency
  for /f "delims=" %%a in ('npm show react-native-windows@%RNW_VERSION% devDependencies.react') do @set R_VERSION=%%a
)

@echo creaternwlib.cmd Determining concrete versions for react@%R_VERSION%, react-native@%RN_VERSION%, and react-native-windows@%RNW_VERSION% 
for /f "delims=" %%a in ('npm show react-native-windows@%RNW_VERSION% version') do @set RNW_VERSION=%%a
for /f "delims=" %%a in ('npm show react-native@%RN_VERSION% version') do @set RN_VERSION=%%a
for /f "delims=" %%a in ('npm show @react-native-community/cli@%RNCLI_VERSION% version') do @set RNCLI_VERSION=%%a
for /f "delims=" %%a in ('npm show react@%R_VERSION% version') do @set R_VERSION=%%a

@echo creaternwlib.cmd Creating RNW lib "%LIB_NAME%" with react@%R_VERSION%, react-native@%RN_VERSION%, and react-native-windows@%RNW_VERSION%

@echo creaternwlib.cmd Creating base RN library project with: npx --yes create-react-native-library@latest --slug %LIB_NAME% --description %LIB_NAME% --author-name "React-Native-Windows Bot" --author-email 53619745+rnbot@users.noreply.github.com --author-url http://example.com --repo-url http://example.com --languages kotlin-objc --type %RN_TEMPLATE_TYPE% --react-native-version %RN_VERSION% --example vanilla %LIB_NAME%
call npx --yes create-react-native-library@latest --slug %LIB_NAME% --description %LIB_NAME% --author-name "React-Native-Windows Bot" --author-email 53619745+rnbot@users.noreply.github.com --author-url http://example.com --repo-url http://example.com --languages kotlin-objc --type %RN_TEMPLATE_TYPE% --react-native-version %RN_VERSION% --example vanilla %LIB_NAME%

if %ERRORLEVEL% neq 0 (
  @echo creaternwlib.cmd: Unable to create base RN library project
  exit /b %ERRORLEVEL%
)

pushd "%LIB_NAME%"

if not "x%RN_VERSION:nightly=%"=="x%RN_VERSION%" (
  @echo creaternwlib.cmd Fixing react-native nightly issue
  pwsh.exe -Command "(gc package.json) -replace '""nightly""', '""%RN_VERSION%""' | Out-File -encoding utf8NoBOM package.json"
  pwsh.exe -Command "(gc package.json) -replace '""@react-native-community/cli"": "".*""', '""@react-native-community/cli"": ""%RNCLI_VERSION%""' | Out-File -encoding utf8NoBOM package.json"
  pwsh.exe -Command "(gc package.json) -replace '""@react-native-community/cli-platform-android"": "".*""', '""@react-native-community/cli-platform-android"": ""%RNCLI_VERSION%""' | Out-File -encoding utf8NoBOM package.json"
  pwsh.exe -Command "(gc package.json) -replace '""@react-native-community/cli-platform-ios"": "".*""', '""@react-native-community/cli-platform-ios"": ""%RNCLI_VERSION%""' | Out-File -encoding utf8NoBOM package.json"
  pushd example
  pwsh.exe -Command "(gc package.json) -replace '""nightly""', '""%RN_VERSION%""' | Out-File -encoding utf8NoBOM package.json"
  pwsh.exe -Command "(gc package.json) -replace '""@react-native-community/cli"": "".*""', '""@react-native-community/cli"": ""%RNCLI_VERSION%""' | Out-File -encoding utf8NoBOM package.json"
  pwsh.exe -Command "(gc package.json) -replace '""@react-native-community/cli-platform-android"": "".*""', '""@react-native-community/cli-platform-android"": ""%RNCLI_VERSION%""' | Out-File -encoding utf8NoBOM package.json"
  pwsh.exe -Command "(gc package.json) -replace '""@react-native-community/cli-platform-ios"": "".*""', '""@react-native-community/cli-platform-ios"": ""%RNCLI_VERSION%""' | Out-File -encoding utf8NoBOM package.json"
  popd
)

@echo creaternwlib.cmd: Calling yarn install
call yarn install

if %USE_VERDACCIO% equ 1 (
  @echo creaternwlib.cmd: Setting yarn to use verdaccio at http://localhost:4873
  call yarn config set registry http://localhost:4873
  call yarn config set npmRegistryServer http://localhost:4873
  call yarn config set unsafeHttpWhitelist --json "[\"localhost\"]"
)

@echo creaternwlib.cmd Adding RNW dependency to library
call yarn add react-native-windows@%RNW_VERSION% --dev
call yarn add react-native-windows@* --peer

if %LINK_RNW% equ 1 (
  @echo creaternwlib.cmd Linking RNW dependency to local repo
  call yarn link %RNW_ROOT%\vnext
)

@echo creaternwlib.cmd: Calling yarn install
call yarn install

@echo creaternwlib.cmd Creating commit to save current state
call git add .
call git commit -m "chore: add rnw dependency"

@echo creaternwlib.cmd Running init-windows with: yarn react-native init-windows --template %RNW_TEMPLATE_TYPE% --overwrite --logging
call yarn react-native init-windows --template %RNW_TEMPLATE_TYPE% --overwrite --logging

@echo creaternwlib.cmd Done, see new %RNW_TEMPLATE_TYPE% project in %CD% with react@%R_VERSION%, react-native@%RN_VERSION%, and react-native-windows@%RNW_VERSION%

popd

endlocal

exit /b %ERRORLEVEL%