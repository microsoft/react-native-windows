@echo off
REM creaternwapp.cmd [name]
REM Creates a RNW app using the new arch template
REM
REM Options:
REM
REM name            The name of the app to create (default: testapp)
REM /r [version]    Use react@version (default: latest)
REM /rn [version]   Use react-native@version (default: latest)
REM /rnw [version]  Use react-native-windows@version (default: latest)
REM /t [template]   Use template (default: cpp-app)
REM /linkrnw        Use your local RNW repo at RNW_ROOT
REM /verdaccio      Configure new project to use verdaccio (used in CI)
REM
REM Requirements:
REM - You've set the RNW_ROOT environment variable with the path to your clone

setlocal enableextensions enabledelayedexpansion

call git rev-parse --is-inside-work-tree > NUL 2>&1
if %ERRORLEVEL% equ 0 (
  @echo creaternwapp.cmd: Unable to create a new project in an existing git repo
  exit /b -1
)

if "%RNW_ROOT%"=="" (
  @echo creaternwapp.cmd: RNW_ROOT environment variable set to %~dp0..\..
  set RNW_ROOT=%~dp0..\..
)

set APP_NAME=testapp
set RNW_TEMPLATE_TYPE=cpp-app

set R_VERSION=
set RN_VERSION=
set RNW_VERSION=
set RNCLI_VERSION=

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
  ) else if "%part%"=="/t" (
      set RNW_TEMPLATE_TYPE=%param%
      shift
  ) else if "%part:~0,1%"=="/" (
      @echo creaternwapp.cmd: Unknown flag "%part%"
      exit /b 1
  ) else (
      set APP_NAME=%part%
  )
  shift
  goto :loop
)
:loopend

if %USE_VERDACCIO% equ 1 (
  @echo creaternwapp.cmd: Setting npm to use verdaccio at http://localhost:4873
  call npm config set registry http://localhost:4873
)

if %LINK_RNW% equ 1 (
  @echo creaternwapp.cmd Determining versions from local RNW repo at %RNW_ROOT%
  for /f "delims=" %%a in ('npm show "%RNW_ROOT%\vnext" devDependencies.react') do @set R_VERSION=%%a
  for /f "delims=" %%a in ('npm show "%RNW_ROOT%\vnext" devDependencies.react-native') do @set RN_VERSION=%%a
  for /f "delims=" %%a in ('npm show "%RNW_ROOT%\vnext" version') do @set RNW_VERSION=%%a
  for /f "delims=" %%a in ('npm show "%RNW_ROOT%\vnext" dependencies.@react-native-community/cli') do @set RNCLI_VERSION=%%a
)

if "%RNW_VERSION%"=="" (
  @echo creaternwapp.cmd Defaulting react-native-windows version to latest
  set RNW_VERSION=latest
)

if "%RN_VERSION%"=="" (
  @echo creaternwapp.cmd Determining react-native version from react-native-windows dependency
  for /f "delims=" %%a in ('npm show react-native-windows@%RNW_VERSION% devDependencies.react-native') do @set RN_VERSION=%%a
)

if "%RNCLI_VERSION%"=="" (
  @echo creaternwapp.cmd Determining @react-native-community/cli version from react-native-windows dependency
  for /f "delims=" %%a in ('npm show react-native-windows@%RNW_VERSION% dependencies.@react-native-community/cli') do @set RNCLI_VERSION=%%a
)

if "%R_VERSION%"=="" (
  @echo creaternwapp.cmd Determining react version from react-native-windows dependency
  for /f "delims=" %%a in ('npm show react-native-windows@%RNW_VERSION% devDependencies.react') do @set R_VERSION=%%a
)

@echo creaternwapp.cmd Determining concrete versions for react@%R_VERSION%, react-native@%RN_VERSION%, @react-native-community/cli@%RNCLI_VERSION%, and react-native-windows@%RNW_VERSION% 
for /f "delims=" %%a in ('npm show react-native-windows@%RNW_VERSION% version') do @set RNW_VERSION=%%a
for /f "delims=" %%a in ('npm show react-native@%RN_VERSION% version') do @set RN_VERSION=%%a
for /f "delims=" %%a in ('npm show @react-native-community/cli@%RNCLI_VERSION% version') do @set RNCLI_VERSION=%%a
for /f "delims=" %%a in ('npm show react@%R_VERSION% version') do @set R_VERSION=%%a

@echo creaternwapp.cmd Creating RNW app "%APP_NAME%" with react@%R_VERSION%, react-native@%RN_VERSION%, and react-native-windows@%RNW_VERSION%

set RNCLI_TEMPLATE=
if not "x%RN_VERSION:nightly=%"=="x%RN_VERSION%" (
  @echo creaternwapp.cmd Override @react-native-community/template version
  for /f "tokens=1 delims=-" %%a in ("%RN_VERSION%") do @set CLEAN_RN_VERSION=%%a
  set RNCLI_TEMPLATE=--template "@react-native-community/template@^%CLEAN_RN_VERSION%"
)

@echo creaternwapp.cmd: Creating base RN app project with: npx --yes @react-native-community/cli@latest init %APP_NAME% --version %RN_VERSION% %RNCLI_TEMPLATE% --verbose --skip-install --install-pods false --skip-git-init true
call npx --yes @react-native-community/cli@latest init %APP_NAME% --version %RN_VERSION% %RNCLI_TEMPLATE% --verbose --skip-install --install-pods false --skip-git-init true

if %ERRORLEVEL% neq 0 (
  @echo creaternwapp.cmd: Unable to create base RN app project
  exit /b %ERRORLEVEL%
)

pushd "%APP_NAME%"

if not "x%RN_VERSION:nightly=%"=="x%RN_VERSION%" (
  @echo creaternwapp.cmd Fixing react-native nightly issues
  pwsh.exe -Command "(gc package.json) -replace '""react-native"": ""[^\*]*""', '""react-native"": ""%RN_VERSION%""' | Out-File -encoding utf8NoBOM package.json"
  pwsh.exe -Command "(gc package.json) -replace '""@react-native/(.+-(config|preset))"": "".*""', '""@react-native/$1"": ""%RN_VERSION%""' | Out-File -encoding utf8NoBOM package.json"
  pwsh.exe -Command "(gc package.json) -replace '""@react-native-community/cli((-platform-)?(ios|android))?"": "".*""', '""@react-native-community/cli$1"": ""%RNCLI_VERSION%""' | Out-File -encoding utf8NoBOM package.json"
)

@echo creaternwapp.cmd: Calling yarn install
call yarn install

@echo creaternwapp.cmd: Creating commit to save current state
if not exist ".git\" call git init .
call git add .
call git commit -m "npx --yes @react-native-community/cli@latest init %APP_NAME% --version %RN_VERSION% %RNCLI_TEMPLATE% --verbose --skip-install --install-pods false --skip-git-init true"

if %USE_VERDACCIO% equ 1 (
  @echo creaternwapp.cmd: Setting yarn to use verdaccio at http://localhost:4873
  call yarn config set registry http://localhost:4873
  call yarn config set npmRegistryServer http://localhost:4873
  call yarn config set unsafeHttpWhitelist --json "[\"localhost\"]"
)

@echo creaternwapp.cmd: Adding RNW dependency to app
call yarn add react-native-windows@%RNW_VERSION%

if %LINK_RNW% equ 1 (
  @echo creaternwapp.cmd: Linking RNW dependency to local repo
  if exist ".yarnrc.yml" (
    call yarn link %RNW_ROOT%\vnext
  ) else (
    pushd %RNW_ROOT%\vnext
    call yarn link
    popd
    call yarn link react-native-windows
  )
)

@echo creaternwapp.cmd: Calling yarn install
call yarn install

@echo creaternwapp.cmd: Creating commit to save current state
call git add .
call git commit -m "add rnw dependency"

@echo creaternwapp.cmd Running init-windows with: npx --yes @react-native-community/cli@latest init-windows --template %RNW_TEMPLATE_TYPE% --overwrite --logging
call npx --yes @react-native-community/cli@latest init-windows --template %RNW_TEMPLATE_TYPE% --overwrite --logging

@echo creaternwapp.cmd Done, see new %RNW_TEMPLATE_TYPE% project in %CD% with react@%R_VERSION%, react-native@%RN_VERSION%, and react-native-windows@%RNW_VERSION%

popd

endlocal

exit /b %ERRORLEVEL%