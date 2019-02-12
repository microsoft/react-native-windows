# ReactNative for Windows 10 (C++)
See the official [React Native website](https://facebook.github.io/react-native/) for an introduction to React-Native. The [master branch of this repository]( https://github.com/Microsoft/react-native-windows) adds support for React-Native for Windows10 implemented from scratch in C#. It reimplemented the JS<->Native bridge in C#, and shared the JS with Facebook's implementation of React-Native.  The current direction of React-Native involves a closer interaction between C++ and JS which is hard to achieve with a separate C# implementation.

In this branch, we are working on a rewrite of React-Native for Windows10 built in C++ that reuses the C++ core bridge implementation from Facebook’s React-Native.  This will allow React-Native for Windows10 to innovate and provide features by sharing the same core as Facebook’s React-Native.

### Note - ONLY CURRENTLY BUILDABLE USING INTERNAL CREDS
We are actively working on removing all dependencies internal to Microsoft. This should be completed in the next month.

# Getting Started
This is a summary of setup steps needed to install and work with React-Native for Windows10 (C++). See the [React Native Getting Started Guide](http://facebook.github.io/react-native/docs/getting-started.html) for React Native details.

## System requirements
* You can run React-Native for Windows10 apps only on Windows 10 devices; sdk version: 10.0.14393.795 or higher.
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

## Install dependencies
* [Git](https://git-scm.com/download/win)
* [Node.js](https://nodejs.org) (last verified with version 8.14.0)
* [NuGet](https://dist.nuget.org/index.html)<br/>
  Add `NuGet.exe`'s location to `PATH`.
* [NuGet Credential Provider](https://nuget.pkgs.visualstudio.com/_apis/public/nuget/client/CredentialProviderBundle.zip)<br/>
  Add `CredentialProvider.VSS.exe`'s location to `PATH`.


## Build Steps
1. Clone the repo [https://github.com/Microsoft/react-native-windows.git]
2. Install dependencies. This step may take a while on the first run due to dependency download.
    ```cmd
    cd RNWCPP
    npm install
    ```

    If the above command results in an error message indicating missing authentication credentials (e.g.
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


## Running the Sample Universal Windows App
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

