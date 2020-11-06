# react-native-windows-init

CLI to bootstrap the addition of the Windows platform to `react-native` projects.

## Usage

Run this from an existing `react-native` project, to install `react-native-windows` and generate initial project files for windows.


Example usage
```
npx react-native-windows-init --language cpp
```

Options:
| option      | description                                    | type                                             |
|-------------|------------------------------------------------|--------------------------------------------------|
| `--help`      | Show help                                      | [boolean]                                        |
| `--version`   | The version of react-native-windows to use.    | [string]                                         |
| `--namespace` | The native project namespace.                  | [string]                                         |
| `--verbose`   | Enables logging.                               | [boolean]                                        |
| `--language`  | Which language the app is written in.          | [string] [choices: "cs", "cpp"] [default: "cpp"] |
| `--overwrite` | Overwrite any existing files without prompting | [boolean]                                        |
| `--no-telemetry` | Disables sending telemetry that allows analysis of failures of the react-native-windows CLI | [boolean] |


## Data Collection
The software may collect information about you and your use of the software and send it to Microsoft. Microsoft may use this information to provide services and improve our products and services. You may turn off the telemetry as described in the repository. There are also some features in the software that may enable you and Microsoft to collect data from users of your applications. If you use these features, you must comply with applicable law, including providing appropriate notices to users of your applications together with a copy of Microsoft's privacy statement. Our privacy statement is located at https://go.microsoft.com/fwlink/?LinkID=824704. You can learn more about data collection and use in the help documentation and our privacy statement. Your use of the software operates as your consent to these practices.

This data collection notice only applies to the process of **creating** a new React Native for Windows app with the CLI.