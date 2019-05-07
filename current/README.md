# ReactNative for Windows (current)
![Hero Image with Logo](../.github/hero2.png)

See the official [React Native website](https://facebook.github.io/react-native/) for an introduction to React Native. See [main landing page](https://github.com/Microsoft/react-native-windows) for more details on overall direction of `react-native-windows`. 

This `current` subdirectory holds support for an existing React Native for Windows implementation built in C#. See [vnext](https://github.com/Microsoft/react-native-windows/tree/master/vnext) for details on the `vnext` rewrite of React Native for Windows built in C++.

## Status and roadmap

We anticipate that there will be increased activity in the overall repository related to the `vnext` refactoring effort, but that overall effort allocated to the existing architecture will be minimized. See [Opening Issues](#opening-issues) for details on how to open issues if you encounter bugs with the existing architecture.

We are working on releasing a `v0.58` and a subsequent `v0.59` for the existing architecture in order to ensure existing apps can continue to upgrade to match `react-native` versions while the `vnext` implementation is in progress. See [releases](docs/Releases.md) for more information on `react-native-windows` release strategy.

## System Requirements

- You can run React Native for Windows UWP apps only on Windows 10 devices. This `current` implementation supports Windows 10 SDK > [14393](https://developer.microsoft.com/en-us/windows/downloads/sdk-archive) and Windows 8.1.
- You can run React Native for Windows WPF applications using this `current` implementation on Windows 7-10 so long as .NET 4.6 is installed on the end user's machine
- Download [Visual Studio 2017 Community or Greater](https://www.visualstudio.com/downloads/). (Visual Studio 2015 support has been deprecated.)
	- You will need to start Visual Studio after it is installed to do some final setup before it can be used to build or run your React Native Windows application

*Note*: Development on React Native Windows itself currently requires Visual Studio 2017. It is not supported with VS Code, but we will gladly accept pull requests to enable a great developer experience in those environments.

## Getting Started
See Getting Started Guide for [React Native for Windows - current](docs/GettingStarted.md) to begin developing apps using `react-native-windows`. 

## Documentation

[React Native already has great documentation](https://facebook.github.io/react-native/docs/getting-started.html), and we're working to ensure the React Native Windows is part of that documentation story. Check out the [React documentation](http://facebook.github.io/react/) for further details about the React API in general.

For information on parity status with Android and iOS, including details on implemented and missing components and modules, along with related issues for missing features from partial implementations, go [here](docs/CoreParityStatus.md)

### Showing the Debug Menu

- On UWP, press Shift+F10 to open the debug menu.
- On WPF, press CTRL+D or CTRL+M to open the debug menu.

### Extending React Native

- See [Extending React Native](https://github.com/microsoft/react-native-windows#extending-react-native) for more general information on community modules and other extension capabilities. 
- Read the guides on [Native Modules for Windows](docs/NativeModulesWindows.md) and [Native UI Components for Windows](docs/NativeComponentsWindows.md) if you are interested in extending native functionality.

## Opening issues
If you encounter a bug with the `current` architecture, we would like to hear about it. Search the [existing issues](https://github.com/Microsoft/react-native-windows/issues?label%3A.NET) and try to make sure your problem doesn’t already exist before opening a new issue. It’s helpful if you include the version of Windows, React Native, React Native Windows plugin, and device family (i.e., desktop, Xbox, etc.) you’re using. Please include a stack trace and reduced repro case when appropriate, too.

Please continue to report issues as you encounter them, but be sure to use the [correct template](https://github.com/Microsoft/react-native-windows/issues/new?assignees=rozele&labels=.NET&template=DOTNET.md) for issues related to the existing `react-native-windows` implementation.

## Contributing
See [Contributing guidelines](docs/CONTRIBUTING.md) for how to setup your fork of the repo and start a PR to contribute to React Native Windows - current. 