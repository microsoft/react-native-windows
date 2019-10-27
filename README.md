<h1 align="center"> React Native for Windows </h1>

<p align="center">
  Build native Windows apps with React.
</p>

<p align="center">
  <a href="https://github.com/microsoft/react-native-windows/blob/master/LICENSE">
    <img src="https://img.shields.io/badge/license-MIT-blue.svg" alt="React Native for Windows is released under the MIT license." />
  </a>
  <a href="https://www.npmjs.org/package/react-native-windows">
    <img src="https://img.shields.io/npm/v/react-native-windows?color=e80441&label=react-native-windows" alt="Current npm package version." />
  </a>
  <a href="https://www.npmjs.org/package/react-native-windows">
    <img src="https://img.shields.io/npm/v/react-native-windows/vnext?color=0052cc&label=react-native-windows%40vnext" alt="vnext npm package version." />
  </a>
  <a href="https://github.com/microsoft/react-native-windows#contributing">
    <img src="https://img.shields.io/badge/PRs-welcome-brightgreen.svg" alt="PRs welcome!" />
  </a>
</p>

![Hero Image with Logo](./.github/hero2.png)

[React Native](http://facebook.github.io/react-native) enables you to build world-class application experiences on native platforms using a consistent developer experience based on JavaScript and [React](https://reactjs.org/). The focus of React Native is on developer efficiency across all the platforms you care about - learn once, write anywhere. Facebook uses React Native in multiple production apps and will continue investing in React Native.

This repository adds support for the [Windows 10 SDK](https://developer.microsoft.com/en-us/windows/downloads), which allows you to build apps for [all devices supported by Windows 10](https://developer.microsoft.com/en-us/windows/get-started-windows-10) including PCs, tablets, 2-in-1s, Xbox, Mixed reality devices etc. 

## Status and roadmap

We are in the process of re-implementing react-native-windows in C++, for better performance, and to better align with the shared C++ react-native core as it evolves. Please see the [Roadmap 2019](blog/Roadmap2019.md) blog for details on this effort and investment roadmap for the upcoming months.

Please see the [July 2019 Updates Blog](blog/July2019Updates.md) for all the latest updates!

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

- You can run React Native Windows UWP apps only on Windows 10 devices
   - React Native Windows -[current](https://github.com/microsoft/react-native-windows/tree/master/current) supports Windows 10 SDK >= [14393](https://developer.microsoft.com/en-us/windows/downloads/sdk-archive) and Windows 8.1
   - React Native Windows -[vnext](https://github.com/microsoft/react-native-windows/tree/master/vnext) currently supports Windows 10 SDK >= [15063](https://developer.microsoft.com/en-us/windows/downloads/sdk-archive). See [Windows 10 Compatability](./win10compat.md) for version support details.
- You can run React Native Windows WPF apps using the [current](https://github.com/microsoft/react-native-windows/tree/master/current) implementation on Windows 7-10 so long as .NET 4.6 is installed on the end user's machine
- Download [Visual Studio 2017 Community or Greater](https://www.visualstudio.com/downloads/). (Visual Studio 2015 support has been deprecated.)
	- You will need to start Visual Studio after it is installed to do some final setup before it can be used to build or run your React Native Windows application

*Note*: Development on React Native Windows itself currently requires Visual Studio 2017 or higher. It is not currently supported with VS Code, but work is ongoing as tracked [here](https://github.com/microsoft/vscode-react-native/issues/1007). 

## Getting Started

Want to develop a React Native Windows app? Head over to our [Getting Started Guide - current](current/docs/GettingStarted.md).

You can also begin to prototype or try out the [vnext](https://github.com/microsoft/react-native-windows/tree/master/vnext) version under development by checking out the [Getting Started Guide - vnext](vnext/docs/GettingStarted.md) 

## Getting Help

- Chat with us on [Reactiflux](https://discord.gg/0ZcbPKXt5bWJVmUY) in #react-native-platforms
- If it turns out that you may have found a bug, please [open an issue](#opening-issues)

## Documentation

[React Native already has great documentation](https://facebook.github.io/react-native/docs/getting-started.html), and we're working to ensure the React Native Windows is part of that documentation story. Check out the [React documentation](https://reactjs.org/docs/getting-started.html) for further details about the React API in general.

For information on parity status with Android and iOS, including details on implemented and missing components and modules, along with related issues for missing features from partial implementations, go [here](current/docs/CoreParityStatus.md) for the current `react-native-windows` implementation and [here](vnext/docs/ParityStatus.md) for `vnext`.

## Extending React Native

- Looking for a component? [JS.coach](https://js.coach/react-native)
- Fellow developers write and publish React Native modules to npm and open source them on GitHub.
- Third party modules may be missing Windows support, so reach out to the project owners directly.
- Making modules helps grow the React Native ecosystem and community. We recommend writing modules for your use cases and sharing them on npm.
- You should almost never need to modify or fork React Native Windows itself (the ReactNative and ReactNative.Shared project/sources) to create custom controls. The extension points should already exist to do just about anything!
- Read the guides on [Native Modules for Windows](current/docs/NativeModulesWindows.md) and [Native UI Components for Windows](current/docs/NativeComponentsWindows.md) if you are interested in extending native functionality. *Note*: Guides for vnext coming soon.

## Opening Issues

If you encounter a bug with the React Native Windows plugin, we would like to hear about it. Search the [existing issues](https://github.com/microsoft/react-native-windows/issues) and try to make sure your problem doesn’t already exist before opening a new issue. It’s helpful if you include the version of Windows, React Native, React Native Windows plugin, and device family (i.e., mobile, desktop, Xbox, etc.) you’re using. Please include a stack trace and reduced repro case when appropriate, too. 

Please use the [appropriate template](https://github.com/microsoft/react-native-windows/issues/new/choose) to file your issues. 

The GitHub issues are intended for bug reports and feature requests. For help and questions with using the React Native Windows plugin please make use of the resources listed in the [Getting Help](#getting-help) section. 

## Contributing

Make sure to install the [system requirements](#system-requirements). If you just want to get started with developing your own app, read [Getting Started with App Development](#getting-started). You only need to interact with `npm` to use for your app development.

For more information about contributing PRs, see :
- [Contribution Guidelines for current react-native-windows](current/docs/CONTRIBUTING.md)
- [Contribution Guidelines for vnext react-native-windows](vnext/docs/CONTRIBUTING.md)

[Good First Task](https://github.com/microsoft/react-native-windows/labels/Good%20First%20Task) and [help wanted](https://github.com/microsoft/react-native-windows/labels/help%20wanted) are great starting points for PRs.

### Examples

- Using the CLI in the [Getting Started](#getting-started) sections will set you up with a sample React-Native-Windows app that you can begin editing.
- If you're looking for sample code, just browse the RNTester folder in the GitHub web UI

## License

The React Native Windows plugin, including modifications to the original Facebook source code, and all newly contributed code is provided under the [MIT License](LICENSE). Portions of the React Native Windows plugin derived from React Native are copyright Facebook.

## Code of Conduct

This project has adopted the [Microsoft Open Source Code of Conduct](https://opensource.microsoft.com/codeofconduct/). For more information see the [Code of Conduct FAQ](https://opensource.microsoft.com/codeofconduct/faq/) or contact [opencode@microsoft.com](mailto:opencode@microsoft.com) with any additional questions or comments.
