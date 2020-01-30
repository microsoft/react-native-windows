# RNPM Plugin for Universal Windows WPF

# THIS IS DEPRECATED, PLEASE USE NODE MODULE: rnpm-plugin-windows.

To bootstrap the creation of WPF projects for React Native, we've published `rnpm-plugin-wpf`. The purpose of this plugin is to provide project init functionality comparable to `react-native init` and `react-native android`, and to allow users to build and run React Windows applications from the command line.

## Project Initialization

To start, make sure you have the react-native CLI installed globally.

```
npm install -g react-native-cli
```

Once the CLI is installed, install rnpm-plugin-wpf and initialize your project. Note: if you have Yarn installed, the react-native-cli will prefer to use that instead of npm, so use yarn to install `rnpm-plugin-wpf`.

```
npm install --save-dev rnpm-plugin-wpf
react-native windows
```

The `windows` command will do the following:
- Install `react-native-windows` from [NPM](https://www.npmjs.com/package/react-native-windows)
- Read the name of your project from `package.json`
- Use [Yeoman](http://yeoman.io/) to generate the Windows project files.

### Usage

```
react-native windows [name] [--namespace <namespace>] [--windowsVersion <version>]
```

The `windows` command takes an optional command line argument.
- `name` - The name of the project, which will be used for both file names and the name of the component that is registered in the React Native [AppRegistry](https://facebook.github.io/react-native/docs/appregistry.html). Default value is the name given in the root-level `package.json`.

The `windows` command accepts two optional flags.
- `--namepace` - The namespace that will be used in the generated native C# code. Default value is `name`.
- `--windowsVersion` - The version of `react-native-windows` that will be used. Default value is matches the major and minor version of `react-native` if installed, otherwise the latest version.

### React Native 0.27.* -> 0.30.*

For versions of react-native prior to 0.31.0, you'll need to use the [rnpm](http://github.com/rnpm/rnpm) global CLI to run the `windows` command. Since the RNPM plugin syntax for the react-native CLI is not backward compatibile with rnpm, you'll need to use an older version of `rnpm-plugin-wpf`:

```
npm install --save-dev rnpm-plugin-wpf@0.1.*
```

## Running React Windows Applications

Once `react-native-windows` is installed in your project, an additional RNPM plugin for running Windows apps is exposed to the `react-native-cli`.  To deploy your app to Desktop, execute:
```
react-native run-windows
```
For more information on the kinds of options and flags available for deploying to devices and emulators, use the `--help` flag to get the command usage information.
