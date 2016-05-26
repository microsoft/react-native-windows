# React Native plugin for Universal Windows Platform (UWP)

[React Native](http://facebook.github.io/react-native) enables you to build world-class application experiences on native platforms using a consistent developer experience based on JavaScript and [React](http://facebook.github.io/react). The focus of React Native is on developer efficiency across all the platforms you care about - learn once, write anywhere. Facebook uses React Native in multiple production apps and will continue investing in React Native.

This repository adds support for the [Windows 10 SDK](https://developer.microsoft.com/en-us/windows/downloads), which allows you to build apps for:
* Windows 10
* Windows 10 Mobile
* [Xbox One (UWP)](https://msdn.microsoft.com/en-us/windows/uwp/xbox-apps/index)

## Getting Started

- Follow the [Getting Started guide](http://facebook.github.io/react-native/docs/getting-started.html) to install React Native and its dependencies.
- Install [react-native-windows](https://www.npmjs.com/package/react-native-windows) from NPM.
- Install the React Developer Tools for [Chrome](https://chrome.google.com/webstore/detail/react-developer-tools/fmkadmapgofadopljbjfkapdkoienihi) or [Firefox](https://addons.mozilla.org/en-US/firefox/addon/react-devtools/) for better debugging [(read more)](http://facebook.github.io/react-native/docs/debugging.html).

## Requirements

- [Visual Studio 2015](https://msdn.microsoft.com/en-us/library/dd831853.aspx): Community and all other editions supported
- [Windows 10 SDK](https://developer.microsoft.com/en-US/windows/downloads/windows-10-sdk): Download the latests
- [Node.js v5.10.1+](https://nodejs.org/)
- [NPM v3+](https://www.npmjs.com/)
- [Python 2.7](https://www.python.org/): Download the latest edition
- Git Client - For example:
    - [GitHub Desktop for Windows](https://desktop.github.com/)
    - [Git for Windows](https://git-for-windows.github.io/)

## Supported Operating Systems

This setup has only been tested on the following:
- Windows 10 November update
- Windows 10 Mobile November update
- Xbox One

## Installation

First, clone the repository to your local machine which should default to the `ReactWindows` branch:
```bash
git clone https://github.com/ReactWindows/react-native.git
cd react-native
```

Next, install the NPM dependencies.  Note that this will not work using MSYS such as the Git Bash, so launch a regular command prompt if you do not have Git in your `PATH` environment variable.  This will cause a failure for installing `fsevents` which is not supported on Windows.  If you have Git installed in your `PATH` environment variable and use the command prompt, then it will work properly.  If you are using the Git Bash, it may error on installing `fsevents`, so use the regular command prompt instead.

```bash
npm install
```

Note that `fsevents` will not install as it is not compatible with Windows.

## Setting Up the Build Environment

- Open up `ReactNative.sln` in Visual Studio 2015.
- If this is your first time using UWP, you will have to install the SDK. Right click on the solution file in the solution explorer and select the option labeled "Install Missing Components". You may have to shutdown Visual Studio to continue the installation.
- Build the solution once all the packages are installed Build => Rebuild Solution. 

## Running the Playground

In order to run the Playground, you must start the React Native environment by launching the following at the command prompt:
```
node local-cli\cli.js start
```

Then you can Run and Deploy as above when you built the solution.  By default, the Playground will run the UIExplorer App which can be found in `Examples/UIExplorer/UIExplorerApp.windows`.  This allows you to test out each feature of React Native as they are added for UWP.

## Running the Unit Tests

To run the unit tests, do the following:

- Open up the Test Explorer Tests->Windows->Test Explorer, then select Run All. 

## Contributing

If you wish to contribute, we have [help wanted tags](https://github.com/ReactWindows/react-native/issues?q=is%3Aissue+is%3Aopen+label%3A%22help+wanted%22) in our issue tracker which contains items that are open pull requests, first come, first serve.  In order to contribute, you must sign the [Microsoft CLA](https://cla.microsoft.com/) before we can accept your commits.
