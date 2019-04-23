# Getting Started Guide for React Native Windows C++

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

## Dependencies
* Install the dependencies [specified by React Native](http://facebook.github.io/react-native/docs/getting-started.html#node-python2-jdk). Specifically, make sure a recent version of [Node.js](https://nodejs.org) is installed. [Chocolatey](https://chocolatey.org/) is the React Native recommended installation method. On an elevated Command Prompt, run:
```
choco install nodejs
```
* Install [Chrome](https://www.google.com/chrome/) (*optional*, but needed for JS debugging)

## Installing React Native

* Install React Native command line interface using NPM:
```
npm install -g react-native-cli
```
* Initialize your React Native project

Next, [generate a React Native project](http://facebook.github.io/react-native/docs/getting-started.html#creating-a-new-application). In the directory you would like your React Native Windows project directory, run:
```
react-native init <project name>
```
Navigate into this newly created directory:
```
cd <project name>
```

## Installing React Native Windows C++

* Install the React Native Windows command line interface ([rnpm-plugin-windows](https://www.npmjs.com/package/rnpm-plugin-windows)).
If you are using NPM, run
```
npm install --save rnpm-plugin-windows
```
If you are using Yarn, run
```
yarn add rnpm-plugin-windows
```

* Next, initialize your React Native Windows C++ project in the project directory by running:
```
react-native windows --template vnext
```

## Running a React Native Windows App

*Note: Make sure Chrome is launched and running before running a React Native Windows app.*

### Without Visual Studio

In your React Native Windows project directory, run:
```
react-native run-windows
```
A new Command Prompt window will open with the React packager as well as a React Native Windows app. You can now start developing! :tada:

### With Visual Studio

- Open the solution file in the application folder in Visual Studio (e.g., `AwesomeProject/windows/AwesomeProject.sln`)
- Select the `Debug` configuration and the `x64` platform from the combo box controls to the left of the `Run` button and underneath the `Team` and `Tools` menu item.
- Run `yarn start` from your project directory, and wait for the React Native packager to report success.
- Click the `Run` button to the right of the platform combo box control in VS, or select the `Debug`->`Start without Debugging` menu item. You now see your new app! :tada:

## Troubleshooting
* Use VS 2017. Issue [#2320](https://github.com/Microsoft/react-native-windows/issues/2320) tracks support for CLI with VS 2019.
* If after running the app the packager does not update (or) app does not show React Native content - close the packager command prompt window and the app, run `yarn start` and run the app again.  Issue [#2311](https://github.com/Microsoft/react-native-windows/issues/2311) is tracking a known issue on this.
