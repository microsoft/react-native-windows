# ReactNative for Windows 10 (C++)
See the official [React Native website](https://facebook.github.io/react-native/) for an introduction to React Native. The [current folder](https://github.com/Microsoft/react-native-windows/tree/master/current) adds support for React Native for Windows10 implemented from scratch in C#. It reimplemented the JS<->Native bridge in C#, and shared the JS with Facebook's implementation of React Native. The ongoing direction of React Native involves a closer interaction between C++ and JS which is hard to achieve with a separate C# implementation.

In this sub-folder, we are working on a vnext rewrite of React Native for Windows10 built in C++ that reuses the C++ core bridge implementation from Facebook’s React Native.  This will allow React Native for Windows10 to innovate and provide features by sharing the same core as Facebook’s React Native. See [Status](https://github.com/Microsoft/react-native-windows#status) for more details.

## Status and roadmap
The development of the React Native Windows C++ implementation is ongoing. You can take a look at [Milestones](https://github.com/Microsoft/react-native-windows/milestones) and [Projects](https://github.com/Microsoft/react-native-windows/projects) for a view on the work streams and tasks. 

# Getting Started
See [Getting Started Guide for React Native Windows C++](docs/GettingStarted.md).

# Opening issues
If you encounter a bug with the React Native Windows C++ implementation or have a feature request, we would like to hear about it. Search the [existing issues](https://github.com/Microsoft/react-native-windows/issues?label%3Avnext) and try to make sure your problem doesn’t already exist before opening a new issue. It’s helpful if you include the version of Windows, React Native, React Native Windows plugin, and device family (i.e., desktop, Xbox, etc.) you’re using. Please include a stack trace and reduced repro case when appropriate, too.

# Contributing
See [Contributing guidelines](docs/CONTRIBUTING.md) for how to setup your fork of the repo and start a PR to contribute to React Native Windows C++. 
