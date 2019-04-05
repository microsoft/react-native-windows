# ReactNative for Windows 10 (C++)
See the official [React Native website](https://facebook.github.io/react-native/) for an introduction to React Native. The [master branch of this repository]( https://github.com/Microsoft/react-native-windows) adds support for React Native for Windows10 implemented from scratch in C#. It reimplemented the JS<->Native bridge in C#, and shared the JS with Facebook's implementation of React Native.  The current direction of React Native involves a closer interaction between C++ and JS which is hard to achieve with a separate C# implementation.

In this branch, we are working on a rewrite of React Native for Windows10 built in C++ that reuses the C++ core bridge implementation from Facebook’s React Native.  This will allow React Native for Windows10 to innovate and provide features by sharing the same core as Facebook’s React Native. See [Status](https://github.com/Microsoft/react-native-windows#status) for more details.

### Note - ONLY CURRENTLY BUILDABLE USING INTERNAL CREDS
We are actively working on removing all dependencies internal to Microsoft. This should be completed in the next month.
Issues remaining : [#2170](https://github.com/Microsoft/react-native-windows/issues/2170), [#2169](https://github.com/Microsoft/react-native-windows/issues/2169), [#2154](https://github.com/Microsoft/react-native-windows/issues/2154), [#2153](https://github.com/Microsoft/react-native-windows/issues/2153) 

# Getting Started
This is a summary of setup steps needed to install and work with React Native for Windows10 (C++). See the [React Native Getting Started Guide](http://facebook.github.io/react-native/docs/getting-started.html) for React Native details.

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
* [Chrome](https://www.google.com/chrome/) (*optional*, but needed for JS debugging)
* [NuGet](https://dist.nuget.org/index.html)<br/>
  Add `NuGet.exe`'s location to `PATH`.
* [NuGet Credential Provider](https://nuget.pkgs.visualstudio.com/_apis/public/nuget/client/CredentialProviderBundle.zip)<br/>
  Add `CredentialProvider.VSS.exe`'s location to `PATH`.


## Build Steps
1. Clone the repo
    ```cmd
    git clone https://github.com/Microsoft/react-native-windows.git
    cd react-native-windows
    ```
2. Checkout the rnwcpp-preview branch
    ```cmd
    git checkout rnwcpp-preview
    ```
3. Install dependencies. This step may take a while on the first run due to dependency download.
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

4. Build solution.
    * Using MSBuild
    ```cmd
    MSBuild.exe [/p:Platform=$(TargetPlatform)] [/p:Configuration=$(TargetConfiguration)]
    ```

    * Using Visual Studio IDE
      1. Open `ReactWindows-UWP.sln`.
      2. Set your `Platform` to `x86` or `x64` and `Configuration ` to `Debug`.
      3. Select `Project / Build Solution (Ctrl+Shift+B)`

5. *[Optional]* Install [react-devtools](https://github.com/facebook/react-devtools/tree/master/packages/react-devtools) for enhanced JS debugging:

    ```cmd
   npm install -g react-devtools
   ```

## Running the Sample Universal Windows App
1. Run `npm run build`.
2. Run `Scripts\launchPackager.bat`.
3. *[Optional]* If you want to debug the JS code:
   - Make sure Chrome is running if you're not already running it
   - If you want to use the react-devtools that you installed earlier, run `react-devtools`
4. In Visual Studio, set React.Windows.Universal.SampleApp as the StartUp Project.
   - If you didn't already, make sure to set your `Platform` to `x86` or `x64` and `Configuration ` to `Debug`.
5. Run project (`F5` or `Debug / Start Debugging`).
6. If you chose to skip step 3 above, make sure to uncheck the "Web Debugger" checkbox.
7. Press the "Load" button on the left side of the Windows 10 application window that appears.

The selected ReactNative app (defaulted to `Bootstrap`) should start in the bottom of the application window.

Try these samples by entering the JS file name and App names below into the textboxes at the top of the application window before pressing "Load":
   - Sample: JavaScript file: `Universal.SampleApp\index.uwp` App Name: `Bootstrap`
   - RNTester: JavaScript file: `lib\RNTester\RNTesterApp.uwp` App Name: `RNTesterApp`

If you did choose to debug the JS code with "Web Debugger" enabled, Chrome should have loaded `http://localhost:8081/debugger-ui/` in a new tab. Press `F12` or `Ctrl+Shift+I` in Chrome to open its Developer Tools.

**Note:** If nothing happened, make sure you started the packager (and if debugging the JS code, Chrome and the react-devtools) before step 7.

## Running Unit Tests
1. Run `Scripts\UnitTest.ps1 [-Platform <test platform> -Configuration <test configuration>]`.

## Running Integration Tests
### PowerShell
1. Run `Scripts\IntegrationTests.ps1 -Run`. This runs all Desktop tests.
### Visual Studio
1. Run `Scripts\IntegrationTests.ps1` to start the JS bundler and WebSocket server.
2. In the `Test Explorer` window, select and run any desired tests.

Defaults for platform and configuration are `x64`, `Debug`.

