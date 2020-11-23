@echo off
setlocal

set version=%1
set targetDir=%2
set slices=%3
set basePlatform=%4
set baseConfiguration=%5

set ScriptFolder=%~dp0

set defaultTargetDir=c:\temp\RnWNugetTesting
set defaultSlices="@('x64.Debug')"
set defaultBaseConfiguration=Debug
set defaultBasePlatform=x64

echo Invoking publish nuget packages with: %0 %*

:PrepVersion
    if "%version%" == "" (
        echo Usage: %~nx0 ^<version^> [^<targetDir^>] [^<slices^>] [^<basePlatform^>] [^<baseConfiguration^>]
        echo.
        echo Example: %~nx0 0.0.1-Test001
        echo Example: %~nx0 0.0.1-Test001 %defaultTargetDir% %defaultSlices%
        echo Example: %~nx0 0.0.1-Test001 %defaultTargetDir% %defaultSlices% %defaultBasePlatform% %defaultBaseConfiguration%
        exit /b 1
    )
    if "%baseConfiguration%" == "" (
        set baseConfiguration=%defaultBaseConfiguration%
    )
    if "%basePlatform%" == "" (
        set basePlatform=%defaultBasePlatform%
    )

:PrepTargetDir
    if "%targetDir%" == "" (
        set targetDir=%defaultTargetDir%
    )

    if EXIST %targetDir% (
        rmdir /s/q %targetDir%
    )
    mkdir %targetDir%\pkgs
    mkdir %targetDir%\feed

:PrepSlices
    if "%slices%" == "" (
        set slices=%defaultSlices%
    )

:PrepCodeGen
    set publishArgs=/p:Configuration=%baseConfiguration% /p:Platform=x64 /p:DeployOnBuild=true /p:PublishProfile=DeployAsTool-%baseConfiguration% /p:NoWarn=1023
    msbuild %ScriptFolder%..\Microsoft.ReactNative.Managed.CodeGen\Microsoft.ReactNative.Managed.CodeGen.csproj /t:Publish /v:m %publishArgs%
    if ERRORLEVEL 1 (
        Failed to publish Managed CodeGen 
        exti /b 1
    )

:PrepFiles
    powershell %ScriptFolder%tfs\Layout-Headers.ps1

call :ProcessNuget Microsoft.ReactNative                 strip -preferRelease $true
call :ProcessNuget Microsoft.ReactNative.Cxx             nostrip
call :ProcessNuget Microsoft.ReactNative.Managed         strip -preferRelease $true
call :ProcessNuget Microsoft.ReactNative.Managed.CodeGen nostrip
exit /b 0

:ProcessNuget
    set packageId=%1
    set strip=%2
    set nuspecFile=%1.nuspec
    set targetNuspec=%targetDir%\pkgs\%nuspecFile%
    set targetNupkg=%targetDir%\pkgs\%packageId%.%version%.nupkg
    set nugetRoot=%ScriptFolder%..\target
    echo.
    echo.
    echo --------------------------------
    echo Processing %packageId%
    echo --------------------------------
    if '%strip%' == 'strip' (
        powershell %ScriptFolder%StripAdditionalPlatformsFromNuspec.ps1 -nuspec %scriptFolder%%nuspecFile% -outFile %targetNuspec% -slices %slices% %3 %4 %5 %6
    ) else (
        copy %scriptFolder%%nuspecFile% %targetNuspec% /y
    )
    echo nuget pack %targetNuspec% -properties CommitId=TestCommit;version=%version%;id=%packageId%;nugetroot=%nugetRoot%;baseconfiguration=%baseConfiguration%;baseplatform=%basePlatform%
    nuget pack %targetNuspec% -OutputDirectory %targetDir%\pkgs -properties CommitId=TestCommit;version=%version%;id=%packageId%;nugetroot=%nugetRoot%;baseconfiguration=%baseConfiguration%;baseplatform=%basePlatform%;NoWarn=NU5100
    nuget add %targetNupkg% -Source %targetDir%\feed
    goto :EOF
