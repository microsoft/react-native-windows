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
  <a href="https://github.com/microsoft/react-native-windows#contributing">
    <img src="https://img.shields.io/badge/PRs-welcome-brightgreen.svg" alt="PRs welcome!" />
  </a>
</p>

![Hero Image with Logo](https://raw.githubusercontent.com/microsoft/react-native-windows/master/.github/hero2.png)

> See the official [React Native website](https://reactnative.dev/) for an introduction to React Native.

[React Native](https://reactnative.dev) is a framework developed by Facebook that enables you to build world-class application experiences on native platforms using a consistent developer experience based on JavaScript and [React](https://reactjs.org/). The focus of React Native is on developer efficiency across all the platforms you care about - learn once, write anywhere.

This repository adds support for the [Windows 10 SDK](https://developer.microsoft.com/en-us/windows/downloads), which allows you to build apps for [all devices supported by Windows 10](https://developer.microsoft.com/en-us/windows/get-started-windows-10) including PCs, tablets, 2-in-1s, Xbox, Mixed reality devices etc.

## Status and roadmap
[Check out our blog](https://microsoft.github.io/react-native-windows/blog/) If you'd like to stay up to date on the status of React Native for Windows and check out current and past roadmaps,  We will post all new releases, updates and general news about the project there.

## System Requirements

- You can run React Native Windows UWP apps only on Windows 10 devices
   - React Native Windows currently supports Windows 10 SDK >= [15063](https://developer.microsoft.com/en-us/windows/downloads/sdk-archive). See [Windows 10 Compatibility](https://microsoft.github.io/react-native-windows/docs/win10-compat) for version support details.
- Download [Visual Studio 2017 Community or Greater](https://www.visualstudio.com/downloads/). (Visual Studio 2015 support has been deprecated.)
	- You will need to start Visual Studio after it is installed to do some final setup before it can be used to build or run your React Native Windows application

> Development on React Native Windows itself currently requires Visual Studio 2017 or higher. It is not currently supported with VS Code, but work is ongoing as tracked [here](https://github.com/microsoft/vscode-react-native/issues/1007).

## Documentation
[React Native already has great documentation](https://reactnative.dev/docs/getting-started.html) and we're working to ensure the React Native Windows is part of that documentation story.

[React Native for Windows](https://microsoft.github.io/react-native-windows/) has it's own separate documentation site where Windows and Mac specific information, like API docs and blog updates live.

## Get Started
See the [Getting Started Guide](https://microsoft.github.io/react-native-windows/docs/getting-started) on our React Native for Windows website.

## Need Help?

Come join us and chat on the [Reactiflux](https://discord.gg/0ZcbPKXt5bWJVmUY) Discord server under in **#react-native**

If it turns out that you may have found a bug,  we would like to hear about it!

### Logging Issues
Search the [existing issues](https://github.com/microsoft/react-native-windows/issues) and try to make sure your problem doesn’t already exist before opening a new issue. If your issue doesn't exist yet, try to make sure you provide as much information as possible to us so we can help you sooner. It’s helpful if you include information like:

- The version of Windows, React Native, React Native Windows plugin, and device family (i.e., mobile, desktop, Xbox, etc.) where you ran into the issue.
- A stack trace and reduced repro case when possible.
- Ensure the [appropriate template](https://github.com/microsoft/react-native-windows/issues/new?labels=vnext&template=vnext.md) is used when filing your issue(s).

The GitHub issues are intended for bug reports and feature requests. For help and questions with using the React Native Windows plugin please make use of the resources listed our main README's the [Getting Help](https://github.com/microsoft/react-native-windows#getting-help) section.

## Contributing
See [Contributing guidelines](./docs/contributing.md) for how to setup your fork of the repo and start a PR to contribute to React Native for Windows.

## License

The React Native Windows plugin, including modifications to the original Facebook source code, and all newly contributed code is provided under the [MIT License](LICENSE). Portions of the React Native Windows plugin derived from React Native are copyright Facebook.

## Code of Conduct

This project has adopted the [Microsoft Open Source Code of Conduct](https://opensource.microsoft.com/codeofconduct/). For more information see the [Code of Conduct FAQ](https://opensource.microsoft.com/codeofconduct/faq/) or contact [opencode@microsoft.com](mailto:opencode@microsoft.com) with any additional questions or comments.
