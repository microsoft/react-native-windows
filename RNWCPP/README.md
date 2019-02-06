# Note - ONLY CURRENTLY BUILDABLE USING INTERNAL CREDS
We are actively working on removing all internal dependencies.  This should be completely in the coming months.

# Intro
The original react-native-windows implementation was written from scratch in C#.  It reimplemented the JS<->Native bridge in C#, and shared the JS with facebook's implementation of React-Native.  The current direction of react-native involves a closer interaction between C++ and JS which is hard to achieve with this seperate C# implementation.

This is a rewrite of react-native-windows built in C++ that reuses the C++ core bridge implementation in react-native.  This will allow react-native-windows to innovate and provide features that will only be possible by sharing the same core as react-native.

# Building

## Prerequisites
* [Git](https://git-scm.com/download/win)
* [Node.js](https://nodejs.org)
* [NuGet](https://dist.nuget.org/index.html)<br/>
  Add `NuGet.exe`'s location to `PATH`.
* [NuGet Credential Provider](https://nuget.pkgs.visualstudio.com/_apis/public/nuget/client/CredentialProviderBundle.zip)<br/>
  Add `CredentialProvider.VSS.exe`'s location to `PATH`.
* [Visual Studio 2017](https://www.visualstudio.com/downloads) with the following options:
  * Workloads
    * Universal Windows Platform development
      * Enable the optional 'C++ Universal Windows Platform tools'
    * Desktop development with C++
  * Individual Components
    * Development activities
      * Node.js development support
    * SDKs, libraries, and frameworks
      * Windows 10 SDK (10.0.14393.0)
      * Windows 10 SDK (10.0.17763.0)

## Build Steps
1. Install dependencies. This step may take a while on the first run due to dependency download.
    ```cmd
    npm install
    nuget restore
    ```

    If the above command results in an error message indicating missing authentication crendentials (e.g.
    `... npm ERR! This request requires auth credentials ...`), run

    ```cmd
    npm install -g vsts-npm-auth --registry https://registry.npmjs.com
    vsts-npm-auth -config .npmrc
    ```

3. Build solution.
    * Using MSBuild
    ```cmd
    MSBuild.exe [/p:Platform=$(TargetPlatform)] [/p:Configuration=$(TargetConfiguration)]
    ```

    * Using Visual Studio IDE
      1. Open `ReactWindows.sln`.
      1. Select `Project / Build Solution (Ctrl+Shift+B)`

# Testing

## Sample Universal Windows App
1. Run `npm run build`.
1. Run `Scripts\launchPackager.bat`.
1. In Visual Studio, set React.Windows.Universal.SampleApp as the StartUp Project.
1. Run project (`F5` or `Debug / Start Debugging`).
1. Open your browser if not already running (or the next step may hang opening it)
1. Press the "Load" button on the left side of the applicatoin window that appears.<br/>
Browser should open showing a page for the React Native JS tools. Press F12 to open the tools.
1. Try these samples:
   - Sample: JavaScript file: `Universal.SampleApp\index.uwp` App Name: `Bootstrap`
   - RNTester: JavaScript file: `lib\RNTester\RNTesterApp.uwp` App Name: `RNTesterApp`

## Running Unit Tests
1. Run `Scripts\UnitTest.ps1 [-Platform <test platform> -Configuration <test configuration>]`.

## Running Integration Tests
### PowerShell
1. Run `Scripts\IntegrationTests.ps1 -Run`. This runs all Desktop tests.
### Visual Studio
1. Run `Scripts\IntegrationTests.ps1` to start the JS bundler and WebSocket server.
1. In the `Test Explorer` window, select and run any desired tests.

Defaults for platform and configuration are `x64`, `Debug`.
