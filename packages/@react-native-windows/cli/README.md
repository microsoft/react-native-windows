# @react-native-windows/cli

CLI to build and run React Native for Windows apps.

## `autolink-windows`

The `autolink-windows` CLI command is used to link the native code and build systems for a React Native for Windows app with any native community modules it uses.

**Note:** Autolinking runs automatically as part of running the `run-windows` CLI command, unless the `--no-autolink` argument is used.

### Usage
Runs Windows-specific autolinking for your RNW project.
  
```bat
npx @react-native-community/cli autolink-windows
```
### Options

Here are the options that `npx @react-native-community/cli autolink-windows` takes:
| Option                | Input Type | Description                                      |
|-----------------------|------------|--------------------------------------------------|
| `--logging`           | boolean    | Verbose output logging                           |
| `--check`             | boolean    | Only check whether any autolinked files need to change |
| `--sln`               | string     | Override the app solution file determined by 'npx @react-native-community/cli config', i.e. `windows\myApp.sln` |
| `--proj`              | string     | Override the app project file determined by 'npx @react-native-community/cli config', i.e. `windows\myApp\myApp.vcxproj` |
| `--no-telemetry`      | boolean    | Disables sending telemetry that allows analysis of usage and failures of the react-native-windows CLI |
| `-h`, `--help`        | boolean    | Display help for command                         |

## `codegen-windows`

The `codegen-windows` CLI command is used to generate some necessary Windows-specific native code for native modules.

### Usage
Runs Windows-specific codegen for native modules.
  
```bat
npx @react-native-community/cli codegen-windows
```
### Options

Here are the options that `npx @react-native-community/cli codegen-windows` takes:
| Option                | Input Type | Description                                      |
|-----------------------|------------|--------------------------------------------------|
| `--logging`           | boolean    | Verbose output logging                           |
| `--check`             | boolean    | Only check whether any codegen files need to change |
| `--no-telemetry`      | boolean    | Disables sending telemetry that allows analysis of usage and failures of the react-native-windows CLI |
| `-h`, `--help`        | boolean    | Display help for command                         |

## `init-windows`

The `init-windows` CLI command is used to initialize a new React Native for Windows project inside an existing React Native project. 

### Usage
Initializes a new RNW project from a given template.
  
```bat
npx @react-native-community/cli init-windows
```
### Options

Here are the options that `npx @react-native-community/cli init-windows` takes:
| Option                | Input Type | Description                                      |
|-----------------------|------------|--------------------------------------------------|
| `--logging`           | boolean    | Verbose output logging                           |
| `--template`          | string     | Specify the template to use                      |
| `--name`              | string     | The native project name. Defaults to the name property in `app.json` or `package.json` |
| `--namespace`         | string     | The native project namespace, expressed using dots as separators, i.e. `Level1.Level2.Level3`. Defaults to the same as name |
| `--overwrite`         | boolean    | Overwrite any existing files without prompting  |
| `--no-telemetry`      | boolean    | Disables sending telemetry that allows analysis of usage and failures of the react-native-windows CLI |
| `-h`, `--help`        | boolean    | Display help for command                         |

## `run-windows`

The `run-windows` CLI command is used to build and run React Native for Windows apps. 

### Usage
Builds your RNW app and starts it on a connected Windows desktop, emulator or device.
  
```bat
npx @react-native-community/cli run-windows
```
### Options

> **Note:** Remote Debugging was officially marked as [deprecated](https://github.com/react-native-community/discussions-and-proposals/discussions/734) in RN 0.73 and will be removed in a later release.

Here are the options that `npx @react-native-community/cli run-windows` takes:
| Option                | Input Type | Description                                      |
|-----------------------|------------|--------------------------------------------------|
| `--release`           | boolean    | Specifies a Release build                        |
| `--root`              | string     | Override the root directory for the project which contains the `windows` folder |
| `--arch`              | string     | The build architecture, i.e. `ARM64`, `x86`, `x64`. Defaults to system architecture |
| `--singleproc`        | boolean    | Disables multi-proc build                        |
| `--emulator`          | boolean    | Deploys the app to an emulator                   |
| `--device`            | boolean    | Deploys the app to a connected device            |
| `--target`            | string     | Deploys the app to the specified `GUID` for a device |
| `--remote-debugging`  | boolean    | **(Deprecated)** Deploys the app in remote debugging mode |
| `--logging`           | boolean    | Verbose output logging                           |
| `--no-packager`       | boolean    | Do not launch the packager while building        |
| `--bundle`            | boolean    | Enable Bundle configuration, i.e. `ReleaseBundle`/`DebugBundle` rather than `Release`/`Debug` |
| `--no-launch`         | boolean    | Do not launch the app after deployment           |
| `--no-autolink`       | boolean    | Do not run autolinking                           |
| `--no-build`          | boolean    | Do not build the solution                        |
| `--no-deploy`         | boolean    | Do not deploy the app                            |
| `--deploy-from-layout`| boolean    | Force deploy from layout even in Release builds |
| `--sln`               | string     | Override the app solution file determined by 'npx @react-native-community/cli config', i.e. `windows\myApp.sln` |
| `--proj`              | string     | Override the app project file determined by 'npx @react-native-community/cli config', i.e. `windows\myApp\myApp.vcxproj` |
| `--msbuildprops`      | string     | Comma separated props to pass to MSBuild, i.e. `prop1=value1,prop2=value2` |
| `--buildLogDirectory` | string     | Optional directory where MSBuild log files should be stored |
| `--info`              | boolean    | Dump environment information                     |
| `--direct-debugging`  | number     | Enable direct debugging on specified port        |
| `--no-telemetry`      | boolean    | Disables sending telemetry that allows analysis of usage and failures of the react-native-windows CLI |
| `-h`, `--help`        | boolean    | Display help for command                         |
| `--client-logs`       | boolean    | Enable plain text JavaScript log streaming for all connected apps |

## Data Collection
The software may collect information about you and your use of the software and send it to Microsoft. Microsoft may use this information to provide services and improve our products and services. You may turn off the telemetry as described in the repository. There are also some features in the software that may enable you and Microsoft to collect data from users of your applications. If you use these features, you must comply with applicable law, including providing appropriate notices to users of your applications together with a copy of Microsoft's privacy statement. Our privacy statement is located at https://go.microsoft.com/fwlink/?LinkID=824704. You can learn more about data collection and use in the help documentation and our privacy statement. Your use of the software operates as your consent to these practices.

This data collection notice only applies to the process of running the react-native-windows CLI (i.e. the commands above).
