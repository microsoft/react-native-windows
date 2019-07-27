# Getting Started Guide for React Native Windows (vnext)

This is a summary of setup steps needed to install and work with React Native for Windows (vnext). See the [React Native Getting Started Guide](http://facebook.github.io/react-native/docs/getting-started.html) for React Native details and see [Getting Started Guide - current](https://github.com/microsoft/react-native-windows/blob/master/current/docs/GettingStarted.md) for working with the `current` React Native for Windows implementation.

## System requirements
* You can run React-Native for Windows10 apps only on Windows 10 devices and Windows version: 10.0.15063.0 or higher.
* [Visual Studio 2019](https://www.visualstudio.com/downloads) with the following options:
  * Workloads
    * Universal Windows Platform development
      * Enable the optional `C++ (v141) Universal Windows Platform tools`
    * Desktop development with C++
  * Individual Components
    * Development activities
      * Node.js development support (optional)
    * SDKs, libraries, and frameworks per your versioning needs
      * Windows 10 SDK (10.0.18362.0)

## Dependencies
* Install the dependencies [specified by React Native](http://facebook.github.io/react-native/docs/getting-started.html#node-python2-jdk). Specifically, make sure a recent version of [Node.js](https://nodejs.org) is installed. [Chocolatey](https://chocolatey.org/) is the React Native recommended installation method. On an elevated Command Prompt, run:
  ```
  choco install nodejs
  ```
* Install [Chrome](https://www.google.com/chrome/) (*optional*, but needed for JS debugging)

## Installing React Native for Windows (vnext)
There are two ways to install and work with React Native for Windows (vnext):
1. [Simple](#simple-install-method) : Using our simple CLI that does all of the work for you behind the scenes.
2. [Advanced](#advanced-install-method) : Manually by cloning the repo and running all install steps yourself.

### 1. Simple install method

#### Installing React Native

* Install React Native command line interface using NPM:
  ```
  npm install -g react-native-cli
  ```
* Initialize your React Native project

  Next, [generate a React Native project](http://facebook.github.io/react-native/docs/getting-started.html#creating-a-new-application). In the directory you would like your React Native Windows project directory, run:
  ```
  react-native init <project name> --version 0.59.10
  ```
  Navigate into this newly created directory:
  ```
  cd <project name>
  ```

#### Installing React Native Windows (vnext)

* Install the React Native Windows command line interface ([rnpm-plugin-windows](https://www.npmjs.com/package/rnpm-plugin-windows)).

  If you are using Yarn, run
  ```
  yarn add rnpm-plugin-windows
  ```
  Alternatively, if you are using NPM, run
  ```
  npm install --save rnpm-plugin-windows
  ```

* Next, initialize your React Native Windows `vnext` in the project directory. This step may take a while during first run due to dependency download.
  ```
  react-native windows --template vnext
  ```

#### Running a React Native Windows App

*Note: Make sure Chrome is launched and running before running a React Native Windows app.*

- Without Visual Studio

  In your React Native Windows project directory, run:
  ```
  react-native run-windows
  ```
  A new Command Prompt window will open with the React packager as well as a `react-native-windows` app. This step may take a while during first run since it involves building the entire project and all dependencies. You can now start developing! :tada:

- With Visual Studio

   - Open the solution file in the application folder in Visual Studio (e.g., `AwesomeProject/windows/AwesomeProject.sln`)
   - Select the `Debug` configuration and the `x64` platform from the combo box controls to the left of the `Run` button and underneath the `Team` and `Tools` menu item.
   - Run `yarn start` from your project directory, and wait for the React Native packager to report success.
   - Click the `Run` button to the right of the platform combo box control in VS, or select the `Debug`->`Start without Debugging` menu item. You now see your new app and Chrome should have loaded `http://localhost:8081/debugger-ui/` in a new tab. Press `F12` or `Ctrl+Shift+I` in Chrome to open its Developer Tools. :tada:


### 2. Advanced install method

- Make sure you have installed [dependencies](#dependencies)
- Install [Git](https://git-scm.com/download/win) if you don't have it installed in your development machine

#### Build Steps
* Clone the repo
    ```cmd
    git clone https://github.com/microsoft/react-native-windows.git
    cd react-native-windows
    ```

* Install dependencies. This step may take a while on the first run due to dependency download.
    ```cmd
    yarn
    cd vnext
    nuget restore
    ```

* Run `yarn build` in the vnext folder.

* Run `Scripts\launchPackager.bat`.	This is needed to ensure the JS files can be packaged and bundled to the UWP app.

* Make sure Chrome is running if you're not already running it

#### Running the Playground app

* Build solution.
    * Using MSBuild
    ```cmd
    MSBuild.exe [/p:Platform=$(TargetPlatform)] [/p:Configuration=$(TargetConfiguration)]
    ```

    * Using Visual Studio IDE
      1. Open `Playground.sln`.
      2. Set your `Platform` to `x86` or `x64` and `Configuration ` to `Debug`.
      3. Select `Project / Build Solution (Ctrl+Shift+B)`

* In Visual Studio, set Playground as the StartUp Project.

* Run project (`F5` or `Debug / Start Debugging`).

You now see your new app and Chrome should have loaded `http://localhost:8081/debugger-ui/` in a new tab. Press `F12` or `Ctrl+Shift+I` in Chrome to open its Developer Tools. :tada:

#### Running the Sample Universal Windows App
* Build solution.
    * Using MSBuild
    ```cmd
    MSBuild.exe [/p:Platform=$(TargetPlatform)] [/p:Configuration=$(TargetConfiguration)]
    ```

    * Using Visual Studio IDE
      1. Open `ReactWindows-UWP.sln`.
      2. Set your `Platform` to `x86` or `x64` and `Configuration ` to `Debug`.
      3. Select `Project / Build Solution (Ctrl+Shift+B)`


* In Visual Studio, set React.Windows.Universal.SampleApp as the StartUp Project.

* If you didn't already, make sure to set your `Platform` to `x86` or `x64` and `Configuration ` to `Debug`.

* Run project (`F5` or `Debug / Start Debugging`).

* Press the "Load" button on the left side of the Windows 10 application window that appears.

 The selected React Native component (defaulted to `Bootstrap`) should get loaded in the bottom of the application window. Chrome should have loaded `http://localhost:8081/debugger-ui/` in a new tab. Press `F12` or `Ctrl+Shift+I` in Chrome to open its Developer Tools. :tada:

 Try these samples by entering the JS file name and App names below into the textboxes at the top of the application window before pressing "Load":
   - Sample: JavaScript file: `Universal.SampleApp\index.uwp` App Name: `Bootstrap`
   - RNTester: JavaScript file: `RNTester\RNTesterApp.uwp` App Name: `RNTesterApp`

## Troubleshooting
* If after running the app the packager does not update (or) app does not show React Native content - close the packager command prompt window and the app, run `yarn start` and run the app again.  Issue [#2311](https://github.com/microsoft/react-native-windows/issues/2311) is tracking a known issue on this.
* If you get a red error box in your UWP app window with the error message : `ERROR: Instance failed to start. A connection with the server cannot be established`, make sure you have the packager running using `yarn start` and run the app again.
* If you are trying to run your `react-native` app on iOS/Android while using this `vnext` implementation for developing/running on Windows, you will encounter errors while running the app for other platforms. This will be fixed once we address Issues [#2264](https://github.com/microsoft/react-native-windows/issues/2264) and [#2535](https://github.com/microsoft/react-native-windows/issues/2535). Until this is fixed, please refer to [this comment](https://github.com/microsoft/react-native-windows/issues/2515#issuecomment-497375198) which describes the workaround for running on other platforms while developing for windows using `vnext`.
