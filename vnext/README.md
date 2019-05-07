# ReactNative for Windows (vnext)
![Hero Image with Logo](../.github/hero2.png)

See the official [React Native website](https://facebook.github.io/react-native/) for an introduction to React Native. See [main landing page](https://github.com/Microsoft/react-native-windows) for more details on overall direction of `react-native-windows`. 

The [`current`](https://github.com/Microsoft/react-native-windows/tree/master/current) subdirectory adds support for an earlier `react-native-windows` implementation built in C#. It reimplemented the JS<->Native bridge in C#, and shared the JS with Facebook's implementation of `react-native`. The ongoing direction of `react-native` involves a closer interaction between C++ and JS which is hard to achieve with a separate C# implementation.

In this `vnext` sub-folder, we are working on a rewrite of `react-native-windows` built in C++ that reuses the C++ core bridge implementation from Facebook’s React Native. This will allow React Native for Windows to innovate and provide features by sharing the same core as Facebook’s React Native. See [Status](https://github.com/Microsoft/react-native-windows#status) for more details.

## Status and roadmap
The development of the React Native Windows `vnext` implementation is ongoing. You can take a look at [Milestones](https://github.com/Microsoft/react-native-windows/milestones) and [Projects](https://github.com/Microsoft/react-native-windows/projects) for a view on the work streams and tasks. 

The `vnext` package currently supports `v0.58` of corresponding `react-native` major version. See [releases](docs/Releases.md) for more information on `vnext` release strategy.

## Documentation
[React Native already has great documentation](https://facebook.github.io/react-native/docs/getting-started.html), and we're working to ensure the React Native Windows is part of that documentation story. Check out the [React documentation](http://facebook.github.io/react/) for further details about the React API in general.

Coming soon - we will be publishing more documentation including some of the following shortly:
- A live document that captures the ongoing parity status of this `vnext` implementation with Android and iOS.
- Guides on how to extend native Windows capabilties in C# and C++ using `vnext` 

## Getting Started
See [Getting Started Guide for React Native Windows C++](docs/GettingStarted.md).

## Opening issues
If you encounter a bug with the React Native Windows C++ implementation or have a feature request, we would like to hear about it. Search the [existing issues](https://github.com/Microsoft/react-native-windows/issues?label%3Avnext) and try to make sure your problem doesn’t already exist before opening a new issue. It’s helpful if you include the version of Windows, React Native, React Native Windows plugin, and device family (i.e., desktop, Xbox, etc.) you’re using. Please include a stack trace and reduced repro case when appropriate, too.

Please make sure to use the [correct template](https://github.com/microsoft/react-native-windows/issues/new?labels=vnext&template=vnext.md) for issues related to the vnext `react-native-windows` implementation.

## Contributing
See [Contributing guidelines](docs/CONTRIBUTING.md) for how to setup your fork of the repo and start a PR to contribute to React Native Windows C++. 
