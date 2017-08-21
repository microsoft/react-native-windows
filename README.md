# React Native plugin for Universal Windows Platform (UWP)

![Hero Image with Logo](./.github/hero.png)

[React Native](http://facebook.github.io/react-native) enables you to build world-class application experiences on native platforms using a consistent developer experience based on JavaScript and [React](http://facebook.github.io/react). The focus of React Native is on developer efficiency across all the platforms you care about - learn once, write anywhere. Facebook uses React Native in multiple production apps and will continue investing in React Native.

This repository adds support for the [Windows 10 SDK](https://developer.microsoft.com/en-us/windows/downloads), which allows you to build apps for:
* Windows 10
* Windows 10 Mobile
* [Xbox One (UWP)](https://msdn.microsoft.com/en-us/windows/uwp/xbox-apps/index)

## Quick Links

- [Getting Started](#getting-started)
- [Getting Help](#getting-help)
- [Documentation](#documentation)
- [Examples](#examples)
- [Extending React Native](#extending-react-native)
- [Opening Issues](#opening-issues)
- [Contributing](#contributing)
- [License](#license)
- [Code of Conduct](#code-of-conduct)

## Introduction

See the official [React Native website](https://facebook.github.io/react-native/) for an introduction to React Native.

## System Requirements

- You can build and deploy React Native Windows apps on the Pro or Enterprise versions of Windows 7 SP1, Windows 8.1, or Windows 10
- *Note:* You cannot run the emulators and some other developer tooling on the "Starter" or "Home" versions of these operating systems
- You can run React Native Windows UWP apps only on Windows 10 devices, but React Native Windows WPF apps will run on Windows 7-10 so long as .NET 4.6 is installed on the end user's machine
- Download [Visual Studio 2017 Community or Greater](https://www.visualstudio.com/downloads/). (Visual Studio 2015 support is being deprecated.)
- - You will need to start Visual Studio after it is installed to do some final setup before it can be used to build or run your React Native Windows application
- [Windows 10 SDK Build 14393](https://developer.microsoft.com/en-us/windows/downloads/sdk-archive)

*Note*: Development on React Native Windows itself currently requires Visual Studio 2017. It is not supported with VS Code, but we will gladly accept pull requests to enable a great developer experience in those environments.

## Getting Started

- Follow the [Getting Started guide](http://facebook.github.io/react-native/docs/getting-started.html) to install React Native and its dependencies.
- Change your working directory to the project directory you specified in the `react-native init` command from the [Getting Started guide](http://facebook.github.io/react-native/docs/getting-started.html) (e.g., `cd AwesomeProject`).
- Install [rnpm-plugin-windows](https://www.npmjs.com/package/rnpm-plugin-windows) from NPM (i.e., `npm install --save-dev rnpm-plugin-windows` or `yarn add rnpm-plugin-windows --dev` if you have Yarn installed).
- Initialize Windows project with `react-native windows` command in the project directory.
- *Note*: You will need to install the [requirements](#system-requirements) before you can build and run your application.
- Open the solution file in the application folder in Visual Studio (e.g., `AwesomeProject/windows/AwesomeProject.sln`)
- *Note*: If this is your first time doing UWP development on the computer you're using, you may be asked to install additional UWP tooling. After opening the solution, right click the Solution in the Solution Explorer and select the option labeled "Install Missing Components". You may have to shutdown Visual Studio to continue the installation.
- *Note*: If you are using Visual Studio 2017, you will need to retarget the ChakraBridge project by right clicking on the project in the Solution Explorer and selecting "Retarget Projects" and pressing okay on the popup dialog (Platform Toolset should say "Upgrade to v141" in the dialog).
- Select the "Debug" configuration and the "x64" platform from the combo box controls to the left of the Run button and underneath the Team and Tools menu item.
- *Note:* If you are running on, or targeting, an x86 platform select "x86" instead. If you are deploying to Windows 10 Phone, select ARM.
- Click the Run button to the right of the platform combo box control, or select the "Debug"->"Start without Debugging" menu item.
- You should now see a typical React Native app running on Windows that is showing an error saying it needs to contact the dev server. Almost there!
- Run `react-native start` from your project directory, and wait for the React Native packager to report success. Then, press control+R (or click Reload button) in your running app. You now see your new app! :tada:
- *Note:* You should **only** modify the project and source files for your app (e.g. `AwesomeProject`). The files for the "ReactNative" and other projects shown in the Visual Studio solution are in the `node_modules` directory (which will not be committed to your source repository since it is ignored in `.gitignore`). Any changes to files in `node_modules` will be overwritten when doing an `npm install` or `npm update`. If you need to add a new native module or override some React Native behavior, see [Extending React Native](#extending-react-native)
- For more information about how to deploy to Windows simulators/devices, and how to create a bundle for Release, view [RunningOnDeviceWindows](docs/RunningOnDeviceWindows.md)
- Learn more about various [tools for debugging React Native](http://facebook.github.io/react-native/docs/debugging.html).

### Setup Troubleshooting

If you happen to run into issues while setting up React Native Windows on your machine, check out the [Setup Troubleshooting](docs/SetupTroubleshooting.md) documentation.

## Getting Help

Please use these community resources for getting help. We use the GitHub issues for tracking bugs and feature requests and have limited bandwidth to address them.

- Chat with us on [Reactiflux](https://discord.gg/0ZcbPKXt5bWJVmUY) in #react-native-platforms
- If it turns out that you may have found a bug, please [open an issue](#opening-issues)

## Documentation

[React Native already has great documentation](http://facebook.github.io/react-native/docs), and we're working to ensure the React Native Windows plugin is part of that documentation story. Check out the [React documentation](http://facebook.github.io/react/) for further details about the React API in general.

For information on parity status with Android and iOS, including details on implemented and missing components and modules, along with related issues for missing features from partial implementations, go [here](docs/CoreParityStatus.md).

### Showing the Debug Menu

- On UWP, press Shift+F10 to open the debug menu.
- On WPF, press CTRL+D or CTRL+M to open the debug menu.

## Extending React Native

- Looking for a component? [JS.coach](https://js.coach/react-native)
- Fellow developers write and publish React Native modules to npm and open source them on GitHub.
- Third party modules may be missing Windows support, so reach out to the project owners directly.
- Making modules helps grow the React Native ecosystem and community. We recommend writing modules for your use cases and sharing them on npm.
- You should almost never need to modify or fork React Native Windows itself (the ReactNative and ReactNative.Shared project/sources) to create custom controls. The extension points should already exist to do just about anything!
- Read the guides on [Native Modules for Windows](docs/NativeModulesWindows.md) and [Native UI Components for Windows](docs/NativeComponentsWindows.md) if you are interested in extending native functionality.

## Opening Issues

If you encounter a bug with the React Native Windows plugin, we would like to hear about it. Search the [existing issues](https://github.com/ReactWindows/react-native-windows/issues) and try to make sure your problem doesn’t already exist before opening a new issue. It’s helpful if you include the version of Windows, React Native, React Native Windows plugin, and device family (i.e., mobile, desktop, Xbox, etc.) you’re using. Please include a stack trace and reduced repro case when appropriate, too.

The GitHub issues are intended for bug reports and feature requests. For help and questions with using the React Native Windows plugin please make use of the resources listed in the [Getting Help](#getting-help) section. There are limited resources available for handling issues, and by keeping the list of open issues lean we can respond in a timely manner.

## Contributing

- Install the [requirements](#requirements)
- Check out the React Native Windows code itself and install npm dependencies
```
git clone --recursive https://github.com/ReactWindows/react-native-windows.git
cd react-native-windows
npm install
```
- - *Note:* If you just want to get started with developing your own app, read [Getting Started with App Development](#GettingStarted). You only need to interact with `npm` to use for your app development.

For more information about contributing PRs and issues, see our [Contribution Guidelines](https://github.com/ReactWindows/react-native-windows/blob/master/CONTRIBUTING.md) **(Coming Soon)**.

[Good First Task](https://github.com/ReactWindows/react-native-windows/labels/Good%20First%20Task) and [help wanted](https://github.com/ReactWindows/react-native-windows/labels/help%20wanted) are great starting points for PRs.

Each pull request has the unit tests, code analysis, and a [Winium](https://github.com/2gis/Winium) integration test run in the AppVeyor CI service. To shorten the feedback cycle, please be sure to run the unit tests in Visual Studio and verify they are passing before submitting pull requests. For extra credit, verify the examples in RNTester continue to work properly.

### Examples

- If you're looking for sample code, just browse the RNTester folder in the GitHub web UI
- From your recursive clone of the react-native-windows repo, run ``git pull --recurse-submodules`` to make sure you're up to date with latest code
- Start the React Native packager by running `react-native start`
- Open RNTester.sln from the RNTester submodule folder in Visual Studio.
- Set the StartUp project to the example you want to run, press F5.

## License

The React Native Windows plugin is provided under the [MIT License](LICENSE).

## Code of Conduct

This project has adopted the [Microsoft Open Source Code of Conduct](https://opensource.microsoft.com/codeofconduct/). For more information see the [Code of Conduct FAQ](https://opensource.microsoft.com/codeofconduct/faq/) or contact [opencode@microsoft.com](mailto:opencode@microsoft.com) with any additional questions or comments.
