<h1 align="center"> React Native for Windows </h1>

<p align="center">
  Build native Windows apps with React.
</p>

![Hero Image with Logo](./.github/hero2.png)

[React Native](http://facebook.github.io/react-native) enables you to build world-class application experiences on native platforms using a consistent developer experience based on JavaScript and [React](http://facebook.github.io/react). The focus of React Native is on developer efficiency across all the platforms you care about - learn once, write anywhere. Facebook uses React Native in multiple production apps and will continue investing in React Native.

This repository adds support for the [Windows 10 SDK](https://developer.microsoft.com/en-us/windows/downloads), which allows you to build apps for [all devices supported by Windows 10](https://developer.microsoft.com/en-us/windows/get-started-windows-10) including PCs, tablets, 2-in-1s, Xbox, Mixed reality devices etc. 

## Status

We are in the process of re-implementing react-native-windows in C++, for better performance, and to better align with the shared C++ react-native core as it evolves. This effort is currently taking place in the [vnext](https://github.com/Microsoft/react-native-windows/tree/master/vnext) subdirectory within this GitHub repo. You can read more about this effort [here](vnext/README.md). We are tracking progress on this work through [Projects](https://github.com/Microsoft/react-native-windows/projects) and [Issues](https://github.com/Microsoft/react-native-windows/issues?q=is%3Aopen+is%3Aissue+label%3Arnwcpp) on this GitHub repo.

The ["current"](https://github.com/Microsoft/react-native-windows/tree/master/current) subdirectory holds the earlier C# implementation for `react-native-windows`. 

Our intent is to provide a compatibility layer that will support existing apps, view managers, and native modules written in C# with minimal breaking changes.

We anticipate that there will be increased activity on the repository related to this refactoring effort, but that overall effort allocated to the existing architecture will be minimized. Please continue to report issues as you encounter them, but be sure to use the [correct template](https://github.com/Microsoft/react-native-windows/issues/new?assignees=rozele&labels=.NET&template=DOTNET.md) for issues related to the existing `react-native-windows` package (version 0.57 and earlier).

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

- You can build and deploy React Native Windows apps on the Pro or Enterprise versions of Windows
	- You cannot run the emulators and some other developer tooling on the "Starter" or "Home" versions of these operating systems
	- You can run React Native Windows UWP apps only on Windows 10 devices
       - React Native Windows -[current](https://github.com/Microsoft/react-native-windows/tree/master/current) supports Windows 10 SDK > [14393](https://developer.microsoft.com/en-us/windows/downloads/sdk-archive) and Windows 8.1
       - React Native Windows -[vnext](https://github.com/Microsoft/react-native-windows/tree/master/vnext) currently supports Windows 10 SDK > [14393](https://developer.microsoft.com/en-us/windows/downloads/sdk-archive)
    - You can run React Native Windows WPF apps using the [current](https://github.com/Microsoft/react-native-windows/tree/master/current) implementation on Windows 7-10 so long as .NET 4.6 is installed on the end user's machine
- Download [Visual Studio 2017 Community or Greater](https://www.visualstudio.com/downloads/). (Visual Studio 2015 support has been deprecated.)
	- You will need to start Visual Studio after it is installed to do some final setup before it can be used to build or run your React Native Windows application

*Note*: Development on React Native Windows itself currently requires Visual Studio 2017. It is not supported with VS Code, but we will gladly accept pull requests to enable a great developer experience in those environments.

## Getting Started

Want to develop a React Native Windows app? Head over to our [Getting Started Guide - current](current/docs/GettingStarted.md).

You can also begin to prototype or try out the [vnext](https://github.com/Microsoft/react-native-windows/tree/master/vnext) version under development by checking out the [Getting Started Guide - vnext](vnext/docs/GettingStarted.md) 

## Getting Help

- Chat with us on [Reactiflux](https://discord.gg/0ZcbPKXt5bWJVmUY) in #react-native-platforms
- If it turns out that you may have found a bug, please [open an issue](#opening-issues)

## Documentation

[React Native already has great documentation](https://facebook.github.io/react-native/docs/getting-started.html), and we're working to ensure the React Native Windows is part of that documentation story. Check out the [React documentation](http://facebook.github.io/react/) for further details about the React API in general.

For information on parity status with Android and iOS, including details on implemented and missing components and modules, along with related issues for missing features from partial implementations, go [here](current/docs/CoreParityStatus.md) for the current react-native-windows implementation. We are working on publishing a similar parity status for vnext shortly.

### Showing the Debug Menu

- On UWP, press Shift+F10 to open the debug menu.
- On WPF, press CTRL+D or CTRL+M to open the debug menu.

*Note*: Support for Debug menu in vnext is tracked by [#2276](https://github.com/Microsoft/react-native-windows/issues/2276)

## Extending React Native

- Looking for a component? [JS.coach](https://js.coach/react-native)
- Fellow developers write and publish React Native modules to npm and open source them on GitHub.
- Third party modules may be missing Windows support, so reach out to the project owners directly.
- Making modules helps grow the React Native ecosystem and community. We recommend writing modules for your use cases and sharing them on npm.
- You should almost never need to modify or fork React Native Windows itself (the ReactNative and ReactNative.Shared project/sources) to create custom controls. The extension points should already exist to do just about anything!
- Read the guides on [Native Modules for Windows](current/docs/NativeModulesWindows.md) and [Native UI Components for Windows](current/docs/NativeComponentsWindows.md) if you are interested in extending native functionality. *Note*: Guides for vnext coming soon.

## Opening Issues

If you encounter a bug with the React Native Windows plugin, we would like to hear about it. Search the [existing issues](https://github.com/Microsoft/react-native-windows/issues) and try to make sure your problem doesn’t already exist before opening a new issue. It’s helpful if you include the version of Windows, React Native, React Native Windows plugin, and device family (i.e., mobile, desktop, Xbox, etc.) you’re using. Please include a stack trace and reduced repro case when appropriate, too. 

Please use either the [current](https://github.com/Microsoft/react-native-windows/issues/new?assignees=rozele&labels=.NET&template=DOTNET.md) vs. [vnext](https://github.com/Microsoft/react-native-windows/issues/new?labels=vnext&template=vnext.md) issue template as appropriate. 

The GitHub issues are intended for bug reports and feature requests. For help and questions with using the React Native Windows plugin please make use of the resources listed in the [Getting Help](#getting-help) section. 

## Contributing

Make sure to install the [system requirements](#system-requirements). If you just want to get started with developing your own app, read [Getting Started with App Development](#getting-started). You only need to interact with `npm` to use for your app development.

For more information about contributing PRs, see :
- [Contribution Guidelines for current react-native-windows](current/docs/CONTRIBUTING.md)
- [Contribution Guidelines for vnext react-native-windows](vnext/docs/CONTRIBUTING.md)

[Good First Task](https://github.com/Microsoft/react-native-windows/labels/Good%20First%20Task) and [help wanted](https://github.com/Microsoft/react-native-windows/labels/help%20wanted) are great starting points for PRs.

### Examples

- Using the CLI in the [Getting Started](#getting-started) sections will set you up with a sample React-Native-Windows app that you can begin editing.
- If you're looking for sample code, just browse the RNTester folder in the GitHub web UI

## License

The React Native Windows plugin, including modifications to the original Facebook source code, and all newly contributed code is provided under the [MIT License](LICENSE). Portions of the React Native Windows plugin derived from React Native are copyright Facebook.

## Code of Conduct

This project has adopted the [Microsoft Open Source Code of Conduct](https://opensource.microsoft.com/codeofconduct/). For more information see the [Code of Conduct FAQ](https://opensource.microsoft.com/codeofconduct/faq/) or contact [opencode@microsoft.com](mailto:opencode@microsoft.com) with any additional questions or comments.
