# RNPM Plugin for Universal Windows

To bootstrap the creation of Windows projects for React Native, we've published `rnpm-plugin-windows`. The purpose of this plugin is to provide project init functionality comparable to `react-native init` and `react-native android`, and to allow users to build and run React Windows applications from the command line.

## Project Initialization

To start, make sure you have RNPM installed globally.

```
npm install -g rnpm
```

Once RNPM is installed, install the Windows plugin for RNPM and initialize your project.

```
npm install --save-dev rnpm-plugin-windows
rnpm windows
```

The `windows` command will do the following:
- Install `react-native-windows` from [NPM](https://www.npmjs.com/package/react-native-windows)
- Read the name of your project from `package.json`  
- Use [Yeoman](http://yeoman.io/) to generate the Windows project files.

### Usage

```
rnpm windows [name] [--namespace <namespace>] [--windowsVersion <version>]
```

The RNPM `windows` command takes an optional command line argument.
- `name` - The name of the project, which will be used for both file names and the name of the component that is registered in the React Native [AppRegistry](https://facebook.github.io/react-native/docs/appregistry.html). Default value is the name given in the root-level `package.json`.

The RNPM `windows` command accepts two optional flags.
- `--namepace` - The namespace that will be used in the generated native C# code. Default value is `name`.
- `--windowsVersion` - The version of `react-native-windows` that will be used. Default value is matches the major and minor version of `react-native` if installed, otherwise the latest version.

## Running React Windows Applications

Coming soon.
